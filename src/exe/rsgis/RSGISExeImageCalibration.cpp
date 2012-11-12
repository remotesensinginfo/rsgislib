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

RSGISExeImageCalibration::RSGISExeImageCalibration() : RSGISAlgorithmParameters()
{
	this->algorithm = "imagecalibration";
}

RSGISAlgorithmParameters* RSGISExeImageCalibration::getInstance()
{
	return new RSGISExeImageCalibration();
}

void RSGISExeImageCalibration::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;	
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *optionLandsatRadCal = XMLString::transcode("landsatradcal");
    XMLCh *optionTopAtmosRefl = XMLString::transcode("topatmosrefl");
    XMLCh *optionSPOTRadCal = XMLString::transcode("spotradcal");
    XMLCh *optionIkonosRadCal = XMLString::transcode("ikonosradcal");
    XMLCh *optionASTERRadCal = XMLString::transcode("asterradcal");
    XMLCh *optionIRSRadCal = XMLString::transcode("irsradcal");
    XMLCh *optionQuickbird16bitRadCal = XMLString::transcode("quickbird16bitradcal");
    XMLCh *optionQuickbird8bitRadCal = XMLString::transcode("quickbird8bitradcal");
    XMLCh *optionWorldView2RadCal = XMLString::transcode("worldview2radcal");
    XMLCh *optionApply6S = XMLString::transcode("apply6s");
    
	try
	{
		const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
		if(!XMLString::equals(algorName, algorNameEle))
		{
			throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
		}
        
        // Set output image format (defaults to ENVI)
		this->imageFormat = "ENVI";
		XMLCh *formatXMLStr = XMLString::transcode("format");
		if(argElement->hasAttribute(formatXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
			this->imageFormat = string(charValue);
			XMLString::release(&charValue);
		}
		XMLString::release(&formatXMLStr);
        
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
		if(XMLString::equals(optionLandsatRadCal, optionXML))
		{
            this->option = landsatradcal;
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

            
            DOMNodeList *bandsNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            cout << "Found " << this->numBands << " Bands listed.\n";
            
            DOMElement *bandElement = NULL;
            landsatRadGainOffs = new LandsatRadianceGainsOffsets[numBands];
            inputImages = new string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->landsatRadGainOffs[i].bandName = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->landsatRadGainOffs[i].band = mathUtils.strtounsignedint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    string sensorBand = string(charValue);
                    if(sensorBand == "LMSS1_B1")
                    {
                        this->landsatRadGainOffs[i].lMin = 0;
                        this->landsatRadGainOffs[i].lMax = 248;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS1_B2")
                    {
                        this->landsatRadGainOffs[i].lMin = 0;
                        this->landsatRadGainOffs[i].lMax = 200;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS1_B3")
                    {
                        this->landsatRadGainOffs[i].lMin = 0;
                        this->landsatRadGainOffs[i].lMax = 176;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS1_B4")
                    {
                        this->landsatRadGainOffs[i].lMin = 0;
                        this->landsatRadGainOffs[i].lMax = 153;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS2_B1")
                    {
                        this->landsatRadGainOffs[i].lMin = 8;
                        this->landsatRadGainOffs[i].lMax = 263;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS2_B2")
                    {
                        this->landsatRadGainOffs[i].lMin = 6;
                        this->landsatRadGainOffs[i].lMax = 176;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS2_B3")
                    {
                        this->landsatRadGainOffs[i].lMin = 6;
                        this->landsatRadGainOffs[i].lMax = 152;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS2_B4")
                    {
                        this->landsatRadGainOffs[i].lMin = 3.66667;
                        this->landsatRadGainOffs[i].lMax = 130.333;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS3_B1")
                    {
                        this->landsatRadGainOffs[i].lMin = 4;
                        this->landsatRadGainOffs[i].lMax = 259;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS3_B2")
                    {
                        this->landsatRadGainOffs[i].lMin = 3;
                        this->landsatRadGainOffs[i].lMax = 179;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS3_B3")
                    {
                        this->landsatRadGainOffs[i].lMin = 3;
                        this->landsatRadGainOffs[i].lMax = 149;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS3_B4")
                    {
                        this->landsatRadGainOffs[i].lMin = 1;
                        this->landsatRadGainOffs[i].lMax = 128;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS4_B1")
                    {
                        this->landsatRadGainOffs[i].lMin = 4;
                        this->landsatRadGainOffs[i].lMax = 238;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS4_B2")
                    {
                        this->landsatRadGainOffs[i].lMin = 4;
                        this->landsatRadGainOffs[i].lMax = 164;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS4_B3")
                    {
                        this->landsatRadGainOffs[i].lMin = 5;
                        this->landsatRadGainOffs[i].lMax = 142;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS4_B4")
                    {
                        this->landsatRadGainOffs[i].lMin = 4;
                        this->landsatRadGainOffs[i].lMax = 116;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS5_B1")
                    {
                        this->landsatRadGainOffs[i].lMin = 3;
                        this->landsatRadGainOffs[i].lMax = 268;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS5_B2")
                    {
                        this->landsatRadGainOffs[i].lMin = 3;
                        this->landsatRadGainOffs[i].lMax = 179;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS5_B3")
                    {
                        this->landsatRadGainOffs[i].lMin = 5;
                        this->landsatRadGainOffs[i].lMax = 148;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }
                    else if(sensorBand == "LMSS5_B4")
                    {
                        this->landsatRadGainOffs[i].lMin = 3;
                        this->landsatRadGainOffs[i].lMax = 123;
                        this->landsatRadGainOffs[i].qCalMin = 0;
                        this->landsatRadGainOffs[i].qCalMax = 127;
                    }                    
                    else if(sensorBand == "LTM5_B1_Pre1992")
                    {
                        this->landsatRadGainOffs[i].lMin = -1.52;
                        this->landsatRadGainOffs[i].lMax = 169;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B1_Post1992")
                    {
                        this->landsatRadGainOffs[i].lMin = -1.52;
                        this->landsatRadGainOffs[i].lMax = 169;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B1_Post1992")
                    {
                        this->landsatRadGainOffs[i].lMin = -1.52;
                        this->landsatRadGainOffs[i].lMax = 193;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B2_Pre1992")
                    {
                        this->landsatRadGainOffs[i].lMin = -2.84;
                        this->landsatRadGainOffs[i].lMax = 333;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B2_Post1992")
                    {
                        this->landsatRadGainOffs[i].lMin = -2.84;
                        this->landsatRadGainOffs[i].lMax = 365;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B3")
                    {
                        this->landsatRadGainOffs[i].lMin = -1.17;
                        this->landsatRadGainOffs[i].lMax = 264;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B4")
                    {
                        this->landsatRadGainOffs[i].lMin = -1.51;
                        this->landsatRadGainOffs[i].lMax = 221;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B5")
                    {
                        this->landsatRadGainOffs[i].lMin = -0.37;
                        this->landsatRadGainOffs[i].lMax = 30.2;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B6")
                    {
                        this->landsatRadGainOffs[i].lMin = 1.238;
                        this->landsatRadGainOffs[i].lMax = 15.303;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LTM5_B7")
                    {
                        this->landsatRadGainOffs[i].lMin = -0.15;
                        this->landsatRadGainOffs[i].lMax = 16.6;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B1")
                    {
                        this->landsatRadGainOffs[i].lMin = -6.2;
                        this->landsatRadGainOffs[i].lMax = 191.6;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B2")
                    {
                        this->landsatRadGainOffs[i].lMin = -6.4;
                        this->landsatRadGainOffs[i].lMax = 196.5;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B3")
                    {
                        this->landsatRadGainOffs[i].lMin = -5;
                        this->landsatRadGainOffs[i].lMax = 152.9;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B4")
                    {
                        this->landsatRadGainOffs[i].lMin = -5.1;
                        this->landsatRadGainOffs[i].lMax = 241.1;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B5")
                    {
                        this->landsatRadGainOffs[i].lMin = -1;
                        this->landsatRadGainOffs[i].lMax = 31.06;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B61")
                    {
                        this->landsatRadGainOffs[i].lMin = 0;
                        this->landsatRadGainOffs[i].lMax = 17.04;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B62")
                    {
                        this->landsatRadGainOffs[i].lMin = 3.2;
                        this->landsatRadGainOffs[i].lMax = 12.65;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_B7")
                    {
                        this->landsatRadGainOffs[i].lMin = -0.35;
                        this->landsatRadGainOffs[i].lMax = 10.8;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else if(sensorBand == "LETM7_PAN")
                    {
                        this->landsatRadGainOffs[i].lMin = -4.7;
                        this->landsatRadGainOffs[i].lMax = 243.1;
                        this->landsatRadGainOffs[i].qCalMin = 1;
                        this->landsatRadGainOffs[i].qCalMax = 255;
                    }
                    else
                    {
                        cout << "sensorBand = " << sensorBand << endl;
                        throw RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    XMLString::release(&charValue);
                }
                else
                {
                    //throw RSGISXMLArgumentsException("No \'sensorband\' attribute was provided.");
                    
                    XMLCh *lMinXMLStr = XMLString::transcode("lmin");
                    if(bandElement->hasAttribute(lMinXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(lMinXMLStr));
                        this->landsatRadGainOffs[i].lMin = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'lmin\' attribute was provided.");
                    }
                    XMLString::release(&lMinXMLStr);
                    
                    XMLCh *lMaxXMLStr = XMLString::transcode("lmax");
                    if(bandElement->hasAttribute(lMaxXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(lMaxXMLStr));
                        this->landsatRadGainOffs[i].lMax = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'lmax\' attribute was provided.");
                    }
                    XMLString::release(&lMaxXMLStr);
                    
                    XMLCh *qCalMinXMLStr = XMLString::transcode("qcalmin");
                    if(bandElement->hasAttribute(qCalMinXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(qCalMinXMLStr));
                        this->landsatRadGainOffs[i].qCalMin = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'qcalmin\' attribute was provided.");
                    }
                    XMLString::release(&qCalMinXMLStr);
                    
                    XMLCh *qCalMaxXMLStr = XMLString::transcode("qcalmax");
                    if(bandElement->hasAttribute(qCalMaxXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(qCalMaxXMLStr));
                        this->landsatRadGainOffs[i].qCalMax = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'qcalmax\' attribute was provided.");
                    }
                    XMLString::release(&qCalMaxXMLStr);
                }
                XMLString::release(&sensorBandXMLStr);	
            }
		}
        else if(XMLString::equals(optionTopAtmosRefl, optionXML))
		{
            this->option = topatmosrefl;
            
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
            
            // Set scaling factor for output images
            this->scaleFactor = 1.0;
            XMLCh *scaleFactorXMLStr = XMLString::transcode("scaleFactor");
            if(argElement->hasAttribute(scaleFactorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(scaleFactorXMLStr));
                this->scaleFactor = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            XMLString::release(&scaleFactorXMLStr);
            
            XMLCh *julianDayXMLStr = XMLString::transcode("julianday");
            if(argElement->hasAttribute(julianDayXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(julianDayXMLStr));
                this->julianDay = mathUtils.strtounsignedint(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                //throw RSGISXMLArgumentsException("No \'julianday\' attribute was provided.");
                unsigned int day = 0;
                unsigned int month = 0;
                unsigned int year = 0;
                XMLCh *dayXMLStr = XMLString::transcode("day");
                if(argElement->hasAttribute(dayXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(dayXMLStr));
                    day = mathUtils.strtounsignedint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'day\' attribute was provided.");
                }
                XMLString::release(&dayXMLStr);
                
                XMLCh *monthXMLStr = XMLString::transcode("month");
                if(argElement->hasAttribute(monthXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(monthXMLStr));
                    month = mathUtils.strtounsignedint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'month\' attribute was provided.");
                }
                XMLString::release(&monthXMLStr);
                
                XMLCh *yearXMLStr = XMLString::transcode("year");
                if(argElement->hasAttribute(yearXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(yearXMLStr));
                    year = mathUtils.strtounsignedint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'year\' attribute was provided.");
                }
                XMLString::release(&yearXMLStr);
                
                
                boost::gregorian::date d(year,month,day);
                julianDay = d.day_of_year();
            }
            XMLString::release(&julianDayXMLStr);
            
            
            XMLCh *zenithXMLStr = XMLString::transcode("zenith");
            if(argElement->hasAttribute(zenithXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(zenithXMLStr));
                this->solarZenith = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elevationXMLStr = XMLString::transcode("elevation");
                if(argElement->hasAttribute(elevationXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elevationXMLStr));
                    this->solarZenith = 90-mathUtils.strtofloat(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'zenith\' or \'elevation\' attribute was provided.");
                }
                XMLString::release(&elevationXMLStr);
            }
            XMLString::release(&zenithXMLStr);
            
            
            
            DOMNodeList *bandsNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            cout << "Found " << this->numBands << " Bands listed.\n";
            
            DOMElement *bandElement = NULL;
            solarIrradiance = new float[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *sensorBandXMLStr = XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    string sensorBand = string(charValue);
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
                    else
                    {
                        cout << "sensorBand = " << sensorBand << endl;
                        throw RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    XMLString::release(&charValue);
                }
                else
                {
                    //throw RSGISXMLArgumentsException("No \'sensorband\' attribute was provided.");
                    
                    XMLCh *irradianceXMLStr = XMLString::transcode("irradiance");
                    if(bandElement->hasAttribute(irradianceXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(irradianceXMLStr));
                        this->solarIrradiance[i] = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'irradiance\' attribute was provided.");
                    }
                    XMLString::release(&irradianceXMLStr);
                    
                }
                XMLString::release(&sensorBandXMLStr);	
            }
        }
        else if(XMLString::equals(optionSPOTRadCal, optionXML))
        {
            this->option = spotradcal;
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
            
            
            DOMNodeList *bandsNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            cout << "Found " << this->numBands << " Bands listed.\n";
            
            DOMElement *bandElement = NULL;
            spotRadGainOffs = new SPOTRadianceGainsOffsets[numBands];
            inputImages = new string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->spotRadGainOffs[i].bandName = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->spotRadGainOffs[i].band = mathUtils.strtounsignedint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                XMLString::release(&bandIdxXMLStr);
                
                XMLCh *gainXMLStr = XMLString::transcode("gain");
				if(bandElement->hasAttribute(gainXMLStr))
				{
					char *charValue = XMLString::transcode(bandElement->getAttribute(gainXMLStr));
					this->spotRadGainOffs[i].gain = mathUtils.strtofloat(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'gain\' attribute was provided.");
				}
				XMLString::release(&gainXMLStr);	
            }
        }
        else if(XMLString::equals(optionIkonosRadCal, optionXML))
        {
            this->option = ikonosradcal;
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
            
            
            DOMNodeList *bandsNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            cout << "Found " << this->numBands << " Bands listed.\n";
            
            DOMElement *bandElement = NULL;
            ikonosRadGainOffs = new IkonosRadianceGainsOffsets[numBands];
            inputImages = new string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->ikonosRadGainOffs[i].bandName = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->ikonosRadGainOffs[i].band = mathUtils.strtounsignedint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    string sensorBand = string(charValue);
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
                        cout << "sensorBand = " << sensorBand << endl;
                        throw RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    XMLString::release(&charValue);
                }
                else
                {
                    XMLCh *calCoefXMLStr = XMLString::transcode("calcoef");
                    if(bandElement->hasAttribute(calCoefXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(calCoefXMLStr));
                        this->ikonosRadGainOffs[i].calCoef = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'calcoef\' attribute was provided.");
                    }
                    XMLString::release(&calCoefXMLStr);
                    
                    XMLCh *bandwidthXMLStr = XMLString::transcode("bandwidth");
                    if(bandElement->hasAttribute(bandwidthXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(bandwidthXMLStr));
                        this->ikonosRadGainOffs[i].bandwidth = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'bandwidth\' attribute was provided.");
                    }
                    XMLString::release(&bandwidthXMLStr);
                }
                XMLString::release(&sensorBandXMLStr);
            }
        }
        else if(XMLString::equals(optionASTERRadCal, optionXML))
        {
            this->option = asterradcal;
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
            
            
            DOMNodeList *bandsNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            cout << "Found " << this->numBands << " Bands listed.\n";
            
            DOMElement *bandElement = NULL;
            asterRadGainOffs = new ASTERRadianceGainsOffsets[numBands];
            inputImages = new string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->asterRadGainOffs[i].bandName = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->asterRadGainOffs[i].band = mathUtils.strtounsignedint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    string sensorBand = string(charValue);
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
                        cout << "sensorBand = " << sensorBand << endl;
                        throw RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    XMLString::release(&charValue);
                }
                else
                {
                    XMLCh *coefficientXMLStr = XMLString::transcode("coefficient");
                    if(bandElement->hasAttribute(coefficientXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(coefficientXMLStr));
                        this->asterRadGainOffs[i].unitConCoef = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'coefficient\' attribute was provided.");
                    }
                    XMLString::release(&coefficientXMLStr);
                }
                XMLString::release(&sensorBandXMLStr);
            }
        }
        else if(XMLString::equals(optionIRSRadCal, optionXML))
        {
            this->option = irsradcal;
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
            
            
            DOMNodeList *bandsNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            cout << "Found " << this->numBands << " Bands listed.\n";
            
            DOMElement *bandElement = NULL;
            irsRadGainOffs = new IRSRadianceGainsOffsets[numBands];
            inputImages = new string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->irsRadGainOffs[i].bandName = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->irsRadGainOffs[i].band = mathUtils.strtounsignedint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    string sensorBand = string(charValue);
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
                        cout << "sensorBand = " << sensorBand << endl;
                        throw RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    XMLString::release(&charValue);
                }
                else
                {
                    XMLCh *lMinXMLStr = XMLString::transcode("lmin");
                    if(bandElement->hasAttribute(lMinXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(lMinXMLStr));
                        this->irsRadGainOffs[i].lMin = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'lmin\' attribute was provided.");
                    }
                    XMLString::release(&lMinXMLStr);
                    
                    XMLCh *lMaxXMLStr = XMLString::transcode("lmax");
                    if(bandElement->hasAttribute(lMaxXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(lMaxXMLStr));
                        this->irsRadGainOffs[i].lMax = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'lmax\' attribute was provided.");
                    }
                    XMLString::release(&lMaxXMLStr);
                    
                    XMLCh *qCalMinXMLStr = XMLString::transcode("qcalmin");
                    if(bandElement->hasAttribute(qCalMinXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(qCalMinXMLStr));
                        this->irsRadGainOffs[i].qCalMin = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'qcalmin\' attribute was provided.");
                    }
                    XMLString::release(&qCalMinXMLStr);
                    
                    XMLCh *qCalMaxXMLStr = XMLString::transcode("qcalmax");
                    if(bandElement->hasAttribute(qCalMaxXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(qCalMaxXMLStr));
                        this->irsRadGainOffs[i].qCalMax = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'qcalmax\' attribute was provided.");
                    }
                    XMLString::release(&qCalMaxXMLStr);
                }
                XMLString::release(&sensorBandXMLStr);
            }
        }
        else if(XMLString::equals(optionQuickbird16bitRadCal, optionXML))
        {
            this->option = quickbird16bitradcal;
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
            
            
            DOMNodeList *bandsNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            cout << "Found " << this->numBands << " Bands listed.\n";
            
            DOMElement *bandElement = NULL;
            quickbird16bitRadGainOffs = new Quickbird16bitRadianceGainsOffsets[numBands];
            inputImages = new string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->quickbird16bitRadGainOffs[i].bandName = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->quickbird16bitRadGainOffs[i].band = mathUtils.strtounsignedint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    string sensorBand = string(charValue);
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
                        cout << "sensorBand = " << sensorBand << endl;
                        throw RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    XMLString::release(&charValue);
                }
                else
                {
                    XMLCh *calFactorXMLStr = XMLString::transcode("calfactor");
                    if(bandElement->hasAttribute(calFactorXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(calFactorXMLStr));
                        this->quickbird16bitRadGainOffs[i].calFactor = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'calfactor\' attribute was provided.");
                    }
                    XMLString::release(&calFactorXMLStr);
                    
                    XMLCh *bandIntegrateXMLStr = XMLString::transcode("bandintegrate");
                    if(bandElement->hasAttribute(bandIntegrateXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(bandIntegrateXMLStr));
                        this->quickbird16bitRadGainOffs[i].bandIntegrate = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'bandintegrate\' attribute was provided.");
                    }
                    XMLString::release(&bandIntegrateXMLStr);
                }
                XMLString::release(&sensorBandXMLStr);
            }
        }
        else if(XMLString::equals(optionQuickbird8bitRadCal, optionXML))
        {
            this->option = quickbird8bitradcal;
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
            
            
            DOMNodeList *bandsNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            cout << "Found " << this->numBands << " Bands listed.\n";
            
            DOMElement *bandElement = NULL;
            quickbird8bitRadGainOffs = new Quickbird8bitRadianceGainsOffsets[numBands];
            inputImages = new string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->quickbird8bitRadGainOffs[i].bandName = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->quickbird8bitRadGainOffs[i].band = mathUtils.strtounsignedint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    string sensorBand = string(charValue);
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
                        cout << "sensorBand = " << sensorBand << endl;
                        throw RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    XMLString::release(&charValue);
                }
                else
                {
                    XMLCh *calFactorXMLStr = XMLString::transcode("calfactor");
                    if(bandElement->hasAttribute(calFactorXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(calFactorXMLStr));
                        this->quickbird8bitRadGainOffs[i].calFactor = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'calfactor\' attribute was provided.");
                    }
                    XMLString::release(&calFactorXMLStr);
                    
                    XMLCh *kXMLStr = XMLString::transcode("k");
                    if(bandElement->hasAttribute(kXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(kXMLStr));
                        this->quickbird8bitRadGainOffs[i].k = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'k\' attribute was provided.");
                    }
                    XMLString::release(&kXMLStr);
                    
                    XMLCh *bandIntegrateXMLStr = XMLString::transcode("bandintegrate");
                    if(bandElement->hasAttribute(bandIntegrateXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(bandIntegrateXMLStr));
                        this->quickbird8bitRadGainOffs[i].bandIntegrate = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'bandintegrate\' attribute was provided.");
                    }
                    XMLString::release(&bandIntegrateXMLStr);
                }
                XMLString::release(&sensorBandXMLStr);
            }
        }
        else if(XMLString::equals(optionWorldView2RadCal, optionXML))
        {
            this->option = worldview2radcal;
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
            
            
            DOMNodeList *bandsNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:band"));
            this->numBands = bandsNodesList->getLength();		
            
            cout << "Found " << this->numBands << " Bands listed.\n";
            
            DOMElement *bandElement = NULL;
            worldview2RadGainOffs = new WorldView2RadianceGainsOffsets[numBands];
            inputImages = new string[numBands];
            
            for(unsigned int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<DOMElement*>(bandsNodesList->item(i));
                
                XMLCh *bandNameXMLStr = XMLString::transcode("name");
                if(bandElement->hasAttribute(bandNameXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandNameXMLStr));
                    this->worldview2RadGainOffs[i].bandName = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                XMLString::release(&bandNameXMLStr);
                
                XMLCh *bandImageXMLStr = XMLString::transcode("image");
                if(bandElement->hasAttribute(bandImageXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandImageXMLStr));
                    this->inputImages[i] = string(charValue);
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
                }
                XMLString::release(&bandImageXMLStr);
                
                XMLCh *bandIdxXMLStr = XMLString::transcode("band");
                if(bandElement->hasAttribute(bandIdxXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(bandIdxXMLStr));
                    this->worldview2RadGainOffs[i].band = mathUtils.strtounsignedint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                XMLString::release(&bandIdxXMLStr);
                
                XMLCh *sensorBandXMLStr = XMLString::transcode("sensorband");
                if(bandElement->hasAttribute(sensorBandXMLStr))
                {
                    char *charValue = XMLString::transcode(bandElement->getAttribute(sensorBandXMLStr));
                    string sensorBand = string(charValue);
                    if(sensorBand == "WV2_Coast")
                    {
                        this->worldview2RadGainOffs[i].calFactor = 0;
                        this->worldview2RadGainOffs[i].bandIntegrate = 0.0473;
                    }
                    else if(sensorBand == "WV2_Blue")
                    {
                        this->worldview2RadGainOffs[i].calFactor = 0;
                        this->worldview2RadGainOffs[i].bandIntegrate = 0.0534;
                    }
                    else if(sensorBand == "WV2_Green")
                    {
                        this->worldview2RadGainOffs[i].calFactor = 0;
                        this->worldview2RadGainOffs[i].bandIntegrate = 0.0630;
                    }
                    else if(sensorBand == "WV2_Yellow")
                    {
                        this->worldview2RadGainOffs[i].calFactor = 0;
                        this->worldview2RadGainOffs[i].bandIntegrate = 0.0374;
                    }
                    else if(sensorBand == "WV2_Red")
                    {
                        this->worldview2RadGainOffs[i].calFactor = 0;
                        this->worldview2RadGainOffs[i].bandIntegrate = 0.0574;
                    }
                    else if(sensorBand == "WV2_Red_Edge")
                    {
                        this->worldview2RadGainOffs[i].calFactor = 0;
                        this->worldview2RadGainOffs[i].bandIntegrate = 0.393;
                    }
                    else if(sensorBand == "WV2_NIR1")
                    {
                        this->worldview2RadGainOffs[i].calFactor = 0;
                        this->worldview2RadGainOffs[i].bandIntegrate = 0.989;
                    }
                    else if(sensorBand == "WV2_NIR2")
                    {
                        this->worldview2RadGainOffs[i].calFactor = 0;
                        this->worldview2RadGainOffs[i].bandIntegrate = 0.996;
                    }
                    else
                    {
                        cout << "sensorBand = " << sensorBand << endl;
                        throw RSGISXMLArgumentsException("Do not recognised sensorBand.");
                    }
                    XMLString::release(&charValue);
                }
                else
                {
                    XMLCh *calFactorXMLStr = XMLString::transcode("calfactor");
                    if(bandElement->hasAttribute(calFactorXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(calFactorXMLStr));
                        this->worldview2RadGainOffs[i].calFactor = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'calfactor\' attribute was provided.");
                    }
                    XMLString::release(&calFactorXMLStr);
                    
                    XMLCh *bandIntegrateXMLStr = XMLString::transcode("bandintegrate");
                    if(bandElement->hasAttribute(bandIntegrateXMLStr))
                    {
                        char *charValue = XMLString::transcode(bandElement->getAttribute(bandIntegrateXMLStr));
                        this->worldview2RadGainOffs[i].bandIntegrate = mathUtils.strtofloat(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISXMLArgumentsException("No \'bandintegrate\' attribute was provided.");
                    }
                    XMLString::release(&bandIntegrateXMLStr);
                }
                XMLString::release(&sensorBandXMLStr);
            }
        }
        else if(XMLString::equals(optionApply6S, optionXML))
		{
            this->option = apply6s;
            this->useTopo6S = false;
			
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
            
            // Set scaling factor for output images
            this->scaleFactor = 1.0;
            XMLCh *scaleFactorXMLStr = XMLString::transcode("scaleFactor");
            if(argElement->hasAttribute(scaleFactorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(scaleFactorXMLStr));
                this->scaleFactor = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
            }
            XMLString::release(&scaleFactorXMLStr);
            
			XMLCh *demXMLStr = XMLString::transcode("inputDEM");
            if(argElement->hasAttribute(demXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(demXMLStr));
                this->inputDEM = string(charValue);
				this->useTopo6S = true;
                XMLString::release(&charValue);
            }
            else
            {
                cout << "\tNo DEM provided, using the same coefficients for all elevations" << endl;
            }
            XMLString::release(&demXMLStr);
			
            DOMNodeList *bandsNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:band"));
            this->numValues = bandsNodesList->getLength();		
            this->numElevation = 0;
            cout << "Found " << this->numValues << " Bands listed.\n";
            
            DOMElement *bandElement = NULL;
			DOMElement *elevationElement = NULL;
            this->aX = new float*[numValues];
            this->bX = new float*[numValues];
            this->cX = new float*[numValues];
            this->imageBands = new unsigned int[numValues];
            
            for(int i = 0; i < numValues; i++)
            {
                bandElement = static_cast<DOMElement*>(bandsNodesList->item(i));
				
				XMLCh *bandXMLStr = XMLString::transcode("band");
				if(bandElement->hasAttribute(bandXMLStr))
				{
					char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
					this->imageBands[i] = mathUtils.strtounsignedint(string(charValue))-1;
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
				}
				XMLString::release(&bandXMLStr);
				
				if (!this->useTopo6S) 
				{
					this->aX[i] = new float[this->numElevation+1];
					this->bX[i] = new float[this->numElevation+1];
					this->cX[i] = new float[this->numElevation+1];
					this->elevationThreash = NULL;
					
					XMLCh *aXXMLStr = XMLString::transcode("ax");
					if(bandElement->hasAttribute(aXXMLStr))
					{
						char *charValue = XMLString::transcode(bandElement->getAttribute(aXXMLStr));
						this->aX[i][0] = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'ax\' attribute was provided.");
					}
					XMLString::release(&aXXMLStr);
					
					XMLCh *bXXMLStr = XMLString::transcode("bx");
					if(bandElement->hasAttribute(bXXMLStr))
					{
						char *charValue = XMLString::transcode(bandElement->getAttribute(bXXMLStr));
						this->bX[i][0] = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'bx\' attribute was provided.");
					}
					XMLString::release(&bXXMLStr);
					
					XMLCh *cXXMLStr = XMLString::transcode("cx");
					if(bandElement->hasAttribute(cXXMLStr))
					{
						char *charValue = XMLString::transcode(bandElement->getAttribute(cXXMLStr));
						this->cX[i][0] = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'cx\' attribute was provided.");
					}
					XMLString::release(&cXXMLStr);
				}
				
				else
				{
					DOMNodeList *elevationNodesList = bandElement->getElementsByTagName(XMLString::transcode("rsgis:elevation"));
					if (i == 0) // Set number of elevation values based on first band
					{
						this->numElevation = elevationNodesList->getLength();
						cout << "numElevation = " << this->numElevation << endl;
					}
					else 
					{
						 if(elevationNodesList->getLength() != this->numElevation)
						 {
							 throw RSGISXMLArgumentsException("The number of elevation threasholds is not equal for all bands");
						 }
					}
					
					this->aX[i] = new float[this->numElevation];
					this->bX[i] = new float[this->numElevation];
					this->cX[i] = new float[this->numElevation];
					this->elevationThreash = new float[this->numElevation];
					
					for(unsigned int et = 0; et < this->numElevation; et++)
					{
						elevationElement = static_cast<DOMElement*>(elevationNodesList->item(et));
						
						if (i == 0) // Seting elevation threasholds using first band
						{
							XMLCh *elevationUpperXXMLStr = XMLString::transcode("elevationUpper");
							if(elevationElement->hasAttribute(elevationUpperXXMLStr))
							{
								char *charValue = XMLString::transcode(elevationElement->getAttribute(elevationUpperXXMLStr));
								this->elevationThreash[et] = mathUtils.strtofloat(string(charValue));
								XMLString::release(&charValue);
							}
							else
							{
								throw RSGISXMLArgumentsException("No \'elevationUpper\' attribute was provided.");
							}
							XMLString::release(&elevationUpperXXMLStr);
						}
						
						XMLCh *aXXMLStr = XMLString::transcode("ax");
						if(elevationElement->hasAttribute(aXXMLStr))
						{
							char *charValue = XMLString::transcode(elevationElement->getAttribute(aXXMLStr));
							this->aX[i][et] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'ax\' attribute was provided for elevtion threashold.");
						}
						XMLString::release(&aXXMLStr);
						
						XMLCh *bXXMLStr = XMLString::transcode("bx");
						if(elevationElement->hasAttribute(bXXMLStr))
						{
							char *charValue = XMLString::transcode(elevationElement->getAttribute(bXXMLStr));
							this->bX[i][et] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'bx\' attribute was provided for elevtion threashold.");
						}
						XMLString::release(&bXXMLStr);
						
						XMLCh *cXXMLStr = XMLString::transcode("cx");
						if(elevationElement->hasAttribute(cXXMLStr))
						{
							char *charValue = XMLString::transcode(elevationElement->getAttribute(cXXMLStr));
							this->cX[i][et] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'cx\' attribute was provided for elevtion threashold.");
						}
						XMLString::release(&cXXMLStr);
						
						
					}

					
				}
				
            }
        }
		else 
		{
			string message = "RSGISExeImageCalibration did not recognise option " + string(XMLString::transcode(optionXML));
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
	XMLString::release(&optionLandsatRadCal);
    XMLString::release(&optionTopAtmosRefl);
    XMLString::release(&optionSPOTRadCal);
    XMLString::release(&optionIkonosRadCal);
    XMLString::release(&optionASTERRadCal);
    XMLString::release(&optionIRSRadCal);
    XMLString::release(&optionQuickbird16bitRadCal);
    XMLString::release(&optionQuickbird8bitRadCal);
    XMLString::release(&optionWorldView2RadCal);
    XMLString::release(&optionApply6S);
	
	parsed = true; // if all successful, it is parsed
}

void RSGISExeImageCalibration::runAlgorithm() throw(RSGISException)
{
	cout.precision(10);
	
	if(!parsed)
	{
		throw RSGISException("Before running the parameters must be retrieved");
	}
	else
	{
		if(this->option == RSGISExeImageCalibration::landsatradcal)
		{
			cout << "This command converts Landsat from digital number values to at sensor radiance.\n";
			cout << "Output Image: " << this->outputImage << endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				cout << i << ")\t name = " << landsatRadGainOffs[i].bandName << " band = " << landsatRadGainOffs[i].band << endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISLandsatRadianceCalibration *radianceCalibration = NULL;
			RSGISCalcImage *calcImage = NULL;
            string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					cout << "Opening: " << this->inputImages[i] << endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->landsatRadGainOffs[i].band > numRasterBands)
					{
						throw RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->landsatRadGainOffs[i].band = totalNumRasterBands + this->landsatRadGainOffs[i].band-1;
					outBandNames[i] = this->landsatRadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new RSGISLandsatRadianceCalibration(numBands, this->landsatRadGainOffs);
				
				calcImage = new RSGISCalcImage(radianceCalibration, "", true);
				calcImage->calcImage(datasets, this->numBands, this->outputImage, true, outBandNames,this->imageFormat);
				
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					GDALClose(datasets[i]);
				}
				delete[] datasets;
				delete[] landsatRadGainOffs;
				delete[] outBandNames;
				
				delete radianceCalibration;
				delete calcImage;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
        else if(this->option == RSGISExeImageCalibration::spotradcal)
        {
            cout << "This command converts SPOT from digital number values to at sensor radiance.\n";
			cout << "Output Image: " << this->outputImage << endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				cout << i << ")\t name = " << spotRadGainOffs[i].bandName << " band = " << spotRadGainOffs[i].band << endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISSPOTRadianceCalibration *radianceCalibration = NULL;
			RSGISCalcImage *calcImage = NULL;
            string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					cout << "Opening: " << this->inputImages[i] << endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->spotRadGainOffs[i].band > numRasterBands)
					{
						throw RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->spotRadGainOffs[i].band = totalNumRasterBands + this->spotRadGainOffs[i].band-1;
					outBandNames[i] = this->spotRadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new RSGISSPOTRadianceCalibration(numBands, this->spotRadGainOffs);
				
				calcImage = new RSGISCalcImage(radianceCalibration, "", true);
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
			catch(RSGISException e)
			{
				throw e;
			}

        }
        else if(this->option == RSGISExeImageCalibration::ikonosradcal)
        {
            cout << "This command converts Ikonos from digital number values to at sensor radiance.\n";
			cout << "Output Image: " << this->outputImage << endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				cout << i << ")\t name = " << ikonosRadGainOffs[i].bandName << " band = " << ikonosRadGainOffs[i].band << endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISIkonosRadianceCalibration *radianceCalibration = NULL;
			RSGISCalcImage *calcImage = NULL;
            string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					cout << "Opening: " << this->inputImages[i] << endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->ikonosRadGainOffs[i].band > numRasterBands)
					{
						throw RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->ikonosRadGainOffs[i].band = totalNumRasterBands + this->ikonosRadGainOffs[i].band-1;
					outBandNames[i] = this->ikonosRadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new RSGISIkonosRadianceCalibration(numBands, this->ikonosRadGainOffs);
				
				calcImage = new RSGISCalcImage(radianceCalibration, "", true);
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
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::asterradcal)
        {
            cout << "This command converts ASTER from digital number values to at sensor radiance.\n";
			cout << "Output Image: " << this->outputImage << endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				cout << i << ")\t name = " << asterRadGainOffs[i].bandName << " band = " << asterRadGainOffs[i].band << endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISASTERRadianceCalibration *radianceCalibration = NULL;
			RSGISCalcImage *calcImage = NULL;
            string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					cout << "Opening: " << this->inputImages[i] << endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->asterRadGainOffs[i].band > numRasterBands)
					{
						throw RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->asterRadGainOffs[i].band = totalNumRasterBands + this->asterRadGainOffs[i].band-1;
					outBandNames[i] = this->asterRadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new RSGISASTERRadianceCalibration(numBands, this->asterRadGainOffs);
				
				calcImage = new RSGISCalcImage(radianceCalibration, "", true);
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
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::irsradcal)
        {
            cout << "This command converts IRS from digital number values to at sensor radiance.\n";
			cout << "Output Image: " << this->outputImage << endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				cout << i << ")\t name = " << irsRadGainOffs[i].bandName << " band = " << irsRadGainOffs[i].band << endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISIRSRadianceCalibration *radianceCalibration = NULL;
			RSGISCalcImage *calcImage = NULL;
            string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					cout << "Opening: " << this->inputImages[i] << endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->irsRadGainOffs[i].band > numRasterBands)
					{
						throw RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->irsRadGainOffs[i].band = totalNumRasterBands + this->irsRadGainOffs[i].band-1;
					outBandNames[i] = this->irsRadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new RSGISIRSRadianceCalibration(numBands, this->irsRadGainOffs);
				
				calcImage = new RSGISCalcImage(radianceCalibration, "", true);
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
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::quickbird16bitradcal)
        {
            cout << "This command converts QuickBird 16bit from digital number values to at sensor radiance.\n";
			cout << "Output Image: " << this->outputImage << endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				cout << i << ")\t name = " << quickbird16bitRadGainOffs[i].bandName << " band = " << quickbird16bitRadGainOffs[i].band << endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISQuickbird16bitRadianceCalibration *radianceCalibration = NULL;
			RSGISCalcImage *calcImage = NULL;
            string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					cout << "Opening: " << this->inputImages[i] << endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->quickbird16bitRadGainOffs[i].band > numRasterBands)
					{
						throw RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->quickbird16bitRadGainOffs[i].band = totalNumRasterBands + this->quickbird16bitRadGainOffs[i].band-1;
					outBandNames[i] = this->quickbird16bitRadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new RSGISQuickbird16bitRadianceCalibration(numBands, this->quickbird16bitRadGainOffs);
				
				calcImage = new RSGISCalcImage(radianceCalibration, "", true);
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
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::quickbird8bitradcal)
        {
            cout << "This command converts QuickBird 8 bit from digital number values to at sensor radiance.\n";
			cout << "Output Image: " << this->outputImage << endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				cout << i << ")\t name = " << quickbird8bitRadGainOffs[i].bandName << " band = " << quickbird8bitRadGainOffs[i].band << endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISQuickbird8bitRadianceCalibration *radianceCalibration = NULL;
			RSGISCalcImage *calcImage = NULL;
            string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					cout << "Opening: " << this->inputImages[i] << endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->quickbird8bitRadGainOffs[i].band > numRasterBands)
					{
						throw RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->quickbird8bitRadGainOffs[i].band = totalNumRasterBands + this->quickbird8bitRadGainOffs[i].band-1;
					outBandNames[i] = this->quickbird8bitRadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new RSGISQuickbird8bitRadianceCalibration(numBands, this->quickbird8bitRadGainOffs);
				
				calcImage = new RSGISCalcImage(radianceCalibration, "", true);
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
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::worldview2radcal)
        {
            cout << "This command converts WorldView2 from digital number values to at sensor radiance.\n";
			cout << "Output Image: " << this->outputImage << endl;
			for(unsigned int i = 0; i < numBands; ++i)
			{
				cout << i << ")\t name = " << worldview2RadGainOffs[i].bandName << " band = " << worldview2RadGainOffs[i].band << endl;
			}
            
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISWorldView2RadianceCalibration *radianceCalibration = NULL;
			RSGISCalcImage *calcImage = NULL;
            string *outBandNames = NULL;
            
			try
			{
				datasets = new GDALDataset*[numBands];
                outBandNames = new string[numBands];
				
				unsigned int numRasterBands = 0;
				unsigned int totalNumRasterBands = 0;
				
				for(unsigned int i = 0; i < this->numBands; ++i)
				{
					cout << "Opening: " << this->inputImages[i] << endl;
					datasets[i] = (GDALDataset *) GDALOpen(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
					
					numRasterBands = datasets[i]->GetRasterCount();
					
					if(this->worldview2RadGainOffs[i].band > numRasterBands)
					{
						throw RSGISImageException("You have specified a band which is not within the image");
					}
					
					this->worldview2RadGainOffs[i].band = totalNumRasterBands + this->worldview2RadGainOffs[i].band-1;
					outBandNames[i] = this->worldview2RadGainOffs[i].bandName;
                    
					totalNumRasterBands += numRasterBands;
				}	
				
				radianceCalibration = new RSGISWorldView2RadianceCalibration(numBands, this->worldview2RadGainOffs);
				
				calcImage = new RSGISCalcImage(radianceCalibration, "", true);
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
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::topatmosrefl)
        {
            cout << "This command converts at sensor radiance to top of atmosphere reflectance.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Scaling factor: " << this->scaleFactor << endl;
            cout << "Julian Day: " << this->julianDay << endl;
            cout << "Solar Zenith: " << this->solarZenith << endl;
            
			for(unsigned int i = 0; i < numBands; ++i)
			{
				cout << "Solar Irradiance for band " <<  i+1 << ":\t" << this->solarIrradiance[i] << endl;
			}
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalculateTopOfAtmosphereReflectance *calcTopAtmosRefl = NULL;
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
				
				unsigned int numRasterBands = datasets[0]->GetRasterCount();
                if(this->numBands != numRasterBands)
                {
                    GDALClose(datasets[0]);
                    delete[] datasets;
                    throw RSGISException("The number of input image bands and solar irradiance values are different.");
                }
                
				
                double solarDistance = 0;
                
                solarDistance = rsgisCalcSolarDistance(this->julianDay);
                
                /*for(int i = 1; i < 366; ++i)
                 {
                 double radiansJulianDay = ((i - 4)*0.9856)*(M_PI/180);
                 cout << "radiansJulianDay = " << radiansJulianDay << endl;
                 cout << "cos(radiansJulianDay) = " << cos(radiansJulianDay) << endl;
                 double distanceTmp = 1 - 0.01672 * cos(radiansJulianDay);
                 cout << "Day " << i << " = " << distanceTmp << endl;
                 cout << "Day " << i << " = " << rsgisCalcSolarDistance(i) << endl;
                 }*/
                
				calcTopAtmosRefl = new RSGISCalculateTopOfAtmosphereReflectance(numRasterBands, solarIrradiance, solarDistance, (solarZenith*(M_PI/180)), this->scaleFactor);
				
				calcImage = new RSGISCalcImage(calcTopAtmosRefl, "", true);
				calcImage->calcImage(datasets, 1, this->outputImage, false, NULL, this->imageFormat, this->outDataType);
				
				
				GDALClose(datasets[0]);
				delete[] datasets;
                delete[] solarIrradiance;
                
				delete calcTopAtmosRefl;
				delete calcImage;
			}
			catch(RSGISException e)
			{
				throw e;
			}
        }
        else if(this->option == RSGISExeImageCalibration::apply6s)
        {
            cout << "This command applies coefficients exported from 6S to a radiance image.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Scale Factor: " << this->scaleFactor << endl;
            cout << "Num Elevations: " << this->numElevation << endl;
			if(this->useTopo6S)
            {
                cout << "Coefficients (for first elevation threashold):" << endl;
            }
			else
            {
                cout << "Coefficients:" << endl;
            }
            for(int i = 0; i < numValues; ++i)
			{
				cout << " Band " <<  this->imageBands[i]+1 << ": " << "ax = " <<this->aX[i][0] << ", bx = " << this->bX[i][0] << ", cx = " << this->cX[i][0] << endl;
			}
            
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISApply6SCoefficients *apply6SCoefficients = NULL;
			RSGISCalcImage *calcImage = NULL;
            
            try
			{
				unsigned int nDatasets = 1;
				if(!this->useTopo6S)
				{
					datasets = new GDALDataset*[1];
					cout << "Open image" << this->inputImage << endl;
					datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
					if(datasets[0] == NULL)
					{
						string message = string("Could not open image ") + this->inputImage;
						throw RSGISImageException(message.c_str());
					}
					
					int numRasterBands = datasets[0]->GetRasterCount();
					if(this->numValues > numRasterBands)
					{
						GDALClose(datasets[0]);
						delete[] datasets;
						throw RSGISException("The number of input image bands is less than the number of values specified.");
					}
					
				}
				else
				{
					nDatasets = 2;
					
					datasets = new GDALDataset*[2];
					
					cout << "Open DEM" << this->inputDEM << endl;
					datasets[0] = (GDALDataset *) GDALOpen(this->inputDEM.c_str(), GA_ReadOnly);
					if(datasets[0] == NULL)
					{
						string message = string("Could not open DEM ") + this->inputDEM;
						throw RSGISImageException(message.c_str());
					}
					
					if(datasets[0]->GetRasterCount() != 1)
					{
						GDALClose(datasets[0]);
						delete[] datasets;
						throw RSGISException("The DEM should only contain one band");
					}
					
					
					cout << "Open image" << this->inputImage << endl;
					datasets[1] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
					if(datasets[1] == NULL)
					{
						string message = string("Could not open image ") + this->inputImage;
						throw RSGISImageException(message.c_str());
					}
					
					int numRasterBands = datasets[1]->GetRasterCount();
					if(this->numValues > numRasterBands)
					{
						GDALClose(datasets[1]);
						delete[] datasets;
						throw RSGISException("The number of input image bands is less than the number of values specified.");
					}
					
				}
				
				apply6SCoefficients = new RSGISApply6SCoefficients(this->numValues, this->imageBands, this->aX, this->bX, this->cX, this->numValues, this->elevationThreash, this->numElevation, this->scaleFactor);
				
				calcImage = new RSGISCalcImage(apply6SCoefficients, "", true);
				calcImage->calcImage(datasets, nDatasets, this->outputImage, false, NULL, this->imageFormat, this->outDataType);
				
				
				GDALClose(datasets[0]);
				if(useTopo6S)
				{
					GDALClose(datasets[1]);
				}
				delete[] datasets;
                
                delete[] imageBands;
                delete[] this->elevationThreash;
				
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
			catch(RSGISException e)
			{
				throw e;
			}
            
        }
		else
		{
			throw RSGISException("RSGISExeImageCalibration does not know this option");
		}
	}
}


void RSGISExeImageCalibration::printParameters()
{
	cout.precision(10);
	
	if(!parsed)
	{
		throw RSGISException("Before running the parameters must be retrieved");
	}
	else
	{
		if(this->option == RSGISExeImageCalibration::landsatradcal)
		{
			
		}
        else if(this->option == RSGISExeImageCalibration::topatmosrefl)
        {
            
        }
        else if(this->option == RSGISExeImageCalibration::apply6s)
        {
            cout << "This command applies coefficients exported from 6S to a radiance image.\n";
			cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            for(int i = 0; i < numValues; ++i)
			{
				cout << "Band " <<  i+1 << ":\t" << this->aX[i] << this->bX[i] << this->cX[i] << endl;
			}
        }
		else
		{
			throw RSGISException("RSGISExeImageCalibration does not know this option");
		}
	}
}

void RSGISExeImageCalibration::help()
{
	cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
	cout << "" << endl;
    cout << "<rsgis:command algor=\"imagecalibration\" option=\"landsatradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image1\" band=\"int\" [sensorband=\"string\" | lmin=\"float\" lmax=\"float\" qcalmin=\"float\" qcalmax=\"float\"] />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image2\" band=\"int\" [sensorband=\"string\" | lmin=\"float\" lmax=\"float\" qcalmin=\"float\" qcalmax=\"float\"]/>" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image3\" band=\"int\" [sensorband=\"string\" | lmin=\"float\" lmax=\"float\" qcalmin=\"float\" qcalmax=\"float\"]/>" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"imagecalibration\" option=\"spotradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image1\" band=\"int\"gain=\"float\" />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image2\" band=\"int\"gain=\"float\" />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image3\" band=\"int\"gain=\"float\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"imagecalibration\" option=\"ikonosradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image1\" band=\"int\" [sensorband=\"string\" | calcoef=\"float\" bandwidth=\"float\" ] />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image2\" band=\"int\" [sensorband=\"string\" | calcoef=\"float\" bandwidth=\"float\" ] />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image3\" band=\"int\" [sensorband=\"string\" | calcoef=\"float\" bandwidth=\"float\" ] />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"imagecalibration\" option=\"asterradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image1\" band=\"int\" [sensorband=\"string\" | coefficient=\"float\" ] />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image2\" band=\"int\" [sensorband=\"string\" | coefficient=\"float\" ] />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image3\" band=\"int\" [sensorband=\"string\" | coefficient=\"float\" ] />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"imagecalibration\" option=\"irsradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image1\" band=\"int\" [sensorband=\"string\" | lmin=\"float\" lmax=\"float\" qcalmin=\"float\" qcalmax=\"float\"] />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image2\" band=\"int\" [sensorband=\"string\" | lmin=\"float\" lmax=\"float\" qcalmin=\"float\" qcalmax=\"float\"]/>" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image3\" band=\"int\" [sensorband=\"string\" | lmin=\"float\" lmax=\"float\" qcalmin=\"float\" qcalmax=\"float\"]/>" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"imagecalibration\" option=\"quickbird16bitradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image1\" band=\"int\" [sensorband=\"string\" | calfactor=\"float\" bandintegrate=\"float\" ] />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image2\" band=\"int\" [sensorband=\"string\" | calfactor=\"float\" bandintegrate=\"float\" ] />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image3\" band=\"int\" [sensorband=\"string\" | calfactor=\"float\" bandintegrate=\"float\" ] />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"imagecalibration\" option=\"quickbird8bitradcal\" output=\"image_out.env\" format=\"GDAL Format\" >" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image1\" band=\"int\" [sensorband=\"string\" | calfactor=\"float\" bandintegrate=\"float\" k=\"float\" ] />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image2\" band=\"int\" [sensorband=\"string\" | calfactor=\"float\" bandintegrate=\"float\" k=\"float\" ] />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image3\" band=\"int\" [sensorband=\"string\" | calfactor=\"float\" bandintegrate=\"float\" k=\"float\" ] />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"imagecalibration\" option=\"worldview2radcal\" output=\"image_out.env\" format=\"GDAL Format\">" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image1\" band=\"int\" [sensorband=\"string\" | calfactor=\"float\" bandintegrate=\"float\" ] />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image2\" band=\"int\" [sensorband=\"string\" | calfactor=\"float\" bandintegrate=\"float\" ] />" << endl;
    cout << "    <rsgis:band name=\"string\" image=\"image3\" band=\"int\" [sensorband=\"string\" | calfactor=\"float\" bandintegrate=\"float\" ] />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"imagecalibration\" option=\"topatmosrefl\" input=\"image_in.env\" output=\"image_out.env\" format=\"GDAL Format\" scaleFactor=\"float\" [julianday=\"int\" | day=\"int\" month=\"int\" year=\"int\"] [zenith=\"float\" | elevation=\"float\"] >" << endl;
    cout << "    <rsgis:band [sensorband=\"string\" | irradiance=\"float\"] />" << endl;
    cout << "    <rsgis:band [sensorband=\"string\" | irradiance=\"float\"] />" << endl;
    cout << "    <rsgis:band [sensorband=\"string\" | irradiance=\"float\"] />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"imagecalibration\" option=\"apply6s\" input=\"image_in.env\" output=\"image_out.env\" scaleFactor=\"float\" format=\"GDAL Format\" >" << endl;
    cout << "    <rsgis:band band=\"int\" ax=\"float\" bx=\"float\" cx=\"float\" />" << endl;
    cout << "    <rsgis:band band=\"int\" ax=\"float\" bx=\"float\" cx=\"float\" />" << endl;
    cout << "    <rsgis:band band=\"int\" ax=\"float\" bx=\"float\" cx=\"float\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"imagecalibration\" option=\"apply6s\" input=\"image_in.env\" output=\"image_out.env\" format=\"GDAL Format\" scaleFactor=\"float\" inputDEM=\"in_dem.env\">" << endl;
    cout << "    <rsgis:band band=\"int\" >" << endl;
    cout << "          <rsgis:elevation elevationUpper=\"float\" ax=\"float\" bx=\"float\" cx=\"float\"  />" << endl;
    cout << "          <rsgis:elevation elevationUpper=\"float\" ax=\"float\" bx=\"float\" cx=\"float\"  />" << endl;
    cout << "    </rsgls:band>" << endl;
    cout << "    <rsgis:band band=\"int\" >" << endl;
    cout << "          <rsgis:elevation elevationUpper=\"float\" ax=\"float\" bx=\"float\" cx=\"float\"  />" << endl;
    cout << "          <rsgis:elevation elevationUpper=\"float\" ax=\"float\" bx=\"float\" cx=\"float\"  />" << endl;
    cout << "    </rsgls:band>" << endl;
    cout << "</rsgis:command>" << endl;
	cout << "</rsgis:commands>\n";
}

string RSGISExeImageCalibration::getDescription()
{
	return "Provides a set of tools for calibrating Remotely Sensed image data.";
}

string RSGISExeImageCalibration::getXMLSchema()
{
	return "NOT PROVIDED!";
}

RSGISExeImageCalibration::~RSGISExeImageCalibration()
{
	
}
    
    
}
    
