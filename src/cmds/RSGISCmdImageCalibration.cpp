/*
 *  RSGISCmdImageCalibration.cpp
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

#include "RSGISCmdImageCalibration.h"
#include "RSGISCmdParent.h"

#include "calibration/RSGISStandardDN2RadianceCalibration.h"
#include "calibration/RSGISCalculateTopOfAtmosphereReflectance.h"
#include "calibration/RSGISApply6SCoefficients.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

namespace rsgis{ namespace cmds {
    
    void executeConvertLandsat2Radiance(std::string outputImage, std::string gdalFormat, std::vector<CmdsLandsatRadianceGainsOffsets> landsatRadGainOffs)throw(RSGISCmdException)
    {
        GDALAllRegister();
        
        try
        {
            unsigned int numBands = landsatRadGainOffs.size();
            GDALDataset **datasets = new GDALDataset*[numBands];
            std::string *outBandNames = new std::string[numBands];
            
            rsgis::calib::LandsatRadianceGainsOffsets *lsRadGainOffs = new rsgis::calib::LandsatRadianceGainsOffsets[numBands];
            
            unsigned int i = 0;
            unsigned int numRasterBands = 0;
            unsigned int totalNumRasterBands = 0;
            for(std::vector<rsgis::cmds::CmdsLandsatRadianceGainsOffsets>::iterator iterBands = landsatRadGainOffs.begin(); iterBands != landsatRadGainOffs.end(); ++iterBands)
			{
                std::cout << "Opening: " << (*iterBands).imagePath << std::endl;
                
                datasets[i] = (GDALDataset *) GDALOpen((*iterBands).imagePath.c_str(), GA_ReadOnly);
                if(datasets[i] == NULL)
                {
                    std::string message = std::string("Could not open image ") + (*iterBands).imagePath;
                    throw RSGISImageException(message.c_str());
                }
                
                numRasterBands = datasets[i]->GetRasterCount();
                
                if((*iterBands).band > numRasterBands)
                {
                    throw RSGISImageException("You have specified a band which is not within the image");
                }
                lsRadGainOffs[i].band = totalNumRasterBands + (*iterBands).band-1;
                
                lsRadGainOffs[i].lMax = (*iterBands).lMax;
                lsRadGainOffs[i].lMin = (*iterBands).lMin;
                lsRadGainOffs[i].qCalMax = (*iterBands).qCalMax;
                lsRadGainOffs[i].qCalMin = (*iterBands).qCalMin;
                
                outBandNames[i] = (*iterBands).bandName;
                
                totalNumRasterBands += numRasterBands;
                ++i;
			}
            
            rsgis::calib::RSGISLandsatRadianceCalibration *radianceCalibration = new rsgis::calib::RSGISLandsatRadianceCalibration(numBands, lsRadGainOffs);
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(radianceCalibration, "", true);
 
            calcImage->calcImage(datasets, numBands, outputImage, true, outBandNames, gdalFormat);
            
            for(unsigned int i = 0; i < numBands; ++i)
            {
                GDALClose(datasets[i]);
            }
            
            delete[] datasets;
            delete[] lsRadGainOffs;
            delete[] outBandNames;
            
            delete radianceCalibration;
            delete calcImage;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeConvertLandsat2RadianceMultiAdd(std::string outputImage, std::string gdalFormat, std::vector<CmdsLandsatRadianceGainsOffsetsMultiAdd> landsatRadGainOffs)throw(RSGISCmdException)
    {
        GDALAllRegister();
        
        try
        {
            unsigned int numBands = landsatRadGainOffs.size();
            GDALDataset **datasets = new GDALDataset*[numBands];
            std::string *outBandNames = new std::string[numBands];
            
            rsgis::calib::LandsatRadianceGainsOffsetsMultiAdd *lsRadGainOffs = new rsgis::calib::LandsatRadianceGainsOffsetsMultiAdd[numBands];
            
            unsigned int i = 0;
            unsigned int numRasterBands = 0;
            unsigned int totalNumRasterBands = 0;
            for(std::vector<rsgis::cmds::CmdsLandsatRadianceGainsOffsetsMultiAdd>::iterator iterBands = landsatRadGainOffs.begin(); iterBands != landsatRadGainOffs.end(); ++iterBands)
			{
                std::cout << "Opening: " << (*iterBands).imagePath << std::endl;
                
                datasets[i] = (GDALDataset *) GDALOpen((*iterBands).imagePath.c_str(), GA_ReadOnly);
                if(datasets[i] == NULL)
                {
                    std::string message = std::string("Could not open image ") + (*iterBands).imagePath;
                    throw RSGISImageException(message.c_str());
                }
                
                numRasterBands = datasets[i]->GetRasterCount();
                
                if((*iterBands).band > numRasterBands)
                {
                    throw RSGISImageException("You have specified a band which is not within the image");
                }
                lsRadGainOffs[i].band = totalNumRasterBands + (*iterBands).band-1;
                
                lsRadGainOffs[i].addVal = (*iterBands).addVal;
                lsRadGainOffs[i].multiVal = (*iterBands).multiVal;
                
                outBandNames[i] = (*iterBands).bandName;
                
                totalNumRasterBands += numRasterBands;
                ++i;
			}
            
            rsgis::calib::RSGISLandsatRadianceCalibrationMultiAdd *radianceCalibration = new rsgis::calib::RSGISLandsatRadianceCalibrationMultiAdd(numBands, lsRadGainOffs);
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(radianceCalibration, "", true);
            
            calcImage->calcImage(datasets, numBands, outputImage, true, outBandNames, gdalFormat);
            
            for(unsigned int i = 0; i < numBands; ++i)
            {
                GDALClose(datasets[i]);
            }
            
            delete[] datasets;
            delete[] lsRadGainOffs;
            delete[] outBandNames;
            
            delete radianceCalibration;
            delete calcImage;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeConvertRadiance2TOARefl(std::string inputImage, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, unsigned int julianDay, bool useJulianDay, unsigned int year, unsigned int month, unsigned int day, float solarZenith, float *solarIrradiance, unsigned int numBands) throw(RSGISCmdException)
    {
        GDALAllRegister();
        GDALDataset **datasets = NULL;
        rsgis::calib::RSGISCalculateTopOfAtmosphereReflectance *calcTopAtmosRefl = NULL;
        rsgis::img::RSGISCalcImage *calcImage = NULL;
        
        try
        {
            datasets = new GDALDataset*[1];
            
            std::cout << "Open " << inputImage << std::endl;
            datasets[0] = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            unsigned int numRasterBands = datasets[0]->GetRasterCount();
            if(numBands != numRasterBands)
            {
                GDALClose(datasets[0]);
                delete[] datasets;
                throw rsgis::RSGISException("The number of input image bands and solar irradiance values are different.");
            }
            
            double solarDistance = 0;
            
            solarDistance = rsgis::calib::rsgisCalcSolarDistance(julianDay);

            
            calcTopAtmosRefl = new rsgis::calib::RSGISCalculateTopOfAtmosphereReflectance(numRasterBands, solarIrradiance, solarDistance, solarZenith, scaleFactor);
            
            calcImage = new rsgis::img::RSGISCalcImage(calcTopAtmosRefl, "", true);
            calcImage->calcImage(datasets, 1, outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(rsgisOutDataType));
            
            GDALClose(datasets[0]);
            delete[] datasets;
            
            delete calcTopAtmosRefl;
            delete calcImage;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeRad2SREFSingle6sParams(std::string inputImage, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, unsigned int *imageBands, float *aX, float *bX, float *cX, int numValues, float noDataVal, bool useNoDataVal)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset **datasets = new GDALDataset*[1];
            std::cout << "Open image" << inputImage << std::endl;
            datasets[0] = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            int numRasterBands = datasets[0]->GetRasterCount();
            if(numValues != numRasterBands)
            {
                GDALClose(datasets[0]);
                delete[] datasets;
                throw rsgis::RSGISException("The number of input image bands is not equal to the number coefficients provided.");
            }
            
            for(int i = 0; i < numValues; ++i)
            {
                imageBands[i] -= 1;
            }
            
            rsgis::calib::RSGISApply6SCoefficientsSingleParam *apply6SCoefficients = new rsgis::calib::RSGISApply6SCoefficientsSingleParam(imageBands, aX, bX, cX, numValues, noDataVal, useNoDataVal, scaleFactor);
            
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(apply6SCoefficients, "", true);
            calcImage->calcImage(datasets, 1, outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(rsgisOutDataType));
            
            delete apply6SCoefficients;
            delete calcImage;
            
            GDALClose(datasets[0]);
            delete[] datasets;            
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
        
    }
                
                
    void executeRad2SREFElevLUT6sParams(std::string inputRadImage, std::string inputDEM, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, std::vector<Cmds6SElevationLUT> *lut, float noDataVal, bool useNoDataVal)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset **datasets = new GDALDataset*[2];
            std::cout << "Open DEM image: \'" << inputDEM << "\'" << std::endl;
            datasets[0] = (GDALDataset *) GDALOpen(inputDEM.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputDEM;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            std::cout << "Open Radiance image: \'" << inputRadImage << "\'" << std::endl;
            datasets[1] = (GDALDataset *) GDALOpen(inputRadImage.c_str(), GA_ReadOnly);
            if(datasets[1] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputRadImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            int numRasterBands = datasets[1]->GetRasterCount();
            
            std::vector<rsgis::calib::LUT6SElevation> *rsgisLUT = new std::vector<rsgis::calib::LUT6SElevation>();
            
            for(std::vector<Cmds6SElevationLUT>::iterator iterLUT = lut->begin(); iterLUT != lut->end(); ++iterLUT)
            {
                rsgis::calib::LUT6SElevation lutVal = rsgis::calib::LUT6SElevation();
                std::cout << "Elevation: " << (*iterLUT).elev << std::endl;
                
                lutVal.numValues = (*iterLUT).numValues;
                lutVal.elev = (*iterLUT).elev;
                lutVal.imageBands = new unsigned int[lutVal.numValues];
                lutVal.aX = new float[lutVal.numValues];
                lutVal.bX = new float[lutVal.numValues];
                lutVal.cX = new float[lutVal.numValues];
                
                
                for(unsigned int i = 0; i < (*iterLUT).numValues; ++i)
                {
                    if((*iterLUT).imageBands[i] > numRasterBands)
                    {
                        GDALClose(datasets[0]);
                        GDALClose(datasets[1]);
                        delete[] datasets;
                        throw rsgis::RSGISException("The number of input image bands is not equal to the number coefficients provided.");
                    }
                    lutVal.imageBands[i] = (*iterLUT).imageBands[i];
                    lutVal.aX[i] = (*iterLUT).aX[i];
                    lutVal.bX[i] = (*iterLUT).bX[i];
                    lutVal.cX[i] = (*iterLUT).cX[i];
                    std::cout << "\tBand " << lutVal.imageBands[i] << ": aX = " << lutVal.aX[i] << " bX = " << lutVal.bX[i] << " cX = " << lutVal.cX[i] << std::endl;
                }
                
                rsgisLUT->push_back(lutVal);
            }
            
            std::cout << "Apply Coefficients to input image...\n";

            rsgis::calib::RSGISApply6SCoefficientsElevLUTParam *apply6SCoefficients = new rsgis::calib::RSGISApply6SCoefficientsElevLUTParam(numRasterBands, rsgisLUT, noDataVal, useNoDataVal, scaleFactor);
            
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(apply6SCoefficients, "", true);
            calcImage->calcImage(datasets, 2, outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(rsgisOutDataType));
            
            delete apply6SCoefficients;
            delete calcImage;
            
            for(std::vector<rsgis::calib::LUT6SElevation>::iterator iterLUT = rsgisLUT->begin(); iterLUT != rsgisLUT->end(); ++iterLUT)
            {
                delete[] (*iterLUT).imageBands;
                delete[] (*iterLUT).aX;
                delete[] (*iterLUT).bX;
                delete[] (*iterLUT).cX;
            }
            delete rsgisLUT;
            
            GDALClose(datasets[0]);
            GDALClose(datasets[1]);
            delete[] datasets;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
}}



