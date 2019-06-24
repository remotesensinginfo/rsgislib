/*
 *  RSGISEstimationFPC.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 05/01/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#include "RSGISEstimationFPC.h"

namespace rsgis { namespace radar {

	RSGISEstimationFPCCanopyScattering::RSGISEstimationFPCCanopyScattering(int numberOutBands, rsgis::math::RSGISMathFunction *calcScatteringHH, rsgis::math::RSGISMathFunction *calcScatteringHV, rsgis::math::RSGISMathFunction *calcScatteringVV) : rsgis::img::RSGISCalcImageValue(numberOutBands)
	{
		this->calcScatteringHH = calcScatteringHH;
		this->calcScatteringHV = calcScatteringHV;
		this->calcScatteringVV = calcScatteringVV;
	}
	void RSGISEstimationFPCCanopyScattering::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		double fpc = bandValues[0]; // Get FPC
		output[0] = calcScatteringHH->calcFunction(fpc);
		output[1] = calcScatteringHV->calcFunction(fpc);
		output[2] = calcScatteringVV->calcFunction(fpc);
	}
	void RSGISEstimationFPCCanopyScattering::calcValue(double fpc, double *canopyScattering)
	{
		canopyScattering[0] = calcScatteringHH->calcFunction(fpc);
		canopyScattering[1] = calcScatteringHV->calcFunction(fpc);
		canopyScattering[2] = calcScatteringVV->calcFunction(fpc);
	}
	
	RSGISEstimationFPCCanopyScattering::~RSGISEstimationFPCCanopyScattering()
	{
		
	}
	
	RSGISEstimationFPCCanopyAttenuation::RSGISEstimationFPCCanopyAttenuation(int numberOutBands, rsgis::math::RSGISMathFunction *calcAttenuationH, rsgis::math::RSGISMathFunction *calcAttenuationV) : rsgis::img::RSGISCalcImageValue(numberOutBands)
	{
		this->calcAttenuationH = calcAttenuationH;
		this->calcAttenuationV = calcAttenuationV;
	}
	void RSGISEstimationFPCCanopyAttenuation::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		double fpc = bandValues[0]; // Get FPC
		output[0] = calcAttenuationH->calcFunction(fpc);
		output[1] = calcAttenuationV->calcFunction(fpc);
	}
	void RSGISEstimationFPCCanopyAttenuation::calcValue(double fpc, double *canopyAttenuation)
	{
		canopyAttenuation[0] = calcAttenuationH->calcFunction(fpc);
		canopyAttenuation[1] = calcAttenuationV->calcFunction(fpc);
	}
	RSGISEstimationFPCCanopyAttenuation::~RSGISEstimationFPCCanopyAttenuation()
	{
		
	}
	
	RSGISEstimationFPCCanopyScatteringAttenuation::RSGISEstimationFPCCanopyScatteringAttenuation(int numberOutBands, rsgis::math::RSGISMathFunction *calcScatteringHH, rsgis::math::RSGISMathFunction *calcScatteringHV, rsgis::math::RSGISMathFunction *calcScatteringVV,rsgis::math::RSGISMathFunction *calcAttenuationH, rsgis::math::RSGISMathFunction *calcAttenuationV) : rsgis::img::RSGISCalcImageValue(numberOutBands)
	{
		this->calcScatteringHH = calcScatteringHH;
		this->calcScatteringHV = calcScatteringHV;
		this->calcScatteringVV = calcScatteringVV;
		this->calcAttenuationH = calcAttenuationH;
		this->calcAttenuationV = calcAttenuationV;
	}
	void RSGISEstimationFPCCanopyScatteringAttenuation::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		double fpc = bandValues[0]; // Get FPC
		output[0] = calcScatteringHH->calcFunction(fpc);
		output[1] = calcScatteringHV->calcFunction(fpc);
		output[2] = calcScatteringVV->calcFunction(fpc);
		output[3] = calcAttenuationH->calcFunction(fpc);
		output[4] = calcAttenuationV->calcFunction(fpc);
	}
	RSGISEstimationFPCCanopyScatteringAttenuation::~RSGISEstimationFPCCanopyScatteringAttenuation()
	{
		
	}
	
	RSGISEstimationFPCDualPolTrunkGround::RSGISEstimationFPCDualPolTrunkGround(int numberOutBands, rsgis::math::RSGISMathFunction *calcScatteringHH, rsgis::math::RSGISMathFunction *calcScatteringHV, rsgis::math::RSGISMathFunction *calcAttenuationH, rsgis::math::RSGISMathFunction *calcAttenuationV) : rsgis::img::RSGISCalcImageValue(numberOutBands)
	{
		this->calcScatteringHH = calcScatteringHH;
		this->calcScatteringHV = calcScatteringHV;
		this->calcAttenuationH = calcAttenuationH;
		this->calcAttenuationV = calcAttenuationV;
	}
	void RSGISEstimationFPCDualPolTrunkGround::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		double fpc = bandValues[0];
		double totalHH = bandValues[1];
		double totalHV = bandValues[2];
		
		double canopyScatteringHH = calcScatteringHH->calcFunction(fpc);
		double canopyScatteringHV = calcScatteringHV->calcFunction(fpc);
		double canopyAttenuationH = calcAttenuationH->calcFunction(fpc);
		double canopyAttenuationV = calcAttenuationV->calcFunction(fpc);
		
		double trunkGroundHH = 10 * log10((pow(10,totalHH/10) - pow(10,canopyScatteringHH/10)) / ( pow(10,canopyAttenuationH/10 )));
		double trunkGroundHV = 10 * log10((pow(10,totalHV/10) - pow(10,canopyScatteringHV/10)) / ( pow(10,canopyAttenuationV/10 )));
		
		output[0] = trunkGroundHH;
		output[1] = trunkGroundHV;
		
	}
	void RSGISEstimationFPCDualPolTrunkGround::calcValue(double fpc, double totalHH, double totalHV, double *trunkGround)
	{		
		double canopyScatteringHH = calcScatteringHH->calcFunction(fpc);
		double canopyScatteringHV = calcScatteringHV->calcFunction(fpc);
		double canopyAttenuationH = calcAttenuationH->calcFunction(fpc);
		double canopyAttenuationV = calcAttenuationV->calcFunction(fpc);
		
		double trunkGroundHH = 10 * log10((pow(10,totalHH/10) - pow(10,canopyScatteringHH/10)) / ( pow(10,canopyAttenuationH/10 )));
		double trunkGroundHV = 10 * log10((pow(10,totalHV/10) - pow(10,canopyScatteringHV/10)) / ( pow(10,canopyAttenuationV/10 )));
		
		trunkGround[0] = trunkGroundHH;
		trunkGround[1] = trunkGroundHV;
		
	}
	RSGISEstimationFPCDualPolTrunkGround::~RSGISEstimationFPCDualPolTrunkGround()
	{
		
	}
	
	RSGISEstimationFPCFullPolTrunkGroundDualPol::RSGISEstimationFPCFullPolTrunkGroundDualPol(int numberOutBands, rsgis::math::RSGISMathFunction *calcScatteringHH, rsgis::math::RSGISMathFunction *calcScatteringHV, rsgis::math::RSGISMathFunction *calcScatteringVV, rsgis::math::RSGISMathFunction *calcAttenuationH, rsgis::math::RSGISMathFunction *calcAttenuationV) : rsgis::img::RSGISCalcImageValue(numberOutBands)
	{
		this->calcScatteringHH = calcScatteringHH;
		this->calcScatteringHV = calcScatteringHV;
		this->calcScatteringVV = calcScatteringVV;
		this->calcAttenuationH = calcAttenuationH;
		this->calcAttenuationV = calcAttenuationV;
	}
	void RSGISEstimationFPCFullPolTrunkGroundDualPol::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		double fpc = bandValues[0];
		double totalHH = bandValues[1];
		double totalHV = bandValues[2];
		double totalVV = bandValues[3];
		
		double canopyScatteringHH = calcScatteringHH->calcFunction(fpc);
		double canopyScatteringHV = calcScatteringHV->calcFunction(fpc);
		double canopyScatteringVV = calcScatteringVV->calcFunction(fpc);
		double canopyAttenuationH = calcAttenuationH->calcFunction(fpc);
		double canopyAttenuationV = calcAttenuationV->calcFunction(fpc);
		
		double trunkGroundHH = 10 * log10((pow(10,totalHH/10) - pow(10,canopyScatteringHH/10)) / ( pow(10,canopyAttenuationH/10 )));
		double trunkGroundHV = 10 * log10((pow(10,totalHV/10) - pow(10,canopyScatteringHV/10)) / ( pow(10,canopyAttenuationV/10 )));
		double trunkGroundVV = 10 * log10((pow(10,totalVV/10) - pow(10,canopyScatteringVV/10)) / ( pow(10,canopyAttenuationV/10 )));
		
		output[0] = trunkGroundHH;
		output[1] = trunkGroundHV;
		output[2] = trunkGroundVV;
		
	}
	void RSGISEstimationFPCFullPolTrunkGroundDualPol::calcValue(double fpc, double totalHH, double totalHV, double totalVV, double *trunkGround)
	{
		double canopyScatteringHH = calcScatteringHH->calcFunction(fpc);
		double canopyScatteringHV = calcScatteringHV->calcFunction(fpc);
		double canopyScatteringVV = calcScatteringVV->calcFunction(fpc);
		double canopyAttenuationH = calcAttenuationH->calcFunction(fpc);
		double canopyAttenuationV = calcAttenuationV->calcFunction(fpc);
		
		double trunkGroundHH = 10 * log10((pow(10,totalHH/10) - pow(10,canopyScatteringHH/10)) / ( pow(10,canopyAttenuationH/10 )));
		double trunkGroundHV = 10 * log10((pow(10,totalHV/10) - pow(10,canopyScatteringHV/10)) / ( pow(10,canopyAttenuationV/10 )));
		double trunkGroundVV = 10 * log10((pow(10,totalVV/10) - pow(10,canopyScatteringVV/10)) / ( pow(10,canopyAttenuationV/10 )));
		
		trunkGround[0] = trunkGroundHH;
		trunkGround[1] = trunkGroundHV;
		trunkGround[2] = trunkGroundVV;
	}
	RSGISEstimationFPCFullPolTrunkGroundDualPol::~RSGISEstimationFPCFullPolTrunkGroundDualPol()
	{
		
	}
}}
