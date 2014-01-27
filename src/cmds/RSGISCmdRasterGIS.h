/*
 *  RSGISCmdRasterGIS.h
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

#ifndef RSGISCmdRasterGIS_H
#define RSGISCmdRasterGIS_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

namespace rsgis{ namespace cmds {

    enum SpectralDistanceMethodCmds
    {
        stdEucSpecDist,
        originWeightedEuc
    };

    enum rsgismlpriorscmds
    {
        rsgis_samples = 0,
        rsgis_area = 1,
        rsgis_equal = 2,
        rsgis_userdefined = 3,
        rsgis_weighted = 4
    };

    enum rsgisshapeindexcmds
    {
        rsgis_shapena = 0,
        rsgis_shapearea = 1,
        rsgis_asymmetry = 2,
        rsgis_borderindex = 3,
        rsgis_borderlength = 4,
        rsgis_compactness = 5,
        rsgis_density = 6,
        rsgis_ellipticfit = 7,
        rsgis_length = 8,
        rsgis_lengthwidth = 9,
        rsgis_width = 10,
        rsgis_maindirection = 11,
        rsgis_radiuslargestenclosedellipse = 12,
        rsgis_radiussmallestenclosedellipse = 13,
        rsgis_rectangularfit = 14,
        rsgis_roundness = 15,
        rsgis_shapeindex = 16
    };

    struct RSGISBandAttStatsCmds
    {
        unsigned int band;
        float threshold;
        bool calcCount;
        std::string countField;
        bool calcMin;
        std::string minField;
        bool calcMax;
        std::string maxField;
        bool calcMean;
        std::string meanField;
        bool calcStdDev;
        std::string stdDevField;
        bool calcMedian;
        std::string medianField;
        bool calcSum;
        std::string sumField;
    };

    struct RSGISBandAttPercentilesCmds
    {
        unsigned int band;
        unsigned int percentile;
        std::string fieldName;
    };
    
    struct RSGISShapeParamCmds
    {
        rsgisshapeindexcmds idx;
        std::string colName;
        unsigned int colIdx;
    };
    
    struct RSGISClassChangeFieldsCmds
    {
        std::string name;
        int outName;
        float threshold;
    };


    class RSGISColourIntCmds {
    public:
        RSGISColourIntCmds() { this->red = 0; this->green = 0; this->blue = 0; this->alpha = 0; };
        RSGISColourIntCmds(int r, int g, int b, int a) { this->red = r; this->green = g; this->blue = b; this->alpha = a;};
        int getRed() { return this->red; };
        int getGreen() { return this->green; };        int getBlue() { return this->blue; };
        int getAlpha() { return this->alpha; };
        ~RSGISColourIntCmds(){};
    protected:
        int red;
        int green;
        int blue;
        int alpha;
    };

    /** Function to populate statics for thermatic images */
    void executePopulateStats(std::string clumpsImage, bool addColourTable2Img, bool calcImgPyramids)throw(RSGISCmdException);

    /** Function for copying a GDAL RAT from one image to anoother */
    void executeCopyRAT(std::string inputImage, std::string clumpsImage)throw(RSGISCmdException);

    /** Function for copying GDAL RAT columns from one image to another */
    void executeCopyGDALATTColumns(std::string inputImage, std::string clumpsImage, std::vector<std::string> fields)throw(RSGISCmdException);

    /** Function for adding spatial location columns to the attribute table */
    void executeSpatialLocation(std::string inputImage, std::string eastingsField, std::string northingsField)throw(RSGISCmdException);

    /** Function for populating an attribute table from an image */
    void executePopulateRATWithStats(std::string inputImage, std::string clumpsImage, std::vector<rsgis::cmds::RSGISBandAttStatsCmds*> *bandStatsCmds)throw(RSGISCmdException);

    /** Function for populating an attribute table with a percentile of the pixel values */
    void executePopulateRATWithPercentiles(std::string inputImage, std::string clumpsImage, std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*> *bandPercentilesCmds)throw(RSGISCmdException);

    /** Function for populating the attribute table with the proporations of intersecting catagories */
    void executePopulateCategoryProportions(std::string categoriesImage, std::string clumpsImage, std::string outColsName, std::string majorityColName, bool copyClassNames, std::string majClassNameField, std::string classNameField)throw(RSGISCmdException);

    /** Function for copying an attribute tables colour table to another table based on class column */
    void executeCopyCategoriesColours(std::string categoriesImage, std::string clumpsImage, std::string classField)throw(RSGISCmdException);

    /** Function for exporting columns of the attribute table as GDAL images */
    void executeExportCols2GDALImage(std::string inputImage, std::string outputFile, std::string imageFormat, RSGISLibDataType outDataType, std::vector<std::string> fields)throw(RSGISCmdException);

    /** Function for calculating the euclidean distance from a feature to all other features */
    void executeEucDistFromFeature(std::string inputImage, size_t fid, std::string outputField, std::vector<std::string> fields)throw(RSGISCmdException);

    /** Function to calculate the top N features within a given spatial distance */
    void executeFindTopN(std::string inputImage, std::string spatialDistField, std::string distanceField, std::string outputField, unsigned int nFeatures, float distThreshold)throw(RSGISCmdException);

    /** Function to calculate the features within a given spatial and spectral distance */
    void executeFindSpecClose(std::string inputImage, std::string distanceField, std::string spatialDistField, std::string outputField, float specDistThreshold, float distThreshold)throw(RSGISCmdException);

    /** Function to classify segments using KNN majority classification */
    void executeKnnMajorityClassifier(std::string inputImage, std::string inClassNameField, std::string outClassNameField, std::string trainingSelectCol, std::string eastingsField, std::string northingsField, std::string areaField, std::string majWeightField, std::vector<std::string> fields, unsigned int nFeatures, float distThreshold, float weightA, void *majorMethod)throw(RSGISCmdException);

    /** Function to export columns from a GDAL RAT to ascii */
    void executeExport2Ascii(std::string inputImage, std::string outputFile, std::vector<std::string> fields)throw(RSGISCmdException);

    /** Function to translate a set of classes to another */
    void executeClassTranslate(std::string inputImage, std::string classInField, std::string classOutField, std::map<size_t, size_t> classPairs)throw(RSGISCmdException);

    /** Function to set a colour table for a set of classes within the attribute table */
    void executeColourClasses(std::string inputImage, std::string classInField, std::map<size_t, RSGISColourIntCmds> classColourPairs)throw(RSGISCmdException);

    /** Function to set a colour table for a set of classes (string column) within the attribute table */
    void executeColourStrClasses(std::string inputImage, std::string classInField, std::map<std::string, RSGISColourIntCmds> classStrColourPairs)throw(RSGISCmdException);

    /** Function to generate a colour table using an input image */
    void executeGenerateColourTable(std::string inputImage, std::string clumpsImage, unsigned int redBand, unsigned int greenBand, unsigned int blueBand)throw(RSGISCmdException);

    /** Function to find the majority for class (string - field) from a set of small objects to large objects */
    void executeStrClassMajority(std::string baseSegment, std::string infoSegment, std::string baseClassCol, std::string infoClassCol)throw(RSGISCmdException);

    /** Function to classify segments using a spectral distance majority classification */
    void executeSpecDistMajorityClassifier(std::string inputImage, std::string inClassNameField, std::string outClassNameField, std::string trainingSelectCol, std::string eastingsField, std::string northingsField, std::string areaField, std::string majWeightField, std::vector<std::string> fields, float distThreshold, float specDistThreshold, SpectralDistanceMethodCmds distMethod, float specThresOriginDist)throw(RSGISCmdException);

    /** Function to classify segments using a maximum likelihood classification */
    void executeMaxLikelihoodClassifier(std::string inputImage, std::string inClassNameField, std::string outClassNameField, std::string trainingSelectCol, std::string classifySelectCol, std::string areaField, std::vector<std::string> fields, rsgismlpriorscmds priorsMethod, std::vector<std::string> priorStrs)throw(RSGISCmdException);

    /** Function to classify segments using a spectral distance majority classification */
    void executeMaxLikelihoodClassifierLocalPriors(std::string inputImage, std::string inClassNameField, std::string outClassNameField, std::string trainingSelectCol, std::string classifySelectCol, std::string areaField, std::vector<std::string> fields, std::string eastingsField, std::string northingsField, float distThreshold, rsgismlpriorscmds priorsMethod, float weightA, bool allowZeroPriors, bool forceChangeInClassification)throw(RSGISCmdException);

    /** Function to generate a mask for paraticular class */
    void executeClassMask(std::string inputImage, std::string classField, std::string className, std::string outputFile, std::string imageFormat, RSGISLibDataType dataType)throw(RSGISCmdException);
    
    /** Function to find the clump neighbours */
    void executeFindNeighbours(std::string inputImage)throw(RSGISCmdException);

    /** Function to identify the pixels on the boundary of the clumps */
    void executeFindBoundaryPixels(std::string inputImage, std::string outputFile, std::string imageFormat)throw(RSGISCmdException);

    /** Function to calculate the border length of the clumps */
    void executeCalcBorderLength(std::string inputImage, bool ignoreZeroEdges, std::string outColsName)throw(RSGISCmdException);

    /** Function to calculate the relative border length of the clumps to a class */
    void executeCalcRelBorder(std::string inputImage, std::string outColsName, std::string classNameField, std::string className, bool ignoreZeroEdges)throw(RSGISCmdException);

    /** Function to command to calculate shape indices for clumps */
    void executeCalcShapeIndices(std::string inputImage, std::vector<cmds::RSGISShapeParamCmds> shapeIndexes)throw(RSGISCmdException);

    /** Function to define the position within the file of the clumps */
    void executeDefineClumpTilePositions(std::string clumpsImage, std::string tileImage, std::string outColsName, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody)throw(RSGISCmdException);

    /** Function to define the clumps which are on the border within the file of the clumps using a mask */
    void executeDefineBorderClumps(std::string clumpsImage, std::string tileImage, std::string outColsName, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody)throw(RSGISCmdException);

    /** Function to identify segments which have changed by looking for statistical outliers (std dev) from class population */
    void executeFindChangeClumpsFromStdDev(std::string clumpsImage, std::string classField, std::string changeField, std::vector<std::string> attFields, std::vector<cmds::RSGISClassChangeFieldsCmds> classChangeFields)throw(RSGISCmdException);
    
    /** Function to identify an extreme clump/segment with regions of the image, regions defined on a grid */
    void executeIdentifyClumpExtremesOnGrid(std::string clumpsImage, std::string inSelectField, std::string outSelectField, std::string eastingsCol, std::string northingsCol, std::string methodStr, unsigned int rows, unsigned int cols, std::string metricField)throw(RSGISCmdException);
    
    /** Function to interpolate values from clumps to the whole image of pixels */
    void executeInterpolateClumpValuesToImage(std::string clumpsImage, std::string selectField, std::string eastingsField, std::string northingsField, std::string methodStr, std::string valueField, std::string outputFile, std::string imageFormat, RSGISLibDataType dataType)throw(RSGISCmdException);
    
}}


#endif

