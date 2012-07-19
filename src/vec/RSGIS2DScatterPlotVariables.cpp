/*
 *  RSGIS2DScatterPlotVariables.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/10/2009.
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

#include "RSGIS2DScatterPlotVariables.h"


namespace rsgis{namespace vec{
	
	RSGIS2DScatterPlotVariables::RSGIS2DScatterPlotVariables(rsgis::utils::RSGISExportForPlottingIncremental *plotter, std::string col1, std::string col2)
	{
		this->plotter = plotter;
		this->col1 = col1;
		this->col2 = col2;
	}
	
	void RSGIS2DScatterPlotVariables::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGIS2DScatterPlotVariables::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		OGRFeatureDefn *featureDefn = feature->GetDefnRef();
		
		int col1attributeIdx = featureDefn->GetFieldIndex(this->col1.c_str());
		if(col1attributeIdx < 0)
		{
			std::string message = "This layer does not contain a field with the name \'" + this->col1 + "\'";
			throw RSGISVectorException(message.c_str());
		}
		double valueCol1 = feature->GetFieldAsDouble(col1attributeIdx);

		int col2attributeIdx = featureDefn->GetFieldIndex(this->col2.c_str());
		if(col2attributeIdx < 0)
		{
			std::string message = "This layer does not contain a field with the name \'" + this->col2 + "\'";
			throw RSGISVectorException(message.c_str());
		}
		double valueCol2 = feature->GetFieldAsDouble(col2attributeIdx);
		
		
		plotter->writeScatter2DLine(valueCol1, valueCol2);
				
	}
	
	void RSGIS2DScatterPlotVariables::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		// Nothing to do!
	}
	
	RSGIS2DScatterPlotVariables::~RSGIS2DScatterPlotVariables()
	{

	}
}}





