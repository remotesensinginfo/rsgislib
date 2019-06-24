/*
 *  RSGISRasterAttUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/08/2012.
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

#ifndef RSGISRasterAttUtils_H
#define RSGISRasterAttUtils_H

#define RAT_BLOCK_LENGTH 100000 // Define block length (Default block length for KEA is 1000)

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <math.h>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include "libkea/KEAImageIO.h"

#include "common/RSGISAttributeTableException.h"

#include "utils/RSGISColour.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

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
    
    inline int RSGISRATStatsTextProgress( double dfComplete, const char *pszMessage, void *pData)
    {
        int nPercent = int(dfComplete*100);
        int *pnLastComplete = (int*)pData;
        
        if(nPercent < 10)
        {
            nPercent = 0;
        }
        else if(nPercent < 20)
        {
            nPercent = 10;
        }
        else if(nPercent < 30)
        {
            nPercent = 20;
        }
        else if(nPercent < 40)
        {
            nPercent = 30;
        }
        else if(nPercent < 50)
        {
            nPercent = 40;
        }
        else if(nPercent < 60)
        {
            nPercent = 50;
        }
        else if(nPercent < 70)
        {
            nPercent = 60;
        }
        else if(nPercent < 80)
        {
            nPercent = 70;
        }
        else if(nPercent < 90)
        {
            nPercent = 80;
        }
        else if(nPercent < 95)
        {
            nPercent = 90;
        }
        else
        {
            nPercent = 100;
        }
        
        if( (pnLastComplete != NULL) && (nPercent != *pnLastComplete ))
        {
            if(nPercent == 0)
            {
                std::cout << "Started ." << nPercent << "." << std::flush;
            }
            else if(nPercent == 100)
            {
                std::cout << "." << nPercent << ". Complete." << std::endl;
            }
            else
            {
                std::cout << "." << nPercent << "." << std::flush;
            }
        }
        
        *pnLastComplete = nPercent;
        
        return true;
    };
    
    struct DllExport RSGISRATCol
    {
        std::string name;
        unsigned int idx;
        GDALRATFieldType type;
        GDALRATFieldUsage usage;
        
        bool operator==(RSGISRATCol val) const
        {
            bool equal = true;
            if(val.name != this->name)
            {
                equal = false;
            }
            else if (val.type != this->type)
            {
                equal = false;
            }
            else if (val.usage != this->usage)
            {
                equal = false;
            }
            return equal;
        }
    };
    
    class DllExport RSGISRasterAttUtils
    {
    public:
        RSGISRasterAttUtils();
        void copyAttColumns(GDALDataset *inImage, GDALDataset *outImage, std::vector<std::string> fields,  bool copyColours=true, bool copyHist=true, int ratBand=1);
        void copyAttColumnsWithOff(GDALRasterAttributeTable *inRAT, GDALRasterAttributeTable *outRAT, std::vector<std::string> fields, unsigned int offset, bool ignoreFirstRow=true, bool copyColours=true, bool copyHist=true);
        void copyColourForCats(GDALDataset *catsImage, GDALDataset *classImage, std::string classField);
        void exportColumns2ASCII(GDALDataset *inImage, std::string outputFile, std::vector<std::string> fields, int ratBand=1);
        void translateClasses(GDALDataset *inImage, std::string classInField, std::string classOutField, std::map<size_t, size_t> classPairs);
        void applyClassColours(GDALDataset *inImage, std::string classInField, std::map<size_t, rsgis::utils::RSGISColourInt> classColoursPairs, int ratBand=1);
        void applyClassStrColours(GDALDataset *inImage, std::string classInField, std::map<std::string, rsgis::utils::RSGISColourInt> classColoursPairs, int ratBand=1);
        unsigned int findColumnIndex(const GDALRasterAttributeTable *gdalATT, std::string colName);
        unsigned int findColumnIndexOrCreate(GDALRasterAttributeTable *gdalATT, std::string colName, GDALRATFieldType dType, GDALRATFieldUsage dUsage=GFU_Generic);
        double readDoubleColumnVal(const GDALRasterAttributeTable *gdalATT, std::string colName, unsigned int row);
        long readIntColumnVal(const GDALRasterAttributeTable *gdalATT, std::string colName, unsigned int row);
        std::string readStringColumnVal(const GDALRasterAttributeTable *gdalATT, std::string colName, unsigned int row);
        double* readDoubleColumn(GDALRasterAttributeTable *attTable, std::string colName, size_t *colLen);
        int* readIntColumn(GDALRasterAttributeTable *attTable, std::string colName, size_t *colLen);
        char** readStrColumn(GDALRasterAttributeTable *attTable, std::string colName, size_t *colLen);
        std::string* readStrColumnStdStr(GDALRasterAttributeTable *attTable, std::string colName, size_t *colLen);
        std::vector<double>* readDoubleColumnAsVec(GDALRasterAttributeTable *attTable, std::string colName);
        std::vector<int>* readIntColumnAsVec(GDALRasterAttributeTable *attTable, std::string colName);
        std::vector<std::string>* readStrColumnAsVec(GDALRasterAttributeTable *attTable, std::string colName);
        std::vector<std::vector<size_t>* >* getRATNeighbours(GDALDataset *clumpImage, unsigned int ratBand);
        void writeStrColumn(GDALRasterAttributeTable *attTable, std::string colName, std::string *strDataVal, size_t colLen);
        void writeIntColumn(GDALRasterAttributeTable *attTable, std::string colName, int *intDataVal, size_t colLen);
        void writeRealColumn(GDALRasterAttributeTable *attTable, std::string colName, double *realDataVal, size_t colLen);
        std::vector<RSGISRATCol>* getRatColumnsList(GDALRasterAttributeTable *gdalATT);
        std::vector<RSGISRATCol>* getVectorColumns(OGRLayer *layer, bool ignoreErr=false);
        void getImageBandMinMax(GDALDataset *inImage, int band, long *minVal, long *maxVal);
        ~RSGISRasterAttUtils();
    };
    
    
    class DllExport RSGISCalcImgMinMax : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcImgMinMax(long *minVal, long *maxVal, unsigned int band);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals);
        ~RSGISCalcImgMinMax();
    private:
        long *minVal;
        long *maxVal;
        bool first;
        unsigned int band;
    };
	
}}

#endif

