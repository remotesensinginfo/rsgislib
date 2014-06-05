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
	
	void RSGISCopyImage::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
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
    
    void RSGISCopyImageBandSelect::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
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
    
    
}}


