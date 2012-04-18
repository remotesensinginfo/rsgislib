/*
 *  RSGISFindMeanDist2Neighbours.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 28/03/2012.
 *  Copyright 2012 RSGISLib.
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

#ifndef RSGISFindMeanDist2Neighbours_H
#define RSGISFindMeanDist2Neighbours_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <algorithm>

#include "common/RSGISAttributeTableException.h"
#include "rastergis/RSGISAttributeTable.h"

using namespace std;
using namespace rsgis;

namespace rsgis{namespace rastergis{
    
    class RSGISFindMeanDist2Neighbours
    {
    public:
        RSGISFindMeanDist2Neighbours();
        void findMeanEuclideanDist2Neighbours(RSGISAttributeTable *attTable, vector<vector<unsigned long > > *neighbours, vector<string> *attributeNames, string outMeanAttName, string outMaxAttName, string outMinAttName)throw(RSGISAttributeTableException);
        void findMeanEuclideanDist2Neighbours(RSGISAttributeTable *attTable, vector<string> *attributeNames, string outMeanAttName, string outMaxAttName, string outMinAttName)throw(RSGISAttributeTableException);
        ~RSGISFindMeanDist2Neighbours();
    };
    
}}

#endif
