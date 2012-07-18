/*
 *  RSGISCalcEditImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/08/2011.
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

#include "img/RSGISCalcEditImage.h"

namespace rsgis 
{
	namespace img
	{
        RSGISCalcEditImage::RSGISCalcEditImage(RSGISCalcImageValue *valueCalc)
        {
            calc = valueCalc;
        }
            
        void RSGISCalcEditImage::calcImage(GDALDataset *dataset) throw(RSGISImageCalcException,RSGISImageBandException)
        {
            GDALAllRegister();
            RSGISImageUtils imgUtils;
            double *gdalTranslation = new double[6];

            int height = 0;
            int width = 0;
            int numInBands = 0;
            
            float **inputData = NULL;
            float *inDataColumn = NULL;
            
            GDALRasterBand **inputRasterBands = NULL;
            
            try
            {
                dataset->GetGeoTransform(gdalTranslation);
                numInBands = dataset->GetRasterCount();
                width = dataset->GetRasterXSize();
                height = dataset->GetRasterYSize();
                
                // Get Image Input Bands
                inputRasterBands = new GDALRasterBand*[numInBands];
                for(int i = 0; i < numInBands; i++)
				{
					inputRasterBands[i] = dataset->GetRasterBand(i+1);
                }
                
                // Allocate memory
                inputData = new float*[numInBands];
                for(int i = 0; i < numInBands; i++)
                {
                    inputData[i] = (float *) CPLMalloc(sizeof(float)*width);
                }
                inDataColumn = new float[numInBands];
                
                geos::geom::Envelope extent;
                double pxlTLX = 0;
                double pxlTLY = 0;
                double pxlWidth = 0;
                double pxlHeight = 0;
                
                pxlTLX = gdalTranslation[0];
                pxlTLY = gdalTranslation[3];
                pxlWidth = gdalTranslation[1];
                pxlHeight = gdalTranslation[5];
                
                if(pxlHeight < 0)
                {
                    pxlHeight *= (-1);
                }
                
                int feedback = height/10;
                int feedbackCounter = 0;
                std::cout << "Started" << std::flush;
                // Loop images to process data
                for(int i = 0; i < height; i++)
                {
                    //std::cout << i << " of " << height << std::endl;
                    if((i % feedback) == 0)
                    {
                        std::cout << ".." << feedbackCounter << ".." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(int n = 0; n < numInBands; n++)
                    {
                        inputRasterBands[n]->RasterIO(GF_Read, 0, i, width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
                    }
                    
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][j];
                        }
                        
                        extent.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
                        this->calc->calcImageValue(inDataColumn, numInBands, extent);
                        
                        for(int n = 0; n < numInBands; n++)
                        {
                            inputData[n][j] = inDataColumn[n];
                        }
                        
                        pxlTLX += pxlWidth;
                    }
                    
                    for(int n = 0; n < numInBands; n++)
                    {
                        inputRasterBands[n]->RasterIO(GF_Write, 0, i, width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
                    }
                    
                    pxlTLY -= pxlHeight;
                    pxlTLX = gdalTranslation[0];
                }
                std::cout << " Complete.\n";
            }
            catch(RSGISImageCalcException& e)
            {
                if(gdalTranslation != NULL)
                {
                    delete[] gdalTranslation;
                }
                
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
                if(inDataColumn != NULL)
                {
                    delete[] inDataColumn;
                }
                if(inputRasterBands != NULL)
                {
                    delete[] inputRasterBands;
                }
                throw e;
            }
            catch(RSGISImageBandException& e)
            {
                if(gdalTranslation != NULL)
                {
                    delete[] gdalTranslation;
                }			
                
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
                if(inDataColumn != NULL)
                {
                    delete[] inDataColumn;
                }
                if(inputRasterBands != NULL)
                {
                    delete[] inputRasterBands;
                }
                throw e;
            }
            
            if(gdalTranslation != NULL)
            {
                delete[] gdalTranslation;
            }
            
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
            if(inDataColumn != NULL)
            {
                delete[] inDataColumn;
            }
            if(inputRasterBands != NULL)
            {
                delete[] inputRasterBands;
            }
        }
        
        RSGISCalcEditImage::~RSGISCalcEditImage()
        {
            
        }
	}
}


