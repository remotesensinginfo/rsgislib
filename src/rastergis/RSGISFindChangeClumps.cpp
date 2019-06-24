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


    RSGISFindChangeClumpsStdDevThreshold::RSGISFindChangeClumpsStdDevThreshold(GDALDataset *clumpsDataset, std::string classCol, std::string changeField,
                                                                               std::vector<std::string> *fields, std::vector<rsgis::rastergis::RSGISClassChangeFields*> *classChangeField,
                                                                               unsigned int ratBand) : RSGISRATCalcValue()
    {
        this->classCol = classCol;
        this->fields = fields;
        this->classChangeField = classChangeField;

        RSGISRasterAttUtils attUtils;
        GDALRasterAttributeTable *attTableTmp = clumpsDataset->GetRasterBand(ratBand)->GetDefaultRAT();
        this->attTable = NULL;
        if(attTableTmp != NULL)
        {
            this->attTable = attTableTmp;
        }
        else
        {
            this->attTable = new GDALDefaultRasterAttributeTable();
        }

        // Get number of rows
        this->numRows = attTable->GetRowCount();

        // Get number of fields and classess
        this->numFields = fields->size();
        this->numClasses = classChangeField->size();

        // Get indexes
        this->classColIdx = attUtils.findColumnIndex(attTable, classCol);
        this->changeFieldIdx = attUtils.findColumnIndexOrCreate(attTable, changeField, GFT_Integer);

        // Get index for each field
        this->fieldIdxs = new unsigned int[this->numFields];
        for(unsigned int i = 0; i < this->numFields; ++i)
        {
            this->fieldIdxs[i] = attUtils.findColumnIndex(this->attTable, fields->at(i));
        }

        // Set up structure to hold statistics for each class
        for(unsigned int i = 0; i < this->numClasses; ++i)
        {
            this->classChangeField->at(i)->means = new float[this->numFields];
            this->classChangeField->at(i)->stddev = new float[this->numFields];
            for(unsigned int n = 0; n < this->numFields; ++n)
            {
                this->classChangeField->at(i)->means[n] = 0;
                this->classChangeField->at(i)->stddev[n] = 0;
            }
            this->classChangeField->at(i)->count = 0;
        }

        // Allocate array for thresholds
        this->thresholds = new float**[this->numClasses];
        for(unsigned int i = 0; i < this->numClasses; ++i)
        {
            this->thresholds[i] = new float*[this->numFields];
            for(unsigned int j = 0; j < this->numFields; ++j)
            {
                this->thresholds[i][j] = new float[2];
                this->thresholds[i][j][0] = 0;
                this->thresholds[i][j][1] = 0;
            }
        }

        // Get threasholds
        getThresholds();

    }

    void RSGISFindChangeClumpsStdDevThreshold::getThresholds()
    {
        RSGISRasterAttUtils attUtils;

        // Load classes column to memory
        std::string *classNamesCol = NULL;
        size_t colLen = 0;

        classNamesCol = attUtils.readStrColumnStdStr(attTable, this->classCol, &colLen);

        if(colLen != this->numRows)
        {
            RSGISAttributeTableException("Read less rows than in attribute table");
        }

        // Itterate through fields (only load one column to memory at a time)
        for(unsigned int n = 0; n < this->numFields; ++n)
        {
            // Load column to memory
            double *changeFieldCol = attUtils.readDoubleColumn(attTable, this->fields->at(n), &colLen);

            if(colLen != this->numRows)
            {
                RSGISAttributeTableException("Read less rows than in attribute table");
            }

            // Calculate mean
            for(unsigned int i = 0; i < this->numRows; ++i)
            {
                bool foundClass = false;
                unsigned int classIdx = 0;
                for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = this->classChangeField->begin(); iterClasses != this->classChangeField->end(); ++iterClasses)
                {
                    if(classNamesCol[i] == (*iterClasses)->name)
                    {
                        foundClass = true;
                        break;
                    }
                    ++classIdx;
                }

                if(foundClass)
                {
                    if(boost::math::isfinite(changeFieldCol[i]) && (changeFieldCol[i] != 0))
                    {
                        this->classChangeField->at(classIdx)->means[n] += changeFieldCol[i];
                        ++this->classChangeField->at(classIdx)->count;
                    }
                }
            }
            for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = this->classChangeField->begin(); iterClasses != this->classChangeField->end(); ++iterClasses)
            {
                (*iterClasses)->means[n] = (*iterClasses)->means[n] / (*iterClasses)->count;
            }
            // Calculate standard deviation
            for(unsigned int i = 1; i < this->numRows; ++i)
            {
                bool foundClass = false;
                unsigned int classIdx = 0;
                for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = this->classChangeField->begin(); iterClasses != this->classChangeField->end(); ++iterClasses)
                {
                    if(classNamesCol[i] == (*iterClasses)->name)
                    {
                        foundClass = true;
                        break;
                    }
                    ++classIdx;
                }

                if(foundClass)
                {
                    if(boost::math::isfinite(changeFieldCol[i]) && (changeFieldCol[i] != 0))
                    {
                        this->classChangeField->at(classIdx)->stddev[n] += pow(changeFieldCol[i] - classChangeField->at(classIdx)->means[n], 2);
                    }


                }
            }

            for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = this->classChangeField->begin(); iterClasses != this->classChangeField->end(); ++iterClasses)
            {
                (*iterClasses)->stddev[n] = sqrt((*iterClasses)->stddev[n] / (*iterClasses)->count);
            }
            delete[] changeFieldCol;
        }

        // Get thresholds
        unsigned int idx = 0;
        for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = this->classChangeField->begin(); iterClasses != this->classChangeField->end(); ++iterClasses)
        {
            std::cout << (*iterClasses)->name << ": " << (*iterClasses)->outName << "  " << (*iterClasses)->threshold << std::endl;
            for(unsigned int n = 0; n < this->numFields; ++n)
            {
                this->thresholds[idx][n][0] = ((*iterClasses)->means[n]-((*iterClasses)->stddev[n]*(*iterClasses)->threshold));
                this->thresholds[idx][n][1] = ((*iterClasses)->means[n]+((*iterClasses)->stddev[n]*(*iterClasses)->threshold));
                std::cout << "\t" << (*iterClasses)->means[n] << " - " << (*iterClasses)->stddev[n] << "\t(Thresholds " << thresholds[idx][n][0] << " -- " << thresholds[idx][n][1] << ")" << std::endl;
            }
            ++idx;
        }

        delete[] classNamesCol;
    }

    void RSGISFindChangeClumpsStdDevThreshold::calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols,
                                                            int *inIntCols, unsigned int numInIntCols,
                                                            std::string *inStringCols, unsigned int numInStringCols,
                                                            double *outRealCols, unsigned int numOutRealCols, int *outIntCols,
                                                            unsigned int numOutIntCols,
                                                            std::string *outStringCols, unsigned int numOutStringCols)
    {
        bool withinClass = false;

        std::string className = inStringCols[0];
        
        bool foundClass = false;
        unsigned int classIdx = 0;
        for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = this->classChangeField->begin(); iterClasses != this->classChangeField->end(); ++iterClasses)
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
            for(unsigned int n = 0; n < this->numFields; ++n)
            {
                if((inRealCols[n] < this->thresholds[classIdx][n][0]) | (inRealCols[n] > this->thresholds[classIdx][n][1]) )
                {
                    withinClass = false;
                    break;
                }
            }
            if(!withinClass)
            {
                outIntCols[0] = this->classChangeField->at(classIdx)->outName;
            }
            else
            {
                outIntCols[0] = 0;
            }
        }
        else
        {
            outIntCols[0] = 0;
        }

    }

    RSGISFindChangeClumpsStdDevThreshold::~RSGISFindChangeClumpsStdDevThreshold()
    {
        for(unsigned int i = 0; i < this->numClasses; ++i)
        {
            for(unsigned int j = 0; j < this->numFields; ++j)
            {
                delete[] this->thresholds[i][j];
            }
            delete[] this->thresholds[i];
        }
        delete[] this->thresholds;

        delete[] this->fieldIdxs;
    }


    RSGISGetGlobalClassStats::RSGISGetGlobalClassStats(GDALDataset *clumpsDataset, std::string classCol, std::vector<std::string> *fields,
                                                       std::vector<rsgis::rastergis::RSGISClassChangeFields*> *classChangeField, unsigned int ratBand) : RSGISRATCalcValue()
    {
        this->classCol = classCol;
        this->fields = fields;
        this->classChangeField = classChangeField;

        RSGISRasterAttUtils attUtils;
        GDALRasterAttributeTable *attTableTmp = clumpsDataset->GetRasterBand(ratBand)->GetDefaultRAT();
        this->attTable = NULL;
        if(attTableTmp != NULL)
        {
            this->attTable = attTableTmp;
        }
        else
        {
            this->attTable = new GDALDefaultRasterAttributeTable();
        }

        // Get number of rows
        this->numRows = attTable->GetRowCount();

        // Get number of fields and classess
        this->numFields = fields->size();
        this->numClasses = this->classChangeField->size();

        // Get indexes
        this->classColIdx = attUtils.findColumnIndex(attTable, classCol);

        // Get index for each field
        this->fieldIdxs = new unsigned int[this->numFields];
        for(unsigned int i = 0; i < this->numFields; ++i)
        {
            this->fieldIdxs[i] = attUtils.findColumnIndex(this->attTable, fields->at(i));
        }

        // Get Indexes for output fields
        this->classStatsIdx = new unsigned int[this->numFields*2]; // Mean + Standard deviation for each field

        unsigned int offset = 0;
        for(unsigned int i = 0; i < this->numFields; ++i)
        {
            std::string classMeanName = classCol + fields->at(i) + "Avg";
            std::string classStdName = classCol + fields->at(i) + "Std";

            this->classStatsIdx[i+offset] = attUtils.findColumnIndexOrCreate(this->attTable, classMeanName, GFT_Real);
            this->classStatsIdx[i+offset+1] = attUtils.findColumnIndexOrCreate(this->attTable, classStdName, GFT_Real);
            ++offset;
        }

        // Set up structure to hold statistics for each class
        for(unsigned int i = 0; i < this->numClasses; ++i)
        {
            this->classChangeField->at(i)->means = new float[this->numFields];
            this->classChangeField->at(i)->stddev = new float[this->numFields];
            for(unsigned int n = 0; n < this->numFields; ++n)
            {
                this->classChangeField->at(i)->means[n] = 0;
                this->classChangeField->at(i)->stddev[n] = 0;
            }
            this->classChangeField->at(i)->count = 0;
        }

        // Get stats
        getStats();

    }

    void RSGISGetGlobalClassStats::getStats()
    {
        RSGISRasterAttUtils attUtils;

        // Load classes column to memory
        char **classNamesCol = NULL;
        size_t colLen = 0;

        classNamesCol = attUtils.readStrColumn(attTable, this->classCol, &colLen);

        if(colLen != this->numRows)
        {
            RSGISAttributeTableException("Read less rows than in attribute table");
        }

        // Itterate through fields (only load one column to memory at a time)
        for(unsigned int n = 0; n < this->numFields; ++n)
        {

            // Load column to memory
            double *changeFieldCol = NULL;

            changeFieldCol = attUtils.readDoubleColumn(attTable, this->fields->at(n), &colLen);

            if(colLen != this->numRows)
            {
                RSGISAttributeTableException("Read less rows than in attribute table");
            }

            // Calculate mean
            for(unsigned int i = 0; i < this->numRows; ++i)
            {
                bool foundClass = false;
                unsigned int classIdx = 0;
                for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = this->classChangeField->begin(); iterClasses != this->classChangeField->end(); ++iterClasses)
                {
                    if(classNamesCol[i] == (*iterClasses)->name)
                    {
                        foundClass = true;
                        break;
                    }
                    ++classIdx;
                }

                if(foundClass)
                {
                    if(boost::math::isfinite(changeFieldCol[i]) && (changeFieldCol[i] != 0))
                    {
                        this->classChangeField->at(classIdx)->means[n] += changeFieldCol[i];
                        ++this->classChangeField->at(classIdx)->count;
                    }

                }
            }
            for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = this->classChangeField->begin(); iterClasses != this->classChangeField->end(); ++iterClasses)
            {
                (*iterClasses)->means[n] = (*iterClasses)->means[n] / (*iterClasses)->count;
            }
            // Calculate standard deviation
            for(unsigned int i = 1; i < this->numRows; ++i)
            {
                bool foundClass = false;
                unsigned int classIdx = 0;
                for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = this->classChangeField->begin(); iterClasses != this->classChangeField->end(); ++iterClasses)
                {
                    if(classNamesCol[i] == (*iterClasses)->name)
                    {
                        foundClass = true;
                        break;
                    }
                    ++classIdx;
                }

                if(foundClass)
                {
                    if(boost::math::isfinite(changeFieldCol[i]) && (changeFieldCol[i] != 0))
                    {
                        this->classChangeField->at(classIdx)->stddev[n] += pow(changeFieldCol[i] - classChangeField->at(classIdx)->means[n], 2);
                    }

                }
            }

            for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = this->classChangeField->begin(); iterClasses != this->classChangeField->end(); ++iterClasses)
            {
                (*iterClasses)->stddev[n] = sqrt((*iterClasses)->stddev[n] / (*iterClasses)->count);
            }
            delete[] changeFieldCol;
        }
    }

    void RSGISGetGlobalClassStats::calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols,
                                                            int *inIntCols, unsigned int numInIntCols,
                                                            std::string *inStringCols, unsigned int numInStringCols,
                                                            double *outRealCols, unsigned int numOutRealCols, int *outIntCols,
                                                            unsigned int numOutIntCols,
                                                            std::string *outStringCols, unsigned int numOutStringCols)
    {
        //bool withinClass = false;

        std::string className = inStringCols[0];

        bool foundClass = false;
        unsigned int classIdx = 0;
        for(std::vector<rsgis::rastergis::RSGISClassChangeFields*>::iterator iterClasses = this->classChangeField->begin(); iterClasses != this->classChangeField->end(); ++iterClasses)
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
            unsigned int offset = 0;
            for(unsigned int n = 0; n < this->numFields; ++n)
            {
                outRealCols[n+offset] = classChangeField->at(classIdx)->means[n];
                outRealCols[n+1+offset] = classChangeField->at(classIdx)->stddev[n];
                ++offset;
            }

        }
        else
        {
            unsigned int offset = 0;
            for(unsigned int n = 0; n < this->numFields; ++n)
            {
                outRealCols[n+offset] = 0;
                outRealCols[n+1+offset] = 0;
                ++offset;
            }
        }

    }

    RSGISGetGlobalClassStats::~RSGISGetGlobalClassStats()
    {
        delete[] this->fieldIdxs;
        delete[] this->classStatsIdx;
    }


}}
