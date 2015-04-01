/*
 *  RSGISMergeSegmentations.cpp
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

#include "RSGISMergeSegmentations.h"

namespace rsgis{namespace segment{
    
    RSGISMergeSegmentations::RSGISMergeSegmentations()
    {
        
    }
    
    void RSGISMergeSegmentations::includeRegions(GDALDataset *clumpsImage, GDALDataset *regionsImage, std::string outputImage, std::string format)throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            double maxClumpID = 0.0;
            int nLastProgress = -1;
            clumpsImage->GetRasterBand(1)->ComputeStatistics(false, NULL, &maxClumpID, NULL, NULL,  (GDALProgressFunc)RSGISSegStatsTextProgress, &nLastProgress);
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumpsImage;
            datasets[1] = regionsImage;
            
            RSGISAddWithSegOffset calcImgValAddOff(maxClumpID);
            rsgis::img::RSGISCalcImage calcImage(&calcImgValAddOff);
            calcImage.calcImage(datasets, 2, 0, outputImage, false, NULL, format, GDT_UInt32);
            delete[] datasets;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    RSGISMergeSegmentations::~RSGISMergeSegmentations()
    {
        
    }
    
    
    RSGISAddWithSegOffset::RSGISAddWithSegOffset(unsigned long offset) : rsgis::img::RSGISCalcImageValue(1)
    {
        this->offset = offset;
    }

    void RSGISAddWithSegOffset::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(intBandValues[1] != 0)
        {
            output[0] = (intBandValues[1] + this->offset);
        }
        else
        {
            output[0] = intBandValues[0];
        }
    }

    RSGISAddWithSegOffset::~RSGISAddWithSegOffset()
    {
        
    }
    
    
    
}}

