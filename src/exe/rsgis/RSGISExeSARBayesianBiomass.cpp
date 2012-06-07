/*
 *  RSGISExeSARBayesianBiomass.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 16/12/2008.
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

#include "RSGISExeSARBayesianBiomass.h"

namespace rsgisexe{

RSGISExeSARBayesianBiomass::RSGISExeSARBayesianBiomass() : RSGISAlgorithmParameters()
{
	this->algorithm = "BayesianBiomass";
	this->inputImage = "";
	this->outputImage = "";
	this->option = RSGISExeSARBayesianBiomass::none;
	this->coefAGT = 0;
	this->coefBGT = 0;
	this->coefALT = 0;
	this->coefBLT = 0;
	this->split = 0;
	this->variance = 0;
	this->upperLimit = 0.9;
	this->lowerLimit = 0.1;
	this->interval = 1; // Set default value for interval as 1
	this->minVal = 1; // Set the minimum biomass as 0, can't be lower than this value
	this->maxVal = 400; // Set the default maximum biomass as 500	
}

RSGISAlgorithmParameters* RSGISExeSARBayesianBiomass::getInstance()
{
	return new RSGISExeSARBayesianBiomass();
}

void RSGISExeSARBayesianBiomass::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *typeNoPrior = XMLString::transcode("noPrior");
	XMLCh *typePriorNormal = XMLString::transcode("priorNormal");
	XMLCh *typePriorExp = XMLString::transcode("priorExp");
	XMLCh *typePriorGamma = XMLString::transcode("priorGamma");
	XMLCh *typeNoPriorOpenLHH = XMLString::transcode("noPriorOpenLHH");
	XMLCh *typeNoPriorOpenLHV = XMLString::transcode("noPriorOpenLHV");
	XMLCh *typeNoPriorOpenLVV = XMLString::transcode("noPriorOpenLVV");
	XMLCh *typeNoPriorOpenPHH = XMLString::transcode("noPriorOpenPHH");
	XMLCh *typeNoPriorOpenPHV = XMLString::transcode("noPriorOpenPHV");
	XMLCh *typeNoPriorOpenPVV = XMLString::transcode("noPriorOpenPVV");
	XMLCh *typeNoPriorClosedLHH = XMLString::transcode("noPriorClosedLHH");
	XMLCh *typeNoPriorClosedLHV = XMLString::transcode("noPriorClosedLHV");
	XMLCh *typeNoPriorClosedLVV = XMLString::transcode("noPriorClosedLVV");
	XMLCh *typeNoPriorClosedPHH = XMLString::transcode("noPriorClosedPHH");
	XMLCh *typeNoPriorClosedPHV = XMLString::transcode("noPriorClosedPHV");
	XMLCh *typeNoPriorClosedPVV = XMLString::transcode("noPriorClosedPVV");
	XMLCh *typeNoPriorClosedPHVLeToan = XMLString::transcode("noPriorClosedPHVLeToan");
	
	const XMLCh *algorNameEle = argElement->getAttribute(XMLString::transcode("algor"));
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	
	// Set method for determining confidence limits
	const XMLCh *typeDeltaProb = XMLString::transcode("prob");
	const XMLCh *typeDeltaArea = XMLString::transcode("area");
	const XMLCh *deltatypeStr = argElement->getAttribute(XMLString::transcode("deltatype"));
	
	if(XMLString::equals(typeDeltaProb, deltatypeStr))
	{
		this->deltatype = prob;
		cout << "\tdelta type = prob" << endl;
	}
	else if(XMLString::equals(typeDeltaArea, deltatypeStr))
	{
		this->deltatype = area;
		cout << "\tdelta type = area" << endl;
	}
	else
	{
		cout << "Delta type either not defined or recognised therefore using a DEFAULT value of area.\n";
		this->deltatype = area;
	}
	
	// Retrieve input and output image and type
	XMLCh *inputXMLStr = XMLString::transcode("input");
	if(argElement->hasAttribute(inputXMLStr))
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
		this->inputImage = string(charValue);
		XMLString::release(&charValue);
	}
	else
	{
		throw RSGISXMLArgumentsException("Input images not provided..");
	}
	XMLString::release(&inputXMLStr);
	
	XMLCh *outputXMLStr = XMLString::transcode("output");
	if(argElement->hasAttribute(outputXMLStr))
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
		this->outputImage = string(charValue);
		XMLString::release(&charValue);
	}
	else
	{
		throw RSGISXMLArgumentsException("Output image not provided..");
	}
	XMLString::release(&outputXMLStr);
	
	const XMLCh *optionStr = argElement->getAttribute(XMLString::transcode("option"));
	
	XMLCh *coefAGTStr = XMLString::transcode("coefAGT");
	XMLCh *coefBGTStr = XMLString::transcode("coefBGT");
	XMLCh *coefALTStr = XMLString::transcode("coefALT");
	XMLCh *coefBLTStr = XMLString::transcode("coefBLT");
	XMLCh *splitStr = XMLString::transcode("split");
	XMLCh *varianceStr = XMLString::transcode("variance");
	XMLCh *upperLimitStr = XMLString::transcode("upperLimit");
	XMLCh *lowerLimitStr = XMLString::transcode("lowerLimit");
	XMLCh *maxValStr = XMLString::transcode("maxVal");
	//XMLCh *minValStr = XMLString::transcode("minVal");
	XMLCh *intervalStr = XMLString::transcode("interval");
	
	/*
	 this->variance = mathUtils.strtodouble(XMLString::transcode(varianceStr));
	 this->upperLimit = mathUtils.strtodouble(XMLString::transcode(upperLimitStr));
	 this->lowerLimit = mathUtils.strtodouble(XMLString::transcode(lowerLimitStr));
	 this->interval = mathUtils.strtodouble(XMLString::transcode(intervalStr));
	 */
	
	if(argElement->hasAttribute(varianceStr)) // Varience
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(varianceStr));
		this->variance = mathUtils.strtofloat(string(charValue));
		XMLString::release(&charValue);
	}
	else
	{
		cout << "\tNo value for varience provided, defaulting to one" << endl;
		this->variance = 1;
	}
	XMLString::release(&varianceStr);
	if(argElement->hasAttribute(lowerLimitStr)) // Lower Limit
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(lowerLimitStr));
		this->lowerLimit = mathUtils.strtofloat(string(charValue));
		XMLString::release(&charValue);
	}
	else
	{
		cout << "\tNo lower confidence level defined, using default of 0.1" <<endl;
		this->lowerLimit = 0.1;
	}
	XMLString::release(&lowerLimitStr);	
	if(argElement->hasAttribute(upperLimitStr)) // Upper Limit
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(upperLimitStr));
		this->upperLimit = mathUtils.strtofloat(string(charValue));
		XMLString::release(&charValue);
	}
	else
	{
		cout << "\tNo upper confidence level defined, using default of 0.9" <<endl;
		this->upperLimit = 0.9;
	}
	XMLString::release(&upperLimitStr);
	if(argElement->hasAttribute(maxValStr)) // Max Value
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(maxValStr));
		this->maxVal = mathUtils.strtofloat(string(charValue));
		XMLString::release(&charValue);
	}
	else
	{
		cout << "\tUsing default upper biomass limit of 400 Mg/ha" <<endl;
	}
	XMLString::release(&maxValStr);
	if(argElement->hasAttribute(intervalStr)) // Interval
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(intervalStr));
		this->interval = mathUtils.strtofloat(string(charValue));
		XMLString::release(&charValue);
	}
	else
	{
		this->interval = 1;
	}
	XMLString::release(&intervalStr);
	
	cout << "\tOptionStr: " << XMLString::transcode(optionStr) << endl;
	
	if(XMLString::equals(typeNoPrior, optionStr))
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		// Read coefficients and check there is a value
		if(argElement->hasAttribute(coefAGTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefAGTStr));
			this->coefAGT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefAGT was provided");
		}
		XMLString::release(&coefAGTStr);
		if(argElement->hasAttribute(coefBGTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefBGTStr));
			this->coefBGT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefBGT was provided");
		}
		XMLString::release(&coefBGTStr);	
		if(argElement->hasAttribute(coefALTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefALTStr));
			this->coefALT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefALT was provided");
		}
		XMLString::release(&coefALTStr);
		if(argElement->hasAttribute(coefBLTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefBLTStr));
			this->coefBLT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefBLT was provided");
		}
		XMLString::release(&coefBLTStr);
		if(argElement->hasAttribute(splitStr)) // Split
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(splitStr));
			this->split = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for split was provided");
		}
		XMLString::release(&splitStr);
	}
	else if(XMLString::equals(typePriorNormal, optionStr))
	{
		this->option = RSGISExeSARBayesianBiomass::priorNormal;
		// Read coefficients and check there is a value
		if(argElement->hasAttribute(coefAGTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefAGTStr));
			this->coefAGT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefAGT was provided");
		}
		XMLString::release(&coefAGTStr);
		if(argElement->hasAttribute(coefBGTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefBGTStr));
			this->coefBGT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefBGT was provided");
		}
		XMLString::release(&coefBGTStr);	
		if(argElement->hasAttribute(coefALTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefALTStr));
			this->coefALT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefALT was provided");
		}
		XMLString::release(&coefALTStr);
		if(argElement->hasAttribute(coefBLTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefBLTStr));
			this->coefBLT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefBLT was provided");
		}
		XMLString::release(&coefBLTStr);
		if(argElement->hasAttribute(splitStr)) // Split
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(splitStr));
			this->split = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for split was provided");
		}
		XMLString::release(&splitStr);
		
		// Mean biomass
		XMLCh *meanBStr = XMLString::transcode("meanB");
		if(argElement->hasAttribute(meanBStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(meanBStr));
			this->meanB = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for mean biomass was provided");
		}
		XMLString::release(&meanBStr);
		
		// StDev of Biomass
		XMLCh *stdevBStr = XMLString::transcode("stdevB");
		if(argElement->hasAttribute(stdevBStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(stdevBStr));
			this->meanB = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for standard deviation of biomass was provided");
		}
		XMLString::release(&stdevBStr);
		cout << "Reading Coefficients" << endl;
	}	
	else if(XMLString::equals(typePriorExp, optionStr))
	{
		this->option = RSGISExeSARBayesianBiomass::priorExp;
		// Read coefficients and check there is a value
		if(argElement->hasAttribute(coefAGTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefAGTStr));
			this->coefAGT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefAGT was provided");
		}
		XMLString::release(&coefAGTStr);
		if(argElement->hasAttribute(coefBGTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefBGTStr));
			this->coefBGT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefBGT was provided");
		}
		XMLString::release(&coefBGTStr);	
		if(argElement->hasAttribute(coefALTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefALTStr));
			this->coefALT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefALT was provided");
		}
		XMLString::release(&coefALTStr);
		if(argElement->hasAttribute(coefBLTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefBLTStr));
			this->coefBLT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefBLT was provided");
		}
		XMLString::release(&coefBLTStr);
		if(argElement->hasAttribute(splitStr)) // Split
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(splitStr));
			this->split = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for split was provided");
		}
		XMLString::release(&splitStr);
		// Rate
		XMLCh *rateBStr = XMLString::transcode("rateB");
		if(argElement->hasAttribute(rateBStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rateBStr));
			this->rateB = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for Biomass rate was provided");
		}
		XMLString::release(&rateBStr);
	}
	else if(XMLString::equals(typePriorGamma, optionStr))
	{
		this->option = RSGISExeSARBayesianBiomass::priorGamma;
		// Read coefficients and check there is a value
		if(argElement->hasAttribute(coefAGTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefAGTStr));
			this->coefAGT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefAGT was provided");
		}
		XMLString::release(&coefAGTStr);
		if(argElement->hasAttribute(coefBGTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefBGTStr));
			this->coefBGT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefBGT was provided");
		}
		XMLString::release(&coefBGTStr);	
		if(argElement->hasAttribute(coefALTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefALTStr));
			this->coefALT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefALT was provided");
		}
		XMLString::release(&coefALTStr);
		if(argElement->hasAttribute(coefBLTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(coefBLTStr));
			this->coefBLT = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for coefBLT was provided");
		}
		XMLString::release(&coefBLTStr);
		if(argElement->hasAttribute(splitStr)) // Split
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(splitStr));
			this->split = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for split was provided");
		}
		XMLString::release(&splitStr);
		// Shape
		XMLCh *shapeBStr = XMLString::transcode("shapeB");
		if(argElement->hasAttribute(shapeBStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(shapeBStr));
			this->shapeB = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for shape was provided");
		}
		XMLString::release(&shapeBStr);
		// Scale
		XMLCh *scaleBStr = XMLString::transcode("scaleB");
		if(argElement->hasAttribute(scaleBStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(scaleBStr));
			this->scaleB = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value for scale was provided");
		}
		XMLString::release(&scaleBStr);
	}
	else if(XMLString::equals(typeNoPriorOpenLHH, optionStr)) /// Use coefficeints for open forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing coefficients for L-band HH, open forest" << endl;
		this->coefAGT=2.9909;
		this->coefBGT=0.1174;
		this->coefALT=6.1633;
		this->coefBLT=21.403;
		this->split=-40;
	}	
	else if(XMLString::equals(typeNoPriorOpenLVV, optionStr)) /// Use coefficeints for open forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing coefficients for L-band VV, open forest" << endl;
		this->coefAGT=3.7943;
		this->coefBGT=0.1393; 
		this->coefALT=9.6046;
		this->coefBLT=24.103;
		this->split=-40;
	}
	else if(XMLString::equals(typeNoPriorOpenLHV, optionStr)) /// Use coefficeints for open forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing coefficients for L-band HV, open forest" << endl;
		this->coefAGT=3.5861;
		this->coefBGT=0.0953;
		this->coefALT=12.3831;
		this->coefBLT=32.8825;
		this->split=-40; 
	}
	else if(XMLString::equals(typeNoPriorOpenPHH, optionStr)) /// Use coefficeints for open forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing coefficients for P-band HH, open forest" << endl;
		this->coefAGT=2.6421;
		this->coefBGT=0.0292;
		this->coefALT=4.9357;
		this->coefBLT=20.9475;
		this->split=-40;
	}
	else if(XMLString::equals(typeNoPriorOpenPVV, optionStr)) /// Use coefficeints for open forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing coefficients for P-band VV, open forest" << endl;
		this->coefAGT=5.8346;
		this->coefBGT=0.1273;
		this->coefALT=10.0723;
		this->coefBLT=24.3553;
		this->split=-40;
	}
	else if(XMLString::equals(typeNoPriorOpenPHV, optionStr)) /// Use coefficeints for open forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing coefficients for P-band HV, open forest" << endl;
		this->coefAGT=3.7629;
		this->coefBGT=0.0917;
		this->coefALT=14.5024;
		this->coefBLT=35.69;
		this->split=-40;
	}
	else if(XMLString::equals(typeNoPriorClosedLHH, optionStr)) /// Use coefficeints for closed forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing coefficients for L-band HH, closed forest" << endl;
		this->coefAGT=4.112887686;
		this->coefBGT=0.35805;
		this->coefALT=3.53;
		this->coefBLT=15.58;
		this->split=-8.0;
	}	
	else if(XMLString::equals(typeNoPriorClosedLVV, optionStr)) /// Use coefficeints for closed forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing coefficients for L-band VV, closed forest" << endl;
		this->coefAGT=4.51355858;
		this->coefBGT=0.44021;
		this->coefALT=3.38;
		this->coefBLT=15.15;
		this->split=-7.0;
	}
	else if(XMLString::equals(typeNoPriorClosedLHV, optionStr)) /// Use coefficeints for closed forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing coefficients for L-band HV, closed forest" << endl;
		this->coefAGT=5.421108362;
		this->coefBGT=0.31216;
		this->coefALT=7.45;
		this->coefBLT=24.31;
		this->split=-12.0;
	}
	else if(XMLString::equals(typeNoPriorClosedPHH, optionStr)) /// Use coefficeints for closed forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing coefficients for P-band HH, closed forest" << endl;
		this->coefAGT=3.539210149;
		this->coefBGT=0.20507;
		this->coefALT=4.77;
		this->coefBLT=18.25;
		this->split=-7.0; 
	}
	else if(XMLString::equals(typeNoPriorClosedPVV, optionStr)) /// Use coefficeints for closed forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing coefficients for P-band VV, closed forest" << endl;
		this->coefAGT=4.769536659;
		this->coefBGT=0.36644;
		this->coefALT=5.12;
		this->coefBLT=17.08;
		this->split=-7.0;
	}
	else if(XMLString::equals(typeNoPriorClosedPHV, optionStr)) /// Use coefficeints for open forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing coefficients for P-band HV, closed forest" << endl;
		this->coefAGT=4.880450875;
		this->coefBGT=0.18876;
		this->coefALT=9.81;
		this->coefBLT=30.00;
		this->split=-12;
	}
	else if(XMLString::equals(typeNoPriorClosedPHVLeToan, optionStr)) /// Use coefficeints for open forest derived by Lucas et al.
	{
		this->option = RSGISExeSARBayesianBiomass::noPrior;
		cout << "\tUsing LeToan coefficients for P-band HV, closed forest" << endl;
		this->coefAGT=4.5563;
		this->coefBGT=0.18;
		this->coefALT=10;
		this->coefBLT=32.00;
		this->split=-15.0;
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionStr)) + string(") is not known: RSGISSARBayesianBiomass.");
		throw RSGISXMLArgumentsException(message.c_str());
	}	
	
	parsed = true;
	
	// Release XML
	XMLString::release(&algorName);
	XMLString::release(&typeNoPrior);
	XMLString::release(&typePriorNormal);
	XMLString::release(&typePriorExp);
	XMLString::release(&typePriorGamma);
	XMLString::release(&typeNoPriorOpenLHH);
	XMLString::release(&typeNoPriorOpenLHV);
	XMLString::release(&typeNoPriorOpenLVV);
	XMLString::release(&typeNoPriorOpenPHH);
	XMLString::release(&typeNoPriorOpenPHV);
	XMLString::release(&typeNoPriorOpenPVV);
	XMLString::release(&typeNoPriorClosedLHH);
	XMLString::release(&typeNoPriorClosedLHV);
	XMLString::release(&typeNoPriorClosedLVV);
	XMLString::release(&typeNoPriorClosedPHH);
	XMLString::release(&typeNoPriorClosedPHV);
	XMLString::release(&typeNoPriorClosedPVV);
	XMLString::release(&typeNoPriorClosedPHVLeToan);
}

void RSGISExeSARBayesianBiomass::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		
		GDALAllRegister();
		GDALDataset **datasets = NULL;
		
		try
		{
			datasets = new GDALDataset*[1];
			cout << this->inputImage << endl;
			datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
			if(datasets[0] == NULL)
			{
				string message = string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
		}
		catch(RSGISImageException e)
		{
			throw e;
		}		
		
		if(this->option == RSGISExeSARBayesianBiomass::noPrior)
		{
			RSGISCalcImage *calcImg = NULL;
			RSGISDefaultSplitBiomassFunction *biomassFunction = NULL;
			RSGISImageCalcValueBaysianNoPrior * baysian = NULL;
			try
			{
				biomassFunction = new RSGISDefaultSplitBiomassFunction(this->coefAGT, this->coefBGT, this->coefALT, this->coefBLT, this->split);
				baysian = new RSGISImageCalcValueBaysianNoPrior(5, biomassFunction, this->variance, this->interval, this->minVal, this->maxVal, this->lowerLimit, this->upperLimit, this->deltatype);
				calcImg = new RSGISCalcImage(baysian, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
				
				delete baysian;
				delete biomassFunction;
				delete calcImg;
			}
			catch(RSGISException e)
			{
				throw e;
			}
			
		}
		else if(this->option == RSGISExeSARBayesianBiomass::priorNormal)
		{
			RSGISCalcImage *calcImg = NULL;
			RSGISDefaultSplitBiomassFunction *biomassFunction = NULL;
			RSGISImageCalcValueBaysianPrior * baysian = NULL;
			RSGISProbDistro *priorDistro = NULL;
			
			try
			{
				biomassFunction = new RSGISDefaultSplitBiomassFunction(this->coefAGT, this->coefBGT, this->coefALT, this->coefBLT, this->split);
				priorDistro = new RSGISProbDistNormal(this->meanB, this->stdevB);
				baysian = new RSGISImageCalcValueBaysianPrior(5, biomassFunction, priorDistro, this->variance, this->interval, this->minVal, this->maxVal, this->lowerLimit, this->upperLimit, this->deltatype);
				calcImg = new RSGISCalcImage(baysian, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
				
				delete baysian;
				delete priorDistro;
				delete biomassFunction;
				delete calcImg;
			}
			catch(RSGISException e)
			{
				throw e;
			}
			
		}
		else if(this->option == RSGISExeSARBayesianBiomass::priorExp)
		{
			RSGISCalcImage *calcImg = NULL;
			RSGISDefaultSplitBiomassFunction *biomassFunction = NULL;
			RSGISImageCalcValueBaysianPrior * baysian = NULL;
			RSGISProbDistExponential *priorDistro = NULL;
			try
			{
				biomassFunction = new RSGISDefaultSplitBiomassFunction(this->coefAGT, this->coefBGT, this->coefALT, this->coefBLT, this->split);
				priorDistro = new RSGISProbDistExponential(this->rateB);
				baysian = new RSGISImageCalcValueBaysianPrior(5, biomassFunction, priorDistro, this->variance, this->interval, this->minVal, this->maxVal, this->lowerLimit, this->upperLimit, this->deltatype);
				calcImg = new RSGISCalcImage(baysian, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
				
				delete baysian;
				delete priorDistro;
				delete biomassFunction;
				delete calcImg;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(this->option == RSGISExeSARBayesianBiomass::priorGamma)
		{
			RSGISCalcImage *calcImg = NULL;
			RSGISDefaultSplitBiomassFunction *biomassFunction = NULL;
			RSGISImageCalcValueBaysianPrior *baysian = NULL;
			RSGISProbDistGamma *priorDistro = NULL;
			try
			{
				biomassFunction = new RSGISDefaultSplitBiomassFunction(this->coefAGT, this->coefBGT, this->coefALT, this->coefBLT, this->split);
				priorDistro = new RSGISProbDistGamma(this->shapeB, this->scaleB);
				baysian = new RSGISImageCalcValueBaysianPrior(5, biomassFunction, priorDistro, this->variance, this->interval, this->minVal, this->maxVal, this->lowerLimit, this->upperLimit, this->deltatype);
				calcImg = new RSGISCalcImage(baysian, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
				
				delete baysian;
				delete priorDistro;
				delete biomassFunction;
				delete calcImg;
			}
			catch(RSGISException e)
			{
				throw e;
			} 
		}
		else
		{
			cout << "Unknown Option\n";
		}
		
		if(datasets != NULL)
		{
			GDALClose(datasets[0]);
			delete[] datasets;
		}
		
	}
	
}

void RSGISExeSARBayesianBiomass::printParameters()
{
	if(parsed)
	{		
		if(this->option == RSGISExeSARBayesianBiomass::noPrior)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			
			cout << "coefAGT = " << this->coefAGT << endl;
			cout << "coefBGT = " << this->coefBGT << endl;
			cout << "coefALT = " << this->coefALT << endl;
			cout << "coefBLT = " << this->coefBLT << endl;		
			cout << "Split = " << this->split << endl;
			cout << "Variance = " << this->variance << endl;
			cout << "Interval = " << this->interval << endl;
			cout << "Upper confidence limit = " << this->upperLimit << endl;
			cout << "Lower confidence limit = " << this->lowerLimit << endl;
			cout << "Method for generating confidence limits = " << this->deltatype << endl;
		}
		
		if(this->option == RSGISExeSARBayesianBiomass::priorNormal)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			
			cout << "coefAGT = " << this->coefAGT << endl;
			cout << "coefBGT = " << this->coefBGT << endl;
			cout << "coefALT = " << this->coefALT << endl;
			cout << "coefBLT = " << this->coefBLT << endl;		
			cout << "Split = " << this->split << endl;
			cout << "Variance = " << this->variance << endl;
			cout << "Interval = " << this->interval << endl;
			cout << "Average biomass (prior) = " << this->meanB << endl;
			cout << "Biomass standard deviation (prior) = " << this->stdevB << endl;
			cout << "Upper confidence limit = " << this->upperLimit << endl;
			cout << "Lower confidence limit = " << this->lowerLimit << endl;
			cout << "Method for generating confidence limits = " << this->deltatype << endl;
		}
		
		if(this->option == RSGISExeSARBayesianBiomass::priorExp)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			
			cout << "coefAGT = " << this->coefAGT << endl;
			cout << "coefBGT = " << this->coefBGT << endl;
			cout << "coefALT = " << this->coefALT << endl;
			cout << "coefBLT = " << this->coefBLT << endl;		
			cout << "Split = " << this->split << endl;
			cout << "Variance = " << this->variance << endl;
			cout << "Interval = " << this->interval << endl;
			cout << "Rate (prior) = " << this->rateB << endl;
			cout << "Upper confidence limit = " << this->upperLimit << endl;
			cout << "Lower confidence limit = " << this->lowerLimit << endl;
			cout << "Method for generating confidence limits = " << this->deltatype << endl;
		}
		
		if(this->option == RSGISExeSARBayesianBiomass::priorGamma)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			
			cout << "coefAGT = " << this->coefAGT << endl;
			cout << "coefBGT = " << this->coefBGT << endl;
			cout << "coefALT = " << this->coefALT << endl;
			cout << "coefBLT = " << this->coefBLT << endl;		
			cout << "Split = " << this->split << endl;
			cout << "Variance = " << this->variance << endl;
			cout << "Interval = " << this->interval << endl;
			cout << "Shape (prior) = " << this->shapeB << endl;
			cout << "Scale (prior) = " << this->scaleB << endl;
			cout << "Upper confidence limit = " << this->upperLimit << endl;
			cout << "Lower confidence limit = " << this->lowerLimit << endl;
			cout << "Method for generating confidence limits = " << this->deltatype << endl;
		}
		
		else
		{
			cout << "Option unknown\n";
		}
	}
	
	else		
	{
		cout << "The parameters have yet to be parsed\n";
	}
}

void RSGISExeSARBayesianBiomass::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPrior\"  deltatype=\"area\" input=\"input image\" output = \"output image\" \n";
	cout <<  "\t\t coefAGT=\"coefAGT\" coefBGT=\"coefBGT\" coefALT=\"coefALT\" coefBLT=\"coefBLT\" \n";
	cout <<  "\t\t split=\"split\" variance=\"variance\" upperLimit=\"upperLimit\" lowerLimit=\"lowerLimit\" interval=\"interval (optional)\" /> \n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPrior\"  deltatype=\"prob\" input=\"input image\" output = \"output image\" \n";
	cout <<  "\t\t coefAGT=\"coefAGT\" coefBGT=\"coefBGT\" coefALT=\"coefALT\" coefBLT=\"coefBLT\" \n";
	cout <<  "\t\t split=\"split\" variance=\"variance\" upperLimit=\"upperLimit\" lowerLimit=\"lowerLimit\" interval=\"interval (optional)\" /> \n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"priorNorm\"  deltatype=\"area\" input=\"input image\" output = \"output image\" \n";
	cout <<  "\t\t coefAGT=\"coefAGT\" coefBGT=\"coefBGT\" coefALT=\"coefALT\" coefBLT=\"coefBLT\" meanB=\"meanB\" stdevB=\"stdevB\" \n";
	cout <<  "\t\t split=\"split\" variance=\"variance\" upperLimit=\"upperLimit\" lowerLimit=\"lowerLimit\" interval=\"interval (optional)\" /> \n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"priorNorm\"  deltatype=\"prob\" input=\"input image\" output = \"output image\" \n";
	cout <<  "\t\t coefAGT=\"coefAGT\" coefBGT=\"coefBGT\" coefALT=\"coefALT\" coefBLT=\"coefBLT\" meanB=\"meanB\" stdevB=\"stdevB\" \n";
	cout <<  "\t\t split=\"split\" variance=\"variance\" upperLimit=\"upperLimit\" lowerLimit=\"lowerLimit\" interval=\"interval (optional)\" /> \n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"priorExp\"  deltatype=\"area\" input=\"input image\" output = \"output image\" \n";
	cout <<  "\t\t coefAGT=\"coefAGT\" coefBGT=\"coefBGT\" coefALT=\"coefALT\" coefBLT=\"coefBLT\" rateB=\"rateB\" \n";
	cout <<  "\t\t split=\"split\" variance=\"variance\" upperLimit=\"upperLimit\" lowerLimit=\"lowerLimit\" interval=\"interval (optional)\" /> \n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"priorExp\"  deltatype=\"prob\" input=\"input image\" output = \"output image\" \n";
	cout <<  "\t\t coefAGT=\"coefAGT\" coefBGT=\"coefBGT\" coefALT=\"coefALT\" coefBLT=\"coefBLT\" rateB=\"rateB\" \n";
	cout <<  "\t\t split=\"split\" variance=\"variance\" upperLimit=\"upperLimit\" lowerLimit=\"lowerLimit\" interval=\"interval (optional)\" /> \n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"priorGamma\"  deltatype=\"area\" input=\"input image\" output = \"output image\" \n";
	cout <<  "\t\t coefAGT=\"coefAGT\" coefBGT=\"coefBGT\" coefALT=\"coefALT\" coefBLT=\"coefBLT\" shapeB=\"shapeB\" scaleB=\"scaleB\" \n";
	cout <<  "\t\t split=\"split\" variance=\"variance\" upperLimit=\"upperLimit\" lowerLimit=\"lowerLimit\" interval=\"interval (optional)\" /> \n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"priorGamma\"  deltatype=\"prob\" input=\"input image\" output = \"output image\" \n";
	cout <<  "\t\t coefAGT=\"coefAGT\" coefBGT=\"coefBGT\" coefALT=\"coefALT\" coefBLT=\"coefBLT\" shapeB=\"shapeB\" scaleB=\"scaleB\" \n";
	cout <<  "\t\t split=\"split\" variance=\"variance\" upperLimit=\"upperLimit\" lowerLimit=\"lowerLimit\" interval=\"interval (optional)\" /> \n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPriorOpenLHH\" input=\"input image\" output = \"output image\"/>\n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPriorOpenLHV\" input=\"input image\" output = \"output image\"/>\n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPriorOpenLVV\" input=\"input image\" output = \"output image\"/>\n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPriorOpenPHH\" input=\"input image\" output = \"output image\"/>\n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPriorOpenPHV\" input=\"input image\" output = \"output image\"/>\n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPriorOpenPVV\" input=\"input image\" output = \"output image\"/>\n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPriorClosedLHH\" input=\"input image\" output = \"output image\"/>\n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPriorClosedLHV\" input=\"input image\" output = \"output image\"/>\n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPriorClosedLVV\" input=\"input image\" output = \"output image\"/>\n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPriorClosedPHH\" input=\"input image\" output = \"output image\"/>\n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPriorClosedPHV\" input=\"input image\" output = \"output image\"/>\n";
	cout <<  "\t <rsgis:command algor =\"BayesianBiomass\" option=\"noPriorClosedPVV\" input=\"input image\" output = \"output image\"/>\n";
	cout <<  "</rsgiscommands>";
	
}

string RSGISExeSARBayesianBiomass::getDescription()
{
	return "Calculates biomass, and confidence images using the algorithm of LeToan";
}

string RSGISExeSARBayesianBiomass::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeSARBayesianBiomass::~RSGISExeSARBayesianBiomass()
{
	
}
    
}

