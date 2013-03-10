/*
 *  RSGISExeImageMorphology.cpp
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 01/03/2012
 *  Copyright 2012 RSGISLib.
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

#include "RSGISExeImageMorphology.h"

namespace rsgisexe{

RSGISExeImageMorphology::RSGISExeImageMorphology() : RSGISAlgorithmParameters()
{
	this->algorithm = "morphology";
	this->inputImage = "";
	this->outputImage = "";
}

RSGISAlgorithmParameters* RSGISExeImageMorphology::getInstance()
{
	return new RSGISExeImageMorphology();
}

void RSGISExeImageMorphology::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISFileUtils fileUtils;
	RSGISMathsUtils mathUtils;
    RSGISMatrices matrixUtils;
    try
    {
        XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
        XMLCh *algorXMLStr = XMLString::transcode("algor");
        XMLCh *optionXMLStr = XMLString::transcode("option");
        XMLCh *optionDilate = XMLString::transcode("dilate");
        XMLCh *optionErode = XMLString::transcode("erode");
        XMLCh *optionGradient = XMLString::transcode("gradient");
        XMLCh *optionDilateAll = XMLString::transcode("dilateall");
        XMLCh *optionErodeAll = XMLString::transcode("erodeall");
        XMLCh *optionGradientAll = XMLString::transcode("gradientall");
        XMLCh *optionLocalMinima = XMLString::transcode("localminima");
        XMLCh *optionLocalMinimaAll = XMLString::transcode("localminimaall");
        XMLCh *optionClosing = XMLString::transcode("closing");
        XMLCh *optionOpening = XMLString::transcode("opening");
        XMLCh *optionWhiteTopHat = XMLString::transcode("whitetophat");
        XMLCh *optionBlackTopHat = XMLString::transcode("blacktophat");
        
        
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
        
        this->outDataType = GDT_Float32;
        XMLCh *datatypeXMLStr = XMLString::transcode("datatype");
        if(argElement->hasAttribute(datatypeXMLStr))
        {
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
                cerr << "Data type not recognised, defaulting to 32 bit float.";
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
        
        
        
        
        
        const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
        if(XMLString::equals(optionDilate, optionXML))
        {		
            this->option = RSGISExeImageMorphology::dilate;
            
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
            
            XMLCh *operatorXMLStr = XMLString::transcode("operator");
            if(argElement->hasAttribute(operatorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(operatorXMLStr));
                this->matrixOperator = matrixUtils.readMatrixFromGridTxt(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elementSizeXMLStr = XMLString::transcode("elementsize");
                if(argElement->hasAttribute(elementSizeXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elementSizeXMLStr));
                    unsigned int size = mathUtils.strtounsignedint(string(charValue));
                    this->matrixOperator = matrixUtils.createMatrix(size, size);
                    matrixUtils.setValues(this->matrixOperator, 1);
                    XMLString::release(&charValue);
                }
                else
                {
                    cout << "WARNING: Neither elementsize or operator were defined to defaulting to 3x3 square operator.\n";
                    this->matrixOperator = matrixUtils.createMatrix(3, 3);
                    matrixUtils.setValues(this->matrixOperator, 1);
                }
                XMLString::release(&elementSizeXMLStr);
            }
            XMLString::release(&operatorXMLStr);
        }
        else if(XMLString::equals(optionErode, optionXML))
        {		
            this->option = RSGISExeImageMorphology::erode;
            
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
            
            XMLCh *operatorXMLStr = XMLString::transcode("operator");
            if(argElement->hasAttribute(operatorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(operatorXMLStr));
                this->matrixOperator = matrixUtils.readMatrixFromGridTxt(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elementSizeXMLStr = XMLString::transcode("elementsize");
                if(argElement->hasAttribute(elementSizeXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elementSizeXMLStr));
                    unsigned int size = mathUtils.strtounsignedint(string(charValue));
                    this->matrixOperator = matrixUtils.createMatrix(size, size);
                    matrixUtils.setValues(this->matrixOperator, 1);
                    XMLString::release(&charValue);
                }
                else
                {
                    cout << "WARNING: Neither elementsize or operator were defined to defaulting to 3x3 square operator.\n";
                    this->matrixOperator = matrixUtils.createMatrix(3, 3);
                    matrixUtils.setValues(this->matrixOperator, 1);
                }
                XMLString::release(&elementSizeXMLStr);
            }
            XMLString::release(&operatorXMLStr);
        }
        else if(XMLString::equals(optionGradient, optionXML))
        {		
            this->option = RSGISExeImageMorphology::gradient;
            
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
            
            XMLCh *operatorXMLStr = XMLString::transcode("operator");
            if(argElement->hasAttribute(operatorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(operatorXMLStr));
                this->matrixOperator = matrixUtils.readMatrixFromGridTxt(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elementSizeXMLStr = XMLString::transcode("elementsize");
                if(argElement->hasAttribute(elementSizeXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elementSizeXMLStr));
                    unsigned int size = mathUtils.strtounsignedint(string(charValue));
                    this->matrixOperator = matrixUtils.createMatrix(size, size);
                    matrixUtils.setValues(this->matrixOperator, 1);
                    XMLString::release(&charValue);
                }
                else
                {
                    cout << "WARNING: Neither elementsize or operator were defined to defaulting to 3x3 square operator.\n";
                    this->matrixOperator = matrixUtils.createMatrix(3, 3);
                    matrixUtils.setValues(this->matrixOperator, 1);
                }
                XMLString::release(&elementSizeXMLStr);
            }
            XMLString::release(&operatorXMLStr);
        }
        else if(XMLString::equals(optionDilateAll, optionXML))
        {		
            this->option = RSGISExeImageMorphology::dilateall;
            
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
            
            XMLCh *operatorXMLStr = XMLString::transcode("operator");
            if(argElement->hasAttribute(operatorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(operatorXMLStr));
                this->matrixOperator = matrixUtils.readMatrixFromGridTxt(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elementSizeXMLStr = XMLString::transcode("elementsize");
                if(argElement->hasAttribute(elementSizeXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elementSizeXMLStr));
                    unsigned int size = mathUtils.strtounsignedint(string(charValue));
                    this->matrixOperator = matrixUtils.createMatrix(size, size);
                    matrixUtils.setValues(this->matrixOperator, 1);
                    XMLString::release(&charValue);
                }
                else
                {
                    cout << "WARNING: Neither elementsize or operator were defined to defaulting to 3x3 square operator.\n";
                    this->matrixOperator = matrixUtils.createMatrix(3, 3);
                    matrixUtils.setValues(this->matrixOperator, 1);
                }
                XMLString::release(&elementSizeXMLStr);
            }
            XMLString::release(&operatorXMLStr);
        }
        else if(XMLString::equals(optionErodeAll, optionXML))
        {		
            this->option = RSGISExeImageMorphology::erodeall;
            
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
            
            XMLCh *operatorXMLStr = XMLString::transcode("operator");
            if(argElement->hasAttribute(operatorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(operatorXMLStr));
                this->matrixOperator = matrixUtils.readMatrixFromGridTxt(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elementSizeXMLStr = XMLString::transcode("elementsize");
                if(argElement->hasAttribute(elementSizeXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elementSizeXMLStr));
                    unsigned int size = mathUtils.strtounsignedint(string(charValue));
                    this->matrixOperator = matrixUtils.createMatrix(size, size);
                    matrixUtils.setValues(this->matrixOperator, 1);
                    XMLString::release(&charValue);
                }
                else
                {
                    cout << "WARNING: Neither elementsize or operator were defined to defaulting to 3x3 square operator.\n";
                    this->matrixOperator = matrixUtils.createMatrix(3, 3);
                    matrixUtils.setValues(this->matrixOperator, 1);
                }
                XMLString::release(&elementSizeXMLStr);
            }
            XMLString::release(&operatorXMLStr);
        }
        else if(XMLString::equals(optionGradientAll, optionXML))
        {		
            this->option = RSGISExeImageMorphology::gradientall;
            
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
            
            XMLCh *operatorXMLStr = XMLString::transcode("operator");
            if(argElement->hasAttribute(operatorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(operatorXMLStr));
                this->matrixOperator = matrixUtils.readMatrixFromGridTxt(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elementSizeXMLStr = XMLString::transcode("elementsize");
                if(argElement->hasAttribute(elementSizeXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elementSizeXMLStr));
                    unsigned int size = mathUtils.strtounsignedint(string(charValue));
                    this->matrixOperator = matrixUtils.createMatrix(size, size);
                    matrixUtils.setValues(this->matrixOperator, 1);
                    XMLString::release(&charValue);
                }
                else
                {
                    cout << "WARNING: Neither elementsize or operator were defined to defaulting to 3x3 square operator.\n";
                    this->matrixOperator = matrixUtils.createMatrix(3, 3);
                    matrixUtils.setValues(this->matrixOperator, 1);
                }
                XMLString::release(&elementSizeXMLStr);
            }
            XMLString::release(&operatorXMLStr);
        }
        else if(XMLString::equals(optionLocalMinima, optionXML))
        {		
            this->option = RSGISExeImageMorphology::localminima;
            
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
            
            XMLCh *operatorXMLStr = XMLString::transcode("operator");
            if(argElement->hasAttribute(operatorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(operatorXMLStr));
                this->matrixOperator = matrixUtils.readMatrixFromGridTxt(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elementSizeXMLStr = XMLString::transcode("elementsize");
                if(argElement->hasAttribute(elementSizeXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elementSizeXMLStr));
                    unsigned int size = mathUtils.strtounsignedint(string(charValue));
                    this->matrixOperator = matrixUtils.createMatrix(size, size);
                    matrixUtils.setValues(this->matrixOperator, 1);
                    XMLString::release(&charValue);
                }
                else
                {
                    cout << "WARNING: Neither elementsize or operator were defined to defaulting to 3x3 square operator.\n";
                    this->matrixOperator = matrixUtils.createMatrix(3, 3);
                    matrixUtils.setValues(this->matrixOperator, 1);
                }
                XMLString::release(&elementSizeXMLStr);
            }
            XMLString::release(&operatorXMLStr);
            
            
            XMLCh *allowEqualsXMLStr = XMLString::transcode("allowequals");
            if(argElement->hasAttribute(allowEqualsXMLStr))
            {
                XMLCh *yesStr = XMLString::transcode("yes");
                const XMLCh *allowValue = argElement->getAttribute(allowEqualsXMLStr);
                
                if(XMLString::equals(allowValue, yesStr))
                {
                    this->allowEquals = true;
                }
                else
                {
                    this->allowEquals = false;
                }
                XMLString::release(&yesStr);
            }
            else
            {
                cout << "WARNING. Using default, equal values not allowed.\n";
                this->allowEquals = false;
            }
            XMLString::release(&allowEqualsXMLStr);
            
            
            XMLCh *numberingXMLStr = XMLString::transcode("numbering");
            if(argElement->hasAttribute(numberingXMLStr))
            {
                XMLCh *binaryStr = XMLString::transcode("binary");
                XMLCh *sequentialStr = XMLString::transcode("sequential");
                const XMLCh *numberingValue = argElement->getAttribute(numberingXMLStr);
                
                if(XMLString::equals(numberingValue, binaryStr))
                {
                    this->minOutType = RSGISImageMorphologyFindExtrema::binary;
                }
                else if(XMLString::equals(numberingValue, sequentialStr))
                {
                    this->minOutType = RSGISImageMorphologyFindExtrema::sequential;
                }
                else
                {
                    throw RSGISXMLArgumentsException("Numbering output unknown (binary | sequential).");
                }
                XMLString::release(&binaryStr);
                XMLString::release(&sequentialStr);
            }
            else
            {
                cout << "WARNING. Using default, generating a binary output.\n";
                this->minOutType = RSGISImageMorphologyFindExtrema::binary;
            }
            XMLString::release(&numberingXMLStr);
            
            
        }
        else if(XMLString::equals(optionLocalMinimaAll, optionXML))
        {		
            this->option = RSGISExeImageMorphology::localminimaall;
            
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
            
            XMLCh *operatorXMLStr = XMLString::transcode("operator");
            if(argElement->hasAttribute(operatorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(operatorXMLStr));
                this->matrixOperator = matrixUtils.readMatrixFromGridTxt(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elementSizeXMLStr = XMLString::transcode("elementsize");
                if(argElement->hasAttribute(elementSizeXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elementSizeXMLStr));
                    unsigned int size = mathUtils.strtounsignedint(string(charValue));
                    this->matrixOperator = matrixUtils.createMatrix(size, size);
                    matrixUtils.setValues(this->matrixOperator, 1);
                    XMLString::release(&charValue);
                }
                else
                {
                    cout << "WARNING: Neither elementsize or operator were defined to defaulting to 3x3 square operator.\n";
                    this->matrixOperator = matrixUtils.createMatrix(3, 3);
                    matrixUtils.setValues(this->matrixOperator, 1);
                }
                XMLString::release(&elementSizeXMLStr);
            }
            XMLString::release(&operatorXMLStr);
            
            XMLCh *allowEqualsXMLStr = XMLString::transcode("allowequals");
            if(argElement->hasAttribute(allowEqualsXMLStr))
            {
                XMLCh *yesStr = XMLString::transcode("yes");
                const XMLCh *allowValue = argElement->getAttribute(allowEqualsXMLStr);
                
                if(XMLString::equals(allowValue, yesStr))
                {
                    this->allowEquals = true;
                }
                else
                {
                    this->allowEquals = false;
                }
                XMLString::release(&yesStr);
            }
            else
            {
                cout << "WARNING. Using default, equal values not allowed.\n";
                this->allowEquals = false;
            }
            XMLString::release(&allowEqualsXMLStr);
            
            
            XMLCh *numberingXMLStr = XMLString::transcode("numbering");
            if(argElement->hasAttribute(numberingXMLStr))
            {
                XMLCh *binaryStr = XMLString::transcode("binary");
                XMLCh *sequentialStr = XMLString::transcode("sequential");
                const XMLCh *numberingValue = argElement->getAttribute(numberingXMLStr);
                
                if(XMLString::equals(numberingValue, binaryStr))
                {
                    this->minOutType = RSGISImageMorphologyFindExtrema::binary;
                }
                else if(XMLString::equals(numberingValue, sequentialStr))
                {
                    this->minOutType = RSGISImageMorphologyFindExtrema::sequential;
                }
                else
                {
                    throw RSGISXMLArgumentsException("Numbering output unknown (binary | sequential).");
                }
                XMLString::release(&binaryStr);
                XMLString::release(&sequentialStr);
            }
            else
            {
                cout << "WARNING. Using default, generating a binary output.\n";
                this->minOutType = RSGISImageMorphologyFindExtrema::binary;
            }
            XMLString::release(&numberingXMLStr);
        }
        else if(XMLString::equals(optionClosing, optionXML))
        {		
            this->option = RSGISExeImageMorphology::closing;
            
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
            
            XMLCh *tempImgXMLStr = XMLString::transcode("temp");
            if(argElement->hasAttribute(tempImgXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(tempImgXMLStr));
                this->tempImage = string(charValue);
                XMLString::release(&charValue);
                this->useMemory = false;
            }
            else
            {
                cout << "Using memory for processing\n";
                this->tempImage = "";
                this->useMemory = true;
            }
            XMLString::release(&tempImgXMLStr);
            
            XMLCh *iterationsXMLStr = XMLString::transcode("iterations");
            if(argElement->hasAttribute(iterationsXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(iterationsXMLStr));
                this->numIterations = mathUtils.strtounsignedint(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                cout << "The number of iterations was not specified, default is 1\n";
                this->numIterations = 1;
            }
            XMLString::release(&iterationsXMLStr);
            
            XMLCh *operatorXMLStr = XMLString::transcode("operator");
            if(argElement->hasAttribute(operatorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(operatorXMLStr));
                this->matrixOperator = matrixUtils.readMatrixFromGridTxt(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elementSizeXMLStr = XMLString::transcode("elementsize");
                if(argElement->hasAttribute(elementSizeXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elementSizeXMLStr));
                    unsigned int size = mathUtils.strtounsignedint(string(charValue));
                    this->matrixOperator = matrixUtils.createMatrix(size, size);
                    matrixUtils.setValues(this->matrixOperator, 1);
                    XMLString::release(&charValue);
                }
                else
                {
                    cout << "WARNING: Neither elementsize or operator were defined to defaulting to 3x3 square operator.\n";
                    this->matrixOperator = matrixUtils.createMatrix(3, 3);
                    matrixUtils.setValues(this->matrixOperator, 1);
                }
                XMLString::release(&elementSizeXMLStr);
            }
            XMLString::release(&operatorXMLStr);
        }
        else if(XMLString::equals(optionOpening, optionXML))
        {		
            this->option = RSGISExeImageMorphology::opening;
            
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
            
            XMLCh *tempImgXMLStr = XMLString::transcode("temp");
            if(argElement->hasAttribute(tempImgXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(tempImgXMLStr));
                this->tempImage = string(charValue);
                XMLString::release(&charValue);
                this->useMemory = false;
            }
            else
            {
                cout << "Using memory for processing\n";
                this->tempImage = "";
                this->useMemory = true;
            }
            XMLString::release(&tempImgXMLStr);
            
            XMLCh *iterationsXMLStr = XMLString::transcode("iterations");
            if(argElement->hasAttribute(iterationsXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(iterationsXMLStr));
                this->numIterations = mathUtils.strtounsignedint(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                cout << "The number of iterations was not specified, default is 1\n";
                this->numIterations = 1;
            }
            XMLString::release(&iterationsXMLStr);
            
            XMLCh *operatorXMLStr = XMLString::transcode("operator");
            if(argElement->hasAttribute(operatorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(operatorXMLStr));
                this->matrixOperator = matrixUtils.readMatrixFromGridTxt(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elementSizeXMLStr = XMLString::transcode("elementsize");
                if(argElement->hasAttribute(elementSizeXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elementSizeXMLStr));
                    unsigned int size = mathUtils.strtounsignedint(string(charValue));
                    this->matrixOperator = matrixUtils.createMatrix(size, size);
                    matrixUtils.setValues(this->matrixOperator, 1);
                    XMLString::release(&charValue);
                }
                else
                {
                    cout << "WARNING: Neither elementsize or operator were defined to defaulting to 3x3 square operator.\n";
                    this->matrixOperator = matrixUtils.createMatrix(3, 3);
                    matrixUtils.setValues(this->matrixOperator, 1);
                }
                XMLString::release(&elementSizeXMLStr);
            }
            XMLString::release(&operatorXMLStr);
        }
        else if(XMLString::equals(optionBlackTopHat, optionXML))
        {		
            this->option = RSGISExeImageMorphology::blacktophat;
            
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
            
            XMLCh *tempImgXMLStr = XMLString::transcode("temp");
            if(argElement->hasAttribute(tempImgXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(tempImgXMLStr));
                this->tempImage = string(charValue);
                XMLString::release(&charValue);
                this->useMemory = false;
            }
            else
            {
                cout << "Using memory for processing\n";
                this->tempImage = "";
                this->useMemory = true;
            }
            XMLString::release(&tempImgXMLStr);
            
            XMLCh *operatorXMLStr = XMLString::transcode("operator");
            if(argElement->hasAttribute(operatorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(operatorXMLStr));
                this->matrixOperator = matrixUtils.readMatrixFromGridTxt(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elementSizeXMLStr = XMLString::transcode("elementsize");
                if(argElement->hasAttribute(elementSizeXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elementSizeXMLStr));
                    unsigned int size = mathUtils.strtounsignedint(string(charValue));
                    this->matrixOperator = matrixUtils.createMatrix(size, size);
                    matrixUtils.setValues(this->matrixOperator, 1);
                    XMLString::release(&charValue);
                }
                else
                {
                    cout << "WARNING: Neither elementsize or operator were defined to defaulting to 3x3 square operator.\n";
                    this->matrixOperator = matrixUtils.createMatrix(3, 3);
                    matrixUtils.setValues(this->matrixOperator, 1);
                }
                XMLString::release(&elementSizeXMLStr);
            }
            XMLString::release(&operatorXMLStr);
        }
        else if(XMLString::equals(optionWhiteTopHat, optionXML))
        {		
            this->option = RSGISExeImageMorphology::whitetophat;
            
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
            
            XMLCh *tempImgXMLStr = XMLString::transcode("temp");
            if(argElement->hasAttribute(tempImgXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(tempImgXMLStr));
                this->tempImage = string(charValue);
                XMLString::release(&charValue);
                this->useMemory = false;
            }
            else
            {
                cout << "Using memory for processing\n";
                this->tempImage = "";
                this->useMemory = true;
            }
            XMLString::release(&tempImgXMLStr);
            
            XMLCh *operatorXMLStr = XMLString::transcode("operator");
            if(argElement->hasAttribute(operatorXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(operatorXMLStr));
                this->matrixOperator = matrixUtils.readMatrixFromGridTxt(string(charValue));
                XMLString::release(&charValue);
            }
            else
            {
                XMLCh *elementSizeXMLStr = XMLString::transcode("elementsize");
                if(argElement->hasAttribute(elementSizeXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(elementSizeXMLStr));
                    unsigned int size = mathUtils.strtounsignedint(string(charValue));
                    this->matrixOperator = matrixUtils.createMatrix(size, size);
                    matrixUtils.setValues(this->matrixOperator, 1);
                    XMLString::release(&charValue);
                }
                else
                {
                    cout << "WARNING: Neither elementsize or operator were defined to defaulting to 3x3 square operator.\n";
                    this->matrixOperator = matrixUtils.createMatrix(3, 3);
                    matrixUtils.setValues(this->matrixOperator, 1);
                }
                XMLString::release(&elementSizeXMLStr);
            }
            XMLString::release(&operatorXMLStr);
        }
        else
        {
            string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeImageMorphology.");
            throw RSGISXMLArgumentsException(message.c_str());
        }
        
        XMLString::release(&algorName);
        XMLString::release(&algorXMLStr);
        XMLString::release(&optionXMLStr);
        XMLString::release(&optionDilate);
        XMLString::release(&optionErode);
        XMLString::release(&optionGradient);
        XMLString::release(&optionDilateAll);
        XMLString::release(&optionErodeAll);
        XMLString::release(&optionGradientAll);
        XMLString::release(&optionLocalMinima);
        XMLString::release(&optionLocalMinimaAll);
        XMLString::release(&optionClosing);
        XMLString::release(&optionOpening);
        XMLString::release(&optionBlackTopHat);
        XMLString::release(&optionWhiteTopHat);
    }
    catch(RSGISXMLArgumentsException &e)
    {
        throw e;
    }
    catch(RSGISException &e)
    {
        throw RSGISXMLArgumentsException(e.what());
    }
    catch(std::exception &e)
    {
        throw RSGISXMLArgumentsException(e.what());
    }
	
	parsed = true;
}

void RSGISExeImageMorphology::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else if(option == RSGISExeImageMorphology::dilate)
    {
        cout << "Calculate a morphological dilation of the input image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset **dataset = new GDALDataset*[1];
            dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageMorphologyDilate newDilate;
            newDilate.dilateImage(dataset, this->outputImage, this->matrixOperator, this->imageFormat, this->outDataType);
            
            GDALClose(dataset[0]);
            delete[] dataset;
        }
        catch(RSGISException e)
        {
            throw e;
        }	
    }
    else if(option == RSGISExeImageMorphology::erode)
    {
        cout << "Calculate a morphological erosion of the input image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset **dataset = new GDALDataset*[1];
            cout << this->inputImage << endl;
            dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageMorphologyErode newErode;
            newErode.erodeImage(dataset, this->outputImage, this->matrixOperator, this->imageFormat, this->outDataType);
            
            GDALClose(dataset[0]);
            delete[] dataset;
        }
        catch(RSGISException e)
        {
            throw e;
        }	
    }
    else if(option == RSGISExeImageMorphology::gradient)
    {
        cout << "Calculate a morphological gradient for the input image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset **dataset = new GDALDataset*[1];
            cout << this->inputImage << endl;
            dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageMorphologyGradient newGradient;
            newGradient.calcGradientImage(dataset, this->outputImage, this->matrixOperator, this->imageFormat, this->outDataType);
            
            GDALClose(dataset[0]);
            delete[] dataset;
        }
        catch(RSGISException e)
        {
            throw e;
        }	
    }
    else if(option == RSGISExeImageMorphology::dilateall)
    {
        cout << "Calculate a morphological dilation of the input image creating a single output band\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset **dataset = new GDALDataset*[1];
            dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageMorphologyDilate newDilate;
            newDilate.dilateImageAll(dataset, this->outputImage, this->matrixOperator, this->imageFormat, this->outDataType);
            
            GDALClose(dataset[0]);
            delete[] dataset;
        }
        catch(RSGISException e)
        {
            throw e;
        }	
    }
    else if(option == RSGISExeImageMorphology::erodeall)
    {
        cout << "Calculate a morphological erosion of the input image creating a single output band\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset **dataset = new GDALDataset*[1];
            cout << this->inputImage << endl;
            dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageMorphologyErode newErode;
            newErode.erodeImageAll(dataset, this->outputImage, this->matrixOperator, this->imageFormat, this->outDataType);
            
            GDALClose(dataset[0]);
            delete[] dataset;
        }
        catch(RSGISException e)
        {
            throw e;
        }	
    }
    else if(option == RSGISExeImageMorphology::gradientall)
    {
        cout << "Calculate a morphological gradient for the input image creating a single output band\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset **dataset = new GDALDataset*[1];
            cout << this->inputImage << endl;
            dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageMorphologyGradient newGradient;
            newGradient.calcGradientImageAll(dataset, this->outputImage, this->matrixOperator, this->imageFormat, this->outDataType);
            
            GDALClose(dataset[0]);
            delete[] dataset;
        }
        catch(RSGISException e)
        {
            throw e;
        }	
    }
    else if(option == RSGISExeImageMorphology::localminima)
    {
        cout << "Apply a filter to find local minima - used as input into a morphological watershed\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset **dataset = new GDALDataset*[1];
            cout << this->inputImage << endl;
            dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageMorphologyFindExtrema findExtrema;
            findExtrema.findMinima(dataset, this->outputImage, this->matrixOperator, minOutType, allowEquals, this->imageFormat, this->outDataType);
            
            GDALClose(dataset[0]);
            delete[] dataset;
        }
        catch(RSGISException e)
        {
            throw e;
        }	
    }
    else if(option == RSGISExeImageMorphology::localminimaall)
    {
        cout << "Apply a filter to find local minima using all image bands to produce a single band output - used as input into a morphological watershed\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        
        try
        {
            GDALAllRegister();
            GDALDataset **dataset = new GDALDataset*[1];
            cout << this->inputImage << endl;
            dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageMorphologyFindExtrema findExtrema;
            findExtrema.findMinimaAll(dataset, this->outputImage, this->matrixOperator, minOutType, allowEquals, this->imageFormat, this->outDataType);
            
            GDALClose(dataset[0]);
            delete[] dataset;
        }
        catch(RSGISException e)
        {
            throw e;
        }	
    }
    else if(option == RSGISExeImageMorphology::closing)
    {
        cout << "Apply a morphological closing to an image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(useMemory)
        {
            cout << "Processing taking place in memory\n";
        }
        else
        {
            cout << "Temporary Image: " << this->tempImage << endl;
        }
        cout << "Number of iterations: " << this->numIterations << endl;
        
        try
        {
            GDALAllRegister();
            cout << this->inputImage << endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageMorphologyClosing morphClosing;
            morphClosing.performClosing(dataset, this->outputImage, this->tempImage, this->useMemory, this->matrixOperator, this->numIterations, this->imageFormat, this->outDataType);
            
            GDALClose(dataset);
        }
        catch(RSGISException e)
        {
            throw e;
        }	
    }
    else if(option == RSGISExeImageMorphology::opening)
    {
        cout << "Apply a morphological opening to an image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(useMemory)
        {
            cout << "Processing taking place in memory\n";
        }
        else
        {
            cout << "Temporary Image: " << this->tempImage << endl;
        }
        cout << "Number of iterations: " << this->numIterations << endl;
        
        try
        {
            GDALAllRegister();
            cout << this->inputImage << endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageMorphologyOpening morphOpening;
            morphOpening.performOpening(dataset, this->outputImage, this->tempImage, this->useMemory, this->matrixOperator, this->numIterations, this->imageFormat, this->outDataType);
            
            GDALClose(dataset);
        }
        catch(RSGISException e)
        {
            throw e;
        }	
    }
    else if(option == RSGISExeImageMorphology::blacktophat)
    {
        cout << "Apply a morphological black top hat to an image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(useMemory)
        {
            cout << "Processing taking place in memory\n";
        }
        else
        {
            cout << "Temporary Image: " << this->tempImage << endl;
        }
        
        try
        {
            GDALAllRegister();
            cout << this->inputImage << endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageMorphologyTopHat morphTopHat;
            morphTopHat.performBlackTopHat(dataset, this->outputImage, this->tempImage, this->useMemory, this->matrixOperator, this->imageFormat, this->outDataType);
        
            GDALClose(dataset);
        }
        catch(RSGISException e)
        {
            throw e;
        }	
    }
    else if(option == RSGISExeImageMorphology::whitetophat)
    {
        cout << "Apply a morphological white top hat to an image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(useMemory)
        {
            cout << "Processing taking place in memory\n";
        }
        else
        {
            cout << "Temporary Image: " << this->tempImage << endl;
        }
        
        try
        {
            GDALAllRegister();
            cout << this->inputImage << endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                string message = string("Could not open image ") + this->inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            RSGISImageMorphologyTopHat morphTopHat;
            morphTopHat.performWhiteTopHat(dataset, this->outputImage, this->tempImage, this->useMemory, this->matrixOperator, this->imageFormat, this->outDataType);
            
            GDALClose(dataset);
        }
        catch(RSGISException e)
        {
            throw e;
        }	
    }
    else
    {
        cout << "RSGISExeImageMorphology: Options not recognised\n";
    }
}


void RSGISExeImageMorphology::printParameters()
{
	if(!parsed)
	{
		cout << "Parameters have not been parsed.\n";
	}
	else if(option == RSGISExeImageMorphology::dilate)
    {
        cout << "Perform a Dilation of the input image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
    }
    else if(option == RSGISExeImageMorphology::erode)
    {
        cout << "Perform a Erosion of the input image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
    }
    else if(option == RSGISExeImageMorphology::gradient)
    {
        cout << "Calculate a morphological gradient for the input image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
    }
    else if(option == RSGISExeImageMorphology::dilateall)
    {
        cout << "Calculate a morphological dilation of the input image creating a single output band\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
    }
    else if(option == RSGISExeImageMorphology::erodeall)
    {
        cout << "Calculate a morphological erosion of the input image creating a single output band\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
    }
    else if(option == RSGISExeImageMorphology::gradientall)
    {
        cout << "Calculate a morphological gradient for the input image creating a single output band\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
    }
    else if(option == RSGISExeImageMorphology::localminima)
    {
        cout << "Apply a filter to find local minima - used as input into a morphological watershed\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
    }
    else if(option == RSGISExeImageMorphology::localminimaall)
    {
        cout << "Apply a filter to find local minima using all image bands to produce a single band output - used as input into a morphological watershed\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
    }
    else if(option == RSGISExeImageMorphology::closing)
    {
        cout << "Apply a morphological closing to an image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(useMemory)
        {
            cout << "Processing taking place in memory\n";
        }
        else
        {
            cout << "Temporary Image: " << this->tempImage << endl;
        }
        cout << "Number of iterations: " << this->numIterations << endl;
    }
    else if(option == RSGISExeImageMorphology::opening)
    {
        cout << "Apply a morphological opening to an image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(useMemory)
        {
            cout << "Processing taking place in memory\n";
        }
        else
        {
            cout << "Temporary Image: " << this->tempImage << endl;
        }
        cout << "Number of iterations: " << this->numIterations << endl;
    }
    else if(option == RSGISExeImageMorphology::blacktophat)
    {
        cout << "Apply a morphological black top hat to an image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(useMemory)
        {
            cout << "Processing taking place in memory\n";
        }
        else
        {
            cout << "Temporary Image: " << this->tempImage << endl;
        }
    }
    else if(option == RSGISExeImageMorphology::whitetophat)
    {
        cout << "Apply a morphological white top hat to an image\n";
        cout << "Input Image: " << this->inputImage << endl;
        cout << "Output Image: " << this->outputImage << endl;
        if(useMemory)
        {
            cout << "Processing taking place in memory\n";
        }
        else
        {
            cout << "Temporary Image: " << this->tempImage << endl;
        }
    }
    else
    {
        cout << "RSGISExeImageMorphology: Options not recognised\n";
    }
}

void RSGISExeImageMorphology::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
    cout << "<!-- A command to apply a morphological dilation to an image -->" << endl;
    cout << "<rsgis:command algor=\"morphology\" option=\"dilate\" image=\"image.env\" output=\"image_out.env\" [operator=\"matrix.gmtxt\" | elementsize=\"int\"] />" << endl;
    cout << "<!-- A command to apply a morphological erosion to an image -->" << endl;
    cout << "<rsgis:command algor=\"morphology\" option=\"erode\" image=\"image.env\" output=\"image_out.env\" [operator=\"matrix.gmtxt\" | elementsize=\"int\"] />" << endl;
    cout << "<!-- A command to apply a morphological gradient to an image -->" << endl;
    cout << "<rsgis:command algor=\"morphology\" option=\"gradient\" image=\"image.env\" output=\"image_out.env\" [operator=\"matrix.gmtxt\" | elementsize=\"int\"] />" << endl;
    cout << "<!-- A command to apply a morphological dilation to all image bands to produce a single band output -->" << endl;
    cout << "<rsgis:command algor=\"morphology\" option=\"dilateall\" image=\"image.env\" output=\"image_out.env\" [operator=\"matrix.gmtxt\" | elementsize=\"int\"] />" << endl;
    cout << "<!-- A command to apply a morphological erosion to all image bands to produce a single band output -->" << endl;
    cout << "<rsgis:command algor=\"morphology\" option=\"erodeall\" image=\"image.env\" output=\"image_out.env\" [operator=\"matrix.gmtxt\" | elementsize=\"int\"] />" << endl;
    cout << "<!-- A command to apply a morphological gradient to all image bands to produce a single band output -->" << endl;
    cout << "<rsgis:command algor=\"morphology\" option=\"gradientall\" image=\"image.env\" output=\"image_out.env\" [operator=\"matrix.gmtxt\" | elementsize=\"int\"] />" << endl;
    cout << "<!-- A command to apply a filter to find local minima - used as input into a morphological watershed -->" << endl;
    cout << "<rsgis:command algor=\"morphology\" option=\"localminima\" image=\"image.env\" output=\"image_out.env\" [operator=\"matrix.gmtxt\" | elementsize=\"int\"] numbering=\"binary | sequencial\" allowequals=\"yes | no\" />" << endl;
    cout << "<!-- A command to apply a filter to find local minima using all image bands to produce a single band output - used as input into a morphological watershed -->" << endl;
    cout << "<rsgis:command algor=\"morphology\" option=\"localminimaall\" image=\"image.env\" output=\"image_out.env\" [operator=\"matrix.gmtxt\" | elementsize=\"int\"] numbering=\"binary | sequencial\" allowequals=\"yes | no\" />" << endl;
    cout << "<!-- A command to apply a morphological closing to an image (if temp image not provided processing will be done in memory) -->" << endl;
    cout << "<rsgis:command algor=\"morphology\" option=\"closing\" image=\"image.env\" output=\"image_out.env\" [temp=\"tmp_image.env\"] [operator=\"matrix.gmtxt\" | elementsize=\"int\"] [iterations=\"int\"] />" << endl;
    cout << "<!-- A command to apply a morphological opening to an image (if temp image not provided processing will be done in memory) -->" << endl;
    cout << "<rsgis:command algor=\"morphology\" option=\"opening\" image=\"image.env\" output=\"image_out.env\" [temp=\"tmp_image.env\"] [operator=\"matrix.gmtxt\" | elementsize=\"int\"] [iterations=\"int\"] />" << endl;
    cout << "<!-- A command to apply a morphological black top hat to an image (if temp image not provided processing will be done in memory) -->" << endl;
    cout << "<rsgis:command algor=\"morphology\" option=\"blacktophat\" image=\"image.env\" output=\"image_out.env\" [temp=\"tmp_image.env\"] [operator=\"matrix.gmtxt\" | elementsize=\"int\"] [iterations=\"int\"] />" << endl;
    cout << "<!-- A command to apply a morphological white top hat to an image (if temp image not provided processing will be done in memory) -->" << endl;
    cout << "<rsgis:command algor=\"morphology\" option=\"whitetophat\" image=\"image.env\" output=\"image_out.env\" [temp=\"tmp_image.env\"] [operator=\"matrix.gmtxt\" | elementsize=\"int\"] [iterations=\"int\"] />" << endl;
    cout << "<\rsgis:commands>" << endl;
}

string RSGISExeImageMorphology::getDescription()
{
	return "Image morphology operations.";
}

string RSGISExeImageMorphology::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeImageMorphology::~RSGISExeImageMorphology()
{
	
}
    
    
}

