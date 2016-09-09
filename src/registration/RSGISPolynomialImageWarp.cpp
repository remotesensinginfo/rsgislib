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
	
	
	RSGISPolynomialImageWarp::RSGISPolynomialImageWarp(std::string inputImage, std::string outputImage, std::string outProjWKT, std::string gcpFilePath, float outImgRes, RSGISWarpImageInterpolator *interpolator, unsigned int polyOrder, std::string gdalFormat) : RSGISWarpImage(inputImage, outputImage, outProjWKT, gcpFilePath, outImgRes, interpolator, gdalFormat)
	{
		this->polyOrder = polyOrder;
        std::cout << "polyOrder = " << polyOrder << std::endl;
	}
	
	void RSGISPolynomialImageWarp::initWarp()throw(RSGISImageWarpException)
	{
		/** Initialises warp by create polynominal models based on ground countrol points.
            Models are created expressing image pixels as a function of easting and northing, used for warping
            and expressing easting and northing as a function of image pixels to determine the corner location of the
            image to be warped.
         */
        
        unsigned int coeffSize = 3 * this->polyOrder; // x**N + y**N + xy**(N-1) + 1
        
		// Set up matrices
		gsl_matrix *eastNorthPow = gsl_matrix_alloc(gcps->size(), coeffSize); // Matrix to hold powers of easting and northing (used for both fits)
        gsl_matrix *xyPow = gsl_matrix_alloc(gcps->size(), coeffSize); // Matrix to hold powers of x and y (used for both fits)
		gsl_vector *pixValX = gsl_vector_alloc(gcps->size()); // Vector to hold pixel values (X)
        gsl_vector *pixValY = gsl_vector_alloc(gcps->size()); // Vector to hold pixel values (Y)
		gsl_vector *eastingVal = gsl_vector_alloc(gcps->size()); // Vector to hold easting values
        gsl_vector *northingVal = gsl_vector_alloc(gcps->size()); // Vector to hold northing values
		this->aX = gsl_vector_alloc(coeffSize); // Vector to hold coeffifients of X (Easting)
        this->aY = gsl_vector_alloc(coeffSize); // Vector to hold coeffifients of Y (Northing)
		this->aE = gsl_vector_alloc(coeffSize); // Vector to hold coeffifients of Easting (X)
        this->aN = gsl_vector_alloc(coeffSize); // Vector to hold coeffifients of Northing (Y)
        
        unsigned int pointN = 0;
        unsigned int offset = 0;
		
        std::vector<RSGISGCPImg2MapNode*>::iterator iterGCPs;
		for(iterGCPs = gcps->begin(); iterGCPs != gcps->end(); ++iterGCPs) // Populate matrices using ground control points.
		{
            // Add pixel values into vectors
            gsl_vector_set(pixValX, pointN, (*iterGCPs)->imgX()); // Pixel X
            gsl_vector_set(pixValY, pointN, (*iterGCPs)->imgY()); // Pixel Y
            gsl_vector_set(eastingVal, pointN, (*iterGCPs)->eastings()); // Easting
            gsl_vector_set(northingVal, pointN, (*iterGCPs)->northings()); // Northing

            gsl_matrix_set(eastNorthPow, pointN, 0, 1.);
            gsl_matrix_set(xyPow, pointN, 0, 1.);
            
			for(int j = 1; j < polyOrder; ++j)
			{
                offset = 1 + (3 * (j - 1));
                gsl_matrix_set(eastNorthPow, pointN, offset, pow((*iterGCPs)->eastings(), j));
                gsl_matrix_set(eastNorthPow, pointN, offset+1, pow((*iterGCPs)->northings(), j));
                gsl_matrix_set(eastNorthPow, pointN, offset+2, pow((*iterGCPs)->eastings()*(*iterGCPs)->northings(), j));
                
                gsl_matrix_set(xyPow, pointN, offset, pow((*iterGCPs)->imgX(), j));
                gsl_matrix_set(xyPow, pointN, offset+1, pow((*iterGCPs)->imgY(), j));
                gsl_matrix_set(xyPow, pointN, offset+2, pow((*iterGCPs)->imgX()*(*iterGCPs)->imgY(), j));
			}
            
            offset = 1 + (3 * (this->polyOrder - 1));
            gsl_matrix_set(eastNorthPow, pointN, offset, pow((*iterGCPs)->eastings(), this->polyOrder));
            gsl_matrix_set(eastNorthPow, pointN, offset+1, pow((*iterGCPs)->northings(), this->polyOrder));
            
            gsl_matrix_set(xyPow, pointN, offset, pow((*iterGCPs)->imgX(), this->polyOrder));
            gsl_matrix_set(xyPow, pointN, offset+1, pow((*iterGCPs)->imgY(), this->polyOrder));
            
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
        // Fit for E
		gsl_multifit_linear(xyPow, eastingVal, this->aE, cov, &chisq, workspace);
        // Fit for N
		gsl_multifit_linear(xyPow, northingVal, this->aN, cov, &chisq, workspace);       
        
        std::cout << "Fitted polynomial." << std::endl;
        
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
            
            //std::cout <<  << ", " << pX << ", " << (*iterGCPs)->imgY() << ", " << pY << std::endl;
        }
    
        double sqMean = sqSum / double(gcps->size());
		
		double rmse = sqrt(sqMean);
		
		std::cout << "RMSE = " << rmse << " pixels " << std::endl;
        
        // Tidy up
		gsl_multifit_linear_free(workspace);
		gsl_matrix_free(eastNorthPow);
        gsl_matrix_free(xyPow);
		gsl_vector_free(pixValX);
        gsl_vector_free(pixValY);
        gsl_vector_free(eastingVal);
        gsl_vector_free(northingVal);
		gsl_matrix_free(cov);

	}
	
	geos::geom::Envelope* RSGISPolynomialImageWarp::newImageExtent(unsigned int width, unsigned int height) throw(RSGISImageWarpException)
	{
		
        double minEastings = 0;
		double maxEastings = 0;
		double minNorthings = 0;
		double maxNorthings = 0;
        
        minEastings = gsl_vector_get(this->aE, 0);
        minNorthings = gsl_vector_get(this->aN, 0);
        
        maxEastings = gsl_vector_get(this->aE, 0);
        maxNorthings = gsl_vector_get(this->aN, 0);
        
        unsigned int offset = 0;
        
        for(int j = 1; j < this->polyOrder; ++j)
        {
            offset = 1 + (3 * (j - 1));
            
            maxEastings = maxEastings + (gsl_vector_get(this->aE, offset) * pow(float(width), j));
            maxEastings = maxEastings + (gsl_vector_get(this->aE, offset+1) * pow(float(height), j));
            maxEastings = maxEastings + (gsl_vector_get(this->aE, offset+2) * pow(float(width)*float(height), j));
            
            maxNorthings = maxNorthings + (gsl_vector_get(this->aN, offset) * pow(float(width), j));
            maxNorthings = maxNorthings + (gsl_vector_get(this->aN, offset+1) * pow(float(height), j));
            maxNorthings = maxNorthings + (gsl_vector_get(this->aN, offset+2) * pow(float(width)*float(height), j));
        }
        
        offset = 1 + (3 * (this->polyOrder - 1));
        maxEastings = maxEastings + (gsl_vector_get(this->aE, offset) * pow(float(width), this->polyOrder));
        maxEastings = maxEastings + (gsl_vector_get(this->aE, offset+1) * pow(float(height), this->polyOrder));
        
        maxNorthings = maxNorthings + (gsl_vector_get(this->aN, offset) * pow(float(width), this->polyOrder));
        maxNorthings = maxNorthings + (gsl_vector_get(this->aN, offset+1) * pow(float(height), this->polyOrder));
		
        if(maxNorthings < minNorthings) // Swap min and max northings for southern hemisphere lat / long.
        {
            double tempNorthing = maxNorthings;
            maxNorthings = minNorthings;
            minNorthings = tempNorthing;
        }
		
        std::cout << "Eastings: [" << minEastings << "," << maxEastings << "]\n";
		std::cout << "Northings: [" << minNorthings << "," << maxNorthings << "]\n";
        
		geos::geom::Envelope *env = new geos::geom::Envelope(minEastings, maxEastings, minNorthings, maxNorthings);
		
		double geoWidth = maxEastings - minEastings;
		double geoHeight = maxNorthings - minNorthings;
        
        //std::cout << "geoWidth = " << geoWidth << ", geoHeight = " << geoHeight << std::endl;
		
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
    
		//*x = floor(pX+0.5);
		//*y = floor(pY+0.5);
        *x = ceil(pX);
		*y = ceil(pY);
	}
		
	RSGISPolynomialImageWarp::~RSGISPolynomialImageWarp()
	{
        gsl_vector_free(this->aX);
        gsl_vector_free(this->aY);
        gsl_vector_free(this->aE);
        gsl_vector_free(this->aN);
	}
	
}}




