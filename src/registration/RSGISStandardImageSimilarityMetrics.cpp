/*
 *  RSGISStandardImageSimilarityMetrics.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/09/2010.
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

#include "RSGISStandardImageSimilarityMetrics.h"


namespace rsgis{namespace reg{
	
	float RSGISEuclideanSimilarityMetric::calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims)
	{
		unsigned int totalNumVals = 0;
		
		double sqDiff = 0;
		
		for(unsigned int i = 0; i < numDims; ++i)
		{
			for(unsigned int j = 0; j < numVals; ++j)
			{
				if((!((boost::math::isnan)(reference[i][j]))) & (!((boost::math::isnan)(floating[i][j]))))
				{
					sqDiff += ((reference[i][j] - floating[i][j]) * (reference[i][j] - floating[i][j]));
					++totalNumVals;
				}
			}
		}
		
		return sqrt(sqDiff/totalNumVals);
	}
	
	float RSGISSquaredDifferenceSimilarityMetric::calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims)
	{
		unsigned int totalNumVals = 0;
		
		double sqDiff = 0;
		
		for(unsigned int i = 0; i < numDims; ++i)
		{
			for(unsigned int j = 0; j < numVals; ++j)
			{
				if((!((boost::math::isnan)(reference[i][j]))) & (!((boost::math::isnan)(floating[i][j]))))
				{
					sqDiff += ((reference[i][j] - floating[i][j]) * (reference[i][j] - floating[i][j]));
					++totalNumVals;
				}
			}
		}
		
		return sqDiff/totalNumVals;
	}
	
	float RSGISManhattanSimilarityMetric::calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims)
	{
		unsigned int totalNumVals = 0;
		
		double absDiff = 0;
		
		for(unsigned int i = 0; i < numDims; ++i)
		{
			for(unsigned int j = 0; j < numVals; ++j)
			{
				if((!((boost::math::isnan)(reference[i][j]))) & (!((boost::math::isnan)(floating[i][j]))))
				{
					absDiff += fabs(reference[i][j] - floating[i][j]);
					++totalNumVals;
				}
			}
		}
		
		return absDiff/totalNumVals;
	}
	
	float RSGISCorrelationSimilarityMetric::calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims)
	{
		unsigned int n = 0;
		
		double sumRF = 0;
		double sumR = 0;
		double sumF = 0;
		double sumRSq = 0;
		double sumFSq = 0;
				
		for(unsigned int i = 0; i < numDims; ++i)
		{
			for(unsigned int j = 0; j < numVals; ++j)
			{
				if((!((boost::math::isnan)(reference[i][j]))) & (!((boost::math::isnan)(floating[i][j]))))
				{
					sumRF += (reference[i][j] * floating[i][j]);
					sumR += reference[i][j];
					sumF += floating[i][j];
					sumRSq += (reference[i][j] * reference[i][j]);
					sumFSq += (floating[i][j] * floating[i][j]);
					++n;
				}
			}
		}
		
		float val = (((n * sumRF) - (sumR * sumF))/sqrt(((n*sumRSq)-(sumR*sumR))*((n*sumFSq)-(sumF*sumF))));
        
        if(val < 0)
        {
            val *= -1;
        }
		
		return val;
	}







    RSGISImgCalcEuclideanSimilarityMetric::RSGISImgCalcEuclideanSimilarityMetric(std::vector<unsigned int> imgABands,
                                                                                 std::vector<unsigned int> imgBBands,
                                                                                 float imgANoData, bool useImgANoData,
                                                                                 float imgBNoData, bool useImgBNoData) : RSGISImageCalcSimilarityMetric(imgABands,
                                                                                                                                                        imgBBands,
                                                                                                                                                        imgANoData,
                                                                                                                                                        useImgANoData,
                                                                                                                                                        imgBNoData,
                                                                                                                                                        useImgBNoData)
    {
        this->n = 0;
        this->sum = 0.0;

    }

    void RSGISImgCalcEuclideanSimilarityMetric::calcImageValue(float *bandValues, int numBands)
    {
	    unsigned int bandA = 0;
	    unsigned int bandB = 0;
	    bool noDataFound = false;
        for(unsigned int i = 0; i < this->nBands; ++i)
        {
            bandA = this->imgABands.at(i);
            bandB = this->imgBBands.at(i);
            if(this->useImgANoData && (bandValues[bandA] == this->imgANoData))
            {
                noDataFound = true;
            }
            else if( (boost::math::isnan)(bandValues[bandA]) )
            {
                noDataFound = true;
            }
            else if(this->useImgBNoData && (bandValues[bandB] == this->imgBNoData))
            {
                noDataFound = true;
            }
            else if( (boost::math::isnan)(bandValues[bandB]) )
            {
                noDataFound = true;
            }

            if(!noDataFound)
            {
                this->sum += ((bandValues[bandA] - bandValues[bandB]) * (bandValues[bandA] - bandValues[bandB]));
                ++this->n;
            }
        }
    }

    void RSGISImgCalcEuclideanSimilarityMetric::reset()
    {
        this->n = 0;
        this->sum = 0.0;
    }

    double RSGISImgCalcEuclideanSimilarityMetric::metricVal()
    {
        return sqrt(this->sum / this->n);
    }



    RSGISImgCalcManhattanSimilarityMetric::RSGISImgCalcManhattanSimilarityMetric(std::vector<unsigned int> imgABands,
                                                                                 std::vector<unsigned int> imgBBands,
                                                                                 float imgANoData, bool useImgANoData,
                                                                                 float imgBNoData, bool useImgBNoData) : RSGISImageCalcSimilarityMetric(imgABands,
                                                                                                                                                        imgBBands,
                                                                                                                                                        imgANoData,
                                                                                                                                                        useImgANoData,
                                                                                                                                                        imgBNoData,
                                                                                                                                                        useImgBNoData)
    {
        this->n = 0;
        this->sum = 0.0;

    }

    void RSGISImgCalcManhattanSimilarityMetric::calcImageValue(float *bandValues, int numBands)
    {
        unsigned int bandA = 0;
        unsigned int bandB = 0;
        bool noDataFound = false;
        for(unsigned int i = 0; i < this->nBands; ++i)
        {
            bandA = this->imgABands.at(i);
            bandB = this->imgBBands.at(i);
            if(this->useImgANoData && (bandValues[bandA] == this->imgANoData))
            {
                noDataFound = true;
            }
            else if( (boost::math::isnan)(bandValues[bandA]) )
            {
                noDataFound = true;
            }
            else if(this->useImgBNoData && (bandValues[bandB] == this->imgBNoData))
            {
                noDataFound = true;
            }
            else if( (boost::math::isnan)(bandValues[bandB]) )
            {
                noDataFound = true;
            }

            if(!noDataFound)
            {
                this->sum += fabs(bandValues[bandA] - bandValues[bandB]);
                ++this->n;
            }
        }
    }

    void RSGISImgCalcManhattanSimilarityMetric::reset()
    {
        this->n = 0;
        this->sum = 0.0;
    }

    double RSGISImgCalcManhattanSimilarityMetric::metricVal()
    {
        return this->sum / this->n;
    }


    RSGISImgCalcSquaredDifferenceSimilarityMetric::RSGISImgCalcSquaredDifferenceSimilarityMetric(std::vector<unsigned int> imgABands,
                                                                                                 std::vector<unsigned int> imgBBands,
                                                                                                 float imgANoData, bool useImgANoData,
                                                                                                 float imgBNoData, bool useImgBNoData) : RSGISImageCalcSimilarityMetric(imgABands,
                                                                                                                                                                        imgBBands,
                                                                                                                                                                        imgANoData,
                                                                                                                                                                        useImgANoData,
                                                                                                                                                                        imgBNoData,
                                                                                                                                                                        useImgBNoData)
    {
        this->n = 0;
        this->sum = 0.0;
    }

    void RSGISImgCalcSquaredDifferenceSimilarityMetric::calcImageValue(float *bandValues, int numBands)
    {
        unsigned int bandA = 0;
        unsigned int bandB = 0;
        bool noDataFound = false;
        for(unsigned int i = 0; i < this->nBands; ++i)
        {
            bandA = this->imgABands.at(i);
            bandB = this->imgBBands.at(i);
            if(this->useImgANoData && (bandValues[bandA] == this->imgANoData))
            {
                noDataFound = true;
            }
            else if( (boost::math::isnan)(bandValues[bandA]) )
            {
                noDataFound = true;
            }
            else if(this->useImgBNoData && (bandValues[bandB] == this->imgBNoData))
            {
                noDataFound = true;
            }
            else if( (boost::math::isnan)(bandValues[bandB]) )
            {
                noDataFound = true;
            }

            if(!noDataFound)
            {
                this->sum += ((bandValues[bandA] - bandValues[bandB]) * (bandValues[bandA] - bandValues[bandB]));
                ++this->n;
            }
        }
    }

    void RSGISImgCalcSquaredDifferenceSimilarityMetric::reset()
    {
        this->n = 0;
        this->sum = 0.0;
    }

    double RSGISImgCalcSquaredDifferenceSimilarityMetric::metricVal()
    {
        return this->sum / this->n;
    }


    RSGISImgCalcCorrelationSimilarityMetric::RSGISImgCalcCorrelationSimilarityMetric(std::vector<unsigned int> imgABands,
                                                                                     std::vector<unsigned int> imgBBands,
                                                                                     float imgANoData, bool useImgANoData,
                                                                                     float imgBNoData, bool useImgBNoData) : RSGISImageCalcSimilarityMetric(imgABands,
                                                                                                                                                            imgBBands,
                                                                                                                                                            imgANoData,
                                                                                                                                                            useImgANoData,
                                                                                                                                                            imgBNoData,
                                                                                                                                                            useImgBNoData)
    {
        this->n = 0;
        this->sumRF = 0.0;
        this->sumR = 0.0;
        this->sumF = 0.0;
        this->sumRSq = 0.0;
        this->sumFSq = 0.0;
    }

    void RSGISImgCalcCorrelationSimilarityMetric::calcImageValue(float *bandValues, int numBands)
    {
        unsigned int bandA = 0;
        unsigned int bandB = 0;
        bool noDataFound = false;
        for(unsigned int i = 0; i < this->nBands; ++i)
        {
            bandA = this->imgABands.at(i);
            bandB = this->imgBBands.at(i);
            if(this->useImgANoData && (bandValues[bandA] == this->imgANoData))
            {
                noDataFound = true;
            }
            else if( (boost::math::isnan)(bandValues[bandA]) )
            {
                noDataFound = true;
            }
            else if(this->useImgBNoData && (bandValues[bandB] == this->imgBNoData))
            {
                noDataFound = true;
            }
            else if( (boost::math::isnan)(bandValues[bandB]) )
            {
                noDataFound = true;
            }

            if(!noDataFound)
            {
                this->sumRF += (bandValues[bandA] * bandValues[bandB]);
                this->sumR += bandValues[bandA];
                this->sumF += bandValues[bandB];
                this->sumRSq += (bandValues[bandA] * bandValues[bandA]);
                this->sumFSq += (bandValues[bandB] * bandValues[bandB]);
                ++this->n;
            }
        }
    }

    void RSGISImgCalcCorrelationSimilarityMetric::reset()
    {
        this->n = 0;
        this->sumRF = 0.0;
        this->sumR = 0.0;
        this->sumF = 0.0;
        this->sumRSq = 0.0;
        this->sumFSq = 0.0;
    }

    double RSGISImgCalcCorrelationSimilarityMetric::metricVal()
    {
        double val = (((this->n * this->sumRF) - (this->sumR * this->sumF))/
                sqrt(
                        ((this->n*this->sumRSq)-(this->sumR*this->sumR))
                        *
                        ((this->n*this->sumFSq)-(this->sumF*this->sumF))
                        )
                        );

        if(val < 0)
        {
            val *= -1;
        }

        return val;
    }



}}

