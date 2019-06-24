/*
 *  RSGISCalcCorrelationCoefficient.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/07/2008.
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

#include "RSGISCalcCorrelationCoefficient.h"

namespace rsgis{namespace img{
	
	RSGISCalcCC::RSGISCalcCC(int numOutputValues) : RSGISCalcImageSingleValue(numOutputValues)
	{
		this->n = 0;
		this->ab = 0;
		this->a = 0;
		this->b = 0;
		this->aSQ = 0;
		this->bSQ = 0;
	}
	
	void RSGISCalcCC::calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) 
	{
		if(bandA > numBands)
		{
			std::cout << "The band A specificed is larger than the number of available bands." << std::endl;
			throw RSGISImageCalcException("The band A specificed is larger than the number of available bands.");
		}
		else if(bandB > numBands)
		{
			std::cout << "The band B specificed is larger than the number of available bands." << std::endl;
			throw RSGISImageCalcException("The band B specificed is larger than the number of available bands.");
		}
		
		n++;
		a += bandValuesImageA[bandA];
		b += bandValuesImageB[bandB];
		ab += (bandValuesImageA[bandA] * bandValuesImageB[bandB]);
		aSQ += (bandValuesImageA[bandA] * bandValuesImageA[bandA]);
		bSQ += (bandValuesImageB[bandB] * bandValuesImageB[bandB]);
	}
	
	void RSGISCalcCC::calcImageValue(float *bandValuesImageA, int numBands, int band) 
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISCalcCC::calcImageValue(float *bandValuesImageA, int numBands, geos::geom::Envelope *extent) 
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISCalcCC::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) 
	{
		throw RSGISImageCalcException("Not implemented!");
	}
	
	double* RSGISCalcCC::getOutputValues()  
	{
		double partA = n * ab;
		double partB = a * b;
		double topline = partA - partB;
				
		double partC = (n * aSQ) - (a * a);
		double partD = (n * bSQ) - (b * b);
		double bottomline = sqrt(partC * partD);
        
		this->outputValues[0] = topline/bottomline;
		return this->outputValues;
	}
	
	void RSGISCalcCC::reset()
	{
		this->n = 0;
		this->ab = 0;
		this->a = 0;
		this->b = 0;
		this->aSQ = 0;
		this->bSQ = 0;
	}

    
    
}}
