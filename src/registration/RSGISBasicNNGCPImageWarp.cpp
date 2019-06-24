/*
 *  RSGISBasicNNGCPImageWarp.cpp
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

#include "RSGISBasicNNGCPImageWarp.h"


namespace rsgis{namespace reg{
	
	
	RSGISBasicNNGCPImageWarp::RSGISBasicNNGCPImageWarp(std::string inputImage, std::string outputImage, std::string outProjWKT, std::string gcpFilePath, float outImgRes, RSGISWarpImageInterpolator *interpolator, std::string gdalFormat) : RSGISWarpImage(inputImage, outputImage, outProjWKT, gcpFilePath, outImgRes, interpolator, gdalFormat), pointIndex(NULL)
	{
		
	}
	
	void RSGISBasicNNGCPImageWarp::initWarp()
	{
		// Create Index
		std::cout << "Creating Index\n";
		this->pointIndex = new geos::index::quadtree::Quadtree();
		geos::geom::Envelope *env = NULL;
		std::vector<RSGISGCPImg2MapNode*>::iterator iterGCPs;
		for(iterGCPs = gcps->begin(); iterGCPs != gcps->end(); ++iterGCPs)
		{
			env = new geos::geom::Envelope(((*iterGCPs)->eastings() - 2), ((*iterGCPs)->eastings() + 2), ((*iterGCPs)->northings() - 2), ((*iterGCPs)->northings() + 2));
			pointIndex->insert(env, *iterGCPs);
		}
	}
	
	geos::geom::Envelope* RSGISBasicNNGCPImageWarp::newImageExtent(unsigned int width, unsigned int height)
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
	
	void RSGISBasicNNGCPImageWarp::findNearestPixel(double eastings, double northings, unsigned int *x, unsigned int *y, float inImgRes)
	{
		RSGISGCPImg2MapNode *pxl = new RSGISGCPImg2MapNode(eastings, northings, 0, 0);
		
		geos::geom::Envelope *searchEnv = new geos::geom::Envelope((eastings-(20*inImgRes)), (eastings+(20*inImgRes)), (northings-(20*inImgRes)), (northings+(20*inImgRes)));
		std::vector<void*> values = std::vector<void*>();
		
		this->pointIndex->query(searchEnv, values);
		
        if(values.size() > 0)
        {
		
            RSGISGCPImg2MapNode *tmpGCP = NULL;
            bool first = true;
            double distance = 0;
            RSGISGCPImg2MapNode *closestGCP = NULL;
            double closestDist = 0;
            std::vector<void*>::iterator iterVals;
            for(iterVals = values.begin(); iterVals != values.end(); ++iterVals)
            {
                tmpGCP = (RSGISGCPImg2MapNode*)(*iterVals);
                distance = tmpGCP->distanceGeo(pxl);
                if(first)
                {
                    closestGCP = tmpGCP;
                    closestDist = distance;
                    first = false;
                }
                else if(distance < closestDist)
                {
                    closestGCP = tmpGCP;
                    closestDist = distance;
                }			
            }
            
            double xDistance = closestGCP->eastings() - eastings;
            double yDistance = closestGCP->northings() - northings;
            
            double pxlDistX = xDistance/inImgRes;
            double pxlDistY = yDistance/inImgRes;
            
            *x = floor((closestGCP->imgX()-pxlDistX)+0.5);
            *y = floor((closestGCP->imgY()+pxlDistY)+0.5);
        }
        else 
        {
            throw RSGISImageWarpException("Tie point could not be founded within search radius.");
        }
		
		delete pxl;
		delete searchEnv;
		
	}
		
	RSGISBasicNNGCPImageWarp::~RSGISBasicNNGCPImageWarp()
	{
		if(pointIndex != NULL)
		{
			delete pointIndex;
		}
	}
	
}}




