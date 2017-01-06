/*
 *  RSGISImageInterpolator.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 15/05/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISImageInterpolator_H
#define RSGISImageInterpolator_H

#include <iostream>
#include <string>

#include "common/RSGISImageException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_img_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace img{
	
	class DllExport RSGISInterpolator
		{
		public:
			RSGISInterpolator();
			virtual double interpolate(double xShift, double yShift, double *pixels) throw(rsgis::RSGISImageException)=0;
			virtual ~RSGISInterpolator();
		};
	
	class DllExport RSGISCubicInterpolator : public RSGISInterpolator
		{
		public:
			RSGISCubicInterpolator();
			double interpolate(double xShift, double yShift, double *pixels) throw(rsgis::RSGISImageException);
		protected:
			double estimateNewValueFromCurve(double *pixels, double shift);
		};

	class DllExport RSGISBilinearAreaInterpolator : public RSGISInterpolator
		{
		public:
			RSGISBilinearAreaInterpolator();
			double interpolate(double xShift, double yShift, double *pixels) throw(rsgis::RSGISImageException);
		};
	
	class DllExport RSGISBilinearPointInterpolator : public RSGISInterpolator
		{
		public:
			RSGISBilinearPointInterpolator();
			double interpolate(double xShift, double yShift, double *pixels) throw(rsgis::RSGISImageException);
		};
	
	class DllExport RSGISNearestNeighbourInterpolator : public RSGISInterpolator
		{
		public:
			RSGISNearestNeighbourInterpolator();
			double interpolate(double xShift, double yShift, double *pixels) throw(rsgis::RSGISImageException);
		protected:
			int findIndexOfMax(double *arr, int size);
		};
	
	class DllExport RSGISTriangulationInterpolator : public RSGISInterpolator
		{
		public:
			RSGISTriangulationInterpolator();
			double interpolate(double xShift, double yShift, double *pixels) throw(rsgis::RSGISImageException);
		protected:
			double triangle(double xShift, double yShift, double *pixels, bool triangulation);
		};
	
}}

#endif

