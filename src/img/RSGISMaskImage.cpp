/*
 *  RSGISMaskImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/05/2008.
 *  Copyright 2008 RSGISLib.
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

#include "RSGISMaskImage.h"

namespace rsgis{namespace img{
	
	RSGISMaskImage::RSGISMaskImage()
	{
		
	}
	
	void RSGISMaskImage::maskImage(GDALDataset *dataset, GDALDataset *mask, std::string outputImage, std::string imageFormat, GDALDataType outDataType, double outputValue, std::vector<float> maskValues)
	{
		GDALDataset **datasets = NULL;
		try
		{
			int numDS = 2;
			datasets = new GDALDataset*[numDS];
			datasets[0] = mask;
			datasets[1] = dataset;
			
			RSGISApplyImageMask applyMask = RSGISApplyImageMask(dataset->GetRasterCount(), outputValue, maskValues);
			RSGISCalcImage calcImg = RSGISCalcImage(&applyMask, "", true);
			calcImg.calcImage(datasets, numDS, outputImage, false, NULL, imageFormat, outDataType);
			
            if(datasets != NULL)
            {
                delete[] datasets;
            }
		}
		catch(RSGISImageCalcException &e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			
			throw e;
		}
		catch(RSGISImageBandException &e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			
			throw e;
		}
	}
    
    void RSGISMaskImage::genFiniteImgMask(GDALDataset *dataset, std::string outputImage, std::string imageFormat)
    {
        try
        {
            RSGISCreateFiniteImageMask createMask = RSGISCreateFiniteImageMask();
            RSGISCalcImage calcImg = RSGISCalcImage(&createMask, "", true);
            calcImg.calcImage(&dataset, 1, outputImage, false, NULL, imageFormat, GDT_Byte);
        }
        catch(RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(RSGISImageBandException &e)
        {
            throw e;
        }
    }
    
    void RSGISMaskImage::genValidImgMask(GDALDataset **dataset, unsigned int numImages, std::string outputImage, std::string imageFormat, float noDataVal)
    {
        try
        {
            RSGISGenValidImageMask createMask = RSGISGenValidImageMask(noDataVal);
            RSGISCalcImage calcImg = RSGISCalcImage(&createMask, "", true);
            calcImg.calcImage(dataset, numImages, outputImage, false, NULL, imageFormat, GDT_Byte);
        }
        catch(RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(RSGISImageBandException &e)
        {
            throw e;
        }
    }
    
	
	RSGISApplyImageMask::RSGISApplyImageMask(int numberOutBands, double outputValue, std::vector<float> maskValues) : RSGISCalcImageValue(numberOutBands)
	{
		this->outputValue = outputValue;
        this->maskValues = maskValues;
	}
	
	void RSGISApplyImageMask::calcImageValue(float *bandValues, int numBands, double *output) 
	{
        bool foundMaskVal = false;
        for(std::vector<float>::iterator iterVals = maskValues.begin(); iterVals != maskValues.end(); ++iterVals)
        {
            if(bandValues[0] == (*iterVals))
            {
                foundMaskVal = true;
                break;
            }
        }
        
		if(foundMaskVal)
		{
			for(int i = 0; i < numOutBands; i++)
			{
				output[i] = outputValue;
			}
		}
		else 
		{
			for(int i = 0; i < numOutBands; i++)
			{
				output[i] = bandValues[i+1];
			}
		}
	}
		
	RSGISApplyImageMask::~RSGISApplyImageMask()
	{
		
	}
    
    
 
    RSGISCreateFiniteImageMask::RSGISCreateFiniteImageMask() :RSGISCalcImageValue(1)
    {
        
    }
    
    void RSGISCreateFiniteImageMask::calcImageValue(float *bandValues, int numBands, double *output) 
    {
        bool finiteVal = true;
        for(int i = 0; i < numBands; ++i)
        {
            if(!boost::math::isfinite(bandValues[i]))
            {
                finiteVal = false;
            }
        }
        
        if(finiteVal)
        {
            output[0] = 1;
        }
        else
        {
            output[0] = 0;
        }
    }
    
    RSGISCreateFiniteImageMask::~RSGISCreateFiniteImageMask()
    {
        
    }
    
    
    
    RSGISGenValidImageMask::RSGISGenValidImageMask(float noDataVal) :RSGISCalcImageValue(1)
    {
        this->noDataVal = noDataVal;
    }
    
    void RSGISGenValidImageMask::calcImageValue(float *bandValues, int numBands, double *output) 
    {
        bool validData = true;
        for(int i = 0; i < numBands; ++i)
        {
            if(bandValues[i] == noDataVal)
            {
                validData = false;
            }
        }
        
        if(validData)
        {
            output[0] = 1;
        }
        else
        {
            output[0] = 0;
        }
    }
    
    RSGISGenValidImageMask::~RSGISGenValidImageMask()
    {
        
    }
    
	
}}
