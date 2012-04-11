/*
 *  RSGISStretchImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/09/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#include "RSGISStretchImage.h"

namespace rsgis { namespace img {

	RSGISStretchImage::RSGISStretchImage(GDALDataset *inputImage, string outputImage, bool ignoreZeros, string imageFormat, GDALDataType outDataType): inputImage(NULL), outputImage(""), ignoreZeros(true)
	{
		this->inputImage = inputImage;
		this->outputImage = outputImage;
        this->ignoreZeros = ignoreZeros;
        this->imageFormat = imageFormat;
        this->outDataType = outDataType;
	}
	
	void RSGISStretchImage::executeLinearMinMaxStretch() throw(RSGISImageCalcException)
	{
		GDALDataset **datasets = NULL;
		RSGISImageStatistics *calcImageStats = NULL;
		ImageStats **stats = NULL;
		RSGISCalcImage *calcImg = NULL;
		RSGISLinearStretchImage *linearStretchImage = NULL;
		double *imageMax = NULL;
		double *imageMin = NULL;
		double *outMax = NULL;
		double *outMin = NULL;
		try
		{
			int numBands = inputImage->GetRasterCount();
			datasets = new GDALDataset*[1];
			datasets[0] = inputImage;
			
			imageMax = new double[numBands];
			imageMin = new double[numBands];
			outMax = new double[numBands];
			outMin = new double[numBands];
			
			stats = new ImageStats*[numBands];
			for(int i = 0; i < numBands; i++)
			{
				stats[i] = new ImageStats();
			}
			calcImageStats = new RSGISImageStatistics();
			calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, false, ignoreZeros);
			
			for(int i = 0; i < numBands; i++)
			{
				imageMin[i] = stats[i]->min;
				imageMax[i] = stats[i]->max;
				outMax[i] = 255;
				outMin[i] = 0;
			}
			
			for(int i = 0; i < numBands; i++)
			{
				delete stats[i];
			}
			delete[] stats;
			delete calcImageStats;
			
			linearStretchImage = new RSGISLinearStretchImage(numBands, imageMax, imageMin, outMax, outMin);
			calcImg = new RSGISCalcImage(linearStretchImage, "", true);
			calcImg->calcImage(datasets, 1, outputImage, false, NULL, imageFormat, outDataType);
			
		}
		catch(RSGISImageCalcException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw RSGISImageCalcException(e.what());
		}
		
		delete[] imageMax;
		delete[] imageMin;
		delete[] outMax;
		delete[] outMin;
		
		delete linearStretchImage;
		delete calcImg;
		
		if(datasets != NULL)
		{
			delete[] datasets;
		}
	}
	
	void RSGISStretchImage::executeLinearPercentStretch(float percent) throw(RSGISImageCalcException)
	{
		GDALDataset **datasets = NULL;
		RSGISImageStatistics *calcImageStats = NULL;
		ImageStats **stats = NULL;
		RSGISCalcImage *calcImg = NULL;
		RSGISLinearStretchImage *linearStretchImage = NULL;
		double *imageMax = NULL;
		double *imageMin = NULL;
		double *outMax = NULL;
		double *outMin = NULL;
		try
		{
			int numBands = inputImage->GetRasterCount();
			datasets = new GDALDataset*[1];
			datasets[0] = inputImage;
			
			imageMax = new double[numBands];
			imageMin = new double[numBands];
			outMax = new double[numBands];
			outMin = new double[numBands];
			
			stats = new ImageStats*[numBands];
			for(int i = 0; i < numBands; i++)
			{
				stats[i] = new ImageStats();
			}
			calcImageStats = new RSGISImageStatistics();
			calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, false, ignoreZeros);
			
			double onePercent = 0;
			double onePercentUpper = 0;
			double onePercentLower = 0;
			
			for(int i = 0; i < numBands; i++)
			{				
				onePercent = (stats[i]->max - stats[i]->min)/100;
				
				onePercentUpper = (stats[i]->max - stats[i]->mean)/50;
				onePercentLower = (stats[i]->mean - stats[i]->min)/50;
				
				//cout << "Band " << i << " 1 % = " << onePercent << endl;
				//cout << "Band " << i << " 1 % U = " << onePercentUpper << endl;
				//cout << "Band " << i << " 1 % L = " << onePercentLower << endl;
				
				imageMin[i] = stats[i]->min + (onePercentLower * percent);
				imageMax[i] = stats[i]->max - (onePercentUpper * percent);
				outMax[i] = 255;
				outMin[i] = 0;
			}
			
			for(int i = 0; i < numBands; i++)
			{
				delete stats[i];
			}
			delete[] stats;
			delete calcImageStats;
			
			linearStretchImage = new RSGISLinearStretchImage(numBands, imageMax, imageMin, outMax, outMin);
			calcImg = new RSGISCalcImage(linearStretchImage, "", true);
			calcImg->calcImage(datasets, 1, outputImage, false, NULL, imageFormat, outDataType);
			
		}
		catch(RSGISImageCalcException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw RSGISImageCalcException(e.what());
		}
		
		delete[] imageMax;
		delete[] imageMin;
		delete[] outMax;
		delete[] outMin;
		
		delete linearStretchImage;
		delete calcImg;
		
		if(datasets != NULL)
		{
			delete[] datasets;
		}
		
	}
	
	void RSGISStretchImage::executeLinearStdDevStretch(float stddev) throw(RSGISImageCalcException)
	{
		GDALDataset **datasets = NULL;
		RSGISImageStatistics *calcImageStats = NULL;
		ImageStats **stats = NULL;
		RSGISCalcImage *calcImg = NULL;
		RSGISLinearStretchImage *linearStretchImage = NULL;
		double *imageMax = NULL;
		double *imageMin = NULL;
		double *outMax = NULL;
		double *outMin = NULL;
		try
		{
			int numBands = inputImage->GetRasterCount();
			datasets = new GDALDataset*[1];
			datasets[0] = inputImage;
			
			imageMax = new double[numBands];
			imageMin = new double[numBands];
			outMax = new double[numBands];
			outMin = new double[numBands];
			
			stats = new ImageStats*[numBands];
			for(int i = 0; i < numBands; i++)
			{
				stats[i] = new ImageStats();
			}
			calcImageStats = new RSGISImageStatistics();
			calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, true, ignoreZeros);
			
			for(int i = 0; i < numBands; i++)
			{				
				imageMin[i] = stats[i]->mean - (stats[i]->stddev * stddev);
				imageMax[i] = stats[i]->mean + (stats[i]->stddev * stddev);
                
                if(imageMin[i] < stats[i]->min)
                {
                    imageMin[i] = stats[i]->min;
                }
                if(imageMax[i] > stats[i]->max)
                {
                    imageMax[i] = stats[i]->max;
                }
                
                cout << "Band[" << i+1 << "] Min = " << stats[i]->min << " Mean = " << stats[i]->mean << " (Std Dev = " << stats[i]->stddev << ") max = " << stats[i]->max << endl;
                
				outMax[i] = 255;
				outMin[i] = 0;
			}
			
			for(int i = 0; i < numBands; i++)
			{
				delete stats[i];
			}
			delete[] stats;
			delete calcImageStats;
			
			linearStretchImage = new RSGISLinearStretchImage(numBands, imageMax, imageMin, outMax, outMin);
			calcImg = new RSGISCalcImage(linearStretchImage, "", true);
			calcImg->calcImage(datasets, 1, outputImage, false, NULL, imageFormat, outDataType);
			
		}
		catch(RSGISImageCalcException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw RSGISImageCalcException(e.what());
		}
		
		delete[] imageMax;
		delete[] imageMin;
		delete[] outMax;
		delete[] outMin;
		
		delete linearStretchImage;
		delete calcImg;
		
		if(datasets != NULL)
		{
			delete[] datasets;
		}
	}
	
	void RSGISStretchImage::executeHistogramStretch() throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Histogram Stretch is not implmented yet\n");
	}
	
	void RSGISStretchImage::executeExponentialStretch() throw(RSGISImageCalcException)
	{
		GDALDataset **datasets = NULL;
		RSGISImageStatistics *calcImageStats = NULL;
		ImageStats **stats = NULL;
		RSGISCalcImage *calcImg = NULL;
		RSGISFuncLinearStretchImage *stretchImage = NULL;
		double *imageMax = NULL;
		double *imageMin = NULL;
		double *outMax = NULL;
		double *outMin = NULL;
		
		RSGISMathFunction *function = new RSGISExponentStretchFunction();
		
		try
		{
			int numBands = inputImage->GetRasterCount();
			datasets = new GDALDataset*[1];
			datasets[0] = inputImage;
			
			imageMax = new double[numBands];
			imageMin = new double[numBands];
			outMax = new double[numBands];
			outMin = new double[numBands];
			
			stats = new ImageStats*[numBands];
			for(int i = 0; i < numBands; i++)
			{
				stats[i] = new ImageStats();
			}
			calcImageStats = new RSGISImageStatistics();
			calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, true, function, ignoreZeros);
			
			for(int i = 0; i < numBands; i++)
			{				
				imageMin[i] = stats[i]->mean - (stats[i]->stddev * 2);
				imageMax[i] = stats[i]->mean + (stats[i]->stddev * 2);
                if(imageMin[i] < stats[i]->min)
                {
                    imageMin[i] = stats[i]->min;
                }
                if(imageMax[i] > stats[i]->max)
                {
                    imageMax[i] = stats[i]->max;
                }
				outMax[i] = 255;
				outMin[i] = 0;
			}
			
			for(int i = 0; i < numBands; i++)
			{
				delete stats[i];
			}
			delete[] stats;
			delete calcImageStats;
			
			stretchImage = new RSGISFuncLinearStretchImage(numBands, imageMax, imageMin, outMax, outMin, function);
			calcImg = new RSGISCalcImage(stretchImage, "", true);
			calcImg->calcImage(datasets, 1, outputImage, false, NULL, imageFormat, outDataType);
			
		}
		catch(RSGISImageCalcException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw RSGISImageCalcException(e.what());
		}
		
		delete[] imageMax;
		delete[] imageMin;
		delete[] outMax;
		delete[] outMin;
		
		delete stretchImage;
		delete calcImg;
		delete function;
		
		if(datasets != NULL)
		{
			delete[] datasets;
		}
	}
	
	void RSGISStretchImage::executeLogrithmicStretch() throw(RSGISImageCalcException)
	{
		GDALDataset **datasets = NULL;
		RSGISImageStatistics *calcImageStats = NULL;
		ImageStats **stats = NULL;
		RSGISCalcImage *calcImg = NULL;
		RSGISFuncLinearStretchImage *stretchImage = NULL;
		double *imageMax = NULL;
		double *imageMin = NULL;
		double *outMax = NULL;
		double *outMin = NULL;
		
		RSGISMathFunction *function = new RSGISLogrithmStretchFunction();
		
		try
		{
			int numBands = inputImage->GetRasterCount();
			datasets = new GDALDataset*[1];
			datasets[0] = inputImage;
			
			imageMax = new double[numBands];
			imageMin = new double[numBands];
			outMax = new double[numBands];
			outMin = new double[numBands];
			
			stats = new ImageStats*[numBands];
			for(int i = 0; i < numBands; i++)
			{
				stats[i] = new ImageStats();
			}
			calcImageStats = new RSGISImageStatistics();
			calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, true, function, ignoreZeros);
			
			for(int i = 0; i < numBands; i++)
			{				
				imageMin[i] = stats[i]->mean - (stats[i]->stddev * 2);
				imageMax[i] = stats[i]->mean + (stats[i]->stddev * 2);
                if(imageMin[i] < stats[i]->min)
                {
                    imageMin[i] = stats[i]->min;
                }
                if(imageMax[i] > stats[i]->max)
                {
                    imageMax[i] = stats[i]->max;
                }
				outMax[i] = 255;
				outMin[i] = 0;
			}
			
			for(int i = 0; i < numBands; i++)
			{
				delete stats[i];
			}
			delete[] stats;
			delete calcImageStats;
			
			stretchImage = new RSGISFuncLinearStretchImage(numBands, imageMax, imageMin, outMax, outMin, function);
			calcImg = new RSGISCalcImage(stretchImage, "", true);
			calcImg->calcImage(datasets, 1, outputImage, false, NULL, imageFormat, outDataType);
			
		}
		catch(RSGISImageCalcException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw RSGISImageCalcException(e.what());
		}
		
		delete[] imageMax;
		delete[] imageMin;
		delete[] outMax;
		delete[] outMin;
		
		delete stretchImage;
		delete calcImg;
		delete function;
		
		if(datasets != NULL)
		{
			delete[] datasets;
		}
	}
	
	void RSGISStretchImage::executePowerLawStretch(float power) throw(RSGISImageCalcException)
	{
		GDALDataset **datasets = NULL;
		RSGISImageStatistics *calcImageStats = NULL;
		ImageStats **stats = NULL;
		RSGISCalcImage *calcImg = NULL;
		RSGISFuncLinearStretchImage *stretchImage = NULL;
		double *imageMax = NULL;
		double *imageMin = NULL;
		double *outMax = NULL;
		double *outMin = NULL;
		
		RSGISMathFunction *function = new RSGISPowerLawStretchFunction(power);
		
		try
		{
			int numBands = inputImage->GetRasterCount();
			datasets = new GDALDataset*[1];
			datasets[0] = inputImage;
			
			imageMax = new double[numBands];
			imageMin = new double[numBands];
			outMax = new double[numBands];
			outMin = new double[numBands];
			
			stats = new ImageStats*[numBands];
			for(int i = 0; i < numBands; i++)
			{
				stats[i] = new ImageStats();
			}
			calcImageStats = new RSGISImageStatistics();
			calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, true, function, ignoreZeros);
			
			for(int i = 0; i < numBands; i++)
			{				
				imageMin[i] = stats[i]->mean - (stats[i]->stddev * 2);
				imageMax[i] = stats[i]->mean + (stats[i]->stddev * 2);
                if(imageMin[i] < stats[i]->min)
                {
                    imageMin[i] = stats[i]->min;
                }
                if(imageMax[i] > stats[i]->max)
                {
                    imageMax[i] = stats[i]->max;
                }
				outMax[i] = 255;
				outMin[i] = 0;
			}
			
			for(int i = 0; i < numBands; i++)
			{
				delete stats[i];
			}
			delete[] stats;
			delete calcImageStats;
			
			stretchImage = new RSGISFuncLinearStretchImage(numBands, imageMax, imageMin, outMax, outMin, function);
			calcImg = new RSGISCalcImage(stretchImage, "", true);
			calcImg->calcImage(datasets, 1, outputImage, false, NULL, imageFormat, outDataType);
			
		}
		catch(RSGISImageCalcException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw RSGISImageCalcException(e.what());
		}
		
		delete[] imageMax;
		delete[] imageMin;
		delete[] outMax;
		delete[] outMin;
		
		delete stretchImage;
		delete calcImg;
		delete function;
		
		if(datasets != NULL)
		{
			delete[] datasets;
		}
	}
	
	RSGISStretchImage::~RSGISStretchImage()
	{
		
	}
	
	
	double RSGISExponentStretchFunction::calcFunction(double value) throw(RSGISMathException)
	{
		//cout << value << " = " << exp(value) << endl;
		float outVal = exp(value);
		if((boost::math::isinf)(outVal))
		{
			outVal = FLT_MAX;
		}
		return outVal;
	}
	
	double RSGISLogrithmStretchFunction::calcFunction(double value) throw(RSGISMathException)
	{
		float outVal = log(value);
		if(((boost::math::isinf)(outVal)) & (outVal < 0))
		{
			outVal = FLT_MIN;
		}
		return outVal;
	}
	
	double RSGISPowerLawStretchFunction::calcFunction(double value) throw(RSGISMathException)
	{
		return pow(value,((double)power));
	}
	
	
	RSGISLinearStretchImage::RSGISLinearStretchImage(int numberOutBands, double *imageMaxIn, double *imageMinIn, double *outMaxIn, double *outMinIn) : RSGISCalcImageValue(numberOutBands)
	{
		this->imageMax = imageMaxIn;
		this->imageMin = imageMinIn;
		this->outMax = outMaxIn;
		this->outMin = outMinIn;
	}
	
	void RSGISLinearStretchImage::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
	{
		double inDiff = 0;
		double norm2min = 0;
		double outDiff = 0;
		for(int i = 0; i < numBands; i++)
		{
			if((boost::math::isnan)(bandValues[i]))
			{
				output[i] = outMin[i];
			}
			else if(bandValues[i] < imageMin[i])
			{
				output[i] = outMin[i];
			}
			else if(bandValues[i] > imageMax[i])
			{
				output[i] = outMax[i];
			}
			else 
			{
				inDiff = imageMax[i] - imageMin[i];
				norm2min = bandValues[i] - imageMin[i];
				outDiff = outMax[i] - outMin[i];
				output[i] = ((norm2min/inDiff)*outDiff)+outMin[i];
			}
		}
	}
	
	RSGISLinearStretchImage::~RSGISLinearStretchImage()
	{
		
	}

	
	
	RSGISFuncLinearStretchImage::RSGISFuncLinearStretchImage(int numberOutBands, double *imageMaxIn, double *imageMinIn, double *outMaxIn, double *outMinIn, RSGISMathFunction *func) : RSGISCalcImageValue(numberOutBands)
	{
		this->imageMax = imageMaxIn;
		this->imageMin = imageMinIn;
		this->outMax = outMaxIn;
		this->outMin = outMinIn;
		this->func = func;
	}
	
	void RSGISFuncLinearStretchImage::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
	{
		for(int i = 0; i < numBands; i++)
		{
			if(!(boost::math::isnan)(bandValues[i]))
			{
				bandValues[i] = func->calcFunction(bandValues[i]);
			}
		}
		
		
		double inDiff = 0;
		double norm2min = 0;
		double outDiff = 0;
		for(int i = 0; i < numBands; i++)
		{
			if((boost::math::isnan)(bandValues[i]))
			{
				output[i] = outMin[i];
			}
			else if(bandValues[i] < imageMin[i])
			{
				output[i] = outMin[i];
			}
			else if(bandValues[i] > imageMax[i])
			{
				output[i] = outMax[i];
			}
			else 
			{
				inDiff = imageMax[i] - imageMin[i];
				norm2min = bandValues[i] - imageMin[i];
				outDiff = outMax[i] - outMin[i];
				output[i] = ((norm2min/inDiff)*outDiff)+outMin[i];
			}
		}
	}
	
	RSGISFuncLinearStretchImage::~RSGISFuncLinearStretchImage()
	{
		
	}
	
	
}}


