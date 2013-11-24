/*
 *  RSGISExeFilterImages.cpp
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

#include "RSGISExeFilterImages.h"

namespace rsgisexe{

RSGISExeFilterImages::RSGISExeFilterImages() : rsgis::RSGISAlgorithmParameters()
{
	this->algorithm = "imagefilter";
	this->option = RSGISExeFilterImages::none;
	this->inputImage = "";
	this->outputImageBase = "";
	this->filterBank = NULL;
}

rsgis::RSGISAlgorithmParameters* RSGISExeFilterImages::getInstance()
{
	return new RSGISExeFilterImages();
}

void RSGISExeFilterImages::retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException)
{
	rsgis::math::RSGISMathsUtils mathUtils;

	const XMLCh *algorName = xercesc::XMLString::transcode(this->algorithm.c_str());
	const XMLCh *optionFilter = xercesc::XMLString::transcode("filter");
	const XMLCh *optionExportFilterBank = xercesc::XMLString::transcode("exportfilterbank");
    const XMLCh *optionNLDenoising = xercesc::XMLString::transcode("nldenoising");

	const XMLCh *algorNameEle = argElement->getAttribute(xercesc::XMLString::transcode("algor"));
	if(!xercesc::XMLString::equals(algorName, algorNameEle))
	{
		throw rsgis::RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}

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
        this->outputImageBase = std::string(charValue);
        xercesc::XMLString::release(&charValue);
    }
    else
    {
        throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
    }
    xercesc::XMLString::release(&outputXMLStr);


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

    // Set output image fomat (defaults to KEA)
	this->imageExt = "kea";
	XMLCh *extensionXMLStr = xercesc::XMLString::transcode("extension");
	if(argElement->hasAttribute(extensionXMLStr))
	{
		char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(extensionXMLStr));
		this->imageExt = std::string(charValue);
		xercesc::XMLString::release(&charValue);
	}
    else if(this->imageFormat != "KEA")
    {
        throw rsgis::RSGISXMLArgumentsException("No \'extension\' attribute was provided.");
    }
	xercesc::XMLString::release(&extensionXMLStr);

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

	if(argElement->hasAttribute(xercesc::XMLString::transcode("filterbank")))
	{
		const XMLCh *filterBankXMLStr = argElement->getAttribute(xercesc::XMLString::transcode("filterbank"));
		std::cout << "Default Filter Bank " << xercesc::XMLString::transcode(filterBankXMLStr) << " to be created\n";

		const XMLCh *filterBankLM = xercesc::XMLString::transcode("LM");
		if(xercesc::XMLString::equals(filterBankXMLStr, filterBankLM))
		{
			this->filterParameters = rsgis::cmds::createLeungMalikFilterBank();
		}
		std::cout << "Created filter banks\n";
	}
	else
	{
		xercesc::DOMNodeList *filterNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:filter"));
		int numFilters = filterNodesList->getLength();
		xercesc::DOMElement *filterElement = NULL;

		XMLCh *filterTypeGuassianSmooth = xercesc::XMLString::transcode("GaussianSmooth");
		XMLCh *filterTypeGuassian1st = xercesc::XMLString::transcode("Gaussian1st");
		XMLCh *filterTypeGuassian2nd = xercesc::XMLString::transcode("Gaussian2nd");
		XMLCh *filterTypeLaplacian = xercesc::XMLString::transcode("Laplacian");
		XMLCh *filterTypeSobel = xercesc::XMLString::transcode("Sobel");
		XMLCh *filterTypePrewitt = xercesc::XMLString::transcode("Prewitt");
		XMLCh *filterTypeMean = xercesc::XMLString::transcode("Mean");
		XMLCh *filterTypeMedian = xercesc::XMLString::transcode("Median");
		XMLCh *filterTypeMode = xercesc::XMLString::transcode("Mode");
		XMLCh *filterTypeRange = xercesc::XMLString::transcode("Range");
		XMLCh *filterTypeStdDev = xercesc::XMLString::transcode("StdDev");
        XMLCh *filterTypeCoeffOfVar = xercesc::XMLString::transcode("CoeffOfVar");
		XMLCh *filterTypeMin = xercesc::XMLString::transcode("Min");
		XMLCh *filterTypeMax = xercesc::XMLString::transcode("Max");
		XMLCh *filterTypeTotal = xercesc::XMLString::transcode("Total");
		XMLCh *filterTypeKuwahara = xercesc::XMLString::transcode("Kuwahara");
		XMLCh *filterTypeHarlick = xercesc::XMLString::transcode("Harlick");
		XMLCh *filterTypeFree = xercesc::XMLString::transcode("Free");
        XMLCh *filterTypeLee = xercesc::XMLString::transcode("Lee");
        XMLCh *filterTypeNormVar = xercesc::XMLString::transcode("NormVar");
        XMLCh *filterTypeNormVarPow = xercesc::XMLString::transcode("NormVarPower");
        XMLCh *filterTypeNormVarSqrt = xercesc::XMLString::transcode("NormVarSqrt");
        XMLCh *filterTypeNormVarAmp = xercesc::XMLString::transcode("NormVarAmplitude");
        XMLCh *filterTypeNormVarLn = xercesc::XMLString::transcode("NormVarLn");
        XMLCh *filterTypeNormVarLnPow = xercesc::XMLString::transcode("NormVarLnPower");
        XMLCh *filterTypeNormLn = xercesc::XMLString::transcode("NormLn");
        XMLCh *filterTypeTextureVar = xercesc::XMLString::transcode("TextureVar");

		XMLCh *filterTypeOptionX = xercesc::XMLString::transcode("x");
		XMLCh *filterTypeOptionY = xercesc::XMLString::transcode("y");
		XMLCh *filterTypeOptionXY = xercesc::XMLString::transcode("xy");
                
        this->filterParameters = new std::vector<rsgis::cmds::RSGISFilterParameters*>();
        this->filterParameters->reserve(numFilters);
        
        rsgis::cmds::RSGISFilterParameters *filterPar = NULL;
        
		for(int i = 0; i < numFilters; i++)
		{
			filterElement = static_cast<xercesc::DOMElement*>(filterNodesList->item(i));
			const XMLCh *filterType = filterElement->getAttribute(xercesc::XMLString::transcode("type"));
			std::cout << "Filter: " << xercesc::XMLString::transcode(filterType) << std::endl;

			if(xercesc::XMLString::equals(filterTypeGuassianSmooth, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));
				float stddevX = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("stddevX"))));
				float stddevY = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("stddevY"))));
				float angle = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("angle"))));
				float angleRadians = angle*(M_PI/180);

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "GaussianSmooth";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                filterPar->stddevX = stddevX;
                filterPar->stddevY = stddevY;
                filterPar->angle = angleRadians;
                
                this->filterParameters->push_back(filterPar);

			}
			else if(xercesc::XMLString::equals(filterTypeGuassian1st, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));
				float stddevX = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("stddevX"))));
				float stddevY = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("stddevY"))));
				float angle = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("angle"))));
				float angleRadians = angle*(M_PI/180);

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Gaussian1st";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                filterPar->stddevX = stddevX;
                filterPar->stddevY = stddevY;
                filterPar->angle = angleRadians;
                
                this->filterParameters->push_back(filterPar);

			}
			else if(xercesc::XMLString::equals(filterTypeGuassian2nd, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));
				float stddevX = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("stddevX"))));
				float stddevY = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("stddevY"))));
				float angle = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("angle"))));
				float angleRadians = angle*(M_PI/180);
                
                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Gaussian2nd";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                filterPar->stddevX = stddevX;
                filterPar->stddevY = stddevY;
                filterPar->angle = angleRadians;
                
                this->filterParameters->push_back(filterPar);
                
			}
			else if(xercesc::XMLString::equals(filterTypeLaplacian, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));
				float stddev = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("stddev"))));
                
                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Laplacian";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                filterPar->stddev = stddev;
                
                this->filterParameters->push_back(filterPar);

			}
			else if(xercesc::XMLString::equals(filterTypeSobel, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);
                
                filterPar = new rsgis::cmds::RSGISFilterParameters();
                filterPar->type = "Sobel";
                filterPar->fileEnding = fileEnding;
                
                const XMLCh *sobelFilterOption = filterElement->getAttribute(xercesc::XMLString::transcode("option"));

				if(xercesc::XMLString::equals(filterTypeOptionX, sobelFilterOption))
				{
					filterPar->option = "x";
				}
				else if(xercesc::XMLString::equals(filterTypeOptionY, sobelFilterOption))
				{
					filterPar->option = "y";
				}
				else if(xercesc::XMLString::equals(filterTypeOptionXY, sobelFilterOption))
				{
					filterPar->option = "xy";
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("Sobel type not recognised");
				}

                this->filterParameters->push_back(filterPar);
			}
			else if(xercesc::XMLString::equals(filterTypePrewitt, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);
                
                filterPar = new rsgis::cmds::RSGISFilterParameters();
                filterPar->type = "Prewitt";
                filterPar->fileEnding = fileEnding;
                
                const XMLCh *prewittFilterOption = filterElement->getAttribute(xercesc::XMLString::transcode("option"));
                
				if(xercesc::XMLString::equals(filterTypeOptionX, prewittFilterOption))
				{
					filterPar->option = "x";
				}
				else if(xercesc::XMLString::equals(filterTypeOptionY, prewittFilterOption))
				{
					filterPar->option = "y";
				}
				else if(xercesc::XMLString::equals(filterTypeOptionXY, prewittFilterOption))
				{
					filterPar->option = "xy";
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("Prewitt type not recognised");
				}
                
                this->filterParameters->push_back(filterPar);

			}
			else if(xercesc::XMLString::equals(filterTypeMean, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Mean";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);
                
			}
			else if(xercesc::XMLString::equals(filterTypeMedian, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);
                
                int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));
                
                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Median";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);

			}
			else if(xercesc::XMLString::equals(filterTypeMode, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Mode";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);
                
			}
			else if(xercesc::XMLString::equals(filterTypeRange, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Range";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);

			}
			else if(xercesc::XMLString::equals(filterTypeStdDev, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "StdDev";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);

			}
            else if(xercesc::XMLString::equals(filterTypeCoeffOfVar, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "CoeffOfVar";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);
                
			}
			else if(xercesc::XMLString::equals(filterTypeMin, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Min";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);

			}
			else if(xercesc::XMLString::equals(filterTypeMax, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Max";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);

			}
			else if(xercesc::XMLString::equals(filterTypeTotal, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Total";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);

			}
			else if(xercesc::XMLString::equals(filterTypeKuwahara, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Kuwahara";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);
                
			}
            else if(xercesc::XMLString::equals(filterTypeLee, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

				int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                unsigned int nLooks = size; // Set number of looks to window size
                
                XMLCh *nLooksXMLStr = xercesc::XMLString::transcode("nLooks");
                if(argElement->hasAttribute(nLooksXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(nLooksXMLStr));
                    nLooks = mathUtils.strtoint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    nLooks = size;
                }
                xercesc::XMLString::release(&nLooksXMLStr);
                
                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Lee";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);

			}
            // Normalised variance of DN (Power for SAR image)
            else if((xercesc::XMLString::equals(filterTypeNormVar, filterType)) | (xercesc::XMLString::equals(filterTypeNormVarPow, filterType)))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

                int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "NormVar";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);

			}
            // Normalised variance of Sqrt DN (Amplitude for SAR image)
            else if((xercesc::XMLString::equals(filterTypeNormVarSqrt, filterType)) | (xercesc::XMLString::equals(filterTypeNormVarAmp, filterType)))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

                int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));
                
                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "NormVarSqrt";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);
                
			}
            // Normalised variance of ln DN (Natural log Power for SAR image)
            else if((xercesc::XMLString::equals(filterTypeNormVarLn, filterType)) | (xercesc::XMLString::equals(filterTypeNormVarLnPow, filterType)))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

                int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "NormVarLn";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);
                
			}
            // Normalised natural log
            else if(xercesc::XMLString::equals(filterTypeNormLn, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

                int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));
                
                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "NormLn";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);
                
			}
            // Texture variance (eq. 21.89b in Ulaby et. al, Microwave Remove Sensing - Vol III)
            else if(xercesc::XMLString::equals(filterTypeTextureVar, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(xercesc::XMLString::transcode("fileending"));
				std::string fileEnding = xercesc::XMLString::transcode(fileEndingStr);

                int size = mathUtils.strtofloat(xercesc::XMLString::transcode(filterElement->getAttribute(xercesc::XMLString::transcode("size"))));

                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "TextureVar";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                
                this->filterParameters->push_back(filterPar);
                
			}
			else if(xercesc::XMLString::equals(filterTypeHarlick, filterType))
			{
				throw rsgis::RSGISXMLArgumentsException("Harlick features are not implemented");
			}
			else if(xercesc::XMLString::equals(filterTypeFree, filterType))
			{
				throw rsgis::RSGISXMLArgumentsException("Free filter not implemented");
			}
			else
			{
				throw rsgis::RSGISXMLArgumentsException("Filter Type not recognised");
			}
		}

        xercesc::XMLString::release(&filterTypeGuassianSmooth);
        xercesc::XMLString::release(&filterTypeGuassian1st);
        xercesc::XMLString::release(&filterTypeGuassian2nd);
        xercesc::XMLString::release(&filterTypeLaplacian);
        xercesc::XMLString::release(&filterTypeSobel);
        xercesc::XMLString::release(&filterTypePrewitt);
        xercesc::XMLString::release(&filterTypeMean);
        xercesc::XMLString::release(&filterTypeMedian);
        xercesc::XMLString::release(&filterTypeMode);
        xercesc::XMLString::release(&filterTypeRange);
        xercesc::XMLString::release(&filterTypeStdDev);
        xercesc::XMLString::release(&filterTypeCoeffOfVar);
        xercesc::XMLString::release(&filterTypeMin);
        xercesc::XMLString::release(&filterTypeMax);
        xercesc::XMLString::release(&filterTypeTotal);
        xercesc::XMLString::release(&filterTypeKuwahara);
        xercesc::XMLString::release(&filterTypeHarlick);
        xercesc::XMLString::release(&filterTypeFree);
        xercesc::XMLString::release(&filterTypeLee);
        xercesc::XMLString::release(&filterTypeNormVar);
        xercesc::XMLString::release(&filterTypeNormVarPow);
        xercesc::XMLString::release(&filterTypeNormVarSqrt);
        xercesc::XMLString::release(&filterTypeNormVarAmp);
        xercesc::XMLString::release(&filterTypeNormVarLn);
        xercesc::XMLString::release(&filterTypeNormVarLnPow);
        xercesc::XMLString::release(&filterTypeNormLn);
        xercesc::XMLString::release(&filterTypeTextureVar);
        xercesc::XMLString::release(&filterTypeOptionX);
        xercesc::XMLString::release(&filterTypeOptionY);
        xercesc::XMLString::release(&filterTypeOptionXY);
    }

    const XMLCh *optionXML = argElement->getAttribute(xercesc::XMLString::transcode("option"));
    if(xercesc::XMLString::equals(optionFilter, optionXML))
    {
        this->option = RSGISExeFilterImages::filter;
    }
    else if(xercesc::XMLString::equals(optionNLDenoising, optionXML))
    {
        this->option = RSGISExeFilterImages::nldenoising;
    }
    else if(xercesc::XMLString::equals(optionExportFilterBank, optionXML))
    {
        this->option = RSGISExeFilterImages::exportfilterbank;
    }
    else
    {
        std::string message = std::string("The option (") + std::string(xercesc::XMLString::transcode(optionXML)) + std::string(") is not known: RSGISExeFilterImages.");
        throw rsgis::RSGISXMLArgumentsException(message.c_str());
    }

    parsed = true;
}

void RSGISExeFilterImages::runAlgorithm() throw(rsgis::RSGISException)
{
    if(!parsed)
    {
        throw rsgis::RSGISException("Before running the parameters much be retrieved");
    }
    else
    {
        if(option == RSGISExeFilterImages::filter)
        {
            try
            {
                rsgis::cmds::executeFilter(this->inputImage, this->filterParameters, this->outputImageBase, this->imageFormat, this->imageExt, this->rsgisOutDataType);
                
                // Delete filter parameters
                for(std::vector<rsgis::cmds::RSGISFilterParameters*>::iterator iterFilter = this->filterParameters->begin(); iterFilter != this->filterParameters->end(); ++iterFilter)
                {
                    delete *iterFilter;
                }
                delete this->filterParameters;
            }
            catch(rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
        }
        else if(option == RSGISExeFilterImages::nldenoising)
        {
            GDALAllRegister();
            GDALDataset **dataset = NULL;
            try
            {
                dataset = new GDALDataset*[1];
                std::cout << this->inputImage << std::endl;
                dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                if(dataset[0] == NULL)
                {
                    std::string message = std::string("Could not open image ") + this->inputImage;
                    throw rsgis::RSGISImageException(message.c_str());
                }

                this->aPar = 2;
                this->hPar = 2;

                rsgis::filter::RSGISApplyNonLocalDenoising *dnFilter = new rsgis::filter::RSGISApplyNonLocalDenoising();
                dnFilter->ApplyFilter(dataset, 1, this->outputImageBase, 5, 256, this->aPar, this->hPar, this->imageFormat, this->outDataType);

                GDALClose(dataset[0]);
                delete dnFilter;
                delete[] dataset;
            }
            catch(rsgis::RSGISException e)
            {
                throw e;
            }
        }
        else if(option == RSGISExeFilterImages::exportfilterbank)
        {
            std::cout << "NOT IMPLEMENTED YET! - well interface not written!:)";
        }
        else
        {
            std::cout << "Options not recognised\n";
        }
    }
}


void RSGISExeFilterImages::printParameters()
{
    if(parsed)
    {
        if(option == RSGISExeFilterImages::filter)
        {
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image Base: " << this->outputImageBase << std::endl;
        }
        else if(option == RSGISExeFilterImages::exportfilterbank)
        {
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image Base: " << this->outputImageBase << std::endl;
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

void RSGISExeFilterImages::help()
{
    std::cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << std::endl;
    std::cout << "\t<rsgis:command algor=\"imagefilter\" option=\"filter\" image=\"image.env\" output=\"output_image_base\" >\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"GaussianSmooth\" fileending=\"gausmooth\" stddevX=\"float\" stddevY=\"float\" angle=\"int\"/>\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Gaussian1st\" fileending=\"gau1st\" stddevX=\"float\" stddevY=\"float\" angle=\"int\"/>\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Gaussian2nd\" fileending=\"gau2nd\" stddevX=\"float\" stddevY=\"float\" angle=\"int\"/>\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Laplacian\" fileending=\"laplacian\" stddev=\"float\"/>\n";
    std::cout << "\t\t<rsgis:filter type=\"Sobel\" fileending=\"sobel\" option=\"x | y | xy\"/>\n";
    std::cout << "\t\t<rsgis:filter type=\"Prewitt\" fileending=\"prewitt\" option=\"x | y | xy\"/>\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Mean\" fileending=\"mean\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Median\" fileending=\"median\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Mode\" fileending=\"mode\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Range\" fileending=\"range\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"StdDev\" fileending=\"stddev\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Min\" fileending=\"min\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Max\" fileending=\"max\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Total\" fileending=\"total\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Kuwahara\" fileending=\"kuwahara\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Lee\" fileending=\"lee\" />\n";
    std::cout << "\t</rsgis:command>\n";
    std::cout << "\t<rsgis:command algor=\"imagefilter\" option=\"filter\" image=\"image.env\" output=\"output_image_base\" filterbank=\"LM\"/>\n";
    std::cout << "\t<rsgis:command algor=\"imagefilter\" option=\"exportfilterbank\" output=\"output_image_base\" >\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"GaussianSmooth\" fileending=\"gausmooth\" stddevX=\"float\" stddevY=\"float\" angle=\"int\"/>\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Gaussian1st\" fileending=\"gau1st\" stddevX=\"float\" stddevY=\"float\" angle=\"int\"/>\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Gaussian2nd\" fileending=\"gau2nd\" stddevX=\"float\" stddevY=\"float\" angle=\"int\"/>\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Laplacian\" fileending=\"laplacian\" stddev=\"float\"/>\n";
    std::cout << "\t\t<rsgis:filter type=\"Sobel\" fileending=\"sobel\" option=\"x | y | xy\"/>\n";
    std::cout << "\t\t<rsgis:filter type=\"Prewitt\" fileending=\"prewitt\" option=\"x | y | xy\"/>\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Mean\" fileending=\"mean\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Median\" fileending=\"median\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Mode\" fileending=\"mode\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Range\" fileending=\"range\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"StdDev\" fileending=\"stddev\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Min\" fileending=\"min\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Max\" fileending=\"max\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Total\" fileending=\"total\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Kuwahara\" fileending=\"kuwahara\" />\n";
    std::cout << "\t\t<rsgis:filter size=\"int\" type=\"Lee\" fileending=\"lee\" />\n";
    std::cout << "\t</rsgis:command>\n";
    std::cout << "\t<rsgis:command algor=\"imagefilter\" option=\"exportfilterbank\" output=\"output_image_base\" filterbank=\"LM\"/>\n";
    std::cout << "</rsgis:commands>\n";
}

std::string RSGISExeFilterImages::getDescription()
{
    return "Provides access to the image filtering functionality available within the library.";
}

std::string RSGISExeFilterImages::getXMLSchema()
{
    return "NOT DONE!";
}

RSGISExeFilterImages::~RSGISExeFilterImages()
{

}

}

