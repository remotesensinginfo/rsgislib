/*
 *  RSGISCmdRasterGIS.cpp
 *
 *
 *  Created by Pete Bunting on 03/05/2013.
 *  Copyright 2013 RSGISLib.
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

#include "RSGISCmdRasterGIS.h"
#include "RSGISCmdParent.h"

#include "common/RSGISImageException.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISCalcClumpStats.h"
#include "rastergis/RSGISCalcClusterLocation.h"
#include "rastergis/RSGISFindClumpCatagoryStats.h"
#include "rastergis/RSGISExportColumns2Image.h"

namespace rsgis{ namespace cmds {
    
    void executePopulateStats(std::string clumpsImage, bool addColourTable2Img, bool calcImgPyramids)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *clumpsDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_Update);
            if(clumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            rsgis::rastergis::RSGISPopulateWithImageStats popImageStats;
            popImageStats.populateImageWithRasterGISStats(clumpsDataset, addColourTable2Img, calcImgPyramids);            
            
            GDALClose(clumpsDataset);
            GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeCoptRAT(std::string inputImage, std::string clumpsImage)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inputDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *outRATDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_Update);
            if(outRATDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            std::cout << "Import attribute table\n";
            const GDALRasterAttributeTable *gdalAtt = inputDataset->GetRasterBand(1)->GetDefaultRAT();
            
            std::cout << "Adding RAT\n";
            outRATDataset->GetRasterBand(1)->SetDefaultRAT(gdalAtt);
            outRATDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            GDALClose(inputDataset);
            GDALClose(outRATDataset);
            GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeCopyGDALATTColumns(std::string inputImage, std::string clumpsImage, std::vector<std::string> fields)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inputDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *outRATDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_Update);
            if(outRATDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            attUtils.copyAttColumns(inputDataset, outRATDataset, fields);
            
            outRATDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            GDALClose(inputDataset);
            GDALClose(outRATDataset);
            GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeSpatialLocation(std::string inputImage, std::string clumpsImage, std::string eastingsField, std::string northingsField)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inputDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::rastergis::RSGISCalcClusterLocation calcLoc;
            calcLoc.populateAttWithClumpLocation(inputDataset, eastingsField, northingsField);
            
            GDALClose(inputDataset);
            GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executePopulateRATWithStats(std::string inputImage, std::string clumpsImage, std::vector<rsgis::cmds::RSGISBandAttStatsCmds*> *bandStatsCmds)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *clumpsDataset = (GDALDataset *) GDALOpenShared(clumpsImage.c_str(), GA_Update);
            if(clumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            GDALDataset *imageDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(imageDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats = new std::vector<rsgis::rastergis::RSGISBandAttStats*>();
            bandStats->reserve(bandStatsCmds->size());
            
            rsgis::rastergis::RSGISBandAttStats *bandStat = NULL;
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iterBand = bandStatsCmds->begin(); iterBand != bandStatsCmds->end(); ++iterBand)
            {
                bandStat = new rsgis::rastergis::RSGISBandAttStats();
                bandStat->band = (*iterBand)->band;
                bandStat->threshold = (*iterBand)->threshold;
                bandStat->calcCount = (*iterBand)->calcCount;
                bandStat->countField = (*iterBand)->countField;
                bandStat->calcMin = (*iterBand)->calcMin;
                bandStat->minField = (*iterBand)->minField;
                bandStat->calcMax = (*iterBand)->calcMax;
                bandStat->maxField = (*iterBand)->maxField;
                bandStat->calcMean = (*iterBand)->calcMean;
                bandStat->meanField = (*iterBand)->meanField;
                bandStat->calcStdDev = (*iterBand)->calcStdDev;
                bandStat->stdDevField = (*iterBand)->stdDevField;
                bandStat->calcMedian = (*iterBand)->calcMedian;
                bandStat->medianField = (*iterBand)->medianField;
                bandStat->calcSum = (*iterBand)->calcSum;
                bandStat->sumField = (*iterBand)->sumField;
                
                bandStat->countIdxDef = false;
                bandStat->minIdxDef = false;
                bandStat->maxIdxDef = false;
                bandStat->meanIdxDef = false;
                bandStat->sumIdxDef = false;
                bandStat->stdDevIdxDef = false;
                bandStat->medianIdxDef = false;
                
                bandStats->push_back(bandStat);
                delete (*iterBand);
            }
            delete bandStatsCmds;
            
            rsgis::rastergis::RSGISCalcClumpStats clumpStats;
            clumpStats.calcImageClumpStatistic(clumpsDataset, imageDataset, bandStats);
            
            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBand = bandStats->begin(); iterBand != bandStats->end(); ++iterBand)
            {
                delete *iterBand;
            }
            delete bandStats;
            
            clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            GDALClose(clumpsDataset);
            GDALClose(imageDataset);
            GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executePopulateRATWithPercentiles(std::string inputImage, std::string clumpsImage, std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*> *bandPercentilesCmds)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *clumpsDataset = (GDALDataset *) GDALOpenShared(clumpsImage.c_str(), GA_Update);
            if(clumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            GDALDataset *imageDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(imageDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            std::vector<rsgis::rastergis::RSGISBandAttPercentiles*> *bandPercentiles = new std::vector<rsgis::rastergis::RSGISBandAttPercentiles*>();
            bandPercentiles->reserve(bandPercentilesCmds->size());
            
            rsgis::rastergis::RSGISBandAttPercentiles *bandPercentile;
            
            for(std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*>::iterator iterBand = bandPercentilesCmds->begin(); iterBand != bandPercentilesCmds->end(); ++iterBand)
            {
                bandPercentile = new rsgis::rastergis::RSGISBandAttPercentiles();
                
                bandPercentile->band = (*iterBand)->band;
                bandPercentile->percentile = (*iterBand)->percentile;
                bandPercentile->fieldName = (*iterBand)->fieldName;
                
                bandPercentile->fieldIdxDef = false;
                
                bandPercentiles->push_back(bandPercentile);
            }
            
            rsgis::rastergis::RSGISCalcClumpStats clumpStats;
            clumpStats.calcImageClumpPercentiles(clumpsDataset, imageDataset, bandPercentiles);
            
            for(std::vector<rsgis::rastergis::RSGISBandAttPercentiles*>::iterator iterBand = bandPercentiles->begin(); iterBand != bandPercentiles->end(); ++iterBand)
            {
                delete *iterBand;
            }
            delete bandPercentiles;
            
            clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            GDALClose(clumpsDataset);
            GDALClose(imageDataset);
            GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executePopulateCategoryProportions(std::string categoriesImage, std::string clumpsImage, std::string outColsName, std::string majorityColName, bool copyClassNames, std::string majClassNameField, std::string classNameField)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *clumpsDataset = (GDALDataset *) GDALOpenShared(clumpsImage.c_str(), GA_Update);
            if(clumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            GDALDataset *catsDataset = (GDALDataset *) GDALOpenShared(categoriesImage.c_str(), GA_ReadOnly);
            if(catsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + categoriesImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::rastergis::RSGISFindClumpCatagoryStats findClumpStats;
            findClumpStats.calcCatergoriesOverlaps(clumpsDataset, catsDataset, outColsName, majorityColName, copyClassNames, majClassNameField, classNameField);
            
            clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            GDALClose(clumpsDataset);
            GDALClose(catsDataset);
            GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeCopyCatagoriesColours(std::string categoriesImage, std::string clumpsImage, std::string classField)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *clumpsDataset = (GDALDataset *) GDALOpenShared(clumpsImage.c_str(), GA_Update);
            if(clumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            GDALDataset *catsDataset = (GDALDataset *) GDALOpenShared(categoriesImage.c_str(), GA_ReadOnly);
            if(catsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + categoriesImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            attUtils.copyColourForCats(clumpsDataset, catsDataset, classField);
            
            clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            GDALClose(clumpsDataset);
            GDALClose(catsDataset);
            GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeExportCols2GDALImage(std::string inputImage, std::string outputFile, std::string imageFormat, RSGISLibDataType outDataType, std::vector<std::string> fields)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inputDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            const GDALRasterAttributeTable *gdalATT = inputDataset->GetRasterBand(1)->GetDefaultRAT();
            
            std::vector<unsigned int> *colIdxs = new std::vector<unsigned int>();
            std::string *bandNames = new std::string[fields.size()];
            unsigned int i = 0;
            for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
            {
                bandNames[i] = *iterFields;
                colIdxs->push_back(attUtils.findColumnIndex(gdalATT, *iterFields));
                ++i;
            }
            
            rsgis::rastergis::RSGISExportColumns2ImageCalcImage *calcImageVal = new rsgis::rastergis::RSGISExportColumns2ImageCalcImage(fields.size(), gdalATT, colIdxs);
            rsgis::img::RSGISCalcImage calcImage(calcImageVal);
            calcImage.calcImage(&inputDataset, 1, outputFile, true, bandNames, imageFormat, RSGIS_to_GDAL_Type(outDataType));
            delete calcImageVal;
            delete[] bandNames;
            
            GDALClose(inputDataset);
            GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
}}

