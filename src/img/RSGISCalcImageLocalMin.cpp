/*
 *  RSGISCalcImageLocalMin.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/08/2018.
 *  Copyright 2018 RSGISLib.
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

#include "RSGISCalcImageLocalMin.h"



namespace rsgis{namespace img{
    
    RSGISCalcLocalMinInWin::RSGISCalcLocalMinInWin(std::vector<unsigned int> bands, float noDataValue, bool useNoDataValue):RSGISCalcImageValue(1)
    {
        this->bands = bands;
        this->noDataValue = noDataValue;
        this->useNoDataValue = useNoDataValue;
        this->minVals = new double[bands.size()];
        this->first = new bool[bands.size()];
    }
    
    void RSGISCalcLocalMinInWin::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, double *outRefVal, unsigned int nOutRefVals) throw(RSGISImageCalcException)
    {
        for(int n = 0; n < this->bands.size(); ++n)
        {
            this->first[n] = true;
            this->minVals[n] = 0;
        }
        bool noData = false;
        for(int i = 0; i < winSize; ++i)
        {
            for(int j = 0; j < winSize; ++j)
            {
                for(int n = 0; n < this->bands.size(); ++n)
                {
                    noData = false;
                    if(this->useNoDataValue && (dataBlock[bands.at(n)-1][i][j] == this->noDataValue))
                    {
                        noData = true;
                    }
                    if(!noData)
                    {
                        if(first[n])
                        {
                            this->minVals[n] = dataBlock[bands.at(n)-1][i][j];
                            first[n] = false;
                        }
                        else if(dataBlock[bands.at(n)-1][i][j] < this->minVals[n])
                        {
                            this->minVals[n] = dataBlock[bands.at(n)-1][i][j];
                        }
                    }
                }
            }
        }
        
        double minVal = 0;
        double outBand = 0;
        bool firstBand = true;
        for(int n = 0; n < this->bands.size(); ++n)
        {
            if(!first[n])
            {
                if(firstBand)
                {
                    minVal = this->minVals[n];
                    outBand = bands.at(n);
                    firstBand = false;
                }
                else if(this->minVals[n] < minVal)
                {
                    minVal = this->minVals[n];
                    outBand = bands.at(n);
                }
            }
        }
        if(firstBand)
        {
            output[0] = minVal;
            outRefVal[0] = outBand;
        }
        else
        {
            output[0] = minVal;
            outRefVal[0] = outBand;
        }
    }
    
    RSGISCalcLocalMinInWin::~RSGISCalcLocalMinInWin()
    {
        delete[] this->minVals;
        delete[] this->first;
    }
    
}}
