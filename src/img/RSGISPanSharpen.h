 /*
 *  RSGISPanSharpen.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 31/10/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
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

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISImageException.h"
#include "img/RSGISImageCalcException.h"

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"

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

namespace rsgis { namespace img {

	class DllExport RSGISHCSPanSharpen : public RSGISCalcImageValue
	{
		/** 
		 Implements Hyperspherical Colour Space (HSC) Pan Sharpening from the following paper:
		 
		 Padwick, C., Deskevich, M., Pacifici, F., Smallwood, S. 2010. WorldView-2 Pan-Sharpening. ASPRS 2010 Annual Conference, San Diego, California (2010) pp. 26-30.
		 
		 Takes array of image bands, where the panchromatic band is the last band in the image and array of statistics of the form:
		 meanMS
		 meanPAN
		 sdMS
		 sdPAN
		 */
	public: 
		RSGISHCSPanSharpen(int numberOutBands, float *imageStats);
		void calcImageValue(float *bandValues, int numBands, double *output);
		void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");}
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("No implemented");};
		~RSGISHCSPanSharpen(){};
	private:
		unsigned int numberOutBands;
		float *imageStats;
	};

	class DllExport RSGISHCSPanSharpenCalcMeanStats : public RSGISCalcImageValue
	{
	public:
		RSGISHCSPanSharpenCalcMeanStats(int numberOutBands, float *outStats);
		void calcImageValue(float *bandValues, int numBands, double *output) {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");}
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("No implemented");};
		void returnStats();
		~RSGISHCSPanSharpenCalcMeanStats(){};
	private:
		unsigned int numberOutBands;
		float *outStats;
		double sumMS;
		double sumPAN;
		long int nPix;
	};

	class DllExport RSGISHCSPanSharpenCalcSDStats : public RSGISCalcImageValue
	{
	public:
		RSGISHCSPanSharpenCalcSDStats(int numberOutBands, float *outStats);
		void calcImageValue(float *bandValues, int numBands, double *output) {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");}
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("No implemented");};
		void returnStats();
		~RSGISHCSPanSharpenCalcSDStats(){};
	private:
		unsigned int numberOutBands;
		float *outStats;
		double sumMS;
		double sumPAN;
		long int nPix;
	};
	
}}

