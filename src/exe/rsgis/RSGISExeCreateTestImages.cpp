/*
 *  RSGISExeCreateTestImages.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 06/12/2008.
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

#include "RSGISExeCreateTestImages.h"

namespace rsgisexe{

RSGISExeCreateTestImages::RSGISExeCreateTestImages() : RSGISAlgorithmParameters()
{
	this->algorithm = "testimages";

	this->outputImage = "";
	this->width = 0;
	this->height = 0;
	this->option = RSGISExeCreateTestImages::none;
}

RSGISAlgorithmParameters* RSGISExeCreateTestImages::getInstance()
{
	return new RSGISExeCreateTestImages();
}

void RSGISExeCreateTestImages::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;
	
	const XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	const XMLCh *optionRowMajorNumbered = XMLString::transcode("rowmajornumbered");
	
	const XMLCh *algorNameEle = argElement->getAttribute(XMLString::transcode("algor"));
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	
	const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
	this->outputImage = XMLString::transcode(output);
	
	const XMLCh *optionXML = argElement->getAttribute(XMLString::transcode("option"));
	if(XMLString::equals(optionRowMajorNumbered, optionXML))
	{		
		this->option = RSGISExeCreateTestImages::rowmajornumbered;
		
		const XMLCh *widthStr = argElement->getAttribute(XMLString::transcode("width"));
		this->width = mathUtils.strtoint(XMLString::transcode(widthStr));
		
		const XMLCh *heightStr = argElement->getAttribute(XMLString::transcode("height"));
		this->height = mathUtils.strtoint(XMLString::transcode(heightStr));
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeCreateTestImages.");
		throw RSGISXMLArgumentsException(message.c_str());
	}
	
	parsed = true;
}

void RSGISExeCreateTestImages::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		RSGISCreateTestImages createTestImages;
		if(option == RSGISExeCreateTestImages::rowmajornumbered)
		{
			createTestImages.createRowMajorNumberedImage(this->outputImage, this->width, this->height);
		}
		else
		{
			throw RSGISException("Option not recognised");
		}		
	}
}


void RSGISExeCreateTestImages::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeCreateTestImages::rowmajornumbered)
		{
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Image size: [" << this->width << "," << this->height << "]\n";
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

void RSGISExeCreateTestImages::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
	cout << "\t<rsgis:command algor=\"testimages\" option=\"rowmajornumbered\" output=\"image_out.env\" width=\"int\" height=\"int\" />\n";
	cout << "</rsgis:commands>\n";
}

string RSGISExeCreateTestImages::getDescription()
{
	return "Creates basic images used for testing algorithms.";
}

string RSGISExeCreateTestImages::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeCreateTestImages::~RSGISExeCreateTestImages()
{
	
}

}

