/*
 *  RSGISFindMeanDist2Neighbours.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 28/03/2012.
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

#include "RSGISFindMeanDist2Neighbours.h"

namespace rsgis{namespace rastergis{
    
   
    RSGISFindMeanDist2Neighbours::RSGISFindMeanDist2Neighbours()
    {
        
    }
    
    void RSGISFindMeanDist2Neighbours::findMeanEuclideanDist2Neighbours(RSGISAttributeTable *attTable, std::vector<std::vector<unsigned long > > *neighbours, std::vector<std::string> *attributeNames, std::string outMeanAttName, std::string outMaxAttName, std::string outMinAttName)throw(RSGISAttributeTableException)
    {
        try 
        {
            if(attTable->getSize() != neighbours->size())
            {
                throw RSGISAttributeTableException("The number of features in the attribute table and neighbours file do not match.");
            }
            
            if(attributeNames->size() == 0)
            {
                throw RSGISAttributeTableException("There must be at least one attribute name provided\n");
            }
            
            if(attTable->hasAttribute(outMeanAttName))
            {
                if(attTable->getDataType(outMeanAttName) != rsgis_float)
                {
                    std::string message = outMeanAttName + std::string(" field already exists and is not of type float and therefore cannot be used.");
                    throw RSGISAttributeTableException(message);
                }
            }
            else
            {
                attTable->addAttFloatField(outMeanAttName, 0);
            }
            unsigned int outMeanFieldIdx = attTable->getFieldIndex(outMeanAttName);
            
            if(attTable->hasAttribute(outMaxAttName))
            {
                if(attTable->getDataType(outMaxAttName) != rsgis_float)
                {
                    std::string message = outMaxAttName + std::string(" field already exists and is not of type float and therefore cannot be used.");
                    throw RSGISAttributeTableException(message);
                }
            }
            else
            {
                attTable->addAttFloatField(outMaxAttName, 0);
            }
            unsigned int outMaxFieldIdx = attTable->getFieldIndex(outMaxAttName);
            
            if(attTable->hasAttribute(outMinAttName))
            {
                if(attTable->getDataType(outMinAttName) != rsgis_float)
                {
                    std::string message = outMinAttName + std::string(" field already exists and is not of type float and therefore cannot be used.");
                    throw RSGISAttributeTableException(message);
                }
            }
            else
            {
                attTable->addAttFloatField(outMinAttName, 0);
            }
            unsigned int outMinFieldIdx = attTable->getFieldIndex(outMinAttName);
            
            
            unsigned int numTestAtts = attributeNames->size();
            RSGISAttributeDataType *attDataTypes = new RSGISAttributeDataType[numTestAtts];
            unsigned int *attIdxes = new unsigned int[numTestAtts];
            
            unsigned int counter = 0;
            for(std::vector<std::string>::iterator iterNames = attributeNames->begin(); iterNames != attributeNames->end(); ++iterNames)
            {
                attDataTypes[counter] = attTable->getDataType(*iterNames);
                attIdxes[counter] = attTable->getFieldIndex(*iterNames);
                
                if(!((attDataTypes[counter] == rsgis_float) | (attDataTypes[counter] == rsgis_int)))
                {
                    throw RSGISAttributeTableException("Attribute can only be of type int or float.");
                }
                
                ++counter;
            }
            
            RSGISFeature *cFeat = NULL;
            RSGISFeature *tFeat = NULL;
            double sqDist = 0;
            double dist = 0;
            double sumDist = 0;
            double minDist = 0;
            double maxDist = 0;
            bool first = true;
            
            for(unsigned long i = 0; i < attTable->getSize(); ++i)
            {
                cFeat = attTable->getFeature(i);
                if(neighbours->at(i).size() > 0)
                {
                    sumDist = 0;
                    first = true;
                    for(unsigned int j = 0; j < neighbours->at(i).size(); ++j)
                    {
                        tFeat = attTable->getFeature(neighbours->at(i)[j]);
                        sqDist = 0;
                        for(unsigned int k = 0; k < numTestAtts; ++k)
                        {
                            if(attDataTypes[k] == rsgis_float)
                            {
                                sqDist += pow((cFeat->floatFields->at(attIdxes[k]) - tFeat->floatFields->at(attIdxes[k])), 2.0);
                            }
                            else if(attDataTypes[k] == rsgis_int)
                            {
                                sqDist += pow(((double)(cFeat->intFields->at(attIdxes[k]) - tFeat->intFields->at(attIdxes[k]))), 2.0);
                            }
                        }
                        if(sqDist != 0)
                        {
                            dist = sqrt(sqDist/numTestAtts);
                            
                            if(first)
                            {
                                minDist = dist;
                                maxDist = dist;
                                first = false;
                            }
                            else if(dist < minDist)
                            {
                                minDist = dist;
                            }
                            else if(dist > maxDist)
                            {
                                maxDist = dist;
                            }
                            
                            sumDist += dist;
                        }
                        else
                        {
                            dist = 0;
                            if(first)
                            {
                                minDist = dist;
                                maxDist = dist;
                                first = false;
                            }
                            else if(dist < minDist)
                            {
                                minDist = dist;
                            }
                            else if(dist > maxDist)
                            {
                                maxDist = dist;
                            }
                        }
                    }
                    
                    cFeat->floatFields->at(outMeanFieldIdx) = sumDist/neighbours->at(i).size();
                    cFeat->floatFields->at(outMaxFieldIdx) = maxDist;
                    cFeat->floatFields->at(outMinFieldIdx) = minDist;
                }
                else
                {
                    cFeat->floatFields->at(outMeanFieldIdx) = 0;
                    cFeat->floatFields->at(outMaxFieldIdx) = 0;
                    cFeat->floatFields->at(outMinFieldIdx) = 0;
                }
            }
            
            delete[] attDataTypes;
            delete[] attIdxes;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    void RSGISFindMeanDist2Neighbours::findMeanEuclideanDist2Neighbours(RSGISAttributeTable *attTable, std::vector<std::string> *attributeNames, std::string outMeanAttName, std::string outMaxAttName, std::string outMinAttName)throw(RSGISAttributeTableException)
    {
        try 
        {
            if(attributeNames->size() == 0)
            {
                throw RSGISAttributeTableException("There must be at least one attribute name provided\n");
            }
            
            if(attTable->hasAttribute(outMeanAttName))
            {
                if(attTable->getDataType(outMeanAttName) != rsgis_float)
                {
                    std::string message = outMeanAttName + std::string(" field already exists and is not of type float and therefore cannot be used.");
                    throw RSGISAttributeTableException(message);
                }
            }
            else
            {
                attTable->addAttFloatField(outMeanAttName, 0);
            }
            unsigned int outMeanFieldIdx = attTable->getFieldIndex(outMeanAttName);
            
            if(attTable->hasAttribute(outMaxAttName))
            {
                if(attTable->getDataType(outMaxAttName) != rsgis_float)
                {
                    std::string message = outMaxAttName + std::string(" field already exists and is not of type float and therefore cannot be used.");
                    throw RSGISAttributeTableException(message);
                }
            }
            else
            {
                attTable->addAttFloatField(outMaxAttName, 0);
            }
            unsigned int outMaxFieldIdx = attTable->getFieldIndex(outMaxAttName);
            
            if(attTable->hasAttribute(outMinAttName))
            {
                if(attTable->getDataType(outMinAttName) != rsgis_float)
                {
                    std::string message = outMinAttName + std::string(" field already exists and is not of type float and therefore cannot be used.");
                    throw RSGISAttributeTableException(message);
                }
            }
            else
            {
                attTable->addAttFloatField(outMinAttName, 0);
            }
            unsigned int outMinFieldIdx = attTable->getFieldIndex(outMinAttName);
            
            
            unsigned int numTestAtts = attributeNames->size();
            RSGISAttributeDataType *attDataTypes = new RSGISAttributeDataType[numTestAtts];
            unsigned int *attIdxes = new unsigned int[numTestAtts];
            
            unsigned int counter = 0;
            for(std::vector<std::string>::iterator iterNames = attributeNames->begin(); iterNames != attributeNames->end(); ++iterNames)
            {
                attDataTypes[counter] = attTable->getDataType(*iterNames);
                attIdxes[counter] = attTable->getFieldIndex(*iterNames);
                
                if(!((attDataTypes[counter] == rsgis_float) | (attDataTypes[counter] == rsgis_int)))
                {
                    throw RSGISAttributeTableException("Attribute can only be of type int or float.");
                }
                
                ++counter;
            }
            
            RSGISFeature *cFeat = NULL;
            RSGISFeature *tFeat = NULL;
            double sqDist = 0;
            double dist = 0;
            double sumDist = 0;
            double minDist = 0;
            double maxDist = 0;
            bool first = true;
            
            for(unsigned long i = 0; i < attTable->getSize(); ++i)
            {
                cFeat = attTable->getFeature(i);
                if(cFeat->neighbours->size() > 0)
                {
                    sumDist = 0;
                    first = true;
                    for(unsigned int j = 0; j < cFeat->neighbours->size(); ++j)
                    {
                        tFeat = attTable->getFeature(cFeat->neighbours->at(j));
                        sqDist = 0;
                        for(unsigned int k = 0; k < numTestAtts; ++k)
                        {
                            if(attDataTypes[k] == rsgis_float)
                            {
                                sqDist += pow((cFeat->floatFields->at(attIdxes[k]) - tFeat->floatFields->at(attIdxes[k])), 2.0);
                            }
                            else if(attDataTypes[k] == rsgis_int)
                            {
                                sqDist += pow(((double)(cFeat->intFields->at(attIdxes[k]) - tFeat->intFields->at(attIdxes[k]))), 2.0);
                            }
                        }
                        if(sqDist != 0)
                        {
                            dist = sqrt(sqDist/numTestAtts);
                            
                            if(first)
                            {
                                minDist = dist;
                                maxDist = dist;
                                first = false;
                            }
                            else if(dist < minDist)
                            {
                                minDist = dist;
                            }
                            else if(dist > maxDist)
                            {
                                maxDist = dist;
                            }
                            
                            sumDist += dist;
                        }
                        else
                        {
                            dist = 0;
                            if(first)
                            {
                                minDist = dist;
                                maxDist = dist;
                                first = false;
                            }
                            else if(dist < minDist)
                            {
                                minDist = dist;
                            }
                            else if(dist > maxDist)
                            {
                                maxDist = dist;
                            }
                        }
                    }
                    
                    cFeat->floatFields->at(outMeanFieldIdx) = sumDist/cFeat->neighbours->size();
                    cFeat->floatFields->at(outMaxFieldIdx) = maxDist;
                    cFeat->floatFields->at(outMinFieldIdx) = minDist;
                }
                else
                {
                    cFeat->floatFields->at(outMeanFieldIdx) = 0;
                    cFeat->floatFields->at(outMaxFieldIdx) = 0;
                    cFeat->floatFields->at(outMinFieldIdx) = 0;
                }
            }
            
            delete[] attDataTypes;
            delete[] attIdxes;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    RSGISFindMeanDist2Neighbours::~RSGISFindMeanDist2Neighbours()
    {
        
    }
    
}}

