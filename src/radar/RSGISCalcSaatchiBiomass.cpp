/*
 *  RSGISCalcSaatchiBiomass.cpp
 *  RSGIS_LIB
 *  
 *  Created by Pete Bunting on 23/04/2008.
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

#include "RSGISCalcSaatchiBiomass.h"

namespace rsgis{namespace radar{
	
	RSGISCalcSaatchiBiomassCrown::RSGISCalcSaatchiBiomassCrown(int numberOutBands, double a0Coef, double a1Coef, double a2Coef, double b1Coef, double b2Coef, double c1Coef, double c2Coef) : RSGISCalcImageValue(numberOutBands)
	{
		this->a0 = a0Coef;
		this->a1 = a1Coef;
		this->a2 = a2Coef;
		this->b1 = b1Coef;
		this->b2 = b2Coef;
		this->c1 = c1Coef;
		this->c2 = c2Coef;
	}
	
	void RSGISCalcSaatchiBiomassCrown::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
		// Input Bands Order:
		// HH
		// HV
		// VV
		// Incidence Angle
		// Local Incidence Angle
		
		if(numBands != 5)
		{
			throw rsgis::img::RSGISImageCalcException("The correct number of bands was not provided.");
		}
		
		// Calculate HV term
		double partA1 = a0;
		double partA2 = a1 * bandValues[1] * cos(bandValues[3]-bandValues[4]);
		double partA3 = a2 * ((bandValues[1] * cos(bandValues[3]-bandValues[4]))*
				(bandValues[1] * cos(bandValues[3]-bandValues[4])));
		double partA = partA1 + partA2 + partA3;
		
		// Calculate HH term (0 when no incidence angle equal to local incidence angle)			
		double partB1 = b1 * bandValues[0] * sin(bandValues[3]-bandValues[4]);
		double partB2 = b2 * ((bandValues[0] * sin(bandValues[3]-bandValues[4]))*
			   (bandValues[0] * sin(bandValues[3]-bandValues[4])));
		double partB = partB1 + partB2;
				
		// Calculate VV term
		double partC1 = c1 * bandValues[2] * cos(bandValues[3]-bandValues[4]);
		double partC2 = c2 * ((bandValues[2] * cos(bandValues[3]-bandValues[4]))*
			   (bandValues[2] * cos(bandValues[3]-bandValues[4])));
		double partC = partC1 + partC2;
				
		double logBio =  partA + partB + partC;
		output[0] = exp(logBio);
	}
	
	void RSGISCalcSaatchiBiomassCrown::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not implemented");
	}
		
	
	void RSGISCalcSaatchiBiomassCrown::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not Implemented");
	}
	
	
	RSGISCalcSaatchiBiomassStem::RSGISCalcSaatchiBiomassStem(int numberOutBands, double a0Coef, double a1Coef, double a2Coef, double b1Coef, double b2Coef, double c1Coef, double c2Coef) : RSGISCalcImageValue(numberOutBands)
	{
		this->a0 = a0Coef;
		this->a1 = a1Coef;
		this->a2 = a2Coef;
		this->b1 = b1Coef;
		this->b2 = b2Coef;
		this->c1 = c1Coef;
		this->c2 = c2Coef;
	}
	
	void RSGISCalcSaatchiBiomassStem::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
		// Bands Order:
		// HH
		// HV
		// VV
		// Incidence Angle
		// Local Incidence Angle
		
		if(numBands != 5)
		{
			throw rsgis::img::RSGISImageCalcException("The correct number of bands was not provided.");
		}
		
		// Calculate HV term (0 when no incidence angle equal to local incidence angle)	
		double partA0 = a0;
		double partA1 = a1 * bandValues[1] * sin(bandValues[3]-bandValues[4]);
		double partA2 = a2 * ((bandValues[1] * sin(bandValues[3]-bandValues[4]))*
			   (bandValues[1] * sin(bandValues[3]-bandValues[4])));
		double partA = partA0 + partA1 + partA2;
		
		// Calculate HH term
		double partB1 = b1 * bandValues[0] * cos(bandValues[3]-bandValues[4]);
		double partB2 = b2 * ((bandValues[0] * cos(bandValues[3]-bandValues[4]))*
			   (bandValues[0] * cos(bandValues[3]-bandValues[4])));
		double partB = partB1 + partB2;
		
		// Calculate VV term
		double partC1 = c1 * bandValues[2] * cos(bandValues[3]-bandValues[4]);
		double partC2 = c2 * ((bandValues[2] * cos(bandValues[3]-bandValues[4]))*
			   (bandValues[2] * cos(bandValues[3]-bandValues[4])));
		double partC = partC1 + partC2;
		
		double locBio = partA + partB + partC;
		
		output[0] = exp(locBio);
	}
	
	void RSGISCalcSaatchiBiomassStem::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not implemented");
	}
	
	void RSGISCalcSaatchiBiomassStem::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not Implemented");
	}
	
	
	RSGISCalcSaatchiBiomassCrownPL::RSGISCalcSaatchiBiomassCrownPL(int numberOutBands, double a0Coef, double a1Coef, double a2Coef, double b1Coef, double b2Coef) : RSGISCalcImageValue(numberOutBands)
	{
		this->a0 = a0Coef;
		this->a1 = a1Coef;
		this->a2 = a2Coef;
		this->b1 = b1Coef;
		this->b2 = b2Coef;
	}
	
	void RSGISCalcSaatchiBiomassCrownPL::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
		
		// Bands Order:
		// LHH
		// LHV
		// LVV
		// PHH
		// PHV
		// PVV
		// Incidence Angle
		// Local Incidence Angle
		
		if(numBands != 8)
		{
			throw rsgis::img::RSGISImageCalcException("The correct number of bands was not provided.");
		}
		
		// Calculate L-band term
		double partA1 = a0;
		double partA2 = a1 * bandValues[1] * cos(bandValues[6]-bandValues[7]);
		double partA3 = a2 * ((bandValues[1] * cos(bandValues[6]-bandValues[7]))*
			   (bandValues[1] * cos(bandValues[6]-bandValues[7])));
		double partA = partA1 + partA2 + partA3;
		
		// Calculate P-band term
		double partB1 = b1 * bandValues[4] * cos(bandValues[6]-bandValues[7]);
		double partB2 =	b2 * ((bandValues[4] * cos(bandValues[6]-bandValues[7]))*
			   (bandValues[4] * cos(bandValues[6]-bandValues[7])));
		double partB = partB1 + partB2;
		
		double logbio = partA + partB;
		
		output[0] = exp(logbio);
	}
	
	void RSGISCalcSaatchiBiomassCrownPL::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not implemented");
	}
	
	void RSGISCalcSaatchiBiomassCrownPL::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not Implemented");
	}
	
	RSGISCalcSaatchiNoIABiomassCrown::RSGISCalcSaatchiNoIABiomassCrown(int numberOutBands, double a0Coef, double a1Coef, double a2Coef, double c1Coef, double c2Coef) : RSGISCalcImageValue(numberOutBands)
	{
		this->a0 = a0Coef;
		this->a1 = a1Coef;
		this->a2 = a2Coef;
		this->c1 = c1Coef;
		this->c2 = c2Coef;
	}
	
	void RSGISCalcSaatchiNoIABiomassCrown::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
		// Input Bands Order:
		// HH
		// HV
		// VV
		
		if(numBands < 3) 
        {
			throw rsgis::img::RSGISImageCalcException("The correct number of bands was not provided.");
		}
		
		// Calculate HV term
		double partA1 = a0;
		double partA2 = a1 * bandValues[1];
		double partA3 = a2 * (bandValues[1] * bandValues[1]);
		double partA = partA1 + partA2 + partA3;
				
		// Calculate VV term
		double partC1 = c1 * bandValues[2];
		double partC2 = c2 * (bandValues[2] * bandValues[2]);
		double partC = partC1 + partC2;
				
		double logBio =  partA + partC;
		output[0] = exp(logBio);
	}
	
	void RSGISCalcSaatchiNoIABiomassCrown::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not implemented");
	}
	
	void RSGISCalcSaatchiNoIABiomassCrown::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not Implemented");
	}
	
	RSGISCalcSaatchiNoIABiomassStem::RSGISCalcSaatchiNoIABiomassStem(int numberOutBands, double a0Coef, double a1Coef, double a2Coef, double b1Coef, double b2Coef, double c1Coef, double c2Coef) : RSGISCalcImageValue(numberOutBands)
	{
		this->a0 = a0Coef;
		this->a1 = a1Coef;
		this->a2 = a2Coef;
		this->b1 = b1Coef;
		this->b2 = b2Coef;
		this->c1 = c1Coef;
		this->c2 = c2Coef;
	}
	
	void RSGISCalcSaatchiNoIABiomassStem::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
		// Bands Order:
		// HH
		// HV
		// VV
		
		if(numBands < 3)
		{
			throw rsgis::img::RSGISImageCalcException("The correct number of bands was not provided.");
		}
		
		// Calculate HV term
		double partA0 = a0;
		double partA1 = a1 * bandValues[1];
		double partA2 = a2 * (bandValues[1] * bandValues[1]);
		double partA = partA0 + partA1 + partA2;
		
		// Calculate HH term
		double partB1 = b1 * bandValues[0];
		double partB2 = b2 * (bandValues[0] * bandValues[0]);
		double partB = partB1 + partB2;
		
		// Calculate VV term
		double partC1 = c1 * bandValues[2];
		double partC2 = c2 * (bandValues[2]*bandValues[2]);
		double partC = partC1 + partC2;
		
		double locBio = partA + partB + partC;
		
		output[0] = exp(locBio);
	}
	
	void RSGISCalcSaatchiNoIABiomassStem::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not implemented");
	}
	
	void RSGISCalcSaatchiNoIABiomassStem::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not Implemented");
	}
	
	RSGISCalcSaatchiNoIABiomassCrownPL::RSGISCalcSaatchiNoIABiomassCrownPL(int numberOutBands, double a0Coef, double a1Coef, double a2Coef, double b1Coef, double b2Coef) : RSGISCalcImageValue(numberOutBands)
	{
		this->a0 = a0Coef;
		this->a1 = a1Coef;
		this->a2 = a2Coef;
		this->b1 = b1Coef;
		this->b2 = b2Coef;
	}
	
	void RSGISCalcSaatchiNoIABiomassCrownPL::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
		
		// Bands Order:
		// LHH
		// LHV
		// LVV
		// PHH
		// PHV
		// PVV
		
		if(numBands < 6)
		{
			throw rsgis::img::RSGISImageCalcException("The correct number of bands was not provided.");
		}
		
		// Calculate L-band term
		double partA1 = a0;
		double partA2 = a1 * bandValues[1];
		double partA3 = a2 * (bandValues[1]*bandValues[1]);
		double partA = partA1 + partA2 + partA3;
		
		// Calculate P-band term
		double partB1 = b1 * bandValues[4];
		double partB2 =	b2 * (bandValues[4]*bandValues[4]);
		double partB = partB1 + partB2;
		
		double logbio = partA + partB;
		
		output[0] = exp(logbio);
	}
	
	void RSGISCalcSaatchiNoIABiomassCrownPL::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not implemented");
	}
	
	void RSGISCalcSaatchiNoIABiomassCrownPL::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not Implemented");
	}
	
	
}}


