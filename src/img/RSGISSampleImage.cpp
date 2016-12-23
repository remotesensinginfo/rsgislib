/*
 *  RSGISSampleImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/04/2013.
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

#include "RSGISSampleImage.h"


namespace rsgis{namespace img{
    
    RSGISSampleImage::RSGISSampleImage()
    {
        
    }
    
    void RSGISSampleImage::subSampleImage(GDALDataset *inputImage, std::string outputFile, unsigned int sample, float noData, bool useNoData)throw(RSGISImageException)
    {
        try
        {
            rsgis::math::RSGISMathsUtils mathUtils;
            rsgis::utils::RSGISExportColumnData2HDF *export2HDF = new rsgis::utils::RSGISExportColumnData2HDF();
            std::string description = "Data subsampled with a sample of " + mathUtils.uinttostring(sample) + " from " + std::string(inputImage->GetFileList()[0]);
            std::cout << "Description: " << description << std::endl;
            export2HDF->createFile(outputFile, inputImage->GetRasterCount(), description, H5::PredType::IEEE_F32LE);
            
            float *dataRow = new float[inputImage->GetRasterCount()];
            
            RSGISSampleCalcImage *calcImageSample = new RSGISSampleCalcImage(sample, noData, useNoData, export2HDF, dataRow);
			RSGISCalcImage calcImg = RSGISCalcImage(calcImageSample, "", true);
			calcImg.calcImage(&inputImage, 1);
            delete calcImageSample;
            delete[] dataRow;
            
            export2HDF->close();
        }
        catch (RSGISImageCalcException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }
    
    void RSGISSampleImage::randomSampleImageMask(GDALDataset *inputImage, unsigned int imgBand, GDALDataset *outputImage, std::vector<int> maskVals, unsigned long numSamples)throw(RSGISImageException)
    {
        try
        {
            RSGISImageUtils imgUtils;
            unsigned int imgPxlXSize = inputImage->GetRasterXSize()-1;
            unsigned int imgPxlYSize = inputImage->GetRasterYSize()-1;
            unsigned int maxVal = imgPxlXSize;
            if(imgPxlYSize > maxVal)
            {
                maxVal = imgPxlYSize;
            }
            bool foundAllSamples = false;
            
            unsigned int *maskValCounts = new unsigned int[maskVals.size()];
            for(int i = 0; i < maskVals.size(); ++i)
            {
                maskValCounts[i] = 0;
            }
            
            boost::mt19937 rng (NULL);
            
            boost::uniform_int<> pxlRange( 0, maxVal );
            
            boost::variate_generator< boost::mt19937, boost::uniform_int<> > pxlGen(rng, pxlRange);
            
            unsigned int xPxl = 0;
            unsigned int yPxl = 0;
            
            double pxlVal = 0.0;
            
            while(!foundAllSamples)
            {
                xPxl = pxlGen();
                while(xPxl >= imgPxlXSize)
                {
                    xPxl = pxlGen();
                }
                
                yPxl = pxlGen();
                while(yPxl >= imgPxlYSize)
                {
                    yPxl = pxlGen();
                }
                                
                pxlVal = imgUtils.getPixelValue(inputImage, imgBand, xPxl, yPxl);
                
                for(int i = 0; i < maskVals.size(); ++i)
                {
                    if(maskVals.at(i) == pxlVal)
                    {
                        if(maskValCounts[i] <= numSamples)
                        {
                            imgUtils.setPixelValue(outputImage, imgBand, xPxl, yPxl, pxlVal);
                            maskValCounts[i] += 1;
                        }
                        break;
                    }
                }
                
                foundAllSamples = true;
                for(int i = 0; i < maskVals.size(); ++i)
                {
                    if(maskValCounts[i] <= numSamples)
                    {
                        foundAllSamples = false;
                        break;
                    }
                }
            }
        }
        catch (RSGISImageCalcException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }
    
    
    void RSGISSampleImage::randomSampleImageMaskSmallPxlCount(GDALDataset *inputImage, unsigned int imgBand, GDALDataset *outputImage, std::vector<int> maskVals, unsigned long numSamples)throw(RSGISImageException)
    {
        try
        {
            RSGISImageUtils imgUtils;
            
            unsigned int numMskVals = maskVals.size();
            std::vector<std::pair<long, long> > *maskPxlLocs = new std::vector<std::pair<long, long> >[numMskVals];
            
            RSGISGetPxlLocAsVec getPxlLoc = RSGISGetPxlLocAsVec(maskVals, maskPxlLocs, imgBand);
            RSGISCalcImage calcImg = RSGISCalcImage(&getPxlLoc, "", true);
            calcImg.calcImagePosPxl(&inputImage, 1, 0);
            
            unsigned int maxNPxls = 0;
            for(int i = 0; i < maskVals.size(); ++i)
            {
                if(i == 0)
                {
                    maxNPxls = maskPxlLocs[i].size();
                }
                else if(maskPxlLocs[i].size() > maxNPxls)
                {
                    maxNPxls = maskPxlLocs[i].size();
                }
            }
            
            std::cout << "Max Number of Pixels within a class is " << maxNPxls << std::endl;

            boost::mt19937 rng (NULL);
            boost::uniform_int<> pxlRange( 0, maxNPxls );
            boost::variate_generator< boost::mt19937, boost::uniform_int<> > pxlGen(rng, pxlRange);
            
            unsigned int xPxl = 0;
            unsigned int yPxl = 0;
            
            unsigned int pxlIdx = 0;
            bool foundSamples = false;
            unsigned int samplesCount = 0;
            for(unsigned int i = 0; i < maskVals.size(); ++i)
            {
                foundSamples = false;
                samplesCount = 0;
                
                for(unsigned long j = 0; j < numSamples; ++j)
                {
                    pxlIdx = pxlGen();
                    while(pxlIdx >= maskPxlLocs[i].size())
                    {
                        pxlIdx = pxlGen();
                    }
                    
                    xPxl = maskPxlLocs[i].at(pxlIdx).first;
                    yPxl = maskPxlLocs[i].at(pxlIdx).second;
                    
                    imgUtils.setPixelValue(outputImage, imgBand, xPxl, yPxl, maskVals.at(i));
                }
            }
            delete[] maskPxlLocs;
            
        }
        catch (RSGISImageCalcException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }
    
    RSGISSampleImage::~RSGISSampleImage()
    {
        
    }
    
    
    RSGISSampleCalcImage::RSGISSampleCalcImage(unsigned int sample, float noData, bool useNoData, rsgis::utils::RSGISExportColumnData2HDF *dataExport, float *dataRow):RSGISCalcImageValue(0)
    {
        this->sample = sample;
        this->noData = noData;
        this->useNoData = useNoData;
        this->currentPxl = 1;
        this->dataExport = dataExport;
        this->dataRow = dataRow;
    }
    
    void RSGISSampleCalcImage::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        try
        {
            bool noDataFound = false;
            if(useNoData)
            {
                for(unsigned int i = 0; i < numBands; ++i)
                {
                    if(bandValues[i] == this->noData)
                    {
                        noDataFound = true;
                        break;
                    }
                }
            }
            
            if(!noDataFound)
            {
                if(currentPxl == sample)
                {
                    for(unsigned int i = 0; i < numBands; ++i)
                    {
                        this->dataRow[i] = bandValues[i];
                    }
                    dataExport->addDataRow(dataRow, H5::PredType::NATIVE_FLOAT);
                    currentPxl = 1;
                }
                else
                {
                    ++currentPxl;
                }
            }
        
        }
        catch (RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (RSGISImageException &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        catch (RSGISException &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISImageCalcException(e.what());
        }
    }
    
    
    RSGISSampleCalcImage::~RSGISSampleCalcImage()
    {
        
    }
    
    
    
    
    RSGISGetPxlLocAsVec::RSGISGetPxlLocAsVec(std::vector<int> maskVals, std::vector<std::pair<long, long> > *maskPxlLocs, unsigned int imgBand):RSGISCalcImageValue(0)
    {
        this->maskVals = maskVals;
        this->maskPxlLocs = maskPxlLocs;
        this->imgBand = imgBand;
    }
    
    void RSGISGetPxlLocAsVec::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException)
    {
        for(size_t i = 0; i < maskVals.size(); ++i)
        {
            if(intBandValues[imgBand-1] == maskVals.at(i))
            {
                maskPxlLocs[i].push_back(std::pair<double, double>(extent.getMinX(), extent.getMinY()));
                break;
            }
        }
        
    }
    
    RSGISGetPxlLocAsVec::~RSGISGetPxlLocAsVec()
    {
        
    }
    
    
    
    
    
}}



