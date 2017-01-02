/*
 *  RSGISLabelPolygonsFromClassification.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/03/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#include "RSGISLabelPolygonsFromClassification.h"

namespace rsgis{namespace vec{
	
	RSGISLabelPolygonsFromClassification::RSGISLabelPolygonsFromClassification(RSGISClassPolygon **labelsClassData, unsigned long numFeatures, dominantspecies dominant)
	{
		this->labelsClassData = labelsClassData;
		this->numFeatures = numFeatures;
		this->dominant = dominant;
	}
	
	void RSGISLabelPolygonsFromClassification::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		RSGISVectorUtils vecUtils;
		OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
		OGRwkbGeometryType geometryType = inFeature->GetGeometryRef()->getGeometryType();
		
		if( geometryType == wkbPolygon )
		{
			OGRPolygon *ogrPolygon = (OGRPolygon *) inFeature->GetGeometryRef()->clone();
			geos::geom::Polygon *poly = vecUtils.convertOGRPolygon2GEOSPolygon(ogrPolygon);
			
			std::vector<std::string> speciesNames = std::vector<std::string>();
			std::vector<int> speciesCount = std::vector<int>();
			std::vector<float> speciesArea = std::vector<float>();
			std::vector<std::string>::iterator iterSpecies;
			bool found = false;
			int count = 0;
			int speciesIndex = 0;
			bool first = true;
			
			for(unsigned long i = 0; i < numFeatures; ++i)
			{
				if(poly->intersects(labelsClassData[i]->getPolygon()))
				{
					if(first)
					{
						speciesNames.push_back(labelsClassData[i]->getClassName());
						speciesCount.push_back(1);
						speciesArea.push_back(labelsClassData[i]->getPolygon()->getArea());
						first = false;
					}
					else
					{
						found = false;
						count = 0;
						for(iterSpecies = speciesNames.begin(); iterSpecies != speciesNames.end(); ++iterSpecies)
						{
							if(labelsClassData[i]->getClassName() == *iterSpecies)
							{
								found = true;
								speciesIndex = count;
								break;
							}
							++count;
						}
						
						
						if(found)
						{
							speciesCount[speciesIndex] = speciesCount[speciesIndex] + 1;
							speciesArea[speciesIndex] = speciesArea[speciesIndex] + labelsClassData[i]->getPolygon()->getArea();
						}
						else 
						{
							speciesNames.push_back(labelsClassData[i]->getClassName());
							speciesCount.push_back(1);
							speciesArea.push_back(labelsClassData[i]->getPolygon()->getArea());
						}
					}

				}
			}
			
			double totalArea = poly->getArea();
			double areaThreshold = totalArea * 0.66;
			
			delete poly;
	
			int firstCount = -1;
			int secondCount = -1;
			int thirdCount = -1;
			int firstArea = -1;
			int secondArea = -1;
			int thirdArea = -1;
			first = true;
			count = 0;
			for(iterSpecies = speciesNames.begin(); iterSpecies != speciesNames.end(); ++iterSpecies)
			{
				if(first)
				{
					firstCount = 0;
					firstArea = 0;
					first = false;
				}
				else 
				{
					if(speciesArea[count] > speciesArea[firstArea])
					{
						thirdArea = secondArea;
						secondArea = firstArea;
						firstArea = count;
					}
					else if(speciesArea[count] > speciesArea[secondArea])
					{
						thirdArea = secondArea;
						secondArea = count;
					}
					else if(speciesArea[count] > speciesArea[thirdArea])
					{
						thirdArea = count;
					}
					else if(secondArea == -1)
					{
						secondArea = count;
					}
					else if(thirdArea == -1)
					{
						thirdArea = count;
					}
					
					if(speciesCount[count] > speciesCount[firstCount])
					{
						thirdCount = secondCount;
						secondCount = firstCount;
						firstCount = count;
					}
					else if(speciesCount[count] > speciesCount[secondCount])
					{
						thirdCount = secondCount;
						secondCount = count;
					}
					else if(speciesCount[count] > speciesCount[thirdCount])
					{
						thirdCount = count;
					}
					else if(secondCount == -1)
					{
						secondCount = count;
					}
					else if(thirdCount == -1)
					{
						thirdCount = count;
					}
				}

				++count;
			}

            double areaRunTotal = 0;
			if(dominant == rsgis::vec::speciesCount)
			{
				std::string domSPStr = "";
				std::string domSPLTStr = "";
				
				if(firstCount != -1)
				{
					domSPStr += speciesNames[firstCount];
					domSPLTStr = speciesNames[firstArea];
					outFeature->SetField(outFeatureDefn->GetFieldIndex("DOMSP1"), speciesNames[firstCount].c_str());
					outFeature->SetField(outFeatureDefn->GetFieldIndex("CDOMSP1"), speciesCount[firstCount]);
					outFeature->SetField(outFeatureDefn->GetFieldIndex("ADOMSP1"), speciesArea[firstCount]);
				}
				areaRunTotal = speciesArea[firstCount];
				if(secondCount != -1)
				{
					if(areaRunTotal < areaThreshold)
					{
						domSPLTStr += speciesNames[secondCount];
					}
					
					domSPStr += speciesNames[secondCount];
					outFeature->SetField(outFeatureDefn->GetFieldIndex("DOMSP2"), speciesNames[secondCount].c_str());
					outFeature->SetField(outFeatureDefn->GetFieldIndex("CDOMSP2"), speciesCount[secondCount]);
					outFeature->SetField(outFeatureDefn->GetFieldIndex("ADOMSP2"), speciesArea[secondCount]);
				}
				areaRunTotal += speciesArea[secondCount];
				
				if(thirdCount != -1)
				{
					if(areaRunTotal < areaThreshold)
					{
						domSPLTStr += speciesNames[thirdCount];
					}
					
					domSPStr += speciesNames[thirdCount];
					outFeature->SetField(outFeatureDefn->GetFieldIndex("DOMSP3"), speciesNames[thirdCount].c_str());
					outFeature->SetField(outFeatureDefn->GetFieldIndex("CDOMSP3"), speciesCount[thirdCount]);
					outFeature->SetField(outFeatureDefn->GetFieldIndex("ADOMSP3"), speciesArea[thirdCount]);
				}
				
				outFeature->SetField(outFeatureDefn->GetFieldIndex("DOMSP"), domSPStr.c_str());
				outFeature->SetField(outFeatureDefn->GetFieldIndex("DOMSPLT"), domSPLTStr.c_str());
				outFeature->SetField(outFeatureDefn->GetFieldIndex("AREA"), totalArea);
			}
			else if(dominant == rsgis::vec::speciesArea)
			{
				
				std::string domSPStr = "";
				std::string domSPLTStr = "";
				
				if(firstArea != -1)
				{
					domSPStr = speciesNames[firstArea];
					domSPLTStr = speciesNames[firstArea];
					outFeature->SetField(outFeatureDefn->GetFieldIndex("DOMSP1"), speciesNames[firstArea].c_str());
					outFeature->SetField(outFeatureDefn->GetFieldIndex("CDOMSP1"), speciesCount[firstArea]);
					outFeature->SetField(outFeatureDefn->GetFieldIndex("ADOMSP1"), speciesArea[firstArea]);
				}

                areaRunTotal = speciesArea[firstArea];
				if(secondArea != -1)
				{
					if(areaRunTotal < areaThreshold)
					{
						domSPLTStr += speciesNames[secondArea];
					}
					
					domSPStr += speciesNames[secondArea];
					outFeature->SetField(outFeatureDefn->GetFieldIndex("DOMSP2"), speciesNames[secondArea].c_str());
					outFeature->SetField(outFeatureDefn->GetFieldIndex("CDOMSP2"), speciesCount[secondArea]);
					outFeature->SetField(outFeatureDefn->GetFieldIndex("ADOMSP2"), speciesArea[secondArea]);
				}
				areaRunTotal += speciesArea[secondArea];
				if(thirdArea != -1)
				{
					if(areaRunTotal < areaThreshold)
					{
						domSPLTStr += speciesNames[thirdArea];
					}
					
					domSPStr += speciesNames[thirdArea];
					outFeature->SetField(outFeatureDefn->GetFieldIndex("DOMSP3"), speciesNames[thirdArea].c_str());
					outFeature->SetField(outFeatureDefn->GetFieldIndex("CDOMSP3"), speciesCount[thirdArea]);
					outFeature->SetField(outFeatureDefn->GetFieldIndex("ADOMSP3"), speciesArea[thirdArea]);
				}
				
				outFeature->SetField(outFeatureDefn->GetFieldIndex("DOMSP"), domSPStr.c_str());
				outFeature->SetField(outFeatureDefn->GetFieldIndex("DOMSPLT"), domSPLTStr.c_str());
				outFeature->SetField(outFeatureDefn->GetFieldIndex("AREA"), totalArea);
			}
			else
			{
				throw RSGISVectorException("Method for dominance calculation has not been defined.");
			}
						
			
			
		} 
		else
		{
			std::string message = std::string("Unsupport data type: ") + std::string(inFeature->GetGeometryRef()->getGeometryName());
			throw RSGISVectorException(message);
		}
		
	}
	
	void RSGISLabelPolygonsFromClassification::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISLabelPolygonsFromClassification::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		///////////////////////////////////////////
		// Text codes of top three dominant species
		///////////////////////////////////////////
		OGRFieldDefn shpFieldDomSP( "DOMSP", OFTString );
		shpFieldDomSP.SetWidth(100);
		if( outputLayer->CreateField( &shpFieldDomSP ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"DOMSP\" has failed");
		}
		OGRFieldDefn shpFieldDomSPLT( "DOMSPLT", OFTString );
		shpFieldDomSPLT.SetWidth(100);
		if( outputLayer->CreateField( &shpFieldDomSPLT ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"DOMSPLT\" has failed");
		}
		
		OGRFieldDefn shpFieldDomSP1( "DOMSP1", OFTString );
		shpFieldDomSP1.SetWidth(100);
		if( outputLayer->CreateField( &shpFieldDomSP1 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"DOMSP1\" has failed");
		}
		
		OGRFieldDefn shpFieldDomSP2( "DOMSP2", OFTString );
		shpFieldDomSP2.SetWidth(100);
		if( outputLayer->CreateField( &shpFieldDomSP2 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"DOMSP2\" has failed");
		}
		
		OGRFieldDefn shpFieldDomSP3( "DOMSP3", OFTString );
		shpFieldDomSP3.SetWidth(100);
		if( outputLayer->CreateField( &shpFieldDomSP3 ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"DOMSP3\" has failed");
		}
		
		
		///////////////////////////////////////////
		// Count of top three dominant species
		///////////////////////////////////////////
		OGRFieldDefn shpFieldCDomSP1("CDOMSP1", OFTReal);
		shpFieldCDomSP1.SetPrecision(10);
		if( outputLayer->CreateField( &shpFieldCDomSP1 ) != OGRERR_NONE )
		{
			std::string message = std::string("Creating shapefile field \'CDOMSP1\' has failed");
			throw RSGISVectorOutputException(message.c_str());
		}
		
		OGRFieldDefn shpFieldCDomSP2("CDOMSP2", OFTReal);
		shpFieldCDomSP2.SetPrecision(10);
		if( outputLayer->CreateField( &shpFieldCDomSP2 ) != OGRERR_NONE )
		{
			std::string message = std::string("Creating shapefile field \'CDOMSP2\' has failed");
			throw RSGISVectorOutputException(message.c_str());
		}
		
		OGRFieldDefn shpFieldCDomSP3("CDOMSP3", OFTReal);
		shpFieldCDomSP3.SetPrecision(10);
		if( outputLayer->CreateField( &shpFieldCDomSP3 ) != OGRERR_NONE )
		{
			std::string message = std::string("Creating shapefile field \'CDOMSP3\' has failed");
			throw RSGISVectorOutputException(message.c_str());
		}
		
		///////////////////////////////////////////
		// Area of top three dominant species
		///////////////////////////////////////////
		
		OGRFieldDefn shpFieldADomSP1("ADOMSP1", OFTReal);
		shpFieldADomSP1.SetPrecision(10);
		if( outputLayer->CreateField( &shpFieldADomSP1 ) != OGRERR_NONE )
		{
			std::string message = std::string("Creating shapefile field \'ADOMSP1\' has failed");
			throw RSGISVectorOutputException(message.c_str());
		}
		
		OGRFieldDefn shpFieldADomSP2("ADOMSP2", OFTReal);
		shpFieldADomSP2.SetPrecision(10);
		if( outputLayer->CreateField( &shpFieldADomSP2 ) != OGRERR_NONE )
		{
			std::string message = std::string("Creating shapefile field \'ADOMSP2\' has failed");
			throw RSGISVectorOutputException(message.c_str());
		}
		
		OGRFieldDefn shpFieldADomSP3("ADOMSP3", OFTReal);
		shpFieldADomSP3.SetPrecision(10);
		if( outputLayer->CreateField( &shpFieldADomSP3 ) != OGRERR_NONE )
		{
			std::string message = std::string("Creating shapefile field \'ADOMSP3\' has failed");
			throw RSGISVectorOutputException(message.c_str());
		}
		
		
		OGRFieldDefn shpFieldAREA("AREA", OFTReal);
		shpFieldAREA.SetPrecision(10);
		if( outputLayer->CreateField( &shpFieldAREA ) != OGRERR_NONE )
		{
			std::string message = std::string("Creating shapefile field \'AREA\' has failed");
			throw RSGISVectorOutputException(message.c_str());
		}
		
	}
	
	RSGISLabelPolygonsFromClassification::~RSGISLabelPolygonsFromClassification()
	{
		
	}
}}



