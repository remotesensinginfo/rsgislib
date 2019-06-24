/*
 *  RSGISCopyFeatures.cpp
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

#include "RSGISCopyFeatures.h"

namespace rsgis{namespace vec{
	

	RSGISCopyFeatures::RSGISCopyFeatures()
	{
		
	}
	
	void RSGISCopyFeatures::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
	{
		
	}
	
	void RSGISCopyFeatures::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
	{
		
		
	}
	
	void RSGISCopyFeatures::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
	{
		
	}
	
	RSGISCopyFeatures::~RSGISCopyFeatures()
	{
		
	}
    
    
    
    
    RSGISCopyFeaturesAddFIDCol::RSGISCopyFeaturesAddFIDCol(unsigned int initFID)
	{
		fidCount = initFID;
        fieldIdx = 0;
	}
	
	void RSGISCopyFeaturesAddFIDCol::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
	{        
		outFeature->SetField(fieldIdx, fidCount);
        ++fidCount;
	}
	
	void RSGISCopyFeaturesAddFIDCol::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
	{
		
		
	}
	
	void RSGISCopyFeaturesAddFIDCol::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
	{
		if( outputLayer->CreateField( new OGRFieldDefn("FID", OFTInteger)) != OGRERR_NONE )
        {
            throw RSGISVectorOutputException("Creating FID field has failed.");
        }
        
        fieldIdx = outputLayer->GetLayerDefn()->GetFieldIndex("FID");
	}
	
	RSGISCopyFeaturesAddFIDCol::~RSGISCopyFeaturesAddFIDCol()
	{
		
	}
}}



