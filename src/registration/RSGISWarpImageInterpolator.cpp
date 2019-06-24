/*
 *  RSGISWarpImageInterpolator.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/09/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 * 
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISWarpImageInterpolator.h"

namespace rsgis{namespace reg{
	
	void RSGISWarpImageNNInterpolator::calcValue(GDALDataset *image, float *outValues, unsigned int numOutVals, double eastings, double northings, unsigned int xPxl, unsigned int yPxl, float inImgRes, float outImgRes)
	{
        rsgis::img::RSGISImageUtils imgUtils;
		
		int *dsOffsets = new int[2];
		dsOffsets[0] = xPxl;
		dsOffsets[1] = yPxl;
		
		unsigned int numVals = 0;
		
		float **dataVals =  imgUtils.getImageDataBlock(image, dsOffsets, 1, 1, &numVals);
		
		if(numVals != 1)
		{
			RSGISImageWarpException("The number of values retrieved from the input image should be equal to 1.");
		}
		
		for(unsigned int i = 0; i < numOutVals; ++i)
		{
			outValues[i] = dataVals[i][0];
		}
        
        // Tidy up
        for(unsigned int i = 0; i < numOutVals; ++i)
		{
			delete[] dataVals[i];
		}
        
        delete[] dataVals;
        delete[] dsOffsets;
	}

}}




