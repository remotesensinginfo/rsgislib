/*
 *  RSGISClumpRegionGrowing.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/08/2014.
 *  Copyright 2014 RSGISLib.
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

#include "RSGISClumpRegionGrowing.h"

namespace rsgis{namespace rastergis{
    
    RSGISClumpRegionGrowing::RSGISClumpRegionGrowing()
    {
        
    }
    
    void RSGISClumpRegionGrowing::growClassRegion(GDALDataset *inputClumps, std::string classColumn, std::string classVal, int maxIter, unsigned int ratBand, std::string xmlBlock)throw(RSGISAttributeTableException)
    {
        try
        {
            RSGISRasterAttUtils attUtils;
            rsgis::math::RSGISMathsUtils mathUtils;
            
            if(ratBand == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT Band must be greater than zero.");
            }
            if(ratBand > inputClumps->GetRasterCount())
            {
                throw rsgis::RSGISAttributeTableException("RAT Band is larger than the number of bands within the image.");
            }
            
            GDALRasterAttributeTable *rat = inputClumps->GetRasterBand(ratBand)->GetDefaultRAT();
            
            size_t numRows = rat->GetRowCount();
            if(numRows == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT has no rows, i.e., it is empty!");
            }
            
            std::vector<double*> *ratCols = new std::vector<double*>();
            
            rsgis::rastergis::RSGISRATLogicXMLParse parseLogicXMLObj;
            std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*> *colIdxes = new std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>();
            rsgis::math::RSGISLogicExpression* exp = parseLogicXMLObj.parseLogicXML(xmlBlock, colIdxes);
            
            size_t colLenTmp = 0;
            for(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>::iterator iterColIdx = colIdxes->begin(); iterColIdx != colIdxes->end(); ++iterColIdx)
            {
                if((*iterColIdx)->useThreshold)
                {
                    (*iterColIdx)->col1Idx = attUtils.findColumnIndex(rat, (*iterColIdx)->column1Name);
                    std::cout << (*iterColIdx)->column1Name << " = " << (*iterColIdx)->col1Idx << std::endl;
                    ratCols->push_back(attUtils.readDoubleColumn(rat, (*iterColIdx)->column1Name, &colLenTmp));
                    if(colLenTmp != numRows)
                    {
                        throw rsgis::RSGISAttributeTableException("The returned column array is not the same length as the RAT.");
                    }
                    (*iterColIdx)->col1Idx = ratCols->size()-1;
                }
                else if((*iterColIdx)->singleCol)
                {
                    throw rsgis::RSGISAttributeTableException("A single column expresion without threshold cannot be evaluted for the region going - this should be the neighbour criteria.");
                }
                else
                {
                    (*iterColIdx)->col1Idx = attUtils.findColumnIndex(rat, (*iterColIdx)->column1Name);
                    std::cout << (*iterColIdx)->column1Name << " = " << (*iterColIdx)->col1Idx << std::endl;
                    ratCols->push_back(attUtils.readDoubleColumn(rat, (*iterColIdx)->column1Name, &colLenTmp));
                    if(colLenTmp != numRows)
                    {
                        throw rsgis::RSGISAttributeTableException("The returned column array is not the same length as the RAT.");
                    }
                    (*iterColIdx)->col1Idx = ratCols->size()-1;
                    (*iterColIdx)->col2Idx = attUtils.findColumnIndex(rat, (*iterColIdx)->column2Name);
                    std::cout << (*iterColIdx)->column2Name << " = " << (*iterColIdx)->col2Idx << std::endl;
                    ratCols->push_back(attUtils.readDoubleColumn(rat, (*iterColIdx)->column2Name, &colLenTmp));
                    if(colLenTmp != numRows)
                    {
                        throw rsgis::RSGISAttributeTableException("The returned column array is not the same length as the RAT.");
                    }
                    (*iterColIdx)->col2Idx = ratCols->size()-1;
                }
            }
            
            std::vector<std::vector<size_t>* > *neighbours = attUtils.getRATNeighbours(inputClumps, ratBand);
            
            if(numRows != neighbours->size())
            {
                for(std::vector<std::vector<size_t>* >::iterator iterNeigh = neighbours->begin(); iterNeigh != neighbours->end(); ++iterNeigh)
                {
                    delete *iterNeigh;
                }
                delete neighbours;
                
                throw rsgis::RSGISAttributeTableException("RAT size is different to the number of neighbours retrieved.");
            }
            
            size_t colLen = 0;
            std::string *classColVals = attUtils.readStrColumnStdStr(rat, classColumn, &colLen);
            std::string *classColValsTmp = new std::string[colLen];
            for(size_t i = 0; i < numRows; ++i)
            {
                classColValsTmp[i] = "";
            }
            
            if(colLen != numRows)
            {
                for(std::vector<std::vector<size_t>* >::iterator iterNeigh = neighbours->begin(); iterNeigh != neighbours->end(); ++iterNeigh)
                {
                    delete *iterNeigh;
                }
                delete neighbours;
                delete[] classColVals;
                throw rsgis::RSGISAttributeTableException("The column does not have enough values ");
            }
            
            bool changeFound = true;
            unsigned int numChangeFeats = 0;
            bool maxIterDef = false;
            if(maxIter >= 0)
            {
                maxIterDef = true;
            }
            int numIter = 0;
            int feedback = numRows/10.0;
            int feedbackCounter = 0;
            while(changeFound)
            {
                changeFound = false;
                std::cout << "Started " << std::flush;
                feedbackCounter = 0;
                numChangeFeats = 0;
                for(size_t i = 0; i < numRows; ++i)
                {
                    if((feedback != 0) && ((i % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    if(classColVals[i] == classVal)
                    {
                        // Check the neighbours...
                        std::vector<size_t> *clumpNeigh = neighbours->at(i);
                        for(std::vector<size_t>::iterator iterNeigh = clumpNeigh->begin(); iterNeigh != clumpNeigh->end(); ++iterNeigh)
                        {
                            if(classColVals[*iterNeigh] != classVal)
                            {
                                // Check if condition is met, if met then 'grow' and set change flag...
                                for(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>::iterator iterColIdx = colIdxes->begin(); iterColIdx != colIdxes->end(); ++iterColIdx)
                                {
                                    if((*iterColIdx)->useThreshold)
                                    {
                                        (*iterColIdx)->col1Val = ratCols->at((*iterColIdx)->col1Idx)[*iterNeigh];
                                    }
                                    else
                                    {
                                        (*iterColIdx)->col1Val = ratCols->at((*iterColIdx)->col1Idx)[*iterNeigh];
                                        (*iterColIdx)->col2Val = ratCols->at((*iterColIdx)->col2Idx)[*iterNeigh];
                                    }
                                }
                                
                                if(exp->evaluate())
                                {
                                    classColValsTmp[*iterNeigh] = classVal;
                                    changeFound = true;
                                    ++numChangeFeats;
                                }
                            }
                        }
                    }
                }
                std::cout << ".Completed\n";
                std::cout << "Iteration " << numIter << " changed " << numChangeFeats << " features\n";

                numChangeFeats = 0;
                //unsigned int changeCountTmp = 0;
                // Copy class names to 'main' array...
                for(size_t i = 0; i < numRows; ++i)
                {
                    if(classColValsTmp[i] == classVal)
                    {
                        classColVals[i] = classColValsTmp[i];
                    }
                    classColValsTmp[i] = "";
                }
                
                if(maxIterDef && (numIter > maxIter))
                {
                    break;
                }
                ++numIter;
            }
            std::cout << "Writing classification column\n";
            
            attUtils.writeStrColumn(rat, classColumn, classColVals, colLen);
            
            std::cout << "Tidying up...\n";
            
            delete[] classColVals;
            delete[] classColValsTmp;
            for(std::vector<std::vector<size_t>* >::iterator iterNeigh = neighbours->begin(); iterNeigh != neighbours->end(); ++iterNeigh)
            {
                delete *iterNeigh;
            }
            delete neighbours;
            for(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>::iterator iterColIdx = colIdxes->begin(); iterColIdx != colIdxes->end(); ++iterColIdx)
            {
                delete *iterColIdx;
            }
            delete colIdxes;
            delete exp;
            for(std::vector<double*>::iterator iterCols = ratCols->begin(); iterCols != ratCols->end(); ++iterCols)
            {
                delete[] *iterCols;
            }
            delete ratCols;
            std::cout << "Completed.\n";
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    void RSGISClumpRegionGrowing::growClassRegionNeighCriteria(GDALDataset *inputClumps, std::string classColumn, std::string classVal, int maxIter, unsigned int ratBand, std::string xmlBlockCriteria, std::string xmlBlockNeighCriteria)throw(RSGISAttributeTableException)
    {
        try
        {
            RSGISRasterAttUtils attUtils;
            rsgis::math::RSGISMathsUtils mathUtils;
            
            if(ratBand == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT Band must be greater than zero.");
            }
            if(ratBand > inputClumps->GetRasterCount())
            {
                throw rsgis::RSGISAttributeTableException("RAT Band is larger than the number of bands within the image.");
            }
            
            GDALRasterAttributeTable *rat = inputClumps->GetRasterBand(ratBand)->GetDefaultRAT();
            
            size_t numRows = rat->GetRowCount();
            if(numRows == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT has no rows, i.e., it is empty!");
            }
            
            std::vector<double*> *ratCols = new std::vector<double*>();
            
            rsgis::rastergis::RSGISRATLogicXMLParse parseLogicXMLObj;
            std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*> *colIdxesCritExp = new std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>();
            rsgis::math::RSGISLogicExpression* expCrit = parseLogicXMLObj.parseLogicXML(xmlBlockCriteria, colIdxesCritExp);
            
            std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*> *colIdxesNeighExp = new std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>();
            rsgis::math::RSGISLogicExpression* expNeigh = parseLogicXMLObj.parseLogicXML(xmlBlockNeighCriteria, colIdxesNeighExp);
            
            size_t colLenTmp = 0;
            for(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>::iterator iterColIdx = colIdxesCritExp->begin(); iterColIdx != colIdxesCritExp->end(); ++iterColIdx)
            {
                if((*iterColIdx)->useThreshold)
                {
                    (*iterColIdx)->col1Idx = attUtils.findColumnIndex(rat, (*iterColIdx)->column1Name);
                    std::cout << (*iterColIdx)->column1Name << " = " << (*iterColIdx)->col1Idx << std::endl;
                    ratCols->push_back(attUtils.readDoubleColumn(rat, (*iterColIdx)->column1Name, &colLenTmp));
                    if(colLenTmp != numRows)
                    {
                        throw rsgis::RSGISAttributeTableException("The returned column array is not the same length as the RAT.");
                    }
                    (*iterColIdx)->col1Idx = ratCols->size()-1;
                }
                else if((*iterColIdx)->singleCol)
                {
                    throw rsgis::RSGISAttributeTableException("A single column expresion without threshold cannot be evaluted for the region growing - this should be the neighbour criteria.");
                }
                else
                {
                    (*iterColIdx)->col1Idx = attUtils.findColumnIndex(rat, (*iterColIdx)->column1Name);
                    std::cout << (*iterColIdx)->column1Name << " = " << (*iterColIdx)->col1Idx << std::endl;
                    ratCols->push_back(attUtils.readDoubleColumn(rat, (*iterColIdx)->column1Name, &colLenTmp));
                    if(colLenTmp != numRows)
                    {
                        throw rsgis::RSGISAttributeTableException("The returned column array is not the same length as the RAT.");
                    }
                    (*iterColIdx)->col1Idx = ratCols->size()-1;
                    (*iterColIdx)->col2Idx = attUtils.findColumnIndex(rat, (*iterColIdx)->column2Name);
                    std::cout << (*iterColIdx)->column2Name << " = " << (*iterColIdx)->col2Idx << std::endl;
                    ratCols->push_back(attUtils.readDoubleColumn(rat, (*iterColIdx)->column2Name, &colLenTmp));
                    if(colLenTmp != numRows)
                    {
                        throw rsgis::RSGISAttributeTableException("The returned column array is not the same length as the RAT.");
                    }
                    (*iterColIdx)->col2Idx = ratCols->size()-1;
                }
            }
            
            colLenTmp = 0;
            for(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>::iterator iterColIdx = colIdxesNeighExp->begin(); iterColIdx != colIdxesNeighExp->end(); ++iterColIdx)
            {
                if((*iterColIdx)->useThreshold)
                {
                    throw rsgis::RSGISAttributeTableException("A single column expresion with threshold cannot be evaluted for comparison to neighbours.");
                }
                else if((*iterColIdx)->singleCol)
                {
                    (*iterColIdx)->col1Idx = attUtils.findColumnIndex(rat, (*iterColIdx)->column1Name);
                    std::cout << (*iterColIdx)->column1Name << " = " << (*iterColIdx)->col1Idx << std::endl;
                    ratCols->push_back(attUtils.readDoubleColumn(rat, (*iterColIdx)->column1Name, &colLenTmp));
                    if(colLenTmp != numRows)
                    {
                        throw rsgis::RSGISAttributeTableException("The returned column array is not the same length as the RAT.");
                    }
                    (*iterColIdx)->col1Idx = ratCols->size()-1;
                }
                else
                {
                    throw rsgis::RSGISAttributeTableException("A multiple column expresion without threshold cannot be evaluted for comparison to neighbours.");
                }
            }
            
            
            std::vector<std::vector<size_t>* > *neighbours = attUtils.getRATNeighbours(inputClumps, ratBand);
            
            if(numRows != neighbours->size())
            {
                for(std::vector<std::vector<size_t>* >::iterator iterNeigh = neighbours->begin(); iterNeigh != neighbours->end(); ++iterNeigh)
                {
                    delete *iterNeigh;
                }
                delete neighbours;
                
                throw rsgis::RSGISAttributeTableException("RAT size is different to the number of neighbours retrieved.");
            }
            
            size_t colLen = 0;
            std::string *classColVals = attUtils.readStrColumnStdStr(rat, classColumn, &colLen);
            std::string *classColValsTmp = new std::string[colLen];
            for(size_t i = 0; i < numRows; ++i)
            {
                classColValsTmp[i] = "";
            }
            
            if(colLen != numRows)
            {
                for(std::vector<std::vector<size_t>* >::iterator iterNeigh = neighbours->begin(); iterNeigh != neighbours->end(); ++iterNeigh)
                {
                    delete *iterNeigh;
                }
                delete neighbours;
                delete[] classColVals;
                throw rsgis::RSGISAttributeTableException("The column does not have enough values ");
            }
            
            bool changeFound = true;
            unsigned int numChangeFeats = 0;
            bool maxIterDef = false;
            if(maxIter >= 0)
            {
                maxIterDef = true;
            }
            int numIter = 0;
            int feedback = numRows/10.0;
            int feedbackCounter = 0;
            while(changeFound)
            {
                changeFound = false;
                std::cout << "Started " << std::flush;
                feedbackCounter = 0;
                numChangeFeats = 0;
                for(size_t i = 0; i < numRows; ++i)
                {
                    if((feedback != 0) && ((i % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    if(classColVals[i] == classVal)
                    {
                        // Check the neighbours...
                        std::vector<size_t> *clumpNeigh = neighbours->at(i);
                        for(std::vector<size_t>::iterator iterNeigh = clumpNeigh->begin(); iterNeigh != clumpNeigh->end(); ++iterNeigh)
                        {
                            if(classColVals[*iterNeigh] != classVal)
                            {
                                // ALSO NEEDS TO MEET THE SECOND CRITERIA COMPARING CURRENT OBJECT TO NEIGHBOUR...
                                for(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>::iterator iterColIdx = colIdxesNeighExp->begin(); iterColIdx != colIdxesNeighExp->end(); ++iterColIdx)
                                {
                                    if(((*iterColIdx)->singleCol) & (!(*iterColIdx)->thresholdVal))
                                    {
                                        (*iterColIdx)->col1Val = ratCols->at((*iterColIdx)->col1Idx)[*iterNeigh];
                                        (*iterColIdx)->col2Val = ratCols->at((*iterColIdx)->col1Idx)[i];
                                    }
                                    else
                                    {
                                        throw rsgis::RSGISAttributeTableException("Could not populate expression values as only single columns without threshold supported for neighbours");
                                    }
                                }
                                
                                if(expNeigh->evaluate())
                                {
                                    // Check if condition is met, if met then 'grow' and set change flag...
                                    for(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>::iterator iterColIdx = colIdxesCritExp->begin(); iterColIdx != colIdxesCritExp->end(); ++iterColIdx)
                                    {
                                        if((*iterColIdx)->useThreshold)
                                        {
                                            (*iterColIdx)->col1Val = ratCols->at((*iterColIdx)->col1Idx)[*iterNeigh];
                                        }
                                        else
                                        {
                                            (*iterColIdx)->col1Val = ratCols->at((*iterColIdx)->col1Idx)[*iterNeigh];
                                            (*iterColIdx)->col2Val = ratCols->at((*iterColIdx)->col2Idx)[*iterNeigh];
                                        }
                                    }
                                    
                                    if(expCrit->evaluate())
                                    {
                                        classColValsTmp[*iterNeigh] = classVal;
                                        changeFound = true;
                                        ++numChangeFeats;
                                    }
                                }
                            }
                        }
                    }
                }
                std::cout << ".Completed\n";
                std::cout << "Iteration " << numIter << " changed " << numChangeFeats << " features\n";
                
                numChangeFeats = 0;
                //unsigned int changeCountTmp = 0;
                // Copy class names to 'main' array...
                for(size_t i = 0; i < numRows; ++i)
                {
                    if(classColValsTmp[i] == classVal)
                    {
                        classColVals[i] = classColValsTmp[i];
                    }
                    classColValsTmp[i] = "";
                }
                
                if(maxIterDef && (numIter > maxIter))
                {
                    break;
                }
                ++numIter;
            }
            std::cout << "Writing classification column\n";
            
            attUtils.writeStrColumn(rat, classColumn, classColVals, colLen);
            
            std::cout << "Tidying up...\n";
            
            delete[] classColVals;
            delete[] classColValsTmp;
            for(std::vector<std::vector<size_t>* >::iterator iterNeigh = neighbours->begin(); iterNeigh != neighbours->end(); ++iterNeigh)
            {
                delete *iterNeigh;
            }
            delete neighbours;
            for(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>::iterator iterColIdx = colIdxesCritExp->begin(); iterColIdx != colIdxesCritExp->end(); ++iterColIdx)
            {
                delete *iterColIdx;
            }
            delete colIdxesCritExp;
            delete expCrit;
            for(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>::iterator iterColIdx = colIdxesNeighExp->begin(); iterColIdx != colIdxesNeighExp->end(); ++iterColIdx)
            {
                delete *iterColIdx;
            }
            delete colIdxesNeighExp;
            delete expNeigh;
            for(std::vector<double*>::iterator iterCols = ratCols->begin(); iterCols != ratCols->end(); ++iterCols)
            {
                delete[] *iterCols;
            }
            delete ratCols;
            std::cout << "Completed.\n";
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISClumpRegionGrowing::~RSGISClumpRegionGrowing()
    {
        
    }
    
    
}}
