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

    class DllExport RSGISGetOGRGeometries : public RSGISProcessOGRFeature
    {
    public:
        RSGISGetOGRGeometries(std::vector<OGRGeometry*> *geometries);
        virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, OGREnvelope *env, long fid);
        virtual void processFeature(OGRFeature *feature, OGREnvelope *env, long fid);
        virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
        virtual ~RSGISGetOGRGeometries();
    protected:
        std::vector<OGRGeometry*> *geometries;
    };


    class DllExport RSGISPrintGeometryToConsole : public RSGISProcessOGRFeature
    {
    public:
        RSGISPrintGeometryToConsole();
        virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, OGREnvelope *env, long fid);
        virtual void processFeature(OGRFeature *inFeature, OGREnvelope *env, long fid);
        virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
        void printRing(OGRLinearRing *inGeomRing);
        virtual ~RSGISPrintGeometryToConsole();
    };


    class DllExport RSGISOGRPointReader : public RSGISProcessOGRFeature
    {
    public:
        RSGISOGRPointReader(std::vector<OGRPoint*> *points);
        void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, OGREnvelope *env, long fid);
        void processFeature(OGRFeature *feature, OGREnvelope *env, long fid);
        void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
        ~RSGISOGRPointReader();
    protected:
        std::vector<OGRPoint*> *points;
    };

    class DllExport RSGISOGRLineReader : public RSGISProcessOGRFeature
    {
    public:
        RSGISOGRLineReader(std::vector<OGRLineString*> *lines);
        void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, OGREnvelope *env, long fid);
        void processFeature(OGRFeature *feature, OGREnvelope *env, long fid);
        void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
        ~RSGISOGRLineReader();
    protected:
        std::vector<OGRLineString*> *lines;
    };


}}

#endif
