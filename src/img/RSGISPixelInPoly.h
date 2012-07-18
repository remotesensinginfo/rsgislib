/*
 *  RSGISPixelInPoly.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 05/03/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#ifndef RSGISPixelInPoly_H
#define RSGISPixelInPoly_H

#include "ogrsf_frmts.h"
#include "geos/geom/GeometryFactory.h"

#include "common/RSGISVectorException.h"

namespace rsgis{namespace img {
	
	enum pixelInPolyOption 
	{
		polyContainsPixel, /// Polygon completely contains pixel
		polyContainsPixelCenter, /// Pixel center is within the polygon
		polyOverlapsPixel, /// Polygon overlaps the pixel
		polyOverlapsOrContainsPixel, /// Polygon overlaps or contains the pixel
		pixelContainsPoly, /// Pixel contains the polygon
		pixelContainsPolyCenter, /// Polygon center is within pixel
		adaptive, /// The method is chosen based on relative areas of pixel and polygon.
		envelope, /// All pixels in polygon envelope chosen
		pixelAreaInPoly, /// Percent of pixel area that is within the polygon
		polyAreaInPixel /// Percent of polygon area that is within pixel
	};
	
	class RSGISPixelInPoly 
	{
	public:
		RSGISPixelInPoly(pixelInPolyOption method);
		bool findPixelInPoly(OGRGeometry *poly, OGRPolygon *pixel) throw(rsgis::RSGISVectorException);
		~RSGISPixelInPoly();
	private:
		pixelInPolyOption method;
		OGRPolygon *polyOGRPoly;
	};
	
}}

#endif
