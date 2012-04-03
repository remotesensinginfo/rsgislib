/*
 *  RSGISCalcCovariance.cpp
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

#include "RSGISCalcCovariance.h"


namespace rsgis{namespace img{
	
	RSGISCalcCovariance::RSGISCalcCovariance(int numOutputValues, Matrix *aMeans, Matrix *bMeans) : RSGISCalcImageSingleValue(numOutputValues)
	{
		this->n = 0;
		this->sum = 0;
		this->aMeans = aMeans;
		this->bMeans = bMeans;
	}
	
	void RSGISCalcCovariance::calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException)
	{
		if(bandA > numBands)
		{
			cout << "The band A specificed is larger than the number of available bands." << endl;
			throw new RSGISImageCalcException("The band A specificed is larger than the number of available bands.");
		}
		else if(bandB > numBands)
		{
			cout << "The band B specificed is larger than the number of available bands." << endl;
			throw new RSGISImageCalcException("The band B specificed is larger than the number of available bands.");
		}
		
		n++;
		sum += ((bandValuesImageA[bandA]-aMeans->matrix[bandA])*(bandValuesImageB[bandB]-bMeans->matrix[bandB]));
	}
	
	void RSGISCalcCovariance::calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISCalcCovariance::calcImageValue(float *bandValuesImage, int numBands, Envelope *extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISCalcCovariance::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	double* RSGISCalcCovariance::getOutputValues()  throw(RSGISImageCalcException)
	{
		this->outputValues[0] = sum/(n-1);
		return outputValues;
	}
	
	void RSGISCalcCovariance::reset()
	{
		this->n = 0;
		this->sum = 0;
	}
	
}}

