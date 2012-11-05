/*
 *  RSGISMaxLikelihoodRATClassification.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/11/2012.
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

#include "RSGISMaxLikelihoodRATClassification.h"


namespace rsgis{namespace rastergis{
    
    RSGISMaxLikelihoodRATClassification::RSGISMaxLikelihoodRATClassification()
    {
        
    }
    
    void RSGISMaxLikelihoodRATClassification::applyMLClassifier(GDALDataset *image, std::string inClassCol, std::string outClassCol, std::string trainingSelectCol, std::string areaCol, std::vector<std::string> inColumns, rsgismlpriors priorsMethod, std::vector<float> defPriors) throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            const GDALRasterAttributeTable *attTableTmp = image->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *attTable = NULL;
            if(attTableTmp != NULL)
            {
                attTable = new GDALRasterAttributeTable(*attTableTmp);
            }
            else
            {
                attTable = new GDALRasterAttributeTable();
            }
            int numRows = attTable->GetRowCount();
            int numColumns = attTable->GetColumnCount();
            
            if(numRows == 0)
            {
                throw rsgis::RSGISAttributeTableException("The attribute table does not have any rows.");
            }
            
            if(numColumns == 0)
            {
                throw rsgis::RSGISAttributeTableException("The attribute table does not have any column.");
            }
            
            
            // Get the column indexes and create output column if not present.
            
            int inClassColIdx = 0;
            bool inClassColFound = false;
            int trainingSelectColIdx = 0;
            bool trainingSelectColFound = false;
            int outClassColIdx = 0;
            bool outClassColFound = false;
            int areaColIdx = 0;
            bool areaColFound = false;
            bool *foundIdx = new bool[inColumns.size()];
            int *colIdxs = new int[inColumns.size()];
            for(size_t i = 0; i < inColumns.size(); ++i)
            {
                foundIdx[i] = false;
                colIdxs[i] = 0;
            }
            
            for(int i = 0; i < numColumns; ++i)
            {
                if(!inClassColFound && (std::string(attTable->GetNameOfCol(i)) == inClassCol))
                {
                    inClassColFound = true;
                    inClassColIdx = i;
                }
                else if(!trainingSelectColFound && (std::string(attTable->GetNameOfCol(i)) == trainingSelectCol))
                {
                    trainingSelectColFound = true;
                    trainingSelectColIdx = i;
                }
                else if(!outClassColFound && (std::string(attTable->GetNameOfCol(i)) == outClassCol))
                {
                    outClassColFound = true;
                    outClassColIdx = i;
                }
                else if(!areaColFound && (std::string(attTable->GetNameOfCol(i)) == areaCol))
                {
                    areaColFound = true;
                    areaColIdx = i;
                }
                else
                {
                    for(size_t j = 0; j < inColumns.size(); ++j)
                    {
                        if(!foundIdx[j] && (std::string(attTable->GetNameOfCol(i)) == inColumns.at(j)))
                        {
                            colIdxs[j] = i;
                            foundIdx[j] = true;
                        }
                    }
                }
            }
            
            if(!inClassColFound)
            {
                throw rsgis::RSGISAttributeTableException("Could not find the input class column.");
            }
            
            if(!trainingSelectColFound)
            {
                throw rsgis::RSGISAttributeTableException("Could not find the selected for training column.");
            }
            
            if(!areaColFound)
            {
                throw rsgis::RSGISAttributeTableException("Could not find the area column.");
            }
            
            if(!outClassColFound)
            {
                attTable->CreateColumn(outClassCol.c_str(), GFT_Integer, GFU_Generic);
                outClassColIdx = numColumns++;
            }
            
            for(size_t j = 0; j < inColumns.size(); ++j)
            {
                if(!foundIdx[j])
                {
                    std::string message = std::string("Column ") + inColumns.at(j) + std::string(" is not within the attribute table.");
                    throw rsgis::RSGISAttributeTableException(message);
                }
            }
            delete[] foundIdx;
            
            int numTrainingSamples = 0;
            std::list<int> classes;
            int classID = 0;
            std::cout << "Finding the classes...\n";
            for(size_t i = 1; i < numRows; ++i)
            {
                classID = attTable->GetValueAsInt(i, inClassColIdx);
                if((attTable->GetValueAsInt(i, trainingSelectColIdx) == 1) & (classID > 0))
                {
                    classes.push_back(classID);
                    ++numTrainingSamples;
                }
            }
            
            classes.sort();
            classes.unique();
            
            rsgis::math::MaximumLikelihood *mlStruct = new rsgis::math::MaximumLikelihood();
            mlStruct->nclasses = classes.size();
            mlStruct->classes = new int[mlStruct->nclasses];
            mlStruct->d = inColumns.size();
            
            unsigned int idx = 0;
            std::vector<std::string> outColPostNames;
            rsgis::math::RSGISMathsUtils mathUtils;
            std::string colName = "";
            for(std::list<int>::iterator iterClasses = classes.begin(); iterClasses != classes.end(); ++iterClasses)
            {
                std::cout << "Class " << *iterClasses << std::endl;
                mlStruct->classes[idx] = *iterClasses;
                colName = outClassCol + std::string("_") + mathUtils.inttostring(*iterClasses);
                outColPostNames.push_back(colName);
                ++idx;
            }
            
            
            bool *foundOutPostIdx = new bool[mlStruct->nclasses];
            int *colOutPostIdxs = new int[mlStruct->nclasses];
            for(size_t i = 0; i < mlStruct->nclasses; ++i)
            {
                foundOutPostIdx[i] = false;
                colOutPostIdxs[i] = 0;
            }
            
            for(int i = 0; i < numColumns; ++i)
            {
                for(size_t j = 0; j < mlStruct->nclasses; ++j)
                {
                    if(!foundOutPostIdx[j] && (std::string(attTable->GetNameOfCol(i)) == outColPostNames.at(j)))
                    {
                        colOutPostIdxs[j] = i;
                        foundOutPostIdx[j] = true;
                    }
                }
            }
            
            for(size_t j = 0; j < mlStruct->nclasses; ++j)
            {
                if(!foundOutPostIdx[j])
                {
                    attTable->CreateColumn(outColPostNames.at(j).c_str(), GFT_Real, GFU_Generic);
                    colOutPostIdxs[j] = numColumns++;
                }
            }
            delete[] foundOutPostIdx;
            
            std::cout << "Training the ML classifier\n";
            int *mlClasses = new int[numTrainingSamples];
            double **samples = new double*[numTrainingSamples];
            double *samplesArea = new double[numTrainingSamples];
            double totalArea = 0;
            idx = 0;
            for(size_t i = 1; i < numRows; ++i)
            {
                classID = attTable->GetValueAsInt(i, inClassColIdx);
                if((attTable->GetValueAsInt(i, trainingSelectColIdx) == 1) & (classID > 0))
                {
                    mlClasses[idx] = classID;
                    samplesArea[idx] = attTable->GetValueAsDouble(i, areaColIdx);
                    totalArea += samplesArea[idx];
                    samples[idx] = new double[inColumns.size()];
                    for(size_t j = 0; j < inColumns.size(); ++j)
                    {
                        samples[idx][j] = attTable->GetValueAsDouble(i, colIdxs[j]);
                    }
                    ++idx;
                }
            }
                        
            rsgis::math::RSGISMaximumLikelihood mlObj;
            
            mlObj.compute_ml(mlStruct, numTrainingSamples, inColumns.size(), samples, mlClasses);
            
            for(int i = 0; i < mlStruct->nclasses; ++i)
            {
                if(priorsMethod == rsgis_equal)
                {
                    mlStruct->priors[i] = 1.0 / ((float)(mlStruct->nclasses));
                }
                else if(priorsMethod == rsgis_samples)
                {
                    // Do nothing this is the library default...
                }
                else if(priorsMethod == rsgis_area)
                {
                    double classAreaTotal = 0;
                    for(int n = 0; n < numTrainingSamples; ++n)
                    {
                        if(mlClasses[n] == mlStruct->classes[i])
                        {
                            classAreaTotal += samplesArea[n];
                        }
                    }
                    mlStruct->priors[i] = classAreaTotal / totalArea;
                }
                else if(priorsMethod == rsgis_userdefined)
                {
                    mlStruct->priors[i] = defPriors.at(i);
                }
                std::cout << "Class " << mlStruct->classes[i] << " has prior " << mlStruct->priors[i] << std::endl;
            }
            
            
            double *data = new double[inColumns.size()];
            double *posteriorProbs = new double[inColumns.size()];
            
            std::cout << "Iterating through columns to classify...\n";
            unsigned int feedbackStep = numRows/10;
            unsigned int feedback = 10;
            std::cout << "Started..0." << std::flush;
            for(int i = 1; i < numRows; ++i)
            {
                if( (numRows > 20) && (i % feedbackStep == 0))
                {
                    std::cout << "." << feedback << "." << std::flush;
                    feedback += 10;
                }
                classID = 0;
                if(attTable->GetValueAsInt(i, inClassColIdx) > 0)
                {
                    for(size_t j = 0; j < inColumns.size(); ++j)
                    {
                        data[j] = attTable->GetValueAsDouble(i, colIdxs[j]);
                    }
                    
                    classID = mlObj.predict_ml(mlStruct, data, &posteriorProbs);
                    
                    //std::cout << i << ")\n";
                    for(int j = 0; j < mlStruct->nclasses; ++j)
                    {
                        //std::cout << "\tClass " << mlStruct->classes[j] << " has Posterior "<< posteriorProbs[j] << std::endl;
                        attTable->SetValue(i, colOutPostIdxs[j], posteriorProbs[j]);
                    }
                    
                }
                attTable->SetValue(i, outClassColIdx, classID);
            }
            std::cout << ".Completed\n";
            
            image->GetRasterBand(1)->SetDefaultRAT(attTable);            
            
            delete[] mlClasses;
            for(int i = 0; i < numTrainingSamples; ++i)
            {
                delete[] samples[i];
            }
            delete[] samples;
            delete[] data;
            delete[] posteriorProbs;
            delete[] colOutPostIdxs;
            delete[] colIdxs;
            delete[] samplesArea;
        }
        catch(rsgis::RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISMaxLikelihoodRATClassification::~RSGISMaxLikelihoodRATClassification()
    {
        
    }
	
}}


