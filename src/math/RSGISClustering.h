/*
 *  RSGISClustering.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 14/02/2012.
 *  Copyright 2012 RSGISLib.
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

#ifndef RSGISClustering_h
#define RSGISClustering_h

#include <iostream>
#include <math.h>
#include <vector>

#include "math/RSGISProbabilityDistributions.h"
#include "math/RSGISRandomDistro.h"
#include "math/RSGISClustererException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_maths_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis {namespace math{
	
    struct DllExport RSGISClusterCentre
    {
        std::vector<float> centre;
        unsigned int numPxl;
        std::vector<float> stdDev;
    };
    
    class DllExport RSGISClusterer
	{
	public:
		RSGISClusterer(){};
        virtual std::vector< RSGISClusterCentre >* calcClusterCentres(std::vector< std::vector<float> > *input, unsigned int numFeatures, unsigned int numClusters, unsigned int maxNumIterations, float degreeOfChange) = 0;
        void calcDataRanges(std::vector< std::vector<float> > *input, unsigned int numFeatures, float *min, float *max);
        void calcDataStats(std::vector< std::vector<float> > *input, unsigned int numFeatures, float *min, float *max, float *mean, float *stddev);
        std::vector< RSGISClusterCentre >* initializeClusterCentresRandom(unsigned int numFeatures, float *min, float *max, unsigned int numClusters);
        std::vector< RSGISClusterCentre >* initializeClusterCentresRandom(std::vector< std::vector<float> > *input, unsigned int numFeatures, unsigned int numClusters);
        std::vector< RSGISClusterCentre >* initializeClusterCentresDiagonal(unsigned int numFeatures, float *min, float *max, unsigned int numClusters);
        std::vector< RSGISClusterCentre >* initializeClusterCentresDiagonal(unsigned int numFeatures, float *min, float *max, float *mean, float *stddev, unsigned int numClusters);
        std::vector< RSGISClusterCentre >* initializeClusterCentresDiagonal(std::vector< std::vector<float> > *input, unsigned int numFeatures, float *min, float *max, unsigned int numClusters);
        std::vector< RSGISClusterCentre >* initializeClusterCentresDiagonal(std::vector< std::vector<float> > *input, unsigned int numFeatures, float *min, float *max, float *mean, float *stddev, unsigned int numClusters);
        std::vector< RSGISClusterCentre >* initializeClusterCentresKPP(std::vector< std::vector<float> > *input, unsigned int numFeatures, float *min, float *max, unsigned int numClusters);
        
        std::vector< std::pair< unsigned int, std::vector<float> > >* createClusterDataInitClusterIDs(std::vector< std::vector<float> > *input, std::vector< RSGISClusterCentre > *clusterCentres);
        unsigned int reassignClusterIDs( std::vector< std::pair< unsigned int, std::vector<float> > > *clusterData, std::vector< RSGISClusterCentre > *clusterCentres);
        void recalcClusterCentres( std::vector< std::pair< unsigned int, std::vector<float> > > *clusterData, std::vector< RSGISClusterCentre > *clusterCentres, bool calcStdDev);
        void assign2ClosestDataPoint(RSGISClusterCentre *cc, std::vector< std::vector<float> > *input, unsigned int numFeatures, std::vector< RSGISClusterCentre > *used);
        
        ~RSGISClusterer(){};
    protected:
        double calcEucDistance(std::vector<float> d1, std::vector<float> d2)
        {
            unsigned int numVals = d1.size(); 
            if(numVals != d2.size())
            {
                throw RSGISClustererException("Cannot calculate Euclidean distance for vectors of different length.");
            }
            double dist = 0;
            for(unsigned int i = 0; i < numVals; ++i)
            {
                dist += (d1[i] - d2[i]) * (d1[i] - d2[i]);
            }
            
            if(dist != 0)
            {
                dist = sqrt(dist/numVals);
            }
            
            return dist;
        };
	};
    
    enum InitClustererMethods
    {
        init_random,
        init_diagonal_full,
        init_diagonal_stddev,
        init_diagonal_full_attach,
        init_diagonal_stddev_attach,
        init_kpp
    };
    
    class DllExport RSGISKMeansClusterer: public RSGISClusterer
    {
    public:
		RSGISKMeansClusterer(InitClustererMethods initCentres);
        std::vector< RSGISClusterCentre >* calcClusterCentres(std::vector< std::vector<float> > *input, unsigned int numFeatures, unsigned int numClusters, unsigned int maxNumIterations, float degreeOfChange);
		~RSGISKMeansClusterer();
    private:
        InitClustererMethods initCentres;
    };
    
    class DllExport RSGISISODataClusterer: public RSGISClusterer
    {
    public:
		RSGISISODataClusterer(InitClustererMethods initCentres, float minDistBetweenClusters, unsigned int minNumFeatures, float maxStdDev, unsigned int minNumClusters, unsigned int startIteration, unsigned int endIteration);
        std::vector< RSGISClusterCentre >* calcClusterCentres(std::vector< std::vector<float> > *input, unsigned int numFeatures, unsigned int numClusters, unsigned int maxNumIterations, float degreeOfChange);
		void addRemoveClusters(std::vector< RSGISClusterCentre > *clusterCentres);
        ~RSGISISODataClusterer();
    private:
        InitClustererMethods initCentres;
        float minDistBetweenClusters;
        unsigned int minNumFeatures;
        float maxStdDev;
        unsigned int minNumClusters;
        unsigned int startIteration;
        unsigned int endIteration;
    };
}}

#endif



