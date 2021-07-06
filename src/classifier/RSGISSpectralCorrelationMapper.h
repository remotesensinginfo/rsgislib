 /*
 *  RSGISSpectralCorrelationMapper.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 14/02/2010.
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

#ifndef RSGISSpectralCorrelationMapper_H
#define RSGISSpectralCorrelationMapper_H

#include <math.h>
#include <gsl/gsl_matrix.h>

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_classify_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis { namespace classifier {
	    
	/**
	 This implements the Spectral Correlation mapper approach to calculate the correlation between image specta and a reference spectra.
	 As only correlation is importand not negative or positive values, the result is normallised to be between 0 and 1
	 
	 De Carvalho and Meneses. Spectral correlation mapper (SCM): <br>
	 An improvement on the spectral angle mapper (SAM). Airborne Visible/Infrared Imaging Spectrometer (AVIRIS) <br
	 2000 Workshop Proceedings, Pasadena (2000)<br
	 */
	
	
	class DllExport RSGISSpectralCorrelationMapperRule : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISSpectralCorrelationMapperRule(int numOutBands, gsl_matrix *refSpectra);
		void calcImageValue(float *bandValues, int numBands, double *output);
		~RSGISSpectralCorrelationMapperRule();
	private:
		double *imageSpecArray;
		gsl_matrix *refSpectra;
	};
		
	/// Classify rule image produced by SAM
	class DllExport RSGISSpectralCorrelationMapperClassifier : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISSpectralCorrelationMapperClassifier(int numOutBands, double threashold);
		void calcImageValue(float *bandValues, int numBands, double *output);
		~RSGISSpectralCorrelationMapperClassifier();
	private:
		double threashold;
	};
	
}}

#endif
