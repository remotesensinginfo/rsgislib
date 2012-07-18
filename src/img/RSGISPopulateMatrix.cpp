/*
 *  RSGISPopulateMatrix.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/11/2008.
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

#include "RSGISPopulateMatrix.h"

namespace rsgis{namespace img{
	
	RSGISPopulateMatrix::RSGISPopulateMatrix(int numOutputValues, int numPixels) : RSGISCalcImageSingleValue(numOutputValues)
	{
		this->matrixCounter = 0;
		this->numPixels = numPixels;
        rsgis::math::RSGISMatrices rsgisMatrices;
		this->matrix = rsgisMatrices.createMatrix(numOutputValues, numPixels);
	}
	
	void RSGISPopulateMatrix::calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISPopulateMatrix::calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISPopulateMatrix::calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISPopulateMatrix::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(RSGISImageCalcException)
	{
		if(numBands != numOutputValues)
		{
			throw RSGISImageCalcException("The number of image bands needs to be the same as the number of output values");
		}
		
		for(int i = 0; i < numBands; i++)
		{
			this->matrix->matrix[matrixCounter++] = bandValuesImage[i];
		}
	}
	
	double* RSGISPopulateMatrix::getOutputValues()  throw(RSGISImageCalcException)
	{
		for(int i = 0; i < numOutputValues; i++)
		{
			this->outputValues[i] = 0;
		}
		return outputValues;
	}
	
    rsgis::math::Matrix* RSGISPopulateMatrix::getMatrix()
	{
		return matrix;
	}
	
	void RSGISPopulateMatrix::reset(int numPixels)
	{
		if(this->matrix != NULL)
		{
			if(this->matrix->matrix != NULL)
			{
				delete this->matrix->matrix;
			}
			delete this->matrix;
		}
		this->matrixCounter = 0;
		this->numPixels = numPixels;
        rsgis::math::RSGISMatrices rsgisMatrices;
		this->matrix = rsgisMatrices.createMatrix(numOutputValues, numPixels);
	}
	
	void RSGISPopulateMatrix::reset()
	{
		this->matrixCounter = 0;
	}
	
	RSGISPopulateMatrix::~RSGISPopulateMatrix()
	{
		if(this->matrix != NULL)
		{
			if(this->matrix->matrix != NULL)
			{
				delete this->matrix->matrix;
			}
			delete this->matrix;
		}
	}
	
}}

