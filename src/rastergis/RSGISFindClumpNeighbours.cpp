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
    
    vector<list<unsigned long>* >* RSGISFindClumpNeighbours::findNeighbours(GDALDataset *clumpImage) throw(RSGISImageCalcException)
    {
        vector<list<unsigned long>* > *neighbours = new vector<list<unsigned long>* >();
        
        try
        {
            
            unsigned int width = clumpImage->GetRasterXSize();
            unsigned int height = clumpImage->GetRasterYSize();
            GDALRasterBand *imgBand = clumpImage->GetRasterBand(1);
            
            unsigned int *clumpIdxs = new unsigned int[width];
            unsigned long maxClumpIdx = 0;
            
            int feedback = height/10;
			int feedbackCounter = 0;
			cout << "Started" << flush;
            
            for(unsigned int i = 0; i < height; ++i)
            {
                if((i % feedback) == 0)
				{
					cout << ".." << feedbackCounter << ".." << flush;
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
            cout << " Complete.\n";
            delete[] clumpIdxs;
            
            cout << "Number of clumps = " << maxClumpIdx << endl;
            
            neighbours->reserve(maxClumpIdx);
            
            for(unsigned long i = 0; i < maxClumpIdx; ++i)
            {
                neighbours->push_back(new list<unsigned long>());
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
			cout << "Started" << flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{				
				if((i % feedback) == 0)
				{
					cout << ".." << feedbackCounter << ".." << flush;
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
			cout << " Complete.\n";
            
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
        catch(RSGISImageCalcException &e)
        {
            throw e;
        }
        
        return neighbours;
    }
    
    void RSGISFindClumpNeighbours::findNeighbours(GDALDataset *clumpImage, RSGISAttributeTable *attTable) throw(RSGISImageCalcException)
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
			cout << "Started" << flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{				
				if((i % feedback) == 0)
				{
					cout << ".." << feedbackCounter << ".." << flush;
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
			cout << " Complete.\n";
            
			for(int i = 0; i < windowSize; i++)
            {
                delete[] dataBlock[i];
                delete[] inputData[i];
            }
            delete[] dataBlock;
            delete[] inputData;
        }
        catch(RSGISImageCalcException &e)
        {
            throw e;
        }
    }
        
    RSGISFindClumpNeighbours::~RSGISFindClumpNeighbours()
    {
        
    }
    
}}
