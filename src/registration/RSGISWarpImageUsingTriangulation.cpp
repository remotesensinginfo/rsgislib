/*
 *  RSGISWarpImageUsingTriangulation.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/09/2010.
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

#include "RSGISWarpImageUsingTriangulation.h"

namespace rsgis{namespace reg{

	RSGISWarpImageUsingTriangulation::RSGISWarpImageUsingTriangulation(std::string inputImage, std::string outputImage, std::string outProjWKT, std::string gcpFilePath, float outImgRes, RSGISWarpImageInterpolator *interpolator, std::string gdalFormat) : RSGISWarpImage(inputImage, outputImage, outProjWKT, gcpFilePath, outImgRes, interpolator, gdalFormat), dt(NULL), values(NULL)
	{
        
	}
	
	void RSGISWarpImageUsingTriangulation::initWarp()throw(RSGISImageWarpException)
	{
        std::cout << "Building Triangulation\n";
        
        try
        {
            dt = new DelaunayTriangulation();
            values = new PointValueMap();
            
            std::vector<RSGISGCPImg2MapNode*>::iterator iterGCPs;
            for(iterGCPs = gcps->begin(); iterGCPs != gcps->end(); ++iterGCPs)
            {
                K::Point_2 cgalPt((*iterGCPs)->eastings(),(*iterGCPs)->northings());
                dt->insert(cgalPt);
                
                values->insert(std::make_pair(cgalPt, (*iterGCPs)));
            }
        }
        catch(RSGISImageWarpException &e)
        {
            throw e;
        }
        
	}
	
	geos::geom::Envelope* RSGISWarpImageUsingTriangulation::newImageExtent(unsigned int width, unsigned int height) throw(RSGISImageWarpException)
	{
		double minEastings = 0;
		double maxEastings = 0;
		double minNorthings = 0;
		double maxNorthings = 0;
		bool first = true;
		
		std::vector<RSGISGCPImg2MapNode*>::iterator iterGCPs;
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
		
		std::cout << "Eastings: [" << minEastings << "," << maxEastings << "]\n";
		std::cout << "Northings: [" << minNorthings << "," << maxNorthings << "]\n";
		
		geos::geom::Envelope *env = new geos::geom::Envelope(minEastings, maxEastings, minNorthings, maxNorthings);
		
		double geoWidth = maxEastings - minEastings;
		double geoHeight = maxNorthings - minNorthings;
		
		if((geoWidth <= 0) | (geoHeight <= 0))
		{
			throw RSGISImageWarpException("Either the output image width or height are <= 0.");
		}
		
		return env;
	}
	
	void RSGISWarpImageUsingTriangulation::findNearestPixel(double eastings, double northings, unsigned int *x, unsigned int *y, float inImgRes) throw(RSGISImageWarpException)
	{
		CGALPoint p(eastings, northings);
        Vertex_handle vh = dt->nearest_vertex(p);
        CGALPoint nearestPt = vh->point();
        PointValueMap::iterator iterVal;
        Face_handle fh = dt->locate(nearestPt);
        Vertex_handle pt1Vh = fh->vertex(0);
        iterVal = values->find(pt1Vh->point());
        RSGISGCPImg2MapNode *nodeA = (*iterVal).second;
        Vertex_handle pt2Vh = fh->vertex(1);
        iterVal = values->find(pt2Vh->point());
        RSGISGCPImg2MapNode *nodeB = (*iterVal).second;
        Vertex_handle pt3Vh = fh->vertex(2);
        iterVal = values->find(pt3Vh->point());
        RSGISGCPImg2MapNode *nodeC = (*iterVal).second;
		
        /*
		std::cout << "GCP 1: [" << nodeA->imgX() << "," << nodeA->imgY() << "]\n";
		std::cout << "GCP 2: [" << nodeB->imgX() << "," << nodeB->imgY() << "]\n";
		std::cout << "GCP 3: [" << nodeC->imgX() << "," << nodeC->imgY() << "]\n\n";
		*/
        
		// Create node list
        std::list<const RSGISGCPImg2MapNode*> *triPts = new std::list<const RSGISGCPImg2MapNode*>();
		triPts->push_back(nodeA);
		triPts->push_back(nodeB);
		triPts->push_back(nodeC);
		
        std::list<RSGISGCPImg2MapNode*> *normTriPts = normGCPs(triPts, eastings, northings);
		
		double planeA = 0;
		double planeB = 0;
		double planeC = 0;
		
		this->fitPlane2XPoints(normTriPts, &planeA, &planeB, &planeC);
		*x = floor(planeC+0.5);
		this->fitPlane2YPoints(normTriPts, &planeA, &planeB, &planeC);
		*y = floor(planeC+0.5);
		
        std::list<RSGISGCPImg2MapNode*>::iterator iterGCPs;
		for(iterGCPs = normTriPts->begin(); iterGCPs != normTriPts->end(); )
		{
			delete *iterGCPs;
			normTriPts->erase(iterGCPs++);
		}
		delete normTriPts;
    }
	
	std::list<RSGISGCPImg2MapNode*>* RSGISWarpImageUsingTriangulation::normGCPs(std::list<const RSGISGCPImg2MapNode*> *gcps, double eastings, double northings)
	{
        std::list<RSGISGCPImg2MapNode*> *normTriPts = new std::list<RSGISGCPImg2MapNode*>();
		
		RSGISGCPImg2MapNode *tmpGCP = NULL;
		
        std::list<const RSGISGCPImg2MapNode*>::iterator iterGCPs;
		for(iterGCPs = gcps->begin(); iterGCPs != gcps->end(); ++iterGCPs)
		{
			tmpGCP = new RSGISGCPImg2MapNode(((*iterGCPs)->eastings() - eastings),
											 ((*iterGCPs)->northings() - northings),
											 (*iterGCPs)->imgX(),
											 (*iterGCPs)->imgY());
			normTriPts->push_back(tmpGCP);
		}
		
		return normTriPts;
	}
	
	void RSGISWarpImageUsingTriangulation::fitPlane2XPoints(std::list<RSGISGCPImg2MapNode*> *normPts, double *a, double *b, double *c) throw(RSGISImageWarpException)
	{
		rsgis::math::RSGISMatrices matrices;
		
		try
		{
			double sXY = 0;
			double sX = 0;
			double sXSqu = 0;
			double sY = 0;
			double sYSqu = 0;
			double sXZ = 0;
			double sYZ = 0;
			double sZ = 0;
			
            std::list<RSGISGCPImg2MapNode*>::iterator iterPts;
			
			for(iterPts = normPts->begin(); iterPts != normPts->end(); ++iterPts)
			{
				sXY += ((*iterPts)->eastings() * (*iterPts)->northings());
				sX += (*iterPts)->eastings();
				sXSqu += ((*iterPts)->eastings() * (*iterPts)->eastings());
				sY += (*iterPts)->northings();
				sYSqu += ((*iterPts)->northings() * (*iterPts)->northings());
				sXZ += ((*iterPts)->eastings() * (*iterPts)->imgX());
				sYZ += ((*iterPts)->northings() * (*iterPts)->imgX());
				sZ += (*iterPts)->imgX();
			}
			
			rsgis::math::Matrix *matrixA = matrices.createMatrix(3, 3);
			matrixA->matrix[0] = sXSqu;
			matrixA->matrix[1] = sXY;
			matrixA->matrix[2] = sX;
			matrixA->matrix[3] = sXY;
			matrixA->matrix[4] = sYSqu;
			matrixA->matrix[5] = sY;
			matrixA->matrix[6] = sX;
			matrixA->matrix[7] = sY;
			matrixA->matrix[8] = normPts->size();
			rsgis::math::Matrix *matrixB = matrices.createMatrix(1, 3);
			matrixB->matrix[0] = sXZ;
			matrixB->matrix[1] = sYZ;
			matrixB->matrix[2] = sZ;
			
			double determinantA = matrices.determinant(matrixA);
			rsgis::math::Matrix *matrixCoFactors = matrices.cofactors(matrixA);
			rsgis::math::Matrix *matrixCoFactorsT = matrices.transpose(matrixCoFactors);
			double multiplier = 1/determinantA;
			matrices.multipleSingle(matrixCoFactorsT, multiplier);
			rsgis::math::Matrix *outputs = matrices.multiplication(matrixCoFactorsT, matrixB);
			*a = outputs->matrix[0];
			*b = outputs->matrix[1];
			*c = outputs->matrix[2];
			
			matrices.freeMatrix(matrixA);
			matrices.freeMatrix(matrixB);
			matrices.freeMatrix(matrixCoFactors);
			matrices.freeMatrix(matrixCoFactorsT);
			matrices.freeMatrix(outputs);
		}
		catch(rsgis::math::RSGISMatricesException e)
		{
			throw RSGISImageWarpException(e.what());
		}
	}
	
	void RSGISWarpImageUsingTriangulation::fitPlane2YPoints(std::list<RSGISGCPImg2MapNode*> *normPts, double *a, double *b, double *c) throw(RSGISImageWarpException)
	{
		rsgis::math::RSGISMatrices matrices;
		
		try
		{
			double sXY = 0;
			double sX = 0;
			double sXSqu = 0;
			double sY = 0;
			double sYSqu = 0;
			double sXZ = 0;
			double sYZ = 0;
			double sZ = 0;
			
            std::list<RSGISGCPImg2MapNode*>::iterator iterPts;
			
			for(iterPts = normPts->begin(); iterPts != normPts->end(); ++iterPts)
			{
				sXY += ((*iterPts)->eastings() * (*iterPts)->northings());
				sX += (*iterPts)->eastings();
				sXSqu += ((*iterPts)->eastings() * (*iterPts)->eastings());
				sY += (*iterPts)->northings();
				sYSqu += ((*iterPts)->northings() * (*iterPts)->northings());
				sXZ += ((*iterPts)->eastings() * (*iterPts)->imgY());
				sYZ += ((*iterPts)->northings() * (*iterPts)->imgY());
				sZ += (*iterPts)->imgY();
			}
			
			rsgis::math::Matrix *matrixA = matrices.createMatrix(3, 3);
			matrixA->matrix[0] = sXSqu;
			matrixA->matrix[1] = sXY;
			matrixA->matrix[2] = sX;
			matrixA->matrix[3] = sXY;
			matrixA->matrix[4] = sYSqu;
			matrixA->matrix[5] = sY;
			matrixA->matrix[6] = sX;
			matrixA->matrix[7] = sY;
			matrixA->matrix[8] = normPts->size();
			rsgis::math::Matrix *matrixB = matrices.createMatrix(1, 3);
			matrixB->matrix[0] = sXZ;
			matrixB->matrix[1] = sYZ;
			matrixB->matrix[2] = sZ;
			
			double determinantA = matrices.determinant(matrixA);
			rsgis::math::Matrix *matrixCoFactors = matrices.cofactors(matrixA);
			rsgis::math::Matrix *matrixCoFactorsT = matrices.transpose(matrixCoFactors);
			double multiplier = 1/determinantA;
			matrices.multipleSingle(matrixCoFactorsT, multiplier);
			rsgis::math::Matrix *outputs = matrices.multiplication(matrixCoFactorsT, matrixB);
			*a = outputs->matrix[0];
			*b = outputs->matrix[1];
			*c = outputs->matrix[2];
			
			matrices.freeMatrix(matrixA);
			matrices.freeMatrix(matrixB);
			matrices.freeMatrix(matrixCoFactors);
			matrices.freeMatrix(matrixCoFactorsT);
			matrices.freeMatrix(outputs);
		}
		catch(rsgis::math::RSGISMatricesException e)
		{
			throw RSGISImageWarpException(e.what());
		}
	}
	
	RSGISWarpImageUsingTriangulation::~RSGISWarpImageUsingTriangulation()
	{
		if(dt != NULL)
		{
			delete dt;
		}
        
        if(values != NULL)
        {
            delete values;
        }
	}
	
}}



