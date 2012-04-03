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


RSGISExeElevationDataTools::RSGISExeElevationDataTools() : RSGISAlgorithmParameters()
{
	this->algorithm = "elevation";
}

RSGISAlgorithmParameters* RSGISExeElevationDataTools::getInstance()
{
	return new RSGISExeElevationDataTools();
}

void RSGISExeElevationDataTools::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
    cout << "parsing within RSGISExeElevationDataTools\n";
	RSGISMathsUtils mathUtils;	
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *optionSlope = XMLString::transcode("slope");
    XMLCh *optionAspect = XMLString::transcode("aspect");
    XMLCh *optionSlopeAspect = XMLString::transcode("slopeaspect");
    XMLCh *optionHillShade = XMLString::transcode("hillshade");
    XMLCh *optionShadowMask = XMLString::transcode("shadowmask");
    XMLCh *optionIncidenceAngle = XMLString::transcode("incidenceangle");
    XMLCh *optionExitanceAngle = XMLString::transcode("exitanceangle");
    XMLCh *optionIncidenceExistanceAngles = XMLString::transcode("incidenceexitanceangles");    
    
	try
	{
		const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
		if(!XMLString::equals(algorName, algorNameEle))
		{
			throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
		}
		
		const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
		if(XMLString::equals(optionSlope, optionXML))
		{
            this->option = slope;
            
            XMLCh *inputXMLStr = XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = string(charValue);
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
            
            XMLCh *bandXMLStr = XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(string(charValue))-1;
                XMLString::release(&charValue);
            }
            else
            {
                imageBand = 0;
            }
            XMLString::release(&bandXMLStr);
            
            XMLCh *outTypeXMLStr = XMLString::transcode("outtype");
            if(argElement->hasAttribute(outTypeXMLStr))
            {
                XMLCh *radiansStr = XMLString::transcode("radians");
                const XMLCh *outTypeValue = argElement->getAttribute(outTypeXMLStr);
                
                if(XMLString::equals(outTypeValue, radiansStr))
                {
                    this->slopeOutputType = 1;
                }
                else
                {
                    this->slopeOutputType = 0;
                }
                XMLString::release(&radiansStr);
            }
            else
            {
                this->slopeOutputType = 0;
            }
            XMLString::release(&outTypeXMLStr);
        }
        else if(XMLString::equals(optionAspect, optionXML))
		{
            this->option = aspect;
            
            XMLCh *inputXMLStr = XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = string(charValue);
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
            
            XMLCh *bandXMLStr = XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(string(charValue))-1;
                XMLString::release(&charValue);
            }
            else
            {
                imageBand = 0;
            }
            XMLString::release(&bandXMLStr);
        }
        else if(XMLString::equals(optionSlopeAspect, optionXML))
		{
            this->option = slopeaspect;
            
            XMLCh *inputXMLStr = XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = string(charValue);
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
            
            XMLCh *bandXMLStr = XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(string(charValue))-1;
                XMLString::release(&charValue);
            }
            else
            {
                imageBand = 0;
            }
            XMLString::release(&bandXMLStr);
        }
        else if(XMLString::equals(optionHillShade, optionXML))
		{
            this->option = hillshade;
            
            XMLCh *inputXMLStr = XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = string(charValue);
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
            
            XMLCh *bandXMLStr = XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(string(charValue))-1;
                XMLString::release(&charValue);
            }
            else
            {
                imageBand = 0;
            }
            XMLString::release(&bandXMLStr);
            
            XMLCh *zenithXMLStr = XMLString::transcode("zenith");
            if(argElement->hasAttribute(zenithXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(zenithXMLStr));
                this->solarZenith = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                this->solarZenith = 45;
            }
            XMLString::release(&zenithXMLStr);
            
            XMLCh *azimuthXMLStr = XMLString::transcode("azimuth");
            if(argElement->hasAttribute(azimuthXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(azimuthXMLStr));
                this->solarAzimuth = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                this->solarAzimuth = 315;
            }
            XMLString::release(&azimuthXMLStr);
        }
        else if(XMLString::equals(optionShadowMask, optionXML))
		{
            this->option = shadowmask;
            
            XMLCh *inputXMLStr = XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = string(charValue);
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
            
            XMLCh *bandXMLStr = XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                imageBand = 1;
            }
            XMLString::release(&bandXMLStr);
            
            XMLCh *zenithXMLStr = XMLString::transcode("zenith");
            if(argElement->hasAttribute(zenithXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(zenithXMLStr));
                this->solarZenith = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'zenith\' attribute was provided.");
            }
            XMLString::release(&zenithXMLStr);
            
            XMLCh *azimuthXMLStr = XMLString::transcode("azimuth");
            if(argElement->hasAttribute(azimuthXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(azimuthXMLStr));
                this->solarAzimuth = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'azimuth\' attribute was provided.");
            }
            XMLString::release(&azimuthXMLStr);

            XMLCh *maxElevationXMLStr = XMLString::transcode("maxelevation");
            if(argElement->hasAttribute(maxElevationXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(maxElevationXMLStr));
                this->maxElevHeight = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                cerr << "Warning: A default maximum elevation value of 4000 m is being used.";
                this->maxElevHeight = 4000;
            }
            XMLString::release(&maxElevationXMLStr);

        }
        else if(XMLString::equals(optionIncidenceAngle, optionXML))
        {
            this->option = incidenceangle;
            
            XMLCh *inputXMLStr = XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = string(charValue);
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
            
            XMLCh *bandXMLStr = XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(string(charValue))-1;
                XMLString::release(&charValue);
            }
            else
            {
                imageBand = 0;
            }
            XMLString::release(&bandXMLStr);
            
            XMLCh *zenithXMLStr = XMLString::transcode("zenith");
            if(argElement->hasAttribute(zenithXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(zenithXMLStr));
                this->solarZenith = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'zenith\' attribute was provided.");
            }
            XMLString::release(&zenithXMLStr);
            
            XMLCh *azimuthXMLStr = XMLString::transcode("azimuth");
            if(argElement->hasAttribute(azimuthXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(azimuthXMLStr));
                this->solarAzimuth = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'azimuth\' attribute was provided.");
            }
            XMLString::release(&azimuthXMLStr);
        }
        else if(XMLString::equals(optionExitanceAngle, optionXML))
        {
            this->option = exitanceangle;
            
            XMLCh *inputXMLStr = XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = string(charValue);
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
            
            XMLCh *bandXMLStr = XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(string(charValue))-1;
                XMLString::release(&charValue);
            }
            else
            {
                imageBand = 0;
            }
            XMLString::release(&bandXMLStr);
            
            XMLCh *zenithXMLStr = XMLString::transcode("zenith");
            if(argElement->hasAttribute(zenithXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(zenithXMLStr));
                this->viewZenith = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'zenith\' attribute was provided.");
            }
            XMLString::release(&zenithXMLStr);
            
            XMLCh *azimuthXMLStr = XMLString::transcode("azimuth");
            if(argElement->hasAttribute(azimuthXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(azimuthXMLStr));
                this->viewAzimuth = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'azimuth\' attribute was provided.");
            }
            XMLString::release(&azimuthXMLStr);
        }
        else if(XMLString::equals(optionIncidenceExistanceAngles, optionXML))
        {
            this->option = incidenceexistanceangles;
            
            XMLCh *inputXMLStr = XMLString::transcode("input");
            if(argElement->hasAttribute(inputXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
                this->inputImage = string(charValue);
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
            
            XMLCh *bandXMLStr = XMLString::transcode("band");
            if(argElement->hasAttribute(bandXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(bandXMLStr));
                imageBand = mathUtils.strtounsignedint(string(charValue))-1;
                XMLString::release(&charValue);
            }
            else
            {
                imageBand = 0;
            }
            XMLString::release(&bandXMLStr);
            
            XMLCh *solarZenithXMLStr = XMLString::transcode("solarZenith");
            if(argElement->hasAttribute(solarZenithXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(solarZenithXMLStr));
                this->solarZenith = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'solarZenith\' attribute was provided.");
            }
            XMLString::release(&solarZenithXMLStr);
            
            XMLCh *solarAzimuthXMLStr = XMLString::transcode("solarAzimuth");
            if(argElement->hasAttribute(solarAzimuthXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(solarAzimuthXMLStr));
                this->solarAzimuth = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'solarAzimuth\' attribute was provided.");
            }
            XMLString::release(&solarAzimuthXMLStr);
            
            XMLCh *viewZenithXMLStr = XMLString::transcode("viewZenith");
            if(argElement->hasAttribute(viewZenithXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(viewZenithXMLStr));
                this->viewZenith = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'viewZenith\' attribute was provided.");
            }
            XMLString::release(&viewZenithXMLStr);
            
            XMLCh *viewAzimuthXMLStr = XMLString::transcode("viewAzimuth");
            if(argElement->hasAttribute(viewAzimuthXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(viewAzimuthXMLStr));
                this->viewAzimuth = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'viewAzimuth\' attribute was provided.");
            }
            XMLString::release(&viewAzimuthXMLStr);
        }
		else 
		{
			string message = "RSGISExeElevationDataTools did not recognise option " + string(XMLString::transcode(optionXML));
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
	XMLString::release(&optionSlope);
    XMLString::release(&optionAspect);
    XMLString::release(&optionSlopeAspect);
    XMLString::release(&optionHillShade);
    XMLString::release(&optionShadowMask);
    XMLString::release(&optionIncidenceAngle);
    XMLString::release(&optionExitanceAngle);
    XMLString::release(&optionIncidenceExistanceAngles);
	
	parsed = true; // if all successful, it is parsed
}

void RSGISExeElevationDataTools::runAlgorithm() throw(RSGISException)
{
	cout.precision(10);
	
	if(!parsed)
	{
		throw RSGISException("Before running the parameters must be retrieved");
	}
	else
	{
		if(this->option == RSGISExeElevationDataTools::slope)
        {
            cout << "This command calculates the slope from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand+1 << endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcSlope *calcSlope = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				cout << "Open " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand >= numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw RSGISException("The specified image band is not within the input image.");
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
                
				calcSlope = new RSGISCalcSlope(1, imageBand, imageEWRes, imageNSRes, slopeOutputType);
				
				calcImage = new RSGISCalcImage(calcSlope, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3);

				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcSlope;
				delete calcImage;
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::aspect)
        {
            cout << "This command calculates the aspect from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand+1 << endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcAspect *calcAspect = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				cout << "Open " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand >= numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw RSGISException("The specified image band is not within the input image.");
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
                
				calcAspect = new RSGISCalcAspect(1, imageBand, imageEWRes, imageNSRes);
				
				calcImage = new RSGISCalcImage(calcAspect, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3);
                
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcAspect;
				delete calcImage;
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::slopeaspect)
        {
            cout << "This command calculates the slope and aspect from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand << endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcSlopeAspect *calcSlopeAspect = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				cout << "Open " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand >= numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw RSGISException("The specified image band is not within the input image.");
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
                
				calcSlopeAspect = new RSGISCalcSlopeAspect(2, imageBand, imageEWRes, imageNSRes);
				
				calcImage = new RSGISCalcImage(calcSlopeAspect, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3);
                
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcSlopeAspect;
				delete calcImage;
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::hillshade)
        {
            cout << "This command generates a hill shade image from the DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand+1 << endl;
            cout << "Solar Azimuth: " << this->solarAzimuth << endl;
            cout << "Solar Zenith: " << this->solarZenith << endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcHillShade *calcHillShade = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				cout << "Open " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand >= numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw RSGISException("The specified image band is not within the input image.");
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
                
				calcHillShade = new RSGISCalcHillShade(1, imageBand, imageEWRes, imageNSRes, solarZenith, solarAzimuth);
				
				calcImage = new RSGISCalcImage(calcHillShade, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3);
                
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcHillShade;
				delete calcImage;
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::shadowmask)
        {
            cout << "This command calculates a shadow mask from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand+1 << endl;
            cout << "Solar Azimuth: " << this->solarAzimuth << endl;
            cout << "Solar Zenith: " << this->solarZenith << endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcShadowBinaryMask *calcMaskShadow = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			try
			{
                if((this->solarZenith < 0) | (this->solarZenith > 90))
                {
                    throw RSGISException("The solar zenith should be between 0 and 90 degrees.");
                }
                
                if((this->solarAzimuth < 0) | (this->solarAzimuth > 360))
                {
                    throw RSGISException("The solar azimuth should be between 0 and 360 degrees.");
                }
                
                this->solarZenith = 90 - this->solarZenith;
                
				datasets = new GDALDataset*[1];
				
				cout << "Open " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if((this->imageBand > numRasterBands) | (this->imageBand == 0))
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw RSGISException("The specified image band is not within the input image.");
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
                
				calcMaskShadow = new RSGISCalcShadowBinaryMask(1, datasets[0], imageBand, imageEWRes, imageNSRes, solarZenith, solarAzimuth, maxElevHeight);
				
				calcImage = new RSGISCalcImage(calcMaskShadow, "", true);
				calcImage->calcImageExtent(datasets, 1, outputImage);
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcMaskShadow;
				delete calcImage;
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::incidenceangle)
        {
            cout << "This command calculate the incidence angle of light (from the provided zenith and azimuth) from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand+1 << endl;
            cout << "Solar Azimuth: " << this->solarAzimuth << endl;
            cout << "Solar Zenith: " << this->solarZenith << endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcRayIncidentAngle *calcIncidence = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				cout << "Open " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand >= numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw RSGISException("The specified image band is not within the input image.");
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
                
				calcIncidence = new RSGISCalcRayIncidentAngle(1, imageBand, imageEWRes, imageNSRes, solarZenith, solarAzimuth);
				
				calcImage = new RSGISCalcImage(calcIncidence, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3);
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcIncidence;
				delete calcImage;
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::exitanceangle)
        {
            cout << "This command calculate the exitance angle of light (from the provided zenith and azimuth) from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand+1 << endl;
            cout << "View Azimuth: " << this->viewAzimuth << endl;
            cout << "View Zenith: " << this->viewZenith << endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcRayExitanceAngle *calcIncidence = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				cout << "Open " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand >= numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw RSGISException("The specified image band is not within the input image.");
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
                
				calcIncidence = new RSGISCalcRayExitanceAngle(1, imageBand, imageEWRes, imageNSRes, solarZenith, solarAzimuth);
				
				calcImage = new RSGISCalcImage(calcIncidence, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3);
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcIncidence;
				delete calcImage;
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeElevationDataTools::incidenceexistanceangles)
        {
            cout << "This command calculate the incidence angle of light (from the provided zenith and azimuth) from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand+1 << endl;
            cout << "Solar Azimuth: " << this->solarAzimuth << endl;
            cout << "Solar Zenith: " << this->solarZenith << endl;
            cout << "View Azimuth: " << this->viewAzimuth << endl;
            cout << "View Zenith: " << this->viewZenith << endl;
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcRayIncidentAndExitanceAngles *calcIncidence = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[1];
				
				cout << "Open " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				int numRasterBands = datasets[0]->GetRasterCount();
                if(this->imageBand >= numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw RSGISException("The specified image band is not within the input image.");
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
                
				calcIncidence = new RSGISCalcRayIncidentAndExitanceAngles(2, imageBand, imageEWRes, imageNSRes, solarZenith, solarAzimuth, viewZenith, viewAzimuth);
				
				calcImage = new RSGISCalcImage(calcIncidence, "", true);
				calcImage->calcImageWindowData(datasets, 1, this->outputImage, 3);
				
				GDALClose(datasets[0]);
				delete[] datasets;
                
				delete calcIncidence;
				delete calcImage;
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
		else
		{
			throw RSGISException("RSGISExeElevationDataTools does not know this option");
		}
	}
}


void RSGISExeElevationDataTools::printParameters()
{
	cout.precision(10);
	
	if(!parsed)
	{
		throw RSGISException("Before running the parameters must be retrieved");
	}
	else
	{
		if(this->option == RSGISExeElevationDataTools::slope)
        {
            cout << "This command calculates the slope from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand << endl;
        }
        else if(this->option == RSGISExeElevationDataTools::aspect)
        {
            cout << "This command calculates the aspect from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand << endl;
        }
        else if(this->option == RSGISExeElevationDataTools::slopeaspect)
        {
            cout << "This command calculates the slope and aspect from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand << endl;
        }
        else if(this->option == RSGISExeElevationDataTools::hillshade)
        {
            cout << "This command generates a hill shade imnage from the DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand << endl;
            cout << "Solar Azimuth: " << this->solarAzimuth << endl;
            cout << "Solar Zenith: " << this->solarZenith << endl;
        }
        else if(this->option == RSGISExeElevationDataTools::shadowmask)
        {
            cout << "This command calculates a shadow mask from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand << endl;
            cout << "Solar Azimuth: " << this->solarAzimuth << endl;
            cout << "Solar Zenith: " << this->solarZenith << endl;
        }
        else if(this->option == RSGISExeElevationDataTools::incidenceangle)
        {
            cout << "This command calculate the incidence angle of light (from the provided zenith and azimuth) from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand+1 << endl;
            cout << "Solar Azimuth: " << this->solarAzimuth << endl;
            cout << "Solar Zenith: " << this->solarZenith << endl;
        }
        else if(this->option == RSGISExeElevationDataTools::exitanceangle)
        {
            cout << "This command calculate the exitance angle of light (from the provided zenith and azimuth) from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand+1 << endl;
            cout << "View Azimuth: " << this->viewAzimuth << endl;
            cout << "View Zenith: " << this->viewZenith << endl;
        }
        else if(this->option == RSGISExeElevationDataTools::incidenceexistanceangles)
        {
            cout << "This command calculate the incidence angle of light (from the provided zenith and azimuth) from a DEM.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Input Band: " << this->imageBand+1 << endl;
            cout << "Solar Azimuth: " << this->solarAzimuth << endl;
            cout << "Solar Zenith: " << this->solarZenith << endl;
            cout << "View Azimuth: " << this->viewAzimuth << endl;
            cout << "View Zenith: " << this->viewZenith << endl;
        }
		else
		{
			throw RSGISException("RSGISExeImageCalibration does not know this option");
		}
	}
}

void RSGISExeElevationDataTools::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
	cout << "<!-- A command to calculate the slope of an elevation surface -->" << endl;
    cout << "<rsgis:command algor=\"elevation\" option=\"slope\" input=\"image.env\" output=\"out_image.env\" [band=\"int\"] [outtype=\"radians|degrees\"] />" << endl;
    cout << "<!-- A command to calculate the aspect of an elevation surface -->" << endl;
    cout << "<rsgis:command algor=\"elevation\" option=\"aspect\" input=\"image.env\" output=\"out_image.env\" [band=\"int\"] />" << endl;
    cout << "<!-- A command to calculate the slope and aspect of an elevation surface -->" << endl;
    cout << "<rsgis:command algor=\"elevation\" option=\"slopeaspect\" input=\"image.env\" output=\"out_image.env\" [band=\"int\"] />" << endl;
    cout << "<!-- A command to calculate the hillshade of an elevation surface -->" << endl;
    cout << "<rsgis:command algor=\"elevation\" option=\"hillshade\" input=\"image.env\" output=\"out_image.env\" azimuth=\"float\" zenith=\"float\" [band=\"int\"]  />" << endl;
    cout << "<!-- A command to calculate a mask for the regions of shadow from an elevation surface -->" << endl;
    cout << "<rsgis:command algor=\"elevation\" option=\"shadowmask\" input=\"image.env\" output=\"out_image.env\" azimuth=\"float\" zenith=\"float\" maxelevation=\"int\" [band=\"int\"]  />" << endl;
    cout << "<!-- A command to calculate the incidence angle from a elevation surface and sun position -->" << endl;
    cout << "<rsgis:command algor=\"elevation\" option=\"incidenceangle\" input=\"image.env\" output=\"out_image.env\" azimuth=\"float\" zenith=\"float\" [band=\"int\"]  />" << endl;
    cout << "<!-- A command to calculate the exitance angle from a elevation surface and viewer position -->" << endl;
    cout << "<rsgis:command algor=\"elevation\" option=\"exitanceangle\" input=\"image.env\" output=\"out_image.env\" azimuth=\"float\" zenith=\"float\" [band=\"int\"]  />" << endl;
    cout << "<!-- A command to calculate the incidence and exitance angles from a elevation surface and sun and viewer positions -->" << endl;
    cout << "<rsgis:command algor=\"elevation\" option=\"incidenceexitanceangles\" input=\"image.env\" output=\"out_image.env\" solarAzimuth=\"float\" solarZenith=\"float\" viewAzimuth=\"float\" viewZenith=\"float\" [band=\"int\"]  />" << endl;

	cout << "</rsgis:commands>\n";
}

string RSGISExeElevationDataTools::getDescription()
{
	return "Provides a set of tools for processsing elevation (DEMs) data.";
}

string RSGISExeElevationDataTools::getXMLSchema()
{
	return "NOT PROVIDED!";
}

RSGISExeElevationDataTools::~RSGISExeElevationDataTools()
{
	
}



