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
#include "common/RSGISAttributeTableException.h"

#include "utils/RSGISTextUtils.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISPopRATWithStats.h"
#include "rastergis/RSGISCalcImageStatsAndPyramids.h"
/*
#include "rastergis/RSGISCalcClumpStats.h"
#include "rastergis/RSGISCalcClusterLocation.h"
#include "rastergis/RSGISFindClumpCatagoryStats.h"
#include "rastergis/RSGISExportColumns2Image.h"
#include "rastergis/RSGISCalcEucDistanceInAttTable.h"
#include "rastergis/RSGISFindTopNWithinDist.h"
#include "rastergis/RSGISFindClosestSpecSpatialFeats.h"
#include "rastergis/RSGISKNNATTMajorityClassifier.h"
#include "rastergis/RSGISFindInfoBetweenLayers.h"
#include "rastergis/RSGISMaxLikelihoodRATClassification.h"
#include "rastergis/RSGISClassMask.h"
#include "rastergis/RSGISFindClumpNeighbours.h"
#include "rastergis/RSGISClumpBorders.h"
#include "rastergis/RSGISCalcClumpShapeParameters.h"
#include "rastergis/RSGISDefineImageTiles.h"
#include "rastergis/RSGISFindChangeClumps.h"
#include "rastergis/RSGISSelectClumps.h"
#include "rastergis/RSGISInterpolateClumpValues2Image.h"
 
 #include "math/RSGIS2DInterpolation.h"

*/
namespace rsgis{ namespace cmds {

    void executePopulateStats(std::string clumpsImage, bool addColourTable2Img, bool calcImgPyramids, bool ignoreZero, unsigned int ratBand)throw(RSGISCmdException)
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
            popImageStats.populateImageWithRasterGISStats(clumpsDataset, addColourTable2Img, calcImgPyramids, ignoreZero, ratBand);

            GDALClose(clumpsDataset);
            //GDALDestroyDriverManager();
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
/*
    void executeCopyRAT(std::string inputImage, std::string clumpsImage)throw(RSGISCmdException) {
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
            //GDALDestroyDriverManager();
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

    void executeCopyGDALATTColumns(std::string inputImage, std::string clumpsImage, std::vector<std::string> fields)throw(RSGISCmdException) {
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
            //GDALDestroyDriverManager();
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

    void executeSpatialLocation(std::string inputImage, std::string eastingsField, std::string northingsField)throw(RSGISCmdException) {
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
            //GDALDestroyDriverManager();
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
*/
    void executePopulateRATWithStats(std::string inputImage, std::string clumpsImage, std::vector<rsgis::cmds::RSGISBandAttStatsCmds*> *bandStatsCmds, unsigned int ratBand)throw(RSGISCmdException)
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
                bandStat->calcMin = (*iterBand)->calcMin;
                bandStat->minField = (*iterBand)->minField;
                bandStat->calcMax = (*iterBand)->calcMax;
                bandStat->maxField = (*iterBand)->maxField;
                bandStat->calcMean = (*iterBand)->calcMean;
                bandStat->meanField = (*iterBand)->meanField;
                bandStat->calcStdDev = (*iterBand)->calcStdDev;
                bandStat->stdDevField = (*iterBand)->stdDevField;
                bandStat->calcSum = (*iterBand)->calcSum;
                bandStat->sumField = (*iterBand)->sumField;

                bandStats->push_back(bandStat);
            }

            rsgis::rastergis::RSGISPopRATWithStats clumpStats;
            clumpStats.populateRATWithBasicStats(clumpsDataset, imageDataset, bandStats, ratBand);

            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBand = bandStats->begin(); iterBand != bandStats->end(); ++iterBand)
            {
                delete *iterBand;
            }
            delete bandStats;

            clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");

            GDALClose(clumpsDataset);
            GDALClose(imageDataset);
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
/*
    void executePopulateRATWithPercentiles(std::string inputImage, std::string clumpsImage, std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*> *bandPercentilesCmds)throw(RSGISCmdException) {
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
            //GDALDestroyDriverManager();
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

    void executePopulateCategoryProportions(std::string categoriesImage, std::string clumpsImage, std::string outColsName, std::string majorityColName, bool copyClassNames, std::string majClassNameField, std::string classNameField)throw(RSGISCmdException) {
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
            //GDALDestroyDriverManager();
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

    void executeCopyCategoriesColours(std::string categoriesImage, std::string clumpsImage, std::string classField)throw(RSGISCmdException) {
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
            //GDALDestroyDriverManager();
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

    void executeExportCols2GDALImage(std::string inputImage, std::string outputFile, std::string imageFormat, RSGISLibDataType outDataType, std::vector<std::string> fields)throw(RSGISCmdException) {
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
            //GDALDestroyDriverManager();
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

    void executeEucDistFromFeature(std::string inputImage, size_t fid, std::string outputField, std::vector<std::string> fields)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;

        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISCalcEucDistanceInAttTable calcDist;
            calcDist.calcEucDist(inputDataset, fid, outputField, fields);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeFindTopN(std::string inputImage, std::string spatialDistField, std::string distanceField, std::string outputField, unsigned int nFeatures, float distThreshold)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;

        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);

            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISFindTopNWithinDist calcTopN;
            calcTopN.calcMinDistTopN(inputDataset, spatialDistField, distanceField, outputField, nFeatures, distThreshold);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeFindSpecClose(std::string inputImage, std::string distanceField, std::string spatialDistField, std::string outputField, float specDistThreshold, float distThreshold)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;

        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);

            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISFindClosestSpecSpatialFeats findFeats;
            findFeats.calcFeatsWithinSpatSpecThresholds(inputDataset, spatialDistField, distanceField, outputField, specDistThreshold, distThreshold);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }

    }

    void executeKnnMajorityClassifier(std::string inputImage, std::string inClassNameField, std::string outClassNameField, std::string trainingSelectCol, std::string eastingsField, std::string northingsField, std::string areaField, std::string majWeightField, std::vector<std::string> fields, unsigned int nFeatures, float distThreshold, float weightA, void *majorMethod)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;

        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);

            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::ClassMajorityMethod *majMethodPtr = (rsgis::rastergis::ClassMajorityMethod *) majorMethod;

            rsgis::rastergis::RSGISKNNATTMajorityClassifier knnMajorityClass;
            knnMajorityClass.applyKNNClassifier(inputDataset, inClassNameField, outClassNameField, trainingSelectCol, eastingsField, northingsField, areaField, majWeightField, fields, nFeatures, distThreshold,weightA, *majMethodPtr);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }

    }

    void executeExport2Ascii(std::string inputImage, std::string outputFile, std::vector<std::string> fields)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;

        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);

            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            attUtils.exportColumns2ASCII(inputDataset, outputFile, fields);

            GDALClose(inputDataset);
        }
        catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeClassTranslate(std::string inputImage, std::string classInField, std::string classOutField, std::map<size_t, size_t> classPairs)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;

        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);

            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            attUtils.translateClasses(inputDataset, classInField, classOutField, classPairs);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeColourClasses(std::string inputImage, std::string classInField, std::map<size_t, RSGISColourIntCmds> classColourPairs)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;

        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);

            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            // translate from RSGISColourIntCmds to RSGISColourInt object
            std::map<size_t, utils::RSGISColourInt> ccPairs;
            int r,g,b,a;

            for(std::map<size_t, RSGISColourIntCmds>::iterator iterClass = classColourPairs.begin(); iterClass != classColourPairs.end(); ++iterClass) {
                r = (*iterClass).second.getRed();
                g = (*iterClass).second.getGreen();
                b = (*iterClass).second.getBlue();
                a = (*iterClass).second.getAlpha();
                ccPairs[(*iterClass).first] = utils::RSGISColourInt(r, g, b, a);
            }

            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            attUtils.applyClassColours(inputDataset, classInField, ccPairs);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeColourStrClasses(std::string inputImage, std::string classInField, std::map<std::string, RSGISColourIntCmds> classStrColourPairs)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;
        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);

            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            // translate to map using RSGISColourInt type
            std::map<std::string, utils::RSGISColourInt> ccPairs;
            int r,g,b,a;

            for(std::map<std::string, RSGISColourIntCmds>::iterator iterClass = classStrColourPairs.begin(); iterClass != classStrColourPairs.end(); ++iterClass) {
                r = (*iterClass).second.getRed();
                g = (*iterClass).second.getGreen();
                b = (*iterClass).second.getBlue();
                a = (*iterClass).second.getAlpha();
                ccPairs[(*iterClass).first] = utils::RSGISColourInt(r, g, b, a);
            }

            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            attUtils.applyClassStrColours(inputDataset, classInField, ccPairs);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeGenerateColourTable(std::string inputImage, std::string clumpsImage, unsigned int redBand, unsigned int greenBand, unsigned int blueBand)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset, *clumpsDataset;
        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            clumpsDataset = (GDALDataset *) GDALOpenShared(clumpsImage.c_str(), GA_Update);
            if(clumpsDataset == NULL) {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISCalcClumpStats clumpStats;
            clumpStats.populateColourTable(clumpsDataset, inputDataset, redBand, greenBand, blueBand);

            clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");

            GDALClose(inputDataset);
            GDALClose(clumpsDataset);
        } catch (rsgis::RSGISException &e) {
            throw e;
        }

    }

    void executeStrClassMajority(std::string baseSegment, std::string infoSegment, std::string baseClassCol, std::string infoClassCol, bool ignoreZero)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *baseSegDataset, *infoSegDataset;
        try {
            baseSegDataset = (GDALDataset *) GDALOpen(baseSegment.c_str(), GA_Update);
            if(baseSegDataset == NULL) {
                std::string message = std::string("Could not open image ") + baseSegment;
                throw rsgis::RSGISImageException(message.c_str());
            }

            infoSegDataset = (GDALDataset *) GDALOpen(infoSegment.c_str(), GA_Update);
            if(infoSegDataset == NULL) {
                std::string message = std::string("Could not open image ") + infoSegment;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISFindInfoBetweenLayers findClassMajority;
            findClassMajority.findClassMajority(baseSegDataset, infoSegDataset, baseClassCol, infoClassCol, ignoreZero);

            GDALClose(baseSegDataset);
            GDALClose(infoSegDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeSpecDistMajorityClassifier(std::string inputImage, std::string inClassNameField, std::string outClassNameField, std::string trainingSelectCol, std::string eastingsField, std::string northingsField, std::string areaField, std::string majWeightField, std::vector<std::string> fields, float distThreshold, float specDistThreshold, SpectralDistanceMethodCmds distMethod, float specThresOriginDist)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;
        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::SpectralDistanceMethod distThresMethod = (rsgis::rastergis::SpectralDistanceMethod) distMethod;

            rsgis::rastergis::RSGISFindClosestSpecSpatialFeats findFeats;
            findFeats.applyMajorityClassifier(inputDataset, inClassNameField, outClassNameField, trainingSelectCol, eastingsField, northingsField, areaField, majWeightField, fields, distThreshold, specDistThreshold, distThresMethod, specThresOriginDist);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeMaxLikelihoodClassifier(std::string inputImage, std::string inClassNameField, std::string outClassNameField, std::string trainingSelectCol,
            std::string classifySelectCol, std::string areaField, std::vector<std::string> fields, rsgismlpriorscmds priorsMethod, std::vector<std::string> priorStrs)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;
        std::vector<float> priors;
        try
        {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);

            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            // if the method is user defined we need to unpack the priorsStrs vector into priors
            if(priorsMethod == rsgis_userdefined) {
                rsgis::utils::RSGISTextUtils textUtils;
                for(std::vector<std::string>::iterator iterStrs = priorStrs.begin(); iterStrs != priorStrs.end(); ++iterStrs) {
                    try {
                        priors.push_back(textUtils.strtofloat(*iterStrs));
                    } catch (rsgis::utils::RSGISTextException &e) {
                        throw rsgis::RSGISException(e.what());
                    }
                }
            }

            rsgis::rastergis::rsgismlpriors priMeth = (rsgis::rastergis::rsgismlpriors) priorsMethod;

            rsgis::rastergis::RSGISMaxLikelihoodRATClassification mlRat;
            mlRat.applyMLClassifier(inputDataset, inClassNameField, outClassNameField, trainingSelectCol, classifySelectCol, areaField, fields, priMeth, priors);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeMaxLikelihoodClassifierLocalPriors(std::string inputImage, std::string inClassNameField, std::string outClassNameField, std::string trainingSelectCol, std::string classifySelectCol,
                                                  std::string areaField, std::vector<std::string> fields, std::string eastingsField, std::string northingsField,
                                                  float distThreshold, rsgismlpriorscmds priorsMethod, float weightA, bool allowZeroPriors, bool forceChangeInClassification)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;
        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISMaxLikelihoodRATClassification mlRat;

            rsgis::rastergis::rsgismlpriors priMeth = (rsgis::rastergis::rsgismlpriors) priorsMethod;

            mlRat.applyMLClassifierLocalPriors(inputDataset, inClassNameField, outClassNameField, trainingSelectCol, classifySelectCol, areaField,
                    fields, eastingsField, northingsField, distThreshold, priMeth, weightA, allowZeroPriors, forceChangeInClassification);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeClassMask(std::string inputImage, std::string classField, std::string className, std::string outputFile, std::string imageFormat, RSGISLibDataType dataType)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;
        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            const GDALRasterAttributeTable *gdalATT = inputDataset->GetRasterBand(1)->GetDefaultRAT();

            unsigned int colIdx = attUtils.findColumnIndex(gdalATT, classField);
            std::string *bandNames = new std::string[1];
            bandNames[0] = std::string("MASK - ") + className;

            rsgis::rastergis::RSGISClassMask *calcImageVal = new rsgis::rastergis::RSGISClassMask(gdalATT, colIdx, className);
            rsgis::img::RSGISCalcImage calcImage(calcImageVal);

            GDALDataType outDataType = (GDALDataType) dataType;

            calcImage.calcImage(&inputDataset, 1, outputFile, true, bandNames, imageFormat, outDataType);
            delete calcImageVal;
            delete[] bandNames;

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeFindNeighbours(std::string inputImage)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;
        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISFindClumpNeighbours findNeighboursObj;
            findNeighboursObj.findNeighboursKEAImageCalc(inputDataset);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeFindBoundaryPixels(std::string inputImage, std::string outputFile, std::string imageFormat)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;
        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::img::RSGISCalcImageValue *findBoundaries = new rsgis::rastergis::RSGISIdentifyBoundaryPixels();
            rsgis::img::RSGISCalcImage imgCalc = rsgis::img::RSGISCalcImage(findBoundaries);

            imgCalc.calcImageWindowData(&inputDataset, 1, outputFile, 3, imageFormat, GDT_Byte);

            GDALClose(inputDataset);
            delete findBoundaries;
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeCalcBorderLength(std::string inputImage, bool ignoreZeroEdges, std::string outColsName)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;
        try
        {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISClumpBorders clumpBorders;
            clumpBorders.calcClumpBorderLength(inputDataset, !ignoreZeroEdges, outColsName);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }

    }

    void executeCalcRelBorder(std::string inputImage, std::string outColsName, std::string classNameField, std::string className, bool ignoreZeroEdges)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;
        try {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inputDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISClumpBorders clumpBorders;
            clumpBorders.calcClumpRelBorderLen2Class(inputDataset, !ignoreZeroEdges, outColsName, classNameField, className);

            GDALClose(inputDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeCalcShapeIndices(std::string inputImage, std::vector<RSGISShapeParamCmds> shapeIndexes)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *inputDataset;
        try
        {
            inputDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inputDataset == NULL) {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            std::vector<rastergis::RSGISShapeParam*> shapes;
            shapes.reserve(shapeIndexes.size());
            std::vector<RSGISShapeParamCmds>::iterator shapeIter;
            rastergis::RSGISShapeParam *shapeStore = new rastergis::RSGISShapeParam[shapeIndexes.size()];
            int i;

            for(shapeIter = shapeIndexes.begin(), i = 0; shapeIter != shapeIndexes.end(); ++shapeIter, ++i) {
                rastergis::rsgisshapeindex idx = (rastergis::rsgisshapeindex) (*shapeIter).idx;
                std::string colName = (*shapeIter).colName;
                unsigned int colIdx = (*shapeIter).colIdx;

                rastergis::RSGISShapeParam p;
                p.idx = idx;
                p.colName = colName;
                p.colIdx = colIdx;
                shapeStore[i] = p;
                shapes.push_back(&shapeStore[i]);
            }

            // now call function
            rsgis::rastergis::RSGISCalcClumpShapeParameters calcShapeParams;
            calcShapeParams.calcClumpShapeParams(inputDataset, &shapes);

            GDALClose(inputDataset);
            delete[] shapeStore;
        }
        catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeDefineClumpTilePositions(std::string clumpsImage, std::string tileImage, std::string outColsName, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *clumpsDataset, *tileDataset;
        try {
            clumpsDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_Update);
            if(clumpsDataset == NULL) {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            tileDataset = (GDALDataset *) GDALOpen(tileImage.c_str(), GA_ReadOnly);
            if(tileDataset == NULL) {
                std::string message = std::string("Could not open image ") + tileImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISDefineSegmentsWithinTiles defineSegsInTile;
            defineSegsInTile.defineSegmentTilePos(clumpsDataset, tileDataset, outColsName, tileOverlap, tileBoundary, tileBody);

            GDALClose(clumpsDataset);
            GDALClose(tileDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeDefineBorderClumps(std::string clumpsImage, std::string tileImage, std::string outColsName, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody)throw(RSGISCmdException) {
        GDALAllRegister();
        GDALDataset *clumpsDataset, *maskDataset;

        try {
            clumpsDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_Update);
            if(clumpsDataset == NULL) {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            maskDataset = (GDALDataset *) GDALOpen(tileImage.c_str(), GA_ReadOnly);
            if(maskDataset == NULL) {
                std::string message = std::string("Could not open image ") + tileImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISDefineSegmentsWithinTiles defineSegsInTile;
            defineSegsInTile.defineBorderSegmentsUsingMask(clumpsDataset, maskDataset, outColsName, tileOverlap, tileBoundary, tileBody);

            GDALClose(clumpsDataset);
            GDALClose(maskDataset);
        } catch(rsgis::RSGISException &e) {
            throw RSGISCmdException(e.what());
        }
    }

    void executeFindChangeClumpsFromStdDev(std::string clumpsImage, std::string classField, std::string changeField, std::vector<std::string> attFields, std::vector<cmds::RSGISClassChangeFieldsCmds> classChangeFields)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *clumpsDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_Update);
            if(clumpsDataset == NULL) {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            std::vector<rastergis::RSGISClassChangeFields *> *classFields = new std::vector<rastergis::RSGISClassChangeFields *>();
            classFields->reserve(classChangeFields.size());

            for(std::vector<cmds::RSGISClassChangeFieldsCmds>::iterator classIter = classChangeFields.begin(); classIter != classChangeFields.end(); ++classIter)
            {
                rastergis::RSGISClassChangeFields *c = new rastergis::RSGISClassChangeFields();
                c->name = (*classIter).name;
                c->outName = (*classIter).outName;
                c->threshold = (*classIter).threshold;
                c->means = NULL;
                c->stddev = NULL;
                c->count = 0;
                classFields->push_back(c);
            }

            rsgis::rastergis::RSGISFindChangeClumps changeClumps;
            changeClumps.findChangeStdDevThreshold(clumpsDataset, classField, changeField, &attFields, classFields);

            for(std::vector<rastergis::RSGISClassChangeFields*>::iterator classIter = classFields->begin(); classIter != classFields->end(); ++classIter)
            {
                delete *classIter;
            }
            delete classFields;
            GDALClose(clumpsDataset);
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeIdentifyClumpExtremesOnGrid(std::string clumpsImage, std::string inSelectField, std::string outSelectField, std::string eastingsCol, std::string northingsCol, std::string methodStr, unsigned int rows, unsigned int cols, std::string metricField)throw(RSGISCmdException)
    {
        GDALAllRegister();
        GDALDataset *clumpsDataset;

        try
        {
            rsgis::rastergis::RSGISSelectMethods method = rsgis::rastergis::noMethod;
            if(methodStr == "min")
            {
                method = rsgis::rastergis::minMethod;
            }
            else if(methodStr == "max")
            {
                method = rsgis::rastergis::maxMethod;
            }
            else if(methodStr == "mean")
            {
                method = rsgis::rastergis::meanMethod;
            }
            else
            {
                throw rsgis::RSGISAttributeTableException("Method was not recognised. Must be \'min\', \'max\' or \'mean\'.");
            }

            clumpsDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_Update);
            if(clumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISSelectClumpsOnGrid selectClumps;
            selectClumps.selectClumpsOnGrid(clumpsDataset, inSelectField, outSelectField, eastingsCol, northingsCol, metricField, rows, cols, method);


            GDALClose(clumpsDataset);
        }
        catch(rsgis::RSGISAttributeTableException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeInterpolateClumpValuesToImage(std::string clumpsImage, std::string selectField, std::string eastingsField, std::string northingsField, std::string methodStr, std::string valueField, std::string outputFile, std::string imageFormat, RSGISLibDataType dataType)throw(RSGISCmdException)
    {
        GDALAllRegister();
        GDALDataset *clumpsDataset;

        try
        {
            std::cout.precision(12);

            rsgis::math::RSGIS2DInterpolator *interpolator = NULL;
            if(methodStr == "nearestneighbour")
            {
                interpolator = new rsgis::math::RSGISNearestNeighbour2DInterpolator();
            }
            else if(methodStr == "naturalneighbour")
            {
                interpolator = new rsgis::math::RSGISNaturalNeighbor2DInterpolator();
            }
            else if(methodStr == "naturalnearestneighbour")
            {
                interpolator = new rsgis::math::RSGISNaturalNeighbor2DInterpolator();
            }
            else if(methodStr == "knearestneighbour")
            {
                interpolator = new rsgis::math::RSGISKNearestNeighbour2DInterpolator(3);
            }
            else if(methodStr == "idwall")
            {
                interpolator = new rsgis::math::RSGISAllPointsIDWInterpolator(8);
            }
            else
            {
                throw rsgis::RSGISAttributeTableException("The interpolated specified was not recognised.");
            }

            clumpsDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_ReadOnly);
            if(clumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::rastergis::RSGISInterpolateClumpValues2Image interpClumpVals;
            interpClumpVals.interpolateImageFromClumps(clumpsDataset, selectField, eastingsField, northingsField, valueField, outputFile, imageFormat, rsgis::cmds::RSGIS_to_GDAL_Type(dataType), interpolator);

            delete interpolator;

            GDALClose(clumpsDataset);
        }
        catch(rsgis::RSGISAttributeTableException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
            
            
            
    float executeFindGlobalSegmentationScore4Clumps(std::string clumpsImage, std::string inputImage, std::string colPrefix, bool calcNeighbours, float minNormV, float maxNormV, float minNormMI, float maxNormMI, std::vector<cmds::RSGISJXSegQualityScoreBandCmds> *scoreBandComps)throw(RSGISCmdException)
    {
        double returnGSSVal = 0.0;
        GDALAllRegister();
        GDALDataset *clumpsDataset;
        GDALDataset *inputImageDataset;
        
        try
        {
            std::cout.precision(12);
            rsgis::utils::RSGISTextUtils txtUtils;
            
            clumpsDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_Update);
            if(clumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            inputImageDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inputImageDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            if(calcNeighbours)
            {
                std::cout << "Populating the clumps with their neighbours\n";
                rsgis::rastergis::RSGISFindClumpNeighbours findNeighboursObj;
                findNeighboursObj.findNeighboursKEAImageCalc(clumpsDataset);
            }
  
            unsigned int numImgBands = inputImageDataset->GetRasterCount();
            
            std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats = new std::vector<rsgis::rastergis::RSGISBandAttStats*>();
            bandStats->reserve(numImgBands);
            
            rsgis::rastergis::RSGISBandAttStats *bandStat = NULL;
            for(unsigned int i = 0; i < numImgBands; ++i)
            {
                bandStat = new rsgis::rastergis::RSGISBandAttStats();
                bandStat->band = i+1;
                bandStat->threshold = 0;
                bandStat->calcCount = false;
                bandStat->countField = "";
                bandStat->calcMin = false;
                bandStat->minField = "";
                bandStat->calcMax = false;
                bandStat->maxField = "";
                bandStat->calcMean = true;
                bandStat->meanField = colPrefix + "_b" + txtUtils.uInt16bittostring(i+1) + "_Mean";
                bandStat->calcStdDev = true;
                bandStat->stdDevField = colPrefix + "_b" + txtUtils.uInt16bittostring(i+1) + "_StdDev";
                bandStat->calcMedian = false;
                bandStat->medianField = "";
                bandStat->calcSum = false;
                bandStat->sumField = "";
                
                bandStat->countIdxDef = false;
                bandStat->minIdxDef = false;
                bandStat->maxIdxDef = false;
                bandStat->meanIdxDef = false;
                bandStat->sumIdxDef = false;
                bandStat->stdDevIdxDef = false;
                bandStat->medianIdxDef = false;
                
                bandStats->push_back(bandStat);
            }
            
            std::cout << "Calculating the clump statistics (Mean and Standard Deviation).\n";
            rsgis::rastergis::RSGISCalcClumpStats clumpStats;
            clumpStats.calcImageClumpStatistic(clumpsDataset, inputImageDataset, bandStats);
            
            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBand = bandStats->begin(); iterBand != bandStats->end(); ++iterBand)
            {
                delete *iterBand;
            }
            delete bandStats;
            
            std::vector<rsgis::rastergis::JXSegQualityScoreBand*> *scoreComponents = new std::vector<rsgis::rastergis::JXSegQualityScoreBand*>();
            rsgis::rastergis::RSGISCalcSegmentQualityStatistics  calcSegsQuality;
            returnGSSVal = calcSegsQuality.calcJohnsonXie2011Metric(clumpsDataset, numImgBands, colPrefix, minNormV, maxNormV, minNormMI, maxNormMI, scoreComponents);
            
            for(std::vector<rsgis::rastergis::JXSegQualityScoreBand*>::iterator iterScores = scoreComponents->begin(); iterScores != scoreComponents->end(); ++iterScores)
            {
                scoreBandComps->push_back(cmds::RSGISJXSegQualityScoreBandCmds((*iterScores)->bandVar, (*iterScores)->bandMI, (*iterScores)->bandVarNorm, (*iterScores)->bandMINorm));
                
                delete *iterScores;
            }
            delete scoreComponents;
            
            GDALClose(clumpsDataset);
            GDALClose(inputImageDataset);
        }
        catch(rsgis::RSGISAttributeTableException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        
        return returnGSSVal;
    }
*/
}}

