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
	
    void RSGISImageStatistics::calcImageStatistics(GDALDataset **datasets, int numDS, ImageStats **stats, int numInputBands, bool stddev, bool noDataSpecified, float noDataVal, bool onePassSD, double xMin, double xMax, double yMin, double yMax)throw(RSGISImageCalcException,RSGISImageBandException)
    {
        RSGISCalcImageStatisticsNoData *calcImageStats = NULL;
		RSGISCalcImage *calcImg = NULL;
		try
		{
            geos::geom::Envelope *env = new geos::geom::Envelope(xMin, xMax, yMin, yMax);
            
			calcImageStats = new RSGISCalcImageStatisticsNoData(numInputBands, false, NULL, noDataSpecified, noDataVal, onePassSD);
			calcImg = new RSGISCalcImage(calcImageStats, "", true);
			
            if(stddev && onePassSD)
            {
                calcImageStats->calcStdDev();
            }
            
            calcImg->calcImageInEnv(datasets, numDS, env);
			
			if(stddev && !onePassSD)
			{
				calcImageStats->calcStdDev();
                calcImg->calcImageInEnv(datasets, numDS, env);
			}
			
			calcImageStats->getImageStats(stats, numInputBands);
            
            delete env;
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
    
    void RSGISImageStatistics::calcImageHistogram(GDALDataset **datasets, int numDS, unsigned int imgBand, unsigned int numBins, float *binRanges, unsigned int *binCounts, bool noDataSpecified, float noDataVal, double xMin, double xMax, double yMin, double yMax)throw(RSGISImageCalcException,RSGISImageBandException)
    {
        RSGISCalcImageHistogramNoData *calcImageStats = NULL;
		RSGISCalcImage *calcImg = NULL;
		try
		{
            geos::geom::Envelope *env = new geos::geom::Envelope(xMin, xMax, yMin, yMax);
            
			calcImageStats = new RSGISCalcImageHistogramNoData(imgBand, noDataSpecified, noDataVal, numBins, binRanges, binCounts);
			calcImg = new RSGISCalcImage(calcImageStats, "", true);
			
            calcImg->calcImageInEnv(datasets, numDS, env);

            delete env;
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
    
    void RSGISImageStatistics::calcImageStatisticsMask(GDALDataset *dataset, GDALDataset *imgMask, long maskVal, ImageStats **stats, double *noDataVals, bool useNoData, int numInputBands, bool stddev, bool onePassSD)throw(RSGISImageCalcException,RSGISImageBandException)
    {
        GDALDataset **datasets = new GDALDataset*[2];
        datasets[0] = imgMask;
        datasets[1] = dataset;

        try
        {
            RSGISCalcImageStatisticsMaskStatsNoData calcImageStats = RSGISCalcImageStatisticsMaskStatsNoData(0, numInputBands, maskVal, noDataVals, useNoData, false, onePassSD);
            RSGISCalcImage calcImg = RSGISCalcImage(&calcImageStats, "", true);
            
            if(stddev && onePassSD)
            {
                calcImageStats.calcStdDev();
            }
            calcImg.calcImage(datasets, 1, 1);
            
            if(stddev && !onePassSD)
            {
                calcImageStats.calcStdDev();
                calcImg.calcImage(datasets, 1, 1);
            }
            calcImageStats.getImageStats(stats, numInputBands);
            delete[] datasets;
        }
        catch(RSGISImageCalcException e)
        {
            throw e;
        }
        catch(RSGISImageBandException e)
        {
            throw e;
        }
    }
    
    void RSGISImageStatistics::calcImageBandStatistics(GDALDataset *dataset, int imgBand, ImageStats *stats, bool stddev, bool useNoData, float noDataVal, bool onePassSD)throw(RSGISImageCalcException,RSGISImageBandException)
    {
        try
        {
            int numBands = dataset->GetRasterCount();
            
            if((imgBand < 1) | (imgBand > numBands))
            {
                throw RSGISImageBandException("The specified image band is not within the image.");
            }
            
            RSGISCalcImageStatisticsNoData calcImageStats = RSGISCalcImageStatisticsNoData(numBands, false, NULL, useNoData, noDataVal, onePassSD);
            RSGISCalcImage calcImg = RSGISCalcImage(&calcImageStats, "", true);
            
            if(stddev && onePassSD)
            {
                calcImageStats.calcStdDev();
            }
            
            calcImg.calcImage(&dataset, 1);
            
            if(stddev && !onePassSD)
            {
                calcImageStats.calcStdDev();
                calcImg.calcImage(&dataset, 1);
            }
            
            ImageStats **bandStats = new ImageStats*[numBands];
            for(int i = 0; i < numBands; ++i)
            {
                bandStats[i] = new ImageStats();
                bandStats[i]->mean = 0;
                bandStats[i]->max = 0;
                bandStats[i]->min = 0;
                bandStats[i]->stddev = 0;
                bandStats[i]->sum = 0;
            }
            calcImageStats.getImageStats(bandStats, numBands);
            
            stats->mean = bandStats[imgBand-1]->mean;
            stats->max = bandStats[imgBand-1]->max;
            stats->min = bandStats[imgBand-1]->min;
            stats->stddev = bandStats[imgBand-1]->stddev;
            stats->sum = bandStats[imgBand-1]->sum;
            
            for(int i = 0; i < numBands; ++i)
            {
                delete bandStats[i];
            }
            delete[] bandStats;
        }
        catch(RSGISImageCalcException e)
        {
            throw e;
        }
        catch(RSGISImageBandException e)
        {
            throw e;
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
        
        if(!foundNan)
        {
            if(!(ignoreZeros & allBandsZero))
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
    
    
    
    
    
    RSGISCalcImageStatisticsNoData::RSGISCalcImageStatisticsNoData(int numInputBands, bool calcSD, rsgis::math::RSGISMathFunction *func, bool noDataSpecified, float noDataVal, bool onePassSD) : RSGISCalcImageValue(0)
	{
        this->noDataSpecified = noDataSpecified;
        this->noDataVal = noDataVal;
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
	
	void RSGISCalcImageStatisticsNoData::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		if(numBands != this->numInputBands)
		{
			throw RSGISImageCalcException("The number input bands needs to match the figure provided.");
		}
		
		bool foundNan = false;
        bool allBandsNoData = true;
		
		for(int i = 0; i < numBands; i++)
		{
			if(boost::math::isnan(bandValues[i]))
			{
				foundNan = true;
			}
            if(bandValues[i] != noDataVal)
            {
                allBandsNoData = false;
            }
		}
        
        if(!foundNan)
        {
            if(!(noDataSpecified & allBandsNoData))
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
                            if(!(noDataSpecified & (bandValues[i] == noDataVal)))
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
                            if(!(noDataSpecified & (bandValues[i] == noDataVal)))
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
                            if(!(noDataSpecified & (bandValues[i] == noDataVal)))
                            {
                                mean[i] = meanSum[i]/n[i];
                                diffZ = mean[i] - bandValues[i];
                                sumDiffZ[i] = (diffZ * diffZ);
                                firstSD[i] = false;
                            }
                        }
                        else
                        {
                            if(!(noDataSpecified & (bandValues[i] == noDataVal)))
                            {
                                diffZ = mean[i] - bandValues[i];
                                sumDiffZ[i] = sumDiffZ[i] + (diffZ * diffZ);
                            }
                        }
                    }
                }
            }
        }
	}
	
	void RSGISCalcImageStatisticsNoData::getImageStats(ImageStats** inStats, int numInputBands) throw(RSGISImageCalcException)
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
	
	void RSGISCalcImageStatisticsNoData::calcStdDev()
	{
		calcSD = true;
	}
	
	RSGISCalcImageStatisticsNoData::~RSGISCalcImageStatisticsNoData()
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
		
        if(!foundNan)
        {
            if(!(ignoreZeros & allBandsZero))
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
    
    
    
    RSGISImagePercentiles::RSGISImagePercentiles()
    {
        
    }
    
    rsgis::math::Matrix* RSGISImagePercentiles::getPercentilesForAllBands(GDALDataset* dataset, float percentile, float noDataVal, bool noDataDefined)throw(rsgis::RSGISImageException)
    {
        rsgis::math::RSGISMatrices matrixUtils;
        rsgis::math::Matrix *outPercentiles = NULL;
        try
        {
            unsigned numImageBands = dataset->GetRasterCount();
            outPercentiles = matrixUtils.createMatrix(numImageBands, 1);
            for(unsigned int n = 0; n < numImageBands; ++n)
            {
                std::cout << "\tCalculating Percentile " << percentile << " of band " << n+1 << std::flush;
                outPercentiles->matrix[n] = this->getPercentile(dataset, n+1, percentile, noDataVal, noDataDefined);
                std::cout << " = " << outPercentiles->matrix[n] << std::endl;
            }
        }
        catch (rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        
        return outPercentiles;
    }
    
    double RSGISImagePercentiles::getPercentile(GDALDataset *dataset, unsigned int band, float percentile, float noDataVal, bool noDataDefined)throw(rsgis::RSGISImageException)
    {
        double percentileVal = 0.0;
        try
        {
            RSGISImageUtils imageUtils;
            std::vector<double> *dataVals = imageUtils.getImageBandValues(dataset, band, noDataDefined, noDataVal);
            std::sort(dataVals->begin(), dataVals->end());            
            percentileVal = gsl_stats_quantile_from_sorted_data(&(*dataVals)[0], 1, dataVals->size(), percentile);
            delete dataVals;
        }
        catch (rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        
        return percentileVal;
    }
    
    double RSGISImagePercentiles::getPercentile(GDALDataset *dataset, unsigned int band, GDALDataset *maskDS, int maskVal, float percentile, float noDataVal, bool noDataDefined)throw(rsgis::RSGISImageException)
    {
        double percentileVal = 0.0;
        try
        {
            std::vector<double> *dataVals = new std::vector<double>();
            
            RSGISGetPixelBandValues calcGetBandVals = RSGISGetPixelBandValues(dataVals, band, maskVal, noDataVal, noDataDefined);
            RSGISCalcImage calcImg = RSGISCalcImage(&calcGetBandVals, "", true);
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = maskDS;
            datasets[1] = dataset;
            calcImg.calcImage(datasets, 1, 1);
            
            std::sort(dataVals->begin(), dataVals->end());
            percentileVal = gsl_stats_quantile_from_sorted_data(&(*dataVals)[0], 1, dataVals->size(), percentile);
            delete dataVals;
        }
        catch (rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        
        return percentileVal;
    }
    
    double RSGISImagePercentiles::getPercentile(GDALDataset *dataset, unsigned int band, GDALDataset *maskDS, int maskVal, float percentile, float noDataVal, bool noDataDefined, geos::geom::Envelope *env, bool quiet)throw(rsgis::RSGISImageException)
    {
        double percentileVal = 0.0;
        try
        {
            std::vector<double> *dataVals = new std::vector<double>();
            
            RSGISGetPixelBandValues calcGetBandVals = RSGISGetPixelBandValues(dataVals, band, maskVal, noDataVal, noDataDefined);
            RSGISCalcImage calcImg = RSGISCalcImage(&calcGetBandVals, "", true);
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = maskDS;
            datasets[1] = dataset;
            calcImg.calcImageInEnv(datasets, 1, 1, env, quiet);
            
            std::sort(dataVals->begin(), dataVals->end());
            percentileVal = gsl_stats_quantile_from_sorted_data(&(*dataVals)[0], 1, dataVals->size(), percentile);
            delete dataVals;
        }
        catch (rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        
        return percentileVal;
    }
    
    RSGISImagePercentiles::~RSGISImagePercentiles()
    {
        
    }
	
    
    
    void RSGISGetPixelBandValues::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException)
    {
        if(numIntVals != 1)
        {
            throw RSGISImageCalcException("Mask image should only have 1 band.");
        }
        
        if(intBandValues[0] == this->maskVal)
        {
            if((this->noDataDefined) && (floatBandValues[band-1] != this->noDataVal))
            {
                this->dataVals->push_back(floatBandValues[band-1]);
            }
            else
            {
                this->dataVals->push_back(floatBandValues[band-1]);
            }
        }
    }
    
    
    
    

    RSGISImagePixelSummaries::RSGISImagePixelSummaries(unsigned int numOutBands, rsgis::math::RSGISStatsSummary *statsSummary, float noDataValue, bool useNoDataValue) : RSGISCalcImageValue(numOutBands)
    {
        this->statsSummary = statsSummary;
        this->noDataValue = noDataValue;
        this->useNoDataValue = useNoDataValue;
    }
    
    void RSGISImagePixelSummaries::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
    {
        std::vector<double> *dataVals = new std::vector<double>();
        for(int i = 0; i < numBands; ++i)
        {
            if(this->useNoDataValue & (bandValues[i] == this->noDataValue))
            {
                // ignore value.
            }
            else
            {
                dataVals->push_back(bandValues[i]);
            }
        }
        std::sort(dataVals->begin(), dataVals->end());
        
        rsgis::math::RSGISMathsUtils mathUtils;
        mathUtils.initStatsSummaryValues(this->statsSummary);
        mathUtils.generateStats(dataVals, statsSummary);
        
        unsigned int outIdx = 0;

        if(statsSummary->calcMin)
        {
            output[outIdx++] = statsSummary->min;
        }
        if(statsSummary->calcMax)
        {
            output[outIdx++] = statsSummary->max;
        }
        if(statsSummary->calcMean)
        {
            output[outIdx++] = statsSummary->mean;
        }
        if(statsSummary->calcMedian)
        {
            output[outIdx++] = statsSummary->median;
        }
        if(statsSummary->calcSum)
        {
            output[outIdx++] = statsSummary->sum;
        }
        if(statsSummary->calcStdDev)
        {
            output[outIdx++] = statsSummary->stdDev;
        }

        delete dataVals;
    }
    
    RSGISImagePixelSummaries::~RSGISImagePixelSummaries()
    {
        
    }
    
    
    
    
    
    
    
    
    RSGISCalcImageHistogramNoData::RSGISCalcImageHistogramNoData(unsigned int imgBand, bool noDataSpecified, float noDataVal, unsigned int numBins, float *binRanges, unsigned int *binCounts): RSGISCalcImageValue(0)
    {
        this->imgBand = imgBand;
        this->noDataSpecified = noDataSpecified;
        this->noDataVal = noDataVal;
        this->numBins = numBins;
        this->binRanges = binRanges;
        this->binCounts = binCounts;
    }
    
    void RSGISCalcImageHistogramNoData::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        if(this->noDataSpecified)
        {
            if(bandValues[imgBand-1] != noDataVal)
            {
                for(unsigned int i = 0; i < numBins; ++i)
                {
                    if((bandValues[imgBand-1] >= binRanges[i]) & (bandValues[imgBand-1] < binRanges[i+1]))
                    {
                        ++this->binCounts[i];
                        break;
                    }
                }
            }
        }
        else
        {
            for(unsigned int i = 0; i < numBins; ++i)
            {
                if((bandValues[imgBand-1] >= binRanges[i]) & (bandValues[imgBand-1] < binRanges[i+1]))
                {
                    ++this->binCounts[i];
                    break;
                }
            }
        }
    }
    
    RSGISCalcImageHistogramNoData::~RSGISCalcImageHistogramNoData()
    {
        
    }
    
    
    
    
    RSGISCalcImageStatisticsMaskStatsNoData::RSGISCalcImageStatisticsMaskStatsNoData(int numberOutBands, int numInputBands, long maskVal, double *noDataVals, bool useNoData, bool calcSD, bool onePassSD) : RSGISCalcImageValue(numberOutBands)
    {
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
        this->noDataVals = noDataVals;
        this->maskVal = maskVal;
        this->useNoData = useNoData;
    }
    
    void RSGISCalcImageStatisticsMaskStatsNoData::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException)
    {
        if(numfloatVals != this->numInputBands)
        {
            throw RSGISImageCalcException("The number input bands needs to match the figure provided.");
        }
        
        if(numIntVals != 1)
        {
            throw RSGISImageCalcException("The number of mask bands should be 1.");
        }
        
        if(intBandValues[0] == maskVal)
        {
            if(!calcSD || (calcSD && onePassSD)) // If not calculating SD or calculating SD with a single pass calculate mean, min + max
            {
                calcMean = true;
                for(int i = 0; i < numfloatVals; i++)
                {
                    if(!boost::math::isnan(floatBandValues[i]))
                    {
                        if(firstMean[i])
                        {
                            if((!useNoData) || (useNoData && (floatBandValues[i] != this->noDataVals[i])))
                            {
                                meanSum[i] = floatBandValues[i];
                                min[i] = floatBandValues[i];
                                max[i] = floatBandValues[i];
                                ++n[i];
                                firstMean[i] = false;
                            }
                        }
                        else
                        {
                            if((!useNoData) || (useNoData && (floatBandValues[i] != this->noDataVals[i])))
                            {
                                meanSum[i] = meanSum[i] + floatBandValues[i];
                                if(floatBandValues[i] < min[i])
                                {
                                    min[i] = floatBandValues[i];
                                }
                                if(floatBandValues[i] > max[i])
                                {
                                    max[i] = floatBandValues[i];
                                }
                                ++n[i];
                            }
                        }
                        if(calcSD && onePassSD)
                        {
                            if((!useNoData) || (useNoData && (floatBandValues[i] != this->noDataVals[i])))
                            {
                                sumSq[i] = sumSq[i] + floatBandValues[i]*floatBandValues[i];
                            }
                        }
                    }
                }
            }
            else
            {
                if(!calcMean)
                {
                    throw RSGISImageCalcException("The standard deviation cannot be calculated before the mean.");
                }
                
                for(int i = 0; i < numfloatVals; i++)
                {
                    if(firstSD[i])
                    {
                        if((!useNoData) || (useNoData && (floatBandValues[i] != this->noDataVals[i])))
                        {
                            mean[i] = meanSum[i]/n[i];
                            diffZ = mean[i] - floatBandValues[i];
                            sumDiffZ[i] = (diffZ * diffZ);
                            firstSD[i] = false;
                        }
                    }
                    else
                    {
                        if((!useNoData) || (useNoData && (floatBandValues[i] != this->noDataVals[i])))
                        {
                            diffZ = mean[i] - floatBandValues[i];
                            sumDiffZ[i] = sumDiffZ[i] + (diffZ * diffZ);
                        }
                    }
                }
            }
        }
    }
    
    void RSGISCalcImageStatisticsMaskStatsNoData::getImageStats(ImageStats** inStats, int numInputBands) throw(RSGISImageCalcException)
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
    
    void RSGISCalcImageStatisticsMaskStatsNoData::calcStdDev()
    {
        calcSD = true;
    }
    
    RSGISCalcImageStatisticsMaskStatsNoData::~RSGISCalcImageStatisticsMaskStatsNoData()
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
    
    
    
    
    
    
    
    RSGISCalcMultiImageStatSummaries::RSGISCalcMultiImageStatSummaries(unsigned int numOutBands, rsgis::math::rsgissummarytype sumType, unsigned int numInImgs, unsigned int numInImgBands, float noDataValue, bool useNoDataValue): RSGISCalcImageValue(numOutBands)
    {
        this->sumType = sumType;
        this->numInImgs = numInImgs;
        this->numInImgBands = numInImgBands;
        this->noDataValue = noDataValue;
        this->useNoDataValue = useNoDataValue;
        
        this->mathUtils = new rsgis::math::RSGISMathsUtils();
        this->statsSumObj = new rsgis::math::RSGISStatsSummary();
        this->mathUtils->initStatsSummary(statsSumObj);
        
        if(sumType == rsgis::math::sumtype_mean)
        {
            statsSumObj->calcMean = true;
        }
        else if(sumType == rsgis::math::sumtype_max)
        {
            statsSumObj->calcMax = true;
        }
        else if(sumType == rsgis::math::sumtype_min)
        {
            statsSumObj->calcMin = true;
        }
        else if(sumType == rsgis::math::sumtype_stddev)
        {
            statsSumObj->calcStdDev = true;
        }
        else if(sumType == rsgis::math::sumtype_median)
        {
            statsSumObj->calcMedian = true;
        }
        else if(sumType == rsgis::math::sumtype_mode)
        {
            statsSumObj->calcMode = true;
        }
        else if(sumType == rsgis::math::sumtype_sum)
        {
            statsSumObj->calcSum = true;
        }
        else if(sumType == rsgis::math::sumtype_range)
        {
            statsSumObj->calcMin = true;
            statsSumObj->calcMax = true;
        }
        else
        {
            throw RSGISImageCalcException("Did not recognise the summary type.");
        }
        
        this->data = new std::vector<double>();
        
        
        this->totNumInBands = numInImgBands * numInImgs;
    }
    
    void RSGISCalcMultiImageStatSummaries::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
    {
        if(numBands != totNumInBands)
        {
            throw RSGISImageCalcException("Number of input image bands does not match the number expected.");
        }
        
        this->mathUtils->initStatsSummaryValues(statsSumObj);
        unsigned int idx = 0;
        
        for(int i = 0; i < this->numInImgBands; ++i)
        {
            data->clear();
            for(int n = 0; n < this->numInImgs; ++n)
            {
                idx = (n * this->numInImgBands) + i;
                if(idx >= numBands)
                {
                    throw RSGISImageCalcException("Something has gone really wrong, calculated index is more than the number of image layers.");
                }
                
                if(this->useNoDataValue)
                {
                    if(bandValues[idx] != noDataValue)
                    {
                        data->push_back(bandValues[idx]);
                    }
                }
                else
                {
                    data->push_back(bandValues[idx]);
                }
            }
            if(data->size() > 1)
            {
                this->mathUtils->generateStats(data, statsSumObj);
                if(sumType == rsgis::math::sumtype_mean)
                {
                    output[i] = statsSumObj->mean;
                }
                else if(sumType == rsgis::math::sumtype_max)
                {
                    output[i] = statsSumObj->max;
                }
                else if(sumType == rsgis::math::sumtype_min)
                {
                    output[i] = statsSumObj->min;
                }
                else if(sumType == rsgis::math::sumtype_stddev)
                {
                    output[i] = statsSumObj->stdDev;
                }
                else if(sumType == rsgis::math::sumtype_median)
                {
                    output[i] = statsSumObj->median;
                }
                else if(sumType == rsgis::math::sumtype_mode)
                {
                    output[i] = statsSumObj->mode;
                }
                else if(sumType == rsgis::math::sumtype_sum)
                {
                    output[i] = statsSumObj->sum;
                }
                else if(sumType == rsgis::math::sumtype_range)
                {
                    output[i] = statsSumObj->max - statsSumObj->min;
                }
            }
            else if(data->size() == 1)
            {
                output[i] = data->at(0);
            }
            else
            {
                output[i] = 0.0;
            }
        }
        
    }

    RSGISCalcMultiImageStatSummaries::~RSGISCalcMultiImageStatSummaries()
    {
        delete this->mathUtils;
        delete this->statsSumObj;
        delete this->data;
    }
    
    
    
    

    RSGISCalcImageDifference::RSGISCalcImageDifference(unsigned int numOutBands):RSGISCalcImageValue(numOutBands)
    {
        
    }
    
    void RSGISCalcImageDifference::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
    {
        if((numBands/2) != this->numOutBands)
        {
            throw RSGISImageCalcException("The number of image bands must be twice the number of output bands.");
        }
        
        for(int i = 0; i < this->numOutBands; ++i)
        {
            output[i] = bandValues[i] - bandValues[i+this->numOutBands];
        }
        
    }
    
    RSGISCalcImageDifference::~RSGISCalcImageDifference()
    {
        
    }
    
    
    
    
    
    
}}
