/*
 *  RSGISImageRegistration.h
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

#ifndef RSGISImageRegistration_H
#define RSGISImageRegistration_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include <list>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "common/RSGISRegistrationException.h"

#include "registration/RSGISImageSimilarityMetric.h"

#include "img/RSGISImageBandException.h"
#include "img/RSGISImageUtils.h"

#include "math/RSGISPolyFit.h"

#include "boost/math/special_functions/fpclassify.hpp"

#include "geos/geom/Envelope.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

namespace rsgis{namespace reg{
    
	class DllExport RSGISImageRegistration
	{
	public:
		
		struct DllExport TiePoint
		{
			double eastings;
			double northings;
			unsigned int xRef;
			unsigned int yRef;
			double xFloat;
			double yFloat;
			double metricVal;
			double xShift;
			double yShift;
			
			friend bool operator==(const TiePoint &pt1, const TiePoint &pt2)
			{
				bool returnVal = false;
				if((pt1.xRef == pt2.xRef) & (pt1.yRef == pt2.yRef))
				{
					returnVal = true;
				}
				return returnVal;
			}
			
			bool operator==(const TiePoint *pt) const
			{
				bool returnVal = false;
				if((this->xRef == pt->xRef) & (this->yRef == pt->yRef))
				{
					returnVal = true;
				}
				return returnVal;
			}
			
			float refDistance(TiePoint *pt)
			{
				double sqSum = ((this->xRef - pt->xRef)*(this->xRef - pt->xRef)) + ((this->yRef - pt->yRef)*(this->yRef - pt->yRef));
				
				return sqrt(sqSum/2);
			}
			
			float floatDistance(TiePoint *pt)
			{
				double sqSum = (((this->xFloat-this->xShift) - (pt->xFloat-pt->xShift))*((this->xFloat-this->xShift) - (pt->xFloat-pt->xShift))) + (((this->yFloat-this->yShift) - (pt->yFloat-pt->yShift))*((this->yFloat-this->yShift) - (pt->yFloat-pt->yShift)));
				
				return sqrt(sqSum/2);
			}
		};
		
		struct DllExport OverlapRegion
		{
			double tlX;
			double tlY;
			double brX;
			double brY;
			double xRes;
			double yRes;
			double xRot;
			double yRot;
			float refXStart;
			float refYStart;
			float floatXStart;
			float floatYStart;
			unsigned long xSize;
			unsigned long ySize;
			unsigned int numRefBands;
			unsigned int numFloatBands;
		};
		
		RSGISImageRegistration(GDALDataset *reference, GDALDataset *floating);
		void runCompleteRegistration();
		virtual void initRegistration()throw(RSGISRegistrationException)=0;
		virtual void executeRegistration()throw(RSGISRegistrationException)=0;
		virtual void finaliseRegistration()throw(RSGISRegistrationException)=0;
		virtual void exportTiePointsENVIImage2Map(std::string filepath)throw(RSGISRegistrationException)=0;
		virtual void exportTiePointsENVIImage2Image(std::string filepath)throw(RSGISRegistrationException)=0;
		virtual void exportTiePointsRSGISImage2Map(std::string filepath)throw(RSGISRegistrationException)=0;
        virtual void exportTiePointsRSGISMapOffs(std::string filepath)throw(RSGISRegistrationException)=0;
		virtual ~RSGISImageRegistration();
	protected:
		void findOverlap()throw(RSGISRegistrationException);
		void defineFirstTiePoint(unsigned int *startXOff, unsigned int *startYOff, unsigned int numXPts, unsigned int numYPts, unsigned int gap) throw(RSGISRegistrationException);
		float findTiePointLocation(TiePoint *tiePt, unsigned int windowSize, unsigned int searchArea, RSGISImageSimilarityMetric *metric, float metricThreshold, unsigned int subPixelResolution, float *moveInX, float *moveInY) throw(RSGISRegistrationException);
        float findTiePointLocation(TiePoint *tiePt, unsigned int windowSize, unsigned int searchArea, RSGISImageSimilarityMetric *metric, unsigned int subPixelResolution, float *moveInX, float *moveInY) throw(RSGISRegistrationException);
		float findExtreme(bool findMin, gsl_vector *coefficients, unsigned int order, float minRange, float maxRange, unsigned int resolution, float *extremeVal);
        void getImageOverlapFloat(GDALDataset **datasets, int numDS,  float **dsOffsets, int *width, int *height, double *gdalTransform) throw(RSGISRegistrationException);
		void getImageOverlapWithFloatShift(float xShift, float yShift, int **dsOffsets, int *width, int *height, double *gdalTransform, geos::geom::Envelope *env, float *remainderX, float *remainderY) throw(RSGISRegistrationException);
		void removeTiePointsWithLowStdDev(std::list<TiePoint*> *tiePts, unsigned int windowSize, float stdDevRefThreshold, float stdDevFloatThreshold);
		double calcStdDev(float **data, unsigned int numVals, unsigned int numDims);
		void exportTiePointsENVIImage2MapImpl(std::string filepath, std::list<TiePoint*> *tiePts)throw(RSGISRegistrationException);
		void exportTiePointsENVIImage2ImageImpl(std::string filepath, std::list<TiePoint*> *tiePts)throw(RSGISRegistrationException);
		void exportTiePointsRSGISImage2MapImpl(std::string filepath, std::list<TiePoint*> *tiePts)throw(RSGISRegistrationException);
        void exportTiePointsRSGISMapOffsImpl(std::string filepath, std::list<TiePoint*> *tiePts)throw(RSGISRegistrationException);
		GDALDataset *referenceIMG;
		GDALDataset *floatingIMG;
		OverlapRegion* overlap;
		bool overlapDefined;
	};
}}

#endif


