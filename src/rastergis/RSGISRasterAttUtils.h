/*
 *  RSGISRasterAttUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/08/2012.
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

#ifndef RSGISRasterAttUtils_H
#define RSGISRasterAttUtils_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <math.h>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"

#include "utils/RSGISColour.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

namespace rsgis{namespace rastergis{
	
    class RSGISRasterAttUtils
    {
    public:
        RSGISRasterAttUtils();
        void copyAttColumns(GDALDataset *inImage, GDALDataset *outImage, std::vector<std::string> fields) throw(RSGISAttributeTableException);
        void copyColourForCats(GDALDataset *catsImage, GDALDataset *classImage, std::string classField) throw(RSGISAttributeTableException);
        void exportColumns2ASCII(GDALDataset *inImage, std::string outputFile, std::vector<std::string> fields) throw(RSGISAttributeTableException);
        void translateClasses(GDALDataset *inImage, std::string classInField, std::string classOutField, std::map<size_t, size_t> classPairs) throw(RSGISAttributeTableException);
        void applyClassColours(GDALDataset *inImage, std::string classInField, std::map<size_t, rsgis::utils::RSGISColourInt> classColoursPairs) throw(RSGISAttributeTableException);
        void applyClassStrColours(GDALDataset *inImage, std::string classInField, std::map<std::string, rsgis::utils::RSGISColourInt> classColoursPairs) throw(RSGISAttributeTableException);
        unsigned int findColumnIndex(const GDALRasterAttributeTable *gdalATT, std::string colName) throw(RSGISAttributeTableException);
        unsigned int findColumnIndexOrCreate(GDALRasterAttributeTable *gdalATT, std::string colName, GDALRATFieldType dType) throw(RSGISAttributeTableException);
        ~RSGISRasterAttUtils();
    };
	
}}

#endif

