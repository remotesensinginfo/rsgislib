/*
 *  RSGISVectorMaths.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 26/12/2010.
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

#include "RSGISVectorMaths.h"

namespace rsgis{namespace vec{
	
	RSGISVectorMaths::RSGISVectorMaths(VariableFields **variables, int numVariables, string mathsExpression, string outHeading)
	{
		this->variables = variables;
		this->numVariables = numVariables;
		this->outHeading = outHeading;
		
		muParser = new Parser();
		this->inVals = new value_type[numVariables];
		for(int i = 0; i < numVariables; ++i)
		{
			muParser->DefineVar(_T(variables[i]->name.c_str()), &inVals[i]);
		}
		muParser->SetExpr(mathsExpression.c_str());
	}
	
	void RSGISVectorMaths::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		try 
		{
			OGRFeatureDefn *inFeatureDefn = inFeature->GetDefnRef();
			// Get variables
			for(int i = 0; i < numVariables; ++i)
			{
				int fieldIdx = inFeatureDefn->GetFieldIndex(this->variables[i]->fieldName.c_str());
				inVals[i] = inFeature->GetFieldAsDouble(fieldIdx);
			}
			value_type result = 0;
			result = muParser->Eval();
			
			// Save out Variables
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			outFeature->SetField(outFeatureDefn->GetFieldIndex(this->outHeading.c_str()), result);
			
			
		}
		catch (ParserError &e) 
		{
			string message = string("ERROR: ") + string(e.GetMsg()) + string(":\t \'") + string(e.GetExpr()) +string("\'");
			throw RSGISVectorException(message);
		}
		
	}	
	
	void RSGISVectorMaths::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		if(this->outHeading.length() > 10)
		{
			cout << this->outHeading << " will be truncated to \'" << this->outHeading.substr(0, 10) << "\'\n";
			this->outHeading = this->outHeading.substr(0, 10);
		}
		
		OGRFieldDefn shpField(this->outHeading.c_str(), OFTReal);
		shpField.SetPrecision(10);
		if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
		{
			string message = string("Creating output field has failed");
			throw RSGISVectorOutputException(message.c_str());
		}
	}
	
	
	RSGISVectorMaths::~RSGISVectorMaths()
	{
		
	}
}}
