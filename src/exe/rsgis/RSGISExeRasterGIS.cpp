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
        rsgis::utils::RSGISTextUtils textUtils;
        
        XMLCh *algorName = xercesc::XMLString::transcode(this->algorithm.c_str());
        XMLCh *algorXMLStr = xercesc::XMLString::transcode("algor");
        XMLCh *optionXMLStr = xercesc::XMLString::transcode("option");
        
        XMLCh *optionCopyGDALATT = xercesc::XMLString::transcode("copyGDALATT");
        XMLCh *optionSpatialLocation = xercesc::XMLString::transcode("spatiallocation");
        XMLCh *optionEucDistFromFeat = xercesc::XMLString::transcode("eucdistfromfeat");
        XMLCh *optionFindTopN = xercesc::XMLString::transcode("findtopn");
        XMLCh *optionCopyGDALATTColumns = xercesc::XMLString::transcode("copyGDALATTColumns");
        
        
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
        else if(xercesc::XMLString::equals(optionSpatialLocation, optionXML))
        {		
            this->option = RSGISExeRasterGIS::spatiallocation;
            
            XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
            if(argElement->hasAttribute(imageXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
            }
            xercesc::XMLString::release(&imageXMLStr);
            
            XMLCh *eastingsXMLStr = xercesc::XMLString::transcode("eastings");
            if(argElement->hasAttribute(eastingsXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(eastingsXMLStr));
                this->eastingsField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'eastings\' attribute was provided.");
            }
            xercesc::XMLString::release(&eastingsXMLStr);
            
            XMLCh *northingsXMLStr = xercesc::XMLString::transcode("northings");
            if(argElement->hasAttribute(northingsXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(northingsXMLStr));
                this->northingsField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'northings\' attribute was provided.");
            }
            xercesc::XMLString::release(&northingsXMLStr);
        }
        else if(xercesc::XMLString::equals(optionEucDistFromFeat, optionXML))
        {		
            this->option = RSGISExeRasterGIS::eucdistfromfeat;
            
            XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
            if(argElement->hasAttribute(imageXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
            }
            xercesc::XMLString::release(&imageXMLStr);
            
            XMLCh *outFieldXMLStr = xercesc::XMLString::transcode("outfield");
            if(argElement->hasAttribute(outFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outFieldXMLStr));
                this->outputField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'outfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&outFieldXMLStr);
            
            XMLCh *featureXMLStr = xercesc::XMLString::transcode("feature");
            if(argElement->hasAttribute(featureXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(featureXMLStr));
                this->fid = textUtils.strtosizet(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'feature\' attribute was provided.");
            }
            xercesc::XMLString::release(&featureXMLStr);
            
            XMLCh *rsgisFieldXMLStr = xercesc::XMLString::transcode("rsgis:field");
            xercesc::DOMNodeList *fieldNodesList = argElement->getElementsByTagName(rsgisFieldXMLStr);
            unsigned int numFieldTags = fieldNodesList->getLength();
            
            std::cout << "Found " << numFieldTags << " field tags" << std::endl;
            
            if(numFieldTags == 0)
            {
                throw rsgis::RSGISXMLArgumentsException("No field tags have been provided, at least 1 is required.");
            }
            
            fields.reserve(numFieldTags);
            
            xercesc::DOMElement *attElement = NULL;
            std::string fieldName = "";
            for(int i = 0; i < numFieldTags; i++)
            {
                attElement = static_cast<xercesc::DOMElement*>(fieldNodesList->item(i));
                
                XMLCh *nameXMLStr = xercesc::XMLString::transcode("name");
                if(attElement->hasAttribute(nameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(nameXMLStr));
                    fields.push_back(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                xercesc::XMLString::release(&nameXMLStr);
            }  
        }
        else if(xercesc::XMLString::equals(optionFindTopN, optionXML))
        {		
            this->option = RSGISExeRasterGIS::findtopn;
            
            XMLCh *imageXMLStr = xercesc::XMLString::transcode("image");
            if(argElement->hasAttribute(imageXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'image\' attribute was provided.");
            }
            xercesc::XMLString::release(&imageXMLStr);
            
            XMLCh *spatialDistXMLStr = xercesc::XMLString::transcode("spatialdist");
            if(argElement->hasAttribute(spatialDistXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(spatialDistXMLStr));
                this->spatialDistField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'spatialdist\' attribute was provided.");
            }
            xercesc::XMLString::release(&spatialDistXMLStr);

            
            XMLCh *metricDistXMLStr = xercesc::XMLString::transcode("metricdist");
            if(argElement->hasAttribute(metricDistXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(metricDistXMLStr));
                this->distanceField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'metricdist\' attribute was provided.");
            }
            xercesc::XMLString::release(&metricDistXMLStr);
            
            XMLCh *outFieldXMLStr = xercesc::XMLString::transcode("outfield");
            if(argElement->hasAttribute(outFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outFieldXMLStr));
                this->outputField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'outfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&outFieldXMLStr);
            
            XMLCh *distThresholdXMLStr = xercesc::XMLString::transcode("distthreshold");
            if(argElement->hasAttribute(distThresholdXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(distThresholdXMLStr));
                this->distThreshold = textUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'distthreshold\' attribute was provided.");
            }
            xercesc::XMLString::release(&distThresholdXMLStr);
            
            XMLCh *nXMLStr = xercesc::XMLString::transcode("n");
            if(argElement->hasAttribute(nXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(nXMLStr));
                this->nFeatures = textUtils.strto16bitUInt(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'n\' attribute was provided.");
            }
            xercesc::XMLString::release(&nXMLStr);
             
        }
        else if(xercesc::XMLString::equals(optionCopyGDALATTColumns, optionXML))
        {		
            this->option = RSGISExeRasterGIS::copyGDALATTColumns;
            
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
            
            XMLCh *rsgisFieldXMLStr = xercesc::XMLString::transcode("rsgis:field");
            xercesc::DOMNodeList *fieldNodesList = argElement->getElementsByTagName(rsgisFieldXMLStr);
            unsigned int numFieldTags = fieldNodesList->getLength();
            
            std::cout << "Found " << numFieldTags << " field tags" << std::endl;
            
            if(numFieldTags == 0)
            {
                throw rsgis::RSGISXMLArgumentsException("No field tags have been provided, at least 1 is required.");
            }
            
            fields.reserve(numFieldTags);
            
            xercesc::DOMElement *attElement = NULL;
            std::string fieldName = "";
            for(int i = 0; i < numFieldTags; i++)
            {
                attElement = static_cast<xercesc::DOMElement*>(fieldNodesList->item(i));
                
                XMLCh *nameXMLStr = xercesc::XMLString::transcode("name");
                if(attElement->hasAttribute(nameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(nameXMLStr));
                    fields.push_back(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                xercesc::XMLString::release(&nameXMLStr);
            }
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
        xercesc::XMLString::release(&optionSpatialLocation);
        xercesc::XMLString::release(&optionFindTopN);
        xercesc::XMLString::release(&optionCopyGDALATTColumns);
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
                    outRATDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
                    
                    GDALClose(inputDataset);
                    GDALClose(outRATDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }	
            }
            else if(this->option == RSGISExeRasterGIS::spatiallocation)
            {
                std::cout << "Populate the attribute table with mean eastings and northings\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Eastings Field: " << this->eastingsField << std::endl;
                std::cout << "Northings Field: " << this->northingsField << std::endl;
                
                try
                {
                    GDALAllRegister();
                    
                    GDALDataset *inputDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                    if(inputDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->inputImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    rsgis::rastergis::RSGISCalcClusterLocation calcLoc;
                    calcLoc.populateAttWithClumpLocation(inputDataset, eastingsField, northingsField);
                    
                    GDALClose(inputDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }
                
            }
            else if(this->option == RSGISExeRasterGIS::eucdistfromfeat)
            {
                std::cout << "Calculate the euclidean distance from a feature to all other features.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Feature: " << this->fid << std::endl;
                std::cout << "Output Field: " << this->outputField << std::endl;
                std::cout << "Distance calculated using:\n";
                for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
                {
                    std::cout << "\tField: " << (*iterFields) << std::endl;
                }
                
                try
                {
                    GDALAllRegister();
                    
                    GDALDataset *inputDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                    if(inputDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->inputImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                
                    rsgis::rastergis::RSGISCalcEucDistanceInAttTable calcDist;
                    calcDist.calcEucDist(inputDataset, this->fid, this->outputField, this->fields);
                    
                    GDALClose(inputDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }
                
            }
            else if(this->option == RSGISExeRasterGIS::findtopn)
            {
                std::cout << "Calculate the top N features within a given spatial distance.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Distance Field: " << this->distanceField << std::endl;
                std::cout << "Spatial Dist. Field: " << this->spatialDistField << std::endl;
                std::cout << "Output Field: " << this->outputField << std::endl;
                std::cout << "Distance threshold: " << this->distThreshold << std::endl;
                std::cout << "N: " << this->nFeatures << std::endl;
                
                try
                {
                    GDALAllRegister();
                    
                    GDALDataset *inputDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                    if(inputDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->inputImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    rsgis::rastergis::RSGISFindTopNWithinDist calcTopN;
                    calcTopN.calcMinDistTopN(inputDataset, this->spatialDistField, this->distanceField, this->outputField, this->nFeatures, this->distThreshold);
                    
                    GDALClose(inputDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }
                
            }
            else if(this->option == RSGISExeRasterGIS::copyGDALATTColumns)
            {
                std::cout << "Copy specified attribute table columns from one file to another\n";
                std::cout << "Input Table: " << this->inputImage << std::endl;
                std::cout << "Output Image: " << this->clumpsImage << std::endl;
                std::cout << "Fields to be copied:\n";
                for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
                {
                    std::cout << "\tField: " << (*iterFields) << std::endl;
                }
                
                
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
                    
                    rsgis::rastergis::RSGISRasterAttUtils attUtils;
                    attUtils.copyAttColumns(inputDataset, outRATDataset, fields);
                    
                    outRATDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
                    
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
            else if(this->option == RSGISExeRasterGIS::spatiallocation)
            {
                std::cout << "Populate the attribute table with mean eastings and northings\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Eastings Field: " << this->eastingsField << std::endl;
                std::cout << "Northings Field: " << this->northingsField << std::endl;
            }
            else if(this->option == RSGISExeRasterGIS::eucdistfromfeat)
            {
                std::cout << "Calculate the euclidean distance from a feature to all other features.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Feature: " << this->fid << std::endl;
                std::cout << "Output Field: " << this->outputField << std::endl;
                std::cout << "Distance calculated using:\n";
                for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
                {
                    std::cout << "\tField: " << (*iterFields) << std::endl;
                }
            }
            else
            {
                std::cout << "The option was not recognised: RSGISExeRasterGIS\n";
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



