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

namespace rsgisexe{

RSGISExeImageCalculation::RSGISExeImageCalculation() : rsgis::RSGISAlgorithmParameters()
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
    this->imageFormat = "ENVI";
    this->outDataType = GDT_Float32;
}

    rsgis::RSGISAlgorithmParameters* RSGISExeImageCalculation::getInstance()
{
	return new RSGISExeImageCalculation();
}

void RSGISExeImageCalculation::retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException)
{
	rsgis::math::RSGISMathsUtils mathUtils;
	rsgis::utils::RSGISFileUtils fileUtils;
	
	XMLCh *algorName = xercesc::XMLString::transcode(this->algorithm.c_str());
	XMLCh *optionNormalise = xercesc::XMLString::transcode("normalise");
	XMLCh *optionCorrelation = xercesc::XMLString::transcode("correlation");
	XMLCh *optionCovariance = xercesc::XMLString::transcode("covariance");
	XMLCh *optionMeanVector = xercesc::XMLString::transcode("meanvector");
	XMLCh *optionPCA = xercesc::XMLString::transcode("pca");
	XMLCh *optionStandardise = xercesc::XMLString::transcode("standardise");
	XMLCh *optionBandMaths = xercesc::XMLString::transcode("bandmaths");
	XMLCh *optionReplaceValuesLessThan = xercesc::XMLString::transcode("replacevalueslessthan");
	XMLCh *optionUnitArea = xercesc::XMLString::transcode("unitarea");
	XMLCh *optionImageMaths = xercesc::XMLString::transcode("imagemaths");
	XMLCh *optionMovementSpeed = xercesc::XMLString::transcode("movementspeed");
	XMLCh *optionCountValsInCol = xercesc::XMLString::transcode("countvalsincol");
	XMLCh *optionCalcRMSE = xercesc::XMLString::transcode("calcRMSE");
	XMLCh *optionApply2VarFunction = xercesc::XMLString::transcode("apply2VarFunction");
	XMLCh *optionApply3VarFunction = xercesc::XMLString::transcode("apply3VarFunction");
    XMLCh *optionDist2Geoms = xercesc::XMLString::transcode("dist2geoms"); 
    XMLCh *optionImageBandStats = xercesc::XMLString::transcode("imagebandstats"); 
    XMLCh *optionImageStats = xercesc::XMLString::transcode("imagestats"); 
    XMLCh *optionUnconLinearSpecUnmix = xercesc::XMLString::transcode("unconlinearspecunmix"); 
    XMLCh *optionExhConLinearSpecUnmix = xercesc::XMLString::transcode("exhconlinearspecunmix");
    XMLCh *optionConSum1LinearSpecUnmix = xercesc::XMLString::transcode("consum1linearspecunmix"); 
    XMLCh *optionNNConSum1LinearSpecUnmix = xercesc::XMLString::transcode("nnconsum1linearspecunmix");
	XMLCh *optionKMeansCentres = xercesc::XMLString::transcode("kmeanscentres");
    XMLCh *optionISODataCentres = xercesc::XMLString::transcode("isodatacentres");
    XMLCh *optionAllBandsEqualTo = xercesc::XMLString::transcode("allbandsequalto");
    XMLCh *optionHistogram = xercesc::XMLString::transcode("histogram");
    XMLCh *optionBandPercentile = xercesc::XMLString::transcode("bandpercentile");
    XMLCh *optionImgDist2Geoms = xercesc::XMLString::transcode("imgdist2geoms");
    XMLCh *optionImgCalcDist = xercesc::XMLString::transcode("imgcalcdist");
    XMLCh *optionMahalanobisDistWindow = xercesc::XMLString::transcode("mahalanobisdistwindow");
    XMLCh *optionMahalanobisDistImg2Window = xercesc::XMLString::transcode("mahalanobisdistimg2window");
    

	const XMLCh *algorNameEle = argElement->getAttribute(xercesc::XMLString::transcode("algor"));
	if(!xercesc::XMLString::equals(algorName, algorNameEle))
	{
			throw rsgis::RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}

	// Set output image fomat (defaults to ENVI)
	this->imageFormat = "ENVI";
	XMLCh *formatXMLStr = xercesc::XMLString::transcode("format");
	if(argElement->hasAttribute(formatXMLStr))
	{
		char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(formatXMLStr));
		this->imageFormat = std::string(charValue);
		xercesc::XMLString::release(&charValue);
	}
	xercesc::XMLString::release(&formatXMLStr);


    this->outDataType = GDT_Float32;
    this->rsgisOutDataType = rsgis::rsgis_32float;
	XMLCh *datatypeXMLStr = xercesc::XMLString::transcode("datatype");
	if(argElement->hasAttribute(datatypeXMLStr))
	{
        XMLCh *dtByte = xercesc::XMLString::transcode("Byte");
        XMLCh *dtUInt16 = xercesc::XMLString::transcode("UInt16");
        XMLCh *dtInt16 = xercesc::XMLString::transcode("Int16");
        XMLCh *dtUInt32 = xercesc::XMLString::transcode("UInt32");
        XMLCh *dtInt32 = xercesc::XMLString::transcode("Int32");
        XMLCh *dtFloat32 = xercesc::XMLString::transcode("Float32");
        XMLCh *dtFloat64 = xercesc::XMLString::transcode("Float64");
        
        const XMLCh *dtXMLValue = argElement->getAttribute(datatypeXMLStr);
        if(xercesc::XMLString::equals(dtByte, dtXMLValue))
        {
            this->outDataType = GDT_Byte;
            this->rsgisOutDataType = rsgis::rsgis_8int;
        }
        else if(xercesc::XMLString::equals(dtUInt16, dtXMLValue))
        {
            this->outDataType = GDT_UInt16;
            this->rsgisOutDataType = rsgis::rsgis_16uint;
        }
        else if(xercesc::XMLString::equals(dtInt16, dtXMLValue))
        {
            this->outDataType = GDT_Int16;
            this->rsgisOutDataType = rsgis::rsgis_16int;
        }
        else if(xercesc::XMLString::equals(dtUInt32, dtXMLValue))
        {
            this->outDataType = GDT_UInt32;
            this->rsgisOutDataType = rsgis::rsgis_32uint;
        }
        else if(xercesc::XMLString::equals(dtInt32, dtXMLValue))
        {
            this->outDataType = GDT_Int32;
            this->rsgisOutDataType = rsgis::rsgis_32int;
        }
        else if(xercesc::XMLString::equals(dtFloat32, dtXMLValue))
        {
            this->outDataType = GDT_Float32;
            this->rsgisOutDataType = rsgis::rsgis_32float;
        }
        else if(xercesc::XMLString::equals(dtFloat64, dtXMLValue))
        {
            this->outDataType = GDT_Float64;
            this->rsgisOutDataType = rsgis::rsgis_64float;
        }
        else
        {
            std::cerr << "Data type not recognised, defaulting to 32 bit float.";
            this->outDataType = GDT_Float32;
            this->rsgisOutDataType = rsgis::rsgis_32float;
        }
        
        xercesc::XMLString::release(&dtByte);
        xercesc::XMLString::release(&dtUInt16);
        xercesc::XMLString::release(&dtInt16);
        xercesc::XMLString::release(&dtUInt32);
        xercesc::XMLString::release(&dtInt32);
        xercesc::XMLString::release(&dtFloat32);
        xercesc::XMLString::release(&dtFloat64);
	}
	xercesc::XMLString::release(&datatypeXMLStr);

	const XMLCh *optionXML = argElement->getAttribute(xercesc::XMLString::transcode("option"));
	if(xercesc::XMLString::equals(optionNormalise, optionXML))
	{		
		this->option = RSGISExeImageCalculation::normalise;
		
		if(argElement->hasAttribute(xercesc::XMLString::transcode("dir")))
		{
            std::string dirStr = "";
			XMLCh *dirXMLStr = xercesc::XMLString::transcode("dir");
            if(argElement->hasAttribute(dirXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(dirXMLStr));
                dirStr = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'dir\' attribute was provided.");
            }
            xercesc::XMLString::release(&dirXMLStr);
            
            std::string extStr = "";
			XMLCh *extXMLStr = xercesc::XMLString::transcode("ext");
            if(argElement->hasAttribute(extXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(extXMLStr));
                extStr = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'ext\' attribute was provided.");
            }
            xercesc::XMLString::release(&extXMLStr);
            
            
            std::string outputBase = "";
            XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
            if(argElement->hasAttribute(outputXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
                outputBase = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
            }
            xercesc::XMLString::release(&outputXMLStr);
			
			try
			{
				this->inputImages = fileUtils.getDIRList(dirStr, extStr, &this->numImages, false);
			}
			catch(rsgis::RSGISException e)
			{
				throw rsgis::RSGISXMLArgumentsException(e.what());
			}
			this->outputImages = new std::string[this->numImages];
			for(int i = 0; i < this->numImages; i++)
			{
				outputImages[i] = outputBase +  fileUtils.getFileNameNoExtension(inputImages[i]) + std::string("_norm.env");
			}
		}
		else
		{
            XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
            if(argElement->hasAttribute(imageXMLStr))
            {
                this->numImages = 1;
                this->inputImages = new std::string[1];
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
                this->inputImages[0] = std::string(charValue);
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
                this->outputImages = new std::string[1];
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
                this->outputImages[0] = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
            }
            xercesc::XMLString::release(&outputXMLStr);
		}
        
        
        XMLCh *outminXMLStr = xercesc::XMLString::transcode("outmin");
		if(argElement->hasAttribute(outminXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outminXMLStr));
			this->outMin = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'outmin\' attribute was provided.");
		}
		xercesc::XMLString::release(&outminXMLStr);
        
        XMLCh *outmaxXMLStr = xercesc::XMLString::transcode("outmax");
		if(argElement->hasAttribute(outmaxXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outmaxXMLStr));
			this->outMax = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'outmax\' attribute was provided.");
		}
		xercesc::XMLString::release(&outmaxXMLStr);
        
        XMLCh *inmaxXMLStr = xercesc::XMLString::transcode("inmax");
        XMLCh *inminXMLStr = xercesc::XMLString::transcode("inmin");
		if(argElement->hasAttribute(inmaxXMLStr) & argElement->hasAttribute(inminXMLStr))
		{
            char *charValue1 = xercesc::XMLString::transcode(argElement->getAttribute(inmaxXMLStr));
			this->inMin = mathUtils.strtodouble(std::string(charValue1));
			xercesc::XMLString::release(&charValue1);
            
            char *charValue2 = xercesc::XMLString::transcode(argElement->getAttribute(inminXMLStr));
			this->inMax = mathUtils.strtodouble(std::string(charValue2));
			xercesc::XMLString::release(&charValue2);
            
			this->calcInMinMax = false;
		}
		else
		{
			this->calcInMinMax = true;
		}
        xercesc::XMLString::release(&inmaxXMLStr);
        xercesc::XMLString::release(&inminXMLStr);
	}
	else if(xercesc::XMLString::equals(optionCorrelation, optionXML))
	{		
		this->option = RSGISExeImageCalculation::correlation;
		
        XMLCh *imageAXMLStr = xercesc::XMLString::transcode("imageA");
		if(argElement->hasAttribute(imageAXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageAXMLStr));
			this->inputImageA = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'imageA\' attribute was provided.");
		}
		xercesc::XMLString::release(&imageAXMLStr);
        
        
        XMLCh *imageBXMLStr = xercesc::XMLString::transcode("imageB");
		if(argElement->hasAttribute(imageBXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageBXMLStr));
			this->inputImageB = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'imageB\' attribute was provided.");
		}
		xercesc::XMLString::release(&imageBXMLStr);
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputMatrix = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
	}
	else if(xercesc::XMLString::equals(optionCovariance, optionXML))
	{		
		this->option = RSGISExeImageCalculation::covariance;
		
        XMLCh *imageAXMLStr = xercesc::XMLString::transcode("imageA");
		if(argElement->hasAttribute(imageAXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageAXMLStr));
			this->inputImageA = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'imageA\' attribute was provided.");
		}
		xercesc::XMLString::release(&imageAXMLStr);
        
        
        XMLCh *imageBXMLStr = xercesc::XMLString::transcode("imageB");
		if(argElement->hasAttribute(imageBXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageBXMLStr));
			this->inputImageB = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'imageB\' attribute was provided.");
		}
		xercesc::XMLString::release(&imageBXMLStr);
		
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputMatrix = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
		
		if(argElement->hasAttribute(xercesc::XMLString::transcode("meanA")) & argElement->hasAttribute(xercesc::XMLString::transcode("meanB")))
		{
			this->calcMean = false;
			
			const XMLCh *meanA = argElement->getAttribute(xercesc::XMLString::transcode("meanA"));
			this->inputMatrixA = xercesc::XMLString::transcode(meanA);
			
			const XMLCh *meanB = argElement->getAttribute(xercesc::XMLString::transcode("meanB"));
			this->inputMatrixB = xercesc::XMLString::transcode(meanB);
		}
		else
		{
			this->calcMean = true;
		}
		
	}
	else if(xercesc::XMLString::equals(optionMeanVector, optionXML))
	{		
		this->option = RSGISExeImageCalculation::meanvector;

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
			this->outputMatrix = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
	}
	else if(xercesc::XMLString::equals(optionPCA, optionXML))
	{		
		this->option = RSGISExeImageCalculation::pca;
		
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
        
        
        XMLCh *eigenvectorsXMLStr = xercesc::XMLString::transcode("eigenvectors");
		if(argElement->hasAttribute(eigenvectorsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(eigenvectorsXMLStr));
			this->eigenvectors = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'eigenvectors\' attribute was provided.");
		}
		xercesc::XMLString::release(&eigenvectorsXMLStr);
        
        
        XMLCh *componentsXMLStr = xercesc::XMLString::transcode("components");
		if(argElement->hasAttribute(componentsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(componentsXMLStr));
			this->numComponents = mathUtils.strtoint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'components\' attribute was provided.");
		}
		xercesc::XMLString::release(&componentsXMLStr);
        
	}
	else if(xercesc::XMLString::equals(optionStandardise, optionXML))
	{		
		this->option = RSGISExeImageCalculation::standardise;
		
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

        XMLCh *meanVectorXMLStr = xercesc::XMLString::transcode("meanvector");
		if(argElement->hasAttribute(meanVectorXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(meanVectorXMLStr));
			this->meanvectorStr = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'meanvector\' attribute was provided.");
		}
		xercesc::XMLString::release(&meanVectorXMLStr);
	
    }
	else if(xercesc::XMLString::equals(optionBandMaths, optionXML))
	{
		this->option = RSGISExeImageCalculation::bandmaths;
		
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
		
		XMLCh *expressionXMLStr = xercesc::XMLString::transcode("expression");
		XMLCh *expressionFileXMLStr = xercesc::XMLString::transcode("expressionFile");
		if(argElement->hasAttribute(expressionXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(expressionXMLStr));
            std::string muExpression = std::string(charValue);
            boost::algorithm::replace_all(muExpression, "&lt;", "<");
            boost::algorithm::replace_all(muExpression, "&gt;", ">");
            boost::algorithm::replace_all(muExpression, "&ge;", ">=");
            boost::algorithm::replace_all(muExpression, "&le;", "<=");
            
            boost::algorithm::replace_all(muExpression, "lt", "<");
            boost::algorithm::replace_all(muExpression, "gt", ">");
            boost::algorithm::replace_all(muExpression, "ge", ">=");
            boost::algorithm::replace_all(muExpression, "le", "<=");

			this->mathsExpression = muExpression;
			xercesc::XMLString::release(&charValue);
		}
		else if(argElement->hasAttribute(expressionFileXMLStr)) // Get expression from text file
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(expressionFileXMLStr));
            std::string inputExprFileName = std::string(charValue);
            std::string muExpression = "";
			std::ifstream inputExprFile;
			inputExprFile.open(inputExprFileName.c_str());
			if(!inputExprFile.is_open())
			{
				throw rsgis::RSGISXMLArgumentsException("Could not open input expression file.");
			}
			std::string strLine;
			while(!inputExprFile.eof())
			{
				getline(inputExprFile, strLine, '\n');
				muExpression += strLine;
			}
			// Shouldn't need to replace XML specific expressions but try anyway.
            boost::algorithm::replace_all(muExpression, "&lt;", "<");
            boost::algorithm::replace_all(muExpression, "&gt;", ">");
            boost::algorithm::replace_all(muExpression, "&ge;", ">=");
            boost::algorithm::replace_all(muExpression, "&le;", "<=");
            
            boost::algorithm::replace_all(muExpression, "lt", "<");
            boost::algorithm::replace_all(muExpression, "gt", ">");
            boost::algorithm::replace_all(muExpression, "ge", ">=");
            boost::algorithm::replace_all(muExpression, "le", "<=");
			
			// Replace tabs
			boost::algorithm::replace_all(muExpression, "\t", ""); 
			boost::algorithm::replace_all(muExpression, "\n", ""); 
			boost::algorithm::replace_all(muExpression, " ", ""); 
			
			this->mathsExpression = muExpression;
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'expression\' attribute was provided.");
		}
		xercesc::XMLString::release(&expressionXMLStr);
		xercesc::XMLString::release(&expressionFileXMLStr);
		
		xercesc::DOMNodeList *varNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:variable"));
		this->numVars = varNodesList->getLength();		
		
		std::cout << "Found " << this->numVars << " Variables \n";
		
		xercesc::DOMElement *varElement = NULL;
		variables = new rsgis::cmds::VariableStruct[numVars];
		
		for(int i = 0; i < numVars; i++)
		{
			varElement = static_cast<xercesc::DOMElement*>(varNodesList->item(i));
			
			XMLCh *varNameXMLStr = xercesc::XMLString::transcode("name");
			if(varElement->hasAttribute(varNameXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(varElement->getAttribute(varNameXMLStr));
				this->variables[i].name = std::string(charValue);
                if((this-> variables[i].name == "lt") || (this-> variables[i].name == "gt") || (this-> variables[i].name == "le") || (this-> variables[i].name == "ge"))
                {
                    throw rsgis::RSGISXMLArgumentsException("Can't use \'lt\', \'gt\',\'ge\' or \'le\' for variable names");
                }
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			xercesc::XMLString::release(&varNameXMLStr);
			
			XMLCh *varImageXMLStr = xercesc::XMLString::transcode("image");
			if(varElement->hasAttribute(varImageXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(varElement->getAttribute(varImageXMLStr));
				this->variables[i].image = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
			}
			xercesc::XMLString::release(&varImageXMLStr);
			
			XMLCh *varBandXMLStr = xercesc::XMLString::transcode("band");
			if(varElement->hasAttribute(varBandXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(varElement->getAttribute(varBandXMLStr));
				this->variables[i].bandNum = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			xercesc::XMLString::release(&varBandXMLStr);			
		}
	}
	else if (xercesc::XMLString::equals(optionReplaceValuesLessThan, optionXML))
	{		
		this->option = RSGISExeImageCalculation::replacevalueslessthan;
		
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
		
		XMLCh *thresholdXMLStr = xercesc::XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			this->threshold = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
		}
		xercesc::XMLString::release(&thresholdXMLStr);
		
		XMLCh *valueXMLStr = xercesc::XMLString::transcode("value");
		if(argElement->hasAttribute(valueXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(valueXMLStr));
			this->value = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'value\' attribute was provided.");
		}
		xercesc::XMLString::release(&valueXMLStr);
	}
	else if (xercesc::XMLString::equals(optionUnitArea, optionXML))
	{		
		this->option = RSGISExeImageCalculation::unitarea;
		
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
		

	}
	else if(xercesc::XMLString::equals(optionImageMaths, optionXML))
	{
		this->option = RSGISExeImageCalculation::imagemaths;
		
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
		
		XMLCh *expressionXMLStr = xercesc::XMLString::transcode("expression");
		XMLCh *expressionFileXMLStr = xercesc::XMLString::transcode("expressionFile");
		if(argElement->hasAttribute(expressionXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(expressionXMLStr));
            std::string muExpression = std::string(charValue);
            boost::algorithm::replace_all(muExpression, "&lt;", "<");
            boost::algorithm::replace_all(muExpression, "&gt;", ">");
            boost::algorithm::replace_all(muExpression, "&ge;", ">=");
            boost::algorithm::replace_all(muExpression, "&le;", "<=");
            
            boost::algorithm::replace_all(muExpression, "lt", "<");
            boost::algorithm::replace_all(muExpression, "gt", ">");
            boost::algorithm::replace_all(muExpression, "ge", ">=");
            boost::algorithm::replace_all(muExpression, "le", "<=");
			
			this->mathsExpression = muExpression;
			xercesc::XMLString::release(&charValue);
		}
		else if(argElement->hasAttribute(expressionFileXMLStr)) // Get expression from text file
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(expressionFileXMLStr));
			std::string inputExprFileName = std::string(charValue);
            std::string muExpression = "";
			std::ifstream inputExprFile;
			inputExprFile.open(inputExprFileName.c_str());
			if(!inputExprFile.is_open())
			{
				throw rsgis::RSGISXMLArgumentsException("Could not open input expression file.");
			}
			std::string strLine;
			while(!inputExprFile.eof())
			{
				getline(inputExprFile, strLine, '\n');
				muExpression += strLine;
			}
			// Shouldn't need to replace XML specific expressions but try anyway.
            boost::algorithm::replace_all(muExpression, "&lt;", "<");
            boost::algorithm::replace_all(muExpression, "&gt;", ">");
            boost::algorithm::replace_all(muExpression, "&ge;", ">=");
            boost::algorithm::replace_all(muExpression, "&le;", "<=");
            
            boost::algorithm::replace_all(muExpression, "lt", "<");
            boost::algorithm::replace_all(muExpression, "gt", ">");
            boost::algorithm::replace_all(muExpression, "ge", ">=");
            boost::algorithm::replace_all(muExpression, "le", "<=");
			
			// Replace tabs
			boost::algorithm::replace_all(muExpression, "\t", ""); 
			boost::algorithm::replace_all(muExpression, "\n", ""); 
			boost::algorithm::replace_all(muExpression, " ", ""); 
			
			this->mathsExpression = muExpression;
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'expression\' attribute was provided.");
		}
		xercesc::XMLString::release(&expressionXMLStr);
		xercesc::XMLString::release(&expressionFileXMLStr);
	}
	else if(xercesc::XMLString::equals(optionMovementSpeed, optionXML))
	{
		this->option = RSGISExeImageCalculation::movementspeed;
		
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
		
		XMLCh *upperXMLStr = xercesc::XMLString::transcode("upper");
		if(argElement->hasAttribute(upperXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(upperXMLStr));
			this->upper = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'upper\' attribute was provided.");
		}
		xercesc::XMLString::release(&upperXMLStr);
		
		XMLCh *lowerXMLStr = xercesc::XMLString::transcode("lower");
		if(argElement->hasAttribute(lowerXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(lowerXMLStr));
			this->lower = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'lower\' attribute was provided.");
		}
		xercesc::XMLString::release(&lowerXMLStr);
		
		xercesc::DOMNodeList *imgNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:image"));
		this->numImages = imgNodesList->getLength();		
		
		std::cout << "Found " << this->numImages << " Images.\n";
		
		inputImages = new std::string[numImages];
		imageBands = new unsigned int[numImages];
		imageTimes = new float[numImages];
		
		xercesc::DOMElement *imgElement = NULL;
		
		for(int i = 0; i < numImages; ++i)
		{
			imgElement = static_cast<xercesc::DOMElement*>(imgNodesList->item(i));
			
			XMLCh *imgImageXMLStr = xercesc::XMLString::transcode("image");
			if(imgElement->hasAttribute(imgImageXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(imgElement->getAttribute(imgImageXMLStr));
				this->inputImages[i] = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
			}
			xercesc::XMLString::release(&imgImageXMLStr);
			
			XMLCh *imgBandXMLStr = xercesc::XMLString::transcode("band");
			if(imgElement->hasAttribute(imgBandXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(imgElement->getAttribute(imgBandXMLStr));
				this->imageBands[i] = mathUtils.strtoint(std::string(charValue))-1;
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			xercesc::XMLString::release(&imgBandXMLStr);
			
			XMLCh *imgTimeXMLStr = xercesc::XMLString::transcode("time");
			if(imgElement->hasAttribute(imgTimeXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(imgElement->getAttribute(imgTimeXMLStr));
				this->imageTimes[i] = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'time\' attribute was provided.");
			}
			xercesc::XMLString::release(&imgTimeXMLStr);	
		}
	}
	else if(xercesc::XMLString::equals(optionCountValsInCol, optionXML))
	{
		this->option = RSGISExeImageCalculation::countvalsincol;
		
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
		
		XMLCh *upperXMLStr = xercesc::XMLString::transcode("upper");
		if(argElement->hasAttribute(upperXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(upperXMLStr));
			this->upper = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'upper\' attribute was provided.");
		}
		xercesc::XMLString::release(&upperXMLStr);
		
		XMLCh *lowerXMLStr = xercesc::XMLString::transcode("lower");
		if(argElement->hasAttribute(lowerXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(lowerXMLStr));
			this->lower = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'lower\' attribute was provided.");
		}
		xercesc::XMLString::release(&lowerXMLStr);
	}
	else if(xercesc::XMLString::equals(optionCalcRMSE, optionXML))
	{
		
		this->option = RSGISExeImageCalculation::calcRMSE;
		
		XMLCh *imageAXMLStr = xercesc::XMLString::transcode("imageA");
		if(argElement->hasAttribute(imageAXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageAXMLStr));
			this->inputImageA = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("imageA not provided");
		}
		xercesc::XMLString::release(&imageAXMLStr);
		
		XMLCh *imageBXMLStr = xercesc::XMLString::transcode("imageB");
		if(argElement->hasAttribute(imageBXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageBXMLStr));
			this->inputImageB = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("imageB not provided");
		}
		xercesc::XMLString::release(&imageBXMLStr);
		
		XMLCh *bandAXMLStr = xercesc::XMLString::transcode("bandA");
		if(argElement->hasAttribute(bandAXMLStr)) 
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandAXMLStr));
			this->inputBandA = mathUtils.strtoint(std::string(charValue)) - 1;
			if (this->inputBandA < 0) 
			{
				std::cout << "\tBand numbering starts at 1, assuming first band for image A" << std::endl;
				this->inputBandA = 0;
			}
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->inputBandA = 0;
			std::cout << "\tNo band set for image A, using default of band 1" << std::endl;
		}
		xercesc::XMLString::release(&bandAXMLStr);
		
		XMLCh *bandBXMLStr = xercesc::XMLString::transcode("bandB");
		if(argElement->hasAttribute(bandBXMLStr)) 
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandBXMLStr));
			this->inputBandB = mathUtils.strtoint(std::string(charValue)) - 1;
			if (this->inputBandB < 0) 
			{
				std::cout << "\tBand numbering starts at 1, assuming first band for image B" << std::endl;
				this->inputBandB = 0;
			}
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->inputBandB = 0;
			std::cout << "\tNo band set for image B, using default of band 1" << std::endl;
		}
		xercesc::XMLString::release(&bandBXMLStr);
		
	}
	else if(xercesc::XMLString::equals(optionApply2VarFunction,optionXML))
	{		
		this->option = RSGISExeImageCalculation::apply2VarFunction;
		
		rsgis::math::RSGISMatrices matrixUtils;
		
		XMLCh *imageXMLStr = xercesc::XMLString::transcode("input");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'input\' attribute was provided.");
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
		
		// Get function to use.
		// The XML interface is available for a number of functions within the RSGISExeEstimationAlgorithm.
		XMLCh *function2DPoly = xercesc::XMLString::transcode("2DPoly");
		const XMLCh *functionStr = argElement->getAttribute(xercesc::XMLString::transcode("function"));
		
		if (xercesc::XMLString::equals(function2DPoly,functionStr))
		{
			// Read coefficients
			XMLCh *coefficients = xercesc::XMLString::transcode("coefficients");
			if(argElement->hasAttribute(coefficients))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(coefficients));
				std::string coeffFile = std::string(charValue);
				this->coeffMatrix = matrixUtils.readGSLMatrixFromTxt(coeffFile);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No coefficents provided for function");
			}
			xercesc::XMLString::release(&coefficients);
			
			this->twoVarFunction = (rsgis::math::RSGISMathTwoVariableFunction *) new rsgis::math::RSGISFunction2DPoly(coeffMatrix);
		}
		else 
		{
			throw rsgis::RSGISXMLArgumentsException("Unknown function");
 		}

		xercesc::XMLString::release(&function2DPoly);
	}
	else if(xercesc::XMLString::equals(optionApply3VarFunction,optionXML))
	{		
		this->option = RSGISExeImageCalculation::apply3VarFunction;
		
		rsgis::math::RSGISMatrices matrixUtils;
		
		XMLCh *imageXMLStr = xercesc::XMLString::transcode("input");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'input\' attribute was provided.");
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
		
		// Get function to use.
		// The XML interface is available for a number of functions within the RSGISExeEstimationAlgorithm.
		XMLCh *function3DPoly = xercesc::XMLString::transcode("3DPoly");
		const XMLCh *functionStr = argElement->getAttribute(xercesc::XMLString::transcode("function"));
		
		if (xercesc::XMLString::equals(function3DPoly,functionStr))
		{
			// Read coefficients
			XMLCh *coefficients = xercesc::XMLString::transcode("coefficients");
			if(argElement->hasAttribute(coefficients))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(coefficients));
				std::string coeffFile = std::string(charValue);
				this->coeffMatrix = matrixUtils.readGSLMatrixFromTxt(coeffFile);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No coefficents provided for function");
			}
			xercesc::XMLString::release(&coefficients);
			
			bool sameOrderBothFits = false;
			XMLCh *polyOrderStr = xercesc::XMLString::transcode("polyOrder"); // Polynomial Order
			if(argElement->hasAttribute(polyOrderStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(polyOrderStr));
				int orderInt = mathUtils.strtoint(std::string(charValue));
				std::cout << "\tusing same order polynomial for both sets of fits, use \'polyOrderX\', \'polyOrderY\' and \'polyOrderZ\' to set seperately" << std::endl;
				sameOrderBothFits = true;
				this->polyOrderX = orderInt + 1;
				this->polyOrderY = orderInt + 1;
				this->polyOrderZ = orderInt + 1;
				xercesc::XMLString::release(&charValue);
			}
			xercesc::XMLString::release(&polyOrderStr);
			XMLCh *polyOrderXStr = xercesc::XMLString::transcode("polyOrderX"); // Polynomial Order
			if(argElement->hasAttribute(polyOrderXStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(polyOrderXStr));
				int orderInt = mathUtils.strtoint(std::string(charValue));
				this->polyOrderX = orderInt + 1;
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				if (!sameOrderBothFits) 
				{
					throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for x fits");
				}
			}
			xercesc::XMLString::release(&polyOrderXStr);
			XMLCh *polyOrderYStr = xercesc::XMLString::transcode("polyOrderY"); // Polynomial Order
			if(argElement->hasAttribute(polyOrderYStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(polyOrderYStr));
				int orderInt = mathUtils.strtoint(std::string(charValue));
				this->polyOrderY = orderInt + 1;
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				if (!sameOrderBothFits) 
				{
					throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for y fits");
				}
			}
			xercesc::XMLString::release(&polyOrderYStr);
			XMLCh *polyOrderZStr = xercesc::XMLString::transcode("polyOrderZ"); // Polynomial Order
			if(argElement->hasAttribute(polyOrderZStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(polyOrderZStr));
				int orderInt = mathUtils.strtoint(std::string(charValue));
				this->polyOrderZ = orderInt + 1;
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				if (!sameOrderBothFits) 
				{
					throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order for z fits");
				}
			}
			xercesc::XMLString::release(&polyOrderZStr);
			
			this->threeVarFunction = (rsgis::math::RSGISMathThreeVariableFunction *) new rsgis::math::RSGISFunction3DPoly(coeffMatrix, polyOrderX, polyOrderY, polyOrderZ);
		}
		else 
		{
			throw rsgis::RSGISXMLArgumentsException("Unknown function");
 		}
		
		xercesc::XMLString::release(&function3DPoly);
	}
    else if(xercesc::XMLString::equals(optionDist2Geoms, optionXML))
	{
		this->option = RSGISExeImageCalculation::dist2geoms;
		
        XMLCh *vectorXMLStr = xercesc::XMLString::transcode("vector");
		if(argElement->hasAttribute(vectorXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(vectorXMLStr));
			this->inputVector = std::string(charValue);
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
			this->outputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
		
        XMLCh *resolutionXMLStr = xercesc::XMLString::transcode("resolution");
		if(argElement->hasAttribute(resolutionXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(resolutionXMLStr));
			this->imgResolution = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'resolution\' attribute was provided.");
		}
		xercesc::XMLString::release(&resolutionXMLStr);
		
	}
    else if(xercesc::XMLString::equals(optionImageBandStats, optionXML))
	{		
		this->option = RSGISExeImageCalculation::imagebandstats;
		
        
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
			this->outputFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *ignoreZerosXMLStr = xercesc::XMLString::transcode("ignorezeros");
		if(argElement->hasAttribute(ignoreZerosXMLStr))
		{
            XMLCh *noStr = xercesc::XMLString::transcode("no");
			const XMLCh *ignoreValue = argElement->getAttribute(ignoreZerosXMLStr);
			if(xercesc::XMLString::equals(ignoreValue, noStr))
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
			std::cerr << "No \'ignorezeros\' attribute was provided so defaulting to ignore zeros.\n";
            this->ignoreZeros = true;
		}
		xercesc::XMLString::release(&ignoreZerosXMLStr);
        
	}
    else if(xercesc::XMLString::equals(optionImageStats, optionXML))
	{		
		this->option = RSGISExeImageCalculation::imagestats;
		
        
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
			this->outputFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *ignoreZerosXMLStr = xercesc::XMLString::transcode("ignorezeros");
		if(argElement->hasAttribute(ignoreZerosXMLStr))
		{
            XMLCh *noStr = xercesc::XMLString::transcode("no");
			const XMLCh *ignoreValue = argElement->getAttribute(ignoreZerosXMLStr);
			if(xercesc::XMLString::equals(ignoreValue, noStr))
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
			std::cerr << "No \'ignorezeros\' attribute was provided so defaulting to ignore zeros.\n";
            this->ignoreZeros = true;
		}
		xercesc::XMLString::release(&ignoreZerosXMLStr);
        
	}
    else if(xercesc::XMLString::equals(optionUnconLinearSpecUnmix, optionXML))
	{		
		this->option = RSGISExeImageCalculation::unconlinearspecunmix;
		
        
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
			this->outputFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *endmembersXMLStr = xercesc::XMLString::transcode("endmembers");
		if(argElement->hasAttribute(endmembersXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(endmembersXMLStr));
			this->endmembersFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'endmembers\' attribute was provided.");
		}
		xercesc::XMLString::release(&endmembersXMLStr);
        
        XMLCh *gainXMLStr = xercesc::XMLString::transcode("gain");
		if(argElement->hasAttribute(gainXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(gainXMLStr));
			this->lsumGain = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->lsumGain = 1;
		}
		xercesc::XMLString::release(&gainXMLStr);
        
        XMLCh *offsetXMLStr = xercesc::XMLString::transcode("offset");
		if(argElement->hasAttribute(offsetXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(offsetXMLStr));
			this->lsumOffset = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->lsumOffset = 0;
		}
		xercesc::XMLString::release(&offsetXMLStr);
	}
    else if(xercesc::XMLString::equals(optionExhConLinearSpecUnmix, optionXML))
	{		
		this->option = RSGISExeImageCalculation::exhconlinearspecunmix;
		
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
			this->outputFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *endmembersXMLStr = xercesc::XMLString::transcode("endmembers");
		if(argElement->hasAttribute(endmembersXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(endmembersXMLStr));
			this->endmembersFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'endmembers\' attribute was provided.");
		}
		xercesc::XMLString::release(&endmembersXMLStr);
        
        
        XMLCh *stepXMLStr = xercesc::XMLString::transcode("step");
		if(argElement->hasAttribute(stepXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(stepXMLStr));
			this->stepResolution = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'step\' attribute was provided.");
		}
		xercesc::XMLString::release(&stepXMLStr);
        
        XMLCh *gainXMLStr = xercesc::XMLString::transcode("gain");
		if(argElement->hasAttribute(gainXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(gainXMLStr));
			this->lsumGain = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->lsumGain = 1;
		}
		xercesc::XMLString::release(&gainXMLStr);
        
        XMLCh *offsetXMLStr = xercesc::XMLString::transcode("offset");
		if(argElement->hasAttribute(offsetXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(offsetXMLStr));
			this->lsumOffset = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->lsumOffset = 0;
		}
		xercesc::XMLString::release(&offsetXMLStr);
	}
    else if(xercesc::XMLString::equals(optionConSum1LinearSpecUnmix, optionXML))
	{		
		this->option = RSGISExeImageCalculation::consum1linearspecunmix;
		
        
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
			this->outputFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *endmembersXMLStr = xercesc::XMLString::transcode("endmembers");
		if(argElement->hasAttribute(endmembersXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(endmembersXMLStr));
			this->endmembersFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'endmembers\' attribute was provided.");
		}
		xercesc::XMLString::release(&endmembersXMLStr);
        
        XMLCh *weightXMLStr = xercesc::XMLString::transcode("weight");
		if(argElement->hasAttribute(weightXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(weightXMLStr));
			this->lsumWeight = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'weight\' attribute was provided.");
		}
		xercesc::XMLString::release(&weightXMLStr);
        
        XMLCh *gainXMLStr = xercesc::XMLString::transcode("gain");
		if(argElement->hasAttribute(gainXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(gainXMLStr));
			this->lsumGain = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->lsumGain = 1;
		}
		xercesc::XMLString::release(&gainXMLStr);
        
        XMLCh *offsetXMLStr = xercesc::XMLString::transcode("offset");
		if(argElement->hasAttribute(offsetXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(offsetXMLStr));
			this->lsumOffset = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->lsumOffset = 0;
		}
		xercesc::XMLString::release(&offsetXMLStr);
	}
    else if(xercesc::XMLString::equals(optionNNConSum1LinearSpecUnmix, optionXML))
	{		
		this->option = RSGISExeImageCalculation::nnconsum1linearspecunmix;
		
        
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
			this->outputFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *endmembersXMLStr = xercesc::XMLString::transcode("endmembers");
		if(argElement->hasAttribute(endmembersXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(endmembersXMLStr));
			this->endmembersFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'endmembers\' attribute was provided.");
		}
		xercesc::XMLString::release(&endmembersXMLStr);
        
        XMLCh *weightXMLStr = xercesc::XMLString::transcode("weight");
		if(argElement->hasAttribute(weightXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(weightXMLStr));
			this->lsumWeight = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'weight\' attribute was provided.");
		}
		xercesc::XMLString::release(&weightXMLStr);
        
        XMLCh *gainXMLStr = xercesc::XMLString::transcode("gain");
		if(argElement->hasAttribute(gainXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(gainXMLStr));
			this->lsumGain = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->lsumGain = 1;
		}
		xercesc::XMLString::release(&gainXMLStr);
        
        XMLCh *offsetXMLStr = xercesc::XMLString::transcode("offset");
		if(argElement->hasAttribute(offsetXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(offsetXMLStr));
			this->lsumOffset = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->lsumOffset = 0;
		}
		xercesc::XMLString::release(&offsetXMLStr);
	}
    else if(xercesc::XMLString::equals(optionKMeansCentres, optionXML))
	{		
		this->option = RSGISExeImageCalculation::kmeanscentres;
		
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
			this->outputFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *numClustersXMLStr = xercesc::XMLString::transcode("numclusters");
		if(argElement->hasAttribute(numClustersXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(numClustersXMLStr));
			this->numClusters = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'numclusters\' attribute was provided.");
		}
		xercesc::XMLString::release(&numClustersXMLStr);
        
        
        XMLCh *maxIterationsXMLStr = xercesc::XMLString::transcode("maxiterations");
		if(argElement->hasAttribute(maxIterationsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
			this->maxNumIterations = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'maxiterations\' attribute was provided.");
		}
		xercesc::XMLString::release(&maxIterationsXMLStr);
        
        
        XMLCh *degreeOfChangeXMLStr = xercesc::XMLString::transcode("degreeofchange");
		if(argElement->hasAttribute(degreeOfChangeXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(degreeOfChangeXMLStr));
			this->degreeOfChange = mathUtils.strtofloat(std::string(charValue))/100;
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'degreeofchange\' attribute was provided.");
		}
		xercesc::XMLString::release(&degreeOfChangeXMLStr);
        
        XMLCh *subsampleXMLStr = xercesc::XMLString::transcode("subsample");
		if(argElement->hasAttribute(subsampleXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(subsampleXMLStr));
			this->subSample = mathUtils.strtounsignedint(std::string(charValue));
            if(this->subSample == 0)
            {
                std::cerr << "Warning: SubSample must have a value of at least 1. Value updated to 1.\n";
                this->subSample = 1;
            }
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'subsample\' attribute was provided.");
		}
		xercesc::XMLString::release(&subsampleXMLStr);
        
        XMLCh *initMethodXMLStr = xercesc::XMLString::transcode("initmethod");
		if(argElement->hasAttribute(initMethodXMLStr))
		{
            XMLCh *randomStr = xercesc::XMLString::transcode("random");
            XMLCh *diagonalRangeStr = xercesc::XMLString::transcode("diagonal_range");
            XMLCh *diagonalStdDevStr = xercesc::XMLString::transcode("diagonal_stddev");
            XMLCh *diagonalRangeAttachStr = xercesc::XMLString::transcode("diagonal_range_attach");
            XMLCh *diagonalStdDevAttachStr = xercesc::XMLString::transcode("diagonal_stddev_attach");
            XMLCh *kppStr = xercesc::XMLString::transcode("kpp");
			const XMLCh *strValue = argElement->getAttribute(initMethodXMLStr);
			
			if(xercesc::XMLString::equals(strValue, randomStr))
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_random;
			}
            else if(xercesc::XMLString::equals(strValue, diagonalRangeStr))
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_diagonal_full;
			}
            else if(xercesc::XMLString::equals(strValue, diagonalStdDevStr))
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_diagonal_stddev;
			}
            else if(xercesc::XMLString::equals(strValue, diagonalRangeAttachStr))
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_diagonal_full_attach;
			}
            else if(xercesc::XMLString::equals(strValue, diagonalStdDevAttachStr))
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_diagonal_stddev_attach;
			}
            else if(xercesc::XMLString::equals(strValue, kppStr))
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_kpp;
			}
			else
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_random;
                std::cerr << "The initial cluster method was not reconised so random method is being used\n";
			}
			xercesc::XMLString::release(&randomStr);
            xercesc::XMLString::release(&diagonalRangeStr);
            xercesc::XMLString::release(&diagonalStdDevStr);
            xercesc::XMLString::release(&diagonalRangeAttachStr);
            xercesc::XMLString::release(&diagonalStdDevAttachStr);
            xercesc::XMLString::release(&kppStr);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'initmethod\' attribute was provided.");
		}
		xercesc::XMLString::release(&initMethodXMLStr);
        
        
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
	}
    else if(xercesc::XMLString::equals(optionISODataCentres, optionXML))
	{		
		this->option = RSGISExeImageCalculation::isodatacentres;
		
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
			this->outputFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *numClustersXMLStr = xercesc::XMLString::transcode("numclusters");
		if(argElement->hasAttribute(numClustersXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(numClustersXMLStr));
			this->numClusters = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'numclusters\' attribute was provided.");
		}
		xercesc::XMLString::release(&numClustersXMLStr);
        
        XMLCh *maxIterationsXMLStr = xercesc::XMLString::transcode("maxiterations");
		if(argElement->hasAttribute(maxIterationsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
			this->maxNumIterations = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'maxiterations\' attribute was provided.");
		}
		xercesc::XMLString::release(&maxIterationsXMLStr);
        
        
        XMLCh *degreeOfChangeXMLStr = xercesc::XMLString::transcode("degreeofchange");
		if(argElement->hasAttribute(degreeOfChangeXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(degreeOfChangeXMLStr));
			this->degreeOfChange = mathUtils.strtofloat(std::string(charValue))/100;
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'degreeofchange\' attribute was provided.");
		}
		xercesc::XMLString::release(&degreeOfChangeXMLStr);
        
        XMLCh *subsampleXMLStr = xercesc::XMLString::transcode("subsample");
		if(argElement->hasAttribute(subsampleXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(subsampleXMLStr));
			this->subSample = mathUtils.strtounsignedint(std::string(charValue));
            if(this->subSample == 0)
            {
                std::cerr << "Warning: SubSample must have a value of at least 1. Value updated to 1.\n";
                this->subSample = 1;
            }
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'subsample\' attribute was provided.");
		}
		xercesc::XMLString::release(&subsampleXMLStr);
        
        XMLCh *initMethodXMLStr = xercesc::XMLString::transcode("initmethod");
		if(argElement->hasAttribute(initMethodXMLStr))
		{
            XMLCh *randomStr = xercesc::XMLString::transcode("random");
            XMLCh *diagonalRangeStr = xercesc::XMLString::transcode("diagonal_range");
            XMLCh *diagonalStdDevStr = xercesc::XMLString::transcode("diagonal_stddev");
            XMLCh *diagonalRangeAttachStr = xercesc::XMLString::transcode("diagonal_range_attach");
            XMLCh *diagonalStdDevAttachStr = xercesc::XMLString::transcode("diagonal_stddev_attach");
            XMLCh *kppStr = xercesc::XMLString::transcode("kpp");
			const XMLCh *strValue = argElement->getAttribute(initMethodXMLStr);
			
			if(xercesc::XMLString::equals(strValue, randomStr))
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_random;
			}
            else if(xercesc::XMLString::equals(strValue, diagonalRangeStr))
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_diagonal_full;
			}
            else if(xercesc::XMLString::equals(strValue, diagonalStdDevStr))
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_diagonal_stddev;
			}
            else if(xercesc::XMLString::equals(strValue, diagonalRangeAttachStr))
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_diagonal_full_attach;
			}
            else if(xercesc::XMLString::equals(strValue, diagonalStdDevAttachStr))
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_diagonal_stddev_attach;
			}
            else if(xercesc::XMLString::equals(strValue, kppStr))
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_kpp;
			}
			else
			{
				this->initClusterMethod = rsgis::cmds::rsgis_init_random;
                std::cerr << "The initial cluster method was not reconised so random method is being used\n";
			}
			xercesc::XMLString::release(&randomStr);
            xercesc::XMLString::release(&diagonalRangeStr);
            xercesc::XMLString::release(&diagonalStdDevStr);
            xercesc::XMLString::release(&diagonalRangeAttachStr);
            xercesc::XMLString::release(&diagonalStdDevAttachStr);
            xercesc::XMLString::release(&kppStr);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'initmethod\' attribute was provided.");
		}
		xercesc::XMLString::release(&initMethodXMLStr);
        
        
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
                
        XMLCh *minNumXMLStr = xercesc::XMLString::transcode("minnum");
		if(argElement->hasAttribute(minNumXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(minNumXMLStr));
			this->minNumFeatures = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'minnum\' attribute was provided.");
		}
		xercesc::XMLString::release(&minNumXMLStr);
        
        XMLCh *minDistXMLStr = xercesc::XMLString::transcode("mindist");
		if(argElement->hasAttribute(minDistXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(minDistXMLStr));
			this->minDistBetweenClusters = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'mindist\' attribute was provided.");
		}
		xercesc::XMLString::release(&minDistXMLStr);
        
        XMLCh *maxStdDevXMLStr = xercesc::XMLString::transcode("maxstddev");
		if(argElement->hasAttribute(maxStdDevXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maxStdDevXMLStr));
			this->maxStdDev = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'maxstddev\' attribute was provided.");
		}
		xercesc::XMLString::release(&maxStdDevXMLStr);
        
        XMLCh *minNumClustersXMLStr = xercesc::XMLString::transcode("minnumclusters");
		if(argElement->hasAttribute(minNumClustersXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(minNumClustersXMLStr));
			this->minNumClusters = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'minnumclusters\' attribute was provided.");
		}
		xercesc::XMLString::release(&minNumClustersXMLStr);
        
        
        XMLCh *editStartXMLStr = xercesc::XMLString::transcode("editstart");
		if(argElement->hasAttribute(editStartXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(editStartXMLStr));
			this->startIteration = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'editstart\' attribute was provided.");
		}
		xercesc::XMLString::release(&editStartXMLStr);
        
        XMLCh *editEndXMLStr = xercesc::XMLString::transcode("editend");
		if(argElement->hasAttribute(editEndXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(editEndXMLStr));
			this->endIteration = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'editend\' attribute was provided.");
		}
		xercesc::XMLString::release(&editEndXMLStr);
	}
    else if(xercesc::XMLString::equals(optionAllBandsEqualTo, optionXML))
    {
        this->option = RSGISExeImageCalculation::allbandsequalto;
		
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
        
        XMLCh *valueXMLStr = xercesc::XMLString::transcode("value");
		if(argElement->hasAttribute(valueXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(valueXMLStr));
			this->imgValue = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'value\' attribute was provided.");
		}
		xercesc::XMLString::release(&valueXMLStr);
        
        XMLCh *trueOutXMLStr = xercesc::XMLString::transcode("trueout");
		if(argElement->hasAttribute(trueOutXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(trueOutXMLStr));
			this->outputTrueVal = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'trueout\' attribute was provided.");
		}
		xercesc::XMLString::release(&trueOutXMLStr);
        
        XMLCh *falseOutXMLStr = xercesc::XMLString::transcode("falseout");
		if(argElement->hasAttribute(falseOutXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(falseOutXMLStr));
			this->outputFalseVal = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'falseout\' attribute was provided.");
		}
		xercesc::XMLString::release(&falseOutXMLStr);
    }
    else if(xercesc::XMLString::equals(optionHistogram, optionXML))
    {
        this->option = RSGISExeImageCalculation::histogram;
		
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
			this->outputFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *maskXMLStr = xercesc::XMLString::transcode("mask");
		if(argElement->hasAttribute(maskXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maskXMLStr));
			this->imageMask = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'mask\' attribute was provided.");
		}
		xercesc::XMLString::release(&maskXMLStr);
        
        XMLCh *valueXMLStr = xercesc::XMLString::transcode("value");
		if(argElement->hasAttribute(valueXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(valueXMLStr));
			this->imgValue = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'value\' attribute was provided.");
		}
		xercesc::XMLString::release(&valueXMLStr);
        
        XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
		if(argElement->hasAttribute(bandXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandXMLStr));
			this->imgBand = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
		}
		xercesc::XMLString::release(&bandXMLStr);
        
        calcInMinMax = false;
        
        XMLCh *minXMLStr = xercesc::XMLString::transcode("min");
		if(argElement->hasAttribute(minXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(minXMLStr));
			this->inMin = mathUtils.strtoint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			calcInMinMax = true;
            std::cerr << "Calculating the min and max values of the input image\n";
		}
		xercesc::XMLString::release(&minXMLStr);
        
        XMLCh *maxXMLStr = xercesc::XMLString::transcode("max");
		if(argElement->hasAttribute(maxXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maxXMLStr));
			this->inMax = mathUtils.strtoint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			calcInMinMax = true;
            std::cerr << "Calculating the min and max values of the input image\n";
		}
		xercesc::XMLString::release(&maxXMLStr);
        
        XMLCh *widthXMLStr = xercesc::XMLString::transcode("width");
		if(argElement->hasAttribute(widthXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(widthXMLStr));
			this->binWidth = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->binWidth = 1;
            std::cerr << "No bin width provided, defaulting to a value of 1.\n";
		}
		xercesc::XMLString::release(&widthXMLStr);
    }
    else if(xercesc::XMLString::equals(optionBandPercentile, optionXML))
    {
        this->option = RSGISExeImageCalculation::bandpercentile;
		
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
			this->outputFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        this->noDataValueSpecified = false;
        XMLCh *valueXMLStr = xercesc::XMLString::transcode("nodata");
		if(argElement->hasAttribute(valueXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(valueXMLStr));
			this->noDataValue = mathUtils.strtofloat(std::string(charValue));
            this->noDataValueSpecified = true;
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->noDataValueSpecified = false;
		}
		xercesc::XMLString::release(&valueXMLStr);
        
        XMLCh *percentileXMLStr = xercesc::XMLString::transcode("percentile");
		if(argElement->hasAttribute(percentileXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(percentileXMLStr));
			this->percentile = mathUtils.strtofloat(std::string(charValue));
            if((percentile < 0) | (percentile > 1))
            {
                throw rsgis::RSGISXMLArgumentsException("The percentile value needs to be between 0 and 1.");
            }
			xercesc::XMLString::release(&charValue);
		}
		else
		{
            throw rsgis::RSGISXMLArgumentsException("No \'percentile\' attribute was provided.");
		}
		xercesc::XMLString::release(&percentileXMLStr);
    }
    else if(xercesc::XMLString::equals(optionImgDist2Geoms, optionXML))
	{
		this->option = RSGISExeImageCalculation::imgdist2geoms;
		
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
        
        XMLCh *vectorXMLStr = xercesc::XMLString::transcode("vector");
		if(argElement->hasAttribute(vectorXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(vectorXMLStr));
			this->inputVector = std::string(charValue);
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
			this->outputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
	}
    else if(xercesc::XMLString::equals(optionImgCalcDist, optionXML))
	{
		this->option = RSGISExeImageCalculation::imgcalcdist;
		
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
	}
    else if(xercesc::XMLString::equals(optionMahalanobisDistWindow, optionXML))
	{
		this->option = RSGISExeImageCalculation::mahalanobisdistwindow;
		
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
        
        XMLCh *windowXMLStr = xercesc::XMLString::transcode("window");
		if(argElement->hasAttribute(windowXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(windowXMLStr));
			this->windowSize = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'window\' attribute was provided.");
		}
		xercesc::XMLString::release(&windowXMLStr);
	}
    else if(xercesc::XMLString::equals(optionMahalanobisDistImg2Window, optionXML))
	{
		this->option = RSGISExeImageCalculation::mahalanobisdistimg2window;
		
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
        
        XMLCh *windowXMLStr = xercesc::XMLString::transcode("window");
		if(argElement->hasAttribute(windowXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(windowXMLStr));
			this->windowSize = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'window\' attribute was provided.");
		}
		xercesc::XMLString::release(&windowXMLStr);
	}
	else
	{
		std::string message = std::string("The option (") + std::string(xercesc::XMLString::transcode(optionXML)) + std::string(") is not known: RSGISExeImageCalculation.");
		throw rsgis::RSGISXMLArgumentsException(message.c_str());
	}
	
	xercesc::XMLString::release(&algorName);
	xercesc::XMLString::release(&optionNormalise);
	xercesc::XMLString::release(&optionCorrelation);
	xercesc::XMLString::release(&optionCovariance);
	xercesc::XMLString::release(&optionMeanVector);
	xercesc::XMLString::release(&optionPCA);
	xercesc::XMLString::release(&optionStandardise);
	xercesc::XMLString::release(&optionBandMaths);
	xercesc::XMLString::release(&optionReplaceValuesLessThan);
	xercesc::XMLString::release(&optionUnitArea);
	xercesc::XMLString::release(&optionImageMaths);
	xercesc::XMLString::release(&optionMovementSpeed);
	xercesc::XMLString::release(&optionCountValsInCol);
	xercesc::XMLString::release(&optionApply2VarFunction);
	xercesc::XMLString::release(&optionApply3VarFunction);
    xercesc::XMLString::release(&optionDist2Geoms);
    xercesc::XMLString::release(&optionImageBandStats);
    xercesc::XMLString::release(&optionImageStats);
    xercesc::XMLString::release(&optionUnconLinearSpecUnmix);
    xercesc::XMLString::release(&optionExhConLinearSpecUnmix);
    xercesc::XMLString::release(&optionConSum1LinearSpecUnmix);
    xercesc::XMLString::release(&optionNNConSum1LinearSpecUnmix);
    xercesc::XMLString::release(&optionKMeansCentres);
    xercesc::XMLString::release(&optionISODataCentres);
    xercesc::XMLString::release(&optionAllBandsEqualTo);
    xercesc::XMLString::release(&optionHistogram);
    xercesc::XMLString::release(&optionBandPercentile);
    xercesc::XMLString::release(&optionImgDist2Geoms);
    xercesc::XMLString::release(&optionImgCalcDist);
    xercesc::XMLString::release(&optionMahalanobisDistWindow);
    xercesc::XMLString::release(&optionMahalanobisDistImg2Window);

	parsed = true;
}

void RSGISExeImageCalculation::runAlgorithm() throw(rsgis::RSGISException)
{
    std::cout.precision(12);
	if(!parsed)
	{
		throw rsgis::RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(option == RSGISExeImageCalculation::normalise)
		{
			GDALAllRegister();
			GDALDataset *dataset = NULL;
            rsgis::img::RSGISImageNormalisation *normImage = NULL;
			double *imageMaxBands = NULL;
			double *imageMinBands = NULL;
			double *outMinBands = NULL;
			double *outMaxBands = NULL;
			
			int rasterCount = 0;
			
			try
			{
				normImage = new rsgis::img::RSGISImageNormalisation();
				
				for(int i = 0; i < this->numImages; i++)
				{
					std::cout << this->inputImages[i] << std::endl;
					dataset = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(dataset == NULL)
					{
						std::string message = std::string("Could not open image ") + this->inputImages[i];
						throw rsgis::RSGISImageException(message.c_str());
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
			catch(rsgis::RSGISException e)
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
			
			rsgis::math::RSGISMatrices matrixUtils;
			
			rsgis::img::RSGISCalcImageSingle *calcImgSingle = NULL;
			rsgis::img::RSGISCalcCC *calcCC = NULL;
			rsgis::img::RSGISCalcImageMatrix *calcImgMatrix = NULL;
			
			rsgis::math::Matrix *correlationMatrix = NULL;
			
			try
			{
				datasetsA = new GDALDataset*[1];
				std::cout << this->inputImageA << std::endl;
				datasetsA[0] = (GDALDataset *) GDALOpenShared(this->inputImageA.c_str(), GA_ReadOnly);
				if(datasetsA[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImageA;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				
				datasetsB = new GDALDataset*[1];
				std::cout << this->inputImageB << std::endl;
				datasetsB[0] = (GDALDataset *) GDALOpenShared(this->inputImageB.c_str(), GA_ReadOnly);
				if(datasetsB[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImageB;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				calcCC = new rsgis::img::RSGISCalcCC(1);
				calcImgSingle = new rsgis::img::RSGISCalcImageSingle(calcCC);
				calcImgMatrix = new rsgis::img::RSGISCalcImageMatrix(calcImgSingle);
				correlationMatrix = calcImgMatrix->calcImageMatrix(datasetsA, datasetsB, 1);
				matrixUtils.saveMatrix2txt(correlationMatrix, this->outputMatrix);
				
				delete calcCC;
				delete calcImgMatrix;
				delete calcImgSingle;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageCalculation::covariance)
		{
			GDALAllRegister();
			GDALDataset **datasetsA = NULL;
			GDALDataset **datasetsB = NULL;
			
			rsgis::math::RSGISMatrices matrixUtils;
			
			rsgis::img::RSGISCalcImageSingle *calcImgSingle = NULL;
			rsgis::img::RSGISCalcCovariance *calcCovar = NULL;
			rsgis::img::RSGISCalcImageMatrix *calcImgMatrix = NULL;
			
			rsgis::img::RSGISCalcImageSingle *calcImgSingleMean = NULL;
			rsgis::img::RSGISCalcMeanVectorIndividual *calcMean = NULL;
			rsgis::img::RSGISCalcImageMatrix *calcImgMatrixMean = NULL;
			
			rsgis::math::Matrix *meanAMatrix = NULL;
			rsgis::math::Matrix *meanBMatrix = NULL;
			rsgis::math::Matrix *covarianceMatrix = NULL;
			
			try
			{
				datasetsA = new GDALDataset*[1];
				std::cout << this->inputImageA << std::endl;
				datasetsA[0] = (GDALDataset *) GDALOpenShared(this->inputImageA.c_str(), GA_ReadOnly);
				if(datasetsA[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImageA;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				datasetsB = new GDALDataset*[1];
				std::cout << this->inputImageB << std::endl;
				datasetsB[0] = (GDALDataset *) GDALOpenShared(this->inputImageB.c_str(), GA_ReadOnly);
				if(datasetsB[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImageB;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				
				if(calcMean)
				{
					std::cout << "Mean vectors will be calculated\n";
					calcMean = new rsgis::img::RSGISCalcMeanVectorIndividual(1);
					calcImgSingleMean = new rsgis::img::RSGISCalcImageSingle(calcMean);
					calcImgMatrixMean = new rsgis::img::RSGISCalcImageMatrix(calcImgSingle);
					meanAMatrix = calcImgMatrixMean->calcImageVector(datasetsA, 1);
					meanBMatrix = calcImgMatrixMean->calcImageVector(datasetsB, 1);
					std::cout << "Mean Vectors have been calculated\n";
				}
				else
				{
					meanAMatrix = matrixUtils.readMatrixFromTxt(this->inputMatrixA);
					meanBMatrix = matrixUtils.readMatrixFromTxt(this->inputMatrixB);
				}
				
				calcCovar = new rsgis::img::RSGISCalcCovariance(1, meanAMatrix, meanBMatrix);
				calcImgSingle = new rsgis::img::RSGISCalcImageSingle(calcCovar);
				calcImgMatrix = new rsgis::img::RSGISCalcImageMatrix(calcImgSingle);
				covarianceMatrix = calcImgMatrix->calcImageMatrix(datasetsA, datasetsB, 1);
				matrixUtils.saveMatrix2txt(covarianceMatrix, this->outputMatrix);
			}
			catch(rsgis::RSGISException e)
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
			
			rsgis::img::RSGISCalcImageSingle *calcImgSingle = NULL;
			rsgis::img::RSGISCalcMeanVectorIndividual *calcMean = NULL;
			rsgis::img::RSGISCalcImageMatrix *calcImgMatrix = NULL;
			
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::Matrix *meanVectorMatrix = NULL;
			
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
				
				calcMean = new rsgis::img::RSGISCalcMeanVectorIndividual(1);
				calcImgSingle = new rsgis::img::RSGISCalcImageSingle(calcMean);
				calcImgMatrix = new rsgis::img::RSGISCalcImageMatrix(calcImgSingle);
				meanVectorMatrix = calcImgMatrix->calcImageVector(datasets, 1);
				matrixUtils.saveMatrix2txt(meanVectorMatrix, this->outputMatrix);
			}
			catch(rsgis::RSGISException e)
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
			
			rsgis::math::RSGISMatrices matrixUtils;	
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			rsgis::img::RSGISApplyEigenvectors *applyPCA = NULL;
			rsgis::math::Matrix *eigenvectorsMatrix = NULL;
			
			try
			{
				std::cout << "Reading in from file " << this->eigenvectors << std::endl;
				eigenvectorsMatrix = matrixUtils.readMatrixFromTxt(this->eigenvectors);
				std::cout << "Finished reading in matrix\n";
				
				datasets = new GDALDataset*[1];
				std::cout << "Reading in image " << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				applyPCA = new rsgis::img::RSGISApplyEigenvectors(this->numComponents, eigenvectorsMatrix);
				calcImage = new rsgis::img::RSGISCalcImage(applyPCA, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				GDALClose(datasets[0]);
			}
			catch(rsgis::RSGISException e)
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
			
			rsgis::math::RSGISMatrices matrixUtils;	
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			rsgis::img::RSGISStandardiseImage *stdImg = NULL;
			rsgis::math::Matrix *meanVectorMatrix = NULL;
			
			try
			{
				std::cout << "Reading in from file " << this->meanvectorStr << std::endl;
				meanVectorMatrix = matrixUtils.readMatrixFromTxt(this->meanvectorStr);
				std::cout << "Finished reading in matrix\n";
				
				datasets = new GDALDataset*[1];
				std::cout << "Reading in image " << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				stdImg = new rsgis::img::RSGISStandardiseImage(datasets[0]->GetRasterCount(), meanVectorMatrix);
				calcImage = new rsgis::img::RSGISCalcImage(stdImg, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				delete calcImage;
				delete stdImg;
				matrixUtils.freeMatrix(meanVectorMatrix);
				GDALClose(datasets[0]);
				
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageCalculation::bandmaths)
		{
			std::cout << "This command performs band maths\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Expression: " << this->mathsExpression << std::endl;
			for(int i = 0; i < numVars; ++i)
			{
				std::cout << i << ")\t name = " << variables[i].name << " image = " << variables[i].image << " band = " << variables[i].bandNum << std::endl;
			}
			
			try
            {
                rsgis::cmds::executeBandMaths(variables, numVars, outputImage, mathsExpression, imageFormat, rsgisOutDataType);
                delete[] variables;
            }
            catch (rsgis::RSGISException &e)
            {
                throw e;
            }			
		}
		else if(option == RSGISExeImageCalculation::replacevalueslessthan)
		{
			std::cout << "Replace values less than a threshold with a new value.\n";
			std::cout << "Image: " << this->inputImage << std::endl;
			std::cout << "Output: " << this->outputImage << std::endl;
			std::cout << "Threshold: " << this->threshold << std::endl;
			std::cout << "Value: " << this->value << std::endl;
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::img::RSGISCalcImageValue *calcImageValue = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
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

				
				calcImageValue = new rsgis::img::RSGISReplaceValuesLessThanGivenValue(numImgBands, threshold, value);
				
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
		else if(option == RSGISExeImageCalculation::unitarea)
		{
			std::cout << "Converts the image spectra to unit area.\n";
			std::cout << "Image: " << this->inputImage << std::endl;
			std::cout << "Output: " << this->outputImage << std::endl;
			std::cout << "Image Bands Matrix: " << this->inMatrixfile << std::endl;
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::img::RSGISCalcImageValue *calcImageValue = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			rsgis::math::RSGISMatrices matrixUtils;
			
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
				
				calcImageValue = new rsgis::img::RSGISConvertSpectralToUnitArea(numImgBands, bandsValuesMatrix);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcImageValue, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				
				GDALClose(datasets[0]);
				
				matrixUtils.freeMatrix(bandsValuesMatrix);
				
				delete calcImageValue;
				delete calcImage;
			}
			catch(rsgis::RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageCalculation::imagemaths)
		{
			std::cout << "This command performs band maths on each band within an image\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Expression: " << this->mathsExpression << std::endl;
			
            try
            {
                rsgis::cmds::executeImageMaths(this->inputImage, this->outputImage, this->mathsExpression, this->imageFormat, rsgisOutDataType);
            }
            catch(rsgis::RSGISException &e)
            {
                throw e;
            }
		}
		else if(option == RSGISExeImageCalculation::movementspeed)
		{
			std::cout << "This command calculates the speed of movment (mean, min and max)\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			for(int i = 0; i < this->numImages; ++i)
			{
				std::cout << "Image: " << this->inputImages[i] << " using band " << this->imageBands[i] << " at time " << this->imageTimes[i] << std::endl;
			}
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			rsgis::img::RSGISCalculateImageMovementSpeed *calcImageValue = NULL;
			
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
						std::string message = std::string("Could not open image ") + this->inputImages[i];
						throw rsgis::RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					imgBandsInStack[i] = totalNumRasterBands + imageBands[i];
					
					std::cout << "Opened Image: " << this->inputImages[i] << " will be using band " << imgBandsInStack[i] << " in stack." << std::endl;
					
					if(imageBands[i] > (numRasterBands-1))
					{
						throw rsgis::RSGISImageException("You have specified a band which is not within the image");
					}
					
					totalNumRasterBands += numRasterBands;
				}
				
				int numOutputBands = ((this->numImages-1)*2) + 3;
                
                rsgis::math::RSGISMathsUtils mathUtils;
                std::string *outBandNames = new std::string[numOutputBands];
                outBandNames[0] = std::string("Mean Movement Speed");
                outBandNames[1] = std::string("Min Movement Speed");
                outBandNames[2] = std::string("Max Movement Speed");
                
                int idx = 0;
                for(int i = 0; i < this->numImages-1; ++i)
                {
                    idx = (i * 2) + 3;
                    outBandNames[idx] = std::string("Images ") + mathUtils.inttostring(i+1) + std::string("-") + mathUtils.inttostring(i+2) + std::string(" Displacement");
                    outBandNames[idx+1] = std::string("Images ") + mathUtils.inttostring(i+1) + std::string("-") + mathUtils.inttostring(i+2) + std::string(" Movement");
                }
				
				std::cout << "Number of Output Image bands = " << numOutputBands << std::endl;
				
				calcImageValue = new rsgis::img::RSGISCalculateImageMovementSpeed(numOutputBands, this->numImages, imgBandsInStack, this->imageTimes, upper, lower);
				calcImage = new rsgis::img::RSGISCalcImage(calcImageValue, "", true);
				calcImage->calcImage(datasets, this->numImages, this->outputImage, true, outBandNames);
				
				for(int i = 0; i < this->numImages; ++i)
				{
					GDALClose(datasets[i]);
				}
				delete[] datasets;

				delete calcImage;
				delete calcImageValue;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
			
		}
		else if(option == RSGISExeImageCalculation::countvalsincol)
		{
			std::cout << "This command counts the number of values with a give range for each column\n";
			std::cout << "input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Upper: " << this->upper << std::endl;
			std::cout << "lower: " << this->lower << std::endl;
			
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			rsgis::img::RSGISCountValsAboveThresInCol *calcImageValue = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				calcImageValue = new rsgis::img::RSGISCountValsAboveThresInCol(1, upper, lower);
				calcImage = new rsgis::img::RSGISCalcImage(calcImageValue, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				GDALClose(datasets[0]);
				delete[] datasets;
				
				delete calcImage;
				delete calcImageValue;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageCalculation::calcRMSE)
		{
			GDALAllRegister();
			GDALDataset **datasetsA = NULL;
			GDALDataset **datasetsB = NULL;
						
			rsgis::img::RSGISCalcImageSingle *calcImgSingle = NULL;
			rsgis::img::RSGISCalcRMSE *calculateRSME = NULL;
			double *outRMSE;
			outRMSE = new double[1];
			
			std::cout << "Calculating RMSE between: " << this->inputImageA << " (Band " << this->inputBandA + 1 << ") and " << this->inputImageB << " (Band " << this->inputBandB + 1 << ")" << std::endl;
			
			try
			{
				datasetsA = new GDALDataset*[1];
				std::cout << this->inputImageA << std::endl;
				datasetsA[0] = (GDALDataset *) GDALOpenShared(this->inputImageA.c_str(), GA_ReadOnly);
				if(datasetsA[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImageA;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				
				datasetsB = new GDALDataset*[1];
				std::cout << this->inputImageB << std::endl;
				datasetsB[0] = (GDALDataset *) GDALOpenShared(this->inputImageB.c_str(), GA_ReadOnly);
				if(datasetsB[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImageB;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				calculateRSME = new rsgis::img::RSGISCalcRMSE(1);
				calcImgSingle = new rsgis::img::RSGISCalcImageSingle(calculateRSME);
				calcImgSingle->calcImage(datasetsA, datasetsB, 1, outRMSE, this->inputBandA, this->inputBandB);
				
				std::cout << "RMSE = " << outRMSE[0] << std::endl;
				
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
			catch(rsgis::RSGISException e)
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
				std::cout << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
			rsgis::img::RSGISCalcImage *calcImg = NULL;
			rsgis::img::RSGISApplyTwoVarFunction *applyFunction = NULL;
			try
			{
				std::cout << "Applying two variable function" <<std::endl;
				applyFunction = new rsgis::img::RSGISApplyTwoVarFunction(1, this->twoVarFunction);
				calcImg = new rsgis::img::RSGISCalcImage(applyFunction, "", true);
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
			delete applyFunction;
		}
		else if(option == RSGISExeImageCalculation::apply3VarFunction)
		{
			GDALAllRegister();
			GDALDataset **datasets = NULL;
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
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
			rsgis::img::RSGISCalcImage *calcImg = NULL;
			rsgis::img::RSGISApplyThreeVarFunction *applyFunction = NULL;
			try
			{
				std::cout << "Applying three variable function" <<std::endl;
				applyFunction = new rsgis::img::RSGISApplyThreeVarFunction(1, this->threeVarFunction);
				calcImg = new rsgis::img::RSGISCalcImage(applyFunction, "", true);
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
			delete applyFunction;
		}	
        else if(option == RSGISExeImageCalculation::dist2geoms)
        {
            std::cout << "A command to calculate the distance to the nearest geometry for each pixel within an image.\n";
            std::cout << "Input Vector: " << inputVector << std::endl;
            std::cout << "Output Image: " << outputImage << std::endl;
            std::cout << "Image Resolution: " << imgResolution << std::endl;
            
            OGRRegisterAll();
			
			rsgis::utils::RSGISFileUtils fileUtils;
			rsgis::vec::RSGISVectorUtils vecUtils;
			
            std::string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
            
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
					std::string message = std::string("Could not open vector file ") + this->inputVector;
					throw rsgis::RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
					throw rsgis::RSGISFileException(message.c_str());
				}
                inputSpatialRef = inputSHPLayer->GetSpatialRef();
                char **wktString = new char*[1];
                inputSpatialRef->exportToWkt(wktString);
                std::string projection = std::string(wktString[0]);
                OGRFree(wktString);
                OGREnvelope ogrExtent;
                inputSHPLayer->GetExtent(&ogrExtent);
                geos::geom::Envelope extent = geos::geom::Envelope(ogrExtent.MinX, ogrExtent.MaxX, ogrExtent.MinY, ogrExtent.MaxY);
				
                // Get Geometries into memory
                std::vector<OGRGeometry*> *ogrGeoms = new std::vector<OGRGeometry*>();
                rsgis::vec::RSGISGetOGRGeometries *getOGRGeoms = new rsgis::vec::RSGISGetOGRGeometries(ogrGeoms);
                rsgis::vec::RSGISProcessVector processVector = rsgis::vec::RSGISProcessVector(getOGRGeoms);
                processVector.processVectorsNoOutput(inputSHPLayer, false);
                delete getOGRGeoms;
                
                // Create Geometry Collection
                OGRGeometryCollection *geomCollectionOrig = new OGRGeometryCollection();
                for(std::vector<OGRGeometry*>::iterator iterGeoms = ogrGeoms->begin(); iterGeoms != ogrGeoms->end(); ++iterGeoms)
                {
                    geomCollectionOrig->addGeometryDirectly(*iterGeoms);
                }
                
                OGRGeometryCollection *geomCollectionLines = new OGRGeometryCollection();
                rsgis::geom::RSGISGeometry geomUtils;
                geomUtils.convertGeometryCollection2Lines(geomCollectionOrig, geomCollectionLines);
                
                // Create blank image
                rsgis::img::RSGISImageUtils imageUtils;
                GDALDataset *outImage = imageUtils.createBlankImage(this->outputImage, extent, this->imgResolution, 1, projection, 0);
                
                rsgis::img::RSGISCalcDist2Geom *dist2GeomCalcValue = new rsgis::img::RSGISCalcDist2Geom(1, geomCollectionLines, geomCollectionOrig);
                rsgis::img::RSGISCalcEditImage *calcEditImage = new rsgis::img::RSGISCalcEditImage(dist2GeomCalcValue);
                calcEditImage->calcImage(outImage);
                
                // Clean up memory.
                delete geomCollectionOrig;
                delete geomCollectionLines;
                delete ogrGeoms;
				OGRDataSource::DestroyDataSource(inputSHPDS);
				OGRCleanupAll();
                GDALClose(outImage);
			}
			catch (rsgis::RSGISException e) 
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::imagebandstats)
		{
            std::cout << "A command to calculate the statistics for the individual image bands\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output File: " << outputFile << std::endl;
            if(this->ignoreZeros)
            {
                std::cout << "Ignoring Zeros\n";
            }
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			rsgis::img::RSGISImageStatistics calcImgStats;
			
			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                int numImageBands = datasets[0]->GetRasterCount();
                rsgis::img::ImageStats **stats = new rsgis::img::ImageStats*[numImageBands];
                for(int i = 0; i < numImageBands; ++i)
                {
                    stats[i] = new rsgis::img::ImageStats;
                    stats[i]->min = 0;
                    stats[i]->max = 0;
                    stats[i]->mean = 0;
                    stats[i]->sum = 0;
                    stats[i]->stddev = 0;
                }
				
                calcImgStats.calcImageStatistics(datasets, 1, stats, numImageBands, true, this->ignoreZeros);
                
                std::ofstream outTxtFile;
                outTxtFile.open(outputFile.c_str());
                outTxtFile.precision(15);
                outTxtFile << "Min,Max,Mean,StdDev,Sum\n";
                for(int i = 0; i < numImageBands; ++i)
                {
                    outTxtFile << stats[i]->min << "," << stats[i]->max << "," << stats[i]->mean << "," << stats[i]->stddev << "," << stats[i]->sum << std::endl;
                }
                outTxtFile.flush();
                outTxtFile.close();
                
                GDALClose(datasets[0]);
                delete[] datasets;
				
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
			
        } 
        else if(option == RSGISExeImageCalculation::imagestats)
		{
            std::cout << "A command to calculate the statistics for the whole image across all bands\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output File: " << outputFile << std::endl;
            if(this->ignoreZeros)
            {
                std::cout << "Ignoring Zeros\n";
            }
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			rsgis::img::RSGISImageStatistics calcImgStats;
			
			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                rsgis::img::ImageStats *stats = new rsgis::img::ImageStats();
                stats->min = 0;
                stats->max = 0;
                stats->mean = 0;
                stats->sum = 0;
                stats->stddev = 0;
				
                calcImgStats.calcImageStatistics(datasets, 1, stats, true, this->ignoreZeros);
                
                std::ofstream outTxtFile;
                outTxtFile.open(outputFile.c_str());
                outTxtFile.precision(15);
                outTxtFile << "Min,Max,Mean,StdDev,Sum\n";
                outTxtFile << stats->min << "," << stats->max << "," << stats->mean << "," << stats->stddev << "," << stats->sum << std::endl;
                outTxtFile.flush();
                outTxtFile.close();
                
                GDALClose(datasets[0]);
                delete[] datasets;
				
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
			
        } 
        else if(option == RSGISExeImageCalculation::unconlinearspecunmix)
		{
            std::cout << "A command to undertake an unconstrained linear spectral unmixing of the input image for a set of endmembers\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output File: " << this->outputFile << std::endl;
            std::cout << "Endmemebers File: " << this->endmembersFile << std::endl;
            std::cout << "Gain: " << this->lsumGain << std::endl;
            std::cout << "Offset: " << this->lsumOffset << std::endl;
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
						
			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                rsgis::img::RSGISCalcLinearSpectralUnmixing calcSpecUnmix(this->imageFormat, this->outDataType, this->lsumGain, this->lsumOffset);
                calcSpecUnmix.performUnconstainedLinearSpectralUnmixing(datasets, 1, this->outputFile, this->endmembersFile);
                
                GDALClose(datasets[0]);
                delete[] datasets;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::exhconlinearspecunmix)
		{
            std::cout << "A command to undertake an exhaustive constrained linear spectral unmixing of the input image for a set of endmembers\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output File: " << this->outputFile << std::endl;
            std::cout << "Endmemebers File: " << this->endmembersFile << std::endl;
            std::cout << "Step Resolution: " << this->stepResolution << std::endl;
            std::cout << "Gain: " << this->lsumGain << std::endl;
            std::cout << "Offset: " << this->lsumOffset << std::endl;
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                rsgis::img::RSGISCalcLinearSpectralUnmixing calcSpecUnmix(this->imageFormat, this->outDataType, this->lsumGain, this->lsumOffset);
                calcSpecUnmix.performExhaustiveConstrainedSpectralUnmixing(datasets, 1, this->outputFile, this->endmembersFile, this->stepResolution);
                
                GDALClose(datasets[0]);
                delete[] datasets;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::consum1linearspecunmix)
		{
            std::cout << "A command to undertake a partially constrained linear spectral unmixing of the input image for a set of endmembers where the sum of the unmixing will be approximately 1\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output File: " << this->outputFile << std::endl;
            std::cout << "Endmemebers File: " << this->endmembersFile << std::endl;
            std::cout << "Weight: " << this->lsumWeight << std::endl;
            std::cout << "Gain: " << this->lsumGain << std::endl;
            std::cout << "Offset: " << this->lsumOffset << std::endl;
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
						
			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                rsgis::img::RSGISCalcLinearSpectralUnmixing calcSpecUnmix(this->imageFormat, this->outDataType, this->lsumGain, this->lsumOffset);
                calcSpecUnmix.performPartConstainedLinearSpectralUnmixing(datasets, 1, this->outputFile, this->endmembersFile, this->lsumWeight);
                
                GDALClose(datasets[0]);
                delete[] datasets;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::nnconsum1linearspecunmix)
		{
            std::cout << "A command to undertake a constrained linear spectral unmixing of the input image for a set of endmembers where the sum of the unmixing will be approximately 1 and non-negative\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output File: " << this->outputFile << std::endl;
            std::cout << "Endmemebers File: " << this->endmembersFile << std::endl;
            std::cout << "Weight: " << this->lsumWeight << std::endl;
            std::cout << "Gain: " << this->lsumGain << std::endl;
            std::cout << "Offset: " << this->lsumOffset << std::endl;
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
						
			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                rsgis::img::RSGISCalcLinearSpectralUnmixing calcSpecUnmix(this->imageFormat, this->outDataType, this->lsumGain, this->lsumOffset);
                calcSpecUnmix.performConstainedNNLinearSpectralUnmixing(datasets, 1, this->outputFile, this->endmembersFile, this->lsumWeight);
                
                GDALClose(datasets[0]);
                delete[] datasets;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::kmeanscentres)
        {
            std::cout << "Generate KMeans cluster centres\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output Matrix: " << outputFile << std::endl;
            std::cout << "Number of Clusters: " << numClusters << std::endl;
            std::cout << "Max Number of Iterations: " << maxNumIterations << std::endl;
            std::cout << "Degree of Change: " << degreeOfChange << std::endl;
            std::cout << "Sub Sampling: " << subSample << std::endl;
            
            //GDALAllRegister();
			//GDALDataset *dataset = NULL;
			
			try
			{
                /*
				dataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                rsgis::img::RSGISImageClustering imgClustering;
                imgClustering.findKMeansCentres(dataset, outputFile, numClusters, maxNumIterations, subSample, ignoreZeros, degreeOfChange, initClusterMethod);
                
                GDALClose(dataset);
                */
                rsgis::cmds::executeKMeansClustering(this->inputImage, this->outputFile, this->numClusters, this->maxNumIterations, this->subSample, this->ignoreZeros, this->degreeOfChange, this->initClusterMethod);
			}
			catch(rsgis::cmds::RSGISCmdException &e)
			{
				throw rsgis::RSGISException(e.what());
			}
        }
        else if(option == RSGISExeImageCalculation::isodatacentres)
        {
            std::cout << "Generate ISOData cluster centres\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output Matrix: " << outputFile << std::endl;
            std::cout << "Number of Clusters: " << numClusters << std::endl;
            std::cout << "Min Number of clusters: " << minNumClusters << std::endl;
            std::cout << "Max Number of Iterations: " << maxNumIterations << std::endl;
            std::cout << "Degree of Change: " << degreeOfChange << std::endl;
            std::cout << "Sub Sampling: " << subSample << std::endl;
            std::cout << "Min Number of features: " << minNumFeatures << std::endl;
            std::cout << "Min Distance between clusters: " << minDistBetweenClusters << std::endl;
            std::cout << "Max Std Dev: " << maxStdDev << std::endl;
            std::cout << "Start Iteration: " << startIteration << std::endl;
            std::cout << "End Iteration: " << endIteration << std::endl;
                        
            //GDALAllRegister();
			//GDALDataset *dataset = NULL;
			
			try
			{
				/*dataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                rsgis::img::RSGISImageClustering imgClustering;
                imgClustering.findISODataCentres(dataset, outputFile, numClusters, maxNumIterations, subSample, ignoreZeros, degreeOfChange, initClusterMethod, minDistBetweenClusters, minNumFeatures, maxStdDev, minNumClusters, startIteration, endIteration);
                
                GDALClose(dataset);
                 */
                rsgis::cmds::executeISODataClustering(this->inputImage, this->outputFile, this->numClusters, this->maxNumIterations, this->subSample, this->ignoreZeros, this->degreeOfChange, this->initClusterMethod, this->minDistBetweenClusters, this->minNumFeatures, this->maxStdDev, this->minNumClusters, this->startIteration, this->endIteration);
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::allbandsequalto)
        {
            std::cout << "Test whether all bands are equal to the same value.\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Image Value: " << this->imgValue << std::endl;
            std::cout << "If true output: " << this->outputTrueVal << std::endl;
            std::cout << "If false output: " << this->outputFalseVal << std::endl;
            			
			try
			{
                GDALAllRegister();
                GDALDataset **datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                rsgis::img::RSGISAllBandsEqualTo *calcImageValue = new rsgis::img::RSGISAllBandsEqualTo(1, this->imgValue, this->outputTrueVal, this->outputFalseVal);
                rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcImageValue, "", true);
                calcImage.calcImage(datasets, 1, this->outputImage, false, NULL, this->imageFormat, this->outDataType);
                
                GDALClose(datasets[0]);
                delete[] datasets;
                delete calcImageValue;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::histogram)
        {
            std::cout << "Generate a histogram for the region of the mask selected.\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Mask Image: " << this->imageMask << std::endl;
            std::cout << "Output File: " << this->outputFile << std::endl;
            std::cout << "Image Band: " << this->imgBand << std::endl;
            std::cout << "Mask Value: " << this->imgValue << std::endl;
            std::cout << "Histogram Bin Width: " << this->binWidth << std::endl;
            if(this->calcInMinMax)
            {
                std::cout << "Calculating Min and Max Values from the image.\n";
            }
            else
            {
                std::cout << "Min: " << this->inMin << std::endl;
                std::cout << "Max: " << this->inMax << std::endl;
            }
            
			try
			{
                GDALAllRegister();
                GDALDataset **datasets = new GDALDataset*[2];
				
                datasets[0] = (GDALDataset *) GDALOpenShared(this->imageMask.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->imageMask;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                datasets[1] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[1] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                if(this->calcInMinMax)
                {
                    unsigned int numImgBands = datasets[1]->GetRasterCount();
                    rsgis::img::ImageStats **imgStats = new rsgis::img::ImageStats*[numImgBands];
                    for(unsigned int i = 0; i < numImgBands; ++i)
                    {
                        imgStats[i] = new rsgis::img::ImageStats();
                        imgStats[i]->max = 0;
                        imgStats[i]->min = 0;
                        imgStats[i]->mean = 0;
                        imgStats[i]->stddev = 0;
                        imgStats[i]->sum = 0;
                    }
                    rsgis::img::RSGISImageStatistics calcStats;
                    calcStats.calcImageStatistics(&datasets[1], 1, imgStats, numImgBands, false, false);
                    
                    this->inMin = imgStats[this->imgBand-1]->min;
                    this->inMax = imgStats[this->imgBand-1]->max;
                    
                    for(unsigned int i = 0; i < numImgBands; ++i)
                    {
                        delete imgStats;
                    }
                    delete[] imgStats;
                }
                
                this->inMin = floor(this->inMin);
                this->inMax = ceil(this->inMax);
                
                rsgis::img::RSGISGenHistogram genHistogram;
                genHistogram.genHistogram(datasets, 2, this->outputFile, this->imgBand, this->inMin, this->inMax, this->imgValue, this->binWidth);

                
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                delete[] datasets;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::bandpercentile)
        {
            std::cout << "Calculate image band percentiles\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output File: " << this->outputFile << ".mtxt\n";
            std::cout << "Percentile: " << this->percentile << std::endl;
            if(noDataValueSpecified)
            {
                std::cout << "No Data Value: " << this->noDataValue << std::endl;
            }
            
            try
			{
                GDALAllRegister();
                GDALDataset *imageDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(imageDataset == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                rsgis::math::RSGISMatrices matrixUtils;
                rsgis::img::RSGISImagePercentiles calcPercentiles;
                
                rsgis::math::Matrix *bandPercentiles = calcPercentiles.getPercentilesForAllBands(imageDataset, this->percentile, this->noDataValue, this->noDataValueSpecified);
                
                matrixUtils.saveMatrix2GridTxt(bandPercentiles, this->outputFile);
                matrixUtils.freeMatrix(bandPercentiles);
                
                GDALClose(imageDataset);
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::imgdist2geoms)
        {
            std::cout << "A command to calculate the distance to the nearest geometry for each pixel within an image.\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Input Vector: " << inputVector << std::endl;
            std::cout << "Output Image: " << outputImage << std::endl;
            
            GDALAllRegister();
            OGRRegisterAll();
			
			rsgis::utils::RSGISFileUtils fileUtils;
			rsgis::vec::RSGISVectorUtils vecUtils;
			
            std::string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
            
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			try
			{
				
                GDALDataset *imgDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(imgDataset == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputSHPDS == NULL)
				{
					std::string message = std::string("Could not open vector file ") + this->inputVector;
					throw rsgis::RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
					throw rsgis::RSGISFileException(message.c_str());
				}
                
				
                // Get Geometries into memory
                std::vector<OGRGeometry*> *ogrGeoms = new std::vector<OGRGeometry*>();
                rsgis::vec::RSGISGetOGRGeometries *getOGRGeoms = new rsgis::vec::RSGISGetOGRGeometries(ogrGeoms);
                rsgis::vec::RSGISProcessVector processVector = rsgis::vec::RSGISProcessVector(getOGRGeoms);
                processVector.processVectorsNoOutput(inputSHPLayer, false);
                delete getOGRGeoms;
                
                // Create Geometry Collection
                OGRGeometryCollection *geomCollectionOrig = new OGRGeometryCollection();
                for(std::vector<OGRGeometry*>::iterator iterGeoms = ogrGeoms->begin(); iterGeoms != ogrGeoms->end(); ++iterGeoms)
                {
                    geomCollectionOrig->addGeometryDirectly(*iterGeoms);
                }
                
                OGRGeometryCollection *geomCollectionLines = new OGRGeometryCollection();
                rsgis::geom::RSGISGeometry geomUtils;
                geomUtils.convertGeometryCollection2Lines(geomCollectionOrig, geomCollectionLines);
                
                // Create blank image
                rsgis::img::RSGISImageUtils imageUtils;
                GDALDataset *outImage = imageUtils.createCopy(imgDataset, 1, this->outputImage, this->imageFormat, GDT_Float32);
                
                rsgis::img::RSGISCalcDist2Geom *dist2GeomCalcValue = new rsgis::img::RSGISCalcDist2Geom(1, geomCollectionLines, geomCollectionOrig);
                rsgis::img::RSGISCalcEditImage *calcEditImage = new rsgis::img::RSGISCalcEditImage(dist2GeomCalcValue);
                calcEditImage->calcImage(outImage);
                
                // Clean up memory.
                delete geomCollectionOrig;
                delete geomCollectionLines;
                delete ogrGeoms;
				OGRDataSource::DestroyDataSource(inputSHPDS);
				OGRCleanupAll();
                GDALClose(outImage);
                GDALClose(imgDataset);
			}
			catch (rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeImageCalculation::imgcalcdist)
        {
            std::cout << "A command to calculate the distance to the nearest geometry for each pixel within an image.\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output Image: " << outputImage << std::endl;
            
			try
			{
                rsgis::cmds::executeImageCalcDistance(this->inputImage, this->outputImage, this->imageFormat);
			}
			catch (rsgis::RSGISException e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (rsgis::cmds::RSGISCmdException e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (std::exception e)
            {
                throw rsgis::RSGISException(e.what());
            }
        }
        else if(option == RSGISExeImageCalculation::mahalanobisdistwindow)
        {
            std::cout << "A command to calculate the mahalanobis distance within a window to the centre pixel.\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output Image: " << outputImage << std::endl;
            std::cout << "Window Size: " << this->windowSize << std::endl;
            std::cout << "Output Format: " << this->imageFormat << std::endl;
            
            try
            {
                rsgis::cmds::executeMahalanobisDistFilter(this->inputImage, this->outputImage, this->windowSize, this->imageFormat, this->rsgisOutDataType);
            }
            catch (rsgis::RSGISException e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (rsgis::cmds::RSGISCmdException e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (std::exception e)
            {
                throw rsgis::RSGISException(e.what());
            }
        }
        else if(option == RSGISExeImageCalculation::mahalanobisdistimg2window)
        {
            std::cout << "A command to calculate the mahalanobis distance within a window to the whole image.\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output Image: " << outputImage << std::endl;
            std::cout << "Window Size: " << this->windowSize << std::endl;
            std::cout << "Output Format: " << this->imageFormat << std::endl;
            
            try
            {
                rsgis::cmds::executeMahalanobisDist2ImgFilter(this->inputImage, this->outputImage, this->windowSize, this->imageFormat, this->rsgisOutDataType);
            }
            catch (rsgis::RSGISException e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (rsgis::cmds::RSGISCmdException e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (std::exception e)
            {
                throw rsgis::RSGISException(e.what());
            }
        }
		else
		{
			std::cout << "Options not recognised\n";
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
				std::cout << "Input: " << this->inputImages[i] << std::endl;
				std::cout << "Output: " << this->outputImages[i] << std::endl;
			}
			
			if(this->calcInMinMax)
			{
				std::cout << "The input image minimum and maximum values will be calculated\n";
			}
			else
			{
				std::cout << "Input Minimum Value is: " << this->inMin << std::endl;
				std::cout << "Input Maximum Value is: " << this->inMax << std::endl;
			}
			
			std::cout << "Output Minimum Value is: " << this->outMin << std::endl;
			std::cout << "Output Maximum Value is: " << this->outMax << std::endl;
		}
		else if(option == RSGISExeImageCalculation::correlation)
		{
			std::cout << "Image A: " << this->inputImageA << std::endl;
			std::cout << "Image B: " << this->inputImageB << std::endl;
			std::cout << "Output Image: " << this->outputMatrix << std::endl;
		}
		else if(option == RSGISExeImageCalculation::covariance)
		{
			std::cout << "Image A: " << this->inputImageA << std::endl;
			std::cout << "Image B: " << this->inputImageB << std::endl;
			std::cout << "Output Image: " << this->outputMatrix << std::endl;
			if(calcMean)
			{
				std::cout << "Mean vectors will be calculated\n";
			}
			else
			{
				std::cout << "Mean Vector (A): " << this->inputMatrixA << std::endl;
				std::cout << "Mean Vector (B): " << this->inputMatrixB << std::endl;
			}
		}
		else if(option == RSGISExeImageCalculation::meanvector)
		{
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Matrix: " << this->outputMatrix << std::endl;
		}
		else if(option == RSGISExeImageCalculation::pca)
		{
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Eigenvectors: " << this->eigenvectors << std::endl;
			std::cout << "Number of Components: " << this->numComponents << std::endl;
		}
		else if(option == RSGISExeImageCalculation::standardise)
		{
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "MeanVector: " << this->meanvectorStr << std::endl;
		}
		else if(option == RSGISExeImageCalculation::bandmaths)
		{
			std::cout << "This command performs band maths\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Expression: " << this->mathsExpression << std::endl;
			for(int i = 0; i < numVars; ++i)
			{
				std::cout << i << ")\t name = " << variables[i].name << " image = " << variables[i].image << " band = " << variables[i].bandNum << std::endl;
			}
		}
		else if(option == RSGISExeImageCalculation::replacevalueslessthan)
		{
			std::cout << "Replace values less than a threshold with a new value.\n";
			std::cout << "Image: " << this->inputImage << std::endl;
			std::cout << "Output: " << this->outputImage << std::endl;
			std::cout << "Threshold: " << this->threshold << std::endl;
			std::cout << "Value: " << this->value << std::endl;
		}
		else if(option == RSGISExeImageCalculation::unitarea)
		{
			std::cout << "Converts the image spectra to unit area.\n";
			std::cout << "Image: " << this->inputImage << std::endl;
			std::cout << "Output: " << this->outputImage << std::endl;
			std::cout << "Image Bands Matrix: " << this->inMatrixfile << std::endl;
		}
		else if(option == RSGISExeImageCalculation::imagemaths)
		{
			std::cout << "This command performs band maths on each band within an image\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Expression: " << this->mathsExpression << std::endl;
		}
		else if(option == RSGISExeImageCalculation::movementspeed)
		{
			std::cout << "This command calculates the speed of movment (mean, min and max)\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			for(int i = 0; i < this->numImages; ++i)
			{
				std::cout << "Image: " << this->inputImages[i] << " using band " << this->imageBands[i] << " at time " << this->imageTimes[i] << std::endl;
			}
		}
		else if(option == RSGISExeImageCalculation::countvalsincol)
		{
			std::cout << "This command counts the number of values with a give range for each column\n";
			std::cout << "input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Upper: " << this->upper << std::endl;
			std::cout << "lower: " << this->lower << std::endl;
		}
        else if(option == RSGISExeImageCalculation::dist2geoms)
        {
            std::cout << "A command to calculate the distance to the nearest geometry for each pixel within an image.\n";
            std::cout << "Input Vector: " << inputVector << std::endl;
            std::cout << "Output Image: " << outputImage << std::endl;
            std::cout << "Image Resolution: " << imgResolution << std::endl;
        }
        else if(option == RSGISExeImageCalculation::imagebandstats)
		{
            std::cout << "A command to calculate the overall image stats\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output File: " << outputFile << std::endl;
        }
        else if(option == RSGISExeImageCalculation::unconlinearspecunmix)
		{
            std::cout << "A command to undertake an unconstrained linear spectral unmixing of the input image for a set of endmembers\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output File: " << this->outputFile << std::endl;
            std::cout << "Endmemebers File: " << this->endmembersFile << std::endl;
        }
        else if(option == RSGISExeImageCalculation::exhconlinearspecunmix)
		{
            std::cout << "A command to undertake an exhaustive constrained linear spectral unmixing of the input image for a set of endmembers\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output File: " << this->outputFile << std::endl;
            std::cout << "Endmemebers File: " << this->endmembersFile << std::endl;
            std::cout << "Step Resolution: " << this->stepResolution << std::endl;
        }
        else if(option == RSGISExeImageCalculation::consum1linearspecunmix)
		{
            std::cout << "A command to undertake a partially constrained linear spectral unmixing of the input image for a set of endmembers where the sum of the unmixing will be approximately 1\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output File: " << this->outputFile << std::endl;
            std::cout << "Endmemebers File: " << this->endmembersFile << std::endl;
            std::cout << "Weight: " << this->lsumWeight << std::endl;
        }
        else if(option == RSGISExeImageCalculation::nnconsum1linearspecunmix)
		{
            std::cout << "A command to undertake a constrained linear spectral unmixing of the input image for a set of endmembers where the sum of the unmixing will be approximately 1 and non-negative\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output File: " << this->outputFile << std::endl;
            std::cout << "Endmemebers File: " << this->endmembersFile << std::endl;
            std::cout << "Weight: " << this->lsumWeight << std::endl;
        }
        else if(option == RSGISExeImageCalculation::kmeanscentres)
        {
            std::cout << "Generate KMeans cluster centres\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output Matrix: " << outputFile << std::endl;
            std::cout << "Number of Clusters: " << numClusters << std::endl;
            std::cout << "Max Number of Iterations: " << maxNumIterations << std::endl;
            std::cout << "Degree of Change: " << degreeOfChange << std::endl;
            std::cout << "Sub Sampling: " << subSample << std::endl;
        }
        else if(option == RSGISExeImageCalculation::isodatacentres)
        {
            std::cout << "Generate ISOData cluster centres\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output Matrix: " << outputFile << std::endl;
            std::cout << "Number of Clusters: " << numClusters << std::endl;
            std::cout << "Max Number of Iterations: " << maxNumIterations << std::endl;
            std::cout << "Degree of Change: " << degreeOfChange << std::endl;
            std::cout << "Sub Sampling: " << subSample << std::endl;
            std::cout << "Min Number of features: " << minNumFeatures << std::endl;
            std::cout << "Min Distance between clusters: " << minDistBetweenClusters << std::endl;
            std::cout << "Max Std Dev: " << maxStdDev << std::endl;
        }
        else if(option == RSGISExeImageCalculation::allbandsequalto)
        {
            std::cout << "Test whether all bands are equal to the same value.\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Image Value: " << this->imgValue << std::endl;
            std::cout << "If true output: " << this->outputTrueVal << std::endl;
            std::cout << "If false output: " << this->outputFalseVal << std::endl;
        }
        else if(option == RSGISExeImageCalculation::histogram)
        {
            std::cout << "Generate a histogram for the region of the mask selected.\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Mask Image: " << this->imageMask << std::endl;
            std::cout << "Output File: " << this->outputFile << std::endl;
            std::cout << "Image Band: " << this->imgBand << std::endl;
            std::cout << "Mask Value: " << this->imgValue << std::endl;
            std::cout << "Histogram Bin Width: " << this->binWidth << std::endl;
            if(this->calcInMinMax)
            {
                std::cout << "Calculating Min and Max Values from the image.\n";
            }
            else
            {
                std::cout << "Min: " << this->inMin << std::endl;
                std::cout << "Max: " << this->inMax << std::endl;
            }
        }
        else if(option == RSGISExeImageCalculation::histogram)
        {
            std::cout << "Generate a histogram for the region of the mask selected.\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Mask Image: " << this->imageMask << std::endl;
            std::cout << "Output File: " << this->outputFile << std::endl;
            std::cout << "Image Band: " << this->imgBand << std::endl;
            std::cout << "Mask Value: " << this->imgValue << std::endl;
            std::cout << "Histogram Bin Width: " << this->binWidth << std::endl;
            if(this->calcInMinMax)
            {
                std::cout << "Calculating Min and Max Values from the image.\n";
            }
            else
            {
                std::cout << "Min: " << this->inMin << std::endl;
                std::cout << "Max: " << this->inMax << std::endl;
            }
        }
        else if(option == RSGISExeImageCalculation::imgcalcdist)
        {
            std::cout << "A command to calculate the distance to the nearest geometry for each pixel within an image.\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output Image: " << outputImage << std::endl;
        }
        else if(option == RSGISExeImageCalculation::mahalanobisdistwindow)
        {
            std::cout << "A command to calculate the mahalanobis distance within a window to the centre pixel.\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output Image: " << outputImage << std::endl;
            std::cout << "Window Size: " << this->windowSize << std::endl;
            std::cout << "Output Format: " << this->imageFormat << std::endl;
        }
        else if(option == RSGISExeImageCalculation::mahalanobisdistimg2window)
        {
            std::cout << "A command to calculate the mahalanobis distance within a window to the whole image.\n";
            std::cout << "Input Image: " << inputImage << std::endl;
            std::cout << "Output Image: " << outputImage << std::endl;
            std::cout << "Window Size: " << this->windowSize << std::endl;
            std::cout << "Output Format: " << this->imageFormat << std::endl;
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

void RSGISExeImageCalculation::help()
{
    std::cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << std::endl;
    std::cout << "<!-- A command to normalise an input image to output image pixel values to a given range (the input minimum and maximum as derived from the image) -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"normalise\" image=\"image.env\" output=\"image_out.env\" outmin=\"double\" outmax=\"double\" />" << std::endl;
    std::cout << "<!-- A command to normalise an input image to output image pixel values to a given range from the provided input minimum and maximum pixel values. -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"normalise\" image=\"image.env\" output=\"image_out.env\" inmin=\"double\" inmax=\"double\" outmin=\"double\" outmax=\"double\" />" << std::endl;
    std::cout << "<!-- A command to normalise a directory of images to output image pixel values to a given range (the input minimum and maximum as derived from the image) -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"normalise\" dir=\"directory\" ext=\"file_extension\" output=\"image_out_base\" outmin=\"double\" outmax=\"double\" />" << std::endl;
    std::cout << "<!-- A command to normalise a directory of images to output image pixel values to a given range from the provided input minimum and maximum pixel values. -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"normalise\" dir=\"directory\" ext=\"file_extension\" output=\"image_out_base\" inmin=\"double\" inmax=\"double\" outmin=\"double\" outmax=\"double\" />" << std::endl;
    std::cout << "<!-- A command to calculate the correlation between two images -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"correlation\" imageA=\"imageA.env\" imageB=\"imageB.env\" output=\"matrix.mtxt\" />" << std::endl;
    std::cout << "<!-- A command to calculate the covariance between two images -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"covariance\" imageA=\"imageA.env\" imageB=\"imageB.env\" output=\"matrix.mtxt\" />" << std::endl;
    std::cout << "<!-- A command to calculate the covariance between two images -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"covariance\" imageA=\"imageA.env\" imageB=\"imageB.env\" output=\"matrix.mtxt\" meanA=\"matrixA.mtxt\" meanB=\"matrixB.mtxt\"/>" << std::endl;
    std::cout << "<!-- A command to calculate the RMSE between two images -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"calcRMSE\" imageA=\"imageA.env\" imageB=\"imageB.env\" bandA=\"int=1\" bandB=\"int=1\" />" << std::endl;
    std::cout << "<!-- A command to calculate the mean vector of an image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"meanvector\" image=\"image.env\" output=\"matrix.mtxt\" />" << std::endl;
    std::cout << "<!-- A command to generate a PCA for an input image based on the provided eigenvectors -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"pca\" image=\"image.env\" output=\"image_out.env\" eigenvectors=\"matrix.mtxt\" components=\"int\" />" << std::endl;
    std::cout << "<!-- A command to generate a standardised image using the mean vector provided -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"standardise\" image=\"image.env\" output=\"image_out.env\" meanvector=\"matrix.mtxt\" />" << std::endl;
    std::cout << "<!-- A command to undertake band maths operations (band math operations are defined using muparser). -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"bandmaths\" output=\"image_out.env\" format=\"GDAL Format\" expression=\"string\" | expressionFile=\"string\" >" << std::endl;
    std::cout << "    <rsgis:variable name=\"string\" image=\"image1\" band=\"int\" />" << std::endl;
    std::cout << "    <rsgis:variable name=\"string\" image=\"image2\" band=\"int\" />" << std::endl;
    std::cout << "    <rsgis:variable name=\"string\" image=\"image3\" band=\"int\" />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "<!-- A command to apply a single expression to each band of any image (in the expression the band needs to be referred to as 'b1') -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"imagemaths\" image=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" expression=\"string\" | expressionFile=\"string\" >" << std::endl;
    std::cout << "<!-- A command to replace values within an image which are below a given value -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"replacevalueslessthan\" image=\"image.env\" output=\"image_out.env\" threshold=\"float\" value=\"float\" />" << std::endl;
    std::cout << "<!-- A command to convert the a spectral curve such that it has a unit area (of value 1) -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"unitarea\" image=\"image.env\" output=\"image_out.env\" imagebands=\"matrix.mtxt\"/>" << std::endl;
    std::cout << "<!-- A command to calculate mean, min and max speed of movement and displacement-->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"movementspeed\" output=\"image_out.env\" upper=\"float\" lower=\"float\" >" << std::endl;
    std::cout << "    <rsgis:image image=\"image1\" band=\"int\" time=\"float\" />" << std::endl;
    std::cout << "    <rsgis:image image=\"image2\" band=\"int\" time=\"float\" />" << std::endl;
    std::cout << "    <rsgis:image image=\"image3\" band=\"int\" time=\"float\" />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"countvalsincol\" image=\"image.env\" output=\"image_out.env\" upper=\"float\" lower=\"float\" />" << std::endl;
    std::cout << "<!-- A command to calculate the distance from each pixel to nearest geometry -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"dist2geoms\" vector=\"vector.shp\" output=\"image_out.env\" resolution=\"float\" />" << std::endl;
    std::cout << "<!-- A command to calculate the statistics for the individual image bands -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"imagebandstats\" image=\"image.env\" output=\"outfile.txt\" ignorezeros=\"yes | no\" />" << std::endl;
    std::cout << "<!-- A command to calculate the statistics for the whole image across all bands -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"imagestats\" image=\"image.env\" output=\"outfile.txt\" ignorezeros=\"yes | no\" />" << std::endl;
    std::cout << "<!-- A command to undertake an unconstrained linear spectral unmixing of the input image for a set of endmembers -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"unconlinearspecunmix\" image=\"image.env\" output=\"image\" endmembers=\"matrix.mtxt\" />" << std::endl;
    std::cout << "<!-- A command to undertake an exhaustive constrained linear spectral unmixing of the input image for a set of endmembers -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"exhconlinearspecunmix\" image=\"image.env\" output=\"image\" endmembers=\"matrix.mtxt\" step=\"float\" />" << std::endl;
    std::cout << "<!-- A command to calculate cluster centres for the image using K-Means clustering -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"kmeanscentres\" image=\"image.env\" output=\"matrix.gmtxt\" numclusters=\"int\" maxiterations=\"int\" degreeofchange=\"float\" subsample=\"int\" initmethod=\"random | diagonal_range | diagonal_stddev | diagonal_range_attach | diagonal_stddev_attach | kpp\" ignorezeros=\"yes | no\" />" << std::endl;
    std::cout << "<!-- A command to calculate cluster centres for the image using ISOData clustering -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"isodatacentres\" image=\"image.env\" output=\"matrix.gmtxt\" numclusters=\"int\" minnumclusters=\"int\" maxiterations=\"int\" degreeofchange=\"float\" subsample=\"int\" initmethod=\"random | diagonal_range | diagonal_stddev | diagonal_range_attach | diagonal_stddev_attach | kpp\" ignorezeros=\"yes | no\" mindist=\"float\" minnum=\"unsigned int\" maxstddev=\"float\" editstart=\"int\" editend=\"int\" />" << std::endl;
	std::cout << "<!-- A command to test whether all bands are equal to the same value - useful for creating images masks -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalc\" option=\"allbandsequalto\" image=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" value=\"float\" trueout=\"float\" falseout=\"float\" />" << std::endl;
    std::cout << "</rsgis:commands>\n";
}

std::string RSGISExeImageCalculation::getDescription()
{
	return "Interface for performing various image based calculations.";
}

std::string RSGISExeImageCalculation::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeImageCalculation::~RSGISExeImageCalculation()
{
	
}

}


