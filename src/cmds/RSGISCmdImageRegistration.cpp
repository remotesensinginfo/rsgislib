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


namespace rsgis{ namespace cmds {

    void excecuteBasicRegistration(std::string inputReferenceImage, std::string inputFloatingmage, int gcpGap,
                                                  float metricThreshold, int windowSize, int searchArea, float stdDevRefThreshold,
                                                  float stdDevFloatThreshold, int subPixelResolution, unsigned int metricTypeInt,
                                                  unsigned int outputType, std::string outputGCPFile)
    {
        
        try
        {
            GDALAllRegister();
            GDALDataset *inRefDataset = NULL;
            GDALDataset *inFloatDataset = NULL;
            
            inRefDataset = (GDALDataset *) GDALOpenShared(inputReferenceImage.c_str(), GA_ReadOnly);
            if(inRefDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputReferenceImage;
                throw rsgis::RSGISException(message.c_str());
            }
            
            inFloatDataset = (GDALDataset *) GDALOpenShared(inputFloatingmage.c_str(), GA_ReadOnly);
            if(inFloatDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputFloatingmage;
                throw rsgis::RSGISException(message.c_str());
            }
            
            rsgis::reg::RSGISImageSimilarityMetric *similarityMetric = NULL;
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
    
    void excecuteSingleLayerConnectedRegistration(std::string inputReferenceImage, std::string inputFloatingmage, int gcpGap,
                                                  float metricThreshold, int windowSize, int searchArea, float stdDevRefThreshold,
                                                  float stdDevFloatThreshold, int subPixelResolution, int distanceThreshold,
                                                  int maxNumIterations, float moveChangeThreshold, float pSmoothness, unsigned int metricTypeInt,
                                                  unsigned int outputType, std::string outputGCPFile)
    {
                
        try
        {
            GDALAllRegister();
            GDALDataset *inRefDataset = NULL;
            GDALDataset *inFloatDataset = NULL;
            
            inRefDataset = (GDALDataset *) GDALOpenShared(inputReferenceImage.c_str(), GA_ReadOnly);
            if(inRefDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputReferenceImage;
                throw rsgis::RSGISException(message.c_str());
            }
            
            inFloatDataset = (GDALDataset *) GDALOpenShared(inputFloatingmage.c_str(), GA_ReadOnly);
            if(inFloatDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputFloatingmage;
                throw rsgis::RSGISException(message.c_str());
            }
            
            rsgis::reg::RSGISImageSimilarityMetric *similarityMetric = NULL;
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
}}

