/*
 *  RSGISCmdVectorUtils.h
 *
 *
 *  Created by Pete Bunting on 04/07/2013.
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

#ifndef RSGISCmdVectorUtils_H
#define RSGISCmdVectorUtils_H

#include <iostream>
#include <string>
#include <vector>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

namespace rsgis{ namespace cmds{
    
    struct DllExport RSGISVariableFieldCmds
    {
        std::string name;
        std::string fieldName;
    };
    
    
    /** Function to produce convex hulls for groups of (X, Y, Attribute) point locations */
    DllExport void executeGenerateConvexHullsGroups(std::string inputFile, std::string outputVector, std::string outVecProj, bool force, unsigned int eastingsColIdx, unsigned int northingsColIdx, unsigned int attributeColIdx)throw(RSGISCmdException);
    /** Function to copy geometry but remove attributes */
    DllExport void executeRemoveAttributes(std::string inputVector, std::string outputVector, bool force)throw(RSGISCmdException);
    /** Function to buffer vector */
    DllExport void executeBufferVector(std::string inputVector, std::string outputVector, float bufferDist, bool force)throw(RSGISCmdException);
    /** Function to print polygon geometry */
    DllExport void executePrintPolyGeom(std::string inputVector) throw(RSGISCmdException);
    /** Function to find and replace text within an attribute table */
    DllExport void executeFindReplaceText(std::string inputVector, std::string attribute, std::string find, std::string replace) throw(RSGISCmdException);
    /** Function to calculate polygon area */
    DllExport void executeCalcPolyArea(std::string inputVector, std::string outputVector, bool force) throw(RSGISCmdException);
    /** Split polygons in in vector by polygons in cover vector */
    DllExport void executePolygonsInPolygon(std::string inputVector, std::string inputCoverVector, std::string output_DIR, std::string attributeName, bool force) throw(RSGISCmdException);
    /** Populate the Z field on the vector geometries */
    DllExport void executePopulateGeomZField(std::string inputVector, std::string inputImage, unsigned int imgBand, std::string outputVector, bool force) throw(RSGISCmdException);
    /** Function to calculate a maths functions between  */
    DllExport void executeVectorMaths(std::string inputVector, std::string outputVector, std::string outColumn, std::string expression, bool force, std::vector<RSGISVariableFieldCmds> vars) throw(RSGISCmdException);
    /** Function to add a FID column to a shapefile  */
    DllExport void executeAddFIDColumn(std::string inputVector, std::string outputVector, bool force) throw(RSGISCmdException);
    /** Function to find the common extent of list of images and create a shapefile for the extent  */
    DllExport void executeFindCommonImgExtent(std::vector<std::string> inputImages, std::string outputVector, bool force) throw(RSGISCmdException);
    /** Function to split the polygons within a shapefile into different shapefiles using an attribute  */
    DllExport void executeSplitFeatures(std::string inputVector, std::string outputVectorBase, bool force) throw(RSGISCmdException);
    /** Function to export a binary image to points */
    DllExport void executeExportPxls2Pts(std::string inputImage, std::string outputVec, bool force, float maskVal) throw(RSGISCmdException);
    /** Function to calculate the distance to the nearest geometry */
    DllExport double executeCalcDist2NearestGeom(std::string inputVec, std::string outputVec, bool force) throw(RSGISCmdException);
    /** Function to calculate the distance to the nearest geometry */
    DllExport double executeCalcMaxDist2NearestGeom(std::string inputVec) throw(RSGISCmdException);
    /** Function spaitally cluster points using a graph */
    DllExport void executeSpatialGraphClusterGeoms(std::string inputVec, std::string outputVec, bool useMinSpanTree, float edgeLenSDThres, double maxEdgeLen, bool force, std::string shpFileEdges="", bool outShpEdges=false, std::string h5EdgeLengths="", bool outH5EdgeLens=false) throw(RSGISCmdException);
    /** Function to create a polygon from a set of points */
    DllExport void executeFitPolygonToPoints(std::string inputVec, std::string outputVec, double alphaVal, bool force) throw(RSGISCmdException);
    /** Function to create a set of polygons from a set of point which have been clustered */
    DllExport void executeFitPolygonsToPointClusters(std::string inputVec, std::string outputVec, std::string clusterField, double alphaVal, bool force) throw(RSGISCmdException);
    /** Function to convert a set of lines into regularly spaced set of points */
    DllExport void executeCreateLinesOfPoints(std::string inputLinesVec, std::string outputPtsVec, double step, bool force) throw(RSGISCmdException);
    /** Function to adjust polygon boundaries using active contours */
    DllExport void executeFitActiveContourBoundaries(std::string inputPolysVec, std::string outputPolysVec, std::string externalForceImg, double interAlpha, double interBeta, double interGamma, double minExtThres, bool force) throw(RSGISCmdException);
}}


#endif

