/*
 *  RSGISMergeSegmentations.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 08/12/2014.
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

#ifndef RSGISMergeSegmentations_H
#define RSGISMergeSegmentations_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>

#include "common/RSGISAttributeTableException.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISImageStatistics.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"


namespace rsgis{namespace segment{
    
    inline int RSGISSegStatsTextProgress( double dfComplete, const char *pszMessage, void *pData)
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
    
    class DllExport RSGISMergeSegmentations
    {
    public:
        RSGISMergeSegmentations();
        void includeRegions(GDALDataset *clumpsImage, GDALDataset *regionsImage, std::string outputImage, std::string format)throw(rsgis::img::RSGISImageCalcException);
        ~RSGISMergeSegmentations();
    };
    
    class DllExport RSGISAddWithSegOffset : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISAddWithSegOffset(unsigned long offset);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISAddWithSegOffset();
    private:
        unsigned long offset;
    };
    
}}

#endif
