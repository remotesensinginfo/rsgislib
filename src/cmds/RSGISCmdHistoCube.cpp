/*
 *  RSGISCmdHistoCube.cpp
 *
 *
 *  Created by Pete Bunting on 17/02/2017.
 *  Copyright 2017 RSGISLib.
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

#include "RSGISCmdHistoCube.h"
#include "RSGISCmdParent.h"

#include "common/RSGISHistoCubeException.h"
#include "histocube/RSGISHistoCubeFileIO.h"
#include "histocube/RSGISPopulateHistoCube.h"

#include "img/RSGISCalcImage.h"
#include "img/RSGISImageStatistics.h"

namespace rsgis{ namespace cmds {
    
    void executeCreateEmptyHistoCube(std::string histCubeFile, unsigned long numFeats)throw(RSGISCmdException)
    {
        try
        {
            rsgis::histocube::RSGISHistoCubeFile histoCubeFileObj = rsgis::histocube::RSGISHistoCubeFile();
            histoCubeFileObj.createNewFile(histCubeFile, numFeats);
            histoCubeFileObj.closeFile();
        }
        catch(rsgis::RSGISHistoCubeException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeCreateHistoCubeLayer(std::string histCubeFile, std::string layerName, int lowBin, int upBin, float scale, float offset, bool hasDateTime, std::string dataTime)throw(RSGISCmdException)
    {
        try
        {
            if(lowBin >= upBin)
            {
                rsgis::RSGISHistoCubeException("The upper bin must be greater than the lower bin.");
            }
            rsgis::histocube::RSGISHistoCubeFile histoCubeFileObj = rsgis::histocube::RSGISHistoCubeFile();
            histoCubeFileObj.openFile(histCubeFile, true);
            
            std::vector<int> bins = std::vector<int>();
            for(int i = lowBin; i <= upBin; ++i)
            {
                bins.push_back(i);
            }
            
            boost::posix_time::ptime *layerDateTime = NULL;
            if(hasDateTime)
            {
                layerDateTime = new boost::posix_time::ptime(boost::posix_time::time_from_string(dataTime));
            }
            
            histoCubeFileObj.createDataset(layerName, bins, scale, offset, hasDateTime, layerDateTime);
            histoCubeFileObj.closeFile();
        }
        catch(rsgis::RSGISHistoCubeException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executePopulateSingleHistoCubeLayer(std::string histCubeFile, std::string layerName, std::string clumpsImg, std::string valsImg, unsigned int imgBand, bool inMem)throw(RSGISCmdException)
    {
        GDALAllRegister();
        
        try
        {
            if(imgBand == 0)
            {
                throw rsgis::RSGISImageException("The band specified is not within the values image.");
            }
            
            rsgis::histocube::RSGISHistoCubeFile histoCubeFileObj = rsgis::histocube::RSGISHistoCubeFile();
            histoCubeFileObj.openFile(histCubeFile, true);
            
            std::vector<rsgis::histocube::RSGISHistCubeLayerMeta*> *cubeLayers = histoCubeFileObj.getCubeLayersList();
            rsgis::histocube::RSGISHistCubeLayerMeta *cubeLayer = NULL;
            bool found = false;
            for(std::vector<rsgis::histocube::RSGISHistCubeLayerMeta*>::iterator iterLayers = cubeLayers->begin(); iterLayers != cubeLayers->end(); ++iterLayers)
            {
                if((*iterLayers)->name == layerName)
                {
                    cubeLayer = (*iterLayers);
                    found = true;
                    break;
                }
            }
            
            if(!found)
            {
                throw rsgis::RSGISHistoCubeException("Column was not found within the histogram cube.");
            }
            
            GDALDataset **datasets = new GDALDataset*[2];
            
            datasets[0] = (GDALDataset *) GDALOpen(clumpsImg.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            if(datasets[0]->GetRasterCount() != 1)
            {
                GDALClose(datasets[0]);
                delete[] datasets;
                throw rsgis::RSGISImageException("The clumps image must only have 1 image band.");
            }
            
            datasets[1] = (GDALDataset *) GDALOpen(valsImg.c_str(), GA_ReadOnly);
            if(datasets[1] == NULL)
            {
                std::string message = std::string("Could not open image ") + valsImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            if(imgBand > datasets[1]->GetRasterCount())
            {
                GDALClose(datasets[1]);
                delete[] datasets;
                throw rsgis::RSGISImageException("The band specified is not within the values image.");
            }
            
            unsigned int bandIdx = imgBand-1;
            unsigned int maxRow = histoCubeFileObj.getNumFeatures()-1;
            
            if(inMem)
            {
                unsigned int nBins = cubeLayer->bins.size();
                unsigned long dataArrLen = (maxRow*nBins)+nBins;
                unsigned int *dataArr = new unsigned int[dataArrLen];
                histoCubeFileObj.getHistoRows(layerName, 0, maxRow, dataArr, dataArrLen);
                
                rsgis::histocube::RSGISPopHistoCubeLayerFromImgBandInMem popCubeLyrMem = rsgis::histocube::RSGISPopHistoCubeLayerFromImgBandInMem(dataArr, dataArrLen, bandIdx, maxRow, cubeLayer->scale, cubeLayer->offset, cubeLayer->bins);
                rsgis::img::RSGISCalcImage calcImgPopCubeMem = rsgis::img::RSGISCalcImage(&popCubeLyrMem);
                calcImgPopCubeMem.calcImage(datasets, 1, 1);
                
                histoCubeFileObj.setHistoRows(layerName, 0, maxRow, dataArr, dataArrLen);
                delete[] dataArr;
            }
            else
            {
                rsgis::histocube::RSGISPopHistoCubeLayerFromImgBand popCubeLyr = rsgis::histocube::RSGISPopHistoCubeLayerFromImgBand(&histoCubeFileObj, layerName, bandIdx, maxRow, cubeLayer->scale, cubeLayer->offset, cubeLayer->bins);
                rsgis::img::RSGISCalcImage calcImgPopCube = rsgis::img::RSGISCalcImage(&popCubeLyr);
                calcImgPopCube.calcImage(datasets, 1, 1);
            }
            histoCubeFileObj.closeFile();
            GDALClose(datasets[0]);
            GDALClose(datasets[1]);
            delete[] datasets;
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISHistoCubeException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    DllExport void executeExportHistBins2Img(std::string histCubeFile, std::string layerName, std::string clumpsImg, std::string outputImg, std::string gdalFormat, std::vector<unsigned int> exportBins) throw(RSGISCmdException)
    {
        try
        {
            rsgis::histocube::RSGISHistoCubeFile histoCubeFileObj = rsgis::histocube::RSGISHistoCubeFile();
            histoCubeFileObj.openFile(histCubeFile, true);
            
            std::vector<rsgis::histocube::RSGISHistCubeLayerMeta*> *cubeLayers = histoCubeFileObj.getCubeLayersList();
            rsgis::histocube::RSGISHistCubeLayerMeta *cubeLayer = NULL;
            bool found = false;
            for(std::vector<rsgis::histocube::RSGISHistCubeLayerMeta*>::iterator iterLayers = cubeLayers->begin(); iterLayers != cubeLayers->end(); ++iterLayers)
            {
                if((*iterLayers)->name == layerName)
                {
                    cubeLayer = (*iterLayers);
                    found = true;
                    break;
                }
            }
            
            if(!found)
            {
                throw rsgis::RSGISHistoCubeException("Column was not found within the histogram cube.");
            }
            
            GDALDataset *dataset = (GDALDataset *) GDALOpen(clumpsImg.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            if(dataset->GetRasterCount() != 1)
            {
                GDALClose(dataset);
                throw rsgis::RSGISImageException("The clumps image must only have 1 image band.");
            }
            
            unsigned int maxRow = histoCubeFileObj.getNumFeatures()-1;
            unsigned int nBins = cubeLayer->bins.size();
            unsigned long dataArrLen = (maxRow*nBins)+nBins;
            unsigned int *dataArr = new unsigned int[dataArrLen];
            histoCubeFileObj.getHistoRows(layerName, 0, maxRow, dataArr, dataArrLen);
            
            
            
            
            delete[] dataArr;
            
            
            GDALClose(dataset);
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISHistoCubeException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
}}




