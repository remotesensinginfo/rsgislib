/*
 *  RSGISPopulateFeatsElev.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 26/06/2014.
 *  Copyright 2014 RSGISLib. All rights reserved.
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

#ifndef RSGISPopulateFeatsElev_H
#define RSGISPopulateFeatsElev_H

#include <iostream>
#include <string>
#include <list>

#include <boost/algorithm/string/replace.hpp>

#include "ogrsf_frmts.h"
#include "ogr_geometry.h"

#include "gdal_priv.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISVectorUtils.h"

#include "math/RSGISMathsUtils.h"

#include "geos/geom/Envelope.h"

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
	
	class DllExport RSGISPopulateFeatsElev : public RSGISProcessOGRFeature
	{
	public:
		RSGISPopulateFeatsElev(GDALDataset *image, unsigned int band)throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *inFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		double* getPixelColumns(int xPxl, int yPxl);
        OGRLinearRing* popZfield(OGRLinearRing *inGeomRing) throw(RSGISVectorOutputException);
		virtual ~RSGISPopulateFeatsElev();
	private:
		GDALDataset *image;
		GDALRasterBand **bands;
		int numImgBands;
		geos::geom::Envelope *imageExtent;
		double imgRes;
		double *pxlValues;
        unsigned int band;
    };
}}

#endif



