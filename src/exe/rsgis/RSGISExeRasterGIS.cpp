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
        try
        {
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
            XMLCh *optionClassMask = xercesc::XMLString::transcode("classmask");
            XMLCh *optionFindNeighbours = xercesc::XMLString::transcode("findneighbours");
            XMLCh *optionFindBoundaryPixels = xercesc::XMLString::transcode("findboundarypixels");
            XMLCh *optionCalcBorderLength = xercesc::XMLString::transcode("calcborderlength");
            XMLCh *optionCalcRelBorderLength = xercesc::XMLString::transcode("calcrelborderlength");
            XMLCh *optionCalcShapeIndices = xercesc::XMLString::transcode("calcshapeindices");
            XMLCh *optionDefineClumpTilePosition = xercesc::XMLString::transcode("defineclumptileposition");
            XMLCh *optionDefineBorderClumps = xercesc::XMLString::transcode("defineborderclumps");
            XMLCh *optionPopulateStats = xercesc::XMLString::transcode("populatestats");
            XMLCh *optionFindChangeClumpsFromStddev = xercesc::XMLString::transcode("findchangeclumpsfromstddev");

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

                bandStats = new std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>();
                bandStats->reserve(numBands);

                rsgis::cmds::RSGISBandAttStatsCmds *bandStat = NULL;
                xercesc::DOMElement *bandElement = NULL;
                for(int i = 0; i < numBands; i++)
                {
                    bandElement = static_cast<xercesc::DOMElement*>(bandNodesList->item(i));

                    bandStat = new rsgis::cmds::RSGISBandAttStatsCmds();

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

                bandPercentiles = new std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*>();
                bandPercentiles->reserve(numBands);

                rsgis::cmds::RSGISBandAttPercentilesCmds *bandPercentile = NULL;
                xercesc::DOMElement *bandElement = NULL;
                for(int i = 0; i < numBands; i++)
                {
                    bandElement = static_cast<xercesc::DOMElement*>(bandNodesList->item(i));

                    bandPercentile = new rsgis::cmds::RSGISBandAttPercentilesCmds();

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
                        this->rsgisOutDataType = rsgis::rsgis_8int;
                    }
                    else if(xercesc::XMLString::equals(dtUInt16, dtXMLValue))
                    {
                        this->rsgisOutDataType = rsgis::rsgis_16uint;
                    }
                    else if(xercesc::XMLString::equals(dtInt16, dtXMLValue))
                    {
                        this->rsgisOutDataType = rsgis::rsgis_16int;
                    }
                    else if(xercesc::XMLString::equals(dtUInt32, dtXMLValue))
                    {
                        this->rsgisOutDataType = rsgis::rsgis_32uint;
                    }
                    else if(xercesc::XMLString::equals(dtInt32, dtXMLValue))
                    {
                        this->rsgisOutDataType = rsgis::rsgis_32int;
                    }
                    else if(xercesc::XMLString::equals(dtFloat32, dtXMLValue))
                    {
                        this->rsgisOutDataType = rsgis::rsgis_32float;
                    }
                    else if(xercesc::XMLString::equals(dtFloat64, dtXMLValue))
                    {
                        this->rsgisOutDataType = rsgis::rsgis_64float;
                    }
                    else
                    {
                        std::cerr << "Data type not recognised, defaulting to 32 bit float.";
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
                    if(majMethodStr == "area")
                    {
                        this->priorsMethod = rsgis::rastergis::rsgis_area;
                    }
                    else if(majMethodStr == "weighted")
                    {
                        this->priorsMethod = rsgis::rastergis::rsgis_weighted;
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("Majority method is not recognised, options are \'area\' or \'weighted\'.");
                    }
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'majoritymethod\' attribute was provided.");
                }
                xercesc::XMLString::release(&majMethodXMLStr);

                XMLCh *noZeroPriorsXMLStr = xercesc::XMLString::transcode("nozeropriors");
                if(argElement->hasAttribute(noZeroPriorsXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(noZeroPriorsXMLStr));
                    std::string noZeroPriorsStr = std::string(charValue);
                    if(noZeroPriorsStr == "yes")
                    {
                        this->allowZeroPriors = true;
                    }
                    else
                    {
                        this->allowZeroPriors = false;
                    }
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'nozeropriors\' attribute was provided.");
                }
                xercesc::XMLString::release(&noZeroPriorsXMLStr);

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
            else if(xercesc::XMLString::equals(optionClassMask, optionXML))
            {
                this->option = RSGISExeRasterGIS::classmask;

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


                this->outDataType = GDT_Byte;
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
                        this->outDataType = GDT_Byte;
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

                XMLCh *classColumnXMLStr = xercesc::XMLString::transcode("classcolumn");
                if(argElement->hasAttribute(classColumnXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(classColumnXMLStr));
                    this->classField = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'classcolumn\' attribute was provided.");
                }
                xercesc::XMLString::release(&classColumnXMLStr);

                XMLCh *classXMLStr = xercesc::XMLString::transcode("class");
                if(argElement->hasAttribute(classXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(classXMLStr));
                    this->className = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'class\' attribute was provided.");
                }
                xercesc::XMLString::release(&classXMLStr);


            }
            else if(xercesc::XMLString::equals(optionFindNeighbours, optionXML))
            {
                this->option = RSGISExeRasterGIS::findneighbours;

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

            }
            else if(xercesc::XMLString::equals(optionFindBoundaryPixels, optionXML))
            {
                this->option = RSGISExeRasterGIS::findboundarypixels;

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
            }
            else if(xercesc::XMLString::equals(optionCalcBorderLength, optionXML))
            {
                this->option = RSGISExeRasterGIS::calcborderlength;

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

                XMLCh *ignoreZeroEdgesXMLStr = xercesc::XMLString::transcode("ignorezeroedges");
                if(argElement->hasAttribute(ignoreZeroEdgesXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(ignoreZeroEdgesXMLStr));
                    std::string ignoreZeroEdgesStr = std::string(charValue);
                    if(ignoreZeroEdgesStr == "yes")
                    {
                        this->ignoreZeroEdges = true;
                    }
                    else
                    {
                        this->ignoreZeroEdges = false;
                    }
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'ignorezeroedges\' attribute was provided.");
                }
                xercesc::XMLString::release(&ignoreZeroEdgesXMLStr);

                XMLCh *colNameXMLStr = xercesc::XMLString::transcode("colname");
                if(argElement->hasAttribute(colNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(colNameXMLStr));
                    this->outColsName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'colname\' attribute was provided.");
                }
                xercesc::XMLString::release(&colNameXMLStr);

            }
            else if(xercesc::XMLString::equals(optionCalcRelBorderLength, optionXML))
            {
                this->option = RSGISExeRasterGIS::calcrelborder;

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

                XMLCh *ignoreZeroEdgesXMLStr = xercesc::XMLString::transcode("ignorezeroedges");
                if(argElement->hasAttribute(ignoreZeroEdgesXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(ignoreZeroEdgesXMLStr));
                    std::string ignoreZeroEdgesStr = std::string(charValue);
                    if(ignoreZeroEdgesStr == "yes")
                    {
                        this->ignoreZeroEdges = true;
                    }
                    else
                    {
                        this->ignoreZeroEdges = false;
                    }
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'ignorezeroedges\' attribute was provided.");
                }
                xercesc::XMLString::release(&ignoreZeroEdgesXMLStr);

                XMLCh *colNameXMLStr = xercesc::XMLString::transcode("colname");
                if(argElement->hasAttribute(colNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(colNameXMLStr));
                    this->outColsName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'colname\' attribute was provided.");
                }
                xercesc::XMLString::release(&colNameXMLStr);

                XMLCh *classColumnXMLStr = xercesc::XMLString::transcode("classcolumn");
                if(argElement->hasAttribute(classColumnXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(classColumnXMLStr));
                    this->classNameField = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'classcolumn\' attribute was provided.");
                }
                xercesc::XMLString::release(&classColumnXMLStr);

                XMLCh *classNameXMLStr = xercesc::XMLString::transcode("classname");
                if(argElement->hasAttribute(classNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(classNameXMLStr));
                    this->className = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'classname\' attribute was provided.");
                }
                xercesc::XMLString::release(&classNameXMLStr);

            }
            else if(xercesc::XMLString::equals(optionCalcShapeIndices, optionXML))
            {
                this->option = RSGISExeRasterGIS::calcshapeindices;

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

                XMLCh *rsgisIndexXMLStr = xercesc::XMLString::transcode("rsgis:index");
                xercesc::DOMNodeList *indexNodesList = argElement->getElementsByTagName(rsgisIndexXMLStr);
                unsigned int numIndexTags = indexNodesList->getLength();

                std::cout << "Found " << numIndexTags << " index tags" << std::endl;

                if(numIndexTags == 0)
                {
                    throw rsgis::RSGISXMLArgumentsException("No index tags have been provided, at least 1 is required.");
                }

                shapeIndexes = new std::vector<rsgis::rastergis::RSGISShapeParam*>();
                shapeIndexes->reserve(numIndexTags);

                rsgis::rastergis::RSGISShapeParam *index = NULL;
                xercesc::DOMElement *attElement = NULL;
                for(int i = 0; i < numIndexTags; i++)
                {
                    attElement = static_cast<xercesc::DOMElement*>(indexNodesList->item(i));

                    index = new rsgis::rastergis::RSGISShapeParam();


                    XMLCh *nameXMLStr = xercesc::XMLString::transcode("name");
                    if(attElement->hasAttribute(nameXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(nameXMLStr));
                        index->idx = rsgis::rastergis::RSGISCalcClumpShapeParameters::getRSGISShapeIndex(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&nameXMLStr);


                    XMLCh *columnXMLStr = xercesc::XMLString::transcode("column");
                    if(attElement->hasAttribute(columnXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(columnXMLStr));
                        index->colName = std::string(charValue);
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'column\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&columnXMLStr);

                    shapeIndexes->push_back(index);
                }
            }
            else if(xercesc::XMLString::equals(optionDefineClumpTilePosition, optionXML))
            {
                this->option = RSGISExeRasterGIS::defineclumptileposition;

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

                XMLCh *tileXMLStr = xercesc::XMLString::transcode("tile");
                if(argElement->hasAttribute(tileXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(tileXMLStr));
                    this->tileImage = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'tile\' attribute was provided.");
                }
                xercesc::XMLString::release(&tileXMLStr);

                XMLCh *colNameXMLStr = xercesc::XMLString::transcode("colname");
                if(argElement->hasAttribute(colNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(colNameXMLStr));
                    this->outColsName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'colname\' attribute was provided.");
                }
                xercesc::XMLString::release(&colNameXMLStr);

                XMLCh *overlapXMLStr = xercesc::XMLString::transcode("overlap");
                if(argElement->hasAttribute(overlapXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(overlapXMLStr));
                    this->tileOverlap = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'overlap\' attribute was provided.");
                }
                xercesc::XMLString::release(&overlapXMLStr);


                XMLCh *boundaryXMLStr = xercesc::XMLString::transcode("boundary");
                if(argElement->hasAttribute(boundaryXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(boundaryXMLStr));
                    this->tileBoundary = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'boundary\' attribute was provided.");
                }
                xercesc::XMLString::release(&boundaryXMLStr);


                XMLCh *bodyXMLStr = xercesc::XMLString::transcode("body");
                if(argElement->hasAttribute(bodyXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bodyXMLStr));
                    this->tileBody = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'body\' attribute was provided.");
                }
                xercesc::XMLString::release(&bodyXMLStr);
            }
            else if(xercesc::XMLString::equals(optionDefineBorderClumps, optionXML))
            {
                this->option = RSGISExeRasterGIS::defineborderclumps;

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

                XMLCh *maskXMLStr = xercesc::XMLString::transcode("bordermask");
                if(argElement->hasAttribute(maskXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(maskXMLStr));
                    this->maskImage = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'bordermask\' attribute was provided.");
                }
                xercesc::XMLString::release(&maskXMLStr);

                XMLCh *colNameXMLStr = xercesc::XMLString::transcode("colname");
                if(argElement->hasAttribute(colNameXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(colNameXMLStr));
                    this->outColsName = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'colname\' attribute was provided.");
                }
                xercesc::XMLString::release(&colNameXMLStr);

                XMLCh *overlapXMLStr = xercesc::XMLString::transcode("overlap");
                if(argElement->hasAttribute(overlapXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(overlapXMLStr));
                    this->tileOverlap = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'overlap\' attribute was provided.");
                }
                xercesc::XMLString::release(&overlapXMLStr);


                XMLCh *boundaryXMLStr = xercesc::XMLString::transcode("boundary");
                if(argElement->hasAttribute(boundaryXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(boundaryXMLStr));
                    this->tileBoundary = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'boundary\' attribute was provided.");
                }
                xercesc::XMLString::release(&boundaryXMLStr);


                XMLCh *bodyXMLStr = xercesc::XMLString::transcode("body");
                if(argElement->hasAttribute(bodyXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(bodyXMLStr));
                    this->tileBody = textUtils.strto32bitInt(std::string(charValue));
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'body\' attribute was provided.");
                }
                xercesc::XMLString::release(&bodyXMLStr);

            }
            else if(xercesc::XMLString::equals(optionPopulateStats, optionXML))
            {
                this->option = RSGISExeRasterGIS::populatestats;

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

                XMLCh *pyramidsXMLStr = xercesc::XMLString::transcode("pyramids");
                if(argElement->hasAttribute(pyramidsXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(pyramidsXMLStr));
                    std::string typeStr = std::string(charValue);
                    if(typeStr == "yes")
                    {
                        this->calcImgPyramids = true;
                    }
                    else
                    {
                        this->calcImgPyramids = false;
                    }

                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    this->calcImgPyramids = false;
                }
                xercesc::XMLString::release(&pyramidsXMLStr);

                XMLCh *colourtableXMLStr = xercesc::XMLString::transcode("colourtable");
                if(argElement->hasAttribute(colourtableXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(colourtableXMLStr));
                    std::string typeStr = std::string(charValue);
                    if(typeStr == "yes")
                    {
                        this->addColourTable2Img = true;
                    }
                    else
                    {
                        this->addColourTable2Img = false;
                    }

                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    this->addColourTable2Img = false;
                }
                xercesc::XMLString::release(&colourtableXMLStr);

            }
            else if(xercesc::XMLString::equals(optionFindChangeClumpsFromStddev, optionXML))
            {
                this->option = RSGISExeRasterGIS::findchangeclumpsfromstddev;

                XMLCh *imageXMLStr = xercesc::XMLString::transcode("clumps");
                if(argElement->hasAttribute(imageXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(imageXMLStr));
                    this->clumpsImage = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'clumps\' attribute was provided.");
                }
                xercesc::XMLString::release(&imageXMLStr);

                XMLCh *classColXMLStr = xercesc::XMLString::transcode("classcol");
                if(argElement->hasAttribute(classColXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(classColXMLStr));
                    this->classField = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'classcol\' attribute was provided.");
                }
                xercesc::XMLString::release(&classColXMLStr);

                XMLCh *changeFieldXMLStr = xercesc::XMLString::transcode("changefield");
                if(argElement->hasAttribute(changeFieldXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(changeFieldXMLStr));
                    this->changeField = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'changefield\' attribute was provided.");
                }
                xercesc::XMLString::release(&changeFieldXMLStr);

                XMLCh *fieldsXMLStr = xercesc::XMLString::transcode("fields");
                if(argElement->hasAttribute(fieldsXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(argElement->getAttribute(fieldsXMLStr));
                    std::string fields = std::string(charValue);
                    attFields = new std::vector<std::string>();
                    textUtils.tokenizeString(fields, ',', attFields);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw rsgis::RSGISXMLArgumentsException("No \'fields\' attribute was provided.");
                }
                xercesc::XMLString::release(&fieldsXMLStr);

                XMLCh *rsgisClassXMLStr = xercesc::XMLString::transcode("rsgis:class");
                xercesc::DOMNodeList *classNodesList = argElement->getElementsByTagName(rsgisClassXMLStr);
                unsigned int numClassTags = classNodesList->getLength();

                std::cout << "Found " << numClassTags << " class tags" << std::endl;

                if(numClassTags == 0)
                {
                    throw rsgis::RSGISXMLArgumentsException("No class tags have been provided, at least 1 is required.");
                }
                classChangeField = new std::vector<rsgis::rastergis::RSGISClassChangeFields*>();
                classChangeField->reserve(numClassTags);

                xercesc::DOMElement *attElement = NULL;
                rsgis::rastergis::RSGISClassChangeFields *classChange = NULL;
                for(int i = 0; i < numClassTags; i++)
                {
                    attElement = static_cast<xercesc::DOMElement*>(classNodesList->item(i));

                    classChange = new rsgis::rastergis::RSGISClassChangeFields();

                    XMLCh *nameXMLStr = xercesc::XMLString::transcode("name");
                    if(attElement->hasAttribute(nameXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(nameXMLStr));
                        classChange->name = std::string(charValue);
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'name\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&nameXMLStr);

                    XMLCh *stddevThresXMLStr = xercesc::XMLString::transcode("stddevthres");
                    if(attElement->hasAttribute(stddevThresXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(stddevThresXMLStr));
                        classChange->threshold = textUtils.strtofloat(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'stddevthres\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&stddevThresXMLStr);

                    XMLCh *changeValXMLStr = xercesc::XMLString::transcode("changeval");
                    if(attElement->hasAttribute(changeValXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(attElement->getAttribute(changeValXMLStr));
                        classChange->outName = textUtils.strto32bitUInt(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw rsgis::RSGISXMLArgumentsException("No \'changeval\' attribute was provided.");
                    }
                    xercesc::XMLString::release(&changeValXMLStr);

                    classChangeField->push_back(classChange);
                }

                xercesc::XMLString::release(&rsgisClassXMLStr);
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
            xercesc::XMLString::release(&optionClassMask);
            xercesc::XMLString::release(&optionFindNeighbours);
            xercesc::XMLString::release(&optionFindBoundaryPixels);
            xercesc::XMLString::release(&optionCalcBorderLength);
            xercesc::XMLString::release(&optionCalcRelBorderLength);
            xercesc::XMLString::release(&optionCalcShapeIndices);
            xercesc::XMLString::release(&optionDefineClumpTilePosition);
            xercesc::XMLString::release(&optionDefineBorderClumps);
            xercesc::XMLString::release(&optionPopulateStats);
            xercesc::XMLString::release(&optionFindChangeClumpsFromStddev);
        }
        catch (rsgis::RSGISXMLArgumentsException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::RSGISXMLArgumentsException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::RSGISXMLArgumentsException(e.what());
        }
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

                try
                {
                    rsgis::cmds::executeCopyRAT(this->inputImage, this->clumpsImage);
                }
                catch(rsgis::cmds::RSGISCmdException &e)
                {
                    throw rsgis::RSGISException(e.what());
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
                    rsgis::cmds::executeSpatialLocation(this->inputImage, this->clumpsImage, this->eastingsField, this->northingsField);
                }
                catch(rsgis::cmds::RSGISCmdException &e)
                {
                    throw rsgis::RSGISException(e.what());
                }
            }
            else if(this->option == RSGISExeRasterGIS::eucdistfromfeat)
            {
                std::cout << "Calculate the euclidean distance from a feature to all other features.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Feature: " << this->fid << std::endl;
                std::cout << "Output Field: " << this->outputField << std::endl;
                std::cout << "Distance calculated using:\n";
                for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields) {
                    std::cout << "\tField: " << (*iterFields) << std::endl;
                }

                try {
                    rsgis::cmds::executeEucDistFromFeature(this->inputImage, this->fid, this->outputField, this->fields);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
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

                try {
                    rsgis::cmds::executeFindTopN(this->inputImage, this->spatialDistField, this->distanceField, this->outputField, this->nFeatures, this->distThreshold);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
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

                try {
                    rsgis::cmds::executeFindSpecClose(this->inputImage, this->distanceField, this->spatialDistField, this->outputField, this->specDistThreshold, this->distThreshold);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
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

                try
                {
                    rsgis::cmds::executeCopyGDALATTColumns(this->inputImage, this->clumpsImage, this->fields);
                }
                catch(rsgis::cmds::RSGISCmdException &e)
                {
                    throw rsgis::RSGISException(e.what());
                }
            }
            else if(this->option == RSGISExeRasterGIS::popattributestats)
            {
                std::cout << "A command to populate an attribute table with statistics from an image in a memory efficient manor.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Clump Image: " << this->clumpsImage << std::endl;
                std::cout << "Statistics to be calculated:\n";
                for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
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

                try
                {
                    rsgis::cmds::executePopulateRATWithStats(this->inputImage, this->clumpsImage, this->bandStats);
                }
                catch (rsgis::cmds::RSGISCmdException &e)
                {
                    throw rsgis::RSGISException(e.what());
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
                    rsgis::cmds::executePopulateCategoryProportions(this->categoriesImage, this->clumpsImage, this->outColsName, this->majorityColName, this->copyClassNames, this->majClassNameField, this->classNameField);
                }
                catch (rsgis::cmds::RSGISCmdException &e)
                {
                    throw rsgis::RSGISException(e.what());
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
                    rsgis::cmds::executeCopyCatagoriesColours(this->categoriesImage, this->clumpsImage, this->classField);
                }
                catch (rsgis::cmds::RSGISCmdException &e)
                {
                    throw rsgis::RSGISException(e.what());
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
                else if(this->majMethod == rsgis::rastergis::weightedMajority)
                {
                    std::cout << "Using weighted majority method\n";
                }

                try {
                    rsgis::cmds::executeKnnMajorityClassifier(this->inputImage, this->inClassNameField, this->outClassNameField, this->trainingSelectCol, this->eastingsField, this->northingsField, this->areaField, this->majWeightField, this->fields, this->nFeatures, this->distThreshold, this->weightA, &this->majMethod);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
                }
            }
            else if(this->option == RSGISExeRasterGIS::popattributepercentile)
            {
                std::cout << "A command to populate an attribute table with percentiles from the clumps within an image.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Clump Image: " << this->clumpsImage << std::endl;
                std::cout << "Percentiles to be calculated:\n";
                for(std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*>::iterator iterBands = bandPercentiles->begin(); iterBands != bandPercentiles->end(); ++iterBands)
                {
                    std::cout << "Band " << (*iterBands)->band << ": " << (*iterBands)->fieldName << " percentile " << (*iterBands)->percentile << std::endl;
                }

                try
                {
                    rsgis::cmds::executePopulateRATWithPercentiles(this->inputImage, this->clumpsImage, this->bandPercentiles);
                }
                catch (rsgis::cmds::RSGISCmdException &e)
                {
                    throw rsgis::RSGISException(e.what());
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

                try {
                    rsgis::cmds::executeExport2Ascii(this->inputImage, this->outputFile, this->fields);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
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

                try {
                    rsgis::cmds::executeClassTranslate(this->inputImage, this->classInField, this->classOutField, this->classPairs);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
                }
            }
            else if(this->option == RSGISExeRasterGIS::colourclasses)
            {
                std::cout << "A command to set a colour table for a set of classes within the attribute table.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->classInField << std::endl;
                std::cout << "Class Colour Pairs:\n";

                // print out colours, also translate to map using RSGISColourIntCmds
                int r,g,b,a;
                std::map<size_t, rsgis::cmds::RSGISColourIntCmds> ccPairs;
                for(std::map<size_t, rsgis::utils::RSGISColourInt>::iterator iterClass = this->classColourPairs.begin(); iterClass != this->classColourPairs.end(); ++iterClass) {
                    std::cout << "\tClass " << (*iterClass).first << " = [" << (*iterClass).second.getRed() << "," << (*iterClass).second.getGreen() << "," << (*iterClass).second.getBlue() << "," << (*iterClass).second.getAlpha() << "]" << std::endl;
                    r = (*iterClass).second.getRed();
                    g = (*iterClass).second.getGreen();
                    b = (*iterClass).second.getBlue();
                    a = (*iterClass).second.getAlpha();
                    ccPairs[(*iterClass).first] = rsgis::cmds::RSGISColourIntCmds(r, g, b, a);
                }

                try {
                    rsgis::cmds::executeColourClasses(this->inputImage, this->classInField, ccPairs);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
                }
            }
            else if(this->option == RSGISExeRasterGIS::colourstrclasses)
            {
                std::cout << "A command to set a colour table for a set of classes (string column) within the attribute table.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Input Class Field: " << this->classInField << std::endl;
                std::cout << "Class Colour Pairs:\n";
                // translate and print out colours
                int r,g,b,a;
                std::map<std::string, rsgis::cmds::RSGISColourIntCmds> ccPairs;
                for(std::map<std::string, rsgis::utils::RSGISColourInt>::iterator iterClass = classStrColourPairs.begin(); iterClass != classStrColourPairs.end(); ++iterClass) {
                    std::cout << "\tClass \'" << (*iterClass).first << "\' = [" << (*iterClass).second.getRed() << "," << (*iterClass).second.getGreen() << "," << (*iterClass).second.getBlue() << "," << (*iterClass).second.getAlpha() << "]" << std::endl;
                    r = (*iterClass).second.getRed();
                    g = (*iterClass).second.getGreen();
                    b = (*iterClass).second.getBlue();
                    a = (*iterClass).second.getAlpha();
                    ccPairs[(*iterClass).first] = rsgis::cmds::RSGISColourIntCmds(r, g, b, a);
                }

                try {
                    rsgis::cmds::executeColourStrClasses(this->inputImage, this->classInField, ccPairs);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
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

                try {
                    rsgis::cmds::executeGenerateColourTable(this->inputImage, this->clumpsImage, this->redBand, this->greenBand, this->blueBand);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
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
                    rsgis::cmds::executeExportCols2GDALImage(this->inputImage, this->outputFile, this->imageFormat, this->rsgisOutDataType, this->fields);
                }
                catch (rsgis::cmds::RSGISCmdException &e)
                {
                    throw rsgis::RSGISException(e.what());
                }

            }
            else if(this->option == RSGISExeRasterGIS::strclassmajority)
            {
                std::cout << "A command to find the majority for class (string - field) from a set of small objects to large objects\n";
                std::cout << "Base Segments: " << this->baseSegment << std::endl;
                std::cout << "Info Segments: " << this->infoSegment << std::endl;
                std::cout << "Base Class Columns: " << this->baseClassCol << std::endl;
                std::cout << "Info Class Columns: " << this->infoClassCol << std::endl;

                try {
                    rsgis::cmds::executeStrClassMajority(this->baseSegment, this->infoSegment, this->baseClassCol, this->infoClassCol);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
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

                try {
                    rsgis::cmds::executeSpecDistMajorityClassifier(this->inputImage, this->inClassNameField, this->outClassNameField, this->trainingSelectCol, this->eastingsField, this->northingsField, this->areaField, this->majWeightField, this->fields, this->distThreshold, this->specDistThreshold, (rsgis::cmds::SpectralDistanceMethodCmds)this->distThresMethod, this->specThresOriginDist);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
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
                if(this->priorsMethod == rsgis::rastergis::rsgis_samples) {
                    std::cout << "Priors calculated using number of samples.\n";
                } else if(this->priorsMethod == rsgis::rastergis::rsgis_area) {
                    std::cout << "Priors calculated using area of samples.\n";
                } else if(this->priorsMethod == rsgis::rastergis::rsgis_equal) {
                    std::cout << "Priors will all be equal.\n";
                } else if(this->priorsMethod == rsgis::rastergis::rsgis_userdefined) {
                    std::cout << "Priors have been defined by the user as: \n";
                    int idx = 1;
                    for(std::vector<std::string>::iterator iterStrs = this->priorStrs.begin(); iterStrs != this->priorStrs.end(); ++iterStrs) {
                        std::cout << "\t" << idx << ": " << *iterStrs << std::endl;
                        ++idx;
                    }
                }

                std::cout << "Using Features:\n";
                for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields) {
                    std::cout << "\tField: " << (*iterFields) << std::endl;
                }

                try {
                    rsgis::cmds::executeMaxLikelihoodClassifier(this->inputImage, this->inClassNameField, this->outClassNameField, this->trainingSelectCol, this->areaField, this->fields, (rsgis::cmds::rsgismlpriorscmds)this->priorsMethod, this->priorStrs);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
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
                std::cout << "Area Field: " << this->areaField << std::endl;
                std::cout << "Search Radius: " << this->distThreshold << std::endl;
                if(this->priorsMethod == rsgis::rastergis::rsgis_area)
                {
                    std::cout << "Local priors will be calculated using the area of all segments within radius\n";
                }
                else if(this->priorsMethod == rsgis::rastergis::rsgis_weighted)
                {
                    std::cout << "Local priors will be calculated using the weighted area of all segments within radius\n";
                    std::cout << "Weight: " << this->weightA << std::endl;
                }
                if(this->allowZeroPriors)
                {
                    std::cout << "Zero priors will be allowed\n";
                }
                else
                {
                    std::cout << "Zero priors will not be allowed\n";
                }
                std::cout << "Using Features:\n";
                for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
                {
                    std::cout << "\tField: " << (*iterFields) << std::endl;
                }

                try {
                    rsgis::cmds::executeMaxLikelihoodClassiferLocalPriors(this->inputImage, this->inClassNameField, this->outClassNameField, this->trainingSelectCol, this->areaField, this->fields, this->eastingsField, this->northingsField, this->distThreshold, (rsgis::cmds::rsgismlpriorscmds)this->priorsMethod, this->weightA, this->allowZeroPriors);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
                }
            }
            else if(this->option == RSGISExeRasterGIS::classmask)
            {
                std::cout << "A command to generate a mask for paraticular class\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Class Column: " << this->classField << std::endl;
                std::cout << "Class Name: " << this->className << std::endl;
                std::cout << "Output Format: " << this->imageFormat << std::endl;
                std::cout << "Output File: " << this->outputFile << std::endl;

                try {
                    rsgis::cmds::executeClassMask(this->inputImage, this->classField, this->className, this->outputFile, this->imageFormat, (rsgis::RSGISLibDataType)this->outDataType);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
                }
            }
            else if(this->option == RSGISExeRasterGIS::findneighbours)
            {
                std::cout << "A command to find the clump neighbours.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;

                try {
                    rsgis::cmds::executeFindNeighbours(this->inputImage);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
                }
            }
            else if(this->option == RSGISExeRasterGIS::findboundarypixels)
            {
                std::cout << "A command to identify the pixels on the boundary of the clumps.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Output Image: " << this->outputFile << std::endl;
                std::cout << "Output Image Format: " << this->imageFormat << std::endl;

                try {
                    rsgis::cmds::executeFindBoundaryPixels(this->inputImage, this->outputFile, this->imageFormat);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
                }
            }
            else if(this->option == RSGISExeRasterGIS::calcborderlength)
            {
                std::cout << "A command to calculate the border length of the clumps.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Output Column: " << this->outColsName << std::endl;
                if(ignoreZeroEdges)
                {
                    std::cout << "Ignoring zero edges on clumps\n";
                }
                else
                {
                    std::cout << "Not ignoring zero edges on clumps\n";
                }

                try {
                    rsgis::cmds::executeCalcBorderLength(this->inputImage, this->ignoreZeroEdges, this->outColsName);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
                }
            }
            else if(this->option == RSGISExeRasterGIS::calcrelborder)
            {
                std::cout << "A command to calculate the relative border length of the clumps to a class.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Output Column: " << this->outColsName << std::endl;
                std::cout << "Class Names Column: " << this->classNameField << std::endl;
                std::cout << "Class Name: " << this->className << std::endl;
                if(ignoreZeroEdges)
                {
                    std::cout << "Ignoring zero edges on clumps\n";
                }
                else
                {
                    std::cout << "Not ignoring zero edges on clumps\n";
                }

                try {
                    rsgis::cmds::executeCalcRelBorder(this->inputImage, this->outColsName, this->classNameField, this->className, this->ignoreZeroEdges);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
                }
            }
            else if(this->option == RSGISExeRasterGIS::calcshapeindices)
            {
                std::cout << "A command to calculate shape indices for clumps.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                for(std::vector<rsgis::rastergis::RSGISShapeParam*>::iterator iterIndexes = shapeIndexes->begin(); iterIndexes != shapeIndexes->end(); ++iterIndexes)
                {
                    std::cout << rsgis::rastergis::RSGISCalcClumpShapeParameters::getRSGISShapeIndexAsString((*iterIndexes)->idx) << " Index with output column name \'" << (*iterIndexes)->colName << "\'" << std::endl;
                }

                try {
                    // translate the structures to a vector of cmd types, cleanup this pointers
                    std::vector<cmds::RSGISShapeParamCmds> shapes;
                    shapes.reserve(this->shapeIndexes->size());
                    std::vector<cmds::RSGISShapeParam*>::iterator shapeIter;

                    for(shapeIter = this->shapeIndexes->begin(); shapeIter != this->shapeIndexes->end(); ++shapeIter) {
                        RSGISShapeParam in = (*shapeIter);
                        rsgis::cmds::RSGISShapeParamCmds out;
                        out.idx = (rsgisshapeindexcmds)in->idx;
                        out.colName = in->colName;
                        out.colIdx = in->colIdx;

                        shapes.push_back(out);

                        delete *shapeIter;
                    }
                    delete this->shapeIndexes;
                    // call function passing vector
                    rsgis::cmds::executeCalcShapeIndices(this->inputImage, shapes);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
                }
            }
            else if(this->option == RSGISExeRasterGIS::defineclumptileposition)
            {
                std::cout << "A command to define the position within the file of the clumps\n";
                std::cout << "Clumps: " << this->clumpsImage << std::endl;
                std::cout << "Tile: " << this->tileImage << std::endl;
                std::cout << "Output Column: " << this->outColsName << std::endl;
                std::cout << "Tile Overlap: " << this->tileOverlap << std::endl;
                std::cout << "Tile Boundary: " << this->tileBoundary << std::endl;
                std::cout << "Tile Body: " << this->tileBody << std::endl;

                try {
                    rsgis::cmds::executeDefineClumpTilePositions(this->clumpsImage, this->tileImage, this->outColsName, this->tileOverlap, this->tileBoundary, this->tileBody);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
                }

            }
            else if(this->option == RSGISExeRasterGIS::defineborderclumps)
            {
                std::cout << "A command to define the clumps which are on the border within the file of the clumps using a mask\n";
                std::cout << "Clumps: " << this->clumpsImage << std::endl;
                std::cout << "Border Mask: " << this->maskImage << std::endl;
                std::cout << "Output Column: " << this->outColsName << std::endl;
                std::cout << "Tile Overlap: " << this->tileOverlap << std::endl;
                std::cout << "Tile Boundary: " << this->tileBoundary << std::endl;
                std::cout << "Tile Body: " << this->tileBody << std::endl;

                try {
                    rsgis::cmds::executeDefineBorderClumps(this->clumpsImage, this->tileImage, this->outColsName, this->tileOverlap, this->tileBoundary, this->tileBody);
                } catch (rsgis::RSGISException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (rsgis::cmds::RSGISCmdException e) {
                    throw rsgis::RSGISException(e.what());
                } catch (std::exception e) {
                    throw rsgis::RSGISException(e.what());
                }

            }
            else if(this->option == RSGISExeRasterGIS::populatestats)
            {
                std::cout << "A command to define the clumps which are on the border within the file of the clumps using a mask\n";
                std::cout << "Clumps: " << this->clumpsImage << std::endl;
                if(calcImgPyramids)
                {
                    std::cout << "Image pyramids will be calculated\n";
                }
                else
                {
                    std::cout << "Image pyramids will NOT be calculated\n";
                }
                if(addColourTable2Img)
                {
                    std::cout << "A colour table will be added\n";
                }
                else
                {
                    std::cout << "A colour table will NOT be added\n";
                }

                try
                {
                    rsgis::cmds::executePopulateStats(this->clumpsImage, this->addColourTable2Img, this->calcImgPyramids);
                }
                catch(rsgis::cmds::RSGISCmdException &e)
                {
                    throw rsgis::RSGISException(e.what());
                }

            }
            else if(this->option == RSGISExeRasterGIS::findchangeclumpsfromstddev)
            {
                std::cout << "A command to identify segments which have changed by looking for statistical outliners (std dev) from class population\n";
                std::cout << "Clumps Image: " << this->clumpsImage << std::endl;
                std::cout << "Class Column: " << this->classField << std::endl;
                std::cout << "Change Field: " << this->changeField << std::endl;
                std::cout << "Fields:\n";
                for(std::vector<std::string>::iterator iterFields = attFields->begin(); iterFields != attFields->end(); ++iterFields)
                {
                    std::cout << "\t" << *iterFields << std::endl;
                }
                std::cout << "Class:\n";
                for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClass = classChangeField->begin(); iterClass != classChangeField->end(); ++iterClass)
                {
                    std::cout << "\t" << (*iterClass)->name << "\t" << (*iterClass)->outName << "\t" << (*iterClass)->threshold << std::endl;
                }

                try
                {
                    GDALAllRegister();

                    GDALDataset *clumpsDataset = (GDALDataset *) GDALOpen(this->clumpsImage.c_str(), GA_Update);
                    if(clumpsDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + this->clumpsImage;
                        throw rsgis::RSGISImageException(message.c_str());
                    }

                    rsgis::rastergis::RSGISFindChangeClumps changeClumps;
                    changeClumps.findChangeStdDevThreshold(clumpsDataset, this->classField, this->changeField, attFields, classChangeField);

                    GDALClose(clumpsDataset);
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
                for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
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
                for(std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*>::iterator iterBands = bandPercentiles->begin(); iterBands != bandPercentiles->end(); ++iterBands)
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
            else if(this->option == RSGISExeRasterGIS::classmask)
            {
                std::cout << "A command to generate a mask for paraticular class\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Class Column: " << this->classField << std::endl;
                std::cout << "Class Name: " << this->className << std::endl;
                std::cout << "Output Format: " << this->imageFormat << std::endl;
                std::cout << "Output File: " << this->outputFile << std::endl;
            }
            else if(this->option == RSGISExeRasterGIS::findneighbours)
            {
                std::cout << "A command to find the clump neighbours.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
            }
            else if(this->option == RSGISExeRasterGIS::calcborderlength)
            {
                std::cout << "A command to calculate the border length of the clumps.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Output Column: " << this->outColsName << std::endl;
                if(ignoreZeroEdges)
                {
                    std::cout << "Ignoring zero edges on clumps\n";
                }
                else
                {
                    std::cout << "Not ignoring zero edges on clumps\n";
                }
            }
            else if(this->option == RSGISExeRasterGIS::calcrelborder)
            {
                std::cout << "A command to calculate the relative border length of the clumps to a class.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                std::cout << "Output Column: " << this->outColsName << std::endl;
                std::cout << "Class Names Column: " << this->classNameField << std::endl;
                std::cout << "Class Name: " << this->className << std::endl;
                if(ignoreZeroEdges)
                {
                    std::cout << "Ignoring zero edges on clumps\n";
                }
                else
                {
                    std::cout << "Not ignoring zero edges on clumps\n";
                }
            }
            else if(this->option == RSGISExeRasterGIS::calcshapeindices)
            {
                std::cout << "A command to calculate shape indices for clumps.\n";
                std::cout << "Input Image: " << this->inputImage << std::endl;
                for(std::vector<rsgis::rastergis::RSGISShapeParam*>::iterator iterIndexes = shapeIndexes->begin(); iterIndexes != shapeIndexes->end(); ++iterIndexes)
                {
                    std::cout << rsgis::rastergis::RSGISCalcClumpShapeParameters::getRSGISShapeIndexAsString((*iterIndexes)->idx) << " Index with output column name \'" << (*iterIndexes)->colName << "\'" << std::endl;
                }
            }
            else if(this->option == RSGISExeRasterGIS::defineclumptileposition)
            {
                std::cout << "A command to define the position within the file of the clumps\n";
                std::cout << "Clumps: " << this->clumpsImage << std::endl;
                std::cout << "Tile: " << this->tileImage << std::endl;
                std::cout << "Output Column: " << this->outColsName << std::endl;
                std::cout << "Tile Overlap: " << this->tileOverlap << std::endl;
                std::cout << "Tile Boundary: " << this->tileBoundary << std::endl;
                std::cout << "Tile Body: " << this->tileBody << std::endl;
            }
            else if(this->option == RSGISExeRasterGIS::defineborderclumps)
            {
                std::cout << "A command to define the clumps which are on the border within the file of the clumps using a mask\n";
                std::cout << "Clumps: " << this->clumpsImage << std::endl;
                std::cout << "Border Mask: " << this->maskImage << std::endl;
                std::cout << "Output Column: " << this->outColsName << std::endl;
                std::cout << "Tile Overlap: " << this->tileOverlap << std::endl;
                std::cout << "Tile Boundary: " << this->tileBoundary << std::endl;
                std::cout << "Tile Body: " << this->tileBody << std::endl;
            }
            else if(this->option == RSGISExeRasterGIS::populatestats)
            {
                std::cout << "A command to define the clumps which are on the border within the file of the clumps using a mask\n";
                std::cout << "Clumps: " << this->clumpsImage << std::endl;
                if(calcImgPyramids)
                {
                    std::cout << "Image pyramids will be calculated\n";
                }
                else
                {
                    std::cout << "Image pyramids will NOT be calculated\n";
                }
                if(addColourTable2Img)
                {
                    std::cout << "A colour table will be added\n";
                }
                else
                {
                    std::cout << "A colour table will NOT be added\n";
                }
            }
            else if(this->option == RSGISExeRasterGIS::findchangeclumpsfromstddev)
            {
                std::cout << "A command to identify segments which have changed by looking for statistical outliners (std dev) from class population\n";
                std::cout << "Clumps Image: " << this->clumpsImage << std::endl;
                std::cout << "Class Column: " << this->classField << std::endl;
                std::cout << "Change Field: " << this->changeField << std::endl;
                std::cout << "Fields:\n";
                for(std::vector<std::string>::iterator iterFields = attFields->begin(); iterFields != attFields->end(); ++iterFields)
                {
                    std::cout << "\t" << *iterFields << std::endl;
                }
                std::cout << "Class:\n";
                for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClass = classChangeField->begin(); iterClass != classChangeField->end(); ++iterClass)
                {
                    std::cout << "\t" << (*iterClass)->name << "\t" << (*iterClass)->outName << "\t" << (*iterClass)->threshold << std::endl;
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
        std::cout << "<!-- Copy an attribute table from an existing GDAL datasets and add it to another layer -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"copyGDALATT\" table=\"input.kea\" image=\"output.kea\" />" << std::endl;
        std::cout << "<!-- Copy specified attribute table columns from one file to another -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"copyGDALATTColumns\" table=\"input.kea\" image=\"output.kea\" >" << std::endl;
        std::cout << "	<rsgis:field name=\"string\" />" << std::endl;
        std::cout << "  <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "  <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- Attribute the clumps with their mean eastings and northings -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"spatiallocation\" image=\"image.kea\" eastings=\"field name\" northings=\"field name\" />" << std::endl;
        std::cout << "<!-- Calculate the euclidean distance from feature -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"eucdistfromfeat\" image=\"image.kea\" feature=\"uint\" outfield=\"field name\" >" << std::endl;
        std::cout << "	<rsgis:field name=\"string\" />" << std::endl;
        std::cout << "  <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "  <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to calculate the top N features within a given spatial distance -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"findtopn\" image=\"image.kea\" spatialdist=\"string\" metricdist=\"string\" n=\"int\"" << std::endl;
        std::cout << " distthreshold=\"float\" outfield=\"string\" />" << std::endl;
        std::cout << "<!-- A command to calculate the features within a given spatial and spectral distance -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"findspecclose\" image=\"image.kea\" spatialdist=\"string\" metricdist=\"string\" " << std::endl;
        std::cout << "  specdistthreshold=\"float\" spatdistthreshold=\"float\" outfield=\"string\" />" << std::endl;
        std::cout << "<!-- A command to populate an attribute table with statistics from an image -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"popattributestats\" clumps=\"clumps.env\" input=\"image.env\" >" << std::endl;
        std::cout << "    <rsgis:band band=\"int\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] />" << std::endl;
        std::cout << "    <rsgis:band band=\"int\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] />" << std::endl;
        std::cout << "    <rsgis:band band=\"int\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] />" << std::endl;
        std::cout << "    <rsgis:band band=\"int\" [min=\"field\"] [max=\"field\"] [mean=\"field\"] [sum=\"field\"] [stddev=\"field\"] />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to populate an attribute table with percentiles from the clumps within an image -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"popattributepercentile\" clumps=\"clumps.env\" input=\"image.env\" >" << std::endl;
        std::cout << "    <rsgis:band band=\"int\" name=\"field\" percentile=\"int\" />" << std::endl;
        std::cout << "    <rsgis:band band=\"int\" name=\"field\" percentile=\"int\" />" << std::endl;
        std::cout << "	<rsgis:band band=\"int\" name=\"field\" percentile=\"int\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to populate an attribute table with the proportions of the categories overlapping the clumps -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"popcategoryproportions\" clumps=\"clumps.env\" categories=\"image.env\"" << std::endl;
        std::cout << "  outcols=\"string\" majority=\"column name\" [majclassname=\"string\" classname=\"string\"] />" << std::endl;
        std::cout << "<!-- A command to copy the colours from classified data to clumps -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"copycatcolours\" clumps=\"clumps.env\" categories=\"image.env\" classfield=\"string\" />" << std::endl;
        std::cout << "<!-- A command to classify segments using KNN majority classification -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"knnmajorityclassifier\" image=\"image.kea\" inclassfield=\"string\" " << std::endl;
        std::cout << "  outclassfield=\"string\" trainingcol=\"string\" eastings=\"string\" northings=\"string\" n=\"int\" area=\"string\" " << std::endl;
        std::cout << "  distthreshold=\"float\" weightfield=\"string\" majoritymethod=\"standardKNN | weightedKNN\" [weighta=\"float\"] >" << std::endl;
        std::cout << "	<rsgis:field name=\"string\" />" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to export columns from a GDAL RAT to ascii -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"export2ascii\" table=\"input.kea\" output=\"output.txt\" >" << std::endl;
        std::cout << "	<rsgis:field name=\"string\" />" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to translate a set of classes to another -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"classtranslate\" table=\"input.kea\" infield=\"string\" outfield=\"string\" >" << std::endl;
        std::cout << "	<rsgis:class inid=\"int\" outid=\"int\" />" << std::endl;
        std::cout << "    <rsgis:class inid=\"int\" outid=\"int\" />" << std::endl;
        std::cout << "	<rsgis:class inid=\"int\" outid=\"int\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to set a colour table for a set of classes within the attribute table. -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"colourclasses\" table=\"input.kea\" classfield=\"string\" >" << std::endl;
        std::cout << "	<rsgis:class id=\"int\" r=\"int\" g=\"int\" b=\"int\" a=\"int\" />" << std::endl;
        std::cout << "  <rsgis:class id=\"int\" r=\"int\" g=\"int\" b=\"int\" a=\"int\" />" << std::endl;
        std::cout << "	<rsgis:class id=\"int\" r=\"int\" g=\"int\" b=\"int\" a=\"int\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to set a colour table for a set of classes (string column) within the attribute table. -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"colourstrclasses\" table=\"input.kea\" classfield=\"string\" >" << std::endl;
        std::cout << "	<rsgis:class name=\"string\" r=\"int\" g=\"int\" b=\"int\" a=\"int\" />" << std::endl;
        std::cout << "    <rsgis:class name=\"string\" r=\"int\" g=\"int\" b=\"int\" a=\"int\" />" << std::endl;
        std::cout << "	<rsgis:class name=\"string\" r=\"int\" g=\"int\" b=\"int\" a=\"int\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to populate a colour table from an input image -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"gencolourtab\" table=\"clumps.env\" input=\"image.env\" red=\"int band\" " << std::endl;
        std::cout << "  green=\"int band\" blue=\"int band\" />" << std::endl;
        std::cout << "<!-- A command to export columns to a GDAL raster -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"exportcols2raster\" clumps=\"input.kea\" output=\"image_out.env\" format=\"GDAL Format\" " << std::endl;
        std::cout << "  datatype=\"Byte | UInt16 | Int16 | UInt32 | Int32 | Float32 | Float64\" >" << std::endl;
        std::cout << "	<rsgis:field name=\"string\" />" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to find the majority for class (string - field) from one set of objects to another set objects -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"strclassmajority\" base=\"image.kea\" baseclass=\"column\" info=\"image.kea\" " << std::endl;
        std::cout << "  infoclass=\"column\" />" << std::endl;
        std::cout << "<!-- A command to classify segments using a spectral distance majority classification -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"specdistmajorityclassifier\" image=\"image.kea\" inclassfield=\"string\" " << std::endl;
        std::cout << " outclassfield=\"string\" trainingcol=\"string\" eastings=\"string\" northings=\"string\" area=\"string\" specdistthreshold=\"float\" " << std::endl;
        std::cout << "  spatdistthreshold=\"float\" weightfield=\"string\" specdistmethod=\"euclidean | origineucweighted\" [thresorigindist=\"float\"] >" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to classify segments using a global maximum likelihood classifier -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"maxlikelihoodclassifier\" image=\"image.kea\" inclassfield=\"string\" " << std::endl;
        std::cout << "  outclassfield=\"string\" trainingcol=\"string\" area=\"string\" priors=\"file.txt | equal | area | samples\" >" << std::endl;
        std::cout << "	<rsgis:field name=\"string\" />" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to classify segments using maximum likelihood classifier with local priors -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"maxlikelihoodclassifierlocalpriors\" image=\"image.kea\" inclassfield=\"string\" " << std::endl;
        std::cout << "  outclassfield=\"string\" trainingcol=\"string\" area=\"string\" eastings=\"string\" northings=\"string\" spatialradius=\"float\"" << std::endl;
        std::cout << "  nozeropriors=\"yes | no\" majoritymethod=\"area | weighted\" [weighta=\"float\"] >" << std::endl;
        std::cout << "	  <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "    <rsgis:field name=\"string\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to create a binary mask for a class -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"classmask\" clumps=\"image.kea\" classcolumn=\"column\" class=\"string\" " << std::endl;
        std::cout << "  output=\"image.kea\" format=\"GDAL Format\" datatype=\"Byte | UInt16 | Int16 | UInt32 | Int32 | Float32 | Float64\" />" << std::endl;
        std::cout << "<!-- A command to find the clump neighbours and store within the KEA file - the clumps file must be a KEA file -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"findneighbours\" clumps=\"clumps.kea\" />" << std::endl;
        std::cout << "<!-- A command to identify the pixels on the boundary of the clumps -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"findboundarypixels\" clumps=\"clumps.kea\" output=\"image.env\" format=\"GDAL Format\" />" << std::endl;
        std::cout << "<!-- A command to calculate the border length of the clumps -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"calcborderlength\" clumps=\"clumps.kea\" ignorezeroedges=\"yes | no\" colname=\"string\" />" << std::endl;
        std::cout << "<!-- A command to calculate the relative border length of the clumps to a class -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"calcrelborderlength\" clumps=\"clumps.kea\" ignorezeroedges=\"yes | no\" colname=\"string\" " << std::endl;
        std::cout << "  classcolumn=\"string\" classname=\"string\" />" << std::endl;
        std::cout << "<!-- A command to calculate shape indices for clumps -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"calcshapeindices\" clumps=\"clumps.kea\" >" << std::endl;
        std::cout << "	<rsgis:index name=\"Area | Asymmetry | BorderIndex | BorderLength | Compactness| Density | EllipticFit | Length | LengthWidth | " << std::endl;
        std::cout << "    Width | MainDirection | RadiusLargestEnclosedEllipse | RadiusSmallestEnclosedEllipse | RectangularFit | Roundness | ShapeIndex\" " << std::endl;
        std::cout << "  column=\"string\" />" << std::endl;
        std::cout << "	<rsgis:index name=\"Area | Asymmetry | BorderIndex | BorderLength | Compactness| Density | EllipticFit | Length | LengthWidth | " << std::endl;
        std::cout << "    Width | MainDirection | RadiusLargestEnclosedEllipse | RadiusSmallestEnclosedEllipse | RectangularFit | Roundness | ShapeIndex\" " << std::endl;
        std::cout << "  column=\"string\" />" << std::endl;
        std::cout << "	<rsgis:index name=\"Area | Asymmetry | BorderIndex | BorderLength | Compactness| Density | EllipticFit | Length | LengthWidth | " << std::endl;
        std::cout << "    Width | MainDirection | RadiusLargestEnclosedEllipse | RadiusSmallestEnclosedEllipse | RectangularFit | Roundness | ShapeIndex\" " << std::endl;
        std::cout << "  column=\"string\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
        std::cout << "<!-- A command to define the position within the file of the clumps (i.e., on the tile border, in the overlap or the main body).-->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"defineclumptileposition\" clumps=\"clumps.kea\" tile=\"image.kea\" colname=\"string\" " << std::endl;
        std::cout << "  overlap=\"int\" boundary=\"int\" body=\"int\" />" << std::endl;
        std::cout << "<!-- A command to define the clumps which are on the border within the file of the clumps using a mask.-->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"defineborderclumps\" clumps=\"clumps.kea\" bordermask=\"image.kea\" " << std::endl;
        std::cout << "  colname=\"string\" overlap=\"int\" boundary=\"int\" body=\"int\" />" << std::endl;
        std::cout << "<!-- A command to populate an image with the statistics (maximum, minimum, histogram) required for the a thematic image -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"populatestats\" clumps=\"clumps.kea\" pyramids=\"yes | no\" colourtable=\"yes | no\" />" << std::endl;
        std::cout << "<!-- A command to identify segments which have changed by looking for statistical outliners (std dev) from class population -->" << std::endl;
        std::cout << "<rsgis:command algor=\"rastergis\" option=\"findchangeclumpsfromstddev\" clumps=\"clumps.kea\" classcol=\"string\" " << std::endl;
        std::cout << "  changefield=\"string\" fields=\"col1,col2,col3\" >" << std::endl;
        std::cout << "	  <rsgis:class name=\"string\" stddevthres=\"float\" changeval=\"int\" />" << std::endl;
        std::cout << "    <rsgis:class name=\"string\" stddevthres=\"float\" changeval=\"int\" />" << std::endl;
        std::cout << "    <rsgis:class name=\"string\" stddevthres=\"float\" changeval=\"int\" />" << std::endl;
        std::cout << "</rsgis:command>" << std::endl;
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



