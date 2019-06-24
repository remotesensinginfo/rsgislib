 /*
 *  RSGISImageComposite.h
 *  RSGISLIB
 *
 *  Created by Daniel Clewley on 27/02/2012.
 *  Copyright 2012 RSGISLib. All rights reserved.
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

#include <math.h>

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

    
    enum compositeStat
    {
        /// Available parameters for the estimation algorithm
        compositeMean,
        compositeMin,
        compositeMax,
        compositeRange
    };
    
    struct DllExport RSGISCompositeInfo
    {
        unsigned int year;
        unsigned int day;
        std::string compImg;
        std::string imgRef;
        bool outRef;
        double dist;
        bool usedInComp;
        std::set<long> pxlRefContrib2Fill;
        std::vector<std::string> *imgFileNames;
    };
    
    inline bool compare_CompositeInfoDates (const RSGISCompositeInfo& first, const RSGISCompositeInfo& second)
    {
        bool returnVal = true;
        if(first.year > second.year)
        {
            returnVal = false;
        }
        else if((first.year == second.year) && (first.day > second.day))
        {
            returnVal = false;
        }
        
        return returnVal;
    }
    
    inline bool compare_CompositeInfoDist (const RSGISCompositeInfo& first, const RSGISCompositeInfo& second)
    {
        return (first.dist < second.dist);
    }
    
    inline double calcAbsDateDistCompositeInfo (const RSGISCompositeInfo& first, const RSGISCompositeInfo& second)
    {
        double dist = 0.0;
        if(first.year == second.year)
        {
            if(first.day > second.day)
            {
                dist = first.day - second.day;
            }
            else
            {
                dist = second.day - first.day;
            }
        }
        else if(first.year < second.year)
        {
            int nYear = second.year - first.year;
            if(nYear > 1)
            {
                int nDays2EndYear = 366-first.day;
                dist = nDays2EndYear + second.day + (366*(nYear-1));
            }
            else
            {
                int nDays2EndYear = 366-first.day;
                dist = nDays2EndYear + second.day;
            }
        }
        else
        {
            int nYear = first.year - second.year;
            if(nYear > 1)
            {
                int nDays2EndYear = 366-second.day;
                dist = nDays2EndYear + first.day + (366*(nYear-1));
            }
            else
            {
                int nDays2EndYear = 366-second.day;
                dist = nDays2EndYear + first.day;
            }
        }
        return dist;
    }
    
	class DllExport RSGISImageComposite : public RSGISCalcImageValue
	{

	public: 
		RSGISImageComposite(int numberOutBands, unsigned int nCompositeBands, compositeStat outStat = compositeMean);
		void calcImageValue(float *bandValues, int numBands, double *output);
		void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");}
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("No implemented");};
		~RSGISImageComposite(){};
	private:
        int numberOutBands;
        unsigned int nCompositeBands;
        compositeStat outStat;
	};
    
    
    
    class DllExport RSGISMaxNDVIImageComposite : public RSGISCalcImageValue
    {
    public:
        RSGISMaxNDVIImageComposite(int numberOutBands, unsigned int redBand, unsigned int nirBand, unsigned int numInImgs);
        void calcImageValue(float *bandValues, int numBands, double *output);
        void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");}
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("No implemented");};
        ~RSGISMaxNDVIImageComposite(){};
    private:
        unsigned int redBand;
        unsigned int nirBand;
        unsigned int numInImgs;
    };
    
    
    class DllExport RSGISRefImgImageComposite : public RSGISCalcImageValue
    {
    public:
        RSGISRefImgImageComposite(int numberOutBands, unsigned int numInImgs, float outNoDataVal);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
        ~RSGISRefImgImageComposite(){};
    private:
        unsigned int numInImgs;
        float outNoDataVal;
    };
    
    
    class DllExport RSGISTimeseriesFillRefImgImageComposite : public RSGISCalcImageValue
    {
    public:
        RSGISTimeseriesFillRefImgImageComposite(std::vector<rsgis::img::RSGISCompositeInfo*> compInfoVec, std::map<std::string, unsigned int> *ratImgLst, int refImgInitPxlVal);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
        long getMaxRefPxlVal(){return maxRefPxlVal;};
        ~RSGISTimeseriesFillRefImgImageComposite(){};
    private:
        std::vector<rsgis::img::RSGISCompositeInfo*> compInfoVec;
        std::map<std::string, unsigned int> *ratImgLst;
        long maxRefPxlVal;
        int refImgInitPxlVal;
    };
    
    class DllExport RSGISTimeseriesFillImgImageComposite : public RSGISCalcImageValue
    {
    public:
        RSGISTimeseriesFillImgImageComposite(std::vector<rsgis::img::RSGISCompositeInfo*> compInfoVec, unsigned int *imgIdxLUT, unsigned int nLUT, unsigned int nBands);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
        ~RSGISTimeseriesFillImgImageComposite(){};
    private:
        std::vector<rsgis::img::RSGISCompositeInfo*> compInfoVec;
        unsigned int *imgIdxLUT;
        unsigned int nLUT;
    };
    
    class DllExport RSGISTimeseriesFillFinalRefImgImageComposite : public RSGISCalcImageValue
    {
    public:
        RSGISTimeseriesFillFinalRefImgImageComposite(std::vector<rsgis::img::RSGISCompositeInfo*> compInfoVec, std::map<std::string, unsigned int> *ratImgLst);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
        ~RSGISTimeseriesFillFinalRefImgImageComposite(){};
    private:
        std::vector<rsgis::img::RSGISCompositeInfo*> compInfoVec;
        std::map<std::string, unsigned int> *ratImgLst;
    };
    
    
    class DllExport RSGISCombineImgBands2SingleBand : public RSGISCalcImageValue
    {
    public:
        RSGISCombineImgBands2SingleBand(double noDataVal);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
        ~RSGISCombineImgBands2SingleBand();
    protected:
        double noDataVal;
    };
    
    
}}
