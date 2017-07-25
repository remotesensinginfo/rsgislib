/*
 *  RSGISGenHistogram.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/07/2012.
 *  Copyright 2012 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#include "RSGISGenHistogram.h"

namespace rsgis { namespace img {
	
    RSGISGenHistogram::RSGISGenHistogram()
    {
        
    }
    
    void RSGISGenHistogram::genHistogram(GDALDataset **datasets, unsigned int numDS, std::string outputFile, unsigned int imgBand, double imgMin, double imgMax, float maskValue, float binWidth)throw(RSGISImageCalcException)
    {
        try 
        {
            double range = (imgMax - imgMin);
            unsigned int numBins = ceil((range/binWidth)+0.5);
            
            unsigned int *bins = new unsigned int[numBins];
            float *binRanges = new float[numBins+1];
            
            for(unsigned int i = 0; i < numBins; ++i)
            {
                bins[i] = 0;
                binRanges[i] = imgMin + (i * binWidth);
            }
            binRanges[numBins] = imgMin + (numBins * binWidth);
            
            // Populate the Histogram - using image calc...
            RSGISGenHistogramCalcVal *genHists = new RSGISGenHistogramCalcVal(bins, binRanges, imgBand, numBins, maskValue, binWidth);
            RSGISCalcImage calcImage = RSGISCalcImage(genHists);
            calcImage.calcImage(datasets, numDS);
            
            // Export the histogram to text file.
            std::ofstream outFile;
            outFile.precision(10);
            outFile.open(outputFile.c_str());
            if(outFile.is_open())
            {
                for(unsigned int i = 0; i < numBins; ++i)
                {
                    outFile << binRanges[i] << "," << bins[i] << std::endl;
                }
                outFile.flush();
                outFile.close();
            }
            else
            {
                RSGISImageCalcException("Could not generate the output text file.");
            }
            
            delete[] bins;
            delete[] binRanges;
            delete genHists;
            
        }
        catch (RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw e;
        }
    }
    
    unsigned int* RSGISGenHistogram::genGetHistogram(GDALDataset *dataset, unsigned int imgBand, double imgMin, double imgMax, float binWidth, unsigned int *nBins)throw(RSGISImageCalcException)
    {
        unsigned int *bins = NULL;
        try
        {
            double range = (imgMax - imgMin);
            *nBins = ceil((range/binWidth)+0.5);
            bins = new unsigned int[(*nBins)];
            float *binRanges = new float[(*nBins)+1];
            
            for(unsigned int i = 0; i < (*nBins); ++i)
            {
                bins[i] = 0;
                binRanges[i] = imgMin + (i * binWidth);
            }
            binRanges[(*nBins)] = imgMin + ((*nBins) * binWidth);
            
            // Populate the Histogram - using image calc...
            RSGISGenHistogramNoMaskCalcVal *genHists = new RSGISGenHistogramNoMaskCalcVal(bins, binRanges, imgBand, (*nBins), binWidth);
            RSGISCalcImage calcImage = RSGISCalcImage(genHists);
            calcImage.calcImage(&dataset, 1);
            
            delete[] binRanges;
            delete genHists;
        }
        catch (RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw e;
        }
        
        return bins;
    }
    
    void RSGISGenHistogram::gen2DHistogram(GDALDataset **datasets, unsigned int numDS, unsigned int img1BandIdx, unsigned int img2BandIdx, double **histgramMatrix, unsigned int numBins, double *img1Bins, double *img2Bins, double img1Scale, double img2Scale, double img1Off, double img2Off, double *rSq)throw(RSGISImageCalcException)
    {
        try
        {
            RSGISGen2DHistogramCalcVal *genHists = new RSGISGen2DHistogramCalcVal(img1BandIdx, img2BandIdx, histgramMatrix, numBins, img1Bins, img2Bins, img1Scale, img2Scale, img1Off, img2Off);
            RSGISCalcImage calcImage = RSGISCalcImage(genHists);
            calcImage.calcImage(datasets, numDS);
            delete genHists;
            
            double img1Mean = 0.0;
            double img1N = 0.0;
            double lclN = 0.0;
            double binValImg1 = 0.0;
            for(unsigned int i = 0; i < numBins; ++i)
            {
                lclN = 0.0;
                binValImg1 = img1Bins[i] + ((img1Bins[i+1]-img1Bins[i])/2);
                for(unsigned int j = 0; j < numBins; ++j)
                {
                    lclN += histgramMatrix[i][j];
                }
                img1Mean += (lclN * binValImg1);
                img1N += lclN;
            }
            img1Mean = img1Mean / img1N;
            
            double img1Diff2Mean = 0.0;
            double img1DiffImg2 = 0.0;
            double binValImg2 = 0.0;
            for(unsigned int i = 0; i < numBins; ++i)
            {
                lclN = 0.0;
                binValImg1 = img1Bins[i] + ((img1Bins[i+1]-img1Bins[i])/2);
                for(unsigned int j = 0; j < numBins; ++j)
                {
                    lclN += histgramMatrix[i][j];
                    binValImg2 = img2Bins[j] + ((img2Bins[j+1]-img2Bins[j])/2);
                    for(unsigned int n = 0; n < histgramMatrix[i][j]; ++n)
                    {
                        img1DiffImg2 += ((binValImg1 - binValImg2) * (binValImg1 - binValImg2));
                    }
                }
                
                for(unsigned int n = 0; n < lclN; ++n)
                {
                    img1Diff2Mean += ((binValImg1 - img1Mean) * (binValImg1 - img1Mean));
                }
            }
            
            *rSq = 1 - (img1DiffImg2 / img1Diff2Mean);
        }
        catch (RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw e;
        }
    }
    
    RSGISGenHistogram::~RSGISGenHistogram()
    {
        
    }
    
    
    RSGISGenHistogramCalcVal::RSGISGenHistogramCalcVal(unsigned int *bins, float *binRanges, unsigned int band, unsigned int numBins, float maskValue, float binWidth): RSGISCalcImageValue(0)
    {
        this->bins = bins;
        this->binRanges = binRanges;
        this->band = band;
        this->numBins = numBins;
        this->maskValue = maskValue;
        this->binWidth = binWidth;
    }
    
    void RSGISGenHistogramCalcVal::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        try
        {
            if(numBands <= 1)
            {
                throw RSGISImageCalcException("Must be at least 2 bands.");
            }
            
            if(band > numBands)
            {
                throw RSGISImageCalcException("Band is beyond band range of the image.");
            }
            
            float maskVal = bandValues[0];
            
            if(maskVal == this->maskValue)
            {
                float dist = bandValues[band] - binRanges[0];
                if(dist > 0)
                {
                    unsigned int binIdx = floor(dist/binWidth);
                    if(binIdx < numBins)
                    {
                        ++bins[binIdx];
                    }
                }
            }
        } 
        catch (RSGISImageCalcException &e)
        {
            throw e;
        }
    }

    RSGISGenHistogramCalcVal::~RSGISGenHistogramCalcVal()
    {
        
	}
    
    
    
    RSGISGenHistogramNoMaskCalcVal::RSGISGenHistogramNoMaskCalcVal(unsigned int *bins, float *binRanges, unsigned int band, unsigned int numBins, float binWidth): RSGISCalcImageValue(0)
    {
        this->bins = bins;
        this->binRanges = binRanges;
        this->band = band;
        this->numBins = numBins;
        this->binWidth = binWidth;
    }
    
    void RSGISGenHistogramNoMaskCalcVal::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        try
        {
            if(band > numBands)
            {
                throw RSGISImageCalcException("Band is beyond band range of the image.");
            }
            
            float dist = bandValues[band] - binRanges[0];
            if(dist > 0)
            {
                unsigned int binIdx = floor(dist/binWidth);
                if(binIdx < numBins)
                {
                    ++bins[binIdx];
                }
            }
        }
        catch (RSGISImageCalcException &e)
        {
            throw e;
        }
    }
    
    RSGISGenHistogramNoMaskCalcVal::~RSGISGenHistogramNoMaskCalcVal()
    {
        
	}
	
    
    
    
    RSGISGen2DHistogramCalcVal::RSGISGen2DHistogramCalcVal(unsigned int img1BandIdx, unsigned int img2BandIdx, double **histgramMatrix, unsigned int numBins, double *img1Bins, double *img2Bins, double img1Scale, double img2Scale, double img1Off, double img2Off): RSGISCalcImageValue(0)
    {
        this->img1BandIdx = img1BandIdx;
        this->img2BandIdx = img2BandIdx;
        this->histgramMatrix = histgramMatrix;
        this->numBins = numBins;
        this->img1Bins = img1Bins;
        this->img2Bins = img2Bins;
        this->img1Scale = img1Scale;
        this->img2Scale = img2Scale;
        this->img1Off = img1Off;
        this->img2Off = img2Off;
    }
    
    void RSGISGen2DHistogramCalcVal::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        try
        {
            if(numBands <= 1)
            {
                throw RSGISImageCalcException("Must be at least 2 bands.");
            }

            float img1Val = this->img1Off + (bandValues[img1BandIdx]*this->img1Scale);
            float img2Val = this->img2Off + (bandValues[img2BandIdx]*this->img2Scale);
  
            bool foundBinImg1 = false;
            unsigned int img1Bin = 0;
            
            for(unsigned i = 0; i < numBins; ++i)
            {
                if((img1Val >= img1Bins[i]) & (img1Val < img1Bins[i+1]))
                {
                    img1Bin = i;
                    foundBinImg1 = true;
                    break;
                }
            }
            
            if(foundBinImg1)
            {
                unsigned int img2Bin = 0;
                bool foundBinImg2 = false;
                for(unsigned i = 0; i < numBins; ++i)
                {
                    if((img2Val >= img2Bins[i]) & (img2Val < img2Bins[i+1]))
                    {
                        img2Bin = i;
                        foundBinImg2 = true;
                        break;
                    }
                }
                
                if(foundBinImg2)
                {
                    this->histgramMatrix[img1Bin][img2Bin] = this->histgramMatrix[img1Bin][img2Bin] + 1;
                }
            }
            
        }
        catch (RSGISImageCalcException &e)
        {
            throw e;
        }
    }
    
    RSGISGen2DHistogramCalcVal::~RSGISGen2DHistogramCalcVal()
    {
        
    }

    
    
    
	
}}

