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

#include "common/RSGISFileException.h"

#include "utils/RSGISTextUtils.h"

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
    
    struct DllExport BandSpecThresholdStats
    {
        BandSpecThresholdStats(){};
        BandSpecThresholdStats(size_t band, float imgMin, float imgMax, float origMin, float origMax)
        {
            this->band = band;
            this->imgMin = imgMin;
            this->imgMax = imgMax;
            this->origMin = origMin;
            this->origMax = origMax;
        }
        size_t band;
        float imgMin;
        float imgMax;
        float origMin;
        float origMax;
    };
    

	class DllExport RSGISStretchImage
	{
	public:
		RSGISStretchImage(GDALDataset *inputImage, std::string outputImage, bool outStats, std::string outStatsFile, bool ignoreZeros, bool onePassSD, std::string imageFormat, GDALDataType outDataType);
		void executeLinearMinMaxStretch() throw(RSGISImageCalcException);
		void executeLinearPercentStretch(float percent) throw(RSGISImageCalcException);
		void executeLinearStdDevStretch(float stddev) throw(RSGISImageCalcException);
		void executeHistogramStretch() throw(RSGISImageCalcException);
		void executeExponentialStretch() throw(RSGISImageCalcException);
		void executeLogrithmicStretch() throw(RSGISImageCalcException);
		void executePowerLawStretch(float power) throw(RSGISImageCalcException);
        
        static std::vector<BandSpecThresholdStats>* readBandSpecThresholds(std::string inputFile)throw(rsgis::RSGISFileException)
        {
            std::vector<BandSpecThresholdStats> *bandStats = new std::vector<BandSpecThresholdStats>();
            
            try
            {
                std::vector<std::string> *tokens = new std::vector<std::string>();
                
                rsgis::utils::RSGISTextUtils textUtils;
                rsgis::utils::RSGISTextFileLineReader reader;
                std::string line = "";
                size_t band = 0;
                float imgMin = 0;
                float imgMax = 0;
                float origMin = 0;
                float origMax = 0;
                reader.openFile(inputFile);
                while(!reader.endOfFile())
                {
                    line = reader.readLine();
                    
                    if(!textUtils.lineStart(line, '#') & !textUtils.blankline(line))
                    {
                        textUtils.tokenizeString(line, ',', tokens, true, true);
                        if(tokens->size() != 5)
                        {
                            throw rsgis::utils::RSGISTextException("A line should have 5 tokens (band,img_min,img_max,out_min,out_max).");
                        }
                        
                        band = textUtils.strtosizet(tokens->at(0));
                        origMin = textUtils.strtofloat(tokens->at(1));
                        origMax = textUtils.strtofloat(tokens->at(2));
                        imgMin = textUtils.strtofloat(tokens->at(3));
                        imgMax = textUtils.strtofloat(tokens->at(4));
                        
                        bandStats->push_back(BandSpecThresholdStats(band, imgMin, imgMax, origMin, origMax));
                    }
                    
                    tokens->clear();
                }
                
                delete tokens;
                reader.closeFile();
            }
            catch(rsgis::RSGISFileException &e)
            {
                throw e; 
            }
            catch(rsgis::utils::RSGISTextException &e)
            {
                throw rsgis::RSGISFileException(e.what()); 
            }
            catch(std::exception &e)
            {
                throw rsgis::RSGISFileException(e.what());
            }
            
            return bandStats;
        };
		~RSGISStretchImage();
	protected:
		GDALDataset *inputImage;
        std::string outputImage;
        bool outStats;
        std::string outStatsFile;
        bool ignoreZeros;
        bool onePassSD;
        std::string imageFormat;
        GDALDataType outDataType;
	};
    
    class DllExport RSGISStretchImageWithStats
	{
	public:
		RSGISStretchImageWithStats(GDALDataset *inputImage, std::string outputImage, std::string inStatsFile, std::string imageFormat, GDALDataType outDataType);
		void executeLinearMinMaxStretch() throw(RSGISImageCalcException);
		void executeHistogramStretch() throw(RSGISImageCalcException);
		void executeExponentialStretch() throw(RSGISImageCalcException);
		void executeLogrithmicStretch() throw(RSGISImageCalcException);
		void executePowerLawStretch(float power) throw(RSGISImageCalcException);
        
        static std::vector<BandSpecThresholdStats>* readBandSpecThresholds(std::string inputFile)throw(rsgis::RSGISFileException)
        {
            std::vector<BandSpecThresholdStats> *bandStats = new std::vector<BandSpecThresholdStats>();
            
            try
            {
                std::vector<std::string> *tokens = new std::vector<std::string>();
                
                rsgis::utils::RSGISTextUtils textUtils;
                rsgis::utils::RSGISTextFileLineReader reader;
                std::string line = "";
                size_t band = 0;
                float imgMin = 0;
                float imgMax = 0;
                float origMin = 0;
                float origMax = 0;
                reader.openFile(inputFile);
                while(!reader.endOfFile())
                {
                    line = reader.readLine();
                    
                    if(!textUtils.lineStart(line, '#') & !textUtils.blankline(line))
                    {
                        textUtils.tokenizeString(line, ',', tokens, true, true);
                        if(tokens->size() != 5)
                        {
                            throw rsgis::utils::RSGISTextException("A line should have 5 tokens (band,img_min,img_max,out_min,out_max).");
                        }
                        
                        band = textUtils.strtosizet(tokens->at(0));
                        origMin = textUtils.strtofloat(tokens->at(1));
                        origMax = textUtils.strtofloat(tokens->at(2));
                        imgMin = textUtils.strtofloat(tokens->at(3));
                        imgMax = textUtils.strtofloat(tokens->at(4));
                        
                        bandStats->push_back(BandSpecThresholdStats(band, imgMin, imgMax, origMin, origMax));
                    }
                    
                    tokens->clear();
                }
                
                delete tokens;
                reader.closeFile();
            }
            catch(rsgis::RSGISFileException &e)
            {
                throw e;
            }
            catch(rsgis::utils::RSGISTextException &e)
            {
                throw rsgis::RSGISFileException(e.what());
            }
            catch(std::exception &e)
            {
                throw rsgis::RSGISFileException(e.what());
            }
            
            return bandStats;
        };
		~RSGISStretchImageWithStats();
	protected:
		GDALDataset *inputImage;
        std::string outputImage;
        std::string inStatsFile;
        std::string imageFormat;
        GDALDataType outDataType;
	};

	class DllExport RSGISExponentStretchFunction : public rsgis::math::RSGISMathFunction
	{
	public:
		RSGISExponentStretchFunction(){};
		double calcFunction(double value) throw(rsgis::math::RSGISMathException);
		double dX(double value) throw(rsgis::math::RSGISMathException){throw rsgis::math::RSGISMathException("dX is not implemented");};
		int numCoefficients() throw(rsgis::math::RSGISMathException) {return 0;}
		void updateCoefficents(double *newCoefficents){}
		~RSGISExponentStretchFunction(){};
	};

	class DllExport RSGISLogrithmStretchFunction : public rsgis::math::RSGISMathFunction
	{
	public:
		RSGISLogrithmStretchFunction(){};
		double calcFunction(double value) throw(rsgis::math::RSGISMathException);
		double dX(double value) throw(rsgis::math::RSGISMathException){throw rsgis::math::RSGISMathException("dX is not implemented");};
		int numCoefficients() throw(rsgis::math::RSGISMathException) {return 0;}
		void updateCoefficents(double *newCoefficents){}
		~RSGISLogrithmStretchFunction(){};
	};

	class DllExport RSGISPowerLawStretchFunction : public rsgis::math::RSGISMathFunction
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



	class DllExport RSGISLinearStretchImage : public RSGISCalcImageValue
	{
	public:
		RSGISLinearStretchImage(int numberOutBands, double *imageMaxIn, double *imageMinIn, double *outMaxIn, double *outMinIn);
		void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		~RSGISLinearStretchImage();
	protected:
		double *imageMax;
		double *imageMin;
		double *outMax;
		double *outMin;
	};


	class DllExport RSGISFuncLinearStretchImage : public RSGISCalcImageValue
	{
	public:
		RSGISFuncLinearStretchImage(int numberOutBands, double *imageMaxIn, double *imageMinIn, double *outMaxIn, double *outMinIn, rsgis::math::RSGISMathFunction *func);
		void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
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
