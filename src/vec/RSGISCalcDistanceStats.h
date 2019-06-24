/*
 *  RSGISCalcDistanceStats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 08/02/2013.
 *  Copyright 2013 RSGISLib. All rights reserved.
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

#ifndef RSGISCalcDistanceStats_H
#define RSGISCalcDistanceStats_H

#include <iostream>
#include <string>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISVectorUtils.h"

#include "math/RSGISMathsUtils.h"

#include "geos/geom/Envelope.h"
#include "geos/index/strtree/STRtree.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
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
	
	class DllExport RSGISCalcMinDists2Polys : public RSGISProcessOGRFeature
    {
    public:
        RSGISCalcMinDists2Polys(std::vector<OGRGeometry*> *polys);
        virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid);
        virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid){throw RSGISVectorException("Not implemented");};
        virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
        virtual ~RSGISCalcMinDists2Polys();
    protected:
        std::vector<OGRGeometry*> *polys;
    };
    
    
    class DllExport RSGISCalcMinDist2Geoms : public RSGISProcessOGRFeature
    {
    public:
        RSGISCalcMinDist2Geoms(std::string outColName, std::vector<OGRGeometry*> *geoms);
        virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid);
        virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid);
        virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
        double getMaxMinDist(){return this->maxMinDist;};
        virtual ~RSGISCalcMinDist2Geoms();
    protected:
        std::string outColName;
        std::vector<OGRGeometry*> *geoms;
        bool firstGeom;
        double maxMinDist;
    };
    
    
    class DllExport RSGISCalcMinDist2GeomsUseIdx : public RSGISProcessOGRFeature
    {
    public:
        RSGISCalcMinDist2GeomsUseIdx(std::string outColName, geos::index::SpatialIndex *geomIdx, double maxSearchDist);
        virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid);
        virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid);
        virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
        double getMaxMinDist(){return this->maxMinDist;};
        virtual ~RSGISCalcMinDist2GeomsUseIdx();
    protected:
        std::string outColName;
        geos::index::SpatialIndex *geomIdx;
        bool firstGeom;
        double maxMinDist;
        double maxSearchDist;
    };
    
    
}}


#endif




