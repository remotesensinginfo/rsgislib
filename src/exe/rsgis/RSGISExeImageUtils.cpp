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

RSGISAlgorithmParameters* RSGISExeImageUtils::getInstance()
{
	return new RSGISExeImageUtils();
}

void RSGISExeImageUtils::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;
	RSGISFileUtils fileUtils;
    RSGISTextUtils textUtils;

	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *optionColour = XMLString::transcode("colourimage");
	XMLCh *optionMosaic = XMLString::transcode("mosaic");
	XMLCh *optionInclude = XMLString::transcode("include");
	XMLCh *optionCut2Poly = XMLString::transcode("cut2poly");
    XMLCh *optionCut2Polys = XMLString::transcode("cut2polys");
	XMLCh *optionMask = XMLString::transcode("mask");
	XMLCh *optionResample = XMLString::transcode("resample");
	XMLCh *optionRasteriseDef = XMLString::transcode("rasterisedefiniens");
	XMLCh *projImage = XMLString::transcode("IMAGE");
	XMLCh *projOSGB = XMLString::transcode("OSGB");
    XMLCh *projNZ2000 = XMLString::transcode("NZ2000");
    XMLCh *projNZ1949 = XMLString::transcode("NZ1949");
	XMLCh *rsgisimageXMLStr = XMLString::transcode("rsgis:image");
	XMLCh *optionPrintProj4 = XMLString::transcode("printProj4");
	XMLCh *optionPrintWKT = XMLString::transcode("printWKT");
	XMLCh *optionExtract2DScatterPtxt = XMLString::transcode("extract2dscatterptxt");
	XMLCh *optionSGSmoothing = XMLString::transcode("sgsmoothing");
	XMLCh *optionCumulativeArea = XMLString::transcode("cumulativearea");
	XMLCh *optionCreateImage = XMLString::transcode("createimage");
	XMLCh *optionStretchImage = XMLString::transcode("stretch");
	XMLCh *optionHueColour = XMLString::transcode("huecolour");
	XMLCh *optionRemoveSpatialRef = XMLString::transcode("removespatialref");
	XMLCh *optionAddnoise = XMLString::transcode("addnoise");
    XMLCh *optionSubset = XMLString::transcode("subset");
    XMLCh *optionSubset2Polys = XMLString::transcode("subset2polys");
	XMLCh *optionDefineSpatialRef = XMLString::transcode("definespatialref");
	XMLCh *optionPanSharpen = XMLString::transcode("pansharpen");
    XMLCh *optionColourImageBands = XMLString::transcode("colourimagebands");
    XMLCh *optionCreateSlices = XMLString::transcode("createslices");
	XMLCh *optionClump = XMLString::transcode("clump");
    XMLCh *optionComposite = XMLString::transcode("composite");
    XMLCh *optionStackStats = XMLString::transcode("stackStats");
    XMLCh *optionRelabel = XMLString::transcode("relabel");
    XMLCh *optionAssignProj = XMLString::transcode("assignproj");
    XMLCh *optionPopImgStats = XMLString::transcode("popimgstats");
    XMLCh *optionCreateCopy = XMLString::transcode("createcopy");
    XMLCh *optionCreateKMLFile = XMLString::transcode("createKMLFile");
    XMLCh *optionCreateTiles = XMLString::transcode("createtiles");
    XMLCh *optionBandColourUsage = XMLString::transcode("bandcolourusage");
    XMLCh *optionAssignSpatialInfo = XMLString::transcode("assignspatialinfo");
    XMLCh *optionGenAssessPoints = XMLString::transcode("genassesspoints");
    XMLCh *optionUniquePxlClumps = XMLString::transcode("uniquepxlclumps");
    XMLCh *optionSubset2Image = XMLString::transcode("subset2img");
    XMLCh *optionDefineImgTiles = XMLString::transcode("defineimgtiles");
    XMLCh *optionGenTileMasks = XMLString::transcode("gentilemasks");
    XMLCh *optionCutOutTile = XMLString::transcode("cutouttile");
    XMLCh *optionStretchImageWithStats = XMLString::transcode("stretchwithstats");
    XMLCh *optionSubSampleImage = XMLString::transcode("subsampleimage");
    XMLCh *optionDarkTargetMask = XMLString::transcode("darktargetmask");
    XMLCh *optionCopyProjDef = XMLString::transcode("copyprojdef");
    XMLCh *optionCopyProjDefSpatialInfo = XMLString::transcode("copyprojdefspatialinfo");
    XMLCh *optionImageRasterZone2HDF = XMLString::transcode("imagerasterzone2hdf");
    XMLCh *optionBandSelect = XMLString::transcode("bandselect");

	const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
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

    // Get extension for out image format (only required for multiple output images)
    XMLCh *formatExtXMLStr = XMLString::transcode("extension");
	if(argElement->hasAttribute(formatExtXMLStr))
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(formatExtXMLStr));
		this->outFileExtension = string(charValue);
		XMLString::release(&charValue);
	}
    else
    {
        this->outFileExtension = "env";
        if(this->imageFormat == "ENVI"){this->outFileExtension = "env";}
        else if(this->imageFormat == "KEA"){this->outFileExtension = "kea";}
        else if(this->imageFormat == "GTiff"){this->outFileExtension = "tif";}
        else if(this->imageFormat == "HFA"){this->outFileExtension = "img";}
        else if(this->imageFormat == "PNG"){this->outFileExtension = "png";}
        else if(this->imageFormat == "AAIGrid"){this->outFileExtension = "asc";}
        else if(this->imageFormat == "PCIDSK"){this->outFileExtension = "pix";}
        else{std::cout << "Extension not known for file format, using \".env\"" << std::endl;}
    }
	XMLString::release(&formatExtXMLStr);


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

        XMLString::release(&dtByte);
        XMLString::release(&dtUInt16);
        XMLString::release(&dtInt16);
        XMLString::release(&dtUInt32);
        XMLString::release(&dtInt32);
        XMLString::release(&dtFloat32);
        XMLString::release(&dtFloat64);
	}
	XMLString::release(&datatypeXMLStr);

	const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
	if(XMLString::equals(optionColour, optionXML))
	{
		this->option = RSGISExeImageUtils::colour;

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

		DOMNodeList *classNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:colour"));
		this->numClasses = classNodesList->getLength();

		cout << "Found " << this->numClasses << " Classes \n";

		DOMElement *classElement = NULL;
		classColour = new ClassColour*[numClasses];
		for(int i = 0; i < numClasses; i++)
		{
			classColour[i] = new ClassColour();
			classElement = static_cast<DOMElement*>(classNodesList->item(i));

			XMLCh *nameXMLStr = XMLString::transcode("name");
			if(classElement->hasAttribute(nameXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(nameXMLStr));
				classColour[i]->className = string(charValue);
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
				classColour[i]->classID = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'id\' attribute was provided.");
			}
			XMLString::release(&idXMLStr);

			XMLCh *bandXMLStr = XMLString::transcode("band");
			if(classElement->hasAttribute(bandXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(bandXMLStr));
				classColour[i]->imgBand = mathUtils.strtoint(string(charValue))-1; // Band refers to the array index not image band
 				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			XMLString::release(&bandXMLStr);

			XMLCh *lowerXMLStr = XMLString::transcode("lower");
			if(classElement->hasAttribute(lowerXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(lowerXMLStr));
				classColour[i]->lower = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'lower\' attribute was provided.");
			}
			XMLString::release(&lowerXMLStr);

			XMLCh *upperXMLStr = XMLString::transcode("upper");
			if(classElement->hasAttribute(upperXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(upperXMLStr));
				classColour[i]->upper = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'upper\' attribute was provided.");
			}
			XMLString::release(&upperXMLStr);

			XMLCh *redXMLStr = XMLString::transcode("red");
			if(classElement->hasAttribute(redXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(redXMLStr));
				classColour[i]->red = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'red\' attribute was provided.");
			}
			XMLString::release(&redXMLStr);

			XMLCh *greenXMLStr = XMLString::transcode("green");
			if(classElement->hasAttribute(greenXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(greenXMLStr));
				classColour[i]->green = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'green\' attribute was provided.");
			}
			XMLString::release(&greenXMLStr);

			XMLCh *blueXMLStr = XMLString::transcode("blue");
			if(classElement->hasAttribute(blueXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(blueXMLStr));
				classColour[i]->blue = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'blue\' attribute was provided.");
			}
			XMLString::release(&blueXMLStr);
		}
	}
    else if(XMLString::equals(optionColourImageBands, optionXML))
	{
		this->option = RSGISExeImageUtils::colourimagebands;

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

		DOMNodeList *classNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:colour"));
		this->numClasses = classNodesList->getLength();

		cout << "Found " << this->numClasses << " Classes \n";

		DOMElement *classElement = NULL;
		classColour = new ClassColour*[numClasses];
		for(int i = 0; i < numClasses; i++)
		{
			classColour[i] = new ClassColour();
			classElement = static_cast<DOMElement*>(classNodesList->item(i));

			XMLCh *nameXMLStr = XMLString::transcode("name");
			if(classElement->hasAttribute(nameXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(nameXMLStr));
				classColour[i]->className = string(charValue);
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
				classColour[i]->classID = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'id\' attribute was provided.");
			}
			XMLString::release(&idXMLStr);

			XMLCh *lowerXMLStr = XMLString::transcode("lower");
			if(classElement->hasAttribute(lowerXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(lowerXMLStr));
				classColour[i]->lower = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'lower\' attribute was provided.");
			}
			XMLString::release(&lowerXMLStr);

			XMLCh *upperXMLStr = XMLString::transcode("upper");
			if(classElement->hasAttribute(upperXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(upperXMLStr));
				classColour[i]->upper = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'upper\' attribute was provided.");
			}
			XMLString::release(&upperXMLStr);

			XMLCh *redXMLStr = XMLString::transcode("red");
			if(classElement->hasAttribute(redXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(redXMLStr));
				classColour[i]->red = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'red\' attribute was provided.");
			}
			XMLString::release(&redXMLStr);

			XMLCh *greenXMLStr = XMLString::transcode("green");
			if(classElement->hasAttribute(greenXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(greenXMLStr));
				classColour[i]->green = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'green\' attribute was provided.");
			}
			XMLString::release(&greenXMLStr);

			XMLCh *blueXMLStr = XMLString::transcode("blue");
			if(classElement->hasAttribute(blueXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(blueXMLStr));
				classColour[i]->blue = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'blue\' attribute was provided.");
			}
			XMLString::release(&blueXMLStr);
		}
	}
	else if (XMLString::equals(optionMosaic, optionXML))
	{
		this->option = RSGISExeImageUtils::mosaic;
		this->mosaicSkipVals = false;
		this->mosaicSkipThresh = false;
        this->overlapBehaviour = 0;
		this->skipLowerThresh = -numeric_limits<double>::infinity();
		this->skipUpperThresh = +numeric_limits<double>::infinity();
		this->skipBand = 0;

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

		XMLCh *nodataXMLStr = XMLString::transcode("nodata");
		if(argElement->hasAttribute(nodataXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *noDataValue = argElement->getAttribute(nodataXMLStr);
			if(XMLString::equals(noDataValue, NaNStr))
			{
                const char *val = "NaN";
				this->nodataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(nodataXMLStr));
                this->nodataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);
		}
		else
		{
			//throw RSGISXMLArgumentsException("No \'nodata\' attribute was provided.");
			cout << "\tUsing default of 0 for background values" << endl;
			this->nodataValue = 0;
		}
		XMLString::release(&nodataXMLStr);

		// Set value in first band to skip, if not set no error will be printed and standard mosaic method will be used.
		XMLCh *skipValueXMLStr = XMLString::transcode("skipValue");
		if(argElement->hasAttribute(skipValueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(skipValueXMLStr));
			this->skipValue = mathUtils.strtofloat(string(charValue));
			this->mosaicSkipVals = true;
			XMLString::release(&charValue);
		}
		XMLString::release(&skipValueXMLStr);

		// Set upper threashold to skip in all bands, if not set no error will be printed and standard mosaic method will be used.
		XMLCh *skipUpperThreshXMLStr = XMLString::transcode("skipUpperThresh");
		if(argElement->hasAttribute(skipUpperThreshXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(skipUpperThreshXMLStr));
			this->skipUpperThresh = mathUtils.strtofloat(string(charValue));
			this->mosaicSkipThresh = true;
			XMLString::release(&charValue);
			if (mosaicSkipVals)
			{
				cout << "\tCan't use \'skipValue\' with \'skipUpperThresh\', using threshold instead" << endl;
			}
		}
		XMLString::release(&skipUpperThreshXMLStr);

		// Set lower threashold to skip in all bands, if not set no error will be printed and standard mosaic method will be used.
		XMLCh *skipLowerThreshXMLStr = XMLString::transcode("skipLowerThresh");
		if(argElement->hasAttribute(skipLowerThreshXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(skipLowerThreshXMLStr));
			this->skipLowerThresh = mathUtils.strtofloat(string(charValue));
			this->mosaicSkipThresh = true;
			XMLString::release(&charValue);
			if (mosaicSkipVals)
			{
				cout << "\tCan't use \'skipValue\' with \'skipLowerThresh\', using threshold instead" << endl;
			}
		}
		XMLString::release(&skipLowerThreshXMLStr);

		XMLCh *skipBandXMLStr = XMLString::transcode("setSkipBand");
		if(argElement->hasAttribute(skipBandXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(skipBandXMLStr));
			this->skipBand = mathUtils.strtofloat(string(charValue)) - 1;
			XMLString::release(&charValue);
			if (this->mosaicSkipVals)
			{
				cout << "\tSkiping pixels with a value of \'" << this->skipValue << "\' in band \'" << this->skipBand + 1 << "\'" << endl;
			}
			else if (this->mosaicSkipThresh)
			{
				cout << "\tSkiping pixels with a value between \'" << this->skipLowerThresh << "\' and \'" << this->skipUpperThresh << "\' in band \'" << this->skipBand + 1<< "\'" << endl;
			}
			else
			{
				cout << "\tBand set to define values to skip using \'setSkipBand\' but no value or thresholds set - IGNORING" << endl;
			}

		}
		else
		{
			if (this->mosaicSkipVals)
			{
				cout << "\tSkiping pixels with a value of \'" << this->skipValue << "\' using the first band (default)" << endl;
			}
			else if (this->mosaicSkipThresh)
			{
				cout << "\tSkiping pixels with a value between \'" << this->skipLowerThresh << "\' and \'" << this->skipUpperThresh << "\' using the first band (default)" << endl;			}
		}
		XMLString::release(&skipBandXMLStr);

        XMLCh *overlapBehaviourXML = XMLString::transcode("overlapBehaviour");
        if(argElement->hasAttribute(overlapBehaviourXML))
        {
            if(!this->mosaicSkipVals && !this->mosaicSkipThresh)
            {
                throw RSGISXMLArgumentsException("Can't define overlap behaviour for simple mosaic, define \'skipValue\' or \'skipLowerThresh\' and \'skipLowerThresh\'");
            }

            const XMLCh *overlapBehaviourStr = argElement->getAttribute(overlapBehaviourXML);

            XMLCh *minXML = XMLString::transcode("min");
            XMLCh *maxXML = XMLString::transcode("max");

            if(XMLString::equals(overlapBehaviourStr, minXML))
            {
                std::cout << "\tTaking the minimum value for overlaping pixels" << std::endl;
                this->overlapBehaviour=1;
            }
            else if(XMLString::equals(overlapBehaviourStr, maxXML))
            {
                std::cout << "\tTaking the maximum value for overlaping pixels" << std::endl;
                this->overlapBehaviour=2;
            }
            else
            {
                throw RSGISXMLArgumentsException("Overlap behaviour not recognised, options are 'min' or 'max'");
            }

            XMLString::release(&minXML);
            XMLString::release(&maxXML);
        }


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
            this->projFromImage = true;
			this->proj = "";
		}
		XMLString::release(&projXMLStr);


		XMLCh *dirXMLStr = XMLString::transcode("dir");
		XMLCh *extXMLStr = XMLString::transcode("ext");
		if(argElement->hasAttribute(dirXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(dirXMLStr));
			string dirStr = string(charValue);
			XMLString::release(&charValue);

			charValue = XMLString::transcode(argElement->getAttribute(extXMLStr));
			string extStr = string(charValue);
			XMLString::release(&charValue);

			try
			{
				this->inputImages = fileUtils.getDIRList(dirStr, extStr, &this->numImages, false);
			}
			catch(RSGISException e)
			{
				throw RSGISXMLArgumentsException(e.what());
			}
		}
		else
		{
			DOMElement *imageElement = NULL;

			DOMNodeList *imagesList = argElement->getElementsByTagName(rsgisimageXMLStr);
			this->numImages = imagesList->getLength();
			this->inputImages = new string[numImages];

			XMLCh *fileXMLStr = XMLString::transcode("file");
			for(int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<DOMElement*>(imagesList->item(i));

				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					this->inputImages[i] = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'file\' attribute was provided.");
				}
			}
			XMLString::release(&fileXMLStr);
		}
		XMLString::release(&dirXMLStr);
		XMLString::release(&extXMLStr);
	}
	else if (XMLString::equals(optionInclude, optionXML))
	{
		this->option = RSGISExeImageUtils::include;

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

        XMLCh *bandsXMLStr = XMLString::transcode("bands");
		if(argElement->hasAttribute(bandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bandsXMLStr));
			string bandsList = string(charValue);
			XMLString::release(&charValue);

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
                    cout << "Warning: " << *iterTokens << " is not an integer!\n";
                }
            }
            bandsDefined = true;
		}
		else
		{
			bandsDefined = false;
		}
		XMLString::release(&bandsXMLStr);


		XMLCh *dirXMLStr = XMLString::transcode("dir");
		XMLCh *extXMLStr = XMLString::transcode("ext");
		if(argElement->hasAttribute(dirXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(dirXMLStr));
			string dirStr = string(charValue);
			XMLString::release(&charValue);

			charValue = XMLString::transcode(argElement->getAttribute(extXMLStr));
			string extStr = string(charValue);
			XMLString::release(&charValue);

			try
			{
				this->inputImages = fileUtils.getDIRList(dirStr, extStr, &this->numImages, false);
			}
			catch(RSGISException e)
			{
				throw RSGISXMLArgumentsException(e.what());
			}
		}
		else
		{
			DOMElement *imageElement = NULL;

			DOMNodeList *imagesList = argElement->getElementsByTagName(rsgisimageXMLStr);
			this->numImages = imagesList->getLength();
			this->inputImages = new string[numImages];

			XMLCh *fileXMLStr = XMLString::transcode("file");
			for(int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<DOMElement*>(imagesList->item(i));

				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					this->inputImages[i] = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'file\' attribute was provided.");
				}
			}
			XMLString::release(&fileXMLStr);
		}
		XMLString::release(&dirXMLStr);
		XMLString::release(&extXMLStr);
	}
	else if ((XMLString::equals(optionCut2Poly, optionXML)) | (XMLString::equals(optionCut2Polys, optionXML)))
	{
		/* Changed to cut2polys for concistency with subset to polys.
           Also works with cut2poly to enable compatibility with old scripts - Dan */
        this->option = RSGISExeImageUtils::cut2poly;

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


		XMLCh *outfilenameXMLStr = XMLString::transcode("outfilename");
		if(argElement->hasAttribute(outfilenameXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outfilenameXMLStr));
			this->filenameAttribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'outfilename\' attribute was provided.");
		}
		XMLString::release(&outfilenameXMLStr);

		XMLCh *nodataXMLStr = XMLString::transcode("nodata");
		if(argElement->hasAttribute(nodataXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *noDataValue = argElement->getAttribute(nodataXMLStr);
			if(XMLString::equals(noDataValue, NaNStr))
			{
                const char *val = "NaN";
				this->nodataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(nodataXMLStr));
                this->nodataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);

		}
		else
		{
			throw RSGISXMLArgumentsException("No \'nodata\' attribute was provided.");
		}
		XMLString::release(&nodataXMLStr);
	}
	else if (XMLString::equals(optionMask, optionXML))
	{
		this->option = RSGISExeImageUtils::mask;

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


		XMLCh *maskXMLStr = XMLString::transcode("mask");
		if(argElement->hasAttribute(maskXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maskXMLStr));
			this->imageMask = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'mask\' attribute was provided.");
		}
		XMLString::release(&maskXMLStr);


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

        XMLCh *outputValueXMLStr = XMLString::transcode("outputvalue");
		if(argElement->hasAttribute(outputValueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputValueXMLStr));
			this->outValue = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'outputvalue\' attribute was provided.");
		}
		XMLString::release(&outputValueXMLStr);

		XMLCh *maskValueXMLStr = XMLString::transcode("maskvalue");
		if(argElement->hasAttribute(maskValueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maskValueXMLStr));
			this->maskValue = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'maskvalue\' attribute was provided.");
		}
		XMLString::release(&maskValueXMLStr);

	}
	else if (XMLString::equals(optionResample, optionXML))
	{
		this->option = RSGISExeImageUtils::resample;

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

		XMLCh *scaleXMLStr = XMLString::transcode("scale");
		if(argElement->hasAttribute(scaleXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(scaleXMLStr));
			this->resampleScale = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'scale\' attribute was provided.");
		}
		XMLString::release(&scaleXMLStr);

		XMLCh *interpolatorCubic = XMLString::transcode("Cubic");
		XMLCh *interpolatorBilinearArea = XMLString::transcode("BilinearArea");
		XMLCh *interpolatorBilinearPoint = XMLString::transcode("BilinearPoint");
		XMLCh *interpolatorNN = XMLString::transcode("NN");
		XMLCh *interpolatorTriangular = XMLString::transcode("Triangular");

		XMLCh *interpolatorXMLStr = XMLString::transcode("interpolation");
		if(argElement->hasAttribute(interpolatorXMLStr))
		{
			const XMLCh *interpolatorXMLValue = argElement->getAttribute(interpolatorXMLStr);
			if(XMLString::equals(interpolatorCubic, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::cubic;
			}
			else if (XMLString::equals(interpolatorBilinearArea, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::billinearArea;
			}
			else if (XMLString::equals(interpolatorBilinearPoint, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::billinearPt;
			}
			else if (XMLString::equals(interpolatorNN, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::nn;
			}
			else if (XMLString::equals(interpolatorTriangular, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::trangular;
			}
			else
			{
				throw RSGISXMLArgumentsException("Interpolator was not recognized.");
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'interpolation\' attribute was provided.");
		}
		XMLString::release(&interpolatorXMLStr);

		XMLString::release(&interpolatorCubic);
		XMLString::release(&interpolatorBilinearArea);
		XMLString::release(&interpolatorBilinearPoint);
		XMLString::release(&interpolatorNN);
		XMLString::release(&interpolatorTriangular);
	}
	else if (XMLString::equals(optionRasteriseDef, optionXML))
	{
		this->option = RSGISExeImageUtils::rasterisedefiniens;

        this->definiensTiles = false;

		XMLCh *inDIRXMLStr = XMLString::transcode("inDIR");
        XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(inDIRXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inDIRXMLStr));
			this->inputDIR = string(charValue);
			XMLString::release(&charValue);


            XMLCh *outDIRXMLStr = XMLString::transcode("outDIR");
            if(argElement->hasAttribute(outDIRXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(outDIRXMLStr));
                this->outputDIR = string(charValue);
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'outDIR\' attribute was provided.");
            }
            XMLString::release(&outDIRXMLStr);

            this->definiensTiles = true;
		}
		else if(argElement->hasAttribute(imageXMLStr))
		{

            char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);

            XMLCh *inCSVXMLStr = XMLString::transcode("csv");
            if(argElement->hasAttribute(inCSVXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(inCSVXMLStr));
                this->inputCSV = string(charValue);
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'csv\' attribute was provided.");
            }
            XMLString::release(&inCSVXMLStr);


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

            this->definiensTiles = false;
		}
        else
		{
			throw RSGISXMLArgumentsException("No \'image\' or \'inDIR\' attribute was provided one or other is required.");
		}
		XMLString::release(&inDIRXMLStr);
		XMLString::release(&imageXMLStr);

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
				cout << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
            this->projFromImage = true;
			this->proj = "";
		}
		XMLString::release(&projXMLStr);
	}
	else if (XMLString::equals(optionPrintProj4, optionXML))
	{
		this->option = RSGISExeImageUtils::printProj4;

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
	}
	else if (XMLString::equals(optionPrintWKT, optionXML))
	{
		this->option = RSGISExeImageUtils::printWKT;

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
	}
	else if (XMLString::equals(optionExtract2DScatterPtxt, optionXML))
	{
		this->option = RSGISExeImageUtils::extract2dscatterptxt;

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

		XMLCh *band1XMLStr = XMLString::transcode("band1");
		if(argElement->hasAttribute(band1XMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(band1XMLStr));
			this->imgBand1 = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'band1\' attribute was provided.");
		}
		XMLString::release(&band1XMLStr);

		XMLCh *band2XMLStr = XMLString::transcode("band2");
		if(argElement->hasAttribute(band2XMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(band2XMLStr));
			this->imgBand2 = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'band2\' attribute was provided.");
		}
		XMLString::release(&band2XMLStr);

		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->numImages = 1;
			this->inputImages = new string[numImages];
			this->inputImages[0] = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			DOMElement *imageElement = NULL;

			DOMNodeList *imagesList = argElement->getElementsByTagName(rsgisimageXMLStr);
			this->numImages = imagesList->getLength();
			this->inputImages = new string[numImages];

			XMLCh *fileXMLStr = XMLString::transcode("file");
			for(int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<DOMElement*>(imagesList->item(i));

				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					this->inputImages[i] = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'file\' attribute was provided.");
				}
			}
			XMLString::release(&fileXMLStr);
		}
		XMLString::release(&imageXMLStr);
	}
	else if (XMLString::equals(optionSGSmoothing, optionXML))
	{
		this->option = RSGISExeImageUtils::sgsmoothing;

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

		XMLCh *orderXMLStr = XMLString::transcode("order");
		if(argElement->hasAttribute(orderXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(orderXMLStr));
			this->order = mathUtils.strtoint(string(charValue))+1; // Order starts at zero therefore +1
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'order\' attribute was provided.");
		}
		XMLString::release(&orderXMLStr);

		XMLCh *windowXMLStr = XMLString::transcode("window");
		if(argElement->hasAttribute(windowXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(windowXMLStr));
			this->window = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'window\' attribute was provided.");
		}
		XMLString::release(&windowXMLStr);

		XMLCh *imagebandsXMLStr = XMLString::transcode("imagebands");
		if(argElement->hasAttribute(imagebandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imagebandsXMLStr));
			this->bandFloatValuesVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'imagebands\' attribute was provided.");
		}
		XMLString::release(&imagebandsXMLStr);
	}
	else if (XMLString::equals(optionCumulativeArea, optionXML))
	{
		this->option = RSGISExeImageUtils::cumulativearea;

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

		XMLCh *imagebandsXMLStr = XMLString::transcode("imagebands");
		if(argElement->hasAttribute(imagebandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imagebandsXMLStr));
			this->inMatrixfile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'imagebands\' attribute was provided.");
		}
		XMLString::release(&imagebandsXMLStr);
	}
	else if (XMLString::equals(optionCreateImage, optionXML))
	{
		this->option = RSGISExeImageUtils::createimage;

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

		XMLCh *numBandsXMLStr = XMLString::transcode("numbands");
		if(argElement->hasAttribute(numBandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numBandsXMLStr));
			this->numBands = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'numbands\' attribute was provided.");
		}
		XMLString::release(&numBandsXMLStr);

		XMLCh *widthXMLStr = XMLString::transcode("width");
		if(argElement->hasAttribute(widthXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(widthXMLStr));
			this->width = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'width\' attribute was provided.");
		}
		XMLString::release(&widthXMLStr);

		XMLCh *heightXMLStr = XMLString::transcode("height");
		if(argElement->hasAttribute(heightXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(heightXMLStr));
			this->height = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'height\' attribute was provided.");
		}
		XMLString::release(&heightXMLStr);


		XMLCh *eastingsXMLStr = XMLString::transcode("eastings");
		if(argElement->hasAttribute(eastingsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(eastingsXMLStr));
			this->eastings = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'eastings\' attribute was provided.");
		}
		XMLString::release(&eastingsXMLStr);


		XMLCh *northingsXMLStr = XMLString::transcode("northings");
		if(argElement->hasAttribute(northingsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(northingsXMLStr));
			this->northings = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'northings\' attribute was provided.");
		}
		XMLString::release(&northingsXMLStr);

		XMLCh *valueXMLStr = XMLString::transcode("value");
		if(argElement->hasAttribute(valueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(valueXMLStr));
			this->outValue = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'value\' attribute was provided.");
		}
		XMLString::release(&valueXMLStr);


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


		XMLCh *wktFileXMLStr = XMLString::transcode("wktfile");
		if(argElement->hasAttribute(wktFileXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(wktFileXMLStr));
			this->projFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'wktfile\' attribute was provided.");
		}
		XMLString::release(&wktFileXMLStr);


	}
	else if (XMLString::equals(optionStretchImage, optionXML))
	{
		this->option = RSGISExeImageUtils::stretch;

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

        this->outStatsFile = false;
        XMLCh *outputStatsXMLStr = XMLString::transcode("stats");
		if(argElement->hasAttribute(outputStatsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputStatsXMLStr));
			this->outputFile = string(charValue);
            this->outStatsFile = true;
			XMLString::release(&charValue);
		}
		else
		{
			this->outStatsFile = false;
		}
		XMLString::release(&outputStatsXMLStr);


		XMLCh *stretchLinearMinMax = XMLString::transcode("LinearMinMax");
		XMLCh *stretchLinearPercent = XMLString::transcode("LinearPercent");
		XMLCh *stretchLinearStdDev = XMLString::transcode("LinearStdDev");
		XMLCh *stretchHistogram = XMLString::transcode("Histogram");
		XMLCh *stretchExponential = XMLString::transcode("Exponential");
		XMLCh *stretchLogarithmic = XMLString::transcode("Logarithmic");
		XMLCh *stretchPowerLaw = XMLString::transcode("PowerLaw");

		XMLCh *stretchXMLStr = XMLString::transcode("stretch");
		if(argElement->hasAttribute(stretchXMLStr))
		{
			const XMLCh *stretchXMLValue = argElement->getAttribute(stretchXMLStr);
			if(XMLString::equals(stretchLinearMinMax, stretchXMLValue))
			{
				this->stretchType = rsgis::cmds::linearMinMax;
			}
			else if (XMLString::equals(stretchLinearPercent, stretchXMLValue))
			{
				this->stretchType = rsgis::cmds::linearPercent;

				XMLCh *percentXMLStr = XMLString::transcode("percent");
				if(argElement->hasAttribute(percentXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(percentXMLStr));
					this->percent = mathUtils.strtofloat(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'percent\' attribute was provided.");
				}
				XMLString::release(&percentXMLStr);
			}
			else if (XMLString::equals(stretchLinearStdDev, stretchXMLValue))
			{
				this->stretchType = rsgis::cmds::linearStdDev;

				XMLCh *stdDevXMLStr = XMLString::transcode("stddev");
				if(argElement->hasAttribute(stdDevXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(stdDevXMLStr));
					this->stddev = mathUtils.strtofloat(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'stddev\' attribute was provided.");
				}
				XMLString::release(&stdDevXMLStr);
			}
			else if (XMLString::equals(stretchHistogram, stretchXMLValue))
			{
				this->stretchType = rsgis::cmds::histogram;
			}
			else if (XMLString::equals(stretchExponential, stretchXMLValue))
			{
				this->stretchType = rsgis::cmds::exponential;
			}
			else if (XMLString::equals(stretchLogarithmic, stretchXMLValue))
			{
				this->stretchType = rsgis::cmds::logarithmic;
			}
			else if (XMLString::equals(stretchPowerLaw, stretchXMLValue))
			{
				this->stretchType = rsgis::cmds::powerLaw;
				XMLCh *powerXMLStr = XMLString::transcode("power");
				if(argElement->hasAttribute(powerXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(powerXMLStr));
					this->power = mathUtils.strtofloat(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'power\' attribute was provided.");
				}
				XMLString::release(&powerXMLStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("Stretch was not recognized.");
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'stretch\' attribute was provided.");
		}
		XMLString::release(&stretchXMLStr);

		XMLString::release(&stretchLinearMinMax);
		XMLString::release(&stretchLinearPercent);
		XMLString::release(&stretchLinearStdDev);
		XMLString::release(&stretchHistogram);
		XMLString::release(&stretchExponential);
		XMLString::release(&stretchLogarithmic);
		XMLString::release(&stretchPowerLaw);

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
        XMLCh *onePassSDXMLStr = XMLString::transcode("onePassSD");
		if(argElement->hasAttribute(onePassSDXMLStr))
		{
            XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *onePassValue = argElement->getAttribute(onePassSDXMLStr);
			if(XMLString::equals(onePassValue, yesStr))
			{
                this->onePassSD = true;
			}
			else
			{
				this->onePassSD = false;
			}
			XMLString::release(&yesStr);
		}
		XMLString::release(&onePassSDXMLStr);

	}
	else if (XMLString::equals(optionHueColour, optionXML))
	{
		this->option = RSGISExeImageUtils::huecolour;

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

		XMLCh *bandXMLStr = XMLString::transcode("band");
		if(argElement->hasAttribute(bandXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bandXMLStr));
			this->imgBand1 = mathUtils.strtoint(string(charValue))-1; // -1 so interface starts at 1 not 0.
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
		}
		XMLString::release(&bandXMLStr);


		XMLCh *lowValueXMLStr = XMLString::transcode("lowvalue");
		if(argElement->hasAttribute(lowValueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(lowValueXMLStr));
			this->lowerRangeValue = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'lowvalue\' attribute was provided.");
		}
		XMLString::release(&lowValueXMLStr);

		XMLCh *highValueXMLStr = XMLString::transcode("highvalue");
		if(argElement->hasAttribute(highValueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(highValueXMLStr));
			this->upperRangeValue = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'highvalue\' attribute was provided.");
		}
		XMLString::release(&highValueXMLStr);

		XMLCh *backgroundXMLStr = XMLString::transcode("background");
		if(argElement->hasAttribute(backgroundXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *noDataValue = argElement->getAttribute(backgroundXMLStr);
			if(XMLString::equals(noDataValue, NaNStr))
			{
                const char *val = "NaN";
				this->nodataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(backgroundXMLStr));
                this->nodataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);

		}
		else
		{
			throw RSGISXMLArgumentsException("No \'background\' attribute was provided.");
		}
		XMLString::release(&backgroundXMLStr);


	}
	else if (XMLString::equals(optionRemoveSpatialRef, optionXML))
	{
		this->option = RSGISExeImageUtils::removespatialref;

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
	}
	else if (XMLString::equals(optionAddnoise, optionXML))
	{
		this->option = RSGISExeImageUtils::addnoise;

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

		XMLCh *scaleXMLStr = XMLString::transcode("scale");
		if(argElement->hasAttribute(scaleXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(scaleXMLStr));
			this->scale = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'scale\' attribute was provided.");
		}
		XMLString::release(&scaleXMLStr);

		XMLCh *typeXMLStr = XMLString::transcode("type");
		if(argElement->hasAttribute(typeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(typeXMLStr));
			string typeStr = string(charValue);
			if (typeStr == "percentGaussianNoise")
			{
				this->noise = percentGaussianNoise;
			}
			else
			{
				this->noise = randomNoise;
			}

			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'scale\' attribute was provided.");
		}
		XMLString::release(&typeXMLStr);


	}
	else if (XMLString::equals(optionDefineSpatialRef, optionXML))
	{
		this->option = RSGISExeImageUtils::definespatialref;

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


		XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(projXMLStr));
			this->proj = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
            this->projFromImage = true;
			this->proj = "";
		}
		XMLString::release(&projXMLStr);

		XMLCh *tlxXMLStr = XMLString::transcode("tlx");
		if(argElement->hasAttribute(tlxXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(tlxXMLStr));
			this->eastings = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'tlx\' attribute was provided.");
		}
		XMLString::release(&tlxXMLStr);

		XMLCh *tlyXMLStr = XMLString::transcode("tly");
		if(argElement->hasAttribute(tlyXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(tlyXMLStr));
			this->northings = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'tly\' attribute was provided.");
		}
		XMLString::release(&tlyXMLStr);

		XMLCh *resxXMLStr = XMLString::transcode("resx");
		if(argElement->hasAttribute(resxXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resxXMLStr));
			this->xRes = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'resx\' attribute was provided.");
		}
		XMLString::release(&resxXMLStr);

		XMLCh *resyXMLStr = XMLString::transcode("resy");
		if(argElement->hasAttribute(resyXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resyXMLStr));
			this->yRes = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'resy\' attribute was provided.");
		}
		XMLString::release(&resyXMLStr);

	}
    else if (XMLString::equals(optionSubset, optionXML))
	{
		this->option = RSGISExeImageUtils::subset;

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
	}
    else if (XMLString::equals(optionSubset2Polys, optionXML))
	{
		this->option = RSGISExeImageUtils::subset2polys;

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


		XMLCh *outfilenameXMLStr = XMLString::transcode("outfilename");
		if(argElement->hasAttribute(outfilenameXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outfilenameXMLStr));
			this->filenameAttribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'outfilename\' attribute was provided.");
		}
		XMLString::release(&outfilenameXMLStr);
    }
	else if (XMLString::equals(optionPanSharpen, optionXML))
	{
		this->option = RSGISExeImageUtils::pansharpen;

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

		XMLCh *winSizeXMLStr = XMLString::transcode("winSize");
		if(argElement->hasAttribute(winSizeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(winSizeXMLStr));
			this->panWinSize = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			this->panWinSize = 7;
		}
		XMLString::release(&winSizeXMLStr);


	}
    else if (XMLString::equals(optionCreateSlices, optionXML))
	{
		this->option = RSGISExeImageUtils::createslices;

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
	}
    else if(XMLString::equals(optionClump, optionXML))
    {
        this->option = RSGISExeImageUtils::clump;

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
            this->projFromImage = true;
			this->proj = "";
		}
		XMLString::release(&projXMLStr);

    }
    else if ( (XMLString::equals(optionComposite, optionXML)) | (XMLString::equals(optionStackStats, optionXML)) )
	{
		this->option = RSGISExeImageUtils::imageComposite;

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

        this->allBands = true;
		XMLCh *cBandsXMLStr = XMLString::transcode("compositeBands");
        XMLCh *nBandsXMLStr = XMLString::transcode("numBands");
		if(argElement->hasAttribute(cBandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(cBandsXMLStr));
			this->compositeBands = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
            this->allBands = false;
		}
        
		if(argElement->hasAttribute(nBandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(nBandsXMLStr));
			this->compositeBands = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
            this->allBands = false;
		}
		else
		{
            this->allBands = true;
            std::cout << "Calculating statistics over all bands" << std::endl;
		}
		XMLString::release(&cBandsXMLStr);
        XMLString::release(&nBandsXMLStr);

		XMLCh *typeXMLStr = XMLString::transcode("stats");
        this->outCompStat = compositeMean; // Set to default (mean)
		if(argElement->hasAttribute(typeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(typeXMLStr));
			string typeStr = string(charValue);
			if (typeStr == "mean")
			{
				this->outCompStat = "mean";
			}
			else if (typeStr == "min")
			{
				this->outCompStat = "min";
			}
            else if (typeStr == "max")
			{
				this->outCompStat = "max";
			}
            else if (typeStr == "range")
			{
				this->outCompStat = "range";
			}
            else
            {
                throw RSGISXMLArgumentsException("Statistics not recognised / available. Options are mean, min, max and range.");
            }

			XMLString::release(&charValue);
		}
		else
		{
			cout << "No \'stats\' attribute was provided, assuming default of mean" << endl;
		}
		XMLString::release(&typeXMLStr);


	}
    else if (XMLString::equals(optionRelabel, optionXML))
	{
		this->option = RSGISExeImageUtils::relabel;

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

        XMLCh *lutXMLStr = XMLString::transcode("lut");
		if(argElement->hasAttribute(lutXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(lutXMLStr));
			this->lutMatrixFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'lut\' attribute was provided.");
		}
		XMLString::release(&lutXMLStr);
	}
    else if (XMLString::equals(optionAssignProj, optionXML))
    {
        this->option = RSGISExeImageUtils::assignproj;

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


        XMLCh *projWKTXMLStr = XMLString::transcode("projwkt");
		if(argElement->hasAttribute(projWKTXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(projWKTXMLStr));
			this->projFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'projwkt\' attribute was provided.");
		}
		XMLString::release(&projWKTXMLStr);
    }
    else if (XMLString::equals(optionPopImgStats, optionXML))
    {
        this->option = RSGISExeImageUtils::popimgstats;

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


        XMLCh *nodataXMLStr = XMLString::transcode("ignore");
		if(argElement->hasAttribute(nodataXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *noDataValue = argElement->getAttribute(nodataXMLStr);
			if(XMLString::equals(noDataValue, NaNStr))
			{
                const char *val = "NaN";
				this->nodataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(nodataXMLStr));
                this->nodataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);

            this->useIgnoreVal = true;
		}
		else
		{
			this->useIgnoreVal = false;
		}
		XMLString::release(&nodataXMLStr);

        XMLCh *pyramidsXMLStr = XMLString::transcode("pyramids");
		if(argElement->hasAttribute(pyramidsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(pyramidsXMLStr));
			string typeStr = string(charValue);
			if(typeStr == "yes")
			{
				this->calcImgPyramids = true;
			}
            else
            {
                this->calcImgPyramids = false;
            }

			XMLString::release(&charValue);
		}
		else
		{
			this->calcImgPyramids = true;
		}
		XMLString::release(&pyramidsXMLStr);

    }
    else if (XMLString::equals(optionCreateCopy, optionXML))
	{
		this->option = RSGISExeImageUtils::createcopy;

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


        XMLCh *pixelValXMLStr = XMLString::transcode("pixelval");
		if(argElement->hasAttribute(pixelValXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *dataValue = argElement->getAttribute(pixelValXMLStr);
			if(XMLString::equals(dataValue, NaNStr))
			{
                const char *val = "NaN";
				this->dataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(pixelValXMLStr));
                this->dataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'pixelval\' attribute was provided.");
		}
		XMLString::release(&pixelValXMLStr);

        XMLCh *numBandsXMLStr = XMLString::transcode("numbands");
		if(argElement->hasAttribute(numBandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numBandsXMLStr));
			this->numBands = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'numbands\' attribute was provided.");
		}
		XMLString::release(&numBandsXMLStr);

	}
    else if (XMLString::equals(optionCreateKMLFile, optionXML))
	{
		this->option = RSGISExeImageUtils::createKMLFile;

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

        XMLCh *outKMLFileXMLStr = XMLString::transcode("outKMLFile");
		if(argElement->hasAttribute(outKMLFileXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outKMLFileXMLStr));
			this->outKMLFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'outKMLFile\' attribute was provided.");
		}
		XMLString::release(&outKMLFileXMLStr);
	}
    else if (XMLString::equals(optionCreateTiles, optionXML))
	{
		this->option = RSGISExeImageUtils::createtiles;

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

        XMLCh *outTilesListXMLStr = XMLString::transcode("outTilesList");
		if(argElement->hasAttribute(outTilesListXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outTilesListXMLStr));
			this->outTilesList = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			this->outTilesList = "";
		}
		XMLString::release(&outTilesListXMLStr);

        XMLCh *widthXMLStr = XMLString::transcode("width");
		if(argElement->hasAttribute(widthXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(widthXMLStr));
			this->width = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'width\' attribute was provided.");
		}
		XMLString::release(&widthXMLStr);

		XMLCh *heightXMLStr = XMLString::transcode("height");
		if(argElement->hasAttribute(heightXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(heightXMLStr));
			this->height = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'height\' attribute was provided.");
		}
		XMLString::release(&heightXMLStr);

        XMLCh *overlapXMLStr = XMLString::transcode("overlap");
		if(argElement->hasAttribute(overlapXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(overlapXMLStr));
			this->tileOverlap = mathUtils.strtoint(string(charValue));
            std::cout << " Using overlap of " << this->tileOverlap << " pixels" << std::endl;
            XMLString::release(&charValue);
		}
		else
		{
			this->tileOverlap = 0;
		}
		XMLString::release(&overlapXMLStr);


        XMLCh *offsetXMLStr = XMLString::transcode("offset");
		if(argElement->hasAttribute(offsetXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(offsetXMLStr));
			string typeStr = string(charValue);
			if(typeStr == "yes")
			{
				this->offsetTiling = true;
			}
            else
            {
                this->offsetTiling = false;
            }

			XMLString::release(&charValue);
		}
		else
		{
			this->offsetTiling = false;
		}
		XMLString::release(&offsetXMLStr);

    }
    else if (XMLString::equals(optionBandColourUsage, optionXML))
	{
		this->option = RSGISExeImageUtils::bandcolourusage;

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

        DOMElement *bandElement = NULL;
        XMLCh *rsgisBandXMLStr = XMLString::transcode("rsgis:band");
        XMLCh *bandXMLStr = XMLString::transcode("band");
        XMLCh *usageXMLStr = XMLString::transcode("usage");

        DOMNodeList *bandsList = argElement->getElementsByTagName(rsgisBandXMLStr);
        unsigned int numBandsSpecified = bandsList->getLength();
        this->bandClrUses.reserve(numBandsSpecified);


        unsigned int bandNo = 0;
        GDALColorInterp gdalClrInterp = GCI_GrayIndex;
        string clrInterp = "";
        for(int i = 0; i < numBandsSpecified; i++)
        {
            bandElement = static_cast<DOMElement*>(bandsList->item(i));

            if(bandElement->hasAttribute(bandXMLStr))
            {
                char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
                bandNo = mathUtils.strtounsignedint(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
            }

            if(bandElement->hasAttribute(usageXMLStr))
            {
                char *charValue = XMLString::transcode(bandElement->getAttribute(usageXMLStr));
                clrInterp = string(charValue);
                if(clrInterp == "palette")
                {
                    gdalClrInterp = GCI_PaletteIndex;
                }
                else if(clrInterp == "grayscale")
                {
                    gdalClrInterp = GCI_GrayIndex;
                }
                else if(clrInterp == "red")
                {
                    gdalClrInterp = GCI_RedBand;
                }
                else if(clrInterp == "green")
                {
                    gdalClrInterp = GCI_GreenBand;
                }
                else if(clrInterp == "blue")
                {
                    gdalClrInterp = GCI_BlueBand;
                }
                else if(clrInterp == "alpha")
                {
                    gdalClrInterp = GCI_AlphaBand;
                }
                else
                {
                    throw RSGISXMLArgumentsException("Usage was not recognised.");
                }
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'usage\' attribute was provided.");
            }

            bandClrUses.push_back(std::pair<unsigned int, GDALColorInterp>(bandNo, gdalClrInterp));
        }
        XMLString::release(&bandXMLStr);
        XMLString::release(&rsgisBandXMLStr);
        XMLString::release(&usageXMLStr);
	}
    else if (XMLString::equals(optionAssignSpatialInfo, optionXML))
	{
		this->option = RSGISExeImageUtils::assignspatialinfo;

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

        XMLCh *tlxXMLStr = XMLString::transcode("tlx");
		if(argElement->hasAttribute(tlxXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(tlxXMLStr));
			this->tlx = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
            this->tlxDef = true;
		}
		else
		{
			this->tlxDef = false;
		}
		XMLString::release(&tlxXMLStr);

        XMLCh *tlyXMLStr = XMLString::transcode("tly");
		if(argElement->hasAttribute(tlyXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(tlyXMLStr));
			this->tly = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
            this->tlyDef = true;
		}
		else
		{
			this->tlyDef = false;
		}
		XMLString::release(&tlyXMLStr);

        XMLCh *resXXMLStr = XMLString::transcode("resX");
		if(argElement->hasAttribute(resXXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resXXMLStr));
			this->resX = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
            this->resXDef = true;
		}
		else
		{
			this->resXDef = false;
		}
		XMLString::release(&resXXMLStr);

        XMLCh *resYXMLStr = XMLString::transcode("resY");
		if(argElement->hasAttribute(resYXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resYXMLStr));
			this->resY = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
            this->resYDef = true;
		}
		else
		{
			this->resYDef = false;
		}
		XMLString::release(&resYXMLStr);

        XMLCh *rotXXMLStr = XMLString::transcode("rotX");
		if(argElement->hasAttribute(rotXXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rotXXMLStr));
			this->rotX = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
            this->rotXDef = true;
		}
		else
		{
			this->rotXDef = false;
		}
		XMLString::release(&rotXXMLStr);

        XMLCh *rotYXMLStr = XMLString::transcode("rotY");
		if(argElement->hasAttribute(rotYXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rotYXMLStr));
			this->rotY = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
            this->rotYDef = true;
		}
		else
		{
			this->rotYDef = false;
		}
		XMLString::release(&rotYXMLStr);
	}
    else if (XMLString::equals(optionGenAssessPoints, optionXML))
	{
		this->option = RSGISExeImageUtils::genassesspoints;

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

        XMLCh *demXMLStr = XMLString::transcode("dem");
		if(argElement->hasAttribute(demXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(demXMLStr));
			this->inputDEM = string(charValue);
            this->demProvided = true;
			XMLString::release(&charValue);
		}
		else
		{
			this->demProvided = false;
		}
		XMLString::release(&demXMLStr);

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

        XMLCh *classColumnXMLStr = XMLString::transcode("classcolumn");
		if(argElement->hasAttribute(classColumnXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(classColumnXMLStr));
			this->classColumnName = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'classcolumn\' attribute was provided.");
		}
		XMLString::release(&classColumnXMLStr);


        XMLCh *typeXMLStr = XMLString::transcode("type");
		if(argElement->hasAttribute(typeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(typeXMLStr));
            string typeString = string(charValue);
			/*
            if(typeString == "random")
            {
                this->accuracyPtsType = rsgis_randompts;
            }
            else if(typeString == "stratified")
            {
                this->accuracyPtsType = rsgis_stratifiedpts;
            }
            else
            {
                throw RSGISXMLArgumentsException("The \'type\' attribute must be either \'random\' or \'stratified\'.");
            }
             */
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'type\' attribute was provided.");
		}
		XMLString::release(&typeXMLStr);

        XMLCh *nStr = XMLString::transcode("n");
		if(argElement->hasAttribute(nStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(nStr));
			this->numPoints = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'n\' attribute was provided.");
		}
		XMLString::release(&nStr);

        XMLCh *seedStr = XMLString::transcode("seed");
		if(argElement->hasAttribute(seedStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(seedStr));
			this->seed = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'seed\' attribute was provided.");
		}
		XMLString::release(&seedStr);
	}
    else if (XMLString::equals(optionUniquePxlClumps, optionXML))
	{
		this->option = RSGISExeImageUtils::uniquepxlclumps;

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

        noDataValDefined = false;
        XMLCh *noDataValXMLStr = XMLString::transcode("nodata");
		if(argElement->hasAttribute(noDataValXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *dataValue = argElement->getAttribute(noDataValXMLStr);
			if(XMLString::equals(dataValue, NaNStr))
			{
                const char *val = "NaN";
				this->dataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(noDataValXMLStr));
                this->nodataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);
            noDataValDefined = true;
		}
		else
		{
			noDataValDefined = false;
		}
		XMLString::release(&noDataValXMLStr);

	}
    else if (XMLString::equals(optionSubset2Image, optionXML))
	{
		this->option = RSGISExeImageUtils::subset2img;

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


		XMLCh *roiXMLStr = XMLString::transcode("roi");
		if(argElement->hasAttribute(roiXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(roiXMLStr));
			this->inputROIImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'roi\' attribute was provided.");
		}
		XMLString::release(&roiXMLStr);
	}
    else if (XMLString::equals(optionDefineImgTiles, optionXML))
	{
		this->option = RSGISExeImageUtils::defineimgtiles;

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

        noDataValDefined = false;
        XMLCh *noDataValXMLStr = XMLString::transcode("nodata");
		if(argElement->hasAttribute(noDataValXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *dataValue = argElement->getAttribute(noDataValXMLStr);
			if(XMLString::equals(dataValue, NaNStr))
			{
                const char *val = "NaN";
				this->dataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(noDataValXMLStr));
                this->nodataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);
            noDataValDefined = true;
		}
		else
		{
			noDataValDefined = false;
		}
		XMLString::release(&noDataValXMLStr);

        validPixelRatio = 0.0;
        XMLCh *ratioXMLStr = XMLString::transcode("validpixelratio");
		if(argElement->hasAttribute(ratioXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(ratioXMLStr));
            this->validPixelRatio = mathUtils.strtofloat(string(charValue));
            XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'validpixelratio\' attribute was provided.");
		}
		XMLString::release(&ratioXMLStr);

        XMLCh *tileSizeXMLStr = XMLString::transcode("tilesize");
		if(argElement->hasAttribute(tileSizeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(tileSizeXMLStr));
            this->tileSizePxl = mathUtils.strtounsignedint(string(charValue));
            XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'tilesize\' attribute was provided.");
		}
		XMLString::release(&tileSizeXMLStr);

	}
    else if (XMLString::equals(optionGenTileMasks, optionXML))
	{
		this->option = RSGISExeImageUtils::gentilemasks;

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


		XMLCh *outputXMLStr = XMLString::transcode("outputbase");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImageBase = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'outputbase\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

        createAnOverlap = false;
        XMLCh *overlapXMLStr = XMLString::transcode("overlap");
		if(argElement->hasAttribute(overlapXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(overlapXMLStr));
            this->overlap = mathUtils.strtofloat(string(charValue));
            createAnOverlap = true;
            XMLString::release(&charValue);
		}
		else
		{
            createAnOverlap = false;
		}
		XMLString::release(&overlapXMLStr);


        XMLCh *growOverlapXMLStr = XMLString::transcode("growoverlap");
		if(argElement->hasAttribute(growOverlapXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *strValue = argElement->getAttribute(growOverlapXMLStr);

			if(XMLString::equals(strValue, yesStr))
			{
				this->growOverlap = true;
			}
			else
			{
				this->growOverlap = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'growoverlap\' attribute was provided.");
		}
		XMLString::release(&growOverlapXMLStr);
	}
    else if (XMLString::equals(optionCutOutTile, optionXML))
	{
		this->option = RSGISExeImageUtils::cutouttile;

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

        XMLCh *tileXMLStr = XMLString::transcode("tile");
		if(argElement->hasAttribute(tileXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(tileXMLStr));
			this->tileImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'tile\' attribute was provided.");
		}
		XMLString::release(&tileXMLStr);


        XMLCh *noDataValXMLStr = XMLString::transcode("nodataval");
		if(argElement->hasAttribute(noDataValXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *dataValue = argElement->getAttribute(noDataValXMLStr);
			if(XMLString::equals(dataValue, NaNStr))
			{
                const char *val = "NaN";
				this->dataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(noDataValXMLStr));
                this->nodataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'nodataval\' attribute was provided.");
		}
		XMLString::release(&noDataValXMLStr);

	}
    else if (XMLString::equals(optionStretchImageWithStats, optionXML))
	{
		this->option = RSGISExeImageUtils::stretchwithstats;

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

        XMLCh *inputStatsXMLStr = XMLString::transcode("stats");
		if(argElement->hasAttribute(inputStatsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inputStatsXMLStr));
			this->inputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'stats\' attribute was provided.");
		}
		XMLString::release(&inputStatsXMLStr);


		XMLCh *stretchLinearMinMax = XMLString::transcode("LinearMinMax");
		XMLCh *stretchHistogram = XMLString::transcode("Histogram");
		XMLCh *stretchExponential = XMLString::transcode("Exponential");
		XMLCh *stretchLogarithmic = XMLString::transcode("Logarithmic");
		XMLCh *stretchPowerLaw = XMLString::transcode("PowerLaw");

		XMLCh *stretchXMLStr = XMLString::transcode("stretch");
		if(argElement->hasAttribute(stretchXMLStr))
		{
			const XMLCh *stretchXMLValue = argElement->getAttribute(stretchXMLStr);
			if(XMLString::equals(stretchLinearMinMax, stretchXMLValue))
			{
				this->stretchType = rsgis::cmds::linearMinMax;
			}
			else if (XMLString::equals(stretchHistogram, stretchXMLValue))
			{
				this->stretchType = rsgis::cmds::histogram;
			}
			else if (XMLString::equals(stretchExponential, stretchXMLValue))
			{
				this->stretchType = rsgis::cmds::exponential;
			}
			else if (XMLString::equals(stretchLogarithmic, stretchXMLValue))
			{
				this->stretchType = rsgis::cmds::logarithmic;
			}
			else if (XMLString::equals(stretchPowerLaw, stretchXMLValue))
			{
				this->stretchType = rsgis::cmds::powerLaw;
				XMLCh *powerXMLStr = XMLString::transcode("power");
				if(argElement->hasAttribute(powerXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(powerXMLStr));
					this->power = mathUtils.strtofloat(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'power\' attribute was provided.");
				}
				XMLString::release(&powerXMLStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("Stretch was not recognized.");
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'stretch\' attribute was provided.");
		}
		XMLString::release(&stretchXMLStr);

		XMLString::release(&stretchLinearMinMax);
		XMLString::release(&stretchHistogram);
		XMLString::release(&stretchExponential);
		XMLString::release(&stretchLogarithmic);
		XMLString::release(&stretchPowerLaw);



	}
    else if (XMLString::equals(optionSubSampleImage, optionXML))
	{
		this->option = RSGISExeImageUtils::subsampleimage;

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


        XMLCh *nodataXMLStr = XMLString::transcode("nodata");
		if(argElement->hasAttribute(nodataXMLStr))
		{
            char *charValue = XMLString::transcode(argElement->getAttribute(nodataXMLStr));
            this->nodataValue = mathUtils.strtofloat(string(charValue));
            XMLString::release(&charValue);
            this->useIgnoreVal = true;
		}
		else
		{
			this->useIgnoreVal = false;
		}
		XMLString::release(&nodataXMLStr);


        XMLCh *subSampleXMLStr = XMLString::transcode("subsample");
		if(argElement->hasAttribute(subSampleXMLStr))
		{
            char *charValue = XMLString::transcode(argElement->getAttribute(subSampleXMLStr));
            this->imageSample = mathUtils.strtounsignedint(string(charValue));
            XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'subsample\' attribute was provided.");
		}
		XMLString::release(&subSampleXMLStr);



	}
    else if (XMLString::equals(optionDarkTargetMask, optionXML))
	{
		this->option = RSGISExeImageUtils::darktargetmask;
        
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
        
        
        XMLCh *nodataXMLStr = XMLString::transcode("nodata");
		if(argElement->hasAttribute(nodataXMLStr))
		{
            char *charValue = XMLString::transcode(argElement->getAttribute(nodataXMLStr));
            this->nodataValue = mathUtils.strtofloat(string(charValue));
            XMLString::release(&charValue);
            this->useIgnoreVal = true;
		}
		else
		{
			this->useIgnoreVal = false;
		}
		XMLString::release(&nodataXMLStr);
        
	}
    else if (XMLString::equals(optionCopyProjDef, optionXML))
	{
		this->option = RSGISExeImageUtils::copyprojdef;
        
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
        
		XMLCh *refImageXMLStr = XMLString::transcode("refimage");
		if(argElement->hasAttribute(refImageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(refImageXMLStr));
			this->inputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'refimage\' attribute was provided.");
		}
		XMLString::release(&refImageXMLStr);
	}
    else if (XMLString::equals(optionCopyProjDefSpatialInfo, optionXML))
	{
		this->option = RSGISExeImageUtils::copyprojdefspatialinfo;
        
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
        
		XMLCh *refImageXMLStr = XMLString::transcode("refimage");
		if(argElement->hasAttribute(refImageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(refImageXMLStr));
			this->inputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'refimage\' attribute was provided.");
		}
		XMLString::release(&refImageXMLStr);
	}
    else if (XMLString::equals(optionImageRasterZone2HDF, optionXML))
	{
		this->option = RSGISExeImageUtils::imagerasterzone2hdf;
        
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
			this->outputHDFFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        
		XMLCh *maskXMLStr = XMLString::transcode("mask");
		if(argElement->hasAttribute(maskXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maskXMLStr));
			this->imageMask = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'mask\' attribute was provided.");
		}
		XMLString::release(&maskXMLStr);
        
        XMLCh *maskValueXMLStr = XMLString::transcode("maskvalue");
		if(argElement->hasAttribute(maskValueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maskValueXMLStr));
			this->maskValue = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'maskvalue\' attribute was provided.");
		}
		XMLString::release(&maskValueXMLStr);
	}
    else if (XMLString::equals(optionBandSelect, optionXML))
	{
		this->option = RSGISExeImageUtils::bandselect;
        
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
        
        DOMElement *bandElement = NULL;
        XMLCh *rsgisBandXMLStr = XMLString::transcode("rsgis:band");
        XMLCh *bandXMLStr = XMLString::transcode("band");
        
        DOMNodeList *bandsList = argElement->getElementsByTagName(rsgisBandXMLStr);
        unsigned int numBandsSpecified = bandsList->getLength();
        this->selectBands.reserve(numBandsSpecified);
        
        
        unsigned int bandNo = 0;
        for(int i = 0; i < numBandsSpecified; i++)
        {
            bandElement = static_cast<DOMElement*>(bandsList->item(i));
            
            if(bandElement->hasAttribute(bandXMLStr))
            {
                char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
                bandNo = mathUtils.strtounsignedint(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
            }
            
            selectBands.push_back(bandNo);
        }
        XMLString::release(&bandXMLStr);
        XMLString::release(&rsgisBandXMLStr);
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeImageUtils.");
		throw RSGISXMLArgumentsException(message.c_str());
	}

	XMLString::release(&algorName);
	XMLString::release(&algorXMLStr);
	XMLString::release(&optionXMLStr);
	XMLString::release(&optionColour);
	XMLString::release(&optionMosaic);
	XMLString::release(&optionInclude);
	XMLString::release(&optionCut2Poly);
    XMLString::release(&optionCut2Polys);
	XMLString::release(&optionMask);
	XMLString::release(&optionResample);
	XMLString::release(&optionRasteriseDef);
	XMLString::release(&projImage);
	XMLString::release(&projOSGB);
    XMLString::release(&projNZ2000);
    XMLString::release(&projNZ1949);
	XMLString::release(&rsgisimageXMLStr);
	XMLString::release(&optionPrintProj4);
	XMLString::release(&optionPrintWKT);
	XMLString::release(&optionExtract2DScatterPtxt);
	XMLString::release(&optionSGSmoothing);
	XMLString::release(&optionCumulativeArea);
	XMLString::release(&optionCreateImage);
	XMLString::release(&optionStretchImage);
	XMLString::release(&optionHueColour);
	XMLString::release(&optionRemoveSpatialRef);
	XMLString::release(&optionAddnoise);
	XMLString::release(&optionDefineSpatialRef);
    XMLString::release(&optionSubset);
    XMLString::release(&optionSubset2Polys);
    XMLString::release(&optionPanSharpen);
    XMLString::release(&optionColourImageBands);
    XMLString::release(&optionCreateSlices);
    XMLString::release(&optionClump);
    XMLString::release(&optionComposite);
    XMLString::release(&optionRelabel);
    XMLString::release(&optionAssignProj);
    XMLString::release(&optionPopImgStats);
	XMLString::release(&optionCreateCopy);
    XMLString::release(&optionCreateKMLFile);
    XMLString::release(&optionCreateTiles);
    XMLString::release(&optionBandColourUsage);
    XMLString::release(&optionAssignSpatialInfo);
    XMLString::release(&optionGenAssessPoints);
    XMLString::release(&optionSubset2Image);
    XMLString::release(&optionDefineImgTiles);
    XMLString::release(&optionGenTileMasks);
    XMLString::release(&optionCutOutTile);
    XMLString::release(&optionStretchImageWithStats);
    XMLString::release(&optionSubSampleImage);
    XMLString::release(&optionDarkTargetMask);
    XMLString::release(&optionCopyProjDef);
    XMLString::release(&optionCopyProjDefSpatialInfo);
    XMLString::release(&optionImageRasterZone2HDF);
    XMLString::release(&optionBandSelect);

	parsed = true;
}

void RSGISExeImageUtils::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(option == RSGISExeImageUtils::colour)
		{
			cout << "Colouring image\n";
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISColourUpImage *colourImage = NULL;
			RSGISCalcImage *calcImage = NULL;

			int numBands = 0;
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
				numBands = datasets[0]->GetRasterCount();

				for(int i = 0; i < this->numClasses; i++)
				{
					classColour[i]->numInputBands = numBands;

					cout << i <<  ") Class " << classColour[i]->className << " with ID ";
					cout << classColour[i]->classID << " uses image band " << classColour[i]->imgBand + 1;
					cout << " where a total of " << classColour[i]->numInputBands << " is available:\n";
					cout << "Lower = " << classColour[i]->lower << endl;
					cout << "Upper = " << classColour[i]->upper << endl;
					cout << "Red = " << classColour[i]->red << endl;
					cout << "Green = " << classColour[i]->green << endl;
					cout << "Blue = " << classColour[i]->blue << endl;
				}

				colourImage = new RSGISColourUpImage(3, this->classColour, this->numClasses);
				calcImage = new RSGISCalcImage(colourImage, "", true);

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
			catch (RSGISException e)
			{
				cout << "Exception occured: " << e.what() << endl;
			}

		}
        else if(option == RSGISExeImageUtils::colourimagebands)
		{
			cout << "Colouring image bands\n";
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISColourUpImageBand *colourImage = NULL;
			RSGISCalcImage *calcImage = NULL;

			int numBands = 0;
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
				numBands = datasets[0]->GetRasterCount();

				for(int i = 0; i < this->numClasses; i++)
				{
					classColour[i]->numInputBands = numBands;

					cout << i <<  ") Class " << classColour[i]->className << " with ID ";
					cout << classColour[i]->classID << endl;
					cout << "Lower = " << classColour[i]->lower << endl;
					cout << "Upper = " << classColour[i]->upper << endl;
					cout << "Red = " << classColour[i]->red << endl;
					cout << "Green = " << classColour[i]->green << endl;
					cout << "Blue = " << classColour[i]->blue << endl;
				}

				colourImage = new RSGISColourUpImageBand(3, this->classColour, this->numClasses);
				calcImage = new RSGISCalcImage(colourImage, "", true);

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
			catch (RSGISException e)
			{
				cout << "Exception occured: " << e.what() << endl;
			}

		}
		else if(option == RSGISExeImageUtils::mosaic)
		{
            if(this->numImages < 1)
            {
                throw RSGISException("No input images have been provided.");
            }
			cout << "Mosaicing Images\n";
            cout << "Number of Images = " << this->numImages << endl;
            // Don't print if over 100 images
            if(this->numImages < 100)
            {
                cout << "Input Images: \n";
                for(int i = 0; i < this->numImages; ++i)
                {
                    cout << i+1 << ": " << inputImages[i] << endl;
                }
            }

			RSGISImageMosaic mosaic;
			try
			{
				if (this->mosaicSkipVals)
				{
                    std::cout << "Skipping pixel values = " << this->skipValue << " in band " << this->skipBand + 1 << std::endl;
					mosaic.mosaicSkipVals(inputImages, this->numImages, this->outputImage, this->nodataValue, this->skipValue, this->projFromImage, this->proj, this->skipBand, this->overlapBehaviour, this->imageFormat, this->outDataType);
				}
				else if (this->mosaicSkipThresh)
				{
                    std::cout << "Skipping pixel values between " << this->skipLowerThresh << " and " << this->skipUpperThresh << " in band " << this->skipBand + 1 << std::endl;
					mosaic.mosaicSkipThresh(inputImages, this->numImages, this->outputImage, this->nodataValue, this->skipLowerThresh, this->skipUpperThresh, this->projFromImage, this->proj, this->skipBand, this->overlapBehaviour, this->imageFormat, this->outDataType);
				}
				else
				{
                    std::cout << "Simple mosaic (all pixels)" << std::endl;
					mosaic.mosaic(inputImages, this->numImages, this->outputImage, this->nodataValue, this->projFromImage, this->proj, this->imageFormat, this->outDataType);
				}

				delete[] inputImages;
			}
			catch(RSGISException e)
			{
				throw e;
			}

		}
		else if(option == RSGISExeImageUtils::include)
		{
            cout << "Include images into a larger image\n";
            if(bandsDefined)
            {
                for(vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                {
                    cout << "Band " << *iterBands << endl;
                }
            }
            cout << this->inputImage << endl;
			try
            {
                rsgis::cmds::executeImageInclude(this->inputImages, this->numImages, this->inputImage, this->bandsDefined, this->bands);
            }
            catch(rsgis::cmds::RSGISCmdException &e)
            {
                throw RSGISException(e.what());
            }
		}
		else if(option == RSGISExeImageUtils::cut2poly)
		{
			cout << "Cutting image to polygons\n";
			GDALAllRegister();
			OGRRegisterAll();

			GDALDataset **dataset = NULL;
			GDALDataset *inputVecDS = NULL;
			OGRLayer *inputVecLayer = NULL;

			RSGISCopyImage *copyImage = NULL;
			RSGISCalcImage *calcImage = NULL;

			RSGISVectorIO vecIO;
			RSGISPolygonData **polyData = NULL;
			RSGISImageTileVector **data = NULL;
			RSGISVectorUtils vecUtils;

			string vectorLayerName = vecUtils.getLayerName(this->inputVector);
			int numImageBands = 0;
			int numFeatures = 0;
			string outputFilePath;

			try
			{
				// Open Image
				dataset = new GDALDataset*[1];
				cout << this->inputImage << endl;
				dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				numImageBands = dataset[0]->GetRasterCount();
				cout << "Raster Band Count = " << numImageBands << endl;

				// Open vector
				inputVecDS = (GDALDataset*) GDALOpenEx(this->inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputVecDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputVecLayer = inputVecDS->GetLayerByName(vectorLayerName.c_str());
				if(inputVecLayer == NULL)
				{
					string message = string("Could not open vector layer ") + vectorLayerName;
					throw RSGISFileException(message.c_str());
				}

				// READ IN SHAPEFILE
				numFeatures = inputVecLayer->GetFeatureCount();
				polyData = new RSGISPolygonData*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					polyData[i] = new RSGISImageTileVector(this->filenameAttribute);
				}
				cout << "Reading in " << numFeatures << " features\n";
				vecIO.readPolygons(inputVecLayer, polyData, numFeatures);

				//Convert to RSGISImageTileVector
				data = new RSGISImageTileVector*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					data[i] = dynamic_cast<RSGISImageTileVector*>(polyData[i]);
				}
				delete[] polyData;

				copyImage = new RSGISCopyImage(numImageBands);
				calcImage = new RSGISCalcImage(copyImage, "", true);

                unsigned int failCount = 0;

				for(int i = 0; i < numFeatures; i++)
				{
					outputFilePath = this->outputImage + data[i]->getFileName() + "." + this->outFileExtension;
					cout << i << ": " << outputFilePath << endl;
                    try
                    {
                        calcImage->calcImageWithinPolygon(dataset, 1, outputFilePath, data[i]->getBBox(), data[i]->getPolygon(), this->nodataValue, polyContainsPixelCenter,this->imageFormat, this->outDataType);
                    }
                    catch (RSGISImageBandException e)
                    {
                        ++failCount;
                        if(failCount <= 100)
                        {
                            cerr << "RSGISException caught: " << e.what() << endl;
                            cerr << "Check output path exists and is writable and all polygons in shapefile:" << endl;
                            cerr << " " << this->inputVector << endl;
                            cerr << "Are completely within:" << endl;
                            cerr << " " << this->inputImage << endl;
                        }
                        else
                        {
                            cerr << "Over 100 exceptions have been caught, exiting" << endl;
                            throw e;
                        }
                    }
				}

				GDALClose(dataset[0]);
				delete[] dataset;
				GDALClose(inputVecDS);
				OGRCleanupAll();
				GDALDestroyDriverManager();
				delete calcImage;
				delete copyImage;
			}
			catch(RSGISException e)
			{
				cerr << "RSGISException caught: " << e.what() << endl;
			}
		}
		else if(option == RSGISExeImageUtils::mask)
		{
            std::cout << "A command to mask an input image\n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Mask Image: " << this->imageMask << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Image Format: " << this->imageFormat << std::endl;
            std::cout << "Output Data Type: " << this->outDataType << std::endl;
            std::cout << "Output value: " << this->outValue << std::endl;
            std::cout << "Mask value: " << this->maskValue << std::endl;

			try
            {
                std::vector<float> values;
                values.push_back(this->maskValue);
                
                rsgis::cmds::executeMaskImage(this->inputImage, this->imageMask, this->outputImage, this->imageFormat, this->rsgisOutDataType, this->outValue, values);
            }
            catch(rsgis::cmds::RSGISCmdException &e)
            {
                throw RSGISException(e.what());
            }
		}
		else if(option == RSGISExeImageUtils::resample)
		{
			GDALAllRegister();

			RSGISImageInterpolation *interpolation = NULL;
			RSGISInterpolator *interpolator = NULL;

			GDALDataset *inDataset = NULL;

			int xOutResolution = 0;
			int yOutResolution = 0;

			try
			{
				if(this->interpolator == RSGISExeImageUtils::cubic)
				{
					cout << "Using a cubic interpolator\n";
					interpolator = new RSGISCubicInterpolator();
				}
				else if(this->interpolator == RSGISExeImageUtils::billinearArea)
				{
					cout << "Using a bilinear (area) interpolator\n";
					interpolator = new RSGISBilinearAreaInterpolator();
				}
				else if(this->interpolator == RSGISExeImageUtils::billinearPt)
				{
					cout << "Using a bilinear (point) interpolator\n";
					interpolator = new RSGISBilinearPointInterpolator();
				}
				else if(this->interpolator == RSGISExeImageUtils::nn)
				{
					cout << "Using a nearest neighbour interpolator\n";
					interpolator = new RSGISNearestNeighbourInterpolator();
				}
				else if(this->interpolator == RSGISExeImageUtils::trangular)
				{
					cout << "Using a triangular interpolator\n";
					interpolator = new RSGISTriangulationInterpolator();
				}
				else
				{
					throw RSGISException("Interpolator Option Not Reconised");
				}

				interpolation = new RSGISImageInterpolation(interpolator);

				cout << this->inputImage << endl;
				inDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

				interpolation->findOutputResolution(inDataset, this->resampleScale, &xOutResolution, &yOutResolution);

				interpolation->interpolateNewImage(inDataset, xOutResolution, yOutResolution, this->outputImage);

				GDALClose(inDataset);

				delete interpolation;
				delete interpolator;
				GDALDestroyDriverManager();

			}
			catch(RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::rasterisedefiniens)
		{
			GDALAllRegister();
			RSGISFileUtils fileUtils;
			RSGISDefiniensCSVRasterise rasterisedefiniensCSV = RSGISDefiniensCSVRasterise(projFromImage, proj);
			cout << "Rasterise Definiens\n";

            if(this->definiensTiles)
            {
                cout << "Input DIR: " << this->inputDIR << endl;
                cout << "Output DIR: " << this->outputDIR << endl;
                string tif = string(".tif");
                string csv = string(".CSV");

                string *inputTIF = NULL;
                int numTIFs = 0;

                string *inputCSV = NULL;
                int numCSVs = 0;

                SortedGenericList<RSGISDefiniensWorkspaceFileName> *sortedTIFs = NULL;
                SortedGenericList<RSGISDefiniensWorkspaceFileName> *sortedCSVs = NULL;

                try
                {
                    inputTIF = fileUtils.getDIRList(this->inputDIR, tif, &numTIFs, false);
                    inputCSV = fileUtils.getDIRList(this->inputDIR, csv, &numCSVs, false);

                    cout << "numTIFs = " << numTIFs << endl;
                    cout << "numCSVs = " << numCSVs << endl;

                    if(numTIFs != numCSVs)
                    {
                        throw RSGISException("number of TIFs and CSV files found do not match.");
                    }

                    if(numTIFs == 0)
                    {
                        throw RSGISException("No input files were found.");
                    }

                    sortedTIFs = new SortedGenericList<RSGISDefiniensWorkspaceFileName>(numTIFs, numTIFs/2);
                    sortedCSVs = new SortedGenericList<RSGISDefiniensWorkspaceFileName>(numCSVs, numCSVs/2);

                    for(int i = 0; i < numTIFs; i++)
                    {
                        //cout << i << ")\t" << inputTIF[i] << "\t" << inputCSV[i] << endl;
                        sortedTIFs->add(new RSGISDefiniensWorkspaceFileName(inputTIF[i]));
                        sortedCSVs->add(new RSGISDefiniensWorkspaceFileName(inputCSV[i]));
                    }

                    sortedTIFs->printAsc();
                    sortedCSVs->printAsc();

                    //Check basename is the same:
                    string basename = sortedTIFs->peekTop()->getBaseName();
                    //cout << "BaseName = " << basename << endl;
                    int size = sortedCSVs->getSize();
                    for(int i = 0; i < size; i++)
                    {
                        if(sortedTIFs->getAt(i)->getBaseName() != basename)
                        {
                            throw RSGISException("Base filenames are not the same.");
                        }

                        if(sortedCSVs->getAt(i)->getBaseName() != basename)
                        {
                            throw RSGISException("Base filenames are not the same.");
                        }
                    }

                    cout << "Base filenames match\n";
                    int oldTIFs = 0;

                    for(int i = size-1; i > 0; i--)
                    {
                        if(sortedTIFs->getAt(i)->getTile() == sortedTIFs->getAt(i-1)->getTile())
                        {
                            if(sortedTIFs->getAt(i)->getVersion() == sortedTIFs->getAt(i-1)->getVersion())
                            {
                                throw RSGISException("The tile has the same number and version.");
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
                                throw RSGISException("The tile has the same number and version.");
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
                        throw RSGISException("A different number of old version tiles were identified in the TIF and CSV lists.");
                    }

                    cout << oldTIFs << " old versions of tiles have been identified and will be ignored.\n";
                    // sortedTIFs->printAsc();
                    // sortedCSVs->printAsc();

                    for(int i = size-1; i >= 0; i--)
                    {
                        if(sortedTIFs->getAt(i)->getTile() != sortedCSVs->getAt(i)->getTile())
                        {
                            throw RSGISException("TIF AND CSV lists are different. (tile number)");
                        }
                        else if(sortedTIFs->getAt(i)->getVersion() != sortedCSVs->getAt(i)->getVersion())
                        {
                            throw RSGISException("TIF AND CSV lists are different. (version number)");
                        }
                        else if(sortedTIFs->getAt(i)->getOldVersion() != sortedCSVs->getAt(i)->getOldVersion())
                        {
                            throw RSGISException("TIF AND CSV lists are different. (old version)");
                        }
                    }
                    cout << "Files have been checked and the corrsponding TIF and CSV files are present.\n";
                    cout << "Starting Rasterisation...\n";

                    rasterisedefiniensCSV.rasteriseTiles(sortedTIFs, sortedCSVs, outputDIR);


                    sortedTIFs->clearListDelete();
                    delete sortedTIFs;
                    sortedCSVs->clearListDelete();
                    delete sortedCSVs;

                    delete[] inputTIF;
                    delete[] inputCSV;
                }
                catch(RSGISException& e)
                {
                    throw e;
                }
            }
            else
            {
                cout << "Input Image: " << this->inputImage << endl;
                cout << "Input CSV: " << this->inputCSV << endl;
                cout << "Output Image: " << this->outputImage << endl;

                try
                {
                    rasterisedefiniensCSV.rasteriseFile(this->inputImage, this->inputCSV, this->outputImage);
                }
                catch(RSGISException& e)
                {
                    throw e;
                }
            }
            GDALDestroyDriverManager();
            cout << "Finished Rasterisation\n";
		}
		else if(option == RSGISExeImageUtils::printProj4)
		{
			cout << "Print Spatial Reference Proj4\n";
			cout << "Input Image: " << this->inputImage << endl;
			GDALAllRegister();
			GDALDataset *inDataset = NULL;

			try
			{
				inDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

				const char *wtkSpatialRef = inDataset->GetProjectionRef();
				OGRSpatialReference ogrSpatial = OGRSpatialReference(wtkSpatialRef);

				char **proj4spatialref = new char*[1];
				proj4spatialref[0] = new char[1000];
				ogrSpatial.exportToProj4(proj4spatialref);
				cout << proj4spatialref[0] << endl;

				GDALClose(inDataset);
				GDALDestroyDriverManager();

			}
			catch(RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::printWKT)
		{
			cout << "Print Spatial Reference WKT\n";
			cout << "Input Image: " << this->inputImage << endl;

			GDALAllRegister();
			GDALDataset *inDataset = NULL;

			try
			{
				inDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

				const char *wtkSpatialRef = inDataset->GetProjectionRef();

				cout << wtkSpatialRef << endl;

				GDALClose(inDataset);
				GDALDestroyDriverManager();

			}
			catch(RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::extract2dscatterptxt)
		{
			cout << "Extract image data to create 2D scatter plot (exported as ptxt)\n";

			cout << "Input Images:\n";
			for(int i = 0; i < this->numImages; ++i)
			{
				cout << i << ") " << this->inputImages[i] << endl;
			}
			cout << "Image Band 1: " << this->imgBand1 << endl;
			cout << "Image Band 2: " << this->imgBand2 << endl;
			cout << "Output File: " << this->outputFile << endl;

			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISExport2DScatterPTxt *export2DScatter = NULL;
			RSGISCalcImage *calcImage = NULL;

			try
			{
				datasets = new GDALDataset*[numImages];

				for(int i = 0; i < numImages; ++i)
				{
					datasets[i] = (GDALDataset *) GDALOpenShared(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
				}

				RSGISExportForPlottingIncremental *plotter = new RSGISExportForPlottingIncremental();
				plotter->openFile(this->outputFile, scatter2d);
				export2DScatter = new RSGISExport2DScatterPTxt(plotter, imgBand1, imgBand2);
				calcImage = new RSGISCalcImage(export2DScatter, "", true);

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
			catch (RSGISException e)
			{
				cout << "Exception occured: " << e.what() << endl;
			}



		}
		else if(option == RSGISExeImageUtils::sgsmoothing)
		{
			cout << "Apply Savitzky-Golay Smoothing Filters to data\n";

			cout << "Input Image = " << this->inputImage << endl;
			cout << "Output Image = " << this->outputImage << endl;
			cout << "Image Band Values = " << this->bandFloatValuesVector << endl;
			cout << "Order = " << this->order << endl;
			cout << "Window = " << this->window << endl;

			GDALAllRegister();

			RSGISVectors vectorUtils;

			GDALDataset **datasets = NULL;

			RSGISCalcImage *calcImage = NULL;
			RSGISCalcImageValue *calcImageValue = NULL;

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
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

				Vector *bandValues = vectorUtils.readVectorFromTxt(this->bandFloatValuesVector);

				cout << "Input Image band values:\n";
				vectorUtils.printVector(bandValues);

				int numInBands = datasets[0]->GetRasterCount();

				calcImageValue = new RSGISSavitzkyGolaySmoothingFilters(numInBands, this->order, this->window, bandValues);
				calcImage = new RSGISCalcImage(calcImageValue, "", true);

				calcImage->calcImage(datasets, 1, this->outputImage);

				delete calcImage;
				delete calcImageValue;

				vectorUtils.freeVector(bandValues);

				GDALClose(datasets[0]);
				delete[] datasets;
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			GDALDestroyDriverManager();
		}
		else if(option == RSGISExeImageUtils::cumulativearea)
		{
			cout << "Calculate the cumulative area of the image profile/spectra\n";
			cout << "Input Image = " << this->inputImage << endl;
			cout << "Output Image = " << this->outputImage << endl;
			cout << "Image Band Widths = " << this->bandFloatValuesVector << endl;

			GDALAllRegister();

			RSGISMatrices matixUtils;

			GDALDataset **datasets = NULL;

			RSGISCalcImage *calcImage = NULL;
			RSGISCalcImageValue *calcImageValue = NULL;

			try
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

				Matrix *bandValues = matixUtils.readMatrixFromTxt(this->inMatrixfile);

				int numInBands = datasets[0]->GetRasterCount();

				calcImageValue = new RSGISCumulativeArea(numInBands, bandValues);
				calcImage = new RSGISCalcImage(calcImageValue, "", true);

				calcImage->calcImage(datasets, 1, this->outputImage);

				delete calcImage;
				delete calcImageValue;

				matixUtils.freeMatrix(bandValues);

				GDALClose(datasets[0]);
				delete[] datasets;
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			GDALDestroyDriverManager();
		}
		else if(option == RSGISExeImageUtils::createimage)
		{
			cout << "Create a new blank image\n";
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Num. Image bands: " << this->numBands << endl;
			cout << "Size: [" << this->width << "," << this->height << "]\n";
			cout << "TL Geo: [" << this->eastings << "," << this->northings << "]\n";
			cout << "Resolution: " << this->resolution << endl;
			cout << "Default Value: " << this->outValue << endl;
			cout << "Projection: " << this->projFile << endl;

			//RSGISImageUtils imgUtils;
			try
			{
                rsgis::cmds::executeCreateBlankImage(this->outputImage, this->numBands, this->width, this->height, this->eastings, this->northings, this->resolution, this->outValue, this->projFile, "", imageFormat, rsgisOutDataType);
                
                /*
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
					projection = string(wktspatialref[0]);
					OGRFree(wktspatialref);
				}

				GDALDataset* outImage = imgUtils.createBlankImage(outputImage, transformation, width, height, numBands, projection, outValue);
				GDALClose(outImage);
				GDALDestroyDriverManager();
                */
			}
			catch(rsgis::cmds::RSGISCmdException &e)
            {
                throw RSGISException(e.what());
            }
		}
		else if(option == RSGISExeImageUtils::stretch)
		{
			cout << "Apply an enhancement stretch to an input image - usually for visualisation\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
            if(this->outStatsFile)
            {
                cout << "Output Stats File: " << this->outputFile << endl;
            }
			if(stretchType == rsgis::cmds::linearMinMax)
			{
				cout << "Linear Min-Max stretch\n";
			}
			else if(stretchType == rsgis::cmds::linearPercent)
			{
				cout << "Linear " << percent << " % stretch\n";
			}
			else if(stretchType == rsgis::cmds::linearStdDev)
			{
				cout << "Linear " << stddev << " Standard Deviation stretch\n";
			}
			else if(stretchType == rsgis::cmds::histogram)
			{
				cout << "Histogram stretch\n";
			}
			else if(stretchType == rsgis::cmds::exponential)
			{
				cout << "Exponential stretch\n";
			}
			else if(stretchType == rsgis::cmds::logarithmic)
			{
				cout << "Logarithmic stretch\n";
			}
			else if(stretchType == rsgis::cmds::powerLaw)
			{
				cout << power << " Power Law stretch\n";
			}
			else
			{
				throw RSGISException("Stretch is not recognised.");
			}
            if(this->ignoreZeros)
            {
                cout << "Ignoring Zeros\n";
            }
            if(this->onePassSD)
            {
                std::cout << "Calculating SD in one pass\n";
            }

            try
            {
                if(stretchType == rsgis::cmds::linearPercent)
                {
                    rsgis::cmds::executeStretchImage(this->inputImage, this->outputImage, this->outStatsFile, this->outputFile, this->ignoreZeros, this->onePassSD, this->imageFormat, rsgisOutDataType, stretchType, this->percent);
                }
                else if(stretchType == rsgis::cmds::linearStdDev)
                {
                    rsgis::cmds::executeStretchImage(this->inputImage, this->outputImage, this->outStatsFile, this->outputFile, this->ignoreZeros, this->onePassSD, this->imageFormat, rsgisOutDataType, stretchType, this->stddev);
                }
                else if(stretchType == rsgis::cmds::powerLaw)
                {
                    rsgis::cmds::executeStretchImage(this->inputImage, this->outputImage, this->outStatsFile, this->outputFile, this->ignoreZeros, this->onePassSD, this->imageFormat, rsgisOutDataType, stretchType, this->power);
                }
                else
                {
                    rsgis::cmds::executeStretchImage(this->inputImage, this->outputImage, this->outStatsFile, this->outputFile, this->ignoreZeros, this->onePassSD, this->imageFormat, rsgisOutDataType, stretchType, 0.0);
                }
            }
            catch(rsgis::cmds::RSGISCmdException &e)
            {
                throw RSGISException(e.what());
            }


		}
		else if(option == RSGISExeImageUtils::huecolour)
		{
			cout << "Generate a colour image (through Hue) representing an image band - usually for visualisation\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Band: " << this->imgBand1 << endl;
			cout << "Range: [" << this->lowerRangeValue << "," << this->upperRangeValue << "]\n";
			cout << "Background value: " << this->nodataValue << endl;

			GDALAllRegister();

			RSGISCalcImage *calcImage = NULL;
			RSGISCalcImageValue *calcImageValue = NULL;

			try
			{
				GDALDataset **datasets = NULL;

				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

				if(this->imgBand1 >= datasets[0]->GetRasterCount())
				{
					throw RSGISImageException("Insufficient number of bands in the input image for select band.");
				}

				calcImageValue = new RSGISColourUsingHue(3, this->imgBand1, this->lowerRangeValue, this->upperRangeValue, this->nodataValue);
				calcImage = new RSGISCalcImage(calcImageValue, "", true);

				calcImage->calcImage(datasets, 1, this->outputImage);

				delete calcImage;
				delete calcImageValue;

				GDALClose(datasets[0]);
				delete[] datasets;
				GDALDestroyDriverManager();
			}
			catch(RSGISException& e)
			{
				throw e;
			}

		}
		else if(option == RSGISExeImageUtils::removespatialref)
		{
			cout << "Remove / define spatial reference to nothing and size set to pixel size\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;

			try
			{
				RSGISImageUtils imgUtils;
				imgUtils.copyImageRemoveSpatialReference(inputImage, outputImage);
			}
			catch(RSGISException& e)
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

			if (this->noise == percentGaussianNoise )
			{
				cout << "Adding " << this->scale * 100 << "% Gaussian Noise" << endl;
				RSGISAddRandomGaussianNoisePercent *addNoise = NULL;
				try
				{
					addNoise = new RSGISAddRandomGaussianNoisePercent(datasets[0]->GetRasterCount(),this->scale);
					calcImg = new RSGISCalcImage(addNoise, "", true);
					calcImg->calcImage(datasets, 1, this->outputImage);
					delete addNoise;
				}
				catch(RSGISException& e)
				{
					throw e;
				}
			}

			else
			{
				cout << "Adding random noise" << endl;
				RSGISAddRandomNoise *addNoise = NULL;
				try
				{
					addNoise = new RSGISAddRandomNoise(datasets[0]->GetRasterCount(),this->scale);
					calcImg = new RSGISCalcImage(addNoise, "", true);
					calcImg->calcImage(datasets, 1, this->outputImage);
					delete addNoise;
				}
				catch(RSGISException& e)
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
			cout << "Make a copy of the input image and define the projection and spatial locations\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Projection: " << this->proj << endl;
			cout << "TL [" << this->eastings << "," << this->northings << "]\n";
			cout << "Resolution [" << this->xRes << "," << this->yRes << "]\n";

			try
			{
				RSGISImageUtils imgUtils;
				imgUtils.copyImageDefiningSpatialReference(inputImage, outputImage, this->proj, this->eastings, this->northings, this->xRes, this->yRes);
			}
			catch(RSGISException& e)
			{
				throw e;
			}
		}
        else if(option == RSGISExeImageUtils::subset)
		{
			cout << "Subset image to vector\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Image: " << this->outputImage << endl;
            
            try
            {
                rsgis::cmds::executeSubset(this->inputImage, this->inputVector, this->outputImage, this->imageFormat, this->rsgisOutDataType);
            }
            catch(rsgis::cmds::RSGISCmdException &e)
            {
                throw RSGISException(e.what());
            }
		}
        else if(option == RSGISExeImageUtils::subset2polys)
		{
            std::cout << "Subset image to bounding box of polygons\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Image base: " << this->outputImage << endl;
        
            try
            {
                rsgis::cmds::executeSubset2Polys(this->inputImage, this->inputVector, this->filenameAttribute, this->outputImage, this->imageFormat, this->rsgisOutDataType, this->outFileExtension);
            }
			catch(RSGISException e)
			{
				cerr << "RSGISException caught: " << e.what() << endl;
			}
		}
		else if(option == RSGISExeImageUtils::pansharpen)
		{
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcImage *calcImage = NULL;

			datasets = new GDALDataset*[1];

			datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(datasets[0] == NULL)
			{
				string message = string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}

			int numRasterBands = datasets[0]->GetRasterCount();

			// Calculate statistics
			float *imageStats = new float[4];  // Set up an array to hold image stats

			cout << "Calculating image mean.." << endl;
			RSGISHCSPanSharpenCalcMeanStats *panMean = new RSGISHCSPanSharpenCalcMeanStats(numRasterBands, imageStats);
			RSGISCalcImage *calcImageMean = new RSGISCalcImage(panMean, "", true);
			calcImageMean->calcImage(datasets, 1);
			panMean->returnStats();

			cout << "Calculating image standard deviation.." << endl;
			RSGISHCSPanSharpenCalcSDStats *panSD = new RSGISHCSPanSharpenCalcSDStats(numRasterBands, imageStats);
			RSGISCalcImage *calcImageSD = new RSGISCalcImage(panSD, "", true);
			calcImageSD->calcImage(datasets, 1);
			panSD->returnStats();

			/*cout << "meanMS = " << imageStats[0] << endl;
			cout << "meanPAN = "<< imageStats[1] << endl;
			cout << "sdMS = " << imageStats[2] << endl;
			cout << "sdPAN = "<< imageStats[3] << endl;*/

			cout << "Pan sharpening.." << endl;
			RSGISHCSPanSharpen *panSharpen = new RSGISHCSPanSharpen(numRasterBands - 1, imageStats);
			calcImage = new RSGISCalcImage(panSharpen, "", true);
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
            cout << "Create Image slices from a multiband input image\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image Base: " << this->outputImage << endl;

			GDALAllRegister();
			GDALDataset *dataset = NULL;

			dataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(dataset == NULL)
			{
				string message = string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}

            RSGISImageUtils imageUtils;
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
            cout << "Clump the input image for a given values.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;

            GDALAllRegister();
			GDALDataset *inDataset = NULL;
			inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(inDataset == NULL)
			{
				string message = string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}

            /*
            unsigned long width = inDataset->GetRasterXSize();
            unsigned long height = inDataset->GetRasterYSize();
            unsigned int numBands = inDataset->GetRasterCount();
            */
            RSGISImageUtils imgUtils;

            GDALDataset *processingDataset = NULL;

            cout << "Copying input dataset\n";
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                processingDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                processingDataset = imgUtils.createCopy(inDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            imgUtils.copyUIntGDALDataset(inDataset, processingDataset);


            cout << "Performing clumping\n";
            RSGISCalcImgValueAlongsideOut *clumpImg = new RSGISClumpImage();
            RSGISCalcImgAlongsideOut calcImg = RSGISCalcImgAlongsideOut(clumpImg);
            calcImg.calcImageIterate(processingDataset);
            delete clumpImg;

            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
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
            if(this->allBands){cout << "A command to calculate summary stats for each pixel over all bands in a stack\n";}
            else{cout << "A command to calculate summary stats for each pixel over every " << this->compositeBands << "bands in a stack\n";}
            
            cout << "Input Image: " << this->inputImage << std::endl;
            cout << "Output Image: " << this->outputImage << std::endl;
            
            try
            {
                rsgis::cmds::executeStackStats(this->inputImage, this->outputImage, this->outCompStat, this->allBands, this->compositeBands, this->imageFormat, this->rsgisOutDataType);
            }
            catch(rsgis::cmds::RSGISCmdException& e)
            {
                throw RSGISException(e.what());
            }
            catch(std::exception& e)
            {
                throw RSGISException(e.what());
            }
		}
        else if(option == RSGISExeImageUtils::createtiles)
		{
			cout << "A command to create tiles from image\n";
            cout << "Input Image: " << this->inputImage << std::endl;
            cout << "Output Image Base: " << this->outputImage << std::endl;
            if(this->outTilesList != "")
            {
                std::cout << "Saving list of tiles to: " << this->outTilesList << std::endl;
            }

            try
            {
                if(this->outTilesList != "")
                {
                    std::vector<std::string> *outFileNames = new std::vector<std::string>;
                    rsgis::cmds::executeCreateTiles(this->inputImage, this->outputImage, this->width, this->height, this->tileOverlap, this->offsetTiling, this->imageFormat, this->rsgisOutDataType, this->outFileExtension, outFileNames);
                    std::ofstream tilesListFile;
                    tilesListFile.open(this->outTilesList.c_str());
                    for( std::vector<std::string>::iterator itr = outFileNames->begin(); itr != outFileNames->end(); itr++)
                    {
                        tilesListFile << *itr << "\n";
                    }
                    tilesListFile.close();
                }
                else
                {
                    rsgis::cmds::executeCreateTiles(this->inputImage, this->outputImage, this->width, this->height, this->tileOverlap, this->offsetTiling, this->imageFormat, this->rsgisOutDataType, this->outFileExtension);
                }
            }
            catch(rsgis::cmds::RSGISCmdException& e)
            {
                throw RSGISException(e.what());
            }
            catch(std::exception& e)
            {
                throw RSGISException(e.what());
            }


		}
        else if(option == RSGISExeImageUtils::relabel)
        {
            cout << "A command to relabel image pixel using a look up table from a gmtxt matrix file.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Look Up Table: " << this->lutMatrixFile << endl;

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

                RSGISRelabelPixelValuesFromLUT relabelPixels;
                relabelPixels.relabelPixelValues(inDataset, this->outputImage, this->lutMatrixFile, this->imageFormat);

                GDALClose(inDataset);
                GDALDestroyDriverManager();
            }
            catch (RSGISException &e)
            {
                throw e;
            }

        }
        else if(option == RSGISExeImageUtils::assignproj)
        {
            cout << "Assign and update and image to a specific projection\n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Projection File: " << this->projFile << endl;

            try
            {
                rsgis::cmds::executeAssignProj(this->inputImage, "", true, this->projFile);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
        }
        else if(option == RSGISExeImageUtils::popimgstats)
        {
            cout << "Populate an image with image statistics and image pyramids\n";
            cout << "Image: " << this->inputImage << endl;
            if(this->useIgnoreVal)
            {
                cout << "Ignore Val: " << this->nodataValue << endl;
            }
            if(this->calcImgPyramids)
            {
                cout << "Calculating image pyramids\n";
            }

            try
            {
                rsgis::cmds::executePopulateImgStats(this->inputImage, this->useIgnoreVal, this->nodataValue, this->calcImgPyramids);
            }
            catch(rsgis::cmds::RSGISCmdException& e)
            {
                throw RSGISException(e.what());
            }
            catch(std::exception& e)
            {
                throw RSGISException(e.what());
            }
        }
        else if(option == RSGISExeImageUtils::createcopy)
        {
            cout << "Create a new image from an existing image\n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Output: " << this->outputImage << endl;
            cout << "Data Value: " << this->dataValue << endl;
            cout << "Num Image Bands: " << this->numBands << endl;
            cout << "Image format: " << this->imageFormat << endl;

            try
            {
                rsgis::cmds::executeCreateCopyBlankImage(inputImage, outputImage, numBands, dataValue, imageFormat, rsgisOutDataType);
                /*GDALAllRegister();
                GDALDataset *inDataset = NULL;
                inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                if(inDataset == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }

                RSGISImageUtils imgUtils;
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, this->numBands, this->outputImage, this->imageFormat, outDataType);
                imgUtils.assignValGDALDataset(outDataset, this->dataValue);

                GDALClose(inDataset);
                GDALClose(outDataset);
                GDALDestroyDriverManager();*/
            }
            catch(rsgis::cmds::RSGISCmdException& e)
            {
                throw RSGISException(e.what());
            }
            catch(std::exception& e)
            {
                throw RSGISException(e.what());
            }
        }
        else if(option == RSGISExeImageUtils::createKMLFile)
		{
			RSGISImageUtils imgUtils;

            cout << "Create KML Text file for:" << this->inputImage << endl;
            cout << "Saving to: " <<  this->outKMLFile << endl;
            cout << "NOTE: This command assumed relavent pre-processing has already" << endl;
            cout << "been carried out for the image and will only work for a stretched " << endl;
            cout << "three band image, readable by GoogleEarth" << endl;
            imgUtils.createKMLText(this->inputImage, this->outKMLFile);
		}
        else if(option == RSGISExeImageUtils::bandcolourusage)
        {
            cout << "Defining the colour usage parameter on the image bands\n";
            cout << "Image: " << this->inputImage << std::endl;

            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }

            unsigned int numBandsInDS = inDataset->GetRasterCount();

            for(std::vector<std::pair<unsigned int, GDALColorInterp> >::iterator iterUsages = bandClrUses.begin(); iterUsages != bandClrUses.end(); ++iterUsages)
            {
                cout << "Band " << (*iterUsages).first;
                if((*iterUsages).second == GCI_GrayIndex)
                {
                    cout << " Greyscale usage\n";
                }
                else if((*iterUsages).second == GCI_PaletteIndex)
                {
                    cout << " Paletted usage\n";
                }
                else if((*iterUsages).second == GCI_RedBand)
                {
                    cout << " Red usage\n";
                }
                else if((*iterUsages).second == GCI_GreenBand)
                {
                    cout << " Green usage\n";
                }
                else if((*iterUsages).second == GCI_BlueBand)
                {
                    cout << " Blue usage\n";
                }
                else if((*iterUsages).second == GCI_AlphaBand)
                {
                    cout << " Alpha usage\n";
                }
                else
                {
                    throw RSGISImageException("Band usage is unknown...");
                }

                if((*iterUsages).first == 0)
                {
                    throw RSGISImageException("Band not within the image - indexing starts at 1.");
                }
                else if((*iterUsages).first > numBandsInDS)
                {
                    throw RSGISImageException("Band not within the image - too many bands referenced.");
                }

                inDataset->GetRasterBand((*iterUsages).first)->SetColorInterpretation((*iterUsages).second);
            }

            GDALClose(inDataset);
            GDALDestroyDriverManager();
        }
        else if(option == RSGISExeImageUtils::assignspatialinfo)
        {
            cout.precision(12);
            cout << "Assign and update image spatial header info\n";
            cout << "Image: " << this->inputImage << endl;
            if(this->tlxDef)
            {
                cout << "TLX: " << this->tlx << std::endl;
            }
            if(this->tlyDef)
            {
                cout << "TLY: " << this->tly << std::endl;
            }
            if(this->resXDef)
            {
                cout << "Res X: " << this->resX << std::endl;
            }
            if(this->resYDef)
            {
                cout << "Res Y: " << this->resY << std::endl;
            }
            if(this->rotXDef)
            {
                cout << "Rot X: " << this->rotX << std::endl;
            }
            if(this->rotYDef)
            {
                cout << "Rot Y: " << this->rotY << std::endl;
            }

            try
            {
                rsgis::cmds::executeAssignSpatialInfo(this->inputImage, this->tlx, this->tly, this->resX, this->resY, this->rotX, this->rotY, this->tlxDef, this->tlyDef, this->resXDef, this->resYDef, this->rotXDef, this->rotYDef);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
        }
        else if(option == RSGISExeImageUtils::genassesspoints)
        {
            cout << "IMPLEMENTATION NOT WORKING IN XML - SEE PYTHON INTERFACE (under classification)\n";
            cout << "Generating random points for accuracy assessment.\n";
            cout << "Input File: " << this->inputImage << endl;
            cout << "Classification Column: " << this->classColumnName << endl;
            cout << "Output File: " << this->outputFile << endl;
            if(demProvided)
            {
                cout << "DEM: " << this->inputDEM << endl;
            }
            else
            {
                cout << "A DEM was not provided therefore Elevations values will be 0.\n";
            }
            /*
            if(accuracyPtsType == rsgis_randompts)
            {
                cout << "Generating " << this->numPoints << " random points across the image.\n";
            }
            else if(accuracyPtsType ==  rsgis_stratifiedpts)
            {
                cout << "Generating " << this->numPoints << " stratified random points per class.\n";
            }
            cout << "Random generator seed: " << this->seed << endl;
            
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            GDALDataset *inDEMDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }

            if(demProvided)
            {
                inDEMDataset = (GDALDataset *) GDALOpen(this->inputDEM.c_str(), GA_ReadOnly);
                if(inDEMDataset == NULL)
                {
                    string message = string("Could not open image ") + this->inputDEM;
                    throw RSGISImageException(message.c_str());
                }
            }

            RSGISGenAccuracyPoints mapAcc;

            if(accuracyPtsType == rsgis_randompts)
            {
                mapAcc.generateRandomPoints(inDataset, inDEMDataset, this->demProvided, this->outputFile, this->classColumnName, this->numPoints, this->seed);
            }
            else if(accuracyPtsType ==  rsgis_stratifiedpts)
            {
                mapAcc.generateStratifiedRandomPoints(inDataset, inDEMDataset, this->demProvided, this->outputFile, this->classColumnName, this->numPoints, this->seed);
            }


            GDALClose(inDataset);
            GDALClose(inDEMDataset);
            GDALDestroyDriverManager();
             */
        }
        else if(option == RSGISExeImageUtils::uniquepxlclumps)
        {
            cout << "Create an image where each pixel is a unique \'clump\' \n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Output: " << this->outputImage << endl;
            if(noDataValDefined)
            {
                cout << "Data Value: " << this->nodataValue << endl;
            }
            else
            {
                cout << "A no data value has not been defined\n";
            }
            cout << "Image format: " << this->imageFormat << endl;

            GDALAllRegister();

			GDALDataset **datasets = NULL;

			RSGISCalcImage *calcImage = NULL;
			RSGISCalcImageValue *calcImageValue = NULL;

			try
			{
                datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

				calcImageValue = new RSGISUniquePixelClumps(this->noDataValDefined, this->nodataValue);
				calcImage = new RSGISCalcImage(calcImageValue, "", true);

				calcImage->calcImage(datasets, 1, this->outputImage, false, NULL, this->imageFormat, GDT_UInt32);

				delete calcImage;
				delete calcImageValue;

				GDALClose(datasets[0]);
				delete[] datasets;
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			GDALDestroyDriverManager();

        }
        else if(option == RSGISExeImageUtils::subset2img)
		{
            std::cout << "Subset image to another image\n";
            std::cout << "Image: " << this->inputImage << std::endl;
            std::cout << "ROI: " << this->inputROIImage << std::endl;
            std::cout << "Output: " << this->outputImage << std::endl;
            try
            {
                rsgis::cmds::executeSubset2Img(this->inputImage, this->inputROIImage, this->outputImage, this->imageFormat, this->rsgisOutDataType);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
		}
        else if(option == RSGISExeImageUtils::defineimgtiles)
        {
            cout << "Create an image where each pixel is a unique \'clump\' \n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Output: " << this->outputImage << endl;
            cout << "Tile Size: " << this->tileSizePxl << endl;
            cout << "Valid Pixel Ratio: " << this->validPixelRatio << endl;
            if(noDataValDefined)
            {
                cout << "Data Value: " << this->nodataValue << endl;
            }
            else
            {
                cout << "A no data value has not been defined\n";
            }
            cout << "Image format: " << this->imageFormat << endl;
            /*
            GDALAllRegister();

			try
			{
				GDALDataset *inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

                rsgis::rastergis::RSGISDefineImageTiles defImgTiles;
                defImgTiles.defineTiles(inDataset, this->outputImage, this->imageFormat, this->tileSizePxl, this->validPixelRatio, this->nodataValue, noDataValDefined);

				GDALClose(inDataset);
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			GDALDestroyDriverManager();
            */
        }
        else if(option == RSGISExeImageUtils::gentilemasks)
        {
            cout << "create individual mask images for each tile with an optional overlap.\n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Output Base: " << this->outputImageBase << endl;
            if(this->createAnOverlap)
            {
                cout << "Overlap: " << this->overlap << endl;
            }
            else
            {
                cout << "No overlap is being used\n";
            }
            cout << "Image format: " << this->imageFormat << endl;
            cout << "Image Extension: " << this->outFileExtension << endl;
            /*
            GDALAllRegister();

			try
			{
				GDALDataset *inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

                rsgis::rastergis::RSGISCreateImageTileMasks tileMasks;
                tileMasks.createTileMasks(inDataset, this->outputImageBase, this->imageFormat, this->outFileExtension, this->createAnOverlap, this->overlap, this->growOverlap);

				GDALClose(inDataset);
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			GDALDestroyDriverManager();
             */
        }
        else if(option == RSGISExeImageUtils::cutouttile)
        {
            cout << "A command to extract and a region for the tile and apply a mask.\n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Output: " << this->outputImage << endl;
            cout << "Tile: " << this->tileImage << endl;
            cout << "Image format: " << this->imageFormat << endl;
            cout << "No Data: " << this->nodataValue << endl;
            /*
            GDALAllRegister();

			try
			{
                GDALDataset **dataset = new GDALDataset*[2];

                dataset[0] = (GDALDataset *) GDALOpen(this->tileImage.c_str(), GA_ReadOnly);
				if(dataset[0] == NULL)
				{
					string message = string("Could not open image ") + this->tileImage;
					throw RSGISImageException(message.c_str());
				}

                dataset[1] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset[1] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

                rsgis::rastergis::RSGISCutOutTile *cutTiles = new rsgis::rastergis::RSGISCutOutTile(nodataValue, dataset[1]->GetRasterCount());
                rsgis::img::RSGISCalcImage calcTile = rsgis::img::RSGISCalcImage(cutTiles);

                calcTile.calcImage(dataset, 2, this->outputImage, false, NULL, this->imageFormat, this->outDataType);

                delete cutTiles;

				GDALClose(dataset[0]);
                GDALClose(dataset[1]);
                delete[] dataset;
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			GDALDestroyDriverManager();
            */
        }
        else if(option == RSGISExeImageUtils::stretchwithstats)
		{
			cout << "Apply an enhancement stretch to the an input image using defined stats\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Stats File: " << this->inputFile << endl;
			if(stretchType == rsgis::cmds::linearMinMax)
			{
				cout << "Linear Min-Max stretch\n";
			}
			else if(stretchType == rsgis::cmds::histogram)
			{
				cout << "Histogram stretch\n";
			}
			else if(stretchType == rsgis::cmds::exponential)
			{
				cout << "Exponential stretch\n";
			}
			else if(stretchType == rsgis::cmds::logarithmic)
			{
				cout << "Logarithmic stretch\n";
			}
			else if(stretchType == rsgis::cmds::powerLaw)
			{
				cout << power << " Power Law stretch\n";
			}
			else
			{
				throw RSGISException("Stretch is not recognised.");
			}


			try
			{
				GDALAllRegister();
				RSGISStretchImageWithStats *stretchImg = NULL;
				GDALDataset *inDataset = NULL;

				inDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

				stretchImg = new RSGISStretchImageWithStats(inDataset, this->outputImage, this->inputFile, this->imageFormat, this->outDataType);
				if(stretchType == rsgis::cmds::linearMinMax)
				{
					stretchImg->executeLinearMinMaxStretch();
				}
				else if(stretchType == rsgis::cmds::histogram)
				{
					stretchImg->executeHistogramStretch();
				}
				else if(stretchType == rsgis::cmds::exponential)
				{
					stretchImg->executeExponentialStretch();
				}
				else if(stretchType == rsgis::cmds::logarithmic)
				{
					stretchImg->executeLogrithmicStretch();
				}
				else if(stretchType == rsgis::cmds::powerLaw)
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
			catch(RSGISException& e)
			{
				throw e;
			}

		}
        else if(option == RSGISExeImageUtils::subsampleimage)
		{
			cout << "A command to extract a subsample of image data to a hdf5 file.\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output File: " << this->outputFile << endl;
            cout << "Sub-Sample: " << this->imageSample << endl;
            if(this->useIgnoreVal)
            {
                cout << "No data value: " << this->nodataValue << endl;
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

                rsgis::img::RSGISSampleImage sampleData;
                sampleData.subSampleImage(inDataset, this->outputFile, this->imageSample, this->nodataValue, this->useIgnoreVal);

				GDALClose(inDataset);
				GDALDestroyDriverManager();
			}
			catch(RSGISException& e)
			{
				throw e;
			}

		}
        else if(option == RSGISExeImageUtils::copyprojdef)
        {
            cout.precision(12);
            cout << "A command to copy the projection definition from one image to another\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Reference Image: " << this->inputFile << endl;
            try
            {
                rsgis::cmds::executeCopyProj(this->inputImage, this->inputFile);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
        }
        else if(option == RSGISExeImageUtils::copyprojdefspatialinfo)
        {
            cout.precision(12);
            cout << "A command to copy the projection definition and spatial information from one image to another\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Reference Image: " << this->inputFile << endl;
            try
            {
                rsgis::cmds::executeCopyProjSpatial(this->inputImage, this->inputFile);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
        }
        else if(option == RSGISExeImageUtils::imagerasterzone2hdf)
        {
            cout.precision(12);
            cout << "A command to extract the all the pixel values for raster regions to a HDF5 file (1 column for each image band).\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Mask Image: " << this->imageMask << endl;
            cout << "Output: " << this->outputHDFFile << endl;
            cout << "Mask Value: " << this->maskValue << endl;
            try
            {
                rsgis::cmds::executeImageRasterZone2HDF(this->inputImage, this->imageMask, this->outputHDFFile, this->maskValue);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
        }
        else if(option == RSGISExeImageUtils::bandselect)
        {
            cout.precision(12);
            cout << "A command to subset an image to a given set of the input image bands.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputFile << endl;
            cout << "Bands: ";
            bool first = true;
            for(std::vector<unsigned int>::iterator iterBands = selectBands.begin(); iterBands != selectBands.end(); ++iterBands)
            {
                if(first)
                {
                    cout << (*iterBands);
                    first = false;
                }
                else
                {
                    cout << ", " << (*iterBands);
                }
            }
            cout << endl;
            
            try
            {
                rsgis::cmds::executeSubsetImageBands(this->inputImage, this->outputFile, selectBands, this->imageFormat, this->rsgisOutDataType);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
        }
		else
		{
			cout << "Options not recognised\n";
		}
	}
}


void RSGISExeImageUtils::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeImageUtils::colour)
		{
			cout << "Colour Image\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			for(int i = 0; i < numClasses; i++)
			{
				cout << i <<  ") Class " << classColour[i]->className << " with ID ";
				cout << classColour[i]->classID << " uses image band " << classColour[i]->imgBand << "\n";
				cout << "Lower = " << classColour[i]->lower << endl;
				cout << "Upper = " << classColour[i]->upper << endl;
				cout << "Red = " << classColour[i]->red << endl;
				cout << "Green = " << classColour[i]->green << endl;
				cout << "Blue = " << classColour[i]->blue << endl;
			}
		}
        else if(option == RSGISExeImageUtils::colourimagebands)
		{
			cout << "Colour Image\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			for(int i = 0; i < numClasses; i++)
			{
				cout << i <<  ") Class " << classColour[i]->className << " with ID ";
                cout << classColour[i]->classID << endl;
                cout << "Lower = " << classColour[i]->lower << endl;
                cout << "Upper = " << classColour[i]->upper << endl;
                cout << "Red = " << classColour[i]->red << endl;
                cout << "Green = " << classColour[i]->green << endl;
                cout << "Blue = " << classColour[i]->blue << endl;
			}
		}
		else if(option == RSGISExeImageUtils::mosaic)
		{
			for(int i = 0; i < this->numImages; i++)
			{
				cout << "Input Image: " << this->inputImages[i] << endl;
			}
			cout << "Output Image: " << this->outputImage << endl;
			cout << "No Data Value: " << this->nodataValue << endl;

			if(projFromImage)
			{
				cout << "Projection is being taken from the first image in the list.\n";
			}
			else
			{
				cout << "Projection: " << proj << endl;
			}
		}
		else if(option == RSGISExeImageUtils::include)
		{
			cout << "Base Image: " << this->inputImage << endl;
			for(int i = 0; i < this->numImages; i++)
			{
				cout << "Input Image: " << this->inputImages[i] << endl;
			}
		}
		else if(option == RSGISExeImageUtils::cut2poly)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Filename attribute: " << this->filenameAttribute << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "No Data Value: " << this->nodataValue << endl;
		}
		else if(option == RSGISExeImageUtils::mask)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Image Mask: " << this->imageMask << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Mask Value: " << this->maskValue << endl;
		}
		else if(option == RSGISExeImageUtils::resample)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Scale Image: " << this->resampleScale << endl;
			if(this->interpolator == RSGISExeImageUtils::cubic)
			{
				cout << "Using a cubic interpolator\n";
			}
			else if(this->interpolator == RSGISExeImageUtils::billinearArea)
			{
				cout << "Using a bilinear (area) interpolator\n";
			}
			else if(this->interpolator == RSGISExeImageUtils::billinearPt)
			{
				cout << "Using a bilinear (point) interpolator\n";
			}
			else if(this->interpolator == RSGISExeImageUtils::nn)
			{
				cout << "Using a nearest neighbour interpolator\n";
			}
			else if(this->interpolator == RSGISExeImageUtils::trangular)
			{
				cout << "Using a triangular interpolator\n";
			}
		}
		else if(option == RSGISExeImageUtils::rasterisedefiniens)
		{
			cout << "Rasterise Definiens\n";
			cout << "Input DIR: " << this->inputDIR << endl;
			cout << "Output DIR: " << this->outputDIR << endl;
			if(projFromImage)
			{
				cout << "Projection is being taken from the first image in the list.\n";
			}
			else
			{
				cout << "Projection: " << proj << endl;
			}
		}
		else if(option == RSGISExeImageUtils::printProj4)
		{
			cout << "Print Spatial Reference Proj4\n";
			cout << "Input Image: " << this->inputImage << endl;
		}
		else if(option == RSGISExeImageUtils::printWKT)
		{
			cout << "Print Spatial Reference WKT\n";
			cout << "Input Image: " << this->inputImage << endl;
		}
		else if(option == RSGISExeImageUtils::extract2dscatterptxt)
		{
			cout << "Extract image data to create 2D scatter plot (exported as ptxt)\n";

			cout << "Input Images:\n";
			for(int i = 0; i < this->numImages; ++i)
			{
				cout << i << ") " << this->inputImages[i] << endl;
			}
			cout << "Image Band 1: " << this->imgBand1 << endl;
			cout << "Image Band 2: " << this->imgBand2 << endl;
			cout << "Output File: " << this->outputFile << endl;
		}
		else if(option == RSGISExeImageUtils::sgsmoothing)
		{
			cout << "Apply Savitzky-Golay Smoothing Filters to data\n";

			cout << "Input Image = " << this->inputImage << endl;
			cout << "Output Image = " << this->outputImage << endl;
			cout << "Image Band Values = " << this->bandFloatValuesVector << endl;
			cout << "Order = " << this->order << endl;
			cout << "Window = " << this->window << endl;
		}
		else if(option == RSGISExeImageUtils::cumulativearea)
		{
			cout << "Calculate the cumulative area of the image profile/spectra\n";
			cout << "Input Image = " << this->inputImage << endl;
			cout << "Output Image = " << this->outputImage << endl;
			cout << "Image Band Widths = " << this->bandFloatValuesVector << endl;
		}
		else if(option == RSGISExeImageUtils::createimage)
		{
			cout << "Create a new blank image\n";
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Num. Image bands: " << this->numBands << endl;
			cout << "Size: [" << this->width << "," << this->height << "]\n";
			cout << "TL Geo: [" << this->eastings << "," << this->northings << "]\n";
			cout << "Resolution: " << this->resolution << endl;
			cout << "Default Value: " << this->outValue << endl;
			cout << "Projection: " << this->proj << endl;
		}
		else if(option == RSGISExeImageUtils::stretch)
		{
			cout << "Apply an enhancement stretch to the an input image - usually for visualisation\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			if(stretchType == rsgis::cmds::linearMinMax)
			{
				cout << "Linear Min-Max stretch\n";
			}
			else if(stretchType == rsgis::cmds::linearPercent)
			{
				cout << "Linear " << percent << " % stretch\n";
			}
			else if(stretchType == rsgis::cmds::linearStdDev)
			{
				cout << "Linear " << stddev << " Standard Deviation stretch\n";
			}
			else if(stretchType == rsgis::cmds::histogram)
			{
				cout << "Histogram stretch\n";
			}
			else if(stretchType == rsgis::cmds::exponential)
			{
				cout << "Exponential stretch\n";
			}
			else if(stretchType == rsgis::cmds::logarithmic)
			{
				cout << "Logarithmic stretch\n";
			}
			else if(stretchType == rsgis::cmds::powerLaw)
			{
				cout << "Power Law stretch\n";
			}
			else
			{
				throw RSGISException("Stretch is not recognised.");
			}

            if(this->ignoreZeros)
            {
                cout << "Ignoring Zeros\n";
            }
		}
		else if(option == RSGISExeImageUtils::huecolour)
		{
			cout << "Generate a colour image (through Hue) representing an image band - usually for visualisation\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Band: " << this->imgBand1 << endl;
			cout << "Range: [" << this->lowerRangeValue << "," << this->upperRangeValue << "]\n";
			cout << "Background value: " << this->nodataValue << endl;
		}
		else if(option == RSGISExeImageUtils::definespatialref)
		{
			cout << "Make a copy of the input image and define the projection and spatial locations\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Projection: " << this->proj << endl;
			cout << "TL [" << this->eastings << "," << this->northings << "]\n";
			cout << "Resolution [" << this->xRes << "," << this->yRes << "]\n";
		}
        else if(option == RSGISExeImageUtils::createslices)
		{
            cout << "Create Image slices from a multiband input image\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image Base: " << this->outputImage << endl;
        }
        else if(option == RSGISExeImageUtils::clump)
        {
            cout << "Clump the input image for a given values.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
        }
        else if(option == RSGISExeImageUtils::relabel)
        {
            cout << "A command to relabel image pixel using a look up table from a gmtxt matrix file.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Look Up Table: " << this->lutMatrixFile << endl;
        }
        else if(option == RSGISExeImageUtils::assignproj)
        {
            cout << "Assign and update and image to a specific projection\n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Projection File: " << this->projFile << endl;
        }
        else if(option == RSGISExeImageUtils::popimgstats)
        {
            cout << "Populate an image with image statistics and image pyramids\n";
            cout << "Image: " << this->inputImage << endl;
            if(this->useIgnoreVal)
            {
                cout << "Ignore Val: " << this->nodataValue << endl;
            }
            if(this->calcImgPyramids)
            {
                cout << "Calculating image pyramids\n";
            }
        }
        else if(option == RSGISExeImageUtils::createcopy)
        {
            cout << "Create a new image from an existing image\n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Output: " << this->outputImage << endl;
            cout << "Data Value: " << this->dataValue << endl;
            cout << "Num Image Bands: " << this->numBands << endl;
            cout << "Image format: " << this->imageFormat << endl;
        }
        else if(option == RSGISExeImageUtils::bandcolourusage)
        {
            cout << "Defining the colour usage parameter on the image bands\n";
            cout << "Image: " << this->inputImage << std::endl;
            for(std::vector<std::pair<unsigned int, GDALColorInterp> >::iterator iterUsages = bandClrUses.begin(); iterUsages != bandClrUses.end(); ++iterUsages)
            {
                cout << "Band " << (*iterUsages).first;
                if((*iterUsages).second == GCI_GrayIndex)
                {
                    cout << " Greyscale usage\n";
                }
                else if((*iterUsages).second == GCI_PaletteIndex)
                {
                    cout << " Paletted usage\n";
                }
                else if((*iterUsages).second == GCI_RedBand)
                {
                    cout << " Red usage\n";
                }
                else if((*iterUsages).second == GCI_GreenBand)
                {
                    cout << " Green usage\n";
                }
                else if((*iterUsages).second == GCI_BlueBand)
                {
                    cout << " Blue usage\n";
                }
                else if((*iterUsages).second == GCI_AlphaBand)
                {
                    cout << " Alpha usage\n";
                }
            }
        }
        else if(option == RSGISExeImageUtils::assignspatialinfo)
        {
            cout << "Assign and update image spatial header info\n";
            cout << "Image: " << this->inputImage << endl;
            if(this->tlxDef)
            {
                cout << "TLX: " << this->tlx << std::endl;
            }
            if(this->tlyDef)
            {
                cout << "TLY: " << this->tly << std::endl;
            }
            if(this->resXDef)
            {
                cout << "Res X: " << this->resX << std::endl;
            }
            if(this->resYDef)
            {
                cout << "Res Y: " << this->resY << std::endl;
            }
            if(this->rotXDef)
            {
                cout << "Rot X: " << this->rotX << std::endl;
            }
            if(this->rotYDef)
            {
                cout << "Rot Y: " << this->rotY << std::endl;
            }
        }
        else if(option == RSGISExeImageUtils::genassesspoints)
        {
            cout << "Generating random points for accuracy assessment.\n";
            cout << "Input File: " << this->inputImage << endl;
            cout << "Classification Column: " << this->classColumnName << endl;
            cout << "Output File: " << this->outputFile << endl;
            if(demProvided)
            {
                cout << "DEM: " << this->inputDEM << endl;
            }
            else
            {
                cout << "A DEM was not provided therefore Elevations values will be 0.\n";
            }
            /*
            if(accuracyPtsType == rsgis_randompts)
            {
                cout << "Generating " << this->numPoints << " random points across the image.\n";
            }
            else if(accuracyPtsType ==  rsgis_stratifiedpts)
            {
                cout << "Generating " << this->numPoints << " stratified random points per class.\n";
            }
             */
            cout << "Random generator seed: " << this->seed << endl;
        }
        else if(option == RSGISExeImageUtils::defineimgtiles)
        {
            cout << "Create an image where each pixel is a unique \'clump\' \n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Output: " << this->outputImage << endl;
            cout << "Tile Size: " << this->tileSizePxl << endl;
            cout << "Valid Pixel Ratio: " << this->validPixelRatio << endl;
            if(noDataValDefined)
            {
                cout << "Data Value: " << this->nodataValue << endl;
            }
            else
            {
                cout << "A no data value has not been defined\n";
            }
            cout << "Image format: " << this->imageFormat << endl;
        }
        else if(option == RSGISExeImageUtils::gentilemasks)
        {
            cout << "create individual mask images for each tile with an optional overlap.\n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Output Base: " << this->outputImageBase << endl;
            if(this->createAnOverlap)
            {
                cout << "Overlap: " << this->overlap << endl;
            }
            else
            {
                cout << "No overlap is being used\n";
            }
            cout << "Image format: " << this->imageFormat << endl;
            cout << "Image Extension: " << this->outFileExtension << endl;
        }
        else if(option == RSGISExeImageUtils::cutouttile)
        {
            cout << "A command to extract and a region for the tile and apply a mask.\n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Output: " << this->outputImage << endl;
            cout << "Tile: " << this->tileImage << endl;
            cout << "Image format: " << this->imageFormat << endl;
            cout << "No Data: " << this->nodataValue << endl;
        }
        else if(option == RSGISExeImageUtils::stretchwithstats)
		{
			cout << "Apply an enhancement stretch to the an input image using defined stats\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Stats File: " << this->inputFile << endl;
			if(stretchType == rsgis::cmds::linearMinMax)
			{
				cout << "Linear Min-Max stretch\n";
			}
			else if(stretchType == rsgis::cmds::histogram)
			{
				cout << "Histogram stretch\n";
			}
			else if(stretchType == rsgis::cmds::exponential)
			{
				cout << "Exponential stretch\n";
			}
			else if(stretchType == rsgis::cmds::logarithmic)
			{
				cout << "Logarithmic stretch\n";
			}
			else if(stretchType == rsgis::cmds::powerLaw)
			{
				cout << power << " Power Law stretch\n";
			}
			else
			{
				throw RSGISException("Stretch is not recognised.");
			}
        }
        else if(option == RSGISExeImageUtils::subsampleimage)
		{
			cout << "A command to extract a subsample of image data to a hdf5 file.\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output File: " << this->outputFile << endl;
            cout << "Sub-Sample: " << this->imageSample << endl;
            if(this->useIgnoreVal)
            {
                cout << "No data value: " << this->nodataValue << endl;
            }
        }
        else if(option == RSGISExeImageUtils::copyprojdef)
        {
            cout << "A command to copy the projection definition from one image to another\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Reference Image: " << this->inputFile << endl;
        }
        else if(option == RSGISExeImageUtils::copyprojdefspatialinfo)
        {
            cout << "A command to copy the projection definition and spatial information from one image to another\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Reference Image: " << this->inputFile << endl;
        }
        else if(option == RSGISExeImageUtils::imagerasterzone2hdf)
        {
            cout.precision(12);
            cout << "A command to extract the all the pixel values for raster regions to a HDF5 file (1 column for each image band).\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Mask Image: " << this->imageMask << endl;
            cout << "Output: " << this->outputHDFFile << endl;
            cout << "Mask Value: " << this->maskValue << endl;
        }
        else if(option == RSGISExeImageUtils::bandselect)
        {
            cout.precision(12);
            cout << "A command to subset an image to a given set of the input image bands.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputFile << endl;
            cout << "Bands: ";
            bool first = true;
            for(std::vector<unsigned int>::iterator iterBands = selectBands.begin(); iterBands != selectBands.end(); ++iterBands)
            {
                if(first)
                {
                    cout << (*iterBands);
                    first = false;
                }
                else
                {
                    cout << ", " << (*iterBands);
                }
            }
            cout << endl;
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

void RSGISExeImageUtils::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
    cout << "<!-- A command that will generate an RGB colour image based on thresholds applied to the input image - useful for generating quicklooks and visualisation of classification -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"colourimage\" image=\"image.env\" output=\"image_out.env\">" << endl;
    cout << "    <rsgis:colour name=\"class_name_1\" id=\"int\" band=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << endl;
    cout << "    <rsgis:colour name=\"class_name_2\" id=\"int\" band=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << endl;
    cout << "    <rsgis:colour name=\"class_name_3\" id=\"int\" band=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command that will generate an RGB colour image based on thresholds applied to each input image band, generating a new image for each input band - useful for generating quicklooks and visualisation of classification.-->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"colourimagebands\" image=\"image.env\" output=\"image_out_base\">" << endl;
    cout << "    <rsgis:colour name=\"class_name_1\" id=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << endl;
    cout << "    <rsgis:colour name=\"class_name_2\" id=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << endl;
    cout << "    <rsgis:colour name=\"class_name_3\" id=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to mosaic a set of input images to generate a single output image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"mosaic\" output=\"image_out.env\" nodata=\"float=0\" skipValue=\"float (optional)\" skipLowerThresh=\"float (optional)\" skipUpperThresh=\"float (optional)\"  proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" setSkipBand=\"1\" >" << endl;
    cout << "    <rsgis:image file=\"image1\" />" << endl;
    cout << "    <rsgis:image file=\"image2\" />" << endl;
    cout << "    <rsgis:image file=\"image3\" />" << endl;
    cout << "    <rsgis:image file=\"image4\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to mosaic a set of input images from a directory to generate a single output image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"mosaic\" dir=\"directory\" ext=\"file_extension\" output=\"image_out.env\" nodata=\"float=0\" skipValue=\"float (optional)\" skipLowerThresh=\"float (optional)\" skipUpperThresh=\"float (optional)\"  proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to include a set of input images into an existing image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"include\" image=\"base_image.env\" >" << endl;
    cout << "    <rsgis:image file=\"image1\" />" << endl;
    cout << "    <rsgis:image file=\"image2\" />" << endl;
    cout << "    <rsgis:image file=\"image3\" />" << endl;
    cout << "    <rsgis:image file=\"image4\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to include a set of input images from a directory into an existing image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"include\" image=\"base_image.env\" dir=\"directory\" ext=\"file_extension\" />" << endl;
    cout << "<!-- A command to cut an image to an input shapefile where each polygon geometry will created a new output image representing the region within the polygon -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"cut2poly\" image=\"image.env\" output=\"image_out_base\" vector=\"vector.shp\" outfilename=\"attribute\" nodata=\"float\" />" << endl;
    cout << "<!-- A command to mask the input image with a second 'mask' image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"mask\" image=\"image.env\" mask=\"mask.env\" output=\"image_out.env\" maskvalue=\"float\" />" << endl;
    cout << "<!-- A command resample an input image to another resolution -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"resample\" image=\"image.env\" output=\"image_out.env\" scale=\"float\" interpolation=\"Cubic | BilinearArea | BilinearPoint | NN | Triangular\" />" << endl;
    cout << "<!-- TODO A command to create a multiband raster image, based on thematic rasters exported from Definiens. The values for each band are held in a csv file (exported with raster image from Definiens -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"rasterisedefiniens\" [inDIR=\"/input/directory/\" outDIR=\"/output/directory\"] [image=\"image.env\" csv=\"input.csv\" output=\"image_out.env\"] proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command which prints (to the console) the proj4 string representing the projection of the inputted image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"printProj4\" image=\"image.env\" />" << endl;
    cout << "<!-- A command which prints (to the console) the WKT string representing the projection of the inputted image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"printWKT\" image=\"image.env\" />" << endl;
    cout << "<!-- A command extracts pixel values from two image bands and output them as 2D scatter ptxt file -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"extract2dscatterptxt\" image=\"image.env\" band1=\"int\" band2=\"int\" output=\"string\" />" << endl;
    cout << "<!-- A command extracts pixel values from two image bands (bands are numbered sequencially down the list of input files) and output them as 2D scatter ptxt file -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"extract2dscatterptxt\" band1=\"int\" band2=\"int\" output=\"string\" >" << endl;
    cout << "    <rsgis:image file=\"image1\" />" << endl;
    cout << "    <rsgis:image file=\"image2\" />" << endl;
    cout << "    <rsgis:image file=\"image3\" />" << endl;
    cout << "    <rsgis:image file=\"image4\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to smooth an spectral profiles of the input image pixels -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"sgsmoothing\" image=\"image.env\" output=\"image_out.env\" order=\"int\" window=\"int\" imagebands=\"vector.mtxt\"/>" << endl;
    cout << "<!-- A command to generate a cumulativeAreaImage from a spectral curve (or profile) -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"cumulativearea\" image=\"image.env\" output=\"image_out.env\" imagebands=\"matrix.mtxt\"/>" << endl;
    cout << "<!-- A command to generate new image with a default value -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"createimage\" output=\"image_out.env\" numbands=\"int\" width=\"int\" height=\"int\" resolution=\"float\" eastings=\"double\" northings=\"double\" proj4=\"string\" value=\"float\"/>" << endl;
    cout << "<!-- A command to apply an enhancement stretch an images pixel values to a range of 0 to 255 - normally used for visualisation -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"stretch\" image=\"image.env\" output=\"image_out.env\" ignorezeros=\"yes | no\" stretch=\"LinearMinMax | LinearPercent | LinearStdDev | Histogram | Exponential | Logarithmic | PowerLaw\" percent=\"float - only LinearPercent\" stddev=\"float - only LinearStdDev\" power=\"float - only PowerLaw\"/>" << endl;
    cout << "<!-- A command to colour to generate a colour image, using the Hue of a HSV transformation, for a particular input image band -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"huecolour\" image=\"image.env\" output=\"image_out.env\" band=\"int\" lowvalue=\"float\" highvalue=\"float\" background=\"float\" />" << endl;
    cout << "<!-- A command to remove / define spatial reference to nothing and size set to pixel size -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"removespatialref\" image=\"image.env\" output=\"image_out.env\" />" << endl;
    cout << "<!-- A command to add noise to an image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"addnoise\" type=\"randomNoise | percentGaussianNoise\" scale=\"float\" image=\"image.env\" output=\"image_out.env\"/>" << endl;
    cout << "<!-- A command to subset an image to the same extent as a shapefile -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"subset\" image=\"image.env\" output=\"output_img.env\" vector=\"vector.shp\" />" << endl;
    cout << "<!-- A command to subset an image to polygons within shapefile -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"subset2polys\" image=\"image.env\" output=\"image_out_base\" vector=\"vector.shp\" outfilename=\"attribute\" />" << endl;
    cout << "<!-- A command to pan sharpen an image, takes stack of multispectral image (resampled to pan resolution) and panchromatic image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"pansharpen\" image=\"ms_pan_img.env\" output=\"ps_image.env\" />" << endl;
    cout << "<!-- A command to generate a set of slices from a multi-band image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"createslices\" image=\"image.env\" output=\"image_out_base\" />" << endl;
    cout << "<!-- A command to clump an image for a given pixel values -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"clump\" image=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to create a composite image from a multi-band input image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"composite\" image=\"image.env\" output=\"image_out.env\" compositeBands=\"int\" stats=\"mean | min | max | range\" />" << endl;
    cout << "<!-- A command to relabel image pixel using a look up table from a gmtxt matrix file (m=2 n=X) -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"relabel\" image=\"image.env\" output=\"image_out.env\" lut=\"matrix.gmtxt\" />" << endl;
    cout << "<!-- A command to assign and update and image to a specific projection -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"assignproj\" image=\"image.env\" projwkt=\"txt.wkt\" />" << endl;
	cout << "</rsgis:commands>\n";
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




