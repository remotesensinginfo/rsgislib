/*
 *  RSGISExtractEndMembers2Matrix.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/02/2012.
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

#include "RSGISExtractEndMembers2Matrix.h"

namespace rsgis{namespace vec{
	

    RSGISExtractEndMembers2Matrix::RSGISExtractEndMembers2Matrix()
    {
        
    }
		
    void RSGISExtractEndMembers2Matrix::extractColumnPerPolygon2Matrix(GDALDataset *dataset, OGRLayer *vecLayer, std::string outputMatrix, rsgis::img::pixelInPolyOption pixelPolyOption)
    {
        try
        {
            rsgis::math::RSGISMatrices matrixUtils;
            GDALDataset **datasets = new GDALDataset*[1];
            datasets[0] = dataset;
            unsigned int numImageBands = dataset->GetRasterCount();
            
            float *sumVals = new float[numImageBands];
            for(unsigned int j = 0; j < numImageBands; ++j)
            {
                sumVals[j] = 0;
            }
            unsigned int countVals = 0;
            
            RSGISCalcSumValues *valueCalc = new RSGISCalcSumValues(sumVals, &countVals, numImageBands);
            RSGISExtractSumPixelValues *extractMeanValues = new RSGISExtractSumPixelValues(numImageBands, valueCalc, datasets, 1, pixelPolyOption);
            
            unsigned int numFeatures = vecLayer->GetFeatureCount(TRUE);
            
            rsgis::math::Matrix *endMembers = matrixUtils.createMatrix(numImageBands, numFeatures);
            for(unsigned int i = 0; i < numFeatures; ++i)
            {
                for(unsigned int j = 0; j < numImageBands; ++j)
                {
                    endMembers->matrix[(i*numImageBands)+j] = 0;
                }
            }
            
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
                std::cout << "Extracting Data for feature " << fid << std::endl;
				
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
					std::cout << "WARNING: NULL Geometry Present within input file - IGNORED\n";
				}
				
				if(!nullGeometry)
				{
                    std::cout << "Env: [" << env->getMinX() << ", " << env->getMaxX() << "][" << env->getMinY() << ", " << env->getMaxY() << "]\n";
                    
					extractMeanValues->processFeature(inFeature, polygon, env, fid);
                    
                    for(unsigned int j = 0; j < numImageBands; ++j)
                    {
                        endMembers->matrix[(j*numFeatures)+featureCount] = sumVals[j]/countVals;
                        sumVals[j] = 0;
                    }
                    countVals = 0;
					delete env;
                    ++featureCount;
				}
				
				OGRFeature::DestroyFeature(inFeature);
			}
            
            matrixUtils.saveMatrix2txt(endMembers, outputMatrix);
            
            matrixUtils.freeMatrix(endMembers);
            delete[] sumVals;
            delete valueCalc;
            delete extractMeanValues;
            delete[] datasets;
        }
        catch(RSGISException &e)
        {
            throw RSGISVectorZonalException(e.what());
        }
    }
    
    RSGISExtractEndMembers2Matrix::~RSGISExtractEndMembers2Matrix()
    {
        
    }
    
    
    
    
    RSGISExtractSumPixelValues::RSGISExtractSumPixelValues(unsigned int numImageBands, RSGISCalcSumValues *valueCalc, GDALDataset **datasets, int numDS, rsgis::img::pixelInPolyOption pixelPolyOption)
    {
        this->numImageBands = numImageBands;
        this->valueCalc = valueCalc;
        this->datasets = datasets;
        this->numDS = numDS;
        this->pixelPolyOption = pixelPolyOption;
    }
    
    void RSGISExtractSumPixelValues::processFeature(OGRFeature *feature, OGRPolygon *poly, geos::geom::Envelope *env, long fid)
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
    
    RSGISExtractSumPixelValues::~RSGISExtractSumPixelValues()
    {

    }
    
    
    
    RSGISCalcSumValues::RSGISCalcSumValues(float *sumVals, unsigned int *countVals, unsigned int numSumVals):rsgis::img::RSGISCalcImageValue(0)
    {
        this->sumVals = sumVals;
        this->countVals = countVals;
        this->numSumVals = numSumVals;
    }
    
    void RSGISCalcSumValues::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) 
    {
        if(numSumVals != ((unsigned int) numBands))
        {
            throw rsgis::img::RSGISImageCalcException("Number of expected bands and the number of bands inputted are not the same.");
        }
        
        for(int i = 0; i < numBands; ++i)
        {
            sumVals[i] = sumVals[i] + bandValues[i];
        }
        ++(*countVals);
    }
    
    RSGISCalcSumValues::~RSGISCalcSumValues()
    {
        
    }

    
    
}}




