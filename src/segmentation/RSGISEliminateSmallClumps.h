/*
 *  RSGISEliminateSmallClumps.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/01/2012.
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

#ifndef RSGISEliminateSmallClumps_h
#define RSGISEliminateSmallClumps_h

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <list>
#include <math.h>

#include "gdal_priv.h"

#include "common/RSGISAttributeTableException.h"

#include "utils/RSGISTextUtils.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "rastergis/RSGISAttributeTable.h"
#include "rastergis/RSGISPopulateAttributeTable.h"
#include "rastergis/RSGISFindClumpNeighbours.h"

namespace rsgis{namespace segment{
    
    using namespace std;
    using namespace rsgis::img;
    using namespace rsgis::rastergis;
    using namespace rsgis::utils;
    using namespace rsgis;
    
    class RSGISEliminateSmallClumps
    {
    public:
        RSGISEliminateSmallClumps();
        void eliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold) throw(RSGISImageCalcException);
        void stepwiseEliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold) throw(RSGISImageCalcException);
        void stepwiseEliminateSmallClumpsWithAtt(GDALDataset *spectral, GDALDataset *clumps, string outputImageFile, string imageFormat, bool useImageProj, string proj, RSGISAttributeTable *attTable, unsigned int minClumpSize, float specThreshold, bool outputWithConsecutiveFIDs) throw(RSGISImageCalcException);
        ~RSGISEliminateSmallClumps();
    protected:
        void defineOutputFID(RSGISAttributeTable *attTable, RSGISFeature *feat, unsigned int eliminatedFieldIdx, unsigned int mergedToFIDIdx, unsigned int outFIDIdx, unsigned int outFIDSetFieldIdx) throw(RSGISAttributeTableException);
        void performElimination(RSGISAttributeTable *attTable, vector<pair<unsigned long, unsigned long> > *eliminationPairs, unsigned int eliminatedFieldIdx, unsigned int mergedToFIDIdx, unsigned int pxlCountIdx, vector<RSGISBandAttStats*> *bandStats) throw(RSGISAttributeTableException);
        RSGISFeature* getEliminatedNeighbour(RSGISFeature *feat, RSGISAttributeTable *attTable, unsigned int eliminatedFieldIdx, unsigned int mergedToFIDIdx)throw(RSGISAttributeTableException);
    };
    
    class RSGISEliminateFeature : public RSGISProcessFeature
    {
    public:
        RSGISEliminateFeature(unsigned int eliminatedFieldIdx, unsigned int mergedToFIDIdx, float specThreshold, unsigned int pxlCountIdx, vector<RSGISBandAttStats*> *bandStats, vector<pair<unsigned long, unsigned long> > *eliminationPairs);
        void processFeature(RSGISFeature *feat, RSGISAttributeTable *attTable)throw(RSGISAttributeTableException);
        ~RSGISEliminateFeature();
    protected:
        double calcDistance(RSGISFeature *feat1, RSGISFeature *feat2, vector<RSGISBandAttStats*> *bandStats)throw(RSGISAttributeTableException);
        RSGISFeature* getEliminatedNeighbour(RSGISFeature *feat, RSGISAttributeTable *attTable)throw(RSGISAttributeTableException); 
        unsigned int eliminatedFieldIdx;
        unsigned int mergedToFIDIdx;
        float specThreshold;
        unsigned int pxlCountIdx;
        vector<RSGISBandAttStats*> *bandStats;
        vector<pair<unsigned long, unsigned long> > *eliminationPairs;
    };
    
    
    class RSGISApplyOutputFIDs : public RSGISCalcImageValue
    {
    public:
        RSGISApplyOutputFIDs(RSGISAttributeTable *attTable, unsigned int outFIDIdx, unsigned int outFIDSetFieldIdx);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented.");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented.");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented.");};
        ~RSGISApplyOutputFIDs();
    protected:
        RSGISAttributeTable *attTable;
        unsigned int outFIDIdx;
        unsigned int outFIDSetFieldIdx;
    };

    
}}

#endif
