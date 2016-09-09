/*
 *  RSGISGenerateImageFromXYZData.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/02/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
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

#include "RSGISGenerateImageFromXYZData.h"

namespace rsgis { namespace img {	
	

	RSGISGenerateImageFromXYZData::RSGISGenerateImageFromXYZData(std::string inputDataFile, std::string outputFile, char delimiter, std::string proj4, bool xyOrder, float resolution)
	{
		this->inputFile = inputDataFile;
		this->outputFile = outputFile;
		this->delimiter = delimiter;
		this->proj4 = proj4;
		this->data = new std::vector<XYZData*>();
		this->bbox = new double[4];
		this->xyOrder = xyOrder;
		this->resolution = resolution;
	}
	
	void RSGISGenerateImageFromXYZData::createImageRepresentingXYZData() throw(rsgis::RSGISFileException, rsgis::RSGISImageException)
	{
		try 
		{
			// Read input data
			this->readInputData();
			
			std::cout << "BBOX: [" << bbox[0] << "," << bbox[1] << "][" << bbox[2] << "," << bbox[3] << "]\n";
			
			// Create output Image
			RSGISImageUtils imgUtils;
			
			double *transformation = new double[6];
			transformation[0] = bbox[0] - (resolution/2);
			transformation[1] = resolution;
			transformation[2] = 0;
			transformation[3] = bbox[3] + (resolution/2);
			transformation[4] = 0;
			transformation[5] = resolution;
			
			unsigned int xSize = ceil((bbox[1] - bbox[0])/resolution)+1;
			unsigned int ySize = ceil((bbox[3] - bbox[2])/resolution)+1;
			
			std::cout << "Size = [" << xSize << ", " << ySize << "]\n";
			
			GDALDataset* outImage = NULL;
			outImage = imgUtils.createBlankImage(outputFile, transformation, xSize, ySize, 2, proj4, 0);
						
			// Populate output Image arrays
			float **zValues = new float*[ySize];
			unsigned int **countValues = new unsigned int*[ySize];
			for(unsigned int i = 0; i < ySize; ++i)
			{
				zValues[i] = new float[xSize];
				countValues[i] = new unsigned int[xSize];
				for(unsigned int j = 0; j < xSize; ++j)
				{
					zValues[i][j] = 0;
					countValues[i][j] = 0;
				}
			}
			
			double TLX = bbox[0] - (resolution/2);
			double TLY = bbox[3] + (resolution/2);
			
			unsigned int xPxl = 0;
			unsigned int yPxl = 0;
			
			std::cout << "TL [" << TLX << ", " << TLY << "]\n";
			
			std::vector<XYZData*>::iterator iterData;
			for(iterData = data->begin(); iterData != data->end(); ++iterData)
			{
				xPxl = floor(((*iterData)->x - TLX) / resolution);
				yPxl = floor((TLY - (*iterData)->y) / resolution);
				zValues[yPxl][xPxl] += (*iterData)->z;
				++countValues[yPxl][xPxl];
			}
			
			// Write data to the output image
			GDALRasterBand *imgBandZValues = outImage->GetRasterBand(1);
			GDALRasterBand *imgBandCount = outImage->GetRasterBand(2);

			float *imgDataZValues = (float *) CPLMalloc(sizeof(float)*xSize);
			float *imgDataCounts = (float *) CPLMalloc(sizeof(float)*xSize);
			
			for(unsigned int i = 0; i < ySize; ++i)
			{
				for(unsigned int j = 0; j < xSize; ++j)
				{
					imgDataZValues[j] = zValues[i][j] / countValues[i][j];
					imgDataCounts[j] = countValues[i][j];
				}
				imgBandZValues->RasterIO(GF_Write, 0, i, xSize, 1, imgDataZValues, xSize, 1, GDT_Float32, 0, 0);
				imgBandCount->RasterIO(GF_Write, 0, i, xSize, 1, imgDataCounts, xSize, 1, GDT_Float32, 0, 0);
			}
			
			delete imgDataZValues;
			delete imgDataCounts;
			
			GDALClose(outImage);
			delete[] transformation;
		}
		catch (rsgis::RSGISFileException &e) 
		{
			throw e;
		}
		catch (RSGISImageBandException &e) 
		{
			throw rsgis::RSGISImageException(e.what());
		}
		catch (rsgis::RSGISImageException &e) 
		{
			throw e;
		}
		catch (RSGISException &e) 
		{
			throw rsgis::RSGISImageException(e.what());
		}
	}
	
	void RSGISGenerateImageFromXYZData::readInputData() throw(rsgis::RSGISFileException)
	{
        rsgis::utils::RSGISTextFileLineReader lineReader;
		rsgis::utils::RSGISTextUtils textUtils;
		
		try 
		{
			bool first = true;
			unsigned long numLines = textUtils.countLines(inputFile);
			data->reserve(numLines);

			std::string line = "";
			std::vector<std::string> *lineTokens = new std::vector<std::string>();
			XYZData *dataItem = NULL;
			lineReader.openFile(inputFile);
			while(!lineReader.endOfFile())
			{
				line = lineReader.readLine();
				if(!textUtils.blankline(line))
				{
					textUtils.tokenizeString(line, delimiter, lineTokens, true);
					if(lineTokens->size() < 3)
					{
						std::cout << "ERROR LINE: " << line << std::endl;
						throw rsgis::RSGISFileException("The line needs to have at least 3 tokens.");
					}
					
					dataItem = new XYZData();
					if(xyOrder)
					{
						dataItem->x = textUtils.strtodouble(lineTokens->at(0));
						dataItem->y = textUtils.strtodouble(lineTokens->at(1));
					}
					else
					{
						dataItem->x = textUtils.strtodouble(lineTokens->at(1));
						dataItem->y = textUtils.strtodouble(lineTokens->at(0));
					}
					dataItem->z = textUtils.strtodouble(lineTokens->at(2));
					data->push_back(dataItem);
					if(first)
					{
						bbox[0] = dataItem->x;
						bbox[1] = dataItem->x;
						bbox[2] = dataItem->y;
						bbox[3] = dataItem->y;
						first = false;
					}
					else
					{
						if(dataItem->x < bbox[0])
						{
							bbox[0] = dataItem->x;
						}
						else if(dataItem->x > bbox[1])
						{
							bbox[1] = dataItem->x;
						}
						
						if(dataItem->y < bbox[2])
						{
							bbox[2] = dataItem->y;
						}
						else if(dataItem->y > bbox[3])
						{
							bbox[3] = dataItem->y;
						}
					}
					lineTokens->clear();
				}
			}
			delete lineTokens;
		}
		catch (rsgis::RSGISFileException &e) 
		{
			throw e;
		}
		catch (RSGISException &e) 
		{
			throw rsgis::RSGISFileException(e.what());
		}
		
	}
	
	RSGISGenerateImageFromXYZData::~RSGISGenerateImageFromXYZData()
	{
		delete[] bbox;
		//delete data;
	}
	
}}

