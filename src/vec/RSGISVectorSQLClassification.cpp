/*
 *  RSGISVectorSQLClassification.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 26/02/2009.
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

#include "RSGISVectorSQLClassification.h"


namespace rsgis{namespace vec{
	
	RSGISVectorSQLClassification::RSGISVectorSQLClassification()
	{
		
	}
	
	void RSGISVectorSQLClassification::classifyVector(GDALDataset *source, OGRLayer *inputLayer, sqlclass **rules, int numRules, std::string classAttribute)
	{
		// Check layer - output feature is present.
		OGRFeatureDefn *layerDef = inputLayer->GetLayerDefn();
		int fieldIdx = 0;
		
		fieldIdx = layerDef->GetFieldIndex(classAttribute.c_str());
		if(fieldIdx < 0)
		{
			std::string message = "This layer does not contain a field with the name \'" + classAttribute + "\'";
			throw RSGISVectorException(message.c_str());
		}
		
		OGRFieldDefn *fieldDef = layerDef->GetFieldDefn(fieldIdx);
		if(fieldDef->GetType() != OFTString)
		{
			std::string message =  classAttribute + " attribute needs to be of type string.";
			throw RSGISVectorException(message.c_str());
		}
		
		std::cout << "Running Query - Results saved to memory\n";
		// Execute Rules.
		int fid = 0;
		OGRFeature *queryFeature = NULL;
		int numFeatures = inputLayer->GetFeatureCount(TRUE);
		int queryFeatureCount = 0;
		std::string *classification = new std::string[numFeatures];
		OGRLayer *resultsSet = NULL;
		for(int i = 0; i < numRules; i++)
		{
			std::cout << i << ") Executing: " << rules[i]->name << " - " << rules[i]->sql << std::flush; 
			resultsSet = source->ExecuteSQL(rules[i]->sql.c_str(), NULL, "generic");
			std::cout << " - Finished Query - " << std::flush;
			
			if(resultsSet != NULL)
			{
				queryFeatureCount = resultsSet->GetFeatureCount(true);
				if(queryFeatureCount > 0)
				{
					std::cout << "Updating " << queryFeatureCount << " classnames ... " << std::flush;
					resultsSet->ResetReading();

					while( (queryFeature = resultsSet->GetNextFeature()) != NULL )
					{
						fid = queryFeature->GetFID();
						classification[fid] = rules[i]->name;
						OGRFeature::DestroyFeature(queryFeature);
					}
				}
				else
				{
					std::cout << " ** No Features identified ** " << std::flush;
				}
				source->ReleaseResultSet(resultsSet);
			}
			else
			{
				std::cout << " ** No Features identified ** " << std::flush;
			}
			std::cout << "Done\n";
		}
		
		std::cout << "Update layer on disk\n";
		// Update OGR Layer
		int feedback = numFeatures/10;
		int feedbackCounter = 0;
		int i = 0;
		std::cout << "Started" << std::flush;

		OGRFeature *feature = NULL;
		inputLayer->ResetReading();
		while( (feature = inputLayer->GetNextFeature()) != NULL )
		{
			if((i % feedback) == 0)
			{
				std::cout << ".." << feedbackCounter << ".." << std::flush;
				feedbackCounter = feedbackCounter + 10;
			}
			
			fid = feature->GetFID();
						
			feature->SetField(fieldIdx, classification[fid].c_str());
			if( inputLayer->SetFeature(feature) != OGRERR_NONE )
			{
				throw RSGISVectorException("Failed to write updated feature to layer.");
			}
			
			OGRFeature::DestroyFeature(feature);
			i++;
		}
		std::cout << " Complete.\n";
		
		delete[] classification;
	}
	
	RSGISVectorSQLClassification::~RSGISVectorSQLClassification()
	{
		
	}
}}



