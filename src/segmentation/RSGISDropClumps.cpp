/*
 *  RSGISDropClumps.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/04/2015.
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

#include "RSGISDropClumps.h"

namespace rsgis{namespace segment{
    
    
    RSGISDropClumps::RSGISDropClumps()
    {
        
    }
    
    void RSGISDropClumps::dropSelectedClumps(GDALDataset *clumpsImage, std::string outputImage, std::string selClumps2Drop, std::string gdalFormat, unsigned int ratBand)throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            rastergis::RSGISRasterAttUtils attUtils;
            
            GDALRasterBand *clumpBand = clumpsImage->GetRasterBand(ratBand);
            GDALRasterAttributeTable *rat = clumpBand->GetDefaultRAT();
            size_t numRows = rat->GetRowCount();
            std::cout << "Number of clumps is " << numRows << "\n";
            
            std::cout << "Read input column\n";
            size_t tmpNumRows = 0;
            int *selectCol = attUtils.readIntColumn(rat, selClumps2Drop, &tmpNumRows);
            
            if(tmpNumRows != numRows)
            {
                throw rsgis::img::RSGISImageCalcException("Number of rows read is not what was expected.");
            }
            
            size_t *newClumpIds = new size_t[numRows];
            size_t clumpID = 1;
            for(size_t i = 1; i < numRows; ++i)
            {
                if(selectCol[i] == 1)
                {
                    newClumpIds[i] = 0;
                }
                else
                {
                    newClumpIds[i] = clumpID++;
                }
            }
            
            RSGISDropClumpsRelabelling *calcImgValRelabel = new RSGISDropClumpsRelabelling(newClumpIds, numRows);
            rsgis::img::RSGISCalcImage calcImageRelabel(calcImgValRelabel);
            calcImageRelabel.calcImage(&clumpsImage, 1, 0, outputImage, false, NULL, gdalFormat, GDT_UInt32);
            delete calcImgValRelabel;
            
            GDALDataset *outClumpsDataset = (GDALDataset *) GDALOpen(outputImage.c_str(), GA_Update);
            if(outClumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + outputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            outClumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            rsgis::rastergis::RSGISPopulateWithImageStats popImageStats;
            popImageStats.populateImageWithRasterGISStats(outClumpsDataset, true, true, 1);
            popImageStats.calcPyramids(outClumpsDataset);
            
            GDALClose(outClumpsDataset);
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    RSGISDropClumps::~RSGISDropClumps()
    {
        
    }
    
    
    
    

    RSGISDropClumpsRelabelling::RSGISDropClumpsRelabelling(size_t *newClumpIds, size_t nRows): rsgis::img::RSGISCalcImageValue(1)
    {
        this->newClumpIds = newClumpIds;
        this->nRows = nRows;
    }
    
    void RSGISDropClumpsRelabelling::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(intBandValues[0] > 0)
        {
            output[0] = newClumpIds[intBandValues[0]];
        }
        else
        {
            output[0] = 0;
        }
    }

    RSGISDropClumpsRelabelling::~RSGISDropClumpsRelabelling()
    {
        
    }
    
    
}}


