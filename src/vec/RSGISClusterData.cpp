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
		this->classListCluster = new vector<ClassInfo>();
		this->classListAll = new vector<ClassInfo>();
		this->area = 0;
		this->clusterPolyArea = 0;
		this->allPolyArea = 0;
		this->propClusterPolyArea = 0;
		this->propAllPolyArea = 0;
	}
	
	RSGISClusterData::RSGISClusterData(Polygon *poly, vector<RSGISClassificationPolygon*> *clusterPolys, vector<RSGISClassificationPolygon*> *allIntersect)
	{
		this->clusterPolys = clusterPolys;
		this->allIntersect = allIntersect;
		
		this->setPolygon(poly);
		
		this->area = poly->getArea();
		this->clusterPolyArea = 0;
		this->allPolyArea = 0;
		this->propClusterPolyArea = 0;
		this->propAllPolyArea = 0;
		
		vector<RSGISClassificationPolygon*>::iterator iterClassPolys;
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
		
		this->classListCluster = new vector<ClassInfo>();
		this->classListAll = new vector<ClassInfo>();
		
		this->findClassInfo(clusterPolys, classListCluster);
		this->findClassInfo(allIntersect, classListAll);
		
		vector<ClassInfo>::iterator iterClassInfo;
		cout << "Cluster Crowns Info (" << classListCluster->size() << "):\n";
		for(iterClassInfo = classListCluster->begin(); iterClassInfo != classListCluster->end(); ++iterClassInfo)
		{
			cout << (*iterClassInfo).name << ": " << (*iterClassInfo).proportion << endl;
		}
		
		cout << "All intersecting Crowns Info (" << classListAll->size() << "):\n";
		for(iterClassInfo = classListAll->begin(); iterClassInfo != classListAll->end(); ++iterClassInfo)
		{
			cout << (*iterClassInfo).name << ": " << (*iterClassInfo).proportion << endl;
		}

				
	}
	
	void RSGISClusterData::readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
		/*RSGISMathsUtils mathUtils;
		
		// Read the class attribute
		OGRFieldDefn *fieldDef = NULL;
		string columnName = "";
		int fieldCount = featDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			fieldDef = feature->GetFieldDefnRef(i);
			columnName = fieldDef->GetNameRef();
			if(columnName == "ClustClass")
			{
				this->clusterClass = string(feature->GetFieldAsString(i));
			}
			else if(columnName == "Area")
			{
				this->area = mathUtils.strtofloat(string(feature->GetFieldAsString(i)));
			}
			else if(columnName == "PolysArea")
			{
				this->polysArea = mathUtils.strtofloat(string(feature->GetFieldAsString(i)));
			}
			else if(columnName == "ProPolysAr")
			{
				this->proPolysArea = mathUtils.strtofloat(string(feature->GetFieldAsString(i)));
			}
			else if(columnName == "ProTotalAr")
			{
				this->proTotalArea = mathUtils.strtofloat(string(feature->GetFieldAsString(i)));
			}
			else if(columnName == "EdgeThres")
			{
				this->edgeThreshold = mathUtils.strtofloat(string(feature->GetFieldAsString(i)));
			}
		}
		 */
	}
	
	void RSGISClusterData::createLayerDefinition(OGRLayer *outputSHPLayer)throw(RSGISVectorOutputException)
	{
		/*
		// Create class attribute column
		OGRFieldDefn shpField1 = OGRFieldDefn( "ClustClass", OFTString );
		shpField1.SetWidth(100);
		if( outputSHPLayer->CreateField( &shpField1 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"ClustClass\" has failed");
		}
		
		OGRFieldDefn shpField2 = OGRFieldDefn( "Area", OFTReal );
		shpField2.SetPrecision(4);
		if( outputSHPLayer->CreateField( &shpField2 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"Area\" has failed");
		}
		
		OGRFieldDefn shpField3 = OGRFieldDefn( "PolysArea", OFTReal );
		shpField3.SetPrecision(4);
		if( outputSHPLayer->CreateField( &shpField3 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"PolysArea\" has failed");
		}
		
		OGRFieldDefn shpField4 = OGRFieldDefn( "ProPolysAr", OFTReal );
		shpField4.SetPrecision(4);
		if( outputSHPLayer->CreateField( &shpField4 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"ProPolysAr\" has failed");
		}
		
		OGRFieldDefn shpField5 = OGRFieldDefn( "ProTotalAr", OFTReal );
		shpField5.SetPrecision(4);
		if( outputSHPLayer->CreateField( &shpField5 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"ProTotalAr\" has failed");
		}
		
		OGRFieldDefn shpField6 = OGRFieldDefn( "EdgeThres", OFTReal );
		shpField6.SetPrecision(4);
		if( outputSHPLayer->CreateField( &shpField6 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"EdgeThreshold\" has failed");
		}
		 */
	}
	
	void RSGISClusterData::populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
		/*
		RSGISVectorUtils vecUtils;
		
		if(polygonType)
		{
			feature->SetGeometryDirectly(vecUtils.convertGEOSPolygon2OGRPolygon(polygonGeom));
		}
		else
		{
			feature->SetGeometryDirectly(vecUtils.convertGEOSMultiPolygon2OGRMultiPolygon(multiPolygonGeom));
		}
		
		// Set class
		feature->SetField(featDefn->GetFieldIndex("ClustClass"), clusterClass.c_str());
		feature->SetField(featDefn->GetFieldIndex("Area"), area);
		feature->SetField(featDefn->GetFieldIndex("PolysArea"), polysArea);
		feature->SetField(featDefn->GetFieldIndex("ProPolysAr"), proPolysArea);
		feature->SetField(featDefn->GetFieldIndex("ProTotalAr"), proTotalArea);
		feature->SetField(featDefn->GetFieldIndex("EdgeThres"), edgeThreshold);
		 */
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
	
	vector<ClassInfo>* RSGISClusterData::getClassListCluster()
	{
		return classListCluster;
	}

	
	vector<ClassInfo>* RSGISClusterData::getClassListAll()
	{
		return classListAll;
	}

	vector<RSGISClassificationPolygon*>* RSGISClusterData::getClusterPolys()
	{
		return clusterPolys;
	}
	
	vector<RSGISClassificationPolygon*>* RSGISClusterData::getAllIntersect()
	{
		return allIntersect;
	}
	
	string RSGISClusterData::getDominateClassCluster()
	{
		string dominate;
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
	
	string RSGISClusterData::getDominateClassAll()
	{
		string dominate;
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
	
	void RSGISClusterData::findClassInfo(vector<RSGISClassificationPolygon*> *polys, vector<ClassInfo> *classList)
	{
		vector<RSGISClassificationPolygon*>::iterator iterClassPolys;
		vector<string> *classes = new vector<string>();
		vector<string>::iterator iterClassNames;
		string name = "";
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
		
		vector<float> proportions;
		for(unsigned int i = 0; i < classes->size(); ++i)
		{
			classesInfo[i].proportion = classesInfo[i].totalArea / totalPolyArea;
			proportions.push_back(classesInfo[i].proportion);
		}
		
		sort(proportions.begin(), proportions.end());
		
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





