/*
 *  RSGISDEMTools.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/08/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#include "RSGISDEMTools.h"


namespace rsgis{namespace calib{
    
    
    RSGISCalcSlope::RSGISCalcSlope(int numberOutBands, unsigned int band, float ewRes, float nsRes, int outType) : rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
        this->outType = outType;
    }
    void RSGISCalcSlope::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(winSize != 3)
        {
            throw rsgis::img::RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
        }
        
        if(!(band < numBands))
        {
            throw rsgis::img::RSGISImageCalcException("Specified image band is not within the image.");
        }

        const double radiansToDegrees = 180.0 / M_PI;

        double dx, dy, slopeRad;
        
        dx = ((dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]) - 
              (dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]))/ewRes;
        
        dy = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
              (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;

        slopeRad = atan(sqrt((dx * dx) + (dy * dy))/8);

        if(outType == 0)
        {
            output[0] = (slopeRad * radiansToDegrees);
        }
        else
        {
            output[0] = slopeRad;
        }
    }
    
    RSGISCalcSlope::~RSGISCalcSlope()
    {
        
    }

    RSGISCalcAspect::RSGISCalcAspect(int numberOutBands, unsigned int band, float ewRes, float nsRes) : rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
    }
		
    void RSGISCalcAspect::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(winSize != 3)
        {
            throw rsgis::img::RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
        }
        
        if(!(band < numBands))
        {
            throw rsgis::img::RSGISImageCalcException("Specified image band is not within the image.");
        }
        
        const double radiansToDegrees = 180.0 / M_PI;
        
        double dx, dy, aspect;
        
        dx = ((dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]) - 
              (dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]))/ewRes;
        
        dy = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
              (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
        
        aspect = atan2(-dx, dy)*radiansToDegrees;
                
        if (dx == 0 && dy == 0)
        {
            /* Flat area */
            aspect = std::numeric_limits<double>::signaling_NaN();
        }
        else if(aspect < 0)
        {
            aspect += 360.0;
        }
        else if(aspect == 360.0)
        {
            aspect = 0.0;
        }
        else if(aspect > 360)
        {
            double num = aspect / 360.0;
            int num360s = floor(num);
            aspect = aspect - (360 * num360s);
        }
        
        output[0] = aspect;
    }

    RSGISCalcAspect::~RSGISCalcAspect()
    {
        
    }
    
    
    RSGISRecodeAspect::RSGISRecodeAspect():rsgis::img::RSGISCalcImageValue(1)
    {
        
    }

    void RSGISRecodeAspect::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(boost::math::isnan(bandValues[0]))
        {
            output[0] = 0;
        }
        else
        {
            if(bandValues[0] > 360)
            {
                double num = bandValues[0] / 360.0;
                int num360s = floor(num);
                bandValues[0] = bandValues[0] - (360 * num360s);
            }
            
            if((bandValues[0] >= 0) & (bandValues[0] < 45))
            {
                output[0] = 1;
            }
            else if((bandValues[0] >= 45) & (bandValues[0] < 90))
            {
                output[0] = 2;
            }
            else if((bandValues[0] >= 90) & (bandValues[0] < 135))
            {
                output[0] = 3;
            }
            else if((bandValues[0] >= 135) & (bandValues[0] < 180))
            {
                output[0] = 4;
            }
            else if((bandValues[0] >= 180) & (bandValues[0] < 225))
            {
                output[0] = 5;
            }
            else if((bandValues[0] >= 225) & (bandValues[0] < 270))
            {
                output[0] = 6;
            }
            else if((bandValues[0] >= 270) & (bandValues[0] < 315))
            {
                output[0] = 7;
            }
            else if((bandValues[0] >= 315) & (bandValues[0] <= 360))
            {
                output[0] = 8;
            }
            else if((bandValues[0] >= 360) & (bandValues[0] <= 405))
            {
                output[0] = 1;
            }
            else
            {
                std::cerr << "Input Aspect Value = " << bandValues[0] << std::endl;
                throw rsgis::img::RSGISImageCalcException("The input image pixel values much be between 0 and 360 degrees.");
            }
        }
    }
    
    RSGISRecodeAspect::~RSGISRecodeAspect()
    {
        
    }
    
    
    

    RSGISCalcSlopeAspect::RSGISCalcSlopeAspect(int numberOutBands, unsigned int band, float ewRes, float nsRes) : rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
    }
    void RSGISCalcSlopeAspect::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(winSize != 3)
        {
            throw rsgis::img::RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
        }
        
        if(!(band < numBands))
        {
            throw rsgis::img::RSGISImageCalcException("Specified image band is not within the image.");
        }
        
        const double radiansToDegrees = 180.0 / M_PI;
        
        double dxSlope, dySlope, slopeRad;
        
        dxSlope = ((dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]) - 
              (dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]))/ewRes;
        
        dySlope = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
              (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
        
        slopeRad = atan(sqrt((dxSlope * dxSlope) + (dySlope * dySlope))/8);
        
        output[0] = (slopeRad * radiansToDegrees);
        
        double dxAspect, dyAspect, aspect;
        
        dxAspect = ((dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]) - 
              (dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]))/ewRes;
        
        dyAspect = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
              (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
        
        aspect = atan2(-dxAspect, dyAspect)*radiansToDegrees;
        
        if (dxAspect == 0 && dyAspect == 0)
        {
            /* Flat area */
            aspect = std::numeric_limits<double>::signaling_NaN();
        }
        
        if (aspect < 0)
        {
            aspect += 360.0;
        }
        
        if (aspect == 360.0)
        {
            aspect = 0.0;
        }
        
        output[1] = aspect;
    }
    
    RSGISCalcSlopeAspect::~RSGISCalcSlopeAspect()
    {
        
    }
    
    
    
    
    RSGISCalcHillShade::RSGISCalcHillShade(int numberOutBands, unsigned int band, float ewRes, float nsRes, float sunZenith, float sunAzimuth) : rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
        this->sunZenith = sunZenith;
        this->sunAzimuth = sunAzimuth;
        
        this->sunAzimuth = 360 - this->sunAzimuth;
        this->sunAzimuth = this->sunAzimuth + 90;
        if(this->sunAzimuth > 360)
        {
            this->sunAzimuth = this->sunAzimuth - 360;
        }
    }
    
    void RSGISCalcHillShade::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(winSize != 3)
        {
            throw rsgis::img::RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
        }
        
        if(!(band < numBands))
        {
            throw rsgis::img::RSGISImageCalcException("Specified image band is not within the image.");
        }
        
        const double degreesToRadians = M_PI / 180.0;
        
        double dx, dy, aspect;
        
        dx = ((dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2])-
                (dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]))/(ewRes*8);
        
        dy = ((dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2])-
              (dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]))/(nsRes*8);       
        
        double xx_plus_yy = dx * dx + dy * dy;
        
        // aspect...
        aspect = atan2(dy,dx);
        
        // shade value
        double sunZenRad = sunZenith * degreesToRadians;
        double sunAzRad = sunAzimuth * degreesToRadians;
                
        double cang = (sin(sunZenRad) -
                cos(sunZenRad) * sqrt(xx_plus_yy) *
                sin(aspect - (sunAzRad-M_PI/2))) /
                sqrt(1 + 1 * xx_plus_yy);
        
        if (cang <= 0.0)
        {
            cang = 1.0;
        }
        else
        {
            cang = 1.0 + (254.0 * cang);
        }
        
        output[0] = cang;
    }
    
    RSGISCalcHillShade::~RSGISCalcHillShade()
    {
        
    }
    
    
    
    
    
    RSGISCalcShadowBinaryMask::RSGISCalcShadowBinaryMask(int numberOutBands, GDALDataset *inputImage, unsigned int band, float ewRes, float nsRes, float sunZenith, float sunAzimuth, float maxElevHeight, double noDataVal) : rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
        this->sunZenith = sunZenith;
        this->sunAzimuth = sunAzimuth;
        this->inputImage = inputImage;
        this->maxElevHeight = maxElevHeight;
        
        this->demWidth = inputImage->GetRasterXSize() * ewRes;
        this->demHeight = inputImage->GetRasterYSize() * nsRes;
        
        this->sunRange = sqrt((demWidth * demWidth) + (demHeight * demHeight))*2;
        
        this->noDataVal = noDataVal;
        this->degreesToRadians = M_PI / 180.0;
        this->radiansToDegrees = 180.0 / M_PI;

        extractPixels = new rsgis::img::RSGISExtractImagePixelsOnLine();
    }
		
    void RSGISCalcShadowBinaryMask::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
    {
        
        if(winSize != 3)
        {
            throw rsgis::img::RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
        }
        
        if(!(this->band <= numBands))
        {
            throw rsgis::img::RSGISImageCalcException("Specified image band is not within the image.");
        }
        
        output[0] = 0;
        try
        {   bool flatGround = false;
            bool pxlAwayFromSun = false;
            
            if( dataBlock[band-1][1][1] != this->noDataVal)
            {
                double dx, dy, slopeRad = 0.0;
                dx = ((dataBlock[band-1][0][0] + dataBlock[band-1][1][0] + dataBlock[band-1][1][0] + dataBlock[band-1][2][0]) -
                      (dataBlock[band-1][0][2] + dataBlock[band-1][1][2] + dataBlock[band-1][1][2] + dataBlock[band-1][2][2]))/ewRes;
                dy = ((dataBlock[band-1][2][0] + dataBlock[band-1][2][1] + dataBlock[band-1][2][1] + dataBlock[band-1][2][2]) -
                      (dataBlock[band-1][0][0] + dataBlock[band-1][0][1] + dataBlock[band-1][0][1] + dataBlock[band-1][0][2]))/nsRes;
                slopeRad = atan(sqrt((dx * dx) + (dy * dy))/8);
                
                double dxAspect, dyAspect, aspect;
                dxAspect = ((dataBlock[band-1][0][2] + dataBlock[band-1][1][2] + dataBlock[band-1][1][2] + dataBlock[band-1][2][2]) -
                            (dataBlock[band-1][0][0] + dataBlock[band-1][1][0] + dataBlock[band-1][1][0] + dataBlock[band-1][2][0]))/ewRes;
                dyAspect = ((dataBlock[band-1][2][0] + dataBlock[band-1][2][1] + dataBlock[band-1][2][1] + dataBlock[band-1][2][2]) -
                            (dataBlock[band-1][0][0] + dataBlock[band-1][0][1] + dataBlock[band-1][0][1] + dataBlock[band-1][0][2]))/nsRes;
                aspect = atan2(-dxAspect, dyAspect)*radiansToDegrees;
                
                if (dxAspect == 0 && dyAspect == 0)
                {
                    // Flat area
                    aspect = std::numeric_limits<double>::signaling_NaN();
                    flatGround = true;
                }
                if(!flatGround)
                {
                    if (aspect < 0)
                    {
                        aspect += 360.0;
                    }
                    if (aspect == 360.0)
                    {
                        aspect = 0.0;
                    }
                    aspect = aspect * degreesToRadians;
                    
                    
                    double sunZenRad = sunZenith * degreesToRadians;
                    double sunAzRad = sunAzimuth * degreesToRadians;
                    
                    double ic = (cos(sunZenRad) * cos(slopeRad)) + (sin(sunZenRad) * sin(slopeRad) * cos((sunAzRad) - aspect));
                    //output[0] = ic;
                    if(ic < 0)
                    {
                        pxlAwayFromSun = true;
                    }
                }
                
                if(pxlAwayFromSun)
                {
                    output[0] = 1;
                }
                else
                {
                    // do ray tracing...
                    // Location of active point.
                    double x = extent.getMinX() + (extent.getMaxX() - extent.getMinX())/2;
                    double y = extent.getMinY() + (extent.getMaxY() - extent.getMinY())/2;
                    double z = dataBlock[band-1][1][1];
                    
                    
                    double sunAzTrans = 360-this->sunAzimuth;
                    sunAzTrans = sunAzTrans + 90;
                    if(sunAzTrans > 360)
                    {
                        sunAzTrans = sunAzTrans-360;
                    }
                    
                    double sunZenRad = sunZenith * degreesToRadians;
                    double sunAzRad = sunAzTrans * degreesToRadians;
                    
                    // Location of the sun.
                    //double sunX = x + (sunRange * sin(sunZenRad) * cos(sunAzRad));
                    //double sunY = y + (sunRange * sin(sunZenRad) * sin(sunAzRad));
                    //double sunZ = z + (sunRange * cos(sunZenRad));
                    
                    
                    // Create Ray Line
                    geos::geom::Coordinate pxlPt;
                    pxlPt.x = x;
                    pxlPt.y = y;
                    pxlPt.z = z;
                    
                    
                    std::vector<rsgis::img::ImagePixelValuePt*> *imagePxlPts = extractPixels->getImagePixelValues(inputImage, band, &pxlPt, sunAzRad, sunZenRad, maxElevHeight);
                    
                    // Check whether pixel intersects with ray.
                    for(std::vector<rsgis::img::ImagePixelValuePt*>::iterator iterPxls = imagePxlPts->begin(); iterPxls != imagePxlPts->end(); ++iterPxls)
                    {
                        if((*iterPxls)->pt->z < (*iterPxls)->value)
                        {
                            output[0] = 1;
                            break;
                        }
                    }
                    
                    // Clean up memory..
                    for(std::vector<rsgis::img::ImagePixelValuePt*>::iterator iterPxls = imagePxlPts->begin(); iterPxls != imagePxlPts->end(); )
                    {
                        delete (*iterPxls)->pt;
                        delete (*iterPxls);
                        iterPxls = imagePxlPts->erase(iterPxls);
                    }
                    delete imagePxlPts;
                }
                
            }
            else
            {
                output[0] = 0;
            }
        }
        catch (rsgis::img::RSGISImageCalcException &e) 
        {
            throw e;
        }
    }
    
    RSGISCalcShadowBinaryMask::~RSGISCalcShadowBinaryMask()
    {
        delete extractPixels;
    }
    
    
    
    

    RSGISCalcRayIncidentAngle::RSGISCalcRayIncidentAngle(int numberOutBands, unsigned int band, float ewRes, float nsRes, float sunZenith, float sunAzimuth) : rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
        this->sunZenith = sunZenith;
        this->sunAzimuth = sunAzimuth;
    }
		
    void RSGISCalcRayIncidentAngle::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        float outputValue = 0;
        
        const double degreesToRadians = M_PI / 180.0;
        
        try 
        {
            if(winSize != 3)
            {
                throw rsgis::img::RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
            }
            
            if(!(band < numBands))
            {
                throw rsgis::img::RSGISImageCalcException("Specified image band is not within the image.");
            }
            
            const double radiansToDegrees = 180.0 / M_PI;
            
            double dxSlope, dySlope, slopeRad;
            
            dxSlope = ((dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]) - 
                       (dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]))/ewRes;
            
            dySlope = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
                       (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
            
            slopeRad = atan(sqrt((dxSlope * dxSlope) + (dySlope * dySlope))/8);
            
            
            double dxAspect, dyAspect, aspect;
            
            dxAspect = ((dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]) - 
                        (dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]))/ewRes;
            
            dyAspect = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
                        (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
            
            aspect = atan2(-dxAspect, dyAspect)*radiansToDegrees;
            
            if (dxAspect == 0 && dyAspect == 0)
            {
                /* Flat area */
                aspect = std::numeric_limits<double>::signaling_NaN();
            }
            
            if (aspect < 0)
            {
                aspect += 360.0;
            }
            
            if (aspect == 360.0)
            {
                aspect = 0.0;
            }
            double aspectRad = aspect*degreesToRadians;
            
            // UNIT VECTOR FOR SURFACE
            double pA = sin(slopeRad) * cos(aspectRad);
            double pB = sin(slopeRad) * sin(aspectRad);
            double pC = cos(slopeRad);
            
            double sunAzTrans = this->sunAzimuth + 180;
            if(sunAzTrans > 360)
            {
                sunAzTrans = sunAzTrans - 360;
            }
            double sunZenRad = sunZenith * degreesToRadians;
            double sunAzRad = sunAzTrans * degreesToRadians;
            
            // UNIT VECTOR FOR INCIDENT RAY
            double rA = sin(sunZenRad) * cos(sunAzRad);
            double rB = sin(sunZenRad) * sin(sunAzRad);
            double rC = cos(sunZenRad);
            
            outputValue = acos((pA*rA)+(pB*rB)+(pC*rC)) * radiansToDegrees;
            
            if(boost::math::isnan(outputValue))
            {
                outputValue = sunZenith;
            }
        } 
        catch (rsgis::img::RSGISImageCalcException &e) 
        {
            throw e;
        }

        output[0] = outputValue;
    }
    
    RSGISCalcRayIncidentAngle::~RSGISCalcRayIncidentAngle()
    {
        
	}
    
    
    
    RSGISCalcRayExitanceAngle::RSGISCalcRayExitanceAngle(int numberOutBands, unsigned int band, float ewRes, float nsRes, float viewZenith, float viewAzimuth) : rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
        this->viewZenith = viewZenith;
        this->viewAzimuth = viewAzimuth;
    }
		
    void RSGISCalcRayExitanceAngle::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        float outputValue = 0;
        
        const double degreesToRadians = M_PI / 180.0;
        
        try 
        {
            if(winSize != 3)
            {
                throw rsgis::img::RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
            }
            
            if(!(band < numBands))
            {
                throw rsgis::img::RSGISImageCalcException("Specified image band is not within the image.");
            }
            
            const double radiansToDegrees = 180.0 / M_PI;
            
            double dxSlope, dySlope, slopeRad, slope;
            
            dxSlope = ((dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]) - 
                       (dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]))/ewRes;
            
            dySlope = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
                       (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
            
            slopeRad = atan(sqrt((dxSlope * dxSlope) + (dySlope * dySlope))/8);
            
            slope = (slopeRad * radiansToDegrees);
            
            double dxAspect, dyAspect, aspect;
            
            dxAspect = ((dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]) - 
                        (dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]))/ewRes;
            
            dyAspect = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
                        (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
            
            aspect = atan2(-dxAspect, dyAspect)*radiansToDegrees;
            
            if (dxAspect == 0 && dyAspect == 0)
            {
                /* Flat area */
                aspect = std::numeric_limits<double>::signaling_NaN();
            }
            
            if (aspect < 0)
            {
                aspect += 360.0;
            }
            
            if (aspect == 360.0)
            {
                aspect = 0.0;
            }
            
            // UNIT VECTOR FOR SURFACE
            double pA = sin(slope*degreesToRadians) * cos(aspect*degreesToRadians);
            double pB = sin(slope*degreesToRadians) * sin(aspect*degreesToRadians);
            double pC = cos(slope*degreesToRadians);
            
            // UNIT VECTOR FOR EXITANCE RAY
            double rA = sin(viewZenith*degreesToRadians) * cos(viewAzimuth*degreesToRadians);
            double rB = sin(viewZenith*degreesToRadians) * sin(viewAzimuth*degreesToRadians);
            double rC = cos(viewZenith*degreesToRadians);
            
            outputValue = acos((pA*rA)+(pB*rB)+(pC*rC)) * radiansToDegrees;
            
            if(boost::math::isnan(outputValue))
            {
                outputValue = 0;
            }
        } 
        catch (rsgis::img::RSGISImageCalcException &e) 
        {
            throw e;
        }
        
        output[0] = outputValue;
    }
		
    RSGISCalcRayExitanceAngle::~RSGISCalcRayExitanceAngle()
    {
        
    }

    
    
    
    RSGISCalcRayIncidentAndExitanceAngles::RSGISCalcRayIncidentAndExitanceAngles(int numberOutBands, unsigned int band, float ewRes, float nsRes, float sunZenith, float sunAzimuth, float viewZenith, float viewAzimuth) : rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
        this->sunZenith = sunZenith;
        this->sunAzimuth = sunAzimuth;
        this->viewZenith = viewZenith;
        this->viewAzimuth = viewAzimuth;
    }
		
    void RSGISCalcRayIncidentAndExitanceAngles::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        float incidenceAngle = 0;
        float existanceAngle = 0;
        
        const double degreesToRadians = M_PI / 180.0;
        
        try 
        {
            if(winSize != 3)
            {
                throw rsgis::img::RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
            }
            
            if(!(band < numBands))
            {
                throw rsgis::img::RSGISImageCalcException("Specified image band is not within the image.");
            }
            
            const double radiansToDegrees = 180.0 / M_PI;
            
            double dxSlope, dySlope, slopeRad, slope;
            
            dxSlope = ((dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]) - 
                       (dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]))/ewRes;
            
            dySlope = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
                       (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
            
            slopeRad = atan(sqrt((dxSlope * dxSlope) + (dySlope * dySlope))/8);
            
            slope = (slopeRad * radiansToDegrees);
            
            double dxAspect, dyAspect, aspect;
            
            dxAspect = ((dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]) - 
                        (dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]))/ewRes;
            
            dyAspect = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
                        (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
            
            aspect = atan2(-dxAspect, dyAspect)*radiansToDegrees;
            
            if (dxAspect == 0 && dyAspect == 0)
            {
                /* Flat area */
                aspect = std::numeric_limits<double>::signaling_NaN();
            }
            
            if (aspect < 0)
            {
                aspect += 360.0;
            }
            
            if (aspect == 360.0)
            {
                aspect = 0.0;
            }
            
            double aspectRad = aspect*degreesToRadians;
            
            // UNIT VECTOR FOR SURFACE
            double pA = sin(slopeRad) * cos(aspectRad);
            double pB = sin(slopeRad) * sin(aspectRad);
            double pC = cos(slopeRad);
            
            double sunZenRad = sunZenith * degreesToRadians;
            double sunAzRad = sunAzimuth * degreesToRadians;
            
            // UNIT VECTOR FOR INCIDENT RAY
            double rA = sin(sunZenRad) * cos(sunAzRad);
            double rB = sin(sunZenRad) * sin(sunAzRad);
            double rC = cos(sunZenRad);
            
            incidenceAngle = acos((pA*rA)+(pB*rB)+(pC*rC)) * radiansToDegrees;
            
            if(boost::math::isnan(incidenceAngle))
            {
                incidenceAngle = sunZenith;
            }
            
            // UNIT VECTOR FOR EXITANCE RAY
            rA = sin(viewZenith*degreesToRadians) * cos(viewAzimuth*degreesToRadians);
            rB = sin(viewZenith*degreesToRadians) * sin(viewAzimuth*degreesToRadians);
            rC = cos(viewZenith*degreesToRadians);
            
            existanceAngle = acos((pA*rA)+(pB*rB)+(pC*rC)) * radiansToDegrees;
            
            if(boost::math::isnan(existanceAngle))
            {
                existanceAngle = 0;
            }            
        } 
        catch (rsgis::img::RSGISImageCalcException &e) 
        {
            throw e;
        }
        
        output[0] = incidenceAngle;
        output[1] = existanceAngle;
    }
    
    RSGISCalcRayIncidentAndExitanceAngles::~RSGISCalcRayIncidentAndExitanceAngles()
    {
        
    }
    
    
    
    
    
    
    RSGISFillDEMHoles::RSGISFillDEMHoles(float holeValue, float nodata) : rsgis::img::RSGISCalcImageValue(3)
    {
        this->holeValue = holeValue;
        this->nodata = nodata;
    }
		
    void RSGISFillDEMHoles::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numBands != numOutBands)
        {
            throw rsgis::img::RSGISImageCalcException("There should be 3 input and 3 output image bands.");
        }
        
        int midPoint = floor(((float)winSize)/2.0);
        
        if(dataBlock[0][midPoint][midPoint] == this->holeValue)
        {
            
        }
        else
        {
            output[0] = dataBlock[0][midPoint][midPoint];
            output[1] = dataBlock[1][midPoint][midPoint];
            output[2] = dataBlock[2][midPoint][midPoint];
        }
        
    }
    
    bool RSGISFillDEMHoles::changeOccurred()
    {
        return change;
    }
    
    void RSGISFillDEMHoles::resetChange()
    {
        change = false;
    }
        
    RSGISFillDEMHoles::~RSGISFillDEMHoles()
    {
        
    }
    
    
    
    
    
    
    
    
    
    RSGISInFillDerivedHoles::RSGISInFillDerivedHoles(float holeValue) : rsgis::img::RSGISCalcImageValue(1)
    {
        this->holeValue = holeValue;
    }
    
    void RSGISInFillDerivedHoles::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        int midPoint = floor(((float)winSize)/2.0);
        
        bool foundNoData = false;
        for(unsigned int i = 0; i < 3; ++i)
        {
            for(unsigned int j = 0; j < 3; ++j)
            {
                if(dataBlock[0][i][j] == this->holeValue)
                {
                    foundNoData = true;
                    break;
                }
            }
        }
        
        if(foundNoData)
        {
            output[0] = dataBlock[2][midPoint][midPoint];
        }
        else
        {
            output[0] = dataBlock[1][midPoint][midPoint];
        }
    }
    
    RSGISInFillDerivedHoles::~RSGISInFillDerivedHoles()
    {
        
    }
    
    
    
    
    
    RSGISFilterDTMWithAspectMedianFilter::RSGISFilterDTMWithAspectMedianFilter(float aspectRange) : rsgis::img::RSGISCalcImageValue(1)
    {
        this->aspectRange = aspectRange;
    }
    
    void RSGISFilterDTMWithAspectMedianFilter::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        rsgis::math::RSGISMathsUtils mathUtils;
        int midPoint = floor(((float)winSize)/2.0);
        
        float aspectVal = dataBlock[1][midPoint][midPoint];
        float lowerAspThres = aspectVal - aspectRange;
        float upperAspThres = aspectVal + aspectRange;
        
        if(lowerAspThres < 0)
        {
            lowerAspThres = 360 + lowerAspThres;
        }
        
        if(upperAspThres > 360)
        {
            upperAspThres = upperAspThres - 360;
        }
        
        std::vector<float> vals;
        
        for(unsigned int i = 0; i < winSize; ++i)
        {
            for(unsigned int j = 0; j < winSize; ++j)
            {
                if(mathUtils.angleWithinRange(dataBlock[1][i][j], lowerAspThres, upperAspThres))
                {
                    if(!boost::math::isnan(dataBlock[0][i][j]))
                    {
                        vals.push_back(dataBlock[0][i][j]);
                    }
                }
            }
        }
        if(vals.size() > 0)
        {
            std::sort(vals.begin(), vals.end());
            int midpt = floor(vals.size()/2.0);
            
            output[0] = vals.at(midpt);
        }
        else
        {
            for(unsigned int i = 0; i < winSize; ++i)
            {
                for(unsigned int j = 0; j < winSize; ++j)
                {
                    if(!boost::math::isnan(dataBlock[0][i][j]))
                    {
                        vals.push_back(dataBlock[0][i][j]);
                    }
                }
            }
            if(vals.size() > 0)
            {
                std::sort(vals.begin(), vals.end());
                int midpt = floor(vals.size()/2.0);
                
                output[0] = vals.at(midpt);
            }
            else
            {
                output[0] = std::numeric_limits<float>::signaling_NaN();
            }
            
        }
        
    }
    
    RSGISFilterDTMWithAspectMedianFilter::~RSGISFilterDTMWithAspectMedianFilter()
    {
        
    }
    
    
    
    
    
    
	
}}


