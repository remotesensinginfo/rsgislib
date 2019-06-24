/*
 *  RSGISWarpImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/09/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#include "RSGISWarpImage.h"

namespace rsgis{namespace reg{
	
	RSGISWarpImage::RSGISWarpImage(std::string inputImage, std::string outputImage, std::string outProjWKT, std::string gcpFilePath, float outImgRes, RSGISWarpImageInterpolator *interpolator, std::string gdalFormat):inputImage(""), outputImage(""), outProjWKT(""), gcpFilePath(""), outImgRes(0), interpolator(NULL), gdalFormat("ENVI")
	{
		this->inputImage = inputImage;
		this->outputImage = outputImage;
		this->outProjWKT = outProjWKT;
		this->gcpFilePath = gcpFilePath;
		this->outImgRes = outImgRes;
		this->interpolator = interpolator;
        this->gdalFormat = gdalFormat;
		gcps = new std::vector<RSGISGCPImg2MapNode*>();
	}
	
	void RSGISWarpImage::performWarp()
	{
		try 
		{
			std::cout << "Read in GCPs\n";
			this->readGCPFile();
			std::cout << "Initialise Warp\n";
			this->initWarp();
			std::cout << "Create blank output image\n";
			this->createOutputImage();
			std::cout << "Assign Output Pixel values\n";
			this->populateOutputImage();
		}
		catch (RSGISImageWarpException &e) 
		{
			throw e;
		}
	}
    
    void RSGISWarpImage::generateTransformImage()
	{
		try 
		{
			std::cout << "Read in GCPs\n";
			this->readGCPFile();
			std::cout << "Initialise Warp\n";
			this->initWarp();
			std::cout << "Create blank output image\n";
			this->createOutputTransformImage();
			std::cout << "Assign Output Pixel values\n";
			this->populateTransformImage();
		}
		catch (RSGISImageWarpException &e) 
		{
			throw e;
		}
	}
	
	void RSGISWarpImage::readGCPFile()
	{
        rsgis::utils::RSGISTextUtils textUtils;
        rsgis::math::RSGISMathsUtils mathUtils;
		
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
	}
	
	void RSGISWarpImage::createOutputImage()
	{
        rsgis::img::RSGISImageUtils imgUtils;

		GDALDataset *inputImageDS = NULL;
        geos::geom::Envelope *imageGeoExtent = NULL;
		
		try 
		{			
			inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            			
            imageGeoExtent = this->newImageExtent(inputImageDS->GetRasterXSize()-1, inputImageDS->GetRasterYSize()-1);
            
			if(inputImageDS == NULL)
			{
				std::string message = std::string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
			
			unsigned int numOutBands = inputImageDS->GetRasterCount();
			
			GDALDataset* outputImageDS = imgUtils.createBlankImage(this->outputImage, *imageGeoExtent, this->outImgRes, numOutBands, outProjWKT, 0, this->gdalFormat);
				
			GDALClose(inputImageDS);
			GDALClose(outputImageDS);
		}
		catch (RSGISImageWarpException &e) 
		{
			throw e;
		}
		catch (rsgis::img::RSGISImageBandException &e) 
		{
			throw RSGISImageWarpException(e.what());
		}
		catch (RSGISImageException &e) 
		{
			throw RSGISImageWarpException(e.what());
		} 
	}
    
    void RSGISWarpImage::createOutputTransformImage()
	{
        rsgis::img::RSGISImageUtils imgUtils;
        
		GDALDataset *inputImageDS = NULL;
        geos::geom::Envelope *imageGeoExtent = NULL;
		
		try 
		{			
			inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            
            imageGeoExtent = this->newImageExtent(inputImageDS->GetRasterXSize()-1, inputImageDS->GetRasterYSize()-1);
            
			if(inputImageDS == NULL)
			{
				std::string message = std::string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
						
			GDALDataset* outputImageDS = imgUtils.createBlankImage(this->outputImage, *imageGeoExtent, this->outImgRes, 2, outProjWKT, 0, this->gdalFormat);
            
			GDALClose(inputImageDS);
			GDALClose(outputImageDS);
		}
		catch (RSGISImageWarpException &e) 
		{
			throw e;
		}
		catch (rsgis::img::RSGISImageBandException &e) 
		{
			throw RSGISImageWarpException(e.what());
		}
		catch (RSGISImageException &e) 
		{
			throw RSGISImageWarpException(e.what());
		} 
	}
	
	void RSGISWarpImage::populateOutputImage()
	{
        rsgis::img::RSGISImageUtils imgUtils;
		
		GDALDataset *inputImageDS = NULL;
		GDALDataset *outputImageDS = NULL;

		
		try 
		{
			inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(inputImageDS == NULL)
			{
				std::string message = std::string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
			outputImageDS = (GDALDataset *) GDALOpen(this->outputImage.c_str(), GA_Update);
			if(outputImageDS == NULL)
			{
				std::string message = std::string("Could not open image ") + this->outputImage;
				throw RSGISImageException(message.c_str());
			}
			
			if(inputImageDS->GetRasterCount() != outputImageDS->GetRasterCount())
			{
				throw RSGISImageWarpException("The number input and output image bands are not equal.");
			}
			
			unsigned int numBands = outputImageDS->GetRasterCount();
			
			double *gdalTransformation= new double[6];
			inputImageDS->GetGeoTransform(gdalTransformation);
			
			//double inTLX = gdalTransformation[0];
			//double inTLY = gdalTransformation[3];
			//double inBRX = inTLX + (inputImageDS->GetRasterXSize() * gdalTransformation[1]);
			//double inBRY = inTLY + (inputImageDS->GetRasterYSize() * gdalTransformation[5]);
			float inImgRes = gdalTransformation[1];
			
			outputImageDS->GetGeoTransform(gdalTransformation);
            
			double outTLX = gdalTransformation[0];
			double outTLY = gdalTransformation[3];
			//double outBRX = outTLX + (outputImageDS->GetRasterXSize() * gdalTransformation[1]);
			//double outBRY = outTLY + (outputImageDS->GetRasterYSize() * gdalTransformation[5]);

			delete[] gdalTransformation;
            
            unsigned int inWidth = inputImageDS->GetRasterXSize();
			unsigned int inHeight = inputImageDS->GetRasterYSize();
			unsigned int outWidth = outputImageDS->GetRasterXSize();
			unsigned int outHeight = outputImageDS->GetRasterYSize();
			
			//Get Image Output Bands
			GDALRasterBand **outputRasterBands = new GDALRasterBand*[numBands];
			for(unsigned int i = 0; i < numBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
			}
			
			// Allocate memory			
			float **outputData = new float*[numBands];
			for(unsigned int i = 0; i < numBands; i++)
			{
				outputData[i] = (float *) CPLMalloc(sizeof(float)*outWidth);
			}
			float *outDataColumn = new float[numBands];
			
			double startEastings = outTLX - (this->outImgRes+(this->outImgRes/2));
			double startNorthings = outTLY + (this->outImgRes+(this->outImgRes/2));
			double currentEastings = startEastings;
			double currentNorthings = startNorthings;
			
			unsigned int xPxl = 0;
			unsigned int yPxl = 0;
			
			int feedback = outHeight/10;
			int feedbackCounter = 0;
			std::cout << "Started ." << std::flush;

			for(unsigned int i = 0; i < outHeight; ++i)
			{
				if((outHeight > 10) && ((i % feedback) == 0))
				{
					std::cout << "." << feedbackCounter << "." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				currentEastings = startEastings;
				for(unsigned int j = 0; j < outWidth; ++j)
				{
					try 
					{
						this->findNearestPixel(currentEastings, currentNorthings, &xPxl, &yPxl, inImgRes);
                        
                        // Check xPxl and yPxl are within input image.
						if((xPxl < inWidth) && (yPxl < inHeight))
                        {
                            this->interpolator->calcValue(inputImageDS, outDataColumn, numBands, currentEastings, currentNorthings, xPxl, yPxl, inImgRes, outImgRes);
                        }
                        else
                        {
                            for(unsigned int n = 0; n < numBands; n++)
                            {
                                outDataColumn[n] = 0;
                            }
                        }
                        
					}
					catch (RSGISImageWarpException) 
					{
						// ignore... - set output as NaN
						for(unsigned int n = 0; n < numBands; n++)
						{
							outDataColumn[n] = std::numeric_limits<double>::signaling_NaN();//NAN;
						}
					}
					
					for(unsigned int n = 0; n < numBands; n++)
					{
						outputData[n][j] = outDataColumn[n];
					}
					
					currentEastings += this->outImgRes;
				}
				
				for(unsigned int n = 0; n < numBands; n++)
				{
					outputRasterBands[n]->RasterIO(GF_Write, 0, i, outWidth, 1, outputData[n], outWidth, 1, GDT_Float32, 0, 0);
				}
				
				currentNorthings -= this->outImgRes;
			}
			std::cout << ". Complete\n";
			
			delete[] outputRasterBands;
			
			for(unsigned int i = 0; i < numBands; i++)
			{
				CPLFree(outputData[i]);
			}
			delete[] outputData;
			delete[] outDataColumn;
			
			GDALClose(inputImageDS);
			GDALClose(outputImageDS);
		}
		catch (RSGISImageWarpException &e) 
		{
			GDALClose(inputImageDS);
			GDALClose(outputImageDS);
			throw e;
		}
		catch (rsgis::img::RSGISImageBandException &e) 
		{
			GDALClose(inputImageDS);
			GDALClose(outputImageDS);
			throw RSGISImageWarpException(e.what());
		}
		catch (RSGISImageException &e) 
		{
			GDALClose(inputImageDS);
			GDALClose(outputImageDS);
			throw RSGISImageWarpException(e.what());
		} 
	}
    
    void RSGISWarpImage::populateTransformImage()
	{
		rsgis::img::RSGISImageUtils imgUtils;
		
		GDALDataset *inputImageDS = NULL;
		GDALDataset *outputImageDS = NULL;
		
		try 
		{
			inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(inputImageDS == NULL)
			{
				std::string message = std::string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
			outputImageDS = (GDALDataset *) GDALOpen(this->outputImage.c_str(), GA_Update);
			if(outputImageDS == NULL)
			{
				std::string message = std::string("Could not open image ") + this->outputImage;
				throw RSGISImageException(message.c_str());
			}

			unsigned int numBands = outputImageDS->GetRasterCount();
			
			double *gdalTransformation= new double[6];
			inputImageDS->GetGeoTransform(gdalTransformation);
			
			double inTLX = gdalTransformation[0];
			double inTLY = gdalTransformation[3];
			//double inBRX = inTLX + (inputImageDS->GetRasterXSize() * gdalTransformation[1]);
			//double inBRY = inTLY + (inputImageDS->GetRasterYSize() * gdalTransformation[5]);
			float inImgRes = gdalTransformation[1];
			
			outputImageDS->GetGeoTransform(gdalTransformation);
			
			double outTLX = gdalTransformation[0];
			double outTLY = gdalTransformation[3];
			//double outBRX = outTLX + (outputImageDS->GetRasterXSize() * gdalTransformation[1]);
			//double outBRY = outTLY + (outputImageDS->GetRasterYSize() * gdalTransformation[5]);
            
			delete[] gdalTransformation;
			/*
             if(outTLX < inTLX)
             {
             throw RSGISImageWarpException("Output image is outside input image, TLX");
             }
             
             if(outBRX > inBRX)
             {
             throw RSGISImageWarpException("Output image is outside input image, BRX");
             }
             
             if(outTLY > inTLY)
             {
             throw RSGISImageWarpException("Output image is outside input image, TLY");
             }
             
             if(outBRY < inBRY)
             {
             throw RSGISImageWarpException("Output image is outside input image, BRY");
             }
             */
			unsigned int outWidth = outputImageDS->GetRasterXSize();
			unsigned int outHeight = outputImageDS->GetRasterYSize();
			
			//Get Image Output Bands
			GDALRasterBand **outputRasterBands = new GDALRasterBand*[numBands];
			for(unsigned int i = 0; i < numBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
			}
			
			// Allocate memory			
			float **outputData = new float*[numBands];
			for(unsigned int i = 0; i < numBands; i++)
			{
				outputData[i] = (float *) CPLMalloc(sizeof(float)*outWidth);
			}
			float *outDataColumn = new float[numBands];
			
			double startEastings = outTLX - (this->outImgRes+(this->outImgRes/2));
			double startNorthings = outTLY + (this->outImgRes+(this->outImgRes/2));
			double currentEastings = startEastings;
			double currentNorthings = startNorthings;
            double pxlEastings = 0;
			double pxlNorthings = 0;
			
			unsigned int xPxl = 0;
			unsigned int yPxl = 0;
			
			int feedback = outHeight/10;
			int feedbackCounter = 0;
			std::cout << "Started ." << std::flush;
			for(unsigned int i = 0; i < outHeight; ++i)
			{
				if((outHeight > 10) && ((i % feedback) == 0))
				{
					std::cout << "." << feedbackCounter << "." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				currentEastings = startEastings;
				for(unsigned int j = 0; j < outWidth; ++j)
				{
					try 
					{
						this->findNearestPixel(currentEastings, currentNorthings, &xPxl, &yPxl, inImgRes);
                        
                        pxlEastings = inTLX + (xPxl*inImgRes);
                        pxlNorthings = inTLY - (yPxl*inImgRes);
                        
						outDataColumn[0] = (currentEastings - pxlEastings)/this->outImgRes;
                        outDataColumn[1] = (currentNorthings - pxlNorthings)/this->outImgRes;
					}
					catch (RSGISImageWarpException) 
					{
						// ignore... - set output as NaN
						for(unsigned int n = 0; n < numBands; n++)
						{
							outDataColumn[n] = std::numeric_limits<double>::signaling_NaN();//NAN;
						}
					}
					
					for(unsigned int n = 0; n < numBands; n++)
					{
						outputData[n][j] = outDataColumn[n];
					}
					
					currentEastings += this->outImgRes;
				}
				
				for(unsigned int n = 0; n < numBands; n++)
				{
					outputRasterBands[n]->RasterIO(GF_Write, 0, i, outWidth, 1, outputData[n], outWidth, 1, GDT_Float32, 0, 0);
				}
				
				currentNorthings -= this->outImgRes;
			}
			std::cout << ". Complete\n";
			
			delete[] outputRasterBands;
			
			for(unsigned int i = 0; i < numBands; i++)
			{
				CPLFree(outputData[i]);
			}
			delete[] outputData;
			delete[] outDataColumn;
			
			GDALClose(inputImageDS);
			GDALClose(outputImageDS);
		}
		catch (RSGISImageWarpException &e) 
		{
			GDALClose(inputImageDS);
			GDALClose(outputImageDS);
			throw e;
		}
		catch (rsgis::img::RSGISImageBandException &e) 
		{
			GDALClose(inputImageDS);
			GDALClose(outputImageDS);
			throw RSGISImageWarpException(e.what());
		}
		catch (RSGISImageException &e) 
		{
			GDALClose(inputImageDS);
			GDALClose(outputImageDS);
			throw RSGISImageWarpException(e.what());
		} 
	}
	
	RSGISWarpImage::~RSGISWarpImage()
	{
		if(gcps != NULL)
		{
			delete gcps;
		}
		
	}
	
}}






