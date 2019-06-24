/*
 *  RSGISHistoCubeUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/02/2017.
 *
 *  Copyright 2017 RSGISLib.
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


#include "RSGISHistoCubeUtils.h"

namespace rsgis {namespace histocube{
    
    
    RSGISHistoCubeUtils::RSGISHistoCubeUtils()
    {
        
    }
    
    long RSGISHistoCubeUtils::getBinsIndex(int val, std::vector<int> bins)
    {
        long idx = -1;
        try
        {
            long cIdx = 0;
            bool found = false;
            for(std::vector<int>::iterator iterVal = bins.begin(); iterVal != bins.end(); ++iterVal)
            {
                if((*iterVal) == val)
                {
                    idx = cIdx;
                    found = true;
                    break;
                }
                ++cIdx;
            }
            
            if(!found)
            {
                idx = -1;
            }
            
            /*
            std::vector<int>::iterator iterVal = std::find(bins.begin(), bins.end(), val);
            if(iterVal != bins.end())
            {
                //std::cout << (*iterVal) << std::endl;
                idx = std::distance(bins.begin(), iterVal);
                //std::cout << idx << std::endl;
            }
             */
        }
        catch(rsgis::RSGISHistoCubeException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw rsgis::RSGISHistoCubeException(e.what());
        }
        
        return idx;
    }
    
    RSGISHistoCubeUtils::~RSGISHistoCubeUtils()
    {
        
    }
    
}}


