/*
 *  RSGISCalcDistanceStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 08/02/2013.
 *  Copyright 2013 RSGISLib. All rights reserved.
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

#include "RSGISCalcDistanceStats.h"

namespace rsgis{namespace vec{
	
    RSGISCalcMinDists2Polys::RSGISCalcMinDists2Polys(std::vector<OGRGeometry*> *polys)
    {
        this->polys = polys;
    }
    
    void RSGISCalcMinDists2Polys::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
    {
        try
        {
            OGRGeometry *geom = inFeature->GetGeometryRef();
            OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
            int i = 0;
            double dist = 0;
            rsgis::math::RSGISMathsUtils mathUtils;
            std::string name = "";
            for(std::vector<OGRGeometry*>::iterator iterGeoms = polys->begin(); iterGeoms != polys->end(); ++iterGeoms)
            {
                dist = geom->Distance(*iterGeoms);
                name = "Dist_" + mathUtils.inttostring(i+1);
                
                outFeature->SetField(outFeatureDefn->GetFieldIndex(name.c_str()), dist);
                ++i;
            }
            
        }
        catch (RSGISVectorException &e)
        {
            throw e;
        }
    }
    
    void RSGISCalcMinDists2Polys::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
    {
        rsgis::math::RSGISMathsUtils mathUtils;
		std::string name = "";
		for(int i = 0; i < polys->size(); i++)
		{
			name = "Dist_" + mathUtils.inttostring(i+1);
			OGRFieldDefn shpField(name.c_str(), OFTReal);
            shpField.SetPrecision(10);
			if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field") + name + std::string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
    }
    
    RSGISCalcMinDists2Polys::~RSGISCalcMinDists2Polys()
    {
        
    }
}}




