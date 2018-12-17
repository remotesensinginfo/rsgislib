/*
 *  RSGISZonalStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/04/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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

#include "RSGISZonalStats.h"

namespace rsgis{namespace vec{
	
	ZonalStats::ZonalStats()
	{
		
	}
	
	void ZonalStats::zonalStats(GDALDataset *image, OGRLayer *vector, bool **toCalc,  OGRLayer *outputSHPLayer)  throw(RSGISVectorZonalException, RSGISVectorOutputException)
	{
		this->createOutputSHPDefinition(vector, outputSHPLayer, toCalc, image->GetRasterCount());
		OGRPolygon *polygon = NULL;
		OGRGeometry *polyGeometry = NULL;
		OGRFeature *polyFeature = NULL;
		OGRFeatureDefn *featDefn = vector->GetLayerDefn();
		int fieldCount = featDefn->GetFieldCount();
		int numFeatures = vector->GetFeatureCount();
		std::cout << "Number of features = " << numFeatures << std::endl;
		
		imagestats **featureStats = new imagestats*[numFeatures];
		for(int i = 0; i < numFeatures; i++)
		{
			featureStats[i] = new imagestats[image->GetRasterCount()];
		}
		
		int feedbackGap = 10;
		if(numFeatures > 5000)
		{
			feedbackGap = 100;
		}
		
		//int feedback = numFeatures/feedbackGap;
		int counter = 0;
		vector->ResetReading();
		while( (polyFeature = vector->GetNextFeature()) != NULL )
		{
			std::cout << "Calculation of stats for " << counter << " of " << numFeatures << " have been completed\n";
			polyGeometry = polyFeature->GetGeometryRef();
			if( polyGeometry != NULL && wkbFlatten(polyGeometry->getGeometryType()) == wkbPolygon )
			{
				std::cout << "Found polygon - Now casting\n";
				polygon = (OGRPolygon *) polyGeometry;
				std::cout << "Casted Polygon\n";
				std::cout << "Polygon: " << polygon << std::endl;
				this->calcImageStats(image, polygon, featureStats[counter]);
			} 
			else if( polyGeometry != NULL && wkbFlatten(polyGeometry->getGeometryType()) == wkbMultiPolygon )
			{
				std::cout << "Ignoring feature as multi polygon\n";
			}
			OGRFeature::DestroyFeature( polyFeature );
			counter++;
		}
		
		this->outputData2SHP(vector, outputSHPLayer, fieldCount, toCalc, image->GetRasterCount(), featureStats);
	}
	
	void ZonalStats::zonalStatsVector(GDALDataset *image, OGRLayer *vector, bool **toCalc,  OGRLayer *outputSHPLayer) throw(RSGISVectorZonalException,RSGISVectorOutputException)
	{
		int numAttributes = image->GetRasterCount();
		int numFeatures = vector->GetFeatureCount();
		RSGISPolygonData **data = new RSGISPolygonData*[numFeatures];
		for(int i = 0; i < numFeatures; i++)
		{
			data[i] = new RSGISZonalPolygons(numAttributes, toCalc);
		}
		
		RSGISVectorIO *vecIO = new RSGISVectorIO();
		vecIO->readPolygons(vector, data, numFeatures);
		RSGISZonalPolygons *poly = NULL;
		
		for(int i = 0; i < numFeatures; i++)
		{
			std::cout << "Polygon " << (i+1) << " of " << numFeatures << std::endl;
			//data[i]->printGeometry();
			poly = (RSGISZonalPolygons *) data[i];
			this->calcImageStats(image, poly);
		}
		
		vecIO->exportPolygons2Shp(outputSHPLayer, data, numFeatures);
	}
	
	void ZonalStats::zonalStatsRaster(GDALDataset *image, GDALDataset *rasterFeatures, OGRLayer *inputLayer, OGRLayer *outputLayer, bool **toCalc) throw(rsgis::img::RSGISImageCalcException, rsgis::img::RSGISImageBandException, RSGISVectorOutputException)
	{
		int numFeatures = inputLayer->GetFeatureCount();
		int numAttributes = image->GetRasterCount();
		rsgis::img::RSGISCalcImage *calcImg = NULL;
		RSGISCalcZonalStatsFromRaster *rasterZonalCalc = NULL;
		imagestats **featureStats = NULL;
		OGRFeatureDefn *featDefn = NULL;
		
		try
		{
			this->createOutputSHPDefinition(inputLayer, outputLayer, toCalc, numAttributes);
			
			featureStats = new imagestats*[numFeatures];
			for(int i = 0; i < numFeatures; i++)
			{
				featureStats[i] = new imagestats[numAttributes];
				for(int j = 0; j < numAttributes; j++)
				{
					featureStats[i][j].meanSum = 0;
					featureStats[i][j].n = 0;
					featureStats[i][j].sumDiff = 0;
					featureStats[i][j].first = true;
				}
			}
			
			int numDS = 2;
			GDALDataset **datasets = new GDALDataset*[numDS];
			datasets[0] = rasterFeatures;
			datasets[1] = image;
			
			rasterZonalCalc = new RSGISCalcZonalStatsFromRaster(0, featureStats, numAttributes, numFeatures, false);
			calcImg = new rsgis::img::RSGISCalcImage(rasterZonalCalc, "", true);
			calcImg->calcImage(datasets, numDS);
			for(int i = 0; i < numFeatures; i++)
			{
				for(int j = 0; j < numAttributes; j++)
				{
					featureStats[i][j].mean = featureStats[i][j].meanSum/featureStats[i][j].n;
				}
			}
			rasterZonalCalc->setStdDev(true);
			calcImg->calcImage(datasets, numDS);
			for(int i = 0; i < numFeatures; i++)
			{
				for(int j = 0; j < numAttributes; j++)
				{
					featureStats[i][j].stddev = sqrt(featureStats[i][j].sumDiff/featureStats[i][j].n);
				}
			}
			delete[] datasets;
			featDefn = inputLayer->GetLayerDefn();
			int fieldCount = featDefn->GetFieldCount();
			this->outputData2SHP(inputLayer, outputLayer, fieldCount, toCalc, numAttributes, featureStats);
		}
		catch(rsgis::img::RSGISImageCalcException e)
		{
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			if(rasterZonalCalc != NULL)
			{
				delete rasterZonalCalc;
			}
			if(featDefn != NULL)
			{
				delete featDefn;
			}
			if(featureStats != NULL)
			{
				featureStats = new imagestats*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					delete[] featureStats[i];
				}
				delete[] featureStats;
			}
			throw e;
		}
		catch(rsgis::img::RSGISImageBandException e)
		{
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			if(rasterZonalCalc != NULL)
			{
				delete rasterZonalCalc;
			}
			if(featDefn != NULL)
			{
				delete featDefn;
			}
			if(featureStats != NULL)
			{
				featureStats = new imagestats*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					delete[] featureStats[i];
				}
				delete[] featureStats;
			}
			throw e;
		}
		catch(RSGISVectorOutputException e)
		{
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			if(rasterZonalCalc != NULL)
			{
				delete rasterZonalCalc;
			}
			if(featDefn != NULL)
			{
				delete featDefn;
			}
			if(featureStats != NULL)
			{
				featureStats = new imagestats*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					delete[] featureStats[i];
				}
				delete[] featureStats;
			}
			throw e;
		}
		
		if(calcImg != NULL)
		{
			delete calcImg;
		}
		if(rasterZonalCalc != NULL)
		{
			delete rasterZonalCalc;
		}
		if(featDefn != NULL)
		{
			delete featDefn;
		}
		if(featureStats != NULL)
		{
			featureStats = new imagestats*[numFeatures];
			for(int i = 0; i < numFeatures; i++)
			{
				delete[] featureStats[i];
			}
			delete[] featureStats;
		}
	}
		
	void ZonalStats::zonalStatsRaster2txt(GDALDataset *image, GDALDataset *rasterFeatures, OGRLayer *inputLayer, std::string outputTxt, bool **toCalc) throw(rsgis::img::RSGISImageCalcException, rsgis::img::RSGISImageBandException)
	{
		int numFeatures = inputLayer->GetFeatureCount();
		int numAttributes = image->GetRasterCount();
		
		std::cout << "Input vector has " << numFeatures << " features.\n";
		std::cout << "Image has " << numAttributes << " bands (output attributes)\n";
		
		rsgis::img::RSGISCalcImage *calcImg = NULL;
		RSGISCalcZonalStatsFromRaster *rasterZonalCalc = NULL;
		imagestats **featureStats = NULL;
		
		try
		{
			featureStats = new imagestats*[numFeatures];
			for(int i = 0; i < numFeatures; i++)
			{
				featureStats[i] = new imagestats[numAttributes];
				for(int j = 0; j < numAttributes; j++)
				{
					featureStats[i][j].meanSum = 0;
					featureStats[i][j].n = 0;
					featureStats[i][j].sumDiff = 0;
					featureStats[i][j].first = true;
				}
			}
			
			int numDS = 2;
			GDALDataset **datasets = new GDALDataset*[numDS];
			datasets[0] = rasterFeatures;
			datasets[1] = image;
			
			rasterZonalCalc = new RSGISCalcZonalStatsFromRaster(0, featureStats, numAttributes, numFeatures, false);
			calcImg = new rsgis::img::RSGISCalcImage(rasterZonalCalc, "", true);
			std::cout << "Calc stats part 1\n";
			calcImg->calcImage(datasets, numDS);
			for(int i = 0; i < numFeatures; i++)
			{
				for(int j = 0; j < numAttributes; j++)
				{
					featureStats[i][j].mean = featureStats[i][j].meanSum/featureStats[i][j].n;
				}
			}
			delete[] datasets;
			this->outputData2Text(outputTxt, toCalc, featureStats, numFeatures, numAttributes);
		}
		catch(rsgis::img::RSGISImageCalcException e)
		{
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			if(rasterZonalCalc != NULL)
			{
				delete rasterZonalCalc;
			}
			if(featureStats != NULL)
			{
				featureStats = new imagestats*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					delete[] featureStats[i];
				}
				delete[] featureStats;
			}
			throw e;
		}
		catch(rsgis::img::RSGISImageBandException e)
		{
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			if(rasterZonalCalc != NULL)
			{
				delete rasterZonalCalc;
			}
			if(featureStats != NULL)
			{
				featureStats = new imagestats*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					delete[] featureStats[i];
				}
				delete[] featureStats;
			}
			throw e;
		}
		
		if(calcImg != NULL)
		{
			delete calcImg;
		}
		if(rasterZonalCalc != NULL)
		{
			delete rasterZonalCalc;
		}
		if(featureStats != NULL)
		{
			featureStats = new imagestats*[numFeatures];
			for(int i = 0; i < numFeatures; i++)
			{
				delete[] featureStats[i];
			}
			delete[] featureStats;
		}
	}

    void ZonalStats::zonalStatsFeatsVectorLyr(GDALDataset *image, OGRLayer *vecLyr, std::vector<ZonalBandAttrs> *zonalBandAtts, rsgis::img::pixelInPolyOption pixelInPolyMethod) throw(rsgis::img::RSGISImageCalcException, rsgis::img::RSGISImageBandException)
    {
        try
        {
            // Define the output fields within vector layer.
            this->addVecLyrDefn(vecLyr, zonalBandAtts);
            RSGISZonalStatsPolyUpdateLyr *computeStats = new RSGISZonalStatsPolyUpdateLyr(image, zonalBandAtts, pixelInPolyMethod);
            RSGISProcessVector processVec = RSGISProcessVector(computeStats);

            long nFeats = vecLyr->GetFeatureCount();
            bool moreFeedback = false;
            if(nFeats > 20000)
            {
                moreFeedback = true;
            }
            processVec.processVectors(vecLyr, false, moreFeedback);
            delete computeStats;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }

    void ZonalStats::addVecLyrDefn(OGRLayer *vecLyr, std::vector<ZonalBandAttrs> *zonalBandAtts) throw(RSGISVectorOutputException)
    {
        try
        {
            int fCount = 0;
            bool found = false;
            OGRFeatureDefn *lyrDefn = vecLyr->GetLayerDefn();
            for(std::vector<ZonalBandAttrs>::iterator iterAtts = zonalBandAtts->begin(); iterAtts != zonalBandAtts->end(); ++iterAtts)
            {
                // Min
                if((*iterAtts).outMin)
                {
                    found = false;
                    fCount = lyrDefn->GetFieldCount();
                    for(int i = 0; i < fCount; ++i)
                    {
                        if(boost::algorithm::to_lower_copy(std::string(lyrDefn->GetFieldDefn(i)->GetNameRef())) == boost::algorithm::to_lower_copy((*iterAtts).minName))
                        {
                            found = true;
                            break;
                        }
                    }
                    if(!found)
                    {
                        OGRFieldDefn vecField((*iterAtts).minName.c_str(), OFTReal);
                        vecField.SetPrecision(10);
                        if( vecLyr->CreateField( &vecField ) != OGRERR_NONE )
                        {
                            std::string message = std::string("Creating vector field '") + std::string((*iterAtts).minName) + std::string("' has failed");
                            throw RSGISVectorOutputException(message.c_str());
                        }
                    }
                }
                // Max
                if((*iterAtts).outMax)
                {
                    found = false;
                    fCount = lyrDefn->GetFieldCount();
                    for(int i = 0; i < fCount; ++i)
                    {
                        if(boost::algorithm::to_lower_copy(std::string(lyrDefn->GetFieldDefn(i)->GetNameRef())) == boost::algorithm::to_lower_copy((*iterAtts).maxName))
                        {
                            found = true;
                            break;
                        }
                    }
                    if(!found)
                    {
                        OGRFieldDefn vecField((*iterAtts).maxName.c_str(), OFTReal);
                        vecField.SetPrecision(10);
                        if( vecLyr->CreateField( &vecField ) != OGRERR_NONE )
                        {
                            std::string message = std::string("Creating vector field '") + std::string((*iterAtts).maxName) + std::string("' has failed");
                            throw RSGISVectorOutputException(message.c_str());
                        }
                    }
                }
                // Mean
                if((*iterAtts).outMean)
                {
                    found = false;
                    fCount = lyrDefn->GetFieldCount();
                    for(int i = 0; i < fCount; ++i)
                    {
                        if(boost::algorithm::to_lower_copy(std::string(lyrDefn->GetFieldDefn(i)->GetNameRef())) == boost::algorithm::to_lower_copy((*iterAtts).meanName))
                        {
                            found = true;
                            break;
                        }
                    }
                    if(!found)
                    {
                        OGRFieldDefn vecField((*iterAtts).meanName.c_str(), OFTReal);
                        vecField.SetPrecision(10);
                        if( vecLyr->CreateField( &vecField ) != OGRERR_NONE )
                        {
                            std::string message = std::string("Creating vector field '") + std::string((*iterAtts).meanName) + std::string("' has failed");
                            throw RSGISVectorOutputException(message.c_str());
                        }
                    }
                }
                // StdDev
                if((*iterAtts).outStDev)
                {
                    found = false;
                    fCount = lyrDefn->GetFieldCount();
                    for(int i = 0; i < fCount; ++i)
                    {
                        if(boost::algorithm::to_lower_copy(std::string(lyrDefn->GetFieldDefn(i)->GetNameRef())) == boost::algorithm::to_lower_copy((*iterAtts).stdName))
                        {
                            found = true;
                            break;
                        }
                    }
                    if(!found)
                    {
                        OGRFieldDefn vecField((*iterAtts).stdName.c_str(), OFTReal);
                        vecField.SetPrecision(10);
                        if( vecLyr->CreateField( &vecField ) != OGRERR_NONE )
                        {
                            std::string message = std::string("Creating vector field '") + std::string((*iterAtts).stdName) + std::string("' has failed");
                            throw RSGISVectorOutputException(message.c_str());
                        }
                    }
                }
                // Count
                if((*iterAtts).outCount)
                {
                    found = false;
                    fCount = lyrDefn->GetFieldCount();
                    for(int i = 0; i < fCount; ++i)
                    {
                        if(boost::algorithm::to_lower_copy(std::string(lyrDefn->GetFieldDefn(i)->GetNameRef())) == boost::algorithm::to_lower_copy((*iterAtts).countName))
                        {
                            found = true;
                            break;
                        }
                    }
                    if(!found)
                    {
                        OGRFieldDefn vecField((*iterAtts).countName.c_str(), OFTReal);
                        vecField.SetPrecision(10);
                        if( vecLyr->CreateField( &vecField ) != OGRERR_NONE )
                        {
                            std::string message = std::string("Creating vector field '") + std::string((*iterAtts).countName) + std::string("' has failed");
                            throw RSGISVectorOutputException(message.c_str());
                        }
                    }
                }
                // Mode
                if((*iterAtts).outMode)
                {
                    found = false;
                    fCount = lyrDefn->GetFieldCount();
                    for(int i = 0; i < fCount; ++i)
                    {
                        if(boost::algorithm::to_lower_copy(std::string(lyrDefn->GetFieldDefn(i)->GetNameRef())) == boost::algorithm::to_lower_copy((*iterAtts).modeName))
                        {
                            found = true;
                            break;
                        }
                    }
                    if(!found)
                    {
                        OGRFieldDefn vecField((*iterAtts).modeName.c_str(), OFTReal);
                        vecField.SetPrecision(10);
                        if( vecLyr->CreateField( &vecField ) != OGRERR_NONE )
                        {
                            std::string message = std::string("Creating vector field '") + std::string((*iterAtts).modeName) + std::string("' has failed");
                            throw RSGISVectorOutputException(message.c_str());
                        }
                    }
                }
                // Median
                if((*iterAtts).outMedian)
                {
                    found = false;
                    fCount = lyrDefn->GetFieldCount();
                    for(int i = 0; i < fCount; ++i)
                    {
                        if(boost::algorithm::to_lower_copy(std::string(lyrDefn->GetFieldDefn(i)->GetNameRef())) == boost::algorithm::to_lower_copy((*iterAtts).medianName))
                        {
                            found = true;
                            break;
                        }
                    }
                    if(!found)
                    {
                        OGRFieldDefn vecField((*iterAtts).medianName.c_str(), OFTReal);
                        vecField.SetPrecision(10);
                        if( vecLyr->CreateField( &vecField ) != OGRERR_NONE )
                        {
                            std::string message = std::string("Creating vector field '") + std::string((*iterAtts).medianName) + std::string("' has failed");
                            throw RSGISVectorOutputException(message.c_str());
                        }
                    }
                }
                // Sum
                if((*iterAtts).outSum)
                {
                    found = false;
                    fCount = lyrDefn->GetFieldCount();
                    for(int i = 0; i < fCount; ++i)
                    {
                        if(boost::algorithm::to_lower_copy(std::string(lyrDefn->GetFieldDefn(i)->GetNameRef())) == boost::algorithm::to_lower_copy((*iterAtts).sumName))
                        {
                            found = true;
                            break;
                        }
                    }
                    if(!found)
                    {
                        OGRFieldDefn vecField((*iterAtts).sumName.c_str(), OFTReal);
                        vecField.SetPrecision(10);
                        if( vecLyr->CreateField( &vecField ) != OGRERR_NONE )
                        {
                            std::string message = std::string("Creating vector field '") + std::string((*iterAtts).sumName) + std::string("' has failed");
                            throw RSGISVectorOutputException(message.c_str());
                        }
                    }
                }
            }
        }
        catch(RSGISVectorOutputException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw RSGISVectorOutputException(e.what());
        }
    }
	
	void ZonalStats::createOutputSHPDefinition(OGRLayer *inputSHPLayer, OGRLayer *outputSHPLayer, bool **toCalc, int numBands) throw(RSGISVectorOutputException)
	{
		////////////////////////////////////////////
		//
		// Copy shapefile def
		//
		////////////////////////////////////////////
		OGRFeatureDefn *featDefn = inputSHPLayer->GetLayerDefn();
		int fieldCount = featDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			if( outputSHPLayer->CreateField( featDefn->GetFieldDefn(i) ) != OGRERR_NONE )
			{
				std::string errorMessage = std::string("Creating ") + std::string(featDefn->GetFieldDefn(i)->GetNameRef()) + std::string(" field failed.");
				throw RSGISVectorOutputException(errorMessage.c_str());
			}
		}
		
		////////////////////////////////////////////
		//
		// Add additional columns
		//
		////////////////////////////////////////////
		
		for(int i = 0; i < numBands; i++)
		{
			if(toCalc[i][0])
			{
				char colname[8];
				sprintf(colname, "mean_%d", i);
				OGRFieldDefn shpField(colname, OFTReal);
				shpField.SetPrecision(10);
				if( outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field") + std::string(colname) + std::string(" has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
			if(toCalc[i][1])
			{
				char colname[7];
				sprintf(colname, "max_%d", i);
				OGRFieldDefn shpField(colname, OFTReal);
				shpField.SetPrecision(10);
				if( outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field") + std::string(colname) + std::string(" has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
			if(toCalc[i][2])
			{
				char colname[7];
				sprintf(colname, "min_%d", i);
				OGRFieldDefn shpField(colname, OFTReal);
				shpField.SetPrecision(10);
				if( outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field") + std::string(colname) + std::string(" has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
			if(toCalc[i][3])
			{
				char colname[10];
				sprintf(colname, "stddev_%d", i);
				OGRFieldDefn shpField(colname, OFTReal);
				shpField.SetPrecision(10);
				if( outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field") + std::string(colname) + std::string(" has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
		}
	}
	
	void ZonalStats::createOutputSHPDefinition(OGRLayer *outputSHPLayer, classzonalstats** attributes, int numAttributes, OGRFeatureDefn *inLayerDef) throw(RSGISVectorOutputException)
	{
		int fieldCount = inLayerDef->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			if( outputSHPLayer->CreateField( inLayerDef->GetFieldDefn(i) ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating ") + std::string(inLayerDef->GetFieldDefn(i)->GetNameRef()) + std::string(" field failed.");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
		
		for(int i = 0; i < numAttributes; i++)
		{
			if(attributes[i]->name.length() > 10)
			{
				std::cout << attributes[i]->name << " will be truncated to \'" << attributes[i]->name.substr(0, 10) << "\'\n";
			}
			OGRFieldDefn shpField(attributes[i]->name.c_str(), OFTReal);
			shpField.SetPrecision(10);
			if( outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field ") + attributes[i]->name + std::string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
		
		OGRFieldDefn shpField("NumPxls", OFTReal);
		shpField.SetPrecision(10);
		if( outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
		{
			std::string message = std::string("Creating shapefile field \'NumPxls\' has failed");
			throw RSGISVectorOutputException(message.c_str());
		}
	}
	
	void ZonalStats::outputData2SHP(OGRLayer *inputLayer, OGRLayer *outputSHPLayer, int featureFieldCount, bool **toCalc, int numBands, imagestats **stats) throw(RSGISVectorOutputException)
	{
		OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
		OGRFeature *featureOutput = NULL;
		OGRFeature *feature = NULL;
		OGRGeometry *geometry = NULL;
		OGRPolygon *polygon = NULL;
		//OGRMultiPolygon *mPolygon = NULL;		
		int numFeatures = inputLayer->GetFeatureCount();
		int feedback = numFeatures/10;
		int counter = 0;
		inputLayer->ResetReading();
		while( (feature = inputLayer->GetNextFeature()) != NULL )
		{
			featureOutput = OGRFeature::CreateFeature(outputDefn);
			if(counter % feedback == 0)
			{
				std::cout << "Outputted " << counter << " of " << numFeatures << " features\n";
			}
			geometry = feature->GetGeometryRef();
			if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
			{
				polygon = (OGRPolygon *) geometry;
				featureOutput->SetGeometry(polygon);
			}
			
			////////////////////////////////////////////
			//
			// Copy shapefile data
			//
			////////////////////////////////////////////
			for(int i = 0; i < featureFieldCount; i++)
			{
				OGRFieldDefn *fieldDef = feature->GetFieldDefnRef(i);
				if(fieldDef->GetType() == OFTString)
				{
					featureOutput->SetField(i, feature->GetFieldAsString(i));
				}
				else if(fieldDef->GetType() == OFTReal)
				{
					featureOutput->SetField(i, feature->GetFieldAsString(i));
				}
			}
			
			////////////////////////////////////////////
			//
			// Add additional columns
			//
			////////////////////////////////////////////
			
			for(int i = 0; i < numBands; i++)
			{
				if(toCalc[i][0])
				{
					char colname[8];
					sprintf(colname, "mean_%d", i);
					featureOutput->SetField(outputDefn->GetFieldIndex(colname), stats[counter][i].mean);
				}
				if(toCalc[i][1])
				{
					char colname[7];
					sprintf(colname, "max_%d", i);
					featureOutput->SetField(outputDefn->GetFieldIndex(colname), stats[counter][i].max);
				}
				if(toCalc[i][2])
				{
					char colname[7];
					sprintf(colname, "min_%d", i);
					featureOutput->SetField(outputDefn->GetFieldIndex(colname), stats[counter][i].min);
				}
				if(toCalc[i][3])
				{
					char colname[10];
					sprintf(colname, "stddev_%d", i);
					featureOutput->SetField(outputDefn->GetFieldIndex(colname), stats[counter][i].stddev);
				}
			}
			
			if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
			}
			OGRFeature::DestroyFeature(featureOutput);
			OGRFeature::DestroyFeature(feature);
			counter++;
		}	
	}
	
	void ZonalStats::calcImageStats(GDALDataset *image, RSGISZonalPolygons *polygon) throw(RSGISVectorZonalException)
	{
		std::cout.precision(15);
		geos::geom::Envelope *envelope = NULL;
		GDALRasterBand *imageBand;
		double *transformation = NULL;
		float *imgData = NULL;
		geos::geom::Coordinate *coord = NULL;
		
		int imgXSize = 0;
		int imgYSize = 0;
		double imgRes = 0;
		double imgResHalf = 0;
		double imgTLX = 0;
		double imgTLY = 0;
		double imgBRX = 0;
		double imgBRY = 0;
		
		int polyTLPxlX = 0;
		int polyTLPxlY = 0;
		int polyWidthPxl = 0;
		int polyHeightPxl = 0;
		int endXPxl = 0;
		int endYPxl = 0;
		
		bool first = true;
		int numPxls = 0;
		double sumPxls = 0;
		double minPxl = 0;
		double maxPxl = 0;
		double meanDiff = 0;
		double meanDiffSqSum = 0;
		double mean = 0;
		
		try
		{
			envelope = polygon->getBBox();
			
			transformation = new double[6];
			image->GetGeoTransform(transformation);
			
			imgRes = transformation[1];
			imgResHalf = imgRes/2;
			
			imgXSize = image->GetRasterXSize();
			imgYSize = image->GetRasterYSize();
			
			imgTLX = transformation[0];
			imgTLY = transformation[3];
			imgBRX = imgTLX + (imgXSize * imgRes);
			imgBRY = imgTLY - (imgYSize * imgRes);
			
			if((envelope->getMinX() < imgTLX) | (envelope->getMaxY() > imgTLY)
			   | (envelope->getMaxX() > imgBRX) | (envelope->getMinY() < imgBRY))
			{
				std::cout.precision(12);
				std::cout << "Polygon = [" << envelope->getMinX() << "," << envelope->getMaxY() << "][" << envelope->getMaxX() << "," << envelope->getMinY() << "]\n";
				std::cout << "Image = [" << imgTLX << "," << imgTLY << "][" << imgBRX << "," << imgBRY << "]\n";
				throw RSGISVectorZonalException("Vector does not within the raster");
			}
			
			polyTLPxlX = floor((envelope->getMinX() - imgTLX)/imgRes);
			polyTLPxlY = floor((imgTLY - envelope->getMaxY())/imgRes);
			polyWidthPxl = ceil((envelope->getMaxX() - envelope->getMinX())/imgRes);
			polyHeightPxl = ceil((envelope->getMaxY() - envelope->getMinY())/imgRes);
			endXPxl = polyTLPxlX + polyWidthPxl;
			endYPxl = polyTLPxlY + polyHeightPxl;
			
			imgData = (float *) CPLMalloc(sizeof(float)*imgXSize);
			
			coord = new geos::geom::Coordinate(0, 0, 0);
			
			for(int n = 0; n < image->GetRasterCount(); n++)
			{
				std::cout << "Band " << (n+1) << " of " << image->GetRasterCount() << std::endl;
				imageBand = image->GetRasterBand(n+1);
				first = true;
				numPxls = 0;
				sumPxls = 0;
				minPxl = 0;
				maxPxl = 0;
				meanDiff = 0;
				meanDiffSqSum = 0;
				mean = 0;
				
				for(int i = polyTLPxlY; i <= endYPxl; i++)
				{
					imageBand->RasterIO(GF_Read, 0, i, imgXSize, 1, imgData, imgXSize, 1, GDT_Float32, 0, 0);
					for(int j = polyTLPxlX; j <=endXPxl; j++)
					{
						coord->x = (imgTLX + (j * imgRes) + imgResHalf);
						coord->y = (imgTLY - (i * imgRes) - imgResHalf);
						
						if(polygon->contains(coord))
						{
							if(first)
							{
								minPxl = imgData[j];
								maxPxl = imgData[j];
								first = false;
							}
							else
							{
								if(imgData[j] < minPxl)
								{
									minPxl = imgData[j];
								}
								else if(imgData[j] > maxPxl)
								{
									maxPxl = imgData[j];
								}
							}
							sumPxls = sumPxls + imgData[j];
							numPxls++;
						}
					}
				}
				mean = (sumPxls/numPxls);
				polygon->setMean(mean, n);
				polygon->setMax(maxPxl, n);
				polygon->setMin(minPxl, n);
				
				if(polygon->calcStdDev(n))
				{
					for(int i = polyTLPxlY; i <= endYPxl; i++)
					{
						imageBand->RasterIO(GF_Read, 0, i, imgXSize, 1, imgData, imgXSize, 1, GDT_Float32, 0, 0);
						for(int j = polyTLPxlX; j <=endXPxl; j++)
						{
							coord->x = (imgTLX + (j * imgRes) + imgResHalf);
							coord->y = (imgTLY - (i * imgRes) - imgResHalf);
							
							if(polygon->contains(coord))
							{
								meanDiff = mean - imgData[j];
								meanDiffSqSum = meanDiffSqSum + (meanDiff * meanDiff);
							}
						}
					}
					
					polygon->setStdDev(sqrt(meanDiffSqSum/numPxls), n);
				}
				else
				{
					polygon->setStdDev(-1, n);
				}
			}
			
		}
		catch(RSGISVectorZonalException e)
		{
			if(envelope != NULL)
			{
				delete envelope;
			}
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgData != NULL)
			{
				delete imgData;
			}
			if(coord != NULL)
			{
				delete coord;
			}
			throw e;
		}
		
		if(envelope != NULL)
		{
			delete envelope;
		}
		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgData != NULL)
		{
			delete imgData;
		}
		if(coord != NULL)
		{
			delete coord;
		}
	}
	
	void ZonalStats::calcImageStats(GDALDataset *image, OGRPolygon *polygon, imagestats *stats) throw(RSGISVectorZonalException)
	{
		OGREnvelope *polyEnvelope = NULL;
		GDALRasterBand *imageBand;
		double *transformation = NULL;
		float *imgData = NULL;
		OGRSpatialReference *spatialReference = NULL;
		OGRPoint *tmpPoint = NULL;
		
		int imgXSize = 0;
		int imgYSize = 0;
		double imgRes = 0;
		double imgResHalf = 0;
		double imgTLX = 0;
		double imgTLY = 0;
		double imgBRX = 0;
		double imgBRY = 0;
		
		int polyTLPxlX = 0;
		int polyTLPxlY = 0;
		int polyWidthPxl = 0;
		int polyHeightPxl = 0;
		int endXPxl = 0;
		int endYPxl = 0;
		
		bool first = true;
		int numPxls = 0;
		double sumPxls = 0;
		double minPxl = 0;
		double maxPxl = 0;
		double meanDiff = 0;
		double meanDiffSqSum = 0;
		
		try
		{
			polyEnvelope = new OGREnvelope();
			polygon->getEnvelope(polyEnvelope);
			transformation = new double[6];
			image->GetGeoTransform(transformation);
			
			imgRes = transformation[1];
			imgResHalf = imgRes/2;
			
			imgXSize = image->GetRasterXSize();
			imgYSize = image->GetRasterYSize();
			
			imgTLX = transformation[0];
			imgTLY = transformation[3];
			imgBRX = imgTLX + (imgXSize * imgRes);
			imgBRY = imgTLY - (imgYSize * imgRes);
			
			if((polyEnvelope->MinX < imgTLX) | (polyEnvelope->MaxY > imgTLY)
			   | (polyEnvelope->MaxX > imgBRX) | (polyEnvelope->MinY < imgBRY))
			{
				throw RSGISVectorZonalException("Vector does not within the raster");
			}
			
			polyTLPxlX = floor((polyEnvelope->MinX - imgTLX)/imgRes);
			polyTLPxlY = floor((imgTLY - polyEnvelope->MaxY)/imgRes);
			polyWidthPxl = ceil((polyEnvelope->MaxX - polyEnvelope->MinX)/imgRes);
			polyHeightPxl = ceil((polyEnvelope->MaxY - polyEnvelope->MinY)/imgRes);
			endXPxl = polyTLPxlX + polyWidthPxl;
			endYPxl = polyTLPxlY + polyHeightPxl;
			
			imgData = (float *) CPLMalloc(sizeof(float)*imgXSize);
			spatialReference = polygon->getSpatialReference();
			
			tmpPoint = new OGRPoint();
			tmpPoint->assignSpatialReference(spatialReference);
			
			for(int n = 0; n < image->GetRasterCount(); n++)
			{
				std::cout << "Band " << (n+1) << " of " << image->GetRasterCount() << std::endl;
				imageBand = image->GetRasterBand(n+1);
				first = true;
				numPxls = 0;
				sumPxls = 0;
				minPxl = 0;
				maxPxl = 0;
				meanDiff = 0;
				meanDiffSqSum = 0;
				
				std::cout << "[" << polyTLPxlY << "," << endYPxl << "]" << std::endl;
				std::cout << "[" << polyTLPxlX << "," << endXPxl << "]" << std::endl;
				
				for(int i = polyTLPxlY; i <= endYPxl; i++)
				{
					imageBand->RasterIO(GF_Read, 0, i, imgXSize, 1, imgData, imgXSize, 1, GDT_Float32, 0, 0);
					for(int j = polyTLPxlX; j <=endXPxl; j++)
					{
						tmpPoint->setX(imgTLX + (j * imgRes) + imgResHalf);
						tmpPoint->setY(imgTLY - (i * imgRes) - imgResHalf);
						
						if(polygon->Contains(tmpPoint))
						{
							if(first)
							{
								minPxl = imgData[j];
								maxPxl = imgData[j];
								first = false;
							}
							else
							{
								if(imgData[j] < minPxl)
								{
									minPxl = imgData[j];
								}
								else if(imgData[j] > maxPxl)
								{
									maxPxl = imgData[j];
								}
							}
							sumPxls = sumPxls + imgData[j];
							numPxls++;
						}
					}
				}
				stats[n].mean = sumPxls/numPxls;
				stats[n].max = maxPxl;
				stats[n].min = minPxl;
				
				std::cout << "Mean = " << stats[n].mean << std::endl;
				std::cout << "Max = " << stats[n].max << std::endl;
				std::cout << "Min = " << stats[n].min << std::endl;
				
                stats[n].stddev = 0;
			}
			
		}
		catch(RSGISVectorZonalException e)
		{
			if(polyEnvelope != NULL)
			{
				delete polyEnvelope;
			}
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgData != NULL)
			{
				delete imgData;
			}
			if(spatialReference != NULL)
			{
				delete spatialReference;
			}
			if(tmpPoint != NULL)
			{
				delete tmpPoint;
			}
			throw e;
		}
		
		if(polyEnvelope != NULL)
		{
			delete polyEnvelope;
		}
		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgData != NULL)
		{
			delete imgData;
		}
		if(spatialReference != NULL)
		{
			delete spatialReference;
		}
		if(tmpPoint != NULL)
		{
			delete tmpPoint;
		}
	}
	
	void ZonalStats::outputData2Text(std::string outputTxt, bool **toCalc, imagestats **stats, int numFeatures, int numAttributes) throw(rsgis::utils::RSGISTextException)
	{
		std::ofstream outputTxtFile(outputTxt.c_str());
		try
		{
			if(!outputTxtFile.is_open())
			{
				throw rsgis::utils::RSGISTextException("Text file did not open!");
			}
			
			outputTxtFile << "Feature";
			for(int i = 0; i < numAttributes; i++)
			{
				outputTxtFile << ",mean_" << i << ",max_" << i << ",min_" << i << "stddev_" << i;
			}
			outputTxtFile << std::endl;
			
			for(int n = 0; n < numFeatures; n++)
			{
				outputTxtFile << n+1;
				for(int i = 0; i < numAttributes; i++)
				{
					if(toCalc[i][0])
					{
						//mean
						outputTxtFile << "," << stats[n][i].mean;
					}
					else
					{
						outputTxtFile << ",";
					}
					if(toCalc[i][1])
					{
						//max
						outputTxtFile << "," << stats[n][i].max;
					}
					else
					{
						outputTxtFile << ",";
					}
					if(toCalc[i][2])
					{
						//min
						outputTxtFile << "," << stats[n][i].min;
					}
					else
					{
						outputTxtFile << ",";
					}
					if(toCalc[i][3])
					{
						//stddev
						outputTxtFile << "," << stats[n][i].stddev;
					}
					else
					{
						outputTxtFile << ",";
					}
				}	
				outputTxtFile << std::endl;
			}
			outputTxtFile.flush();
			outputTxtFile.close();
		}
		catch(rsgis::utils::RSGISTextException e)
		{
			if(outputTxtFile.is_open())
			{
				outputTxtFile.flush();
				outputTxtFile.close();
			}
			throw e;
		}
	}
	
	ZonalStats::~ZonalStats()
	{
		
	}
	
	RSGISCalcZonalStatsFromRaster::RSGISCalcZonalStatsFromRaster(int numberOutBands, imagestats **stats, int numInBands, int numFeatures, bool stddev) : rsgis::img::RSGISCalcImageValue(numberOutBands)
	{
		this->stats = stats;
		this->numInBands = numInBands;
		this->numFeatures = numFeatures;
		this->stddev = stddev;
	}
	
	void RSGISCalcZonalStatsFromRaster::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("RSGISCalcZonalStatsFromRaster: No Implemented.");
	}
	
	void RSGISCalcZonalStatsFromRaster::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("RSGISCalcZonalStatsFromRaster: Not Implemented");
	}
	
	void RSGISCalcZonalStatsFromRaster::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("RSGISCalcZonalStatsFromRaster: No Implemented.");
	}
	
	void RSGISCalcZonalStatsFromRaster::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
	{
		if((numBands-1) != numInBands)
		{
			throw rsgis::img::RSGISImageCalcException("Number of input bands does not match!");
		}
		
		int feature = bandValues[0];
		if(feature > numFeatures)
		{
			throw rsgis::img::RSGISImageCalcException("There were not enough features defined for the pixel values");
		}
		
		if(feature != -1)
		{
			int band = 1;
			if(stddev)
			{
				double diff = 0;
				for(int i = 0; i < numInBands; i++)
				{
					diff = stats[feature][i-1].mean - bandValues[band];
					stats[feature][i].sumDiff += (diff * diff);
					band++;
				}
			}
			else
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(stats[feature][i].first)
					{
						stats[feature][i].n = 1;
						stats[feature][i].meanSum = bandValues[band];
						stats[feature][i].min = bandValues[band];
						stats[feature][i].max = bandValues[band];
						stats[feature][i].first = false;
					}
					else
					{
						stats[feature][i].n++;
						stats[feature][i].meanSum += bandValues[band];
						if(bandValues[band] > stats[feature][i].max)
						{
							stats[feature][i].max = bandValues[band];
						}
						if(bandValues[band] < stats[feature][i].min)
						{
							stats[feature][i].min = bandValues[band];
						}
					}
					band++;
				}
			}
		}
	}
	
	void RSGISCalcZonalStatsFromRaster::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not implemented");
	}
	
	bool RSGISCalcZonalStatsFromRaster::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not implemented");
	}
	
	void RSGISCalcZonalStatsFromRaster::setStdDev(bool stddev)
	{
		this->stddev = stddev;
	}
	
	RSGISCalcZonalStatsFromRaster::~RSGISCalcZonalStatsFromRaster()
	{
		
	}
	
	RSGISZonalStats::RSGISZonalStats(GDALDataset *image, GDALDataset *rasterFeatures, ZonalAttributes** attributes, int numAttributes, bool outPxlCount, std::string outZonalFileName)
	{
		this->datasets = new GDALDataset*[2];
		this->datasets[0] = rasterFeatures;
		this->datasets[1] = image;
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->outPxlCount = outPxlCount;
		
		int dataSize = (numAttributes * 7) + 1; // Create array large enough to hold min, max, mean and stdev for each attribute an number of pixels
		
		this->data = new double[dataSize];
		
        calcValue = new RSGISCalcZonalStatsFromRasterPolygon(dataSize, attributes, numAttributes);
		//calcValue = new RSGISCalcZonalStatsFromPolygon(dataSize, attributes, numAttributes);
		calcImage = new rsgis::img::RSGISCalcImageSingle(calcValue);
		
		if (outZonalFileName != "") 
		{
			this->outputToTextFile = true;
			this->outZonalFile.open(outZonalFileName.c_str());
			this->firstLine = true;
		}
	}
	
	void RSGISZonalStats::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		try
		{
			calcValue->reset();
			calcImage->calcImageWithinRasterPolygon(datasets, 2, data, env, fid, true);
            
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			
            for(int i = 0; i < numAttributes; i++)
			{
				std::string attMin = attributes[i]->name + std::string("Min");
				std::string attMax = attributes[i]->name + std::string("Max");
				std::string attAvg = attributes[i]->name + std::string("Avg");
				std::string attStd = attributes[i]->name + std::string("Std");
				std::string attMde = attributes[i]->name + std::string("Mde");
                std::string attSum = attributes[i]->name + std::string("Sum");
				std::string attPix = attributes[i]->name + std::string("Pix");
								
				int offset = (7 * i) + 1; // Offset by total number of stats + 1 for count
				
				if (attributes[i]->outMin) 
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attMin.c_str()), this->data[offset]);
				}
				if (attributes[i]->outMax) 
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attMax.c_str()), this->data[offset+1]);
				}
				if (attributes[i]->outMean) 
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attAvg.c_str()), this->data[offset+2]);
				}
				if (attributes[i]->outStDev) 
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attStd.c_str()), this->data[offset+3]);
				}
				if (attributes[i]->outMode)
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attMde.c_str()), this->data[offset+4]);
				}
                if (attributes[i]->outSum)
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attSum.c_str()), this->data[offset+5]);
				}
				if (attributes[i]->outCount)
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attPix.c_str()), this->data[offset+6]);
				}
			}
					
			if(outPxlCount)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("TotalPxls"), this->data[0]);
			}
		}
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}
		
	}
	
	void RSGISZonalStats::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		
		if(!outZonalFile.is_open())
		{
			throw RSGISVectorException("Text file not open to write to!");
		}
		
		// Zonal stats - output to text file.
		try
		{
			calcValue->reset();
			
			calcImage->calcImageWithinRasterPolygon(datasets, 2, data, env, fid, true);
			
			// Write header info for first line
			if(this->firstLine)
			{
				this->outZonalFile << "FID";
				for(int i = 0; i < numAttributes; i++)
				{
					
					std::string attMin = attributes[i]->name + std::string("Min");
					std::string attMax = attributes[i]->name + std::string("Max");
					std::string attAvg = attributes[i]->name + std::string("Avg");
                    std::string attStd = attributes[i]->name + std::string("Std");
					std::string attMde = attributes[i]->name + std::string("Mde");
                    std::string attSum = attributes[i]->name + std::string("Sum");
					std::string attPix = attributes[i]->name + std::string("Pix");
					
					if (attributes[i]->outMin) 
					{
						this->outZonalFile << "," << attMin ;
					}
					if (attributes[i]->outMax) 
					{
						this->outZonalFile << "," << attMax;
					}
					if (attributes[i]->outMean) 
					{
						this->outZonalFile << "," << attAvg;
					}
					if (attributes[i]->outStDev) 
					{
						this->outZonalFile << "," << attStd;
					}
					if (attributes[i]->outMode) 
					{
						this->outZonalFile << "," << attMde;
					}
                    if (attributes[i]->outSum)
					{
						this->outZonalFile << "," << attSum;
					}
					if (attributes[i]->outCount)
					{
						this->outZonalFile << "," << attPix;
					}
				}
				
				if(outPxlCount)
				{
					this->outZonalFile << ",TotalPxls";
				}
				
				this->outZonalFile << "\n";
				this->firstLine = false;
			}
			
			this->outZonalFile << fid;
			
			for(int i = 0; i < numAttributes; i++)
			{
				
				int offset = (7 * i) + 1; // Offset by total number of stats + 1 for count
				
				if (attributes[i]->outMin) 
				{
					this->outZonalFile << "," << this->data[offset];
				}
				if (attributes[i]->outMax) 
				{
					this->outZonalFile << "," << this->data[offset+1];
				}
				if (attributes[i]->outMean) 
				{
					this->outZonalFile << "," << this->data[offset+2];
				}
				if (attributes[i]->outStDev) 
				{
					this->outZonalFile << "," << this->data[offset+3];
				}
				if (attributes[i]->outMode) 
				{
					this->outZonalFile << "," << this->data[offset+4];
				}
                if (attributes[i]->outSum)
				{
					this->outZonalFile << "," << this->data[offset+5];
				}
				if (attributes[i]->outCount)
				{
					this->outZonalFile << "," << this->data[offset+6];
				}
			}
			if(outPxlCount)
			{
				this->outZonalFile << "," << this->data[0];
			}
			this->outZonalFile << "\n";
			
		}
		
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}
	}
	
	void RSGISZonalStats::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		for(int i = 0; i < numAttributes; i++)
		{
			
			// Create names for fields
			if(attributes[i]->name.length() > 7)
			{
				std::cout << attributes[i]->name << " will be truncated to \'" << attributes[i]->name.substr(0, 7) << "\'\n";
				attributes[i]->name = attributes[i]->name.substr(0, 7);
			}
			
			std::string attMin = attributes[i]->name + std::string("Min");
			std::string attMax = attributes[i]->name + std::string("Max");
			std::string attAvg = attributes[i]->name + std::string("Avg");
			std::string attStd = attributes[i]->name + std::string("Std");
            std::string attMde = attributes[i]->name + std::string("Mde");
            std::string attSum = attributes[i]->name + std::string("Sum");
			std::string attPix = attributes[i]->name + std::string("Pix");
			
			if(attributes[i]->outMin)
			{
				OGRFieldDefn shpField(attMin.c_str(), OFTReal);
				shpField.SetPrecision(10);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'Min\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
			
			if(attributes[i]->outMax)
			{
				OGRFieldDefn shpField(attMax.c_str(), OFTReal);
				shpField.SetPrecision(10);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'Max\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
			
			if(attributes[i]->outMean)
			{
				OGRFieldDefn shpField(attAvg.c_str(), OFTReal);
				shpField.SetPrecision(10);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'Mean\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
			
			if(attributes[i]->outStDev)
			{
				OGRFieldDefn shpField(attStd.c_str(), OFTReal);
				shpField.SetPrecision(10);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'StDev\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
            if(attributes[i]->outMode)
			{
				OGRFieldDefn shpField(attMde.c_str(), OFTReal);
				shpField.SetPrecision(0);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'IntMode\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
            if(attributes[i]->outSum)
			{
				OGRFieldDefn shpField(attSum.c_str(), OFTReal);
				shpField.SetPrecision(0);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'outSum\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
			if(attributes[i]->outCount)
			{
				OGRFieldDefn shpField(attPix.c_str(), OFTReal);
				shpField.SetPrecision(10);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'Count\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
		}
		
		if(outPxlCount)
		{
			OGRFieldDefn shpField("TotalPxls", OFTReal);
			shpField.SetPrecision(10);
			if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'TotalPxls\' has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
		
	}
	
	RSGISZonalStats::~RSGISZonalStats()
	{
		delete[] datasets;
		delete[] data;
		delete calcValue;
		delete calcImage;
	}
	
	
	RSGISCalcZonalStatsFromRasterPolygon::RSGISCalcZonalStatsFromRasterPolygon(int numOutputValues, ZonalAttributes **attributes, int numAttributes) : rsgis::img::RSGISCalcImageSingleValue(numOutputValues)
	{
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->values = new std::vector<double>*[numAttributes];
		
		for (int i = 0; i < numAttributes; i++) 
		{
			values[i] = new std::vector<double>();
		}
	}
	
	void RSGISCalcZonalStatsFromRasterPolygon::calcImageValue(float *bandValuesImage, int numBands, int band) throw(rsgis::img::RSGISImageCalcException)
	{
		for(int i = 0; i < this->numAttributes; i++)
		{
			bool first = true;
			bool found = false;
			float minVal = 0;
			
			for(int j = 0; j < attributes[i]->numBands; j++)
			{
				if((attributes[i]->bands[j] > numBands) | (attributes[i]->bands[j] < 0))
				{
					throw rsgis::img::RSGISImageCalcException("The band attributes do not match the image.");
				}
				// Calculates the minimum of the two input image bands
				if(!boost::math::isnan(bandValuesImage[attributes[i]->bands[j]+1]))
                {
					if(first)
					{
						if((bandValuesImage[attributes[i]->bands[j]+1] > attributes[i]->minThresholds[j]) && (bandValuesImage[attributes[i]->bands[j]+1] < attributes[i]->maxThresholds[j]))
						{
							minVal = bandValuesImage[attributes[i]->bands[j]+1];
							first = false;
							found = true;
						}
					}
					else
					{
						if((bandValuesImage[attributes[i]->bands[j]+1] > attributes[i]->minThresholds[j]) && (bandValuesImage[attributes[i]->bands[j]+1] < attributes[i]->maxThresholds[j]))
						{
							if(bandValuesImage[attributes[i]->bands[j]+1] < minVal)
							{
								minVal = bandValuesImage[attributes[i]->bands[j]+1];
							}
						}
					}
				}
			}
			
			
			if(found)
			{
				this->values[i]->push_back(minVal);
			}
		}
		totalPxl++;
	}
	
	double* RSGISCalcZonalStatsFromRasterPolygon::getOutputValues() throw(rsgis::img::RSGISImageCalcException)
	{
		/* Calculte statistics and save to data array
		 *  Array is in the form:
		 *	 totalPix
		 *   min attribute1
		 *   max attribute 1
		 *	 mean attribute 1
	     *	 stDev attribute 1
         *   mode attribute 1
         *   sum attribute 1
		 *   count attribute 1
		 *   min attribute2
		 *   max attribute 2
		 *	 mean attribute 2
	     *	 stDev attribute 2
         *   mode attribute 2
         *   sum attribute 2
		 *   count attribute 2
		 *
		 * This format is used to ensure compatibility with calc image
		 */
		//double *outputValues = new double[this->numOutputValues];
		outputValues[0] = this->totalPxl;
		
		float minVal = 0;
		float maxVal = 0;
		float sum = 0;
		float pixVal = 0;
		float mean = 0;
		float stDev = 0;
		
		for(int i = 0; i < numAttributes; i++) // Loop through attributes
		{
			int offset = (7 * i) + 1; // Offset by total number of stats + 1 for count
			mean = 0;
			sum = 0;
			
			for (unsigned int j = 0; j < values[i]->size(); j++)
			{
				if (j == 0)
				{
					pixVal = values[i]->at(j);
					minVal = pixVal;
					maxVal = pixVal;
					sum = pixVal;
				}
				else
				{
					pixVal = values[i]->at(j);
					sum = sum + pixVal; // Add to sum
					if(pixVal < minVal){minVal = pixVal;} // If lower update minimum
					if(pixVal > maxVal){maxVal = pixVal;} // If higher update maximum
				}
				
			}
			
			// Add min to output data
			outputValues[offset] = minVal;
			outputValues[offset+1] = maxVal;
			outputValues[offset+6] = values[i]->size();
			outputValues[offset+5] = sum;
            
			// Calculate mean
			if((sum != 0) && (values[i]->size() != 0))
			{
				mean = sum / values[i]->size();
				outputValues[offset+2] = mean;
				sum = 0;
				
				// Calculate stdev
                if(attributes[i]->outStDev)
                {
                    for (unsigned int j = 0; j < values[i]->size(); j++)
                    {
                        pixVal = values[i]->at(j);
                        sum = sum + ((pixVal - mean) * (pixVal - mean)); // (meanx - x)^2
                    }
                    stDev = sqrt(sum / values[i]->size());
                    outputValues[offset+3] = stDev;
                }
                else
                {
                    outputValues[offset+3] = 0;
                }
                
                
			}
			else
			{
				// Set mean and standard deviation to zero
				outputValues[offset+2] = 0;
				outputValues[offset+3] = 0;
			}
            
            if(attributes[i]->outMode) // Calculate integer mode
            {
                if(values[i]->size() > 0)
                {
                    std::vector<int> *valuesInt = new std::vector<int>();
                    
                    for (unsigned int j = 0; j < values[i]->size(); j++)
                    {
                        valuesInt->push_back(int(values[i]->at(j)));
                    }
                    
                    std::sort(valuesInt->begin(), valuesInt->end());
                    
                    unsigned int maxCount = 0;
                    unsigned int currentCount = 0;
                    unsigned int currentNumber = valuesInt->at(0);
                    unsigned int modeVal = valuesInt->at(0);
                    
                    for (unsigned int j = 0; j < valuesInt->size(); ++j)
                    {
                        if(valuesInt->at(j) == currentNumber)
                        {
                            ++currentCount;
                        }
                        else // New number
                        {
                            if(currentCount > maxCount) // Update mode (if more)
                            {
                                maxCount = currentCount;
                                modeVal = currentNumber;
                            }
                            currentCount = 1; // Reset count (include 1 for current number
                            currentNumber = valuesInt->at(j);
                        }
                    }
                    
                    // Check for last number
                    if(currentCount > maxCount) // Update mode (if more)
                    {
                        modeVal = valuesInt->back();
                    }
                    
                    outputValues[offset+4] = modeVal;
                    
                    delete valuesInt;
                }
                else{outputValues[offset+4] = 0;}
            }
            else{outputValues[offset+4] = 0;}
            
		}
		
		return outputValues;
		
	}
	
	void RSGISCalcZonalStatsFromRasterPolygon::reset()
	{
		for(int i = 0; i < numAttributes; i++)
		{
			values[i]->clear();
		}
		totalPxl = 0;
	}
	
	RSGISCalcZonalStatsFromRasterPolygon::~RSGISCalcZonalStatsFromRasterPolygon()
	{
		delete values;
	}
	
	
	RSGISZonalStatsPoly::RSGISZonalStatsPoly(GDALDataset *image, ZonalAttributes** attributes, int numAttributes, bool outPxlCount, rsgis::img::pixelInPolyOption method, std::string outZonalFileName)
	{
		this->datasets = new GDALDataset*[1];
		this->datasets[0] = image;
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->outPxlCount = outPxlCount;
		this->method = method;
		
		int dataSize = (numAttributes * 7) + 1; // Create array large enough to hold min, max, mean and stdev for each attribute an number of pixels
		
		this->data = new double[dataSize];
		
		calcValue = new RSGISCalcZonalStatsFromPolygon(dataSize, attributes, numAttributes);
		calcImage = new rsgis::img::RSGISCalcImageSingle(calcValue);
		
		if (outZonalFileName != "")
		{
			this->outputToTextFile = true;
			this->outZonalFile.open(outZonalFileName.c_str());
			this->firstLine = true;
		}
        
    }
	
	void RSGISZonalStatsPoly::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
        // Check for polygon geometry
        if( (wkbFlatten(inFeature->GetGeometryRef()->getGeometryType()) != wkbPolygon) && (wkbFlatten(inFeature->GetGeometryRef()->getGeometryType()) != wkbMultiPolygon)   )
        {
            if(wkbFlatten(inFeature->GetGeometryRef()->getGeometryType()) == wkbPoint){throw RSGISVectorException("This function is for polygons. Either buffer points first (using \'buffervector\') or use \'pixelval\' instead.");}
            else{throw RSGISVectorException("Unsupported geometry.");}
        }
        try
		{
			RSGISVectorUtils vecUtils;
			calcValue->reset();
			
			OGRPolygon *inOGRPoly;
			geos::geom::Polygon *poly;
            
			inOGRPoly = (OGRPolygon *) inFeature->GetGeometryRef();
			poly = vecUtils.convertOGRPolygon2GEOSPolygon(inOGRPoly);

			calcImage->calcImageWithinPolygon(datasets, 1, this->data, env, poly, true, method);
			
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			
			for(int i = 0; i < numAttributes; i++)  
			{
				std::string attMin = attributes[i]->name + std::string("Min");
				std::string attMax = attributes[i]->name + std::string("Max");
				std::string attAvg = attributes[i]->name + std::string("Avg");
				std::string attStd = attributes[i]->name + std::string("Std");
				std::string attMde = attributes[i]->name + std::string("Mde");
                std::string attSum = attributes[i]->name + std::string("Sum");
				std::string attPix = attributes[i]->name + std::string("Pix");
								
				int offset = (7 * i) + 1; // Offset by total number of stats + 1 for count
				          
				if (attributes[i]->outMin) 
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attMin.c_str()), this->data[offset]);
				}
				if (attributes[i]->outMax) 
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attMax.c_str()), this->data[offset+1]);
				}
				if (attributes[i]->outMean) 
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attAvg.c_str()), this->data[offset+2]);
				}
				if (attributes[i]->outStDev) 
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attStd.c_str()), this->data[offset+3]);
				}
				if (attributes[i]->outMode) 
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attMde.c_str()), this->data[offset+4]);
				}
                if (attributes[i]->outSum)
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attSum.c_str()), this->data[offset+5]);
				}
				if (attributes[i]->outCount)
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(attPix.c_str()), this->data[offset+6]);
				}
			}
			
			if(outPxlCount)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("TotalPxls"), this->data[0]);
			}
		}
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}
	}
	
	void RSGISZonalStatsPoly::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		// Zonal stats - output to text file.
		if(!outZonalFile.is_open())
		{
			throw RSGISVectorException("Text file not open to write to!");
		}
        // Check for polygon geometry
        if( (wkbFlatten(feature->GetGeometryRef()->getGeometryType()) != wkbPolygon) && (wkbFlatten(feature->GetGeometryRef()->getGeometryType()) != wkbMultiPolygon)   )
        {
            if(wkbFlatten(feature->GetGeometryRef()->getGeometryType()) == wkbPoint){throw RSGISVectorException("This function is for polygons. Either buffer points first (using \'buffervector\') or use \'pixelval\' instead.");}
            else{throw RSGISVectorException("Unsupported geometry.");}
        }
		try
		{
			RSGISVectorUtils vecUtils;
			calcValue->reset();
			
			OGRPolygon *inOGRPoly;
			geos::geom::Polygon *poly;
            
			inOGRPoly = (OGRPolygon *) feature->GetGeometryRef();
			poly = vecUtils.convertOGRPolygon2GEOSPolygon(inOGRPoly);
			
			calcImage->calcImageWithinPolygon(datasets, 1, this->data, env, poly, true, method);
			
			// Add header info for first line
			if(this->firstLine)
			{
				this->outZonalFile << "FID";
				for(int i = 0; i < numAttributes; i++)
				{
					
					std::string attMin = attributes[i]->name + std::string("Min");
					std::string attMax = attributes[i]->name + std::string("Max");
					std::string attAvg = attributes[i]->name + std::string("Avg");
                    std::string attStd = attributes[i]->name + std::string("Std");
					std::string attMde = attributes[i]->name + std::string("Mde");
                    std::string attSum = attributes[i]->name + std::string("Sum");
					std::string attPix = attributes[i]->name + std::string("Pix");
					
					if (attributes[i]->outMin) 
					{
						this->outZonalFile << "," << attMin ;
					}
					if (attributes[i]->outMax) 
					{
						this->outZonalFile << "," << attMax;
					}
					if (attributes[i]->outMean) 
					{
						this->outZonalFile << "," << attAvg;
					}
					if (attributes[i]->outStDev) 
					{
						this->outZonalFile << "," << attStd;
					}
					if (attributes[i]->outMode) 
					{
						this->outZonalFile << "," << attMde;
					}
                    if (attributes[i]->outSum)
					{
						this->outZonalFile << "," << attSum;
					}
					if (attributes[i]->outCount)
					{
						this->outZonalFile << "," << attPix;
					}
				}
				
				if(outPxlCount)
				{
					this->outZonalFile << ",TotalPxls";
				}
				
				this->outZonalFile << "\n";
				this->firstLine = false;
			}
			
			this->outZonalFile << fid;
			
			for(int i = 0; i < numAttributes; i++)
			{
				
				int offset = (7 * i) + 1; // Offset by total number of stats + 1 for count
				
				if (attributes[i]->outMin) 
				{
					this->outZonalFile << "," << this->data[offset];
				}
				if (attributes[i]->outMax) 
				{
					this->outZonalFile << "," << this->data[offset+1];
				}
				if (attributes[i]->outMean) 
				{
					this->outZonalFile << "," << this->data[offset+2];
				}
				if (attributes[i]->outStDev) 
				{
					this->outZonalFile << "," << this->data[offset+3];
				}
				if (attributes[i]->outMode) 
				{
					this->outZonalFile << "," << this->data[offset+4];
				}
                if (attributes[i]->outSum)
				{
					this->outZonalFile << "," << this->data[offset+5];
				}
				if (attributes[i]->outCount)
				{
					this->outZonalFile << "," << this->data[offset+6];
				}
			}
			if(outPxlCount)
			{
				this->outZonalFile << "," << this->data[0];
			}
			this->outZonalFile << "\n";

		}
		
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}
	}
	
	void RSGISZonalStatsPoly::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		for(int i = 0; i < numAttributes; i++)
		{
			
			// Create names for fields
			if(attributes[i]->name.length() > 7)
			{
				std::cout << attributes[i]->name << " will be truncated to \'" << attributes[i]->name.substr(0, 7) << "\'\n";
				attributes[i]->name = attributes[i]->name.substr(0, 7);
			}
			
			std::string attMin = attributes[i]->name + std::string("Min");
			std::string attMax = attributes[i]->name + std::string("Max");
			std::string attAvg = attributes[i]->name + std::string("Avg");
			std::string attStd = attributes[i]->name + std::string("Std");
            std::string attMde = attributes[i]->name + std::string("Mde");
            std::string attSum = attributes[i]->name + std::string("Sum");
			std::string attPix = attributes[i]->name + std::string("Pix");
			
			if(attributes[i]->outMin)
			{
				OGRFieldDefn shpField(attMin.c_str(), OFTReal);
				shpField.SetPrecision(10);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'Min\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
			
			if(attributes[i]->outMax)
			{
				OGRFieldDefn shpField(attMax.c_str(), OFTReal);
				shpField.SetPrecision(10);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'Max\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
			
			if(attributes[i]->outMean)
			{
				OGRFieldDefn shpField(attAvg.c_str(), OFTReal);
				shpField.SetPrecision(10);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'Mean\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
			
			if(attributes[i]->outStDev)
			{
				OGRFieldDefn shpField(attStd.c_str(), OFTReal);
				shpField.SetPrecision(10);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'StDev\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
            if(attributes[i]->outMode)
			{
				OGRFieldDefn shpField(attMde.c_str(), OFTReal);
				shpField.SetPrecision(0);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'IntMode\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
            if(attributes[i]->outSum)
			{
				OGRFieldDefn shpField(attSum.c_str(), OFTReal);
				shpField.SetPrecision(0);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'IntMode\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
			if(attributes[i]->outCount)
			{
				OGRFieldDefn shpField(attPix.c_str(), OFTReal);
				shpField.SetPrecision(10);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field \'Count\' has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
		}
		
		if(outPxlCount)
		{
			OGRFieldDefn shpField("TotalPxls", OFTReal);
			shpField.SetPrecision(10);
			if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'TotalPxls\' has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
		
	}
	
	RSGISZonalStatsPoly::~RSGISZonalStatsPoly()
	{
		delete[] datasets;
		delete[] data;
		delete calcValue;
		delete calcImage;
		if (this->outputToTextFile) 
		{
			this->outZonalFile.close();
		}
	}
	
	
	RSGISCalcZonalStatsFromPolygon::RSGISCalcZonalStatsFromPolygon(int numOutputValues, ZonalAttributes **attributes, int numAttributes) : rsgis::img::RSGISCalcImageSingleValue(numOutputValues)
	{
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->values = new std::vector<double>*[numAttributes];
		
		for (int i = 0; i < numAttributes; i++) 
		{
			values[i] = new std::vector<double>();
		}
	}
	
	void RSGISCalcZonalStatsFromPolygon::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(rsgis::img::RSGISImageCalcException)
	{
		for(int i = 0; i < this->numAttributes; i++)
		{
			bool first = true;
			bool found = false;  
			float minVal = 0;
			
			for(int j = 0; j < attributes[i]->numBands; j++)
			{
				if((attributes[i]->bands[j] > numBands) | (attributes[i]->bands[j] < 0))
				{
					throw rsgis::img::RSGISImageCalcException("The band attributes do not match the image.");
				}
				// Calculates the minimum of the two input image bands
                if(!boost::math::isnan(bandValuesImage[attributes[i]->bands[j]]))
                {
                    if(first)
                    {
                        if((bandValuesImage[attributes[i]->bands[j]] > attributes[i]->minThresholds[j]) && (bandValuesImage[attributes[i]->bands[j]] < attributes[i]->maxThresholds[j]))
                        {
                            minVal = bandValuesImage[attributes[i]->bands[j]];
                            first = false;
                            found = true;
                        }

                    }
                    else
                    {
                        if((bandValuesImage[attributes[i]->bands[j]] > attributes[i]->minThresholds[j]) && (bandValuesImage[attributes[i]->bands[j]] < attributes[i]->maxThresholds[j]))
                        {
                            if(bandValuesImage[attributes[i]->bands[j]] < minVal)
                            {
                                minVal = bandValuesImage[attributes[i]->bands[j]];
                            }
                        }
                    }
                }
			}
			
			if(found)
			{
				this->values[i]->push_back(minVal);
			}
		}
		totalPxl++;
	}
	
	double* RSGISCalcZonalStatsFromPolygon::getOutputValues() throw(rsgis::img::RSGISImageCalcException)
	{
		/* Calculte statistics and save to data array
		 *  Array is in the form:
		 *	 totalPix
		 *   min attribute1
		 *   max attribute 1
		 *	 mean attribute 1
	     *	 stDev attribute 1
         *   mode attribute 1
         *   sum attribute 1
		 *   count attribute 1
		 *   min attribute2
		 *   max attribute 2
		 *	 mean attribute 2
	     *	 stDev attribute 2
         *   mode attribute 2
         *   sum attribute 2
		 *   count attribute 2
		 *
		 * This format is used to ensure compatibility with calc image
		 */
		//double *outputValues = new double[this->numOutputValues];
		outputValues[0] = this->totalPxl;
		
		float minVal = 0;
		float maxVal = 0;
		float sum = 0;
		float pixVal = 0;
		float mean = 0;
		float stDev = 0;
		
		for(int i = 0; i < numAttributes; i++) // Loop through attributes
		{
			int offset = (7 * i) + 1; // Offset by total number of stats + 1 for count
			mean = 0;
			sum = 0;
			
			for (unsigned int j = 0; j < values[i]->size(); j++) 
			{
				if (j == 0) 
				{
					pixVal = values[i]->at(j);
					minVal = pixVal;
					maxVal = pixVal;
					sum = pixVal;
				}
				else 
				{
					pixVal = values[i]->at(j);
					sum = sum + pixVal; // Add to sum
					if(pixVal < minVal){minVal = pixVal;} // If lower update minimum
					if(pixVal > maxVal){maxVal = pixVal;} // If higher update maximum
				}
				
			}
			
			// Add min to output data
			outputValues[offset] = minVal;
			outputValues[offset+1] = maxVal;
			outputValues[offset+6] = values[i]->size();
			outputValues[offset+5] = sum;
            			
			// Calculate mean
			if((sum != 0) && (values[i]->size() != 0))
			{
				mean = sum / values[i]->size();
				outputValues[offset+2] = mean;
				sum = 0;
				
				// Calculate stdev
                if(attributes[i]->outStDev)
                {
                    for (unsigned int j = 0; j < values[i]->size(); j++) 
                    {
                        pixVal = values[i]->at(j);
                        sum = sum + ((pixVal - mean) * (pixVal - mean)); // (meanx - x)^2
                    }
                    stDev = sqrt(sum / values[i]->size());
                    outputValues[offset+3] = stDev;
                }
                else 
                {
                    outputValues[offset+3] = 0;
                }


			}
			else 
			{
				// Set mean and standard deviation to zero
				outputValues[offset+2] = 0;
				outputValues[offset+3] = 0;
			}
            
            if(attributes[i]->outMode) // Calculate integer mode
            {
                if(values[i]->size() > 0)
                {
                    std::vector<int> *valuesInt = new std::vector<int>();
                    
                    for (unsigned int j = 0; j < values[i]->size(); j++) 
                    {
                        valuesInt->push_back(int(values[i]->at(j)));
                    }
                    
                    std::sort(valuesInt->begin(), valuesInt->end());
                    
                    unsigned int maxCount = 0;
                    unsigned int currentCount = 0;
                    unsigned int currentNumber = valuesInt->at(0);
                    unsigned int modeVal = valuesInt->at(0);
                    
                    for (unsigned int j = 0; j < valuesInt->size(); ++j)
                    {
                        if(valuesInt->at(j) == currentNumber)
                        {
                            ++currentCount;
                        }
                        else // New number
                        {
                            if(currentCount > maxCount) // Update mode (if more)
                            {
                                maxCount = currentCount;
                                modeVal = currentNumber;
                            }
                            currentCount = 1; // Reset count (include 1 for current number
                            currentNumber = valuesInt->at(j);
                        }
                    }
                    
                    // Check for last number
                    if(currentCount > maxCount) // Update mode (if more)
                    {
                        modeVal = valuesInt->back();
                    }
                    
                    outputValues[offset+4] = modeVal;
                    
                    delete valuesInt;                    
                }
                else{outputValues[offset+4] = 0;}
            }
            else{outputValues[offset+4] = 0;}
                
		}
		
		return outputValues;
		
	}
    
	void RSGISCalcZonalStatsFromPolygon::reset()
	{
		for(int i = 0; i < numAttributes; i++)
		{
			values[i]->clear();
		}
		totalPxl = 0;
	}
	
	RSGISCalcZonalStatsFromPolygon::~RSGISCalcZonalStatsFromPolygon()
	{
		delete[] values;
	}





    RSGISZonalStatsPolyUpdateLyr::RSGISZonalStatsPolyUpdateLyr(GDALDataset *image, std::vector<ZonalBandAttrs> *zonalBandAtts, rsgis::img::pixelInPolyOption method)
    {
        this->datasets = new GDALDataset*[1];
        this->datasets[0] = image;
        this->zonalBandAtts = zonalBandAtts;
        this->method = method;
        this->nBands = image->GetRasterCount();
        this->pxlVals = new std::vector<float>*[this->nBands];
        for(unsigned int i = 0; i < this->nBands; ++i)
        {
            this->pxlVals[i] = new std::vector<float>();
            this->pxlVals[i]->reserve(62500);
        }

        this->calcValue = new rsgis::img::RSGISGetPixelsInPoly(this->pxlVals, this->nBands);
        this->calcImage = new rsgis::img::RSGISCalcImage(this->calcValue);

        this->vecUtils = new RSGISVectorUtils();
        this->dataVal = new std::vector<double>();
        this->dataVal->reserve(62500);

        this->statsSummary = new rsgis::math::RSGISStatsSummary();
        this->mathUtils = new rsgis::math::RSGISMathsUtils();
        this->mathUtils->initStatsSummaryValues(this->statsSummary);
    }

    void RSGISZonalStatsPolyUpdateLyr::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
    {
        // Check for polygon geometry
        if((wkbFlatten(feature->GetGeometryRef()->getGeometryType()) != wkbPolygon) && (wkbFlatten(feature->GetGeometryRef()->getGeometryType()) != wkbMultiPolygon))
        {
            throw RSGISVectorException("Unsupported geometry; geometry must be polygon or multi-polygon.");
        }
        try
        {
            // Get Polygon
            inOGRPoly = (OGRPolygon *) feature->GetGeometryRef();
            poly = vecUtils->convertOGRPolygon2GEOSPolygon(inOGRPoly);
            // Get Pixel values.
            calcImage->calcImageWithinPolygonExtentInMem(this->datasets, 1, env, poly, this->method);

            // Get Stats from pixels and add to feature.
            for(std::vector<ZonalBandAttrs>::iterator iterAtts = zonalBandAtts->begin(); iterAtts != zonalBandAtts->end(); ++iterAtts)
            {
                int binIdx = (*iterAtts).band-1;
                this->mathUtils->initStatsSummaryValues(this->statsSummary);
                this->dataVal->clear();
                for(std::vector<float>::iterator iterVal = this->pxlVals[binIdx]->begin(); iterVal != this->pxlVals[binIdx]->end(); ++iterVal)
                {
                    if( ((*iterVal) >= (*iterAtts).minThres) & ((*iterVal) < (*iterAtts).maxThres) )
                    {
                        this->dataVal->push_back(*iterVal);
                    }
                }
                this->statsSummary->calcMin = (*iterAtts).outMin;
                this->statsSummary->calcMax = (*iterAtts).outMax;
                this->statsSummary->calcMean = (*iterAtts).outMean;
                this->statsSummary->calcSum = (*iterAtts).outSum;
                this->statsSummary->calcStdDev = (*iterAtts).outStDev;
                this->statsSummary->calcMedian = (*iterAtts).outMedian;
                this->statsSummary->calcMode = (*iterAtts).outMode;
                this->statsSummary->calcVariance = false;
                if(this->dataVal->size() > 0)
                {
                    this->mathUtils->generateStats(this->dataVal, this->statsSummary);
                }

                if((*iterAtts).outMin)
                {
                    feature->SetField((*iterAtts).minName.c_str(), this->statsSummary->min);
                }
                if((*iterAtts).outMax)
                {
                    feature->SetField((*iterAtts).maxName.c_str(), this->statsSummary->max);
                }
                if((*iterAtts).outMean)
                {
                    feature->SetField((*iterAtts).meanName.c_str(), this->statsSummary->mean);
                }
                if((*iterAtts).outSum)
                {
                    feature->SetField((*iterAtts).sumName.c_str(), this->statsSummary->sum);
                }
                if((*iterAtts).outStDev)
                {
                    feature->SetField((*iterAtts).stdName.c_str(), this->statsSummary->stdDev);
                }
                if((*iterAtts).outMedian)
                {
                    feature->SetField((*iterAtts).medianName.c_str(), this->statsSummary->median);
                }
                if((*iterAtts).outMode)
                {
                    feature->SetField((*iterAtts).modeName.c_str(), this->statsSummary->mode);
                }
                if((*iterAtts).outCount)
                {
                    feature->SetField((*iterAtts).countName.c_str(), (double)(this->dataVal->size()));
                }
            }

            // Reset.
            delete poly;
            for(unsigned int i = 0; i < this->nBands; ++i)
            {
                this->pxlVals[i]->clear();
            }
        }
        catch(RSGISException& e)
        {
            throw RSGISVectorException(e.what());
        }
    }

    RSGISZonalStatsPolyUpdateLyr::~RSGISZonalStatsPolyUpdateLyr()
    {
        for(unsigned int i = 0; i < this->nBands; ++i)
        {
            delete this->pxlVals[i];
        }
        delete[] this->pxlVals;
        delete[] this->datasets;
        delete this->calcValue;
        delete this->calcImage;
        delete this->vecUtils;
        delete this->dataVal;
        delete this->statsSummary;
        delete this->mathUtils;
    }
	
	
}}


