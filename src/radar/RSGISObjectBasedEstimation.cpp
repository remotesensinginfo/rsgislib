/*
 *  RSGISObjectBasedEstimation.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 25/11/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
 *
 *  RSGISLib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RSGISLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISObjectBasedEstimation.h"

namespace rsgis{namespace radar{

	RSGISObjectBasedEstimation::RSGISObjectBasedEstimation(GDALDataset *inputImage, GDALDataset *outputImage, GDALDataset *rasterFeatures, std::vector<gsl_vector*> *initialPar, std::vector<RSGISEstimationOptimiser*> *slowOptimiser, std::vector<RSGISEstimationOptimiser*> *fastOptimiser, estParameters parameters, double ***minMaxVals, std::string classHeading, bool useClass)
	{
		if(rasterFeatures == NULL)
        {
            this->datasetsIO = new GDALDataset*[2];
            this->datasetsInput = new GDALDataset*[1];
            this->datasetsIO[0] = inputImage;
            this->datasetsIO[1] = outputImage;
            this->datasetsInput[0] = inputImage;
            this->useRasPoly = false;
        }
        else
        {
            this->datasetsIO = new GDALDataset*[3];
            this->datasetsInput = new GDALDataset*[2];
            this->datasetsIO[0] = rasterFeatures;
            this->datasetsIO[1] = inputImage;
            this->datasetsIO[2] = outputImage;
            this->datasetsInput[0] = rasterFeatures;
            this->datasetsInput[1] = inputImage;
            this->useRasPoly = true;
        }

		this->slowOptimiser = slowOptimiser; // Initial optimiser, for selection of object.
		this->fastOptimiser = fastOptimiser; // Fast optimser, for entire object.
		this->numBands = inputImage->GetRasterCount();
		this->objectSamplesPercent = 0.1; // Percent of pixels in object to use with slow optimisers.
		this->parameters = parameters;
		this->initialPar = initialPar;
		this->classHeading = classHeading;
		this->useClass = useClass;
		this->minMaxVals = minMaxVals;
		this->useDefaultMinMax = true;

		if ((this->parameters == cDepthDensity) | (this->parameters == heightDensity)) // Set number of output parameters
		{
			this->numOutputPar = 2;
		}
		else if(this->parameters == dielectricDensityHeight)
		{
			this->numOutputPar = 3;
		}
		else
		{
			throw RSGISException("Parameters not recognised");
		}

		this->numOutputBands = numOutputPar + 2; // Extra bands for biomass and error
		this->pixelVals = new std::vector<float>*[this->numBands];

		// Calc image to get values (for initial inversion)
		this->getValues = new RSGISObjectBasedEstimationGetObjVals(this->pixelVals, this->numBands);
		this->calcImageSingle = new rsgis::img::RSGISCalcImageSingle(this->getValues);

		if (!useClass)
		{
			std::cout << "Not using parameters" << std::endl;
			this->slowOptimiserSingle = this->slowOptimiser->at(0);
			this->fastOptimiserSingle = this->fastOptimiser->at(0);
			this->initialParSingle = this->initialPar->at(0);
		}
		if (this->minMaxVals != NULL)
		{
			this->useDefaultMinMax = false; // If minimum and maximum values are passed in use these insead
            std::cout << "Not using default min/max values" << std::endl;
		}

		std::cout << "Parameters read in OK" << std::endl;
	}

	void RSGISObjectBasedEstimation::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		try
		{
			// GET DATA
			rsgis::vec::RSGISVectorUtils vecUtils;
			OGRPolygon *inOGRPoly;
			geos::geom::Polygon *poly;
			inOGRPoly = (OGRPolygon *) inFeature->GetGeometryRef();
			poly = vecUtils.convertOGRPolygon2GEOSPolygon(inOGRPoly);
			rsgis::img::RSGISCalcImageValue *invValuesObj;
			rsgis::img::RSGISCalcImageValue *invValues;

			getValues->reset();

            // Get values for pixels within polygon
            if(this->useRasPoly)
            {
                calcImageSingle->calcImageWithinRasterPolygon(this->datasetsInput, 2, NULL, env, fid, false);
            }
            else
            {
                calcImageSingle->calcImageWithinPolygon(this->datasetsInput, 1, NULL, env, poly, false, rsgis::img::polyContainsPixelCenter); // The pixel in poly method is hardcoded as 'polyContainsPixelCenter', no output is required
            }

			unsigned int estClass = 0;
			OGRFeatureDefn *inFeatureDefn = inFeature->GetDefnRef();

			if (useClass)
			{
				int fieldIdx = inFeatureDefn->GetFieldIndex(this->classHeading.c_str());
				estClass = inFeature->GetFieldAsInteger(fieldIdx) - 1;

				if (estClass >= this->slowOptimiser->size())
				{
					std::cout << "Class number greater than number classes parameterised for. Using last available class.\n";
					estClass = this->slowOptimiser->size() - 1;
				}

                this->slowOptimiserSingle = this->slowOptimiser->at(estClass);
				this->fastOptimiserSingle = this->fastOptimiser->at(estClass);
				this->initialParSingle = this->initialPar->at(estClass);

                if(this->slowOptimiserSingle->getOptimiserType() == rsgis::radar::noOptimiser)
                {
                    throw rsgis::RSGISException("Writing to a shapefile is not supported at the object level.");
                }
			}

			// OBTAIN AVERAGE FOR OBJECT AND PERFORM INVERSION
			if (this->useDefaultMinMax)
			{
				invValuesObj = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->slowOptimiserSingle, this->parameters);
			}
			else
			{
                invValuesObj = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->slowOptimiserSingle, this->parameters, this->minMaxVals[estClass]);
			}

			double *outData = new double[numOutputBands]; // Create array large enough to hold all output bands (more than parameters)
			float *inData = new float[this->numBands];
			bool *indB = new bool[this->numBands];
			bool *convertdB = new bool[this->numBands];

			for (int i = 0; i < this->numBands; i++)  // Loop through bands
			{
				inData[i] = 0;
				for(unsigned int r = 0; r < pixelVals[i]->size(); r++) // Loop through pixels in object
				{
					if (r == 0)
					{
						if ((pixelVals[i]->at(0) > 0))
						{
							indB[i] = false;
							if ((pixelVals[i]->at(0) < 1))
							{
								convertdB[i] = true;
							}
							else
							{
								convertdB[i] = false;
							}
						}
					}
					if (indB[i])
					{
						inData[i] = inData[i] + pow(10,(pixelVals[i]->at(r) / 10));
					}
					else
					{
						inData[i] = inData[i] + pixelVals[i]->at(r);
					}
				}
			}

			for (int i = 0; i < this->numBands; i++) // Obtain average and convert to dB
			{
				if (convertdB[i])
				{
					inData[i] = 10*log10(inData[i] / pixelVals[i]->size());
				}
				else
				{
					inData[i] = inData[i] / pixelVals[i]->size();
				}
			}

			invValuesObj->calcImageValue(inData, this->numBands, outData);

			// Get averages for each band.
			gsl_vector *localPar;
			localPar = gsl_vector_alloc(this->numOutputPar);
			for(unsigned int i = 0; i < this->numOutputPar; i++)
			{
				gsl_vector_set(localPar, i, outData[i]);
			}

			// SAVE PARAMETERS TO OUTPUT SHAPEFILE
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			if (this->parameters == heightDensity)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objHeight"), outData[0]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objDens"), outData[1]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objError"), outData[3]);
			}
			else if (this->parameters == cDepthDensity)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objCDepth"), outData[0]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objDens"), outData[1]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objError"), outData[3]);
			}
			else if(this->parameters == dielectricDensityHeight)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objHeight"), outData[0]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objDens"), outData[1]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objEps"), outData[2]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objError"), outData[4]);
			}

			// PARAMETERISE OPIMISER USING VALUES FROM DATA
			// Update optimser if error low enough
            if ( (outData[this->numOutputBands - 1] < 1e-8) && (outData[this->numOutputBands - 1] > 0))
            {
                this->fastOptimiserSingle->modifyAPriori(localPar);
                // RUN INVERISION ON ALL PIXELS IN OBJECT
                if (this->useDefaultMinMax)
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, localPar, this->fastOptimiserSingle, this->parameters);
                }
                else
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, localPar, this->fastOptimiserSingle, this->parameters, this->minMaxVals[estClass]);
                }
            }
            else
            {
                this->fastOptimiserSingle->modifyAPriori(this->initialParSingle);
                // RUN INVERISION ON ALL PIXELS IN OBJECT
                if (this->useDefaultMinMax)
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->fastOptimiserSingle, this->parameters);
                }
                else
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->fastOptimiserSingle, this->parameters, this->minMaxVals[estClass]);
                }
            }

            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(invValues, "", true);

            if(this->useRasPoly)
            {
                calcImage->calcImageWithinRasterPolygon(this->datasetsIO, 3, env, fid);
            }
            else
            {
                calcImage->calcImageWithinPolygon(this->datasetsIO, 2, env, poly, rsgis::img::polyContainsPixelCenter);
            }

			// TIDY
			gsl_vector_free(localPar);
			delete invValues;
			delete invValuesObj;
			delete calcImage;
			delete[] inData;
			delete[] outData;
			delete[] indB;
			delete[] convertdB;
		}
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}

	}

	void RSGISObjectBasedEstimation::processFeature(OGRFeature *inFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{

		try
		{
			// GET DATA
			rsgis::vec::RSGISVectorUtils vecUtils;
			OGRPolygon *inOGRPoly;
			geos::geom::Polygon *poly;
			inOGRPoly = (OGRPolygon *) inFeature->GetGeometryRef();
			poly = vecUtils.convertOGRPolygon2GEOSPolygon(inOGRPoly);
			rsgis::img::RSGISCalcImageValue *invValuesObj;
			rsgis::img::RSGISCalcImageValue *invValues;
            gsl_vector *localPar;
            localPar = gsl_vector_alloc(this->numOutputPar);
            double *outData = new double[numOutputBands]; // Create array large enough to hold all output bands (more than parameters)
            float *inData = new float[this->numBands];
            bool *indB = new bool[this->numBands];
            bool *convertdB = new bool[this->numBands];

			getValues->reset();

            // Get values for pixels within polygon
            if(this->useRasPoly)
            {
                calcImageSingle->calcImageWithinRasterPolygon(this->datasetsInput, 2, NULL, env, fid, false);
            }
            else
            {
                calcImageSingle->calcImageWithinPolygon(this->datasetsInput, 1, NULL, env, poly, false, rsgis::img::polyContainsPixelCenter); // The pixel in poly method is hardcoded as 'polyContainsPixelCenter', no output is required
            }


			unsigned int estClass = 0;
			OGRFeatureDefn *inFeatureDefn = inFeature->GetDefnRef();

			if (useClass)
			{
				int fieldIdx = inFeatureDefn->GetFieldIndex(this->classHeading.c_str());
				estClass = inFeature->GetFieldAsInteger(fieldIdx) - 1;

				if (estClass >= this->slowOptimiser->size())
				{
					std::cout << "Class number greater than number classes parameterised for. Using last available class.\n";
					estClass = this->slowOptimiser->size() - 1;
				}

                this->slowOptimiserSingle = this->slowOptimiser->at(estClass);
				this->fastOptimiserSingle = this->fastOptimiser->at(estClass);
				this->initialParSingle = this->initialPar->at(estClass);
			}

			// OBTAIN AVERAGE FOR OBJECT AND PERFORM INVERSION
            if(this->slowOptimiserSingle->getOptimiserType() != rsgis::radar::noOptimiser)
            {
                if (this->useDefaultMinMax)
                {
                    invValuesObj = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->slowOptimiserSingle, this->parameters);
                }
                else
                {
                    invValuesObj = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->slowOptimiserSingle, this->parameters, this->minMaxVals[estClass]);
                }

                for (int i = 0; i < this->numBands; i++)  // Loop through bands
                {
                    inData[i] = 0;
                    for(unsigned int r = 0; r < pixelVals[i]->size(); r++) // Loop through pixels in object
                    {
                        if (r == 0)
                        {
                            if ((pixelVals[i]->at(0) > 0))
                            {
                                indB[i] = false;
                                if ((pixelVals[i]->at(0) < 1))
                                {
                                    convertdB[i] = true;
                                }
                                else
                                {
                                    convertdB[i] = false;
                                }
                            }
                        }
                        if (indB[i])
                        {
                            inData[i] = inData[i] + pow(10,(pixelVals[i]->at(r) / 10));
                        }
                        else
                        {
                            inData[i] = inData[i] + pixelVals[i]->at(r);
                        }
                    }
                }

                for (int i = 0; i < this->numBands; i++) // Obtain average and convert to dB
                {
                    if (convertdB[i])
                    {
                        inData[i] = 10*log10(inData[i] / pixelVals[i]->size());
                    }
                    else
                    {
                        inData[i] = inData[i] / pixelVals[i]->size();
                    }

                }


                invValuesObj->calcImageValue(inData, this->numBands, outData);

                for(unsigned int i = 0; i < this->numOutputPar; i++)
                {
                    gsl_vector_set(localPar, i, outData[i]);
                }

                delete invValuesObj;
            }

			// PARAMETERISE OPIMISER USING VALUES FROM DATA
			// Update optimser if error low enough
            if ((outData[this->numOutputBands - 1] < 1e-8) && (this->slowOptimiserSingle->getOptimiserType() != rsgis::radar::noOptimiser))
            {
                this->fastOptimiserSingle->modifyAPriori(localPar);
                // RUN INVERISION ON ALL PIXELS IN OBJECT
                if (this->useDefaultMinMax)
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, localPar, this->fastOptimiserSingle, this->parameters);
                }
                else
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, localPar, this->fastOptimiserSingle, this->parameters, this->minMaxVals[estClass]);
                }
            }
            else
            {
                this->fastOptimiserSingle->modifyAPriori(this->initialParSingle);
                // RUN INVERISION ON ALL PIXELS IN OBJECT
                if (this->useDefaultMinMax)
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->fastOptimiserSingle, this->parameters);
                }
                else
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->fastOptimiserSingle, this->parameters, this->minMaxVals[estClass]);
                }
            }

            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(invValues, "", true);

            if(this->useRasPoly)
            {
                calcImage->calcImageWithinRasterPolygon(this->datasetsIO, 3, env, fid);
            }
            else
            {
                calcImage->calcImageWithinPolygon(this->datasetsIO, 2, env, poly, rsgis::img::polyContainsPixelCenter);
            }

			// TIDY
			gsl_vector_free(localPar);
			delete invValues;
			delete calcImage;
			delete[] inData;
			delete[] outData;
			delete[] indB;
			delete[] convertdB;
		}
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}
	}

	void RSGISObjectBasedEstimation::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(rsgis::vec::RSGISVectorOutputException)
	{
		if(this->parameters == heightDensity)
		{
			OGRFieldDefn shpField1("objHeight", OFTReal);
			shpField1.SetPrecision(10);
			if( outputLayer->CreateField( &shpField1 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objHeight\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField2("objDens", OFTReal);
			shpField2.SetPrecision(10);
			if( outputLayer->CreateField( &shpField2 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objHeight\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField3("objError", OFTReal);
			shpField3.SetPrecision(10);
			if( outputLayer->CreateField( &shpField3 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objError\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
		}
		else if(this->parameters == cDepthDensity)
		{
			OGRFieldDefn shpField1("objCDepth", OFTReal);
			shpField1.SetPrecision(10);
			if( outputLayer->CreateField( &shpField1 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objCDepth\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField2("objDens", OFTReal);
			shpField2.SetPrecision(10);
			if( outputLayer->CreateField( &shpField2 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objHeight\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField3("objError", OFTReal);
			shpField3.SetPrecision(10);
			if( outputLayer->CreateField( &shpField3 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objError\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
		}
		else if(this->parameters == dielectricDensityHeight)
		{
			OGRFieldDefn shpField1("objHeight", OFTReal);
			shpField1.SetPrecision(10);
			if( outputLayer->CreateField( &shpField1 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objHeight\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField2("objDens", OFTReal);
			shpField2.SetPrecision(10);
			if( outputLayer->CreateField( &shpField2 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objHeight\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField3("objEps", OFTReal);
			shpField3.SetPrecision(10);
			if( outputLayer->CreateField( &shpField3 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objEps\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField4("objError", OFTReal);
			shpField4.SetPrecision(10);
			if( outputLayer->CreateField( &shpField4 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objError\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
		}
		else
		{
			throw rsgis::vec::RSGISVectorOutputException("Input parameters not recognised, could not create fields\n");
		}
	}

	RSGISObjectBasedEstimation::~RSGISObjectBasedEstimation()
	{
        delete[] this->datasetsIO;
        delete[] this->datasetsInput;
        delete getValues;
		delete calcImageSingle;
		delete[] pixelVals;
	}

    RSGISObjectBasedEstimationObjectAP::RSGISObjectBasedEstimationObjectAP(GDALDataset *inputImage, GDALDataset *outputImage, GDALDataset *rasterFeatures, std::vector<gsl_vector*> *initialPar, std::vector<RSGISEstimationOptimiser*> *slowOptimiser, std::vector<RSGISEstimationOptimiser*> *fastOptimiser, estParameters parameters, std::string *apParField, double ***minMaxVals, std::string classHeading, bool useClass)
	{
		if(rasterFeatures == NULL)
        {
            this->datasetsIO = new GDALDataset*[2];
            this->datasetsInput = new GDALDataset*[1];
            this->datasetsIO[0] = inputImage;
            this->datasetsIO[1] = outputImage;
            this->datasetsInput[0] = inputImage;
            this->useRasPoly = false;
        }
        else
        {
            this->datasetsIO = new GDALDataset*[3];
            this->datasetsInput = new GDALDataset*[2];
            this->datasetsIO[0] = rasterFeatures;
            this->datasetsIO[1] = inputImage;
            this->datasetsIO[2] = outputImage;
            this->datasetsInput[0] = rasterFeatures;
            this->datasetsInput[1] = inputImage;
            this->useRasPoly = true;
        }
		this->slowOptimiser = slowOptimiser; // Initial optimiser, for selection of object.
		this->fastOptimiser = fastOptimiser; // Fast optimser, for entire object.
		this->numBands = inputImage->GetRasterCount();
		this->objectSamplesPercent = 0.1; // Percent of pixels in object to use with slow optimisers.
		this->parameters = parameters;
		this->initialPar = initialPar;
		this->classHeading = classHeading;
		this->useClass = useClass;
		this->minMaxVals = minMaxVals;
		this->useDefaultMinMax = true;
		this->apParField = apParField;
		if ((this->parameters == cDepthDensity) | (this->parameters == heightDensity)) // Set number of output parameters
		{
			this->numOutputPar = 2;
		}
		else if(this->parameters == dielectricDensityHeight)
		{
			this->numOutputPar = 3;
		}
		else
		{
			throw RSGISException("Parameters not recognised");
		}
		this->numOutputBands = numOutputPar + 2; // Extra bands for biomass and error
		this->pixelVals = new std::vector<float>*[numBands];

		// Calc image to get values (for initial inversion)
		this->getValues = new RSGISObjectBasedEstimationGetObjVals(pixelVals, numBands);
		this->calcImageSingle = new rsgis::img::RSGISCalcImageSingle(getValues);

		if (!useClass)
		{
			this->initialParSingle = this->initialPar->at(0);
		}
		if (this->minMaxVals != NULL)
		{
			this->useDefaultMinMax = false; // If minimum and maximum values are passed in use these insead
		}

	}

	void RSGISObjectBasedEstimationObjectAP::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		try
		{
			// GET DATA
			rsgis::vec::RSGISVectorUtils vecUtils;
			OGRPolygon *inOGRPoly;
			geos::geom::Polygon *poly;
			rsgis::img::RSGISCalcImageValue *invValuesObj;
			rsgis::img::RSGISCalcImageValue *invValues;

			inOGRPoly = (OGRPolygon *) inFeature->GetGeometryRef();
			poly = vecUtils.convertOGRPolygon2GEOSPolygon(inOGRPoly);
			getValues->reset();

            // Get values for pixels within polygon
            if(this->useRasPoly)
            {
                calcImageSingle->calcImageWithinRasterPolygon(this->datasetsInput, 2, NULL, env, fid, false);
            }
            else
            {
                calcImageSingle->calcImageWithinPolygon(this->datasetsInput, 1, NULL, env, poly, false, rsgis::img::polyContainsPixelCenter); // The pixel in poly method is hardcoded as 'polyContainsPixelCenter', no output is required
            }

			unsigned int estClass = 0;
			OGRFeatureDefn *inFeatureDefn = inFeature->GetDefnRef();

			if (useClass)
			{
				int fieldIdx = inFeatureDefn->GetFieldIndex(this->classHeading.c_str());
				estClass = inFeature->GetFieldAsInteger(fieldIdx) - 1;

				if (estClass >= this->slowOptimiser->size())
				{
					std::cout << "Class number greater than number classes parameterised for. Using last available class.\n";
					estClass = this->slowOptimiser->size() - 1;
				}
				this->slowOptimiserSingle = this->slowOptimiser->at(estClass);
				this->fastOptimiserSingle = this->fastOptimiser->at(estClass);
				this->initialParSingle = this->initialPar->at(estClass);

                if(this->slowOptimiserSingle->getOptimiserType() == rsgis::radar::noOptimiser)
                {
                    throw rsgis::RSGISException("Writing to a shapefile is not supported at the object level.");
                }
			}

			// MODIFY AP PARAMETERS BASED ON INPUT SHAPEFILE
			gsl_vector *objAP;
			objAP = gsl_vector_alloc(this->numOutputPar);

			for(unsigned int i = 0; i < this->numOutputPar; i++)
			{
				int fieldIdx = inFeatureDefn->GetFieldIndex(this->apParField[i].c_str());
				double apPar = inFeature->GetFieldAsDouble(fieldIdx);
				if (apPar != 0)
				{
					gsl_vector_set(objAP, i, apPar);
				}
				else // If values from object are zero, revert to general a priori estimates
				{
					gsl_vector_set(objAP, i, gsl_vector_get(this->initialParSingle, i));
				}

			}

			this->slowOptimiserSingle->modifyAPriori(objAP);

			// OBTAIN AVERAGE FOR OBJECT AND PERFORM INVERSION
			if (this->useDefaultMinMax)
			{
				invValuesObj = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->slowOptimiserSingle, this->parameters);
			}
			else
			{
				invValuesObj = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->slowOptimiserSingle, this->parameters, this->minMaxVals[estClass]);
			}

			double *outData = new double[numOutputBands]; // Create array large enough to hold all output bands (more than parameters)
			float *inData = new float[this->numBands];
			bool *indB = new bool[this->numBands];
			bool *convertdB = new bool[this->numBands];

			for (int i = 0; i < this->numBands; i++)  // Loop through bands
			{
				inData[i] = 0;
				for(unsigned int r = 0; r < pixelVals[i]->size(); r++) // Loop through pixels in object
				{
					if (r == 0)
					{
						if ((pixelVals[i]->at(0) > 0))
						{
							indB[i] = false;
							if ((pixelVals[i]->at(0) < 1))
							{
								convertdB[i] = true;
							}
							else
							{
								convertdB[i] = false;
							}
						}
					}
					if (indB[i])
					{
						inData[i] = inData[i] + pow(10,(pixelVals[i]->at(r) / 10));
					}
					else
					{
						inData[i] = inData[i] + pixelVals[i]->at(r);
					}
				}
			}

			for (int i = 0; i < this->numBands; i++) // Obtain average and convert to dB
			{
				if (convertdB[i])
				{
					inData[i] = 10*log10(inData[i] / pixelVals[i]->size());
				}
				else
				{
					inData[i] = inData[i] / pixelVals[i]->size();
				}
			}

			invValuesObj->calcImageValue(inData, this->numBands, outData);

			// Get averages for each band.
			gsl_vector *localPar;
			localPar = gsl_vector_alloc(this->numOutputPar);
			for(unsigned int i = 0; i < this->numOutputPar; i++)
			{
				gsl_vector_set(localPar, i, outData[i]);
			}

			// SAVE PARAMETERS TO OUTPUT SHAPEFILE
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			if (this->parameters == heightDensity)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objHeight"), outData[0]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objDens"), outData[1]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objError"), outData[3]);
			}
			else if (this->parameters == cDepthDensity)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objCDepth"), outData[0]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objDens"), outData[1]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objError"), outData[3]);
			}
			else if(this->parameters == dielectricDensityHeight)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objHeight"), outData[0]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objDens"), outData[1]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objEps"), outData[2]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex("objError"), outData[4]);
			}

			// PARAMETERISE OPIMISER USING VALUES FROM DATA
			// Update optimser
            if ( outData[this->numOutputBands - 1] < 1e-8)
            {
                this->fastOptimiserSingle->modifyAPriori(localPar);
                // RUN INVERISION ON ALL PIXELS IN OBJECT
                if (this->useDefaultMinMax)
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, localPar, this->fastOptimiserSingle, this->parameters);
                }
                else
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, localPar, this->fastOptimiserSingle, this->parameters, this->minMaxVals[estClass]);
                }
            }
            else
            {
                this->fastOptimiserSingle->modifyAPriori(this->initialParSingle);
                // RUN INVERISION ON ALL PIXELS IN OBJECT
                if (this->useDefaultMinMax)
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->fastOptimiserSingle, this->parameters);
                }
                else
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->fastOptimiserSingle, this->parameters, this->minMaxVals[estClass]);
                }
            }

			calcImage = new rsgis::img::RSGISCalcImage(invValues, "", true);

            if(this->useRasPoly)
            {
                calcImage->calcImageWithinRasterPolygon(this->datasetsIO, 3, env, fid);
            }
            else
            {
                calcImage->calcImageWithinPolygon(this->datasetsIO, 2, env, poly, rsgis::img::polyContainsPixelCenter);
            }


			// TIDY
			gsl_vector_free(localPar);
			gsl_vector_free(objAP);
			delete invValuesObj;
			delete invValues;
			delete calcImage;
			delete[] inData;
			delete[] outData;
		}
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}

	}

	void RSGISObjectBasedEstimationObjectAP::processFeature(OGRFeature *inFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		try
		{
			// GET DATA
			rsgis::vec::RSGISVectorUtils vecUtils;
			OGRPolygon *inOGRPoly;
			geos::geom::Polygon *poly;
			rsgis::img::RSGISCalcImageValue *invValuesObj;
			rsgis::img::RSGISCalcImageValue *invValues;

            double *outData = new double[numOutputBands]; // Create array large enough to hold all output bands (more than parameters)
            float *inData = new float[this->numBands];
            bool *indB = new bool[this->numBands];
            bool *convertdB = new bool[this->numBands];

            gsl_vector *localPar;
            localPar = gsl_vector_alloc(this->numOutputPar);

			inOGRPoly = (OGRPolygon *) inFeature->GetGeometryRef();
			poly = vecUtils.convertOGRPolygon2GEOSPolygon(inOGRPoly);
			getValues->reset();

            // Get values for pixels within polygon
            if(this->useRasPoly)
            {
                calcImageSingle->calcImageWithinRasterPolygon(this->datasetsInput, 2, NULL, env, fid, false);
            }
            else
            {
                calcImageSingle->calcImageWithinPolygon(this->datasetsInput, 1, NULL, env, poly, false, rsgis::img::polyContainsPixelCenter); // The pixel in poly method is hardcoded as 'polyContainsPixelCenter', no output is required
            }

			unsigned int estClass = 0;
			OGRFeatureDefn *inFeatureDefn = inFeature->GetDefnRef();


			if (useClass)
			{
				int fieldIdx = inFeatureDefn->GetFieldIndex(this->classHeading.c_str());
				estClass = inFeature->GetFieldAsInteger(fieldIdx) - 1;

				if (estClass >= this->slowOptimiser->size())
				{
					std::cout << "Class number greater than number classes parameterised for. Using last available class.\n";
					estClass = this->slowOptimiser->size() - 1;
				}
				this->slowOptimiserSingle = this->slowOptimiser->at(estClass);
				this->fastOptimiserSingle = this->fastOptimiser->at(estClass);
				this->initialParSingle = this->initialPar->at(estClass);
			}

			// MODIFY AP PARAMETERS BASED ON INPUT SHAPEFILE
			gsl_vector *objAP;
			objAP = gsl_vector_alloc(this->numOutputPar);

			for(unsigned int i = 0; i < this->numOutputPar; i++)
			{
				int fieldIdx = inFeatureDefn->GetFieldIndex(this->apParField[i].c_str());
				double apPar = inFeature->GetFieldAsDouble(fieldIdx);
				if (apPar != 0)
				{
					gsl_vector_set(objAP, i, apPar);
				}
				else // If values from object are zero, revert to general a priori estimates
				{
					gsl_vector_set(objAP, i, gsl_vector_get(this->initialParSingle, i));
				}

			}

            if(this->slowOptimiserSingle->getOptimiserType() != rsgis::radar::noOptimiser)
            {

                this->slowOptimiserSingle->modifyAPriori(objAP);

                // OBTAIN AVERAGE FOR OBJECT AND PERFORM INVERSION
                if (this->useDefaultMinMax)
                {
                    invValuesObj = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->slowOptimiserSingle, this->parameters);
                }
                else
                {
                    invValuesObj = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->slowOptimiserSingle, this->parameters, this->minMaxVals[estClass]);
                }


                for (int i = 0; i < this->numBands; i++)  // Loop through bands
                {
                    inData[i] = 0;
                    for(unsigned int r = 0; r < pixelVals[i]->size(); r++) // Loop through pixels in object
                    {
                        if (r == 0)
                        {
                            if ((pixelVals[i]->at(0) > 0))
                            {
                                indB[i] = false;
                                if ((pixelVals[i]->at(0) < 1))
                                {
                                    convertdB[i] = true;
                                }
                                else
                                {
                                    convertdB[i] = false;
                                }
                            }
                        }
                        if (indB[i])
                        {
                            inData[i] = inData[i] + pow(10,(pixelVals[i]->at(r) / 10));
                        }
                        else
                        {
                            inData[i] = inData[i] + pixelVals[i]->at(r);
                        }
                    }
                }

                for (int i = 0; i < this->numBands; i++) // Obtain average and convert to dB
                {
                    if (convertdB[i])
                    {
                        inData[i] = 10*log10(inData[i] / pixelVals[i]->size());
                    }
                    else
                    {
                        inData[i] = inData[i] / pixelVals[i]->size();
                    }
                }

                invValuesObj->calcImageValue(inData, this->numBands, outData);

                // Get averages for each band.
                for(unsigned int i = 0; i < this->numOutputPar; i++)
                {
                    gsl_vector_set(localPar, i, outData[i]);
                }


            }
            else
            {
                this->fastOptimiserSingle->modifyAPriori(objAP);
            }
			// PARAMETERISE OPIMISER USING VALUES FROM DATA
			// Update optimser
            if ((outData[this->numOutputBands - 1] < 1e-8) && (this->slowOptimiserSingle->getOptimiserType() != rsgis::radar::noOptimiser))
            {
                this->fastOptimiserSingle->modifyAPriori(localPar);
                // RUN INVERISION ON ALL PIXELS IN OBJECT
                if (this->useDefaultMinMax)
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, localPar, this->fastOptimiserSingle, this->parameters);
                }
                else
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, localPar, this->fastOptimiserSingle, this->parameters, this->minMaxVals[estClass]);
                }
            }
            else
            {
                this->fastOptimiserSingle->modifyAPriori(this->initialParSingle);
                // RUN INVERISION ON ALL PIXELS IN OBJECT
                if (this->useDefaultMinMax)
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->fastOptimiserSingle, this->parameters);
                }
                else
                {
                    invValues = new RSGISEstimationAlgorithmSingleSpecies(this->numOutputBands, this->initialParSingle, this->fastOptimiserSingle, this->parameters, this->minMaxVals[estClass]);
                }
            }


			calcImage = new rsgis::img::RSGISCalcImage(invValues, "", true);

            if(this->useRasPoly)
            {
                calcImage->calcImageWithinRasterPolygon(this->datasetsIO, 3, env, fid);
            }
            else
            {
                calcImage->calcImageWithinPolygon(this->datasetsIO, 2, env, poly, rsgis::img::polyContainsPixelCenter);
            }

			// TIDY
			gsl_vector_free(localPar);
			gsl_vector_free(objAP);
			delete invValues;
			delete calcImage;
			delete[] inData;
			delete[] outData;
		}
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}

	}

	void RSGISObjectBasedEstimationObjectAP::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(rsgis::vec::RSGISVectorOutputException)
	{
		if(this->parameters == heightDensity)
		{
			OGRFieldDefn shpField1("objHeight", OFTReal);
			shpField1.SetPrecision(10);
			if( outputLayer->CreateField( &shpField1 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objHeight\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField2("objDens", OFTReal);
			shpField2.SetPrecision(10);
			if( outputLayer->CreateField( &shpField2 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objHeight\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField3("objError", OFTReal);
			shpField3.SetPrecision(10);
			if( outputLayer->CreateField( &shpField3 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objError\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
		}
		else if(this->parameters == cDepthDensity)
		{
			OGRFieldDefn shpField1("objCDepth", OFTReal);
			shpField1.SetPrecision(10);
			if( outputLayer->CreateField( &shpField1 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objCDepth\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField2("objDens", OFTReal);
			shpField2.SetPrecision(10);
			if( outputLayer->CreateField( &shpField2 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objHeight\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField3("objError", OFTReal);
			shpField3.SetPrecision(10);
			if( outputLayer->CreateField( &shpField3 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objError\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
		}
		else if(this->parameters == dielectricDensityHeight)
		{
			OGRFieldDefn shpField1("objHeight", OFTReal);
			shpField1.SetPrecision(10);
			if( outputLayer->CreateField( &shpField1 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objHeight\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField2("objDens", OFTReal);
			shpField2.SetPrecision(10);
			if( outputLayer->CreateField( &shpField2 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objHeight\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField3("objEps", OFTReal);
			shpField3.SetPrecision(10);
			if( outputLayer->CreateField( &shpField3 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objEps\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
			OGRFieldDefn shpField4("objError", OFTReal);
			shpField4.SetPrecision(10);
			if( outputLayer->CreateField( &shpField4 ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'objError\' has failed");
				throw rsgis::vec::RSGISVectorOutputException(message.c_str());
			}
		}
		else
		{
			throw rsgis::vec::RSGISVectorOutputException("Input parameters not recognised, could not create fields\n");
		}
	}

	RSGISObjectBasedEstimationObjectAP::~RSGISObjectBasedEstimationObjectAP()
	{
        delete[] this->datasetsIO;
        delete[] this->datasetsInput;
        delete getValues;
		delete calcImageSingle;
		delete[] pixelVals;
	}

	RSGISObjectBasedEstimationGetObjVals::RSGISObjectBasedEstimationGetObjVals(std::vector<float> **pixelVals, int numBands) : rsgis::img::RSGISCalcImageSingleValue(numBands)
	{
		this->pixelVals = pixelVals;
		this->numInBands = numBands;

		for(int i = 0; i < numBands; i++)
		{
			this->pixelVals[i] = new std::vector<float>();
        }
	}

	void RSGISObjectBasedEstimationGetObjVals::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(rsgis::img::RSGISImageCalcException)
	{
		for(int i = 0; i < this->numInBands; i++) // Loop through bands
		{
			if ((boost::math::isnan(bandValuesImage[i]) == false) && (bandValuesImage[i] != 0.0) && (bandValuesImage[i] > -100))
			{
				this->pixelVals[i]->push_back(bandValuesImage[i]);
			}
		}
	}

	void RSGISObjectBasedEstimationGetObjVals::calcImageValue(float *bandValuesImage, int numBands, int band) throw(rsgis::img::RSGISImageCalcException)
	{
		for(int i = 1; i < this->numInBands + 1;  i++) // Loop through bands
		{
			if ((boost::math::isnan(bandValuesImage[i]) == false) && (bandValuesImage[i] != 0.0) && (bandValuesImage[i] > -100))
			{
				this->pixelVals[i-1]->push_back(bandValuesImage[i]);
			}
		}
	}

	double* RSGISObjectBasedEstimationGetObjVals::getOutputValues() throw(rsgis::img::RSGISImageCalcException)
	{
		return NULL;
	}

	void RSGISObjectBasedEstimationGetObjVals::reset()
	{
		for(int i = 0; i < this->numInBands; i++)
		{
			this->pixelVals[i]->clear();
		}
	}

	RSGISObjectBasedEstimationGetObjVals::~RSGISObjectBasedEstimationGetObjVals()
	{
		for(int i = 0; i < this->numInBands; i++)
		{
			pixelVals[i]->clear();
		}
	}

	int RSGISEstimationAssignAP::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		for (unsigned int i = 0; i < initialPar->size; ++i)
		{
			gsl_vector_set(outParError, i, gsl_vector_get(initialPar, i));
		}
		gsl_vector_set(outParError, initialPar->size, 9999);
		return 0;
	}


}}



