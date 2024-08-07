/*
 *  RSGISApplyEigenvectors.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/08/2008.
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

#include "RSGISApplyEigenvectors.h"

namespace rsgis{namespace img{

	RSGISApplyEigenvectors::RSGISApplyEigenvectors(int numberOutBands, rsgis::math::Matrix *eigenvectors) : RSGISCalcImageValue(numberOutBands)
	{
		this->eigenvectors = eigenvectors;
	}
	
	void RSGISApplyEigenvectors::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		if(this->numOutBands > this->eigenvectors->n)
		{
			RSGISImageCalcException("There are no enough eigenvectors for the number of output bands");
		}
		
		int eigenIndex = 0;
		for(int i = 0; i < this->numOutBands; i++)
		{
			output[i] = 0;
			
			for(int j = 0; j < eigenvectors->m; j++)
			{
				eigenIndex = (i*eigenvectors->m)+j;
				output[i] += (bandValues[j] * eigenvectors->matrix[eigenIndex]);
			}
		}
	}

	RSGISApplyEigenvectors::~RSGISApplyEigenvectors()
	{
		
	}

}}
