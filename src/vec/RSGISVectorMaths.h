 /*
 *  RSGISVectorMaths.h
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

#ifndef RSGISVectorMaths_H
#define RSGISVectorMaths_H

#include <iostream>
#include <string>

#include "vec/RSGISProcessOGRFeature.h"
#include "muParser.h"


using namespace std;
using namespace rsgis;
using namespace mu;

namespace rsgis{namespace vec{
	
	struct VariableFields 
	{
		string name;
		string fieldName;
	};
	
	class RSGISVectorMaths : public RSGISProcessOGRFeature
	{
	public:
		RSGISVectorMaths(VariableFields **variables, int numVariables, string mathsExpression, string outHeading);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException){throw RSGISVectorException("Not Implemented");};
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		~RSGISVectorMaths();
	private:
		VariableFields **variables;
		int numVariables;
		Parser *muParser;
		value_type *inVals;
		string outHeading;
	};
}}

#endif
