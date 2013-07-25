/*
 *  RSGISCmdImageCalc.cpp
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

#include "RSGISCmdImageCalc.h"
#include "RSGISCmdParent.h"

#include "common/RSGISImageException.h"

#include "img/RSGISBandMath.h"
#include "img/RSGISImageMaths.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISImageClustering.h"
#include "img/RSGISImageWindowStats.h"
#include "img/RSGISImageStatistics.h"
#include "img/RSGISCalcCovariance.h"
#include "img/RSGISCalcEditImage.h"
#include "img/RSGISCalcDist2Geom.h"
#include "img/RSGISFitFunction2Pxls.h"

#include "math/RSGISVectors.h"
#include "math/RSGISMatrices.h"

#include "utils/RSGISTextUtils.h"

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

#include "muParser.h"

namespace rsgis{ namespace cmds {
    
    void executeBandMaths(VariableStruct *variables, unsigned int numVars, std::string outputImage, std::string mathsExpression, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        std::string *outBandName = new std::string[1];
        outBandName[0] = mathsExpression;
        
        GDALAllRegister();
        GDALDataset **datasets = NULL;
        rsgis::img::RSGISBandMath *bandmaths = NULL;
        rsgis::img::RSGISCalcImage *calcImage = NULL;
        mu::Parser *muParser = new mu::Parser();
        
        try
        {
            rsgis::img::VariableBands **processVaribles = new rsgis::img::VariableBands*[numVars];
            datasets = new GDALDataset*[numVars];
            
            int numRasterBands = 0;
            int totalNumRasterBands = 0;
            
            for(int i = 0; i < numVars; ++i)
            {
                std::cout << variables[i].image << std::endl;
                datasets[i] = (GDALDataset *) GDALOpen(variables[i].image.c_str(), GA_ReadOnly);
                if(datasets[i] == NULL)
                {
                    std::string message = std::string("Could not open image ") + variables[i].image;
                    throw rsgis::RSGISImageException(message.c_str());
                }
                
                numRasterBands = datasets[i]->GetRasterCount();
                
                if((variables[i].bandNum < 0) | (variables[i].bandNum > numRasterBands))
                {
                    throw rsgis::RSGISImageException("You have specified a band which is not within the image");
                }
                
                processVaribles[i] = new rsgis::img::VariableBands();
                processVaribles[i]->name = variables[i].name;
                processVaribles[i]->band = totalNumRasterBands + (variables[i].bandNum - 1);
                
                totalNumRasterBands += numRasterBands;
            }
            
            mu::value_type *inVals = new mu::value_type[numVars];
            for(int i = 0; i < numVars; ++i)
            {
                inVals[i] = 0;
                muParser->DefineVar(_T(processVaribles[i]->name.c_str()), &inVals[i]);
            }
            
            muParser->SetExpr(mathsExpression.c_str());
            
            bandmaths = new rsgis::img::RSGISBandMath(1, processVaribles, numVars, muParser);
            
            calcImage = new rsgis::img::RSGISCalcImage(bandmaths, "", true);
            calcImage->calcImage(datasets, numVars, outputImage, true, outBandName, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            for(int i = 0; i < numVars; ++i)
            {
                GDALClose(datasets[i]);
                delete processVaribles[i];
            }
            delete[] datasets;
            delete[] processVaribles;
            
            delete[] inVals;
            
            delete muParser;
            delete bandmaths;
            delete calcImage;
            delete[] outBandName;
            //GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (mu::ParserError &e)
        {
            std::string message = std::string("ERROR: ") + std::string(e.GetMsg()) + std::string(":\t \'") + std::string(e.GetExpr()) + std::string("\'");
            throw RSGISCmdException(message);
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeImageMaths(std::string inputImage, std::string outputImage, std::string mathsExpression, std::string imageFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        GDALAllRegister();
        GDALDataset **datasets = NULL;
        rsgis::img::RSGISImageMaths *imageMaths = NULL;
        rsgis::img::RSGISCalcImage *calcImage = NULL;
        mu::Parser *muParser = new mu::Parser();
        
        try
        {
            datasets = new GDALDataset*[1];
            
            datasets[0] = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            int numRasterBands = datasets[0]->GetRasterCount();
            
            mu::value_type inVal;
            muParser->DefineVar(_T("b1"), &inVal);
            muParser->SetExpr(mathsExpression.c_str());
            
            imageMaths = new rsgis::img::RSGISImageMaths(numRasterBands, muParser);
            
            calcImage = new rsgis::img::RSGISCalcImage(imageMaths, "", true);
            calcImage->calcImage(datasets, 1, outputImage, false, NULL, imageFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(datasets[0]);
            delete[] datasets;
            
            delete muParser;
            delete imageMaths;
            delete calcImage;
            //GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (mu::ParserError &e)
        {
            std::string message = std::string("ERROR: ") + std::string(e.GetMsg()) + std::string(":\t \'") + std::string(e.GetExpr()) + std::string("\'");
            throw RSGISCmdException(message);
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeKMeansClustering(std::string inputImage, std::string outputMatrixFile, unsigned int numClusters, unsigned int maxNumIterations, unsigned int subSample, bool ignoreZeros, float degreeOfChange, RSGISInitClustererMethods initClusterMethod)throw(RSGISCmdException)
    {
        
        std::cout << "inputImage = " << inputImage << std::endl;
        std::cout << "outputMatrixFile = " << outputMatrixFile << std::endl;
        std::cout << "numClusters = " << numClusters << std::endl;
        std::cout << "maxNumIterations = " << maxNumIterations << std::endl;
        std::cout << "subSample = " << subSample << std::endl;
        std::cout << "degreeOfChange = " << degreeOfChange << std::endl;
        if(ignoreZeros)
        {
            std::cout << "Ignoring Zeros\n";
        }
        
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::math::InitClustererMethods initMethod = rsgis::math::init_diagonal_full_attach;
            
            switch( initClusterMethod )
            {
                case rsgis::cmds::rsgis_init_random:
                    initMethod = rsgis::math::init_random;
                    break;
                case rsgis::cmds::rsgis_init_diagonal_full:
                    initMethod = rsgis::math::init_diagonal_full;
                    break;
                case rsgis::cmds::rsgis_init_diagonal_stddev:
                    initMethod = rsgis::math::init_diagonal_stddev;
                    break;
                case rsgis::cmds::rsgis_init_diagonal_full_attach:
                    std::cout << "Full diag attached.\n";
                    initMethod = rsgis::math::init_diagonal_full_attach;
                    break;
                case rsgis::cmds::rsgis_init_diagonal_stddev_attach:
                    initMethod = rsgis::math::init_diagonal_stddev_attach;
                    break;
                case rsgis::cmds::rsgis_init_kpp:
                    initMethod = rsgis::math::init_kpp;
                    break;
                default:
                    initMethod = rsgis::math::init_diagonal_full_attach;
                    break;
            }
            
            rsgis::img::RSGISImageClustering imgClustering;
            imgClustering.findKMeansCentres(dataset, outputMatrixFile, numClusters, maxNumIterations, subSample, ignoreZeros, degreeOfChange, initMethod);
            
            GDALClose(dataset);
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

    void executeISODataClustering(std::string inputImage, std::string outputMatrixFile, unsigned int numClusters, unsigned int maxNumIterations, unsigned int subSample, bool ignoreZeros, float degreeOfChange, RSGISInitClustererMethods initClusterMethod, float minDistBetweenClusters, unsigned int minNumFeatures, float maxStdDev, unsigned int minNumClusters, unsigned int startIteration, unsigned int endIteration)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::math::InitClustererMethods initMethod = rsgis::math::init_diagonal_full_attach;
            
            switch( initClusterMethod )
            {
                case rsgis::cmds::rsgis_init_random:
                    initMethod = rsgis::math::init_random;
                    break;
                case rsgis::cmds::rsgis_init_diagonal_full:
                    initMethod = rsgis::math::init_diagonal_full;
                    break;
                case rsgis::cmds::rsgis_init_diagonal_stddev:
                    initMethod = rsgis::math::init_diagonal_stddev;
                    break;
                case rsgis::cmds::rsgis_init_diagonal_full_attach:
                    initMethod = rsgis::math::init_diagonal_full_attach;
                    break;
                case rsgis::cmds::rsgis_init_diagonal_stddev_attach:
                    initMethod = rsgis::math::init_diagonal_stddev_attach;
                    break;
                case rsgis::cmds::rsgis_init_kpp:
                    initMethod = rsgis::math::init_kpp;
                    break;
                default:
                    initMethod = rsgis::math::init_diagonal_full_attach;
                    break;
            }
            
            rsgis::img::RSGISImageClustering imgClustering;
            imgClustering.findISODataCentres(dataset, outputMatrixFile, numClusters, maxNumIterations, subSample, ignoreZeros, degreeOfChange, initMethod, minDistBetweenClusters, minNumFeatures, maxStdDev, minNumClusters, startIteration, endIteration);
            
            GDALClose(dataset);
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
    
    void executeMahalanobisDistFilter(std::string inputImage, std::string outputImage, unsigned int winSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset **datasets = new GDALDataset*[1];
            
            datasets[0] = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISCalcImage *calcImage = NULL;
            
            rsgis::img::RSGISCalcImgPxlNeighboursDist *calcDistWindow = new rsgis::img::RSGISCalcImgPxlNeighboursDist();
            
            calcImage = new rsgis::img::RSGISCalcImage(calcDistWindow, "", true);
            calcImage->calcImageWindowData(datasets, 1, outputImage, winSize, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            delete calcDistWindow;
            delete calcImage;
            
            
            
            GDALClose(datasets[0]);
            delete[] datasets;
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
    
    void executeMahalanobisDist2ImgFilter(std::string inputImage, std::string outputImage, unsigned int winSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset **datasets = new GDALDataset*[1];
            
            datasets[0] = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISCalcImage *calcImage = NULL;
            
            // Initial Mahalanobis distance Calculation
            int numRasterBands = datasets[0]->GetRasterCount();
            rsgis::img::ImageStats **imgStats = new rsgis::img::ImageStats*[numRasterBands];
            for(int i = 0; i < numRasterBands; ++i)
            {
                imgStats[i] = new rsgis::img::ImageStats();
                imgStats[i]->max = 0;
                imgStats[i]->min = 0;
                imgStats[i]->mean = 0;
                imgStats[i]->sum = 0;
                imgStats[i]->stddev = 0;
            }
            rsgis::img::RSGISImageStatistics calcStats;
            calcStats.calcImageStatistics(datasets, 1, imgStats, numRasterBands, true, true);
            
            rsgis::math::RSGISVectors vecUtils;
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Vector *varMeans = vecUtils.createVector(numRasterBands);
            for(int i = 0; i < numRasterBands; ++i)
            {
                varMeans->vector[i] = imgStats[i]->mean;
                delete imgStats[i];
            }
            delete[] imgStats;
            
            rsgis::math::Matrix *covarianceMatrix = matrixUtils.createMatrix(numRasterBands, numRasterBands);
            matrixUtils.setValues(covarianceMatrix, 0.0);
            
            rsgis::img::RSGISCreateCovarianceMatrix createCovarMatrix = rsgis::img::RSGISCreateCovarianceMatrix(varMeans, covarianceMatrix);
            calcImage = new rsgis::img::RSGISCalcImage(&createCovarMatrix, "", true);
            calcImage->calcImage(datasets, 1);
            delete calcImage;
            
            std::cout << "Covariance Matrix:\n";
            matrixUtils.printMatrix(covarianceMatrix);
            
            size_t numVals = covarianceMatrix->m;
            
            gsl_matrix *coVarGSL = matrixUtils.convertRSGIS2GSLMatrix(covarianceMatrix);
            
            gsl_matrix *invCovarianceMatrix = gsl_matrix_alloc(covarianceMatrix->m, covarianceMatrix->n);
            gsl_permutation *p = gsl_permutation_alloc(covarianceMatrix->m);
            int signum = 0;
            gsl_linalg_LU_decomp(coVarGSL, p, &signum);
            gsl_linalg_LU_invert (coVarGSL, p, invCovarianceMatrix);
            gsl_permutation_free(p);
            gsl_matrix_free(coVarGSL);
            
            std::cout << "Inverse Covariance Matrix:\n";
            matrixUtils.printGSLMatrix(invCovarianceMatrix);
            
            gsl_vector *dVals = gsl_vector_alloc(numVals);
            gsl_vector *outVec = gsl_vector_alloc(numVals);
            
            rsgis::img::RSGISCalcImgPxl2WindowDist *calcDistWindow = new rsgis::img::RSGISCalcImgPxl2WindowDist(invCovarianceMatrix, varMeans, dVals, outVec);

            calcImage = new rsgis::img::RSGISCalcImage(calcDistWindow, "", true);
            calcImage->calcImageWindowData(datasets, 1, outputImage, winSize, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            delete calcDistWindow;
            delete calcImage;
            gsl_vector_free(dVals);
            gsl_vector_free(outVec);
            gsl_matrix_free(invCovarianceMatrix);
            matrixUtils.freeMatrix(covarianceMatrix);
            vecUtils.freeVector(varMeans);
            
            GDALClose(datasets[0]);
            delete[] datasets;
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
    
    void executeImageCalcDistance(std::string inputImage, std::string outputImage, std::string gdalFormat)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *imgDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(imgDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            // Create blank image
            rsgis::img::RSGISImageUtils imageUtils;
            GDALDataset *outImage = imageUtils.createCopy(imgDataset, 1, outputImage, gdalFormat, GDT_Float32);
            imageUtils.copyFloatGDALDataset(imgDataset, outImage);
            
            double *transform = new double[6];
            outImage->GetGeoTransform(transform);
            
            rsgis::img::RSGISCalcDistViaIterativeGrowth *calcDist = new rsgis::img::RSGISCalcDistViaIterativeGrowth(transform[1]);
            rsgis::img::RSGISCalcEditImage *calcEditImage = new rsgis::img::RSGISCalcEditImage(calcDist);
            
            bool change = true;
            unsigned int dist = 1;
            while(change)
            {
                std::cout << "Distance " << dist << ":\t" << std::flush;
                calcDist->resetChange();
                calcEditImage->calcImageWindowData(outImage, 3, -2);
                change = calcDist->changeOccurred();
                calcDist->incrementCounter();
                dist += 1;
            }
            
            // Translate to distance...
            std::cout << "Multiple by pixel size\n";
            calcEditImage->calcImageUseOut(outImage);
            
            // Clean up memory.
            GDALClose(outImage);
            GDALClose(imgDataset);
            delete[] transform;
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
    
    void executeImagePixelColumnSummary(std::string inputImage, std::string outputImage, rsgis::cmds::RSGISCmdStatsSummary summaryStats, std::string gdalFormat, RSGISLibDataType outDataType, float noDataValue, bool useNoDataValue)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *imgDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(imgDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMathsUtils mathUtils;
            rsgis::math::RSGISStatsSummary *mathSummaryStats = new rsgis::math::RSGISStatsSummary();
            mathUtils.initStatsSummary(mathSummaryStats);
            
            mathSummaryStats->calcMin = summaryStats.calcMin;
            mathSummaryStats->calcMax = summaryStats.calcMax;
            mathSummaryStats->calcMean = summaryStats.calcMean;
            mathSummaryStats->calcStdDev = summaryStats.calcStdDev;
            mathSummaryStats->calcSum = summaryStats.calcSum;
            mathSummaryStats->calcMedian = summaryStats.calcMedian;
            
            unsigned int numOutBands = 0;
            if(mathSummaryStats->calcMin)
            {
                ++numOutBands;
            }
            if(mathSummaryStats->calcMax)
            {
                ++numOutBands;
            }
            if(mathSummaryStats->calcMean)
            {
                ++numOutBands;
            }
            if(mathSummaryStats->calcMedian)
            {
                ++numOutBands;
            }
            if(mathSummaryStats->calcSum)
            {
                ++numOutBands;
            }
            if(mathSummaryStats->calcStdDev)
            {
                ++numOutBands;
            }
            
            if(numOutBands == 0)
            {
                throw RSGISException("No summaries where specified and therefore there would be no output image bands.");
            }
            
            std::string *bandNames = new std::string[numOutBands];
            int nameIdx = 0;
            if(mathSummaryStats->calcMin)
            {
                bandNames[nameIdx++] = "Min";
            }
            if(mathSummaryStats->calcMax)
            {
                bandNames[nameIdx++] = "Max";
            }
            if(mathSummaryStats->calcMean)
            {
                bandNames[nameIdx++] = "Mean";
            }
            if(mathSummaryStats->calcMedian)
            {
                bandNames[nameIdx++] = "Median";
            }
            if(mathSummaryStats->calcSum)
            {
                bandNames[nameIdx++] = "Sum";
            }
            if(mathSummaryStats->calcStdDev)
            {
                bandNames[nameIdx++] = "StdDev";
            }
            
            rsgis::img::RSGISImagePixelSummaries *pxlSummary = new rsgis::img::RSGISImagePixelSummaries(numOutBands, mathSummaryStats, noDataValue, useNoDataValue);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(pxlSummary, "", true);
            calcImage.calcImage(&imgDataset, 1, outputImage, true, bandNames, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            delete[] bandNames;
            delete pxlSummary;
            GDALClose(imgDataset);
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
    
    
    void executeImagePixelLinearFit(std::string inputImage, std::string outputImage, std::string gdalFormat, std::string bandValues, float noDataValue, bool useNoDataValue)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *imgDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(imgDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::utils::RSGISTextUtils textUtils;
            std::vector<std::string> strValues = textUtils.readFileToStringVector(bandValues);
            std::vector<float> bandXValues;
            for(std::vector<std::string>::iterator iterStrVals = strValues.begin(); iterStrVals != strValues.end(); ++iterStrVals)
            {
                if((*iterStrVals) != "")
                {
                    try
                    {
                        bandXValues.push_back(textUtils.strtofloat(*iterStrVals));
                    }
                    catch (rsgis::RSGISException &e)
                    {
                        // ignore.
                        std::cout << "Warning \'" << *iterStrVals << "\' could not be converted to an float.\n";
                    }
                }
            }
            
            if(bandXValues.size() != imgDataset->GetRasterCount())
            {
                std::cout << "bandXValues.size() = " << bandXValues.size() << std::endl;
                std::cout << "imgDataset->GetRasterCount() = " << imgDataset->GetRasterCount() << std::endl;
                GDALClose(imgDataset);
                throw RSGISException("The number of image bands and x values are not the same.");
            }
            
            std::string *bandNames = new std::string[3];
            bandNames[0] = "Intercept";
            bandNames[1] = "Slope";
            bandNames[2] = "SumSq";
            
            rsgis::img::RSGISLinearFit2Column *linearFit = new rsgis::img::RSGISLinearFit2Column(bandXValues, noDataValue, useNoDataValue);
            
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(linearFit, "", true);
            calcImage.calcImage(&imgDataset, 1, outputImage, true, bandNames, gdalFormat, GDT_Float32);
            
            delete[] bandNames;
            delete linearFit;
            
            
            GDALClose(imgDataset);
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

