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
#include "calibration/RSGISApplySubtractOffsets.h"
#include "calibration/RSGISCloudMasking.h"
#include "calibration/RSGISHydroDEMFillSoilleGratin94.h"
#include "calibration/RSGISImgCalibUtils.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcEditImage.h"
#include "img/RSGISCopyImage.h"
#include "img/RSGISImageStatistics.h"

#include "filtering/RSGISMorphologyDilate.h"

#include "segmentation/RSGISClumpPxls.h"
#include "segmentation/RSGISEliminateSmallClumps.h"

#include "math/RSGISMatrices.h"
#include "math/RSGISMathsUtils.h"

#include "rastergis/RSGISCalcImageStatsAndPyramids.h"
#include "rastergis/RSGISPopRATWithStats.h"
#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISExportColumns2Image.h"

namespace rsgis{ namespace cmds {
    
    void executeConvertLandsat2Radiance(std::string outputImage, std::string gdalFormat, std::vector<CmdsLandsatRadianceGainsOffsets> landsatRadGainOffs)
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
    
    void executeConvertLandsat2RadianceMultiAdd(std::string outputImage, std::string gdalFormat, std::vector<CmdsLandsatRadianceGainsOffsetsMultiAdd> landsatRadGainOffs)
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
    
    void executeConvertRadiance2TOARefl(std::string inputImage, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, unsigned int julianDay, bool useJulianDay, unsigned int year, unsigned int month, unsigned int day, float solarZenith, float *solarIrradiance, unsigned int numBands) 
    {
        GDALAllRegister();
        try
        {
            std::cout << "Open " << inputImage << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            unsigned int numRasterBands = dataset->GetRasterCount();
            if(numBands != numRasterBands)
            {
                GDALClose(dataset);
                throw rsgis::RSGISException("The number of input image bands and solar irradiance values are different.");
            }
            
            double solarDistance = 0;
            if(!useJulianDay)
            {
                julianDay = rsgis::calib::rsgisGetJulianDay(day, month, year);
            }
            solarDistance = rsgis::calib::rsgisCalcSolarDistance(julianDay);
            
            rsgis::calib::RSGISCalculateTopOfAtmosphereReflectance calcTopAtmosRefl = rsgis::calib::RSGISCalculateTopOfAtmosphereReflectance(numRasterBands, solarIrradiance, solarDistance, solarZenith, scaleFactor);
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(&calcTopAtmosRefl, "", true);
            calcImage.calcImage(&dataset, 1, outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(rsgisOutDataType));
            
            GDALClose(dataset);
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
                
    void executeConvertTOARefl2Radiance(std::vector<std::string> inputImages, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, float solarDistance, float solarZenith, float *solarIrradiance, unsigned int numBands) 
    {
        GDALAllRegister();
        try
        {
            if(inputImages.size() == 1)
            {
                std::string inputImage = inputImages.at(1);
                std::cout << "Open " << inputImage << std::endl;
                GDALDataset *dataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImage;
                    throw rsgis::RSGISImageException(message.c_str());
                }
                
                unsigned int numRasterBands = dataset->GetRasterCount();
                if(numBands != numRasterBands)
                {
                    GDALClose(dataset);
                    throw rsgis::RSGISException("The number of input image bands and solar irradiance values are different.");
                }
                
                rsgis::calib::RSGISCalculateRadianceFromTOAReflectance calcRadFromTOARefl = rsgis::calib::RSGISCalculateRadianceFromTOAReflectance(numRasterBands, solarIrradiance, solarDistance, solarZenith, scaleFactor);
                rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(&calcRadFromTOARefl, "", true);
                calcImage.calcImage(&dataset, 1, outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(rsgisOutDataType));
                
                GDALClose(dataset);
            }
            else if(inputImages.size() > 1)
            {
                if(inputImages.size() != numBands)
                {
                    throw RSGISException("There must be either a single multi-band image or the number of ESUN values must equal the number of input images.");
                }
                
                unsigned int nImgsBands = inputImages.size();
                GDALDataset **datasets = new GDALDataset*[nImgsBands];
                for(unsigned int i = 0; i < nImgsBands; ++i)
                {
                    datasets[i] = (GDALDataset *) GDALOpen(inputImages.at(i).c_str(), GA_ReadOnly);
                    
                    if(datasets[i] == NULL)
                    {
                        std::string message = std::string("Could not open image ") + inputImages.at(i);
                        throw rsgis::RSGISImageException(message.c_str());
                    }
                    
                    if(datasets[i]->GetRasterCount() != 1)
                    {
                        throw RSGISException("Each input image can only have a single image band unless only a single image is provided.");
                    }
                }
                
                rsgis::calib::RSGISCalculateRadianceFromTOAReflectance calcTopAtmosRefl = rsgis::calib::RSGISCalculateRadianceFromTOAReflectance(nImgsBands, solarIrradiance, solarDistance, solarZenith, scaleFactor);
                rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(&calcTopAtmosRefl, "", true);
                calcImage.calcImage(datasets, nImgsBands, outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(rsgisOutDataType));
                for(unsigned int i = 0; i < nImgsBands; ++i)
                {
                    GDALClose(datasets[i]);
                }
                delete[] datasets;
            }
            else
            {
                throw RSGISException("No input images where provided.");
            }
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
                
    void executeRad2SREFSingle6sParams(std::string inputImage, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, unsigned int *imageBands, float *aX, float *bX, float *cX, int numValues, float noDataVal, bool useNoDataVal)
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
                
    void executeRad2SREFElevLUT6sParams(std::string inputRadImage, std::string inputDEM, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, std::vector<Cmds6SElevationLUT> *lut, float noDataVal, bool useNoDataVal)
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
            
            double demNoDataVal = 0.0;
            int demNoDataValAvail = false;
            demNoDataVal = datasets[0]->GetRasterBand(1)->GetNoDataValue(&demNoDataValAvail);
            if(!demNoDataValAvail)
            {
                GDALClose(datasets[0]);
                delete[] datasets;
                throw rsgis::RSGISException("The DEM image file does not have a no data value defined. ");
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
            rsgis::calib::RSGISApply6SCoefficientsElevLUTParam *apply6SCoefficients = new rsgis::calib::RSGISApply6SCoefficientsElevLUTParam(numRasterBands, rsgisLUT, demNoDataVal, noDataVal, useNoDataVal, scaleFactor);
            
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
                
    void executeRad2SREFElevAOTLUT6sParams(std::string inputRadImage, std::string inputDEM, std::string inputAOTImg, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, std::vector<Cmds6SBaseElevAOTLUT> *lut, float noDataVal, bool useNoDataVal)
    {
        try
        {
            GDALAllRegister();
            GDALDataset **datasets = new GDALDataset*[3];
            std::cout << "Open DEM image: \'" << inputDEM << "\'" << std::endl;
            datasets[0] = (GDALDataset *) GDALOpen(inputDEM.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputDEM;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            std::cout << "Open AOT image: \'" << inputAOTImg << "\'" << std::endl;
            datasets[1] = (GDALDataset *) GDALOpen(inputAOTImg.c_str(), GA_ReadOnly);
            if(datasets[1] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputAOTImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            std::cout << "Open Radiance image: \'" << inputRadImage << "\'" << std::endl;
            datasets[2] = (GDALDataset *) GDALOpen(inputRadImage.c_str(), GA_ReadOnly);
            if(datasets[2] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputRadImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            int numRasterBands = datasets[2]->GetRasterCount();
            
            std::vector<rsgis::calib::LUT6SBaseElevAOT> *rsgisLUT = new std::vector<rsgis::calib::LUT6SBaseElevAOT>();
            
            for(std::vector<Cmds6SBaseElevAOTLUT>::iterator iterElevLUT = lut->begin(); iterElevLUT != lut->end(); ++iterElevLUT)
            {
                rsgis::calib::LUT6SBaseElevAOT lutElevVal = rsgis::calib::LUT6SBaseElevAOT();
                std::cout << "Elevation: " << (*iterElevLUT).elev << std::endl;
                lutElevVal.aotLUT = std::vector<rsgis::calib::LUT6SAOT>();
                lutElevVal.aotLUT.reserve((*iterElevLUT).aotLUT.size());
                
                for(std::vector<Cmds6SAOTLUT>::iterator iterAOTLUT = (*iterElevLUT).aotLUT.begin(); iterAOTLUT != (*iterElevLUT).aotLUT.end(); ++iterAOTLUT)
                {
                    rsgis::calib::LUT6SAOT aotLUTVal = rsgis::calib::LUT6SAOT();
                    aotLUTVal.aot = (*iterAOTLUT).aot;
                    aotLUTVal.numValues = (*iterAOTLUT).numValues;
                    aotLUTVal.imageBands = new unsigned int[aotLUTVal.numValues];
                    aotLUTVal.aX = new float[aotLUTVal.numValues];
                    aotLUTVal.bX = new float[aotLUTVal.numValues];
                    aotLUTVal.cX = new float[aotLUTVal.numValues];
                    
                    std::cout << "\tAOT: " << (*iterAOTLUT).aot << std::endl;
                    for(unsigned int i = 0; i < (*iterAOTLUT).numValues; ++i)
                    {
                        if((*iterAOTLUT).imageBands[i] > numRasterBands)
                        {
                            GDALClose(datasets[0]);
                            GDALClose(datasets[1]);
                            GDALClose(datasets[2]);
                            delete[] datasets;
                            throw rsgis::RSGISException("The number of input image bands is not equal to the number coefficients provided.");
                        }
                        aotLUTVal.imageBands[i] = (*iterAOTLUT).imageBands[i]+1;
                        aotLUTVal.aX[i] = (*iterAOTLUT).aX[i];
                        aotLUTVal.bX[i] = (*iterAOTLUT).bX[i];
                        aotLUTVal.cX[i] = (*iterAOTLUT).cX[i];
                        std::cout << "\t\tBand " << aotLUTVal.imageBands[i] << ": aX = " << aotLUTVal.aX[i] << " bX = " << aotLUTVal.bX[i] << " cX = " << aotLUTVal.cX[i] << std::endl;
                    }
                    
                    lutElevVal.aotLUT.push_back(aotLUTVal);
                }
                lutElevVal.elev = (*iterElevLUT).elev;
                
                rsgisLUT->push_back(lutElevVal);
            }
            
            std::cout << "Apply Coefficients to input image...\n";
            
            rsgis::calib::RSGISApply6SCoefficientsElevAOTLUTParam *apply6SCoefficients = new rsgis::calib::RSGISApply6SCoefficientsElevAOTLUTParam(numRasterBands, rsgisLUT, noDataVal, useNoDataVal, scaleFactor);
            
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(apply6SCoefficients, "", true);
            calcImage->calcImage(datasets, 3, outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(rsgisOutDataType));
            
            delete apply6SCoefficients;
            delete calcImage;
            
            for(std::vector<rsgis::calib::LUT6SBaseElevAOT>::iterator iterLUT = rsgisLUT->begin(); iterLUT != rsgisLUT->end(); ++iterLUT)
            {
                for(std::vector<rsgis::calib::LUT6SAOT>::iterator iterAOTLUT = (*iterLUT).aotLUT.begin(); iterAOTLUT != (*iterLUT).aotLUT.end(); ++iterAOTLUT)
                {
                    delete[] (*iterAOTLUT).imageBands;
                    delete[] (*iterAOTLUT).aX;
                    delete[] (*iterAOTLUT).bX;
                    delete[] (*iterAOTLUT).cX;
                }
            }
            delete rsgisLUT;
            
            GDALClose(datasets[0]);
            GDALClose(datasets[1]);
            GDALClose(datasets[2]);
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
                
    void executeApplySubtractOffsets(std::string inputImage, std::string outputImage, std::string offsetImage, bool nonNegative, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float noDataVal, bool useNoDataVal, float darkObjReflVal) 
    {
        try
        {
            GDALAllRegister();
            GDALDataset **datasets = new GDALDataset*[2];
            
            std::cout << "Open input image: \'" << inputImage << "\'" << std::endl;
            datasets[0] = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            std::cout << "Open Offset image: \'" << offsetImage << "\'" << std::endl;
            datasets[1] = (GDALDataset *) GDALOpen(offsetImage.c_str(), GA_ReadOnly);
            if(datasets[1] == NULL)
            {
                std::string message = std::string("Could not open image ") + offsetImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            if(datasets[0]->GetRasterCount() != datasets[1]->GetRasterCount())
            {
                throw rsgis::RSGISImageException("The two input images need to have the same number of image bands.");
            }
            int numRasterBands = datasets[0]->GetRasterCount();
            
            std::cout << "Apply offsets to input image...\n";
            
            rsgis::calib::RSGISApplyDarkObjSubtractOffsets *applyOffsets = new rsgis::calib::RSGISApplyDarkObjSubtractOffsets(numRasterBands, nonNegative, noDataVal, useNoDataVal, darkObjReflVal);
            
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(applyOffsets, "", true);
            calcImage->calcImage(datasets, 2, outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(rsgisOutDataType));
            
            delete applyOffsets;
            delete calcImage;
            
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
                
    void executeLandsatThermalRad2ThermalBrightness(std::string inputImage, std::string outputImage, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float scaleFactor, std::vector<CmdsLandsatThermalCoeffs> landsatThermalCoeffs) 
    {
        GDALAllRegister();
        try
        {
            unsigned int numBands = landsatThermalCoeffs.size();
            
            std::cout << "Opening: " << inputImage << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            unsigned int numRasterBands = dataset->GetRasterCount();
            
            std::string *outBandNames = new std::string[numRasterBands];
            float *k1 = new float[numRasterBands];
            float *k2 = new float[numRasterBands];
            
            unsigned int i = 0;
            for(std::vector<rsgis::cmds::CmdsLandsatThermalCoeffs>::iterator iterBands = landsatThermalCoeffs.begin(); iterBands != landsatThermalCoeffs.end(); ++iterBands)
			{
                if((*iterBands).band != (i+1))
                {
                    throw RSGISImageException("The bands must be specified in order.");
                }
                
                if((*iterBands).band > numRasterBands)
                {
                    throw RSGISImageException("You have specified a band which is not within the image");
                }
                
                k1[i] = (*iterBands).k1;
                k2[i] = (*iterBands).k2;
                outBandNames[i] = (*iterBands).bandName;
                
                ++i;
			}
            
            rsgis::calib::RSGISCalculateTOAThermalBrightness *calibLandsatThermalTOA = new rsgis::calib::RSGISCalculateTOAThermalBrightness(numBands, k1, k2, scaleFactor);
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(calibLandsatThermalTOA, "", true);
            
            calcImage->calcImage(&dataset, 1, outputImage, true, outBandNames, gdalFormat, RSGIS_to_GDAL_Type(rsgisOutDataType));
            
            
            GDALClose(dataset);
            delete[] k1;
            delete[] k2;
            delete[] outBandNames;
            
            delete calibLandsatThermalTOA;
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
                
    void executeGenerateSaturationMask(std::string outputImage, std::string gdalFormat, std::vector<CmdsSaturatedPixel> imgBandInfo)
    {
        GDALAllRegister();
        try
        {
            unsigned int numBands = imgBandInfo.size();
            GDALDataset **datasets = new GDALDataset*[numBands];
            std::string *outBandNames = new std::string[numBands];
            
            rsgis::calib::RSGISSaturatedPixelInfo *satBandPxlInfo = new rsgis::calib::RSGISSaturatedPixelInfo[numBands];
            
            unsigned int i = 0;
            unsigned int numRasterBands = 0;
            unsigned int totalNumRasterBands = 0;
            for(std::vector<rsgis::cmds::CmdsSaturatedPixel>::iterator iterBands = imgBandInfo.begin(); iterBands != imgBandInfo.end(); ++iterBands)
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
                satBandPxlInfo[i].band = totalNumRasterBands + (*iterBands).band-1;
                satBandPxlInfo[i].satVal = (*iterBands).satVal;
                outBandNames[i] = (*iterBands).bandName;
                
                totalNumRasterBands += numRasterBands;
                ++i;
			}
            
            rsgis::calib::RSGISIdentifySaturatePixels *createSatPxlMask = new rsgis::calib::RSGISIdentifySaturatePixels(numBands, satBandPxlInfo);
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(createSatPxlMask, "", true);
            
            calcImage->calcImage(datasets, numBands, outputImage, true, outBandNames, gdalFormat, GDT_Byte);
            
            for(unsigned int i = 0; i < numBands; ++i)
            {
                GDALClose(datasets[i]);
            }
            
            delete[] datasets;
            delete[] satBandPxlInfo;
            delete[] outBandNames;
            
            delete createSatPxlMask;
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
    
    void executeLandsatTMCloudFMask(std::string inputTOAImage, std::string inputThermalImage, std::string inputSaturateImage, std::string validImg, std::string outputImage, std::string gdalFormat, double sunAz, double sunZen, double senAz, double senZen, float whitenessThreshold, float scaleFactorIn, std::string tmpImgsBase, std::string tmpImgFileExt, bool rmTmpImgs) 
    {
        GDALAllRegister();
        try
        {
            std::cout.precision(12);
            rsgis::img::RSGISImageUtils imgUtils;
            rsgis::rastergis::RSGISRasterAttUtils ratUtils;
            rsgis::rastergis::RSGISPopRATWithStats calcClumpStats;
            
            std::string pass1TmpOutImage = tmpImgsBase + "_pass1layers"+tmpImgFileExt;
            std::string landWaterTmpOutImage = tmpImgsBase + "_landwater"+tmpImgFileExt;
            std::string cloudLandProbTmpOutImage = tmpImgsBase + "_cloudProb"+tmpImgFileExt;
            std::string tmpNIRBandImg = tmpImgsBase + "_nirband"+tmpImgFileExt;
            std::string tmpNIRFillBandImg = tmpImgsBase + "_nirbandfill"+tmpImgFileExt;
            std::string tmpSWIRBandImg = tmpImgsBase + "_swirband"+tmpImgFileExt;
            std::string tmpSWIRFillBandImg = tmpImgsBase + "_swirbandfill"+tmpImgFileExt;
            std::string tmpPotentShadows = tmpImgsBase + "_potentshadows"+tmpImgFileExt;
            std::string tmpCloudsExtent = tmpImgsBase + "_baseClouds"+tmpImgFileExt;
            std::string tmpCloudsClump = tmpImgsBase + "_baseCloudClumps"+tmpImgFileExt;
            std::string tmpCloudsClumpRMSmall = tmpImgsBase + "_baseCloudClumpsRMSmall"+tmpImgFileExt;
            std::string tmpCloudsClumpRMSmallRelabel = tmpImgsBase + "_baseCloudClumpsRMSmallRelabel"+tmpImgFileExt;
            std::string tmpCloudsInitHeights = tmpImgsBase + "_baseCloudInitHeights"+tmpImgFileExt;
            std::string tmpCloudsShadowTestRegions = tmpImgsBase + "_testShadowRegions"+tmpImgFileExt;
            std::string tmpCloudsShadows = tmpImgsBase + "_shadowRegions"+tmpImgFileExt;
            std::string tmpFinalShadowsDialate = tmpImgsBase + "_finalShadowsDialate"+tmpImgFileExt;
            std::string tmpFinalClouds = tmpImgsBase + "_finalClouds"+tmpImgFileExt;
            std::string tmpFinalCloudsDialate = tmpImgsBase + "_finalCloudsDialate"+tmpImgFileExt;
            
            GDALDataset **datasets = NULL;
            std::cout << "Opening: " << inputTOAImage << std::endl;
            GDALDataset *reflDataset = (GDALDataset *) GDALOpen(inputTOAImage.c_str(), GA_ReadOnly);
            if(reflDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputTOAImage;
                throw RSGISImageException(message.c_str());
            }
            unsigned int numReflBands = reflDataset->GetRasterCount();
            
            
            GDALDataType imgReflDT = reflDataset->GetRasterBand(1)->GetRasterDataType();
            if((imgReflDT == GDT_Float32) | (imgReflDT == GDT_Float64) | (imgReflDT == GDT_Unknown) | (imgReflDT == GDT_CInt16) | (imgReflDT == GDT_CInt32) | (imgReflDT == GDT_CFloat32) | (imgReflDT == GDT_CFloat64))
            {
                throw rsgis::RSGISImageException("Input TOA image must be of an integer data type.");
            }
            
            
            std::cout << "Opening: " << inputThermalImage << std::endl;
            GDALDataset *thermDataset = (GDALDataset *) GDALOpen(inputThermalImage.c_str(), GA_ReadOnly);
            if(thermDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputThermalImage;
                throw RSGISImageException(message.c_str());
            }
            unsigned int numThermBands = thermDataset->GetRasterCount();
            
            std::cout << "Opening: " << inputSaturateImage << std::endl;
            GDALDataset *saturateDataset = (GDALDataset *) GDALOpen(inputSaturateImage.c_str(), GA_ReadOnly);
            if(saturateDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputSaturateImage;
                throw RSGISImageException(message.c_str());
            }
            unsigned int numSaturateBands = saturateDataset->GetRasterCount();
            
            std::cout << "Opening: " << validImg << std::endl;
            GDALDataset *validAreaDataset = (GDALDataset *) GDALOpen(validImg.c_str(), GA_ReadOnly);
            if(validAreaDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + validImg;
                throw RSGISImageException(message.c_str());
            }
            
            if((numReflBands+numThermBands) != numSaturateBands)
            {
                throw RSGISImageException("The number of saturation bands is not equal to the number of refl and thermal bands.");
                
                GDALClose(reflDataset);
                GDALClose(thermDataset);
                GDALClose(saturateDataset);
                GDALClose(validAreaDataset);
            }
            rsgis::rastergis::RSGISPopulateWithImageStats popImageStats;
            rsgis::rastergis::RSGISRasterAttUtils attUtils;

            std::cout << "Apply first pass FMask to classifiy initial clear sky regions...\n";
            rsgis::calib::RSGISLandsatFMaskPass1CloudMasking cloudMaskPass1 = rsgis::calib::RSGISLandsatFMaskPass1CloudMasking(scaleFactorIn, (numReflBands+numThermBands), whitenessThreshold);
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(&cloudMaskPass1, "", true);
            datasets = new GDALDataset*[3];
            datasets[0] = reflDataset;
            datasets[1] = thermDataset;
            datasets[2] = saturateDataset;
            GDALDataset *pass1DS = imgUtils.createCopy(datasets, 3, 16, pass1TmpOutImage, gdalFormat, GDT_Float32);
            calcImage->calcImage(datasets, 3, pass1DS);
            delete calcImage;
            delete[] datasets;
            
            std::vector<std::string> bandNames;
            bandNames.push_back("ndsi");
            bandNames.push_back("ndvi");
            bandNames.push_back("basicTest");
            bandNames.push_back("meanVis");
            bandNames.push_back("whitenessTest");
            bandNames.push_back("hotTest");
            bandNames.push_back("nirswirTest");
            bandNames.push_back("waterTest");
            bandNames.push_back("pcp");
            bandNames.push_back("clearSkyLand");
            bandNames.push_back("snowTest");
            bandNames.push_back("varProb");
            bandNames.push_back("modNDVI");
            bandNames.push_back("modNDSI");
            bandNames.push_back("whiteness");
            bandNames.push_back("clearSkyWater");
            imgUtils.setImageBandNames(pass1DS, bandNames, true);
            
            
            std::cout << "Export Land and Water regions and check PCP coverage.\n";
            GDALDataset *landWaterClearSkyDS = imgUtils.createCopy(pass1DS, 1, landWaterTmpOutImage, gdalFormat, GDT_UInt32);
            rsgis::calib::RSGISLandsatFMaskExportPass1LandWaterCloudMasking exportLandWaterRegions = rsgis::calib::RSGISLandsatFMaskExportPass1LandWaterCloudMasking();
            datasets = new GDALDataset*[2];
            datasets[0] = validAreaDataset;
            datasets[1] = pass1DS;
            calcImage = new rsgis::img::RSGISCalcImage(&exportLandWaterRegions, "", true);
            calcImage->calcImage(datasets, 2, landWaterClearSkyDS);
            double propPCP = exportLandWaterRegions.propOfPCPPixels();
            delete calcImage;
            delete[] datasets;
            
            std::cout << "Proportion of PCP coverage of the scene is " << propPCP << std::endl;
            
            if(propPCP < 0.95)
            {
                
                popImageStats.populateImageWithRasterGISStats(landWaterClearSkyDS, true, true, 1);
                
                std::cout << "Populating RAT with Thermal Stats\n";
                // Equation 8 and 13 calc percentiles (Zhu and Woodcock 2012, RSE 118, pp83-94):
                std::vector<rsgis::rastergis::RSGISBandAttPercentiles *> *bandPercentStats = new std::vector<rsgis::rastergis::RSGISBandAttPercentiles *>();
                rsgis::rastergis::RSGISBandAttPercentiles *tempUpperPercent = new rsgis::rastergis::RSGISBandAttPercentiles();
                tempUpperPercent->fieldName = "UpperTempThres";
                tempUpperPercent->percentile = 82.5;
                bandPercentStats->push_back(tempUpperPercent);
                rsgis::rastergis::RSGISBandAttPercentiles *tempLowerPercent = new rsgis::rastergis::RSGISBandAttPercentiles();
                tempLowerPercent->fieldName = "LowerTempThres";
                tempLowerPercent->percentile = 17.5;
                bandPercentStats->push_back(tempLowerPercent);
                calcClumpStats.populateRATWithPercentileStats(landWaterClearSkyDS, thermDataset, 1, bandPercentStats, 1, 200);
                delete tempUpperPercent;
                delete tempLowerPercent;
                delete bandPercentStats;
                
                std::cout << "Get Thresholds From the RAT\n";
                const GDALRasterAttributeTable *landWaterRAT = landWaterClearSkyDS->GetRasterBand(1)->GetDefaultRAT();
                double lowerWaterThres = ratUtils.readDoubleColumnVal(landWaterRAT, "LowerTempThres", 2)/scaleFactorIn;
                double upperWaterThres = ratUtils.readDoubleColumnVal(landWaterRAT, "UpperTempThres", 2)/scaleFactorIn;
                double lowerLandThres = ratUtils.readDoubleColumnVal(landWaterRAT, "LowerTempThres", 1)/scaleFactorIn;
                double upperLandThres = ratUtils.readDoubleColumnVal(landWaterRAT, "UpperTempThres", 1)/scaleFactorIn;
                
                std::cout << "Lower Water Threshold = " << lowerWaterThres << std::endl;
                std::cout << "Upper Water Threshold = " << upperWaterThres << std::endl;
                
                std::cout << "Lower Land Threshold = " << lowerLandThres << std::endl;
                std::cout << "Upper Land Threshold = " << upperLandThres << std::endl;
                
                std::cout << "Calculate the cloud probability over the land area...\n";
                rsgis::calib::RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking cloudMaskPass2Part1 = rsgis::calib::RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking(scaleFactorIn, (numReflBands+numThermBands), upperWaterThres, upperLandThres, lowerLandThres);
                calcImage = new rsgis::img::RSGISCalcImage(&cloudMaskPass2Part1, "", true);
                datasets = new GDALDataset*[4];
                datasets[0] = landWaterClearSkyDS;
                datasets[1] = reflDataset;
                datasets[2] = thermDataset;
                datasets[3] = pass1DS;
                GDALDataset *pass2DS = imgUtils.createCopy(pass1DS, 6, cloudLandProbTmpOutImage, gdalFormat, GDT_Float32);
                calcImage->calcImage(datasets, 4, pass2DS);
                delete calcImage;
                delete[] datasets;
                
                bandNames = std::vector<std::string>();
                bandNames.push_back("wTempProb");
                bandNames.push_back("brightnessProb");
                bandNames.push_back("waterCloudProb");
                bandNames.push_back("landTempProb");
                bandNames.push_back("varProb");
                bandNames.push_back("landCloudProb");
                imgUtils.setImageBandNames(pass2DS, bandNames, true);
                
                
                // Equation 17 (Zhu and Woodcock 2012, RSE 118, pp83-94):
                std::cout << "Calculate percentile probability thresholds for water and land...\n";
                bandPercentStats = new std::vector<rsgis::rastergis::RSGISBandAttPercentiles *>();
                rsgis::rastergis::RSGISBandAttPercentiles *landCloudProbPercent = new rsgis::rastergis::RSGISBandAttPercentiles();
                landCloudProbPercent->fieldName = "UpperCloudLandThres";
                landCloudProbPercent->percentile = 82.5; // THRESHOLD FOR DIFFERENCIATING LAND AND CLOUD
                bandPercentStats->push_back(landCloudProbPercent);
                calcClumpStats.populateRATWithPercentileStats(landWaterClearSkyDS, pass2DS, 6, bandPercentStats, 1, 200);
                delete landCloudProbPercent;
                delete bandPercentStats;
                double landCloudProbUpperThres = ratUtils.readDoubleColumnVal(landWaterRAT, "UpperCloudLandThres", 1);
                
                // Equation 18 threshold of 0.5 used in Zhu and Woodcock 2012, RSE 118, pp83-94 changed in Zhu et al (2015) RSE 159 pp269-277 to be dynamic:
                bandPercentStats = new std::vector<rsgis::rastergis::RSGISBandAttPercentiles *>();
                rsgis::rastergis::RSGISBandAttPercentiles *waterCloudProbPercent = new rsgis::rastergis::RSGISBandAttPercentiles();
                waterCloudProbPercent->fieldName = "UpperCloudWaterThres";
                waterCloudProbPercent->percentile = 82.5; // THRESHOLD FOR DIFFERENCIATING LAND AND WATER
                bandPercentStats->push_back(waterCloudProbPercent);
                calcClumpStats.populateRATWithPercentileStats(landWaterClearSkyDS, pass2DS, 3, bandPercentStats, 1, 200);
                delete waterCloudProbPercent;
                delete bandPercentStats;
                double waterCloudProbUpperThres = ratUtils.readDoubleColumnVal(landWaterRAT, "UpperCloudWaterThres", 2);
                
                landCloudProbUpperThres = landCloudProbUpperThres + 0.2;
                waterCloudProbUpperThres = waterCloudProbUpperThres + 0.2;
                if (waterCloudProbUpperThres > 0.5)
                {
                    waterCloudProbUpperThres = 0.5;
                }
                
                std::cout << "Upper Land Cloud Prob Threshold = " << landCloudProbUpperThres << std::endl;
                std::cout << "Upper Water Cloud Prob Threshold = " << waterCloudProbUpperThres << std::endl;

                std::cout << "Apply second pass FMask to classify final clouds mask...\n";
                rsgis::calib::RSGISLandsatFMaskPass2CloudMasking cloudMaskPass2Part2 = rsgis::calib::RSGISLandsatFMaskPass2CloudMasking(scaleFactorIn, (numReflBands+numThermBands), landCloudProbUpperThres, waterCloudProbUpperThres, lowerLandThres);
                calcImage = new rsgis::img::RSGISCalcImage(&cloudMaskPass2Part2, "", true);
                datasets = new GDALDataset*[5];
                datasets[0] = landWaterClearSkyDS;
                datasets[1] = reflDataset;
                datasets[2] = thermDataset;
                datasets[3] = pass1DS;
                datasets[4] = pass2DS;
                GDALDataset *cloudMaskDS = imgUtils.createCopy(pass1DS, 1, tmpCloudsExtent, gdalFormat, GDT_Int32);
                calcImage->calcImage(datasets, 5, cloudMaskDS);
                delete calcImage;
                delete[] datasets;
                
                std::cout << "Apply cloud majority filter...\n";
                rsgis::calib::RSGISCalcImageCloudMajorityFilter cloudMajFilter = rsgis::calib::RSGISCalcImageCloudMajorityFilter();
                rsgis::img::RSGISCalcEditImage editImgCalc = rsgis::img::RSGISCalcEditImage(&cloudMajFilter);
                editImgCalc.calcImageWindowData(cloudMaskDS, 5);
                
                
                std::cout << "Get cloud objects\n";
                GDALDataset *cloudClumpsDS = imgUtils.createCopy(pass1DS, 1, tmpCloudsClump, gdalFormat, GDT_UInt32);
                rsgis::segment::RSGISClumpPxls clumpImg;
                clumpImg.performClump(cloudMaskDS, cloudClumpsDS, true, 0.0, NULL);
                popImageStats.populateImageWithRasterGISStats(cloudClumpsDS, true, true, true, 1);
                
                std::cout << "Remove small clouds\n";
                float smallCloudThreshold = 32; // THRESHOLD (IN PIXELS) FOR SMALL CLOUDS
                GDALRasterAttributeTable *cloudsRAT = cloudClumpsDS->GetRasterBand(1)->GetDefaultRAT();
                
                size_t numcloudsRATHistoRows = 0;
                int *cloudsRATHisto = attUtils.readIntColumn(cloudsRAT, "Histogram", &numcloudsRATHistoRows);
                GDALDataset *cloudClumpsRMSmallDS = imgUtils.createCopy(pass1DS, 1, tmpCloudsClumpRMSmall, gdalFormat, GDT_UInt32);
                rsgis::segment::RSGISRemoveClumpsBelowThreshold rmClumpBelowSize = rsgis::segment::RSGISRemoveClumpsBelowThreshold(smallCloudThreshold, cloudsRATHisto, numcloudsRATHistoRows);
                rsgis::img::RSGISCalcImage calcImgRmSmallClump = rsgis::img::RSGISCalcImage(&rmClumpBelowSize);
                calcImgRmSmallClump.calcImage(&cloudClumpsDS, 1, 0, cloudClumpsRMSmallDS);
                delete[] cloudsRATHisto;
                
                rsgis::segment::RSGISRelabelClumps relabelImg;
                GDALDataset *cloudClumpsRMSmallReLblDS = imgUtils.createCopy(pass1DS, 1, tmpCloudsClumpRMSmallRelabel, gdalFormat, GDT_UInt32);
                relabelImg.relabelClumpsCalcImg(cloudClumpsRMSmallDS, cloudClumpsRMSmallReLblDS);
                popImageStats.populateImageWithRasterGISStats(cloudClumpsRMSmallReLblDS, true, true, 1);

                std::cout << "Calculate Shadow Mask\n";
                int nirIdx = 4;
                if(reflDataset->GetRasterCount() == 7)
                {
                    nirIdx = 5;
                }
                
                bandPercentStats = new std::vector<rsgis::rastergis::RSGISBandAttPercentiles *>();
                rsgis::rastergis::RSGISBandAttPercentiles *landNIRPercent = new rsgis::rastergis::RSGISBandAttPercentiles();
                landNIRPercent->fieldName = "LowerNIRLandValue175";
                landNIRPercent->percentile = 17.5; // NIR LAND THRESHOLD 17.5 %
                bandPercentStats->push_back(landNIRPercent);
                calcClumpStats.populateRATWithPercentileStats(landWaterClearSkyDS, reflDataset, nirIdx, bandPercentStats, 1, 200);
                delete landNIRPercent;
                delete bandPercentStats;
                double landNIR175Val = ratUtils.readDoubleColumnVal(landWaterRAT, "LowerNIRLandValue175", 1);
                std::cout << "Land NIR 17.5% Percentile = " << landNIR175Val << std::endl;
                
                // Extract NIR band.
                std::cout << "Extract NIR Band\n";
                GDALDataset *nirBandDS = imgUtils.createCopy(validAreaDataset, 1, tmpNIRBandImg, gdalFormat, imgReflDT);
                std::vector<unsigned int> bands;
                bands.push_back(nirIdx);
                rsgis::img::RSGISCopyImageBandSelect selImageBands = rsgis::img::RSGISCopyImageBandSelect(bands);
                rsgis::img::RSGISCalcImage calcSelBandsImage = rsgis::img::RSGISCalcImage(&selImageBands);
                datasets = new GDALDataset*[2];
                datasets[0] = reflDataset;
                datasets[1] = validAreaDataset;
                calcSelBandsImage.calcImage(datasets, 2, nirBandDS);
                delete[] datasets;
                
                std::cout << "Fill NIR Band\n";
                GDALDataset *nirBandFillDS = imgUtils.createCopy(reflDataset, 1, tmpNIRFillBandImg, gdalFormat, imgReflDT);
                rsgis::calib::RSGISHydroDEMFillSoilleGratin94 fillDEMInst;
                fillDEMInst.performSoilleGratin94Fill(nirBandDS, validAreaDataset, nirBandFillDS, false, landNIR175Val);
      
                ////////////////////////////////////// Perform fill with SWIR: Extra on paper which only uses the NIR.
                int swirIdx = 5;
                if(reflDataset->GetRasterCount() == 7)
                {
                    swirIdx = 6;
                }
                bandPercentStats = new std::vector<rsgis::rastergis::RSGISBandAttPercentiles *>();
                rsgis::rastergis::RSGISBandAttPercentiles *landSWIRPercent = new rsgis::rastergis::RSGISBandAttPercentiles();
                landSWIRPercent->fieldName = "LowerSWIRLandValue175";
                landSWIRPercent->percentile = 17.5; // SWIR LAND THRESHOLD 17.5 %
                bandPercentStats->push_back(landSWIRPercent);
                calcClumpStats.populateRATWithPercentileStats(landWaterClearSkyDS, reflDataset, swirIdx, bandPercentStats, 1, 200);
                delete landSWIRPercent;
                delete bandPercentStats;
                double landSWIR175Val = ratUtils.readDoubleColumnVal(landWaterRAT, "LowerSWIRLandValue175", 1);
                std::cout << "Land SWIR 17.5% Percentile = " << landSWIR175Val << std::endl;
                
                // Extract SWIR band.
                std::cout << "Extract SWIR Band\n";
                GDALDataset *swirBandDS = imgUtils.createCopy(validAreaDataset, 1, tmpSWIRBandImg, gdalFormat, imgReflDT);
                bands = std::vector<unsigned int>();
                bands.push_back(swirIdx);
                selImageBands = rsgis::img::RSGISCopyImageBandSelect(bands);
                calcSelBandsImage = rsgis::img::RSGISCalcImage(&selImageBands);
                datasets = new GDALDataset*[2];
                datasets[0] = reflDataset;
                datasets[1] = validAreaDataset;
                calcSelBandsImage.calcImage(datasets, 2, swirBandDS);
                delete[] datasets;
                
                std::cout << "Fill SWIR Band\n";
                GDALDataset *swirBandFillDS = imgUtils.createCopy(reflDataset, 1, tmpSWIRFillBandImg, gdalFormat, imgReflDT);
                fillDEMInst = rsgis::calib::RSGISHydroDEMFillSoilleGratin94();
                fillDEMInst.performSoilleGratin94Fill(swirBandDS, validAreaDataset, swirBandFillDS, false, landSWIR175Val);
                //////////////////////////////////////////////////////////////////////////////////////////////////
                
                // In addition to what is in the paper potential shadows have to be within the NIR and SWIR - helps to remove false positives.
                std::cout << "Produce Potential Cloud Shadows Mask\n";
                GDALDataset *potentCloudShadowDS = imgUtils.createCopy(validAreaDataset, 1, tmpPotentShadows, gdalFormat, GDT_Int32);
                rsgis::calib::RSGISCalcImagePotentialCloudShadowsMask imgCalcPotentShadows = rsgis::calib::RSGISCalcImagePotentialCloudShadowsMask(scaleFactorIn);
                rsgis::img::RSGISCalcImage calcPotentShadowImage = rsgis::img::RSGISCalcImage(&imgCalcPotentShadows);
                datasets = new GDALDataset*[5];
                datasets[0] = validAreaDataset;
                datasets[1] = nirBandDS;
                datasets[2] = nirBandFillDS;
                datasets[3] = swirBandDS;
                datasets[4] = swirBandFillDS;
                calcPotentShadowImage.calcImage(datasets, 5, potentCloudShadowDS);
                popImageStats.populateImageWithRasterGISStats(potentCloudShadowDS, true, true, 1);
                delete[] datasets;
                

                GDALDataset *initCloudHeightsDS = imgUtils.createCopy(pass1DS, 2, tmpCloudsInitHeights, gdalFormat, GDT_Float32);
                rsgis::calib::RSGISCalcCloudParams calcCloudParams;
                calcCloudParams.calcCloudHeights(thermDataset, cloudClumpsRMSmallReLblDS, initCloudHeightsDS, lowerLandThres, upperLandThres, scaleFactorIn);
                
                GDALDataset *cloudShadowTestRegionsDS = imgUtils.createCopy(pass1DS, 1, tmpCloudsShadowTestRegions, gdalFormat, GDT_Byte);
                GDALDataset *cloudShadowRegionsDS = imgUtils.createCopy(pass1DS, 1, tmpCloudsShadows, gdalFormat, GDT_Byte);
                
                calcCloudParams.projFitCloudShadow(cloudClumpsRMSmallReLblDS, initCloudHeightsDS, potentCloudShadowDS, cloudShadowTestRegionsDS, cloudShadowRegionsDS, sunAz, sunZen, senAz, senZen);
                
                
                std::cout << "Apply cloud shadow majority filter...\n";
                rsgis::calib::RSGISCalcImageCloudMajorityFilter cloudShadowMajFilter = rsgis::calib::RSGISCalcImageCloudMajorityFilter();
                rsgis::img::RSGISCalcEditImage editImgCalcShadow = rsgis::img::RSGISCalcEditImage(&cloudShadowMajFilter);
                editImgCalcShadow.calcImageWindowData(cloudShadowRegionsDS, 5);
                 
                rsgis::math::RSGISMatrices matrixUtils;
                
                rsgis::math::Matrix *matrixMorphOperator = matrixUtils.createMatrix(15, 15);
                matrixUtils.makeCircularBinaryMatrix(matrixMorphOperator);
                
                rsgis::filter::RSGISImageMorphologyDilate morphDialate;
                morphDialate.dilateImage(&cloudShadowRegionsDS, tmpFinalShadowsDialate, matrixMorphOperator, gdalFormat, GDT_Byte);
                GDALDataset *finalShadowsDialateDS = (GDALDataset *) GDALOpen(tmpFinalShadowsDialate.c_str(), GA_Update);
                if(finalShadowsDialateDS == NULL)
                {
                    std::string message = std::string("Could not open image ") + tmpFinalShadowsDialate;
                    throw RSGISImageException(message.c_str());
                }
                
                GDALRasterAttributeTable *cloudsRATRelbl = cloudClumpsRMSmallReLblDS->GetRasterBand(1)->GetDefaultRAT();
                numcloudsRATHistoRows = 0;
                cloudsRATHisto = attUtils.readIntColumn(cloudsRATRelbl, "Histogram", &numcloudsRATHistoRows);
                
                for(size_t i = 0; i < numcloudsRATHistoRows; ++i)
                {
                    cloudsRATHisto[i] = 1;
                }
                attUtils.writeIntColumn(cloudsRATRelbl, "CloudMask", cloudsRATHisto, numcloudsRATHistoRows);
                delete[] cloudsRATHisto;
                
                unsigned int columnIndex = attUtils.findColumnIndex(cloudsRATRelbl, "CloudMask");
                rsgis::rastergis::RSGISExportColumns2ImageCalcImage calcImageVal = rsgis::rastergis::RSGISExportColumns2ImageCalcImage(1, cloudsRATRelbl, columnIndex);
                rsgis::img::RSGISCalcImage calcImageExportRATCol(&calcImageVal);
                GDALDataset *finalCloudsDS = imgUtils.createCopy(pass1DS, 1, tmpFinalClouds, gdalFormat, GDT_Byte);
                calcImageExportRATCol.calcImage(&cloudClumpsRMSmallReLblDS, 1, 0, finalCloudsDS);
      
                
                morphDialate.dilateImage(&finalCloudsDS, tmpFinalCloudsDialate, matrixMorphOperator, gdalFormat, GDT_Byte);
                GDALDataset *finalCloudsDialateDS = (GDALDataset *) GDALOpen(tmpFinalCloudsDialate.c_str(), GA_Update);
                if(finalCloudsDialateDS == NULL)
                {
                    std::string message = std::string("Could not open image ") + tmpFinalCloudsDialate;
                    throw RSGISImageException(message.c_str());
                }
                matrixUtils.freeMatrix(matrixMorphOperator);
                
                GDALDataset *finalResultDS = imgUtils.createCopy(pass1DS, 1, outputImage, gdalFormat, GDT_Byte);
                datasets = new GDALDataset*[2];
                datasets[0] = finalCloudsDialateDS;
                datasets[1] = finalShadowsDialateDS;
                rsgis::calib::RSGISCalcCombineMasks calcCombineMask;
                rsgis::img::RSGISCalcImage calcImgCombineMasks = rsgis::img::RSGISCalcImage(&calcCombineMask);
                calcImgCombineMasks.calcImage(datasets, 2, 0, finalResultDS);
                delete[] datasets;
                
                popImageStats.populateImageWithRasterGISStats(finalResultDS, true, true, 1);
                
                int *red = new int[3];
                int *green = new int[3];
                int *blue = new int[3];
                std::string *classNames = new std::string[3];
                
                red[0] = 0;
                green[0] = 0;
                blue[0] = 0;
                classNames[0] = "";
                
                red[1] = 0;
                green[1] = 0;
                blue[1] = 255;
                classNames[1] = "Clouds";
                
                red[2] = 0;
                green[2] = 255;
                blue[2] = 255;
                classNames[2] = "Shadows";
                
                GDALRasterAttributeTable *finalResultRAT = finalResultDS->GetRasterBand(1)->GetDefaultRAT();
                finalResultRAT->SetRowCount(3);
                attUtils.writeIntColumn(finalResultRAT, "Red", red, 3);
                attUtils.writeIntColumn(finalResultRAT, "Green", green, 3);
                attUtils.writeIntColumn(finalResultRAT, "Blue", blue, 3);
                attUtils.writeStrColumn(finalResultRAT, "ClassName", classNames, 3);
                
                delete[] red;
                delete[] green;
                delete[] blue;
                delete[] classNames;
                
                GDALClose(pass1DS);
                GDALClose(landWaterClearSkyDS);
                GDALClose(pass2DS);
                GDALClose(nirBandDS);
                GDALClose(nirBandFillDS);
                GDALClose(potentCloudShadowDS);
                GDALClose(cloudMaskDS);
                GDALClose(cloudClumpsDS);
                GDALClose(cloudClumpsRMSmallDS);
                GDALClose(cloudClumpsRMSmallReLblDS);
                GDALClose(initCloudHeightsDS);
                GDALClose(cloudShadowTestRegionsDS);
                GDALClose(cloudShadowRegionsDS);
                GDALClose(finalShadowsDialateDS);
                GDALClose(finalCloudsDS);
                GDALClose(finalCloudsDialateDS);
                GDALClose(finalResultDS);
                GDALClose(reflDataset);
                GDALClose(thermDataset);
                GDALClose(saturateDataset);
                GDALClose(validAreaDataset);
                GDALClose(swirBandFillDS);
                GDALClose(swirBandDS);
                
                if(rmTmpImgs)
                {
                    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
                    if(poDriver == NULL)
                    {
                        throw RSGISImageException("Image driver is not available.");
                    }
                    
                    poDriver->Delete(pass1TmpOutImage.c_str());
                    poDriver->Delete(landWaterTmpOutImage.c_str());
                    poDriver->Delete(cloudLandProbTmpOutImage.c_str());
                    poDriver->Delete(tmpNIRBandImg.c_str());
                    poDriver->Delete(tmpNIRFillBandImg.c_str());
                    poDriver->Delete(tmpPotentShadows.c_str());
                    poDriver->Delete(tmpCloudsExtent.c_str());
                    poDriver->Delete(tmpCloudsClump.c_str());
                    poDriver->Delete(tmpCloudsClumpRMSmall.c_str());
                    poDriver->Delete(tmpCloudsClumpRMSmallRelabel.c_str());
                    poDriver->Delete(tmpCloudsInitHeights.c_str());
                    poDriver->Delete(tmpCloudsShadowTestRegions.c_str());
                    poDriver->Delete(tmpCloudsShadows.c_str());
                    poDriver->Delete(tmpFinalShadowsDialate.c_str());
                    poDriver->Delete(tmpFinalClouds.c_str());
                    poDriver->Delete(tmpFinalCloudsDialate.c_str());
                    poDriver->Delete(tmpSWIRBandImg.c_str());
                    poDriver->Delete(tmpSWIRFillBandImg.c_str());
                }
            }
            else
            {
                GDALDataset *finalResultDS = imgUtils.createCopy(validAreaDataset, 1, outputImage, gdalFormat, GDT_Byte);
                rsgis::calib::RSGISExportMaskForOverPCPThreshold calcFinalMask;
                rsgis::img::RSGISCalcImage calcImgProduceFinalMask = rsgis::img::RSGISCalcImage(&calcFinalMask);
                calcImgProduceFinalMask.calcImage(&validAreaDataset, 1, 0, finalResultDS);
                popImageStats.populateImageWithRasterGISStats(finalResultDS, true, true, 1);
                
                int *red = new int[3];
                int *green = new int[3];
                int *blue = new int[3];
                std::string *classNames = new std::string[3];
                
                red[0] = 0;
                green[0] = 0;
                blue[0] = 0;
                classNames[0] = "";
                
                red[1] = 0;
                green[1] = 0;
                blue[1] = 255;
                classNames[1] = "Clouds";
                
                red[2] = 0;
                green[2] = 255;
                blue[2] = 255;
                classNames[2] = "Shadows";
                
                GDALRasterAttributeTable *finalResultRAT = finalResultDS->GetRasterBand(1)->GetDefaultRAT();
                finalResultRAT->SetRowCount(3);
                attUtils.writeIntColumn(finalResultRAT, "Red", red, 3);
                attUtils.writeIntColumn(finalResultRAT, "Green", green, 3);
                attUtils.writeIntColumn(finalResultRAT, "Blue", blue, 3);
                attUtils.writeStrColumn(finalResultRAT, "ClassName", classNames, 3);
                
                delete[] red;
                delete[] green;
                delete[] blue;
                delete[] classNames;
                
                
                GDALClose(pass1DS);
                GDALClose(landWaterClearSkyDS);
                GDALClose(reflDataset);
                GDALClose(thermDataset);
                GDALClose(saturateDataset);
                GDALClose(validAreaDataset);
                GDALClose(finalResultDS);
                
                if(rmTmpImgs)
                {
                    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
                    if(poDriver == NULL)
                    {
                        throw RSGISImageException("Image driver is not available.");
                    }
                    
                    poDriver->Delete(pass1TmpOutImage.c_str());
                    poDriver->Delete(landWaterTmpOutImage.c_str());
                }
            }
            
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
                
    void executeConvertWorldView2ToRadiance(std::string inputImage, std::string outputImage, std::string gdalFormat, std::vector<CmdsWorldView2RadianceGainsOffsets> wv2RadGainOffs)
    {
        GDALAllRegister();
        
        try
        {
            unsigned int numBands = wv2RadGainOffs.size();
            std::cout << "Opening: " << inputImage << std::endl;
            GDALDataset *inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            if(inDataset->GetRasterCount() != numBands)
            {
                std::cerr << "Number of input coefficients = " << numBands << std::endl;
                std::cerr << "Number of input image bands = " << inDataset->GetRasterCount() << std::endl;
                throw RSGISImageException("The number of band coefficients needs to equal the number input image bands.");
            }
            
            std::string *outBandNames = new std::string[numBands];
            
            rsgis::calib::WorldView2RadianceGainsOffsets *wv2RadGainOffsInternal = new rsgis::calib::WorldView2RadianceGainsOffsets[numBands];
            
            unsigned int i = 0;
            for(std::vector<rsgis::cmds::CmdsWorldView2RadianceGainsOffsets>::iterator iterBands = wv2RadGainOffs.begin(); iterBands != wv2RadGainOffs.end(); ++iterBands)
            {
                wv2RadGainOffsInternal[i].band = (*iterBands).band;
                wv2RadGainOffsInternal[i].absCalFact = (*iterBands).absCalFact;
                wv2RadGainOffsInternal[i].effBandWidth = (*iterBands).effBandWidth;
                
                outBandNames[i] = (*iterBands).bandName;
                ++i;
            }
            
            rsgis::calib::RSGISWorldView2RadianceCalibration *radianceCalibration = new rsgis::calib::RSGISWorldView2RadianceCalibration(numBands, wv2RadGainOffsInternal);
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(radianceCalibration, "", true);
            
            calcImage->calcImage(&inDataset, 1, outputImage, true, outBandNames, gdalFormat);

            GDALClose(inDataset);
            delete[] wv2RadGainOffsInternal;
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

                
    void executeConvertSPOT5ToRadiance(std::string inputImage, std::string outputImage, std::string gdalFormat, std::vector<CmdsSPOTRadianceGainsOffsets> spot5RadGainOffs)
    {
        GDALAllRegister();
        
        try
        {
            unsigned int numBands = spot5RadGainOffs.size();
            std::cout << "Opening: " << inputImage << std::endl;
            GDALDataset *inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            if(inDataset->GetRasterCount() != numBands)
            {
                std::cerr << "Number of input coefficients = " << numBands << std::endl;
                std::cerr << "Number of input image bands = " << inDataset->GetRasterCount() << std::endl;
                throw RSGISImageException("The number of band coefficients needs to equal the number input image bands.");
            }
            
            std::string *outBandNames = new std::string[numBands];
            
            rsgis::calib::SPOTRadianceGainsOffsets *spot5RadGainOffsInternal = new rsgis::calib::SPOTRadianceGainsOffsets[numBands];
            
            unsigned int i = 0;
            for(std::vector<rsgis::cmds::CmdsSPOTRadianceGainsOffsets>::iterator iterBands = spot5RadGainOffs.begin(); iterBands != spot5RadGainOffs.end(); ++iterBands)
            {
                if(((*iterBands).band < 1) | ((*iterBands).band > 4))
                {
                    throw RSGISImageException("Image bands must be between 1 and 4.");
                }
                
                spot5RadGainOffsInternal[i].band = (*iterBands).band;
                spot5RadGainOffsInternal[i].gain = (*iterBands).gain;
                spot5RadGainOffsInternal[i].bias = (*iterBands).bias;
                
                outBandNames[spot5RadGainOffsInternal[i].band-1] = (*iterBands).bandName;
                ++i;
            }
            
            rsgis::calib::RSGISSPOTRadianceCalibration *radianceCalibration = new rsgis::calib::RSGISSPOTRadianceCalibration(numBands, spot5RadGainOffsInternal);
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(radianceCalibration, "", true);
            
            calcImage->calcImage(&inDataset, 1, outputImage, true, outBandNames, gdalFormat);
            
            GDALClose(inDataset);
            delete[] spot5RadGainOffsInternal;
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
                
    void executeApplySubtractSingleOffsets(std::string inputImage, std::string outputImage, std::vector<double> offsetValues, bool nonNegative, std::string gdalFormat, rsgis::RSGISLibDataType rsgisOutDataType, float noDataVal, bool useNoDataVal, float darkObjReflVal) 
    {
        try
        {
            GDALAllRegister();
            
            std::cout << "Open input image: \'" << inputImage << "\'" << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            if(dataset->GetRasterCount() != offsetValues.size())
            {
                throw rsgis::RSGISImageException("The two input images need to have the same number of image bands.");
            }
            int numRasterBands = dataset->GetRasterCount();
            
            std::cout << "Apply offsets to input image...\n";
            rsgis::calib::RSGISApplyDarkObjSubtractSingleOffsets *applyOffsets = new rsgis::calib::RSGISApplyDarkObjSubtractSingleOffsets(numRasterBands, offsetValues, nonNegative, noDataVal, useNoDataVal, darkObjReflVal);
            
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(applyOffsets, "", true);
            calcImage->calcImage(&dataset, 1, outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(rsgisOutDataType));
            
            delete applyOffsets;
            delete calcImage;
            
            GDALClose(dataset);
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
    
                
    void executeCalcNadirImageViewAngle(std::string imgFootprint, std::string outViewAngleImg, std::string gdalFormat, double sateAltitude, std::string minXXCol, std::string minXYCol, std::string maxXXCol, std::string maxXYCol, std::string minYXCol, std::string minYYCol, std::string maxYXCol, std::string maxYYCol) 
    {
        try
        {
            GDALAllRegister();
            
            std::cout << "Open input image: \'" << imgFootprint << "\'" << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpen(imgFootprint.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + imgFootprint;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::calib::RSGISImgCalibUtils calibUtils;
            calibUtils.calcNadirImgViewAngle(dataset, outViewAngleImg, gdalFormat, sateAltitude, minXXCol, minXYCol, maxXXCol, maxXYCol, minYXCol, minYYCol, maxYXCol, maxYYCol);
            
            GDALClose(dataset);
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
                
    void executeCalcIrradianceElevLUT(std::string inputDataMaskImg, std::string inputDEMImg, std::string inputIncidenceAngleImg, std::string inputSlopeImg, std::string shadowMaskImg, std::string srefInputImage, std::string outputImg, std::string gdalFormat, float solarZenith, float reflScaleFactor, std::vector<Cmds6SElevationLUT> *lut) 
    {
        try
        {
            GDALAllRegister();
            GDALDataset **datasets = new GDALDataset*[5];
            datasets[0] = (GDALDataset *) GDALOpen(inputDataMaskImg.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputDataMaskImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            datasets[1] = (GDALDataset *) GDALOpen(inputDEMImg.c_str(), GA_ReadOnly);
            if(datasets[1] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputDEMImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            datasets[2] = (GDALDataset *) GDALOpen(inputIncidenceAngleImg.c_str(), GA_ReadOnly);
            if(datasets[2] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputIncidenceAngleImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            datasets[3] = (GDALDataset *) GDALOpen(inputSlopeImg.c_str(), GA_ReadOnly);
            if(datasets[3] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputSlopeImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            datasets[4] = (GDALDataset *) GDALOpen(shadowMaskImg.c_str(), GA_ReadOnly);
            if(datasets[4] == NULL)
            {
                std::string message = std::string("Could not open image ") + shadowMaskImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *srefImgDS = (GDALDataset *) GDALOpen(srefInputImage.c_str(), GA_ReadOnly);
            if(srefImgDS == NULL)
            {
                std::string message = std::string("Could not open image ") + srefInputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            int numRasterReflBands = srefImgDS->GetRasterCount();
            
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
                lutVal.directIrr = new float[lutVal.numValues];
                lutVal.diffuseIrr = new float[lutVal.numValues];
                lutVal.envIrr = new float[lutVal.numValues];
                
                for(unsigned int i = 0; i < (*iterLUT).numValues; ++i)
                {
                    if((*iterLUT).imageBands[i] > numRasterReflBands)
                    {
                        GDALClose(datasets[0]);
                        GDALClose(datasets[1]);
                        GDALClose(datasets[2]);
                        GDALClose(datasets[3]);
                        delete[] datasets;
                        GDALClose(srefImgDS);
                        throw rsgis::RSGISException("The number of input image bands is not equal to the number coefficients provided.");
                    }
                    lutVal.imageBands[i] = (*iterLUT).imageBands[i];
                    lutVal.aX[i] = (*iterLUT).aX[i];
                    lutVal.bX[i] = (*iterLUT).bX[i];
                    lutVal.cX[i] = (*iterLUT).cX[i];
                    lutVal.directIrr[i] = (*iterLUT).directIrr[i];
                    lutVal.diffuseIrr[i] = (*iterLUT).diffuseIrr[i];
                    lutVal.envIrr[i] = (*iterLUT).envIrr[i];
                    std::cout << "\tBand " << lutVal.imageBands[i] << ": Direct = " << lutVal.directIrr[i] << " Diffuse = " << lutVal.diffuseIrr[i] << " Env = " << lutVal.envIrr[i] << std::endl;
                }
                
                rsgisLUT->push_back(lutVal);
            }
            // Calculate the mean SREF for each band within mask area.
            rsgis::img::ImageStats **srefBandStats = new rsgis::img::ImageStats*[numRasterReflBands];
            double *srefNoDataVals = new double[numRasterReflBands];
            for(unsigned int i = 0; i < numRasterReflBands; ++i)
            {
                srefBandStats[i] = new rsgis::img::ImageStats();
                srefBandStats[i]->mean = 0.0;
                srefBandStats[i]->max = 0.0;
                srefBandStats[i]->min = 0.0;
                srefBandStats[i]->stddev = 0.0;
                srefBandStats[i]->sum = 0.0;
                srefNoDataVals[i] = 0.0;
            }
            
            rsgis::img::RSGISImageStatistics calcSREFMean;
            calcSREFMean.calcImageStatisticsMask(srefImgDS, datasets[0], 1, srefBandStats, srefNoDataVals, true, numRasterReflBands, false);
            double *srefMean = new double[numRasterReflBands];
            for(unsigned int i = 0; i < numRasterReflBands; ++i)
            {
                srefMean[i] = srefBandStats[i]->mean/reflScaleFactor;
                delete srefBandStats[i];
                std::cout << "Mean B" << i+1 << ": " << srefMean[i] << std::endl;
            }
            delete[] srefBandStats;
            delete[] srefNoDataVals;
            
            // Number of output bands:
            int numOutBands = numRasterReflBands * 4;
            rsgis::math::RSGISMathsUtils mathUtils;
            std::string *bandNames = new std::string[numOutBands];
            for(unsigned int i = 0; i < numRasterReflBands; ++i)
            {
                bandNames[i*4] = "B" + mathUtils.uinttostring(i+1) + "_Direct";
                bandNames[(i*4)+1] = "B" + mathUtils.uinttostring(i+1) + "_Diffuse";
                bandNames[(i*4)+2] = "B" + mathUtils.uinttostring(i+1) + "_Env";
                bandNames[(i*4)+3] = "B" + mathUtils.uinttostring(i+1) + "_Total";
            }

            // Calculate Irradiance Image...
            rsgis::calib::RSGISCalcSolarIrradianceElevLUTParam calcSolarIrr = rsgis::calib::RSGISCalcSolarIrradianceElevLUTParam(numOutBands, rsgisLUT, srefMean, numRasterReflBands, solarZenith);
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(&calcSolarIrr, "", true);
            calcImage.calcImage(datasets, 5, outputImg, true, bandNames, gdalFormat, GDT_Float32);
            
            delete[] bandNames;
            delete[] srefMean;
            for(std::vector<rsgis::calib::LUT6SElevation>::iterator iterLUT = rsgisLUT->begin(); iterLUT != rsgisLUT->end(); ++iterLUT)
            {
                delete[] (*iterLUT).imageBands;
                delete[] (*iterLUT).aX;
                delete[] (*iterLUT).bX;
                delete[] (*iterLUT).cX;
                delete[] (*iterLUT).directIrr;
                delete[] (*iterLUT).diffuseIrr;
                delete[] (*iterLUT).envIrr;
            }
            delete rsgisLUT;
            
            GDALClose(datasets[0]);
            GDALClose(datasets[1]);
            GDALClose(datasets[2]);
            GDALClose(datasets[3]);
            GDALClose(datasets[4]);
            delete[] datasets;
            GDALClose(srefImgDS);
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
                
    void executeCalcStandardisedReflectanceSD2010(std::string inputDataMaskImg, std::string srefInputImage, std::string inputSolarIrradiance, std::string inputIncidenceAngleImg, std::string inputExitanceAngleImg, std::string outputImg, std::string gdalFormat, float brdfBeta, float outIncidenceAngle, float outExitanceAngle, float reflScaleFactor) 
    {
        try
        {
            GDALAllRegister();
            GDALDataset **datasets = new GDALDataset*[5];
            datasets[0] = (GDALDataset *) GDALOpen(inputDataMaskImg.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputDataMaskImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            datasets[1] = (GDALDataset *) GDALOpen(inputIncidenceAngleImg.c_str(), GA_ReadOnly);
            if(datasets[1] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputIncidenceAngleImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            datasets[2] = (GDALDataset *) GDALOpen(inputExitanceAngleImg.c_str(), GA_ReadOnly);
            if(datasets[2] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputExitanceAngleImg;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            datasets[3] = (GDALDataset *) GDALOpen(srefInputImage.c_str(), GA_ReadOnly);
            if(datasets[3] == NULL)
            {
                std::string message = std::string("Could not open image ") + srefInputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            datasets[4] = (GDALDataset *) GDALOpen(inputSolarIrradiance.c_str(), GA_ReadOnly);
            if(datasets[4] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputSolarIrradiance;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            int numRasterReflBands = datasets[3]->GetRasterCount();
            
            // Calculate standardised Reflectance Image...
            rsgis::calib::RSGISCalcStandardisedReflectanceSD2010 calcStdRefl = rsgis::calib::RSGISCalcStandardisedReflectanceSD2010(numRasterReflBands, numRasterReflBands, brdfBeta, outIncidenceAngle, outExitanceAngle, reflScaleFactor);
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(&calcStdRefl, "", true);
            calcImage.calcImage(datasets, 5, outputImg, false, NULL, gdalFormat, GDT_UInt16);
                        
            GDALClose(datasets[0]);
            GDALClose(datasets[1]);
            GDALClose(datasets[2]);
            GDALClose(datasets[3]);
            GDALClose(datasets[4]);
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
                
    unsigned int executeGetJulianDay(unsigned int year, unsigned int month, unsigned int day) 
    {
        unsigned int julianDay = 0;
        try
        {
            julianDay = rsgis::calib::rsgisGetJulianDay(day, month, year);
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
        return julianDay;
    }
                
    float executeGetEarthSunDistance(unsigned int julianDay) 
    {
        float dist = 0.0;
        try
        {
            dist = rsgis::calib::rsgisCalcSolarDistance(julianDay);
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
        return dist;
    }
                
                
    void executePerformCloudShadowMasking(std::string cloudMsk, std::string inputImage, std::string validAreaImage, unsigned int darkFillBand, std::string outputImg, std::string gdalFormat, float scaleFactorIn, std::string tmpImgsBase, std::string tmpImgFileExt, bool rmTmpImgs, double sunAz, double sunZen, double senAz, double senZen) 
    {
        GDALAllRegister();
        try
        {
            std::cout.precision(12);
            rsgis::img::RSGISImageUtils imgUtils;
            rsgis::rastergis::RSGISPopulateWithImageStats popImageStats;
            
            std::cout << "Opening: " << inputImage << std::endl;
            GDALDataset *reflDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(reflDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            unsigned int numReflBands = reflDataset->GetRasterCount();
            
            GDALDataType imgReflDT = reflDataset->GetRasterBand(1)->GetRasterDataType();
            
            std::cout << "Opening: " << validAreaImage << std::endl;
            GDALDataset *validDataset = (GDALDataset *) GDALOpen(validAreaImage.c_str(), GA_ReadOnly);
            if(validDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + validAreaImage;
                throw RSGISImageException(message.c_str());
            }
            
            std::cout << "Opening: " << cloudMsk << std::endl;
            GDALDataset *cloudMskDataset = (GDALDataset *) GDALOpen(cloudMsk.c_str(), GA_ReadOnly);
            if(validDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + cloudMsk;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset **datasets = NULL;
            
            if((darkFillBand == 0) & (darkFillBand > numReflBands))
            {
                throw RSGISImageException("Band specified is not within the image.");
            }
            
            std::cout << "Calculate Shadow Mask\n";
            std::string tmpDarkBandImg = tmpImgsBase + "_darkband"+tmpImgFileExt;
            std::string tmpDarkFillBandImg = tmpImgsBase + "_darkbandfill"+tmpImgFileExt;
            std::string tmpPotentShadows = tmpImgsBase + "_potentshadows"+tmpImgFileExt;
            std::string tmpClumpClouds = tmpImgsBase + "_cloudclumps"+tmpImgFileExt;
            std::string tmpCloudsShadowTestRegions = tmpImgsBase + "_testShadowRegions"+tmpImgFileExt;
            std::string tmpCloudsShadows = tmpImgsBase + "_shadowRegions"+tmpImgFileExt;
            std::string tmpCloudsInitHeights = tmpImgsBase + "_baseCloudInitHeights"+tmpImgFileExt;
            
            GDALDataset *tmpClumpCloudsDS = imgUtils.createCopy(cloudMskDataset, 1, tmpClumpClouds, gdalFormat, GDT_UInt32, true, "");
            
            rsgis::segment::RSGISClumpPxls clumpImg;
            clumpImg.performClump(cloudMskDataset, tmpClumpCloudsDS, true, 0.0, NULL);
            popImageStats.populateImageWithRasterGISStats(tmpClumpCloudsDS, true, true, 1);
            
            rsgis::img::RSGISImagePercentiles calcPercentiles;
            rsgis::math::Matrix *bandPercentiles = calcPercentiles.getPercentilesForAllBands(reflDataset, .175, 0.0, true);
            
            double darkImgBand175PercentVal = bandPercentiles->matrix[darkFillBand-1];
            
            std::cout << "Dark 17.5% Value: " << darkImgBand175PercentVal << std::endl;
            
            // Extract Dark band.
            std::cout << "Extract Dark Band\n";
            GDALDataset *darkBandDS = imgUtils.createCopy(validDataset, 1, tmpDarkBandImg, gdalFormat, imgReflDT);
            std::vector<unsigned int> bands;
            bands.push_back(darkFillBand);
            rsgis::img::RSGISCopyImageBandSelect selImageBands = rsgis::img::RSGISCopyImageBandSelect(bands);
            rsgis::img::RSGISCalcImage calcSelBandsImage = rsgis::img::RSGISCalcImage(&selImageBands);
            datasets = new GDALDataset*[2];
            datasets[0] = reflDataset;
            datasets[1] = validDataset;
            calcSelBandsImage.calcImage(datasets, 2, darkBandDS);
            delete[] datasets;
            
            std::cout << "Fill Dark Band\n";
            GDALDataset *darkBandFillDS = imgUtils.createCopy(reflDataset, 1, tmpDarkFillBandImg, gdalFormat, imgReflDT);
            rsgis::calib::RSGISHydroDEMFillSoilleGratin94 fillDEMInst;
            fillDEMInst.performSoilleGratin94Fill(darkBandDS, validDataset, darkBandFillDS, false, darkImgBand175PercentVal);
            
            std::cout << "Produce Potential Cloud Shadows Mask\n";
            GDALDataset *potentCloudShadowDS = imgUtils.createCopy(validDataset, 1, tmpPotentShadows, gdalFormat, GDT_Int32);
            rsgis::calib::RSGISCalcImagePotentialCloudShadowsMaskSingleInput imgCalcPotentShadows = rsgis::calib::RSGISCalcImagePotentialCloudShadowsMaskSingleInput(scaleFactorIn);
            rsgis::img::RSGISCalcImage calcPotentShadowImage = rsgis::img::RSGISCalcImage(&imgCalcPotentShadows);
            datasets = new GDALDataset*[3];
            datasets[0] = validDataset;
            datasets[1] = darkBandDS;
            datasets[2] = darkBandFillDS;
            calcPotentShadowImage.calcImage(datasets, 3, potentCloudShadowDS);
            popImageStats.populateImageWithRasterGISStats(potentCloudShadowDS, true, true, 1);
            delete[] datasets;
            
            GDALDataset *initCloudHeightsDS = imgUtils.createCopy(validDataset, 2, tmpCloudsInitHeights, gdalFormat, GDT_Float32);
            imgUtils.assignValGDALDataset(initCloudHeightsDS, 0.0);
            
            GDALDataset *cloudShadowTestRegionsDS = imgUtils.createCopy(validDataset, 1, tmpCloudsShadowTestRegions, gdalFormat, GDT_Byte);
            GDALDataset *cloudShadowRegionsDS = imgUtils.createCopy(validDataset, 1, tmpCloudsShadows, gdalFormat, GDT_Byte);
            
            rsgis::calib::RSGISCalcCloudParams calcCloudParams;
            calcCloudParams.calcCloudHeightsNoThermal(tmpClumpCloudsDS, initCloudHeightsDS);
            calcCloudParams.projFitCloudShadow(tmpClumpCloudsDS, initCloudHeightsDS, potentCloudShadowDS, cloudShadowTestRegionsDS, cloudShadowRegionsDS, sunAz, sunZen, senAz, senZen);

            std::cout << "Apply cloud shadow majority filter...\n";
            rsgis::calib::RSGISCalcImageCloudMajorityFilter cloudShadowMajFilter = rsgis::calib::RSGISCalcImageCloudMajorityFilter();
            rsgis::img::RSGISCalcEditImage editImgCalcShadow = rsgis::img::RSGISCalcEditImage(&cloudShadowMajFilter);
            editImgCalcShadow.calcImageWindowData(cloudShadowRegionsDS, 5);
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixMorphOperator = matrixUtils.createMatrix(15, 15);
            matrixUtils.makeCircularBinaryMatrix(matrixMorphOperator);
            rsgis::filter::RSGISImageMorphologyDilate morphDialate;
            morphDialate.dilateImage(&cloudShadowRegionsDS, outputImg, matrixMorphOperator, gdalFormat, GDT_Byte);
            
            GDALClose(darkBandDS);
            GDALClose(darkBandFillDS);
            GDALClose(potentCloudShadowDS);
            GDALClose(initCloudHeightsDS);
            GDALClose(cloudShadowTestRegionsDS);
            GDALClose(cloudShadowRegionsDS);
            GDALClose(tmpClumpCloudsDS);
            
            if(rmTmpImgs)
            {
                GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
                if(poDriver == NULL)
                {
                    throw RSGISImageException("Image driver is not available.");
                }
                
                poDriver->Delete(tmpDarkBandImg.c_str());
                poDriver->Delete(tmpDarkFillBandImg.c_str());
                poDriver->Delete(tmpPotentShadows.c_str());
                poDriver->Delete(tmpClumpClouds.c_str());
                poDriver->Delete(tmpCloudsShadowTestRegions.c_str());
                poDriver->Delete(tmpCloudsShadows.c_str());
                poDriver->Delete(tmpCloudsInitHeights.c_str());
            }
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



