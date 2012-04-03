/*
 *  RSGISExeStackBands.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/12/2008.
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

#include "RSGISExeStackBands.h"

RSGISExeStackBands::RSGISExeStackBands() : RSGISAlgorithmParameters()
{
	this->algorithm = "stackbands";
	this->inputImages = NULL;
	this->outputImage = "";
	this->numImages = 0;
    this->imageBandNames = NULL;
}

RSGISAlgorithmParameters* RSGISExeStackBands::getInstance()
{
	return new RSGISExeStackBands();
}

void RSGISExeStackBands::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISFileUtils fileUtils;
	RSGISMathsUtils mathsUtils;
	
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *optionDIR = XMLString::transcode("dir");
	XMLCh *optionIMGS = XMLString::transcode("imgs");
	
	this->createVRT = false; // Create GDAL virtual raster
	this->useBandNames = false;
	
	const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	
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
	
	XMLCh *createVRTXMLStr = XMLString::transcode("createVRT");
	if(argElement->hasAttribute(createVRTXMLStr))
	{
		XMLCh *yesStr = XMLString::transcode("yes");
		const XMLCh *vrtValue = argElement->getAttribute(createVRTXMLStr);
		if(XMLString::equals(vrtValue, yesStr))
		{
			this->createVRT = true;
			cout << "\tCreating GDAL Virtual Raster Table (VRT) using \'gdalbuildvrt\'" << endl;
		}
		XMLString::release(&yesStr);
	}
	XMLString::release(&createVRTXMLStr);
	
	const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
	if(XMLString::equals(optionDIR, optionXML))
	{		
		string dirStr = "";
		string extStr = "";
		
		XMLCh *dirXMLStr = XMLString::transcode("dir");
		if(argElement->hasAttribute(dirXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(dirXMLStr));
			dirStr = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'dir\' attribute was provided.");
		}
		XMLString::release(&dirXMLStr);
		
		
		XMLCh *extXMLStr = XMLString::transcode("ext");
		if(argElement->hasAttribute(extXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(extXMLStr));
			extStr = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'ext\' attribute was provided.");
		}
		XMLString::release(&extXMLStr);
		
		try
		{
			this->inputImages = fileUtils.getDIRList(dirStr, extStr, &this->numImages, false);
            this->imageBandNames = new string[numImages];
		}
		catch(RSGISException e)
		{
			throw RSGISXMLArgumentsException(e.what());
		}
	}
	else if(XMLString::equals(optionIMGS, optionXML))
	{
		XMLCh *rsgisimageXMLStr = XMLString::transcode("rsgis:image");
		DOMElement *imageElement = NULL;

		DOMNodeList *imagesList = argElement->getElementsByTagName(rsgisimageXMLStr);
		this->numImages = imagesList->getLength();
		this->inputImages = new string[numImages];
		
		XMLCh *fileXMLStr = XMLString::transcode("file");
        XMLCh *nameXMLStr = XMLString::transcode("name");
		bool printVRTNameWarning = false;
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
            
            if(imageElement->hasAttribute(nameXMLStr))
			{
				if ((this->createVRT) && (!printVRTNameWarning)) 
				{
					cout << "\tBand Names not supported for GDAL VRT" << endl;
					printVRTNameWarning = true; // Only need to print warning once.
				}
				if(i == 0) // Create image band names array if name is supplied for first name.
				{
					this->imageBandNames = new string[numImages];
					this->useBandNames = true;
				}
				if(this->useBandNames)
				{
					char *charValue = XMLString::transcode(imageElement->getAttribute(nameXMLStr));
					this->imageBandNames[i] = string(charValue);
					XMLString::release(&charValue);
				}
				else if(i > 0)
				{
					cout << "\tBand names must be set for all bands. None was set for the first band ignoring for subsequent bands" << endl;
				}

			}
			else
			{
				if(i == 0) // Create image band names array if name is supplied for first name.
				{
					this->imageBandNames = NULL;
				}
				else if(this->useBandNames)
				{
					throw RSGISXMLArgumentsException("Band name set for first band but not for band: " + mathsUtils.inttostring(i+1) + ". If used names are required for all bands");
				}
			}
		}
		XMLString::release(&fileXMLStr);
        XMLString::release(&nameXMLStr);
		XMLString::release(&rsgisimageXMLStr);
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeStackBands.");
		throw RSGISXMLArgumentsException(message.c_str());
	}
	
	XMLString::release(&algorName);
	XMLString::release(&algorXMLStr);
	XMLString::release(&optionXMLStr);
	XMLString::release(&optionDIR);
	XMLString::release(&optionIMGS);
	
	parsed = true;
}

void RSGISExeStackBands::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if (this->createVRT) 
		{
			cout << "There are " << this->numImages << " images to stack\n";
			
			string gdalCommand = "gdalbuildvrt -separate -overwrite " + this->outputImage + ".vrt";
			for(int i = 0; i < numImages; i++)
			{
				gdalCommand = gdalCommand + " " +  this->inputImages[i];
			}
			if(system(gdalCommand.c_str()))
			{
				throw RSGISException("Command did not execute successfully.");
			}
			
		}
		else 
		{
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISAddBands stackbands;
			try
			{
				cout << "There are " << this->numImages << " images to stack\n";
				datasets = new GDALDataset*[this->numImages];
				for(int i = 0; i < numImages; i++)
				{
					cout << this->inputImages[i] << endl;
					datasets[i] = (GDALDataset *) GDALOpenShared(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
				}
				
				stackbands.stackImages(datasets, this->numImages, this->outputImage, this->imageBandNames);
				
				if(datasets != NULL)
				{
					for(int i = 0; i < numImages; i++)
					{
						if(datasets[i] != NULL)
						{
							GDALClose(datasets[i]);
						}
					}
					delete[] datasets;
				}
				
				delete[] inputImages;
				if (this->useBandNames) 
				{
					delete[] imageBandNames;
				}
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}

	}
}


void RSGISExeStackBands::printParameters()
{
	if(parsed)
	{
		cout << "Output Image: " << this->outputImage << endl;
		cout << "Number of Input Images: " << this->numImages << endl;
		cout << "Input Images: " << endl;
		for(int i = 0; i < this->numImages; i++)
		{
			cout << i << ": " << this->inputImages[i] << endl;
		}
	}
	else
	{
		cout << "The parameters have yet to be parsed\n";
	}
}

void RSGISExeStackBands::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
	cout << "\t<rsgis:command algor=\"stackbands\" option=\"dir\" dir=\"input_DIR\" output=\"outputimage\" ext=\"file_extension\"/>\n";
	cout << "\t<rsgis:command algor=\"stackbands\" option=\"imgs\" output=\"outputimage\">\n";
	cout << "\t\t<rsgis:image file=\"image1\" />\n";
	cout << "\t\t<rsgis:image file=\"image2\" />\n";
	cout << "\t\t<rsgis:image file=\"image3\" />\n";
	cout << "\t\t<rsgis:image file=\"image4\" />\n";
	cout << "\t</rsgis:command>\n";
	cout << "</rsgis:commands>\n";
}

string RSGISExeStackBands::getDescription()
{
	return "Provides an output image of the overlapping area containing all the input bands.";
}

string RSGISExeStackBands::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeStackBands::~RSGISExeStackBands()
{
	
}

