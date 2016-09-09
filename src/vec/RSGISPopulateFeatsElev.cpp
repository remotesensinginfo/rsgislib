/*
 *  RSGISPopulateFeatsElev.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 26/06/2014.
 *  Copyright 2014 RSGISLib. All rights reserved.
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

#include "RSGISPopulateFeatsElev.h"

namespace rsgis{namespace vec{
	
	RSGISPopulateFeatsElev::RSGISPopulateFeatsElev(GDALDataset *image, unsigned int band)throw(RSGISVectorException)
	{
        this->image = image;
		this->band = band;
        this->numImgBands = image->GetRasterCount();
        
        if(band > numImgBands)
        {
            throw RSGISVectorException("The band specified is not within the image.");
        }
        this->band -= 1; // set so using array index rather than image.
        
		this->bands = new GDALRasterBand*[numImgBands];
        
		for(int i = 0; i < numImgBands; ++i)
		{
			bands[i] = image->GetRasterBand(i+1);
		}
		
		double geoTransform[6];
		
		if( image->GetGeoTransform( geoTransform ) == CE_None )
		{
			double xMin = geoTransform[0];
			double yMax = geoTransform[3];
			
			double xMax = geoTransform[0] + (image->GetRasterXSize() * geoTransform[1]);
			double yMin = geoTransform[3] + (image->GetRasterYSize() * geoTransform[5]);
			
			imageExtent = new geos::geom::Envelope(xMin, xMax, yMin, yMax);
			
			imgRes = geoTransform[1];
            
			pxlValues = (double *) CPLMalloc(sizeof(double));
		}
    }
	
	void RSGISPopulateFeatsElev::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		OGRGeometry *geometry = inFeature->GetGeometryRef();
		if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
		{
			OGRPolygon *poly = (OGRPolygon *) geometry;
            OGRPolygon *nPoly = new OGRPolygon();
            
            nPoly->addRingDirectly(this->popZfield(poly->getExteriorRing()));
            unsigned int numInternalRings = poly->getNumInteriorRings();
            for(unsigned int i = 0; i < numInternalRings; ++i)
            {
                nPoly->addRingDirectly(this->popZfield(poly->getInteriorRing(i)));
            }
            
            outFeature->SetGeometryDirectly(nPoly);
            
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiPolygon )
		{
			OGRMultiPolygon *mPoly = (OGRMultiPolygon *) geometry;
            OGRMultiPolygon *nMPoly = new OGRMultiPolygon();
            OGRPolygon *poly = NULL;
            
            unsigned int numPolys = mPoly->getNumGeometries();
            for(unsigned int n = 0; n < numPolys; ++n)
            {
                poly = (OGRPolygon *) mPoly->getGeometryRef(n);
                
                OGRPolygon *nPoly = new OGRPolygon();
                
                nPoly->addRingDirectly(this->popZfield(poly->getExteriorRing()));
                unsigned int numInternalRings = poly->getNumInteriorRings();
                for(unsigned int i = 0; i < numInternalRings; ++i)
                {
                    nPoly->addRingDirectly(this->popZfield(poly->getInteriorRing(i)));
                }
                
                nMPoly->addGeometryDirectly(nPoly);
            }
            
            outFeature->SetGeometryDirectly(nMPoly);
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPoint )
		{
			OGRPoint *point = (OGRPoint *) geometry;
			
			if( (point->getX() > imageExtent->getMinX()) &&
               (point->getX() < imageExtent->getMaxX()) &&
               (point->getY() > imageExtent->getMinY()) &&
               (point->getY() < imageExtent->getMaxY()))
			{
				double xDiff = point->getX() - imageExtent->getMinX();
				double yDiff = imageExtent->getMaxY() - point->getY();
				
				int xPxl = static_cast<int> (xDiff/imgRes);
				int yPxl = static_cast<int> (yDiff/imgRes);
				
				double *values = this->getPixelColumns(xPxl, yPxl);

                point->setZ(values[this->band]);
                
                delete[] values;
                
			}
			else
			{
				throw RSGISVectorException("Point not within image file provided\n");
			}
            
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbLineString )
		{
			throw RSGISVectorException("Polylines not implemented yet.");
		}
		else if(geometry != NULL)
		{
			std::string message = std::string("Unsupported data type: ") + std::string(geometry->getGeometryName());
			throw RSGISVectorException(message);
		}
		else
		{
			throw RSGISVectorException("WARNING: NULL Geometry Present within input file");
		}
	}
	
	void RSGISPopulateFeatsElev::processFeature(OGRFeature *inFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
        throw RSGISVectorException("RSGISPopulateFeatsElev::processFeature(OGRFeature *inFeature, geos::geom::Envelope *env, long fid) is not implemented as not needed.");
	}
	
	void RSGISPopulateFeatsElev::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
        
	}
	
	double* RSGISPopulateFeatsElev::getPixelColumns(int xPxl, int yPxl)
	{
		double *values = new double[numImgBands];
		for(int i = 0; i < numImgBands; ++i)
		{
			bands[i]->RasterIO(GF_Read, xPxl, yPxl, 1, 1, pxlValues, 1, 1, GDT_Float64, 0, 0);
			values[i] = pxlValues[0];
		}
		return values;
	}
    
    OGRLinearRing* RSGISPopulateFeatsElev::popZfield(OGRLinearRing *inGeomRing) throw(RSGISVectorOutputException)
    {
        OGRLinearRing *outGeomRing = new OGRLinearRing();
        try
        {
            unsigned int numPts = inGeomRing->getNumPoints();
            OGRPoint *point = new OGRPoint();
            for(unsigned int i = 0; i < numPts; ++i)
            {
                inGeomRing->getPoint(i, point);
                
                //std::cout << "POINT: [" << point->getX() << ", " << point->getY() << "]" << std::endl;
                //std::cout << "IMAGE [XMIN, XMAX][YMIN, YMAX]: [" << imageExtent->getMinX() << ", " << imageExtent->getMaxX() << "][" << imageExtent->getMinY() << ", " << imageExtent->getMaxY() << "]" << std::endl;
                
                if((point->getX() >= imageExtent->getMinX()) &&
                   (point->getX() <= imageExtent->getMaxX()) &&
                   (point->getY() >= imageExtent->getMinY()) &&
                   (point->getY() <= imageExtent->getMaxY()))
                {
                    double xDiff = point->getX() - imageExtent->getMinX();
                    double yDiff = imageExtent->getMaxY() - point->getY();
                    
                    int xPxl = static_cast<int> (xDiff/imgRes);
                    int yPxl = static_cast<int> (yDiff/imgRes);
                    
                    double *values = this->getPixelColumns(xPxl, yPxl);
                    
                    //std::cout << "Setting Z = " << values[this->band] << std::endl;
                    
                    point->setZ(values[this->band]);
                    
                    delete[] values;
                    
                    //std::cout << "POINT: [" << point->getX() << ", " << point->getY() << ", " << point->getZ() << "]" << std::endl;
                    
                    outGeomRing->setPoint(i, point);
                }
                else
                {
                    throw RSGISVectorException("Point not within image file provided\n");
                }
            }
        
        }
        catch (RSGISVectorOutputException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw RSGISVectorOutputException(e.what());
        }
        
        outGeomRing->closeRings();
        
        return outGeomRing;
    }
	
	RSGISPopulateFeatsElev::~RSGISPopulateFeatsElev()
	{
        delete[] this->bands;
		delete this->imageExtent;
		delete[] this->pxlValues;
	}
}}

