/*
 *  RSGISDefineSpectralDivision.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/03/2012.
 *  Copyright 2012 RSGISLib.
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

#include "RSGISDefineSpectralDivision.h"


namespace rsgis{namespace segment{
    
   
    
    RSGISDefineSpectralDivision::RSGISDefineSpectralDivision()
    {
        
    }
        
    void RSGISDefineSpectralDivision::findSpectralDivision(GDALDataset *inData, std::string outputImage, unsigned int subDivision, float noDataVal, bool noDataValProvided, bool projFromImage, std::string proj, std::string format)throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            GDALDataset **datasets = new GDALDataset*[1];
            datasets[0] = inData;
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *outImageDataset = imgUtils.createCopy(inData, 1, outputImage, format, GDT_UInt32, projFromImage, proj);
            imgUtils.zerosUIntGDALDataset(outImageDataset);
            
            int numBands = inData->GetRasterCount();
            rsgis::img::ImageStats **stats = new rsgis::img::ImageStats*[numBands];
            for(int n = 0; n < numBands; ++n)
            {
                stats[n] = new rsgis::img::ImageStats();
                stats[n]->min = 0;
                stats[n]->max = 0;
                stats[n]->mean = 0;
                stats[n]->sum = 0;
                stats[n]->stddev = 0;
            }
            
            std::cout << "Calc Image Stats\n";
            rsgis::img::RSGISImageStatistics imgStats;
            imgStats.calcImageStatistics(datasets, 1, stats, numBands, false);
            
            unsigned int numCats = subDivision;
            for(int n = 0; n < numBands-1; ++n)
            {
                numCats *= subDivision;
            }
            
            std::cout << "Generating " << numCats << " categories\n";
            
            
            std::vector<std::pair<float, float> > **catThresholds = new std::vector<std::pair<float, float> >*[numBands];
            std::vector<std::pair<float, float> > **bandThresholds = new std::vector<std::pair<float, float> >*[numBands];
            
            float bandStep = 0;
            float bandMin = 0;
            float bandMax = 0;
            for(int n = 0; n < numBands; ++n)
            {
                catThresholds[n] = new std::vector<std::pair<float, float> >();
                catThresholds[n]->reserve(numCats);
                bandThresholds[n] = new std::vector<std::pair<float, float> >();
                bandThresholds[n]->reserve(subDivision);
                bandStep = (stats[n]->max - stats[n]->min)/subDivision;
                bandMin = stats[n]->min;
                bandMax = bandMin + bandStep;
                for(unsigned j = 0; j < subDivision; ++j)
                {
                    bandThresholds[n]->push_back(std::pair<float,float>(bandMin, bandMax));
                    bandMin += bandStep;
                    bandMax += bandStep;
                }
            }
            
            std::pair<float, float> *pThreshold = NULL;
            for(unsigned j = 0; j < subDivision; ++j)
            {
                pThreshold = new std::pair<float, float>[numBands];
                for(int n = 0; n < numBands; ++n)
                {
                    pThreshold[n] = std::pair<float, float>(0,0);
                }
                pThreshold[0] = std::pair<float,float>(bandThresholds[0]->at(j).first, bandThresholds[0]->at(j).second);
                
                this->generateSpectralDivThresholds(pThreshold, 1, numBands, subDivision, bandThresholds, catThresholds);
            }
            
            std::cout << "Number of categories: " << catThresholds[0]->size() << std::endl;
            
            if(catThresholds[0]->size() != numCats)
            {
                throw rsgis::img::RSGISImageCalcException("The number of categories generated is not what was expected.");
            }
            
            std::cout << "Applying to output image\n";
            this->assignToCategory(inData, outImageDataset, catThresholds, numBands, numCats, noDataVal, noDataValProvided);
            
            std::cout << "Completed\n";
            GDALClose(outImageDataset);
            for(int n = 0; n < numBands; ++n)
            {
                delete catThresholds[n];
                delete bandThresholds[n];
                delete stats[n];
            }
            delete[] stats;
            delete[] catThresholds;
            delete[] bandThresholds;
            delete[] datasets;
        }
        catch(RSGISException &e)
        {
            throw e;
        }
    }
    
    void RSGISDefineSpectralDivision::generateSpectralDivThresholds(std::pair<float, float> *pThreshold, unsigned int bandIdx, unsigned int numBands, unsigned int subDivision, std::vector<std::pair<float, float> > **bandThresholds, std::vector<std::pair<float, float> > **catThresholds)
    {
        if(bandIdx == numBands)
        {
            for(unsigned int n = 0; n < numBands; ++n)
            {
                catThresholds[n]->push_back(std::pair<float, float>(pThreshold[n].first, pThreshold[n].second));
            }
        }
        else
        {
            std::pair<float, float> *cThreshold = NULL;
            for(unsigned j = 0; j < subDivision; ++j)
            {
                cThreshold = new std::pair<float, float>[numBands];
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    cThreshold[n] = std::pair<float, float>(0,0);
                }
                for(unsigned int n = 0; n < bandIdx; ++n)
                {
                    cThreshold[n] = std::pair<float, float>(pThreshold[n].first, pThreshold[n].second);
                }
                
                cThreshold[bandIdx] = std::pair<float,float>(bandThresholds[bandIdx]->at(j).first, bandThresholds[bandIdx]->at(j).second);
                
                this->generateSpectralDivThresholds(cThreshold, bandIdx+1, numBands, subDivision, bandThresholds, catThresholds);
            }
        }
        
        delete[] pThreshold;
    }
    
    void RSGISDefineSpectralDivision::assignToCategory(GDALDataset *reflDataset, GDALDataset *catsDataset, std::vector<std::pair<float, float> > **catThresholds, unsigned int numBands, unsigned int numCats, float noDataVal, bool noDataValProvided)throw(rsgis::img::RSGISImageCalcException)
    {
        try 
        {
            if(numBands != reflDataset->GetRasterCount())
            {
                throw rsgis::img::RSGISImageCalcException("The number of image bands does not match.");
            }
            
            if(reflDataset->GetRasterXSize() != catsDataset->GetRasterXSize())
            {
                throw rsgis::img::RSGISImageCalcException("Widths do not match.");
            }
            
            if(reflDataset->GetRasterYSize() != catsDataset->GetRasterYSize())
            {
                throw rsgis::img::RSGISImageCalcException("Heights do not match.");
            }
            
            unsigned int width = reflDataset->GetRasterXSize();
            unsigned int height = reflDataset->GetRasterYSize();
            
            unsigned int *catData = new unsigned int[width];
            float **reflData = new float*[numBands];
            
            GDALRasterBand *catBand = catsDataset->GetRasterBand(1);
            GDALRasterBand **reflBands = new GDALRasterBand*[numBands];
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                reflBands[n] = reflDataset->GetRasterBand(n+1);
                reflData[n] = new float[width];
            }
            
            bool foundCat = true;
            bool noDataFound = false;
            for(unsigned int i = 0; i < height; ++i)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    reflBands[n]->RasterIO(GF_Read, 0, i, width, 1, reflData[n], width, 1, GDT_Float32, 0, 0);
                }
                
                for(unsigned int j = 0; j < width; ++j)
                {
                    noDataFound = false;
                    if(noDataValProvided)
                    {
                        noDataFound = true;
                        for(unsigned int n = 0; n < numBands; ++n)
                        {
                            if(reflData[n][j] != noDataVal)
                            {
                                noDataFound = false;
                                break;
                            }
                        }
                    }
                    
                    if(noDataFound)
                    {
                        catData[j] = 0;
                    }
                    else
                    {
                        for(unsigned int c = 0; c < numCats; ++c)
                        {
                            foundCat = true;
                            for(unsigned int n = 0; n < numBands; ++n)
                            {
                                if((reflData[n][j] < catThresholds[n]->at(c).first) | (reflData[n][j] > catThresholds[n]->at(c).second))
                                {
                                    foundCat = false;
                                    break;
                                }
                            }
                            
                            if(foundCat)
                            {
                                catData[j] = c+1;
                                break;
                            }
                        }
                        
                        if(!foundCat)
                        {
                            catData[j] = numCats;
                        }
                    }
                }
                
                catBand->RasterIO(GF_Write, 0, i, width, 1, catData, width, 1, GDT_UInt32, 0, 0);
            }
            
            
            delete[] catData;
            for(unsigned int n = 0; n < numBands; ++n)
            {
                delete[] reflData[n];
            }
            delete[] reflData;
            delete[] reflBands;
            
        } 
        catch (RSGISException &e) 
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        
    }
        
    RSGISDefineSpectralDivision::~RSGISDefineSpectralDivision()
    {
        
    }

    
}}

