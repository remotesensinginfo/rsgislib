/*
 *  RSGISCalcDist2Geom.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/08/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 * 
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISCalcDist2Geom.h"

namespace rsgis{namespace img{
	
    RSGISCalcDist2Geom::RSGISCalcDist2Geom(int numberOutBands, OGRGeometryCollection *geomCollection, OGRGeometryCollection *geomOrigCollection):RSGISCalcImageValue(numberOutBands)
    {
        this->geomCollection = geomCollection;
        this->geomOrigCollection = geomOrigCollection;
    }

    void RSGISCalcDist2Geom::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
    {
        try 
        {
            
            OGRPoint pt;
            pt.setX((extent.getMinX() + (extent.getMaxX() - extent.getMinX())/2));
            pt.setY((extent.getMinY() + (extent.getMaxY() - extent.getMinY())/2));
            
            
            if(geomOrigCollection->Contains(&pt))
            {
                bandValues[0]  *= (-1);
            }
            else
            {
                bandValues[0] = pt.Distance(geomCollection);
            }
            
            //std::cout << "Dist = " << bandValues[0] << std::endl;
            
        } 
        catch (std::exception &e) 
        {
            RSGISImageCalcException(e.what());
        }
    }
    
    RSGISCalcDist2Geom::~RSGISCalcDist2Geom()
    {
        
    }
    
    
    RSGISCalcDistViaIterativeGrowth::RSGISCalcDistViaIterativeGrowth(double imgRes):RSGISCalcImageValue(1)
    {
        counter = 0;
        change = false;
        this->imgRes = imgRes;
    }
    
    void RSGISCalcDistViaIterativeGrowth::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
    {
        output[0] = bandValues[0] * imgRes;
    }
		
    void RSGISCalcDistViaIterativeGrowth::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
    {
        int midPoint = floor(((float)winSize)/2.0);
                
        if(dataBlock[0][midPoint][midPoint] == -1)
        {
            if((dataBlock[0][midPoint-1][midPoint-1] == counter) || (dataBlock[0][midPoint-1][midPoint-1] == counter+0.5))
            {
                output[0] = counter+1;
                change = true;
            }
            else if((dataBlock[0][midPoint-1][midPoint] == counter) || (dataBlock[0][midPoint-1][midPoint] == counter+0.5))
            {
                output[0] = counter+1;
                change = true;
            }
            else if((dataBlock[0][midPoint-1][midPoint+1] == counter) || (dataBlock[0][midPoint-1][midPoint+1] == counter+0.5))
            {
                output[0] = counter+1;
                change = true;
            }
            else if((dataBlock[0][midPoint][midPoint-1] == counter) || (dataBlock[0][midPoint][midPoint-1] == counter+0.5))
            {
                output[0] = counter+1;
                change = true;
            }
            else if((dataBlock[0][midPoint][midPoint+1] == counter) || (dataBlock[0][midPoint][midPoint+1] == counter+0.5))
            {
                output[0] = counter+1;
                change = true;
            }
            else if((dataBlock[0][midPoint+1][midPoint-1] == counter) || (dataBlock[0][midPoint+1][midPoint-1] == counter+0.5))
            {
                output[0] = counter+1;
                change = true;
            }
            else if((dataBlock[0][midPoint+1][midPoint] == counter) || (dataBlock[0][midPoint+1][midPoint] == counter+0.5))
            {
                output[0] = counter+1;
                change = true;
            }
            else if((dataBlock[0][midPoint+1][midPoint+1] == counter) || (dataBlock[0][midPoint+1][midPoint+1] == counter+0.5))
            {
                output[0] = counter+1;
                change = true;
            }
            else
            {
                output[0] = dataBlock[0][midPoint][midPoint];
            }
        }
        else
        {
            output[0] = dataBlock[0][midPoint][midPoint];
        }        
    }
    
    bool RSGISCalcDistViaIterativeGrowth::changeOccurred()
    {
        return change;
    }
    
    void RSGISCalcDistViaIterativeGrowth::resetChange()
    {
        change = false;
    }
    
    void RSGISCalcDistViaIterativeGrowth::incrementCounter()
    {
        counter += 1;
    }
    
    RSGISCalcDistViaIterativeGrowth::~RSGISCalcDistViaIterativeGrowth()
    {
        
    }
	
}}


