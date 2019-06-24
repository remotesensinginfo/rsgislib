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
    void RSGISImageMorphologyTopHat::performBlackTopHat(GDALDataset *dataset, std::string outputImage, std::string tempImage, bool useMemory, rsgis::math::Matrix *matrixOperator, std::string format, GDALDataType outDataType)
    {
        try 
        {
            if(matrixOperator->n != matrixOperator->m)
            {
                throw rsgis::img::RSGISImageCalcException("Morphological operator must be a square matrix.");
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            GDALDataset *outDataset = NULL;
            GDALDataset *tmpDataset = NULL;
            GDALDataset **tmpGDALDataArray = new GDALDataset*[2];
            
            outDataset = imgUtils.createCopy(dataset, outputImage, format, outDataType);
            imgUtils.zerosFloatGDALDataset(outDataset);
            
            if(useMemory)
            {
                tmpDataset = imgUtils.createCopy(dataset, tempImage, "MEM", outDataType);
            }
            else
            {
                tmpDataset = imgUtils.createCopy(dataset, tempImage, format, outDataType);
            }
            imgUtils.zerosFloatGDALDataset(tmpDataset);
            
            
            rsgis::img::RSGISCalcImageValue *imgErode = new RSGISMorphologyErode(dataset->GetRasterCount(), matrixOperator);
            rsgis::img::RSGISCalcImage calcImageErode(imgErode);
            rsgis::img::RSGISCalcImageValue *imgDilate = new RSGISMorphologyDilate(dataset->GetRasterCount(), matrixOperator);
            rsgis::img::RSGISCalcImage calcImageDilate(imgDilate);
            rsgis::img::RSGISCalcImageValue *imgDiff = new RSGISMorphologyImageDiff(dataset->GetRasterCount());
            rsgis::img::RSGISCalcImage calcImageDiff(imgDiff);
            
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
        catch (rsgis::img::RSGISImageCalcException &e) 
        {
            throw e;
        }
        catch (rsgis::img::RSGISImageBandException &e) 
        {
            throw e;
        }
        catch (RSGISImageException &e) 
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    // Input Image - Opening
    void RSGISImageMorphologyTopHat::performWhiteTopHat(GDALDataset *dataset, std::string outputImage, std::string tempImage, bool useMemory, rsgis::math::Matrix *matrixOperator, std::string format, GDALDataType outDataType)
    {
        try 
        {
            if(matrixOperator->n != matrixOperator->m)
            {
                throw rsgis::img::RSGISImageCalcException("Morphological operator must be a square matrix.");
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            GDALDataset *outDataset = NULL;
            GDALDataset *tmpDataset = NULL;
            GDALDataset **tmpGDALDataArray = new GDALDataset*[2];
            
            outDataset = imgUtils.createCopy(dataset, outputImage, format, outDataType);
            imgUtils.zerosFloatGDALDataset(outDataset);
            
            if(useMemory)
            {
                tmpDataset = imgUtils.createCopy(dataset, tempImage, "MEM", outDataType);
            }
            else
            {
                tmpDataset = imgUtils.createCopy(dataset, tempImage, format, outDataType);
            }
            imgUtils.zerosFloatGDALDataset(tmpDataset);
            
            
            rsgis::img::RSGISCalcImageValue *imgErode = new RSGISMorphologyErode(dataset->GetRasterCount(), matrixOperator);
            rsgis::img::RSGISCalcImage calcImageErode(imgErode);
            rsgis::img::RSGISCalcImageValue *imgDilate = new RSGISMorphologyDilate(dataset->GetRasterCount(), matrixOperator);
            rsgis::img::RSGISCalcImage calcImageDilate(imgDilate);
            rsgis::img::RSGISCalcImageValue *imgDiff = new RSGISMorphologyImageDiff(dataset->GetRasterCount());
            rsgis::img::RSGISCalcImage calcImageDiff(imgDiff);
            
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
        catch (rsgis::img::RSGISImageCalcException &e) 
        {
            throw e;
        }
        catch (rsgis::img::RSGISImageBandException &e) 
        {
            throw e;
        }
        catch (RSGISImageException &e) 
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    RSGISImageMorphologyTopHat::~RSGISImageMorphologyTopHat()
    {
        
    }
    
    
    RSGISMorphologyImageDiff::RSGISMorphologyImageDiff(int numberOutBands) : RSGISCalcImageValue(numberOutBands)
	{

	}
    
	void RSGISMorphologyImageDiff::calcImageValue(float *bandValues, int numBands, double *output) 
	{        
        if((numBands/2) != this->numOutBands)
        {
            throw rsgis::img::RSGISImageCalcException("Expecting twice as many input bands as output.");
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