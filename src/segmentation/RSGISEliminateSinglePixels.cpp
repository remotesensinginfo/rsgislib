/*
 *  RSGISEliminateSinglePixels.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/05/2012.
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

#include "RSGISEliminateSinglePixels.h"

namespace rsgis{namespace segment{
    
    RSGISEliminateSinglePixels::RSGISEliminateSinglePixels()
    {
        
    }
    
    void RSGISEliminateSinglePixels::eliminate(GDALDataset *inSpecData, GDALDataset *inClumpsData, GDALDataset *tmpData, std::string outputImage, float noDataVal, bool noDataValProvided, bool projFromImage, std::string proj, std::string format)throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            // Check images have the same size!
            if(inSpecData->GetRasterXSize() != inClumpsData->GetRasterXSize())
            {
                throw rsgis::img::RSGISImageCalcException("Widths are not the same (spectral and categories)");
            }
            if(inSpecData->GetRasterYSize() != inClumpsData->GetRasterYSize())
            {
                throw rsgis::img::RSGISImageCalcException("Heights are not the same (spectral and categories)");
            }
            if(inSpecData->GetRasterXSize() != tmpData->GetRasterXSize())
            {
                throw rsgis::img::RSGISImageCalcException("Widths are not the same (spectral and temp)");
            }
            if(inSpecData->GetRasterYSize() != tmpData->GetRasterYSize())
            {
                throw rsgis::img::RSGISImageCalcException("Heights are not the same (spectral and temp)");
            }
            
            GDALDataset *outData = NULL;
            rsgis::img::RSGISImageUtils imgUtils;
            outData = imgUtils.createCopy(inClumpsData, outputImage, format, GDT_UInt32, projFromImage, proj);
            imgUtils.copyUIntGDALDataset(inClumpsData, outData);
            
            unsigned long singlesCount = 0;
            bool singlesRemoved = false;
            
            while(!singlesRemoved)
            {
                singlesCount = this->findSinglePixels(outData, tmpData, noDataVal, noDataValProvided);
                if(singlesCount > 0)
                {
                    cout << "There are " << singlesCount << " single pixels within the image\n";
                    if(!eliminateSinglePixels(inSpecData, outData, tmpData, outData, noDataVal, noDataValProvided))
                    {
                        singlesRemoved = true;
                        break;
                    }
                }
                else
                {
                    singlesRemoved = true;
                    break;
                }
            }
            cout << "Complete, all connected single pixels have been removed\n";
            
            GDALClose(outData);
            
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(RSGISImageException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    
    unsigned long RSGISEliminateSinglePixels::findSinglePixels(GDALDataset *inClumpsData, GDALDataset *tmpData, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException)
    {
        unsigned long countSingles = 0;
        try 
        {
            unsigned int width = inClumpsData->GetRasterXSize();
            unsigned int height = inClumpsData->GetRasterYSize();
            
            GDALRasterBand *clumpBand = inClumpsData->GetRasterBand(1);
            GDALRasterBand *tmpBand = tmpData->GetRasterBand(1);
            
            unsigned int **inData = new unsigned int*[3];
            inData[0] = new unsigned int[width];
            inData[1] = new unsigned int[width];
            inData[2] = new unsigned int[width];
            unsigned int *outData = new unsigned int[width];
            bool notSingle = false;
            
            int feedback = height/10;
            int feedbackCounter = 0;
            cout << "Started" << flush;
            for(unsigned int i = 0; i < height; ++i)
            {
                if((i % feedback) == 0)
                {
                    cout << "." << feedbackCounter << "." << flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                if(i == 0)
                {
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, inData[2], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        notSingle = false;
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(j == 0)
                            {
                                //right
                                if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                            }
                            else if(j == (width-1))
                            {
                                //left
                                if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                            }
                            else 
                            {
                                //left
                                if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                            }
                        }
                        else
                        {
                            notSingle = true;
                        }
                        
                        if(notSingle)
                        {
                            outData[j] = 0;
                        }
                        else
                        {
                            outData[j] = 1;
                            ++countSingles;
                        }
                    }
                }
                else if(i == (height-1))
                {
                    clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, inData[0], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        notSingle = false;
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(j == 0)
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                            else if(j == (width-1))
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //left
                                else if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                            else 
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //left
                                else if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                        }
                        else
                        {
                            notSingle = true;
                        }
                        
                        if(notSingle)
                        {
                            outData[j] = 0;
                        }
                        else
                        {
                            outData[j] = 1;
                            ++countSingles;
                        }
                    }
                }
                else
                {
                    clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, inData[0], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, inData[2], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        notSingle = false;
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(j == 0)
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                            }
                            else if(j == (width-1))
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //left
                                else if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                            }
                            else 
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //left
                                else if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                            }
                        }
                        else
                        {
                            notSingle = true;
                        }
                        
                        if(notSingle)
                        {
                            outData[j] = 0;
                        }
                        else
                        {
                            outData[j] = 1;
                            ++countSingles;
                        }
                    }
                }
                
                tmpBand->RasterIO(GF_Write, 0, i, width, 1, outData, width, 1, GDT_UInt32, 0, 0);
            }
            cout << ". Complete\n";
            
            delete[] inData[0];
            delete[] inData[1];
            delete[] inData[2];
            delete[] inData;
            delete[] outData;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        
        return countSingles;
    }
    
    bool RSGISEliminateSinglePixels::eliminateSinglePixels(GDALDataset *inSpecData, GDALDataset *inClumpsData, GDALDataset *tmpData, GDALDataset *outDataset, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException)
    {
        bool hasChangeOccured = false;
        try 
        {
            unsigned int width = inClumpsData->GetRasterXSize();
            unsigned int height = inClumpsData->GetRasterYSize();
            unsigned int numBands = inSpecData->GetRasterCount();
            
            GDALRasterBand *clumpBand = inClumpsData->GetRasterBand(1);
            GDALRasterBand *singlePxlBand = tmpData->GetRasterBand(1);
            GDALRasterBand *outBand = outDataset->GetRasterBand(1);
            GDALRasterBand **specBands = new GDALRasterBand*[numBands];
            
            unsigned int **inData = new unsigned int*[3];
            inData[0] = new unsigned int[width];
            inData[1] = new unsigned int[width];
            inData[2] = new unsigned int[width];
            
            unsigned int **singlePxl = new unsigned int*[3];
            singlePxl[0] = new unsigned int[width];
            singlePxl[1] = new unsigned int[width];
            singlePxl[2] = new unsigned int[width];
                        
            float ***specData = new float**[3];
            specData[0] = new float*[numBands];
            specData[1] = new float*[numBands];
            specData[2] = new float*[numBands];
            for(unsigned int i = 0; i < numBands; ++i)
            {
                specData[0][i] = new float[width];
                specData[1][i] = new float[width];
                specData[2][i] = new float[width];
                specBands[i] = inSpecData->GetRasterBand(i+1);
            }
            
            unsigned int *outData = new unsigned int[width];
            bool first = true;
            float minDist = 0;
            float dist = 0;
            unsigned int minDistVal = 0;
            
            int feedback = height/10;
            int feedbackCounter = 0;
            cout << "Started" << flush;
            for(unsigned int i = 0; i < height; ++i)
            {
                if((i % feedback) == 0)
                {
                    cout << "." << feedbackCounter << "." << flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                if(i == 0)
                {
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, inData[2], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i, width, 1, singlePxl[1], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i+1, width, 1, singlePxl[2], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned int n = 0; n < numBands; ++n)
                    {
                        specBands[n]->RasterIO(GF_Read, 0, i, width, 1, specData[1][n], width, 1, GDT_Float32, 0, 0);
                        specBands[n]->RasterIO(GF_Read, 0, i+1, width, 1, specData[2][n], width, 1, GDT_Float32, 0, 0);
                    }
                    
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(singlePxl[1][j] == 1)
                            {
                                if(j == 0)
                                {
                                    first = true;
                                    minDist = 0;
                                    minDistVal = 0;
                                    
                                    //right
                                    if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        dist = this->eucDistance(specData[1], specData[1], numBands, j+1, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j+1];
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j+1];
                                        }
                                    }
                                    //bottom
                                    else if(singlePxl[2][j] == 0)
                                    {
                                        dist = this->eucDistance(specData[2], specData[1], numBands, j, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[2][j];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[2][j];
                                        }
                                    }
                                    
                                    if(first)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = minDistVal;
                                        hasChangeOccured = true;
                                    }
                                }
                                else if(j == (width-1))
                                {
                                    first = true;
                                    minDist = 0;
                                    minDistVal = 0;
                                    
                                    //left
                                    if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        dist = this->eucDistance(specData[1], specData[1], numBands, j-1, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j-1];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j-1];
                                        }
                                    }
                                    //bottom
                                    else if(singlePxl[2][j] == 0)
                                    {
                                        dist = this->eucDistance(specData[2], specData[1], numBands, j, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[2][j];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[2][j];
                                        }
                                    }
                                    
                                    if(first)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = minDistVal;
                                        hasChangeOccured = true;
                                    }
                                }
                                else 
                                {
                                    first = true;
                                    minDist = 0;
                                    minDistVal = 0;
                                    
                                    //left
                                    if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        dist = this->eucDistance(specData[1], specData[1], numBands, j-1, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j-1];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j-1];
                                        }
                                    }
                                    //right
                                    else if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        dist = this->eucDistance(specData[1], specData[1], numBands, j+1, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j+1];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j+1];
                                        }
                                    }
                                    //bottom
                                    else if(singlePxl[2][j] == 0)
                                    {
                                        dist = this->eucDistance(specData[2], specData[1], numBands, j, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[2][j];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[2][j];
                                        }
                                    }
                                    
                                    if(first)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = minDistVal;
                                        hasChangeOccured = true;
                                    }
                                }
                            }
                            else
                            {
                                outData[j] = inData[1][j];
                            }
                        }
                        else
                        {
                            outData[j] = inData[1][j];
                        }
                    }
                    
                }
                else if(i == (height-1))
                {
                    clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, inData[0], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i-1, width, 1, singlePxl[0], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i, width, 1, singlePxl[1], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned int n = 0; n < numBands; ++n)
                    {
                        specBands[n]->RasterIO(GF_Read, 0, i-1, width, 1, specData[0][n], width, 1, GDT_Float32, 0, 0);
                        specBands[n]->RasterIO(GF_Read, 0, i, width, 1, specData[1][n], width, 1, GDT_Float32, 0, 0);
                    }
                    
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(singlePxl[1][j] == 1)
                            {
                                if(j == 0)
                                {
                                    first = true;
                                    minDist = 0;
                                    minDistVal = 0;
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        dist = this->eucDistance(specData[0], specData[1], numBands, j, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[0][j];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[0][j];
                                        }
                                    }
                                    //right
                                    else if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        dist = this->eucDistance(specData[1], specData[1], numBands, j+1, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j+1];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j+1];
                                        }
                                    }
                                    
                                    if(first)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = minDistVal;
                                        hasChangeOccured = true;
                                    }
                                }
                                else if(j == (width-1))
                                {
                                    first = true;
                                    minDist = 0;
                                    minDistVal = 0;
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        dist = this->eucDistance(specData[0], specData[1], numBands, j, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[0][j];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[0][j];
                                        }
                                    }
                                    //left
                                    else if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        dist = this->eucDistance(specData[1], specData[1], numBands, j-1, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j-1];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j-1];
                                        }
                                    }
                                    
                                    if(first)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = minDistVal;
                                        hasChangeOccured = true;
                                    }
                                }
                                else 
                                {
                                    first = true;
                                    minDist = 0;
                                    minDistVal = 0;
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        dist = this->eucDistance(specData[0], specData[1], numBands, j, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[0][j];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[0][j];
                                        }
                                    }
                                    //left
                                    else if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        dist = this->eucDistance(specData[1], specData[1], numBands, j-1, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j-1];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j-1];
                                        }
                                    }
                                    //right
                                    else if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        dist = this->eucDistance(specData[1], specData[1], numBands, j+1, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j+1];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j+1];
                                        }
                                    }
                                    
                                    if(first)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = minDistVal;
                                        hasChangeOccured = true;
                                    }
                                }                                
                            }
                            else
                            {
                                outData[j] = inData[1][j];
                            }
                        }
                        else
                        {
                            outData[j] = inData[1][j];
                        }
                    }
                }
                else
                {
                    clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, inData[0], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, inData[2], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i-1, width, 1, singlePxl[0], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i, width, 1, singlePxl[1], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i+1, width, 1, singlePxl[2], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned int n = 0; n < numBands; ++n)
                    {
                        specBands[n]->RasterIO(GF_Read, 0, i-1, width, 1, specData[0][n], width, 1, GDT_Float32, 0, 0);
                        specBands[n]->RasterIO(GF_Read, 0, i, width, 1, specData[1][n], width, 1, GDT_Float32, 0, 0);
                        specBands[n]->RasterIO(GF_Read, 0, i+1, width, 1, specData[2][n], width, 1, GDT_Float32, 0, 0);
                    }
                    
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(singlePxl[1][j] == 1)
                            {
                                if(j == 0)
                                {
                                    first = true;
                                    minDist = 0;
                                    minDistVal = 0;
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        dist = this->eucDistance(specData[0], specData[1], numBands, j, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[0][j];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[0][j];
                                        }
                                    }
                                    //right
                                    else if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        dist = this->eucDistance(specData[1], specData[1], numBands, j+1, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j+1];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j+1];
                                        }
                                    }
                                    //bottom
                                    else if(singlePxl[2][j] == 0)
                                    {
                                        dist = this->eucDistance(specData[2], specData[1], numBands, j, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[2][j];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[2][j];
                                        }
                                    }
                                    
                                    if(first)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = minDistVal;
                                        hasChangeOccured = true;
                                    }
                                }
                                else if(j == (width-1))
                                {
                                    first = true;
                                    minDist = 0;
                                    minDistVal = 0;
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        dist = this->eucDistance(specData[0], specData[1], numBands, j, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[0][j];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[0][j];
                                        }
                                    }
                                    //left
                                    else if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        dist = this->eucDistance(specData[1], specData[1], numBands, j-1, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j-1];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j-1];
                                        }
                                    }
                                    //bottom
                                    else if(singlePxl[2][j] == 0)
                                    {
                                        dist = this->eucDistance(specData[2], specData[1], numBands, j, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[2][j];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[2][j];
                                        }
                                    }
                                    
                                    if(first)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = minDistVal;
                                        hasChangeOccured = true;
                                    }
                                }
                                else 
                                {
                                    first = true;
                                    minDist = 0;
                                    minDistVal = 0;
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        dist = this->eucDistance(specData[0], specData[1], numBands, j, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[0][j];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[0][j];
                                        }
                                    }
                                    //left
                                    else if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        dist = this->eucDistance(specData[1], specData[1], numBands, j-1, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j-1];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j-1];
                                        }
                                    }
                                    //right
                                    else if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        dist = this->eucDistance(specData[1], specData[1], numBands, j+1, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j+1];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[1][j+1];
                                        }
                                    }
                                    //bottom
                                    else if(singlePxl[2][j] == 0)
                                    {
                                        dist = this->eucDistance(specData[2], specData[1], numBands, j, j);
                                        if(first)
                                        {
                                            minDist = dist;
                                            minDistVal = inData[2][j];
                                            first = false;
                                        }
                                        else if(dist < minDist) 
                                        {
                                            minDist = dist;
                                            minDistVal = inData[2][j];
                                        }
                                    }
                                    
                                    if(first)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = minDistVal;
                                        hasChangeOccured = true;
                                    }
                                }
                            }
                            else
                            {
                                outData[j] = inData[1][j];
                            }
                        }
                        else
                        {
                            outData[j] = inData[1][j];
                        }
                    }
                }
                
                outBand->RasterIO(GF_Write, 0, i, width, 1, outData, width, 1, GDT_UInt32, 0, 0);
            }
            cout << ". Complete\n";
            
            delete[] inData[0];
            delete[] inData[1];
            delete[] inData[2];
            delete[] inData;
            delete[] singlePxl[0];
            delete[] singlePxl[1];
            delete[] singlePxl[2];
            delete[] singlePxl;
            for(unsigned int i = 0; i < numBands; ++i)
            {
                delete[] specData[0][i];
                delete[] specData[1][i];
                delete[] specData[2][i];
            }
            delete[] specData[0];
            delete[] specData[1];
            delete[] specData[2];
            delete[] specData;
            delete[] outData;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        
        return hasChangeOccured;
    }
    
    float RSGISEliminateSinglePixels::eucDistance(float **vals1, float **vals2, unsigned int numBands, unsigned int col1, unsigned int col2)
    {
        float dist = 0;
        for(unsigned int i = 0; i < numBands; ++i)
        {
            dist += (vals1[i][col1] - vals2[i][col2]) * (vals1[i][col1] - vals2[i][col2]);
        }
        
        if(dist > 0)
        {
            dist = sqrt(dist/numBands);
        }
        //cout << dist << endl;
        return dist;
    }
        
    RSGISEliminateSinglePixels::~RSGISEliminateSinglePixels()
    {
        
    }
    
}}
