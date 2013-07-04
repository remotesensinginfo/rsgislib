/*
 *  RSGISCmdRasterGIS.h
 *
 *
 *  Created by Pete Bunting on 03/05/2013.
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

#ifndef RSGISCmdRasterGIS_H
#define RSGISCmdRasterGIS_H

#include <iostream>
#include <string>
#include <vector>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

namespace rsgis{ namespace cmds {
    
    struct RSGISBandAttStatsCmds
    {
        unsigned int band;
        float threshold;
        bool calcCount;
        std::string countField;
        bool calcMin;
        std::string minField;
        bool calcMax;
        std::string maxField;
        bool calcMean;
        std::string meanField;
        bool calcStdDev;
        std::string stdDevField;
        bool calcMedian;
        std::string medianField;
        bool calcSum;
        std::string sumField;
    };
    
    struct RSGISBandAttPercentilesCmds
    {
        unsigned int band;
        unsigned int percentile;
        std::string fieldName;
    };
    
    /** Function to populate statics for thermatic imagess */
    void executePopulateStats(std::string clumpsImage, bool addColourTable2Img, bool calcImgPyramids)throw(RSGISCmdException);
    
    /** Function for copying a GDAL RAT from one image to anoother */
    void executeCoptRAT(std::string inputImage, std::string clumpsImage)throw(RSGISCmdException);
    
    /** Function for copying GDAL RAT columns from one image to another */
    void executeCopyGDALATTColumns(std::string inputImage, std::string clumpsImage, std::vector<std::string> fields)throw(RSGISCmdException);
   
    /** Function for adding spatial location columns to the attribute table */
    void executeSpatialLocation(std::string inputImage, std::string clumpsImage, std::string eastingsField, std::string northingsField)throw(RSGISCmdException);
    
    /** Function for populating an attribute table from an image */
    void executePopulateRATWithStats(std::string inputImage, std::string clumpsImage, std::vector<rsgis::cmds::RSGISBandAttStatsCmds*> *bandStatsCmds)throw(RSGISCmdException);
    
    /** Function for populating an attribute table with a percentile of the pixel values */
    void executePopulateRATWithPercentiles(std::string inputImage, std::string clumpsImage, std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*> *bandPercentilesCmds)throw(RSGISCmdException);
    
    /** Function for populating the attribute table with the proporations of intersecting catagories */
    void executePopulateCategoryProportions(std::string categoriesImage, std::string clumpsImage, std::string outColsName, std::string majorityColName, bool copyClassNames, std::string majClassNameField, std::string classNameField)throw(RSGISCmdException);
    
    /** Function for copying an attribute tables colour table to another table based on class column */
    void executeCopyCatagoriesColours(std::string categoriesImage, std::string clumpsImage, std::string classField)throw(RSGISCmdException);
    
    /** Function for exporting columns of the attribute table as GDAL images */
    void executeExportCols2GDALImage(std::string inputImage, std::string outputFile, std::string imageFormat, RSGISLibDataType outDataType, std::vector<std::string> fields)throw(RSGISCmdException);
    
}}


#endif

