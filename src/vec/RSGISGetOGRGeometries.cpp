/*
 *  RSGISGetOGRGeometries.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/08/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
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

#include "RSGISGetOGRGeometries.h"

namespace rsgis{namespace vec{
	

    RSGISGetOGRGeometries::RSGISGetOGRGeometries(std::vector<OGRGeometry*> *geometries):RSGISProcessOGRFeature()
    {
        this->geometries = geometries;
    }
    
	void RSGISGetOGRGeometries::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
    {
        throw RSGISVectorException("Not implemented..");
    }
    
	void RSGISGetOGRGeometries::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
    {
        geometries->push_back(feature->GetGeometryRef()->clone());
    }
		
    void RSGISGetOGRGeometries::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
    {
        // Do nothing...
    }
    
    RSGISGetOGRGeometries::~RSGISGetOGRGeometries()
    {
        
    }
    
    
    RSGISPrintGeometryToConsole::RSGISPrintGeometryToConsole():RSGISProcessOGRFeature()
    {
        std::cout.precision(12);
    }
    
	void RSGISPrintGeometryToConsole::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
    {
        throw RSGISVectorException("Not implemented..");
    }
    
	void RSGISPrintGeometryToConsole::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
    {
        OGRGeometry *geometry = feature->GetGeometryRef();
		if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
		{
            std::cout << "Polygon:\n";
			OGRPolygon *poly = (OGRPolygon *) geometry;
            
            std::cout << "\tExterior Ring:\n\t";
            this->printRing(poly->getExteriorRing());
            unsigned int numInternalRings = poly->getNumInteriorRings();
            for(unsigned int i = 0; i < numInternalRings; ++i)
            {
                std::cout << "\tInternal Ring(" << i << "):\n\t";
                this->printRing(poly->getInteriorRing(i));
            }
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiPolygon )
		{
            std::cout << "Multi-Polygon:\n";
			OGRMultiPolygon *mPoly = (OGRMultiPolygon *) geometry;
            OGRPolygon *poly = NULL;
            
            unsigned int numPolys = mPoly->getNumGeometries();
            for(unsigned int n = 0; n < numPolys; ++n)
            {
                std::cout << "\tPolygon (" << n << "):\n";
                poly = (OGRPolygon *) mPoly->getGeometryRef(n);
                std::cout << "\t\tExterior Ring:\n\t\t";
                this->printRing(poly->getExteriorRing());
                unsigned int numInternalRings = poly->getNumInteriorRings();
                for(unsigned int i = 0; i < numInternalRings; ++i)
                {
                    std::cout << "\t\tInternal Ring(" << i << "):\n\t\t";
                    this->printRing(poly->getInteriorRing(i));
                }
            }
            std::cout << std::endl;
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPoint )
		{
            std::cout << "Point: ";
			OGRPoint *point = (OGRPoint *) geometry;
			
            std::cout << "[" << point->getX() << ", " << point->getY() << ", " << point->getZ() << "]\n";
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbLineString )
		{
			throw RSGISVectorException("Polylines not implemented yet.");
		}
		else if(geometry != NULL)
		{
			std::string message = std::string("Unsupported data type: ") + std::string(geometry->getGeometryName());
			throw RSGISVectorException(message);
		}
		else
		{
			throw RSGISVectorException("WARNING: NULL Geometry Present within input file");
		}
    }
    
    void RSGISPrintGeometryToConsole::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
    {
        // Do nothing...
    }
    
    void RSGISPrintGeometryToConsole::printRing(OGRLinearRing *inGeomRing) throw(RSGISVectorOutputException)
    {
        unsigned int numPts = inGeomRing->getNumPoints();
        std::cout << "Line (" << numPts << "):\t";
        OGRPoint *point = new OGRPoint();
        for(unsigned int i = 0; i < numPts; ++i)
        {
            inGeomRing->getPoint(i, point);
            if(i == 0)
            {
                std::cout << "[" << point->getX() << ", " << point->getY() << ", " << point->getZ() << "]";
            }
            else
            {
                std::cout << ", [" << point->getX() << ", " << point->getY() << ", " << point->getZ() << "]";
            }
        }
        std::cout << std::endl;
    }
    
    RSGISPrintGeometryToConsole::~RSGISPrintGeometryToConsole()
    {
        
    }
    
    
    RSGISOGRPointReader::RSGISOGRPointReader(std::vector<OGRPoint*> *points)
    {
        this->points = points;
    }
    
    void RSGISOGRPointReader::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
    {
        throw RSGISVectorException("Not implemented..");
    }
    
    void RSGISOGRPointReader::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
    {
        OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
        
        if( geometryType == wkbPoint )
        {
            OGRPoint *point = (OGRPoint *) feature->GetGeometryRef()->clone();
            points->push_back(point);
        }
        else
        {
            std::string message = std::string("Unsupport data type: ") + std::string(feature->GetGeometryRef()->getGeometryName());
            throw RSGISVectorException(message);
        }
    }
    
    void RSGISOGRPointReader::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
    {
        // Nothing to do!
    }
    
    RSGISOGRPointReader::~RSGISOGRPointReader()
    {
        
    }

    
    RSGISOGRLineReader::RSGISOGRLineReader(std::vector<OGRLineString*> *lines)
    {
        this->lines = lines;
    }
    
    void RSGISOGRLineReader::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
    {
        throw RSGISVectorException("Not implemented..");
    }
    
    void RSGISOGRLineReader::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
    {
        OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
        
        if( geometryType == wkbLineString )
        {
            OGRLineString *line = (OGRLineString *) feature->GetGeometryRef()->clone();
            lines->push_back(line);
        }
        else
        {
            std::string message = std::string("Unsupport data type: ") + std::string(feature->GetGeometryRef()->getGeometryName());
            throw RSGISVectorException(message);
        }
    }
    
    void RSGISOGRLineReader::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
    {
        // Nothing to do!
    }
    
    RSGISOGRLineReader::~RSGISOGRLineReader()
    {
        
    }
    
    
    
}}
