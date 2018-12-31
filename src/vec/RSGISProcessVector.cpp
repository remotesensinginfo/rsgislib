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
		
		geos::geom::Envelope *env = NULL;
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
			this->processFeatures->createOutputLayerDefinition(outputLayer, inFeatureDefn);
			
			outFeatureDefn = outputLayer->GetLayerDefn();
			
			int numFeatures = inputLayer->GetFeatureCount(TRUE);
			
			bool nullGeometry = false;
			
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
			int i = 0;
			
			if(outVertical)
			{
				std::cout << "Started, " << numFeatures << " features to process.\n";
			}
			else
			{
				std::cout << "Started" << std::flush;
			}	

            bool inTransaction = false;
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if((numFeatures > 10) && ((i % feedback) == 0) && feedbackCounter <= 100)
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
				}
                if(!inTransaction)
                {
                    outputLayer->StartTransaction();
                    inTransaction = true;
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
                if(((i % 20000) == 0) & inTransaction)
                {
                    inputLayer->CommitTransaction();
                    inTransaction = false;
                }
            }
            if(inTransaction)
            {
                inputLayer->CommitTransaction();
                inTransaction = false;
            }
			std::cout << " Complete.\n";
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
		
	void RSGISProcessVector::processVectors(OGRLayer *inputLayer, bool outVertical, bool morefeedback) throw(RSGISVectorOutputException,RSGISVectorException)
	{
		RSGISVectorUtils vecUtils;
		
		geos::geom::Envelope *env = NULL;
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
            unsigned long nextFeedback = 0;
			int i = 0;

            if(morefeedback)
            {
                feedback = numFeatures/20;
            }
			
			if(outVertical)
			{
				std::cout << "Started, " << numFeatures << " features to process.\n";
			}
			else
			{
				std::cout << "Started (" << numFeatures << " features) " << std::flush;
			}	

            bool inTransaction = false;
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if((numFeatures > 10) && (i == nextFeedback) && feedbackCounter <= 100)
				{
					if(outVertical)
					{
						std::cout << feedbackCounter << "% Done" << std::endl;
					}
					else
					{
						std::cout << "." << feedbackCounter << "." << std::flush;
					}

                    if(morefeedback)
                    {
                        feedbackCounter = feedbackCounter + 5;
                    }
                    else
                    {
                        feedbackCounter = feedbackCounter + 10;
                    }
                    nextFeedback = nextFeedback + feedback;
				}
                if(!inTransaction)
                {
                    inputLayer->StartTransaction();
                    inTransaction = true;
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
						throw RSGISVectorOutputException("Failed to write feature to the vector layer.");
					}
				}
				
				OGRFeature::DestroyFeature(inFeature);
				i++;
                if(((i % 20000) == 0) & inTransaction)
                {
                    std::cout << "w" << std::flush;
                    inputLayer->CommitTransaction();
                    inTransaction = false;
                }
			}
            if(inTransaction)
            {
                std::cout << "w" << std::flush;
                inputLayer->CommitTransaction();
                inTransaction = false;
            }
			std::cout << " Complete.\n";
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
		
		geos::geom::Envelope *env = NULL;
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
				std::cout << "Started, " << numFeatures << " features to process.\n";
			}
			else
			{
				std::cout << "Started" << std::flush;
			}
			
			
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if((numFeatures > 10) && ((i % feedback) == 0) && feedbackCounter <= 100)
				{
					if(outVertical)
					{
						std::cout << feedbackCounter << "% Done\n";
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
                else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiLineString )
				{
					OGRMultiLineString *line = (OGRMultiLineString *) geometry;
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
			std::cout << " Complete.\n";			
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
				std::string message = std::string("Creating ") + std::string(inFeatureDefn->GetFieldDefn(i)->GetNameRef()) + std::string(" field has failed.");
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
	
    void RSGISProcessVector::printGeometry(OGRGeometry *geometry)
    {
        if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
		{
            std::cout << "Polygon:\n";
			OGRPolygon *poly = (OGRPolygon *) geometry;
            
            std::cout << "\tExterior Ring:\n\t";
            this->printRing(poly->getExteriorRing());
            unsigned int numInternalRings = poly->getNumInteriorRings();
            for(unsigned int i = 0; i < numInternalRings; ++i)
            {
                std::cout << "\tInternal Ring(" << i << "):\n\t";
                this->printRing(poly->getInteriorRing(i));
            }
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiPolygon )
		{
            std::cout << "Multi-Polygon:\n";
			OGRMultiPolygon *mPoly = (OGRMultiPolygon *) geometry;
            OGRPolygon *poly = NULL;
            
            unsigned int numPolys = mPoly->getNumGeometries();
            for(unsigned int n = 0; n < numPolys; ++n)
            {
                std::cout << "\tPolygon (" << n << "):\n";
                poly = (OGRPolygon *) mPoly->getGeometryRef(n);
                std::cout << "\t\tExterior Ring:\n\t\t";
                this->printRing(poly->getExteriorRing());
                unsigned int numInternalRings = poly->getNumInteriorRings();
                for(unsigned int i = 0; i < numInternalRings; ++i)
                {
                    std::cout << "\t\tInternal Ring(" << i << "):\n\t\t";
                    this->printRing(poly->getInteriorRing(i));
                }
            }
            std::cout << std::endl;
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPoint )
		{
            std::cout << "Point: ";
			OGRPoint *point = (OGRPoint *) geometry;
			
            std::cout << "[" << point->getX() << ", " << point->getY() << ", " << point->getZ() << "]\n";
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbLineString )
		{
			throw RSGISVectorException("Polylines not implemented yet.");
		}
		else if(geometry != NULL)
		{
			std::string message = std::string("Unsupported data type: ") + std::string(geometry->getGeometryName());
			throw RSGISVectorException(message);
		}
		else
		{
			throw RSGISVectorException("WARNING: NULL Geometry Present within input file");
		}
    }
    
    void RSGISProcessVector::printRing(OGRLinearRing *inGeomRing)
    {
        unsigned int numPts = inGeomRing->getNumPoints();
        std::cout << "Line (" << numPts << "):\t";
        OGRPoint *point = new OGRPoint();
        for(unsigned int i = 0; i < numPts; ++i)
        {
            inGeomRing->getPoint(i, point);
            if(i == 0)
            {
                std::cout << "[" << point->getX() << ", " << point->getY() << ", " << point->getZ() << "]";
            }
            else
            {
                std::cout << ", [" << point->getX() << ", " << point->getY() << ", " << point->getZ() << "]";
            }
        }
        std::cout << std::endl;
    }
    
	RSGISProcessVector::~RSGISProcessVector()
	{
		
	}

}}

