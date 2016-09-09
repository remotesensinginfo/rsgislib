/*
 *  RSGISZonalCountStats.cpp
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

#include "RSGISZonalCountStats.h"

namespace rsgis{namespace vec{
	
	RSGISZonalCountStats::RSGISZonalCountStats(GDALDataset *image, GDALDataset *rasterFeatures, CountAttributes** attributes, int numAttributes, bool outPxlCount, bool outEmptyCount)
	{
		this->datasets = new GDALDataset*[2];
		this->datasets[0] = rasterFeatures;
		this->datasets[1] = image;
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->outPxlCount = outPxlCount;
		this->outEmptyCount = outEmptyCount;
		
		dataSize = numAttributes + 2; // 0 Pxl Count // 1 Empty Set // 2 ..  n Attribute data
		data = new double[dataSize];
		
		calcValue = new RSGISCalcZonalCountFromRasterPolygon(dataSize, attributes, numAttributes);
		calcImage = new rsgis::img::RSGISCalcImageSingle(calcValue);
	}
	
	void RSGISZonalCountStats::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		try
		{
			calcValue->reset();
			calcImage->calcImageWithinRasterPolygon(datasets, 2, data, env, fid, true);
			
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			int count = 0;
			for(int i = 2; i < dataSize; i++)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex(attributes[count]->name.c_str()), data[i]);
				count++;
			}
			
			if(outPxlCount)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("TotalPxls"), data[0]);
			}
			
			if(outEmptyCount)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("EmptyPxls"), data[1]);
			}
		}
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}
	}
	
	void RSGISZonalCountStats::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISZonalCountStats::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
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
		
		if(outEmptyCount)
		{
			OGRFieldDefn shpField("EmptyPxls", OFTReal);
			shpField.SetPrecision(10);
			if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field \'EmptyPxls\' has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
	}
	
	RSGISZonalCountStats::~RSGISZonalCountStats()
	{
		/*for(int i = 0; i < numAttributes; i++)
		{
			delete attributes[i];
		}*/
		//delete[] attributes;
		delete[] data;
		
		delete calcImage;
		delete calcValue;
		
		delete[] datasets;
	}
	
	
	
	
	RSGISCalcZonalCountFromRasterPolygon::RSGISCalcZonalCountFromRasterPolygon(int numOutputValues, CountAttributes **attributes, int numAttributes) : rsgis::img::RSGISCalcImageSingleValue(numOutputValues)
	{
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->count = new int[numAttributes];
		for(int i = 0; i < numAttributes; i++)
		{
			count[i] = 0;
		}
		this->emptyPxl = 0;
		this->totalPxl = 0;
	}
	
	void RSGISCalcZonalCountFromRasterPolygon::calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISCalcZonalCountFromRasterPolygon::calcImageValue(float *bandValuesImage, int numBands, int band) throw(rsgis::img::RSGISImageCalcException)
	{
		float min = 0;
		bool first = true;
		bool notEmpty = false;
		bool found = false;
		
		for(int i = 0; i < this->numAttributes; i++)
		{
			first = true;
			found = false;
			for(int j = 0; j < attributes[i]->numBands; j++)
			{
				if(attributes[i]->bands[j] > (numBands-1) | attributes[i]->bands[j] < 0)
				{
					throw rsgis::img::RSGISImageCalcException("The band attributes do not match the image.");
				}
				
				if(first)
				{
					if(bandValuesImage[attributes[i]->bands[j]] > attributes[i]->thresholds[j])
					{
						min = bandValuesImage[attributes[i]->bands[j]];
						first = false;
						found = true;
						notEmpty = true;
					}
				}
				else
				{
					if(bandValuesImage[attributes[i]->bands[j]] > attributes[i]->thresholds[j])
					{
						if(bandValuesImage[attributes[i]->bands[j]] < min)
						{
							min = bandValuesImage[attributes[i]->bands[j]];
						}
					}
				}
			}
			
			if(found)
			{
				this->count[i]++;
			}
		}
		
		if(!notEmpty)
		{
			this->emptyPxl++;
		}
		totalPxl++;
	}
	
	void RSGISCalcZonalCountFromRasterPolygon::calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISCalcZonalCountFromRasterPolygon::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not Implemented");
	}
	
	double* RSGISCalcZonalCountFromRasterPolygon::getOutputValues() throw(rsgis::img::RSGISImageCalcException)
	{
		//double *outputData = new double[this->numOutputValues];
		outputValues[0] = this->totalPxl;
		outputValues[1] = this->emptyPxl;
		
		int j = 0;
		for(int i = 2; i < this->numOutputValues; i++)
		{
			outputValues[i] = count[j++];
		}

		return outputValues;
	}
	
	void RSGISCalcZonalCountFromRasterPolygon::reset()
	{
		for(int i = 0; i < numAttributes; i++)
		{
			count[i] = 0;
		}
		totalPxl = 0;
		emptyPxl = 0;
	}
	
	RSGISCalcZonalCountFromRasterPolygon::~RSGISCalcZonalCountFromRasterPolygon()
	{
		delete[] count;
	}
	
}}

