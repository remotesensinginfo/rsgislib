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
	
	void RSGISMaskImage::maskImage(GDALDataset *dataset, GDALDataset *mask, std::string outputImage, std::string imageFormat, GDALDataType outDataType, double outputValue, double maskValue)throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALDataset **datasets = NULL;
		RSGISCalcImage *calcImg = NULL;
		RSGISApplyImageMask *applyMask = NULL;
		try
		{
			int numDS = 2;
			datasets = new GDALDataset*[numDS];
			datasets[0] = mask;
			datasets[1] = dataset;
			
			applyMask = new RSGISApplyImageMask(dataset->GetRasterCount(), outputValue, maskValue);
			calcImg = new RSGISCalcImage(applyMask, "", true);
			calcImg->calcImage(datasets, numDS, outputImage, false, NULL, imageFormat, outDataType);
			
		}
		catch(RSGISImageCalcException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			if(applyMask != NULL)
			{
				delete applyMask;
			}
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			if(applyMask != NULL)
			{
				delete applyMask;
			}
			throw e;
		}
		
		if(datasets != NULL)
		{
			delete[] datasets;
		}
		if(calcImg != NULL)
		{
			delete calcImg;
		}
		if(applyMask != NULL)
		{
			delete applyMask;
		}
	}
    
    void RSGISMaskImage::genFiniteImgMask(GDALDataset *dataset, std::string outputImage, std::string imageFormat)throw(RSGISImageCalcException,RSGISImageBandException)
    {
        RSGISCalcImage *calcImg = NULL;
        try
        {
            RSGISCreateFiniteImageMask createMask = RSGISCreateFiniteImageMask();
            calcImg = new RSGISCalcImage(&createMask, "", true);
            calcImg->calcImage(&dataset, 1, outputImage, false, NULL, imageFormat, GDT_Byte);
        }
        catch(RSGISImageCalcException e)
        {
            if(calcImg != NULL)
            {
                delete calcImg;
            }
            throw e;
        }
        catch(RSGISImageBandException e)
        {
            if(calcImg != NULL)
            {
                delete calcImg;
            }
            throw e;
        }
        
        if(calcImg != NULL)
        {
            delete calcImg;
        }
    }
	
	RSGISApplyImageMask::RSGISApplyImageMask(int numberOutBands, double outputValue, double maskValue) : RSGISCalcImageValue(numberOutBands)
	{
		this->outputValue = outputValue;
        this->maskValue = maskValue;
	}
	
	void RSGISApplyImageMask::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
	{
		if(bandValues[0] == maskValue)
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
	
	void RSGISApplyImageMask::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISApplyImageMask::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISApplyImageMask::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISApplyImageMask::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}

	bool RSGISApplyImageMask::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	RSGISApplyImageMask::~RSGISApplyImageMask()
	{
		
	}
    
    
 
    RSGISCreateFiniteImageMask::RSGISCreateFiniteImageMask() :RSGISCalcImageValue(1)
    {
        
    }
    
    void RSGISCreateFiniteImageMask::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
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
    
	
}}
