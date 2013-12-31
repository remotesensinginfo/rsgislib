/*
 *  RSGISCmdImageCalibration.h
 *
 *
 *  Created by Pete Bunting on 10/07/2013.
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

#ifndef RSGISCmdImageCalibration_H
#define RSGISCmdImageCalibration_H

#include <iostream>
#include <string>
#include <vector>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

namespace rsgis{ namespace cmds {
    
    struct CmdsLandsatRadianceGainsOffsets
    {
        std::string imagePath;
        std::string bandName;
        unsigned int band;
        float lMax;
        float lMin;
        float qCalMax;
        float qCalMin;
    };
    
    struct CmdsLandsatRadianceGainsOffsetsMultiAdd
    {
        std::string imagePath;
        std::string bandName;
        unsigned int band;
        float addVal;
        float multiVal;
    };
    
    struct Cmds6SElevationLUT
    {
        float elev;
        unsigned int *imageBands;
        float *aX;
        float *bX;
        float *cX;
        unsigned int numValues;
    };
    
    /** Function to convert DN landsat scence to radiance */
    void executeConvertLandsat2Radiance(std::string outputImage, std::string gdalFormat, std::vector<CmdsLandsatRadianceGainsOffsets> landsatRadGainOffs)throw(RSGISCmdException);
    
    /** Function to convert DN landsat scence to radiance using mutliplication and addition values */
    void executeConvertLandsat2RadianceMultiAdd(std::string outputImage, std::string gdalFormat, std::vector<CmdsLandsatRadianceGainsOffsetsMultiAdd> landsatRadGainOffs)throw(RSGISCmdException);
    
    /** Function to convert radiance into TOA reflectance */
    void executeConvertRadiance2TOARefl(std::string inputImage, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, unsigned int julianDay, bool useJulianDay, unsigned int year, unsigned int month, unsigned int day, float solarZenith, float *solarIrradiance, unsigned int numBands) throw(RSGISCmdException);
    
    /** Function to convert radiance into surface reflectance using a single parameterisation of 6S */
    void executeRad2SREFSingle6sParams(std::string inputImage, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, unsigned int *imageBands, float *aX, float *bX, float *cX, int numValues, float noDataVal, bool useNoDataVal)throw(RSGISCmdException);
    
    /** Function to convert radiance into surface reflectance using a LUT for surface elevation of 6S */
    void executeRad2SREFElevLUT6sParams(std::string inputRadImage, std::string inputDEM, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, std::vector<Cmds6SElevationLUT> *lut, float noDataVal, bool useNoDataVal)throw(RSGISCmdException);
}}


#endif

