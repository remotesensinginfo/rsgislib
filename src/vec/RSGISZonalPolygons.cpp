/*
 *  RSGISZonalPolygons.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 30/07/2008.
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

#include "RSGISZonalPolygons.h"

namespace rsgis{namespace vec{
	

	RSGISZonalPolygons::RSGISZonalPolygons(int numBands, bool **toCalc)  : RSGISPolygonData()
	{
		this->mean = new double[numBands];
		this->min = new double[numBands];
		this->max = new double[numBands];
		this->stddev = new double[numBands];
		this->numBands = numBands;
		this->toCalc = toCalc;
	}
	
	void RSGISZonalPolygons::readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
		// There are no attributes to read.
	}
	
	void RSGISZonalPolygons::createLayerDefinition(OGRLayer *outputSHPLayer)throw(RSGISVectorOutputException)
	{
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
	
	void RSGISZonalPolygons::populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
		RSGISVectorUtils vecUtils;
		OGRPolygon *polygon = NULL;
		OGRMultiPolygon *mPolygon = NULL;
		
		if(polygonType)
		{
			polygon = vecUtils.convertGEOSPolygon2OGRPolygon(polygonGeom);
			feature->SetGeometry(polygon);
		}
		else
		{
			mPolygon = vecUtils.convertGEOSMultiPolygon2OGRMultiPolygon(multiPolygonGeom);
			feature->SetGeometry(mPolygon);
		}
		
		for(int i = 0; i < numBands; i++)
		{
			if(toCalc[i][0])
			{
				char colname[8];
				sprintf(colname, "mean_%d", i);
				feature->SetField(featDefn->GetFieldIndex(colname), mean[i]);
			}
			if(toCalc[i][1])
			{
				char colname[7];
				sprintf(colname, "max_%d", i);
				feature->SetField(featDefn->GetFieldIndex(colname), max[i]);
			}
			if(toCalc[i][2])
			{
				char colname[7];
				sprintf(colname, "min_%d", i);
				feature->SetField(featDefn->GetFieldIndex(colname), min[i]);
			}
			if(toCalc[i][3])
			{
				char colname[10];
				sprintf(colname, "stddev_%d", i);
				feature->SetField(featDefn->GetFieldIndex(colname), stddev[i]);
			}
		}
	}
	
	void RSGISZonalPolygons::setMean(double mean, int index)
	{
		this->mean[index] = mean;
	}
	
	void RSGISZonalPolygons::setMin(double min, int index)
	{
		this->min[index] = min;
	}
	
	void RSGISZonalPolygons::setMax(double max, int index)
	{
		this->max[index] = max;
	}
	
	void RSGISZonalPolygons::setStdDev(double stddev, int index)
	{
		this->stddev[index] = stddev;
	}
	
	double* RSGISZonalPolygons::getMean()
	{
		return mean;
	}
	
	double* RSGISZonalPolygons::getMax()
	{
		return max;
	}
	
	double* RSGISZonalPolygons::getMin()
	{
		return min;
	}
	
	double* RSGISZonalPolygons::getStdDev()
	{
		return stddev;
	}
	
	bool RSGISZonalPolygons::calcStdDev(int band)
	{
		if(band < numBands)
		{
			return toCalc[band][3];
		}
		return true;
	}
	
	RSGISZonalPolygons::~RSGISZonalPolygons()
	{
		delete[] mean;
		delete[] min;
		delete[] max;
		delete[] stddev;
	}
}}

