/*
 *  RSGISSARTextureFilters.cpp
 *  RSGIS_LIB
 *
 *  Created by Dan Clewley on 06/08/2012.
 *  Copyright 2013 RSGISLib.
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

#include "RSGISSARTextureFilters.h"

namespace rsgis{namespace filter{

    RSGISNormVarPowerFilter::RSGISNormVarPowerFilter(int numberOutBands, int size, std::string filenameEnding) : RSGISImageFilter(numberOutBands, size, filenameEnding){}

    void RSGISNormVarPowerFilter::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
        unsigned int middleVal = floor(((float)winSize) / 2);
        unsigned int numVal = 0; // Number of values counted
        double outI = 0;
        double iMean = 0;
        double iMeanSq = 0;
        double iSqMean = 0;

        for(int i = 0; i < numBands; i++)
        {
            iMean = 0;
            iMeanSq = 0;
            iSqMean = 0;
            numVal = 0;

            // Check for data at the centre of the block (skip if no data to preserve scene edges)
            if((dataBlock[i][middleVal][middleVal] == 0) | ((boost::math::isnan)(dataBlock[i][middleVal][middleVal])))
            {
                outI = 0;
            }
            else
            {
                for(int j = 0; j < size; j++)
                {
                    for(int k = 0; k < size; k++)
                    {
                         if((dataBlock[i][j][k] != 0) && ((boost::math::isnan)(dataBlock[i][j][k]) == false))
                         {
                             iMean = iMean + dataBlock[i][j][k];
                             iSqMean = iSqMean + (dataBlock[i][j][k]*dataBlock[i][j][k]);

                             ++numVal;
                         }
                    }
                }

                // Check there were at least three data values
                if(numVal > 3)
                {
                    iMean = iMean / numVal;
                    iMeanSq = iMean*iMean;
                    iSqMean = iSqMean / numVal;
                    outI = (iSqMean / iMeanSq) - 1;
                }
                else
                {
                    outI = 0;
                }
            }
            output[i] = outI;
        }
	}


    RSGISNormVarAmplitudeFilter::RSGISNormVarAmplitudeFilter(int numberOutBands, int size, std::string filenameEnding) : RSGISImageFilter(numberOutBands, size, filenameEnding){}

    void RSGISNormVarAmplitudeFilter::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
        unsigned int middleVal = floor(((float)winSize) / 2);
        unsigned int numVal = 0; // Number of values counted
        double outI = 0;
        double iMean = 0;
        double iMeanSq = 0;
        double iSqMean = 0;

        for(int i = 0; i < numBands; i++)
        {
            iMean = 0;
            iMeanSq = 0;
            iSqMean = 0;
            numVal = 0;

            // Check for data at the centre of the block (skip if no data to preserve scene edges)
            if((dataBlock[i][middleVal][middleVal] == 0) | ((boost::math::isnan)(dataBlock[i][middleVal][middleVal])))
            {
                outI = 0;
            }
            else
            {
                for(int j = 0; j < size; j++)
                {
                    for(int k = 0; k < size; k++)
                    {
                        if((dataBlock[i][j][k] != 0) && ((boost::math::isnan)(dataBlock[i][j][k]) == false))
                        {
                            iMean = iMean + sqrt(dataBlock[i][j][k]);
                            iSqMean = iSqMean + dataBlock[i][j][k];

                            ++numVal;
                        }
                    }
                }

                // Check there were at least three data values
                if(numVal > 3)
                {

                    iMean = iMean / numVal;
                    iMeanSq = iMean*iMean;
                    iSqMean = iSqMean / numVal;

                    outI = (iSqMean / iMeanSq) - 1;
                }
                else{outI = 0;}
            }
            output[i] = outI;
        }
	}


    RSGISNormVarLnPowerFilter::RSGISNormVarLnPowerFilter(int numberOutBands, int size, std::string filenameEnding) : RSGISImageFilter(numberOutBands, size, filenameEnding){}

    void RSGISNormVarLnPowerFilter::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
        unsigned int middleVal = floor(((float)winSize) / 2);
        unsigned int numVal = 0; // Number of values counted
        double outI = 0;
        double iMean = 0;
        double iMeanSq = 0;
        double iSqMean = 0;

        for(int i = 0; i < numBands; i++)
        {
            iMean = 0;
            iMeanSq = 0;
            iSqMean = 0;
            numVal = 0;

            // Check for data at the centre of the block (skip if no data to preserve scene edges)
            if((dataBlock[i][middleVal][middleVal] == 0) | ((boost::math::isnan)(dataBlock[i][middleVal][middleVal])))
            {
                outI = 0;
            }
            else
            {
                for(int j = 0; j < size; j++)
                {
                    for(int k = 0; k < size; k++)
                    {
                        if((dataBlock[i][j][k] != 0) && ((boost::math::isnan)(dataBlock[i][j][k]) == false))
                        {
                            iMean = iMean + log(dataBlock[i][j][k]);
                            iSqMean = iSqMean + log(dataBlock[i][j][k])*log(dataBlock[i][j][k]);

                            ++numVal;
                        }
                    }
                }

                // Check there were at least three data values
                if(numVal > 3)
                {

                    iMean = iMean / numVal;
                    iMeanSq = iMean*iMean;
                    iSqMean = iSqMean / numVal;

                    outI = (iSqMean / iMeanSq) - 1;
                }
                else{outI = 0;}
            }
            output[i] = outI;
        }
	}

    RSGISNormLnFilter::RSGISNormLnFilter(int numberOutBands, int size, std::string filenameEnding) : RSGISImageFilter(numberOutBands, size, filenameEnding){}

    void RSGISNormLnFilter::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
        unsigned int middleVal = floor(((float)winSize) / 2);
        unsigned int numVal = 0; // Number of values counted
        double outI = 0;
        double iMean = 0;
        double iMeanLn = 0;
        double iLnMean = 0;

        for(int i = 0; i < numBands; i++)
        {
            iMean = 0;
            iMeanLn = 0;
            iLnMean = 0;
            numVal = 0;

            // Check for data at the centre of the block (skip if no data to preserve scene edges)
            if((dataBlock[i][middleVal][middleVal] == 0) | ((boost::math::isnan)(dataBlock[i][middleVal][middleVal])))
            {
                outI = 0;
            }
            else
            {
                for(int j = 0; j < size; j++)
                {
                    for(int k = 0; k < size; k++)
                    {
                        if((dataBlock[i][j][k] != 0) && ((boost::math::isnan)(dataBlock[i][j][k]) == false))
                        {
                            iMean = iMean + dataBlock[i][j][k];
                            iLnMean = iLnMean + log(dataBlock[i][j][k]);

                            ++numVal;
                        }
                    }
                }

                // Check there were at least three data values
                if(numVal > 3)
                {

                    iMean = iMean / numVal;
                    iMeanLn = log(iMean);
                    iLnMean = iLnMean / numVal;

                    outI = iLnMean - iMeanLn;
                }
                else{outI = 0;}
            }
            output[i] = outI;
        }
	}

    RSGISTextureVar::RSGISTextureVar(int numberOutBands, int size, std::string filenameEnding) : RSGISImageFilter(numberOutBands, size, filenameEnding){}

    void RSGISTextureVar::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
        unsigned int middleVal = floor(((float)winSize) / 2);
        unsigned int numVal = 0; // Number of values counted
        double outI = 0;
        double stDev = 0;
        double iMean = 0;
        double iSum = 0;
        double iMinusMeanSqSum = 0;

        for(int i = 0; i < numBands; i++)
        {
            iMean = 0;
            iSum = 0;
            iMinusMeanSqSum = 0;
            numVal = 0;

            // Check for data at the centre of the block (skip if no data to preserve scene edges)
            if((dataBlock[i][middleVal][middleVal] == 0) | ((boost::math::isnan)(dataBlock[i][middleVal][middleVal])))
            {
                outI = 0;
            }
            else
            {
                for(int j = 0; j < size; j++)
                {
                    for(int k = 0; k < size; k++)
                    {
                         if((dataBlock[i][j][k] != 0) && ((boost::math::isnan)(dataBlock[i][j][k]) == false))
                         {
                             iSum = iSum + dataBlock[i][j][k];
                             ++numVal;
                         }
                    }
                }

                // Check there were at least three data values
                if(numVal > 3)
                {
                    iMean = iSum / numVal;

                    for(int j = 0; j < size; j++)
                    {
                        for(int k = 0; k < size; k++)
                        {
                             if((dataBlock[i][j][k] != 0) && ((boost::math::isnan)(dataBlock[i][j][k]) == false))
                             {
                                 iMinusMeanSqSum = iMinusMeanSqSum + pow((dataBlock[i][j][k] - iMean),2);
                             }
                        }
                    }


                    stDev = iMinusMeanSqSum / numVal; // Variance
                    stDev = sqrt(stDev); // Standard deviation

                    outI = (pow((stDev / iMean),2)-(1/numVal))/(1+(1/numVal));

                }
                else{outI = 0;}
            }
            output[i] = outI;
        }
	}

}}
