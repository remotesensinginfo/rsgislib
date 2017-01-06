/*
 *  RSGISSelectClumps.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 13/09/2013.
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

#ifndef RSGISSelectClumps_H
#define RSGISSelectClumps_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <algorithm>

#include "common/RSGISAttributeTableException.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageUtils.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISRATCalcValue.h"
#include "rastergis/RSGISRATCalc.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISFitGaussianMixModel.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

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
    
    enum RSGISSelectMethods
    {
        noMethod = 0,
        minMethod = 1,
        maxMethod = 2,
        meanMethod = 3,
    };
    

    class DllExport RSGISSelectClumpsOnGrid
    {
    public:
        RSGISSelectClumpsOnGrid();
        void selectClumpsOnGrid(GDALDataset *clumpsDataset, std::string inSelectField, std::string outSelectField, std::string eastingsField, std::string northingsField, std::string metricField, unsigned int rows, unsigned int cols, RSGISSelectMethods method)throw(rsgis::RSGISAttributeTableException);
        ~RSGISSelectClumpsOnGrid();
    };
    
    
    class DllExport RSGISCalcTileStats : public RSGISRATCalcValue
    {
    public:
        RSGISCalcTileStats(unsigned int numRows, unsigned int numCols, double *selectVal, unsigned int *selectIdx, std::vector<unsigned int> **tileIdxs, OGREnvelope **tilesEnvs, bool *first, RSGISSelectMethods method);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISCalcTileStats();
    private:
        unsigned int numRows;
        unsigned int numCols;
        double *selectVal;
        unsigned int *selectIdx;
        std::vector<unsigned int> **tileIdxs;
        OGREnvelope **tilesEnvs;
        bool *first;
        RSGISSelectMethods method;
    };
    
    class DllExport RSGISSelectClumpClosest2TileMean : public RSGISRATCalcValue
    {
    public:
        RSGISSelectClumpClosest2TileMean(unsigned int numRows, unsigned int numCols, double *selectVal, double *selectDistVal, unsigned int *selectIdx, std::vector<unsigned int> **tileIdxs, OGREnvelope **tilesEnvs, bool *first);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISSelectClumpClosest2TileMean();
    private:
        unsigned int numRows;
        unsigned int numCols;
        double *selectVal;
        double *selectDistVal;
        unsigned int *selectIdx;
        std::vector<unsigned int> **tileIdxs;
        OGREnvelope **tilesEnvs;
        bool *first;
    };
    
    
    class DllExport RSGISWriteSelectedClumpsColumn : public RSGISRATCalcValue
    {
    public:
        RSGISWriteSelectedClumpsColumn(unsigned int *selectIdx, unsigned int numIdxes);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISWriteSelectedClumpsColumn();
    private:
        unsigned int *selectIdx;
        unsigned int numIdxes;
    };
    
    
    
    class DllExport RSGISStatsSamplingClumps
    {
    public:
        RSGISStatsSamplingClumps();
        void histogramSampling(GDALDataset *clumpsDataset, std::string varCol, std::string outSelectCol, float propOfSample, float binWidth, bool classRestrict=false, std::string classColumn="", std::string classVal="", unsigned int ratBand=1)throw(rsgis::RSGISAttributeTableException);
        ~RSGISStatsSamplingClumps();
    };
    
    
    class DllExport RSGISCalcClassMinMax : public RSGISRATCalcValue
    {
    public:
        RSGISCalcClassMinMax(bool useClassName, std::string className, double *minVal, double *maxVal, size_t *numVals);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISCalcClassMinMax();
    private:
        bool useClassName;
        std::string className;
        double *minVal;
        double *maxVal;
        bool firstVal;
        size_t *numVals;
    };
    
    
    class DllExport RSGISCalcGenVecPairs : public RSGISRATCalcValue
    {
    public:
        RSGISCalcGenVecPairs(bool useClassName, std::string className, std::vector<std::pair<size_t, double> > *dataPairs);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISCalcGenVecPairs();
    private:
        bool useClassName;
        std::string className;
        std::vector<std::pair<size_t, double> > *dataPairs;
    };
    
    
    class DllExport RSGISSelectClumpsGMMSplit
    {
    public:
        RSGISSelectClumpsGMMSplit();
        void splitClassUsingGMM(GDALDataset *clumpsDataset, std::string outCol, std::string varCol, float binWidth, std::string classColumn, std::string classVal, unsigned int ratBand=1)throw(RSGISAttributeTableException);
        ~RSGISSelectClumpsGMMSplit();
    };
    
    
    
    
}}

#endif


