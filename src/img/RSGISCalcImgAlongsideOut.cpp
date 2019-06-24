/*
 *  RSGISCalcImgAlongsideOut.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/12/2011.
 *  Copyright 2011 RSGISLib.
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

#include "img/RSGISCalcImgAlongsideOut.h"

namespace rsgis{namespace img{

    RSGISCalcImgAlongsideOut::RSGISCalcImgAlongsideOut(RSGISCalcImgValueAlongsideOut *valueCalc)
    {
        this->calc = valueCalc;
    }
        
    void RSGISCalcImgAlongsideOut::calcImageIterate(GDALDataset *dataset)
    {
		unsigned long height = 0;
		unsigned long width = 0;
		unsigned int numBands = 0;
        int idx = 0;
		
		unsigned int *dataColumn = NULL;
        unsigned int ***calcRows = NULL;
        unsigned int *leftPxl = NULL;
        unsigned int *rightPxl = NULL;
        unsigned int **rowAbove = NULL;
        unsigned int **rowBelow = NULL;
		
		GDALRasterBand **rasterBands = NULL;
		
		try
		{    
            width = dataset->GetRasterXSize();
            height = dataset->GetRasterYSize();
            numBands = dataset->GetRasterCount();
            
			// Get Image Bands and Allocate memory
            dataColumn = new unsigned int[numBands];
            leftPxl = new unsigned int[numBands];
            rightPxl = new unsigned int[numBands];
            rowAbove = new unsigned int*[numBands];
            rowBelow = new unsigned int*[numBands];
			rasterBands = new GDALRasterBand*[numBands];
            calcRows = new unsigned int**[numBands];
            
			for(unsigned int n = 0; n < numBands; ++n)
			{
				rasterBands[n] = dataset->GetRasterBand(n+1);
                calcRows[n] = new unsigned int*[3];
                for(int j = 0; j < 3; j++)
                {
                    calcRows[n][j] = new unsigned int[width];
                }
                rowAbove[n] = new unsigned int[3];
                rowBelow[n] = new unsigned int[3];
			}
            
			unsigned int feedback = height/10;
			unsigned int feedbackCounter = 0;
			bool change = true;
            unsigned int iterCount = 0;
            unsigned long numChanges = 0;
            
            std::cout << "Started Iteration " << iterCount << " " << std::flush;
            
            for(unsigned int n = 0; n < numBands; n++)
            {
                for(unsigned long x = 0; x < width; ++x)
                {
                    calcRows[n][0][x] = 0;
                    calcRows[n][1][x] = 0;
                    calcRows[n][2][x] = 0;
                }
            }
            // Loop image to process data
            for(unsigned long y = 0; y < height; ++y)
            {                    
                if((y % feedback) == 0)
                {
                    std::cout << ".." << feedbackCounter << ".." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                
                for(unsigned int n = 0; n < numBands; n++)
                {
                    rasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, calcRows[n][1], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned long x = 0; x < width; x++)
                    {
                        calcRows[n][2][x] = 0;
                    }
                }
                
                for(unsigned long x = 1; x < width-1; ++x)
                {
                    for(unsigned int n = 0; n < numBands; ++n)
                    {
                        dataColumn[n] = calcRows[n][1][x];
                        leftPxl[n] = calcRows[n][1][x-1];
                        rightPxl[n] = 0;
                        
                        idx = 0;
                        for(unsigned int m = x-1; m < x+1; ++m)
                        {
                            rowAbove[n][idx] = calcRows[n][0][m];
                            rowBelow[n][idx] = calcRows[n][2][m];
                            ++idx;
                        }
                    }
                    
                    if(this->calc->calcValue(true, numBands, dataColumn, rowAbove, rowBelow, leftPxl, rightPxl))
                    {
                        change = true;
                        ++numChanges;
                    }
                    
                    for(unsigned int n = 0; n < numBands; ++n)
                    {
                        calcRows[n][1][x] = dataColumn[n];
                    }                        
                }
                
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    rasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, calcRows[n][1], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned long x = 0; x < width; ++x)
                    {
                        calcRows[n][0][x] = calcRows[n][1][x];
                    }
                }
            }
            std::cout << " Complete. = " << numChanges << " value changes." << std::endl;
            
            numChanges = 0;
            ++iterCount;
            change = true;
            while(change)
            {
                change = false;
                feedbackCounter = 0;
                std::cout << "Started Iteration " << iterCount << " " << std::flush;
                
                for(unsigned int n = 0; n < numBands; n++)
                {
                    for(unsigned long j = 0; j < width; j++)
                    {
                        calcRows[n][0][j] = 0;
                        calcRows[n][1][j] = 0;
                        calcRows[n][2][j] = 0;
                    }
                }
                // Loop images to process data
                for(unsigned long y = 0; y < height; ++y)
                {                    
                    if((y % feedback) == 0)
                    {
                        std::cout << ".." << feedbackCounter << ".." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    if(y == height-1)
                    {
                        for(unsigned int n = 0; n < numBands; n++)
                        {
                            rasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, calcRows[n][1], width, 1, GDT_UInt32, 0, 0);
                            for(unsigned long x = 0; x < width; x++)
                            {
                                calcRows[n][2][x] = 0;
                            }
                        }
                    }
                    else
                    {
                        for(unsigned int n = 0; n < numBands; n++)
                        {
                            rasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, calcRows[n][1], width, 1, GDT_UInt32, 0, 0);
                            rasterBands[n]->RasterIO(GF_Read, 0, y+1, width, 1, calcRows[n][2], width, 1, GDT_UInt32, 0, 0);
                        }
                    }
                    
                    
                    
                    for(unsigned long x = 1; x < width-1; ++x)
                    {
                        for(unsigned int n = 0; n < numBands; ++n)
                        {
                            dataColumn[n] = calcRows[n][1][x];
                            leftPxl[n] = calcRows[n][1][x-1];
                            rightPxl[n] = calcRows[n][1][x+1];
                            
                            idx = 0;
                            for(unsigned int m = x-1; m < x+1; ++m)
                            {
                                rowAbove[n][idx] = calcRows[n][0][m];
                                rowBelow[n][idx] = calcRows[n][2][m];
                                ++idx;
                            }
                        }
                        
                                //int numBands, float *dataCol, float **rowAbove, float **rowBelow, float *left, float *right
                        if(this->calc->calcValue(false, numBands, dataColumn, rowAbove, rowBelow, leftPxl, rightPxl))
                        {
                            change = true;
                            ++numChanges;
                        }
                        
                        for(unsigned int n = 0; n < numBands; ++n)
                        {
                            calcRows[n][1][x] = dataColumn[n];
                        }                        
                    }
                    
                    for(unsigned int n = 0; n < numBands; ++n)
                    {
                        rasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, calcRows[n][1], width, 1, GDT_UInt32, 0, 0);
                        for(unsigned long x = 0; x < width; ++x)
                        {
                            calcRows[n][0][x] = calcRows[n][1][x];
                        }
                    }
                }
                std::cout << " Complete. = " << numChanges << " value changes." << std::endl;
  
                ++iterCount;
                numChanges = 0;
            }
		}
		catch(RSGISImageCalcException& e)
		{			
            if(calcRows != NULL)
			{            
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    for(int j = 0; j < 3; j++)
                    {
                        delete[] calcRows[n][j];
                    }
                    delete[] calcRows[n];
                }
                delete[] calcRows;
            }
            
            if(rowAbove != NULL)
			{
				for(unsigned int i = 0; i < numBands; i++)
				{
					if(rowAbove[i] != NULL)
					{
						delete[] rowAbove[i];
					}
				}
				delete[] rowAbove;
			}
            
            if(rowBelow != NULL)
			{
				for(unsigned int i = 0; i < numBands; i++)
				{
					if(rowBelow[i] != NULL)
					{
						delete[] rowBelow[i];
					}
				}
				delete[] rowBelow;
			}
            
			if(leftPxl != NULL)
			{
				delete[] leftPxl;
			}
            
            if(rightPxl != NULL)
			{
				delete[] rightPxl;
			}
            
			if(dataColumn != NULL)
			{
				delete[] dataColumn;
			}
			
			if(rasterBands != NULL)
			{
				delete[] rasterBands;
			}
            
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			if(calcRows != NULL)
			{            
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    for(int j = 0; j < 3; j++)
                    {
                        delete[] calcRows[n][j];
                    }
                    delete[] calcRows[n];
                }
                delete[] calcRows;
            }
            
            if(rowAbove != NULL)
			{
				for(unsigned int i = 0; i < numBands; i++)
				{
					if(rowAbove[i] != NULL)
					{
						delete[] rowAbove[i];
					}
				}
				delete[] rowAbove;
			}
            
            if(rowBelow != NULL)
			{
				for(unsigned int i = 0; i < numBands; i++)
				{
					if(rowBelow[i] != NULL)
					{
						delete[] rowBelow[i];
					}
				}
				delete[] rowBelow;
			}
            
			if(leftPxl != NULL)
			{
				delete[] leftPxl;
			}
            
            if(rightPxl != NULL)
			{
				delete[] rightPxl;
			}
            
			if(dataColumn != NULL)
			{
				delete[] dataColumn;
			}
			
			if(rasterBands != NULL)
			{
				delete[] rasterBands;
			}
			throw e;
		}
		
		if(calcRows != NULL)
        {            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                for(int j = 0; j < 3; j++)
                {
                    delete[] calcRows[n][j];
                }
                delete[] calcRows[n];
            }
            delete[] calcRows;
        }
        
        if(rowAbove != NULL)
        {
            for(unsigned int i = 0; i < numBands; i++)
            {
                if(rowAbove[i] != NULL)
                {
                    delete[] rowAbove[i];
                }
            }
            delete[] rowAbove;
        }
        
        if(rowBelow != NULL)
        {
            for(unsigned int i = 0; i < numBands; i++)
            {
                if(rowBelow[i] != NULL)
                {
                    delete[] rowBelow[i];
                }
            }
            delete[] rowBelow;
        }
        
        if(leftPxl != NULL)
        {
            delete[] leftPxl;
        }
        
        if(rightPxl != NULL)
        {
            delete[] rightPxl;
        }
        
        if(dataColumn != NULL)
        {
            delete[] dataColumn;
        }
        
        if(rasterBands != NULL)
        {
            delete[] rasterBands;
        }
    }

}}

