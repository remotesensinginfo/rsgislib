/*
 *  RSGISExtractImageValues.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 9/10/2013.
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


#ifndef RSGISExtractImageValues_H
#define RSGISExtractImageValues_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "utils/RSGISExportData2HDF.h"

#include "utils/RSGISExportForPlottingIncremental.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

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
	
    
    class DllExport RSGISExtractImageValues
    {
    public:
        RSGISExtractImageValues();
        void extractDataWithinMask2HDF(GDALDataset *mask, GDALDataset *image, std::string outHDFFile, float maskValue);
        void extractImgBandDataWithinMask2HDF(std::vector<std::pair<std::string, std::vector<unsigned int> > > imageFiles, std::string maskImage, std::string outHDFFile, float maskValue);
        void sampleExtractedHDFData(std::string inputH5, std::string outputH5, unsigned int nSamples, int seed);
        void splitExtractedHDFData(std::string inputH5, std::string outputP1H5, std::string outputP2H5, unsigned int nSamples, int seed);
        ~RSGISExtractImageValues();
    };
    
	
	class DllExport RSGISExtractImageValuesWithMask : public RSGISCalcImageValue
	{
	public:
		RSGISExtractImageValuesWithMask(std::vector<float*> *pxlVals, float maskValue);
		void calcImageValue(float *bandValues, int numBands, double *output)  {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent)  {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent)  {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output)  {throw RSGISImageCalcException("No implemented");};
		~RSGISExtractImageValuesWithMask();
    private:
        std::vector<float*> *pxlVals;
        float maskValue;
	};
    
    
    class DllExport RSGISExtractImageBandValuesWithMask : public RSGISCalcImageValue
    {
    public:
        RSGISExtractImageBandValuesWithMask(std::vector<float*> *pxlVals, std::vector<unsigned int> imgBands, float maskValue);
        void calcImageValue(float *bandValues, int numBands, double *output)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output)  {throw RSGISImageCalcException("No implemented");};
        ~RSGISExtractImageBandValuesWithMask();
    private:
        std::vector<float*> *pxlVals;
        std::vector<unsigned int> imgBands;
        unsigned int numOutVals;
        float maskValue;
    };
    


    
    class DllExport RSGISExtractPxlsAsPts
    {
    public:
        RSGISExtractPxlsAsPts();
        void exportPixelsAsPoints(GDALDataset *image, OGRLayer *vecLayer, float maskVal);
        void exportPixelsAsPoints(GDALDataset *image, float maskVal, std::vector<std::pair<double,double> > *pxPts, geos::geom::Envelope *env=NULL);
        void exportPixelsAsPointsWithVal(GDALDataset *image, float maskVal, GDALDataset *valImg, int valImgBand, std::vector<std::pair<std::pair<double,double>,double> > *pxPts, bool quiet=false, geos::geom::Envelope *env=NULL);
        ~RSGISExtractPxlsAsPts();
    };
    
    
    class DllExport RSGISExtractPxlsAsPtsImgCalc : public RSGISCalcImageValue
    {
    public:
        RSGISExtractPxlsAsPtsImgCalc(OGRLayer *vecLayer, float maskValue);
        void calcImageValue(float *bandValues, int numBands, double *output)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent);
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output)  {throw RSGISImageCalcException("No implemented");};
        ~RSGISExtractPxlsAsPtsImgCalc();
    private:
        OGRLayer *vecLayer;
        OGRFeatureDefn *featDefn;
        float maskValue;
    };
    
    class DllExport RSGISExtractPxlsAsPts2VecImgCalc : public RSGISCalcImageValue
    {
    public:
        RSGISExtractPxlsAsPts2VecImgCalc(std::vector<std::pair<double,double> > *pxPts, float maskValue);
        void calcImageValue(float *bandValues, int numBands, double *output)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent);
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output)  {throw RSGISImageCalcException("No implemented");};
        ~RSGISExtractPxlsAsPts2VecImgCalc();
    private:
        std::vector<std::pair<double,double> > *pxPts;
        float maskValue;
    };
    
    class DllExport RSGISExtractPxlsAsPts2VecWithValImgCalc : public RSGISCalcImageValue
    {
    public:
        RSGISExtractPxlsAsPts2VecWithValImgCalc(std::vector<std::pair<std::pair<double,double>,double> > *pxPts, float maskValue, int valIdx);
        void calcImageValue(float *bandValues, int numBands, double *output)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent);
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output)  {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output)  {throw RSGISImageCalcException("No implemented");};
        ~RSGISExtractPxlsAsPts2VecWithValImgCalc();
    private:
        std::vector<std::pair<std::pair<double,double>,double> > *pxPts;
        float maskValue;
        int valIdx;
    };
	
}}

#endif



