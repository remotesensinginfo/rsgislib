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

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISRATCalcValue.h"
#include "rastergis/RSGISRATCalc.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include <boost/math/special_functions/fpclassify.hpp>

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

    struct DllExport RSGISClassChangeFields
    {
        std::string name;
        int outName;
        float threshold;
        float *means;
        float *stddev;
        size_t count;
    };


    class DllExport RSGISFindChangeClumpsStdDevThreshold : public RSGISRATCalcValue
    {
    public:
        RSGISFindChangeClumpsStdDevThreshold(GDALDataset *clumpsDataset, std::string classCol, std::string changeField, std::vector<std::string> *fields,
                                             std::vector<rsgis::rastergis::RSGISClassChangeFields*> *classChangeField, unsigned int ratBand=1);
        void getThresholds();
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols,
                          unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols,
                          std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISFindChangeClumpsStdDevThreshold();
    public:
        GDALRasterAttributeTable *attTable;
        unsigned int *fieldIdxs;
        unsigned int classColIdx;
        unsigned int changeFieldIdx;
        unsigned int numFields;
    private:
        std::string classCol;
        std::string changeField;
        std::vector<std::string> *fields;
        std::vector<rsgis::rastergis::RSGISClassChangeFields*> *classChangeField;
        float ***thresholds;
        unsigned int numRows;
        unsigned int numClasses;
    };

    class DllExport RSGISGetGlobalClassStats : public RSGISRATCalcValue
    {
    /**

    Calculates the global mean and standard deviation for each class in 'classCol' and saves out for each row.

    Output columns have the following names:

    classCol + fields->at(i) + 'Avg'
    classCol + fields->at(i) + 'Std'

    Very similar to RSGISFindChangeClumpsStdDevThreshold but allows the threshold to be applied externally, or calculation of change based on greater than or less than mean.

    */

    public:
        RSGISGetGlobalClassStats(GDALDataset *clumpsDataset, std::string classCol, std::vector<std::string> *fields,
                                 std::vector<rsgis::rastergis::RSGISClassChangeFields*> *classChangeField, unsigned int ratBand=1);
        void getStats();
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols,
                          std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols,
                          int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISGetGlobalClassStats();
    public:
        GDALRasterAttributeTable *attTable;
        unsigned int *fieldIdxs;
        unsigned int *classStatsIdx;
        unsigned int classColIdx;
        unsigned int numFields;
    private:
        std::string classCol;
        std::vector<std::string> *fields;
        std::vector<rsgis::rastergis::RSGISClassChangeFields*> *classChangeField;
        unsigned int numRows;
        unsigned int numClasses;
    };
}}

#endif
