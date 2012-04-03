/*
 *  RSGISRegionGrowingFromClumps.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 20/01/2012.
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

#include "RSGISRegionGrowingFromClumps.h"

namespace rsgis{namespace segment{
    

    RSGISFindClumpIDs::RSGISFindClumpIDs()
    {
        
    }
    
    void RSGISFindClumpIDs::exportClumpIDsAsTxtFile(GDALDataset *clumps, string outputText, vector<ImgSeeds> *seedPxls) throw(RSGISImageCalcException)
    {
        unsigned int width = clumps->GetRasterXSize();
        unsigned int height = clumps->GetRasterYSize();
        
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        unsigned long clumpIdx = 0;
        
        ofstream outTextFile;
        outTextFile.open (outputText.c_str());
        
        for(vector<ImgSeeds>::iterator iterSeeds = seedPxls->begin(); iterSeeds != seedPxls->end(); ++iterSeeds)
        {
            if(((*iterSeeds).xPxl < width) & ((*iterSeeds).yPxl < height))
            {
                clumpBand->RasterIO(GF_Read, (*iterSeeds).xPxl, (*iterSeeds).yPxl, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                outTextFile << clumpIdx << "," << (*iterSeeds).seedID << endl;
            }
            else
            {
                outTextFile.flush();
                outTextFile.close();
                throw RSGISImageCalcException("Pixel is not within the clump Image.");
            }
        }
        
        outTextFile.flush();
        outTextFile.close();
    }
    
    vector<ClumpSeed>* RSGISFindClumpIDs::readClumpSeedIDs(string inputTextFile)throw(RSGISTextException)
    {
        vector<ClumpSeed> *clumpSeeds = new vector<ClumpSeed>();
        
        unsigned long clump = 0;
        unsigned long id = 0;
        string line = "";
        vector<string> *tokens = new vector<string>();
        
        RSGISMathsUtils mathUtils;
        RSGISTextUtils textUtils;
        RSGISTextFileLineReader reader;
        reader.openFile(inputTextFile);
        while(!reader.endOfFile())
        {
            line = reader.readLine();
            if(!textUtils.blankline(line))
            {
                tokens->clear();
                textUtils.tokenizeString(line, ',', tokens, true);
                if(tokens->size() != 2)
                {
                    throw RSGISTextException("Could not parse file, format incorrect.");
                }
                clump = mathUtils.strtounsignedlong(tokens->at(0));
                id = mathUtils.strtounsignedlong(tokens->at(1));
                clumpSeeds->push_back(ClumpSeed(clump, id));
            }
        }
        delete tokens;
        
        return clumpSeeds;
    }
    
    RSGISFindClumpIDs::~RSGISFindClumpIDs()
    {
        
    }
    
    
    RSGISRegionGrowingSegmentation::RSGISRegionGrowingSegmentation()
    {
        
    }
    
    void RSGISRegionGrowingSegmentation::performRegionGrowUsingClumps(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *output, vector<ClumpSeed> *seeds, float initThreshold, float thresholdIncrements, float maxThreshold, unsigned int maxIterations )throw(RSGISImageCalcException)
    {
        if((spectral->GetRasterXSize() != clumps->GetRasterXSize()) |
           (spectral->GetRasterXSize() != output->GetRasterXSize()))
        {
            throw RSGISImageCalcException("Widths are not the same");
        }
        if((spectral->GetRasterYSize() != clumps->GetRasterYSize()) |
           (spectral->GetRasterYSize() != output->GetRasterYSize()))
        {
            throw RSGISImageCalcException("Heights are not the same");
        }
        
        RSGISImageUtils imgUtils;
        imgUtils.zerosUIntGDALDataset(output);
        
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        unsigned int *clumpIdxsAbove = new unsigned int[width];
        unsigned int *clumpIdxs = new unsigned int[width];
        unsigned int *clumpIdxsBelow = new unsigned int[width];
        unsigned int *outVals = new unsigned int[width];
        float **spectralVals = new float*[numSpecBands];
        
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
            spectralVals[n] = new float[width]; 
        }
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        GDALRasterBand *outBand = output->GetRasterBand(1);
        
        cout << "Building Clump Table\n";
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
        
        vector<ImgClumpRG*> *clumpTab = new vector<ImgClumpRG*>();
        clumpTab->reserve(maxClumpIdx);
        ImgClumpRG *cClump;
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            cClump = new ImgClumpRG(i+1);
            cClump->sumVals = new float[numSpecBands];
            cClump->meanVals = new float[numSpecBands];
            cClump->pxls = new vector<PxlLoc>();
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                cClump->sumVals[n] = 0;
                cClump->meanVals[n] = 0;
            }
            cClump->seedVal = 0;
            clumpTab->push_back(cClump);
        }
        
        for(unsigned int i = 0; i < height; ++i)
        {
            if(((long)i)-1 >= 0)
            {
                clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, clumpIdxsAbove, width, 1, GDT_UInt32, 0, 0);
            }
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            if(((long)i)+1 < height)
            {
                clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, clumpIdxsBelow, width, 1, GDT_UInt32, 0, 0);
            }
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
                    cClump->pxls->push_back(PxlLoc(j,i));
                    
                    // Above
                    if(((long)i)-1 >= 0)
                    {
                        if(clumpIdxsAbove[j] != clumpIdxs[j])
                        {
                            cClump->neighbours.push_back(clumpIdxsAbove[j]);
                        }
                    }
                    // Below
                    if(((long)i)+1 < height)
                    {
                        if(clumpIdxsBelow[j] != clumpIdxs[j])
                        {
                            cClump->neighbours.push_back(clumpIdxsBelow[j]);
                        }
                    }
                    // Left
                    if(((long)j)-1 >= 0)
                    {
                        if(clumpIdxs[j-1] != clumpIdxs[j])
                        {
                            cClump->neighbours.push_back(clumpIdxs[j-1]);
                        }
                    }
                    // Right
                    if(((long)j)+1 < width)
                    {
                        if(clumpIdxs[j+1] != clumpIdxs[j])
                        {
                            cClump->neighbours.push_back(clumpIdxs[j+1]);
                        }
                    }
                }
            }
        }
        
        for(vector<ImgClumpRG*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                (*iterClumps)->meanVals[n] = (*iterClumps)->sumVals[n] / (*iterClumps)->pxls->size();
            }
            (*iterClumps)->neighbours.sort();
            (*iterClumps)->neighbours.unique();
        }
        
        unsigned int numThresSteps = ceil((maxThreshold - initThreshold)/thresholdIncrements);
        float cThres = 0;
        float percentInAreaInc = 0;
        ImgClumpRG *cRegion = NULL;
        ImgClumpRG *pRegion = NULL;
        list<unsigned long> *pClumpIds = NULL;
        list<unsigned long> *cClumpIds = NULL;
        
        cout << "Processing seeds\n";
        for(vector<ClumpSeed>::iterator iterSeeds = seeds->begin(); iterSeeds != seeds->end(); ++iterSeeds)
        {
            cout << "Processing seed " << (*iterSeeds).seedID << endl;
            pClumpIds = new list<unsigned long>();
            pRegion = this->growRegion(initThreshold, maxIterations, clumpTab, numSpecBands, (*iterSeeds).clumpID, (*iterSeeds).seedID, pClumpIds);
            
            if(pRegion == NULL)
            {
                pClumpIds->push_back((*iterSeeds).clumpID);
                cClumpIds = pClumpIds;
            }
            else
            {
                // Try seed growth thresholds until the 'right' answer is found.
                for(unsigned int i = 1; i < numThresSteps; ++i)
                {
                    cThres = initThreshold + (i * thresholdIncrements);
                    
                    cClumpIds = new list<unsigned long>();
                    cRegion = this->growRegion(cThres, maxIterations, clumpTab, numSpecBands, (*iterSeeds).clumpID, (*iterSeeds).seedID, cClumpIds);
                    
                    if(cRegion != NULL)
                    {
                        percentInAreaInc = (((double)cRegion->pxls->size()) - ((double)pRegion->pxls->size()))/((double)cRegion->pxls->size());
                        
                        //cout << "percentInAreaInc = " << percentInAreaInc << endl;
                        
                        if(percentInAreaInc > 0.9)
                        {
                            delete[] cRegion->meanVals;
                            delete[] cRegion->sumVals;
                            delete cRegion->pxls;
                            delete cRegion;
                            delete cClumpIds;
                            
                            cRegion = pRegion;
                            cClumpIds = pClumpIds;
                            break;
                        }
                        else
                        {
                            // Delete Previous
                            delete[] pRegion->meanVals;
                            delete[] pRegion->sumVals;
                            delete pRegion->pxls;
                            delete pRegion;
                            delete pClumpIds;
                            
                            pRegion = cRegion;
                            pClumpIds = cClumpIds;
                        }
                    }
                    else
                    {
                        delete cClumpIds;
                        cRegion = pRegion;
                        cClumpIds = pClumpIds;
                        break;
                    }
                }
            }
            
            for(list<unsigned long>::iterator iterClumpIDs = cClumpIds->begin(); iterClumpIDs != cClumpIds->end(); ++iterClumpIDs)
            {
                clumpTab->at((*iterClumpIDs)-1)->seedVal = (*iterSeeds).seedID;
            }

            // Clear up.
            if(cRegion != NULL)
            {
                delete[] cRegion->meanVals;
                delete[] cRegion->sumVals;
                delete cRegion->pxls;
                delete cRegion;
                delete cClumpIds;
            }
        }
        
        // Output the grown seed to the output image.
        cout << "Writing output to image\n";
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            outBand->RasterIO(GF_Read, 0, i, width, 1, outVals, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    outVals[j] = clumpTab->at(clumpIdxs[j]-1)->seedVal;
                }
            }
            outBand->RasterIO(GF_Write, 0, i, width, 1, outVals, width, 1, GDT_UInt32, 0, 0);
        }
        
        for(vector<ImgClumpRG*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            delete[] (*iterClumps)->meanVals;
            delete[] (*iterClumps)->sumVals;
            delete (*iterClumps)->pxls;
            delete (*iterClumps);
        }
        delete clumpTab;
        delete[] outVals;
        delete[] clumpIdxs;
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            delete[] spectralVals[n]; 
        }
        delete[] spectralVals;

    }
    
    
    ImgClumpRG* RSGISRegionGrowingSegmentation::growRegion(float threshold, unsigned int maxNumIterations, vector<ImgClumpRG*> *clumpTab, unsigned int numSpecBands, unsigned long seedClumpID, unsigned long seed, list<unsigned long> *regionClumps)throw(RSGISImageCalcException)
    {
        ImgClumpRG *region = NULL;
        try 
        {
            region = new ImgClumpRG(seed);
            region->meanVals = new float[numSpecBands];
            region->sumVals = new float[numSpecBands];
            region->pxls = new vector<PxlLoc>();
            regionClumps->clear();
            regionClumps->push_back(seedClumpID);
            
            ImgClumpRG *tmpClump = clumpTab->at(seedClumpID-1);
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                region->meanVals[n] = tmpClump->meanVals[n];
                region->sumVals[n] = tmpClump->sumVals[n];
            }
            for(vector<PxlLoc>::iterator iterPxls = tmpClump->pxls->begin(); iterPxls != tmpClump->pxls->end(); ++iterPxls)
            {
                region->pxls->push_back(*iterPxls);
            }
            for(list<unsigned long>::iterator iterNeighbours = tmpClump->neighbours.begin(); iterNeighbours != tmpClump->neighbours.end(); ++iterNeighbours)
            {
                region->neighbours.push_back(*iterNeighbours);
            }
            
            float *cMean = new float[numSpecBands];
            list<unsigned long> nextNeighbours;
            list<unsigned long> testedClumps;
            testedClumps.push_back(seedClumpID);
            double distance = 0;
            bool alreadyTested = true;
            bool change = true;
            bool maxNumIterationsReached = false;
            unsigned int numIterations = 0;
            while(change)
            {
                ++numIterations;
                change = false;
                // Make copy of mean values.
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    cMean[n] = region->meanVals[n];
                }
                
                // Test all neighbours to see whether they are part of the region.
                nextNeighbours.clear();
                for(list<unsigned long>::iterator iterNeighbour = region->neighbours.begin(); iterNeighbour != region->neighbours.end(); ++iterNeighbour)
                {
                    tmpClump = clumpTab->at(*iterNeighbour-1);
                    
                    distance = 0;
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        distance += ((tmpClump->meanVals[n] - cMean[n])*(tmpClump->meanVals[n] - cMean[n]));
                    }
                    distance = sqrt(distance);
                    
                    if(distance < threshold)
                    {
                        regionClumps->push_back(*iterNeighbour);
                        for(unsigned int n = 0; n < numSpecBands; ++n)
                        {
                            region->sumVals[n] += tmpClump->sumVals[n];
                        }
                        for(vector<PxlLoc>::iterator iterPxls = tmpClump->pxls->begin(); iterPxls != tmpClump->pxls->end(); ++iterPxls)
                        {
                            region->pxls->push_back(*iterPxls);
                        }
                        for(list<unsigned long>::iterator iterClumpNeighbours = tmpClump->neighbours.begin(); iterClumpNeighbours != tmpClump->neighbours.end(); ++iterClumpNeighbours)
                        {
                            nextNeighbours.push_back(*iterClumpNeighbours);
                        }
                        change = true;
                    }
                    
                    testedClumps.push_back(*iterNeighbour);
                }
                
                // Recalculate the mean of the region.
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    region->meanVals[n] = region->sumVals[n]/region->pxls->size();
                }
                
                region->neighbours.clear();
                nextNeighbours.sort();
                nextNeighbours.unique();
                for(list<unsigned long>::iterator iterNeighbour = nextNeighbours.begin(); iterNeighbour != nextNeighbours.end(); ++iterNeighbour)
                {
                    alreadyTested = false;
                    
                    for(list<unsigned long>::iterator iterClumps = testedClumps.begin(); iterClumps != testedClumps.end(); ++iterClumps)
                    {
                        if((*iterNeighbour) == (*iterClumps))
                        {
                            alreadyTested = true;
                            break;
                        }
                    }
                    
                    if(!alreadyTested)
                    {
                        region->neighbours.push_back(*iterNeighbour);
                    }
                }
                
                if(region->neighbours.size() == 0)
                {
                     break;
                }
                
                if(numIterations == maxNumIterations)
                {
                    maxNumIterationsReached = true;
                    break;
                }
            }
            
            if(maxNumIterationsReached)
            {
                delete[] region->meanVals;
                delete[] region->sumVals;
                delete region->pxls;
                delete region;
                region = NULL;
                regionClumps->clear();
            }
                        
            delete[] cMean;
        } 
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
        
        return region;
    }

                                                                   
    RSGISRegionGrowingSegmentation::~RSGISRegionGrowingSegmentation()
    {
        
    } 
    
    
    
    
    
    
    
    
    
    
    RSGISGenerateSeeds::RSGISGenerateSeeds()
    {
        
    }
    
    void RSGISGenerateSeeds::genSeedsHistogram(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *output, vector<BandThreshold> *thresholds) throw(RSGISImageCalcException)
    {
        if((spectral->GetRasterXSize() != clumps->GetRasterXSize()) |
           (spectral->GetRasterXSize() != output->GetRasterXSize()))
        {
            throw RSGISImageCalcException("Widths are not the same");
        }
        if((spectral->GetRasterYSize() != clumps->GetRasterYSize()) |
           (spectral->GetRasterYSize() != output->GetRasterYSize()))
        {
            throw RSGISImageCalcException("Heights are not the same");
        }
        
        if(((unsigned int)spectral->GetRasterCount()) != thresholds->size())
        {
            throw RSGISImageCalcException("The number of bands is not the same as the number of thresholds supplied.");
        }
        
        for(unsigned int i = 0; i < thresholds->size(); ++i)
        {
            if((thresholds->at(i).band < 1) | (thresholds->at(i).band > ((unsigned int) spectral->GetRasterCount())))
            {
                throw RSGISImageCalcException("Band specified is not within the image.");
            }
        }
        
        RSGISImageUtils imgUtils;
        imgUtils.zerosUIntGDALDataset(output);
        
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        float *percentThresholds = new float[numSpecBands];
        bool bandFound = false;
        for(unsigned int i = 1; i <= numSpecBands; ++i)
        {
            bandFound = false;
            for(unsigned int j = 0; j < thresholds->size(); ++j)
            {
                if(i == thresholds->at(j).band)
                {
                    if((thresholds->at(j).threshold < 0) | (thresholds->at(j).threshold > 1))
                    {
                        throw RSGISImageCalcException("Threshold should be between 0 and 1.");
                    }
                    percentThresholds[i-1] = thresholds->at(j).threshold;
                    bandFound = true;
                    break;
                }
            }
            if(!bandFound)
            {
                throw RSGISImageCalcException("Thresholds were not provided for all bands.");
            }
        }
        
        
        unsigned int *clumpIdxs = new unsigned int[width];
        unsigned int *outVals = new unsigned int[width];
        float **spectralVals = new float*[numSpecBands];
        
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
            spectralVals[n] = new float[width]; 
        }
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        GDALRasterBand *outBand = output->GetRasterBand(1);
        
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
        
        vector<ImgClumpMean*> *clumpTab = new vector<ImgClumpMean*>();
        clumpTab->reserve(maxClumpIdx);
        ImgClumpMean *cClump;
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            cClump = new ImgClumpMean(i+1);
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
        
        
        
        for(vector<ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                (*iterClumps)->meanVals[n] = (*iterClumps)->sumVals[n] / (*iterClumps)->numPxls;
            }
        }
        
        float *minVals = new float[numSpecBands];
        float *maxVals = new float[numSpecBands];
        bool first = true;
        for(vector<ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            if(first)
            {
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    minVals[n] = (*iterClumps)->meanVals[n];
                    maxVals[n] = (*iterClumps)->meanVals[n];
                }
                first = false;
            }
            else
            {
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    if((*iterClumps)->meanVals[n] < minVals[n])
                    {
                        minVals[n] = (*iterClumps)->meanVals[n];
                    }
                    else if((*iterClumps)->meanVals[n] > maxVals[n])
                    {
                        maxVals[n] = (*iterClumps)->meanVals[n];
                    }
                }
            }
        }
        
        unsigned int **histogram = new unsigned int*[numSpecBands];
        unsigned int *numBins = new unsigned int[numSpecBands];
        unsigned int numVals = 0;
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            numBins[n] = ceil(maxVals[n] - minVals[n]);
            histogram[n] = new unsigned int[numBins[n]];
            for(unsigned int i = 0; i < numBins[n]; ++i)
            {
                histogram[n][i] = 0;
            }
        }
        
        unsigned int idx = 0;
        for(vector<ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                idx = floor((*iterClumps)->meanVals[n] - minVals[n]);
                ++histogram[n][idx];
            }
            ++numVals;
        }
        
        float *specThresholds = new float[numSpecBands];
        unsigned int sum = 0;
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            sum = 0;
            for(long i = numBins[n]-1; i >= 0; --i)
            {
                sum += histogram[n][i];
                if((((float)sum)/numVals) > percentThresholds[n])
                {
                    specThresholds[n] = minVals[n] + i;
                    cout << "Band " << n+1 << " threshold = " << specThresholds[n] << endl;
                    break;
                }
            }
        }
        
        bool classified = false;
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    classified = true;
                    cClump = clumpTab->at(clumpIdxs[j] - 1);
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        if(cClump->meanVals[n] < specThresholds[n])
                        {
                            classified = false;
                        }
                    }
                    
                    if(classified)
                    {
                        outVals[j] = clumpIdxs[j];
                    }
                    else
                    {
                        outVals[j] = 0;
                    }
                }
                else
                {
                    outVals[j] = 0;
                }
            }
            
            outBand->RasterIO(GF_Write, 0, i, width, 1, outVals, width, 1, GDT_UInt32, 0, 0);
        }
        
        
        delete [] specThresholds;
        delete [] percentThresholds;
        delete [] minVals;
        delete [] maxVals;
        delete [] numBins;
        
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            delete [] histogram[n];
        }
        delete [] histogram;
        
        delete[] clumpIdxs;
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            delete[] spectralVals[n];
        }
        delete[] spectralVals;
        
        delete[] spectralBands;
        
        delete[] outVals;
        
        for(vector<ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            delete[] (*iterClumps)->meanVals;
            delete[] (*iterClumps)->sumVals;
            delete *iterClumps;
        }
        delete clumpTab;
    }
    
    void RSGISGenerateSeeds::genSeedsHistogram(GDALDataset *spectral, GDALDataset *clumps, string outputFile, vector<BandThreshold> *thresholds) throw(RSGISImageCalcException)
    {
        if(spectral->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw RSGISImageCalcException("Widths are not the same");
        }
        if(spectral->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw RSGISImageCalcException("Heights are not the same");
        }
        
        if(((unsigned int)spectral->GetRasterCount()) != thresholds->size())
        {
            throw RSGISImageCalcException("The number of bands is not the same as the number of thresholds supplied.");
        }
        
        for(unsigned int i = 0; i < thresholds->size(); ++i)
        {
            if((thresholds->at(i).band < 1) | (thresholds->at(i).band > ((unsigned int) spectral->GetRasterCount())))
            {
                throw RSGISImageCalcException("Band specified is not within the image.");
            }
        }
                
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        float *percentThresholds = new float[numSpecBands];
        bool bandFound = false;
        for(unsigned int i = 1; i <= numSpecBands; ++i)
        {
            bandFound = false;
            for(unsigned int j = 0; j < thresholds->size(); ++j)
            {
                if(i == thresholds->at(j).band)
                {
                    if((thresholds->at(j).threshold < 0) | (thresholds->at(j).threshold > 1))
                    {
                        throw RSGISImageCalcException("Threshold should be between 0 and 1.");
                    }
                    percentThresholds[i-1] = thresholds->at(j).threshold;
                    bandFound = true;
                    break;
                }
            }
            if(!bandFound)
            {
                throw RSGISImageCalcException("Thresholds were not provided for all bands.");
            }
        }
        
        
        unsigned int *clumpIdxs = new unsigned int[width];
        float **spectralVals = new float*[numSpecBands];
        
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
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
        
        vector<ImgClumpMean*> *clumpTab = new vector<ImgClumpMean*>();
        clumpTab->reserve(maxClumpIdx);
        ImgClumpMean *cClump;
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            cClump = new ImgClumpMean(i+1);
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
        
        
        
        for(vector<ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                (*iterClumps)->meanVals[n] = (*iterClumps)->sumVals[n] / (*iterClumps)->numPxls;
            }
        }
        
        float *minVals = new float[numSpecBands];
        float *maxVals = new float[numSpecBands];
        bool first = true;
        for(vector<ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            if(first)
            {
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    minVals[n] = (*iterClumps)->meanVals[n];
                    maxVals[n] = (*iterClumps)->meanVals[n];
                }
                first = false;
            }
            else
            {
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    if((*iterClumps)->meanVals[n] < minVals[n])
                    {
                        minVals[n] = (*iterClumps)->meanVals[n];
                    }
                    else if((*iterClumps)->meanVals[n] > maxVals[n])
                    {
                        maxVals[n] = (*iterClumps)->meanVals[n];
                    }
                }
            }
        }
        
        unsigned int **histogram = new unsigned int*[numSpecBands];
        unsigned int *numBins = new unsigned int[numSpecBands];
        unsigned int numVals = 0;
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            numBins[n] = ceil(maxVals[n] - minVals[n]);
            histogram[n] = new unsigned int[numBins[n]];
            for(unsigned int i = 0; i < numBins[n]; ++i)
            {
                histogram[n][i] = 0;
            }
        }
        
        unsigned int idx = 0;
        for(vector<ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                idx = floor((*iterClumps)->meanVals[n] - minVals[n]);
                ++histogram[n][idx];
            }
            ++numVals;
        }
        
        float *specThresholds = new float[numSpecBands];
        unsigned int sum = 0;
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            sum = 0;
            for(long i = numBins[n]-1; i >= 0; --i)
            {
                sum += histogram[n][i];
                if((((float)sum)/numVals) > percentThresholds[n])
                {
                    specThresholds[n] = minVals[n] + i;
                    cout << "Band " << n+1 << " threshold = " << specThresholds[n] << endl;
                    break;
                }
            }
        }
        
        bool classified = false;
        list<unsigned long> outClumpsIds;
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    classified = true;
                    cClump = clumpTab->at(clumpIdxs[j] - 1);
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        if(cClump->meanVals[n] < specThresholds[n])
                        {
                            classified = false;
                        }
                    }
                    
                    if(classified)
                    {
                        outClumpsIds.push_back(clumpIdxs[j]);
                    }
                }
            }            
        }
        
        outClumpsIds.sort();
        outClumpsIds.unique();
        
        ofstream outTxtFile;
        outTxtFile.open(outputFile.c_str());
        unsigned long seedID = 1;
        for(list<unsigned long>::iterator iterClumps = outClumpsIds.begin(); iterClumps != outClumpsIds.end(); ++iterClumps)
        {
            outTxtFile << *iterClumps << "," << seedID++ << endl;
        }
        outTxtFile.flush();
        outTxtFile.close();
        
        delete [] specThresholds;
        delete [] percentThresholds;
        delete [] minVals;
        delete [] maxVals;
        delete [] numBins;
        
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            delete [] histogram[n];
        }
        delete [] histogram;
        
        delete[] clumpIdxs;
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            delete[] spectralVals[n];
        }
        delete[] spectralVals;
        
        delete[] spectralBands;
                
        for(vector<ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            delete[] (*iterClumps)->meanVals;
            delete[] (*iterClumps)->sumVals;
            delete *iterClumps;
        }
        delete clumpTab;
    }
    
    RSGISGenerateSeeds::~RSGISGenerateSeeds()
    {
        
    }
    
    
    
    RSGISSelectClumps::RSGISSelectClumps()
    {
        
    }
    
    void RSGISSelectClumps::selectClumps(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *largeClumps, GDALDataset *output, ClumpSelection selection) throw(RSGISImageCalcException)
    {
        if((spectral->GetRasterXSize() != clumps->GetRasterXSize()) |
           (spectral->GetRasterXSize() != largeClumps->GetRasterXSize()) |
           (spectral->GetRasterXSize() != output->GetRasterXSize()))
        {
            throw RSGISImageCalcException("Widths are not the same");
        }
        if((spectral->GetRasterYSize() != clumps->GetRasterYSize()) |
           (spectral->GetRasterYSize() != largeClumps->GetRasterYSize()) |
           (spectral->GetRasterYSize() != output->GetRasterYSize()))
        {
            throw RSGISImageCalcException("Heights are not the same");
        }

        
        RSGISImageUtils imgUtils;
        imgUtils.zerosUIntGDALDataset(output);
        
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        unsigned int *clumpIdxs = new unsigned int[width];
        unsigned int *lClumpIdxs = new unsigned int[width];
        unsigned int *outClumpIdxs = new unsigned int[width];
        float **spectralVals = new float*[numSpecBands];
        
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
            spectralVals[n] = new float[width]; 
        }
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        GDALRasterBand *lClumpBand = largeClumps->GetRasterBand(1);
        GDALRasterBand *outClumpBand = output->GetRasterBand(1);
        
        unsigned long maxClumpIdx = 0;
        unsigned long maxLClumpIdx = 0;
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            lClumpBand->RasterIO(GF_Read, 0, i, width, 1, lClumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int j = 0; j < width; ++j)
            {
                if((i == 0) & (j == 0))
                {
                    maxClumpIdx = clumpIdxs[j];
                    maxLClumpIdx = lClumpIdxs[j];
                }
                else 
                {
                    if(clumpIdxs[j] > maxClumpIdx)
                    {
                        maxClumpIdx = clumpIdxs[j];
                    }
                    
                    if(lClumpIdxs[j] > maxLClumpIdx)
                    {
                        maxLClumpIdx = lClumpIdxs[j];
                    }
                }
            }
        }
        
        vector< list<unsigned long>* > *largeClumpTab = new vector< list<unsigned long>* >();
        largeClumpTab->reserve(maxLClumpIdx);
        for(unsigned int i = 0; i < maxLClumpIdx; ++i)
        {
            largeClumpTab->push_back(new list<unsigned long>());
        }
        
        vector<ImgClumpMean*> *clumpTab = new vector<ImgClumpMean*>();
        clumpTab->reserve(maxClumpIdx);
        ImgClumpMean *cClump;
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            cClump = new ImgClumpMean(i+1);
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
            lClumpBand->RasterIO(GF_Read, 0, i, width, 1, lClumpIdxs, width, 1, GDT_UInt32, 0, 0);
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
                
                if(lClumpIdxs[j] != 0)
                {
                    largeClumpTab->at(lClumpIdxs[j]-1)->push_back(clumpIdxs[j]);
                }
            }
        }
        
        for(vector<ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                (*iterClumps)->meanVals[n] = (*iterClumps)->sumVals[n] / (*iterClumps)->numPxls;
            }
        }
        
        for(unsigned int i = 0; i < maxLClumpIdx; ++i)
        {
            largeClumpTab->at(i)->sort();
            largeClumpTab->at(i)->unique();
        }
        
        list<unsigned long> outClumps;
        float tmpMeanVal = 0;
        float distVal = 0;
        unsigned long sClump = 0;
        float meanVal = 0;
        float meanBrightness = 0;
        float medianBrightness = 0;
        float sumVals = 0;
        list<float> allVals;
        vector<double> allValsVec;
        bool first = true;
        for(unsigned int i = 0; i < maxLClumpIdx; ++i)
        {
            first = true;
            allValsVec.clear();
            allVals.clear();
            sumVals = 0;
            if(largeClumpTab->at(i)->size() > 0)
            {
                for(list<unsigned long>::iterator iterClump = largeClumpTab->at(i)->begin(); iterClump != largeClumpTab->at(i)->end(); ++iterClump)
                {
                    for(unsigned int i = 0; i < numSpecBands; ++i)
                    {
                        if(i == 0)
                        {
                            meanVal = clumpTab->at((*iterClump)-1)->meanVals[i];
                        }
                        else
                        {
                            meanVal += clumpTab->at((*iterClump)-1)->meanVals[i];
                        }
                    }
                    meanVal = meanVal / numSpecBands;
                    if((selection == min) | (selection == max))
                    {
                        if(first)
                        {
                            tmpMeanVal = meanVal;
                            sClump = *iterClump;
                            first = false;
                        }
                        else
                        {
                            if(selection == max)
                            {
                                if(meanVal > tmpMeanVal)
                                {
                                    tmpMeanVal = meanVal;
                                    sClump = *iterClump;
                                }
                            }
                            else if(selection == min)
                            {
                                if(meanVal < tmpMeanVal)
                                {
                                    tmpMeanVal = meanVal;
                                    sClump = *iterClump;
                                }
                            }
                        }
                    }
                    else if(selection == median)
                    {
                        allVals.push_back(meanVal);
                    }
                    else if(selection == mean)
                    {
                        sumVals += meanVal;
                    }
                    else if((selection == percent75th) | (selection == percent95th))
                    {
                        allValsVec.push_back(meanVal);
                    }
                    else
                    {
                        throw RSGISImageCalcException("Did not recognise clump selection choice.");
                    }
                }
                if(selection == median)
                {
                    allVals.sort();
                    unsigned int counter = 0;
                    for(list<float>::iterator iterVals = allVals.begin(); iterVals != allVals.end(); ++iterVals)
                    {
                        if(counter++ == (allVals.size()/2))
                        {
                            medianBrightness = *iterVals;
                            break;
                        }
                    }
                    
                    first = true;
                    for(list<unsigned long>::iterator iterClump = largeClumpTab->at(i)->begin(); iterClump != largeClumpTab->at(i)->end(); ++iterClump)
                    {
                        for(unsigned int i = 0; i < numSpecBands; ++i)
                        {
                            if(i == 0)
                            {
                                meanVal = clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                            else
                            {
                                meanVal += clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                        }
                        meanVal = meanVal / numSpecBands;
                        distVal = sqrt((meanVal - meanVal)*(meanVal - meanVal));
                        if(first)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                            first = false;
                        }
                        else if( distVal < tmpMeanVal)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                        }
                    }
                }
                else if(selection == mean)
                {
                    meanBrightness = sumVals/largeClumpTab->size();
                    first = true;
                    for(list<unsigned long>::iterator iterClump = largeClumpTab->at(i)->begin(); iterClump != largeClumpTab->at(i)->end(); ++iterClump)
                    {
                        for(unsigned int i = 0; i < numSpecBands; ++i)
                        {
                            if(i == 0)
                            {
                                meanVal = clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                            else
                            {
                                meanVal += clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                        }
                        meanVal = meanVal / numSpecBands;
                        distVal = sqrt((meanVal - meanBrightness)*(meanVal - meanBrightness));
                        if(first)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                            first = false;
                        }
                        else if( distVal < tmpMeanVal)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                        }
                    }
                }
                else if(selection == percent75th)
                {
                    sort(allValsVec.begin(), allValsVec.end());
                    float percentileVal = gsl_stats_quantile_from_sorted_data(&allValsVec[0], 1, allValsVec.size(), 0.75);
                    //cout << "percentileVal = " << percentileVal << endl;
                    first = true;
                    for(list<unsigned long>::iterator iterClump = largeClumpTab->at(i)->begin(); iterClump != largeClumpTab->at(i)->end(); ++iterClump)
                    {
                        for(unsigned int i = 0; i < numSpecBands; ++i)
                        {
                            if(i == 0)
                            {
                                meanVal = clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                            else
                            {
                                meanVal += clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                        }
                        meanVal = meanVal / numSpecBands;
                        distVal = sqrt((meanVal - percentileVal)*(meanVal - percentileVal));
                        //cout << "\t" << *iterClump << " has mean value " << meanVal << " Percentile Value = " << percentileVal << endl;
                        if(first)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                            first = false;
                        }
                        else if( distVal < tmpMeanVal)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                        }
                    }
                }
                else if(selection == percent95th)
                {
                    sort(allValsVec.begin(), allValsVec.end());
                    float percentileVal = gsl_stats_quantile_from_sorted_data(&allValsVec[0], 1, allValsVec.size(), 0.95);
                    //cout << "percentileVal = " << percentileVal << endl;
                    first = true;
                    for(list<unsigned long>::iterator iterClump = largeClumpTab->at(i)->begin(); iterClump != largeClumpTab->at(i)->end(); ++iterClump)
                    {
                        for(unsigned int i = 0; i < numSpecBands; ++i)
                        {
                            if(i == 0)
                            {
                                meanVal = clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                            else
                            {
                                meanVal += clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                        }
                        meanVal = meanVal / numSpecBands;
                        distVal = sqrt((meanVal - percentileVal)*(meanVal - percentileVal));
                        //cout << "\t" << *iterClump << " has mean value " << meanVal << " Percentile Value = " << percentileVal << endl;
                        if(first)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                            first = false;
                        }
                        else if( distVal < tmpMeanVal)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                        }
                    }
                }
                
                //cout << "sClump = " << sClump << endl;
                outClumps.push_back(sClump);
            }
        }
        
        bool foundClump = false;
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    foundClump = false;
                    for(list<unsigned long>::iterator iterClump = outClumps.begin(); iterClump != outClumps.end(); ++iterClump)
                    {
                        if(clumpIdxs[j] == *iterClump)
                        {
                            outClumpIdxs[j] = clumpIdxs[j];
                            foundClump = true;
                            break;
                        }
                    }
                    if(!foundClump)
                    {
                        outClumpIdxs[j] = 0;
                    }
                }
                else
                {
                    outClumpIdxs[j] = 0;
                }
            }
            
            outClumpBand->RasterIO(GF_Write, 0, i, width, 1, outClumpIdxs, width, 1, GDT_UInt32, 0, 0);
        }
                
        for(vector< list<unsigned long>* >::iterator iterLClumps = largeClumpTab->begin(); iterLClumps != largeClumpTab->end(); ++iterLClumps)
        {
            delete *iterLClumps;
        }
        delete largeClumpTab;
        
        delete[] lClumpIdxs;
        delete[] outClumpIdxs;
        delete[] clumpIdxs;
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            delete[] spectralVals[n];
        }
        delete[] spectralVals;
        delete[] spectralBands;
        
        for(vector<ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            delete[] (*iterClumps)->meanVals;
            delete[] (*iterClumps)->sumVals;
            delete *iterClumps;
        }
        delete clumpTab;
    }
    
    void RSGISSelectClumps::selectClumps(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *largeClumps, string outputFile, ClumpSelection selection) throw(RSGISImageCalcException)
    {
        if((spectral->GetRasterXSize() != clumps->GetRasterXSize()) |
           (spectral->GetRasterXSize() != largeClumps->GetRasterXSize()))
        {
            throw RSGISImageCalcException("Widths are not the same");
        }
        if((spectral->GetRasterYSize() != clumps->GetRasterYSize()) |
           (spectral->GetRasterYSize() != largeClumps->GetRasterYSize()))
        {
            throw RSGISImageCalcException("Heights are not the same");
        }
        
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        unsigned int *clumpIdxs = new unsigned int[width];
        unsigned int *lClumpIdxs = new unsigned int[width];
        float **spectralVals = new float*[numSpecBands];
        
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
            spectralVals[n] = new float[width]; 
        }
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        GDALRasterBand *lClumpBand = largeClumps->GetRasterBand(1);
        
        unsigned long maxClumpIdx = 0;
        unsigned long maxLClumpIdx = 0;
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            lClumpBand->RasterIO(GF_Read, 0, i, width, 1, lClumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int j = 0; j < width; ++j)
            {
                if((i == 0) & (j == 0))
                {
                    maxClumpIdx = clumpIdxs[j];
                    maxLClumpIdx = lClumpIdxs[j];
                }
                else 
                {
                    if(clumpIdxs[j] > maxClumpIdx)
                    {
                        maxClumpIdx = clumpIdxs[j];
                    }
                    
                    if(lClumpIdxs[j] > maxLClumpIdx)
                    {
                        maxLClumpIdx = lClumpIdxs[j];
                    }
                }
            }
        }
        
        vector< list<unsigned long>* > *largeClumpTab = new vector< list<unsigned long>* >();
        largeClumpTab->reserve(maxLClumpIdx);
        for(unsigned int i = 0; i < maxLClumpIdx; ++i)
        {
            largeClumpTab->push_back(new list<unsigned long>());
        }
        
        vector<ImgClumpMean*> *clumpTab = new vector<ImgClumpMean*>();
        clumpTab->reserve(maxClumpIdx);
        ImgClumpMean *cClump;
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            cClump = new ImgClumpMean(i+1);
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
            lClumpBand->RasterIO(GF_Read, 0, i, width, 1, lClumpIdxs, width, 1, GDT_UInt32, 0, 0);
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
                
                if(lClumpIdxs[j] != 0)
                {
                    largeClumpTab->at(lClumpIdxs[j]-1)->push_back(clumpIdxs[j]);
                }
            }
        }
        
        for(vector<ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                (*iterClumps)->meanVals[n] = (*iterClumps)->sumVals[n] / (*iterClumps)->numPxls;
            }
        }
        
        for(unsigned int i = 0; i < maxLClumpIdx; ++i)
        {
            largeClumpTab->at(i)->sort();
            largeClumpTab->at(i)->unique();
        }
        
        list<unsigned long> outClumps;
        float tmpMeanVal = 0;
        float distVal = 0;
        unsigned long sClump = 0;
        float meanVal = 0;
        float meanBrightness = 0;
        float medianBrightness = 0;
        float sumVals = 0;
        list<float> allVals;
        vector<double> allValsVec;
        bool first = true;
        for(unsigned int i = 0; i < maxLClumpIdx; ++i)
        {
            first = true;
            allValsVec.clear();
            allVals.clear();
            sumVals = 0;
            if(largeClumpTab->at(i)->size() > 0)
            {
                for(list<unsigned long>::iterator iterClump = largeClumpTab->at(i)->begin(); iterClump != largeClumpTab->at(i)->end(); ++iterClump)
                {
                    for(unsigned int i = 0; i < numSpecBands; ++i)
                    {
                        if(i == 0)
                        {
                            meanVal = clumpTab->at((*iterClump)-1)->meanVals[i];
                        }
                        else
                        {
                            meanVal += clumpTab->at((*iterClump)-1)->meanVals[i];
                        }
                    }
                    meanVal = meanVal / numSpecBands;
                    if((selection == min) | (selection == max))
                    {
                        if(first)
                        {
                            tmpMeanVal = meanVal;
                            sClump = *iterClump;
                            first = false;
                        }
                        else
                        {
                            if(selection == max)
                            {
                                if(meanVal > tmpMeanVal)
                                {
                                    tmpMeanVal = meanVal;
                                    sClump = *iterClump;
                                }
                            }
                            else if(selection == min)
                            {
                                if(meanVal < tmpMeanVal)
                                {
                                    tmpMeanVal = meanVal;
                                    sClump = *iterClump;
                                }
                            }
                        }
                    }
                    else if(selection == median)
                    {
                        allVals.push_back(meanVal);
                    }
                    else if(selection == mean)
                    {
                        sumVals += meanVal;
                    }
                    else if((selection == percent75th) | (selection == percent95th))
                    {
                        allValsVec.push_back(meanVal);
                    }
                    else
                    {
                        throw RSGISImageCalcException("Did not recognise clump selection choice.");
                    }
                }
                if(selection == median)
                {
                    allVals.sort();
                    unsigned int counter = 0;
                    for(list<float>::iterator iterVals = allVals.begin(); iterVals != allVals.end(); ++iterVals)
                    {
                        if(counter++ == (allVals.size()/2))
                        {
                            medianBrightness = *iterVals;
                            break;
                        }
                    }
                    
                    first = true;
                    for(list<unsigned long>::iterator iterClump = largeClumpTab->at(i)->begin(); iterClump != largeClumpTab->at(i)->end(); ++iterClump)
                    {
                        for(unsigned int i = 0; i < numSpecBands; ++i)
                        {
                            if(i == 0)
                            {
                                meanVal = clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                            else
                            {
                                meanVal += clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                        }
                        meanVal = meanVal / numSpecBands;
                        distVal = sqrt((meanVal - meanVal)*(meanVal - meanVal));
                        if(first)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                            first = false;
                        }
                        else if( distVal < tmpMeanVal)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                        }
                    }
                }
                else if(selection == mean)
                {
                    meanBrightness = sumVals/largeClumpTab->size();
                    first = true;
                    for(list<unsigned long>::iterator iterClump = largeClumpTab->at(i)->begin(); iterClump != largeClumpTab->at(i)->end(); ++iterClump)
                    {
                        for(unsigned int i = 0; i < numSpecBands; ++i)
                        {
                            if(i == 0)
                            {
                                meanVal = clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                            else
                            {
                                meanVal += clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                        }
                        meanVal = meanVal / numSpecBands;
                        distVal = sqrt((meanVal - meanBrightness)*(meanVal - meanBrightness));
                        if(first)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                            first = false;
                        }
                        else if( distVal < tmpMeanVal)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                        }
                    }
                }
                else if(selection == percent75th)
                {
                    sort(allValsVec.begin(), allValsVec.end());
                    float percentileVal = gsl_stats_quantile_from_sorted_data(&allValsVec[0], 1, allValsVec.size(), 0.75);
                    //cout << "percentileVal = " << percentileVal << endl;
                    first = true;
                    for(list<unsigned long>::iterator iterClump = largeClumpTab->at(i)->begin(); iterClump != largeClumpTab->at(i)->end(); ++iterClump)
                    {
                        for(unsigned int i = 0; i < numSpecBands; ++i)
                        {
                            if(i == 0)
                            {
                                meanVal = clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                            else
                            {
                                meanVal += clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                        }
                        meanVal = meanVal / numSpecBands;
                        distVal = sqrt((meanVal - percentileVal)*(meanVal - percentileVal));
                        //cout << "\t" << *iterClump << " has mean value " << meanVal << " Percentile Value = " << percentileVal << endl;
                        if(first)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                            first = false;
                        }
                        else if( distVal < tmpMeanVal)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                        }
                    }
                }
                else if(selection == percent95th)
                {
                    sort(allValsVec.begin(), allValsVec.end());
                    float percentileVal = gsl_stats_quantile_from_sorted_data(&allValsVec[0], 1, allValsVec.size(), 0.95);
                    //cout << "percentileVal = " << percentileVal << endl;
                    first = true;
                    for(list<unsigned long>::iterator iterClump = largeClumpTab->at(i)->begin(); iterClump != largeClumpTab->at(i)->end(); ++iterClump)
                    {
                        for(unsigned int i = 0; i < numSpecBands; ++i)
                        {
                            if(i == 0)
                            {
                                meanVal = clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                            else
                            {
                                meanVal += clumpTab->at((*iterClump)-1)->meanVals[i];
                            }
                        }
                        meanVal = meanVal / numSpecBands;
                        distVal = sqrt((meanVal - percentileVal)*(meanVal - percentileVal));
                        //cout << "\t" << *iterClump << " has mean value " << meanVal << " Percentile Value = " << percentileVal << endl;
                        if(first)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                            first = false;
                        }
                        else if( distVal < tmpMeanVal)
                        {
                            tmpMeanVal = distVal;
                            sClump = *iterClump;
                        }
                    }
                }
                
                //cout << "sClump = " << sClump << endl;
                outClumps.push_back(sClump);
            }
        }
        
        ofstream outTxtFile;
        outTxtFile.open(outputFile.c_str());
        unsigned long seedID = 1;
        for(list<unsigned long>::iterator iterClumps = outClumps.begin(); iterClumps != outClumps.end(); ++iterClumps)
        {
            outTxtFile << *iterClumps << "," << seedID++ << endl;
        }
        outTxtFile.flush();
        outTxtFile.close();
        
        for(vector< list<unsigned long>* >::iterator iterLClumps = largeClumpTab->begin(); iterLClumps != largeClumpTab->end(); ++iterLClumps)
        {
            delete *iterLClumps;
        }
        delete largeClumpTab;
        
        delete[] lClumpIdxs;
        delete[] clumpIdxs;
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            delete[] spectralVals[n];
        }
        delete[] spectralVals;
        delete[] spectralBands;
        
        for(vector<ImgClumpMean*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            delete[] (*iterClumps)->meanVals;
            delete[] (*iterClumps)->sumVals;
            delete *iterClumps;
        }
        delete clumpTab;
    }
    
    RSGISSelectClumps::~RSGISSelectClumps()
    {
        
    }
    
    
}}
