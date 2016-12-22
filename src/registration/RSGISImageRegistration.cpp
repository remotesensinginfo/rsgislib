/*
 *  RSGISImageRegistration.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 31/08/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#include "RSGISImageRegistration.h"



namespace rsgis{namespace reg{

		
	RSGISImageRegistration::RSGISImageRegistration(GDALDataset *reference, GDALDataset *floating): referenceIMG(NULL), floatingIMG(NULL), overlap(NULL), overlapDefined(false)
	{
		this->referenceIMG = reference;
		this->floatingIMG = floating;
	}
	
	void RSGISImageRegistration::runCompleteRegistration()
	{
		std::cout << "Initialising the registration process\n"; 
		this->initRegistration();
		std::cout << "Execunting the registration\n";
		this->executeRegistration();
		std::cout << "Finalising the registration\n";
		this->finaliseRegistration();
	}
	
	void RSGISImageRegistration::findOverlap() throw(RSGISRegistrationException)
	{
		try 
		{
			// Find overlapping region.
			GDALDataset **imgs = new GDALDataset*[2];
			imgs[0] = referenceIMG;
			imgs[1] = floatingIMG;
			float **dsOffsets = new float*[2];
			dsOffsets[0] = new float[2];
			dsOffsets[1] = new float[2];
			int overlapWidth = 0;
			int overlapHeight = 0;
			double *overlapTransform = new double[6];
			this->getImageOverlapFloat(imgs, 2, dsOffsets, &overlapWidth, &overlapHeight, overlapTransform);
			
			// Store overlapping region
			overlap = new OverlapRegion();
			overlap->xRes = overlapTransform[1];
			overlap->yRes = overlapTransform[5];
			overlap->xRot = overlapTransform[2];
			overlap->yRot = overlapTransform[4];
			if(overlap->yRes < 0)
			{
				overlap->yRes = overlap->yRes * (-1);
			}
			overlap->xSize = overlapWidth;
			overlap->ySize = overlapHeight;
			overlap->tlX = overlapTransform[0];
			overlap->tlY = overlapTransform[3];
			overlap->brX = overlapTransform[0] + (((float)overlap->xRes)*overlapWidth);
			overlap->brY = overlapTransform[3] + (((float)overlap->yRes)*overlapHeight);
			overlap->refXStart = dsOffsets[0][0];
			overlap->refYStart = dsOffsets[0][1];
			overlap->floatXStart = dsOffsets[1][0];
			overlap->floatYStart = dsOffsets[1][1];
			overlap->numRefBands = referenceIMG->GetRasterCount();
			overlap->numFloatBands = floatingIMG->GetRasterCount();
			
			delete[] overlapTransform;
			delete[] dsOffsets[0];
			delete[] dsOffsets[1];
			delete[] dsOffsets;
			delete[] imgs;
		}
		catch (rsgis::img::RSGISImageBandException &e) 
		{
			throw RSGISRegistrationException(e.what());
		}
		
		overlapDefined = true;
	}
	
	void RSGISImageRegistration::defineFirstTiePoint(unsigned int *startXOff, unsigned int *startYOff, unsigned int numXPts, unsigned int numYPts, unsigned int gap) throw(RSGISRegistrationException)
	{
		if(!overlapDefined)
		{
			throw RSGISRegistrationException("The overlap needs to be defined before first tie point can be defined.");
		}
		unsigned long xRegion = (numXPts-1)*gap;
		unsigned long yRegion = (numYPts-1)*gap;
		
		unsigned int diffX = overlap->xSize - xRegion;
		unsigned int diffY = overlap->ySize - yRegion;
		
		*startXOff = diffX/2;
		*startYOff = diffY/2;

	}
	
	float RSGISImageRegistration::findTiePointLocation(TiePoint *tiePt, unsigned int windowSize, unsigned int searchArea, RSGISImageSimilarityMetric *metric, float metricThreshold, unsigned int subPixelResolution, float *moveInX, float *moveInY) throw(RSGISRegistrationException)
	{
		float distanceMoved = 0;
		
		if(!overlapDefined)
		{
			throw RSGISRegistrationException("The overlap needs to be defined before tie location can be defined.");
		}
		
		rsgis::img::RSGISImageUtils imgUtils;
		try 
		{
			// Setup overlapping region variables.
			int **dsOffsets = new int*[2];
			dsOffsets[0] = new int[2];
			dsOffsets[1] = new int[2];
			int overlapWidth = 0;
			int overlapHeight = 0;
			double *overlapTransform = new double[6];
			
			double windowXWidth = (((double)windowSize)*overlap->xRes);
			double windowYHeight = (((double)windowSize)*overlap->yRes);
			
			geos::geom::Envelope *env = new geos::geom::Envelope();
			env->init((tiePt->eastings - windowXWidth), 
					  (tiePt->eastings + windowXWidth + overlap->xRes), 
					  (tiePt->northings - windowYHeight),
					  (tiePt->northings + windowYHeight + +overlap->yRes));
			
			unsigned int numSearchPoints = (searchArea*2)+1;
			float **imageSimilarity = new float*[numSearchPoints];
			for(unsigned int i = 0; i < numSearchPoints; ++i)
			{
				imageSimilarity[i] = new float[numSearchPoints];
			}
			
			int xShiftStart = searchArea * (-1);
			int yShiftStart = searchArea * (-1);
			int xShiftEnd = searchArea;
			int yShiftEnd = searchArea;
			
			float **refDataBlock = NULL;
			float **floatDataBlock = NULL;
			unsigned int numRefDataVals = 0;
			unsigned int numFloatDataVals = 0;
			
			bool first = true;
			double currentMetricVal = 0;
			double metricVal = 0;
			int currentShiftX = 0;
			int currentShiftY = 0;
			
			unsigned int xIdx = 0;
			unsigned int yIdx = 0;
			unsigned int currentXIdx = 0;
			unsigned int currentYIdx = 0;
            
            // Set up variables to hold remainder from image overlap
            float remainderX = 0;
            float remainderY = 0;
            // Remainder for heighest metric
            float currentRemainderX = 0;
            float currentRemainderY = 0;
            
            
			
            // Move floating window over search space
			for(int yShift = yShiftStart; yShift <= yShiftEnd; ++yShift)
			{
				xIdx = 0;
				for(int xShift = xShiftStart; xShift <= xShiftEnd; ++xShift)
				{
					try
					{
						this->getImageOverlapWithFloatShift((((float)xShift)+tiePt->xShift), (((float)yShift)+tiePt->yShift), dsOffsets, &overlapWidth, &overlapHeight, overlapTransform, env, &remainderX, &remainderY);
						
						if((overlapWidth > 0) & (overlapHeight > 0))
						{
							numRefDataVals = 0;
							numFloatDataVals = 0;
							
							refDataBlock = imgUtils.getImageDataBlock(referenceIMG, dsOffsets[0], overlapWidth, overlapHeight, &numRefDataVals);
							floatDataBlock = imgUtils.getImageDataBlock(floatingIMG, dsOffsets[1], overlapWidth, overlapHeight, &numFloatDataVals);
							
							if(numRefDataVals != numFloatDataVals)
							{
								throw RSGISRegistrationException("The number of data values read from the images does not match.");
							}
							
							metricVal = metric->calcValue(refDataBlock, floatDataBlock, numRefDataVals, overlap->numRefBands);
							
							imageSimilarity[yIdx][xIdx] = metricVal;
							
							if(!((boost::math::isnan)(metricVal)))
							{
								if(first)
								{
									currentMetricVal = metricVal;
									currentShiftX = xShift;
									currentShiftY = yShift;
									currentXIdx = xIdx;
									currentYIdx = yIdx;
                                    currentRemainderX = remainderX;
                                    currentRemainderY = remainderY;
									first = false;
								}
								else if(metric->findMin() & (metricVal < currentMetricVal))
								{
									currentMetricVal = metricVal;
									currentShiftX = xShift;
									currentShiftY = yShift;
									currentXIdx = xIdx;
									currentYIdx = yIdx;
                                    currentRemainderX = remainderX;
                                    currentRemainderY = remainderY;
								}
								else if(!metric->findMin() & (metricVal > currentMetricVal))
								{
									currentMetricVal = metricVal;
									currentShiftX = xShift;
									currentShiftY = yShift;
									currentXIdx = xIdx;
									currentYIdx = yIdx;
                                    currentRemainderX = remainderX;
                                    currentRemainderY = remainderY;
								}
							}
							
							for(unsigned int i = 0; i < overlap->numRefBands; ++i)
							{
								delete[] refDataBlock[i];
								delete[] floatDataBlock[i];
							}
							delete[] refDataBlock;
							delete[] floatDataBlock;
						}
					}
					catch (RSGISRegistrationException &e) 
					{
						// ignore
						std::cerr << "Tie Point = [" << tiePt->xRef << "," << tiePt->yRef << "]\n";
						std::cerr << "Shift = [" << (xShift+tiePt->xShift) << "," << (yShift+tiePt->yShift) << "]\n";
						std::cerr << "WARNING: " << e.what() << std::endl;
					}
					xIdx++;
				}
				++yIdx;
			}
			
			float subPixelXShift = 0;
			float subPixelYShift = 0;
			float subPixelXMetric = currentMetricVal;
			float subPixelYMetric = currentMetricVal;
			
			rsgis::math::RSGISPolyFit polyFit;
									
			// Find subpixel component
			if(searchArea == 1)
			{
				// 2nd Order Poly
				// Find subpixel X
				if((currentXIdx != 0) & (currentXIdx != (numSearchPoints-1)))
				{
					gsl_matrix *inputDataMatrix = gsl_matrix_alloc(3,2);
					gsl_matrix_set (inputDataMatrix, 0, 0, -1);
					gsl_matrix_set (inputDataMatrix, 0, 1, imageSimilarity[currentYIdx][currentXIdx-1]);
					gsl_matrix_set (inputDataMatrix, 1, 0, 0);
					gsl_matrix_set (inputDataMatrix, 1, 1, imageSimilarity[currentYIdx][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 2, 0, 1);
					gsl_matrix_set (inputDataMatrix, 2, 1, imageSimilarity[currentYIdx][currentXIdx+1]);
					
					unsigned int order = 3; // 2nd Order - starts at zero.
					gsl_vector *coefficients = polyFit.PolyfitOneDimensionQuiet(order, inputDataMatrix);
					
					subPixelXShift = findExtreme(metric->findMin(), coefficients, order, -1, 1, subPixelResolution, &subPixelXMetric);
					
					gsl_matrix_free(inputDataMatrix);
				}

				// Find subpixel Y
				if((currentYIdx != 0) & (currentYIdx != (numSearchPoints-1)))
				{
					gsl_matrix *inputDataMatrix = gsl_matrix_alloc(3,2);
					gsl_matrix_set (inputDataMatrix, 0, 0, -1);
					gsl_matrix_set (inputDataMatrix, 0, 1, imageSimilarity[currentYIdx-1][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 1, 0, 0);
					gsl_matrix_set (inputDataMatrix, 1, 1, imageSimilarity[currentYIdx][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 2, 0, 1);
					gsl_matrix_set (inputDataMatrix, 2, 1, imageSimilarity[currentYIdx+1][currentXIdx]);
					
					unsigned int order = 3; // 2nd Order - starts at zero.
					gsl_vector *coefficients = polyFit.PolyfitOneDimensionQuiet(order, inputDataMatrix);
					
					subPixelYShift = findExtreme(metric->findMin(), coefficients, order, -1, 1, subPixelResolution, &subPixelYMetric);
					
					gsl_matrix_free(inputDataMatrix);
				}
			}
			else
			{
				// 4th Order Poly
				if((currentXIdx > 1) & (currentXIdx < (numSearchPoints-2)))
				{
					gsl_matrix *inputDataMatrix = gsl_matrix_alloc(5,2);
					gsl_matrix_set (inputDataMatrix, 0, 0, -2);
					gsl_matrix_set (inputDataMatrix, 0, 1, imageSimilarity[currentYIdx][currentXIdx-2]);
					gsl_matrix_set (inputDataMatrix, 1, 0, -1);
					gsl_matrix_set (inputDataMatrix, 1, 1, imageSimilarity[currentYIdx][currentXIdx-1]);
					gsl_matrix_set (inputDataMatrix, 2, 0, 0);
					gsl_matrix_set (inputDataMatrix, 2, 1, imageSimilarity[currentYIdx][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 3, 0, 1);
					gsl_matrix_set (inputDataMatrix, 3, 1, imageSimilarity[currentYIdx][currentXIdx+1]);
					gsl_matrix_set (inputDataMatrix, 4, 0, 2);
					gsl_matrix_set (inputDataMatrix, 4, 1, imageSimilarity[currentYIdx][currentXIdx+2]);
					
					unsigned int order = 4; // 3rd Order - starts at zero.
					gsl_vector *coefficients = polyFit.PolyfitOneDimensionQuiet(order, inputDataMatrix);
					
					subPixelXShift = findExtreme(metric->findMin(), coefficients, order, -1, 1, subPixelResolution, &subPixelXMetric);
					
					gsl_matrix_free(inputDataMatrix);
					gsl_vector_free(coefficients);
				}
				
				if((currentYIdx > 1) & (currentYIdx < (numSearchPoints-2)))
				{
					gsl_matrix *inputDataMatrix = gsl_matrix_alloc(5,2);
					gsl_matrix_set (inputDataMatrix, 0, 0, -2);
					gsl_matrix_set (inputDataMatrix, 0, 1, imageSimilarity[currentYIdx-2][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 1, 0, -1);
					gsl_matrix_set (inputDataMatrix, 1, 1, imageSimilarity[currentYIdx-1][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 2, 0, 0);
					gsl_matrix_set (inputDataMatrix, 2, 1, imageSimilarity[currentYIdx][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 3, 0, 1);
					gsl_matrix_set (inputDataMatrix, 3, 1, imageSimilarity[currentYIdx+1][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 4, 0, 2);
					gsl_matrix_set (inputDataMatrix, 4, 1, imageSimilarity[currentYIdx+2][currentXIdx]);
					
					unsigned int order = 4; // 3rd Order - starts at zero.
					gsl_vector *coefficients = polyFit.PolyfitOneDimensionQuiet(order, inputDataMatrix);
					
					subPixelYShift = findExtreme(metric->findMin(), coefficients, order, -1, 1, subPixelResolution, &subPixelYMetric);
					
					gsl_matrix_free(inputDataMatrix);
					gsl_vector_free(coefficients);
				}
				
			}
			
			currentMetricVal = (subPixelXMetric + subPixelYMetric)/2;
            
            // Calculate final shift, adding on offsets due to rounding in image overlap calculation
            float finalXShift = (((float)currentShiftX) + subPixelXShift) + currentRemainderX;
			float finalYShift = (((float)currentShiftY) + subPixelYShift) + currentRemainderY;
            
			distanceMoved = sqrt(((finalXShift*finalXShift)+(finalYShift*finalYShift))/2);
			*moveInX = finalXShift;
			*moveInY = finalYShift;
            
			if(metric->findMin() & (currentMetricVal < metricThreshold))
			{
				tiePt->xShift += finalXShift;
				tiePt->yShift += finalYShift;
				tiePt->metricVal = currentMetricVal;
			}
			else if(!metric->findMin() & (currentMetricVal > metricThreshold))
			{
				tiePt->xShift += finalXShift;
				tiePt->yShift += finalYShift;
				tiePt->metricVal = currentMetricVal;
			}
			else
			{
				tiePt->metricVal = std::numeric_limits<double>::signaling_NaN();//NAN;
				distanceMoved = 0;
				*moveInX = 0;
				*moveInY = 0;
			}
			
			
			delete[] overlapTransform;
			delete[] dsOffsets[0];
			delete[] dsOffsets[1];
			delete[] dsOffsets;
		}
		catch (rsgis::img::RSGISImageBandException &e) 
		{
			throw RSGISRegistrationException(e.what());
		}
		catch (RSGISRegistrationException &e) 
		{
			throw e;
		}
		
		return distanceMoved;
	}
    
    float RSGISImageRegistration::findTiePointLocation(TiePoint *tiePt, unsigned int windowSize, unsigned int searchArea, RSGISImageSimilarityMetric *metric, unsigned int subPixelResolution, float *moveInX, float *moveInY) throw(RSGISRegistrationException)
	{
		float distanceMoved = 0;
		
		if(!overlapDefined)
		{
			throw RSGISRegistrationException("The overlap needs to be defined before tie location can be defined.");
		}
		
		rsgis::img::RSGISImageUtils imgUtils;
		try
		{
			// Setup overlapping region variables.
			int **dsOffsets = new int*[2];
			dsOffsets[0] = new int[2];
			dsOffsets[1] = new int[2];
			int overlapWidth = 0;
			int overlapHeight = 0;
			double *overlapTransform = new double[6];
			
			double windowXWidth = (((double)windowSize)*overlap->xRes);
			double windowYHeight = (((double)windowSize)*overlap->yRes);
			
			geos::geom::Envelope *env = new geos::geom::Envelope();
			env->init((tiePt->eastings - windowXWidth),
					  (tiePt->eastings + windowXWidth + overlap->xRes),
					  (tiePt->northings - windowYHeight),
					  (tiePt->northings + windowYHeight + +overlap->yRes));
			
			unsigned int numSearchPoints = (searchArea*2)+1;
			float **imageSimilarity = new float*[numSearchPoints];
			for(unsigned int i = 0; i < numSearchPoints; ++i)
			{
				imageSimilarity[i] = new float[numSearchPoints];
			}
			
			int xShiftStart = searchArea * (-1);
			int yShiftStart = searchArea * (-1);
			int xShiftEnd = searchArea;
			int yShiftEnd = searchArea;
			
			float **refDataBlock = NULL;
			float **floatDataBlock = NULL;
			unsigned int numRefDataVals = 0;
			unsigned int numFloatDataVals = 0;
			
			bool first = true;
			double currentMetricVal = 0;
			double metricVal = 0;
			int currentShiftX = 0;
			int currentShiftY = 0;
			
			unsigned int xIdx = 0;
			unsigned int yIdx = 0;
			unsigned int currentXIdx = 0;
			unsigned int currentYIdx = 0;
            
            // Set up variables to hold remainder from image overlap
            float remainderX = 0;
            float remainderY = 0;
            // Remainder for heighest metric
            float currentRemainderX = 0;
            float currentRemainderY = 0;
			
			for(int yShift = yShiftStart; yShift <= yShiftEnd; ++yShift)
			{
				xIdx = 0;
				for(int xShift = xShiftStart; xShift <= xShiftEnd; ++xShift)
				{
					try
					{
						this->getImageOverlapWithFloatShift((((float)xShift)+tiePt->xShift), (((float)yShift)+tiePt->yShift), dsOffsets, &overlapWidth, &overlapHeight, overlapTransform, env, &remainderX, &remainderY);
						
						if((overlapWidth > 0) & (overlapHeight > 0))
						{
							numRefDataVals = 0;
							numFloatDataVals = 0;
							
							refDataBlock = imgUtils.getImageDataBlock(referenceIMG, dsOffsets[0], overlapWidth, overlapHeight, &numRefDataVals);
							floatDataBlock = imgUtils.getImageDataBlock(floatingIMG, dsOffsets[1], overlapWidth, overlapHeight, &numFloatDataVals);
							
							if(numRefDataVals != numFloatDataVals)
							{
								throw RSGISRegistrationException("The number of data values read from the images does not match.");
							}
							
							metricVal = metric->calcValue(refDataBlock, floatDataBlock, numRefDataVals, overlap->numRefBands);
							
							imageSimilarity[yIdx][xIdx] = metricVal;
							
							if(!((boost::math::isnan)(metricVal)))
							{
								if(first)
								{
									currentMetricVal = metricVal;
									currentShiftX = xShift;
									currentShiftY = yShift;
									currentXIdx = xIdx;
									currentYIdx = yIdx;
                                    currentRemainderX = remainderX;
                                    currentRemainderY = remainderY;
									first = false;
								}
								else if(metric->findMin() & (metricVal < currentMetricVal))
								{
									currentMetricVal = metricVal;
									currentShiftX = xShift;
									currentShiftY = yShift;
									currentXIdx = xIdx;
									currentYIdx = yIdx;
                                    currentRemainderX = remainderX;
                                    currentRemainderY = remainderY;
								}
								else if(!metric->findMin() & (metricVal > currentMetricVal))
								{
									currentMetricVal = metricVal;
									currentShiftX = xShift;
									currentShiftY = yShift;
									currentXIdx = xIdx;
									currentYIdx = yIdx;
                                    currentRemainderX = remainderX;
                                    currentRemainderY = remainderY;
								}
							}
							
							for(unsigned int i = 0; i < overlap->numRefBands; ++i)
							{
								delete[] refDataBlock[i];
								delete[] floatDataBlock[i];
							}
							delete[] refDataBlock;
							delete[] floatDataBlock;
						}
					}
					catch (RSGISRegistrationException &e)
					{
						// ignore
						std::cerr << "Tie Point = [" << tiePt->xRef << "," << tiePt->yRef << "]\n";
						std::cerr << "Shift = [" << (xShift+tiePt->xShift) << "," << (yShift+tiePt->yShift) << "]\n";
						std::cerr << "WARNING: " << e.what() << std::endl;
					}
					xIdx++;
				}
				++yIdx;
			}
			
			float subPixelXShift = 0;
			float subPixelYShift = 0;
			float subPixelXMetric = currentMetricVal;
			float subPixelYMetric = currentMetricVal;
			
			rsgis::math::RSGISPolyFit polyFit;
            
			// Find subpixel component
			if(searchArea == 1)
			{
				// 2nd Order Poly
				// Find subpixel X
				if((currentXIdx != 0) & (currentXIdx != (numSearchPoints-1)))
				{
					gsl_matrix *inputDataMatrix = gsl_matrix_alloc(3,2);
					gsl_matrix_set (inputDataMatrix, 0, 0, -1);
					gsl_matrix_set (inputDataMatrix, 0, 1, imageSimilarity[currentYIdx][currentXIdx-1]);
					gsl_matrix_set (inputDataMatrix, 1, 0, 0);
					gsl_matrix_set (inputDataMatrix, 1, 1, imageSimilarity[currentYIdx][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 2, 0, 1);
					gsl_matrix_set (inputDataMatrix, 2, 1, imageSimilarity[currentYIdx][currentXIdx+1]);
					
					unsigned int order = 3; // 2nd Order - starts at zero.
					gsl_vector *coefficients = polyFit.PolyfitOneDimensionQuiet(order, inputDataMatrix);
					
					subPixelXShift = findExtreme(metric->findMin(), coefficients, order, -1, 1, subPixelResolution, &subPixelXMetric);
					
					gsl_matrix_free(inputDataMatrix);
				}
                
				// Find subpixel Y
				if((currentYIdx != 0) & (currentYIdx != (numSearchPoints-1)))
				{
					gsl_matrix *inputDataMatrix = gsl_matrix_alloc(3,2);
					gsl_matrix_set (inputDataMatrix, 0, 0, -1);
					gsl_matrix_set (inputDataMatrix, 0, 1, imageSimilarity[currentYIdx-1][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 1, 0, 0);
					gsl_matrix_set (inputDataMatrix, 1, 1, imageSimilarity[currentYIdx][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 2, 0, 1);
					gsl_matrix_set (inputDataMatrix, 2, 1, imageSimilarity[currentYIdx+1][currentXIdx]);
					
					unsigned int order = 3; // 2nd Order - starts at zero.
					gsl_vector *coefficients = polyFit.PolyfitOneDimensionQuiet(order, inputDataMatrix);
					
					subPixelYShift = findExtreme(metric->findMin(), coefficients, order, -1, 1, subPixelResolution, &subPixelYMetric);
					
					gsl_matrix_free(inputDataMatrix);
				}
			}
			else
			{
				// 4th Order Poly
				if((currentXIdx > 1) & (currentXIdx < (numSearchPoints-2)))
				{
					gsl_matrix *inputDataMatrix = gsl_matrix_alloc(5,2);
					gsl_matrix_set (inputDataMatrix, 0, 0, -2);
					gsl_matrix_set (inputDataMatrix, 0, 1, imageSimilarity[currentYIdx][currentXIdx-2]);
					gsl_matrix_set (inputDataMatrix, 1, 0, -1);
					gsl_matrix_set (inputDataMatrix, 1, 1, imageSimilarity[currentYIdx][currentXIdx-1]);
					gsl_matrix_set (inputDataMatrix, 2, 0, 0);
					gsl_matrix_set (inputDataMatrix, 2, 1, imageSimilarity[currentYIdx][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 3, 0, 1);
					gsl_matrix_set (inputDataMatrix, 3, 1, imageSimilarity[currentYIdx][currentXIdx+1]);
					gsl_matrix_set (inputDataMatrix, 4, 0, 2);
					gsl_matrix_set (inputDataMatrix, 4, 1, imageSimilarity[currentYIdx][currentXIdx+2]);
					
					unsigned int order = 4; // 3th Order - starts at zero.
					gsl_vector *coefficients = polyFit.PolyfitOneDimensionQuiet(order, inputDataMatrix);
					
					subPixelXShift = findExtreme(metric->findMin(), coefficients, order, -1, 1, subPixelResolution, &subPixelXMetric);
					
					gsl_matrix_free(inputDataMatrix);
					gsl_vector_free(coefficients);
				}
				
				if((currentYIdx > 1) & (currentYIdx < (numSearchPoints-2)))
				{
					gsl_matrix *inputDataMatrix = gsl_matrix_alloc(5,2);
					gsl_matrix_set (inputDataMatrix, 0, 0, -2);
					gsl_matrix_set (inputDataMatrix, 0, 1, imageSimilarity[currentYIdx-2][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 1, 0, -1);
					gsl_matrix_set (inputDataMatrix, 1, 1, imageSimilarity[currentYIdx-1][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 2, 0, 0);
					gsl_matrix_set (inputDataMatrix, 2, 1, imageSimilarity[currentYIdx][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 3, 0, 1);
					gsl_matrix_set (inputDataMatrix, 3, 1, imageSimilarity[currentYIdx+1][currentXIdx]);
					gsl_matrix_set (inputDataMatrix, 4, 0, 2);
					gsl_matrix_set (inputDataMatrix, 4, 1, imageSimilarity[currentYIdx+2][currentXIdx]);
					
					unsigned int order = 4; // 3th Order - starts at zero.
					gsl_vector *coefficients = polyFit.PolyfitOneDimensionQuiet(order, inputDataMatrix);
					
					subPixelYShift = findExtreme(metric->findMin(), coefficients, order, -1, 1, subPixelResolution, &subPixelYMetric);
					
					gsl_matrix_free(inputDataMatrix);
					gsl_vector_free(coefficients);
				}
				
			}
			
			currentMetricVal = (subPixelXMetric + subPixelYMetric)/2;
			
            // Calculate final shift, adding on offsets due to rounding in image overlap calculation
            float finalXShift = (((float)currentShiftX) + subPixelXShift) + currentRemainderX;
			float finalYShift = (((float)currentShiftY) + subPixelYShift) + currentRemainderY;
			
			distanceMoved = sqrt(((finalXShift*finalXShift)+(finalYShift*finalYShift))/2);
			*moveInX = finalXShift;
			*moveInY = finalYShift;
			
			tiePt->xShift += finalXShift;
            tiePt->yShift += finalYShift;
            tiePt->metricVal = currentMetricVal;
			
			delete[] overlapTransform;
			delete[] dsOffsets[0];
			delete[] dsOffsets[1];
			delete[] dsOffsets;
		}
		catch (rsgis::img::RSGISImageBandException &e)
		{
			throw RSGISRegistrationException(e.what());
		}
		catch (RSGISRegistrationException &e) 
		{
			throw e;
		}
		
		return distanceMoved;
	}
	
	float RSGISImageRegistration::findExtreme(bool findMin, gsl_vector *coefficients, unsigned int order, float minRange, float maxRange, unsigned int resolution, float *extremeVal)
	{
		double division = ((float)1)/((float)resolution);
				
		float range = maxRange - minRange;
		unsigned int numTests = ceil(range/division);
				
		double xValue = 0;
		double yPredicted = 0;
		
		bool first = true;
		double extremeX = 0;
		double extremeY = 0;

        // Quadratic
        if(order == 3)
        {
            // Calculate value for which first order derivative equals 0.
            extremeX = (-1.0*gsl_vector_get(coefficients,1)) / (2.0*gsl_vector_get(coefficients,2));
            
            // Predict coresponding y value.
            double yPredicted = 0;
			for(unsigned int j = 0; j < order ; j++)
			{
				double xPow = pow(extremeX, static_cast<int>(j)); // x^n;
				double coeff = gsl_vector_get(coefficients, j); // a_n
				double coeffXPow = coeff * xPow; // a_n * x^n
				yPredicted = yPredicted + coeffXPow;
			}
            extremeY=yPredicted;
			
            
        }
        else if(order == 4)
        {
            // Find the values for which the first order derivative is equal to 0.
            double a = gsl_vector_get(coefficients,1);
            double b = 2.0*gsl_vector_get(coefficients,2);
            double c = 3.0*gsl_vector_get(coefficients,3);
            double xValue1 = ((-1.0*b)+sqrt(b*b - 4*a*c))/(2.0*a);
            double xValue2 = ((-1.0*b)-sqrt(b*b - 4*a*c))/(2.0*a);
            
            // Predict coresponding y value.
            double yPredicted1 = 0;
			for(unsigned int j = 0; j < order ; j++)
			{
				double xPow = pow(xValue1, static_cast<int>(j)); // x^n;
				double coeff = gsl_vector_get(coefficients, j); // a_n
				double coeffXPow = coeff * xPow; // a_n * x^n
				yPredicted1 = yPredicted1 + coeffXPow;
			}
            
            double yPredicted2 = 0;
			for(unsigned int j = 0; j < order ; j++)
			{
				double xPow = pow(xValue2, static_cast<int>(j)); // x^n;
				double coeff = gsl_vector_get(coefficients, j); // a_n
				double coeffXPow = coeff * xPow; // a_n * x^n
				yPredicted2 = yPredicted2 + coeffXPow;
			}
            if(yPredicted1 > yPredicted2)
            {
                extremeX = xValue1;
                extremeY = yPredicted1;
            }
            else
            {
                extremeX = xValue2;
                extremeY = yPredicted2;
            }
            
            // Check if outside range (something went wrong)
            if((extremeX < minRange) | (extremeX > maxRange))
            {
                for(unsigned int i = 0; i < numTests; ++i)
                {
                    xValue = minRange + (i*division);
                    yPredicted = 0;
                    for(unsigned int j = 0; j < order ; j++)
                    {
                        double xPow = pow(xValue, static_cast<int>(j)); // x^n;
                        double coeff = gsl_vector_get(coefficients, j); // a_n
                        double coeffXPow = coeff * xPow; // a_n * x^n
                        yPredicted = yPredicted + coeffXPow;
                    }
                    
                    if(first)
                    {
                        extremeX = xValue;
                        extremeY = yPredicted;
                        first = false;
                    }
                    else if(findMin & (yPredicted < extremeY))
                    {
                        extremeX = xValue;
                        extremeY = yPredicted;
                    }
                    else if(!findMin & (yPredicted > extremeY))
                    {
                        extremeX = xValue;
                        extremeY = yPredicted;
                    }
                }
            }
        }
        else
        {
            for(unsigned int i = 0; i < numTests; ++i)
            {
			xValue = minRange + (i*division);
			yPredicted = 0;
			for(unsigned int j = 0; j < order ; j++)
			{
				double xPow = pow(xValue, static_cast<int>(j)); // x^n;
				double coeff = gsl_vector_get(coefficients, j); // a_n
				double coeffXPow = coeff * xPow; // a_n * x^n
				yPredicted = yPredicted + coeffXPow;
			}
			
			if(first)
			{
				extremeX = xValue;
				extremeY = yPredicted;
				first = false;
			}
			else if(findMin & (yPredicted < extremeY))
			{
				extremeX = xValue;
				extremeY = yPredicted;
			}
			else if(!findMin & (yPredicted > extremeY))
			{
				extremeX = xValue;
				extremeY = yPredicted;
			}
		}
        }
		*extremeVal = extremeY;
		return extremeX;
	}
	
    void RSGISImageRegistration::getImageOverlapFloat(GDALDataset **datasets, int numDS,  float **dsOffsets, int *width, int *height, double *gdalTransform) throw(RSGISRegistrationException)
	{
        /**
         Get the overlap of two images. Like getImageOverlap in image utils, however the offsets are retured as floats.
         */
        rsgis::img::RSGISImageUtils imageUtils;
        
        double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		bool first = true;
		const char *proj = NULL;
		
		try
		{
			for(int i = 0; i < numDS; i++)
			{
                if(transformations[i] == NULL)
                {
                    throw RSGISRegistrationException("No projection transformation has been provided..");
                }
                
				if(first)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
                    
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if((imageUtils.closeResTest(pixelXRes, transformations[i][1]) == false) | (imageUtils.closeResTest(pixelYRes, transformations[i][5]) == false))
					{
						throw RSGISRegistrationException("Not all image bands have the same resolution..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISRegistrationException("Not all image bands have the same rotation..");
					}
					
					if(std::string(datasets[i]->GetProjectionRef()) != std::string(proj))
					{
						std::cout << "Not all image bands have the same projection" << std::endl;
					}
                    
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
				}
			}
            
			if(maxX - minX <= 0)
			{
				throw RSGISRegistrationException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				throw RSGISRegistrationException("Images do not overlap in the Y axis");
			}
			
			gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
            
			*width = floor(((maxX - minX)/pixelXRes)+0.5);
			*height = floor(((maxY - minY)/pixelYResPos)+0.5);
			
			double diffX = 0;
			double diffY = 0;
			
			for(int i = 0; i < numDS; i++)
			{
				diffX = minX - transformations[i][0];
				diffY = transformations[i][3] - maxY;
				
				if(!((diffX > -0.0001) & (diffX < 0.0001)))
				{
					dsOffsets[i][0] = diffX/pixelXRes;
				}
				else
				{
					dsOffsets[i][0] = 0;
				}
				
				if(!((diffY > -0.0001) & (diffY < 0.0001)))
				{
					dsOffsets[i][1] = diffY/pixelYResPos;
				}
				else
				{
					dsOffsets[i][1] = 0;
				}
			}
			
		}
		catch(RSGISRegistrationException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
    
    void RSGISImageRegistration::getImageOverlapWithFloatShift(float xShift, float yShift, int **dsOffsets, int *width, int *height, double *gdalTransform, geos::geom::Envelope *env, float *remainderX, float *remainderY) throw(RSGISRegistrationException)
	{
		if(!overlapDefined)
		{
			throw RSGISRegistrationException("The overlap needs to be defined.");
		}
		double *refTransform = new double[6];
		double *floatTransform = new double[6];
		try 
		{
			// Find transformations
			referenceIMG->GetGeoTransform(refTransform);
			int refSizeX = referenceIMG->GetRasterXSize();
			int refSizeY = referenceIMG->GetRasterYSize();
			
			floatingIMG->GetGeoTransform(floatTransform);
			int floatSizeX = floatingIMG->GetRasterXSize();
			int floatSizeY = floatingIMG->GetRasterYSize();
			
			// Apply Shift
			floatTransform[0] += (((float)xShift)*overlap->xRes);
			floatTransform[3] -= (((float)yShift)*overlap->yRes);
			
			// Define spatial boundary of each image
			double refTLX = refTransform[0];
			double refTLY = refTransform[3];
			double refBRX = refTransform[0] + (refSizeX * overlap->xRes);
			double refBRY = refTransform[3] - (refSizeY * overlap->xRes);
			
			double floatTLX = floatTransform[0];
			double floatTLY = floatTransform[3];
			double floatBRX = floatTransform[0] + (floatSizeX * overlap->xRes);
			double floatBRY = floatTransform[3] - (floatSizeY * overlap->xRes);
			
			// Define the overlapping region
			double tlX = 0;
			double tlY = 0;
			double brX = 0;
			double brY = 0;
			
			if(refTLX > floatTLX)
			{
				tlX = refTLX;
			}
			else
			{
				tlX = floatTLX;
			}
			
			if(refTLY < floatTLY)
			{
				tlY = refTLY;
			}
			else 
			{
				tlY = floatTLY;
			}

			if(refBRX < floatBRX)
			{
				brX = refBRX;
			}
			else
			{
				brX = floatBRX;
			}
			
			if(refBRY > floatBRY)
			{
				brY = refBRY;
			}
			else
			{
				brY = floatBRY;
			}
			
			// Check the images (with shift) overlap
			if((brX - tlX) <= 0)
			{
				throw RSGISRegistrationException("Images do not overlap in the X axis.");
			}
			
			if((tlY - brY) <= 0)
			{
				throw RSGISRegistrationException("Images do not overlap in the Y axis.");
			}
			
			// Check whether the overlapping region intersects within the Envelop
			geos::geom::Envelope overlapEnv = geos::geom::Envelope(tlX, brX, brY, tlY);
			if(!env->intersects(&overlapEnv))
			{
				throw RSGISRegistrationException("The overlapping region of the images does not intersect with the envelop provided");
			}
			
			// Trim to region overlapping with the envelop
			if(tlX < env->getMinX())
			{
				tlX = env->getMinX();
			}
			
			if(tlY > env->getMaxY())
			{
				tlY = env->getMaxY();
			}
			
			if(brX > env->getMaxX())
			{
				brX = env->getMaxX();
			}
			
			if(brY < env->getMinY())
			{
				brY = env->getMinY();
			}

			
			// Define output values.
			gdalTransform[0] = tlX;
			gdalTransform[1] = overlap->xRes;
			gdalTransform[2] = overlap->xRot;
			gdalTransform[3] = tlY;
			gdalTransform[4] = overlap->yRot;
			gdalTransform[5] = overlap->yRes;
			
			*width = floor(((brX - tlX)/overlap->xRes)+0.5);
			*height = floor(((tlY - brY)/overlap->yRes)+0.5);
            
			double diffX = 0;
			double diffY = 0;
			
			// Define reference offsets
			diffX = tlX - refTransform[0];
			diffY = refTransform[3] - tlY;
            
            if(!((diffX > -0.0001) & (diffX < 0.0001)))
            {
                dsOffsets[0][0] = floor((diffX/overlap->xRes));
            }
            else
            {
                dsOffsets[0][0] = 0;
            }
            
            if(!((diffY > -0.0001) & (diffY < 0.0001)))
            {
                dsOffsets[0][1] = floor((diffY/overlap->yRes));
            }
            else
            {
                dsOffsets[0][1] = 0;
            }
			
			// Define floating offsets
			diffX = tlX - floatTransform[0];
			diffY = floatTransform[3] - tlY;
            if(!((diffX > -0.0001) & (diffX < 0.0001)))
            {
                dsOffsets[1][0] = floor((diffX/overlap->xRes));
                *remainderX = (diffX/overlap->xRes) - dsOffsets[1][0];
            }
            else
            {
                dsOffsets[1][0] = 0;
                *remainderX = 0;
            }
            
            if(!((diffY > -0.0001) & (diffY < 0.0001)))
            {
                dsOffsets[1][1] = floor((diffY/overlap->yRes));
                *remainderY = (diffY/overlap->yRes) - dsOffsets[1][1];
            }
            else
            {
                dsOffsets[1][1] = 0;
                *remainderY = 0;
            }
		}
		catch(RSGISRegistrationException &e)
		{
			delete[] refTransform;
			delete[] floatTransform;
			throw e;
		}
		delete[] refTransform;
		delete[] floatTransform;

	}
	
	void RSGISImageRegistration::removeTiePointsWithLowStdDev(std::list<TiePoint*> *tiePts, unsigned int windowSize, float stdDevRefThreshold, float stdDevFloatThreshold)
	{
		rsgis::img::RSGISImageUtils imgUtils;
		
		int **dsOffsets = new int*[2];
		dsOffsets[0] = new int[2];
		dsOffsets[1] = new int[2];
		int overlapWidth = 0;
		int overlapHeight = 0;
		double *overlapTransform = new double[6];
		
		double windowXWidth = (((double)windowSize)*overlap->xRes);
		double windowYHeight = (((double)windowSize)*overlap->yRes);
		
		geos::geom::Envelope *env = new geos::geom::Envelope();
		
		float **refDataBlock = NULL;
		float **floatDataBlock = NULL;
		unsigned int numRefDataVals = 0;
		unsigned int numFloatDataVals = 0;
		
		double floatStdDev = 0;
		double refStdDev = 0;
        
        // Set up variables to hold remainder from overlap (won't use them here)
        float remainderX = 0;
        float remainderY = 0;
		
		try 
		{
			std::list<TiePoint*>::iterator iterTiePts;
			for(iterTiePts = tiePts->begin(); iterTiePts != tiePts->end(); )
			{
				env->init(((*iterTiePts)->eastings - windowXWidth), 
						  ((*iterTiePts)->eastings + windowXWidth + overlap->xRes), 
						  ((*iterTiePts)->northings - windowYHeight),
						  ((*iterTiePts)->northings + windowYHeight + +overlap->yRes));
				
				this->getImageOverlapWithFloatShift(0, 0, dsOffsets, &overlapWidth, &overlapHeight, overlapTransform, env, &remainderX, &remainderY);
				
				refDataBlock = imgUtils.getImageDataBlock(referenceIMG, dsOffsets[0], overlapWidth, overlapHeight, &numRefDataVals);
				floatDataBlock = imgUtils.getImageDataBlock(floatingIMG, dsOffsets[1], overlapWidth, overlapHeight, &numFloatDataVals);
				
				if(numRefDataVals != numFloatDataVals)
				{
					throw RSGISRegistrationException("The number of data values read from the images does not match.");
				}
				
				refStdDev = this->calcStdDev(refDataBlock, numRefDataVals, overlap->numRefBands);
				floatStdDev = this->calcStdDev(floatDataBlock, numFloatDataVals, overlap->numRefBands);
								
				if((refStdDev < stdDevRefThreshold) | (floatStdDev < stdDevFloatThreshold))
				{
					delete *iterTiePts;
					tiePts->erase(iterTiePts++);
				}
				else
				{
					++iterTiePts;
				}
				
				for(unsigned int i = 0; i < overlap->numRefBands; ++i)
				{
					delete[] refDataBlock[i];
					delete[] floatDataBlock[i];
				}
				delete[] refDataBlock;
				delete[] floatDataBlock;
			}
		}
		catch (rsgis::img::RSGISImageBandException &e) 
		{
			throw RSGISRegistrationException(e.what());
		}
		catch (RSGISRegistrationException &e) 
		{
			throw e;
		}
		
		delete[] overlapTransform;
		delete[] dsOffsets[0];
		delete[] dsOffsets[1];
		delete[] dsOffsets;
	}
	
	double RSGISImageRegistration::calcStdDev(float **data, unsigned int numVals, unsigned int numDims)
	{
		/** Calculate standard deviation for an image window considers only data with values > 0 and non NaN.
		 *  Returns standard deviation or -1 if less than half the pixels have no data.
		 */
		unsigned int totalNumVals = numDims * numVals;
		unsigned int dataVals = 0;
		double stdDev = 0;
		
		double sum = 0;
		
		for(unsigned int i = 0; i < numDims; ++i)
		{
			for(unsigned int j = 0; j < numVals; ++j)
			{
				if((!((boost::math::isnan)(data[i][j]))) && (data[i][j] != 0))
				{
					sum += data[i][j];
					dataVals+=1;
				}
			}
		}
		
		// If less than half the values have no data, return -1,
		// probably at an image edge so best to avoid.
		if(((float) dataVals / (float) totalNumVals) < 0.5)
		{
			stdDev = -1;
		}
		else
		{
			double mean = sum/dataVals;

			double sqDiff = 0;

			for(unsigned int i = 0; i < numDims; ++i)
			{
				for(unsigned int j = 0; j < numVals; ++j)
				{
					if((!((boost::math::isnan)(data[i][j]))) && (data[i][j] != 0))
					{
						sqDiff += ((data[i][j] - mean) * (data[i][j] - mean));
					}
				}
			}

			stdDev = sqrt(sqDiff/dataVals);
		}
		return stdDev;
	}
	
	void RSGISImageRegistration::exportTiePointsENVIImage2MapImpl(std::string filepath, std::list<TiePoint*> *tiePts) throw(RSGISRegistrationException)
	{
        std::ofstream outPtsFile(filepath.c_str(), std::ios::out | std::ios::trunc);
		
		if(!outPtsFile.is_open())
		{
			std::string message = std::string("Could not open tie points file: ") + filepath;
			throw RSGISRegistrationException(message);
		}
		
		outPtsFile.precision(12);
		outPtsFile << std::fixed;
		
		outPtsFile << "; ENVI Image to Map GCP File\n";
		outPtsFile << "; projection info = ... \n";
		outPtsFile << "; warp file: ... \n";
		outPtsFile << "; Map (x,y), Image (x,y)\n";
		outPtsFile << ";\n";
		
		std::cout << tiePts->size() << " tie points to be exported\n";
		
		std::list<TiePoint*>::iterator iterTiePts;
		for(iterTiePts = tiePts->begin(); iterTiePts != tiePts->end(); ++iterTiePts)
		{
			outPtsFile << "\t" << (*iterTiePts)->eastings << "\t" << (*iterTiePts)->northings << "\t" << (*iterTiePts)->xFloat << "\t" << (*iterTiePts)->yFloat << std::endl;
		}
		
		std::cout << "Export Complete\n";
		
		outPtsFile.flush();
		outPtsFile.close();
	}
	
	void RSGISImageRegistration::exportTiePointsENVIImage2ImageImpl(std::string filepath, std::list<TiePoint*> *tiePts) throw(RSGISRegistrationException)
	{
        std::ofstream outPtsFile(filepath.c_str(), std::ios::out | std::ios::trunc);
		
		if(!outPtsFile.is_open())
		{
			std::string message = std::string("Could not open tie points file: ") + filepath;
			throw RSGISRegistrationException(message);
		}
		
		outPtsFile.precision(12);
		outPtsFile << std::fixed;
		
		outPtsFile << "; ENVI Image to Image GCP File\n";
		outPtsFile << "; base file: ... \n";
		outPtsFile << "; warp file: ... \n";
		outPtsFile << "; Base Image (x,y), Warp Image (x,y)\n";
		outPtsFile << ";\n";
		
		std::cout << tiePts->size() << " tie points to be exported\n";
		
		std::list<TiePoint*>::iterator iterTiePts;
		for(iterTiePts = tiePts->begin(); iterTiePts != tiePts->end(); ++iterTiePts)
		{
			outPtsFile << "\t" << (*iterTiePts)->xRef << "\t" << (*iterTiePts)->yRef << "\t" << (*iterTiePts)->xFloat << "\t" << (*iterTiePts)->yFloat << std::endl;
		}
		
		std::cout << "Export Complete\n";
		
		outPtsFile.flush();
		outPtsFile.close();
	}
	
	void RSGISImageRegistration::exportTiePointsRSGISImage2MapImpl(std::string filepath, std::list<TiePoint*> *tiePts) throw(RSGISRegistrationException)
	{
		std::ofstream outPtsFile(filepath.c_str(), std::ios::out | std::ios::trunc);
		
		if(!outPtsFile.is_open())
		{
			std::string message = std::string("Could not open tie points file: ") + filepath;
			throw RSGISRegistrationException(message);
		}
		
		outPtsFile.precision(12);
		outPtsFile << std::fixed;
		
		outPtsFile << "# RSGISLib Image to Map GCP File\n";
		outPtsFile << "# Reference Map (E,N), Floating Image (x,y), Metric Value\n";
		outPtsFile << "#\n";
		
		std::cout << tiePts->size() << " tie points to be exported\n";
		
		std::list<TiePoint*>::iterator iterTiePts;
		for(iterTiePts = tiePts->begin(); iterTiePts != tiePts->end(); ++iterTiePts)
		{
			outPtsFile << (*iterTiePts)->eastings << "," << (*iterTiePts)->northings << "," << (*iterTiePts)->xFloat << ","
            << (*iterTiePts)->yFloat << "," << (*iterTiePts)->metricVal << std::endl;
		}
		
		outPtsFile << "# End Of File\n";
		
		std::cout << "Export Complete\n";
		
		outPtsFile.flush();
		outPtsFile.close();
	}
    
    void RSGISImageRegistration::exportTiePointsRSGISMapOffsImpl(std::string filepath, std::list<TiePoint*> *tiePts)throw(RSGISRegistrationException)
    {
        std::ofstream outPtsFile(filepath.c_str(), std::ios::out | std::ios::trunc);
		
		if(!outPtsFile.is_open())
		{
			std::string message = std::string("Could not open tie points file: ") + filepath;
			throw RSGISRegistrationException(message);
		}
		
		outPtsFile.precision(12);
		outPtsFile << std::fixed;
		
		outPtsFile << "# RSGISLib Map offsets GCP File\n";
		outPtsFile << "# Eastings, Northings, Offset to correct location for floating image (E,N), Metric Value\n";
		outPtsFile << "#\n";
		
		std::cout << tiePts->size() << " tie points to be exported\n";
		
		std::list<TiePoint*>::iterator iterTiePts;
		for(iterTiePts = tiePts->begin(); iterTiePts != tiePts->end(); ++iterTiePts)
		{
			outPtsFile << (*iterTiePts)->eastings << "," << (*iterTiePts)->northings << "," << (*iterTiePts)->xShift << ","
            << (*iterTiePts)->yShift << "," << (*iterTiePts)->metricVal << std::endl;
		}
		
		outPtsFile << "# End Of File\n";
		
		std::cout << "Export Complete\n";
		
		outPtsFile.flush();
		outPtsFile.close();
    }
	
	RSGISImageRegistration::~RSGISImageRegistration()
	{
		if(overlap != NULL)
		{
			delete overlap;
		}
	}
	

}}


