/*
 *  RSGISExeImageCalculation.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/12/2008.
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

#include "RSGISExeImageCalculation.h"


RSGISExeImageCalculation::RSGISExeImageCalculation() : RSGISAlgorithmParameters()
{
	this->algorithm = "imagecalc";
	this->option = RSGISExeImageCalculation::none;
	this->inputImage = "";
	this->inputImages = NULL;
	this->outputImage = "";
	this->outputImages = NULL;
	this->inputImageA = "";
	this->inputImageB = "";
	this->inputMatrixA = "";
	this->inputMatrixB = "";
	this->outputMatrix = "";
	this->eigenvectors = "";
	this->numComponents = 0;
	this->numImages = 0;
	this->inMin = 0;
	this->inMax = 0;
	this->outMin = 0;
	this->outMax = 0;
	this->calcInMinMax = true;
	this->calcMean = true;
}

RSGISAlgorithmParameters* RSGISExeImageCalculation::getInstance()
{
	return new RSGISExeImageCalculation();
}

void RSGISExeImageCalculation::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;
	RSGISFileUtils fileUtils;
	
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *optionNormalise = XMLString::transcode("normalise");
	XMLCh *optionCorrelation = XMLString::transcode("correlation");
	XMLCh *optionCovariance = XMLString::transcode("covariance");
	XMLCh *optionMeanVector = XMLString::transcode("meanvector");
	XMLCh *optionPCA = XMLString::transcode("pca");
	XMLCh *optionStandardise = XMLString::transcode("standardise");
	XMLCh *optionBandMaths = XMLString::transcode("bandmaths");
	XMLCh *optionReplaceValuesLessThan = XMLString::transcode("replacevalueslessthan");
	XMLCh *optionUnitArea = XMLString::transcode("unitarea");
	XMLCh *optionImageMaths = XMLString::transcode("imagemaths");
	XMLCh *optionMovementSpeed = XMLString::transcode("movementspeed");
	XMLCh *optionCountValsInCol = XMLString::transcode("countvalsincol");
	XMLCh *optionCalcRMSE = XMLString::transcode("calcRMSE");
	XMLCh *optionApply2VarFunction = XMLString::transcode("apply2VarFunction");
	XMLCh *optionApply3VarFunction = XMLString::transcode("apply3VarFunction");
    XMLCh *optionDist2Geoms = XMLString::transcode("dist2geoms"); 
    XMLCh *optionImageBandStats = XMLString::transcode("imagebandstats"); 
    XMLCh *optionImageStats = XMLString::transcode("imagestats"); 
    XMLCh *optionUnconLinearSpecUnmix = XMLString::transcode("unconlinearspecunmix"); 
    XMLCh *optionExhConLinearSpecUnmix = XMLString::transcode("exhconlinearspecunmix"); 
	XMLCh *optionKMeansCentres = XMLString::transcode("kmeanscentres");
    XMLCh *optionISODataCentres = XMLString::transcode("isodatacentres");

	const XMLCh *algorNameEle = argElement->getAttribute(XMLString::transcode("algor"));
	if(!XMLString::equals(algorName, algorNameEle))
	{
			throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}

	// Set output image fomat (defaults to ENVI)
	this->imageFormat = "ENVI";
	XMLCh *formatXMLStr = XMLString::transcode("format");
	if(argElement->hasAttribute(formatXMLStr))
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
		this->imageFormat = string(charValue);
		XMLString::release(&charValue);
	}
	XMLString::release(&formatXMLStr);



	const XMLCh *optionXML = argElement->getAttribute(XMLString::transcode("option"));
	if(XMLString::equals(optionNormalise, optionXML))
	{		
		this->option = RSGISExeImageCalculation::normalise;
		
		if(argElement->hasAttribute(XMLString::transcode("dir")))
		{
			const XMLCh *dir = argElement->getAttribute(XMLString::transcode("dir"));
			string dirStr = XMLString::transcode(dir);
			
			const XMLCh *ext = argElement->getAttribute(XMLString::transcode("ext"));
			string extStr = XMLString::transcode(ext);
			
			const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
			string outputBase = XMLString::transcode(output);
			try
			{
				this->inputImages = fileUtils.getDIRList(dirStr, extStr, &this->numImages, false);
			}
			catch(RSGISException e)
			{
				throw RSGISXMLArgumentsException(e.what());
			}
			this->outputImages = new string[this->numImages];
			for(int i = 0; i < this->numImages; i++)
			{
				outputImages[i] = outputBase +  fileUtils.getFileNameNoExtension(inputImages[i]) + string("_norm.env");
			}
		}
		else
		{
			this->numImages = 1;
			this->inputImages = new string[1];
			const XMLCh *image = argElement->getAttribute(XMLString::transcode("image"));
			this->inputImages[0] = XMLString::transcode(image);
			
			this->outputImages = new string[1];
			const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
			this->outputImages[0] = XMLString::transcode(output);
			
		}
				
		this->outMin = mathUtils.strtodouble(XMLString::transcode(argElement->getAttribute(XMLString::transcode("outmin"))));
		this->outMax = mathUtils.strtodouble(XMLString::transcode(argElement->getAttribute(XMLString::transcode("outmax"))));
		
		if(argElement->hasAttribute(XMLString::transcode("inmin")) & argElement->hasAttribute(XMLString::transcode("inmax")))
		{
			this->inMin = mathUtils.strtodouble(XMLString::transcode(argElement->getAttribute(XMLString::transcode("inmin"))));
			this->inMax = mathUtils.strtodouble(XMLString::transcode(argElement->getAttribute(XMLString::transcode("inmax"))));
			this->calcInMinMax = false;
		}
		else
		{
			this->calcInMinMax = true;
		}
	}
	else if(XMLString::equals(optionCorrelation, optionXML))
	{		
		this->option = RSGISExeImageCalculation::correlation;
		
		const XMLCh *image = argElement->getAttribute(XMLString::transcode("imageA"));
		this->inputImageA = XMLString::transcode(image);
		
		image = argElement->getAttribute(XMLString::transcode("imageB"));
		this->inputImageB = XMLString::transcode(image);
		
		const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
		this->outputMatrix = XMLString::transcode(output);
	}
	else if(XMLString::equals(optionCovariance, optionXML))
	{		
		this->option = RSGISExeImageCalculation::covariance;
		
		const XMLCh *image = argElement->getAttribute(XMLString::transcode("imageA"));
		this->inputImageA = XMLString::transcode(image);
		
		image = argElement->getAttribute(XMLString::transcode("imageB"));
		this->inputImageB = XMLString::transcode(image);
		
		const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
		this->outputMatrix = XMLString::transcode(output);
		
		if(argElement->hasAttribute(XMLString::transcode("meanA")) & argElement->hasAttribute(XMLString::transcode("meanB")))
		{
			this->calcMean = false;
			
			const XMLCh *meanA = argElement->getAttribute(XMLString::transcode("meanA"));
			this->inputMatrixA = XMLString::transcode(meanA);
			
			const XMLCh *meanB = argElement->getAttribute(XMLString::transcode("meanB"));
			this->inputMatrixB = XMLString::transcode(meanB);
		}
		else
		{
			this->calcMean = true;
		}
		
	}
	else if(XMLString::equals(optionMeanVector, optionXML))
	{		
		this->option = RSGISExeImageCalculation::meanvector;

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
			this->outputMatrix = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
	}
	else if(XMLString::equals(optionPCA, optionXML))
	{		
		this->option = RSGISExeImageCalculation::pca;
		
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
		
		const XMLCh *eigenvectorsStr = argElement->getAttribute(XMLString::transcode("eigenvectors"));
		this->eigenvectors = XMLString::transcode(eigenvectorsStr);
		
		this->numComponents = mathUtils.strtoint(XMLString::transcode(argElement->getAttribute(XMLString::transcode("components"))));
	}
	else if(XMLString::equals(optionStandardise, optionXML))
	{		
		this->option = RSGISExeImageCalculation::standardise;
		
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
		
		const XMLCh *meanvectorXMLStr = argElement->getAttribute(XMLString::transcode("meanvector"));
		this->meanvectorStr = XMLString::transcode(meanvectorXMLStr);
		
		this->numComponents = mathUtils.strtoint(XMLString::transcode(argElement->getAttribute(XMLString::transcode("components"))));
	}
	else if(XMLString::equals(optionBandMaths, optionXML))
	{
		this->option = RSGISExeImageCalculation::bandmaths;
		
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
		
		XMLCh *expressionXMLStr = XMLString::transcode("expression");
		XMLCh *expressionFileXMLStr = XMLString::transcode("expressionFile");
		if(argElement->hasAttribute(expressionXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(expressionXMLStr));
            string muExpression = string(charValue);
            replace_all(muExpression, "&lt;", "<");
            replace_all(muExpression, "&gt;", ">");
            replace_all(muExpression, "&ge;", ">=");
            replace_all(muExpression, "&le;", "<=");
            
            replace_all(muExpression, "lt", "<");
            replace_all(muExpression, "gt", ">");
            replace_all(muExpression, "ge", ">=");
            replace_all(muExpression, "le", "<=");

			this->mathsExpression = muExpression;
			XMLString::release(&charValue);
		}
		else if(argElement->hasAttribute(expressionFileXMLStr)) // Get expression from text file
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(expressionFileXMLStr));
			string inputExprFileName = string(charValue);
            string muExpression = "";
			ifstream inputExprFile;
			inputExprFile.open(inputExprFileName.c_str());
			if(!inputExprFile.is_open())
			{
				throw RSGISXMLArgumentsException("Could not open input expression file.");
			}
			string strLine;
			while(!inputExprFile.eof())
			{
				getline(inputExprFile, strLine, '\n');
				muExpression += strLine;
			}
			// Shouldn't need to replace XML specific expressions but try anyway.
            replace_all(muExpression, "&lt;", "<");
            replace_all(muExpression, "&gt;", ">");
            replace_all(muExpression, "&ge;", ">=");
            replace_all(muExpression, "&le;", "<=");
            
            replace_all(muExpression, "lt", "<");
            replace_all(muExpression, "gt", ">");
            replace_all(muExpression, "ge", ">=");
            replace_all(muExpression, "le", "<=");
			
			// Replace tabs
			replace_all(muExpression, "\t", ""); 
			replace_all(muExpression, "\n", ""); 
			replace_all(muExpression, " ", ""); 
			
			this->mathsExpression = muExpression;
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'expression\' attribute was provided.");
		}
		XMLString::release(&expressionXMLStr);
		XMLString::release(&expressionFileXMLStr);
		
		DOMNodeList *varNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:variable"));
		this->numVars = varNodesList->getLength();		
		
		cout << "Found " << this->numVars << " Variables \n";
		
		DOMElement *varElement = NULL;
		variables = new VariableStruct[numVars];
		
		for(int i = 0; i < numVars; i++)
		{
			varElement = static_cast<DOMElement*>(varNodesList->item(i));
			
			XMLCh *varNameXMLStr = XMLString::transcode("name");
			if(varElement->hasAttribute(varNameXMLStr))
			{
				char *charValue = XMLString::transcode(varElement->getAttribute(varNameXMLStr));
				this->variables[i].name = string(charValue);
                if((this-> variables[i].name == "lt") || (this-> variables[i].name == "gt") || (this-> variables[i].name == "le") || (this-> variables[i].name == "ge"))
                {
                    throw RSGISXMLArgumentsException("Can't use \'lt\', \'gt\',\'ge\' or \'le\' for variable names");
                }
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			XMLString::release(&varNameXMLStr);
			
			XMLCh *varImageXMLStr = XMLString::transcode("image");
			if(varElement->hasAttribute(varImageXMLStr))
			{
				char *charValue = XMLString::transcode(varElement->getAttribute(varImageXMLStr));
				this->variables[i].image = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
			}
			XMLString::release(&varImageXMLStr);
			
			XMLCh *varBandXMLStr = XMLString::transcode("band");
			if(varElement->hasAttribute(varBandXMLStr))
			{
				char *charValue = XMLString::transcode(varElement->getAttribute(varBandXMLStr));
				this->variables[i].bandNum = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			XMLString::release(&varBandXMLStr);			
		}
	}
	else if (XMLString::equals(optionReplaceValuesLessThan, optionXML))
	{		
		this->option = RSGISExeImageCalculation::replacevalueslessthan;
		
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
		
		XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			this->threshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
		}
		XMLString::release(&thresholdXMLStr);
		
		XMLCh *valueXMLStr = XMLString::transcode("value");
		if(argElement->hasAttribute(valueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(valueXMLStr));
			this->value = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'value\' attribute was provided.");
		}
		XMLString::release(&valueXMLStr);
	}
	else if (XMLString::equals(optionUnitArea, optionXML))
	{		
		this->option = RSGISExeImageCalculation::unitarea;
		
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
		

	}
	else if(XMLString::equals(optionImageMaths, optionXML))
	{
		this->option = RSGISExeImageCalculation::imagemaths;
		
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
		
		XMLCh *expressionXMLStr = XMLString::transcode("expression");
		XMLCh *expressionFileXMLStr = XMLString::transcode("expressionFile");
		if(argElement->hasAttribute(expressionXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(expressionXMLStr));
            string muExpression = string(charValue);
            replace_all(muExpression, "&lt;", "<");
            replace_all(muExpression, "&gt;", ">");
            replace_all(muExpression, "&ge;", ">=");
            replace_all(muExpression, "&le;", "<=");
            
            replace_all(muExpression, "lt", "<");
            replace_all(muExpression, "gt", ">");
            replace_all(muExpression, "ge", ">=");
            replace_all(muExpression, "le", "<=");
			
			this->mathsExpression = muExpression;
			XMLString::release(&charValue);
		}
		else if(argElement->hasAttribute(expressionFileXMLStr)) // Get expression from text file
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(expressionFileXMLStr));
			string inputExprFileName = string(charValue);
            string muExpression = "";
			ifstream inputExprFile;
			inputExprFile.open(inputExprFileName.c_str());
			if(!inputExprFile.is_open())
			{
				throw RSGISXMLArgumentsException("Could not open input expression file.");
			}
			string strLine;
			while(!inputExprFile.eof())
			{
				getline(inputExprFile, strLine, '\n');
				muExpression += strLine;
			}
			// Shouldn't need to replace XML specific expressions but try anyway.
            replace_all(muExpression, "&lt;", "<");
            replace_all(muExpression, "&gt;", ">");
            replace_all(muExpression, "&ge;", ">=");
            replace_all(muExpression, "&le;", "<=");
            
            replace_all(muExpression, "lt", "<");
            replace_all(muExpression, "gt", ">");
            replace_all(muExpression, "ge", ">=");
            replace_all(muExpression, "le", "<=");
			
			// Replace tabs
			replace_all(muExpression, "\t", ""); 
			replace_all(muExpression, "\n", ""); 
			replace_all(muExpression, " ", ""); 
			
			this->mathsExpression = muExpression;
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'expression\' attribute was provided.");
		}
		XMLString::release(&expressionXMLStr);
		XMLString::release(&expressionFileXMLStr);
	}
	else if(XMLString::equals(optionMovementSpeed, optionXML))
	{
		this->option = RSGISExeImageCalculation::movementspeed;
		
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
		
		XMLCh *upperXMLStr = XMLString::transcode("upper");
		if(argElement->hasAttribute(upperXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(upperXMLStr));
			this->upper = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'upper\' attribute was provided.");
		}
		XMLString::release(&upperXMLStr);
		
		XMLCh *lowerXMLStr = XMLString::transcode("lower");
		if(argElement->hasAttribute(lowerXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(lowerXMLStr));
			this->lower = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'lower\' attribute was provided.");
		}
		XMLString::release(&lowerXMLStr);
		
		DOMNodeList *imgNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:image"));
		this->numImages = imgNodesList->getLength();		
		
		cout << "Found " << this->numImages << " Images.\n";
		
		inputImages = new string[numImages];
		imageBands = new unsigned int[numImages];
		imageTimes = new float[numImages];
		
		DOMElement *imgElement = NULL;
		
		for(int i = 0; i < numImages; ++i)
		{
			imgElement = static_cast<DOMElement*>(imgNodesList->item(i));
			
			XMLCh *imgImageXMLStr = XMLString::transcode("image");
			if(imgElement->hasAttribute(imgImageXMLStr))
			{
				char *charValue = XMLString::transcode(imgElement->getAttribute(imgImageXMLStr));
				this->inputImages[i] = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
			}
			XMLString::release(&imgImageXMLStr);
			
			XMLCh *imgBandXMLStr = XMLString::transcode("band");
			if(imgElement->hasAttribute(imgBandXMLStr))
			{
				char *charValue = XMLString::transcode(imgElement->getAttribute(imgBandXMLStr));
				this->imageBands[i] = mathUtils.strtoint(string(charValue))-1;
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			XMLString::release(&imgBandXMLStr);
			
			XMLCh *imgTimeXMLStr = XMLString::transcode("time");
			if(imgElement->hasAttribute(imgTimeXMLStr))
			{
				char *charValue = XMLString::transcode(imgElement->getAttribute(imgTimeXMLStr));
				this->imageTimes[i] = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'time\' attribute was provided.");
			}
			XMLString::release(&imgTimeXMLStr);	
		}
	}
	else if(XMLString::equals(optionCountValsInCol, optionXML))
	{
		this->option = RSGISExeImageCalculation::countvalsincol;
		
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
		
		XMLCh *upperXMLStr = XMLString::transcode("upper");
		if(argElement->hasAttribute(upperXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(upperXMLStr));
			this->upper = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'upper\' attribute was provided.");
		}
		XMLString::release(&upperXMLStr);
		
		XMLCh *lowerXMLStr = XMLString::transcode("lower");
		if(argElement->hasAttribute(lowerXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(lowerXMLStr));
			this->lower = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'lower\' attribute was provided.");
		}
		XMLString::release(&lowerXMLStr);
	}
	else if(XMLString::equals(optionCalcRMSE, optionXML))
	{
		
		this->option = RSGISExeImageCalculation::calcRMSE;
		
		XMLCh *imageAXMLStr = XMLString::transcode("imageA");
		if(argElement->hasAttribute(imageAXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageAXMLStr));
			this->inputImageA = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("imageA not provided");
		}
		XMLString::release(&imageAXMLStr);
		
		XMLCh *imageBXMLStr = XMLString::transcode("imageB");
		if(argElement->hasAttribute(imageBXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageBXMLStr));
			this->inputImageB = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("imageB not provided");
		}
		XMLString::release(&imageBXMLStr);
		
		XMLCh *bandAXMLStr = XMLString::transcode("bandA");
		if(argElement->hasAttribute(bandAXMLStr)) 
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bandAXMLStr));
			this->inputBandA = mathUtils.strtoint(string(charValue)) - 1;
			if (this->inputBandA < 0) 
			{
				cout << "\tBand numbering starts at 1, assuming first band for image A" << endl;
				this->inputBandA = 0;
			}
			XMLString::release(&charValue);
		}
		else
		{
			this->inputBandA = 0;
			cout << "\tNo band set for image A, using default of band 1" << endl;
		}
		XMLString::release(&bandAXMLStr);
		
		XMLCh *bandBXMLStr = XMLString::transcode("bandB");
		if(argElement->hasAttribute(bandBXMLStr)) 
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bandBXMLStr));
			this->inputBandB = mathUtils.strtoint(string(charValue)) - 1;
			if (this->inputBandB < 0) 
			{
				cout << "\tBand numbering starts at 1, assuming first band for image B" << endl;
				this->inputBandB = 0;
			}
			XMLString::release(&charValue);
		}
		else
		{
			this->inputBandB = 0;
			cout << "\tNo band set for image B, using default of band 1" << endl;
		}
		XMLString::release(&bandBXMLStr);
		
	}
	else if(XMLString::equals(optionApply2VarFunction,optionXML))
	{		
		this->option = RSGISExeImageCalculation::apply2VarFunction;
		
		RSGISMatrices matrixUtils;
		
		XMLCh *imageXMLStr = XMLString::transcode("input");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
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
		
		// Get function to use.
		// The XML interface is available for a number of functions within the RSGISExeEstimationAlgorithm.
		XMLCh *function2DPoly = XMLString::transcode("2DPoly");
		const XMLCh *functionStr = argElement->getAttribute(XMLString::transcode("function"));
		
		if (XMLString::equals(function2DPoly,functionStr))
		{
			// Read coefficients
			XMLCh *coefficients = XMLString::transcode("coefficients");
			if(argElement->hasAttribute(coefficients))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(coefficients));
				string coeffFile = string(charValue);
				this->coeffMatrix = matrixUtils.readGSLMatrixFromTxt(coeffFile);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No coefficents provided for function");
			}
			XMLString::release(&coefficients);
			
			this->twoVarFunction = (RSGISMathTwoVariableFunction *) new RSGISFunction2DPoly(coeffMatrix);
		}
		else 
		{
			throw RSGISXMLArgumentsException("Unknown function");
 		}

		XMLString::release(&function2DPoly);
	}
	else if(XMLString::equals(optionApply3VarFunction,optionXML))
	{		
		this->option = RSGISExeImageCalculation::apply3VarFunction;
		
		RSGISMatrices matrixUtils;
		
		XMLCh *imageXMLStr = XMLString::transcode("input");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
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
		
		// Get function to use.
		// The XML interface is available for a number of functions within the RSGISExeEstimationAlgorithm.
		XMLCh *function3DPoly = XMLString::transcode("3DPoly");
		const XMLCh *functionStr = argElement->getAttribute(XMLString::transcode("function"));
		
		if (XMLString::equals(function3DPoly,functionStr))
		{
			// Read coefficients
			XMLCh *coefficients = XMLString::transcode("coefficients");
			if(argElement->hasAttribute(coefficients))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(coefficients));
				string coeffFile = string(charValue);
				this->coeffMatrix = matrixUtils.readGSLMatrixFromTxt(coeffFile);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No coefficents provided for function");
			}
			XMLString::release(&coefficients);
			
			bool sameOrderBothFits = false;
			XMLCh *polyOrderStr = XMLString::transcode("polyOrder"); // Polynomial Order
			if(argElement->hasAttribute(polyOrderStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderStr));
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
			if(argElement->hasAttribute(polyOrderXStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderXStr));
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
			if(argElement->hasAttribute(polyOrderYStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderYStr));
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
			if(argElement->hasAttribute(polyOrderZStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderZStr));
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
			
			this->threeVarFunction = (RSGISMathThreeVariableFunction *) new RSGISFunction3DPoly(coeffMatrix, polyOrderX, polyOrderY, polyOrderZ);
		}
		else 
		{
			throw RSGISXMLArgumentsException("Unknown function");
 		}
		
		XMLString::release(&function3DPoly);
	}
    else if(XMLString::equals(optionDist2Geoms, optionXML))
	{
		this->option = RSGISExeImageCalculation::dist2geoms;
		
        XMLCh *vectorXMLStr = XMLString::transcode("vector");
		if(argElement->hasAttribute(vectorXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
			this->inputVector = string(charValue);
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
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
        XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
		if(argElement->hasAttribute(resolutionXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resolutionXMLStr));
			this->imgResolution = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided.");
		}
		XMLString::release(&resolutionXMLStr);
		
	}
    else if(XMLString::equals(optionImageBandStats, optionXML))
	{		
		this->option = RSGISExeImageCalculation::imagebandstats;
		
        
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
			this->outputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        XMLCh *ignoreZerosXMLStr = XMLString::transcode("ignorezeros");
		if(argElement->hasAttribute(ignoreZerosXMLStr))
		{
            XMLCh *noStr = XMLString::transcode("no");
			const XMLCh *ignoreValue = argElement->getAttribute(ignoreZerosXMLStr);
			if(XMLString::equals(ignoreValue, noStr))
			{
                this->ignoreZeros = false;
			}
			else
			{
				this->ignoreZeros = true;
			}
			XMLString::release(&noStr);
		}
		else
		{
			cerr << "No \'ignorezeros\' attribute was provided so defaulting to ignore zeros.\n";
            this->ignoreZeros = true;
		}
		XMLString::release(&ignoreZerosXMLStr);
        
	}
    else if(XMLString::equals(optionImageStats, optionXML))
	{		
		this->option = RSGISExeImageCalculation::imagestats;
		
        
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
			this->outputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        XMLCh *ignoreZerosXMLStr = XMLString::transcode("ignorezeros");
		if(argElement->hasAttribute(ignoreZerosXMLStr))
		{
            XMLCh *noStr = XMLString::transcode("no");
			const XMLCh *ignoreValue = argElement->getAttribute(ignoreZerosXMLStr);
			if(XMLString::equals(ignoreValue, noStr))
			{
                this->ignoreZeros = false;
			}
			else
			{
				this->ignoreZeros = true;
			}
			XMLString::release(&noStr);
		}
		else
		{
			cerr << "No \'ignorezeros\' attribute was provided so defaulting to ignore zeros.\n";
            this->ignoreZeros = true;
		}
		XMLString::release(&ignoreZerosXMLStr);
        
	}
    else if(XMLString::equals(optionUnconLinearSpecUnmix, optionXML))
	{		
		this->option = RSGISExeImageCalculation::unconlinearspecunmix;
		
        
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
			this->outputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        XMLCh *endmembersXMLStr = XMLString::transcode("endmembers");
		if(argElement->hasAttribute(endmembersXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(endmembersXMLStr));
			this->endmembersFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'endmembers\' attribute was provided.");
		}
		XMLString::release(&endmembersXMLStr);
	}
    else if(XMLString::equals(optionExhConLinearSpecUnmix, optionXML))
	{		
		this->option = RSGISExeImageCalculation::exhconlinearspecunmix;
		
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
			this->outputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        XMLCh *endmembersXMLStr = XMLString::transcode("endmembers");
		if(argElement->hasAttribute(endmembersXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(endmembersXMLStr));
			this->endmembersFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'endmembers\' attribute was provided.");
		}
		XMLString::release(&endmembersXMLStr);
        
        
        XMLCh *stepXMLStr = XMLString::transcode("step");
		if(argElement->hasAttribute(stepXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(stepXMLStr));
			this->stepResolution = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'step\' attribute was provided.");
		}
		XMLString::release(&stepXMLStr);
	}
    else if(XMLString::equals(optionKMeansCentres, optionXML))
	{		
		this->option = RSGISExeImageCalculation::kmeanscentres;
		
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
			this->outputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        XMLCh *numClustersXMLStr = XMLString::transcode("numclusters");
		if(argElement->hasAttribute(numClustersXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numClustersXMLStr));
			this->numClusters = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'numclusters\' attribute was provided.");
		}
		XMLString::release(&numClustersXMLStr);
        
        
        XMLCh *maxIterationsXMLStr = XMLString::transcode("maxiterations");
		if(argElement->hasAttribute(maxIterationsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
			this->maxNumIterations = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'maxiterations\' attribute was provided.");
		}
		XMLString::release(&maxIterationsXMLStr);
        
        
        XMLCh *degreeOfChangeXMLStr = XMLString::transcode("degreeofchange");
		if(argElement->hasAttribute(degreeOfChangeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(degreeOfChangeXMLStr));
			this->degreeOfChange = mathUtils.strtofloat(string(charValue))/100;
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'degreeofchange\' attribute was provided.");
		}
		XMLString::release(&degreeOfChangeXMLStr);
        
        XMLCh *subsampleXMLStr = XMLString::transcode("subsample");
		if(argElement->hasAttribute(subsampleXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(subsampleXMLStr));
			this->subSample = mathUtils.strtounsignedint(string(charValue));
            if(this->subSample == 0)
            {
                cerr << "Warning: SubSample must have a value of at least 1. Value updated to 1.\n";
                this->subSample = 1;
            }
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'subsample\' attribute was provided.");
		}
		XMLString::release(&subsampleXMLStr);
        
        XMLCh *initMethodXMLStr = XMLString::transcode("initmethod");
		if(argElement->hasAttribute(initMethodXMLStr))
		{
            XMLCh *randomStr = XMLString::transcode("random");
            XMLCh *diagonalRangeStr = XMLString::transcode("diagonal_range");
            XMLCh *diagonalStdDevStr = XMLString::transcode("diagonal_stddev");
            XMLCh *diagonalRangeAttachStr = XMLString::transcode("diagonal_range_attach");
            XMLCh *diagonalStdDevAttachStr = XMLString::transcode("diagonal_stddev_attach");
            XMLCh *kppStr = XMLString::transcode("kpp");
			const XMLCh *strValue = argElement->getAttribute(initMethodXMLStr);
			
			if(XMLString::equals(strValue, randomStr))
			{
				this->initClusterMethod = rsgis::math::init_random;
			}
            else if(XMLString::equals(strValue, diagonalRangeStr))
			{
				this->initClusterMethod = rsgis::math::init_diagonal_full;
			}
            else if(XMLString::equals(strValue, diagonalStdDevStr))
			{
				this->initClusterMethod = rsgis::math::init_diagonal_stddev;
			}
            else if(XMLString::equals(strValue, diagonalRangeAttachStr))
			{
				this->initClusterMethod = rsgis::math::init_diagonal_full_attach;
			}
            else if(XMLString::equals(strValue, diagonalStdDevAttachStr))
			{
				this->initClusterMethod = rsgis::math::init_diagonal_stddev_attach;
			}
            else if(XMLString::equals(strValue, kppStr))
			{
				this->initClusterMethod = rsgis::math::init_kpp;
			}
			else
			{
				this->initClusterMethod = rsgis::math::init_random;
                cerr << "The initial cluster method was not reconised so random method is being used\n";
			}
			XMLString::release(&randomStr);
            XMLString::release(&diagonalRangeStr);
            XMLString::release(&diagonalStdDevStr);
            XMLString::release(&diagonalRangeAttachStr);
            XMLString::release(&diagonalStdDevAttachStr);
            XMLString::release(&kppStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'initmethod\' attribute was provided.");
		}
		XMLString::release(&initMethodXMLStr);
        
        
        XMLCh *ignoreZerosXMLStr = XMLString::transcode("ignorezeros");
		if(argElement->hasAttribute(ignoreZerosXMLStr))
		{
            XMLCh *noStr = XMLString::transcode("no");
			const XMLCh *strValue = argElement->getAttribute(ignoreZerosXMLStr);
			
			if(XMLString::equals(strValue, noStr))
			{
				this->ignoreZeros = false;
			}
			else
			{
				this->ignoreZeros = true;
			}
			XMLString::release(&noStr);
		}
		else
		{
			this->ignoreZeros = true;
            cerr << "Ignoring zeros options not specified by default zero will be ignored.\n";
		}
		XMLString::release(&ignoreZerosXMLStr);
	}
    else if(XMLString::equals(optionISODataCentres, optionXML))
	{		
		this->option = RSGISExeImageCalculation::isodatacentres;
		
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
			this->outputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        XMLCh *numClustersXMLStr = XMLString::transcode("numclusters");
		if(argElement->hasAttribute(numClustersXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numClustersXMLStr));
			this->numClusters = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'numclusters\' attribute was provided.");
		}
		XMLString::release(&numClustersXMLStr);
        
        XMLCh *maxIterationsXMLStr = XMLString::transcode("maxiterations");
		if(argElement->hasAttribute(maxIterationsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
			this->maxNumIterations = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'maxiterations\' attribute was provided.");
		}
		XMLString::release(&maxIterationsXMLStr);
        
        
        XMLCh *degreeOfChangeXMLStr = XMLString::transcode("degreeofchange");
		if(argElement->hasAttribute(degreeOfChangeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(degreeOfChangeXMLStr));
			this->degreeOfChange = mathUtils.strtofloat(string(charValue))/100;
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'degreeofchange\' attribute was provided.");
		}
		XMLString::release(&degreeOfChangeXMLStr);
        
        XMLCh *subsampleXMLStr = XMLString::transcode("subsample");
		if(argElement->hasAttribute(subsampleXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(subsampleXMLStr));
			this->subSample = mathUtils.strtounsignedint(string(charValue));
            if(this->subSample == 0)
            {
                cerr << "Warning: SubSample must have a value of at least 1. Value updated to 1.\n";
                this->subSample = 1;
            }
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'subsample\' attribute was provided.");
		}
		XMLString::release(&subsampleXMLStr);
        
        XMLCh *initMethodXMLStr = XMLString::transcode("initmethod");
		if(argElement->hasAttribute(initMethodXMLStr))
		{
            XMLCh *randomStr = XMLString::transcode("random");
            XMLCh *diagonalRangeStr = XMLString::transcode("diagonal_range");
            XMLCh *diagonalStdDevStr = XMLString::transcode("diagonal_stddev");
            XMLCh *diagonalRangeAttachStr = XMLString::transcode("diagonal_range_attach");
            XMLCh *diagonalStdDevAttachStr = XMLString::transcode("diagonal_stddev_attach");
            XMLCh *kppStr = XMLString::transcode("kpp");
			const XMLCh *strValue = argElement->getAttribute(initMethodXMLStr);
			
			if(XMLString::equals(strValue, randomStr))
			{
				this->initClusterMethod = rsgis::math::init_random;
			}
            else if(XMLString::equals(strValue, diagonalRangeStr))
			{
				this->initClusterMethod = rsgis::math::init_diagonal_full;
			}
            else if(XMLString::equals(strValue, diagonalStdDevStr))
			{
				this->initClusterMethod = rsgis::math::init_diagonal_stddev;
			}
            else if(XMLString::equals(strValue, diagonalRangeAttachStr))
			{
				this->initClusterMethod = rsgis::math::init_diagonal_full_attach;
			}
            else if(XMLString::equals(strValue, diagonalStdDevAttachStr))
			{
				this->initClusterMethod = rsgis::math::init_diagonal_stddev_attach;
			}
            else if(XMLString::equals(strValue, kppStr))
			{
				this->initClusterMethod = rsgis::math::init_kpp;
			}
			else
			{
				this->initClusterMethod = rsgis::math::init_random;
                cerr << "The initial cluster method was not reconised so random method is being used\n";
			}
			XMLString::release(&randomStr);
            XMLString::release(&diagonalRangeStr);
            XMLString::release(&diagonalStdDevStr);
            XMLString::release(&diagonalRangeAttachStr);
            XMLString::release(&diagonalStdDevAttachStr);
            XMLString::release(&kppStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'initmethod\' attribute was provided.");
		}
		XMLString::release(&initMethodXMLStr);
        
        
        XMLCh *ignoreZerosXMLStr = XMLString::transcode("ignorezeros");
		if(argElement->hasAttribute(ignoreZerosXMLStr))
		{
            XMLCh *noStr = XMLString::transcode("no");
			const XMLCh *strValue = argElement->getAttribute(ignoreZerosXMLStr);
			
			if(XMLString::equals(strValue, noStr))
			{
				this->ignoreZeros = false;
			}
			else
			{
				this->ignoreZeros = true;
			}
			XMLString::release(&noStr);
		}
		else
		{
			this->ignoreZeros = true;
            cerr << "Ignoring zeros options not specified by default zero will be ignored.\n";
		}
		XMLString::release(&ignoreZerosXMLStr);
                
        XMLCh *minNumXMLStr = XMLString::transcode("minnum");
		if(argElement->hasAttribute(minNumXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minNumXMLStr));
			this->minNumFeatures = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'minnum\' attribute was provided.");
		}
		XMLString::release(&minNumXMLStr);
        
        XMLCh *minDistXMLStr = XMLString::transcode("mindist");
		if(argElement->hasAttribute(minDistXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minDistXMLStr));
			this->minDistBetweenClusters = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'mindist\' attribute was provided.");
		}
		XMLString::release(&minDistXMLStr);
        
        XMLCh *maxStdDevXMLStr = XMLString::transcode("maxstddev");
		if(argElement->hasAttribute(maxStdDevXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxStdDevXMLStr));
			this->maxStdDev = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'maxstddev\' attribute was provided.");
		}
		XMLString::release(&maxStdDevXMLStr);
        
        XMLCh *minNumClustersXMLStr = XMLString::transcode("minnumclusters");
		if(argElement->hasAttribute(minNumClustersXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minNumClustersXMLStr));
			this->minNumClusters = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'minnumclusters\' attribute was provided.");
		}
		XMLString::release(&minNumClustersXMLStr);
        
        
        XMLCh *editStartXMLStr = XMLString::transcode("editstart");
		if(argElement->hasAttribute(editStartXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(editStartXMLStr));
			this->startIteration = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'editstart\' attribute was provided.");
		}
		XMLString::release(&editStartXMLStr);
        
        XMLCh *editEndXMLStr = XMLString::transcode("editend");
		if(argElement->hasAttribute(editEndXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(editEndXMLStr));
			this->endIteration = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'editend\' attribute was provided.");
		}
		XMLString::release(&editEndXMLStr);
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeImageCalculation.");
		throw RSGISXMLArgumentsException(message.c_str());
	}
	
	XMLString::release(&algorName);
	XMLString::release(&optionNormalise);
	XMLString::release(&optionCorrelation);
	XMLString::release(&optionCovariance);
	XMLString::release(&optionMeanVector);
	XMLString::release(&optionPCA);
	XMLString::release(&optionStandardise);
	XMLString::release(&optionBandMaths);
	XMLString::release(&optionReplaceValuesLessThan);
	XMLString::release(&optionUnitArea);
	XMLString::release(&optionImageMaths);
	XMLString::release(&optionMovementSpeed);
	XMLString::release(&optionCountValsInCol);
	XMLString::release(&optionApply2VarFunction);
	XMLString::release(&optionApply3VarFunction);
    XMLString::release(&optionDist2Geoms);
    XMLString::release(&optionImageBandStats);
    XMLString::release(&optionImageStats);
    XMLString::release(&optionUnconLinearSpecUnmix);
    XMLString::release(&optionExhConLinearSpecUnmix);
    XMLString::release(&optionKMeansCentres);
    XMLString::release(&optionISODataCentres);

	parsed = true;
}

void RSGISExeImageCalculation::runAlgorithm() throw(RSGISException)
{
    cout.precision(12);
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(option == RSGISExeImageCalculation::normalise)
		{
			GDALAllRegister();
			GDALDataset *dataset = NULL;
			RSGISImageNormalisation *normImage = NULL;
			double *imageMaxBands = NULL;
			double *imageMinBands = NULL;
			double *outMinBands = NULL;
			double *outMaxBands = NULL;
			
			int rasterCount = 0;
			
			try
			{
				normImage = new RSGISImageNormalisation();
				
				for(int i = 0; i < this->numImages; i++)
				{
					cout << this->inputImages[i] << endl;
					dataset = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(dataset == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
					
					rasterCount = dataset->GetRasterCount();
					
					imageMaxBands = new double[rasterCount];
					imageMinBands = new double[rasterCount];
					outMinBands = new double[rasterCount];
					outMaxBands = new double[rasterCount];
					
					for(int j = 0; j < rasterCount; j++)
					{
						if(this->calcInMinMax)
						{
							imageMaxBands[j] = 0;
							imageMinBands[j] = 0;
						}
						else 
						{
							imageMaxBands[j] = this->inMax;
							imageMinBands[j] = this->inMin;
						}
						outMaxBands[j] = this->outMax;
						outMinBands[j] = this->outMin;
					}
					normImage->normaliseImage(dataset, imageMaxBands, imageMinBands, outMaxBands, outMinBands, this->calcInMinMax, this->outputImages[i]);
					
					GDALClose(dataset);
					delete[] imageMinBands;
					delete[] imageMaxBands;
					delete[] outMinBands;
					delete[] outMaxBands;
				}	
				delete[] inputImages;
				delete[] outputImages;
			}
			catch(RSGISException e)
			{
				throw e;
			}
			
			if(normImage != NULL)
			{
				delete normImage;
			}			
		}
		else if(option == RSGISExeImageCalculation::correlation)
		{
			GDALAllRegister();
			GDALDataset **datasetsA = NULL;
			GDALDataset **datasetsB = NULL;
			
			RSGISMatrices matrixUtils;
			
			RSGISCalcImageSingle *calcImgSingle = NULL;
			RSGISCalcCC *calcCC = NULL;
			RSGISCalcImageMatrix *calcImgMatrix = NULL;
			
			Matrix *correlationMatrix = NULL;
			
			try
			{
				datasetsA = new GDALDataset*[1];
				cout << this->inputImageA << endl;
				datasetsA[0] = (GDALDataset *) GDALOpenShared(this->inputImageA.c_str(), GA_ReadOnly);
				if(datasetsA[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImageA;
					throw RSGISImageException(message.c_str());
				}
				
				
				datasetsB = new GDALDataset*[1];
				cout << this->inputImageB << endl;
				datasetsB[0] = (GDALDataset *) GDALOpenShared(this->inputImageB.c_str(), GA_ReadOnly);
				if(datasetsB[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImageB;
					throw RSGISImageException(message.c_str());
				}
				
				calcCC = new RSGISCalcCC(1);
				calcImgSingle = new RSGISCalcImageSingle(calcCC);
				calcImgMatrix = new RSGISCalcImageMatrix(calcImgSingle);
				correlationMatrix = calcImgMatrix->calcImageMatrix(datasetsA, datasetsB, 1);
				matrixUtils.saveMatrix2txt(correlationMatrix, this->outputMatrix);
				
				delete calcCC;
				delete calcImgMatrix;
				delete calcImgSingle;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageCalculation::covariance)
		{
			GDALAllRegister();
			GDALDataset **datasetsA = NULL;
			GDALDataset **datasetsB = NULL;
			
			RSGISMatrices matrixUtils;
			
			RSGISCalcImageSingle *calcImgSingle = NULL;
			RSGISCalcCovariance *calcCovar = NULL;
			RSGISCalcImageMatrix *calcImgMatrix = NULL;
			
			RSGISCalcImageSingle *calcImgSingleMean = NULL;
			RSGISCalcMeanVectorIndividual *calcMean = NULL;
			RSGISCalcImageMatrix *calcImgMatrixMean = NULL;
			
			Matrix *meanAMatrix = NULL;
			Matrix *meanBMatrix = NULL;
			Matrix *covarianceMatrix = NULL;
			
			try
			{
				datasetsA = new GDALDataset*[1];
				cout << this->inputImageA << endl;
				datasetsA[0] = (GDALDataset *) GDALOpenShared(this->inputImageA.c_str(), GA_ReadOnly);
				if(datasetsA[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImageA;
					throw RSGISImageException(message.c_str());
				}
				
				datasetsB = new GDALDataset*[1];
				cout << this->inputImageB << endl;
				datasetsB[0] = (GDALDataset *) GDALOpenShared(this->inputImageB.c_str(), GA_ReadOnly);
				if(datasetsB[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImageB;
					throw RSGISImageException(message.c_str());
				}
				
				
				if(calcMean)
				{
					cout << "Mean vectors will be calculated\n";
					calcMean = new RSGISCalcMeanVectorIndividual(1);
					calcImgSingleMean = new RSGISCalcImageSingle(calcMean);
					calcImgMatrixMean = new RSGISCalcImageMatrix(calcImgSingle);
					meanAMatrix = calcImgMatrixMean->calcImageVector(datasetsA, 1);
					meanBMatrix = calcImgMatrixMean->calcImageVector(datasetsB, 1);
					cout << "Mean Vectors have been calculated\n";
				}
				else
				{
					meanAMatrix = matrixUtils.readMatrixFromTxt(this->inputMatrixA);
					meanBMatrix = matrixUtils.readMatrixFromTxt(this->inputMatrixB);
				}
				
				calcCovar = new RSGISCalcCovariance(1, meanAMatrix, meanBMatrix);
				calcImgSingle = new RSGISCalcImageSingle(calcCovar);
				calcImgMatrix = new RSGISCalcImageMatrix(calcImgSingle);
				covarianceMatrix = calcImgMatrix->calcImageMatrix(datasetsA, datasetsB, 1);
				matrixUtils.saveMatrix2txt(covarianceMatrix, this->outputMatrix);
			}
			catch(RSGISException e)
			{
				throw e;
			}
			
			if(calcImgSingle != NULL)
			{
				delete calcImgSingle;
			}
			if(calcCovar != NULL)
			{
				delete calcCovar;
			}
			if(calcImgMatrix != NULL)
			{
				delete calcImgMatrix;
			}
			
			if(calcImgSingleMean != NULL)
			{
				delete calcImgSingleMean;
			}
			if(calcMean != NULL)
			{
				delete calcMean;
			}
			if(calcImgMatrixMean != NULL)
			{
				delete calcImgMatrixMean;
			}
			
			if(meanAMatrix != NULL)
			{
				matrixUtils.freeMatrix(meanAMatrix);
			}
			if(meanBMatrix != NULL)
			{
				matrixUtils.freeMatrix(meanBMatrix);
			}
			if(covarianceMatrix != NULL)
			{
				matrixUtils.freeMatrix(covarianceMatrix);
			}
		}
		else if(option == RSGISExeImageCalculation::meanvector)
		{
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			RSGISCalcImageSingle *calcImgSingle = NULL;
			RSGISCalcMeanVectorIndividual *calcMean = NULL;
			RSGISCalcImageMatrix *calcImgMatrix = NULL;
			
			RSGISMatrices matrixUtils;
			Matrix *meanVectorMatrix = NULL;
			
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
				
				calcMean = new RSGISCalcMeanVectorIndividual(1);
				calcImgSingle = new RSGISCalcImageSingle(calcMean);
				calcImgMatrix = new RSGISCalcImageMatrix(calcImgSingle);
				meanVectorMatrix = calcImgMatrix->calcImageVector(datasets, 1);
				matrixUtils.saveMatrix2txt(meanVectorMatrix, this->outputMatrix);
			}
			catch(RSGISException e)
			{
				throw e;
			}
			
			if(calcImgSingle != NULL)
			{
				delete calcImgSingle;
			}
			if(calcMean != NULL)
			{
				delete calcMean;
			}
			if(calcImgMatrix != NULL)
			{
				delete calcImgMatrix;
			}
			
			if(meanVectorMatrix != NULL)
			{
				matrixUtils.freeMatrix(meanVectorMatrix);
			}
		}
		else if(option == RSGISExeImageCalculation::pca)
		{
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			RSGISMatrices matrixUtils;	
			RSGISCalcImage *calcImage = NULL;
			RSGISApplyEigenvectors *applyPCA = NULL;
			Matrix *eigenvectorsMatrix = NULL;
			
			try
			{
				cout << "Reading in from file " << this->eigenvectors << endl;
				eigenvectorsMatrix = matrixUtils.readMatrixFromTxt(this->eigenvectors);
				cout << "Finished reading in matrix\n";
				
				datasets = new GDALDataset*[1];
				cout << "Reading in image " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				applyPCA = new RSGISApplyEigenvectors(this->numComponents, eigenvectorsMatrix);
				calcImage = new RSGISCalcImage(applyPCA, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				GDALClose(datasets[0]);
			}
			catch(RSGISException e)
			{
				throw e;
			}
			
			if(calcImage != NULL)
			{
				delete calcImage;
			}
			if(applyPCA != NULL)
			{
				delete applyPCA;
			}
			
			if(eigenvectorsMatrix != NULL)
			{
				matrixUtils.freeMatrix(eigenvectorsMatrix);
			}
		}
		else if(option == RSGISExeImageCalculation::standardise)
		{
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			RSGISMatrices matrixUtils;	
			RSGISCalcImage *calcImage = NULL;
			RSGISStandardiseImage *stdImg = NULL;
			Matrix *meanVectorMatrix = NULL;
			
			try
			{
				cout << "Reading in from file " << this->meanvectorStr << endl;
				meanVectorMatrix = matrixUtils.readMatrixFromTxt(this->meanvectorStr);
				cout << "Finished reading in matrix\n";
				
				datasets = new GDALDataset*[1];
				cout << "Reading in image " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				stdImg = new RSGISStandardiseImage(datasets[0]->GetRasterCount(), meanVectorMatrix);
				calcImage = new RSGISCalcImage(stdImg, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				delete calcImage;
				delete stdImg;
				matrixUtils.freeMatrix(meanVectorMatrix);
				GDALClose(datasets[0]);
				
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageCalculation::bandmaths)
		{
			cout << "This command performs band maths\n";
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Expression: " << this->mathsExpression << endl;
			for(int i = 0; i < numVars; ++i)
			{
				cout << i << ")\t name = " << variables[i].name << " image = " << variables[i].image << " band = " << variables[i].bandNum << endl;
			}
			
			string *outBandName = new string[1];
			outBandName[0] = this->mathsExpression;

			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISBandMath *bandmaths = NULL;
			RSGISCalcImage *calcImage = NULL;
			Parser *muParser = new Parser();
						
			try
			{
				VariableBands **processVaribles = new VariableBands*[numVars];
				datasets = new GDALDataset*[numVars];
				
				int numRasterBands = 0;
				int totalNumRasterBands = 0;
				
				for(int i = 0; i < this->numVars; ++i)
				{
					cout << this->variables[i].image << endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->variables[i].image.c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->variables[i].image;
						throw RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if((this->variables[i].bandNum < 0) | (this->variables[i].bandNum > numRasterBands))
					{
						throw RSGISImageException("You have specified a band which is not within the image");
					}
					
					processVaribles[i] = new VariableBands();
					processVaribles[i]->name = this->variables[i].name;
					processVaribles[i]->band = totalNumRasterBands + (this->variables[i].bandNum - 1);
					
					totalNumRasterBands += numRasterBands;
				}	
				
				value_type *inVals = new value_type[this->numVars];
				for(int i = 0; i < this->numVars; ++i)
				{
					inVals[i] = 0;
					muParser->DefineVar(_T(processVaribles[i]->name.c_str()), &inVals[i]);
				}
				
				muParser->SetExpr(this->mathsExpression.c_str());
				
				bandmaths = new RSGISBandMath(1, processVaribles, this->numVars, muParser);
				
				calcImage = new RSGISCalcImage(bandmaths, "", true);
				calcImage->calcImage(datasets, this->numVars, this->outputImage, true, outBandName, this->imageFormat);
				
				
				for(int i = 0; i < this->numVars; ++i)
				{
					GDALClose(datasets[i]);
					delete processVaribles[i];
				}
				delete[] datasets;
				delete[] processVaribles;
				
				delete[] inVals;
				
				delete muParser;
				delete bandmaths;
				delete calcImage;
				delete[] outBandName;
			}
			catch(RSGISException e)
			{
				throw e;
			}
			catch (ParserError &e) 
			{
				string message = string("ERROR: ") + string(e.GetMsg()) + string(":\t \'") + string(e.GetExpr()) +string("\'");
				throw RSGISException(message);
			}
			
			
		}
		else if(option == RSGISExeImageCalculation::replacevalueslessthan)
		{
			cout << "Replace values less than a threshold with a new value.\n";
			cout << "Image: " << this->inputImage << endl;
			cout << "Output: " << this->outputImage << endl;
			cout << "Threshold: " << this->threshold << endl;
			cout << "Value: " << this->value << endl;
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcImageValue *calcImageValue = NULL;
			RSGISCalcImage *calcImage = NULL;
			
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

				
				calcImageValue = new RSGISReplaceValuesLessThanGivenValue(numImgBands, threshold, value);
				
				calcImage = new RSGISCalcImage(calcImageValue, "", true);
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
		else if(option == RSGISExeImageCalculation::unitarea)
		{
			cout << "Converts the image spectra to unit area.\n";
			cout << "Image: " << this->inputImage << endl;
			cout << "Output: " << this->outputImage << endl;
			cout << "Image Bands Matrix: " << this->inMatrixfile << endl;
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcImageValue *calcImageValue = NULL;
			RSGISCalcImage *calcImage = NULL;
			RSGISMatrices matrixUtils;
			
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
				
				calcImageValue = new RSGISConvertSpectralToUnitArea(numImgBands, bandsValuesMatrix);
				
				calcImage = new RSGISCalcImage(calcImageValue, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				
				GDALClose(datasets[0]);
				
				matrixUtils.freeMatrix(bandsValuesMatrix);
				
				delete calcImageValue;
				delete calcImage;
			}
			catch(RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageCalculation::imagemaths)
		{
			cout << "This command performs band maths on each band within an image\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Expression: " << this->mathsExpression << endl;
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISImageMaths *imageMaths = NULL;
			RSGISCalcImage *calcImage = NULL;
			Parser *muParser = new Parser();
			
			try
			{
				datasets = new GDALDataset*[1];
				
				cout << "Open " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
				
				value_type inVal;
				muParser->DefineVar(_T("b1"), &inVal);
				muParser->SetExpr(this->mathsExpression.c_str());
				
				imageMaths = new RSGISImageMaths(numRasterBands, muParser);
				
				calcImage = new RSGISCalcImage(imageMaths, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage, false, NULL, this->imageFormat);
				
				
				GDALClose(datasets[0]);
				delete[] datasets;

				delete muParser;
				delete imageMaths;
				delete calcImage;
			}
			catch(RSGISException e)
			{
				throw e;
			}
			catch (ParserError &e) 
			{
				string message = string("ERROR: ") + string(e.GetMsg()) + string(":\t \'") + string(e.GetExpr()) +string("\'");
				throw RSGISException(message);
			}			
		}
		else if(option == RSGISExeImageCalculation::movementspeed)
		{
			cout << "This command calculates the speed of movment (mean, min and max)\n";
			cout << "Output Image: " << this->outputImage << endl;
			for(int i = 0; i < this->numImages; ++i)
			{
				cout << "Image: " << this->inputImages[i] << " using band " << this->imageBands[i] << " at time " << this->imageTimes[i] << endl;
			}
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcImage *calcImage = NULL;
			RSGISCalculateImageMovementSpeed *calcImageValue = NULL;
			
			try
			{
				datasets = new GDALDataset*[numImages];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				unsigned int *imgBandsInStack = new unsigned int[numImages];
				
				for(int i = 0; i < this->numImages; ++i)
				{
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					imgBandsInStack[i] = totalNumRasterBands + imageBands[i];
					
					cout << "Opened Image: " << this->inputImages[i] << " will be using band " << imgBandsInStack[i] << " in stack." << endl;
					
					if(imageBands[i] > (numRasterBands-1))
					{
						throw RSGISImageException("You have specified a band which is not within the image");
					}
					
					totalNumRasterBands += numRasterBands;
				}
				
				int numOutputBands = ((this->numImages-1)*2) + 3;
                
                RSGISMathsUtils mathUtils;
                string *outBandNames = new string[numOutputBands];
                outBandNames[0] = string("Mean Movement Speed");
                outBandNames[1] = string("Min Movement Speed");
                outBandNames[2] = string("Max Movement Speed");
                
                int idx = 0;
                for(int i = 0; i < this->numImages-1; ++i)
                {
                    idx = (i * 2) + 3;
                    outBandNames[idx] = string("Images ") + mathUtils.inttostring(i+1) + string("-") + mathUtils.inttostring(i+2) + string(" Displacement");
                    outBandNames[idx+1] = string("Images ") + mathUtils.inttostring(i+1) + string("-") + mathUtils.inttostring(i+2) + string(" Movement");
                }
				
				cout << "Number of Output Image bands = " << numOutputBands << endl;
				
				calcImageValue = new RSGISCalculateImageMovementSpeed(numOutputBands, this->numImages, imgBandsInStack, this->imageTimes, upper, lower);
				calcImage = new RSGISCalcImage(calcImageValue, "", true);
				calcImage->calcImage(datasets, this->numImages, this->outputImage, true, outBandNames);
				
				for(int i = 0; i < this->numImages; ++i)
				{
					GDALClose(datasets[i]);
				}
				delete[] datasets;

				delete calcImage;
				delete calcImageValue;
			}
			catch(RSGISException e)
			{
				throw e;
			}
			
		}
		else if(option == RSGISExeImageCalculation::countvalsincol)
		{
			cout << "This command counts the number of values with a give range for each column\n";
			cout << "input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Upper: " << this->upper << endl;
			cout << "lower: " << this->lower << endl;
			
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcImage *calcImage = NULL;
			RSGISCountValsAboveThresInCol *calcImageValue = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				calcImageValue = new RSGISCountValsAboveThresInCol(1, upper, lower);
				calcImage = new RSGISCalcImage(calcImageValue, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				GDALClose(datasets[0]);
				delete[] datasets;
				
				delete calcImage;
				delete calcImageValue;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageCalculation::calcRMSE)
		{
			GDALAllRegister();
			GDALDataset **datasetsA = NULL;
			GDALDataset **datasetsB = NULL;
						
			RSGISCalcImageSingle *calcImgSingle = NULL;
			RSGISCalcRMSE *calculateRSME = NULL;
			double *outRMSE;
			outRMSE = new double[1];
			
			cout << "Calculating RMSE between: " << this->inputImageA << " (Band " << this->inputBandA + 1 << ") and " << this->inputImageB << " (Band " << this->inputBandB + 1 << ")" << endl;
			
			try
			{
				datasetsA = new GDALDataset*[1];
				cout << this->inputImageA << endl;
				datasetsA[0] = (GDALDataset *) GDALOpenShared(this->inputImageA.c_str(), GA_ReadOnly);
				if(datasetsA[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImageA;
					throw RSGISImageException(message.c_str());
				}
				
				
				datasetsB = new GDALDataset*[1];
				cout << this->inputImageB << endl;
				datasetsB[0] = (GDALDataset *) GDALOpenShared(this->inputImageB.c_str(), GA_ReadOnly);
				if(datasetsB[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImageB;
					throw RSGISImageException(message.c_str());
				}
				
				calculateRSME = new RSGISCalcRMSE(1);
				calcImgSingle = new RSGISCalcImageSingle(calculateRSME);
				calcImgSingle->calcImage(datasetsA, datasetsB, 1, outRMSE, this->inputBandA, this->inputBandB);
				
				cout << "RMSE = " << outRMSE[0] << endl;
				
				delete calculateRSME;
				delete[] outRMSE;
				
				if(datasetsA != NULL)
				{
					GDALClose(datasetsA [0]);
					delete[] datasetsA;
				}
				
				if(datasetsB != NULL)
				{
					GDALClose(datasetsB[0]);
					delete[] datasetsB;
				}

				
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageCalculation::apply2VarFunction)
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
			RSGISApplyTwoVarFunction *applyFunction = NULL;
			try
			{
				cout << "Applying two variable function" <<endl;
				applyFunction = new RSGISApplyTwoVarFunction(1, this->twoVarFunction);
				calcImg = new RSGISCalcImage(applyFunction, "", true);
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
			delete applyFunction;
		}
		else if(option == RSGISExeImageCalculation::apply3VarFunction)
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
			RSGISApplyThreeVarFunction *applyFunction = NULL;
			try
			{
				cout << "Applying three variable function" <<endl;
				applyFunction = new RSGISApplyThreeVarFunction(1, this->threeVarFunction);
				calcImg = new RSGISCalcImage(applyFunction, "", true);
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
			delete applyFunction;
		}	
        else if(option == RSGISExeImageCalculation::dist2geoms)
        {
            cout << "A command to calculate the distance to the nearest geometry for each pixel within an image.\n";
            cout << "Input Vector: " << inputVector << endl;
            cout << "Output Image: " << outputImage << endl;
            cout << "Image Resolution: " << imgResolution << endl;
            
            OGRRegisterAll();
			
			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			
			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
            
			OGRSpatialReference* inputSpatialRef = NULL;
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			try
			{
				
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
                inputSpatialRef = inputSHPLayer->GetSpatialRef();
                char **wktString = new char*[1];
                inputSpatialRef->exportToWkt(wktString);
                string projection = string(wktString[0]);
                OGRFree(wktString);
                OGREnvelope ogrExtent;
                inputSHPLayer->GetExtent(&ogrExtent);
                Envelope extent = Envelope(ogrExtent.MinX, ogrExtent.MaxX, ogrExtent.MinY, ogrExtent.MaxY);
				
                // Get Geometries into memory
                vector<OGRGeometry*> *ogrGeoms = new vector<OGRGeometry*>();
                RSGISGetOGRGeometries *getOGRGeoms = new RSGISGetOGRGeometries(ogrGeoms);
                RSGISProcessVector processVector = RSGISProcessVector(getOGRGeoms);
                processVector.processVectorsNoOutput(inputSHPLayer, false);
                delete getOGRGeoms;
                
                // Create Geometry Collection
                OGRGeometryCollection *geomCollectionOrig = new OGRGeometryCollection();
                for(vector<OGRGeometry*>::iterator iterGeoms = ogrGeoms->begin(); iterGeoms != ogrGeoms->end(); ++iterGeoms)
                {
                    geomCollectionOrig->addGeometryDirectly(*iterGeoms);
                }
                
                OGRGeometryCollection *geomCollectionLines = new OGRGeometryCollection();
                RSGISGeometry geomUtils;
                geomUtils.convertGeometryCollection2Lines(geomCollectionOrig, geomCollectionLines);
                
                // Create blank image
                RSGISImageUtils imageUtils;
                GDALDataset *outImage = imageUtils.createBlankImage(this->outputImage, extent, this->imgResolution, 1, projection, 0);
                
                RSGISCalcDist2Geom *dist2GeomCalcValue = new RSGISCalcDist2Geom(1, geomCollectionLines, geomCollectionOrig);
                RSGISCalcEditImage *calcEditImage = new RSGISCalcEditImage(dist2GeomCalcValue);
                calcEditImage->calcImage(outImage);
                
                // Clean up memory.
                delete geomCollectionOrig;
                delete geomCollectionLines;
                delete ogrGeoms;
				OGRDataSource::DestroyDataSource(inputSHPDS);
				OGRCleanupAll();
                GDALClose(outImage);
			}
			catch (RSGISException e) 
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::imagebandstats)
		{
            cout << "A command to calculate the statistics for the individual image bands\n";
            cout << "Input Image: " << inputImage << endl;
            cout << "Output File: " << outputFile << endl;
            if(this->ignoreZeros)
            {
                cout << "Ignoring Zeros\n";
            }
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			RSGISImageStatistics calcImgStats;
			
			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
                
                int numImageBands = datasets[0]->GetRasterCount();
                ImageStats **stats = new ImageStats*[numImageBands];
                for(int i = 0; i < numImageBands; ++i)
                {
                    stats[i] = new ImageStats;
                    stats[i]->min = 0;
                    stats[i]->max = 0;
                    stats[i]->mean = 0;
                    stats[i]->sum = 0;
                    stats[i]->stddev = 0;
                }
				
                calcImgStats.calcImageStatistics(datasets, 1, stats, numImageBands, true, this->ignoreZeros);
                
                ofstream outTxtFile;
                outTxtFile.open(outputFile.c_str());
                outTxtFile.precision(15);
                outTxtFile << "Min,Max,Mean,StdDev,Sum\n";
                for(int i = 0; i < numImageBands; ++i)
                {
                    outTxtFile << stats[i]->min << "," << stats[i]->max << "," << stats[i]->mean << "," << stats[i]->stddev << "," << stats[i]->sum << endl;
                }
                outTxtFile.flush();
                outTxtFile.close();
                
                GDALClose(datasets[0]);
                delete[] datasets;
				
			}
			catch(RSGISException e)
			{
				throw e;
			}
			
        } 
        else if(option == RSGISExeImageCalculation::imagestats)
		{
            cout << "A command to calculate the statistics for the whole image across all bands\n";
            cout << "Input Image: " << inputImage << endl;
            cout << "Output File: " << outputFile << endl;
            if(this->ignoreZeros)
            {
                cout << "Ignoring Zeros\n";
            }
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			RSGISImageStatistics calcImgStats;
			
			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
                
                ImageStats *stats = new ImageStats();
                stats->min = 0;
                stats->max = 0;
                stats->mean = 0;
                stats->sum = 0;
                stats->stddev = 0;
				
                calcImgStats.calcImageStatistics(datasets, 1, stats, true, this->ignoreZeros);
                
                ofstream outTxtFile;
                outTxtFile.open(outputFile.c_str());
                outTxtFile.precision(15);
                outTxtFile << "Min,Max,Mean,StdDev,Sum\n";
                outTxtFile << stats->min << "," << stats->max << "," << stats->mean << "," << stats->stddev << "," << stats->sum << endl;
                outTxtFile.flush();
                outTxtFile.close();
                
                GDALClose(datasets[0]);
                delete[] datasets;
				
			}
			catch(RSGISException e)
			{
				throw e;
			}
			
        } 
        else if(option == RSGISExeImageCalculation::unconlinearspecunmix)
		{
            cout << "A command to undertake an unconstrained linear spectral unmixing of the input image for a set of endmembers\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output File: " << this->outputFile << endl;
            cout << "Endmemebers File: " << this->endmembersFile << endl;
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			RSGISCalcLinearSpectralUnmixing calcSpecUnmix;
			
			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
                
                calcSpecUnmix.performUnconstainedLinearSpectralUnmixing(datasets, 1, this->outputFile, this->endmembersFile);
                
                GDALClose(datasets[0]);
                delete[] datasets;
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::exhconlinearspecunmix)
		{
            cout << "A command to undertake an exhaustive constrained linear spectral unmixing of the input image for a set of endmembers\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output File: " << this->outputFile << endl;
            cout << "Endmemebers File: " << this->endmembersFile << endl;
            cout << "Step Resolution: " << this->stepResolution << endl;
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			RSGISCalcLinearSpectralUnmixing calcSpecUnmix;
			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
                
                calcSpecUnmix.performExhaustiveConstrainedSpectralUnmixing(datasets, 1, this->outputFile, this->endmembersFile, this->stepResolution);
                
                GDALClose(datasets[0]);
                delete[] datasets;
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::kmeanscentres)
        {
            cout << "Generate KMeans cluster centres\n";
            cout << "Input Image: " << inputImage << endl;
            cout << "Output Matrix: " << outputFile << endl;
            cout << "Number of Clusters: " << numClusters << endl;
            cout << "Max Number of Iterations: " << maxNumIterations << endl;
            cout << "Degree of Change: " << degreeOfChange << endl;
            cout << "Sub Sampling: " << subSample << endl;
            
            GDALAllRegister();
			GDALDataset *dataset = NULL;
			
			try
			{
				dataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
                
                RSGISImageClustering imgClustering;
                imgClustering.findKMeansCentres(dataset, outputFile, numClusters, maxNumIterations, subSample, ignoreZeros, degreeOfChange, initClusterMethod);
                
                GDALClose(dataset);
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::isodatacentres)
        {
            cout << "Generate ISOData cluster centres\n";
            cout << "Input Image: " << inputImage << endl;
            cout << "Output Matrix: " << outputFile << endl;
            cout << "Number of Clusters: " << numClusters << endl;
            cout << "Min Number of clusters: " << minNumClusters << endl;
            cout << "Max Number of Iterations: " << maxNumIterations << endl;
            cout << "Degree of Change: " << degreeOfChange << endl;
            cout << "Sub Sampling: " << subSample << endl;
            cout << "Min Number of features: " << minNumFeatures << endl;
            cout << "Min Distance between clusters: " << minDistBetweenClusters << endl;
            cout << "Max Std Dev: " << maxStdDev << endl;
            cout << "Start Iteration: " << startIteration << endl;
            cout << "End Iteration: " << endIteration << endl;
                        
            GDALAllRegister();
			GDALDataset *dataset = NULL;
			
			try
			{
				dataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
                
                RSGISImageClustering imgClustering;
                imgClustering.findISODataCentres(dataset, outputFile, numClusters, maxNumIterations, subSample, ignoreZeros, degreeOfChange, initClusterMethod, minDistBetweenClusters, minNumFeatures, maxStdDev, minNumClusters, startIteration, endIteration);
                
                GDALClose(dataset);
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
		else
		{
			cout << "Options not recognised\n";
		}
	}
}


void RSGISExeImageCalculation::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeImageCalculation::normalise)
		{
			for(int i= 0; i < this->numImages; i++)
			{
				cout << "Input: " << this->inputImages[i] << endl;
				cout << "Output: " << this->outputImages[i] << endl;
			}
			
			if(this->calcInMinMax)
			{
				cout << "The input image minimum and maximum values will be calculated\n";
			}
			else
			{
				cout << "Input Minimum Value is: " << this->inMin << endl;
				cout << "Input Maximum Value is: " << this->inMax << endl;
			}
			
			cout << "Output Minimum Value is: " << this->outMin << endl;
			cout << "Output Maximum Value is: " << this->outMax << endl;
		}
		else if(option == RSGISExeImageCalculation::correlation)
		{
			cout << "Image A: " << this->inputImageA << endl;
			cout << "Image B: " << this->inputImageB << endl;
			cout << "Output Image: " << this->outputMatrix << endl;
		}
		else if(option == RSGISExeImageCalculation::covariance)
		{
			cout << "Image A: " << this->inputImageA << endl;
			cout << "Image B: " << this->inputImageB << endl;
			cout << "Output Image: " << this->outputMatrix << endl;
			if(calcMean)
			{
				cout << "Mean vectors will be calculated\n";
			}
			else
			{
				cout << "Mean Vector (A): " << this->inputMatrixA << endl;
				cout << "Mean Vector (B): " << this->inputMatrixB << endl;
			}
		}
		else if(option == RSGISExeImageCalculation::meanvector)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Matrix: " << this->outputMatrix << endl;
		}
		else if(option == RSGISExeImageCalculation::pca)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Eigenvectors: " << this->eigenvectors << endl;
			cout << "Number of Components: " << this->numComponents << endl;
		}
		else if(option == RSGISExeImageCalculation::standardise)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "MeanVector: " << this->meanvectorStr << endl;
		}
		else if(option == RSGISExeImageCalculation::bandmaths)
		{
			cout << "This command performs band maths\n";
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Expression: " << this->mathsExpression << endl;
			for(int i = 0; i < numVars; ++i)
			{
				cout << i << ")\t name = " << variables[i].name << " image = " << variables[i].image << " band = " << variables[i].bandNum << endl;
			}
		}
		else if(option == RSGISExeImageCalculation::replacevalueslessthan)
		{
			cout << "Replace values less than a threshold with a new value.\n";
			cout << "Image: " << this->inputImage << endl;
			cout << "Output: " << this->outputImage << endl;
			cout << "Threshold: " << this->threshold << endl;
			cout << "Value: " << this->value << endl;
		}
		else if(option == RSGISExeImageCalculation::unitarea)
		{
			cout << "Converts the image spectra to unit area.\n";
			cout << "Image: " << this->inputImage << endl;
			cout << "Output: " << this->outputImage << endl;
			cout << "Image Bands Matrix: " << this->inMatrixfile << endl;
		}
		else if(option == RSGISExeImageCalculation::imagemaths)
		{
			cout << "This command performs band maths on each band within an image\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Expression: " << this->mathsExpression << endl;
		}
		else if(option == RSGISExeImageCalculation::movementspeed)
		{
			cout << "This command calculates the speed of movment (mean, min and max)\n";
			cout << "Output Image: " << this->outputImage << endl;
			for(int i = 0; i < this->numImages; ++i)
			{
				cout << "Image: " << this->inputImages[i] << " using band " << this->imageBands[i] << " at time " << this->imageTimes[i] << endl;
			}
		}
		else if(option == RSGISExeImageCalculation::countvalsincol)
		{
			cout << "This command counts the number of values with a give range for each column\n";
			cout << "input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Upper: " << this->upper << endl;
			cout << "lower: " << this->lower << endl;
		}
        else if(option == RSGISExeImageCalculation::dist2geoms)
        {
            cout << "A command to calculate the distance to the nearest geometry for each pixel within an image.\n";
            cout << "Input Vector: " << inputVector << endl;
            cout << "Output Image: " << outputImage << endl;
            cout << "Image Resolution: " << imgResolution << endl;
        }
        else if(option == RSGISExeImageCalculation::imagebandstats)
		{
            cout << "A command to calculate the overall image stats\n";
            cout << "Input Image: " << inputImage << endl;
            cout << "Output File: " << outputFile << endl;
        }
        else if(option == RSGISExeImageCalculation::unconlinearspecunmix)
		{
            cout << "A command to undertake an unconstrained linear spectral unmixing of the input image for a set of endmembers\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output File: " << this->outputFile << endl;
            cout << "Endmemebers File: " << this->endmembersFile << endl;
        }
        else if(option == RSGISExeImageCalculation::exhconlinearspecunmix)
		{
            cout << "A command to undertake an exhaustive constrained linear spectral unmixing of the input image for a set of endmembers\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output File: " << this->outputFile << endl;
            cout << "Endmemebers File: " << this->endmembersFile << endl;
            cout << "Step Resolution: " << this->stepResolution << endl;
        }
        else if(option == RSGISExeImageCalculation::kmeanscentres)
        {
            cout << "Generate KMeans cluster centres\n";
            cout << "Input Image: " << inputImage << endl;
            cout << "Output Matrix: " << outputFile << endl;
            cout << "Number of Clusters: " << numClusters << endl;
            cout << "Max Number of Iterations: " << maxNumIterations << endl;
            cout << "Degree of Change: " << degreeOfChange << endl;
            cout << "Sub Sampling: " << subSample << endl;
        }
        else if(option == RSGISExeImageCalculation::isodatacentres)
        {
            cout << "Generate ISOData cluster centres\n";
            cout << "Input Image: " << inputImage << endl;
            cout << "Output Matrix: " << outputFile << endl;
            cout << "Number of Clusters: " << numClusters << endl;
            cout << "Max Number of Iterations: " << maxNumIterations << endl;
            cout << "Degree of Change: " << degreeOfChange << endl;
            cout << "Sub Sampling: " << subSample << endl;
            cout << "Min Number of features: " << minNumFeatures << endl;
            cout << "Min Distance between clusters: " << minDistBetweenClusters << endl;
            cout << "Max Std Dev: " << maxStdDev << endl;
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

void RSGISExeImageCalculation::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
    cout << "<!-- A command to normalise an input image to output image pixel values to a given range (the input minimum and maximum as derived from the image) -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"normalise\" image=\"image.env\" output=\"image_out.env\" outmin=\"double\" outmax=\"double\" />" << endl;
    cout << "<!-- A command to normalise an input image to output image pixel values to a given range from the provided input minimum and maximum pixel values. -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"normalise\" image=\"image.env\" output=\"image_out.env\" inmin=\"double\" inmax=\"double\" outmin=\"double\" outmax=\"double\" />" << endl;
    cout << "<!-- A command to normalise a directory of images to output image pixel values to a given range (the input minimum and maximum as derived from the image) -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"normalise\" dir=\"directory\" ext=\"file_extension\" output=\"image_out_base\" outmin=\"double\" outmax=\"double\" />" << endl;
    cout << "<!-- A command to normalise a directory of images to output image pixel values to a given range from the provided input minimum and maximum pixel values. -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"normalise\" dir=\"directory\" ext=\"file_extension\" output=\"image_out_base\" inmin=\"double\" inmax=\"double\" outmin=\"double\" outmax=\"double\" />" << endl;
    cout << "<!-- A command to calculate the correlation between two images -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"correlation\" imageA=\"imageA.env\" imageB=\"imageB.env\" output=\"matrix.mtxt\" />" << endl;
    cout << "<!-- A command to calculate the covariance between two images -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"covariance\" imageA=\"imageA.env\" imageB=\"imageB.env\" output=\"matrix.mtxt\" />" << endl;
    cout << "<!-- A command to calculate the covariance between two images -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"covariance\" imageA=\"imageA.env\" imageB=\"imageB.env\" output=\"matrix.mtxt\" meanA=\"matrixA.mtxt\" meanB=\"matrixB.mtxt\"/>" << endl;
    cout << "<!-- A command to calculate the RMSE between two images -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"calcRMSE\" imageA=\"imageA.env\" imageB=\"imageB.env\" bandA=\"int=1\" bandB=\"int=1\" />" << endl;
    cout << "<!-- A command to calculate the mean vector of an image -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"meanvector\" image=\"image.env\" output=\"matrix.mtxt\" />" << endl;
    cout << "<!-- A command to generate a PCA for an input image based on the provided eigenvectors -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"pca\" image=\"image.env\" output=\"image_out.env\" eigenvectors=\"matrix.mtxt\" components=\"int\" />" << endl;
    cout << "<!-- A command to generate a standardised image using the mean vector provided -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"standardise\" image=\"image.env\" output=\"image_out.env\" meanvector=\"matrix.mtxt\" />" << endl;
    cout << "<!-- A command to undertake band maths operations (band math operations are defined using muparser). -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"bandmaths\" output=\"image_out.env\" format=\"GDAL Format\" expression=\"string\" | expressionFile=\"string\" >" << endl;
    cout << "    <rsgis:variable name=\"string\" image=\"image1\" band=\"int\" />" << endl;
    cout << "    <rsgis:variable name=\"string\" image=\"image2\" band=\"int\" />" << endl;
    cout << "    <rsgis:variable name=\"string\" image=\"image3\" band=\"int\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to apply a single expression to each band of any image (in the expression the band needs to be referred to as 'b1') -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"imagemaths\" image=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" expression=\"string\" | expressionFile=\"string\" >" << endl;
    cout << "<!-- A command to replace values within an image which are below a given value -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"replacevalueslessthan\" image=\"image.env\" output=\"image_out.env\" threshold=\"float\" value=\"float\" />" << endl;
    cout << "<!-- A command to convert the a spectral curve such that it has a unit area (of value 1) -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"unitarea\" image=\"image.env\" output=\"image_out.env\" imagebands=\"matrix.mtxt\"/>" << endl;
    cout << "<!-- A command to calculate mean, min and max speed of movement and displacement-->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"movementspeed\" output=\"image_out.env\" upper=\"float\" lower=\"float\" >" << endl;
    cout << "    <rsgis:image image=\"image1\" band=\"int\" time=\"float\" />" << endl;
    cout << "    <rsgis:image image=\"image2\" band=\"int\" time=\"float\" />" << endl;
    cout << "    <rsgis:image image=\"image3\" band=\"int\" time=\"float\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"countvalsincol\" image=\"image.env\" output=\"image_out.env\" upper=\"float\" lower=\"float\" />" << endl;
    cout << "<!-- A command to calculate the distance from each pixel to nearest geometry -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"dist2geoms\" vector=\"vector.shp\" output=\"image_out.env\" resolution=\"float\" />" << endl;
    cout << "<!-- A command to calculate the statistics for the individual image bands -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"imagebandstats\" image=\"image.env\" output=\"outfile.txt\" ignorezeros=\"yes | no\" />" << endl;
    cout << "<!-- A command to calculate the statistics for the whole image across all bands -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"imagestats\" image=\"image.env\" output=\"outfile.txt\" ignorezeros=\"yes | no\" />" << endl;
    cout << "<!-- A command to undertake an unconstrained linear spectral unmixing of the input image for a set of endmembers -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"unconlinearspecunmix\" image=\"image.env\" output=\"image\" endmembers=\"matrix.mtxt\" />" << endl;
    cout << "<!-- A command to undertake an exhaustive constrained linear spectral unmixing of the input image for a set of endmembers -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"exhconlinearspecunmix\" image=\"image.env\" output=\"image\" endmembers=\"matrix.mtxt\" step=\"float\" />" << endl;
    cout << "<!-- A command to calculate cluster centres for the image using K-Means clustering -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"kmeanscentres\" image=\"image.env\" output=\"matrix.gmtxt\" numclusters=\"int\" maxiterations=\"int\" degreeofchange=\"float\" subsample=\"int\" initmethod=\"random | diagonal_range | diagonal_stddev | diagonal_range_attach | diagonal_stddev_attach | kpp\" ignorezeros=\"yes | no\" />" << endl;
    cout << "<!-- A command to calculate cluster centres for the image using ISOData clustering -->" << endl;
    cout << "<rsgis:command algor=\"imagecalc\" option=\"isodatacentres\" image=\"image.env\" output=\"matrix.gmtxt\" numclusters=\"int\" minnumclusters=\"int\" maxiterations=\"int\" degreeofchange=\"float\" subsample=\"int\" initmethod=\"random | diagonal_range | diagonal_stddev | diagonal_range_attach | diagonal_stddev_attach | kpp\" ignorezeros=\"yes | no\" mindist=\"float\" minnum=\"unsigned int\" maxstddev=\"float\" editstart=\"int\" editend=\"int\" />" << endl;
	cout << "</rsgis:commands>\n";
}

string RSGISExeImageCalculation::getDescription()
{
	return "Interface for performing various image based calculations.";
}

string RSGISExeImageCalculation::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeImageCalculation::~RSGISExeImageCalculation()
{
	
}




