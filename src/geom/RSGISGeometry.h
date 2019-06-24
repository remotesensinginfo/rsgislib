/*
 *  RSGISGeometry.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 15/05/2008.
 *  Copyright 2008 Aberystwyth University. All rights reserved.
 *
 */


#ifndef RSGISGeometry_H
#define RSGISGeometry_H

#include <string>
#include <iostream>
#include <math.h>
#include <vector>
#include <list>
#include <algorithm>

#include "geom/RSGISGeometryException.h"
#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISPolygon.h"
#include "geom/RSGISTriangle.h"
#include "geom/RSGISGeomTestExport.h"
#include "geom/RSGISIdentifyNonConvexPolygons.h"

#include "utils/RSGISGEOSFactoryGenerator.h"
#include "utils/RSGISExportForPlotting.h"

#include "math/RSGISMatricesException.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISMathsUtils.h"

#include "datastruct/SortedGenericList.cpp"

#include "geos/geom/GeometryFactory.h"
#include "geos/geom/Point.h"
#include "geos/geom/MultiPoint.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/Envelope.h"
#include "geos/geom/GeometryCollection.h"
#include "geos/util/TopologyException.h"
#include "geos/geom/LineSegment.h"
#include "geos/algorithm/LineIntersector.h"
#include "geos/geom/CoordinateArraySequenceFactory.h"
#include "geos/util/IllegalArgumentException.h"
#include "geos/index/quadtree/Quadtree.h"
#include "geos/simplify/DouglasPeuckerSimplifier.h"
#include "geos/simplify/TopologyPreservingSimplifier.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_geom_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace geom{

	struct SortCoordinates : std::binary_function< geos::geom::Coordinate, geos::geom::Coordinate, bool >
	{
		const geos::geom::Coordinate coord;
		SortCoordinates( const geos::geom::Coordinate& coord_ ): coord( coord_ ) { }
		bool operator()( const geos::geom::Coordinate& a, const geos::geom::Coordinate& b ) const
		{
			bool returnVal = false;
			if(a.x < b.x)
			{
				returnVal = true;
			}
			else if(a.x == b.x)
			{
				if(a.y < b.y)
				{
					returnVal = true;
				}
				else if(a.z == b.z)
				{
					if(a.z < b.z)
					{
						returnVal = true;
					}
				}
			}
			return returnVal;
		}
	};
	
	struct DllExport point2D
	{
		double x;
		double y;
	};
	
	struct DllExport point3D
	{
		double x;
		double y;
		double z;
	};
		
	enum Morphology
	{
		morphologyNotDefined,
		closing,
		opening,
		dilation,
		erosion
	};
	
	class DllExport RSGISGeometry
	{
	public:
		RSGISGeometry();
		void fitPlane2Points(point3D *pts, int numPts, double *a, double *b, double *c);
		double calcPlaneFit(point3D *pts, int numPts, double a, double b, double c);
		double calcPlaneZAt(double a, double b, double c, double x, double y);
		double distance2D(point3D *pt1, point2D *pt2);
		geos::geom::Polygon* createCircle(float a, float b, float r, float resolution);
		geos::geom::Polygon* createPolygon(float xTL, float yTL, float xBR, float yBR);
		//SortedGenericList<double>* findValuesOnEdge(rsgis::math::Matrix *matrix, geos::geom::Polygon **bboxes, geos::geom::Polygon *poly);
		geos::geom::Polygon** findMaxPointsOnPolyEdge(rsgis::math::Matrix *matrix, geos::geom::Polygon **bboxes, geos::geom::Polygon *poly, int *numPolys);
		geos::geom::Envelope* findBBox(std::list<RSGIS2DPoint*> *pts);
		geos::geom::Envelope* findBBox(std::vector<RSGIS2DPoint*> *pts);
		RSGISTriangle* findBoundingTriangle(std::list<RSGIS2DPoint*> *pts);
		RSGISTriangle* findBoundingTriangle(std::vector<RSGIS2DPoint*> *pts);
		double calcAngle(RSGIS2DPoint *pbase, RSGIS2DPoint *pt);
		geos::geom::MultiPolygon* createMultiPolygon(const std::vector<geos::geom::Geometry*> *polygons);
		geos::geom::Polygon* findConvexHull(std::vector<geos::geom::Polygon*> *geoms);
		geos::geom::Polygon* findConvexHull(std::list<geos::geom::Polygon*> *geoms);
		geos::geom::Polygon* findConvexHull(const std::vector<geos::geom::Geometry*> *geometries);
		geos::geom::Polygon* findConvexHull(std::vector<geos::geom::LineSegment> *lines);
		geos::geom::Polygon* findConvexHull(std::vector<geos::geom::Coordinate> *coordinates);
        geos::geom::Polygon* findBoundingBox(std::vector<geos::geom::Coordinate> *coordinates);
		/** 
		 * turnDirection. A method to identify which direction the middle point b 
		 * is turning. 
		 * a - Left Hand Point
		 * b - Middle Point
		 * c - Right Hand Point
		 * return int Direction -1 -> Left hand turn 0 -> No turn 1 -> Right hand turn
		 */
		int turnDirections(geos::geom::Coordinate *a, geos::geom::Coordinate *b, geos::geom::Coordinate *c);
		bool removeItem(std::vector<geos::geom::Coordinate> *coordinates, geos::geom::Coordinate coord);
		/**
		 * Find the straight line between the two points
		 * y = mx + c
		 */
		void findLine(geos::geom::Coordinate *p1, geos::geom::Coordinate *p2, double *m, double *c);
		void findPointOnLine(geos::geom::Coordinate *p1, geos::geom::Coordinate *p2, float distance, geos::geom::Coordinate *p3);
		void findPointOnLine(geos::geom::Coordinate *p1, geos::geom::Coordinate *p2, float distance, std::list<RSGIS2DPoint*> *pts);
		void findPointOnLine(geos::geom::Coordinate *p1, geos::geom::Coordinate *p2, float distance, geos::geom::CoordinateSequence *coords);
		void findPoint2Side(geos::geom::Coordinate *pStart, geos::geom::Coordinate *p2, geos::geom::Coordinate *pEnd, float distance, geos::geom::Coordinate *p3);
		geos::geom::Envelope* getEnvelope(geos::geom::Geometry *geom);
		bool findClosestIntersect(std::vector<geos::geom::LineSegment> *lines, geos::geom::LineSegment *line, geos::geom::Coordinate *pt, geos::geom::Coordinate *coord);
		void covert2LineSegments(geos::geom::Polygon *poly, std::vector<geos::geom::LineSegment> *lines);
		void dissolveMergeSmallPolygons(std::vector<geos::geom::Polygon*> *polygons, std::vector<geos::geom::Polygon*> *smallPolygons, RSGISIdentifyNonConvexPolygons *nonconvexoutline);
		void mergeWithNearest(std::vector<geos::geom::Polygon*> *polygons, std::vector<geos::geom::Polygon*> *polygonsToMerge, RSGISIdentifyNonConvexPolygons *identifyNonConvexPolygon);
		void mergeWithNeighbor(std::vector<geos::geom::Polygon*> *polygons, std::vector<geos::geom::Polygon*> *polygonsToMerge, float relBorderThreshold, RSGISIdentifyNonConvexPolygons *nonconvexoutline);
		float calcRelativeBorder(geos::geom::Polygon *poly1, geos::geom::Polygon *poly2);
		geos::geom::Polygon* mergePolygons(geos::geom::Polygon *poly1, geos::geom::Polygon *poly2, RSGISIdentifyNonConvexPolygons *nonconvexoutline);
		geos::geom::Polygon* mergePolygons(geos::geom::Polygon *poly1, std::vector<geos::geom::Polygon*> *polygons, RSGISIdentifyNonConvexPolygons *nonconvexoutline);
		void findContaining(geos::geom::Polygon *outline, std::vector<geos::geom::Polygon*> *polygons, std::vector<geos::geom::Polygon*> *outPolygons);
		void removeOverlaps(std::vector<geos::geom::Polygon*> *polygons, geos::geom::Envelope *env, float tolerance, float dissolveThreshold);
		geos::geom::Polygon* createHole(const geos::geom::Polygon *poly, const geos::geom::Polygon *hole);
		geos::geom::Polygon* insertIntersectNodes(geos::geom::Polygon *poly, geos::geom::Polygon *intersect);
		double minDistanceBetweenPoints(geos::geom::Polygon *poly);
		geos::geom::Polygon* snapToXYGrid(geos::geom::Polygon *poly, double tolerance, bool calcTolerance, geos::geom::Envelope *env);
		void printPolygonCoords2File(geos::geom::Polygon *poly, std::string filename);
		void retrievePolygons(geos::geom::Geometry *geom, std::vector<geos::geom::Polygon*> *polygons);
		void retrieveLines(geos::geom::Geometry *geom, std::vector<geos::geom::LineString*> *lines);
		void performMorphologicalOperation(std::vector<geos::geom::Polygon*> *polygons, Morphology morphology, float buffer, int curveSegments);
		void performMorphologicalOperation(std::vector<geos::geom::Geometry*> *geometries, Morphology morphology, float buffer, int curveSegments);
		/** Find the Euclidian distance between two points in 3D space (r in a sperical coordinate system) */ 
		double get3DlineLenght(geos::geom::LineSegment *inLine);
		/** 
		 * Find the zenith (phi) and azimuth (theta) angle of a line in 3D space. May also be used for converting from
		 * Cartesian to Spherical coordinates.
		 * A full explanation is provided here http://mathworld.wolfram.com/SphericalCoordinates.html
		 */
		double get3DLineZenithAngle(geos::geom::LineSegment *inLine);
		double get3DlineAzimuthAngle(geos::geom::LineSegment *inLine);
		double get3DLineZenithAngleDeg(geos::geom::LineSegment *inLine);
		double get3DlineAzimuthAngleDeg(geos::geom::LineSegment *inLine);
		std::vector<geos::geom::Polygon*>* polygonsBasicClean(std::vector<geos::geom::Polygon*> *polygons);
		std::vector<geos::geom::Polygon*>* polygonsSnapToXYGrid(std::vector<geos::geom::Polygon*> *polygons, float tolerance);
		geos::geom::Geometry* getIntersection(geos::geom::Polygon *poly1, geos::geom::Polygon *poly2);
		geos::geom::Geometry* getIntersection(geos::geom::Geometry *geom1, geos::geom::Geometry *geom2);
		geos::geom::Geometry* getDifference(geos::geom::Geometry *geom1, geos::geom::Geometry *geom2);
		/// Merge touching polygons
		void mergeTouchingPolygons(std::vector<geos::geom::Polygon*> *polys);
		/// Merge touching polygons
		void mergeTouchingPolygonsWithIndex(std::vector<geos::geom::Polygon*> *polys);
		/// Merge touching polygons, ignores geos topography warnings
		void mergeTouchingPolygonsForce(std::vector<geos::geom::Polygon*> *polys);
		geos::geom::Polygon* polygonUnion(geos::geom::Polygon *poly1, geos::geom::Polygon *poly2);
		bool shareBorder(geos::geom::Polygon *poly1, geos::geom::Polygon *poly2);
		bool equalLineSegments(geos::geom::LineSegment *line1, geos::geom::LineSegment *line2, float threshold);
		bool straightLine(RSGIS2DPoint *p1, RSGIS2DPoint *p2, RSGIS2DPoint *p3);
		float amountOfOverlap(geos::geom::Polygon *poly, std::vector<geos::geom::Polygon*> *polys);
		void mergeTouchingPolygons(std::vector<geos::geom::Polygon*> *polys, float sizeThreshold);
		bool geometryBetweenFast(geos::geom::Polygon *poly1, geos::geom::Polygon *poly2, geos::geom::Geometry *master);
		bool geometryBetweenAccurate(geos::geom::Polygon *poly1, geos::geom::Polygon *poly2, geos::geom::Geometry *master);
		geos::geom::Polygon* addNodes(geos::geom::Polygon *poly, int nodeStep);
		std::vector<geos::geom::LineSegment*>* findLineProj(geos::geom::Polygon *poly);
		std::vector<geos::geom::LineSegment*>* findLineProj(std::vector<geos::geom::Coordinate*> *coords, float lineLength);
		void removeNeighborDuplicates(std::vector<geos::geom::Coordinate*> *coords);
		/// Check for overlaping polygons
		float overlap(geos::geom::Geometry *geom1, geos::geom::Geometry *geom2);
		/// Check for overlaping polygons, ignore geos topography warnings
		float overlapForce(geos::geom::Geometry *geom1, geos::geom::Geometry *geom2);
		geos::geom::Polygon* removeHoles(geos::geom::Polygon* poly);
        std::vector<geos::geom::Polygon*>* removeHoles(std::vector<geos::geom::Polygon*> *polys);
        void convertGeometryCollection2Lines(OGRGeometryCollection *geomCollectionIn, OGRGeometryCollection *geomCollectionOut);
        void convertPolygons2Lines(OGRPolygon *poly, OGRGeometryCollection *geomCollectionOut);
        std::vector<geos::geom::Polygon*>* simplifyPolygons(std::vector<geos::geom::Polygon*> *polys, double distTol);
        //std::vector<geos::geom::Polygon*>* simplifyTopologyPolygons(std::vector<geos::geom::Polygon*> *polys);
		geos::geom::Polygon* applyOffset2Polygon(geos::geom::Polygon *poly, double xOff, double yOff);
        std::vector<geos::geom::Polygon*>* applyOffset2Polygons(std::vector<geos::geom::Polygon*> *polys, double xOff, double yOff);
        ~RSGISGeometry();
	};
	
}}

#endif
