/*
 *  RSGISMeanVector.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/07/2008.
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

#include "RSGISMeanVector.h"

namespace rsgis{namespace img{
	
	RSGISCalcMeanVectorIndividual::RSGISCalcMeanVectorIndividual(int numOutputValues) : RSGISCalcImageSingleValue(numOutputValues)
	{
		this->n = 0;
		this->sum = 0;
	}
	
	void RSGISCalcMeanVectorIndividual::calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISCalcMeanVectorIndividual::calcImageValue(float *bandValuesImageA, int numBands, geos::geom::Envelope *extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISCalcMeanVectorIndividual::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISCalcMeanVectorIndividual::calcImageValue(float *bandValuesImageA, int numBands, int band) throw(RSGISImageCalcException)
	{
		this->n++;
		this->sum += bandValuesImageA[band]; 
	}
	
	double* RSGISCalcMeanVectorIndividual::getOutputValues() throw(RSGISImageCalcException)
	{
		this->outputValues[0] = sum/n;
		return this->outputValues;
	}
	
	void RSGISCalcMeanVectorIndividual::reset()
	{
		this->n = 0;
		this->sum = 0;
	}
	
	
	RSGISCalcMeanVectorAll::RSGISCalcMeanVectorAll(int numOutputValues) : RSGISCalcImageSingleValue(numOutputValues)
	{
		this->n = 0;
		this->sum = new double[numOutputValues];
	}
	
	void RSGISCalcMeanVectorAll::calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISCalcMeanVectorAll::calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISCalcMeanVectorAll::calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException)
	{
		if(this->numOutputValues != numBands)
		{
			throw RSGISImageCalcException("The number of output values and number of input bands do not match.");
		}
		this->n++;
		
		for(int i = 0; i < numBands; i++)
		{
			this->sum[i] += bandValuesImage[i];
		}
	}
	
	void RSGISCalcMeanVectorAll::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	double* RSGISCalcMeanVectorAll::getOutputValues() throw(RSGISImageCalcException)
	{
		for(int i = 0; i < this->numOutputValues; i++)
		{
			this->outputValues[i] = this->sum[i]/n;
		}
		return this->outputValues;
	}
	
	void RSGISCalcMeanVectorAll::reset()
	{
		this->n = 0;
		for(int i = 0; i < this->numOutputValues; i++)
		{
			this->sum[i] = 0;
		}
	}

	
}}
