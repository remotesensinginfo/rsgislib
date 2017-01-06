/*
 *  RSGISInputShapefileAttributes2RAT.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/12/2014.
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

#ifndef RSGISInputShapefileAttributes2RAT_H
#define RSGISInputShapefileAttributes2RAT_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <list>

#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include "common/RSGISAttributeTableException.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISRATCalc.h"
#include "rastergis/RSGISRATCalcValue.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_rastergis_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace rastergis{
    
    class DllExport RSGISInputShapefileAttributes2RAT
    {
    public:
        RSGISInputShapefileAttributes2RAT();
        void copyVectorAtt2Rat(GDALDataset *clumpsImage, unsigned int ratBand, OGRLayer *vecLayer, std::vector<std::string> *colNames)throw(RSGISAttributeTableException);
        virtual ~RSGISInputShapefileAttributes2RAT();
    };
    
    
    class DllExport RSGISRATCalcValueCopyShapeAtts: public RSGISRATCalcValue
    {
    public:
        RSGISRATCalcValueCopyShapeAtts(OGRLayer *vecLayer, std::vector<RSGISRATCol> ratColInfo, size_t numVecFeats);
        virtual void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        virtual ~RSGISRATCalcValueCopyShapeAtts();
    protected:
        OGRLayer *vecLayer;
        std::vector<RSGISRATCol> ratColInfo;
        size_t numVecFeats;
    };
    
}}

#endif