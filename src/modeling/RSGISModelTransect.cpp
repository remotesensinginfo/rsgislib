/*
 *  RSGISModelTransect.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 28/10/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
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

#include "RSGISModelTransect.h"

namespace rsgis{ namespace modeling  {
	RSGISModelTransect::RSGISModelTransect(RSGISTransect *transect)
	{
		this->transect = transect;
		this->transectLenght = transect->getLenth();
		this->transectWidth = transect->getWidth();
		this->transectHeight = transect->getHeight();
		this->transectRes = transect->getRes();
		
		// TO DO: Add check to ensure array will fit into memory.
	}
	
	void RSGISModelTransect::createConvexHull(std::vector<geos::geom::Polygon*> *canopyPoly)
	{
		this->canopyPoly = canopyPoly;
		this->convexHull = true;
	}
	
	void RSGISModelTransect::populateSingleSizeSpecies(double quadratSize, unsigned int numTrees, rsgis::math::RSGISProbDistro *vegDistro, rsgis::math::RSGISProbDistro *treePosXDistro, rsgis::math::RSGISProbDistro *treePosYDistro, RSGISModelAddVegetation *addVeg)
	{		
		
		// Populate transect with trees
		unsigned int quadratSizeVox = int(quadratSize / transectRes);
		unsigned int NQuadratWidth = int(transectWidth / quadratSizeVox);
		unsigned int NQuadratLenght = int(transectLenght / quadratSizeVox);
		unsigned int NQuadrat = NQuadratWidth * NQuadratLenght;
		
		// Calcualte the number of trees per quadrat
		double vegProbTotal = 0;
		double *vegProb = new double[NQuadrat];
		unsigned int *nTreesQuadrat = new unsigned int[NQuadrat];

		for(unsigned int i = 0; i < NQuadrat; i++)
		{
			vegProb[i] = vegDistro->calcRand();
			vegProbTotal = vegProbTotal + vegProb[i];
		}
		
		unsigned int totalTrees = 0;
		for(unsigned int i = 0; i < NQuadrat; i++)
		{
			nTreesQuadrat[i] = int(((vegProb[i] / vegProbTotal) * numTrees)+0.5);
			totalTrees = totalTrees + nTreesQuadrat[i];
		}

		unsigned int quadratStart = 0;
		unsigned int treesAdded = 0;
		
		for(unsigned int j = 0; j < NQuadratLenght; j++) // Loop through quadrants (moving along plot)
		{
			for(unsigned int i = 0; i < NQuadratWidth; i++) // Loop through quadrants (moving across plot)
			{
				for(unsigned int t = 0; t < nTreesQuadrat[i]; t++) // Loop through trees in quadrat i
				{
					unsigned int subQuadSize = (transectLenght / nTreesQuadrat[i]);
					
					/* Calculate the center location of the vegetation based on the current quadrat and
					 * an offset, taken from a random distrobution
					 */
					int xOffset = treePosXDistro->calcRand();
					int yOffset = treePosYDistro->calcRand();
					
					//cout << "xOffset = " << xOffset << endl;
					
					//unsigned int centerX = (t * subQuadSize) + (subQuadSize / 2) + xOffset;
					//unsigned int centerY = (i * quadratSizeVox) + (quadratSizeVox / 2) + yOffset;
					
					unsigned int centerX = (i * quadratSizeVox) + (quadratSizeVox / 2) + xOffset;
					unsigned int centerY = (j * quadratSizeVox) + ((subQuadSize * t) / 2) + yOffset;
					
					//cout << "Tree Position = (" << centerX << ", " << centerY << ")" << endl;
					
					if (centerX >= transectWidth) 
					{
						centerX = (i * quadratSizeVox) + (quadratSizeVox / 2);
					}
					if (centerY >= transectLenght) 
					{
						centerY = (j * quadratSizeVox) + (quadratSizeVox / 2);
					}
					
					// Add vegetation to transect
					if (this->convexHull == true) // Add to 'transect' and save convex hull to 'canopyPoly' 
					{
						addVeg->addVegTransConvexHull(transect, centerX, centerY, subQuadSize, quadratSizeVox, canopyPoly);
					}
					else
					{
						addVeg->addVegTrans(transect, centerX, centerY, subQuadSize, quadratSizeVox);
					}
					
					treesAdded++;
					
					if(treesAdded >= numTrees) // Stop if attempting to add to many trees.
					{
						break;
					}
				}
				
				quadratStart = quadratStart + quadratSizeVox; // Move to next quadrat
				
				if(treesAdded >= numTrees) // Stop if attempting to add to many trees.
				{
					break;
				}
			}
		}
		
		delete[] vegProb;
		delete[] nTreesQuadrat;
	}
	
	RSGISModelTransect::~RSGISModelTransect(){}
}}

