/*
 *  RSGISExeImageCalibration.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/05/2011.
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

#include "RSGISExeImageCalibration.h"

namespace rsgisexe{

RSGISExeImageCalibration::RSGISExeImageCalibration() : rsgis::RSGISAlgorithmParameters()
{
	this->algorithm = "imagecalibration";
}

rsgis::RSGISAlgorithmParameters* RSGISExeImageCalibration::getInstance()
{
	return new RSGISExeImageCalibration();
}

void RSGISExeImageCalibration::retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException)
{
	rsgis::math::RSGISMathsUtils mathUtils;
	XMLCh *algorName = xercesc::XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = xercesc::XMLString::transcode("algor");
	XMLCh *optionXMLStr = xercesc::XMLString::transcode("option");
	XMLCh *optionLandsatRadCal = xercesc::XMLString::transcode("landsatradcal");
    XMLCh *optionLandsatRadCalMultiAdd = xercesc::XMLString::transcode("landsatradcalmultiadd");
    XMLCh *optionTopAtmosRefl = xercesc::XMLString::transcode("topatmosrefl");
    XMLCh *optionSPOTRadCal = xercesc::XMLString::transcode("spotradcal");
    XMLCh *optionIkonosRadCal = xercesc::XMLString::transcode("ikonosradcal");
    XMLCh *optionASTERRadCal = xercesc::XMLString::transcode("asterradcal");
    XMLCh *optionIRSRadCal = xercesc::XMLString::transcode("irsradcal");
    XMLCh *optionQuickbird16bitRadCal = xercesc::XMLString::transcode("quickbird16bitradcal");
    XMLCh *optionQuickbird8bitRadCal = xercesc::XMLString::transcode("quickbird8bitradcal");
    XMLCh *optionWorldView2RadCal = xercesc::XMLString::transcode("worldview2radcal");
    XMLCh *optionApply6SSingle = xercesc::XMLString::transcode("apply6sSingle");
    XMLCh *optionApply6S = xercesc::XMLString::transcode("apply6s");
    
	try
	{
		const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
		if(!xercesc::XMLString::equals(algorName, algorNameEle))
		{
			throw rsgis::RSGISXMLArgumentsException("The algorithm name is incorrect.");
		}
        
        // Set output image format (defaults to ENVI)
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
		if(xercesc::XMLString::equals(optionLandsatRadCal, optionXML))
		{
            this->option = landsatradcal;
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

            
            xercesc::DOMNodeList *bandsNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            std::cout << "Found " << this->numBands << " Bands listed.\n";
            
            xercesc::DOMElement *bandElement = NULL;
            
            landsatRadGainOffs.reserve(numBands);
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandsNodesList->item(i));
                
                rsgis::cmds::CmdsLandsatRadianceGainsOffsets radVals;
                
                XMLCh *bandNameXMLStr = xercesc::XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    radVals.bandName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = xercesc::XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    radVals.imagePath = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = xercesc::XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    radVals.band = mathUtils.strtounsignedint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = xercesc::XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    std::string sensorBand = std::string(charValue);
                    if(sensorBand == "LMSS1_B1")
                    {
                        radVals.lMin = 0;
                        radVals.lMax = 248;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS1_B2")
                    {
                        radVals.lMin = 0;
                        radVals.lMax = 200;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS1_B3")
                    {
                        radVals.lMin = 0;
                        radVals.lMax = 176;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS1_B4")
                    {
                        radVals.lMin = 0;
                        radVals.lMax = 153;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS2_B1")
                    {
                        radVals.lMin = 8;
                        radVals.lMax = 263;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS2_B2")
                    {
                        radVals.lMin = 6;
                        radVals.lMax = 176;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS2_B3")
                    {
                        radVals.lMin = 6;
                        radVals.lMax = 152;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS2_B4")
                    {
                        radVals.lMin = 3.66667;
                        radVals.lMax = 130.333;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS3_B1")
                    {
                        radVals.lMin = 4;
                        radVals.lMax = 259;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS3_B2")
                    {
                        radVals.lMin = 3;
                        radVals.lMax = 179;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS3_B3")
                    {
                        radVals.lMin = 3;
                        radVals.lMax = 149;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS3_B4")
                    {
                        radVals.lMin = 1;
                        radVals.lMax = 128;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS4_B1")
                    {
                        radVals.lMin = 4;
                        radVals.lMax = 238;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS4_B2")
                    {
                        radVals.lMin = 4;
                        radVals.lMax = 164;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS4_B3")
                    {
                        radVals.lMin = 5;
                        radVals.lMax = 142;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS4_B4")
                    {
                        radVals.lMin = 4;
                        radVals.lMax = 116;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS5_B1")
                    {
                        radVals.lMin = 3;
                        radVals.lMax = 268;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS5_B2")
                    {
                        radVals.lMin = 3;
                        radVals.lMax = 179;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS5_B3")
                    {
                        radVals.lMin = 5;
                        radVals.lMax = 148;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS5_B4")
                    {
                        radVals.lMin = 3;
                        radVals.lMax = 123;
                        radVals.qCalMin = 0;
                        radVals.qCalMax = 127;
                    }                    
                    else if(sensorBand == "LTM5_B1_Pre1992")
                    {
                        radVals.lMin = -1.52;
                        radVals.lMax = 169;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B1_Post1992")
                    {
                        radVals.lMin = -1.52;
                        radVals.lMax = 169;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B1_Post1992")
                    {
                        radVals.lMin = -1.52;
                        radVals.lMax = 193;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B2_Pre1992")
                    {
                        radVals.lMin = -2.84;
                        radVals.lMax = 333;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B2_Post1992")
                    {
                        radVals.lMin = -2.84;
                        radVals.lMax = 365;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B3")
                    {
                        radVals.lMin = -1.17;
                        radVals.lMax = 264;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B4")
                    {
                        radVals.lMin = -1.51;
                        radVals.lMax = 221;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B5")
                    {
                        radVals.lMin = -0.37;
                        radVals.lMax = 30.2;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B6")
                    {
                        radVals.lMin = 1.238;
                        radVals.lMax = 15.303;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B7")
                    {
                        radVals.lMin = -0.15;
                        radVals.lMax = 16.6;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B1")
                    {
                        radVals.lMin = -6.2;
                        radVals.lMax = 191.6;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B2")
                    {
                        radVals.lMin = -6.4;
                        radVals.lMax = 196.5;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B3")
                    {
                        radVals.lMin = -5;
                        radVals.lMax = 152.9;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B4")
                    {
                        radVals.lMin = -5.1;
                        radVals.lMax = 241.1;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B5")
                    {
                        radVals.lMin = -1;
                        radVals.lMax = 31.06;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B61")
                    {
                        radVals.lMin = 0;
                        radVals.lMax = 17.04;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B62")
                    {
                        radVals.lMin = 3.2;
                        radVals.lMax = 12.65;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B7")
                    {
                        radVals.lMin = -0.35;
                        radVals.lMax = 10.8;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_PAN")
                    {
                        radVals.lMin = -4.7;
                        radVals.lMax = 243.1;
                        radVals.qCalMin = 1;
                        radVals.qCalMax = 255;
                    }
                    else
                    {
                        std::cout << "sensorBand = " << sensorBand << std::endl;
                        throw rsgis::RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    //throw rsgis::RSGISXMLArgumentsException("No \'sensorband\' attribute was provided.");
                    
                    XMLCh *lMinXMLStr = xercesc::XMLString::transcode("lmin");
                    if(bandElement->hasAttribute(lMinXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(lMinXMLStr));
                        radVals.lMin = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'lmin\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&lMinXMLStr);
                    
                    XMLCh *lMaxXMLStr = xercesc::XMLString::transcode("lmax");
                    if(bandElement->hasAttribute(lMaxXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(lMaxXMLStr));
                        radVals.lMax = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'lmax\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&lMaxXMLStr);
                    
                    XMLCh *qCalMinXMLStr = xercesc::XMLString::transcode("qcalmin");
                    if(bandElement->hasAttribute(qCalMinXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(qCalMinXMLStr));
                        radVals.qCalMin = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'qcalmin\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&qCalMinXMLStr);
                    
                    XMLCh *qCalMaxXMLStr = xercesc::XMLString::transcode("qcalmax");
                    if(bandElement->hasAttribute(qCalMaxXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(qCalMaxXMLStr));
                        radVals.qCalMax = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'qcalmax\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&qCalMaxXMLStr);
                }
                xercesc::XMLString::release(&sensorBandXMLStr);
                
                landsatRadGainOffs.push_back(radVals);
            }
		}
        else if(xercesc::XMLString::equals(optionLandsatRadCalMultiAdd, optionXML))
		{
            this->option = landsatradcalmultiadd;
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
            
            
            xercesc::DOMNodeList *bandsNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();
            
            std::cout << "Found " << this->numBands << " Bands listed.\n";
            
            xercesc::DOMElement *bandElement = NULL;
            
            landsatRadGainOffsMultiAdd.reserve(numBands);
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandsNodesList->item(i));
                
                rsgis::cmds::CmdsLandsatRadianceGainsOffsetsMultiAdd radVals;
                
                XMLCh *bandNameXMLStr = xercesc::XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    radVals.bandName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = xercesc::XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    radVals.imagePath = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = xercesc::XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    radVals.band = mathUtils.strtounsignedint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandIdxXMLStr);
                
                XMLCh *addValXMLStr = xercesc::XMLString::transcode("addval");
                if(bandElement->hasAttribute(addValXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(addValXMLStr));
                    radVals.addVal = mathUtils.strtofloat(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'addval\' attribute was provided.");
                }
                xercesc::XMLString::release(&addValXMLStr);
                
                XMLCh *multiValXMLStr = xercesc::XMLString::transcode("multival");
                if(bandElement->hasAttribute(multiValXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(multiValXMLStr));
                    radVals.multiVal = mathUtils.strtofloat(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'multival\' attribute was provided.");
                }
                xercesc::XMLString::release(&multiValXMLStr);
                
                landsatRadGainOffsMultiAdd.push_back(radVals);
            }
		}
        else if(xercesc::XMLString::equals(optionTopAtmosRefl, optionXML))
		{
            this->option = topatmosrefl;
            
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
            
            // Set scaling factor for output images
            this->scaleFactor = 1.0;
            XMLCh *scaleFactorXMLStr = xercesc::XMLString::transcode("scaleFactor");
            if(argElement->hasAttribute(scaleFactorXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(scaleFactorXMLStr));
                this->scaleFactor = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            xercesc::XMLString::release(&scaleFactorXMLStr);
            
            XMLCh *julianDayXMLStr = xercesc::XMLString::transcode("julianday");
            if(argElement->hasAttribute(julianDayXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(julianDayXMLStr));
                this->julianDay = mathUtils.strtounsignedint(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                //throw rsgis::RSGISXMLArgumentsException("No \'julianday\' attribute was provided.");
                unsigned int day = 0;
                unsigned int month = 0;
                unsigned int year = 0;
                XMLCh *dayXMLStr = xercesc::XMLString::transcode("day");
                if(argElement->hasAttribute(dayXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(dayXMLStr));
                    day = mathUtils.strtounsignedint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'day\' attribute was provided.");
                }
                xercesc::XMLString::release(&dayXMLStr);
                
                XMLCh *monthXMLStr = xercesc::XMLString::transcode("month");
                if(argElement->hasAttribute(monthXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(monthXMLStr));
                    month = mathUtils.strtounsignedint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'month\' attribute was provided.");
                }
                xercesc::XMLString::release(&monthXMLStr);
                
                XMLCh *yearXMLStr = xercesc::XMLString::transcode("year");
                if(argElement->hasAttribute(yearXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(yearXMLStr));
                    year = mathUtils.strtounsignedint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'year\' attribute was provided.");
                }
                xercesc::XMLString::release(&yearXMLStr);
                
                
                boost::gregorian::date d(year,month,day);
                julianDay = d.julian_day();
            }
            xercesc::XMLString::release(&julianDayXMLStr);
            
            
            XMLCh *zenithXMLStr = xercesc::XMLString::transcode("zenith");
            if(argElement->hasAttribute(zenithXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(zenithXMLStr));
                this->solarZenith = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elevationXMLStr = xercesc::XMLString::transcode("elevation");
                if(argElement->hasAttribute(elevationXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(elevationXMLStr));
                    this->solarZenith = 90-mathUtils.strtofloat(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'zenith\' or \'elevation\' attribute was provided.");
                }
                xercesc::XMLString::release(&elevationXMLStr);
            }
            xercesc::XMLString::release(&zenithXMLStr);
            
            
            
            xercesc::DOMNodeList *bandsNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            std::cout << "Found " << this->numBands << " Bands listed.\n";
            
            xercesc::DOMElement *bandElement = NULL;
            solarIrradiance = new float[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *sensorBandXMLStr = xercesc::XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    std::string sensorBand = std::string(charValue);
                    if(sensorBand == "LMSS1_B1")
                    {
                        this->solarIrradiance[i] = 1823;
                    }
                    else if(sensorBand == "LMSS1_B2")
                    {
                        this->solarIrradiance[i] = 1559;
                    }
                    else if(sensorBand == "LMSS1_B3")
                    {
                        this->solarIrradiance[i] = 1276;
                    }
                    else if(sensorBand == "LMSS1_B4")
                    {
                        this->solarIrradiance[i] = 880.1;
                    }
                    else if(sensorBand == "LMSS2_B1")
                    {
                        this->solarIrradiance[i] = 1829;
                    }
                    else if(sensorBand == "LMSS2_B2")
                    {
                        this->solarIrradiance[i] = 1539;
                    }
                    else if(sensorBand == "LMSS2_B3")
                    {
                        this->solarIrradiance[i] = 1268;
                    }
                    else if(sensorBand == "LMSS2_B4")
                    {
                        this->solarIrradiance[i] = 886.6;
                    }
                    else if(sensorBand == "LMSS3_B1")
                    {
                        this->solarIrradiance[i] = 1839;
                    }
                    else if(sensorBand == "LMSS3_B2")
                    {
                        this->solarIrradiance[i] = 1555;
                    }
                    else if(sensorBand == "LMSS3_B3")
                    {
                        this->solarIrradiance[i] = 1291;
                    }
                    else if(sensorBand == "LMSS3_B4")
                    {
                        this->solarIrradiance[i] = 887.9;
                    }
                    else if(sensorBand == "LMSS4_B1")
                    {
                        this->solarIrradiance[i] = 1827;
                    }
                    else if(sensorBand == "LMSS4_B2")
                    {
                        this->solarIrradiance[i] = 1569;
                    }
                    else if(sensorBand == "LMSS4_B3")
                    {
                        this->solarIrradiance[i] = 1260;
                    }
                    else if(sensorBand == "LMSS4_B4")
                    {
                        this->solarIrradiance[i] = 866.4;
                    }
                    else if(sensorBand == "LMSS5_B1")
                    {
                        this->solarIrradiance[i] = 1824;
                    }
                    else if(sensorBand == "LMSS5_B2")
                    {
                        this->solarIrradiance[i] = 1570;
                    }
                    else if(sensorBand == "LMSS5_B3")
                    {
                        this->solarIrradiance[i] = 1249;
                    }
                    else if(sensorBand == "LMSS5_B4")
                    {
                        this->solarIrradiance[i] = 853.4;
                    }
                    else if(sensorBand == "LTM4_B1")
                    {
                        this->solarIrradiance[i] = 1983;
                    }
                    else if(sensorBand == "LTM4_B2")
                    {
                        this->solarIrradiance[i] = 1795;
                    }
                    else if(sensorBand == "LTM4_B3")
                    {
                        this->solarIrradiance[i] = 1539;
                    }
                    else if(sensorBand == "LTM4_B4")
                    {
                        this->solarIrradiance[i] = 1028;
                    }
                    else if(sensorBand == "LTM4_B5")
                    {
                        this->solarIrradiance[i] = 219.8;
                    }
                    else if(sensorBand == "LTM4_B7")
                    {
                        this->solarIrradiance[i] = 83.49;
                    }
                    else if(sensorBand == "LTM5_B1")
                    {
                        this->solarIrradiance[i] = 1983;
                    }
                    else if(sensorBand == "LTM5_B2")
                    {
                        this->solarIrradiance[i] = 1796;
                    }
                    else if(sensorBand == "LTM5_B3")
                    {
                        this->solarIrradiance[i] = 1536;
                    }
                    else if(sensorBand == "LTM5_B4")
                    {
                        this->solarIrradiance[i] = 1031;
                    }
                    else if(sensorBand == "LTM5_B5")
                    {
                        this->solarIrradiance[i] = 220.0;
                    }
                    else if(sensorBand == "LTM5_B7")
                    {
                        this->solarIrradiance[i] = 83.44;
                    }
                    else if(sensorBand == "LETM7_B1")
                    {
                        this->solarIrradiance[i] = 1997;
                    }
                    else if(sensorBand == "LETM7_B2")
                    {
                        this->solarIrradiance[i] = 1812;
                    }
                    else if(sensorBand == "LETM7_B3")
                    {
                        this->solarIrradiance[i] = 1533;
                    }
                    else if(sensorBand == "LETM7_B4")
                    {
                        this->solarIrradiance[i] = 1039;
                    }
                    else if(sensorBand == "LETM7_B5")
                    {
                        this->solarIrradiance[i] = 230.8;
                    }
                    else if(sensorBand == "LETM7_B7")
                    {
                        this->solarIrradiance[i] = 84.9;
                    }
                    else if(sensorBand == "LETM7_PAN")
                    {
                        this->solarIrradiance[i] = 1362;
                    }
                    else if(sensorBand == "IK_Pan")
                    {
                        this->solarIrradiance[i] = 1375.8;
                    }
                    else if(sensorBand == "IK_Blue")
                    {
                        this->solarIrradiance[i] = 1930.9;
                    }
                    else if(sensorBand == "IK_Green")
                    {
                        this->solarIrradiance[i] = 1854.8;
                    }
                    else if(sensorBand == "IK_Red")
                    {
                        this->solarIrradiance[i] = 1556.5;
                    }
                    else if(sensorBand == "IK_NIR")
                    {
                        this->solarIrradiance[i] = 1156.9;
                    }
                    else if(sensorBand == "WV2_Pan")
                    {
                        this->solarIrradiance[i] = 1580.8140;
                    }
                    else if(sensorBand == "WV2_Coast")
                    {
                        this->solarIrradiance[i] = 1758.2229;
                    }
                    else if(sensorBand == "WV2_Blue")
                    {
                        this->solarIrradiance[i] = 1974.2416;
                    }
                    else if(sensorBand == "WV2_Green")
                    {
                        this->solarIrradiance[i] = 1856.4104;
                    }
                    else if(sensorBand == "WV2_Yellow")
                    {
                        this->solarIrradiance[i] = 1738.4791;
                    }
                    else if(sensorBand == "WV2_Red")
                    {
                        this->solarIrradiance[i] = 1559.4555;
                    }
                    else if(sensorBand == "WV2_Red_Edge")
                    {
                        this->solarIrradiance[i] = 1342.0695;
                    }
                    else if(sensorBand == "WV2_NIR1")
                    {
                        this->solarIrradiance[i] = 1069.7302;
                    }
                    else if(sensorBand == "WV2_NIR2")
                    {
                        this->solarIrradiance[i] = 861.2866;
                    }
                    else if(sensorBand == "SPOT1_HRV2_Pan")
                    {
                        this->solarIrradiance[i] = 1690;
                    }
                    else if(sensorBand == "SPOT1_HRV2_Green")
                    {
                        this->solarIrradiance[i] = 1845;
                    }
                    else if(sensorBand == "SPOT1_HRV2_Red")
                    {
                        this->solarIrradiance[i] = 1575;
                    }
                    else if(sensorBand == "SPOT1_HRV2_NIR")
                    {
                        this->solarIrradiance[i] = 1040;
                    }
                    else if(sensorBand == "SPOT2_HRV1_Pan")
                    {
                        this->solarIrradiance[i] = 1750;
                    }
                    else if(sensorBand == "SPOT2_HRV1_Green")
                    {
                        this->solarIrradiance[i] = 1865;
                    }
                    else if(sensorBand == "SPOT2_HRV1_Red")
                    {
                        this->solarIrradiance[i] = 1620;
                    }
                    else if(sensorBand == "SPOT2_HRV1_NIR")
                    {
                        this->solarIrradiance[i] = 1085;
                    }
                    else if(sensorBand == "SPOT2_HRV2_Pan")
                    {
                        this->solarIrradiance[i] = 1670;
                    }
                    else if(sensorBand == "SPOT2_HRV2_Green")
                    {
                        this->solarIrradiance[i] = 1865;
                    }
                    else if(sensorBand == "SPOT2_HRV2_Red")
                    {
                        this->solarIrradiance[i] = 1615;
                    }
                    else if(sensorBand == "SPOT2_HRV2_NIR")
                    {
                        this->solarIrradiance[i] = 1090;
                    }
                    else if(sensorBand == "SPOT3_HRV1_Pan")
                    {
                        this->solarIrradiance[i] = 1668;
                    }
                    else if(sensorBand == "SPOT3_HRV1_Green")
                    {
                        this->solarIrradiance[i] = 1854;
                    }
                    else if(sensorBand == "SPOT3_HRV1_Red")
                    {
                        this->solarIrradiance[i] = 1580;
                    }
                    else if(sensorBand == "SPOT3_HRV1_NIR")
                    {
                        this->solarIrradiance[i] = 1065;
                    }
                    else if(sensorBand == "SPOT3_HRV2_Pan")
                    {
                        this->solarIrradiance[i] = 1667;
                    }
                    else if(sensorBand == "SPOT3_HRV2_Green")
                    {
                        this->solarIrradiance[i] = 1855;
                    }
                    else if(sensorBand == "SPOT3_HRV2_Red")
                    {
                        this->solarIrradiance[i] = 1597;
                    }
                    else if(sensorBand == "SPOT3_HRV2_NIR")
                    {
                        this->solarIrradiance[i] = 1067;
                    }
                    else if(sensorBand == "SPOT4_HRV1_Mon")
                    {
                        this->solarIrradiance[i] = 1568;
                    }
                    else if(sensorBand == "SPOT4_HRV1_Green")
                    {
                        this->solarIrradiance[i] = 1843;
                    }
                    else if(sensorBand == "SPOT4_HRV1_Red")
                    {
                        this->solarIrradiance[i] = 1568;
                    }
                    else if(sensorBand == "SPOT4_HRV1_NIR")
                    {
                        this->solarIrradiance[i] = 1052;
                    }
                    else if(sensorBand == "SPOT4_HRV1_MIR")
                    {
                        this->solarIrradiance[i] = 233;
                    }
                    else if(sensorBand == "SPOT4_HRV2_Mon")
                    {
                        this->solarIrradiance[i] = 1586;
                    }
                    else if(sensorBand == "SPOT4_HRV2_Green")
                    {
                        this->solarIrradiance[i] = 1851;
                    }
                    else if(sensorBand == "SPOT4_HRV2_Red")
                    {
                        this->solarIrradiance[i] = 1586;
                    }
                    else if(sensorBand == "SPOT4_HRV2_NIR")
                    {
                        this->solarIrradiance[i] = 1054;
                    }
                    else if(sensorBand == "SPOT4_HRV2_MIR")
                    {
                        this->solarIrradiance[i] = 240;
                    }
                    else if(sensorBand == "SPOT5_HRG1_Pan")
                    {
                        this->solarIrradiance[i] = 1762;
                    }
                    else if(sensorBand == "SPOT5_HRG1_Green")
                    {
                        this->solarIrradiance[i] = 1858;
                    }
                    else if(sensorBand == "SPOT5_HRG1_Red")
                    {
                        this->solarIrradiance[i] = 1573;
                    }
                    else if(sensorBand == "SPOT5_HRG1_NIR")
                    {
                        this->solarIrradiance[i] = 1043;
                    }
                    else if(sensorBand == "SPOT5_HRG1_MIR")
                    {
                        this->solarIrradiance[i] = 236;
                    }
                    else if(sensorBand == "SPOT5_HRG2_Pan")
                    {
                        this->solarIrradiance[i] = 1773;
                    }
                    else if(sensorBand == "SPOT5_HRG2_Green")
                    {
                        this->solarIrradiance[i] = 1858;
                    }
                    else if(sensorBand == "SPOT5_HRG2_Red")
                    {
                        this->solarIrradiance[i] = 1575;
                    }
                    else if(sensorBand == "SPOT5_HRG2_NIR")
                    {
                        this->solarIrradiance[i] = 1047;
                    }
                    else if(sensorBand == "SPOT5_HRG2_MIR")
                    {
                        this->solarIrradiance[i] = 234;
                    }
                    else if(sensorBand == "QB_Pan")
                    {
                        this->solarIrradiance[i] = 1381.79;
                    }
                    else if(sensorBand == "QB_Blue")
                    {
                        this->solarIrradiance[i] = 1924.59;
                    }
                    else if(sensorBand == "QB_Green")
                    {
                        this->solarIrradiance[i] = 1843.08;
                    }
                    else if(sensorBand == "QB_Red")
                    {
                        this->solarIrradiance[i] = 1574.77;
                    }
                    else if(sensorBand == "QB_NIR")
                    {
                        this->solarIrradiance[i] = 1113.71;
                    }
                    else if(sensorBand == "IRSP6LISS3_Green")
                    {
                        this->solarIrradiance[i] = 1846.77;
                    }
                    else if(sensorBand == "IRSP6LISS3_Red")
                    {
                        this->solarIrradiance[i] = 1575.50;
                    }
                    else if(sensorBand == "IRSP6LISS3_NIR")
                    {
                        this->solarIrradiance[i] = 1087.34;
                    }
                    else if(sensorBand == "IRSP6LISS3_SWIR")
                    {
                        this->solarIrradiance[i] = 236.65;
                    }
                    else if(sensorBand == "ASTER_VNIR_B1")
                    {
                        this->solarIrradiance[i] = 1845.99;
                    }
                    else if(sensorBand == "ASTER_VNIR_B2")
                    {
                        this->solarIrradiance[i] = 1555.74;
                    }
                    else if(sensorBand == "ASTER_VNIR_B3")
                    {
                        this->solarIrradiance[i] = 1119.47;
                    }
                    else if(sensorBand == "ASTER_SWIR_B4")
                    {
                        this->solarIrradiance[i] = 231.25;
                    }
                    else if(sensorBand == "ASTER_SWIR_B5")
                    {
                        this->solarIrradiance[i] = 79.81;
                    }
                    else if(sensorBand == "ASTER_SWIR_B6")
                    {
                        this->solarIrradiance[i] = 74.99;
                    }
                    else if(sensorBand == "ASTER_SWIR_B7")
                    {
                        this->solarIrradiance[i] = 68.66;
                    }
                    else if(sensorBand == "ASTER_SWIR_B8")
                    {
                        this->solarIrradiance[i] = 59.74;
                    }
                    else if(sensorBand == "ASTER_SWIR_B9")
                    {
                        this->solarIrradiance[i] = 56.92;
                    }
                    else if(sensorBand == "RapidEye_Blue")
                    {
                        this->solarIrradiance[i] = 1997.8;
                    }
                    else if(sensorBand == "RapidEye_Green")
                    {
                        this->solarIrradiance[i] = 1863.5;
                    }
                    else if(sensorBand == "RapidEye_Red")
                    {
                        this->solarIrradiance[i] = 1560.4;
                    }
                    else if(sensorBand == "RapidEye_RedEdge")
                    {
                        this->solarIrradiance[i] = 1395.0;
                    }
                    else if(sensorBand == "RapidEye_NIR")
                    {
                        this->solarIrradiance[i] = 1124.4;
                    }
                    else
                    {
                        std::cout << "sensorBand = " << sensorBand << std::endl;
                        throw rsgis::RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    //throw rsgis::RSGISXMLArgumentsException("No \'sensorband\' attribute was provided.");
                    
                    XMLCh *irradianceXMLStr = xercesc::XMLString::transcode("irradiance");
                    if(bandElement->hasAttribute(irradianceXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(irradianceXMLStr));
                        this->solarIrradiance[i] = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'irradiance\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&irradianceXMLStr);
                    
                }
                xercesc::XMLString::release(&sensorBandXMLStr);	
            }
        }
        else if(xercesc::XMLString::equals(optionSPOTRadCal, optionXML))
        {
            this->option = spotradcal;
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
            
            
            xercesc::DOMNodeList *bandsNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            std::cout << "Found " << this->numBands << " Bands listed.\n";
            
            xercesc::DOMElement *bandElement = NULL;
            spotRadGainOffs = new rsgis::calib::SPOTRadianceGainsOffsets[numBands];
            inputImages = new std::string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandImageXMLStr = xercesc::XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = xercesc::XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->spotRadGainOffs[i].band = mathUtils.strtounsignedint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandIdxXMLStr);
                
                XMLCh *gainXMLStr = xercesc::XMLString::transcode("gain");
				if(bandElement->hasAttribute(gainXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(gainXMLStr));
					this->spotRadGainOffs[i].gain = mathUtils.strtofloat(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'gain\' attribute was provided.");
				}
				xercesc::XMLString::release(&gainXMLStr);	
            }
        }
        else if(xercesc::XMLString::equals(optionIkonosRadCal, optionXML))
        {
            this->option = ikonosradcal;
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
            
            
            xercesc::DOMNodeList *bandsNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            std::cout << "Found " << this->numBands << " Bands listed.\n";
            
            xercesc::DOMElement *bandElement = NULL;
            ikonosRadGainOffs = new rsgis::calib::IkonosRadianceGainsOffsets[numBands];
            inputImages = new std::string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = xercesc::XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->ikonosRadGainOffs[i].bandName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = xercesc::XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = xercesc::XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->ikonosRadGainOffs[i].band = mathUtils.strtounsignedint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = xercesc::XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    std::string sensorBand = std::string(charValue);
                    if(sensorBand == "IK_Pan_Pre2001")
                    {
                        this->ikonosRadGainOffs[i].calCoef = 161;
                        this->ikonosRadGainOffs[i].bandwidth = 403;
                    }
                    else if(sensorBand == "IK_Blue_Pre2001")
                    {
                        this->ikonosRadGainOffs[i].calCoef = 633;
                        this->ikonosRadGainOffs[i].bandwidth = 71.3;
                    }
                    else if(sensorBand == "IK_Green_Pre2001")
                    {
                        this->ikonosRadGainOffs[i].calCoef = 649;
                        this->ikonosRadGainOffs[i].bandwidth = 88.6;
                    }
                    else if(sensorBand == "IK_Red_Pre2001")
                    {
                        this->ikonosRadGainOffs[i].calCoef = 840;
                        this->ikonosRadGainOffs[i].bandwidth = 65.8;
                    }
                    else if(sensorBand == "IK_NIR_Pre2001")
                    {
                        this->ikonosRadGainOffs[i].calCoef = 746;
                        this->ikonosRadGainOffs[i].bandwidth = 95.4;
                    }
                    else if(sensorBand == "IK_Pan_Post2001")
                    {
                        this->ikonosRadGainOffs[i].calCoef = 161;
                        this->ikonosRadGainOffs[i].bandwidth = 403;
                    }
                    else if(sensorBand == "IK_Blue_Post2001")
                    {
                        this->ikonosRadGainOffs[i].calCoef = 728;
                        this->ikonosRadGainOffs[i].bandwidth = 71.3;
                    }
                    else if(sensorBand == "IK_Green_Post2001")
                    {
                        this->ikonosRadGainOffs[i].calCoef = 727;
                        this->ikonosRadGainOffs[i].bandwidth = 88.6;
                    }
                    else if(sensorBand == "IK_Red_Post2001")
                    {
                        this->ikonosRadGainOffs[i].calCoef = 949;
                        this->ikonosRadGainOffs[i].bandwidth = 65.8;
                    }
                    else if(sensorBand == "IK_NIR_Post2001")
                    {
                        this->ikonosRadGainOffs[i].calCoef = 843;
                        this->ikonosRadGainOffs[i].bandwidth = 95.4;
                    }
                    else
                    {
                        std::cout << "sensorBand = " << sensorBand << std::endl;
                        throw rsgis::RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    XMLCh *calCoefXMLStr = xercesc::XMLString::transcode("calcoef");
                    if(bandElement->hasAttribute(calCoefXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(calCoefXMLStr));
                        this->ikonosRadGainOffs[i].calCoef = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'calcoef\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&calCoefXMLStr);
                    
                    XMLCh *bandwidthXMLStr = xercesc::XMLString::transcode("bandwidth");
                    if(bandElement->hasAttribute(bandwidthXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandwidthXMLStr));
                        this->ikonosRadGainOffs[i].bandwidth = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'bandwidth\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&bandwidthXMLStr);
                }
                xercesc::XMLString::release(&sensorBandXMLStr);
            }
        }
        else if(xercesc::XMLString::equals(optionASTERRadCal, optionXML))
        {
            this->option = asterradcal;
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
            
            
            xercesc::DOMNodeList *bandsNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            std::cout << "Found " << this->numBands << " Bands listed.\n";
            
            xercesc::DOMElement *bandElement = NULL;
            asterRadGainOffs = new rsgis::calib::ASTERRadianceGainsOffsets[numBands];
            inputImages = new std::string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = xercesc::XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->asterRadGainOffs[i].bandName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = xercesc::XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = xercesc::XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->asterRadGainOffs[i].band = mathUtils.strtounsignedint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = xercesc::XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    std::string sensorBand = std::string(charValue);
                    if(sensorBand == "XXX")
                    {
                        this->asterRadGainOffs[i].unitConCoef = 0;
                    }
                    else if(sensorBand == "XXX")
                    {
                        this->asterRadGainOffs[i].unitConCoef = 0;
                    }
                    else
                    {
                        std::cout << "sensorBand = " << sensorBand << std::endl;
                        throw rsgis::RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    XMLCh *coefficientXMLStr = xercesc::XMLString::transcode("coefficient");
                    if(bandElement->hasAttribute(coefficientXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(coefficientXMLStr));
                        this->asterRadGainOffs[i].unitConCoef = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'coefficient\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&coefficientXMLStr);
                }
                xercesc::XMLString::release(&sensorBandXMLStr);
            }
        }
        else if(xercesc::XMLString::equals(optionIRSRadCal, optionXML))
        {
            this->option = irsradcal;
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
            
            
            xercesc::DOMNodeList *bandsNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            std::cout << "Found " << this->numBands << " Bands listed.\n";
            
            xercesc::DOMElement *bandElement = NULL;
            irsRadGainOffs = new rsgis::calib::IRSRadianceGainsOffsets[numBands];
            inputImages = new std::string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = xercesc::XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->irsRadGainOffs[i].bandName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = xercesc::XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = xercesc::XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->irsRadGainOffs[i].band = mathUtils.strtounsignedint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = xercesc::XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    std::string sensorBand = std::string(charValue);
                    if(sensorBand == "IRSP6LISS3_Green")
                    {
                        this->irsRadGainOffs[i].lMin = 0.0;
                        this->irsRadGainOffs[i].lMax = 12.06400;
                        this->irsRadGainOffs[i].qCalMin = 1;
                        this->irsRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "IRSP6LISS3_Red")
                    {
                        this->irsRadGainOffs[i].lMin = 0.0;
                        this->irsRadGainOffs[i].lMax = 15.13100;
                        this->irsRadGainOffs[i].qCalMin = 1;
                        this->irsRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "IRSP6LISS3_NIR")
                    {
                        this->irsRadGainOffs[i].lMin = 0.0;
                        this->irsRadGainOffs[i].lMax = 15.75700;
                        this->irsRadGainOffs[i].qCalMin = 1;
                        this->irsRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "IRSP6LISS3_SWIR")
                    {
                        this->irsRadGainOffs[i].lMin = 0.0;
                        this->irsRadGainOffs[i].lMax = 3.39700;
                        this->irsRadGainOffs[i].qCalMin = 1;
                        this->irsRadGainOffs[i].qCalMax = 255;
                    }
                    else
                    {
                        std::cout << "sensorBand = " << sensorBand << std::endl;
                        throw rsgis::RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    XMLCh *lMinXMLStr = xercesc::XMLString::transcode("lmin");
                    if(bandElement->hasAttribute(lMinXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(lMinXMLStr));
                        this->irsRadGainOffs[i].lMin = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'lmin\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&lMinXMLStr);
                    
                    XMLCh *lMaxXMLStr = xercesc::XMLString::transcode("lmax");
                    if(bandElement->hasAttribute(lMaxXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(lMaxXMLStr));
                        this->irsRadGainOffs[i].lMax = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'lmax\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&lMaxXMLStr);
                    
                    XMLCh *qCalMinXMLStr = xercesc::XMLString::transcode("qcalmin");
                    if(bandElement->hasAttribute(qCalMinXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(qCalMinXMLStr));
                        this->irsRadGainOffs[i].qCalMin = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'qcalmin\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&qCalMinXMLStr);
                    
                    XMLCh *qCalMaxXMLStr = xercesc::XMLString::transcode("qcalmax");
                    if(bandElement->hasAttribute(qCalMaxXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(qCalMaxXMLStr));
                        this->irsRadGainOffs[i].qCalMax = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'qcalmax\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&qCalMaxXMLStr);
                }
                xercesc::XMLString::release(&sensorBandXMLStr);
            }
        }
        else if(xercesc::XMLString::equals(optionQuickbird16bitRadCal, optionXML))
        {
            this->option = quickbird16bitradcal;
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
            
            
            xercesc::DOMNodeList *bandsNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            std::cout << "Found " << this->numBands << " Bands listed.\n";
            
            xercesc::DOMElement *bandElement = NULL;
            quickbird16bitRadGainOffs = new rsgis::calib::Quickbird16bitRadianceGainsOffsets[numBands];
            inputImages = new std::string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = xercesc::XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->quickbird16bitRadGainOffs[i].bandName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = xercesc::XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = xercesc::XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->quickbird16bitRadGainOffs[i].band = mathUtils.strtounsignedint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = xercesc::XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    std::string sensorBand = std::string(charValue);
                    if(sensorBand == "XXX")
                    {
                        this->quickbird16bitRadGainOffs[i].calFactor = 0;
                        this->quickbird16bitRadGainOffs[i].bandIntegrate = 0;
                    }
                    else if(sensorBand == "XXX")
                    {
                        this->quickbird16bitRadGainOffs[i].calFactor = 0;
                        this->quickbird16bitRadGainOffs[i].bandIntegrate = 0;
                    }
                    else
                    {
                        std::cout << "sensorBand = " << sensorBand << std::endl;
                        throw rsgis::RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    XMLCh *calFactorXMLStr = xercesc::XMLString::transcode("calfactor");
                    if(bandElement->hasAttribute(calFactorXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(calFactorXMLStr));
                        this->quickbird16bitRadGainOffs[i].calFactor = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'calfactor\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&calFactorXMLStr);
                    
                    XMLCh *bandIntegrateXMLStr = xercesc::XMLString::transcode("bandintegrate");
                    if(bandElement->hasAttribute(bandIntegrateXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandIntegrateXMLStr));
                        this->quickbird16bitRadGainOffs[i].bandIntegrate = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'bandintegrate\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&bandIntegrateXMLStr);
                }
                xercesc::XMLString::release(&sensorBandXMLStr);
            }
        }
        else if(xercesc::XMLString::equals(optionQuickbird8bitRadCal, optionXML))
        {
            this->option = quickbird8bitradcal;
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
            
            
            xercesc::DOMNodeList *bandsNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            std::cout << "Found " << this->numBands << " Bands listed.\n";
            
            xercesc::DOMElement *bandElement = NULL;
            quickbird8bitRadGainOffs = new rsgis::calib::Quickbird8bitRadianceGainsOffsets[numBands];
            inputImages = new std::string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = xercesc::XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->quickbird8bitRadGainOffs[i].bandName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = xercesc::XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = xercesc::XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->quickbird8bitRadGainOffs[i].band = mathUtils.strtounsignedint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = xercesc::XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    std::string sensorBand = std::string(charValue);
                    if(sensorBand == "XXX")
                    {
                        this->quickbird8bitRadGainOffs[i].calFactor = 0;
                        this->quickbird8bitRadGainOffs[i].k = 0;
                        this->quickbird8bitRadGainOffs[i].bandIntegrate = 0;
                    }
                    else if(sensorBand == "XXX")
                    {
                        this->quickbird8bitRadGainOffs[i].calFactor = 0;
                        this->quickbird8bitRadGainOffs[i].k = 0;
                        this->quickbird8bitRadGainOffs[i].bandIntegrate = 0;
                    }
                    else
                    {
                        std::cout << "sensorBand = " << sensorBand << std::endl;
                        throw rsgis::RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    XMLCh *calFactorXMLStr = xercesc::XMLString::transcode("calfactor");
                    if(bandElement->hasAttribute(calFactorXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(calFactorXMLStr));
                        this->quickbird8bitRadGainOffs[i].calFactor = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'calfactor\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&calFactorXMLStr);
                    
                    XMLCh *kXMLStr = xercesc::XMLString::transcode("k");
                    if(bandElement->hasAttribute(kXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(kXMLStr));
                        this->quickbird8bitRadGainOffs[i].k = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'k\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&kXMLStr);
                    
                    XMLCh *bandIntegrateXMLStr = xercesc::XMLString::transcode("bandintegrate");
                    if(bandElement->hasAttribute(bandIntegrateXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandIntegrateXMLStr));
                        this->quickbird8bitRadGainOffs[i].bandIntegrate = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'bandintegrate\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&bandIntegrateXMLStr);
                }
                xercesc::XMLString::release(&sensorBandXMLStr);
            }
        }
        else if(xercesc::XMLString::equals(optionWorldView2RadCal, optionXML))
        {
            this->option = worldview2radcal;
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
            
            
            xercesc::DOMNodeList *bandsNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            std::cout << "Found " << this->numBands << " Bands listed.\n";
            
            xercesc::DOMElement *bandElement = NULL;
            worldview2RadGainOffs = new rsgis::calib::WorldView2RadianceGainsOffsets[numBands];
            inputImages = new std::string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = xercesc::XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->worldview2RadGainOffs[i].bandName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = xercesc::XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = xercesc::XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->worldview2RadGainOffs[i].band = mathUtils.strtounsignedint(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = xercesc::XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    std::string sensorBand = std::string(charValue);
                    if(sensorBand == "WV2_Coast")
                    {
                        this->worldview2RadGainOffs[i].absCalFact = 0;
                        this->worldview2RadGainOffs[i].effBandWidth = 0.0473;
                    }
                    else if(sensorBand == "WV2_Blue")
                    {
                        this->worldview2RadGainOffs[i].absCalFact = 0;
                        this->worldview2RadGainOffs[i].effBandWidth = 0.0534;
                    }
                    else if(sensorBand == "WV2_Green")
                    {
                        this->worldview2RadGainOffs[i].absCalFact = 0;
                        this->worldview2RadGainOffs[i].effBandWidth = 0.0630;
                    }
                    else if(sensorBand == "WV2_Yellow")
                    {
                        this->worldview2RadGainOffs[i].absCalFact = 0;
                        this->worldview2RadGainOffs[i].effBandWidth = 0.0374;
                    }
                    else if(sensorBand == "WV2_Red")
                    {
                        this->worldview2RadGainOffs[i].absCalFact = 0;
                        this->worldview2RadGainOffs[i].effBandWidth = 0.0574;
                    }
                    else if(sensorBand == "WV2_Red_Edge")
                    {
                        this->worldview2RadGainOffs[i].absCalFact = 0;
                        this->worldview2RadGainOffs[i].effBandWidth = 0.393;
                    }
                    else if(sensorBand == "WV2_NIR1")
                    {
                        this->worldview2RadGainOffs[i].absCalFact = 0;
                        this->worldview2RadGainOffs[i].effBandWidth = 0.989;
                    }
                    else if(sensorBand == "WV2_NIR2")
                    {
                        this->worldview2RadGainOffs[i].absCalFact = 0;
                        this->worldview2RadGainOffs[i].effBandWidth = 0.996;
                    }
                    else
                    {
                        std::cout << "sensorBand = " << sensorBand << std::endl;
                        throw rsgis::RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    XMLCh *calFactorXMLStr = xercesc::XMLString::transcode("calfactor");
                    if(bandElement->hasAttribute(calFactorXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(calFactorXMLStr));
                        this->worldview2RadGainOffs[i].absCalFact = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'calfactor\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&calFactorXMLStr);
                    
                    XMLCh *bandIntegrateXMLStr = xercesc::XMLString::transcode("bandintegrate");
                    if(bandElement->hasAttribute(bandIntegrateXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandIntegrateXMLStr));
                        this->worldview2RadGainOffs[i].effBandWidth = mathUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'bandintegrate\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&bandIntegrateXMLStr);
                }
                xercesc::XMLString::release(&sensorBandXMLStr);
            }
        }
        else if(xercesc::XMLString::equals(optionApply6SSingle, optionXML))
		{
            this->option = apply6ssingle;
			
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
            
            // Set scaling factor for output images
            this->scaleFactor = 1.0;
            XMLCh *scaleFactorXMLStr = xercesc::XMLString::transcode("scaleFactor");
            if(argElement->hasAttribute(scaleFactorXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(scaleFactorXMLStr));
                this->scaleFactor = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            xercesc::XMLString::release(&scaleFactorXMLStr);
            
            this->noDataVal = 0.0;
            this->useNoDataVal = false;
            XMLCh *noDataXMLStr = xercesc::XMLString::transcode("noData");
            if(argElement->hasAttribute(noDataXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(noDataXMLStr));
                this->noDataVal = mathUtils.strtofloat(std::string(charValue));
                this->useNoDataVal = true;
                xercesc::XMLString::release(&charValue);
            }
            xercesc::XMLString::release(&noDataXMLStr);
			
            xercesc::DOMNodeList *bandsNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:band"));
            this->numValues = bandsNodesList->getLength();
            std::cout << "Found " << this->numValues << " Bands listed.\n";
            
            xercesc::DOMElement *bandElement = NULL;
            this->aXSingle = new float[numValues];
            this->bXSingle = new float[numValues];
            this->cXSingle = new float[numValues];
            this->imageBands = new unsigned int[numValues];
            
            for(int i = 0; i < numValues; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandsNodesList->item(i));
				
				XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
				if(bandElement->hasAttribute(bandXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandXMLStr));
					this->imageBands[i] = mathUtils.strtounsignedint(std::string(charValue));
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
				}
				xercesc::XMLString::release(&bandXMLStr);
                
                XMLCh *aXXMLStr = xercesc::XMLString::transcode("ax");
                if(bandElement->hasAttribute(aXXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(aXXMLStr));
                    this->aXSingle[i] = mathUtils.strtofloat(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'ax\' attribute was provided.");
                }
                xercesc::XMLString::release(&aXXMLStr);
                
                XMLCh *bXXMLStr = xercesc::XMLString::transcode("bx");
                if(bandElement->hasAttribute(bXXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bXXMLStr));
                    this->bXSingle[i] = mathUtils.strtofloat(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'bx\' attribute was provided.");
                }
                xercesc::XMLString::release(&bXXMLStr);
                
                XMLCh *cXXMLStr = xercesc::XMLString::transcode("cx");
                if(bandElement->hasAttribute(cXXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(cXXMLStr));
                    this->cXSingle[i] = mathUtils.strtofloat(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'cx\' attribute was provided.");
                }
                xercesc::XMLString::release(&cXXMLStr);
            }
        }
        else if(xercesc::XMLString::equals(optionApply6S, optionXML))
		{
            this->option = apply6s;
            this->useTopo6S = false;
			
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
            
            // Set scaling factor for output images
            this->scaleFactor = 1.0;
            XMLCh *scaleFactorXMLStr = xercesc::XMLString::transcode("scaleFactor");
            if(argElement->hasAttribute(scaleFactorXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(scaleFactorXMLStr));
                this->scaleFactor = mathUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            xercesc::XMLString::release(&scaleFactorXMLStr);
            
			XMLCh *demXMLStr = xercesc::XMLString::transcode("inputDEM");
            if(argElement->hasAttribute(demXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(demXMLStr));
                this->inputDEM = std::string(charValue);
				this->useTopo6S = true;
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                std::cout << "\tNo DEM provided, using the same coefficients for all elevations" << std::endl;
            }
            xercesc::XMLString::release(&demXMLStr);
			
            xercesc::DOMNodeList *bandsNodesList = argElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:band"));
            this->numValues = bandsNodesList->getLength();		
            this->numElevation = 0;
            std::cout << "Found " << this->numValues << " Bands listed.\n";
            
            xercesc::DOMElement *bandElement = NULL;
			xercesc::DOMElement *elevationElement = NULL;
            this->aX = new float*[numValues];
            this->bX = new float*[numValues];
            this->cX = new float*[numValues];
            this->imageBands = new unsigned int[numValues];
            
            for(int i = 0; i < numValues; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandsNodesList->item(i));
				
				XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
				if(bandElement->hasAttribute(bandXMLStr))
				{
					char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandXMLStr));
					this->imageBands[i] = mathUtils.strtounsignedint(std::string(charValue))-1;
					xercesc::XMLString::release(&charValue);
				}
				else
				{
					throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
				}
				xercesc::XMLString::release(&bandXMLStr);
				
				if (!this->useTopo6S) 
				{
					this->aX[i] = new float[this->numElevation+1];
					this->bX[i] = new float[this->numElevation+1];
					this->cX[i] = new float[this->numElevation+1];
					this->elevationThresh = NULL;
					
					XMLCh *aXXMLStr = xercesc::XMLString::transcode("ax");
					if(bandElement->hasAttribute(aXXMLStr))
					{
						char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(aXXMLStr));
						this->aX[i][0] = mathUtils.strtofloat(std::string(charValue));
						xercesc::XMLString::release(&charValue);
					}
					else
					{
						throw rsgis::RSGISXMLArgumentsException("No \'ax\' attribute was provided.");
					}
					xercesc::XMLString::release(&aXXMLStr);
					
					XMLCh *bXXMLStr = xercesc::XMLString::transcode("bx");
					if(bandElement->hasAttribute(bXXMLStr))
					{
						char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bXXMLStr));
						this->bX[i][0] = mathUtils.strtofloat(std::string(charValue));
						xercesc::XMLString::release(&charValue);
					}
					else
					{
						throw rsgis::RSGISXMLArgumentsException("No \'bx\' attribute was provided.");
					}
					xercesc::XMLString::release(&bXXMLStr);
					
					XMLCh *cXXMLStr = xercesc::XMLString::transcode("cx");
					if(bandElement->hasAttribute(cXXMLStr))
					{
						char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(cXXMLStr));
						this->cX[i][0] = mathUtils.strtofloat(std::string(charValue));
						xercesc::XMLString::release(&charValue);
					}
					else
					{
						throw rsgis::RSGISXMLArgumentsException("No \'cx\' attribute was provided.");
					}
					xercesc::XMLString::release(&cXXMLStr);
				}
				
				else
				{
					xercesc::DOMNodeList *elevationNodesList = bandElement->getElementsByTagName(xercesc::XMLString::transcode("rsgis:elevation"));
					if (i == 0) // Set number of elevation values based on first band
					{
						this->numElevation = elevationNodesList->getLength();
						std::cout << "numElevation = " << this->numElevation << std::endl;
					}
					else 
					{
						 if(elevationNodesList->getLength() != this->numElevation)
						 {
							 throw rsgis::RSGISXMLArgumentsException("The number of elevation threasholds is not equal for all bands");
						 }
					}
					
					this->aX[i] = new float[this->numElevation];
					this->bX[i] = new float[this->numElevation];
					this->cX[i] = new float[this->numElevation];
					this->elevationThresh = new float[this->numElevation];
					
					for(unsigned int et = 0; et < this->numElevation; et++)
					{
						elevationElement = static_cast<xercesc::DOMElement*>(elevationNodesList->item(et));
						
						if (i == 0) // Seting elevation threasholds using first band
						{
							XMLCh *elevationUpperXXMLStr = xercesc::XMLString::transcode("elevationUpper");
							if(elevationElement->hasAttribute(elevationUpperXXMLStr))
							{
								char *charValue = xercesc::XMLString::transcode(elevationElement->getAttribute(elevationUpperXXMLStr));
								this->elevationThresh[et] = mathUtils.strtofloat(std::string(charValue));
								xercesc::XMLString::release(&charValue);
							}
							else
							{
								throw rsgis::RSGISXMLArgumentsException("No \'elevationUpper\' attribute was provided.");
							}
							xercesc::XMLString::release(&elevationUpperXXMLStr);
						}
						
						XMLCh *aXXMLStr = xercesc::XMLString::transcode("ax");
						if(elevationElement->hasAttribute(aXXMLStr))
						{
							char *charValue = xercesc::XMLString::transcode(elevationElement->getAttribute(aXXMLStr));
							this->aX[i][et] = mathUtils.strtofloat(std::string(charValue));
							xercesc::XMLString::release(&charValue);
						}
						else
						{
							throw rsgis::RSGISXMLArgumentsException("No \'ax\' attribute was provided for elevtion threashold.");
						}
						xercesc::XMLString::release(&aXXMLStr);
						
						XMLCh *bXXMLStr = xercesc::XMLString::transcode("bx");
						if(elevationElement->hasAttribute(bXXMLStr))
						{
							char *charValue = xercesc::XMLString::transcode(elevationElement->getAttribute(bXXMLStr));
							this->bX[i][et] = mathUtils.strtofloat(std::string(charValue));
							xercesc::XMLString::release(&charValue);
						}
						else
						{
							throw rsgis::RSGISXMLArgumentsException("No \'bx\' attribute was provided for elevtion threashold.");
						}
						xercesc::XMLString::release(&bXXMLStr);
						
						XMLCh *cXXMLStr = xercesc::XMLString::transcode("cx");
						if(elevationElement->hasAttribute(cXXMLStr))
						{
							char *charValue = xercesc::XMLString::transcode(elevationElement->getAttribute(cXXMLStr));
							this->cX[i][et] = mathUtils.strtofloat(std::string(charValue));
							xercesc::XMLString::release(&charValue);
						}
						else
						{
							throw rsgis::RSGISXMLArgumentsException("No \'cx\' attribute was provided for elevtion threashold.");
						}
						xercesc::XMLString::release(&cXXMLStr);
						
						
					}

					
				}
				
            }
        }
		else 
		{
			std::string message = "RSGISExeImageCalibration did not recognise option " + std::string(xercesc::XMLString::transcode(optionXML));
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
	xercesc::XMLString::release(&optionLandsatRadCal);
    xercesc::XMLString::release(&optionLandsatRadCalMultiAdd);
    xercesc::XMLString::release(&optionTopAtmosRefl);
    xercesc::XMLString::release(&optionSPOTRadCal);
    xercesc::XMLString::release(&optionIkonosRadCal);
    xercesc::XMLString::release(&optionASTERRadCal);
    xercesc::XMLString::release(&optionIRSRadCal);
    xercesc::XMLString::release(&optionQuickbird16bitRadCal);
    xercesc::XMLString::release(&optionQuickbird8bitRadCal);
    xercesc::XMLString::release(&optionWorldView2RadCal);
    xercesc::XMLString::release(&optionApply6SSingle);
    xercesc::XMLString::release(&optionApply6S);
	
	parsed = true; // if all successful, it is parsed
}

void RSGISExeImageCalibration::runAlgorithm() throw(rsgis::RSGISException)
{
	std::cout.precision(10);
	
	if(!parsed)
	{
		throw rsgis::RSGISException("Before running the parameters must be retrieved");
	}
	else
	{
		if(this->option == RSGISExeImageCalibration::landsatradcal)
		{
			std::cout << "This command converts Landsat from digital number values to at sensor radiance.\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
            int i = 1;
			for(std::vector<rsgis::cmds::CmdsLandsatRadianceGainsOffsets>::iterator iterBands = landsatRadGainOffs.begin(); iterBands != landsatRadGainOffs.end(); ++iterBands)
			{
				std::cout << i << ")\t name = " << (*iterBands).bandName << "    band = " << (*iterBands).band << std::endl;
                ++i;
			}
            
            try
            {
                rsgis::cmds::executeConvertLandsat2Radiance(this->outputImage, this->imageFormat, this->landsatRadGainOffs);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (rsgis::RSGISException &e)
            {
                throw e;
            }
            catch (std::exception &e)
            {
                throw rsgis::RSGISException(e.what());
            }
		}
        else if(this->option == RSGISExeImageCalibration::landsatradcalmultiadd)
		{
			std::cout << "This command converts Landsat from digital number values to at sensor radiance.\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
            int i = 1;
			for(std::vector<rsgis::cmds::CmdsLandsatRadianceGainsOffsetsMultiAdd>::iterator iterBands = landsatRadGainOffsMultiAdd.begin(); iterBands != landsatRadGainOffsMultiAdd.end(); ++iterBands)
			{
				std::cout << i << ")\t name = " << (*iterBands).bandName << "    band = " << (*iterBands).band << std::endl;
                ++i;
			}
            
            try
            {
                rsgis::cmds::executeConvertLandsat2RadianceMultiAdd(this->outputImage, this->imageFormat, this->landsatRadGainOffsMultiAdd);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (rsgis::RSGISException &e)
            {
                throw e;
            }
            catch (std::exception &e)
            {
                throw rsgis::RSGISException(e.what());
            }
		}
        else if(this->option == RSGISExeImageCalibration::spotradcal)
        {
            std::cout << "This command converts SPOT from digital number values to at sensor radiance.\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				std::cout << i << ")\t band = " << spotRadGainOffs[i].band << std::endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISSPOTRadianceCalibration *radianceCalibration = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
            std::string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new std::string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					std::cout << "Opening: " << this->inputImages[i] << std::endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						std::string message = std::string("Could not open image ") + this->inputImages[i];
						throw rsgis::RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->spotRadGainOffs[i].band > numRasterBands)
					{
						throw rsgis::RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->spotRadGainOffs[i].band = totalNumRasterBands + this->spotRadGainOffs[i].band-1;
					outBandNames[i] = "Band";
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new rsgis::calib::RSGISSPOTRadianceCalibration(numBands, this->spotRadGainOffs);
				
				calcImage = new rsgis::img::RSGISCalcImage(radianceCalibration, "", true);
				calcImage->calcImage(datasets, this->numBands, this->outputImage, true, outBandNames,this->imageFormat);
				
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					GDALClose(datasets[i]);
				}
				delete[] datasets;
				delete[] spotRadGainOffs;
				delete[] outBandNames;
				
				delete radianceCalibration;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}

        }
        else if(this->option == RSGISExeImageCalibration::ikonosradcal)
        {
            std::cout << "This command converts Ikonos from digital number values to at sensor radiance.\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				std::cout << i << ")\t name = " << ikonosRadGainOffs[i].bandName << " band = " << ikonosRadGainOffs[i].band << std::endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISIkonosRadianceCalibration *radianceCalibration = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
            std::string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new std::string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					std::cout << "Opening: " << this->inputImages[i] << std::endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						std::string message = std::string("Could not open image ") + this->inputImages[i];
						throw rsgis::RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->ikonosRadGainOffs[i].band > numRasterBands)
					{
						throw rsgis::RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->ikonosRadGainOffs[i].band = totalNumRasterBands + this->ikonosRadGainOffs[i].band-1;
					outBandNames[i] = this->ikonosRadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new rsgis::calib::RSGISIkonosRadianceCalibration(numBands, this->ikonosRadGainOffs);
				
				calcImage = new rsgis::img::RSGISCalcImage(radianceCalibration, "", true);
				calcImage->calcImage(datasets, this->numBands, this->outputImage, true, outBandNames,this->imageFormat);
				
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					GDALClose(datasets[i]);
				}
				delete[] datasets;
				delete[] ikonosRadGainOffs;
				delete[] outBandNames;
				
				delete radianceCalibration;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::asterradcal)
        {
            std::cout << "This command converts ASTER from digital number values to at sensor radiance.\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				std::cout << i << ")\t name = " << asterRadGainOffs[i].bandName << " band = " << asterRadGainOffs[i].band << std::endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISASTERRadianceCalibration *radianceCalibration = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
            std::string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new std::string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					std::cout << "Opening: " << this->inputImages[i] << std::endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						std::string message = std::string("Could not open image ") + this->inputImages[i];
						throw rsgis::RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->asterRadGainOffs[i].band > numRasterBands)
					{
						throw rsgis::RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->asterRadGainOffs[i].band = totalNumRasterBands + this->asterRadGainOffs[i].band-1;
					outBandNames[i] = this->asterRadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new rsgis::calib::RSGISASTERRadianceCalibration(numBands, this->asterRadGainOffs);
				
				calcImage = new rsgis::img::RSGISCalcImage(radianceCalibration, "", true);
				calcImage->calcImage(datasets, this->numBands, this->outputImage, true, outBandNames,this->imageFormat);
				
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					GDALClose(datasets[i]);
				}
				delete[] datasets;
				delete[] asterRadGainOffs;
				delete[] outBandNames;
				
				delete radianceCalibration;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::irsradcal)
        {
            std::cout << "This command converts IRS from digital number values to at sensor radiance.\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				std::cout << i << ")\t name = " << irsRadGainOffs[i].bandName << " band = " << irsRadGainOffs[i].band << std::endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISIRSRadianceCalibration *radianceCalibration = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
            std::string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new std::string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					std::cout << "Opening: " << this->inputImages[i] << std::endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						std::string message = std::string("Could not open image ") + this->inputImages[i];
						throw rsgis::RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->irsRadGainOffs[i].band > numRasterBands)
					{
						throw rsgis::RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->irsRadGainOffs[i].band = totalNumRasterBands + this->irsRadGainOffs[i].band-1;
					outBandNames[i] = this->irsRadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new rsgis::calib::RSGISIRSRadianceCalibration(numBands, this->irsRadGainOffs);
				
				calcImage = new rsgis::img::RSGISCalcImage(radianceCalibration, "", true);
				calcImage->calcImage(datasets, this->numBands, this->outputImage, true, outBandNames,this->imageFormat);
				
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					GDALClose(datasets[i]);
				}
				delete[] datasets;
				delete[] irsRadGainOffs;
				delete[] outBandNames;
				
				delete radianceCalibration;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::quickbird16bitradcal)
        {
            std::cout << "This command converts QuickBird 16bit from digital number values to at sensor radiance.\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				std::cout << i << ")\t name = " << quickbird16bitRadGainOffs[i].bandName << " band = " << quickbird16bitRadGainOffs[i].band << std::endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISQuickbird16bitRadianceCalibration *radianceCalibration = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
            std::string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new std::string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					std::cout << "Opening: " << this->inputImages[i] << std::endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						std::string message = std::string("Could not open image ") + this->inputImages[i];
						throw rsgis::RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->quickbird16bitRadGainOffs[i].band > numRasterBands)
					{
						throw rsgis::RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->quickbird16bitRadGainOffs[i].band = totalNumRasterBands + this->quickbird16bitRadGainOffs[i].band-1;
					outBandNames[i] = this->quickbird16bitRadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new rsgis::calib::RSGISQuickbird16bitRadianceCalibration(numBands, this->quickbird16bitRadGainOffs);
				
				calcImage = new rsgis::img::RSGISCalcImage(radianceCalibration, "", true);
				calcImage->calcImage(datasets, this->numBands, this->outputImage, true, outBandNames,this->imageFormat);
				
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					GDALClose(datasets[i]);
				}
				delete[] datasets;
				delete[] quickbird16bitRadGainOffs;
				delete[] outBandNames;
				
				delete radianceCalibration;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::quickbird8bitradcal)
        {
            std::cout << "This command converts QuickBird 8 bit from digital number values to at sensor radiance.\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				std::cout << i << ")\t name = " << quickbird8bitRadGainOffs[i].bandName << " band = " << quickbird8bitRadGainOffs[i].band << std::endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISQuickbird8bitRadianceCalibration *radianceCalibration = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
            std::string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new std::string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					std::cout << "Opening: " << this->inputImages[i] << std::endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						std::string message = std::string("Could not open image ") + this->inputImages[i];
						throw rsgis::RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->quickbird8bitRadGainOffs[i].band > numRasterBands)
					{
						throw rsgis::RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->quickbird8bitRadGainOffs[i].band = totalNumRasterBands + this->quickbird8bitRadGainOffs[i].band-1;
					outBandNames[i] = this->quickbird8bitRadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new rsgis::calib::RSGISQuickbird8bitRadianceCalibration(numBands, this->quickbird8bitRadGainOffs);
				
				calcImage = new rsgis::img::RSGISCalcImage(radianceCalibration, "", true);
				calcImage->calcImage(datasets, this->numBands, this->outputImage, true, outBandNames,this->imageFormat);
				
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					GDALClose(datasets[i]);
				}
				delete[] datasets;
				delete[] quickbird8bitRadGainOffs;
				delete[] outBandNames;
				
				delete radianceCalibration;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::worldview2radcal)
        {
            std::cout << "This command converts WorldView2 from digital number values to at sensor radiance.\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				std::cout << i << ")\t name = " << worldview2RadGainOffs[i].bandName << " band = " << worldview2RadGainOffs[i].band << std::endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			rsgis::calib::RSGISWorldView2RadianceCalibration *radianceCalibration = NULL;
			rsgis::img::RSGISCalcImage *calcImage = NULL;
            std::string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new std::string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					std::cout << "Opening: " << this->inputImages[i] << std::endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						std::string message = std::string("Could not open image ") + this->inputImages[i];
						throw rsgis::RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->worldview2RadGainOffs[i].band > numRasterBands)
					{
						throw rsgis::RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->worldview2RadGainOffs[i].band = totalNumRasterBands + this->worldview2RadGainOffs[i].band-1;
					outBandNames[i] = this->worldview2RadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new rsgis::calib::RSGISWorldView2RadianceCalibration(numBands, this->worldview2RadGainOffs);
				
				calcImage = new rsgis::img::RSGISCalcImage(radianceCalibration, "", true);
				calcImage->calcImage(datasets, this->numBands, this->outputImage, true, outBandNames,this->imageFormat);
				
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					GDALClose(datasets[i]);
				}
				delete[] datasets;
				delete[] worldview2RadGainOffs;
				delete[] outBandNames;
				
				delete radianceCalibration;
				delete calcImage;
			}
			catch(rsgis::RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::topatmosrefl)
        {
            std::cout << "This command converts at sensor radiance to top of atmosphere reflectance.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Scaling factor: " << this->scaleFactor << std::endl;
            std::cout << "Julian Day: " << this->julianDay << std::endl;
            std::cout << "Solar Zenith: " << this->solarZenith << std::endl;
            
			for(unsigned int i = 0; i < numBands; ++i)
			{
				std::cout << "Solar Irradiance for band " <<  i+1 << ":\t" << this->solarIrradiance[i] << std::endl;
			}
            
            try
            {
                rsgis::cmds::executeConvertRadiance2TOARefl(this->inputImage, this->outputImage, this->imageFormat, this->rsgisOutDataType, this->scaleFactor, this->julianDay, true, 0, 0, 0, (this->solarZenith*(M_PI/180)), this->solarIrradiance, this->numBands);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (rsgis::RSGISException &e)
            {
                throw e;
            }
            catch (std::exception &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            
            delete[] solarIrradiance;
        }
        else if(this->option == RSGISExeImageCalibration::apply6ssingle)
        {
            std::cout << "This command applies a single set of coefficients exported from 6S to a radiance image.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Scale Factor: " << this->scaleFactor << std::endl;
            std::cout << "Num Elevations: " << this->numElevation << std::endl;
            if(this->useNoDataVal)
            {
                std::cout << "Using no data value " << this->noDataVal << std::endl;
            }
			std::cout << "Coefficients:" << std::endl;
            for(int i = 0; i < numValues; ++i)
			{
				std::cout << "\tBand " <<  this->imageBands[i]+1 << ": " << "ax = " <<this->aXSingle[i] << ", bx = " << this->bXSingle[i] << ", cx = " << this->cXSingle[i] << std::endl;
			}
            
            try
            {
                rsgis::cmds::executeRad2SREFSingle6sParams(this->inputImage, this->outputImage, this->imageFormat, this->rsgisOutDataType, this->scaleFactor, this->imageBands, this->aXSingle, this->bXSingle, this->cXSingle, this->numValues, this->noDataVal, this->useNoDataVal);
                delete[] this->aXSingle;
                delete[] this->bXSingle;
                delete[] this->cXSingle;
                delete[] this->imageBands;
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (rsgis::RSGISException &e)
            {
                throw e;
            }
            catch (std::exception &e)
            {
                throw rsgis::RSGISException(e.what());
            }
        }
        else if(this->option == RSGISExeImageCalibration::apply6s)
        {
            std::cout << "This command applies coefficients exported from 6S to a radiance image.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Scale Factor: " << this->scaleFactor << std::endl;
            std::cout << "Num Elevations: " << this->numElevation << std::endl;
			if(this->useTopo6S)
            {
                std::cout << "Coefficients (for first elevation threashold):" << std::endl;
            }
			else
            {
                std::cout << "Coefficients:" << std::endl;
            }
            for(int i = 0; i < numValues; ++i)
			{
				std::cout << " Band " <<  this->imageBands[i]+1 << ": " << "ax = " <<this->aX[i][0] << ", bx = " << this->bX[i][0] << ", cx = " << this->cX[i][0] << std::endl;
			}
            
            if(this->useTopo6S)
            {
                GDALAllRegister();
                GDALDataset **datasets = NULL;
                rsgis::calib::RSGISApply6SCoefficients *apply6SCoefficients = NULL;
                rsgis::img::RSGISCalcImage *calcImage = NULL;
                
                try
                {
                    unsigned int nDatasets = 1;
                    if(!this->useTopo6S)
                    {
                        datasets = new GDALDataset*[1];
                        std::cout << "Open image" << this->inputImage << std::endl;
                        datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
                        if(datasets[0] == NULL)
                        {
                            std::string message = std::string("Could not open image ") + this->inputImage;
                            throw rsgis::RSGISImageException(message.c_str());
                        }
                        
                        int numRasterBands = datasets[0]->GetRasterCount();
                        if(this->numValues > numRasterBands)
                        {
                            GDALClose(datasets[0]);
                            delete[] datasets;
                            throw rsgis::RSGISException("The number of input image bands is less than the number of values specified.");
                        }
                        
                    }
                    else
                    {
                        nDatasets = 2;
                        
                        datasets = new GDALDataset*[2];
                        
                        std::cout << "Open DEM" << this->inputDEM << std::endl;
                        datasets[0] = (GDALDataset *) GDALOpen(this->inputDEM.c_str(), GA_ReadOnly);
                        if(datasets[0] == NULL)
                        {
                            std::string message = std::string("Could not open DEM ") + this->inputDEM;
                            throw rsgis::RSGISImageException(message.c_str());
                        }
                        
                        if(datasets[0]->GetRasterCount() != 1)
                        {
                            GDALClose(datasets[0]);
                            delete[] datasets;
                            throw rsgis::RSGISException("The DEM should only contain one band");
                        }
                        
                        
                        std::cout << "Open image" << this->inputImage << std::endl;
                        datasets[1] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
                        if(datasets[1] == NULL)
                        {
                            std::string message = std::string("Could not open image ") + this->inputImage;
                            throw rsgis::RSGISImageException(message.c_str());
                        }
                        
                        int numRasterBands = datasets[1]->GetRasterCount();
                        if(this->numValues > numRasterBands)
                        {
                            GDALClose(datasets[1]);
                            delete[] datasets;
                            throw rsgis::RSGISException("The number of input image bands is less than the number of values specified.");
                        }
                        
                    }
                    
                    apply6SCoefficients = new rsgis::calib::RSGISApply6SCoefficients(this->numValues, this->imageBands, this->aX, this->bX, this->cX, this->numValues, this->elevationThresh, this->numElevation, this->scaleFactor);
                    
                    calcImage = new rsgis::img::RSGISCalcImage(apply6SCoefficients, "", true);
                    calcImage->calcImage(datasets, nDatasets, this->outputImage, false, NULL, this->imageFormat, this->outDataType);
                    
                    
                    GDALClose(datasets[0]);
                    if(useTopo6S)
                    {
                        GDALClose(datasets[1]);
                    }
                    delete[] datasets;
                    
                    delete[] imageBands;
                    delete[] this->elevationThresh;
                    
                    for (int i = 0; i < this->numValues; ++i) 
                    {
                        delete[] this->aX[i];
                        delete[] this->bX[i];
                        delete[] this->cX[i];
                    }
                    delete[] this->aX;
                    delete[] this->bX;
                    delete[] this->cX;
                    
                    
                    delete apply6SCoefficients;
                    delete calcImage;
                }
                catch(rsgis::RSGISException e)
                {
                    throw e;
                }
            }
            else
            {
                throw rsgis::RSGISException("The Apply6S Single commands must now be used it the topographic functionality is not required.");
            }
            
            
        }
		else
		{
			throw rsgis::RSGISException("RSGISExeImageCalibration does not know this option");
		}
	}
}


void RSGISExeImageCalibration::printParameters()
{
	std::cout.precision(10);
	
	if(!parsed)
	{
		throw rsgis::RSGISException("Before running the parameters must be retrieved");
	}
	else
	{
		if(this->option == RSGISExeImageCalibration::landsatradcal)
		{
			std::cout << "This command converts Landsat from digital number values to at sensor radiance.\n";
			std::cout << "Output Image: " << this->outputImage << std::endl;
            int i = 1;
			for(std::vector<rsgis::cmds::CmdsLandsatRadianceGainsOffsets>::iterator iterBands = landsatRadGainOffs.begin(); iterBands != landsatRadGainOffs.end(); ++iterBands)
			{
				std::cout << i << ")\t name = " << (*iterBands).bandName << " band = " << (*iterBands).band << std::endl;
                ++i;
			}
		}
        else if(this->option == RSGISExeImageCalibration::topatmosrefl)
        {
            std::cout << "This command converts at sensor radiance to top of atmosphere reflectance.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Scaling factor: " << this->scaleFactor << std::endl;
            std::cout << "Julian Day: " << this->julianDay << std::endl;
            std::cout << "Solar Zenith: " << this->solarZenith << std::endl;
            
			for(unsigned int i = 0; i < numBands; ++i)
			{
				std::cout << "Solar Irradiance for band " <<  i+1 << ":\t" << this->solarIrradiance[i] << std::endl;
			}
        }
        else if(this->option == RSGISExeImageCalibration::apply6ssingle)
        {
            std::cout << "This command applies a single set of coefficients exported from 6S to a radiance image.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            std::cout << "Scale Factor: " << this->scaleFactor << std::endl;
            std::cout << "Num Elevations: " << this->numElevation << std::endl;
			std::cout << "Coefficients:" << std::endl;
            for(int i = 0; i < numValues; ++i)
			{
				std::cout << "\tBand " <<  this->imageBands[i]+1 << ": " << "ax = " <<this->aXSingle[i] << ", bx = " << this->bXSingle[i] << ", cx = " << this->cXSingle[i] << std::endl;
			}
            
            delete[] this->aXSingle;
            delete[] this->bXSingle;
            delete[] this->cXSingle;
        }
        else if(this->option == RSGISExeImageCalibration::apply6s)
        {
            std::cout << "This command applies coefficients exported from 6S to a radiance image.\n";
			std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Output Image: " << this->outputImage << std::endl;
            for(int i = 0; i < numValues; ++i)
			{
				std::cout << "Band " <<  i+1 << ":\t" << this->aX[i] << this->bX[i] << this->cX[i] << std::endl;
			}
        }
		else
		{
			throw rsgis::RSGISException("RSGISExeImageCalibration does not know this option");
		}
	}
}

void RSGISExeImageCalibration::help()
{
	std::cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << std::endl;
	std::cout << "" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalibration\" option=\"landsatradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image1\" band=\"int\" [sensorband=\"std::string\" | lmin=\"float\" lmax=\"float\" qcalmin=\"float\" qcalmax=\"float\"] />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image2\" band=\"int\" [sensorband=\"std::string\" | lmin=\"float\" lmax=\"float\" qcalmin=\"float\" qcalmax=\"float\"]/>" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image3\" band=\"int\" [sensorband=\"std::string\" | lmin=\"float\" lmax=\"float\" qcalmin=\"float\" qcalmax=\"float\"]/>" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalibration\" option=\"spotradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image1\" band=\"int\"gain=\"float\" />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image2\" band=\"int\"gain=\"float\" />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image3\" band=\"int\"gain=\"float\" />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalibration\" option=\"ikonosradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image1\" band=\"int\" [sensorband=\"std::string\" | calcoef=\"float\" bandwidth=\"float\" ] />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image2\" band=\"int\" [sensorband=\"std::string\" | calcoef=\"float\" bandwidth=\"float\" ] />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image3\" band=\"int\" [sensorband=\"std::string\" | calcoef=\"float\" bandwidth=\"float\" ] />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalibration\" option=\"asterradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image1\" band=\"int\" [sensorband=\"std::string\" | coefficient=\"float\" ] />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image2\" band=\"int\" [sensorband=\"std::string\" | coefficient=\"float\" ] />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image3\" band=\"int\" [sensorband=\"std::string\" | coefficient=\"float\" ] />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalibration\" option=\"irsradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image1\" band=\"int\" [sensorband=\"std::string\" | lmin=\"float\" lmax=\"float\" qcalmin=\"float\" qcalmax=\"float\"] />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image2\" band=\"int\" [sensorband=\"std::string\" | lmin=\"float\" lmax=\"float\" qcalmin=\"float\" qcalmax=\"float\"]/>" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image3\" band=\"int\" [sensorband=\"std::string\" | lmin=\"float\" lmax=\"float\" qcalmin=\"float\" qcalmax=\"float\"]/>" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalibration\" option=\"quickbird16bitradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image1\" band=\"int\" [sensorband=\"std::string\" | calfactor=\"float\" bandintegrate=\"float\" ] />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image2\" band=\"int\" [sensorband=\"std::string\" | calfactor=\"float\" bandintegrate=\"float\" ] />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image3\" band=\"int\" [sensorband=\"std::string\" | calfactor=\"float\" bandintegrate=\"float\" ] />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalibration\" option=\"quickbird8bitradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image1\" band=\"int\" [sensorband=\"std::string\" | calfactor=\"float\" bandintegrate=\"float\" k=\"float\" ] />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image2\" band=\"int\" [sensorband=\"std::string\" | calfactor=\"float\" bandintegrate=\"float\" k=\"float\" ] />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image3\" band=\"int\" [sensorband=\"std::string\" | calfactor=\"float\" bandintegrate=\"float\" k=\"float\" ] />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalibration\" option=\"worldview2radcal\" output=\"image_out.env\" format=\"GDAL Format\">" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image1\" band=\"int\" [sensorband=\"std::string\" | calfactor=\"float\" bandintegrate=\"float\" ] />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image2\" band=\"int\" [sensorband=\"std::string\" | calfactor=\"float\" bandintegrate=\"float\" ] />" << std::endl;
    std::cout << "    <rsgis:band name=\"std::string\" image=\"image3\" band=\"int\" [sensorband=\"std::string\" | calfactor=\"float\" bandintegrate=\"float\" ] />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalibration\" option=\"topatmosrefl\" input=\"image_in.env\" output=\"image_out.env\" format=\"GDAL Format\" scaleFactor=\"float\" [julianday=\"int\" | day=\"int\" month=\"int\" year=\"int\"] [zenith=\"float\" | elevation=\"float\"] >" << std::endl;
    std::cout << "    <rsgis:band [sensorband=\"std::string\" | irradiance=\"float\"] />" << std::endl;
    std::cout << "    <rsgis:band [sensorband=\"std::string\" | irradiance=\"float\"] />" << std::endl;
    std::cout << "    <rsgis:band [sensorband=\"std::string\" | irradiance=\"float\"] />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalibration\" option=\"apply6s\" input=\"image_in.env\" output=\"image_out.env\" scaleFactor=\"float\" format=\"GDAL Format\" >" << std::endl;
    std::cout << "    <rsgis:band band=\"int\" ax=\"float\" bx=\"float\" cx=\"float\" />" << std::endl;
    std::cout << "    <rsgis:band band=\"int\" ax=\"float\" bx=\"float\" cx=\"float\" />" << std::endl;
    std::cout << "    <rsgis:band band=\"int\" ax=\"float\" bx=\"float\" cx=\"float\" />" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "<rsgis:command algor=\"imagecalibration\" option=\"apply6s\" input=\"image_in.env\" output=\"image_out.env\" format=\"GDAL Format\" scaleFactor=\"float\" inputDEM=\"in_dem.env\">" << std::endl;
    std::cout << "    <rsgis:band band=\"int\" >" << std::endl;
    std::cout << "          <rsgis:elevation elevationUpper=\"float\" ax=\"float\" bx=\"float\" cx=\"float\"  />" << std::endl;
    std::cout << "          <rsgis:elevation elevationUpper=\"float\" ax=\"float\" bx=\"float\" cx=\"float\"  />" << std::endl;
    std::cout << "    </rsgls:band>" << std::endl;
    std::cout << "    <rsgis:band band=\"int\" >" << std::endl;
    std::cout << "          <rsgis:elevation elevationUpper=\"float\" ax=\"float\" bx=\"float\" cx=\"float\"  />" << std::endl;
    std::cout << "          <rsgis:elevation elevationUpper=\"float\" ax=\"float\" bx=\"float\" cx=\"float\"  />" << std::endl;
    std::cout << "    </rsgls:band>" << std::endl;
    std::cout << "</rsgis:command>" << std::endl;
	std::cout << "</rsgis:commands>\n";
}

std::string RSGISExeImageCalibration::getDescription()
{
	return "Provides a set of tools for calibrating Remotely Sensed image data.";
}

std::string RSGISExeImageCalibration::getXMLSchema()
{
	return "NOT PROVIDED!";
}

RSGISExeImageCalibration::~RSGISExeImageCalibration()
{
	
}
    
    
}
    
