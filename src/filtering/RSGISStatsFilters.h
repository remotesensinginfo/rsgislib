/*
 *  RSGISStatsFilters.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/12/2008.
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

#ifndef RSGISStatsFilters_H
#define RSGISStatsFilters_H

#include <iostream>

#include "common/RSGISImageException.h"

#include "filtering/RSGISImageFilterException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "filtering/RSGISImageFilter.h"

#include "datastruct/SortedGenericList.cpp"

using namespace rsgis;
using namespace rsgis::datastruct;
using namespace rsgis::img;

namespace rsgis{namespace filter{
	
	class RSGISMeanFilter : public RSGISImageFilter
		{
		public: 
			RSGISMeanFilter(int numberOutBands, int size, string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);					
			virtual void exportAsImage(string filename) throw(RSGISImageFilterException);
			~RSGISMeanFilter();
		};
	
	class RSGISMedianFilter : public RSGISImageFilter
		{
		public: 
			RSGISMedianFilter(int numberOutBands, int size, string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);					
			virtual void exportAsImage(string filename) throw(RSGISImageFilterException);
			~RSGISMedianFilter();
		};
	
	class RSGISModeFilter : public RSGISImageFilter
		{
		public: 
			RSGISModeFilter(int numberOutBands, int size, string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);					
			virtual void exportAsImage(string filename) throw(RSGISImageFilterException);
			~RSGISModeFilter();
		};
	
	class RSGISRangeFilter : public RSGISImageFilter
		{
		public: 
			RSGISRangeFilter(int numberOutBands, int size, string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);					
			virtual void exportAsImage(string filename) throw(RSGISImageFilterException);
			~RSGISRangeFilter();
		};
	
	class RSGISStdDevFilter : public RSGISImageFilter
		{
		public: 
			RSGISStdDevFilter(int numberOutBands, int size, string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);					
			virtual void exportAsImage(string filename) throw(RSGISImageFilterException);
			~RSGISStdDevFilter();
		};
	
	class RSGISMinFilter : public RSGISImageFilter
		{
		public: 
			RSGISMinFilter(int numberOutBands, int size, string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);					
			virtual void exportAsImage(string filename) throw(RSGISImageFilterException);
			~RSGISMinFilter();
		};
	
	class RSGISMaxFilter : public RSGISImageFilter
		{
		public: 
			RSGISMaxFilter(int numberOutBands, int size, string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);					
			virtual void exportAsImage(string filename) throw(RSGISImageFilterException);
			~RSGISMaxFilter();
		};
	
	class RSGISTotalFilter : public RSGISImageFilter
		{
		public: 
			RSGISTotalFilter(int numberOutBands, int size, string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);					
			virtual void exportAsImage(string filename) throw(RSGISImageFilterException);
			~RSGISTotalFilter();
		};
	
	class RSGISKuwaharaFilter : public RSGISImageFilter
		{
		public: 
			RSGISKuwaharaFilter(int numberOutBands, int size, string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);					
			virtual void exportAsImage(string filename) throw(RSGISImageFilterException);
			~RSGISKuwaharaFilter();
		};
}}

#endif


