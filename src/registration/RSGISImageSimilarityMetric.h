 /*
 *  RSGISImageSimilarityMetric.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 31/08/2010.
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

#ifndef RSGISImageSimilarityMetric_H
#define RSGISImageSimilarityMetric_H

#include <cmath>

#include "math/RSGISMathException.h"
#include "img/RSGISCalcImageValue.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_registration_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace reg{
    		
	class DllExport RSGISImageSimilarityMetric
	{
	public:
		virtual float calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims)=0;
		virtual bool findMin()=0;
		virtual ~RSGISImageSimilarityMetric(){};
	};

    class DllExport RSGISImageCalcSimilarityMetric : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISImageCalcSimilarityMetric(std::vector<unsigned int> imgABands, std::vector<unsigned int> imgBBands, float imgANoData, bool useImgANoData, float imgBNoData, bool useImgBNoData):rsgis::img::RSGISCalcImageValue(0)
        {
            this->imgABands = imgABands;
            this->imgBBands = imgBBands;
            this->imgANoData = imgANoData;
            this->useImgANoData = useImgANoData;
            this->imgBNoData = imgBNoData;
            this->useImgBNoData = useImgBNoData;

            if(imgABands.size() != imgBBands.size())
            {
                throw rsgis::RSGISException("There are a different number of bands for the two images.");
            }

            this->nBands = imgABands.size();
        };
        virtual bool findMin()=0;
        virtual void reset()=0;
        virtual double metricVal()=0;
    protected:
        std::vector<unsigned int> imgABands;
        std::vector<unsigned int> imgBBands;
        float imgANoData;
        bool useImgANoData;
        float imgBNoData;
        bool useImgBNoData;
        unsigned int nBands;
    };
}}
									  
#endif
