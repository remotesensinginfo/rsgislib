/*
 *  RSGISFitFunction2Pxls.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 3/7/2013.
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

#include "RSGISFitFunction2Pxls.h"

namespace rsgis{namespace img{
	
	RSGISLinearFit2Column::RSGISLinearFit2Column(std::vector<float> bandXValues, float noDataValue, bool useNoDataValue):RSGISCalcImageValue(3)
    {
        this->bandXValues = bandXValues;
        this->noDataValue = noDataValue;
        this->useNoDataValue = useNoDataValue;
    }
    
    void RSGISLinearFit2Column::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
    {
        try
        {
            std::vector<double> *dataVals = new std::vector<double>();
            std::vector<double> *xVals = new std::vector<double>();
            for(int i = 0; i < numBands; ++i)
            {
                if(this->useNoDataValue & (bandValues[i] == this->noDataValue))
                {
                    // ignore value.
                }
                else
                {
                    dataVals->push_back(bandValues[i]);
                    xVals->push_back(bandXValues.at(i));
                }
            }
            
            if(dataVals->size() != xVals->size())
            {
                std::cout << "dataVals->size() = " << dataVals->size() << std::endl;
                std::cout << "xVals->size() = " << xVals->size() << std::endl;
                throw RSGISImageCalcException("The X and Y vectors need to be of the same length.");
            }
            else if(dataVals->size() > 0)
            {
                double c0 = 0.0;
                double c1 = 0.0;
                double cov00 = 0.0;
                double cov01 = 0.0;
                double cov11 = 0.0;
                double sumsq = 0.0;
                
                gsl_fit_linear(&(*xVals)[0], 1, &(*dataVals)[0], 1, dataVals->size(), &c0, &c1, &cov00, &cov01, &cov11, &sumsq);
                
                output[0] = c0;
                output[1] = c1;
                output[2] = sumsq;
            }
            else
            {
                output[0] = 0.0;
                output[1] = 0.0;
                output[2] = 0.0;
            }
            
            
            delete dataVals;
            delete xVals;
        }
        catch(RSGISException &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISImageCalcException(e.what());
        }
    }
       
    RSGISLinearFit2Column::~RSGISLinearFit2Column()
    {
        
    }
    
    
}}
