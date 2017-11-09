/*
 *  RSGISCalcImageSingle.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/07/2008.
 *  Copyright 2008 RSGISLib.
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

#include "RSGISCalcImageSingle.h"

namespace rsgis{namespace img{
	
	RSGISCalcImageSingle::RSGISCalcImageSingle(RSGISCalcImageSingleValue *valueCalc)
	{
		this->valueCalc = valueCalc;
	}
	
	void RSGISCalcImageSingle::calcImage(GDALDataset **datasetsA, GDALDataset **datasetsB, int numDS, double *outputValue, int bandA, int bandB) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = NULL;
		
		int totalNumDS = numDS + numDS;
		GDALDataset **datasets = NULL;
		
		int **dsOffsets = NULL;		
		int **bandOffsetsA = NULL;
		int **bandOffsetsB = NULL;
		int height = 0;
		int width = 0;
		int numInBandsDSA = 0;
		int numInBandsDSB = 0;
		int numInBands = 0;
		
		float **inputDataA = NULL;
		float **inputDataB = NULL;
		float *inDataColumnA = NULL;
		float *inDataColumnB = NULL;
		
		GDALRasterBand **inputRasterBandsA = NULL;
		GDALRasterBand **inputRasterBandsB = NULL;
		
		try
		{
			
			datasets = new GDALDataset*[totalNumDS];
			for(int i = 0; i < numDS; i++)
			{
				datasets[i] = datasetsA[i];
			}
			for(int i = 0; i < numDS; i++)
			{
				datasets[i+numDS] = datasetsB[i];
			}
			
			dsOffsets = new int*[totalNumDS];
			for(int i = 0; i < totalNumDS; i++)
			{
				dsOffsets[i] = new int[2];
			}
			
			
			// Find image overlap
			gdalTranslation = new double[6];
			imgUtils.getImageOverlap(datasets, totalNumDS, dsOffsets, &width, &height, gdalTranslation);
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBandsDSA += datasetsA[i]->GetRasterCount();
			}
			
			for(int i = 0; i < numDS; i++)
			{
				numInBandsDSB += datasetsB[i]->GetRasterCount();
			}

			numInBands = numInBandsDSA;
			
			// Get Image Input Bands
			bandOffsetsA = new int*[numInBands];
			inputRasterBandsA = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasetsA[i]->GetRasterCount(); j++)
				{
					inputRasterBandsA[counter] = datasetsA[i]->GetRasterBand(j+1);
					bandOffsetsA[counter] = new int[2];
					bandOffsetsA[counter][0] = dsOffsets[i][0];
					bandOffsetsA[counter][1] = dsOffsets[i][1];
					counter++;
				}
			}
			
			bandOffsetsB = new int*[numInBands];
			inputRasterBandsB = new GDALRasterBand*[numInBands];
			counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasetsB[i]->GetRasterCount(); j++)
				{
					inputRasterBandsB[counter] = datasetsB[i]->GetRasterBand(j+1);
					bandOffsetsB[counter] = new int[2];
					bandOffsetsB[counter][0] = dsOffsets[i+numDS][0];
					bandOffsetsB[counter][1] = dsOffsets[i+numDS][1];
					counter++;
				}
			}
			
			// Allocate memory
			inputDataA = new float*[numInBandsDSA];
			for(int i = 0; i < numInBandsDSA; i++)
			{
				inputDataA[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumnA = new float[numInBandsDSA];
			
			inputDataB = new float*[numInBandsDSB];
			for(int i = 0; i < numInBandsDSB; i++)
			{
				inputDataB[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumnB = new float[numInBandsDSB];
			
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				if((i % feedback) == 0)
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				
				for(int n = 0; n < numInBandsDSA; n++)
				{
					inputRasterBandsA[n]->RasterIO(GF_Read, bandOffsetsA[n][0], (bandOffsetsA[n][1]+i), width, 1, inputDataA[n], width, 1, GDT_Float32, 0, 0);
				}
				for(int n = 0; n < numInBandsDSB; n++)
				{
					inputRasterBandsB[n]->RasterIO(GF_Read, bandOffsetsB[n][0], (bandOffsetsB[n][1]+i), width, 1, inputDataB[n], width, 1, GDT_Float32, 0, 0);
				}
				
 				for(int j = 0; j < width; j++)
				{
					for(int n = 0; n < numInBandsDSA; n++)
					{
						inDataColumnA[n] = inputDataA[n][j];
					}
					for (int n = 0; n < numInBandsDSB; n++) 
					{
						inDataColumnB[n] = inputDataB[n][j];
					}
					this->valueCalc->calcImageValue(inDataColumnA, inDataColumnB, numInBands, bandA, bandB);
				}
			}
			std::cout << " Complete.\n";
			
			double *tempOutVal;
			tempOutVal = valueCalc->getOutputValues();
			
			for(int i = 0; i < valueCalc->getNumberOfOutValues(); i++)
			{
				outputValue[i] = tempOutVal[i];
			}
			
		}
		catch(RSGISImageCalcException e)
		{
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			throw e;
		}
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(datasets != NULL)
		{
			delete[] datasets;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < totalNumDS; i++)
			{
				delete[] dsOffsets[i];
			}
			delete[] dsOffsets;
		}
		
		if(inputDataA != NULL)
		{
			for(int i = 0; i < numInBandsDSA; i++)
			{
				if(inputDataA[i] != NULL)
				{
					delete[] inputDataA[i];
				}
			}
			delete[] inputDataA;
		}	
		
		if(inputDataB != NULL)
		{
			for(int i = 0; i < numInBandsDSB; i++)
			{
				if(inputDataB[i] != NULL)
				{
					delete[] inputDataB[i];
				}
			}
			delete[] inputDataB;
		}	
		
		if(inDataColumnA != NULL)
		{
			delete[] inDataColumnA;
		}
		
		if(inDataColumnB != NULL)
		{
			delete[] inDataColumnB;
		}
		
		if(inputRasterBandsA != NULL)
		{
			delete[] inputRasterBandsA;
		}
		
		if(inputRasterBandsB != NULL)
		{
			delete[] inputRasterBandsB;
		}
	}
	
	void RSGISCalcImageSingle::calcImage(GDALDataset **datasetsA, int numDS, double *outputValue, int band) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = NULL;
		
		int **dsOffsets = NULL;		
		int **bandOffsetsA = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputDataA = NULL;
		float *inDataColumnA = NULL;
		
		GDALRasterBand **inputRasterBandsA = NULL;		
		try
		{
			dsOffsets = new int*[numDS];
			for(int i = 0; i < numDS; i++)
			{
				dsOffsets[i] = new int[2];
			}
			
			
			// Find image overlap
			gdalTranslation = new double[6];
			imgUtils.getImageOverlap(datasetsA, numDS, dsOffsets, &width, &height, gdalTranslation);
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasetsA[i]->GetRasterCount();
			}
			
			// Get Image Input Bands
			bandOffsetsA = new int*[numInBands];
			inputRasterBandsA = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasetsA[i]->GetRasterCount(); j++)
				{
					inputRasterBandsA[counter] = datasetsA[i]->GetRasterBand(j+1);
					bandOffsetsA[counter] = new int[2];
					bandOffsetsA[counter][0] = dsOffsets[i][0];
					bandOffsetsA[counter][1] = dsOffsets[i][1];
					counter++;
				}
			}
			
			// Allocate memory
			inputDataA = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputDataA[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumnA = new float[numInBands];
			
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				if((i % feedback) == 0)
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				
				for(int n = 0; n < numInBands; n++)
				{
					inputRasterBandsA[n]->RasterIO(GF_Read, bandOffsetsA[n][0], (bandOffsetsA[n][1]+i), width, 1, inputDataA[n], width, 1, GDT_Float32, 0, 0);
				}
 				for(int j = 0; j < width; j++)
				{
					for(int n = 0; n < numInBands; n++)
					{
						inDataColumnA[n] = inputDataA[n][j];
					}
					this->valueCalc->calcImageValue(inDataColumnA, numInBands, band);
				}
			}
			std::cout << " Complete.\n";
			for(int i = 0; i < valueCalc->getNumberOfOutValues(); i++)
			{
				outputValue[i] = this->valueCalc->getOutputValues()[i];
			}
			
		}
		catch(RSGISImageCalcException e)
		{
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			throw e;
		}
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] dsOffsets[i];
			}
			delete[] dsOffsets;
		}
		
		if(inputDataA != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(inputDataA[i] != NULL)
				{
					delete[] inputDataA[i];
				}
			}
			delete[] inputDataA;
		}	
		
		if(inDataColumnA != NULL)
		{
			delete[] inDataColumnA;
		}
		
		if(inputRasterBandsA != NULL)
		{
			delete[] inputRasterBandsA;
		}
	}
	
	void RSGISCalcImageSingle::calcImageWindow(GDALDataset **datasetsA, int numDS, double *outputValue) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = NULL;
		
		int **dsOffsets = NULL;		
		int **bandOffsetsA = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputDataA = NULL;
		float *inDataColumnA = NULL;
		
		geos::geom::Envelope extent;
		double pxlTLX = 0;
		double pxlTLY = 0;
		double pxlWidth = 0;
		double pxlHeight = 0;
		
		GDALRasterBand **inputRasterBandsA = NULL;		
		try
		{
			dsOffsets = new int*[numDS];
			for(int i = 0; i < numDS; i++)
			{
				dsOffsets[i] = new int[2];
			}
			
			// Find image overlap
			gdalTranslation = new double[6];
			imgUtils.getImageOverlap(datasetsA, numDS, dsOffsets, &width, &height, gdalTranslation);
			
			pxlTLX = gdalTranslation[0];
			pxlTLY = gdalTranslation[3];
			pxlWidth = gdalTranslation[1];
			pxlHeight = gdalTranslation[1];
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasetsA[i]->GetRasterCount();
			}
			
			// Get Image Input Bands
			bandOffsetsA = new int*[numInBands];
			inputRasterBandsA = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasetsA[i]->GetRasterCount(); j++)
				{
					inputRasterBandsA[counter] = datasetsA[i]->GetRasterBand(j+1);
					bandOffsetsA[counter] = new int[2];
					bandOffsetsA[counter][0] = dsOffsets[i][0];
					bandOffsetsA[counter][1] = dsOffsets[i][1];
					counter++;
				}
			}
			
			// Allocate memory
			inputDataA = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputDataA[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumnA = new float[numInBands];
			
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				if((i % feedback) == 0)
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				
				for(int n = 0; n < numInBands; n++)
				{
					inputRasterBandsA[n]->RasterIO(GF_Read, bandOffsetsA[n][0], (bandOffsetsA[n][1]+i), width, 1, inputDataA[n], width, 1, GDT_Float32, 0, 0);
				}
 				for(int j = 0; j < width; j++)
				{
					for(int n = 0; n < numInBands; n++)
					{
						inDataColumnA[n] = inputDataA[n][j];
					}
					
					extent.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
					this->valueCalc->calcImageValue(inDataColumnA, numInBands, &extent);
					pxlTLX += pxlWidth;
				}
				pxlTLY -= pxlHeight;
				pxlTLX = gdalTranslation[0];
			}
			std::cout << " Complete.\n";
			for(int i = 0; i < valueCalc->getNumberOfOutValues(); i++)
			{
				outputValue[i] = valueCalc->getOutputValues()[i];
			}
			
		}
		catch(RSGISImageCalcException& e)
		{
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			throw e;
		}
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] dsOffsets[i];
			}
			delete[] dsOffsets;
		}
		
		if(inputDataA != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(inputDataA[i] != NULL)
				{
					delete[] inputDataA[i];
				}
			}
			delete[] inputDataA;
		}	
		
		if(inDataColumnA != NULL)
		{
			delete[] inDataColumnA;
		}
		
		if(inputRasterBandsA != NULL)
		{
			delete[] inputRasterBandsA;
		}
	}
	
	void RSGISCalcImageSingle::calcImageWithinPolygon(GDALDataset **datasets, int numDS, double *outputValue, geos::geom::Envelope *env, geos::geom::Polygon *poly, bool output, pixelInPolyOption pixelPolyOption) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = NULL;
        const geos::geom::GeometryFactory *geomFactory = geos::geom::GeometryFactory::getDefaultInstance();
		
		int **dsOffsets = NULL;		
		int **bandOffsetsA = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputDataA = NULL;
		float *inDataColumnA = NULL;
		
		geos::geom::Envelope extent;
		double pxlTLX = 0;
		double pxlTLY = 0;
		double pxlWidth = 0;
		double pxlHeight = 0;
		
		GDALRasterBand **inputRasterBandsA = NULL;		
		try
		{
			dsOffsets = new int*[numDS];
			for(int i = 0; i < numDS; i++)
			{
				dsOffsets[i] = new int[2];
			}
			
			// CHECK ENVELOPE IS AT LEAST 1 x 1 Pixel
			/* For small polygons the the envelope can be smaller than a pixel, which will cause problems.
			 * To avoid errors a buffer is applied to the envelope and this buffered envelope is used for 'getImageOverlap'
			 * The buffered envelope is created and destroyed in this class and does not effect the passed in envelope
			 */
			bool buffer = false;
			
			double *transformations = new double[6];
			datasets[0]->GetGeoTransform(transformations);
			
			// Get pixel size			
			pxlWidth = transformations[1];
			pxlHeight = transformations[5];
			
			if(pxlHeight < 0) // Check resolution is positive (negative in Southern hemisphere).
			{
				pxlHeight = pxlHeight * (-1);
			}
			
			delete[] transformations;
			
			geos::geom::Envelope *bufferedEnvelope = NULL;
			
			if ((env->getWidth() < pxlWidth) | (env->getHeight() < pxlHeight))
			{
				buffer = true;
				bufferedEnvelope = new geos::geom::Envelope(env->getMinX() - pxlWidth / 2, env->getMaxX() + pxlWidth / 2, env->getMinY() - pxlHeight / 2, env->getMaxY() + pxlHeight / 2);
			}
			
			// Find image overlap
			gdalTranslation = new double[6];
			
			if (buffer) // Use buffered envelope.
			{
				imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, bufferedEnvelope);
			}
			else // Use envelope passed in.
			{
				imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, env);
			}
            
			pxlTLX = gdalTranslation[0];
			pxlTLY = gdalTranslation[3];
			
			delete bufferedEnvelope;
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			
			// Get Image Input Bands
			bandOffsetsA = new int*[numInBands];
			inputRasterBandsA = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBandsA[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsetsA[counter] = new int[2];
					bandOffsetsA[counter][0] = dsOffsets[i][0];
					bandOffsetsA[counter][1] = dsOffsets[i][1];
					counter++;
				}
			}
			
			// Allocate memory
			inputDataA = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputDataA[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumnA = new float[numInBands];
			
			int feedback = height/10;
			if (feedback == 0) {feedback = 1;} // Set feedback to 1
			int feedbackCounter = 0;
			if(height > 100)
			{
				std::cout << "\rStarted.." << std::flush;
			}
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				if (height > 100)
				{
					if((i % feedback) == 0)
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
						feedbackCounter = feedbackCounter + 10;
					}
				}
				
				for(int n = 0; n < numInBands; n++)
				{
					inputRasterBandsA[n]->RasterIO(GF_Read, bandOffsetsA[n][0], (bandOffsetsA[n][1]+i), width, 1, inputDataA[n], width, 1, GDT_Float32, 0, 0);
				}
 				for(int j = 0; j < width; j++)
				{
					for(int n = 0; n < numInBands; n++)
					{
						inDataColumnA[n] = inputDataA[n][j];
					}
					geos::geom::Coordinate pxlCentre;
                    const geos::geom::GeometryFactory *geomFactory = geos::geom::GeometryFactory::getDefaultInstance();
					geos::geom::Point *pt = NULL;
					
					extent.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
					extent.centre(pxlCentre);
					pt = geomFactory->createPoint(pxlCentre);
					
					/* It was previously hardcoded to use 'polyContainsPixelCenter'
					 calculated here.
					 As other methods, available from 'RSGISPixelInPoly' require conversion to 
					 OGRGeometry and OGR Polygon the existing method has been retained to maintain performance.
					 Dan Clewley - 17/06/2010
					 */
					
					if (pixelPolyOption == polyContainsPixelCenter) 
					{
						if(poly->contains(pt)) // If polygon contains pixel center
						{
							this->valueCalc->calcImageValue(inDataColumnA, 1, numInBands, poly, pt);
						}

					}
					else if (pixelPolyOption == pixelAreaInPoly) 
					{
						geos::geom::CoordinateSequence *coords = NULL;
						geos::geom::LinearRing *ring = NULL;
						geos::geom::Polygon *pixelGeosPoly = NULL;
						geos::geom::Geometry *intersectionGeom;
						
						coords = new geos::geom::CoordinateArraySequence();
						coords->add(geos::geom::Coordinate(pxlTLX, pxlTLY, 0));
						coords->add(geos::geom::Coordinate(pxlTLX + pxlWidth, pxlTLY, 0));
						coords->add(geos::geom::Coordinate(pxlTLX + pxlWidth, pxlTLY - pxlHeight, 0));
						coords->add(geos::geom::Coordinate(pxlTLX, pxlTLY - pxlHeight, 0));
						coords->add(geos::geom::Coordinate(pxlTLX, pxlTLY, 0));
						ring = geomFactory->createLinearRing(coords);
						pixelGeosPoly = geomFactory->createPolygon(ring, NULL);
						
						intersectionGeom = pixelGeosPoly->intersection(poly);
						double intersectionArea = intersectionGeom->getArea()  / pixelGeosPoly->getArea();
						
						if(intersectionArea > 0)
						{
							for(int n = 0; n < numInBands; n++)
							{
								this->valueCalc->calcImageValue(inDataColumnA, intersectionArea, numInBands, poly, pt);
							}
						}
					}
					else 
					{
						RSGISPixelInPoly *pixelInPoly;
						OGRLinearRing *ring;
						OGRPolygon *pixelPoly;
						OGRPolygon *ogrPoly;
						OGRGeometry *ogrGeom;
						
						pixelInPoly = new RSGISPixelInPoly(pixelPolyOption);
						
						ring = new OGRLinearRing();
						ring->addPoint(pxlTLX, pxlTLY, 0);
						ring->addPoint(pxlTLX + pxlWidth, pxlTLY, 0);
						ring->addPoint(pxlTLX + pxlWidth, pxlTLY - pxlHeight, 0);
						ring->addPoint(pxlTLX, pxlTLY - pxlHeight, 0);
						ring->addPoint(pxlTLX, pxlTLY, 0);
						
						pixelPoly = new OGRPolygon();
						pixelPoly->addRingDirectly(ring);
												
						//Convert GEOS Polygon to OGR Polygon
						ogrPoly = new OGRPolygon();
						const geos::geom::LineString *line = poly->getExteriorRing();
						OGRLinearRing *ogrRing = new OGRLinearRing();
						const geos::geom::CoordinateSequence *coords = line->getCoordinatesRO();
						int numCoords = coords->getSize();
						geos::geom::Coordinate coord;
						for(int i = 0; i < numCoords; i++)
						{
							coord = coords->getAt(i);
							ogrRing->addPoint(coord.x, coord.y, coord.z);
						}
						ogrPoly->addRing(ogrRing);
						ogrGeom = (OGRGeometry *) ogrPoly;
						
						// Check if the pixel should be classed as part of the polygon using the specified method
						if (pixelInPoly->findPixelInPoly(ogrGeom, pixelPoly)) 
						{
							this->valueCalc->calcImageValue(inDataColumnA, 1, numInBands, poly, pt);
						}
						
						// Tidy
						delete pixelInPoly;
						delete pixelPoly;
						delete ogrPoly;
					}
					
					delete pt;
					
					pxlTLX += pxlWidth;
				}
				pxlTLY -= pxlHeight;
				pxlTLX = gdalTranslation[0];
			}
			if (height > 100) 
			{
				std::cout << "Complete\r" << std::flush;
				std::cout << "\r                                                                                    \r" << std::flush;
			}
			if(output)
			{	
				double *tempOutVal;
				tempOutVal = valueCalc->getOutputValues();
				
				for(int i = 0; i < valueCalc->getNumberOfOutValues(); i++)
				{
					outputValue[i] = tempOutVal[i];
				}
			}
		}
		catch(RSGISImageCalcException& e)
		{
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			throw e;
		}
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] dsOffsets[i];
			}
			delete[] dsOffsets;
		}
		
		if(inputDataA != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(inputDataA[i] != NULL)
				{
					delete[] inputDataA[i];
				}
			}
			delete[] inputDataA;
		}	
		
		if(bandOffsetsA != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsetsA[i] != NULL)
				{
					delete[] bandOffsetsA[i];
				}
			}
			delete[] bandOffsetsA;
		}
		
		if(inDataColumnA != NULL)
		{
			delete[] inDataColumnA;
		}
		
		if(inputRasterBandsA != NULL)
		{
			delete[] inputRasterBandsA;
		}
	}
	
	void RSGISCalcImageSingle::calcImageWithinRasterPolygon(GDALDataset **datasets, int numDS, double *outputValue, geos::geom::Envelope *env, long fid, bool output) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = NULL;
		
		int **dsOffsets = NULL;		
		int **bandOffsetsA = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputDataA = NULL;
		float *inDataColumnA = NULL;
		
		double *tmpOutput = NULL;
		
		GDALRasterBand **inputRasterBandsA = NULL;		
		try
		{
			dsOffsets = new int*[numDS];
			for(int i = 0; i < numDS; i++)
			{
				dsOffsets[i] = new int[2];
			}
			
			// CHECK ENVELOPE IS AT LEAST 1 x 1 Pixel
			/* For small polygons the the envelope can be smaller than a pixel, which will cause problems.
			 * To avoid errors a buffer is applied to the envelope and this buffered envelope is used for 'getImageOverlap'
			 * The buffered envelope is created and destroyed in this class and does not effect the passed in envelope
			 */
			bool buffer = false;
			
			double *transformations = new double[6];
			datasets[1]->GetGeoTransform(transformations);
			
			// Get pixel size
			double pixelXRes = transformations[1];
			double pixelYRes = transformations[5];
			
			if(pixelYRes < 0) // Check resolution is positive (negative in Southern hemisphere).
			{
				pixelYRes = pixelYRes * (-1);
			}
			
			delete[] transformations;
			
			geos::geom::Envelope *bufferedEnvelope = NULL;
			
			if ((env->getWidth() < pixelXRes) | (env->getHeight() < pixelYRes))
			{
				buffer = true;
				bufferedEnvelope = new geos::geom::Envelope(env->getMinX() - pixelXRes / 2, env->getMaxX() + pixelXRes / 2, env->getMinY() - pixelYRes / 2, env->getMaxY() + pixelYRes / 2);
			}
			
			// Find image overlap
			gdalTranslation = new double[6];
			
			if (buffer) // Use buffered envelope.
			{
				imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, bufferedEnvelope);
			}
			else // Use envelope passed in.
			{
				imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, env);
			}
			
			delete bufferedEnvelope;
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			
			// Get Image Input Bands
			bandOffsetsA = new int*[numInBands];
			inputRasterBandsA = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBandsA[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsetsA[counter] = new int[2];
					bandOffsetsA[counter][0] = dsOffsets[i][0];
					bandOffsetsA[counter][1] = dsOffsets[i][1];
					counter++;
				}
			}
			
			// Allocate memory
			inputDataA = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputDataA[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumnA = new float[numInBands];
			
			int feedback = height/10;
			int feedbackCounter = 0;
			if(height > 100)
			{
				std::cout << "\rStarted .." << std::flush;
			}
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				if(height > 100)
				{
					if((i % feedback) == 0)
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
						feedbackCounter = feedbackCounter + 10;
					}
				}
				
				for(int n = 0; n < numInBands; n++)
				{
					inputRasterBandsA[n]->RasterIO(GF_Read, bandOffsetsA[n][0], (bandOffsetsA[n][1]+i), width, 1, inputDataA[n], width, 1, GDT_Float32, 0, 0);
				}
 				for(int j = 0; j < width; j++)
				{
					for(int n = 0; n < numInBands; n++)
					{
						inDataColumnA[n] = inputDataA[n][j];
					}
					
					if(inDataColumnA[0] == fid)
					{
						this->valueCalc->calcImageValue(inDataColumnA, numInBands, 0);
					}					
				}
			}
			if(height > 100)
			{
				std::cout << "Complete\r" << std::flush;
				std::cout << "\r                                                                                    \r" << std::flush;
			}
			if(output)
			{
				tmpOutput = valueCalc->getOutputValues();
				for(int i = 0; i < valueCalc->getNumberOfOutValues(); i++)
				{
					outputValue[i] = tmpOutput[i];
				}
			}			
		}
		catch(RSGISImageCalcException& e)
		{
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			throw e;
		}
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] dsOffsets[i];
			}
			delete[] dsOffsets;
		}
		
		if(inputDataA != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(inputDataA[i] != NULL)
				{
					CPLFree(inputDataA[i]);
				}
			}
			delete[] inputDataA;
		}	
		
		if(bandOffsetsA != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsetsA[i] != NULL)
				{
					delete[] bandOffsetsA[i];
				}
			}
			delete[] bandOffsetsA;
		}
		
		if(inDataColumnA != NULL)
		{
			delete[] inDataColumnA;
		}
		
		if(inputRasterBandsA != NULL)
		{
			delete[] inputRasterBandsA;
		}
	}
	
	RSGISCalcImageSingleValue* RSGISCalcImageSingle::getRSGISCalcImageSingleValue()
	{
		return this->valueCalc;
	}
	
	RSGISCalcImageSingle::~RSGISCalcImageSingle()
	{
		
	}
	
}}
