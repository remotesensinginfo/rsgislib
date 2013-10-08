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

namespace rsgisexe{

RSGISExeStackBands::RSGISExeStackBands() : RSGISAlgorithmParameters()
{
	this->algorithm = "stackbands";
	this->inputImages = NULL;
	this->outputImage = "";
	this->numImages = 0;
    this->imageBandNames = NULL;
    this->createVRT = false; // Create GDAL virtual raster
	this->useBandNames = false;
    this->imageFormat = "ENVI";
    this->outDataType = GDT_Float32;
    this->skipValue = 0;
    this->skipPixels = false;
}

RSGISAlgorithmParameters* RSGISExeStackBands::getInstance()
{
	return new RSGISExeStackBands();
}

void RSGISExeStackBands::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	rsgis::utils::RSGISFileUtils fileUtils;
	rsgis::math::RSGISMathsUtils mathsUtils;
	
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *optionDIR = XMLString::transcode("dir");
	XMLCh *optionIMGS = XMLString::transcode("imgs");
    
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
    
    // Set output image fomat (defaults to ENVI)
	XMLCh *formatXMLStr = XMLString::transcode("format");
	if(argElement->hasAttribute(formatXMLStr))
	{
		if(this->createVRT){cout << "\'format\' not supported for GDAL VRT" << endl;}
        char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
		this->imageFormat = string(charValue);
		XMLString::release(&charValue);
	}
	XMLString::release(&formatXMLStr);

    // Set output data type (defaults to Float32)
	XMLCh *datatypeXMLStr = XMLString::transcode("datatype");
	if(argElement->hasAttribute(datatypeXMLStr))
	{
        if(this->createVRT){cout << "\'datatype\' not supported for GDAL VRT" << endl;}
        XMLCh *dtByte = XMLString::transcode("Byte");
        XMLCh *dtUInt16 = XMLString::transcode("UInt16");
        XMLCh *dtInt16 = XMLString::transcode("Int16");
        XMLCh *dtUInt32 = XMLString::transcode("UInt32");
        XMLCh *dtInt32 = XMLString::transcode("Int32");
        XMLCh *dtFloat32 = XMLString::transcode("Float32");
        XMLCh *dtFloat64 = XMLString::transcode("Float64");
        
        const XMLCh *dtXMLValue = argElement->getAttribute(datatypeXMLStr);
        if(XMLString::equals(dtByte, dtXMLValue))
        {
            this->outDataType = GDT_Byte;
        }
        else if(XMLString::equals(dtUInt16, dtXMLValue))
        {
            this->outDataType = GDT_UInt16;
        }
        else if(XMLString::equals(dtInt16, dtXMLValue))
        {
            this->outDataType = GDT_Int16;
        }
        else if(XMLString::equals(dtUInt32, dtXMLValue))
        {
            this->outDataType = GDT_UInt32;
        }
        else if(XMLString::equals(dtInt32, dtXMLValue))
        {
            this->outDataType = GDT_Int32;
        }
        else if(XMLString::equals(dtFloat32, dtXMLValue))
        {
            this->outDataType = GDT_Float32;
        }
        else if(XMLString::equals(dtFloat64, dtXMLValue))
        {
            this->outDataType = GDT_Float64;
        }
        else
        {
            this->outDataType = GDT_Float32;
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
    
    this->replaceBandNames = false;
    XMLCh *replaceBandNamesXMLStr = XMLString::transcode("replacebandnames");
	if(argElement->hasAttribute(replaceBandNamesXMLStr))
	{
		XMLCh *yesStr = XMLString::transcode("yes");
		const XMLCh *vrtValue = argElement->getAttribute(replaceBandNamesXMLStr);
		if(XMLString::equals(vrtValue, yesStr))
		{
			this->replaceBandNames = true;
			cout << "\t Replacing band names." << endl;
		}
		XMLString::release(&yesStr);
	}
	XMLString::release(&replaceBandNamesXMLStr);
    
    // Set value in any band to skip, if non is set standard algoritm is used and no error is printed
    XMLCh *skipValueXMLStr = XMLString::transcode("skipValue");
    if(argElement->hasAttribute(skipValueXMLStr))
    {
        char *charValue = XMLString::transcode(argElement->getAttribute(skipValueXMLStr));
        this->skipValue = mathsUtils.strtofloat(string(charValue));
        cout << "\tSkipping pixels with a value of " << this->skipValue << ", in any band." << endl;
        this->skipPixels = true;
        XMLString::release(&charValue);
    }
    XMLString::release(&skipValueXMLStr);
    
    // Get no data value
    XMLCh *nodataXMLStr = XMLString::transcode("nodata");
    if(argElement->hasAttribute(nodataXMLStr))
    {
        XMLCh *NaNStr = XMLString::transcode("NaN");
        const XMLCh *noDataValue = argElement->getAttribute(nodataXMLStr);
        if(XMLString::equals(noDataValue, NaNStr))
        {
            const char *val = "NaN";
            this->noDataValue = nan(val);
        }
        else
        {
            char *charValue = XMLString::transcode(argElement->getAttribute(nodataXMLStr));
            this->noDataValue = mathsUtils.strtofloat(string(charValue));
            XMLString::release(&charValue);
        }
        XMLString::release(&NaNStr);
    }
    else
    {
        if(this->skipPixels)
        {
            cout << "\tUsing default of 0 for background values" << endl;
        }
        this->noDataValue = 0;
    }
    XMLString::release(&nodataXMLStr);

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
			rsgis::math::RSGISMathsUtils mathsUtils;
            
            cout << "There are " << this->numImages << " images to stack\n";
			
			string gdalCommand = "gdalbuildvrt -separate -overwrite ";
            if (this->skipPixels) 
            {
                gdalCommand = gdalCommand + " -srcnodata " + mathsUtils.doubletostring(this->skipValue).c_str() + " -vrtnodata " + mathsUtils.doubletostring(this->noDataValue).c_str() + " ";
            }
            gdalCommand = gdalCommand + this->outputImage + ".vrt";
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
            try
			{
                rsgis::cmds::executeStackImageBands(this->inputImages, this->imageBandNames, this->numImages, this->outputImage, this->skipPixels, this->skipValue, this->noDataValue, this->imageFormat, rsgis::cmds::GDAL_to_RSGIS_Type(this->outDataType), this->replaceBandNames);
            }
            catch(rsgis::cmds::RSGISCmdException &e)
            {
                throw RSGISException(e.what());
            }
            /*
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
				
				stackbands.stackImages(datasets, this->numImages, this->outputImage, this->imageBandNames, this->skipPixels, this->skipValue, this->noDataValue, this->imageFormat, this->outDataType, this->replaceBandNames);
				
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
             */
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
    
}

