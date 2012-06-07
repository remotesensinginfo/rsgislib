/*
 *  RSGISExeSARSaatchiBiomass.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 04/12/2008.
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

#include "RSGISExeSARSaatchiBiomass.h"

namespace rsgisexe{

RSGISExeSARSaatchiBiomass::RSGISExeSARSaatchiBiomass() : RSGISAlgorithmParameters()
{
	this->algorithm = "SaatchiBiomass";
	this->inputImage = "";
	this->outputImage = "";
	this->option = RSGISExeSARSaatchiBiomass::none;
	this->a0 = 0;
	this->a1 = 0;
	this->a2 = 0;
	this->b1 = 0;
	this->b2 = 0;
	this->c1 = 0;
	this->c2 = 0;
}

RSGISAlgorithmParameters* RSGISExeSARSaatchiBiomass::getInstance()
{
	return new RSGISExeSARSaatchiBiomass();
}

void RSGISExeSARSaatchiBiomass::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	
	RSGISMathsUtils mathUtils;
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *typeStem = XMLString::transcode("stem");
	XMLCh *typeCrown = XMLString::transcode("crown");
	XMLCh *typeCrownPL = XMLString::transcode("crownPL");
	XMLCh *typeStemNIA = XMLString::transcode("stemNIA");
	XMLCh *typeCrownNIA = XMLString::transcode("crownNIA");
	XMLCh *typeCrownPLNIA = XMLString::transcode("crownPLNIA");
	XMLCh *typeOpenStemL = XMLString::transcode("openStemL");
	XMLCh *typeOpenStemP = XMLString::transcode("openStemP");
	XMLCh *typeOpenCrownL = XMLString::transcode("openCrownL");
	XMLCh *typeOpenCrownP = XMLString::transcode("openCrownP");
	XMLCh *typeOpenCrownPL = XMLString::transcode("openCrownPL");
	XMLCh *typeClosedStemL = XMLString::transcode("closedStemL");
	XMLCh *typeClosedStemP = XMLString::transcode("closedStemP");
	XMLCh *typeClosedCrownL = XMLString::transcode("closedCrownL");
	XMLCh *typeClosedCrownP = XMLString::transcode("closedCrownP");
	XMLCh *typeClosedCrownPL = XMLString::transcode("closedCrownPL");
	XMLCh *typeClosedLeToanStemP = XMLString::transcode("closedLeToanStemP");
	XMLCh *typeClosedLeToanCrownP = XMLString::transcode("closedLeToanCrownP");
	
	const XMLCh *algorNameEle = argElement->getAttribute(XMLString::transcode("algor"));
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
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
	
	if(XMLString::equals(typeStem, optionStr)) // Stem Biomass, with correction for incidence angle
	{
		this->option = RSGISExeSARSaatchiBiomass::stem;
		cout << "\tCalculating stem biomass" << endl;
		
		XMLCh *a0Str = XMLString::transcode("a0");
		XMLCh *a1Str = XMLString::transcode("a1");
		XMLCh *a2Str = XMLString::transcode("a2");
		XMLCh *b1Str = XMLString::transcode("b1");
		XMLCh *b2Str = XMLString::transcode("b2");
		XMLCh *c1Str = XMLString::transcode("c1");
		XMLCh *c2Str = XMLString::transcode("c2");
		
		if(argElement->hasAttribute(a0Str)) // a0
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a0Str));
			this->a0 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a0 was provided");
		}
		XMLString::release(&a0Str);
		if(argElement->hasAttribute(a1Str)) // a1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a1Str));
			this->a1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a1 was provided");
		}
		XMLString::release(&a1Str);
		if(argElement->hasAttribute(a2Str)) // a2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a2Str));
			this->a2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a2 was provided");
		}
		XMLString::release(&a2Str);
		if(argElement->hasAttribute(b1Str)) // b1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(b1Str));
			this->b1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient b1 was provided");
		}
		XMLString::release(&b1Str);
		if(argElement->hasAttribute(b2Str)) // b2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(b2Str));
			this->b2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient b2 was provided");
		}
		XMLString::release(&b2Str);
		if(argElement->hasAttribute(c1Str)) // c1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(c1Str));
			this->c1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient c1 was provided");
		}
		XMLString::release(&c1Str);
		if(argElement->hasAttribute(c2Str)) // c2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(c2Str));
			this->c2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient c2 was provided");
		}
		XMLString::release(&c2Str);
	}
	else if(XMLString::equals(typeCrown, optionStr)) // Crown Biomass, with correction for incidence angle
	{
		this->option = RSGISExeSARSaatchiBiomass::crown;
		cout << "\tCalculating crown biomass" << endl;
		
		XMLCh *a0Str = XMLString::transcode("a0");
		XMLCh *a1Str = XMLString::transcode("a1");
		XMLCh *a2Str = XMLString::transcode("a2");
		XMLCh *b1Str = XMLString::transcode("b1");
		XMLCh *b2Str = XMLString::transcode("b2");
		XMLCh *c1Str = XMLString::transcode("c1");
		XMLCh *c2Str = XMLString::transcode("c2");
		
		if(argElement->hasAttribute(a0Str)) // a0
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a0Str));
			this->a0 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a0 was provided");
		}
		XMLString::release(&a0Str);
		if(argElement->hasAttribute(a1Str)) // a1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a1Str));
			this->a1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a1 was provided");
		}
		XMLString::release(&a1Str);
		if(argElement->hasAttribute(a2Str)) // a2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a2Str));
			this->a2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a2 was provided");
		}
		XMLString::release(&a2Str);
		if(argElement->hasAttribute(b1Str)) // b1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(b1Str));
			this->b1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient b1 was provided");
		}
		XMLString::release(&b1Str);
		if(argElement->hasAttribute(b2Str)) // b2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(b2Str));
			this->b2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient b2 was provided");
		}
		XMLString::release(&b2Str);
		if(argElement->hasAttribute(c1Str)) // c1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(c1Str));
			this->c1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient c1 was provided");
		}
		XMLString::release(&c1Str);
		if(argElement->hasAttribute(c2Str)) // c2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(c2Str));
			this->c2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient c2 was provided");
		}
		XMLString::release(&c2Str);
	}
	else if(XMLString::equals(typeCrownPL, optionStr)) // Crown Biomass, using L and P bands
	{
		this->option = RSGISExeSARSaatchiBiomass::crownPL; 
		cout << "\tCalculating crown biomass using L and P band" << endl;
		
		XMLCh *a0Str = XMLString::transcode("a0");
		XMLCh *a1Str = XMLString::transcode("a1");
		XMLCh *a2Str = XMLString::transcode("a2");
		XMLCh *b1Str = XMLString::transcode("b1");
		XMLCh *b2Str = XMLString::transcode("b2");
		
		if(argElement->hasAttribute(a0Str)) // a0
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a0Str));
			this->a0 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a0 was provided");
		}
		XMLString::release(&a0Str);
		if(argElement->hasAttribute(a1Str)) // a1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a1Str));
			this->a1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a1 was provided");
		}
		XMLString::release(&a1Str);
		if(argElement->hasAttribute(a2Str)) // a2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a2Str));
			this->a2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a2 was provided");
		}
		XMLString::release(&a2Str);
		if(argElement->hasAttribute(b1Str)) // b1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(b1Str));
			this->b1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient b1 was provided");
		}
		XMLString::release(&b1Str);
		if(argElement->hasAttribute(b2Str)) // b2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(b2Str));
			this->b2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient b2 was provided");
		}
		XMLString::release(&b2Str);
	}
	else if(XMLString::equals(typeStemNIA, optionStr)) // Stem Biomass, with no incidence angle correction
	{
		this->option = RSGISExeSARSaatchiBiomass::stemNIA; 
		cout << "\tCalculating stem biomass, with no correction for incidence angle" << endl;
		
		XMLCh *a0Str = XMLString::transcode("a0");
		XMLCh *a1Str = XMLString::transcode("a1");
		XMLCh *a2Str = XMLString::transcode("a2");
		XMLCh *b1Str = XMLString::transcode("b1");
		XMLCh *b2Str = XMLString::transcode("b2");
		XMLCh *c1Str = XMLString::transcode("c1");
		XMLCh *c2Str = XMLString::transcode("c2");
		
		if(argElement->hasAttribute(a0Str)) // a0
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a0Str));
			this->a0 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a0 was provided");
		}
		XMLString::release(&a0Str);
		if(argElement->hasAttribute(a1Str)) // a1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a1Str));
			this->a1 = mathUtils.strtofloat(string(charValue));
			if(a1 != 0)
			{
				cout << "\tUsing LeToan modification";
			}
			XMLString::release(&charValue);
		}
		else
		{
			this->a1 = 0;
		}
		XMLString::release(&a1Str);
		if(argElement->hasAttribute(a2Str)) // a2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a2Str));
			this->a2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			this->a2 = 0;
		}
		XMLString::release(&a2Str);
		if(argElement->hasAttribute(b1Str)) // b1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(b1Str));
			this->b1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient b1 was provided");
		}
		XMLString::release(&b1Str);
		if(argElement->hasAttribute(b2Str)) // b2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(b2Str));
			this->b2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient b2 was provided");
		}
		XMLString::release(&b2Str);
		if(argElement->hasAttribute(c1Str)) // c1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(c1Str));
			this->c1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient c1 was provided");
		}
		XMLString::release(&c1Str);
		if(argElement->hasAttribute(c2Str)) // c2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(c2Str));
			this->c2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient c2 was provided");
		}
		XMLString::release(&c2Str);
	}
	else if(XMLString::equals(typeCrownNIA, optionStr)) // Crown Biomass, with no incidence angle correction
	{
		this->option = RSGISExeSARSaatchiBiomass::crownNIA;
		cout << "\tCalculating crown biomass, with no correction for incidence angle" << endl;
		
		XMLCh *a0Str = XMLString::transcode("a0");
		XMLCh *a1Str = XMLString::transcode("a1");
		XMLCh *a2Str = XMLString::transcode("a2");
		XMLCh *c1Str = XMLString::transcode("c1");
		XMLCh *c2Str = XMLString::transcode("c2");
		
		if(argElement->hasAttribute(a0Str)) // a0
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a0Str));
			this->a0 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a0 was provided");
		}
		XMLString::release(&a0Str);
		if(argElement->hasAttribute(a1Str)) // a1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a1Str));
			this->a1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a1 was provided");
		}
		XMLString::release(&a1Str);
		if(argElement->hasAttribute(a2Str)) // a2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a2Str));
			this->a2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a2 was provided");
		}
		XMLString::release(&a2Str);
		if(argElement->hasAttribute(c1Str)) // c1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(c1Str));
			this->c1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient c1 was provided");
		}
		XMLString::release(&c1Str);
		if(argElement->hasAttribute(c2Str)) // c2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(c2Str));
			this->c2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient c2 was provided");
		}
		XMLString::release(&c2Str);
	}
	else if(XMLString::equals(typeCrownPLNIA, optionStr)) // Crown Biomass, using L and P bands 
	{
		this->option = RSGISExeSARSaatchiBiomass::crownPLNIA;
		cout << "\tCalculating crown biomass using L and P band, with no correction for incidence angle" << endl;
		
		XMLCh *a0Str = XMLString::transcode("a0");
		XMLCh *a1Str = XMLString::transcode("a1");
		XMLCh *a2Str = XMLString::transcode("a2");
		XMLCh *b1Str = XMLString::transcode("b1");
		XMLCh *b2Str = XMLString::transcode("b2");
		
		if(argElement->hasAttribute(a0Str)) // a0
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a0Str));
			this->a0 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a0 was provided");
		}
		XMLString::release(&a0Str);
		if(argElement->hasAttribute(a1Str)) // a1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a1Str));
			this->a1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a1 was provided");
		}
		XMLString::release(&a1Str);
		if(argElement->hasAttribute(a2Str)) // a2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(a2Str));
			this->a2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient a2 was provided");
		}
		XMLString::release(&a2Str);
		if(argElement->hasAttribute(b1Str)) // b1
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(b1Str));
			this->b1 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient b1 was provided");
		}
		XMLString::release(&b1Str);
		if(argElement->hasAttribute(b2Str)) // b2
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(b2Str));
			this->b2 = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Value for coefficient b2 was provided");
		}
		XMLString::release(&b2Str);
	}
	else if(XMLString::equals(typeOpenStemL, optionStr)) // Stem Biomass - Open forest, L band
	{
		this->option = RSGISExeSARSaatchiBiomass::stemNIA;
		cout << "\tCalculating stem biomass, using L-band coefficients for open forest" << endl;
		this->a0=6.591433;
		this->a1=0.2931488;
		this->a2=0.0056944;
		this->b1=-0.2118106;
		this->b2=-0.0148108;
		this->c1=-0.0301716;
		this->c2=-0.0005826;
	}	
	else if(XMLString::equals(typeOpenCrownL, optionStr)) // Crown Biomass - Open forest, L band
	{
		this->option = RSGISExeSARSaatchiBiomass::crownNIA;
		cout << "\tCalculating crown biomass, using L-band coefficients for open forest" << endl;
		this->a0=-0.0494395;
		this->a1=0.120779;
		this->a2=-0.0043865;
		this->b1=0;
		this->b2=0;
		this->c1=-0.3951039;
		this->c2=-0.013844;
	}
	else if(XMLString::equals(typeOpenStemP, optionStr)) // Stem Biomass - Open forest, P band
	{
		this->option = RSGISExeSARSaatchiBiomass::stemNIA;
		cout << "\tCalculating stem biomass, using P-band coefficients for open forest" << endl;
		this->a0=2.4261112;
		this->a1=0.2470936;
		this->a2=0.0056361;
		this->b1=0.0134467;
		this->b2=-0.001529;
		this->c1=-0.7007589;
		this->c2=-0.0263756;
	}	
	else if(XMLString::equals(typeOpenCrownP, optionStr)) // Crown Biomass - Open forest, P band
	{
		this->option = RSGISExeSARSaatchiBiomass::crownNIA;
		cout << "\tCalculating crown biomass, using P-band coefficients for open forest" << endl;
		this->a0=3.5215194;
		this->a1=0.10149;
		this->a2=0.0000118;
		this->b1=0;
		this->b2=0;
		this->c1=-0.3141069;
		this->c2=-0.0115178;
	}
	else if(XMLString::equals(typeOpenCrownPL, optionStr)) // Crown Biomass - Open forest, L+P band
	{
		this->option = RSGISExeSARSaatchiBiomass::crownPL;
		cout << "\tCalculating crown biomass, using L and P-band coefficients for open forest" << endl;
		this->a0=3.2641606;
		this->a1=-0.4925952;
		this->a2=-0.0126369;
		this->b1=0.321497;
		this->b2=0.0050075;
	}
	else if(XMLString::equals(typeClosedStemL, optionStr)) // Stem Biomass - Closed forest, L band
	{
		this->option = RSGISExeSARSaatchiBiomass::stemNIA;
		cout << "\tCalculating stem biomass, using L-band coefficients for Closed forest" << endl;
		/*
		Full set of coefficients:
		this->a0=9.184;
		this->a1=0.769;
		this->a2=0.0085;
		this->b1=0.188;
		this->b2=0.0002;
		this->c1=-0.165;
		this->c2=-0.0038;
		*/
		// With no correction for incidence angle
		this->a0=9.184;
		this->a1=0.769;
		this->a2=0.0085;
		this->b1=0.188;
		this->b2=0.0002;
		this->c1=-0.165;
		this->c2=-0.0038;
	}	
	else if(XMLString::equals(typeClosedCrownL, optionStr)) // Crown Biomass - Closed forest, L band
	{
		this->option = RSGISExeSARSaatchiBiomass::crownNIA;
		cout << "\tCalculating crown biomass, using L-band coefficients for Closed forest" << endl;
		/*
		Full set of coefficients
		this->a0=7.496;
		this->a1=0.664;
		this->a2=0.0084;
		this->b1=0.017;
		this->b2=-0.0016;
		this->c1=-0.322;
		this->c2=0.000007;
		*/
		// With no correction for incidence angle
		this->a0=7.496;
		this->a1=0.664;
		this->a2=0.0084;
		this->b1=0;
		this->b2=0;
		this->c1=-0.322;
		this->c2=0.000007;
	}
	else if(XMLString::equals(typeClosedStemP, optionStr)) // Stem Biomass - Closed forest, P band
	{
		this->option = RSGISExeSARSaatchiBiomass::stemNIA;
		cout << "\tCalculating stem biomass, using P-band coefficients for Closed forest" << endl;
		/*
		Full set of coefficients
		this->a0=8.104;
		this->a1=0.112;
		this->a2=-0.0018;
		this->b1=0.396;
		this->b2=0.0143;
		this->c1=-0.131;
		this->c2=-0.0081;
		*/
		// With no correction for incidence angle
		this->a0=8.104;
		this->a1=0;
		this->a2=0;
		this->b1=0.396;
		this->b2=0.0143;
		this->c1=-0.131;
		this->c2=-0.0081;
	}	
	else if(XMLString::equals(typeClosedCrownP, optionStr)) // Crown Biomass - Closed forest, P band
	{
		this->option = RSGISExeSARSaatchiBiomass::crownNIA;
		cout << "\tCalculating crown biomass, using P-band coefficients for Closed forest" << endl;
		/*
		Full set of coefficients:
		this->a0=6.215;
		this->a1=0.058;
		this->a2=-0.0017;
		this->b1=0.192;
		this->b2=0.0098;
		this->c1=0.0962;
		this->c2=-0.0028;
		*/
		// With no correction for incidence angle
		this->a0=6.215;
		this->a1=0.058;
		this->a2=-0.0017;
		this->b1=0;
		this->b2=0;
		this->c1=0.0962;
		this->c2=-0.0028;
	}
	else if(XMLString::equals(typeClosedCrownPL, optionStr)) // Crown Biomass - Closed forest, L+P band
	{
		this->option = RSGISExeSARSaatchiBiomass::crownPL;
		cout << "\tCalculating crown biomass, using L and P-band coefficients for Closed forest" << endl;
		this->a0=4.784;
		this->a1=0.0931;
		this->a2=0.0012;
		this->b1=0.0538;
		this->b2=0.00034;
	}
	else if(XMLString::equals(typeClosedLeToanStemP, optionStr)) // Stem Biomass - Closed forest, P band, LeToan modification
	{
		this->option = RSGISExeSARSaatchiBiomass::stemNIA;
		cout << "\tCalculating stem biomass, using LeToan's modified P-band coefficients for Closed forest" << endl;
		this->a0=6.45513;
		this->a1=-0.035;
		this->a2=-0.0042;
		this->b1=0.396;
		this->b2=0.0143;
		this->c1=-0.131;
		this->c2=-0.0081;
	}
	else if(XMLString::equals(typeClosedLeToanCrownP, optionStr)) // Crown Biomass - Closed forest, P band, LeToan modification
	{
		this->option = RSGISExeSARSaatchiBiomass::crownNIA;
		cout << "\tCalculating crown biomass, using LeToan's modified P-band coefficients for Closed forest" << endl;
		this->a0=6.45513;
		this->a1=0.023;
		this->a2=-0.0059;
		this->b1=0;
		this->b2=0;
		this->c1=0.0962;
		this->c2=-0.0028;
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionStr)) + string(") is not known: RSGISExeSARSaatchiBiomass.");
		throw RSGISXMLArgumentsException(message.c_str());
	}

	parsed = true;
	// Release XML
	XMLString::release(&algorName);
	XMLString::release(&typeStem);
	XMLString::release(&typeCrown);
	XMLString::release(&typeCrownPL);
	XMLString::release(&typeStemNIA);
	XMLString::release(&typeCrownNIA);
	XMLString::release(&typeCrownPLNIA);
	XMLString::release(&typeOpenStemL);
	XMLString::release(&typeOpenStemP);
	XMLString::release(&typeOpenCrownL);
	XMLString::release(&typeOpenCrownP);
	XMLString::release(&typeOpenCrownPL);
	XMLString::release(&typeClosedStemL);
	XMLString::release(&typeClosedStemP);	
	XMLString::release(&typeClosedCrownL);
	XMLString::release(&typeClosedCrownP);
	XMLString::release(&typeClosedCrownPL);
	XMLString::release(&typeClosedLeToanStemP);
	XMLString::release(&typeClosedLeToanCrownP);
	
}

void RSGISExeSARSaatchiBiomass::runAlgorithm() throw(RSGISException)
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
		
		if(option == RSGISExeSARSaatchiBiomass::stem) // Stem
		{
			RSGISCalcImage *calcImg = NULL;
			RSGISCalcSaatchiBiomassStem *sassan = NULL;
			try
			{
				sassan = new RSGISCalcSaatchiBiomassStem(1, this->a0, this->a1, this->a2, this->b1, this->b2, this->c1, this->c2);
				calcImg = new RSGISCalcImage(sassan, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
				delete calcImg;
				delete sassan;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeSARSaatchiBiomass::crown) // Crown
		{
			RSGISCalcImage *calcImg = NULL;
			RSGISCalcSaatchiBiomassCrown *sassan = NULL;
			try
			{
				sassan = new RSGISCalcSaatchiBiomassCrown(1, this->a0, this->a1, this->a2, this->b1, this->b2, this->c1, this->c2);
				calcImg = new RSGISCalcImage(sassan, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
				delete calcImg;
				delete sassan;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeSARSaatchiBiomass::crownPL) // Crown using P and L band
		{
			RSGISCalcImage *calcImg = NULL;
			RSGISCalcSaatchiBiomassCrownPL *sassan = NULL;
			try
			{
				sassan = new RSGISCalcSaatchiBiomassCrownPL(1, this->a0, this->a1, this->a2, this->b1, this->b2);
				calcImg = new RSGISCalcImage(sassan, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
				delete calcImg;
				delete sassan;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeSARSaatchiBiomass::stemNIA) // Stem with no incidence angle correction
		{
			RSGISCalcImage *calcImg = NULL;
			RSGISCalcSaatchiNoIABiomassStem *sassan = NULL;
			try
			{
				sassan = new RSGISCalcSaatchiNoIABiomassStem(1, this->a0, this->a1, this->a2, this->b1, this->b2, this->c1, this->c2);
				calcImg = new RSGISCalcImage(sassan, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
				delete calcImg;
				delete sassan;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeSARSaatchiBiomass::crownNIA) // Crown with no incidence angle correction
		{
			RSGISCalcImage *calcImg = NULL;
			RSGISCalcSaatchiNoIABiomassCrown *sassan = NULL;
			try
			{
				sassan = new RSGISCalcSaatchiNoIABiomassCrown(1, this->a0, this->a1, this->a2, this->c1, this->c2);
				calcImg = new RSGISCalcImage(sassan, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
				delete calcImg;
				delete sassan;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeSARSaatchiBiomass::crownPLNIA) // Crown using P and L band with no incidence angle correction
		{
			RSGISCalcImage *calcImg = NULL;
			RSGISCalcSaatchiNoIABiomassCrownPL *sassan = NULL;
			try
			{
				sassan = new RSGISCalcSaatchiNoIABiomassCrownPL(1, this->a0, this->a1, this->a2, this->b1, this->b2);
				calcImg = new RSGISCalcImage(sassan, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
				delete calcImg;
				delete sassan;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else
		{
			cout << "Unknown Option;";
		}
		
		if(datasets != NULL)
		{
			GDALClose(datasets[0]);
			delete[] datasets;
		}
		
	}
	
}

void RSGISExeSARSaatchiBiomass::printParameters()
{
	if(parsed)
	{
		cout << "Input Image: " << this->inputImage << endl;
		cout << "Output Image: " << this->outputImage << endl;
		
		if(option == RSGISExeSARSaatchiBiomass::stem)
		{
			cout << "a0 = " << this->a0 << endl;
			cout << "a1 = " << this->a1 << endl;
			cout << "a2 = " << this->a2 << endl;
			cout << "b1 = " << this->b1 << endl;
			cout << "b2 = " << this->b2 << endl;
			cout << "c1 = " << this->c1 << endl;
			cout << "c2 = " << this->c2 << endl;
		}
		else if(option == RSGISExeSARSaatchiBiomass::crown)
		{
			cout << "a0 = " << this->a0 << endl;
			cout << "a1 = " << this->a1 << endl;
			cout << "a2 = " << this->a2 << endl;
			cout << "b1 = " << this->b1 << endl;
			cout << "b2 = " << this->b2 << endl;
			cout << "c1 = " << this->c1 << endl;
			cout << "c2 = " << this->c2 << endl;
		}
		else if(option == RSGISExeSARSaatchiBiomass::crownPL)
		{
			cout << "a0 = " << this->a0 << endl;
			cout << "a1 = " << this->a1 << endl;
			cout << "a2 = " << this->a2 << endl;
			cout << "b1 = " << this->b1 << endl;
			cout << "b2 = " << this->b2 << endl;
		}
		else if(option == RSGISExeSARSaatchiBiomass::stemNIA)
		{
			cout << "a0 = " << this->a0 << endl;
			cout << "a1 = " << this->a1 << endl;
			cout << "a2 = " << this->a2 << endl;
			cout << "b1 = " << this->b1 << endl;
			cout << "b2 = " << this->b2 << endl;
			cout << "c1 = " << this->c1 << endl;
			cout << "c2 = " << this->c2 << endl;
		}
		else if(option == RSGISExeSARSaatchiBiomass::crownNIA)
		{
			cout << "a0 = " << this->a0 << endl;
			cout << "a1 = " << this->a1 << endl;
			cout << "a2 = " << this->a2 << endl;
			cout << "c1 = " << this->c1 << endl;
			cout << "c2 = " << this->c2 << endl;
		}
		else if(option == RSGISExeSARSaatchiBiomass::crownPLNIA)
		{
			cout << "a0 = " << this->a0 << endl;
			cout << "a1 = " << this->a1 << endl;
			cout << "a2 = " << this->a2 << endl;
			cout << "b1 = " << this->b1 << endl;
			cout << "b2 = " << this->b2 << endl;
		}
		else
		{
			cout << "Unknown Option;";
		}
	}
	else
	{
		cout << "The parameters have yet to be parsed;";
	}
}

void RSGISExeSARSaatchiBiomass::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
    cout << "\t <rsgis:command algor=\"SaatchiBiomass\" option=\"stem\" input=\"inputimage\" output=\"outputimage\"\n";
	cout <<  "\t \t a0=\"a0\" a1=\"a1\" a2=\"a2\" b1=\"b1\" b2=\"b2\" c1=\"c1\" c2=\"c2\"/>\"\n";
	cout << "\t <rsgis:command algor=\"SaatchiBiomass\" option=\"crown\" input=\"inputimage\" output=\"outputimage\"\n";
	cout <<  "\t \t a0=\"a0\" a1=\"a1\" a2=\"a2\" b1=\"b1\" b2=\"b2\" c1=\"c1\" c2=\"c2\"/>\n";
	cout << "\t <rsgis:command algor=\"SaatchiBiomass\" option=\"crownPL\" input=\"inputimage\" output=\"outputimage\"\n";
	cout <<  "\t \t a0=\"a0\" a1=\"a1\" a2=\"a2\" b1=\"b1\" b2=\"b2\" c1=\"c1\" c2=\"c2\"/>\n";
    cout << "\t <rsgis:command algor=\"SaatchiBiomass\" option=\"stemNIA\" input=\"inputimage\" output=\"outputimage\"\n";
	cout <<  "\t \t a0=\"a0\" a1=\"a1\" a2=\"a2\" b1=\"b1\" b2=\"b2\" c1=\"c1\" c2=\"c2\"/>\n";
	cout << "\t <rsgis:command algor=\"SaatchiBiomass\" option=\"crownNIA\" input=\"inputimage\" output=\"outputimage\"\n";
	cout <<  "\t \t a0=\"a0\" a1=\"a1\" a2=\"a2\" b1=\"b1\" b2=\"b2\" c1=\"c1\" c2=\"c2\"/>\n";
	cout << "\t <rsgis:command algor=\"SaatchiBiomass\" option=\"crownPLNIA\" input=\"inputimage\" output=\"outputimage\"\n";
	cout <<  "\t \t a0=\"a0\" a1=\"a1\" a2=\"a2\" b1=\"b1\" b2=\"b2\" c1=\"c1\" c2=\"c2\"/></rsgis:command>\n";
	cout << "</rsgis:commands>\n";

}

string RSGISExeSARSaatchiBiomass::getDescription()
{
	return "Calculates stem or crown biomass from L and or P band data using; the algorithm of Saatch et. al. 2002";
}

string RSGISExeSARSaatchiBiomass::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeSARSaatchiBiomass::~RSGISExeSARSaatchiBiomass()
{
	
}
    
}
