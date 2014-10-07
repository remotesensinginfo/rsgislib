/*
 *  RSGISZonalPolygons.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 30/07/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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

#ifndef RSGISZonalPolygons_H
#define RSGISZonalPolygons_H

#include <iostream>
#include <string>

#include "vec/RSGISPolygonData.h"
#include "vec/RSGISVectorUtils.h"

namespace rsgis{namespace vec{
	
	class DllExport RSGISZonalPolygons : public RSGISPolygonData
		{
		public:
			RSGISZonalPolygons(int numBands, bool **toCalc);
			virtual void readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn);
			virtual void createLayerDefinition(OGRLayer *outputSHPLayer)throw(RSGISVectorOutputException);
			virtual void populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn);
			void setMean(double mean, int index);
			void setMin(double min, int index);
			void setMax(double max, int index);
			void setStdDev(double stddev, int index);
			double* getMean();
			double* getMax();
			double* getMin();
			double* getStdDev();
			bool calcStdDev(int band);
			~RSGISZonalPolygons();
		protected:
			double *mean;
			double *min;
			double *max;
			double *stddev;
			double numBands;
			bool **toCalc;
		};
}}

#endif


