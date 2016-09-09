/*
 *  RSGISClassificationPolygonReader.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2009.
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

#include "RSGISClassificationPolygonReader.h"


namespace rsgis{namespace vec{
	
	RSGISClassificationPolygonReader::RSGISClassificationPolygonReader(std::string classattribute, std::list<rsgis::geom::RSGIS2DPoint*> *data)
	{
		this->classattribute = classattribute;
		vecUtils = new RSGISVectorUtils();
		this->data = data;
	}
	
	void RSGISClassificationPolygonReader::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISClassificationPolygonReader::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		//cout << "FID: " << fid << endl;
		// Set up the Species Polygon object and add to Delunay triangulation.
		
		
		OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
			
		OGRFeatureDefn *featureDefn = feature->GetDefnRef();
		
		int classattributeIdx = featureDefn->GetFieldIndex(this->classattribute.c_str());
		if(classattributeIdx < 0)
		{
			std::string message = "This layer does not contain a field with the name \'" + this->classattribute + "\'";
			throw RSGISVectorException(message.c_str());
		}
		std::string classValue = std::string(feature->GetFieldAsString(classattributeIdx));
		
		if( geometryType == wkbPolygon )
		{
			RSGISClassificationPolygon *tmpSpeciesPoly = new RSGISClassificationPolygon();
			tmpSpeciesPoly->setClassification(classValue);
			OGRPolygon *polygon = (OGRPolygon *) feature->GetGeometryRef();
			geos::geom::Polygon *geosPoly = vecUtils->convertOGRPolygon2GEOSPolygon(polygon);
			tmpSpeciesPoly->setPolygon(geosPoly);
			data->push_back(tmpSpeciesPoly);
		} 
		else if( geometryType == wkbMultiPolygon )
		{
			RSGISClassificationPolygon *tmpSpeciesPoly = NULL;
			
            rsgis::geom::RSGISGeometry geomUtils;
			OGRMultiPolygon *mPolygon = (OGRMultiPolygon *) feature->GetGeometryRef();
			//cout << polygon->exportToGML() << endl;
			geos::geom::MultiPolygon *mGEOSPolygon = vecUtils->convertOGRMultiPolygonGEOSMultiPolygon(mPolygon);
			std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
			geomUtils.retrievePolygons(mGEOSPolygon, polys);
			std::vector<geos::geom::Polygon*>::iterator iterPolys = polys->begin();
			while(iterPolys != polys->end())
			{
				tmpSpeciesPoly = new RSGISClassificationPolygon();
				tmpSpeciesPoly->setClassification(classValue);
				tmpSpeciesPoly->setPolygon(*iterPolys);
				data->push_back(tmpSpeciesPoly);
				iterPolys++;
			}
			delete polys;
		} 
		else
		{
			std::string message = std::string("Unsupport data type: ") + std::string(feature->GetGeometryRef()->getGeometryName());
			throw RSGISVectorException(message);
		}
		
		
	}
	
	void RSGISClassificationPolygonReader::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		// Nothing to do!
	}
	
	RSGISClassificationPolygonReader::~RSGISClassificationPolygonReader()
	{
		delete vecUtils;
	}
}}




