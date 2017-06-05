/*
 *  RSGISCalcImgAlongsideOut.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/12/2011.
 *  Copyright 2011 RSGISLib.
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

#ifndef RSGISCalcImgAlongsideOut_h
#define RSGISCalcImgAlongsideOut_h

#include <iostream>
#include <string>

#include "gdal_priv.h"

#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageUtils.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_img_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis 
{
	namespace img
	{
        class DllExport RSGISCalcImgValueAlongsideOut
        {
        public:
            RSGISCalcImgValueAlongsideOut(){};
            virtual bool calcValue(bool firstIter, unsigned int numBands, unsigned int *dataCol, unsigned int **rowAbove, unsigned int **rowBelow, unsigned int *left, unsigned int *right)throw(RSGISImageCalcException)=0;
            virtual ~RSGISCalcImgValueAlongsideOut(){};
        };
        
		class DllExport RSGISCalcImgAlongsideOut
        {
        public:
            RSGISCalcImgAlongsideOut(RSGISCalcImgValueAlongsideOut *valueCalc);
            void calcImageIterate(GDALDataset *dataset) throw(RSGISImageCalcException,RSGISImageBandException);
            ~RSGISCalcImgAlongsideOut(){};
        private:
            RSGISCalcImgValueAlongsideOut *calc;
        };
	}
}

#endif
