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

RSGISAlgorithmParameters* RSGISExeClassification::getInstance()
{
	return new RSGISExeClassification();
}

void RSGISExeClassification::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;
	RSGISMatrices matrixUtils;
	RSGISVectors vectorUtils;
	
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *classXMLStr = XMLString::transcode("rsgis:class");
	XMLCh *optionNN = XMLString::transcode("nn");
	XMLCh *optionVectorSQL = XMLString::transcode("vectorsql");
	XMLCh *optionSAMRule = XMLString::transcode("samRule");
	XMLCh *optionSAMClassify = XMLString::transcode("samClassify");
	XMLCh *optionSCMRule = XMLString::transcode("scmRule");
	XMLCh *optionSCMClassify = XMLString::transcode("scmClassify");
	XMLCh *optionCumulativeAreaRule = XMLString::transcode("cumulativeAreaRule");
	XMLCh *optionCumulativeAreaClassify = XMLString::transcode("cumulativeAreaClassify");
	XMLCh *optionKMeans = XMLString::transcode("kmeans");
	XMLCh *optionISOData = XMLString::transcode("isodata");
	XMLCh *optionCreateSpecLib = XMLString::transcode("createspeclib");
	
	const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
	if(XMLString::equals(optionNN, optionXML))
	{		
		this->option = RSGISExeClassification::nn;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		
		DOMNodeList *classNodesList = argElement->getElementsByTagName(classXMLStr);
		this->numClasses = classNodesList->getLength();
		
		cout << "Found " << this->numClasses << " classes" << endl;
		
		trainingData = new ClassData*[numClasses];
		DOMElement *classElement = NULL;
		for(int i = 0; i < numClasses; i++)
		{
			trainingData[i] = new ClassData();
			classElement = static_cast<DOMElement*>(classNodesList->item(i));
			
			XMLCh *nameXMLStr = XMLString::transcode("name");
			if(classElement->hasAttribute(nameXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(nameXMLStr));
				trainingData[i]->className = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			XMLString::release(&nameXMLStr);
			
			XMLCh *idXMLStr = XMLString::transcode("id");
			if(classElement->hasAttribute(idXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(idXMLStr));
				trainingData[i]->classID = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'id\' attribute was provided.");
			}
			XMLString::release(&idXMLStr);
			
			XMLCh *matrixXMLStr = XMLString::transcode("matrix");
			if(classElement->hasAttribute(matrixXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(matrixXMLStr));
				trainingData[i]->data = matrixUtils.readMatrixFromTxt(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'matrix\' attribute was provided.");
			}
			XMLString::release(&matrixXMLStr);
		}
		
	}
	else if(XMLString::equals(optionVectorSQL, optionXML))
	{
		this->option = RSGISExeClassification::vectorsql;
		
		XMLCh *vectorXMLStr = XMLString::transcode("vector");
		if(argElement->hasAttribute(vectorXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
			this->vector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'vector\' attribute was provided.");
		}
		XMLString::release(&vectorXMLStr);
		
		
		XMLCh *classAttributeXMLStr = XMLString::transcode("class");
		if(argElement->hasAttribute(classAttributeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(classAttributeXMLStr));
			this->classAttribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'class\' attribute was provided.");
		}
		XMLString::release(&classAttributeXMLStr);
		
		DOMNodeList *classNodesList = argElement->getElementsByTagName(classXMLStr);
		this->numClasses = classNodesList->getLength();
		
		cout << "Found " << this->numClasses << " classes" << endl;
		
		rules = new sqlclass*[numClasses];
		DOMElement *classElement = NULL;
		for(int i = 0; i < numClasses; i++)
		{
			rules[i] = new sqlclass();
			classElement = static_cast<DOMElement*>(classNodesList->item(i));
			
			XMLCh *nameXMLStr = XMLString::transcode("name");
			if(classElement->hasAttribute(nameXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(nameXMLStr));
				rules[i]->name = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			XMLString::release(&nameXMLStr);
			
			
			XMLCh *sqlXMLStr = XMLString::transcode("sql");
			if(classElement->hasAttribute(sqlXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(sqlXMLStr));
				rules[i]->sql = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'sql\' attribute was provided.");
			}
			XMLString::release(&sqlXMLStr);
		}
	}
	else if(XMLString::equals(optionSAMRule, optionXML))
	{
		this->option = RSGISExeClassification::samRule;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *ruleImageXMLStr = XMLString::transcode("ruleImage");
		if(argElement->hasAttribute(ruleImageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(ruleImageXMLStr));
			this->ruleImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'rule\' image was provided.");
		}
		XMLString::release(&ruleImageXMLStr);
		
		XMLCh *specLibXMLStr = XMLString::transcode("specLib");
		if(argElement->hasAttribute(specLibXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(specLibXMLStr));
			RSGISMatrices matrixUtils;
			string specLibStr = string(charValue);
			this->specLib = matrixUtils.readGSLMatrixFromGridTxt(specLibStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'specLib\' spectral library file provided");
		}
		XMLString::release(&specLibXMLStr);
		
	}
	else if(XMLString::equals(optionSAMClassify, optionXML))
	{
		this->option = RSGISExeClassification::samClassify;
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *ruleImageXMLStr = XMLString::transcode("ruleImage");
		if(argElement->hasAttribute(ruleImageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(ruleImageXMLStr));
			this->ruleImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'rule\' image was provided.");
		}
		XMLString::release(&ruleImageXMLStr);
		
		XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			string thresholdStr = string(charValue);
			this->threshold = mathUtils.strtodouble(thresholdStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for threshold");
		}
		XMLString::release(&thresholdXMLStr);
	}
	else if(XMLString::equals(optionSCMRule, optionXML))
	{
		this->option = RSGISExeClassification::scmRule;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *ruleImageXMLStr = XMLString::transcode("ruleImage");
		if(argElement->hasAttribute(ruleImageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(ruleImageXMLStr));
			this->ruleImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'rule\' image was provided.");
		}
		XMLString::release(&ruleImageXMLStr);
		
		XMLCh *specLibXMLStr = XMLString::transcode("specLib");
		if(argElement->hasAttribute(specLibXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(specLibXMLStr));
			RSGISMatrices matrixUtils;
			string specLibStr = string(charValue);
			this->specLib = matrixUtils.readGSLMatrixFromGridTxt(specLibStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'specLib\' spectral library file provided");
		}
		XMLString::release(&specLibXMLStr);
		
	}
	else if(XMLString::equals(optionSCMClassify, optionXML))
	{
		this->option = RSGISExeClassification::scmClassify;
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *ruleImageXMLStr = XMLString::transcode("ruleImage");
		if(argElement->hasAttribute(ruleImageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(ruleImageXMLStr));
			this->ruleImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'rule\' image was provided.");
		}
		XMLString::release(&ruleImageXMLStr);
		
		XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			string thresholdStr = string(charValue);
			this->threshold = mathUtils.strtodouble(thresholdStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for threshold");
		}
		XMLString::release(&thresholdXMLStr);
	}	
	else if(XMLString::equals(optionCumulativeAreaRule, optionXML))
	{		
		this->option = RSGISExeClassification::cumulativeAreaRule;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *imageBandsXMLStr = XMLString::transcode("imagebands");
		if(argElement->hasAttribute(imageBandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageBandsXMLStr));
			this->inMatrixfile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'imagebands\' attribute was provided.");
		}
		XMLString::release(&imageBandsXMLStr);
		
		XMLCh *specLibXMLStr = XMLString::transcode("specLib");
		if(argElement->hasAttribute(specLibXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(specLibXMLStr));
			inMatrixSpecLibStr = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'specLib\' spectral library file provided");
		}
		XMLString::release(&specLibXMLStr);
		
	}
	else if(XMLString::equals(optionCumulativeAreaClassify, optionXML))
	{
		this->option = RSGISExeClassification::cumulativeAreaClassify;
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *ruleImageXMLStr = XMLString::transcode("ruleImage");
		if(argElement->hasAttribute(ruleImageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(ruleImageXMLStr));
			this->ruleImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'rule\' image was provided.");
		}
		XMLString::release(&ruleImageXMLStr);
		
		XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			string thresholdStr = string(charValue);
			this->threshold = mathUtils.strtodouble(thresholdStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for threshold");
		}
		XMLString::release(&thresholdXMLStr);
	}
	else if(XMLString::equals(optionKMeans, optionXML))
	{
		this->option = RSGISExeClassification::kmeans;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);		
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *numClustersXMLStr = XMLString::transcode("numclusters");
		if(argElement->hasAttribute(numClustersXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numClustersXMLStr));
			string strVal = string(charValue);
			this->numClusters = mathUtils.strtoint(strVal);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for numclusters");
		}
		XMLString::release(&numClustersXMLStr);
		
		
		XMLCh *initAlgorXMLStr = XMLString::transcode("initalgor");
		if(argElement->hasAttribute(initAlgorXMLStr))
		{
			XMLCh *ranStr = XMLString::transcode("random");
			XMLCh *kppStr = XMLString::transcode("kpp");
			
			const XMLCh *initAlgorValue = argElement->getAttribute(initAlgorXMLStr);
			
			if(XMLString::equals(initAlgorValue, ranStr))
			{
				this->initAlgor = randomInit;
			}
			else if(XMLString::equals(initAlgorValue, kppStr))
			{
				this->initAlgor = kppInit;
			}
			else
			{
				this->initAlgor = undefinedInit;
				throw RSGISXMLArgumentsException("\'initalgor\' value not recognised.");
			}
			XMLString::release(&ranStr);
			XMLString::release(&kppStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'initalgor\' attribute was provided.");
		}
		XMLString::release(&initAlgorXMLStr);
		
		
		XMLCh *maxIterationsXMLStr = XMLString::transcode("maxiterations");
		if(argElement->hasAttribute(maxIterationsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
			string strVal = string(charValue);
			this->maxNumIterations = mathUtils.strtoint(strVal);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for \'maxiterations\'");
		}
		XMLString::release(&maxIterationsXMLStr);
		
		XMLCh *clusterMoveXMLStr = XMLString::transcode("clustermove");
		if(argElement->hasAttribute(clusterMoveXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clusterMoveXMLStr));
			string strVal = string(charValue);
			this->clusterMoveThreshold = mathUtils.strtofloat(strVal);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for \'clustermove\'");
		}
		XMLString::release(&clusterMoveXMLStr);
		
		XMLCh *printInfoXMLStr = XMLString::transcode("printinfo");
		if(argElement->hasAttribute(printInfoXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(printInfoXMLStr);
			
			if(XMLString::equals(forceValue, yesStr))
			{
				this->printinfo = true;
			}
			else
			{
				this->printinfo = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'printinfo\' attribute was provided.");
		}
		XMLString::release(&printInfoXMLStr);
		
		
	}
	else if(XMLString::equals(optionISOData, optionXML))
	{
		this->option = RSGISExeClassification::isodata;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);		
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *numClustersXMLStr = XMLString::transcode("numclusters");
		if(argElement->hasAttribute(numClustersXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numClustersXMLStr));
			string strVal = string(charValue);
			this->numClusters = mathUtils.strtoint(strVal);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for numclusters");
		}
		XMLString::release(&numClustersXMLStr);
		
		
		XMLCh *initAlgorXMLStr = XMLString::transcode("initalgor");
		if(argElement->hasAttribute(initAlgorXMLStr))
		{
			XMLCh *ranStr = XMLString::transcode("random");
			XMLCh *kppStr = XMLString::transcode("kpp");
			
			const XMLCh *initAlgorValue = argElement->getAttribute(initAlgorXMLStr);
			
			if(XMLString::equals(initAlgorValue, ranStr))
			{
				this->initAlgor = randomInit;
			}
			else if(XMLString::equals(initAlgorValue, kppStr))
			{
				this->initAlgor = kppInit;
			}
			else
			{
				this->initAlgor = undefinedInit;
				throw RSGISXMLArgumentsException("\'initalgor\' value not recognised.");
			}
			XMLString::release(&ranStr);
			XMLString::release(&kppStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'initalgor\' attribute was provided.");
		}
		XMLString::release(&initAlgorXMLStr);
		
		
		XMLCh *maxIterationsXMLStr = XMLString::transcode("maxiterations");
		if(argElement->hasAttribute(maxIterationsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
			string strVal = string(charValue);
			this->maxNumIterations = mathUtils.strtoint(strVal);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for \'maxiterations\'");
		}
		XMLString::release(&maxIterationsXMLStr);
		
		XMLCh *clusterMoveXMLStr = XMLString::transcode("clustermove");
		if(argElement->hasAttribute(clusterMoveXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clusterMoveXMLStr));
			string strVal = string(charValue);
			this->clusterMoveThreshold = mathUtils.strtofloat(strVal);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for \'clustermove\'");
		}
		XMLString::release(&clusterMoveXMLStr);
		
		
		XMLCh *minNumberValsXMLStr = XMLString::transcode("minnumbervals");
		if(argElement->hasAttribute(minNumberValsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minNumberValsXMLStr));
			string strVal = string(charValue);
			this->minNumVals = mathUtils.strtoint(strVal);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for \'minnumbervals\'");
		}
		XMLString::release(&minNumberValsXMLStr);
		
		XMLCh *minDistanceBetweenCentresXMLStr = XMLString::transcode("mindistbetweencluster");
		if(argElement->hasAttribute(minDistanceBetweenCentresXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minDistanceBetweenCentresXMLStr));
			string strVal = string(charValue);
			this->minDistanceBetweenCentres = mathUtils.strtofloat(strVal);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for \'mindistbetweencluster\'");
		}
		XMLString::release(&minDistanceBetweenCentresXMLStr);
		
		
		XMLCh *stddevThresXMLStr = XMLString::transcode("maxstddev");
		if(argElement->hasAttribute(stddevThresXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(stddevThresXMLStr));
			string strVal = string(charValue);
			this->stddevThres = mathUtils.strtofloat(strVal);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for \'maxstddev\'");
		}
		XMLString::release(&stddevThresXMLStr);
		
		
		XMLCh *propOverAvgDistXMLStr = XMLString::transcode("propAvgDist");
		if(argElement->hasAttribute(propOverAvgDistXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(propOverAvgDistXMLStr));
			string strVal = string(charValue);
			this->propOverAvgDist = mathUtils.strtofloat(strVal);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No value provided for \'propAvgDist\'");
		}
		XMLString::release(&propOverAvgDistXMLStr);
		
		XMLCh *printInfoXMLStr = XMLString::transcode("printinfo");
		if(argElement->hasAttribute(printInfoXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(printInfoXMLStr);
			
			if(XMLString::equals(forceValue, yesStr))
			{
				this->printinfo = true;
			}
			else
			{
				this->printinfo = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'printinfo\' attribute was provided.");
		}
		XMLString::release(&printInfoXMLStr);
		
	}
	else if(XMLString::equals(optionCreateSpecLib, optionXML))
	{
		this->option = RSGISExeClassification::createspeclib;
		
		XMLCh *vectorXMLStr = XMLString::transcode("vector");
		if(argElement->hasAttribute(vectorXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
			this->vector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'vector\' attribute was provided.");
		}
		XMLString::release(&vectorXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' image was provided.");
		}
		XMLString::release(&outputXMLStr);
				
		XMLCh *numClustersXMLStr = XMLString::transcode("classattribute");
		if(argElement->hasAttribute(numClustersXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numClustersXMLStr));
			this->classAttribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No classattribute provided for numclusters");
		}
		XMLString::release(&numClustersXMLStr);
		
		XMLCh *valueAttributesXMLStr = XMLString::transcode("valueattributes");
		if(argElement->hasAttribute(valueAttributesXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(valueAttributesXMLStr));
			string attributes = string(charValue);
			XMLString::release(&charValue);
			
			valueAttributes = new std::vector<std::string>();
			
			RSGISTextUtils textUtils;
			textUtils.tokenizeString(attributes, ',', valueAttributes, true);
		}
		else
		{
			throw RSGISXMLArgumentsException("No valueattributes provided for numclusters");
		}
		XMLString::release(&valueAttributesXMLStr);		
		
		XMLCh *groupXMLStr = XMLString::transcode("group");
		if(argElement->hasAttribute(groupXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *groupValue = argElement->getAttribute(groupXMLStr);
			
			if(XMLString::equals(groupValue, yesStr))
			{
				this->groupSamples = true;
			}
			else
			{
				this->groupSamples = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'group\' attribute was provided.");
		}
		XMLString::release(&groupXMLStr);
		
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeClassification.");
		throw RSGISXMLArgumentsException(message.c_str());
	}
	
	XMLString::release(&algorName);
	XMLString::release(&algorXMLStr);
	XMLString::release(&optionXMLStr);
	XMLString::release(&optionNN);
	XMLString::release(&optionVectorSQL);
	XMLString::release(&optionSAMRule);
	XMLString::release(&optionSAMClassify);
	XMLString::release(&optionSCMRule);
	XMLString::release(&optionSCMClassify);
	XMLString::release(&classXMLStr);
	XMLString::release(&optionCumulativeAreaRule);
	XMLString::release(&optionCumulativeAreaClassify);
	XMLString::release(&optionKMeans);
	XMLString::release(&optionISOData);
	XMLString::release(&optionCreateSpecLib);
	parsed = true;
}

void RSGISExeClassification::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(option == RSGISExeClassification::nn)
		{
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			RSGISNearestNeighbourClassifier *nnClassifier = NULL;
			RSGISApplyClassifier *applyClassifier = NULL;
			
			try
			{				
				datasets = new GDALDataset*[1];
				cout << "Reading in image " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				nnClassifier = new RSGISNearestNeighbourClassifier(this->trainingData, this->numClasses);
				nnClassifier->printClassIDs();
				applyClassifier = new RSGISApplyClassifier(1, nnClassifier);
				calcImage = new rsgis::img::RSGISCalcImage(applyClassifier, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				GDALClose(datasets[0]);
				GDALDestroyDriverManager();
				
				delete nnClassifier;
				delete applyClassifier;
				delete calcImage;
			}
			catch(RSGISException e)
			{
				throw e;
			}			
		}
		else if(option == RSGISExeClassification::vectorsql)
		{
			OGRRegisterAll();
			cout << "SQL Rulebased Classification of vector data\n";
			cout << "Vector Data: " << this->vector << endl;
			cout << "Class Name attribute: " << this->classAttribute << endl;
			
			RSGISVectorUtils vecUtils;
			RSGISVectorSQLClassification vecSQLClass;
			
			string SHPFileInLayer = vecUtils.getLayerName(this->vector);
			
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			
			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = OGRSFDriverRegistrar::Open(this->vector.c_str(), TRUE, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->vector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				
				string sqlprefix = string("SELECT * FROM ") + SHPFileInLayer + string(" WHERE ");
				for(int i = 0; i < numClasses; i++)
				{
					rules[i]->sql = sqlprefix + rules[i]->sql;
				}
				vecSQLClass.classifyVector(inputSHPDS, inputSHPLayer, this->rules, this->numClasses, this->classAttribute);
				
				OGRDataSource::DestroyDataSource(inputSHPDS);
				OGRCleanupAll();
			}
			catch (RSGISException e) 
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
			RSGISSpectralAngleMapperRule *samRule = NULL;
			int numOutputBands = specLib->size2;
			
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
			catch(RSGISImageException& e)
			{
				throw e;
			}
			
			try
			{
				cout << "Producing Rule image" <<endl;
				samRule = new RSGISSpectralAngleMapperRule(numOutputBands, specLib);
				calcImg = new rsgis::img::RSGISCalcImage(samRule, "", true);
				calcImg->calcImage(datasets, 1, this->ruleImage);
			}
			catch(RSGISException& e)
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
			RSGISSpectralAngleMapperClassifier *samClassify = NULL;

			try
			{
				datasets = new GDALDataset*[1];
				cout << this->ruleImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->ruleImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->ruleImage;
					throw RSGISImageException(message.c_str());
				}
			}
			catch(RSGISImageException& e)
			{
				throw e;
			}
			
			try
			{
				cout << "Producing classification" <<endl;
				samClassify = new RSGISSpectralAngleMapperClassifier(1, this->threshold);
				calcImg = new rsgis::img::RSGISCalcImage(samClassify, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
			}
			catch(RSGISException& e)
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
			RSGISSpectralCorrelationMapperRule *scmRule = NULL;
			int numOutputBands = specLib->size2;
			
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
			catch(RSGISImageException& e)
			{
				throw e;
			}
			
			try
			{
				cout << "Producing Rule image" <<endl;
				scmRule = new RSGISSpectralCorrelationMapperRule(numOutputBands, specLib);
				calcImg = new rsgis::img::RSGISCalcImage(scmRule, "", true);
				calcImg->calcImage(datasets, 1, this->ruleImage);
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
			delete scmRule;
			
		}
		else if(option == RSGISExeClassification::scmClassify)
		{
			// Produce classification
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			rsgis::img::RSGISCalcImage *calcImg = NULL;
			RSGISSpectralCorrelationMapperClassifier *scmClassify = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				cout << this->ruleImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->ruleImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->ruleImage;
					throw RSGISImageException(message.c_str());
				}
			}
			catch(RSGISImageException& e)
			{
				throw e;
			}
			
			try
			{
				cout << "Producing classification" <<endl;
				scmClassify = new RSGISSpectralCorrelationMapperClassifier(1, this->threshold);
				calcImg = new rsgis::img::RSGISCalcImage(scmClassify, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
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
			delete scmClassify;
			
			
		}
		else if(option == RSGISExeClassification::cumulativeAreaRule)
		{
			cout << "Generate cumulative area rule image\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Band wavelength and widths: " << this->inMatrixfile << endl;
			cout << "Spectral library: " << this->inMatrixSpecLibStr << endl;
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::img::RSGISCalcImageValue *calcImageValue = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			RSGISMatrices matrixUtils;
			RSGISMathsUtils mathsUtils;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				int numImgBands = datasets[0]->GetRasterCount();
				
				Matrix *bandsValuesMatrix = matrixUtils.readMatrixFromTxt(this->inMatrixfile);
				
				if(bandsValuesMatrix->n != numImgBands)
				{
					GDALClose(datasets[0]);
					matrixUtils.freeMatrix(bandsValuesMatrix);
					
					throw RSGISException("The bandvalues matrix needs to have the same number of rows as the input image has bands");
				}
				
				if(bandsValuesMatrix->m != 2)
				{
					GDALClose(datasets[0]);
					matrixUtils.freeMatrix(bandsValuesMatrix);
					
					throw RSGISException("The bandvalues matrix needs to have 2 columns (Wavelength, Width)");
				}
				
				// Read in reference library
				Matrix *refDataMatrix = matrixUtils.readMatrixFromTxt(this->inMatrixSpecLibStr);
				
				// Convert reference library to cumulative area.
				Matrix *cumAreaRefDataMatrix = mathsUtils.calculateCumulativeArea(refDataMatrix, bandsValuesMatrix);
				
				matrixUtils.printMatrix(cumAreaRefDataMatrix);
				
				calcImageValue = new RSGISCumulativeAreaClassifierGenRules(cumAreaRefDataMatrix->m, bandsValuesMatrix, cumAreaRefDataMatrix);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcImageValue, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				
				GDALClose(datasets[0]);
				
				matrixUtils.freeMatrix(bandsValuesMatrix);
				matrixUtils.freeMatrix(refDataMatrix);
				matrixUtils.freeMatrix(cumAreaRefDataMatrix);
				
				delete calcImageValue;
				delete calcImage;
			}
			catch(RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeClassification::cumulativeAreaClassify)
		{
			cout << "Generate cumulative area Classification\n";
			cout << "Input rule image: " << this->ruleImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Threshold = " << this->threshold << endl;
			
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
					string message = string("Could not open image ") + this->ruleImage;
					throw RSGISImageException(message.c_str());
				}
				
				calcImageValue = new RSGISCumulativeAreaClassifierDecide(1, threshold);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcImageValue, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				
				GDALClose(datasets[0]);
				
				delete calcImageValue;
				delete calcImage;
			}
			catch(RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeClassification::kmeans)
		{
			cout << "Undertaking a KMeans classifcation/clustering\n";
			cout << "Input  image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Number of clusters = " << this->numClusters << endl;
			cout << "Distance Movement Threshold = " << this->clusterMoveThreshold << endl;
			cout << "Max. Number of Iterations = " << this->maxNumIterations << endl;
			if(initAlgor == randomInit)
			{
				cout << "The cluster centres will be initialised with random values\n";
			}
			else if(initAlgor == kppInit)
			{
				cout << "The KMeans++ algorithm will be used to initialised the cluster centres\n";
			}
			else
			{
				cout << "The initialisation algorithm has not been defined\n";
			}
			
			
			try
			{
				RSGISKMeansClassifier *kMeansClassifier = new RSGISKMeansClassifier(this->inputImage, printinfo);
				
				cout << "Initialise the KMeans classifier\n";
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
					throw RSGISException("The initialisation algorithm has not been defined");
				}
				cout << "Find cluster centres\n";
				kMeansClassifier->calcClusterCentres(clusterMoveThreshold, maxNumIterations);
				cout << "Generate output Image\n";
				kMeansClassifier->generateOutputImage(this->outputImage);
				
				delete kMeansClassifier;
				
			}
			catch(RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeClassification::isodata)
		{
			cout << "Undertaking a IOSDATA classifcation/clustering\n";
			cout << "Input  image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Number of clusters = " << this->numClusters << endl;
			cout << "Distance Movement Threshold = " << this->clusterMoveThreshold << endl;
			cout << "Max. Number of Iterations = " << this->maxNumIterations << endl;
			cout << "Minimum number of Values within a cluster = " << minNumVals << endl;
			cout << "Minimum distance between clusters = " << minDistanceBetweenCentres << endl;
			cout << "Maximum standard deviation within cluster = " << stddevThres << endl;
			cout << "Proportion over overall distance threshold = " << propOverAvgDist << endl;
			if(initAlgor == randomInit)
			{
				cout << "The cluster centres will be initialised with random values\n";
			}
			else if(initAlgor == kppInit)
			{
				cout << "The KMeans++ algorithm will be used to initialised the cluster centres\n";
			}
			else
			{
				cout << "The initialisation algorithm has not been defined\n";
			}
			
			
			try
			{
				RSGISISODATAClassifier *isodataClassifier = new RSGISISODATAClassifier(this->inputImage, printinfo);
				
				cout << "Initialise the ISOData classifier\n";
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
					throw RSGISException("The initialisation algorithm has not been defined");
				}
				cout << "Find cluster centres\n";
				isodataClassifier->calcClusterCentres(clusterMoveThreshold, maxNumIterations, minNumVals, minDistanceBetweenCentres, stddevThres, propOverAvgDist);//2000, 20, 20, 1.5
				cout << "Generate output Image\n";
				isodataClassifier->generateOutputImage(this->outputImage);
				
				delete isodataClassifier;
				
			}
			catch(RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeClassification::createspeclib)
		{
			cout << "Input vector file: " << this->vector << endl;
			cout << "Output file: " << this->outputFile << endl;
			cout << "Class attribute: " << this->classAttribute << endl;
			cout << "Value Attributes: ";
			std::vector<std::string>::iterator iterAttributes;
			bool first = true;
			for(iterAttributes = valueAttributes->begin(); iterAttributes != valueAttributes->end(); ++iterAttributes)
			{
				if(first)
				{
					cout << "\'" << *iterAttributes << "\'";
					first = false;
				}
				else
				{
					cout << ", \'" << *iterAttributes << "\'";
				}
			}
			cout << endl;
			if(groupSamples)
			{
				cout << "Group samples into single class\n";
			}
			else
			{
				cout << "All samples will be a single class\n";
			}
			
			RSGISClassificationUtils classificationUtils;
			classificationUtils.convertShapeFile2SpecLib(this->vector, this->outputFile, this->classAttribute, valueAttributes, groupSamples);
		}
		else
		{
			cout << "RSGISExeClassification: Options not recognised\n";
		}
		
	}
}


void RSGISExeClassification::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeClassification::nn)
		{
			cout << "Nearest Neighbour Classification\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			for(int i = 0; i < numClasses; i++)
			{
				cout << "Class " << trainingData[i]->className << " has id " << trainingData[i]->classID << "\n";
			}
		}
		else if(option == RSGISExeClassification::vectorsql)
		{
			cout << "SQL Rulebased Classification of vector data\n";
			cout << "Vector Data: " << this->vector << endl;
			cout << "Class Name attribute: " << this->classAttribute << endl;
			for(int i = 0; i < numClasses; i++)
			{
				cout << "Class " << rules[i]->name << " SQL: " << rules[i]->sql << "\n";
			}
		}
		else if(option == RSGISExeClassification::samRule)
		{
			cout << "Generate SAM rule image\n";
		}
		else if(option == RSGISExeClassification::samClassify)
		{
			cout << "Generate SAM Classification\n";
		}
		else if(option == RSGISExeClassification::scmRule)
		{
			cout << "Generate SCM rule image\n";
		}
		else if(option == RSGISExeClassification::scmClassify)
		{
			cout << "Generate SCM Classification\n";
		}
		else if(option == RSGISExeClassification::cumulativeAreaRule)
		{
			cout << "Generate cumulative area rule image\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Band wavelength and widths: " << this->inMatrixfile << endl;
			cout << "Spectral library: " << this->inMatrixSpecLibStr << endl;
		}
		else if(option == RSGISExeClassification::cumulativeAreaClassify)
		{
			cout << "Generate cumulative area Classification\n";
			cout << "Input rule image: " << this->ruleImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Threshold = " << this->threshold << endl;
		}
		else if(option == RSGISExeClassification::kmeans)
		{
			cout << "Undertaking a KMeans classifcation/clustering\n";
			cout << "Input  image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Number of clusters = " << this->numClusters << endl;
			cout << "Distance Movement Threshold = " << this->clusterMoveThreshold << endl;
			cout << "Max. Number of Iterations = " << this->maxNumIterations << endl;
			if(initAlgor == randomInit)
			{
				cout << "The cluster centres will be initialised with random values\n";
			}
			else if(initAlgor == kppInit)
			{
				cout << "The KMeans++ algorithm will be used to initialised the cluster centres\n";
			}
			else
			{
				cout << "The initialisation algorithm has not been defined\n";
			}
		}
		else if(option == RSGISExeClassification::isodata)
		{
			cout << "Undertaking a ISOData classifcation/clustering\n";
			cout << "Input  image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Number of clusters = " << this->numClusters << endl;
			cout << "Distance Movement Threshold = " << this->clusterMoveThreshold << endl;
			cout << "Max. Number of Iterations = " << this->maxNumIterations << endl;
			cout << "Minimum number of Values within a cluster = " << minNumVals << endl;
			cout << "Minimum distance between clusters = " << minDistanceBetweenCentres << endl;
			cout << "Maximum standard deviation within cluster = " << stddevThres << endl;
			cout << "Proportion over overall distance threshold = " << propOverAvgDist << endl;
			if(initAlgor == randomInit)
			{
				cout << "The cluster centres will be initialised with random values\n";
			}
			else if(initAlgor == kppInit)
			{
				cout << "The KMeans++ algorithm will be used to initialised the cluster centres\n";
			}
			else
			{
				cout << "The initialisation algorithm has not been defined\n";
			}
		}
		else if(option == RSGISExeClassification::createspeclib)
		{
			cout << "Input vector file: " << this->vector << endl;
			cout << "Output file: " << this->outputFile << endl;
			cout << "Class attribute: " << this->classAttribute << endl;
			cout << "Value Attributes: ";
			std::vector<std::string>::iterator iterAttributes;
			bool first = true;
			for(iterAttributes = valueAttributes->begin(); iterAttributes != valueAttributes->end(); ++iterAttributes)
			{
				if(first)
				{
					cout << *iterAttributes;
					first = false;
				}
				else
				{
					cout << ", " << *iterAttributes;
				}
			}
			cout << endl;
			if(groupSamples)
			{
				cout << "Group samples into single class\n";
			}
			else
			{
				cout << "All samples will be a single class\n";
			}
		}
		
		else
		{
			cout << "Options not recognised\n";
		}
	}
	else
	{
		cout << "The parameters have yet to be parsed\n";
	}
}

void RSGISExeClassification::help()
{
	cout << "<rsgis:commands>\n";
	cout << "\t<rsgis:command algor=\"classification\" option=\"nn\" image=\"image.env\" output=\"image_out.env\">\n";
	cout << "\t\t<rsgis:class name=\"1\" id=\"1\" matrix=\"matrix1.mtxt\" />\n";
	cout << "\t\t<rsgis:class name=\"2\" id=\"2\" matrix=\"matrix2.mtxt\" />\n";
	cout << "\t\t<rsgis:class name=\"3\" id=\"3\" matrix=\"matrix3.mtxt\" />\n";
	cout << "\t\t<rsgis:class name=\"4\" id=\"4\" matrix=\"matrix4.mtxt\" />\n";
	cout << "\t\t<rsgis:class name=\"5\" id=\"5\" matrix=\"matrix5.mtxt\" />\n";
	cout << "\t</rsgis:command>\n";
	cout << "\t<rsgis:command algor=\"classification\" option=\"vectorsql\" vector=\"vector.shp\" class=\"string\" >\n";
	cout << "\t\t</rsgis:class name=\"string\" sql=\"sql_statment\" />\n";
	cout << "\t\t</rsgis:class name=\"string\" sql=\"sql_statment\" />\n";
	cout << "\t\t</rsgis:class name=\"string\" sql=\"sql_statment\" />\n";
	cout << "\t\t</rsgis:class name=\"string\" sql=\"sql_statment\" />\n";
	cout << "\t\t</rsgis:class name=\"string\" sql=\"sql_statment\" />\n";
	cout << "\t</rsgis:command>\n";
	cout << "\t<rsgis:command algor=\"classification\" option=\"samRule\" image=\"image.env\" ruleImage=\"ruleImage.env\" \n";
	cout << "\t specLib=\"specLib\"/>\n";
	cout << "\t<rsgis:command algor=\"classification\" option=\"samClassify\"ruleImage=\"ruleImage.env\" \n";
	cout << "\t output=\"image_out.env\" threshold=\"threshold\"/>\n";
	cout << "\t<rsgis:command algor=\"classification\" option=\"scmRule\" image=\"image.env\" ruleImage=\"ruleImage.env\" \n";
	cout << "\t specLib=\"specLib\"/>\n";
	cout << "\t<rsgis:command algor=\"classification\" option=\"scmClassify\"ruleImage=\"ruleImage.env\" \n";
	cout << "\t output=\"image_out.env\" threshold=\"threshold\"/>\n";
	cout << "</rsgis:commands>\n";
}

string RSGISExeClassification::getDescription()
{
	return "Utilities for classification of imagery.";
}

string RSGISExeClassification::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeClassification::~RSGISExeClassification()
{
	
}

}
