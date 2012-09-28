/*
 *  RSGISImageStatistics.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/05/2008.
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

#include "RSGISImageStatistics.h"

namespace rsgis{namespace img{
	
	RSGISImageStatistics::RSGISImageStatistics()
	{
		
	}
	
	void RSGISImageStatistics::calcImageStatistics(GDALDataset **datasets, int numDS, ImageStats **stats, int numInputBands, bool stddev, bool ignoreZeros, bool onePassSD)throw(RSGISImageCalcException,RSGISImageBandException)
	{
		RSGISCalcImageStatistics *calcImageStats = NULL;
		RSGISCalcImage *calcImg = NULL;
		try
		{
			calcImageStats = new RSGISCalcImageStatistics(0, numInputBands, false, NULL, ignoreZeros, onePassSD);
			calcImg = new RSGISCalcImage(calcImageStats, "", true);
			
            if(stddev && onePassSD){calcImageStats->calcStdDev();}
            
            calcImg->calcImage(datasets, numDS);
			
			if(stddev && !onePassSD)
			{
				calcImageStats->calcStdDev();
                calcImg->calcImage(datasets, numDS);
			}
			
			calcImageStats->getImageStats(stats, numInputBands);
		}
		catch(RSGISImageCalcException e)
		{
			if(calcImageStats != NULL)
			{
				delete calcImageStats;
			}
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			if(calcImageStats != NULL)
			{
				delete calcImageStats;
			}
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			throw e;
		}
		
		if(calcImageStats != NULL)
		{
			delete calcImageStats;
		}
		if(calcImg != NULL)
		{
			delete calcImg;
		}
	}
	
	void RSGISImageStatistics::calcImageStatistics(GDALDataset **datasets, int numDS, ImageStats **stats, int numInputBands, bool stddev, rsgis::math::RSGISMathFunction *func, bool ignoreZeros, bool onePassSD)throw(RSGISImageCalcException,RSGISImageBandException)
	{
		RSGISCalcImageStatistics *calcImageStats = NULL;
		RSGISCalcImage *calcImg = NULL;
		try
		{
			calcImageStats = new RSGISCalcImageStatistics(0, numInputBands, false, func, ignoreZeros);
			calcImg = new RSGISCalcImage(calcImageStats, "", true);
            if(stddev && onePassSD){calcImageStats->calcStdDev();}
            
            calcImg->calcImage(datasets, numDS);
			
			if(stddev && !onePassSD)
			{
				calcImageStats->calcStdDev();
                calcImg->calcImage(datasets, numDS);
			}
			
			calcImageStats->getImageStats(stats, numInputBands);
		}
		catch(RSGISImageCalcException e)
		{
			if(calcImageStats != NULL)
			{
				delete calcImageStats;
			}
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			if(calcImageStats != NULL)
			{
				delete calcImageStats;
			}
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			throw e;
		}
		
		if(calcImageStats != NULL)
		{
			delete calcImageStats;
		}
		if(calcImg != NULL)
		{
			delete calcImg;
		}
	}
    
    void RSGISImageStatistics::calcImageStatistics(GDALDataset **datasets, int numDS, ImageStats *stats, bool stddev, bool ignoreZeros, bool onePassSD)throw(RSGISImageCalcException,RSGISImageBandException)
	{
		RSGISCalcImageStatisticsAllBands *calcImageStats = NULL;
		RSGISCalcImage *calcImg = NULL;
		try
		{
			calcImageStats = new RSGISCalcImageStatisticsAllBands(0, false, NULL, ignoreZeros);
			calcImg = new RSGISCalcImage(calcImageStats, "", true);
            if(stddev && onePassSD){calcImageStats->calcStdDev();}
            
            calcImg->calcImage(datasets, numDS);
			
			if(stddev && !onePassSD)
			{
				calcImageStats->calcStdDev();
                calcImg->calcImage(datasets, numDS);
			}
			
			calcImageStats->getImageStats(stats);
		}
		catch(RSGISImageCalcException e)
		{
			if(calcImageStats != NULL)
			{
				delete calcImageStats;
			}
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			if(calcImageStats != NULL)
			{
				delete calcImageStats;
			}
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			throw e;
		}
		
		if(calcImageStats != NULL)
		{
			delete calcImageStats;
		}
		if(calcImg != NULL)
		{
			delete calcImg;
		}
	}
	
    
    
    
    
    
	RSGISCalcImageStatistics::RSGISCalcImageStatistics(int numberOutBands, int numInputBands, bool calcSD, rsgis::math::RSGISMathFunction *func, bool ignoreZeros, bool onePassSD) : RSGISCalcImageValue(numberOutBands)
	{
        this->ignoreZeros = ignoreZeros;
        this->onePassSD = onePassSD;
		this->calcSD = calcSD;
		this->numInputBands = numInputBands;
		this->calcMean = false;
		this->mean = new double[numInputBands];
		this->meanSum = new double[numInputBands];
        this->sumSq = new double[numInputBands];
		this->min = new double[numInputBands];
		this->max = new double[numInputBands];
		this->sumDiffZ = new double[numInputBands];
		this->diffZ = 0;
		this->n = new unsigned long[numInputBands];
        this->firstMean = new bool[numInputBands];
        this->firstSD = new bool[numInputBands];
        for(int i = 0; i < numInputBands; ++i)
        {
            mean[i] = 0;
            meanSum[i] = 0;
            sumSq[i] = 0;
            min[i] = 0;
            max[i] = 0;
            sumDiffZ[i] = 0;
            n[i] = 0;
            firstMean[i] = true;
            firstSD[i] = true;
        }
		this->func = func;
	}
	
	void RSGISCalcImageStatistics::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		if(numBands != this->numInputBands)
		{
			throw RSGISImageCalcException("The number input bands needs to match the figure provided.");
		}
		
		bool foundNan = false;
        bool allBandsZero = true;
		
		for(int i = 0; i < numBands; i++)
		{
			if(boost::math::isnan(bandValues[i]))
			{
				foundNan = true;
			}
            if(bandValues[i] != 0)
            {
                allBandsZero = false;
            }
		}
		
		if(!foundNan | !(ignoreZeros & allBandsZero))
		{
			if(func != NULL)
			{
				for(int i = 0; i < numBands; i++)
				{
					bandValues[i] = func->calcFunction(bandValues[i]);
				}
			}
			
            if(!calcSD || (calcSD && onePassSD)) // If not calculating SD or calculating SD with a single pass calculate mean, min + max
			{
				calcMean = true;
                
                for(int i = 0; i < numBands; i++)
                {
                    if(firstMean[i])
                    {
                        if(!(ignoreZeros & (bandValues[i] == 0)))
                        {
                            meanSum[i] = bandValues[i];
                            min[i] = bandValues[i];
                            max[i] = bandValues[i];
                            ++n[i];
                            firstMean[i] = false;
                        }
                    }
                    else
                    {
                        if(!(ignoreZeros & (bandValues[i] == 0)))
                        {
                            meanSum[i] = meanSum[i] + bandValues[i];
                            if(bandValues[i] < min[i])
                            {
                                min[i] = bandValues[i];
                            }
                            if(bandValues[i] > max[i])
                            {
                                max[i] = bandValues[i];
                            }
                            ++n[i];
                        }
                    }
                    
                    if(calcSD && onePassSD)
                    {                        
                        sumSq[i] = sumSq[i] + bandValues[i]*bandValues[i];
                    }
                        
                }
			}
            
			else
			{
				if(!calcMean)
				{
					throw RSGISImageCalcException("The standard deviation cannot be calculated before the mean.");
				}
                
                for(int i = 0; i < numBands; i++)
                {
                    if(firstSD[i])
                    {
                        if(!(ignoreZeros & (bandValues[i] == 0)))
                        {
                            mean[i] = meanSum[i]/n[i];
                            diffZ = mean[i] - bandValues[i];
                            sumDiffZ[i] = (diffZ * diffZ);
                            firstSD[i] = false;
                        } 
                    }
                    else
                    {
                        if(!(ignoreZeros & (bandValues[i] == 0)))
                        {
                            diffZ = mean[i] - bandValues[i];
                            sumDiffZ[i] = sumDiffZ[i] + (diffZ * diffZ);
                        }
                    }
                }
			}
		}
	}
	
	void RSGISCalcImageStatistics::getImageStats(ImageStats** inStats, int numInputBands) throw(RSGISImageCalcException)
	{
		if(this->numInputBands != numInputBands)
		{
			throw RSGISImageCalcException("The number of stats objects passed to the function need to the be same as those defined in the class");
		}
		
		for(int i = 0; i < numInputBands; i++)
		{
			inStats[i]->mean = meanSum[i]/n[i];
			inStats[i]->min = min[i];
			inStats[i]->max = max[i];
            inStats[i]->sum = meanSum[i];
            if(calcSD)
            {
                if (onePassSD)
                {
                    double var = (sumSq[i] - ( (meanSum[i]*meanSum[i]) /n[i] ) )/ n[i];
                    inStats[i]->stddev = sqrt(var);
                }
                else
                {
                    inStats[i]->stddev = sqrt(sumDiffZ[i]/n[i]);
                }
            }
            else
            {
                inStats[i]->stddev = 0;
            }
		}
	}
	
	void RSGISCalcImageStatistics::calcStdDev()
	{
		calcSD = true;
	}
	
	RSGISCalcImageStatistics::~RSGISCalcImageStatistics()
	{
		delete[] mean;
		delete[] meanSum;
		delete[] min;
		delete[] max;
		delete[] sumDiffZ;
        delete[] n;
        delete[] firstMean;
		delete[] firstSD;
	}
    
    
    
    
    
    
    
    RSGISCalcImageStatisticsAllBands::RSGISCalcImageStatisticsAllBands(int numberOutBands, bool calcSD, rsgis::math::RSGISMathFunction *func, bool ignoreZeros) : RSGISCalcImageValue(numberOutBands)
	{
        this->ignoreZeros = ignoreZeros;
		this->calcSD = calcSD;
		this->calcMean = false;
		this->mean = 0;
		this->meanSum = 0;
		this->min = 0;
		this->max = 0;
		this->sumDiffZ = 0;
		this->diffZ = 0;
		this->n = 0;
        this->firstMean = true;
        this->firstSD = true;
		this->func = func;
	}
	
	void RSGISCalcImageStatisticsAllBands::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		
		bool foundNan = false;
        bool allBandsZero = true;
		
		for(int i = 0; i < numBands; i++)
		{
			if(boost::math::isnan(bandValues[i]))
			{
				foundNan = true;
			}
            if(bandValues[i] != 0)
            {
                allBandsZero = false;
            }
		}
		
		if(!foundNan | !(ignoreZeros & allBandsZero))
		{
			if(func != NULL)
			{
				for(int i = 0; i < numBands; i++)
				{
					bandValues[i] = func->calcFunction(bandValues[i]);
				}
			}
			
			
			if(calcSD)
			{
				if(!calcMean)
				{
					throw RSGISImageCalcException("The standard deviation cannot be calculated before the mean.");
				}
                
                for(int i = 0; i < numBands; i++)
                {
                    if(firstSD)
                    {
                        if(!(ignoreZeros & (bandValues[i] == 0)))
                        {
                            mean = meanSum/n;
                            diffZ = mean - bandValues[i];
                            sumDiffZ = (diffZ * diffZ);
                            firstSD = false;
                        } 
                    }
                    else
                    {
                        if(!(ignoreZeros & (bandValues[i] == 0)))
                        {
                            diffZ = mean - bandValues[i];
                            sumDiffZ = sumDiffZ + (diffZ * diffZ);
                        }
                    }
                }
			}
			else
			{
				calcMean = true;
                
                for(int i = 0; i < numBands; i++)
                {
                    if(firstMean)
                    {
                        if(!(ignoreZeros & (bandValues[i] == 0)))
                        {
                            meanSum = bandValues[i];
                            min = bandValues[i];
                            max = bandValues[i];
                            ++n;
                            firstMean = false;
                        } 
                    }
                    else
                    {
                        if(!(ignoreZeros & (bandValues[i] == 0)))
                        {
                            meanSum = meanSum + bandValues[i];
                            if(bandValues[i] < min)
                            {
                                min = bandValues[i];
                            }
                            if(bandValues[i] > max)
                            {
                                max = bandValues[i];
                            }
                            ++n;
                        }
                    }
                }
			}
		}
	}
	
    
	void RSGISCalcImageStatisticsAllBands::getImageStats(ImageStats *inStats) throw(RSGISImageCalcException)
	{
		inStats->mean = meanSum/n;
        inStats->min = min;
        inStats->max = max;
        inStats->sum = meanSum;
        if(calcSD)
        {
            inStats->stddev = sqrt(sumDiffZ/n);
        }
        else
        {
            inStats->stddev = 0;
        }
	}
	
	void RSGISCalcImageStatisticsAllBands::calcStdDev()
	{
		calcSD = true;
	}
	
	RSGISCalcImageStatisticsAllBands::~RSGISCalcImageStatisticsAllBands()
	{
		
	}
	
}}
