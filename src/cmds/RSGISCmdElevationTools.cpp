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
#include "calibration/RSGISHydroDEMFillSoilleGratin94.h"

namespace rsgis{ namespace cmds {
    
    void executeCalcSlope(std::string demImage, std::string outputImage, RSGISAngleMeasure outAngleUnit, std::string outImageFormat)
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
            
            double demNoDataVal = 0.0;
            int demNoDataValAvail = false;
            demNoDataVal = dataset->GetRasterBand(1)->GetNoDataValue(&demNoDataValAvail);
            if(!demNoDataValAvail)
            {
                GDALClose(dataset);
                throw rsgis::RSGISException("The DEM image file does not have a no data value defined. ");
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
            
            rsgis::calib::RSGISCalcSlope *calcSlope = new rsgis::calib::RSGISCalcSlope(1, 0, imageEWRes, imageNSRes, outAngleUnit, demNoDataVal);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcSlope, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Float32);
            
            
            GDALClose(dataset);
            delete calcSlope;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    
    void executeCalcAspect(std::string demImage, std::string outputImage, std::string outImageFormat)
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
            
            double demNoDataVal = 0.0;
            int demNoDataValAvail = false;
            demNoDataVal = dataset->GetRasterBand(1)->GetNoDataValue(&demNoDataValAvail);
            if(!demNoDataValAvail)
            {
                GDALClose(dataset);
                throw rsgis::RSGISException("The DEM image file does not have a no data value defined. ");
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
            
            rsgis::calib::RSGISCalcAspect *calcAspect = new rsgis::calib::RSGISCalcAspect(1, 0, imageEWRes, imageNSRes, demNoDataVal);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcAspect, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Float32);
            
            
            GDALClose(dataset);
            delete calcAspect;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    void executeCatagoriseAspect(std::string aspectImage, std::string outputImage, std::string outImageFormat)
    {
        try
        {
            GDALAllRegister();
            
            std::cout << "Open Aspect Image: " << aspectImage << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(aspectImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + aspectImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::calib::RSGISRecodeAspect *catAspect = new rsgis::calib::RSGISRecodeAspect();
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(catAspect, "", true);
            calcImage.calcImage(&dataset, 1, outputImage, false, NULL, outImageFormat, GDT_Byte);
            
            GDALClose(dataset);
            delete catAspect;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    
    void executeCalcHillshade(std::string demImage, std::string outputImage, float solarAzimuth, float solarZenith, std::string outImageFormat)
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
            
            double demNoDataVal = 0.0;
            int demNoDataValAvail = false;
            demNoDataVal = dataset->GetRasterBand(1)->GetNoDataValue(&demNoDataValAvail);
            if(!demNoDataValAvail)
            {
                GDALClose(dataset);
                throw rsgis::RSGISException("The DEM image file does not have a no data value defined. ");
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
            
            rsgis::calib::RSGISCalcHillShade *calcHillshade = new rsgis::calib::RSGISCalcHillShade(1, 0, imageEWRes, imageNSRes, solarZenith, solarAzimuth, demNoDataVal);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcHillshade, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Byte);
            
            
            GDALClose(dataset);
            delete calcHillshade;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    
    void executeCalcShadowMask(std::string demImage, std::string outputImage, float solarAzimuth, float solarZenith, float maxHeight, std::string outImageFormat)
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
            
            std::cout << "Open " << demImage << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + demImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            double demNoDataVal = 0.0;
            int demNoDataValAvail = false;
            demNoDataVal = dataset->GetRasterBand(1)->GetNoDataValue(&demNoDataValAvail);
            if(!demNoDataValAvail)
            {
                GDALClose(dataset);
                throw rsgis::RSGISException("The DEM image file does not have a no data value defined. ");
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
            
            rsgis::calib::RSGISCalcShadowBinaryMask *calcShadowMask = new rsgis::calib::RSGISCalcShadowBinaryMask(1, dataset, 0, imageEWRes, imageNSRes, solarZenith, solarAzimuth, maxHeight, demNoDataVal);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcShadowMask, "", true);
            
            calcImage.calcImageWindowDataExtent(&dataset, 1, outputImage, 3, outImageFormat, GDT_Byte);
            
            GDALClose(dataset);
            delete calcShadowMask;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    
    void executeCalcLocalIncidenceAngle(std::string demImage, std::string outputImage, float solarAzimuth, float solarZenith, std::string outImageFormat)
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
            
            double demNoDataVal = 0.0;
            int demNoDataValAvail = false;
            demNoDataVal = dataset->GetRasterBand(1)->GetNoDataValue(&demNoDataValAvail);
            if(!demNoDataValAvail)
            {
                GDALClose(dataset);
                throw rsgis::RSGISException("The DEM image file does not have a no data value defined. ");
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
            
            rsgis::calib::RSGISCalcRayIncidentAngle *calcIncidAngle = new rsgis::calib::RSGISCalcRayIncidentAngle(1, 0, imageEWRes, imageNSRes, solarZenith, solarAzimuth, demNoDataVal);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcIncidAngle, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Float32);
            
            
            GDALClose(dataset);
            delete calcIncidAngle;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    
    void executeCalcLocalExitanceAngle(std::string demImage, std::string outputImage, float viewAzimuth, float viewZenith, std::string outImageFormat)
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
            
            double demNoDataVal = 0.0;
            int demNoDataValAvail = false;
            demNoDataVal = dataset->GetRasterBand(1)->GetNoDataValue(&demNoDataValAvail);
            if(!demNoDataValAvail)
            {
                GDALClose(dataset);
                throw rsgis::RSGISException("The DEM image file does not have a no data value defined. ");
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
            
            rsgis::calib::RSGISCalcRayExitanceAngle *calcExitAngle = new rsgis::calib::RSGISCalcRayExitanceAngle(1, 0, imageEWRes, imageNSRes, viewZenith, viewAzimuth, demNoDataVal);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcExitAngle, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Float32);
            
            
            GDALClose(dataset);
            delete calcExitAngle;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    void executeDTMAspectMedianFilter(std::string demImage, std::string aspectImage, std::string outputImage, float aspectRange, int winHSize, std::string outImageFormat)
    {
        try
        {
            GDALAllRegister();
            GDALDataset **datasets = new GDALDataset*[2];
            
            std::cout << "Open " << demImage << std::endl;
            datasets[0] = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + demImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            double demNoDataVal = 0.0;
            int demNoDataValAvail = false;
            demNoDataVal = datasets[0]->GetRasterBand(1)->GetNoDataValue(&demNoDataValAvail);
            if(!demNoDataValAvail)
            {
                GDALClose(datasets[0]);
                delete[] datasets;
                throw rsgis::RSGISException("The DEM image file does not have a no data value defined. ");
            }
            
            std::cout << "Open " << aspectImage << std::endl;
            datasets[1] = (GDALDataset *) GDALOpen(aspectImage.c_str(), GA_ReadOnly);
            if(datasets[1] == NULL)
            {
                std::string message = std::string("Could not open image ") + aspectImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            int winSize = (winHSize * 2) + 1;
            
            rsgis::calib::RSGISFilterDTMWithAspectMedianFilter *dtmFilter = new rsgis::calib::RSGISFilterDTMWithAspectMedianFilter(aspectRange, demNoDataVal);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(dtmFilter, "", true);
            calcImage.calcImageWindowData(datasets, 2, outputImage, winSize, outImageFormat, GDT_Float32);
            
            delete dtmFilter;

            GDALClose(datasets[0]);
            GDALClose(datasets[1]);
            delete[] datasets;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeDEMFillSoilleGratin1994(std::string inImage, std::string validDataImg, std::string outputImage, std::string outImageFormat)
    {
        try
        {
            GDALAllRegister();
            
            std::cout << "Open " << inImage << std::endl;
            GDALDataset *inImgDS = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(inImgDS == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            std::cout << "Open " << validDataImg << std::endl;
            GDALDataset *inValidImgDS = (GDALDataset *) GDALOpen(validDataImg.c_str(), GA_ReadOnly);
            if(inValidImgDS == NULL)
            {
                std::string message = std::string("Could not open image ") + validDataImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataType imgDT = inImgDS->GetRasterBand(1)->GetRasterDataType();
            if((imgDT == GDT_Float32) | (imgDT == GDT_Float64) | (imgDT == GDT_Unknown) | (imgDT == GDT_CInt16) | (imgDT == GDT_CInt32) | (imgDT == GDT_CFloat32) | (imgDT == GDT_CFloat64))
            {
                throw rsgis::RSGISImageException("Input image must be of an integer data type.");
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            GDALDataset *outImgDS = imgUtils.createCopy(inImgDS, 1, outputImage, outImageFormat, imgDT);
            
            rsgis::calib::RSGISHydroDEMFillSoilleGratin94 fillDEMInst;
            fillDEMInst.performSoilleGratin94Fill(inImgDS, inValidImgDS, outImgDS, true);
            
            GDALClose(inImgDS);
            GDALClose(inValidImgDS);
            GDALClose(outImgDS);
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executePlaneFitDetreadDEM(std::string demImage, std::string outputImage, std::string outImageFormat, int winSize)
    {
        try
        {
            GDALAllRegister();
            
            std::cout << "Open " << demImage << std::endl;
            GDALDataset *inImgDS = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
            if(inImgDS == NULL)
            {
                std::string message = std::string("Could not open image ") + demImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            double demNoDataVal = 0.0;
            int demNoDataValAvail = false;
            demNoDataVal = inImgDS->GetRasterBand(1)->GetNoDataValue(&demNoDataValAvail);
            if(!demNoDataValAvail)
            {
                GDALClose(inImgDS);
                throw rsgis::RSGISException("The DEM image file does not have a no data value defined. ");
            }
            
            rsgis::calib::RSGISDetreadDEMUsingPlaneFit *detreadDEM = new rsgis::calib::RSGISDetreadDEMUsingPlaneFit(demNoDataVal, winSize);
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(detreadDEM, "", true);
            calcImage.calcImageWindowData(&inImgDS, 1, outputImage, winSize, outImageFormat, GDT_Float32);
            delete detreadDEM;
            
            GDALClose(inImgDS);
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
}}



