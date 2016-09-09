/*
 *  RSGISNonLocalDenoising.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 23/02/2013.
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

#include "RSGISNonLocalDenoising.h"

namespace rsgis{namespace filter{
    
    RSGISApplyNonLocalDenoising::RSGISApplyNonLocalDenoising()
    {
    }
    
    void RSGISApplyNonLocalDenoising::ApplyFilter(GDALDataset **inputImageDS, int numDS, std::string outputImage, unsigned int filterWindowSize, unsigned int searchWindowSize, double aPar, double hPar, std::string gdalFormat, GDALDataType gdalDataType) throw(rsgis::img::RSGISImageCalcException,rsgis::img::RSGISImageBandException)
	{
        
        rsgis::img::RSGISImageUtils imgUtils;
        rsgis::math::RSGISVectors vectorUtils;
        
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		unsigned int numInBands = 0;
        unsigned int numOutBands;
        int xBlockSize = 0;
        int yBlockSize = 0;
        size_t numPxlsInSearchWindow = 0;
		
		float **inputDataSearchWindow = NULL;
		float **outputData = NULL;

        unsigned int searchWindowWidth = searchWindowSize;
        unsigned int searchWindowHeight = searchWindowSize;
        unsigned int searchWindowOverlap = 0;
        
        double invHParSq = -1.0/hPar*hPar;
        
        double *swPixelValues = NULL; // Pixel values for search window.
        double *swPixelWeights = NULL; // Pixel weights for search window.
		
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
        
        GDALDriver *gdalDriver = NULL;
        GDALDataset *outputImageDS = NULL;
		
		try
		{
			if(filterWindowSize % 2 == 0)
			{
				throw rsgis::img::RSGISImageCalcException("Window size needs to be an odd number (min = 3).");
			}
			else if(searchWindowSize < 2*filterWindowSize)
			{
				throw rsgis::img::RSGISImageCalcException("Search window size needs to at least twice the filter window size");
			}
			unsigned int filterWinPix = floor((float)filterWindowSize / 2); // Number of pixels each side of middle pixel
			unsigned int windowMid = floor(((float)filterWindowSize)/2.0); // Pixel at centre of block
            
            std::cout << "Search window Size: " << searchWindowSize << std::endl;
            std::cout << "Filter window Size: " << filterWindowSize << std::endl;
            
			// Find image overlap
            imgUtils.getImageOverlap(inputImageDS, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += inputImageDS[i]->GetRasterCount();
			}
            
            numOutBands = numInBands;
            
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(gdalDriver == NULL)
			{
				throw rsgis::img::RSGISImageBandException("Driver does not exists..");
			}
			std::cout << "New image width = " << width << " height = " << height << " bands = " << numOutBands << std::endl;
            
			outputImageDS = gdalDriver->Create(outputImage.c_str(), width, height, numOutBands, gdalDataType, NULL);
			
			if(outputImageDS == NULL)
			{
				throw rsgis::img::RSGISImageBandException("Output image could not be created. Check filepath.");
			}
			
            outputImageDS->SetGeoTransform(gdalTranslation);
            outputImageDS->SetProjection(inputImageDS[0]->GetProjectionRef());
            
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < inputImageDS[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = inputImageDS[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					counter++;
				}
			}
			
			//Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[numOutBands];
			for(int i = 0; i < numOutBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
			}
            
            // Subset window as multiple of block size.
            int outXBlockSize = 0;
            int outYBlockSize = 0;
            inputRasterBands[0]->GetBlockSize (&outXBlockSize, &outYBlockSize);
            
            std::cout << "outXBlockSize = " << outXBlockSize << ", outYBlockSize = " << outYBlockSize << std::endl;
            
            // Set search window height and width so multiples of block size
            if(searchWindowWidth > outXBlockSize)
            {
                searchWindowWidth =  floor((float)searchWindowWidth / outXBlockSize) + outXBlockSize;
            }
            if(searchWindowHeight > outYBlockSize)
            {
                searchWindowHeight = floor((float)searchWindowHeight / outYBlockSize) + outYBlockSize;
            }
            searchWindowOverlap = filterWindowSize*2;
            if(searchWindowOverlap < outYBlockSize)
            {
                searchWindowOverlap = outYBlockSize;
            }
            
			// Allocate memory
            numPxlsInSearchWindow = searchWindowWidth*searchWindowHeight;
            inputDataSearchWindow = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
                inputDataSearchWindow[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInSearchWindow);
                for(int k = 0; k < numPxlsInSearchWindow; k++)
                {
                    inputDataSearchWindow[i][k] = 0;
                }
			}
			
			outputData = new float*[numOutBands];
			for(int i = 0; i < numOutBands; i++)
			{
				outputData[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInSearchWindow);
			}
			
            // Set up GSL vector for data to be filtered.
            gsl_vector *inDataVector =  gsl_vector_alloc(filterWindowSize*filterWindowSize);
            gsl_vector *inSearchBlockVector =  gsl_vector_alloc(filterWindowSize*filterWindowSize);
            
            //gsl_matrix *gaussKernel = gsl_matrix_alloc(filterWindowSize*filterWindowSize,filterWindowSize*filterWindowSize);
            
            swPixelValues = new double[numPxlsInSearchWindow];
            swPixelWeights = new double[numPxlsInSearchWindow];
            
            // Initialise top corner at common overlap.
            unsigned int startSearchX = bandOffsets[0][0];
            unsigned int startSearchY = bandOffsets[0][1];
            
            unsigned int maxSearchX = width - searchWindowWidth;
            unsigned int maxSearchY = height - searchWindowHeight;
            unsigned int searchWinOffsetX = 0; // Offset for start pixel in search window
            //unsigned int searchWinOffsetY = 0; // Offset for end pixel in search window
            
            std::cout << "height = " << height << ", searchWindowHeight = " << searchWindowHeight << std::endl;
            std::cout << "width = " << width << ", searchWindowWidth = " << searchWindowWidth << std::endl;
            
            //unsigned int feedback = height*width / 10.0;
			//int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			
            while (startSearchY < maxSearchY)
            {
                std::cout << "startSearchY = " << startSearchY << ", maxSearchY = " << maxSearchY << std::endl;
                
                startSearchX = 0; // Reset X
                std::cout << "startSearchX = " << startSearchX << ", maxSearchX = " << maxSearchX << std::endl;
                while (startSearchX < maxSearchX)
                {
                    
                    // Read search window into memory
                    for(int n = 0; n < numOutBands; n++)
                    {
                        inputRasterBands[n]->RasterIO(GF_Read, startSearchX, startSearchY, searchWindowWidth, searchWindowHeight, inputDataSearchWindow[n], searchWindowWidth, searchWindowHeight, GDT_Float32, 0, 0);
                    }
                    
                    // Loop through pixels in search window
                    unsigned int cPxl = 0;
                    for(unsigned int i = 0; i < searchWindowHeight; ++i)
                    {
                        searchWinOffsetX = 0; // Reset x offset
                        for(unsigned int j = 0; j < searchWindowWidth; ++j)
                        {
                            // Get data block
                            int startDataBlockX = j - filterWinPix;
                            int startDataBlockY = i - filterWinPix;
                            int endDataBlockX = j + filterWinPix;
                            int endDataBlockY = i + filterWinPix;
                            
                            if (startDataBlockX < 0)
                            {
                                startDataBlockX = 0;
                                endDataBlockX = filterWindowSize;
                            }
                            if (startDataBlockY < 0)
                            {
                                startDataBlockY = 0;
                                endDataBlockY = filterWindowSize;
                            }
                            if (endDataBlockX >= searchWindowWidth)
                            {
                                startDataBlockX = searchWindowWidth-filterWinPix;
                                endDataBlockX = searchWindowWidth;
                            }
                            if (endDataBlockY >= searchWindowHeight)
                            {
                                startDataBlockY = searchWindowHeight-filterWinPix;
                                endDataBlockY = searchWindowHeight;
                            }
                            
                            for(int n = 0; n < numOutBands; n++)
                            {
                                
                                // Get vector for data block
                                unsigned int pixVecNum = 0;
                                for(unsigned int y = startDataBlockY;y < endDataBlockY; ++y)
                                {
                                    for(unsigned int x = startDataBlockX; x < endDataBlockX; ++x)
                                    {
                                        double pixVal = inputDataSearchWindow[n][(y*searchWindowWidth)+x];
                                        gsl_vector_set(inDataVector, pixVecNum, pixVal);
                                        ++pixVecNum;
                                    }
                                }
                                
                                // Calculate pixel values and weights for all blocks in search window
                                unsigned int swValNum = 0;
                                for(unsigned int searchWinY = 0; searchWinY < searchWindowHeight; ++searchWinY)
                                {
                                    for(unsigned int searchWinX = 0; searchWinX < searchWindowWidth; ++searchWinX)
                                    {
                                        // Get data block
                                        int startSearchBlockX = j - filterWinPix;
                                        int startSearchBlockY = i - filterWinPix;
                                        int endSearchBlockX = j + filterWinPix;
                                        int endSearchBlockY = i + filterWinPix;
                                        
                                        if (startSearchBlockX < 0)
                                        {
                                            startSearchBlockX = 0;
                                            endSearchBlockX = filterWindowSize;
                                        }
                                        if (startSearchBlockY < 0)
                                        {
                                            startSearchBlockY = 0;
                                            endSearchBlockY = filterWindowSize;
                                        }
                                        if (endSearchBlockX >= searchWindowWidth)
                                        {
                                            startSearchBlockX = searchWindowWidth-filterWinPix;
                                            endSearchBlockX = searchWindowWidth;
                                        }
                                        if (endSearchBlockY >= searchWindowHeight)
                                        {
                                            startSearchBlockY = searchWindowHeight-filterWinPix;
                                            endSearchBlockY = searchWindowHeight;
                                        }
                                                                                    
                                        // Add to GSL vector
                                        unsigned int pixVecNum = 0;
                                        for(unsigned int y = startSearchBlockY;y < endSearchBlockY; ++y)
                                        {
                                            for(unsigned int x = startSearchBlockX; x < endSearchBlockX; ++x)
                                            {
                                                double pixVal = inputDataSearchWindow[n][(y*searchWindowWidth)+x];
                                                double pixelDiff = pixVal - gsl_vector_get(inDataVector, pixVecNum);
                                                gsl_vector_set(inSearchBlockVector, pixVecNum, pixelDiff);
                                                
                                                if((x == windowMid) && (y == windowMid))
                                                {
                                                    // Add centre pixel value to vector.
                                                    swPixelValues[swValNum] = pixVal;
                                                }
                                                
                                                ++pixVecNum;
                                            }
                                        }
                                        
                                        // Calculate weights
                                        // v(Ni) . Gauss . v(Nj)
                                        double inBlockSearchBlockGDiff = vectorUtils.dotProductVectorVectorGSL(inSearchBlockVector, inSearchBlockVector);
                                        double swPixWeight = exp(invHParSq * inBlockSearchBlockGDiff);
                                        
                                        swPixelWeights[swValNum] = swPixWeight;
                                        ++swValNum;

                                    }
                                }
                                
                                // Estimate denoised pixel values
                                
                                // Calculate sum of weights
                                double sumWeights = 0;
                                for(unsigned int w=0; w < numPxlsInSearchWindow; ++w)
                                {
                                    sumWeights+=swPixelWeights[w];
                                }
                                
                                // Calculate weighted mean
                                double denoisedPixVal = 0;
                                double weightPixVal = 0;
                                
                                for(unsigned int w=0; w < numPxlsInSearchWindow; ++w)
                                {
                                    weightPixVal = (1. / sumWeights) * swPixelWeights[w] * swPixelValues[w];
                                    denoisedPixVal+= weightPixVal;
                                }
 
                                // Save filterd pixel value to output column
                                outputData[n][cPxl] = denoisedPixVal;
                                
                            }
                            if(cPxl % 100 == 0){std::cout << cPxl << std::endl;}
                            ++cPxl;
                        }
                    }
                    
                    for(int n = 0; n < numOutBands; n++)
                    {
                        outputRasterBands[n]->RasterIO(GF_Write, startSearchX, startSearchY, searchWindowWidth, searchWindowHeight, outputData[n], searchWindowWidth, searchWindowHeight, GDT_Float32, 0, 0);
                    }
                    startSearchX+=searchWindowWidth;
                }
                startSearchY+=searchWindowWidth;
            }

            std::cout << " Complete.\n";
		}
		catch(rsgis::img::RSGISImageCalcException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete dsOffsets[i];
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete bandOffsets[i];
				}
				delete[] bandOffsets;
			}
            
            if(inputDataSearchWindow != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataSearchWindow[i];
				}
				delete[] inputDataSearchWindow;
			}
			
			if(outputData != NULL)
			{
				for(int i = 0; i < numOutBands; i++)
				{
					delete outputData[i];
				}
				delete outputData;
			}
            
			throw e;
		}
		catch(rsgis::img::RSGISImageBandException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete dsOffsets[i];
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete bandOffsets[i];
				}
				delete[] bandOffsets;
			}
            
            if(inputDataSearchWindow != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataSearchWindow[i];
				}
				delete[] inputDataSearchWindow;
			}
			
			if(outputData != NULL)
			{
				for(int i = 0; i < numOutBands; i++)
				{
					delete outputData[i];
				}
				delete outputData;
			}
			
			throw e;
		}
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete dsOffsets[i];
			}
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				delete bandOffsets[i];
			}
			delete[] bandOffsets;
		}
        
        if(inputDataSearchWindow != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataSearchWindow[i];
            }
            delete[] inputDataSearchWindow;
        }
        
		if(outputData != NULL)
		{
			for(int i = 0; i < numOutBands; i++)
			{
				delete outputData[i];
			}
			delete outputData;
		}
		
	}
	
	
	RSGISApplyNonLocalDenoising::~RSGISApplyNonLocalDenoising()
	{
		
	}
    
    
}}
