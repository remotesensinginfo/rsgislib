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

#include <boost/lexical_cast.hpp>

#include "geos/geom/Envelope.h"

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
//#include "img/RSGISGenAccuracyPoints.h"
#include "img/RSGISSampleImage.h"


#include "vec/RSGISImageTileVector.h"
#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISVectorIO.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISPolygonData.h"

//#include "rastergis/RSGISDefineImageTiles.h"
//#include "rastergis/RSGISCreateImageTiles.h"

#include "cmds/RSGISCmdImageUtils.h"
#include "cmds/RSGISCmdException.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::math;
using namespace rsgis::img;
using namespace rsgis::utils;
using namespace rsgis::vec;
using namespace rsgis::datastruct;

class DllExport RSGISExeImageUtils : public RSGISAlgorithmParameters
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
            createtiles,
            clump,
            imageComposite,
            relabel,
            assignproj,
            popimgstats,
            createcopy,
            createKMLFile,
            bandcolourusage,
            assignspatialinfo,
            genassesspoints,
            uniquepxlclumps,
            subset2img,
            defineimgtiles,
            gentilemasks,
            cutouttile,
            stretchwithstats,
            subsampleimage,
            darktargetmask,
            copyprojdef,
            copyprojdefspatialinfo,
            imagerasterzone2hdf,
            bandselect
		};

		enum interpolators
		{
			cubic,
			billinearArea,
			billinearPt,
			nn,
			trangular
		};



		RSGISExeImageUtils();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeImageUtils();
	protected:
		options option;
		interpolators interpolator;
        rsgis::cmds::RSGISStretches stretchType;
		string inputImage;
		string outputImage;
        string outputHDFFile;
        string outputImageBase;
		string *inputImages;
		string inputVector;
        string inputCSV;
        string inputDEM;
        string inputFile;
		string outputFile;
		string filenameAttribute;
		string imageMask;
		string inputDIR;
		string outputDIR;
		string inMatrixfile;
		string proj;
        string projFile;
        string imageFormat;
        string lutMatrixFile;
        string classColumnName;
        string inputROIImage;
        string tileImage;
        string outTilesList;
		ClassColour **classColour;
		float nodataValue;
		float skipValue;
		float skipUpperThresh;
		float skipLowerThresh;
		unsigned int skipBand;
		float maskValue;
		float resampleScale;
        float dataValue;
		int numClasses;
		int numImages;
		bool projFromImage;
		int imgBand1;
		int imgBand2;
		string bandFloatValuesVector;
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
		bool mosaicSkipThresh;
		noiseType noise;
		float scale;
        bool definiensTiles;
		int panWinSize;
        bool processInMemory;
        unsigned int compositeBands;
        std::string outCompStat;
        bool ignoreZeros;
        bool onePassSD;
        GDALDataType outDataType;
        RSGISLibDataType rsgisOutDataType;
        bool calcImgPyramids;
        bool useIgnoreVal;
        bool bandsDefined;
        vector<int> bands;
        string outKMLFile;
        bool outStatsFile;
        std::vector<std::pair<unsigned int, GDALColorInterp> > bandClrUses;
        double tlx;
        double tly;
        double resX;
        double resY;
        double rotX;
        double rotY;
        bool tlxDef;
        bool tlyDef;
        bool resXDef;
        bool resYDef;
        bool rotXDef;
        bool rotYDef;
        string outFileExtension;
        unsigned int tileOverlap;
        bool demProvided;
        //AccPtsType accuracyPtsType;
        unsigned int numPoints;
        unsigned int seed;
        bool noDataValDefined;
        float validPixelRatio;
        unsigned int tileSizePxl;
        float overlap;
        bool createAnOverlap;
        bool growOverlap;
        bool offsetTiling;
        unsigned int imageSample;
        unsigned int overlapBehaviour; // Behaviour for multiple pixels in mosaic
        std::vector<unsigned int> selectBands;
        bool allBands;
	};
}
#endif



