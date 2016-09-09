/*
 *  RSGISImageBand2Matrix.cpp
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

#include "RSGISImageBand2Matrix.h"


namespace rsgis{namespace img{
	
	RSGISImageBand2Matrix::RSGISImageBand2Matrix(int numOutputValues, int band, int x, int y) : RSGISCalcImageSingleValue(numOutputValues)
	{
		this->matrixCounter = 0;
		this->band = band;
		rsgis::math::RSGISMatrices rsgisMatrices;
		this->matrix = rsgisMatrices.createMatrix(y, x);
	}
	
	void RSGISImageBand2Matrix::calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISImageBand2Matrix::calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException)
	{
		if(band >= numBands)
		{
			throw RSGISImageCalcException("There are insufficient number of bands in the image for the selected band");
		}
		
		this->matrix->matrix[matrixCounter++] = bandValuesImage[band];
	}
	
	void RSGISImageBand2Matrix::calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISImageBand2Matrix::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(RSGISImageCalcException)
	{
		if(band >= numBands)
		{
			throw RSGISImageCalcException("There are insufficient number of bands in the image for the selected band");
		}
		
		this->matrix->matrix[matrixCounter++] = bandValuesImage[band];
	}
	
	double* RSGISImageBand2Matrix::getOutputValues()  throw(RSGISImageCalcException)
	{
		for(int i = 0; i < numOutputValues; i++)
		{
			this->outputValues[i] = 0;
		}
		return outputValues;
	}
	
	rsgis::math::Matrix* RSGISImageBand2Matrix::getMatrix()
	{
		return matrix;
	}
	
	void RSGISImageBand2Matrix::reset(int band, int x, int y)
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
		this->band = band;
		rsgis::math::RSGISMatrices rsgisMatrices;
		this->matrix = rsgisMatrices.createMatrix(y, x);
	}
	
	void RSGISImageBand2Matrix::reset()
	{
		this->matrixCounter = 0;
	}
	
	RSGISImageBand2Matrix::~RSGISImageBand2Matrix()
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

