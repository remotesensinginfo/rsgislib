/*
 *  RSGISStretchImage.h
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

#ifndef RSGISStretchImage_H
#define RSGISStretchImage_H

#include <iostream>
#include <fstream>
#include <math.h>
#include <float.h>

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISImageStatistics.h"

#include "math/RSGISMathFunction.h"
#include "math/RSGISMathException.h"

#include "boost/math/special_functions/fpclassify.hpp"

namespace rsgis { namespace img {


	class RSGISStretchImage
	{
	public:
		RSGISStretchImage(GDALDataset *inputImage, std::string outputImage, bool outStats, std::string outStatsFile, bool ignoreZeros, std::string imageFormat, GDALDataType outDataType);
		void executeLinearMinMaxStretch() throw(RSGISImageCalcException);
		void executeLinearPercentStretch(float percent) throw(RSGISImageCalcException);
		void executeLinearStdDevStretch(float stddev) throw(RSGISImageCalcException);
		void executeHistogramStretch() throw(RSGISImageCalcException);
		void executeExponentialStretch() throw(RSGISImageCalcException);
		void executeLogrithmicStretch() throw(RSGISImageCalcException);
		void executePowerLawStretch(float power) throw(RSGISImageCalcException);
		~RSGISStretchImage();
	protected:
		GDALDataset *inputImage;
        std::string outputImage;
        bool outStats;
        std::string outStatsFile;
        bool ignoreZeros;
        std::string imageFormat;
        GDALDataType outDataType;
	};

	class RSGISExponentStretchFunction : public rsgis::math::RSGISMathFunction
	{
	public:
		RSGISExponentStretchFunction(){};
		double calcFunction(double value) throw(rsgis::math::RSGISMathException);
		double dX(double value) throw(rsgis::math::RSGISMathException){throw rsgis::math::RSGISMathException("dX is not implemented");};
		int numCoefficients() throw(rsgis::math::RSGISMathException) {return 0;}
		void updateCoefficents(double *newCoefficents){}
		~RSGISExponentStretchFunction(){};
	};

	class RSGISLogrithmStretchFunction : public rsgis::math::RSGISMathFunction
	{
	public:
		RSGISLogrithmStretchFunction(){};
		double calcFunction(double value) throw(rsgis::math::RSGISMathException);
		double dX(double value) throw(rsgis::math::RSGISMathException){throw rsgis::math::RSGISMathException("dX is not implemented");};
		int numCoefficients() throw(rsgis::math::RSGISMathException) {return 0;}
		void updateCoefficents(double *newCoefficents){}
		~RSGISLogrithmStretchFunction(){};
	};

	class RSGISPowerLawStretchFunction : public rsgis::math::RSGISMathFunction
	{
	public:
		RSGISPowerLawStretchFunction(float power){this->power = power;};
		double calcFunction(double value) throw(rsgis::math::RSGISMathException);
		double dX(double value) throw(rsgis::math::RSGISMathException){throw rsgis::math::RSGISMathException("dX is not implemented");};
		int numCoefficients() throw(rsgis::math::RSGISMathException) {return 0;}
		void updateCoefficents(double *newCoefficents){}
		~RSGISPowerLawStretchFunction(){};
	protected:
		float power;
	};



	class RSGISLinearStretchImage : public RSGISCalcImageValue
	{
	public:
		RSGISLinearStretchImage(int numberOutBands, double *imageMaxIn, double *imageMinIn, double *outMaxIn, double *outMinIn);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		~RSGISLinearStretchImage();
	protected:
		double *imageMax;
		double *imageMin;
		double *outMax;
		double *outMin;
	};


	class RSGISFuncLinearStretchImage : public RSGISCalcImageValue
	{
	public:
		RSGISFuncLinearStretchImage(int numberOutBands, double *imageMaxIn, double *imageMinIn, double *outMaxIn, double *outMinIn, rsgis::math::RSGISMathFunction *func);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		~RSGISFuncLinearStretchImage();
	protected:
		double *imageMax;
		double *imageMin;
		double *outMax;
		double *outMin;
		rsgis::math::RSGISMathFunction *func;
	};

}}

#endif
