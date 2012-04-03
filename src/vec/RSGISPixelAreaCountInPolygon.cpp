/*
 *  RSGISPixelAreaCountInPolygon.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/03/2009.
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

#include "RSGISPixelAreaCountInPolygon.h"

namespace rsgis{namespace vec{
	
	RSGISPixelAreaCountInPolygon::RSGISPixelAreaCountInPolygon(GDALDataset *rasterFeatures)
	{
		this->datasets = new GDALDataset*[1];
		this->datasets[0] = rasterFeatures;
		
		double *transformation = new double[6];
		datasets[0]->GetGeoTransform(transformation);
		pxlresolution = transformation[1];
		delete[] transformation;
		
		cout << "Pixel resolution = " << pxlresolution << endl;
		
		dataSize = 1; // 0 Pxl Count
		data = new double[dataSize];
		
		calcValue = new RSGISCalcPixelCountFromRasterPolygon(dataSize);
		calcImage = new RSGISCalcImageSingle(calcValue);
	}
	
	void RSGISPixelAreaCountInPolygon::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		calcValue->reset();
		calcImage->calcImageWithinRasterPolygon(datasets, 1, data, env, fid, true);
		
		OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
		outFeature->SetField(outFeatureDefn->GetFieldIndex("TotalPxls"), data[0]);
		outFeature->SetField(outFeatureDefn->GetFieldIndex("Area"), (data[0]*pxlresolution));
	}
	
	void RSGISPixelAreaCountInPolygon::processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISPixelAreaCountInPolygon::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		OGRFieldDefn shpField_totalpxl("TotalPxls", OFTReal);
		shpField_totalpxl.SetPrecision(10);
		if( outputLayer->CreateField( &shpField_totalpxl ) != OGRERR_NONE )
		{
			string message = string("Creating shapefile field \'TotalPxls\' has failed");
			throw RSGISVectorOutputException(message.c_str());
		}
		
		OGRFieldDefn shpField_area("Area", OFTReal);
		shpField_area.SetPrecision(10);
		if( outputLayer->CreateField( &shpField_area ) != OGRERR_NONE )
		{
			string message = string("Creating shapefile field \'Area\' has failed");
			throw RSGISVectorOutputException(message.c_str());
		}
	}
	
	RSGISPixelAreaCountInPolygon::~RSGISPixelAreaCountInPolygon()
	{
		delete[] data;
		
		delete calcImage;
		delete calcValue;
	}
	
	
	
	
	RSGISCalcPixelCountFromRasterPolygon::RSGISCalcPixelCountFromRasterPolygon(int numOutputValues) : RSGISCalcImageSingleValue(numOutputValues)
	{
		this->totalPxl = 0;
	}
	
	void RSGISCalcPixelCountFromRasterPolygon::calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISCalcPixelCountFromRasterPolygon::calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException)
	{
		totalPxl++;
	}
	
	void RSGISCalcPixelCountFromRasterPolygon::calcImageValue(float *bandValuesImage, int numBands, Envelope *extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISCalcPixelCountFromRasterPolygon::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	double* RSGISCalcPixelCountFromRasterPolygon::getOutputValues() throw(RSGISImageCalcException)
	{
		outputValues[0] = this->totalPxl;
		
		return outputValues;
	}
	
	void RSGISCalcPixelCountFromRasterPolygon::reset()
	{
		totalPxl = 0;
	}
	
	RSGISCalcPixelCountFromRasterPolygon::~RSGISCalcPixelCountFromRasterPolygon()
	{

	}
	
}}



