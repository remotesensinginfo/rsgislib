/*
 *  RSGISCopyImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/11/2008.
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

#include "RSGISCopyImage.h"

namespace rsgis{namespace img{
	
	RSGISCopyImage::RSGISCopyImage(int numberOutBands) : RSGISCalcImageValue(numberOutBands)
	{
		
	}
	
	void RSGISCopyImage::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		if(numBands != numOutBands)
		{
			RSGISImageCalcException("The number of input bands should be equal to the number of output bands..");
		}
		
		for(int i = 0; i < numBands; i++)
		{
			output[i] = bandValues[i];
		}
	}
	
	RSGISCopyImage::~RSGISCopyImage()
	{
		
	}
    
    
    
    RSGISCopyImageBandSelect::RSGISCopyImageBandSelect(std::vector<unsigned int> bands) : RSGISCalcImageValue(bands.size())
    {
        this->bands = bands;
    }
    
    void RSGISCopyImageBandSelect::calcImageValue(float *bandValues, int numBands, double *output) 
    {
        if(numOutBands == 0)
		{
			RSGISImageCalcException("The number of output image bands must be greater than 0.");
		}
        else if(numOutBands != bands.size())
        {
            RSGISImageCalcException("The number of output image bands must be equal to the number of band specified.");
        }
		
        unsigned int bandIdx = 0;
		for(std::vector<unsigned int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
		{
            if((*iterBands) == 0)
            {
                RSGISImageCalcException("A band index of zero has been specified, band numbering starts at 1.");
            }
            if((*iterBands) > numBands)
            {
                RSGISImageCalcException("A band index outside of the input image range has been specified, band numbering starts at 1.");
            }
			output[bandIdx++] = bandValues[(*iterBands)-1];
		}
    }
    
    RSGISCopyImageBandSelect::~RSGISCopyImageBandSelect()
    {
        
    }


    RSGISCopyImageBandToBits::RSGISCopyImageBandToBits(int numberOutBands, unsigned int imgBand, GDALDataType gdalDataType) : RSGISCalcImageValue(numberOutBands)
    {
        this->imgBandIdx = imgBand-1;
        this->gdalDataType = gdalDataType;
    }

    void RSGISCopyImageBandToBits::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output)
    {
        if(gdalDataType == GDT_Byte)
        {
            std::bitset<8> bitValsArr = intBandValues[this->imgBandIdx];
            for(int i = 0; i < 8; ++i)
            {
                output[i]=bitValsArr[i];
            }
        }
        else if((gdalDataType == GDT_UInt16) | (gdalDataType == GDT_Int16))
        {
            std::bitset<16> bitValsArr = intBandValues[this->imgBandIdx];
            for(int i = 0; i < 16; ++i)
            {
                output[i]=bitValsArr[i];
            }
        }
        else if((gdalDataType == GDT_UInt32) | (gdalDataType == GDT_Int32))
        {
            std::bitset<32> bitValsArr = intBandValues[this->imgBandIdx];
            for(int i = 0; i < 32; ++i)
            {
                output[i]=bitValsArr[i];
            }
        }
    }

    RSGISCopyImageBandToBits::~RSGISCopyImageBandToBits()
    {

    }
    
    
}}


