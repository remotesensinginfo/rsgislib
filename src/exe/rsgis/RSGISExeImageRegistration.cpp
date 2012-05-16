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

RSGISExeImageRegistration::RSGISExeImageRegistration() : RSGISAlgorithmParameters()
{
	this->algorithm = "registration";
}

RSGISAlgorithmParameters* RSGISExeImageRegistration::getInstance()
{
	return new RSGISExeImageRegistration();
}

void RSGISExeImageRegistration::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;	
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *optionBasic = XMLString::transcode("basic");
	XMLCh *optionSingleLayer = XMLString::transcode("singlelayer");
	XMLCh *optionTriangularWarp = XMLString::transcode("triangularwarp");
	XMLCh *optionNNWarp = XMLString::transcode("nnwarp");
    XMLCh *optionPolyWarp = XMLString::transcode("polywarp");
	
	try
	{
		const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
		if(!XMLString::equals(algorName, algorNameEle))
		{
			throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
		}
		
		
		const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
		if(XMLString::equals(optionBasic, optionXML))
		{
			this->option = RSGISExeImageRegistration::basic;
			
			XMLCh *referenceXMLStr = XMLString::transcode("reference");
			if(argElement->hasAttribute(referenceXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(referenceXMLStr));
				this->inputReferenceImage = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'reference\' attribute was provided.");
			}
			XMLString::release(&referenceXMLStr);
			
			XMLCh *floatingXMLStr = XMLString::transcode("floating");
			if(argElement->hasAttribute(floatingXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(floatingXMLStr));
				this->inputFloatingmage = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'floating\' attribute was provided.");
			}
			XMLString::release(&floatingXMLStr);
			
			
			XMLCh *outputXMLStr = XMLString::transcode("output");
			if(argElement->hasAttribute(outputXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputGCPFile = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			XMLString::release(&outputXMLStr);
			
			
			XMLCh *outputTypeXMLStr = XMLString::transcode("outputType");
			if(argElement->hasAttribute(outputTypeXMLStr))
			{
				XMLCh *envImg2ImgStr = XMLString::transcode("envi_img2img");
				XMLCh *enviImg2MapStr = XMLString::transcode("envi_img2map");
				XMLCh *rsgisImg2MapStr = XMLString::transcode("rsgis_img2map");
				
				const XMLCh *outTypeValue = argElement->getAttribute(outputTypeXMLStr);
				
				if(XMLString::equals(outTypeValue, envImg2ImgStr))
				{
					this->outputType = RSGISExeImageRegistration::envi_img2img;
				}
				else if(XMLString::equals(outTypeValue, enviImg2MapStr))
				{
					this->outputType = RSGISExeImageRegistration::envi_img2map;
				}
				else if(XMLString::equals(outTypeValue, rsgisImg2MapStr))
				{
					this->outputType = RSGISExeImageRegistration::rsgis_img2map;
				}
				else
				{
					this->outputType = RSGISExeImageRegistration::undefinedOutput;
					throw RSGISXMLArgumentsException("The \'outputType\' attribute needs to have one of the following values envi_img2img | envi_img2map | gdal.");
				}
				XMLString::release(&envImg2ImgStr);
				XMLString::release(&enviImg2MapStr);
				XMLString::release(&rsgisImg2MapStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'outputType\' attribute was provided.");
			}
			XMLString::release(&outputTypeXMLStr);
			
			
			XMLCh *metricXMLStr = XMLString::transcode("metric");
			if(argElement->hasAttribute(metricXMLStr))
			{
				XMLCh *euclideanStr = XMLString::transcode("euclidean");
				XMLCh *sqdiffStr = XMLString::transcode("sqdiff");
				XMLCh *manhattenStr = XMLString::transcode("manhatten");
				XMLCh *correlationStr = XMLString::transcode("correlation");
				
				const XMLCh *metricValue = argElement->getAttribute(metricXMLStr);
				
				if(XMLString::equals(metricValue, euclideanStr))
				{
					this->metricType = RSGISExeImageRegistration::euclidean;
				}
				else if(XMLString::equals(metricValue, sqdiffStr))
				{
					this->metricType = RSGISExeImageRegistration::sqdiff;
				}
				else if(XMLString::equals(metricValue, manhattenStr))
				{
					this->metricType = RSGISExeImageRegistration::manhatten;
				}
				else if(XMLString::equals(metricValue, correlationStr))
				{
					this->metricType = RSGISExeImageRegistration::correlation;
				}
				else
				{
					this->metricType = RSGISExeImageRegistration::undefinedMetric;
					throw RSGISXMLArgumentsException("The \'metric\' attribute needs to have one of the following values euclidean | sqdiff | manhatten | correlation.");
				}
				XMLString::release(&euclideanStr);
				XMLString::release(&sqdiffStr);
				XMLString::release(&manhattenStr);
				XMLString::release(&correlationStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'metric\' attribute was provided.");
			}
			XMLString::release(&metricXMLStr);
			
			
			XMLCh *pixelGapXMLStr = XMLString::transcode("pixelgap");
			if(argElement->hasAttribute(pixelGapXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(pixelGapXMLStr));
				this->gcpGap = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'pixelgap\' attribute was provided.");
			}
			XMLString::release(&pixelGapXMLStr);
			
			XMLCh *windowXMLStr = XMLString::transcode("window");
			if(argElement->hasAttribute(windowXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(windowXMLStr));
				this->windowSize = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'window\' attribute was provided.");
			}
			XMLString::release(&windowXMLStr);
			
			XMLCh *searchXMLStr = XMLString::transcode("search");
			if(argElement->hasAttribute(searchXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(searchXMLStr));
				this->searchArea = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'search\' attribute was provided.");
			}
			XMLString::release(&searchXMLStr);
			
			
			XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
			if(argElement->hasAttribute(thresholdXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
				this->metricThreshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
			}
			XMLString::release(&thresholdXMLStr);
			
			XMLCh *stddevRefXMLStr = XMLString::transcode("stddevRef");
			if(argElement->hasAttribute(stddevRefXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(stddevRefXMLStr));
				this->stdDevRefThreshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'stddevRef\' attribute was provided.");
			}
			XMLString::release(&stddevRefXMLStr);
			
			XMLCh *stddevFloatXMLStr = XMLString::transcode("stddevFloat");
			if(argElement->hasAttribute(stddevFloatXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(stddevFloatXMLStr));
				this->stdDevFloatThreshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'stddevFloat\' attribute was provided.");
			}
			XMLString::release(&stddevFloatXMLStr);
			
			XMLCh *subPixelResXMLStr = XMLString::transcode("subpixelresolution");
			if(argElement->hasAttribute(subPixelResXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(subPixelResXMLStr));
				this->subPixelResolution = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'subpixelresolution\' attribute was provided.");
			}
			XMLString::release(&subPixelResXMLStr);
		}
		else if(XMLString::equals(optionSingleLayer, optionXML))
		{
			this->option = RSGISExeImageRegistration::singlelayer;
			
			XMLCh *referenceXMLStr = XMLString::transcode("reference");
			if(argElement->hasAttribute(referenceXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(referenceXMLStr));
				this->inputReferenceImage = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'reference\' attribute was provided.");
			}
			XMLString::release(&referenceXMLStr);
			
			XMLCh *floatingXMLStr = XMLString::transcode("floating");
			if(argElement->hasAttribute(floatingXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(floatingXMLStr));
				this->inputFloatingmage = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'floating\' attribute was provided.");
			}
			XMLString::release(&floatingXMLStr);
			
			
			XMLCh *outputXMLStr = XMLString::transcode("output");
			if(argElement->hasAttribute(outputXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputGCPFile = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			XMLString::release(&outputXMLStr);
			
			
			XMLCh *outputTypeXMLStr = XMLString::transcode("outputType");
			if(argElement->hasAttribute(outputTypeXMLStr))
			{
				XMLCh *envImg2ImgStr = XMLString::transcode("envi_img2img");
				XMLCh *enviImg2MapStr = XMLString::transcode("envi_img2map");
				XMLCh *rsgisImg2MapStr = XMLString::transcode("rsgis_img2map");
				
				const XMLCh *outTypeValue = argElement->getAttribute(outputTypeXMLStr);
				
				if(XMLString::equals(outTypeValue, envImg2ImgStr))
				{
					this->outputType = RSGISExeImageRegistration::envi_img2img;
				}
				else if(XMLString::equals(outTypeValue, enviImg2MapStr))
				{
					this->outputType = RSGISExeImageRegistration::envi_img2map;
				}
				else if(XMLString::equals(outTypeValue, rsgisImg2MapStr))
				{
					this->outputType = RSGISExeImageRegistration::rsgis_img2map;
				}
				else
				{
					this->outputType = RSGISExeImageRegistration::undefinedOutput;
					throw RSGISXMLArgumentsException("The \'outputType\' attribute needs to have one of the following values envi_img2img | envi_img2map | gdal.");
				}
				XMLString::release(&envImg2ImgStr);
				XMLString::release(&enviImg2MapStr);
				XMLString::release(&rsgisImg2MapStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'outputType\' attribute was provided.");
			}
			XMLString::release(&outputTypeXMLStr);
			
			
			XMLCh *metricXMLStr = XMLString::transcode("metric");
			if(argElement->hasAttribute(metricXMLStr))
			{
				XMLCh *euclideanStr = XMLString::transcode("euclidean");
				XMLCh *sqdiffStr = XMLString::transcode("sqdiff");
				XMLCh *manhattenStr = XMLString::transcode("manhatten");
				XMLCh *correlationStr = XMLString::transcode("correlation");
				
				const XMLCh *metricValue = argElement->getAttribute(metricXMLStr);
				
				if(XMLString::equals(metricValue, euclideanStr))
				{
					this->metricType = RSGISExeImageRegistration::euclidean;
				}
				else if(XMLString::equals(metricValue, sqdiffStr))
				{
					this->metricType = RSGISExeImageRegistration::sqdiff;
				}
				else if(XMLString::equals(metricValue, manhattenStr))
				{
					this->metricType = RSGISExeImageRegistration::manhatten;
				}
				else if(XMLString::equals(metricValue, correlationStr))
				{
					this->metricType = RSGISExeImageRegistration::correlation;
				}
				else
				{
					this->metricType = RSGISExeImageRegistration::undefinedMetric;
					throw RSGISXMLArgumentsException("The \'metric\' attribute needs to have one of the following values euclidean | sqdiff | manhatten | correlation.");
				}
				XMLString::release(&euclideanStr);
				XMLString::release(&sqdiffStr);
				XMLString::release(&manhattenStr);
				XMLString::release(&correlationStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'metric\' attribute was provided.");
			}
			XMLString::release(&metricXMLStr);
			
			
			XMLCh *pixelGapXMLStr = XMLString::transcode("pixelgap");
			if(argElement->hasAttribute(pixelGapXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(pixelGapXMLStr));
				this->gcpGap = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'pixelgap\' attribute was provided.");
			}
			XMLString::release(&pixelGapXMLStr);
			
			XMLCh *windowXMLStr = XMLString::transcode("window");
			if(argElement->hasAttribute(windowXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(windowXMLStr));
				this->windowSize = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'window\' attribute was provided.");
			}
			XMLString::release(&windowXMLStr);
			
			XMLCh *searchXMLStr = XMLString::transcode("search");
			if(argElement->hasAttribute(searchXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(searchXMLStr));
				this->searchArea = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'search\' attribute was provided.");
			}
			XMLString::release(&searchXMLStr);
			
			
			XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
			if(argElement->hasAttribute(thresholdXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
				this->metricThreshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
			}
			XMLString::release(&thresholdXMLStr);
			
			XMLCh *stddevRefXMLStr = XMLString::transcode("stddevRef");
			if(argElement->hasAttribute(stddevRefXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(stddevRefXMLStr));
				this->stdDevRefThreshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'stddevRef\' attribute was provided.");
			}
			XMLString::release(&stddevRefXMLStr);
			
			XMLCh *stddevFloatXMLStr = XMLString::transcode("stddevFloat");
			if(argElement->hasAttribute(stddevFloatXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(stddevFloatXMLStr));
				this->stdDevFloatThreshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'stddevFloat\' attribute was provided.");
			}
			XMLString::release(&stddevFloatXMLStr);
			
			XMLCh *subPixelResXMLStr = XMLString::transcode("subpixelresolution");
			if(argElement->hasAttribute(subPixelResXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(subPixelResXMLStr));
				this->subPixelResolution = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'subpixelresolution\' attribute was provided.");
			}
			XMLString::release(&subPixelResXMLStr);
			
			
			XMLCh *distanceThresholdXMLStr = XMLString::transcode("distanceThreshold");
			if(argElement->hasAttribute(distanceThresholdXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(distanceThresholdXMLStr));
				this->distanceThreshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'distanceThreshold\' attribute was provided.");
			}
			XMLString::release(&distanceThresholdXMLStr);
			
			
			XMLCh *maxIterationsXMLStr = XMLString::transcode("maxiterations");
			if(argElement->hasAttribute(maxIterationsXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(maxIterationsXMLStr));
				this->maxNumIterations = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'maxiterations\' attribute was provided.");
			}
			XMLString::release(&maxIterationsXMLStr);
			
			XMLCh *moveChangeThresholdXMLStr = XMLString::transcode("movementThreshold");
			if(argElement->hasAttribute(moveChangeThresholdXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(moveChangeThresholdXMLStr));
				this->moveChangeThreshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'movementThreshold\' attribute was provided.");
			}
			XMLString::release(&moveChangeThresholdXMLStr);

			
			XMLCh *pSmoothnessXMLStr = XMLString::transcode("pSmoothness");
			if(argElement->hasAttribute(pSmoothnessXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(pSmoothnessXMLStr));
				this->pSmoothness = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'pSmoothness\' attribute was provided.");
			}
			XMLString::release(&pSmoothnessXMLStr);
			
		}
		else if(XMLString::equals(optionTriangularWarp, optionXML))
		{
			this->option = triangularwarp;
			
			XMLCh *gcpsXMLStr = XMLString::transcode("gcps");
			if(argElement->hasAttribute(gcpsXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(gcpsXMLStr));
				this->inputGCPs = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'gcps\' attribute was provided.");
			}
			XMLString::release(&gcpsXMLStr);

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
			
			XMLCh *outputImageXMLStr = XMLString::transcode("output");
			if(argElement->hasAttribute(outputImageXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputImageXMLStr));
				this->outputImage = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			XMLString::release(&outputImageXMLStr);

			XMLCh *projectionXMLStr = XMLString::transcode("projection");
			if(argElement->hasAttribute(projectionXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(projectionXMLStr));
				this->projFile = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'projection\' attribute was provided.");
			}
			XMLString::release(&projectionXMLStr);
			
			XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
			if(argElement->hasAttribute(resolutionXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(resolutionXMLStr));
				this->resolution = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided.");
			}
			XMLString::release(&resolutionXMLStr);
			
			XMLCh *outImageFormatXMLStr = XMLString::transcode("format");
			if(argElement->hasAttribute(outImageFormatXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outImageFormatXMLStr));
				this->outImageFormat = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				this->outImageFormat = "ENVI";
			}
			XMLString::release(&outImageFormatXMLStr);
            
            XMLCh *outTransformXMLStr = XMLString::transcode("transform");
			if(argElement->hasAttribute(outTransformXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outTransformXMLStr));
				string outTransform = string(charValue);
                if(outTransform == "yes")
                {
                    this->genTransformImage = true;
                }
                else
                {
                    this->genTransformImage = false;
                }
				XMLString::release(&charValue);
			}
			else
			{
				this->genTransformImage = false;
			}
			XMLString::release(&outTransformXMLStr);
			
		}
		else if((XMLString::equals(optionNNWarp, optionXML)) | (XMLString::equals(optionPolyWarp, optionXML)))
		{
			if(XMLString::equals(optionNNWarp, optionXML)){this->option = nnwarp;}
            else{this->option = polywarp;}
			
			XMLCh *gcpsXMLStr = XMLString::transcode("gcps");
			if(argElement->hasAttribute(gcpsXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(gcpsXMLStr));
				this->inputGCPs = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'gcps\' attribute was provided.");
			}
			XMLString::release(&gcpsXMLStr);
			
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
			
			XMLCh *outputImageXMLStr = XMLString::transcode("output");
			if(argElement->hasAttribute(outputImageXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputImageXMLStr));
				this->outputImage = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			XMLString::release(&outputImageXMLStr);
			
			XMLCh *projectionXMLStr = XMLString::transcode("projection");
			if(argElement->hasAttribute(projectionXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(projectionXMLStr));
				this->projFile = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'projection\' attribute was provided.");
			}
			XMLString::release(&projectionXMLStr);
			
			XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
			if(argElement->hasAttribute(resolutionXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(resolutionXMLStr));
				this->resolution = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided.");
			}
			XMLString::release(&resolutionXMLStr);
			
			
            XMLCh *outImageFormatXMLStr = XMLString::transcode("format");
			if(argElement->hasAttribute(outImageFormatXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outImageFormatXMLStr));
				this->outImageFormat = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				this->outImageFormat = "ENVI";
			}
			XMLString::release(&outImageFormatXMLStr);
            
            if(this->option == polywarp) // Get polynominal order for polynominal warp
            {
                XMLCh *polyOrderStr = XMLString::transcode("polyOrder"); // Polynomial Order
                if(argElement->hasAttribute(polyOrderStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(polyOrderStr));
                    this->polyOrder = mathUtils.strtoint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No value provided for polynomial order");
                }
                XMLString::release(&polyOrderStr);
            }
            
            
            
            XMLCh *outTransformXMLStr = XMLString::transcode("transform");
			if(argElement->hasAttribute(outTransformXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outTransformXMLStr));
				string outTransform = string(charValue);
                if(outTransform == "yes")
                {
                    this->genTransformImage = true;
                }
                else
                {
                    this->genTransformImage = false;
                }
				XMLString::release(&charValue);
			}
			else
			{
				this->genTransformImage = false;
			}
			XMLString::release(&outTransformXMLStr);
			
		}
		else 
		{
			string message = "RSGISExeImageRegistration did not recognise option " + string(XMLString::transcode(optionXML));
			throw RSGISXMLArgumentsException(message);
		}

	}
	catch(RSGISXMLArgumentsException &e)
	{
		throw e;
	}
	
	XMLString::release(&algorName);
	XMLString::release(&algorXMLStr);
	XMLString::release(&optionXMLStr);
	XMLString::release(&optionBasic);
	XMLString::release(&optionSingleLayer);
	XMLString::release(&optionTriangularWarp);
	XMLString::release(&optionNNWarp);
    XMLString::release(&optionPolyWarp);
	
	parsed = true; // if all successful, it is parsed
}

void RSGISExeImageRegistration::runAlgorithm() throw(RSGISException)
{
	cout.precision(10);
	
	if(!parsed)
	{
		throw RSGISException("Before running the parameters must be retrieved");
	}
	else
	{
		if(this->option == RSGISExeImageRegistration::basic)
		{
			cout << "A basic registration algorithm which simply loops throught a set of tie points and moves them using the selected metric\n";
			cout << "Reference Image: " << this->inputReferenceImage << endl;
			cout << "Floating Image: " << this->inputFloatingmage << endl;
			cout << "Output GCP File: " << this->outputGCPFile << endl;
			if(outputType == envi_img2img)
			{
				cout << "Output in image to image GCPs for ENVI\n";
			}
			else if(outputType == envi_img2map)
			{
				cout << "Output in image to map GCPs for ENVI\n";
			}
			else if(outputType == rsgis_img2map)
			{
				cout << "Output GCPs for RSGIS image to map\n";
			}
			else 
			{
				throw RSGISException("Output format Unknown");
			}
			if(metricType == euclidean)
			{
				cout << "The Euclidean similarity metric will be used\n";
			}
			else if(metricType == sqdiff)
			{
				cout << "The Squared Difference similarity metric will be used\n";
			}
			else if(metricType == manhatten)
			{
				cout << "The Manhatten (taxi cab) similarity metric will be used\n";
			}
			else if(metricType == correlation)
			{
				cout << "The Correlation Coefficent similarity metric will be used\n";
			}
			else 
			{
				throw RSGISException("Similarity Metric Unknown");
			}
			cout << "Window Size = " << windowSize << endl;
			cout << "Search area = " << searchArea << endl;
			cout << "Metric Threshold = " << metricThreshold << endl;
			cout << "Std Dev reference threshold = " << stdDevRefThreshold << endl;
			cout << "Std Dev floating threshold = " << stdDevFloatThreshold << endl;
			cout << "Sub pixel resolution = " << subPixelResolution << endl;

			try
			{
				GDALAllRegister();
				GDALDataset *inRefDataset = NULL;
				GDALDataset *inFloatDataset = NULL;
				
				inRefDataset = (GDALDataset *) GDALOpenShared(inputReferenceImage.c_str(), GA_ReadOnly);
				if(inRefDataset == NULL)
				{
					string message = string("Could not open image ") + inputReferenceImage;
					throw RSGISException(message.c_str());
				}
				
				inFloatDataset = (GDALDataset *) GDALOpenShared(inputFloatingmage.c_str(), GA_ReadOnly);
				if(inFloatDataset == NULL)
				{
					string message = string("Could not open image ") + inputFloatingmage;
					throw RSGISException(message.c_str());
				}
				
				RSGISImageSimilarityMetric *similarityMetric = NULL;
				if(metricType == euclidean)
				{
					similarityMetric = new RSGISEuclideanSimilarityMetric();
				}
				else if(metricType == sqdiff)
				{
					similarityMetric = new RSGISSquaredDifferenceSimilarityMetric();		
				}
				else if(metricType == manhatten)
				{
					similarityMetric = new RSGISManhattanSimilarityMetric();		
				}
				else if(metricType == correlation)
				{
					similarityMetric = new RSGISCorrelationSimilarityMetric();		
				}
				
				
				RSGISImageRegistration *regImgs = new RSGISBasicImageRegistration(inRefDataset, inFloatDataset, gcpGap, metricThreshold, windowSize, searchArea, similarityMetric, stdDevRefThreshold, stdDevFloatThreshold, subPixelResolution);

				regImgs->runCompleteRegistration();
				
				if(outputType == envi_img2img)
				{
					regImgs->exportTiePointsENVIImage2Image(this->outputGCPFile);
				}
				else if(outputType == envi_img2map)
				{
					regImgs->exportTiePointsENVIImage2Map(this->outputGCPFile);
				}
				else if(outputType == rsgis_img2map)
				{
					regImgs->exportTiePointsRSGISImage2Map(this->outputGCPFile);
				}
				
				delete similarityMetric;
				delete regImgs;
				
				GDALClose(inRefDataset);
				GDALClose(inFloatDataset);
				GDALDestroyDriverManager();
			}
			catch(RSGISRegistrationException &e)
			{
				throw RSGISException(e.what());
			}
			catch(RSGISException& e)
			{
				throw e;
			}
		}
		else if(this->option == RSGISExeImageRegistration::singlelayer)
		{
			cout << "A registration algorithm which representings a single layer of tie\n";
			cout << "points which are connected within a given distance and updated as the neighbouring tie points are moved.\n";
			cout << "Reference Image: " << this->inputReferenceImage << endl;
			cout << "Floating Image: " << this->inputFloatingmage << endl;
			cout << "Output GCP File: " << this->outputGCPFile << endl;
			if(outputType == envi_img2img)
			{
				cout << "Output in image to image GCPs for ENVI\n";
			}
			else if(outputType == envi_img2map)
			{
				cout << "Output in image to map GCPs for ENVI\n";
			}
			else if(outputType == rsgis_img2map)
			{
				cout << "Output GCPs for RSGIS image to map\n";
			}
			else 
			{
				throw RSGISException("Output format Unknown");
			}
			if(metricType == euclidean)
			{
				cout << "The Euclidean similarity metric will be used\n";
			}
			else if(metricType == sqdiff)
			{
				cout << "The Squared Difference similarity metric will be used\n";
			}
			else if(metricType == manhatten)
			{
				cout << "The Manhatten (taxi cab) similarity metric will be used\n";
			}
			else if(metricType == correlation)
			{
				cout << "The Correlation Coefficent similarity metric will be used\n";
			}
			else 
			{
				throw RSGISException("Similarity Metric Unknown");
			}
			cout << "Window Size = " << windowSize << endl;
			cout << "Search area = " << searchArea << endl;
			cout << "Metric Threshold = " << metricThreshold << endl;
			cout << "Std Dev reference threshold = " << stdDevRefThreshold << endl;
			cout << "Std Dev floating threshold = " << stdDevFloatThreshold << endl;
			cout << "Sub pixel resolution = " << subPixelResolution << endl;
			cout << "Distance threshold = " << distanceThreshold << endl;
			cout << "Max. number of iterations = " << maxNumIterations << endl;
			cout << "Movement Threshold = " << moveChangeThreshold << endl;
			cout << "p Smoothness = " << pSmoothness << endl;
			
			try
			{
				GDALAllRegister();
				GDALDataset *inRefDataset = NULL;
				GDALDataset *inFloatDataset = NULL;
				
				inRefDataset = (GDALDataset *) GDALOpenShared(inputReferenceImage.c_str(), GA_ReadOnly);
				if(inRefDataset == NULL)
				{
					string message = string("Could not open image ") + inputReferenceImage;
					throw RSGISException(message.c_str());
				}
				
				inFloatDataset = (GDALDataset *) GDALOpenShared(inputFloatingmage.c_str(), GA_ReadOnly);
				if(inFloatDataset == NULL)
				{
					string message = string("Could not open image ") + inputFloatingmage;
					throw RSGISException(message.c_str());
				}
				
				RSGISImageSimilarityMetric *similarityMetric = NULL;
				if(metricType == euclidean)
				{
					similarityMetric = new RSGISEuclideanSimilarityMetric();
				}
				else if(metricType == sqdiff)
				{
					similarityMetric = new RSGISSquaredDifferenceSimilarityMetric();		
				}
				else if(metricType == manhatten)
				{
					similarityMetric = new RSGISManhattanSimilarityMetric();		
				}
				else if(metricType == correlation)
				{
					similarityMetric = new RSGISCorrelationSimilarityMetric();		
				}
				
				
				RSGISImageRegistration *regImgs = new RSGISSingleConnectLayerImageRegistration(inRefDataset, inFloatDataset, gcpGap, metricThreshold, windowSize, searchArea, similarityMetric, stdDevRefThreshold, stdDevFloatThreshold, subPixelResolution, distanceThreshold, maxNumIterations, moveChangeThreshold, pSmoothness);
				
				regImgs->runCompleteRegistration();
				
				if(outputType == envi_img2img)
				{
					regImgs->exportTiePointsENVIImage2Image(this->outputGCPFile);
				}
				else if(outputType == envi_img2map)
				{
					regImgs->exportTiePointsENVIImage2Map(this->outputGCPFile);
				}
				else if(outputType == rsgis_img2map)
				{
					regImgs->exportTiePointsRSGISImage2Map(this->outputGCPFile);
				}
				
				delete similarityMetric;
				delete regImgs;
				
				GDALClose(inRefDataset);
				GDALClose(inFloatDataset);
				GDALDestroyDriverManager();
			}
			catch(RSGISRegistrationException &e)
			{
				throw RSGISException(e.what());
			}
			catch(RSGISException& e)
			{
				throw e;
			}			
		}
		else if(this->option == RSGISExeImageRegistration::triangularwarp)
		{
			cout << "Warp an image image using a set of image to map ground control points using a triangulation\n";
			cout << "GCPs: " << this->inputGCPs << endl;
			cout << "Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Projection: " << this->projFile << endl;
			cout << "Output Resolution: " << this->resolution << endl;
            cout << "Output Image format: " << this->outImageFormat << endl;
			
			GDALAllRegister();
			RSGISWarpImage *warp = NULL;
			RSGISWarpImageInterpolator *interpolator = new RSGISWarpImageNNInterpolator();
			
			try 
			{
				string projWKTStr = "";
                if(this->projFile != "")
                {
                    RSGISTextUtils textUtils;
                    projWKTStr = textUtils.readFileToString(this->projFile);
                }
                
				warp = new RSGISWarpImageUsingTriangulation(this->inputImage, this->outputImage, projWKTStr, this->inputGCPs, this->resolution, interpolator, this->outImageFormat);
				if(this->genTransformImage)
                {
                    warp->generateTransformImage();
                }
                else
                {
                    warp->performWarp();
                }
				delete warp;
			}
			catch (RSGISException &e) 
			{
				throw e;
			}
			GDALDestroyDriverManager();
		}
		else if(this->option == RSGISExeImageRegistration::nnwarp)
		{
			cout << "Warp an image using a set of image to map ground control points using a NN algorithm\n";
			cout << "GCPs: " << this->inputGCPs << endl;
			cout << "Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Projection: " << this->projFile << endl;
			cout << "Output Resolution: " << this->resolution << endl;
            cout << "Output Image format: " << this->outImageFormat << endl;
			
			GDALAllRegister();
			RSGISWarpImage *warp = NULL;
			RSGISWarpImageInterpolator *interpolator = new RSGISWarpImageNNInterpolator();
			
			try 
			{
                string projWKTStr = "";
                if(this->projFile != "")
                {
                    RSGISTextUtils textUtils;
                    projWKTStr = textUtils.readFileToString(this->projFile);
                }
                
				warp = new RSGISBasicNNGCPImageWarp(this->inputImage, this->outputImage, projWKTStr, this->inputGCPs, this->resolution, interpolator, this->outImageFormat);
				if(this->genTransformImage)
                {
                    warp->generateTransformImage();
                }
                else
                {
                    warp->performWarp();
                }
				delete warp;
			}
			catch (RSGISException &e) 
			{
				throw e;
			}
			GDALDestroyDriverManager();
		}
		else if(this->option == RSGISExeImageRegistration::polywarp)
		{
			cout << "Warp an image using a set of image to map ground control points using a "; 
            if (this->polyOrder == 1) {cout << "1st order polynominal" << endl;}
            if (this->polyOrder == 2) {cout << "2nd order polynominal" << endl;}
            if (this->polyOrder == 3) {cout << "3rd order polynominal" << endl;}
            else {cout << this->polyOrder << "th order polynominal" << endl;}
            
			cout << "GCPs: " << this->inputGCPs << endl;
			cout << "Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Projection: " << this->projFile << endl;
			cout << "Output Resolution: " << this->resolution << endl;
            cout << "Output Image format: " << this->outImageFormat << endl;
			
			GDALAllRegister();
			RSGISWarpImage *warp = NULL;
			RSGISWarpImageInterpolator *interpolator = new RSGISWarpImageNNInterpolator();
			
			try 
			{
                string projWKTStr = "";
                if(this->projFile != "")
                {
                    RSGISTextUtils textUtils;
                    projWKTStr = textUtils.readFileToString(this->projFile);
                }
                
				warp = new RSGISPolynomialImageWarp(this->inputImage, this->outputImage, projWKTStr, this->inputGCPs, this->resolution, interpolator, this->polyOrder, this->outImageFormat);
				if(this->genTransformImage)
                {
                    warp->generateTransformImage();
                }
                else
                {
                    warp->performWarp();
                }
				delete warp;
			}
			catch (RSGISException &e) 
			{
				throw e;
			}
			GDALDestroyDriverManager();
		}
		else
		{
			throw RSGISException("RSGISExeImageRegistration does not know this option");
		}
		
	}
}


void RSGISExeImageRegistration::printParameters()
{
	if(parsed)
	{
		if(this->option == RSGISExeImageRegistration::basic)
		{
			cout << "A basic registration algorithm which simply loops throught a set of tie points and moves them using the selected metric\n";
			cout << "Reference Image: " << this->inputReferenceImage << endl;
			cout << "Floating Image: " << this->inputFloatingmage << endl;
			cout << "Output GCP File: " << this->outputGCPFile << endl;
			if(outputType == envi_img2img)
			{
				cout << "Output in image to image GCPs for ENVI\n";
			}
			else if(outputType == envi_img2img)
			{
				cout << "Output in image to map GCPs for ENVI\n";
			}
			else if(outputType == envi_img2img)
			{
				cout << "Output GCPs for GDAL\n";
			}
			else 
			{
				cout << "Format unknown\n";
			}
			if(metricType == euclidean)
			{
				cout << "The Euclidean similarity metric will be used\n";
			}
			else if(metricType == sqdiff)
			{
				cout << "The Squared Difference similarity metric will be used\n";
			}
			else if(metricType == manhatten)
			{
				cout << "The Manhatten (taxi cab) similarity metric will be used\n";
			}
			else if(metricType == correlation)
			{
				cout << "The Correlation Coefficent similarity metric will be used\n";
			}
			else 
			{
				cout << "Metric unknown\n";
			}
			cout << "Window Size = " << windowSize << endl;
			cout << "Search area = " << searchArea << endl;
			cout << "Metric Threshold = " << metricThreshold << endl;
			cout << "Std Dev reference threshold = " << stdDevRefThreshold << endl;
			cout << "Std Dev floating threshold = " << stdDevFloatThreshold << endl;
			cout << "Sub pixel resolution = " << subPixelResolution << endl;
		}
		else if(this->option == RSGISExeImageRegistration::singlelayer)
		{
			cout << "A registration algorithm which representings a single layer of tie\n";
			cout << "points which are connected within a given distance and updated as the neighbouring tie points are moved.\n";
			cout << "Reference Image: " << this->inputReferenceImage << endl;
			cout << "Floating Image: " << this->inputFloatingmage << endl;
			cout << "Output GCP File: " << this->outputGCPFile << endl;
			if(outputType == envi_img2img)
			{
				cout << "Output in image to image GCPs for ENVI\n";
			}
			else if(outputType == envi_img2map)
			{
				cout << "Output in image to map GCPs for ENVI\n";
			}
			else if(outputType == rsgis_img2map)
			{
				cout << "Output GCPs for RSGIS image to Map\n";
			}
			else 
			{
				throw RSGISException("Output format Unknown");
			}
			if(metricType == euclidean)
			{
				cout << "The Euclidean similarity metric will be used\n";
			}
			else if(metricType == sqdiff)
			{
				cout << "The Squared Difference similarity metric will be used\n";
			}
			else if(metricType == manhatten)
			{
				cout << "The Manhatten (taxi cab) similarity metric will be used\n";
			}
			else if(metricType == correlation)
			{
				cout << "The Correlation Coefficent similarity metric will be used\n";
			}
			else 
			{
				throw RSGISException("Similarity Metric Unknown");
			}
			cout << "Window Size = " << windowSize << endl;
			cout << "Search area = " << searchArea << endl;
			cout << "Metric Threshold = " << metricThreshold << endl;
			cout << "Std Dev reference threshold = " << stdDevRefThreshold << endl;
			cout << "Std Dev floating threshold = " << stdDevFloatThreshold << endl;
			cout << "Sub pixel resolution = " << subPixelResolution << endl;
			cout << "Distance threshold = " << distanceThreshold << endl;
			cout << "Max. number of iterations = " << maxNumIterations << endl;
			cout << "Movement Threshold = " << moveChangeThreshold << endl;
			cout << "p Smoothness = " << pSmoothness << endl;
		}
		else if(this->option == RSGISExeImageRegistration::triangularwarp)
		{
			cout << "Warp an image using a set of image to map ground control points using a triangulation\n";
			cout << "GCPs: " << this->inputGCPs << endl;
			cout << "Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Projection: " << this->projFile << endl;
			cout << "Output Resolution: " << this->resolution << endl;
            cout << "Output Image format: " << this->outImageFormat << endl;
		}
		else if(this->option == RSGISExeImageRegistration::nnwarp)
		{
			cout << "Warp an image using a set of image to map ground control points using a NN algorithm\n";
			cout << "GCPs: " << this->inputGCPs << endl;
			cout << "Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Projection: " << this->projFile << endl;
			cout << "Output Resolution: " << this->resolution << endl;
            cout << "Output Image format: " << this->outImageFormat << endl;
		}
		else
		{
			throw RSGISException("RSGISExeImageRegistration does not know this option");
		}
		
	}
	else
	{
		throw RSGISException("The parameters have yet to be parsed");
	}
}

void RSGISExeImageRegistration::help()
{
	cout << "<rsgis:commands>\n";
	
	cout << "</rsgis:commands>\n";
}

string RSGISExeImageRegistration::getDescription()
{
	return "Provides a set of tools for automatically generating ground control points (GCPs) from image data.";
}

string RSGISExeImageRegistration::getXMLSchema()
{
	return "NOT PROVIDED!";
}

RSGISExeImageRegistration::~RSGISExeImageRegistration()
{
	
}



