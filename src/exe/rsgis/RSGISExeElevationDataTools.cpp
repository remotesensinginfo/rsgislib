/*
 *  RSGISExeElevationDataTools.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/08/2011.
 *  Copyright 2011 RSGISLib.
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

#include "RSGISExeElevationDataTools.h"

namespace rsgisexe{

RSGISExeElevationDataTools::RSGISExeElevationDataTools() : RSGISAlgorithmParameters()
{
	this->algorithm = "elevation";
}

rsgis::RSGISAlgorithmParameters* RSGISExeElevationDataTools::getInstance()
{
	return new RSGISExeElevationDataTools();
}

void RSGISExeElevationDataTools::retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException)
{
	rsgis::math::RSGISMathsUtils mathUtils;
	XMLCh *algorName = xercesc::XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = xercesc::XMLString::transcode("algor");
	XMLCh *optionXMLStr = xercesc::XMLString::transcode("option");
	XMLCh *optionSlope = xercesc::XMLString::transcode("slope");
    XMLCh *optionAspect = xercesc::XMLString::transcode("aspect");
    XMLCh *optionSlopeAspect = xercesc::XMLString::transcode("slopeaspect");
    XMLCh *optionHillShade = xercesc::XMLString::transcode("hillshade");
    XMLCh *optionShadowMask = xercesc::XMLString::transcode("shadowmask");
    XMLCh *optionIncidenceAngle = xercesc::XMLString::transcode("incidenceangle");
    XMLCh *optionExitanceAngle = xercesc::XMLString::transcode("exitanceangle");
    XMLCh *optionIncidenceExistanceAngles = xercesc::XMLString::transcode("incidenceexitanceangles");    
    XMLCh *optionFill = xercesc::XMLString::transcode("fill");
    XMLCh *optionInFillDLayers = xercesc::XMLString::transcode("infilldlayers");
    
	try
	{
		const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
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
	        }
	        else if(xercesc::XMLString::equals(dtUInt16, dtXMLValue))
	        {
	            this->outDataType = GDT_UInt16;
	        }
	        else if(xercesc::XMLString::equals(dtInt16, dtXMLValue))
	        {
	            this->outDataType = GDT_Int16;
	        }
	        else if(xercesc::XMLString::equals(dtUInt32, dtXMLValue))
	        {
	            this->outDataType = GDT_UInt32;
	        }
	        else if(xercesc::XMLString::equals(dtInt32, dtXMLValue))
	        {
	            this->outDataType = GDT_Int32;
	        }
	        else if(xercesc::XMLString::equals(dtFloat32, dtXMLValue))
	        {
	            this->outDataType = GDT_Float32;
	        }
	        else if(xercesc::XMLString::equals(dtFloat64, dtXMLValue))
	        {
	            this->outDataType = GDT_Float64;
	        }
	        else
	        {
	            std::cerr << "Data type not recognised, defaulting to 32 bit float.";
	            this->outDataType = GDT_Float32;
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
		if(xercesc::XMLString::equals(optionSlope, optionXML))
		{
            this->option = slope;
            
            XMLCh *inputXMLStr = xercesc::XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'input\' attribute was provided.");
            }
            xercesc::XMLString::release(&inputXMLStr);
            
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
            
            XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                imageBand = 1;
            }
            xercesc::XMLString::release(&bandXMLStr);
            
            XMLCh *outTypeXMLStr = xercesc::XMLString::transcode("outtype");
            if(argElement->hasAttribute(outTypeXMLStr))
            {
                XMLCh *radiansStr = xercesc::XMLString::transcode("radians");
                const XMLCh *outTypeValue = argElement->getAttribute(outTypeXMLStr);
                
                if(xercesc::XMLString::equals(outTypeValue, radiansStr))
                {
                    this->slopeOutputType = 1;
                }
                else
                {
                    this->slopeOutputType = 0;
                }
                xercesc::XMLString::release(&radiansStr);
            }
            else
            {
                this->slopeOutputType = 0;
            }
            xercesc::XMLString::release(&outTypeXMLStr);
        }
        else if(xercesc::XMLString::equals(optionAspect, optionXML))
		{
            this->option = aspect;
            
            XMLCh *inputXMLStr = xercesc::XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'input\' attribute was provided.");
            }
            xercesc::XMLString::release(&inputXMLStr);
            
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
            
            XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                imageBand = 1;
            }
            xercesc::XMLString::release(&bandXMLStr);
        }
        else if(xercesc::XMLString::equals(optionSlopeAspect, optionXML))
		{
            this->option = slopeaspect;
            
            XMLCh *inputXMLStr = xercesc::XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'input\' attribute was provided.");
            }
            xercesc::XMLString::release(&inputXMLStr);
            
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
            
            XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                imageBand = 1;
            }
            xercesc::XMLString::release(&bandXMLStr);
        }
        else if(xercesc::XMLString::equals(optionHillShade, optionXML))
		{
            this->option = hillshade;
            
            XMLCh *inputXMLStr = xercesc::XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'input\' attribute was provided.");
            }
            xercesc::XMLString::release(&inputXMLStr);
            
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
            
            XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                imageBand = 1;
            }
            xercesc::XMLString::release(&bandXMLStr);
            
            XMLCh *zenithXMLStr = xercesc::XMLString::transcode("zenith");
            if(argElement->hasAttribute(zenithXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(zenithXMLStr));
                this->solarZenith = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                this->solarZenith = 45;
            }
            xercesc::XMLString::release(&zenithXMLStr);
            
            XMLCh *azimuthXMLStr = xercesc::XMLString::transcode("azimuth");
            if(argElement->hasAttribute(azimuthXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(azimuthXMLStr));
                this->solarAzimuth = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                this->solarAzimuth = 315;
            }
            xercesc::XMLString::release(&azimuthXMLStr);
        }
        else if(xercesc::XMLString::equals(optionShadowMask, optionXML))
		{
            this->option = shadowmask;
            
            XMLCh *inputXMLStr = xercesc::XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'input\' attribute was provided.");
            }
            xercesc::XMLString::release(&inputXMLStr);
            
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
            
            XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                imageBand = 1;
            }
            xercesc::XMLString::release(&bandXMLStr);
            
            XMLCh *zenithXMLStr = xercesc::XMLString::transcode("zenith");
            if(argElement->hasAttribute(zenithXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(zenithXMLStr));
                this->solarZenith = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'zenith\' attribute was provided.");
            }
            xercesc::XMLString::release(&zenithXMLStr);
            
            XMLCh *azimuthXMLStr = xercesc::XMLString::transcode("azimuth");
            if(argElement->hasAttribute(azimuthXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(azimuthXMLStr));
                this->solarAzimuth = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'azimuth\' attribute was provided.");
            }
            xercesc::XMLString::release(&azimuthXMLStr);

            XMLCh *maxElevationXMLStr = xercesc::XMLString::transcode("maxelevation");
            if(argElement->hasAttribute(maxElevationXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maxElevationXMLStr));
                this->maxElevHeight = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                std::cerr << "Warning: A default maximum elevation value of 4000 m is being used.";
                this->maxElevHeight = 4000;
            }
            xercesc::XMLString::release(&maxElevationXMLStr);

        }
        else if(xercesc::XMLString::equals(optionIncidenceAngle, optionXML))
        {
            this->option = incidenceangle;
            
            XMLCh *inputXMLStr = xercesc::XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'input\' attribute was provided.");
            }
            xercesc::XMLString::release(&inputXMLStr);
            
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
            
            XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                imageBand = 1;
            }
            xercesc::XMLString::release(&bandXMLStr);
            
            XMLCh *zenithXMLStr = xercesc::XMLString::transcode("zenith");
            if(argElement->hasAttribute(zenithXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(zenithXMLStr));
                this->solarZenith = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'zenith\' attribute was provided.");
            }
            xercesc::XMLString::release(&zenithXMLStr);
            
            XMLCh *azimuthXMLStr = xercesc::XMLString::transcode("azimuth");
            if(argElement->hasAttribute(azimuthXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(azimuthXMLStr));
                this->solarAzimuth = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'azimuth\' attribute was provided.");
            }
            xercesc::XMLString::release(&azimuthXMLStr);
        }
        else if(xercesc::XMLString::equals(optionExitanceAngle, optionXML))
        {
            this->option = exitanceangle;
            
            XMLCh *inputXMLStr = xercesc::XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'input\' attribute was provided.");
            }
            xercesc::XMLString::release(&inputXMLStr);
            
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
            
            XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                imageBand = 1;
            }
            xercesc::XMLString::release(&bandXMLStr);
            
            XMLCh *zenithXMLStr = xercesc::XMLString::transcode("zenith");
            if(argElement->hasAttribute(zenithXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(zenithXMLStr));
                this->viewZenith = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'zenith\' attribute was provided.");
            }
            xercesc::XMLString::release(&zenithXMLStr);
            
            XMLCh *azimuthXMLStr = xercesc::XMLString::transcode("azimuth");
            if(argElement->hasAttribute(azimuthXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(azimuthXMLStr));
                this->viewAzimuth = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'azimuth\' attribute was provided.");
            }
            xercesc::XMLString::release(&azimuthXMLStr);
        }
        else if(xercesc::XMLString::equals(optionIncidenceExistanceAngles, optionXML))
        {
            this->option = incidenceexistanceangles;
            
            XMLCh *inputXMLStr = xercesc::XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'input\' attribute was provided.");
            }
            xercesc::XMLString::release(&inputXMLStr);
            
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
            
            XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                imageBand = 1;
            }
            xercesc::XMLString::release(&bandXMLStr);
            
            XMLCh *solarZenithXMLStr = xercesc::XMLString::transcode("solarZenith");
            if(argElement->hasAttribute(solarZenithXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(solarZenithXMLStr));
                this->solarZenith = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'solarZenith\' attribute was provided.");
            }
            xercesc::XMLString::release(&solarZenithXMLStr);
            
            XMLCh *solarAzimuthXMLStr = xercesc::XMLString::transcode("solarAzimuth");
            if(argElement->hasAttribute(solarAzimuthXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(solarAzimuthXMLStr));
                this->solarAzimuth = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'solarAzimuth\' attribute was provided.");
            }
            xercesc::XMLString::release(&solarAzimuthXMLStr);
            
            XMLCh *viewZenithXMLStr = xercesc::XMLString::transcode("viewZenith");
            if(argElement->hasAttribute(viewZenithXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(viewZenithXMLStr));
                this->viewZenith = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'viewZenith\' attribute was provided.");
            }
            xercesc::XMLString::release(&viewZenithXMLStr);
            
            XMLCh *viewAzimuthXMLStr = xercesc::XMLString::transcode("viewAzimuth");
            if(argElement->hasAttribute(viewAzimuthXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(viewAzimuthXMLStr));
                this->viewAzimuth = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'viewAzimuth\' attribute was provided.");
            }
            xercesc::XMLString::release(&viewAzimuthXMLStr);
        }
        else if(xercesc::XMLString::equals(optionFill, optionXML))
        {
            this->option = fill;
            
            XMLCh *inputXMLStr = xercesc::XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'input\' attribute was provided.");
            }
            xercesc::XMLString::release(&inputXMLStr);
            
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
            
            XMLCh *holesValueXMLStr = xercesc::XMLString::transcode("holesval");
            if(argElement->hasAttribute(holesValueXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(holesValueXMLStr));
                this->holesValue = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'holesval\' attribute was provided.");
            }
            xercesc::XMLString::release(&holesValueXMLStr);
            
            
            XMLCh *noDataValueXMLStr = xercesc::XMLString::transcode("nodata");
            if(argElement->hasAttribute(noDataValueXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(noDataValueXMLStr));
                this->noDataVal = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'nodata\' attribute was provided.");
            }
            xercesc::XMLString::release(&noDataValueXMLStr);
            
            
        }
        else if(xercesc::XMLString::equals(optionInFillDLayers, optionXML))
        {
            this->option = infilldlayers;
            
            XMLCh *inputBaseXMLStr = xercesc::XMLString::transcode("baseimage");
            if(argElement->hasAttribute(inputBaseXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputBaseXMLStr));
                this->inputImageBase = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'baseimage\' attribute was provided.");
            }
            xercesc::XMLString::release(&inputBaseXMLStr);
            
            
            XMLCh *inputInFillXMLStr = xercesc::XMLString::transcode("infillimage");
            if(argElement->hasAttribute(inputInFillXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputInFillXMLStr));
                this->inputImageInFill = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'infillimage\' attribute was provided.");
            }
            xercesc::XMLString::release(&inputInFillXMLStr);
            
            XMLCh *inputDEMXMLStr = xercesc::XMLString::transcode("dem");
            if(argElement->hasAttribute(inputDEMXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inputDEMXMLStr));
                this->inputDEM = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'dem\' attribute was provided.");
            }
            xercesc::XMLString::release(&inputDEMXMLStr);
            
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
            
            XMLCh *holesValueXMLStr = xercesc::XMLString::transcode("holesval");
            if(argElement->hasAttribute(holesValueXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(holesValueXMLStr));
                this->holesValue = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'holesval\' attribute was provided.");
            }
            xercesc::XMLString::release(&holesValueXMLStr);
            
        }
		else 
		{
			std::string message = "RSGISExeElevationDataTools did not recognise option " + std::string(xercesc::XMLString::transcode(optionXML));
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
	xercesc::XMLString::release(&optionSlope);
    xercesc::XMLString::release(&optionAspect);
    xercesc::XMLString::release(&optionSlopeAspect);
    xercesc::XMLString::release(&optionHillShade);
    xercesc::XMLString::release(&optionShadowMask);
    xercesc::XMLString::release(&optionIncidenceAngle);
    xercesc::XMLString::release(&optionExitanceAngle);
    xercesc::XMLString::release(&optionIncidenceExistanceAngles);
    xercesc::XMLString::release(&optionFill);
    xercesc::XMLString::release(&optionInFillDLayers);
	
	parsed = true; // if all successful, it is parsed
}

void RSGISExeElevationDataTools::runAlgorithm() throw(rsgis::RSGISException)
{
	std::cout.precision(10);
	
	if(!parsed)
	{
		throw rsgis::RSGISException("Before running the parameters must be retrieved");
	}
	else
	{
		if(this->option == RSGISExeElevationDataTools::slope)
        {
            std::cout << "This command calculates the slope from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISCalcSlope *calcSlope = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				std::cout << "Open " << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand > numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw rsgis::RSGISException("The specified image band is not within the input image.");
                }
                
                double *transformation = new double[6];
                datasets[0]->GetGeoTransform(transformation);
                
                float imageEWRes = transformation[1];
                float imageNSRes = transformation[5];
                
                if(imageNSRes < 0)
                {
                    imageNSRes = imageNSRes * (-1);
                }
                
                delete[] transformation;
                
				calcSlope = new rsgis::calib::RSGISCalcSlope(1, imageBand-1, imageEWRes, imageNSRes, slopeOutputType);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcSlope, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3, this->imageFormat, this->outDataType);

				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcSlope;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::aspect)
        {
            std::cout << "This command calculates the aspect from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISCalcAspect *calcAspect = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				std::cout << "Open " << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand > numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw rsgis::RSGISException("The specified image band is not within the input image.");
                }
                
                double *transformation = new double[6];
                datasets[0]->GetGeoTransform(transformation);
                
                float imageEWRes = transformation[1];
                float imageNSRes = transformation[5];
                
                if(imageNSRes < 0)
                {
                    imageNSRes = imageNSRes * (-1);
                }
                
                delete[] transformation;
                
				calcAspect = new rsgis::calib::RSGISCalcAspect(1, imageBand-1, imageEWRes, imageNSRes);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcAspect, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3, this->imageFormat, this->outDataType);
                
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcAspect;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::slopeaspect)
        {
            std::cout << "This command calculates the slope and aspect from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISCalcSlopeAspect *calcSlopeAspect = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				std::cout << "Open " << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand > numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw rsgis::RSGISException("The specified image band is not within the input image.");
                }
                
                double *transformation = new double[6];
                datasets[0]->GetGeoTransform(transformation);
                
                float imageEWRes = transformation[1];
                float imageNSRes = transformation[5];
                
                if(imageNSRes < 0)
                {
                    imageNSRes = imageNSRes * (-1);
                }
                
                delete[] transformation;
                
				calcSlopeAspect = new rsgis::calib::RSGISCalcSlopeAspect(2, imageBand-1, imageEWRes, imageNSRes);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcSlopeAspect, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3, this->imageFormat, this->outDataType);
                
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcSlopeAspect;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::hillshade)
        {
            std::cout << "This command generates a hill shade image from the DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            std::cout << "Solar Azimuth: " << this->solarAzimuth << std::endl;
            std::cout << "Solar Zenith: " << this->solarZenith << std::endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISCalcHillShade *calcHillShade = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				std::cout << "Open " << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand > numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw rsgis::RSGISException("The specified image band is not within the input image.");
                }
                
                double *transformation = new double[6];
                datasets[0]->GetGeoTransform(transformation);
                
                float imageEWRes = transformation[1];
                float imageNSRes = transformation[5];
                
                if(imageNSRes < 0)
                {
                    imageNSRes = imageNSRes * (-1);
                }
                
                delete[] transformation;
                
				calcHillShade = new rsgis::calib::RSGISCalcHillShade(1, imageBand-1, imageEWRes, imageNSRes, solarZenith, solarAzimuth);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcHillShade, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3, this->imageFormat, this->outDataType);
                
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcHillShade;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::shadowmask)
        {
            std::cout << "This command calculates a shadow mask from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            std::cout << "Solar Azimuth: " << this->solarAzimuth << std::endl;
            std::cout << "Solar Zenith: " << this->solarZenith << std::endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISCalcShadowBinaryMask *calcMaskShadow = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			try
			{
                if((this->solarZenith < 0) | (this->solarZenith > 90))
                {
                    throw rsgis::RSGISException("The solar zenith should be between 0 and 90 degrees.");
                }
                
                if((this->solarAzimuth < 0) | (this->solarAzimuth > 360))
                {
                    throw rsgis::RSGISException("The solar azimuth should be between 0 and 360 degrees.");
                }
                
                this->solarZenith = 90 - this->solarZenith;
                
				datasets = new GDALDataset*[1];
				
				std::cout << "Open " << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand > numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw rsgis::RSGISException("The specified image band is not within the input image.");
                }
                
                double *transformation = new double[6];
                datasets[0]->GetGeoTransform(transformation);
                
                float imageEWRes = transformation[1];
                float imageNSRes = transformation[5];
                
                if(imageNSRes < 0)
                {
                    imageNSRes = imageNSRes * (-1);
                }
                
                delete[] transformation;
                
				calcMaskShadow = new rsgis::calib::RSGISCalcShadowBinaryMask(1, datasets[0], imageBand-1, imageEWRes, imageNSRes, solarZenith, solarAzimuth, maxElevHeight);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcMaskShadow, "", true);
				calcImage->calcImageExtent(datasets, 1, outputImage, this->imageFormat);
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcMaskShadow;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::incidenceangle)
        {
            std::cout << "This command calculate the incidence angle of light (from the provided zenith and azimuth) from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            std::cout << "Solar Azimuth: " << this->solarAzimuth << std::endl;
            std::cout << "Solar Zenith: " << this->solarZenith << std::endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISCalcRayIncidentAngle *calcIncidence = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				std::cout << "Open " << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand > numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw rsgis::RSGISException("The specified image band is not within the input image.");
                }
                
                double *transformation = new double[6];
                datasets[0]->GetGeoTransform(transformation);
                
                float imageEWRes = transformation[1];
                float imageNSRes = transformation[5];
                
                if(imageNSRes < 0)
                {
                    imageNSRes = imageNSRes * (-1);
                }
                
                delete[] transformation;
                
				calcIncidence = new rsgis::calib::RSGISCalcRayIncidentAngle(1, imageBand-1, imageEWRes, imageNSRes, solarZenith, solarAzimuth);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcIncidence, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3, this->imageFormat, this->outDataType);
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcIncidence;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::exitanceangle)
        {
            std::cout << "This command calculate the exitance angle of light (from the provided zenith and azimuth) from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            std::cout << "View Azimuth: " << this->viewAzimuth << std::endl;
            std::cout << "View Zenith: " << this->viewZenith << std::endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISCalcRayExitanceAngle *calcIncidence = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				std::cout << "Open " << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand > numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw rsgis::RSGISException("The specified image band is not within the input image.");
                }
                
                double *transformation = new double[6];
                datasets[0]->GetGeoTransform(transformation);
                
                float imageEWRes = transformation[1];
                float imageNSRes = transformation[5];
                
                if(imageNSRes < 0)
                {
                    imageNSRes = imageNSRes * (-1);
                }
                
                delete[] transformation;
                
				calcIncidence = new rsgis::calib::RSGISCalcRayExitanceAngle(1, imageBand-1, imageEWRes, imageNSRes, solarZenith, solarAzimuth);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcIncidence, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3);
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcIncidence;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::incidenceexistanceangles)
        {
            std::cout << "This command calculate the incidence angle of light (from the provided zenith and azimuth) from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            std::cout << "Solar Azimuth: " << this->solarAzimuth << std::endl;
            std::cout << "Solar Zenith: " << this->solarZenith << std::endl;
            std::cout << "View Azimuth: " << this->viewAzimuth << std::endl;
            std::cout << "View Zenith: " << this->viewZenith << std::endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISCalcRayIncidentAndExitanceAngles *calcIncidence = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				std::cout << "Open " << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand > numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw rsgis::RSGISException("The specified image band is not within the input image.");
                }
                
                double *transformation = new double[6];
                datasets[0]->GetGeoTransform(transformation);
                
                float imageEWRes = transformation[1];
                float imageNSRes = transformation[5];
                
                if(imageNSRes < 0)
                {
                    imageNSRes = imageNSRes * (-1);
                }
                
                delete[] transformation;
                
				calcIncidence = new rsgis::calib::RSGISCalcRayIncidentAndExitanceAngles(2, imageBand-1, imageEWRes, imageNSRes, solarZenith, solarAzimuth, viewZenith, viewAzimuth);
				
				calcImage = new rsgis::img::RSGISCalcImage(calcIncidence, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3);
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcIncidence;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::fill)
        {
            std::cout << "This command fill the holes within a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Holes Value: " << this->holesValue << std::endl;
            std::cout << "No Data Value: " << this->noDataVal << std::endl;
            
            GDALAllRegister();
			
			try
			{			
				std::cout << "Open " << this->inputImage << std::endl;
				GDALDataset *dataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImage;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
				// Copy the input image
                rsgis::img::RSGISImageUtils imageUtils;
                GDALDataset *outImage = imageUtils.createCopy(dataset, 1, this->outputImage, this->imageFormat, this->outDataType);
                imageUtils.copyFloatGDALDataset(dataset, outImage);
                
                rsgis::calib::RSGISFillDEMHoles *calcFillHoles = new rsgis::calib::RSGISFillDEMHoles(this->holesValue, this->noDataVal);
                rsgis::img::RSGISCalcEditImage calcEditImage = rsgis::img::RSGISCalcEditImage(calcFillHoles);
                
                bool change = true;
                while(change)
                {
                    calcFillHoles->resetChange();
                    calcEditImage.calcImageWindowData(outImage, 3, this->noDataVal);
                    change = calcFillHoles->changeOccurred();
                }

                
                // Clean up memory.
                GDALClose(outImage);
                GDALClose(dataset);
                delete calcFillHoles;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::infilldlayers)
        {
            std::cout << "This command in fill the holes within layers derived from a DEM (i.e., slope and aspect).\n";
			std::cout << "Input Base Image: " << this->inputImageBase << std::endl;
            std::cout << "Input In Fill Image: " << this->inputImageInFill << std::endl;
            std::cout << "Input DEM: " << this->inputDEM << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Holes Value: " << this->holesValue << std::endl;
            
            GDALAllRegister();
			
			try
			{
				std::cout << "Open " << this->inputImage << std::endl;
				GDALDataset **datasets = new GDALDataset*[3];
                datasets[0] = (GDALDataset *) GDALOpen(this->inputDEM.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputDEM;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                datasets[1] = (GDALDataset *) GDALOpen(this->inputImageBase.c_str(), GA_ReadOnly);
				if(datasets[1] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImageBase;
					throw rsgis::RSGISImageException(message.c_str());
				}
                
                datasets[2] = (GDALDataset *) GDALOpen(this->inputImageInFill.c_str(), GA_ReadOnly);
				if(datasets[2] == NULL)
				{
					std::string message = std::string("Could not open image ") + this->inputImageInFill;
					throw rsgis::RSGISImageException(message.c_str());
				}
				
                rsgis::calib::RSGISInFillDerivedHoles *calcFillDHole = new rsgis::calib::RSGISInFillDerivedHoles(this->holesValue);
                rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcFillDHole, "", true);
                calcImage.calcImageWindowData(datasets, 3, this->outputImage, 3, this->imageFormat, this->outDataType);
                
                // Clean up memory.
                GDALClose(datasets[0]);
                GDALClose(datasets[1]);
                GDALClose(datasets[2]);
                delete[] datasets;
                delete calcFillDHole;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
		else
		{
			throw rsgis::RSGISException("RSGISExeElevationDataTools does not know this option");
		}
	}
}


void RSGISExeElevationDataTools::printParameters()
{
	std::cout.precision(10);
	
	if(!parsed)
	{
		throw rsgis::RSGISException("Before running the parameters must be retrieved");
	}
	else
	{
		if(this->option == RSGISExeElevationDataTools::slope)
        {
            std::cout << "This command calculates the slope from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
        }
        else if(this->option == RSGISExeElevationDataTools::aspect)
        {
            std::cout << "This command calculates the aspect from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
        }
        else if(this->option == RSGISExeElevationDataTools::slopeaspect)
        {
            std::cout << "This command calculates the slope and aspect from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
        }
        else if(this->option == RSGISExeElevationDataTools::hillshade)
        {
            std::cout << "This command generates a hill shade imnage from the DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            std::cout << "Solar Azimuth: " << this->solarAzimuth << std::endl;
            std::cout << "Solar Zenith: " << this->solarZenith << std::endl;
        }
        else if(this->option == RSGISExeElevationDataTools::shadowmask)
        {
            std::cout << "This command calculates a shadow mask from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            std::cout << "Solar Azimuth: " << this->solarAzimuth << std::endl;
            std::cout << "Solar Zenith: " << this->solarZenith << std::endl;
        }
        else if(this->option == RSGISExeElevationDataTools::incidenceangle)
        {
            std::cout << "This command calculate the incidence angle of light (from the provided zenith and azimuth) from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            std::cout << "Solar Azimuth: " << this->solarAzimuth << std::endl;
            std::cout << "Solar Zenith: " << this->solarZenith << std::endl;
        }
        else if(this->option == RSGISExeElevationDataTools::exitanceangle)
        {
            std::cout << "This command calculate the exitance angle of light (from the provided zenith and azimuth) from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            std::cout << "View Azimuth: " << this->viewAzimuth << std::endl;
            std::cout << "View Zenith: " << this->viewZenith << std::endl;
        }
        else if(this->option == RSGISExeElevationDataTools::incidenceexistanceangles)
        {
            std::cout << "This command calculate the incidence angle of light (from the provided zenith and azimuth) from a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Input Band: " << this->imageBand << std::endl;
            std::cout << "Solar Azimuth: " << this->solarAzimuth << std::endl;
            std::cout << "Solar Zenith: " << this->solarZenith << std::endl;
            std::cout << "View Azimuth: " << this->viewAzimuth << std::endl;
            std::cout << "View Zenith: " << this->viewZenith << std::endl;
        }
        else if(this->option == RSGISExeElevationDataTools::fill)
        {
            std::cout << "This command fill the holes within a DEM.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Holes Value: " << this->holesValue << std::endl;
            std::cout << "No Data Value: " << this->noDataVal << std::endl;
        }
        else if(this->option == RSGISExeElevationDataTools::infilldlayers)
        {
            std::cout << "This command in fill the holes within layers derived from a DEM (i.e., slope and aspect).\n";
			std::cout << "Input Base Image: " << this->inputImageBase << std::endl;
            std::cout << "Input In Fill Image: " << this->inputImageInFill << std::endl;
            std::cout << "Input DEM: " << this->inputDEM << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Holes Value: " << this->holesValue << std::endl;
        }
		else
		{
			throw rsgis::RSGISException("RSGISExeImageCalibration does not know this option");
		}
	}
}

void RSGISExeElevationDataTools::help()
{
    std::cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << std::endl;
	std::cout << "<!-- A command to calculate the slope of an elevation surface -->" << std::endl;
    std::cout << "<rsgis:command algor=\"elevation\" option=\"slope\" input=\"image.env\" output=\"out_image.env\" [band=\"int\"] [outtype=\"radians|degrees\"] />" << std::endl;
    std::cout << "<!-- A command to calculate the aspect of an elevation surface -->" << std::endl;
    std::cout << "<rsgis:command algor=\"elevation\" option=\"aspect\" input=\"image.env\" output=\"out_image.env\" [band=\"int\"] />" << std::endl;
    std::cout << "<!-- A command to calculate the slope and aspect of an elevation surface -->" << std::endl;
    std::cout << "<rsgis:command algor=\"elevation\" option=\"slopeaspect\" input=\"image.env\" output=\"out_image.env\" [band=\"int\"] />" << std::endl;
    std::cout << "<!-- A command to calculate the hillshade of an elevation surface -->" << std::endl;
    std::cout << "<rsgis:command algor=\"elevation\" option=\"hillshade\" input=\"image.env\" output=\"out_image.env\" azimuth=\"float\" zenith=\"float\" [band=\"int\"]  />" << std::endl;
    std::cout << "<!-- A command to calculate a mask for the regions of shadow from an elevation surface -->" << std::endl;
    std::cout << "<rsgis:command algor=\"elevation\" option=\"shadowmask\" input=\"image.env\" output=\"out_image.env\" azimuth=\"float\" zenith=\"float\" maxelevation=\"int\" [band=\"int\"]  />" << std::endl;
    std::cout << "<!-- A command to calculate the incidence angle from a elevation surface and sun position -->" << std::endl;
    std::cout << "<rsgis:command algor=\"elevation\" option=\"incidenceangle\" input=\"image.env\" output=\"out_image.env\" azimuth=\"float\" zenith=\"float\" [band=\"int\"]  />" << std::endl;
    std::cout << "<!-- A command to calculate the exitance angle from a elevation surface and viewer position -->" << std::endl;
    std::cout << "<rsgis:command algor=\"elevation\" option=\"exitanceangle\" input=\"image.env\" output=\"out_image.env\" azimuth=\"float\" zenith=\"float\" [band=\"int\"]  />" << std::endl;
    std::cout << "<!-- A command to calculate the incidence and exitance angles from a elevation surface and sun and viewer positions -->" << std::endl;
    std::cout << "<rsgis:command algor=\"elevation\" option=\"incidenceexitanceangles\" input=\"image.env\" output=\"out_image.env\" solarAzimuth=\"float\" solarZenith=\"float\" viewAzimuth=\"float\" viewZenith=\"float\" [band=\"int\"]  />" << std::endl;

	std::cout << "</rsgis:commands>\n";
}

std::string RSGISExeElevationDataTools::getDescription()
{
	return "Provides a set of tools for processsing elevation (DEMs) data.";
}

std::string RSGISExeElevationDataTools::getXMLSchema()
{
	return "NOT PROVIDED!";
}

RSGISExeElevationDataTools::~RSGISExeElevationDataTools()
{
	
}

}

