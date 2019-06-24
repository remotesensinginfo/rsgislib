/*
 *  RSGISProcessGeometry.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 14/04/2009.
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

#include "RSGISProcessGeometry.h"


namespace rsgis{namespace vec{
	
	RSGISProcessGeometry::RSGISProcessGeometry(RSGISProcessOGRGeometry *processGeom)
	{
		this->processGeom = processGeom;
	}
	
	void RSGISProcessGeometry::processGeometry(OGRLayer *inputLayer, OGRLayer *outputLayer, bool copyData, bool outVertical)
	{
		OGRGeometry *geometry = NULL;
		
		OGRFeature *inFeature = NULL;
		OGRFeature *outFeature = NULL;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		OGRFeatureDefn *outFeatureDefn = NULL;
		
		long fid = 0;
		
		try
		{
			inFeatureDefn = inputLayer->GetLayerDefn();
			
			if(copyData)
			{
				this->copyFeatureDefn(outputLayer, inFeatureDefn);
			}
			
			outFeatureDefn = outputLayer->GetLayerDefn();
			
			int numFeatures = inputLayer->GetFeatureCount(TRUE);
			
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
            unsigned long nextFeedback = 0;
			int i = 0;
			if(outVertical)
			{
				std::cout << "Started, " << numFeatures << " features to process.\n";
			}
			else
			{
				std::cout << "Started " << std::flush;
			}

            unsigned int transactionStep = 20000;
            unsigned int nextTransaction = transactionStep;
            bool openTransaction = false;

			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if((numFeatures > 10) && (i == nextFeedback))
				{
					if(outVertical)
					{
						std::cout << feedbackCounter << "% Done" << std::endl;
					}
					else
					{
						std::cout << "." << feedbackCounter << "." << std::flush;
					}
					
					feedbackCounter = feedbackCounter + 10;
                    nextFeedback = nextFeedback + feedback;
				}

                if(!openTransaction)
                {
                    outputLayer->StartTransaction();
                    openTransaction = true;
                }

				fid = inFeature->GetFID();
				
				outFeature = OGRFeature::CreateFeature(outFeatureDefn);
				
				// Get Geometry.
				geometry = inFeature->GetGeometryRef();
				if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
				{
					OGRPolygon *polygon = (OGRPolygon *) geometry;
					processGeom->processGeometry(polygon);
					outFeature->SetGeometry(polygon);
				} 
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiPolygon )
				{
					OGRMultiPolygon *multiPolygon = (OGRMultiPolygon *) geometry;
					processGeom->processGeometry(multiPolygon);
					outFeature->SetGeometry(multiPolygon);
				}
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPoint )
				{
					OGRPoint *point = (OGRPoint *) geometry;
					processGeom->processGeometry(point);
					outFeature->SetGeometry(point);
				}	
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbLineString )
				{
					OGRLineString *line = (OGRLineString *) geometry;
					processGeom->processGeometry(line);
					outFeature->SetGeometry(line);
				}
				else
				{
					throw RSGISVectorException("Unsupport data type.");
				}
												
				outFeature->SetFID(fid);
				
				if(copyData)
				{
					this->copyFeatureData(inFeature, outFeature, inFeatureDefn, outFeatureDefn);
				}
				
				if( outputLayer->CreateFeature(outFeature) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}

                if(openTransaction && (i == nextTransaction))
                {
                    outputLayer->CommitTransaction();
                    openTransaction = false;
                    nextTransaction = nextTransaction + transactionStep;
                }
				
				OGRFeature::DestroyFeature(inFeature);
				OGRFeature::DestroyFeature(outFeature);
				i++;
			}

            if(openTransaction)
            {
                outputLayer->CommitTransaction();
                openTransaction = false;
            }
			std::cout << " Complete.\n";
			
		}
		catch(RSGISVectorOutputException& e)
		{
			throw e;
		}
		catch(RSGISVectorException& e)
		{
			throw RSGISVectorException(std::string(e.what()).c_str());
		}
	}
	
	void RSGISProcessGeometry::processGeometryPolygonOutput(OGRLayer *inputLayer, OGRLayer *outputLayer, bool copyData, bool outVertical)
	{
		OGRGeometry *geometry = NULL;
		
		OGRFeature *inFeature = NULL;
		OGRFeature *outFeature = NULL;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		OGRFeatureDefn *outFeatureDefn = NULL;
		
		long fid = 0;
		
		try
		{
			inFeatureDefn = inputLayer->GetLayerDefn();
			
			if(copyData)
			{
				this->copyFeatureDefn(outputLayer, inFeatureDefn);
			}
			
			outFeatureDefn = outputLayer->GetLayerDefn();
			
			int numFeatures = inputLayer->GetFeatureCount(TRUE);
			
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
            unsigned long nextFeedback = 0;
			int i = 0;
			if(outVertical)
			{
				std::cout << "Started, " << numFeatures << " features to process.\n";
			}
			else
			{
				std::cout << "Started " << std::flush;
			}

            unsigned int transactionStep = 20000;
            unsigned int nextTransaction = transactionStep;
            bool openTransaction = false;
			
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if((numFeatures > 10) && (i == nextFeedback))
				{
					if(outVertical)
					{
						std::cout << feedbackCounter << "% Done" << std::endl;
					}
					else
					{
						std::cout << "." << feedbackCounter << "." << std::flush;
					}
				 
					feedbackCounter = feedbackCounter + 10;
                    nextFeedback = nextFeedback + feedback;
				}

                if(!openTransaction)
                {
                    outputLayer->StartTransaction();
                    openTransaction = true;
                }
				
				fid = inFeature->GetFID();
				
				outFeature = OGRFeature::CreateFeature(outFeatureDefn);
				
				// Get Geometry.
				geometry = inFeature->GetGeometryRef();
				if( geometry != NULL)
				{
					OGRPolygon *polygon = processGeom->processGeometry(geometry);					
					outFeature->SetGeometry(polygon);
				} 
				else
				{
					throw RSGISVectorException("Unsupport data type.");
				}
				
				outFeature->SetFID(fid);
				
				if(copyData)
				{
					this->copyFeatureData(inFeature, outFeature, inFeatureDefn, outFeatureDefn);
				}
				
				if( outputLayer->CreateFeature(outFeature) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}

                if(openTransaction && (i == nextTransaction))
                {
                    outputLayer->CommitTransaction();
                    openTransaction = false;
                    nextTransaction = nextTransaction + transactionStep;
                }
				
				OGRFeature::DestroyFeature(inFeature);
				OGRFeature::DestroyFeature(outFeature);
				i++;
			}
            if(openTransaction)
            {
                outputLayer->CommitTransaction();
                openTransaction = false;
            }
			std::cout << " Complete.\n";
			
		}
		catch(RSGISVectorOutputException& e)
		{
			throw e;
		}
		catch(RSGISVectorException& e)
		{
			throw RSGISVectorException(std::string(e.what()).c_str());
		}
	}
	
	void RSGISProcessGeometry::copyFeatureDefn(OGRLayer *outputSHPLayer, OGRFeatureDefn *inFeatureDefn)
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
	
	void RSGISProcessGeometry::copyFeatureData(OGRFeature *inFeature, OGRFeature *outFeature, OGRFeatureDefn *inFeatureDefn, OGRFeatureDefn *outFeatureDefn)
	{
		int fieldCount = inFeatureDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			outFeature->SetField(outFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef()), inFeature->GetRawFieldRef(inFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef())));
		}
		
	}
	
	RSGISProcessGeometry::~RSGISProcessGeometry()
	{
		
	}
	
}}




