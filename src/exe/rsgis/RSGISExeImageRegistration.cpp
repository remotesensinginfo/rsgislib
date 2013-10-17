/*
 *  RSGISExeImageRegistration.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/09/2010.
 *  Copyright 2010 RSGISLib.
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

#include "RSGISExeImageRegistration.h"

namespace rsgisexe{

RSGISExeImageRegistration::RSGISExeImageRegistration() : RSGISAlgorithmParameters()
{
	this->algorithm = "registration";
}

    rsgis::RSGISAlgorithmParameters* RSGISExeImageRegistration::getInstance()
{
	return new RSGISExeImageRegistration();
}

void RSGISExeImageRegistration::retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException)
{
	rsgis::math::RSGISMathsUtils mathUtils;
	XMLCh *algorName = xercesc::XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = xercesc::XMLString::transcode("algor");
	XMLCh *optionXMLStr = xercesc::XMLString::transcode("option");
	XMLCh *optionBasic = xercesc::XMLString::transcode("basic");
	XMLCh *optionSingleLayer = xercesc::XMLString::transcode("singlelayer");
	XMLCh *optionTriangularWarp = xercesc::XMLString::transcode("triangularwarp");
	XMLCh *optionNNWarp = xercesc::XMLString::transcode("nnwarp");
    XMLCh *optionPolyWarp = xercesc::XMLString::transcode("polywarp");
    XMLCh *optionPxlShift = xercesc::XMLString::transcode("pxlshift");
	XMLCh *optionGCP2GDAL = xercesc::XMLString::transcode("gcp2gdal");
    
	try
	{
		const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
		if(!xercesc::XMLString::equals(algorName, algorNameEle))
		{
			throw rsgis::RSGISXMLArgumentsException("The algorithm name is incorrect.");
		}
		
		
        // Set output image fomat (defaults to KEA)
        this->imageFormat = "KEA";
        XMLCh *formatXMLStr = xercesc::XMLString::transcode("format");
        if(argElement->hasAttribute(formatXMLStr))
        {
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(formatXMLStr));
            this->imageFormat = std::string(charValue);
            xercesc::XMLString::release(&charValue);
        }
        xercesc::XMLString::release(&formatXMLStr);

        // Set output data type (defaults to 32 bit float)
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
        
		const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
		if(xercesc::XMLString::equals(optionBasic, optionXML))
		{
			this->option = RSGISExeImageRegistration::basic;
			
			XMLCh *referenceXMLStr = xercesc::XMLString::transcode("reference");
			if(argElement->hasAttribute(referenceXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(referenceXMLStr));
				this->inputReferenceImage = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'reference\' attribute was provided.");
			}
			xercesc::XMLString::release(&referenceXMLStr);
			
			XMLCh *floatingXMLStr = xercesc::XMLString::transcode("floating");
			if(argElement->hasAttribute(floatingXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(floatingXMLStr));
				this->inputFloatingmage = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'floating\' attribute was provided.");
			}
			xercesc::XMLString::release(&floatingXMLStr);
			
			
			XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
			if(argElement->hasAttribute(outputXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputGCPFile = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			xercesc::XMLString::release(&outputXMLStr);
			
			
			XMLCh *outputTypeXMLStr = xercesc::XMLString::transcode("outputType");
			if(argElement->hasAttribute(outputTypeXMLStr))
			{
				XMLCh *envImg2ImgStr = xercesc::XMLString::transcode("envi_img2img");
				XMLCh *enviImg2MapStr = xercesc::XMLString::transcode("envi_img2map");
				XMLCh *rsgisImg2MapStr = xercesc::XMLString::transcode("rsgis_img2map");
                XMLCh *rsgisMapOffsStr = xercesc::XMLString::transcode("rsgis_mapoffs");
				
				const XMLCh *outTypeValue = argElement->getAttribute(outputTypeXMLStr);
				
				if(xercesc::XMLString::equals(outTypeValue, envImg2ImgStr))
				{
					this->outputType = RSGISExeImageRegistration::envi_img2img;
				}
				else if(xercesc::XMLString::equals(outTypeValue, enviImg2MapStr))
				{
					this->outputType = RSGISExeImageRegistration::envi_img2map;
				}
				else if(xercesc::XMLString::equals(outTypeValue, rsgisImg2MapStr))
				{
					this->outputType = RSGISExeImageRegistration::rsgis_img2map;
				}
                else if(xercesc::XMLString::equals(outTypeValue, rsgisMapOffsStr))
				{
					this->outputType = RSGISExeImageRegistration::rsgis_mapoffs;
				}
				else
				{
					this->outputType = RSGISExeImageRegistration::undefinedOutput;
					throw rsgis::RSGISXMLArgumentsException("The \'outputType\' attribute needs to have one of the following values:\n\tenvi_img2img | envi_img2map | rsgis_img2map | rsgis_mapoffs.");
				}
				xercesc::XMLString::release(&envImg2ImgStr);
				xercesc::XMLString::release(&enviImg2MapStr);
				xercesc::XMLString::release(&rsgisImg2MapStr);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'outputType\' attribute was provided.");
			}
			xercesc::XMLString::release(&outputTypeXMLStr);
			
			
			XMLCh *metricXMLStr = xercesc::XMLString::transcode("metric");
			if(argElement->hasAttribute(metricXMLStr))
			{
				XMLCh *euclideanStr = xercesc::XMLString::transcode("euclidean");
				XMLCh *sqdiffStr = xercesc::XMLString::transcode("sqdiff");
				XMLCh *manhattenStr = xercesc::XMLString::transcode("manhatten");
				XMLCh *correlationStr = xercesc::XMLString::transcode("correlation");
				
				const XMLCh *metricValue = argElement->getAttribute(metricXMLStr);
				
				if(xercesc::XMLString::equals(metricValue, euclideanStr))
				{
					this->metricType = RSGISExeImageRegistration::euclidean;
				}
				else if(xercesc::XMLString::equals(metricValue, sqdiffStr))
				{
					this->metricType = RSGISExeImageRegistration::sqdiff;
				}
				else if(xercesc::XMLString::equals(metricValue, manhattenStr))
				{
					this->metricType = RSGISExeImageRegistration::manhatten;
				}
				else if(xercesc::XMLString::equals(metricValue, correlationStr))
				{
					this->metricType = RSGISExeImageRegistration::correlation;
				}
				else
				{
					this->metricType = RSGISExeImageRegistration::undefinedMetric;
					throw rsgis::RSGISXMLArgumentsException("The \'metric\' attribute needs to have one of the following values euclidean | sqdiff | manhatten | correlation.");
				}
				xercesc::XMLString::release(&euclideanStr);
				xercesc::XMLString::release(&sqdiffStr);
				xercesc::XMLString::release(&manhattenStr);
				xercesc::XMLString::release(&correlationStr);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'metric\' attribute was provided.");
			}
			xercesc::XMLString::release(&metricXMLStr);
			
			
			XMLCh *pixelGapXMLStr = xercesc::XMLString::transcode("pixelgap");
			if(argElement->hasAttribute(pixelGapXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(pixelGapXMLStr));
				this->gcpGap = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'pixelgap\' attribute was provided.");
			}
			xercesc::XMLString::release(&pixelGapXMLStr);
			
			XMLCh *windowXMLStr = xercesc::XMLString::transcode("window");
			if(argElement->hasAttribute(windowXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(windowXMLStr));
				this->windowSize = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'window\' attribute was provided.");
			}
			xercesc::XMLString::release(&windowXMLStr);
			
			XMLCh *searchXMLStr = xercesc::XMLString::transcode("search");
			if(argElement->hasAttribute(searchXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(searchXMLStr));
				this->searchArea = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'search\' attribute was provided.");
			}
			xercesc::XMLString::release(&searchXMLStr);
			
			
			XMLCh *thresholdXMLStr = xercesc::XMLString::transcode("threshold");
			if(argElement->hasAttribute(thresholdXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
				this->metricThreshold = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
			}
			xercesc::XMLString::release(&thresholdXMLStr);
			
			XMLCh *stddevRefXMLStr = xercesc::XMLString::transcode("stddevRef");
			if(argElement->hasAttribute(stddevRefXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(stddevRefXMLStr));
				this->stdDevRefThreshold = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'stddevRef\' attribute was provided.");
			}
			xercesc::XMLString::release(&stddevRefXMLStr);
			
			XMLCh *stddevFloatXMLStr = xercesc::XMLString::transcode("stddevFloat");
			if(argElement->hasAttribute(stddevFloatXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(stddevFloatXMLStr));
				this->stdDevFloatThreshold = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'stddevFloat\' attribute was provided.");
			}
			xercesc::XMLString::release(&stddevFloatXMLStr);
			
			XMLCh *subPixelResXMLStr = xercesc::XMLString::transcode("subpixelresolution");
			if(argElement->hasAttribute(subPixelResXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(subPixelResXMLStr));
				this->subPixelResolution = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'subpixelresolution\' attribute was provided.");
			}
			xercesc::XMLString::release(&subPixelResXMLStr);
		}
		else if(xercesc::XMLString::equals(optionSingleLayer, optionXML))
		{
			this->option = RSGISExeImageRegistration::singlelayer;
			
			XMLCh *referenceXMLStr = xercesc::XMLString::transcode("reference");
			if(argElement->hasAttribute(referenceXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(referenceXMLStr));
				this->inputReferenceImage = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'reference\' attribute was provided.");
			}
			xercesc::XMLString::release(&referenceXMLStr);
			
			XMLCh *floatingXMLStr = xercesc::XMLString::transcode("floating");
			if(argElement->hasAttribute(floatingXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(floatingXMLStr));
				this->inputFloatingmage = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'floating\' attribute was provided.");
			}
			xercesc::XMLString::release(&floatingXMLStr);
			
			
			XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
			if(argElement->hasAttribute(outputXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputGCPFile = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			xercesc::XMLString::release(&outputXMLStr);
			
			
			XMLCh *outputTypeXMLStr = xercesc::XMLString::transcode("outputType");
			if(argElement->hasAttribute(outputTypeXMLStr))
			{
				XMLCh *envImg2ImgStr = xercesc::XMLString::transcode("envi_img2img");
				XMLCh *enviImg2MapStr = xercesc::XMLString::transcode("envi_img2map");
				XMLCh *rsgisImg2MapStr = xercesc::XMLString::transcode("rsgis_img2map");
                XMLCh *rsgisMapOffsStr = xercesc::XMLString::transcode("rsgis_mapoffs");
				
				const XMLCh *outTypeValue = argElement->getAttribute(outputTypeXMLStr);
				
				if(xercesc::XMLString::equals(outTypeValue, envImg2ImgStr))
				{
					this->outputType = RSGISExeImageRegistration::envi_img2img;
				}
				else if(xercesc::XMLString::equals(outTypeValue, enviImg2MapStr))
				{
					this->outputType = RSGISExeImageRegistration::envi_img2map;
				}
				else if(xercesc::XMLString::equals(outTypeValue, rsgisImg2MapStr))
				{
					this->outputType = RSGISExeImageRegistration::rsgis_img2map;
				}
                else if(xercesc::XMLString::equals(outTypeValue, rsgisMapOffsStr))
				{
					this->outputType = RSGISExeImageRegistration::rsgis_mapoffs;
				}
				else
				{
					this->outputType = RSGISExeImageRegistration::undefinedOutput;
					throw rsgis::RSGISXMLArgumentsException("The \'outputType\' attribute needs to have one of the following values envi_img2img | envi_img2map | rsgis_img2map | rsgis_mapoffs.");
				}
				xercesc::XMLString::release(&envImg2ImgStr);
				xercesc::XMLString::release(&enviImg2MapStr);
				xercesc::XMLString::release(&rsgisImg2MapStr);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'outputType\' attribute was provided.");
			}
			xercesc::XMLString::release(&outputTypeXMLStr);
			
			
			XMLCh *metricXMLStr = xercesc::XMLString::transcode("metric");
			if(argElement->hasAttribute(metricXMLStr))
			{
				XMLCh *euclideanStr = xercesc::XMLString::transcode("euclidean");
				XMLCh *sqdiffStr = xercesc::XMLString::transcode("sqdiff");
				XMLCh *manhattenStr = xercesc::XMLString::transcode("manhatten");
				XMLCh *correlationStr = xercesc::XMLString::transcode("correlation");
				
				const XMLCh *metricValue = argElement->getAttribute(metricXMLStr);
				
				if(xercesc::XMLString::equals(metricValue, euclideanStr))
				{
					this->metricType = RSGISExeImageRegistration::euclidean;
				}
				else if(xercesc::XMLString::equals(metricValue, sqdiffStr))
				{
					this->metricType = RSGISExeImageRegistration::sqdiff;
				}
				else if(xercesc::XMLString::equals(metricValue, manhattenStr))
				{
					this->metricType = RSGISExeImageRegistration::manhatten;
				}
				else if(xercesc::XMLString::equals(metricValue, correlationStr))
				{
					this->metricType = RSGISExeImageRegistration::correlation;
				}
				else
				{
					this->metricType = RSGISExeImageRegistration::undefinedMetric;
					throw rsgis::RSGISXMLArgumentsException("The \'metric\' attribute needs to have one of the following values euclidean | sqdiff | manhatten | correlation.");
				}
				xercesc::XMLString::release(&euclideanStr);
				xercesc::XMLString::release(&sqdiffStr);
				xercesc::XMLString::release(&manhattenStr);
				xercesc::XMLString::release(&correlationStr);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'metric\' attribute was provided.");
			}
			xercesc::XMLString::release(&metricXMLStr);
			
			
			XMLCh *pixelGapXMLStr = xercesc::XMLString::transcode("pixelgap");
			if(argElement->hasAttribute(pixelGapXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(pixelGapXMLStr));
				this->gcpGap = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'pixelgap\' attribute was provided.");
			}
			xercesc::XMLString::release(&pixelGapXMLStr);
			
			XMLCh *windowXMLStr = xercesc::XMLString::transcode("window");
			if(argElement->hasAttribute(windowXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(windowXMLStr));
				this->windowSize = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'window\' attribute was provided.");
			}
			xercesc::XMLString::release(&windowXMLStr);
			
			XMLCh *searchXMLStr = xercesc::XMLString::transcode("search");
			if(argElement->hasAttribute(searchXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(searchXMLStr));
				this->searchArea = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'search\' attribute was provided.");
			}
			xercesc::XMLString::release(&searchXMLStr);
			
			
			XMLCh *thresholdXMLStr = xercesc::XMLString::transcode("threshold");
			if(argElement->hasAttribute(thresholdXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
				this->metricThreshold = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
			}
			xercesc::XMLString::release(&thresholdXMLStr);
			
			XMLCh *stddevRefXMLStr = xercesc::XMLString::transcode("stddevRef");
			if(argElement->hasAttribute(stddevRefXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(stddevRefXMLStr));
				this->stdDevRefThreshold = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'stddevRef\' attribute was provided.");
			}
			xercesc::XMLString::release(&stddevRefXMLStr);
			
			XMLCh *stddevFloatXMLStr = xercesc::XMLString::transcode("stddevFloat");
			if(argElement->hasAttribute(stddevFloatXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(stddevFloatXMLStr));
				this->stdDevFloatThreshold = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'stddevFloat\' attribute was provided.");
			}
			xercesc::XMLString::release(&stddevFloatXMLStr);
			
			XMLCh *subPixelResXMLStr = xercesc::XMLString::transcode("subpixelresolution");
			if(argElement->hasAttribute(subPixelResXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(subPixelResXMLStr));
				this->subPixelResolution = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'subpixelresolution\' attribute was provided.");
			}
			xercesc::XMLString::release(&subPixelResXMLStr);
			
			
			XMLCh *distanceThresholdXMLStr = xercesc::XMLString::transcode("distanceThreshold");
			if(argElement->hasAttribute(distanceThresholdXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(distanceThresholdXMLStr));
				this->distanceThreshold = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'distanceThreshold\' attribute was provided.");
			}
			xercesc::XMLString::release(&distanceThresholdXMLStr);
			
			
			XMLCh *maxIterationsXMLStr = xercesc::XMLString::transcode("maxiterations");
			if(argElement->hasAttribute(maxIterationsXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
				this->maxNumIterations = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'maxiterations\' attribute was provided.");
			}
			xercesc::XMLString::release(&maxIterationsXMLStr);
			
			XMLCh *moveChangeThresholdXMLStr = xercesc::XMLString::transcode("movementThreshold");
			if(argElement->hasAttribute(moveChangeThresholdXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(moveChangeThresholdXMLStr));
				this->moveChangeThreshold = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'movementThreshold\' attribute was provided.");
			}
			xercesc::XMLString::release(&moveChangeThresholdXMLStr);

			
			XMLCh *pSmoothnessXMLStr = xercesc::XMLString::transcode("pSmoothness");
			if(argElement->hasAttribute(pSmoothnessXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(pSmoothnessXMLStr));
				this->pSmoothness = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'pSmoothness\' attribute was provided.");
			}
			xercesc::XMLString::release(&pSmoothnessXMLStr);
			
		}
		else if(xercesc::XMLString::equals(optionTriangularWarp, optionXML))
		{
			this->option = triangularwarp;
			
			XMLCh *gcpsXMLStr = xercesc::XMLString::transcode("gcps");
			if(argElement->hasAttribute(gcpsXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(gcpsXMLStr));
				this->inputGCPs = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'gcps\' attribute was provided.");
			}
			xercesc::XMLString::release(&gcpsXMLStr);

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
			
			XMLCh *outputImageXMLStr = xercesc::XMLString::transcode("output");
			if(argElement->hasAttribute(outputImageXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputImageXMLStr));
				this->outputImage = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			xercesc::XMLString::release(&outputImageXMLStr);

			XMLCh *projectionXMLStr = xercesc::XMLString::transcode("projection");
			if(argElement->hasAttribute(projectionXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(projectionXMLStr));
				this->projFile = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'projection\' attribute was provided.");
			}
			xercesc::XMLString::release(&projectionXMLStr);
			
			XMLCh *resolutionXMLStr = xercesc::XMLString::transcode("resolution");
			if(argElement->hasAttribute(resolutionXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(resolutionXMLStr));
				this->resolution = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'resolution\' attribute was provided.");
			}
			xercesc::XMLString::release(&resolutionXMLStr);
			         
            XMLCh *outTransformXMLStr = xercesc::XMLString::transcode("transform");
			if(argElement->hasAttribute(outTransformXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outTransformXMLStr));
				std::string outTransform = std::string(charValue);
                if(outTransform == "yes")
                {
                    this->genTransformImage = true;
                }
                else
                {
                    this->genTransformImage = false;
                }
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				this->genTransformImage = false;
			}
			xercesc::XMLString::release(&outTransformXMLStr);
			
		}
		else if((xercesc::XMLString::equals(optionNNWarp, optionXML)) | (xercesc::XMLString::equals(optionPolyWarp, optionXML)))
		{
			if(xercesc::XMLString::equals(optionNNWarp, optionXML)){this->option = nnwarp;}
            else{this->option = polywarp;}
			
			XMLCh *gcpsXMLStr = xercesc::XMLString::transcode("gcps");
			if(argElement->hasAttribute(gcpsXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(gcpsXMLStr));
				this->inputGCPs = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'gcps\' attribute was provided.");
			}
			xercesc::XMLString::release(&gcpsXMLStr);
			
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
			
			XMLCh *outputImageXMLStr = xercesc::XMLString::transcode("output");
			if(argElement->hasAttribute(outputImageXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputImageXMLStr));
				this->outputImage = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			xercesc::XMLString::release(&outputImageXMLStr);
			
			XMLCh *projectionXMLStr = xercesc::XMLString::transcode("projection");
			if(argElement->hasAttribute(projectionXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(projectionXMLStr));
				this->projFile = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'projection\' attribute was provided.");
			}
			xercesc::XMLString::release(&projectionXMLStr);
			
			XMLCh *resolutionXMLStr = xercesc::XMLString::transcode("resolution");
			if(argElement->hasAttribute(resolutionXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(resolutionXMLStr));
				this->resolution = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'resolution\' attribute was provided.");
			}
			xercesc::XMLString::release(&resolutionXMLStr);
            
            if(this->option == polywarp) // Get polynominal order for polynominal warp
            {
                XMLCh *polyOrderStr = xercesc::XMLString::transcode("polyOrder"); // Polynomial Order
                if(argElement->hasAttribute(polyOrderStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(polyOrderStr));
                    this->polyOrder = mathUtils.strtoint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No value provided for polynomial order");
                }
                xercesc::XMLString::release(&polyOrderStr);
            }
            
            XMLCh *outTransformXMLStr = xercesc::XMLString::transcode("transform");
			if(argElement->hasAttribute(outTransformXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outTransformXMLStr));
				std::string outTransform = std::string(charValue);
                if(outTransform == "yes")
                {
                    this->genTransformImage = true;
                }
                else
                {
                    this->genTransformImage = false;
                }
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				this->genTransformImage = false;
			}
			xercesc::XMLString::release(&outTransformXMLStr);
			
		}
        else if(xercesc::XMLString::equals(optionPxlShift, optionXML))
		{
			this->option = RSGISExeImageRegistration::pxlshift;
			
			XMLCh *referenceXMLStr = xercesc::XMLString::transcode("reference");
			if(argElement->hasAttribute(referenceXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(referenceXMLStr));
				this->inputReferenceImage = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'reference\' attribute was provided.");
			}
			xercesc::XMLString::release(&referenceXMLStr);
			
			XMLCh *floatingXMLStr = xercesc::XMLString::transcode("floating");
			if(argElement->hasAttribute(floatingXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(floatingXMLStr));
				this->inputFloatingmage = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'floating\' attribute was provided.");
			}
			xercesc::XMLString::release(&floatingXMLStr);
			
			
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
			
			XMLCh *metricXMLStr = xercesc::XMLString::transcode("metric");
			if(argElement->hasAttribute(metricXMLStr))
			{
				XMLCh *euclideanStr = xercesc::XMLString::transcode("euclidean");
				XMLCh *sqdiffStr = xercesc::XMLString::transcode("sqdiff");
				XMLCh *manhattenStr = xercesc::XMLString::transcode("manhatten");
				XMLCh *correlationStr = xercesc::XMLString::transcode("correlation");
				
				const XMLCh *metricValue = argElement->getAttribute(metricXMLStr);
				
				if(xercesc::XMLString::equals(metricValue, euclideanStr))
				{
					this->metricType = RSGISExeImageRegistration::euclidean;
				}
				else if(xercesc::XMLString::equals(metricValue, sqdiffStr))
				{
					this->metricType = RSGISExeImageRegistration::sqdiff;
				}
				else if(xercesc::XMLString::equals(metricValue, manhattenStr))
				{
					this->metricType = RSGISExeImageRegistration::manhatten;
				}
				else if(xercesc::XMLString::equals(metricValue, correlationStr))
				{
					this->metricType = RSGISExeImageRegistration::correlation;
				}
				else
				{
					this->metricType = RSGISExeImageRegistration::undefinedMetric;
					throw rsgis::RSGISXMLArgumentsException("The \'metric\' attribute needs to have one of the following values euclidean | sqdiff | manhatten | correlation.");
				}
				xercesc::XMLString::release(&euclideanStr);
				xercesc::XMLString::release(&sqdiffStr);
				xercesc::XMLString::release(&manhattenStr);
				xercesc::XMLString::release(&correlationStr);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'metric\' attribute was provided.");
			}
			xercesc::XMLString::release(&metricXMLStr);
			
			XMLCh *windowXMLStr = xercesc::XMLString::transcode("window");
			if(argElement->hasAttribute(windowXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(windowXMLStr));
				this->windowSize = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'window\' attribute was provided.");
			}
			xercesc::XMLString::release(&windowXMLStr);
			
			XMLCh *searchXMLStr = xercesc::XMLString::transcode("search");
			if(argElement->hasAttribute(searchXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(searchXMLStr));
				this->searchArea = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'search\' attribute was provided.");
			}
			xercesc::XMLString::release(&searchXMLStr);
			
			XMLCh *subPixelResXMLStr = xercesc::XMLString::transcode("subpixelresolution");
			if(argElement->hasAttribute(subPixelResXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(subPixelResXMLStr));
				this->subPixelResolution = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'subpixelresolution\' attribute was provided.");
			}
			xercesc::XMLString::release(&subPixelResXMLStr);
		}
        else if(xercesc::XMLString::equals(optionGCP2GDAL, optionXML))
		{
            this->option = gcp2gdal;
			
			XMLCh *gcpsXMLStr = xercesc::XMLString::transcode("gcps");
			if(argElement->hasAttribute(gcpsXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(gcpsXMLStr));
				this->inputGCPs = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'gcps\' attribute was provided.");
			}
			xercesc::XMLString::release(&gcpsXMLStr);
			
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
			
			XMLCh *outputImageXMLStr = xercesc::XMLString::transcode("output");
			if(argElement->hasAttribute(outputImageXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputImageXMLStr));
				this->outputImage = std::string(charValue);
				xercesc::XMLString::release(&charValue);
			}
			else
			{
                std::cout << "No \'output\' attribute was provided, adding GCPs to input image" << std::endl;
                this->outputImage = "";
			}
			xercesc::XMLString::release(&outputImageXMLStr);
			
		}
		else
		{
			std::string message = "RSGISExeImageRegistration did not recognise option " + std::string(xercesc::XMLString::transcode(optionXML));
			throw rsgis::RSGISXMLArgumentsException(message);
		}

	}
	catch(rsgis::RSGISXMLArgumentsException &e)
	{
		throw e;
	}
	
	xercesc::XMLString::release(&algorName);
	xercesc::XMLString::release(&algorXMLStr);
	xercesc::XMLString::release(&optionXMLStr);
	xercesc::XMLString::release(&optionBasic);
	xercesc::XMLString::release(&optionSingleLayer);
	xercesc::XMLString::release(&optionTriangularWarp);
	xercesc::XMLString::release(&optionNNWarp);
    xercesc::XMLString::release(&optionPolyWarp);
    xercesc::XMLString::release(&optionPxlShift);
    xercesc::XMLString::release(&optionGCP2GDAL);
	
	parsed = true; // if all successful, it is parsed
}

void RSGISExeImageRegistration::runAlgorithm() throw(rsgis::RSGISException)
{
	std::cout.precision(10);
	
	if(!parsed)
	{
		throw rsgis::RSGISException("Before running the parameters must be retrieved");
	}
	else
	{
		if(this->option == RSGISExeImageRegistration::basic)
		{
			std::cout << "A basic registration algorithm which simply loops throught a set of tie points and moves them using the selected metric\n";
			std::cout << "Reference Image: " << this->inputReferenceImage << std::endl;
			std::cout << "Floating Image: " << this->inputFloatingmage << std::endl;
			std::cout << "Output GCP File: " << this->outputGCPFile << std::endl;
            unsigned int metricTypeInt = 4;
            unsigned int outputGCPTypeInt = 3;
            
			if(outputType == envi_img2img)
			{
				std::cout << "Output in image to image GCPs for ENVI\n";
                outputGCPTypeInt = 1;
			}
			else if(outputType == envi_img2map)
			{
				std::cout << "Output in image to map GCPs for ENVI\n";
                outputGCPTypeInt = 2;
			}
			else if(outputType == rsgis_img2map)
			{
				std::cout << "Output GCPs for RSGIS image to map\n";
                outputGCPTypeInt = 3;
			}
            else if(outputType == rsgis_mapoffs)
			{
				std::cout << "Output GCPs for RSGIS map offsets\n";
                outputGCPTypeInt = 4;
			}
			else
			{
				throw rsgis::RSGISException("Output format Unknown");
			}
			if(metricType == euclidean)
			{
				std::cout << "The Euclidean similarity metric will be used\n";
                metricTypeInt = 1;
			}
			else if(metricType == sqdiff)
			{
				std::cout << "The Squared Difference similarity metric will be used\n";
                metricTypeInt = 2;
			}
			else if(metricType == manhatten)
			{
				std::cout << "The Manhatten (taxi cab) similarity metric will be used\n";
                metricTypeInt = 3;
			}
			else if(metricType == correlation)
			{
				std::cout << "The Correlation Coefficent similarity metric will be used\n";
                metricTypeInt = 4;
			}
			else 
			{
				throw rsgis::RSGISException("Similarity Metric Unknown");
			}
			std::cout << "Window Size = " << windowSize << std::endl;
			std::cout << "Search area = " << searchArea << std::endl;
			std::cout << "Metric Threshold = " << metricThreshold << std::endl;
			std::cout << "Std Dev reference threshold = " << stdDevRefThreshold << std::endl;
			std::cout << "Std Dev floating threshold = " << stdDevFloatThreshold << std::endl;
			std::cout << "Sub pixel resolution = " << subPixelResolution << std::endl;

			try
			{
                rsgis::cmds::excecuteBasicRegistration(this->inputReferenceImage, this->inputFloatingmage, this->gcpGap,
                                                      this->metricThreshold, this->windowSize, this->searchArea, this->stdDevRefThreshold,
                                                      this->stdDevFloatThreshold, this->subPixelResolution, metricTypeInt,
                                                      outputGCPTypeInt, this->outputGCPFile);
                
            }
            catch(rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
		}
		else if(this->option == RSGISExeImageRegistration::singlelayer)
		{
			std::cout << "A registration algorithm using a single layer of tie.\n";
			std::cout << "points which are connected within a given distance and updated as the neighbouring tie points are moved.\n";
			std::cout << "Reference Image: " << this->inputReferenceImage << std::endl;
			std::cout << "Floating Image: " << this->inputFloatingmage << std::endl;
			std::cout << "Output GCP File: " << this->outputGCPFile << std::endl;
            
            unsigned int metricTypeInt = 4;
            unsigned int outputGCPTypeInt = 3;
            
			if(outputType == envi_img2img)
			{
				std::cout << "Output in image to image GCPs for ENVI\n";
                outputGCPTypeInt = 1;
			}
			else if(outputType == envi_img2map)
			{
				std::cout << "Output in image to map GCPs for ENVI\n";
                outputGCPTypeInt = 2;
			}
			else if(outputType == rsgis_img2map)
			{
				std::cout << "Output GCPs for RSGIS image to map\n";
                outputGCPTypeInt = 3;
			}
            else if(outputType == rsgis_mapoffs)
			{
				std::cout << "Output GCPs for RSGIS map offsets\n";
                outputGCPTypeInt = 4;
			}
			else 
			{
				throw rsgis::RSGISException("Output format Unknown");
			}
			if(metricType == euclidean)
			{
				std::cout << "The Euclidean similarity metric will be used\n";
                metricTypeInt = 1;
			}
			else if(metricType == sqdiff)
			{
				std::cout << "The Squared Difference similarity metric will be used\n";
                metricTypeInt = 2;
			}
			else if(metricType == manhatten)
			{
				std::cout << "The Manhatten (taxi cab) similarity metric will be used\n";
                metricTypeInt = 3;
			}
			else if(metricType == correlation)
			{
				std::cout << "The Correlation Coefficent similarity metric will be used\n";
                metricTypeInt = 4;
			}
			else 
			{
				throw rsgis::RSGISException("Similarity Metric Unknown");
			}
			std::cout << "Window Size = " << this->windowSize << std::endl;
			std::cout << "Search area = " << this->searchArea << std::endl;
			std::cout << "Metric Threshold = " << this->metricThreshold << std::endl;
			std::cout << "Std Dev reference threshold = " << this->stdDevRefThreshold << std::endl;
			std::cout << "Std Dev floating threshold = " << this->stdDevFloatThreshold << std::endl;
			std::cout << "Sub pixel resolution = " << this->subPixelResolution << std::endl;
			std::cout << "Distance threshold = " << this->distanceThreshold << std::endl;
			std::cout << "Max. number of iterations = " << this->maxNumIterations << std::endl;
			std::cout << "Movement Threshold = " << this->moveChangeThreshold << std::endl;
			std::cout << "p Smoothness = " << this->pSmoothness << std::endl;
			
			try
			{
                rsgis::cmds::excecuteSingleLayerConnectedRegistration(this->inputReferenceImage, this->inputFloatingmage, this->gcpGap,
                                                                      this->metricThreshold, this->windowSize, this->searchArea, this->stdDevRefThreshold,
                                                                      this->stdDevFloatThreshold, this->subPixelResolution, this->distanceThreshold,
                                                                      this->maxNumIterations, this->moveChangeThreshold, this->pSmoothness, metricTypeInt,
                                                                      outputGCPTypeInt, this->outputGCPFile);
			}
            catch(rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
		}
		else if(this->option == RSGISExeImageRegistration::triangularwarp)
		{
			std::cout << "Warp an image image using a set of image to map ground control points using a triangulation\n";
			std::cout << "GCPs: " << this->inputGCPs << std::endl;
			std::cout << "Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Projection: " << this->projFile << std::endl;
			std::cout << "Output Resolution: " << this->resolution << std::endl;
            std::cout << "Output Image format: " << this->imageFormat << std::endl;
			
			try
			{
                rsgis::cmds::excecuteTriangularWarp(this->inputImage, this->outputImage, this->projFile, this->inputGCPs, this->resolution, this->imageFormat, this->genTransformImage);
            }
			catch(rsgis::cmds::RSGISCmdException &e)
			{
				throw rsgis::RSGISException(e.what());
			}
            
		}
		else if(this->option == RSGISExeImageRegistration::nnwarp)
		{
			std::cout << "Warp an image using a set of image to map ground control points using a NN algorithm\n";
			std::cout << "GCPs: " << this->inputGCPs << std::endl;
			std::cout << "Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Projection: " << this->projFile << std::endl;
			std::cout << "Output Resolution: " << this->resolution << std::endl;
            std::cout << "Output Image format: " << this->imageFormat << std::endl;
            
			try
			{
                rsgis::cmds::excecuteNNWarp(this->inputImage, this->outputImage, this->projFile, this->inputGCPs, this->resolution, this->imageFormat, this->genTransformImage);
            }
			catch(rsgis::cmds::RSGISCmdException &e)
			{
				throw rsgis::RSGISException(e.what());
			}

		}
		else if(this->option == RSGISExeImageRegistration::polywarp)
		{
			std::cout << "Warp an image using a set of image to map ground control points using a "; 
            if (this->polyOrder == 1) {std::cout << "1st order polynominal" << std::endl;}
            if (this->polyOrder == 2) {std::cout << "2nd order polynominal" << std::endl;}
            if (this->polyOrder == 3) {std::cout << "3rd order polynominal" << std::endl;}
            else {std::cout << this->polyOrder << "th order polynominal" << std::endl;}
            
			std::cout << "GCPs: " << this->inputGCPs << std::endl;
			std::cout << "Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Projection: " << this->projFile << std::endl;
			std::cout << "Output Resolution: " << this->resolution << std::endl;
            std::cout << "Output Image format: " << this->imageFormat << std::endl;
			
            try
			{
                rsgis::cmds::excecutePolyWarp(this->inputImage, this->outputImage, this->projFile, this->inputGCPs, this->resolution, this->polyOrder, this->imageFormat, this->genTransformImage);
            }
			catch(rsgis::cmds::RSGISCmdException &e)
			{
				throw rsgis::RSGISException(e.what());
			}

		}
        else if(this->option == RSGISExeImageRegistration::pxlshift)
		{
			std::cout << "A registration algorithm which attempts to register every pixel in the image.\n";
			std::cout << "Reference Image: " << this->inputReferenceImage << std::endl;
			std::cout << "Floating Image: " << this->inputFloatingmage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Output Format: " << this->imageFormat << std::endl;
			if(metricType == euclidean)
			{
				std::cout << "The Euclidean similarity metric will be used\n";
			}
			else if(metricType == sqdiff)
			{
				std::cout << "The Squared Difference similarity metric will be used\n";
			}
			else if(metricType == manhatten)
			{
				std::cout << "The Manhatten (taxi cab) similarity metric will be used\n";
			}
			else if(metricType == correlation)
			{
				std::cout << "The Correlation Coefficent similarity metric will be used\n";
			}
			else
			{
				throw rsgis::RSGISException("Similarity Metric Unknown");
			}
			std::cout << "Window Size = " << windowSize << std::endl;
			std::cout << "Search area = " << searchArea << std::endl;
			std::cout << "Sub pixel resolution = " << subPixelResolution << std::endl;
            
			try
			{
				GDALAllRegister();
				GDALDataset *inRefDataset = NULL;
				GDALDataset *inFloatDataset = NULL;
				
				inRefDataset = (GDALDataset *) GDALOpenShared(inputReferenceImage.c_str(), GA_ReadOnly);
				if(inRefDataset == NULL)
				{
					std::string message = std::string("Could not open image ") + inputReferenceImage;
					throw rsgis::RSGISException(message.c_str());
				}
				
				inFloatDataset = (GDALDataset *) GDALOpenShared(inputFloatingmage.c_str(), GA_ReadOnly);
				if(inFloatDataset == NULL)
				{
					std::string message = std::string("Could not open image ") + inputFloatingmage;
					throw rsgis::RSGISException(message.c_str());
				}
				
				rsgis::reg::RSGISImageSimilarityMetric *similarityMetric = NULL;
				if(metricType == euclidean)
				{
					similarityMetric = new rsgis::reg::RSGISEuclideanSimilarityMetric();
				}
				else if(metricType == sqdiff)
				{
					similarityMetric = new rsgis::reg::RSGISSquaredDifferenceSimilarityMetric();
				}
				else if(metricType == manhatten)
				{
					similarityMetric = new rsgis::reg::RSGISManhattanSimilarityMetric();
				}
				else if(metricType == correlation)
				{
					similarityMetric = new rsgis::reg::RSGISCorrelationSimilarityMetric();
				}
				
				rsgis::reg::RSGISImageRegistration *regImgs = new rsgis::reg::RSGISImagePixelRegistration(inRefDataset, inFloatDataset, this->outputImage, this->imageFormat, windowSize, searchArea, similarityMetric, subPixelResolution);
                
				regImgs->runCompleteRegistration();				
				
				delete similarityMetric;
				delete regImgs;
				
				GDALClose(inRefDataset);
				GDALClose(inFloatDataset);
				GDALDestroyDriverManager();
			}
			catch(rsgis::RSGISRegistrationException &e)
			{
				throw rsgis::RSGISException(e.what());
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
		}
        else if(this->option == RSGISExeImageRegistration::gcp2gdal)
		{
			std::cout << "Add GCPs to GDAL dataset";
            
			std::cout << "GCPs: " << this->inputGCPs << std::endl;
			std::cout << "Image: " << this->inputImage << std::endl;
            if(this->outputImage != ""){std::cout << "Output Image: " << this->outputImage << std::endl;}
			
			try
			{

                rsgis::cmds::excecuteAddGCPsGDAL(this->inputImage, this->inputGCPs, this->outputImage, this->imageFormat, this->rsgisOutDataType);
            }
			catch(rsgis::cmds::RSGISCmdException &e)
			{
				throw rsgis::RSGISException(e.what());
			}
		}
		else
		{
			throw rsgis::RSGISException("RSGISExeImageRegistration does not know this option");
		}
		
	}
}


void RSGISExeImageRegistration::printParameters()
{
	if(parsed)
	{
		if(this->option == RSGISExeImageRegistration::basic)
		{
			std::cout << "A basic registration algorithm which simply loops throught a set of tie points and moves them using the selected metric\n";
			std::cout << "Reference Image: " << this->inputReferenceImage << std::endl;
			std::cout << "Floating Image: " << this->inputFloatingmage << std::endl;
			std::cout << "Output GCP File: " << this->outputGCPFile << std::endl;
			if(outputType == envi_img2img)
			{
				std::cout << "Output in image to image GCPs for ENVI\n";
			}
			else if(outputType == envi_img2map)
			{
				std::cout << "Output in image to map GCPs for ENVI\n";
			}
			else if(outputType == rsgis_img2map)
			{
				std::cout << "Output GCPs for RSGIS image to map\n";
			}
            else if(outputType == rsgis_mapoffs)
			{
				std::cout << "Output GCPs for RSGIS map offsets\n";
			}
			else
			{
				throw rsgis::RSGISException("Output format Unknown");
			}
			if(metricType == euclidean)
			{
				std::cout << "The Euclidean similarity metric will be used\n";
			}
			else if(metricType == sqdiff)
			{
				std::cout << "The Squared Difference similarity metric will be used\n";
			}
			else if(metricType == manhatten)
			{
				std::cout << "The Manhatten (taxi cab) similarity metric will be used\n";
			}
			else if(metricType == correlation)
			{
				std::cout << "The Correlation Coefficent similarity metric will be used\n";
			}
			else 
			{
				std::cout << "Metric unknown\n";
			}
			std::cout << "Window Size = " << windowSize << std::endl;
			std::cout << "Search area = " << searchArea << std::endl;
			std::cout << "Metric Threshold = " << metricThreshold << std::endl;
			std::cout << "Std Dev reference threshold = " << stdDevRefThreshold << std::endl;
			std::cout << "Std Dev floating threshold = " << stdDevFloatThreshold << std::endl;
			std::cout << "Sub pixel resolution = " << subPixelResolution << std::endl;
		}
		else if(this->option == RSGISExeImageRegistration::singlelayer)
		{
			std::cout << "A registration algorithm which representings a single layer of tie\n";
			std::cout << "points which are connected within a given distance and updated as the neighbouring tie points are moved.\n";
			std::cout << "Reference Image: " << this->inputReferenceImage << std::endl;
			std::cout << "Floating Image: " << this->inputFloatingmage << std::endl;
			std::cout << "Output GCP File: " << this->outputGCPFile << std::endl;
			if(outputType == envi_img2img)
			{
				std::cout << "Output in image to image GCPs for ENVI\n";
			}
			else if(outputType == envi_img2map)
			{
				std::cout << "Output in image to map GCPs for ENVI\n";
			}
			else if(outputType == rsgis_img2map)
			{
				std::cout << "Output GCPs for RSGIS image to map\n";
			}
            else if(outputType == rsgis_mapoffs)
			{
				std::cout << "Output GCPs for RSGIS map offsets\n";
			}
			else
			{
				throw rsgis::RSGISException("Output format Unknown");
			}
			if(metricType == euclidean)
			{
				std::cout << "The Euclidean similarity metric will be used\n";
			}
			else if(metricType == sqdiff)
			{
				std::cout << "The Squared Difference similarity metric will be used\n";
			}
			else if(metricType == manhatten)
			{
				std::cout << "The Manhatten (taxi cab) similarity metric will be used\n";
			}
			else if(metricType == correlation)
			{
				std::cout << "The Correlation Coefficent similarity metric will be used\n";
			}
			else 
			{
				throw rsgis::RSGISException("Similarity Metric Unknown");
			}
			std::cout << "Window Size = " << windowSize << std::endl;
			std::cout << "Search area = " << searchArea << std::endl;
			std::cout << "Metric Threshold = " << metricThreshold << std::endl;
			std::cout << "Std Dev reference threshold = " << stdDevRefThreshold << std::endl;
			std::cout << "Std Dev floating threshold = " << stdDevFloatThreshold << std::endl;
			std::cout << "Sub pixel resolution = " << subPixelResolution << std::endl;
			std::cout << "Distance threshold = " << distanceThreshold << std::endl;
			std::cout << "Max. number of iterations = " << maxNumIterations << std::endl;
			std::cout << "Movement Threshold = " << moveChangeThreshold << std::endl;
			std::cout << "p Smoothness = " << pSmoothness << std::endl;
		}
		else if(this->option == RSGISExeImageRegistration::triangularwarp)
		{
			std::cout << "Warp an image using a set of image to map ground control points using a triangulation\n";
			std::cout << "GCPs: " << this->inputGCPs << std::endl;
			std::cout << "Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Projection: " << this->projFile << std::endl;
			std::cout << "Output Resolution: " << this->resolution << std::endl;
            std::cout << "Output Image format: " << this->imageFormat << std::endl;
		}
		else if(this->option == RSGISExeImageRegistration::nnwarp)
		{
			std::cout << "Warp an image using a set of image to map ground control points using a NN algorithm\n";
			std::cout << "GCPs: " << this->inputGCPs << std::endl;
			std::cout << "Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Projection: " << this->projFile << std::endl;
			std::cout << "Output Resolution: " << this->resolution << std::endl;
            std::cout << "Output Image format: " << this->imageFormat << std::endl;
		}
		else
		{
			throw rsgis::RSGISException("RSGISExeImageRegistration does not know this option");
		}
		
	}
	else
	{
		throw rsgis::RSGISException("The parameters have yet to be parsed");
	}
}

void RSGISExeImageRegistration::help()
{
	std::cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << std::endl;
	std::cout << "<!-- A command for automatically generating a set of ground control points between a pair of images " << std::endl;
    std::cout << "    - A basic algorithm with a single iteration and movement of gcps-->" << std::endl;
    std::cout << "<rsgis:command algor=\"registration\" option=\"basic\" reference=\"image\" " << std::endl;
    std::cout << "    floating=\"image\" output=\"gcps.txt\" outputType=\"envi_img2img|envi_img2map|rsgis_img2map|rsgis_mapoffs\" " << std::endl;
    std::cout << "    metric=\"euclidean|sqdiff|manhatten|correlation\" pixelgap=\"int\" window=\"int\" " << std::endl;
    std::cout << "    search=\"int\" threshold=\"float\" stddevRef=\"float\" stddevFloat=\"float\" subpixelresolution=\"int\"/>" << std::endl;
    std::cout << "<!-- A command for automatically generating a set of ground control points between a pair of images " << std::endl;
    std::cout << "    - An algorithm with a single layer of connected gcps which used IDW to shift neighbouring gcps within a distance threshold -->" << std::endl;
    std::cout << "<rsgis:command algor=\"registration\" option=\"singlelayer\" reference=\"image\" floating=\"image\" " << std::endl;
    std::cout << "    output=\"gcps.txt\" outputType=\"envi_img2img|envi_img2map|rsgis_img2map|rsgis_mapoffs\" " << std::endl;
    std::cout << "    metric=\"euclidean|sqdiff|manhatten|correlation\" pixelgap=\"int\" window=\"int\" search=\"int\" " << std::endl;
    std::cout << "    threshold=\"float\" stddevRef=\"float\" stddevFloat=\"float\" subpixelresolution=\"int\" distanceThreshold=\"float\" " << std::endl;
    std::cout << "    maxiterations=\"int\" movementThreshold=\"float\" pSmoothness=\"float\"/>" << std::endl;
    std::cout << "<!-- A command to warp an input image using a set of ground control points using a nearest neighbour algorithm -->" << std::endl;
    std::cout << "<rsgis:command algor=\"registration\" option=\"nnwarp\" gcps=\"string\" image=\"string\" output=\"string\" " << std::endl;
    std::cout << "    projection=\"file.wkt\" resolution=\"float\" format=\"string\" transform=\"yes | no\" />" << std::endl;
    std::cout << "<!-- A command to warp an input image using a set of ground control points using a polynomial -->" << std::endl;
    std::cout << "<rsgis:command algor=\"registration\" option=\"polywarp\" gcps=\"string\" image=\"string\" output=\"string\" " << std::endl;
    std::cout << "    projection=\"file.wkt\" resolution=\"float\" format=\"string\" polyOrder=\"int\" transform=\"yes | no\" />" << std::endl;
    std::cout << "<!-- A command to warp an input image using a set of ground control points using a triangulation -->" << std::endl;
    std::cout << "<rsgis:command algor=\"registration\" option=\"triangularwarp\" gcps=\"string\" image=\"string\" output=\"string\" " << std::endl;
    std::cout << "    projection=\"file.wkt\" resolution=\"float\" format=\"string\" transform=\"yes | no\" />" << std::endl;
    std::cout << "<!-- A command to add tie points as GCPs to GDAL dataset -->" << std::endl;
    std::cout << "<rsgis:command algor=\"registration\" option=\"gcp2gdal\" gcps=\"string\" image=\"image\" [output=\"image\" ] />" << std::endl;
    std::cout << "<!-- A command for doing a per pixel image matching where a 3 band image is outputted " << std::endl;
    std::cout << "   with the X Shift, Y Shift and Metric value -->" << std::endl;
    std::cout << "<rsgis:command algor=\"registration\" option=\"pxlshift\" reference=\"image\" floating=\"image\" output=\"image\" " << std::endl;
    std::cout << "format=\"string\" metric=\"euclidean|sqdiff|manhatten|correlation\" window=\"int\" search=\"int\" subpixelresolution=\"int\"/>" << std::endl;
	std::cout << "</rsgis:commands>" << std::endl;
}

std::string RSGISExeImageRegistration::getDescription()
{
	return "Provides a set of tools for automatically generating ground control points (GCPs) from image data.";
}

std::string RSGISExeImageRegistration::getXMLSchema()
{
	return "NOT PROVIDED!";
}

RSGISExeImageRegistration::~RSGISExeImageRegistration()
{
	
}

}

