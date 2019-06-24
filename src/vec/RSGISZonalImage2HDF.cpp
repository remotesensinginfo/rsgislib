/*
 *  RSGISZonalImage2HDF.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/09/2013.
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

#include "RSGISZonalImage2HDF.h"

namespace rsgis{namespace vec{
    
    RSGISZonalImage2HDF::RSGISZonalImage2HDF()
    {
        
    }
		
    void RSGISZonalImage2HDF::extractBandsToColumns(GDALDataset *dataset, OGRLayer *vecLayer, std::string outputFile, rsgis::img::pixelInPolyOption pixelPolyOption)
    {
        try
        {
            unsigned int numImageBands = dataset->GetRasterCount();
            
            std::vector<float*> *pxlVals = new std::vector<float*>();
            
            RSGISExtractPixelValues *extractVals = new RSGISExtractPixelValues(pxlVals);
            RSGISExtractZonalPixelValues *extractZonalValues = new RSGISExtractZonalPixelValues(extractVals, &dataset, 1, pixelPolyOption);
            
            RSGISVectorUtils vecUtils;
            OGRPolygon *polygon = NULL;
            OGRGeometry *geometry = NULL;
            geos::geom::Envelope *env = NULL;
            OGRFeature *inFeature = NULL;
            long fid = 0;
            bool nullGeometry;
            unsigned int featureCount = 0;
            
            while( (inFeature = vecLayer->GetNextFeature()) != NULL )
			{
				fid = inFeature->GetFID();
                std::cout << "\rExtracting Data for feature " << fid << "...       ";
				
				// Get Geometry.
				nullGeometry = false;
				geometry = inFeature->GetGeometryRef();
				if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
				{
					polygon = (OGRPolygon *) geometry;
					env = vecUtils.getEnvelope(polygon);
				}
				else
				{
					nullGeometry = true;
					std::cout << "\nWARNING: NULL Geometry Present within input file - IGNORED\n";
				}
				
				if(!nullGeometry)
				{
					extractZonalValues->processFeature(inFeature, polygon, env, fid);

					delete env;
                    ++featureCount;
				}
				
				OGRFeature::DestroyFeature(inFeature);
			}
            std::cout << std::endl;
            
            rsgis::utils::RSGISExportColumnData2HDF exportCols2HDF;
            exportCols2HDF.createFile(outputFile, numImageBands, std::string("Pixels Extracted from ")+std::string(dataset->GetFileList()[0]), H5::PredType::IEEE_F32LE);
            float *row = new float[numImageBands];
            for(unsigned int j = 0; j < pxlVals->size(); ++j)
            {
                for(unsigned int i = 0; i < numImageBands; ++i)
                {
                    row[i] = pxlVals->at(j)[i];
                }
                exportCols2HDF.addDataRow(row, H5::PredType::NATIVE_FLOAT);
            }
            exportCols2HDF.close();
            delete[] row;
            
            for(std::vector<float*>::iterator iterVals = pxlVals->begin(); iterVals != pxlVals->end(); ++iterVals)
            {
                delete[] *iterVals;
            }
            delete pxlVals;
            
            delete extractVals;
            delete extractZonalValues;
        }
        catch(RSGISException &e)
        {
            throw RSGISVectorZonalException(e.what());
        }
    }
		
    RSGISZonalImage2HDF::~RSGISZonalImage2HDF()
    {
        
    }
    
    
    RSGISExtractPixelValues::RSGISExtractPixelValues(std::vector<float*> *pxlVals):rsgis::img::RSGISCalcImageValue(0)
    {
        this->pxlVals = pxlVals;
    }
    
    void RSGISExtractPixelValues::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) 
    {
        float *data = new float[numBands];
        for(unsigned i = 0; i < numBands; ++i)
        {
            data[i] = bandValues[i];
        }
        pxlVals->push_back(data);
    }
    
    RSGISExtractPixelValues::~RSGISExtractPixelValues()
    {
        
    }
    
    
    RSGISExtractZonalPixelValues::RSGISExtractZonalPixelValues(RSGISExtractPixelValues *valueCalc, GDALDataset **datasets, int numDS, rsgis::img::pixelInPolyOption pixelPolyOption)
    {
        this->valueCalc = valueCalc;
        this->datasets = datasets;
        this->numDS = numDS;
        this->pixelPolyOption = pixelPolyOption;
    }
    
    void RSGISExtractZonalPixelValues::processFeature(OGRFeature *feature, OGRPolygon *poly, geos::geom::Envelope *env, long fid)
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
    
    RSGISExtractZonalPixelValues::~RSGISExtractZonalPixelValues()
    {
        
    }
    
    
    
}}



