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
	
	
}}

