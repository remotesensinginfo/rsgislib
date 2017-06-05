/*
 *  RSGISBinaryClassifyClumps.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/08/2014.
 *  Copyright 2014 RSGISLib.
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

#ifndef RSGISBinaryClassifyClumps_H
#define RSGISBinaryClassifyClumps_H

#include <string>
#include <vector>
#include <math.h>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISLogicExpEvaluation.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISRATCalcValue.h"
#include "rastergis/RSGISRATCalc.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
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
    
    enum DllExport RSGISLogicExpType
    {
        rsgis_expNA = 0,
        rsgis_singlecolthres = 1,
        rsgis_multicols = 2,
        rsgis_singlecol = 3
    };
    
    struct DllExport RSGISColumnLogicIdxs
    {
        std::string column1Name;
        unsigned int col1Idx;
        double col1Val;
        std::string column2Name;
        unsigned int col2Idx;
        double col2Val;
        double thresholdVal;
        bool useThreshold;
        bool singleCol;
    };
    
    class DllExport RSGISRATLogicXMLParse
    {
    public:
        RSGISRATLogicXMLParse(){};
        rsgis::math::RSGISLogicExpression* parseLogicXML(std::string xmlStr, std::vector<RSGISColumnLogicIdxs*> *colIdxes) throw(RSGISAttributeTableException);
        rsgis::math::RSGISLogicExpression* createExpression(xercesc::DOMElement *expElement, std::vector<RSGISColumnLogicIdxs*> *colIdxes) throw(RSGISAttributeTableException);
        ~RSGISRATLogicXMLParse(){};
    };
    
    class DllExport RSGISBinaryClassifyClumps
    {
    public:
        RSGISBinaryClassifyClumps();
        void classifyClumps(GDALDataset *inputClumps, unsigned int ratBand, std::string xmlBlock, std::string outColumn)throw(RSGISAttributeTableException);
        ~RSGISBinaryClassifyClumps();
    };
    
    
    class DllExport RSGISBinaryClumpClassifier : public RSGISRATCalcValue
    {
    public:
        RSGISBinaryClumpClassifier(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*> *colIdxes, rsgis::math::RSGISLogicExpression *exp);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISBinaryClumpClassifier();
    protected:
        std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*> *colIdxes;
        rsgis::math::RSGISLogicExpression *exp;
    };
    
}}

#endif

