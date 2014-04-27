/*
 *  RSGISInterpolateClumpValues2Image.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 14/09/2013.
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

#include "RSGISInterpolateClumpValues2Image.h"

namespace rsgis{namespace rastergis{
    
    RSGISInterpolateClumpValues2Image::RSGISInterpolateClumpValues2Image()
    {
        
    }
    
    void RSGISInterpolateClumpValues2Image::interpolateImageFromClumps(GDALDataset *clumpsDataset, std::string selectField, std::string eastingsField, std::string northingsField, std::string valueField, std::string outputFile, std::string imageFormat, GDALDataType dataType, rsgis::math::RSGIS2DInterpolator *interpolator)throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            std::cout << "Import attribute tables to memory.\n";
            const GDALRasterAttributeTable *gdalAtt = clumpsDataset->GetRasterBand(1)->GetDefaultRAT();
            
            if((gdalAtt == NULL) || (gdalAtt->GetRowCount() == 0))
            {
                throw RSGISAttributeTableException("Input image does not have an attribute table.");
            }
            
            unsigned int numRATRows = gdalAtt->GetRowCount();
            //std::cout << "Number of RAT Rows: " << numRATRows << std::endl;
            
            RSGISRasterAttUtils attUtils;
            rsgis::img::RSGISImageUtils imgUtils;
            
            unsigned int selectIdx = attUtils.findColumnIndex(gdalAtt, selectField);
            unsigned int eastingsIdx = attUtils.findColumnIndex(gdalAtt, eastingsField);
            unsigned int northingsIdx = attUtils.findColumnIndex(gdalAtt, northingsField);
            unsigned int valueIdx = attUtils.findColumnIndex(gdalAtt, valueField);
            
            double eastings = 0;
            double northings = 0;
            double value = 0;
            
            unsigned int numSelectValues = 0;
            std::vector<rsgis::math::RSGISInterpolatorDataPoint> *pts = new std::vector<rsgis::math::RSGISInterpolatorDataPoint>();
            for(unsigned int i = 0; i < numRATRows; ++i)
            {
                //std::cout << "Row " << i << std::endl;
                if(gdalAtt->GetValueAsInt(i, selectIdx) == 1)
                {
                    eastings = gdalAtt->GetValueAsDouble(i, eastingsIdx);
                    northings = gdalAtt->GetValueAsDouble(i, northingsIdx);
                    value = gdalAtt->GetValueAsDouble(i, valueIdx);
                    std::cout << "Clump " << i << " [" << eastings << "," << northings << "] = " << value << std::endl;
                    pts->push_back(rsgis::math::RSGISInterpolatorDataPoint(eastings, northings, value));
                    ++numSelectValues;
                }
            }
            std::cout << "There are " << numSelectValues << " selected features.\n";
            
            interpolator->initInterpolator(pts);
            
            GDALDataset *outputDS = imgUtils.createCopy(clumpsDataset, 1, outputFile, imageFormat, dataType);
            
            rsgis::img::RSGISPopulateImageFromInterpolator popImg;
            popImg.populateImage(interpolator, outputDS);
            
            GDALClose(outputDS);
            delete pts;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
        
    RSGISInterpolateClumpValues2Image::~RSGISInterpolateClumpValues2Image()
    {
        
    }
    
    
}}



