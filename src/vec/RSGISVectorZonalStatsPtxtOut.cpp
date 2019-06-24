/*
 *  RSGISVectorZonalStatsPtxtOut.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/11/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
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

#include "RSGISVectorZonalStatsPtxtOut.h"


namespace rsgis{namespace vec{
	
	RSGISVectorZonalStatsPtxtOut::RSGISVectorZonalStatsPtxtOut(GDALDataset *image, rsgis::utils::RSGISExportForPlottingIncremental *plotter, int bX, int bY, int bZ, int bC )
	{
		this->plotter = plotter;
		this->bX = bX;
		this->bY = bY;
		this->bZ = bZ;
		this->bC = bC;
		this->image = image;
		this->numImgBands = image->GetRasterCount();
		bands = new GDALRasterBand*[numImgBands];
		for(int i = 0; i < numImgBands; ++i)
		{
			bands[i] = image->GetRasterBand(i+1);
		}
		
		if(plotter->getPlotType() == rsgis::utils::scatter2d)
		{
			if(bX < 0 | bY < 0)
			{
				throw RSGISVectorException("Either bX or bY are less than zero");
			}
			
			if(bX >= numImgBands | bY >= numImgBands)
			{
				throw RSGISVectorException("Either bX or bY are greater than the number of image bands");
			}
		}
		else if(plotter->getPlotType() == rsgis::utils::cscatter2d)
		{
			if(bX < 0 | bY < 0 | bC < 0)
			{
				throw RSGISVectorException("Either bX or bY or bC are less than zero");
			}
			
			if(bX >= numImgBands | bY >= numImgBands | bC >= numImgBands)
			{
				throw RSGISVectorException("Either bX or bY or bC are greater than the number of image bands");
			}
		}
		else if(plotter->getPlotType() == rsgis::utils::scatter3d)
		{
			if(bX < 0 | bY < 0 | bZ < 0)
			{
				throw RSGISVectorException("Either bX or bY or bZ are less than zero");
			}
			
			if(bX >= numImgBands | bY >= numImgBands | bZ >= numImgBands)
			{
				throw RSGISVectorException("Either bX or bY or bZ are greater than the number of image bands");
			}
		}
		else if(plotter->getPlotType() == rsgis::utils::cscatter3d)
		{
			if(bX < 0 | bY < 0 | bZ < 0 | bC < 0)
			{
				throw RSGISVectorException("Either bX or bY or bZ or bC are less than zero");
			}
			
			if(bX >= numImgBands | bY >= numImgBands | bZ >= numImgBands | bC >= numImgBands)
			{
				throw RSGISVectorException("Either bX or bY or bZ or bC are greater than the number of image bands");
			}
		}
		else 
		{
			throw RSGISVectorException("Unknown output plotting type.");
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
						
			pxlValues = (float *) CPLMalloc(sizeof(float));
		}
		
	}
	
	void RSGISVectorZonalStatsPtxtOut::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
	{
		OGRGeometry *geometry = inFeature->GetGeometryRef();
		if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
		{
			//OGRPolygon *polygon = (OGRPolygon *) geometry;
			throw RSGISVectorException("Polygons not implemented yet.");
		} 
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiPolygon )
		{
			//OGRMultiPolygon *multiPolygon = (OGRMultiPolygon *) geometry;
			throw RSGISVectorException("Multi-Polygons not implemented yet.");
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
				
				float *values = this->getPixelColumns(xPxl, yPxl);
				
				if(plotter->getPlotType() == rsgis::utils::scatter2d)
				{
					plotter->writeScatter2DLine(values[bX], values[bY]);
				}
				else if(plotter->getPlotType() == rsgis::utils::cscatter2d)
				{
					plotter->writeCScatter2DLine(values[bX], values[bY], values[bC]);
				}
				else if(plotter->getPlotType() == rsgis::utils::scatter3d)
				{
					plotter->writeScatter3DLine(values[bX], values[bY], values[bZ]);
				}
				else if(plotter->getPlotType() == rsgis::utils::cscatter3d)
				{
					plotter->writeCScatter3DLine(values[bX], values[bY], values[bZ], values[bC]);
				}
				else 
				{
					throw RSGISVectorException("Unknown output plotting type.");
				}
			}
			else 
			{
                std::cout << "WARNING: Point not within image\n";
			}
		}	
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbLineString )
		{
			//OGRLineString *line = (OGRLineString *) geometry;
			throw RSGISVectorException("Polylines not implemented yet.");
		}
		else if(geometry != NULL)
		{
            std::string message = std::string("Unsupport data type: ") + std::string(geometry->getGeometryName());
			throw RSGISVectorException(message);
		}
		else 
		{
			throw RSGISVectorException("WARNING: NULL Geometry Present within input file");
		}
	}
	
	void RSGISVectorZonalStatsPtxtOut::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
	{
		throw RSGISVectorException("Not implemented");
	}
	
	void RSGISVectorZonalStatsPtxtOut::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
	{

	}
	
	float* RSGISVectorZonalStatsPtxtOut::getPixelColumns(int xPxl, int yPxl)
	{
		float *values = new float[numImgBands];
		for(int i = 0; i < numImgBands; ++i)
		{
			bands[i]->RasterIO(GF_Read, xPxl, yPxl, 1, 1, pxlValues, 1, 1, GDT_Float32, 0, 0);
			values[i] = pxlValues[0];
		}
		return values;
	}
	
	RSGISVectorZonalStatsPtxtOut::~RSGISVectorZonalStatsPtxtOut()
	{
		delete[] bands;
		delete imageExtent;
		delete[] pxlValues;
	}
}}




