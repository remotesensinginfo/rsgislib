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
	
    
    RSGISAttCountPolyIntersect::RSGISAttCountPolyIntersect()
    {
        
    }
    
    void RSGISAttCountPolyIntersect::countPolysIntersections(RSGISAttributeTable *attTable, GDALDataset *clumps, OGRLayer *polys, string field, pixelInPolyOption pixelPolyOption)throw(RSGISAttributeTableException)
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
            if(attTable->getDataType(field) != rsgis_int)
            {
                throw RSGISAttributeTableException("The field provided must be an integer type.");
            }
            
            unsigned int fieldIdx = attTable->getFieldIndex(field);
            
            while( (inFeature = polys->GetNextFeature()) != NULL )
			{
				fid = inFeature->GetFID();
                cout << "Extracting Data for feature " << fid << endl;
				
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
					cout << "WARNING: NULL Geometry Present within input file - IGNORED\n";
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
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    void RSGISAttCountPolyIntersect::countPolysIntersections(RSGISAttributeTable *attTable, GDALDataset *clumps, vector<OGRPolygon*> *polys, string field, pixelInPolyOption pixelPolyOption)throw(RSGISAttributeTableException)
    {
        try
        {
            if(!attTable->hasAttribute(field))
            {
                attTable->addAttIntField(field, 0);
            }
            if(attTable->getDataType(field) != rsgis_int)
            {
                throw RSGISAttributeTableException("The field provided must be an integer type.");
            }
            
            unsigned int fieldIdx = attTable->getFieldIndex(field);
            
            for(vector<OGRPolygon*>::iterator iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys)
            {
                this->countPolyIntersections(attTable, clumps, *iterPolys, fieldIdx, pixelPolyOption);
            }
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    void RSGISAttCountPolyIntersect::countPolyIntersections(RSGISAttributeTable *attTable, GDALDataset *clumps, OGRPolygon *ogrPoly, unsigned int field, pixelInPolyOption pixelPolyOption)throw(RSGISAttributeTableException)
    {
        try
        {
            RSGISVectorUtils vecUtils;
            GDALDataset **datasets = new GDALDataset*[1];
            datasets[0] = clumps;
            Envelope *env = vecUtils.getEnvelope(ogrPoly);
            
            RSGISCalcValueCountInAttTable calcVal(attTable, field);
            RSGISPopulateAttTableForPolygonIntersect popAttTab(&calcVal, datasets, 1, pixelPolyOption);
            popAttTab.processFeature(NULL, ogrPoly, env, 0);
            
            delete[] datasets;
            delete env;
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISAttCountPolyIntersect::~RSGISAttCountPolyIntersect()
    {
        
    }
    

    
    
    RSGISPopulateAttTableForPolygonIntersect::RSGISPopulateAttTableForPolygonIntersect(RSGISCalcValueCountInAttTable *valueCalc, GDALDataset **datasets, int numDS, pixelInPolyOption pixelPolyOption)
    {
        this->numImageBands = numImageBands;
        this->valueCalc = valueCalc;
        this->datasets = datasets;
        this->numDS = numDS;
        this->pixelPolyOption = pixelPolyOption;
    }
    
    void RSGISPopulateAttTableForPolygonIntersect::processFeature(OGRFeature *feature, OGRPolygon *poly, Envelope *env, long fid) throw(RSGISVectorException)
    {
        try
        {
            RSGISVectorUtils vecUtils;
            RSGISCalcImage calcImage(valueCalc);
            Polygon *geosPoly = vecUtils.convertOGRPolygon2GEOSPolygon(poly);
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
    
     
     
    RSGISCalcValueCountInAttTable::RSGISCalcValueCountInAttTable(RSGISAttributeTable *attTable, unsigned int fieldIdx):RSGISCalcImageValue(0)
    {
        this->attTable = attTable;
        this->fieldIdx = fieldIdx;
    }
    
    void RSGISCalcValueCountInAttTable::calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException)
    {        
        if(bandValues[0] > 0)
        {
            unsigned long clumpIdx = 0;
            try
            {
                clumpIdx = lexical_cast<unsigned long>(bandValues[0])-1;
            }
            catch(bad_lexical_cast &e)
            {
                throw RSGISImageCalcException(e.what());
            }
            
            ++attTable->getFeature(clumpIdx)->intFields->at(fieldIdx);
        }
        
    }
    
    RSGISCalcValueCountInAttTable::~RSGISCalcValueCountInAttTable()
    {
        
    }
    
    
    
}}




