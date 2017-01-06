/*
 *  RSGISGenAccuracyPoints.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/11/2012.
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

#ifndef RSGISGenAccuracyPoints_H
#define RSGISGenAccuracyPoints_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <utility>

#include "gdal_priv.h"
#include "gdal_rat.h"
#include "ogrsf_frmts.h"

#include "common/RSGISImageException.h"
#include "utils/RSGISTextUtils.h"

#include "rastergis/RSGISRasterAttUtils.h"

#include <boost/algorithm/string/trim_all.hpp>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_classify_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace classifier{
	
    enum AccPtsType
	{
		rsgis_randompts,
        rsgis_stratifiedpts
	};
    
    struct DllExport RSGISAccPoint
    {
        unsigned int ptID;
        double eastings;
        double northings;
        float elevation;
        std::string mapClassName;
        std::string trueClassName;
        int status;
        std::string comment;
    };
    
    inline bool compareMapClass(RSGISAccPoint *first, RSGISAccPoint *second)
    {
        unsigned int i=0;
        while( (i < first->mapClassName.length()) && (i < second->mapClassName.length()))
        {
            if (tolower(first->mapClassName[i]) < tolower(second->mapClassName[i]))
            {
                return true;
            }
            else if (tolower(first->mapClassName[i]) > tolower(second->mapClassName[i]))
            {
                return false;
            }
            ++i;
        }
        if(first->mapClassName.length() < second->mapClassName.length())
        {
            return true;
        }
        else
        {
            return false;
        }
    };
    
	class DllExport RSGISGenAccuracyPoints
    {
    public:
        RSGISGenAccuracyPoints();
        void generateRandomPoints(GDALDataset *inputImage, GDALDataset *inputDEM, bool demProvided, std::string outputFile, std::string classColName, unsigned int numPts, unsigned int seed) throw(rsgis::RSGISImageException);
        void generateStratifiedRandomPoints(GDALDataset *inputImage, GDALDataset *inputDEM, bool demProvided, std::string outputFile, std::string classColName, unsigned int numPts, unsigned int seed) throw(rsgis::RSGISImageException);
        
        void generateRandomPointsVecOut(GDALDataset *inputImage, OGRLayer *outputSHPLayer, std::string imgClassCol, std::string vecClassImgCol, std::string vecClassRefCol, unsigned int numPts, unsigned int seed) throw(rsgis::RSGISImageException);
        void generateStratifiedRandomPointsVecOut(GDALDataset *inputImage, OGRLayer *outputSHPLayer, std::string imgClassCol, std::string vecClassImgCol, std::string vecClassRefCol, unsigned int numPts, unsigned int seed) throw(rsgis::RSGISImageException);
        void popClassInfo2Vec(GDALDataset *inputImage, OGRLayer *outputSHPLayer, std::string imgClassCol, std::string vecClassImgCol, std::string vecClassRefCol, bool addRefCol)throw(rsgis::RSGISImageException);
        
        ~RSGISGenAccuracyPoints();
    protected:
        float findPixelVal(GDALDataset *image, unsigned int band, double eastings, double northings, double tlX, double tlY, double xRes, double yRes, unsigned int xSize, unsigned int ySize) throw(rsgis::RSGISImageException);
        std::string findClassVal(GDALDataset *image, unsigned int band, GDALRasterAttributeTable *attTable, unsigned int classNameColIdx, unsigned int xPxl, unsigned int yPxl) throw(rsgis::RSGISImageException);
        std::list<std::string>* findUniqueClasses(GDALRasterAttributeTable *attTable, unsigned int classNameColIdx, int histoColIdx) throw(rsgis::RSGISImageException);
    };
		
}}

#endif


