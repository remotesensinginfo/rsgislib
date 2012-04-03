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

using namespace std;
using namespace geos::geom;
using namespace geos::util;
using namespace geos::algorithm;
using namespace rsgis::math;
using namespace geos::index::quadtree;
using namespace rsgis::utils;

namespace rsgis{namespace geom{

	struct SortCoordinates : binary_function< Coordinate, Coordinate, bool >
	{
		const Coordinate coord;
		SortCoordinates( const Coordinate& coord_ ): coord( coord_ ) { }
		bool operator()( const Coordinate& a, const Coordinate& b ) const
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
	
	struct point2D
	{
		double x;
		double y;
	};
	
	struct point3D
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
	
	class RSGISGeometry
	{
	public:
		RSGISGeometry();
		void fitPlane2Points(point3D *pts, int numPts, double *a, double *b, double *c) throw(RSGISGeometryException);
		double calcPlaneFit(point3D *pts, int numPts, double a, double b, double c) throw(RSGISGeometryException);
		double calcPlaneZAt(double a, double b, double c, double x, double y) throw(RSGISGeometryException);
		double distance2D(point3D *pt1, point2D *pt2);
		Polygon* createCircle(float a, float b, float r, float resolution)throw(RSGISGeometryException);
		Polygon* createPolygon(float xTL, float yTL, float xBR, float yBR);
		//SortedGenericList<double>* findValuesOnEdge(Matrix *matrix, Polygon **bboxes, Polygon *poly);
		Polygon** findMaxPointsOnPolyEdge(Matrix *matrix, Polygon **bboxes, Polygon *poly, int *numPolys);
		Envelope* findBBox(list<RSGIS2DPoint*> *pts) throw(RSGISGeometryException);
		Envelope* findBBox(vector<RSGIS2DPoint*> *pts) throw(RSGISGeometryException);
		RSGISTriangle* findBoundingTriangle(list<RSGIS2DPoint*> *pts) throw(RSGISGeometryException);
		RSGISTriangle* findBoundingTriangle(vector<RSGIS2DPoint*> *pts) throw(RSGISGeometryException);
		double calcAngle(RSGIS2DPoint *pbase, RSGIS2DPoint *pt);
		MultiPolygon* createMultiPolygon(const vector<Geometry*> *polygons);
		Polygon* findConvexHull(vector<Polygon*> *geoms) throw(RSGISGeometryException);
		Polygon* findConvexHull(list<Polygon*> *geoms) throw(RSGISGeometryException);
		Polygon* findConvexHull(const vector<Geometry*> *geometries) throw(RSGISGeometryException);
		Polygon* findConvexHull(vector<LineSegment> *lines) throw(RSGISGeometryException);
		Polygon* findConvexHull(vector<Coordinate> *coordinates) throw(RSGISGeometryException);
		/** 
		 * turnDirection. A method to identify which direction the middle point b 
		 * is turning. 
		 * a - Left Hand Point
		 * b - Middle Point
		 * c - Right Hand Point
		 * return int Direction -1 -> Left hand turn 0 -> No turn 1 -> Right hand turn
		 */
		int turnDirections(Coordinate *a, Coordinate *b, Coordinate *c);
		bool removeItem(vector<Coordinate> *coordinates, Coordinate coord);
		/**
		 * Find the straight line between the two points
		 * y = mx + c
		 */
		void findLine(Coordinate *p1, Coordinate *p2, double *m, double *c);
		void findPointOnLine(Coordinate *p1, Coordinate *p2, float distance, Coordinate *p3);
		void findPointOnLine(Coordinate *p1, Coordinate *p2, float distance, list<RSGIS2DPoint*> *pts);
		void findPointOnLine(Coordinate *p1, Coordinate *p2, float distance, CoordinateSequence *coords);
		void findPoint2Side(Coordinate *pStart, Coordinate *p2, Coordinate *pEnd, float distance, Coordinate *p3);
		Envelope* getEnvelope(Geometry *geom);
		bool findClosestIntersect(vector<LineSegment> *lines, LineSegment *line, Coordinate *pt, Coordinate *coord);
		void covert2LineSegments(Polygon *poly, vector<LineSegment> *lines);
		void dissolveMergeSmallPolygons(vector<Polygon*> *polygons, vector<Polygon*> *smallPolygons, RSGISIdentifyNonConvexPolygons *nonconvexoutline) throw(RSGISGeometryException);
		void mergeWithNearest(vector<Polygon*> *polygons, vector<Polygon*> *polygonsToMerge, RSGISIdentifyNonConvexPolygons *identifyNonConvexPolygon) throw(RSGISGeometryException);
		void mergeWithNeighbor(vector<Polygon*> *polygons, vector<Polygon*> *polygonsToMerge, float relBorderThreshold, RSGISIdentifyNonConvexPolygons *nonconvexoutline) throw(RSGISGeometryException);
		float calcRelativeBorder(Polygon *poly1, Polygon *poly2) throw(RSGISGeometryException);
		Polygon* mergePolygons(Polygon *poly1, Polygon *poly2, RSGISIdentifyNonConvexPolygons *nonconvexoutline) throw(RSGISGeometryException);
		Polygon* mergePolygons(Polygon *poly1, vector<Polygon*> *polygons, RSGISIdentifyNonConvexPolygons *nonconvexoutline) throw(RSGISGeometryException);
		void findContaining(Polygon *outline, vector<Polygon*> *polygons, vector<Polygon*> *outPolygons) throw(RSGISGeometryException);
		void removeOverlaps(vector<Polygon*> *polygons, Envelope *env, float tolerance, float dissolveThreshold) throw(RSGISGeometryException);
		Polygon* createHole(const Polygon *poly, const Polygon *hole) throw(RSGISGeometryException);
		Polygon* insertIntersectNodes(Polygon *poly, Polygon *intersect) throw(RSGISGeometryException);
		double minDistanceBetweenPoints(Polygon *poly) throw(RSGISGeometryException);
		Polygon* snapToXYGrid(Polygon *poly, double tolerance, bool calcTolerance, Envelope *env) throw(RSGISGeometryException);
		void printPolygonCoords2File(Polygon *poly, string filename);
		void retrievePolygons(Geometry *geom, vector<Polygon*> *polygons) throw(RSGISGeometryException);
		void retrieveLines(Geometry *geom, vector<LineString*> *lines) throw(RSGISGeometryException);
		void performMorphologicalOperation(vector<Polygon*> *polygons, Morphology morphology, float buffer, int curveSegments) throw(RSGISGeometryException);
		void performMorphologicalOperation(vector<Geometry*> *geometries, Morphology morphology, float buffer, int curveSegments) throw(RSGISGeometryException);
		/** Find the Euclidian distance between two points in 3D space (r in a sperical coordinate system) */ 
		double get3DlineLenght(LineSegment *inLine);
		/** 
		 * Find the zenith (phi) and azimuth (theta) angle of a line in 3D space. May also be used for converting from
		 * Cartesian to Spherical coordinates.
		 * A full explanation is provided here http://mathworld.wolfram.com/SphericalCoordinates.html
		 */
		double get3DLineZenithAngle(LineSegment *inLine);
		double get3DlineAzimuthAngle(LineSegment *inLine);
		double get3DLineZenithAngleDeg(LineSegment *inLine);
		double get3DlineAzimuthAngleDeg(LineSegment *inLine);
		vector<Polygon*>* polygonsBasicClean(vector<Polygon*> *polygons) throw(RSGISGeometryException);
		vector<Polygon*>* polygonsSnapToXYGrid(vector<Polygon*> *polygons, float tolerance) throw(RSGISGeometryException);
		Geometry* getIntersection(Polygon *poly1, Polygon *poly2) throw(RSGISGeometryException);
		Geometry* getIntersection(Geometry *geom1, Geometry *geom2) throw(RSGISGeometryException);
		Geometry* getDifference(Geometry *geom1, Geometry *geom2) throw(RSGISGeometryException);
		/// Merge touching polygons
		void mergeTouchingPolygons(vector<Polygon*> *polys) throw(RSGISGeometryException);
		/// Merge touching polygons
		void mergeTouchingPolygonsWithIndex(vector<Polygon*> *polys) throw(RSGISGeometryException);
		/// Merge touching polygons, ignores geos topography warnings
		void mergeTouchingPolygonsForce(vector<Polygon*> *polys) throw(RSGISGeometryException);
		Polygon* polygonUnion(Polygon *poly1, Polygon *poly2) throw(RSGISGeometryException);
		bool shareBorder(Polygon *poly1, Polygon *poly2) throw(RSGISGeometryException);
		bool equalLineSegments(LineSegment *line1, LineSegment *line2, float threshold) throw(RSGISGeometryException);
		bool straightLine(RSGIS2DPoint *p1, RSGIS2DPoint *p2, RSGIS2DPoint *p3) throw(RSGISGeometryException);
		float amountOfOverlap(Polygon *poly, vector<Polygon*> *polys) throw(RSGISGeometryException);
		void mergeTouchingPolygons(vector<Polygon*> *polys, float sizeThreshold) throw(RSGISGeometryException);
		bool geometryBetweenFast(Polygon *poly1, Polygon *poly2, Geometry *master) throw(RSGISGeometryException);
		bool geometryBetweenAccurate(Polygon *poly1, Polygon *poly2, Geometry *master) throw(RSGISGeometryException);
		Polygon* addNodes(Polygon *poly, int nodeStep) throw(RSGISGeometryException);
		vector<LineSegment*>* findLineProj(Polygon *poly) throw(RSGISGeometryException);
		vector<LineSegment*>* findLineProj(vector<Coordinate*> *coords, float lineLength) throw(RSGISGeometryException);
		void removeNeighborDuplicates(vector<Coordinate*> *coords);
		/// Check for overlaping polygons
		float overlap(Geometry *geom1, Geometry *geom2) throw(RSGISGeometryException);
		/// Check for overlaping polygons, ignore geos topography warnings
		float overlapForce(Geometry *geom1, Geometry *geom2) throw(RSGISGeometryException);
		Polygon* removeHoles(Polygon* poly);
        vector<Polygon*>* removeHoles(vector<Polygon*> *polys);
        void convertGeometryCollection2Lines(OGRGeometryCollection *geomCollectionIn, OGRGeometryCollection *geomCollectionOut) throw(RSGISGeometryException);
        void convertPolygons2Lines(OGRPolygon *poly, OGRGeometryCollection *geomCollectionOut) throw(RSGISGeometryException);
        vector<Polygon*>* simplifyPolygons(vector<Polygon*> *polys, double distTol);
        //vector<Polygon*>* simplifyTopologyPolygons(vector<Polygon*> *polys);
		Polygon* applyOffset2Polygon(Polygon *poly, double xOff, double yOff);
        vector<Polygon*>* applyOffset2Polygons(vector<Polygon*> *polys, double xOff, double yOff);
        ~RSGISGeometry();
	};
	
}}

#endif
