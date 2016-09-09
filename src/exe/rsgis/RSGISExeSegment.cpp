/*
 *  RSGISExeSegment.cpp
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 13/01/2012
 *  Copyright 2012 RSGISLib.
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

#include "RSGISExeSegment.h"

namespace rsgisexe{

RSGISExeSegment::RSGISExeSegment() : rsgis::RSGISAlgorithmParameters()
{
	this->algorithm = "segmentation";
	this->inputImage = "";
	this->outputImage = "";
}

rsgis::RSGISAlgorithmParameters* RSGISExeSegment::getInstance()
{
	return new RSGISExeSegment();
}

void RSGISExeSegment::retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException)
{
	rsgis::utils::RSGISFileUtils fileUtils;
	rsgis::math::RSGISMathsUtils mathUtils;

    XMLCh *algorName = xercesc::XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = xercesc::XMLString::transcode("algor");
	XMLCh *optionXMLStr = xercesc::XMLString::transcode("option");
    XMLCh *optionSpecGrp = xercesc::XMLString::transcode("specgrp");
    XMLCh *optionMeanImg = xercesc::XMLString::transcode("meanimg");
    XMLCh *optionRMSmallClumps = xercesc::XMLString::transcode("rmsmallclumps");
    XMLCh *optionRMSmallClumpsStepwise = xercesc::XMLString::transcode("rmsmallclumpsstepwise");
    XMLCh *optionClump = xercesc::XMLString::transcode("clump");
    XMLCh *optionRandomColourClumps = xercesc::XMLString::transcode("randomcolourclumps");
    XMLCh *optionRegionGrowingSeedClumpIds = xercesc::XMLString::transcode("regiongrowingseedclumpids");
    XMLCh *optionGrowRegionsUsingClumps = xercesc::XMLString::transcode("growregionsusingclumps");
    XMLCh *optionHistogramSeeds = xercesc::XMLString::transcode("histogramseeds");
    XMLCh *optionHistogramSeedsTxt = xercesc::XMLString::transcode("histogramseedstxt");
    XMLCh *optionSelectClumps = xercesc::XMLString::transcode("selectclumps");
    XMLCh *optionSelectClumpsTxt = xercesc::XMLString::transcode("selectclumpstxt");
    XMLCh *optionMergeSmallClumps = xercesc::XMLString::transcode("mergesmallclumps");
    XMLCh *optionRelabelClumps = xercesc::XMLString::transcode("relabelclumps");
    XMLCh *optionSpecGrpWeighted = xercesc::XMLString::transcode("specgrpweighted");
    XMLCh *optionLabelsFromClusters = xercesc::XMLString::transcode("labelsfromclusters");
    XMLCh *optionLabelsFromPixels = xercesc::XMLString::transcode("labelsfrompixels");
    XMLCh *optionGrowRegionsPixels = xercesc::XMLString::transcode("growregionspixels");
    XMLCh *optionGrowRegionsPixelsAuto = xercesc::XMLString::transcode("growregionspixelsauto");
    XMLCh *optionSpectralDiv = xercesc::XMLString::transcode("spectraldiv");
    XMLCh *optionStepwiseElimination = xercesc::XMLString::transcode("stepwiseelimination");
    XMLCh *optionElimSinglePxls = xercesc::XMLString::transcode("elimsinglepxls");
    XMLCh *optionUnionSegments = xercesc::XMLString::transcode("unionsegments");
    XMLCh *optionMergeClumpTiles = xercesc::XMLString::transcode("mergeclumptiles");
    XMLCh *optionFindTileBordersMask = xercesc::XMLString::transcode("findtilebordersmask");
    XMLCh *optionMergeClumpImages = xercesc::XMLString::transcode("mergeclumpimages");
    XMLCh *optionExtractBrightFeatures = xercesc::XMLString::transcode("extractbrightfeatures");
    
    XMLCh *projImage = xercesc::XMLString::transcode("IMAGE");
	XMLCh *projOSGB = xercesc::XMLString::transcode("OSGB");
    XMLCh *projNZ2000 = xercesc::XMLString::transcode("NZ2000");
    XMLCh *projNZ1949 = xercesc::XMLString::transcode("NZ1949");
    
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
            this->rsgisOutDataType = rsgis::rsgis_8uint;
        }
        else if(xercesc::XMLString::equals(dtUInt16, dtXMLValue))
        {
            this->rsgisOutDataType = rsgis::rsgis_16uint;
        }
        else if(xercesc::XMLString::equals(dtInt16, dtXMLValue))
        {
            this->rsgisOutDataType = rsgis::rsgis_16int;
        }
        else if(xercesc::XMLString::equals(dtUInt32, dtXMLValue))
        {
            this->rsgisOutDataType = rsgis::rsgis_32uint;
        }
        else if(xercesc::XMLString::equals(dtInt32, dtXMLValue))
        {
            this->rsgisOutDataType = rsgis::rsgis_32int;
        }
        else if(xercesc::XMLString::equals(dtFloat32, dtXMLValue))
        {
            this->rsgisOutDataType = rsgis::rsgis_8uint;
        }
        else if(xercesc::XMLString::equals(dtFloat64, dtXMLValue))
        {
            this->rsgisOutDataType = rsgis::rsgis_64float;
        }
        else
        {
            std::cerr << "Data type not recognised, defaulting to 32 bit float." << std::endl;
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
    
    const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
	if(!xercesc::XMLString::equals(algorName, algorNameEle))
	{
		throw rsgis::RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	
	const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
	if(xercesc::XMLString::equals(optionSpecGrp, optionXML))
    {
        this->option = RSGISExeSegment::specgrp;
		
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
        
        
        XMLCh *specThresholdXMLStr = xercesc::XMLString::transcode("specthreshold");
		if(argElement->hasAttribute(specThresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(specThresholdXMLStr));
			this->specThreshold = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'specthreshold\' attribute was provided.");
        }
		xercesc::XMLString::release(&specThresholdXMLStr);
        
        XMLCh *noDataXMLStr = xercesc::XMLString::transcode("nodata");
		if(argElement->hasAttribute(noDataXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(noDataXMLStr));
			this->noDataVal = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
            this->noDataValProvided = true;
		}
        else
        {
            this->noDataValProvided = false;
        }
		xercesc::XMLString::release(&noDataXMLStr);
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionMeanImg, optionXML))
    {
        this->option = RSGISExeSegment::meanimg;
		
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
		
        XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&clumpsXMLStr);
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionRMSmallClumps, optionXML))
    {
        this->option = RSGISExeSegment::rmsmallclumps;
		
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
		
        XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&clumpsXMLStr);
        
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
        
        XMLCh *specThresholdXMLStr = xercesc::XMLString::transcode("maxspectraldist");
		if(argElement->hasAttribute(specThresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(specThresholdXMLStr));
			this->specThreshold = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'maxspectraldist\' attribute was provided.");
        }
		xercesc::XMLString::release(&specThresholdXMLStr);
        
        XMLCh *minSizeXMLStr = xercesc::XMLString::transcode("minsize");
		if(argElement->hasAttribute(minSizeXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(minSizeXMLStr));
			this->minClumpSize = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'minsize\' attribute was provided.");
        }
		xercesc::XMLString::release(&minSizeXMLStr);
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionRMSmallClumpsStepwise, optionXML))
    {
        this->option = RSGISExeSegment::rmsmallclumpsstepwise;
		
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
		
        XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&clumpsXMLStr);
        
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
        
        stretchStatsAvail = false;
        XMLCh *stretchStatsXMLStr = xercesc::XMLString::transcode("stretchstats");
		if(argElement->hasAttribute(stretchStatsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(stretchStatsXMLStr));
			this->stretchStatsFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
            stretchStatsAvail = true;
		}
		else
		{
			stretchStatsAvail = false;
		}
		xercesc::XMLString::release(&stretchStatsXMLStr);
        
        XMLCh *specThresholdXMLStr = xercesc::XMLString::transcode("maxspectraldist");
		if(argElement->hasAttribute(specThresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(specThresholdXMLStr));
			this->specThreshold = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'maxspectraldist\' attribute was provided.");
        }
		xercesc::XMLString::release(&specThresholdXMLStr);
        
        XMLCh *minSizeXMLStr = xercesc::XMLString::transcode("minsize");
		if(argElement->hasAttribute(minSizeXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(minSizeXMLStr));
			this->minClumpSize = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'minsize\' attribute was provided.");
        }
		xercesc::XMLString::release(&minSizeXMLStr);
        
        
        XMLCh *storeMeanXMLStr = xercesc::XMLString::transcode("storemean");
		if(argElement->hasAttribute(storeMeanXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *storeMValue = argElement->getAttribute(storeMeanXMLStr);
			
			if(xercesc::XMLString::equals(storeMValue, yesStr))
			{
				this->storeMean = true;
			}
			else
			{
				this->storeMean = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
            this->storeMean = true;
		}
		xercesc::XMLString::release(&storeMeanXMLStr);
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionClump, optionXML))
    {
        this->option = RSGISExeSegment::clump;
		
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
        
        XMLCh *noDataXMLStr = xercesc::XMLString::transcode("nodata");
		if(argElement->hasAttribute(noDataXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(noDataXMLStr));
			this->noDataVal = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
            this->noDataValProvided = true;
		}
        else
        {
            this->noDataValProvided = false;
        }
		xercesc::XMLString::release(&noDataXMLStr);
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionRandomColourClumps, optionXML))
    {
        this->option = RSGISExeSegment::randomcolourclumps;
		
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
        
        XMLCh *importLUTXMLStr = xercesc::XMLString::transcode("importLUT");
		if(argElement->hasAttribute(importLUTXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(importLUTXMLStr));
			this->importLUTFile = std::string(charValue);
            this->importLUT = true;
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->importLUT = false;
		}
		xercesc::XMLString::release(&importLUTXMLStr);
        
        
        XMLCh *exportLUTXMLStr = xercesc::XMLString::transcode("exportLUT");
		if(argElement->hasAttribute(exportLUTXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(exportLUTXMLStr));
			this->exportLUTFile = std::string(charValue);
            this->exportLUT = true;
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->exportLUT = false;
		}
		xercesc::XMLString::release(&exportLUTXMLStr);
        
    }
    else if(xercesc::XMLString::equals(optionRegionGrowingSeedClumpIds, optionXML))
    {
        this->option = RSGISExeSegment::regiongrowingseedclumpids;
		
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
			this->outputTextFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *rsgisSeedsXMLStr = xercesc::XMLString::transcode("rsgis:seed");
        xercesc::DOMNodeList *seedNodesList = argElement->getElementsByTagName(rsgisSeedsXMLStr);
		unsigned int numSeeds = seedNodesList->getLength();		
		if(numSeeds > 0)
		{
            seedPxls = new std::vector<rsgis::segment::ImgSeeds>();
            seedPxls->reserve(numSeeds);
			xercesc::DOMElement *seedElement = NULL;
            unsigned int x = 0;
            unsigned int y = 0;
            unsigned long id = 0;
			for(unsigned int i = 0; i < numSeeds; i++)
			{
				seedElement = static_cast<xercesc::DOMElement*>(seedNodesList->item(i));
				
				XMLCh *xXMLStr = xercesc::XMLString::transcode("x");
				if(seedElement->hasAttribute(xXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(seedElement->getAttribute(xXMLStr));
					x = mathUtils.strtounsignedint(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'x\' attribute was provided for seed.");
				}
				xercesc::XMLString::release(&xXMLStr);
                
                XMLCh *yXMLStr = xercesc::XMLString::transcode("y");
				if(seedElement->hasAttribute(yXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(seedElement->getAttribute(yXMLStr));
					y = mathUtils.strtounsignedint(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'y\' attribute was provided for seed.");
				}
				xercesc::XMLString::release(&yXMLStr);
                
                XMLCh *idXMLStr = xercesc::XMLString::transcode("id");
				if(seedElement->hasAttribute(idXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(seedElement->getAttribute(idXMLStr));
					id = mathUtils.strtounsignedlong(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'id\' attribute was provided for seed.");
				}
				xercesc::XMLString::release(&idXMLStr);
                
                seedPxls->push_back(rsgis::segment::ImgSeeds(x,y,id));
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No attributes \'rsgis:seed\' tags were provided.");
		}
        xercesc::XMLString::release(&rsgisSeedsXMLStr);
    }
    else if(xercesc::XMLString::equals(optionGrowRegionsUsingClumps, optionXML))
    {
        this->option = RSGISExeSegment::growregionsusingclumps;
		
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
		
        XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&clumpsXMLStr);
        
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
        
        XMLCh *seedsXMLStr = xercesc::XMLString::transcode("seeds");
		if(argElement->hasAttribute(seedsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(seedsXMLStr));
			this->seedsTextFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'seeds\' attribute was provided.");
		}
		xercesc::XMLString::release(&seedsXMLStr);
        
        XMLCh *initThresholdXMLStr = xercesc::XMLString::transcode("initthreshold");
		if(argElement->hasAttribute(initThresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(initThresholdXMLStr));
			this->initRegionGrowthreshold = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'initthreshold\' attribute was provided.");
        }
		xercesc::XMLString::release(&initThresholdXMLStr);
        
        XMLCh *thresholdIncrementsXMLStr = xercesc::XMLString::transcode("thresholdincrements");
		if(argElement->hasAttribute(thresholdIncrementsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(thresholdIncrementsXMLStr));
			this->thresholdRegionGrowincrements = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'thresholdincrements\' attribute was provided.");
        }
		xercesc::XMLString::release(&thresholdIncrementsXMLStr);
        
        XMLCh *maxThresholdXMLStr = xercesc::XMLString::transcode("maxthreshold");
		if(argElement->hasAttribute(maxThresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maxThresholdXMLStr));
			this->maxRegionGrowthreshold = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'maxthreshold\' attribute was provided.");
        }
		xercesc::XMLString::release(&maxThresholdXMLStr);
        
        XMLCh *maxIterationsXMLStr = xercesc::XMLString::transcode("maxiterations");
		if(argElement->hasAttribute(maxIterationsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
			this->maxRegionGrowiterations = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'maxiterations\' attribute was provided.");
        }
        xercesc::XMLString::release(&maxIterationsXMLStr);
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionHistogramSeeds, optionXML))
    {
        this->option = RSGISExeSegment::histogramseeds;
		
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
		
        XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&clumpsXMLStr);
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
        
        
        XMLCh *rsgisThresholdXMLStr = xercesc::XMLString::transcode("rsgis:threshold");
        xercesc::DOMNodeList *thresholdNodesList = argElement->getElementsByTagName(rsgisThresholdXMLStr);
		unsigned int numThresholds = thresholdNodesList->getLength();		
		if(numThresholds > 0)
		{
            bandThresholds = new std::vector<rsgis::segment::BandThreshold>();
            bandThresholds->reserve(numThresholds);
			xercesc::DOMElement *thresholdElement = NULL;
            unsigned int band = 0;
            float threshold = 0;

			for(unsigned int i = 0; i < numThresholds; i++)
			{
				thresholdElement = static_cast<xercesc::DOMElement*>(thresholdNodesList->item(i));
				
				XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
				if(thresholdElement->hasAttribute(bandXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(thresholdElement->getAttribute(bandXMLStr));
					band = mathUtils.strtounsignedint(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided for seed.");
				}
				xercesc::XMLString::release(&bandXMLStr);
                                
                XMLCh *thresholdXMLStr = xercesc::XMLString::transcode("threshold");
				if(thresholdElement->hasAttribute(thresholdXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(thresholdElement->getAttribute(thresholdXMLStr));
					threshold = mathUtils.strtofloat(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'threshold\' attribute was provided for seed.");
				}
				xercesc::XMLString::release(&thresholdXMLStr);
                
                bandThresholds->push_back(rsgis::segment::BandThreshold(band,threshold));
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No attributes \'rsgis:threshold\' tags were provided.");
		}
        xercesc::XMLString::release(&rsgisThresholdXMLStr);
    }
    else if(xercesc::XMLString::equals(optionHistogramSeedsTxt, optionXML))
    {
        this->option = RSGISExeSegment::histogramseedstxt;
		
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
		
        XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&clumpsXMLStr);
        
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->seedsTextFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        
        XMLCh *rsgisThresholdXMLStr = xercesc::XMLString::transcode("rsgis:threshold");
        xercesc::DOMNodeList *thresholdNodesList = argElement->getElementsByTagName(rsgisThresholdXMLStr);
		unsigned int numThresholds = thresholdNodesList->getLength();		
		if(numThresholds > 0)
		{
            bandThresholds = new std::vector<rsgis::segment::BandThreshold>();
            bandThresholds->reserve(numThresholds);
			xercesc::DOMElement *thresholdElement = NULL;
            unsigned int band = 0;
            float threshold = 0;
            
			for(unsigned int i = 0; i < numThresholds; i++)
			{
				thresholdElement = static_cast<xercesc::DOMElement*>(thresholdNodesList->item(i));
				
				XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
				if(thresholdElement->hasAttribute(bandXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(thresholdElement->getAttribute(bandXMLStr));
					band = mathUtils.strtounsignedint(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided for seed.");
				}
				xercesc::XMLString::release(&bandXMLStr);
                
                XMLCh *thresholdXMLStr = xercesc::XMLString::transcode("threshold");
				if(thresholdElement->hasAttribute(thresholdXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(thresholdElement->getAttribute(thresholdXMLStr));
					threshold = mathUtils.strtofloat(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'threshold\' attribute was provided for seed.");
				}
				xercesc::XMLString::release(&thresholdXMLStr);
                
                bandThresholds->push_back(rsgis::segment::BandThreshold(band,threshold));
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No attributes \'rsgis:threshold\' tags were provided.");
		}
        xercesc::XMLString::release(&rsgisThresholdXMLStr);
    }
    else if(xercesc::XMLString::equals(optionSelectClumps, optionXML))
    {
        this->option = RSGISExeSegment::selectclumps;
		
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
		
        XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&clumpsXMLStr);
        
        XMLCh *largeClumpsXMLStr = xercesc::XMLString::transcode("largeclumps");
		if(argElement->hasAttribute(largeClumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(largeClumpsXMLStr));
			this->largeClumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'largeclumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&largeClumpsXMLStr);
        
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
        
        
        XMLCh *typeXMLStr = xercesc::XMLString::transcode("type");
		if(argElement->hasAttribute(typeXMLStr))
		{
			XMLCh *maxStr = xercesc::XMLString::transcode("max");
            XMLCh *minStr = xercesc::XMLString::transcode("min");
            XMLCh *meanStr = xercesc::XMLString::transcode("mean");
            XMLCh *medianStr = xercesc::XMLString::transcode("median");
            XMLCh *percentile75thStr = xercesc::XMLString::transcode("percentile75th");
            XMLCh *percentile95thStr = xercesc::XMLString::transcode("percentile95th");
			const XMLCh *inMemValue = argElement->getAttribute(typeXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, maxStr))
			{
				this->selectionMethod = rsgis::segment::RSGISSelectClumps::max;
			}
            else if(xercesc::XMLString::equals(inMemValue, minStr))
			{
				this->selectionMethod = rsgis::segment::RSGISSelectClumps::min;
			}
            else if(xercesc::XMLString::equals(inMemValue, meanStr))
			{
				this->selectionMethod = rsgis::segment::RSGISSelectClumps::mean;
			}
            else if(xercesc::XMLString::equals(inMemValue, medianStr))
			{
				this->selectionMethod = rsgis::segment::RSGISSelectClumps::median;
			}
            else if(xercesc::XMLString::equals(inMemValue, percentile75thStr))
			{
				this->selectionMethod = rsgis::segment::RSGISSelectClumps::percent75th;
			}
            else if(xercesc::XMLString::equals(inMemValue, percentile95thStr))
			{
				this->selectionMethod = rsgis::segment::RSGISSelectClumps::percent95th;
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("Selection method not recognised.");
			}
			xercesc::XMLString::release(&maxStr);
            xercesc::XMLString::release(&minStr);
            xercesc::XMLString::release(&meanStr);
            xercesc::XMLString::release(&medianStr);
            xercesc::XMLString::release(&percentile75thStr);
            xercesc::XMLString::release(&percentile95thStr);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'type\' attribute was provided.");
		}
		xercesc::XMLString::release(&typeXMLStr);
        
        
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionSelectClumpsTxt, optionXML))
    {
        this->option = RSGISExeSegment::selectclumpstxt;
		
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
		
        XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&clumpsXMLStr);
        
        XMLCh *largeClumpsXMLStr = xercesc::XMLString::transcode("largeclumps");
		if(argElement->hasAttribute(largeClumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(largeClumpsXMLStr));
			this->largeClumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'largeclumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&largeClumpsXMLStr);
        
		XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->seedsTextFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		xercesc::XMLString::release(&outputXMLStr);
        
        
        XMLCh *typeXMLStr = xercesc::XMLString::transcode("type");
		if(argElement->hasAttribute(typeXMLStr))
		{
			XMLCh *maxStr = xercesc::XMLString::transcode("max");
            XMLCh *minStr = xercesc::XMLString::transcode("min");
            XMLCh *meanStr = xercesc::XMLString::transcode("mean");
            XMLCh *medianStr = xercesc::XMLString::transcode("median");
            XMLCh *percentile75thStr = xercesc::XMLString::transcode("percentile75th");
            XMLCh *percentile95thStr = xercesc::XMLString::transcode("percentile95th");
			const XMLCh *inMemValue = argElement->getAttribute(typeXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, maxStr))
			{
				this->selectionMethod = rsgis::segment::RSGISSelectClumps::max;
			}
            else if(xercesc::XMLString::equals(inMemValue, minStr))
			{
				this->selectionMethod = rsgis::segment::RSGISSelectClumps::min;
			}
            else if(xercesc::XMLString::equals(inMemValue, meanStr))
			{
				this->selectionMethod = rsgis::segment::RSGISSelectClumps::mean;
			}
            else if(xercesc::XMLString::equals(inMemValue, medianStr))
			{
				this->selectionMethod = rsgis::segment::RSGISSelectClumps::median;
			}
            else if(xercesc::XMLString::equals(inMemValue, percentile75thStr))
			{
				this->selectionMethod = rsgis::segment::RSGISSelectClumps::percent75th;
			}
            else if(xercesc::XMLString::equals(inMemValue, percentile95thStr))
			{
				this->selectionMethod = rsgis::segment::RSGISSelectClumps::percent95th;
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("Selection method not recognised.");
			}
			xercesc::XMLString::release(&maxStr);
            xercesc::XMLString::release(&minStr);
            xercesc::XMLString::release(&meanStr);
            xercesc::XMLString::release(&medianStr);
            xercesc::XMLString::release(&percentile75thStr);
            xercesc::XMLString::release(&percentile95thStr);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'type\' attribute was provided.");
		}
		xercesc::XMLString::release(&typeXMLStr);        
        
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionMergeSmallClumps, optionXML))
    {
        this->option = RSGISExeSegment::mergesmallclumps;
		
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
		
        XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&clumpsXMLStr);
        
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
        
        XMLCh *minSizeXMLStr = xercesc::XMLString::transcode("minsize");
		if(argElement->hasAttribute(minSizeXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(minSizeXMLStr));
			this->minClumpSize = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'minsize\' attribute was provided.");
        }
		xercesc::XMLString::release(&minSizeXMLStr);
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionRelabelClumps, optionXML))
    {
        this->option = RSGISExeSegment::relabelclumps;
		
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionSpecGrpWeighted, optionXML))
    {
        this->option = RSGISExeSegment::specgrpweighted;
		
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
        
        
        XMLCh *specThresholdXMLStr = xercesc::XMLString::transcode("specthreshold");
		if(argElement->hasAttribute(specThresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(specThresholdXMLStr));
			this->specThreshold = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'specthreshold\' attribute was provided.");
        }
		xercesc::XMLString::release(&specThresholdXMLStr);
        
        XMLCh *noDataXMLStr = xercesc::XMLString::transcode("nodata");
		if(argElement->hasAttribute(noDataXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(noDataXMLStr));
			this->noDataVal = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
            this->noDataValProvided = true;
		}
        else
        {
            this->noDataValProvided = false;
        }
		xercesc::XMLString::release(&noDataXMLStr);
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionLabelsFromClusters, optionXML))
    {
        this->option = RSGISExeSegment::labelsfromclusters;
		
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
        
        XMLCh *clustersXMLStr = xercesc::XMLString::transcode("clusters");
		if(argElement->hasAttribute(clustersXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clustersXMLStr));
			this->clustersMatrix = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'clusters\' attribute was provided.");
		}
		xercesc::XMLString::release(&clustersXMLStr);
        
        
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
                
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionLabelsFromPixels, optionXML))
    {
        this->option = RSGISExeSegment::labelsfrompixels;
		
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
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionGrowRegionsPixels, optionXML))
    {
        this->option = RSGISExeSegment::growregionspixels;
		
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
        
        XMLCh *seedsXMLStr = xercesc::XMLString::transcode("seeds");
		if(argElement->hasAttribute(seedsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(seedsXMLStr));
            try 
            {
                //this->regionGrowingPxlSeeds = rsgis::segment::RSGISRegionGrowSegmentsPixels::parseSeedsText(std::string(charValue));
            } 
            catch (rsgis::utils::RSGISTextException &e)
            {
                throw rsgis::RSGISXMLArgumentsException(e.what());
            }
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'seeds\' attribute was provided.");
		}
		xercesc::XMLString::release(&seedsXMLStr);
        
        XMLCh *thresholdXMLStr = xercesc::XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			this->specThreshold = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
        }
		xercesc::XMLString::release(&thresholdXMLStr);
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionGrowRegionsPixelsAuto, optionXML))
    {
        this->option = RSGISExeSegment::growregionspixelsauto;
		
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
        
        XMLCh *seedsXMLStr = xercesc::XMLString::transcode("seeds");
		if(argElement->hasAttribute(seedsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(seedsXMLStr));
            try 
            {
                //this->regionGrowingPxlSeeds = rsgis::segment::RSGISRegionGrowSegmentsPixels::parseSeedsText(std::string(charValue));
            } 
            catch (rsgis::utils::RSGISTextException &e)
            {
                throw rsgis::RSGISXMLArgumentsException(e.what());
            }
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'seeds\' attribute was provided.");
		}
		xercesc::XMLString::release(&seedsXMLStr);
        
        XMLCh *initThresholdXMLStr = xercesc::XMLString::transcode("initthreshold");
		if(argElement->hasAttribute(initThresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(initThresholdXMLStr));
			this->initRegionGrowthreshold = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'initthreshold\' attribute was provided.");
        }
		xercesc::XMLString::release(&initThresholdXMLStr);
        
        XMLCh *thresholdIncrementsXMLStr = xercesc::XMLString::transcode("thresholdincrements");
		if(argElement->hasAttribute(thresholdIncrementsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(thresholdIncrementsXMLStr));
			this->thresholdRegionGrowincrements = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'thresholdincrements\' attribute was provided.");
        }
		xercesc::XMLString::release(&thresholdIncrementsXMLStr);
        
        XMLCh *maxThresholdXMLStr = xercesc::XMLString::transcode("maxthreshold");
		if(argElement->hasAttribute(maxThresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maxThresholdXMLStr));
			this->maxRegionGrowthreshold = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'maxthreshold\' attribute was provided.");
        }
		xercesc::XMLString::release(&maxThresholdXMLStr);
        
        XMLCh *maxIterationsXMLStr = xercesc::XMLString::transcode("maxiterations");
		if(argElement->hasAttribute(maxIterationsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
			this->maxRegionGrowiterations = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'maxiterations\' attribute was provided.");
        }
        xercesc::XMLString::release(&maxIterationsXMLStr);
        
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
        
        XMLCh *inMemoryXMLStr = xercesc::XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(xercesc::XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
			std::cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		xercesc::XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionSpectralDiv, optionXML))
    {
        this->option = RSGISExeSegment::spectraldiv;
		
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
        
        XMLCh *subDivisionXMLStr = xercesc::XMLString::transcode("subdivision");
		if(argElement->hasAttribute(subDivisionXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(subDivisionXMLStr));
			this->subDivision = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'subdivision\' attribute was provided.");
        }
		xercesc::XMLString::release(&subDivisionXMLStr);
                
        
        XMLCh *noDataXMLStr = xercesc::XMLString::transcode("nodata");
		if(argElement->hasAttribute(noDataXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(noDataXMLStr));
			this->noDataVal = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
            this->noDataValProvided = true;
		}
        else
        {
            this->noDataValProvided = false;
        }
		xercesc::XMLString::release(&noDataXMLStr);
        
        
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
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionStepwiseElimination, optionXML))
    {
        this->option = RSGISExeSegment::stepwiseelimination;
		
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
		
        XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&clumpsXMLStr);
        
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
        
        stretchStatsAvail = false;
        XMLCh *stretchStatsXMLStr = xercesc::XMLString::transcode("stretchstats");
		if(argElement->hasAttribute(stretchStatsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(stretchStatsXMLStr));
			this->stretchStatsFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
            stretchStatsAvail = true;
		}
		else
		{
			stretchStatsAvail = false;
		}
		xercesc::XMLString::release(&stretchStatsXMLStr);
        
        XMLCh *tmpTableXMLStr = xercesc::XMLString::transcode("tmptable");
		if(argElement->hasAttribute(tmpTableXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(tmpTableXMLStr));
			this->tempTable = std::string(charValue);
			xercesc::XMLString::release(&charValue);
            this->processInMemory = false;
            
            XMLCh *cacheSizeXMLStr = xercesc::XMLString::transcode("cachesize");
            if(argElement->hasAttribute(cacheSizeXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(cacheSizeXMLStr));
                this->cacheSize = mathUtils.strtounsignedint(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                this->cacheSize = 10000;
            }
            xercesc::XMLString::release(&cacheSizeXMLStr);

            
		}
		else
		{
			this->processInMemory = true;
		}
		xercesc::XMLString::release(&tmpTableXMLStr);
        
        XMLCh *outConsecXMLStr = xercesc::XMLString::transcode("outconsec");
		if(argElement->hasAttribute(outConsecXMLStr))
		{
			XMLCh *yesStr = xercesc::XMLString::transcode("yes");
			const XMLCh *outConsecValue = argElement->getAttribute(outConsecXMLStr);
			
			if(xercesc::XMLString::equals(outConsecValue, yesStr))
			{
				this->outputWithConsecutiveFIDs = true;
			}
			else
			{
				this->outputWithConsecutiveFIDs = false;
			}
			xercesc::XMLString::release(&yesStr);
		}
		else
		{
            this->outputWithConsecutiveFIDs = true;
		}
		xercesc::XMLString::release(&outConsecXMLStr);
        
        XMLCh *specThresholdXMLStr = xercesc::XMLString::transcode("maxspectraldist");
		if(argElement->hasAttribute(specThresholdXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(specThresholdXMLStr));
			this->specThreshold = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'maxspectraldist\' attribute was provided.");
        }
		xercesc::XMLString::release(&specThresholdXMLStr);
        
        XMLCh *minSizeXMLStr = xercesc::XMLString::transcode("minsize");
		if(argElement->hasAttribute(minSizeXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(minSizeXMLStr));
			this->minClumpSize = mathUtils.strtounsignedint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'minsize\' attribute was provided.");
        }
		xercesc::XMLString::release(&minSizeXMLStr);
        
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
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionElimSinglePxls, optionXML))
    {
        this->option = RSGISExeSegment::elimsinglepxls;
		
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
		
        XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		xercesc::XMLString::release(&clumpsXMLStr);
        
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
        
        XMLCh *tmpTableXMLStr = xercesc::XMLString::transcode("temp");
		if(argElement->hasAttribute(tmpTableXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(tmpTableXMLStr));
			this->tempImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
            this->processInMemory = false;
		}
		else
		{
			this->processInMemory = true;
		}
		xercesc::XMLString::release(&tmpTableXMLStr);
        
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
        
        XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(xercesc::XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(xercesc::XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::OSGB_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ2000_Proj;
			}
            else if(xercesc::XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = rsgis::img::NZ1949_Proj;
			}
			else
			{
				std::cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
    }
    else if(xercesc::XMLString::equals(optionUnionSegments, optionXML))
    {
        this->option = RSGISExeSegment::unionsegments;
		        
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
        
        XMLCh *noDataXMLStr = xercesc::XMLString::transcode("nodata");
		if(argElement->hasAttribute(noDataXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(noDataXMLStr));
            try
            {
                this->noDataVal = mathUtils.strtounsignedint(std::string(charValue));
            }
            catch(rsgis::RSGISException &e)
            {
                throw rsgis::RSGISXMLArgumentsException("\'nodata\' attribute was provided but was not a number.");
            }
			xercesc::XMLString::release(&charValue);
            this->noDataValProvided = true;
		}
        else
        {
            this->noDataValProvided = false;
        }
		xercesc::XMLString::release(&noDataXMLStr);
        
        XMLCh *rsgisImageXMLStr = xercesc::XMLString::transcode("rsgis:image");
        xercesc::DOMNodeList *imageNodesList = argElement->getElementsByTagName(rsgisImageXMLStr);
		unsigned int numImages = imageNodesList->getLength();
		if(numImages > 0)
		{
            inputImagePaths.reserve(numImages);
			xercesc::DOMElement *imageElement = NULL;
            
			for(unsigned int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<xercesc::DOMElement*>(imageNodesList->item(i));
				
				XMLCh *fileXMLStr = xercesc::XMLString::transcode("file");
				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					inputImagePaths.push_back(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'file\' attribute was provided for seed.");
				}
				xercesc::XMLString::release(&fileXMLStr);
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No attributes \'rsgis:image\' tags were provided.");
		}
        xercesc::XMLString::release(&rsgisImageXMLStr);
    }
    else if(xercesc::XMLString::equals(optionMergeClumpTiles, optionXML))
    {
        this->option = RSGISExeSegment::mergeclumptiles;
        
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
        
        
        XMLCh *borderMaskXMLStr = xercesc::XMLString::transcode("bordermask");
		if(argElement->hasAttribute(borderMaskXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(borderMaskXMLStr));
			this->borderMaskImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'bordermask\' attribute was provided.");
		}
		xercesc::XMLString::release(&borderMaskXMLStr);
        
        XMLCh *colNameXMLStr = xercesc::XMLString::transcode("colname");
        if(argElement->hasAttribute(colNameXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(colNameXMLStr));
            this->colsName = std::string(charValue);
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'colname\' attribute was provided.");
        }
        xercesc::XMLString::release(&colNameXMLStr);
        
        XMLCh *overlapXMLStr = xercesc::XMLString::transcode("overlap");
        if(argElement->hasAttribute(overlapXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(overlapXMLStr));
            this->tileOverlap = mathUtils.strtounsignedint(std::string(charValue));
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'overlap\' attribute was provided.");
        }
        xercesc::XMLString::release(&overlapXMLStr);
        
        
        XMLCh *boundaryXMLStr = xercesc::XMLString::transcode("boundary");
        if(argElement->hasAttribute(boundaryXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(boundaryXMLStr));
            this->tileBoundary = mathUtils.strtounsignedint(std::string(charValue));
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'boundary\' attribute was provided.");
        }
        xercesc::XMLString::release(&boundaryXMLStr);
        
        
        XMLCh *bodyXMLStr = xercesc::XMLString::transcode("body");
        if(argElement->hasAttribute(bodyXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bodyXMLStr));
            this->tileBody = mathUtils.strtounsignedint(std::string(charValue));
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'body\' attribute was provided.");
        }
        xercesc::XMLString::release(&bodyXMLStr);
        
        XMLCh *rsgisImageXMLStr = xercesc::XMLString::transcode("rsgis:image");
        xercesc::DOMNodeList *imageNodesList = argElement->getElementsByTagName(rsgisImageXMLStr);
		unsigned int numImages = imageNodesList->getLength();
		if(numImages > 0)
		{
            inputImagePaths.reserve(numImages);
			xercesc::DOMElement *imageElement = NULL;
            
			for(unsigned int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<xercesc::DOMElement*>(imageNodesList->item(i));
				
				XMLCh *fileXMLStr = xercesc::XMLString::transcode("clumps");
				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					inputImagePaths.push_back(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided for seed.");
				}
				xercesc::XMLString::release(&fileXMLStr);
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No attributes \'rsgis:image\' tags were provided.");
		}
        xercesc::XMLString::release(&rsgisImageXMLStr);
    }
    else if(xercesc::XMLString::equals(optionFindTileBordersMask, optionXML))
    {
        this->option = RSGISExeSegment::findtilebordersmask;
        
        XMLCh *borderMaskXMLStr = xercesc::XMLString::transcode("bordermask");
		if(argElement->hasAttribute(borderMaskXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(borderMaskXMLStr));
			this->borderMaskImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'bordermask\' attribute was provided.");
		}
		xercesc::XMLString::release(&borderMaskXMLStr);
        
        XMLCh *colNameXMLStr = xercesc::XMLString::transcode("colname");
        if(argElement->hasAttribute(colNameXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(colNameXMLStr));
            this->colsName = std::string(charValue);
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'colname\' attribute was provided.");
        }
        xercesc::XMLString::release(&colNameXMLStr);
        
        XMLCh *overlapXMLStr = xercesc::XMLString::transcode("overlap");
        if(argElement->hasAttribute(overlapXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(overlapXMLStr));
            this->tileOverlap = mathUtils.strtounsignedint(std::string(charValue));
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'overlap\' attribute was provided.");
        }
        xercesc::XMLString::release(&overlapXMLStr);
        
        
        XMLCh *boundaryXMLStr = xercesc::XMLString::transcode("boundary");
        if(argElement->hasAttribute(boundaryXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(boundaryXMLStr));
            this->tileBoundary = mathUtils.strtounsignedint(std::string(charValue));
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'boundary\' attribute was provided.");
        }
        xercesc::XMLString::release(&boundaryXMLStr);
        
        
        XMLCh *bodyXMLStr = xercesc::XMLString::transcode("body");
        if(argElement->hasAttribute(bodyXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bodyXMLStr));
            this->tileBody = mathUtils.strtounsignedint(std::string(charValue));
            xercesc::XMLString::release(&charValue);
        }
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'body\' attribute was provided.");
        }
        xercesc::XMLString::release(&bodyXMLStr);
        
        XMLCh *rsgisImageXMLStr = xercesc::XMLString::transcode("rsgis:image");
        xercesc::DOMNodeList *imageNodesList = argElement->getElementsByTagName(rsgisImageXMLStr);
		unsigned int numImages = imageNodesList->getLength();
		if(numImages > 0)
		{
            inputImagePaths.reserve(numImages);
			xercesc::DOMElement *imageElement = NULL;
            
			for(unsigned int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<xercesc::DOMElement*>(imageNodesList->item(i));
				
				XMLCh *fileXMLStr = xercesc::XMLString::transcode("clumps");
				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					inputImagePaths.push_back(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided for seed.");
				}
				xercesc::XMLString::release(&fileXMLStr);
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No attributes \'rsgis:image\' tags were provided.");
		}
        xercesc::XMLString::release(&rsgisImageXMLStr);
    }
    else if(xercesc::XMLString::equals(optionMergeClumpImages, optionXML))
    {
        this->option = RSGISExeSegment::mergeclumpimages;
        
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
        
        XMLCh *rsgisImageXMLStr = xercesc::XMLString::transcode("rsgis:image");
        xercesc::DOMNodeList *imageNodesList = argElement->getElementsByTagName(rsgisImageXMLStr);
		unsigned int numImages = imageNodesList->getLength();
		if(numImages > 0)
		{
            inputImagePaths.reserve(numImages);
			xercesc::DOMElement *imageElement = NULL;
            
			for(unsigned int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<xercesc::DOMElement*>(imageNodesList->item(i));
				
				XMLCh *fileXMLStr = xercesc::XMLString::transcode("clumps");
				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					inputImagePaths.push_back(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided for seed.");
				}
				xercesc::XMLString::release(&fileXMLStr);
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No attributes \'rsgis:image\' tags were provided.");
		}
        xercesc::XMLString::release(&rsgisImageXMLStr);
    }
    else if(xercesc::XMLString::equals(optionExtractBrightFeatures, optionXML))
    {
        this->option = RSGISExeSegment::extractbrightfeatures;
        
        XMLCh *inputXMLStr = xercesc::XMLString::transcode("image");
		if(argElement->hasAttribute(inputXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputXMLStr));
			this->inputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		xercesc::XMLString::release(&inputXMLStr);
		
        XMLCh *maskXMLStr = xercesc::XMLString::transcode("mask");
		if(argElement->hasAttribute(maskXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maskXMLStr));
			this->maskImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'mask\' attribute was provided.");
		}
		xercesc::XMLString::release(&maskXMLStr);
        
        XMLCh *temp1ImgXMLStr = xercesc::XMLString::transcode("tmp1");
		if(argElement->hasAttribute(temp1ImgXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(temp1ImgXMLStr));
			this->temp1Image = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'tmp1\' attribute was provided.");
		}
		xercesc::XMLString::release(&temp1ImgXMLStr);
        
        
        XMLCh *temp2ImgXMLStr = xercesc::XMLString::transcode("tmp2");
		if(argElement->hasAttribute(temp2ImgXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(temp2ImgXMLStr));
			this->temp2Image = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'tmp2\' attribute was provided.");
		}
		xercesc::XMLString::release(&temp2ImgXMLStr);
        
        
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
        
        XMLCh *formatXMLStr = xercesc::XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "KEA";
		}
		xercesc::XMLString::release(&formatXMLStr);
        
        XMLCh *initThresXMLStr = xercesc::XMLString::transcode("initthres");
		if(argElement->hasAttribute(initThresXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(initThresXMLStr));
			this->initThres = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'initthres\' attribute was provided.");
        }
		xercesc::XMLString::release(&initThresXMLStr);
        
        
        XMLCh *thresIncrementXMLStr = xercesc::XMLString::transcode("thresincrement");
		if(argElement->hasAttribute(thresIncrementXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(thresIncrementXMLStr));
			this->thresIncrement = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'thresincrement\' attribute was provided.");
        }
		xercesc::XMLString::release(&thresIncrementXMLStr);
        
        XMLCh *thresholdUpperXMLStr = xercesc::XMLString::transcode("thresholdupper");
		if(argElement->hasAttribute(thresholdUpperXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(thresholdUpperXMLStr));
			this->thresholdUpper = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
        else
        {
            throw rsgis::RSGISXMLArgumentsException("No \'thresholdupper\' attribute was provided.");
        }
		xercesc::XMLString::release(&thresholdUpperXMLStr);
        
        
        XMLCh *rsgisDescriptionXMLStr = xercesc::XMLString::transcode("rsgis:description");
        xercesc::DOMNodeList *descriptNodesList = argElement->getElementsByTagName(rsgisDescriptionXMLStr);
		unsigned int numDescriptions = descriptNodesList->getLength();
		if(numDescriptions > 0)
		{
            rsgis::cmds::FeatureShapeDescription *shapeFeat = NULL;
            shapeFeatDescript.reserve(numDescriptions);
			xercesc::DOMElement *descriptElement = NULL;
            
			for(unsigned int i = 0; i < numDescriptions; i++)
			{
				descriptElement = static_cast<xercesc::DOMElement*>(descriptNodesList->item(i));
				shapeFeat = new rsgis::cmds::FeatureShapeDescription();
                shapeFeat->area = false;
                shapeFeat->areaLower = 0;
                shapeFeat->areaUpper = 0;
                shapeFeat->lenWidth = false;
                shapeFeat->lenWidthLower = 0;
                shapeFeat->lenWidthUpper = 0;
                
				XMLCh *areaLowXMLStr = xercesc::XMLString::transcode("arealow");
                XMLCh *areaHighXMLStr = xercesc::XMLString::transcode("areahigh");
				if(descriptElement->hasAttribute(areaLowXMLStr) & descriptElement->hasAttribute(areaHighXMLStr))
				{
					char *charValueLow = xercesc::XMLString::transcode(descriptElement->getAttribute(areaLowXMLStr));
					shapeFeat->areaLower = mathUtils.strtofloat(std::string(charValueLow));
					xercesc::XMLString::release(&charValueLow);
                    
                    char *charValueUpper = xercesc::XMLString::transcode(descriptElement->getAttribute(areaHighXMLStr));
					shapeFeat->areaUpper = mathUtils.strtofloat(std::string(charValueUpper));
					xercesc::XMLString::release(&charValueUpper);
                    
                    shapeFeat->area = true;
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("Both \'arealow\' and \'areahigh\' attributes need to be provided.");
				}
				xercesc::XMLString::release(&areaLowXMLStr);
                xercesc::XMLString::release(&areaHighXMLStr);
                
                
                XMLCh *lenWidthLowXMLStr = xercesc::XMLString::transcode("lenwidthlow");
                XMLCh *lenWidthHighXMLStr = xercesc::XMLString::transcode("lenwidthhigh");
				if(descriptElement->hasAttribute(lenWidthLowXMLStr) & descriptElement->hasAttribute(lenWidthHighXMLStr))
				{
					char *charValueLow = xercesc::XMLString::transcode(descriptElement->getAttribute(lenWidthLowXMLStr));
					shapeFeat->lenWidthLower = mathUtils.strtofloat(std::string(charValueLow));
					xercesc::XMLString::release(&charValueLow);
                    
                    char *charValueUpper = xercesc::XMLString::transcode(descriptElement->getAttribute(lenWidthHighXMLStr));
					shapeFeat->lenWidthUpper = mathUtils.strtofloat(std::string(charValueUpper));
					xercesc::XMLString::release(&charValueUpper);
                    
                    shapeFeat->lenWidth = true;
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("Both \'arealow\' and \'areahigh\' attributes need to be provided.");
				}
				xercesc::XMLString::release(&lenWidthLowXMLStr);
                xercesc::XMLString::release(&lenWidthHighXMLStr);
                
                shapeFeatDescript.push_back(shapeFeat);
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No attributes \'rsgis:description\' tags were provided.");
		}
        xercesc::XMLString::release(&rsgisDescriptionXMLStr);
    }
    else
    {
        std::string message = std::string("The option (") + std::string(xercesc::XMLString::transcode(optionXML)) + std::string(") is not known: RSGISExeSegment.");
		throw rsgis::RSGISXMLArgumentsException(message.c_str());
    }
    
    xercesc::XMLString::release(&algorName);
	xercesc::XMLString::release(&algorXMLStr);
	xercesc::XMLString::release(&optionXMLStr);
    xercesc::XMLString::release(&optionSpecGrp);
    xercesc::XMLString::release(&optionMeanImg);
    xercesc::XMLString::release(&optionRMSmallClumps);
    xercesc::XMLString::release(&optionRMSmallClumpsStepwise);
    xercesc::XMLString::release(&optionClump);
    xercesc::XMLString::release(&optionRandomColourClumps);
    xercesc::XMLString::release(&optionRegionGrowingSeedClumpIds);
    xercesc::XMLString::release(&optionGrowRegionsUsingClumps);
    xercesc::XMLString::release(&optionHistogramSeeds);
    xercesc::XMLString::release(&optionHistogramSeedsTxt);
    xercesc::XMLString::release(&optionSelectClumps);
    xercesc::XMLString::release(&optionSelectClumpsTxt);
    xercesc::XMLString::release(&optionMergeSmallClumps);
    xercesc::XMLString::release(&optionRelabelClumps);
    xercesc::XMLString::release(&optionSpecGrpWeighted);
    xercesc::XMLString::release(&optionLabelsFromClusters);
    xercesc::XMLString::release(&optionLabelsFromPixels);
    xercesc::XMLString::release(&optionGrowRegionsPixels);
    xercesc::XMLString::release(&optionGrowRegionsPixelsAuto);
    xercesc::XMLString::release(&optionSpectralDiv);
    xercesc::XMLString::release(&optionStepwiseElimination);
    xercesc::XMLString::release(&optionElimSinglePxls);
    xercesc::XMLString::release(&optionUnionSegments);
    xercesc::XMLString::release(&optionMergeClumpTiles);
    xercesc::XMLString::release(&optionFindTileBordersMask);
    xercesc::XMLString::release(&optionMergeClumpImages);
    xercesc::XMLString::release(&optionExtractBrightFeatures);
    
    xercesc::XMLString::release(&projImage);
	xercesc::XMLString::release(&projOSGB);
    xercesc::XMLString::release(&projNZ2000);
    xercesc::XMLString::release(&projNZ1949);
	
	parsed = true; // if all successful, it is parsed
}

void RSGISExeSegment::runAlgorithm() throw(rsgis::RSGISException)
{
	if(!parsed)
	{
		throw rsgis::RSGISException("Before running the parameters much be retrieved");
	}
    else if(option == RSGISExeSegment::specgrp)
    {
        std::cout << "Locally spectrally cluster/group the pixels into clumps.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Spectral Threshold: " << this->specThreshold << std::endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                std::cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                resultDataset = imgUtils.createCopy(inDataset, 1, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                std::cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }

            std::cout << "Performing Segmentation\n";
            rsgis::segment::RSGISSpecGroupSegmentation specGrpSeg;
            specGrpSeg.performSimpleClump(spectralDataset, resultDataset, this->specThreshold, this->noDataValProvided, noDataVal);
                   
            if(this->processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(spectralDataset);
            }
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::specgrpweighted)
    {
        std::cout << "Locally spectrally cluster/group the pixels into clumps using stddev to weight distance metric.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Spectral Threshold: " << this->specThreshold << std::endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                std::cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                resultDataset = imgUtils.createCopy(inDataset, 1, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                std::cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            
            std::cout << "Performing Segmentation\n";
            rsgis::segment::RSGISSpecGroupSegmentation specGrpSeg;
            specGrpSeg.performSimpleClumpStdDevWeights(spectralDataset, resultDataset, this->specThreshold, this->noDataValProvided, noDataVal);
            
            if(this->processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(spectralDataset);
            }
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager(); 
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::meanimg)
    {
        std::cout << "Create a mean spectral image from a clumped image.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        
        try
        {
            rsgis::cmds::executeMeanImage(this->inputImage, this->clumpsImage, this->outputImage, this->imageFormat, this->rsgisOutDataType, this->processInMemory);
        }
        catch (rsgis::cmds::RSGISCmdException &e)
        {
            throw rsgis::RSGISException(e.what());
        }
    }
    else if(option == RSGISExeSegment::rmsmallclumps)
    {
        std::cout << "Eliminate clumps smaller than a given size from the scene.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                std::cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                resultDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                std::cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                clumpsDataset = inClumpDataset;
                resultDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            imgUtils.copyUIntGDALDataset(clumpsDataset, resultDataset);
            
            std::cout << "Eliminant Clumps\n";
            rsgis::segment::RSGISEliminateSmallClumps eliminate;
            eliminate.eliminateSmallClumps(spectralDataset, resultDataset, minClumpSize, specThreshold);
            
            if(this->processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(spectralDataset);
                GDALClose(clumpsDataset);
            }
            
            resultDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(inClumpDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::rmsmallclumpsstepwise)
    {
        std::cout << "Eliminate clumps smaller than a given size from the scene using stepwise approach.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        if(this->stretchStatsAvail)
        {
            std::cout << "Statistics File: " << this->stretchStatsFile << std::endl;
        }
        if(processInMemory)
        {
            std::cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            std::cout << "Processing to undertaken from disk\n";
        }
        if(this->storeMean)
        {
            std::cout << "Mean values are stored in memory.\n";
        }
        else
        {
            std::cout << "Mean values are calculated at runtime.\n";
        }
        
        try
        {
            rsgis::cmds::executeRMSmallClumpsStepwise(this->inputImage, this->clumpsImage, this->outputImage, this->imageFormat, this->stretchStatsAvail, this->stretchStatsFile, this->storeMean, this->processInMemory, this->minClumpSize, this->specThreshold);
        }
        catch(rsgis::cmds::RSGISCmdException &e)
        {
            throw rsgis::RSGISException(e.what());
        }
        
    }
    else if(option == RSGISExeSegment::clump)
    {
        std::cout << "Clump a categorical image into clumps with unique IDs.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        
        try
        {
            rsgis::cmds::executeClump(this->inputImage, this->outputImage, this->imageFormat, this->processInMemory, this->noDataValProvided, this->noDataVal);
        }
        catch(rsgis::cmds::RSGISCmdException &e)
        {
            throw rsgis::RSGISException(e.what());
        }
    }
    else if(option == RSGISExeSegment::randomcolourclumps)
    {
        std::cout << "Colour clumps randomly for visualisation\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        
        try
        {
            rsgis::cmds::executeRandomColourClumps(this->inputImage, this->outputImage, this->imageFormat, this->processInMemory, this->importLUTFile, this->importLUT, this->exportLUTFile, this->exportLUT);
        }
        catch(rsgis::cmds::RSGISCmdException &e)
        {
            throw rsgis::RSGISException(e.what());
        }
        
    }
    else if(option == RSGISExeSegment::regiongrowingseedclumpids)
    {
        std::cout << "Find clump ids for pixels and output region growing seeds.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output file: " << this->outputTextFile << std::endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            std::cout << "Exporting clump IDs for seeds\n";
            rsgis::segment::RSGISFindClumpIDs findClumpIDs;
            findClumpIDs.exportClumpIDsAsTxtFile(inDataset, this->outputTextFile, this->seedPxls);
            
            // Tidy up
            GDALClose(inDataset);
            delete this->seedPxls;
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::growregionsusingclumps)
    {
        std::cout << "A command to region grow from clump seeds using the clumps as base units.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Seeds File: " << this->seedsTextFile << std::endl;
        std::cout << "Initial Threshold: " << this->initRegionGrowthreshold << std::endl;
        std::cout << "Threshold Increments: " << this->thresholdRegionGrowincrements << std::endl;
        std::cout << "Maximum Threshold: " << this->maxRegionGrowthreshold << std::endl;
        std::cout << "Maximum Number of Iterations: " << this->maxRegionGrowiterations << std::endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                std::cout << "Processing in Memory\n";
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                resultDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                std::cout << "Processing using Disk\n";
                clumpsDataset = inClumpDataset;
                resultDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            
            std::vector<rsgis::segment::ClumpSeed> *clumpSeeds = NULL;
            try 
            {
                rsgis::segment::RSGISFindClumpIDs findClumpIds;
                clumpSeeds = findClumpIds.readClumpSeedIDs(this->seedsTextFile);
                std::cout << "Performing Region Growing...\n";
                rsgis::segment::RSGISRegionGrowingSegmentation regionGrow;
                regionGrow.performRegionGrowUsingClumps(inDataset, clumpsDataset, resultDataset, clumpSeeds, this->initRegionGrowthreshold, this->thresholdRegionGrowincrements, this->maxRegionGrowthreshold, this->maxRegionGrowiterations);
            } 
            catch (rsgis::utils::RSGISTextException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch(rsgis::img::RSGISImageCalcException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (rsgis::RSGISException &e) 
            {
                throw e;
            }
            
            
            if(this->processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(clumpsDataset);
            }
            
            // Tidy up
            if(clumpSeeds != NULL)
            {
                delete clumpSeeds;
            }
            
            GDALClose(inDataset);
            GDALClose(inClumpDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::histogramseeds)
    {
        std::cout << "Create a mean spectral image from a clumped image.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        for(std::vector<rsgis::segment::BandThreshold>::iterator iterThreshold = bandThresholds->begin(); iterThreshold != bandThresholds->end(); ++iterThreshold)
        {
            std::cout << "Band " << (*iterThreshold).band << " threshold = " << (*iterThreshold).threshold << std::endl;
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                std::cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                resultDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                std::cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                clumpsDataset = inClumpDataset;
                resultDataset = imgUtils.createCopy(inDataset, this->outputImage, this->imageFormat, GDT_Float32, this->projFromImage, this->proj);
            }
            
            std::cout << "Calculating Seeds\n";
            rsgis::segment::RSGISGenerateSeeds genSeeds;
            genSeeds.genSeedsHistogram(spectralDataset, clumpsDataset, resultDataset, bandThresholds);
            
            if(this->processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(spectralDataset);
                GDALClose(clumpsDataset);
            }
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(inClumpDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::histogramseedstxt)
    {
        std::cout << "Create a mean spectral image from a clumped image.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output File: " << this->seedsTextFile << std::endl;
        for(std::vector<rsgis::segment::BandThreshold>::iterator iterThreshold = bandThresholds->begin(); iterThreshold != bandThresholds->end(); ++iterThreshold)
        {
            std::cout << "Band " << (*iterThreshold).band << " threshold = " << (*iterThreshold).threshold << std::endl;
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *clumpsDataset = NULL;
            
            if(this->processInMemory)
            {
                std::cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
            }
            else
            {
                std::cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                clumpsDataset = inClumpDataset;
            }
            
            std::cout << "Calculating Seeds\n";
            rsgis::segment::RSGISGenerateSeeds genSeeds;
            genSeeds.genSeedsHistogram(spectralDataset, clumpsDataset, this->seedsTextFile, bandThresholds);
            
            if(this->processInMemory)
            {
                GDALClose(spectralDataset);
                GDALClose(clumpsDataset);
            }
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(inClumpDataset);
            GDALDestroyDriverManager();
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::selectclumps)
    {
        std::cout << "Select clumps within larger parent clumps with highest/lowest value.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clumps Image: " << this->clumpsImage << std::endl;
        std::cout << "Large Clumps Image: " << this->largeClumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        if(this->selectionMethod == rsgis::segment::RSGISSelectClumps::max)
        {
            std::cout << "Selecting clumps with highest values.\n";
        }
        else if (this->selectionMethod == rsgis::segment::RSGISSelectClumps::min)
        {
            std::cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == rsgis::segment::RSGISSelectClumps::mean)
        {
            std::cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == rsgis::segment::RSGISSelectClumps::median)
        {
            std::cout << "Selecting clumps with lowest values.\n";
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *spectralDataset = NULL;
            spectralDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(spectralDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *inLClumpDataset = NULL;
            inLClumpDataset = (GDALDataset *) GDALOpen(this->largeClumpsImage.c_str(), GA_ReadOnly);
            if(inLClumpDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->largeClumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *lClumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                std::cout << "Processing in Memory\n";

                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                
                lClumpsDataset = imgUtils.createCopy(inLClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inLClumpDataset, lClumpsDataset);
                
                resultDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                std::cout << "Processing using Disk\n";
                clumpsDataset = inClumpDataset;
                lClumpsDataset = inLClumpDataset;
                resultDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            
            std::cout << "Select clumps.\n";
            rsgis::segment::RSGISSelectClumps selectClumpsObj;
            selectClumpsObj.selectClumps(spectralDataset, clumpsDataset, lClumpsDataset, resultDataset, this->selectionMethod);
            
            if(this->processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyFloatGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(clumpsDataset);
                GDALClose(lClumpsDataset);
            }
            
            // Tidy up
            GDALClose(spectralDataset);
            GDALClose(inClumpDataset);
            GDALClose(inLClumpDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::selectclumpstxt)
    {
        std::cout << "Select clumps within larger parent clumps with highest/lowest value.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clumps Image: " << this->clumpsImage << std::endl;
        std::cout << "Large Clumps Image: " << this->largeClumpsImage << std::endl;
        std::cout << "Output Seeds File: " << this->seedsTextFile << std::endl;
        if(this->selectionMethod == rsgis::segment::RSGISSelectClumps::max)
        {
            std::cout << "Selecting clumps with highest values.\n";
        }
        else if (this->selectionMethod == rsgis::segment::RSGISSelectClumps::min)
        {
            std::cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == rsgis::segment::RSGISSelectClumps::mean)
        {
            std::cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == rsgis::segment::RSGISSelectClumps::median)
        {
            std::cout << "Selecting clumps with lowest values.\n";
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *spectralDataset = NULL;
            spectralDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(spectralDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *inLClumpDataset = NULL;
            inLClumpDataset = (GDALDataset *) GDALOpen(this->largeClumpsImage.c_str(), GA_ReadOnly);
            if(inLClumpDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->largeClumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *lClumpsDataset = NULL;
            
            if(this->processInMemory)
            {
                std::cout << "Processing in Memory\n";
                
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                
                lClumpsDataset = imgUtils.createCopy(inLClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inLClumpDataset, lClumpsDataset);
            }
            else
            {
                std::cout << "Processing using Disk\n";
                clumpsDataset = inClumpDataset;
                lClumpsDataset = inLClumpDataset;
            }
            
            std::cout << "Select clumps.\n";
            rsgis::segment::RSGISSelectClumps selectClumpsObj;
            selectClumpsObj.selectClumps(spectralDataset, clumpsDataset, lClumpsDataset, this->seedsTextFile, this->selectionMethod);
            
            if(this->processInMemory)
            {
                GDALClose(clumpsDataset);
                GDALClose(lClumpsDataset);
            }
            
            // Tidy up
            GDALClose(spectralDataset);
            GDALClose(inClumpDataset);
            GDALClose(inLClumpDataset);
            GDALDestroyDriverManager();
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::mergesmallclumps)
    {
        std::cout << "Merge small clumps to one another or neighbouring larger clumps.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Small Clump Threshold: " << this->minClumpSize << std::endl;
        
        try 
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                std::cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                resultDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                std::cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                clumpsDataset = inClumpDataset;
                resultDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            imgUtils.copyUIntGDALDataset(clumpsDataset, resultDataset);
            
            std::cout << "Eliminant Clumps\n";
            rsgis::segment::RSGISMergeSmallClumps mergeClumps;
            mergeClumps.mergeSmallClumps(spectralDataset, resultDataset, minClumpSize);
            
            if(this->processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(spectralDataset);
                GDALClose(clumpsDataset);
            }
            
            resultDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(inClumpDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::relabelclumps)
    {
        std::cout << "Relabel clumps to produce clumps with consecutive unique IDs.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        
        try
        {
            rsgis::cmds::executeRelabelClumps(this->inputImage, this->outputImage, this->imageFormat, this->processInMemory);
        }
        catch (rsgis::cmds::RSGISCmdException &e)
        {
            throw rsgis::RSGISException(e.what());
        }
        
        
    }
    else if(option == RSGISExeSegment::labelsfromclusters)
    {
        std::cout << "A command which labels image pixels with the id of the nearest cluster centre.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Cluster Matrix: " << this->clustersMatrix << std::endl;
        if(ignoreZeros)
        {
            std::cout << "Ignoring Zeros\n";
        }
        
        try
        {
            rsgis::cmds::executeLabelPixelsFromClusterCentres(this->inputImage, this->outputImage, this->clustersMatrix, this->ignoreZeros, this->imageFormat);
        }
        catch (rsgis::cmds::RSGISCmdException &e)
        {
            throw rsgis::RSGISException(e.what());
        }        
    }
    else if(option == RSGISExeSegment::labelsfrompixels)
    {
        std::cout << "A command which produces an output image using the number of each pixel.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        if(ignoreZeros)
        {
            std::cout << "Ignoring Zeros\n";
        }
        
        
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
        rsgis::segment::RSGISLabelPixelsUsingPixelsCalcImg *labelPixelsUsingPixels = NULL;
        
        try
        {
            std::string *outBandName = new std::string[1];
            outBandName[0] = "clumps";
            
            labelPixelsUsingPixels = new rsgis::segment::RSGISLabelPixelsUsingPixelsCalcImg(1, this->ignoreZeros);
            calcImg = new rsgis::img::RSGISCalcImage(labelPixelsUsingPixels, "", true);
            calcImg->calcImage(datasets, 1, this->outputImage, true, outBandName, this->imageFormat, GDT_UInt32);
            
            delete[] outBandName;
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
        delete labelPixelsUsingPixels;
        
    }
    else if(option == RSGISExeSegment::growregionspixels)
    {
        std::cout << "A command to grow regions from seeds up to a given distance in spectral space.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Threshold: " << this->specThreshold << std::endl;
        /*
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *resultDataset = NULL;
            GDALDataset *spectralDataset = NULL;
            
            if(this->processInMemory)
            {
                std::cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                resultDataset = imgUtils.createCopy(inDataset, 1, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                std::cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            
            try 
            {
                std::cout << "Performing Region Growing...\n";
                rsgis::segment::RSGISRegionGrowSegmentsPixels regionGrow(spectralDataset, resultDataset);
                regionGrow.performRegionGrowing(regionGrowingPxlSeeds, specThreshold);
            } 
            catch (rsgis::utils::RSGISTextException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch(rsgis::img::RSGISImageCalcException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch(rsgis::RSGISImageException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (rsgis::RSGISException &e) 
            {
                throw e;
            }
            
            
            if(this->processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(spectralDataset);
            }
            
            // Tidy up
            delete regionGrowingPxlSeeds;
            
            GDALClose(inDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
        */
    }
    else if(option == RSGISExeSegment::growregionspixelsauto)
    {
        std::cout << "A command to region grow from seeds on a per pixel basis.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Initial Threshold: " << this->initRegionGrowthreshold << std::endl;
        std::cout << "Threshold Increments: " << this->thresholdRegionGrowincrements << std::endl;
        std::cout << "Maximum Threshold: " << this->maxRegionGrowthreshold << std::endl;
        std::cout << "Maximum Number of Iterations: " << this->maxRegionGrowiterations << std::endl;
       /*
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *resultDataset = NULL;
            GDALDataset *spectralDataset = NULL;
            
            if(this->processInMemory)
            {
                std::cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                resultDataset = imgUtils.createCopy(inDataset, 1, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                std::cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            
            try 
            {
                std::cout << "Performing Region Growing...\n";
                rsgis::segment::RSGISRegionGrowSegmentsPixels regionGrow(spectralDataset, resultDataset);
                regionGrow.performRegionGrowing(regionGrowingPxlSeeds, initRegionGrowthreshold, thresholdRegionGrowincrements, maxRegionGrowthreshold, maxRegionGrowiterations);
            } 
            catch (rsgis::utils::RSGISTextException &e) 
            {
                throw rsgis::RSGISException(e.what());
            }
            catch(rsgis::img::RSGISImageCalcException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch(rsgis::RSGISImageException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (rsgis::RSGISException &e) 
            {
                throw e;
            }
            
            
            if(this->processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, 1,  this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(spectralDataset);
            }
            
            // Tidy up
            delete regionGrowingPxlSeeds;
            
            GDALClose(inDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
        */
    }
    else if(option == RSGISExeSegment::spectraldiv)
    {
        std::cout << "Finding spectral division using a defined subdivision for bands.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Sub division: " << this->subDivision << std::endl;
        if(this->noDataValProvided)
        {
            std::cout << "No Data Value: " << this->noDataVal << std::endl;
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::segment::RSGISDefineSpectralDivision defSpecDiv;
            defSpecDiv.findSpectralDivision(inDataset, this->outputImage, this->subDivision, this->noDataVal, this->noDataValProvided, this->projFromImage, this->proj, this->imageFormat);
            
            GDALClose(inDataset);
            GDALDestroyDriverManager();
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
        
    }
    else if(option == RSGISExeSegment::stepwiseelimination)
    {
        std::cout << "Eliminate clumps smaller than a given size from the scene using stepwise approach using an attribute table.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        if(this->stretchStatsAvail)
        {
            std::cout << "Statistics File: " << this->stretchStatsFile << std::endl;
        }
        if(!this->processInMemory)
        {
            std::cout << "Temp Table: " << this->tempTable << std::endl;
            std::cout << "Cache Size: " << this->cacheSize << std::endl;
        }
        if(this->outputWithConsecutiveFIDs)
        {
            std::cout << "Outputting with consecutive FIDs\n";
        }
        /*
        try
        {
            GDALAllRegister();
            GDALDataset *spectralDataset = NULL;
            spectralDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(spectralDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *clumpsDataset = NULL;
            clumpsDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(clumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + this->clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            std::vector<rsgis::img::BandSpecThresholdStats> *bandStretchStats = NULL;
            if(this->stretchStatsAvail)
            {
                bandStretchStats = rsgis::img::RSGISStretchImage::readBandSpecThresholds(stretchStatsFile);
            }
                        
            std::cout << "Create Attribute Table\n";
            rsgis::rastergis::RSGISCreateNewAttributeTable createAtt;
            rsgis::rastergis::RSGISAttributeTable *attTable = NULL;
            if(this->processInMemory)
            {
                attTable = createAtt.createAndPopPixelCount(clumpsDataset, this->processInMemory, this->tempTable, this->cacheSize);
            }
            else
            {
                attTable = createAtt.createAndPopPixelCountOffLine(clumpsDataset, this->processInMemory, this->tempTable, this->cacheSize);
            }
            
            std::cout << "Eliminating Clumps\n";
            rsgis::segment::RSGISEliminateSmallClumps eliminate;
            eliminate.stepwiseEliminateSmallClumpsWithAtt(spectralDataset, clumpsDataset, this->outputImage, this->imageFormat, this->projFromImage, this->proj, attTable, minClumpSize, specThreshold, outputWithConsecutiveFIDs, bandStretchStats, this->stretchStatsAvail);
                        
            // Tidy up
            if(this->stretchStatsAvail)
            {
                delete bandStretchStats;
            }
            
            clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            delete attTable;
            GDALClose(spectralDataset);
            GDALClose(clumpsDataset);
            GDALDestroyDriverManager();
        } 
        catch (rsgis::RSGISException &e) 
        {
            throw e;
        }
        */
    }
    else if(option == RSGISExeSegment::elimsinglepxls)
    {
        std::cout << "A command which iteratively removes single pixel labelled neighbours into spectrally closest large group.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        if(!this->processInMemory)
        {
            std::cout << "Temp Image: " << this->tempImage << std::endl;
        }
        if(this->ignoreZeros)
        {
            std::cout << "Ignoring pixels with a value of zero in clumps file\n";
        }
        
        try
        {
            rsgis::cmds::executeEliminateSinglePixels(this->inputImage, this->clumpsImage, this->outputImage, this->tempImage, this->imageFormat, this->processInMemory, this->ignoreZeros);
        }
        catch (rsgis::cmds::RSGISCmdException &e)
        {
            throw rsgis::RSGISException(e.what());
        }
    }
    else if(option == RSGISExeSegment::unionsegments)
    {
        std::cout << "Combine segmentations creating a single segmentation through an union\n";
        std::cout << "Output File: " << this->outputImage << std::endl;
        std::cout << "Output Format: " << this->imageFormat << std::endl;
        if(noDataValProvided)
        {
            std::cout << "No Data: " << this->noDataVal << std::endl;
        }
        else
        {
            std::cout << "A no data value was not provided\n";
        }
        std::cout << "Input Images: \n";
        for(std::vector<std::string>::iterator iterFiles = inputImagePaths.begin(); iterFiles != inputImagePaths.end(); ++iterFiles)
        {
            std::cout << "\t" << *iterFiles << std::endl;
        }
        
        try
        {
            rsgis::cmds::executeUnionOfClumps(this->inputImagePaths, this->outputImage, this->imageFormat, this->noDataValProvided, this->noDataVal);
        }
        catch(rsgis::cmds::RSGISCmdException &e)
        {
            throw rsgis::RSGISException(e.what());
        }
    }
    else if(option == RSGISExeSegment::mergeclumptiles)
    {
        std::cout << "A command to merge tiles of image clumps into an existing image file.\n";
        std::cout << "Output File: " << this->outputImage << std::endl;
        std::cout << "Border Mask File: " << this->borderMaskImage << std::endl;
        std::cout << "Tile Info Column: " << this->colsName << std::endl;
        std::cout << "Tile Overlap: " << this->tileOverlap << std::endl;
        std::cout << "Tile Boundary: " << this->tileBoundary << std::endl;
        std::cout << "Tile Body: " << this->tileBody << std::endl;
        std::cout << "Input Clump Images: \n";
        for(std::vector<std::string>::iterator iterFiles = inputImagePaths.begin(); iterFiles != inputImagePaths.end(); ++iterFiles)
        {
            std::cout << "\t" << *iterFiles << std::endl;
        }
        
        try
        {
            rsgis::cmds::executeMergeSegmentationTiles(this->outputImage, this->borderMaskImage, this->inputImagePaths, this->tileBoundary, this->tileOverlap, this->tileBody, this->colsName);
        }
        catch(rsgis::cmds::RSGISCmdException &e)
        {
            throw rsgis::RSGISException(e.what());
        }
    }
    else if(option == RSGISExeSegment::findtilebordersmask)
    {
        std::cout << "A command to create mask of the border clumps from the segmented tiles.\n";
        std::cout << "Border Mask File: " << this->borderMaskImage << std::endl;
        std::cout << "Tile Info Column: " << this->colsName << std::endl;
        std::cout << "Tile Overlap: " << this->tileOverlap << std::endl;
        std::cout << "Tile Boundary: " << this->tileBoundary << std::endl;
        std::cout << "Tile Body: " << this->tileBody << std::endl;
        std::cout << "Input Clump Images: \n";
        for(std::vector<std::string>::iterator iterFiles = inputImagePaths.begin(); iterFiles != inputImagePaths.end(); ++iterFiles)
        {
            std::cout << "\t" << *iterFiles << std::endl;
        }
        
        try
        {
            rsgis::cmds::executeFindTileBordersMask(this->inputImagePaths, this->borderMaskImage, this->tileBoundary, this->tileOverlap, this->tileBody, this->colsName);
        }
        catch(rsgis::cmds::RSGISCmdException &e)
        {
            throw rsgis::RSGISException(e.what());
        }
    }
    else if(option == RSGISExeSegment::mergeclumpimages)
    {
        std::cout << "A command to merge clump images into an exiting clump image.\n";
        std::cout << "Output File: " << this->outputImage << std::endl;
        std::cout << "Input Clump Images: \n";
        for(std::vector<std::string>::iterator iterFiles = inputImagePaths.begin(); iterFiles != inputImagePaths.end(); ++iterFiles)
        {
            std::cout << "\t" << *iterFiles << std::endl;
        }
        
        try
        {
            rsgis::cmds::executeMergeClumpImages(this->inputImagePaths, this->outputImage, false);
        }
        catch(rsgis::cmds::RSGISCmdException &e)
        {
            throw rsgis::RSGISException(e.what());
        }
    }
    else if(option == RSGISExeSegment::extractbrightfeatures)
    {
        std::cout << "A command to extract features from which are bright within the scene with a particular shape and size.";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Mask Image: " << this->maskImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Output Image Format: " << this->imageFormat << std::endl;
        std::cout << "Temp 1 Image: " << this->temp1Image << std::endl;
        std::cout << "Temp 2 Image: " << this->temp2Image << std::endl;
        std::cout << "Initial Threshold: " << this->initThres << std::endl;
        std::cout << "Threshold Increment: " << this->thresIncrement << std::endl;
        std::cout << "Threshold Limit: " << this->thresholdUpper << std::endl;
        for(std::vector<rsgis::cmds::FeatureShapeDescription*>::iterator iterShpFeat = shapeFeatDescript.begin(); iterShpFeat != shapeFeatDescript.end(); ++iterShpFeat)
        {
            std::cout << "Feature: (Area: " << (*iterShpFeat)->areaLower << ", " << (*iterShpFeat)->areaUpper << ") (Length / Width: " << (*iterShpFeat)->lenWidthLower << ", " << (*iterShpFeat)->lenWidthUpper << ")\n";
        }
        
        try
        {
            rsgis::cmds::executeExtractBrightFeatures(this->inputImage, this->maskImage, this->outputImage, this->temp1Image, this->temp2Image, this->imageFormat, this->initThres, this->thresIncrement, this->thresholdUpper, shapeFeatDescript);
        }
        catch(rsgis::cmds::RSGISCmdException &e)
        {
            throw rsgis::RSGISException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw rsgis::RSGISException(e.what());
        }
        
        for(std::vector<rsgis::cmds::FeatureShapeDescription*>::iterator iterShpFeat = shapeFeatDescript.begin(); iterShpFeat != shapeFeatDescript.end(); ++iterShpFeat)
        {
            delete *iterShpFeat;
        }
    }
    else
    {
        std::cout << "RSGISExeSegment: Options not recognised\n";
    }
}


void RSGISExeSegment::printParameters()
{
	if(!parsed)
	{
		std::cout << "Parameters have not been parsed.\n";
	}
    else if(option == RSGISExeSegment::specgrp)
    {
        std::cout << "Locally spectrally cluster/group the pixels into clumps.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Spectral Threshold: " << this->specThreshold << std::endl;
        if(processInMemory)
        {
            std::cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            std::cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::specgrpweighted)
    {
        std::cout << "Locally spectrally cluster/group the pixels into clumps using stddev to weight distance metric.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Spectral Threshold: " << this->specThreshold << std::endl;
        if(processInMemory)
        {
            std::cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            std::cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::meanimg)
    {
        std::cout << "Create a mean spectral image from a clumped image.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        if(processInMemory)
        {
            std::cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            std::cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::rmsmallclumps)
    {
        std::cout << "Eliminate clumps smaller than a given size from the scene.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Spectral Threshold: " << this->specThreshold << std::endl;
        std::cout << "Min Clump Size: " << this->minClumpSize << std::endl;
        if(processInMemory)
        {
            std::cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            std::cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::rmsmallclumpsstepwise)
    {
        std::cout << "Eliminate clumps smaller than a given size from the scene using stepwise approach.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Spectral Threshold: " << this->specThreshold << std::endl;
        std::cout << "Min Clump Size: " << this->minClumpSize << std::endl;
        if(processInMemory)
        {
            std::cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            std::cout << "Processing to undertaken from disk\n";
        }
        if(this->storeMean)
        {
            std::cout << "Mean values are stored in memory.\n";
        }
        else
        {
            std::cout << "Mean values are calculated at runtime.\n";
        }
    }
    else if(option == RSGISExeSegment::clump)
    {
        std::cout << "Clump a categorical image into clumps with unique IDs.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        if(processInMemory)
        {
            std::cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            std::cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::randomcolourclumps)
    {
        std::cout << "Colour clumps randomly for visualisation\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
    }
    else if(option == RSGISExeSegment::regiongrowingseedclumpids)
    {
        std::cout << "Find clump ids for pixels and output region growing seeds.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output file: " << this->outputTextFile << std::endl;
    }
    else if(option == RSGISExeSegment::growregionsusingclumps)
    {
        std::cout << "A command to region grow from clump seeds using the clumps as base units.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Seeds File: " << this->seedsTextFile << std::endl;
        std::cout << "Initial Threshold: " << this->initRegionGrowthreshold << std::endl;
        std::cout << "Threshold Increments: " << this->thresholdRegionGrowincrements << std::endl;
        std::cout << "Maximum Threshold: " << this->maxRegionGrowthreshold << std::endl;
        std::cout << "Maximum Number of Iterations: " << this->maxRegionGrowiterations << std::endl;
    }
    else if(option == RSGISExeSegment::histogramseeds)
    {
        std::cout << "Create a mean spectral image from a clumped image.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        for(std::vector<rsgis::segment::BandThreshold>::iterator iterThreshold = bandThresholds->begin(); iterThreshold != bandThresholds->end(); ++iterThreshold)
        {
            std::cout << "Band " << (*iterThreshold).band << " threshold = " << (*iterThreshold).threshold << std::endl;
        }
    }
    else if(option == RSGISExeSegment::histogramseedstxt)
    {
        std::cout << "Create a mean spectral image from a clumped image.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output File: " << this->seedsTextFile << std::endl;
        for(std::vector<rsgis::segment::BandThreshold>::iterator iterThreshold = bandThresholds->begin(); iterThreshold != bandThresholds->end(); ++iterThreshold)
        {
            std::cout << "Band " << (*iterThreshold).band << " threshold = " << (*iterThreshold).threshold << std::endl;
        }
    }
    else if(option == RSGISExeSegment::selectclumps)
    {
        std::cout << "Select clumps within larger parent clumps with highest/lowest value.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clumps Image: " << this->clumpsImage << std::endl;
        std::cout << "Large Clumps Image: " << this->largeClumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        if(this->selectionMethod == rsgis::segment::RSGISSelectClumps::max)
        {
            std::cout << "Selecting clumps with highest values.\n";
        }
        else if (this->selectionMethod == rsgis::segment::RSGISSelectClumps::min)
        {
            std::cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == rsgis::segment::RSGISSelectClumps::mean)
        {
            std::cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == rsgis::segment::RSGISSelectClumps::median)
        {
            std::cout << "Selecting clumps with lowest values.\n";
        }
    }
    else if(option == RSGISExeSegment::selectclumpstxt)
    {
        std::cout << "Select clumps within larger parent clumps with highest/lowest value.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clumps Image: " << this->clumpsImage << std::endl;
        std::cout << "Large Clumps Image: " << this->largeClumpsImage << std::endl;
        std::cout << "Output Seeds File: " << this->seedsTextFile << std::endl;
        if(this->selectionMethod == rsgis::segment::RSGISSelectClumps::max)
        {
            std::cout << "Selecting clumps with highest values.\n";
        }
        else if (this->selectionMethod == rsgis::segment::RSGISSelectClumps::min)
        {
            std::cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == rsgis::segment::RSGISSelectClumps::mean)
        {
            std::cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == rsgis::segment::RSGISSelectClumps::median)
        {
            std::cout << "Selecting clumps with lowest values.\n";
        }
    }
    else if(option == RSGISExeSegment::mergesmallclumps)
    {
        std::cout << "Merge small clumps to one another or neighbouring larger clumps.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        if(processInMemory)
        {
            std::cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            std::cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::labelsfromclusters)
    {
        std::cout << "A command which labels image pixels with the id of the nearest cluster centre.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Cluster Matrix: " << this->clustersMatrix << std::endl;
        if(ignoreZeros)
        {
            std::cout << "Ignoring Zeros\n";
        }
    }
    else if(option == RSGISExeSegment::growregionspixels)
    {
        std::cout << "A command to grow regions from seeds up to a given distance in spectral space.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Threshold: " << this->specThreshold << std::endl;
        if(processInMemory)
        {
            std::cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            std::cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::growregionspixelsauto)
    {
        std::cout << "A command to region grow from seeds on a per pixel basis.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Initial Threshold: " << this->initRegionGrowthreshold << std::endl;
        std::cout << "Threshold Increments: " << this->thresholdRegionGrowincrements << std::endl;
        std::cout << "Maximum Threshold: " << this->maxRegionGrowthreshold << std::endl;
        std::cout << "Maximum Number of Iterations: " << this->maxRegionGrowiterations << std::endl;
        if(processInMemory)
        {
            std::cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            std::cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::spectraldiv)
    {
        std::cout << "Finding spectral division using a defined subdivision for bands.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        std::cout << "Sub division: " << this->subDivision << std::endl;
        std::cout << "Number of Levels: " << this->levels << std::endl;
        if(this->noDataValProvided)
        {
            std::cout << "No Data Value: " << this->noDataVal << std::endl;
        }
    }
    else if(option == RSGISExeSegment::stepwiseelimination)
    {
        std::cout << "Eliminate clumps smaller than a given size from the scene using stepwise approach using an attribute table.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        if(!this->processInMemory)
        {
            std::cout << "Temp Table: " << this->tempTable << std::endl;
            std::cout << "Cache Size: " << this->cacheSize << std::endl;
        }
        if(this->outputWithConsecutiveFIDs)
        {
            std::cout << "Outputting with consecutive FIDs\n";
        }
    }
    else if(option == RSGISExeSegment::elimsinglepxls)
    {
        std::cout << "A command which iteratively removes single pixel labelled neighbours into spectrally closest large group.\n";
        std::cout << "Input Image: " << this->inputImage << std::endl;
        std::cout << "Clump Image: " << this->clumpsImage << std::endl;
        std::cout << "Output Image: " << this->outputImage << std::endl;
        if(!this->processInMemory)
        {
            std::cout << "Temp Image: " << this->tempImage << std::endl;
        }
        if(this->ignoreZeros)
        {
            std::cout << "Ignoring pixels with a value of zero in clumps file\n";
        }
    }
    else if(option == RSGISExeSegment::unionsegments)
    {
        std::cout << "Combine segmentations creating a single segmentation through an union\n";
        std::cout << "Output File: " << this->outputImage << std::endl;
        std::cout << "Output Format: " << this->imageFormat << std::endl;
        if(noDataValProvided)
        {
            std::cout << "No Data: " << this->noDataVal << std::endl;
        }
        else
        {
            std::cout << "A no data value was not provided\n";
        }
        std::cout << "Input Images: \n";
        for(std::vector<std::string>::iterator iterFiles = inputImagePaths.begin(); iterFiles != inputImagePaths.end(); ++iterFiles)
        {
            std::cout << "\t" << *iterFiles << std::endl;
        }
    }
    else if(option == RSGISExeSegment::mergeclumptiles)
    {
        std::cout << "A command to merge tiles of image clumps into an existing image file.\n";
        std::cout << "Output File: " << this->outputImage << std::endl;
        std::cout << "Tile Info Column: " << this->colsName << std::endl;
        std::cout << "Tile Overlap: " << this->tileOverlap << std::endl;
        std::cout << "Tile Boundary: " << this->tileBoundary << std::endl;
        std::cout << "Tile Body: " << this->tileBody << std::endl;
        std::cout << "Input Clump Images: \n";
        for(std::vector<std::string>::iterator iterFiles = inputImagePaths.begin(); iterFiles != inputImagePaths.end(); ++iterFiles)
        {
            std::cout << "\t" << *iterFiles << std::endl;
        }
    }
    else
    {
        std::cout << "RSGISExeSegment: Options not recognised\n";
    }
}

void RSGISExeSegment::help()
{
    std::cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << std::endl;
    std::cout << "<!-- A command to locally spectrally cluster/group the pixels into clumps -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"specgrp\" image=\"image.env\" output=\"image_out.env\" specthreshold=\"float\" nodata=\"unsigned int\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to locally spectrally cluster/group the pixels into clumps where the distance metric is weighted by the stddev of the band -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"specgrpweighted\" image=\"image.env\" output=\"image_out.env\" specthreshold=\"float\" nodata=\"unsigned int\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to generate a mean image from a spectral image and a clumped image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"meanimg\" image=\"image.env\" clumps=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to eliminate clumps smaller than a given size from the scene, small clump will be combined with their spectrally closest neighbouring clump unless over spectral distance threshold -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"rmsmallclumps\" image=\"image.env\" clumps=\"image.env\" output=\"image_out.env\" minsize=\"int (in pxls)\" maxspectraldist=\"float\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to eliminate clumps smaller than a given size from the scene, small clump will be combined with their spectrally closest neighbouring clump in a stepwise fashion unless over spectral distance threshold -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"rmsmallclumpsstepwise\" image=\"image.env\" clumps=\"image.env\" output=\"image_out.env\" minsize=\"int (in pxls)\" maxspectraldist=\"float\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to clump a categorical image into clumps with unique IDs -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"clump\" image=\"image.env\" output=\"image_out.env\" nodata=\"unsigned int\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to colour clumps using random colours for visualisation -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"randomcolourclumps\" image=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- Generate an from a list of pixel locations generate a list of clump ids which will be the seeds for a region growing algorithm -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"regiongrowingseedclumpids\" image=\"clumpsimage.env\" output=\"clumpids.txt\" >" << std::endl;
    std::cout << "    <rsgis:seed x=\"int\" y=\"int\" id=\"int\" />" << std::endl;
    std::cout << "    <rsgis:seed x=\"int\" y=\"int\" id=\"int\" />" << std::endl;
    std::cout << "    <rsgis:seed x=\"int\" y=\"int\" id=\"int\" />" << std::endl;
    std::cout << "    <rsgis:seed x=\"int\" y=\"int\" id=\"int\" />" << std::endl;
    std::cout << "    <rsgis:seed x=\"int\" y=\"int\" id=\"int\" />" << std::endl;
    std::cout << "    <rsgis:seed x=\"int\" y=\"int\" id=\"int\" />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "<!-- A command to grow regions from seeds to there maximum extent using the clumps. -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"growregionsusingclumps\" image=\"image.env\" clumps=\"image.env\" output=\"image_out.env\" seeds=\"seedsfile.txt\" initthreshold=\"float\" thresholdincrements=\"float\" maxthreshold=\"float\" maxiterations=\"int\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to identify seeds for region growing -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"histogramseeds\" image=\"image.env\" clumps=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" >" << std::endl;
    std::cout << "    <rsgis:threshold band=\"int\" threshold=\"float\" />" << std::endl;
    std::cout << "    <rsgis:threshold band=\"int\" threshold=\"float\" />" << std::endl;
    std::cout << "    <rsgis:threshold band=\"int\" threshold=\"float\" />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "<!-- A command to identify seeds for region growing - output is a text file for input into growregionsusingclumps -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"histogramseedstxt\" image=\"image.env\" clumps=\"image.env\" output=\"seedsfile.txt\" inmemory=\"yes | no\"  >" << std::endl;
    std::cout << "    <rsgis:threshold band=\"int\" threshold=\"float\" />" << std::endl;
    std::cout << "    <rsgis:threshold band=\"int\" threshold=\"float\" />" << std::endl;
    std::cout << "    <rsgis:threshold band=\"int\" threshold=\"float\" />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "<!-- A command which selects the clump within the clumps file which intersect with either the minimum or maximum mean clump -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"selectclumps\" image=\"image.env\" clumps=\"image.env\" largeclumps=\"image.env\" output=\"image_out.env\" type=\"max | min | mean | median | percentile75th | percentile95th\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command which selects the clump within the clumps file which intersect with either the minimum or maximum mean clump with output to a text file.-->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"selectclumpstxt\" image=\"image.env\" clumps=\"image.env\" largeclumps=\"image.env\" output=\"seedsfile.txt\" type=\"max | min | mean | median | percentile75th | percentile95th\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to remove small clumps from a segmentation -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"mergesmallclumps\" image=\"image.env\" clumps=\"image.env\" output=\"image_out.env\" minsize=\"int (in pxls)\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to relabel clumps to ensure they are ordered consecutively -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"relabelclumps\" image=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command which labels image pixels with the id of the nearest cluster centre -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"labelsfromclusters\" image=\"image.env\" output=\"image_out.env\" clusters=\"matrix.gmtxt\" ignorezeros=\"yes | no\" format=\"GDAL Format\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to grow regions from seeds up to a given distance in spectral space. -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"growregionspixels\" image=\"image.env\" output=\"image_out.env\" seeds=\"seedsfile.txt\" threshold=\"float\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to grow regions from seeds to there maximum extent on a per pixel bases. -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"growregionspixelsauto\" image=\"image.env\" output=\"image_out.env\" seeds=\"seedsfile.txt\" initthreshold=\"float\" thresholdincrements=\"float\" maxthreshold=\"float\" maxiterations=\"int\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to find spectral categories using a defined subdivision and number of levels -->" << std::endl;
    std::cout << "<rsgis:command algor=\"segmentation\" option=\"spectraldiv\" image=\"image.env\" output=\"image_out.env\" subdivision=\"int\" nodata=\"unsigned int\" format=\"GDAL Format\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "</rsgis:commands>" << std::endl;
}

std::string RSGISExeSegment::getDescription()
{
	return "Provides an output image of the overlapping area containing all the input bands.";
}

std::string RSGISExeSegment::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeSegment::~RSGISExeSegment()
{
	
}

}

