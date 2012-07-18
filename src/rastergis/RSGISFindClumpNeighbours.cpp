/*
 *  RSGISFindClumpNeighbours.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/03/2012.
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

#include "RSGISFindClumpNeighbours.h"

namespace rsgis{namespace rastergis{
  
    RSGISFindClumpNeighbours::RSGISFindClumpNeighbours()
    {
        
    }
    
    std::vector<std::list<size_t>* >* RSGISFindClumpNeighbours::findNeighbours(GDALDataset *clumpImage) throw(rsgis::img::RSGISImageCalcException)
    {
        std::vector<std::list<size_t>* > *neighbours = new std::vector<std::list<size_t>* >();
        
        try
        {
            
            unsigned int width = clumpImage->GetRasterXSize();
            unsigned int height = clumpImage->GetRasterYSize();
            GDALRasterBand *imgBand = clumpImage->GetRasterBand(1);
            
            unsigned int *clumpIdxs = new unsigned int[width];
            unsigned long maxClumpIdx = 0;
            
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
                imgBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
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
            std::cout << " Complete.\n";
            delete[] clumpIdxs;
            
            std::cout << "Number of clumps = " << maxClumpIdx << std::endl;
            
            neighbours->reserve(maxClumpIdx);
            
            for(unsigned long i = 0; i < maxClumpIdx; ++i)
            {
                neighbours->push_back(new std::list<unsigned long>());
            }
            
            int windowSize = 3;
            
            unsigned int **inputData = new unsigned int*[3];
			for(int i = 0; i < windowSize; i++)
            {
                inputData[i] = new unsigned int[width];
                for(int j = 0; j < width; j++)
                {
                    inputData[i][j] = 0;
                }
            }
            
            unsigned int **dataBlock = new unsigned int*[windowSize];
            for(int i = 0; i < windowSize; i++)
            {
                dataBlock[i] = new unsigned int[windowSize];
            }
            
            unsigned long clumpID = 0;
            
			feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{				
				if((i % feedback) == 0)
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				                
				for(int m = 0; m < windowSize; m++)
				{
					if(m == 0)
					{
						if(i == 0)
						{
							for(int k = 0; k < width; k++)
                            {
                                inputData[m][k] = 0;
                            }
						}
						else
						{
							imgBand->RasterIO(GF_Read, 0, i-1, width, 1, inputData[m], width, 1, GDT_UInt32, 0, 0);
						}
					}
					else if(m == 2)
					{
						if((i + 1) >= height)
						{
							for(int k = 0; k < width; k++)
                            {
                                inputData[m][k] = 0;
                            }
						}
						else
						{
							imgBand->RasterIO(GF_Read, 0, i+1, width, 1, inputData[m], width, 1, GDT_UInt32, 0, 0);
						}
					}
					else
					{
						imgBand->RasterIO(GF_Read, 0, i, width, 1, inputData[m], width, 1, GDT_UInt32, 0, 0);
					}
				}
				
				for(int j = 0; j < width; j++)
				{
					for(int m = 0; m < windowSize; m++)
					{
						for(int k = 0; k < windowSize; k++)
						{
                            
							if(k == 0)
							{
								if(j == 0)
								{
									dataBlock[m][k] = 0;
								}
								else
								{
									dataBlock[m][k] = inputData[m][(j-1)];
								}
							}
							else if(k == 2)
							{
								if((j + 1) >= width)
								{
									dataBlock[m][k] = 0;
								}
								else
								{
                                    dataBlock[m][k] = inputData[m][(j+1)];
								}
							}
							else
							{
								dataBlock[m][k] = inputData[m][j];
							}
						}
					}
					
					
                    // Process Window.
                    clumpID = dataBlock[1][1];
                    if(clumpID > 0)
                    {
                        if((dataBlock[0][1] > 0) & (dataBlock[0][1] != clumpID))
                        {
                            neighbours->at(clumpID-1)->push_back(dataBlock[0][1]-1);
                        }
                        if((dataBlock[1][0] > 0) & (dataBlock[1][0] != clumpID))
                        {
                            neighbours->at(clumpID-1)->push_back(dataBlock[1][0]-1);
                        }
                        if((dataBlock[1][2] > 0) & (dataBlock[1][2] != clumpID))
                        {
                            neighbours->at(clumpID-1)->push_back(dataBlock[1][2]-1);
                        }
                        if((dataBlock[2][1] > 0) & (dataBlock[2][1] != clumpID))
                        {
                            neighbours->at(clumpID-1)->push_back(dataBlock[2][1]-1);
                        }
                    }
				}
			}
			std::cout << " Complete.\n";
            
			for(int i = 0; i < windowSize; i++)
            {
                delete[] dataBlock[i];
                delete[] inputData[i];
            }
            delete[] dataBlock;
            delete[] inputData;
            
            for(unsigned long i = 0; i < maxClumpIdx; ++i)
            {
                neighbours->at(i)->sort();
                neighbours->at(i)->unique();
            }
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        
        return neighbours;
    }
    
    void RSGISFindClumpNeighbours::findNeighbours(GDALDataset *clumpImage, RSGISAttributeTable *attTable) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {            
            unsigned int width = clumpImage->GetRasterXSize();
            unsigned int height = clumpImage->GetRasterYSize();
            GDALRasterBand *imgBand = clumpImage->GetRasterBand(1);
            
            int windowSize = 3;
            
            unsigned int **inputData = new unsigned int*[3];
			for(int i = 0; i < windowSize; i++)
            {
                inputData[i] = new unsigned int[width];
                for(int j = 0; j < width; j++)
                {
                    inputData[i][j] = 0;
                }
            }
            
            unsigned int **dataBlock = new unsigned int*[windowSize];
            for(int i = 0; i < windowSize; i++)
            {
                dataBlock[i] = new unsigned int[windowSize];
            }
            
            unsigned long clumpID = 0;
            RSGISFeature *feat = NULL;
            
            int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{				
				if((i % feedback) == 0)
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
                
				for(int m = 0; m < windowSize; m++)
				{
					if(m == 0)
					{
						if(i == 0)
						{
							for(int k = 0; k < width; k++)
                            {
                                inputData[m][k] = 0;
                            }
						}
						else
						{
							imgBand->RasterIO(GF_Read, 0, i-1, width, 1, inputData[m], width, 1, GDT_UInt32, 0, 0);
						}
					}
					else if(m == 2)
					{
						if((i + 1) >= height)
						{
							for(int k = 0; k < width; k++)
                            {
                                inputData[m][k] = 0;
                            }
						}
						else
						{
							imgBand->RasterIO(GF_Read, 0, i+1, width, 1, inputData[m], width, 1, GDT_UInt32, 0, 0);
						}
					}
					else
					{
						imgBand->RasterIO(GF_Read, 0, i, width, 1, inputData[m], width, 1, GDT_UInt32, 0, 0);
					}
				}
				
				for(int j = 0; j < width; j++)
				{
					for(int m = 0; m < windowSize; m++)
					{
						for(int k = 0; k < windowSize; k++)
						{
                            
							if(k == 0)
							{
								if(j == 0)
								{
									dataBlock[m][k] = 0;
								}
								else
								{
									dataBlock[m][k] = inputData[m][(j-1)];
								}
							}
							else if(k == 2)
							{
								if((j + 1) >= width)
								{
									dataBlock[m][k] = 0;
								}
								else
								{
                                    dataBlock[m][k] = inputData[m][(j+1)];
								}
							}
							else
							{
								dataBlock[m][k] = inputData[m][j];
							}
						}
					}
					
					
                    // Process Window.
                    clumpID = dataBlock[1][1];
                    if(clumpID > 0)
                    {
                        feat = attTable->getFeature(clumpID-1);
                        if((dataBlock[0][1] > 0) & (dataBlock[0][1] != clumpID))
                        {
                            this->addNeighbourToFeature(feat, dataBlock[0][1]-1);
                        }
                        if((dataBlock[1][0] > 0) & (dataBlock[1][0] != clumpID))
                        {
                            this->addNeighbourToFeature(feat, dataBlock[1][0]-1);
                        }
                        if((dataBlock[1][2] > 0) & (dataBlock[1][2] != clumpID))
                        {
                            this->addNeighbourToFeature(feat, dataBlock[1][2]-1);
                        }
                        if((dataBlock[2][1] > 0) & (dataBlock[2][1] != clumpID))
                        {
                            this->addNeighbourToFeature(feat, dataBlock[2][1]-1);
                        }
                    }
				}
			}
			std::cout << " Complete.\n";
            
			for(int i = 0; i < windowSize; i++)
            {
                delete[] dataBlock[i];
                delete[] inputData[i];
            }
            delete[] dataBlock;
            delete[] inputData;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
    }
    
    void RSGISFindClumpNeighbours::findNeighboursInBlocks(GDALDataset *clumpImage, RSGISAttributeTable *attTable) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {            
            unsigned int width = clumpImage->GetRasterXSize();
            unsigned int height = clumpImage->GetRasterYSize();
            GDALRasterBand *imgBand = clumpImage->GetRasterBand(1);
            
            int windowSize = 3;
            
            unsigned int **inputData = new unsigned int*[3];
			for(int i = 0; i < windowSize; i++)
            {
                inputData[i] = new unsigned int[width];
                for(int j = 0; j < width; j++)
                {
                    inputData[i][j] = 0;
                }
            }
            
            unsigned int **dataBlock = new unsigned int*[windowSize];
            for(int i = 0; i < windowSize; i++)
            {
                dataBlock[i] = new unsigned int[windowSize];
            }
            
            unsigned long clumpID = 0;
            RSGISFeature *feat = NULL;
            
            int feedback = height/10;
			int feedbackCounter = 0;
            
            size_t totalNumBlocks = (attTable->getSize()/ATT_WRITE_CHUNK_SIZE)+1;
            size_t numBlocksInSample = attTable->getNumOfBlocks()*0.9;
            size_t numSamples = (totalNumBlocks/numBlocksInSample)+1;
            size_t startBlock = 0;
            size_t endBlock = numBlocksInSample;
            
            size_t numFeatInSample = ATT_WRITE_CHUNK_SIZE * numBlocksInSample;
            size_t startFID = 0;
            size_t endFID = numFeatInSample;
            
            std::cout << "The neighbours will be processed in " << numSamples << " samples." << std::endl;
            
            for(size_t n = 0; n < numSamples; ++n)
            {
                attTable->loadBlocks(startBlock, endBlock);
                
                feedback = height/10;
                feedbackCounter = 0;
                std::cout << "Started (" << n+1 << " of " << numSamples << ")" << std::flush;
                // Loop images to process data
                for(int i = 0; i < height; i++)
                {				
                    if((i % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(int m = 0; m < windowSize; m++)
                    {
                        if(m == 0)
                        {
                            if(i == 0)
                            {
                                for(int k = 0; k < width; k++)
                                {
                                    inputData[m][k] = 0;
                                }
                            }
                            else
                            {
                                imgBand->RasterIO(GF_Read, 0, i-1, width, 1, inputData[m], width, 1, GDT_UInt32, 0, 0);
                            }
                        }
                        else if(m == 2)
                        {
                            if((i + 1) >= height)
                            {
                                for(int k = 0; k < width; k++)
                                {
                                    inputData[m][k] = 0;
                                }
                            }
                            else
                            {
                                imgBand->RasterIO(GF_Read, 0, i+1, width, 1, inputData[m], width, 1, GDT_UInt32, 0, 0);
                            }
                        }
                        else
                        {
                            imgBand->RasterIO(GF_Read, 0, i, width, 1, inputData[m], width, 1, GDT_UInt32, 0, 0);
                        }
                    }
                    
                    for(int j = 0; j < width; j++)
                    {
                        // Process Window.
                        clumpID = inputData[1][j];
                        if((clumpID > 0) & ((clumpID >= startFID) & (clumpID <= endFID)))
                        {
                            for(int m = 0; m < windowSize; m++)
                            {
                                for(int k = 0; k < windowSize; k++)
                                {
                                    
                                    if(k == 0)
                                    {
                                        if(j == 0)
                                        {
                                            dataBlock[m][k] = 0;
                                        }
                                        else
                                        {
                                            dataBlock[m][k] = inputData[m][(j-1)];
                                        }
                                    }
                                    else if(k == 2)
                                    {
                                        if((j + 1) >= width)
                                        {
                                            dataBlock[m][k] = 0;
                                        }
                                        else
                                        {
                                            dataBlock[m][k] = inputData[m][(j+1)];
                                        }
                                    }
                                    else
                                    {
                                        dataBlock[m][k] = inputData[m][j];
                                    }
                                }
                            }
                            
                            feat = attTable->getFeature(clumpID-1);
                            if((dataBlock[0][1] > 0) & (dataBlock[0][1] != clumpID))
                            {
                                this->addNeighbourToFeature(feat, dataBlock[0][1]-1);
                            }
                            if((dataBlock[1][0] > 0) & (dataBlock[1][0] != clumpID))
                            {
                                this->addNeighbourToFeature(feat, dataBlock[1][0]-1);
                            }
                            if((dataBlock[1][2] > 0) & (dataBlock[1][2] != clumpID))
                            {
                                this->addNeighbourToFeature(feat, dataBlock[1][2]-1);
                            }
                            if((dataBlock[2][1] > 0) & (dataBlock[2][1] != clumpID))
                            {
                                this->addNeighbourToFeature(feat, dataBlock[2][1]-1);
                            }
                        }
                    }
                }
                std::cout << " Complete.\n";
                
                attTable->flushAllFeatures(true);
                startBlock += numBlocksInSample;
                endBlock += numBlocksInSample;
                startFID += numFeatInSample;
                endFID += numFeatInSample;
            }
            
			for(int i = 0; i < windowSize; i++)
            {
                delete[] dataBlock[i];
                delete[] inputData[i];
            }
            delete[] dataBlock;
            delete[] inputData;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
    }
        
    RSGISFindClumpNeighbours::~RSGISFindClumpNeighbours()
    {
        
    }
    
}}
