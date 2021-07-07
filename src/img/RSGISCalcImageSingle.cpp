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
	
	void RSGISCalcImageSingle::calcImage(GDALDataset **datasetsA, GDALDataset **datasetsB, int numDS, double *outputValue, int bandA, int bandB)
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
			
			rsgis_tqdm pbar;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				pbar.progress(i, height);
				
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
			
			double *tempOutVal;
			tempOutVal = valueCalc->getOutputValues();
			
			for(int i = 0; i < valueCalc->getNumberOfOutValues(); i++)
			{
				outputValue[i] = tempOutVal[i];
			}
			pbar.finish();
		}
		catch(RSGISImageCalcException &e)
		{
			throw e;
		}
		catch(RSGISImageBandException &e)
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
	
	void RSGISCalcImageSingle::calcImage(GDALDataset **datasetsA, int numDS, double *outputValue, int band)
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
			
			rsgis_tqdm pbar;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				pbar.progress(i, height);
				
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
			
			for(int i = 0; i < valueCalc->getNumberOfOutValues(); i++)
			{
				outputValue[i] = this->valueCalc->getOutputValues()[i];
			}
			pbar.finish();
		}
		catch(RSGISImageCalcException &e)
		{
			throw e;
		}
		catch(RSGISImageBandException &e)
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
	
	void RSGISCalcImageSingle::calcImageWindow(GDALDataset **datasetsA, int numDS, double *outputValue)
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
		
		OGREnvelope extent;
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
			
			rsgis_tqdm pbar;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				pbar.progress(i, height);
				
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
					
                    extent.MinX = pxlTLX;
                    extent.MaxX = (pxlTLX+pxlWidth);
                    extent.MinY = pxlTLY;
                    extent.MaxY = (pxlTLY-pxlHeight);
					this->valueCalc->calcImageValue(inDataColumnA, numInBands, &extent);
					pxlTLX += pxlWidth;
				}
				pxlTLY -= pxlHeight;
				pxlTLX = gdalTranslation[0];
			}
			
			for(int i = 0; i < valueCalc->getNumberOfOutValues(); i++)
			{
				outputValue[i] = valueCalc->getOutputValues()[i];
			}
			pbar.finish();
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
	
	void RSGISCalcImageSingle::calcImageWithinPolygon(GDALDataset **datasets, int numDS, double *outputValue, OGREnvelope *env, OGRPolygon *poly, bool output, pixelInPolyOption pixelPolyOption)
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
		
		OGREnvelope extent;
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
			
			OGREnvelope *bufferedEnvelope = NULL;

            if (((env->MaxX - env->MinX) < pxlWidth) | ((env->MaxY - env->MinY) < pxlHeight))
            {
                buffer = true;
                bufferedEnvelope = new OGREnvelope();
                bufferedEnvelope->MinX = env->MinX - pxlWidth / 2;
                bufferedEnvelope->MinX = env->MaxX + pxlWidth / 2;
                bufferedEnvelope->MinY = env->MinY - pxlHeight / 2;
                bufferedEnvelope->MaxY = env->MaxY + pxlHeight / 2;
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

                    double x_pt = pxlTLX + (pxlWidth / 2);
                    double y_pt = pxlTLY - (pxlHeight / 2);

                    OGRPoint *pxlCentre = new OGRPoint(x_pt, y_pt);

                    if (pixelPolyOption == polyContainsPixelCenter)
                    {
                        if(poly->Contains(pxlCentre)) // If polygon contains pixel center
                        {
                            this->valueCalc->calcImageValue(inDataColumnA, 1, numInBands, poly, pxlCentre);
                        }
                    }
                    else if (pixelPolyOption == pixelAreaInPoly)
                    {
                        OGRLinearRing *ring = new OGRLinearRing();
                        ring->addPoint(pxlTLX, pxlTLY, 0);
                        ring->addPoint(pxlTLX + pxlWidth, pxlTLY, 0);
                        ring->addPoint(pxlTLX + pxlWidth, pxlTLY - pxlHeight, 0);
                        ring->addPoint(pxlTLX, pxlTLY - pxlHeight, 0);
                        ring->addPoint(pxlTLX, pxlTLY, 0);

                        OGRPolygon *pixelPoly = new OGRPolygon();
                        pixelPoly->addRingDirectly(ring);

                        double intersectionArea = 0.0;
                        OGRGeometry *intersectGeom = pixelPoly->Intersection(poly);
                        if(intersectGeom->getGeometryType() == wkbPolygon)
                        {
                            intersectionArea = dynamic_cast<OGRPolygon*>(intersectGeom)->get_Area()  / pixelPoly->get_Area();
                        }
                        else if(intersectGeom->getGeometryType() == wkbMultiPolygon)
                        {
                            intersectionArea = dynamic_cast<OGRMultiPolygon*>(intersectGeom)->get_Area()  / pixelPoly->get_Area();
                        }
                        else if(intersectGeom->getGeometryType() == wkbSurface)
                        {
                            intersectionArea = dynamic_cast<OGRSurface*>(intersectGeom)->get_Area()  / pixelPoly->get_Area();
                        }
                        else if(intersectGeom->getGeometryType() == wkbMultiSurface)
                        {
                            intersectionArea = dynamic_cast<OGRMultiSurface *>(intersectGeom)->get_Area() / pixelPoly->get_Area();
                        }

                        if(intersectionArea > 0)
                        {
                            for(int n = 0; n < numInBands; n++)
                            {
                                this->valueCalc->calcImageValue(inDataColumnA, 1, numInBands, poly, pxlCentre);
                            }
                        }

                        delete pixelPoly;
                        delete intersectGeom;
                    }
                    else
                    {
                        RSGISPixelInPoly *pixelInPoly;
                        OGRLinearRing *ring;
                        OGRPolygon *pixelPoly;

                        pixelInPoly = new RSGISPixelInPoly(pixelPolyOption);

                        ring = new OGRLinearRing();
                        ring->addPoint(pxlTLX, pxlTLY, 0);
                        ring->addPoint(pxlTLX + pxlWidth, pxlTLY, 0);
                        ring->addPoint(pxlTLX + pxlWidth, pxlTLY - pxlHeight, 0);
                        ring->addPoint(pxlTLX, pxlTLY - pxlHeight, 0);
                        ring->addPoint(pxlTLX, pxlTLY, 0);

                        pixelPoly = new OGRPolygon();
                        pixelPoly->addRingDirectly(ring);

                        // Check if the pixel should be classed as part of the polygon using the specified method
                        if (pixelInPoly->findPixelInPoly(poly, pixelPoly))
                        {
                            this->valueCalc->calcImageValue(inDataColumnA, 1, numInBands, poly, pxlCentre);
                        }

                        // Tidy
                        delete pixelInPoly;
                        delete pixelPoly;
                    }
                    delete pxlCentre;
					
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
	
	void RSGISCalcImageSingle::calcImageWithinRasterPolygon(GDALDataset **datasets, int numDS, double *outputValue, OGREnvelope *env, long fid, bool output)
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
			
			OGREnvelope *bufferedEnvelope = NULL;
            if (((env->MaxX - env->MinX) < pixelXRes) | ((env->MaxY - env->MinY) < pixelYRes))
            {
                buffer = true;
                bufferedEnvelope = new OGREnvelope();
                bufferedEnvelope->MinX = env->MinX - pixelXRes / 2;
                bufferedEnvelope->MinX = env->MaxX + pixelXRes / 2;
                bufferedEnvelope->MinY = env->MinY - pixelYRes / 2;
                bufferedEnvelope->MaxY = env->MaxY + pixelYRes / 2;
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
