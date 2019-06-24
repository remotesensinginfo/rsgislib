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

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_cmds_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{ namespace cmds {

    enum SpectralDistanceMethodCmds
    {
        stdEucSpecDist,
        originWeightedEuc
    };
    
    enum rsgisKNNSummeriseCmd
    {
        rsgisKNNMode = 1,
        rsgisKNNMean = 2,
        rsgisKNNMedian = 3,
        rsgisKNNMin = 4,
        rsgisKNNMax = 5,
        rsgisKNNStdDev = 6
    };
    
    enum rsgisKNNDistCmd
    {
        rsgisKNNEuclidean = 1,
        rsgisKNNMahalanobis = 2,
        rsgisKNNManhattan = 3,
        rsgisKNNChebyshev = 4,
        rsgisKNNMinkowski = 5
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

    struct DllExport RSGISBandAttStatsCmds
    {
        unsigned int band;
        bool calcMin;
        std::string minField;
        bool calcMax;
        std::string maxField;
        bool calcMean;
        std::string meanField;
        bool calcStdDev;
        std::string stdDevField;
        bool calcSum;
        std::string sumField;
    };
    
    struct DllExport RSGISFieldAttStatsCmds
    {
        std::string field;
        bool calcMin;
        std::string minField;
        bool calcMax;
        std::string maxField;
        bool calcMean;
        std::string meanField;
        bool calcStdDev;
        std::string stdDevField;
        bool calcSum;
        std::string sumField;
    };

    struct DllExport RSGISBandAttPercentilesCmds
    {
        float percentile;
        std::string fieldName;
    };
/*
    struct DllExport RSGISShapeParamCmds
    {
        rsgisshapeindexcmds idx;
        std::string colName;
        unsigned int colIdx;
    };
*/
    struct DllExport RSGISClassChangeFieldsCmds
    {
        std::string name;
        int outName;
        float threshold;
    };
 /*
    struct DllExport RSGISJXSegQualityScoreBandCmds
    {
        RSGISJXSegQualityScoreBandCmds(float bandVar, float bandMI, float bandVarNorm, float bandMINorm)
        {
            this->bandVar = bandVar;
            this->bandMI = bandMI;
            this->bandVarNorm = bandVarNorm;
            this->bandMINorm = bandMINorm;
        };

        float bandVar;
        float bandMI;
        float bandVarNorm;
        float bandMINorm;
    };
 */

    class DllExport RSGISColourIntCmds {
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
    DllExport void executePopulateStats(std::string clumpsImage, bool addColourTable2Img, bool calcImgPyramids, bool ignoreZero, unsigned int ratBand);

    /** Function for copying a GDAL RAT from one image to anoother */
    DllExport void executeCopyRAT(std::string inputImage, std::string clumpsImage, int ratBand=1);

    /** Function for copying GDAL RAT columns from one image to another */
    DllExport void executeCopyGDALATTColumns(std::string inputImage, std::string clumpsImage, std::vector<std::string> fields, bool copyColours=false, bool copyHist=false, int ratBand=1);

    /** Function for adding spatial location columns to the attribute table */
    DllExport void executeSpatialLocation(std::string inputImage, unsigned int ratBand, std::string eastingsField, std::string northingsField);
    
    /** Function for adding the spatial extent for each clump as columns to the attribute table */
    DllExport void executeSpatialLocationExtent(std::string inputImage, unsigned int ratBand, std::string minXColX, std::string minXColY, std::string maxXColX, std::string maxXColY, std::string minYColX, std::string minYColY, std::string maxYColX, std::string maxYColY);

    /** Function for populating an attribute table from an image */
    DllExport void executePopulateRATWithStats(std::string inputImage, std::string clumpsImage, std::vector<rsgis::cmds::RSGISBandAttStatsCmds*> *bandStatsCmds, unsigned int ratBand);

    /** Function for populating an attribute table with a percentile of the pixel values */
    DllExport void executePopulateRATWithPercentiles(std::string inputImage, std::string clumpsImage, unsigned int band, std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*> *bandPercentilesCmds, unsigned int ratBand, unsigned int numHistBins);

    /** Function for populating the attribute table with the proporations of intersecting catagories */
    DllExport void executePopulateCategoryProportions(std::string categoriesImage, std::string clumpsImage, std::string outColsName, std::string majorityColName, bool copyClassNames, std::string majClassNameField, std::string classNameField, unsigned int ratBandClumps, unsigned int ratBandCats);

    /** Function for populating the attribute table with the mode of intersecting catagories */
    DllExport void executePopulateRATWithMode(std::string inputImage, std::string clumpsImage, std::string outColsName, bool useNoDataVal, long noDataVal, bool outNoDataVal, unsigned int modeBand, unsigned int ratBand);
    
    /** Function for copying an attribute tables colour table to another table based on class column */
    //DllExport void executeCopyCategoriesColours(std::string categoriesImage, std::string clumpsImage, std::string classField);

    /** Function for exporting columns of the attribute table as GDAL images */
    DllExport void executeExportCols2GDALImage(std::string inputImage, std::string outputFile, std::string imageFormat, RSGISLibDataType outDataType, std::string field, int ratBand=1);

    /** Function for calculating the euclidean distance from a feature to all other features */
    //DllExport void executeEucDistFromFeature(std::string inputImage, size_t fid, std::string outputField, std::vector<std::string> fields);

    /** Function to calculate the top N features within a given spatial distance */
    //DllExport void executeFindTopN(std::string inputImage, std::string spatialDistField, std::string distanceField, std::string outputField, unsigned int nFeatures, float distThreshold);

    /** Function to calculate the features within a given spatial and spectral distance */
    //DllExport void executeFindSpecClose(std::string inputImage, std::string distanceField, std::string spatialDistField, std::string outputField, float specDistThreshold, float distThreshold);

    /** Function to extrapolate values on segments using KNN, use mode for classification */
    DllExport void executeApplyKNN(std::string inClumpsImage, unsigned int ratBand, std::string inExtrapField, std::string outExtrapField, std::string trainRegionsField, std::string applyRegionsField, bool useApplyField, std::vector<std::string> fields, unsigned int kFeatures, rsgisKNNDistCmd distKNNCmd, float distThreshold, rsgisKNNSummeriseCmd summeriseKNNCmd);

    /** Function to export columns from a GDAL RAT to ascii */
    DllExport void executeExport2Ascii(std::string inputImage, std::string outputFile, std::vector<std::string> fields, int ratBand=1);

    /** Function to translate a set of classes to another */
    //DllExport void executeClassTranslate(std::string inputImage, std::string classInField, std::string classOutField, std::map<size_t, size_t> classPairs);

    /** Function to set a colour table for a set of classes within the attribute table */
    DllExport void executeColourClasses(std::string inputImage, std::string classInField, std::map<size_t, RSGISColourIntCmds> classColourPairs, int ratBand=1);

    /** Function to set a colour table for a set of classes (string column) within the attribute table */
    DllExport void executeColourStrClasses(std::string inputImage, std::string classInField, std::map<std::string, RSGISColourIntCmds> classStrColourPairs, int ratBand=1);

    /** Function to generate a colour table using an input image */
    //DllExport void executeGenerateColourTable(std::string inputImage, std::string clumpsImage, unsigned int redBand, unsigned int greenBand, unsigned int blueBand);

    /** Function to find the majority for class (string - field) from a set of small objects to large objects */
    DllExport void executeStrClassMajority(std::string baseSegment, std::string infoSegment, std::string baseClassCol, std::string infoClassCol, bool ignoreZero = true, int baseRatBand=1, int infoRatBand=1);

    /** Function to classify segments using a spectral distance majority classification */
    //DllExport void executeSpecDistMajorityClassifier(std::string inputImage, std::string inClassNameField, std::string outClassNameField, std::string trainingSelectCol, std::string eastingsField, std::string northingsField, std::string areaField, std::string majWeightField, std::vector<std::string> fields, float distThreshold, float specDistThreshold, SpectralDistanceMethodCmds distMethod, float specThresOriginDist);

    /** Function to classify segments using a maximum likelihood classification */
    //DllExport void executeMaxLikelihoodClassifier(std::string inputImage, std::string inClassNameField, std::string outClassNameField, std::string trainingSelectCol, std::string classifySelectCol, std::string areaField, std::vector<std::string> fields, rsgismlpriorscmds priorsMethod, std::vector<std::string> priorStrs);

    /** Function to classify segments using a spectral distance majority classification */
    //DllExport void executeMaxLikelihoodClassifierLocalPriors(std::string inputImage, std::string inClassNameField, std::string outClassNameField, std::string trainingSelectCol, std::string classifySelectCol, std::string areaField, std::vector<std::string> fields, std::string eastingsField, std::string northingsField, float distThreshold, rsgismlpriorscmds priorsMethod, float weightA, bool allowZeroPriors, bool forceChangeInClassification);

    /** Function to generate a mask for paraticular class */
    //DllExport void executeClassMask(std::string inputImage, std::string classField, std::string className, std::string outputFile, std::string imageFormat, RSGISLibDataType dataType);

    /** Function to find the clump neighbours */
    DllExport void executeFindNeighbours(std::string inputImage, unsigned int ratBand);

    /** Function to identify the pixels on the boundary of the clumps */
    DllExport void executeFindBoundaryPixels(std::string inputImage, unsigned int ratBand, std::string outputFile, std::string imageFormat);

    /** Function to calculate the border length of the clumps */
    DllExport void executeCalcBorderLength(std::string inputImage, bool ignoreZeroEdges, std::string outColsName);

    /** Function to calculate the relative border length of the clumps to a class */
    DllExport void executeCalcRelBorder(std::string inputImage, std::string outColsName, std::string classNameField, std::string className, bool ignoreZeroEdges);

    /** Function to command to calculate shape indices for clumps */
    //DllExport void executeCalcShapeIndices(std::string inputImage, std::vector<cmds::RSGISShapeParamCmds> shapeIndexes);

    /** Function to define the position within the file of the clumps */
    DllExport void executeDefineClumpTilePositions(std::string clumpsImage, std::string tileImage, std::string outColsName, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody);

    /** Function to define the clumps which are on the border within the file of the clumps */
    DllExport void executeDefineBorderClumps(std::string clumpsImage, std::string outColsName);

    /** Function to identify segments which have changed by looking for statistical outliers (std dev) from class population */
    DllExport void executeFindChangeClumpsFromStdDev(std::string clumpsImage, std::string classField, std::string changeField, std::vector<std::string> attFields, std::vector<cmds::RSGISClassChangeFieldsCmds> classChangeFields, int ratBand=1);

    /** Function to attribute each row with mean and standard deviation for the class population, similar to executeFindChangeClumpsFromStdDev but requires change to be calculated externally */
    DllExport void executeGetGlobalClassStats(std::string clumpsImage, std::string classField, std::vector<std::string> attFields, std::vector<cmds::RSGISClassChangeFieldsCmds> classChangeFields, int ratBand=1);

    /** Function to identify an extreme clump/segment with regions of the image, regions defined on a grid */
    DllExport void executeIdentifyClumpExtremesOnGrid(std::string clumpsImage, std::string inSelectField, std::string outSelectField, std::string eastingsCol, std::string northingsCol, std::string methodStr, unsigned int rows, unsigned int cols, std::string metricField);

    /** Function to interpolate values from clumps to the whole image of pixels */
    DllExport void executeInterpolateClumpValuesToImage(std::string clumpsImage, std::string selectField, std::string eastingsField, std::string northingsField, std::string methodStr, std::string valueField, std::string outputFile, std::string imageFormat, RSGISLibDataType dataType, unsigned int ratband);

    /** Function to calculate the 'Global Segmentation Score' for the clumps using a given input image */
    //float executeFindGlobalSegmentationScore4Clumps(std::string clumpsImage, std::string inputImage, std::string colPrefix, bool calcNeighbours, float minNormV, float maxNormV, float minNormMI, float maxNormMI, std::vector<cmds::RSGISJXSegQualityScoreBandCmds> *scoreBandComps);

    /** Function to calculate relative difference statistic to neighbouring clumps. */
    DllExport void executeCalcRelDiffNeighbourStats(std::string clumpsImage, rsgis::cmds::RSGISFieldAttStatsCmds *fieldStatsCmds, bool useAbsDiff, unsigned int ratBand);
    
    /** Function to undertaken region growing of a class */
    DllExport void executeClassRegionGrowing(std::string clumpsImage, unsigned int ratBand, std::string classColumn, std::string classVal, int maxIter, std::string xmlBlock);
    
    /** Function to evaluate regions to produce a binary classification */
    DllExport void executeBinaryClassify(std::string clumpsImage, unsigned int ratBand, std::string xmlBlock, std::string outColumn);
    
    /** Function for populating an attribute table from an image with 'mean-lit' values. */
    DllExport void executePopulateRATWithMeanLitStats(std::string inputImage, std::string clumpsImage, std::string inputMeanLitImage, unsigned int meanlitBand, std::string meanLitColumn, std::string pxlCountCol, std::vector<rsgis::cmds::RSGISBandAttStatsCmds*> *bandStatsCmds, unsigned int ratBand);
    
    /** Function for collapsing a RAT and assocated image clumps based on a binary column 'selected' column in the RAT */
    DllExport void executeCollapseRAT(std::string clumpsImage, unsigned int ratBand, std::string selectColumn, std::string outImage, std::string gdalFormat);
    
    /** Function for importing attribute table from a shapefile into a RAT */
    DllExport void executeImportShpAtts(std::string clumpsImage, unsigned int ratBand, std::string inputVector, std::string inputVectorLyr, std::string fidColStr, std::vector<std::string> *colNames=NULL);
    
    /** Function to undertaken region growing of a class with a neighbour / object criteria */
    DllExport void executeClassRegionGrowingNeighCritera(std::string clumpsImage, unsigned int ratBand, std::string classColumn, std::string classVal, int maxIter, std::string xmlBlockGrowCriteria, std::string xmlBlockNeighCriteria);
    
    /** Function to statistically sample the RAT using a histogram method for a single variable. */
    DllExport void executeHistSampling(std::string clumpsImage, unsigned int ratBand, std::string varCol, std::string outSelectCol, float propOfSample, float binWidth, bool classRestrict=false, std::string classColumn="", std::string classVal="");
    
    /** Function to fit a Gaussian mixture model to histogram */
    DllExport void executeFitHistGausianMixtureModel(std::string clumpsImage, unsigned int ratBand, std::string outH5File, std::string varCol, float binWidth, std::string classColumn, std::string classVal, bool outputHist, std::string outHistFile);

    /** Function to fit a Gaussian mixture model to histogram and to split the class accordingly */
    DllExport void executeClassSplitFitHistGausianMixtureModel(std::string clumpsImage, unsigned int ratBand, std::string outColumn, std::string varCol, float binWidth, std::string classColumn, std::string classVal);
    
    /** Function which populates the RAT with proportion of valid pixels within a clump */
    DllExport void executeCalcPropOfValidPixelsInClump(std::string inputImage, std::string clumpsImage, unsigned int ratBand, std::string outColumn, double noDataVal=0);
    
    /** Function to calculate the 2D Jeffries Matusita distance between two classes. */
    DllExport float executeCalc1DJMDistance(std::string clumpsImage, std::string varCol, float binWidth, std::string classColumn, std::string class1Val, std::string class2Val, unsigned int ratBand=1);
    
    /** Function to calculate the 3D Jeffries Matusita distance between two classes. */
    DllExport float executeCalc2DJMDistance(std::string clumpsImage, std::string var1Col, std::string var2Col, float var1binWidth, float var2binWidth, std::string classColumn, std::string class1Val, std::string class2Val, unsigned int ratBand=1);
    
    /** Function to calculate the Bhattacharyya distance between two classes. */
    DllExport float executeCalcBhattacharyyaDistance(std::string clumpsImage, std::string varCol, std::string classColumn, std::string class1Val, std::string class2Val, unsigned int ratBand=1);
    
    /** Function to export each clump to an individual image file */
    DllExport void executeExportClumps2Images(std::string clumpsImage, std::string outImgBase, std::string imgFileExt, std::string imageFormat, bool binaryOut, unsigned int ratBand=1);
    
    
}}


#endif

