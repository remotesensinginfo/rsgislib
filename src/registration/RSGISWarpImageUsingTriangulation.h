 /*
 *  RSGISWarpImageUsingTriangulation.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/09/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 * 
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef RSGISWarpImageUsingTriangulation_H
#define RSGISWarpImageUsingTriangulation_H

#include <iostream>
#include <string>
#include <math.h>
#include <list>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "registration/RSGISImageWarpException.h"
#include "registration/RSGISWarpImage.h"

#include "math/RSGISMatrices.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Interpolation_traits_2.h>
#include <CGAL/natural_neighbor_coordinates_2.h>
#include <CGAL/interpolation_functions.h>
#include <CGAL/algorithm.h>
#include <CGAL/Origin.h>
#include <CGAL/squared_distance_2.h>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_registration_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace reg{
	
    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
    typedef K::FT                                         CGALCoordType;
    typedef K::Vector_2                                   CGALVector;
    typedef K::Point_2                                    CGALPoint;
    
    typedef CGAL::Delaunay_triangulation_2<K>             DelaunayTriangulation;
    typedef CGAL::Interpolation_traits_2<K>               InterpTraits;
    typedef CGAL::Delaunay_triangulation_2<K>::Vertex_handle    Vertex_handle;
    typedef CGAL::Delaunay_triangulation_2<K>::Face_handle    Face_handle;
    
    typedef std::vector< std::pair<CGALPoint, CGALCoordType> >   CoordinateVector;
    typedef std::map<CGALPoint, rsgis::reg::RSGISGCPImg2MapNode*, K::Less_xy_2>     PointValueMap;
    
	class DllExport RSGISWarpImageUsingTriangulation : public RSGISWarpImage
	{
	public:
		RSGISWarpImageUsingTriangulation(std::string inputImage, std::string outputImage, std::string outProjWKT, std::string gcpFilePath, float outImgRes, RSGISWarpImageInterpolator *interpolator, std::string gdalFormat);
		void initWarp()throw(RSGISImageWarpException);
		~RSGISWarpImageUsingTriangulation();
	protected:
        geos::geom::Envelope* newImageExtent(unsigned int width, unsigned int height) throw(RSGISImageWarpException);
		void findNearestPixel(double eastings, double northings, unsigned int *x, unsigned int *y, float inImgRes) throw(RSGISImageWarpException);
		std::list<RSGISGCPImg2MapNode*>* normGCPs(std::list<const RSGISGCPImg2MapNode*> *gcps, double eastings, double northings);
		void fitPlane2XPoints(std::list<RSGISGCPImg2MapNode*> *normPts, double *a, double *b, double *c) throw(RSGISImageWarpException);
		void fitPlane2YPoints(std::list<RSGISGCPImg2MapNode*> *normPts, double *a, double *b, double *c) throw(RSGISImageWarpException);
		DelaunayTriangulation *dt;
        PointValueMap *values;
	};
	
}}

#endif



