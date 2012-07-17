/*
 *  RSGISExeImageUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/12/2008.
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

#ifndef RSGISExeImageUtils_H
#define RSGISExeImageUtils_H

#include <string>
#include <iostream>
#include <limits>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISVectors.h"

#include "utils/RSGISTextUtils.h"
#include "utils/RSGISFileUtils.h"
#include "utils/RSGISDefiniensWorkspaceFileName.h"
#include "utils/RSGISExportForPlottingIncremental.h"

#include "datastruct/SortedGenericList.cpp"

#include "img/RSGISCalcImage.h"
#include "img/RSGISColourUpImage.h"
#include "img/RSGISImageMosaic.h"
#include "img/RSGISCopyImage.h"
#include "img/RSGISMaskImage.h"
#include "img/RSGISImageInterpolation.h"
#include "img/RSGISImageInterpolator.h"
#include "img/RSGISDefiniensCSVRasterise.h"
#include "img/RSGISProjectionStrings.h"
#include "img/RSGISExport2DScatterPTxt.h"
#include "img/RSGISSavitzkyGolaySmoothingFilters.h"
#include "img/RSGISCumulativeArea.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISStretchImage.h"
#include "img/RSGISColourUsingHue.h"
#include "img/RSGISAddNoise.h"
#include "img/RSGISPanSharpen.h"
#include "img/RSGISCalcImgAlongsideOut.h"
#include "img/RSGISClumpImage.h"
#include "img/RSGISImageComposite.h"
#include "img/RSGISRelabelPixelValuesFromLUT.h"
#include "img/RSGISPopWithStats.h"

#include "vec/RSGISImageTileVector.h"
#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISVectorIO.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISPolygonData.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

namespace rsgisexe{

    /*
using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::math;
using namespace rsgis::img;
using namespace rsgis::utils;
using namespace rsgis::vec;
using namespace rsgis::datastruct;
     */

class RSGISExeImageUtils : public rsgis::RSGISAlgorithmParameters
	{
	public:
		
		enum options 
		{
			none,
			colour,
			mosaic,
			include,
			cut2poly,
			mask,
			resample,
			rasterisedefiniens,
			printProj4,
			printWKT,
			extract2dscatterptxt,
			sgsmoothing,
			cumulativearea,
			createimage,
			stretch,
			huecolour,
			removespatialref,
			addnoise,
			definespatialref,
            subset,
            subset2polys,
			pansharpen,
            colourimagebands,
            createslices,
            clump,
            imageComposite,
            relabel,
            assignproj,
            popimgstats,
            createcopy,
            createKMLFile
		};
		
		enum interpolators
		{
			cubic,
			billinearArea,
			billinearPt,
			nn,
			trangular
		};
		
		enum stretches
		{
			linearMinMax,
			linearPercent,
			linearStdDev,
			histogram,
			exponential,
			logarithmic,
			powerLaw
		};
		
		RSGISExeImageUtils();
		virtual rsgis::RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(rsgis::RSGISException);
		virtual void printParameters();
		virtual std::string getDescription();
		virtual std::string getXMLSchema();
		virtual void help();
		~RSGISExeImageUtils();
	protected:
		options option;
		interpolators interpolator;
		stretches stretchType;
		std::string inputImage;
		std::string outputImage;
		std::string *inputImages;
		std::string inputVector;
        std::string inputCSV;
		std::string outputFile;
		std::string filenameAttribute;
		std::string imageMask;
		std::string inputDIR;
		std::string outputDIR;
		std::string inMatrixfile;
		std::string proj;
        std::string projFile;
        std::string imageFormat;
        std::string lutMatrixFile;
        rsgis::img::ClassColour **classColour;
		float nodataValue;
		float skipValue;
		float skipUpperThreash;
		float skipLowerThreash;
		unsigned int skipBand;
		float maskValue;
		float resampleScale;
        float dataValue;
		int numClasses;
		int numImages;
		bool projFromImage;
		int imgBand1;
		int imgBand2;
		std::string bandFloatValuesVector;
		int order;
		int window;
		int height;
		int width;
		int numBands;
		double eastings;
		double northings;
		float outValue;
		float resolution;
		float xRes;
		float yRes;
		float percent;
		float stddev;
		float power;
		float lowerRangeValue;
		float upperRangeValue;
		bool mosaicSkipVals;
		bool mosaicSkipThreash;
		rsgis::img::noiseType noise;
		float scale;
        bool definiensTiles;
		int panWinSize;
        bool processInMemory;
        unsigned int compositeBands;
        rsgis::img::compositeStat outCompStat;
        bool ignoreZeros;
        GDALDataType outDataType;
        bool calcImgPyramids;
        bool useIgnoreVal;
        bool bandsDefined;
        std::vector<int> bands;
        std::string outKMLFile;
	};
}
#endif



