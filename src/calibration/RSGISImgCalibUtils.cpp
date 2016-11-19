/*
 *  RSGISImgCalibUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/11/2016.
 *  Copyright 2016 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#include "RSGISImgCalibUtils.h"

namespace rsgis{namespace calib{
    
    void RSGISImgCalibUtils::calcNadirImgViewAngle(GDALDataset *imgFPDataset, std::string outViewAngleImg, std::string gdalFormat, double sateAltitude, std::string minXXCol, std::string minXYCol, std::string maxXXCol, std::string maxXYCol, std::string minYXCol, std::string minYYCol, std::string maxYXCol, std::string maxYYCol) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            std::cout.precision(12);
            const GDALRasterAttributeTable *fpRAT = imgFPDataset->GetRasterBand(1)->GetDefaultRAT();
            
            rsgis::rastergis::RSGISRasterAttUtils ratUtils;
            double minXX = ratUtils.readDoubleColumnVal(fpRAT, minXXCol, 1);
            double minXY = ratUtils.readDoubleColumnVal(fpRAT, minXYCol, 1);
            
            double maxXX = ratUtils.readDoubleColumnVal(fpRAT, maxXXCol, 1);
            double maxXY = ratUtils.readDoubleColumnVal(fpRAT, maxXYCol, 1);
            
            double minYX = ratUtils.readDoubleColumnVal(fpRAT, minYXCol, 1);
            double minYY = ratUtils.readDoubleColumnVal(fpRAT, minYYCol, 1);
            
            double maxYX = ratUtils.readDoubleColumnVal(fpRAT, maxYXCol, 1);
            double maxYY = ratUtils.readDoubleColumnVal(fpRAT, maxYYCol, 1);
            
            double cenLineTX = maxYX + (maxXX - maxYX)/2;
            double cenLineTY = maxXY + (maxYY - maxXY)/2;
            double cenLineBX = minXX + (minYX - minXX)/2;
            double cenLineBY = minYY + (minXY - minYY)/2;
            
            std::cout << "Center Line: Top[" << cenLineTX << ", " << cenLineTY << "][" << cenLineBX << ", " << cenLineBY << "]\n";
            
            double diffX = cenLineTX - cenLineBX;
            double diffY = cenLineTY - cenLineBY;
            
            double clSlope = diffY / diffX;
            double clOff = cenLineTY - (clSlope*cenLineTX);
            
            RSGISCalcNadirViewAngle calcNadirViewAngle = RSGISCalcNadirViewAngle(clSlope, clOff, sateAltitude);
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(&calcNadirViewAngle, "", true);
            calcImage.calcImageExtent(&imgFPDataset, 1, outViewAngleImg, gdalFormat, GDT_Float32);
            
        }
        catch(RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    
    RSGISCalcNadirViewAngle::RSGISCalcNadirViewAngle(double clSlope, double clOff, double sateAltitude):rsgis::img::RSGISCalcImageValue(1)
    {
        this->clSlope = clSlope;
        this->clOff = clOff;
        this->sateAltitude = sateAltitude;
    }
    
    void RSGISCalcNadirViewAngle::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException)
    {
        if(numBands != 1)
        {
            throw rsgis::img::RSGISImageCalcException("RSGISCalcNadirViewAngle: Must have only one input image band.");
        }
        output[0] = 99999;
        int pxlVal = boost::lexical_cast<int>(bandValues[0]);
        if(pxlVal == 0)
        {
            output[0] = 99999;
        }
        else
        {
            double ptX = extent.getMinX() + extent.getWidth()/2;
            double ptY = extent.getMinY() + extent.getHeight()/2;
            
            
            double orthSlope = -1/(clSlope);
            double orthLineOff = ptY-(orthSlope*ptX);
            
            double intPtX = (orthLineOff-clOff)/(clSlope-orthSlope);
            double intPtY = (intPtX*clSlope) + clOff;
            
            double dist = sqrt((intPtX - ptX)*(intPtX - ptX) + (intPtY - ptY)*(intPtY - ptY));

            double angleDeg = atan(sateAltitude/dist) * 180 / 3.141592653589793; // Divided by PI
            
            output[0] = 90 - angleDeg; // From Sensor point of view.
        }
    }
    
    RSGISCalcNadirViewAngle::~RSGISCalcNadirViewAngle()
    {
        
    }
    
    
}}


