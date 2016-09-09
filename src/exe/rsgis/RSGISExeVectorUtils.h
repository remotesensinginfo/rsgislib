/*
 *  RSGISExeVectorUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/12/2008.
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

#ifndef RSGISExeVectorUtils_H
#define RSGISExeVectorUtils_H

#include <string>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>

#include "cmds/RSGISCmdException.h"
#include "cmds/RSGISCmdVectorUtils.h"


#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISFileException.h"

#include "utils/RSGISFileUtils.h"
#include "utils/RSGISExportForPlotting.h"
#include "utils/RSGISPlotPolygonsCSVParse.h"
#include "utils/RSGISImageFootprintPolygonsCSVParse.h"
#include "utils/RSGISGEOSFactoryGenerator.h"
#include "utils/RSGISExportForPlottingIncremental.h"

#include "math/RSGISMathsUtils.h"

#include "img/RSGISPixelInPoly.h"

#include "vec/RSGISPolygonData.h"
#include "vec/RSGISPointData.h"
#include "vec/RSGISEmptyPolygon.h"
#include "vec/RSGISCirclePolygon.h"
#include "vec/RSGISCirclePoint.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISVectorBuffer.h"
#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISVectorIO.h"
#include "vec/RSGISVectorProcessing.h"
#include "vec/RSGISVectorAttributeFindReplace.h"
#include "vec/RSGISIdentifyTopLayerAttributes.h"
#include "vec/RSGISAddAttributes.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISProcessFeatureCopyVector.h"
#include "vec/RSGISPrintFIDs.h"
#include "vec/RSGISPixelAreaCountInPolygon.h"
#include "vec/RSGISConvertToASCII.h"
#include "vec/RSGISProcessVector.h"
#include "vec/RSGISProcessGeometry.h"
#include "vec/RSGISGeometryRotateAroundFixedPoint.h"
#include "vec/RSGISGeometryToCircle.h"
#include "vec/RSGISCalcPolygonArea.h"
#include "vec/RSGISGEOSPolygonReader.h"
#include "vec/RSGISGetAttributeValues.h"
#include "vec/RSGISRasterizeVector.h"
#include "vec/RSGISGEOSMultiPolygonReader.h"
#include "vec/RSGIS2DScatterPlotVariables.h"
#include "vec/RSGISCopyCheckPolygons.h"
#include "vec/RSGISCopyPolygonsInPolygon.h"
#include "vec/RSGISCreateListOfAttributeValues.h"
#include "vec/RSGISProcessVectorSQL.h"
#include "vec/RSGISCopyFeatures.h"
#include "vec/RSGISRemoveContainedPolygons.h"
#include "vec/RSGISOGRPolygonReader.h"
#include "vec/RSGISSplitSmallLargePolygons.h"
#include "vec/RSGISAppendToVectorLayer.h"
#include "vec/RSGISRemovePolygonHoles.h"
#include "vec/RSGISDropSmallPolygons.h"
#include "vec/RSGISVectorMaths.h"
#include "vec/RSGISGetOGRGeometries.h"
#include "vec/RSGISCalcDistanceStats.h"

#include "geom/RSGISIdentifyNonConvexPolygons.h"
#include "geom/RSGISIdentifyNonConvexPolygonsLineProject.h"
#include "geom/RSGISIdentifyNonConvexPolygonsDelaunay.h"
#include "geom/RSGISIdentifyNonConvexPolygonsSnakes.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "geos/geom/Coordinate.h"

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace boost;
using namespace rsgis;
using namespace rsgis::img;
using namespace rsgis::utils;
using namespace rsgis::math;
using namespace rsgis::vec;
using namespace rsgis::geom;
using namespace geos::geom;

class DllExport RSGISExeVectorUtils : public RSGISAlgorithmParameters
	{
	public:

		enum polygonizer
		{
			polyonizernotdefined,
			lineproj,
			delaunay1,
			delaunay2,
			convexhull,
			multipoly,
			snakes
		};

		enum options
		{
			none,
			removeAttributes,
			bufferVector,
			geom2circles,
			printpolygeom,
			findreplacetext,
			listattributes,
			printattribute,
			topattributes,
			addattributes,
			copypolygons,
			printfids,
			countpxlsarea,
			points2ascii,
			rotate,
			calcarea,
			splitfeatures,
			removepolyoverlaps,
			mergesmallpolysnear,
			mergesmallpolystouching,
			morphologicalopening,
			morphologicalclosing,
			morphologicalerosion,
			morphologicaldilation,
			nonconvexoutlinedelaunay,
			nonconvexoutlinelineproj,
			nonconvexoutlinesnakes,
			convexoutline,
			mergetouchingpoly,
			freqdist,
			rasterise,
            burnrasterise,
			polygonplots,
			polygonImageFootprints,
			polygonizempolys,
			scatter2D,
			copycheckpolys,
			splitbyattribute,
			removecontainedpolygons,
			removecontainedpolygonsDIR,
			polygonsInPolygon,
			mergetouchingpolyDIR,
			splitlargesmall,
			removepolygonscontainedwithingeom,
			mergeshps,
			fixpolyextboundary,
			removepolyholes,
			dropsmallpolys,
			extractlargestpoly,
			generatesinglepoly,
			generategrid,
			generateimagegrid,
			vectormaths,
            select,
            calcmeanmindist,
            copyassignproj,
            printwkt,
            addfidcol,
            mindist2polys,
            convexhullgrps
		};

		RSGISExeVectorUtils();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeVectorUtils();
	protected:
		options option;
		list<string> *inputVectors;
		string inputGeometry;
		string inputVector;
		string inputFile;
		string inputDIR;
		string outputVector;
		string outputVectorBase;
		string outputImage;
		string inputImage;
		string outputPlot;
		string output_DIR;
		string outputlarge;
		string outputsmall;
        string outputTextFile;
		string inputCoverVector;
        string projFile;
		float buffer;
		float resolution;
		float radius;
		string areaAttribute;
		string heightAttribute;
		string radiusAttribute;
		string attribute;
		string attribute1;
		string attribute2;
		string find;
		string replace;
		string inputRasterVector;
		string *attributes;
		string extension;
        string bufferValueFile;
		int numAttributes;
		int numVars;
		string attributeName;
		rsgissummarytype summary;
		Attribute **newAttributes;
		int numTop;
		bool force;
		Coordinate *fixedPt;
		float angle;
		bool snap2Grid;
		float tolerance;
		float dissolve;
		float area;
		float relBorderThreshold;
		int curvesegments;
		bool singlevector;
		bool inputFileNotVector;
		float constVal;
		float alpha;
		float beta;
		float gamma;
		float delta;
		polygonizer polygonizertype;
		pixelInPolyOption method;
		bool attributeDef;
		float threshold;
		bool ignoreAttr;
		int maxNumIterations;
		string proj4;
		double tlx;
		double tly;
		double xres;
		double yres;
		double width;
		double height;
		bool useExistingRaster;
		string mathsExpression;
		VariableFields **variables;
		string outHeading;
        string sqlExpression;
        bool outTextFile;
        bool bufferValueInText;
        unsigned int initFID;
        bool areaThresholdProvided;
        unsigned int eastingsColIdx;
        unsigned int northingsColIdx;
        unsigned int attributeColIdx;
	};
}
#endif


