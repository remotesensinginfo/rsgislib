/*
 *  RSGISGenMeanSegImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/01/2012.
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

#include "RSGISGenMeanSegImage.h"

namespace rsgis{namespace segment{
    
    
    RSGISGenMeanSegImage::RSGISGenMeanSegImage()
    {
        
    }
    
    void RSGISGenMeanSegImage::generateMeanImage(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *meanImg) throw(rsgis::img::RSGISImageCalcException)
    {
        if((spectral->GetRasterXSize() != clumps->GetRasterXSize()) |
           (spectral->GetRasterXSize() != meanImg->GetRasterXSize()))
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if((spectral->GetRasterYSize() != clumps->GetRasterYSize()) |
           (spectral->GetRasterYSize() != meanImg->GetRasterYSize()))
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
        
        if(spectral->GetRasterCount() != meanImg->GetRasterCount())
        {
            throw rsgis::img::RSGISImageCalcException("The number of bands is not the same");
        }
        
        rsgis::img::RSGISImageUtils imgUtils;
        imgUtils.zerosFloatGDALDataset(meanImg);
        
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        bool **pxlMask = new bool*[height];
        for(unsigned int i = 0; i < height; ++i)
        {
            pxlMask[i] = new bool[width];
            for(unsigned int j = 0; j < width; ++j)
            {
                pxlMask[i][j] = false;
            }
        }
        
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        GDALRasterBand **meanBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
            meanBands[n] = meanImg->GetRasterBand(n+1);
        }
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        unsigned long clumpIdx = 0;
        unsigned int uiPxlVal = 0;
        std::vector<rsgis::img::PxlLoc> clumpPxls;
        std::queue<rsgis::img::PxlLoc> clumpSearchPxls;
        std::vector<float> *clumpSpecPxlVals = new std::vector<float>[numSpecBands];
        float *specPxlVals = new float[numSpecBands];
        
        int feedback = height/10;
        int feedbackCounter = 0;
        std::cout << "Started" << std::flush;
        for(unsigned int i = 0; i < height; ++i)
        {
            if((i % feedback) == 0)
            {
                std::cout << ".." << feedbackCounter << ".." << std::flush;
                feedbackCounter = feedbackCounter + 10;
            }
            
            for(unsigned int j = 0; j < width; ++j)
            {
                //std::cout << "Processing Pixel [" << j << "," << i << "]\n";
                // Get pixel value from clump image for (j,i)
                if(!pxlMask[i][j])
                {
                    clumpBand->RasterIO(GF_Read, j, i, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                    
                    // Make sure all lists are empty.
                    clumpPxls.clear();
                    if(!clumpSearchPxls.empty())
                    {
                        while(clumpSearchPxls.size() > 0)
                        {
                            clumpSearchPxls.pop();
                        }
                    }
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        clumpSpecPxlVals[n].clear();
                        spectralBands[n]->RasterIO(GF_Read, j, i, 1, 1, &specPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                        clumpSpecPxlVals[n].push_back(specPxlVals[n]);
                    }
                    clumpPxls.push_back(rsgis::img::PxlLoc(j, i));
                    clumpSearchPxls.push(rsgis::img::PxlLoc(j, i));
                    pxlMask[i][j] = true;
                    
                    // Add neigbouring pixels to clump.
                    // If no more pixels to add then stop.
                    while(clumpSearchPxls.size() > 0)
                    {
                        rsgis::img::PxlLoc pxl = clumpSearchPxls.front();
                        clumpSearchPxls.pop();
                        
                        //std::cout << "\tSearch Size = " << clumpSearchPxls.size() << std::endl;
                        //std::cout << "\t\tProcessing [" << pxl.xPos << "," << pxl.yPos << "]\n";
                        
                        // Above
                        if((((long)pxl.yPos)-1 >= 0) && (!pxlMask[pxl.yPos-1][pxl.xPos]))
                        {
                            clumpBand->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if(uiPxlVal == clumpIdx)
                            {
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &specPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                }
                                                            
                                clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                                clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    clumpSpecPxlVals[n].push_back(specPxlVals[n]);
                                }
                                pxlMask[pxl.yPos-1][pxl.xPos] = true;
                            }
                        }
                        // Below
                        if((((long)pxl.yPos)+1 < height) && (!pxlMask[pxl.yPos+1][pxl.xPos]))
                        {
                            clumpBand->RasterIO(GF_Read, pxl.xPos, pxl.yPos+1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if(uiPxlVal == clumpIdx)
                            {
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos+1, 1, 1, &specPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                }
                                
                                clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                                clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    clumpSpecPxlVals[n].push_back(specPxlVals[n]);
                                }
                                pxlMask[pxl.yPos+1][pxl.xPos] = true;
                            }
                        }
                        // Left
                        if((((long)pxl.xPos-1) >= 0) && (!pxlMask[pxl.yPos][pxl.xPos-1]))
                        {
                            clumpBand->RasterIO(GF_Read, pxl.xPos-1, pxl.yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if(uiPxlVal == clumpIdx)
                            {
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    spectralBands[n]->RasterIO(GF_Read, pxl.xPos-1, pxl.yPos, 1, 1, &specPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                }
                                
                                clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                                clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    clumpSpecPxlVals[n].push_back(specPxlVals[n]);
                                }
                                pxlMask[pxl.yPos][pxl.xPos-1] = true;
                            }
                        }
                        // Right
                        if((((long)pxl.xPos+1) < width) && (!pxlMask[pxl.yPos][pxl.xPos+1]))
                        {
                            clumpBand->RasterIO(GF_Read, pxl.xPos+1, pxl.yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if(uiPxlVal == clumpIdx)
                            {
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    spectralBands[n]->RasterIO(GF_Read, pxl.xPos+1, pxl.yPos, 1, 1, &specPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                }
                                
                                clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                                clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    clumpSpecPxlVals[n].push_back(specPxlVals[n]);
                                }
                                pxlMask[pxl.yPos][pxl.xPos+1] = true;
                            }
                        }
                    }
                    
                    // Calc Mean Values;
                    //std::cout << "Clump " << clumpIdx << " has clumpPxls.size() = " << clumpPxls.size() << std::endl;
                    for(size_t n = 0; n < clumpPxls.size(); ++n)
                    {
                        for(unsigned int b = 0; b < numSpecBands; ++b)
                        {
                            if(n == 0)
                            {
                                specPxlVals[b] = clumpSpecPxlVals[b][n];
                            }
                            else
                            {
                                specPxlVals[b] += clumpSpecPxlVals[b][n];
                            }
                        }
                    }
                    
                    for(unsigned int b = 0; b < numSpecBands; ++b)
                    {
                        specPxlVals[b] = specPxlVals[b]/clumpPxls.size();
                    }
                    
                    for(size_t n = 0; n < clumpPxls.size(); ++n)
                    {
                        for(unsigned int b = 0; b < numSpecBands; ++b)
                        {
                            meanBands[b]->RasterIO(GF_Write, clumpPxls[n].xPos, clumpPxls[n].yPos, 1, 1, &specPxlVals[b], 1, 1, GDT_Float32, 0, 0);
                        }
                    }
                }
            }
        }
        std::cout << " Complete.\n";
        
        delete[] clumpSpecPxlVals;
        delete[] specPxlVals;
        delete[] meanBands;
        delete[] spectralBands;
        
        for(unsigned int i = 0; i < height; ++i)
        {
            delete[] pxlMask[i];
        }
        delete[] pxlMask;
    }
    
    void RSGISGenMeanSegImage::generateMeanImageUsingClumpTable(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *meanImg) throw(rsgis::img::RSGISImageCalcException)
    {
        if((spectral->GetRasterXSize() != clumps->GetRasterXSize()) |
           (spectral->GetRasterXSize() != meanImg->GetRasterXSize()))
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if((spectral->GetRasterYSize() != clumps->GetRasterYSize()) |
           (spectral->GetRasterYSize() != meanImg->GetRasterYSize()))
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
        
        if(spectral->GetRasterCount() != meanImg->GetRasterCount())
        {
            throw rsgis::img::RSGISImageCalcException("The number of bands is not the same");
        }
        
        rsgis::img::RSGISImageUtils imgUtils;
        imgUtils.zerosFloatGDALDataset(meanImg);
        
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        unsigned int *clumpIdxs = new unsigned int[width];
        float **spectralVals = new float*[numSpecBands];
        
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        GDALRasterBand **meanBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
            meanBands[n] = meanImg->GetRasterBand(n+1);
            spectralVals[n] = new float[width]; 
        }
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);

        unsigned long maxClumpIdx = 0;
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int j = 0; j < width; ++j)
            {
                if((i == 0) & (j == 0))
                {
                    maxClumpIdx = clumpIdxs[j];
                }
                else if(clumpIdxs[j] > maxClumpIdx)
                {
                    maxClumpIdx = clumpIdxs[j];
                }
            }
        }
        
        std::vector<rsgis::img::ImgClumpMean*> *clumpTab = new std::vector<rsgis::img::ImgClumpMean*>();
        clumpTab->reserve(maxClumpIdx);
        rsgis::img::ImgClumpMean *cClump;
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            cClump = new rsgis::img::ImgClumpMean(i+1);
            cClump->sumVals = new float[numSpecBands];
            cClump->meanVals = new float[numSpecBands];
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                cClump->sumVals[n] = 0;
                cClump->meanVals[n] = 0;
            }
            clumpTab->push_back(cClump);
        }
        
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                spectralBands[n]->RasterIO(GF_Read, 0, i, width, 1, spectralVals[n], width, 1, GDT_Float32, 0, 0);
            }
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    cClump = clumpTab->at(clumpIdxs[j] - 1);
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        cClump->sumVals[n] += spectralVals[n][j];
                    }
                    ++cClump->numPxls;
                }
            }
        }
        
        for(std::vector<rsgis::img::ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                (*iterClumps)->meanVals[n] = (*iterClumps)->sumVals[n] / (*iterClumps)->numPxls;
            }
        }
        
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    cClump = clumpTab->at(clumpIdxs[j] - 1);
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        spectralVals[n][j] = cClump->meanVals[n];
                    }
                }
                else
                {
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        spectralVals[n][j] = 0;
                    }
                }
            }
            
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                meanBands[n]->RasterIO(GF_Write, 0, i, width, 1, spectralVals[n], width, 1, GDT_Float32, 0, 0);
            }
        }
        
        
        delete[] clumpIdxs;
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            delete[] spectralVals[n];
        }
        delete[] spectralVals;
        
        delete[] spectralBands;
        delete[] meanBands;
        
        for(std::vector<rsgis::img::ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            delete[] (*iterClumps)->meanVals;
            delete[] (*iterClumps)->sumVals;
            delete *iterClumps;
        }
        delete clumpTab;
    }
    
    void RSGISGenMeanSegImage::generateMeanImageUsingCalcImage(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *meanImg) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            if((spectral->GetRasterXSize() != clumps->GetRasterXSize()) |
               (spectral->GetRasterXSize() != meanImg->GetRasterXSize()))
            {
                throw rsgis::img::RSGISImageCalcException("Widths are not the same");
            }
            if((spectral->GetRasterYSize() != clumps->GetRasterYSize()) |
               (spectral->GetRasterYSize() != meanImg->GetRasterYSize()))
            {
                throw rsgis::img::RSGISImageCalcException("Heights are not the same");
            }
            if(spectral->GetRasterCount() != meanImg->GetRasterCount())
            {
                throw rsgis::img::RSGISImageCalcException("The number of bands is not the same");
            }
            
            GDALRasterBand *clumpsBand = clumps->GetRasterBand(1);
            
            std::cout << "Finding maximum image value\n";
            double maxVal = 0;
            clumpsBand->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
            unsigned int maxClumpIdx = boost::lexical_cast<unsigned long>(maxVal);
            
            unsigned int numSpecBands = spectral->GetRasterCount();
                        
            std::vector<rsgis::img::ImgClumpMean*> *clumpTable = new std::vector<rsgis::img::ImgClumpMean*>();
            clumpTable->reserve(maxClumpIdx);
            
            std::cout << "Build clump table\n";
            rsgis::img::ImgClumpMean *cClump = NULL;
            for(unsigned int i = 0; i < maxClumpIdx; ++i)
            {
                cClump = new rsgis::img::ImgClumpMean(i+1);
                cClump->sumVals = new float[numSpecBands];
                cClump->meanVals = new float[numSpecBands];
                cClump->numPxls = 0;
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    cClump->sumVals[n] = 0;
                    cClump->meanVals[n] = 0;
                }
                clumpTable->push_back(cClump);
            }
            
            std::cout << "Creating Look up table.\n";
            RSGISPopulateMeans *createLookUp = new RSGISPopulateMeans(clumpTable, numSpecBands, maxClumpIdx);
            rsgis::img::RSGISCalcImage calcImgCreateLoopUp = rsgis::img::RSGISCalcImage(createLookUp);
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumps;
            datasets[1] = spectral;
            calcImgCreateLoopUp.calcImage(datasets, 2);
            delete createLookUp;
            
            
            
            //for(unsigned int i = 0; i < maxClumpIdx; ++i)
            for(std::vector<rsgis::img::ImgClumpMean*>::iterator iterClump = clumpTable->begin(); iterClump != clumpTable->end(); ++iterClump)
            {
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    (*iterClump)->meanVals[n] = (*iterClump)->sumVals[n]/(*iterClump)->numPxls;
                }
            }
            
            std::cout << "Applying Look up table.\n";
            RSGISApplyMeans2Output *applyMeansLookUp = new RSGISApplyMeans2Output(clumpTable, numSpecBands, maxClumpIdx);
            rsgis::img::RSGISCalcImage calcImgApplyLookUp = rsgis::img::RSGISCalcImage(applyMeansLookUp);
            calcImgApplyLookUp.calcImage(datasets, 2, meanImg);
            delete applyMeansLookUp;
            delete[] datasets;
            
            for(std::vector<rsgis::img::ImgClumpMean*>::iterator iterClump = clumpTable->begin(); iterClump != clumpTable->end(); ++iterClump)
            {
                delete[] (*iterClump)->meanVals;
                delete[] (*iterClump)->sumVals;
                delete (*iterClump);
            }
            delete clumpTable;
            
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    
    RSGISGenMeanSegImage::~RSGISGenMeanSegImage()
    {
        
    }
    
    
    RSGISPopulateMeans::RSGISPopulateMeans(std::vector<rsgis::img::ImgClumpMean*> *clumpTable, unsigned int numSpecBands, unsigned int numClumps):rsgis::img::RSGISCalcImageValue(0)
    {
        this->clumpTable = clumpTable;
        this->numSpecBands = numSpecBands;
        this->numClumps = numClumps;
    }
    
    void RSGISPopulateMeans::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            if((bandValues[0] > 0) & (bandValues[0] < numClumps))
            {
                size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
                rsgis::img::ImgClumpMean *cClump = clumpTable->at(fid - 1);
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    cClump->sumVals[n] += bandValues[n+1];
                }
                ++cClump->numPxls;
            }
        }
        catch(boost::numeric::negative_overflow& e)
        {
            //std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(boost::numeric::positive_overflow& e)
        {
            //std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(boost::numeric::bad_numeric_cast& e)
        {
            //std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    RSGISPopulateMeans::~RSGISPopulateMeans()
    {
        
    }
    
    
    RSGISApplyMeans2Output::RSGISApplyMeans2Output(std::vector<rsgis::img::ImgClumpMean*> *clumpTable, unsigned int numSpecBands, unsigned int numClumps):rsgis::img::RSGISCalcImageValue(numSpecBands)
    {
        this->clumpTable = clumpTable;
        this->numSpecBands = numSpecBands;
        this->numClumps = numClumps;
    }
    
    void RSGISApplyMeans2Output::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            if((bandValues[0] > 0) & (bandValues[0] < numClumps))
            {
                size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
                
                rsgis::img::ImgClumpMean *cClump = clumpTable->at(fid - 1);
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    output[n] = cClump->meanVals[n];
                }
            }
            else
            {
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    output[n] = 0;
                }
            }
        }
        catch(boost::numeric::negative_overflow& e)
        {
            //std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(boost::numeric::positive_overflow& e)
        {
            //std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(boost::numeric::bad_numeric_cast& e)
        {
            //std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    RSGISApplyMeans2Output::~RSGISApplyMeans2Output()
    {
        
    }
    
    
}}

