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
	
	
	RSGISBasicNNGCPImageWarp::RSGISBasicNNGCPImageWarp(string inputImage, string outputImage, string outProjWKT, string gcpFilePath, float outImgRes, RSGISWarpImageInterpolator *interpolator, string gdalFormat) : RSGISWarpImage(inputImage, outputImage, outProjWKT, gcpFilePath, outImgRes, interpolator, gdalFormat), pointIndex(NULL)
	{
		
	}
	
	void RSGISBasicNNGCPImageWarp::initWarp()throw(RSGISImageWarpException)
	{
		// Create Index
		cout << "Creating Index\n";
		this->pointIndex = new Quadtree();
		Envelope *env = NULL;
		vector<RSGISGCPImg2MapNode*>::iterator iterGCPs;
		for(iterGCPs = gcps->begin(); iterGCPs != gcps->end(); ++iterGCPs)
		{
			env = new Envelope(((*iterGCPs)->eastings() - 2), ((*iterGCPs)->eastings() + 2), ((*iterGCPs)->northings() - 2), ((*iterGCPs)->northings() + 2));
			pointIndex->insert(env, *iterGCPs);
		}
	}
	
	Envelope* RSGISBasicNNGCPImageWarp::newImageExtent(unsigned int width, unsigned int height) throw(RSGISImageWarpException)
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
	
	void RSGISBasicNNGCPImageWarp::findNearestPixel(double eastings, double northings, unsigned int *x, unsigned int *y, float inImgRes) throw(RSGISImageWarpException)
	{
		RSGISGCPImg2MapNode *pxl = new RSGISGCPImg2MapNode(eastings, northings, 0, 0);
		
		Envelope *searchEnv = new Envelope((eastings-(10*inImgRes)), (eastings+(10*inImgRes)), (northings-(10*inImgRes)), (northings+(10*inImgRes)));
		vector<void*> values = vector<void*>();
		
		this->pointIndex->query(searchEnv, values);
		
		
		RSGISGCPImg2MapNode *tmpGCP = NULL;
		bool first = true;
		double distance = 0;
		RSGISGCPImg2MapNode *closestGCP = NULL;
		double closestDist = 0;
		vector<void*>::iterator iterVals;
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
		
		//cout.precision(12);
		//cout << "Pixel : [" << eastings << "," << northings << "]\n";
		//cout << "Closest: [" << closestGCP->eastings << "," << closestGCP->northings << "] = [" << closestGCP->imgX << "," << closestGCP->imgY << "]\n";
		
		double xDistance = closestGCP->eastings() - eastings;
		double yDistance = closestGCP->northings() - northings;
		
		double pxlDistX = xDistance/inImgRes;
		double pxlDistY = yDistance/inImgRes;
		
		*x = floor((closestGCP->imgX()-pxlDistX)+0.5);
		*y = floor((closestGCP->imgY()+pxlDistY)+0.5);
		
		//*x = floor((closestGCP->imgX)+0.5);
		//*y = floor((closestGCP->imgY)+0.5);
		
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




