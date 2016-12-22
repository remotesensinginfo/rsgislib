/*
 *  RSGISProcessVectorSQL.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/02/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 * 
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISProcessVectorSQL.h"


namespace rsgis{namespace vec{
	
	RSGISProcessVectorSQL::RSGISProcessVectorSQL(RSGISProcessOGRFeature *processFeatures)
	{
		this->processFeatures = processFeatures;
	}
	
	void RSGISProcessVectorSQL::processVectors(GDALDataset *inputDS, OGRLayer *outputLayer, bool copyData, bool outVertical, std::string sql) throw(RSGISVectorOutputException,RSGISVectorException)
	{
		RSGISVectorUtils vecUtils;
		
		geos::geom::Envelope *env = NULL;
		OGRGeometry *geometry = NULL;
		
		OGRFeature *inFeature = NULL;
		OGRFeature *outFeature = NULL;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		OGRFeatureDefn *outFeatureDefn = NULL;
		
		OGRLayer *inputLayer = NULL;
		
		long fid = 0;
		
		try
		{	
			/* Run SQL statement */
			inputLayer = inputDS->ExecuteSQL(sql.c_str(), NULL, "generic");
			
			/* Create output layer definition */
			inFeatureDefn = inputLayer->GetLayerDefn();
			
			if(copyData)
			{
				this->copyFeatureDefn(outputLayer, inFeatureDefn);
			}
			this->processFeatures->createOutputLayerDefinition(outputLayer, inFeatureDefn);
			
			outFeatureDefn = outputLayer->GetLayerDefn();
			
			int numFeatures = inputLayer->GetFeatureCount(TRUE);
			
            std::cout << numFeatures << " features where selected.\n";
            
			bool nullGeometry = false;
			
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
			int i = 0;
			bool toFeedback = true;
			if(numFeatures < 10)
			{
				toFeedback = false;
			}
			
			if(toFeedback)
			{
				if(outVertical)
				{
					std::cout << "Started, " << numFeatures << " features to process.\n";
				}
				else
				{
					std::cout << "Started" << std::flush;
				}				
			}			
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if(toFeedback && (i % feedback) == 0)
				{
					if(outVertical)
					{
						std::cout << feedbackCounter << "% Done" << std::endl;
					}
					else
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
					}
					
					feedbackCounter = feedbackCounter + 10;
				}
				
				fid = inFeature->GetFID();
				
				outFeature = OGRFeature::CreateFeature(outFeatureDefn);
				
				// Get Geometry.
				nullGeometry = false;
				geometry = inFeature->GetGeometryRef();
				if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
				{
					OGRPolygon *polygon = (OGRPolygon *) geometry;
					outFeature->SetGeometry(polygon);
					env = vecUtils.getEnvelope(polygon);
				} 
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiPolygon )
				{
					OGRMultiPolygon *multiPolygon = (OGRMultiPolygon *) geometry;
					outFeature->SetGeometry(multiPolygon);
					env = vecUtils.getEnvelope(multiPolygon);
				}
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPoint )
				{
					OGRPoint *point = (OGRPoint *) geometry;
					outFeature->SetGeometry(point);
					env = vecUtils.getEnvelope(point);
				}	
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbLineString )
				{
					OGRLineString *line = (OGRLineString *) geometry;
					outFeature->SetGeometry(line);
					env = vecUtils.getEnvelope(line);
				}
				else if(geometry != NULL)
				{
					std::string message = std::string("Unsupport data type: ") + std::string(geometry->getGeometryName());
					throw RSGISVectorException(message);
				}
				else 
				{
					nullGeometry = true;
					std::cout << "WARNING: NULL Geometry Present within input file - IGNORED\n";
				}
				
				if(!nullGeometry)
				{
					processFeatures->processFeature(inFeature, outFeature, env, fid);
					
					delete env;
					
					outFeature->SetFID(fid);
					
					if(copyData)
					{
						this->copyFeatureData(inFeature, outFeature, inFeatureDefn, outFeatureDefn);
					}
					
					if( outputLayer->CreateFeature(outFeature) != OGRERR_NONE )
					{
						throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
					}
					
					OGRFeature::DestroyFeature(outFeature);
				}
				
				OGRFeature::DestroyFeature(inFeature);
				i++;
			}
			if(toFeedback)
			{
				std::cout << " Complete.\n";
			}
		}
		catch(RSGISVectorOutputException& e)
		{
			throw e;
		}
		catch(RSGISVectorException& e)
		{
			throw e;
		}
	}
	
	void RSGISProcessVectorSQL::processVectors(GDALDataset *inputDS, bool outVertical, std::string sql) throw(RSGISVectorOutputException,RSGISVectorException)
	{
		RSGISVectorUtils vecUtils;
		
		geos::geom::Envelope *env = NULL;
		OGRGeometry *geometry = NULL;
		
		OGRFeature *inFeature = NULL;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		
		long fid = 0;
		
		OGRLayer *inputLayer = NULL;
		
		try
		{
			/* Run SQL statement */
			inputLayer = inputDS->ExecuteSQL(sql.c_str(), NULL, "generic");
			
			/* Create output layer definition */
			inFeatureDefn = inputLayer->GetLayerDefn();
			
			int numFeatures = inputLayer->GetFeatureCount(TRUE);
			
			bool nullGeometry = false;
			
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
			int i = 0;
			bool toFeedback = true;
			if(numFeatures < 10)
			{
				toFeedback = false;
			}
			
			if(toFeedback)
			{
				if(outVertical)
				{
					std::cout << "Started, " << numFeatures << " features to process.\n";
				}
				else
				{
					std::cout << "Started" << std::flush;
				}				
			}
			
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if(toFeedback && (i % feedback) == 0)
				{
					if(outVertical)
					{
						std::cout << feedbackCounter << "% Done" << std::endl;
					}
					else
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
					}
					
					feedbackCounter = feedbackCounter + 10;
				}
				
				fid = inFeature->GetFID();
				
				// Get Geometry.
				nullGeometry = false;
				geometry = inFeature->GetGeometryRef();
				if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
				{
					OGRPolygon *polygon = (OGRPolygon *) geometry;
					env = vecUtils.getEnvelope(polygon);
				} 
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiPolygon )
				{
					OGRMultiPolygon *multiPolygon = (OGRMultiPolygon *) geometry;
					env = vecUtils.getEnvelope(multiPolygon);
				}
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPoint )
				{
					OGRPoint *point = (OGRPoint *) geometry;
					env = vecUtils.getEnvelope(point);
				}	
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbLineString )
				{
					OGRLineString *line = (OGRLineString *) geometry;
					env = vecUtils.getEnvelope(line);
				}
				else if(geometry != NULL)
				{
					std::string message = std::string("Unsupport data type: ") + std::string(geometry->getGeometryName());
					throw RSGISVectorException(message);
				}
				else 
				{
					nullGeometry = true;
					std::cout << "WARNING: NULL Geometry Present within input file - IGNORED\n";
				}
				
				if(!nullGeometry)
				{					
					processFeatures->processFeature(inFeature, env, fid);
					
					delete env;
					
					if( inputLayer->SetFeature(inFeature) != OGRERR_NONE )
					{
						throw RSGISVectorOutputException("Failed to write feature to the shapefile.");
					}
				}
				
				OGRFeature::DestroyFeature(inFeature);
				i++;
			}
			if(toFeedback)
			{
				std::cout << " Complete.\n";
			}
		}
		catch(RSGISVectorOutputException& e)
		{
			throw e;
		}
		catch(RSGISVectorException& e)
		{
			throw e;
		}
	}
	
	void RSGISProcessVectorSQL::processVectorsNoOutput(GDALDataset *inputDS, bool outVertical, std::string sql) throw(RSGISVectorOutputException,RSGISVectorException)
	{
		RSGISVectorUtils vecUtils;
		
		geos::geom::Envelope *env = NULL;
		OGRGeometry *geometry = NULL;
		
		OGRFeature *inFeature = NULL;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		
		OGRLayer *inputLayer = NULL;
		
		long fid = 0;
		
		try
		{
			/* Run SQL statement */
			inputLayer = inputDS->ExecuteSQL(sql.c_str(), NULL, "generic");
			
			/* Create output layer definition */
			inFeatureDefn = inputLayer->GetLayerDefn();
			
			bool nullGeometry = false;
			
			int numFeatures = inputLayer->GetFeatureCount(TRUE);
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
			bool toFeedback = true;
			if(numFeatures < 10)
			{
				toFeedback = false;
			}
			
			int i = 0;
			if(toFeedback)
			{
				if(outVertical)
				{
					std::cout << "Started, " << numFeatures << " features to process.\n";
				}
				else
				{
					std::cout << "Started" << std::flush;
				}
			}
			
			
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if(toFeedback && (i % feedback) == 0)
				{
					if(outVertical)
					{
						std::cout << feedbackCounter << "% Done" << std::endl;
					}
					else
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
					}
					
					feedbackCounter = feedbackCounter + 10;
				}
				
				fid = inFeature->GetFID();
				
				// Get Geometry.
				nullGeometry = false;
				geometry = inFeature->GetGeometryRef();
				if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
				{
					OGRPolygon *polygon = (OGRPolygon *) geometry;
					env = vecUtils.getEnvelope(polygon);
				} 
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiPolygon )
				{
					OGRMultiPolygon *multiPolygon = (OGRMultiPolygon *) geometry;
					env = vecUtils.getEnvelope(multiPolygon);
				}
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPoint )
				{
					OGRPoint *point = (OGRPoint *) geometry;
					env = vecUtils.getEnvelope(point);
				}
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbLineString )
				{
					OGRLineString *line = (OGRLineString *) geometry;
					env = vecUtils.getEnvelope(line);
				}
				else if(geometry != NULL)
				{
					std::string message = std::string("Unsupport data type: ") + std::string(geometry->getGeometryName());
					throw RSGISVectorException(message);
				}
				else 
				{
					nullGeometry = true;
					std::cout << "WARNING: NULL Geometry Present within input file - IGNORED\n";
				}
				
				if(!nullGeometry)
				{			
					processFeatures->processFeature(inFeature, env, fid);
					
					delete env;
				}
				
				OGRFeature::DestroyFeature(inFeature);
				i++;
			}
			if(toFeedback)
			{
				std::cout << " Complete.\n";
			}			
		}
		catch(RSGISVectorOutputException& e)
		{
			throw e;
		}
		catch(RSGISVectorException& e)
		{
			throw e;
		}
	}
	
	void RSGISProcessVectorSQL::copyFeatureDefn(OGRLayer *outputSHPLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		int fieldCount = inFeatureDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			if( outputSHPLayer->CreateField( inFeatureDefn->GetFieldDefn(i) ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating ") + std::string(inFeatureDefn->GetFieldDefn(i)->GetNameRef()) + std::string(" field has failed.");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
	}
	
	void RSGISProcessVectorSQL::copyFeatureData(OGRFeature *inFeature, OGRFeature *outFeature, OGRFeatureDefn *inFeatureDefn, OGRFeatureDefn *outFeatureDefn)
	{
		int fieldCount = inFeatureDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			outFeature->SetField(outFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef()), inFeature->GetRawFieldRef(inFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef())));
		}
		
	}
	
	RSGISProcessVectorSQL::~RSGISProcessVectorSQL()
	{
		
	}
	
}}



