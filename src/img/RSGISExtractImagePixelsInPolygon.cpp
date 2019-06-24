/*
 *  RSGISExtractImagePixelsInPolygon.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/08/2011.
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

#include "RSGISExtractImagePixelsInPolygon.h"


namespace rsgis 
{
	namespace img
	{
        RSGISExtractImagePixelsOnLine::RSGISExtractImagePixelsOnLine()
        {

        }
        
        std::vector<ImagePixelValuePt*>* RSGISExtractImagePixelsOnLine::getImagePixelValues(GDALDataset *image, unsigned int imageBand, geos::geom::Coordinate *pt1, float azimuthRad, float zenithRad, float rayElevThreshold) 
        {
            std::vector<ImagePixelValuePt*> *pxlValues = new std::vector<ImagePixelValuePt*>();
            
            try 
            {
                double *transform = new double[6];
                image->GetGeoTransform(transform);
                
                double xMin = transform[0];
                double xMax = transform[0] + image->GetRasterXSize()*transform[1];
                double yMax = transform[3];
                double yMin = transform[3] + image->GetRasterYSize()*transform[5];
                
                if(transform[5] < 0)
                {
                    transform[5] *= -1;
                }
                
                double xDiff = (pt1->x - xMin)/transform[1];
                double yDiff = (yMax - pt1->y)/transform[5];
                
                boost::uint_fast32_t xPxl = 0;
                boost::uint_fast32_t yPxl = 0;
                
                boost::uint_fast32_t xPxlPrev = 0;
                boost::uint_fast32_t yPxlPrev = 0;
                
                try
                {
                    xPxlPrev = xPxl = boost::numeric_cast<boost::uint_fast32_t>(xDiff);
                    yPxlPrev = yPxl = boost::numeric_cast<boost::uint_fast32_t>(yDiff);
                }
                catch(boost::numeric::negative_overflow& e) 
                {
                    throw RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e) 
                {
                    throw RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e) 
                {
                    throw RSGISImageCalcException(e.what());
                }
                
                double stepRange = transform[1];
                if(transform[5] < stepRange)
                {
                    stepRange = transform[5];
                }

                double xStep = (stepRange * sin(zenithRad) * cos(azimuthRad));
                double yStep = (stepRange * sin(zenithRad) * sin(azimuthRad));
                double zStep = (stepRange * cos(zenithRad));
                
                xStep = xStep/3;
                yStep = yStep/3;
                zStep = zStep/3;
                
                double xVal = pt1->x;
                double yVal = pt1->y;
                double zVal = pt1->z;
                
                bool complete = false;
                while(!complete)
                {
                    xVal += xStep;
                    yVal += yStep;
                    zVal += zStep;
                    
                    // Reached edge of image?
                    if(xVal < xMin)
                    {
                        complete = true;
                        break;
                    }
                    else if(xVal > xMax)
                    {
                        complete = true;
                        break;
                    }
                   
                    if(yVal > yMax)
                    {
                        complete = true;
                        break;
                    }
                    else if(yVal < yMin)
                    {
                        complete = true;
                        break;
                    }
                    
                    xDiff = (xVal - xMin)/transform[1];
                    yDiff = (yMax - yVal)/transform[5];
                    
                    try
                    {
                        xPxl = boost::numeric_cast<boost::uint_fast32_t>(xDiff);
                        yPxl = boost::numeric_cast<boost::uint_fast32_t>(yDiff);
                    }
                    catch(boost::numeric::negative_overflow& e) 
                    {
                        throw RSGISImageCalcException(e.what());
                    }
                    catch(boost::numeric::positive_overflow& e) 
                    {
                        throw RSGISImageCalcException(e.what());
                    }
                    catch(boost::numeric::bad_numeric_cast& e) 
                    {
                        throw RSGISImageCalcException(e.what());
                    }
                    
                    if(!complete)
                    {
                        if((xPxl != xPxlPrev) | (yPxl != yPxlPrev))
                        {
                            ImagePixelValuePt *pxlVal = new ImagePixelValuePt();
                            pxlVal->pt  = new geos::geom::Coordinate();
                            pxlVal->pt->x = xVal;
                            pxlVal->pt->y = yVal;
                            pxlVal->pt->z = zVal;
                            pxlVal->imgX = xPxl;
                            pxlVal->imgY = yPxl;
                            
                            pxlValues->push_back(pxlVal);
                        }
                    }
                    
                    xPxlPrev = xPxl;
                    yPxlPrev = yPxl;
                    
                    if(zVal > rayElevThreshold)
                    {
                        complete = true;
                        break;
                    }
                }
                delete[] transform;
                
                this->populateWithImageValues(image, imageBand, pxlValues);                
            }
            catch (std::exception &e) 
            {
                throw RSGISImageCalcException(e.what());
            }
            
            return pxlValues;
        }
        
        void RSGISExtractImagePixelsOnLine::populateWithImageValues(GDALDataset *image, unsigned int imageBand, std::vector<ImagePixelValuePt*> *ptPxlValues)
        {
            if(ptPxlValues->size() > 0)
            {
                try 
                {
                    if(image == NULL)
                    {
                        throw RSGISImageException("GDAL Dataset is not open.");
                    }
                    
                    if((imageBand > image->GetRasterCount()) | (imageBand == 0))
                    {
                        throw RSGISImageException("Image band is not within the GDAL dataset.");
                    }
                    
                    GDALRasterBand *gdalImgBand = image->GetRasterBand(imageBand);
                    
                    boost::uint_fast32_t imgXSize = gdalImgBand->GetXSize();
                    boost::uint_fast32_t imgYSize = gdalImgBand->GetYSize();
                    
                    for(std::vector<ImagePixelValuePt*>::iterator iterPxls = ptPxlValues->begin(); iterPxls != ptPxlValues->end(); ++iterPxls)
                    {
                        if(((*iterPxls)->imgX > imgXSize) | ((*iterPxls)->imgY > imgYSize))
                        {
                            throw RSGISImageException("Required pixel is not within the image.");
                        }
                        
                        gdalImgBand->RasterIO(GF_Read, (*iterPxls)->imgX, (*iterPxls)->imgY, 1, 1, &(*iterPxls)->value, 1, 1, GDT_Float32, 0, 0);
                    }
                    
                } 
                catch (RSGISImageException &e) 
                {
                    throw e;
                }
            }
        }
        
        RSGISExtractImagePixelsOnLine::~RSGISExtractImagePixelsOnLine()
        {

        }
        
    }
}


