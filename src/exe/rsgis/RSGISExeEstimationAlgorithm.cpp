/*
 *  RSGISExeEstimationAlgorithm.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 16/02/2009.
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

#include "RSGISExeEstimationAlgorithm.h"

namespace rsgisexe{

RSGISExeEstimationAlgorithm::RSGISExeEstimationAlgorithm() : RSGISAlgorithmParameters()
{
	this->algorithm = "estimation";
	this->inputImage = "";
	this->outputImage = "";
	this->option = RSGISExeEstimationAlgorithm::none;
	this->nPar = 0;
}

RSGISAlgorithmParameters* RSGISExeEstimationAlgorithm::getInstance()
{
	return new RSGISExeEstimationAlgorithm();
}

void RSGISExeEstimationAlgorithm::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;
	RSGISMatrices matrixUtils;
	RSGISVectors vectorUtils;

	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	// Options
	XMLCh *typeDualPolObject = XMLString::transcode("dualPolObject");
	XMLCh *typeFullPolObject = XMLString::transcode("fullPolObject");
	XMLCh *typeDualPolObjectObjAP = XMLString::transcode("dualPolObjectObjAP");
	XMLCh *typeFullPolObjectObjAP = XMLString::transcode("fullPolObjectObjAP");
	XMLCh *typeSimData2Var2Data = XMLString::transcode("simData2Var2Data");

	// Parameters
	XMLCh *typeCDepthDensity = XMLString::transcode("cDepthDensity");
	XMLCh *typeDielectricDensityHeight = XMLString::transcode("dielectricDensityHeight");
	XMLCh *typeHeightDensity = XMLString::transcode("heightDensity");

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
		throw RSGISXMLArgumentsException("Input image not provided..");
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
	const XMLCh *parametersStr = argElement->getAttribute(XMLString::transcode("parameters"));



	//---- Dual Pol Single Species - , Object Based ---//
	if((XMLString::equals(typeDualPolObject,optionStr)) | (XMLString::equals(typeDualPolObjectObjAP,optionStr)))
	{
		// Object based estimation
		cout << "\tObject based inversion" << endl;
		this->createOutSHP = false;
		this->useRasPoly = false;
		this->force = true;
		this->copyAttributes = true;
		this->classField = "";
		this->useClass = false;
		this->useDefaultMinMax = false;
		this->objAP = false;

		this->option = RSGISExeEstimationAlgorithm::dualPolObject;
		this->nBands = 4;

		XMLCh *objectXMLStr = XMLString::transcode("object");
		if(argElement->hasAttribute(objectXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(objectXMLStr));
			this->inputObjPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'object\' attribute was provided.");
		}
		XMLString::release(&objectXMLStr);

		XMLCh *rasterXMLStr = XMLString::transcode("raster");
		if(argElement->hasAttribute(rasterXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rasterXMLStr));
			this->inputRasPolys = string(charValue);
			this->useRasPoly = true;
			XMLString::release(&charValue);
		}
		else
		{
			cout << "\tNot using rasterised version of polygon (may be slower)" << endl;
			this->useRasPoly = false;
		}
		XMLString::release(&rasterXMLStr);

		XMLCh *classFieldXMLStr = XMLString::transcode("classField");
		if(argElement->hasAttribute(classFieldXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(classFieldXMLStr));
			this->classField = string(charValue);
			XMLString::release(&charValue);
			this->useClass = true;
			cout << "\tUsing field: \"" << this->classField << "\" to define classes" << endl;
		}
		XMLString::release(&classFieldXMLStr);

		XMLCh *outSHPXMLStr = XMLString::transcode("outSHP");
		if(argElement->hasAttribute(outSHPXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outSHPXMLStr));
			this->outputSHP = string(charValue);
			XMLString::release(&charValue);
			this->createOutSHP = true;
			cout << "\tCreating output shapefile" << endl;
		}
		XMLString::release(&outSHPXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			if (this->createOutSHP)
			{
				cout << "\tOverwriting output shapefile, if it exists." << endl;
			}
		}
		XMLString::release(&forceXMLStr);

		if(XMLString::equals(typeHeightDensity,parametersStr) | XMLString::equals(typeCDepthDensity,parametersStr))
		{
			if(XMLString::equals(typeHeightDensity,parametersStr))
			{
				cout << "\tParameters to be retrieved are height and stem density" << endl;
				this->parameters = heightDensity;
			}
			else
			{
				cout << "\tParameters to be retrieved are canopy depth and density" << endl;
				this->parameters = cDepthDensity;
			}

			// Get fields to use for AP
			if (XMLString::equals(typeDualPolObjectObjAP,optionStr))
			{
				this->objAP = true;
				this->objAPParField = new string[2];
				XMLCh *heightFieldXMLStr = XMLString::transcode("heightField");
				if(argElement->hasAttribute(heightFieldXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(heightFieldXMLStr));
					this->objAPParField[0] = string(charValue);
					XMLString::release(&charValue);
					cout << "\tUsing field: \"" << this->objAPParField[0] << "\" to provide a priori estimates of heights" << endl;
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'heightField\' attribute was provided. The field name for a priori values of height must be provided");
				}
				XMLString::release(&heightFieldXMLStr);

				XMLCh *densityFieldXMLStr = XMLString::transcode("densityField");
				if(argElement->hasAttribute(densityFieldXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(densityFieldXMLStr));
					this->objAPParField[1] = string(charValue);
					XMLString::release(&charValue);
					cout << "\tUsing field: \"" << this->objAPParField[1] << "\" to provide a priori estimates of densities" << endl;
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'densityField\' attribute was provided. The field name for a priori values of density must be provided");
				}
				XMLString::release(&densityFieldXMLStr);
			}

			this->nPar = 2;

			this->initialParClass= new vector <gsl_vector*>;
			this->estFastOptimiserClass = new vector <RSGISEstimationOptimiser*>;
			this->estSlowOptimiserClass = new vector <RSGISEstimationOptimiser*>;

			/**************************************
			 * Get method to use for optimisation *
			 **************************************/
			XMLCh *methodConjugateGradient = XMLString::transcode("conjugateGradient");
			XMLCh *methodConjugateGradientWithRestarts = XMLString::transcode("conjugateGradientWithRestarts");
			XMLCh *methodConjugateGradientPolynomial = XMLString::transcode("conjugateGradientPolynomial");
			XMLCh *methodExhaustiveSearch = XMLString::transcode("exhaustiveSearch");
			XMLCh *methodExhaustiveSearchAP = XMLString::transcode("exhaustiveSearchAP");
			XMLCh *methodSimulatedAnnealing = XMLString::transcode("simulatedAnnealing");
			XMLCh *methodSimulatedAnnealingAP = XMLString::transcode("simulatedAnnealingAP");
			XMLCh *methodAssignAP = XMLString::transcode("assignAP");
            XMLCh *methodNone= XMLString::transcode("none");

			DOMNodeList *slowOptimiserNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:estSlowOptimiserParameters"));
			DOMNodeList *fastOptimiserNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:estFastOptimiserParameters"));
			DOMElement *slowOptimiserElement = NULL;
			DOMElement *fastOptimiserElement = NULL;

			// Set up array to hold min / max values, these are read in with fast optimisers
			this->minMaxValuesClass = new double**[fastOptimiserNodesList->getLength()];

			// Loop through slow optimisers
			for(unsigned int i = 0; i < slowOptimiserNodesList->getLength(); i++)
			{
				slowOptimiserElement = static_cast<DOMElement*>(slowOptimiserNodesList->item(i));
				const XMLCh *methodStr = slowOptimiserElement->getAttribute(XMLString::transcode("method"));

				double initialHeight = 0;
				double initialDensity = 0;

				// Set initial parameters
				XMLCh *initialHeightStr = XMLString::transcode("initialHeight");
				if(slowOptimiserElement->hasAttribute(initialHeightStr))
				{
					char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(initialHeightStr));
					initialHeight = mathUtils.strtodouble(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No initial height provided");
				}
				XMLString::release(&initialHeightStr);
				XMLCh *initialDensityStr = XMLString::transcode("initialDensity");
				if(slowOptimiserElement->hasAttribute(initialDensityStr))
				{
					char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(initialDensityStr));
					initialDensity = mathUtils.strtodouble(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No initial density provided");
				}
				XMLString::release(&initialDensityStr);

				// Save parameters to vector
				this->initialParClass->push_back(gsl_vector_alloc(this->nPar));
				gsl_vector_set(this->initialParClass->at(i),0,initialHeight);
				gsl_vector_set(this->initialParClass->at(i),1,initialDensity);

				// Get type of function
				const XMLCh *functionStr = slowOptimiserElement->getAttribute(XMLString::transcode("function"));
				XMLCh *functionLn2Var = XMLString::transcode("ln2Var");
				XMLCh *functionLn2VarQuadratic = XMLString::transcode("ln2VarQuadratic");
				XMLCh *functionLinXfLinY = XMLString::transcode("linXfLinY");
				XMLCh *function2DPoly = XMLString::transcode("2DPoly");

				if (XMLString::equals(functionLn2Var,functionStr))
				{
					gsl_vector *coeffHH;
					XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
					if(slowOptimiserElement->hasAttribute(inCoeffHH))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffHH));
						string inCoeffHHFileName = string(charValue);
						coeffHH = vectorUtils.readGSLVectorFromTxt(inCoeffHHFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
					}
					XMLString::release(&inCoeffHH);

					gsl_vector *coeffHV;
					XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
					if(slowOptimiserElement->hasAttribute(inCoeffHV))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffHV));
						string inCoeffHVFileName = string(charValue);
						coeffHV = vectorUtils.readGSLVectorFromTxt(inCoeffHVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
					}
					XMLString::release(&inCoeffHV);


					RSGISFunction2VarLn *functionLnHH;
					RSGISFunction2VarLn *functionLnHV;

					functionLnHH = new RSGISFunction2VarLn(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2));
					functionLnHV = new RSGISFunction2VarLn(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2));

					this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
					this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;

					gsl_vector_free(coeffHH);
					gsl_vector_free(coeffHV);
				}
				else if (XMLString::equals(functionLn2VarQuadratic,functionStr))
				{
					gsl_vector *coeffHH;
					XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
					if(slowOptimiserElement->hasAttribute(inCoeffHH))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffHH));
						string inCoeffHHFileName = string(charValue);
						coeffHH = vectorUtils.readGSLVectorFromTxt(inCoeffHHFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
					}
					XMLString::release(&inCoeffHH);

					gsl_vector *coeffHV;
					XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
					if(slowOptimiserElement->hasAttribute(inCoeffHV))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffHV));
						string inCoeffHVFileName = string(charValue);
						coeffHV = vectorUtils.readGSLVectorFromTxt(inCoeffHVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
					}
					XMLString::release(&inCoeffHV);


					RSGISFunction2VarLnQuadratic *functionLnHH;
					RSGISFunction2VarLnQuadratic *functionLnHV;

					functionLnHH = new RSGISFunction2VarLnQuadratic(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2));
					functionLnHV = new RSGISFunction2VarLnQuadratic(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2));

					this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
					this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;

					gsl_vector_free(coeffHH);
					gsl_vector_free(coeffHV);
				}
				else if (XMLString::equals(functionLinXfLinY,functionStr))
				{
					gsl_vector *coeffHH;
					XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
					if(slowOptimiserElement->hasAttribute(inCoeffHH))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffHH));
						string inCoeffHHFileName = string(charValue);
						coeffHH = vectorUtils.readGSLVectorFromTxt(inCoeffHHFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
					}
					XMLString::release(&inCoeffHH);

					gsl_vector *coeffHV;
					XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
					if(slowOptimiserElement->hasAttribute(inCoeffHV))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffHV));
						string inCoeffHVFileName = string(charValue);
						coeffHV = vectorUtils.readGSLVectorFromTxt(inCoeffHVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
					}
					XMLString::release(&inCoeffHV);


					RSGISFunctionLinXfLinY *functionLnHH;
					RSGISFunctionLinXfLinY *functionLnHV;

					functionLnHH = new RSGISFunctionLinXfLinY(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2),gsl_vector_get(coeffHH, 3));
					functionLnHV = new RSGISFunctionLinXfLinY(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2),gsl_vector_get(coeffHV, 3));

					this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
					this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;

					gsl_vector_free(coeffHH);
					gsl_vector_free(coeffHV);
				}
				else if (XMLString::equals(function2DPoly,functionStr))
				{
					// Read coefficients
					XMLCh *coefficientsHH = XMLString::transcode("coefficientsHH");
					if(slowOptimiserElement->hasAttribute(coefficientsHH))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(coefficientsHH));
						string coeffHHFile = string(charValue);
						this->coeffHH = matrixUtils.readGSLMatrixFromTxt(coeffHHFile);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No coefficents provided for HH");
					}
					XMLString::release(&coefficientsHH);
					XMLCh *coefficientsHV = XMLString::transcode("coefficientsHV");
					if(slowOptimiserElement->hasAttribute(coefficientsHV))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(coefficientsHV));
						string coeffHVFile = string(charValue);
						this->coeffHV = matrixUtils.readGSLMatrixFromTxt(coeffHVFile);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No coefficents provided for HV");
					}
					XMLString::release(&coefficientsHV);

					// Set up functions
					RSGISFunction2DPoly *functionPolyHH;
					RSGISFunction2DPoly *functionPolyHV;

					functionPolyHH = new RSGISFunction2DPoly(coeffHH);
					functionPolyHV = new RSGISFunction2DPoly(coeffHV);

					this->functionHH = (RSGISMathTwoVariableFunction *) functionPolyHH;
					this->functionHV = (RSGISMathTwoVariableFunction *) functionPolyHV;
				}
				else
				{
					if(!XMLString::equals(methodNone, methodStr))
					{
						throw RSGISXMLArgumentsException("Function type not provieded / not recognised");
					}
				}

				XMLString::release(&functionLn2Var);
				XMLString::release(&functionLn2VarQuadratic);
				XMLString::release(&functionLinXfLinY);
				XMLString::release(&function2DPoly);

				// Get optimisation method
				if(XMLString::equals(methodConjugateGradientWithRestarts, methodStr) | XMLString::equals(methodConjugateGradient, methodStr))
				{
					if (XMLString::equals(methodConjugateGradientWithRestarts, methodStr)) {cout << "\tUsing ConjugateGradient - with restarts" << endl;}
					else{cout << "\tUsing ConjugateGradient" << endl;}

					// Maximum number of itterations
					XMLCh *ittmaxStr = XMLString::transcode("ittmax");
					if(slowOptimiserElement->hasAttribute(ittmaxStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(ittmaxStr));
						this->ittmax = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						this->ittmax = 10;
					}
					XMLString::release(&ittmaxStr);

					// Number or Restarts
					int numRestarts = 5;
					XMLCh *numRestartsStr = XMLString::transcode("numRestarts");
					if(slowOptimiserElement->hasAttribute(numRestartsStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(numRestartsStr));
						numRestarts = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&numRestartsStr);

					// Minimum error
					double minError = 1e-12;
					XMLCh *targetErrorStr = XMLString::transcode("targetError");
					if(slowOptimiserElement->hasAttribute(targetErrorStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(targetErrorStr));
						minError = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&targetErrorStr);

					/* Get minimum and maximum heights and density.
					 * Setting these is optional and default values will be chosen with no message if
					 * values are not set
					 */

					double *minMaxStepHeight = new double[3];
					double *minMaxStepDensity = new double[3];

					XMLCh *minHeightStr = XMLString::transcode("minHeight");
					if(slowOptimiserElement->hasAttribute(minHeightStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minHeightStr));
						minMaxStepHeight[0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[0] = 1;
					}
					XMLString::release(&minHeightStr);

					XMLCh *maxHeightStr = XMLString::transcode("maxHeight");
					if(slowOptimiserElement->hasAttribute(maxHeightStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxHeightStr));
						minMaxStepHeight[1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[1] = 20;
					}
					XMLString::release(&maxHeightStr);

					XMLCh *heightStepStr = XMLString::transcode("heightStep");
					if(slowOptimiserElement->hasAttribute(heightStepStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(heightStepStr));
						minMaxStepHeight[2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[2] = 0.01;
					}
					XMLString::release(&heightStepStr);

					XMLCh *minDensityStr = XMLString::transcode("minDensity");
					if(slowOptimiserElement->hasAttribute(minDensityStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minDensityStr));
						minMaxStepDensity[0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[0] = 0.1;
					}
					XMLString::release(&minDensityStr);

					XMLCh *maxDensityStr = XMLString::transcode("maxDensity");
					if(slowOptimiserElement->hasAttribute(maxDensityStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxDensityStr));
						minMaxStepDensity[1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[1] = 2;
					}
					XMLString::release(&maxDensityStr);

					XMLCh *densityStepStr = XMLString::transcode("densityStep");
					if(slowOptimiserElement->hasAttribute(densityStepStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(densityStepStr));
						minMaxStepDensity[2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[2] = 0.001;
					}
					XMLString::release(&densityStepStr);

					/******************************
					 * Set up covariance matrices *
					 ******************************/
					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;

					XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
					if(slowOptimiserElement->hasAttribute(covMatrixPXML))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(covMatrixPXML));
						string covMatrixPFile = string(charValue);
						covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
						XMLString::release(&charValue);
						cout << "\tRead Cm from file" << endl;
						if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
						{
							throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
						}

					}
					else
					{
						// Set to default values if no file (don't print warning)
						covMatrixP = gsl_matrix_alloc(2,2);
                        // Values of matrix based on assumption that standard deviation is 50 percent of value
						double pCov1 = pow(gsl_vector_get(this->initialParClass->at(i),0)*0.5,2)*1e6;
						double pCov2 = pow(gsl_vector_get(this->initialParClass->at(i),1)*0.5,2)*1e6*
                                        gsl_vector_get(this->initialParClass->at(i),0)/gsl_vector_get(this->initialParClass->at(i),1);
						gsl_matrix_set_zero(covMatrixP);
						gsl_matrix_set(covMatrixP, 0, 0, pCov1);
						gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					}
					XMLString::release(&covMatrixPXML);

					XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
					if(slowOptimiserElement->hasAttribute(invCovMatrixDXML))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(invCovMatrixDXML));
						string invCovMatrixDFile = string(charValue);
						invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
						XMLString::release(&charValue);
						cout << "\tRead 1 / Cd from file" << endl;
						if(invCovMatrixD->size1 != 2 or invCovMatrixD->size2 != 2)
						{
							throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
						}
					}
					else
					{
						// Set to default values if no file (don't print warning)
						invCovMatrixD = gsl_matrix_alloc(2, 2);
						double dCovInv1 = 1;
						double dCovInv2 = 1;
						gsl_matrix_set_zero(invCovMatrixD);
						gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
						gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

					}
					XMLString::release(&invCovMatrixDXML);

					if (XMLString::equals(methodConjugateGradientWithRestarts, methodStr))
					{
						this->estSlowOptimiserClass->push_back(new RSGISEstimationConjugateGradient2Var2DataWithRestarts(functionHH, functionHV, minMaxStepHeight, minMaxStepDensity, this->initialParClass->at(i), covMatrixP, invCovMatrixD, minError, this->ittmax, numRestarts));
					}
					else
					{
						this->estSlowOptimiserClass->push_back(new RSGISEstimationConjugateGradient2Var2Data(functionHH, functionHV, this->initialParClass->at(i), covMatrixP, invCovMatrixD, minError, this->ittmax));
					}


				}
				else if(XMLString::equals(methodConjugateGradientPolynomial, methodStr))
				{
					cout << "\tUsing ConjugateGradient (polynomial)" << endl;

					// Maximum number of itterations
					XMLCh *ittmaxStr = XMLString::transcode("ittmax");
					if(slowOptimiserElement->hasAttribute(ittmaxStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(ittmaxStr));
						this->ittmax = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						this->ittmax = 10;
					}
					XMLString::release(&ittmaxStr);

					// Number or Restarts
					int numRestarts = 10;
					XMLCh *numRestartsStr = XMLString::transcode("numRestarts");
					if(slowOptimiserElement->hasAttribute(numRestartsStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(numRestartsStr));
						numRestarts = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&numRestartsStr);

					// Minimum error
					double minError = 1e-12;
					XMLCh *targetErrorStr = XMLString::transcode("targetError");
					if(slowOptimiserElement->hasAttribute(targetErrorStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(targetErrorStr));
						minError = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&targetErrorStr);

					/* Get minimum and maximum heights and density.
					 * Setting these is optional and default values will be chosen with no message if
					 * values are not set
					 */

					double *minMaxStepHeight = new double[3];
					double *minMaxStepDensity = new double[3];

					XMLCh *minHeightStr = XMLString::transcode("minHeight");
					if(slowOptimiserElement->hasAttribute(minHeightStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minHeightStr));
						minMaxStepHeight[0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[0] = 1;
					}
					XMLString::release(&minHeightStr);

					XMLCh *maxHeightStr = XMLString::transcode("maxHeight");
					if(slowOptimiserElement->hasAttribute(maxHeightStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxHeightStr));
						minMaxStepHeight[1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[1] = 20;
					}
					XMLString::release(&maxHeightStr);

					XMLCh *heightStepStr = XMLString::transcode("heightStep");
					if(slowOptimiserElement->hasAttribute(heightStepStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(heightStepStr));
						minMaxStepHeight[2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[2] = 0.01;
					}
					XMLString::release(&heightStepStr);

					XMLCh *minDensityStr = XMLString::transcode("minDensity");
					if(slowOptimiserElement->hasAttribute(minDensityStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minDensityStr));
						minMaxStepDensity[0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[0] = 0.1;
					}
					XMLString::release(&minDensityStr);

					XMLCh *maxDensityStr = XMLString::transcode("maxDensity");
					if(slowOptimiserElement->hasAttribute(maxDensityStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxDensityStr));
						minMaxStepDensity[1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[1] = 2;
					}
					XMLString::release(&maxDensityStr);

					XMLCh *densityStepStr = XMLString::transcode("densityStep");
					if(slowOptimiserElement->hasAttribute(densityStepStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(densityStepStr));
						minMaxStepDensity[2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[2] = 0.001;
					}
					XMLString::release(&densityStepStr);

					/************************
					 * Read in coefficients *
					 ************************/

					XMLCh *coefficientsHH = XMLString::transcode("coefficientsHH");
					if(slowOptimiserElement->hasAttribute(coefficientsHH))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(coefficientsHH));
						string coeffHHFile = string(charValue);
						this->coeffHH = matrixUtils.readGSLMatrixFromTxt(coeffHHFile);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No coefficents provided for HH");
					}
					XMLString::release(&coefficientsHH);
					XMLCh *coefficientsHV = XMLString::transcode("coefficientsHV");
					if(slowOptimiserElement->hasAttribute(coefficientsHV))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(coefficientsHV));
						string coeffHVFile = string(charValue);
						this->coeffHV = matrixUtils.readGSLMatrixFromTxt(coeffHVFile);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No coefficents provided for HV");
					}
					XMLString::release(&coefficientsHV);

					/******************************
					 * Set up covariance matrices *
					 ******************************/
					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;

					XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
					if(slowOptimiserElement->hasAttribute(covMatrixPXML))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(covMatrixPXML));
						string covMatrixPFile = string(charValue);
						covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
						XMLString::release(&charValue);
						cout << "\tRead Cm from file" << endl;
						if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
						{
							throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
						}

					}
					else
					{
						// Set to default values if no file (don't print warning)
						covMatrixP = gsl_matrix_alloc(2,2);
						double pCov1 = 1e10;
						double pCov2 = 1e10 * (gsl_vector_get(this->initialParClass->at(i), 1) / gsl_vector_get(this->initialParClass->at(i), 0));
						gsl_matrix_set_zero(covMatrixP);
						gsl_matrix_set(covMatrixP, 0, 0, pCov1);
						gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					}
					XMLString::release(&covMatrixPXML);

					XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
					if(slowOptimiserElement->hasAttribute(invCovMatrixDXML))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(invCovMatrixDXML));
						string invCovMatrixDFile = string(charValue);
						invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
						XMLString::release(&charValue);
						cout << "\tRead 1 / Cd from file" << endl;
						if(invCovMatrixD->size1 != 2 or invCovMatrixD->size2 != 2)
						{
							throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
						}
					}
					else
					{
						// Set to default values if no file (don't print warning)
						invCovMatrixD = gsl_matrix_alloc(2, 2);
						double dCovInv1 = 1;
						double dCovInv2 = 1;
						gsl_matrix_set_zero(invCovMatrixD);
						gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
						gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

					}
					XMLString::release(&invCovMatrixDXML);

					this->estSlowOptimiserClass->push_back(new RSGISEstimationConjugateGradient2DPoly2Channel(coeffHH, coeffHV, covMatrixP, invCovMatrixD, ittmax));

				}
				else if((XMLString::equals(methodExhaustiveSearch, methodStr)) | (XMLString::equals(methodExhaustiveSearchAP, methodStr)))
				{
					cout << "\tUsing Exhaustive Search" << endl;

					double *minMaxStepHeight = new double[3];
					double *minMaxStepDensity = new double[3];

					/* Get minimum and maximum heights and density.
					 * Setting these is optional and default values will be chosen with no message if
					 * values are not set
					 */

					XMLCh *minHeightStr = XMLString::transcode("minHeight");
					if(slowOptimiserElement->hasAttribute(minHeightStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minHeightStr));
						minMaxStepHeight[0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[0] = 1;
					}
					XMLString::release(&minHeightStr);

					XMLCh *maxHeightStr = XMLString::transcode("maxHeight");
					if(slowOptimiserElement->hasAttribute(maxHeightStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxHeightStr));
						minMaxStepHeight[1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[1] = 20;
					}
					XMLString::release(&maxHeightStr);

					XMLCh *heightStepStr = XMLString::transcode("heightStep");
					if(slowOptimiserElement->hasAttribute(heightStepStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(heightStepStr));
						minMaxStepHeight[2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[2] = 0.1;
					}
					XMLString::release(&heightStepStr);

					XMLCh *minDensityStr = XMLString::transcode("minDensity");
					if(slowOptimiserElement->hasAttribute(minDensityStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minDensityStr));
						minMaxStepDensity[0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[0] = 0.1;
					}
					XMLString::release(&minDensityStr);

					XMLCh *maxDensityStr = XMLString::transcode("maxDensity");
					if(slowOptimiserElement->hasAttribute(maxDensityStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxDensityStr));
						minMaxStepDensity[1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[1] = 2;
					}
					XMLString::release(&maxDensityStr);

					XMLCh *densityStepStr = XMLString::transcode("densityStep");
					if(slowOptimiserElement->hasAttribute(densityStepStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(densityStepStr));
						minMaxStepDensity[2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[2] = 0.01;
					}
					XMLString::release(&densityStepStr);

					if (XMLString::equals(methodExhaustiveSearchAP, methodStr))
					{
						/******************************
						 * Set up covariance matrices *
						 ******************************/
						gsl_matrix *covMatrixP;
						gsl_matrix *invCovMatrixD;

						XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
						if(slowOptimiserElement->hasAttribute(covMatrixPXML))
						{
							char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(covMatrixPXML));
							string covMatrixPFile = string(charValue);
							covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
							XMLString::release(&charValue);
							cout << "\tRead Cm from file" << endl;
							if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
							{
								throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
							}

						}
						else
						{
							// Set to default values if no file (don't print warning)
							covMatrixP = gsl_matrix_alloc(2,2);
							double pCov1 = 1e10;
							double pCov2 = 1e10;
							gsl_matrix_set_zero(covMatrixP);
							gsl_matrix_set(covMatrixP, 0, 0, pCov1);
							gsl_matrix_set(covMatrixP, 1, 1, pCov2);

						}
						XMLString::release(&covMatrixPXML);

						XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
						if(slowOptimiserElement->hasAttribute(invCovMatrixDXML))
						{
							char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(invCovMatrixDXML));
							string invCovMatrixDFile = string(charValue);
							invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
							XMLString::release(&charValue);
							cout << "\tRead 1 / Cd from file" << endl;
							if(invCovMatrixD->size1 != 2 or invCovMatrixD->size2 != 2)
							{
								throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
							}
						}
						else
						{
							// Set to default values if no file (don't print warning)
							invCovMatrixD = gsl_matrix_alloc(2, 2);
							double dCovInv1 = 1;
							double dCovInv2 = 1;
							gsl_matrix_set_zero(invCovMatrixD);
							gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
							gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

						}
						XMLString::release(&invCovMatrixDXML);

						this->estSlowOptimiserClass->push_back(new RSGISEstimationExhaustiveSearch2Var2DataWithAP(functionHH, functionHV, covMatrixP, invCovMatrixD, this->initialParClass->at(i), minMaxStepHeight, minMaxStepDensity));
					}
					else
					{
						this->estSlowOptimiserClass->push_back(new RSGISEstimationExhaustiveSearch2Var2Data(functionHH, functionHV, minMaxStepHeight, minMaxStepDensity));
					}
				}
				else if((XMLString::equals(methodSimulatedAnnealing, methodStr)) | (XMLString::equals(methodSimulatedAnnealingAP, methodStr)))
				{
                    std::cout << "\tUsing Simulated Annealing" << std::endl;

					/* Get minimum and maximum heights and density.
					 * Setting these is optional and default values will be chosen with no message if
					 * values are not set
					 */

					double **minMaxStepAll = new double*[2];
                    minMaxStepAll[0] = new double[3];
                    minMaxStepAll[1] = new double[3];

					XMLCh *minHeightStr = XMLString::transcode("minHeight");
					if(slowOptimiserElement->hasAttribute(minHeightStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minHeightStr));
						minMaxStepAll[0][0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepAll[0][0] = 1;
					}
					XMLString::release(&minHeightStr);

					XMLCh *maxHeightStr = XMLString::transcode("maxHeight");
					if(slowOptimiserElement->hasAttribute(maxHeightStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxHeightStr));
						minMaxStepAll[0][1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepAll[0][1] = 20;
					}
					XMLString::release(&maxHeightStr);

					XMLCh *heightStepStr = XMLString::transcode("heightStep");
					if(slowOptimiserElement->hasAttribute(heightStepStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(heightStepStr));
						minMaxStepAll[0][2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepAll[0][2] = 0.01;
					}
					XMLString::release(&heightStepStr);

					XMLCh *minDensityStr = XMLString::transcode("minDensity");
					if(slowOptimiserElement->hasAttribute(minDensityStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minDensityStr));
						minMaxStepAll[1][0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepAll[1][0] = 0.1;
					}
					XMLString::release(&minDensityStr);

					XMLCh *maxDensityStr = XMLString::transcode("maxDensity");
					if(slowOptimiserElement->hasAttribute(maxDensityStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxDensityStr));
						minMaxStepAll[1][1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepAll[1][1] = 2;
					}
					XMLString::release(&maxDensityStr);

					XMLCh *densityStepStr = XMLString::transcode("densityStep");
					if(slowOptimiserElement->hasAttribute(densityStepStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(densityStepStr));
						minMaxStepAll[1][2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepAll[1][2] = 0.001;
					}
					XMLString::release(&densityStepStr);

					/* Get other parameters for simulated annealing.
					 * Setting these is optional and default values will be chosen with no message if
					 * values are not set
					 */

					double startTemp = 1000;
					unsigned int runsStep = 20; // Number of runs at each step size
					unsigned int runsTemp = 100; // Number of times step is changed at each temperature
					double cooling = 0.85; // Cooling factor
					double minEnergy = 1e-12; // Set the target energy
					this->ittmax = 50; // Maximum number of itterations

					// Maximum number of itterations
					XMLCh *ittmaxStr = XMLString::transcode("ittmax");
					if(slowOptimiserElement->hasAttribute(ittmaxStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(ittmaxStr));
						this->ittmax = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&ittmaxStr);

					// Number of runs at each step size
					XMLCh *runsStepStr = XMLString::transcode("runsStep");
					if(slowOptimiserElement->hasAttribute(runsStepStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(runsStepStr));
						runsStep = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&runsStepStr);

					// Number of times step is changed at each temperature
					XMLCh *runsTempStr = XMLString::transcode("runsTemp");
					if(slowOptimiserElement->hasAttribute(runsTempStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(runsTempStr));
						runsTemp = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&runsTempStr);

					// Cooling factor
					XMLCh *coolingStr = XMLString::transcode("cooling");
					if(slowOptimiserElement->hasAttribute(coolingStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(coolingStr));
						cooling = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&coolingStr);

					// Minimum energy
					XMLCh *targetErrorStr = XMLString::transcode("targetError");
					if(slowOptimiserElement->hasAttribute(targetErrorStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(targetErrorStr));
						minEnergy = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&targetErrorStr);

					// Start Temp
					XMLCh *startTempStr = XMLString::transcode("startTemp");
					if(slowOptimiserElement->hasAttribute(startTempStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(startTempStr));
						startTemp = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&startTempStr);

                    /******************************
                     * Set up covariance matrices *
                     ******************************/
                    gsl_matrix *covMatrixP;
                    gsl_matrix *invCovMatrixD;
                    bool covMatrixPProvided = false;
                    bool covMatrixDProvided = false;

                    if (XMLString::equals(methodSimulatedAnnealingAP, methodStr))
					{

						XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
						if(slowOptimiserElement->hasAttribute(covMatrixPXML))
						{
							char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(covMatrixPXML));
							string covMatrixPFile = string(charValue);
							covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
							XMLString::release(&charValue);
							cout << "\tRead Cm from file" << endl;
                            covMatrixPProvided = true;
							if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
							{
								throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
							}

						}
                        XMLString::release(&covMatrixPXML);

                        XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
						if(slowOptimiserElement->hasAttribute(invCovMatrixDXML))
						{
							char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(invCovMatrixDXML));
							string invCovMatrixDFile = string(charValue);
							invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
							XMLString::release(&charValue);
							cout << "\tRead 1 / Cd from file" << endl;
                            covMatrixDProvided = true;
							if(invCovMatrixD->size1 != 2 or invCovMatrixD->size2 != 2)
							{
								throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
							}
						}
						XMLString::release(&invCovMatrixDXML);

                    }

                    if (!covMatrixPProvided) // If not provided, or not being used
                    {
                        // Set to default values if no file (don't print warning)
                        covMatrixP = gsl_matrix_alloc(2,2);
                        double pCov1 = 1e10;
                        double pCov2 = 1e10;
                        gsl_matrix_set_zero(covMatrixP);
                        gsl_matrix_set(covMatrixP, 0, 0, pCov1);
                        gsl_matrix_set(covMatrixP, 1, 1, pCov2);

                    }

                    if(!covMatrixDProvided) // If not provided, or not being used
                    {
                        // Set to default values if no file (don't print warning)
                        invCovMatrixD = gsl_matrix_alloc(2, 2);
                        double dCovInv1 = 1;
                        double dCovInv2 = 1;
                        gsl_matrix_set_zero(invCovMatrixD);
                        gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
                        gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

                    }

                    vector <int> *polyOrdersHH = new vector <int>();
                    polyOrdersHH->push_back(coeffHH->size1);
                    polyOrdersHH->push_back(coeffHH->size2 - 1);

                    vector <int> *polyOrdersHV = new vector <int>();
                    polyOrdersHV->push_back(coeffHV->size1);
                    polyOrdersHV->push_back(coeffHV->size2 - 1);

                    vector <RSGISMathNVariableFunction*> *functionsAll = new vector <RSGISMathNVariableFunction*>;
                    functionsAll->push_back(new RSGISFunctionNDPoly(coeffHH, polyOrdersHH));
                    functionsAll->push_back(new RSGISFunctionNDPoly(coeffHV, polyOrdersHV));

                    this->estSlowOptimiserClass->push_back(new RSGISEstimationSimulatedAnnealingWithAP(functionsAll, minMaxStepAll, minEnergy, startTemp, runsStep, runsTemp, cooling, ittmax, covMatrixP, invCovMatrixD, this->initialParClass->at(i)));

				}
                else if(XMLString::equals(methodNone,methodNone))
                {
                    cout << "\tUsing No optimiser (pixel based estimation)." << endl;
                    this->estSlowOptimiserClass->push_back(new rsgis::radar::RSGISEstimationNoEstimation());
                }
				else if(XMLString::equals(methodAssignAP, methodStr))
				{
					throw RSGISXMLArgumentsException("The optimiser \"assignAP\" cannot be used at the object level");
				}
				else
				{
					throw RSGISXMLArgumentsException("The optimiser was not recognised");
				}
			}

			// Loop through fast optimisers
			for(unsigned int i = 0; i < fastOptimiserNodesList->getLength(); i++)
			{
				fastOptimiserElement = static_cast<DOMElement*>(fastOptimiserNodesList->item(i));
				const XMLCh *methodStr = fastOptimiserElement->getAttribute(XMLString::transcode("method"));

				// Get type of function
				const XMLCh *functionStr = fastOptimiserElement->getAttribute(XMLString::transcode("function"));
				XMLCh *functionLn2Var = XMLString::transcode("ln2Var");
				XMLCh *functionLn2VarQuadratic = XMLString::transcode("ln2VarQuadratic");
				XMLCh *functionLinXfLinY = XMLString::transcode("linXfLinY");
				XMLCh *function2DPoly = XMLString::transcode("2DPoly");

				if (XMLString::equals(functionLn2Var,functionStr))
				{
					gsl_vector *coeffHH;
					XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
					if(fastOptimiserElement->hasAttribute(inCoeffHH))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffHH));
						string inCoeffHHFileName = string(charValue);
						coeffHH = vectorUtils.readGSLVectorFromTxt(inCoeffHHFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
					}
					XMLString::release(&inCoeffHH);

					gsl_vector *coeffHV;
					XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
					if(fastOptimiserElement->hasAttribute(inCoeffHV))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffHV));
						string inCoeffHVFileName = string(charValue);
						coeffHV = vectorUtils.readGSLVectorFromTxt(inCoeffHVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
					}
					XMLString::release(&inCoeffHV);


					RSGISFunction2VarLn *functionLnHH;
					RSGISFunction2VarLn *functionLnHV;

					functionLnHH = new RSGISFunction2VarLn(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2));
					functionLnHV = new RSGISFunction2VarLn(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2));

					this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
					this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;

					gsl_vector_free(coeffHH);
					gsl_vector_free(coeffHV);
				}
				else if (XMLString::equals(functionLn2VarQuadratic,functionStr))
				{
					gsl_vector *coeffHH;
					XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
					if(fastOptimiserElement->hasAttribute(inCoeffHH))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffHH));
						string inCoeffHHFileName = string(charValue);
						coeffHH = vectorUtils.readGSLVectorFromTxt(inCoeffHHFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
					}
					XMLString::release(&inCoeffHH);

					gsl_vector *coeffHV;
					XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
					if(fastOptimiserElement->hasAttribute(inCoeffHV))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffHV));
						string inCoeffHVFileName = string(charValue);
						coeffHV = vectorUtils.readGSLVectorFromTxt(inCoeffHVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
					}
					XMLString::release(&inCoeffHV);

					RSGISFunction2VarLnQuadratic *functionLnHH;
					RSGISFunction2VarLnQuadratic *functionLnHV;

					functionLnHH = new RSGISFunction2VarLnQuadratic(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2));
					functionLnHV = new RSGISFunction2VarLnQuadratic(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2));

					this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
					this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;

					gsl_vector_free(coeffHH);
					gsl_vector_free(coeffHV);
				}
				else if (XMLString::equals(functionLinXfLinY,functionStr))
				{
					gsl_vector *coeffHH;
					XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
					if(fastOptimiserElement->hasAttribute(inCoeffHH))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffHH));
						string inCoeffHHFileName = string(charValue);
						coeffHH = vectorUtils.readGSLVectorFromTxt(inCoeffHHFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
					}
					XMLString::release(&inCoeffHH);

					gsl_vector *coeffHV;
					XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
					if(fastOptimiserElement->hasAttribute(inCoeffHV))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffHV));
						string inCoeffHVFileName = string(charValue);
						coeffHV = vectorUtils.readGSLVectorFromTxt(inCoeffHVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
					}
					XMLString::release(&inCoeffHV);


					RSGISFunctionLinXfLinY *functionLnHH;
					RSGISFunctionLinXfLinY *functionLnHV;

					functionLnHH = new RSGISFunctionLinXfLinY(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2),gsl_vector_get(coeffHH, 3));
					functionLnHV = new RSGISFunctionLinXfLinY(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2),gsl_vector_get(coeffHV, 3));

					this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
					this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;

					gsl_vector_free(coeffHH);
					gsl_vector_free(coeffHV);
				}
				else if (XMLString::equals(function2DPoly,functionStr))
				{
					// Read coefficients
					XMLCh *coefficientsHH = XMLString::transcode("coefficientsHH");
					if(fastOptimiserElement->hasAttribute(coefficientsHH))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(coefficientsHH));
						string coeffHHFile = string(charValue);
						this->coeffHH = matrixUtils.readGSLMatrixFromTxt(coeffHHFile);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No coefficents provided for HH");
					}
					XMLString::release(&coefficientsHH);
					XMLCh *coefficientsHV = XMLString::transcode("coefficientsHV");
					if(fastOptimiserElement->hasAttribute(coefficientsHV))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(coefficientsHV));
						string coeffHVFile = string(charValue);
						this->coeffHV = matrixUtils.readGSLMatrixFromTxt(coeffHVFile);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No coefficents provided for HV");
					}
					XMLString::release(&coefficientsHV);

					// Set up functions
					RSGISFunction2DPoly *functionPolyHH;
					RSGISFunction2DPoly *functionPolyHV;

					functionPolyHH = new RSGISFunction2DPoly(coeffHH);
					functionPolyHV = new RSGISFunction2DPoly(coeffHV);

					this->functionHH = (RSGISMathTwoVariableFunction *) functionPolyHH;
					this->functionHV = (RSGISMathTwoVariableFunction *) functionPolyHV;
				}
				else
				{
					if(!XMLString::equals(methodAssignAP, methodStr))
					{
						throw RSGISXMLArgumentsException("Function type not provieded / not recognised");
					}
				}

				XMLString::release(&functionLn2Var);
				XMLString::release(&functionLn2VarQuadratic);
				XMLString::release(&functionLinXfLinY);
				XMLString::release(&function2DPoly);

				// Get optimisation method
				if(XMLString::equals(methodConjugateGradientWithRestarts, methodStr) | XMLString::equals(methodConjugateGradient, methodStr))
				{
					if(XMLString::equals(methodConjugateGradientWithRestarts, methodStr)){cout << "\tUsing ConjugateGradient" << endl;}
					else{cout << "\tUsing ConjugateGradient - with restarts" << endl;}

					// Maximum number of itterations
					XMLCh *ittmaxStr = XMLString::transcode("ittmax");
					if(fastOptimiserElement->hasAttribute(ittmaxStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(ittmaxStr));
						this->ittmax = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						this->ittmax = 10;
					}
					XMLString::release(&ittmaxStr);

					// Number or Restarts
					int numRestarts = 5;
					XMLCh *numRestartsStr = XMLString::transcode("numRestarts");
					if(fastOptimiserElement->hasAttribute(numRestartsStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(numRestartsStr));
						numRestarts = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&numRestartsStr);

					// Minimum error
					double minError = 1e-12;
					XMLCh *targetErrorStr = XMLString::transcode("targetError");
					if(fastOptimiserElement->hasAttribute(targetErrorStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(targetErrorStr));
						minError = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&targetErrorStr);

					/* Get minimum and maximum heights and density.
					 * Setting these is optional and default values will be chosen with no message if
					 * values are not set
					 */

					double *minMaxStepHeight = new double[3];
					double *minMaxStepDensity = new double[3];

					XMLCh *minHeightStr = XMLString::transcode("minHeight");
					if(fastOptimiserElement->hasAttribute(minHeightStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minHeightStr));
						minMaxStepHeight[0] = mathUtils.strtodouble(string(charValue));
						this->useDefaultMinMax = false;
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[0] = 1;
					}
					XMLString::release(&minHeightStr);

					XMLCh *maxHeightStr = XMLString::transcode("maxHeight");
					if(fastOptimiserElement->hasAttribute(maxHeightStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxHeightStr));
						minMaxStepHeight[1] = mathUtils.strtodouble(string(charValue));
						this->useDefaultMinMax = false;
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[1] = 20;
					}
					XMLString::release(&maxHeightStr);

					XMLCh *heightStepStr = XMLString::transcode("heightStep");
					if(fastOptimiserElement->hasAttribute(heightStepStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(heightStepStr));
						minMaxStepHeight[2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[2] = 0.01;
					}
					XMLString::release(&heightStepStr);

					XMLCh *minDensityStr = XMLString::transcode("minDensity");
					if(fastOptimiserElement->hasAttribute(minDensityStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minDensityStr));
						minMaxStepDensity[0] = mathUtils.strtodouble(string(charValue));
						this->useDefaultMinMax = false;
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[0] = 0.1;
					}
					XMLString::release(&minDensityStr);

					XMLCh *maxDensityStr = XMLString::transcode("maxDensity");
					if(fastOptimiserElement->hasAttribute(maxDensityStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxDensityStr));
						minMaxStepDensity[1] = mathUtils.strtodouble(string(charValue));
						this->useDefaultMinMax = false;
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[1] = 2;
					}
					XMLString::release(&maxDensityStr);

					XMLCh *densityStepStr = XMLString::transcode("densityStep");
					if(fastOptimiserElement->hasAttribute(densityStepStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(densityStepStr));
						minMaxStepDensity[2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[2] = 0.001;
					}
					XMLString::release(&densityStepStr);

					/******************************
					 * Set up covariance matrices *
					 ******************************/
					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;

					XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
					if(fastOptimiserElement->hasAttribute(covMatrixPXML))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(covMatrixPXML));
						string covMatrixPFile = string(charValue);
						covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
						XMLString::release(&charValue);
						cout << "\tRead Cm from file" << endl;
						if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
						{
							throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
						}

					}
					else
					{
						// Set to default values if no file (don't print warning)
						covMatrixP = gsl_matrix_alloc(2,2);
                        // Values of matrix based on assumption that standard deviation is 50 percent of value
						double pCov1 = pow(gsl_vector_get(this->initialParClass->at(i),0)*0.5,2)*1e6;
						double pCov2 = pow(gsl_vector_get(this->initialParClass->at(i),1)*0.5,2)*1e6*
                        gsl_vector_get(this->initialParClass->at(i),0)/gsl_vector_get(this->initialParClass->at(i),1);
						gsl_matrix_set_zero(covMatrixP);
						gsl_matrix_set(covMatrixP, 0, 0, pCov1);
						gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					}
					XMLString::release(&covMatrixPXML);

					XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
					if(fastOptimiserElement->hasAttribute(invCovMatrixDXML))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(invCovMatrixDXML));
						string invCovMatrixDFile = string(charValue);
						invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
						XMLString::release(&charValue);
						cout << "\tRead 1 / Cd from file" << endl;
						if(invCovMatrixD->size1 != 2 or invCovMatrixD->size2 != 2)
						{
							throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
						}
					}
					else
					{
						// Set to default values if no file (don't print warning)
						invCovMatrixD = gsl_matrix_alloc(2, 2);
						double dCovInv1 = 1;
						double dCovInv2 = 1;
						gsl_matrix_set_zero(invCovMatrixD);
						gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
						gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

					}
					XMLString::release(&invCovMatrixDXML);

					if(XMLString::equals(methodConjugateGradient, methodStr))
					{
						this->estFastOptimiserClass->push_back(new RSGISEstimationConjugateGradient2Var2Data(functionHH, functionHV, this->initialParClass->at(i), covMatrixP, invCovMatrixD, minError, this->ittmax));
					}
					else
					{
						this->estFastOptimiserClass->push_back(new RSGISEstimationConjugateGradient2Var2DataWithRestarts(functionHH, functionHV, minMaxStepHeight, minMaxStepDensity,this->initialParClass->at(i), covMatrixP, invCovMatrixD, minError, this->ittmax, numRestarts));
					}

					// Add to min / max values for inversion
					if (!this->useDefaultMinMax)
					{
						this->minMaxValuesClass[i] = new double*[2];
						this->minMaxValuesClass[i][0] = new double[2];
						this->minMaxValuesClass[i][1] = new double[2];

						this->minMaxValuesClass[i][0][0] = minMaxStepHeight[0];
						this->minMaxValuesClass[i][0][1] = minMaxStepHeight[1];
						this->minMaxValuesClass[i][1][0] = minMaxStepDensity[0];
						this->minMaxValuesClass[i][1][1] = minMaxStepDensity[1];
					}


				}
				else if(XMLString::equals(methodConjugateGradientPolynomial, methodStr))
				{
					cout << "\tUsing ConjugateGradient (polynomial)" << endl;

					// Maximum number of itterations
					XMLCh *ittmaxStr = XMLString::transcode("ittmax");
					if(fastOptimiserElement->hasAttribute(ittmaxStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(ittmaxStr));
						this->ittmax = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						this->ittmax = 10;
					}
					XMLString::release(&ittmaxStr);

					// Number or Restarts
					int numRestarts = 10;
					XMLCh *numRestartsStr = XMLString::transcode("numRestarts");
					if(fastOptimiserElement->hasAttribute(numRestartsStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(numRestartsStr));
						numRestarts = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&numRestartsStr);

					// Minimum error
					double minError = 1e-12;
					XMLCh *targetErrorStr = XMLString::transcode("targetError");
					if(fastOptimiserElement->hasAttribute(targetErrorStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(targetErrorStr));
						minError = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&targetErrorStr);

					/* Get minimum and maximum heights and density.
					 * Setting these is optional and default values will be chosen with no message if
					 * values are not set
					 */

					double *minMaxStepHeight = new double[3];
					double *minMaxStepDensity = new double[3];

					XMLCh *minHeightStr = XMLString::transcode("minHeight");
					if(fastOptimiserElement->hasAttribute(minHeightStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minHeightStr));
						minMaxStepHeight[0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[0] = 1;
					}
					XMLString::release(&minHeightStr);

					XMLCh *maxHeightStr = XMLString::transcode("maxHeight");
					if(fastOptimiserElement->hasAttribute(maxHeightStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxHeightStr));
						minMaxStepHeight[1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[1] = 20;
					}
					XMLString::release(&maxHeightStr);

					XMLCh *heightStepStr = XMLString::transcode("heightStep");
					if(fastOptimiserElement->hasAttribute(heightStepStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(heightStepStr));
						minMaxStepHeight[2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[2] = 0.01;
					}
					XMLString::release(&heightStepStr);

					XMLCh *minDensityStr = XMLString::transcode("minDensity");
					if(fastOptimiserElement->hasAttribute(minDensityStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minDensityStr));
						minMaxStepDensity[0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[0] = 0.1;
					}
					XMLString::release(&minDensityStr);

					XMLCh *maxDensityStr = XMLString::transcode("maxDensity");
					if(fastOptimiserElement->hasAttribute(maxDensityStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxDensityStr));
						minMaxStepDensity[1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[1] = 2;
					}
					XMLString::release(&maxDensityStr);

					XMLCh *densityStepStr = XMLString::transcode("densityStep");
					if(fastOptimiserElement->hasAttribute(densityStepStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(densityStepStr));
						minMaxStepDensity[2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[2] = 0.001;
					}
					XMLString::release(&densityStepStr);

					/************************
					 * Read in coefficients *
					 ************************/

					XMLCh *coefficientsHH = XMLString::transcode("coefficientsHH");
					if(fastOptimiserElement->hasAttribute(coefficientsHH))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(coefficientsHH));
						string coeffHHFile = string(charValue);
						this->coeffHH = matrixUtils.readGSLMatrixFromTxt(coeffHHFile);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No coefficents provided for HH");
					}
					XMLString::release(&coefficientsHH);
					XMLCh *coefficientsHV = XMLString::transcode("coefficientsHV");
					if(fastOptimiserElement->hasAttribute(coefficientsHV))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(coefficientsHV));
						string coeffHVFile = string(charValue);
						this->coeffHV = matrixUtils.readGSLMatrixFromTxt(coeffHVFile);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No coefficents provided for HV");
					}
					XMLString::release(&coefficientsHV);

					/******************************
					 * Set up covariance matrices *
					 ******************************/
					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;

					XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
					if(fastOptimiserElement->hasAttribute(covMatrixPXML))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(covMatrixPXML));
						string covMatrixPFile = string(charValue);
						covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
						XMLString::release(&charValue);
						cout << "\tRead Cm from file" << endl;
						if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
						{
							throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
						}

					}
					else
					{
						// Set to default values if no file (don't print warning)
						covMatrixP = gsl_matrix_alloc(2,2);
                        // Values of matrix based on assumption that standard deviation is 50 percent of value
						double pCov1 = pow(gsl_vector_get(this->initialParClass->at(i),0)*0.5,2)*1e6;
						double pCov2 = pow(gsl_vector_get(this->initialParClass->at(i),1)*0.5,2)*1e6*
                        gsl_vector_get(this->initialParClass->at(i),0)/gsl_vector_get(this->initialParClass->at(i),1);
						gsl_matrix_set_zero(covMatrixP);
						gsl_matrix_set(covMatrixP, 0, 0, pCov1);
						gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					}
					XMLString::release(&covMatrixPXML);

					XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
					if(fastOptimiserElement->hasAttribute(invCovMatrixDXML))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(invCovMatrixDXML));
						string invCovMatrixDFile = string(charValue);
						invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
						XMLString::release(&charValue);
						cout << "\tRead 1 / Cd from file" << endl;
						if(invCovMatrixD->size1 != 2 or invCovMatrixD->size2 != 2)
						{
							throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
						}
					}
					else
					{
						// Set to default values if no file (don't print warning)
						invCovMatrixD = gsl_matrix_alloc(2, 2);
						double dCovInv1 = 1;
						double dCovInv2 = 1;
						gsl_matrix_set_zero(invCovMatrixD);
						gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
						gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

					}
					XMLString::release(&invCovMatrixDXML);

					this->estFastOptimiserClass->push_back(new RSGISEstimationConjugateGradient2DPoly2Channel(coeffHH, coeffHV, covMatrixP, invCovMatrixD, ittmax));

				}
				else if((XMLString::equals(methodExhaustiveSearch, methodStr)) | (XMLString::equals(methodExhaustiveSearchAP, methodStr)))
				{
					cout << "\tUsing Exhaustive Search" << endl;

					double *minMaxStepHeight = new double[3];
					double *minMaxStepDensity = new double[3];

					/* Get minimum and maximum heights and density.
					 * Setting these is optional and default values will be chosen with no message if
					 * values are not set
					 */

					XMLCh *minHeightStr = XMLString::transcode("minHeight");
					if(fastOptimiserElement->hasAttribute(minHeightStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minHeightStr));
						minMaxStepHeight[0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[0] = 1;
					}
					XMLString::release(&minHeightStr);

					XMLCh *maxHeightStr = XMLString::transcode("maxHeight");
					if(fastOptimiserElement->hasAttribute(maxHeightStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxHeightStr));
						minMaxStepHeight[1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[1] = 20;
					}
					XMLString::release(&maxHeightStr);

					XMLCh *heightStepStr = XMLString::transcode("heightStep");
					if(fastOptimiserElement->hasAttribute(heightStepStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(heightStepStr));
						minMaxStepHeight[2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepHeight[2] = 0.1;
					}
					XMLString::release(&heightStepStr);

					XMLCh *minDensityStr = XMLString::transcode("minDensity");
					if(fastOptimiserElement->hasAttribute(minDensityStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minDensityStr));
						minMaxStepDensity[0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[0] = 0.1;
					}
					XMLString::release(&minDensityStr);

					XMLCh *maxDensityStr = XMLString::transcode("maxDensity");
					if(fastOptimiserElement->hasAttribute(maxDensityStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxDensityStr));
						minMaxStepDensity[1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[1] = 2;
					}
					XMLString::release(&maxDensityStr);

					XMLCh *densityStepStr = XMLString::transcode("densityStep");
					if(fastOptimiserElement->hasAttribute(densityStepStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(densityStepStr));
						minMaxStepDensity[2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepDensity[2] = 0.01;
					}
					XMLString::release(&densityStepStr);

					if (XMLString::equals(methodExhaustiveSearchAP, methodStr))
					{
						/******************************
						 * Set up covariance matrices *
						 ******************************/
						gsl_matrix *covMatrixP;
						gsl_matrix *invCovMatrixD;

						XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
						if(fastOptimiserElement->hasAttribute(covMatrixPXML))
						{
							char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(covMatrixPXML));
							string covMatrixPFile = string(charValue);
							covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
							XMLString::release(&charValue);
							cout << "\tRead Cm from file" << endl;
							if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
							{
								throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
							}

						}
						else
						{
							// Set to default values if no file (don't print warning)
							covMatrixP = gsl_matrix_alloc(2,2);
							double pCov1 = 1e10;
							double pCov2 = 1e10;
							gsl_matrix_set_zero(covMatrixP);
							gsl_matrix_set(covMatrixP, 0, 0, pCov1);
							gsl_matrix_set(covMatrixP, 1, 1, pCov2);

						}
						XMLString::release(&covMatrixPXML);

						XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
						if(fastOptimiserElement->hasAttribute(invCovMatrixDXML))
						{
							char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(invCovMatrixDXML));
							string invCovMatrixDFile = string(charValue);
							invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
							XMLString::release(&charValue);
							cout << "\tRead 1 / Cd from file" << endl;
							if(invCovMatrixD->size1 != 2 or invCovMatrixD->size2 != 2)
							{
								throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
							}
						}
						else
						{
							// Set to default values if no file (don't print warning)
							invCovMatrixD = gsl_matrix_alloc(2, 2);
							double dCovInv1 = 1;
							double dCovInv2 = 1;
							gsl_matrix_set_zero(invCovMatrixD);
							gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
							gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

						}
						XMLString::release(&invCovMatrixDXML);

						this->estFastOptimiserClass->push_back(new RSGISEstimationExhaustiveSearch2Var2DataWithAP(functionHH, functionHV, covMatrixP, invCovMatrixD,this->initialParClass->at(i), minMaxStepHeight, minMaxStepDensity));
					}
					else
					{
						this->estFastOptimiserClass->push_back(new RSGISEstimationExhaustiveSearch2Var2Data(functionHH, functionHV, minMaxStepHeight, minMaxStepDensity));
					}
					// Set min / max values
					this->minMaxValuesClass[i] = new double*[2];
					this->minMaxValuesClass[i][0] = new double[2];
					this->minMaxValuesClass[i][1] = new double[2];

					this->minMaxValuesClass[i][0][0] = minMaxStepHeight[0];
					this->minMaxValuesClass[i][0][1] = minMaxStepHeight[1];
					this->minMaxValuesClass[i][1][0] = minMaxStepDensity[0];
					this->minMaxValuesClass[i][1][1] = minMaxStepDensity[1];
				}
				else if((XMLString::equals(methodSimulatedAnnealing, methodStr)) | (XMLString::equals(methodSimulatedAnnealingAP, methodStr)))
				{
					cout << "\tUsing Simulated Annealing" << endl;

					/* Get minimum and maximum heights and density.
					 * Setting these is optional and default values will be chosen with no message if
					 * values are not set
					 */

					double **minMaxStepAll = new double*[2];
                    minMaxStepAll[0] = new double[3];
                    minMaxStepAll[1] = new double[3];

					XMLCh *minHeightStr = XMLString::transcode("minHeight");
					if(fastOptimiserElement->hasAttribute(minHeightStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minHeightStr));
						minMaxStepAll[0][0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepAll[0][0] = 1;
					}
					XMLString::release(&minHeightStr);

					XMLCh *maxHeightStr = XMLString::transcode("maxHeight");
					if(fastOptimiserElement->hasAttribute(maxHeightStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxHeightStr));
						minMaxStepAll[0][1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepAll[0][1] = 20;
					}
					XMLString::release(&maxHeightStr);

					XMLCh *heightStepStr = XMLString::transcode("heightStep");
					if(fastOptimiserElement->hasAttribute(heightStepStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(heightStepStr));
						minMaxStepAll[0][2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepAll[0][2] = 0.01;
					}
					XMLString::release(&heightStepStr);

					XMLCh *minDensityStr = XMLString::transcode("minDensity");
					if(fastOptimiserElement->hasAttribute(minDensityStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minDensityStr));
						minMaxStepAll[1][0] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepAll[1][0] = 0.1;
					}
					XMLString::release(&minDensityStr);

					XMLCh *maxDensityStr = XMLString::transcode("maxDensity");
					if(fastOptimiserElement->hasAttribute(maxDensityStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxDensityStr));
						minMaxStepAll[1][1] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepAll[1][1] = 2;
					}
					XMLString::release(&maxDensityStr);

					XMLCh *densityStepStr = XMLString::transcode("densityStep");
					if(fastOptimiserElement->hasAttribute(densityStepStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(densityStepStr));
						minMaxStepAll[1][2] = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						minMaxStepAll[1][2] = 0.001;
					}
					XMLString::release(&densityStepStr);

					/* Get other parameters for simulated annealing.
					 * Setting these is optional and default values will be chosen with no message if
					 * values are not set
					 */

					double startTemp = 1000;
					unsigned int runsStep = 20; // Number of runs at each step size
					unsigned int runsTemp = 100; // Number of times step is changed at each temperature
					double cooling = 0.85; // Cooling factor
					double minEnergy = 1e-12; // Set the target energy
					this->ittmax = 50; // Maximum number of itterations

					// Maximum number of itterations
					XMLCh *ittmaxStr = XMLString::transcode("ittmax");
					if(fastOptimiserElement->hasAttribute(ittmaxStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(ittmaxStr));
						this->ittmax = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&ittmaxStr);

					// Number of runs at each step size
					XMLCh *runsStepStr = XMLString::transcode("runsStep");
					if(fastOptimiserElement->hasAttribute(runsStepStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(runsStepStr));
						runsStep = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&runsStepStr);

					// Number of times step is changed at each temperature
					XMLCh *runsTempStr = XMLString::transcode("runsTemp");
					if(fastOptimiserElement->hasAttribute(runsTempStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(runsTempStr));
						runsTemp = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&runsTempStr);

					// Cooling factor
					XMLCh *coolingStr = XMLString::transcode("cooling");
					if(fastOptimiserElement->hasAttribute(coolingStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(coolingStr));
						cooling = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&coolingStr);

					// Minimum energy
					XMLCh *targetErrorStr = XMLString::transcode("targetError");
					if(fastOptimiserElement->hasAttribute(targetErrorStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(targetErrorStr));
						minEnergy = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&targetErrorStr);

					// Start Temp
					XMLCh *startTempStr = XMLString::transcode("startTemp");
					if(fastOptimiserElement->hasAttribute(startTempStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(startTempStr));
						startTemp = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&startTempStr);

                    /******************************
                     * Set up covariance matrices *
                     ******************************/
                    gsl_matrix *covMatrixP;
                    gsl_matrix *invCovMatrixD;
                    bool covMatrixPProvided = false;
                    bool covMatrixDProvided = false;

                    if (XMLString::equals(methodSimulatedAnnealingAP, methodStr))
					{

						XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
						if(fastOptimiserElement->hasAttribute(covMatrixPXML))
						{
							char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(covMatrixPXML));
							string covMatrixPFile = string(charValue);
							covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
							XMLString::release(&charValue);
							cout << "\tRead Cm from file" << endl;
                            covMatrixPProvided = true;
							if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
							{
								throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
							}

						}
                        XMLString::release(&covMatrixPXML);

                        XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
						if(fastOptimiserElement->hasAttribute(invCovMatrixDXML))
						{
							char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(invCovMatrixDXML));
							string invCovMatrixDFile = string(charValue);
							invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
							XMLString::release(&charValue);
							cout << "\tRead 1 / Cd from file" << endl;
                            covMatrixDProvided = true;
							if(invCovMatrixD->size1 != 2 or invCovMatrixD->size2 != 2)
							{
								throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
							}
						}
						XMLString::release(&invCovMatrixDXML);

                    }

                    if (!covMatrixPProvided) // If not provided, or not being used
                    {
                        // Set to default values if no file (don't print warning)
                        covMatrixP = gsl_matrix_alloc(2,2);
                        double pCov1 = 1e10;
                        double pCov2 = 1e10;
                        gsl_matrix_set_zero(covMatrixP);
                        gsl_matrix_set(covMatrixP, 0, 0, pCov1);
                        gsl_matrix_set(covMatrixP, 1, 1, pCov2);

                    }

                    if(!covMatrixDProvided) // If not provided, or not being used
                    {
                        // Set to default values if no file (don't print warning)
                        invCovMatrixD = gsl_matrix_alloc(2, 2);
                        double dCovInv1 = 1;
                        double dCovInv2 = 1;
                        gsl_matrix_set_zero(invCovMatrixD);
                        gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
                        gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

                    }

                    vector <int> *polyOrdersHH = new vector <int>();
                    polyOrdersHH->push_back(coeffHH->size1);
                    polyOrdersHH->push_back(coeffHH->size2 - 1);

                    vector <int> *polyOrdersHV = new vector <int>();
                    polyOrdersHV->push_back(coeffHV->size1);
                    polyOrdersHV->push_back(coeffHV->size2 - 1);

                    vector <RSGISMathNVariableFunction*> *functionsAll = new vector <RSGISMathNVariableFunction*>;
                    functionsAll->push_back(new RSGISFunctionNDPoly(coeffHH, polyOrdersHH));
                    functionsAll->push_back(new RSGISFunctionNDPoly(coeffHV, polyOrdersHV));

                    this->estFastOptimiserClass->push_back(new RSGISEstimationSimulatedAnnealingWithAP(functionsAll, minMaxStepAll, minEnergy, startTemp, runsStep, runsTemp, cooling, ittmax, covMatrixP, invCovMatrixD, this->initialPar));


					// Set min / max values
					this->minMaxValuesClass[i] = new double*[2];
					this->minMaxValuesClass[i][0] = new double[2];
					this->minMaxValuesClass[i][1] = new double[2];

					this->minMaxValuesClass[i][0][0] = minMaxStepAll[0][0];
					this->minMaxValuesClass[i][0][1] = minMaxStepAll[0][1];
					this->minMaxValuesClass[i][1][0] = minMaxStepAll[1][0];
					this->minMaxValuesClass[i][1][1] = minMaxStepAll[1][1];

				}
				else if(XMLString::equals(methodAssignAP, methodStr))
				{
					cout << "\tUsing Values from objects (assign)" << endl;

					this->estFastOptimiserClass->push_back(new rsgis::radar::RSGISEstimationAssignAP());
				}
				else
				{
					throw RSGISXMLArgumentsException("The optimiser was not recognised");
				}
			}

			XMLString::release(&methodConjugateGradient);
			XMLString::release(&methodConjugateGradientWithRestarts);
			XMLString::release(&methodExhaustiveSearch);
			XMLString::release(&methodExhaustiveSearchAP);
			XMLString::release(&methodSimulatedAnnealing);
			XMLString::release(&methodSimulatedAnnealingAP);
			XMLString::release(&methodAssignAP);
            XMLString::release(&methodNone);

		}
		else
		{
			throw RSGISXMLArgumentsException("Parameters to be retrieved not recognised or compatible with this option");
		}
	}
	//---- Full Pol Single Species - Object Based ---//
	else if((XMLString::equals(typeFullPolObject,optionStr)) | (XMLString::equals(typeFullPolObjectObjAP,optionStr)))
	{
		// Object based estimation
		cout << "\tObject based inversion" << endl;
		this->createOutSHP = false;
		this->useRasPoly = false;
		this->force = true;
		this->copyAttributes = true;
		this->classField = "";
		this->useClass = false;
		this->useDefaultMinMax = false;
		this->objAP = false;
		this->nBands = 4;
		this->option = RSGISExeEstimationAlgorithm::fullPolObject;


		XMLCh *objectXMLStr = XMLString::transcode("object");
		if(argElement->hasAttribute(objectXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(objectXMLStr));
			this->inputObjPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'object\' attribute was provided.");
		}
		XMLString::release(&objectXMLStr);

		XMLCh *rasterXMLStr = XMLString::transcode("raster");
		if(argElement->hasAttribute(rasterXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rasterXMLStr));
			this->inputRasPolys = string(charValue);
			this->useRasPoly = true;
			XMLString::release(&charValue);
		}
		else
		{
			cout << "\tNot using rasterised version of polygon (may be slower)" << endl;
			this->useRasPoly = false;
		}
		XMLString::release(&rasterXMLStr);

		XMLCh *classFieldXMLStr = XMLString::transcode("classField");
		if(argElement->hasAttribute(classFieldXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(classFieldXMLStr));
			this->classField = string(charValue);
			XMLString::release(&charValue);
			this->useClass = true;
			cout << "\tUsing field: \"" << this->classField << "\" to define classes" << endl;
		}
		XMLString::release(&classFieldXMLStr);

		XMLCh *outSHPXMLStr = XMLString::transcode("outSHP");
		if(argElement->hasAttribute(outSHPXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outSHPXMLStr));
			this->outputSHP = string(charValue);
			XMLString::release(&charValue);
			this->createOutSHP = true;
			cout << "\tCreating output shapefile" << endl;
		}
		XMLString::release(&outSHPXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			if (this->createOutSHP)
			{
				cout << "\tOverwriting output shapefile, if it exists." << endl;
			}
		}
		XMLString::release(&forceXMLStr);

		if(XMLString::equals(typeHeightDensity,parametersStr) | XMLString::equals(typeCDepthDensity,parametersStr))
		{
			if(XMLString::equals(typeHeightDensity,parametersStr))
			{
				cout << "\tParameters to be retrieved are height and stem density" << endl;
				this->parameters = heightDensity;
				this->objAPParField = new string[2];
			}
			else
			{
				cout << "\tParameters to be retrieved are canopy depth and density" << endl;
				this->parameters = cDepthDensity;
				this->objAPParField = new string[2];
			}

			// Get fields to use for AP
			if (XMLString::equals(typeFullPolObjectObjAP,optionStr))
			{
				this->objAP = true;
				this->objAPParField = new string[2];
				XMLCh *heightFieldXMLStr = XMLString::transcode("heightField");
				if(argElement->hasAttribute(heightFieldXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(heightFieldXMLStr));
					this->objAPParField[0] = string(charValue);
					XMLString::release(&charValue);
					cout << "\tUsing field: \"" << this->objAPParField[0] << "\" to provide a priori estimates of heights" << endl;
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'heightField\' attribute was provided. The field name for a priori values of height must be provided");
				}
				XMLString::release(&heightFieldXMLStr);

				XMLCh *densityFieldXMLStr = XMLString::transcode("densityField");
				if(argElement->hasAttribute(densityFieldXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(densityFieldXMLStr));
					this->objAPParField[1] = string(charValue);
					XMLString::release(&charValue);
					cout << "\tUsing field: \"" << this->objAPParField[1] << "\" to provide a priori estimates of densities" << endl;
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'densityField\' attribute was provided. The field name for a priori values of density must be provided");
				}
				XMLString::release(&densityFieldXMLStr);
			}

			this->nPar = 2;
			this->nBands = 4; // nPar + 2

			this->initialParClass= new vector <gsl_vector*>;
			this->estFastOptimiserClass = new vector <RSGISEstimationOptimiser*>;
			this->estSlowOptimiserClass = new vector <RSGISEstimationOptimiser*>;

			/**************************************
			 * Get method to use for optimisation *
			 **************************************/
			XMLCh *methodConjugateGradient = XMLString::transcode("conjugateGradient");
			XMLCh *methodConjugateGradientWithRestarts = XMLString::transcode("conjugateGradientWithRestarts");
			XMLCh *methodExhaustiveSearch = XMLString::transcode("exhaustiveSearch");
			XMLCh *methodExhaustiveSearchAP = XMLString::transcode("exhaustiveSearchAP");
			XMLCh *methodSimulatedAnnealing = XMLString::transcode("simulatedAnnealing");
			XMLCh *methodSimulatedAnnealingAP = XMLString::transcode("simulatedAnnealingAP");
			XMLCh *methodAssignAP = XMLString::transcode("assignAP");
            XMLCh *methodNone= XMLString::transcode("none");

			DOMNodeList *slowOptimiserNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:estSlowOptimiserParameters"));
			DOMNodeList *fastOptimiserNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:estFastOptimiserParameters"));
			DOMElement *slowOptimiserElement = NULL;
			DOMElement *fastOptimiserElement = NULL;

			// Set up array to hold min / max values, these are read in with fast optimisers
			this->minMaxValuesClass = new double**[fastOptimiserNodesList->getLength()];

			// Loop through slow optimisers
			for(unsigned int i = 0; i < slowOptimiserNodesList->getLength(); i++)
			{
				slowOptimiserElement = static_cast<DOMElement*>(slowOptimiserNodesList->item(i));
				const XMLCh *methodStr = slowOptimiserElement->getAttribute(XMLString::transcode("method"));

				double initialHeight = 0;
				double initialDensity = 0;

				// Set initial parameters
				XMLCh *initialHeightStr = XMLString::transcode("initialHeight");
				if(slowOptimiserElement->hasAttribute(initialHeightStr))
				{
					char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(initialHeightStr));
					initialHeight = mathUtils.strtodouble(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No initial height provided");
				}
				XMLString::release(&initialHeightStr);
				XMLCh *initialDensityStr = XMLString::transcode("initialDensity");
				if(slowOptimiserElement->hasAttribute(initialDensityStr))
				{
					char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(initialDensityStr));
					initialDensity = mathUtils.strtodouble(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No initial density provided");
				}
				XMLString::release(&initialDensityStr);

				// Save parameters to vector
				this->initialParClass->push_back(gsl_vector_alloc(this->nPar));
				gsl_vector_set(this->initialParClass->at(i),0,initialHeight);
				gsl_vector_set(this->initialParClass->at(i),1,initialDensity);

				// Get type of function
				const XMLCh *functionStr = slowOptimiserElement->getAttribute(XMLString::transcode("function"));
				XMLCh *functionLn2Var = XMLString::transcode("ln2Var");
				XMLCh *functionLn2VarQuadratic = XMLString::transcode("ln2VarQuadratic");
				XMLCh *functionLinXfLinY = XMLString::transcode("linXfLinY");
				XMLCh *function2DPoly = XMLString::transcode("2DPoly");

				if ((XMLString::equals(functionLn2Var,functionStr)) || (XMLString::equals(functionLn2VarQuadratic,functionStr)) || (XMLString::equals(functionLinXfLinY,functionStr)))
				{

					gsl_vector *coeffHH;
					XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
					if(slowOptimiserElement->hasAttribute(inCoeffHH))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffHH));
						string inCoeffHHFileName = string(charValue);
						coeffHH = vectorUtils.readGSLVectorFromTxt(inCoeffHHFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
					}
					XMLString::release(&inCoeffHH);

					gsl_vector *coeffHV;
					XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
					if(slowOptimiserElement->hasAttribute(inCoeffHV))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffHV));
						string inCoeffHVFileName = string(charValue);
						coeffHV = vectorUtils.readGSLVectorFromTxt(inCoeffHVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
					}
					XMLString::release(&inCoeffHV);

					gsl_vector *coeffVV;
					XMLCh *inCoeffVV = XMLString::transcode("coefficientsVV"); // Coefficients
					if(slowOptimiserElement->hasAttribute(inCoeffVV))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffVV));
						string inCoeffVVFileName = string(charValue);
						coeffVV = vectorUtils.readGSLVectorFromTxt(inCoeffVVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for VV");
					}
					XMLString::release(&inCoeffVV);

					if (XMLString::equals(functionLn2Var,functionStr))
					{
						RSGISFunction2VarLn *functionLnHH;
						RSGISFunction2VarLn *functionLnHV;
						RSGISFunction2VarLn *functionLnVV;

						functionLnHH = new RSGISFunction2VarLn(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2));
						functionLnHV = new RSGISFunction2VarLn(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2));
						functionLnVV = new RSGISFunction2VarLn(gsl_vector_get(coeffVV, 0), gsl_vector_get(coeffVV, 1),gsl_vector_get(coeffVV, 2));

						this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
						this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;
						this->functionVV = (RSGISMathTwoVariableFunction *) functionLnVV;
					}
					else if (XMLString::equals(functionLn2VarQuadratic,functionStr))
					{
						RSGISFunction2VarLnQuadratic *functionLnHH;
						RSGISFunction2VarLnQuadratic *functionLnHV;
						RSGISFunction2VarLnQuadratic *functionLnVV;

						functionLnHH = new RSGISFunction2VarLnQuadratic(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2));
						functionLnHV = new RSGISFunction2VarLnQuadratic(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2));
						functionLnVV = new RSGISFunction2VarLnQuadratic(gsl_vector_get(coeffVV, 0), gsl_vector_get(coeffVV, 1),gsl_vector_get(coeffVV, 2));

						this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
						this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;
						this->functionVV = (RSGISMathTwoVariableFunction *) functionLnVV;
					}
					else if (XMLString::equals(functionLinXfLinY,functionStr))
					{
						RSGISFunctionLinXfLinY *functionLnHH;
						RSGISFunctionLinXfLinY *functionLnHV;
						RSGISFunctionLinXfLinY *functionLnVV;

						functionLnHH = new RSGISFunctionLinXfLinY(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2),gsl_vector_get(coeffHH, 3));
						functionLnHV = new RSGISFunctionLinXfLinY(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2),gsl_vector_get(coeffHV, 3));
						functionLnVV = new RSGISFunctionLinXfLinY(gsl_vector_get(coeffVV, 0), gsl_vector_get(coeffVV, 1),gsl_vector_get(coeffVV, 2),gsl_vector_get(coeffVV, 3));

						this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
						this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;
						this->functionVV = (RSGISMathTwoVariableFunction *) functionLnVV;
					}

					gsl_vector_free(coeffHH);
					gsl_vector_free(coeffHV);
					gsl_vector_free(coeffVV);

				}
				else if (XMLString::equals(function2DPoly,functionStr))
				{
					XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
					if(slowOptimiserElement->hasAttribute(inCoeffHH))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffHH));
						string inCoeffHHFileName = string(charValue);
						this->coeffHH = matrixUtils.readGSLMatrixFromTxt(inCoeffHHFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
					}
					XMLString::release(&inCoeffHH);

					XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
					if(slowOptimiserElement->hasAttribute(inCoeffHV))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffHV));
						string inCoeffHVFileName = string(charValue);
						this->coeffHV = matrixUtils.readGSLMatrixFromTxt(inCoeffHVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
					}
					XMLString::release(&inCoeffHV);

					XMLCh *inCoeffVV = XMLString::transcode("coefficientsVV"); // Coefficients
					if(slowOptimiserElement->hasAttribute(inCoeffVV))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffVV));
						string inCoeffVVFileName = string(charValue);
						this->coeffVV = matrixUtils.readGSLMatrixFromTxt(inCoeffVVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for VV");
					}
					XMLString::release(&inCoeffVV);

					RSGISFunction2DPoly *functionPolyHH;
					RSGISFunction2DPoly *functionPolyHV;
					RSGISFunction2DPoly *functionPolyVV;

					functionPolyHH = new RSGISFunction2DPoly(this->coeffHH);
					functionPolyHV = new RSGISFunction2DPoly(this->coeffHV);
					functionPolyVV = new RSGISFunction2DPoly(this->coeffVV);

					this->functionHH = (RSGISMathTwoVariableFunction *) functionPolyHH;
					this->functionHV = (RSGISMathTwoVariableFunction *) functionPolyHV;
					this->functionVV = (RSGISMathTwoVariableFunction *) functionPolyVV;
				}
				else
				{
                    if(!XMLString::equals(methodNone, methodStr))
					{
						throw RSGISXMLArgumentsException("Function type not provieded / not recognised");
					}
				}

				XMLString::release(&functionLn2Var);
				XMLString::release(&functionLn2VarQuadratic);
				XMLString::release(&functionLinXfLinY);
				XMLString::release(&function2DPoly);

                /* Get minimum and maximum heights and density.
                 * Setting these is optional and default values will be chosen with no message if
                 * values are not set
                 */

                double *minMaxStepHeight = new double[3];
                double *minMaxStepDensity = new double[3];

                XMLCh *minHeightStr = XMLString::transcode("minHeight");
                if(slowOptimiserElement->hasAttribute(minHeightStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minHeightStr));
                    minMaxStepHeight[0] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepHeight[0] = 1;
                }
                XMLString::release(&minHeightStr);

                XMLCh *maxHeightStr = XMLString::transcode("maxHeight");
                if(slowOptimiserElement->hasAttribute(maxHeightStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxHeightStr));
                    minMaxStepHeight[1] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepHeight[1] = 20;
                }
                XMLString::release(&maxHeightStr);

                XMLCh *heightStepStr = XMLString::transcode("heightStep");
                if(slowOptimiserElement->hasAttribute(heightStepStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(heightStepStr));
                    minMaxStepHeight[2] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepHeight[2] = 0.01;
                }
                XMLString::release(&heightStepStr);

                XMLCh *minDensityStr = XMLString::transcode("minDensity");
                if(slowOptimiserElement->hasAttribute(minDensityStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minDensityStr));
                    minMaxStepDensity[0] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepDensity[0] = 0.1;
                }
                XMLString::release(&minDensityStr);

                XMLCh *maxDensityStr = XMLString::transcode("maxDensity");
                if(slowOptimiserElement->hasAttribute(maxDensityStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxDensityStr));
                    minMaxStepDensity[1] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepDensity[1] = 2;
                }
                XMLString::release(&maxDensityStr);

                XMLCh *densityStepStr = XMLString::transcode("densityStep");
                if(slowOptimiserElement->hasAttribute(densityStepStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(densityStepStr));
                    minMaxStepDensity[2] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepDensity[2] = 0.001;
                }
                XMLString::release(&densityStepStr);


                double **minMaxStepAll = new double*[2];
                minMaxStepAll[0] = new double[3];
                minMaxStepAll[1] = new double[3];
                minMaxStepAll[0][0] = minMaxStepHeight[0];
                minMaxStepAll[0][1] = minMaxStepHeight[1];
                minMaxStepAll[0][2] = minMaxStepHeight[2];
                minMaxStepAll[1][0] = minMaxStepDensity[0];
                minMaxStepAll[1][1] = minMaxStepDensity[1];
                minMaxStepAll[1][2] = minMaxStepDensity[2];


                // Add to min / max values for inversion
                this->minMaxValuesClass[i][0] = new double[2];
                this->minMaxValuesClass[i][1] = new double[2];

                this->minMaxValuesClass[i][0][0] = minMaxStepHeight[0];
                this->minMaxValuesClass[i][0][1] = minMaxStepHeight[1];
                this->minMaxValuesClass[i][1][0] = minMaxStepDensity[0];
                this->minMaxValuesClass[i][1][1] = minMaxStepDensity[1];

				// Get optimisation method
				if(XMLString::equals(methodConjugateGradientWithRestarts, methodStr) | XMLString::equals(methodConjugateGradient, methodStr))
				{
					if (XMLString::equals(methodConjugateGradientWithRestarts, methodStr)) {cout << "\tUsing ConjugateGradient - with restarts" << endl;}
					else{cout << "\tUsing ConjugateGradient" << endl;}

					// Maximum number of itterations
					XMLCh *ittmaxStr = XMLString::transcode("ittmax");
					if(slowOptimiserElement->hasAttribute(ittmaxStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(ittmaxStr));
						this->ittmax = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						this->ittmax = 10;
					}
					XMLString::release(&ittmaxStr);

					// Number or Restarts
					int numRestarts = 5;
					XMLCh *numRestartsStr = XMLString::transcode("numRestarts");
					if(slowOptimiserElement->hasAttribute(numRestartsStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(numRestartsStr));
						numRestarts = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&numRestartsStr);

					// Minimum error
					double minError = 1e-12;
					XMLCh *targetErrorStr = XMLString::transcode("targetError");
					if(slowOptimiserElement->hasAttribute(targetErrorStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(targetErrorStr));
						minError = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&targetErrorStr);


					/******************************
					 * Set up covariance matrices *
					 ******************************/
					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;

					XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
					if(slowOptimiserElement->hasAttribute(covMatrixPXML))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(covMatrixPXML));
						string covMatrixPFile = string(charValue);
						covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
						XMLString::release(&charValue);
						cout << "\tRead Cm from file" << endl;
						if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
						{
							throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
						}

					}
					else
					{
						// Set to default values if no file (don't print warning)
						covMatrixP = gsl_matrix_alloc(2,2);
                        // Values of matrix based on assumption that standard deviation is 50 percent of value
						double pCov1 = pow(gsl_vector_get(this->initialParClass->at(i),0)*0.5,2)*1e6;
						double pCov2 = pow(gsl_vector_get(this->initialParClass->at(i),1)*0.5,2)*1e6*
                        gsl_vector_get(this->initialParClass->at(i),0)/gsl_vector_get(this->initialParClass->at(i),1);
						gsl_matrix_set_zero(covMatrixP);
						gsl_matrix_set(covMatrixP, 0, 0, pCov1);
						gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					}
					XMLString::release(&covMatrixPXML);

					XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
					if(slowOptimiserElement->hasAttribute(invCovMatrixDXML))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(invCovMatrixDXML));
						string invCovMatrixDFile = string(charValue);
						invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
						XMLString::release(&charValue);
						cout << "\tRead 1 / Cd from file" << endl;
						if(invCovMatrixD->size1 != 3 or invCovMatrixD->size2 != 3)
						{
							throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
						}
					}
					else
					{
						// Set to default values if no file (don't print warning)
						invCovMatrixD = gsl_matrix_alloc(3, 3);
						double dCovInv1 = 1;
						double dCovInv2 = 1;
						double dCovInv3 = 1;
						gsl_matrix_set_zero(invCovMatrixD);
						gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
						gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
						gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

					}
					XMLString::release(&invCovMatrixDXML);

					if (XMLString::equals(methodConjugateGradientWithRestarts, methodStr))
					{
						this->estSlowOptimiserClass->push_back(new RSGISEstimationConjugateGradient2Var3DataWithRestarts(functionHH, functionHV, functionVV, minMaxStepHeight, minMaxStepDensity, this->initialParClass->at(i), covMatrixP, invCovMatrixD, minError, this->ittmax, numRestarts));
					}
					else
					{
						this->estSlowOptimiserClass->push_back(new RSGISEstimationConjugateGradient2Var3Data(functionHH, functionHV, functionVV, this->initialParClass->at(i), covMatrixP, invCovMatrixD, minError, this->ittmax));
					}


				}
				else if((XMLString::equals(methodExhaustiveSearch, methodStr)) | (XMLString::equals(methodExhaustiveSearchAP, methodStr)))
				{
					cout << "\tUsing Exhaustive Search" << endl;


					if (XMLString::equals(methodExhaustiveSearchAP, methodStr))
					{
						/******************************
						 * Set up covariance matrices *
						 ******************************/

						throw RSGISXMLArgumentsException("ES with a priori not yet implemented!\n");

						gsl_matrix *covMatrixP;
						gsl_matrix *invCovMatrixD;

						XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
						if(slowOptimiserElement->hasAttribute(covMatrixPXML))
						{
							char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(covMatrixPXML));
							string covMatrixPFile = string(charValue);
							covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
							XMLString::release(&charValue);
							cout << "\tRead Cm from file" << endl;
							if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
							{
								throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
							}

						}
						else
						{
							// Set to default values if no file (don't print warning)
							covMatrixP = gsl_matrix_alloc(2,2);
							double pCov1 = 1e10;
							double pCov2 = 1e10;
							gsl_matrix_set_zero(covMatrixP);
							gsl_matrix_set(covMatrixP, 0, 0, pCov1);
							gsl_matrix_set(covMatrixP, 1, 1, pCov2);

						}
						XMLString::release(&covMatrixPXML);

						XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
						if(slowOptimiserElement->hasAttribute(invCovMatrixDXML))
						{
							char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(invCovMatrixDXML));
							string invCovMatrixDFile = string(charValue);
							invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
							XMLString::release(&charValue);
							cout << "\tRead 1 / Cd from file" << endl;
							if(invCovMatrixD->size1 != 2 or invCovMatrixD->size2 != 2)
							{
								throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
							}
						}
						else
						{
							// Set to default values if no file (don't print warning)
							invCovMatrixD = gsl_matrix_alloc(2, 2);
							double dCovInv1 = 1;
							double dCovInv2 = 1;
							gsl_matrix_set_zero(invCovMatrixD);
							gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
							gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

						}
						XMLString::release(&invCovMatrixDXML);

						//this->estSlowOptimiserClass->push_back(new RSGISEstimationExhaustiveSearch2Var2DataWithAP(functionHH, functionHV, covMatrixP, invCovMatrixD, this->initialParClass->at(i), minMaxStepHeight, minMaxStepDensity));
					}
					else
					{
						this->estSlowOptimiserClass->push_back(new RSGISEstimationExhaustiveSearch2Var3Data(functionHH, functionHV, functionVV, minMaxStepHeight, minMaxStepDensity));
					}
				}
				else if((XMLString::equals(methodSimulatedAnnealing, methodStr)) | (XMLString::equals(methodSimulatedAnnealingAP, methodStr)))
				{
					cout << "\tUsing Simulated Annealing" << endl;

					/* Get other parameters for simulated annealing.
					 * Setting these is optional and default values will be chosen with no message if
					 * values are not set
					 */

					double startTemp = 1000;
					unsigned int runsStep = 20; // Number of runs at each step size
					unsigned int runsTemp = 100; // Number of times step is changed at each temperature
					double cooling = 0.85; // Cooling factor
					double minEnergy = 1e-12; // Set the target energy
					this->ittmax = 50; // Maximum number of itterations

					// Maximum number of itterations
					XMLCh *ittmaxStr = XMLString::transcode("ittmax");
					if(slowOptimiserElement->hasAttribute(ittmaxStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(ittmaxStr));
						this->ittmax = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&ittmaxStr);

					// Number of runs at each step size
					XMLCh *runsStepStr = XMLString::transcode("runsStep");
					if(slowOptimiserElement->hasAttribute(runsStepStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(runsStepStr));
						runsStep = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&runsStepStr);

					// Number of times step is changed at each temperature
					XMLCh *runsTempStr = XMLString::transcode("runsTemp");
					if(slowOptimiserElement->hasAttribute(runsTempStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(runsTempStr));
						runsTemp = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&runsTempStr);

					// Cooling factor
					XMLCh *coolingStr = XMLString::transcode("cooling");
					if(slowOptimiserElement->hasAttribute(coolingStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(coolingStr));
						cooling = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&coolingStr);

					// Minimum energy
					XMLCh *targetErrorStr = XMLString::transcode("targetError");
					if(slowOptimiserElement->hasAttribute(targetErrorStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(targetErrorStr));
						minEnergy = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&targetErrorStr);

					// Start Temp
					XMLCh *startTempStr = XMLString::transcode("startTemp");
					if(slowOptimiserElement->hasAttribute(startTempStr))
					{
						char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(startTempStr));
						startTemp = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&startTempStr);

                    /******************************
                     * Set up covariance matrices *
                     ******************************/
                    gsl_matrix *covMatrixP;
                    gsl_matrix *invCovMatrixD;
                    bool covMatrixPProvided = false;
                    bool covMatrixDProvided = false;

                    if (XMLString::equals(methodSimulatedAnnealingAP, methodStr))
					{

						XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
						if(slowOptimiserElement->hasAttribute(covMatrixPXML))
						{
							char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(covMatrixPXML));
							string covMatrixPFile = string(charValue);
							covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
							XMLString::release(&charValue);
							cout << "\tRead Cm from file" << endl;
                            covMatrixPProvided = true;
							if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
							{
								throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
							}

						}
                        XMLString::release(&covMatrixPXML);

                        XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
						if(slowOptimiserElement->hasAttribute(invCovMatrixDXML))
						{
							char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(invCovMatrixDXML));
							string invCovMatrixDFile = string(charValue);
							invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
							XMLString::release(&charValue);
							cout << "\tRead 1 / Cd from file" << endl;
                            covMatrixDProvided = true;
							if(invCovMatrixD->size1 != 3 or invCovMatrixD->size2 != 3)
							{
								throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
							}
						}
						XMLString::release(&invCovMatrixDXML);

                    }

                    if (!covMatrixPProvided) // If not provided, or not being used
                    {
                        // Set to default values if no file (don't print warning)
                        covMatrixP = gsl_matrix_alloc(2,2);
                        double pCov1 = 1e10;
                        double pCov2 = 1e10;
                        gsl_matrix_set_zero(covMatrixP);
                        gsl_matrix_set(covMatrixP, 0, 0, pCov1);
                        gsl_matrix_set(covMatrixP, 1, 1, pCov2);

                    }

                    if(!covMatrixDProvided) // If not provided, or not being used
                    {
                        // Set to default values if no file (don't print warning)
                        invCovMatrixD = gsl_matrix_alloc(3, 3);
                        double dCovInv1 = 1;
                        double dCovInv2 = 1;
                        double dCovInv3 = 1;
                        gsl_matrix_set_zero(invCovMatrixD);
                        gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
                        gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
                        gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

                    }

                    vector <int> *polyOrdersHH = new vector <int>();
                    polyOrdersHH->push_back(this->coeffHH->size1);
                    polyOrdersHH->push_back(this->coeffHH->size2 - 1);

                    vector <int> *polyOrdersHV = new vector <int>();
                    polyOrdersHV->push_back(this->coeffHV->size1);
                    polyOrdersHV->push_back(this->coeffHV->size2 - 1);

                    vector <int> *polyOrdersVV = new vector <int>();
                    polyOrdersVV->push_back(this->coeffVV->size1);
                    polyOrdersVV->push_back(this->coeffVV->size2 - 1);

                    vector <RSGISMathNVariableFunction*> *functionsAll = new vector <RSGISMathNVariableFunction*>;
                    functionsAll->push_back(new RSGISFunctionNDPoly(this->coeffHH, polyOrdersHH));
                    functionsAll->push_back(new RSGISFunctionNDPoly(this->coeffHV, polyOrdersHV));
                    functionsAll->push_back(new RSGISFunctionNDPoly(this->coeffVV, polyOrdersVV));

                    this->estSlowOptimiserClass->push_back(new RSGISEstimationSimulatedAnnealingWithAP(functionsAll, minMaxStepAll, minEnergy, startTemp, runsStep, runsTemp, cooling, ittmax, covMatrixP, invCovMatrixD, this->initialParClass->at(i)));

				}
                else if(XMLString::equals(methodNone,methodNone))
                {
                    cout << "\tUsing No optimiser (pixel based estimation)." << endl;
                    this->estSlowOptimiserClass->push_back(new rsgis::radar::RSGISEstimationNoEstimation());
                }
				else if(XMLString::equals(methodAssignAP, methodStr))
				{
					throw RSGISXMLArgumentsException("The optimiser \"assignAP\" cannot be used at the object level");
				}
				else
				{
				    throw RSGISXMLArgumentsException("Optimiser not provieded / not recognised");
				}
			}

			// Loop through fast optimisers
			for(unsigned int i = 0; i < fastOptimiserNodesList->getLength(); i++)
			{
				fastOptimiserElement = static_cast<DOMElement*>(fastOptimiserNodesList->item(i));
				const XMLCh *methodStr = fastOptimiserElement->getAttribute(XMLString::transcode("method"));

				double initialHeight = 0;
				double initialDensity = 0;

				// Set initial parameters
				XMLCh *initialHeightStr = XMLString::transcode("initialHeight");
				if(fastOptimiserElement->hasAttribute(initialHeightStr))
				{
					char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(initialHeightStr));
					initialHeight = mathUtils.strtodouble(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					if(!XMLString::equals(methodAssignAP, methodStr)){throw RSGISXMLArgumentsException("No initial height provided");}
				}
				XMLString::release(&initialHeightStr);
				XMLCh *initialDensityStr = XMLString::transcode("initialDensity");
				if(fastOptimiserElement->hasAttribute(initialDensityStr))
				{
					char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(initialDensityStr));
					initialDensity = mathUtils.strtodouble(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					if(!XMLString::equals(methodAssignAP, methodStr)){throw RSGISXMLArgumentsException("No initial density provided");}
				}
				XMLString::release(&initialDensityStr);

				// Save parameters to vector
				this->initialParClass->push_back(gsl_vector_alloc(this->nPar));
				gsl_vector_set(this->initialParClass->at(i),0,initialHeight);
				gsl_vector_set(this->initialParClass->at(i),1,initialDensity);

				// Get type of function
				const XMLCh *functionStr = fastOptimiserElement->getAttribute(XMLString::transcode("function"));
				XMLCh *functionLn2Var = XMLString::transcode("ln2Var");
				XMLCh *functionLn2VarQuadratic = XMLString::transcode("ln2VarQuadratic");
				XMLCh *functionLinXfLinY = XMLString::transcode("linXfLinY");
				XMLCh *function2DPoly = XMLString::transcode("2DPoly");

				if ((XMLString::equals(functionLn2Var,functionStr)) || (XMLString::equals(functionLn2VarQuadratic,functionStr)) || (XMLString::equals(functionLinXfLinY,functionStr)))
				{

					gsl_vector *coeffHH;
					XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
					if(fastOptimiserElement->hasAttribute(inCoeffHH))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffHH));
						string inCoeffHHFileName = string(charValue);
						coeffHH = vectorUtils.readGSLVectorFromTxt(inCoeffHHFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
					}
					XMLString::release(&inCoeffHH);

					gsl_vector *coeffHV;
					XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
					if(fastOptimiserElement->hasAttribute(inCoeffHV))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffHV));
						string inCoeffHVFileName = string(charValue);
						coeffHV = vectorUtils.readGSLVectorFromTxt(inCoeffHVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
					}
					XMLString::release(&inCoeffHV);

					gsl_vector *coeffVV;
					XMLCh *inCoeffVV = XMLString::transcode("coefficientsVV"); // Coefficients
					if(fastOptimiserElement->hasAttribute(inCoeffVV))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffVV));
						string inCoeffVVFileName = string(charValue);
						coeffVV = vectorUtils.readGSLVectorFromTxt(inCoeffVVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for VV");
					}
					XMLString::release(&inCoeffVV);

					if (XMLString::equals(functionLn2Var,functionStr))
					{
						RSGISFunction2VarLn *functionLnHH;
						RSGISFunction2VarLn *functionLnHV;
						RSGISFunction2VarLn *functionLnVV;

						functionLnHH = new RSGISFunction2VarLn(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2));
						functionLnHV = new RSGISFunction2VarLn(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2));
						functionLnVV = new RSGISFunction2VarLn(gsl_vector_get(coeffVV, 0), gsl_vector_get(coeffVV, 1),gsl_vector_get(coeffVV, 2));

						this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
						this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;
						this->functionVV = (RSGISMathTwoVariableFunction *) functionLnVV;
					}
					else if (XMLString::equals(functionLn2VarQuadratic,functionStr))
					{
						RSGISFunction2VarLnQuadratic *functionLnHH;
						RSGISFunction2VarLnQuadratic *functionLnHV;
						RSGISFunction2VarLnQuadratic *functionLnVV;

						functionLnHH = new RSGISFunction2VarLnQuadratic(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2));
						functionLnHV = new RSGISFunction2VarLnQuadratic(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2));
						functionLnVV = new RSGISFunction2VarLnQuadratic(gsl_vector_get(coeffVV, 0), gsl_vector_get(coeffVV, 1),gsl_vector_get(coeffVV, 2));

						this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
						this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;
						this->functionVV = (RSGISMathTwoVariableFunction *) functionLnVV;
					}
					else if (XMLString::equals(functionLinXfLinY,functionStr))
					{
						RSGISFunctionLinXfLinY *functionLnHH;
						RSGISFunctionLinXfLinY *functionLnHV;
						RSGISFunctionLinXfLinY *functionLnVV;

						functionLnHH = new RSGISFunctionLinXfLinY(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2),gsl_vector_get(coeffHH, 3));
						functionLnHV = new RSGISFunctionLinXfLinY(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2),gsl_vector_get(coeffHV, 3));
						functionLnVV = new RSGISFunctionLinXfLinY(gsl_vector_get(coeffVV, 0), gsl_vector_get(coeffVV, 1),gsl_vector_get(coeffVV, 2),gsl_vector_get(coeffVV, 3));

						this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
						this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;
						this->functionVV = (RSGISMathTwoVariableFunction *) functionLnVV;
					}

					gsl_vector_free(coeffHH);
					gsl_vector_free(coeffHV);
					gsl_vector_free(coeffVV);

				}
				else if (XMLString::equals(function2DPoly,functionStr))
				{
					gsl_matrix *coeffHH;
					XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
					if(fastOptimiserElement->hasAttribute(inCoeffHH))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffHH));
						string inCoeffHHFileName = string(charValue);
						coeffHH = matrixUtils.readGSLMatrixFromTxt(inCoeffHHFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
					}
					XMLString::release(&inCoeffHH);

					gsl_matrix *coeffHV;
					XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
					if(fastOptimiserElement->hasAttribute(inCoeffHV))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffHV));
						string inCoeffHVFileName = string(charValue);
						coeffHV = matrixUtils.readGSLMatrixFromTxt(inCoeffHVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
					}
					XMLString::release(&inCoeffHV);

					gsl_matrix *coeffVV;
					XMLCh *inCoeffVV = XMLString::transcode("coefficientsVV"); // Coefficients
					if(fastOptimiserElement->hasAttribute(inCoeffVV))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffVV));
						string inCoeffVVFileName = string(charValue);
						coeffVV = matrixUtils.readGSLMatrixFromTxt(inCoeffVVFileName);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No Coefficients not Provided for VV");
					}
					XMLString::release(&inCoeffVV);

					RSGISFunction2DPoly *functionPolyHH;
					RSGISFunction2DPoly *functionPolyHV;
					RSGISFunction2DPoly *functionPolyVV;

					functionPolyHH = new RSGISFunction2DPoly(coeffHH);
					functionPolyHV = new RSGISFunction2DPoly(coeffHV);
					functionPolyVV = new RSGISFunction2DPoly(coeffVV);

					this->functionHH = (RSGISMathTwoVariableFunction *) functionPolyHH;
					this->functionHV = (RSGISMathTwoVariableFunction *) functionPolyHV;
					this->functionVV = (RSGISMathTwoVariableFunction *) functionPolyVV;

				}
				else
				{
					if(!XMLString::equals(methodAssignAP, methodStr))
					{
						throw RSGISXMLArgumentsException("Function type not provieded / not recognised");
					}
				}

				XMLString::release(&functionLn2Var);
				XMLString::release(&functionLn2VarQuadratic);
				XMLString::release(&functionLinXfLinY);
				XMLString::release(&function2DPoly);

                double *minMaxStepHeight = new double[3];
                double *minMaxStepDensity = new double[3];

                /* Get minimum and maximum heights and density.
                 * Setting these is optional and default values will be chosen with no message if
                 * values are not set
                 */

                XMLCh *minHeightStr = XMLString::transcode("minHeight");
                if(fastOptimiserElement->hasAttribute(minHeightStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minHeightStr));
                    minMaxStepHeight[0] = mathUtils.strtodouble(string(charValue));
                    this->useDefaultMinMax = false;
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepHeight[0] = 1;
                }
                XMLString::release(&minHeightStr);

                XMLCh *maxHeightStr = XMLString::transcode("maxHeight");
                if(fastOptimiserElement->hasAttribute(maxHeightStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxHeightStr));
                    minMaxStepHeight[1] = mathUtils.strtodouble(string(charValue));
                    this->useDefaultMinMax = false;
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepHeight[1] = 20;
                }
                XMLString::release(&maxHeightStr);

                XMLCh *heightStepStr = XMLString::transcode("heightStep");
                if(fastOptimiserElement->hasAttribute(heightStepStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(heightStepStr));
                    minMaxStepHeight[2] = mathUtils.strtodouble(string(charValue));
                    this->useDefaultMinMax = false;
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepHeight[2] = 0.1;
                }
                XMLString::release(&heightStepStr);

                XMLCh *minDensityStr = XMLString::transcode("minDensity");
                if(fastOptimiserElement->hasAttribute(minDensityStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minDensityStr));
                    minMaxStepDensity[0] = mathUtils.strtodouble(string(charValue));
                    this->useDefaultMinMax = false;
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepDensity[0] = 0.1;
                }
                XMLString::release(&minDensityStr);

                XMLCh *maxDensityStr = XMLString::transcode("maxDensity");
                if(fastOptimiserElement->hasAttribute(maxDensityStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxDensityStr));
                    minMaxStepDensity[1] = mathUtils.strtodouble(string(charValue));
                    this->useDefaultMinMax = false;
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepDensity[1] = 2;
                }
                XMLString::release(&maxDensityStr);

                XMLCh *densityStepStr = XMLString::transcode("densityStep");
                if(fastOptimiserElement->hasAttribute(densityStepStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(densityStepStr));
                    minMaxStepDensity[2] = mathUtils.strtodouble(string(charValue));
                    this->useDefaultMinMax = false;
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepDensity[2] = 0.01;
                }
                XMLString::release(&densityStepStr);

                double **minMaxStepAll = new double*[2];
                minMaxStepAll[0] = new double[3];
                minMaxStepAll[1] = new double[3];
                minMaxStepAll[0][0] = minMaxStepHeight[0];
                minMaxStepAll[0][1] = minMaxStepHeight[1];
                minMaxStepAll[0][2] = minMaxStepHeight[2];
                minMaxStepAll[1][0] = minMaxStepDensity[0];
                minMaxStepAll[1][1] = minMaxStepDensity[1];
                minMaxStepAll[1][2] = minMaxStepDensity[2];

                // Add to min / max values for inversion
                this->minMaxValuesClass[i] = new double*[2];
                this->minMaxValuesClass[i][0] = new double[2];
                this->minMaxValuesClass[i][1] = new double[2];

                this->minMaxValuesClass[i][0][0] = minMaxStepHeight[0];
                this->minMaxValuesClass[i][0][1] = minMaxStepHeight[1];
                this->minMaxValuesClass[i][1][0] = minMaxStepDensity[0];
                this->minMaxValuesClass[i][1][1] = minMaxStepDensity[1];

				// Get optimisation method
				if(XMLString::equals(methodConjugateGradientWithRestarts, methodStr) | XMLString::equals(methodConjugateGradient, methodStr))
				{
					if (XMLString::equals(methodConjugateGradientWithRestarts, methodStr)) {cout << "\tUsing ConjugateGradient - with restarts" << endl;}
					else{cout << "\tUsing ConjugateGradient" << endl;}

					// Maximum number of itterations
					XMLCh *ittmaxStr = XMLString::transcode("ittmax");
					if(fastOptimiserElement->hasAttribute(ittmaxStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(ittmaxStr));
						this->ittmax = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						this->ittmax = 10;
					}
					XMLString::release(&ittmaxStr);

					// Number or Restarts
					int numRestarts = 5;
					XMLCh *numRestartsStr = XMLString::transcode("numRestarts");
					if(fastOptimiserElement->hasAttribute(numRestartsStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(numRestartsStr));
						numRestarts = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&numRestartsStr);

					// Minimum error
					double minError = 1e-12;
					XMLCh *targetErrorStr = XMLString::transcode("targetError");
					if(fastOptimiserElement->hasAttribute(targetErrorStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(targetErrorStr));
						minError = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&targetErrorStr);

					/******************************
					 * Set up covariance matrices *
					 ******************************/
					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;

					XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
					if(fastOptimiserElement->hasAttribute(covMatrixPXML))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(covMatrixPXML));
						string covMatrixPFile = string(charValue);
						covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
						XMLString::release(&charValue);
						cout << "\tRead Cm from file" << endl;
						if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
						{
							throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
						}

					}
					else
					{
						// Set to default values if no file (don't print warning)
						covMatrixP = gsl_matrix_alloc(2,2);
                        // Values of matrix based on assumption that standard deviation is 50 percent of value
						double pCov1 = pow(gsl_vector_get(this->initialParClass->at(i),0)*0.5,2)*1e6;
						double pCov2 = pow(gsl_vector_get(this->initialParClass->at(i),1)*0.5,2)*1e6*
                        gsl_vector_get(this->initialParClass->at(i),0)/gsl_vector_get(this->initialParClass->at(i),1);
						gsl_matrix_set_zero(covMatrixP);
						gsl_matrix_set(covMatrixP, 0, 0, pCov1);
						gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					}
					XMLString::release(&covMatrixPXML);

					XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
					if(fastOptimiserElement->hasAttribute(invCovMatrixDXML))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(invCovMatrixDXML));
						string invCovMatrixDFile = string(charValue);
						invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
						XMLString::release(&charValue);
						cout << "\tRead 1 / Cd from file" << endl;
						if(invCovMatrixD->size1 != 3 or invCovMatrixD->size2 != 3)
						{
							throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
						}
					}
					else
					{
						// Set to default values if no file (don't print warning)
						invCovMatrixD = gsl_matrix_alloc(3, 3);
						double dCovInv1 = 1;
						double dCovInv2 = 1;
						double dCovInv3 = 1;
						gsl_matrix_set_zero(invCovMatrixD);
						gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
						gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
						gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

					}
					XMLString::release(&invCovMatrixDXML);

					if (XMLString::equals(methodConjugateGradientWithRestarts, methodStr))
					{
						this->estFastOptimiserClass->push_back(new RSGISEstimationConjugateGradient2Var3DataWithRestarts(functionHH, functionHV, functionVV, minMaxStepHeight, minMaxStepDensity, this->initialParClass->at(i), covMatrixP, invCovMatrixD, minError, this->ittmax, numRestarts));
					}
					else
					{
						this->estFastOptimiserClass->push_back(new RSGISEstimationConjugateGradient2Var3Data(functionHH, functionHV, functionVV, this->initialParClass->at(i), covMatrixP, invCovMatrixD, minError, this->ittmax));
					}

				}
				else if((XMLString::equals(methodExhaustiveSearch, methodStr)) | (XMLString::equals(methodExhaustiveSearchAP, methodStr)))
				{
					cout << "\tUsing Exhaustive Search" << endl;

					if (XMLString::equals(methodExhaustiveSearchAP, methodStr))
					{
						/******************************
						 * Set up covariance matrices *
						 ******************************/

						throw RSGISXMLArgumentsException("ES with a priori not yet implemented!\n");

						gsl_matrix *covMatrixP;
						gsl_matrix *invCovMatrixD;

						XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
						if(fastOptimiserElement->hasAttribute(covMatrixPXML))
						{
							char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(covMatrixPXML));
							string covMatrixPFile = string(charValue);
							covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
							XMLString::release(&charValue);
							cout << "\tRead Cm from file" << endl;
							if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
							{
								throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
							}

						}
						else
						{
							// Set to default values if no file (don't print warning)
							covMatrixP = gsl_matrix_alloc(2,2);
							double pCov1 = 1e10;
							double pCov2 = 1e10;
							gsl_matrix_set_zero(covMatrixP);
							gsl_matrix_set(covMatrixP, 0, 0, pCov1);
							gsl_matrix_set(covMatrixP, 1, 1, pCov2);

						}
						XMLString::release(&covMatrixPXML);

						XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
						if(fastOptimiserElement->hasAttribute(invCovMatrixDXML))
						{
							char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(invCovMatrixDXML));
							string invCovMatrixDFile = string(charValue);
							invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
							XMLString::release(&charValue);
							cout << "\tRead 1 / Cd from file" << endl;
							if(invCovMatrixD->size1 != 2 or invCovMatrixD->size2 != 2)
							{
								throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
							}
						}
						else
						{
							// Set to default values if no file (don't print warning)
							invCovMatrixD = gsl_matrix_alloc(2, 2);
							double dCovInv1 = 1;
							double dCovInv2 = 1;
							gsl_matrix_set_zero(invCovMatrixD);
							gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
							gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

						}
						XMLString::release(&invCovMatrixDXML);

						//this->estFastOptimiserClass->push_back(new RSGISEstimationExhaustiveSearch2Var2DataWithAP(functionHH, functionHV, covMatrixP, invCovMatrixD, this->initialParClass->at(i), minMaxStepHeight, minMaxStepDensity));
					}
					else
					{
						this->estFastOptimiserClass->push_back(new RSGISEstimationExhaustiveSearch2Var3Data(functionHH, functionHV, functionVV, minMaxStepHeight, minMaxStepDensity));
					}


				}
				else if((XMLString::equals(methodSimulatedAnnealing, methodStr)) | (XMLString::equals(methodSimulatedAnnealingAP, methodStr)))
				{
					cout << "\tUsing Simulated Annealing" << endl;

					/* Get other parameters for simulated annealing.
					 * Setting these is optional and default values will be chosen with no message if
					 * values are not set
					 */

					double startTemp = 1000;
					unsigned int runsStep = 20; // Number of runs at each step size
					unsigned int runsTemp = 100; // Number of times step is changed at each temperature
					double cooling = 0.85; // Cooling factor
					double minEnergy = 1e-12; // Set the target energy
					this->ittmax = 50; // Maximum number of itterations

					// Maximum number of itterations
					XMLCh *ittmaxStr = XMLString::transcode("ittmax");
					if(fastOptimiserElement->hasAttribute(ittmaxStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(ittmaxStr));
						this->ittmax = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&ittmaxStr);

					// Number of runs at each step size
					XMLCh *runsStepStr = XMLString::transcode("runsStep");
					if(fastOptimiserElement->hasAttribute(runsStepStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(runsStepStr));
						runsStep = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&runsStepStr);

					// Number of times step is changed at each temperature
					XMLCh *runsTempStr = XMLString::transcode("runsTemp");
					if(fastOptimiserElement->hasAttribute(runsTempStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(runsTempStr));
						runsTemp = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&runsTempStr);

					// Cooling factor
					XMLCh *coolingStr = XMLString::transcode("cooling");
					if(fastOptimiserElement->hasAttribute(coolingStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(coolingStr));
						cooling = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&coolingStr);

					// Minimum energy
					XMLCh *targetErrorStr = XMLString::transcode("targetError");
					if(fastOptimiserElement->hasAttribute(targetErrorStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(targetErrorStr));
						minEnergy = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&targetErrorStr);

					// Start Temp
					XMLCh *startTempStr = XMLString::transcode("startTemp");
					if(fastOptimiserElement->hasAttribute(startTempStr))
					{
						char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(startTempStr));
						startTemp = mathUtils.strtodouble(string(charValue));
						XMLString::release(&charValue);
					}
					XMLString::release(&startTempStr);


                    /******************************
                     * Set up covariance matrices *
                     ******************************/
                    gsl_matrix *covMatrixP;
                    gsl_matrix *invCovMatrixD;
                    bool covMatrixPProvided = false;
                    bool covMatrixDProvided = false;

                    if (XMLString::equals(methodSimulatedAnnealingAP, methodStr))
					{
						XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
						if(fastOptimiserElement->hasAttribute(covMatrixPXML))
						{
							char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(covMatrixPXML));
							string covMatrixPFile = string(charValue);
							covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
							XMLString::release(&charValue);
							cout << "\tRead Cm from file" << endl;
                            covMatrixPProvided = true;
							if(covMatrixP->size1 != 2 or covMatrixP->size2 != 2)
							{
								throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
							}

						}
                        XMLString::release(&covMatrixPXML);

                        XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
						if(fastOptimiserElement->hasAttribute(invCovMatrixDXML))
						{
							char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(invCovMatrixDXML));
							string invCovMatrixDFile = string(charValue);
							invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
							XMLString::release(&charValue);
							cout << "\tRead 1 / Cd from file" << endl;
                            covMatrixDProvided = true;
							if(invCovMatrixD->size1 != 3 or invCovMatrixD->size2 != 3)
							{
								throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
							}
						}
						XMLString::release(&invCovMatrixDXML);

                    }

                    if (!covMatrixPProvided) // If not provided, or not being used
                    {
                        // Set to default values if no file (don't print warning)
                        covMatrixP = gsl_matrix_alloc(2,2);
                        double pCov1 = 1e10;
                        double pCov2 = 1e10;
                        gsl_matrix_set_zero(covMatrixP);
                        gsl_matrix_set(covMatrixP, 0, 0, pCov1);
                        gsl_matrix_set(covMatrixP, 1, 1, pCov2);

                    }

                    if(!covMatrixDProvided) // If not provided, or not being used
                    {
                        // Set to default values if no file (don't print warning)
                        invCovMatrixD = gsl_matrix_alloc(3, 3);
                        double dCovInv1 = 1;
                        double dCovInv2 = 1;
                        double dCovInv3 = 1;
                        gsl_matrix_set_zero(invCovMatrixD);
                        gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
                        gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
                        gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

                    }

                    vector <int> *polyOrdersHH = new vector <int>();
                    polyOrdersHH->push_back(coeffHH->size1);
                    polyOrdersHH->push_back(coeffHH->size2 - 1);

                    vector <int> *polyOrdersHV = new vector <int>();
                    polyOrdersHV->push_back(coeffHV->size1);
                    polyOrdersHV->push_back(coeffHV->size2 - 1);

                    vector <int> *polyOrdersVV = new vector <int>();
                    polyOrdersVV->push_back(coeffVV->size1);
                    polyOrdersVV->push_back(coeffVV->size2 - 1);

                    vector <RSGISMathNVariableFunction*> *functionsAll = new vector <RSGISMathNVariableFunction*>;
                    functionsAll->push_back(new RSGISFunctionNDPoly(coeffHH, polyOrdersHH));
                    functionsAll->push_back(new RSGISFunctionNDPoly(coeffHV, polyOrdersHV));
                    functionsAll->push_back(new RSGISFunctionNDPoly(coeffVV, polyOrdersVV));

                    this->estFastOptimiserClass->push_back(new RSGISEstimationSimulatedAnnealingWithAP(functionsAll, minMaxStepAll, minEnergy, startTemp, runsStep, runsTemp, cooling, ittmax, covMatrixP, invCovMatrixD, this->initialPar));

				}
				else if(XMLString::equals(methodAssignAP, methodStr))
				{
					cout << "\tUsing Values from objects (assign)" << endl;

					this->estFastOptimiserClass->push_back(new RSGISEstimationAssignAP());
				}
				else
				{
				    throw RSGISXMLArgumentsException("Optimiser not provieded / not recognised");
				}
			}

			XMLString::release(&methodConjugateGradient);
			XMLString::release(&methodConjugateGradientWithRestarts);
			XMLString::release(&methodExhaustiveSearch);
			XMLString::release(&methodExhaustiveSearchAP);
			XMLString::release(&methodSimulatedAnnealing);
			XMLString::release(&methodSimulatedAnnealingAP);
			XMLString::release(&methodAssignAP);
            XMLString::release(&methodNone);

		}
		else if(XMLString::equals(typeDielectricDensityHeight,parametersStr))
		{
            cout << "\tParameters to be retrieved are height, density and dielectric" << endl;
			this->parameters = dielectricDensityHeight;
			this->nPar = 3;
			this->nBands = 5; // nPar + 2

			// Get fields to use for AP
			if (XMLString::equals(typeFullPolObjectObjAP,optionStr))
			{
				this->objAP = true;
				this->objAPParField = new string[3];
				XMLCh *heightFieldXMLStr = XMLString::transcode("heightField");
				if(argElement->hasAttribute(heightFieldXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(heightFieldXMLStr));
					this->objAPParField[0] = string(charValue);
					XMLString::release(&charValue);
					cout << "\tUsing field: \"" << this->objAPParField[0] << "\" to provide a priori estimates of heights" << endl;
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'heightField\' attribute was provided. The field name for a priori values of height must be provided");
				}
				XMLString::release(&heightFieldXMLStr);

				XMLCh *densityFieldXMLStr = XMLString::transcode("densityField");
				if(argElement->hasAttribute(densityFieldXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(densityFieldXMLStr));
					this->objAPParField[1] = string(charValue);
					XMLString::release(&charValue);
					cout << "\tUsing field: \"" << this->objAPParField[1] << "\" to provide a priori estimates of densities" << endl;
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'densityField\' attribute was provided. The field name for a priori values of density must be provided");
				}
				XMLString::release(&densityFieldXMLStr);

				XMLCh *dielectricFieldXMLStr = XMLString::transcode("dielectricField");
				if(argElement->hasAttribute(dielectricFieldXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(dielectricFieldXMLStr));
					this->objAPParField[2] = string(charValue);
					XMLString::release(&charValue);
					cout << "\tUsing field: \"" << this->objAPParField[2] << "\" to provide a priori estimates of dielectric" << endl;
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'dielectricField\' attribute was provided. The field name for a priori values of dielectric must be provided");
				}
				XMLString::release(&dielectricFieldXMLStr);
			}

			this->initialParClass= new vector <gsl_vector*>;
			this->estFastOptimiserClass = new vector <RSGISEstimationOptimiser*>;
			this->estSlowOptimiserClass = new vector <RSGISEstimationOptimiser*>;


			/**************************************
			 * Get method to use for optimisation *
			 **************************************/
			XMLCh *methodConjugateGradient = XMLString::transcode("conjugateGradient");
			XMLCh *methodConjugateGradientWithRestarts = XMLString::transcode("conjugateGradientWithRestarts");
			XMLCh *methodSimulatedAnnealing = XMLString::transcode("simulatedAnnealing");
			XMLCh *methodSimulatedAnnealingAP = XMLString::transcode("simulatedAnnealingAP");
			XMLCh *methodAssignAP = XMLString::transcode("assignAP");
            XMLCh *methodNone= XMLString::transcode("none");

            DOMNodeList *slowOptimiserNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:estSlowOptimiserParameters"));
			DOMNodeList *fastOptimiserNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:estFastOptimiserParameters"));
			DOMElement *slowOptimiserElement = NULL;
			DOMElement *fastOptimiserElement = NULL;

			// Set up array to hold min / max values, these are read in with fast optimisers
			this->minMaxValuesClass = new double**[fastOptimiserNodesList->getLength()];

			// Loop through slow optimisers
			for(unsigned int i = 0; i < slowOptimiserNodesList->getLength(); i++)
			{
                slowOptimiserElement = static_cast<DOMElement*>(slowOptimiserNodesList->item(i));
                const XMLCh *methodStr = slowOptimiserElement->getAttribute(XMLString::transcode("method"));

                // Set initial parameters
				double initialHeight = 0;
				double initialDensity = 0;
				double initialDielectric = 0;

                XMLCh *initialDielectricStr = XMLString::transcode("initialDielectric");
                if(slowOptimiserElement->hasAttribute(initialDielectricStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(initialDielectricStr));
					initialDielectric = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No initial dielectric provided");
                }
                XMLString::release(&initialDielectricStr);
                XMLCh *initialDensityStr = XMLString::transcode("initialDensity");
                if(slowOptimiserElement->hasAttribute(initialDensityStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(initialDensityStr));
					initialDensity = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No initial density provided");
                }
                XMLString::release(&initialDensityStr);
                XMLCh *initialHeightStr = XMLString::transcode("initialHeight");
                if(slowOptimiserElement->hasAttribute(initialHeightStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(initialHeightStr));
					initialHeight = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No initial height provided");
                }
                XMLString::release(&initialHeightStr);

				// Save parameters to vector
				this->initialParClass->push_back(gsl_vector_alloc(this->nPar));
				gsl_vector_set(this->initialParClass->at(i),0,initialHeight);
				gsl_vector_set(this->initialParClass->at(i),1,initialDensity);
				gsl_vector_set(this->initialParClass->at(i),2,initialDielectric);

                // Get coefficients
                const XMLCh *functionStr = slowOptimiserElement->getAttribute(XMLString::transcode("function"));
                XMLCh *function3DPoly = XMLString::transcode("3DPoly");
                if (XMLString::equals(function3DPoly,functionStr))
                {
                    // Read coefficients (if using optimiser at the object level
                    if(!XMLString::equals(methodNone, methodStr))
                    {
                        XMLCh *coefficientsHH = XMLString::transcode("coefficientsHH");
                        if(slowOptimiserElement->hasAttribute(coefficientsHH))
                        {
                            char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(coefficientsHH));
                            string coeffHHFile = string(charValue);
                            this->coeffHH = matrixUtils.readGSLMatrixFromTxt(coeffHHFile);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISXMLArgumentsException("No coefficents provided for HH");
                        }
                        XMLString::release(&coefficientsHH);
                        XMLCh *coefficientsHV = XMLString::transcode("coefficientsHV");
                        if(slowOptimiserElement->hasAttribute(coefficientsHV))
                        {
                            char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(coefficientsHV));
                            string coeffHVFile = string(charValue);
                            this->coeffHV = matrixUtils.readGSLMatrixFromTxt(coeffHVFile);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISXMLArgumentsException("No coefficents provided for HV");
                        }
                        XMLString::release(&coefficientsHV);

                        XMLCh *inCoeffVV = XMLString::transcode("coefficientsVV"); // Coefficients
                        if(slowOptimiserElement->hasAttribute(inCoeffVV))
                        {
                            char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(inCoeffVV));
                            string inCoeffVVFileName = string(charValue);
                            this->coeffVV = matrixUtils.readGSLMatrixFromTxt(inCoeffVVFileName);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISXMLArgumentsException("No Coefficients not Provided for VV");
                        }
                        XMLString::release(&inCoeffVV);

                        bool sameOrderBothFits = false;
                        XMLCh *polyOrderStr = XMLString::transcode("polyOrder"); // Polynomial Order
                        if(slowOptimiserElement->hasAttribute(polyOrderStr))
                        {
                            char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(polyOrderStr));
                            int orderInt = mathUtils.strtoint(string(charValue));
                            cout << "\tusing same order polynomial for both sets of fits, use \'polyOrderX\', \'polyOrderY\' and \'polyOrderZ\' to set seperately" << endl;
                            sameOrderBothFits = true;
                            this->polyOrderX = orderInt + 1;
                            this->polyOrderY = orderInt + 1;
                            this->polyOrderZ = orderInt + 1;
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&polyOrderStr);
                        XMLCh *polyOrderXStr = XMLString::transcode("polyOrderX"); // Polynomial Order
                        if(slowOptimiserElement->hasAttribute(polyOrderXStr))
                        {
                            char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(polyOrderXStr));
                            int orderInt = mathUtils.strtoint(string(charValue));
                            this->polyOrderX = orderInt + 1;
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            if (!sameOrderBothFits)
                            {
                                throw RSGISXMLArgumentsException("No value provided for polynomial order for x fits");
                            }
                        }
                        XMLString::release(&polyOrderXStr);
                        XMLCh *polyOrderYStr = XMLString::transcode("polyOrderY"); // Polynomial Order
                        if(slowOptimiserElement->hasAttribute(polyOrderYStr))
                        {
                            char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(polyOrderYStr));
                            int orderInt = mathUtils.strtoint(string(charValue));
                            this->polyOrderY = orderInt + 1;
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            if (!sameOrderBothFits)
                            {
                                throw RSGISXMLArgumentsException("No value provided for polynomial order for y fits");
                            }
                        }
                        XMLString::release(&polyOrderYStr);
                        XMLCh *polyOrderZStr = XMLString::transcode("polyOrderZ"); // Polynomial Order
                        if(slowOptimiserElement->hasAttribute(polyOrderZStr))
                        {
                            char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(polyOrderZStr));
                            int orderInt = mathUtils.strtoint(string(charValue));
                            this->polyOrderZ = orderInt + 1;
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            if (!sameOrderBothFits)
                            {
                                throw RSGISXMLArgumentsException("No value provided for polynomial order for z fits");
                            }
                        }
                        XMLString::release(&polyOrderZStr);
                    }
                }
                else
                {
                    if(!XMLString::equals(methodNone, methodStr))
					{
						throw RSGISXMLArgumentsException("Function type not provieded / not recognised");
					}
                }
                XMLString::release(&function3DPoly);

                /* Get minimum and maximum heights and density.
                 * Setting these is optional and default values will be chosen with no message if
                 * values are not set
                 */

                double **minMaxStepAll = new double*[3];
                minMaxStepAll[0] = new double[3];
                minMaxStepAll[1] = new double[3];
                minMaxStepAll[2] = new double[3];

                XMLCh *minHeightStr = XMLString::transcode("minHeight");
                if(slowOptimiserElement->hasAttribute(minHeightStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minHeightStr));
                    minMaxStepAll[0][0] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[0][0] = 1;
                }
                XMLString::release(&minHeightStr);

                XMLCh *maxHeightStr = XMLString::transcode("maxHeight");
                if(slowOptimiserElement->hasAttribute(maxHeightStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxHeightStr));
                    minMaxStepAll[0][1] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[0][1] = 20;
                }
                XMLString::release(&maxHeightStr);

                XMLCh *heightStepStr = XMLString::transcode("heightStep");
                if(slowOptimiserElement->hasAttribute(heightStepStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(heightStepStr));
                    minMaxStepAll[0][2] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[0][2] = 0.01;
                }
                XMLString::release(&heightStepStr);

                XMLCh *minDensityStr = XMLString::transcode("minDensity");
                if(slowOptimiserElement->hasAttribute(minDensityStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minDensityStr));
                    minMaxStepAll[1][0] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[1][0] = 0.1;
                }
                XMLString::release(&minDensityStr);

                XMLCh *maxDensityStr = XMLString::transcode("maxDensity");
                if(slowOptimiserElement->hasAttribute(maxDensityStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxDensityStr));
                    minMaxStepAll[1][1] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[1][1] = 2;
                }
                XMLString::release(&maxDensityStr);

                XMLCh *densityStepStr = XMLString::transcode("densityStep");
                if(slowOptimiserElement->hasAttribute(densityStepStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(densityStepStr));
                    minMaxStepAll[1][2] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[1][2] = 0.001;
                }
                XMLString::release(&densityStepStr);

                XMLCh *minDielectricStr = XMLString::transcode("minDielectric");
                if(slowOptimiserElement->hasAttribute(minDielectricStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(minDielectricStr));
                    minMaxStepAll[2][0] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[2][0] = 10;
                }
                XMLString::release(&minDielectricStr);

                XMLCh *maxDielectricStr = XMLString::transcode("maxDielectric");
                if(slowOptimiserElement->hasAttribute(maxDielectricStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(maxDielectricStr));
                    minMaxStepAll[2][1] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[2][1] = 60;
                }
                XMLString::release(&maxDielectricStr);

                XMLCh *dielectricStepStr = XMLString::transcode("dielectricStep");
                if(slowOptimiserElement->hasAttribute(dielectricStepStr))
                {
                    char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(dielectricStepStr));
                    minMaxStepAll[2][2] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[2][2] = 0.5;
                }
                XMLString::release(&dielectricStepStr);

                if(XMLString::equals(methodConjugateGradientWithRestarts, methodStr) | XMLString::equals(methodConjugateGradient, methodStr) | XMLString::equals(methodSimulatedAnnealing, methodStr) | XMLString::equals(methodSimulatedAnnealingAP, methodStr))
                {
                    if (XMLString::equals(methodConjugateGradientWithRestarts, methodStr)) {cout << "\tUsing ConjugateGradient - with restarts" << endl;}
                    else if (XMLString::equals(methodConjugateGradient, methodStr)){cout << "\tUsing ConjugateGradient" << endl;}
                    else {cout << "\tUsing Simulated Annealing" << endl;}

                    // Maximum number of itterations
                    XMLCh *ittmaxStr = XMLString::transcode("ittmax");
                    if(slowOptimiserElement->hasAttribute(ittmaxStr))
                    {
                        char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(ittmaxStr));
                        this->ittmax = mathUtils.strtoint(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        if(XMLString::equals(methodConjugateGradientWithRestarts, methodStr) | XMLString::equals(methodConjugateGradient, methodStr)){this->ittmax = 100;}
                        else{this->ittmax = 10000;}
                    }
                    XMLString::release(&ittmaxStr);

                    // Number or Restarts
                    int numRestarts = 5;
                    XMLCh *numRestartsStr = XMLString::transcode("numRestarts");
                    if(slowOptimiserElement->hasAttribute(numRestartsStr))
                    {
                        char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(numRestartsStr));
                        numRestarts = mathUtils.strtoint(string(charValue));
                        XMLString::release(&charValue);
                    }
                    XMLString::release(&numRestartsStr);

                    // Minimum error
                    double minError = 1e-12;
                    XMLCh *targetErrorStr = XMLString::transcode("targetError");
                    if(slowOptimiserElement->hasAttribute(targetErrorStr))
                    {
                        char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(targetErrorStr));
                        minError = mathUtils.strtodouble(string(charValue));
                        XMLString::release(&charValue);
                    }
                    XMLString::release(&targetErrorStr);

                    /******************************
                     * Set up covariance matrices *
                     ******************************/
                    gsl_matrix *covMatrixP;
                    gsl_matrix *invCovMatrixD;

                    XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
                    if(slowOptimiserElement->hasAttribute(covMatrixPXML))
                    {
                        char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(covMatrixPXML));
                        string covMatrixPFile = string(charValue);
                        covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
                        XMLString::release(&charValue);
                        cout << "\tRead Cm from file" << endl;
                        if(covMatrixP->size1 != 3 or covMatrixP->size2 != 3)
                        {
                            throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
                        }
                    }
                    else
                    {
                        // Set to default values if no file (don't print warning)
                        covMatrixP = gsl_matrix_alloc(3,3);
						double pCov1 = pow(gsl_vector_get(this->initialParClass->at(i),0)*0.5,2)*1e6;
						double pCov2 = pow(gsl_vector_get(this->initialParClass->at(i),1)*0.5,2)*1e6*
                            gsl_vector_get(this->initialParClass->at(i),0)/gsl_vector_get(this->initialParClass->at(i),1);
						double pCov3 = pow(gsl_vector_get(this->initialParClass->at(i),2)*0.5,2)*1e6*
                            gsl_vector_get(this->initialParClass->at(i),0)/gsl_vector_get(this->initialParClass->at(i),2);
                        // If simulated annealing set all values to 10e10 (not used)
                        if (XMLString::equals(methodSimulatedAnnealing, methodStr))
                        {
                            pCov1 = 1e10;
                            pCov2 = 1e10;
                            pCov3 = 1e10;
                        }
                        gsl_matrix_set_zero(covMatrixP);
                        gsl_matrix_set(covMatrixP, 0, 0, pCov1);
                        gsl_matrix_set(covMatrixP, 1, 1, pCov2);
                        gsl_matrix_set(covMatrixP, 2, 2, pCov3);

                    }
                    XMLString::release(&covMatrixPXML);

                    XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
                    if(slowOptimiserElement->hasAttribute(invCovMatrixDXML))
                    {
                        char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(invCovMatrixDXML));
                        string invCovMatrixDFile = string(charValue);
                        invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
                        XMLString::release(&charValue);
                        cout << "\tRead 1 / Cd from file" << endl;
                        if(invCovMatrixD->size1 != 3 or invCovMatrixD->size2 != 3)
                        {
                            throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
                        }
                    }
                    else
                    {
                        // Set to default values if no file (don't print warning)
                        invCovMatrixD = gsl_matrix_alloc(3, 3);
                        double dCovInv1 = 1;
                        double dCovInv2 = 1;
                        double dCovInv3 = 1;
                        gsl_matrix_set_zero(invCovMatrixD);
                        gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
                        gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
                        gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

                    }
                    XMLString::release(&invCovMatrixDXML);

                    if (XMLString::equals(methodConjugateGradientWithRestarts, methodStr))
					{
						this->estSlowOptimiserClass->push_back(new RSGISEstimationConjugateGradient3Var3DataWithRestarts(this->coeffHH, this->coeffHV, this->coeffVV, this->polyOrderX, this->polyOrderY, this->polyOrderZ, minMaxStepAll[0], minMaxStepAll[1], minMaxStepAll[2], this->initialParClass->at(0), covMatrixP, invCovMatrixD, minError, this->ittmax, numRestarts));
					}
					else if(XMLString::equals(methodConjugateGradient, methodStr))
					{
						this->estSlowOptimiserClass->push_back(new RSGISEstimationConjugateGradient3DPoly3Channel(this->coeffHH, this->coeffHV, this->coeffVV, this->polyOrderX, this->polyOrderY, this->polyOrderZ, this->initialParClass->at(0), covMatrixP, invCovMatrixD, minError, this->ittmax));
					}
                    else
                    {
                        /* Get other parameters for simulated annealing.
                         * Setting these is optional and default values will be chosen with no message if
                         * values are not set
                         */

                        double startTemp = 1000;
                        unsigned int runsStep = 20; // Number of runs at each step size
                        unsigned int runsTemp = 100; // Number of times step is changed at each temperature
                        double cooling = 0.85; // Cooling factor
                        double minEnergy = 1e-12; // Set the target energy


                        // Number of runs at each step size
                        XMLCh *runsStepStr = XMLString::transcode("runsStep");
                        if(slowOptimiserElement->hasAttribute(runsStepStr))
                        {
                            char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(runsStepStr));
                            runsStep = mathUtils.strtoint(string(charValue));
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&runsStepStr);

                        // Number of times step is changed at each temperature
                        XMLCh *runsTempStr = XMLString::transcode("runsTemp");
                        if(slowOptimiserElement->hasAttribute(runsTempStr))
                        {
                            char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(runsTempStr));
                            runsTemp = mathUtils.strtoint(string(charValue));
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&runsTempStr);

                        // Cooling factor
                        XMLCh *coolingStr = XMLString::transcode("cooling");
                        if(slowOptimiserElement->hasAttribute(coolingStr))
                        {
                            char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(coolingStr));
                            cooling = mathUtils.strtodouble(string(charValue));
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&coolingStr);

                        // Minimum energy
                        XMLCh *targetErrorStr = XMLString::transcode("targetError");
                        if(slowOptimiserElement->hasAttribute(targetErrorStr))
                        {
                            char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(targetErrorStr));
                            minEnergy = mathUtils.strtodouble(string(charValue));
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&targetErrorStr);

                        // Start Temp
                        XMLCh *startTempStr = XMLString::transcode("startTemp");
                        if(slowOptimiserElement->hasAttribute(startTempStr))
                        {
                            char *charValue = XMLString::transcode(slowOptimiserElement->getAttribute(startTempStr));
                            startTemp = mathUtils.strtodouble(string(charValue));
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&startTempStr);

                        vector <int> *polyOrders = new vector <int>();
                        polyOrders->push_back(this->polyOrderX);
                        polyOrders->push_back(this->polyOrderY);
                        polyOrders->push_back(this->polyOrderZ);

                        vector <RSGISMathNVariableFunction*> *functionsAll = new vector <RSGISMathNVariableFunction*>;
                        functionsAll->push_back(new RSGISFunctionNDPoly(coeffHH, polyOrders));
                        functionsAll->push_back(new RSGISFunctionNDPoly(coeffHV, polyOrders));
                        functionsAll->push_back(new RSGISFunctionNDPoly(coeffVV, polyOrders));

                        this->estSlowOptimiserClass->push_back(new RSGISEstimationSimulatedAnnealingWithAP(functionsAll, minMaxStepAll, minEnergy, startTemp, runsStep, runsTemp, cooling, ittmax, covMatrixP, invCovMatrixD, this->initialParClass->at(0)));


                    }


                }
                else if(XMLString::equals(methodNone,methodNone))
                {
                    cout << "\tUsing No optimiser (pixel based estimation)." << endl;
                    this->estSlowOptimiserClass->push_back(new rsgis::radar::RSGISEstimationNoEstimation());
                }
				else if(XMLString::equals(methodAssignAP, methodStr))
				{
					throw RSGISXMLArgumentsException("The optimiser \"assignAP\" cannot be used at the object level");
				}
                else
                {
                    throw RSGISXMLArgumentsException("Method for optimisation not recognised.");
                }

			}

			// Loop through fast optimisers
			for(unsigned int i = 0; i < fastOptimiserNodesList->getLength(); i++)
			{

                fastOptimiserElement = static_cast<DOMElement*>(fastOptimiserNodesList->item(i));
                const XMLCh *methodStr = fastOptimiserElement->getAttribute(XMLString::transcode("method"));

                // Get coefficients
                if(!XMLString::equals(methodAssignAP, methodStr))
                {
                    const XMLCh *functionStr = fastOptimiserElement->getAttribute(XMLString::transcode("function"));
                    XMLCh *function3DPoly = XMLString::transcode("3DPoly");
                    if (XMLString::equals(function3DPoly,functionStr))
                    {
                        // Read coefficients
                        XMLCh *coefficientsHH = XMLString::transcode("coefficientsHH");
                        if(fastOptimiserElement->hasAttribute(coefficientsHH))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(coefficientsHH));
                            string coeffHHFile = string(charValue);
                            this->coeffHH = matrixUtils.readGSLMatrixFromTxt(coeffHHFile);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISXMLArgumentsException("No coefficents provided for HH");
                        }
                        XMLString::release(&coefficientsHH);
                        XMLCh *coefficientsHV = XMLString::transcode("coefficientsHV");
                        if(fastOptimiserElement->hasAttribute(coefficientsHV))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(coefficientsHV));
                            string coeffHVFile = string(charValue);
                            this->coeffHV = matrixUtils.readGSLMatrixFromTxt(coeffHVFile);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISXMLArgumentsException("No coefficents provided for HV");
                        }
                        XMLString::release(&coefficientsHV);

                        XMLCh *inCoeffVV = XMLString::transcode("coefficientsVV"); // Coefficients
                        if(fastOptimiserElement->hasAttribute(inCoeffVV))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(inCoeffVV));
                            string inCoeffVVFileName = string(charValue);
                            this->coeffVV = matrixUtils.readGSLMatrixFromTxt(inCoeffVVFileName);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISXMLArgumentsException("No Coefficients not Provided for VV");
                        }
                        XMLString::release(&inCoeffVV);

                        bool sameOrderBothFits = false;
                        XMLCh *polyOrderStr = XMLString::transcode("polyOrder"); // Polynomial Order
                        if(fastOptimiserElement->hasAttribute(polyOrderStr))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(polyOrderStr));
                            int orderInt = mathUtils.strtoint(string(charValue));
                            cout << "\tusing same order polynomial for both sets of fits, use \'polyOrderX\', \'polyOrderY\' and \'polyOrderZ\' to set seperately" << endl;
                            sameOrderBothFits = true;
                            this->polyOrderX = orderInt + 1;
                            this->polyOrderY = orderInt + 1;
                            this->polyOrderZ = orderInt + 1;
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&polyOrderStr);
                        XMLCh *polyOrderXStr = XMLString::transcode("polyOrderX"); // Polynomial Order
                        if(fastOptimiserElement->hasAttribute(polyOrderXStr))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(polyOrderXStr));
                            int orderInt = mathUtils.strtoint(string(charValue));
                            this->polyOrderX = orderInt + 1;
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            if (!sameOrderBothFits)
                            {
                                throw RSGISXMLArgumentsException("No value provided for polynomial order for x fits");
                            }
                        }
                        XMLString::release(&polyOrderXStr);
                        XMLCh *polyOrderYStr = XMLString::transcode("polyOrderY"); // Polynomial Order
                        if(fastOptimiserElement->hasAttribute(polyOrderYStr))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(polyOrderYStr));
                            int orderInt = mathUtils.strtoint(string(charValue));
                            this->polyOrderY = orderInt + 1;
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            if (!sameOrderBothFits)
                            {
                                throw RSGISXMLArgumentsException("No value provided for polynomial order for y fits");
                            }
                        }
                        XMLString::release(&polyOrderYStr);
                        XMLCh *polyOrderZStr = XMLString::transcode("polyOrderZ"); // Polynomial Order
                        if(fastOptimiserElement->hasAttribute(polyOrderZStr))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(polyOrderZStr));
                            int orderInt = mathUtils.strtoint(string(charValue));
                            this->polyOrderZ = orderInt + 1;
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            if (!sameOrderBothFits)
                            {
                                throw RSGISXMLArgumentsException("No value provided for polynomial order for z fits");
                            }
                        }
                        XMLString::release(&polyOrderZStr);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("Function type not provieded / not recognised");
                    }
                    XMLString::release(&function3DPoly);
                }


                /* Get minimum and maximum heights and density.
                 * Setting these is optional and default values will be chosen with no message if
                 * values are not set
                 */

                double **minMaxStepAll = new double*[3];
                minMaxStepAll[0] = new double[3];
                minMaxStepAll[1] = new double[3];
                minMaxStepAll[2] = new double[3];

                XMLCh *minHeightStr = XMLString::transcode("minHeight");
                if(fastOptimiserElement->hasAttribute(minHeightStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minHeightStr));
                    minMaxStepAll[0][0] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[0][0] = 1;
                }
                XMLString::release(&minHeightStr);

                XMLCh *maxHeightStr = XMLString::transcode("maxHeight");
                if(fastOptimiserElement->hasAttribute(maxHeightStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxHeightStr));
                    minMaxStepAll[0][1] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[0][1] = 20;
                }
                XMLString::release(&maxHeightStr);

                XMLCh *heightStepStr = XMLString::transcode("heightStep");
                if(fastOptimiserElement->hasAttribute(heightStepStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(heightStepStr));
                    minMaxStepAll[0][2] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[0][2] = 0.01;
                }
                XMLString::release(&heightStepStr);

                XMLCh *minDensityStr = XMLString::transcode("minDensity");
                if(fastOptimiserElement->hasAttribute(minDensityStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minDensityStr));
                    minMaxStepAll[1][0] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[1][0] = 0.1;
                }
                XMLString::release(&minDensityStr);

                XMLCh *maxDensityStr = XMLString::transcode("maxDensity");
                if(fastOptimiserElement->hasAttribute(maxDensityStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxDensityStr));
                    minMaxStepAll[1][1] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[1][1] = 2;
                }
                XMLString::release(&maxDensityStr);

                XMLCh *densityStepStr = XMLString::transcode("densityStep");
                if(fastOptimiserElement->hasAttribute(densityStepStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(densityStepStr));
                    minMaxStepAll[1][2] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[1][2] = 0.001;
                }
                XMLString::release(&densityStepStr);

                XMLCh *minDielectricStr = XMLString::transcode("minDielectric");
                if(fastOptimiserElement->hasAttribute(minDielectricStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(minDielectricStr));
                    minMaxStepAll[2][0] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[2][0] = 1;
                }
                XMLString::release(&minDielectricStr);

                XMLCh *maxDielectricStr = XMLString::transcode("maxDielectric");
                if(fastOptimiserElement->hasAttribute(maxDielectricStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(maxDielectricStr));
                    minMaxStepAll[2][1] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[2][1] = 60;
                }
                XMLString::release(&maxDielectricStr);

                XMLCh *dielectricStepStr = XMLString::transcode("dielectricStep");
                if(fastOptimiserElement->hasAttribute(dielectricStepStr))
                {
                    char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(dielectricStepStr));
                    minMaxStepAll[2][2] = mathUtils.strtodouble(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    minMaxStepAll[2][2] = 0.5;
                }
                XMLString::release(&dielectricStepStr);

                // Set minimum and maximum values
                this->minMaxValuesClass[i] = new double*[3];
                this->minMaxValuesClass[i][0] = new double[2];
                this->minMaxValuesClass[i][1] = new double[2];
                this->minMaxValuesClass[i][2] = new double[2];

                this->minMaxValuesClass[i][0][0] = minMaxStepAll[0][0];
                this->minMaxValuesClass[i][0][1] = minMaxStepAll[0][1];
                this->minMaxValuesClass[i][1][0] = minMaxStepAll[1][0];
                this->minMaxValuesClass[i][1][1] = minMaxStepAll[1][1];
                this->minMaxValuesClass[i][2][0] = minMaxStepAll[2][0];
                this->minMaxValuesClass[i][2][1] = minMaxStepAll[2][1];

                /**************************************
                 * Get method to use for optimisation *
                 **************************************/

                if(XMLString::equals(methodConjugateGradientWithRestarts, methodStr) | XMLString::equals(methodConjugateGradient, methodStr) | XMLString::equals(methodSimulatedAnnealing, methodStr) | XMLString::equals(methodSimulatedAnnealingAP, methodStr))
                {
                    if (XMLString::equals(methodConjugateGradientWithRestarts, methodStr)) {cout << "\tUsing ConjugateGradient - with restarts" << endl;}
                    else if (XMLString::equals(methodConjugateGradient, methodStr)){cout << "\tUsing ConjugateGradient" << endl;}
                    else {cout << "\tUsing Simulated Annealing" << endl;}

                    // Maximum number of itterations
                    XMLCh *ittmaxStr = XMLString::transcode("ittmax");
                    if(fastOptimiserElement->hasAttribute(ittmaxStr))
                    {
                        char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(ittmaxStr));
                        this->ittmax = mathUtils.strtoint(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {

                    }
                    XMLString::release(&ittmaxStr);

                    // Number or Restarts
                    int numRestarts = 5;
                    XMLCh *numRestartsStr = XMLString::transcode("numRestarts");
                    if(fastOptimiserElement->hasAttribute(numRestartsStr))
                    {
                        char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(numRestartsStr));
                        numRestarts = mathUtils.strtoint(string(charValue));
                        XMLString::release(&charValue);
                    }
                    XMLString::release(&numRestartsStr);

                    // Minimum error
                    double minError = 1e-12;
                    XMLCh *targetErrorStr = XMLString::transcode("targetError");
                    if(fastOptimiserElement->hasAttribute(targetErrorStr))
                    {
                        char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(targetErrorStr));
                        minError = mathUtils.strtodouble(string(charValue));
                        XMLString::release(&charValue);
                    }
                    XMLString::release(&targetErrorStr);


                    /******************************
                     * Set up covariance matrices *
                     ******************************/
                    gsl_matrix *covMatrixP;
                    gsl_matrix *invCovMatrixD;

                    XMLCh *covMatrixPXML = XMLString::transcode("covMatrixP");
                    if(fastOptimiserElement->hasAttribute(covMatrixPXML))
                    {
                        char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(covMatrixPXML));
                        string covMatrixPFile = string(charValue);
                        covMatrixP = matrixUtils.readGSLMatrixFromTxt(covMatrixPFile);
                        XMLString::release(&charValue);
                        cout << "\tRead Cm from file" << endl;
                        if(covMatrixP->size1 != 3 or covMatrixP->size2 != 3)
                        {
                            throw RSGISXMLArgumentsException("A priori covariance matrix is not of correct dimensions\n");
                        }
                    }
                    else
                    {
                        // Set to default values if no file (don't print warning)
                        covMatrixP = gsl_matrix_alloc(3,3);

						double pCov1 = pow(gsl_vector_get(this->initialParClass->at(i),0)*0.5,2)*1e6;
						double pCov2 = pow(gsl_vector_get(this->initialParClass->at(i),1)*0.5,2)*1e6*
                            gsl_vector_get(this->initialParClass->at(i),0)/gsl_vector_get(this->initialParClass->at(i),1);
						double pCov3 = pow(gsl_vector_get(this->initialParClass->at(i),2)*0.5,2)*1e6*
                            gsl_vector_get(this->initialParClass->at(i),0)/gsl_vector_get(this->initialParClass->at(i),2);
                        // If simulated annealing set all values to 10e10 (not used)
                        if (XMLString::equals(methodSimulatedAnnealing, methodStr))
                        {
                            pCov1 = 1e10;
                            pCov2 = 1e10;
                            pCov3 = 1e10;
                        }

                        gsl_matrix_set_zero(covMatrixP);
                        gsl_matrix_set(covMatrixP, 0, 0, pCov1);
                        gsl_matrix_set(covMatrixP, 1, 1, pCov2);
                        gsl_matrix_set(covMatrixP, 2, 2, pCov3);

                    }
                    XMLString::release(&covMatrixPXML);

                    XMLCh *invCovMatrixDXML = XMLString::transcode("invCovMatrixD"); // Set the inverse covarience matrix for the data (only use inverse matrix)
                    if(fastOptimiserElement->hasAttribute(invCovMatrixDXML))
                    {
                        char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(invCovMatrixDXML));
                        string invCovMatrixDFile = string(charValue);
                        invCovMatrixD = matrixUtils.readGSLMatrixFromTxt(invCovMatrixDFile);
                        XMLString::release(&charValue);
                        cout << "\tRead 1 / Cd from file" << endl;
                        if(invCovMatrixD->size1 != 3 or invCovMatrixD->size2 != 3)
                        {
                            throw RSGISXMLArgumentsException("Data covariance matrix is not of correct dimensions\n");
                        }
                    }
                    else
                    {
                        // Set to default values if no file (don't print warning)
                        invCovMatrixD = gsl_matrix_alloc(3, 3);
                        double dCovInv1 = 1;
                        double dCovInv2 = 1;
                        double dCovInv3 = 1;
                        gsl_matrix_set_zero(invCovMatrixD);
                        gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
                        gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
                        gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

                    }
                    XMLString::release(&invCovMatrixDXML);

                    if (XMLString::equals(methodConjugateGradientWithRestarts, methodStr))
					{
						this->estFastOptimiserClass->push_back(new RSGISEstimationConjugateGradient3Var3DataWithRestarts(this->coeffHH, this->coeffHV, this->coeffVV, this->polyOrderX, this->polyOrderY, this->polyOrderZ, minMaxStepAll[0], minMaxStepAll[1], minMaxStepAll[2], this->initialPar, covMatrixP, invCovMatrixD, minError, this->ittmax, numRestarts));
					}
					else if(XMLString::equals(methodConjugateGradient, methodStr))
					{
						this->estFastOptimiserClass->push_back(new RSGISEstimationConjugateGradient3DPoly3Channel(this->coeffHH, this->coeffHV, this->coeffVV, this->polyOrderX, this->polyOrderY, this->polyOrderZ, this->initialPar, covMatrixP, invCovMatrixD, minError, this->ittmax));
					}
                    else
                    {
                        /* Get other parameters for simulated annealing.
                         * Setting these is optional and default values will be chosen with no message if
                         * values are not set
                         */

                        double startTemp = 1000;
                        unsigned int runsStep = 20; // Number of runs at each step size
                        unsigned int runsTemp = 100; // Number of times step is changed at each temperature
                        double cooling = 0.85; // Cooling factor
                        double minEnergy = 1e-12; // Set the target energy
                        this->ittmax = 50;                  // Maximum number of itterations

                        XMLCh *ittmaxStr = XMLString::transcode("ittmax");
                        if(fastOptimiserElement->hasAttribute(ittmaxStr))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(ittmaxStr));
                            this->ittmax = mathUtils.strtoint(string(charValue));
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&ittmaxStr);
                        // Number of runs at each step size
                        XMLCh *runsStepStr = XMLString::transcode("runsStep");
                        if(fastOptimiserElement->hasAttribute(runsStepStr))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(runsStepStr));
                            runsStep = mathUtils.strtoint(string(charValue));
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&runsStepStr);

                        // Number of times step is changed at each temperature
                        XMLCh *runsTempStr = XMLString::transcode("runsTemp");
                        if(fastOptimiserElement->hasAttribute(runsTempStr))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(runsTempStr));
                            runsTemp = mathUtils.strtoint(string(charValue));
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&runsTempStr);

                        // Cooling factor
                        XMLCh *coolingStr = XMLString::transcode("cooling");
                        if(fastOptimiserElement->hasAttribute(coolingStr))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(coolingStr));
                            cooling = mathUtils.strtodouble(string(charValue));
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&coolingStr);

                        // Minimum energy
                        XMLCh *targetErrorStr = XMLString::transcode("targetError");
                        if(fastOptimiserElement->hasAttribute(targetErrorStr))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(targetErrorStr));
                            minEnergy = mathUtils.strtodouble(string(charValue));
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&targetErrorStr);

                        // Start Temp
                        XMLCh *startTempStr = XMLString::transcode("startTemp");
                        if(fastOptimiserElement->hasAttribute(startTempStr))
                        {
                            char *charValue = XMLString::transcode(fastOptimiserElement->getAttribute(startTempStr));
                            startTemp = mathUtils.strtodouble(string(charValue));
                            XMLString::release(&charValue);
                        }
                        XMLString::release(&startTempStr);

                        vector <int> *polyOrders = new vector <int>();
                        polyOrders->push_back(this->polyOrderX);
                        polyOrders->push_back(this->polyOrderY);
                        polyOrders->push_back(this->polyOrderZ);

                        vector <RSGISMathNVariableFunction*> *functionsAll = new vector <RSGISMathNVariableFunction*>;
                        functionsAll->push_back(new RSGISFunctionNDPoly(coeffHH, polyOrders));
                        functionsAll->push_back(new RSGISFunctionNDPoly(coeffHV, polyOrders));
                        functionsAll->push_back(new RSGISFunctionNDPoly(coeffVV, polyOrders));

                        this->estFastOptimiserClass->push_back(new RSGISEstimationSimulatedAnnealingWithAP(functionsAll, minMaxStepAll, minEnergy, startTemp, runsStep, runsTemp, cooling, this->ittmax, covMatrixP, invCovMatrixD, this->initialParClass->at(i)));

                    }

                }
                else if(XMLString::equals(methodAssignAP, methodStr))
				{
					cout << "\tUsing Values from objects (assign)" << endl;

					this->estFastOptimiserClass->push_back(new rsgis::radar::RSGISEstimationAssignAP());
				}
                else
                {
                    throw RSGISXMLArgumentsException("Method for optimisation not recognised.");
                }
			}

            XMLString::release(&methodConjugateGradient);
			XMLString::release(&methodConjugateGradientWithRestarts);
			XMLString::release(&methodSimulatedAnnealing);
			XMLString::release(&methodSimulatedAnnealingAP);
			XMLString::release(&methodAssignAP);
            XMLString::release(&methodNone);

        }
		else
		{
			throw RSGISXMLArgumentsException("Parameters to be retrieved not recognised or compatible with this option");
		}
	}
	//---- Simulated Data - 2Var2Data ---//
	else if(XMLString::equals(typeSimData2Var2Data,optionStr))
	{
		this->option = RSGISExeEstimationAlgorithm::simData2Var2Data;
		this->nBands = 2;

		// Get function to use
		XMLCh *functionLn2Var = XMLString::transcode("ln2Var");
		XMLCh *functionLn2VarQuadratic = XMLString::transcode("ln2VarQuadratic");
		XMLCh *functionLinXfLinY = XMLString::transcode("linXfLinY");
		XMLCh *function2DPoly = XMLString::transcode("2DPoly");
		const XMLCh *functionStr = argElement->getAttribute(XMLString::transcode("function"));

		if (XMLString::equals(functionLn2Var,functionStr))
		{
			gsl_vector *coeffHH;
			XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
			if(argElement->hasAttribute(inCoeffHH))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inCoeffHH));
				string inCoeffHHFileName = string(charValue);
				coeffHH = vectorUtils.readGSLVectorFromTxt(inCoeffHHFileName);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
			}
			XMLString::release(&inCoeffHH);

			gsl_vector *coeffHV;
			XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
			if(argElement->hasAttribute(inCoeffHV))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inCoeffHV));
				string inCoeffHVFileName = string(charValue);
				coeffHV = vectorUtils.readGSLVectorFromTxt(inCoeffHVFileName);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
			}
			XMLString::release(&inCoeffHV);


			RSGISFunction2VarLn *functionLnHH;
			RSGISFunction2VarLn *functionLnHV;

			functionLnHH = new RSGISFunction2VarLn(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2));
			functionLnHV = new RSGISFunction2VarLn(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2));

			this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
			this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;

			gsl_vector_free(coeffHH);
			gsl_vector_free(coeffHV);
		}
		else if (XMLString::equals(functionLn2VarQuadratic,functionStr))
		{
			gsl_vector *coeffHH;
			XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
			if(argElement->hasAttribute(inCoeffHH))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inCoeffHH));
				string inCoeffHHFileName = string(charValue);
				coeffHH = vectorUtils.readGSLVectorFromTxt(inCoeffHHFileName);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
			}
			XMLString::release(&inCoeffHH);

			gsl_vector *coeffHV;
			XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
			if(argElement->hasAttribute(inCoeffHV))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inCoeffHV));
				string inCoeffHVFileName = string(charValue);
				coeffHV = vectorUtils.readGSLVectorFromTxt(inCoeffHVFileName);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
			}
			XMLString::release(&inCoeffHV);


			RSGISFunction2VarLnQuadratic *functionLnHH;
			RSGISFunction2VarLnQuadratic *functionLnHV;

			functionLnHH = new RSGISFunction2VarLnQuadratic(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2));
			functionLnHV = new RSGISFunction2VarLnQuadratic(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2));

			this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
			this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;

			gsl_vector_free(coeffHH);
			gsl_vector_free(coeffHV);
		}
		else if (XMLString::equals(functionLinXfLinY,functionStr))
		{
			gsl_vector *coeffHH;
			XMLCh *inCoeffHH = XMLString::transcode("coefficientsHH"); // Coefficients
			if(argElement->hasAttribute(inCoeffHH))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inCoeffHH));
				string inCoeffHHFileName = string(charValue);
				coeffHH = vectorUtils.readGSLVectorFromTxt(inCoeffHHFileName);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No Coefficients not Provided for HH");
			}
			XMLString::release(&inCoeffHH);

			gsl_vector *coeffHV;
			XMLCh *inCoeffHV = XMLString::transcode("coefficientsHV"); // Coefficients
			if(argElement->hasAttribute(inCoeffHV))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inCoeffHV));
				string inCoeffHVFileName = string(charValue);
				coeffHV = vectorUtils.readGSLVectorFromTxt(inCoeffHVFileName);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No Coefficients not Provided for HV");
			}
			XMLString::release(&inCoeffHV);


			RSGISFunctionLinXfLinY *functionLnHH;
			RSGISFunctionLinXfLinY *functionLnHV;

			functionLnHH = new RSGISFunctionLinXfLinY(gsl_vector_get(coeffHH, 0), gsl_vector_get(coeffHH, 1),gsl_vector_get(coeffHH, 2),gsl_vector_get(coeffHH, 3));
			functionLnHV = new RSGISFunctionLinXfLinY(gsl_vector_get(coeffHV, 0), gsl_vector_get(coeffHV, 1),gsl_vector_get(coeffHV, 2),gsl_vector_get(coeffHV, 3));

			this->functionHH = (RSGISMathTwoVariableFunction *) functionLnHH;
			this->functionHV = (RSGISMathTwoVariableFunction *) functionLnHV;

			gsl_vector_free(coeffHH);
			gsl_vector_free(coeffHV);
		}
		else if (XMLString::equals(function2DPoly,functionStr))
		{
			// Read coefficients
			XMLCh *coefficientsHH = XMLString::transcode("coefficientsHH");
			if(argElement->hasAttribute(coefficientsHH))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(coefficientsHH));
				string coeffHHFile = string(charValue);
				this->coeffHH = matrixUtils.readGSLMatrixFromTxt(coeffHHFile);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No coefficents provided for HH");
			}
			XMLString::release(&coefficientsHH);
			XMLCh *coefficientsHV = XMLString::transcode("coefficientsHV");
			if(argElement->hasAttribute(coefficientsHV))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(coefficientsHV));
				string coeffHVFile = string(charValue);
				this->coeffHV = matrixUtils.readGSLMatrixFromTxt(coeffHVFile);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No coefficents provided for HV");
			}
			XMLString::release(&coefficientsHV);

			// Set up functions
			RSGISFunction2DPoly *functionPolyHH;
			RSGISFunction2DPoly *functionPolyHV;

			functionPolyHH = new RSGISFunction2DPoly(coeffHH);
			functionPolyHV = new RSGISFunction2DPoly(coeffHV);

			this->functionHH = (RSGISMathTwoVariableFunction *) functionPolyHH;
			this->functionHV = (RSGISMathTwoVariableFunction *) functionPolyHV;
		}

		XMLString::release(&functionLn2Var);
		XMLString::release(&functionLn2VarQuadratic);
		XMLString::release(&functionLinXfLinY);
		XMLString::release(&function2DPoly);
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionStr)) + string(") is not known: RSGISExeEstimationAlgorithm.");
		throw RSGISXMLArgumentsException(message.c_str());
	}

	parsed = true;

	// Release XML
	XMLString::release(&typeDualPolObject);
	XMLString::release(&typeFullPolObject);
	XMLString::release(&typeDualPolObjectObjAP);
	XMLString::release(&typeFullPolObjectObjAP);
	XMLString::release(&typeSimData2Var2Data);
}

void RSGISExeEstimationAlgorithm::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		//---- Dual / Full Pol Single Species - Object Based ---//
        if((option == RSGISExeEstimationAlgorithm::dualPolObject) | (option == RSGISExeEstimationAlgorithm::fullPolObject) | (option == RSGISExeEstimationAlgorithm::fullPolObjectObjAP) | (option == RSGISExeEstimationAlgorithm::dualPolObjectObjAP) )
		{
			GDALAllRegister();
			OGRRegisterAll();

			rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
			rsgis::img::RSGISImageUtils imgUtils;

			rsgis::vec::RSGISProcessVector *processVector = NULL;
			rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputObjPolys);

			GDALDataset *inputImageDS = NULL;
			GDALDataset *outputImageDS = NULL;
			GDALDataset **dataset = NULL;
			GDALDataset *inputRasterFeaturesDS = NULL;
			GDALDataset *inputSHPDS = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;
			GDALDriver *shpFiledriver = NULL;

			string outputDIR = "";

			try
			{
				// OPEN INPUT IMAGE
				inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inputImageDS == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}

				if (this->useRasPoly)
				{
					inputRasterFeaturesDS = (GDALDataset *) GDALOpen(this->inputRasPolys.c_str(), GA_ReadOnly);
					if(inputRasterFeaturesDS == NULL)
					{
						string message = string("Could not open image ") + this->inputRasPolys;
						throw RSGISException(message.c_str());
					}
				}

				// CREATE OUTPUT IMAGE
				GDALDriver *gdalDriver = NULL;
				double *gdalTranslation = new double[6];
				int **dsOffsets = new int*[1];
				dsOffsets[0] = new int[2];
				int height = 0;
				int width = 0;

				dataset = new GDALDataset*[1];
				dataset[0] = inputImageDS;

				imgUtils.getImageOverlap(dataset, 1, dsOffsets, &width, &height, gdalTranslation);
				gdalDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
				if(gdalDriver == NULL)
				{
					throw RSGISImageBandException("ENVI driver does not exists..");
				}
				cout << "New image width = " << width << " height = " << height << " bands = " << this->nBands << endl;

				outputImageDS = imgUtils.createBlankImage(outputImage.c_str(), gdalTranslation, width, height, this->nBands, inputImageDS->GetProjectionRef(), 0);

				delete[] gdalTranslation;
				delete[] dataset;

				// OPEN INPUT SHAPEFILE
				inputSHPDS = (GDALDataset*) GDALOpenEx(this->inputObjPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputObjPolys;
					throw RSGISException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
				}

				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				// CREATE OUTPUT SHAPEFILE (IF USING)
				if (this->createOutSHP)
				{
					string outputDIR = fileUtils.getFileDirectoryPath(this->outputSHP);
					string SHPFileOutLayer = vecUtils.getLayerName(this->outputSHP);

					// Check is output shapefile exists
					if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
					{
						if(this->force)
						{
							vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
						}
						else
						{
							throw RSGISException("Shapefile already exists, either delete or select force.");
						}
					}

					const char *pszDriverName = "ESRI Shapefile";
					shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
					if( shpFiledriver == NULL )
					{
						throw RSGISException("SHP driver not available.");
					}
					outputSHPDS = shpFiledriver->Create(this->outputSHP.c_str(), 0, 0, 0, GDT_Unknown, NULL );
					if( outputSHPDS == NULL )
					{
						string message = string("Could not create vector file ") + this->outputSHP;
						throw RSGISException(message.c_str());
					}
					outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
					if( outputSHPLayer == NULL )
					{
						string message = string("Could not create vector layer ") + SHPFileOutLayer;
						throw RSGISException(message.c_str());
					}
				}

				if (useDefaultMinMax)
				{
					this->minMaxValuesClass = NULL;
                    std::cout << "Using default min/max values for parameters" << std::endl;
				}

				// Perform Inversion
				if (this->useRasPoly)
				{
					if (this->objAP)
					{
                        processFeature = new rsgis::radar::RSGISObjectBasedEstimationObjectAP(inputImageDS, outputImageDS, inputRasterFeaturesDS, this->initialParClass, this->estSlowOptimiserClass, this->estFastOptimiserClass, this->parameters, this->objAPParField, this->minMaxValuesClass, this->classField, this->useClass);
						processVector = new rsgis::vec::RSGISProcessVector(processFeature);
						if (this->createOutSHP)
						{
							processVector->processVectors(inputSHPLayer, outputSHPLayer, this->copyAttributes, true, false);
						}
						else
						{
							processVector->processVectorsNoOutput(inputSHPLayer, true);
						}
						delete processVector;
						delete processFeature;
					}
					else
					{
                        processFeature = new rsgis::radar::RSGISObjectBasedEstimation(inputImageDS, outputImageDS, inputRasterFeaturesDS, this->initialParClass, this->estSlowOptimiserClass, this->estFastOptimiserClass, this->parameters, this->minMaxValuesClass, this->classField, this->useClass);
                        processVector = new rsgis::vec::RSGISProcessVector(processFeature);
						if (this->createOutSHP)
						{
							processVector->processVectors(inputSHPLayer, outputSHPLayer, this->copyAttributes, true, false);
						}
						else
						{
							processVector->processVectorsNoOutput(inputSHPLayer, true);
						}
						delete processVector;
						delete processFeature;
					}

				}
				else
				{
					if (this->objAP)
					{
						processFeature = new rsgis::radar::RSGISObjectBasedEstimationObjectAP(inputImageDS, outputImageDS, NULL, this->initialParClass, this->estSlowOptimiserClass, this->estFastOptimiserClass, this->parameters, this->objAPParField, this->minMaxValuesClass, this->classField, this->useClass);
						processVector = new rsgis::vec::RSGISProcessVector(processFeature);
						if (this->createOutSHP)
						{
							processVector->processVectors(inputSHPLayer, outputSHPLayer, this->copyAttributes, true, false);
						}
						else
						{
							processVector->processVectorsNoOutput(inputSHPLayer, true);
						}
						delete processVector;
						delete processFeature;
					}
					else
					{
						processFeature = new rsgis::radar::RSGISObjectBasedEstimation(inputImageDS, outputImageDS, NULL, this->initialParClass, this->estSlowOptimiserClass, this->estFastOptimiserClass, this->parameters, this->minMaxValuesClass, this->classField, this->useClass);
						processVector = new rsgis::vec::RSGISProcessVector(processFeature);
						if (this->createOutSHP)
						{
							processVector->processVectors(inputSHPLayer, outputSHPLayer, this->copyAttributes, true, false);
						}
						else
						{
							processVector->processVectorsNoOutput(inputSHPLayer, true);
						}
						delete processVector;
						delete processFeature;
					}
				}

				// TIDY
				GDALClose(inputImageDS); // Close input image
				GDALClose(outputImageDS); // Close output image
				cout << "Image closed OK" << endl;
				GDALClose(inputSHPDS); // Close inputshape
				cout << "Input Object SHP closed OK" << endl;
				if (this->createOutSHP)
				{
					GDALClose(outputSHPDS); // Close outputshape
					cout << "Output SHP closed OK" << endl;
				}

				//OGRCleanupAll();
				//GDALDestroyDriverManager();
			}
			catch (RSGISException& e)
			{
				throw e;
			}

			// TIDY
			this->estSlowOptimiserClass->clear();
			this->estFastOptimiserClass->clear();
			for (unsigned int i = 0; i < this->initialParClass->size(); i++)
			{
				gsl_vector_free(this->initialParClass->at(i));
			}
			this->initialParClass->clear();
		}
		// --- Simulated Data - 2Var2Data --//
		else if(option == RSGISExeEstimationAlgorithm::simData2Var2Data)
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
			catch(RSGISException& e)
			{
				throw e;
			}
			RSGISCalcImage *calcImg = NULL;
			RSGISEstimationAlgorithmGenerateSimulatedData2Var2Data *simData = NULL;
			try
			{
				cout << "Starting to generate simulated data set" <<endl;
				simData = new RSGISEstimationAlgorithmGenerateSimulatedData2Var2Data(this->nBands, this->functionHH, this->functionHV);
				cout << "calc image" << endl;
				calcImg = new RSGISCalcImage(simData, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
				cout << "returned to exe\n";
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			if(datasets != NULL)
			{
				GDALClose(datasets[0]);
				delete[] datasets;
			}

			delete calcImg;
			delete simData;
		}
	}

}

void RSGISExeEstimationAlgorithm::printParameters()
{
}

void RSGISExeEstimationAlgorithm::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
    cout << "<!-- A command to estimate parameters from SAR data using image objects --> " << endl;
    cout << "<rsgis:command algor=\"estimation\" option=\"dualPolObject | fullPolObject\" parameters=\"heightDensity\"" << endl;
    cout << "input=\"input.env\" output=\"output.env\" object=\"objects.shp\" classField=\"ClassID\" [raster=\"objects_raster.shp\"] [outSHP=\"output.shp\"] >" << endl;
    cout << "" << endl;
    cout << "<rsgis:estSlowOptimiserParameters     " << endl;
    cout << "    method=\"simulatedAnnealingAP\" " << endl;
    cout << "    function=\"2DPoly\" " << endl;
    cout << "    coefficientsHH=\"heightDensHH_Coeff.mtxt\" " << endl;
    cout << "    coefficientsHV=\"heightDensHH_Coeff.mtxt\"" << endl;
    cout << "    [coefficientsVV=\"heightDensVV_Coeff.mtxt\"]" << endl;
    cout << "    covMatrixP=\"covMatrixP.mtxt\"" << endl;
    cout << "    initialHeight=\"float\" initialDensity=\"float\"" << endl;
    cout << "    minHeight=\"float\" maxHeight=\"float\" minDensity=\"float\" maxDensity=\"float\" heightStep=\"float\" densityStep=\"float\" startTemp=\"int\" ittmax=\"int\" />" << endl;
    cout << "<rsgis:estFastOptimiserParameters     " << endl;
    cout << "    method=\"conjugateGradient\" " << endl;
    cout << "    function=\"2DPoly\" " << endl;
    cout << "    coefficientsHH=\"heightDensHH_Coeff.mtxt\" " << endl;
    cout << "    coefficientsHV=\"heightDensHH_Coeff.mtxt\"" << endl;
    cout << "    [coefficientsVV=\"heightDensVV_Coeff.mtxt\"]" << endl;
    cout << "    covMatrixP=\"covMatrixP.mtxt\"" << endl;
    cout << "    initialHeight=\"float\" initialDensity=\"float\"" << endl;
    cout << "    minHeight=\"float\" maxHeight=\"float\" minDensity=\"float\" maxDensity=\"float\" ittmax=\"int\" />" << endl;
    cout << "</rsgis:command>" << endl;
}

string RSGISExeEstimationAlgorithm::getDescription()
{
	return "Estimates parameters from polametric SAR data";
}

string RSGISExeEstimationAlgorithm::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeEstimationAlgorithm::~RSGISExeEstimationAlgorithm()
{

}

}

