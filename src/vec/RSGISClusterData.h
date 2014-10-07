/*
 *  RSGISClusterData.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 13/09/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
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

#ifndef RSGISClusterData_H
#define RSGISClusterData_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

#include "vec/RSGISPolygonData.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISClassificationPolygon.h"

#include "math/RSGISMathsUtils.h"

namespace rsgis{namespace vec{
	
	struct DllExport ClassInfo
	{
		std::string name;
		float proportion;
		double totalArea;
	};
	
	class DllExport RSGISClusterData : public RSGISPolygonData
	{
	public:
		RSGISClusterData();
		RSGISClusterData(geos::geom::Polygon *poly, std::vector<RSGISClassificationPolygon*> *clusterPolys, std::vector<RSGISClassificationPolygon*> *allIntersect);
		virtual void readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn);
		virtual void createLayerDefinition(OGRLayer *outputSHPLayer)throw(RSGISVectorOutputException);
		virtual void populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn);
		float getArea();
		float getClusterPolyArea();
		float getAllPolyArea();
		float getPropClusterPolyArea();
		float getPropAllPolyArea();
		std::vector<ClassInfo>* getClassListCluster();
		std::vector<ClassInfo>* getClassListAll();
		std::vector<RSGISClassificationPolygon*>* getClusterPolys();
		std::vector<RSGISClassificationPolygon*>* getAllIntersect();
		std::string getDominateClassCluster();
		std::string getDominateClassAll();
		~RSGISClusterData();
	protected:
		void findClassInfo(std::vector<RSGISClassificationPolygon*> *polys, std::vector<ClassInfo> *classList);
		std::vector<RSGISClassificationPolygon*> *clusterPolys;
		std::vector<RSGISClassificationPolygon*> *allIntersect;
		std::vector<ClassInfo> *classListCluster;
		std::vector<ClassInfo> *classListAll;
		float area;
		float clusterPolyArea;
		float allPolyArea;
		float propClusterPolyArea;
		float propAllPolyArea;
	};
}}

#endif



