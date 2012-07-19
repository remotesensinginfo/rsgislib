/*
 *  RSGISPointZonalStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/09/2009.
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

#include "RSGISVectorZonalStats.h"

namespace rsgis{namespace vec{
	
	RSGISVectorZonalStats::RSGISVectorZonalStats(GDALDataset *image)
	{
		this->image = image;
		this->numImgBands = image->GetRasterCount();
		bands = new GDALRasterBand*[numImgBands];
		for(int i = 0; i < numImgBands; ++i)
		{
			bands[i] = image->GetRasterBand(i+1);
		}
		
		double geoTransform[6];
		
		if( image->GetGeoTransform( geoTransform ) == CE_None )
		{
			double xMin = geoTransform[0];
			double yMax = geoTransform[3];
			
			//std::cout << "Origin [" << geoTransform[0] << "," << geoTransform[3] << "]\n";
			//std::cout << "Size [" << image->GetRasterXSize() << "," << image->GetRasterYSize() << "]\n";
			//std::cout << "Resolution [" << geoTransform[1] << "," << geoTransform[5] << "]\n";
			
			double xMax = geoTransform[0] + (image->GetRasterXSize() * geoTransform[1]);
			double yMin = geoTransform[3] + (image->GetRasterYSize() * geoTransform[5]);
			
			imageExtent = new geos::geom::Envelope(xMin, xMax, yMin, yMax);
			
			imgRes = geoTransform[1];
			
			//std::cout << "Image Extent: [" << imageExtent->getMinX() << "," << imageExtent->getMinY() << "][" << imageExtent->getMaxX() << "," << imageExtent->getMaxY() << "]\n";
		
			pxlValues = (float *) CPLMalloc(sizeof(float));
		}

	}
	
	void RSGISVectorZonalStats::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
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
			
			//std::cout << "Point [" << point->getX() << "," << point->getY() << "]\n";
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			
			if( (point->getX() > imageExtent->getMinX()) &&
			    (point->getX() < imageExtent->getMaxX()) &&
				(point->getY() > imageExtent->getMinY()) &&
			    (point->getY() < imageExtent->getMaxY()))
			{
				//std::cout << "Point within image\n";
				double xDiff = point->getX() - imageExtent->getMinX();
				double yDiff = imageExtent->getMaxY() - point->getY();
				
				int xPxl = static_cast<int> (xDiff/imgRes);
				int yPxl = static_cast<int> (yDiff/imgRes);
				
				//std::cout << "Pixel [" << xPxl << "," << yPxl << "]\n";
				float *values = this->getPixelColumns(xPxl, yPxl);
				rsgis::math::RSGISMathsUtils mathUtils;
				std::string fieldname = "";
				for(int i = 0; i < numImgBands; ++i)
				{
					//std::cout << values[i] << ",";
					fieldname = std::string("b") + mathUtils.inttostring(i);
					outFeature->SetField(outFeatureDefn->GetFieldIndex(fieldname.c_str()), values[i]);
				}
				//std::cout << std::endl;
			}
			else 
			{
				//throw RSGISVectorException("Point not within image.");
				std::cout << "WARNING: Point not within image\n";
				
                rsgis::math::RSGISMathsUtils mathUtils;
				std::string fieldname = "";
				
				for(int i = 0; i < numImgBands; ++i)
				{
					//std::cout << values[i] << ",";
					fieldname = std::string("b") + mathUtils.inttostring(i);
					outFeature->SetField(outFeatureDefn->GetFieldIndex(fieldname.c_str()), 0);
				}
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
	
	void RSGISVectorZonalStats::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not implemented");
	}
	
	void RSGISVectorZonalStats::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
        rsgis::math::RSGISMathsUtils mathUtils;
		std::string fieldname = "";
		for(int i = 0; i < numImgBands; ++i)
		{
			fieldname = std::string("b") + mathUtils.inttostring(i);
			OGRFieldDefn shpField(fieldname.c_str(), OFTReal);
			shpField.SetPrecision(10);
			if(outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field ") + fieldname + std::string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
	}
	
	float* RSGISVectorZonalStats::getPixelColumns(int xPxl, int yPxl)
	{
		float *values = new float[numImgBands];
		for(int i = 0; i < numImgBands; ++i)
		{
			bands[i]->RasterIO(GF_Read, xPxl, yPxl, 1, 1, pxlValues, 1, 1, GDT_Float32, 0, 0);
			values[i] = pxlValues[0];
		}
		return values;
	}
	
	RSGISVectorZonalStats::~RSGISVectorZonalStats()
	{
		delete[] bands;
		delete imageExtent;
		delete[] pxlValues;
	}
}}

