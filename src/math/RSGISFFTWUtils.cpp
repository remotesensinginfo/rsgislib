/*
 *  RSGISFFTWUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 26/11/2008.
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

#include "RSGISFFTWUtils.h"


namespace rsgis{namespace math{

	RSGISFFTWUtils::RSGISFFTWUtils()
	{
		
	}
	/*
	Matrix* RSGISFFTWUtils::computeFFTW(Matrix *inputData)
	{
		RSGISMatrices matrixUtils;
		Matrix *outputMatrixReal = NULL;
		fftw_complex *out;
		fftw_complex *in;
		fftw_plan plan_f;

		int inputSize = inputData->m * inputData->n;
		
		outputMatrixReal = matrixUtils.createMatrix(inputData->n, inputData->m);
		in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * inputSize);
		out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * inputSize);		
		plan_f = fftw_plan_dft_2d(inputData->m, inputData->n, in, out, FFTW_FORWARD, FFTW_MEASURE);
				 
		for(int i = 0; i < inputSize; i++)
		{
			in[i][0] = inputData->matrix[i];
			in[i][1] = 0;
		}
		
		std::cout << "Executing FFTW...\n";
		fftw_execute(plan_f);
		std::cout << "Finished FFTW...\n";
						
		for(int i = 0; i < inputSize; i++)
		{
			outputMatrixReal->matrix[i] = sqrt((out[i][0] * out[i][0]) + (out[i][1] * out[i][1]));
		}
		
		fftw_destroy_plan(plan_f); 
		fftw_free(in);
		fftw_free(out);

		return outputMatrixReal;
	}
	
	void RSGISFFTWUtils::fftwShift(fftw_complex *in, fftw_complex *out, int width, int height)
	{
		int length = width * height;
		int row = 0;
		int column = 0;
		
		int hw1 = 0;
		int hw2 = 0;
		
		int hh1 = 0;
		int hh2 = 0;
		
		if(width % 2 == 0)
		{
			// even
			hw1 = hw2 = width/2;
		}
		else
		{
			// odd
			hw1 = (width/2) + 1;
			hw2 = width/2;
		}
		
		if(height % 2 == 0)
		{
			// even
			hh1 = hh2 = height/2;
		}
		else
		{
			// odd
			hh1 = (height/2) + 1;
			hh2 = height/2;
		}
		
		int newI = 0;
		
		for(int i = 0; i < length; i++)
		{
			if(column < hw1 & row < hh1)
			{
				// Top LEFT
				newI = i + ((hh2 * width) + hw2);
			}
			else if(column >= hw1 & row < hh1)
			{
				// TOP RIGHT
				newI = i + ((hh2 * width) - hw1);
			}
			else if(column < hw1 & row >= hh1)
			{
				// BOTTOM LEFT
				newI = (i - (hh1 * width)) + hw2;
			}
			else if(column >= hw1 & row >= hh1)
			{
				// BOTTOM RIGHT
				newI = (i - (hh1 * width)) - hw1;
			}
			out[newI][0] = in[i][0];
			out[newI][1] = in[i][1];
			
			column++;
			if(column == width)
			{
				row++;
				column = 0;
			}
		}
	}
	
	void RSGISFFTWUtils::fftwShift(Matrix *in, Matrix *out)
	{
		int width = in->m;
		int height = in->n;
		int length = width * height;
		int row = 0;
		int column = 0;
		
		int hw1 = 0;
		int hw2 = 0;
		 
		int hh1 = 0;
		int hh2 = 0;
		
		if(width % 2 == 0)
		{
			// even
			hw1 = hw2 = width/2;
		}
		else
		{
			// odd
			hw1 = (width/2) + 1;
			hw2 = width/2;
		}
		
		if(height % 2 == 0)
		{
			// even
			hh1 = hh2 = height/2;
		}
		else
		{
			// odd
			hh1 = (height/2) + 1;
			hh2 = height/2;
		}
		
		int newI = 0;
		
		for(int i = 0; i < length; i++)
		{
			if(column < hw1 & row < hh1)
			{
				// Top LEFT
				newI = i + ((hh2 * width) + hw2);
			}
			else if(column >= hw1 & row < hh1)
			{
				// TOP RIGHT
				newI = i + ((hh2 * width) - hw1);
			}
			else if(column < hw1 & row >= hh1)
			{
				// BOTTOM LEFT
				newI = (i - (hh1 * width)) + hw2;
			}
			else if(column >= hw1 & row >= hh1)
			{
				// BOTTOM RIGHT
				newI = (i - (hh1 * width)) - hw1;
			}
			out->matrix[newI] = in->matrix[i];
			
			column++;
			if(column == width)
			{
				row++;
				column = 0;
			}
		}
	}
	*/
	RSGISFFTWUtils::~RSGISFFTWUtils()
	{
		
	}
}}
