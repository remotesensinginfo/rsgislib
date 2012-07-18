/*
 *  RSGISFilterBank.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/12/2008.
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

#include "RSGISFilterBank.h"

namespace rsgis{namespace filter{
	

	RSGISFilterBank::RSGISFilterBank()
	{
		this->filters = new std::vector<RSGISImageFilter*>();
	}
	
	void RSGISFilterBank::addFilter(RSGISImageFilter *filter)
	{
		this->filters->push_back(filter);
	}
	
	RSGISImageFilter* RSGISFilterBank::getFilter(int i)
	{
		return this->filters->at(i);
	}
	
	int RSGISFilterBank::getNumFilters()
	{
		return this->filters->size();
	}
	
	void RSGISFilterBank::executeFilters(GDALDataset **datasets, int numDS, string outImageBase) throw(rsgis::RSGISImageException)
	{
		try
		{
			int numOutBands = 0;
			for(int i = 0; i < numDS; i++)
			{
				numOutBands += datasets[i]->GetRasterCount();
			}
			
			int size = this->filters->size();
			string filename = "";
			for(int i = 0; i < size; i++)
			{
				cout << "Executing filter " << (i+1) << " of " << size << endl;
				filename = outImageBase + string("_") + this->filters->at(i)->getFileNameEnding() + string(".env");
				dynamic_cast<rsgis::img::RSGISCalcImageValue*>(this->filters->at(i))->setNumOutBands(numOutBands);
				this->filters->at(i)->runFilter(datasets, numDS, filename);
			}
		}
		catch(rsgis::RSGISImageException e)
		{
			throw e;
		}
	}
	
	void RSGISFilterBank::exectuteFilter(int i, GDALDataset **datasets, int numDS, string outImageBase) throw(rsgis::RSGISImageException)
	{
		try
		{
			string filename = outImageBase + this->filters->at(i)->getFileNameEnding();
			this->filters->at(i)->runFilter(datasets, numDS, filename);
		}
		catch(rsgis::RSGISImageException e)
		{
			throw e;
		}
	}
	
	void RSGISFilterBank::exportFilterBankImages(string imagebase)
	{
		int size = this->filters->size();
		string filename = "";
		for(int i = 0; i < size; i++)
		{
			filename = imagebase + this->filters->at(i)->getFileNameEnding();
			this->filters->at(i)->exportAsImage(filename);
		}
	}
	
	void RSGISFilterBank::createLeungMalikFilterBank() throw(rsgis::RSGISImageException)
	{
		float stddev = 0;
		float stddevX = 0;
		float stddevY = 0;
		float angle = 0;
		int size = 51;
		string fileEnding = "";
		string angleStr = "";
		RSGISGenerateFilter *genFilter = NULL;
		RSGISImageFilter *filter = NULL;
		ImageFilter *filterKernal = NULL;
		
		RSGISCalcLapacianFilter *calcLapacianFilter = NULL;
		RSGISCalcGaussianSmoothFilter *calcGaussianSmoothFilter = NULL;
		RSGISCalcGaussianFirstDerivativeFilter *calcGaussian1stDerivFilter = NULL;
		RSGISCalcGaussianSecondDerivativeFilter *calcGaussian2ndDerivFilter = NULL;
		
		try
		{
			// GENERATE LAPACIAN FILTERS
			for(int i = 0; i < 8; i++)
			{
				if(i == 0)
				{
					stddev = 1;
					fileEnding = "lap_1";
				}
				else if(i == 1)
				{
					stddev = sqrt(2.0);
					fileEnding = "lap_sqrt2";
				}
				else if(i == 2)
				{
					stddev = 2;
					fileEnding = "lap_2";
				}
				else if(i == 3)
				{
					stddev = 2*sqrt(2.0);
					fileEnding = "lap_2sqrt2";
				}
				else if(i == 4)
				{
					stddev = 3;
					fileEnding = "lap_3";
				}
				else if(i == 5)
				{
					stddev = 3*sqrt(2.0);
					fileEnding = "lap_3sqrt2";
				}
				else if(i == 6)
				{
					stddev = 6;
					fileEnding = "lap_6";
				}
				else if(i == 7)
				{
					stddev = 6*sqrt(2.0);
					fileEnding = "lap_6sqrt2";
				}
				
				calcLapacianFilter = new RSGISCalcLapacianFilter(stddev);
				genFilter = new RSGISGenerateFilter(calcLapacianFilter);
				filterKernal = genFilter->generateFilter(size);
				filter = new RSGISImageKernelFilter(0, size, fileEnding, filterKernal);
				this->addFilter(filter);
				delete calcLapacianFilter;
				delete genFilter;
			}
			
			
			// Gaussian Smoothing Filters
			angle = 0;
			for(int i = 0; i < 4; i++)
			{
				if(i == 0)
				{
					stddevX = 1;
					stddevY = 1;
					fileEnding = "gausmooth1";
				}
				else if(i == 1)
				{
					stddevX = sqrt(2.0);
					stddevY = sqrt(2.0);
					fileEnding = "gausmoothsqrt2";
				}
				else if(i == 2)
				{
					stddevX = 2;
					stddevY = 2;
					fileEnding = "gausmooth2";
				}
				else if(i == 3)
				{
					stddevX = 2*sqrt(2.0);
					stddevY = 2*sqrt(2.0);
					fileEnding = "gausmooth2sqrt2";
				}
				
				calcGaussianSmoothFilter = new RSGISCalcGaussianSmoothFilter(stddevX, stddevY, angle);
				genFilter = new RSGISGenerateFilter(calcGaussianSmoothFilter);
				filterKernal = genFilter->generateFilter(size);
				filter = new RSGISImageKernelFilter(0, size, fileEnding, filterKernal);
				this->addFilter(filter);
				delete calcGaussianSmoothFilter;
				delete genFilter;
			}
			
			//Gaussain First Filters
			for(int i = 0; i < 6; i++)
			{
				if(i == 0)
				{
					angle = 0;
					angleStr = "0";
				}
				else if(i == 1)
				{
					angle = 30 * (M_PI/180);
					angleStr = "30";
				}
				else if(i == 2)
				{
					angle = 60 * (M_PI/180);
					angleStr = "60";
				}
				else if(i == 3)
				{
					angle = 90 * (M_PI/180);
					angleStr = "90";
				}
				else if(i == 4)
				{
					angle = 120 * (M_PI/180);
					angleStr = "120";
				}
				else if(i == 5)
				{
					angle = 150 * (M_PI/180);
					angleStr = "150";
				}
				
				
				for(int j = 0; j < 3; j++)
				{
					if(j == 0)
					{
						stddevX = 3;
						stddevY = 1;
						fileEnding = "gau1st_3_1_" + angleStr;
					}
					else if(j == 1)
					{
						stddevX = 3*sqrt(2.0);
						stddevY = sqrt(2.0);
						fileEnding = "gau1st_3sqrt1_sqrt2_" + angleStr;
					}
					else if(j == 2)
					{
						stddevX = 6;
						stddevY = 2;
						fileEnding = "gau1st_6_2_" + angleStr;
					}
					
					calcGaussian1stDerivFilter = new RSGISCalcGaussianFirstDerivativeFilter(stddevX, stddevY, angle);
					genFilter = new RSGISGenerateFilter(calcGaussian1stDerivFilter);
					filterKernal = genFilter->generateFilter(size);
					filter = new RSGISImageKernelFilter(0, size, fileEnding, filterKernal);
					this->addFilter(filter);
					delete calcGaussian1stDerivFilter;
					delete genFilter;				
				}
			}
			
			//Gaussain Second Filters
			for(int i = 0; i < 6; i++)
			{
				if(i == 0)
				{
					angle = 0;
					angleStr = "0";
				}
				else if(i == 1)
				{
					angle = 30 * (M_PI/180);
					angleStr = "30";
				}
				else if(i == 2)
				{
					angle = 60 * (M_PI/180);
					angleStr = "60";
				}
				else if(i == 3)
				{
					angle = 90 * (M_PI/180);
					angleStr = "90";
				}
				else if(i == 4)
				{
					angle = 120 * (M_PI/180);
					angleStr = "120";
				}
				else if(i == 5)
				{
					angle = 150 * (M_PI/180);
					angleStr = "150";
				}
				
				
				for(int j = 0; j < 3; j++)
				{
					if(j == 0)
					{
						stddevX = 3;
						stddevY = 1;
						fileEnding = "gau2nd_3_1_" + angleStr;
					}
					else if(j == 1)
					{
						stddevX = 3*sqrt(2.0);
						stddevY = sqrt(2.0);
						fileEnding = "gau2nd_3sqrt1_sqrt2_" + angleStr;
					}
					else if(j == 2)
					{
						stddevX = 6;
						stddevY = 2;
						fileEnding = "gau2nd_6_2_" + angleStr;
					}
					
					calcGaussian2ndDerivFilter = new RSGISCalcGaussianSecondDerivativeFilter(stddevX, stddevY, angle);
					genFilter = new RSGISGenerateFilter(calcGaussian2ndDerivFilter);
					filterKernal = genFilter->generateFilter(size);
					filter = new RSGISImageKernelFilter(0, size, fileEnding, filterKernal);
					this->addFilter(filter);
					delete calcGaussian2ndDerivFilter;
					delete genFilter;				
				}
			}
			
			
		}
		catch (rsgis::RSGISImageException e)
		{
			throw e;
		}
		
		
	}
			
	RSGISFilterBank::~RSGISFilterBank()
	{
		
	}
	
}}

