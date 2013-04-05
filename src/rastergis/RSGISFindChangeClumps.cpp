/*
 *  RSGISFindChangeClumps.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/04/2013.
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

#include "RSGISFindChangeClumps.h"

namespace rsgis{namespace rastergis{
    
    RSGISFindChangeClumps::RSGISFindChangeClumps()
    {
        
    }
    
    void RSGISFindChangeClumps::findChangeStdDevThreshold(GDALDataset *clumpsDataset, std::string classCol, std::string changeField, std::vector<std::string> *fields, std::vector<rsgis::rastergis::RSGISClassChangeFields*> *classChangeField)throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            RSGISRasterAttUtils attUtils;
            const GDALRasterAttributeTable *attTableTmp = clumpsDataset->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *attTable = NULL;
            if(attTableTmp != NULL)
            {
                attTable = new GDALRasterAttributeTable(*attTableTmp);
            }
            else
            {
                attTable = new GDALRasterAttributeTable();
            }
            
            // Make sure it is long enough and extend if required.
            unsigned int numRows = attTable->GetRowCount();
            
            unsigned int classColIdx = attUtils.findColumnIndex(attTable, classCol);
            unsigned int changeFieldIdx = attUtils.findColumnIndexOrCreate(attTable, changeField, GFT_Integer);
            
            unsigned int numFields = fields->size();
            unsigned int numClasses = classChangeField->size();
            unsigned int *fieldIdxs = new unsigned int[numFields];
            for(unsigned int i = 0; i < numFields; ++i)
            {
                fieldIdxs[i] = attUtils.findColumnIndex(attTable, fields->at(i));                
            }
            
            for(unsigned int i = 0; i < numClasses; ++i)
            {
                classChangeField->at(i)->means = new float[numFields];
                classChangeField->at(i)->stddev = new float[numFields];
                for(unsigned int n = 0; n < numFields; ++n)
                {
                    classChangeField->at(i)->means[n] = 0;
                    classChangeField->at(i)->stddev[n] = 0;
                }
                classChangeField->at(i)->count = 0;
            }
            
            std::cout << "Calculate mean values for the classes\n";
            std::string className = "";
            unsigned int classIdx = 0;
            bool foundClass = false;
            for(unsigned int i = 1; i < numRows; ++i)
            {
                className = attTable->GetValueAsString(i, classColIdx);
                foundClass = false;
                classIdx = 0;
                for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = classChangeField->begin(); iterClasses != classChangeField->end(); ++iterClasses)
                {
                    if(className == (*iterClasses)->name)
                    {
                        foundClass = true;
                        break;
                    }
                    ++classIdx;
                }
                
                if(foundClass)
                {                    
                    for(unsigned int n = 0; n < numFields; ++n)
                    {
                        classChangeField->at(classIdx)->means[n] += attTable->GetValueAsDouble(i, fieldIdxs[n]);
                    }
                    ++classChangeField->at(classIdx)->count;
                }
            }
            
            for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = classChangeField->begin(); iterClasses != classChangeField->end(); ++iterClasses)
            {
                for(unsigned int n = 0; n < numFields; ++n)
                {
                    (*iterClasses)->means[n] = (*iterClasses)->means[n] / (*iterClasses)->count;
                }
            }
            
            std::cout << "Calculate standard deviation values for the classes\n";
            for(unsigned int i = 1; i < numRows; ++i)
            {
                className = attTable->GetValueAsString(i, classColIdx);
                foundClass = false;
                classIdx = 0;
                for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = classChangeField->begin(); iterClasses != classChangeField->end(); ++iterClasses)
                {
                    if(className == (*iterClasses)->name)
                    {
                        foundClass = true;
                        break;
                    }
                    ++classIdx;
                }
                
                if(foundClass)
                {
                    for(unsigned int n = 0; n < numFields; ++n)
                    {
                        classChangeField->at(classIdx)->stddev[n] += pow(attTable->GetValueAsDouble(i, fieldIdxs[n]) - classChangeField->at(classIdx)->means[n], 2);
                    }
                }
            }
            
            
            for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = classChangeField->begin(); iterClasses != classChangeField->end(); ++iterClasses)
            {
                for(unsigned int n = 0; n < numFields; ++n)
                {
                    (*iterClasses)->stddev[n] = sqrt((*iterClasses)->stddev[n] / (*iterClasses)->count);
                }
            }
            
            float ***thresholds = new float**[numClasses];
            for(unsigned int i = 0; i < numClasses; ++i)
            {
                thresholds[i] = new float*[numFields];
                for(unsigned int j = 0; j < numFields; ++j)
                {
                    thresholds[i][j] = new float[2];
                    thresholds[i][j][0] = 0;
                    thresholds[i][j][1] = 0;
                }
            }
            
            std::cout << "Thresholds:\n";
            unsigned int idx = 0;
            for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = classChangeField->begin(); iterClasses != classChangeField->end(); ++iterClasses)
            {
                std::cout << (*iterClasses)->name << ": " << (*iterClasses)->outName << "  " << (*iterClasses)->threshold << std::endl;
                for(unsigned int n = 0; n < numFields; ++n)
                {
                    thresholds[idx][n][0] = ((*iterClasses)->means[n]-((*iterClasses)->stddev[n]*(*iterClasses)->threshold));
                    thresholds[idx][n][1] = ((*iterClasses)->means[n]+((*iterClasses)->stddev[n]*(*iterClasses)->threshold));
                    std::cout << "\t" << (*iterClasses)->means[n] << " - " << (*iterClasses)->stddev[n] << "\t(Thresholds " << thresholds[idx][n][0] << " -- " << thresholds[idx][n][1] << ")" << std::endl;
                }
                ++idx;
            }
            
            
            std::cout << "Identify the change units\n";
            bool withinClass = false;
            for(unsigned int i = 1; i < numRows; ++i)
            {
                className = attTable->GetValueAsString(i, classColIdx);
                foundClass = false;
                classIdx = 0;
                for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = classChangeField->begin(); iterClasses != classChangeField->end(); ++iterClasses)
                {
                    if(className == (*iterClasses)->name)
                    {
                        foundClass = true;
                        break;
                    }
                    ++classIdx;
                }
                
                if(foundClass)
                {
                    withinClass = true;
                    for(unsigned int n = 0; n < numFields; ++n)
                    {
                        if((attTable->GetValueAsDouble(i, fieldIdxs[n]) < thresholds[classIdx][n][0]) | (attTable->GetValueAsDouble(i, fieldIdxs[n]) > thresholds[classIdx][n][1]))
                        {
                            withinClass = false;
                            break;
                        }
                    }
                    if(!withinClass)
                    {
                        attTable->SetValue(i, changeFieldIdx, classChangeField->at(classIdx)->outName);
                    }
                    else
                    {
                        attTable->SetValue(i, changeFieldIdx, 0);
                    }
                }
            }
            
            clumpsDataset->GetRasterBand(1)->SetDefaultRAT(attTable);
            
            for(unsigned int i = 0; i < numClasses; ++i)
            {
                for(unsigned int j = 0; j < numFields; ++j)
                {
                    delete[] thresholds[i][j];
                }
                delete[] thresholds[i];
            }
            delete[] thresholds;
            
            delete attTable;
            delete[] fieldIdxs;
            std::cout << "Complete\n";
        }
        catch (rsgis::RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISFindChangeClumps::~RSGISFindChangeClumps()
    {
        
    }
    
}}
