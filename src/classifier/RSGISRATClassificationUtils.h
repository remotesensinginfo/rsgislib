/*
 *  RSGISRATClassificationUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/01/2013.
 *  Copyright 2013 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 *
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef RSGISRATClassificationUtils_H
#define RSGISRATClassificationUtils_H

#include <iostream>
#include <string>
#include <map>

#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageStatistics.h"
#include "img/RSGISCalcImage.h"

#include "common/RSGISClassificationException.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISRATCalcValue.h"
#include "rastergis/RSGISRATCalc.h"
#include "common/RSGISAttributeTableException.h"

#include "gdal_priv.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_classify_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{ namespace classifier{
	
    struct DllExport RSGISClassInfo
    {
        std::string classname;
        int red;
        int blue;
        int green;
        int alpha;
        size_t classID;
    };
    
    class DllExport RSGISCollapseSegmentsClassification
    {
    public:
        RSGISCollapseSegmentsClassification();
        void collapseClassification(GDALDataset *segments, std::string classNameCol, std::string classIntCol, bool useIntCol, std::string outputImage, std::string imageFormat);
        ~RSGISCollapseSegmentsClassification();
    protected:
        std::map<std::string, RSGISClassInfo*>* findAllClassNames(const GDALRasterAttributeTable *inRAT, unsigned int classNameIdx, unsigned int redIdx, unsigned int greenIdx, unsigned int blueIdx, unsigned int alphaIdx);
    };
    
    
    class DllExport RSGISFindAllClassNames : public rsgis::rastergis::RSGISRATCalcValue
    {
    public:
        RSGISFindAllClassNames(std::map<std::string, RSGISClassInfo*> *classes, bool useIntCol);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols);
        ~RSGISFindAllClassNames();
    protected:
        std::map<std::string, RSGISClassInfo*> *classes;
        unsigned int idVal;
        bool useIntCol;
    };
    
    
    
    
    class DllExport RSGISRecodeRasterFromClasses : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISRecodeRasterFromClasses(const GDALRasterAttributeTable *rat, char **classColVals, size_t classNameColLen, std::map<std::string, RSGISClassInfo*> *classes);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
		~RSGISRecodeRasterFromClasses();
	protected:
        const GDALRasterAttributeTable *rat;
        char **classColVals;
        size_t classNameColLen;
		std::map<std::string, RSGISClassInfo*> *classes;
	};
    
    
    
    
    
    
    
    
    
    class DllExport RSGISColourImageFromClassRAT : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISColourImageFromClassRAT(GDALColorTable *clrTab);
		void calcImageValue(float *bandValues, int numBands, double *output);
		~RSGISColourImageFromClassRAT();
	protected:
		GDALColorTable *clrTab;
	};
	
}}

#endif




