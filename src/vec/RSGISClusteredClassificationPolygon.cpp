/*
 *  RSGISClusteredClassificationPolygon.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/08/2009.
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

#include "RSGISClusteredClassificationPolygon.h"


namespace rsgis{namespace vec{
	
	
	RSGISClusteredClassificationPolygon::RSGISClusteredClassificationPolygon() : RSGISPolygonData()
	{
		this->area = 0;
		this->polysArea = 0;
		this->proPolysArea = 0;
		this->proTotalArea = 0;
		this->edgeThreshold = 0;
		this->clusterClass = "";
	}
	
	RSGISClusteredClassificationPolygon::RSGISClusteredClassificationPolygon(RSGISClusteredClassificationPolygon *data) : RSGISPolygonData()
	{
		this->area = data->area;
		this->polysArea = data->polysArea;
		this->proPolysArea = data->proPolysArea;
		this->proTotalArea = data->proTotalArea;
		this->edgeThreshold = data->edgeThreshold;
		this->clusterClass = data->clusterClass;
		this->polygonType = data->polygonType;
		if(this->polygonType)
		{
			this->polygonGeom = dynamic_cast<geos::geom::Polygon*>(data->polygonGeom->clone());
		}
		else
		{
			this->multiPolygonGeom = dynamic_cast<geos::geom::MultiPolygon*>(data->multiPolygonGeom->clone());
		}
		this->envelope = new geos::geom::Envelope(*data->envelope);
	}
	
	void RSGISClusteredClassificationPolygon::readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
        rsgis::math::RSGISMathsUtils mathUtils;
		
		// Read the class attribute
		OGRFieldDefn *fieldDef = NULL;
		std::string columnName = "";
		int fieldCount = featDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			fieldDef = feature->GetFieldDefnRef(i);
			columnName = fieldDef->GetNameRef();
			if(columnName == "ClustClass")
			{
				this->clusterClass = std::string(feature->GetFieldAsString(i));
			}
			else if(columnName == "Area")
			{
				this->area = mathUtils.strtofloat(std::string(feature->GetFieldAsString(i)));
			}
			else if(columnName == "PolysArea")
			{
				this->polysArea = mathUtils.strtofloat(std::string(feature->GetFieldAsString(i)));
			}
			else if(columnName == "ProPolysAr")
			{
				this->proPolysArea = mathUtils.strtofloat(std::string(feature->GetFieldAsString(i)));
			}
			else if(columnName == "ProTotalAr")
			{
				this->proTotalArea = mathUtils.strtofloat(std::string(feature->GetFieldAsString(i)));
			}
			else if(columnName == "EdgeThres")
			{
				this->edgeThreshold = mathUtils.strtofloat(std::string(feature->GetFieldAsString(i)));
			}
		}
	}
	
	void RSGISClusteredClassificationPolygon::createLayerDefinition(OGRLayer *outputSHPLayer)
	{
		// Create class attribute column
		OGRFieldDefn shpField1( "ClustClass", OFTString );
		shpField1.SetWidth(100);
		if( outputSHPLayer->CreateField( &shpField1 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"ClustClass\" has failed");
		}
		
		OGRFieldDefn shpField2( "Area", OFTReal );
		shpField2.SetPrecision(4);
		if( outputSHPLayer->CreateField( &shpField2 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"Area\" has failed");
		}
		
		OGRFieldDefn shpField3( "PolysArea", OFTReal );
		shpField3.SetPrecision(4);
		if( outputSHPLayer->CreateField( &shpField3 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"PolysArea\" has failed");
		}
		
		OGRFieldDefn shpField4( "ProPolysAr", OFTReal );
		shpField4.SetPrecision(4);
		if( outputSHPLayer->CreateField( &shpField4 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"ProPolysAr\" has failed");
		}
		
		OGRFieldDefn shpField5( "ProTotalAr", OFTReal );
		shpField5.SetPrecision(4);
		if( outputSHPLayer->CreateField( &shpField5 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"ProTotalAr\" has failed");
		}
		
		OGRFieldDefn shpField6( "EdgeThres", OFTReal );
		shpField6.SetPrecision(4);
		if( outputSHPLayer->CreateField( &shpField6 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"EdgeThreshold\" has failed");
		}
	}
	
	void RSGISClusteredClassificationPolygon::populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
		RSGISVectorUtils vecUtils;
		
		if(polygonType)
		{
			feature->SetGeometryDirectly(vecUtils.convertGEOSPolygon2OGRPolygon(polygonGeom));
		}
		else
		{
			feature->SetGeometryDirectly(vecUtils.convertGEOSMultiPolygon2OGRMultiPolygon(multiPolygonGeom));
		}
		
		// Set class
		feature->SetField(featDefn->GetFieldIndex("ClustClass"), clusterClass.c_str());
		feature->SetField(featDefn->GetFieldIndex("Area"), area);
		feature->SetField(featDefn->GetFieldIndex("PolysArea"), polysArea);
		feature->SetField(featDefn->GetFieldIndex("ProPolysAr"), proPolysArea);
		feature->SetField(featDefn->GetFieldIndex("ProTotalAr"), proTotalArea);
		feature->SetField(featDefn->GetFieldIndex("EdgeThres"), edgeThreshold);
	}
	
	void RSGISClusteredClassificationPolygon::setArea(float area)
	{
		this->area = area;
	}
	
	float RSGISClusteredClassificationPolygon::getArea()
	{
		return this->area;
	}
	
	void RSGISClusteredClassificationPolygon::setPolysArea(float polysArea)
	{
		this->polysArea = polysArea;
	}
	
	float RSGISClusteredClassificationPolygon::getPolysArea()
	{
		return this->polysArea;
	}
	
	void RSGISClusteredClassificationPolygon::setProPolysArea(float proPolysArea)
	{
		this->proPolysArea = proPolysArea;
	}
	
	float RSGISClusteredClassificationPolygon::getProPolysArea()
	{
		return this->proPolysArea;
	}
	
	void RSGISClusteredClassificationPolygon::setProTotalAr(float proTotalArea)
	{
		this->proTotalArea = proTotalArea;
	}
	
	float RSGISClusteredClassificationPolygon::getProTotalAr()
	{
		return this->proTotalArea;
	}
	
	void RSGISClusteredClassificationPolygon::setEdgeThreshold(float edgeThreshold)
	{
		this->edgeThreshold = edgeThreshold;
	}
	
	float RSGISClusteredClassificationPolygon::getEdgeThreshold()
	{
		return this->edgeThreshold;
	}
	
	void RSGISClusteredClassificationPolygon::setClusterClass(std::string name)
	{
		this->clusterClass = name;
	}
	
	std::string RSGISClusteredClassificationPolygon::getClusterClass()
	{
		return this->clusterClass;
	}
		
	RSGISClusteredClassificationPolygon::~RSGISClusteredClassificationPolygon()
	{
		
	}
}}



