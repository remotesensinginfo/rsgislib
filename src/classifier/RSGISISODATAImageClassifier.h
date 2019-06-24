 /*
 *  RSGISISODATAImageClassifier.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/08/2010.
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



#ifndef RSGISISODATAImageClassifier_H
#define RSGISISODATAImageClassifier_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageStatistics.h"

#include "common/RSGISClassificationException.h"

#include "classifier/RSGISClassifier.h"

#include "gdal_priv.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_classify_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{ namespace classifier{
	
	class DllExport RSGISISODATAClassifier
	{
	public:
		RSGISISODATAClassifier(std::string inputImageFile, bool printinfo);
		void initClusterCentresRandom(unsigned int numClusters);
		void initClusterCentresKpp(unsigned int numClusters);
		void calcClusterCentres(double terminalThreshold, unsigned int maxIterations, unsigned int minNumVals, double minDistanceBetweenCentres, double stddevThres, float propOverAvgDist);
		void generateOutputImage(std::string outputImageFile);
		~RSGISISODATAClassifier();
	protected:
		std::string inputImageFile;
		std::vector<ClusterCentreISO*> *clusterCentres;
		bool hasInitClusterCentres;
		GDALDataset **datasets;
		unsigned int numDatasets;
		unsigned int numImageBands;
		unsigned int clusterIDVal;
		bool printinfo;
	};
	
	class DllExport RSGISISODATACalcPixelClusterCalcImageVal : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISISODATACalcPixelClusterCalcImageVal(int numOutBands, std::vector<ClusterCentreISO*> *clusterCentres, unsigned int numImageBands);
		void calcImageValue(float *bandValues, int numBands, double *output) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		std::vector<ClusterCentreISO*>* getNewClusterCentres();
		void reset(std::vector<ClusterCentreISO*> *clusterCentres);
		double getAverageDistance();
		~RSGISISODATACalcPixelClusterCalcImageVal();
	protected:
		std::vector<ClusterCentreISO*> *clusterCentres;
		std::vector<ClusterCentreISO*> *newClusterCentres;
		unsigned int numImageBands;
		double sumDist;
		unsigned long numVals;
	};
	
	class DllExport RSGISISODATACalcPixelClusterStdDevCalcImageVal : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISISODATACalcPixelClusterStdDevCalcImageVal(int numOutBands, std::vector<ClusterCentreISO*> *clusterCentres, unsigned int numImageBands);
		void calcImageValue(float *bandValues, int numBands, double *output) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void reset(std::vector<ClusterCentreISO*> *clusterCentres);
		~RSGISISODATACalcPixelClusterStdDevCalcImageVal();
	protected:
		std::vector<ClusterCentreISO*> *clusterCentres;
		unsigned int numImageBands;
	};
	
	class DllExport RSGISApplyISODATAClassifierCalcImageVal : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISApplyISODATAClassifierCalcImageVal(int numOutBands, std::vector<ClusterCentreISO*> *clusterCentres);
		void calcImageValue(float *bandValues, int numBands, double *output);
		void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		~RSGISApplyISODATAClassifierCalcImageVal();
	protected:
		std::vector<ClusterCentreISO*> *clusterCentres;
	};
}}

#endif







