/*
 *  RSGISMorphologyTopHat.cpp
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 03/03/2012
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
#include "RSGISMorphologyTopHat.h"

namespace rsgis{namespace filter{
    
    RSGISImageMorphologyTopHat::RSGISImageMorphologyTopHat()
    {
        
    }
    
    // Closing - Input Image
    void RSGISImageMorphologyTopHat::performBlackTopHat(GDALDataset *dataset, string outputImage, string tempImage, bool useMemory, Matrix *matrixOperator) throw(RSGISImageCalcException, RSGISImageBandException)
    {
        try 
        {
            if(matrixOperator->n != matrixOperator->m)
            {
                throw RSGISImageCalcException("Morphological operator must be a square matrix.");
            }
            
            RSGISImageUtils imgUtils;
            GDALDataset *outDataset = NULL;
            GDALDataset *tmpDataset = NULL;
            GDALDataset **tmpGDALDataArray = new GDALDataset*[2];
            
            outDataset = imgUtils.createCopy(dataset, outputImage, "ENVI", GDT_Float32);
            imgUtils.zerosFloatGDALDataset(outDataset);
            
            if(useMemory)
            {
                tmpDataset = imgUtils.createCopy(dataset, tempImage, "MEM", GDT_Float32);
            }
            else
            {
                tmpDataset = imgUtils.createCopy(dataset, tempImage, "ENVI", GDT_Float32);
            }
            imgUtils.zerosFloatGDALDataset(tmpDataset);
            
            
            RSGISCalcImageValue *imgErode = new RSGISMorphologyErode(dataset->GetRasterCount(), matrixOperator);
            RSGISCalcImage calcImageErode(imgErode);
            RSGISCalcImageValue *imgDilate = new RSGISMorphologyDilate(dataset->GetRasterCount(), matrixOperator);
            RSGISCalcImage calcImageDilate(imgDilate);
            RSGISCalcImageValue *imgDiff = new RSGISMorphologyImageDiff(dataset->GetRasterCount());
            RSGISCalcImage calcImageDiff(imgDiff);
            
            tmpGDALDataArray[0] = dataset;
            calcImageDilate.calcImageWindowData(tmpGDALDataArray, 1, outDataset, matrixOperator->n);
            tmpGDALDataArray[0] = outDataset;
            calcImageErode.calcImageWindowData(tmpGDALDataArray, 1, tmpDataset, matrixOperator->n);
            
            // tmp - input
            tmpGDALDataArray[0] = tmpDataset;
            tmpGDALDataArray[1] = dataset;
            calcImageDiff.calcImage(tmpGDALDataArray, 2, outDataset);
            
            delete imgErode;
            delete imgDilate;
            delete imgDiff;
            
            delete[] tmpGDALDataArray;
            
            GDALClose(outDataset);
            GDALClose(tmpDataset);
        } 
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
        catch (RSGISImageBandException &e) 
        {
            throw e;
        }
        catch (RSGISImageException &e) 
        {
            throw RSGISImageCalcException(e.what());
        }
    }
    
    // Input Image - Opening
    void RSGISImageMorphologyTopHat::performWhiteTopHat(GDALDataset *dataset, string outputImage, string tempImage, bool useMemory, Matrix *matrixOperator) throw(RSGISImageCalcException, RSGISImageBandException)
    {
        try 
        {
            if(matrixOperator->n != matrixOperator->m)
            {
                throw RSGISImageCalcException("Morphological operator must be a square matrix.");
            }
            
            RSGISImageUtils imgUtils;
            GDALDataset *outDataset = NULL;
            GDALDataset *tmpDataset = NULL;
            GDALDataset **tmpGDALDataArray = new GDALDataset*[2];
            
            outDataset = imgUtils.createCopy(dataset, outputImage, "ENVI", GDT_Float32);
            imgUtils.zerosFloatGDALDataset(outDataset);
            
            if(useMemory)
            {
                tmpDataset = imgUtils.createCopy(dataset, tempImage, "MEM", GDT_Float32);
            }
            else
            {
                tmpDataset = imgUtils.createCopy(dataset, tempImage, "ENVI", GDT_Float32);
            }
            imgUtils.zerosFloatGDALDataset(tmpDataset);
            
            
            RSGISCalcImageValue *imgErode = new RSGISMorphologyErode(dataset->GetRasterCount(), matrixOperator);
            RSGISCalcImage calcImageErode(imgErode);
            RSGISCalcImageValue *imgDilate = new RSGISMorphologyDilate(dataset->GetRasterCount(), matrixOperator);
            RSGISCalcImage calcImageDilate(imgDilate);
            RSGISCalcImageValue *imgDiff = new RSGISMorphologyImageDiff(dataset->GetRasterCount());
            RSGISCalcImage calcImageDiff(imgDiff);
            
            tmpGDALDataArray[0] = dataset;
            calcImageErode.calcImageWindowData(tmpGDALDataArray, 1, outDataset, matrixOperator->n);
            tmpGDALDataArray[0] = outDataset;
            calcImageDilate.calcImageWindowData(tmpGDALDataArray, 1, tmpDataset, matrixOperator->n);
            
            // input - tmp
            tmpGDALDataArray[0] = dataset;
            tmpGDALDataArray[1] = tmpDataset;
            calcImageDiff.calcImage(tmpGDALDataArray, 2, outDataset);
            
            delete imgErode;
            delete imgDilate;
            delete imgDiff;
            
            delete[] tmpGDALDataArray;
            
            GDALClose(outDataset);
            GDALClose(tmpDataset);
        } 
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
        catch (RSGISImageBandException &e) 
        {
            throw e;
        }
        catch (RSGISImageException &e) 
        {
            throw RSGISImageCalcException(e.what());
        }
    }
    
    RSGISImageMorphologyTopHat::~RSGISImageMorphologyTopHat()
    {
        
    }
    
    
    RSGISMorphologyImageDiff::RSGISMorphologyImageDiff(int numberOutBands) : RSGISCalcImageValue(numberOutBands)
	{

	}
    
	void RSGISMorphologyImageDiff::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
	{        
        if((numBands/2) != this->numOutBands)
        {
            throw RSGISImageCalcException("Expecting twice as many input bands as output.");
        }
        
		for(int b = 0; b < this->numOutBands; b++)
        {
			output[b] = bandValues[b] - bandValues[b+this->numOutBands];
		}
		
	}
    
	RSGISMorphologyImageDiff::~RSGISMorphologyImageDiff()
	{
		
	}
    
}}