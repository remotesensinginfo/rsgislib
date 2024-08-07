/*
 *  RSGISBandMath.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 13/11/2008.
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

#ifndef RSGISBandMath_H
#define RSGISBandMath_H

#include <iostream>
#include <string>
#include <cmath>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "muParser.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_img_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace img{
	
	struct DllExport VariableBands
	{
        std::string name;
		int band;
	};
	
	class DllExport RSGISBandMath : public RSGISCalcImageValue
		{
		public: 
			RSGISBandMath(int numberOutBands, VariableBands **variables, int numVariables, mu::Parser *muParser);
			void calcImageValue(float *bandValues, int numBands, double *output);
			~RSGISBandMath();
		private:
			VariableBands **variables;
			int numVariables;
            mu::Parser *muParser;
            mu::value_type *inVals;
		};
    
    
    class DllExport RSGISCalcPropExpTruePxls : public RSGISCalcImageValue
    {
    public:
        RSGISCalcPropExpTruePxls(VariableBands **variables, int numVariables, mu::Parser *muParser, bool useMask);
        void calcImageValue(float *bandValues, int numBands);
        float getPropPxlVal();
        ~RSGISCalcPropExpTruePxls();
    private:
        VariableBands **variables;
        int numVariables;
        mu::Parser *muParser;
        mu::value_type *inVals;
        bool useMask;
        double truePxlCount;
        double totalPxlCount;
    };
	
}}

#endif

