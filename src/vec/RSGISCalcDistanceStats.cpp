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
    
    void RSGISCalcMinDists2Polys::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
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
    
    void RSGISCalcMinDists2Polys::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
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
    
    
    
    
    
    
    RSGISCalcMinDist2Geoms::RSGISCalcMinDist2Geoms(std::string outColName, std::vector<OGRGeometry*> *geoms)
    {
        this->outColName = outColName;
        this->geoms = geoms;
        this->firstGeom = true;
        this->maxMinDist = 0.0;
    }
    
    void RSGISCalcMinDist2Geoms::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
    {
        try
        {
            OGRGeometry *geom = inFeature->GetGeometryRef();
            OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();

            double dist = 0.0;
            double minDist = 0.0;
            bool first = true;
            for(std::vector<OGRGeometry*>::iterator iterGeoms = geoms->begin(); iterGeoms != geoms->end(); ++iterGeoms)
            {
                if(!geom->Equals(*iterGeoms))
                {
                    dist = geom->Distance(*iterGeoms);
                    if(first)
                    {
                        minDist = dist;
                        first = false;
                    }
                    else if(dist < minDist)
                    {
                        minDist = dist;
                    }
                }
            }
            
            if(firstGeom)
            {
                maxMinDist = minDist;
                firstGeom = false;
            }
            else if(minDist > maxMinDist)
            {
                maxMinDist = minDist;
            }
                
            outFeature->SetField(outFeatureDefn->GetFieldIndex(outColName.c_str()), minDist);
            
        }
        catch (RSGISVectorException &e)
        {
            throw e;
        }
    }
    
    void RSGISCalcMinDist2Geoms::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
    {
        try
        {
            OGRGeometry *geom = feature->GetGeometryRef();
            
            double dist = 0.0;
            double minDist = 0.0;
            int minFID = 0;
            bool first = true;
            int geomCount = 0;
            for(std::vector<OGRGeometry*>::iterator iterGeoms = geoms->begin(); iterGeoms != geoms->end(); ++iterGeoms)
            {
                if(!geom->Equals(*iterGeoms))
                {
                    dist = geom->Distance(*iterGeoms);
                    if(first)
                    {
                        minDist = dist;
                        first = false;
                        minFID = geomCount;
                    }
                    else if(dist < minDist)
                    {
                        minDist = dist;
                        minFID = geomCount;
                    }
                }
                geomCount = geomCount + 1;
            }
            
            if(firstGeom)
            {
                maxMinDist = minDist;
                firstGeom = false;
            }
            else if(minDist > maxMinDist)
            {
                maxMinDist = minDist;
            }
        }
        catch (RSGISVectorException &e)
        {
            throw e;
        }
    }
    
    void RSGISCalcMinDist2Geoms::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
    {
        OGRFieldDefn shpField(outColName.c_str(), OFTReal);
        shpField.SetPrecision(10);
        if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
        {
            std::string message = std::string("Creating shapefile field \'MinDist\' has failed");
            throw RSGISVectorOutputException(message.c_str());
        }
        
        OGRFieldDefn shpMFIDField("MinFID", OFTInteger);
        shpMFIDField.SetWidth(5);
        if( outputLayer->CreateField( &shpMFIDField ) != OGRERR_NONE )
        {
            std::string message = std::string("Creating shapefile field \'MinFID\' has failed");
            throw RSGISVectorOutputException(message.c_str());
        }
    }
    
    RSGISCalcMinDist2Geoms::~RSGISCalcMinDist2Geoms()
    {
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    RSGISCalcMinDist2GeomsUseIdx::RSGISCalcMinDist2GeomsUseIdx(std::string outColName, geos::index::SpatialIndex *geomIdx, double maxSearchDist)
    {
        this->outColName = outColName;
        this->geomIdx = geomIdx;
        this->maxSearchDist = maxSearchDist;
        this->firstGeom = true;
        this->maxMinDist = 0.0;
    }
    
    void RSGISCalcMinDist2GeomsUseIdx::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
    {
        try
        {
            OGRGeometry *inGeom = inFeature->GetGeometryRef();
            OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
            
            env->expandBy(this->maxSearchDist);
            std::vector<void*> geomResults;
            this->geomIdx->query(env, geomResults);
            
            double dist = 0.0;
            double minDist = 0.0;
            int minFID = 0;
            bool first = true;
            RSGISGEOSGeomFID *geomObj;
            OGREnvelope *envOGRTmp = new OGREnvelope();
            geos::geom::Envelope *tmpEnv = new geos::geom::Envelope();
            for(std::vector<void*>::iterator iterGeoms = geomResults.begin(); iterGeoms != geomResults.end(); ++iterGeoms)
            {
                geomObj = (RSGISGEOSGeomFID*) (*iterGeoms);
                geomObj->geom->getEnvelope(envOGRTmp);
                tmpEnv->init(envOGRTmp->MinX, envOGRTmp->MaxX, envOGRTmp->MinY, envOGRTmp->MaxY);
                if(!inGeom->Equals(geomObj->geom))
                {
                    dist = inGeom->Distance(geomObj->geom);
                    if(first)
                    {
                        minDist = dist;
                        minFID = geomObj->fid;
                        first = false;
                    }
                    else if(dist < minDist)
                    {
                        minDist = dist;
                        minFID = geomObj->fid;
                    }
                }
            }
            delete envOGRTmp;
            delete tmpEnv;
            
            if(first)
            {
                minDist = maxSearchDist;
                minFID = -1;
            }
            
            if(firstGeom)
            {
                maxMinDist = minDist;
                firstGeom = false;
            }
            else if(minDist > maxMinDist)
            {
                maxMinDist = minDist;
            }
            
            outFeature->SetField(outFeatureDefn->GetFieldIndex(outColName.c_str()), minDist);
            outFeature->SetField(outFeatureDefn->GetFieldIndex("MinFID"), minFID);
            
        }
        catch (RSGISVectorException &e)
        {
            throw e;
        }
    }
    
    void RSGISCalcMinDist2GeomsUseIdx::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
    {
        try
        {
            OGRGeometry *inGeom = feature->GetGeometryRef();
            OGRFeatureDefn *outFeatureDefn = feature->GetDefnRef();
            
            env->expandBy(this->maxSearchDist);
            std::vector<void*> geomResults;
            this->geomIdx->query(env, geomResults);
            
            double dist = 0.0;
            double minDist = 0.0;
            int minFID = 0;
            bool first = true;
            RSGISGEOSGeomFID *geomObj;
            OGREnvelope *envOGRTmp = new OGREnvelope();
            geos::geom::Envelope *tmpEnv = new geos::geom::Envelope();
            for(std::vector<void*>::iterator iterGeoms = geomResults.begin(); iterGeoms != geomResults.end(); ++iterGeoms)
            {
                geomObj = (RSGISGEOSGeomFID*) (*iterGeoms);
                geomObj->geom->getEnvelope(envOGRTmp);
                tmpEnv->init(envOGRTmp->MinX, envOGRTmp->MaxX, envOGRTmp->MinY, envOGRTmp->MaxY);
                if(!inGeom->Equals(geomObj->geom))
                {
                    dist = inGeom->Distance(geomObj->geom);
                    if(first)
                    {
                        minDist = dist;
                        minFID = geomObj->fid;
                        first = false;
                    }
                    else if(dist < minDist)
                    {
                        minDist = dist;
                        minFID = geomObj->fid;
                    }
                }
            }
            delete envOGRTmp;
            delete tmpEnv;
            
            if(first)
            {
                minDist = maxSearchDist;
                minFID = -1;
            }
            
            if(firstGeom)
            {
                maxMinDist = minDist;
                firstGeom = false;
            }
            else if(minDist > maxMinDist)
            {
                maxMinDist = minDist;
            }
            
            feature->SetField(outFeatureDefn->GetFieldIndex(outColName.c_str()), minDist);
            feature->SetField(outFeatureDefn->GetFieldIndex("MinFID"), minFID);
        }
        catch (RSGISVectorException &e)
        {
            throw e;
        }
    }
    
    void RSGISCalcMinDist2GeomsUseIdx::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
    {
        OGRFieldDefn shpField(outColName.c_str(), OFTReal);
        shpField.SetPrecision(10);
        if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
        {
            std::string message = std::string("Creating shapefile field \'MinDist\' has failed");
            throw RSGISVectorOutputException(message.c_str());
        }
        
        OGRFieldDefn shpMFIDField("MinFID", OFTInteger);
        shpMFIDField.SetWidth(5);
        if( outputLayer->CreateField( &shpMFIDField ) != OGRERR_NONE )
        {
            std::string message = std::string("Creating shapefile field \'MinFID\' has failed");
            throw RSGISVectorOutputException(message.c_str());
        }
    }
    
    RSGISCalcMinDist2GeomsUseIdx::~RSGISCalcMinDist2GeomsUseIdx()
    {
        
    }
    
    
    
    
}}




