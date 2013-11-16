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

namespace rsgis{ namespace cmds {
    
    /** Function to produce convex hulls for groups of (X, Y, Attribute) point locations */
    void executeGenerateConvexHullsGroups(std::string inputFile, std::string outputVector, std::string outVecProj, bool force, unsigned int eastingsColIdx, unsigned int northingsColIdx, unsigned int attributeColIdx)throw(RSGISCmdException);
    /** Function to copy geometry but remove attributes */
    void executeRemoveAttributes(std::string inputVector, std::string outputVector, bool force)throw(RSGISCmdException);
    /** Function to buffer vector */
    void executeBufferVector(std::string inputVector, std::string outputVector, float bufferDist, bool force)throw(RSGISCmdException);
    /** Function to print polygon geometry */
    void executePrintPolyGeom(std::string inputVector) throw(RSGISCmdException);
    /** Function to find and replace text within an attribute table */
    void executeFindReplaceText(std::string inputVector, std::string attribute, std::string find, std::string replace) throw(RSGISCmdException);
    /** Function to calculate polygon area */
    void executeCalcPolyArea(std::string inputVector, std::string outputVector, bool force) throw(RSGISCmdException);
    /** Split polygons in in vector by polygons in cover vector */
    void excecutePolygonsInPolygon(std::string inputVector, std::string inputCoverVector, std::string output_DIR, std::string attributeName, bool force) throw(RSGISCmdException);
    
}}


#endif

