/*
 *  RSGISExeRadarUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 22/07/2009.
 *  Copyright 2009 RSGISLib.
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

#include "RSGISExeRadarUtils.h"

namespace rsgisexe{

RSGISExeRadarUtils::RSGISExeRadarUtils() : RSGISAlgorithmParameters()
{
	this->algorithm = "radarUtils";
	this->inputImage = "";
	this->outputImage = "";
	this->calFactor = 0;
	this->option = RSGISExeRadarUtils::none;
}

RSGISAlgorithmParameters* RSGISExeRadarUtils::getInstance()
{
	return new RSGISExeRadarUtils();
}

void RSGISExeRadarUtils::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;
	
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());	
	
	// Options
	XMLCh *typeConvert2dB = XMLString::transcode("convert2dB");
	XMLCh *typeSoilDielectric = XMLString::transcode("soilDielectric");
	XMLCh *typeSoilDielectricList = XMLString::transcode("soilDielectricList");
	
	const XMLCh *algorNameEle = argElement->getAttribute(XMLString::transcode("algor"));
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	
	const XMLCh *optionStr = argElement->getAttribute(XMLString::transcode("option"));
	
	if(XMLString::equals(typeConvert2dB,optionStr)) /// Convert to dB
	{		
		this->option = RSGISExeRadarUtils::convert2dB;
		
		// Retrieve input and output image
		XMLCh *inputXMLStr = XMLString::transcode("input");
		if(argElement->hasAttribute(inputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("Input images not provided..");
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
			throw RSGISXMLArgumentsException("Output image not provided..");
		}
		XMLString::release(&outputXMLStr);
		
		// Get callibration factor
		XMLCh *calFactorStr = XMLString::transcode("calFactor");
		if(argElement->hasAttribute(calFactorStr)) 
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(calFactorStr));
			this->calFactor = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			cout << "\tNot using callibration factor" << endl;
			this->calFactor = 0; // Set callibration factor to 0 (not used)
		}		
		
	}

	else if(XMLString::equals(typeSoilDielectric,optionStr)) /// Calculate soil dielectric
	{		
		this->option = RSGISExeRadarUtils::soilDielectric;

		// Get frequency
		XMLCh *frequencyStr = XMLString::transcode("frequency");
		XMLCh *bandStr = XMLString::transcode("band");
		if(argElement->hasAttribute(frequencyStr)) 
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(frequencyStr));
			this->frequency = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else if(argElement->hasAttribute(bandStr)) 
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bandStr));
			string band = string(charValue);
			XMLString::release(&charValue);
			if(band == "C")
			{
				this->frequency=5.35e9;
				cout << "\tC-band, frequency = " << frequency << " Hz" << endl;
			}
			else if(band == "L")
			{
				this->frequency=1.25e9;
				cout << "\tL-band, frequency = " << frequency << " Hz" << endl;
			}
			else if(band == "P")
			{
				this->frequency=0.4e9;
				cout << "\tP-band, frequency = " << frequency << " Hz" << endl;
			}
			else
			{
				throw RSGISXMLArgumentsException("Radar band not recognised, options are C, L or P");
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No frequency or radar band provided");
		}
		XMLString::release(&frequencyStr);
		XMLString::release(&bandStr);
		// Get temperature
		XMLCh *temperatureStr = XMLString::transcode("temperature");
		if(argElement->hasAttribute(temperatureStr)) 
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(temperatureStr));
			this->temperature = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No temperature provided");
		}
		XMLString::release(&temperatureStr);
		// Get Bulk Density
		XMLCh *bulkDensityStr = XMLString::transcode("bulkDensity");
		if(argElement->hasAttribute(bulkDensityStr)) 
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bulkDensityStr));
			this->bulkDensity = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Bulk Density provided");
		}
		XMLString::release(&bulkDensityStr);
		// Get Volumetric Moisture Content
		XMLCh *volMoistureStr = XMLString::transcode("volMoisture");
		if(argElement->hasAttribute(volMoistureStr)) 
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(volMoistureStr));
			this->volMoisture = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Volumetric Moisture Content provided");
		}
		XMLString::release(&volMoistureStr);
		// Get Clay Percentage
		XMLCh *clayStr = XMLString::transcode("clay");
		if(argElement->hasAttribute(clayStr)) 
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(clayStr));
			this->clay = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Clay Percentage provided");
		}
		XMLString::release(&clayStr);
		// Get Sand Percentage
		XMLCh *sandStr = XMLString::transcode("sand");
		if(argElement->hasAttribute(sandStr)) 
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(sandStr));
			this->sand = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No Sand Percentage provided");
		}
		XMLString::release(&sandStr);
	}
	
	else if(XMLString::equals(typeSoilDielectricList,optionStr)) /// Calculate soil dielectric for a list of parameters
	{
		this->option = RSGISExeRadarUtils::soilDielectricList;
		
		// Retrieve input file
		XMLCh *inputXMLStr = XMLString::transcode("input");
		if(argElement->hasAttribute(inputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
			this->inputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("Input parameter file not provided..");
		}
		XMLString::release(&inputXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("Output file not provided..");
		}
		XMLString::release(&outputXMLStr);
	}
	
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionStr)) + string(") is not known: RSGISExeRadarUtils.");
		throw RSGISXMLArgumentsException(message.c_str());
	}	
	parsed = true;
	
	// Release XML
	XMLString::release(&typeConvert2dB);
	XMLString::release(&typeSoilDielectric);
}

void RSGISExeRadarUtils::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(option == RSGISExeRadarUtils::convert2dB)
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
			
			unsigned int noBands = datasets[0]->GetRasterCount(); // Get number of Raster bands
			
			RSGISCalcImage *calcImg = NULL;
			RSGISConvert2dB *convertdB = NULL;
			
			try
			{
				convertdB = new RSGISConvert2dB(noBands,this->calFactor);
				cout << "calc image" << endl;
				calcImg = new RSGISCalcImage(convertdB, "", true);
				calcImg->calcImage(datasets, 1, this->outputImage);
				cout << "returned to exe\n";
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			if(datasets != NULL)
			{
				GDALClose(&datasets[0]);
				delete[] datasets;
			}
			delete calcImg;
			delete convertdB;
			
		}
		else if(option == RSGISExeRadarUtils::soilDielectric)
		{
			RSGISSoilDielectricMixingModel *calcDielectric = NULL;
			calcDielectric = new RSGISSoilDielectricMixingModel(frequency, sand, clay, temperature, bulkDensity, volMoisture);
			double realDielectric = calcDielectric->calcRealDielectric();
			double imgDielectric = calcDielectric->calcImaginaryDielectric();
			cout << "Soil dielectric = " << realDielectric << " + j" << imgDielectric << endl; 
		}
		else if(option == RSGISExeRadarUtils::soilDielectricList)
		{
			RSGISMatrices matrixUtils;
			
			Matrix *inPar = matrixUtils.readMatrixFromGridTxt(this->inputFile);
			if(inPar->m != 6)
			{
				throw RSGISException("The correct number of parameters was not provided");
			}
			
			Matrix *outPar = matrixUtils.createMatrix(inPar->n, inPar->m+2);
			
			int indexIn = 0;
			int indexOut = 0;
			
			for(int i = 0; i < inPar->n; i++)
			{
				frequency=inPar->matrix[indexIn]; 
				temperature=inPar->matrix[indexIn+1];
				bulkDensity=inPar->matrix[indexIn+2];
				volMoisture=inPar->matrix[indexIn+3];
				clay=inPar->matrix[indexIn+4];
				sand=inPar->matrix[indexIn+5];
				
				RSGISSoilDielectricMixingModel *calcDielectric = NULL;
				calcDielectric = new RSGISSoilDielectricMixingModel(frequency, sand, clay, temperature, bulkDensity, volMoisture);
				double realDielectric = calcDielectric->calcRealDielectric();
				double imgDielectric = calcDielectric->calcImaginaryDielectric();
				outPar->matrix[indexOut] = frequency;
				outPar->matrix[indexOut+1] = temperature;
				outPar->matrix[indexOut+2] = bulkDensity;
				outPar->matrix[indexOut+3] = volMoisture;
				outPar->matrix[indexOut+4] = clay;
				outPar->matrix[indexOut+5] = sand;
				outPar->matrix[indexOut+6] = realDielectric;
				outPar->matrix[indexOut+7] = imgDielectric;
				
				indexIn = indexIn + 6;
				indexOut = indexOut + 8;
				
			}
			
			matrixUtils.saveMatrix2CSV(outPar, outputFile);
			cout << "Output file saved to: " << outputFile << ".csv" << endl;
		}
	}
}


void RSGISExeRadarUtils::printParameters()
{
	cout << "Not yet implemented" << endl;
}

void RSGISExeRadarUtils::help()
{
	cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
	cout << "\t<rsgis:command algor=\"radarUtils\" option=\"convert2dB\" input=\"input image\" output=\"output Image\"\n";
	cout << "\tcalFactor\"Callibration Factor (optional)\"/>\n";
	cout << "\t<rsgis:command algor=\"radarUtils\" option=\"soilDielectric\" band=\"L\" temperature=\"20\"\n";
	cout << "\t\tbulkDensity=\"1.5\" volMoisture=\"0.2\" clay=\"0.2\" sand=\"0.8\"/>\n";
	cout << "\t<rsgis:command algor=\"radarUtils\" option=\"soilDielectric\" frequency=\"1.25e9\" temperature=\"20\"\n";
	cout << "\t\tbulkDensity=\"1.5\" volMoisture=\"0.2\" clay=\"0.2\" sand=\"0.8\"/>\n";
	cout << "\t<rsgis:command algor=\"radarUtils\" option=\"soilDielectricList\" input=\"input file\" output=\"output file\"/>\n";
	cout << "</rsgis:commands>\n";
}

string RSGISExeRadarUtils::getDescription()
{
	return "Estimates parameters from polametric SAR data";
}

string RSGISExeRadarUtils::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeRadarUtils::~RSGISExeRadarUtils()
{
	
}
    
}
