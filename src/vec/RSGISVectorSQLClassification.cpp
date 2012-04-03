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
	
	void RSGISVectorSQLClassification::classifyVector(OGRDataSource *source, OGRLayer *inputLayer, sqlclass **rules, int numRules, string classAttribute) throw(RSGISVectorException)
	{
		// Check layer - output feature is present.
		OGRFeatureDefn *layerDef = inputLayer->GetLayerDefn();
		int fieldIdx = 0;
		
		fieldIdx = layerDef->GetFieldIndex(classAttribute.c_str());
		if(fieldIdx < 0)
		{
			string message = "This layer does not contain a field with the name \'" + classAttribute + "\'";
			throw RSGISVectorException(message.c_str());
		}
		
		OGRFieldDefn *fieldDef = layerDef->GetFieldDefn(fieldIdx);
		if(fieldDef->GetType() != OFTString)
		{
			string message =  classAttribute + " attribute needs to be of type string.";
			throw RSGISVectorException(message.c_str());
		}
		
		cout << "Running Query - Results saved to memory\n";
		// Execute Rules.
		int fid = 0;
		OGRFeature *queryFeature = NULL;
		int numFeatures = inputLayer->GetFeatureCount(TRUE);
		int queryFeatureCount = 0;
		string *classification = new string[numFeatures];
		OGRLayer *resultsSet = NULL;
		for(int i = 0; i < numRules; i++)
		{
			cout << i << ") Executing: " << rules[i]->name << " - " << rules[i]->sql << flush; 
			resultsSet = source->ExecuteSQL(rules[i]->sql.c_str(), NULL, "generic");
			cout << " - Finished Query - " << flush;
			
			if(resultsSet != NULL)
			{
				queryFeatureCount = resultsSet->GetFeatureCount(true);
				if(queryFeatureCount > 0)
				{
					cout << "Updating " << queryFeatureCount << " classnames ... " << flush;
					resultsSet->ResetReading();

					while( (queryFeature = resultsSet->GetNextFeature()) != NULL )
					{
						fid = queryFeature->GetFID();
						//cout << "\nUpdating " << fid;
						classification[fid] = rules[i]->name;
						OGRFeature::DestroyFeature(queryFeature);
					}
				}
				else
				{
					cout << " ** No Features identified ** " << flush;
				}
				source->ReleaseResultSet(resultsSet);
			}
			else
			{
				cout << " ** No Features identified ** " << flush;
			}
			cout << "Done\n";
		}
		
		cout << "Update layer on disk\n";
		// Update OGR Layer
		int feedback = numFeatures/10;
		int feedbackCounter = 0;
		int i = 0;
		cout << "Started" << flush;

		OGRFeature *feature = NULL;
		inputLayer->ResetReading();
		while( (feature = inputLayer->GetNextFeature()) != NULL )
		{
			if((i % feedback) == 0)
			{
				cout << ".." << feedbackCounter << ".." << flush;
				feedbackCounter = feedbackCounter + 10;
			}
			
			fid = feature->GetFID();
			
			//cout << "Updating " << fid << " with classname " << classification[fid] << endl;
			
			feature->SetField(fieldIdx, classification[fid].c_str());
			if( inputLayer->SetFeature(feature) != OGRERR_NONE )
			{
				throw RSGISVectorException("Failed to write updated feature to layer.");
			}
			
			OGRFeature::DestroyFeature(feature);
			i++;
		}
		cout << " Complete.\n";
		
		delete[] classification;
	}
	
	RSGISVectorSQLClassification::~RSGISVectorSQLClassification()
	{
		
	}
}}



