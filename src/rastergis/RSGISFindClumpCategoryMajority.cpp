/*
 *  RSGISFindClumpCategoryMajority.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/03/2012.
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

#include "RSGISFindClumpCategoryMajority.h"

namespace rsgis{namespace rastergis{
    

    RSGISFindClumpCategoryMajority::RSGISFindClumpCategoryMajority()
    {
        
    }
    
    void RSGISFindClumpCategoryMajority::findMajorityCategory(GDALDataset *categoriesDataset, GDALDataset *clumpsDataset, RSGISAttributeTable *attTable, string areaField, string majorityRatioField, string majorityCatField)throw(RSGISAttributeTableException)
    {
        try
        {
            if(!attTable->hasAttribute(areaField))
            {
                throw RSGISAttributeTableException("The specified area field is not present.");
            }
            else if(attTable->getDataType(areaField) != rsgis_int)
            {
                throw RSGISAttributeTableException("Area field must be of type integer.");
            }
            unsigned int areaFieldIdx = attTable->getFieldIndex(areaField);
            
            if(!attTable->hasAttribute(majorityCatField))
            {
                attTable->addAttIntField(majorityCatField, 0);
            }
            else if(attTable->getDataType(majorityCatField) != rsgis_int)
            {
                throw RSGISAttributeTableException("Majority category field must be of type integer.");
            }
            unsigned int majorityCatFieldIdx = attTable->getFieldIndex(majorityCatField);
            
            if(!attTable->hasAttribute(majorityRatioField))
            {
                attTable->addAttFloatField(majorityRatioField, 0);
            }
            else if(attTable->getDataType(majorityRatioField) != rsgis_float)
            {
                throw RSGISAttributeTableException("Majority ratio field must be of type integer.");
            }
            unsigned int majorityRatioFieldIdx = attTable->getFieldIndex(majorityRatioField);
            
            unsigned long long attSize = attTable->getSize();
            
            vector<vector< pair<unsigned long, unsigned int> > > *attMajorityFields = new vector<vector< pair<unsigned long, unsigned int> > >();
            attMajorityFields->reserve(attSize);
            
            for(unsigned long long i = 0; i < attSize; ++i)
            {
                attMajorityFields->push_back(vector< pair<unsigned long, unsigned int> >());
            }
            
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumpsDataset;
            datasets[1] = categoriesDataset;
            
            RSGISFindClumpCategoryMajorityCalcImage *calcMajorities = new RSGISFindClumpCategoryMajorityCalcImage(0, attMajorityFields);
            RSGISCalcImage calcImage(calcMajorities);
            calcImage.calcImage(datasets, 2);
            delete calcMajorities;
            delete[] datasets;
            
            
            bool first = true;
            unsigned long maxCategory = 0;
            unsigned int maxCount = 0;
            RSGISFeature *feat = NULL;
            for(unsigned long long i = 0; i < attSize; ++i)
            {
                first = true;
                feat = attTable->getFeature(i);
                if(attMajorityFields->at(i).size() == 0)
                {
                    feat->intFields->at(majorityCatFieldIdx) = 0;
                    feat->floatFields->at(majorityRatioFieldIdx) = 0;
                }
                else
                {
                    for(vector< pair<unsigned long, unsigned int> >::iterator iterCats = attMajorityFields->at(i).begin(); iterCats != attMajorityFields->at(i).end(); ++iterCats)
                    {
                        if(first)
                        {
                            maxCategory = (*iterCats).first;
                            maxCount = (*iterCats).second;
                            first = false;
                        }
                        else if((*iterCats).second > maxCount)
                        {
                            maxCategory = (*iterCats).first;
                            maxCount = (*iterCats).second;
                        }
                    }
                    
                    feat->intFields->at(majorityCatFieldIdx) = maxCategory;
                    feat->floatFields->at(majorityRatioFieldIdx) = ((float)maxCount) / feat->intFields->at(areaFieldIdx);
                }                
            }            
            
            delete attMajorityFields;            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISImageCalcException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
        
    RSGISFindClumpCategoryMajority::~RSGISFindClumpCategoryMajority()
    {
        
    }
    
    
    
    
    
    
    RSGISFindClumpCategoryMajorityCalcImage::RSGISFindClumpCategoryMajorityCalcImage(int numberOutBands, vector<vector< pair<unsigned long, unsigned int> > > *attMajorityFields):RSGISCalcImageValue(numberOutBands)
    {
        this->attMajorityFields = attMajorityFields;
    }
        
    void RSGISFindClumpCategoryMajorityCalcImage::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        unsigned long category = 0;
        
        try
        {
            clumpIdx = lexical_cast<unsigned long>(bandValues[0]);
            category = lexical_cast<unsigned long>(bandValues[1]);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx != 0)
        {
            --clumpIdx; // Convert from image space to attribute space.
            
            bool foundCat = false;
            if(attMajorityFields->at(clumpIdx).size() > 0)
            {
                for(vector< pair<unsigned long, unsigned int> >::iterator iterCats = attMajorityFields->at(clumpIdx).begin(); iterCats != attMajorityFields->at(clumpIdx).end(); ++iterCats)
                {
                    if((*iterCats).first == category)
                    {
                        ++(*iterCats).second;
                        foundCat = true;
                        break;
                    }
                }
            }
            
            if(!foundCat)
            {
                attMajorityFields->at(clumpIdx).push_back(pair<unsigned long, unsigned int>(category, 1));
            }
        }
    }

    RSGISFindClumpCategoryMajorityCalcImage::~RSGISFindClumpCategoryMajorityCalcImage()
    {
        
    }
    
    
}}




