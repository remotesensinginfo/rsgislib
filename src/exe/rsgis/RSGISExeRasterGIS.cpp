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
        XMLCh *optionFindSpecClose = xercesc::XMLString::transcode("findspecclose");
        XMLCh *optionCopyGDALATTColumns = xercesc::XMLString::transcode("copyGDALATTColumns");
        XMLCh *optionPopAttributeStats = xercesc::XMLString::transcode("popattributestats");
        XMLCh *optionPopCategoryProportions = xercesc::XMLString::transcode("popcategoryproportions");
        XMLCh *optionCopyCatColours = xercesc::XMLString::transcode("copycatcolours");
        XMLCh *optionKNNMajorityClassifier = xercesc::XMLString::transcode("knnmajorityclassifier");
        XMLCh *optionPopAttributePercentile = xercesc::XMLString::transcode("popattributepercentile");
        XMLCh *optionExport2ASCII = xercesc::XMLString::transcode("export2ascii");
        XMLCh *optionClassTranslate = xercesc::XMLString::transcode("classtranslate");
        XMLCh *optionColourClasses = xercesc::XMLString::transcode("colourclasses");
        XMLCh *optionColourStrClasses = xercesc::XMLString::transcode("colourstrclasses");
        XMLCh *optionGenColourTab = xercesc::XMLString::transcode("gencolourtab");
        XMLCh *optionExportCols2Raster = xercesc::XMLString::transcode("exportcols2raster");
        XMLCh *optionStrClassMajority = xercesc::XMLString::transcode("strclassmajority");
        XMLCh *optionSpecDistMajorityClassifier = xercesc::XMLString::transcode("specdistmajorityclassifier");
        XMLCh *optionMaxLikelihoodClassifier = xercesc::XMLString::transcode("maxlikelihoodclassifier");
        XMLCh *optionMaxLikelihoodClassifierLocalPriors = xercesc::XMLString::transcode("maxlikelihoodclassifierlocalpriors");
        
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
        else if(xercesc::XMLString::equals(optionFindSpecClose, optionXML))
        {
            this->option = RSGISExeRasterGIS::findspecclose;
            
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
            
            XMLCh *specDistThresholdXMLStr = xercesc::XMLString::transcode("specdistthreshold");
            if(argElement->hasAttribute(specDistThresholdXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(specDistThresholdXMLStr));
                this->specDistThreshold = textUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'specdistthreshold\' attribute was provided.");
            }
            xercesc::XMLString::release(&specDistThresholdXMLStr);
            
            XMLCh *spatDistThresholdXMLStr = xercesc::XMLString::transcode("spatdistthreshold");
            if(argElement->hasAttribute(spatDistThresholdXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(spatDistThresholdXMLStr));
                this->distThreshold = textUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'spatdistthreshold\' attribute was provided.");
            }
            xercesc::XMLString::release(&spatDistThresholdXMLStr);
            
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
        else if(xercesc::XMLString::equals(optionPopAttributeStats, optionXML))
        {
            this->option = RSGISExeRasterGIS::popattributestats;
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
            
            XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
            if(argElement->hasAttribute(clumpsXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
                this->clumpsImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
            }
            xercesc::XMLString::release(&clumpsXMLStr);
            
            XMLCh *rsgisBandXMLStr = xercesc::XMLString::transcode("rsgis:band");
            xercesc::DOMNodeList *bandNodesList = argElement->getElementsByTagName(rsgisBandXMLStr);
            unsigned int numBands = bandNodesList->getLength();
            
            std::cout << "Found " << numBands << " Attributes" << std::endl;
            
            bandStats = new std::vector<rsgis::rastergis::RSGISBandAttStats*>();
            bandStats->reserve(numBands);
            
            rsgis::rastergis::RSGISBandAttStats *bandStat = NULL;
            xercesc::DOMElement *bandElement = NULL;
            for(int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandNodesList->item(i));
                
                bandStat = new rsgis::rastergis::RSGISBandAttStats();
                
                XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
                if(bandElement->hasAttribute(bandXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandXMLStr));
                    bandStat->band = textUtils.strto32bitUInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandXMLStr);
                
                XMLCh *minXMLStr = xercesc::XMLString::transcode("min");
                if(bandElement->hasAttribute(minXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(minXMLStr));
                    bandStat->minField = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                    
                    bandStat->calcMin = true;
                }
                else
                {
                    bandStat->calcMin = false;
                }
                xercesc::XMLString::release(&minXMLStr);
                
                XMLCh *maxXMLStr = xercesc::XMLString::transcode("max");
                if(bandElement->hasAttribute(maxXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(maxXMLStr));
                    bandStat->maxField = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                    
                    bandStat->calcMax = true;
                }
                else
                {
                    bandStat->calcMax = false;
                }
                xercesc::XMLString::release(&maxXMLStr);
                
                XMLCh *meanXMLStr = xercesc::XMLString::transcode("mean");
                if(bandElement->hasAttribute(meanXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(meanXMLStr));
                    bandStat->meanField = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                    
                    bandStat->calcMean = true;
                }
                else
                {
                    bandStat->calcMean = false;
                }
                xercesc::XMLString::release(&meanXMLStr);
                
                XMLCh *stdDevXMLStr = xercesc::XMLString::transcode("stddev");
                if(bandElement->hasAttribute(stdDevXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(stdDevXMLStr));
                    bandStat->stdDevField = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                    
                    bandStat->calcStdDev = true;
                }
                else
                {
                    bandStat->calcStdDev = false;
                }
                xercesc::XMLString::release(&stdDevXMLStr);
                
                XMLCh *sumXMLStr = xercesc::XMLString::transcode("sum");
                if(bandElement->hasAttribute(sumXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(sumXMLStr));
                    bandStat->sumField = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                    
                    bandStat->calcSum = true;
                }
                else
                {
                    bandStat->calcSum = false;
                }
                xercesc::XMLString::release(&sumXMLStr);
                
                bandStat->calcMedian = false;
                bandStats->push_back(bandStat);
            }
            xercesc::XMLString::release(&rsgisBandXMLStr);
            
        }
        else if(xercesc::XMLString::equals(optionPopCategoryProportions, optionXML))
        {
            this->option = RSGISExeRasterGIS::popcategoryproportions;
            
            XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
            if(argElement->hasAttribute(clumpsXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
                this->clumpsImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
            }
            xercesc::XMLString::release(&clumpsXMLStr);
            
            
            XMLCh *categoriesXMLStr = xercesc::XMLString::transcode("categories");
            if(argElement->hasAttribute(categoriesXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(categoriesXMLStr));
                this->categoriesImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'categories\' attribute was provided.");
            }
            xercesc::XMLString::release(&categoriesXMLStr);
            
            XMLCh *outColsXMLStr = xercesc::XMLString::transcode("outcols");
            if(argElement->hasAttribute(outColsXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outColsXMLStr));
                this->outColsName = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'outcols\' attribute was provided.");
            }
            xercesc::XMLString::release(&outColsXMLStr);
            
            XMLCh *majorityXMLStr = xercesc::XMLString::transcode("majority");
            if(argElement->hasAttribute(majorityXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(majorityXMLStr));
                this->majorityColName = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'majority\' attribute was provided.");
            }
            xercesc::XMLString::release(&majorityXMLStr);
            
            this->copyClassNames = false;
            XMLCh *majClassNameXMLStr = xercesc::XMLString::transcode("majclassname");
            if(argElement->hasAttribute(majClassNameXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(majClassNameXMLStr));
                this->majClassNameField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
                
                XMLCh *classNameXMLStr = xercesc::XMLString::transcode("classname");
                if(argElement->hasAttribute(classNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(classNameXMLStr));
                    this->classNameField = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'classname\' attribute was provided.");
                }
                xercesc::XMLString::release(&classNameXMLStr);
                
                this->copyClassNames = true;
            }
            else
            {
                this->copyClassNames = false;
            }
            xercesc::XMLString::release(&majClassNameXMLStr);
            
        }
        else if(xercesc::XMLString::equals(optionCopyCatColours, optionXML))
        {
            this->option = RSGISExeRasterGIS::copycatcolours;
            
            XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
            if(argElement->hasAttribute(clumpsXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
                this->clumpsImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
            }
            xercesc::XMLString::release(&clumpsXMLStr);
            
            
            XMLCh *categoriesXMLStr = xercesc::XMLString::transcode("categories");
            if(argElement->hasAttribute(categoriesXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(categoriesXMLStr));
                this->categoriesImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'categories\' attribute was provided.");
            }
            xercesc::XMLString::release(&categoriesXMLStr);
            
            XMLCh *classFieldXMLStr = xercesc::XMLString::transcode("classfield");
            if(argElement->hasAttribute(classFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(classFieldXMLStr));
                this->classField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'classfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&classFieldXMLStr);
        }
        else if(xercesc::XMLString::equals(optionKNNMajorityClassifier, optionXML))
        {
            this->option = RSGISExeRasterGIS::knnmajorityclassifier;
            
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
            
            XMLCh *inClassFieldXMLStr = xercesc::XMLString::transcode("inclassfield");
            if(argElement->hasAttribute(inClassFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inClassFieldXMLStr));
                this->inClassNameField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'inclassfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&inClassFieldXMLStr);
            
            XMLCh *outClassFieldXMLStr = xercesc::XMLString::transcode("outclassfield");
            if(argElement->hasAttribute(outClassFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outClassFieldXMLStr));
                this->outClassNameField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'outclassfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&outClassFieldXMLStr);
            
            XMLCh *trainingColXMLStr = xercesc::XMLString::transcode("trainingcol");
            if(argElement->hasAttribute(trainingColXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(trainingColXMLStr));
                this->trainingSelectCol = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'trainingcol\' attribute was provided.");
            }
            xercesc::XMLString::release(&trainingColXMLStr);
            
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
            
            XMLCh *areaXMLStr = xercesc::XMLString::transcode("area");
            if(argElement->hasAttribute(areaXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(areaXMLStr));
                this->areaField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'area\' attribute was provided.");
            }
            xercesc::XMLString::release(&areaXMLStr);
            
            XMLCh *weightFieldXMLStr = xercesc::XMLString::transcode("weightfield");
            if(argElement->hasAttribute(weightFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(weightFieldXMLStr));
                this->majWeightField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'weightfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&weightFieldXMLStr);
            
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
            
            XMLCh *weightAXMLStr = xercesc::XMLString::transcode("weighta");
            if(argElement->hasAttribute(weightAXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(weightAXMLStr));
                this->weightA = textUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                this->weightA = 3;
            }
            xercesc::XMLString::release(&weightAXMLStr);    
            
            
            XMLCh *majMethodXMLStr = xercesc::XMLString::transcode("majoritymethod");
            if(argElement->hasAttribute(majMethodXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(majMethodXMLStr));
                std::string majMethodStr = std::string(charValue);
                if(majMethodStr == "standardKNN")
                {
                    this->majMethod = rsgis::rastergis::stdMajority;
                }
                else if(majMethodStr == "weightedKNN")
                {
                    this->majMethod = rsgis::rastergis::weightedMajority;
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("Majority method is not recognised, options are \'standardKNN\' or \'weightedKNN\'.");
                }
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'majoritymethod\' attribute was provided.");
            }
            xercesc::XMLString::release(&majMethodXMLStr);
            
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
        else if(xercesc::XMLString::equals(optionPopAttributePercentile, optionXML))
        {
            this->option = RSGISExeRasterGIS::popattributepercentile;
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
            
            XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
            if(argElement->hasAttribute(clumpsXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
                this->clumpsImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
            }
            xercesc::XMLString::release(&clumpsXMLStr);
            
            XMLCh *rsgisBandXMLStr = xercesc::XMLString::transcode("rsgis:band");
            xercesc::DOMNodeList *bandNodesList = argElement->getElementsByTagName(rsgisBandXMLStr);
            unsigned int numBands = bandNodesList->getLength();
            
            std::cout << "Found " << numBands << " Attributes" << std::endl;
            
            bandPercentiles = new std::vector<rsgis::rastergis::RSGISBandAttPercentiles*>();
            bandPercentiles->reserve(numBands);
            
            rsgis::rastergis::RSGISBandAttPercentiles *bandPercentile = NULL;
            xercesc::DOMElement *bandElement = NULL;
            for(int i = 0; i < numBands; i++)
            {
                bandElement = static_cast<xercesc::DOMElement*>(bandNodesList->item(i));
                
                bandPercentile = new rsgis::rastergis::RSGISBandAttPercentiles();
                
                XMLCh *bandXMLStr = xercesc::XMLString::transcode("band");
                if(bandElement->hasAttribute(bandXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(bandXMLStr));
                    bandPercentile->band = textUtils.strto32bitUInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'band\' attribute was provided.");
                }
                xercesc::XMLString::release(&bandXMLStr);
                
                XMLCh *nameXMLStr = xercesc::XMLString::transcode("name");
                if(bandElement->hasAttribute(nameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(nameXMLStr));
                    bandPercentile->fieldName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);                    
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                }
                xercesc::XMLString::release(&nameXMLStr);
                
                XMLCh *percentileXMLStr = xercesc::XMLString::transcode("percentile");
                if(bandElement->hasAttribute(percentileXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(bandElement->getAttribute(percentileXMLStr));
                    bandPercentile->percentile = textUtils.strto32bitUInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'percentile\' attribute was provided.");
                }
                xercesc::XMLString::release(&percentileXMLStr);
                
                bandPercentiles->push_back(bandPercentile);
            }
            xercesc::XMLString::release(&rsgisBandXMLStr);
            
        }
        else if(xercesc::XMLString::equals(optionExport2ASCII, optionXML))
        {
            this->option = RSGISExeRasterGIS::export2ascii;
            
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
            
            XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
            if(argElement->hasAttribute(outputXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
                this->outputFile = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
            }
            xercesc::XMLString::release(&outputXMLStr);
                        
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
        else if(xercesc::XMLString::equals(optionClassTranslate, optionXML))
        {
            this->option = RSGISExeRasterGIS::classtranslate;
            
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
            
            XMLCh *inFieldXMLStr = xercesc::XMLString::transcode("infield");
            if(argElement->hasAttribute(inFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inFieldXMLStr));
                this->classInField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'infield\' attribute was provided.");
            }
            xercesc::XMLString::release(&inFieldXMLStr);
            
            XMLCh *outFieldXMLStr = xercesc::XMLString::transcode("outfield");
            if(argElement->hasAttribute(outFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outFieldXMLStr));
                this->classOutField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'outfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&outFieldXMLStr);
            
            XMLCh *rsgisClassXMLStr = xercesc::XMLString::transcode("rsgis:class");
            xercesc::DOMNodeList *classNodesList = argElement->getElementsByTagName(rsgisClassXMLStr);
            unsigned int numClassTags = classNodesList->getLength();
            
            std::cout << "Found " << numClassTags << " class tags" << std::endl;
            
            if(numClassTags == 0)
            {
                throw rsgis::RSGISXMLArgumentsException("No class tags have been provided, at least 1 is required.");
            }
            
            xercesc::DOMElement *attElement = NULL;
            size_t inClassId = 0;
            size_t outClassId = 0;
            for(int i = 0; i < numClassTags; i++)
            {
                attElement = static_cast<xercesc::DOMElement*>(classNodesList->item(i));
                
                XMLCh *inIDXMLStr = xercesc::XMLString::transcode("inid");
                if(attElement->hasAttribute(inIDXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(inIDXMLStr));
                    inClassId = textUtils.strtosizet(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'inid\' attribute was provided.");
                }
                xercesc::XMLString::release(&inIDXMLStr);
                
                XMLCh *outIDXMLStr = xercesc::XMLString::transcode("outid");
                if(attElement->hasAttribute(outIDXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(outIDXMLStr));
                    outClassId = textUtils.strtosizet(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'outid\' attribute was provided.");
                }
                xercesc::XMLString::release(&outIDXMLStr);
                
                classPairs.insert(std::pair<size_t, size_t>(inClassId, outClassId));
            }
        }
        else if(xercesc::XMLString::equals(optionColourClasses, optionXML))
        {
            this->option = RSGISExeRasterGIS::colourclasses;
            
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
            
            XMLCh *classFieldXMLStr = xercesc::XMLString::transcode("classfield");
            if(argElement->hasAttribute(classFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(classFieldXMLStr));
                this->classInField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'classfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&classFieldXMLStr);
            
            
            XMLCh *rsgisClassXMLStr = xercesc::XMLString::transcode("rsgis:class");
            xercesc::DOMNodeList *classNodesList = argElement->getElementsByTagName(rsgisClassXMLStr);
            unsigned int numClassTags = classNodesList->getLength();
            
            std::cout << "Found " << numClassTags << " class tags" << std::endl;
            
            if(numClassTags == 0)
            {
                throw rsgis::RSGISXMLArgumentsException("No class tags have been provided, at least 1 is required.");
            }
            
            xercesc::DOMElement *attElement = NULL;
            size_t classId = 0;
            int red = 0;
            int green = 0;
            int blue = 0;
            int alpha = 0;
            for(int i = 0; i < numClassTags; i++)
            {
                attElement = static_cast<xercesc::DOMElement*>(classNodesList->item(i));
                
                XMLCh *idXMLStr = xercesc::XMLString::transcode("id");
                if(attElement->hasAttribute(idXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(idXMLStr));
                    classId = textUtils.strtosizet(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'id\' attribute was provided.");
                }
                xercesc::XMLString::release(&idXMLStr);
                
                XMLCh *redXMLStr = xercesc::XMLString::transcode("r");
                if(attElement->hasAttribute(redXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(redXMLStr));
                    red = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'r\' attribute was provided.");
                }
                xercesc::XMLString::release(&redXMLStr);
                
                XMLCh *greenXMLStr = xercesc::XMLString::transcode("g");
                if(attElement->hasAttribute(greenXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(greenXMLStr));
                    green = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'g\' attribute was provided.");
                }
                xercesc::XMLString::release(&greenXMLStr);
                
                XMLCh *blueXMLStr = xercesc::XMLString::transcode("b");
                if(attElement->hasAttribute(blueXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(blueXMLStr));
                    blue = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'b\' attribute was provided.");
                }
                xercesc::XMLString::release(&blueXMLStr);
                
                XMLCh *alphaXMLStr = xercesc::XMLString::transcode("a");
                if(attElement->hasAttribute(alphaXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(alphaXMLStr));
                    alpha = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'a\' attribute was provided.");
                }
                xercesc::XMLString::release(&alphaXMLStr);
                
                classColourPairs.insert(std::pair<size_t, rsgis::utils::RSGISColourInt>(classId, rsgis::utils::RSGISColourInt(red, green, blue, alpha)));
            }
        }
        else if(xercesc::XMLString::equals(optionColourStrClasses, optionXML))
        {
            this->option = RSGISExeRasterGIS::colourstrclasses;
            
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
            
            XMLCh *classFieldXMLStr = xercesc::XMLString::transcode("classfield");
            if(argElement->hasAttribute(classFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(classFieldXMLStr));
                this->classInField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'classfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&classFieldXMLStr);
            
            
            XMLCh *rsgisClassXMLStr = xercesc::XMLString::transcode("rsgis:class");
            xercesc::DOMNodeList *classNodesList = argElement->getElementsByTagName(rsgisClassXMLStr);
            unsigned int numClassTags = classNodesList->getLength();
            
            std::cout << "Found " << numClassTags << " class tags" << std::endl;
            
            if(numClassTags == 0)
            {
                throw rsgis::RSGISXMLArgumentsException("No class tags have been provided, at least 1 is required.");
            }
            
            xercesc::DOMElement *attElement = NULL;
            std::string className = 0;
            int red = 0;
            int green = 0;
            int blue = 0;
            int alpha = 0;
            for(int i = 0; i < numClassTags; i++)
            {
                std::cout << "i = " << i << std::endl;
                attElement = static_cast<xercesc::DOMElement*>(classNodesList->item(i));
                
                XMLCh *nameXMLStr = xercesc::XMLString::transcode("classname");
                if(attElement->hasAttribute(nameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(nameXMLStr));
                    className = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'classname\' attribute was provided.");
                }
                xercesc::XMLString::release(&nameXMLStr);
                
                XMLCh *redXMLStr = xercesc::XMLString::transcode("r");
                if(attElement->hasAttribute(redXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(redXMLStr));
                    red = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'r\' attribute was provided.");
                }
                xercesc::XMLString::release(&redXMLStr);
                
                XMLCh *greenXMLStr = xercesc::XMLString::transcode("g");
                if(attElement->hasAttribute(greenXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(greenXMLStr));
                    green = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'g\' attribute was provided.");
                }
                xercesc::XMLString::release(&greenXMLStr);
                
                XMLCh *blueXMLStr = xercesc::XMLString::transcode("b");
                if(attElement->hasAttribute(blueXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(blueXMLStr));
                    blue = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'b\' attribute was provided.");
                }
                xercesc::XMLString::release(&blueXMLStr);
                
                XMLCh *alphaXMLStr = xercesc::XMLString::transcode("a");
                if(attElement->hasAttribute(alphaXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(alphaXMLStr));
                    alpha = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'a\' attribute was provided.");
                }
                xercesc::XMLString::release(&alphaXMLStr);
                
                classStrColourPairs.insert(std::pair<std::string, rsgis::utils::RSGISColourInt>(className, rsgis::utils::RSGISColourInt(red, green, blue, alpha)));
            }
        }
        else if(xercesc::XMLString::equals(optionGenColourTab, optionXML))
        {
            this->option = RSGISExeRasterGIS::gencolourtab;
            
            XMLCh *tableXMLStr = xercesc::XMLString::transcode("table");
            if(argElement->hasAttribute(tableXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(tableXMLStr));
                this->clumpsImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'table\' attribute was provided.");
            }
            xercesc::XMLString::release(&tableXMLStr);
            
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
                        
            XMLCh *redXMLStr = xercesc::XMLString::transcode("red");
            if(argElement->hasAttribute(redXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(redXMLStr));
                this->redBand = textUtils.strto32bitInt(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'red\' attribute was provided.");
            }
            xercesc::XMLString::release(&redXMLStr);
            
            XMLCh *greenXMLStr = xercesc::XMLString::transcode("green");
            if(argElement->hasAttribute(greenXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(greenXMLStr));
                this->greenBand = textUtils.strto32bitInt(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'green\' attribute was provided.");
            }
            xercesc::XMLString::release(&greenXMLStr);
            
            XMLCh *blueXMLStr = xercesc::XMLString::transcode("blue");
            if(argElement->hasAttribute(blueXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(blueXMLStr));
                this->blueBand = textUtils.strto32bitInt(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'blue\' attribute was provided.");
            }
            xercesc::XMLString::release(&blueXMLStr);
        }
        else if(xercesc::XMLString::equals(optionExportCols2Raster, optionXML))
        {
            this->option = RSGISExeRasterGIS::exportcols2raster;
            
            XMLCh *clumpsXMLStr = xercesc::XMLString::transcode("clumps");
            if(argElement->hasAttribute(clumpsXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(clumpsXMLStr));
                this->inputImage = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
            }
            xercesc::XMLString::release(&clumpsXMLStr);
            
            XMLCh *outputXMLStr = xercesc::XMLString::transcode("output");
            if(argElement->hasAttribute(outputXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outputXMLStr));
                this->outputFile = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'output\' attribute was provided.");
            }
            xercesc::XMLString::release(&outputXMLStr);
            
            // Set output image fomat (defaults to KEA)
            this->imageFormat = "KEA";
            XMLCh *formatXMLStr = xercesc::XMLString::transcode("format");
            if(argElement->hasAttribute(formatXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(formatXMLStr));
                this->imageFormat = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            xercesc::XMLString::release(&formatXMLStr);
            
            
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
                    std::cerr << "Data type not recognised, defaulting to 32 bit float.";
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
        else if(xercesc::XMLString::equals(optionStrClassMajority, optionXML))
        {
            this->option = RSGISExeRasterGIS::strclassmajority;
            
            XMLCh *baseXMLStr = xercesc::XMLString::transcode("base");
            if(argElement->hasAttribute(baseXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(baseXMLStr));
                this->baseSegment = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'base\' attribute was provided.");
            }
            xercesc::XMLString::release(&baseXMLStr);
            
            
            XMLCh *baseClassXMLStr = xercesc::XMLString::transcode("baseclass");
            if(argElement->hasAttribute(baseClassXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(baseClassXMLStr));
                this->baseClassCol = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'baseclass\' attribute was provided.");
            }
            xercesc::XMLString::release(&baseClassXMLStr);

            XMLCh *infoXMLStr = xercesc::XMLString::transcode("info");
            if(argElement->hasAttribute(infoXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(infoXMLStr));
                this->infoSegment = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'info\' attribute was provided.");
            }
            xercesc::XMLString::release(&infoXMLStr);
            
            XMLCh *infoClassXMLStr = xercesc::XMLString::transcode("infoclass");
            if(argElement->hasAttribute(infoClassXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(infoClassXMLStr));
                this->infoClassCol = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'infoclass\' attribute was provided.");
            }
            xercesc::XMLString::release(&infoClassXMLStr);
            
        }
        else if(xercesc::XMLString::equals(optionSpecDistMajorityClassifier, optionXML))
        {
            this->option = RSGISExeRasterGIS::specdistmajorityclassifier;
            
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
            
            XMLCh *inClassFieldXMLStr = xercesc::XMLString::transcode("inclassfield");
            if(argElement->hasAttribute(inClassFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inClassFieldXMLStr));
                this->inClassNameField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'inclassfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&inClassFieldXMLStr);
            
            XMLCh *outClassFieldXMLStr = xercesc::XMLString::transcode("outclassfield");
            if(argElement->hasAttribute(outClassFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outClassFieldXMLStr));
                this->outClassNameField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'outclassfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&outClassFieldXMLStr);
            
            XMLCh *trainingColXMLStr = xercesc::XMLString::transcode("trainingcol");
            if(argElement->hasAttribute(trainingColXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(trainingColXMLStr));
                this->trainingSelectCol = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'trainingcol\' attribute was provided.");
            }
            xercesc::XMLString::release(&trainingColXMLStr);
            
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
            
            XMLCh *areaXMLStr = xercesc::XMLString::transcode("area");
            if(argElement->hasAttribute(areaXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(areaXMLStr));
                this->areaField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'area\' attribute was provided.");
            }
            xercesc::XMLString::release(&areaXMLStr);
            
            XMLCh *weightFieldXMLStr = xercesc::XMLString::transcode("weightfield");
            if(argElement->hasAttribute(weightFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(weightFieldXMLStr));
                this->majWeightField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'weightfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&weightFieldXMLStr);
            
            XMLCh *specDistThresholdXMLStr = xercesc::XMLString::transcode("specdistthreshold");
            if(argElement->hasAttribute(specDistThresholdXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(specDistThresholdXMLStr));
                this->specDistThreshold = textUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'specdistthreshold\' attribute was provided.");
            }
            xercesc::XMLString::release(&specDistThresholdXMLStr);
            
            XMLCh *spatDistThresholdXMLStr = xercesc::XMLString::transcode("spatdistthreshold");
            if(argElement->hasAttribute(spatDistThresholdXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(spatDistThresholdXMLStr));
                this->distThreshold = textUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'spatdistthreshold\' attribute was provided.");
            }
            xercesc::XMLString::release(&spatDistThresholdXMLStr);
            
            XMLCh *thresOriginDistXMLStr = xercesc::XMLString::transcode("thresorigindist");
            if(argElement->hasAttribute(thresOriginDistXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(thresOriginDistXMLStr));
                this->specThresOriginDist = textUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                this->specThresOriginDist = 100;
            }
            xercesc::XMLString::release(&thresOriginDistXMLStr);
            
            XMLCh *majMethodXMLStr = xercesc::XMLString::transcode("specdistmethod");
            if(argElement->hasAttribute(majMethodXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(majMethodXMLStr));
                std::string distMethodStr = std::string(charValue);
                if(distMethodStr == "euclidean")
                {
                    this->distThresMethod = rsgis::rastergis::stdEucSpecDist;
                }
                else if(distMethodStr == "origineucweighted")
                {
                    this->distThresMethod = rsgis::rastergis::originWeightedEuc;
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("Distance method is not recognised, options are \'euclidean\' or \'origineucweighted\'.");
                }
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'majoritymethod\' attribute was provided.");
            }
            xercesc::XMLString::release(&majMethodXMLStr);
            
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
        else if(xercesc::XMLString::equals(optionMaxLikelihoodClassifier, optionXML))
        {
            this->option = RSGISExeRasterGIS::maxlikelihoodclassifier;
            
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
            
            XMLCh *inClassFieldXMLStr = xercesc::XMLString::transcode("inclassfield");
            if(argElement->hasAttribute(inClassFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inClassFieldXMLStr));
                this->inClassNameField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'inclassfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&inClassFieldXMLStr);
            
            XMLCh *outClassFieldXMLStr = xercesc::XMLString::transcode("outclassfield");
            if(argElement->hasAttribute(outClassFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outClassFieldXMLStr));
                this->outClassNameField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'outclassfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&outClassFieldXMLStr);
            
            XMLCh *trainingColXMLStr = xercesc::XMLString::transcode("trainingcol");
            if(argElement->hasAttribute(trainingColXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(trainingColXMLStr));
                this->trainingSelectCol = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'trainingcol\' attribute was provided.");
            }
            xercesc::XMLString::release(&trainingColXMLStr);
            
            XMLCh *areaXMLStr = xercesc::XMLString::transcode("area");
            if(argElement->hasAttribute(areaXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(areaXMLStr));
                this->areaField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'area\' attribute was provided.");
            }
            
            XMLCh *priorsXMLStr = xercesc::XMLString::transcode("priors");
            if(argElement->hasAttribute(priorsXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(priorsXMLStr));
                std::string priorsMethodStr = std::string(charValue);
                if(priorsMethodStr == "equal")
                {
                    this->priorsMethod = rsgis::rastergis::rsgis_equal;
                }
                else if(priorsMethodStr == "area")
                {
                    this->priorsMethod = rsgis::rastergis::rsgis_area;
                }
                else if(priorsMethodStr == "samples")
                {
                    this->priorsMethod = rsgis::rastergis::rsgis_samples;
                }
                else
                {
                    this->priorsMethod = rsgis::rastergis::rsgis_userdefined;
                    try
                    {
                        this->priorStrs = textUtils.readFileToStringVector(priorsMethodStr);
                    }
                    catch(rsgis::utils::RSGISTextException &e)
                    {
                        throw rsgis::RSGISXMLArgumentsException("Error reading priors files. Options are equal | area | samples | file.txt");
                    }
                }
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'priors\' attribute was provided.");
            }
            xercesc::XMLString::release(&priorsXMLStr);
            
            
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
        else if(xercesc::XMLString::equals(optionMaxLikelihoodClassifierLocalPriors, optionXML))
        {
            this->option = RSGISExeRasterGIS::maxlikelihoodclassifierlocalpriors;
            
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
            
            XMLCh *inClassFieldXMLStr = xercesc::XMLString::transcode("inclassfield");
            if(argElement->hasAttribute(inClassFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(inClassFieldXMLStr));
                this->inClassNameField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'inclassfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&inClassFieldXMLStr);
            
            XMLCh *outClassFieldXMLStr = xercesc::XMLString::transcode("outclassfield");
            if(argElement->hasAttribute(outClassFieldXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(outClassFieldXMLStr));
                this->outClassNameField = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'outclassfield\' attribute was provided.");
            }
            xercesc::XMLString::release(&outClassFieldXMLStr);
            
            XMLCh *trainingColXMLStr = xercesc::XMLString::transcode("trainingcol");
            if(argElement->hasAttribute(trainingColXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(trainingColXMLStr));
                this->trainingSelectCol = std::string(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'trainingcol\' attribute was provided.");
            }
            xercesc::XMLString::release(&trainingColXMLStr);
            
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
            
            XMLCh *spatialRadiusXMLStr = xercesc::XMLString::transcode("spatialradius");
            if(argElement->hasAttribute(spatialRadiusXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(spatialRadiusXMLStr));
                this->distThreshold = textUtils.strtofloat(std::string(charValue));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw rsgis::RSGISXMLArgumentsException("No \'spatialradius\' attribute was provided.");
            }
            xercesc::XMLString::release(&spatialRadiusXMLStr);
            
            
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
        xercesc::XMLString::release(&optionFindSpecClose);
        xercesc::XMLString::release(&optionCopyGDALATTColumns);
        xercesc::XMLString::release(&optionPopAttributeStats);
        xercesc::XMLString::release(&optionPopCategoryProportions);
        xercesc::XMLString::release(&optionCopyCatColours);
        xercesc::XMLString::release(&optionKNNMajorityClassifier);
        xercesc::XMLString::release(&optionPopAttributePercentile);
        xercesc::XMLString::release(&optionExport2ASCII);
        xercesc::XMLString::release(&optionClassTranslate);
        xercesc::XMLString::release(&optionColourClasses);
        xercesc::XMLString::release(&optionColourStrClasses);
        xercesc::XMLString::release(&optionGenColourTab);
        xercesc::XMLString::release(&optionExportCols2Raster);
        xercesc::XMLString::release(&optionStrClassMajority);
        xercesc::XMLString::release(&optionSpecDistMajorityClassifier);
        xercesc::XMLString::release(&optionMaxLikelihoodClassifier);
        xercesc::XMLString::release(&optionMaxLikelihoodClassifierLocalPriors);
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
            else if(this->option == RSGISExeRasterGIS::findspecclose)
            {
                std::cout << "Calculate the features within a given spatial and spectral distance.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Distance Field: " << this->distanceField << std::endl;
                std::cout << "Spatial Dist. Field: " << this->spatialDistField << std::endl;
                std::cout << "Output Field: " << this->outputField << std::endl;
                std::cout << "Spatial Distance threshold: " << this->distThreshold << std::endl;
                std::cout << "Spectral Distance threshold: " << this->specDistThreshold << std::endl;
                
                try
                {
                    GDALAllRegister();
                    
                    GDALDataset *inputDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                    if(inputDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->inputImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    rsgis::rastergis::RSGISFindClosestSpecSpatialFeats findFeats;
                    findFeats.calcFeatsWithinSpatSpecThresholds(inputDataset, this->spatialDistField, this->distanceField, this->outputField, this->specDistThreshold, this->distThreshold);
                    
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
            else if(this->option == RSGISExeRasterGIS::popattributestats)
            {
                std::cout << "A command to populate an attribute table with statistics from an image in a memory efficient manor.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Clump Image: " << this->clumpsImage << std::endl;
                std::cout << "Statistics to be calculated:\n";
                for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {
                    std::cout << "Band " << (*iterBands)->band << ": ";
                    if((*iterBands)->calcMin)
                    {
                        std::cout << "MIN (" << (*iterBands)->minField << ") ";
                    }
                    
                    if((*iterBands)->calcMax)
                    {
                        std::cout << "MAX (" << (*iterBands)->maxField << ") ";
                    }
                    
                    if((*iterBands)->calcMean)
                    {
                        std::cout << "MEAN (" << (*iterBands)->meanField << ") ";
                    }
                    
                    if((*iterBands)->calcSum)
                    {
                        std::cout << "SUM (" << (*iterBands)->sumField << ") ";
                    }
                    
                    if((*iterBands)->calcStdDev)
                    {
                        std::cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                    }
                    
                    std::cout << std::endl;
                    
                    (*iterBands)->countIdxDef = false;
                    (*iterBands)->minIdxDef = false;
                    (*iterBands)->maxIdxDef = false;
                    (*iterBands)->meanIdxDef = false;
                    (*iterBands)->sumIdxDef = false;
                    (*iterBands)->stdDevIdxDef = false;
                    (*iterBands)->medianIdxDef = false;
                }
                
                try 
                {
                    GDALAllRegister();
                    
                    GDALDataset *clumpsDataset = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_Update);
                    if(clumpsDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->clumpsImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    GDALDataset *imageDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                    if(imageDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->inputImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    rsgis::rastergis::RSGISCalcClumpStats clumpStats;
                    clumpStats.calcImageClumpStatistic(clumpsDataset, imageDataset, bandStats);
                    
                    clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
                    
                    GDALClose(clumpsDataset);
                    GDALClose(imageDataset);
                } 
                catch (rsgis::RSGISException &e) 
                {
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::popcategoryproportions)
            {
                std::cout << "A command to populate an attribute table with the proportions of the categories overlapping the clumps\n";
                std::cout << "Clump Image: " << this->clumpsImage << std::endl;
                std::cout << "Categories Image: " << this->categoriesImage << std::endl;
                std::cout << "Output Column: " << this->outColsName << std::endl;
                std::cout << "Majority Column: " << this->majorityColName << std::endl;
                if(this->copyClassNames)
                {
                    std::cout << "Majority Column Name: " << this->majClassNameField << std::endl;
                    std::cout << "Class name column: " << this->classNameField << std::endl;
                }
                
                try 
                {
                    GDALAllRegister();
                    
                    GDALDataset *clumpsDataset = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_Update);
                    if(clumpsDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->clumpsImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    GDALDataset *catsDataset = (GDALDataset *) GDALOpenShared(this->categoriesImage.c_str(), GA_ReadOnly);
                    if(catsDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->categoriesImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    rsgis::rastergis::RSGISFindClumpCatagoryStats findClumpStats;
                    findClumpStats.calcCatergoriesOverlaps(clumpsDataset, catsDataset, this->outColsName, this->majorityColName, this->copyClassNames, this->majClassNameField, this->classNameField);                    
                    
                    clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
                    
                    GDALClose(clumpsDataset);
                    GDALClose(catsDataset);
                }
                catch (rsgis::RSGISAttributeTableException &e) 
                {
                    throw e;
                }
                catch (rsgis::RSGISException &e) 
                {
                    std::cout << e.what() << std::endl;
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::copycatcolours)
            {
                std::cout << "A command to copy the colours from classified data to clumps\n";
                std::cout << "Clump Image: " << this->clumpsImage << std::endl;
                std::cout << "Categories Image: " << this->categoriesImage << std::endl;
                std::cout << "Class Field: " << this->classField << std::endl;
                
                try 
                {
                    GDALAllRegister();
                    
                    GDALDataset *clumpsDataset = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_Update);
                    if(clumpsDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->clumpsImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    GDALDataset *catsDataset = (GDALDataset *) GDALOpenShared(this->categoriesImage.c_str(), GA_ReadOnly);
                    if(catsDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->categoriesImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    rsgis::rastergis::RSGISRasterAttUtils attUtils;
                    attUtils.copyColourForCats(clumpsDataset, catsDataset, this->classField);
                    
                    clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
                    
                    GDALClose(clumpsDataset);
                    GDALClose(catsDataset);
                }
                catch (rsgis::RSGISAttributeTableException &e) 
                {
                    throw e;
                }
                catch (rsgis::RSGISException &e) 
                {
                    std::cout << e.what() << std::endl;
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::knnmajorityclassifier)
            {
                std::cout << "A command to classify segments using KNN majority classification.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->inClassNameField << std::endl;
                std::cout << "Output Class Field: " << this->outClassNameField << std::endl;
                std::cout << "Selected Training: " << this->trainingSelectCol << std::endl;
                std::cout << "Distance threshold: " << this->distThreshold << std::endl;
                std::cout << "N: " << this->nFeatures << std::endl;
                std::cout << "Eastings Field: " << this->eastingsField << std::endl;
                std::cout << "Northings Field: " << this->northingsField << std::endl;
                std::cout << "Area Field: " << this->areaField << std::endl;
                std::cout << "Weight Field: " << this->majWeightField << std::endl;
                std::cout << "Weight A: " << this->weightA << std::endl;
                std::cout << "Distance calculated using:\n";
                for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
                {
                    std::cout << "\tField: " << (*iterFields) << std::endl;
                }
                if(this->majMethod == rsgis::rastergis::stdMajority)
                {
                    std::cout << "Using standard majority method\n";
                }
                else if(this->majMethod == rsgis::rastergis::stdMajority)
                {
                    std::cout << "Using standard majority method\n";
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
                    
                    rsgis::rastergis::RSGISKNNATTMajorityClassifier knnMajorityClass;
                    knnMajorityClass.applyKNNClassifier(inputDataset, this->inClassNameField, this->outClassNameField, this->trainingSelectCol, this->eastingsField, this->northingsField, this->areaField, this->majWeightField, this->fields, this->nFeatures, this->distThreshold, this->weightA, this->majMethod);
                    
                    GDALClose(inputDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::popattributepercentile)
            {
                std::cout << "A command to populate an attribute table with percentiles from the clumps within an image.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Clump Image: " << this->clumpsImage << std::endl;
                std::cout << "Percentiles to be calculated:\n";
                for(std::vector<rsgis::rastergis::RSGISBandAttPercentiles*>::iterator iterBands = bandPercentiles->begin(); iterBands != bandPercentiles->end(); ++iterBands)
                {
                    std::cout << "Band " << (*iterBands)->band << ": " << (*iterBands)->fieldName << " percentile " << (*iterBands)->percentile << std::endl;

                    (*iterBands)->fieldIdxDef = false;
                }
                
                try 
                {
                    GDALAllRegister();
                    
                    GDALDataset *clumpsDataset = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_Update);
                    if(clumpsDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->clumpsImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    GDALDataset *imageDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                    if(imageDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->inputImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    rsgis::rastergis::RSGISCalcClumpStats clumpStats;
                    clumpStats.calcImageClumpPercentiles(clumpsDataset, imageDataset, bandPercentiles);
                    
                    clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
                    
                    GDALClose(clumpsDataset);
                    GDALClose(imageDataset);
                } 
                catch (rsgis::RSGISException &e) 
                {
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::export2ascii)
            {
                std::cout << "A command to export columns from a GDAL RAT to ascii.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Output File: " << this->outputFile << std::endl;
                std::cout << "Fields:\n";
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
                    
                    rsgis::rastergis::RSGISRasterAttUtils attUtils;
                    attUtils.exportColumns2ASCII(inputDataset, this->outputFile, this->fields);
                    
                    GDALClose(inputDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::classtranslate)
            {
                std::cout << "A command to translate a set of classes to another\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Field: " << this->classInField << std::endl;
                std::cout << "Output Field: " << this->classOutField << std::endl;
                std::cout << "Class Pairs:\n";
                for(std::map<size_t, size_t>::iterator iterClass = classPairs.begin(); iterClass != classPairs.end(); ++iterClass)
                {
                    std::cout << "\tClass " << (*iterClass).first << " = " << (*iterClass).second << std::endl;
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
                    
                    rsgis::rastergis::RSGISRasterAttUtils attUtils;
                    attUtils.translateClasses(inputDataset, this->classInField, this->classOutField, this->classPairs);
                    
                    GDALClose(inputDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::colourclasses)
            {
                std::cout << "A command to set a colour table for a set of classes within the attribute table.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->classInField << std::endl;
                std::cout << "Class Colour Pairs:\n";
                for(std::map<size_t, rsgis::utils::RSGISColourInt>::iterator iterClass = classColourPairs.begin(); iterClass != classColourPairs.end(); ++iterClass)
                {
                    std::cout << "\tClass " << (*iterClass).first << " = [" << (*iterClass).second.getRed() << "," << (*iterClass).second.getGreen() << "," << (*iterClass).second.getBlue() << "," << (*iterClass).second.getAlpha() << "]" << std::endl;
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
                    
                    rsgis::rastergis::RSGISRasterAttUtils attUtils;
                    attUtils.applyClassColours(inputDataset, this->classInField, this->classColourPairs);
                    
                    GDALClose(inputDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::colourstrclasses)
            {
                std::cout << "A command to set a colour table for a set of classes (string column) within the attribute table.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->classInField << std::endl;
                std::cout << "Class Colour Pairs:\n";
                for(std::map<size_t, rsgis::utils::RSGISColourInt>::iterator iterClass = classColourPairs.begin(); iterClass != classColourPairs.end(); ++iterClass)
                {
                    std::cout << "\tClass \'" << (*iterClass).first << "\' = [" << (*iterClass).second.getRed() << "," << (*iterClass).second.getGreen() << "," << (*iterClass).second.getBlue() << "," << (*iterClass).second.getAlpha() << "]" << std::endl;
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
                    
                    rsgis::rastergis::RSGISRasterAttUtils attUtils;
                    attUtils.applyClassStrColours(inputDataset, this->classInField, this->classStrColourPairs);
                    
                    GDALClose(inputDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::gencolourtab)
            {
                std::cout << "A command to generate a colour table using an input image.";
                std::cout << "Clump Image: " << this->clumpsImage << std::endl;
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Red: " << this->redBand <<std::endl;
                std::cout << "Green: " << this->greenBand <<std::endl;
                std::cout << "Blue: " << this->blueBand <<std::endl;
                
                try
                {
                    GDALAllRegister();
                    
                    GDALDataset *inputDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                    if(inputDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->inputImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    GDALDataset *clumpsDataset = (GDALDataset *) GDALOpenShared(this->clumpsImage.c_str(), GA_Update);
                    if(clumpsDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->clumpsImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    rsgis::rastergis::RSGISCalcClumpStats clumpStats;
                    clumpStats.populateColourTable(clumpsDataset, inputDataset, this->redBand, this->greenBand, this->blueBand);
                    
                    clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
                    
                    GDALClose(inputDataset);
                    GDALClose(clumpsDataset);
                }
                catch (rsgis::RSGISException &e)
                {
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::exportcols2raster)
            {
                std::cout << "A command to export columns to a raster\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Output Format: " << this->imageFormat << std::endl;
                std::cout << "Output File: " << this->outputFile << std::endl;
                std::cout << "Fields:\n";
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
                    
                    rsgis::rastergis::RSGISRasterAttUtils attUtils;
                    const GDALRasterAttributeTable *gdalATT = inputDataset->GetRasterBand(1)->GetDefaultRAT();
                    
                    std::vector<unsigned int> *colIdxs = new std::vector<unsigned int>();
                    for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
                    {
                        colIdxs->push_back(attUtils.findColumnIndex(gdalATT, *iterFields));
                    }
                    
                    rsgis::rastergis::RSGISExportColumns2ImageCalcImage *calcImageVal = new rsgis::rastergis::RSGISExportColumns2ImageCalcImage(fields.size(), gdalATT, colIdxs);
                    rsgis::img::RSGISCalcImage calcImage(calcImageVal);
                    calcImage.calcImage(&inputDataset, 1, this->outputFile, true, fields.data(), this->imageFormat, outDataType);
                    delete calcImageVal;
                    
                    GDALClose(inputDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::strclassmajority)
            {
                std::cout << "A command to find the majority for class (string - field) from a set of small objects to large objects\n";
                std::cout << "Base Segments: " << this->baseSegment << std::endl;
                std::cout << "Info Segments: " << this->infoSegment << std::endl;
                std::cout << "Base Class Columns: " << this->baseClassCol << std::endl;
                std::cout << "Info Class Columns: " << this->infoClassCol << std::endl;
                
                try
                {
                    GDALAllRegister();
                    
                    GDALDataset *baseSegDataset = (GDALDataset *) GDALOpen(this->baseSegment.c_str(), GA_Update);
                    if(baseSegDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->baseSegment;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    GDALDataset *infoSegDataset = (GDALDataset *) GDALOpen(this->infoSegment.c_str(), GA_Update);
                    if(infoSegDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->infoSegment;
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    rsgis::rastergis::RSGISFindInfoBetweenLayers findClassMajority;
                    findClassMajority.findClassMajority(baseSegDataset, infoSegDataset, this->baseClassCol, this->infoClassCol);
                    
                    GDALClose(baseSegDataset);
                    GDALClose(infoSegDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::specdistmajorityclassifier)
            {
                std::cout << "A command to classify segments using a spectral distance majority classification.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->inClassNameField << std::endl;
                std::cout << "Output Class Field: " << this->outClassNameField << std::endl;
                std::cout << "Selected Training: " << this->trainingSelectCol << std::endl;
                std::cout << "Spectral Distance threshold: " << this->specDistThreshold << std::endl;
                std::cout << "Spatial Distance threshold: " << this->distThreshold << std::endl;
                std::cout << "Eastings Field: " << this->eastingsField << std::endl;
                std::cout << "Northings Field: " << this->northingsField << std::endl;
                std::cout << "Area Field: " << this->areaField << std::endl;
                std::cout << "Spectral Origin Distance: " << this->specThresOriginDist << std::endl;
                if(this->distThresMethod == rsgis::rastergis::stdEucSpecDist)
                {
                    std::cout << "Standard Euclidean Distance..." << std::endl;
                }
                else if(this->distThresMethod == rsgis::rastergis::stdEucSpecDist)
                {
                    std::cout << "Standard Euclidean Distance..." << std::endl;
                }
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
                    
                    rsgis::rastergis::RSGISFindClosestSpecSpatialFeats findFeats;
                    findFeats.applyMajorityClassifier(inputDataset, this->inClassNameField, this->outClassNameField, this->trainingSelectCol, this->eastingsField, this->northingsField, this->areaField, this->majWeightField, this->fields, this->distThreshold, this->specDistThreshold, this->distThresMethod, this->specThresOriginDist);
                    
                    GDALClose(inputDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::maxlikelihoodclassifier)
            {
                std::cout << "A command to classify segments using a spectral distance majority classification.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->inClassNameField << std::endl;
                std::cout << "Output Class Field: " << this->outClassNameField << std::endl;
                std::cout << "Selected Training: " << this->trainingSelectCol << std::endl;
                std::cout << "Area Field: " << this->areaField << std::endl;
                std::vector<float> priors;
                if(this->priorsMethod == rsgis::rastergis::rsgis_samples)
                {
                    std::cout << "Priors calculated using number of samples.\n";
                }
                else if(this->priorsMethod == rsgis::rastergis::rsgis_area)
                {
                    std::cout << "Priors calculated using area of samples.\n";
                }
                else if(this->priorsMethod == rsgis::rastergis::rsgis_equal)
                {
                    std::cout << "Priors will all be equal.\n";
                }
                else if(this->priorsMethod == rsgis::rastergis::rsgis_userdefined)
                {
                    std::cout << "Priors have been defined by the user as: \n";
                    int idx = 1;
                    rsgis::utils::RSGISTextUtils textUtils;
                    for(std::vector<std::string>::iterator iterStrs = this->priorStrs.begin(); iterStrs != this->priorStrs.end(); ++iterStrs)
                    {
                        std::cout << "\t" << idx << ": " << *iterStrs << std::endl;
                        try
                        {
                            priors.push_back(textUtils.strtofloat(*iterStrs));
                        }
                        catch (rsgis::utils::RSGISTextException &e)
                        {
                            throw rsgis::RSGISException(e.what());
                        }
                        ++idx;
                    }
                }
                
                std::cout << "Using Features:\n";
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
                    
                    rsgis::rastergis::RSGISMaxLikelihoodRATClassification mlRat;
                    mlRat.applyMLClassifier(inputDataset, this->inClassNameField, this->outClassNameField, this->trainingSelectCol, this->areaField, this->fields, this->priorsMethod, priors);
                    
                    GDALClose(inputDataset);
                }
                catch(rsgis::RSGISException &e)
                {
                    throw e;
                }
            }
            else if(this->option == RSGISExeRasterGIS::maxlikelihoodclassifierlocalpriors)
            {
                std::cout << "A command to classify segments using a spectral distance majority classification.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->inClassNameField << std::endl;
                std::cout << "Output Class Field: " << this->outClassNameField << std::endl;
                std::cout << "Selected Training: " << this->trainingSelectCol << std::endl;
                std::cout << "Eastings Field: " << this->eastingsField << std::endl;
                std::cout << "Northings Field: " << this->northingsField << std::endl;
                std::cout << "Search Radius: " << this->distThreshold << std::endl;
                std::cout << "Using Features:\n";
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
                    
                    rsgis::rastergis::RSGISMaxLikelihoodRATClassification mlRat;
                    mlRat.applyMLClassifierLocalPriors(inputDataset, this->inClassNameField, this->outClassNameField, this->trainingSelectCol, this->fields, this->eastingsField, this->northingsField, this->distThreshold);
                    
                    GDALClose(inputDataset);
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
            else if(this->option == RSGISExeRasterGIS::findtopn)
            {
                std::cout << "Calculate the top N features within a given spatial distance.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Distance Field: " << this->distanceField << std::endl;
                std::cout << "Spatial Dist. Field: " << this->spatialDistField << std::endl;
                std::cout << "Output Field: " << this->outputField << std::endl;
                std::cout << "Distance threshold: " << this->distThreshold << std::endl;
                std::cout << "N: " << this->nFeatures << std::endl;
            }
            else if(this->option == RSGISExeRasterGIS::findspecclose)
            {
                std::cout << "Calculate the features within a given spatial and spectral distance.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Distance Field: " << this->distanceField << std::endl;
                std::cout << "Spatial Dist. Field: " << this->spatialDistField << std::endl;
                std::cout << "Output Field: " << this->outputField << std::endl;
                std::cout << "Spatial Distance threshold: " << this->distThreshold << std::endl;
                std::cout << "Spectral Distance threshold: " << this->specDistThreshold << std::endl;
            }
            else if(this->option == RSGISExeRasterGIS::popattributestats)
            {
                std::cout << "A command to populate an attribute table with statistics from an image in a memory efficient manor.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Clump Image: " << this->clumpsImage << std::endl;
                std::cout << "Statistics to be calculated:\n";
                for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {
                    std::cout << "Band " << (*iterBands)->band << ": ";
                    if((*iterBands)->calcMin)
                    {
                        std::cout << "MIN (" << (*iterBands)->minField << ") ";
                    }
                    
                    if((*iterBands)->calcMax)
                    {
                        std::cout << "MAX (" << (*iterBands)->maxField << ") ";
                    }
                    
                    if((*iterBands)->calcMean)
                    {
                        std::cout << "MEAN (" << (*iterBands)->meanField << ") ";
                    }
                    
                    if((*iterBands)->calcSum)
                    {
                        std::cout << "SUM (" << (*iterBands)->sumField << ") ";
                    }
                    
                    if((*iterBands)->calcStdDev)
                    {
                        std::cout << "STDDEV (" << (*iterBands)->stdDevField << ") ";
                    }
                    
                    std::cout << std::endl;
                }
            }
            else if(this->option == RSGISExeRasterGIS::popcategoryproportions)
            {
                std::cout << "A command to populate an attribute table with the proportions of the categories overlapping the clumps\n";
                std::cout << "Clump Image: " << this->clumpsImage << std::endl;
                std::cout << "Categories Image: " << this->categoriesImage << std::endl;
                std::cout << "Output Column: " << this->outColsName << std::endl;
                std::cout << "Majority Column: " << this->majorityColName << std::endl;
                if(this->copyClassNames)
                {
                    std::cout << "Majority Column Name: " << this->majClassNameField << std::endl;
                    std::cout << "Class name column: " << this->classNameField << std::endl;
                }
            }
            else if(this->option == RSGISExeRasterGIS::copycatcolours)
            {
                std::cout << "A command to copy the colours from classified data to clumps\n";
                std::cout << "Clump Image: " << this->clumpsImage << std::endl;
                std::cout << "Categories Image: " << this->categoriesImage << std::endl;
                std::cout << "Class Field: " << this->classField << std::endl;
            }
            else if(this->option == RSGISExeRasterGIS::knnmajorityclassifier)
            {
                std::cout << "A command to classify segments using KNN majority classification.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->inClassNameField << std::endl;
                std::cout << "Output Class Field: " << this->outClassNameField << std::endl;
                std::cout << "Distance threshold: " << this->distThreshold << std::endl;
                std::cout << "N: " << this->nFeatures << std::endl;
                std::cout << "Eastings Field: " << this->eastingsField << std::endl;
                std::cout << "Northings Field: " << this->northingsField << std::endl;
                std::cout << "Area Field: " << this->areaField << std::endl;
                std::cout << "Weight Field: " << this->majWeightField << std::endl;
                std::cout << "Weight A: " << this->weightA << std::endl;
                std::cout << "Distance calculated using:\n";
                for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
                {
                    std::cout << "\tField: " << (*iterFields) << std::endl;
                }
                if(this->majMethod == rsgis::rastergis::stdMajority)
                {
                    std::cout << "Using standard majority method\n";
                }
                else if(this->majMethod == rsgis::rastergis::stdMajority)
                {
                    std::cout << "Using standard majority method\n";
                }
            }
            else if(this->option == RSGISExeRasterGIS::popattributepercentile)
            {
                std::cout << "A command to populate an attribute table with percentiles from the clumps within an image.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Clump Image: " << this->clumpsImage << std::endl;
                std::cout << "Percentiles to be calculated:\n";
                for(std::vector<rsgis::rastergis::RSGISBandAttPercentiles*>::iterator iterBands = bandPercentiles->begin(); iterBands != bandPercentiles->end(); ++iterBands)
                {
                    std::cout << "Band " << (*iterBands)->band << ": " << (*iterBands)->fieldName << " percentile " << (*iterBands)->percentile << std::endl;
                }
            }
            else if(this->option == RSGISExeRasterGIS::export2ascii)
            {
                std::cout << "A command to export columns from a GDAL RAT to ascii.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Output File: " << this->outputFile << std::endl;
                std::cout << "Fields:\n";
                for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
                {
                    std::cout << "\tField: " << (*iterFields) << std::endl;
                }
            }
            else if(this->option == RSGISExeRasterGIS::classtranslate)
            {
                std::cout << "A command to translate a set of classes to another\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Field: " << this->classInField << std::endl;
                std::cout << "Output Field: " << this->classOutField << std::endl;
                std::cout << "Class Pairs:\n";
                for(std::map<size_t, size_t>::iterator iterClass = classPairs.begin(); iterClass != classPairs.end(); ++iterClass)
                {
                    std::cout << "\tClass " << (*iterClass).first << " = " << (*iterClass).second << std::endl;
                }
            }
            else if(this->option == RSGISExeRasterGIS::colourclasses)
            {
                std::cout << "A command to set a colour table for a set of classes within the attribute table.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->classInField << std::endl;
                std::cout << "Class Colour Pairs:\n";
                for(std::map<size_t, rsgis::utils::RSGISColourInt>::iterator iterClass = classColourPairs.begin(); iterClass != classColourPairs.end(); ++iterClass)
                {
                    std::cout << "\tClass " << (*iterClass).first << " = [" << (*iterClass).second.getRed() << "," << (*iterClass).second.getGreen() << "," << (*iterClass).second.getBlue() << "," << (*iterClass).second.getAlpha() << "]" << std::endl;
                }
            }
            else if(this->option == RSGISExeRasterGIS::colourstrclasses)
            {
                std::cout << "A command to set a colour table for a set of classes (string column) within the attribute table.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->classInField << std::endl;
                std::cout << "Class Colour Pairs:\n";
                for(std::map<size_t, rsgis::utils::RSGISColourInt>::iterator iterClass = classColourPairs.begin(); iterClass != classColourPairs.end(); ++iterClass)
                {
                    std::cout << "\tClass \'" << (*iterClass).first << "\' = [" << (*iterClass).second.getRed() << "," << (*iterClass).second.getGreen() << "," << (*iterClass).second.getBlue() << "," << (*iterClass).second.getAlpha() << "]" << std::endl;
                }
            }
            else if(this->option == RSGISExeRasterGIS::gencolourtab)
            {
                std::cout << "A command to generate a colour table using an input image.";
                std::cout << "Clump Image: " << this->clumpsImage << std::endl;
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Red: " << this->redBand <<std::endl;
                std::cout << "Green: " << this->greenBand <<std::endl;
                std::cout << "Blue: " << this->blueBand <<std::endl;
            }
            else if(this->option == RSGISExeRasterGIS::exportcols2raster)
            {
                std::cout << "A command to export columns to a raster\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Output Format: " << this->imageFormat << std::endl;
                std::cout << "Output File: " << this->outputFile << std::endl;
                std::cout << "Fields:\n";
                for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
                {
                    std::cout << "\tField: " << (*iterFields) << std::endl;
                }
            }
            else if(this->option == RSGISExeRasterGIS::maxlikelihoodclassifier)
            {
                std::cout << "A command to classify segments using a spectral distance majority classification.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->inClassNameField << std::endl;
                std::cout << "Output Class Field: " << this->outClassNameField << std::endl;
                std::cout << "Selected Training: " << this->trainingSelectCol << std::endl;
                std::cout << "Using Features:\n";
                for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
                {
                    std::cout << "\tField: " << (*iterFields) << std::endl;
                }
            }
            else if(this->option == RSGISExeRasterGIS::maxlikelihoodclassifierlocalpriors)
            {
                std::cout << "A command to classify segments using a spectral distance majority classification.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->inClassNameField << std::endl;
                std::cout << "Output Class Field: " << this->outClassNameField << std::endl;
                std::cout << "Selected Training: " << this->trainingSelectCol << std::endl;
                std::cout << "Eastings Field: " << this->eastingsField << std::endl;
                std::cout << "Northings Field: " << this->northingsField << std::endl;
                std::cout << "Search Radius: " << this->distThreshold << std::endl;
                std::cout << "Using Features:\n";
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



