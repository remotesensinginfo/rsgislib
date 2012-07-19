/*
 *  RSGISRasterizeVector.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/08/2009.
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

#include "RSGISRasterizeVector.h"

namespace rsgis{namespace vec{
	

	RSGISRasterizeVector::RSGISRasterizeVector()
	{
		this->method = rsgis::img::pixelContainsPolyCenter;
	}
	
	GDALDataset* RSGISRasterizeVector::createDataset(GDALDriver *gdalDriver, OGRLayer *layer, std::string filename, float resolution, float constVal) throw(RSGISImageException)
	{
		RSGISVectorUtils vecUtils;
		
		OGRFeature *feature = NULL;
		OGRGeometry *geometry = NULL;
		
		geos::geom::Envelope *env = new geos::geom::Envelope();
		geos::geom::Envelope *tmpEnv = NULL;
		
		layer->ResetReading();
		while( (feature = layer->GetNextFeature()) != NULL )
		{
			geometry = feature->GetGeometryRef();
			
			tmpEnv = vecUtils.getEnvelope(geometry);
			env->expandToInclude(tmpEnv);
			delete tmpEnv;
			
			OGRFeature::DestroyFeature(feature);
		}
				
		int imageWidth = (int) ((env->getWidth()/resolution)+1);
		int imageHeight = (int) ((env->getHeight()/resolution)+1);
		
		std::cout << "Creating an Image: [" << imageWidth << "," << imageHeight << "]\n";
	
		GDALDataset *imageDS = gdalDriver->Create(filename.c_str(), imageWidth, imageHeight, 1, GDT_Float32, gdalDriver->GetMetadata());
		
		double *gdalTranslation = new double[6];
		
		gdalTranslation[0] = env->getMinX();
		gdalTranslation[1] = resolution;
		gdalTranslation[2] = 0;
		gdalTranslation[3] = env->getMaxY();
		gdalTranslation[4] = 0;
		gdalTranslation[5] = resolution;
		
		imageDS->SetGeoTransform(gdalTranslation);
		
		char **spatRefWKT = new char*[1];
		OGRSpatialReference *ogrSpatialRef = layer->GetSpatialRef();
		ogrSpatialRef->exportToWkt(spatRefWKT);
		imageDS->SetProjection(spatRefWKT[0]);
		OGRFree(spatRefWKT);
		
		GDALRasterBand *imageBand = imageDS->GetRasterBand(1);
		float *data = (float *) CPLMalloc(sizeof(float)*imageWidth);
		
		for(int i = 0; i < imageWidth; ++i)
		{
			data[i] = constVal;
		}
		
		
		for(int i = 0; i < imageHeight; i++)
		{
			imageBand->RasterIO(GF_Write, 0, i, imageWidth, 1, data, imageWidth, 1, GDT_Float32, 0, 0);
		}
		
		delete env;
		delete[] gdalTranslation;
		delete[] data;
		
		return imageDS;
	}
	
	GDALDataset* RSGISRasterizeVector::createDataset(GDALDriver *gdalDriver, std::vector<geos::geom::Polygon*> *polys, std::string filename, float resolution, OGRSpatialReference *spatialRef, float constVal) throw(RSGISImageException)
	{
		RSGISVectorUtils vecUtils;
		
		geos::geom::Geometry *geom = vecUtils.createGeomCollection(polys);
		
		geos::geom::Envelope *env = vecUtils.getEnvelope(geom);
		
		int imageWidth = (int) ((env->getWidth()/resolution)+0.5)+5;
		int imageHeight = (int) ((env->getHeight()/resolution)+0.5)+5;
		
		GDALDataset *imageDS = gdalDriver->Create(filename.c_str(), imageWidth, imageHeight, 1, GDT_Float32, gdalDriver->GetMetadata());
		
		double *gdalTranslation = new double[6];
		
		gdalTranslation[0] = env->getMinX()-resolution;
		gdalTranslation[1] = resolution;
		gdalTranslation[2] = 0;
		gdalTranslation[3] = env->getMaxY()+resolution;
		gdalTranslation[4] = 0;
		gdalTranslation[5] = resolution;
		
		imageDS->SetGeoTransform(gdalTranslation);
		
		char **spatRefWKT = new char*[1];
		spatialRef->exportToWkt(spatRefWKT);
		imageDS->SetProjection(spatRefWKT[0]);
		OGRFree(spatRefWKT);
		
		GDALRasterBand *imageBand = imageDS->GetRasterBand(1);
		float *data = (float *) CPLMalloc(sizeof(float)*imageWidth);
		
		for(int i = 0; i < imageWidth; ++i)
		{
			data[i] = constVal;
		}
		
		for(int i = 0; i < imageHeight; ++i)
		{
			imageBand->RasterIO(GF_Write, 0, i, imageWidth, 1, data, imageWidth, 1, GDT_Float32, 0, 0);
		}
		
		delete env;
		delete[] gdalTranslation;
		delete[] data;
		
		return imageDS;
	}

	void RSGISRasterizeVector::rasterizeLayer(OGRLayer *layer, GDALDataset *image, std::string attribute, rsgis::img::pixelInPolyOption method) throw(RSGISVectorException)
	{
		this->method = method;
		
		RSGISVectorUtils vecUtils;
		
		try
		{
			OGRFeature *feature = NULL;
			OGRGeometry *geometry = NULL;
			
			geos::geom::Envelope *envImage = new geos::geom::Envelope();
			double *gdalTranslation = new double[6];
			image->GetGeoTransform(gdalTranslation);
			envImage->init(gdalTranslation[0], (gdalTranslation[0]+(image->GetRasterXSize()*gdalTranslation[1])), (gdalTranslation[3]-image->GetRasterYSize()*gdalTranslation[1]), gdalTranslation[3]);
			delete[] gdalTranslation;
			
			geos::geom::Envelope *env = NULL;
			
			bool useFID = false;
			int fieldIdx = -1;
			
			if(attribute == "FID")
			{
				useFID = true;
			}
			else
			{
				useFID = false;
				OGRFeatureDefn *featureDefn = layer->GetLayerDefn();
				fieldIdx = featureDefn->GetFieldIndex(attribute.c_str());
				if(fieldIdx < 0)
				{
					std::string message = "This layer does not contain a field with the name \'" + attribute + "\'";
					throw RSGISVectorException(message.c_str());
				}
			}
			
			
			float pxlValue = 0;
			
			int numFeatures = layer->GetFeatureCount(true);
			
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
			int i = 0;
			int notRasterized = 0; // Count for number of polygons not rasterized.
			std::cout << "Started" << std::flush;
			
			layer->ResetReading();
			while( (feature = layer->GetNextFeature()) != NULL )
			{
				if((numFeatures >= 10) && ((i % feedback) == 0))
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				++i;
				
				geometry = feature->GetGeometryRef();
	
				env = vecUtils.getEnvelope(geometry);
				
				if(useFID)
				{
					pxlValue = feature->GetFID();
				}
				else
				{
					pxlValue = feature->GetFieldAsDouble(fieldIdx);
				}
				
				//std::cout << "Pixel Value = " << pxlValue << std::endl;
				
				/*std::cout << "Image Size = " << envImage->getWidth() << " x " << envImage->getHeight() << std::endl;
				std::cout << "Polygon Envelope Size = " << env->getWidth() << " x " << env->getHeight() << std::endl;
				
				std::cout << "Image X = " << envImage->getMinX() << " Image Y = " << envImage->getMinY() << std::endl;
				std::cout << "Polygon X = " << env->getMinX() << " Polygon Y = " << env->getMinY() << std::endl;*/
				
				
				if(envImage->contains(env))
				{
					int pixelRasterisedCount = this->editPixels(image, pxlValue, env, geometry);
					if (pixelRasterisedCount == 0)
					{
						notRasterized++;
					}
				}
				else
				{
					//std::cout << "NOT INSIDE IMAGE\n";
					notRasterized++;
				}
				
				delete env;
				OGRFeature::DestroyFeature(feature);
			}
			std::cout << " Complete.\n";
			if(notRasterized > 0)
			{
				std::cout << "WARNING " << notRasterized << " polygons were not rasterised.\n" << std::endl;
			}
		}
		catch(RSGISVectorException &e)
		{
			throw e;
		}
	}
	
	void RSGISRasterizeVector::rasterizeLayer(geos::geom::Geometry *geom, GDALDataset *image, bool useFID, float constVal, rsgis::img::pixelInPolyOption method) throw(RSGISVectorException)
	{
		this->method = method;
		try
		{
			geos::geom::Envelope *envImage = new geos::geom::Envelope();
			double *gdalTranslation = new double[6];
			double resolution = gdalTranslation[1];
			image->GetGeoTransform(gdalTranslation);
			envImage->init(gdalTranslation[0], (gdalTranslation[0]+(image->GetRasterXSize()*resolution)), (gdalTranslation[3]-(image->GetRasterYSize()*resolution)), gdalTranslation[3]);
			delete[] gdalTranslation;
			
			unsigned int imgHeight = image->GetRasterYSize();
			unsigned int imgWidth = image->GetRasterXSize();
			
			GDALRasterBand *imageBand = image->GetRasterBand(1);
			float *inData = (float *) CPLMalloc(sizeof(float)*imgWidth);
			float *outData = (float *) CPLMalloc(sizeof(float)*imgWidth);
			
			geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel();
			geos::geom::GeometryFactory *geomFactory = new geos::geom::GeometryFactory(pm);
			
			geos::geom::Coordinate coord;
			geos::geom::Point *pt = NULL;
			
			double pxlMinX = envImage->getMinX();
			double pxlMaxY = envImage->getMaxY();
			
			double pxlX = 0;
			double pxlY = 0;
			double halfPxlWidth = resolution/2;
			
			int feedback = imgHeight/10;
			int feedbackCounter = 0;
			
			if(imgHeight >= 10)
			{
				std::cout << "Started" << std::flush;
			}
			
			for(unsigned int i = 0; i < imgHeight; ++i)
			{
				if((imgHeight >= 10) && ((i % feedback) == 0))
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				
				pxlMinX = envImage->getMinX();
				
				imageBand->RasterIO(GF_Read, 0, i, imgWidth, 1, inData, imgWidth, 1, GDT_Float32, 0, 0);
				
				for(unsigned int j = 0; j < imgWidth; ++j)
				{
					pxlX = pxlMinX + halfPxlWidth;
					pxlY = pxlMaxY - halfPxlWidth;
					coord = geos::geom::Coordinate(pxlX, pxlY, 0);
					
					pt = geomFactory->createPoint(coord);
					
					if(geom->contains(pt))
					{
						outData[j] = constVal;
					}
					else
					{
						outData[j] = inData[j];
					}
					
					delete pt;
					
					pxlMinX += resolution;
				}
				
				imageBand->RasterIO(GF_Write, 0, i, imgWidth, 1, outData, imgWidth, 1, GDT_Float32, 0, 0);
				
				pxlMaxY = pxlMaxY - resolution;
			}
			if(imgHeight >= 10)
			{
				std::cout << " Complete.\n";
			}
			
			delete[] inData;
			delete[] outData;
			
			delete geomFactory;
			delete pm;
		}
		catch(RSGISImageException &e)
		{
			throw e;
		}
	}

	int RSGISRasterizeVector::editPixels(GDALDataset *image, float pixelValue, geos::geom::Envelope *env, OGRGeometry *geom) throw(RSGISImageException)
	{
		long pixelsEdited = 0; // Count for number of pixels edited
		
		try
		{
			//env = vecUtils.getEnvelopePixelBuffer(geom, resolution);
			
			geos::geom::Envelope *envImage = new geos::geom::Envelope();
			double *gdalTranslation = new double[6];
			image->GetGeoTransform(gdalTranslation);
			envImage->init(gdalTranslation[0], (gdalTranslation[0]+(image->GetRasterXSize()*gdalTranslation[1])), (gdalTranslation[3]-image->GetRasterYSize()*gdalTranslation[1]), gdalTranslation[3]);
			
			float resolution = gdalTranslation[1];
			int startXPxl = 0;
			int startYPxl = 0;
			int width = 0;
			int height = 0;
			if((env->getWidth() < resolution) | (env->getHeight() < resolution))
			{
				startXPxl = (int)(((env->getMinX()-envImage->getMinX())/resolution))-1;
				startYPxl = (int)(((envImage->getMaxY()-env->getMaxY())/resolution))+1;
				width = (int)((env->getWidth()/resolution)+0.5)+2;
				height = (int)((env->getHeight()/resolution)+0.5)+2;
			}
			else
			{
				startXPxl = (int)(((env->getMinX()-envImage->getMinX())/resolution));
				startYPxl = (int)(((envImage->getMaxY()-env->getMaxY())/resolution));
				width = (int)((env->getWidth()/resolution)+0.5);
				height = (int)((env->getHeight()/resolution)+0.5);
			}

			//std::cout << "Start: [" << startXPxl << "," << startYPxl << "]\t Width = " << width << " Height = " << height << std::endl;
			
			GDALRasterBand *imageBand = image->GetRasterBand(1);
			float *inData = (float *) CPLMalloc(sizeof(float)*width);
			float *outData = (float *) CPLMalloc(sizeof(float)*width);
			
			float pxlXMin = envImage->getMinX()+(startXPxl*resolution);
			float pxlXMax = pxlXMin + resolution;
			float pxlYMax = envImage->getMaxY()-(startYPxl*resolution);
			float pxlYMin = pxlYMax - resolution;
			
			OGRLinearRing *ring = NULL;
			OGRPolygon *poly = NULL;
			
            rsgis::img::RSGISPixelInPoly *pixelInPoly = NULL;
			pixelInPoly = new rsgis::img::RSGISPixelInPoly(method);
			
			for(int i = 0; i < height; ++i)
			{
				pxlXMin = envImage->getMinX()+(startXPxl*resolution);
				pxlXMax = pxlXMin + resolution;
				
				if(width == 0)
				{
					break;
				}
				
				imageBand->RasterIO(GF_Read, startXPxl, (i+startYPxl), width, 1, inData, width, 1, GDT_Float32, 0, 0);
				
				for(int j = 0; j < width; ++j)
				{
					
					if (method == rsgis::img::polyContainsPixelCenter) 
					{
						// For pixelContainsPolyCenter, quicker to calculate here.
						OGRPoint *centerPoint = NULL;
						centerPoint = new OGRPoint(pxlXMin + (resolution / 2),pxlYMin + (resolution - 2));
						
						if(geom->Contains(centerPoint))
						{
							outData[j] = pixelValue;
							pixelsEdited++; 
						}
						else 
						{
							outData[j] = inData[j];
						}
						
						delete centerPoint;
					}
					else 
					{
						// Create polygon to represent pixel
						ring = new OGRLinearRing();
						ring->addPoint(pxlXMin, pxlYMin, 0);
						ring->addPoint(pxlXMax, pxlYMin, 0);
						ring->addPoint(pxlXMax, pxlYMax, 0);
						ring->addPoint(pxlXMin, pxlYMax, 0);
						ring->addPoint(pxlXMin, pxlYMin, 0);
						
						poly = new OGRPolygon();
						poly->addRingDirectly(ring);
						
						// Check if the pixel should be conted as part of the polygon using the specified method
						if (pixelInPoly->findPixelInPoly(geom, poly)) 
						{
							outData[j] = pixelValue;
							pixelsEdited++; 
						}
						
						else
						{
							outData[j] = inData[j];
						}
						
						delete poly;
					}
					
					//std::cout << std::endl;
									
					pxlXMin += resolution;
					pxlXMax += resolution;
				}
				
				imageBand->RasterIO(GF_Write, startXPxl, (i+startYPxl), width, 1, outData, width, 1, GDT_Float32, 0, 0);
				
				pxlYMin -= resolution;
				pxlYMax -= resolution;
			}
						
			delete[] gdalTranslation;
			delete[] outData;
			delete[] inData;
			delete envImage;
			delete pixelInPoly;

		}
		catch(RSGISImageException &e)
		{
			throw e;
		}
		
		return pixelsEdited;
	}
	
	/*
	void RSGISRasterizeVector::editPixels(GDALDataset *image, float pixelValue, Envelope *env, Geometry *geom) throw(RSGISImageException)
	{
		try
		{
			Envelope *envImage = new Envelope();
			double *gdalTranslation = new double[6];
			image->GetGeoTransform(gdalTranslation);
			envImage->init(gdalTranslation[0], (gdalTranslation[0]+(image->GetRasterXSize()*gdalTranslation[1])), (gdalTranslation[3]-image->GetRasterYSize()*gdalTranslation[1]), gdalTranslation[3]);
			
			float resolution = gdalTranslation[1];
			int startXPxl = (int)(((env->getMinX()-envImage->getMinX())/resolution));
			int startYPxl = (int)(((envImage->getMaxY()-env->getMaxY())/resolution));
			int width = (int)((env->getWidth()/resolution)+0.5);
			int height = (int)((env->getHeight()/resolution)+0.5);
			
			//std::cout << "Start: [" << startXPxl << "," << startYPxl << "]\t Width = " << width << " Height = " << height << std::endl;
			
			GDALRasterBand *imageBand = image->GetRasterBand(1);
			float *inData = (float *) CPLMalloc(sizeof(float)*width);
			float *outData = (float *) CPLMalloc(sizeof(float)*width);
			
			float pxlXMin = envImage->getMinX()+(startXPxl*resolution);
			float pxlXMax = pxlXMin + resolution;
			float pxlYMax = envImage->getMaxY()-(startYPxl*resolution);
			float pxlYMin = pxlYMax - resolution;
			
			CoordinateSequence *coords = NULL;
			LinearRing *ring = NULL;
			Polygon *poly = NULL;
			
			PrecisionModel *pm = new PrecisionModel();
			GeometryFactory *geomFactory = new GeometryFactory(pm);
			
			for(int i = 0; i < height; ++i)
			{
				pxlXMin = envImage->getMinX()+(startXPxl*resolution);
				pxlXMax = pxlXMin + resolution;
				
				imageBand->RasterIO(GF_Read, startXPxl, (i+startYPxl), width, 1, inData, width, 1, GDT_Float32, 0, 0);
				
				for(int j = 0; j < width; ++j)
				{
					coords = new CoordinateArraySequence();
					coords->add(Coordinate(pxlXMin, pxlYMin, 0));
					coords->add(Coordinate(pxlXMax, pxlYMin, 0));
					coords->add(Coordinate(pxlXMax, pxlYMax, 0));
					coords->add(Coordinate(pxlXMin, pxlYMax, 0));
					coords->add(Coordinate(pxlXMin, pxlYMin, 0));
					
					ring = geomFactory->createLinearRing(coords);
					poly = geomFactory->createPolygon(ring, NULL);
					
					if(geom->contains(poly))
					{
						outData[j] = pixelValue;
					}
					else
					{
						outData[j] = inData[j];
					}
					
					delete poly;
					
					pxlXMin += resolution;
					pxlXMax += resolution;
				}
				
				imageBand->RasterIO(GF_Write, startXPxl, (i+startYPxl), width, 1, outData, width, 1, GDT_Float32, 0, 0);
				
				pxlYMin -= resolution;
				pxlYMax -= resolution;
			}
			
			delete geomFactory;
			delete pm;
			
			delete[] gdalTranslation;
			delete[] outData;
			delete[] inData;
		}
		catch(RSGISImageException &e)
		{
			throw e;
		}
	}
	 */
	
	RSGISRasterizeVector::~RSGISRasterizeVector()
	{
		
	}
	
}}


