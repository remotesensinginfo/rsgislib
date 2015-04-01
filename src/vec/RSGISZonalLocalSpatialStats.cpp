/*
 *  RSGISZonalLocalSpatialStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 29/06/2010.
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

#include "RSGISZonalLocalSpatialStats.h"

namespace rsgis{namespace vec{

	RSGISZonalLSSMeanVar::RSGISZonalLSSMeanVar(GDALDataset *image, MeanAttributes** attributes, int numAttributes, bool outPxlCount, int winSize, double offsetSize, rsgis::img::pixelInPolyOption method)
	{
		this->datasets = new GDALDataset*[1];
		this->datasets[0] = image;
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->outPxlCount = outPxlCount;
		this->attributesVar = new std::string[numAttributes];
		this->method = method;
		
		dataSize = numAttributes + 1; // 0 Pxl Count // 1 ..  n Attribute data
		averages = new double[dataSize];
		this->looksX = (winSize - 1) / 2;
		this->looksY = (winSize - 1) / 2;
		this->offsetSize = offsetSize;
		this->numOffsets = winSize * winSize;
		this->offsetAverages = new double* [numOffsets];
		for(int i = 0; i < numOffsets; ++i)
		{
			offsetAverages[i] = new double[dataSize];
		}
		
		calcValueCentre = new RSGISCalcZonalMeanFromPolygon(dataSize, attributes, numAttributes);
		calcValueSurrounding = new RSGISCalcZonalMeanFromPolygon(dataSize, attributes, numAttributes);
		//calcValueCentre = new RSGISCalcZonalWeightedMeanStatsPoly(dataSize, attributes, numAttributes); // Use weighted mean
		//calcValueSurrounding = new RSGISCalcZonalWeightedMeanStatsPoly(dataSize, attributes, numAttributes); - Use weighed mean
		calcImageCentre = new rsgis::img::RSGISCalcImageSingle(calcValueCentre);
		calcImageSurrounding = new rsgis::img::RSGISCalcImageSingle(calcValueSurrounding);
	}
	
	void RSGISZonalLSSMeanVar::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		try
		{
			
			RSGISVectorUtils vecUtils;
			calcValueCentre->reset(); // Reset counter for centre pixel
			calcValueSurrounding->reset(); // Reset counter for surrounding areas.
			
			OGRPolygon *inOGRPoly;
            geos::geom::Polygon *poly;
			inOGRPoly = (OGRPolygon *) inFeature->GetGeometryRef();
			poly = vecUtils.convertOGRPolygon2GEOSPolygon(inOGRPoly);
			
			// Calculate for center
			calcImageCentre->calcImageWithinPolygon(datasets, 1, averages, env, poly, true, method); // Use weighted mean
			//calcImageCentre->calcImageWithinPolygon(datasets, 1, averages, env, poly, true, envelope);
			
			unsigned int off = 0; // counter for number of offsets
			// Calculate for surrounding areas
			for (double x = 0 - looksX; x <= 0 + looksX; x++) // Move in x direction
			{
				for (double y = 0 - looksY; y <= 0 + looksY; y++) // Move in y direction
				{
					if((x == 0) & (y == 0)) // For centre pixel use previously calculated average
					{
						for (int i = 1; i < dataSize; i++) 
						{
							offsetAverages[off][i] = averages[i];
						}
						off++;
					}
					else 
					{
						OGRPolygon *offsetOGRPoly;
						geos::geom::Polygon *offsetPoly;
						geos::geom::Envelope *offsetEnv;
						offsetOGRPoly = vecUtils.moveOGRPolygon(inOGRPoly, x * offsetSize, y * offsetSize, 0);
						offsetPoly = vecUtils.convertOGRPolygon2GEOSPolygon(offsetOGRPoly);
						offsetEnv = vecUtils.getEnvelope(offsetOGRPoly);
						calcImageSurrounding->calcImageWithinPolygon(datasets, 1, offsetAverages[off], offsetEnv, offsetPoly, true, method);// Use weighted mean
						//calcImageSurrounding->calcImageWithinPolygon(datasets, 1, offsetAverages[off], offsetEnv, offsetPoly, true, envelope);						
						off++;
					}
				}
			}
			
			// Calculate mean
			double *tempAverage = new double[dataSize];
			for(int i = 1; i < this->dataSize; i++) // Loop through number of attributes
			{
				double tempSum = 0;
				for (int j = 0; j < numOffsets; j++) // Loop through averages at each polygon offset
				{
					tempSum = tempSum + offsetAverages[j][i]; 
				}
				tempAverage[i] = tempSum / numOffsets;
			}
			
			// Calculate variance
			// Var = Sum(x - x')^2 / N
			double *outputVar = new double[dataSize];
			for(int i = 1; i < this->dataSize; i++) // Loop through number of attributes
			{
				double varSum = 0;
				for (int j = 0; j < numOffsets; j++) // Loop through averages at each polygon offset
				{
					//std::cout << "Centre average = " << averages[i] << " surrounding averge = " << offsetAverages[j][i] << std::endl;
					varSum = varSum + pow(offsetAverages[j][i] - tempAverage[i],2); // (xi - x')^2
				}
				outputVar[i] = varSum / numOffsets;
			}
			
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			int count = 0;
			for(int i = 1; i < dataSize; i++)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex(attributes[count]->name.c_str()), averages[i]);
				outFeature->SetField(outFeatureDefn->GetFieldIndex(attributesVar[count].c_str()), outputVar[i]);
				count++;
			}
			
			if(outPxlCount)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("TotalPxls"), averages[0]);
			}
			delete[] outputVar;
			delete[] tempAverage;
		}
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}
	}
	
	void RSGISZonalLSSMeanVar::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		for(int i = 0; i < numAttributes; i++)
		{
			if(attributes[i]->name.length() > 7)
			{
				std::cout << attributes[i]->name << " will be truncated to \'" << attributes[i]->name.substr(0, 7) << "\'\n";
				attributes[i]->name = attributes[i]->name.substr(0, 7);
			}
			OGRFieldDefn shpField(attributes[i]->name.c_str(), OFTReal);
			shpField.SetPrecision(10);
			if(outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field ") + attributes[i]->name + std::string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
			// Create field for variance on mean
			attributesVar[i] = attributes[i]->name + std::string("Var");
			OGRFieldDefn shpVarField(attributesVar[i].c_str(), OFTReal);
			shpVarField.SetPrecision(10);
			if(outputLayer->CreateField( &shpVarField) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field ") + attributesVar[i] + std::string(" has failed");
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
	
	RSGISZonalLSSMeanVar::~RSGISZonalLSSMeanVar()
	{
		delete[] attributesVar;
		delete[] datasets;
		delete[] averages;
		delete calcValueCentre;
		delete calcImageCentre;
		delete calcValueSurrounding;
		delete calcImageSurrounding;
	}
	
	
	RSGISCalcZonalLSSMeanVar::RSGISCalcZonalLSSMeanVar(int numOutputValues, MeanAttributes **attributes, int numAttributes) : rsgis::img::RSGISCalcImageSingleValue(numOutputValues)
	{
		this->attributes = attributes; 
		this->numAttributes = numAttributes;
		this->sum = new double[numAttributes];
		for(int i = 0; i < numAttributes; i++) // Set sum to zero for all attributes.
		{
			sum[i] = 0;
		}
	}
	
	void RSGISCalcZonalLSSMeanVar::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(rsgis::img::RSGISImageCalcException)
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
					throw rsgis::img::RSGISImageCalcException("The band attributes do not match the image.");
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
			}
		}
		totalPxl++;
	}
	
	double* RSGISCalcZonalLSSMeanVar::getOutputValues() throw(rsgis::img::RSGISImageCalcException)
	{
		//double *outputData = new double[this->numOutputValues];
		outputValues[0] = this->totalPxl;
		
		int j = 0;
		for(int i = 1; i < this->numOutputValues; i++)
		{
			outputValues[i] = sum[j++]/totalPxl;
		}
		return outputValues;
	}
	
	void RSGISCalcZonalLSSMeanVar::reset()
	{
		for(int i = 0; i < numAttributes; i++)
		{
			sum[i] = 0;
		}
		totalPxl = 0;
	}
	
	RSGISCalcZonalLSSMeanVar::~RSGISCalcZonalLSSMeanVar()
	{
		delete[] sum;
	}
	
	
}}
