/*
 *  RSGISCmdImageCalc.h
 *
 *
 *  Created by Pete Bunting on 29/04/2013.
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

#ifndef RSGISCmdImageCalc_H
#define RSGISCmdImageCalc_H

#include <iostream>
#include <string>
#include <vector>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"
#include "RSGISCmdCommon.h"

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

    struct DllExport VariableStruct
    {
        std::string image;
        std::string name;
        int bandNum;
    };
    
    struct DllExport ImageStatsCmds
	{
		double mean;
		double max;
		double min;
		double stddev;
        double sum;
        double median;
        double mode;
	};

    enum RSGISInitClustererMethods
    {
        rsgis_init_random,
        rsgis_init_diagonal_full,
        rsgis_init_diagonal_stddev,
        rsgis_init_diagonal_full_attach,
        rsgis_init_diagonal_stddev_attach,
        rsgis_init_kpp
    };
    
    enum RSGISCmdsSummariseStats
    {
        rsgiscmds_stat_none,
        rsgiscmds_stat_min,
        rsgiscmds_stat_max,
        rsgiscmds_stat_mean,
        rsgiscmds_stat_median,
        rsgiscmds_stat_range,
        rsgiscmds_stat_stddev,
        rsgiscmds_stat_sum,
        rsgiscmds_stat_mode,
        rsgiscmds_stat_count
    };

    /** Function to run the band maths tools */
    DllExport void executeBandMaths(VariableStruct *variables, unsigned int numVars, std::string outputImage, std::string mathsExpression, std::string gdalFormat, RSGISLibDataType outDataType, bool useExpAsbandName)throw(RSGISCmdException);
    /** Function to run the image maths tools */
    DllExport void executeImageMaths(std::string inputImage, std::string outputImage, std::string mathsExpression, std::string imageFormat, RSGISLibDataType outDataType, bool useExpAsbandName)throw(RSGISCmdException);
    /** Function to run the image band maths tools */
    DllExport void executeImageBandMaths(std::string inputImage, std::string outputImage, std::string mathsExpression, std::string imageFormat, RSGISLibDataType outDataType, bool useExpAsbandName)throw(RSGISCmdException);
    /** Function to run the KMeans tool */
    DllExport void executeKMeansClustering(std::string inputImage, std::string outputMatrixFile, unsigned int numClusters, unsigned int maxNumIterations, unsigned int subSample, bool ignoreZeros, float degreeOfChange, RSGISInitClustererMethods initClusterMethod)throw(RSGISCmdException);
    /** Function to run the KMeans tool */
    DllExport void executeISODataClustering(std::string inputImage, std::string outputMatrixFile, unsigned int numClusters, unsigned int maxNumIterations, unsigned int subSample, bool ignoreZeros, float degreeOfChange, RSGISInitClustererMethods initClusterMethod, float minDistBetweenClusters, unsigned int minNumFeatures, float maxStdDev, unsigned int minNumClusters, unsigned int startIteration, unsigned int endIteration)throw(RSGISCmdException);
    /** Function to run mahalanobis distance Window Filter */
    DllExport void executeMahalanobisDistFilter(std::string inputImage, std::string outputImage, unsigned int winSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** Function to run mahalanobis distance Image to Window Filter */
    DllExport void executeMahalanobisDist2ImgFilter(std::string inputImage, std::string outputImage, unsigned int winSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** Function to run image calculate distance command */
    DllExport void executeImageCalcDistance(std::string inputImage, std::string outputImage, std::string gdalFormat)throw(RSGISCmdException);
    /** Function to calculate summary statistics for a column of pixels */
    DllExport void executeImagePixelColumnSummary(std::string inputImage, std::string outputImage, rsgis::cmds::RSGISCmdStatsSummary summaryStats, std::string gdalFormat, RSGISLibDataType outDataType, float noDataValue, bool useNoDataValue)throw(RSGISCmdException);
    /** Function to perform a linear regression on each column of pixels */
    DllExport void executeImagePixelLinearFit(std::string inputImage, std::string outputImage, std::string gdalFormat, std::string bandValues, float noDataValue, bool useNoDataValue)throw(RSGISCmdException);
    /** Function to perform image normalisation */
    DllExport void executeNormalisation(std::vector<std::string> inputImages, std::vector<std::string> outputImages, bool calcInMinMax, double inMin, double inMax, double outMin, double outMax)throw(RSGISCmdException);
    /** Function to calculate the correlation between 2 images */
    DllExport double** executeCorrelation(std::string inputImageA, std::string inputImageB, std::string outputMatrixFile = "", unsigned int *nrows = 0, unsigned int *ncols = 0) throw(RSGISCmdException);
    /** Function to calculate the covariance between 2 images */
    DllExport void executeCovariance(std::string inputImageA, std::string inputImageB, std::string inputMatrixA, std::string inputMatrixB, bool shouldCalcMean, std::string outputMatrix)throw(RSGISCmdException);
    /** Function to calculate the mean vector of an image */
    DllExport void executeMeanVector(std::string inputImage, std::string outputMatrix)throw(RSGISCmdException);
    /** Function to perform principal components analysis of an image */
    DllExport void executePCA(std::string inputImage, std::string eigenvectors, std::string outputImage, int numComponents, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** Function to generate a standardised image using the mean vector provided */
    DllExport void executeStandardise(std::string meanvectorStr, std::string inputImage, std::string outputImage)throw(RSGISCmdException);
    /** Function to replace values less then given, using a threshold */
    DllExport void executeReplaceValuesLessThan(std::string inputImage, std::string outputImage, double threshold, double value)throw(RSGISCmdException);
    /** Function to convert the image spectra to unit area */
    DllExport void executeUnitArea(std::string inputImage, std::string outputImage, std::string inMatrixfile)throw(RSGISCmdException);
    /** Function to calculate the speed of movement (mean, min and max) */
    DllExport void executeMovementSpeed(std::vector<std::string> inputImages, std::vector<unsigned int> imageBands, std::vector<float> imageTimes, float upper, float lower, std::string outputImage)throw(RSGISCmdException);
    /** Function that counts the number of values with a given range for each column*/
    DllExport void executeCountValsInCols(std::string inputImage, float upper, float lower, std::string outputImage)throw(RSGISCmdException);
    /** Function to calculate the root mean squared error between 2 images */
    DllExport double executeCalculateRMSE(std::string inputImageA, int inputBandA, std::string inputImageB, int inputBandB)throw(RSGISCmdException);
    /** Function to apply 2 var function to image */
    DllExport void executeApply2VarFunction(std::string inputImage, void *twoVarFunction, std::string outputImage)throw(RSGISCmdException);
    /** Function to apply 3 var function to image */
    DllExport void executeApply3VarFunction(std::string inputImage, void *threeVarFunction, std::string outputImage)throw(RSGISCmdException);
    /** Function to calculate the distance to the nearest geometry for each pixel in an image */
    DllExport void executeDist2Geoms(std::string inputVector, float imgResolution, std::string outputImage)throw(RSGISCmdException);
    /** Function to calculate statistics for individual image bands */
    DllExport void executeImageBandStats(std::string inputImage, std::string outputFile, bool ignoreZeros)throw(RSGISCmdException);
    /** Function to calculate the statistics for the whole image across all bands */
    DllExport void executeImageStats(std::string inputImage, std::string outputFile, bool ignoreZeros)throw(RSGISCmdException);
    
    /** Function to undertake an unconstrained linear spectral unmixing of the input image for a set of endmembers */
    DllExport void executeUnconLinearSpecUnmix(std::string inputImage, std::string imageFormat, RSGISLibDataType outDataType, float lsumGain, float lsumOffset, std::string outputFile, std::string endmembersFile)throw(RSGISCmdException);
    /** Function to undertake an exhaustive constrained linear spectral unmixing of the input image for a set of endmembers */
    DllExport void executeExhconLinearSpecUnmix(std::string inputImage, std::string imageFormat, RSGISLibDataType outDataType, float lsumGain, float lsumOffset, std::string outputFile, std::string endmembersFile, float stepResolution)throw(RSGISCmdException);
    /** Function to undertake a partially constrained linear spectral unmixing of the input image for a set of endmembers where the sum of the unmixing will be approximately 1*/
    DllExport void executeConSum1LinearSpecUnmix(std::string inputImage, std::string imageFormat, RSGISLibDataType outDataType, float lsumGain, float lsumOffset, float lsumWeight, std::string outputFile, std::string endmembersFile)throw(RSGISCmdException);
    /** Function to undertake a constrained linear spectral unmixing of the input image for a set of endmembers where the sum of the unmixing will be approximately 1 and non-negative */
    DllExport void executeNnConSum1LinearSpecUnmix(std::string inputImage, std::string imageFormat, RSGISLibDataType outDataType, float lsumGain, float lsumOffset, float lsumWeight, std::string outputFile, std::string endmembersFile)throw(RSGISCmdException);
    
    /** Funtion to undertake multiple endmember spectral mixture analysis using non-negative constrained linear spectral unmixing */
    
    
    /** Function to test whether all bands are equal to the same value */
    DllExport void executeAllBandsEqualTo(std::string inputImage, float imgValue, float outputTrueVal, float outputFalseVal, std::string outputImage, std::string imageFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** Function to generate a histogram for the region of the mask selected */
    DllExport void executeHistogram(std::string inputImage, std::string imageMask, std::string outputFile, unsigned int imgBand, float imgValue, double binWidth, bool calcInMinMax, double inMin, double inMax)throw(RSGISCmdException);
    /** Function to generate a histogram and return it */
    DllExport unsigned int* executeGetHistogram(std::string inputImage, unsigned int imgBand, double binWidth, unsigned int *nBins, bool calcInMinMax, double *inMin, double *inMax)throw(RSGISCmdException);
    /** Function to calculate image band percentiles */
    DllExport std::vector<double> executeBandPercentile(std::string inputImage, float percentile, float noDataValue, bool noDataValueSpecified)throw(RSGISCmdException);
    /** Function to calculate the distance to the nearest geometry for every pixel in an image */
    DllExport void executeImageDist2Geoms(std::string inputImage, std::string inputVector, std::string imageFormat, std::string outputImage)throw(RSGISCmdException);
    /** Function to calculate correlation for windows */
    DllExport void executeCorrelationWindow(std::string inputImage, std::string outputImage, unsigned int winSize, unsigned int corrBandA, unsigned int corrBandB, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** Function to calculate the statistics for an individual image band within an envelope defined in Lat / Long */
    DllExport void executeImageBandStatsEnv(std::string inputImage, rsgis::cmds::ImageStatsCmds *stats, unsigned int imgBand, bool noDataValueSpecified, float noDataVal, double latMin, double latMax, double longMin, double longMax)throw(RSGISCmdException);
    /** Function to calculate the mode for an individual image band within an envelope defined in Lat / Long */
    DllExport float executeImageBandModeEnv(std::string inputImage, float binWidth, unsigned int imgBand, bool noDataValueSpecified, float noDataVal, double latMin, double latMax, double longMin, double longMax)throw(RSGISCmdException);
    /** A function to calculate a 2D histogram comparison of two images */
    DllExport double executeImageComparison2dHisto(std::string inputImage1, std::string inputImage2, std::string outputImage, std::string gdalFormat, unsigned int img1Band, unsigned int img2Band, unsigned int numBins, double *binWidthImg1, double *binWidthImg2, double img1Min, double img1Max, double img2Min, double img2Max, double img1Scale=1.0, double img2Scale=1.0, double img1Off=0.0, double img2Off=0.0, bool normOutput=false) throw(RSGISCmdException);
    /** A function to compute the probability of the pixel value from a masked region of the image occuring */
    DllExport void executeCalcMaskImgPxlValProb(std::string inputImage, std::vector<unsigned int> inImgBandIdxs, std::string maskImage, int maskVal, std::string outputImage, std::string gdalFormat, std::vector<float> histBinWidths, bool calcHistBinWidth, bool useImgNoData, bool rescaleProbs) throw(RSGISCmdException);
    /** A function to compute the proportion of true outputs from the expression across the image and optionally within the binary mask */
    DllExport float executeCalcPropTrueExp(VariableStruct *variables, unsigned int numVars, std::string mathsExpression, std::string inValidImage, bool useValidImg) throw(RSGISCmdException);
    /** A function to calculate statistic (e.g., min) across a number of images */
    DllExport void calcMultiImgBandsStats(std::vector<std::string> inputImages, std::string outputImage, RSGISCmdsSummariseStats summaryStats, std::string gdalFormat, RSGISLibDataType outDataType, bool useNoData, float noDataVal) throw(RSGISCmdException);
    /** A function to calculate the difference between two images */
    DllExport void calcImageDifference(std::string inputImage1, std::string inputImage2, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);
    /** A function to get the min and max image values from the input image band specified */
    DllExport std::pair<double,double> getImageBandMinMax(std::string inputImage, unsigned int imgBand, bool useNoData=false, float noDataVal=0.0) throw(RSGISCmdException);
    /** A function to rescale an input image(s) to use a new scale and offset */
    DllExport void executeRescaleImages(std::vector<std::string> inputImgs, std::string outputImg, std::string gdalFormat, RSGISLibDataType outDataType, float cNoDataVal, float cOffset, float cGain, float nNoDataVal, float nOffset, float nGain) throw(RSGISCmdException);
    /** A function to get the index of an input list of images for a particular stat (e.g., min, max, median) */
    DllExport void executeGetImgIdxForStat(std::vector<std::string> inputImgs, std::string outputImg, std::string gdalFormat, float noDataVal, RSGISCmdsSummariseStats sumStat) throw(RSGISCmdException);
    /** A function to derieve summary stats for the high resolution image pixels for regions defined by the low resolution image pixels */
    DllExport void executeGetWithinPxlImgStatSummaries(std::string refImg, std::string statsImg, unsigned int statsImgBand, std::string outImg, std::string gdalFormat, RSGISLibDataType outDataType, bool useNoData, std::vector<RSGISCmdsSummariseStats> cmdSumStats, unsigned int xIOGrid, unsigned int yIOGrid) throw(RSGISCmdException);
    /** A function to identify the image band within the  minimum pixel value from a set of image bands */
    DllExport void executeIdentifyMinPxlValueInWin(std::string inputImg, std::string outputImg, std::string outputRefImg, std::vector<unsigned int> bands, unsigned int winSize, std::string gdalFormat, float noDataValue, bool useNoDataValue) throw(RSGISCmdException);
    /** A function to calculate a mean value across a number of image bands within a mask */
    DllExport float executeCalcImgMeanInMask(std::string inputImg, std::string inputImgMsk, int mskValue, std::vector<unsigned int> bands, float noDataValue, bool useNoDataValue) throw(RSGISCmdException);


}}


#endif

