/*
 *  RSGISProcessPolygonVector.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/03/2009.
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

#include "RSGISProcessVector.h"

namespace rsgis{namespace vec{
	
	RSGISProcessVector::RSGISProcessVector(RSGISProcessOGRFeature *processFeatures)
	{
		this->processFeatures = processFeatures;
	}
	
	void RSGISProcessVector::processVectors(OGRLayer *inputLayer, OGRLayer *outputLayer, bool copyData, bool outVertical, bool newFirst) throw(RSGISVectorOutputException,RSGISVectorException)
	{
		RSGISVectorUtils vecUtils;
		
		Envelope *env = NULL;
		OGRGeometry *geometry = NULL;
		
		OGRFeature *inFeature = NULL;
		OGRFeature *outFeature = NULL;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		OGRFeatureDefn *outFeatureDefn = NULL;
		
		long fid = 0;
		
		try
		{
			inFeatureDefn = inputLayer->GetLayerDefn();
			
		/*	if(newFirst)
			{
				this->processFeatures->createOutputLayerDefinition(outputLayer, inFeatureDefn);
				if(copyData)
				{
					this->copyFeatureDefn(outputLayer, inFeatureDefn);
				}
			}
			else
			{
				if(copyData)
				{
					this->copyFeatureDefn(outputLayer, inFeatureDefn);
				}
				this->processFeatures->createOutputLayerDefinition(outputLayer, inFeatureDefn);
			}*/
			
			if(copyData)
			{
				this->copyFeatureDefn(outputLayer, inFeatureDefn);
			}
			this->processFeatures->createOutputLayerDefinition(outputLayer, inFeatureDefn);
			
			outFeatureDefn = outputLayer->GetLayerDefn();
			
			int numFeatures = inputLayer->GetFeatureCount(TRUE);
			
			bool nullGeometry = false;
			
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
			int i = 0;
			
			if(outVertical)
			{
				cout << "Started, " << numFeatures << " features to process.\n";
			}
			else
			{
				cout << "Started" << flush;
			}	
			
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if((numFeatures > 10) && (i % feedback) == 0)
				{
					if(outVertical)
					{
						cout << feedbackCounter << "% Done" << endl;
					}
					else
					{
						cout << ".." << feedbackCounter << ".." << flush;
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
					string message = string("Unsupport data type: ") + string(geometry->getGeometryName());
					throw RSGISVectorException(message);
				}
				else 
				{
					nullGeometry = true;
					cout << "WARNING: NULL Geometry Present within input file - IGNORED\n";
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
			cout << " Complete.\n";
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
		
	void RSGISProcessVector::processVectors(OGRLayer *inputLayer, bool outVertical) throw(RSGISVectorOutputException,RSGISVectorException)
	{
		RSGISVectorUtils vecUtils;
		
		Envelope *env = NULL;
		OGRGeometry *geometry = NULL;
		
		OGRFeature *inFeature = NULL;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		
		long fid = 0;
		
		try
		{
			inFeatureDefn = inputLayer->GetLayerDefn();
			
			int numFeatures = inputLayer->GetFeatureCount(TRUE);
			
			bool nullGeometry = false;
			
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
			int i = 0;
			
			if(outVertical)
			{
				cout << "Started, " << numFeatures << " features to process.\n";
			}
			else
			{
				cout << "Started" << flush;
			}	
						
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if((numFeatures > 10) && (i % feedback) == 0)
				{
					if(outVertical)
					{
						cout << feedbackCounter << "% Done" << endl;
					}
					else
					{
						cout << ".." << feedbackCounter << ".." << flush;
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
					string message = string("Unsupport data type: ") + string(geometry->getGeometryName());
					throw RSGISVectorException(message);
				}
				else 
				{
					nullGeometry = true;
					cout << "WARNING: NULL Geometry Present within input file - IGNORED\n";
				}
				
				if(!nullGeometry)
				{
					//cout << " FID = " << fid << " " << env->toString() << endl ;
					
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
			cout << " Complete.\n";
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
	
	void RSGISProcessVector::processVectorsNoOutput(OGRLayer *inputLayer, bool outVertical) throw(RSGISVectorOutputException,RSGISVectorException)
	{
		RSGISVectorUtils vecUtils;
		
		Envelope *env = NULL;
		OGRGeometry *geometry = NULL;
		
		OGRFeature *inFeature = NULL;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		
		long fid = 0;
		
		try
		{
			inFeatureDefn = inputLayer->GetLayerDefn();
			
			bool nullGeometry = false;
			
			int numFeatures = inputLayer->GetFeatureCount(TRUE);
			int feedback = numFeatures/10;
			int feedbackCounter = 0;

			int i = 0;
			if(outVertical)
			{
				cout << "Started, " << numFeatures << " features to process.\n";
			}
			else
			{
				cout << "Started" << flush;
			}
			
			
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if((numFeatures > 10) && (i % feedback) == 0)
				{
					if(outVertical)
					{
						cout << feedbackCounter << "% Done\n";
					}
					else
					{
						cout << ".." << feedbackCounter << ".." << flush;
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
					string message = string("Unsupport data type: ") + string(geometry->getGeometryName());
					throw RSGISVectorException(message);
				}
				else 
				{
					nullGeometry = true;
					cout << "WARNING: NULL Geometry Present within input file - IGNORED\n";
				}
				
				if(!nullGeometry)
				{			
					processFeatures->processFeature(inFeature, env, fid);
				
					delete env;
				}

				OGRFeature::DestroyFeature(inFeature);
				i++;
			}
			cout << " Complete.\n";			
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
	
	void RSGISProcessVector::copyFeatureDefn(OGRLayer *outputSHPLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		int fieldCount = inFeatureDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			if( outputSHPLayer->CreateField( inFeatureDefn->GetFieldDefn(i) ) != OGRERR_NONE )
			{
				string message = string("Creating ") + string(inFeatureDefn->GetFieldDefn(i)->GetNameRef()) + string(" field has failed.");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
	}
	
	void RSGISProcessVector::copyFeatureData(OGRFeature *inFeature, OGRFeature *outFeature, OGRFeatureDefn *inFeatureDefn, OGRFeatureDefn *outFeatureDefn)
	{
		int fieldCount = inFeatureDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			outFeature->SetField(outFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef()), inFeature->GetRawFieldRef(inFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef())));
		}
		
	}
	
	RSGISProcessVector::~RSGISProcessVector()
	{
		
	}

}}

