/*
 *  RSGISAttributeTableMem.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/04/2012.
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

#ifndef RSGISAttributeTableMem_h
#define RSGISAttributeTableMem_h

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility>

#include "common/RSGISAttributeTableException.h"

#include "utils/RSGISTextUtils.h"

#include "rastergis/RSGISAttributeTable.h"

#include "boost/math/special_functions/fpclassify.hpp"
#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

#include "H5Cpp.h"

namespace rsgis{namespace rastergis{
    
    using namespace std;
    using namespace rsgis;
    using namespace rsgis::utils;
    
    using boost::lexical_cast;
    using boost::bad_lexical_cast;
    
    using namespace H5;

    class RSGISAttributeTableMem : public RSGISAttributeTable
    {
    public:        
        RSGISAttributeTableMem(size_t numFeatures);
        RSGISAttributeTableMem(size_t numFeatures, vector<pair<string, RSGISAttributeDataType> > *fields);
        
        bool getBoolField(size_t fid, string name) throw(RSGISAttributeTableException);
        long getIntField(size_t fid, string name) throw(RSGISAttributeTableException);
        double getDoubleField(size_t fid, string name) throw(RSGISAttributeTableException);
        string getStringField(size_t fid, string name) throw(RSGISAttributeTableException);
        
        void setBoolField(size_t fid, string name, bool value) throw(RSGISAttributeTableException);
        void setIntField(size_t fid, string name, long value) throw(RSGISAttributeTableException);
        void setDoubleField(size_t fid, string name, double value) throw(RSGISAttributeTableException);
        void setStringField(size_t fid, string name, string value) throw(RSGISAttributeTableException);
        
        void setBoolValue(string name, bool value) throw(RSGISAttributeTableException);
        void setIntValue(string name, long value) throw(RSGISAttributeTableException);
        void setFloatValue(string name, double value) throw(RSGISAttributeTableException);
        void setStringValue(string name, string value) throw(RSGISAttributeTableException);
        
        RSGISFeature* getFeature(size_t fid) throw(RSGISAttributeTableException);
        void returnFeature(RSGISFeature *feat, bool sync) throw(RSGISAttributeTableException);
        
        void addAttBoolField(string name, bool val) throw(RSGISAttributeTableException);
        void addAttIntField(string name, long val) throw(RSGISAttributeTableException);
        void addAttFloatField(string name, double val) throw(RSGISAttributeTableException);
        void addAttStringField(string name, string val) throw(RSGISAttributeTableException);
        
        void addAttributes(vector<RSGISAttribute*> *attributes) throw(RSGISAttributeTableException);
        
        size_t getSize();
        
        void operator++();
        void start();
        bool end();
        RSGISFeature* operator*();
        
        ~RSGISAttributeTableMem();
        
        static RSGISFileType findFileType(string inFile)throw(RSGISAttributeTableException);
        static RSGISAttributeTable* importFromASCII(string inFile)throw(RSGISAttributeTableException);
        static RSGISAttributeTable* importFromHDF5(string inFile)throw(RSGISAttributeTableException);
        static RSGISAttributeTable* importFromGDALRaster(string inFile)throw(RSGISAttributeTableException);
    protected:
        RSGISAttributeTableMem();
        void createAttributeTable(size_t numFeatures);
        void createAttributeTableWithFields(size_t numFeatures);
        vector<RSGISFeature*> *attTable;
        size_t iterIdx;
    };

}}

#endif
