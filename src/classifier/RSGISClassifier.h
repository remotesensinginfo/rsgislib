/*
 *  RSGISClassifier.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/08/2008.
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

#ifndef RSGISClassifier_H
#define RSGISClassifier_H

#include <iostream>
#include <fstream>
#include <string>
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageCalcException.h"
#include "common/RSGISClassificationException.h"
#include "utils/RSGIS_ENVI_ASCII_ROI.h"

namespace rsgis{ namespace classifier{
    
	struct DllExport ClassData
	{
		std::string className;
		int classID;
        rsgis::math::Matrix *data;
	};
	
	struct DllExport ClusterCentre
	{
		std::string className;
		int classID;
		rsgis::math::Vector *data;
		unsigned int numVals;
	};
	
	struct DllExport ClusterCentreISO
	{
		std::string className;
		int classID;
		rsgis::math::Vector *data;
		rsgis::math::Vector *stddev;
		unsigned int numVals;
		bool split;
		double avgDist;
	};
	
	class DllExport RSGISClassifier
	{
	public:
		RSGISClassifier(ClassData **trainingData, int numClasses) throw(RSGISClassificationException);
		virtual int getClassID(float *variables, int numVars) throw(RSGISClassificationException) = 0;
		virtual std::string getClassName(float *variables, int numVars) throw(RSGISClassificationException) = 0;
		int getNumVariables();
		void printClassIDs();
		virtual ~RSGISClassifier();
	protected:
		ClassData **trainingData;
		int numClasses;
		int numVariables;
	};
	
	class DllExport RSGISApplyClassifier : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISApplyClassifier(int numberOutBands, RSGISClassifier *classifier);
		void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException);
		~RSGISApplyClassifier();
	protected:
		RSGISClassifier *classifier;
	};
}}

#endif

