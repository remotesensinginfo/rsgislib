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
    
    inline int pixelInPolyEnum2Int(pixelInPolyOption enumMethod)
    {
        int intMethod = 1;
        switch (enumMethod)
        {
            case polyContainsPixel: // Polygon completely contains pixel
                intMethod = 0;
                break;
            case polyContainsPixelCenter: // Pixel center is within the polygon
                intMethod = 1;
                break;
            case polyOverlapsPixel: // Polygon overlaps the pixel
                intMethod = 2;
                break;
            case polyOverlapsOrContainsPixel: // Polygon overlaps or contains the pixel
                intMethod = 3;
                break;
            case pixelContainsPoly: // Pixel contains the polygon
                intMethod = 4;
                break;
            case pixelContainsPolyCenter: // Polygon center is within pixel
                intMethod = 5;
                break;
            case adaptive: // The method is chosen based on relative areas of pixel and polygon.
                intMethod = 6;
                break;
            case envelope: // All pixels in polygon envelope chosen
                intMethod = 7;
                break;
            case pixelAreaInPoly: // Percent of pixel area that is within the polygon
                intMethod = 8;
                break;
            case polyAreaInPixel: // Percent of polygon area that is within pixel
                intMethod = 9;
                break;
            default:
                intMethod = 1;
        }
        
        return intMethod;
    };
    
    inline pixelInPolyOption pixelInPolyInt2Enum(int intMethod)
    {
        pixelInPolyOption enumMethod = polyContainsPixelCenter;
        switch (intMethod)
        {
            case 0: // Polygon completely contains pixel
                enumMethod = polyContainsPixel;
                break;
            case 1: // Pixel center is within the polygon
                enumMethod = polyContainsPixelCenter;
                break;
            case 2: // Polygon overlaps the pixel
                enumMethod = polyOverlapsPixel;
                break;
            case 3: // Polygon overlaps or contains the pixel
                enumMethod = polyOverlapsOrContainsPixel;
                break;
            case 4: // Pixel contains the polygon
                enumMethod = pixelContainsPoly;
                break;
            case 5: // Polygon center is within pixel
                enumMethod = pixelContainsPolyCenter;
                break;
            case 6: // The method is chosen based on relative areas of pixel and polygon.
                enumMethod = adaptive;
                break;
            case 7: // All pixels in polygon envelope chosen
                enumMethod = envelope;
                break;
            case 8: // Percent of pixel area that is within the polygon
                enumMethod = pixelAreaInPoly;
                break;
            case 9: // Percent of polygon area that is within pixel
                enumMethod = polyAreaInPixel;
                break;
            default:
                enumMethod = polyContainsPixelCenter;
        }
        
        return enumMethod;
    };


	class DllExport RSGISPixelInPoly
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
