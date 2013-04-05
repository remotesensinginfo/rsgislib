/*
 *  RSGISFindChangeClumps.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/04/2013.
 *  Copyright 2013 RSGISLib.
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

#ifndef RSGISFindChangeClumps_H
#define RSGISFindChangeClumps_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <algorithm>

#include "img/RSGISImageCalcException.h"
#include "rastergis/RSGISAttributeTable.h"
#include "rastergis/RSGISRasterAttUtils.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

namespace rsgis{namespace rastergis{
    
    struct RSGISClassChangeFields
    {
        std::string name;
        int outName;
        float threshold;
        float *means;
        float *stddev;
        size_t count;
    };
    
    class RSGISFindChangeClumps
    {
    public:
        RSGISFindChangeClumps();
        void findChangeStdDevThreshold(GDALDataset *clumpsDataset, std::string classCol, std::string changeField, std::vector<std::string> *fields, std::vector<rsgis::rastergis::RSGISClassChangeFields*> *classChangeField)throw(rsgis::RSGISAttributeTableException);
        ~RSGISFindChangeClumps();
    };
    
    
}}

#endif
