/*
 *  RSGISPolygonReader.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/07/2009.
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

#include "RSGISPolygonReader.h"


namespace rsgis{namespace vec{
	
	RSGISPolygonReader::RSGISPolygonReader(list<RSGIS2DPoint*> *data)
	{
		vecUtils = new RSGISVectorUtils();
		this->dataList = data;
		this->listtype = true;
	}
	
	RSGISPolygonReader::RSGISPolygonReader(vector<RSGIS2DPoint*> *data)
	{
		vecUtils = new RSGISVectorUtils();
		this->dataVector= data;
		this->listtype = false;
	}
	
	void RSGISPolygonReader::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISPolygonReader::processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		
		OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
		if( geometryType == wkbPolygon )
		{
			RSGISPolygon *tmpPoly = new RSGISPolygon();
			OGRPolygon *polygon = (OGRPolygon *) feature->GetGeometryRef();
			tmpPoly->setPolygon(vecUtils->convertOGRPolygon2GEOSPolygon(polygon));
			if(listtype)
			{
				dataList->push_back(tmpPoly);
			}
			else
			{
				dataVector->push_back(tmpPoly);
			}
		} 
		else if( geometryType == wkbMultiPolygon )
		{
			RSGISPolygon *tmpPoly = NULL;
			
			RSGISGeometry geomUtils;
			OGRMultiPolygon *mPolygon = (OGRMultiPolygon *) feature->GetGeometryRef();
			//cout << polygon->exportToGML() << endl;
			MultiPolygon *mGEOSPolygon = vecUtils->convertOGRMultiPolygonGEOSMultiPolygon(mPolygon);
			vector<Polygon*> *polys = new vector<Polygon*>();
			geomUtils.retrievePolygons(mGEOSPolygon, polys);
			vector<Polygon*>::iterator iterPolys = polys->begin();
			while(iterPolys != polys->end())
			{
				tmpPoly = new RSGISPolygon();
				tmpPoly->setPolygon(*iterPolys);
				if(listtype)
				{
					dataList->push_back(tmpPoly);
				}
				else
				{
					dataVector->push_back(tmpPoly);
				}
				iterPolys++;
			}
			delete polys;
		} 
		else
		{
			string message = string("Unsupport data type: ") + string(feature->GetGeometryRef()->getGeometryName());
			throw RSGISVectorException(message);
		}
		
	}
	
	void RSGISPolygonReader::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		// Nothing to do!
	}
	
	RSGISPolygonReader::~RSGISPolygonReader()
	{
		delete vecUtils;
	}
}}





