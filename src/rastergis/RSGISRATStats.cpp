/*
 *  RSGISRATStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/08/2015.
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

#include "rastergis/RSGISRATStats.h"


namespace rsgis{namespace rastergis{
    
    RSGISRATStats::RSGISRATStats()
    {
        
    }
    
    float RSGISRATStats::calc1DJMDistance(GDALDataset *clumpsImage, std::string varCol, float binWidth, std::string classColumn, std::string class1Val, std::string class2Val, unsigned int ratBand)
    {
        float dist = 0.0;
        try
        {
            // Get attribute table...
            GDALRasterAttributeTable *attTable = clumpsImage->GetRasterBand(1)->GetDefaultRAT();
            
            if(attTable == NULL)
            {
                throw RSGISAttributeTableException("The image dataset does not have an attribute table.");
            }
            
            rsgis::rastergis::RSGISRasterAttUtils ratUtils;
            std::vector<std::string> *imgClassColVals = ratUtils.readStrColumnAsVec(attTable, classColumn);
            std::vector<double> *varVals = ratUtils.readDoubleColumnAsVec(attTable, varCol);
            
            std::vector<double> *valsClass1 = new std::vector<double>();
            std::vector<double> *valsClass2 = new std::vector<double>();
            
            double minVal1 = 0.0;
            double maxVal1 = 0.0;
            bool first1 = true;
            
            double minVal2 = 0.0;
            double maxVal2 = 0.0;
            bool first2 = true;
            
            size_t numRows = varVals->size();
            for(size_t i = 0; i < numRows; ++i)
            {
                if(imgClassColVals->at(i) == class1Val)
                {
                    valsClass1->push_back(varVals->at(i));
                    if(first1)
                    {
                        minVal1 = varVals->at(i);
                        maxVal1 = varVals->at(i);
                        first1 = false;
                    }
                    else
                    {
                        if(varVals->at(i) < minVal1)
                        {
                            minVal1 = varVals->at(i);
                        }
                        else if(varVals->at(i) > maxVal1)
                        {
                            maxVal1 = varVals->at(i);
                        }
                    }
                }
                else if(imgClassColVals->at(i) == class2Val)
                {
                    valsClass2->push_back(varVals->at(i));
                    
                    if(first2)
                    {
                        minVal2 = varVals->at(i);
                        maxVal2 = varVals->at(i);
                        first2 = false;
                    }
                    else
                    {
                        if(varVals->at(i) < minVal2)
                        {
                            minVal2 = varVals->at(i);
                        }
                        else if(varVals->at(i) > maxVal2)
                        {
                            maxVal2 = varVals->at(i);
                        }
                    }
                }
            }
            
            double min = minVal1;
            if(minVal2 < min)
            {
                min = minVal2;
            }
            
            double max = maxVal1;
            if(maxVal2 > max)
            {
                max = maxVal2;
            }
            
            rsgis::math::RSGISMathsUtils mathUtils;
            std::vector<std::pair<double, double> > *hist1 =  mathUtils.calcHistogram(valsClass1, min, max, binWidth, true);
            std::vector<std::pair<double, double> > *hist2 =  mathUtils.calcHistogram(valsClass2, min, max, binWidth, true);
            
            if(hist1->size() != hist2->size())
            {
                throw rsgis::RSGISAttributeTableException("Histograms must have the same number of bins.");
            }
            
            double sumVals = 0.0;
            double tmpVal = 0.0;
            for(size_t i = 0; i < hist1->size(); ++i)
            {
                tmpVal = hist1->at(i).second * hist2->at(i).second;
                if(tmpVal != 0.0)
                {
                    sumVals += sqrt(tmpVal);
                }
            }
            
            dist = sqrt(2 * (1 - sumVals));
            
            delete valsClass1;
            delete valsClass2;
            delete imgClassColVals;
            delete varVals;
        }
        catch(rsgis::RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        return dist;
    }
    
    float RSGISRATStats::calc2DJMDistance(GDALDataset *clumpsImage, std::string var1Col, std::string var2Col, float var1binWidth, float var2binWidth, std::string classColumn, std::string class1Val, std::string class2Val, unsigned int ratBand)
    {
        float dist = 0.0;
        try
        {
            // Get attribute table...
            GDALRasterAttributeTable *attTable = clumpsImage->GetRasterBand(1)->GetDefaultRAT();
            
            if(attTable == NULL)
            {
                throw RSGISAttributeTableException("The image dataset does not have an attribute table.");
            }
            
            rsgis::rastergis::RSGISRasterAttUtils ratUtils;
            std::vector<std::string> *imgClassColVals = ratUtils.readStrColumnAsVec(attTable, classColumn);
            std::vector<double> *varVals1 = ratUtils.readDoubleColumnAsVec(attTable, var1Col);
            std::vector<double> *varVals2 = ratUtils.readDoubleColumnAsVec(attTable, var2Col);
            
            size_t numRows = imgClassColVals->size();
            
            std::vector<double> *vals1Class1 = new std::vector<double>();
            std::vector<double> *vals1Class2 = new std::vector<double>();
            
            double minVal1C1 = 0.0;
            double maxVal1C1 = 0.0;
            bool first1C1 = true;
            
            double minVal1C2 = 0.0;
            double maxVal1C2 = 0.0;
            bool first1C2 = true;
            
            for(size_t i = 0; i < numRows; ++i)
            {
                if(imgClassColVals->at(i) == class1Val)
                {
                    vals1Class1->push_back(varVals1->at(i));
                    if(first1C1)
                    {
                        minVal1C1 = varVals1->at(i);
                        maxVal1C1 = varVals1->at(i);
                        first1C1 = false;
                    }
                    else
                    {
                        if(varVals1->at(i) < minVal1C1)
                        {
                            minVal1C1 = varVals1->at(i);
                        }
                        else if(varVals1->at(i) > maxVal1C1)
                        {
                            maxVal1C1 = varVals1->at(i);
                        }
                    }
                }
                else if(imgClassColVals->at(i) == class2Val)
                {
                    vals1Class2->push_back(varVals1->at(i));
                    
                    if(first1C2)
                    {
                        minVal1C2 = varVals1->at(i);
                        maxVal1C2 = varVals1->at(i);
                        first1C2 = false;
                    }
                    else
                    {
                        if(varVals1->at(i) < minVal1C2)
                        {
                            minVal1C2 = varVals1->at(i);
                        }
                        else if(varVals1->at(i) > maxVal1C2)
                        {
                            maxVal1C2 = varVals1->at(i);
                        }
                    }
                }
            }
            
            std::vector<double> *vals2Class1 = new std::vector<double>();
            std::vector<double> *vals2Class2 = new std::vector<double>();

            double minVal2C1 = 0.0;
            double maxVal2C1 = 0.0;
            bool first2C1 = true;
            
            double minVal2C2 = 0.0;
            double maxVal2C2 = 0.0;
            bool first2C2 = true;
            
            for(size_t i = 0; i < numRows; ++i)
            {
                if(imgClassColVals->at(i) == class1Val)
                {
                    vals2Class1->push_back(varVals2->at(i));
                    if(first2C1)
                    {
                        minVal2C1 = varVals2->at(i);
                        maxVal2C1 = varVals2->at(i);
                        first2C1 = false;
                    }
                    else
                    {
                        if(varVals2->at(i) < minVal2C1)
                        {
                            minVal2C1 = varVals2->at(i);
                        }
                        else if(varVals2->at(i) > maxVal2C1)
                        {
                            maxVal2C1 = varVals2->at(i);
                        }
                    }
                }
                else if(imgClassColVals->at(i) == class2Val)
                {
                    vals2Class2->push_back(varVals2->at(i));
                    
                    if(first2C2)
                    {
                        minVal2C2 = varVals2->at(i);
                        maxVal2C2 = varVals2->at(i);
                        first2C2 = false;
                    }
                    else
                    {
                        if(varVals2->at(i) < minVal2C2)
                        {
                            minVal2C2 = varVals2->at(i);
                        }
                        else if(varVals2->at(i) > maxVal2C2)
                        {
                            maxVal2C2 = varVals2->at(i);
                        }
                    }
                }
            }
            
            double minVal1 = minVal1C1;
            if(minVal1C2 < minVal1)
            {
                minVal1 = minVal1C2;
            }
            double maxVal1 = maxVal1C1;
            if(maxVal1C2 > maxVal1)
            {
                maxVal1 = maxVal1C2;
            }
            
            double minVal2 = minVal2C1;
            if(minVal2C2 < minVal2)
            {
                minVal2 = minVal2C2;
            }
            double maxVal2 = maxVal2C1;
            if(maxVal2C2 > maxVal2)
            {
                maxVal2 = maxVal2C2;
            }
            
            rsgis::math::RSGISMathsUtils mathUtils;
            std::vector<std::vector<rsgis::math::RSGIS2DHistBin>* > *hist1 =  mathUtils.calc2DHistogram(vals1Class1, minVal1, maxVal1, var1binWidth, vals2Class1, minVal2, maxVal2, var2binWidth, true);
            std::vector<std::vector<rsgis::math::RSGIS2DHistBin>* > *hist2 =  mathUtils.calc2DHistogram(vals1Class2, minVal1, maxVal1, var1binWidth, vals2Class2, minVal2, maxVal2, var2binWidth, true);

            if(hist1->size() != hist2->size())
            {
                throw RSGISAttributeTableException("Histograms are not the same size (first axis).");
            }
            
            double sumVals = 0.0;
            double tmpVal = 0.0;
            for(size_t i = 0; i < hist1->size(); ++i)
            {
                
                if(hist1->at(i)->size() != hist2->at(i)->size())
                {
                    std::cerr << "Histograms are not the same size - " << i << "th row\n" << std::endl;
                    throw RSGISAttributeTableException("Histograms are not the same size (second axis).");
                }
                
                for(size_t j = 0; j < hist1->at(i)->size(); ++j)
                {
                    tmpVal = hist1->at(i)->at(j).freq * hist2->at(i)->at(j).freq;
                    if(tmpVal != 0.0)
                    {
                        sumVals += sqrt(tmpVal);
                    }
                }
                
                delete hist1->at(i);
                delete hist2->at(i);
            }
            delete hist1;
            delete hist2;
            
            dist = sqrt(2 * (1 - sumVals));
            
            delete imgClassColVals;
            delete varVals1;
            delete varVals2;
            delete vals1Class1;
            delete vals1Class2;
            delete vals2Class1;
            delete vals2Class2;
            
        }
        catch(rsgis::RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        return dist;
    }
    
    float RSGISRATStats::calcBhattacharyyaDistance(GDALDataset *clumpsImage, std::string varCol, std::string classColumn, std::string class1Val, std::string class2Val, unsigned int ratBand)
    {
        float dist = 0.0;
        try
        {
            // Get attribute table...
            GDALRasterAttributeTable *attTable = clumpsImage->GetRasterBand(1)->GetDefaultRAT();
            
            if(attTable == NULL)
            {
                throw RSGISAttributeTableException("The image dataset does not have an attribute table.");
            }
            
            rsgis::rastergis::RSGISRasterAttUtils ratUtils;
            std::vector<std::string> *imgClassColVals = ratUtils.readStrColumnAsVec(attTable, classColumn);
            std::vector<double> *varVals = ratUtils.readDoubleColumnAsVec(attTable, varCol);
            
            std::vector<double> *valsClass1 = new std::vector<double>();
            std::vector<double> *valsClass2 = new std::vector<double>();
            
            size_t numRows = varVals->size();
            for(size_t i = 0; i < numRows; ++i)
            {
                if(imgClassColVals->at(i) == class1Val)
                {
                    valsClass1->push_back(varVals->at(i));
                }
                else if(imgClassColVals->at(i) == class2Val)
                {
                    valsClass2->push_back(varVals->at(i));
                }
            }
            
            rsgis::math::RSGISMathsUtils mathUtils;
           
            rsgis::math::RSGISStatsSummary *stats1 = new rsgis::math::RSGISStatsSummary();
            mathUtils.initStatsSummary(stats1);
            stats1->calcMean = true;
            stats1->calcVariance = true;
            mathUtils.generateStats(valsClass1, stats1);
            
            
            rsgis::math::RSGISStatsSummary *stats2 = new rsgis::math::RSGISStatsSummary();
            mathUtils.initStatsSummary(stats2);
            stats2->calcMean = true;
            stats2->calcVariance = true;
            mathUtils.generateStats(valsClass2, stats2);
            
            // https://en.wikipedia.org/wiki/Bhattacharyya_distance
            
            double varPQ = stats1->variance / stats2->variance;
            double varQP = stats2->variance / stats1->variance;
            double diffMeanPQ = (stats1->mean / stats2->mean) * (stats1->mean / stats2->mean);
            double sumVarPQ = stats1->variance + stats2->variance;
            
            double partA = log((varPQ + varQP + 2) / 4)/4;
            
            double partB = (diffMeanPQ/sumVarPQ) / 4;
            
            dist = partA + partB;
            
            delete valsClass1;
            delete valsClass2;
            delete imgClassColVals;
            delete varVals;
            delete stats1;
            delete stats2;
        }
        catch(rsgis::RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        return dist;
    }
    
    RSGISRATStats::~RSGISRATStats()
    {
        
    }
    
}}



