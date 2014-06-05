/*
 *  RSGISDefiniensCSVRasterise.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/02/2009.
 *  Copyright 2009 RSGISLib.
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

#include "RSGISDefiniensCSVRasterise.h"

namespace rsgis{namespace img{
	
	RSGISDefiniensCSVRasterise::RSGISDefiniensCSVRasterise(bool projFromImage, std::string proj)
	{
		this->imageProj = projFromImage;
		this->proj = proj;
	}
	
	void RSGISDefiniensCSVRasterise::rasteriseTiles(rsgis::datastruct::SortedGenericList<rsgis::utils::RSGISDefiniensWorkspaceFileName> *sortedTIFs, rsgis::datastruct::SortedGenericList<rsgis::utils::RSGISDefiniensWorkspaceFileName> *sortedCSVs, std::string outputDIR) throw(rsgis::RSGISImageException,rsgis::RSGISFileException)
	{
		int size = sortedTIFs->getSize();
		
		bool first = true;
		int numFeatures = 0;
		int numFeaturesFirst = 0;
		int numObjects = 0;
		
		float **csvData = NULL;
		try
		{
			for(int i = size-1; i >= 0; i--)
			{
				if(!sortedCSVs->getAt(i)->getOldVersion())
				{
					// Count the number of objects and features (attributes).
					if(first)
					{
						first = false;
						this->calcNumFeaturesObjects(sortedCSVs->getAt(i), &numFeatures, &numObjects);
						std::cout << "Tile " << sortedCSVs->getAt(i)->getTile() << " has " << numFeatures << " features for " << numObjects << " objects\n";
						numFeaturesFirst = numFeatures;
					}
					else
					{
						this->calcNumFeaturesObjects(sortedCSVs->getAt(i), &numFeatures, &numObjects);
						std::cout << "Tile " << sortedCSVs->getAt(i)->getTile() << " has " << numFeatures << " features for " << numObjects << " objects\n";
						if(numFeatures != numFeaturesFirst)
						{
							std::string message = "File " + sortedCSVs->getAt(i)->getFileNameWithPath() + " has a different number of features (attributes)";
							throw rsgis::RSGISImageException(message.c_str());
						}
					}
					
					// Create data structure to hold CSV file.
					csvData = new float*[numObjects];
					for(int j = 0; j < numObjects; j++)
					{
						csvData[j] = new float[numFeatures];
					}
					
					this->readCSVToMemory(sortedCSVs->getAt(i), csvData, numFeatures, numObjects);

					this->checkTIFCreatePopulateImageTile(sortedTIFs->getAt(i), outputDIR, numFeatures, numObjects, csvData);
					
					// Clear Memory
					for(int j = 0; j < numObjects; j++)
					{
						delete[] csvData[j];
					}
					delete[] csvData;
				}
			}
		}
		catch(rsgis::RSGISImageException e)
		{
			throw e;
		}
		catch (rsgis::RSGISFileException e) 
		{
			throw e;
		}
	}
    
    void RSGISDefiniensCSVRasterise::rasteriseFile(std::string tifFile, std::string csvFile, std::string outputImage) throw(rsgis::RSGISImageException, rsgis::RSGISFileException)
    {
        try
        {
            int numFeatures = 0;
            int numObjects = 0;
            float **csvData = NULL;
            
            this->calcNumFeaturesObjects(csvFile, &numFeatures, &numObjects);
            
            csvData = new float*[numObjects];
            for(int j = 0; j < numObjects; j++)
            {
                csvData[j] = new float[numFeatures];
            }
            
            this->readCSVToMemory(csvFile, csvData, numFeatures, numObjects);
            
            this->checkTIFCreatePopulateImageTile(tifFile, outputImage, numFeatures, numObjects, csvData);
            
            // Clear Memory
            for(int j = 0; j < numObjects; j++)
            {
                delete[] csvData[j];
            }
            delete[] csvData;
            
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISFileException &e)
        {
            throw e;
        }
    }
	
	void RSGISDefiniensCSVRasterise::calcNumFeaturesObjects(rsgis::utils::RSGISDefiniensWorkspaceFileName *csvFilepath, int *numFeatures, int *numObjects) throw(rsgis::RSGISFileException)
	{
		std::ifstream csvFile;
		csvFile.open(csvFilepath->getFileNameWithPath().c_str());
		
		if(csvFile.is_open())
		{
			// Count the number of objects.
			int lineNum = 0;
			std::string firstLine;
			std::string strLine;
			bool first = true;
			
			while(!csvFile.eof())
			{
				if(first)
				{
					getline(csvFile, firstLine, '\n');
					first = false;
				}
				else
				{
					getline(csvFile, strLine, '\n');
				}
				lineNum++;
			}
			
			*numObjects = lineNum-1;
			
			// Count number of features (attributes)
			int lineLength = firstLine.size();
			int semiColonCount = 0;
			char semiColon = ';';
			for(int i = 0; i < lineLength; i++)
			{
				if(firstLine.at(i) == semiColon)
				{
					semiColonCount++;
				}
			}
			*numFeatures = semiColonCount;
			
			// Close File.
			csvFile.close();
		}
		else
		{
			std::string message = std::string("Could not open file: ") + csvFilepath->getFileNameWithPath();
			throw rsgis::RSGISFileException(message.c_str());
		}
	}
    
    void RSGISDefiniensCSVRasterise::calcNumFeaturesObjects(std::string csvInFile, int *numFeatures, int *numObjects) throw(rsgis::RSGISFileException)
	{
		std::ifstream csvFile;
		csvFile.open(csvInFile.c_str());
		
		if(csvFile.is_open())
		{
			// Count the number of objects.
			int lineNum = 0;
			std::string firstLine;
			std::string strLine;
			bool first = true;
			
			while(!csvFile.eof())
			{
				if(first)
				{
					getline(csvFile, firstLine, '\n');
					first = false;
				}
				else
				{
					getline(csvFile, strLine, '\n');
				}
				lineNum++;
			}
			
			*numObjects = lineNum-1;
			
			// Count number of features (attributes)
			int lineLength = firstLine.size();
			int semiColonCount = 0;
			char semiColon = ';';
			for(int i = 0; i < lineLength; i++)
			{
				if(firstLine.at(i) == semiColon)
				{
					semiColonCount++;
				}
			}
			*numFeatures = semiColonCount;
			
			// Close File.
			csvFile.close();
		}
		else
		{
			std::string message = std::string("Could not open file: ") + csvInFile;
			throw rsgis::RSGISFileException(message.c_str());
		}
	}
	
	void RSGISDefiniensCSVRasterise::readCSVToMemory(rsgis::utils::RSGISDefiniensWorkspaceFileName *csvFilepath, float **csvData, int numFeatures, int numObjects) throw(rsgis::RSGISFileException)
	{
		std::ifstream csvFile;
		csvFile.open(csvFilepath->getFileNameWithPath().c_str());
		
		if(csvFile.is_open())
		{
			// Count the number of objects.
			int lineNum = 0;
			int objNum = 0;
			bool first = true;
			std::string strLine;
			
			std::cout.precision(11);
			while(!csvFile.eof())
			{
				getline(csvFile, strLine, '\n');
				if(first)
				{
					//column names ignore!
					first = false;
				}
				else
				{
					this->convertCSVLine(strLine, csvData[objNum], numFeatures, objNum);
					objNum++;
				}
				lineNum++;
			}
			
			// Close File.
			csvFile.close();
		}
		else
		{
			std::string message = std::string("Could not open file: ") + csvFilepath->getFileNameWithPath();
			throw rsgis::RSGISFileException(message.c_str());
		}
	}
    
    void RSGISDefiniensCSVRasterise::readCSVToMemory(std::string csvInFile, float **csvData, int numFeatures, int numObjects) throw(rsgis::RSGISFileException)
	{
		std::ifstream csvFile;
		csvFile.open(csvInFile.c_str());
		
		if(csvFile.is_open())
		{
			// Count the number of objects.
			int lineNum = 0;
			int objNum = 0;
			bool first = true;
			std::string strLine;
			
			//std::cout.precision(11);
			while(!csvFile.eof())
			{
				getline(csvFile, strLine, '\n');
				if(first)
				{
					//column names ignore!
					first = false;
				}
				else
				{
					this->convertCSVLine(strLine, csvData[objNum], numFeatures, objNum);
					objNum++;
				}
				lineNum++;
			}
			
			// Close File.
			csvFile.close();
		}
		else
		{
			std::string message = std::string("Could not open file: ") + csvInFile;
			throw rsgis::RSGISFileException(message.c_str());
		}
	}
	
	void RSGISDefiniensCSVRasterise::convertCSVLine(std::string strLine, float *data, int numFeatures, int objNumber) throw(rsgis::RSGISFileException)
	{
		try
        {
            rsgis::math::RSGISMathsUtils mathsUtils;
            int lineLength = strLine.size();
            char semiColon = ';';
            int featNum = 0;
            int strStart = 0;
            int readObjNum = 0;
            bool first = true;
            for(int i = 0; i < lineLength; i++)
            {
                if(strLine.at(i) == semiColon)
                {
                    if(first)
                    {
                        readObjNum = mathsUtils.strtoint(strLine.substr(strStart, (i-strStart)));
                        if(readObjNum != objNumber)
                        {
                            std::string message = "Object ID read from file (" + mathsUtils.inttostring(readObjNum) + ") does not expected value (" + mathsUtils.inttostring(objNumber) + ")";
                            throw rsgis::RSGISFileException(message.c_str());
                        }
                        first = false;
                    }
                    else
                    {
                        data[featNum] = mathsUtils.strtodouble(strLine.substr(strStart, (i-strStart)));
                        featNum++;
                    }
                    strStart = i+1;
                }
            }
            data[featNum] = mathsUtils.strtofloat(strLine.substr(strStart));
        }
        catch(rsgis::RSGISFileException &e)
        {
            throw e;
        }
        catch(rsgis::math::RSGISMathException &e)
        {
            throw rsgis::RSGISFileException(e.what());
        }
	}
	
	void RSGISDefiniensCSVRasterise::checkTIFCreatePopulateImageTile(rsgis::utils::RSGISDefiniensWorkspaceFileName *tifFilepath, std::string outputDIR, int numFeatures, int numObjects, float **csvData) throw(rsgis::RSGISImageException)
	{
		GDALAllRegister();
		
		RSGISImageStatistics calcImgStats;
		RSGISCalcImage *calcImg = NULL;
		RSGISDefiniensCSVRasteriseValue *calcRasteriseValue;
		
		GDALDataset **dataset = new GDALDataset*[1];
		ImageStats **imageStats = NULL;
		std::string outputImageFilePath = "";
		int inputBands = 0;
		try
		{
			dataset[0] = (GDALDataset *) GDALOpenShared(tifFilepath->getFileNameWithPath().c_str(), GA_ReadOnly);
			if(dataset == NULL)
			{
				std::string message = std::string("Could not open image ") + tifFilepath->getFileNameWithPath();
				throw rsgis::RSGISImageException(message.c_str());
			}
			
			inputBands = dataset[0]->GetRasterCount();
			
			imageStats = new ImageStats*[inputBands];
			for(int i = 0; i < inputBands; i++)
			{
				imageStats[i] = new ImageStats();
			}
			
			calcImgStats.calcImageStatistics(dataset, 1, imageStats, inputBands, false);
			
			std::cout << "Number of objects in image = " << imageStats[0]->max+1 << std::endl;
			
			if(numObjects != (imageStats[0]->max + 1))
			{
				throw rsgis::RSGISImageException("The number of objects in the image and CSV file do not match.");
			}
			
			for(int i = 0; i < inputBands; i++)
			{
				delete imageStats[i];
			}
			delete[] imageStats;
			
			outputImageFilePath = outputDIR + tifFilepath->getFileNameNoExtension() + ".env";
			
			calcRasteriseValue = new RSGISDefiniensCSVRasteriseValue(numFeatures, csvData, numObjects);
			
			calcImg = new RSGISCalcImage(calcRasteriseValue, proj, imageProj);
			calcImg->calcImage(dataset, 1, outputImageFilePath);
			
			delete calcImg;
			delete calcRasteriseValue;
			
			GDALClose(dataset[0]);
			delete[] dataset;
		}
		catch(RSGISImageCalcException e)
		{
			throw rsgis::RSGISImageException(e.what());
		}
		catch(RSGISImageBandException e)
		{
			throw rsgis::RSGISImageException(e.what());
		}
		catch(rsgis::RSGISImageException e)
		{
			throw e;
		}
	}
    
    void RSGISDefiniensCSVRasterise::checkTIFCreatePopulateImageTile(std::string tifFilepath, std::string outputFile, int numFeatures, int numObjects, float **csvData) throw(rsgis::RSGISImageException)
	{
		GDALAllRegister();
		
		RSGISImageStatistics calcImgStats;
		RSGISCalcImage *calcImg = NULL;
		RSGISDefiniensCSVRasteriseValue *calcRasteriseValue;
		
		GDALDataset **dataset = new GDALDataset*[1];
		ImageStats **imageStats = NULL;
		std::string outputImageFilePath = "";
		int inputBands = 0;
		try
		{
			dataset[0] = (GDALDataset *) GDALOpenShared(tifFilepath.c_str(), GA_ReadOnly);
			if(dataset == NULL)
			{
				std::string message = std::string("Could not open image ") + tifFilepath;
				throw rsgis::RSGISImageException(message.c_str());
			}
			
			inputBands = dataset[0]->GetRasterCount();
			
			imageStats = new ImageStats*[inputBands];
			for(int i = 0; i < inputBands; i++)
			{
				imageStats[i] = new ImageStats();
			}
			
			calcImgStats.calcImageStatistics(dataset, 1, imageStats, inputBands, false);
			
			std::cout << "Number of objects in image = " << imageStats[0]->max+1 << std::endl;
			
			if(numObjects != (imageStats[0]->max + 1))
			{
				throw rsgis::RSGISImageException("The number of objects in the image and CSV file do not match.");
			}
			
			for(int i = 0; i < inputBands; i++)
			{
				delete imageStats[i];
			}
			delete[] imageStats;
			
			
			calcRasteriseValue = new RSGISDefiniensCSVRasteriseValue(numFeatures, csvData, numObjects);
			
			calcImg = new RSGISCalcImage(calcRasteriseValue, proj, imageProj);
			calcImg->calcImage(dataset, 1, outputFile);
			
			delete calcImg;
			delete calcRasteriseValue;
			
			GDALClose(dataset[0]);
			delete[] dataset;
		}
		catch(RSGISImageCalcException e)
		{
			throw rsgis::RSGISImageException(e.what());
		}
		catch(RSGISImageBandException e)
		{
			throw rsgis::RSGISImageException(e.what());
		}
		catch(rsgis::RSGISImageException e)
		{
			throw e;
		}
	}
	
	RSGISDefiniensCSVRasterise::~RSGISDefiniensCSVRasterise()
	{
		
	}
	
	
	
	
	
	
	
	
	RSGISDefiniensCSVRasteriseValue::RSGISDefiniensCSVRasteriseValue(int numberOutBands, float **csvData, int numberObjects) : RSGISCalcImageValue(numberOutBands)
	{
		this->csvData = csvData;
		this->numObjects = numberObjects;
	}
	
	void RSGISDefiniensCSVRasteriseValue::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
	{
		if(bandValues[0] < 0 | bandValues[0] >= this->numObjects)
		{
			throw RSGISImageCalcException("Pixel value outside the range of objects");
		}
		
		int objIndex = (int)bandValues[0];
		
		for(int i = 0; i < this->numOutBands; i++)
		{
			output[i] = this->csvData[objIndex][i];
		}
	}
	
	void RSGISDefiniensCSVRasteriseValue::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISDefiniensCSVRasteriseValue::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISDefiniensCSVRasteriseValue::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISDefiniensCSVRasteriseValue::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}

	bool RSGISDefiniensCSVRasteriseValue::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	RSGISDefiniensCSVRasteriseValue::~RSGISDefiniensCSVRasteriseValue()
	{
		
	}
	
	
}}

