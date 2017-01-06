/*
 *  RSGISRATFunctionFitting.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/03/2015.
 *  Copyright 2015 RSGISLib.
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

#ifndef RSGISRATFunctionFitting_H
#define RSGISRATFunctionFitting_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include "common/RSGISAttributeTableException.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISFitGaussianMixModel.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISRATCalcValue.h"
#include "rastergis/RSGISRATCalc.h"

#include "utils/RSGISExportData2HDF.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include "H5Cpp.h"

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
    
    class DllExport RSGISFitHistGausianMixtureModel
    {
    public:
        RSGISFitHistGausianMixtureModel(){};
        void performFit(GDALDataset *clumpsDataset, std::string outH5File, std::string varCol, float binWidth, std::string classColumn, std::string classVal, bool outputHist, std::string outHistFile, unsigned int ratBand=1);
        ~RSGISFitHistGausianMixtureModel(){};
    };
    
    
    class DllExport RSGISGetClassData : public RSGISRATCalcValue
    {
    public:
        RSGISGetClassData(std::string className, std::vector<double> *data, double *minVal, double *maxVal);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISGetClassData();
    private:
        std::string className;
        std::vector<double> *data;
        double *minVal;
        double *maxVal;
        bool firstVal;
    };

    
    
}}

#endif


