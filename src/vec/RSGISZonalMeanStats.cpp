/*
 *  RSGISZonalMeanStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/03/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
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

#include "RSGISZonalMeanStats.h"


namespace rsgis{namespace vec{
	
	RSGISZonalMeanStats::RSGISZonalMeanStats(GDALDataset *image, GDALDataset *rasterFeatures, MeanAttributes** attributes, int numAttributes, bool outPxlCount)
	{
		this->datasets = new GDALDataset*[2];
		this->datasets[0] = rasterFeatures;
		this->datasets[1] = image;
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->outPxlCount = outPxlCount;
		
		dataSize = numAttributes+ 1; // 0 Pxl Count // 1 ..  n Attribute data
		data = new double[dataSize];
		
		calcValue = new RSGISCalcZonalMeanFromRasterPolygon(dataSize, attributes, numAttributes);
		calcImage = new rsgis::img:: RSGISCalcImageSingle(calcValue);
	}
	
	void RSGISZonalMeanStats::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		try
		{
			calcValue->reset();
			calcImage->calcImageWithinRasterPolygon(datasets, 2, data, env, fid, true);
			
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			int count = 0;
			for(int i = 1; i < dataSize; i++)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex(attributes[count]->name.c_str()), data[i]);
				count++;
			}
			
			if(outPxlCount)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("TotalPxls"), data[0]);
			}
		}
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}
	
	}
	
	void RSGISZonalMeanStats::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		for(int i = 0; i < numAttributes; i++)
		{
			if(attributes[i]->name.length() > 10)
			{
				std::cout << attributes[i]->name << " will be truncated to \'" << attributes[i]->name.substr(0, 10) << "\'\n";
				attributes[i]->name = attributes[i]->name.substr(0, 10);
			}
			OGRFieldDefn shpField(attributes[i]->name.c_str(), OFTReal);
			shpField.SetPrecision(10);
			if(outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field ") + attributes[i]->name + std::string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
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
	
	RSGISZonalMeanStats::~RSGISZonalMeanStats()
	{
		delete[] datasets;
		delete[] data;
		delete calcValue;
		delete calcImage;
	}
	
	
	RSGISCalcZonalMeanFromRasterPolygon::RSGISCalcZonalMeanFromRasterPolygon(int numOutputValues, MeanAttributes **attributes, int numAttributes) : rsgis::img:: RSGISCalcImageSingleValue(numOutputValues)
	{
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->sum = new double[numAttributes];
		this->pxlCount = new double[numAttributes];
		for(int i = 0; i < numAttributes; i++)
		{
			sum[i] = 0;
			pxlCount[i] = 0;
		}
	}
	
	void RSGISCalcZonalMeanFromRasterPolygon::calcImageValue(float *bandValuesImage, int numBands, int band) throw(rsgis::img:: RSGISImageCalcException)
	{
		float min = 0;
		bool first = true;
		bool found = false;
		
		for(int i = 0; i < this->numAttributes; i++)
		{
			first = true;
			found = false;
			for(int j = 0; j < attributes[i]->numBands; j++)
			{
				//std::cout << "Number of bands = " << numBands << std::endl;
				if((attributes[i]->bands[j] > numBands) | (attributes[i]->bands[j] < 0))
				{
					throw rsgis::img:: RSGISImageCalcException("The band attributes do not match the image.");
				}
				// Calculates the minimum of the two input image bands
				if(!boost::math::isnan(attributes[i]->bands[j]+1))
                {
					if(first)
					{
						if((bandValuesImage[attributes[i]->bands[j]+1] > attributes[i]->minThresholds[j]) && (bandValuesImage[attributes[i]->bands[j]+1] < attributes[i]->maxThresholds[j]))
						{
							min = bandValuesImage[attributes[i]->bands[j]+1];
							first = false;
							found = true;
						}
					}
					else
					{
						if((bandValuesImage[attributes[i]->bands[j]+1] > attributes[i]->minThresholds[j]) && (bandValuesImage[attributes[i]->bands[j]+1] < attributes[i]->maxThresholds[j]))
						{
							if(bandValuesImage[attributes[i]->bands[j]+1] < min)
							{
								min = bandValuesImage[attributes[i]->bands[j]+1];
							}
						}
					}
				}
			}
			
			if(found)
			{
				this->sum[i] = this->sum[i] + min;
				this->pxlCount[i]++;
			}
		}
		totalPxl++;
	}
	
	double* RSGISCalcZonalMeanFromRasterPolygon::getOutputValues() throw(rsgis::img:: RSGISImageCalcException)
	{
		//double *outputData = new double[this->numOutputValues];
		outputValues[0] = this->totalPxl;
		
		for(int i = 0; i < this->numAttributes; i++)
		{
			if (sum[i] == 0 || pxlCount[i] == 0) 
			{
				outputValues[i+1] = 0;
			}
			else 
			{
				outputValues[i+1] = sum[i]/pxlCount[i];
			}
		}
		return outputValues;
	}
	
	void RSGISCalcZonalMeanFromRasterPolygon::reset()
	{
		for(int i = 0; i < numAttributes; i++)
		{
			this->sum[i] = 0;
			this->pxlCount[i] = 0;
		}
		this->totalPxl = 0;
	}
	
	RSGISCalcZonalMeanFromRasterPolygon::~RSGISCalcZonalMeanFromRasterPolygon()
	{
		delete[] sum;
	}
	
	
	RSGISZonalMeanStatsPoly::RSGISZonalMeanStatsPoly(GDALDataset *image, MeanAttributes** attributes, int numAttributes, bool outPxlCount, rsgis::img::pixelInPolyOption method, bool dB)
	{
		this->datasets = new GDALDataset*[1];
		this->datasets[0] = image;
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->outPxlCount = outPxlCount;
		this->method = method;
		
		dataSize = numAttributes + 1; // 0 Pxl Count // 1 ..  n Attribute data
		data = new double[dataSize];
		
		if (dB) 
		{
			calcValue = new RSGISCalcZonalMeanFromPolygondB(dataSize, attributes, numAttributes);
		}
		else
		{
			calcValue = new RSGISCalcZonalMeanFromPolygon(dataSize, attributes, numAttributes);
		}
		calcImage = new rsgis::img::RSGISCalcImageSingle(calcValue);
	}
	
	void RSGISZonalMeanStatsPoly::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		try
		{

			RSGISVectorUtils vecUtils;
			calcValue->reset();
			
			OGRPolygon *inOGRPoly;
			geos::geom::Polygon *poly;
			inOGRPoly = (OGRPolygon *) inFeature->GetGeometryRef();
			poly = vecUtils.convertOGRPolygon2GEOSPolygon(inOGRPoly);
			
			calcImage->calcImageWithinPolygon(datasets, 1, data, env, poly, true, method);
		
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			int count = 0;
			for(int i = 1; i < dataSize; i++)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex(attributes[count]->name.c_str()), data[i]);
				count++;
			}
			
			if(outPxlCount)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("TotalPxls"), data[0]);
			}
		}
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}
	}
	
	void RSGISZonalMeanStatsPoly::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		for(int i = 0; i < numAttributes; i++)
		{
			if(attributes[i]->name.length() > 10)
			{
				std::cout << attributes[i]->name << " will be truncated to \'" << attributes[i]->name.substr(0, 10) << "\'\n";
				attributes[i]->name = attributes[i]->name.substr(0, 10);
			}
			OGRFieldDefn shpField(attributes[i]->name.c_str(), OFTReal);
			shpField.SetPrecision(10);
			if(outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field ") + attributes[i]->name + std::string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
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
	
	RSGISZonalMeanStatsPoly::~RSGISZonalMeanStatsPoly()
	{
		delete[] datasets;
		delete[] data;
		delete calcValue;
		delete calcImage;
	}
	
	
	RSGISCalcZonalMeanFromPolygon::RSGISCalcZonalMeanFromPolygon(int numOutputValues, MeanAttributes **attributes, int numAttributes) : rsgis::img:: RSGISCalcImageSingleValue(numOutputValues)
	{
		this->attributes = attributes; 
		this->numAttributes = numAttributes;
		this->sum = new double[numAttributes];
		this->pxlCount = new double[numAttributes];
		for(int i = 0; i < numAttributes; i++) // Set sum to zero for all attributes.
		{
			sum[i] = 0;
			pxlCount[i] = 0;
		}
	}
	
	void RSGISCalcZonalMeanFromPolygon::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(rsgis::img:: RSGISImageCalcException)
	{
		float min = 0;
		bool first = true;
		bool found = false;
				
		for(int i = 0; i < this->numAttributes; i++) // Loop through attributes
		{
			first = true;
			found = false;
			for(int j = 0; j < attributes[i]->numBands; j++) // Loop through bands
			{
				if((attributes[i]->bands[j] > numBands) | (attributes[i]->bands[j] < 0))
				{
					throw rsgis::img:: RSGISImageCalcException("The band attributes do not match the image.");
				}
				// Calculates the minimum of the two input image bands
				if(!boost::math::isnan(attributes[i]->bands[j]))
                {
					if(first)
					{
						if((bandValuesImage[attributes[i]->bands[j]] > attributes[i]->minThresholds[j]) && (bandValuesImage[attributes[i]->bands[j]] < attributes[i]->maxThresholds[j]))
						{
							min = bandValuesImage[attributes[i]->bands[j]];
							first = false;
							found = true;
						}
					}
					else
					{
						if((bandValuesImage[attributes[i]->bands[j]] > attributes[i]->minThresholds[j]) && (bandValuesImage[attributes[i]->bands[j]] < attributes[i]->maxThresholds[j]))
						{
							if(bandValuesImage[attributes[i]->bands[j]] < min)
							{
								min = bandValuesImage[attributes[i]->bands[j]];
							}
						}
					}
				}
			}
			
			if(found)
			{
				this->sum[i] = this->sum[i] + min;
				this->pxlCount[i]++;
			}
		}
		totalPxl++;
	}
	
	double* RSGISCalcZonalMeanFromPolygon::getOutputValues() throw(rsgis::img:: RSGISImageCalcException)
	{
		//double *outputData = new double[this->numOutputValues];
		outputValues[0] = this->totalPxl;
		
		for(int i = 0; i < this->numAttributes; i++)
		{
			if (sum[i] == 0 || pxlCount[i] == 0) 
			{
				outputValues[i+1] = 0;
			}
			else 
			{
				outputValues[i+1] = sum[i]/pxlCount[i];
			}
		}
		return outputValues;
	}
	
	void RSGISCalcZonalMeanFromPolygon::reset()
	{
		for(int i = 0; i < numAttributes; i++)
		{
			sum[i] = 0;
			pxlCount[i] = 0;
		}
		totalPxl = 0;
	}
	
	RSGISCalcZonalMeanFromPolygon::~RSGISCalcZonalMeanFromPolygon()
	{
		delete[] sum;
		delete[] pxlCount;
	}
	
	RSGISCalcZonalMeanFromPolygondB::RSGISCalcZonalMeanFromPolygondB(int numOutputValues, MeanAttributes **attributes, int numAttributes) : rsgis::img:: RSGISCalcImageSingleValue(numOutputValues)
	{
		this->attributes = attributes; 
		this->numAttributes = numAttributes;
		this->sum = new double[numAttributes];
		this->pxlCount = new double[numAttributes];
		for(int i = 0; i < numAttributes; i++) // Set sum to zero for all attributes.
		{
			sum[i] = 0;
			pxlCount[i] = 0;
		}
	}
	
	void RSGISCalcZonalMeanFromPolygondB::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(rsgis::img:: RSGISImageCalcException)
	{
		float min = 0;
		bool first = true;
		bool found = false;
		
		for(int i = 0; i < this->numAttributes; i++) // Loop through attributes
		{
			first = true;
			found = false;
			for(int j = 0; j < attributes[i]->numBands; j++) // Loop through bands
			{
				if((attributes[i]->bands[j] > numBands) | (attributes[i]->bands[j] < 0))
				{
					throw rsgis::img:: RSGISImageCalcException("The band attributes do not match the image.");
				}
				// Calculates the minimum of the two input image bands
				if(!boost::math::isnan(attributes[i]->bands[j]))
                {
					if(first)
					{
						if((bandValuesImage[attributes[i]->bands[j]] > attributes[i]->minThresholds[j]) && (bandValuesImage[attributes[i]->bands[j]] < attributes[i]->maxThresholds[j]))
						{
							min = pow(10,(bandValuesImage[attributes[i]->bands[j]] / 10));
							first = false;
							found = true;
						}
					}
					else
					{
						if((bandValuesImage[attributes[i]->bands[j]] > attributes[i]->minThresholds[j]) && (bandValuesImage[attributes[i]->bands[j]] < attributes[i]->maxThresholds[j]))
						{
							if(bandValuesImage[attributes[i]->bands[j]] < min)
							{
								min = pow(10,(bandValuesImage[attributes[i]->bands[j]] / 10));
							}
						}
					}
				}
			}
			
			if(found)
			{
				this->sum[i] = this->sum[i] + min;
				this->pxlCount[i]++;
			}
		}
		totalPxl++;
	}
	
	double* RSGISCalcZonalMeanFromPolygondB::getOutputValues() throw(rsgis::img:: RSGISImageCalcException)
	{
		//double *outputData = new double[this->numOutputValues];
		outputValues[0] = this->totalPxl;
		
		for(int i = 0; i < this->numAttributes; i++)
		{
			if (sum[i] == 0 || pxlCount[i] == 0) 
			{
				outputValues[i+1] = 0;
			}
			else 
			{
				outputValues[i+1] = 10*log10(sum[i]/pxlCount[i]);
			}
		}
		return outputValues;
	}
	
	void RSGISCalcZonalMeanFromPolygondB::reset()
	{
		for(int i = 0; i < numAttributes; i++)
		{
			sum[i] = 0;
			pxlCount[i] = 0;
		}
		totalPxl = 0;
	}
	
	RSGISCalcZonalMeanFromPolygondB::~RSGISCalcZonalMeanFromPolygondB()
	{
		delete[] sum;
		delete[] pxlCount;
	}
	
	RSGISZonalWeightedMeanStatsPoly::RSGISZonalWeightedMeanStatsPoly(GDALDataset *image, MeanAttributes** attributes, int numAttributes, bool outPxlCount)
	{
		this->datasets = new GDALDataset*[1];
		this->datasets[0] = image;
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->outPxlCount = outPxlCount;
		
		dataSize = numAttributes + 1; // 0 Pxl Count // 1 ..  n Attribute data
		data = new double[dataSize];
		
		calcValue = new RSGISCalcZonalWeightedMeanStatsPoly(dataSize, attributes, numAttributes);
		calcImage = new rsgis::img:: RSGISCalcImageSingle(calcValue);
	}
	
	void RSGISZonalWeightedMeanStatsPoly::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		try
		{
			RSGISVectorUtils vecUtils;
			calcValue->reset();
			
			OGRPolygon *inOGRPoly;
			geos::geom::Polygon *poly;
			
			inOGRPoly = (OGRPolygon *) inFeature->GetGeometryRef();
			poly = vecUtils.convertOGRPolygon2GEOSPolygon(inOGRPoly);
			
			calcImage->calcImageWithinPolygon(datasets, 1, data, env, poly, true, rsgis::img::pixelAreaInPoly);
			
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			int count = 0;
			for(int i = 1; i < dataSize; i++)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex(attributes[count]->name.c_str()), data[i]);
				count++;
			}
			
			if(outPxlCount)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("TotalPxls"), data[0]);
			}
		}
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}
		
	}
	
	void RSGISZonalWeightedMeanStatsPoly::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		for(int i = 0; i < numAttributes; i++)
		{
			if(attributes[i]->name.length() > 10)
			{
				std::cout << attributes[i]->name << " will be truncated to \'" << attributes[i]->name.substr(0, 10) << "\'\n";
				attributes[i]->name = attributes[i]->name.substr(0, 10);
			}
			OGRFieldDefn shpField(attributes[i]->name.c_str(), OFTReal);
			shpField.SetPrecision(10);
			if(outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field ") + attributes[i]->name + std::string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
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
	
	RSGISZonalWeightedMeanStatsPoly::~RSGISZonalWeightedMeanStatsPoly()
	{
		delete[] datasets;
		delete[] data;
		delete calcValue;
		delete calcImage;
	}
	
	
	RSGISCalcZonalWeightedMeanStatsPoly::RSGISCalcZonalWeightedMeanStatsPoly(int numOutputValues, MeanAttributes **attributes, int numAttributes) : rsgis::img:: RSGISCalcImageSingleValue(numOutputValues)
	{
		this->attributes = attributes; 
		this->numAttributes = numAttributes;
		this->sum = new double[numAttributes];
		this->weightsSum = new double[numAttributes];
		for(int i = 0; i < numAttributes; i++) // Set sum to zero for all attributes.
		{
			sum[i] = 0;
			weightsSum[i] = 0;
		}
	}
	
	void RSGISCalcZonalWeightedMeanStatsPoly::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(rsgis::img:: RSGISImageCalcException)
	{
		double weightedVal = 0;
		bool found = false;
		bool first = true;
		
		for(int i = 0; i < this->numAttributes; i++) // Loop through attributes
		{
			found = false;
			first = true;
			for(int j = 0; j < attributes[i]->numBands; j++) // Loop through bands
			{
				//std::cout << "Number of bands = " << numBands << std::endl;
				if((attributes[i]->bands[j] > numBands) | (attributes[i]->bands[j] < 0))
				{
					throw rsgis::img:: RSGISImageCalcException("The band attributes do not match the image.");
				}
				// Calculates the minimum of the two input image bands
				if(!boost::math::isnan(attributes[i]->bands[j]))
                {
					if(first)
					{
						if((bandValuesImage[attributes[i]->bands[j]] > attributes[i]->minThresholds[j]) && (bandValuesImage[attributes[i]->bands[j]] < attributes[i]->maxThresholds[j]))
						{
							weightedVal = bandValuesImage[attributes[i]->bands[j]] * interceptArea; // Pixel Value
							found = true;
							first = false;
						}
					}
					else
					{
						if((bandValuesImage[attributes[i]->bands[j]] > attributes[i]->minThresholds[j]) && (bandValuesImage[attributes[i]->bands[j]] < attributes[i]->maxThresholds[j]))
						{
							if(bandValuesImage[attributes[i]->bands[j]] < weightedVal)
							{
								weightedVal = bandValuesImage[attributes[i]->bands[j]] * interceptArea; // Pixel Value
							}
						}
					}
				}

			}
			
			if(found)
			{
				this->sum[i] = this->sum[i] + weightedVal;
				this->weightsSum[i] = this->weightsSum[i] + interceptArea;
			}
		}
		totalPxl++;
	}
	
	double* RSGISCalcZonalWeightedMeanStatsPoly::getOutputValues() throw(rsgis::img:: RSGISImageCalcException)
	{
		//double *outputData = new double[this->numOutputValues];
		outputValues[0] = this->totalPxl;
				
		for(int i = 0; i < this->numAttributes; i++)
		{
			if (weightsSum[i] == 0 || sum[i] == 0) 
			{
				outputValues[i+1] = 0;
			}
			else 
			{
				outputValues[i+1] = sum[i]/weightsSum[i];
			}
		}
		return outputValues;
	}
	
	void RSGISCalcZonalWeightedMeanStatsPoly::reset()
	{
		for(int i = 0; i < numAttributes; i++)
		{
			this->sum[i] = 0;
			this->weightsSum[i] = 0;
		}
		totalPxl = 0;
	}
	
	RSGISCalcZonalWeightedMeanStatsPoly::~RSGISCalcZonalWeightedMeanStatsPoly()
	{
		delete[] sum;
		delete[] weightsSum;
	}
}}


