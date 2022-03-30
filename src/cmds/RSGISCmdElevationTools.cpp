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
            auto *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
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
            
            auto calcSlope = rsgis::calib::RSGISCalcSlope(0, imageEWRes, imageNSRes, outAngleUnit, demNoDataVal);
            auto calcImage = rsgis::img::RSGISCalcImage(&calcSlope, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Float32);

            GDALClose(dataset);
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }


    void executeCalcSlopeImgPxlRes(std::string demImage, std::string demPxlResImage, std::string outputImage, RSGISAngleMeasure outAngleUnit, std::string outImageFormat)
    {
        try
        {
            GDALAllRegister();
            auto **datasets = new GDALDataset*[2];

            std::cout << "Open " << demImage << std::endl;
            datasets[0] = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + demImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            std::cout << "Open " << demPxlResImage << std::endl;
            datasets[1] = (GDALDataset *) GDALOpen(demPxlResImage.c_str(), GA_ReadOnly);
            if(datasets[1] == NULL)
            {
                std::string message = std::string("Could not open image ") + demPxlResImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            double demNoDataVal = 0.0;
            int demNoDataValAvail = false;
            demNoDataVal = datasets[0]->GetRasterBand(1)->GetNoDataValue(&demNoDataValAvail);
            if(!demNoDataValAvail)
            {
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                delete[] datasets;
                throw rsgis::RSGISException("The DEM image file does not have a no data value defined. ");
            }

            unsigned int n_dem_bands = datasets[0]->GetRasterCount();
            unsigned int ew_pxl_res_band = n_dem_bands;
            unsigned int ns_pxl_res_band = n_dem_bands+1;

            auto calcSlope = rsgis::calib::RSGISCalcSlopePerPxlRes(0, outAngleUnit, demNoDataVal, ew_pxl_res_band, ns_pxl_res_band);

            auto calcImage = rsgis::img::RSGISCalcImage(&calcSlope, "", true);
            calcImage.calcImageWindowData(datasets, 2, outputImage, 3, outImageFormat, GDT_Float32);

            GDALClose(datasets[0]);
            GDALClose(datasets[1]);
            delete[] datasets;
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
            auto *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
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

            auto *transformation = new double[6];
            dataset->GetGeoTransform(transformation);
            
            float imageEWRes = transformation[1];
            float imageNSRes = transformation[5];
            
            if(imageNSRes < 0)
            {
                imageNSRes = imageNSRes * (-1);
            }
            
            delete[] transformation;

            auto calcAspect = rsgis::calib::RSGISCalcAspect(0, imageEWRes, imageNSRes, demNoDataVal);

            auto calcImage = rsgis::img::RSGISCalcImage(&calcAspect, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Float32);

            GDALClose(dataset);
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeCalcAspectImgPxlRes(std::string demImage, std::string demPxlResImage, std::string outputImage, std::string outImageFormat)
    {
        try
        {
            GDALAllRegister();
            auto **datasets = new GDALDataset*[2];

            std::cout << "Open " << demImage << std::endl;
            datasets[0] = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + demImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            std::cout << "Open " << demPxlResImage << std::endl;
            datasets[1] = (GDALDataset *) GDALOpen(demPxlResImage.c_str(), GA_ReadOnly);
            if(datasets[1] == NULL)
            {
                std::string message = std::string("Could not open image ") + demPxlResImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            double demNoDataVal = 0.0;
            int demNoDataValAvail = false;
            demNoDataVal = datasets[0]->GetRasterBand(1)->GetNoDataValue(&demNoDataValAvail);
            if(!demNoDataValAvail)
            {
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                delete[] datasets;
                throw rsgis::RSGISException("The DEM image file does not have a no data value defined. ");
            }

            unsigned int n_dem_bands = datasets[0]->GetRasterCount();
            unsigned int ew_pxl_res_band = n_dem_bands;
            unsigned int ns_pxl_res_band = n_dem_bands+1;

            auto calcAspect = rsgis::calib::RSGISCalcAspectPerPxlRes(0, demNoDataVal, ew_pxl_res_band, ns_pxl_res_band);
            auto calcImage = rsgis::img::RSGISCalcImage(&calcAspect, "", true);
            calcImage.calcImageWindowData(datasets, 2, outputImage, 3, outImageFormat, GDT_Float32);


            GDALClose(datasets[0]);
            GDALClose(datasets[1]);
            delete[] datasets;
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
            auto *dataset = (GDALDataset *) GDALOpen(aspectImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + aspectImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            auto catAspect = rsgis::calib::RSGISRecodeAspect();
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(&catAspect, "", true);
            calcImage.calcImage(&dataset, 1, outputImage, false, NULL, outImageFormat, GDT_Byte);
            
            GDALClose(dataset);
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
            auto *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
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

            auto *transformation = new double[6];
            dataset->GetGeoTransform(transformation);
            
            float imageEWRes = transformation[1];
            float imageNSRes = transformation[5];
            
            if(imageNSRes < 0)
            {
                imageNSRes = imageNSRes * (-1);
            }
            
            delete[] transformation;

            auto calcHillshade = rsgis::calib::RSGISCalcHillShade(0, imageEWRes, imageNSRes, solarZenith, solarAzimuth, demNoDataVal);
            auto calcImage = rsgis::img::RSGISCalcImage(&calcHillshade, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Byte);

            GDALClose(dataset);
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeCalcHillshadeImgPxlRes(std::string demImage, std::string demPxlResImage, std::string outputImage, float solarAzimuth, float solarZenith, std::string outImageFormat)
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

            auto **datasets = new GDALDataset*[2];

            std::cout << "Open " << demImage << std::endl;
            datasets[0] = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + demImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            std::cout << "Open " << demPxlResImage << std::endl;
            datasets[1] = (GDALDataset *) GDALOpen(demPxlResImage.c_str(), GA_ReadOnly);
            if(datasets[1] == NULL)
            {
                std::string message = std::string("Could not open image ") + demPxlResImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            double demNoDataVal = 0.0;
            int demNoDataValAvail = false;
            demNoDataVal = datasets[0]->GetRasterBand(1)->GetNoDataValue(&demNoDataValAvail);
            if(!demNoDataValAvail)
            {
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                delete[] datasets;
                throw rsgis::RSGISException("The DEM image file does not have a no data value defined. ");
            }

            unsigned int n_dem_bands = datasets[0]->GetRasterCount();
            unsigned int ew_pxl_res_band = n_dem_bands;
            unsigned int ns_pxl_res_band = n_dem_bands+1;

            auto calcHillshade = rsgis::calib::RSGISCalcHillShadePerPxlRes(0, solarZenith, solarAzimuth, demNoDataVal, ew_pxl_res_band, ns_pxl_res_band);
            auto calcImage = rsgis::img::RSGISCalcImage(&calcHillshade, "", true);
            calcImage.calcImageWindowData(datasets, 2, outputImage, 3, outImageFormat, GDT_Byte);

            GDALClose(datasets[0]);
            GDALClose(datasets[1]);
            delete[] datasets;
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
            auto *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
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

            auto *transformation = new double[6];
            dataset->GetGeoTransform(transformation);
            
            float imageEWRes = transformation[1];
            float imageNSRes = transformation[5];
            
            if(imageNSRes < 0)
            {
                imageNSRes = imageNSRes * (-1);
            }
            
            delete[] transformation;

            auto calcShadowMask = rsgis::calib::RSGISCalcShadowBinaryMask(dataset, 0, imageEWRes, imageNSRes, solarZenith, solarAzimuth, maxHeight, demNoDataVal);

            auto calcImage = rsgis::img::RSGISCalcImage(&calcShadowMask, "", true);
            
            calcImage.calcImageWindowDataExtent(&dataset, 1, outputImage, 3, outImageFormat, GDT_Byte);
            
            GDALClose(dataset);
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
            auto *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
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

            auto *transformation = new double[6];
            dataset->GetGeoTransform(transformation);
            
            float imageEWRes = transformation[1];
            float imageNSRes = transformation[5];
            
            if(imageNSRes < 0)
            {
                imageNSRes = imageNSRes * (-1);
            }
            
            delete[] transformation;

            auto calcIncidAngle = rsgis::calib::RSGISCalcRayIncidentAngle(0, imageEWRes, imageNSRes, solarZenith, solarAzimuth, demNoDataVal);
            auto calcImage = rsgis::img::RSGISCalcImage(&calcIncidAngle, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Float32);

            GDALClose(dataset);
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
            auto *dataset = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
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

            auto *transformation = new double[6];
            dataset->GetGeoTransform(transformation);
            
            float imageEWRes = transformation[1];
            float imageNSRes = transformation[5];
            
            if(imageNSRes < 0)
            {
                imageNSRes = imageNSRes * (-1);
            }
            
            delete[] transformation;

            auto calcExitAngle = rsgis::calib::RSGISCalcRayExitanceAngle(0, imageEWRes, imageNSRes, viewZenith, viewAzimuth, demNoDataVal);

            auto calcImage = rsgis::img::RSGISCalcImage(&calcExitAngle, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, 3, outImageFormat, GDT_Float32);

            GDALClose(dataset);
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
            auto **datasets = new GDALDataset*[2];
            
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

            auto dtmFilter = rsgis::calib::RSGISFilterDTMWithAspectMedianFilter(aspectRange, demNoDataVal);
            auto calcImage = rsgis::img::RSGISCalcImage(&dtmFilter, "", true);
            calcImage.calcImageWindowData(datasets, 2, outputImage, winSize, outImageFormat, GDT_Float32);

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
            auto *inImgDS = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(inImgDS == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            std::cout << "Open " << validDataImg << std::endl;
            auto *inValidImgDS = (GDALDataset *) GDALOpen(validDataImg.c_str(), GA_ReadOnly);
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
            auto *inImgDS = (GDALDataset *) GDALOpen(demImage.c_str(), GA_ReadOnly);
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

            auto detreadDEM = rsgis::calib::RSGISDetreadDEMUsingPlaneFit(demNoDataVal, winSize);
            auto calcImage = rsgis::img::RSGISCalcImage(&detreadDEM, "", true);
            calcImage.calcImageWindowData(&inImgDS, 1, outputImage, winSize, outImageFormat, GDT_Float32);

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



