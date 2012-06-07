/*
 *  RSGISExeImageConversion.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/12/2008.
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

#include "RSGISExeImageConversion.h"

namespace rsgisexe{

RSGISExeImageConversion::RSGISExeImageConversion() : RSGISAlgorithmParameters()
{
	this->algorithm = "imageconversion";
	this->option = RSGISExeImageConversion::none;
	this->inputImage = "";
	this->outputImage = "";
	this->inputImages = NULL;
	this->outputImages = NULL;
	this->outputMatrix = "";
	this->imageFormat = "";
	this->outputASCII = "";
	this->imageband = 0;
	this->numImages = 0;
}

RSGISAlgorithmParameters* RSGISExeImageConversion::getInstance()
{
	return new RSGISExeImageConversion();
}

void RSGISExeImageConversion::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;
	RSGISFileUtils fileUtils;
	
	const XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	const XMLCh *optionASCIIColumn = XMLString::transcode("asciicolumn");
	const XMLCh *optionFormat = XMLString::transcode("format");
	const XMLCh *optionSplitBands = XMLString::transcode("splitbands");
	const XMLCh *optionImageToMatrix = XMLString::transcode("image2matrix");
	const XMLCh *optionConvertXYZ = XMLString::transcode("convertXYZ");
    XMLCh *projImage = XMLString::transcode("IMAGE");

	
	const XMLCh *algorNameEle = argElement->getAttribute(XMLString::transcode("algor"));
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	
	const XMLCh *optionXML = argElement->getAttribute(XMLString::transcode("option"));
	if(XMLString::equals(optionASCIIColumn, optionXML))
	{		
		this->option = RSGISExeImageConversion::asciicolumn;
		
		const XMLCh *image = argElement->getAttribute(XMLString::transcode("image"));
		this->inputImage = XMLString::transcode(image);
		
		const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
		this->outputASCII = XMLString::transcode(output);

		this->imageband = mathUtils.strtoint(XMLString::transcode(argElement->getAttribute(XMLString::transcode("band"))));
	}
	else if(XMLString::equals(optionFormat, optionXML))
	{		
		this->option = RSGISExeImageConversion::format;
		
		const XMLCh *formatStr = argElement->getAttribute(XMLString::transcode("format"));
		this->imageFormat= XMLString::transcode(formatStr);
		
		if(argElement->hasAttribute(XMLString::transcode("dir")))
		{
			const XMLCh *dir = argElement->getAttribute(XMLString::transcode("dir"));
			string dirStr = XMLString::transcode(dir);
			
			const XMLCh *ext = argElement->getAttribute(XMLString::transcode("ext"));
			string extStr = XMLString::transcode(ext);
			
			const XMLCh *outend = argElement->getAttribute(XMLString::transcode("outend"));
			string outendStr = XMLString::transcode(outend);
			
			const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
			string outputBase = XMLString::transcode(output);
			try
			{
				this->inputImages = fileUtils.getDIRList(dirStr, extStr, &this->numImages, false);
			}
			catch(RSGISException e)
			{
				throw RSGISXMLArgumentsException(e.what());
			}
			this->outputImages = new string[this->numImages];
			for(int i = 0; i < this->numImages; i++)
			{
				outputImages[i] = outputBase +  fileUtils.getFileNameNoExtension(inputImages[i]) + outendStr;
			}
		}
		else
		{
			this->numImages = 1;
            this->inputImages = new string[1];
            this->outputImages = new string[1];
            XMLCh *imageXMLStr = XMLString::transcode("image");
            if(argElement->hasAttribute(imageXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
                this->inputImages[0] = string(charValue);
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
                this->outputImages[0] = string(charValue);
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
            }
            XMLString::release(&outputXMLStr);
		}
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->projWKTFile = "";
			}
			else
			{
				char *charValue = XMLString::transcode(projXMLValue);
				this->projFromImage = false;
				this->projWKTFile = string(charValue);
			}
		}
		else
		{
			this->projFromImage = true;
            this->projWKTFile = "";
		}
		XMLString::release(&projXMLStr);
		
	}
	else if(XMLString::equals(optionSplitBands, optionXML))
	{		
		this->option = RSGISExeImageConversion::splitbands;
		
		const XMLCh *image = argElement->getAttribute(XMLString::transcode("image"));
		this->inputImage = XMLString::transcode(image);
		
		const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
		this->outputImage = XMLString::transcode(output);
		
		const XMLCh *formatStr = argElement->getAttribute(XMLString::transcode("format"));
		this->imageFormat= XMLString::transcode(formatStr);
	}
	else if(XMLString::equals(optionImageToMatrix, optionXML))
	{		
		this->option = RSGISExeImageConversion::image2matrix;
		const XMLCh *image = argElement->getAttribute(XMLString::transcode("image"));
		this->inputImage = XMLString::transcode(image);
		
		const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
		this->outputMatrix = XMLString::transcode(output);
		
		this->imageband = mathUtils.strtoint(XMLString::transcode(argElement->getAttribute(XMLString::transcode("band"))));
	}
	else if(XMLString::equals(optionConvertXYZ, optionXML))
	{
		this->option = RSGISExeImageConversion::convertXYZ;
		
		XMLCh *inputXMLStr = XMLString::transcode("input");
		if(argElement->hasAttribute(inputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
			this->inputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
		}
		XMLString::release(&inputXMLStr);
		
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
		
		XMLCh *projXMLStr = XMLString::transcode("projWKT");
		if(argElement->hasAttribute(projXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(projXMLStr));
			this->projWKTFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'projWKT\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
		
		XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
		if(argElement->hasAttribute(resolutionXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resolutionXMLStr));
			this->resolution = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided.");
		}
		XMLString::release(&resolutionXMLStr);
		
		XMLCh *delimiterXMLStr = XMLString::transcode("delimiter");
		if(argElement->hasAttribute(delimiterXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(delimiterXMLStr));
			this->delimiter = charValue[0];
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'delimiter\' attribute was provided.");
		}
		XMLString::release(&delimiterXMLStr);
		
		XMLCh *xyOrderXMLStr = XMLString::transcode("xyorder");
		if(argElement->hasAttribute(xyOrderXMLStr))
		{
			XMLCh *xyStr = XMLString::transcode("xy");
			const XMLCh *xyOrderValue = argElement->getAttribute(xyOrderXMLStr);
			
			if(XMLString::equals(xyOrderValue, xyStr))
			{
				this->xyOrder = true;
			}
			else
			{
				this->xyOrder = false;
			}
			XMLString::release(&xyStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'xyorder\' attribute was provided.");
		}
		XMLString::release(&xyOrderXMLStr);		
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeImagfeConversion.");
		throw RSGISXMLArgumentsException(message.c_str());
	}
	
	parsed = true;
}

void RSGISExeImageConversion::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(option == RSGISExeImageConversion::asciicolumn)
		{
			GDALAllRegister();
			GDALDataset *dataset = NULL;
			RSGISImageUtils *imgUtils = NULL;
			
			try
			{
				cout << this->inputImage << endl;
				dataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				imgUtils = new RSGISImageUtils();
				imgUtils->exportImageToTextCol(dataset, this->imageband, this->outputASCII);
				delete imgUtils;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageConversion::format)
		{
			RSGISImageUtils imgUtils;
            RSGISTextUtils txtUtils;
			try
			{
                string wktStr = "";
                if(!this->projFromImage)
                {
                    wktStr = txtUtils.readFileToString(this->projWKTFile);
                    cout << "Define Projection as: " << wktStr << endl;
                }
				
				for(int i = 0; i < this->numImages; i++)
				{
					imgUtils.convertImageFileFormat(this->inputImages[i], this->outputImages[i], this->imageFormat, this->projFromImage, wktStr);
				}
				delete[] inputImages;
				delete[] outputImages;
			}
			catch(RSGISException e)
			{
				cout << "RSGISException: " << e.what() << endl;
			}
			
		}
		else if(option == RSGISExeImageConversion::splitbands)
		{
			RSGISImageUtils *imgUtils = NULL;
			try
			{
				imgUtils = new RSGISImageUtils();
				imgUtils->exportImageBands(this->inputImage, this->outputImage, this->imageFormat);
				delete imgUtils;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageConversion::image2matrix)
		{
			GDALAllRegister();
			GDALDataset **dataset = NULL;
			
			RSGISCalcImageSingleValue *convert2Matrix = NULL;
			RSGISCalcImageSingle *calcConvert2Matrix = NULL;
			
			Matrix *matrix;
			double outputValue = 0;
			
			RSGISMatrices matrixUtils;
			
			try
			{
				dataset = new GDALDataset*[1];
				cout << this->inputImage << endl;
				dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				convert2Matrix = new RSGISImageBand2Matrix(1, this->imageband, dataset[0]->GetRasterXSize(), dataset[0]->GetRasterYSize());
				calcConvert2Matrix = new RSGISCalcImageSingle(convert2Matrix);
				calcConvert2Matrix->calcImage(dataset, 1, &outputValue, this->imageband);
				matrix = dynamic_cast<RSGISImageBand2Matrix*>(convert2Matrix)->getMatrix();
				matrixUtils.saveMatrix2txt(matrix, this->outputMatrix);
				
				GDALClose(dataset[0]);
				delete convert2Matrix;
				delete calcConvert2Matrix;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageConversion::convertXYZ)
		{
			try
			{
				RSGISTextUtils textUtils;
				string wktString = textUtils.readFileToString(projWKTFile);
				RSGISGenerateImageFromXYZData genImgFromXYZ = RSGISGenerateImageFromXYZData(inputFile, outputImage, delimiter, wktString, xyOrder, resolution);
				genImgFromXYZ.createImageRepresentingXYZData();
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else
		{
			cout << "Options not recognised\n";
		}
		
	}
}


void RSGISExeImageConversion::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeImageConversion::asciicolumn)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output ASCII file: " << this->outputASCII << endl;
			cout << "Image band: " << this->imageband << endl;
		}
		else if(option == RSGISExeImageConversion::format)
		{
			for(int i = 0; i < this->numImages; i++)
			{
				cout << "Input Image: " << this->inputImages[i] << endl;
				cout << "Output Image: " << this->outputImages[i] << endl;
			}
			
			cout << "Output Format: " << this->imageFormat << endl;
		}
		else if(option == RSGISExeImageConversion::splitbands)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image Base: " << this->outputImage << endl;
			cout << "Output Format: " << this->imageFormat << endl;
		}
		else if(option == RSGISExeImageConversion::image2matrix)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Matrix: " << this->outputMatrix << endl;
			cout << "Image band: " << this->imageband << endl;
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

void RSGISExeImageConversion::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
	cout << "\t<rsgis:command algor=\"imageconversion\" option=\"asciicolumn\" image=\"image.env\" output=\"ascii_file.txt\" band=\"int\" /> \n";
	cout << "\t<rsgis:command algor=\"imageconversion\" option=\"format\" image=\"image.env\" output=\"image_out.xxx\" format=\"GDAL_NAME\" /> \n";
	cout << "\t<rsgis:command algor=\"imageconversion\" option=\"format\" dir=\"directory\" ext=\"file_extension\" output=\"image_out_base\" format=\"GDAL_NAME\" /> \n";
	cout << "\t<rsgis:command algor=\"imageconversion\" option=\"splitbands\" image=\"image.env\" output=\"image_out_base\" format=\"GDAL_NAME\" /> \n";
	cout << "\t<rsgis:command algor=\"imageconversion\" option=\"image2matrix\" image=\"image.env\" output=\"matrix_out.mtxt\" band=\"int\" /> \n";
	cout << "</rsgis:commands>\n";
}

string RSGISExeImageConversion::getDescription()
{
	return "Image conversion tools.";
}

string RSGISExeImageConversion::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeImageConversion::~RSGISExeImageConversion()
{
	
}
    
}







