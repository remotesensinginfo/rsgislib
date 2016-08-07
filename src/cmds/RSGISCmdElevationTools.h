/*
 *  RSGISCmdElevationTools.h
 *
 *
 *  Created by Pete Bunting on 30/04/2014.
 *  Copyright 2014 RSGISLib.
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

#ifndef RSGISCmdElevationTools_H
#define RSGISCmdElevationTools_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

namespace rsgis{ namespace cmds {
    
    enum RSGISAngleMeasure
    {
        rsgis_degrees = 0,
        rsgis_radians = 1
    };
    
    /** A function to generate a slope layer */
    DllExport void executeCalcSlope(std::string demImage, std::string outputImage, RSGISAngleMeasure outAngleUnit, std::string outImageFormat)throw(RSGISCmdException);
    /** A function to generate an aspect layer */
    DllExport void executeCalcAspect(std::string demImage, std::string outputImage, std::string outImageFormat)throw(RSGISCmdException);
    
    /** A function to categorise an aspect layer */
    DllExport void executeCatagoriseAspect(std::string aspectImage, std::string outputImage, std::string outImageFormat)throw(RSGISCmdException);
    
    /** A function to generate a hillshade layer */
    DllExport void executeCalcHillshade(std::string demImage, std::string outputImage, float solarAzimuth, float solarZenith, std::string outImageFormat)throw(RSGISCmdException);
    /** A function to generate a shadow mask layer */
    DllExport void executeCalcShadowMask(std::string demImage, std::string outputImage, float solarAzimuth, float solarZenith, float maxHeight, std::string outImageFormat)throw(RSGISCmdException);
    /** A function to generate a local incidence angle layer given the sun position */
    DllExport void executeCalcLocalIncidenceAngle(std::string demImage, std::string outputImage, float solarAzimuth, float solarZenith, std::string outImageFormat)throw(RSGISCmdException);
    /** A function to generate a local exitance angle layer given a viewers position */
    DllExport void executeCalcLocalExitanceAngle(std::string demImage, std::string outputImage, float azimuth, float zenith, std::string outImageFormat)throw(RSGISCmdException);
    /** A function to filter a DTM using a variable filter with respect to aspect */
    DllExport void executeDTMAspectMedianFilter(std::string demImage, std::string aspectImage, std::string outputImage, float aspectRange, int winHSize, std::string outImageFormat)throw(RSGISCmdException);
    
    /** A function to fill a DEM using the Soille and Gratin 1994 algorthm */
    DllExport void executeDEMFillSoilleGratin1994(std::string inImage, std::string validDataImg, std::string outputImage, std::string outImageFormat)throw(RSGISCmdException);
}}


#endif


