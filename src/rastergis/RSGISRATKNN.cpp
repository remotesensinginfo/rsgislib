/*
 *  RSGISRATKNN.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 28/02/2015.
 *  Copyright 2015 RSGISLib.
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

#include "RSGISRATKNN.h"

namespace rsgis{namespace rastergis{
    
    RSGISApplyRATKNN::RSGISApplyRATKNN()
    {
        
    }
    
    void RSGISApplyRATKNN::applyKNNExtrapolation(GDALDataset *clumpsDS, std::string inExtrapField, std::string outExtrapField, std::string trainRegionsField, std::string applyRegionsField, bool useApplyField, std::vector<std::string> fields, unsigned int kFeatures, rsgis::math::rsgisdistmetrics distKNN, float distThreshold, rsgis::math::rsgissummarytype summeriseKNN, unsigned int ratBand) throw(RSGISAttributeTableException)
    {
        try
        {
            std::cout << "Open Attribute Table.\n";
            GDALRasterAttributeTable *gdalAtt = clumpsDS->GetRasterBand(ratBand)->GetDefaultRAT();
            
            if((gdalAtt == NULL) || (gdalAtt->GetRowCount() == 0))
            {
                throw RSGISAttributeTableException("Input image does not have an attribute table.");
            }
            
            RSGISRasterAttUtils attUtils;
            
            unsigned int inExtrapFieldIdx = attUtils.findColumnIndex(gdalAtt, inExtrapField);
            unsigned int trainRegFieldIdx = attUtils.findColumnIndex(gdalAtt, trainRegionsField);
            unsigned int applyRegFieldIdx = 0;
            if(useApplyField)
            {
                applyRegFieldIdx = attUtils.findColumnIndex(gdalAtt, applyRegionsField);
            }
            unsigned int outExtrapFieldIdx = attUtils.findColumnIndexOrCreate(gdalAtt, outExtrapField, gdalAtt->GetTypeOfCol(inExtrapFieldIdx));
            
            
            // Find out how many training samples there are.
            std::cout << "Count Number of Training Sample\n";
            size_t numTrainFeats = 0;
            RSGISCountTrainingValues countTrainSamplesVals = RSGISCountTrainingValues(&numTrainFeats);
            RSGISRATCalc ratCalc = RSGISRATCalc(&countTrainSamplesVals);
            std::vector<unsigned int> inRealColIdx;
            std::vector<unsigned int> inIntColIdx;
            inIntColIdx.push_back(trainRegFieldIdx);
            std::vector<unsigned int> inStrColIdx;
            std::vector<unsigned int> outRealColIdx;
            std::vector<unsigned int> outIntColIdx;
            std::vector<unsigned int> outStrColIdx;
            ratCalc.calcRATValues(gdalAtt, inRealColIdx, inIntColIdx, inStrColIdx, outRealColIdx, outIntColIdx, outStrColIdx);
            
            // Check it is enough...
            if(numTrainFeats < kFeatures)
            {
                throw RSGISAttributeTableException("The number of training samples is less than the value of K.");
            }
            
            // Find fields from RAT
            inRealColIdx.push_back(inExtrapFieldIdx);
            std::vector<unsigned int> fieldsIdx;
            for(std::vector<std::string>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
            {
                unsigned int idx = attUtils.findColumnIndex(gdalAtt, (*iterFields));
                if(gdalAtt->GetTypeOfCol(idx) == GFT_String)
                {
                    std::string message = "Columns must either be of type 'Integer' or 'Float' for the distance to be calculated. Issue with: " + (*iterFields);
                    throw RSGISAttributeTableException(message);
                }
                fieldsIdx.push_back(idx);
                inRealColIdx.push_back(idx);
            }
            
            
            // Allocate memory for training data
            double **trainData = new double*[numTrainFeats];
            size_t numFloatVals = inRealColIdx.size();
            for(size_t i = 0; i < numTrainFeats; ++i)
            {
                trainData[i] = new double[numFloatVals];
                for(size_t j = 0; j < numFloatVals; ++j)
                {
                    trainData[i][j] = 0.0;
                }
            }
            
            // Extract training data
            std::cout << "Extract Training Data\n";
            RSGISExtractTrainingValues extractVals = RSGISExtractTrainingValues(trainData, numTrainFeats, numFloatVals);
            ratCalc = RSGISRATCalc(&extractVals);
            extractVals.resetCounter();
            ratCalc.calcRATValues(gdalAtt, inRealColIdx, inIntColIdx, inStrColIdx, outRealColIdx, outIntColIdx, outStrColIdx);
            
            /* Print training Data
            std::cout << "Training Data: [Extrap Val] = [Field Values]\n";
            for(size_t i = 0; i < numTrainFeats; ++i)
            {
                std::cout << i << "\t";
                for(size_t j = 0; j < numFloatVals; ++j)
                {
                    if(j == 0)
                    {
                        std::cout << "[" << trainData[i][j] << "] = [";
                    }
                    else if(j == 1)
                    {
                        std::cout << trainData[i][j];
                    }
                    else
                    {
                        std::cout << ", " << trainData[i][j];
                    }
                }
                std::cout << "]\n";
            }
            */
            
            rsgis::math::RSGISMathsUtils mathUtils;
            rsgis::math::RSGISStatsSummary *mathSumStats = new rsgis::math::RSGISStatsSummary();
            mathUtils.initStatsSummary(mathSumStats);
            if(summeriseKNN == rsgis::math::sumtype_mean)
            {
                mathSumStats->calcMean = true;
            }
            else if(summeriseKNN == rsgis::math::sumtype_median)
            {
                mathSumStats->calcMedian = true;
            }
            else if(summeriseKNN == rsgis::math::sumtype_mode)
            {
                mathSumStats->calcMode = true;
            }
            else if(summeriseKNN == rsgis::math::sumtype_min)
            {
                mathSumStats->calcMin = true;
            }
            else if(summeriseKNN == rsgis::math::sumtype_max)
            {
                mathSumStats->calcMax = true;
            }
            else
            {
                throw RSGISAttributeTableException("Summary method is not supported and/or known.");
            }
            
            rsgis::math::RSGISCalcDistMetric *calcDist = NULL;
            if(distKNN == rsgis::math::rsgis_euclidean)
            {
                calcDist = new rsgis::math::RSGISCalcEuclideanDistMetric();
                calcDist->init();
            }
            else if(distKNN == rsgis::math::rsgis_manhatten)
            {
                calcDist = new rsgis::math::RSGISCalcManhattenDistMetric();
                calcDist->init();
            }
            else if(distKNN == rsgis::math::rsgis_mahalanobis)
            {
                double *meanVec = mathUtils.calcMeanVector(trainData, numTrainFeats, numFloatVals, 1, numFloatVals);
                double **covarMatrix = mathUtils.calcCovarianceMatrix(trainData, meanVec, numTrainFeats, numFloatVals, 1, numFloatVals);
                size_t numVals = numFloatVals - 1;
                delete[] meanVec;
                calcDist = new rsgis::math::RSGISCalcMahalanobisDistMetric(covarMatrix, numVals);
                calcDist->init();
            }
            else if(distKNN == rsgis::math::rsgis_minkowski)
            {
                calcDist = new rsgis::math::RSGISCalcMinkowskiDistMetric();
                calcDist->init();
            }
            else if(distKNN == rsgis::math::rsgis_chebyshev)
            {
                calcDist = new rsgis::math::RSGISCalcChebyshevDistMetric();
                calcDist->init();
            }
            else
            {
                throw RSGISAttributeTableException("Distance method is not supported and/or known.");
            }
            
                
            // Perform KNN
            std::cout << "Perform KNN\n";
            inIntColIdx.clear();
            if(useApplyField)
            {
                inIntColIdx.push_back(applyRegFieldIdx);
            }
            outRealColIdx.push_back(outExtrapFieldIdx);
            RSGISPerformKNNCalcValues performKNN = RSGISPerformKNNCalcValues(trainData, numTrainFeats, numFloatVals, kFeatures, calcDist, distThreshold, mathSumStats);
            ratCalc = RSGISRATCalc(&performKNN);
            ratCalc.calcRATValues(gdalAtt, inRealColIdx, inIntColIdx, inStrColIdx, outRealColIdx, outIntColIdx, outStrColIdx);
            
            // Deallocate memory
            for(size_t i = 0; i < numTrainFeats; ++i)
            {
                delete[] trainData[i];
            }
            delete[]trainData;
            delete mathSumStats;
            delete calcDist;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISApplyRATKNN::~RSGISApplyRATKNN()
    {
        
    }
    
    RSGISCountTrainingValues::RSGISCountTrainingValues(size_t *numTrainPts): RSGISRATCalcValue()
    {
        this->numTrainPts = numTrainPts;
    }
    
    void RSGISCountTrainingValues::calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException)
    {
        if(numInIntCols != 1)
        {
            throw RSGISAttributeTableException("Only expecting one and only one integer column with values of 1 indicating a training sample.");
        }
        
        if(inIntCols[0] == 1)
        {
            ++(*numTrainPts);
        }
    }
    
    RSGISCountTrainingValues::~RSGISCountTrainingValues()
    {
        
    }
    
    RSGISExtractTrainingValues::RSGISExtractTrainingValues(double **trainData, size_t n, size_t m): RSGISRATCalcValue()
    {
        this->trainData = trainData;
        this->n = n;
        this->m = m;
        this->counter = 0;
    }
    
    void RSGISExtractTrainingValues::calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException)
    {
        if(numInIntCols != 1)
        {
            throw RSGISAttributeTableException("Only expecting one and only one integer column with values of 1 indicating a training sample.");
        }
        
        
        if(inIntCols[0] == 1)
        {
            if(m != numInRealCols)
            {
                throw RSGISAttributeTableException("The number of real number columns is not the same size as the array; which ones to copy?");
            }
            
            if(counter == n)
            {
                throw RSGISAttributeTableException("The array for the training data is not long enough.");
            }
            
            for(size_t i = 0; i < this->m; ++i)
            {
                trainData[counter][i] = inRealCols[i];
            }
            ++counter;
        }
    }
    
    void RSGISExtractTrainingValues::resetCounter()
    {
        this->counter = 0;
    }
    
    RSGISExtractTrainingValues::~RSGISExtractTrainingValues()
    {
        
    }
    
    

    RSGISPerformKNNCalcValues::RSGISPerformKNNCalcValues(double **trainData, size_t n, size_t m, unsigned int kFeatures, rsgis::math::RSGISCalcDistMetric *calcDist, float distThreshold, rsgis::math::RSGISStatsSummary *mathSumStats):RSGISRATCalcValue()
    {
        this->trainData = trainData;
        this->n = n;
        this->m = m;
        this->kFeatures = kFeatures;
        this->calcDist = calcDist;
        this->distThreshold = distThreshold;
        this->mathSumStats = mathSumStats;
    }
    
    void RSGISPerformKNNCalcValues::calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException)
    {
        if(numOutRealCols != 1)
        {
            throw RSGISAttributeTableException("Only one output float column should be specified as otherwise don't know which column to write to?");
        }
        
        if(m != numInRealCols)
        {
            throw RSGISAttributeTableException("The number of real number columns is not the same size as the array; which ones to copy?");
        }
        
        try
        {
            bool performKNN = true;
            if(numInIntCols == 1)
            {
                if(inIntCols[0] != 1)
                {
                    performKNN = false;
                }
            }
            if(performKNN)
            {
                // Find K NN samples from training data
                std::list<std::pair<double, double*> > *kVals = new std::list<std::pair<double, double*> >();
                this->findKVals(kVals, inRealCols);
                
                // Derive new value from K NN samples
                std::vector<double> data;
                for(std::list<std::pair<double, double*> >::iterator iterFeat = kVals->begin(); iterFeat != kVals->end(); ++iterFeat)
                {
                    data.push_back((*iterFeat).second[0]);
                }
                rsgis::math::RSGISMathsUtils mathUtils;
                mathUtils.generateStats(&data, this->mathSumStats);
                
                // Write to output column
                if(this->mathSumStats->calcMean)
                {
                    outRealCols[0] = this->mathSumStats->mean;
                }
                else if(this->mathSumStats->calcMedian)
                {
                    outRealCols[0] = this->mathSumStats->median;
                }
                else if(this->mathSumStats->calcMax)
                {
                    outRealCols[0] = this->mathSumStats->max;
                }
                else if(this->mathSumStats->calcMin)
                {
                    outRealCols[0] = this->mathSumStats->min;
                }
                else if(this->mathSumStats->calcMode)
                {
                    outRealCols[0] = this->mathSumStats->mode;
                }
                else if(this->mathSumStats->calcStdDev)
                {
                    outRealCols[0] = this->mathSumStats->stdDev;
                }
                else if(this->mathSumStats->calcSum)
                {
                    outRealCols[0] = this->mathSumStats->sum;
                }
                else
                {
                    throw RSGISAttributeTableException("Summarise option unknown.");
                }
                delete kVals;
            }
            else
            {
                outRealCols[0] = std::numeric_limits<double>::signaling_NaN();
            }
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
       
        
    }
    
    void RSGISPerformKNNCalcValues::findKVals(std::list<std::pair<double, double*> > *kVals, double *featVals)throw(RSGISAttributeTableException)
    {
        try
        {
            double dist = 0.0;
            for(size_t i = 0; i < this->n; ++i)
            {
                dist = this->calcDist->calcDist(this->trainData[i], 1, m, featVals, 1, m);

                if(dist < this->distThreshold)
                {
                    if(kVals->empty())
                    {
                        kVals->push_back(std::pair<double, double*>(dist, this->trainData[i]));
                    }
                    else
                    {
                        for(std::list<std::pair<double, double*> >::iterator iterFeat = kVals->begin(); iterFeat != kVals->end(); ++iterFeat)
                        {
                            if(dist < (*iterFeat).first)
                            {
                                kVals->insert(iterFeat, std::pair<double, double*>(dist, this->trainData[i]));
                                break;
                            }
                        }
                        if(kVals->size() > this->kFeatures)
                        {
                            kVals->pop_back();
                        }
                    }
                }
            }
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISPerformKNNCalcValues::~RSGISPerformKNNCalcValues()
    {
        
    }

    
    
    
}}


