/*
 *  RSGISExeClassification.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/12/2008.
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

#include "RSGISExeClassification.h"

namespace rsgisexe{

RSGISExeClassification::RSGISExeClassification() : RSGISAlgorithmParameters()
{
	this->algorithm = "classification";
	this->inputImage = "";
	this->outputImage = "";
	this->vector = "";
	this->classAttribute = "";
	this->trainingData = NULL;
	this->rules = NULL;
	this->numClasses = 0;
}

rsgis::RSGISAlgorithmParameters* RSGISExeClassification::getInstance()
{
	return new RSGISExeClassification();
}

void RSGISExeClassification::retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException)
{
	rsgis::math::RSGISMathsUtils mathUtils;
    rsgis::utils::RSGISTextUtils textUtils;
	rsgis::math::RSGISMatrices matrixUtils;
	rsgis::math::RSGISVectors vectorUtils;
	
	XMLCh *algorName = xercesc::XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = xercesc::XMLString::transcode("algor");
	XMLCh *optionXMLStr = xercesc::XMLString::transcode("option");
	XMLCh *classXMLStr = xercesc::XMLString::transcode("rsgis:class");
	XMLCh *optionNN = xercesc::XMLString::transcode("nn");
	XMLCh *optionVectorSQL = xercesc::XMLString::transcode("vectorsql");
	XMLCh *optionSAMRule = xercesc::XMLString::transcode("samRule");
	XMLCh *optionSAMClassify = xercesc::XMLString::transcode("samClassify");
	XMLCh *optionSCMRule = xercesc::XMLString::transcode("scmRule");
	XMLCh *optionSCMClassify = xercesc::XMLString::transcode("scmClassify");
	XMLCh *optionCumulativeAreaRule = xercesc::XMLString::transcode("cumulativeAreaRule");
	XMLCh *optionCumulativeAreaClassify = xercesc::XMLString::transcode("cumulativeAreaClassify");
	XMLCh *optionKMeans = xercesc::XMLString::transcode("kmeans");
	XMLCh *optionISOData = xercesc::XMLString::transcode("isodata");
	XMLCh *optionCreateSpecLib = xercesc::XMLString::transcode("createspeclib");
    XMLCh *optionAddColourTable = xercesc::XMLString::transcode("addcolourtable");
    XMLCh *optionElimSinglePxls = xercesc::XMLString::transcode("elimsinglepxls");
    XMLCh *optionCollapseClasses = xercesc::XMLString::transcode("collapseclasses");
    XMLCh *optionColour3Bands = xercesc::XMLString::transcode("colour3bands");
	
	const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
	if(!xercesc::XMLString::equals(algorName, algorNameEle))
	{
		throw rsgis::RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
	if(xercesc::XMLString::equals(optionNN, optionXML))
	{		
		this->option = RSGISExeClassification::nn;
		
		XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		xercesc::XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
		
		
		xercesc::DOMNodeList *classNodesList = argElement->getElementsByTagName(classXMLStr);
		this->numClasses = classNodesList->getLength();
		
		std::cout << "Found " << this->numClasses << " classes" << std::endl;
		
		trainingData = new rsgis::classifier::ClassData*[numClasses];
		xercesc::DOMElement *classElement = NULL;
		for(int i = 0; i < numClasses; i++)
		{
			trainingData[i] = new rsgis::classifier::ClassData();
			classElement = static_cast<xercesc::DOMElement*>(classNodesList->item(i));
			
			XMLCh *nameXMLStr = xercesc::XMLString::transcode("name");
			if(classElement->hasAttribute(nameXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(nameXMLStr));
				trainingData[i]->className = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			xercesc::XMLString::release(&nameXMLStr);
			
			XMLCh *idXMLStr = xercesc::XMLString::transcode("id");
			if(classElement->hasAttribute(idXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(idXMLStr));
				trainingData[i]->classID = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'id\' attribute was provided.");
			}
			xercesc::XMLString::release(&idXMLStr);
			
			XMLCh *matrixXMLStr = xercesc::XMLString::transcode("matrix");
			if(classElement->hasAttribute(matrixXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(matrixXMLStr));
				trainingData[i]->data = matrixUtils.readMatrixFromTxt(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'matrix\' attribute was provided.");
			}
			xercesc::XMLString::release(&matrixXMLStr);
		}
		
	}
	else if(xercesc::XMLString::equals(optionVectorSQL, optionXML))
	{
		this->option = RSGISExeClassification::vectorsql;
		
		XMLCh *vectorXMLStr = xercesc::XMLString::transcode("vector");
		if(argElement->hasAttribute(vectorXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(vectorXMLStr));
			this->vector = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'vector\' attribute was provided.");
		}
		xercesc::XMLString::release(&vectorXMLStr);
		
		
		XMLCh *classAttributeXMLStr = xercesc::XMLString::transcode("class");
		if(argElement->hasAttribute(classAttributeXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(classAttributeXMLStr));
			this->classAttribute = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'class\' attribute was provided.");
		}
		xercesc::XMLString::release(&classAttributeXMLStr);
		
		xercesc::DOMNodeList *classNodesList = argElement->getElementsByTagName(classXMLStr);
		this->numClasses = classNodesList->getLength();
		
		std::cout << "Found " << this->numClasses << " classes" << std::endl;
		
		rules = new rsgis::vec::sqlclass*[numClasses];
		xercesc::DOMElement *classElement = NULL;
		for(int i = 0; i < numClasses; i++)
		{
			rules[i] = new rsgis::vec::sqlclass();
			classElement = static_cast<xercesc::DOMElement*>(classNodesList->item(i));
			
			XMLCh *nameXMLStr = xercesc::XMLString::transcode("name");
			if(classElement->hasAttribute(nameXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(nameXMLStr));
				rules[i]->name = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			xercesc::XMLString::release(&nameXMLStr);
			
			
			XMLCh *sqlXMLStr = xercesc::XMLString::transcode("sql");
			if(classElement->hasAttribute(sqlXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(sqlXMLStr));
				rules[i]->sql = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'sql\' attribute was provided.");
			}
			xercesc::XMLString::release(&sqlXMLStr);
		}
	}
	else if(xercesc::XMLString::equals(optionSAMRule, optionXML))
	{
		this->option = RSGISExeClassification::samRule;
		
		XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'image\' was provided.");
		}
		xercesc::XMLString::release(&imageXMLStr);
		
		XMLCh *ruleImageXMLStr = xercesc::XMLString::transcode("ruleImage");
		if(argElement->hasAttribute(ruleImageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(ruleImageXMLStr));
			this->ruleImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'rule\' image was provided.");
		}
		xercesc::XMLString::release(&ruleImageXMLStr);
		
		XMLCh *specLibXMLStr = xercesc::XMLString::transcode("specLib");
		if(argElement->hasAttribute(specLibXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(specLibXMLStr));
            rsgis::math::RSGISMatrices matrixUtils;
			std::string specLibStr = std::string(charValue);
			this->specLib = matrixUtils.readGSLMatrixFromGridTxt(specLibStr);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'specLib\' spectral library file provided");
		}
		xercesc::XMLString::release(&specLibXMLStr);
		
	}
	else if(xercesc::XMLString::equals(optionSAMClassify, optionXML))
	{
		this->option = RSGISExeClassification::samClassify;
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
		
		XMLCh *ruleImageXMLStr = xercesc::XMLString::transcode("ruleImage");
		if(argElement->hasAttribute(ruleImageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(ruleImageXMLStr));
			this->ruleImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'rule\' image was provided.");
		}
		xercesc::XMLString::release(&ruleImageXMLStr);
		
		XMLCh *thresholdXMLStr = xercesc::XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			std::string thresholdStr = std::string(charValue);
			this->threshold = mathUtils.strtodouble(thresholdStr);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for threshold");
		}
		xercesc::XMLString::release(&thresholdXMLStr);
	}
	else if(xercesc::XMLString::equals(optionSCMRule, optionXML))
	{
		this->option = RSGISExeClassification::scmRule;
		
		XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'image\' was provided.");
		}
		xercesc::XMLString::release(&imageXMLStr);
		
		XMLCh *ruleImageXMLStr = xercesc::XMLString::transcode("ruleImage");
		if(argElement->hasAttribute(ruleImageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(ruleImageXMLStr));
			this->ruleImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'rule\' image was provided.");
		}
		xercesc::XMLString::release(&ruleImageXMLStr);
		
		XMLCh *specLibXMLStr = xercesc::XMLString::transcode("specLib");
		if(argElement->hasAttribute(specLibXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(specLibXMLStr));
			rsgis::math::RSGISMatrices matrixUtils;
			std::string specLibStr = std::string(charValue);
			this->specLib = matrixUtils.readGSLMatrixFromGridTxt(specLibStr);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'specLib\' spectral library file provided");
		}
		xercesc::XMLString::release(&specLibXMLStr);
		
	}
	else if(xercesc::XMLString::equals(optionSCMClassify, optionXML))
	{
		this->option = RSGISExeClassification::scmClassify;
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
		
		XMLCh *ruleImageXMLStr = xercesc::XMLString::transcode("ruleImage");
		if(argElement->hasAttribute(ruleImageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(ruleImageXMLStr));
			this->ruleImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'rule\' image was provided.");
		}
		xercesc::XMLString::release(&ruleImageXMLStr);
		
		XMLCh *thresholdXMLStr = xercesc::XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			std::string thresholdStr = std::string(charValue);
			this->threshold = mathUtils.strtodouble(thresholdStr);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for threshold");
		}
		xercesc::XMLString::release(&thresholdXMLStr);
	}	
	else if(xercesc::XMLString::equals(optionCumulativeAreaRule, optionXML))
	{		
		this->option = RSGISExeClassification::cumulativeAreaRule;
		
		XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		xercesc::XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
		
		XMLCh *imageBandsXMLStr = xercesc::XMLString::transcode("imagebands");
		if(argElement->hasAttribute(imageBandsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageBandsXMLStr));
			this->inMatrixfile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'imagebands\' attribute was provided.");
		}
		xercesc::XMLString::release(&imageBandsXMLStr);
		
		XMLCh *specLibXMLStr = xercesc::XMLString::transcode("specLib");
		if(argElement->hasAttribute(specLibXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(specLibXMLStr));
			inMatrixSpecLibStr = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'specLib\' spectral library file provided");
		}
		xercesc::XMLString::release(&specLibXMLStr);
		
	}
	else if(xercesc::XMLString::equals(optionCumulativeAreaClassify, optionXML))
	{
		this->option = RSGISExeClassification::cumulativeAreaClassify;
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
		
		XMLCh *ruleImageXMLStr = xercesc::XMLString::transcode("ruleImage");
		if(argElement->hasAttribute(ruleImageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(ruleImageXMLStr));
			this->ruleImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'rule\' image was provided.");
		}
		xercesc::XMLString::release(&ruleImageXMLStr);
		
		XMLCh *thresholdXMLStr = xercesc::XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			std::string thresholdStr = std::string(charValue);
			this->threshold = mathUtils.strtodouble(thresholdStr);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for threshold");
		}
		xercesc::XMLString::release(&thresholdXMLStr);
	}
	else if(xercesc::XMLString::equals(optionKMeans, optionXML))
	{
		this->option = RSGISExeClassification::kmeans;
		this->printinfo = false; // Set default to don't print info
		this->savekmeansCentres = false; // Set default to don't export centres;
		
		XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		xercesc::XMLString::release(&imageXMLStr);		
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
		
		XMLCh *numClustersXMLStr = xercesc::XMLString::transcode("numclusters");
		if(argElement->hasAttribute(numClustersXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(numClustersXMLStr));
			std::string strVal = std::string(charValue);
			this->numClusters = mathUtils.strtoint(strVal);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for numclusters");
		}
		xercesc::XMLString::release(&numClustersXMLStr);
		
		
		XMLCh *initAlgorXMLStr = xercesc::XMLString::transcode("initalgor");
		if(argElement->hasAttribute(initAlgorXMLStr))
		{
			XMLCh *ranStr = xercesc::XMLString::transcode("random");
			XMLCh *kppStr = xercesc::XMLString::transcode("kpp");
			
			const XMLCh *initAlgorValue = argElement->getAttribute(initAlgorXMLStr);
			
			if(xercesc::XMLString::equals(initAlgorValue, ranStr))
			{
				this->initAlgor = randomInit;
			}
			else if(xercesc::XMLString::equals(initAlgorValue, kppStr))
			{
				this->initAlgor = kppInit;
			}
			else
			{
				this->initAlgor = undefinedInit;
				throw rsgis::RSGISXMLArgumentsException("\'initalgor\' value not recognised.");
			}
			xercesc::XMLString::release(&ranStr);
			xercesc::XMLString::release(&kppStr);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'initalgor\' attribute was provided.");
		}
		xercesc::XMLString::release(&initAlgorXMLStr);
		
		
		XMLCh *maxIterationsXMLStr = xercesc::XMLString::transcode("maxiterations");
		if(argElement->hasAttribute(maxIterationsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
			std::string strVal = std::string(charValue);
			this->maxNumIterations = mathUtils.strtoint(strVal);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for \'maxiterations\'");
		}
		xercesc::XMLString::release(&maxIterationsXMLStr);
		
		XMLCh *clusterMoveXMLStr = xercesc::XMLString::transcode("clustermove");
		if(argElement->hasAttribute(clusterMoveXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clusterMoveXMLStr));
			std::string strVal = std::string(charValue);
			this->clusterMoveThreshold = mathUtils.strtofloat(strVal);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for \'clustermove\'");
		}
		xercesc::XMLString::release(&clusterMoveXMLStr);
		

		XMLCh *printInfoXMLStr = xercesc::XMLString::transcode("printinfo");
		if(argElement->hasAttribute(printInfoXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(printInfoXMLStr);
			
			if(xercesc::XMLString::equals(forceValue, yesStr))
			{
				this->printinfo = true;
			}
			else
			{
				this->printinfo = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		xercesc::XMLString::release(&printInfoXMLStr);

		XMLCh *centresXMLStr = xercesc::XMLString::transcode("centres");
		if(argElement->hasAttribute(centresXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(centresXMLStr));
			this->outkmeansCentresFileName = std::string(charValue);
			this->savekmeansCentres = true;
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->outkmeansCentresFileName = "";
		}
		xercesc::XMLString::release(&centresXMLStr);

	}
	else if(xercesc::XMLString::equals(optionISOData, optionXML))
	{
		this->option = RSGISExeClassification::isodata;
		
		XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		xercesc::XMLString::release(&imageXMLStr);		
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
		
		XMLCh *numClustersXMLStr = xercesc::XMLString::transcode("numclusters");
		if(argElement->hasAttribute(numClustersXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(numClustersXMLStr));
			std::string strVal = std::string(charValue);
			this->numClusters = mathUtils.strtoint(strVal);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for numclusters");
		}
		xercesc::XMLString::release(&numClustersXMLStr);
		
		
		XMLCh *initAlgorXMLStr = xercesc::XMLString::transcode("initalgor");
		if(argElement->hasAttribute(initAlgorXMLStr))
		{
			XMLCh *ranStr = xercesc::XMLString::transcode("random");
			XMLCh *kppStr = xercesc::XMLString::transcode("kpp");
			
			const XMLCh *initAlgorValue = argElement->getAttribute(initAlgorXMLStr);
			
			if(xercesc::XMLString::equals(initAlgorValue, ranStr))
			{
				this->initAlgor = randomInit;
			}
			else if(xercesc::XMLString::equals(initAlgorValue, kppStr))
			{
				this->initAlgor = kppInit;
			}
			else
			{
				this->initAlgor = undefinedInit;
				throw rsgis::RSGISXMLArgumentsException("\'initalgor\' value not recognised.");
			}
			xercesc::XMLString::release(&ranStr);
			xercesc::XMLString::release(&kppStr);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'initalgor\' attribute was provided.");
		}
		xercesc::XMLString::release(&initAlgorXMLStr);
		
		
		XMLCh *maxIterationsXMLStr = xercesc::XMLString::transcode("maxiterations");
		if(argElement->hasAttribute(maxIterationsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
			std::string strVal = std::string(charValue);
			this->maxNumIterations = mathUtils.strtoint(strVal);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for \'maxiterations\'");
		}
		xercesc::XMLString::release(&maxIterationsXMLStr);
		
		XMLCh *clusterMoveXMLStr = xercesc::XMLString::transcode("clustermove");
		if(argElement->hasAttribute(clusterMoveXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clusterMoveXMLStr));
			std::string strVal = std::string(charValue);
			this->clusterMoveThreshold = mathUtils.strtofloat(strVal);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for \'clustermove\'");
		}
		xercesc::XMLString::release(&clusterMoveXMLStr);
		
		
		XMLCh *minNumberValsXMLStr = xercesc::XMLString::transcode("minnumbervals");
		if(argElement->hasAttribute(minNumberValsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(minNumberValsXMLStr));
			std::string strVal = std::string(charValue);
			this->minNumVals = mathUtils.strtoint(strVal);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for \'minnumbervals\'");
		}
		xercesc::XMLString::release(&minNumberValsXMLStr);
		
		XMLCh *minDistanceBetweenCentresXMLStr = xercesc::XMLString::transcode("mindistbetweencluster");
		if(argElement->hasAttribute(minDistanceBetweenCentresXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(minDistanceBetweenCentresXMLStr));
			std::string strVal = std::string(charValue);
			this->minDistanceBetweenCentres = mathUtils.strtofloat(strVal);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for \'mindistbetweencluster\'");
		}
		xercesc::XMLString::release(&minDistanceBetweenCentresXMLStr);
		
		
		XMLCh *stddevThresXMLStr = xercesc::XMLString::transcode("maxstddev");
		if(argElement->hasAttribute(stddevThresXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(stddevThresXMLStr));
			std::string strVal = std::string(charValue);
			this->stddevThres = mathUtils.strtofloat(strVal);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for \'maxstddev\'");
		}
		xercesc::XMLString::release(&stddevThresXMLStr);
		
		
		XMLCh *propOverAvgDistXMLStr = xercesc::XMLString::transcode("propAvgDist");
		if(argElement->hasAttribute(propOverAvgDistXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(propOverAvgDistXMLStr));
			std::string strVal = std::string(charValue);
			this->propOverAvgDist = mathUtils.strtofloat(strVal);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for \'propAvgDist\'");
		}
		xercesc::XMLString::release(&propOverAvgDistXMLStr);
		
		XMLCh *printInfoXMLStr = xercesc::XMLString::transcode("printinfo");
		if(argElement->hasAttribute(printInfoXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(printInfoXMLStr);
			
			if(xercesc::XMLString::equals(forceValue, yesStr))
			{
				this->printinfo = true;
			}
			else
			{
				this->printinfo = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'printinfo\' attribute was provided.");
		}
		xercesc::XMLString::release(&printInfoXMLStr);
		
	}
	else if(xercesc::XMLString::equals(optionCreateSpecLib, optionXML))
	{
		this->option = RSGISExeClassification::createspeclib;
		
		XMLCh *vectorXMLStr = xercesc::XMLString::transcode("vector");
		if(argElement->hasAttribute(vectorXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(vectorXMLStr));
			this->vector = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'vector\' attribute was provided.");
		}
		xercesc::XMLString::release(&vectorXMLStr);
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
				
		XMLCh *numClustersXMLStr = xercesc::XMLString::transcode("classattribute");
		if(argElement->hasAttribute(numClustersXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(numClustersXMLStr));
			this->classAttribute = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No classattribute provided for numclusters");
		}
		xercesc::XMLString::release(&numClustersXMLStr);
		
		XMLCh *valueAttributesXMLStr = xercesc::XMLString::transcode("valueattributes");
		if(argElement->hasAttribute(valueAttributesXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(valueAttributesXMLStr));
			std::string attributes = std::string(charValue);
			xercesc::XMLString::release(&charValue);
			
			valueAttributes = new std::vector<std::string>();
			
			textUtils.tokenizeString(attributes, ',', valueAttributes, true);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No valueattributes provided for numclusters");
		}
		xercesc::XMLString::release(&valueAttributesXMLStr);		
		
		XMLCh *groupXMLStr = xercesc::XMLString::transcode("group");
		if(argElement->hasAttribute(groupXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *groupValue = argElement->getAttribute(groupXMLStr);
			
			if(xercesc::XMLString::equals(groupValue, yesStr))
			{
				this->groupSamples = true;
			}
			else
			{
				this->groupSamples = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'group\' attribute was provided.");
		}
		xercesc::XMLString::release(&groupXMLStr);
		
	}
    else if(xercesc::XMLString::equals(optionAddColourTable, optionXML))
    {
        this->option = RSGISExeClassification::addcolourtable;
        
        XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
        if(argElement->hasAttribute(imageXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
            this->inputImage = std::string(charValue);
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
        }
        xercesc::XMLString::release(&imageXMLStr);
        
        XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
        if(argElement->hasAttribute(bandXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandXMLStr));
            this->imageBand = textUtils.strto32bitUInt(std::string(charValue));
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
        }
        xercesc::XMLString::release(&bandXMLStr);
                
        XMLCh *rsgisClassXMLStr = xercesc::XMLString::transcode("rsgis:class");
        xercesc::DOMNodeList *classNodesList = argElement->getElementsByTagName(rsgisClassXMLStr);
        unsigned int numClassTags = classNodesList->getLength();
        
        std::cout << "Found " << numClassTags << " class tags" << std::endl;
        
        if(numClassTags == 0)
        {
            throw rsgis::RSGISXMLArgumentsException("No class tags have been provided, at least 1 is required.");
        }
        
        xercesc::DOMElement *attElement = NULL;
        int classValue = 0;
        int red = 0;
        int green = 0;
        int blue = 0;
        int alpha = 0;
        for(int i = 0; i < numClassTags; i++)
        {
            attElement = static_cast<xercesc::DOMElement*>(classNodesList->item(i));
            
            XMLCh *valueXMLStr = xercesc::XMLString::transcode("value");
            if(attElement->hasAttribute(valueXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(valueXMLStr));
                classValue = textUtils.strto32bitInt(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'value\' attribute was provided.");
            }
            xercesc::XMLString::release(&valueXMLStr);
            
            XMLCh *redXMLStr = xercesc::XMLString::transcode("r");
            if(attElement->hasAttribute(redXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(redXMLStr));
                red = textUtils.strto32bitInt(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'r\' attribute was provided.");
            }
            xercesc::XMLString::release(&redXMLStr);
            
            XMLCh *greenXMLStr = xercesc::XMLString::transcode("g");
            if(attElement->hasAttribute(greenXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(greenXMLStr));
                green = textUtils.strto32bitInt(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'g\' attribute was provided.");
            }
            xercesc::XMLString::release(&greenXMLStr);
            
            XMLCh *blueXMLStr = xercesc::XMLString::transcode("b");
            if(attElement->hasAttribute(blueXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(blueXMLStr));
                blue = textUtils.strto32bitInt(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'b\' attribute was provided.");
            }
            xercesc::XMLString::release(&blueXMLStr);
            
            XMLCh *alphaXMLStr = xercesc::XMLString::transcode("a");
            if(attElement->hasAttribute(alphaXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(alphaXMLStr));
                alpha = textUtils.strto32bitInt(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'a\' attribute was provided.");
            }
            xercesc::XMLString::release(&alphaXMLStr);
            
            classColourPairs.push_back(std::pair<int, rsgis::utils::RSGISColourInt>(classValue, rsgis::utils::RSGISColourInt(red, green, blue, alpha)));
        }
        
        
    }
    else if(xercesc::XMLString::equals(optionElimSinglePxls, optionXML))
	{
		this->option = RSGISExeClassification::elimsinglepxls;
		
		XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
        if(argElement->hasAttribute(imageXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
            this->inputImage = std::string(charValue);
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
        }
        xercesc::XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
		XMLCh *connectivityXMLStr = xercesc::XMLString::transcode("connectivity");
		if(argElement->hasAttribute(connectivityXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(connectivityXMLStr));
			std::string connectivityTemp = std::string(charValue);
            if(connectivityTemp == "4")
            {
                filterConnectivity = rsgis::img::rsgis_4connect;
            }
            else if(connectivityTemp == "8")
            {
                filterConnectivity = rsgis::img::rsgis_8connect;
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("Connectivity must be either 4 ot 8.");
            }
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'connectivity\' attribute was provided");
		}
		xercesc::XMLString::release(&connectivityXMLStr);
        
        XMLCh *ignoreZerosXMLStr = xercesc::XMLString::transcode("ignorezeros");
		if(argElement->hasAttribute(ignoreZerosXMLStr))
		{
            XMLCh *noStr = xercesc::XMLString::transcode("no");
			const XMLCh *strValue = argElement->getAttribute(ignoreZerosXMLStr);
			
			if(xercesc::XMLString::equals(strValue, noStr))
			{
				this->ignoreZeros = false;
			}
			else
			{
				this->ignoreZeros = true;
			}
			xercesc::XMLString::release(&noStr);
		}
		else
		{
			this->ignoreZeros = true;
            std::cerr << "Ignoring zeros options not specified by default zero will be ignored.\n";
		}
		xercesc::XMLString::release(&ignoreZerosXMLStr);
        
        
        XMLCh *formatXMLStr = xercesc::XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		xercesc::XMLString::release(&formatXMLStr);
        
	}
    else if(xercesc::XMLString::equals(optionCollapseClasses, optionXML))
	{
		this->option = RSGISExeClassification::collapseclasses;
		
		XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
        if(argElement->hasAttribute(imageXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
            this->inputImage = std::string(charValue);
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
        }
        xercesc::XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *formatXMLStr = xercesc::XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		xercesc::XMLString::release(&formatXMLStr);
        
        XMLCh *classnamesXMLStr = xercesc::XMLString::transcode("classnames");
		if(argElement->hasAttribute(classnamesXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(classnamesXMLStr));
			this->classNameCol = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'classnames\' image was provided.");
		}
		xercesc::XMLString::release(&classnamesXMLStr);
        
	}
    else if(xercesc::XMLString::equals(optionColour3Bands, optionXML))
	{
		this->option = RSGISExeClassification::colour3bands;
		
		XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
        if(argElement->hasAttribute(imageXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
            this->inputImage = std::string(charValue);
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
        }
        xercesc::XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *formatXMLStr = xercesc::XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		xercesc::XMLString::release(&formatXMLStr);
        
	}
	else
	{
		std::string message = std::string("The option (") + std::string(xercesc::XMLString::transcode(optionXML)) + std::string(") is not known: RSGISExeClassification.");
		throw rsgis::RSGISXMLArgumentsException(message.c_str());
	}
	
	xercesc::XMLString::release(&algorName);
	xercesc::XMLString::release(&algorXMLStr);
	xercesc::XMLString::release(&optionXMLStr);
	xercesc::XMLString::release(&optionNN);
	xercesc::XMLString::release(&optionVectorSQL);
	xercesc::XMLString::release(&optionSAMRule);
	xercesc::XMLString::release(&optionSAMClassify);
	xercesc::XMLString::release(&optionSCMRule);
	xercesc::XMLString::release(&optionSCMClassify);
	xercesc::XMLString::release(&classXMLStr);
	xercesc::XMLString::release(&optionCumulativeAreaRule);
	xercesc::XMLString::release(&optionCumulativeAreaClassify);
	xercesc::XMLString::release(&optionKMeans);
	xercesc::XMLString::release(&optionISOData);
	xercesc::XMLString::release(&optionCreateSpecLib);
    xercesc::XMLString::release(&optionAddColourTable);
    xercesc::XMLString::release(&optionElimSinglePxls);
    xercesc::XMLString::release(&optionCollapseClasses);
    xercesc::XMLString::release(&optionColour3Bands);
    
	parsed = true;
}

void RSGISExeClassification::runAlgorithm() throw(rsgis::RSGISException)
{
	if(!parsed)
	{
		throw rsgis::RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(option == RSGISExeClassification::nn)
		{
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			rsgis::classifier::RSGISNearestNeighbourClassifier *nnClassifier = NULL;
			rsgis::classifier::RSGISApplyClassifier *applyClassifier = NULL;
			
			try
			{				
				datasets = new GDALDataset*[1];
				std::cout << "Reading in image " << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				nnClassifier = new rsgis::classifier::RSGISNearestNeighbourClassifier(this->trainingData, this->numClasses);
				nnClassifier->printClassIDs();
				applyClassifier = new rsgis::classifier::RSGISApplyClassifier(1, nnClassifier);
				calcImage = new rsgis::img::RSGISCalcImage(applyClassifier, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				GDALClose(datasets[0]);
				GDALDestroyDriverManager();
				
				delete nnClassifier;
				delete applyClassifier;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}			
		}
		else if(option == RSGISExeClassification::vectorsql)
		{
			OGRRegisterAll();
			std::cout << "SQL Rulebased Classification of vector data\n";
			std::cout << "Vector Data: " << this->vector << std::endl;
			std::cout << "Class Name attribute: " << this->classAttribute << std::endl;
			
            rsgis::vec::RSGISVectorUtils vecUtils;
			rsgis::vec::RSGISVectorSQLClassification vecSQLClass;
			
			std::string SHPFileInLayer = vecUtils.getLayerName(this->vector);
			
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			
			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(this->vector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					std::string message = std::string("Could not open vector file ") + this->vector;
					throw rsgis::RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
					throw rsgis::RSGISFileException(message.c_str());
				}
				
				std::string sqlprefix = std::string("SELECT * FROM ") + SHPFileInLayer + std::string(" WHERE ");
				for(int i = 0; i < numClasses; i++)
				{
					rules[i]->sql = sqlprefix + rules[i]->sql;
				}
				vecSQLClass.classifyVector(inputSHPDS, inputSHPLayer, this->rules, this->numClasses, this->classAttribute);
				
				GDALClose(inputSHPDS);
				OGRCleanupAll();
			}
			catch (rsgis::RSGISException e) 
			{
				throw e;
			}
		}
		else if(option == RSGISExeClassification::samRule)
		{
			// Calculate rule image
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			rsgis::img::RSGISCalcImage *calcImg = NULL;
			rsgis::classifier::RSGISSpectralAngleMapperRule *samRule = NULL;
			int numOutputBands = specLib->size2;
			
			try
			{
				datasets = new GDALDataset*[1];
				std::cout << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
			}
			catch(rsgis::RSGISImageException& e)
			{
				throw e;
			}
			
			try
			{
				std::cout << "Producing Rule image" <<std::endl;
				samRule = new rsgis::classifier::RSGISSpectralAngleMapperRule(numOutputBands, specLib);
				calcImg = new rsgis::img::RSGISCalcImage(samRule, "", true);
				calcImg->calcImage(datasets, 1, this->ruleImage);
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
			if(datasets != NULL)
			{
				GDALClose(&datasets[0]);
				delete[] datasets;
			}
			delete calcImg;
			delete samRule;
			
		}
		else if(option == RSGISExeClassification::samClassify)
		{
			// Produce classification
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			rsgis::img::RSGISCalcImage *calcImg = NULL;
			rsgis::classifier::RSGISSpectralAngleMapperClassifier *samClassify = NULL;

			try
			{
				datasets = new GDALDataset*[1];
				std::cout << this->ruleImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->ruleImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->ruleImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
			}
			catch(rsgis::RSGISImageException& e)
			{
				throw e;
			}
			
			try
			{
				std::cout << "Producing classification" <<std::endl;
				samClassify = new rsgis::classifier::RSGISSpectralAngleMapperClassifier(1, this->threshold);
				calcImg = new rsgis::img::RSGISCalcImage(samClassify, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
			if(datasets != NULL)
			{
				GDALClose(&datasets[0]);
				delete[] datasets;
			}
			delete calcImg;
			delete samClassify;
			
			
		}
		else if(option == RSGISExeClassification::scmRule)
		{
			// Calculate rule image
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			rsgis::img::RSGISCalcImage *calcImg = NULL;
			rsgis::classifier::RSGISSpectralCorrelationMapperRule *scmRule = NULL;
			int numOutputBands = specLib->size2;
			
			try
			{
				datasets = new GDALDataset*[1];
				std::cout << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
			}
			catch(rsgis::RSGISImageException& e)
			{
				throw e;
			}
			
			try
			{
				std::cout << "Producing Rule image" <<std::endl;
				scmRule = new rsgis::classifier::RSGISSpectralCorrelationMapperRule(numOutputBands, specLib);
				calcImg = new rsgis::img::RSGISCalcImage(scmRule, "", true);
				calcImg->calcImage(datasets, 1, this->ruleImage);
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
			if(datasets != NULL)
			{
				GDALClose(datasets[0]);
				delete[] datasets;
			}
			delete calcImg;
			delete scmRule;
			
		}
		else if(option == RSGISExeClassification::scmClassify)
		{
			// Produce classification
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			rsgis::img::RSGISCalcImage *calcImg = NULL;
			rsgis::classifier::RSGISSpectralCorrelationMapperClassifier *scmClassify = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				std::cout << this->ruleImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->ruleImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->ruleImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
			}
			catch(rsgis::RSGISImageException& e)
			{
				throw e;
			}
			
			try
			{
				std::cout << "Producing classification" <<std::endl;
				scmClassify = new rsgis::classifier::RSGISSpectralCorrelationMapperClassifier(1, this->threshold);
				calcImg = new rsgis::img::RSGISCalcImage(scmClassify, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
			if(datasets != NULL)
			{
				GDALClose(datasets[0]);
				delete[] datasets;
			}
			delete calcImg;
			delete scmClassify;
			
			
		}
		else if(option == RSGISExeClassification::cumulativeAreaRule)
		{
			std::cout << "Generate cumulative area rule image\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Band wavelength and widths: " << this->inMatrixfile << std::endl;
			std::cout << "Spectral library: " << this->inMatrixSpecLibStr << std::endl;
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::img::RSGISCalcImageValue *calcImageValue = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::RSGISMathsUtils mathsUtils;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				int numImgBands = datasets[0]->GetRasterCount();
				
				rsgis::math::Matrix *bandsValuesMatrix = matrixUtils.readMatrixFromTxt(this->inMatrixfile);
				
				if(bandsValuesMatrix->n != numImgBands)
				{
					GDALClose(datasets[0]);
					matrixUtils.freeMatrix(bandsValuesMatrix);
					
					throw rsgis::RSGISException("The bandvalues matrix needs to have the same number of rows as the input image has bands");
				}
				
				if(bandsValuesMatrix->m != 2)
				{
					GDALClose(datasets[0]);
					matrixUtils.freeMatrix(bandsValuesMatrix);
					
					throw rsgis::RSGISException("The bandvalues matrix needs to have 2 columns (Wavelength, Width)");
				}
				
				// Read in reference library
				rsgis::math::Matrix *refDataMatrix = matrixUtils.readMatrixFromTxt(this->inMatrixSpecLibStr);
				
				// Convert reference library to cumulative area.
				rsgis::math::Matrix *cumAreaRefDataMatrix = mathsUtils.calculateCumulativeArea(refDataMatrix, bandsValuesMatrix);
				
				matrixUtils.printMatrix(cumAreaRefDataMatrix);
				
				calcImageValue = new rsgis::classifier::RSGISCumulativeAreaClassifierGenRules(cumAreaRefDataMatrix->m, bandsValuesMatrix, cumAreaRefDataMatrix);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcImageValue, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				
				GDALClose(datasets[0]);
				
				matrixUtils.freeMatrix(bandsValuesMatrix);
				matrixUtils.freeMatrix(refDataMatrix);
				matrixUtils.freeMatrix(cumAreaRefDataMatrix);
				
				delete calcImageValue;
				delete calcImage;
			}
			catch(rsgis::RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeClassification::cumulativeAreaClassify)
		{
			std::cout << "Generate cumulative area Classification\n";
			std::cout << "Input rule image: " << this->ruleImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Threshold = " << this->threshold << std::endl;
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::img::RSGISCalcImageValue *calcImageValue = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				datasets[0] = (GDALDataset *) GDALOpen(this->ruleImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->ruleImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				calcImageValue = new rsgis::classifier::RSGISCumulativeAreaClassifierDecide(1, threshold);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcImageValue, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				
				GDALClose(datasets[0]);
				
				delete calcImageValue;
				delete calcImage;
			}
			catch(rsgis::RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeClassification::kmeans)
		{
			std::cout << "Undertaking a KMeans classifcation/clustering\n";
			std::cout << "Input  image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Number of clusters = " << this->numClusters << std::endl;
			std::cout << "Distance Movement Threshold = " << this->clusterMoveThreshold << std::endl;
			std::cout << "Max. Number of Iterations = " << this->maxNumIterations << std::endl;
			if(this->savekmeansCentres){std::cout << "Saving centres to: " << this->outkmeansCentresFileName << std::endl;}
			if(initAlgor == randomInit)
			{
				std::cout << "The cluster centres will be initialised with random values\n";
			}
			else if(initAlgor == kppInit)
			{
				std::cout << "The KMeans++ algorithm will be used to initialised the cluster centres\n";
			}
			else
			{
				std::cout << "The initialisation algorithm has not been defined\n";
			}
			
			
			try
			{
				rsgis::classifier::RSGISKMeansClassifier *kMeansClassifier = new rsgis::classifier::RSGISKMeansClassifier(this->inputImage, printinfo);
				
				std::cout << "Initialise the KMeans classifier\n";
				if(initAlgor == randomInit)
				{
					kMeansClassifier->initClusterCentresRandom(this->numClusters);
				}
				else if(initAlgor == kppInit)
				{
					kMeansClassifier->initClusterCentresKpp(this->numClusters);
				}
				else
				{
					throw rsgis::RSGISException("The initialisation algorithm has not been defined");
				}
				std::cout << "Find cluster centres\n";
				kMeansClassifier->calcClusterCentres(this->clusterMoveThreshold, this->maxNumIterations, this->savekmeansCentres, this->outkmeansCentresFileName);
				std::cout << "Generate output Image\n";
				kMeansClassifier->generateOutputImage(this->outputImage);
				
				delete kMeansClassifier;
				
			}
			catch(rsgis::RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeClassification::isodata)
		{
			std::cout << "Undertaking a IOSDATA classifcation/clustering\n";
			std::cout << "Input  image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Number of clusters = " << this->numClusters << std::endl;
			std::cout << "Distance Movement Threshold = " << this->clusterMoveThreshold << std::endl;
			std::cout << "Max. Number of Iterations = " << this->maxNumIterations << std::endl;
			std::cout << "Minimum number of Values within a cluster = " << minNumVals << std::endl;
			std::cout << "Minimum distance between clusters = " << minDistanceBetweenCentres << std::endl;
			std::cout << "Maximum standard deviation within cluster = " << stddevThres << std::endl;
			std::cout << "Proportion over overall distance threshold = " << propOverAvgDist << std::endl;
			if(initAlgor == randomInit)
			{
				std::cout << "The cluster centres will be initialised with random values\n";
			}
			else if(initAlgor == kppInit)
			{
				std::cout << "The KMeans++ algorithm will be used to initialised the cluster centres\n";
			}
			else
			{
				std::cout << "The initialisation algorithm has not been defined\n";
			}
			
			
			try
			{
				rsgis::classifier::RSGISISODATAClassifier *isodataClassifier = new rsgis::classifier::RSGISISODATAClassifier(this->inputImage, printinfo);
				
				std::cout << "Initialise the ISOData classifier\n";
				if(initAlgor == randomInit)
				{
					isodataClassifier->initClusterCentresRandom(this->numClusters);
				}
				else if(initAlgor == kppInit)
				{
					isodataClassifier->initClusterCentresKpp(this->numClusters);
				}
				else
				{
					throw rsgis::RSGISException("The initialisation algorithm has not been defined");
				}
				std::cout << "Find cluster centres\n";
				isodataClassifier->calcClusterCentres(clusterMoveThreshold, maxNumIterations, minNumVals, minDistanceBetweenCentres, stddevThres, propOverAvgDist);//2000, 20, 20, 1.5
				std::cout << "Generate output Image\n";
				isodataClassifier->generateOutputImage(this->outputImage);
				
				delete isodataClassifier;
				
			}
			catch(rsgis::RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeClassification::createspeclib)
		{
			std::cout << "Input vector file: " << this->vector << std::endl;
			std::cout << "Output file: " << this->outputFile << std::endl;
			std::cout << "Class attribute: " << this->classAttribute << std::endl;
			std::cout << "Value Attributes: ";
			std::vector<std::string>::iterator iterAttributes;
			bool first = true;
			for(iterAttributes = valueAttributes->begin(); iterAttributes != valueAttributes->end(); ++iterAttributes)
			{
				if(first)
				{
					std::cout << "\'" << *iterAttributes << "\'";
					first = false;
				}
				else
				{
					std::cout << ", \'" << *iterAttributes << "\'";
				}
			}
			std::cout << std::endl;
			if(groupSamples)
			{
				std::cout << "Group samples into single class\n";
			}
			else
			{
				std::cout << "All samples will be a single class\n";
			}
			
			rsgis::classifier::RSGISClassificationUtils classificationUtils;
			classificationUtils.convertShapeFile2SpecLib(this->vector, this->outputFile, this->classAttribute, valueAttributes, groupSamples);
		}
        else if(option == RSGISExeClassification::addcolourtable)
        {
            std::cout << "Add a colour table to the image\n";
            std::cout << "Image: " << this->inputImage << std::endl;
            
            GDALAllRegister();
			GDALDataset *imgDataset = NULL;
            
			try
			{
				std::cout << "Reading in image " << this->inputImage << std::endl;
				imgDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
				if(imgDataset == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                GDALColorTable *clrTab = new GDALColorTable();
                
                for(std::vector<std::pair<int, rsgis::utils::RSGISColourInt> >::iterator iterClrs = classColourPairs.begin(); iterClrs != classColourPairs.end(); ++iterClrs)
                {
                    GDALColorEntry *clrEnt = new GDALColorEntry();
                    clrEnt->c1 = (*iterClrs).second.getRed();
                    clrEnt->c2 = (*iterClrs).second.getGreen();
                    clrEnt->c3 = (*iterClrs).second.getBlue();
                    clrEnt->c4 = (*iterClrs).second.getAlpha();
                    clrTab->SetColorEntry((*iterClrs).first, clrEnt);
				}
                imgDataset->GetRasterBand(imageBand)->SetColorTable(clrTab);
                imgDataset->GetRasterBand(imageBand)->SetMetadataItem("LAYER_TYPE", "thematic");
				
				GDALClose(imgDataset);
				GDALDestroyDriverManager();
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeClassification::elimsinglepxls)
        {
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            if(this->filterConnectivity == rsgis::img::rsgis_4connect)
            {
                std::cout << "4 Connectivity being used\n";
            }
            else if(this->filterConnectivity == rsgis::img::rsgis_8connect)
            {
                std::cout << "8 Connectivity being used\n";
            }
            else
            {
                std::cout << "Specified connectivity is not recognised.\n";
            }
            std::cout << "Output Image format: " << this->imageFormat << std::endl;
            if(this->ignoreZeros)
            {
                std::cout << "Ignoring 0 as a no data value\n";
            }
            
            try
            {
                rsgis::img::RSGISImageUtils imgUtils;
                
                GDALAllRegister();
                GDALDataset *imageDataset = NULL;
                imageDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
                if(imageDataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + this->inputImage;
                    throw rsgis::RSGISImageException(message.c_str());
                }
                
                // Create temporaly layer...
                GDALDataset *pixelMaskDataset = imgUtils.createCopy(imageDataset, 1, "", "MEM", GDT_Byte , true, "");
                
                std::cout << "Eliminating Individual Pixels from Classification\n";
                rsgis::classifier::RSGISEliminateSingleClassPixels eliminateSinglePxl;
                eliminateSinglePxl.eliminate(imageDataset, pixelMaskDataset, this->outputImage, 0, this->ignoreZeros, this->imageFormat, this->filterConnectivity);
                
                // Tidy up
                GDALClose(imageDataset);
                GDALClose(pixelMaskDataset);
                GDALDestroyDriverManager();
            } 
            catch (rsgis::RSGISException &e) 
            {
                throw e;
            }
            
        }
        else if(option == RSGISExeClassification::collapseclasses)
        {
            std::cout << "A command to collapse the segmentation classification to a just a classification\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Output Image format: " << this->imageFormat << std::endl;
            std::cout << "Class names column: " << this->classNameCol << std::endl;
            
            try
            {
                rsgis::cmds::executeCollapseRAT2Class(this->inputImage, this->outputImage, this->imageFormat, this->classNameCol);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            
        }
        else if(option == RSGISExeClassification::colour3bands)
        {
            std::cout << "Convert the colour table to a 3 band RGB image\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Output Image format: " << this->imageFormat << std::endl;
            
            try
            {
                rsgis::cmds::executeGenerate3BandFromColourTable(this->inputImage, this->outputImage, this->imageFormat);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            
        }
		else
		{
			std::cout << "RSGISExeClassification: Options not recognised\n";
		}
		
	}
}


void RSGISExeClassification::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeClassification::nn)
		{
			std::cout << "Nearest Neighbour Classification\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			for(int i = 0; i < numClasses; i++)
			{
				std::cout << "Class " << trainingData[i]->className << " has id " << trainingData[i]->classID << "\n";
			}
		}
		else if(option == RSGISExeClassification::vectorsql)
		{
			std::cout << "SQL Rulebased Classification of vector data\n";
			std::cout << "Vector Data: " << this->vector << std::endl;
			std::cout << "Class Name attribute: " << this->classAttribute << std::endl;
			for(int i = 0; i < numClasses; i++)
			{
				std::cout << "Class " << rules[i]->name << " SQL: " << rules[i]->sql << "\n";
			}
		}
		else if(option == RSGISExeClassification::samRule)
		{
			std::cout << "Generate SAM rule image\n";
		}
		else if(option == RSGISExeClassification::samClassify)
		{
			std::cout << "Generate SAM Classification\n";
		}
		else if(option == RSGISExeClassification::scmRule)
		{
			std::cout << "Generate SCM rule image\n";
		}
		else if(option == RSGISExeClassification::scmClassify)
		{
			std::cout << "Generate SCM Classification\n";
		}
		else if(option == RSGISExeClassification::cumulativeAreaRule)
		{
			std::cout << "Generate cumulative area rule image\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Band wavelength and widths: " << this->inMatrixfile << std::endl;
			std::cout << "Spectral library: " << this->inMatrixSpecLibStr << std::endl;
		}
		else if(option == RSGISExeClassification::cumulativeAreaClassify)
		{
			std::cout << "Generate cumulative area Classification\n";
			std::cout << "Input rule image: " << this->ruleImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Threshold = " << this->threshold << std::endl;
		}
		else if(option == RSGISExeClassification::kmeans)
		{
			std::cout << "Undertaking a KMeans classifcation/clustering\n";
			std::cout << "Input  image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Number of clusters = " << this->numClusters << std::endl;
			std::cout << "Distance Movement Threshold = " << this->clusterMoveThreshold << std::endl;
			std::cout << "Max. Number of Iterations = " << this->maxNumIterations << std::endl;
			if(initAlgor == randomInit)
			{
				std::cout << "The cluster centres will be initialised with random values\n";
			}
			else if(initAlgor == kppInit)
			{
				std::cout << "The KMeans++ algorithm will be used to initialised the cluster centres\n";
			}
			else
			{
				std::cout << "The initialisation algorithm has not been defined\n";
			}
		}
		else if(option == RSGISExeClassification::isodata)
		{
			std::cout << "Undertaking a ISOData classifcation/clustering\n";
			std::cout << "Input  image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Number of clusters = " << this->numClusters << std::endl;
			std::cout << "Distance Movement Threshold = " << this->clusterMoveThreshold << std::endl;
			std::cout << "Max. Number of Iterations = " << this->maxNumIterations << std::endl;
			std::cout << "Minimum number of Values within a cluster = " << minNumVals << std::endl;
			std::cout << "Minimum distance between clusters = " << minDistanceBetweenCentres << std::endl;
			std::cout << "Maximum standard deviation within cluster = " << stddevThres << std::endl;
			std::cout << "Proportion over overall distance threshold = " << propOverAvgDist << std::endl;
			if(initAlgor == randomInit)
			{
				std::cout << "The cluster centres will be initialised with random values\n";
			}
			else if(initAlgor == kppInit)
			{
				std::cout << "The KMeans++ algorithm will be used to initialised the cluster centres\n";
			}
			else
			{
				std::cout << "The initialisation algorithm has not been defined\n";
			}
		}
		else if(option == RSGISExeClassification::createspeclib)
		{
			std::cout << "Input vector file: " << this->vector << std::endl;
			std::cout << "Output file: " << this->outputFile << std::endl;
			std::cout << "Class attribute: " << this->classAttribute << std::endl;
			std::cout << "Value Attributes: ";
			std::vector<std::string>::iterator iterAttributes;
			bool first = true;
			for(iterAttributes = valueAttributes->begin(); iterAttributes != valueAttributes->end(); ++iterAttributes)
			{
				if(first)
				{
					std::cout << *iterAttributes;
					first = false;
				}
				else
				{
					std::cout << ", " << *iterAttributes;
				}
			}
			std::cout << std::endl;
			if(groupSamples)
			{
				std::cout << "Group samples into single class\n";
			}
			else
			{
				std::cout << "All samples will be a single class\n";
			}
		}
		else if(option == RSGISExeClassification::elimsinglepxls)
        {
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            if(this->filterConnectivity == rsgis::img::rsgis_4connect)
            {
                std::cout << "4 Connectivity being used\n";
            }
            else if(this->filterConnectivity == rsgis::img::rsgis_8connect)
            {
                std::cout << "8 Connectivity being used\n";
            }
            else
            {
                std::cout << "Specified connectivity is not recognised.\n";
            }
            std::cout << "Output Image format: " << this->imageFormat << std::endl;
            if(this->ignoreZeros)
            {
                std::cout << "Ignoring 0 as a no data value\n";
            }
        }
        else if(option == RSGISExeClassification::collapseclasses)
        {
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Output Image format: " << this->imageFormat << std::endl;
            std::cout << "Class names column: " << this->classNameCol << std::endl;
        }
        else if(option == RSGISExeClassification::colour3bands)
        {
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Output Image format: " << this->imageFormat << std::endl;
        }
		else
		{
			std::cout << "Options not recognised\n";
		}
	}
	else
	{
		std::cout << "The parameters have yet to be parsed\n";
	}
}

void RSGISExeClassification::help()
{
	std::cout << "<rsgis:commands>\n";
	std::cout << "\t<rsgis:command algor=\"classification\" option=\"nn\" image=\"image.env\" output=\"image_out.env\">\n";
	std::cout << "\t\t<rsgis:class name=\"1\" id=\"1\" matrix=\"matrix1.mtxt\" />\n";
	std::cout << "\t\t<rsgis:class name=\"2\" id=\"2\" matrix=\"matrix2.mtxt\" />\n";
	std::cout << "\t\t<rsgis:class name=\"3\" id=\"3\" matrix=\"matrix3.mtxt\" />\n";
	std::cout << "\t\t<rsgis:class name=\"4\" id=\"4\" matrix=\"matrix4.mtxt\" />\n";
	std::cout << "\t\t<rsgis:class name=\"5\" id=\"5\" matrix=\"matrix5.mtxt\" />\n";
	std::cout << "\t</rsgis:command>\n";
	std::cout << "\t<rsgis:command algor=\"classification\" option=\"vectorsql\" vector=\"vector.shp\" class=\"std::string\" >\n";
	std::cout << "\t\t</rsgis:class name=\"std::string\" sql=\"sql_statment\" />\n";
	std::cout << "\t\t</rsgis:class name=\"std::string\" sql=\"sql_statment\" />\n";
	std::cout << "\t\t</rsgis:class name=\"std::string\" sql=\"sql_statment\" />\n";
	std::cout << "\t\t</rsgis:class name=\"std::string\" sql=\"sql_statment\" />\n";
	std::cout << "\t\t</rsgis:class name=\"std::string\" sql=\"sql_statment\" />\n";
	std::cout << "\t</rsgis:command>\n";
	std::cout << "\t<rsgis:command algor=\"classification\" option=\"samRule\" image=\"image.env\" ruleImage=\"ruleImage.env\" \n";
	std::cout << "\t specLib=\"specLib\"/>\n";
	std::cout << "\t<rsgis:command algor=\"classification\" option=\"samClassify\"ruleImage=\"ruleImage.env\" \n";
	std::cout << "\t output=\"image_out.env\" threshold=\"threshold\"/>\n";
	std::cout << "\t<rsgis:command algor=\"classification\" option=\"scmRule\" image=\"image.env\" ruleImage=\"ruleImage.env\" \n";
	std::cout << "\t specLib=\"specLib\"/>\n";
	std::cout << "\t<rsgis:command algor=\"classification\" option=\"scmClassify\"ruleImage=\"ruleImage.env\" \n";
	std::cout << "\t output=\"image_out.env\" threshold=\"threshold\"/>\n";
	std::cout << "</rsgis:commands>\n";
}

std::string RSGISExeClassification::getDescription()
{
	return "Utilities for classification of imagery.";
}

std::string RSGISExeClassification::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeClassification::~RSGISExeClassification()
{
	
}

}
