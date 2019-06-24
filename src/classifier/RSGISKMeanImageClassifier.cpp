/*
 *  RSGISKMeanImageClassifier.cpp
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

#include "RSGISKMeanImageClassifier.h"

namespace rsgis{ namespace classifier{
	
	RSGISKMeansClassifier::RSGISKMeansClassifier(std::string inputImageFile, bool printinfo): clusterCentres(NULL), numClusters(0), hasInitClusterCentres(false), datasets(NULL), numDatasets(0), printinfo(false)
	{
		this->inputImageFile = inputImageFile;
		this->printinfo = printinfo;
	}
		
	void RSGISKMeansClassifier::initClusterCentresKpp(unsigned int numClusters)
	{
		throw RSGISClassificationException("Implementation not complete!!");
		rsgis::math::RSGISVectors vecUtils;
		this->numClusters = numClusters;
		
		// Open Image
		GDALAllRegister();		
		try
		{
			this->numDatasets = 1;
			datasets = new GDALDataset*[1];
			std::cout << this->inputImageFile << std::endl;
			datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImageFile.c_str(), GA_ReadOnly);
			if(datasets[0] == NULL)
			{
				std::string message = std::string("Could not open image ") + this->inputImageFile;
				throw RSGISClassificationException(message.c_str());
			}
		}
		catch(RSGISClassificationException& e)
		{
			throw e;
		}
		
		// Calculate image band stats
		numImageBands = datasets[0]->GetRasterCount();
		
		rsgis::img::ImageStats **stats = new rsgis::img::ImageStats*[numImageBands];
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			stats[i] = new rsgis::img::ImageStats();
		}
		
		rsgis::img::RSGISImageStatistics calcImgStats;
		try 
		{
			calcImgStats.calcImageStatistics(datasets, 1, stats, numImageBands, true);
		}
		catch (rsgis::img::RSGISImageCalcException &e) 
		{
			for(unsigned int i = 0; i < numImageBands; ++i)
			{
				delete stats[i];
			}
			delete[] stats;
			
			throw RSGISClassificationException(e.what());
		}
		catch (rsgis::img::RSGISImageBandException &e) 
		{
			for(unsigned int i = 0; i < numImageBands; ++i)
			{
				delete stats[i];
			}
			delete[] stats;
			
			throw RSGISClassificationException(e.what());
		}
		
		
		// Allocate K cluster centers
		clusterCentres = new ClusterCentre*[numClusters];
		for(unsigned int i = 0; i < numClusters; ++i)
		{
			clusterCentres[i] = new ClusterCentre();
			clusterCentres[i]->classID = i;
			clusterCentres[i]->data = vecUtils.createVector(50);
		}

		
		// Calculate the initial cluster centres.
		
		// Select first centre at random
		boost::mt19937 randomGen;
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			boost::uniform_int<> randomDist(stats[i]->min, stats[i]->max);
			boost::variate_generator<boost::mt19937&, boost::uniform_int<> > randomVal(randomGen, randomDist);
			
			clusterCentres[0]->data->vector[i] = randomVal();
		}
		
		// Select the remaining cluster centres from a distribution weighted by D(x)^2
		RSGISCalcDist2NrCentreCalcImageVal *calcDist2Centres = new RSGISCalcDist2NrCentreCalcImageVal(0, this->clusterCentres, this->numClusters);
		rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(calcDist2Centres, "", true);
		double sumSqDist = 0;
		calcDist2Centres->setClusterCentres(clusterCentres, 1);
		calcImage->calcImage(datasets, numDatasets);
		sumSqDist = calcDist2Centres->getSumSqDistance();
		
		// Free stats from memory
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			delete stats[i];
		}
		delete[] stats;
		
		hasInitClusterCentres = true;
	}
	
	void RSGISKMeansClassifier::initClusterCentresRandom(unsigned int numClusters)
	{
        rsgis::math::RSGISVectors vecUtils;
		this->numClusters = numClusters;
		
		// Open Image
		GDALAllRegister();		
		try
		{
			this->numDatasets = 1;
			datasets = new GDALDataset*[1];
			std::cout << this->inputImageFile << std::endl;
			datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImageFile.c_str(), GA_ReadOnly);
			if(datasets[0] == NULL)
			{
				std::string message = std::string("Could not open image ") + this->inputImageFile;
				throw RSGISClassificationException(message.c_str());
			}
		}
		catch(RSGISClassificationException& e)
		{
			throw e;
		}
		
		// Calculate image band stats
		numImageBands = datasets[0]->GetRasterCount();
		
		rsgis::img::ImageStats **stats = new rsgis::img::ImageStats*[numImageBands];
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			stats[i] = new rsgis::img::ImageStats();
		}
		
		rsgis::img::RSGISImageStatistics calcImgStats;
		try 
		{
			calcImgStats.calcImageStatistics(datasets, 1, stats, numImageBands, false);
		}
		catch (rsgis::img::RSGISImageCalcException &e) 
		{
			for(unsigned int i = 0; i < numImageBands; ++i)
			{
				delete stats[i];
			}
			delete[] stats;
			
			throw RSGISClassificationException(e.what());
		}
		catch (rsgis::img::RSGISImageBandException &e) 
		{
			for(unsigned int i = 0; i < numImageBands; ++i)
			{
				delete stats[i];
			}
			delete[] stats;
			
			throw RSGISClassificationException(e.what());
		}
		
		
		// Allocate K cluster centers
		clusterCentres = new ClusterCentre*[numClusters];
		for(unsigned int i = 0; i < numClusters; ++i)
		{
			clusterCentres[i] = new ClusterCentre();
			clusterCentres[i]->classID = i;
			clusterCentres[i]->data = vecUtils.createVector(numImageBands);
		}
		
		// Define Cluster centres
		boost::mt19937 randomGen;
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			boost::uniform_int<> randomDist(stats[i]->min, stats[i]->max);
			boost::variate_generator<boost::mt19937&, boost::uniform_int<> > randomVal(randomGen, randomDist);
			
			for(unsigned int j = 0; j < numClusters; ++j)
			{
				clusterCentres[j]->data->vector[i] = randomVal();
			}
		}
		
		if(printinfo)
		{
			for(unsigned int i = 0; i < numClusters; ++i)
			{
				std::cout << "Cluster " << i << ": ";
				for(unsigned int j = 0; j < numImageBands; ++j)
				{
					std::cout << clusterCentres[i]->data->vector[j] << ", ";
				}
				std::cout << std::endl;
			}
		}
		
		// Free stats from memory
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			delete stats[i];
		}
		delete[] stats;
		
		hasInitClusterCentres = true;
	}
	
	void RSGISKMeansClassifier::calcClusterCentres(double terminalThreshold, unsigned int maxIterations, bool saveCentres, std::string outCentresFileName)
	{
		if(hasInitClusterCentres)
		{
			rsgis::math::RSGISVectors vecUtils;
			rsgis::math::RSGISMathsUtils mathsUtil;
			try 
			{
				RSGISKMeanCalcPixelClusterCalcImageVal *calcClusterCentre = new RSGISKMeanCalcPixelClusterCalcImageVal(0, this->clusterCentres, this->numClusters, this->numImageBands);
				rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(calcClusterCentre, "", true);
				
				ClusterCentre **newClusterCentres = NULL;
				unsigned long *numPxlsInCluster = NULL;
				double centreMoveDistanceSum = 0;
				double centreMoveDistance = 0;
				bool continueIterating = true;
				unsigned int iterNum = 0;
				
				while(continueIterating & (iterNum < maxIterations))
				{
					std::cout << "Iteration " << iterNum << ":\t" << std::flush;
					centreMoveDistanceSum = 0;
					centreMoveDistance = 0;
					
					// Identify new centres
					calcImage->calcImage(datasets, numDatasets);
					newClusterCentres = calcClusterCentre->getNewClusterCentres();
					numPxlsInCluster = calcClusterCentre->getPxlsInClusters();
					
					// Calculate distance between new and old centres.
					for(unsigned int i = 0; i < numClusters; ++i)
					{
						for(unsigned int j = 0; j < this->numImageBands; ++j)
						{
							if(numPxlsInCluster[i] == 0)
							{
								newClusterCentres[i]->data->vector[j] = clusterCentres[i]->data->vector[j];
							}
							else
							{
								newClusterCentres[i]->data->vector[j] = newClusterCentres[i]->data->vector[j] / numPxlsInCluster[i];
							}
						}
						centreMoveDistanceSum += vecUtils.euclideanDistance(newClusterCentres[i]->data, clusterCentres[i]->data);
					}
					centreMoveDistance = centreMoveDistanceSum/numClusters;
					
					// Replace old cluster centres within new centres
					for(unsigned int i = 0; i < numClusters; ++i)
					{
						for(unsigned int j = 0; j < this->numImageBands; ++j)
						{
							clusterCentres[i]->data->vector[j] = newClusterCentres[i]->data->vector[j];
						}
					}
					
					newClusterCentres = NULL;
					numPxlsInCluster = NULL;
					calcClusterCentre->reset();
					
					
					// Print cluster centres
					if(printinfo)
					{
						for(unsigned int i = 0; i < numClusters; ++i)
						{
							std::cout << "Cluster " << i << ": ";
							for(unsigned int j = 0; j < numImageBands; ++j)
							{
								std::cout << clusterCentres[i]->data->vector[j] << ", ";
							}
							std::cout << std::endl;
						}
					}
					std::cout << "Distance Moved = " << centreMoveDistance << std::endl;
					
					
					if(centreMoveDistance < terminalThreshold)
					{
						continueIterating = false;
					}
					
					++iterNum;
				}
				
				if(saveCentres)
				{
			        // Open text file
					std::ofstream outCentresFile;
					outCentresFile.open(outCentresFileName.c_str());

					// Write header file
					outCentresFile << "Cluster,";
					for(unsigned int j = 0; j < (numImageBands - 1); ++j)
					{
						std::string bandNumberStr = mathsUtil.inttostring(j + 1).c_str();
						outCentresFile << "b" + bandNumberStr << ",";
					}
					std::string bandNumberStr = mathsUtil.inttostring(numImageBands).c_str();
					outCentresFile << "b" + bandNumberStr;
					outCentresFile << std::endl;

					// Write out centres
					for(unsigned int i = 0; i < numClusters; ++i)
					{
						outCentresFile << i << ",";
						for(unsigned int j = 0; j < (numImageBands - 1); ++j)
						{
							outCentresFile << clusterCentres[i]->data->vector[j] << ",";
						}
						outCentresFile << clusterCentres[i]->data->vector[numImageBands-1];
						outCentresFile << std::endl;
					}
					outCentresFile.flush();
					outCentresFile.close();
				}
				
								
				delete calcClusterCentre;
				delete calcImage;
			}
			catch (rsgis::img::RSGISImageCalcException &e) 
			{
				throw RSGISClassificationException(e.what());
			}
		}
		else
		{
			throw RSGISClassificationException("The cluster centres have not been initialised.");
		}
	}
	
	void RSGISKMeansClassifier::generateOutputImage(std::string outputImageFile)
	{
		if(hasInitClusterCentres)
		{
			RSGISApplyKMeanClassifierCalcImageVal *applyClass = new RSGISApplyKMeanClassifierCalcImageVal(1, this->clusterCentres, this->numClusters);
			rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(applyClass, "", true);
			calcImage->calcImage(this->datasets, this->numDatasets, outputImageFile);
			
			delete applyClass;
			delete calcImage;
		}
		else
		{
			throw RSGISClassificationException("The cluster centres have not been initialised.");
		}
	}
	
	RSGISKMeansClassifier::~RSGISKMeansClassifier()
	{
		if(hasInitClusterCentres)
		{
			rsgis::math::RSGISVectors vecUtils;
			for(unsigned int i = 0; i < numClusters; ++i)
			{
				vecUtils.freeVector(clusterCentres[i]->data);
				delete clusterCentres[i];
			}
			delete[] clusterCentres;
			
			for(unsigned int i = 0; i < numDatasets; ++i)
			{
				GDALClose(datasets[i]);
			}
			delete[] datasets;
		}
	}

	
	RSGISKMeanCalcPixelClusterCalcImageVal::RSGISKMeanCalcPixelClusterCalcImageVal(int numOutBands, ClusterCentre **clusterCentres, unsigned int numClusters, unsigned int numImageBands) : RSGISCalcImageValue(numOutBands)
	{
		this->clusterCentres = clusterCentres;
		this->numClusters = numClusters;
		this->numImageBands = numImageBands;
		
		rsgis::math::RSGISVectors vecUtils;
		newClusterCentres = new ClusterCentre*[numClusters];
		numPxlInClusters = new unsigned long[numClusters];
		for(unsigned int i = 0; i < numClusters; ++i)
		{
			newClusterCentres[i] = new ClusterCentre();
			newClusterCentres[i]->classID = clusterCentres[i]->classID;
			newClusterCentres[i]->data = vecUtils.createVector(numImageBands);
			
			numPxlInClusters[i] = 0;
		}
		
	}
	
	void RSGISKMeanCalcPixelClusterCalcImageVal::calcImageValue(float *bandValues, int numBands) 
	{
		// Identify cluster within which point is associated with
		double minDistance = 0;
		unsigned int minIdx = 0;
		bool first = true;
		double sum = 0;
		double distance = 0;
		for(unsigned int i = 0; i < numClusters; ++i)
		{
			sum = 0;
			for(int j = 0; j < numBands; ++j)
			{
				sum += ((clusterCentres[i]->data->vector[j] - bandValues[j])*(clusterCentres[i]->data->vector[j] - bandValues[j]));
			}
			distance = sum/numBands;
			
			if(first)
			{
				minDistance = distance;
				minIdx = i;
				first = false;
			}
			else if(distance < minDistance)
			{
				minDistance = distance;
				minIdx = i;
			}
		}
		
		// add to sum for next centre
		for(int i = 0; i < numBands; ++i)
		{
			newClusterCentres[minIdx]->data->vector[i] += bandValues[i];
		}
		++numPxlInClusters[minIdx];
		
	}
	
	unsigned long* RSGISKMeanCalcPixelClusterCalcImageVal::getPxlsInClusters()
	{
		return numPxlInClusters;
	}
	
	ClusterCentre** RSGISKMeanCalcPixelClusterCalcImageVal::getNewClusterCentres()
	{
		return newClusterCentres;
	}
	
	void RSGISKMeanCalcPixelClusterCalcImageVal::reset()
	{
		for(unsigned int i = 0; i < numClusters; ++i)
		{
			for(unsigned int j = 0; j < numImageBands; ++j)
			{
				newClusterCentres[i]->data->vector[j] = 0;
			}
			numPxlInClusters[i] = 0;
		}
	}
	
	RSGISKMeanCalcPixelClusterCalcImageVal::~RSGISKMeanCalcPixelClusterCalcImageVal()
	{
		rsgis::math::RSGISVectors vecUtils;
		for(unsigned int i = 0; i < numClusters; ++i)
		{
			vecUtils.freeVector(newClusterCentres[i]->data);
			delete newClusterCentres[i];
		}
		delete[] newClusterCentres;
		
		delete[] numPxlInClusters;
	}
	
	RSGISCalcDist2NrCentreCalcImageVal::RSGISCalcDist2NrCentreCalcImageVal(int numOutBands, ClusterCentre **clusterCentres, unsigned int numClusters) : RSGISCalcImageValue(numOutBands)
	{
		this->clusterCentres = clusterCentres;
		this->numClusters = numClusters;
		sumSqDistance = 0;
	}
	
	void RSGISCalcDist2NrCentreCalcImageVal::calcImageValue(float *bandValues, int numBands) 
	{
		double minDistance = 0;
		unsigned int minIdx = 0;
		bool first = true;
		double sum = 0;
		double distance = 0;
		for(unsigned int i = 0; i < numClusters; ++i)
		{
			sum = 0;
			for(int j = 0; j < numBands; ++j)
			{
				sum += ((clusterCentres[i]->data->vector[j] - bandValues[j])*(clusterCentres[i]->data->vector[j] - bandValues[j]));
			}
			distance = sum/numBands;
			
			if(first)
			{
				minDistance = distance;
				minIdx = i;
				first = false;
			}
			else if(distance < minDistance)
			{
				minDistance = distance;
				minIdx = i;
			}
		}
		
		sumSqDistance += minDistance;
	}
	
	double RSGISCalcDist2NrCentreCalcImageVal::getSumSqDistance()
	{
		return sumSqDistance;
	}
	
	void RSGISCalcDist2NrCentreCalcImageVal::reset()
	{
		sumSqDistance = 0;
	}
	
	void RSGISCalcDist2NrCentreCalcImageVal::setClusterCentres(ClusterCentre **clusterCentres, unsigned int numClusters)
	{
		this->clusterCentres = clusterCentres;
		this->numClusters = numClusters;
	}
	
	RSGISCalcDist2NrCentreCalcImageVal::~RSGISCalcDist2NrCentreCalcImageVal()
	{
		
	}
	
	
	
	RSGISCalcDist2NrCentreDistributionCalcImageVal::RSGISCalcDist2NrCentreDistributionCalcImageVal(int numOutBands, ClusterCentre **clusterCentres, unsigned int numClusters, double sqDistance) : RSGISCalcImageValue(numOutBands)
	{
		this->clusterCentres = clusterCentres;
		this->numClusters = numClusters;
		this->sqDistance = sqDistance;
	}
	
	void RSGISCalcDist2NrCentreDistributionCalcImageVal::calcImageValue(float *bandValues, int numBands) 
	{
		double probX = 0;
		double minDistance = 0;
		unsigned int minIdx = 0;
		bool first = true;
		double sum = 0;
		double distance = 0;
		for(unsigned int i = 0; i < numClusters; ++i)
		{
			sum = 0;
			for(int j = 0; j < numBands; ++j)
			{
				sum += ((clusterCentres[i]->data->vector[j] - bandValues[j])*(clusterCentres[i]->data->vector[j] - bandValues[j]));
			}
			distance = sum/numBands;
			
			if(first)
			{
				minDistance = distance;
				minIdx = i;
				first = false;
			}
			else if(distance < minDistance)
			{
				minDistance = distance;
				minIdx = i;
			}
		}
		
		probX = minDistance/sqDistance;
		
		//RSGISExportForPlotting::getInstance()
	}

	void RSGISCalcDist2NrCentreDistributionCalcImageVal::setClusterCentres(ClusterCentre **clusterCentres, unsigned int numClusters, double sqDistance)
	{
		this->clusterCentres = clusterCentres;
		this->numClusters = numClusters;
		this->sqDistance = sqDistance;
	}
	
	RSGISCalcDist2NrCentreDistributionCalcImageVal::~RSGISCalcDist2NrCentreDistributionCalcImageVal()
	{
		
	}
	
	
	
	
	RSGISApplyKMeanClassifierCalcImageVal::RSGISApplyKMeanClassifierCalcImageVal(int numOutBands, ClusterCentre **clusterCentres, unsigned int numClusters) : RSGISCalcImageValue(numOutBands)
	{
		this->clusterCentres = clusterCentres;
		this->numClusters = numClusters;
	}
	
	void RSGISApplyKMeanClassifierCalcImageVal::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		double minDistance = 0;
		unsigned int minIdx = 0;
		bool first = true;
		double sum = 0;
		double distance = 0;
		for(unsigned int i = 0; i < numClusters; ++i)
		{
			sum = 0;
			for(int j = 0; j < numBands; ++j)
			{
				sum += ((clusterCentres[i]->data->vector[j] - bandValues[j])*(clusterCentres[i]->data->vector[j] - bandValues[j]));
			}
			distance = sum/numBands;
			
			if(first)
			{
				minDistance = distance;
				minIdx = i;
				first = false;
			}
			else if(distance < minDistance)
			{
				minDistance = distance;
				minIdx = i;
			}
		}
		output[0] = minIdx;
	}
	
	RSGISApplyKMeanClassifierCalcImageVal::~RSGISApplyKMeanClassifierCalcImageVal()
	{
		
	}
}}
