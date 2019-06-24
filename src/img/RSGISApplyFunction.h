 /*
 *  RSGISApplyFunction.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 27/01/2011.
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

#ifndef RSGISApplyFunction_H
#define RSGISApplyFunction_H

#include <iostream>
#include <stdio.h>
#include <math.h>

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

#include "math/RSGISFunctions.h"
#include "math/RSGISMathFunction.h"

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


	class DllExport RSGISApplySingleVarFunction : public RSGISCalcImageValue
	{
		/// Applies RSGISMathFunction to image
	public: 
		RSGISApplySingleVarFunction(int numOutputBands, rsgis::math::RSGISMathFunction *imagefunction, float ignoreVal = 0);
		virtual void calcImageValue(float *bandValues, int numBands, double *output);
		virtual void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
		virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
		virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
		virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};													
		~RSGISApplySingleVarFunction();
	protected:
        rsgis::math::RSGISMathFunction *imagefunction;
		float ignoreVal;
	};
	
	class DllExport RSGISApplyTwoVarFunction : public RSGISCalcImageValue
	{
		/// Applies RSGISMathTwoVariableFunction to image
	public: 
		RSGISApplyTwoVarFunction(int numOutputBands, rsgis::math::RSGISMathTwoVariableFunction *imagefunction, float ignoreVal = 0);
		virtual void calcImageValue(float *bandValues, int numBands, double *output);
		virtual void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
		virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom:: Envelope extent) {throw RSGISImageCalcException("Not implemented");};
		virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");}
		virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};													
		~RSGISApplyTwoVarFunction();
	protected:
        rsgis::math::RSGISMathTwoVariableFunction *imagefunction;
		float ignoreVal;
	};
	
	
	class DllExport RSGISApplyThreeVarFunction : public RSGISCalcImageValue
	{
		/// Applies RSGISMathThreeVariableFunction to image
	public: 
		RSGISApplyThreeVarFunction(int numOutputBands, rsgis::math::RSGISMathThreeVariableFunction *imagefunction, float ignoreVal = 0);
		virtual void calcImageValue(float *bandValues, int numBands, double *output);
		virtual void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
		virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
		virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
		virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");}
		virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};													
		~RSGISApplyThreeVarFunction();
	protected:
        rsgis::math::RSGISMathThreeVariableFunction *imagefunction;
		float ignoreVal;
	};

}}

#endif
