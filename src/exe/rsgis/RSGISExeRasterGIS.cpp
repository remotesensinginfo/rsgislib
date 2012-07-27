/*
 *  RSGISExeRasterGIS.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/07/2012.
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

#include "RSGISExeRasterGIS.h"

namespace rsgisexe{
    
    RSGISExeRasterGIS::RSGISExeRasterGIS() : rsgis::RSGISAlgorithmParameters()
    {
        this->algorithm = "rastergis";
        
        this->option = RSGISExeRasterGIS::none;
    }
    
    rsgis::RSGISAlgorithmParameters* RSGISExeRasterGIS::getInstance()
    {
        return new RSGISExeRasterGIS();
    }
    
    void RSGISExeRasterGIS::retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException)
    {	
        rsgis::math::RSGISMathsUtils mathUtils;
        rsgis::utils::RSGISTextUtils textUtils;
        
        XMLCh *algorName = xercesc::XMLString::transcode(this->algorithm.c_str());
        XMLCh *algorXMLStr = xercesc::XMLString::transcode("algor");
        XMLCh *optionXMLStr = xercesc::XMLString::transcode("option");
        
        XMLCh *optionCopyGDALATT = xercesc::XMLString::transcode("copyGDALATT");
        
        
        const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
        if(!xercesc::XMLString::equals(algorName, algorNameEle))
        {
            throw rsgis::RSGISXMLArgumentsException("The algorithm name is incorrect.");
        }
        

        
        const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
        if(xercesc::XMLString::equals(optionCopyGDALATT, optionXML))
        {		
            this->option = RSGISExeRasterGIS::copyGDALATT;
            
            XMLCh *tableXMLStr = xercesc::XMLString::transcode("table");
            if(argElement->hasAttribute(tableXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(tableXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'table\' attribute was provided.");
            }
            xercesc::XMLString::release(&tableXMLStr);
            
            
            XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
            if(argElement->hasAttribute(imageXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
                this->clumpsImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
            }
            xercesc::XMLString::release(&imageXMLStr);
        }
        else
        {
            std::string message = std::string("The option (") + std::string(xercesc::XMLString::transcode(optionXML)) + std::string(") is not known: RSGISExeRasterGIS.");
            throw rsgis::RSGISXMLArgumentsException(message.c_str());
        }
        
        parsed = true;
        
        xercesc::XMLString::release(&algorName);
        xercesc::XMLString::release(&algorXMLStr);
        xercesc::XMLString::release(&optionXMLStr);
        xercesc::XMLString::release(&optionCopyGDALATT);
        
    }
    
    void RSGISExeRasterGIS::runAlgorithm() throw(rsgis::RSGISException)
    {
        if(!parsed)
        {
            throw rsgis::RSGISException("Before running the parameters much be retrieved");
        }
        else
        {
            if(this->option == RSGISExeRasterGIS::copyGDALATT)
            {
                std::cout << "Export an attribute table to a GDAL Dataset with a raster attribute table\n";
                std::cout << "Input Table: " << this->inputImage << std::endl;
                std::cout << "Output Image: " << this->clumpsImage << std::endl;
                
                
                GDALAllRegister();
                try
                {
                    GDALDataset *inputDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
                    if(inputDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->inputImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    GDALDataset *outRATDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_Update);
                    if(outRATDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->clumpsImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    std::cout << "Import attribute table\n";
                    const GDALRasterAttributeTable *gdalAtt = inputDataset->GetRasterBand(1)->GetDefaultRAT();
                    
                    std::cout << "Adding RAT\n";
                    outRATDataset->GetRasterBand(1)->SetDefaultRAT(gdalAtt);
                    
                    GDALClose(inputDataset);
                    GDALClose(outRATDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }	
            }
            else
            {
                throw rsgis::RSGISException("The option is not recognised: RSGISExeRasterGIS");
            }
        }
    }
    
    void RSGISExeRasterGIS::printParameters()
    {
        if(parsed)
        {
            if(this->option == RSGISExeRasterGIS::copyGDALATT)
            {
                std::cout << "Export an attribute table to a GDAL Dataset with a raster attribute table\n";
                std::cout << "Input Table: " << this->inputImage << std::endl;
                std::cout << "Output Image: " << this->clumpsImage << std::endl;
            }
        }
        else
        {
            std::cout << "The parameters have yet to be parsed\n";
        }
    }
    
    
    void RSGISExeRasterGIS::help()
    {
        std::cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << std::endl;

        std::cout << "</rsgis:commands>" << std::endl;
    }
    
    std::string RSGISExeRasterGIS::getDescription()
    {
        return "An interface to the available raster GIS functionality.";
    }
    
    std::string RSGISExeRasterGIS::getXMLSchema()
    {
        return "NOT DONE!";
    }
    
    RSGISExeRasterGIS::~RSGISExeRasterGIS()
    {
        
    }
    
}



