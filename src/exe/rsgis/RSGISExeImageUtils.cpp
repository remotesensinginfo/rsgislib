/*
 *  RSGISExeImageUtils.cpp
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

#include "RSGISExeImageUtils.h"

namespace rsgisexe{

RSGISExeImageUtils::RSGISExeImageUtils() : RSGISAlgorithmParameters()
{
	this->algorithm = "imageutils";
	
	this->option = RSGISExeImageUtils::none;
	this->interpolator = RSGISExeImageUtils::cubic;
	
	this->inputImage = "";
	this->outputImage = "";
	this->inputImages = NULL;
	this->inputVector = "";
	this->filenameAttribute = "";
	this->imageMask = "";
	this->inputDIR = "";
	this->outputDIR = "";
	this->proj = "";
	this->classColour = NULL;
	this->nodataValue = 0;
	this->maskValue = 0;
	this->resampleScale = 1;
	this->numImages = 0;
	this->numClasses = 0;
	this->projFromImage = true;
    this->imageFormat = "ENVI";
    this->outDataType = GDT_Float32;
}

    rsgis::RSGISAlgorithmParameters* RSGISExeImageUtils::getInstance()
{
	return new RSGISExeImageUtils();
}

void RSGISExeImageUtils::retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException)
{
    rsgis::math::RSGISMathsUtils mathUtils;
    rsgis::utils::RSGISFileUtils fileUtils;
    rsgis::utils::RSGISTextUtils textUtils;
	
	XMLCh *algorName = xercesc::XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = xercesc::XMLString::transcode("algor");
	XMLCh *optionXMLStr = xercesc::XMLString::transcode("option");
	XMLCh *optionColour = xercesc::XMLString::transcode("colourimage");
	XMLCh *optionMosaic = xercesc::XMLString::transcode("mosaic");
	XMLCh *optionInclude = xercesc::XMLString::transcode("include");
	XMLCh *optionCut2Poly = xercesc::XMLString::transcode("cut2poly");
    XMLCh *optionCut2Polys = xercesc::XMLString::transcode("cut2polys");
	XMLCh *optionMask = xercesc::XMLString::transcode("mask");
	XMLCh *optionResample = xercesc::XMLString::transcode("resample");
	XMLCh *optionRasteriseDef = xercesc::XMLString::transcode("rasterisedefiniens");
	XMLCh *projImage = xercesc::XMLString::transcode("IMAGE");
	XMLCh *projOSGB = xercesc::XMLString::transcode("OSGB");
    XMLCh *projNZ2000 = xercesc::XMLString::transcode("NZ2000");
    XMLCh *projNZ1949 = xercesc::XMLString::transcode("NZ1949");
	XMLCh *rsgisimageXMLStr = xercesc::XMLString::transcode("rsgis:image");
	XMLCh *optionPrintProj4 = xercesc::XMLString::transcode("printProj4");
	XMLCh *optionPrintWKT = xercesc::XMLString::transcode("printWKT");
	XMLCh *optionExtract2DScatterPtxt = xercesc::XMLString::transcode("extract2dscatterptxt");
	XMLCh *optionSGSmoothing = xercesc::XMLString::transcode("sgsmoothing");
	XMLCh *optionCumulativeArea = xercesc::XMLString::transcode("cumulativearea");
	XMLCh *optionCreateImage = xercesc::XMLString::transcode("createimage");
	XMLCh *optionStretchImage = xercesc::XMLString::transcode("stretch");
	XMLCh *optionHueColour = xercesc::XMLString::transcode("huecolour");
	XMLCh *optionRemoveSpatialRef = xercesc::XMLString::transcode("removespatialref");
	XMLCh *optionAddnoise = xercesc::XMLString::transcode("addnoise");
    XMLCh *optionSubset = xercesc::XMLString::transcode("subset");
    XMLCh *optionSubset2Polys = xercesc::XMLString::transcode("subset2polys");
	XMLCh *optionDefineSpatialRef = xercesc::XMLString::transcode("definespatialref");
	XMLCh *optionPanSharpen = xercesc::XMLString::transcode("pansharpen");
    XMLCh *optionColourImageBands = xercesc::XMLString::transcode("colourimagebands");
    XMLCh *optionCreateSlices = xercesc::XMLString::transcode("createslices");
	XMLCh *optionClump = xercesc::XMLString::transcode("clump");
    XMLCh *optionComposite = xercesc::XMLString::transcode("composite");
    XMLCh *optionRelabel = xercesc::XMLString::transcode("relabel");
    XMLCh *optionAssignProj = xercesc::XMLString::transcode("assignproj");
    XMLCh *optionPopImgStats = xercesc::XMLString::transcode("popimgstats");
    XMLCh *optionCreateCopy = xercesc::XMLString::transcode("createcopy");
    XMLCh *optionCreateKMLFile = xercesc::XMLString::transcode("createKMLFile");
    XMLCh *optionAssignSpatialInfo = xercesc::XMLString::transcode("assignspatialinfo");
    
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
            cerr << "Data type not recognised, defaulting to 32 bit float.";
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
	if(xercesc::XMLString::equals(optionColour, optionXML))
	{		
		this->option = RSGISExeImageUtils::colour;
		
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
		
		DOMNodeList *classNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:colour"));
		this->numClasses = classNodesList->getLength();		
		
		std::cout << "Found " << this->numClasses << " Classes \n";
		
		xercesc::DOMElement *classElement = NULL;
		classColour = new rsgis::img::ClassColour*[numClasses];
		for(int i = 0; i < numClasses; i++)
		{
			classColour[i] = new rsgis::img::ClassColour();
			classElement = static_cast<xercesc::DOMElement*>(classNodesList->item(i));
			
			XMLCh *nameXMLStr = xercesc::XMLString::transcode("name");
			if(classElement->hasAttribute(nameXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(nameXMLStr));
				classColour[i]->className = std::string(charValue);
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
				classColour[i]->classID = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'id\' attribute was provided.");
			}
			xercesc::XMLString::release(&idXMLStr);
			
			XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
			if(classElement->hasAttribute(bandXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(bandXMLStr));
				classColour[i]->imgBand = mathUtils.strtoint(std::string(charValue))-1; // Band refers to the array index not image band
 				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			xercesc::XMLString::release(&bandXMLStr);
			
			XMLCh *lowerXMLStr = xercesc::XMLString::transcode("lower");
			if(classElement->hasAttribute(lowerXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(lowerXMLStr));
				classColour[i]->lower = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'lower\' attribute was provided.");
			}
			xercesc::XMLString::release(&lowerXMLStr);
			
			XMLCh *upperXMLStr = xercesc::XMLString::transcode("upper");
			if(classElement->hasAttribute(upperXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(upperXMLStr));
				classColour[i]->upper = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'upper\' attribute was provided.");
			}
			xercesc::XMLString::release(&upperXMLStr);
			
			XMLCh *redXMLStr = xercesc::XMLString::transcode("red");
			if(classElement->hasAttribute(redXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(redXMLStr));
				classColour[i]->red = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'red\' attribute was provided.");
			}
			xercesc::XMLString::release(&redXMLStr);
			
			XMLCh *greenXMLStr = xercesc::XMLString::transcode("green");
			if(classElement->hasAttribute(greenXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(greenXMLStr));
				classColour[i]->green = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'green\' attribute was provided.");
			}
			xercesc::XMLString::release(&greenXMLStr);
			
			XMLCh *blueXMLStr = xercesc::XMLString::transcode("blue");
			if(classElement->hasAttribute(blueXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(blueXMLStr));
				classColour[i]->blue = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'blue\' attribute was provided.");
			}
			xercesc::XMLString::release(&blueXMLStr);
		}
	}
    else if(xercesc::XMLString::equals(optionColourImageBands, optionXML))
	{		
		this->option = RSGISExeImageUtils::colourimagebands;
		
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
		
		DOMNodeList *classNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:colour"));
		this->numClasses = classNodesList->getLength();		
		
		std::cout << "Found " << this->numClasses << " Classes \n";
		
		xercesc::DOMElement *classElement = NULL;
		classColour = new rsgis::img::ClassColour*[numClasses];
		for(int i = 0; i < numClasses; i++)
		{
			classColour[i] = new rsgis::img::ClassColour();
			classElement = static_cast<xercesc::DOMElement*>(classNodesList->item(i));
			
			XMLCh *nameXMLStr = xercesc::XMLString::transcode("name");
			if(classElement->hasAttribute(nameXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(nameXMLStr));
				classColour[i]->className = std::string(charValue);
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
				classColour[i]->classID = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'id\' attribute was provided.");
			}
			xercesc::XMLString::release(&idXMLStr);
			
			XMLCh *lowerXMLStr = xercesc::XMLString::transcode("lower");
			if(classElement->hasAttribute(lowerXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(lowerXMLStr));
				classColour[i]->lower = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'lower\' attribute was provided.");
			}
			xercesc::XMLString::release(&lowerXMLStr);
			
			XMLCh *upperXMLStr = xercesc::XMLString::transcode("upper");
			if(classElement->hasAttribute(upperXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(upperXMLStr));
				classColour[i]->upper = mathUtils.strtofloat(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'upper\' attribute was provided.");
			}
			xercesc::XMLString::release(&upperXMLStr);
			
			XMLCh *redXMLStr = xercesc::XMLString::transcode("red");
			if(classElement->hasAttribute(redXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(redXMLStr));
				classColour[i]->red = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'red\' attribute was provided.");
			}
			xercesc::XMLString::release(&redXMLStr);
			
			XMLCh *greenXMLStr = xercesc::XMLString::transcode("green");
			if(classElement->hasAttribute(greenXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(greenXMLStr));
				classColour[i]->green = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'green\' attribute was provided.");
			}
			xercesc::XMLString::release(&greenXMLStr);
			
			XMLCh *blueXMLStr = xercesc::XMLString::transcode("blue");
			if(classElement->hasAttribute(blueXMLStr))
			{
				char *charValue = xercesc::XMLString::transcode(classElement->getAttribute(blueXMLStr));
				classColour[i]->blue = mathUtils.strtoint(std::string(charValue));
				xercesc::XMLString::release(&charValue);
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("No \'blue\' attribute was provided.");
			}
			xercesc::XMLString::release(&blueXMLStr);
		}
	}
	else if (xercesc::XMLString::equals(optionMosaic, optionXML))
	{		
		this->option = RSGISExeImageUtils::mosaic;
		this->mosaicSkipVals = false;
		this->mosaicSkipThreash = false;
		this->skipLowerThreash = -numeric_limits<double>::infinity();
		this->skipUpperThreash = +numeric_limits<double>::infinity();
		this->skipBand = 0;
		
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

		XMLCh *nodataXMLStr = xercesc::XMLString::transcode("nodata");
		if(argElement->hasAttribute(nodataXMLStr))
		{
            XMLCh *NaNStr = xercesc::XMLString::transcode("NaN");
			const XMLCh *noDataValue = argElement->getAttribute(nodataXMLStr);
			if(xercesc::XMLString::equals(noDataValue, NaNStr))
			{
                const char *val = "NaN";
				this->nodataValue = nan(val);
			}
			else
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(nodataXMLStr));
                this->nodataValue = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
			}
			xercesc::XMLString::release(&NaNStr);
		}
		else
		{
			//throw rsgis::RSGISXMLArgumentsException("No \'nodata\' attribute was provided.");
			std::cout << "\tUsing default of 0 for background values" << std::endl;
			this->nodataValue = 0;
		}
		xercesc::XMLString::release(&nodataXMLStr);
		
		// Set value in first band to skip, if not set no error will be printed and standard mosaic method will be used.
		XMLCh *skipValueXMLStr = xercesc::XMLString::transcode("skipValue");
		if(argElement->hasAttribute(skipValueXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(skipValueXMLStr));
			this->skipValue = mathUtils.strtofloat(std::string(charValue));
			this->mosaicSkipVals = true;
			xercesc::XMLString::release(&charValue);
		}
		xercesc::XMLString::release(&skipValueXMLStr);
		
		// Set upper threashold to skip in all bands, if not set no error will be printed and standard mosaic method will be used.
		XMLCh *skipUpperThreashXMLStr = xercesc::XMLString::transcode("skipUpperThreash");
		if(argElement->hasAttribute(skipUpperThreashXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(skipUpperThreashXMLStr));
			this->skipUpperThreash = mathUtils.strtofloat(std::string(charValue));
			this->mosaicSkipThreash = true;
			xercesc::XMLString::release(&charValue);
			if (mosaicSkipVals) 
			{
				std::cout << "\tCan't use \'skipValue\' with \'skipUpperThreash\', using threashold instead" << std::endl;
			}
		}
		xercesc::XMLString::release(&skipUpperThreashXMLStr);
		
		// Set lower threashold to skip in all bands, if not set no error will be printed and standard mosaic method will be used.
		XMLCh *skipLowerThreashXMLStr = xercesc::XMLString::transcode("skipLowerThreash");
		if(argElement->hasAttribute(skipLowerThreashXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(skipLowerThreashXMLStr));
			this->skipLowerThreash = mathUtils.strtofloat(std::string(charValue));
			this->mosaicSkipThreash = true;
			xercesc::XMLString::release(&charValue);
			if (mosaicSkipVals) 
			{
				std::cout << "\tCan't use \'skipValue\' with \'skipLowerThreash\', using threashold instead" << std::endl;
			}
		}
		xercesc::XMLString::release(&skipLowerThreashXMLStr);
		
		XMLCh *skipBandXMLStr = xercesc::XMLString::transcode("setSkipBand");
		if(argElement->hasAttribute(skipBandXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(skipBandXMLStr));
			this->skipBand = mathUtils.strtofloat(std::string(charValue)) - 1;
			xercesc::XMLString::release(&charValue);
			if (this->mosaicSkipVals) 
			{
				std::cout << "\tSkiping pixels with a value of \'" << this->skipValue << "\' in band \'" << this->skipBand + 1 << "\'" << std::endl;
			}
			else if (this->mosaicSkipThreash) 
			{
				std::cout << "\tSkiping pixels with a value between \'" << this->skipLowerThreash << "\' and \'" << this->skipUpperThreash << "\' in band \'" << this->skipBand + 1<< "\'" << std::endl;
			}
			else 
			{
				std::cout << "\tBand set to define values to skip using \'setSkipBand\' but no value or threasholds set - IGNORING" << std::endl;
			}

		}
		else 
		{
			if (this->mosaicSkipVals) 
			{
				std::cout << "\tSkiping pixels with a value of \'" << this->skipValue << "\' using the first band (default)" << std::endl;
			}
			else if (this->mosaicSkipThreash) 
			{
				std::cout << "\tSkiping pixels with a value between \'" << this->skipLowerThreash << "\' and \'" << this->skipUpperThreash << "\' using the first band (default)" << std::endl;			}
		}

		xercesc::XMLString::release(&skipBandXMLStr);
		
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
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
		
		
		XMLCh *dirXMLStr = xercesc::XMLString::transcode("dir");
		XMLCh *extXMLStr = xercesc::XMLString::transcode("ext");
		if(argElement->hasAttribute(dirXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(dirXMLStr));
			string dirStr = std::string(charValue);
			xercesc::XMLString::release(&charValue);
			
			charValue = xercesc::XMLString::transcode(argElement->getAttribute(extXMLStr));
			string extStr = std::string(charValue);
			xercesc::XMLString::release(&charValue);
			
			try
			{
				this->inputImages = fileUtils.getDIRList(dirStr, extStr, &this->numImages, false);
			}
			catch(rsgis::RSGISException e)
			{
				throw rsgis::RSGISXMLArgumentsException(e.what());
			}
		}
		else
		{
			xercesc::DOMElement *imageElement = NULL;
			
			DOMNodeList *imagesList = argElement->getElementsByTagName(rsgisimageXMLStr);
			this->numImages = imagesList->getLength();
			this->inputImages = new std::string[numImages];
			
			XMLCh *fileXMLStr = xercesc::XMLString::transcode("file");
			for(int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<xercesc::DOMElement*>(imagesList->item(i));
				
				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					this->inputImages[i] = std::string(charValue);
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'file\' attribute was provided.");
				}
			}
			xercesc::XMLString::release(&fileXMLStr);
		}
		xercesc::XMLString::release(&dirXMLStr);
		xercesc::XMLString::release(&extXMLStr);
	}
	else if (xercesc::XMLString::equals(optionInclude, optionXML))
	{		
		this->option = RSGISExeImageUtils::include;
		
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
        
        XMLCh *bandsXMLStr = xercesc::XMLString::transcode("bands");
		if(argElement->hasAttribute(bandsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandsXMLStr));
			string bandsList = std::string(charValue);
			xercesc::XMLString::release(&charValue);
            
            vector<string> *tokens = new vector<string>();
            textUtils.tokenizeString(bandsList, ',', tokens, true, true);
            for(vector<string>::iterator iterTokens = tokens->begin(); iterTokens != tokens->end(); ++iterTokens)
            {
                try 
                {
                    bands.push_back(mathUtils.strtoint(*iterTokens));
                } 
                catch (RSGISMathException &e) 
                {
                    std::cout << "Warning: " << *iterTokens << " is not an integer!\n";
                }
            }
            bandsDefined = true;
		}
		else
		{
			bandsDefined = false;
		}
		xercesc::XMLString::release(&bandsXMLStr);
        
		
		XMLCh *dirXMLStr = xercesc::XMLString::transcode("dir");
		XMLCh *extXMLStr = xercesc::XMLString::transcode("ext");
		if(argElement->hasAttribute(dirXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(dirXMLStr));
			string dirStr = std::string(charValue);
			xercesc::XMLString::release(&charValue);
			
			charValue = xercesc::XMLString::transcode(argElement->getAttribute(extXMLStr));
			string extStr = std::string(charValue);
			xercesc::XMLString::release(&charValue);
			
			try
			{
				this->inputImages = fileUtils.getDIRList(dirStr, extStr, &this->numImages, false);
			}
			catch(rsgis::RSGISException e)
			{
				throw rsgis::RSGISXMLArgumentsException(e.what());
			}
		}
		else
		{
			xercesc::DOMElement *imageElement = NULL;
			
			DOMNodeList *imagesList = argElement->getElementsByTagName(rsgisimageXMLStr);
			this->numImages = imagesList->getLength();
			this->inputImages = new std::string[numImages];
			
			XMLCh *fileXMLStr = xercesc::XMLString::transcode("file");
			for(int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<xercesc::DOMElement*>(imagesList->item(i));
				
				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					this->inputImages[i] = std::string(charValue);
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'file\' attribute was provided.");
				}
			}
			xercesc::XMLString::release(&fileXMLStr);
		}
		xercesc::XMLString::release(&dirXMLStr);
		xercesc::XMLString::release(&extXMLStr);	
	}
	else if ((xercesc::XMLString::equals(optionCut2Poly, optionXML)) | (xercesc::XMLString::equals(optionCut2Polys, optionXML))) 
	{		
		/* Changed to cut2polys for concistency with subset to polys. 
           Also works with cut2poly to enable compatibility with old scripts - Dan */
        this->option = RSGISExeImageUtils::cut2poly;
		
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
		
		
		XMLCh *outfilenameXMLStr = xercesc::XMLString::transcode("outfilename");
		if(argElement->hasAttribute(outfilenameXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outfilenameXMLStr));
			this->filenameAttribute = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'outfilename\' attribute was provided.");
		}
		xercesc::XMLString::release(&outfilenameXMLStr);
		
		XMLCh *nodataXMLStr = xercesc::XMLString::transcode("nodata");
		if(argElement->hasAttribute(nodataXMLStr))
		{
            XMLCh *NaNStr = xercesc::XMLString::transcode("NaN");
			const XMLCh *noDataValue = argElement->getAttribute(nodataXMLStr);
			if(xercesc::XMLString::equals(noDataValue, NaNStr))
			{
                const char *val = "NaN";
				this->nodataValue = nan(val);
			}
			else
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(nodataXMLStr));
                this->nodataValue = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
			}
			xercesc::XMLString::release(&NaNStr);

		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'nodata\' attribute was provided.");
		}
		xercesc::XMLString::release(&nodataXMLStr);
	}
	else if (xercesc::XMLString::equals(optionMask, optionXML))
	{		
		this->option = RSGISExeImageUtils::mask;
		
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
				
		XMLCh *maskvalueXMLStr = xercesc::XMLString::transcode("maskvalue");
		if(argElement->hasAttribute(maskvalueXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maskvalueXMLStr));
			this->maskValue = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'maskvalue\' attribute was provided.");
		}
		xercesc::XMLString::release(&maskvalueXMLStr);
		
	}
	else if (xercesc::XMLString::equals(optionResample, optionXML))
	{		
		this->option = RSGISExeImageUtils::resample;
		
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
				
		XMLCh *scaleXMLStr = xercesc::XMLString::transcode("scale");
		if(argElement->hasAttribute(scaleXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(scaleXMLStr));
			this->resampleScale = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'scale\' attribute was provided.");
		}
		xercesc::XMLString::release(&scaleXMLStr);
				
		XMLCh *interpolatorCubic = xercesc::XMLString::transcode("Cubic");
		XMLCh *interpolatorBilinearArea = xercesc::XMLString::transcode("BilinearArea");
		XMLCh *interpolatorBilinearPoint = xercesc::XMLString::transcode("BilinearPoint");
		XMLCh *interpolatorNN = xercesc::XMLString::transcode("NN");
		XMLCh *interpolatorTriangular = xercesc::XMLString::transcode("Triangular");
		
		XMLCh *interpolatorXMLStr = xercesc::XMLString::transcode("interpolation");
		if(argElement->hasAttribute(interpolatorXMLStr))
		{
			const XMLCh *interpolatorXMLValue = argElement->getAttribute(interpolatorXMLStr);
			if(xercesc::XMLString::equals(interpolatorCubic, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::cubic;
			}
			else if (xercesc::XMLString::equals(interpolatorBilinearArea, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::billinearArea;
			}
			else if (xercesc::XMLString::equals(interpolatorBilinearPoint, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::billinearPt;
			}
			else if (xercesc::XMLString::equals(interpolatorNN, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::nn;
			}
			else if (xercesc::XMLString::equals(interpolatorTriangular, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::trangular;
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("Interpolator was not recognized.");
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'interpolation\' attribute was provided.");
		}
		xercesc::XMLString::release(&interpolatorXMLStr);
		
		xercesc::XMLString::release(&interpolatorCubic);
		xercesc::XMLString::release(&interpolatorBilinearArea);
		xercesc::XMLString::release(&interpolatorBilinearPoint);
		xercesc::XMLString::release(&interpolatorNN);
		xercesc::XMLString::release(&interpolatorTriangular);
	}
	else if (xercesc::XMLString::equals(optionRasteriseDef, optionXML))
	{		
		this->option = RSGISExeImageUtils::rasterisedefiniens;
        
        this->definiensTiles = false;
        
		XMLCh *inDIRXMLStr = xercesc::XMLString::transcode("inDIR");
        XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
		if(argElement->hasAttribute(inDIRXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inDIRXMLStr));
			this->inputDIR = std::string(charValue);
			xercesc::XMLString::release(&charValue);
            
            
            XMLCh *outDIRXMLStr = xercesc::XMLString::transcode("outDIR");
            if(argElement->hasAttribute(outDIRXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outDIRXMLStr));
                this->outputDIR = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'outDIR\' attribute was provided.");
            }
            xercesc::XMLString::release(&outDIRXMLStr);
            
            this->definiensTiles = true;
		}
		else if(argElement->hasAttribute(imageXMLStr))
		{
            
            char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = std::string(charValue);
			xercesc::XMLString::release(&charValue);
            
            XMLCh *inCSVXMLStr = xercesc::XMLString::transcode("csv");
            if(argElement->hasAttribute(inCSVXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inCSVXMLStr));
                this->inputCSV = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'csv\' attribute was provided.");
            }
            xercesc::XMLString::release(&inCSVXMLStr);
            
            
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
            
            this->definiensTiles = false;
		}
        else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'image\' or \'inDIR\' attribute was provided one or other is required.");
		}
		xercesc::XMLString::release(&inDIRXMLStr);
		xercesc::XMLString::release(&imageXMLStr);
		
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
				std::cout << "Proj not reconized therefore defaulting to image.";
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
	else if (xercesc::XMLString::equals(optionPrintProj4, optionXML))
	{		
		this->option = RSGISExeImageUtils::printProj4;
		
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
	}
	else if (xercesc::XMLString::equals(optionPrintWKT, optionXML))
	{		
		this->option = RSGISExeImageUtils::printWKT;
		
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
	}
	else if (xercesc::XMLString::equals(optionExtract2DScatterPtxt, optionXML))
	{		
		this->option = RSGISExeImageUtils::extract2dscatterptxt;
		
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
		
		XMLCh *band1XMLStr = xercesc::XMLString::transcode("band1");
		if(argElement->hasAttribute(band1XMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(band1XMLStr));
			this->imgBand1 = mathUtils.strtoint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'band1\' attribute was provided.");
		}
		xercesc::XMLString::release(&band1XMLStr);

		XMLCh *band2XMLStr = xercesc::XMLString::transcode("band2");
		if(argElement->hasAttribute(band2XMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(band2XMLStr));
			this->imgBand2 = mathUtils.strtoint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'band2\' attribute was provided.");
		}
		xercesc::XMLString::release(&band2XMLStr);
				
		XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->numImages = 1;
			this->inputImages = new std::string[numImages];
			this->inputImages[0] = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			xercesc::DOMElement *imageElement = NULL;
			
			DOMNodeList *imagesList = argElement->getElementsByTagName(rsgisimageXMLStr);
			this->numImages = imagesList->getLength();
			this->inputImages = new std::string[numImages];
			
			XMLCh *fileXMLStr = xercesc::XMLString::transcode("file");
			for(int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<xercesc::DOMElement*>(imagesList->item(i));
				
				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					this->inputImages[i] = std::string(charValue);
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'file\' attribute was provided.");
				}
			}
			xercesc::XMLString::release(&fileXMLStr);
		}
		xercesc::XMLString::release(&imageXMLStr);
	}
	else if (xercesc::XMLString::equals(optionSGSmoothing, optionXML))
	{		
		this->option = RSGISExeImageUtils::sgsmoothing;
		
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
		
		XMLCh *orderXMLStr = xercesc::XMLString::transcode("order");
		if(argElement->hasAttribute(orderXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(orderXMLStr));
			this->order = mathUtils.strtoint(std::string(charValue))+1; // Order starts at zero therefore +1
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'order\' attribute was provided.");
		}
		xercesc::XMLString::release(&orderXMLStr);
		
		XMLCh *windowXMLStr = xercesc::XMLString::transcode("window");
		if(argElement->hasAttribute(windowXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(windowXMLStr));
			this->window = mathUtils.strtoint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'window\' attribute was provided.");
		}
		xercesc::XMLString::release(&windowXMLStr);

		XMLCh *imagebandsXMLStr = xercesc::XMLString::transcode("imagebands");
		if(argElement->hasAttribute(imagebandsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imagebandsXMLStr));
			this->bandFloatValuesVector = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'imagebands\' attribute was provided.");
		}
		xercesc::XMLString::release(&imagebandsXMLStr);
	}
	else if (xercesc::XMLString::equals(optionCumulativeArea, optionXML))
	{		
		this->option = RSGISExeImageUtils::cumulativearea;
		
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
		
		XMLCh *imagebandsXMLStr = xercesc::XMLString::transcode("imagebands");
		if(argElement->hasAttribute(imagebandsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imagebandsXMLStr));
			this->inMatrixfile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'imagebands\' attribute was provided.");
		}
		xercesc::XMLString::release(&imagebandsXMLStr);
	}
	else if (xercesc::XMLString::equals(optionCreateImage, optionXML))
	{
		this->option = RSGISExeImageUtils::createimage;
		
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
		
		XMLCh *numBandsXMLStr = xercesc::XMLString::transcode("numbands");
		if(argElement->hasAttribute(numBandsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(numBandsXMLStr));
			this->numBands = mathUtils.strtoint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'numbands\' attribute was provided.");
		}
		xercesc::XMLString::release(&numBandsXMLStr);
		
		XMLCh *widthXMLStr = xercesc::XMLString::transcode("width");
		if(argElement->hasAttribute(widthXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(widthXMLStr));
			this->width = mathUtils.strtoint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'width\' attribute was provided.");
		}
		xercesc::XMLString::release(&widthXMLStr);
		
		XMLCh *heightXMLStr = xercesc::XMLString::transcode("height");
		if(argElement->hasAttribute(heightXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(heightXMLStr));
			this->height = mathUtils.strtoint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'height\' attribute was provided.");
		}
		xercesc::XMLString::release(&heightXMLStr);
		
		
		XMLCh *eastingsXMLStr = xercesc::XMLString::transcode("eastings");
		if(argElement->hasAttribute(eastingsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(eastingsXMLStr));
			this->eastings = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'eastings\' attribute was provided.");
		}
		xercesc::XMLString::release(&eastingsXMLStr);

		
		XMLCh *northingsXMLStr = xercesc::XMLString::transcode("northings");
		if(argElement->hasAttribute(northingsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(northingsXMLStr));
			this->northings = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'northings\' attribute was provided.");
		}
		xercesc::XMLString::release(&northingsXMLStr);
		
		XMLCh *valueXMLStr = xercesc::XMLString::transcode("value");
		if(argElement->hasAttribute(valueXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(valueXMLStr));
			this->outValue = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'value\' attribute was provided.");
		}
		xercesc::XMLString::release(&valueXMLStr);

		
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
		
		
		XMLCh *proj4XMLStr = xercesc::XMLString::transcode("proj4");
		if(argElement->hasAttribute(proj4XMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(proj4XMLStr));
			this->proj = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj4\' attribute was provided.");
		}
		xercesc::XMLString::release(&proj4XMLStr);
		
		
	}
	else if (xercesc::XMLString::equals(optionStretchImage, optionXML))
	{		
		this->option = RSGISExeImageUtils::stretch;
		
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
		
				
		XMLCh *stretchLinearMinMax = xercesc::XMLString::transcode("LinearMinMax");
		XMLCh *stretchLinearPercent = xercesc::XMLString::transcode("LinearPercent");
		XMLCh *stretchLinearStdDev = xercesc::XMLString::transcode("LinearStdDev");
		XMLCh *stretchHistogram = xercesc::XMLString::transcode("Histogram");
		XMLCh *stretchExponential = xercesc::XMLString::transcode("Exponential");
		XMLCh *stretchLogarithmic = xercesc::XMLString::transcode("Logarithmic");
		XMLCh *stretchPowerLaw = xercesc::XMLString::transcode("PowerLaw");
		
		XMLCh *stretchXMLStr = xercesc::XMLString::transcode("stretch");
		if(argElement->hasAttribute(stretchXMLStr))
		{
			const XMLCh *stretchXMLValue = argElement->getAttribute(stretchXMLStr);
			if(xercesc::XMLString::equals(stretchLinearMinMax, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::linearMinMax;
			}
			else if (xercesc::XMLString::equals(stretchLinearPercent, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::linearPercent;
				
				XMLCh *percentXMLStr = xercesc::XMLString::transcode("percent");
				if(argElement->hasAttribute(percentXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(percentXMLStr));
					this->percent = mathUtils.strtofloat(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'percent\' attribute was provided.");
				}
				xercesc::XMLString::release(&percentXMLStr);				
			}
			else if (xercesc::XMLString::equals(stretchLinearStdDev, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::linearStdDev;
				
				XMLCh *stdDevXMLStr = xercesc::XMLString::transcode("stddev");
				if(argElement->hasAttribute(stdDevXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(stdDevXMLStr));
					this->stddev = mathUtils.strtofloat(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'stddev\' attribute was provided.");
				}
				xercesc::XMLString::release(&stdDevXMLStr);				
			}
			else if (xercesc::XMLString::equals(stretchHistogram, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::histogram;
			}
			else if (xercesc::XMLString::equals(stretchExponential, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::exponential;
			}
			else if (xercesc::XMLString::equals(stretchLogarithmic, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::logarithmic;
			}
			else if (xercesc::XMLString::equals(stretchPowerLaw, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::powerLaw;
				XMLCh *powerXMLStr = xercesc::XMLString::transcode("power");
				if(argElement->hasAttribute(powerXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(powerXMLStr));
					this->power = mathUtils.strtofloat(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'power\' attribute was provided.");
				}
				xercesc::XMLString::release(&powerXMLStr);	
			}			
			else
			{
				throw rsgis::RSGISXMLArgumentsException("Stretch was not recognized.");
			}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'stretch\' attribute was provided.");
		}
		xercesc::XMLString::release(&stretchXMLStr);
		
		xercesc::XMLString::release(&stretchLinearMinMax);
		xercesc::XMLString::release(&stretchLinearPercent);
		xercesc::XMLString::release(&stretchLinearStdDev);
		xercesc::XMLString::release(&stretchHistogram);
		xercesc::XMLString::release(&stretchExponential);
		xercesc::XMLString::release(&stretchLogarithmic);
		xercesc::XMLString::release(&stretchPowerLaw);
        
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
			cerr << "No \'ignorezeros\' attribute was provided so defaulting to ignore zeros.\n";
            this->ignoreZeros = true;
		}
		xercesc::XMLString::release(&ignoreZerosXMLStr);

	}
	else if (xercesc::XMLString::equals(optionHueColour, optionXML))
	{		
		this->option = RSGISExeImageUtils::huecolour;
		
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
		
		XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
		if(argElement->hasAttribute(bandXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bandXMLStr));
			this->imgBand1 = mathUtils.strtoint(std::string(charValue))-1; // -1 so interface starts at 1 not 0.
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
		}
		xercesc::XMLString::release(&bandXMLStr);
		
		
		XMLCh *lowValueXMLStr = xercesc::XMLString::transcode("lowvalue");
		if(argElement->hasAttribute(lowValueXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(lowValueXMLStr));
			this->lowerRangeValue = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'lowvalue\' attribute was provided.");
		}
		xercesc::XMLString::release(&lowValueXMLStr);
		
		XMLCh *highValueXMLStr = xercesc::XMLString::transcode("highvalue");
		if(argElement->hasAttribute(highValueXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(highValueXMLStr));
			this->upperRangeValue = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'highvalue\' attribute was provided.");
		}
		xercesc::XMLString::release(&highValueXMLStr);

		XMLCh *backgroundXMLStr = xercesc::XMLString::transcode("background");
		if(argElement->hasAttribute(backgroundXMLStr))
		{
            XMLCh *NaNStr = xercesc::XMLString::transcode("NaN");
			const XMLCh *noDataValue = argElement->getAttribute(backgroundXMLStr);
			if(xercesc::XMLString::equals(noDataValue, NaNStr))
			{
                const char *val = "NaN";
				this->nodataValue = nan(val);
			}
			else
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(backgroundXMLStr));
                this->nodataValue = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
			}
			xercesc::XMLString::release(&NaNStr);

		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'background\' attribute was provided.");
		}
		xercesc::XMLString::release(&backgroundXMLStr);
		
		
	}
	else if (xercesc::XMLString::equals(optionRemoveSpatialRef, optionXML))
	{		
		this->option = RSGISExeImageUtils::removespatialref;
		
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
	else if (xercesc::XMLString::equals(optionAddnoise, optionXML))
	{		
		this->option = RSGISExeImageUtils::addnoise;
		
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
		
		XMLCh *scaleXMLStr = xercesc::XMLString::transcode("scale");
		if(argElement->hasAttribute(scaleXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(scaleXMLStr));
			this->scale = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'scale\' attribute was provided.");
		}
		xercesc::XMLString::release(&scaleXMLStr);
		
		XMLCh *typeXMLStr = xercesc::XMLString::transcode("type");
		if(argElement->hasAttribute(typeXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(typeXMLStr));
			string typeStr = std::string(charValue);
			if (typeStr == "percentGaussianNoise") 
			{
				this->noise = rsgis::img::percentGaussianNoise;
			}
			else 
			{
				this->noise = rsgis::img::randomNoise;
			}

			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'scale\' attribute was provided.");
		}
		xercesc::XMLString::release(&typeXMLStr);
		
		
	}
	else if (xercesc::XMLString::equals(optionDefineSpatialRef, optionXML))
	{		
		this->option = RSGISExeImageUtils::definespatialref;
		
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
		
		
		XMLCh *projXMLStr = xercesc::XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(projXMLStr));
			this->proj = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		xercesc::XMLString::release(&projXMLStr);
		
		XMLCh *tlxXMLStr = xercesc::XMLString::transcode("tlx");
		if(argElement->hasAttribute(tlxXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(tlxXMLStr));
			this->eastings = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'tlx\' attribute was provided.");
		}
		xercesc::XMLString::release(&tlxXMLStr);

		XMLCh *tlyXMLStr = xercesc::XMLString::transcode("tly");
		if(argElement->hasAttribute(tlyXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(tlyXMLStr));
			this->northings = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'tly\' attribute was provided.");
		}
		xercesc::XMLString::release(&tlyXMLStr);
		
		XMLCh *resxXMLStr = xercesc::XMLString::transcode("resx");
		if(argElement->hasAttribute(resxXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(resxXMLStr));
			this->xRes = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'resx\' attribute was provided.");
		}
		xercesc::XMLString::release(&resxXMLStr);
		
		XMLCh *resyXMLStr = xercesc::XMLString::transcode("resy");
		if(argElement->hasAttribute(resyXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(resyXMLStr));
			this->yRes = mathUtils.strtofloat(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'resy\' attribute was provided.");
		}
		xercesc::XMLString::release(&resyXMLStr);
		
	}
    else if (xercesc::XMLString::equals(optionSubset, optionXML))
	{		
		this->option = RSGISExeImageUtils::subset;
		
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
	}
    else if (xercesc::XMLString::equals(optionSubset2Polys, optionXML))
	{		
		this->option = RSGISExeImageUtils::subset2polys;
		
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
		
		
		XMLCh *outfilenameXMLStr = xercesc::XMLString::transcode("outfilename");
		if(argElement->hasAttribute(outfilenameXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outfilenameXMLStr));
			this->filenameAttribute = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'outfilename\' attribute was provided.");
		}
		xercesc::XMLString::release(&outfilenameXMLStr);
    }
	else if (xercesc::XMLString::equals(optionPanSharpen, optionXML))
	{		
		this->option = RSGISExeImageUtils::pansharpen;
		
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
		
		XMLCh *winSizeXMLStr = xercesc::XMLString::transcode("winSize");
		if(argElement->hasAttribute(winSizeXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(winSizeXMLStr));
			this->panWinSize = mathUtils.strtoint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->panWinSize = 7;
		}
		xercesc::XMLString::release(&winSizeXMLStr);
	
		
	}
    else if (xercesc::XMLString::equals(optionCreateSlices, optionXML))
	{		
		this->option = RSGISExeImageUtils::createslices;
		
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
    else if(xercesc::XMLString::equals(optionClump, optionXML))
    {
        this->option = RSGISExeImageUtils::clump;
		
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
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
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
				cerr << "Proj not reconized therefore defaulting to image.";
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
    else if (xercesc::XMLString::equals(optionComposite, optionXML))
	{		
		this->option = RSGISExeImageUtils::imageComposite;
		
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
		
		XMLCh *cBandsXMLStr = xercesc::XMLString::transcode("compositeBands");
		if(argElement->hasAttribute(cBandsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(cBandsXMLStr));
			this->compositeBands = mathUtils.strtoint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'compositeBands\' attribute was provided.");
		}
		xercesc::XMLString::release(&cBandsXMLStr);
		
		XMLCh *typeXMLStr = xercesc::XMLString::transcode("stats");
        this->outCompStat = rsgis::img::compositeMean; // Set to default (mean)
		if(argElement->hasAttribute(typeXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(typeXMLStr));
			string typeStr = std::string(charValue);
			if (typeStr == "mean") 
			{
				this->outCompStat = rsgis::img::compositeMean;
			}
			else if (typeStr == "min") 
			{
				this->outCompStat = rsgis::img::compositeMin;
			}
            else if (typeStr == "max") 
			{
				this->outCompStat = rsgis::img::compositeMax;
			}
            else if (typeStr == "range") 
			{
				this->outCompStat = rsgis::img::compositeRange;
			}
            else 
            {
                throw rsgis::RSGISXMLArgumentsException("Statistics not recognised / available. Options are mean, min, max and range.");
            }

			xercesc::XMLString::release(&charValue);
		}
		else
		{
			std::cout << "No \'stats\' attribute was provided, assuming default of mean" << std::endl;
		}
		xercesc::XMLString::release(&typeXMLStr);
		
		
	}
    else if (xercesc::XMLString::equals(optionRelabel, optionXML))
	{		
		this->option = RSGISExeImageUtils::relabel;
		
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
        
        XMLCh *lutXMLStr = xercesc::XMLString::transcode("lut");
		if(argElement->hasAttribute(lutXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(lutXMLStr));
			this->lutMatrixFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'lut\' attribute was provided.");
		}
		xercesc::XMLString::release(&lutXMLStr);
	}
    else if (xercesc::XMLString::equals(optionAssignProj, optionXML))
    {
        this->option = RSGISExeImageUtils::assignproj;
        
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
        
        
        XMLCh *projWKTXMLStr = xercesc::XMLString::transcode("projwkt");
		if(argElement->hasAttribute(projWKTXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(projWKTXMLStr));
			this->projFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'projwkt\' attribute was provided.");
		}
		xercesc::XMLString::release(&projWKTXMLStr);
    }
    else if (xercesc::XMLString::equals(optionPopImgStats, optionXML))
    {
        this->option = RSGISExeImageUtils::popimgstats;
        
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
        
        
        XMLCh *nodataXMLStr = xercesc::XMLString::transcode("ignore");
		if(argElement->hasAttribute(nodataXMLStr))
		{
            XMLCh *NaNStr = xercesc::XMLString::transcode("NaN");
			const XMLCh *noDataValue = argElement->getAttribute(nodataXMLStr);
			if(xercesc::XMLString::equals(noDataValue, NaNStr))
			{
                const char *val = "NaN";
				this->nodataValue = nan(val);
			}
			else
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(nodataXMLStr));
                this->nodataValue = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
			}
			xercesc::XMLString::release(&NaNStr);
            
            this->useIgnoreVal = true;
		}
		else
		{
			this->useIgnoreVal = false;
		}
		xercesc::XMLString::release(&nodataXMLStr);
        
        XMLCh *pyramidsXMLStr = xercesc::XMLString::transcode("pyramids");
		if(argElement->hasAttribute(pyramidsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(pyramidsXMLStr));
			string typeStr = std::string(charValue);
			if(typeStr == "yes") 
			{
				this->calcImgPyramids = true;
			}
            else 
            {
                this->calcImgPyramids = false;
            }
            
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			this->calcImgPyramids = true;
		}
		xercesc::XMLString::release(&pyramidsXMLStr);
        
    }
    else if (xercesc::XMLString::equals(optionCreateCopy, optionXML))
	{		
		this->option = RSGISExeImageUtils::createcopy;
		
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
        
        
        XMLCh *pixelValXMLStr = xercesc::XMLString::transcode("pixelval");
		if(argElement->hasAttribute(pixelValXMLStr))
		{
            XMLCh *NaNStr = xercesc::XMLString::transcode("NaN");
			const XMLCh *dataValue = argElement->getAttribute(pixelValXMLStr);
			if(xercesc::XMLString::equals(dataValue, NaNStr))
			{
                const char *val = "NaN";
				this->dataValue = nan(val);
			}
			else
			{
				char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(pixelValXMLStr));
                this->dataValue = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
			}
			xercesc::XMLString::release(&NaNStr);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'pixelval\' attribute was provided.");
		}
		xercesc::XMLString::release(&pixelValXMLStr);
        
        XMLCh *numBandsXMLStr = xercesc::XMLString::transcode("numbands");
		if(argElement->hasAttribute(numBandsXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(numBandsXMLStr));
			this->numBands = mathUtils.strtoint(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'numbands\' attribute was provided.");
		}
		xercesc::XMLString::release(&numBandsXMLStr);
        
	}
    else if (xercesc::XMLString::equals(optionCreateKMLFile, optionXML))
	{		
		this->option = RSGISExeImageUtils::createKMLFile;
		
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
        
        XMLCh *outKMLFileXMLStr = xercesc::XMLString::transcode("outKMLFile");
		if(argElement->hasAttribute(outKMLFileXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outKMLFileXMLStr));
			this->outKMLFile = std::string(charValue);
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'outKMLFile\' attribute was provided.");
		}
		xercesc::XMLString::release(&outKMLFileXMLStr);
	}
    else if (xercesc::XMLString::equals(optionAssignSpatialInfo, optionXML))
	{		
		this->option = RSGISExeImageUtils::assignspatialinfo;
		
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
        
        XMLCh *tlxXMLStr = xercesc::XMLString::transcode("tlx");
		if(argElement->hasAttribute(tlxXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(tlxXMLStr));
			this->eastings = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'tlx\' attribute was provided.");
		}
		xercesc::XMLString::release(&tlxXMLStr);
        
        XMLCh *tlyXMLStr = xercesc::XMLString::transcode("tly");
		if(argElement->hasAttribute(tlyXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(tlyXMLStr));
			this->northings = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'tly\' attribute was provided.");
		}
		xercesc::XMLString::release(&tlyXMLStr);
        
        XMLCh *resxXMLStr = xercesc::XMLString::transcode("resX");
		if(argElement->hasAttribute(resxXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(resxXMLStr));
			this->xRes = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'resX\' attribute was provided.");
		}
		xercesc::XMLString::release(&resxXMLStr);
        
        XMLCh *resyXMLStr = xercesc::XMLString::transcode("resY");
		if(argElement->hasAttribute(resyXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(resyXMLStr));
			this->yRes = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No \'resYy\' attribute was provided.");
		}
		xercesc::XMLString::release(&resyXMLStr);
        
        XMLCh *rotxXMLStr = xercesc::XMLString::transcode("rotX");
		if(argElement->hasAttribute(rotxXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(rotxXMLStr));
			this->xRot = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
            std::cerr << "X rotation was not provided defaulting to value of 0.\n";
            this->xRot = 0;
		}
		xercesc::XMLString::release(&rotxXMLStr);
        
        XMLCh *rotyXMLStr = xercesc::XMLString::transcode("rotY");
		if(argElement->hasAttribute(rotyXMLStr))
		{
			char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(rotyXMLStr));
			this->yRot = mathUtils.strtodouble(std::string(charValue));
			xercesc::XMLString::release(&charValue);
		}
		else
		{
            std::cerr << "Y rotation was not provided defaulting to value of 0.\n";
            this->yRot = 0;
		}
		xercesc::XMLString::release(&rotyXMLStr);
        
	}
	else
	{
		string message = std::string("The option (") + std::string(xercesc::XMLString::transcode(optionXML)) + std::string(") is not known: RSGISExeImageUtils.");
		throw rsgis::RSGISXMLArgumentsException(message.c_str());
	}
	
	xercesc::XMLString::release(&algorName);
	xercesc::XMLString::release(&algorXMLStr);
	xercesc::XMLString::release(&optionXMLStr);
	xercesc::XMLString::release(&optionColour);
	xercesc::XMLString::release(&optionMosaic);
	xercesc::XMLString::release(&optionInclude);
	xercesc::XMLString::release(&optionCut2Poly);
    xercesc::XMLString::release(&optionCut2Polys);
	xercesc::XMLString::release(&optionMask);
	xercesc::XMLString::release(&optionResample);
	xercesc::XMLString::release(&optionRasteriseDef);
	xercesc::XMLString::release(&projImage);
	xercesc::XMLString::release(&projOSGB);
    xercesc::XMLString::release(&projNZ2000);
    xercesc::XMLString::release(&projNZ1949);
	xercesc::XMLString::release(&rsgisimageXMLStr);
	xercesc::XMLString::release(&optionPrintProj4);
	xercesc::XMLString::release(&optionPrintWKT);
	xercesc::XMLString::release(&optionExtract2DScatterPtxt);
	xercesc::XMLString::release(&optionSGSmoothing);
	xercesc::XMLString::release(&optionCumulativeArea);
	xercesc::XMLString::release(&optionCreateImage);
	xercesc::XMLString::release(&optionStretchImage);
	xercesc::XMLString::release(&optionHueColour);
	xercesc::XMLString::release(&optionRemoveSpatialRef);
	xercesc::XMLString::release(&optionAddnoise);
	xercesc::XMLString::release(&optionDefineSpatialRef);
    xercesc::XMLString::release(&optionSubset);
    xercesc::XMLString::release(&optionSubset2Polys);
    xercesc::XMLString::release(&optionPanSharpen);
    xercesc::XMLString::release(&optionColourImageBands);
    xercesc::XMLString::release(&optionCreateSlices);
    xercesc::XMLString::release(&optionClump);
    xercesc::XMLString::release(&optionComposite);
    xercesc::XMLString::release(&optionRelabel);
    xercesc::XMLString::release(&optionAssignProj);
    xercesc::XMLString::release(&optionPopImgStats);
	xercesc::XMLString::release(&optionCreateCopy);
    xercesc::XMLString::release(&optionCreateKMLFile);
    xercesc::XMLString::release(&optionAssignSpatialInfo);
    
	parsed = true;
}

void RSGISExeImageUtils::runAlgorithm() throw(rsgis::RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(option == RSGISExeImageUtils::colour)
		{
			std::cout << "Colouring image\n";
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::img::RSGISColourUpImage *colourImage = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;

			int numBands = 0;
			try
			{
				datasets = new GDALDataset*[1];
				std::cout << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				numBands = datasets[0]->GetRasterCount();
				
				for(int i = 0; i < this->numClasses; i++)
				{
					classColour[i]->numInputBands = numBands;
					
					std::cout << i <<  ") Class " << classColour[i]->className << " with ID ";
					std::cout << classColour[i]->classID << " uses image band " << classColour[i]->imgBand + 1;
					std::cout << " where a total of " << classColour[i]->numInputBands << " is available:\n";
					std::cout << "Lower = " << classColour[i]->lower << std::endl;
					std::cout << "Upper = " << classColour[i]->upper << std::endl;
					std::cout << "Red = " << classColour[i]->red << std::endl;
					std::cout << "Green = " << classColour[i]->green << std::endl;
					std::cout << "Blue = " << classColour[i]->blue << std::endl;
				}
				
				colourImage = new rsgis::img::RSGISColourUpImage(3, this->classColour, this->numClasses);
				calcImage = new rsgis::img::RSGISCalcImage(colourImage, "", true);
				
				calcImage->calcImage(datasets, 1, this->outputImage, false, NULL, this->imageFormat, this->outDataType);
				
				if(datasets != NULL)
				{
					GDALClose(datasets[0]);
					delete[] datasets;
				}
				GDALDestroyDriverManager();
				delete calcImage;
				delete colourImage;
			}
			catch (rsgis::RSGISException e) 
			{
				std::cout << "Exception occured: " << e.what() << std::endl;
			}
			
		}
        else if(option == RSGISExeImageUtils::colourimagebands)
		{
			std::cout << "Colouring image bands\n";
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::img::RSGISColourUpImageBand *colourImage = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
            
			int numBands = 0;
			try
			{
				datasets = new GDALDataset*[1];
				std::cout << this->inputImage << std::endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				numBands = datasets[0]->GetRasterCount();
				
				for(int i = 0; i < this->numClasses; i++)
				{
					classColour[i]->numInputBands = numBands;
					
					std::cout << i <<  ") Class " << classColour[i]->className << " with ID ";
					std::cout << classColour[i]->classID << std::endl;
					std::cout << "Lower = " << classColour[i]->lower << std::endl;
					std::cout << "Upper = " << classColour[i]->upper << std::endl;
					std::cout << "Red = " << classColour[i]->red << std::endl;
					std::cout << "Green = " << classColour[i]->green << std::endl;
					std::cout << "Blue = " << classColour[i]->blue << std::endl;
				}
				
				colourImage = new rsgis::img::RSGISColourUpImageBand(3, this->classColour, this->numClasses);
				calcImage = new rsgis::img::RSGISCalcImage(colourImage, "", true);
				
				calcImage->calcImageBand(datasets, 1, this->outputImage);
				
				if(datasets != NULL)
				{
					GDALClose(datasets[0]);
					delete[] datasets;
				}
				GDALDestroyDriverManager();
				delete calcImage;
				delete colourImage;
			}
			catch (rsgis::RSGISException e) 
			{
				std::cout << "Exception occured: " << e.what() << std::endl;
			}
			
		}
		else if(option == RSGISExeImageUtils::mosaic)
		{
			std::cout << "Mosaicing Images\n";
			rsgis::img::RSGISImageMosaic mosaic;
			try
			{
				if (this->mosaicSkipVals) 
				{
					mosaic.mosaicSkipVals(inputImages, this->numImages, this->outputImage, this->nodataValue, this->skipValue, this->projFromImage, this->proj, this->skipBand);
				}
				else if (this->mosaicSkipThreash)
				{
					mosaic.mosaicSkipThreash(inputImages, this->numImages, this->outputImage, this->nodataValue, this->skipLowerThreash, this->skipUpperThreash, this->projFromImage, this->proj, this->skipBand);
				}
				else 
				{
					mosaic.mosaic(inputImages, this->numImages, this->outputImage, this->nodataValue, this->projFromImage, this->proj);
				}
								
				delete[] inputImages;
			}
			catch(rsgis::RSGISException e) 
			{
				throw e;
			}
			
		}
		else if(option == RSGISExeImageUtils::include)
		{
            std::cout << "Include images into a larger image\n";
            if(bandsDefined)
            {
                for(vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                {
                    std::cout << "Band " << *iterBands << std::endl;
                }
            }
			GDALAllRegister();
			GDALDataset *baseDS = NULL;
			rsgis::img::RSGISImageMosaic mosaic;
			try
			{
				std::cout << this->inputImage << std::endl;
				baseDS = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_Update);
				if(baseDS == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				mosaic.includeDatasets(baseDS, this->inputImages, this->numImages, this->bands, this->bandsDefined);
				
				GDALClose(baseDS);
				GDALDestroyDriverManager();
				delete[] inputImages;
			}
			catch (rsgis::RSGISException e) 
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::cut2poly)
		{
			std::cout << "Cutting image to polygons\n";
			GDALAllRegister();
			OGRRegisterAll();
			
			GDALDataset **dataset = NULL;
			OGRDataSource *inputVecDS = NULL;
			OGRLayer *inputVecLayer = NULL;
			
			rsgis::img::RSGISCopyImage *copyImage = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			rsgis::vec::RSGISVectorIO vecIO;
			rsgis::vec::RSGISPolygonData **polyData = NULL;
			rsgis::vec::RSGISImageTileVector **data = NULL;
			rsgis::vec::RSGISVectorUtils vecUtils;
			
			string vectorLayerName = vecUtils.getLayerName(this->inputVector);
			int numImageBands = 0;
			int numFeatures = 0;
			string outputFilePath;
			
			try
			{
				// Open Image
				dataset = new GDALDataset*[1];
				std::cout << this->inputImage << std::endl;
				dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset[0] == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				numImageBands = dataset[0]->GetRasterCount();
				std::cout << "Raster Band Count = " << numImageBands << std::endl;
				
				// Open vector
				inputVecDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputVecDS == NULL)
				{
					string message = std::string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputVecLayer = inputVecDS->GetLayerByName(vectorLayerName.c_str());
				if(inputVecLayer == NULL)
				{
					string message = std::string("Could not open vector layer ") + vectorLayerName;
					throw RSGISFileException(message.c_str());
				}
				
				// READ IN SHAPEFILE
				numFeatures = inputVecLayer->GetFeatureCount();
				polyData = new rsgis::vec::RSGISPolygonData*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					polyData[i] = new rsgis::vec::RSGISImageTileVector(this->filenameAttribute);
				}
				std::cout << "Reading in " << numFeatures << " features\n";
				vecIO.readPolygons(inputVecLayer, polyData, numFeatures);
				
				//Convert to RSGISImageTileVector
				data = new rsgis::vec::RSGISImageTileVector*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					data[i] = dynamic_cast<rsgis::vec::RSGISImageTileVector*>(polyData[i]);
				}
				delete[] polyData;
				
				copyImage = new rsgis::img::RSGISCopyImage(numImageBands);
				calcImage = new rsgis::img::RSGISCalcImage(copyImage, "", true);
				
                unsigned int failCount = 0;
                
				for(int i = 0; i < numFeatures; i++)
				{
					outputFilePath = this->outputImage + data[i]->getFileName() + ".env";
					std::cout << i << ": " << outputFilePath << std::endl;
                    try
                    {
                        calcImage->calcImageWithinPolygon(dataset, 1, outputFilePath, data[i]->getBBox(), data[i]->getPolygon(), this->nodataValue, rsgis::img::polyContainsPixelCenter);
                    }
                    catch (rsgis::img::RSGISImageBandException e)
                    {
                        ++failCount;
                        if(failCount <= 100)
                        {
                            cerr << "RSGISException caught: " << e.what() << std::endl;
                            cerr << "Check output path exists and is writable and all polygons in shapefile:" << std::endl;
                            cerr << " " << this->inputVector << std::endl;
                            cerr << "Are completely within:" << std::endl;
                            cerr << " " << this->inputImage << std::endl;
                        }
                        else
                        {
                            cerr << "Over 100 exceptions have been caught, exiting" << std::endl;
                            throw e;
                        }
                    }
				}
				
				GDALClose(dataset[0]);
				delete[] dataset;
				OGRDataSource::DestroyDataSource(inputVecDS);
				OGRCleanupAll();
				GDALDestroyDriverManager();
				delete calcImage;
				delete copyImage;
			}
			catch(rsgis::RSGISException e)
			{
				cerr << "RSGISException caught: " << e.what() << std::endl;
			}
		}
		else if(option == RSGISExeImageUtils::mask)
		{
			GDALAllRegister();
			rsgis::img::RSGISMaskImage *maskImage = NULL;
			GDALDataset *dataset = NULL;
			GDALDataset *mask = NULL;
			try
			{
				std::cout << this->inputImage << std::endl;
				dataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				std::cout << this->imageMask << std::endl;
				mask = (GDALDataset *) GDALOpenShared(this->imageMask.c_str(), GA_ReadOnly);
				if(mask == NULL)
				{
					string message = std::string("Could not open image ") + this->imageMask;
					throw RSGISImageException(message.c_str());
				}
				
				maskImage = new rsgis::img::RSGISMaskImage();
				maskImage->maskImage(dataset, mask, this->outputImage, this->imageFormat, this->outDataType, this->nodataValue);
				
				GDALClose(dataset);
				GDALClose(mask);
				delete maskImage;
				GDALDestroyDriverManager();
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::resample)
		{
			GDALAllRegister();
			
			rsgis::img::RSGISImageInterpolation *interpolation = NULL;
			rsgis::img::RSGISInterpolator *interpolator = NULL;
			
			GDALDataset *inDataset = NULL;
			
			int xOutResolution = 0;
			int yOutResolution = 0;
			
			try
			{
				if(this->interpolator == RSGISExeImageUtils::cubic)
				{
					std::cout << "Using a cubic interpolator\n";
					interpolator = new rsgis::img::RSGISCubicInterpolator();
				}
				else if(this->interpolator == RSGISExeImageUtils::billinearArea)
				{
					std::cout << "Using a bilinear (area) interpolator\n";
					interpolator = new rsgis::img::RSGISBilinearAreaInterpolator();
				}
				else if(this->interpolator == RSGISExeImageUtils::billinearPt)
				{
					std::cout << "Using a bilinear (point) interpolator\n";
					interpolator = new rsgis::img::RSGISBilinearPointInterpolator();
				}
				else if(this->interpolator == RSGISExeImageUtils::nn)
				{
					std::cout << "Using a nearest neighbour interpolator\n";
					interpolator = new rsgis::img::RSGISNearestNeighbourInterpolator();
				}
				else if(this->interpolator == RSGISExeImageUtils::trangular)
				{
					std::cout << "Using a triangular interpolator\n";
					interpolator = new rsgis::img::RSGISTriangulationInterpolator();
				}
				else
				{
					throw RSGISException("Interpolator Option Not Reconised");
				}
				
				interpolation = new rsgis::img::RSGISImageInterpolation(interpolator);
				
				std::cout << this->inputImage << std::endl;
				inDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				interpolation->findOutputResolution(inDataset, this->resampleScale, &xOutResolution, &yOutResolution);
				
				interpolation->interpolateNewImage(inDataset, xOutResolution, yOutResolution, this->outputImage);
				
				GDALClose(inDataset);

				delete interpolation;
				delete interpolator;
				GDALDestroyDriverManager();
								
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::rasterisedefiniens)
		{
			GDALAllRegister();
			rsgis::utils::RSGISFileUtils fileUtils;
			rsgis::img::RSGISDefiniensCSVRasterise rasterisedefiniensCSV = rsgis::img::RSGISDefiniensCSVRasterise(projFromImage, proj);
			std::cout << "Rasterise Definiens\n";
			
            if(this->definiensTiles)
            {
                std::cout << "Input DIR: " << this->inputDIR << std::endl;
                std::cout << "Output DIR: " << this->outputDIR << std::endl;
                std::string tif = std::string(".tif");
                std::string csv = std::string(".CSV");
                
                std::string *inputTIF = NULL;
                int numTIFs = 0;
                
                std::string *inputCSV = NULL;
                int numCSVs = 0;
                
                rsgis::datastruct::SortedGenericList<rsgis::utils::RSGISDefiniensWorkspaceFileName> *sortedTIFs = NULL;
                rsgis::datastruct::SortedGenericList<rsgis::utils::RSGISDefiniensWorkspaceFileName> *sortedCSVs = NULL;
                
                try
                {
                    inputTIF = fileUtils.getDIRList(this->inputDIR, tif, &numTIFs, false);
                    inputCSV = fileUtils.getDIRList(this->inputDIR, csv, &numCSVs, false);
                    
                    std::cout << "numTIFs = " << numTIFs << std::endl;
                    std::cout << "numCSVs = " << numCSVs << std::endl;
                    
                    if(numTIFs != numCSVs)
                    {
                        throw RSGISException("number of TIFs and CSV files found do not match.");
                    }
                    
                    if(numTIFs == 0)
                    {
                        throw RSGISException("No input files were found.");
                    }
                    
                    sortedTIFs = new SortedGenericList<rsgis::utils::RSGISDefiniensWorkspaceFileName>(numTIFs, numTIFs/2);
                    sortedCSVs = new SortedGenericList<rsgis::utils::RSGISDefiniensWorkspaceFileName>(numCSVs, numCSVs/2);
                    
                    for(int i = 0; i < numTIFs; i++)
                    {
                        //std::cout << i << ")\t" << inputTIF[i] << "\t" << inputCSV[i] << std::endl;
                        sortedTIFs->add(new rsgis::utils::RSGISDefiniensWorkspaceFileName(inputTIF[i]));
                        sortedCSVs->add(new rsgis::utils::RSGISDefiniensWorkspaceFileName(inputCSV[i]));
                    }
                    
                    sortedTIFs->printAsc();
                    sortedCSVs->printAsc();
                    
                    //Check basename is the same:
                    std::string basename = sortedTIFs->peekTop()->getBaseName();
                    //std::cout << "BaseName = " << basename << std::endl;
                    int size = sortedCSVs->getSize();
                    for(int i = 0; i < size; i++)
                    {
                        if(sortedTIFs->getAt(i)->getBaseName() != basename)
                        {
                            throw rsgis::RSGISException("Base filenames are not the same.");
                        }
                        
                        if(sortedCSVs->getAt(i)->getBaseName() != basename)
                        {
                            throw rsgis::RSGISException("Base filenames are not the same.");
                        }
                    }
                    
                    std::cout << "Base filenames match\n";
                    int oldTIFs = 0;

                    for(int i = size-1; i > 0; i--)
                    {
                        if(sortedTIFs->getAt(i)->getTile() == sortedTIFs->getAt(i-1)->getTile())
                        {
                            if(sortedTIFs->getAt(i)->getVersion() == sortedTIFs->getAt(i-1)->getVersion())
                            {
                                throw rsgis::RSGISException("The tile has the same number and version.");
                            }
                            else
                            {
                                sortedTIFs->getAt(i)->setOldVersion(true);
                                oldTIFs++;
                            }
                        }
                    }
                    
                    int oldCSVs = 0;
                    for(int i = size-1; i > 0; i--)
                    {
                        if(sortedCSVs->getAt(i)->getTile() == sortedCSVs->getAt(i-1)->getTile())
                        {
                            if(sortedCSVs->getAt(i)->getVersion() == sortedCSVs->getAt(i-1)->getVersion())
                            {
                                throw rsgis::RSGISException("The tile has the same number and version.");
                            }
                            else
                            {
                                sortedCSVs->getAt(i)->setOldVersion(true);
                                oldCSVs++;
                            }
                        }
                    }
                    
                    if(oldTIFs != oldCSVs)
                    {
                        throw rsgis::RSGISException("A different number of old version tiles were identified in the TIF and CSV lists.");
                    }
                    
                    std::cout << oldTIFs << " old versions of tiles have been identified and will be ignored.\n";
                    // sortedTIFs->printAsc();
                    // sortedCSVs->printAsc();
                    
                    for(int i = size-1; i >= 0; i--)
                    {
                        if(sortedTIFs->getAt(i)->getTile() != sortedCSVs->getAt(i)->getTile())
                        {
                            throw rsgis::RSGISException("TIF AND CSV lists are different. (tile number)");
                        }
                        else if(sortedTIFs->getAt(i)->getVersion() != sortedCSVs->getAt(i)->getVersion())
                        {
                            throw rsgis::RSGISException("TIF AND CSV lists are different. (version number)");
                        }
                        else if(sortedTIFs->getAt(i)->getOldVersion() != sortedCSVs->getAt(i)->getOldVersion())
                        {
                            throw rsgis::RSGISException("TIF AND CSV lists are different. (old version)");
                        }
                    }
                    std::cout << "Files have been checked and the corrsponding TIF and CSV files are present.\n";
                    std::cout << "Starting Rasterisation...\n";
                    
                    rasterisedefiniensCSV.rasteriseTiles(sortedTIFs, sortedCSVs, outputDIR);
                    
                    
                    sortedTIFs->clearListDelete();
                    delete sortedTIFs;
                    sortedCSVs->clearListDelete();
                    delete sortedCSVs;
                    
                    delete[] inputTIF;
                    delete[] inputCSV;
                }
                catch(rsgis::RSGISException& e)
                {
                    throw e;
                }
            }
            else
            {
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input CSV: " << this->inputCSV << std::endl;
                std::cout << "Output Image: " << this->outputImage << std::endl;

                try
                {
                    rasterisedefiniensCSV.rasteriseFile(this->inputImage, this->inputCSV, this->outputImage);
                }
                catch(rsgis::RSGISException& e)
                {
                    throw e;
                }
            }
            GDALDestroyDriverManager();
            std::cout << "Finished Rasterisation\n";
		}
		else if(option == RSGISExeImageUtils::printProj4)
		{
			std::cout << "Print Spatial Reference Proj4\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			GDALAllRegister();
			GDALDataset *inDataset = NULL;
			
			try
			{
				inDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				const char *wtkSpatialRef = inDataset->GetProjectionRef();
				OGRSpatialReference ogrSpatial = OGRSpatialReference(wtkSpatialRef);
				
				char **proj4spatialref = new char*[1];
				proj4spatialref[0] = new char[1000];
				ogrSpatial.exportToProj4(proj4spatialref);
				std::cout << proj4spatialref[0] << std::endl;
				
				GDALClose(inDataset);
				GDALDestroyDriverManager();
				
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::printWKT)
		{
			std::cout << "Print Spatial Reference WKT\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			
			GDALAllRegister();
			GDALDataset *inDataset = NULL;
			
			try
			{
				inDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				const char *wtkSpatialRef = inDataset->GetProjectionRef();
				
				std::cout << wtkSpatialRef << std::endl;
				
				GDALClose(inDataset);
				GDALDestroyDriverManager();
				
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::extract2dscatterptxt)
		{
			std::cout << "Extract image data to create 2D scatter plot (exported as ptxt)\n";
			
			std::cout << "Input Images:\n";
			for(int i = 0; i < this->numImages; ++i)
			{
				std::cout << i << ") " << this->inputImages[i] << std::endl;
			}
			std::cout << "Image Band 1: " << this->imgBand1 << std::endl;
			std::cout << "Image Band 2: " << this->imgBand2 << std::endl;
			std::cout << "Output File: " << this->outputFile << std::endl;
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::img::RSGISExport2DScatterPTxt *export2DScatter = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[numImages];
				
				for(int i = 0; i < numImages; ++i)
				{
					datasets[i] = (GDALDataset *) GDALOpenShared(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = std::string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
				}
					
				RSGISExportForPlottingIncremental *plotter = new RSGISExportForPlottingIncremental();
				plotter->openFile(this->outputFile, scatter2d);
				export2DScatter = new rsgis::img::RSGISExport2DScatterPTxt(plotter, imgBand1, imgBand2);
				calcImage = new rsgis::img::RSGISCalcImage(export2DScatter, "", true);
				
				calcImage->calcImage(datasets, numImages);
				
				plotter->close();
				delete plotter;
				
				
				for(int i = 0; i < numImages; ++i)
				{
					GDALClose(datasets[i]);
				}
				delete[] datasets;
				
				GDALDestroyDriverManager();
				delete calcImage;
				delete export2DScatter;
			}
			catch (rsgis::RSGISException e) 
			{
				std::cout << "Exception occured: " << e.what() << std::endl;
			}
			
			
			
		}
		else if(option == RSGISExeImageUtils::sgsmoothing)
		{
			std::cout << "Apply Savitzky-Golay Smoothing Filters to data\n";
			
			std::cout << "Input Image = " << this->inputImage << std::endl;
			std::cout << "Output Image = " << this->outputImage << std::endl;
			std::cout << "Image Band Values = " << this->bandFloatValuesVector << std::endl;
			std::cout << "Order = " << this->order << std::endl;
			std::cout << "Window = " << this->window << std::endl;
			
			GDALAllRegister();
			
			RSGISVectors vectorUtils;
			
			GDALDataset **datasets = NULL;
			
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			rsgis::img::RSGISCalcImageValue *calcImageValue = NULL;
			
			try 
			{
				if(order > window)
				{
					throw RSGISException("The window size needs to be at least as large as the order");
				}
				
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				Vector *bandValues = vectorUtils.readVectorFromTxt(this->bandFloatValuesVector);
				
				std::cout << "Input Image band values:\n";
				vectorUtils.printVector(bandValues);
				
				int numInBands = datasets[0]->GetRasterCount();
				
				calcImageValue = new rsgis::img::RSGISSavitzkyGolaySmoothingFilters(numInBands, this->order, this->window, bandValues);
				calcImage = new rsgis::img::RSGISCalcImage(calcImageValue, "", true);
				
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				delete calcImage;
				delete calcImageValue;
				
				vectorUtils.freeVector(bandValues);
				
				GDALClose(datasets[0]);
				delete[] datasets;
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
			GDALDestroyDriverManager();
		}
		else if(option == RSGISExeImageUtils::cumulativearea)
		{
			std::cout << "Calculate the cumulative area of the image profile/spectra\n";
			std::cout << "Input Image = " << this->inputImage << std::endl;
			std::cout << "Output Image = " << this->outputImage << std::endl;
			std::cout << "Image Band Widths = " << this->bandFloatValuesVector << std::endl;
			
			GDALAllRegister();
			
			RSGISMatrices matixUtils;
			
			GDALDataset **datasets = NULL;
			
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			rsgis::img::RSGISCalcImageValue *calcImageValue = NULL;
			
			try 
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				Matrix *bandValues = matixUtils.readMatrixFromTxt(this->inMatrixfile);
		
				int numInBands = datasets[0]->GetRasterCount();
				
				calcImageValue = new rsgis::img::RSGISCumulativeArea(numInBands, bandValues);
				calcImage = new rsgis::img::RSGISCalcImage(calcImageValue, "", true);
				
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				delete calcImage;
				delete calcImageValue;
				
				matixUtils.freeMatrix(bandValues);
				
				GDALClose(datasets[0]);
				delete[] datasets;
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
			GDALDestroyDriverManager();
		}
		else if(option == RSGISExeImageUtils::createimage)
		{
			std::cout << "Create a new blank image\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Num. Image bands: " << this->numBands << std::endl;
			std::cout << "Size: [" << this->width << "," << this->height << "]\n";
			std::cout << "TL Geo: [" << this->eastings << "," << this->northings << "]\n";
			std::cout << "Resolution: " << this->resolution << std::endl;
			std::cout << "Default Value: " << this->outValue << std::endl;
			std::cout << "Projection: " << this->proj << std::endl;
			
			rsgis::img::RSGISImageUtils imgUtils;
			try 
			{
				GDALAllRegister();
				double *transformation = new double[6];
				transformation[0] = eastings;
				transformation[1] = resolution;
				transformation[2] = 0;
				transformation[3] = northings;
				transformation[4] = 0;
				transformation[5] = resolution * (-1);
				
				string projection = "";
				if(proj != "")
				{
					OGRSpatialReference ogrSpatial = OGRSpatialReference();
					ogrSpatial.importFromProj4(proj.c_str());
					
					char **wktspatialref = new char*[1];
					wktspatialref[0] = new char[10000];
					ogrSpatial.exportToWkt(wktspatialref);			
					projection = std::string(wktspatialref[0]);
					OGRFree(wktspatialref);
				}
				
				GDALDataset* outImage = imgUtils.createBlankImage(outputImage, transformation, width, height, numBands, projection, outValue);
				GDALClose(outImage);
				GDALDestroyDriverManager();
			}
			catch(rsgis::img::RSGISImageBandException &e)
			{
				throw RSGISException(e.what());
			}
			catch (rsgis::RSGISImageException &e) 
			{
				throw RSGISException(e.what());
			}
		}
		else if(option == RSGISExeImageUtils::stretch)
		{
			std::cout << "Apply an enhancement stretch to the an input image - usually for visualisation\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			if(stretchType == linearMinMax)
			{
				std::cout << "Linear Min-Max stretch\n";
			}
			else if(stretchType == linearPercent)
			{
				std::cout << "Linear " << percent << " % stretch\n";
			}
			else if(stretchType == linearStdDev)
			{
				std::cout << "Linear " << stddev << " Standard Deviation stretch\n";
			}
			else if(stretchType == histogram)
			{
				std::cout << "Histogram stretch\n";
			}
			else if(stretchType == exponential)
			{
				std::cout << "Exponential stretch\n";
			}
			else if(stretchType == logarithmic)
			{
				std::cout << "Logarithmic stretch\n";
			}
			else if(stretchType == powerLaw)
			{
				std::cout << power << " Power Law stretch\n";
			}
			else
			{
				throw RSGISException("Stretch is not recognised.");
			}
            if(this->ignoreZeros)
            {
                std::cout << "Ignoring Zeros\n";
            }
			
			
			try
			{
				GDALAllRegister();
				rsgis::img::RSGISStretchImage *stretchImg = NULL;
				GDALDataset *inDataset = NULL;
				
				inDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

				stretchImg = new rsgis::img::RSGISStretchImage(inDataset, this->outputImage, this->ignoreZeros, this->imageFormat, this->outDataType);
				if(stretchType == linearMinMax)
				{
					stretchImg->executeLinearMinMaxStretch();
				}
				else if(stretchType == linearPercent)
				{
					stretchImg->executeLinearPercentStretch(this->percent);
				}
				else if(stretchType == linearStdDev)
				{
					stretchImg->executeLinearStdDevStretch(this->stddev);
				}
				else if(stretchType == histogram)
				{
					stretchImg->executeHistogramStretch();
				}
				else if(stretchType == exponential)
				{
					stretchImg->executeExponentialStretch();
				}
				else if(stretchType == logarithmic)
				{
					stretchImg->executeLogrithmicStretch();
				}
				else if(stretchType == powerLaw)
				{
					stretchImg->executePowerLawStretch(power);
				}
				else
				{
					throw RSGISException("Stretch is not recognised.");
				}
				
				GDALClose(inDataset);
				GDALDestroyDriverManager();
				delete stretchImg;
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
			
		}
		else if(option == RSGISExeImageUtils::huecolour)
		{
			std::cout << "Generate a colour image (through Hue) representing an image band - usually for visualisation\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Band: " << this->imgBand1 << std::endl;
			std::cout << "Range: [" << this->lowerRangeValue << "," << this->upperRangeValue << "]\n";
			std::cout << "Background value: " << this->nodataValue << std::endl;
			
			GDALAllRegister();
			
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			rsgis::img::RSGISCalcImageValue *calcImageValue = NULL;
			
			try
			{
				GDALDataset **datasets = NULL;
				
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				if(this->imgBand1 >= datasets[0]->GetRasterCount())
				{
					throw RSGISImageException("Insufficient number of bands in the input image for select band.");
				}
				
				calcImageValue = new rsgis::img::RSGISColourUsingHue(3, this->imgBand1, this->lowerRangeValue, this->upperRangeValue, this->nodataValue);
				calcImage = new rsgis::img::RSGISCalcImage(calcImageValue, "", true);
				
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				delete calcImage;
				delete calcImageValue;
								
				GDALClose(datasets[0]);
				delete[] datasets;
				GDALDestroyDriverManager();
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
			
		}
		else if(option == RSGISExeImageUtils::removespatialref)
		{
			std::cout << "Remove / define spatial reference to nothing and size set to pixel size\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
						
			try
			{
				rsgis::img::RSGISImageUtils imgUtils;
				imgUtils.copyImageRemoveSpatialReference(inputImage, outputImage);
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
			
		}
		else if(option == RSGISExeImageUtils::addnoise)
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
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
			rsgis::img::RSGISCalcImage *calcImg = NULL;
			
			if (this->noise == rsgis::img::percentGaussianNoise ) 
			{
				std::cout << "Adding " << this->scale * 100 << "% Gaussian Noise" << std::endl;
				rsgis::img::RSGISAddRandomGaussianNoisePercent *addNoise = NULL;
				try
				{
					addNoise = new rsgis::img::RSGISAddRandomGaussianNoisePercent(datasets[0]->GetRasterCount(),this->scale);
					calcImg = new rsgis::img::RSGISCalcImage(addNoise, "", true);
					calcImg->calcImage(datasets, 1, this->outputImage);
					delete addNoise;
				}
				catch(rsgis::RSGISException& e)
				{
					throw e;
				}
			}
			
			else
			{
				std::cout << "Adding random noise" << std::endl;
				rsgis::img::RSGISAddRandomNoise *addNoise = NULL;
				try
				{
					addNoise = new rsgis::img::RSGISAddRandomNoise(datasets[0]->GetRasterCount(),this->scale);
					calcImg = new rsgis::img::RSGISCalcImage(addNoise, "", true);
					calcImg->calcImage(datasets, 1, this->outputImage);
					delete addNoise;
				}
				catch(rsgis::RSGISException& e)
				{
					throw e;
				}
			}
			
			if(datasets != NULL)
			{
				GDALClose(datasets[0]);
				delete[] datasets;
			}
			delete calcImg;
						
		}
		else if(option == RSGISExeImageUtils::definespatialref)
		{
			std::cout << "Make a copy of the input image and define the projection and spatial locations\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Projection: " << this->proj << std::endl;
			std::cout << "TL [" << this->eastings << "," << this->northings << "]\n";
			std::cout << "Resolution [" << this->xRes << "," << this->yRes << "]\n";
			
			try
			{
				rsgis::img::RSGISImageUtils imgUtils;
				imgUtils.copyImageDefiningSpatialReference(inputImage, outputImage, this->proj, this->eastings, this->northings, this->xRes, this->yRes);
			}
			catch(rsgis::RSGISException& e)
			{
				throw e;
			}
		}
        else if(option == RSGISExeImageUtils::subset)
		{
			std::cout << "Subset image to vector\n";
			GDALAllRegister();
			OGRRegisterAll();
			
			GDALDataset **dataset = NULL;
			OGRDataSource *inputVecDS = NULL;
			OGRLayer *inputVecLayer = NULL;
			
			rsgis::img::RSGISCopyImage *copyImage = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
            
			rsgis::vec::RSGISVectorUtils vecUtils;
			
			string vectorLayerName = vecUtils.getLayerName(this->inputVector);
			int numImageBands = 0;
			
			try
			{
				// Open Image
				dataset = new GDALDataset*[1];
				std::cout << this->inputImage << std::endl;
				dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset[0] == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				numImageBands = dataset[0]->GetRasterCount();
				std::cout << "Raster Band Count = " << numImageBands << std::endl;
				
				// Open vector
				inputVecDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputVecDS == NULL)
				{
					string message = std::string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputVecLayer = inputVecDS->GetLayerByName(vectorLayerName.c_str());
				if(inputVecLayer == NULL)
				{
					string message = std::string("Could not open vector layer ") + vectorLayerName;
					throw RSGISFileException(message.c_str());
				}
                OGREnvelope ogrExtent;
                inputVecLayer->GetExtent(&ogrExtent);
                Envelope extent = Envelope(ogrExtent.MinX, ogrExtent.MaxX, ogrExtent.MinY, ogrExtent.MaxY);
				
				copyImage = new rsgis::img::RSGISCopyImage(numImageBands);
				calcImage = new rsgis::img::RSGISCalcImage(copyImage, "", true);
                calcImage->calcImageInEnv(dataset, 1, outputImage, &extent);
				
				GDALClose(dataset[0]);
				delete[] dataset;
				OGRDataSource::DestroyDataSource(inputVecDS);
				OGRCleanupAll();
				GDALDestroyDriverManager();
				delete calcImage;
				delete copyImage;
			}
			catch(rsgis::RSGISException e)
			{
				std::cout << "RSGISException caught: " << e.what() << std::endl;
			}
		}
        else if(option == RSGISExeImageUtils::subset2polys)
		{
			std::cout << "Subset image to bounding box of polygons\n";
			GDALAllRegister();
			OGRRegisterAll();
			
			GDALDataset **dataset = NULL;
			OGRDataSource *inputVecDS = NULL;
			OGRLayer *inputVecLayer = NULL;
			
			rsgis::img::RSGISCopyImage *copyImage = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			rsgis::vec::RSGISVectorIO vecIO;
			rsgis::vec::RSGISPolygonData **polyData = NULL;
			rsgis::vec::RSGISImageTileVector **data = NULL;
			rsgis::vec::RSGISVectorUtils vecUtils;
			
            std::string vectorLayerName = vecUtils.getLayerName(this->inputVector);
			int numImageBands = 0;
			int numFeatures = 0;
            std::string outputFilePath;
			
			try
			{
				// Open Image
				dataset = new GDALDataset*[1];
				std::cout << this->inputImage << std::endl;
				dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset[0] == NULL)
				{
					string message = std::string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				numImageBands = dataset[0]->GetRasterCount();
				std::cout << "Raster Band Count = " << numImageBands << std::endl;
				
				// Open vector
				inputVecDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputVecDS == NULL)
				{
					string message = std::string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputVecLayer = inputVecDS->GetLayerByName(vectorLayerName.c_str());
				if(inputVecLayer == NULL)
				{
					string message = std::string("Could not open vector layer ") + vectorLayerName;
					throw RSGISFileException(message.c_str());
				}
				
				// READ IN SHAPEFILE
				numFeatures = inputVecLayer->GetFeatureCount();
				polyData = new rsgis::vec::RSGISPolygonData*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					polyData[i] = new rsgis::vec::RSGISImageTileVector(this->filenameAttribute);
				}
				std::cout << "Reading in " << numFeatures << " features\n";
				vecIO.readPolygons(inputVecLayer, polyData, numFeatures);
				
				//Convert to RSGISImageTileVector
				data = new rsgis::vec::RSGISImageTileVector*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					data[i] = dynamic_cast<rsgis::vec::RSGISImageTileVector*>(polyData[i]);
				}
				delete[] polyData;
				
				copyImage = new rsgis::img::RSGISCopyImage(numImageBands);
				calcImage = new rsgis::img::RSGISCalcImage(copyImage, "", true);
                
				unsigned int failCount = 0;
				for(int i = 0; i < numFeatures; i++)
				{
					outputFilePath = this->outputImage + data[i]->getFileName() + ".env";
					std::cout << i << ": " << outputFilePath << std::endl;
                    try
                    {
                        calcImage->calcImageInEnv(dataset, 1, outputFilePath, data[i]->getBBox());
                    }
                    catch (rsgis::img::RSGISImageBandException e)
                    {
                        ++failCount;
                        if(failCount <= 100)
                        {
                            cerr << "RSGISException caught: " << e.what() << std::endl;
                            cerr << "Check output path exists and is writable and all polygons in shapefile:" << std::endl;
                            cerr << " " << this->inputVector << std::endl;
                            cerr << "Are completely within:" << std::endl;
                            cerr << " " << this->inputImage << std::endl;
                        }
                        else
                        {
                            cerr << "Over 100 exceptions have been caught, exiting" << std::endl;
                            throw e;
                        }
                    }
				}
				
				GDALClose(dataset[0]);
				delete[] dataset;
				OGRDataSource::DestroyDataSource(inputVecDS);
				OGRCleanupAll();
				GDALDestroyDriverManager();
				delete calcImage;
				delete copyImage;
			}
			catch(rsgis::RSGISException e)
			{
				cerr << "RSGISException caught: " << e.what() << std::endl;
			}
		}
		else if(option == RSGISExeImageUtils::pansharpen)
		{
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			datasets = new GDALDataset*[1];
			
			datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(datasets[0] == NULL)
			{
				string message = std::string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
			
			int numRasterBands = datasets[0]->GetRasterCount();
			
			// Calculate statistics
			float *imageStats = new float[4];  // Set up an array to hold image stats
			
			std::cout << "Calculating image mean.." << std::endl;
			rsgis::img::RSGISHCSPanSharpenCalcMeanStats *panMean = new rsgis::img::RSGISHCSPanSharpenCalcMeanStats(numRasterBands, imageStats);
			rsgis::img::RSGISCalcImage *calcImageMean = new rsgis::img::RSGISCalcImage(panMean, "", true);
			calcImageMean->calcImage(datasets, 1);
			panMean->returnStats();
			
			std::cout << "Calculating image standard deviation.." << std::endl;
			rsgis::img::RSGISHCSPanSharpenCalcSDStats *panSD = new rsgis::img::RSGISHCSPanSharpenCalcSDStats(numRasterBands, imageStats);
			rsgis::img::RSGISCalcImage *calcImageSD = new rsgis::img::RSGISCalcImage(panSD, "", true);
			calcImageSD->calcImage(datasets, 1);
			panSD->returnStats();
			
			/*std::cout << "meanMS = " << imageStats[0] << std::endl;
			std::cout << "meanPAN = "<< imageStats[1] << std::endl;
			std::cout << "sdMS = " << imageStats[2] << std::endl;
			std::cout << "sdPAN = "<< imageStats[3] << std::endl;*/
			
			std::cout << "Pan sharpening.." << std::endl;
			rsgis::img::RSGISHCSPanSharpen *panSharpen = new rsgis::img::RSGISHCSPanSharpen(numRasterBands - 1, imageStats);
			calcImage = new rsgis::img::RSGISCalcImage(panSharpen, "", true);
			// naive mode
			//calcImage->calcImage(datasets, 1, this->outputImage);
			// smart mode 
			calcImage->calcImageWindowData(datasets, 1, this->outputImage, this->panWinSize);
			
			
			// Tidy up
			GDALClose(datasets[0]);
			delete[] datasets;
			
			delete calcImageMean;
			delete calcImageSD;
			delete calcImage;
			delete panMean;
			delete panSD;
			delete panSharpen;
			delete[] imageStats;
		}
        else if(option == RSGISExeImageUtils::createslices)
		{
            std::cout << "Create Image slices from a multiband input image\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image Base: " << this->outputImage << std::endl;
            
			GDALAllRegister();
			GDALDataset *dataset = NULL;
			
			dataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(dataset == NULL)
			{
				string message = std::string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
            
            rsgis::img::RSGISImageUtils imageUtils;
            try 
            {
                imageUtils.createImageSlices(dataset, outputImage);
            } 
            catch (RSGISImageException &e) 
            {
                throw e;
            }
			
			// Tidy up
			GDALClose(dataset);
            GDALDestroyDriverManager();
		}
        else if(option == RSGISExeImageUtils::clump)
        {
            std::cout << "Clump the input image for a given values.\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            
            GDALAllRegister();
			GDALDataset *inDataset = NULL;
			inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(inDataset == NULL)
			{
				string message = std::string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
            
            /*
            unsigned long width = inDataset->GetRasterXSize();
            unsigned long height = inDataset->GetRasterYSize();
            unsigned int numBands = inDataset->GetRasterCount();
            */
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *processingDataset = NULL;
            
            std::cout << "Copying input dataset\n";
            if(this->processInMemory)
            {
                std::cout << "Processing in Memory\n";
                processingDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                std::cout << "Processing using Disk\n";
                processingDataset = imgUtils.createCopy(inDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            imgUtils.copyUIntGDALDataset(inDataset, processingDataset);
            
            
            std::cout << "Performing clumping\n";
            rsgis::img::RSGISCalcImgValueAlongsideOut *clumpImg = new rsgis::img::RSGISClumpImage();
            rsgis::img::RSGISCalcImgAlongsideOut calcImg = rsgis::img::RSGISCalcImgAlongsideOut(clumpImg);
            calcImg.calcImageIterate(processingDataset);
            delete clumpImg;
            
            if(this->processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(processingDataset, outDataset);
                GDALClose(outDataset);
            }
			
			// Tidy up
			GDALClose(inDataset);
            GDALClose(processingDataset);
            GDALDestroyDriverManager();
        }
		else if(option == RSGISExeImageUtils::imageComposite)
		{
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
			
			datasets = new GDALDataset*[1];
			
			datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(datasets[0] == NULL)
			{
				string message = std::string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
			
			int numRasterBands = datasets[0]->GetRasterCount();
			
            int numOutputBands = numRasterBands / this->compositeBands;
            
            std::cout << "Calculating statistics for every " << this->compositeBands << " bands of a " << numRasterBands << " band input image to create a " << numOutputBands << " band output image" << std::endl;
            
            
			rsgis::img::RSGISImageComposite *compositeImage = new rsgis::img::RSGISImageComposite(numOutputBands, this->compositeBands, this->outCompStat);
			calcImage = new rsgis::img::RSGISCalcImage(compositeImage, "", true);
			calcImage->calcImage(datasets, 1, this->outputImage);
			
			// Tidy up
			GDALClose(datasets[0]);
			delete[] datasets;
			
			delete calcImage;
			delete compositeImage;
		}
        else if(option == RSGISExeImageUtils::relabel)
        {
            std::cout << "A command to relabel image pixel using a look up table from a gmtxt matrix file.\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Look Up Table: " << this->lutMatrixFile << std::endl;
            
            try 
            {
                GDALAllRegister();
                GDALDataset *inDataset = NULL;
                inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
                if(inDataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                rsgis::img::RSGISRelabelPixelValuesFromLUT relabelPixels;
                relabelPixels.relabelPixelValues(inDataset, this->outputImage, this->lutMatrixFile, this->imageFormat);
                
                GDALClose(inDataset);
                GDALDestroyDriverManager();
            } 
            catch (rsgis::RSGISException &e) 
            {
                throw e;
            }
            
        }
        else if(option == RSGISExeImageUtils::assignproj)
        {
            std::cout << "Assign and update and image to a specific projection\n";
            std::cout << "Image: " << this->inputImage << std::endl;
            std::cout << "Projection File: " << this->projFile << std::endl;
            
            try 
            {
                GDALAllRegister();
                GDALDataset *inDataset = NULL;
                inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                if(inDataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                rsgis::utils::RSGISTextUtils textUtils;
                std::string projWKTStr = textUtils.readFileToString(this->projFile);
                
                inDataset->SetProjection(projWKTStr.c_str());
                
                GDALClose(inDataset);
                GDALDestroyDriverManager();
            } 
            catch (rsgis::RSGISException &e) 
            {
                throw e;
            }
        }
        else if(option == RSGISExeImageUtils::popimgstats)
        {
            std::cout << "Populate an image with image statistics and image pyramids\n";
            std::cout << "Image: " << this->inputImage << std::endl;
            if(this->useIgnoreVal)
            {
                std::cout << "Ignore Val: " << this->nodataValue << std::endl;
            }
            if(this->calcImgPyramids)
            {
                std::cout << "Calculating image pyramids\n";
            }
            
            try 
            {
                GDALAllRegister();
                GDALDataset *inDataset = NULL;
                inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                if(inDataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                rsgis::img::RSGISPopWithStats popWithStats;
                popWithStats.calcPopStats( inDataset, this->useIgnoreVal, this->nodataValue, this->calcImgPyramids );
                
                
                GDALClose(inDataset);
                GDALDestroyDriverManager();
            } 
            catch (rsgis::RSGISException &e) 
            {
                throw e;
            }
        }
        else if(option == RSGISExeImageUtils::createcopy)
        {
            std::cout << "Create a new image from an existing image\n";
            std::cout << "Image: " << this->inputImage << std::endl;
            std::cout << "Output: " << this->outputImage << std::endl;
            std::cout << "Data Value: " << this->dataValue << std::endl;
            std::cout << "Num Image Bands: " << this->numBands << std::endl;
            std::cout << "Image format: " << this->imageFormat << std::endl;
            
            try 
            {
                GDALAllRegister();
                GDALDataset *inDataset = NULL;
                inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                if(inDataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                rsgis::img::RSGISImageUtils imgUtils;
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, this->numBands, this->outputImage, this->imageFormat, outDataType);
                imgUtils.assignValGDALDataset(outDataset, this->dataValue);
                
                GDALClose(inDataset);
                GDALClose(outDataset);
                GDALDestroyDriverManager();
            } 
            catch (rsgis::RSGISException &e) 
            {
                throw e;
            }
        }
        else if(option == RSGISExeImageUtils::createKMLFile)
		{
			rsgis::img::RSGISImageUtils imgUtils;
            
            std::cout << "Create KML Text file for:" << this->inputImage << std::endl;
            std::cout << "Saving to: " <<  this->outKMLFile << std::endl;
            std::cout << "NOTE: This command assumed relavent pre-processing has already" << std::endl;
            std::cout << "been carried out for the image and will only work for a stretched " << std::endl;
            std::cout << "three band image, readable by GoogleEarth" << std::endl;
            imgUtils.createKMLText(this->inputImage, this->outKMLFile);
		}
        else if(option == RSGISExeImageUtils::assignspatialinfo)
        {
            std::cout << "Assign and update an images spatial info.\n";
            std::cout << "Image: " << this->inputImage << std::endl;
            std::cout << "TL  [: " << this->eastings << "," << this->northings << "]" << std::endl;
            std::cout << "Res [: " << this->xRes << "," << this->yRes << "]" << std::endl;
            std::cout << "Rot [: " << this->xRot << "," << this->yRot << "]" << std::endl;
            
            try 
            {
                GDALAllRegister();
                GDALDataset *inDataset = NULL;
                inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                if(inDataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                double *trans = new double[6];
                trans[0] = this->eastings;
                trans[1] = this->xRes; 
                trans[2] = this->xRot; 
                trans[3] = this->northings;
                trans[4] = this->yRot;
                trans[5] = this->yRes;
                
                inDataset->SetGeoTransform(trans);
                delete[] trans;
                
                GDALClose(inDataset);
                GDALDestroyDriverManager();
            } 
            catch (rsgis::RSGISException &e) 
            {
                throw e;
            }
        }
		else
		{
			std::cout << "Options not recognised\n";
		}
		
	}
}


void RSGISExeImageUtils::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeImageUtils::colour)
		{
			std::cout << "Colour Image\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			for(int i = 0; i < numClasses; i++)
			{
				std::cout << i <<  ") Class " << classColour[i]->className << " with ID ";
				std::cout << classColour[i]->classID << " uses image band " << classColour[i]->imgBand << "\n";
				std::cout << "Lower = " << classColour[i]->lower << std::endl;
				std::cout << "Upper = " << classColour[i]->upper << std::endl;
				std::cout << "Red = " << classColour[i]->red << std::endl;
				std::cout << "Green = " << classColour[i]->green << std::endl;
				std::cout << "Blue = " << classColour[i]->blue << std::endl;
			}
		}
        else if(option == RSGISExeImageUtils::colourimagebands)
		{
			std::cout << "Colour Image\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			for(int i = 0; i < numClasses; i++)
			{
				std::cout << i <<  ") Class " << classColour[i]->className << " with ID ";
                std::cout << classColour[i]->classID << std::endl;
                std::cout << "Lower = " << classColour[i]->lower << std::endl;
                std::cout << "Upper = " << classColour[i]->upper << std::endl;
                std::cout << "Red = " << classColour[i]->red << std::endl;
                std::cout << "Green = " << classColour[i]->green << std::endl;
                std::cout << "Blue = " << classColour[i]->blue << std::endl;
			}
		}
		else if(option == RSGISExeImageUtils::mosaic)
		{
			for(int i = 0; i < this->numImages; i++)
			{
				std::cout << "Input Image: " << this->inputImages[i] << std::endl;
			}
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "No Data Value: " << this->nodataValue << std::endl;
			
			if(projFromImage)
			{
				std::cout << "Projection is being taken from the first image in the list.\n";
			}
			else
			{
				std::cout << "Projection: " << proj << std::endl;
			}
		}
		else if(option == RSGISExeImageUtils::include)
		{
			std::cout << "Base Image: " << this->inputImage << std::endl;
			for(int i = 0; i < this->numImages; i++)
			{
				std::cout << "Input Image: " << this->inputImages[i] << std::endl;
			}
		}
		else if(option == RSGISExeImageUtils::cut2poly)
		{
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Input Vector: " << this->inputVector << std::endl;
			std::cout << "Filename attribute: " << this->filenameAttribute << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "No Data Value: " << this->nodataValue << std::endl;
		}
		else if(option == RSGISExeImageUtils::mask)
		{
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Image Mask: " << this->imageMask << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Mask Value: " << this->maskValue << std::endl;
		}
		else if(option == RSGISExeImageUtils::resample)
		{
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Scale Image: " << this->resampleScale << std::endl;
			if(this->interpolator == RSGISExeImageUtils::cubic)
			{
				std::cout << "Using a cubic interpolator\n";
			}
			else if(this->interpolator == RSGISExeImageUtils::billinearArea)
			{
				std::cout << "Using a bilinear (area) interpolator\n";
			}
			else if(this->interpolator == RSGISExeImageUtils::billinearPt)
			{
				std::cout << "Using a bilinear (point) interpolator\n";
			}
			else if(this->interpolator == RSGISExeImageUtils::nn)
			{
				std::cout << "Using a nearest neighbour interpolator\n";
			}
			else if(this->interpolator == RSGISExeImageUtils::trangular)
			{
				std::cout << "Using a triangular interpolator\n";
			}
		}
		else if(option == RSGISExeImageUtils::rasterisedefiniens)
		{
			std::cout << "Rasterise Definiens\n";
			std::cout << "Input DIR: " << this->inputDIR << std::endl;
			std::cout << "Output DIR: " << this->outputDIR << std::endl;
			if(projFromImage)
			{
				std::cout << "Projection is being taken from the first image in the list.\n";
			}
			else
			{
				std::cout << "Projection: " << proj << std::endl;
			}
		}
		else if(option == RSGISExeImageUtils::printProj4)
		{
			std::cout << "Print Spatial Reference Proj4\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
		}
		else if(option == RSGISExeImageUtils::printWKT)
		{
			std::cout << "Print Spatial Reference WKT\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
		}
		else if(option == RSGISExeImageUtils::extract2dscatterptxt)
		{
			std::cout << "Extract image data to create 2D scatter plot (exported as ptxt)\n";
			
			std::cout << "Input Images:\n";
			for(int i = 0; i < this->numImages; ++i)
			{
				std::cout << i << ") " << this->inputImages[i] << std::endl;
			}
			std::cout << "Image Band 1: " << this->imgBand1 << std::endl;
			std::cout << "Image Band 2: " << this->imgBand2 << std::endl;
			std::cout << "Output File: " << this->outputFile << std::endl;
		}
		else if(option == RSGISExeImageUtils::sgsmoothing)
		{
			std::cout << "Apply Savitzky-Golay Smoothing Filters to data\n";
			
			std::cout << "Input Image = " << this->inputImage << std::endl;
			std::cout << "Output Image = " << this->outputImage << std::endl;
			std::cout << "Image Band Values = " << this->bandFloatValuesVector << std::endl;
			std::cout << "Order = " << this->order << std::endl;
			std::cout << "Window = " << this->window << std::endl;
		}
		else if(option == RSGISExeImageUtils::cumulativearea)
		{
			std::cout << "Calculate the cumulative area of the image profile/spectra\n";
			std::cout << "Input Image = " << this->inputImage << std::endl;
			std::cout << "Output Image = " << this->outputImage << std::endl;
			std::cout << "Image Band Widths = " << this->bandFloatValuesVector << std::endl;
		}
		else if(option == RSGISExeImageUtils::createimage)
		{
			std::cout << "Create a new blank image\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Num. Image bands: " << this->numBands << std::endl;
			std::cout << "Size: [" << this->width << "," << this->height << "]\n";
			std::cout << "TL Geo: [" << this->eastings << "," << this->northings << "]\n";
			std::cout << "Resolution: " << this->resolution << std::endl;
			std::cout << "Default Value: " << this->outValue << std::endl;
			std::cout << "Projection: " << this->proj << std::endl;
		}
		else if(option == RSGISExeImageUtils::stretch)
		{
			std::cout << "Apply an enhancement stretch to the an input image - usually for visualisation\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			if(stretchType == linearMinMax)
			{
				std::cout << "Linear Min-Max stretch\n";
			}
			else if(stretchType == linearPercent)
			{
				std::cout << "Linear " << percent << " % stretch\n";
			}
			else if(stretchType == linearStdDev)
			{
				std::cout << "Linear " << stddev << " Standard Deviation stretch\n";
			}			
			else if(stretchType == histogram)
			{
				std::cout << "Histogram stretch\n";
			}
			else if(stretchType == exponential)
			{
				std::cout << "Exponential stretch\n";
			}
			else if(stretchType == logarithmic)
			{
				std::cout << "Logarithmic stretch\n";
			}
			else if(stretchType == powerLaw)
			{
				std::cout << "Power Law stretch\n";
			}
			else
			{
				throw RSGISException("Stretch is not recognised.");
			}
            
            if(this->ignoreZeros)
            {
                std::cout << "Ignoring Zeros\n";
            }
		}
		else if(option == RSGISExeImageUtils::huecolour)
		{
			std::cout << "Generate a colour image (through Hue) representing an image band - usually for visualisation\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Band: " << this->imgBand1 << std::endl;
			std::cout << "Range: [" << this->lowerRangeValue << "," << this->upperRangeValue << "]\n";
			std::cout << "Background value: " << this->nodataValue << std::endl;
		}
		else if(option == RSGISExeImageUtils::definespatialref)
		{
			std::cout << "Make a copy of the input image and define the projection and spatial locations\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
			std::cout << "Output Image: " << this->outputImage << std::endl;
			std::cout << "Projection: " << this->proj << std::endl;
			std::cout << "TL [" << this->eastings << "," << this->northings << "]\n";
			std::cout << "Resolution [" << this->xRes << "," << this->yRes << "]\n";
		}
        else if(option == RSGISExeImageUtils::createslices)
		{
            std::cout << "Create Image slices from a multiband input image\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image Base: " << this->outputImage << std::endl;
        }
        else if(option == RSGISExeImageUtils::clump)
        {
            std::cout << "Clump the input image for a given values.\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
        }
        else if(option == RSGISExeImageUtils::relabel)
        {
            std::cout << "A command to relabel image pixel using a look up table from a gmtxt matrix file.\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Look Up Table: " << this->lutMatrixFile << std::endl;
        }
        else if(option == RSGISExeImageUtils::assignproj)
        {
            std::cout << "Assign and update and image to a specific projection\n";
            std::cout << "Image: " << this->inputImage << std::endl;
            std::cout << "Projection File: " << this->projFile << std::endl;
        }
        else if(option == RSGISExeImageUtils::popimgstats)
        {
            std::cout << "Populate an image with image statistics and image pyramids\n";
            std::cout << "Image: " << this->inputImage << std::endl;
            if(this->useIgnoreVal)
            {
                std::cout << "Ignore Val: " << this->nodataValue << std::endl;
            }
            if(this->calcImgPyramids)
            {
                std::cout << "Calculating image pyramids\n";
            }
        }
        else if(option == RSGISExeImageUtils::createcopy)
        {
            std::cout << "Create a new image from an existing image\n";
            std::cout << "Image: " << this->inputImage << std::endl;
            std::cout << "Output: " << this->outputImage << std::endl;
            std::cout << "Data Value: " << this->dataValue << std::endl;
            std::cout << "Num Image Bands: " << this->numBands << std::endl;
            std::cout << "Image format: " << this->imageFormat << std::endl;
        }
        else if(option == RSGISExeImageUtils::assignspatialinfo)
        {
            std::cout << "Assign and update an images spatial info.\n";
            std::cout << "Image: " << this->inputImage << std::endl;
            std::cout << "TL  [: " << this->eastings << "," << this->northings << "]" << std::endl;
            std::cout << "Res [: " << this->xRes << "," << this->yRes << "]" << std::endl;
            std::cout << "Rot [: " << this->xRot << "," << this->yRot << "]" << std::endl;
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

void RSGISExeImageUtils::help()
{
    std::cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << std::endl;
    std::cout << "<!-- A command that will generate an RGB colour image based on thresholds applied to the input image - useful for generating quicklooks and visualisation of classification -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"colourimage\" image=\"image.env\" output=\"image_out.env\">" << std::endl;
    std::cout << "    <rsgis:colour name=\"class_name_1\" id=\"int\" band=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << std::endl;
    std::cout << "    <rsgis:colour name=\"class_name_2\" id=\"int\" band=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << std::endl;
    std::cout << "    <rsgis:colour name=\"class_name_3\" id=\"int\" band=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "<!-- A command that will generate an RGB colour image based on thresholds applied to each input image band, generating a new image for each input band - useful for generating quicklooks and visualisation of classification.-->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"colourimagebands\" image=\"image.env\" output=\"image_out_base\">" << std::endl;
    std::cout << "    <rsgis:colour name=\"class_name_1\" id=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << std::endl;
    std::cout << "    <rsgis:colour name=\"class_name_2\" id=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << std::endl;
    std::cout << "    <rsgis:colour name=\"class_name_3\" id=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "<!-- A command to mosaic a set of input images to generate a single output image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"mosaic\" output=\"image_out.env\" nodata=\"float=0\" skipValue=\"float (optional)\" skipLowerThreash=\"float (optional)\" skipUpperThreash=\"float (optional)\"  proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" setSkipBand=\"1\" >" << std::endl;
    std::cout << "    <rsgis:image file=\"image1\" />" << std::endl;
    std::cout << "    <rsgis:image file=\"image2\" />" << std::endl;
    std::cout << "    <rsgis:image file=\"image3\" />" << std::endl;
    std::cout << "    <rsgis:image file=\"image4\" />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "<!-- A command to mosaic a set of input images from a directory to generate a single output image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"mosaic\" dir=\"directory\" ext=\"file_extension\" output=\"image_out.env\" nodata=\"float=0\" skipValue=\"float (optional)\" skipLowerThreash=\"float (optional)\" skipUpperThreash=\"float (optional)\"  proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to include a set of input images into an existing image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"include\" image=\"base_image.env\" >" << std::endl;
    std::cout << "    <rsgis:image file=\"image1\" />" << std::endl;
    std::cout << "    <rsgis:image file=\"image2\" />" << std::endl;
    std::cout << "    <rsgis:image file=\"image3\" />" << std::endl;
    std::cout << "    <rsgis:image file=\"image4\" />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "<!-- A command to include a set of input images from a directory into an existing image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"include\" image=\"base_image.env\" dir=\"directory\" ext=\"file_extension\" />" << std::endl;
    std::cout << "<!-- A command to cut an image to an input shapefile where each polygon geometry will created a new output image representing the region within the polygon -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"cut2poly\" image=\"image.env\" output=\"image_out_base\" vector=\"vector.shp\" outfilename=\"attribute\" nodata=\"float\" />" << std::endl;
    std::cout << "<!-- A command to mask the input image with a second 'mask' image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"mask\" image=\"image.env\" mask=\"mask.env\" output=\"image_out.env\" maskvalue=\"float\" />" << std::endl;
    std::cout << "<!-- A command resample an input image to another resolution -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"resample\" image=\"image.env\" output=\"image_out.env\" scale=\"float\" interpolation=\"Cubic | BilinearArea | BilinearPoint | NN | Triangular\" />" << std::endl;
    std::cout << "<!-- TODO A command to create a multiband raster image, based on thematic rasters exported from Definiens. The values for each band are held in a csv file (exported with raster image from Definiens -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"rasterisedefiniens\" [inDIR=\"/input/directory/\" outDIR=\"/output/directory\"] [image=\"image.env\" csv=\"input.csv\" output=\"image_out.env\"] proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command which prints (to the console) the proj4 std::string representing the projection of the inputted image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"printProj4\" image=\"image.env\" />" << std::endl;
    std::cout << "<!-- A command which prints (to the console) the WKT std::string representing the projection of the inputted image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"printWKT\" image=\"image.env\" />" << std::endl;
    std::cout << "<!-- A command extracts pixel values from two image bands and output them as 2D scatter ptxt file -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"extract2dscatterptxt\" image=\"image.env\" band1=\"int\" band2=\"int\" output=\"string\" />" << std::endl;
    std::cout << "<!-- A command extracts pixel values from two image bands (bands are numbered sequencially down the list of input files) and output them as 2D scatter ptxt file -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"extract2dscatterptxt\" band1=\"int\" band2=\"int\" output=\"string\" >" << std::endl;
    std::cout << "    <rsgis:image file=\"image1\" />" << std::endl;
    std::cout << "    <rsgis:image file=\"image2\" />" << std::endl;
    std::cout << "    <rsgis:image file=\"image3\" />" << std::endl;
    std::cout << "    <rsgis:image file=\"image4\" />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "<!-- A command to smooth an spectral profiles of the input image pixels -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"sgsmoothing\" image=\"image.env\" output=\"image_out.env\" order=\"int\" window=\"int\" imagebands=\"vector.mtxt\"/>" << std::endl;
    std::cout << "<!-- A command to generate a cumulativeAreaImage from a spectral curve (or profile) -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"cumulativearea\" image=\"image.env\" output=\"image_out.env\" imagebands=\"matrix.mtxt\"/>" << std::endl;
    std::cout << "<!-- A command to generate new image with a default value -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"createimage\" output=\"image_out.env\" numbands=\"int\" width=\"int\" height=\"int\" resolution=\"float\" eastings=\"double\" northings=\"double\" proj4=\"string\" value=\"float\"/>" << std::endl;
    std::cout << "<!-- A command to apply an enhancement stretch an images pixel values to a range of 0 to 255 - normally used for visualisation -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"stretch\" image=\"image.env\" output=\"image_out.env\" ignorezeros=\"yes | no\" stretch=\"LinearMinMax | LinearPercent | LinearStdDev | Histogram | Exponential | Logarithmic | PowerLaw\" percent=\"float - only LinearPercent\" stddev=\"float - only LinearStdDev\" power=\"float - only PowerLaw\"/>" << std::endl;
    std::cout << "<!-- A command to colour to generate a colour image, using the Hue of a HSV transformation, for a particular input image band -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"huecolour\" image=\"image.env\" output=\"image_out.env\" band=\"int\" lowvalue=\"float\" highvalue=\"float\" background=\"float\" />" << std::endl;
    std::cout << "<!-- A command to remove / define spatial reference to nothing and size set to pixel size -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"removespatialref\" image=\"image.env\" output=\"image_out.env\" />" << std::endl;
    std::cout << "<!-- A command to add noise to an image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"addnoise\" type=\"randomNoise | percentGaussianNoise\" scale=\"float\" image=\"image.env\" output=\"image_out.env\"/>" << std::endl;
    std::cout << "<!-- A command to subset an image to the same extent as a shapefile -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"subset\" image=\"image.env\" output=\"output_img.env\" vector=\"vector.shp\" />" << std::endl;
    std::cout << "<!-- A command to subset an image to polygons within shapefile -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"subset2polys\" image=\"image.env\" output=\"image_out_base\" vector=\"vector.shp\" outfilename=\"attribute\" />" << std::endl;
    std::cout << "<!-- A command to pan sharpen an image, takes stack of multispectral image (resampled to pan resolution) and panchromatic image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"pansharpen\" image=\"ms_pan_img.env\" output=\"ps_image.env\" />" << std::endl;
    std::cout << "<!-- A command to generate a set of slices from a multi-band image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"createslices\" image=\"image.env\" output=\"image_out_base\" />" << std::endl;
    std::cout << "<!-- A command to clump an image for a given pixel values -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"clump\" image=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << std::endl;
    std::cout << "<!-- A command to create a composite image from a multi-band input image -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"composite\" image=\"image.env\" output=\"image_out.env\" compositeBands=\"int\" stats=\"mean | min | max | range\" />" << std::endl;
    std::cout << "<!-- A command to relabel image pixel using a look up table from a gmtxt matrix file (m=2 n=X) -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"relabel\" image=\"image.env\" output=\"image_out.env\" lut=\"matrix.gmtxt\" />" << std::endl;
    std::cout << "<!-- A command to assign and update and image to a specific projection -->" << std::endl;
    std::cout << "<rsgis:command algor=\"imageutils\" option=\"assignproj\" image=\"image.env\" projwkt=\"txt.wkt\" />" << std::endl;
	std::cout << "</rsgis:commands>\n";
}

string RSGISExeImageUtils::getDescription()
{
	return "Image utilities.";
}

string RSGISExeImageUtils::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeImageUtils::~RSGISExeImageUtils()
{

}

}




