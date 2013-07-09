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

namespace rsgis{ namespace cmds {
    
    struct VariableStruct
    {
        std::string image;
        std::string name;
        int bandNum;
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
    
    /** Function to run the band maths tools */
    void executeBandMaths(VariableStruct *variables, unsigned int numVars, std::string outputImage, std::string mathsExpression, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** Function to run the image maths tools */
    void executeImageMaths(std::string inputImage, std::string outputImage, std::string mathsExpression, std::string imageFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** Function to run the KMeans tool */
    void executeKMeansClustering(std::string inputImage, std::string outputMatrixFile, unsigned int numClusters, unsigned int maxNumIterations, unsigned int subSample, bool ignoreZeros, float degreeOfChange, RSGISInitClustererMethods initClusterMethod)throw(RSGISCmdException);
    /** Function to run the KMeans tool */
    void executeISODataClustering(std::string inputImage, std::string outputMatrixFile, unsigned int numClusters, unsigned int maxNumIterations, unsigned int subSample, bool ignoreZeros, float degreeOfChange, RSGISInitClustererMethods initClusterMethod, float minDistBetweenClusters, unsigned int minNumFeatures, float maxStdDev, unsigned int minNumClusters, unsigned int startIteration, unsigned int endIteration)throw(RSGISCmdException);
    /** Function to run mahalanobis distance Window Filter */
    void executeMahalanobisDistFilter(std::string inputImage, std::string outputImage, unsigned int winSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** Function to run mahalanobis distance Image to Window Filter */
    void executeMahalanobisDist2ImgFilter(std::string inputImage, std::string outputImage, unsigned int winSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** Function to run image calculate distance command */
    void executeImageCalcDistance(std::string inputImage, std::string outputImage, std::string gdalFormat)throw(RSGISCmdException);
    /** Function to calculate summary statistics for a column of pixels */
    void executeImagePixelColumnSummary(std::string inputImage, std::string outputImage, rsgis::cmds::RSGISCmdStatsSummary summaryStats, std::string gdalFormat, RSGISLibDataType outDataType, float noDataValue, bool useNoDataValue)throw(RSGISCmdException);
    /** Function to perform a linear regression on each column of pixels */
    void executeImagePixelLinearFit(std::string inputImage, std::string outputImage, std::string gdalFormat, std::string bandValues, float noDataValue, bool useNoDataValue)throw(RSGISCmdException);
    /** Function to perform image normalisation */
    void executeNormalisation(std::vector<std::string> inputImages, std::vector<std::string> outputImages, bool calcInMinMax, double inMin, double inMax, double outMin, double outMax)throw(RSGISCmdException);
    /** Function to calculate the correlation between 2 images */
    void executeCorrelation(std::string inputImageA, std::string inputImageB, std::string outputMatrix) throw(RSGISCmdException);
    /** Function to calculate the covariance between 2 images */
    void executeCovariance(std::string inputImageA, std::string inputImageB, std::string inputMatrixA, std::string inputMatrixB, bool shouldCalcMean, std::string outputMatrix)throw(RSGISCmdException);
    /** Function to calculate the mean vector of an image */
    void executeMeanVector(std::string inputImage, std::string outputMatrix)throw(RSGISCmdException);
    /** Function to perform principal components analysis of an image */
    void executePCA(std::string eigenvectors, std::string inputImage, std::string outputImage, int numComponents)throw(RSGISCmdException);
    /** Function to generate a standardised image using the mean vector provided */
    void executeStandardise(std::string meanvectorStr, std::string inputImage, std::string outputImage)throw(RSGISCmdException);
    /** Function to replace values less then given, using a threshold */
    void executeReplaceValuesLessThan(std::string inputImage, std::string outputImage, double threshold, double value)throw(RSGISCmdException);
}}


#endif

