/*
 *  RSGISCmdImageUtils.cpp
 *
 *
 *  Created by Pete Bunting on 03/05/2013.
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

#include "RSGISCmdImageUtils.h"
#include "RSGISCmdParent.h"

#include "common/RSGISImageException.h"

#include "img/RSGISBandMath.h"
#include "img/RSGISImageMaths.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISStretchImage.h"
#include "img/RSGISMaskImage.h"


namespace rsgis{ namespace cmds {
    
    
    void executeStretchImage(std::string inputImage, std::string outputImage, bool saveOutStats, std::string outStatsFile, bool ignoreZeros, bool onePassSD, std::string gdalFormat, RSGISLibDataType outDataType, RSGISStretches stretchType, float stretchParam)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *inDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISStretchImage stretchImg = rsgis::img::RSGISStretchImage(inDataset, outputImage, saveOutStats, outStatsFile, ignoreZeros, onePassSD, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            if(stretchType == linearMinMax)
            {
                stretchImg.executeLinearMinMaxStretch();
            }
            else if(stretchType == linearPercent)
            {
                stretchImg.executeLinearPercentStretch(stretchParam);
            }
            else if(stretchType == linearStdDev)
            {
                stretchImg.executeLinearStdDevStretch(stretchParam);
            }
            else if(stretchType == histogram)
            {
                stretchImg.executeHistogramStretch();
            }
            else if(stretchType == exponential)
            {
                stretchImg.executeExponentialStretch();
            }
            else if(stretchType == logarithmic)
            {
                stretchImg.executeLogrithmicStretch();
            }
            else if(stretchType == powerLaw)
            {
                stretchImg.executePowerLawStretch(stretchParam);
            }
            else
            {
                throw RSGISException("Stretch is not recognised.");
            }
            
            GDALClose(inDataset);
            GDALDestroyDriverManager();
        }
        catch(RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    
    void executeStretchImage(std::string inputImage, std::string imageMask, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType, float outValue, float maskValue)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *mask = (GDALDataset *) GDALOpenShared(imageMask.c_str(), GA_ReadOnly);
            if(mask == NULL)
            {
                std::string message = std::string("Could not open image ") + imageMask;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISMaskImage maskImage =  rsgis::img::RSGISMaskImage();
            maskImage.maskImage(dataset, mask, outputImage, gdalFormat, RSGIS_to_GDAL_Type(outDataType), outValue, maskValue);
            
            GDALClose(dataset);
            GDALClose(mask);
            GDALDestroyDriverManager();
        }
        catch(RSGISException& e)
        {
            throw e;
        }
    }
    
}}

