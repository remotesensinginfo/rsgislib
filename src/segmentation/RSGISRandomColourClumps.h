/*
 *  RSGISRandomColourClumps.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/01/2012.
 *  Copyright 2012 RSGISLib.
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

#ifndef RSGISRandomColourClumps_h
#define RSGISRandomColourClumps_h

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "gdal_priv.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "utils/RSGISTextUtils.h"
#include "utils/RSGISTextException.h"

using namespace std;
using namespace rsgis::img;
using namespace rsgis::utils;

namespace rsgis{namespace segment{
    
    struct ImgClumpRGB
    {
        ImgClumpRGB(unsigned long clumpID)
        {
            this->clumpID = clumpID;
        };
        unsigned int clumpID;
        int red;
        int green;
        int blue;
    };
    
    
    class RSGISRandomColourClumps
    {
    public:
        RSGISRandomColourClumps();
        void generateRandomColouredClump(GDALDataset *clumps, GDALDataset *colourImg, string inputLUTFile, bool importLUT, string exportLUTFile, bool exportLUT) throw(RSGISImageCalcException);
        ~RSGISRandomColourClumps();
    protected:
        vector<ImgClumpRGB*>* importLUTFromFile(string inFile) throw(RSGISTextException);
        void exportLUT2File(string outFile, vector<ImgClumpRGB*> *clumpTab) throw(RSGISTextException);
    };
    
}}



#endif
