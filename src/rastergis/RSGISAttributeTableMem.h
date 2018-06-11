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

    class RSGISAttributeTableMem : public RSGISAttributeTable
    {
    public:        
        RSGISAttributeTableMem(size_t numFeatures);
        RSGISAttributeTableMem(size_t numFeatures, std::vector<std::pair<std::string, RSGISAttributeDataType> > *fields);
        
        bool getBoolField(size_t fid, std::string name) throw(RSGISAttributeTableException);
        long getIntField(size_t fid, std::string name) throw(RSGISAttributeTableException);
        double getDoubleField(size_t fid, std::string name) throw(RSGISAttributeTableException);
        std::string getStringField(size_t fid, std::string name) throw(RSGISAttributeTableException);
        
        void setBoolField(size_t fid, std::string name, bool value) throw(RSGISAttributeTableException);
        void setIntField(size_t fid, std::string name, long value) throw(RSGISAttributeTableException);
        void setDoubleField(size_t fid, std::string name, double value) throw(RSGISAttributeTableException);
        void setStringField(size_t fid, std::string name, std::string value) throw(RSGISAttributeTableException);
        
        void setBoolValue(std::string name, bool value) throw(RSGISAttributeTableException);
        void setIntValue(std::string name, long value) throw(RSGISAttributeTableException);
        void setFloatValue(std::string name, double value) throw(RSGISAttributeTableException);
        void setStringValue(std::string name, std::string value) throw(RSGISAttributeTableException);
        
        RSGISFeature* getFeature(size_t fid) throw(RSGISAttributeTableException);
        void flushAllFeatures(bool progressFeedback=false) throw(RSGISAttributeTableException);
        
        void addAttBoolField(std::string name, bool val) throw(RSGISAttributeTableException);
        void addAttIntField(std::string name, long val) throw(RSGISAttributeTableException);
        void addAttFloatField(std::string name, double val) throw(RSGISAttributeTableException);
        void addAttStringField(std::string name, std::string val) throw(RSGISAttributeTableException);
        
        void addAttributes(std::vector<RSGISAttribute*> *attributes) throw(RSGISAttributeTableException);
        
        size_t getSize();
        void holdFID(size_t fid);
        void removeHoldFID(size_t fid);
        size_t getNumOfBlocks();
        void loadBlocks(size_t startBlock, size_t endBlock) throw(RSGISAttributeTableException);
        bool attInMemory();
        void findFIDRangeInNeighbours(size_t startFID, size_t endFID, size_t *minFID, size_t *maxFID) throw(RSGISAttributeTableException);
        
        void operator++();
        void start();
        bool end();
        RSGISFeature* operator*();
        
        ~RSGISAttributeTableMem();
        
        static RSGISFileType findFileType(std::string inFile)throw(RSGISAttributeTableException);
        static RSGISAttributeTable* importFromASCII(std::string inFile)throw(RSGISAttributeTableException);
        static RSGISAttributeTable* importFromHDF5(std::string inFile)throw(RSGISAttributeTableException);
        static RSGISAttributeTable* importFromGDALRaster(std::string inFile)throw(RSGISAttributeTableException);
    protected:
        RSGISAttributeTableMem();
        void createAttributeTable(size_t numFeatures);
        void createAttributeTableWithFields(size_t numFeatures);
        std::vector<RSGISFeature*> *attTable;
        size_t iterIdx;
    };

}}

#endif