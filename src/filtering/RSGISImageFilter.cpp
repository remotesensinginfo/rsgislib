/*
 *  RSGISImageFilter.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/12/2008.
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

#include "RSGISImageFilter.h"

namespace rsgis
{
    namespace filter
    {
        RSGISImageFilter::RSGISImageFilter(
            int numberOutBands, int size, std::string filenameEnding
        ) : rsgis::img::RSGISCalcImageValue(numberOutBands)
        {
            this->size = size;
            this->filenameEnding = filenameEnding;
        }

        void RSGISImageFilter::runFilter(
            GDALDataset **datasets, int numDS, std::string outputImage,
            std::string gdalFormat, GDALDataType outDataType
        )
        {
            rsgis::img::RSGISCalcImage *calcImage = this->getCalcImage();
            try
            {
                calcImage->calcImageWindowData(
                    datasets,
                    numDS,
                    outputImage,
                    this->size,
                    gdalFormat,
                    outDataType
                );
                delete calcImage;
            }
            catch (RSGISImageException &e)
            {
                delete calcImage;
                throw e;
            }
        }

        rsgis::img::RSGISCalcImage *RSGISImageFilter::getCalcImage()
        {
            return new rsgis::img::RSGISCalcImage(this, "", true);
        }

        std::string RSGISImageFilter::getFileNameEnding()
        {
            return this->filenameEnding;
        }

        RSGISImageFilter::~RSGISImageFilter()
        {}
    }
}
