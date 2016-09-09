/*
 *  RSGISExeFitting.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 29/01/2009.
 *  Copyright 2009 RSGISLib.
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

#include "RSGISExeFitting.h"

namespace rsgisexe{

RSGISExeFitting::RSGISExeFitting() : rsgis::RSGISAlgorithmParameters()
{
	this->algorithm = "fitting";
	this->infile = "";
	this->outfile = "";
	this->option = RSGISExeFitting::none;
}

rsgis::RSGISAlgorithmParameters* RSGISExeFitting::getInstance()
{
	return new RSGISExeFitting();
}

void RSGISExeFitting::retrieveParameters(DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException)
{
	
	RSGISMathsUtils mathUtils;
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *typeOneDimensionalPolyfit = XMLString::transcode("oneDimensionalPolyfit");
	XMLCh *typeOneDimensionalPolyTest = XMLString::transcode("oneDimensionalPolyTest");
	XMLCh *typeTwoDimensionalPolyfit = XMLString::transcode("twoDimensionalPolyfit");
	XMLCh *typeTwoDimensionalPolyTest = XMLString::transcode("twoDimensionalPolyTest");
	XMLCh *typeThreeDimensionalPolyFit = XMLString::transcode("threeDimensionalPolyfit");
	XMLCh *typeThreeDimensionalPolyTest = XMLString::transcode("threeDimensionalPolyTest");
	//XMLCh *typeLinearFit = XMLString::transcode("linearFit"); Not using yet
	//XMLCh *typeLinearTest = XMLString::transcode("linearTest"); Not using yet
	XMLCh *typeNonlinearFit = XMLString::transcode("nonlinearFit");
	XMLCh *typeNonlinearTest = XMLString::transcode("nonlinearTest");
	
	// Functions
	XMLCh *functionLn2Var = XMLString::transcode("ln2Var");
	XMLCh *functionLn2VarQuadratic = XMLString::transcode("ln2VarQuadratic");
	
	const XMLCh *algorNameEle = argElement->getAttribute(XMLString::transcode("algor"));
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw rsgis::RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	
	// Retrieve input and output file
	XMLCh *inputXMLStr = XMLString::transcode("input");
	if(argElement->hasAttribute(inputXMLStr))
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
		this->infile = string(charValue);
		XMLString::release(&charValue);
	}
	else
	{
		throw rsgis::RSGISXMLArgumentsException("Input file not provided..");
	}
	XMLString::release(&inputXMLStr);
	
	XMLCh *outputXMLStr = XMLString::transcode("output");
	if(argElement->hasAttribute(outputXMLStr))
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
		this->outfile = string(charValue);
		XMLString::release(&charValue);
	}
	else
	{
		throw rsgis::RSGISXMLArgumentsException("Output file not provided..");
	}
	XMLString::release(&outputXMLStr);
	const XMLCh *optionStr = argElement->getAttribute(XMLString::transcode("option"));
	
	if(XMLString::equals(typeOneDimensionalPolyfit, optionStr))
	{
		this->option = RSGISExeFitting::oneDimensionalPolyfit;
		XMLCh *polyOrderStr = XMLString::transcode("polyOrder"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderStr));
			this->polyOrder = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order");
		}
		XMLString::release(&polyOrderStr);
	}
	
	else if(XMLString::equals(typeOneDimensionalPolyTest, optionStr))
	{
		this->option = RSGISExeFitting::oneDimensionalPolyTest;
		XMLCh *inCoeff = XMLString::transcode("inCoeff"); // Coefficients
		if(argElement->hasAttribute(inCoeff))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inCoeff));
			this->inCoeffFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Coefficients not Provided");
		}
		XMLString::release(&inCoeff);
		XMLCh *polyOrderStr = XMLString::transcode("polyOrder"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderStr));
			this->polyOrder = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order");
		}
		XMLString::release(&polyOrderStr);
	}
	
	else if(XMLString::equals(typeTwoDimensionalPolyfit, optionStr))
	{
		this->option = RSGISExeFitting::twoDimensionalPolyfit;
		bool sameOrderBothFits = false;
		XMLCh *polyOrderStr = XMLString::transcode("polyOrder"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			cout << "\tusing same order polynomial for both sets of fits, use \'polyOrderX\' and \'polyOrderY\' to set seperately" << endl;
			sameOrderBothFits = true;
			this->polyOrderX = orderInt;
			this->polyOrderY = orderInt;
			XMLString::release(&charValue);
		}
		XMLString::release(&polyOrderStr);
		XMLCh *polyOrderXStr = XMLString::transcode("polyOrderX"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderXStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderXStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			this->polyOrderX = orderInt;
			XMLString::release(&charValue);
		}
		else
		{
			if (!sameOrderBothFits) 
			{
				throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for x fits");
			}
		}
		XMLString::release(&polyOrderXStr);
		XMLCh *polyOrderYStr = XMLString::transcode("polyOrderY"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderYStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderYStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			this->polyOrderY = orderInt;
			XMLString::release(&charValue);
		}
		else
		{
			if (!sameOrderBothFits) 
			{
				throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for y fits");
			}
		}
		XMLString::release(&polyOrderYStr);
		XMLCh *numXStr = XMLString::transcode("numX"); // Num X
		if(argElement->hasAttribute(numXStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numXStr));
			this->numX = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for number of x terms");
		}
		XMLString::release(&numXStr);
		XMLCh *numYStr = XMLString::transcode("numY"); // Num Y
		if(argElement->hasAttribute(numYStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numYStr));
			this->numY = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for number of y terms");
		}
		XMLString::release(&numYStr);
	}
	
	else if(XMLString::equals(typeTwoDimensionalPolyTest, optionStr))
	{
		this->option = RSGISExeFitting::twoDimensionalPolyTest;
		XMLCh *inCoeff = XMLString::transcode("inCoeff"); // Coefficients
		if(argElement->hasAttribute(inCoeff))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inCoeff));
			this->inCoeffFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Coefficients not Provided");
		}
		XMLString::release(&inCoeff);
		bool sameOrderBothFits = false;
		XMLCh *polyOrderStr = XMLString::transcode("polyOrder"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			cout << "\tusing same order polynomial for both sets of fits, use \'polyOrderX\' and \'polyOrderY\' to set seperately" << endl;
			sameOrderBothFits = true;
			this->polyOrderX = orderInt;
			this->polyOrderY = orderInt;
			XMLString::release(&charValue);
		}
		XMLString::release(&polyOrderStr);
		XMLCh *polyOrderXStr = XMLString::transcode("polyOrderX"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderXStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderXStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			this->polyOrderX = orderInt;
			XMLString::release(&charValue);
		}
		else
		{
			if (!sameOrderBothFits) 
			{
				throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for x fits");
			}
		}
		XMLString::release(&polyOrderXStr);
		XMLCh *polyOrderYStr = XMLString::transcode("polyOrderY"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderYStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderYStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			this->polyOrderY = orderInt;
			XMLString::release(&charValue);
		}
		else
		{
			if (!sameOrderBothFits) 
			{
				throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for y fits");
			}
		}
		XMLString::release(&polyOrderYStr);
	}
	
	else if(XMLString::equals(typeThreeDimensionalPolyFit, optionStr))
	{
		this->option = RSGISExeFitting::threeDimensionalPolyfit;
		bool sameOrderBothFits = false;
		XMLCh *polyOrderStr = XMLString::transcode("polyOrder"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			cout << "\tusing same order polynomial for both sets of fits, use \'polyOrderX\', \'polyOrderY\' and \'polyOrderZ\' to set seperately" << endl;
			sameOrderBothFits = true;
			this->polyOrderX = orderInt;
			this->polyOrderY = orderInt;
			this->polyOrderZ = orderInt;
			XMLString::release(&charValue);
		}
		XMLString::release(&polyOrderStr);
		XMLCh *polyOrderXStr = XMLString::transcode("polyOrderX"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderXStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderXStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			this->polyOrderX = orderInt;
			XMLString::release(&charValue);
		}
		else
		{
			if (!sameOrderBothFits) 
			{
				throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for x fits");
			}
		}
		XMLString::release(&polyOrderXStr);
		XMLCh *polyOrderYStr = XMLString::transcode("polyOrderY"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderYStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderYStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			this->polyOrderY = orderInt;
			XMLString::release(&charValue);
		}
		else
		{
			if (!sameOrderBothFits) 
			{
				throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for y fits");
			}
		}
		XMLString::release(&polyOrderYStr);
		XMLCh *polyOrderZStr = XMLString::transcode("polyOrderZ"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderZStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderZStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			this->polyOrderZ = orderInt;
			XMLString::release(&charValue);
		}
		else
		{
			if (!sameOrderBothFits) 
			{
				throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for z fits");
			}
		}
		XMLString::release(&polyOrderZStr);
		XMLCh *numXStr = XMLString::transcode("numX"); // Num X
		if(argElement->hasAttribute(numXStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numXStr));
			this->numX = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for number of x terms");
		}
		XMLString::release(&numXStr);
		XMLCh *numYStr = XMLString::transcode("numY"); // Num Y
		if(argElement->hasAttribute(numYStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numYStr));
			this->numY = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for number of y terms");
		}
		XMLString::release(&numYStr);
		XMLCh *numZStr = XMLString::transcode("numZ"); // Num Z
		if(argElement->hasAttribute(numZStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numZStr));
			this->numZ = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for number of z terms");
		}
		XMLString::release(&numZStr);
	}
	
	else if(XMLString::equals(typeThreeDimensionalPolyTest, optionStr))
	{
		this->option = RSGISExeFitting::threeDimensionalPolyTest;
		XMLCh *inCoeff = XMLString::transcode("inCoeff"); // Coefficients
		if(argElement->hasAttribute(inCoeff))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inCoeff));
			this->inCoeffFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Coefficients not Provided");
		}
		XMLString::release(&inCoeff);
		bool sameOrderBothFits = false;
		XMLCh *polyOrderStr = XMLString::transcode("polyOrder"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			cout << "\tusing same order polynomial for both sets of fits, use \'polyOrderX\', \'polyOrderY\' and \'polyOrderZ\' to set seperately" << endl;
			sameOrderBothFits = true;
			this->polyOrderX = orderInt;
			this->polyOrderY = orderInt;
			this->polyOrderZ = orderInt;
			XMLString::release(&charValue);
		}
		XMLString::release(&polyOrderStr);
		XMLCh *polyOrderXStr = XMLString::transcode("polyOrderX"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderXStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderXStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			this->polyOrderX = orderInt;
			XMLString::release(&charValue);
		}
		else
		{
			if (!sameOrderBothFits) 
			{
				throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for x fits");
			}
		}
		XMLString::release(&polyOrderXStr);
		XMLCh *polyOrderYStr = XMLString::transcode("polyOrderY"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderYStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderYStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			this->polyOrderY = orderInt;
			XMLString::release(&charValue);
		}
		else
		{
			if (!sameOrderBothFits) 
			{
				throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for y fits");
			}
		}
		XMLString::release(&polyOrderYStr);
		XMLCh *polyOrderZStr = XMLString::transcode("polyOrderZ"); // Polynomial Order
		if(argElement->hasAttribute(polyOrderZStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderZStr));
			int orderInt = mathUtils.strtoint(string(charValue));
			this->polyOrderZ = orderInt;
			XMLString::release(&charValue);
		}
		else
		{
			if (!sameOrderBothFits) 
			{
				throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for z fits");
			}
		}
		XMLString::release(&polyOrderZStr);		
	}
	
	else if(XMLString::equals(typeNonlinearFit, optionStr))
	{
		this->option = RSGISExeFitting::nonLinearFit;
		const XMLCh *functionStr = argElement->getAttribute(XMLString::transcode("function"));
		if (XMLString::equals(functionLn2Var,functionStr)) 
		{
			this->coefficients = new double[3];
			this->fixCoefficients = new bool[3];
			this->function = ln2Var;
			
			// a Coefficient
			XMLCh *inCoeffA = XMLString::transcode("coeffA"); // Variable coefficent
			XMLCh *inFixCoeffA = XMLString::transcode("fixCoeffA"); // Fixed coefficent
			if(argElement->hasAttribute(inCoeffA))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inCoeffA));
				string inCoeffAStr = string(charValue);
				this->coefficients[0] = mathUtils.strtodouble(inCoeffAStr);
				this->fixCoefficients[0] = false;
				XMLString::release(&charValue);
			}
			else if(argElement->hasAttribute(inFixCoeffA))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inFixCoeffA));
				string inCoeffAStr = string(charValue);
				this->coefficients[0] = mathUtils.strtodouble(inCoeffAStr);
				this->fixCoefficients[0] = true;
				XMLString::release(&charValue);
			}
			else
			{
				cout << "\tNo starting value provided for 'a' coefficient" << endl;
				this->coefficients[0] = 1;
				this->fixCoefficients[0] = false;
			}
			XMLString::release(&inCoeffA);
			XMLString::release(&inFixCoeffA);
			
			// b Coefficent
			XMLCh *inCoeffB = XMLString::transcode("coeffB"); // Variable coefficent
			XMLCh *inFixCoeffB = XMLString::transcode("fixCoeffB"); // Fixed coefficent
			if(argElement->hasAttribute(inCoeffB))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inCoeffB));
				string inCoeffBStr = string(charValue);
				this->coefficients[1] = mathUtils.strtodouble(inCoeffBStr);
				this->fixCoefficients[1] = false;
				XMLString::release(&charValue);
			}
			else if(argElement->hasAttribute(inFixCoeffB))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inFixCoeffB));
				string inCoeffBStr = string(charValue);
				this->coefficients[1] = mathUtils.strtodouble(inCoeffBStr);
				this->fixCoefficients[1] = true;
				XMLString::release(&charValue);
			}
			else
			{
				cout << "\tNo starting value provided for 'b' coefficient" << endl;
				this->coefficients[1] = 1;
				this->fixCoefficients[1] = false;
			}
			XMLString::release(&inCoeffB);
			XMLString::release(&inFixCoeffB);
			
			// c Coefficient
			XMLCh *inCoeffC = XMLString::transcode("coeffC"); // Variable coefficent
			XMLCh *inFixCoeffC = XMLString::transcode("fixCoeffC"); // Fixed coefficent
			if(argElement->hasAttribute(inCoeffC))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inCoeffC));
				string inCoeffCStr = string(charValue);
				this->coefficients[2] = mathUtils.strtodouble(inCoeffCStr);
				this->fixCoefficients[2] = false;
				XMLString::release(&charValue);
			}
			else if(argElement->hasAttribute(inFixCoeffC))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inFixCoeffC));
				string inCoeffCStr = string(charValue);
				this->coefficients[2] = mathUtils.strtodouble(inCoeffCStr);
				this->fixCoefficients[2] = true;
				XMLString::release(&charValue);
			}
			else
			{
				cout << "\tNo starting value provided for 'c' coefficient" << endl;
				this->coefficients[2] = 1;
				this->fixCoefficients[2] = false;
			}
			XMLString::release(&inCoeffC);
			XMLString::release(&inFixCoeffC);
		}
		else {throw rsgis::RSGISXMLArgumentsException("Function not recognised\n");}
	}
	
	else if(XMLString::equals(typeNonlinearTest, optionStr))
	{
		this->option = RSGISExeFitting::nonLinearTest;
		const XMLCh *functionStr = argElement->getAttribute(XMLString::transcode("function"));
		if (XMLString::equals(functionLn2Var,functionStr)) 
		{
			this->function = ln2Var;
		}
		else{throw rsgis::RSGISXMLArgumentsException("Function not provided / recognised");}
		
		XMLCh *inCoeff = XMLString::transcode("inCoeff"); // Coefficients
		if(argElement->hasAttribute(inCoeff))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inCoeff));
			this->inCoeffFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Coefficients not Provided");
		}
		XMLString::release(&inCoeff);
	}
	
	
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionStr)) + string(") is not known: RSGISExeFitting.");
		throw rsgis::RSGISXMLArgumentsException(message.c_str());
	}
	
	parsed = true;
	// Release XML	
	XMLString::release(&algorName);
	XMLString::release(&typeOneDimensionalPolyfit);
	XMLString::release(&typeOneDimensionalPolyTest);
	XMLString::release(&typeTwoDimensionalPolyfit);
	XMLString::release(&typeTwoDimensionalPolyTest);
	XMLString::release(&typeThreeDimensionalPolyTest);
	XMLString::release(&functionLn2Var);
	XMLString::release(&functionLn2VarQuadratic);
	
}

void RSGISExeFitting::runAlgorithm() throw(rsgis::RSGISException)
{
	if(!parsed)
	{
		throw rsgis::RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(option == RSGISExeFitting::oneDimensionalPolyfit) // One dimensional polynomal fitting
		{			
			try
			{
				RSGISMatrices matrixUtils;
				RSGISVectors vectorUtils;
				RSGISPolyFit poly;
				
				gsl_matrix *inData;
				gsl_vector *outCoefficients;
				inData = matrixUtils.readGSLMatrixFromGridTxt(infile);
				int order = polyOrder + 1;
				outCoefficients = poly.PolyfitOneDimension(order, inData);
				vectorUtils.saveGSLVector2Txt(outCoefficients, outfile);
				cout << "Coefficients written to: " << outfile << ".mtxt" << endl;
				cout << "\n";
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeFitting::oneDimensionalPolyTest) // One dimensional polynomal testing
		{			
			try
			{
				RSGISMatrices matrixUtils;
				RSGISVectors vectorUtils;
				RSGISPolyFit poly;
				
				gsl_matrix *inData;
				gsl_vector *inCoefficients;
				gsl_matrix *measuredVPredicted;
				inData = matrixUtils.readGSLMatrixFromGridTxt(infile);
				inCoefficients = vectorUtils.readGSLVectorFromTxt(inCoeffFile);
				int order = polyOrder + 1;
				
				// Print message to screen
				string polyOrderSuffix = "th";
				if(polyOrder == 1)
				{
					polyOrderSuffix = "st";
				}
				else if(polyOrder == 2)
				{
					polyOrderSuffix = "nd";
				}
				else if(polyOrder == 3)
				{
					polyOrderSuffix = "rd";
				}
				
				cout << "Testing fit of one-dimensional " << polyOrder << polyOrderSuffix << " order polynomial" << endl;
				cout << " using " << infile << endl;
				
				// Run test
				measuredVPredicted = poly.PolyTestOneDimension(order, inData, inCoefficients);
								
				// Save out seperate arrays
				int numPts = measuredVPredicted->size1;
				
				double *measured = new double[numPts];
				double *predicted = new double[numPts];
				
				//cout << "numPts = " << numPts << endl;
				
				for(int i = 0; i < numPts; i++)
				{
					measured[i] = gsl_matrix_get(measuredVPredicted, i, 0);
					predicted[i] = gsl_matrix_get(measuredVPredicted, i, 1);
				}
				
				// Save out as plot file
				RSGISExportForPlotting::getInstance()->export2DScatter(this->outfile, measured, predicted, numPts);
				
				delete[] measured;
				delete[] predicted;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeFitting::twoDimensionalPolyfit) // Two dimensional polynomal fitting
		{		
			try
			{
				RSGISMatrices matrixUtils;
				RSGISVectors vectorUtils;
				RSGISPolyFit poly;
				
				gsl_matrix *inData;
				gsl_matrix *outCoefficients;
				inData = matrixUtils.readGSLMatrixFromGridTxt(infile);
				int orderX = this->polyOrderX + 1;
				int orderY = this->polyOrderY + 1;
				outCoefficients = poly.PolyfitTwoDimension(numX, numY, orderX, orderY, inData);
				//matrixUtils.printGSLMatrix(outCoefficients);
				matrixUtils.saveGSLMatrix2Txt(outCoefficients, outfile);
				cout << "Coefficients written to: " << outfile << ".mtxt" << endl;
				cout << "\n";
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeFitting::twoDimensionalPolyTest) // Two dimensional polynomal testing
		{		
			try
			{
				RSGISMatrices matrixUtils;
				RSGISVectors vectorUtils;
				RSGISPolyFit poly;
				
				gsl_matrix *inData;
				gsl_matrix *inCoefficients;
				gsl_matrix *measuredVPredicted;
				inData = matrixUtils.readGSLMatrixFromGridTxt(infile);
				inCoefficients = matrixUtils.readGSLMatrixFromTxt(inCoeffFile);
				cout << "coefficients read in: " << endl;
				
				int orderX = this->polyOrderX + 1;
				int orderY = this->polyOrderY + 1;
				
				// Print message to screen
				string polyOrderSuffix1 = "th";
				if(this->polyOrderX == 1)
				{
					polyOrderSuffix1 = "st";
				}
				else if(this->polyOrderX == 2)
				{
					polyOrderSuffix1 = "nd";
				}
				else if(this->polyOrderX == 3)
				{
					polyOrderSuffix1 = "rd";
				}
				string polyOrderSuffix2 = "th";
				if(this->polyOrderY == 1)
				{
					polyOrderSuffix2 = "st";
				}
				else if(this->polyOrderY == 2)
				{
					polyOrderSuffix2 = "nd";
				}
				else if(this->polyOrderY == 3)
				{
					polyOrderSuffix2 = "rd";
				}
				cout << "Testing fit of two-dimensional " << this->polyOrderX  << polyOrderSuffix1 << " and " << this->polyOrderY << polyOrderSuffix2 << " order polynomials" << endl;
				cout << " using " << infile << endl;
				
				// Run test
				measuredVPredicted = poly.PolyTestTwoDimension(orderX, orderY, inData, inCoefficients);
				
				// Save out seperate arrays
				int numPts = measuredVPredicted->size1;
				
				double *measured = new double[numPts];
				double *predicted = new double[numPts];
				
				//cout << "numPts = " << numPts << endl;
				
				for(int i = 0; i < numPts; i++)
				{
					measured[i] = gsl_matrix_get(measuredVPredicted, i, 0);
					predicted[i] = gsl_matrix_get(measuredVPredicted, i, 1);
				}
				
				// Save out as plot file
				RSGISExportForPlotting::getInstance()->export2DScatter(this->outfile, measured, predicted, numPts);
				
				delete[] measured;
				delete[] predicted;
			
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeFitting::threeDimensionalPolyfit) // Three dimensional polynomal fitting
		{		
			try
			{
				RSGISMatrices matrixUtils;
				RSGISVectors vectorUtils;
				RSGISPolyFit poly;
				
				gsl_matrix *inData;
				gsl_matrix *outCoefficients;
				inData = matrixUtils.readGSLMatrixFromGridTxt(infile);
				int orderX = this->polyOrderX + 1;
				int orderY = this->polyOrderY + 1;
				int orderZ = this->polyOrderZ + 1;
				outCoefficients = poly.PolyfitThreeDimension(numX, numY, numZ, orderX, orderY, orderZ, inData);
				//matrixUtils.printGSLMatrix(outCoefficients);
				//matrixUtils.saveGSLMatrix2GridTxt(outCoefficients, outfile);
				matrixUtils.saveGSLMatrix2Txt(outCoefficients, outfile);
				cout << "Coefficients written to: " << outfile << ".gtxt" << endl;
				cout << "\n";
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeFitting::threeDimensionalPolyTest) // Three dimensional polynomal testing
		{		
			try
			{
				RSGISMatrices matrixUtils;
				RSGISVectors vectorUtils;
				RSGISPolyFit poly;
				
				gsl_matrix *inData;
				gsl_matrix *inCoefficients;
				gsl_matrix *measuredVPredicted;
				inData = matrixUtils.readGSLMatrixFromGridTxt(infile);
				cout << "data read in: " << endl;
				//inCoefficients = matrixUtils.readGSLMatrixFromGridTxt(inCoeffFile);
				inCoefficients = matrixUtils.readGSLMatrixFromTxt(inCoeffFile);
				cout << "coefficients read in: " << endl;
				//matrixUtils.printGSLMatrix(inCoefficients);
				int orderX = this->polyOrderX + 1;
				int orderY = this->polyOrderY + 1;
				int orderZ = this->polyOrderZ + 1;
				
				// Print message to screen
				string polyOrderSuffix1 = "th";
				if(this->polyOrderX == 1)
				{
					polyOrderSuffix1 = "st";
				}
				else if(this->polyOrderX == 2)
				{
					polyOrderSuffix1 = "nd";
				}
				else if(this->polyOrderX == 3)
				{
					polyOrderSuffix1 = "rd";
				}
				string polyOrderSuffix2 = "th";
				if(this->polyOrderY == 1)
				{
					polyOrderSuffix2 = "st";
				}
				else if(this->polyOrderY == 2)
				{
					polyOrderSuffix2 = "nd";
				}
				else if(this->polyOrderY == 3)
				{
					polyOrderSuffix2 = "rd";
				}
				string polyOrderSuffix3 = "th";
				if(this->polyOrderZ == 1)
				{
					polyOrderSuffix3 = "st";
				}
				else if(this->polyOrderZ == 2)
				{
					polyOrderSuffix3 = "nd";
				}
				else if(this->polyOrderZ == 3)
				{
					polyOrderSuffix3 = "rd";
				}
				cout << "Testing fit of two-dimensional " << this->polyOrderX  << polyOrderSuffix1 << ", " << this->polyOrderY << polyOrderSuffix2 << " and " << this->polyOrderZ << polyOrderSuffix3 << " order polynomials" << endl;
				cout << " using " << infile << endl;
				
				// Run test
				measuredVPredicted = poly.PolyTestThreeDimension(orderX, orderY, orderZ, inData, inCoefficients);
				
				// Save out seperate arrays
				int numPts = measuredVPredicted->size1;
				
				double *measured = new double[numPts];
				double *predicted = new double[numPts];
				
				cout << "numPts = " << numPts << endl;
				
				for(int i = 0; i < numPts; i++)
				{
					measured[i] = gsl_matrix_get(measuredVPredicted, i, 0);
					predicted[i] = gsl_matrix_get(measuredVPredicted, i, 1);
				}
				
				// Save out as plot file
				RSGISExportForPlotting::getInstance()->export2DScatter(this->outfile, measured, predicted, numPts);
				
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeFitting::nonLinearFit) // Non-linear fitting
		{
			try 
			{
				if (function == ln2Var) // y = a + b x1 + c x1 ln(x0)
				{
					RSGISMatrices matrixUtils;
					RSGISVectors vectorUtils;
					RSGISNonLinearFit nlFit;
					
					RSGISFunction2VarLn *twoVarLnFunction;
					twoVarLnFunction = new RSGISFunction2VarLn(coefficients[0], coefficients[1], coefficients[2]);
					
					gsl_matrix *inFile = matrixUtils.readGSLMatrixFromGridTxt(this->infile);
					cout << "Read infile" << endl;
					
					bool useVariance = false;
					
					if(inFile->size2 == 3) // x0, x1, y
					{
						cout << "Not using variance" << endl;
					}
					else if(inFile->size2 == 4) // x0, x1, y, var
					{
						cout << "Using variance" << endl;
						useVariance = true;
					}
					else 
					{
						throw rsgis::RSGISException("Input file has incorrect number of columns, should be 3 or 4\n");
					}

					
					gsl_matrix *indVar; // x0, x1
					gsl_vector *depVar; // y
					gsl_vector *variance; // variance
					gsl_vector *coeff; // coefficents (gsl)
					indVar = gsl_matrix_alloc(inFile->size1, 2);
					depVar = gsl_vector_alloc(inFile->size1);
					variance = gsl_vector_alloc(inFile->size1);
					coeff = gsl_vector_alloc(twoVarLnFunction->numCoefficients());
					
					gsl_vector_set(coeff, 0, coefficients[0]);
					gsl_vector_set(coeff, 1, coefficients[1]);
					gsl_vector_set(coeff, 2, coefficients[2]);
					
					for (unsigned int i = 0; i < inFile->size1; i++)
					{
						double x0 = gsl_matrix_get(inFile, i, 0);
						double x1 = gsl_matrix_get(inFile, i, 1);
						
						double var;
						double y;
						if (useVariance) 
						{
							var = gsl_matrix_get(inFile, i, 2);
							y = gsl_matrix_get(inFile, i, 3);
						}
						else 
						{
							var = 1; // Set variance to 1.
							y = gsl_matrix_get(inFile, i, 2);
						}

						gsl_matrix_set(indVar, i, 0, x0);
						gsl_matrix_set(indVar, i, 1, x1);
						gsl_vector_set(depVar, i, y);
						gsl_vector_set(variance, i, var);
					}
					
					double chiSq = nlFit.nonLinearFit2Var(indVar, depVar, variance, coeff, fixCoefficients, twoVarLnFunction);
					cout << "----------------------------------------------" << endl;
					cout << "Coefficeints are: " << endl;
					vectorUtils.printGSLVector(coeff);
					cout << "chiSq = " << chiSq << endl;
					cout << "----------------------------------------------" << endl;
					vectorUtils.saveGSLVector2Txt(coeff, outfile); // Save coefficeints to outfile
		
					// Tidy up
					gsl_matrix_free(indVar); // x0, x1
					gsl_vector_free(depVar); // y
					gsl_vector_free(variance); // variance
					gsl_vector_free(coeff);
					delete coefficients;
					delete fixCoefficients;
					
				}
			}
			catch (rsgis::RSGISException e) 
			{
				throw e;
			}
		}
		else if(option == RSGISExeFitting::nonLinearTest) // Non-linear test
		{
			try 
			{
				if (function == ln2Var) // y = a + b x1 + c x1 ln(x0)
				{
					RSGISMatrices matrixUtils;
					RSGISVectors vectorUtils;
					RSGISNonLinearFit nlFit;
					RSGISPolyFit poly;
					
					gsl_vector *inCoeff;
					inCoeff = vectorUtils.readGSLVectorFromTxt(inCoeffFile);
					
					double coeffA = gsl_vector_get(inCoeff, 0);
					double coeffB = gsl_vector_get(inCoeff, 1);
					double coeffC = gsl_vector_get(inCoeff, 2);
					
					RSGISFunction2VarLn *twoVarLnFunction;
					twoVarLnFunction = new RSGISFunction2VarLn(coeffA, coeffB, coeffC);
					
					gsl_matrix *inFile = matrixUtils.readGSLMatrixFromGridTxt(this->infile);
					cout << "Read infile" << endl;
					
					gsl_matrix *indVar; // x0, x1
					gsl_vector *depVar; // y
					
					indVar = gsl_matrix_alloc(inFile->size1, 2);
					depVar = gsl_vector_alloc(inFile->size1);
					
					for (unsigned int i = 0; i < inFile->size1; i++)
					{
						double x0 = gsl_matrix_get(inFile, i, 0);
						double x1 = gsl_matrix_get(inFile, i, 1);
						double y = gsl_matrix_get(inFile, i, 2);

						gsl_matrix_set(indVar, i, 0, x0);
						gsl_matrix_set(indVar, i, 1, x1);
						gsl_vector_set(depVar, i, y);

					}
					
					gsl_matrix *measuredVPredicted; // Matrix for measured and predicted values
					
					measuredVPredicted = nlFit.nonLinearTest2Var(indVar, depVar, twoVarLnFunction);
					poly.calcRSquaredGSLMatrix(measuredVPredicted);
					
					// Save out seperate arrays
					int numPts = measuredVPredicted->size1;
					
					double *measured = new double[numPts];
					double *predicted = new double[numPts];
					
					for(int i = 0; i < numPts; i++)
					{
						measured[i] = gsl_matrix_get(measuredVPredicted, i, 0);
						predicted[i] = gsl_matrix_get(measuredVPredicted, i, 1);
					}
					
					// Save out as plot file
					RSGISExportForPlotting::getInstance()->export2DScatter(this->outfile, measured, predicted, numPts);
					
					// Tidy up
					gsl_matrix_free(indVar); // x0, x1
					gsl_vector_free(depVar); // y
					gsl_vector_free(inCoeff); 
					gsl_matrix_free(measuredVPredicted);
					delete[] measured;
					delete[] predicted;
					
				}
			}
			catch (rsgis::RSGISException e) 
			{
				throw e;
			}
		}
	}
}

void RSGISExeFitting::printParameters()
{
	if(parsed)
	{
		cout << "infile = " << infile << endl;
		cout << "outfile = " << outfile << ".gtxt" << endl;
		
		if(option == RSGISExeFitting::oneDimensionalPolyfit)
		{
			cout << "order = " << polyOrder << endl;
			cout << "\n";
		}
		else if(option == RSGISExeFitting::twoDimensionalPolyfit)
		{
			cout << "order = " << polyOrder << endl;
			cout << "numX = " << numX << endl;
			cout << "numY = " << numY << endl;
			cout << "\n";
 		}
	}
	else
	{
		cout << "The parameters have yet to be parsed\n";
	}
}

void RSGISExeFitting::help()
{
	cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
	cout << "\t<rsgis:command algor=\"visualisation\" option=\"setupplotter\" outdir=\"outDIR\" />\n";
	cout << "\t<rsgis:command algor=\"fitting\" option=\"oneDimensionalPolyfit\" input=\"infile\"\n";
	cout << "\t\toutput=\"outfile\" polyOrder=\"polyOrder\" />\n";
	cout << "\t<rsgis:command algor=\"fitting\" option=\"oneDimensionalPolyTest\" input=\"infile\"\n";
	cout << "\t\tinCoeff=\"inCoefficients\" output=\"outname (name only no path or extension)\" polyOrder=\"polyOrder\" />\n";
	cout << "\t<rsgis:command algor=\"fitting\" option=\"twoDimensionalPolyfit\" input=\"infile\"\n";
	cout << "\t\toutput=\"outfile\" polyOrder=\"polyOrder\" numX=\"numX\" numY=\"numY\" />\n";
	cout << "\t<rsgis:command algor=\"fitting\" option=\"twoDimensionalPolyTest\" input=\"infile\"\n";
	cout << "\t\tinCoeff=\"inCoefficients\" output=\"outname (name only no path or extension)\" polyOrder=\"polyOrder\" />\n";
	cout << "\t<rsgis:command algor=\"fitting\" option=\"threeDimensionalPolyfit\" input=\"infile\"\n";
	cout << "\t\toutput=\"outfile\" polyOrder=\"polyOrder\" numX=\"numX\" numY=\"numY\" numZ\"numZ\" />\n";
	cout << "\t<rsgis:command algor=\"fitting\" option=\"twoDimensionalPolyTest\" input=\"infile\"\n";
	cout << "\t<rsgis:command algor=\"fitting\" option=\"threeDimensionalPolyTest\" input=\"infile\"\n";
	cout << "\t\tinCoeff=\"inCoefficients\" output=\"outname (name only no path or extension)\" polyOrder=\"polyOrder\" />\n";
	cout << "</rsgis:commands>\n";
}

string RSGISExeFitting::getDescription()
{
	return "Fits an equation to input data and writes coefficients to an output file";
}

string RSGISExeFitting::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeFitting::~RSGISExeFitting()
{
	
}
    
}
