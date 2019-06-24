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
	
	void RSGISFilterBank::executeFilters(GDALDataset **datasets, int numDS, std::string outImageBase, std::string gdalFormat, std::string imgExt, GDALDataType outDataType)
	{
		try
		{
			int numOutBands = 0;
			for(int i = 0; i < numDS; i++)
			{
				numOutBands += datasets[i]->GetRasterCount();
			}
			
			int size = this->filters->size();
			std::string filename = "";
			for(int i = 0; i < size; i++)
			{
				std::cout << "Executing filter " << (i+1) << " of " << size << std::endl;
				filename = outImageBase + this->filters->at(i)->getFileNameEnding() + "." + imgExt;
				dynamic_cast<rsgis::img::RSGISCalcImageValue*>(this->filters->at(i))->setNumOutBands(numOutBands);
				this->filters->at(i)->runFilter(datasets, numDS, filename, gdalFormat, outDataType);
			}
		}
		catch(rsgis::RSGISImageException &e)
		{
			throw e;
		}
	}
	
	void RSGISFilterBank::exectuteFilter(int i, GDALDataset **datasets, int numDS, std::string outImageBase, std::string gdalFormat, GDALDataType outDataType)
	{
		try
		{
			std::string filename = outImageBase + this->filters->at(i)->getFileNameEnding();
			this->filters->at(i)->runFilter(datasets, numDS, filename, gdalFormat, outDataType);
		}
		catch(rsgis::RSGISImageException &e)
		{
			throw e;
		}
	}
	
	void RSGISFilterBank::exportFilterBankImages(std::string imagebase)
	{
		int size = this->filters->size();
		std::string filename = "";
		for(int i = 0; i < size; i++)
		{
			filename = imagebase + this->filters->at(i)->getFileNameEnding();
			this->filters->at(i)->exportAsImage(filename);
		}
	}
			
	RSGISFilterBank::~RSGISFilterBank()
	{
		
	}
	
}}

