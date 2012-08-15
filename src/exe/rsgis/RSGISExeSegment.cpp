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

RSGISExeSegment::RSGISExeSegment() : RSGISAlgorithmParameters()
{
	this->algorithm = "segmentation";
	this->inputImage = "";
	this->outputImage = "";
}

RSGISAlgorithmParameters* RSGISExeSegment::getInstance()
{
	return new RSGISExeSegment();
}

void RSGISExeSegment::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISFileUtils fileUtils;
	RSGISMathsUtils mathUtils;

    XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
    XMLCh *optionSpecGrp = XMLString::transcode("specgrp");
    XMLCh *optionMeanImg = XMLString::transcode("meanimg");
    XMLCh *optionRMSmallClumps = XMLString::transcode("rmsmallclumps");
    XMLCh *optionRMSmallClumpsStepwise = XMLString::transcode("rmsmallclumpsstepwise");
    XMLCh *optionClump = XMLString::transcode("clump");
    XMLCh *optionRandomColourClumps = XMLString::transcode("randomcolourclumps");
    XMLCh *optionRegionGrowingSeedClumpIds = XMLString::transcode("regiongrowingseedclumpids");
    XMLCh *optionGrowRegionsUsingClumps = XMLString::transcode("growregionsusingclumps");
    XMLCh *optionHistogramSeeds = XMLString::transcode("histogramseeds");
    XMLCh *optionHistogramSeedsTxt = XMLString::transcode("histogramseedstxt");
    XMLCh *optionSelectClumps = XMLString::transcode("selectclumps");
    XMLCh *optionSelectClumpsTxt = XMLString::transcode("selectclumpstxt");
    XMLCh *optionMergeSmallClumps = XMLString::transcode("mergesmallclumps");
    XMLCh *optionRelabelClumps = XMLString::transcode("relabelclumps");
    XMLCh *optionSpecGrpWeighted = XMLString::transcode("specgrpweighted");
    XMLCh *optionLabelsFromClusters = XMLString::transcode("labelsfromclusters");
    XMLCh *optionLabelsFromPixels = XMLString::transcode("labelsfrompixels");
    XMLCh *optionGrowRegionsPixels = XMLString::transcode("growregionspixels");
    XMLCh *optionGrowRegionsPixelsAuto = XMLString::transcode("growregionspixelsauto");
    XMLCh *optionSpectralDiv = XMLString::transcode("spectraldiv");
    XMLCh *optionStepwiseElimination = XMLString::transcode("stepwiseelimination");
    XMLCh *optionElimSinglePxls = XMLString::transcode("elimsinglepxls");
    
    XMLCh *projImage = XMLString::transcode("IMAGE");
	XMLCh *projOSGB = XMLString::transcode("OSGB");
    XMLCh *projNZ2000 = XMLString::transcode("NZ2000");
    XMLCh *projNZ1949 = XMLString::transcode("NZ1949");
    
    const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	
	const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
	if(XMLString::equals(optionSpecGrp, optionXML))
    {
        this->option = RSGISExeSegment::specgrp;
		
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
        
        
        XMLCh *specThresholdXMLStr = XMLString::transcode("specthreshold");
		if(argElement->hasAttribute(specThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(specThresholdXMLStr));
			this->specThreshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'specthreshold\' attribute was provided.");
        }
		XMLString::release(&specThresholdXMLStr);
        
        XMLCh *noDataXMLStr = XMLString::transcode("nodata");
		if(argElement->hasAttribute(noDataXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(noDataXMLStr));
			this->noDataVal = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
            this->noDataValProvided = true;
		}
        else
        {
            this->noDataValProvided = false;
        }
		XMLString::release(&noDataXMLStr);
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionMeanImg, optionXML))
    {
        this->option = RSGISExeSegment::meanimg;
		
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
		
        XMLCh *clumpsXMLStr = XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		XMLString::release(&clumpsXMLStr);
        
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
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionRMSmallClumps, optionXML))
    {
        this->option = RSGISExeSegment::rmsmallclumps;
		
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
		
        XMLCh *clumpsXMLStr = XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		XMLString::release(&clumpsXMLStr);
        
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
        
        XMLCh *specThresholdXMLStr = XMLString::transcode("maxspectraldist");
		if(argElement->hasAttribute(specThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(specThresholdXMLStr));
			this->specThreshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'maxspectraldist\' attribute was provided.");
        }
		XMLString::release(&specThresholdXMLStr);
        
        XMLCh *minSizeXMLStr = XMLString::transcode("minsize");
		if(argElement->hasAttribute(minSizeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minSizeXMLStr));
			this->minClumpSize = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'minsize\' attribute was provided.");
        }
		XMLString::release(&minSizeXMLStr);
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionRMSmallClumpsStepwise, optionXML))
    {
        this->option = RSGISExeSegment::rmsmallclumpsstepwise;
		
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
		
        XMLCh *clumpsXMLStr = XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		XMLString::release(&clumpsXMLStr);
        
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
        
        stretchStatsAvail = false;
        XMLCh *stretchStatsXMLStr = XMLString::transcode("stretchstats");
		if(argElement->hasAttribute(stretchStatsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(stretchStatsXMLStr));
			this->stretchStatsFile = string(charValue);
			XMLString::release(&charValue);
            stretchStatsAvail = true;
		}
		else
		{
			stretchStatsAvail = false;
		}
		XMLString::release(&stretchStatsXMLStr);
        
        XMLCh *specThresholdXMLStr = XMLString::transcode("maxspectraldist");
		if(argElement->hasAttribute(specThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(specThresholdXMLStr));
			this->specThreshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'maxspectraldist\' attribute was provided.");
        }
		XMLString::release(&specThresholdXMLStr);
        
        XMLCh *minSizeXMLStr = XMLString::transcode("minsize");
		if(argElement->hasAttribute(minSizeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minSizeXMLStr));
			this->minClumpSize = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'minsize\' attribute was provided.");
        }
		XMLString::release(&minSizeXMLStr);
        
        
        XMLCh *storeMeanXMLStr = XMLString::transcode("storemean");
		if(argElement->hasAttribute(storeMeanXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *storeMValue = argElement->getAttribute(storeMeanXMLStr);
			
			if(XMLString::equals(storeMValue, yesStr))
			{
				this->storeMean = true;
			}
			else
			{
				this->storeMean = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
            this->storeMean = true;
		}
		XMLString::release(&storeMeanXMLStr);
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionClump, optionXML))
    {
        this->option = RSGISExeSegment::clump;
		
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
        
        XMLCh *noDataXMLStr = XMLString::transcode("nodata");
		if(argElement->hasAttribute(noDataXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(noDataXMLStr));
			this->noDataVal = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
            this->noDataValProvided = true;
		}
        else
        {
            this->noDataValProvided = false;
        }
		XMLString::release(&noDataXMLStr);
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionRandomColourClumps, optionXML))
    {
        this->option = RSGISExeSegment::randomcolourclumps;
		
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
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
        
        XMLCh *importLUTXMLStr = XMLString::transcode("importLUT");
		if(argElement->hasAttribute(importLUTXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(importLUTXMLStr));
			this->importLUTFile = string(charValue);
            this->importLUT = true;
			XMLString::release(&charValue);
		}
		else
		{
			this->importLUT = false;
		}
		XMLString::release(&importLUTXMLStr);
        
        
        XMLCh *exportLUTXMLStr = XMLString::transcode("exportLUT");
		if(argElement->hasAttribute(exportLUTXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(exportLUTXMLStr));
			this->exportLUTFile = string(charValue);
            this->exportLUT = true;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportLUT = false;
		}
		XMLString::release(&exportLUTXMLStr);
        
    }
    else if(XMLString::equals(optionRegionGrowingSeedClumpIds, optionXML))
    {
        this->option = RSGISExeSegment::regiongrowingseedclumpids;
		
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
			this->outputTextFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        XMLCh *rsgisSeedsXMLStr = XMLString::transcode("rsgis:seed");
        DOMNodeList *seedNodesList = argElement->getElementsByTagName(rsgisSeedsXMLStr);
		unsigned int numSeeds = seedNodesList->getLength();		
		if(numSeeds > 0)
		{
            seedPxls = new vector<ImgSeeds>();
            seedPxls->reserve(numSeeds);
			DOMElement *seedElement = NULL;
            unsigned int x = 0;
            unsigned int y = 0;
            unsigned long id = 0;
			for(unsigned int i = 0; i < numSeeds; i++)
			{
				seedElement = static_cast<DOMElement*>(seedNodesList->item(i));
				
				XMLCh *xXMLStr = XMLString::transcode("x");
				if(seedElement->hasAttribute(xXMLStr))
				{
					char *charValue = XMLString::transcode(seedElement->getAttribute(xXMLStr));
					x = mathUtils.strtounsignedint(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'x\' attribute was provided for seed.");
				}
				XMLString::release(&xXMLStr);
                
                XMLCh *yXMLStr = XMLString::transcode("y");
				if(seedElement->hasAttribute(yXMLStr))
				{
					char *charValue = XMLString::transcode(seedElement->getAttribute(yXMLStr));
					y = mathUtils.strtounsignedint(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'y\' attribute was provided for seed.");
				}
				XMLString::release(&yXMLStr);
                
                XMLCh *idXMLStr = XMLString::transcode("id");
				if(seedElement->hasAttribute(idXMLStr))
				{
					char *charValue = XMLString::transcode(seedElement->getAttribute(idXMLStr));
					id = mathUtils.strtounsignedlong(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'id\' attribute was provided for seed.");
				}
				XMLString::release(&idXMLStr);
                
                seedPxls->push_back(ImgSeeds(x,y,id));
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No attributes \'rsgis:seed\' tags were provided.");
		}
        XMLString::release(&rsgisSeedsXMLStr);
    }
    else if(XMLString::equals(optionGrowRegionsUsingClumps, optionXML))
    {
        this->option = RSGISExeSegment::growregionsusingclumps;
		
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
		
        XMLCh *clumpsXMLStr = XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		XMLString::release(&clumpsXMLStr);
        
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
        
        XMLCh *seedsXMLStr = XMLString::transcode("seeds");
		if(argElement->hasAttribute(seedsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(seedsXMLStr));
			this->seedsTextFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'seeds\' attribute was provided.");
		}
		XMLString::release(&seedsXMLStr);
        
        XMLCh *initThresholdXMLStr = XMLString::transcode("initthreshold");
		if(argElement->hasAttribute(initThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(initThresholdXMLStr));
			this->initRegionGrowthreshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'initthreshold\' attribute was provided.");
        }
		XMLString::release(&initThresholdXMLStr);
        
        XMLCh *thresholdIncrementsXMLStr = XMLString::transcode("thresholdincrements");
		if(argElement->hasAttribute(thresholdIncrementsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(thresholdIncrementsXMLStr));
			this->thresholdRegionGrowincrements = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'thresholdincrements\' attribute was provided.");
        }
		XMLString::release(&thresholdIncrementsXMLStr);
        
        XMLCh *maxThresholdXMLStr = XMLString::transcode("maxthreshold");
		if(argElement->hasAttribute(maxThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxThresholdXMLStr));
			this->maxRegionGrowthreshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'maxthreshold\' attribute was provided.");
        }
		XMLString::release(&maxThresholdXMLStr);
        
        XMLCh *maxIterationsXMLStr = XMLString::transcode("maxiterations");
		if(argElement->hasAttribute(maxIterationsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
			this->maxRegionGrowiterations = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'maxiterations\' attribute was provided.");
        }
        XMLString::release(&maxIterationsXMLStr);
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionHistogramSeeds, optionXML))
    {
        this->option = RSGISExeSegment::histogramseeds;
		
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
		
        XMLCh *clumpsXMLStr = XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		XMLString::release(&clumpsXMLStr);
        
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
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
        
        
        XMLCh *rsgisThresholdXMLStr = XMLString::transcode("rsgis:threshold");
        DOMNodeList *thresholdNodesList = argElement->getElementsByTagName(rsgisThresholdXMLStr);
		unsigned int numThresholds = thresholdNodesList->getLength();		
		if(numThresholds > 0)
		{
            bandThresholds = new vector<BandThreshold>();
            bandThresholds->reserve(numThresholds);
			DOMElement *thresholdElement = NULL;
            unsigned int band = 0;
            float threshold = 0;

			for(unsigned int i = 0; i < numThresholds; i++)
			{
				thresholdElement = static_cast<DOMElement*>(thresholdNodesList->item(i));
				
				XMLCh *bandXMLStr = XMLString::transcode("band");
				if(thresholdElement->hasAttribute(bandXMLStr))
				{
					char *charValue = XMLString::transcode(thresholdElement->getAttribute(bandXMLStr));
					band = mathUtils.strtounsignedint(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'band\' attribute was provided for seed.");
				}
				XMLString::release(&bandXMLStr);
                                
                XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
				if(thresholdElement->hasAttribute(thresholdXMLStr))
				{
					char *charValue = XMLString::transcode(thresholdElement->getAttribute(thresholdXMLStr));
					threshold = mathUtils.strtofloat(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'threshold\' attribute was provided for seed.");
				}
				XMLString::release(&thresholdXMLStr);
                
                bandThresholds->push_back(BandThreshold(band,threshold));
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No attributes \'rsgis:threshold\' tags were provided.");
		}
        XMLString::release(&rsgisThresholdXMLStr);
    }
    else if(XMLString::equals(optionHistogramSeedsTxt, optionXML))
    {
        this->option = RSGISExeSegment::histogramseedstxt;
		
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
		
        XMLCh *clumpsXMLStr = XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		XMLString::release(&clumpsXMLStr);
        
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->seedsTextFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        
        XMLCh *rsgisThresholdXMLStr = XMLString::transcode("rsgis:threshold");
        DOMNodeList *thresholdNodesList = argElement->getElementsByTagName(rsgisThresholdXMLStr);
		unsigned int numThresholds = thresholdNodesList->getLength();		
		if(numThresholds > 0)
		{
            bandThresholds = new vector<BandThreshold>();
            bandThresholds->reserve(numThresholds);
			DOMElement *thresholdElement = NULL;
            unsigned int band = 0;
            float threshold = 0;
            
			for(unsigned int i = 0; i < numThresholds; i++)
			{
				thresholdElement = static_cast<DOMElement*>(thresholdNodesList->item(i));
				
				XMLCh *bandXMLStr = XMLString::transcode("band");
				if(thresholdElement->hasAttribute(bandXMLStr))
				{
					char *charValue = XMLString::transcode(thresholdElement->getAttribute(bandXMLStr));
					band = mathUtils.strtounsignedint(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'band\' attribute was provided for seed.");
				}
				XMLString::release(&bandXMLStr);
                
                XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
				if(thresholdElement->hasAttribute(thresholdXMLStr))
				{
					char *charValue = XMLString::transcode(thresholdElement->getAttribute(thresholdXMLStr));
					threshold = mathUtils.strtofloat(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'threshold\' attribute was provided for seed.");
				}
				XMLString::release(&thresholdXMLStr);
                
                bandThresholds->push_back(BandThreshold(band,threshold));
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No attributes \'rsgis:threshold\' tags were provided.");
		}
        XMLString::release(&rsgisThresholdXMLStr);
    }
    else if(XMLString::equals(optionSelectClumps, optionXML))
    {
        this->option = RSGISExeSegment::selectclumps;
		
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
		
        XMLCh *clumpsXMLStr = XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		XMLString::release(&clumpsXMLStr);
        
        XMLCh *largeClumpsXMLStr = XMLString::transcode("largeclumps");
		if(argElement->hasAttribute(largeClumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(largeClumpsXMLStr));
			this->largeClumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'largeclumps\' attribute was provided.");
		}
		XMLString::release(&largeClumpsXMLStr);
        
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
        
        
        XMLCh *typeXMLStr = XMLString::transcode("type");
		if(argElement->hasAttribute(typeXMLStr))
		{
			XMLCh *maxStr = XMLString::transcode("max");
            XMLCh *minStr = XMLString::transcode("min");
            XMLCh *meanStr = XMLString::transcode("mean");
            XMLCh *medianStr = XMLString::transcode("median");
            XMLCh *percentile75thStr = XMLString::transcode("percentile75th");
            XMLCh *percentile95thStr = XMLString::transcode("percentile95th");
			const XMLCh *inMemValue = argElement->getAttribute(typeXMLStr);
			
			if(XMLString::equals(inMemValue, maxStr))
			{
				this->selectionMethod = RSGISSelectClumps::max;
			}
            else if(XMLString::equals(inMemValue, minStr))
			{
				this->selectionMethod = RSGISSelectClumps::min;
			}
            else if(XMLString::equals(inMemValue, meanStr))
			{
				this->selectionMethod = RSGISSelectClumps::mean;
			}
            else if(XMLString::equals(inMemValue, medianStr))
			{
				this->selectionMethod = RSGISSelectClumps::median;
			}
            else if(XMLString::equals(inMemValue, percentile75thStr))
			{
				this->selectionMethod = RSGISSelectClumps::percent75th;
			}
            else if(XMLString::equals(inMemValue, percentile95thStr))
			{
				this->selectionMethod = RSGISSelectClumps::percent95th;
			}
			else
			{
				throw RSGISXMLArgumentsException("Selection method not recognised.");
			}
			XMLString::release(&maxStr);
            XMLString::release(&minStr);
            XMLString::release(&meanStr);
            XMLString::release(&medianStr);
            XMLString::release(&percentile75thStr);
            XMLString::release(&percentile95thStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'type\' attribute was provided.");
		}
		XMLString::release(&typeXMLStr);
        
        
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionSelectClumpsTxt, optionXML))
    {
        this->option = RSGISExeSegment::selectclumpstxt;
		
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
		
        XMLCh *clumpsXMLStr = XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		XMLString::release(&clumpsXMLStr);
        
        XMLCh *largeClumpsXMLStr = XMLString::transcode("largeclumps");
		if(argElement->hasAttribute(largeClumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(largeClumpsXMLStr));
			this->largeClumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'largeclumps\' attribute was provided.");
		}
		XMLString::release(&largeClumpsXMLStr);
        
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->seedsTextFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        
        XMLCh *typeXMLStr = XMLString::transcode("type");
		if(argElement->hasAttribute(typeXMLStr))
		{
			XMLCh *maxStr = XMLString::transcode("max");
            XMLCh *minStr = XMLString::transcode("min");
            XMLCh *meanStr = XMLString::transcode("mean");
            XMLCh *medianStr = XMLString::transcode("median");
            XMLCh *percentile75thStr = XMLString::transcode("percentile75th");
            XMLCh *percentile95thStr = XMLString::transcode("percentile95th");
			const XMLCh *inMemValue = argElement->getAttribute(typeXMLStr);
			
			if(XMLString::equals(inMemValue, maxStr))
			{
				this->selectionMethod = RSGISSelectClumps::max;
			}
            else if(XMLString::equals(inMemValue, minStr))
			{
				this->selectionMethod = RSGISSelectClumps::min;
			}
            else if(XMLString::equals(inMemValue, meanStr))
			{
				this->selectionMethod = RSGISSelectClumps::mean;
			}
            else if(XMLString::equals(inMemValue, medianStr))
			{
				this->selectionMethod = RSGISSelectClumps::median;
			}
            else if(XMLString::equals(inMemValue, percentile75thStr))
			{
				this->selectionMethod = RSGISSelectClumps::percent75th;
			}
            else if(XMLString::equals(inMemValue, percentile95thStr))
			{
				this->selectionMethod = RSGISSelectClumps::percent95th;
			}
			else
			{
				throw RSGISXMLArgumentsException("Selection method not recognised.");
			}
			XMLString::release(&maxStr);
            XMLString::release(&minStr);
            XMLString::release(&meanStr);
            XMLString::release(&medianStr);
            XMLString::release(&percentile75thStr);
            XMLString::release(&percentile95thStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'type\' attribute was provided.");
		}
		XMLString::release(&typeXMLStr);        
        
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionMergeSmallClumps, optionXML))
    {
        this->option = RSGISExeSegment::mergesmallclumps;
		
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
		
        XMLCh *clumpsXMLStr = XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		XMLString::release(&clumpsXMLStr);
        
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
        
        XMLCh *minSizeXMLStr = XMLString::transcode("minsize");
		if(argElement->hasAttribute(minSizeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minSizeXMLStr));
			this->minClumpSize = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'minsize\' attribute was provided.");
        }
		XMLString::release(&minSizeXMLStr);
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionRelabelClumps, optionXML))
    {
        this->option = RSGISExeSegment::relabelclumps;
		
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
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionSpecGrpWeighted, optionXML))
    {
        this->option = RSGISExeSegment::specgrpweighted;
		
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
        
        
        XMLCh *specThresholdXMLStr = XMLString::transcode("specthreshold");
		if(argElement->hasAttribute(specThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(specThresholdXMLStr));
			this->specThreshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'specthreshold\' attribute was provided.");
        }
		XMLString::release(&specThresholdXMLStr);
        
        XMLCh *noDataXMLStr = XMLString::transcode("nodata");
		if(argElement->hasAttribute(noDataXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(noDataXMLStr));
			this->noDataVal = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
            this->noDataValProvided = true;
		}
        else
        {
            this->noDataValProvided = false;
        }
		XMLString::release(&noDataXMLStr);
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionLabelsFromClusters, optionXML))
    {
        this->option = RSGISExeSegment::labelsfromclusters;
		
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
        
        XMLCh *clustersXMLStr = XMLString::transcode("clusters");
		if(argElement->hasAttribute(clustersXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clustersXMLStr));
			this->clustersMatrix = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'clusters\' attribute was provided.");
		}
		XMLString::release(&clustersXMLStr);
        
        
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
        
        
       
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
                
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionLabelsFromPixels, optionXML))
    {
        this->option = RSGISExeSegment::labelsfrompixels;
		
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
        
        
        
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionGrowRegionsPixels, optionXML))
    {
        this->option = RSGISExeSegment::growregionspixels;
		
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
        
        XMLCh *seedsXMLStr = XMLString::transcode("seeds");
		if(argElement->hasAttribute(seedsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(seedsXMLStr));
            try 
            {
                this->regionGrowingPxlSeeds = RSGISRegionGrowSegmentsPixels::parseSeedsText(string(charValue));
            } 
            catch (RSGISTextException &e) 
            {
                throw RSGISXMLArgumentsException(e.what());
            }
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'seeds\' attribute was provided.");
		}
		XMLString::release(&seedsXMLStr);
        
        XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			this->specThreshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
        }
		XMLString::release(&thresholdXMLStr);
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionGrowRegionsPixelsAuto, optionXML))
    {
        this->option = RSGISExeSegment::growregionspixelsauto;
		
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
        
        XMLCh *seedsXMLStr = XMLString::transcode("seeds");
		if(argElement->hasAttribute(seedsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(seedsXMLStr));
            try 
            {
                this->regionGrowingPxlSeeds = RSGISRegionGrowSegmentsPixels::parseSeedsText(string(charValue));
            } 
            catch (RSGISTextException &e) 
            {
                throw RSGISXMLArgumentsException(e.what());
            }
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'seeds\' attribute was provided.");
		}
		XMLString::release(&seedsXMLStr);
        
        XMLCh *initThresholdXMLStr = XMLString::transcode("initthreshold");
		if(argElement->hasAttribute(initThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(initThresholdXMLStr));
			this->initRegionGrowthreshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'initthreshold\' attribute was provided.");
        }
		XMLString::release(&initThresholdXMLStr);
        
        XMLCh *thresholdIncrementsXMLStr = XMLString::transcode("thresholdincrements");
		if(argElement->hasAttribute(thresholdIncrementsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(thresholdIncrementsXMLStr));
			this->thresholdRegionGrowincrements = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'thresholdincrements\' attribute was provided.");
        }
		XMLString::release(&thresholdIncrementsXMLStr);
        
        XMLCh *maxThresholdXMLStr = XMLString::transcode("maxthreshold");
		if(argElement->hasAttribute(maxThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxThresholdXMLStr));
			this->maxRegionGrowthreshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'maxthreshold\' attribute was provided.");
        }
		XMLString::release(&maxThresholdXMLStr);
        
        XMLCh *maxIterationsXMLStr = XMLString::transcode("maxiterations");
		if(argElement->hasAttribute(maxIterationsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
			this->maxRegionGrowiterations = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'maxiterations\' attribute was provided.");
        }
        XMLString::release(&maxIterationsXMLStr);
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionSpectralDiv, optionXML))
    {
        this->option = RSGISExeSegment::spectraldiv;
		
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
        
        XMLCh *subDivisionXMLStr = XMLString::transcode("subdivision");
		if(argElement->hasAttribute(subDivisionXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(subDivisionXMLStr));
			this->subDivision = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'subdivision\' attribute was provided.");
        }
		XMLString::release(&subDivisionXMLStr);
                
        
        XMLCh *noDataXMLStr = XMLString::transcode("nodata");
		if(argElement->hasAttribute(noDataXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(noDataXMLStr));
			this->noDataVal = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
            this->noDataValProvided = true;
		}
        else
        {
            this->noDataValProvided = false;
        }
		XMLString::release(&noDataXMLStr);
        
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionStepwiseElimination, optionXML))
    {
        this->option = RSGISExeSegment::stepwiseelimination;
		
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
		
        XMLCh *clumpsXMLStr = XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		XMLString::release(&clumpsXMLStr);
        
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
        
        stretchStatsAvail = false;
        XMLCh *stretchStatsXMLStr = XMLString::transcode("stretchstats");
		if(argElement->hasAttribute(stretchStatsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(stretchStatsXMLStr));
			this->stretchStatsFile = string(charValue);
			XMLString::release(&charValue);
            stretchStatsAvail = true;
		}
		else
		{
			stretchStatsAvail = false;
		}
		XMLString::release(&stretchStatsXMLStr);
        
        XMLCh *tmpTableXMLStr = XMLString::transcode("tmptable");
		if(argElement->hasAttribute(tmpTableXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(tmpTableXMLStr));
			this->tempTable = string(charValue);
			XMLString::release(&charValue);
            this->processInMemory = false;
            
            XMLCh *cacheSizeXMLStr = XMLString::transcode("cachesize");
            if(argElement->hasAttribute(cacheSizeXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(cacheSizeXMLStr));
                this->cacheSize = mathUtils.strtounsignedint(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                this->cacheSize = 10000;
            }
            XMLString::release(&cacheSizeXMLStr);

            
		}
		else
		{
			this->processInMemory = true;
		}
		XMLString::release(&tmpTableXMLStr);
        
        XMLCh *outConsecXMLStr = XMLString::transcode("outconsec");
		if(argElement->hasAttribute(outConsecXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *outConsecValue = argElement->getAttribute(outConsecXMLStr);
			
			if(XMLString::equals(outConsecValue, yesStr))
			{
				this->outputWithConsecutiveFIDs = true;
			}
			else
			{
				this->outputWithConsecutiveFIDs = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
            this->outputWithConsecutiveFIDs = true;
		}
		XMLString::release(&outConsecXMLStr);
        
        XMLCh *specThresholdXMLStr = XMLString::transcode("maxspectraldist");
		if(argElement->hasAttribute(specThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(specThresholdXMLStr));
			this->specThreshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'maxspectraldist\' attribute was provided.");
        }
		XMLString::release(&specThresholdXMLStr);
        
        XMLCh *minSizeXMLStr = XMLString::transcode("minsize");
		if(argElement->hasAttribute(minSizeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minSizeXMLStr));
			this->minClumpSize = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
        else
        {
            throw RSGISXMLArgumentsException("No \'minsize\' attribute was provided.");
        }
		XMLString::release(&minSizeXMLStr);
        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);        
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else if(XMLString::equals(optionElimSinglePxls, optionXML))
    {
        this->option = RSGISExeSegment::elimsinglepxls;
		
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
		
        XMLCh *clumpsXMLStr = XMLString::transcode("clumps");
		if(argElement->hasAttribute(clumpsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
			this->clumpsImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
		}
		XMLString::release(&clumpsXMLStr);
        
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
        
        XMLCh *tmpTableXMLStr = XMLString::transcode("temp");
		if(argElement->hasAttribute(tmpTableXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(tmpTableXMLStr));
			this->tempImage = string(charValue);
			XMLString::release(&charValue);
            this->processInMemory = false;
		}
		else
		{
			this->processInMemory = true;
		}
		XMLString::release(&tmpTableXMLStr);
        
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

        
        XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->imageFormat = "ENVI";
		}
		XMLString::release(&formatXMLStr);        
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
    }
    else
    {
        string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeSegment.");
		throw RSGISXMLArgumentsException(message.c_str());
    }
    
    XMLString::release(&algorName);
	XMLString::release(&algorXMLStr);
	XMLString::release(&optionXMLStr);
    XMLString::release(&optionSpecGrp);
    XMLString::release(&optionMeanImg);
    XMLString::release(&optionRMSmallClumps);
    XMLString::release(&optionRMSmallClumpsStepwise);
    XMLString::release(&optionClump);
    XMLString::release(&optionRandomColourClumps);
    XMLString::release(&optionRegionGrowingSeedClumpIds);
    XMLString::release(&optionGrowRegionsUsingClumps);
    XMLString::release(&optionHistogramSeeds);
    XMLString::release(&optionHistogramSeedsTxt);
    XMLString::release(&optionSelectClumps);
    XMLString::release(&optionSelectClumpsTxt);
    XMLString::release(&optionMergeSmallClumps);
    XMLString::release(&optionRelabelClumps);
    XMLString::release(&optionSpecGrpWeighted);
    XMLString::release(&optionLabelsFromClusters);
    XMLString::release(&optionLabelsFromPixels);
    XMLString::release(&optionGrowRegionsPixels);
    XMLString::release(&optionGrowRegionsPixelsAuto);
    XMLString::release(&optionSpectralDiv);
    XMLString::release(&optionStepwiseElimination);
    XMLString::release(&optionElimSinglePxls);
    
    XMLString::release(&projImage);
	XMLString::release(&projOSGB);
    XMLString::release(&projNZ2000);
    XMLString::release(&projNZ1949);
	
	parsed = true; // if all successful, it is parsed
}

void RSGISExeSegment::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
    else if(option == RSGISExeSegment::specgrp)
    {
        cout << "Locally spectrally cluster/group the pixels into clumps.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Spectral Threshold: " << this->specThreshold << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                resultDataset = imgUtils.createCopy(inDataset, 1, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }

            cout << "Performing Segmentation\n";
            RSGISSpecGroupSegmentation specGrpSeg;
            specGrpSeg.performSimpleClump(spectralDataset, resultDataset, this->specThreshold, this->noDataValProvided, noDataVal);
                   
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::specgrpweighted)
    {
        cout << "Locally spectrally cluster/group the pixels into clumps using stddev to weight distance metric.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Spectral Threshold: " << this->specThreshold << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                resultDataset = imgUtils.createCopy(inDataset, 1, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            
            cout << "Performing Segmentation\n";
            RSGISSpecGroupSegmentation specGrpSeg;
            specGrpSeg.performSimpleClumpStdDevWeights(spectralDataset, resultDataset, this->specThreshold, this->noDataValProvided, noDataVal);
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::meanimg)
    {
        cout << "Create a mean spectral image from a clumped image.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                string message = string("Could not open image ") + this->clumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                resultDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                clumpsDataset = inClumpDataset;
                resultDataset = imgUtils.createCopy(inDataset, this->outputImage, this->imageFormat, GDT_Float32, this->projFromImage, this->proj);
            }
            
            cout << "Calculating Mean Image\n";
            RSGISGenMeanSegImage genMeanImg;
            genMeanImg.generateMeanImageUsingClumpTable(spectralDataset, clumpsDataset, resultDataset);
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, this->outputImage, this->imageFormat, GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloatGDALDataset(resultDataset, outDataset);
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::rmsmallclumps)
    {
        cout << "Eliminate clumps smaller than a given size from the scene.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                string message = string("Could not open image ") + this->clumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                resultDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                clumpsDataset = inClumpDataset;
                resultDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            imgUtils.copyUIntGDALDataset(clumpsDataset, resultDataset);
            
            cout << "Eliminant Clumps\n";
            RSGISEliminateSmallClumps eliminate;
            eliminate.eliminateSmallClumps(spectralDataset, resultDataset, minClumpSize, specThreshold);
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::rmsmallclumpsstepwise)
    {
        cout << "Eliminate clumps smaller than a given size from the scene using stepwise approach.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(this->stretchStatsAvail)
        {
            cout << "Statistics File: " << this->stretchStatsFile << endl;
        }
        if(processInMemory)
        {
            cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            cout << "Processing to undertaken from disk\n";
        }
        if(this->storeMean)
        {
            cout << "Mean values are stored in memory.\n";
        }
        else
        {
            cout << "Mean values are calculated at runtime.\n";
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                string message = string("Could not open image ") + this->clumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
                        
            std::vector<BandSpecThresholdStats> *bandStretchStats = NULL;
            if(this->stretchStatsAvail)
            {
                bandStretchStats = rsgis::img::RSGISStretchImage::readBandSpecThresholds(stretchStatsFile);
            }
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                resultDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                clumpsDataset = inClumpDataset;
                resultDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            imgUtils.copyUIntGDALDataset(clumpsDataset, resultDataset);
            
            cout << "Eliminant Clumps\n";
            RSGISEliminateSmallClumps eliminate;
            if(this->storeMean)
            {
                eliminate.stepwiseEliminateSmallClumps(spectralDataset, resultDataset, minClumpSize, specThreshold, bandStretchStats, this->stretchStatsAvail);
            }
            else
            {
                eliminate.stepwiseEliminateSmallClumpsNoMean(spectralDataset, resultDataset, minClumpSize, specThreshold, bandStretchStats, this->stretchStatsAvail);
            }
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(spectralDataset);
                GDALClose(clumpsDataset);
            }
            
            if(this->stretchStatsAvail)
            {
                delete bandStretchStats;
            }
            
            resultDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(inClumpDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        } 
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::clump)
    {
        cout << "Clump a categorical image into clumps with unique IDs.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *catagoryDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                catagoryDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inDataset, catagoryDataset);
                resultDataset = imgUtils.createCopy(inDataset, 1, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                catagoryDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            
            cout << "Performing Clump\n";
            RSGISClumpPxls clumpImg;
            clumpImg.performClump(catagoryDataset, resultDataset, this->noDataValProvided, this->noDataVal);
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(catagoryDataset);
            }
            
            resultDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();  
        } 
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::randomcolourclumps)
    {
        cout << "Colour clumps randomly for visualisation\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *catagoryDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                catagoryDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inDataset, catagoryDataset);
                resultDataset = imgUtils.createCopy(inDataset, 3, "", "MEM", GDT_Byte, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                catagoryDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, 3, this->outputImage, this->imageFormat, GDT_Byte, this->projFromImage, this->proj);
            }
            
            cout << "Generating Random Colours Image\n";
            RSGISRandomColourClumps colourClumps;
            colourClumps.generateRandomColouredClump(catagoryDataset, resultDataset, this->importLUTFile, this->importLUT, this->exportLUTFile, this->exportLUT);
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, 3, this->outputImage, this->imageFormat, GDT_Byte, this->projFromImage, this->proj);
                imgUtils.copyByteGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(catagoryDataset);
            }
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        } 
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::regiongrowingseedclumpids)
    {
        cout << "Find clump ids for pixels and output region growing seeds.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output file: " << this->outputTextFile << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            cout << "Exporting clump IDs for seeds\n";
            RSGISFindClumpIDs findClumpIDs;
            findClumpIDs.exportClumpIDsAsTxtFile(inDataset, this->outputTextFile, this->seedPxls);
            
            // Tidy up
            GDALClose(inDataset);
            delete this->seedPxls;
        } 
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::growregionsusingclumps)
    {
        cout << "A command to region grow from clump seeds using the clumps as base units.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Seeds File: " << this->seedsTextFile << endl;
        cout << "Initial Threshold: " << this->initRegionGrowthreshold << endl;
        cout << "Threshold Increments: " << this->thresholdRegionGrowincrements << endl;
        cout << "Maximum Threshold: " << this->maxRegionGrowthreshold << endl;
        cout << "Maximum Number of Iterations: " << this->maxRegionGrowiterations << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                string message = string("Could not open image ") + this->clumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                resultDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                clumpsDataset = inClumpDataset;
                resultDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            
            vector<ClumpSeed> *clumpSeeds = NULL;
            try 
            {
                RSGISFindClumpIDs findClumpIds;
                clumpSeeds = findClumpIds.readClumpSeedIDs(this->seedsTextFile);
                cout << "Performing Region Growing...\n";
                RSGISRegionGrowingSegmentation regionGrow;
                regionGrow.performRegionGrowUsingClumps(inDataset, clumpsDataset, resultDataset, clumpSeeds, this->initRegionGrowthreshold, this->thresholdRegionGrowincrements, this->maxRegionGrowthreshold, this->maxRegionGrowiterations);
            } 
            catch (RSGISTextException &e) 
            {
                throw RSGISException(e.what());
            }
            catch(RSGISImageCalcException &e)
            {
                throw RSGISImageCalcException(e.what());
            }
            catch (RSGISException &e) 
            {
                throw e;
            }
            
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::histogramseeds)
    {
        cout << "Create a mean spectral image from a clumped image.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        for(vector<BandThreshold>::iterator iterThreshold = bandThresholds->begin(); iterThreshold != bandThresholds->end(); ++iterThreshold)
        {
            cout << "Band " << (*iterThreshold).band << " threshold = " << (*iterThreshold).threshold << endl;
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                string message = string("Could not open image ") + this->clumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                resultDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                clumpsDataset = inClumpDataset;
                resultDataset = imgUtils.createCopy(inDataset, this->outputImage, this->imageFormat, GDT_Float32, this->projFromImage, this->proj);
            }
            
            cout << "Calculating Seeds\n";
            RSGISGenerateSeeds genSeeds;
            genSeeds.genSeedsHistogram(spectralDataset, clumpsDataset, resultDataset, bandThresholds);
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::histogramseedstxt)
    {
        cout << "Create a mean spectral image from a clumped image.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output File: " << this->seedsTextFile << endl;
        for(vector<BandThreshold>::iterator iterThreshold = bandThresholds->begin(); iterThreshold != bandThresholds->end(); ++iterThreshold)
        {
            cout << "Band " << (*iterThreshold).band << " threshold = " << (*iterThreshold).threshold << endl;
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                string message = string("Could not open image ") + this->clumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *clumpsDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
            }
            else
            {
                cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                clumpsDataset = inClumpDataset;
            }
            
            cout << "Calculating Seeds\n";
            RSGISGenerateSeeds genSeeds;
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::selectclumps)
    {
        cout << "Select clumps within larger parent clumps with highest/lowest value.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clumps Image: " << this->clumpsImage << endl;
        cout << "Large Clumps Image: " << this->largeClumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(this->selectionMethod == RSGISSelectClumps::max)
        {
            cout << "Selecting clumps with highest values.\n";
        }
        else if (this->selectionMethod == RSGISSelectClumps::min)
        {
            cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == RSGISSelectClumps::mean)
        {
            cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == RSGISSelectClumps::median)
        {
            cout << "Selecting clumps with lowest values.\n";
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *spectralDataset = NULL;
            spectralDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(spectralDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                string message = string("Could not open image ") + this->clumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *inLClumpDataset = NULL;
            inLClumpDataset = (GDALDataset *) GDALOpen(this->largeClumpsImage.c_str(), GA_ReadOnly);
            if(inLClumpDataset == NULL)
            {
                string message = string("Could not open image ") + this->largeClumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *lClumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";

                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                
                lClumpsDataset = imgUtils.createCopy(inLClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inLClumpDataset, lClumpsDataset);
                
                resultDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                clumpsDataset = inClumpDataset;
                lClumpsDataset = inLClumpDataset;
                resultDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            
            cout << "Select clumps.\n";
            RSGISSelectClumps selectClumpsObj;
            selectClumpsObj.selectClumps(spectralDataset, clumpsDataset, lClumpsDataset, resultDataset, this->selectionMethod);
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::selectclumpstxt)
    {
        cout << "Select clumps within larger parent clumps with highest/lowest value.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clumps Image: " << this->clumpsImage << endl;
        cout << "Large Clumps Image: " << this->largeClumpsImage << endl;
        cout << "Output Seeds File: " << this->seedsTextFile << endl;
        if(this->selectionMethod == RSGISSelectClumps::max)
        {
            cout << "Selecting clumps with highest values.\n";
        }
        else if (this->selectionMethod == RSGISSelectClumps::min)
        {
            cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == RSGISSelectClumps::mean)
        {
            cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == RSGISSelectClumps::median)
        {
            cout << "Selecting clumps with lowest values.\n";
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *spectralDataset = NULL;
            spectralDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(spectralDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                string message = string("Could not open image ") + this->clumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *inLClumpDataset = NULL;
            inLClumpDataset = (GDALDataset *) GDALOpen(this->largeClumpsImage.c_str(), GA_ReadOnly);
            if(inLClumpDataset == NULL)
            {
                string message = string("Could not open image ") + this->largeClumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *lClumpsDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                
                lClumpsDataset = imgUtils.createCopy(inLClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inLClumpDataset, lClumpsDataset);
            }
            else
            {
                cout << "Processing using Disk\n";
                clumpsDataset = inClumpDataset;
                lClumpsDataset = inLClumpDataset;
            }
            
            cout << "Select clumps.\n";
            RSGISSelectClumps selectClumpsObj;
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::mergesmallclumps)
    {
        cout << "Merge small clumps to one another or neighbouring larger clumps.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Small Clump Threshold: " << this->minClumpSize << endl;
        
        try 
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = NULL;
            inClumpDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                string message = string("Could not open image ") + this->clumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                resultDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                clumpsDataset = inClumpDataset;
                resultDataset = imgUtils.createCopy(inClumpDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            imgUtils.copyUIntGDALDataset(clumpsDataset, resultDataset);
            
            cout << "Eliminant Clumps\n";
            RSGISMergeSmallClumps mergeClumps;
            mergeClumps.mergeSmallClumps(spectralDataset, resultDataset, minClumpSize);
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::relabelclumps)
    {
        cout << "Relabel clumps to produce clumps with consecutive unique IDs.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try 
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *catagoryDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                catagoryDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(inDataset, catagoryDataset);
                resultDataset = imgUtils.createCopy(inDataset, 1, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                catagoryDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            
            cout << "Performing relabel\n";
            RSGISRelabelClumps relabelImg;
            relabelImg.relabelClumps(catagoryDataset, resultDataset);
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(catagoryDataset);
            }
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        } 
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::labelsfromclusters)
    {
        cout << "A command which labels image pixels with the id of the nearest cluster centre.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Cluster Matrix: " << this->clustersMatrix << endl;
        if(ignoreZeros)
        {
            cout << "Ignoring Zeros\n";
        }
        
        try 
        {
            GDALAllRegister();
            GDALDataset **datasets = new GDALDataset*[1];
            datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISLabelPixelsUsingClusters labelPixels;
            labelPixels.labelPixelsUsingClusters(datasets, 1, this->outputImage, this->clustersMatrix, this->ignoreZeros, this->imageFormat, this->projFromImage, this->proj);
            
            // Tidy up
            GDALClose(datasets[0]);
            GDALDestroyDriverManager();    
        } 
        catch (RSGISException &e) 
        {
            throw e;
        }   
    }
    else if(option == RSGISExeSegment::labelsfrompixels)
    {
        cout << "A command which produces an output image using the number of each pixel.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(ignoreZeros)
        {
            cout << "Ignoring Zeros\n";
        }
        
        
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
        RSGISLabelPixelsUsingPixelsCalcImg *labelPixelsUsingPixels = NULL;
        
        try
        {
            string *outBandName = new string[1];
            outBandName[0] = "clumps";
            
            labelPixelsUsingPixels = new RSGISLabelPixelsUsingPixelsCalcImg(1, this->ignoreZeros);
            calcImg = new RSGISCalcImage(labelPixelsUsingPixels, "", true);
            calcImg->calcImage(datasets, 1, this->outputImage, true, outBandName, this->imageFormat, GDT_UInt32);
            
            delete[] outBandName;
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
        delete labelPixelsUsingPixels;
        
    }
    else if(option == RSGISExeSegment::growregionspixels)
    {
        cout << "A command to grow regions from seeds up to a given distance in spectral space.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Threshold: " << this->specThreshold << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *resultDataset = NULL;
            GDALDataset *spectralDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                resultDataset = imgUtils.createCopy(inDataset, 1, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, 1, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            
            try 
            {
                cout << "Performing Region Growing...\n";
                RSGISRegionGrowSegmentsPixels regionGrow(spectralDataset, resultDataset);
                regionGrow.performRegionGrowing(regionGrowingPxlSeeds, specThreshold);
            } 
            catch (RSGISTextException &e) 
            {
                throw RSGISException(e.what());
            }
            catch(RSGISImageCalcException &e)
            {
                throw RSGISException(e.what());
            }
            catch(RSGISImageException &e)
            {
                throw RSGISException(e.what());
            }
            catch (RSGISException &e) 
            {
                throw e;
            }
            
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::growregionspixelsauto)
    {
        cout << "A command to region grow from seeds on a per pixel basis.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Initial Threshold: " << this->initRegionGrowthreshold << endl;
        cout << "Threshold Increments: " << this->thresholdRegionGrowincrements << endl;
        cout << "Maximum Threshold: " << this->maxRegionGrowthreshold << endl;
        cout << "Maximum Number of Iterations: " << this->maxRegionGrowiterations << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageUtils imgUtils;
            
            GDALDataset *resultDataset = NULL;
            GDALDataset *spectralDataset = NULL;
            
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, this->projFromImage, this->proj);
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                resultDataset = imgUtils.createCopy(inDataset, 1, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            
            try 
            {
                cout << "Performing Region Growing...\n";
                RSGISRegionGrowSegmentsPixels regionGrow(spectralDataset, resultDataset);
                regionGrow.performRegionGrowing(regionGrowingPxlSeeds, initRegionGrowthreshold, thresholdRegionGrowincrements, maxRegionGrowthreshold, maxRegionGrowiterations);
            } 
            catch (RSGISTextException &e) 
            {
                throw RSGISException(e.what());
            }
            catch(RSGISImageCalcException &e)
            {
                throw RSGISException(e.what());
            }
            catch(RSGISImageException &e)
            {
                throw RSGISException(e.what());
            }
            catch (RSGISException &e) 
            {
                throw e;
            }
            
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::spectraldiv)
    {
        cout << "Finding spectral division using a defined subdivision for bands.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Sub division: " << this->subDivision << endl;
        if(this->noDataValProvided)
        {
            cout << "No Data Value: " << this->noDataVal << endl;
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }

            RSGISDefineSpectralDivision defSpecDiv;
            defSpecDiv.findSpectralDivision(inDataset, this->outputImage, this->subDivision, this->noDataVal, this->noDataValProvided, this->projFromImage, this->proj, this->imageFormat);
            
            GDALClose(inDataset);
            GDALDestroyDriverManager();
        } 
        catch (RSGISException &e) 
        {
            throw e;
        }
        
    }
    else if(option == RSGISExeSegment::stepwiseelimination)
    {
        cout << "Eliminate clumps smaller than a given size from the scene using stepwise approach using an attribute table.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(this->stretchStatsAvail)
        {
            cout << "Statistics File: " << this->stretchStatsFile << endl;
        }
        if(!this->processInMemory)
        {
            cout << "Temp Table: " << this->tempTable << endl;
            cout << "Cache Size: " << this->cacheSize << endl;
        }
        if(this->outputWithConsecutiveFIDs)
        {
            cout << "Outputting with consecutive FIDs\n";
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *spectralDataset = NULL;
            spectralDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(spectralDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *clumpsDataset = NULL;
            clumpsDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(clumpsDataset == NULL)
            {
                string message = string("Could not open image ") + this->clumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            std::vector<BandSpecThresholdStats> *bandStretchStats = NULL;
            if(this->stretchStatsAvail)
            {
                bandStretchStats = rsgis::img::RSGISStretchImage::readBandSpecThresholds(stretchStatsFile);
            }
                        
            cout << "Create Attribute Table\n";
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
            
            cout << "Eliminating Clumps\n";
            RSGISEliminateSmallClumps eliminate;
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
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else if(option == RSGISExeSegment::elimsinglepxls)
    {
        cout << "A command which iteratively removes single pixel labelled neighbours into spectrally closest large group.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(!this->processInMemory)
        {
            cout << "Temp Image: " << this->tempImage << endl;
        }
        if(this->ignoreZeros)
        {
            cout << "Ignoring pixels with a value of zero in clumps file\n";
        }
        
        try
        {
            RSGISImageUtils imgUtils;
            
            GDALAllRegister();
            GDALDataset *spectralDataset = NULL;
            spectralDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(spectralDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *clumpsDataset = NULL;
            clumpsDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
            if(clumpsDataset == NULL)
            {
                string message = string("Could not open image ") + this->clumpsImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *pixelMaskDataset = NULL;
            if(this->processInMemory)
            {
                pixelMaskDataset = imgUtils.createCopy(clumpsDataset, 1, "", "MEM", GDT_Byte , this->projFromImage, this->proj);
            }
            else 
            {
                pixelMaskDataset = imgUtils.createCopy(clumpsDataset, 1, this->tempImage, this->imageFormat, GDT_Byte, this->projFromImage, this->proj);
            }

            
            cout << "Eliminating Individual Pixels\n";
            RSGISEliminateSinglePixels eliminate;
            eliminate.eliminate(spectralDataset, clumpsDataset, pixelMaskDataset, this->outputImage, 0, this->ignoreZeros, this->projFromImage, this->proj, this->imageFormat);
            
            clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            // Tidy up
            GDALClose(spectralDataset);
            GDALClose(clumpsDataset);
            GDALClose(pixelMaskDataset);
            GDALDestroyDriverManager();
        } 
        catch (RSGISException &e) 
        {
            throw e;
        }
    }
    else
    {
        cout << "RSGISExeSegment: Options not recognised\n";
    }
}


void RSGISExeSegment::printParameters()
{
	if(!parsed)
	{
		cout << "Parameters have not been parsed.\n";
	}
    else if(option == RSGISExeSegment::specgrp)
    {
        cout << "Locally spectrally cluster/group the pixels into clumps.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Spectral Threshold: " << this->specThreshold << endl;
        if(processInMemory)
        {
            cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::specgrpweighted)
    {
        cout << "Locally spectrally cluster/group the pixels into clumps using stddev to weight distance metric.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Spectral Threshold: " << this->specThreshold << endl;
        if(processInMemory)
        {
            cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::meanimg)
    {
        cout << "Create a mean spectral image from a clumped image.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(processInMemory)
        {
            cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::rmsmallclumps)
    {
        cout << "Eliminate clumps smaller than a given size from the scene.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Spectral Threshold: " << this->specThreshold << endl;
        cout << "Min Clump Size: " << this->minClumpSize << endl;
        if(processInMemory)
        {
            cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::rmsmallclumpsstepwise)
    {
        cout << "Eliminate clumps smaller than a given size from the scene using stepwise approach.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Spectral Threshold: " << this->specThreshold << endl;
        cout << "Min Clump Size: " << this->minClumpSize << endl;
        if(processInMemory)
        {
            cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            cout << "Processing to undertaken from disk\n";
        }
        if(this->storeMean)
        {
            cout << "Mean values are stored in memory.\n";
        }
        else
        {
            cout << "Mean values are calculated at runtime.\n";
        }
    }
    else if(option == RSGISExeSegment::clump)
    {
        cout << "Clump a categorical image into clumps with unique IDs.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(processInMemory)
        {
            cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::randomcolourclumps)
    {
        cout << "Colour clumps randomly for visualisation\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
    }
    else if(option == RSGISExeSegment::regiongrowingseedclumpids)
    {
        cout << "Find clump ids for pixels and output region growing seeds.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output file: " << this->outputTextFile << endl;
    }
    else if(option == RSGISExeSegment::growregionsusingclumps)
    {
        cout << "A command to region grow from clump seeds using the clumps as base units.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Seeds File: " << this->seedsTextFile << endl;
        cout << "Initial Threshold: " << this->initRegionGrowthreshold << endl;
        cout << "Threshold Increments: " << this->thresholdRegionGrowincrements << endl;
        cout << "Maximum Threshold: " << this->maxRegionGrowthreshold << endl;
        cout << "Maximum Number of Iterations: " << this->maxRegionGrowiterations << endl;
    }
    else if(option == RSGISExeSegment::histogramseeds)
    {
        cout << "Create a mean spectral image from a clumped image.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        for(vector<BandThreshold>::iterator iterThreshold = bandThresholds->begin(); iterThreshold != bandThresholds->end(); ++iterThreshold)
        {
            cout << "Band " << (*iterThreshold).band << " threshold = " << (*iterThreshold).threshold << endl;
        }
    }
    else if(option == RSGISExeSegment::histogramseedstxt)
    {
        cout << "Create a mean spectral image from a clumped image.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output File: " << this->seedsTextFile << endl;
        for(vector<BandThreshold>::iterator iterThreshold = bandThresholds->begin(); iterThreshold != bandThresholds->end(); ++iterThreshold)
        {
            cout << "Band " << (*iterThreshold).band << " threshold = " << (*iterThreshold).threshold << endl;
        }
    }
    else if(option == RSGISExeSegment::selectclumps)
    {
        cout << "Select clumps within larger parent clumps with highest/lowest value.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clumps Image: " << this->clumpsImage << endl;
        cout << "Large Clumps Image: " << this->largeClumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(this->selectionMethod == RSGISSelectClumps::max)
        {
            cout << "Selecting clumps with highest values.\n";
        }
        else if (this->selectionMethod == RSGISSelectClumps::min)
        {
            cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == RSGISSelectClumps::mean)
        {
            cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == RSGISSelectClumps::median)
        {
            cout << "Selecting clumps with lowest values.\n";
        }
    }
    else if(option == RSGISExeSegment::selectclumpstxt)
    {
        cout << "Select clumps within larger parent clumps with highest/lowest value.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clumps Image: " << this->clumpsImage << endl;
        cout << "Large Clumps Image: " << this->largeClumpsImage << endl;
        cout << "Output Seeds File: " << this->seedsTextFile << endl;
        if(this->selectionMethod == RSGISSelectClumps::max)
        {
            cout << "Selecting clumps with highest values.\n";
        }
        else if (this->selectionMethod == RSGISSelectClumps::min)
        {
            cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == RSGISSelectClumps::mean)
        {
            cout << "Selecting clumps with lowest values.\n";
        }
        else if (this->selectionMethod == RSGISSelectClumps::median)
        {
            cout << "Selecting clumps with lowest values.\n";
        }
    }
    else if(option == RSGISExeSegment::mergesmallclumps)
    {
        cout << "Merge small clumps to one another or neighbouring larger clumps.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(processInMemory)
        {
            cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::labelsfromclusters)
    {
        cout << "A command which labels image pixels with the id of the nearest cluster centre.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Cluster Matrix: " << this->clustersMatrix << endl;
        if(ignoreZeros)
        {
            cout << "Ignoring Zeros\n";
        }
    }
    else if(option == RSGISExeSegment::growregionspixels)
    {
        cout << "A command to grow regions from seeds up to a given distance in spectral space.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Threshold: " << this->specThreshold << endl;
        if(processInMemory)
        {
            cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::growregionspixelsauto)
    {
        cout << "A command to region grow from seeds on a per pixel basis.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Initial Threshold: " << this->initRegionGrowthreshold << endl;
        cout << "Threshold Increments: " << this->thresholdRegionGrowincrements << endl;
        cout << "Maximum Threshold: " << this->maxRegionGrowthreshold << endl;
        cout << "Maximum Number of Iterations: " << this->maxRegionGrowiterations << endl;
        if(processInMemory)
        {
            cout << "Processing to be undertaken in Memory\n";
        }
        else
        {
            cout << "Processing to undertaken from disk\n";
        }
    }
    else if(option == RSGISExeSegment::spectraldiv)
    {
        cout << "Finding spectral division using a defined subdivision for bands.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        cout << "Sub division: " << this->subDivision << endl;
        cout << "Number of Levels: " << this->levels << endl;
        if(this->noDataValProvided)
        {
            cout << "No Data Value: " << this->noDataVal << endl;
        }
    }
    else if(option == RSGISExeSegment::stepwiseelimination)
    {
        cout << "Eliminate clumps smaller than a given size from the scene using stepwise approach using an attribute table.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(!this->processInMemory)
        {
            cout << "Temp Table: " << this->tempTable << endl;
            cout << "Cache Size: " << this->cacheSize << endl;
        }
        if(this->outputWithConsecutiveFIDs)
        {
            cout << "Outputting with consecutive FIDs\n";
        }
    }
    else if(option == RSGISExeSegment::elimsinglepxls)
    {
        cout << "A command which iteratively removes single pixel labelled neighbours into spectrally closest large group.\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Clump Image: " << this->clumpsImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(!this->processInMemory)
        {
            cout << "Temp Image: " << this->tempImage << endl;
        }
        if(this->ignoreZeros)
        {
            cout << "Ignoring pixels with a value of zero in clumps file\n";
        }
    }
    else
    {
        cout << "RSGISExeSegment: Options not recognised\n";
    }
}

void RSGISExeSegment::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
    cout << "<!-- A command to locally spectrally cluster/group the pixels into clumps -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"specgrp\" image=\"image.env\" output=\"image_out.env\" specthreshold=\"float\" nodata=\"unsigned int\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to locally spectrally cluster/group the pixels into clumps where the distance metric is weighted by the stddev of the band -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"specgrpweighted\" image=\"image.env\" output=\"image_out.env\" specthreshold=\"float\" nodata=\"unsigned int\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to generate a mean image from a spectral image and a clumped image -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"meanimg\" image=\"image.env\" clumps=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to eliminate clumps smaller than a given size from the scene, small clump will be combined with their spectrally closest neighbouring clump unless over spectral distance threshold -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"rmsmallclumps\" image=\"image.env\" clumps=\"image.env\" output=\"image_out.env\" minsize=\"int (in pxls)\" maxspectraldist=\"float\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to eliminate clumps smaller than a given size from the scene, small clump will be combined with their spectrally closest neighbouring clump in a stepwise fashion unless over spectral distance threshold -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"rmsmallclumpsstepwise\" image=\"image.env\" clumps=\"image.env\" output=\"image_out.env\" minsize=\"int (in pxls)\" maxspectraldist=\"float\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to clump a categorical image into clumps with unique IDs -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"clump\" image=\"image.env\" output=\"image_out.env\" nodata=\"unsigned int\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to colour clumps using random colours for visualisation -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"randomcolourclumps\" image=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- Generate an from a list of pixel locations generate a list of clump ids which will be the seeds for a region growing algorithm -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"regiongrowingseedclumpids\" image=\"clumpsimage.env\" output=\"clumpids.txt\" >" << endl;
    cout << "    <rsgis:seed x=\"int\" y=\"int\" id=\"int\" />" << endl;
    cout << "    <rsgis:seed x=\"int\" y=\"int\" id=\"int\" />" << endl;
    cout << "    <rsgis:seed x=\"int\" y=\"int\" id=\"int\" />" << endl;
    cout << "    <rsgis:seed x=\"int\" y=\"int\" id=\"int\" />" << endl;
    cout << "    <rsgis:seed x=\"int\" y=\"int\" id=\"int\" />" << endl;
    cout << "    <rsgis:seed x=\"int\" y=\"int\" id=\"int\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to grow regions from seeds to there maximum extent using the clumps. -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"growregionsusingclumps\" image=\"image.env\" clumps=\"image.env\" output=\"image_out.env\" seeds=\"seedsfile.txt\" initthreshold=\"float\" thresholdincrements=\"float\" maxthreshold=\"float\" maxiterations=\"int\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to identify seeds for region growing -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"histogramseeds\" image=\"image.env\" clumps=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" >" << endl;
    cout << "    <rsgis:threshold band=\"int\" threshold=\"float\" />" << endl;
    cout << "    <rsgis:threshold band=\"int\" threshold=\"float\" />" << endl;
    cout << "    <rsgis:threshold band=\"int\" threshold=\"float\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to identify seeds for region growing - output is a text file for input into growregionsusingclumps -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"histogramseedstxt\" image=\"image.env\" clumps=\"image.env\" output=\"seedsfile.txt\" inmemory=\"yes | no\"  >" << endl;
    cout << "    <rsgis:threshold band=\"int\" threshold=\"float\" />" << endl;
    cout << "    <rsgis:threshold band=\"int\" threshold=\"float\" />" << endl;
    cout << "    <rsgis:threshold band=\"int\" threshold=\"float\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command which selects the clump within the clumps file which intersect with either the minimum or maximum mean clump -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"selectclumps\" image=\"image.env\" clumps=\"image.env\" largeclumps=\"image.env\" output=\"image_out.env\" type=\"max | min | mean | median | percentile75th | percentile95th\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command which selects the clump within the clumps file which intersect with either the minimum or maximum mean clump with output to a text file.-->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"selectclumpstxt\" image=\"image.env\" clumps=\"image.env\" largeclumps=\"image.env\" output=\"seedsfile.txt\" type=\"max | min | mean | median | percentile75th | percentile95th\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to remove small clumps from a segmentation -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"mergesmallclumps\" image=\"image.env\" clumps=\"image.env\" output=\"image_out.env\" minsize=\"int (in pxls)\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to relabel clumps to ensure they are ordered consecutively -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"relabelclumps\" image=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command which labels image pixels with the id of the nearest cluster centre -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"labelsfromclusters\" image=\"image.env\" output=\"image_out.env\" clusters=\"matrix.gmtxt\" ignorezeros=\"yes | no\" format=\"GDAL Format\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to grow regions from seeds up to a given distance in spectral space. -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"growregionspixels\" image=\"image.env\" output=\"image_out.env\" seeds=\"seedsfile.txt\" threshold=\"float\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to grow regions from seeds to there maximum extent on a per pixel bases. -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"growregionspixelsauto\" image=\"image.env\" output=\"image_out.env\" seeds=\"seedsfile.txt\" initthreshold=\"float\" thresholdincrements=\"float\" maxthreshold=\"float\" maxiterations=\"int\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to find spectral categories using a defined subdivision and number of levels -->" << endl;
    cout << "<rsgis:command algor=\"segmentation\" option=\"spectraldiv\" image=\"image.env\" output=\"image_out.env\" subdivision=\"int\" nodata=\"unsigned int\" format=\"GDAL Format\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "</rsgis:commands>" << endl;
}

string RSGISExeSegment::getDescription()
{
	return "Provides an output image of the overlapping area containing all the input bands.";
}

string RSGISExeSegment::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeSegment::~RSGISExeSegment()
{
	
}

}

