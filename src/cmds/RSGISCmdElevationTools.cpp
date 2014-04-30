/*
 *  RSGISCmdElevationTools.cpp
 *
 *
 *  Created by Pete Bunting on 30/04/2014.
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

#include "RSGISCmdElevationTools.h"
#include "RSGISCmdParent.h"

#include "calibration/RSGISDEMTools.h"

namespace rsgis{ namespace cmds {
    
    void executeCalcSlope(std::string demImage, std::string outputImage, RSGISAngleMeasure outAngleUnit, std::string outImageFormat)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            std::cout << "Open " << demImage << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + demImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            double *transformation = new double[6];
            dataset->GetGeoTransform(transformation);
            
            float imageEWRes = transformation[1];
            float imageNSRes = transformation[5];
            
            if(imageNSRes < 0)
            {
                imageNSRes = imageNSRes * (-1);
            }
            
            delete[] transformation;
            
            rsgis::calib::RSGISCalcSlope *calcSlope = new rsgis::calib::RSGISCalcSlope(1, 0, imageEWRes, imageNSRes, outAngleUnit);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcSlope, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Float32);
            
            
            GDALClose(dataset);
            delete calcSlope;
        }
        catch(rsgis::RSGISException e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    
    void executeCalcAspect(std::string demImage, std::string outputImage, std::string outImageFormat)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            std::cout << "Open " << demImage << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + demImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            double *transformation = new double[6];
            dataset->GetGeoTransform(transformation);
            
            float imageEWRes = transformation[1];
            float imageNSRes = transformation[5];
            
            if(imageNSRes < 0)
            {
                imageNSRes = imageNSRes * (-1);
            }
            
            delete[] transformation;
            
            rsgis::calib::RSGISCalcAspect *calcAspect = new rsgis::calib::RSGISCalcAspect(1, 0, imageEWRes, imageNSRes);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcAspect, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Float32);
            
            
            GDALClose(dataset);
            delete calcAspect;
        }
        catch(rsgis::RSGISException e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    
    void executeCalcHillshade(std::string demImage, std::string outputImage, float solarAzimuth, float solarZenith, std::string outImageFormat)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            std::cout << "Open " << demImage << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + demImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            double *transformation = new double[6];
            dataset->GetGeoTransform(transformation);
            
            float imageEWRes = transformation[1];
            float imageNSRes = transformation[5];
            
            if(imageNSRes < 0)
            {
                imageNSRes = imageNSRes * (-1);
            }
            
            delete[] transformation;
            
            rsgis::calib::RSGISCalcHillShade *calcHillshade = new rsgis::calib::RSGISCalcHillShade(1, 0, imageEWRes, imageNSRes, solarZenith, solarAzimuth);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcHillshade, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Byte);
            
            
            GDALClose(dataset);
            delete calcHillshade;
        }
        catch(rsgis::RSGISException e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    
    void executeCalcShadowMask(std::string demImage, std::string outputImage, float solarAzimuth, float solarZenith, float maxHeight, std::string outImageFormat)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            if((solarZenith < 0) | (solarZenith > 90))
            {
                throw rsgis::RSGISException("The solar zenith should be between 0 and 90 degrees.");
            }
            
            if((solarAzimuth < 0) | (solarAzimuth > 360))
            {
                throw rsgis::RSGISException("The solar azimuth should be between 0 and 360 degrees.");
            }
            
            solarZenith = 90 - solarZenith;
            
            std::cout << "Open " << demImage << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + demImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            double *transformation = new double[6];
            dataset->GetGeoTransform(transformation);
            
            float imageEWRes = transformation[1];
            float imageNSRes = transformation[5];
            
            if(imageNSRes < 0)
            {
                imageNSRes = imageNSRes * (-1);
            }
            
            delete[] transformation;
            
            rsgis::calib::RSGISCalcShadowBinaryMask *calcShadowMask = new rsgis::calib::RSGISCalcShadowBinaryMask(1, dataset, 1, imageEWRes, imageNSRes, solarZenith, solarAzimuth, maxHeight);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcShadowMask, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Byte);
            
            
            GDALClose(dataset);
            delete calcShadowMask;
        }
        catch(rsgis::RSGISException e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    
    void executeCalcLocalIncidenceAngle(std::string demImage, std::string outputImage, float solarAzimuth, float solarZenith, std::string outImageFormat)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            std::cout << "Open " << demImage << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + demImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            double *transformation = new double[6];
            dataset->GetGeoTransform(transformation);
            
            float imageEWRes = transformation[1];
            float imageNSRes = transformation[5];
            
            if(imageNSRes < 0)
            {
                imageNSRes = imageNSRes * (-1);
            }
            
            delete[] transformation;
            
            rsgis::calib::RSGISCalcRayIncidentAngle *calcIncidAngle = new rsgis::calib::RSGISCalcRayIncidentAngle(1, 0, imageEWRes, imageNSRes, solarZenith, solarAzimuth);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcIncidAngle, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Float32);
            
            
            GDALClose(dataset);
            delete calcIncidAngle;
        }
        catch(rsgis::RSGISException e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    
    void executeCalcLocalExitanceAngle(std::string demImage, std::string outputImage, float azimuth, float zenith, std::string outImageFormat)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            std::cout << "Open " << demImage << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + demImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            double *transformation = new double[6];
            dataset->GetGeoTransform(transformation);
            
            float imageEWRes = transformation[1];
            float imageNSRes = transformation[5];
            
            if(imageNSRes < 0)
            {
                imageNSRes = imageNSRes * (-1);
            }
            
            delete[] transformation;
            
            rsgis::calib::RSGISCalcRayExitanceAngle *calcExitAngle = new rsgis::calib::RSGISCalcRayExitanceAngle(1, 0, imageEWRes, imageNSRes, zenith, azimuth);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcExitAngle, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Float32);
            
            
            GDALClose(dataset);
            delete calcExitAngle;
        }
        catch(rsgis::RSGISException e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    
}}



