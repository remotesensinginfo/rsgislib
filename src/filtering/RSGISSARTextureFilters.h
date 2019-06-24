/*
 *  RSGISSARTextureFilters.h
 *  RSGIS_LIB
 *
 *  Created by Dan Clewley on 06/08/2012.
 *  Copyright 2013 RSGISLib.
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

#ifndef RSGISSARTextureFilters_H
#define RSGISSARTextureFilters_H

#include <iostream>

#include "common/RSGISImageException.h"

#include "filtering/RSGISImageFilterException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "filtering/RSGISImageFilter.h"

#include <boost/math/special_functions/fpclassify.hpp>

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

	class DllExport RSGISNormVarPowerFilter : public RSGISImageFilter
    {
        /**

         Normalised variance of power.

         Given in equation 8.4 in Chapter 8 of Oliver, C. and Quegen, S. 1998. Understanding Synthetic Apature Radar Images

         */

    public:

        RSGISNormVarPowerFilter(int numberOutBands, int size, std::string filenameEnding);
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageFilterException("Not implemented for NVarPower filter!");};;
        virtual void exportAsImage(std::string filename){throw RSGISImageFilterException("No image to output!");};
        ~RSGISNormVarPowerFilter(){};
    };

    class DllExport RSGISNormVarAmplitudeFilter : public RSGISImageFilter
    {
        /**

         Normalised variance of amplitude.

         Given for power in Chapter 8 of Oliver, C. and Quegen, S. 1998. Understanding Synthetic Apature Radar Images

         */

    public:

        RSGISNormVarAmplitudeFilter(int numberOutBands, int size, std::string filenameEnding);
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageFilterException("Not implemented for NVarAmplitude filter!");};;
        virtual void exportAsImage(std::string filename){throw RSGISImageFilterException("No image to output!");};
        ~RSGISNormVarAmplitudeFilter(){};
    };

    class DllExport RSGISNormVarLnPowerFilter : public RSGISImageFilter
    {
        /**

         Normalised variance of natural log power (ln(I)).

         Given for power in Chapter 8 of Oliver, C. and Quegen, S. 1998. Understanding Synthetic Apature Radar Images

         */

    public:

        RSGISNormVarLnPowerFilter(int numberOutBands, int size, std::string filenameEnding);
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageFilterException("Not implemented for NVarLogPower filter!");};;
        virtual void exportAsImage(std::string filename){throw RSGISImageFilterException("No image to output!");};
        ~RSGISNormVarLnPowerFilter(){};
    };

    class DllExport RSGISNormLnFilter : public RSGISImageFilter
    {
        /**

         Normalised natural log filter.

         Given in equation 8.27 in Chapter 8 of Oliver, C. and Quegen, S. 1998. Understanding Synthetic Apature Radar Images

         */

    public:

        RSGISNormLnFilter(int numberOutBands, int size, std::string filenameEnding);
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageFilterException("Not implemented for NVarLogPower filter!");};;
        virtual void exportAsImage(std::string filename){throw RSGISImageFilterException("No image to output!");};
        ~RSGISNormLnFilter(){};
    };

    class DllExport RSGISTextureVar : public RSGISImageFilter
    {
        /**

         Texture variance

         Given in equation 21.89b (p1913) of:

         Ulaby, F.T., Moore, R.K., and Fung, A.K. 1986. Microwave remote sensing active and passive. Volume III: from theory to applications. Artech House, Inc., Norwood, Mass. pp. 1909–1912.

         */

    public:

        RSGISTextureVar(int numberOutBands, int size, std::string filenameEnding);
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageFilterException("Not implemented for NVarLogPower filter!");};;
        virtual void exportAsImage(std::string filename){throw RSGISImageFilterException("No image to output!");};
        ~RSGISTextureVar(){};
    };
}}

#endif



