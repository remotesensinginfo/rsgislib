/*
 *  RSGISClusterData.cpp
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

#include "RSGISClusterData.h"



namespace rsgis{namespace vec{
	
	
	RSGISClusterData::RSGISClusterData() : RSGISPolygonData()
	{
		this->clusterPolys = NULL;
		this->allIntersect = NULL;
		this->classListCluster = new std::vector<ClassInfo>();
		this->classListAll = new std::vector<ClassInfo>();
		this->area = 0;
		this->clusterPolyArea = 0;
		this->allPolyArea = 0;
		this->propClusterPolyArea = 0;
		this->propAllPolyArea = 0;
	}
	
	RSGISClusterData::RSGISClusterData(geos::geom::Polygon *poly, std::vector<RSGISClassificationPolygon*> *clusterPolys, std::vector<RSGISClassificationPolygon*> *allIntersect)
	{
		this->clusterPolys = clusterPolys;
		this->allIntersect = allIntersect;
		
		this->setPolygon(poly);
		
		this->area = poly->getArea();
		this->clusterPolyArea = 0;
		this->allPolyArea = 0;
		this->propClusterPolyArea = 0;
		this->propAllPolyArea = 0;
		
		std::vector<RSGISClassificationPolygon*>::iterator iterClassPolys;
		for(iterClassPolys = clusterPolys->begin(); iterClassPolys != clusterPolys->end(); ++iterClassPolys)
		{
			this->clusterPolyArea = this->clusterPolyArea + (*iterClassPolys)->getPolygon()->getArea();
		}
		
		for(iterClassPolys = allIntersect->begin(); iterClassPolys != allIntersect->end(); ++iterClassPolys)
		{
			this->allPolyArea = this->allPolyArea + (*iterClassPolys)->getPolygon()->getArea();
		}
		
		this->propClusterPolyArea = this->clusterPolyArea / this->area;
		this->propAllPolyArea = this->allPolyArea / this->area;
		
		this->classListCluster = new std::vector<ClassInfo>();
		this->classListAll = new std::vector<ClassInfo>();
		
		this->findClassInfo(clusterPolys, classListCluster);
		this->findClassInfo(allIntersect, classListAll);
		
		std::vector<ClassInfo>::iterator iterClassInfo;
		std::cout << "Cluster Crowns Info (" << classListCluster->size() << "):\n";
		for(iterClassInfo = classListCluster->begin(); iterClassInfo != classListCluster->end(); ++iterClassInfo)
		{
			std::cout << (*iterClassInfo).name << ": " << (*iterClassInfo).proportion << std::endl;
		}
		
		std::cout << "All intersecting Crowns Info (" << classListAll->size() << "):\n";
		for(iterClassInfo = classListAll->begin(); iterClassInfo != classListAll->end(); ++iterClassInfo)
		{
			std::cout << (*iterClassInfo).name << ": " << (*iterClassInfo).proportion << std::endl;
		}

				
	}
	
	void RSGISClusterData::readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
		
	}
	
	void RSGISClusterData::createLayerDefinition(OGRLayer *outputSHPLayer)throw(RSGISVectorOutputException)
	{
		
	}
	
	void RSGISClusterData::populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
		
	}

	float RSGISClusterData::getArea()
	{
		return this->area;
	}
	
	float RSGISClusterData::getClusterPolyArea()
	{
		return this->clusterPolyArea;
	}
	
	float RSGISClusterData::getAllPolyArea()
	{
		return this->allPolyArea;
	}
	
	float RSGISClusterData::getPropClusterPolyArea()
	{
		return this->propClusterPolyArea;
	}
	
	float RSGISClusterData::getPropAllPolyArea()
	{
		return this->propAllPolyArea;
	}
	
	std::vector<ClassInfo>* RSGISClusterData::getClassListCluster()
	{
		return classListCluster;
	}

	
	std::vector<ClassInfo>* RSGISClusterData::getClassListAll()
	{
		return classListAll;
	}

	std::vector<RSGISClassificationPolygon*>* RSGISClusterData::getClusterPolys()
	{
		return clusterPolys;
	}
	
	std::vector<RSGISClassificationPolygon*>* RSGISClusterData::getAllIntersect()
	{
		return allIntersect;
	}
	
	std::string RSGISClusterData::getDominateClassCluster()
	{
		std::string dominate;
		if(classListCluster->size() >= 1)
		{
			dominate = classListCluster->front().name;
		}
		else 
		{
			dominate = "NULL";
		}
		return dominate;
	}
	
	std::string RSGISClusterData::getDominateClassAll()
	{
		std::string dominate;
		if(classListAll->size() >= 1)
		{
			dominate = classListAll->front().name;
		}
		else 
		{
			dominate = "NULL";
		}
		return dominate;
	}
	
	void RSGISClusterData::findClassInfo(std::vector<RSGISClassificationPolygon*> *polys, std::vector<ClassInfo> *classList)
	{
		std::vector<RSGISClassificationPolygon*>::iterator iterClassPolys;
		std::vector<std::string> *classes = new std::vector<std::string>();
		std::vector<std::string>::iterator iterClassNames;
		std::string name = "";
		bool found = false;
		for(iterClassPolys = polys->begin(); iterClassPolys != polys->end(); ++iterClassPolys)
		{
			name = (*iterClassPolys)->getClassification();
			found = false;
			for(iterClassNames = classes->begin(); iterClassNames != classes->end(); ++iterClassNames)
			{
				if((*iterClassNames) == name)
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				classes->push_back(name);
			}
		}
		
		ClassInfo *classesInfo = new ClassInfo[classes->size()];
		for(unsigned int i = 0; i < classes->size(); ++i)
		{
			classesInfo[i].name = classes->at(i);
			classesInfo[i].proportion = 0;
			classesInfo[i].totalArea = 0;
		}
		
		int index = -1;
		int count = 0;
		double totalPolyArea = 0;
		for(iterClassPolys = polys->begin(); iterClassPolys != polys->end(); ++iterClassPolys)
		{
			name = (*iterClassPolys)->getClassification();
			index = -1;
			count = 0;
			for(iterClassNames = classes->begin(); iterClassNames != classes->end(); ++iterClassNames)
			{
				if((*iterClassNames) == name)
				{
					index = count;
					break;
				}
				++count;
			}
			if(index != -1)
			{
				classesInfo[index].totalArea += (*iterClassPolys)->getPolygon()->getArea();
			}
			totalPolyArea += (*iterClassPolys)->getPolygon()->getArea();
		}
		
		std::vector<float> proportions;
		for(unsigned int i = 0; i < classes->size(); ++i)
		{
			classesInfo[i].proportion = classesInfo[i].totalArea / totalPolyArea;
			proportions.push_back(classesInfo[i].proportion);
		}
		
        std::sort(proportions.begin(), proportions.end());
		
		for(unsigned int i = 0; i < proportions.size(); ++i)
		{
			for(unsigned int j = 0; j < classes->size(); ++j)
			{
				if(classesInfo[j].proportion == proportions[i])
				{
					classList->push_back(classesInfo[j]);
				}
			}
		}
		delete classes;
		delete[] classesInfo;
		
	}
	
	RSGISClusterData::~RSGISClusterData()
	{
		if(this->clusterPolys != NULL)
		{
			this->clusterPolys->clear();
			delete clusterPolys;
		}
		if(this->allIntersect != NULL)
		{
			this->allIntersect->clear();
			delete allIntersect;
		}
		
		
		this->classListCluster->clear();
		delete this->classListCluster;
		this->classListAll->clear();
		delete this->classListAll;
	}
}}





