/*
 *  RSGISClumpImage.h
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

#ifndef RSGISClumpImage_h
#define RSGISClumpImage_h


#include <iostream>
#include <string>

#include "gdal_priv.h"

#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcImgAlongsideOut.h"

using namespace std;

namespace rsgis 
{
	namespace img
	{
        class RSGISClumpImage : public RSGISCalcImgValueAlongsideOut
        {
        public:
            RSGISClumpImage():RSGISCalcImgValueAlongsideOut()
            {
                this->clumpCounter = 1;
            };
            virtual bool calcValue(bool firstIter, unsigned int numBands, unsigned int *dataCol, unsigned int **rowAbove, unsigned int **rowBelow, unsigned int *left, unsigned int *right)throw(RSGISImageCalcException);
            ~RSGISClumpImage(){};
        private:
            long clumpCounter;
        };
	}
}

#endif
