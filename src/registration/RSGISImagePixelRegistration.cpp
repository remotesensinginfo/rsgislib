/*
 *  RSGISImagePixelRegistration.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/01/2012.
 *  Copyright 2012 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 *
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISImagePixelRegistration.h"

namespace rsgis{namespace reg{
    
    RSGISImagePixelRegistration::RSGISImagePixelRegistration(GDALDataset *reference, GDALDataset *floating, std::string outputImagePath, std::string outputFormat, unsigned int windowSize, unsigned int searchArea, RSGISImageSimilarityMetric *metric, unsigned int subPixelResolution):RSGISImageRegistration(reference, floating)
    {
        this->outputImagePath = outputImagePath;
        this->outputFormat = outputFormat;
        this->windowSize = windowSize;
        this->searchArea = searchArea;
        this->metric = metric;
        this->subPixelResolution = subPixelResolution;
    }
    
    void RSGISImagePixelRegistration::initRegistration()throw(RSGISRegistrationException)
    {
        try
        {
            if(referenceIMG->GetRasterCount() != floatingIMG->GetRasterCount())
            {
                throw RSGISRegistrationException("Both images need to have the same number of image bands.");
            }
            
            // Find image overlap
            try
            {
                this->findOverlap();
            }
            catch(RSGISRegistrationException &e)
            {
                throw e;
            }
            
            GDALDriver *gdalDriver = GetGDALDriverManager()->GetDriverByName(outputFormat.c_str());
            if(gdalDriver == NULL)
            {
                throw rsgis::RSGISImageException("Requested GDAL driver does not exists..");
            }
            
            std::cout << "Output image has size [" << this->overlap->xSize << "," << this->overlap->ySize << "]\n";
            
            outputImage = gdalDriver->Create(outputImagePath.c_str(), this->overlap->xSize, this->overlap->ySize, 3, GDT_Float32, NULL);
            double *gdalTranslation = new double[6];
            gdalTranslation[0] = this->overlap->tlX;
            gdalTranslation[1] = this->overlap->xRes;
            gdalTranslation[2] = this->overlap->xRot;
            gdalTranslation[3] = this->overlap->tlY;
            gdalTranslation[4] = this->overlap->yRot;
            gdalTranslation[5] = this->overlap->yRes*(-1);
            std::cout << "Image Transform: \n";
            std::cout << "\t TL [" << gdalTranslation[0] << "," << gdalTranslation[3] << "]\n";
            std::cout << "\t Res [" << gdalTranslation[1] << "," << gdalTranslation[5] << "]\n";
            std::cout << "\t Rot [" << gdalTranslation[2] << "," << gdalTranslation[4] << "]\n";
            outputImage->SetGeoTransform(gdalTranslation);
            outputImage->SetProjection(this->referenceIMG->GetProjectionRef());
            outputImage->GetRasterBand(1)->SetDescription("X Shift");
            outputImage->GetRasterBand(2)->SetDescription("Y Shift");
            outputImage->GetRasterBand(3)->SetDescription("Metric Value");
            
            delete[] gdalTranslation;
        }
        catch (RSGISRegistrationException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISImageException &e)
        {
            throw RSGISRegistrationException(e.what());
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISRegistrationException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISRegistrationException(e.what());
        }
        
        std::cout << "There are " << (this->overlap->xSize * this->overlap->ySize) << " pixels to be matched\n";
        
        initExecuted = true;
    }
    
    void RSGISImagePixelRegistration::executeRegistration()throw(RSGISRegistrationException)
    {
        try
        {
            float *xShift = new float[this->overlap->xSize];
            float *yShift = new float[this->overlap->xSize];
            float *metricVal = new float[this->overlap->xSize];
            float xShiftPxl = 0;
            float yShiftPxl = 0;
            TiePoint *tiePt = new TiePoint;
            
            double currentEastings = overlap->tlX;
            double currentNorthings = overlap->tlY;
            
            int feedback = this->overlap->ySize/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
            
            for(unsigned int i = 0; i < this->overlap->ySize; ++i)
            {
                if((i % feedback) == 0)
				{
					std::cout << "." << feedbackCounter << "." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
                
                currentEastings = overlap->tlX;
                
                for(unsigned int j = 0; j < this->overlap->xSize; ++j)
                {
                    tiePt->eastings = currentEastings;
                    tiePt->northings = currentNorthings;
                    tiePt->xRef = overlap->refXStart + j;
                    tiePt->yRef = overlap->refYStart + i;
                    tiePt->xFloat = overlap->floatXStart + j;
                    tiePt->yFloat = overlap->floatYStart + i;
                    tiePt->xShift = 0;
                    tiePt->yShift = 0;
                    tiePt->metricVal = std::numeric_limits<double>::signaling_NaN();
                    
                    this->findTiePointLocation(tiePt, windowSize, searchArea, metric, subPixelResolution, &xShiftPxl, &yShiftPxl);
                                        
                    xShift[j] = tiePt->xShift;
                    yShift[j] = tiePt->yShift;
                    metricVal[j] = tiePt->metricVal;
                    
                    currentEastings += overlap->xRes;
                }
                
                this->outputImage->GetRasterBand(1)->RasterIO(GF_Write, 0, i, this->overlap->xSize, 1, xShift, this->overlap->xSize, 1, GDT_Float32, 0, 0);
                this->outputImage->GetRasterBand(2)->RasterIO(GF_Write, 0, i, this->overlap->xSize, 1, yShift, this->overlap->xSize, 1, GDT_Float32, 0, 0);
                this->outputImage->GetRasterBand(3)->RasterIO(GF_Write, 0, i, this->overlap->xSize, 1, metricVal, this->overlap->xSize, 1, GDT_Float32, 0, 0);
                
                currentNorthings -= overlap->yRes;
            }
            std::cout << " Complete.\n";
            
            delete[] xShift;
            delete[] yShift;
            delete[] metricVal;
            delete tiePt;
            
        }
        catch (RSGISRegistrationException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISImageException &e)
        {
            throw RSGISRegistrationException(e.what());
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISRegistrationException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISRegistrationException(e.what());
        }
    }
    
    void RSGISImagePixelRegistration::finaliseRegistration()throw(RSGISRegistrationException)
    {
        try
        {
            GDALClose(outputImage);
        }
        catch (RSGISRegistrationException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISImageException &e)
        {
            throw RSGISRegistrationException(e.what());
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISRegistrationException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISRegistrationException(e.what());
        }
    }
    
    void RSGISImagePixelRegistration::exportTiePointsENVIImage2Map(std::string filepath)throw(RSGISRegistrationException)
    {
        throw RSGISRegistrationException("Tie point export is not implemented.");
    }
    
    void RSGISImagePixelRegistration::exportTiePointsENVIImage2Image(std::string filepath)throw(RSGISRegistrationException)
    {
        throw RSGISRegistrationException("Tie point export is not implemented.");
    }
    
    void RSGISImagePixelRegistration::exportTiePointsRSGISImage2Map(std::string filepath)throw(RSGISRegistrationException)
    {
        throw RSGISRegistrationException("Tie point export is not implemented.");
    }
    
    void RSGISImagePixelRegistration::exportTiePointsRSGISMapOffs(std::string filepath)throw(RSGISRegistrationException)
    {
        throw RSGISRegistrationException("Tie point export is not implemented.");
    }
    
    RSGISImagePixelRegistration::~RSGISImagePixelRegistration()
    {
        
    }
}}



