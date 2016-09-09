/*
 *  RSGISRATCalc.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 4/05/2014.
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

#include "RSGISRATCalc.h"


namespace rsgis{namespace rastergis{
    
    RSGISRATCalc::RSGISRATCalc(RSGISRATCalcValue *ratCalcVal)
    {
        this->ratCalcVal = ratCalcVal;
    }
    
    void RSGISRATCalc::calcRATValues(GDALRasterAttributeTable *gdalRAT, std::vector<unsigned int> inRealColIdx, std::vector<unsigned int> inIntColIdx, std::vector<unsigned int> inStrColIdx, std::vector<unsigned int> outRealColIdx, std::vector<unsigned int> outIntColIdx, std::vector<unsigned int> outStrColIdx) throw(RSGISAttributeTableException)
    {
        try
        {
            unsigned int numInRealCols = inRealColIdx.size();
            unsigned int numInIntCols = inIntColIdx.size();
            unsigned int numInStrCols = inStrColIdx.size();
            unsigned int numOutRealCols = outRealColIdx.size();
            unsigned int numOutIntCols = outIntColIdx.size();
            unsigned int numOutStrCols = outStrColIdx.size();
            
            double *dCalcInVals = NULL;
            if(numInRealCols > 0)
            {
                dCalcInVals = new double[numInRealCols];
            }
            int *iCalcInVals = NULL;
            if(numInIntCols > 0)
            {
                iCalcInVals = new int[numInIntCols];
            }
            std::string *sCalcInVals = NULL;
            if(numInStrCols > 0)
            {
                sCalcInVals = new std::string[numInStrCols];
            }
            
            double *dCalcOutVals = NULL;
            if(numOutRealCols > 0)
            {
                dCalcOutVals = new double[numOutRealCols];
            }
            int *iCalcOutVals = NULL;
            if(numOutIntCols > 0)
            {
                iCalcOutVals = new int[numOutIntCols];
            }
            std::string *sCalcOutVals = NULL;
            if(numOutStrCols > 0)
            {
                sCalcOutVals = new std::string[numOutStrCols];
            }
            
            // Iterate through blocks
            size_t nRows = gdalRAT->GetRowCount();
            unsigned int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
            unsigned int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );
            
            // Allocate Memory
            double **inRealData = NULL;
            if(numInRealCols > 0)
            {
                inRealData = new double*[numInRealCols];
                for(unsigned int i = 0; i < numInRealCols; ++i)
                {
                    inRealData[i] = new double[RAT_BLOCK_LENGTH];
                    for(int j = 0; j < RAT_BLOCK_LENGTH; ++j)
                    {
                        inRealData[i][j] = 0.0;
                    }
                }
            }
            double **outRealData = NULL;
            if(numOutRealCols > 0)
            {
                outRealData = new double*[numOutRealCols];
                for(unsigned int i = 0; i < numOutRealCols; ++i)
                {
                    outRealData[i] = new double[RAT_BLOCK_LENGTH];
                    for(int j = 0; j < RAT_BLOCK_LENGTH; ++j)
                    {
                        outRealData[i][j] = 0.0;
                    }
                }
            }
            int **inIntData = NULL;
            if(numInIntCols > 0)
            {
                inIntData = new int*[numInIntCols];
                for(unsigned int i = 0; i < numInIntCols; ++i)
                {
                    inIntData[i] = new int[RAT_BLOCK_LENGTH];
                    for(int j = 0; j < RAT_BLOCK_LENGTH; ++j)
                    {
                        inIntData[i][j] = 0.0;
                    }
                }
            }
            int **outIntData = NULL;
            if(numOutIntCols > 0)
            {
                outIntData = new int*[numOutIntCols];
                for(unsigned int i = 0; i < numOutIntCols; ++i)
                {
                    outIntData[i] = new int[RAT_BLOCK_LENGTH];
                    for(int j = 0; j < RAT_BLOCK_LENGTH; ++j)
                    {
                        outIntData[i][j] = 0.0;
                    }
                }
            }
            char ***inStrData = NULL;
            if(numInStrCols > 0)
            {
                inStrData = new char**[numInStrCols];
                for(unsigned int i = 0; i < numInStrCols; ++i)
                {
                    inStrData[i] = new char*[RAT_BLOCK_LENGTH];
                    for(int j = 0; j < RAT_BLOCK_LENGTH; ++j)
                    {
                        inStrData[i][j] = "";
                    }
                }
            }
            char ***outStrData = NULL;
            if(numOutStrCols > 0)
            {
                outStrData = new char**[numOutStrCols];
                for(unsigned int i = 0; i < numOutStrCols; ++i)
                {
                    outStrData[i] = new char*[RAT_BLOCK_LENGTH];
                    for(int j = 0; j < RAT_BLOCK_LENGTH; ++j)
                    {
                        outStrData[i][j] = "";
                    }
                }
            }
            
            int feedback = nRows/10.0;
            int feedbackCounter = 0;
            
            std::cout << "Started " << std::flush;
            size_t startRow = 0;
            size_t rowID = 0;
            for(int i = 0; i < nBlocks; i++)
            {
                // Read blocks
                for(unsigned int n = 0; n < numInRealCols; ++n)
                {
                    gdalRAT->ValuesIO(GF_Read, inRealColIdx[n], startRow, RAT_BLOCK_LENGTH, inRealData[n]);
                }
                
                for(unsigned int n = 0; n < numInIntCols; ++n)
                {
                    gdalRAT->ValuesIO(GF_Read, inIntColIdx[n], startRow, RAT_BLOCK_LENGTH, inIntData[n]);
                }
                
                for(unsigned int n = 0; n < numInStrCols; ++n)
                {
                    gdalRAT->ValuesIO(GF_Read, inStrColIdx[n], startRow, RAT_BLOCK_LENGTH, inStrData[n]);
                }
                
                // Loop through block
                for(int j = 0; j < RAT_BLOCK_LENGTH; ++j)
                {
                    // Show progress
                    if((feedback != 0) && ((rowID % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    for(unsigned int n = 0; n < numInRealCols; ++n)
                    {
                        dCalcInVals[n] = inRealData[n][j];
                    }
                    
                    for(unsigned int n = 0; n < numInIntCols; ++n)
                    {
                        iCalcInVals[n] = inIntData[n][j];
                    }
                    
                    for(unsigned int n = 0; n < numInStrCols; ++n)
                    {
                        sCalcInVals[n] = std::string(inStrData[n][j]);
                    }
                    
                    this->ratCalcVal->calcRATValue(rowID, dCalcInVals, numInRealCols, iCalcInVals, numInIntCols, sCalcInVals, numInStrCols, dCalcOutVals, numOutRealCols, iCalcOutVals, numOutIntCols, sCalcOutVals, numOutStrCols);
                    
                    for(unsigned int n = 0; n < numOutRealCols; ++n)
                    {
                        outRealData[n][j] = dCalcOutVals[n];
                    }
                    
                    for(unsigned int n = 0; n < numOutIntCols; ++n)
                    {
                        outIntData[n][j] = iCalcOutVals[n];
                    }
                    
                    for(unsigned int n = 0; n < numOutStrCols; ++n)
                    {
                        outStrData[n][j] = new char[sCalcOutVals[n].size()];
                        strcpy(outStrData[n][j], sCalcOutVals[n].c_str());
                        //outStrData[n][j] = const_cast<char*>(sCalcOutVals[n].c_str());
                    }
                    
                    ++rowID;
                }
                
                //Write blocks
                for(unsigned int n = 0; n < numOutRealCols; ++n)
                {
                    gdalRAT->ValuesIO(GF_Write, outRealColIdx[n], startRow, RAT_BLOCK_LENGTH, outRealData[n]);
                    for(int j = 0; j < RAT_BLOCK_LENGTH; ++j)
                    {
                        outRealData[n][j] = 0.0;
                    }
                }
                
                for(unsigned int n = 0; n < numOutIntCols; ++n)
                {
                    gdalRAT->ValuesIO(GF_Write, outIntColIdx[n], startRow, RAT_BLOCK_LENGTH, outIntData[n]);
                    for(int j = 0; j < RAT_BLOCK_LENGTH; ++j)
                    {
                        outIntData[n][j] = 0.0;
                    }
                }
                
                for(unsigned int n = 0; n < numOutStrCols; ++n)
                {
                    gdalRAT->ValuesIO(GF_Write, outStrColIdx[n], startRow, RAT_BLOCK_LENGTH, outStrData[n]);
                    for(int j = 0; j < RAT_BLOCK_LENGTH; ++j)
                    {
                        outStrData[n][j] = "";
                    }
                }
                
                startRow += RAT_BLOCK_LENGTH;
            }
            if(remainRows > 0)
            {
                // Read blocks
                for(unsigned int n = 0; n < numInRealCols; ++n)
                {
                    gdalRAT->ValuesIO(GF_Read, inRealColIdx[n], startRow, remainRows, inRealData[n]);
                }
                
                for(unsigned int n = 0; n < numInIntCols; ++n)
                {
                    gdalRAT->ValuesIO(GF_Read, inIntColIdx[n], startRow, remainRows, inIntData[n]);
                }
                
                for(unsigned int n = 0; n < numInStrCols; ++n)
                {
                    gdalRAT->ValuesIO(GF_Read, inStrColIdx[n], startRow, remainRows, inStrData[n]);
                }
                
                // Loop through block
                for(int j = 0; j < remainRows; ++j)
                {
                    // Show progress
                    if((feedback != 0) && ((rowID % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    for(unsigned int n = 0; n < numInRealCols; ++n)
                    {
                        dCalcInVals[n] = inRealData[n][j];
                    }
                    
                    for(unsigned int n = 0; n < numInIntCols; ++n)
                    {
                        iCalcInVals[n] = inIntData[n][j];
                    }
                    
                    for(unsigned int n = 0; n < numInStrCols; ++n)
                    {
                        sCalcInVals[n] = std::string(inStrData[n][j]);
                    }
                    
                    this->ratCalcVal->calcRATValue(rowID, dCalcInVals, numInRealCols, iCalcInVals, numInIntCols, sCalcInVals, numInStrCols, dCalcOutVals, numOutRealCols, iCalcOutVals, numOutIntCols, sCalcOutVals, numOutStrCols);
                    
                    for(unsigned int n = 0; n < numOutRealCols; ++n)
                    {
                        outRealData[n][j] = dCalcOutVals[n];
                    }
                    
                    for(unsigned int n = 0; n < numOutIntCols; ++n)
                    {
                        outIntData[n][j] = iCalcOutVals[n];
                    }
                    
                    for(unsigned int n = 0; n < numOutStrCols; ++n)
                    {
                        outStrData[n][j] = new char[sCalcOutVals[n].size()];
                        strcpy(outStrData[n][j], sCalcOutVals[n].c_str());
                        //outStrData[n][j] =  const_cast<char*>(sCalcOutVals[n].c_str());
                    }
                    
                    ++rowID;
                }
                
                // Write blocks
                for(unsigned int n = 0; n < numOutRealCols; ++n)
                {
                    gdalRAT->ValuesIO(GF_Write, outRealColIdx[n], startRow, remainRows, outRealData[n]);
                }
                
                for(unsigned int n = 0; n < numOutIntCols; ++n)
                {
                    gdalRAT->ValuesIO(GF_Write, outIntColIdx[n], startRow, remainRows, outIntData[n]);
                }
                
                for(unsigned int n = 0; n < numOutStrCols; ++n)
                {
                    gdalRAT->ValuesIO(GF_Write, outStrColIdx[n], startRow, remainRows, outStrData[n]);
                }
            }
            std::cout << ".Completed\n";
            
            // Clean out and release memory...
            if(numInRealCols > 0)
            {
                for(unsigned int i = 0; i < numInRealCols; ++i)
                {
                    delete[] inRealData[i];
                }
                delete[] inRealData;
            }
            if(numOutRealCols > 0)
            {
                for(unsigned int i = 0; i < numOutRealCols; ++i)
                {
                    delete[] outRealData[i];
                }
                delete[] outRealData;
            }
            if(numInIntCols > 0)
            {
                for(unsigned int i = 0; i < numInIntCols; ++i)
                {
                    delete[] inIntData[i];
                }
                delete[] inIntData;
            }
            if(numOutIntCols > 0)
            {
                for(unsigned int i = 0; i < numOutIntCols; ++i)
                {
                    delete[] outIntData[i];
                }
                delete[] outIntData;
            }
            if(numInStrCols > 0)
            {
                for(unsigned int i = 0; i < numInStrCols; ++i)
                {
                    delete[] inStrData[i];
                }
                delete[] inStrData;
            }
            if(numOutStrCols > 0)
            {
                for(unsigned int i = 0; i < numOutStrCols; ++i)
                {
                    delete[] outStrData[i];
                }
                delete[] outStrData;
            }
            if(numInRealCols > 0)
            {
                delete[] dCalcInVals;
            }
            if(numInIntCols > 0)
            {
                delete[] iCalcInVals;
            }
            if(numInStrCols > 0)
            {
                delete[] sCalcInVals;
            }
            if(numOutRealCols > 0)
            {
                delete[] dCalcOutVals;
            }
            if(numOutIntCols > 0)
            {
                delete[] iCalcOutVals;
            }
            if(numOutStrCols > 0)
            {
                delete[] sCalcOutVals;
            }            
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISRATCalc::~RSGISRATCalc()
    {
        
    }
    
    
}}




