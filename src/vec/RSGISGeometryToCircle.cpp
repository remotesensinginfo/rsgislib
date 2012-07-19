/*
 *  RSGISGeometryToCircle.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/06/2009.
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

#include "RSGISGeometryToCircle.h"

namespace rsgis{namespace vec{
	
	RSGISGeometryToCircle::RSGISGeometryToCircle(float resolution, std::string areaAttribute, std::string radiusAttribute, float radiusInValue)
	{
		this->resolution = resolution;
		this->areaAttribute = areaAttribute;
		this->radiusAttribute = radiusAttribute;
		this->radiusInValue = radiusInValue;
		
		this->geomUtils = new rsgis::geom::RSGISGeometry();
		this->vecUtils = new RSGISVectorUtils();
	}
	
	void RSGISGeometryToCircle::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		OGRwkbGeometryType geometryType = inFeature->GetGeometryRef()->getGeometryType();
		OGRFeatureDefn *inFeatureDefn = inFeature->GetDefnRef();
		
		int areaAttributeIdx = inFeatureDefn->GetFieldIndex(areaAttribute.c_str());
		int radiusAttributeIdx = inFeatureDefn->GetFieldIndex(radiusAttribute.c_str());
		
		float xCentre = 0;
		float yCentre = 0;
		float radiusValue = 0;
		float lineLength = 0;
		
		calcOptions calcOption = calc;
		if(radiusAttributeIdx >= 0)
		{
			calcOption = radius;
		}
		else if(areaAttributeIdx >= 0)
		{
			calcOption = area;
		}
		else if(radiusInValue > 0)
		{
			calcOption = radiusfixed;
		}
		else
		{
			calcOption = calc;
		}
		
		// Calculate the centre coordinates
		if( geometryType == wkbPolygon )
		{
			OGRPolygon *polygon = (OGRPolygon *) inFeature->GetGeometryRef();
			OGRPoint *point = new OGRPoint();
			polygon->PointOnSurface(point);
			xCentre = point->getX();
			yCentre = point->getY();
			delete point;
		} 
		else if( geometryType == wkbMultiPolygon )
		{
			OGRMultiPolygon *multiPolygon = (OGRMultiPolygon *) inFeature->GetGeometryRef();
			geos::geom::MultiPolygon *geosMultiPolygon = vecUtils->convertOGRMultiPolygonGEOSMultiPolygon(multiPolygon);
			geos::geom::Point *point = geosMultiPolygon->getCentroid();
			xCentre = point->getX();
			yCentre = point->getY();
			delete point;
			delete geosMultiPolygon;
		}
		else if( geometryType == wkbPoint )
		{
			OGRPoint *point = (OGRPoint *) inFeature->GetGeometryRef();
			//std::cout << "Point: " << point->exportToGML() << std::endl;
			xCentre = point->getX();
			yCentre = point->getY();
		}	
		else if( geometryType == wkbLineString )
		{
			OGRLineString *line = (OGRLineString *) inFeature->GetGeometryRef();
			OGRPoint *point = new OGRPoint();
			lineLength = line->get_Length();
			line->Value((lineLength/2), point);
			xCentre = point->getX();
			yCentre = point->getY();
			delete point;
		}
		else
		{
			throw RSGISVectorException("Unsupport data type.");
		}
		
		
		// Calculate or read the radius
		if(calcOption == radius)
		{
			radiusValue = inFeature->GetFieldAsDouble(this->radiusAttribute.c_str());
		}
		else if(calcOption == area)
		{
			float areaValue = inFeature->GetFieldAsDouble(this->areaAttribute.c_str());
			radiusValue = sqrt(areaValue/M_PI);
		}
		else if(calcOption == radiusfixed)
		{
			radiusValue = radiusInValue;
		}
		else if(calcOption == calc)
		{
			if( geometryType == wkbPolygon )
			{
				OGRPolygon *polygon = (OGRPolygon *) inFeature->GetGeometryRef();
				radiusValue = sqrt(polygon->get_Area()/M_PI);
			} 
			else if( geometryType == wkbMultiPolygon )
			{
				OGRMultiPolygon *multiPolygon = (OGRMultiPolygon *) inFeature->GetGeometryRef();
				radiusValue = sqrt(multiPolygon->get_Area()/M_PI);
			}
			else if( geometryType == wkbPoint )
			{
				throw RSGISVectorException("No information has been provided to calculate radius");
			}	
			else if( geometryType == wkbLineString )
			{
				OGRLineString *line = (OGRLineString *) inFeature->GetGeometryRef();
				radiusValue = line->get_Length()/2;
			}
			else
			{
				throw RSGISVectorException("Unsupport data type.");
			}			
		}
		else
		{
			throw RSGISVectorException("Could not calculate circle");
		}
		
		
		
		if(((boost::math::isinf)(xCentre)) | ((boost::math::isinf)(yCentre)) | ((boost::math::isnan)(xCentre)) | ((boost::math::isnan)(yCentre)))
		{
			std::cout << "Centre [" << xCentre << "," << yCentre << "]: " << radiusValue << std::endl;
			throw RSGISVectorException("Invalid number for circle centre.");
		}
		
		// create circle and set output geom
		try 
		{
			geos::geom::Polygon *geosPolygon = geomUtils->createCircle(xCentre, yCentre, radiusValue, this->resolution);
			//std::cout << "Circle: " << geosPolygon->toText() << std::endl;
			OGRPolygon *ogrPolygon = vecUtils->convertGEOSPolygon2OGRPolygon(geosPolygon);
			outFeature->SetGeometry(ogrPolygon);
		}
		catch (rsgis::geom::RSGISGeometryException &e) 
		{
			throw RSGISVectorException(e.what());
		}
		
	}
	
	void RSGISGeometryToCircle::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISGeometryToCircle::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		
	}
	
	RSGISGeometryToCircle::~RSGISGeometryToCircle()
	{
		delete this->geomUtils;
		delete this->vecUtils;
	}
}}



