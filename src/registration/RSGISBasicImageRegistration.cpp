/*
 *  RSGISBasicImageRegistration.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 31/08/2010.
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

#include "RSGISBasicImageRegistration.h"

namespace rsgis{namespace reg{

	RSGISBasicImageRegistration::RSGISBasicImageRegistration(GDALDataset *reference, GDALDataset *floating, unsigned int gap, float metricThreshold, unsigned int windowSize, unsigned int searchArea, RSGISImageSimilarityMetric *metric, float stdDevRefThreshold, float stdDevFloatThreshold, unsigned int subPixelResolution):RSGISImageRegistration(reference, floating), tiePoints(NULL), gap(1), metricThreshold(0), initExecuted(false), windowSize(0), searchArea(0), metric(NULL), stdDevRefThreshold(0), stdDevFloatThreshold(0), subPixelResolution(0)
	{
		tiePoints = new std::list<TiePoint*>();
		this->gap = gap;
		this->metricThreshold = metricThreshold;
		this->windowSize = windowSize;
		this->searchArea = searchArea;
		this->metric = metric;
		this->stdDevRefThreshold = stdDevRefThreshold;
		this->stdDevFloatThreshold = stdDevFloatThreshold;
		this->subPixelResolution = subPixelResolution;
	}
		
	void RSGISBasicImageRegistration::initRegistration() throw(RSGISRegistrationException)
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
        
        std::cout << numXPts << " columns of tie points will be generated\n";
        std::cout << numYPts << " rows of tie points will be generated\n";
        std::cout << "Therefore, a total of " << (numXPts * numYPts) << " tie points will be generated\n";
		
		unsigned int startXOff = 0;
		unsigned int startYOff = 0;
		
		this->defineFirstTiePoint(&startXOff, &startYOff, numXPts, numYPts, gap);
		
		// Generate initial tie points.
		double startEastings = (overlap->tlX - overlap->xRes) + (((double)startXOff)*overlap->xRes);
		double startNorthings = (overlap->tlY + overlap->yRes) - (((double)startYOff)*overlap->yRes);
		double currentEastings = startEastings;
		double currentNorthings = startNorthings;
		
		TiePoint *tmpTiePt = NULL;
				
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
				tmpTiePt->xShift = 0;
				tmpTiePt->yShift = 0;
				tmpTiePt->metricVal = std::numeric_limits<double>::signaling_NaN();//NAN;
				
				// Add tie points to list
				tiePoints->push_back(tmpTiePt);
				
				currentEastings += (overlap->xRes*((double)gap));
			}
			currentNorthings -= (overlap->yRes*((double)gap));
		}
		
		this->removeTiePointsWithLowStdDev(tiePoints, windowSize, stdDevRefThreshold, stdDevFloatThreshold);
        
        std::cout << tiePoints->size() << " are remaining following removal of tie points with low standard deviation of image regions\n";
		
		initExecuted = true;
	}
	
	void RSGISBasicImageRegistration::executeRegistration() throw(RSGISRegistrationException)
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
		
		std::cout << "Started ." << std::flush;
		
		float xShift = 0;
		float yShift = 0;
		
		std::list<TiePoint*>::iterator iterTiePts;
		for(iterTiePts = tiePoints->begin(); iterTiePts != tiePoints->end(); ++iterTiePts)
		{
			if(giveFeedback && ((counter % feedback) == 0))
			{
				std::cout << "." << feedbackVal << "." << std::flush;
				feedbackVal += 10;
			}
			
			this->findTiePointLocation(*iterTiePts, windowSize, searchArea, metric, metricThreshold, subPixelResolution, &xShift, &yShift);            
			++counter;
		}
		std::cout << ". Complete\n";
	}
	
	void RSGISBasicImageRegistration::finaliseRegistration() throw(RSGISRegistrationException)
	{
		if(!initExecuted)
		{
			throw RSGISRegistrationException("The algorithm needs to be initialised and executed before being finalised.");
		}
		
		unsigned int floatImgXSize = floatingIMG->GetRasterXSize();
		unsigned int floatImgYSize = floatingIMG->GetRasterYSize();
		
		std::list<TiePoint*>::iterator iterTiePts;
		for(iterTiePts = tiePoints->begin(); iterTiePts != tiePoints->end(); )
		{
			(*iterTiePts)->xFloat -= (*iterTiePts)->xShift;
			(*iterTiePts)->yFloat -= (*iterTiePts)->yShift;
			
			if((boost::math::isnan)((*iterTiePts)->metricVal))
			{
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else if(metric->findMin() & ((*iterTiePts)->metricVal > metricThreshold))
			{
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else if(!metric->findMin() & ((*iterTiePts)->metricVal < metricThreshold))
			{
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else if((*iterTiePts)->xFloat < 0)
			{
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else if((*iterTiePts)->xFloat > floatImgXSize)
			{
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else if((*iterTiePts)->yFloat < 0)
			{
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else if((*iterTiePts)->yFloat > floatImgYSize)
			{
				delete *iterTiePts;
				tiePoints->erase(iterTiePts++);
			}
			else
			{
				++iterTiePts;
			}
		}
	}
	
	void RSGISBasicImageRegistration::exportTiePointsENVIImage2Map(std::string filepath)throw(RSGISRegistrationException)
	{
		this->exportTiePointsENVIImage2MapImpl(filepath, tiePoints);
	}
	
	void RSGISBasicImageRegistration::exportTiePointsENVIImage2Image(std::string filepath)throw(RSGISRegistrationException)
	{
		this->exportTiePointsENVIImage2ImageImpl(filepath, tiePoints);
	}
	
	void RSGISBasicImageRegistration::exportTiePointsRSGISImage2Map(std::string filepath)throw(RSGISRegistrationException)
	{
		this->exportTiePointsRSGISImage2MapImpl(filepath, tiePoints);
	}
    
    void RSGISBasicImageRegistration::exportTiePointsRSGISMapOffs(std::string filepath)throw(RSGISRegistrationException)
    {
        this->exportTiePointsRSGISMapOffsImpl(filepath, tiePoints);
    }
	
	RSGISBasicImageRegistration::~RSGISBasicImageRegistration()
	{
		std::list<TiePoint*>::iterator iterTiePts;
		for(iterTiePts = tiePoints->begin(); iterTiePts != tiePoints->end(); )
		{
			delete *iterTiePts;
			tiePoints->erase(iterTiePts++);
		}
	}
}}



