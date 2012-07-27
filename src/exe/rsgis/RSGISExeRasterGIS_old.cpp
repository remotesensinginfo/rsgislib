/*
 *  RSGISExeRasterGIS.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 15/02/2012.
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

#include "RSGISExeRasterGIS.h"

namespace rsgisexe{

RSGISExeRasterGIS::RSGISExeRasterGIS() : RSGISAlgorithmParameters()
{
	this->algorithm = "rastergis_old";
	
	this->option = RSGISExeRasterGIS::none;
}

RSGISAlgorithmParameters* RSGISExeRasterGIS::getInstance()
{
	return new RSGISExeRasterGIS();
}

void RSGISExeRasterGIS::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{	
	RSGISMathsUtils mathUtils;
    RSGISTextUtils textUtils;
	
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
    XMLCh *optionCreateAttributeTable = XMLString::transcode("createattributetable");
	XMLCh *optionPopAttributeMean = XMLString::transcode("popattributemean");
    XMLCh *optionExportTable2Img = XMLString::transcode("exporttable2img");
    XMLCh *optionBoolClassTable = XMLString::transcode("boolclasstable");
    XMLCh *optionAddFields = XMLString::transcode("addfields");
    XMLCh *optionRegionGrowThres = XMLString::transcode("regiongrowthres");
    XMLCh *optionFindMajority = XMLString::transcode("findmajority");
    XMLCh *optionCountPolyIntersects = XMLString::transcode("countpolyintersects");
    XMLCh *optionCalcAttributes = XMLString::transcode("calcattributes");
    XMLCh *optionPopAttributeStatsInMem = XMLString::transcode("popattributestatsinmem");
    XMLCh *optionPopAttributeStats = XMLString::transcode("popattributestats");
    XMLCh *optionMeanLitBandsPopAttributeStats = XMLString::transcode("meanlitbandspopattributestats");
    XMLCh *optionMeanLitPopAttributeStatsInMem = XMLString::transcode("meanlitpopattributestatsinmem");
    XMLCh *optionMeanLitPopAttributeStats = XMLString::transcode("meanlitpopattributestats");
    XMLCh *optionExport2GDAL = XMLString::transcode("export2gdal");
    XMLCh *optionPopAttributeStatsAllBands = XMLString::transcode("popattributestatsallbands");
    XMLCh *optionSummariseField = XMLString::transcode("summarisefield");
    XMLCh *optionFindNeighbours = XMLString::transcode("findneighbours");
    XMLCh *optionMeanEucDist2Neighbours = XMLString::transcode("meaneucdist2neighbours");
    XMLCh *optionCalcIntraPxlEucDist = XMLString::transcode("calcintrapxleucdist");
    XMLCh *optionExportField2ASCII = XMLString::transcode("exportfield2ascii");
    XMLCh *optionExport2HDF = XMLString::transcode("export2hdf");
    XMLCh *optionExport2ASCII = XMLString::transcode("export2ascii");
    XMLCh *optionPopMeanSumAttributes = XMLString::transcode("popmeansumattributes");
    XMLCh *optionPrintAttSummary = XMLString::transcode("printattsummary");
    XMLCh *optionExportSize = XMLString::transcode("exportsize");
    XMLCh *optionPopAttributeStatsThresholded = XMLString::transcode("popattributestatsthresholded");
    XMLCh *optionKNNExtrapolate = XMLString::transcode("knnextrapolate");
    XMLCh *optionPopBoolField = XMLString::transcode("popboolfield");
    XMLCh *optionCopyGDALATT = XMLString::transcode("copyGDALATT");
            
    XMLCh *optionRSGISBool = XMLString::transcode("rsgis_bool");
    XMLCh *optionRSGISInt = XMLString::transcode("rsgis_int");
    XMLCh *optionRSGISFloat = XMLString::transcode("rsgis_float");
    XMLCh *optionRSGISString = XMLString::transcode("rsgis_string");
	
	const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
    
    XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
    if(argElement->hasAttribute(inMemoryXMLStr))
    {
        XMLCh *yesStr = XMLString::transcode("yes");
        const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
        
        if(XMLString::equals(inMemValue, yesStr))
        {
            this->attInMemory = true;
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&yesStr);
    }
    else
    {
        this->attInMemory = false;
    }
    XMLString::release(&inMemoryXMLStr);
	
	const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
	if(XMLString::equals(optionCreateAttributeTable, optionXML))
    {
        this->option = RSGISExeRasterGIS::createattributetable;
        
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
            this->outputFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
        }
        XMLString::release(&outputXMLStr);
    }
    else if(XMLString::equals(optionPopAttributeMean, optionXML))
	{		
		this->option = RSGISExeRasterGIS::popattributemean;
		
		XMLCh *inputXMLStr = XMLString::transcode("input");
        if(argElement->hasAttribute(inputXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
            this->inputImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
        }
        XMLString::release(&inputXMLStr);
        
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
            this->outputFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
        }
        XMLString::release(&outputXMLStr);
        
        XMLCh *prefixXMLStr = XMLString::transcode("prefix");
        if(argElement->hasAttribute(prefixXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(prefixXMLStr));
            this->attprefix = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'prefix\' attribute was provided.");
        }
        XMLString::release(&prefixXMLStr);
	}
    else if(XMLString::equals(optionExportTable2Img, optionXML))
	{		
		this->option = RSGISExeRasterGIS::exporttable2img;
        
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
        
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
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
        
        
        XMLCh *rsgisBandXMLStr = XMLString::transcode("rsgis:band");
        DOMNodeList *bandsNodesList = argElement->getElementsByTagName(rsgisBandXMLStr);
		unsigned int numBands = bandsNodesList->getLength();
		
		cout << "Found " << numBands << " Bands" << endl;
		
        bands = new vector<pair<unsigned int, string> >();
        bands->reserve(numBands);
        
        string attributeName = "";
        
		DOMElement *bandElement = NULL;
		for(int i = 0; i < numBands; i++)
		{
			bandElement = static_cast<DOMElement*>(bandsNodesList->item(i));
			
			XMLCh *attributeXMLStr = XMLString::transcode("attribute");
			if(bandElement->hasAttribute(attributeXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(attributeXMLStr));
				attributeName = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'attribute\' attribute was provided.");
			}
			XMLString::release(&attributeXMLStr);
            
            bands->push_back(pair<unsigned int, string>(i+1, attributeName));
		}
        XMLString::release(&rsgisBandXMLStr);
        
	}
    else if(XMLString::equals(optionBoolClassTable, optionXML))
	{		
		this->option = RSGISExeRasterGIS::boolclasstable;
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        try 
        {
             this->statements = RSGISAttributeTable::generateStatments(argElement);
        } 
        catch (RSGISException &e) 
        {
            throw RSGISXMLArgumentsException(e.what());
        }
	}
    else if(XMLString::equals(optionAddFields, optionXML))
    {
        this->option = RSGISExeRasterGIS::addfields;
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);

        XMLCh *rsgisAttributeXMLStr = XMLString::transcode("rsgis:attribute");
        DOMNodeList *attributeNodesList = argElement->getElementsByTagName(rsgisAttributeXMLStr);
		unsigned int numAttributes = attributeNodesList->getLength();
		
		cout << "Found " << numAttributes << " Attributes" << endl;
		
        attributes = new vector<RSGISAttribute*>();
        attributes->reserve(numAttributes);
        
        RSGISAttribute *attribute = NULL;
		DOMElement *attributeElement = NULL;
		for(int i = 0; i < numAttributes; i++)
		{
			attributeElement = static_cast<DOMElement*>(attributeNodesList->item(i));
			
            attribute = new RSGISAttribute();
            
			XMLCh *nameXMLStr = XMLString::transcode("name");
			if(attributeElement->hasAttribute(nameXMLStr))
			{
				char *charValue = XMLString::transcode(attributeElement->getAttribute(nameXMLStr));
				attribute->name = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			XMLString::release(&nameXMLStr);
            
            XMLCh *dataTypeXMLStr = XMLString::transcode("datatype");
			if(attributeElement->hasAttribute(dataTypeXMLStr))
			{
				const XMLCh *dataTypeValXMLStr = attributeElement->getAttribute(dataTypeXMLStr);
				if(XMLString::equals(dataTypeValXMLStr, optionRSGISBool))
                {
                    attribute->dataType = rsgis_bool;
                }
                else if(XMLString::equals(dataTypeValXMLStr, optionRSGISInt))
                {
                    attribute->dataType = rsgis_int;
                }
                else if(XMLString::equals(dataTypeValXMLStr, optionRSGISFloat))
                {
                    attribute->dataType = rsgis_float;
                }
                else if(XMLString::equals(dataTypeValXMLStr, optionRSGISString))
                {
                    attribute->dataType = rsgis_string;
                }
                else
                {
                    throw RSGISXMLArgumentsException("datatype was not recognised.");
                }
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'datatype\' attribute was provided.");
			}
			XMLString::release(&dataTypeXMLStr);
            
            attributes->push_back(attribute);
		}
        XMLString::release(&rsgisAttributeXMLStr);
        
    }
	else if(XMLString::equals(optionRegionGrowThres, optionXML))
    {
        this->option = RSGISExeRasterGIS::regiongrowthres;
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        XMLCh *classXMLStr = XMLString::transcode("class");
        if(argElement->hasAttribute(classXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(classXMLStr));
            this->classAttributeName = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'class\' attribute was provided.");
        }
        XMLString::release(&classXMLStr);
        
        XMLCh *classValXMLStr = XMLString::transcode("classval");
        if(argElement->hasAttribute(classValXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(classValXMLStr));
            this->classAttributeVal = mathUtils.strtoint(string(charValue));
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'classval\' attribute was provided.");
        }
        XMLString::release(&classValXMLStr);
        
        try 
        {
            this->statements = RSGISAttributeTable::generateStatments(argElement);
        } 
        catch (RSGISException &e) 
        {
            throw RSGISXMLArgumentsException(e.what());
        }
    }
    else if(XMLString::equals(optionFindMajority, optionXML))
    {
        this->option = RSGISExeRasterGIS::findmajority;
        
        XMLCh *catagoriesXMLStr = XMLString::transcode("catagories");
        if(argElement->hasAttribute(catagoriesXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(catagoriesXMLStr));
            this->catagoriesImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'catagories\' attribute was provided.");
        }
        XMLString::release(&catagoriesXMLStr);
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        
        XMLCh *areaFieldXMLStr = XMLString::transcode("pxlareafield");
        if(argElement->hasAttribute(areaFieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(areaFieldXMLStr));
            this->areaField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'pxlareafield\' attribute was provided.");
        }
        XMLString::release(&areaFieldXMLStr);
        
        
        XMLCh *majorityCatFieldXMLStr = XMLString::transcode("majoritycatfield");
        if(argElement->hasAttribute(majorityCatFieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(majorityCatFieldXMLStr));
            this->majorityCatField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'majoritycatfield\' attribute was provided.");
        }
        XMLString::release(&majorityCatFieldXMLStr);
        
        XMLCh *majorityRatioFieldXMLStr = XMLString::transcode("majorityratiofield");
        if(argElement->hasAttribute(majorityRatioFieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(majorityRatioFieldXMLStr));
            this->majorityRatioField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'majorityratiofield\' attribute was provided.");
        }
        XMLString::release(&majorityRatioFieldXMLStr);
        
    }
    else if(XMLString::equals(optionCountPolyIntersects, optionXML))
    {
        this->option = RSGISExeRasterGIS::countpolyintersects;
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        
        XMLCh *fieldXMLStr = XMLString::transcode("field");
        if(argElement->hasAttribute(fieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(fieldXMLStr));
            this->attField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'field\' attribute was provided.");
        }
        XMLString::release(&fieldXMLStr);
        
        XMLCh *methodXMLStr = XMLString::transcode("method");
		if(argElement->hasAttribute(methodXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(methodXMLStr));
			string methodStr = string(charValue);
			// Polygon completely contains pixel
			if(methodStr == "polyContainsPixel"){this->pixelPolyOption = polyContainsPixel;}
			// Pixel center is within the polygon
			else if(methodStr == "polyContainsPixelCenter") {this->pixelPolyOption = polyContainsPixelCenter;}
			// Polygon overlaps the pixel
			else if(methodStr == "polyOverlapsPixel"){this->pixelPolyOption = polyOverlapsPixel;}
			// Polygon overlaps or contains the pixel
			else if(methodStr == "polyOverlapsOrContainsPixel"){this->pixelPolyOption = polyOverlapsOrContainsPixel;}
			// Pixel contains the polygon
			else if(methodStr == "pixelContainsPoly"){this->pixelPolyOption = pixelContainsPoly;}
			// Polygon center is within pixel
			else if(methodStr == "pixelContainsPolyCenter"){this->pixelPolyOption = pixelContainsPolyCenter;}
			// The method is chosen based on relative areas of pixel and polygon.
			else if(methodStr == "adaptive"){this->pixelPolyOption = adaptive;}
			// Everything within the polygons envelope is chosen (for debugging)
			else if(methodStr == "envelope"){this->pixelPolyOption = envelope;}
			// Set to default value if not recognised.
			else {cout << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;this->pixelPolyOption = polyContainsPixelCenter;}
			XMLString::release(&charValue);
		}
		else
		{
			this->pixelPolyOption = polyContainsPixelCenter;
		}
		XMLString::release(&methodXMLStr);
        
    }
    else if(XMLString::equals(optionCalcAttributes, optionXML))
    {
        this->option = RSGISExeRasterGIS::calcattributes;
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        
        XMLCh *fieldXMLStr = XMLString::transcode("field");
        if(argElement->hasAttribute(fieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(fieldXMLStr));
            this->attField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'field\' attribute was provided.");
        }
        XMLString::release(&fieldXMLStr);
        
        XMLCh *dataTypeXMLStr = XMLString::transcode("datatype");
        if(argElement->hasAttribute(dataTypeXMLStr))
        {
            const XMLCh *dataTypeValXMLStr = argElement->getAttribute(dataTypeXMLStr);
            if(XMLString::equals(dataTypeValXMLStr, optionRSGISInt))
            {
                this->attFieldDT = rsgis_int;
            }
            else if(XMLString::equals(dataTypeValXMLStr, optionRSGISFloat))
            {
                this->attFieldDT = rsgis_float;
            }
            else
            {
                throw RSGISXMLArgumentsException("datatype was not recognised.");
            }
        }
        else
        {
            cerr << "Warning: defaulting to using a floating point field.\n"; 
            this->attFieldDT = rsgis_float;
        }
        XMLString::release(&dataTypeXMLStr);
        
        XMLCh *expressionXMLStr = XMLString::transcode("expression");
        if(argElement->hasAttribute(expressionXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(expressionXMLStr));
            this->mathsExpression = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            XMLCh *expressionfileXMLStr = XMLString::transcode("expressionfile");
            if(argElement->hasAttribute(expressionfileXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(expressionfileXMLStr));
                try
                {
                    this->mathsExpression = textUtils.readFileToString(string(charValue));
                }
                catch(RSGISException &e)
                {
                    throw RSGISXMLArgumentsException(e.what());
                }
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'expression\' or \'expressionfile\' attribute was provided.");
            }
            XMLString::release(&expressionfileXMLStr);
        }
        XMLString::release(&expressionXMLStr);
        
        
        XMLCh *rsgisAttributeXMLStr = XMLString::transcode("rsgis:attribute");
        DOMNodeList *attributeNodesList = argElement->getElementsByTagName(rsgisAttributeXMLStr);
		unsigned int numAttributes = attributeNodesList->getLength();
		
		cout << "Found " << numAttributes << " Attributes" << endl;
		
        variables = new vector<RSGISMathAttVariable*>();
        variables->reserve(numAttributes);
        
        RSGISMathAttVariable *variable = NULL;
		DOMElement *attributeElement = NULL;
		for(int i = 0; i < numAttributes; i++)
		{
			attributeElement = static_cast<DOMElement*>(attributeNodesList->item(i));
			
            variable = new RSGISMathAttVariable();
            
            XMLCh *variableXMLStr = XMLString::transcode("variable");
			if(attributeElement->hasAttribute(variableXMLStr))
			{
				char *charValue = XMLString::transcode(attributeElement->getAttribute(variableXMLStr));
				variable->variable = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'variable\' attribute was provided.");
			}
			XMLString::release(&variableXMLStr);
            
			XMLCh *nameXMLStr = XMLString::transcode("name");
			if(attributeElement->hasAttribute(nameXMLStr))
			{
				char *charValue = XMLString::transcode(attributeElement->getAttribute(nameXMLStr));
				variable->field = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			XMLString::release(&nameXMLStr);
            
            variables->push_back(variable);
		}
        XMLString::release(&rsgisAttributeXMLStr);
        
    }
    else if(XMLString::equals(optionPopAttributeStatsInMem, optionXML))
    {
        this->option = RSGISExeRasterGIS::popattributestatsinmem;
        XMLCh *inputXMLStr = XMLString::transcode("input");
        if(argElement->hasAttribute(inputXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
            this->inputImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
        }
        XMLString::release(&inputXMLStr);
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        
        
        XMLCh *rsgisBandXMLStr = XMLString::transcode("rsgis:band");
        DOMNodeList *bandNodesList = argElement->getElementsByTagName(rsgisBandXMLStr);
		unsigned int numBands = bandNodesList->getLength();
		
		cout << "Found " << numBands << " Attributes" << endl;
		
        bandStats = new vector<RSGISBandAttStats*>();
        bandStats->reserve(numBands);
        
        RSGISBandAttStats *bandStat = NULL;
		DOMElement *bandElement = NULL;
		for(int i = 0; i < numBands; i++)
		{
			bandElement = static_cast<DOMElement*>(bandNodesList->item(i));
			
            bandStat = new RSGISBandAttStats();
            
			XMLCh *bandXMLStr = XMLString::transcode("band");
			if(bandElement->hasAttribute(bandXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
				bandStat->band = mathUtils.strtounsignedint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			XMLString::release(&bandXMLStr);
            
            XMLCh *minXMLStr = XMLString::transcode("min");
			if(bandElement->hasAttribute(minXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(minXMLStr));
				bandStat->minField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMin = true;
			}
			else
			{
				bandStat->calcMin = false;
			}
			XMLString::release(&minXMLStr);
            
            XMLCh *maxXMLStr = XMLString::transcode("max");
			if(bandElement->hasAttribute(maxXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(maxXMLStr));
				bandStat->maxField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMax = true;
			}
			else
			{
				bandStat->calcMax = false;
			}
			XMLString::release(&maxXMLStr);
            
            XMLCh *meanXMLStr = XMLString::transcode("mean");
			if(bandElement->hasAttribute(meanXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(meanXMLStr));
				bandStat->meanField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMean = true;
			}
			else
			{
				bandStat->calcMean = false;
			}
			XMLString::release(&meanXMLStr);
            
            XMLCh *stdDevXMLStr = XMLString::transcode("stddev");
			if(bandElement->hasAttribute(stdDevXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(stdDevXMLStr));
				bandStat->stdDevField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcStdDev = true;
			}
			else
			{
				bandStat->calcStdDev = false;
			}
			XMLString::release(&stdDevXMLStr);
            
            
            XMLCh *medianXMLStr = XMLString::transcode("median");
			if(bandElement->hasAttribute(medianXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(medianXMLStr));
				bandStat->medianField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMedian = true;
			}
			else
			{
				bandStat->calcMedian = false;
			}
			XMLString::release(&medianXMLStr);
            
            XMLCh *sumXMLStr = XMLString::transcode("sum");
			if(bandElement->hasAttribute(sumXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(sumXMLStr));
				bandStat->sumField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcSum = true;
			}
			else
			{
				bandStat->calcSum = false;
			}
			XMLString::release(&sumXMLStr);
            
            bandStats->push_back(bandStat);
		}
        XMLString::release(&rsgisBandXMLStr);
        
    }
    else if(XMLString::equals(optionPopAttributeStats, optionXML))
    {
        this->option = RSGISExeRasterGIS::popattributestats;
        XMLCh *inputXMLStr = XMLString::transcode("input");
        if(argElement->hasAttribute(inputXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
            this->inputImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
        }
        XMLString::release(&inputXMLStr);
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        
        
        XMLCh *rsgisBandXMLStr = XMLString::transcode("rsgis:band");
        DOMNodeList *bandNodesList = argElement->getElementsByTagName(rsgisBandXMLStr);
		unsigned int numBands = bandNodesList->getLength();
		
		cout << "Found " << numBands << " Attributes" << endl;
		
        bandStats = new vector<RSGISBandAttStats*>();
        bandStats->reserve(numBands);
        
        RSGISBandAttStats *bandStat = NULL;
		DOMElement *bandElement = NULL;
		for(int i = 0; i < numBands; i++)
		{
			bandElement = static_cast<DOMElement*>(bandNodesList->item(i));
			
            bandStat = new RSGISBandAttStats();
            
			XMLCh *bandXMLStr = XMLString::transcode("band");
			if(bandElement->hasAttribute(bandXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
				bandStat->band = mathUtils.strtounsignedint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			XMLString::release(&bandXMLStr);
            
            XMLCh *minXMLStr = XMLString::transcode("min");
			if(bandElement->hasAttribute(minXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(minXMLStr));
				bandStat->minField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMin = true;
			}
			else
			{
				bandStat->calcMin = false;
			}
			XMLString::release(&minXMLStr);
            
            XMLCh *maxXMLStr = XMLString::transcode("max");
			if(bandElement->hasAttribute(maxXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(maxXMLStr));
				bandStat->maxField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMax = true;
			}
			else
			{
				bandStat->calcMax = false;
			}
			XMLString::release(&maxXMLStr);
            
            XMLCh *meanXMLStr = XMLString::transcode("mean");
			if(bandElement->hasAttribute(meanXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(meanXMLStr));
				bandStat->meanField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMean = true;
			}
			else
			{
				bandStat->calcMean = false;
			}
			XMLString::release(&meanXMLStr);
            
            XMLCh *stdDevXMLStr = XMLString::transcode("stddev");
			if(bandElement->hasAttribute(stdDevXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(stdDevXMLStr));
				bandStat->stdDevField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcStdDev = true;
			}
			else
			{
				bandStat->calcStdDev = false;
			}
			XMLString::release(&stdDevXMLStr);
            
            XMLCh *sumXMLStr = XMLString::transcode("sum");
			if(bandElement->hasAttribute(sumXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(sumXMLStr));
				bandStat->sumField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcSum = true;
			}
			else
			{
				bandStat->calcSum = false;
			}
			XMLString::release(&sumXMLStr);
            
            bandStat->calcMedian = false;
            bandStats->push_back(bandStat);
		}
        XMLString::release(&rsgisBandXMLStr);
        
    }
    else if(XMLString::equals(optionExport2GDAL, optionXML))
    {
        this->option = RSGISExeRasterGIS::export2gdal;
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *outputXMLStr = XMLString::transcode("output");
        if(argElement->hasAttribute(outputXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
            this->outputFile = string(charValue);
            XMLString::release(&charValue);
            
            XMLCh *formatXMLStr = XMLString::transcode("format");
            if(argElement->hasAttribute(formatXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
                this->imageFormat = string(charValue);
                XMLString::release(&charValue);
            }
            else
            {
                std::cerr << "No \'format\' attribute was provided. Defaulting to use KEA\n";
                this->imageFormat = "KEA";
            }
            XMLString::release(&formatXMLStr);
            
            outputImagePathProvide = true;
        }
        else
        {
            outputImagePathProvide = false;
        }
        XMLString::release(&outputXMLStr);
        
        
        
        
    }
    else if(XMLString::equals(optionPopAttributeStatsAllBands, optionXML))
    {
        this->option = RSGISExeRasterGIS::popattributestatsallbands;
        XMLCh *inputXMLStr = XMLString::transcode("input");
        if(argElement->hasAttribute(inputXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
            this->inputImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
        }
        XMLString::release(&inputXMLStr);
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        XMLCh *noDataXMLStr = XMLString::transcode("nodata");
        if(argElement->hasAttribute(noDataXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(noDataXMLStr));
            this->noDataVal = mathUtils.strtofloat(string(charValue));
            this->noDataValDefined = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->noDataValDefined = false;
        }
        XMLString::release(&noDataXMLStr);
		
        imageStats = new RSGISBandAttStats();
        
        XMLCh *minXMLStr = XMLString::transcode("min");
        if(argElement->hasAttribute(minXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(minXMLStr));
            imageStats->minField = string(charValue);
            XMLString::release(&charValue);
            
            imageStats->calcMin = true;
        }
        else
        {
            imageStats->calcMin = false;
        }
        XMLString::release(&minXMLStr);
        
        XMLCh *maxXMLStr = XMLString::transcode("max");
        if(argElement->hasAttribute(maxXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(maxXMLStr));
            imageStats->maxField = string(charValue);
            XMLString::release(&charValue);
            
            imageStats->calcMax = true;
        }
        else
        {
            imageStats->calcMax = false;
        }
        XMLString::release(&maxXMLStr);
        
        XMLCh *meanXMLStr = XMLString::transcode("mean");
        if(argElement->hasAttribute(meanXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(meanXMLStr));
            imageStats->meanField = string(charValue);
            XMLString::release(&charValue);
            
            imageStats->calcMean = true;
        }
        else
        {
            imageStats->calcMean = false;
        }
        XMLString::release(&meanXMLStr);
        
        XMLCh *stdDevXMLStr = XMLString::transcode("stddev");
        if(argElement->hasAttribute(stdDevXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(stdDevXMLStr));
            imageStats->stdDevField = string(charValue);
            XMLString::release(&charValue);
            
            imageStats->calcStdDev = true;
        }
        else
        {
            imageStats->calcStdDev = false;
        }
        XMLString::release(&stdDevXMLStr);
        
        
        XMLCh *medianXMLStr = XMLString::transcode("median");
        if(argElement->hasAttribute(medianXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(medianXMLStr));
            imageStats->medianField = string(charValue);
            XMLString::release(&charValue);
            
            imageStats->calcMedian = true;
        }
        else
        {
            imageStats->calcMedian = false;
        }
        XMLString::release(&medianXMLStr);
        
        XMLCh *sumXMLStr = XMLString::transcode("sum");
        if(argElement->hasAttribute(sumXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(sumXMLStr));
            imageStats->sumField = string(charValue);
            XMLString::release(&charValue);
            
            imageStats->calcSum = true;
        }
        else
        {
            imageStats->calcSum = false;
        }
        XMLString::release(&sumXMLStr);
        
    }
    else if(XMLString::equals(optionSummariseField, optionXML))
    {
        this->option = RSGISExeRasterGIS::summarisefield;
                        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
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
        
        XMLCh *fieldXMLStr = XMLString::transcode("field");
        if(argElement->hasAttribute(fieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(fieldXMLStr));
            this->attField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'field\' attribute was provided.");
        }
        XMLString::release(&fieldXMLStr);
        
        
        statsSummary = new RSGISStatsSummary();
		        
        XMLCh *minXMLStr = XMLString::transcode("min");
        if(argElement->hasAttribute(minXMLStr))
        {
            statsSummary->calcMin = true;
        }
        else
        {
            statsSummary->calcMin = false;
        }
        XMLString::release(&minXMLStr);
        
        XMLCh *maxXMLStr = XMLString::transcode("max");
        if(argElement->hasAttribute(maxXMLStr))
        {
            statsSummary->calcMax = true;
        }
        else
        {
            statsSummary->calcMax = false;
        }
        XMLString::release(&maxXMLStr);
        
        XMLCh *meanXMLStr = XMLString::transcode("mean");
        if(argElement->hasAttribute(meanXMLStr))
        {
            statsSummary->calcMean = true;
        }
        else
        {
            statsSummary->calcMean = false;
        }
        XMLString::release(&meanXMLStr);
        
        XMLCh *stdDevXMLStr = XMLString::transcode("stddev");
        if(argElement->hasAttribute(stdDevXMLStr))
        {
            statsSummary->calcStdDev = true;
        }
        else
        {
            statsSummary->calcStdDev = false;
        }
        XMLString::release(&stdDevXMLStr);
        
        
        XMLCh *medianXMLStr = XMLString::transcode("median");
        if(argElement->hasAttribute(medianXMLStr))
        {
            statsSummary->calcMedian = true;
        }
        else
        {
            statsSummary->calcMedian = false;
        }
        XMLString::release(&medianXMLStr);
        
        XMLCh *sumXMLStr = XMLString::transcode("sum");
        if(argElement->hasAttribute(sumXMLStr))
        {
            statsSummary->calcSum = true;
        }
        else
        {
            statsSummary->calcSum = false;
        }
        XMLString::release(&sumXMLStr);
        
    }
    else if(XMLString::equals(optionFindNeighbours, optionXML))
    {
        this->option = RSGISExeRasterGIS::findneighbours;
        
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
        
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            inoutTable = true;
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
            
            XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
            if(argElement->hasAttribute(tableOutXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
                this->outAttTableFile = string(charValue);
                this->attInMemory = true;
                XMLString::release(&charValue);
            }
            else
            {
                this->attInMemory = false;
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
            XMLString::release(&tableOutXMLStr);
        }
        else
        {
            inoutTable = false;
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
        }
        XMLString::release(&tableXMLStr);
       
    }
    else if(XMLString::equals(optionMeanEucDist2Neighbours, optionXML))
    {
        this->option = RSGISExeRasterGIS::meaneucdist2neighbours;
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        XMLCh *neighboursXMLStr = XMLString::transcode("neighbours");
        if(argElement->hasAttribute(neighboursXMLStr))
        {
            neighboursProvided = true;
            char *charValue = XMLString::transcode(argElement->getAttribute(neighboursXMLStr));
            this->neighboursFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            cout << "Using neighbours from attribute table\n";
            neighboursProvided = false;
        }
        XMLString::release(&neighboursXMLStr);
        
        XMLCh *meanFieldXMLStr = XMLString::transcode("meanfield");
        if(argElement->hasAttribute(meanFieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(meanFieldXMLStr));
            this->attMeanField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'meanfield\' attribute was provided.");
        }
        XMLString::release(&meanFieldXMLStr);
        
        XMLCh *minFieldXMLStr = XMLString::transcode("minfield");
        if(argElement->hasAttribute(minFieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(minFieldXMLStr));
            this->attMinField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'minfield\' attribute was provided.");
        }
        XMLString::release(&minFieldXMLStr);
        
        XMLCh *maxFieldXMLStr = XMLString::transcode("maxfield");
        if(argElement->hasAttribute(maxFieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(maxFieldXMLStr));
            this->attMaxField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'maxfield\' attribute was provided.");
        }
        XMLString::release(&maxFieldXMLStr);
        
        XMLCh *rsgisAttributeXMLStr = XMLString::transcode("rsgis:attribute");
        DOMNodeList *attributeNodesList = argElement->getElementsByTagName(rsgisAttributeXMLStr);
		unsigned int numAttributes = attributeNodesList->getLength();
		
		cout << "Found " << numAttributes << " Attributes" << endl;
		
        attributeNames = new vector<string>();
        attributeNames->reserve(numAttributes);
        
		DOMElement *attElement = NULL;
        string attName = "";
		for(int i = 0; i < numAttributes; i++)
		{
			attElement = static_cast<DOMElement*>(attributeNodesList->item(i));
			            
			XMLCh *nameXMLStr = XMLString::transcode("name");
			if(attElement->hasAttribute(nameXMLStr))
			{
				char *charValue = XMLString::transcode(attElement->getAttribute(nameXMLStr));
				attributeNames->push_back(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			XMLString::release(&nameXMLStr);
        }        
    }
    else if(XMLString::equals(optionCalcIntraPxlEucDist, optionXML))
    {
        this->option = RSGISExeRasterGIS::calcintrapxleucdist;
        
        XMLCh *inputXMLStr = XMLString::transcode("input");
        if(argElement->hasAttribute(inputXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
            this->inputImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
        }
        XMLString::release(&inputXMLStr);
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        
        imageStats = new RSGISBandAttStats();
        
        XMLCh *minXMLStr = XMLString::transcode("min");
        if(argElement->hasAttribute(minXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(minXMLStr));
            imageStats->minField = string(charValue);
            XMLString::release(&charValue);
            
            imageStats->calcMin = true;
        }
        else
        {
            imageStats->calcMin = false;
        }
        XMLString::release(&minXMLStr);
        
        XMLCh *maxXMLStr = XMLString::transcode("max");
        if(argElement->hasAttribute(maxXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(maxXMLStr));
            imageStats->maxField = string(charValue);
            XMLString::release(&charValue);
            
            imageStats->calcMax = true;
        }
        else
        {
            imageStats->calcMax = false;
        }
        XMLString::release(&maxXMLStr);
        
        XMLCh *meanXMLStr = XMLString::transcode("mean");
        if(argElement->hasAttribute(meanXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(meanXMLStr));
            imageStats->meanField = string(charValue);
            XMLString::release(&charValue);
            
            imageStats->calcMean = true;
        }
        else
        {
            imageStats->calcMean = false;
        }
        XMLString::release(&meanXMLStr);
        
        XMLCh *stdDevXMLStr = XMLString::transcode("stddev");
        if(argElement->hasAttribute(stdDevXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(stdDevXMLStr));
            imageStats->stdDevField = string(charValue);
            XMLString::release(&charValue);
            
            imageStats->calcStdDev = true;
        }
        else
        {
            imageStats->calcStdDev = false;
        }
        XMLString::release(&stdDevXMLStr);
        
        
        XMLCh *medianXMLStr = XMLString::transcode("median");
        if(argElement->hasAttribute(medianXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(medianXMLStr));
            imageStats->medianField = string(charValue);
            XMLString::release(&charValue);
            
            imageStats->calcMedian = true;
        }
        else
        {
            imageStats->calcMedian = false;
        }
        XMLString::release(&medianXMLStr);
        
        XMLCh *sumXMLStr = XMLString::transcode("sum");
        if(argElement->hasAttribute(sumXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(sumXMLStr));
            imageStats->sumField = string(charValue);
            XMLString::release(&charValue);
            
            imageStats->calcSum = true;
        }
        else
        {
            imageStats->calcSum = false;
        }
        XMLString::release(&sumXMLStr);
        
        
        
        XMLCh *rsgisBandXMLStr = XMLString::transcode("rsgis:band");
        DOMNodeList *bandNodesList = argElement->getElementsByTagName(rsgisBandXMLStr);
		unsigned int numBands = bandNodesList->getLength();
		
		cout << "Found " << numBands << " Attributes" << endl;
		
        bandAttNames = new vector<RSGISBandAttName*>();
        bandAttNames->reserve(numBands);
        
        RSGISBandAttName *bandStat = NULL;
		DOMElement *bandElement = NULL;
		for(int i = 0; i < numBands; i++)
		{
			bandElement = static_cast<DOMElement*>(bandNodesList->item(i));
			
            bandStat = new RSGISBandAttName();
            
			XMLCh *bandXMLStr = XMLString::transcode("band");
			if(bandElement->hasAttribute(bandXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
				bandStat->band = mathUtils.strtounsignedint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			XMLString::release(&bandXMLStr);
            
            XMLCh *nameXMLStr = XMLString::transcode("name");
			if(bandElement->hasAttribute(nameXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(nameXMLStr));
				bandStat->attName = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			XMLString::release(&nameXMLStr);
            
            bandAttNames->push_back(bandStat);
		}
        XMLString::release(&rsgisBandXMLStr);
        
    }
    else if(XMLString::equals(optionMeanLitBandsPopAttributeStats, optionXML))
    {
        this->option = RSGISExeRasterGIS::meanlitbandspopattributestats;
        XMLCh *inputXMLStr = XMLString::transcode("input");
        if(argElement->hasAttribute(inputXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
            this->inputImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
        }
        XMLString::release(&inputXMLStr);
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        
        
        XMLCh *rsgisBandXMLStr = XMLString::transcode("rsgis:band");
        DOMNodeList *bandNodesList = argElement->getElementsByTagName(rsgisBandXMLStr);
		unsigned int numBands = bandNodesList->getLength();
		
		cout << "Found " << numBands << " Attributes" << endl;
		
                
        bandStatsMeanLit = new vector<RSGISBandAttStatsMeanLit*>();
        bandStatsMeanLit->reserve(numBands);
        
        RSGISBandAttStatsMeanLit *bandStatMeanLit = NULL;
		DOMElement *bandElement = NULL;
		for(int i = 0; i < numBands; i++)
		{
			bandElement = static_cast<DOMElement*>(bandNodesList->item(i));
			
            bandStatMeanLit = new RSGISBandAttStatsMeanLit();
            
			XMLCh *bandXMLStr = XMLString::transcode("band");
			if(bandElement->hasAttribute(bandXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
				bandStatMeanLit->band = mathUtils.strtounsignedint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			XMLString::release(&bandXMLStr);
            
            
            XMLCh *fieldXMLStr = XMLString::transcode("field");
			if(bandElement->hasAttribute(fieldXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(fieldXMLStr));
				bandStatMeanLit->fieldName = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'field\' attribute was provided.");
			}
			XMLString::release(&fieldXMLStr);
            
            
            XMLCh *typeXMLStr = XMLString::transcode("type");
			if(bandElement->hasAttribute(typeXMLStr))
			{
                string upperStr = "upper";
				char *charValue = XMLString::transcode(bandElement->getAttribute(typeXMLStr));
                string strVal = string(charValue);
                if(upperStr == strVal)
                {
                    bandStatMeanLit->useUpperVals = true;
                }
                else
                {
                    bandStatMeanLit->useUpperVals = false;
                }
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'type\' attribute was provided.");
			}
			XMLString::release(&typeXMLStr);
            
            
            XMLCh *minXMLStr = XMLString::transcode("min");
			if(bandElement->hasAttribute(minXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(minXMLStr));
				bandStatMeanLit->minField = string(charValue);
				XMLString::release(&charValue);
                
                bandStatMeanLit->calcMin = true;
			}
			else
			{
				bandStatMeanLit->calcMin = false;
			}
			XMLString::release(&minXMLStr);
            
            XMLCh *maxXMLStr = XMLString::transcode("max");
			if(bandElement->hasAttribute(maxXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(maxXMLStr));
				bandStatMeanLit->maxField = string(charValue);
				XMLString::release(&charValue);
                
                bandStatMeanLit->calcMax = true;
			}
			else
			{
				bandStatMeanLit->calcMax = false;
			}
			XMLString::release(&maxXMLStr);
            
            XMLCh *meanXMLStr = XMLString::transcode("mean");
			if(bandElement->hasAttribute(meanXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(meanXMLStr));
				bandStatMeanLit->meanField = string(charValue);
				XMLString::release(&charValue);
                
                bandStatMeanLit->calcMean = true;
			}
			else
			{
				bandStatMeanLit->calcMean = false;
			}
			XMLString::release(&meanXMLStr);
            
            XMLCh *stdDevXMLStr = XMLString::transcode("stddev");
			if(bandElement->hasAttribute(stdDevXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(stdDevXMLStr));
				bandStatMeanLit->stdDevField = string(charValue);
				XMLString::release(&charValue);
                
                bandStatMeanLit->calcStdDev = true;
			}
			else
			{
				bandStatMeanLit->calcStdDev = false;
			}
			XMLString::release(&stdDevXMLStr);
            
            
            XMLCh *medianXMLStr = XMLString::transcode("median");
			if(bandElement->hasAttribute(medianXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(medianXMLStr));
				bandStatMeanLit->medianField = string(charValue);
				XMLString::release(&charValue);
                
                bandStatMeanLit->calcMedian = true;
			}
			else
			{
				bandStatMeanLit->calcMedian = false;
			}
			XMLString::release(&medianXMLStr);
            
            XMLCh *sumXMLStr = XMLString::transcode("sum");
			if(bandElement->hasAttribute(sumXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(sumXMLStr));
				bandStatMeanLit->sumField = string(charValue);
				XMLString::release(&charValue);
                
                bandStatMeanLit->calcSum = true;
			}
			else
			{
				bandStatMeanLit->calcSum = false;
			}
			XMLString::release(&sumXMLStr);
            
            bandStatsMeanLit->push_back(bandStatMeanLit);
		}
        XMLString::release(&rsgisBandXMLStr);
        
    }
    else if(XMLString::equals(optionMeanLitPopAttributeStatsInMem, optionXML))
    {
        this->option = RSGISExeRasterGIS::meanlitpopattributestatsinmem;
        XMLCh *inputXMLStr = XMLString::transcode("input");
        if(argElement->hasAttribute(inputXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
            this->inputImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
        }
        XMLString::release(&inputXMLStr);
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        XMLCh *fieldXMLStr = XMLString::transcode("field");
        if(argElement->hasAttribute(fieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(fieldXMLStr));
            this->meanlitField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'field\' attribute was provided.");
        }
        XMLString::release(&fieldXMLStr);
        
        XMLCh *meanlitImageXMLStr = XMLString::transcode("meanlitimage");
        if(argElement->hasAttribute(meanlitImageXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(meanlitImageXMLStr));
            this->meanlitImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'meanlitimage\' attribute was provided.");
        }
        XMLString::release(&meanlitImageXMLStr);
        
        XMLCh *meanlitImageBandXMLStr = XMLString::transcode("meanlitband");
        if(argElement->hasAttribute(meanlitImageBandXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(meanlitImageBandXMLStr));
            this->meanLitBand = mathUtils.strtounsignedint(string(charValue));
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'meanlitband\' attribute was provided.");
        }
        XMLString::release(&meanlitImageBandXMLStr);
        
        XMLCh *typeXMLStr = XMLString::transcode("meanlittype");
        if(argElement->hasAttribute(typeXMLStr))
        {
            string upperStr = "above";
            char *charValue = XMLString::transcode(argElement->getAttribute(typeXMLStr));
            string strVal = string(charValue);
            if(upperStr == strVal)
            {
                meanLitUseUpper = true;
            }
            else
            {
                meanLitUseUpper = false;
            }
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'meanlittype\' attribute was provided.");
        }
        XMLString::release(&typeXMLStr);
                
        XMLCh *rsgisBandXMLStr = XMLString::transcode("rsgis:band");
        DOMNodeList *bandNodesList = argElement->getElementsByTagName(rsgisBandXMLStr);
		unsigned int numBands = bandNodesList->getLength();
		
		cout << "Found " << numBands << " Attributes" << endl;
		
        bandStats = new vector<RSGISBandAttStats*>();
        bandStats->reserve(numBands);
        
        RSGISBandAttStats *bandStat = NULL;
		DOMElement *bandElement = NULL;
		for(int i = 0; i < numBands; i++)
		{
			bandElement = static_cast<DOMElement*>(bandNodesList->item(i));
			
            bandStat = new RSGISBandAttStats();
            
			XMLCh *bandXMLStr = XMLString::transcode("band");
			if(bandElement->hasAttribute(bandXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
				bandStat->band = mathUtils.strtounsignedint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			XMLString::release(&bandXMLStr);
            
            XMLCh *minXMLStr = XMLString::transcode("min");
			if(bandElement->hasAttribute(minXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(minXMLStr));
				bandStat->minField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMin = true;
			}
			else
			{
				bandStat->calcMin = false;
			}
			XMLString::release(&minXMLStr);
            
            XMLCh *maxXMLStr = XMLString::transcode("max");
			if(bandElement->hasAttribute(maxXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(maxXMLStr));
				bandStat->maxField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMax = true;
			}
			else
			{
				bandStat->calcMax = false;
			}
			XMLString::release(&maxXMLStr);
            
            XMLCh *meanXMLStr = XMLString::transcode("mean");
			if(bandElement->hasAttribute(meanXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(meanXMLStr));
				bandStat->meanField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMean = true;
			}
			else
			{
				bandStat->calcMean = false;
			}
			XMLString::release(&meanXMLStr);
            
            XMLCh *stdDevXMLStr = XMLString::transcode("stddev");
			if(bandElement->hasAttribute(stdDevXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(stdDevXMLStr));
				bandStat->stdDevField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcStdDev = true;
			}
			else
			{
				bandStat->calcStdDev = false;
			}
			XMLString::release(&stdDevXMLStr);
            
            
            XMLCh *medianXMLStr = XMLString::transcode("median");
			if(bandElement->hasAttribute(medianXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(medianXMLStr));
				bandStat->medianField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMedian = true;
			}
			else
			{
				bandStat->calcMedian = false;
			}
			XMLString::release(&medianXMLStr);
            
            XMLCh *sumXMLStr = XMLString::transcode("sum");
			if(bandElement->hasAttribute(sumXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(sumXMLStr));
				bandStat->sumField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcSum = true;
			}
			else
			{
				bandStat->calcSum = false;
			}
			XMLString::release(&sumXMLStr);
            
            bandStats->push_back(bandStat);
		}
        XMLString::release(&rsgisBandXMLStr);
        
    }
    else if(XMLString::equals(optionMeanLitPopAttributeStats, optionXML))
    {
        this->option = RSGISExeRasterGIS::meanlitpopattributestats;
        XMLCh *inputXMLStr = XMLString::transcode("input");
        if(argElement->hasAttribute(inputXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
            this->inputImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
        }
        XMLString::release(&inputXMLStr);
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        XMLCh *fieldXMLStr = XMLString::transcode("field");
        if(argElement->hasAttribute(fieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(fieldXMLStr));
            this->meanlitField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'field\' attribute was provided.");
        }
        XMLString::release(&fieldXMLStr);
        
        XMLCh *meanlitImageXMLStr = XMLString::transcode("meanlitimage");
        if(argElement->hasAttribute(meanlitImageXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(meanlitImageXMLStr));
            this->meanlitImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'meanlitimage\' attribute was provided.");
        }
        XMLString::release(&meanlitImageXMLStr);
        
        XMLCh *meanlitImageBandXMLStr = XMLString::transcode("meanlitband");
        if(argElement->hasAttribute(meanlitImageBandXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(meanlitImageBandXMLStr));
            this->meanLitBand = mathUtils.strtounsignedint(string(charValue));
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'meanlitband\' attribute was provided.");
        }
        XMLString::release(&meanlitImageBandXMLStr);
        
        XMLCh *typeXMLStr = XMLString::transcode("meanlittype");
        if(argElement->hasAttribute(typeXMLStr))
        {
            string upperStr = "above";
            char *charValue = XMLString::transcode(argElement->getAttribute(typeXMLStr));
            string strVal = string(charValue);
            if(upperStr == strVal)
            {
                meanLitUseUpper = true;
            }
            else
            {
                meanLitUseUpper = false;
            }
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'meanlittype\' attribute was provided.");
        }
        XMLString::release(&typeXMLStr);
        
        XMLCh *rsgisBandXMLStr = XMLString::transcode("rsgis:band");
        DOMNodeList *bandNodesList = argElement->getElementsByTagName(rsgisBandXMLStr);
		unsigned int numBands = bandNodesList->getLength();
		
		cout << "Found " << numBands << " Attributes" << endl;
		
        bandStats = new vector<RSGISBandAttStats*>();
        bandStats->reserve(numBands);
        
        RSGISBandAttStats *bandStat = NULL;
		DOMElement *bandElement = NULL;
		for(int i = 0; i < numBands; i++)
		{
			bandElement = static_cast<DOMElement*>(bandNodesList->item(i));
			
            bandStat = new RSGISBandAttStats();
            
			XMLCh *bandXMLStr = XMLString::transcode("band");
			if(bandElement->hasAttribute(bandXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
				bandStat->band = mathUtils.strtounsignedint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			XMLString::release(&bandXMLStr);
            
            XMLCh *minXMLStr = XMLString::transcode("min");
			if(bandElement->hasAttribute(minXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(minXMLStr));
				bandStat->minField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMin = true;
			}
			else
			{
				bandStat->calcMin = false;
			}
			XMLString::release(&minXMLStr);
            
            XMLCh *maxXMLStr = XMLString::transcode("max");
			if(bandElement->hasAttribute(maxXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(maxXMLStr));
				bandStat->maxField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMax = true;
			}
			else
			{
				bandStat->calcMax = false;
			}
			XMLString::release(&maxXMLStr);
            
            XMLCh *meanXMLStr = XMLString::transcode("mean");
			if(bandElement->hasAttribute(meanXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(meanXMLStr));
				bandStat->meanField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMean = true;
			}
			else
			{
				bandStat->calcMean = false;
			}
			XMLString::release(&meanXMLStr);
            
            XMLCh *stdDevXMLStr = XMLString::transcode("stddev");
			if(bandElement->hasAttribute(stdDevXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(stdDevXMLStr));
				bandStat->stdDevField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcStdDev = true;
			}
			else
			{
				bandStat->calcStdDev = false;
			}
			XMLString::release(&stdDevXMLStr);
            
            XMLCh *sumXMLStr = XMLString::transcode("sum");
			if(bandElement->hasAttribute(sumXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(sumXMLStr));
				bandStat->sumField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcSum = true;
			}
			else
			{
				bandStat->calcSum = false;
			}
			XMLString::release(&sumXMLStr);
            
            bandStat->calcMedian = false;
            bandStats->push_back(bandStat);
		}
        XMLString::release(&rsgisBandXMLStr);
        
    }
    else if(XMLString::equals(optionExportField2ASCII, optionXML))
    {
        this->option = RSGISExeRasterGIS::exportfield2ascii;
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        XMLCh *fieldXMLStr = XMLString::transcode("field");
        if(argElement->hasAttribute(fieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(fieldXMLStr));
            this->attField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'field\' attribute was provided.");
        }
        XMLString::release(&fieldXMLStr);
        
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
    }
    else if(XMLString::equals(optionExport2HDF, optionXML))
    {
        this->option = RSGISExeRasterGIS::export2hdf;
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'tableout\' attribute was provided.");
        }
        XMLString::release(&tableOutXMLStr);
    }
    else if(XMLString::equals(optionExport2ASCII, optionXML))
    {
        this->option = RSGISExeRasterGIS::export2ascii;
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'tableout\' attribute was provided.");
        }
        XMLString::release(&tableOutXMLStr);
    }
    else if(XMLString::equals(optionPopMeanSumAttributes, optionXML))
    {
        this->option = RSGISExeRasterGIS::popmeansumattributes;
        XMLCh *inputXMLStr = XMLString::transcode("input");
        if(argElement->hasAttribute(inputXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
            this->inputImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
        }
        XMLString::release(&inputXMLStr);
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        
        
        XMLCh *rsgisBandXMLStr = XMLString::transcode("rsgis:band");
        DOMNodeList *bandNodesList = argElement->getElementsByTagName(rsgisBandXMLStr);
		unsigned int numBands = bandNodesList->getLength();
		
		cout << "Found " << numBands << " Attributes" << endl;
		
        bandStats = new vector<RSGISBandAttStats*>();
        bandStats->reserve(numBands);
        
        RSGISBandAttStats *bandStat = NULL;
		DOMElement *bandElement = NULL;
		for(int i = 0; i < numBands; i++)
		{
			bandElement = static_cast<DOMElement*>(bandNodesList->item(i));
			
            bandStat = new RSGISBandAttStats();
            
			XMLCh *bandXMLStr = XMLString::transcode("band");
			if(bandElement->hasAttribute(bandXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
				bandStat->band = mathUtils.strtounsignedint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			XMLString::release(&bandXMLStr);
            
            XMLCh *meanXMLStr = XMLString::transcode("mean");
			if(bandElement->hasAttribute(meanXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(meanXMLStr));
				bandStat->meanField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMean = true;
			}
			else
			{
				bandStat->calcMean = false;
			}
			XMLString::release(&meanXMLStr);            
            
            XMLCh *sumXMLStr = XMLString::transcode("sum");
			if(bandElement->hasAttribute(sumXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(sumXMLStr));
				bandStat->sumField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcSum = true;
			}
			else
			{
				bandStat->calcSum = false;
			}
			XMLString::release(&sumXMLStr);
            
            bandStat->calcMin = false;
            bandStat->calcMax = false;
            bandStat->calcStdDev = false;
            bandStat->calcMedian = false;
            bandStats->push_back(bandStat);
		}
        XMLString::release(&rsgisBandXMLStr);
        
    }
    else if(XMLString::equals(optionPrintAttSummary, optionXML))
    {
        this->option = RSGISExeRasterGIS::printattsummary;
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
    }
    else if(XMLString::equals(optionExportSize, optionXML))
    {
        this->option = RSGISExeRasterGIS::exportsize;
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
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
        
        XMLCh *fileXMLStr = XMLString::transcode("file");
        if(argElement->hasAttribute(fileXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(fileXMLStr));
            string fileOut = string(charValue);
            if(fileOut == "trunk")
            {
                outFileType = rsgis_trunk;
            }
            else if(fileOut == "append")
            {
                outFileType = rsgis_append;
            }
            else
            {
                cerr << "Default is to append to output file.";
            }
                
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'file\' attribute was provided.");
        }
        XMLString::release(&fileXMLStr);
    }
    else if(XMLString::equals(optionPopAttributeStatsThresholded, optionXML))
    {
        this->option = RSGISExeRasterGIS::popattributestatsthresholded;
        XMLCh *inputXMLStr = XMLString::transcode("input");
        if(argElement->hasAttribute(inputXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
            this->inputImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
        }
        XMLString::release(&inputXMLStr);
        
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
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
                
        XMLCh *rsgisBandXMLStr = XMLString::transcode("rsgis:band");
        DOMNodeList *bandNodesList = argElement->getElementsByTagName(rsgisBandXMLStr);
		unsigned int numBands = bandNodesList->getLength();
		
		cout << "Found " << numBands << " Attributes" << endl;
		
        bandStats = new vector<RSGISBandAttStats*>();
        bandStats->reserve(numBands);
        
        RSGISBandAttStats *bandStat = NULL;
		DOMElement *bandElement = NULL;
		for(int i = 0; i < numBands; i++)
		{
			bandElement = static_cast<DOMElement*>(bandNodesList->item(i));
			
            bandStat = new RSGISBandAttStats();
            
			XMLCh *bandXMLStr = XMLString::transcode("band");
			if(bandElement->hasAttribute(bandXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
				bandStat->band = mathUtils.strtounsignedint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			XMLString::release(&bandXMLStr);
            
            
            XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
			if(bandElement->hasAttribute(thresholdXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(thresholdXMLStr));
				bandStat->threshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
			}
			XMLString::release(&thresholdXMLStr);
            
            XMLCh *countXMLStr = XMLString::transcode("count");
			if(bandElement->hasAttribute(countXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(countXMLStr));
				bandStat->countField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcCount = true;
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'count\' attribute was provided.");
			}
			XMLString::release(&countXMLStr);
            
            XMLCh *minXMLStr = XMLString::transcode("min");
			if(bandElement->hasAttribute(minXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(minXMLStr));
				bandStat->minField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMin = true;
			}
			else
			{
				bandStat->calcMin = false;
			}
			XMLString::release(&minXMLStr);
            
            XMLCh *maxXMLStr = XMLString::transcode("max");
			if(bandElement->hasAttribute(maxXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(maxXMLStr));
				bandStat->maxField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMax = true;
			}
			else
			{
				bandStat->calcMax = false;
			}
			XMLString::release(&maxXMLStr);
            
            XMLCh *meanXMLStr = XMLString::transcode("mean");
			if(bandElement->hasAttribute(meanXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(meanXMLStr));
				bandStat->meanField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcMean = true;
			}
			else
			{
				bandStat->calcMean = false;
			}
			XMLString::release(&meanXMLStr);
            
            XMLCh *stdDevXMLStr = XMLString::transcode("stddev");
			if(bandElement->hasAttribute(stdDevXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(stdDevXMLStr));
				bandStat->stdDevField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcStdDev = true;
			}
			else
			{
				bandStat->calcStdDev = false;
			}
			XMLString::release(&stdDevXMLStr);
            
            XMLCh *sumXMLStr = XMLString::transcode("sum");
			if(bandElement->hasAttribute(sumXMLStr))
			{
				char *charValue = XMLString::transcode(bandElement->getAttribute(sumXMLStr));
				bandStat->sumField = string(charValue);
				XMLString::release(&charValue);
                
                bandStat->calcSum = true;
			}
			else
			{
				bandStat->calcSum = false;
			}
			XMLString::release(&sumXMLStr);
            
            bandStat->calcMedian = false;
            bandStats->push_back(bandStat);
		}
        XMLString::release(&rsgisBandXMLStr);
        
    }
    else if(XMLString::equals(optionKNNExtrapolate, optionXML))
    {
        this->option = RSGISExeRasterGIS::knnextrapolate;
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        XMLCh *trainingFieldXMLStr = XMLString::transcode("training");
        if(argElement->hasAttribute(trainingFieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(trainingFieldXMLStr));
            this->trainingField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'training\' attribute was provided.");
        }
        XMLString::release(&trainingFieldXMLStr);
        
        
        XMLCh *classFieldXMLStr = XMLString::transcode("classfield");
        if(argElement->hasAttribute(classFieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(classFieldXMLStr));
            this->trainingField = string(charValue);
            XMLString::release(&charValue);
            
            XMLCh *classValXMLStr = XMLString::transcode("classval");
            if(argElement->hasAttribute(classValXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(classValXMLStr));
                this->classVal = mathUtils.strtoint(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'classval\' attribute was provided.");
            }
            XMLString::release(&classValXMLStr);
            
            
            classFieldDefined = true;
        }
        else
        {
            classFieldDefined = false;
        }
        XMLString::release(&classFieldXMLStr);
        
        
        
        XMLCh *valueFieldXMLStr = XMLString::transcode("value");
        if(argElement->hasAttribute(valueFieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(valueFieldXMLStr));
            this->valueField = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'value\' attribute was provided.");
        }
        XMLString::release(&valueFieldXMLStr);
        
        XMLCh *kNumberXMLStr = XMLString::transcode("k");
        if(argElement->hasAttribute(kNumberXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(kNumberXMLStr));
            this->numkNN = mathUtils.strtounsignedint(string(charValue));
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'k\' attribute was provided.");
        }
        XMLString::release(&kNumberXMLStr);
        
        XMLCh *distThresholdXMLStr = XMLString::transcode("distancethreshold");
        if(argElement->hasAttribute(distThresholdXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(distThresholdXMLStr));
            this->distanceThreshold = mathUtils.strtofloat(string(charValue));
            XMLString::release(&charValue);
        }
        else
        {
            cerr << "WARNING: The distance metric threshold has not been defined therefore a really large number is being used such that the threshold is ignored.\n";
            this->distanceThreshold = 1000000000000;
        }
        XMLString::release(&distThresholdXMLStr);
        
        XMLCh *maxFieldXMLStr = XMLString::transcode("distancemetric");
        if(argElement->hasAttribute(maxFieldXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(maxFieldXMLStr));
            string metric = string(charValue);
            
            if(metric == "euc")
            {
                distMetric = rsgis_euclidean;
            }
            else if(metric == "man")
            {
                distMetric = rsgis_manhatten;
            }
            else if(metric == "mah")
            {
                distMetric = rsgis_mahalanobis;
            }
            else 
            {
                throw RSGISXMLArgumentsException("Did not recognised the distance metric specified.");
            }
            
            XMLString::release(&charValue);
        }
        else
        {
            cerr << "WARNING: No distance metric was specified, defaulting to Euclidean.\n"; 
            distMetric = rsgis_euclidean;
        }
        XMLString::release(&maxFieldXMLStr);
        
        XMLCh *rsgisAttributeXMLStr = XMLString::transcode("rsgis:attribute");
        DOMNodeList *attributeNodesList = argElement->getElementsByTagName(rsgisAttributeXMLStr);
		unsigned int numAttributes = attributeNodesList->getLength();
		
		cout << "Found " << numAttributes << " Attributes" << endl;
		
        attributeNames = new vector<string>();
        attributeNames->reserve(numAttributes);
        
		DOMElement *attElement = NULL;
        string attName = "";
		for(int i = 0; i < numAttributes; i++)
		{
			attElement = static_cast<DOMElement*>(attributeNodesList->item(i));
            
			XMLCh *nameXMLStr = XMLString::transcode("name");
			if(attElement->hasAttribute(nameXMLStr))
			{
				char *charValue = XMLString::transcode(attElement->getAttribute(nameXMLStr));
				attributeNames->push_back(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			XMLString::release(&nameXMLStr);
        }        
    }
    else if(XMLString::equals(optionPopBoolField, optionXML))
	{		
		this->option = RSGISExeRasterGIS::popboolfield;
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->attTableFile = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *tableOutXMLStr = XMLString::transcode("tableout");
        if(argElement->hasAttribute(tableOutXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableOutXMLStr));
            this->outAttTableFile = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            this->attInMemory = false;
        }
        XMLString::release(&tableOutXMLStr);
        
        try 
        {
            this->statements = RSGISAttributeTable::generateStatments(argElement);
        } 
        catch (RSGISException &e) 
        {
            throw RSGISXMLArgumentsException(e.what());
        }
	}
    else if(XMLString::equals(optionCopyGDALATT, optionXML))
	{		
		this->option = RSGISExeRasterGIS::copyGDALATT;
        
        XMLCh *tableXMLStr = XMLString::transcode("table");
        if(argElement->hasAttribute(tableXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(tableXMLStr));
            this->inputImage = string(charValue);
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'table\' attribute was provided.");
        }
        XMLString::release(&tableXMLStr);
        
        
        XMLCh *imageXMLStr = XMLString::transcode("image");
        if(argElement->hasAttribute(imageXMLStr))
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
            this->clumpsImage = string(charValue);
            this->attInMemory = true;
            XMLString::release(&charValue);
        }
        else
        {
            throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
        }
        XMLString::release(&imageXMLStr);
	}
    else
	{
		string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeRasterGIS.");
		throw RSGISXMLArgumentsException(message.c_str());
	}
	
	parsed = true;
	
	XMLString::release(&algorName);
	XMLString::release(&algorXMLStr);
	XMLString::release(&optionXMLStr);
    XMLString::release(&optionPopAttributeMean);
    XMLString::release(&optionExportTable2Img);
    XMLString::release(&optionBoolClassTable);
    XMLString::release(&optionAddFields);
    XMLString::release(&optionRegionGrowThres);
    XMLString::release(&optionFindMajority);
    XMLString::release(&optionCountPolyIntersects);
    XMLString::release(&optionCalcAttributes);
    XMLString::release(&optionPopAttributeStatsInMem);
    XMLString::release(&optionExport2GDAL);
    XMLString::release(&optionPopAttributeStatsAllBands);
    XMLString::release(&optionSummariseField);
    XMLString::release(&optionFindNeighbours);
    XMLString::release(&optionMeanEucDist2Neighbours);
    XMLString::release(&optionCalcIntraPxlEucDist);
    XMLString::release(&optionMeanLitBandsPopAttributeStats);
    XMLString::release(&optionMeanLitPopAttributeStatsInMem);
    XMLString::release(&optionMeanLitPopAttributeStats);
    XMLString::release(&optionExportField2ASCII);
    XMLString::release(&optionExport2HDF);
    XMLString::release(&optionExport2ASCII);
    XMLString::release(&optionPopMeanSumAttributes);
    XMLString::release(&optionPrintAttSummary);
    XMLString::release(&optionExportSize);
    XMLString::release(&optionPopAttributeStatsThresholded);
    XMLString::release(&optionKNNExtrapolate);
    XMLString::release(&optionPopBoolField);
    XMLString::release(&optionCopyGDALATT);
    
    XMLString::release(&optionRSGISBool);
    XMLString::release(&optionRSGISInt);
    XMLString::release(&optionRSGISFloat);
    XMLString::release(&optionRSGISString);
}

void RSGISExeRasterGIS::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(this->option == RSGISExeRasterGIS::createattributetable)
		{
            cout << "A command to create a new attribute table with pixel counts\n";
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Output File: " << this->outputFile << endl;
            
            GDALAllRegister();
            try
            {
                GDALDataset *clumpsDataset = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(clumpsDataset == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                
                RSGISAttributeTable *attTable = NULL;
                RSGISCreateNewAttributeTable createTable;
                
                if(this->attInMemory)
                {
                    attTable = createTable.createAndPopPixelCount(clumpsDataset, this->attInMemory, this->outputFile);
                    cout << "Exporting to HDF5\n";
                    attTable->exportHDF5(this->outputFile);
                }
                else
                {
                    attTable = createTable.createAndPopPixelCountOffLine(clumpsDataset, this->attInMemory, this->outputFile);
                }
                
                delete attTable;
                
                GDALClose(clumpsDataset);
            }
            catch(RSGISException &e)
            {
                throw e;
            }	
		}
        else if(this->option == RSGISExeRasterGIS::popattributemean)
		{
            cout << "A command to create a new attribute table and populate it with the image means\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Output File: " << this->outputFile << endl;
            cout << "Attribute Prefix: " << this->attprefix << endl;
            
            GDALAllRegister();
            GDALDataset **datasets = NULL;
            try
            {
                datasets = new GDALDataset*[2];
                datasets[0] = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(datasets[0] == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                datasets[1] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                if(datasets[1] == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                RSGISAttributeTable *attTable;
                RSGISPopulateAttributeTableBandMeans popMeans;
                
                attTable = popMeans.populateWithBandsMeans(datasets, 2, this->attprefix);
                
                cout << "Exporting to ASCII\n";
                attTable->exportHDF5(this->outputFile);
                
                delete attTable;
                
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                delete[] datasets;
            }
            catch(RSGISException &e)
            {
                throw e;
            }	
		}
        else if(this->option == RSGISExeRasterGIS::exporttable2img)
        {
            cout << "A command to create an image populated with values from an attribute table.\n";
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Attribute Table: " << this->attTableFile << endl;
            cout << "Output Image: " << this->outputFile << endl;
            for(vector<pair<unsigned int, string> >::iterator iterBands = bands->begin(); iterBands != bands->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands).first << " will be attribute " << (*iterBands).second << endl;
            }
            
            GDALAllRegister();
            GDALDataset *clumpsDatasets = NULL;
            try
            {
                clumpsDatasets = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(clumpsDatasets == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                
                cout << "Importing attribute table.\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                RSGISCreateImageFromAttributeTable createImg;
                createImg.createImageFromAttTable(clumpsDatasets, this->outputFile, attTable, bands, imageFormat);
                
                delete attTable;
                
                GDALClose(clumpsDatasets);
            }
            catch(RSGISException &e)
            {
                throw e;
            }
            
        }
        else if(this->option == RSGISExeRasterGIS::boolclasstable)
        {
            cout << "A command to apply boolean statements to the attribute table (i.e., for classification)\n";
            cout << "Input Table: " << this->attTableFile << endl;
            if(this->attInMemory)
            {
                cout << "Output Table: " << this->outAttTableFile << endl;
            }
            
            try
            {
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(this->attInMemory)
                {
                    if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                    }
                    else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Could not identify attribute table file type.");
                    }
                }
                else
                {
                    attTable = RSGISAttributeTableHDF::importFromHDF5(attTableFile, false);
                }
                //cout << "Adding Field\n";
                //attTable->addAttIntField("class", 0);
                cout << "Create output field\n";
                attTable->createIfStatementsFields(statements, rsgis_int);
                cout << "Populating statements with indexes\n";
                attTable->populateIfStatementsWithIdxs(statements);
                cout << "Apply if statement\n";
                attTable->applyIfStatements(statements);
                if(this->attInMemory)
                {
                    cout << "Exporting Attribute Table\n";
                    attTable->exportHDF5(outAttTableFile);
                    //attTable->exportASCII(outAttTableFile);
                }
                cout << "Finished\n";
                delete attTable;
            }
            catch(RSGISException &e)
            {
                throw e;
            }
        }
        else if(this->option == RSGISExeRasterGIS::addfields)
        {
            cout << "A command to add new fields to the attribute table\n";
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Adding: ";
            bool first = true;
            for(vector<RSGISAttribute*>::iterator iterAtt = attributes->begin(); iterAtt != attributes->end(); ++iterAtt)
            {
                if(first)
                {
                    cout << (*iterAtt)->name;
                    first = false;
                }
                else
                {
                    cout << ", " << (*iterAtt)->name;
                }
            }
            cout << endl;
            
            try
            {
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(this->attInMemory)
                {
                    if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                    }
                    else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Could not identify attribute table file type.");
                    }
                }
                else
                {
                    attTable = RSGISAttributeTableHDF::importFromHDF5(attTableFile, false);
                }
                cout << "Adding Fields\n";
                attTable->addAttributes(attributes);
                if(this->attInMemory)
                {
                    cout << "Exporting Attribute Table\n";
                    attTable->exportHDF5(outAttTableFile);
                    //attTable->exportASCII(outAttTableFile);
                }
                cout << "Finished\n";
                
                delete attributes;
                delete attTable;
            }
            catch(RSGISException &e)
            {
                throw e;
            }
        }
        else if(this->option == RSGISExeRasterGIS::regiongrowthres)
        {
            cout << "A command to region grow regions of the dataset using threshold\n";
            cout << "Clumps Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            
            try
            {
                GDALAllRegister();
                GDALDataset *inDataset = NULL;
                inDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
                if(inDataset == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                
                RSGISImageUtils imgUtils;
                GDALDataset *clumpsDataset = NULL;
                
                clumpsDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_UInt32, true, "");
                imgUtils.copyUIntGDALDataset(inDataset, clumpsDataset);
                GDALClose(inDataset);
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                cout << "Populating statements with indexes\n";
                attTable->populateIfStatementsWithIdxs(statements);

                cout << "Performing Region Growing\n";
                RSGISRegionGrowAttributeTable regionGrow;
                regionGrow.growClassRegionsUsingThresholds(attTable, clumpsDataset, statements, classAttributeName, classAttributeVal);
                
                cout << "Exporting Attribute Table\n";
                attTable->exportHDF5(outAttTableFile);
                cout << "Finished\n";
                
                GDALClose(clumpsDataset);
                delete attTable;
            }
            catch(RSGISException &e)
            {
                throw e;
            }
        }
        else if(this->option == RSGISExeRasterGIS::findmajority)
        {
            cout << "A command to find the majority category intersecting with each clump\n";
            cout << "Categories Image: " << this->catagoriesImage << endl;
            cout << "Clumps Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Area Field: " << this->areaField << endl;
            cout << "Majority Category Field : " << this->majorityCatField << endl;
            cout << "Majority Ratio Field : " << this->majorityRatioField << endl;
            
            try
            {
                GDALAllRegister();
                
                GDALDataset *categoriesDataset = (GDALDataset *) GDALOpen(this->catagoriesImage.c_str(), GA_ReadOnly);
                if(categoriesDataset == NULL)
                {
                    string message = string("Could not open image ") + this->catagoriesImage;
                    throw RSGISImageException(message.c_str());
                }
                
                GDALDataset *clumpsDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
                if(clumpsDataset == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                
                cout << "Find majority categories\n";
                RSGISFindClumpCategoryMajority findMajorities;
                findMajorities.findMajorityCategory(categoriesDataset, clumpsDataset, attTable, areaField, majorityRatioField, majorityCatField);
                
                
                cout << "Exporting Attribute Table\n";
                attTable->exportHDF5(outAttTableFile);
                cout << "Finished\n";
                
                GDALClose(categoriesDataset);
                GDALClose(clumpsDataset);
                delete attTable;
            }
            catch(RSGISException &e)
            {
                throw e;
            }
        }
        else if(this->option == RSGISExeRasterGIS::countpolyintersects)
        {
            cout << "A command to populate the attribute table with the number of pixels in the clumps image which intersect with the polygon.\n";
            cout << "Clumps Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Field: " << this->attField << endl;
            cout << "Shapefile: " << this->inputVector << endl;
            
            try
            {
                GDALAllRegister();
                OGRRegisterAll();
                
                GDALDataset *clumpsDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_ReadOnly);
                if(clumpsDataset == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                
                /////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
                RSGISVectorUtils vecUtils;
                string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
                
                
				OGRDataSource *inputSHPDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISException(message.c_str());
				}
				OGRLayer *inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
				}
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }

                
                RSGISAttCountPolyIntersect attCountIntersects;
                attCountIntersects.countPolysIntersections(attTable, clumpsDataset, inputSHPLayer, attField, pixelPolyOption);
                
                cout << "Exporting Attribute Table\n";
                attTable->exportHDF5(outAttTableFile);
                cout << "Finished\n";
                
                GDALClose(clumpsDataset);
                OGRDataSource::DestroyDataSource(inputSHPDS);
                delete attTable;
            }
            catch(RSGISException &e)
            {
                throw e;
            }
        }
        else if(this->option == RSGISExeRasterGIS::calcattributes)
        {
            cout << "A command to calculate an attribute from other attributes\n";
            cout << "Input Table: " << this->attTableFile << endl;
            if(this->attInMemory)
            {
                cout << "Output Table: " << this->outAttTableFile << endl;
            }
            else
            {
                cout << "Processing within existing table\n";
            }
            cout << "Expression: " << this->mathsExpression << endl;
            cout << "Field: " << this->attField << endl;
            
            try
            {
                cout << "Importing Attribute Table\n";
                RSGISAttributeTable *attTable = NULL;
                if(this->attInMemory)
                {
                    if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                    }
                    else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Could not identify attribute table file type.");
                    }
                }
                else
                {
                    attTable = RSGISAttributeTableHDF::importFromHDF5(attTableFile, false);
                }
                
                cout << "Evalute Expression\n";
                attTable->calculateFieldsMUParser(this->mathsExpression, this->attField, this->attFieldDT, this->variables);
                
                if(this->attInMemory)
                {
                    cout << "Exporting Attribute Table\n";
                    attTable->exportHDF5(outAttTableFile);
                }
                cout << "Finished\n";
                
                delete attTable;
            }
            catch(RSGISException &e)
            {
                throw e;
            }
        }
        else if(this->option == RSGISExeRasterGIS::popattributestatsinmem)
        {
            cout << "A command to populate an attribute table with statistics from an image.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": ";
                if((*iterBands)->calcMin)
                {
                    cout << "MIN (" << (*iterBands)->minField << ") ";
                }
                
                if((*iterBands)->calcMax)
                {
                    cout << "MAX (" << (*iterBands)->maxField << ") ";
                }
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                if((*iterBands)->calcStdDev)
                {
                    cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                }
                
                if((*iterBands)->calcMedian)
                {
                    cout << "MEDIAN (" << (*iterBands)->medianField << ") ";
                }
                
                cout << endl;
            }

            try 
            {
                GDALAllRegister();
                GDALDataset **datasets = new GDALDataset*[2];
                datasets[0] = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(datasets[0] == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                datasets[1] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                if(datasets[1] == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                cout << "Calculating Statistics\n";
                RSGISPopulateAttributeTableBandStats calcBandStats;
                calcBandStats.populateWithBandStatisticsInMem(attTable, datasets, 2, bandStats);
                
                cout << "Exporting Attribute Table\n";
                attTable->exportHDF5(outAttTableFile);
                cout << "Finished\n";               
                
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                delete[] datasets;
                delete attTable;
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }            
        }
        else if(this->option == RSGISExeRasterGIS::popattributestats)
        {
            cout << "A command to populate an attribute table with statistics from an image in a memory efficient manor.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            if(this->attInMemory)
            {
                cout << "Output Table: " << this->outAttTableFile << endl;
            }
            else
            {
                cout << "Processing within existing table\n";
            }
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": ";
                if((*iterBands)->calcMin)
                {
                    cout << "MIN (" << (*iterBands)->minField << ") ";
                }
                
                if((*iterBands)->calcMax)
                {
                    cout << "MAX (" << (*iterBands)->maxField << ") ";
                }
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                if((*iterBands)->calcStdDev)
                {
                    cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                }
                
                cout << endl;
            }
            
            try 
            {
                GDALAllRegister();
                GDALDataset **datasets = new GDALDataset*[2];
                datasets[0] = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(datasets[0] == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                datasets[1] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                if(datasets[1] == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(this->attInMemory)
                {
                    if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                    }
                    else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Could not identify attribute table file type.");
                    }
                }
                else
                {
                    attTable = RSGISAttributeTableHDF::importFromHDF5(attTableFile, false);
                }
                
                cout << "Calculating Statistics\n";
                RSGISPopulateAttributeTableBandStats calcBandStats;
                calcBandStats.populateWithBandStatisticsWithinAtt(attTable, datasets, 2, bandStats);
                
                if(this->attInMemory)
                {
                    cout << "Exporting Attribute Table\n";
                    attTable->exportHDF5(outAttTableFile);
                }
                cout << "Finished\n";               
                
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                delete[] datasets;
                delete attTable;
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }            
        }
        else if(this->option == RSGISExeRasterGIS::export2gdal)
		{
            cout << "Export an attribute table to a GDAL Dataset with a raster attribute table\n";
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Table: " << this->attTableFile << endl;
            if(outputImagePathProvide)
            {
                cout << "Output File: " << this->outputFile << endl;
                cout << "Format: " << this->imageFormat << endl;
            }
            else
            {
                cout << "Appending attribute table to input image\n";
            }
            
            GDALAllRegister();
            try
            {
                
                GDALDataset *outRATDataset = NULL;
                
                if(outputImagePathProvide)
                {
                    GDALDataset *clumpsDataset = (GDALDataset *) GDALOpen   (this->clumpsImage.c_str(), GA_ReadOnly);
                    if(clumpsDataset == NULL)
                    {
                        string message = string("Could not open image ") + this->clumpsImage;
                        throw RSGISImageException(message.c_str());
                    }
                    
                    cout << "Copy Raster Dataset.\n";
                    RSGISImageUtils imageUtils;
                    outRATDataset = imageUtils.createCopy(clumpsDataset, this->outputFile, this->imageFormat, GDT_UInt32);
                    imageUtils.copyUIntGDALDataset(clumpsDataset, outRATDataset);
                    
                    GDALClose(clumpsDataset);
                }
                else
                {
                    outRATDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_Update);
                    if(outRATDataset == NULL)
                    {
                        string message = string("Could not open image ") + this->clumpsImage;
                        throw RSGISImageException(message.c_str());
                    }
                }
                
                cout << "Import attribute table\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                cout << "Adding RAT\n";
                attTable->exportGDALRaster(outRATDataset, 1);
                
                cout << "Finished\n";
                delete attTable;
                
                
                GDALClose(outRATDataset);
            }
            catch(RSGISException &e)
            {
                throw e;
            }	
		}
        else if(this->option == RSGISExeRasterGIS::popattributestatsallbands)
        {
            cout << "A command to populate an attribute table with statistics from an image.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            if(noDataValDefined)
            {
                cout << "No Data Value: " << this->noDataVal << endl;
            }
            cout << "Statistics to be calculated:\n";
            if(imageStats->calcMin)
            {
                cout << "MIN (" << imageStats->minField << ") ";
            }
            if(imageStats->calcMax)
            {
                cout << "MAX (" << imageStats->maxField << ") ";
            }
            if(imageStats->calcMean)
            {
                cout << "MEAN (" << imageStats->meanField << ") ";
            }
            if(imageStats->calcSum)
            {
                cout << "SUM (" << imageStats->sumField << ") ";
            }
            if(imageStats->calcStdDev)
            {
                cout << "STDDEV (" << imageStats->stdDevField << ") ";
            }
            if(imageStats->calcMedian)
            {
                cout << "MEDIAN (" << imageStats->medianField << ") ";
            }
            cout << endl;
            
            try 
            {
                GDALAllRegister();
                GDALDataset **datasets = new GDALDataset*[2];
                datasets[0] = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(datasets[0] == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                datasets[1] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                if(datasets[1] == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                cout << "Calculating Statistics\n";
                RSGISPopulateAttributeTableImageStats calcImageStats;
                calcImageStats.populateWithImageStatisticsInMem(attTable, datasets, 2, imageStats, this->noDataVal, this->noDataValDefined);
                
                cout << "Exporting Attribute Table\n";
                attTable->exportHDF5(outAttTableFile);
                cout << "Finished\n";               
                
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                delete[] datasets;
                delete attTable;
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }            
        }
        else if(this->option == RSGISExeRasterGIS::summarisefield)
        {
            cout << "A command to summarise an attribute from with the table.\n";
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output: " << this->outAttTableFile << endl;
            cout << "Field: " << this->attField << endl;
            cout << "Statistics to be calculated:\n";
            if(statsSummary->calcMin)
            {
                cout << "MIN ";
            }
            if(statsSummary->calcMax)
            {
                cout << "MAX ";
            }
            if(statsSummary->calcMean)
            {
                cout << "MEAN ";
            }
            if(statsSummary->calcSum)
            {
                cout << "SUM ";
            }
            if(statsSummary->calcStdDev)
            {
                cout << "STDDEV ";
            }
            if(statsSummary->calcMedian)
            {
                cout << "MEDIAN ";
            }
            cout << endl;
            
            try 
            {
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }

                cout << "Getting data from attribute Table\n";
                vector<double> *data = attTable->getFieldValues(this->attField);
                
                cout << "Calculating Statistics\n";
                RSGISMathsUtils mathUtils;
                mathUtils.generateStats(data, statsSummary);
                
                cout << "Exporting to file\n";
                ofstream outTxtFile;
                outTxtFile.open(outputFile.c_str(), ios::out | ios::trunc);
                if(statsSummary->calcMin)
                {
                    outTxtFile << "Min, " << statsSummary->min << endl;
                }
                if(statsSummary->calcMax)
                {
                    outTxtFile << "Max, " << statsSummary->max << endl;
                }
                if(statsSummary->calcMean)
                {
                    outTxtFile << "Mean, " << statsSummary->mean << endl;
                }
                if(statsSummary->calcStdDev)
                {
                    outTxtFile << "StdDev, " << statsSummary->stdDev << endl;
                }
                if(statsSummary->calcSum)
                {
                    outTxtFile << "Sum, " << statsSummary->sum << endl;
                }
                if(statsSummary->calcMedian)
                {
                    outTxtFile << "Median, " << statsSummary->median << endl;
                }
                outTxtFile.flush();
                outTxtFile.close();

                delete attTable;
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }            
        }
        else if(this->option == RSGISExeRasterGIS::findneighbours)
        {
            cout << "A command to find the neighbours of the clumps and save as a neighbours file.\n";
            cout << "Clump Image: " << this->clumpsImage << endl;
            if(inoutTable)
            {
                cout << "Input Table: " << this->attTableFile << endl;
                cout << "Output: " << this->outAttTableFile << endl;
            }
            else
            {
                cout << "Output File: " << this->outputFile << endl;
            }
            
            GDALAllRegister();
            try 
            {
                GDALDataset *clumpsDataset = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(clumpsDataset == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                
                RSGISFindClumpNeighbours findNeighbours;
                
                if(inoutTable)
                {
                    RSGISAttributeTable *attTable = NULL;
                    if(this->attInMemory)
                    {
                        if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                        {
                            attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                        }
                        else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                        {
                            attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("Could not identify attribute table file type.");
                        }
                        
                        findNeighbours.findNeighbours(clumpsDataset, attTable);
                        
                        if(this->attInMemory)
                        {
                            cout << "Exporting Attribute Table\n";
                            attTable->exportHDF5(this->outAttTableFile);
                        }
                    }
                    else
                    {
                        attTable = RSGISAttributeTableHDF::importFromHDF5(attTableFile, false, this->cacheSize);
                        
                        findNeighbours.findNeighboursInBlocks(clumpsDataset, attTable);
                    }
                    
                    delete attTable;
                }
                else
                {
                    vector<list<unsigned long >* > *neighbours = findNeighbours.findNeighbours(clumpsDataset);
                    
                    cout << "Exporting to File\n";
                    RSGISAttributeTableNeighbours attNeighbours;
                    attNeighbours.exportToTextFile(neighbours, this->outputFile);
                    attNeighbours.clearMemory(neighbours);
                }
                
            }
            catch (RSGISException &e) 
            {
                throw e;
            }
        }
        else if(this->option == RSGISExeRasterGIS::meaneucdist2neighbours)
        {
            cout << "A command to find mean euclidean distance to neighbours\n";
            cout << "Table: " << this->attTableFile << endl;
            if(neighboursProvided)
            {
                cout << "Neighbours: " << this->neighboursFile << endl;
            }
            cout << "Out Table: " << this->outAttTableFile << endl;
            cout << "Out Field: " << this->attField << endl;
            cout << "Fields to calculate distance: \n";
            for(vector<string>::iterator iterNames = attributeNames->begin(); iterNames != attributeNames->end(); ++iterNames)
            {
                cout << "\t" << *iterNames << endl;
            }
            
            try
            {
                cout << "Importing Attribute Table\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }

                RSGISFindMeanDist2Neighbours calcMeanDist2Neighbours;
                if(neighboursProvided)
                {
                    cout << "Importing Neighbours\n";
                    RSGISAttributeTableNeighbours neighboursUtils;
                    vector<vector<unsigned long > > *neighbours = neighboursUtils.importFromTextFile(this->neighboursFile);
                    cout << "Calculating Distance to neighbours\n";
                    calcMeanDist2Neighbours.findMeanEuclideanDist2Neighbours(attTable, neighbours, attributeNames, this->attMeanField, this->attMaxField, this->attMinField);
                    delete neighbours;
                }
                else
                {
                    cout << "Calculating Distance to neighbours\n";
                    calcMeanDist2Neighbours.findMeanEuclideanDist2Neighbours(attTable, attributeNames, this->attMeanField, this->attMaxField, this->attMinField);
                }
                
                cout << "Exporting Attribute Table\n";
                attTable->exportHDF5(this->outAttTableFile);
                
                delete attTable;
                delete attributeNames;
            }
            catch(RSGISException &e)
            {
                throw e;
            }
        }
        else if(this->option == RSGISExeRasterGIS::calcintrapxleucdist)
        {
            cout << "A command to calculate the euclidean distance to individual pixels within a segment.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Statistics to be calculated:\n";
            if(imageStats->calcMin)
            {
                cout << "MIN (" << imageStats->minField << ") ";
            }
            if(imageStats->calcMax)
            {
                cout << "MAX (" << imageStats->maxField << ") ";
            }
            if(imageStats->calcMean)
            {
                cout << "MEAN (" << imageStats->meanField << ") ";
            }
            if(imageStats->calcSum)
            {
                cout << "SUM (" << imageStats->sumField << ") ";
            }
            if(imageStats->calcStdDev)
            {
                cout << "STDDEV (" << imageStats->stdDevField << ") ";
            }
            if(imageStats->calcMedian)
            {
                cout << "MEDIAN (" << imageStats->medianField << ") ";
            }
            cout << endl;
            for(vector<RSGISBandAttName*>::iterator iterBands = bandAttNames->begin(); iterBands != bandAttNames->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": " << (*iterBands)->attName << endl;
            }
            
            try 
            {
                GDALAllRegister();
                GDALDataset **datasets = new GDALDataset*[2];
                datasets[0] = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(datasets[0] == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                datasets[1] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                if(datasets[1] == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                cout << "Calculating Statistics\n";
                RSGISCalcAttTableWithinSegmentPixelDistStats calcDistStats;
                calcDistStats.populateWithImageStatistics(attTable, datasets, 2, bandAttNames, imageStats);
                
                cout << "Exporting Attribute Table\n";
                attTable->exportHDF5(outAttTableFile);
                cout << "Finished\n";               
                
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                delete[] datasets;
                delete attTable;
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }            
        }
        else if(this->option == RSGISExeRasterGIS::meanlitbandspopattributestats)
        {
            cout << "A command to populate an attribute table with statistics from an image where a segment value either above or below is used.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStatsMeanLit*>::iterator iterBands = bandStatsMeanLit->begin(); iterBands != bandStatsMeanLit->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << "(field = \'" << (*iterBands)->fieldName << "\'): ";
                if((*iterBands)->calcMin)
                {
                    cout << "MIN (" << (*iterBands)->minField << ") ";
                }
                
                if((*iterBands)->calcMax)
                {
                    cout << "MAX (" << (*iterBands)->maxField << ") ";
                }
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                if((*iterBands)->calcStdDev)
                {
                    cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                }
                
                if((*iterBands)->calcMedian)
                {
                    cout << "MEDIAN (" << (*iterBands)->medianField << ") ";
                }
                
                cout << endl;
            }
            
            try 
            {
                GDALAllRegister();
                GDALDataset **datasets = new GDALDataset*[2];
                datasets[0] = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(datasets[0] == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                datasets[1] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                if(datasets[1] == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                cout << "Calculating Statistics\n";
                RSGISPopulateAttributeTableBandStatsMeanLitBands calcBandStats;
                calcBandStats.populateWithBandStatisticsInMem(attTable, datasets, 2, bandStatsMeanLit);
                
                cout << "Exporting Attribute Table\n";
                attTable->exportHDF5(outAttTableFile);
                cout << "Finished\n";               
                
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                delete[] datasets;
                delete attTable;
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }            
        }
        else if(this->option == RSGISExeRasterGIS::meanlitpopattributestatsinmem)
        {
            cout << "A command to populate an attribute table with mean-lit statistics from an image.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Mean Lit Field: " << this->meanlitField << endl;
            cout << "Mean Lit Image: " << this->meanlitImage << " band = " << this->meanLitBand;
            if(this->meanLitUseUpper)
            {
                cout << " using upper values.\n";
            }
            else 
            {
                cout << " using lower values.\n";
            }
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": ";
                if((*iterBands)->calcMin)
                {
                    cout << "MIN (" << (*iterBands)->minField << ") ";
                }
                
                if((*iterBands)->calcMax)
                {
                    cout << "MAX (" << (*iterBands)->maxField << ") ";
                }
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                if((*iterBands)->calcStdDev)
                {
                    cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                }
                
                if((*iterBands)->calcMedian)
                {
                    cout << "MEDIAN (" << (*iterBands)->medianField << ") ";
                }
                
                cout << endl;
            }
            
            try 
            {
                GDALAllRegister();
                GDALDataset **datasets = new GDALDataset*[3];
                datasets[0] = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(datasets[0] == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                datasets[1] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                if(datasets[1] == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                datasets[2] = (GDALDataset *) GDALOpenShared(this->meanlitImage.c_str(), GA_ReadOnly);
                if(datasets[2] == NULL)
                {
                    string message = string("Could not open image ") + this->meanlitImage;
                    throw RSGISImageException(message.c_str());
                }
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                unsigned int meanLitBandIdx =  datasets[0]->GetRasterCount() +  datasets[1]->GetRasterCount() + this->meanLitBand-1;
                
                cout << "Calculating Statistics\n";
                RSGISPopulateAttributeTableBandStatsMeanLit calcBandStats;
                calcBandStats.populateWithBandStatisticsInMem(attTable, datasets, 3, bandStats, meanLitBandIdx, this->meanlitField, this->meanLitUseUpper);
                
                cout << "Exporting Attribute Table\n";
                attTable->exportHDF5(outAttTableFile);
                cout << "Finished\n";               
                
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                GDALClose(datasets[2]);
                delete[] datasets;
                delete attTable;
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }            
        }
        else if(this->option == RSGISExeRasterGIS::meanlitpopattributestats)
        {
            cout << "A command to populate an attribute table with mean-lit statistics from an image in a memory efficient manor.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Mean Lit Field: " << this->meanlitField << endl;
            cout << "Mean Lit Image: " << this->meanlitImage << " band = " << this->meanLitBand;
            if(this->meanLitUseUpper)
            {
                cout << " using upper values.\n";
            }
            else 
            {
                cout << " using lower values.\n";
            }
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": ";
                if((*iterBands)->calcMin)
                {
                    cout << "MIN (" << (*iterBands)->minField << ") ";
                }
                
                if((*iterBands)->calcMax)
                {
                    cout << "MAX (" << (*iterBands)->maxField << ") ";
                }
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                if((*iterBands)->calcStdDev)
                {
                    cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                }
                
                cout << endl;
            }
            
            try 
            {
                GDALAllRegister();
                GDALDataset **datasets = new GDALDataset*[3];
                datasets[0] = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(datasets[0] == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                datasets[1] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                if(datasets[1] == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                datasets[2] = (GDALDataset *) GDALOpenShared(this->meanlitImage.c_str(), GA_ReadOnly);
                if(datasets[2] == NULL)
                {
                    string message = string("Could not open image ") + this->meanlitImage;
                    throw RSGISImageException(message.c_str());
                }
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                unsigned int meanLitBandIdx =  datasets[0]->GetRasterCount() +  datasets[1]->GetRasterCount() + this->meanLitBand-1;
                
                cout << "Calculating Statistics\n";
                RSGISPopulateAttributeTableBandStatsMeanLit calcBandStats;
                calcBandStats.populateWithBandStatisticsWithinAtt(attTable, datasets, 3, bandStats, meanLitBandIdx, this->meanlitField, this->meanLitUseUpper);
                
                cout << "Exporting Attribute Table\n";
                attTable->exportHDF5(outAttTableFile);
                cout << "Finished\n";               
                
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                GDALClose(datasets[2]);
                delete[] datasets;
                delete attTable;
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }            
        }
        else if(this->option == RSGISExeRasterGIS::exportfield2ascii)
        {
            cout << "A command to export an attribute table column as a single ASCII table\n";
            cout << "Table: " << this->attTableFile << endl;
            cout << "Field: " << this->attField << endl;
            cout << "Output: " << this->outputFile << endl;
            
            try 
            {
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                cout << "Exporting Field to ASCII\n";
                if(attTable->hasAttribute(attField))
                {
                    RSGISAttributeDataType dType = attTable->getDataType(attField);
                    if(dType == rsgis_bool)
                    {
                        vector<bool> *vals = attTable->getBoolField(attField);
                        bool first = true;
                        ofstream outTxtFile;
                        outTxtFile.open(this->outputFile.c_str(), ios::out | ios::trunc);
                        for(vector<bool>::iterator iterVals = vals->begin(); iterVals != vals->end(); ++iterVals)
                        {
                            if(*iterVals)
                            {
                                if(first)
                                {
                                    outTxtFile << "TRUE";
                                    first = false;
                                }
                                else
                                {
                                    outTxtFile << ",TRUE";
                                }
                            }
                            else
                            {
                                if(first)
                                {
                                    outTxtFile << "FALSE";
                                    first = false;
                                }
                                else
                                {
                                    outTxtFile << ",FALSE";
                                }
                            }
                        }
                        outTxtFile.flush();
                        outTxtFile.close();
                        delete vals;
                    }
                    else if(dType == rsgis_int)
                    {
                        vector<long> *vals = attTable->getLongField(attField);
                        bool first = true;
                        ofstream outTxtFile;
                        outTxtFile.open(this->outputFile.c_str(), ios::out | ios::trunc);
                        for(vector<long>::iterator iterVals = vals->begin(); iterVals != vals->end(); ++iterVals)
                        {
                            if(first)
                            {
                                outTxtFile << *iterVals;
                                first = false;
                            }
                            else
                            {
                                outTxtFile << "," << *iterVals;
                            }
                        }
                        outTxtFile.flush();
                        outTxtFile.close();
                        delete vals;
                    }
                    else if(dType == rsgis_float)
                    {
                        vector<double> *vals = attTable->getDoubleField(attField);
                        bool first = true;
                        ofstream outTxtFile;
                        outTxtFile.open(this->outputFile.c_str(), ios::out | ios::trunc);
                        for(vector<double>::iterator iterVals = vals->begin(); iterVals != vals->end(); ++iterVals)
                        {
                            if(first)
                            {
                                outTxtFile << *iterVals;
                                first = false;
                            }
                            else
                            {
                                outTxtFile << "," << *iterVals;
                            }
                        }
                        outTxtFile.flush();
                        outTxtFile.close();
                        delete vals;
                    }
                    else if(dType == rsgis_string)
                    {
                        vector<string> *vals = attTable->getStringField(attField);
                        bool first = true;
                        ofstream outTxtFile;
                        outTxtFile.open(this->outputFile.c_str(), ios::out | ios::trunc);
                        for(vector<string>::iterator iterVals = vals->begin(); iterVals != vals->end(); ++iterVals)
                        {
                            if(first)
                            {
                                outTxtFile << *iterVals;
                                first = false;
                            }
                            else
                            {
                                outTxtFile << "," << *iterVals;
                            }
                        }
                        outTxtFile.flush();
                        outTxtFile.close();
                        delete vals;
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Data type was not recognised.");
                    }
                }
                else
                {
                    string message = this->attField + string(" field was not within the attribute table.");
                    throw RSGISAttributeTableException(message);
                }
            }
            catch (RSGISException &e) 
            {
                throw e;
            }
        }
        else if(this->option == RSGISExeRasterGIS::export2hdf)
        {
            cout << "A command to export an attribute table to a hdf file.\n";
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            
            try 
            {
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                cout << "Exporting Attribute Table:\n";
                attTable->exportHDF5(outAttTableFile);
                cout << "Finished\n";               
                
                delete attTable;
            } 
            catch (RSGISException &e) 
            {
                throw e;
            } 
        }
        else if(this->option == RSGISExeRasterGIS::export2ascii)
        {
            cout << "A command to export an attribute table to a ascii file.\n";
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            
            try 
            {
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                cout << "Exporting Attribute Table:\n";
                attTable->exportASCII(outAttTableFile);
                cout << "Finished\n";               
                
                delete attTable;
            } 
            catch (RSGISException &e) 
            {
                throw e;
            } 
        }
        else if(this->option == RSGISExeRasterGIS::popmeansumattributes)
        {
            cout << "A command to populate an attribute table with sum and mean values from an image in a memory efficient and fast manor.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            if(this->attInMemory)
            {
                cout << "Output Table: " << this->outAttTableFile << endl;
            }
            else
            {
                cout << "Processing within existing table\n";
            }
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": ";
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                cout << endl;
            }
            
            try 
            {
                GDALAllRegister();
                GDALDataset **datasets = new GDALDataset*[2];
                datasets[0] = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(datasets[0] == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                datasets[1] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                if(datasets[1] == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(this->attInMemory)
                {
                    if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                    }
                    else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Could not identify attribute table file type.");
                    }
                }
                else
                {
                    attTable = RSGISAttributeTableHDF::importFromHDF5(attTableFile, false);
                }
                
                cout << "Calculating Statistics\n";
                RSGISPopulateAttributeTableBandWithSumAndMeans calcBandStats;
                calcBandStats.populateWithBandStatistics(attTable, datasets, 2, bandStats);
                
                if(this->attInMemory)
                {
                    cout << "Exporting Attribute Table\n";
                    attTable->exportHDF5(outAttTableFile);
                }
                cout << "Finished\n";               
                
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                delete[] datasets;
                delete attTable;
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }            
        }
        else if(this->option == RSGISExeRasterGIS::printattsummary)
        {
            cout << "A command to print a summary of an attribute table.\n";
            cout << "Input table: " << this->attTableFile << endl;
            
            try 
            {
                cout << "Importing Attribute Table:\n";
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    //RSGISAttributeTableMem::importFromASCII(attTableFile);
                    cout << "Can only provide a summary for HDF files\n";
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    RSGISAttributeTableMem::printSummaryHDFAtt(attTableFile);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
            } 
            catch (RSGISException &e) 
            {
                throw e;
            } 
        }
        else if(this->option == RSGISExeRasterGIS::exportsize)
        {
            cout << "A command to export the attribute table size and output to the text file.\n";
            cout << "Input table: " << this->attTableFile << endl;
            cout << "Output file: " << this->outputFile << endl;
            if(this->outFileType == rsgis_trunk)
            {
                cout << "Truncate the output file\n";
            }
            else if(this->outFileType == rsgis_append)
            {
                cout << "Append to the output file\n";
            }
            else
            {
                throw RSGISException("Output type is not recognised.");
            }
            
            try 
            {
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                {
                    attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                }
                else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                {
                    attTable = RSGISAttributeTableHDF::importFromHDF5(attTableFile, false);
                }
                else
                {
                    throw RSGISAttributeTableException("Could not identify attribute table file type.");
                }
                
                ofstream outTxtFile;
                if(this->outFileType == rsgis_trunk)
                {
                    outTxtFile.open(this->outputFile.c_str(), ios::out | ios::trunc);
                }
                else if(this->outFileType == rsgis_append)
                {
                    outTxtFile.open(this->outputFile.c_str(), ios::out | ios::app);
                }
                else
                {
                    throw RSGISException("Output type is not recognised.");
                }
                
                if(outTxtFile.is_open())
                {
                    outTxtFile << attTable->getSize() << endl;
                    outTxtFile.flush();
                    outTxtFile.close();
                }
                else
                {
                    throw RSGISException("Could not open text file.");
                }
                
                delete attTable;
                
            } 
            catch (RSGISException &e) 
            {
                throw e;
            } 
        }
        else if(this->option == RSGISExeRasterGIS::popattributestatsthresholded)
        {
            cout << "A command to populate an attribute table with statistics from an image in a memory efficient manor where a threshold is defined per band and only values above that threshold are valid.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            if(this->attInMemory)
            {
                cout << "Output Table: " << this->outAttTableFile << endl;
            }
            else
            {
                cout << "Processing within existing table\n";
            }
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": ";
                cout << "Threshold (" << (*iterBands)->threshold << ") ";
                cout << "COUNT (" << (*iterBands)->countField << ") ";
                
                if((*iterBands)->calcMin)
                {
                    cout << "MIN (" << (*iterBands)->minField << ") ";
                }
                
                if((*iterBands)->calcMax)
                {
                    cout << "MAX (" << (*iterBands)->maxField << ") ";
                }
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                if((*iterBands)->calcStdDev)
                {
                    cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                }
                
                cout << endl;
            }
            
            try 
            {
                GDALAllRegister();
                GDALDataset **datasets = new GDALDataset*[2];
                datasets[0] = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_ReadOnly);
                if(datasets[0] == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                datasets[1] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                if(datasets[1] == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(this->attInMemory)
                {
                    if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                    }
                    else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Could not identify attribute table file type.");
                    }
                }
                else
                {
                    attTable = RSGISAttributeTableHDF::importFromHDF5(attTableFile, false);
                }
                
                cout << "Calculating Statistics\n";
                RSGISPopulateAttributeTableBandThresholdedStats calcBandStats;
                calcBandStats.populateWithBandStatisticsWithinAtt(attTable, datasets, 2, bandStats);
                
                if(this->attInMemory)
                {
                    cout << "Exporting Attribute Table\n";
                    attTable->exportHDF5(outAttTableFile);
                }
                cout << "Finished\n";               
                
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                delete[] datasets;
                delete attTable;
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }            
        }
        else if(this->option == RSGISExeRasterGIS::knnextrapolate)
        {
            cout.precision(12);
            cout << "A command to extrapolate values for field in the attribute table using a KNN approach\n";
            cout << "Table: " << this->attTableFile << endl;
            if(this->attInMemory)
            {
                cout << "Output Table: " << this->outAttTableFile << endl;
            }
            cout << "Training Field: " << this->trainingField << endl;
            cout << "Value Field: " << this->valueField << endl;
            cout << "K = " << this->numkNN << endl;
            cout << "Distance threshold: " << this->distanceThreshold << endl;
            if(distMetric == rsgis_euclidean)
            {
                cout << "Distance Metric: Euclidean\n";
            }
            else if(distMetric == rsgis_manhatten)
            {
                cout << "Distance Metric: Manhatten\n";
            }
            else if(distMetric == rsgis_mahalanobis)
            {
                cout << "Distance Metric: Mahalanobis\n";
            }
            else
            {
                cout << "Unknown distance metric\n";
            }
            cout << "Fields to calculate distance: \n";
            for(vector<string>::iterator iterNames = attributeNames->begin(); iterNames != attributeNames->end(); ++iterNames)
            {
                cout << "\t" << *iterNames << endl;
            }
            
            try
            {
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(this->attInMemory)
                {
                    if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                    }
                    else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Could not identify attribute table file type.");
                    }
                }
                else
                {
                    attTable = RSGISAttributeTableHDF::importFromHDF5(attTableFile, false);
                }
                
                cout << "Perform KNN extrapolation\n";
                RSGISKNNATTableExtrapolation kNNExtrap(valueField);
                kNNExtrap.initKNN(attTable, trainingField, classField, classFieldDefined, classVal, numkNN, distanceThreshold, distMetric, attributeNames);
                kNNExtrap.performKNN();
                
                if(this->attInMemory)
                {
                    cout << "Exporting Attribute Table\n";
                    attTable->exportHDF5(outAttTableFile);
                }
                
                delete attTable;
                delete attributeNames;
            }
            catch(RSGISException &e)
            {
                throw e;
            }
        }
        else if(this->option == RSGISExeRasterGIS::popboolfield)
        {
            cout << "A command to populate a bool field within the attribute table\n";
            cout << "Input Table: " << this->attTableFile << endl;
            if(this->attInMemory)
            {
                cout << "Output Table: " << this->outAttTableFile << endl;
            }
            
            try
            {
                if(statements->size() > 1)
                {
                    cerr << "WARNING: Only the first statment will be used, is this what you intend?\n";
                }
                else if(statements->size() == 0)
                {
                    throw RSGISAttributeTableException("At least 1 statment must be provided.");
                }
                
                cout << "Importing Attribute Table:\n";
                RSGISAttributeTable *attTable = NULL;
                if(this->attInMemory)
                {
                    if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_ascii_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromASCII(attTableFile);
                    }
                    else if(RSGISAttributeTableMem::findFileType(attTableFile) == rsgis_hdf_attft)
                    {
                        attTable = RSGISAttributeTableMem::importFromHDF5(attTableFile);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Could not identify attribute table file type.");
                    }
                }
                else
                {
                    attTable = RSGISAttributeTableHDF::importFromHDF5(attTableFile, false);
                }
                cout << "Create output field\n";
                attTable->createIfStatementsFields(statements, rsgis_bool);
                cout << "Populating statements with indexes\n";
                attTable->populateIfStatementsWithIdxs(statements);
                cout << "Apply if statement\n";
                attTable->applyIfStatementBoolOut(statements->front());
                if(this->attInMemory)
                {
                    cout << "Exporting Attribute Table\n";
                    attTable->exportHDF5(outAttTableFile);
                    //attTable->exportASCII(outAttTableFile);
                }
                cout << "Finished\n";
                delete attTable;
            }
            catch(RSGISException &e)
            {
                throw e;
            }
        }
        else if(this->option == RSGISExeRasterGIS::copyGDALATT)
		{
            cout << "Export an attribute table to a GDAL Dataset with a raster attribute table\n";
            cout << "Input Table: " << this->inputImage << endl;
            cout << "Output Image: " << this->clumpsImage << endl;

            
            GDALAllRegister();
            try
            {
                GDALDataset *inputDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
                if(inputDataset == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                GDALDataset *outRATDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_Update);
                if(outRATDataset == NULL)
                {
                    string message = string("Could not open image ") + this->clumpsImage;
                    throw RSGISImageException(message.c_str());
                }
                
                cout << "Import attribute table\n";
                const GDALRasterAttributeTable *gdalAtt = inputDataset->GetRasterBand(1)->GetDefaultRAT();
                
                cout << "Adding RAT\n";
                outRATDataset->GetRasterBand(1)->SetDefaultRAT(gdalAtt);
                
                GDALClose(inputDataset);
                GDALClose(outRATDataset);
            }
            catch(RSGISException &e)
            {
                throw e;
            }	
		}
		else
		{
			cout << "The option is not recognised: RSGISExeRasterGIS\n";
		}
	}
}

void RSGISExeRasterGIS::printParameters()
{
	if(parsed)
	{
        if(this->option == RSGISExeRasterGIS::createattributetable)
        {
            cout << "A command to create a new attribute table with pixel counts\n";
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Output File: " << this->outputFile << endl;
        }
		else if(this->option == RSGISExeRasterGIS::popattributemean)
		{
            cout << "A command to create a new attribute table and populate it with the image means\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Output File: " << this->outputFile << endl;
            cout << "Attribute Prefix: " << this->attprefix << endl;
        }
        else if(this->option == RSGISExeRasterGIS::exporttable2img)
        {
            cout << "A command to create an image populated with values from an attribute table.\n";
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Attribute Table: " << this->attTableFile << endl;
            cout << "Output Image: " << this->outputFile << endl;
            for(vector<pair<unsigned int, string> >::iterator iterBands = bands->begin(); iterBands != bands->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands).first << " will be attribute " << (*iterBands).second << endl;
            }
        }
        else if(this->option == RSGISExeRasterGIS::boolclasstable)
        {
            cout << "A command to apply boolean statements to the attribute table (i.e., for classification)\n";
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
        }
        else if(this->option == RSGISExeRasterGIS::addfields)
        {
            cout << "A command add new fields to the attribute table\n";
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Adding: ";
            bool first = true;
            for(vector<RSGISAttribute*>::iterator iterAtt = attributes->begin(); iterAtt != attributes->end(); ++iterAtt)
            {
                if(first)
                {
                    cout << (*iterAtt)->name;
                    first = false;
                }
                else
                {
                    cout << ", " << (*iterAtt)->name;
                }
            }
            cout << endl;
        }
        else if(this->option == RSGISExeRasterGIS::findmajority)
        {
            cout << "A command to find the majority category intersecting with each clump\n";
            cout << "Categories Image: " << this->catagoriesImage << endl;
            cout << "Clumps Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Area Field: " << this->areaField << endl;
            cout << "Majority Category Field : " << this->majorityCatField << endl;
            cout << "Majority Ratio Field : " << this->majorityRatioField << endl;
        }
        else if(this->option == RSGISExeRasterGIS::countpolyintersects)
        {
            cout << "A command to populate the attribute table with the number of pixels in the clumps image which intersect with the polygon.\n";
            cout << "Clumps Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Field: " << this->attField << endl;
            cout << "Shapefile: " << this->inputVector << endl;
        }
        else if(this->option == RSGISExeRasterGIS::calcattributes)
        {
            cout << "A command to calculate an attribute from other attributes\n";
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Expression: " << this->mathsExpression << endl;
            cout << "Field: " << this->attField << endl;
        }
        else if(this->option == RSGISExeRasterGIS::popattributestatsinmem)
        {
            cout << "A command to populate an attribute table with statistics from an image.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": ";
                if((*iterBands)->calcMin)
                {
                    cout << "MIN (" << (*iterBands)->minField << ") ";
                }
                
                if((*iterBands)->calcMax)
                {
                    cout << "MAX (" << (*iterBands)->maxField << ") ";
                }
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                if((*iterBands)->calcStdDev)
                {
                    cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                }
                
                if((*iterBands)->calcMedian)
                {
                    cout << "MEDIAN (" << (*iterBands)->medianField << ") ";
                }
                
                cout << endl;
            }
        }
        else if(this->option == RSGISExeRasterGIS::popattributestats)
        {
            cout << "A command to populate an attribute table with statistics from an image in a memory efficient manor.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": ";
                if((*iterBands)->calcMin)
                {
                    cout << "MIN (" << (*iterBands)->minField << ") ";
                }
                
                if((*iterBands)->calcMax)
                {
                    cout << "MAX (" << (*iterBands)->maxField << ") ";
                }
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                if((*iterBands)->calcStdDev)
                {
                    cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                }
                
                cout << endl;
            }
        }
        else if(this->option == RSGISExeRasterGIS::export2gdal)
		{
            cout << "Export an attribute table to a GDAL Dataset with a raster attribute table\n";
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Table: " << this->attTableFile << endl;
            if(outputImagePathProvide)
            {
                cout << "Output File: " << this->outputFile << endl;
                cout << "Format: " << this->imageFormat << endl;
            }
            else
            {
                cout << "Appending attribute table to input image\n";
            }
        }
        else if(this->option == RSGISExeRasterGIS::popattributestatsallbands)
        {
            cout << "A command to populate an attribute table with statistics from an image.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Statistics to be calculated:\n";
            if(imageStats->calcMin)
            {
                cout << "MIN (" << imageStats->minField << ") ";
            }
            if(imageStats->calcMax)
            {
                cout << "MAX (" << imageStats->maxField << ") ";
            }
            if(imageStats->calcMean)
            {
                cout << "MEAN (" << imageStats->meanField << ") ";
            }
            if(imageStats->calcSum)
            {
                cout << "SUM (" << imageStats->sumField << ") ";
            }
            if(imageStats->calcStdDev)
            {
                cout << "STDDEV (" << imageStats->stdDevField << ") ";
            }
            if(imageStats->calcMedian)
            {
                cout << "MEDIAN (" << imageStats->medianField << ") ";
            }
            cout << endl;
        }
        else if(this->option == RSGISExeRasterGIS::findneighbours)
        {
            cout << "A command to find the neighbours of the clumps and save as a neighbours file.\n";
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Output File: " << this->outputFile << endl;
        }
        else if(this->option == RSGISExeRasterGIS::meaneucdist2neighbours)
        {
            cout << "A command to find mean euclidean distance to neighbours\n";
            cout << "Table: " << this->attTableFile << endl;
            cout << "Neighbours: " << this->neighboursFile << endl;
            cout << "Out Table: " << this->outAttTableFile << endl;
            cout << "Out Field: " << this->attField << endl;
            cout << "Fields to calculate distance: \n";
            for(vector<string>::iterator iterNames = attributeNames->begin(); iterNames != attributeNames->end(); ++iterNames)
            {
                cout << "\t" << *iterNames << endl;
            }
        }
        else if(this->option == RSGISExeRasterGIS::calcintrapxleucdist)
        {
            cout << "A command to calculate the euclidean distance to individual pixels within a segment.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Statistics to be calculated:\n";
            if(imageStats->calcMin)
            {
                cout << "MIN (" << imageStats->minField << ") ";
            }
            if(imageStats->calcMax)
            {
                cout << "MAX (" << imageStats->maxField << ") ";
            }
            if(imageStats->calcMean)
            {
                cout << "MEAN (" << imageStats->meanField << ") ";
            }
            if(imageStats->calcSum)
            {
                cout << "SUM (" << imageStats->sumField << ") ";
            }
            if(imageStats->calcStdDev)
            {
                cout << "STDDEV (" << imageStats->stdDevField << ") ";
            }
            if(imageStats->calcMedian)
            {
                cout << "MEDIAN (" << imageStats->medianField << ") ";
            }
            cout << endl;
            for(vector<RSGISBandAttName*>::iterator iterBands = bandAttNames->begin(); iterBands != bandAttNames->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": " << (*iterBands)->attName << endl;
            }
        }
        else if(this->option == RSGISExeRasterGIS::meanlitbandspopattributestats)
        {
            cout << "A command to populate an attribute table with statistics from an image where a segment value either above or below is used.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStatsMeanLit*>::iterator iterBands = bandStatsMeanLit->begin(); iterBands != bandStatsMeanLit->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << "(field = \'" << (*iterBands)->fieldName << "\'): ";
                if((*iterBands)->calcMin)
                {
                    cout << "MIN (" << (*iterBands)->minField << ") ";
                }
                
                if((*iterBands)->calcMax)
                {
                    cout << "MAX (" << (*iterBands)->maxField << ") ";
                }
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                if((*iterBands)->calcStdDev)
                {
                    cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                }
                
                if((*iterBands)->calcMedian)
                {
                    cout << "MEDIAN (" << (*iterBands)->medianField << ") ";
                }
                
                cout << endl;
            }
        }
        else if(this->option == RSGISExeRasterGIS::meanlitpopattributestatsinmem)
        {
            cout << "A command to populate an attribute table with mean-lit statistics from an image.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Mean Lit Field: " << this->meanlitField << endl;
            cout << "Mean Lit Image: " << this->meanlitImage << " band = " << this->meanLitBand;
            if(this->meanLitUseUpper)
            {
                cout << " using upper values.\n";
            }
            else 
            {
                cout << " using lower values.\n";
            }
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": ";
                if((*iterBands)->calcMin)
                {
                    cout << "MIN (" << (*iterBands)->minField << ") ";
                }
                
                if((*iterBands)->calcMax)
                {
                    cout << "MAX (" << (*iterBands)->maxField << ") ";
                }
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                if((*iterBands)->calcStdDev)
                {
                    cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                }
                
                if((*iterBands)->calcMedian)
                {
                    cout << "MEDIAN (" << (*iterBands)->medianField << ") ";
                }
                
                cout << endl;
            }
        }
        else if(this->option == RSGISExeRasterGIS::meanlitpopattributestats)
        {
            cout << "A command to populate an attribute table with mean-lit statistics from an image in a memory efficient manor.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
            cout << "Mean Lit Field: " << this->meanlitField << endl;
            cout << "Mean Lit Image: " << this->meanlitImage << " band = " << this->meanLitBand;
            if(this->meanLitUseUpper)
            {
                cout << " using upper values.\n";
            }
            else 
            {
                cout << " using lower values.\n";
            }
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": ";
                if((*iterBands)->calcMin)
                {
                    cout << "MIN (" << (*iterBands)->minField << ") ";
                }
                
                if((*iterBands)->calcMax)
                {
                    cout << "MAX (" << (*iterBands)->maxField << ") ";
                }
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                if((*iterBands)->calcStdDev)
                {
                    cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                }
                
                cout << endl;
            }
        }
        else if(this->option == RSGISExeRasterGIS::meanlitpopattributestats)
        {
            cout << "A command to export an attribute table to a hdf file.\n";
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
        }
        else if(this->option == RSGISExeRasterGIS::export2hdf)
        {
            cout << "A command to export an attribute table to a hdf file.\n";
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
        }
        else if(this->option == RSGISExeRasterGIS::export2ascii)
        {
            cout << "A command to export an attribute table to a ascii file.\n";
            cout << "Input Table: " << this->attTableFile << endl;
            cout << "Output Table: " << this->outAttTableFile << endl;
        }
        else if(this->option == RSGISExeRasterGIS::popmeansumattributes)
        {
            cout << "A command to populate an attribute table with sum and mean values from an image in a memory efficient and fast manor.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Clump Image: " << this->clumpsImage << endl;
            cout << "Input Table: " << this->attTableFile << endl;
            if(this->attInMemory)
            {
                cout << "Output Table: " << this->outAttTableFile << endl;
            }
            else
            {
                cout << "Processing within existing table\n";
            }
            cout << "Statistics to be calculated:\n";
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                cout << "Band " << (*iterBands)->band << ": ";
                
                if((*iterBands)->calcMean)
                {
                    cout << "MEAN (" << (*iterBands)->meanField << ") ";
                }
                
                if((*iterBands)->calcSum)
                {
                    cout << "SUM (" << (*iterBands)->sumField << ") ";
                }
                
                cout << endl;
            }
        }
        else if(this->option == RSGISExeRasterGIS::printattsummary)
        {
            cout << "A command to print a summary of an attribute table.\n";
            cout << "Input Image: " << this->inputImage << endl;
        }
        else if(this->option == RSGISExeRasterGIS::exportsize)
        {
            cout << "A command to export the attribute table size and output to the text file.\n";
            cout << "Input table: " << this->attTableFile << endl;
            cout << "Output file: " << this->outputFile << endl;
            if(this->outFileType == rsgis_trunk)
            {
                cout << "Truncate the output file\n";
            }
            else if(this->outFileType == rsgis_append)
            {
                cout << "Append to the output file\n";
            }
            else
            {
                throw RSGISException("Output type is not recognised.");
            }
        }
        else if(this->option == RSGISExeRasterGIS::knnextrapolate)
        {
            cout << "A command to extrapolate values for field in the attribute table using a KNN approach\n";
            cout << "Table: " << this->attTableFile << endl;
            if(this->attInMemory)
            {
                cout << "Output Table: " << this->outAttTableFile << endl;
            }
            cout << "Training Field: " << this->trainingField << endl;
            cout << "Value Field: " << this->valueField << endl;
            cout << "K = " << this->numkNN << endl;
            cout << "Distance threshold: " << this->distanceThreshold << endl;
            if(distMetric == rsgis_euclidean)
            {
                cout << "Distance Metric: Euclidean\n";
            }
            else if(distMetric == rsgis_manhatten)
            {
                cout << "Distance Metric: Manhatten\n";
            }
            else if(distMetric == rsgis_mahalanobis)
            {
                cout << "Distance Metric: Mahalanobis\n";
            }
            else
            {
                cout << "Unknown distance metric\n";
            }
            cout << "Fields to calculate distance: \n";
            for(vector<string>::iterator iterNames = attributeNames->begin(); iterNames != attributeNames->end(); ++iterNames)
            {
                cout << "\t" << *iterNames << endl;
            }
        }
        else if(this->option == RSGISExeRasterGIS::popboolfield)
        {
            cout << "A command to populate a bool field within the attribute table\n";
            cout << "Input Table: " << this->attTableFile << endl;
            if(this->attInMemory)
            {
                cout << "Output Table: " << this->outAttTableFile << endl;
            }
        }
		else
		{
			cout << "The option is not recognised\n";
		}
	}
	else
	{
		cout << "The parameters have yet to be parsed\n";
	}
}


void RSGISExeRasterGIS::help()
{
	cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
    cout << "<!-- A command to create a new attribute table with pixel counts -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"createattributetable\" clumps=\"clumps.env\" output=\"output.att\" />" << endl;
    cout << "<!-- A command to populate an attribute table with statistics from an image -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"popattributestats\" input=\"image.env\" clumps=\"clumps.env\" table=\"input.att\" tableout=\"output.att\" >" << endl;
    cout << "    <rsgis:band band=\"int\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "    <rsgis:band band=\"int\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "    <rsgis:band band=\"int\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "    <rsgis:band band=\"int\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to populate an attribute table with statistics from an image where a segment value either above or below is used -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"meanlitbandspopattributestats\" input=\"image.env\" clumps=\"clumps.env\" table=\"input.att\" tableout=\"output.att\" >" << endl;
    cout << "    <rsgis:band band=\"int\" field=\"string\" type=\"below | above\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "    <rsgis:band band=\"int\" field=\"string\" type=\"below | above\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "    <rsgis:band band=\"int\" field=\"string\" type=\"below | above\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "    <rsgis:band band=\"int\" field=\"string\" type=\"below | above\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to populate an attribute table with statistics from an image where a segment value either above or below is used -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"meanlitpopattributestats\" input=\"image.env\" clumps=\"clumps.env\" table=\"input.att\" tableout=\"output.att\" field=\"meanlit\" meanlitimage=\"image.env\" meanlitband=\"int\" meanlittype=\"below | above\" >" << endl;
    cout << "    <rsgis:band band=\"int\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "    <rsgis:band band=\"int\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "    <rsgis:band band=\"int\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "    <rsgis:band band=\"int\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to populate attribute table with mean image values -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"popattributemean\" input=\"image.env\" clumps=\"clumps.env\" output=\"output.att\" prefix=\"string\" />" << endl;
    cout << "<!-- A command to export table attributes to an image -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"exporttable2img\" clumps=\"clumps.env\" table=\"table.att\" output=\"image.env\" format=\"GDAL Format\" >" << endl;
    cout << "    <rsgis:band attribute=\"name\" />" << endl;
    cout << "    <rsgis:band attribute=\"name\" />" << endl;
    cout << "    <rsgis:band attribute=\"name\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to classify the attribute table using boolean logic  -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"boolclasstable\" table=\"input.att\" tableout=\"output.att\" >" << endl;
    cout << "    <rsgis:if field=\"string\" value=\"number\">" << endl;
    cout << "        <rsgis:exp type=\"and\"></rsgis:exp>" << endl;
    cout << "        <rsgis:exp type=\"or\"></rsgis:exp>" << endl;
    cout << "        <rsgis:exp type=\"not\"></rsgis:exp>" << endl;
    cout << "        <rsgis:exp type=\"gt\" field1=\"string\" field2=\"string\" />" << endl;
    cout << "        <rsgis:exp type=\"lt\" field1=\"string\" field2=\"string\" />" << endl;
    cout << "        <rsgis:exp type=\"gteq\" field1=\"string\" field2=\"string\" />" << endl;
    cout << "        <rsgis:exp type=\"lteq\" field1=\"string\" field2=\"string\" />" << endl;
    cout << "        <rsgis:exp type=\"eq\" field1=\"string\" field2=\"string\" />" << endl;
    cout << "        <rsgis:exp type=\"neq\" field1=\"string\" field2=\"string\" />" << endl;
    cout << "        <rsgis:exp type=\"gtconst\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"ltconst\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"gteqconst\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"lteqconst\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"eqconst\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"neqconst\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"constgt\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"constlt\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"constgteq\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"constlteq\" field=\"string\" value=\"float\" />" << endl;
    cout << "    </rsgis:if>" << endl;
    cout << "    <rsgis:else [field=\"string\" value=\"number\"] />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to add attributes to an attribute fields to the table -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"addfields\" table=\"input.att\" tableout=\"output.att\" >" << endl;
    cout << "    <rsgis:attribute name=\"string\" datatype=\"rsgis_bool | rsgis_int | rsgis_float | rsgis_string\" />" << endl;
    cout << "    <rsgis:attribute name=\"string\" datatype=\"rsgis_bool | rsgis_int | rsgis_float | rsgis_string\" />" << endl;
    cout << "    <rsgis:attribute name=\"string\" datatype=\"rsgis_bool | rsgis_int | rsgis_float | rsgis_string\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to region grow regions of the dataset using threshold -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"regiongrowthres\" clumps=\"clumps.env\" table=\"input.att\" tableout=\"output.att\" class=\"attribute_name\" classval=\"int\" >" << endl;
    cout << "    <rsgis:if field=\"string\" value=\"number\">" << endl;
    cout << "        <rsgis:exp type=\"and\"></rsgis:exp>" << endl;
    cout << "        <rsgis:exp type=\"or\"></rsgis:exp>" << endl;
    cout << "        <rsgis:exp type=\"not\"></rsgis:exp>" << endl;
    cout << "        <rsgis:exp type=\"gt\" field1=\"string\" field2=\"string\" />" << endl;
    cout << "        <rsgis:exp type=\"lt\" field1=\"string\" field2=\"string\" />" << endl;
    cout << "        <rsgis:exp type=\"gteq\" field1=\"string\" field2=\"string\" />" << endl;
    cout << "        <rsgis:exp type=\"lteq\" field1=\"string\" field2=\"string\" />" << endl;
    cout << "        <rsgis:exp type=\"eq\" field1=\"string\" field2=\"string\" />" << endl;
    cout << "        <rsgis:exp type=\"neq\" field1=\"string\" field2=\"string\" />" << endl;
    cout << "        <rsgis:exp type=\"gtconst\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"ltconst\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"gteqconst\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"lteqconst\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"eqconst\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"neqconst\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"constgt\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"constlt\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"constgteq\" field=\"string\" value=\"float\" />" << endl;
    cout << "        <rsgis:exp type=\"constlteq\" field=\"string\" value=\"float\" />" << endl;
    cout << "    </rsgis:if>" << endl;
    cout << "    <rsgis:else [field=\"string\" value=\"number\"] />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to find the majority category intersecting with each clump -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"findmajority\" catagories=\"image.env\" clumps=\"clumps.env\" table=\"input.att\" tableout=\"output.att\" pxlareafield=\"attribute_name\" majoritycatfield=\"attribute_name\" majorityratiofield=\"attribute_name\" />" << endl;
    cout << "<!-- A command to populate the attribute table with the number of pixels in the clumps image which intersect with the polygon -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"countpolyintersects\" vector=\"polys.shp\" clumps=\"clumps.env\" table=\"input.att\" tableout=\"output.att\" field=\"attribute_name\" method=\"polyContainsPixel | polyContainsPixelCenter | polyOverlapsPixel | polyOverlapsOrContainsPixel | pixelContainsPoly | pixelContainsPolyCenter | adaptive | envelope\" />" << endl;
    cout << "<!-- A command to calculate an attribute from other attributes -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"calcattributes\" table=\"input.att\" tableout=\"output.att\" field=\"attribute_name\" datatype=\"rsgis_int | rsgis_float\" [expression=\"MU Parser Expression\" | expressionfile=\"file.txt\"] >" << endl;
    cout << "    <rsgis:attribute variable=\"string\" name=\"string\" />" << endl;
    cout << "    <rsgis:attribute variable=\"string\" name=\"string\" />" << endl;
    cout << "    <rsgis:attribute variable=\"string\" name=\"string\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command export an attribute table to a GDAL Dataset with a raster attribute table -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"export2gdal\" clumps=\"clumps.env\" table=\"input.att\" output=\"output.img\" format=\"GDAL Format\" />" << endl;
    cout << "<!-- A command to populate an attribute table with statistics from an image across all image bands-->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"popattributestatsallbands\" input=\"image.env\" clumps=\"clumps.env\" table=\"input.att\" tableout=\"output.att\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] />" << endl;
    cout << "<!-- A command to create summary statistics for a field in the attribute table -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"summarisefield\" table=\"input.att\" output=\"output.txt\" field=\"attribute_name\" [min=\"yes\"] [max=\"yes\"] [mean=\"yes\"] [sum=\"yes\"] [stddev=\"yes\"] [median=\"yes\"] />" << endl;
    cout << "<!-- A command to find the neighbours of the clumps and save as a neighbours file. -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"findneighbours\" clumps=\"clumps.env\" output=\"neighbours.cnf\" />" << endl;
    cout << "<!-- A command to find mean euclidean distance to neighbours -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"meaneucdist2neighbours\" table=\"input.att\" neighbours=\"neighbours.cnf\" tableout=\"output.att\" meanfield=\"attribute_name\" maxfield=\"attribute_name\" minfield=\"attribute_name\" >" << endl;
    cout << "    <rsgis:attribute name=\"string\" />" << endl;
    cout << "    <rsgis:attribute name=\"string\" />" << endl;
    cout << "    <rsgis:attribute name=\"string\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to calculate the euclidean distance to individual pixels within a segment -->" << endl;
    cout << "<rsgis:command algor=\"rastergis\" option=\"calcintrapxleucdist\" input=\"image.env\" clumps=\"clumps.env\" table=\"input.att\" tableout=\"output.att\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] [median=\"field\"] >" << endl;
    cout << "    <rsgis:band band=\"int\" name=\"string\" />" << endl;
    cout << "    <rsgis:band band=\"int\" name=\"string\" />" << endl;
    cout << "    <rsgis:band band=\"int\" name=\"string\" />" << endl;
    cout << "    <rsgis:band band=\"int\" name=\"string\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "</rsgis:commands>" << endl;
}

string RSGISExeRasterGIS::getDescription()
{
	return "An interface to the available raster GIS functionality.";
}

string RSGISExeRasterGIS::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeRasterGIS::~RSGISExeRasterGIS()
{
	
}

}



