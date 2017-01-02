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

	RSGISStretchImage::RSGISStretchImage(GDALDataset *inputImage, std::string outputImage,  bool outStats, std::string outStatsFile, bool ignoreZeros, bool onePassSD, std::string imageFormat, GDALDataType outDataType): inputImage(NULL), outputImage(""), ignoreZeros(true)
	{
		this->inputImage = inputImage;
		this->outputImage = outputImage;
        this->outStats = outStats;
        this->outStatsFile = outStatsFile;
        this->ignoreZeros = ignoreZeros;
        this->onePassSD = onePassSD;
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
            
			calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, false, ignoreZeros, onePassSD);
			
            std::ofstream outTxtFile;
            if(this->outStats)
            {
                outTxtFile.open(this->outStatsFile.c_str());
                if(!outTxtFile.is_open())
                {
                    throw RSGISImageCalcException("Output file for the statistics could not be opened.");
                }
                outTxtFile << "#linear\n";
                outTxtFile << "#band,img_min,img_max,out_min,out_max\n";
            }
            
			for(int i = 0; i < numBands; i++)
			{
				imageMin[i] = stats[i]->min;
				imageMax[i] = stats[i]->max;
				outMax[i] = 255;
				outMin[i] = 0;
                
                if(this->outStats)
                {
                    outTxtFile << i+1 << "," << imageMin[i] << "," << imageMax[i] << "," << outMin[i] << "," << outMax[i] << std::endl;
                }
			}
            
            if(this->outStats)
            {
                outTxtFile.flush();
                outTxtFile.close();
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
			calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, false, ignoreZeros, onePassSD);
			
			double onePercent = 0;
			double onePercentUpper = 0;
			double onePercentLower = 0;
			
            std::ofstream outTxtFile;
            if(this->outStats)
            {
                outTxtFile.open(this->outStatsFile.c_str());
                if(!outTxtFile.is_open())
                {
                    throw RSGISImageCalcException("Output file for the statistics could not be opened.");
                }
                outTxtFile << "#percent\n";
                outTxtFile << "#band,img_min,img_max,out_min,out_max\n";
            }
            
			for(int i = 0; i < numBands; i++)
			{				
				onePercent = (stats[i]->max - stats[i]->min)/100;
				
				onePercentUpper = (stats[i]->max - stats[i]->mean)/50;
				onePercentLower = (stats[i]->mean - stats[i]->min)/50;
				
				imageMin[i] = stats[i]->min + (onePercentLower * percent);
				imageMax[i] = stats[i]->max - (onePercentUpper * percent);
				outMax[i] = 255;
				outMin[i] = 0;
                
                if(this->outStats)
                {
                    outTxtFile << i+1 << "," << imageMin[i] << "," << imageMax[i] << "," << outMin[i] << "," << outMax[i] << std::endl;
                }
			}
            
            if(this->outStats)
            {
                outTxtFile.flush();
                outTxtFile.close();
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
			calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, true, ignoreZeros, onePassSD);
			
            std::ofstream outTxtFile;
            if(this->outStats)
            {
                outTxtFile.open(this->outStatsFile.c_str());
                if(!outTxtFile.is_open())
                {
                    throw RSGISImageCalcException("Output file for the statistics could not be opened.");
                }
                outTxtFile << "#stddev\n";
                outTxtFile << "#band,img_min,img_max,out_min,out_max\n";
            }
            
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
                
                std::cout << "Band[" << i+1 << "] Min = " << stats[i]->min << " Mean = " << stats[i]->mean << " (Std Dev = " << stats[i]->stddev << ") max = " << stats[i]->max << std::endl;
                
				outMax[i] = 255;
				outMin[i] = 0;
                
                if(this->outStats)
                {
                    outTxtFile << i+1 << "," << imageMin[i] << "," << imageMax[i] << "," << outMin[i] << "," << outMax[i] << std::endl;
                }
			}
            
            if(this->outStats)
            {
                outTxtFile.flush();
                outTxtFile.close();
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
		
		rsgis::math::RSGISMathFunction *function = new RSGISExponentStretchFunction();
		
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
			calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, true, function, ignoreZeros, onePassSD);
			
            std::ofstream outTxtFile;
            if(this->outStats)
            {
                outTxtFile.open(this->outStatsFile.c_str());
                if(!outTxtFile.is_open())
                {
                    throw RSGISImageCalcException("Output file for the statistics could not be opened.");
                }
                outTxtFile << "#exp\n";
                outTxtFile << "#band,img_min,img_max,out_min,out_max\n";
            }
            
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
                
                if(this->outStats)
                {
                    outTxtFile << i+1 << "," << imageMin[i] << "," << imageMax[i] << "," << outMin[i] << "," << outMax[i] << std::endl;
                }
			}
            
            if(this->outStats)
            {
                outTxtFile.flush();
                outTxtFile.close();
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
		
		rsgis::math::RSGISMathFunction *function = new RSGISLogrithmStretchFunction();
		
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
			calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, true, function, ignoreZeros, onePassSD);
			
            std::ofstream outTxtFile;
            if(this->outStats)
            {
                outTxtFile.open(this->outStatsFile.c_str());
                if(!outTxtFile.is_open())
                {
                    throw RSGISImageCalcException("Output file for the statistics could not be opened.");
                }
                outTxtFile << "#log\n";
                outTxtFile << "#band,img_min,img_max,out_min,out_max\n";
            }
            
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
                
                if(this->outStats)
                {
                    outTxtFile << i+1 << "," << imageMin[i] << "," << imageMax[i] << "," << outMin[i] << "," << outMax[i] << std::endl;
                }
			}
            
            if(this->outStats)
            {
                outTxtFile.flush();
                outTxtFile.close();
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
		
		rsgis::math::RSGISMathFunction *function = new RSGISPowerLawStretchFunction(power);
		
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
			calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, true, function, ignoreZeros, onePassSD);
			
            std::ofstream outTxtFile;
            if(this->outStats)
            {
                outTxtFile.open(this->outStatsFile.c_str());
                if(!outTxtFile.is_open())
                {
                    throw RSGISImageCalcException("Output file for the statistics could not be opened.");
                }
                outTxtFile << "#powerlaw\n";
                outTxtFile << "#band,img_min,img_max,out_min,out_max\n";
            }
            
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
            
            if(this->outStats)
            {
                outTxtFile.flush();
                outTxtFile.close();
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
    
    
    
    
    
    
    RSGISStretchImageWithStats::RSGISStretchImageWithStats(GDALDataset *inputImage, std::string outputImage, std::string inStatsFile, std::string imageFormat, GDALDataType outDataType): inputImage(NULL), outputImage("")
	{
		this->inputImage = inputImage;
		this->outputImage = outputImage;
        this->inStatsFile = inStatsFile;
        this->imageFormat = imageFormat;
        this->outDataType = outDataType;
	}
	
	void RSGISStretchImageWithStats::executeLinearMinMaxStretch() throw(RSGISImageCalcException)
	{
		GDALDataset **datasets = NULL;
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
			
            std::vector<BandSpecThresholdStats> *stats = this->readBandSpecThresholds(this->inStatsFile);
            
			for(int i = 0; i < numBands; i++)
			{
				imageMin[i] = stats->at(i).origMin;
				imageMax[i] = stats->at(i).origMax;
				outMax[i] = stats->at(i).imgMax;
				outMin[i] = stats->at(i).imgMin;
                
                std::cout << "Band[" << i+1 << "] Image Min = " << imageMin[i] << " Image Max = " << imageMax[i] << " Output Min = " << outMin[i] << " Output Max = " << outMax[i] << std::endl;
			}
            
			delete stats;
			
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
	
	void RSGISStretchImageWithStats::executeHistogramStretch() throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Histogram Stretch is not implmented yet\n");
	}
	
	void RSGISStretchImageWithStats::executeExponentialStretch() throw(RSGISImageCalcException)
	{
		GDALDataset **datasets = NULL;
		RSGISCalcImage *calcImg = NULL;
		RSGISFuncLinearStretchImage *stretchImage = NULL;
		double *imageMax = NULL;
		double *imageMin = NULL;
		double *outMax = NULL;
		double *outMin = NULL;
		
		rsgis::math::RSGISMathFunction *function = new RSGISExponentStretchFunction();
		
		try
		{
			int numBands = inputImage->GetRasterCount();
			datasets = new GDALDataset*[1];
			datasets[0] = inputImage;
			
			imageMax = new double[numBands];
			imageMin = new double[numBands];
			outMax = new double[numBands];
			outMin = new double[numBands];
			
			std::vector<BandSpecThresholdStats> *stats = this->readBandSpecThresholds(this->inStatsFile);
            
			for(int i = 0; i < numBands; i++)
			{
				imageMin[i] = stats->at(i).origMin;
				imageMax[i] = stats->at(i).origMax;
				outMax[i] = stats->at(i).imgMax;
				outMin[i] = stats->at(i).imgMin;
                
                std::cout << "Band[" << i+1 << "] Image Min = " << imageMin[i] << " Image Max = " << imageMax[i] << " Output Min = " << outMin[i] << " Output Max = " << outMax[i] << std::endl;
			}
            
			delete stats;
			
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
	
	void RSGISStretchImageWithStats::executeLogrithmicStretch() throw(RSGISImageCalcException)
	{
		GDALDataset **datasets = NULL;
		RSGISCalcImage *calcImg = NULL;
		RSGISFuncLinearStretchImage *stretchImage = NULL;
		double *imageMax = NULL;
		double *imageMin = NULL;
		double *outMax = NULL;
		double *outMin = NULL;
		
		rsgis::math::RSGISMathFunction *function = new RSGISLogrithmStretchFunction();
		
		try
		{
			int numBands = inputImage->GetRasterCount();
			datasets = new GDALDataset*[1];
			datasets[0] = inputImage;
			
			imageMax = new double[numBands];
			imageMin = new double[numBands];
			outMax = new double[numBands];
			outMin = new double[numBands];
			
			std::vector<BandSpecThresholdStats> *stats = this->readBandSpecThresholds(this->inStatsFile);
            
			for(int i = 0; i < numBands; i++)
			{
				imageMin[i] = stats->at(i).origMin;
				imageMax[i] = stats->at(i).origMax;
				outMax[i] = stats->at(i).imgMax;
				outMin[i] = stats->at(i).imgMin;
                
                std::cout << "Band[" << i+1 << "] Image Min = " << imageMin[i] << " Image Max = " << imageMax[i] << " Output Min = " << outMin[i] << " Output Max = " << outMax[i] << std::endl;
			}
            
			delete stats;
			
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
	
	void RSGISStretchImageWithStats::executePowerLawStretch(float power) throw(RSGISImageCalcException)
	{
		GDALDataset **datasets = NULL;
		RSGISCalcImage *calcImg = NULL;
		RSGISFuncLinearStretchImage *stretchImage = NULL;
		double *imageMax = NULL;
		double *imageMin = NULL;
		double *outMax = NULL;
		double *outMin = NULL;
		
		rsgis::math::RSGISMathFunction *function = new RSGISPowerLawStretchFunction(power);
		
		try
		{
			int numBands = inputImage->GetRasterCount();
			datasets = new GDALDataset*[1];
			datasets[0] = inputImage;
			
			imageMax = new double[numBands];
			imageMin = new double[numBands];
			outMax = new double[numBands];
			outMin = new double[numBands];
			
			std::vector<BandSpecThresholdStats> *stats = this->readBandSpecThresholds(this->inStatsFile);
            
			for(int i = 0; i < numBands; i++)
			{
				imageMin[i] = stats->at(i).origMin;
				imageMax[i] = stats->at(i).origMax;
				outMax[i] = stats->at(i).imgMax;
				outMin[i] = stats->at(i).imgMin;
                
                std::cout << "Band[" << i+1 << "] Image Min = " << imageMin[i] << " Image Max = " << imageMax[i] << " Output Min = " << outMin[i] << " Output Max = " << outMax[i] << std::endl;
			}
            
			delete stats;
			
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
	
	RSGISStretchImageWithStats::~RSGISStretchImageWithStats()
	{
		
	}
    
    
    
	
	
	double RSGISExponentStretchFunction::calcFunction(double value) throw(rsgis::math::RSGISMathException)
	{
		float outVal = exp(value);
		if((boost::math::isinf)(outVal))
		{
			outVal = FLT_MAX;
		}
		return outVal;
	}
	
	double RSGISLogrithmStretchFunction::calcFunction(double value) throw(rsgis::math::RSGISMathException)
	{
		float outVal = log(value);
		if(((boost::math::isinf)(outVal)) & (outVal < 0))
		{
			outVal = FLT_MIN;
		}
		return outVal;
	}
	
	double RSGISPowerLawStretchFunction::calcFunction(double value) throw(rsgis::math::RSGISMathException)
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
	
	void RSGISLinearStretchImage::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
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

	
	
	RSGISFuncLinearStretchImage::RSGISFuncLinearStretchImage(int numberOutBands, double *imageMaxIn, double *imageMinIn, double *outMaxIn, double *outMinIn, rsgis::math::RSGISMathFunction *func) : RSGISCalcImageValue(numberOutBands)
	{
		this->imageMax = imageMaxIn;
		this->imageMin = imageMinIn;
		this->outMax = outMaxIn;
		this->outMin = outMinIn;
		this->func = func;
	}
	
	void RSGISFuncLinearStretchImage::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
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


