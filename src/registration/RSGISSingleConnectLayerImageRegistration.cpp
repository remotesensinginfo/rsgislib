/*
 *  RSGISSingleConnectLayerImageRegistration.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/09/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#include "RSGISSingleConnectLayerImageRegistration.h"


namespace rsgis{namespace reg{
	
	RSGISSingleConnectLayerImageRegistration::RSGISSingleConnectLayerImageRegistration(GDALDataset *reference, GDALDataset *floating, unsigned int gap, float metricThreshold, unsigned int windowSize, unsigned int searchArea, RSGISImageSimilarityMetric *metric, float stdDevRefThreshold, float stdDevFloatThreshold, unsigned int subPixelResolution, float distanceThreshold, unsigned int maxNumIterations, float moveChangeThreshold, float pSmoothness):RSGISImageRegistration(reference, floating), tiePoints(NULL), gap(1), metricThreshold(0), initExecuted(false), windowSize(0), searchArea(0), metric(NULL), stdDevRefThreshold(0), stdDevFloatThreshold(0), subPixelResolution(0), distanceThreshold(0), maxNumIterations(10), moveChangeThreshold(0), pSmoothness(0)
	{
		tiePoints = new list<TiePointInSingleLayer*>();
		this->gap = gap;
		this->metricThreshold = metricThreshold;
		this->windowSize = windowSize;
		this->searchArea = searchArea;
		this->metric = metric;
		this->stdDevRefThreshold = stdDevRefThreshold;
		this->stdDevFloatThreshold = stdDevFloatThreshold;
		this->subPixelResolution = subPixelResolution;
		this->distanceThreshold = distanceThreshold;
		this->maxNumIterations = maxNumIterations;
		this->moveChangeThreshold = moveChangeThreshold;
		this->pSmoothness = pSmoothness;
	}
	
	void RSGISSingleConnectLayerImageRegistration::initRegistration() throw(RSGISRegistrationException)
	{
		if(gap < 1)
		{
			throw RSGISRegistrationException("Gap needs to be at least 1 (i.e., 1 pixel).");
		}
		
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
		
		unsigned int numXPts = this->overlap->xSize/gap;
		unsigned int numYPts = this->overlap->ySize/gap;
        
        cout << numXPts << " columns of tie points will be generated\n";
        cout << numYPts << " rows of tie points will be generated\n";
        cout << "Therefore, a total of " << (numXPts * numYPts) << " tie points will be generated\n";
		
		unsigned int startXOff = 0;
		unsigned int startYOff = 0;
		
		this->defineFirstTiePoint(&startXOff, &startYOff, numXPts, numYPts, gap);
		
		// Generate initial tie points.
		double startEastings = (overlap->tlX - overlap->xRes) + (((double)startXOff)*overlap->xRes);
		double startNorthings = (overlap->tlY + overlap->yRes) - (((double)startYOff)*overlap->yRes);
		double currentEastings = startEastings;
		double currentNorthings = startNorthings;
		
		TiePoint *tmpTiePt = NULL;
		list<TiePoint*> *tmpTiePts = new list<TiePoint*>();
		
		//cout << "Start offset = [" << startXOff << "," << startYOff << "]" << endl;
		
		for(unsigned int i = 0; i < numYPts; ++i)
		{
			currentEastings = startEastings;
			for(unsigned int j = 0; j < numXPts; ++j)
			{
				// Create Tie Point
				tmpTiePt = new TiePoint;
				tmpTiePt->eastings = currentEastings;
				tmpTiePt->northings = currentNorthings;
				tmpTiePt->xRef = overlap->refXStart + startXOff + (j*gap);
				tmpTiePt->yRef = overlap->refYStart + startYOff + (i*gap);
				tmpTiePt->xFloat = overlap->floatXStart + startXOff + (j*gap);
				tmpTiePt->yFloat = overlap->floatYStart + startYOff + (i*gap);
				tmpTiePt->metricVal = numeric_limits<double>::signaling_NaN();//NAN;
				tmpTiePt->xShift = 0;
				tmpTiePt->yShift = 0;
				
				// Add tie points to list
				tmpTiePts->push_back(tmpTiePt);
				
				currentEastings += (overlap->xRes*((double)gap));
			}
			currentNorthings -= (overlap->yRes*((double)gap));
		}
		
		this->removeTiePointsWithLowStdDev(tmpTiePts, windowSize, stdDevRefThreshold, stdDevFloatThreshold);
        
        cout << tmpTiePts->size() << " are remaining following removal of tie points with low standard deviation of image regions\n";
		
		TiePointInSingleLayer *tmpTiePtInLayer = NULL;
		list<TiePoint*>::iterator iterTiePts;
		list<TiePoint*>::iterator iterNrTiePts;
		for(iterTiePts = tmpTiePts->begin(); iterTiePts != tmpTiePts->end(); iterTiePts++)
		{
			tmpTiePtInLayer = new TiePointInSingleLayer();
			tmpTiePtInLayer->tiePt = *iterTiePts;
			tmpTiePtInLayer->nrTiePts = new list<TiePoint*>();
			
			for(iterNrTiePts = tmpTiePts->begin(); iterNrTiePts != tmpTiePts->end(); iterNrTiePts++)
			{
				if(((*iterNrTiePts) != (*iterTiePts)) && (*iterTiePts)->refDistance(*iterNrTiePts))
				{
					tmpTiePtInLayer->nrTiePts->push_back(*iterNrTiePts);
				}
			}
			
			
			tiePoints->push_back(tmpTiePtInLayer);
			
		}

		delete tmpTiePts;
		
		initExecuted = true;
	}
	
	void RSGISSingleConnectLayerImageRegistration::executeRegistration() throw(RSGISRegistrationException)
	{
		if(!initExecuted)
		{
			throw RSGISRegistrationException("The algorithm needs to be initialised before being executed.");
		}
		
		unsigned int counter = 0;
		unsigned int feedback = 0;
		unsigned int feedbackVal = 0;
		bool giveFeedback = false;
		if(tiePoints->size() > 10)
		{
			feedback = tiePoints->size()/10;
			giveFeedback = true;
		}
		
		
		
		float xShift = 0;
		float yShift = 0;
		double totalMovement = 0;
		double averageMovement = 0;
		
		double distance = 0;
		double invDist = 0;
		float xShiftDiff = 0;
		float yShiftDiff = 0;
		bool first = true;
		float prevAverage = 0;
		
		list<TiePointInSingleLayer*>::iterator iterTiePts;
		list<TiePoint*>::iterator iterNrTiePts;
		
		for(unsigned int i = 0; i < maxNumIterations; ++i)
		{
			cout << "Started (Iteration " << i << ")." << flush;
			totalMovement = 0;
			counter = 0;
			feedbackVal = 0;
			for(iterTiePts = tiePoints->begin(); iterTiePts != tiePoints->end(); ++iterTiePts)
			{
				if(giveFeedback && ((counter % feedback) == 0))
				{
					cout << "." << feedbackVal << "." << flush;
					feedbackVal += 10;
				}
				
				//cout << "Finding location of tie point " << counter << endl;
				totalMovement += this->findTiePointLocation((*iterTiePts)->tiePt, windowSize, searchArea, metric, metricThreshold, subPixelResolution, &xShift, &yShift);

				for(iterNrTiePts = (*iterTiePts)->nrTiePts->begin(); iterNrTiePts != (*iterTiePts)->nrTiePts->end(); ++iterNrTiePts)
				{
					distance = (*iterTiePts)->tiePt->floatDistance((*iterNrTiePts));
					if(distance < 1)
					{
						invDist = 1;
					}
					else
					{
						invDist = 1/(distance*pSmoothness);
					}
					
					xShiftDiff = xShift - (*iterNrTiePts)->xShift;
					yShiftDiff = yShift - (*iterNrTiePts)->yShift;
					
					//cout << "Before = [" << (*iterNrTiePts)->xShift << "," << (*iterNrTiePts)->yShift << "]\n";
					(*iterNrTiePts)->xShift += invDist*xShiftDiff;
					(*iterNrTiePts)->yShift += invDist*yShiftDiff;
					//cout << "After = [" << (*iterNrTiePts)->xShift << "," << (*iterNrTiePts)->yShift << "]\n\n";
				}
				
				++counter;
			}
			averageMovement = totalMovement/tiePoints->size();
			cout << ". Complete - Movement = "<< averageMovement << endl;
			if(first)
			{
				prevAverage = averageMovement;
				first = false;
			}
			else
			{
				float moveDiff = sqrt(((averageMovement - prevAverage)*(averageMovement - prevAverage)));
				if(moveDiff < moveChangeThreshold)
				{
					break;
				}
				prevAverage = averageMovement;
			}
		}
		
		
		
	}
	
	void RSGISSingleConnectLayerImageRegistration::finaliseRegistration() throw(RSGISRegistrationException)
	{
		if(!initExecuted)
		{
			throw RSGISRegistrationException("The algorithm needs to be initialised and executed before being finalised.");
		}
		
		unsigned int floatImgXSize = floatingIMG->GetRasterXSize();
		unsigned int floatImgYSize = floatingIMG->GetRasterYSize();
		
		list<TiePointInSingleLayer*>::iterator iterTiePts;
		for(iterTiePts = tiePoints->begin(); iterTiePts != tiePoints->end(); )
		{
			(*iterTiePts)->tiePt->xFloat -= (*iterTiePts)->tiePt->xShift;
			(*iterTiePts)->tiePt->yFloat -= (*iterTiePts)->tiePt->yShift;
			
			delete (*iterTiePts)->nrTiePts;
			if(((boost::math::isnan)((*iterTiePts)->tiePt->metricVal)))
			{
				delete (*iterTiePts)->tiePt;
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else if(metric->findMin() & ((*iterTiePts)->tiePt->metricVal > metricThreshold))
			{
				delete (*iterTiePts)->tiePt;
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else if(!metric->findMin() & ((*iterTiePts)->tiePt->metricVal < metricThreshold))
			{
				delete (*iterTiePts)->tiePt;
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else if((*iterTiePts)->tiePt->xFloat < 0)
			{
				delete (*iterTiePts)->tiePt;
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else if((*iterTiePts)->tiePt->xFloat > floatImgXSize)
			{
				delete (*iterTiePts)->tiePt;
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else if((*iterTiePts)->tiePt->yFloat < 0)
			{
				delete (*iterTiePts)->tiePt;
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else if((*iterTiePts)->tiePt->yFloat > floatImgYSize)
			{
				delete (*iterTiePts)->tiePt;
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else
			{
				++iterTiePts;
			}
		}
	}
	
	void RSGISSingleConnectLayerImageRegistration::exportTiePointsENVIImage2Map(string filepath)throw(RSGISRegistrationException)
	{
		list<TiePoint*> *tmpTiePts = new list<TiePoint*>();
		list<TiePointInSingleLayer*>::iterator iterTiePts;
		for(iterTiePts = tiePoints->begin(); iterTiePts != tiePoints->end(); ++iterTiePts)
		{
			tmpTiePts->push_back((*iterTiePts)->tiePt);
		}
		this->exportTiePointsENVIImage2MapImpl(filepath, tmpTiePts);
		delete tmpTiePts;
	}
	
	void RSGISSingleConnectLayerImageRegistration::exportTiePointsENVIImage2Image(string filepath)throw(RSGISRegistrationException)
	{
		list<TiePoint*> *tmpTiePts = new list<TiePoint*>();
		list<TiePointInSingleLayer*>::iterator iterTiePts;
		for(iterTiePts = tiePoints->begin(); iterTiePts != tiePoints->end(); ++iterTiePts)
		{
			tmpTiePts->push_back((*iterTiePts)->tiePt);
		}
		this->exportTiePointsENVIImage2ImageImpl(filepath, tmpTiePts);
		delete tmpTiePts;
	}
	
	void RSGISSingleConnectLayerImageRegistration::exportTiePointsRSGISImage2Map(string filepath)throw(RSGISRegistrationException)
	{
		list<TiePoint*> *tmpTiePts = new list<TiePoint*>();
		list<TiePointInSingleLayer*>::iterator iterTiePts;
		for(iterTiePts = tiePoints->begin(); iterTiePts != tiePoints->end(); ++iterTiePts)
		{
			tmpTiePts->push_back((*iterTiePts)->tiePt);
		}
		this->exportTiePointsRSGISImage2MapImpl(filepath, tmpTiePts);
		delete tmpTiePts;
	}
	
	RSGISSingleConnectLayerImageRegistration::~RSGISSingleConnectLayerImageRegistration()
	{
		list<TiePointInSingleLayer*>::iterator iterTiePts;
		for(iterTiePts = tiePoints->begin(); iterTiePts != tiePoints->end(); )
		{
			delete (*iterTiePts)->tiePt;
			delete *iterTiePts;
			tiePoints->erase(iterTiePts++);
		}
	}
}}




