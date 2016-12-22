/*
 *  RSGISAddGCPsGDAL.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 06/06/2013.
 *  Copyright 2013 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 * 
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISAddGCPsGDAL.h"

namespace rsgis{namespace reg{

    RSGISAddGCPsGDAL::RSGISAddGCPsGDAL(std::string inFileName, std::string gcpFilePath, std::string outFileName, std::string gdalFormat, GDALDataType gdalDataType)
    {
        std::string gcpImage = "";
        GDALAllRegister();
        
        // Check if new dataset should be created
        if (outFileName != "")
        {
            this->copyImageWithoutSpatialRef(inFileName, outFileName, gdalFormat, gdalDataType);
            
            gcpImage = outFileName;
        }
        else
        {
            std::cout << "Updating existing image." << std::endl;
            gcpImage = inFileName;
        }
        
        // Open GCP image (new or created) for update.
        GDALDataset *gcpDataset = NULL;
        gcpDataset = (GDALDataset *) GDALOpen(gcpImage.c_str(), GA_Update);
    
        // Read GCPs to vector
        std::vector<RSGISGCPImg2MapNode*> *gcps = new std::vector<RSGISGCPImg2MapNode*>();
        this->readGCPFile(gcpFilePath, gcps);
        
        // Convert to GDAL format
        int gcpCount = gcps->size();
        GDAL_GCP *gdalGCPList = new GDAL_GCP[gcps->size()];
        this->convertRSGIS2GDALGCP(gcps, gdalGCPList);
        
        gcpDataset->SetGCPs(gcpCount, gdalGCPList, gcpDataset->GetProjectionRef());
        
        std::cout << "Added " << gcpCount << " GCPs to file" << std::endl;

        
        // Tidy up
        delete[] gdalGCPList;
        if(gcps != NULL)
		{
			delete gcps;
		}
        GDALClose((GDALDatasetH) gcpDataset);

    }
    
    void RSGISAddGCPsGDAL::readGCPFile(std::string gcpFilePath, std::vector<RSGISGCPImg2MapNode*> *gcps) throw(RSGISImageWarpException)
	{
        rsgis::utils::RSGISTextUtils textUtils;
        rsgis::math::RSGISMathsUtils mathUtils;
        
        // Read in GCP file to GCP format
		std::ifstream gcpFile;
		gcpFile.open(gcpFilePath.c_str());
		if(gcpFile.is_open())
		{
			std::vector<std::string> *tokens = new std::vector<std::string>();
			std::string strLine;
			RSGISGCPImg2MapNode *gcp = NULL;
			while(!gcpFile.eof())
			{
				getline(gcpFile, strLine, '\n');
				if((!textUtils.lineStart(strLine, '#')) & (!textUtils.blankline(strLine)))
				{
					textUtils.tokenizeString(strLine, ',', tokens, true);
					
                    // Accept 4 or 5 tokens to take new format GCPs with metric value
                    // without breaking backwards compatibility.
					if((tokens->size() != 4) && (tokens->size() != 5))
					{
						delete tokens;
						gcpFile.close();
						std::string message = "Line should have 4 tokens: \"" + strLine + "\"";
						throw RSGISImageWarpException(message);
					}
					
					gcp = new RSGISGCPImg2MapNode(mathUtils.strtodouble(tokens->at(0)),
												  mathUtils.strtodouble(tokens->at(1)),
												  mathUtils.strtofloat(tokens->at(2)),
												  mathUtils.strtofloat(tokens->at(3)));
					gcps->push_back(gcp);
					
					tokens->clear();
				}
			}
			delete tokens;
			gcpFile.close();
		}
		else
		{
			std::string message = "Could not open " + gcpFilePath;
			throw RSGISImageWarpException(message);
		}
        
        std::cout << "Read in " << gcps->size() << " GCPs OK" << std::endl;
        
	}
    
    void RSGISAddGCPsGDAL::convertRSGIS2GDALGCP(std::vector<RSGISGCPImg2MapNode*> *gcps, GDAL_GCP *gdalGCPList) throw(RSGISImageWarpException)
    {
        
        unsigned int gcpNum=0;
        std::string gcpInfo = "";
        
        std::vector<RSGISGCPImg2MapNode*>::iterator iterGCPs;
        
		for(iterGCPs = gcps->begin(); iterGCPs != gcps->end(); ++iterGCPs)
		{
            gdalGCPList[gcpNum].pszId = new char[64];
            gdalGCPList[gcpNum].pszInfo = new char[64];
            gdalGCPList[gcpNum].dfGCPPixel = (*iterGCPs)->imgX()-1;  // Pixel X
            gdalGCPList[gcpNum].dfGCPLine = (*iterGCPs)->imgY()-1;   // Pixel Y
            gdalGCPList[gcpNum].dfGCPX = (*iterGCPs)->eastings();  // Easting
            gdalGCPList[gcpNum].dfGCPY = (*iterGCPs)->northings(); // Northing
            gdalGCPList[gcpNum].dfGCPZ = 0;                        // Elevation
            ++gcpNum;
        }
        
    }
    
    void RSGISAddGCPsGDAL::copyImageWithoutSpatialRef(std::string inFileName, std::string outFileName, std::string gdalFormat, GDALDataType gdalDataType) throw(RSGISImageWarpException)
    {
        
        GDALAllRegister();

		int height = 0;
		int width = 0;
		int numInBands = 0;

		float **inputData = NULL;
		
		GDALDataset *inputImageDS = NULL;
        GDALDataset *outputImageDS = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		GDALDriver *gdalDriver = NULL;
        
        try
		{
            inputImageDS = (GDALDataset *) GDALOpen(inFileName.c_str(), GA_ReadOnly);
            numInBands = inputImageDS->GetRasterCount();
            width = inputImageDS->GetRasterXSize();
            height = inputImageDS->GetRasterYSize();
            
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(gdalDriver == NULL)
			{
				throw RSGISImageWarpException("Requested GDAL driver does not exists..");
			}
			
			outputImageDS = gdalDriver->Create(outFileName.c_str(), width, height,numInBands, gdalDataType, NULL);
			
			if(outputImageDS == NULL)
			{
				throw RSGISImageWarpException("Output image could not be created. Check filepath.");
			}
            
            // Set projection (but not geotransform)
            outputImageDS->SetProjection(inputImageDS->GetProjectionRef());
            
            // Get input bands
            inputRasterBands = new GDALRasterBand*[numInBands];
            
            for(int i = 0; i < numInBands; i++)
			{
				inputRasterBands[i] = inputImageDS->GetRasterBand(i+1);
            }
            
			//Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[numInBands];
            
            for(int i = 0; i < numInBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
            }

            int xBlockSize = 0;
            int yBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&xBlockSize, &yBlockSize);
                        
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*(width*yBlockSize));
			}
            
            int nYBlocks = floor(((double)height) / ((double)yBlockSize));
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
			int feedback = height/10.0;
			int feedbackCounter = 0;
            bool provideFeedback = false;
            
            // Only provide feedback if more than 10 blocks
            if(nYBlocks > 10){provideFeedback = true;}
            
			if(provideFeedback){std::cout << "Started" << std::flush;}
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
                if((provideFeedback) && (feedback != 0) && (i*yBlockSize > feedback*feedbackCounter) )
                {
                    std::cout << "." << feedbackCounter*10 << "." << std::flush;
                    ++feedbackCounter;
                }
				for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = yBlockSize * i;
					inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, width, yBlockSize, inputData[n], width, yBlockSize, GDT_Float32, 0, 0);
                    outputRasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, yBlockSize, inputData[n], width, yBlockSize, GDT_Float32, 0, 0);
				}
                
			}
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = yBlockSize * nYBlocks;
					inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, width, remainRows, inputData[n], width, remainRows, GDT_Float32, 0, 0);
                    outputRasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, remainRows, inputData[n], width, remainRows, GDT_Float32, 0, 0);
				}
                
            }
			if(provideFeedback){std::cout << " Complete.\n";}
		}
		catch(RSGISImageWarpException& e)
		{
			if(inputData != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(inputData[i] != NULL)
					{
						delete[] inputData[i];
					}
				}
				delete[] inputData;
			}

			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		
		GDALClose(outputImageDS);
		
		if(inputData != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(inputData[i] != NULL)
				{
					CPLFree(inputData[i]);
				}
			}
			delete[] inputData;
		}

		if(inputRasterBands != NULL)
		{
			delete[] inputRasterBands;
		}
		
		if(outputRasterBands != NULL)
		{
			delete[] outputRasterBands;
		}
        
        
    }

}}






