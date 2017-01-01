/*
 *  RSGISExeZonalStats.h
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

#ifndef RSGISExeZonalStats_H
#define RSGISExeZonalStats_H

#include <string>
#include <iostream>
#include <stdio.h>
#include <limits>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "img/RSGISPixelInPoly.h"

#include "vec/RSGISZonalStats.h"
#include "vec/RSGISFuzzyZonalStats.h"
#include "vec/RSGISZonalCountStats.h"
#include "vec/RSGISZonalMeanStats.h"
#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISZonalStats2Matrix.h"
#include "vec/RSGISPopulateTopPowerSet.h"
#include "vec/RSGISProcessVector.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISVectorZonalStats.h"
#include "vec/RSGISZonalLocalSpatialStats.h"
#include "vec/RSGISExtractEndMembers2Matrix.h"

#include "cmds/RSGISCmdZonalStats.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISMatrices.h"

#include "utils/RSGISFileUtils.h"

#include "gdal_priv.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::math;
using namespace rsgis::vec;
using namespace rsgis::utils;

class DllExport RSGISExeZonalStats : public RSGISAlgorithmParameters
	{
	public:

		enum options
		{
			none,
			polygons2shp,
			rasterpolygons2shp,
			rasterpolygons2txt,
			pixelVals2txt,
            varibles2matrix,
			image2matrix,
			pixelcount,
			pixelstats,
			pixelmean,
			pixelWeightedMean,
			pixelmeanLSSVar,
			fuzzy,
			powersetcount,
			pointvalue,
            endmembers,
            imagezone2hdf,
		};

		RSGISExeZonalStats();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeZonalStats();
	protected:
		options option;
		string inputImage;
		string inputVecPolys;
		string inputRasPolys;
		string outputVecPolys;
		string outputTextFile;
		string outputMatrix;
        string outputFile;
		string polyAttribute;
		string classAttribute;
		CountAttributes **attributeCountList;
		MeanAttributes **attributeMeanList;
		ZonalAttributes **attributeZonalList;
		FuzzyAttributes **attributeFuzzyList;
		int imageBand;
		int numAttributes;
		int nTop;
		float binsize;
		float threshold;
		int windowSize;
		double offsetSize;
		bool usePixelSize;
		bool mean;
		bool max;
		bool min;
		bool stddev;
		bool force;
		bool pxlcount;
		bool emptyset;
		bool useRasPoly;
        bool ignoreProjection;
		bool copyAttributes;
		bool dB;
        rsgis::img::pixelInPolyOption method;
        std::string outtxt;
		bool outputToText;
		bool calcZonalAllBands;
		bool minAll;
		bool maxAll;
		bool meanAll;
		bool stdDevAll;
        bool modeAll;
        bool sumAll;
		bool countAll;
		bool minThreshAll;
		bool maxThreshAll;
		double minThreshAllVal;
		double maxThreshAllVal;
        bool transposeOutput;
        bool useBandNames;
        bool shortenBandNames;
	};
}

#endif


