/*
 *  RSGISAttributeTableNeighbours.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/03/2012.
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

#ifndef RSGISAttributeTableNeighbours_H
#define RSGISAttributeTableNeighbours_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <list>
#include <vector>
#include <algorithm>

#include "common/RSGISAttributeTableException.h"

#include "utils/RSGISTextUtils.h"

using namespace std;
using namespace rsgis;
using namespace rsgis::utils;

namespace rsgis{namespace rastergis{
    
    class RSGISAttributeTableNeighbours
    {
    public:
        RSGISAttributeTableNeighbours();
        vector<vector<unsigned long > >* importFromTextFile(string inFilePath) throw(RSGISAttributeTableException);
        void exportToTextFile(vector<list<unsigned long >* > *neighbours, string outFilePath) throw(RSGISAttributeTableException);
        void exportToTextFile(vector<vector<unsigned long > > *neighbours, string outFilePath) throw(RSGISAttributeTableException);
        void clearMemory(vector<list<unsigned long >* > *neighbours);
        ~RSGISAttributeTableNeighbours();
    };
    
}}

#endif

