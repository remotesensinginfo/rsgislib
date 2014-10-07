/*
 *  RSGISExePostClassification.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/07/2009.
 *  Copyright 2009 RSGISLib.
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

#ifndef RSGISExePostClassification_H
#define RSGISExePostClassification_H

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"
#include "common/RSGISFileException.h"

#include "math/RSGISMathsUtils.h"

#include "utils/RSGISFileUtils.h"
#include "utils/RSGISExportForPlotting.h"
#include "utils/RSGISGEOSFactoryGenerator.h"

#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISProcessVector.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISClassificationPolygonReader.h"
#include "vec/RSGISPolygonReader.h"
#include "vec/RSGISVectorIO.h"
#include "vec/RSGISClusteredClassificationPolygon.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISProcessVector.h"
#include "vec/RSGISGEOSPolygonReader.h"
#include "vec/RSGISGEOSMultiPolygonReader.h"
#include "vec/RSGISClusterData.h"
#include "vec/RSGISIntValuePolygon.h"
#include "vec/RSGISIntValuePolygonReader.h"
#include "vec/RSGISClassPolygon.h"
#include "vec/RSGISLabelPolygonsFromClassification.h"

#include "geom/RSGISGeometry.h"
#include "geom/RSGISIdentifyNonConvexPolygons.h"
#include "geom/RSGISIdentifyNonConvexPolygonsLineProject.h"
#include "geom/RSGISIdentifyNonConvexPolygonsDelaunay.h"
#include "geom/RSGISIdentifyNonConvexPolygonsSnakes.h"
#include "geom/RSGISSpatialClustererInterface.h"
#include "geom/RSGISMinSpanTreeClustererStdDevThreshold.h"
#include "geom/RSGISMinSpanTreeClustererEdgeLenThreshold.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

namespace rsgisexe{
//using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::math;
using namespace rsgis::utils;
using namespace rsgis::geom;
using namespace rsgis::vec;


class DllExport RSGISExePostClassification : public RSGISAlgorithmParameters
	{
	public:

		enum options
		{
			none,
			clusterclass,
			cluster,
			clusterclasspolygonize,
			clusterpolygonize,
			splitlargesmall,
			mergelargepolygons,
			morphology,
			removesmallwithinlarge,
			mergesmallandlarge,
			clusterremainingsmall,
			mergesmall2nearestlarge,
			mergepolygons,
			labelpolys
		};

		enum polygonizer
		{
			polyonizernotdefined,
			lineproj,
			delaunay1,
			delaunay2,
			convexhull,
			multipoly,
			snakes,
			polys
		};

		enum clusterer
		{
			clusterernotdefined,
			edgestddev,
			edgelength
		};

		struct DllExport EdgeLengthPair
		{
			std::string classname;
			float lengththreshold;
		};

		RSGISExePostClassification();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual std::string getDescription();
		virtual std::string getXMLSchema();
		virtual void help();
		~RSGISExePostClassification();
	protected:
		void buildClusterDataStructure(std::vector<RSGISClusterData*> *clusters, RSGISPolygonData **dataClusters, int numClusters, std::list<RSGIS2DPoint*> *inClusterPolygons, std::list<RSGIS2DPoint*> *dataOrigPolys) throw(RSGISException);
		options option;
		std::string inputVector;
		std::string inputVectorLarge;
		std::string inputVectorSmall;
		std::string inputVector_clusters;
		std::string inputVector_polys;
		std::string inputVector_orig;
		std::string outputVector;
		std::string outputLargeVector;
		std::string outputSmallVector;
		std::string classStr;
		std::string intValueAtt;
		std::string labelsVector;
		std::string classAttribute;
		float numStdDevthreshold;
		bool force;
		float resolution;
		float edgelengththreshold;
		float maxEdgeLength;

		float alpha;
		float beta;
		float gamma;
		float delta;
		int maxNumIterations;
		std::vector<EdgeLengthPair*> *classEdgeLengths;
		polygonizer polygonizertype;
		clusterer clusterertype;

		float largeThreshold;
		float largeThreshold1;
		float propPolyArea1;
		float largeThreshold2;
		float propPolyArea2;

		float overlapThreshold;
		float intersectRatio;

		Morphology morphologyType;
		float buffer;

		float maxlength;

		dominantspecies dominant;

	};
}
#endif




