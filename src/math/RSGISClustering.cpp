/*
 *  RSGISClustering.cpp
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

#include "RSGISClustering.h"


namespace rsgis {namespace math{

    void RSGISClusterer::calcDataRanges(std::vector< std::vector<float> > *input, unsigned int numFeatures, float *min, float *max)
    {
        bool first = true;
        std::vector< std::vector<float> >::iterator iterFeatures;
        for(iterFeatures = input->begin(); iterFeatures != input->end(); ++iterFeatures)
        {
            if(first)
            {
                for(unsigned int i = 0; i < numFeatures; ++i)
                {
                    min[i] = (*iterFeatures)[i];
                    max[i] = (*iterFeatures)[i];
                }
                first = false;
            }
            else
            {
                for(unsigned int i = 0; i < numFeatures; ++i)
                {
                    if((*iterFeatures)[i] < min[i])
                    {
                         min[i] = (*iterFeatures)[i];
                    }
                    else if((*iterFeatures)[i] > max[i])
                    {
                        max[i] = (*iterFeatures)[i];
                    }
                }
            }
        }
    }
    
    void RSGISClusterer::calcDataStats(std::vector< std::vector<float> > *input, unsigned int numFeatures, float *min, float *max, float *mean, float *stddev)
    {
        bool first = true;
        std::vector< std::vector<float> >::iterator iterFeatures;
        for(iterFeatures = input->begin(); iterFeatures != input->end(); ++iterFeatures)
        {
            if(first)
            {
                for(unsigned int i = 0; i < numFeatures; ++i)
                {
                    min[i] = (*iterFeatures)[i];
                    max[i] = (*iterFeatures)[i];
                    mean[i] = (*iterFeatures)[i];
                }
                first = false;
            }
            else
            {
                for(unsigned int i = 0; i < numFeatures; ++i)
                {
                    if((*iterFeatures)[i] < min[i])
                    {
                        min[i] = (*iterFeatures)[i];
                    }
                    else if((*iterFeatures)[i] > max[i])
                    {
                        max[i] = (*iterFeatures)[i];
                    }
                    mean[i] += (*iterFeatures)[i];
                }
            }
        }
        
        for(unsigned int i = 0; i < numFeatures; ++i)
        {
            mean[i] = mean[i]/input->size();
        }
        
        first = true;
        for(iterFeatures = input->begin(); iterFeatures != input->end(); ++iterFeatures)
        {
            if(first)
            {
                for(unsigned int i = 0; i < numFeatures; ++i)
                {
                    stddev[i] = (((*iterFeatures)[i] - mean[i]) * ((*iterFeatures)[i] - mean[i]));
                }
                first = false;
            }
            else
            {
                for(unsigned int i = 0; i < numFeatures; ++i)
                {
                    stddev[i] += (((*iterFeatures)[i] - mean[i]) * ((*iterFeatures)[i] - mean[i]));
                }
            }
        }
        
        for(unsigned int i = 0; i < numFeatures; ++i)
        {
            stddev[i] = sqrt(stddev[i]/input->size());
        }
        
    }
    
    std::vector< RSGISClusterCentre >* RSGISClusterer::initializeClusterCentresRandom(unsigned int numFeatures, float *min, float *max, unsigned int numClusters)
    {
        std::vector< RSGISClusterCentre > *clusterCentres = new std::vector< RSGISClusterCentre >();
        clusterCentres->reserve(numClusters);
        
        RSGISPsudoRandDistroUniformDouble probDist(0, 1);
        
        float value = 0;
        
        float *range = new float[numFeatures];
        for(unsigned int i = 0; i < numFeatures; ++i)
        {
            range[i] = max[i] - min[i];
        }
        
        for(unsigned int i = 0; i < numClusters; ++i)
        {
            RSGISClusterCentre cCentre;
            cCentre.centre.reserve(numFeatures);
            cCentre.numPxl = 0;
            cCentre.stdDev.reserve(numFeatures);
            
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                value = min[j] + (probDist.calcRand()*range[j]);
                cCentre.centre.push_back(value);
                cCentre.stdDev.push_back(0);
            }            
            
            clusterCentres->push_back(cCentre);
        }
        
        delete[] range;
        
        return clusterCentres;
    }
    
    std::vector< RSGISClusterCentre >* RSGISClusterer::initializeClusterCentresRandom(std::vector< std::vector<float> > *input, unsigned int numFeatures, unsigned int numClusters)
    {
        std::vector< RSGISClusterCentre > *clusterCentres = new std::vector< RSGISClusterCentre >();
        clusterCentres->reserve(numClusters);
        
        RSGISPsudoRandDistroUniformDouble probDist(0, 1);
        
        unsigned int sampleIndex = 0;
        size_t numVals = input->size(); 
        
        if(numVals < numClusters)
        {
            throw RSGISClustererException("Too many clusters. There needs to be more data points than clusters.");
        }
        
        std::vector<unsigned int> indexesUsed;
        bool findingIdx = true;
        bool idxUsed = false;
        bool sameSeed = true;
        
        for(unsigned int i = 0; i < numClusters; ++i)
        {
            RSGISClusterCentre cCentre;
            cCentre.centre.reserve(numFeatures);
            cCentre.numPxl = 0;
            cCentre.stdDev.reserve(numFeatures);
            
            findingIdx = true;
            while(findingIdx)
            {
                sampleIndex = (probDist.calcRand()*numVals);
                idxUsed = false;
                
                for(std::vector<unsigned int>::iterator iterIdxs = indexesUsed.begin(); iterIdxs != indexesUsed.end(); ++iterIdxs)
                {
                    if((*iterIdxs) == sampleIndex)
                    {
                        idxUsed = true;
                        break;
                    }
                    
                    sameSeed = true;
                    for(unsigned int j = 0; j < numFeatures; ++j)
                    {
                        if(input->at(*iterIdxs)[j] != input->at(sampleIndex)[j])
                        {
                            sameSeed = false;
                        }
                    }
                    
                    if(sameSeed)
                    {
                        idxUsed = true;
                        break;
                    }
                }
                
                if(!idxUsed)
                {
                    indexesUsed.push_back(sampleIndex);
                    findingIdx = false;
                    break;
                }
            }
            
            std::vector<float> sample = input->at(sampleIndex);
            
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                cCentre.centre.push_back(sample[j]);
                cCentre.stdDev.push_back(0);
            }
            
            clusterCentres->push_back(cCentre);
        }
        
        return clusterCentres;
    }
    
    std::vector< RSGISClusterCentre >* RSGISClusterer::initializeClusterCentresDiagonal(unsigned int numFeatures, float *min, float *max, unsigned int numClusters)
    {
        std::vector< RSGISClusterCentre > *clusterCentres = new std::vector< RSGISClusterCentre >();
        clusterCentres->reserve(numClusters);
                
        float *range = new float[numFeatures];
        float *step = new float[numFeatures];
        for(unsigned int i = 0; i < numFeatures; ++i)
        {
            range[i] = max[i] - min[i];
            step[i] = range[i]/numClusters;
        }
        
        float value = 0;
        for(unsigned int i = 0; i < numClusters; ++i)
        {
            RSGISClusterCentre cCentre;
            cCentre.centre.reserve(numFeatures);
            cCentre.numPxl = 0;
            cCentre.stdDev.reserve(numFeatures);
            
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                value = min[j] + (step[j] * i);
                cCentre.centre.push_back(value);
                cCentre.stdDev.push_back(0);
            }
            
            clusterCentres->push_back(cCentre);
        }
        
        delete[] range;
        delete[] step;
        
        return clusterCentres;
    }
    
    std::vector< RSGISClusterCentre >* RSGISClusterer::initializeClusterCentresDiagonal(unsigned int numFeatures, float *min, float *max, float *mean, float *stddev, unsigned int numClusters)
    {
        std::vector< RSGISClusterCentre > *clusterCentres = new std::vector< RSGISClusterCentre >();
        clusterCentres->reserve(numClusters);
        
        float *p2StdDev = new float[numFeatures];
        float *m2StdDev = new float[numFeatures];
        float *range = new float[numFeatures];
        float *step = new float[numFeatures];
        bool betweenStdDevMaxAndMax = false;
        bool betweenStdDevMinAndMin = false;
        for(unsigned int i = 0; i < numFeatures; ++i)
        {
            p2StdDev[i] = mean[i] + (stddev[i]*2);
            if(p2StdDev[i] > max[i])
            {
                p2StdDev[i] = max[i];
            }
            m2StdDev[i] = mean[i] - (stddev[i]*2);
            if(m2StdDev[i] < min[i])
            {
                m2StdDev[i] = min[i];
            }
            
            range[i] = p2StdDev[i] - m2StdDev[i];
            step[i] = range[i]/numClusters;
            
            if(p2StdDev[i] != max[i])
            {
                betweenStdDevMaxAndMax = true;
            }
            if(m2StdDev[i] != min[i])
            {
                betweenStdDevMinAndMin = true;
            }
        }
        
        unsigned int extras = 0;
        if(betweenStdDevMaxAndMax)
        {
            extras += 2;
        }
        if(betweenStdDevMinAndMin)
        {
            extras += 2;
        }
        
        
        float value = 0;
        
        
        if(betweenStdDevMinAndMin)
        {
            RSGISClusterCentre cCentreMin;
            cCentreMin.centre.reserve(numFeatures);
            cCentreMin.numPxl = 0;
            cCentreMin.stdDev.reserve(numFeatures);
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                cCentreMin.centre.push_back(max[j]);
                cCentreMin.stdDev.push_back(0);
            }
            clusterCentres->push_back(cCentreMin);
            
            RSGISClusterCentre cCentreMinMid;
            cCentreMinMid.centre.reserve(numFeatures);
            cCentreMinMid.numPxl = 0;
            cCentreMinMid.stdDev.reserve(numFeatures);
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                cCentreMinMid.centre.push_back(min[j] + ((m2StdDev[j]-min[j])/2));
                cCentreMinMid.stdDev.push_back(0);
            }
            clusterCentres->push_back(cCentreMinMid);
        }        
        
        for(unsigned int i = 0; i < numClusters-extras; ++i)
        {
            RSGISClusterCentre cCentre;
            cCentre.centre.reserve(numFeatures);
            cCentre.numPxl = 0;
            cCentre.stdDev.reserve(numFeatures);
            
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                value = m2StdDev[j] + (step[j] * i);
                cCentre.centre.push_back(value);
                cCentre.stdDev.push_back(0);
            }
            
            clusterCentres->push_back(cCentre);
        }
        
        if(betweenStdDevMaxAndMax)
        {
            RSGISClusterCentre cCentreMaxMid;
            cCentreMaxMid.centre.reserve(numFeatures);
            cCentreMaxMid.numPxl = 0;
            cCentreMaxMid.stdDev.reserve(numFeatures);
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                cCentreMaxMid.centre.push_back(p2StdDev[j] + ((max[j]-p2StdDev[j])/2));
                cCentreMaxMid.stdDev.push_back(0);
            }
            clusterCentres->push_back(cCentreMaxMid);
            
            RSGISClusterCentre cCentreMax;
            cCentreMax.centre.reserve(numFeatures);
            cCentreMax.numPxl = 0;
            cCentreMax.stdDev.reserve(numFeatures);
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                cCentreMax.centre.push_back(max[j]);
                cCentreMax.stdDev.push_back(0);
            }
            clusterCentres->push_back(cCentreMax);            
        }
        
        delete[] p2StdDev;
        delete[] m2StdDev;
        delete[] range;
        delete[] step;
        
        return clusterCentres;
    }
    
    std::vector< RSGISClusterCentre >* RSGISClusterer::initializeClusterCentresDiagonal(std::vector< std::vector<float> > *input, unsigned int numFeatures, float *min, float *max, unsigned int numClusters)
    {
        std::vector< RSGISClusterCentre > *clusterCentres = new std::vector< RSGISClusterCentre >();
        clusterCentres->reserve(numClusters);
        try 
        {
            float *range = new float[numFeatures];
            float *step = new float[numFeatures];
            for(unsigned int i = 0; i < numFeatures; ++i)
            {
                range[i] = max[i] - min[i];
                step[i] = range[i]/numClusters;
            }
            
            float value = 0;
            for(unsigned int i = 0; i < numClusters; ++i)
            {
                RSGISClusterCentre cCentre;
                cCentre.centre.reserve(numFeatures);
                cCentre.numPxl = 0;
                cCentre.stdDev.reserve(numFeatures);
                
                for(unsigned int j = 0; j < numFeatures; ++j)
                {
                    value = min[j] + (step[j] * i);
                    cCentre.centre.push_back(value);
                    cCentre.stdDev.push_back(0);
                }

                this->assign2ClosestDataPoint(&cCentre, input, numFeatures, clusterCentres);
                clusterCentres->push_back(cCentre);
            }
            
            delete[] range;
            delete[] step;
        } 
        catch (RSGISClustererException &e) 
        {
            throw e;
        }
        return clusterCentres;
    }
    
    std::vector< RSGISClusterCentre >* RSGISClusterer::initializeClusterCentresDiagonal(std::vector< std::vector<float> > *input, unsigned int numFeatures, float *min, float *max, float *mean, float *stddev, unsigned int numClusters)
    {
        std::vector< RSGISClusterCentre > *clusterCentres = new std::vector< RSGISClusterCentre >();
        clusterCentres->reserve(numClusters);
        
        float *p2StdDev = new float[numFeatures];
        float *m2StdDev = new float[numFeatures];
        float *range = new float[numFeatures];
        float *step = new float[numFeatures];
        bool betweenStdDevMaxAndMax = false;
        bool betweenStdDevMinAndMin = false;
        for(unsigned int i = 0; i < numFeatures; ++i)
        {
            p2StdDev[i] = mean[i] + (stddev[i]*2);
            if(p2StdDev[i] > max[i])
            {
                p2StdDev[i] = max[i];
            }
            m2StdDev[i] = mean[i] - (stddev[i]*2);
            if(m2StdDev[i] < min[i])
            {
                m2StdDev[i] = min[i];
            }
            
            range[i] = p2StdDev[i] - m2StdDev[i];
            step[i] = range[i]/numClusters;
            
            if(p2StdDev[i] != max[i])
            {
                betweenStdDevMaxAndMax = true;
            }
            if(m2StdDev[i] != min[i])
            {
                betweenStdDevMinAndMin = true;
            }
        }
        
        unsigned int extras = 0;
        if(betweenStdDevMaxAndMax)
        {
            extras += 2;
        }
        if(betweenStdDevMinAndMin)
        {
            extras += 2;
        }
        
        
        float value = 0;
        
        
        if(betweenStdDevMinAndMin)
        {
            RSGISClusterCentre cCentreMin;
            cCentreMin.centre.reserve(numFeatures);
            cCentreMin.numPxl = 0;
            cCentreMin.stdDev.reserve(numFeatures);
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                cCentreMin.centre.push_back(max[j]);
                cCentreMin.stdDev.push_back(0);
            }
            this->assign2ClosestDataPoint(&cCentreMin, input, numFeatures, clusterCentres);
            clusterCentres->push_back(cCentreMin);
            
            RSGISClusterCentre cCentreMinMid;
            cCentreMinMid.centre.reserve(numFeatures);
            cCentreMinMid.numPxl = 0;
            cCentreMinMid.stdDev.reserve(numFeatures);
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                cCentreMinMid.centre.push_back(min[j] + ((m2StdDev[j]-min[j])/2));
                cCentreMinMid.stdDev.push_back(0);
            }
            this->assign2ClosestDataPoint(&cCentreMinMid, input, numFeatures, clusterCentres);
            clusterCentres->push_back(cCentreMinMid);
        }        
        
        for(unsigned int i = 0; i < numClusters-extras; ++i)
        {
            RSGISClusterCentre cCentre;
            cCentre.centre.reserve(numFeatures);
            cCentre.numPxl = 0;
            cCentre.stdDev.reserve(numFeatures);
            
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                value = m2StdDev[j] + (step[j] * i);
                cCentre.centre.push_back(value);
                cCentre.stdDev.push_back(0);
            }
            this->assign2ClosestDataPoint(&cCentre, input, numFeatures, clusterCentres);
            clusterCentres->push_back(cCentre);
        }
        
        if(betweenStdDevMaxAndMax)
        {
            RSGISClusterCentre cCentreMaxMid;
            cCentreMaxMid.centre.reserve(numFeatures);
            cCentreMaxMid.numPxl = 0;
            cCentreMaxMid.stdDev.reserve(numFeatures);
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                cCentreMaxMid.centre.push_back(p2StdDev[j] + ((max[j]-p2StdDev[j])/2));
                cCentreMaxMid.stdDev.push_back(0);
            }
            this->assign2ClosestDataPoint(&cCentreMaxMid, input, numFeatures, clusterCentres);
            clusterCentres->push_back(cCentreMaxMid);
            
            RSGISClusterCentre cCentreMax;
            cCentreMax.centre.reserve(numFeatures);
            cCentreMax.numPxl = 0;
            cCentreMax.stdDev.reserve(numFeatures);
            for(unsigned int j = 0; j < numFeatures; ++j)
            {
                cCentreMax.centre.push_back(max[j]);
                cCentreMax.stdDev.push_back(0);
            }
            this->assign2ClosestDataPoint(&cCentreMax, input, numFeatures, clusterCentres);
            clusterCentres->push_back(cCentreMax);            
        }
        
        delete[] p2StdDev;
        delete[] m2StdDev;
        delete[] range;
        delete[] step;
        
        return clusterCentres;
    }
        
    std::vector< RSGISClusterCentre >* RSGISClusterer::initializeClusterCentresKPP(std::vector< std::vector<float> > *input, unsigned int numFeatures, float *min, float *max, unsigned int numClusters)
    {
        throw RSGISClustererException("initializeClusterCentresKPP is not implemented.");
        return NULL;
    }
        
    std::vector< std::pair< unsigned int, std::vector<float> > >* RSGISClusterer::createClusterDataInitClusterIDs(std::vector< std::vector<float> > *input, std::vector< RSGISClusterCentre > *clusterCentres)
    {
        float dist = 0;
        float minDist = 0;
        unsigned int clusterID = 0;
        
        std::vector< std::pair< unsigned int, std::vector<float> > > *clusterData = new std::vector< std::pair< unsigned int, std::vector<float> > >();
        for(std::vector< std::vector<float> >::iterator iterInData = input->begin(); iterInData != input->end(); ++iterInData)
        {
            dist = 0;
            minDist = 0;
            clusterID = 0;
            
            for(unsigned int i = 0; i < clusterCentres->size(); ++i)
            {
                dist = 0;
                for(unsigned int j = 0; j < clusterCentres->at(i).centre.size(); ++j)
                {
                    dist += ((*iterInData)[j] - clusterCentres->at(i).centre[j]) * ((*iterInData)[j] - clusterCentres->at(i).centre[j]); 
                }
                
                dist = sqrt(dist);
                
                if(i == 0)
                {
                    minDist = dist;
                    clusterID = i;
                }
                else if(dist < minDist)
                {
                    minDist = dist;
                    clusterID = i;
                }
            }
            
            ++clusterCentres->at(clusterID).numPxl;
            clusterData->push_back(std::pair< unsigned int, std::vector<float> >(clusterID, *iterInData));
        }
        
        return clusterData;
    }
    
    unsigned int RSGISClusterer::reassignClusterIDs( std::vector< std::pair< unsigned int, std::vector<float> > > *clusterData, std::vector< RSGISClusterCentre > *clusterCentres)
    {
        unsigned int nChange = 0;
        float dist = 0;
        float minDist = 0;
        unsigned int clusterID = 0;
        
        for(std::vector< std::pair< unsigned int, std::vector<float> > >::iterator iterData = clusterData->begin(); iterData != clusterData->end(); ++iterData)
        {
            dist = 0;
            minDist = 0;
            clusterID = 0;
            
            for(unsigned int i = 0; i < clusterCentres->size(); ++i)
            {
                dist = 0;
                for(unsigned int j = 0; j < clusterCentres->at(i).centre.size(); ++j)
                {
                    dist += ((*iterData).second[j] - clusterCentres->at(i).centre[j]) * ((*iterData).second[j] - clusterCentres->at(i).centre[j]); 
                }
                
                dist = sqrt(dist);
                
                if(i == 0)
                {
                    minDist = dist;
                    clusterID = i;
                }
                else if(dist < minDist)
                {
                    minDist = dist;
                    clusterID = i;
                }
            }
            
            if(clusterID != (*iterData).first)
            {
                (*iterData).first = clusterID;
                ++nChange;
            }
        }
        
        return nChange;
    }
    
    void RSGISClusterer::recalcClusterCentres( std::vector< std::pair< unsigned int, std::vector<float> > > *clusterData, std::vector< RSGISClusterCentre > *clusterCentres, bool calcStdDev)
    {
        for(std::vector< RSGISClusterCentre >::iterator iterClusters = clusterCentres->begin(); iterClusters != clusterCentres->end(); ++iterClusters)
        {
            for(unsigned int i = 0; i < (*iterClusters).centre.size(); ++i)
            {
                (*iterClusters).centre[i] = 0;
                (*iterClusters).stdDev[i] = 0;
            }
            (*iterClusters).numPxl = 0;
        }
        
        for(std::vector< std::pair< unsigned int, std::vector<float> > >::iterator iterData = clusterData->begin(); iterData != clusterData->end(); ++iterData)
        {
            ++clusterCentres->at((*iterData).first).numPxl;
            for(unsigned int i = 0; i < (*iterData).second.size(); ++i)
            {
                clusterCentres->at((*iterData).first).centre[i] += (*iterData).second[i];
            }
        }

        for(std::vector< RSGISClusterCentre >::iterator iterClusters = clusterCentres->begin(); iterClusters != clusterCentres->end(); )
        {
            if((*iterClusters).numPxl == 0)
            {
                iterClusters = clusterCentres->erase(iterClusters);
            }
            else
            {
                for(unsigned int i = 0; i < (*iterClusters).centre.size(); ++i)
                {
                    (*iterClusters).centre[i] = (*iterClusters).centre[i]/(*iterClusters).numPxl;
                }
                ++iterClusters;
            }
        }
        
        if(calcStdDev)
        {
            for(std::vector< std::pair< unsigned int, std::vector<float> > >::iterator iterData = clusterData->begin(); iterData != clusterData->end(); ++iterData)
            {
                for(unsigned int i = 0; i < (*iterData).second.size(); ++i)
                {
                    clusterCentres->at((*iterData).first).stdDev[i] += (clusterCentres->at((*iterData).first).centre[i] - (*iterData).second[i])*(clusterCentres->at((*iterData).first).centre[i] - (*iterData).second[i]);
                }
            }
            
            for(std::vector< RSGISClusterCentre >::iterator iterClusters = clusterCentres->begin(); iterClusters != clusterCentres->end(); ++iterClusters)
            {
                for(unsigned int i = 0; i < (*iterClusters).stdDev.size(); ++i)
                {
                    (*iterClusters).stdDev[i] = sqrt((*iterClusters).stdDev[i]/(*iterClusters).numPxl);
                }
            }
        }
    }
    
    void RSGISClusterer::assign2ClosestDataPoint(RSGISClusterCentre *cc, std::vector< std::vector<float> > *input, unsigned int numFeatures, std::vector< RSGISClusterCentre > *used)
    {
        bool first = true;
        bool alreadyUsed = 0;
        double minDist = 0;
        double dist = 0;
        std::vector<float> cClosest;
        cClosest.reserve(numFeatures);
        for(unsigned int i = 0; i < numFeatures; ++i)
        {
            cClosest.push_back(0);
        }
        try 
        {
            for(std::vector< std::vector<float> >::iterator iterData = input->begin(); iterData != input->end(); ++iterData)
            {
                if(first)
                {
                    if(used->size() > 0)
                    {
                        for(std::vector< RSGISClusterCentre >::iterator iterCC = used->begin(); iterCC != used->end(); ++iterCC)
                        {
                            alreadyUsed = true;
                            for(unsigned int i = 0; i < numFeatures; ++i)
                            {
                                if((*iterData)[i] != (*iterCC).centre[i])
                                {
                                    alreadyUsed = false;
                                }
                            }
                            if(alreadyUsed)
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        alreadyUsed = false; 
                    }
                    
                    if(!alreadyUsed)
                    {
                        minDist = this->calcEucDistance(cc->centre, (*iterData));
                        for(unsigned int i = 0; i < numFeatures; ++i)
                        {
                            cClosest[i] = (*iterData)[i];
                        }
                        first = false;
                    }
                }
                else
                {
                    dist = this->calcEucDistance(cc->centre, (*iterData));
                    
                    if(dist < minDist)
                    {
                        if(used->size() > 0)
                        {
                            for(std::vector< RSGISClusterCentre >::iterator iterCC = used->begin(); iterCC != used->end(); ++iterCC)
                            {
                                alreadyUsed = true;
                                for(unsigned int i = 0; i < numFeatures; ++i)
                                {
                                    if((*iterData)[i] != (*iterCC).centre[i])
                                    {
                                        alreadyUsed = false;
                                    }
                                }
                                if(alreadyUsed)
                                {
                                    break;
                                }
                            }
                        }
                        else
                        {
                            alreadyUsed = false; 
                        }
                        
                        if(!alreadyUsed)
                        {
                            minDist = dist;
                            for(unsigned int i = 0; i < numFeatures; ++i)
                            {
                                cClosest[i] = (*iterData)[i];
                            }
                        }
                    }
                    
                }
            }
            
            if(first)
            {
                throw RSGISClustererException("All data points are already assigned to cluster centres. Not enough unique cluster centres.");
            }
            else
            {
                for(unsigned int i = 0; i < numFeatures; ++i)
                {
                    cc->centre[i] = cClosest[i];
                }
            }
        }
        catch (RSGISClustererException &e) 
        {
            throw e;
        }
    }
    


    RSGISKMeansClusterer::RSGISKMeansClusterer(InitClustererMethods initCentres)
    {
        this->initCentres = initCentres;
    }
        
    std::vector< RSGISClusterCentre >* RSGISKMeansClusterer::calcClusterCentres(std::vector< std::vector<float> > *input, unsigned int numFeatures, unsigned int numClusters, unsigned int maxNumIterations, float degreeOfChange)
    {
        std::vector< RSGISClusterCentre > *clusterCentres = NULL;
        try 
        {
            float *minVals = new float[numFeatures];
            float *maxVals = new float[numFeatures];
                       
            if(this->initCentres == init_random)
            {
                //this->calcDataRanges(input, numFeatures, minVals, maxVals);
                //clusterCentres = this->initializeClusterCentresRandom(numFeatures, minVals, maxVals, numClusters);
                clusterCentres = this->initializeClusterCentresRandom(input, numFeatures, numClusters);
            }
            else if(this->initCentres == init_diagonal_full)
            {
                this->calcDataRanges(input, numFeatures, minVals, maxVals);
                clusterCentres = this->initializeClusterCentresDiagonal(numFeatures, minVals, maxVals, numClusters);
            }
            else if(this->initCentres == init_diagonal_stddev)
            {
                float *meanVals = new float[numFeatures];
                float *stddevVals = new float[numFeatures];
                
                this->calcDataStats(input, numFeatures, minVals, maxVals, meanVals, stddevVals);
                clusterCentres = this->initializeClusterCentresDiagonal(numFeatures, minVals, maxVals, meanVals, stddevVals, numClusters);
                
                delete[] meanVals;
                delete[] stddevVals;
            }
            else if(this->initCentres == init_diagonal_full_attach)
            {
                this->calcDataRanges(input, numFeatures, minVals, maxVals);
                clusterCentres = this->initializeClusterCentresDiagonal(input, numFeatures, minVals, maxVals, numClusters);
            }
            else if(this->initCentres == init_diagonal_stddev_attach)
            {
                float *meanVals = new float[numFeatures];
                float *stddevVals = new float[numFeatures];
                
                this->calcDataStats(input, numFeatures, minVals, maxVals, meanVals, stddevVals);
                clusterCentres = this->initializeClusterCentresDiagonal(input, numFeatures, minVals, maxVals, meanVals, stddevVals, numClusters);
                
                delete[] meanVals;
                delete[] stddevVals;
            }
            else if(this->initCentres == init_kpp)
            {
                this->calcDataRanges(input, numFeatures, minVals, maxVals);
                clusterCentres = this->initializeClusterCentresKPP(input, numFeatures, minVals, maxVals, numClusters);
            }
            else
            {
                throw RSGISClustererException("Cluster initaliser was now recognised.");
            }
            
            delete[] minVals;
            delete[] maxVals;
            
            std::vector< std::pair< unsigned int, std::vector<float> > > *clusterData = this->createClusterDataInitClusterIDs(input, clusterCentres);
            
            unsigned int nIter = 0;
            unsigned int nChange = 0;
            float amountOfChange = 0;
            
            std::cout << "Starting Iterative processing...\n";
            bool contProcess = true;
            while(contProcess)
            {
                contProcess = false;
                
                this->recalcClusterCentres(clusterData, clusterCentres, false);
                
                nChange = this->reassignClusterIDs(clusterData, clusterCentres);
                
                amountOfChange = ((float)nChange)/clusterData->size();
                
                std::cout << "Iteration " << nIter << " has change " << amountOfChange*100 << " % of data clump IDs (" << clusterCentres->size() << " clusters).\n";
                
                if((nIter < maxNumIterations) & (amountOfChange > degreeOfChange))
                {
                    contProcess = true;
                }
                ++nIter;
            }            
        } 
        catch (RSGISClustererException &e) 
        {
            throw e;
        }
        
        return clusterCentres;
    }
    
    RSGISKMeansClusterer::~RSGISKMeansClusterer()
    {
        
    }

    
    
    
    

    RSGISISODataClusterer::RSGISISODataClusterer(InitClustererMethods initCentres, float minDistBetweenClusters, unsigned int minNumFeatures, float maxStdDev, unsigned int minNumClusters, unsigned int startIteration, unsigned int endIteration)
    {
        this->initCentres = initCentres;
        this->minDistBetweenClusters = minDistBetweenClusters;
        this->minNumFeatures = minNumFeatures;
        this->maxStdDev = maxStdDev;
        this->minNumClusters = minNumClusters;
        this->startIteration = startIteration;
        this->endIteration = endIteration;
    }
    
    std::vector< RSGISClusterCentre >* RSGISISODataClusterer::calcClusterCentres(std::vector< std::vector<float> > *input, unsigned int numFeatures, unsigned int numClusters, unsigned int maxNumIterations, float degreeOfChange)
    {
        std::vector< RSGISClusterCentre > *clusterCentres = NULL;
        try 
        {
            float *minVals = new float[numFeatures];
            float *maxVals = new float[numFeatures];
            
            if(this->initCentres == init_random)
            {
                //this->calcDataRanges(input, numFeatures, minVals, maxVals);
                //clusterCentres = this->initializeClusterCentresRandom(numFeatures, minVals, maxVals, numClusters);
                clusterCentres = this->initializeClusterCentresRandom(input, numFeatures, numClusters);
            }
            else if(this->initCentres == init_diagonal_full)
            {
                this->calcDataRanges(input, numFeatures, minVals, maxVals);
                clusterCentres = this->initializeClusterCentresDiagonal(numFeatures, minVals, maxVals, numClusters);
            }
            else if(this->initCentres == init_diagonal_stddev)
            {
                float *meanVals = new float[numFeatures];
                float *stddevVals = new float[numFeatures];
                
                this->calcDataStats(input, numFeatures, minVals, maxVals, meanVals, stddevVals);
                clusterCentres = this->initializeClusterCentresDiagonal(numFeatures, minVals, maxVals, numClusters);
                
                delete[] meanVals;
                delete[] stddevVals;
            }
            else if(this->initCentres == init_diagonal_full_attach)
            {
                this->calcDataRanges(input, numFeatures, minVals, maxVals);
                clusterCentres = this->initializeClusterCentresDiagonal(input, numFeatures, minVals, maxVals, numClusters);
            }
            else if(this->initCentres == init_diagonal_stddev_attach)
            {
                float *meanVals = new float[numFeatures];
                float *stddevVals = new float[numFeatures];
                
                this->calcDataStats(input, numFeatures, minVals, maxVals, meanVals, stddevVals);
                clusterCentres = this->initializeClusterCentresDiagonal(input, numFeatures, minVals, maxVals, meanVals, stddevVals, numClusters);
                
                delete[] meanVals;
                delete[] stddevVals;
            }
            else if(this->initCentres == init_kpp)
            {
                this->calcDataRanges(input, numFeatures, minVals, maxVals);
                clusterCentres = this->initializeClusterCentresKPP(input, numFeatures, minVals, maxVals, numClusters);
            }
            else
            {
                throw RSGISClustererException("Cluster initaliser was now recognised.");
            }
            
            delete[] minVals;
            delete[] maxVals;
            
            std::vector< std::pair< unsigned int, std::vector<float> > > *clusterData = this->createClusterDataInitClusterIDs(input, clusterCentres);
            
            unsigned int nIter = 0;
            unsigned int nChange = 0;
            float amountOfChange = 0;
            
            std::cout << "Starting Iterative processing...\n";
            bool contProcess = true;
            while(contProcess)
            {
                contProcess = false;
                
                this->recalcClusterCentres(clusterData, clusterCentres, true);
                
                if((nIter > this->startIteration) & (nIter < this->endIteration))
                {
                    this->addRemoveClusters(clusterCentres);
                }                
                
                nChange = this->reassignClusterIDs(clusterData, clusterCentres);
                
                amountOfChange = ((float)nChange)/clusterData->size();
                
                std::cout << "Iteration " << nIter << " has change " << amountOfChange*100 << " % of data clump IDs for " << clusterCentres->size() << " cluster centres.\n";
                
                if((nIter < maxNumIterations) & (amountOfChange > degreeOfChange))
                {
                    contProcess = true;
                }
                ++nIter;
            }
            
            std::cout << clusterCentres->size() << " Clusters were outputed\n";
        } 
        catch (RSGISClustererException &e) 
        {
            throw e;
        }
        
        return clusterCentres;
    }
    
    void RSGISISODataClusterer::addRemoveClusters(std::vector< RSGISClusterCentre > *clusterCentres)
    {
        if(clusterCentres->size() > this->minNumClusters)
        {
            // Remove any which are too small or too close to one another.
            float distance = 0;
            bool removed = false;
            bool sameCluster = false;
            for(std::vector< RSGISClusterCentre >::iterator iterClusters = clusterCentres->begin(); iterClusters != clusterCentres->end(); )
            {
                if((*iterClusters).numPxl < this->minNumFeatures)
                {
                    iterClusters = clusterCentres->erase(iterClusters);
                }
                else
                {
                    removed = false;
                    
                    for(std::vector< RSGISClusterCentre >::iterator iterClusters2 = clusterCentres->begin(); iterClusters2 != clusterCentres->end(); ++iterClusters2)
                    {
                        distance = 0;
                        sameCluster = true;
                        for(unsigned i = 0; i < (*iterClusters).centre.size(); ++i)
                        {
                            if((*iterClusters2).centre[i] != (*iterClusters).centre[i])
                            {
                                sameCluster = false;
                            }
                            distance += ((*iterClusters2).centre[i] - (*iterClusters).centre[i]) * ((*iterClusters2).centre[i] - (*iterClusters).centre[i]);
                        }
                        
                        if(!sameCluster)
                        {
                            distance = sqrt(distance);
                                                    
                            if(distance < this->minDistBetweenClusters)
                            {
                                removed = true;
                                iterClusters = clusterCentres->erase(iterClusters);
                                break;
                            }
                        }
                    }
                    
                    if(!removed)
                    {
                        ++iterClusters;
                    }
                }
            }
        }
        
        //Split any clusters where the std dev is too high (and there are sufficient number of features).
        std::vector< RSGISClusterCentre > newClusters;
        float meanStdDev = 0;
        for(std::vector< RSGISClusterCentre >::iterator iterClusters = clusterCentres->begin(); iterClusters != clusterCentres->end(); ++iterClusters)
        {
            meanStdDev = 0;
            for(unsigned i = 0; i < (*iterClusters).stdDev.size(); ++i)
            {
                meanStdDev += (*iterClusters).stdDev[i];
            }
            meanStdDev = meanStdDev/(*iterClusters).stdDev.size();
            
            if((meanStdDev > this->maxStdDev) && ((*iterClusters).numPxl > (this->minNumFeatures*2)))
            {
                RSGISClusterCentre cCentre;
                cCentre.centre.reserve((*iterClusters).centre.size());
                cCentre.stdDev.reserve((*iterClusters).stdDev.size());
                cCentre.numPxl = 0;
                for(unsigned i = 0; i < (*iterClusters).stdDev.size(); ++i)
                {
                    cCentre.centre.push_back((*iterClusters).centre[i] - (*iterClusters).stdDev[i]/2);
                    (*iterClusters).centre[i] = (*iterClusters).centre[i] + (*iterClusters).stdDev[i]/2;
                    cCentre.stdDev.push_back(0);
                }
                newClusters.push_back(cCentre);
            }
        }
        
        for(std::vector< RSGISClusterCentre >::iterator iterClusters = newClusters.begin(); iterClusters != newClusters.end(); ++iterClusters)
        {
            clusterCentres->push_back(*iterClusters);
        }
    }
    
    
    RSGISISODataClusterer::~RSGISISODataClusterer()
    {
        
    }
    
    
    
}}



