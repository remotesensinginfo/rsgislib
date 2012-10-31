/*
 *  RSGISKNNATTMajorityClassifier.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/08/2012.
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

#include "RSGISKNNATTMajorityClassifier.h"

namespace rsgis{namespace rastergis{
	

    RSGISKNNATTMajorityClassifier::RSGISKNNATTMajorityClassifier()
    {
        
    }
    
    void RSGISKNNATTMajorityClassifier::applyKNNClassifier(GDALDataset *image, std::string inClassCol, std::string outClassCol, std::string trainingSelectCol, std::string eastingsCol, std::string northingsCol, std::string areaCol, std::string majWeightCol, std::vector<std::string> inColumns, unsigned int nFeatures, float distThreshold, float weightA, ClassMajorityMethod majMethod) throw(rsgis::RSGISAttributeTableException)
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
            int areaColIdx = 0;
            bool areaColFound = false;
            int majWeightColIdx = 0;
            bool majWeightColFound = false;
            int inClassColIdx = 0;
            bool inClassColFound = false;
            int outClassColIdx = 0;
            bool outClassColFound = false;
            int trainingSelectColIdx = 0;
            bool trainingSelectColFound = false;
            int eastColIdx = 0;
            bool eastColFound = false;
            int northColIdx = 0;
            bool northColFound = false;
            bool *foundIdx = new bool[inColumns.size()];
            int *colIdxs = new int[inColumns.size()];
            for(size_t i = 0; i < inColumns.size(); ++i)
            {
                foundIdx[i] = false;
                colIdxs[i] = 0;
            }
            
            for(int i = 0; i < numColumns; ++i)
            {
                if(!areaColFound && (std::string(attTable->GetNameOfCol(i)) == areaCol))
                {
                    areaColFound = true;
                    areaColIdx = i;
                }
                else if(!majWeightColFound && (std::string(attTable->GetNameOfCol(i)) == majWeightCol))
                {
                    majWeightColFound = true;
                    majWeightColIdx = i;
                }
                else if(!inClassColFound && (std::string(attTable->GetNameOfCol(i)) == inClassCol))
                {
                    inClassColFound = true;
                    inClassColIdx = i;
                }
                else if(!outClassColFound && (std::string(attTable->GetNameOfCol(i)) == outClassCol))
                {
                    outClassColFound = true;
                    outClassColIdx = i;
                }
                else if(!trainingSelectColFound && (std::string(attTable->GetNameOfCol(i)) == trainingSelectCol))
                {
                    trainingSelectColFound = true;
                    trainingSelectColIdx = i;
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
            
            if(!outClassColFound)
            {
                attTable->CreateColumn(outClassCol.c_str(), GFT_Integer, GFU_Generic);
                outClassColIdx = numColumns++;
            }
            
            if(!majWeightColFound)
            {
                attTable->CreateColumn(majWeightCol.c_str(), GFT_Real, GFU_Generic);
                majWeightColIdx = numColumns++;
            }
            
            if(!areaColFound)
            {
                throw rsgis::RSGISAttributeTableException("Could not find the area column.");
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
            
            for(size_t j = 0; j < inColumns.size(); ++j)
            {
                if(!foundIdx[j])
                {
                    std::string message = std::string("Column ") + inColumns.at(j) + std::string(" is not within the attribute table.");
                    throw rsgis::RSGISAttributeTableException(message);
                }
            }
            
            std::cout << "Iterating through columns to classify...\n";
            std::pair<int, double> selectedClass;
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
                
                if(majMethod == stdMajority)
                {
                    selectedClass = this->findMajorityClassStandard(attTable, i, inClassColIdx, trainingSelectColIdx, eastColIdx, northColIdx, colIdxs, inColumns.size(), nFeatures, distThreshold);
                }
                else if (majMethod == weightedMajority)
                {
                    selectedClass = this->findMajorityClassWeighted(attTable, i, inClassColIdx, trainingSelectColIdx, eastColIdx, northColIdx, areaColIdx, colIdxs, inColumns.size(), nFeatures, distThreshold, weightA);
                }
                else
                {
                    throw rsgis::RSGISAttributeTableException("Majority methods is not recognised.");
                }
                attTable->SetValue(i, outClassColIdx, selectedClass.first);
                attTable->SetValue(i, majWeightColIdx, selectedClass.second);
                //std::cout << "i = " << i << " = " << selectedClass << ":\t" << attTable->GetValueAsInt(i, inClassColIdx) << std::endl;
                
            }
            std::cout << ".Completed\n";
            
            
            image->GetRasterBand(1)->SetDefaultRAT(attTable);
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
    
    std::pair<int, double> RSGISKNNATTMajorityClassifier::findMajorityClassStandard(GDALRasterAttributeTable *attTable, size_t fid, int classIdx, int trainingSelectColIdx, int eastingsIdx, int northingsIdx, int *infoColIdxs, size_t numCols, unsigned int nFeatures, float distThreshold) throw(rsgis::RSGISAttributeTableException)
    {
        int classID = -1;
        size_t maxFreq = 0;
        
        try 
        {
            std::vector<double> *fidValsSpatial = new std::vector<double>();
            fidValsSpatial->reserve(2);
            std::vector<double> *fidValsInfo = new std::vector<double>();
            fidValsInfo->reserve(numCols);
            std::vector<double> *vals2 = new std::vector<double>();
            vals2->reserve(numCols);
            std::list<DistItem> topNItems;
            
            fidValsSpatial->push_back(attTable->GetValueAsDouble(fid, eastingsIdx));
            fidValsSpatial->push_back(attTable->GetValueAsDouble(fid, northingsIdx));
            
            for(size_t k = 0; k < numCols; ++k)
            {
                fidValsInfo->push_back(attTable->GetValueAsDouble(fid, infoColIdxs[k]));
            }
            
            // Perform KNN
            bool foundPos = false;
            size_t numRows = attTable->GetRowCount();
            int training = 0;
            double infoDist = 0;
            for(size_t n = 1; n < numRows; ++n)
            {
                if(n != fid)
                {
                    training = attTable->GetValueAsInt(n, trainingSelectColIdx);
                    vals2->push_back(attTable->GetValueAsDouble(n, eastingsIdx));
                    vals2->push_back(attTable->GetValueAsDouble(n, northingsIdx));
                    
                    if((training == 1) && (this->getEuclideanDistance(fidValsSpatial, vals2) < distThreshold))
                    {
                        vals2->clear();
                        for(size_t m = 0; m < numCols; ++m)
                        {
                            vals2->push_back(attTable->GetValueAsDouble(n, infoColIdxs[m]));
                        }
                        infoDist = this->getEuclideanDistance(fidValsInfo, vals2);
                        
                        foundPos = false;
                        for(std::list<DistItem>::iterator iterItems = topNItems.begin(); iterItems != topNItems.end(); ++iterItems)
                        {
                            if(infoDist < (*iterItems).dist)
                            {
                                topNItems.insert(iterItems, DistItem(n, infoDist));
                                foundPos = true;
                                break;
                            }
                        }
                        
                        if(!foundPos & (topNItems.size() < nFeatures))
                        {
                            topNItems.push_back(DistItem(n, infoDist));
                        }
                        
                        if(topNItems.size() > nFeatures)
                        {
                            topNItems.pop_back();
                        }
                        
                    }
                    vals2->clear();
                }
            }
            
            delete fidValsSpatial;
            delete fidValsInfo;
            delete vals2;
            
            // Find the majority class ID.
            std::list<size_t> classIDs;
            for(std::list<DistItem>::iterator iterItems = topNItems.begin(); iterItems != topNItems.end(); ++iterItems)
            {
                classIDs.push_back(attTable->GetValueAsInt((*iterItems).fid, classIdx));
            }
            classIDs.sort();
                        
            size_t maxVal = classIDs.back()+1;
            
            size_t *freq = new size_t[maxVal];
            for(size_t i = 0; i < maxVal; ++i)
            {
                freq[i] = 0;
            }
            for(std::list<size_t>::iterator iterItems = classIDs.begin(); iterItems != classIDs.end(); ++iterItems)
            {
                ++freq[*iterItems];
            }
            
            bool first = true;
            for(size_t i = 0; i < maxVal; ++i)
            {
                if(first)
                {
                    if(freq[i] > 0)
                    {
                        maxFreq = freq[i];
                        classID = i;
                        first = false;
                    }
                }
                else if(freq[i] > maxFreq)
                {
                    maxFreq = freq[i];
                    classID = i;
                }
            }
            delete[] freq;
            
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        
        return std::pair<int, double>(classID, ((double)maxFreq));
    }
    
    std::pair<int, double> RSGISKNNATTMajorityClassifier::findMajorityClassWeighted(GDALRasterAttributeTable *attTable, size_t fid, int classIdx, int trainingSelectColIdx, int eastingsIdx, int northingsIdx, int areaIdx, int *infoColIdxs, size_t numCols, unsigned int nFeatures, float distThreshold, float weightA) throw(rsgis::RSGISAttributeTableException)
    {
        int classID = -1;
        double maxFreq = 0;
        
        try 
        {
            std::vector<double> *fidValsSpatial = new std::vector<double>();
            fidValsSpatial->reserve(2);
            std::vector<double> *fidValsInfo = new std::vector<double>();
            fidValsInfo->reserve(numCols);
            std::vector<double> *vals2 = new std::vector<double>();
            vals2->reserve(numCols);
            std::list<DistItem> topNItems;
            
            fidValsSpatial->push_back(attTable->GetValueAsDouble(fid, eastingsIdx));
            fidValsSpatial->push_back(attTable->GetValueAsDouble(fid, northingsIdx));
            
            for(size_t k = 0; k < numCols; ++k)
            {
                fidValsInfo->push_back(attTable->GetValueAsDouble(fid, infoColIdxs[k]));
            }
            
            // Perform Weighted KNN
            bool foundPos = false;
            size_t numRows = attTable->GetRowCount();
            int training = 0;
            double infoDist = 0;
            double eucSpatDist = 0;
            for(size_t n = 1; n < numRows; ++n)
            {
                if(n != fid)
                {
                    training = attTable->GetValueAsInt(n, trainingSelectColIdx);
                    vals2->push_back(attTable->GetValueAsDouble(n, eastingsIdx));
                    vals2->push_back(attTable->GetValueAsDouble(n, northingsIdx));
                    eucSpatDist = this->getEuclideanDistance(fidValsSpatial, vals2);
                    
                    if((training == 1) & (eucSpatDist < distThreshold))
                    {
                        vals2->clear();
                        for(size_t m = 0; m < numCols; ++m)
                        {
                            vals2->push_back(attTable->GetValueAsDouble(n, infoColIdxs[m]));
                        }
                        infoDist = this->getEuclideanDistance(fidValsInfo, vals2);
                        
                        foundPos = false;
                        for(std::list<DistItem>::iterator iterItems = topNItems.begin(); iterItems != topNItems.end(); ++iterItems)
                        {
                            if(infoDist < (*iterItems).dist)
                            {
                                topNItems.insert(iterItems, DistItem(n, infoDist));
                                foundPos = true;
                                break;
                            }
                        }
                        
                        if(!foundPos & (topNItems.size() < nFeatures))
                        {
                            topNItems.push_back(DistItem(n, infoDist));
                        }
                        
                        if(topNItems.size() > nFeatures)
                        {
                            topNItems.pop_back();
                        }
                        
                    }
                    vals2->clear();
                }
            }
                        
            // Find the majority class ID.
            std::list<std::pair<size_t, size_t> > classIDs;
            size_t tmpClassID = 0;
            size_t maxClassIDVal = 0;
            bool first = true;
            for(std::list<DistItem>::iterator iterItems = topNItems.begin(); iterItems != topNItems.end(); ++iterItems)
            {
                tmpClassID = attTable->GetValueAsInt((*iterItems).fid, classIdx);
                classIDs.push_back(std::pair<size_t, size_t>((*iterItems).fid, tmpClassID));
                if(first)
                {
                    maxClassIDVal = tmpClassID;
                    first = false;
                }
                else if(tmpClassID > maxClassIDVal)
                {
                    maxClassIDVal = tmpClassID;
                }
            }
            
            size_t maxVal = maxClassIDVal+1;
            //std::cout << "maxVal = " << maxVal << std::endl;
            double area = 0;
            double *freq = new double[maxVal];
            for(size_t i = 0; i < maxVal; ++i)
            {
                freq[i] = 0;
            }
            for(std::list<std::pair<size_t, size_t> >::iterator iterItems = classIDs.begin(); iterItems != classIDs.end(); ++iterItems)
            {
                vals2->push_back(attTable->GetValueAsDouble((*iterItems).first, eastingsIdx));
                vals2->push_back(attTable->GetValueAsDouble((*iterItems).first, northingsIdx));
                
                eucSpatDist = this->getEuclideanDistance(fidValsSpatial, vals2)/1000;
                area = attTable->GetValueAsDouble((*iterItems).first, areaIdx);
                
                freq[(*iterItems).second] += this->getWeightedDistance(eucSpatDist, area, weightA);
                
                vals2->clear();
            }
            
            delete fidValsSpatial;
            delete fidValsInfo;
            delete vals2;
            
            
            first = true;
            for(size_t i = 0; i < maxVal; ++i)
            {
                //std::cout << i << "(" << freq[i] << ") " << std::flush;
                if(first)
                {
                    if(freq[i] > 0)
                    {
                        maxFreq = freq[i];
                        classID = i;
                        first = false;
                    }
                }
                else if(freq[i] > maxFreq)
                {
                    maxFreq = freq[i];
                    classID = i;
                }
            }
            delete[] freq;
            //std::cout << std::endl;
            
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        
        return std::pair<int, double>(classID, maxFreq);
    }
    
    double RSGISKNNATTMajorityClassifier::getEuclideanDistance(std::vector<double> *vals1, std::vector<double> *vals2)throw(rsgis::math::RSGISMathException)
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
    
    double RSGISKNNATTMajorityClassifier::getWeightedDistance(double dist, double area, float weightA)
    {
        /*
        double tmp1 = dist;
        std::cout << "tmp1 = " << tmp1 << std::endl;
        double tmp2 = 2;///(1.0/3.0);
        std::cout << "tmp2 = " << tmp2 << std::endl;
        double tmp3 = pow(tmp1, tmp2)/3;
        std::cout << "tmp3 = " << tmp3 << std::endl;
        double tmp4 = exp(tmp3*(-1));
        std::cout << "tmp4 = " << tmp4 << std::endl;
        double tmp5 = tmp4 * area;
        std::cout << "tmp5 = " << tmp5 << std::endl << std::endl;
        return tmp5;
        */
        
        //std::cout << "dist = " << dist << ":\t" << exp((pow(dist, 2)/3)*(-1)) * area << std::endl;
        
        return exp((pow(dist, 2)/weightA)*(-1)) * area;
    }
    
    
    RSGISKNNATTMajorityClassifier::~RSGISKNNATTMajorityClassifier()
    {
        
    }
	
}}





