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
#include <vector>
#include <algorithm>

#include "common/RSGISImageException.h"

#include "filtering/RSGISImageFilterException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "filtering/RSGISImageFilter.h"

#include "datastruct/SortedGenericList.cpp"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_filter_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace filter{

	class DllExport RSGISMeanFilter : public RSGISImageFilter
		{
		public:
			RSGISMeanFilter(int numberOutBands, int size, std::string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
			virtual void exportAsImage(std::string filename);
			~RSGISMeanFilter();
		};

	class DllExport RSGISMedianFilter : public RSGISImageFilter
		{
		public:
			RSGISMedianFilter(int numberOutBands, int size, std::string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
			virtual void exportAsImage(std::string filename);
			~RSGISMedianFilter();
		};

	class DllExport RSGISModeFilter : public RSGISImageFilter
		{
		public:
			RSGISModeFilter(int numberOutBands, int size, std::string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
			virtual void exportAsImage(std::string filename);
			~RSGISModeFilter();
		};

	class DllExport RSGISRangeFilter : public RSGISImageFilter
		{
		public:
			RSGISRangeFilter(int numberOutBands, int size, std::string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
			virtual void exportAsImage(std::string filename);
			~RSGISRangeFilter();
		};

	class DllExport RSGISStdDevFilter : public RSGISImageFilter
		{
		public:
			RSGISStdDevFilter(int numberOutBands, int size, std::string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
			virtual void exportAsImage(std::string filename);
			~RSGISStdDevFilter();
		};

    class DllExport RSGISCoeffOfVarFilter : public RSGISImageFilter
		{
        /**

        Coefficient of variation filter.
        Ratio of standard devation to the mean.
        See http://en.wikipedia.org/wiki/Coefficient_of_variation
        */
		public:
			RSGISCoeffOfVarFilter(int numberOutBands, int size, std::string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
			virtual void exportAsImage(std::string filename);
			~RSGISCoeffOfVarFilter();
		};

	class DllExport RSGISMinFilter : public RSGISImageFilter
		{
		public:
			RSGISMinFilter(int numberOutBands, int size, std::string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
			virtual void exportAsImage(std::string filename);
			~RSGISMinFilter();
		};

	class DllExport RSGISMaxFilter : public RSGISImageFilter
		{
		public:
			RSGISMaxFilter(int numberOutBands, int size, std::string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
			virtual void exportAsImage(std::string filename);
			~RSGISMaxFilter();
		};

	class DllExport RSGISTotalFilter : public RSGISImageFilter
		{
		public:
			RSGISTotalFilter(int numberOutBands, int size, std::string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
			virtual void exportAsImage(std::string filename);
			~RSGISTotalFilter();
		};

	class DllExport RSGISKuwaharaFilter : public RSGISImageFilter
		{
		public:
			RSGISKuwaharaFilter(int numberOutBands, int size, std::string filenameEnding);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
			virtual void exportAsImage(std::string filename);
			~RSGISKuwaharaFilter();
		};
    
    class DllExport RSGISMeanDiffFilter : public RSGISImageFilter
    {
    public:
        RSGISMeanDiffFilter(int numberOutBands, int size, std::string filenameEnding);
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
        virtual void exportAsImage(std::string filename);
        ~RSGISMeanDiffFilter();
    };
    
    class DllExport RSGISMeanDiffAbsFilter : public RSGISImageFilter
    {
    public:
        RSGISMeanDiffAbsFilter(int numberOutBands, int size, std::string filenameEnding);
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
        virtual void exportAsImage(std::string filename);
        ~RSGISMeanDiffAbsFilter();
    };
    
    class DllExport RSGISTotalDiffFilter : public RSGISImageFilter
    {
    public:
        RSGISTotalDiffFilter(int numberOutBands, int size, std::string filenameEnding);
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
        virtual void exportAsImage(std::string filename);
        ~RSGISTotalDiffFilter();
    };
    
    class DllExport RSGISTotalDiffAbsFilter : public RSGISImageFilter
    {
    public:
        RSGISTotalDiffAbsFilter(int numberOutBands, int size, std::string filenameEnding);
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
        virtual void exportAsImage(std::string filename);
        ~RSGISTotalDiffAbsFilter();
    };
}}

#endif


