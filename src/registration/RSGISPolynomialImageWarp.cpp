/*
 *  RSGISPolynomialImageWarp.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/09/2010.
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

#include "RSGISPolynomialImageWarp.h"


namespace rsgis{namespace reg{
	
	
	RSGISPolynomialImageWarp::RSGISPolynomialImageWarp(string inputImage, string outputImage, string outProjWKT, string gcpFilePath, float outImgRes, RSGISWarpImageInterpolator *interpolator, unsigned int polyOrder, string gdalFormat) : RSGISWarpImage(inputImage, outputImage, outProjWKT, gcpFilePath, outImgRes, interpolator, gdalFormat)
	{
		this->polyOrder = polyOrder;
	}
	
	void RSGISPolynomialImageWarp::initWarp()throw(RSGISImageWarpException)
	{
		// Create polynominal model based on ground countrol points.
        
        cout << "Fitting polynomial..." << endl;
        
        unsigned int coeffSize = 3 * this->polyOrder; // x**N + y**N + xy**(N-1) + 1
        
		// Set up matrices
		gsl_matrix *eastNorthPow = gsl_matrix_alloc(gcps->size(), coeffSize); // Matrix to hold powers of easting and northing (used for both fits)
		gsl_vector *pixValX = gsl_vector_alloc(gcps->size()); // Vector to hold pixel values (X)
        gsl_vector *pixValY = gsl_vector_alloc(gcps->size()); // Vector to hold pixel values (Y)
		this->aX = gsl_vector_alloc(coeffSize); // Vector to hold coeffifients of X (Easting)
        this->aY = gsl_vector_alloc(coeffSize); // Vector to hold coeffifients of Y (Northing)
        
        unsigned int pointN = 0;
        unsigned int offset = 0;
		
        vector<RSGISGCPImg2MapNode*>::iterator iterGCPs;
		for(iterGCPs = gcps->begin(); iterGCPs != gcps->end(); ++iterGCPs) // Populate matrices using ground control points.
		{
            // Add pixel values into vectors
            gsl_vector_set(pixValX, pointN, (*iterGCPs)->imgX()); // Pixel X
            gsl_vector_set(pixValY, pointN, (*iterGCPs)->imgY()); // Pixel Y

            gsl_matrix_set(eastNorthPow, pointN, 0, 1.);
            
			for(int j = 1; j < polyOrder; ++j)
			{
				offset = 1 + (3 * (j - 1));
                gsl_matrix_set(eastNorthPow, pointN, offset, pow((*iterGCPs)->eastings(), j));
                gsl_matrix_set(eastNorthPow, pointN, offset+1, pow((*iterGCPs)->northings(), j));
                gsl_matrix_set(eastNorthPow, pointN, offset+2, pow((*iterGCPs)->eastings()*(*iterGCPs)->northings(), j));
			}
            
            offset = 1 + (3 * (this->polyOrder - 1));
            gsl_matrix_set(eastNorthPow, pointN, offset, pow((*iterGCPs)->eastings(), this->polyOrder));
            gsl_matrix_set(eastNorthPow, pointN, offset+1, pow((*iterGCPs)->northings(), this->polyOrder));
            
            ++pointN;
		}
		
		// Set up worksapce for fitting
		gsl_multifit_linear_workspace *workspace;
		workspace = gsl_multifit_linear_alloc(gcps->size(), coeffSize);
		gsl_matrix *cov = gsl_matrix_alloc(coeffSize, coeffSize);
        
		double chisq = 0;
        
        // Fit for X
		gsl_multifit_linear(eastNorthPow, pixValX, this->aX, cov, &chisq, workspace);
        // Fit for Y
		gsl_multifit_linear(eastNorthPow, pixValY, this->aY, cov, &chisq, workspace);        
        
        cout << "Fitted polynomial." << endl;
        
        // Test polynominal fit and calculate RMSE
        double sqSum = 0;
        
        for(iterGCPs = gcps->begin(); iterGCPs != gcps->end(); ++iterGCPs) // Populate matrices using ground control points.
        {
            double pX = 0;
            double pY = 0;
            
            // Add pixel values into vectors        
            pX = pX + gsl_vector_get(this->aX, 0);
            pY = pY + gsl_vector_get(this->aY, 0);
            
            for(int j = 1; j < this->polyOrder; ++j)
            {
                offset = 1 + (3 * (j - 1));
                
                pX = pX + (gsl_vector_get(aX, offset) * pow((*iterGCPs)->eastings(), j));
                pX = pX + (gsl_vector_get(aX, offset+1) * pow((*iterGCPs)->northings(), j));
                pX = pX + (gsl_vector_get(aX, offset+2) * pow((*iterGCPs)->eastings()*(*iterGCPs)->northings(), j));
                
                pY = pY + (gsl_vector_get(aY, offset) * pow((*iterGCPs)->eastings(), j));
                pY = pY + (gsl_vector_get(aY, offset+1) * pow((*iterGCPs)->northings(), j));
                pY = pY + (gsl_vector_get(aY, offset+2) * pow((*iterGCPs)->eastings()*(*iterGCPs)->northings(), j));
            }
            
            offset = 1 + (3 * (this->polyOrder - 1));
            pX = pX + (gsl_vector_get(aX, offset) * pow((*iterGCPs)->eastings(), this->polyOrder));
            pX = pX + (gsl_vector_get(aX, offset+1) * pow((*iterGCPs)->northings(), this->polyOrder));
            
            pY = pY + (gsl_vector_get(aY, offset) * pow((*iterGCPs)->eastings(), this->polyOrder));
            pY = pY + (gsl_vector_get(aY, offset+1) * pow((*iterGCPs)->northings(), this->polyOrder));
            
            sqSum = sqSum + (pow((*iterGCPs)->imgX() - pX ,2) + pow((*iterGCPs)->imgY() - pY ,2));
            
            //cout <<  << ", " << pX << ", " << (*iterGCPs)->imgY() << ", " << pY << endl;
        }
    
        double sqMean = sqSum / double(gcps->size());
		
		double rmse = sqrt(sqMean);
		
		cout << "RMSE = " << rmse << " pixels " << endl;
        
        // Tidy up
		gsl_multifit_linear_free(workspace);
		gsl_matrix_free(eastNorthPow);
		gsl_vector_free(pixValX);
        gsl_vector_free(pixValY);
		gsl_matrix_free(cov);

	}
	
	Envelope* RSGISPolynomialImageWarp::newImageExtent() throw(RSGISImageWarpException)
	{
		double minEastings = 0;
		double maxEastings = 0;
		double minNorthings = 0;
		double maxNorthings = 0;
		bool first = true;
		
		vector<RSGISGCPImg2MapNode*>::iterator iterGCPs;
		for(iterGCPs = gcps->begin(); iterGCPs != gcps->end(); ++iterGCPs)
		{
			if(first)
			{
				minEastings = (*iterGCPs)->eastings();
				maxEastings = (*iterGCPs)->eastings();
				minNorthings = (*iterGCPs)->northings();
				maxNorthings = (*iterGCPs)->northings();
				first = false;
			}
			else
			{
				if((*iterGCPs)->eastings() < minEastings)
				{
					minEastings = (*iterGCPs)->eastings();
				}
				else if((*iterGCPs)->eastings() > maxEastings)
				{
					maxEastings = (*iterGCPs)->eastings();
				}
				
				if((*iterGCPs)->northings() < minNorthings)
				{
					minNorthings = (*iterGCPs)->northings();
				}
				else if((*iterGCPs)->northings() > maxNorthings)
				{
					maxNorthings = (*iterGCPs)->northings();
				}
			}
		}
		
		cout << "Eastings: [" << minEastings << "," << maxEastings << "]\n";
		cout << "Northings: [" << minNorthings << "," << maxNorthings << "]\n";
		
		Envelope *env = new Envelope(minEastings, maxEastings, minNorthings, maxNorthings);
		
		double geoWidth = maxEastings - minEastings;
		double geoHeight = maxNorthings - minNorthings;
		
		if((geoWidth <= 0) | (geoHeight <= 0))
		{
			throw RSGISImageWarpException("Either the output image width or height are <= 0.");
		}
		
		return env;
	}
	
	void RSGISPolynomialImageWarp::findNearestPixel(double eastings, double northings, unsigned int *x, unsigned int *y, float inImgRes) throw(RSGISImageWarpException)
	{
		
        /* Return nearest pixel based on input easting and northing.
           Pixel x and y coordinates are found from polynominal model */
        double pX = 0;
        double pY = 0;
        unsigned int offset = 0;
        
        // Add pixel values into vectors        
        pX = pX + gsl_vector_get(this->aX, 0);
        pY = pY + gsl_vector_get(this->aY, 0);
        
        for(int j = 1; j < this->polyOrder; ++j)
        {
            offset = 1 + (3 * (j - 1));
    
            pX = pX + (gsl_vector_get(aX, offset) * pow(eastings, j));
            pX = pX + (gsl_vector_get(aX, offset+1) * pow(northings, j));
            pX = pX + (gsl_vector_get(aX, offset+2) * pow(eastings*northings, j));

            pY = pY + (gsl_vector_get(aY, offset) * pow(eastings, j));
            pY = pY + (gsl_vector_get(aY, offset+1) * pow(northings, j));
            pY = pY + (gsl_vector_get(aY, offset+2) * pow(eastings*northings, j));
        }
        
        offset = 1 + (3 * (this->polyOrder - 1));
        pX = pX + (gsl_vector_get(aX, offset) * pow(eastings, this->polyOrder));
        pX = pX + (gsl_vector_get(aX, offset+1) * pow(northings, this->polyOrder));
        
        pY = pY + (gsl_vector_get(aY, offset) * pow(eastings, this->polyOrder));
        pY = pY + (gsl_vector_get(aY, offset+1) * pow(northings, this->polyOrder));
    
		*x = floor(pX+0.5);
		*y = floor(pY+0.5);
	}
		
	RSGISPolynomialImageWarp::~RSGISPolynomialImageWarp()
	{
        gsl_vector_free(this->aX);
        gsl_vector_free(this->aY);
	}
	
}}




