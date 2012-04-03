 /*
 *  RSGISKMeanImageClassifier.h
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


#ifndef RSGISKMeanImageClassifier_H
#define RSGISKMeanImageClassifier_H

#include <iostream>
#include <fstream>
#include <string>

#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageStatistics.h"

#include "common/RSGISClassificationException.h"

#include "utils/RSGISExportForPlotting.h"

#include "classifier/RSGISClassifier.h"

#include "gdal_priv.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

using namespace rsgis;
using namespace rsgis::img;
using namespace rsgis::utils;
using namespace std;

namespace rsgis{ namespace classifier{
	
	
	class RSGISKMeansClassifier
	{
	public:
		RSGISKMeansClassifier(string inputImageFile, bool printinfo);
		void initClusterCentresRandom(unsigned int numClusters)throw(RSGISClassificationException);
		void initClusterCentresKpp(unsigned int numClusters)throw(RSGISClassificationException);
		void calcClusterCentres(double terminalThreshold, unsigned int maxIterations)throw(RSGISClassificationException);
		void generateOutputImage(string outputImageFile)throw(RSGISClassificationException);
		~RSGISKMeansClassifier();
	protected:
		string inputImageFile;
		ClusterCentre **clusterCentres;
		unsigned int numClusters;
		bool hasInitClusterCentres;
		GDALDataset **datasets;
		unsigned int numDatasets;
		unsigned int numImageBands;
		bool printinfo;
	};
	
	class RSGISKMeanCalcPixelClusterCalcImageVal : public RSGISCalcImageValue
	{
	public: 
		RSGISKMeanCalcPixelClusterCalcImageVal(int numOutBands, ClusterCentre **clusterCentres, unsigned int numClusters, unsigned int numImageBands);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		unsigned long* getPxlsInClusters();
		ClusterCentre** getNewClusterCentres();
		void reset();
		~RSGISKMeanCalcPixelClusterCalcImageVal();
	protected:
		ClusterCentre **clusterCentres;
		unsigned int numClusters;
		ClusterCentre **newClusterCentres;
		unsigned long *numPxlInClusters;
		unsigned int numImageBands;
	};
	
	class RSGISCalcDist2NrCentreCalcImageVal : public RSGISCalcImageValue
	{
	public: 
		RSGISCalcDist2NrCentreCalcImageVal(int numOutBands, ClusterCentre **clusterCentres, unsigned int numClusters);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		double getSumSqDistance();
		void reset();
		void setClusterCentres(ClusterCentre **clusterCentres, unsigned int numClusters);
		~RSGISCalcDist2NrCentreCalcImageVal();
	protected:
		ClusterCentre **clusterCentres;
		unsigned int numClusters;
		double sumSqDistance;
	};
	
	
	class RSGISCalcDist2NrCentreDistributionCalcImageVal : public RSGISCalcImageValue
	{
	public: 
		RSGISCalcDist2NrCentreDistributionCalcImageVal(int numOutBands, ClusterCentre **clusterCentres, unsigned int numClusters, double sqDistance);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void setClusterCentres(ClusterCentre **clusterCentres, unsigned int numClusters, double sqDistance);
		~RSGISCalcDist2NrCentreDistributionCalcImageVal();
	protected:
		ClusterCentre **clusterCentres;
		unsigned int numClusters;
		double sqDistance;
	};
	
	
	
	class RSGISApplyKMeanClassifierCalcImageVal : public RSGISCalcImageValue
	{
	public: 
		RSGISApplyKMeanClassifierCalcImageVal(int numOutBands, ClusterCentre **clusterCentres, unsigned int numClusters);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		~RSGISApplyKMeanClassifierCalcImageVal();
	protected:
		ClusterCentre **clusterCentres;
		unsigned int numClusters;
	};
}}

#endif




