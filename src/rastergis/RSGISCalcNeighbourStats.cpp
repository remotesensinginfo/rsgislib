/*
 *  RSGISCalcNeighbourStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/08/2014.
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

#include "RSGISCalcNeighbourStats.h"

namespace rsgis{namespace rastergis{
    
    RSGISCalcNeighbourStats::RSGISCalcNeighbourStats()
    {
        
    }
    
    void RSGISCalcNeighbourStats::populateStatsDiff2Neighbours(GDALDataset *inputClumps, RSGISFieldAttStats *fieldStats, bool useAbsDiff, unsigned int ratBand)
    {
        try
        {
            RSGISRasterAttUtils attUtils;
            rsgis::math::RSGISMathsUtils mathUtils;
            double *minDiffCol = NULL;
            double *maxDiffCol = NULL;
            double *sumDiffCol = NULL;
            double *meanDiffCol = NULL;
            double *stddevDiffCol = NULL;
            
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
            
            fieldStats->fieldIdx = attUtils.findColumnIndex(rat, fieldStats->field);
            
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
            double *dataVals = attUtils.readDoubleColumn(rat, fieldStats->field, &colLen);
            
            if(colLen != numRows)
            {
                for(std::vector<std::vector<size_t>* >::iterator iterNeigh = neighbours->begin(); iterNeigh != neighbours->end(); ++iterNeigh)
                {
                    delete *iterNeigh;
                }
                delete neighbours;
                delete[] dataVals;
                throw rsgis::RSGISAttributeTableException("The column does not have enough values ");
            }
            
            std::vector<double> *diffClumpVals = new std::vector<double>();
            rsgis::math::RSGISStatsSummary *stats2Calc = new rsgis::math::RSGISStatsSummary();
            
            if(fieldStats->calcMin)
            {
                stats2Calc->calcMin = true;
                fieldStats->minFieldIdx = attUtils.findColumnIndexOrCreate(rat, fieldStats->minField, GFT_Real);
                minDiffCol = new double[numRows];
            }
            if(fieldStats->calcMax)
            {
                stats2Calc->calcMax = true;
                fieldStats->maxFieldIdx = attUtils.findColumnIndexOrCreate(rat, fieldStats->maxField, GFT_Real);
                maxDiffCol = new double[numRows];
            }
            if(fieldStats->calcMean)
            {
                stats2Calc->calcMean = true;
                fieldStats->meanFieldIdx = attUtils.findColumnIndexOrCreate(rat, fieldStats->meanField, GFT_Real);
                meanDiffCol = new double[numRows];
            }
            if(fieldStats->calcStdDev)
            {
                stats2Calc->calcStdDev = true;
                fieldStats->stdDevFieldIdx = attUtils.findColumnIndexOrCreate(rat, fieldStats->stdDevField, GFT_Real);
                stddevDiffCol = new double[numRows];
            }
            if(fieldStats->calcSum)
            {
                stats2Calc->calcSum = true;
                fieldStats->sumFieldIdx = attUtils.findColumnIndexOrCreate(rat, fieldStats->sumField, GFT_Real);
                sumDiffCol = new double[numRows];
            }
            
            for(size_t i  = 0; i <  numRows; ++i)
            {
                diffClumpVals->clear();
                std::vector<size_t> *clumpNeigh = neighbours->at(i);
                diffClumpVals->reserve(clumpNeigh->size());
                stats2Calc->min = 0.0;
                stats2Calc->max = 0.0;
                stats2Calc->mean = 0.0;
                stats2Calc->stdDev = 0.0;
                stats2Calc->sum = 0.0;
                for(std::vector<size_t>::iterator iterNeigh = clumpNeigh->begin(); iterNeigh != clumpNeigh->end(); ++iterNeigh)
                {
                    if(useAbsDiff)
                    {
                        diffClumpVals->push_back(fabs(dataVals[i] - dataVals[*iterNeigh]));
                    }
                    else
                    {
                        diffClumpVals->push_back((dataVals[i] - dataVals[*iterNeigh]));
                    }
                }
                
                mathUtils.generateStats(diffClumpVals, stats2Calc);
                if(stats2Calc->calcMin)
                {
                    minDiffCol[i] = stats2Calc->min;
                }
                if(stats2Calc->calcMax)
                {
                    maxDiffCol[i] = stats2Calc->max;
                }
                if(stats2Calc->calcMean)
                {
                    meanDiffCol[i] = stats2Calc->mean;
                }
                if(stats2Calc->calcStdDev)
                {
                    stddevDiffCol[i] = stats2Calc->stdDev;
                }
                if(stats2Calc->calcSum)
                {
                    sumDiffCol[i] = stats2Calc->sum;
                }
            }
            
            if(fieldStats->calcMin)
            {
                rat->ValuesIO(GF_Write, fieldStats->minFieldIdx, 0, numRows, minDiffCol);
                delete[] minDiffCol;
            }
            if(fieldStats->calcMax)
            {
                rat->ValuesIO(GF_Write, fieldStats->maxFieldIdx, 0, numRows, maxDiffCol);
                delete[] maxDiffCol;
            }
            if(fieldStats->calcMean)
            {
                rat->ValuesIO(GF_Write, fieldStats->meanFieldIdx, 0, numRows, meanDiffCol);
                delete[] meanDiffCol;
            }
            if(fieldStats->calcStdDev)
            {
                rat->ValuesIO(GF_Write, fieldStats->stdDevFieldIdx, 0, numRows, stddevDiffCol);
                delete[] stddevDiffCol;
            }
            if(fieldStats->calcSum)
            {
                rat->ValuesIO(GF_Write, fieldStats->sumFieldIdx, 0, numRows, sumDiffCol);
                delete[] sumDiffCol;
            }
            delete stats2Calc;
            
            for(std::vector<std::vector<size_t>* >::iterator iterNeigh = neighbours->begin(); iterNeigh != neighbours->end(); ++iterNeigh)
            {
                delete *iterNeigh;
            }
            delete neighbours;
            
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
    
    RSGISCalcNeighbourStats::~RSGISCalcNeighbourStats()
    {
        
    }
    
    
}}
