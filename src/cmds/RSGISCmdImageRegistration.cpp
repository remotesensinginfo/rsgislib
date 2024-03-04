/*
 *  RSGISCmdImageRegistrations.cpp
 *
 *
 *  Created by Dan Clewley on 08/09/2013.
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

#include <boost/filesystem.hpp>

#include "RSGISCmdImageRegistration.h"
#include "RSGISCmdParent.h"

#include "registration/RSGISImageRegistration.h"
#include "registration/RSGISBasicImageRegistration.h"
#include "registration/RSGISImageSimilarityMetric.h"
#include "registration/RSGISStandardImageSimilarityMetrics.h"
#include "registration/RSGISSingleConnectLayerImageRegistration.h"
#include "registration/RSGISAddGCPsGDAL.h"
#include "registration/RSGISFindImageOffset.h"


#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCopyImage.h"

#include "utils/RSGISTextUtils.h"



namespace rsgis{ namespace cmds {

    std::pair<double, double> excecuteFindImageOffset(std::string inputReferenceImage, std::string inputFloatingmage,
                                                      std::vector<unsigned int> refImageBands,
                                                      std::vector<unsigned int> fltImageBands,
                                                      unsigned int xSearch, unsigned int ySearch,
                                                      unsigned int metricTypeInt,
                                                      int subPixelResolution)
    {
        std::pair<double, double> imgOffsets;
        try
        {
            rsgis::utils::RSGISTextUtils txtUtils;
            GDALAllRegister();
            GDALDataset *inRefDataset = nullptr;
            GDALDataset *inFloatDataset = nullptr;

            inRefDataset = (GDALDataset *) GDALOpenShared(inputReferenceImage.c_str(), GA_ReadOnly);
            if(inRefDataset == nullptr)
            {
                std::string message = std::string("Could not open image ") + inputReferenceImage;
                throw rsgis::RSGISException(message.c_str());
            }
            int nRefBands = inRefDataset->GetRasterCount();

            inFloatDataset = (GDALDataset *) GDALOpenShared(inputFloatingmage.c_str(), GA_ReadOnly);
            if(inFloatDataset == nullptr)
            {
                std::string message = std::string("Could not open image ") + inputFloatingmage;
                throw rsgis::RSGISException(message.c_str());
            }
            int nFltBands = inFloatDataset->GetRasterCount();

            if(refImageBands.size() != fltImageBands.size())
            {
                throw rsgis::RSGISException("The number of bands specified from the reference and floating images must be the same.");
            }

            std::vector<unsigned int> refImgBandsArrIdx;
            std::vector<unsigned int> fltImgBandsArrIdx;

            double refImgNoData = 0;
            int useRefImgNoData = false;
            double fltImgNoData = 0;
            int useFltImgNoData = false;

            bool first = true;
            for(auto iterBand = refImageBands.begin(); iterBand != refImageBands.end(); ++iterBand)
            {
                if((*iterBand) < 1)
                {
                    throw rsgis::RSGISException("Image band indexing starts at 1");
                }
                if((*iterBand) > nRefBands)
                {
                    std::string message = std::string("Image band ") + txtUtils.uInttostring((*iterBand)) + std::string(" is not within the reference image.");
                    throw rsgis::RSGISException(message.c_str());
                }
                refImgBandsArrIdx.push_back((*iterBand)-1);
                if(first)
                {
                    refImgNoData = inRefDataset->GetRasterBand((*iterBand))->GetNoDataValue(&useRefImgNoData);
                    first = false;
                }
            }

            first = true;
            for(auto iterBand = fltImageBands.begin(); iterBand != fltImageBands.end(); ++iterBand)
            {
                if((*iterBand) < 1)
                {
                    throw rsgis::RSGISException("Image band indexing starts at 1");
                }
                if((*iterBand) > nFltBands)
                {
                    std::string message = std::string("Image band ") + txtUtils.uInttostring((*iterBand)) + std::string(" is not within the floating image.");
                    throw rsgis::RSGISException(message.c_str());
                }
                fltImgBandsArrIdx.push_back(nRefBands+((*iterBand)-1));
                if(first)
                {
                    fltImgNoData = inRefDataset->GetRasterBand((*iterBand))->GetNoDataValue(&useFltImgNoData);
                    first = false;
                }
            }

            rsgis::reg::RSGISImageCalcSimilarityMetric *similarityMetric = nullptr;
            if(metricTypeInt == 1) // euclidean
            {
                similarityMetric = new rsgis::reg::RSGISImgCalcEuclideanSimilarityMetric(refImgBandsArrIdx, fltImgBandsArrIdx, refImgNoData, useRefImgNoData, fltImgNoData, useFltImgNoData);
            }
            else if(metricTypeInt == 2) // sqdiff
            {
                similarityMetric = new rsgis::reg::RSGISImgCalcSquaredDifferenceSimilarityMetric(refImgBandsArrIdx, fltImgBandsArrIdx, refImgNoData, useRefImgNoData, fltImgNoData, useFltImgNoData);
            }
            else if(metricTypeInt == 3) // manhatten
            {
                similarityMetric = new rsgis::reg::RSGISImgCalcManhattanSimilarityMetric(refImgBandsArrIdx, fltImgBandsArrIdx, refImgNoData, useRefImgNoData, fltImgNoData, useFltImgNoData);
            }
            else if(metricTypeInt == 4) // correlation
            {
                similarityMetric = new rsgis::reg::RSGISImgCalcCorrelationSimilarityMetric(refImgBandsArrIdx, fltImgBandsArrIdx, refImgNoData, useRefImgNoData, fltImgNoData, useFltImgNoData);
            }
            else
            {
                throw rsgis::cmds::RSGISCmdException("Metric not recognised!");
            }
            std::cout << "subPixelResolution = " << subPixelResolution << std::endl;
            bool calcSubPxl = false;
            if(subPixelResolution > 0)
            {
                calcSubPxl = true;
            }

            // DO ANALYSIS!!
            rsgis::reg::RSGISFindImageOffset findImageOffset;
            imgOffsets = findImageOffset.findImageOffset(inRefDataset, inFloatDataset,
                                                                                   xSearch, ySearch,
                                                                                   similarityMetric,
                                                                                   calcSubPxl, subPixelResolution);

            GDALClose(inRefDataset);
            GDALClose(inFloatDataset);
        }
        catch(RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }

        return imgOffsets;
    }

    void excecuteBasicRegistration(std::string inputReferenceImage, std::string inputFloatingmage, int gcpGap,
                                                  float metricThreshold, int windowSize, int searchArea, float stdDevRefThreshold,
                                                  float stdDevFloatThreshold, int subPixelResolution, unsigned int metricTypeInt,
                                                  unsigned int outputType, std::string outputGCPFile) 
    {
        
        try
        {
            GDALAllRegister();
            GDALDataset *inRefDataset = nullptr;
            GDALDataset *inFloatDataset = nullptr;
            
            inRefDataset = (GDALDataset *) GDALOpenShared(inputReferenceImage.c_str(), GA_ReadOnly);
            if(inRefDataset == nullptr)
            {
                std::string message = std::string("Could not open image ") + inputReferenceImage;
                throw rsgis::RSGISException(message.c_str());
            }
            
            inFloatDataset = (GDALDataset *) GDALOpenShared(inputFloatingmage.c_str(), GA_ReadOnly);
            if(inFloatDataset == nullptr)
            {
                std::string message = std::string("Could not open image ") + inputFloatingmage;
                throw rsgis::RSGISException(message.c_str());
            }
            
            rsgis::reg::RSGISImageSimilarityMetric *similarityMetric = nullptr;
            if(metricTypeInt == 1) // euclidean
            {
                similarityMetric = new rsgis::reg::RSGISEuclideanSimilarityMetric();
            }
            else if(metricTypeInt == 2) // sqdiff
            {
                similarityMetric = new rsgis::reg::RSGISSquaredDifferenceSimilarityMetric();
            }
            else if(metricTypeInt == 3) // manhatten
            {
                similarityMetric = new rsgis::reg::RSGISManhattanSimilarityMetric();
            }
            else if(metricTypeInt == 4) // correlation
            {
                similarityMetric = new rsgis::reg::RSGISCorrelationSimilarityMetric();
            }
            else
            {
                throw rsgis::cmds::RSGISCmdException("Metric not recognised!");
            }
            
            rsgis::reg::RSGISImageRegistration *regImgs = new rsgis::reg::RSGISBasicImageRegistration(inRefDataset, inFloatDataset, gcpGap, metricThreshold,
                                                                                                      windowSize, searchArea, similarityMetric, stdDevRefThreshold,
                                                                                                      stdDevFloatThreshold, subPixelResolution);
            
            regImgs->runCompleteRegistration();
            
            if(outputType == 1) // envi_img2img
            {
                regImgs->exportTiePointsENVIImage2Image(outputGCPFile);
            }
            else if(outputType == 2) // envi_img2map
            {
                regImgs->exportTiePointsENVIImage2Map(outputGCPFile);
            }
            else if(outputType == 3) // rsgis_img2map
            {
                regImgs->exportTiePointsRSGISImage2Map(outputGCPFile);
            }
            else if(outputType == 4) // rsgis_mapoffs
            {
                regImgs->exportTiePointsRSGISMapOffs(outputGCPFile);
            }
            
            delete similarityMetric;
            delete regImgs;
            
            GDALClose(inRefDataset);
            GDALClose(inFloatDataset);
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
    
    void excecuteSingleLayerConnectedRegistration(std::string inputReferenceImage, std::string inputFloatingmage, int gcpGap,
                                                  float metricThreshold, int windowSize, int searchArea, float stdDevRefThreshold,
                                                  float stdDevFloatThreshold, int subPixelResolution, int distanceThreshold,
                                                  int maxNumIterations, float moveChangeThreshold, float pSmoothness, unsigned int metricTypeInt,
                                                  unsigned int outputType, std::string outputGCPFile) 
    {
                
        try
        {
            GDALAllRegister();
            GDALDataset *inRefDataset = nullptr;
            GDALDataset *inFloatDataset = nullptr;
            
            inRefDataset = (GDALDataset *) GDALOpenShared(inputReferenceImage.c_str(), GA_ReadOnly);
            if(inRefDataset == nullptr)
            {
                std::string message = std::string("Could not open image ") + inputReferenceImage;
                throw rsgis::RSGISException(message.c_str());
            }
            
            inFloatDataset = (GDALDataset *) GDALOpenShared(inputFloatingmage.c_str(), GA_ReadOnly);
            if(inFloatDataset == nullptr)
            {
                std::string message = std::string("Could not open image ") + inputFloatingmage;
                throw rsgis::RSGISException(message.c_str());
            }
            
            rsgis::reg::RSGISImageSimilarityMetric *similarityMetric = nullptr;
            if(metricTypeInt == 1) // euclidean
            {
                similarityMetric = new rsgis::reg::RSGISEuclideanSimilarityMetric();
            }
            else if(metricTypeInt == 2) // sqdiff
            {
                similarityMetric = new rsgis::reg::RSGISSquaredDifferenceSimilarityMetric();
            }
            else if(metricTypeInt == 3) // manhatten
            {
                similarityMetric = new rsgis::reg::RSGISManhattanSimilarityMetric();
            }
            else if(metricTypeInt == 4) // correlation
            {
                similarityMetric = new rsgis::reg::RSGISCorrelationSimilarityMetric();
            }
            else
            {
                throw rsgis::cmds::RSGISCmdException("Metric not recognised!");
            }
            
            rsgis::reg::RSGISImageRegistration *regImgs = new rsgis::reg::RSGISSingleConnectLayerImageRegistration(inRefDataset, inFloatDataset,
                                                                                                                   gcpGap, metricThreshold, windowSize,
                                                                                                                   searchArea, similarityMetric, stdDevRefThreshold,
                                                                                                                   stdDevFloatThreshold, subPixelResolution,
                                                                                                                   distanceThreshold, maxNumIterations,
                                                                                                                   moveChangeThreshold, pSmoothness);
            
            regImgs->runCompleteRegistration();
            
            if(outputType == 1) // envi_img2img
            {
                regImgs->exportTiePointsENVIImage2Image(outputGCPFile);
            }
            else if(outputType == 2) // envi_img2map
            {
                regImgs->exportTiePointsENVIImage2Map(outputGCPFile);
            }
            else if(outputType == 3) // rsgis_img2map
            {
                regImgs->exportTiePointsRSGISImage2Map(outputGCPFile);
            }
            else if(outputType == 4) // rsgis_mapoffs
            {
                regImgs->exportTiePointsRSGISMapOffs(outputGCPFile);
            }
            
            delete similarityMetric;
            delete regImgs;
            
            GDALClose(inRefDataset);
            GDALClose(inFloatDataset);
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

    
    void excecuteAddGCPsGDAL(std::string inputImage, std::string inputGCPs, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType) 
    {
        try
        {
            
            rsgis::reg::RSGISAddGCPsGDAL(inputImage, inputGCPs, outputImage, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            std::cout << "Added GCPs to: " << outputImage << std::endl;
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
    
    
    void executeApplyOffset2Image(std::string inputImage, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType, double xOff, double yOff) 
    {
        try
        {
            GDALAllRegister();
            
            std::cout << inputImage << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(dataset == nullptr)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            unsigned int numBands = dataset->GetRasterCount();
            
            rsgis::img::RSGISCopyImage copyImage = rsgis::img::RSGISCopyImage(numBands);
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(&copyImage, "", true);
            calcImage->calcImage(&dataset, 1, outputImage, false, nullptr, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            delete calcImage;
            GDALClose(dataset);
            
            GDALDataset *outDataset = (GDALDataset *) GDALOpen(outputImage.c_str(), GA_Update);
            if(outDataset == nullptr)
            {
                std::string message = std::string("Could not open image ") + outputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            double *trans = new double[6];
            outDataset->GetGeoTransform(trans);
            trans[0] = trans[0] + xOff;
            trans[3] = trans[3] + yOff;
            outDataset->SetGeoTransform(trans);
            GDALClose(outDataset);
            delete[] trans;
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
    
    
    
}}

