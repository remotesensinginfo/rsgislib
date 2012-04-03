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

namespace rsgis {namespace math{
	
    struct RSGISClusterCentre
    {
        vector<float> centre;
        unsigned int numPxl;
        vector<float> stdDev;
    };
    
    class RSGISClusterer
	{
	public:
		RSGISClusterer(){};
        virtual vector< RSGISClusterCentre >* calcClusterCentres(vector< vector<float> > *input, unsigned int numFeatures, unsigned int numClusters, unsigned int maxNumIterations, float degreeOfChange) throw(RSGISClustererException) = 0;
        void calcDataRanges(vector< vector<float> > *input, unsigned int numFeatures, float *min, float *max)throw(RSGISClustererException);
        void calcDataStats(vector< vector<float> > *input, unsigned int numFeatures, float *min, float *max, float *mean, float *stddev)throw(RSGISClustererException);
        vector< RSGISClusterCentre >* initializeClusterCentresRandom(unsigned int numFeatures, float *min, float *max, unsigned int numClusters)throw(RSGISClustererException);
        vector< RSGISClusterCentre >* initializeClusterCentresRandom(vector< vector<float> > *input, unsigned int numFeatures, unsigned int numClusters)throw(RSGISClustererException);
        vector< RSGISClusterCentre >* initializeClusterCentresDiagonal(unsigned int numFeatures, float *min, float *max, unsigned int numClusters)throw(RSGISClustererException);
        vector< RSGISClusterCentre >* initializeClusterCentresDiagonal(unsigned int numFeatures, float *min, float *max, float *mean, float *stddev, unsigned int numClusters)throw(RSGISClustererException);
        vector< RSGISClusterCentre >* initializeClusterCentresDiagonal(vector< vector<float> > *input, unsigned int numFeatures, float *min, float *max, unsigned int numClusters)throw(RSGISClustererException);
        vector< RSGISClusterCentre >* initializeClusterCentresDiagonal(vector< vector<float> > *input, unsigned int numFeatures, float *min, float *max, float *mean, float *stddev, unsigned int numClusters)throw(RSGISClustererException);
        vector< RSGISClusterCentre >* initializeClusterCentresKPP(vector< vector<float> > *input, unsigned int numFeatures, float *min, float *max, unsigned int numClusters)throw(RSGISClustererException);
        
        vector< pair< unsigned int, vector<float> > >* createClusterDataInitClusterIDs(vector< vector<float> > *input, vector< RSGISClusterCentre > *clusterCentres);
        unsigned int reassignClusterIDs( vector< pair< unsigned int, vector<float> > > *clusterData, vector< RSGISClusterCentre > *clusterCentres);
        void recalcClusterCentres( vector< pair< unsigned int, vector<float> > > *clusterData, vector< RSGISClusterCentre > *clusterCentres, bool calcStdDev);
        void assign2ClosestDataPoint(RSGISClusterCentre *cc, vector< vector<float> > *input, unsigned int numFeatures, vector< RSGISClusterCentre > *used)throw(RSGISClustererException);
        
        ~RSGISClusterer(){};
    protected:
        double calcEucDistance(vector<float> d1, vector<float> d2)throw(RSGISClustererException)
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
    
    class RSGISKMeansClusterer: public RSGISClusterer
    {
    public:
		RSGISKMeansClusterer(InitClustererMethods initCentres);
        vector< RSGISClusterCentre >* calcClusterCentres(vector< vector<float> > *input, unsigned int numFeatures, unsigned int numClusters, unsigned int maxNumIterations, float degreeOfChange) throw(RSGISClustererException);
		~RSGISKMeansClusterer();
    private:
        InitClustererMethods initCentres;
    };
    
    class RSGISISODataClusterer: public RSGISClusterer
    {
    public:
		RSGISISODataClusterer(InitClustererMethods initCentres, float minDistBetweenClusters, unsigned int minNumFeatures, float maxStdDev, unsigned int minNumClusters, unsigned int startIteration, unsigned int endIteration);
        vector< RSGISClusterCentre >* calcClusterCentres(vector< vector<float> > *input, unsigned int numFeatures, unsigned int numClusters, unsigned int maxNumIterations, float degreeOfChange) throw(RSGISClustererException);
		void addRemoveClusters(vector< RSGISClusterCentre > *clusterCentres);
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



