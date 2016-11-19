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
    
    struct DllExport CmdsLandsatRadianceGainsOffsets
    {
        std::string imagePath;
        std::string bandName;
        unsigned int band;
        float lMax;
        float lMin;
        float qCalMax;
        float qCalMin;
    };
    
    struct DllExport CmdsSaturatedPixel
    {
        std::string imagePath;
        std::string bandName;
        unsigned int band;
        float satVal;
    };
    
    struct DllExport CmdsLandsatThermalCoeffs
    {
        std::string bandName;
        unsigned int band;
        float k1;
        float k2;
    };
    
    struct DllExport CmdsLandsatRadianceGainsOffsetsMultiAdd
    {
        std::string imagePath;
        std::string bandName;
        unsigned int band;
        float addVal;
        float multiVal;
    };
    
    struct DllExport CmdsSPOTRadianceGainsOffsets
    {
        std::string bandName;
        unsigned int band;
        float bias;
        float gain;
    };
    
    struct DllExport Cmds6SElevationLUT
    {
        float elev;
        unsigned int *imageBands;
        float *aX;
        float *bX;
        float *cX;
        unsigned int numValues;
    };
    
    struct DllExport Cmds6SAOTLUT
    {
        float aot;
        unsigned int *imageBands;
        float *aX;
        float *bX;
        float *cX;
        unsigned int numValues;
    };
    
    struct DllExport Cmds6SBaseElevAOTLUT
    {
        float elev;
        std::vector<Cmds6SAOTLUT> aotLUT;
    };
    
    struct DllExport CmdsWorldView2RadianceGainsOffsets
    {
        std::string bandName;
        unsigned int band;
        double absCalFact;
        double effBandWidth;
    };
    
    /** Function to convert DN landsat scence to radiance */
    DllExport void executeConvertLandsat2Radiance(std::string outputImage, std::string gdalFormat, std::vector<CmdsLandsatRadianceGainsOffsets> landsatRadGainOffs)throw(RSGISCmdException);
    
    /** Function to convert DN landsat scence to radiance using mutliplication and addition values */
    DllExport void executeConvertLandsat2RadianceMultiAdd(std::string outputImage, std::string gdalFormat, std::vector<CmdsLandsatRadianceGainsOffsetsMultiAdd> landsatRadGainOffs)throw(RSGISCmdException);
    
    /** Function to convert DN WorldView2 scence to radiance */
    DllExport void executeConvertWorldView2ToRadiance(std::string inputImage, std::string outputImage, std::string gdalFormat, std::vector<CmdsWorldView2RadianceGainsOffsets> wv2RadGainOffs)throw(RSGISCmdException);
    
    /** Function to convert DN SPOT5 scence to radiance */
    DllExport void executeConvertSPOT5ToRadiance(std::string inputImage, std::string outputImage, std::string gdalFormat, std::vector<CmdsSPOTRadianceGainsOffsets> spot5RadGainOffs)throw(RSGISCmdException);
    
    /** Function to convert radiance into TOA reflectance */
    DllExport void executeConvertRadiance2TOARefl(std::string inputImage, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, unsigned int julianDay, bool useJulianDay, unsigned int year, unsigned int month, unsigned int day, float solarZenith, float *solarIrradiance, unsigned int numBands) throw(RSGISCmdException);
    
    /** Function to convert radiance into surface reflectance using a single parameterisation of 6S */
    DllExport void executeRad2SREFSingle6sParams(std::string inputImage, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, unsigned int *imageBands, float *aX, float *bX, float *cX, int numValues, float noDataVal, bool useNoDataVal)throw(RSGISCmdException);
    
    /** Function to convert radiance into surface reflectance using a LUT for surface elevation of 6S */
    DllExport void executeRad2SREFElevLUT6sParams(std::string inputRadImage, std::string inputDEM, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, std::vector<Cmds6SElevationLUT> *lut, float noDataVal, bool useNoDataVal)throw(RSGISCmdException);
    
    /** Function to convert radiance into surface reflectance using a LUT for surface elevation and AOT of 6S */
    DllExport void executeRad2SREFElevAOTLUT6sParams(std::string inputRadImage, std::string inputDEM, std::string inputAOTImg, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, std::vector<Cmds6SBaseElevAOTLUT> *lut, float noDataVal, bool useNoDataVal)throw(RSGISCmdException);
    
    /** Function to apply an offset image within the context of dark object subtraction */
    DllExport void executeApplySubtractOffsets(std::string inputImage, std::string outputImage, std::string offsetImage, bool nonNegative, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float noDataVal, bool useNoDataVal, float darkObjReflVal) throw(RSGISCmdException);
    
    /** Function to convert thermal radiance to thermal brightness for Landsat */
    DllExport void executeLandsatThermalRad2ThermalBrightness(std::string inputImage, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, std::vector<CmdsLandsatThermalCoeffs> landsatThermalCoeffs) throw(RSGISCmdException);
    
    /** Function to generate a per-band image band mask of the saturated image pixels */
    DllExport void executeGenerateSaturationMask(std::string outputImage, std::string gdalFormat, std::vector<CmdsSaturatedPixel> imgBandInfo)throw(RSGISCmdException);
    
    /** Function to apply the FMask algorithm for classifying cloud for Landsat TM and ETM+ data */
    DllExport void executeLandsatTMCloudFMask(std::string inputTOAImage, std::string inputThermalImage, std::string inputSaturateImage, std::string validImg, std::string outputImage, std::string gdalFormat, double sunAz, double sunZen, double senAz, double senZen, float whitenessThreshold, float scaleFactorIn, std::string tmpImgsBase, std::string tmpImgFileExt, bool rmTmpImgs=true) throw(RSGISCmdException);
    
    /** Function to apply DOS offsets (per band) to the input image */
    DllExport void executeApplySubtractSingleOffsets(std::string inputImage, std::string outputImage, std::vector<double> offsetValues, bool nonNegative, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float noDataVal, bool useNoDataVal, float darkObjReflVal) throw(RSGISCmdException);
    
    /** Function to calculate the view angle across the swath of a nadir input image */
    DllExport void executeCalcNadirImageViewAngle(std::string imgFootprint, std::string outViewAngleImg, std::string gdalFormat, double sateAltitude, std::string minXXCol, std::string minXYCol, std::string maxXXCol, std::string maxXYCol, std::string minYXCol, std::string minYYCol, std::string maxYXCol, std::string maxYYCol) throw(RSGISCmdException);
    
    

    
}}


#endif

