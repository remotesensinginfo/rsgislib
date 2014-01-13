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
            
            // find the ids of the classes we need to train on            
            int numTrainingSamples = 0;
            std::set<int> classes;
            int classID = 0;
            std::cout << "Finding the classes...\n";
            for(size_t i = 1; i < numRows; ++i)
            {
                classID = attTable->GetValueAsInt(i, inClassColIdx);
                if((attTable->GetValueAsInt(i, trainingSelectColIdx) == 1) & (classID > 0))
                {
                    classes.insert(classID);
                    ++numTrainingSamples;
                }
            }

            // by default, a std::set is sorted and contains unique values
            // however, rsgis::math::MaximumLikelihood needs values to be 
            // contiguous so we set up a mapping
            std::map<int, int> forwardMapping;
            std::map<int, int> backMapping;

            if( classes.size() < 2 )
            {
                throw rsgis::RSGISAttributeTableException("need at least 2 unique classes");
            }
            else if( classes.size() == 2 )
            {
                // for some reason the ml stuff needs the values to be -1 and 1 in this case
                std::set<int>::iterator itr = classes.begin();
                forwardMapping.insert(std::pair<int, int>(*(itr), -1));
                backMapping.insert(std::pair<int, int>(-1, *(itr)));
                itr++;
                forwardMapping.insert(std::pair<int, int>(*(itr), 1));
                backMapping.insert(std::pair<int, int>(1, (*itr)));
            }
            else
            {
                // make a contiguous mapping
                classID = 1;
                for(std::set<int>::iterator itr = classes.begin(); itr != classes.end(); itr++)
                {
                    forwardMapping.insert(std::pair<int, int>(*(itr), classID));
                    backMapping.insert(std::pair<int, int>(classID, *(itr)));
                    classID++;
                }
            }
            
            rsgis::math::MaximumLikelihood *mlStruct = new rsgis::math::MaximumLikelihood();
            mlStruct->nclasses = classes.size();
            mlStruct->classes = new int[mlStruct->nclasses];
            mlStruct->d = inColumns.size();
            
            unsigned int idx = 0;
            std::vector<std::string> outColPostNames;
            rsgis::math::RSGISMathsUtils mathUtils;
            std::string colName = "";
            for(std::set<int>::iterator iterClasses = classes.begin(); iterClasses != classes.end(); ++iterClasses)
            {
                std::cout << "Class " << *iterClasses << " mapped to " << forwardMapping[*iterClasses] << std::endl;
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
                classID = forwardMapping[classID]; // do lookup into contiguous classes
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
                    classID = backMapping[classID]; // convert back to our non contigous classes
                    
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
    
    void RSGISMaxLikelihoodRATClassification::applyMLClassifierLocalPriors(GDALDataset *image, std::string inClassCol, std::string outClassCol, std::string trainingSelectCol, std::string areaCol, std::vector<std::string> inColumns, std::string eastingsCol, std::string northingsCol, float searchRadius, rsgismlpriors priorsMethod, float weightA, bool allowZeroPriors) throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            // get the RAT
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
            int eastColIdx = 0;
            bool eastColFound = false;
            int northColIdx = 0;
            bool northColFound = false;
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
                else if(!eastColFound && (std::string(attTable->GetNameOfCol(i)) == eastingsCol))
                {
                    eastColFound = true;
                    eastColIdx = i;
                }
                else if(!northColFound && (std::string(attTable->GetNameOfCol(i)) == northingsCol))
                {
                    northColFound = true;
                    northColIdx = i;
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
            
            if(!eastColFound)
            {
                throw rsgis::RSGISAttributeTableException("Could not find the eastings column.");
            }
            
            if(!northColFound)
            {
                throw rsgis::RSGISAttributeTableException("Could not find the northings column.");
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

            // find the ids of the classes we need to train on            
            int numTrainingSamples = 0;
            std::set<int> classes;
            int classID = 0;
            std::cout << "Finding the classes...\n";
            for(size_t i = 1; i < numRows; ++i)
            {
                classID = attTable->GetValueAsInt(i, inClassColIdx);
                if((attTable->GetValueAsInt(i, trainingSelectColIdx) == 1) & (classID > 0))
                {
                    classes.insert(classID);
                    ++numTrainingSamples;
                }
            }

            // by default, a std::set is sorted and contains unique values
            // however, rsgis::math::MaximumLikelihood needs values to be 
            // contiguous so we set up a mapping
            std::map<int, int> forwardMapping;
            std::map<int, int> backMapping;

            if( classes.size() < 2 )
            {
                throw rsgis::RSGISAttributeTableException("need at least 2 unique classes");
            }
            else if( classes.size() == 2 )
            {
                // for some reason the ml stuff needs the values to be -1 and 1 in this case
                std::set<int>::iterator itr = classes.begin();
                forwardMapping.insert(std::pair<int, int>(*(itr), -1));
                backMapping.insert(std::pair<int, int>(-1, *(itr)));
                itr++;
                forwardMapping.insert(std::pair<int, int>(*(itr), 1));
                backMapping.insert(std::pair<int, int>(1, (*itr)));
            }
            else
            {
                // make a contiguous mapping
                classID = 1;
                for(std::set<int>::iterator itr = classes.begin(); itr != classes.end(); itr++)
                {
                    forwardMapping.insert(std::pair<int, int>(*(itr), classID));
                    backMapping.insert(std::pair<int, int>(classID, *(itr)));
                    classID++;
                }
            }
            
            rsgis::math::MaximumLikelihood *mlStruct = new rsgis::math::MaximumLikelihood();
            mlStruct->nclasses = classes.size();
            mlStruct->classes = new int[mlStruct->nclasses];
            mlStruct->d = inColumns.size();
            
            unsigned int idx = 0;
            std::vector<std::string> outColPostNames;
            std::vector<std::string> outColPriorNames;
            rsgis::math::RSGISMathsUtils mathUtils;
            std::string colName = "";
            for(std::set<int>::iterator iterClasses = classes.begin(); iterClasses != classes.end(); ++iterClasses)
            {
                std::cout << "Class " << *iterClasses << " mapped to " << forwardMapping[*iterClasses] << std::endl;
                mlStruct->classes[idx] = *iterClasses;
                colName = outClassCol + std::string("Po_") + mathUtils.inttostring(*iterClasses);
                outColPostNames.push_back(colName);
                colName = outClassCol + std::string("Pr_") + mathUtils.inttostring(*iterClasses);
                outColPriorNames.push_back(colName);
                ++idx;
            }
            
            bool *foundOutPostIdx = new bool[mlStruct->nclasses];
            int *colOutPostIdxs = new int[mlStruct->nclasses];
            for(size_t i = 0; i < mlStruct->nclasses; ++i)
            {
                foundOutPostIdx[i] = false;
                colOutPostIdxs[i] = 0;
            }
            
            bool *foundOutPriorIdx = new bool[mlStruct->nclasses];
            int *colOutPriorIdxs = new int[mlStruct->nclasses];
            for(size_t i = 0; i < mlStruct->nclasses; ++i)
            {
                foundOutPriorIdx[i] = false;
                colOutPriorIdxs[i] = 0;
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
                    else if(!foundOutPriorIdx[j] && (std::string(attTable->GetNameOfCol(i)) == outColPriorNames.at(j)))
                    {
                        colOutPriorIdxs[j] = i;
                        foundOutPriorIdx[j] = true;
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
                
                if(!foundOutPriorIdx[j])
                {
                    attTable->CreateColumn(outColPriorNames.at(j).c_str(), GFT_Real, GFU_Generic);
                    colOutPriorIdxs[j] = numColumns++;
                }
            }
            delete[] foundOutPostIdx;
            delete[] foundOutPriorIdx;
            
            std::cout << "Training the ML classifier\n";
            int *mlClasses = new int[numTrainingSamples];
            double **samples = new double*[numTrainingSamples];
            idx = 0;
            for(size_t i = 1; i < numRows; ++i)
            {
                classID = attTable->GetValueAsInt(i, inClassColIdx);
                classID = forwardMapping[classID]; // do lookup into contiguous classes
                if((attTable->GetValueAsInt(i, trainingSelectColIdx) == 1) & (classID > 0))
                {
                    mlClasses[idx] = classID;
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
                    // Find local priors...
                    this->getLocalPriors(mlStruct, attTable, i, trainingSelectColIdx, eastColIdx, northColIdx, inClassColIdx, forwardMapping, areaColIdx, searchRadius, allowZeroPriors, priorsMethod, weightA);
                    
                    for(size_t j = 0; j < inColumns.size(); ++j)
                    {
                        data[j] = attTable->GetValueAsDouble(i, colIdxs[j]);
                    }
                    
                    classID = mlObj.predict_ml(mlStruct, data, &posteriorProbs);
                    classID = backMapping[classID]; // convert back to our non contigous classes
                    
                    for(int j = 0; j < mlStruct->nclasses; ++j)
                    {
                        attTable->SetValue(i, colOutPostIdxs[j], posteriorProbs[j]);
                        attTable->SetValue(i, colOutPriorIdxs[j], mlStruct->priors[j]);
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
            delete[] colOutPriorIdxs;
            delete[] colIdxs;
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
    
    double RSGISMaxLikelihoodRATClassification::getEuclideanDistance(std::vector<double> *vals1, std::vector<double> *vals2)throw(rsgis::math::RSGISMathException)
    {
        double dist = 0;
        
        if(vals1->size() != vals2->size())
        {
            std::cout << "vals1->size() = " << vals1->size() << std::endl;
            std::cout << "vals2->size() = " << vals2->size() << std::endl;
            throw rsgis::math::RSGISMathException("Value vectors are different sizes.");
        }
        
        size_t numVals = vals1->size();
        for(size_t i = 0; i < numVals; ++i)
        {
            dist += pow((vals1->at(i)-vals2->at(i)), 2);
        }
        
        return sqrt(dist/((double)numVals));
    }
    
    void RSGISMaxLikelihoodRATClassification::getLocalPriors(rsgis::math::MaximumLikelihood *mlStruct, GDALRasterAttributeTable *attTable, size_t fid, int trainingSelectColIdx, int eastingsIdx, int northingsIdx, int classColIdx, std::map<int, int> &forwardMapping, int areaColIdx, float spatialRadius, bool allowZeroPriors, rsgismlpriors priorsMethod, float weightA)throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            std::vector<double> fidValsSpatial;
            fidValsSpatial.reserve(2);
            std::vector<double> cValsSpatial;
            cValsSpatial.reserve(2);
            
            fidValsSpatial.push_back(attTable->GetValueAsDouble(fid, eastingsIdx));
            fidValsSpatial.push_back(attTable->GetValueAsDouble(fid, northingsIdx));
            
            double *classCounts = new double[mlStruct->nclasses];
            for(unsigned int i = 0; i < mlStruct->nclasses; ++i)
            {
                classCounts[i] = 0;
            }
            
            // Find all segments within distance threshold
            size_t numRows = attTable->GetRowCount();
            int classVal = 0;
            double segArea = 0;
            double dist = 0;
            bool classFound = false;
            int classFoundIdx = 0;
            for(size_t n = 1; n < numRows; ++n)
            {
                if((n != fid) && (attTable->GetValueAsInt(n, trainingSelectColIdx) == 1))
                {
                    cValsSpatial.push_back(attTable->GetValueAsDouble(n, eastingsIdx));
                    cValsSpatial.push_back(attTable->GetValueAsDouble(n, northingsIdx));
                    
                    dist = this->getEuclideanDistance(&fidValsSpatial, &cValsSpatial);
                    if(dist < spatialRadius)
                    {
                        classVal = attTable->GetValueAsInt(n, classColIdx);
                        classVal = forwardMapping[classVal];
                        if(classVal > 0)
                        {
                            classFound = false;
                            classFoundIdx = 0;
                            for(unsigned int i = 0; i < mlStruct->nclasses; ++i)
                            {
                                if(classVal == mlStruct->classes[i])
                                {
                                    classFound = true;
                                    classFoundIdx = i;
                                    break;
                                }
                            }
                            
                            if(classFound)
                            {
                                segArea = attTable->GetValueAsDouble(n, areaColIdx);
                                if(priorsMethod == rsgis::rastergis::rsgis_area)
                                {
                                    classCounts[classFoundIdx] += segArea;
                                }
                                else if(priorsMethod == rsgis::rastergis::rsgis_weighted)
                                {
                                    /*
                                    std::cout << "weightA = " << weightA << std::endl;
                                    double tmp1 = dist/1000;
                                    std::cout << "tmp1 = " << tmp1 << std::endl;
                                    double tmp2 = 2;///(1.0/3.0);
                                    std::cout << "tmp2 = " << tmp2 << std::endl;
                                    double tmp3 = pow(tmp1, tmp2)/weightA;
                                    std::cout << "tmp3 = " << tmp3 << std::endl;
                                    double tmp4 = exp(tmp3*(-1));
                                    std::cout << "tmp4 = " << tmp4 << std::endl;
                                    double tmp5 = tmp4 * segArea;
                                    std::cout << "tmp5 = " << tmp5 << std::endl << std::endl;
                                    */
                                    classCounts[classFoundIdx] += exp((pow((dist/1000), 2)/weightA)*(-1)) * segArea;
                                }
                                
                            }
                            else
                            {
                                std::cout << "classVal = " << classVal << std::endl;
                                throw rsgis::RSGISAttributeTableException("Class found which wasn't in the classes list.");
                            }
                        }
                        
                    }
                    cValsSpatial.clear();
                }
            }
            
            double totalSegCount = 0;
            for(unsigned int i = 0; i < mlStruct->nclasses; ++i)
            {
                totalSegCount += classCounts[i];
            }
            
            for(unsigned int i = 0; i < mlStruct->nclasses; ++i)
            {
                mlStruct->priors[i] = classCounts[i]/totalSegCount;
            }
            delete[] classCounts;
            
            if(!allowZeroPriors)
            {
                float minVal = 0;
                bool first = true;
                bool foundZero = false;
                for(unsigned int i = 0; i < mlStruct->nclasses; ++i)
                {
                    if(mlStruct->priors[i] < 0.001)
                    {
                        foundZero = true;
                    }
                    else
                    {
                        if(first)
                        {
                            minVal = mlStruct->priors[i];
                            first = false;
                        }
                        else if(mlStruct->priors[i] < minVal)
                        {
                            minVal = mlStruct->priors[i];
                        }
                    }
                }
                
                if(foundZero)
                {
                    float minPrior = 0.0;
                    if(minVal > (1.0/((float)mlStruct->nclasses)))
                    {
                        minPrior = 1.0/((float)mlStruct->nclasses);
                    }
                    else
                    {
                        minPrior = minVal;
                    }
                    float sumPriors = 0;
                    
                    for(unsigned int i = 0; i < mlStruct->nclasses; ++i)
                    {
                        if(mlStruct->priors[i] < 0.001)
                        {
                            mlStruct->priors[i] = minPrior;
                        }
                        sumPriors += mlStruct->priors[i];
                    }
                    
                    for(unsigned int i = 0; i < mlStruct->nclasses; ++i)
                    {
                        mlStruct->priors[i] = mlStruct->priors[i]/sumPriors;
                    }
                }
            }
            
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISMaxLikelihoodRATClassification::~RSGISMaxLikelihoodRATClassification()
    {
        
    }
	
}}


