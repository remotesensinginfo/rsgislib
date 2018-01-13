/*
 *  RSGISCalcImageValue.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/04/2008.
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

#include "RSGISCalcImageValue.h"

namespace rsgis{namespace img{
	
	RSGISCalcImageValue::RSGISCalcImageValue(int numberOutBands)
	{
		this->numOutBands = numberOutBands;
	}
	
	int RSGISCalcImageValue::getNumOutBands()
	{
		return numOutBands;
	}
	
	void RSGISCalcImageValue::setNumOutBands(int bands)
	{
		numOutBands = bands;
	}
	
	RSGISCalcImageValue::~RSGISCalcImageValue()
	{
		
	}
    
    
    RSGISCalcValuesFromMultiResInputs::RSGISCalcValuesFromMultiResInputs(int numberOutBands)
    {
        this->numOutBands = numberOutBands;
    }
    
    int RSGISCalcValuesFromMultiResInputs::getNumOutBands()
    {
        return numOutBands;
    }
    
    void RSGISCalcValuesFromMultiResInputs::setNumOutBands(int bands)
    {
        numOutBands = bands;
    }
    
    RSGISCalcValuesFromMultiResInputs::~RSGISCalcValuesFromMultiResInputs()
    {
        
    }

	
}} //rsgis::img
