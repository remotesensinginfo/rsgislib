/*
 *  RSGISAttCountPolyIntersect.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 08/03/2012.
 *  Copyright 2012 RSGISLib. All rights reserved.
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

#include "RSGISAttCountPolyIntersect.h"

namespace rsgis{namespace vec{
	
    /*
    RSGISAttCountPolyIntersect::RSGISAttCountPolyIntersect()
    {
        
    }
    
    void RSGISAttCountPolyIntersect::countPolysIntersections(rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, OGRLayer *polys, std::string field, rsgis::img::pixelInPolyOption pixelPolyOption)
    {
        try
        {
            OGRPolygon *ogrPolygon = NULL;
            OGRGeometry *geometry = NULL;
            OGRFeature *inFeature = NULL;
            long fid = 0;
            bool nullGeometry;
            
            if(!attTable->hasAttribute(field))
            {
                attTable->addAttIntField(field, 0);
            }
            if(attTable->getDataType(field) != rsgis::rastergis::rsgis_int)
            {
                throw rsgis::RSGISAttributeTableException("The field provided must be an integer type.");
            }
            
            unsigned int fieldIdx = attTable->getFieldIndex(field);
            
            while( (inFeature = polys->GetNextFeature()) != NULL )
			{
				fid = inFeature->GetFID();
                std::cout << "Extracting Data for feature " << fid << std::endl;
				
				// Get Geometry.
				nullGeometry = false;
				geometry = inFeature->GetGeometryRef();
				if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
				{
					ogrPolygon = (OGRPolygon *) geometry;
					
				}
				else 
				{
					nullGeometry = true;
                    std::cout << "WARNING: NULL Geometry Present within input file - IGNORED\n";
				}
				
				if(!nullGeometry)
				{					
					this->countPolyIntersections(attTable, clumps, ogrPolygon, fieldIdx, pixelPolyOption);
				}
				
				OGRFeature::DestroyFeature(inFeature);
			}
        }
        catch(RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
    }
    
    void RSGISAttCountPolyIntersect::countPolysIntersections(rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, std::vector<OGRPolygon*> *polys, std::string field, rsgis::img::pixelInPolyOption pixelPolyOption)
    {
        try
        {
            if(!attTable->hasAttribute(field))
            {
                attTable->addAttIntField(field, 0);
            }
            if(attTable->getDataType(field) != rsgis::rastergis::rsgis_int)
            {
                throw rsgis::RSGISAttributeTableException("The field provided must be an integer type.");
            }
            
            unsigned int fieldIdx = attTable->getFieldIndex(field);
            
            for(std::vector<OGRPolygon*>::iterator iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys)
            {
                this->countPolyIntersections(attTable, clumps, *iterPolys, fieldIdx, pixelPolyOption);
            }
        }
        catch(RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
    }
    
    void RSGISAttCountPolyIntersect::countPolyIntersections(rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, OGRPolygon *ogrPoly, unsigned int field, rsgis::img::pixelInPolyOption pixelPolyOption)
    {
        try
        {
            RSGISVectorUtils vecUtils;
            GDALDataset **datasets = new GDALDataset*[1];
            datasets[0] = clumps;
            geos::geom::Envelope *env = vecUtils.getEnvelope(ogrPoly);
            
            RSGISCalcValueCountInAttTable calcVal(attTable, field);
            RSGISPopulateAttTableForPolygonIntersect popAttTab(&calcVal, datasets, 1, pixelPolyOption);
            popAttTab.processFeature(NULL, ogrPoly, env, 0);
            
            delete[] datasets;
            delete env;
        }
        catch(RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISAttCountPolyIntersect::~RSGISAttCountPolyIntersect()
    {
        
    }
    

    
    
    RSGISPopulateAttTableForPolygonIntersect::RSGISPopulateAttTableForPolygonIntersect(RSGISCalcValueCountInAttTable *valueCalc, GDALDataset **datasets, int numDS, rsgis::img::pixelInPolyOption pixelPolyOption)
    {
        this->valueCalc = valueCalc;
        this->datasets = datasets;
        this->numDS = numDS;
        this->pixelPolyOption = pixelPolyOption;
    }
    
    void RSGISPopulateAttTableForPolygonIntersect::processFeature(OGRFeature *feature, OGRPolygon *poly, geos::geom::Envelope *env, long fid)
    {
        try
        {
            RSGISVectorUtils vecUtils;
            rsgis::img::RSGISCalcImage calcImage(valueCalc);
            geos::geom::Polygon *geosPoly = vecUtils.convertOGRPolygon2GEOSPolygon(poly);
            calcImage.calcImageWithinPolygonExtent(this->datasets, this->numDS, env, geosPoly, this->pixelPolyOption);
            delete geosPoly;
        }
        catch(RSGISException &e)
        {
            throw RSGISVectorException(e.what());
        }
    }
    
    RSGISPopulateAttTableForPolygonIntersect::~RSGISPopulateAttTableForPolygonIntersect()
    {
        
    }
    
     
     
    RSGISCalcValueCountInAttTable::RSGISCalcValueCountInAttTable(rsgis::rastergis::RSGISAttributeTable *attTable, unsigned int fieldIdx):RSGISCalcImageValue(0)
    {
        this->attTable = attTable;
        this->fieldIdx = fieldIdx;
    }
    
    void RSGISCalcValueCountInAttTable::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) 
    {        
        if(bandValues[0] > 0)
        {
            unsigned long clumpIdx = 0;
            try
            {
                clumpIdx = boost::lexical_cast<unsigned long>(bandValues[0])-1;
            }
            catch(boost::bad_lexical_cast &e)
            {
                throw rsgis::img::RSGISImageCalcException(e.what());
            }
            
            ++attTable->getFeature(clumpIdx)->intFields->at(fieldIdx);
        }
        
    }
    
    RSGISCalcValueCountInAttTable::~RSGISCalcValueCountInAttTable()
    {
        
    }
    
    */
    
}}




