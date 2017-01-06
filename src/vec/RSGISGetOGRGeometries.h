/*
 *  RSGISGetOGRGeometries.h
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

#ifndef RSGISGetOGRGeometries_H
#define RSGISGetOGRGeometries_H

#include <iostream>
#include <string>
#include <list>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISVectorUtils.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_vec_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace vec{
	
	class DllExport RSGISGetOGRGeometries : public RSGISProcessOGRFeature
	{
	public:
		RSGISGetOGRGeometries(std::vector<OGRGeometry*> *geometries);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		virtual ~RSGISGetOGRGeometries();
	protected:
        std::vector<OGRGeometry*> *geometries;
	};
    
    
    class DllExport RSGISPrintGeometryToConsole : public RSGISProcessOGRFeature
	{
	public:
		RSGISPrintGeometryToConsole();
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *inFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
        void printRing(OGRLinearRing *inGeomRing) throw(RSGISVectorOutputException);
		virtual ~RSGISPrintGeometryToConsole();
    };
    
    
    class DllExport RSGISOGRPointReader : public RSGISProcessOGRFeature
    {
    public:
        RSGISOGRPointReader(std::vector<OGRPoint*> *points);
        void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
        void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
        void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
        ~RSGISOGRPointReader();
    protected:
        std::vector<OGRPoint*> *points;
    };
    
    class DllExport RSGISOGRLineReader : public RSGISProcessOGRFeature
    {
    public:
        RSGISOGRLineReader(std::vector<OGRLineString*> *lines);
        void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
        void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
        void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
        ~RSGISOGRLineReader();
    protected:
        std::vector<OGRLineString*> *lines;
    };
    
    
}}

#endif



