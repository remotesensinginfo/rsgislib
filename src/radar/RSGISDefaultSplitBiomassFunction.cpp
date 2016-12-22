/*
 *  RSGISDefaultSplitBiomassFunction.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 12/12/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#include "RSGISDefaultSplitBiomassFunction.h"

namespace rsgis{namespace radar{
	
	RSGISDefaultSplitBiomassFunction::RSGISDefaultSplitBiomassFunction(double coefAGT, double coefBGT, double coefALT, double coefBLT, double split) : rsgis::math::RSGISMathFunction()
	{
		this->coefAGT = coefAGT;
		this->coefBGT = coefBGT;
		this->coefALT = coefALT;
		this->coefBLT = coefBLT;
		this->split = split;
				
		double bBPartA = (split * (-1)) - coefALT;
		double biomassBelow = pow(10, (log(bBPartA/coefBLT)*(-1)));
		double biomassAbove = pow(10, (coefAGT + (coefBGT * split)));
		if(int(biomassBelow) != int(biomassAbove))
		{
			std::cout << "The equations produce different estimates for biomass at the split, an average will be used" << std::endl;
			std::cout << "Lower Biomass = " << biomassBelow << ", Upper Biomass = " << biomassAbove << std::endl;
		}
		this->bioSplit = (biomassBelow + biomassAbove) / 2;
		std::cout << "Using split of: " << bioSplit << " Mg/ha" << std::endl;
		
	}
	
	double RSGISDefaultSplitBiomassFunction::calcFunction(double value) throw(rsgis::math::RSGISMathException)
	{
		double gamma = 0;
		if(value <= 0)
		{
			value = 1;
		}
		if(value <= bioSplit)
		{
			double gammaGtA = exp((-1)*log10(value));
			gamma =  (-coefBLT * gammaGtA) - coefALT;
		}
		else
		{
			gamma = (log10(value) - coefAGT ) / coefBGT;
		}
		
		return gamma;
	}
	
	RSGISDefaultSplitBiomassFunction::~RSGISDefaultSplitBiomassFunction()
	{
		
	}
	
}}

