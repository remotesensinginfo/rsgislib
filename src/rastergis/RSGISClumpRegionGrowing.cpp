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
    
    void RSGISClumpRegionGrowing::growClassRegion(GDALDataset *inputClumps, std::string classColumn, std::string classVal, int maxIter, unsigned int ratBand)throw(RSGISAttributeTableException)
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
            
            //unsigned int classColumnIdx = attUtils.findColumnIndex(rat, classColumn);
            
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
            bool maxIterDef = false;
            if(maxIter >= 0)
            {
                maxIterDef = true;
            }
            int numIter = 0;
            while(changeFound)
            {
                changeFound = false;
                for(size_t i  = 0; i <  numRows; ++i)
                {
                    classColValsTmp[i] = "";
                    
                    if(classColVals[i] == classVal)
                    {
                        // Check the neighbours...
                        std::vector<size_t> *clumpNeigh = neighbours->at(i);
                        for(std::vector<size_t>::iterator iterNeigh = clumpNeigh->begin(); iterNeigh != clumpNeigh->end(); ++iterNeigh)
                        {
                            if(classColVals[*iterNeigh] != classVal)
                            {
                                // Check if condition is met, if met then 'grow' and set change flag...
                                
                                
                                
                                
                                
                                
                            }
                        }
                    }
                }
                
                // Copy class names to 'main' array...
                for(size_t i  = 0; i <  numRows; ++i)
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
            
            
            attUtils.writeStrColumn(rat, classColumn, classColVals, colLen);
            
            delete[] classColVals;
            delete[] classColValsTmp;
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
    
    RSGISClumpRegionGrowing::~RSGISClumpRegionGrowing()
    {
        
    }
    
    
}}