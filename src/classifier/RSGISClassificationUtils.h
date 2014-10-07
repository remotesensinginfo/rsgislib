/*
 *  RSGISClassificationUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/11/2008.
 *  Copyright 2008 RSGISLib.
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


#ifndef RSGISClassificationUtils_H
#define RSGISClassificationUtils_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "common/RSGISClassificationException.h"

#include "utils/RSGIS_ENVI_ASCII_ROI.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISMatrices.h"

#include "vec/RSGISVectorUtils.h"

#include "classifier/RSGISClassifier.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"

#include "datastruct/SortedGenericList.cpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include "ogrsf_frmts.h"
#include "ogr_api.h"

namespace rsgis{ namespace classifier{
        
	class DllExport RSGISClassificationUtils
    {
    public:
        RSGISClassificationUtils();
        ClassData* convertROIs2ClassData(rsgis::utils::enviroi *enviroi, int id);
        ClassData** parseClassificationInputFile(std::string inputFile, int *numClasses) throw(RSGISClassificationException, rsgis::RSGISInputStreamException, rsgis::math::RSGISMatricesException);
        void convertShapeFile2SpecLib(std::string vector, std::string outputFile, std::string classAttribute, std::vector<std::string> *attributes, bool group) throw(RSGISClassificationException);
        ~RSGISClassificationUtils();
    };
    
    class DllExport RSGISEliminateSingleClassPixels
    {
    public:
        RSGISEliminateSingleClassPixels();
        void eliminate(GDALDataset *inImageData, GDALDataset *tmpData, std::string outputImage, float noDataVal, bool noDataValProvided, std::string format, rsgis::img::RSGISRasterConnectivity filterConnectivity)throw(rsgis::img::RSGISImageCalcException);
        ~RSGISEliminateSingleClassPixels();
    private:
        unsigned long findSinglePixelsConnect4(GDALDataset *inImageData, GDALDataset *tmpData, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException);
        bool eliminateSinglePixelsConnect4(GDALDataset *inImageData, GDALDataset *tmpData, GDALDataset *outDataset, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException);
        unsigned long findSinglePixelsConnect8(GDALDataset *inImageData, GDALDataset *tmpData, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException);
        bool eliminateSinglePixelsConnect8(GDALDataset *inImageData, GDALDataset *tmpData, GDALDataset *outDataset, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException);
        unsigned int findMostCommonVal(std::vector<unsigned int> values);
    };
	
}}

#endif


