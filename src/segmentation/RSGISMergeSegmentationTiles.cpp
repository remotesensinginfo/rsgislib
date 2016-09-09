/*
 *  RSGISMergeSegmentationTiles.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/04/2013.
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

#include "RSGISMergeSegmentationTiles.h"

namespace rsgis{namespace segment{
    
    RSGISMergeSegmentationTiles::RSGISMergeSegmentationTiles()
    {
        
    }
    
    void RSGISMergeSegmentationTiles::createTileBorderClumpMask(GDALDataset *borderMaskDataset, std::vector<std::string> inputImagePaths, unsigned int tileBoundary, unsigned int tileOverlap, unsigned int tileBody, std::string colsName) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            GDALRasterAttributeTable *attTable = NULL;
            size_t numRows = 0;
            double maxVal = 0;
            
            for(std::vector<std::string>::iterator iterFiles = inputImagePaths.begin(); iterFiles != inputImagePaths.end(); ++iterFiles)
            {
                std::cout << "\t Opening - " << (*iterFiles) << std::endl;
                GDALDataset *inImage = (GDALDataset *) GDALOpen((*iterFiles).c_str(), GA_Update);
                if(inImage == NULL)
                {
                    std::string message = std::string("Could not open image ") + (*iterFiles);
                    throw rsgis::RSGISImageException(message.c_str());
                }
                
                attTable = inImage->GetRasterBand(1)->GetDefaultRAT();
                
                if(attTable == NULL)
                {
                    throw RSGISImageException("Input image does not have an attribute table.");
                }
                
                numRows = attTable->GetRowCount();
                
                inImage->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
                
                if(maxVal > numRows)
                {
                    throw RSGISImageException("Number of rows and maximum image pixel value does not match.");
                }
                                
                this->addTileBorder2Mask(inImage, borderMaskDataset, attTable, colsName, tileBoundary);
                
                GDALClose(inImage);
            }
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISImageException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    void RSGISMergeSegmentationTiles::mergeClumpBodies(GDALDataset *outputDataset, GDALDataset *borderMaskDataset, std::vector<std::string> inputImagePaths, unsigned int tileBoundary, unsigned int tileOverlap, unsigned int tileBody, std::string colsName) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            GDALRasterAttributeTable *attTable = NULL;
            size_t numRows = 0;
            double maxVal = 0;
            size_t clumpsOffset = 0;
            size_t numClumps = 0;
                           
            for(std::vector<std::string>::iterator iterFiles = inputImagePaths.begin(); iterFiles != inputImagePaths.end(); ++iterFiles)
            {
                std::cout << "\t Opening - " << (*iterFiles) << std::endl;
                GDALDataset *inImage = (GDALDataset *) GDALOpen((*iterFiles).c_str(), GA_Update);
                if(inImage == NULL)
                {
                    std::string message = std::string("Could not open image ") + (*iterFiles);
                    throw rsgis::RSGISImageException(message.c_str());
                }
                
                attTable = inImage->GetRasterBand(1)->GetDefaultRAT();
                
                if(attTable == NULL)
                {
                    throw RSGISImageException("Input image does not have an attribute table.");
                }
                
                numRows = attTable->GetRowCount();
                
                inImage->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
                
                if(maxVal > numRows)
                {
                    throw RSGISImageException("Number of rows and maximum image pixel value does not match.");
                }
                
                //std::cout << "Row Count: " << numRows << std::endl;
                
                //std::cout << "Clumps offset: " << clumpsOffset << std::endl;
                
                numClumps = this->numberBodyClumps(attTable, "GlobalClumpID", colsName, tileBody, clumpsOffset);
                
                //std::cout << "Number of body clumps: " << numClumps << std::endl;
                
                clumpsOffset += numClumps;
                                
                this->addTileBodyClumps(outputDataset, inImage, borderMaskDataset, attTable, "GlobalClumpID", colsName, tileBody, tileBoundary);
                
                GDALClose(inImage);
            }
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISImageException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    void RSGISMergeSegmentationTiles::mergeClumpImages(GDALDataset *outputDataset, std::vector<std::string> inputImagePaths, bool mergeRATs) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            GDALRasterAttributeTable *attTable = NULL;
            GDALRasterAttributeTable *outAttTable = NULL;
            size_t numRows = 0;
            double maxVal = 0;
            size_t clumpsOffset = 0;
            size_t imageOffset = 0;
            size_t numClumps = 0;
            bool first = true;
            
            // Get maximum clumpid           
            outputDataset->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
            clumpsOffset = maxVal;
            imageOffset = maxVal;
            outAttTable = outputDataset->GetRasterBand(1)->GetDefaultRAT();
            numRows = outAttTable->GetRowCount();
            if(maxVal > numRows)
            {
                outAttTable->SetRowCount(clumpsOffset);
            }
            std::vector<std::string> outRATCols;
            
            for(std::vector<std::string>::iterator iterFiles = inputImagePaths.begin(); iterFiles != inputImagePaths.end(); ++iterFiles)
            {
                std::cout << "\t Opening - " << (*iterFiles) << std::endl;
                GDALDataset *inImage = (GDALDataset *) GDALOpen((*iterFiles).c_str(), GA_Update);
                if(inImage == NULL)
                {
                    std::string message = std::string("Could not open image ") + (*iterFiles);
                    throw rsgis::RSGISImageException(message.c_str());
                }
                
                attTable = inImage->GetRasterBand(1)->GetDefaultRAT();
                
                if(attTable == NULL)
                {
                    throw RSGISImageException("Input image does not have an attribute table.");
                }
                
                numRows = attTable->GetRowCount();
                
                inImage->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
                
                if(maxVal > numRows)
                {
                    throw RSGISImageException("Number of rows and maximum image pixel value does not match.");
                }
                
                /*
                if(not first)
                {
                    numRows = numRows-1;
                }
                */
                
                //std::cout << "Row Count: " << numRows << std::endl;
                
                //std::cout << "Clumps offset: " << clumpsOffset << std::endl;
                //std::cout << "Image offset: " << imageOffset << std::endl;
                
                numClumps = this->numberClumps(attTable, "GlobalClumpID", imageOffset);
                
                //std::cout << "Number of body clumps: " << numClumps << std::endl;
                
                this->addImageClumps(outputDataset, inImage, attTable, "GlobalClumpID");
                
                if(first)
                {
                    if(mergeRATs)
                    {
                        int idx = 0;
                        std::vector<rsgis::rastergis::RSGISRATCol> *inRATCols = attUtils.getRatColumnsList(attTable);
                        for(std::vector<rsgis::rastergis::RSGISRATCol>::iterator iterCols = inRATCols->begin(); iterCols != inRATCols->end(); ++iterCols)
                        {
                            idx = attUtils.findColumnIndexOrCreate(outAttTable, (*iterCols).name, (*iterCols).type, (*iterCols).usage);
                            outRATCols.push_back((*iterCols).name);
                        }
                        delete inRATCols;
                    }
                    
                    if(mergeRATs)
                    {
                        attUtils.copyAttColumnsWithOff(attTable, outAttTable, outRATCols, clumpsOffset, false, true, true);
                    }
                    
                    clumpsOffset += numClumps;
                    first = false;
                }
                else
                {
                    if(mergeRATs)
                    {
                        attUtils.copyAttColumnsWithOff(attTable, outAttTable, outRATCols, clumpsOffset, true, true, true);
                    }
                    clumpsOffset += (numClumps-1);
                }
                
                imageOffset += (numClumps-1);
                
                GDALClose(inImage);
            }
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISImageException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    size_t RSGISMergeSegmentationTiles::numberBodyClumps(GDALRasterAttributeTable *gdalATT, std::string outColName, std::string clumpPosColName, int tileBody, size_t clumpsOffset) throw(RSGISException)
    {
        size_t numBodyClumps = 0;
        try
        {
            // create new clumpid col
            size_t numRows = gdalATT->GetRowCount();
            int *colVals = new int[numRows];
            //std::cout << "Number of Rows = " << numRows << std::endl;
            
            // read existing clumpid values
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            size_t currentNumRows = 0;
            int *posVals = attUtils.readIntColumn(gdalATT, clumpPosColName, &currentNumRows);
            
            for(size_t i = 0; i < numRows; ++i)
            {
                if(posVals[i] == tileBody)
                {
                    colVals[i] = (int)clumpsOffset++;
                    ++numBodyClumps;
                }
                else
                {
                    colVals[i] = 0;
                }
            }
            
            attUtils.writeIntColumn(gdalATT, outColName, colVals, numRows);
            
        }
        catch (rsgis::RSGISException &e)
        {
            throw e;
        }
        catch (std::exception &e)
        {
            throw rsgis::RSGISException(e.what());
        }
        
        return numBodyClumps;
    }
    
    size_t RSGISMergeSegmentationTiles::numberClumps(GDALRasterAttributeTable *gdalATT, std::string outColName, size_t clumpsOffset) throw(RSGISException)
    {
        rsgis::rastergis::RSGISRasterAttUtils attUtils;
        size_t numClumps = 0;
        try
        {
            size_t numRows = gdalATT->GetRowCount();
            int *colVals = new int[numRows];
            //std::cout << "Number of Rows = " << numRows << std::endl;
                      
            for(size_t i = 0; i < numRows; ++i)
            {
                colVals[i] = (int)clumpsOffset++;
                ++numClumps;
            }
            
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            attUtils.writeIntColumn(gdalATT, outColName, colVals, numRows);
            
        }
        catch (rsgis::RSGISException &e)
        {
            throw e;
        }
        catch (std::exception &e)
        {
            throw rsgis::RSGISException(e.what());
        }
        
        return numClumps;
    }
    
    void RSGISMergeSegmentationTiles::addTileBodyClumps(GDALDataset *outputDataset, GDALDataset *tileDataset, GDALDataset *borderMaskDataset, GDALRasterAttributeTable *gdalATT, std::string outClumpIDColName, std::string clumpPosColName, unsigned int tileBody, unsigned int tileBoundary) throw(rsgis::img::RSGISImageCalcException)
    {
        rsgis::img::RSGISImageUtils imgUtils;
        try
        {
            GDALDataset **datasets = new GDALDataset*[3];
            datasets[0] = tileDataset;
            datasets[1] = outputDataset;
            datasets[2] = borderMaskDataset;
            
            double *gdalTranslation = new double[6];
            int **dsOffsets = new int*[3];
            for(int i = 0; i < 3; i++)
            {
                dsOffsets[i] = new int[2];
            }
            int height = 0;
            int width = 0;
            
            int xBlockSize = 0;
            int yBlockSize = 0;
            
            imgUtils.getImageOverlap(datasets, 3, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
            
            GDALRasterBand *outputBand = outputDataset->GetRasterBand(1);
            GDALRasterBand *clumpsBand = tileDataset->GetRasterBand(1);
            GDALRasterBand *maskBand = borderMaskDataset->GetRasterBand(1);
                        
            int *inOffset = new int[2];
            inOffset[0] = dsOffsets[0][0];
            inOffset[1] = dsOffsets[0][1];
            
            //std::cout << "Input Offset: [" << inOffset[0] << "," << inOffset[1] << "]\n";
            
            int *outOffset = new int[2];
            outOffset[0] = dsOffsets[1][0];
            outOffset[1] = dsOffsets[1][1];
            
            //std::cout << "Output Offset: [" << outOffset[0] << "," << outOffset[1] << "]\n";
            
            int *maskOffset = new int[2];
            maskOffset[0] = dsOffsets[2][0];
            maskOffset[1] = dsOffsets[2][1];
            
            //std::cout << "Mask Offset: [" << maskOffset[0] << "," << maskOffset[1] << "]\n";
            
            unsigned int *imgInData = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
            unsigned int *imgOutData = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
            unsigned int *imgMaskData = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
            
            int nYBlocks = height / yBlockSize;
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
            // read existing column values
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            size_t numRows = 0;
            int *posVals = attUtils.readIntColumn(gdalATT, clumpPosColName, &numRows);
            int *clumpIdVals = attUtils.readIntColumn(gdalATT, outClumpIDColName, &numRows);
            size_t fid = 0;
            //std::cout << "Number of Rows = " << numRows << std::endl;
            
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				//std::cout << i << " of " << nYBlocks << std::endl;
                
				rowOffset = inOffset[1] + (yBlockSize * i);
                clumpsBand->RasterIO(GF_Read, inOffset[0], rowOffset, width, yBlockSize, imgInData, width, yBlockSize, GDT_UInt32, 0, 0);
                rowOffset = outOffset[1] + (yBlockSize * i);
                outputBand->RasterIO(GF_Read, outOffset[0], rowOffset, width, yBlockSize, imgOutData, width, yBlockSize, GDT_UInt32, 0, 0);
                rowOffset = maskOffset[1] + (yBlockSize * i);
                maskBand->RasterIO(GF_Read, maskOffset[0], rowOffset, width, yBlockSize, imgMaskData, width, yBlockSize, GDT_UInt32, 0, 0);
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(int j = 0; j < width; j++)
                    {
                        //std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                        
                        if((imgInData[(m*width)+j] > 0) & (imgInData[(m*width)+j] < numRows))
                        {
                            try
                            {
                                fid = boost::lexical_cast<size_t>(imgInData[(m*width)+j]);
                            }
                            catch(boost::numeric::negative_overflow& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            catch(boost::numeric::positive_overflow& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            catch(boost::numeric::bad_numeric_cast& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            
                            if(posVals[fid] == tileBody)
                            {
                                imgOutData[(m*width)+j] = clumpIdVals[fid];
                                //std::cout << fid << " = " << imgOutData[(m*width)+j] << std::endl;
                            }
                            else if(posVals[fid] == tileBoundary)
                            {
                                imgMaskData[(m*width)+j] = 1;
                            }
                            
                        }
                    }
                    
                }
                
                rowOffset = outOffset[1] + (yBlockSize * i);
                outputBand->RasterIO(GF_Write, outOffset[0], rowOffset, width, yBlockSize, imgOutData, width, yBlockSize, GDT_UInt32, 0, 0);
                rowOffset = maskOffset[1] + (yBlockSize * i);
                maskBand->RasterIO(GF_Write, maskOffset[0], rowOffset, width, yBlockSize, imgMaskData, width, yBlockSize, GDT_UInt32, 0, 0);
            }
            
            if(remainRows > 0)
            {
                rowOffset = inOffset[1] + (yBlockSize * nYBlocks);
                clumpsBand->RasterIO(GF_Read, inOffset[0], rowOffset, width, remainRows, imgInData, width, remainRows, GDT_UInt32, 0, 0);
                rowOffset = outOffset[1] + (yBlockSize * nYBlocks);
                outputBand->RasterIO(GF_Read, outOffset[0], rowOffset, width, remainRows, imgOutData, width, remainRows, GDT_UInt32, 0, 0);
                rowOffset = maskOffset[1] + (yBlockSize * nYBlocks);
                maskBand->RasterIO(GF_Read, maskOffset[0], rowOffset, width, remainRows, imgMaskData, width, remainRows, GDT_UInt32, 0, 0);
                
                for(int m = 0; m < remainRows; ++m)
                {
                    if((feedback != 0) && ((((nYBlocks*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(int j = 0; j < width; j++)
                    {
                        if((imgInData[(m*width)+j] > 0) & (imgInData[(m*width)+j] < numRows))
                        {
                            try
                            {
                                fid = boost::lexical_cast<size_t>(imgInData[(m*width)+j]);
                            }
                            catch(boost::numeric::negative_overflow& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            catch(boost::numeric::positive_overflow& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            catch(boost::numeric::bad_numeric_cast& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            
                            if(posVals[fid] == tileBody)
                            {
                                imgOutData[(m*width)+j] = clumpIdVals[fid];
                                //std::cout << fid << " = " << imgOutData[(m*width)+j] << std::endl;
                            }
                            else if(posVals[fid] == tileBoundary)
                            {
                                imgMaskData[(m*width)+j] = 1;
                                //std::cout << fid << " = " << imgOutData[(m*width)+j] << std::endl;
                            }
                        }
                    }
                }
                
                rowOffset = outOffset[1] + (yBlockSize * nYBlocks);
                outputBand->RasterIO(GF_Write, outOffset[0], rowOffset, width, remainRows, imgOutData, width, remainRows, GDT_UInt32, 0, 0);
                rowOffset = maskOffset[1] + (yBlockSize * nYBlocks);
                maskBand->RasterIO(GF_Write, maskOffset[0], rowOffset, width, remainRows, imgMaskData, width, remainRows, GDT_UInt32, 0, 0);
            }
            std::cout << " Complete.\n";
            
            CPLFree(imgInData);
            CPLFree(imgOutData);
            CPLFree(imgMaskData);
            delete[] outOffset;
            delete[] inOffset;
            delete[] maskOffset;
            delete[] gdalTranslation;
            for(int i = 0; i < 3; i++)
            {
                delete[] dsOffsets[i];
            }
            delete[] dsOffsets;
            delete[] posVals;
            delete[] clumpIdVals;
        }
        catch(rsgis::img::RSGISImageCalcException& e)
        {
            throw e;
        }
        catch(rsgis::img::RSGISImageBandException& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::RSGISImageException& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::RSGISException& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    void RSGISMergeSegmentationTiles::addTileBorder2Mask(GDALDataset *tileDataset, GDALDataset *borderMaskDataset, GDALRasterAttributeTable *gdalATT, std::string clumpPosColName, unsigned int tileBoundary) throw(rsgis::img::RSGISImageCalcException)
    {
        rsgis::img::RSGISImageUtils imgUtils;
        try
        {
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = tileDataset;
            datasets[1] = borderMaskDataset;
            
            double *gdalTranslation = new double[6];
            int **dsOffsets = new int*[2];
            for(int i = 0; i < 2; i++)
            {
                dsOffsets[i] = new int[2];
            }
            int height = 0;
            int width = 0;
            
            int xBlockSize = 0;
            int yBlockSize = 0;
            
            imgUtils.getImageOverlap(datasets, 2, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
            
            GDALRasterBand *clumpsBand = tileDataset->GetRasterBand(1);
            GDALRasterBand *maskBand = borderMaskDataset->GetRasterBand(1);
            
            int *inOffset = new int[2];
            inOffset[0] = dsOffsets[0][0];
            inOffset[1] = dsOffsets[0][1];
            
            //std::cout << "Input Offset: [" << inOffset[0] << "," << inOffset[1] << "]\n";
            
            int *maskOffset = new int[2];
            maskOffset[0] = dsOffsets[1][0];
            maskOffset[1] = dsOffsets[1][1];
            
            //std::cout << "Mask Offset: [" << maskOffset[0] << "," << maskOffset[1] << "]\n";
            
            unsigned int *imgInData = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
            unsigned int *imgMaskData = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
            
            int nYBlocks = height / yBlockSize;
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
            // read existing column values
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            size_t numRows = 0;
            int *posVals = attUtils.readIntColumn(gdalATT, clumpPosColName, &numRows);
            size_t fid = 0;
            //std::cout << "Number of Rows = " << numRows << std::endl;
            
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				//std::cout << i << " of " << nYBlocks << std::endl;
                
				rowOffset = inOffset[1] + (yBlockSize * i);
                clumpsBand->RasterIO(GF_Read, inOffset[0], rowOffset, width, yBlockSize, imgInData, width, yBlockSize, GDT_UInt32, 0, 0);
                rowOffset = maskOffset[1] + (yBlockSize * i);
                maskBand->RasterIO(GF_Read, maskOffset[0], rowOffset, width, yBlockSize, imgMaskData, width, yBlockSize, GDT_UInt32, 0, 0);
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(int j = 0; j < width; j++)
                    {
                        //std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                        
                        if((imgInData[(m*width)+j] > 0) & (imgInData[(m*width)+j] < numRows))
                        {
                            try
                            {
                                fid = boost::lexical_cast<size_t>(imgInData[(m*width)+j]);
                            }
                            catch(boost::numeric::negative_overflow& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            catch(boost::numeric::positive_overflow& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            catch(boost::numeric::bad_numeric_cast& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            
                            if(posVals[fid] == tileBoundary)
                            {
                                imgMaskData[(m*width)+j] = 1;
                            }
                            
                        }
                    }
                    
                }
                
                rowOffset = maskOffset[1] + (yBlockSize * i);
                maskBand->RasterIO(GF_Write, maskOffset[0], rowOffset, width, yBlockSize, imgMaskData, width, yBlockSize, GDT_UInt32, 0, 0);
            }
            
            if(remainRows > 0)
            {
                rowOffset = inOffset[1] + (yBlockSize * nYBlocks);
                clumpsBand->RasterIO(GF_Read, inOffset[0], rowOffset, width, remainRows, imgInData, width, remainRows, GDT_UInt32, 0, 0);
                rowOffset = maskOffset[1] + (yBlockSize * nYBlocks);
                maskBand->RasterIO(GF_Read, maskOffset[0], rowOffset, width, remainRows, imgMaskData, width, remainRows, GDT_UInt32, 0, 0);
                
                for(int m = 0; m < remainRows; ++m)
                {
                    if((feedback != 0) && ((((nYBlocks*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(int j = 0; j < width; j++)
                    {
                        if((imgInData[(m*width)+j] > 0) & (imgInData[(m*width)+j] < numRows))
                        {
                            try
                            {
                                fid = boost::lexical_cast<size_t>(imgInData[(m*width)+j]);
                            }
                            catch(boost::numeric::negative_overflow& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            catch(boost::numeric::positive_overflow& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            catch(boost::numeric::bad_numeric_cast& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            
                            if(posVals[fid] == tileBoundary)
                            {
                                imgMaskData[(m*width)+j] = 1;
                                //std::cout << fid << " = " << imgOutData[(m*width)+j] << std::endl;
                            }
                        }
                    }
                }
                
                rowOffset = maskOffset[1] + (yBlockSize * nYBlocks);
                maskBand->RasterIO(GF_Write, maskOffset[0], rowOffset, width, remainRows, imgMaskData, width, remainRows, GDT_UInt32, 0, 0);
            }
            std::cout << " Complete.\n";
            
            CPLFree(imgInData);
            CPLFree(imgMaskData);
            delete[] inOffset;
            delete[] maskOffset;
            delete[] gdalTranslation;
            for(int i = 0; i < 2; i++)
            {
                delete[] dsOffsets[i];
            }
            delete[] dsOffsets;
            delete[] posVals;
        }
        catch(rsgis::img::RSGISImageCalcException& e)
        {
            throw e;
        }
        catch(rsgis::img::RSGISImageBandException& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::RSGISImageException& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::RSGISException& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    void RSGISMergeSegmentationTiles::addImageClumps(GDALDataset *outputDataset, GDALDataset *clumpsDataset, GDALRasterAttributeTable *gdalATT, std::string outClumpIDColName) throw(rsgis::img::RSGISImageCalcException)
    {
        rsgis::img::RSGISImageUtils imgUtils;
        try
        {
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumpsDataset;
            datasets[1] = outputDataset;
            
            double *gdalTranslation = new double[6];
            int **dsOffsets = new int*[2];
            for(int i = 0; i < 2; i++)
            {
                dsOffsets[i] = new int[2];
            }
            int height = 0;
            int width = 0;
            
            int xBlockSize = 0;
            int yBlockSize = 0;
            
            imgUtils.getImageOverlap(datasets, 2, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
            
            GDALRasterBand *outputBand = outputDataset->GetRasterBand(1);
            GDALRasterBand *clumpsBand = clumpsDataset->GetRasterBand(1);
            
            int *inOffset = new int[2];
            inOffset[0] = dsOffsets[0][0];
            inOffset[1] = dsOffsets[0][1];
            
            //std::cout << "Input Offset: [" << inOffset[0] << "," << inOffset[1] << "]\n";
            
            int *outOffset = new int[2];
            outOffset[0] = dsOffsets[1][0];
            outOffset[1] = dsOffsets[1][1];
            
            //std::cout << "Output Offset: [" << outOffset[0] << "," << outOffset[1] << "]\n";
            
            unsigned int *imgInData = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
            unsigned int *imgOutData = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
            
            int nYBlocks = height / yBlockSize;
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
            // read existing column values
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            size_t numRows = 0;
            int *clumpIdVals = attUtils.readIntColumn(gdalATT, outClumpIDColName, &numRows);
            size_t fid = 0;
            //std::cout << "Number of Rows = " << numRows << std::endl;
                     
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				//std::cout << i << " of " << nYBlocks << std::endl;
                
				rowOffset = inOffset[1] + (yBlockSize * i);
                clumpsBand->RasterIO(GF_Read, inOffset[0], rowOffset, width, yBlockSize, imgInData, width, yBlockSize, GDT_UInt32, 0, 0);
                rowOffset = outOffset[1] + (yBlockSize * i);
                outputBand->RasterIO(GF_Read, outOffset[0], rowOffset, width, yBlockSize, imgOutData, width, yBlockSize, GDT_UInt32, 0, 0);
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(int j = 0; j < width; j++)
                    {
                        //std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                        
                        if((imgInData[(m*width)+j] > 0) & (imgInData[(m*width)+j] < numRows))
                        {
                            try
                            {
                                fid = boost::lexical_cast<size_t>(imgInData[(m*width)+j]);
                            }
                            catch(boost::numeric::negative_overflow& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            catch(boost::numeric::positive_overflow& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            catch(boost::numeric::bad_numeric_cast& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            
                            imgOutData[(m*width)+j] = clumpIdVals[fid];
                            
                        }
                    }
                    
                }
                
                rowOffset = outOffset[1] + (yBlockSize * i);
                outputBand->RasterIO(GF_Write, outOffset[0], rowOffset, width, yBlockSize, imgOutData, width, yBlockSize, GDT_UInt32, 0, 0);
            }
            
            if(remainRows > 0)
            {
                rowOffset = inOffset[1] + (yBlockSize * nYBlocks);
                clumpsBand->RasterIO(GF_Read, inOffset[0], rowOffset, width, remainRows, imgInData, width, remainRows, GDT_UInt32, 0, 0);
                rowOffset = outOffset[1] + (yBlockSize * nYBlocks);
                outputBand->RasterIO(GF_Read, outOffset[0], rowOffset, width, remainRows, imgOutData, width, remainRows, GDT_UInt32, 0, 0);
                
                for(int m = 0; m < remainRows; ++m)
                {
                    if((feedback != 0) && ((((nYBlocks*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(int j = 0; j < width; j++)
                    {
                        if((imgInData[(m*width)+j] > 0) & (imgInData[(m*width)+j] < numRows))
                        {
                            try
                            {
                                fid = boost::lexical_cast<size_t>(imgInData[(m*width)+j]);
                            }
                            catch(boost::numeric::negative_overflow& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            catch(boost::numeric::positive_overflow& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            catch(boost::numeric::bad_numeric_cast& e)
                            {
                                std::cout << "imgInData[" << (m*width)+j << "] = " << imgInData[(m*width)+j] << std::endl;
                                throw rsgis::img::RSGISImageCalcException(e.what());
                            }
                            
                            imgOutData[(m*width)+j] = clumpIdVals[fid];
                        }
                    }
                }
                
                rowOffset = outOffset[1] + (yBlockSize * nYBlocks);
                outputBand->RasterIO(GF_Write, outOffset[0], rowOffset, width, remainRows, imgOutData, width, remainRows, GDT_UInt32, 0, 0);
            }
            std::cout << " Complete.\n";
            
            CPLFree(imgInData);
            CPLFree(imgOutData);
            delete[] outOffset;
            delete[] inOffset;
            delete[] gdalTranslation;
            for(int i = 0; i < 2; i++)
            {
                delete[] dsOffsets[i];
            }
            delete[] dsOffsets;
            delete[] clumpIdVals;
        }
        catch(rsgis::img::RSGISImageCalcException& e)
        {
            throw e;
        }
        catch(rsgis::img::RSGISImageBandException& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::RSGISImageException& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::RSGISException& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    RSGISMergeSegmentationTiles::~RSGISMergeSegmentationTiles()
    {
        
    }
    
    
}}


