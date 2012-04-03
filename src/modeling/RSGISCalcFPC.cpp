/*
 *  RSGISCalcFPC.cpp
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


#include "RSGISCalcFPC.h"

namespace rsgis { namespace modeling {
	RSGISCalcFPC::RSGISCalcFPC(RSGISTransect *transect)
	{
		this->transect = transect;
	}
	double RSGISCalcFPC::calcFPCGroundRand(unsigned int nMeas, double randSeed)
	{
		
		/* SET UP THE RANDOM NUMBER GENERATOR
		 * The GSL libary is used for the random number generation (see http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Generation.html)
		 * The seed is passed in as input parameter.
		 */
		gsl_rng *rand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (rand, randSeed);
		
		unsigned int numX = transect->getWidth();
		unsigned int numY = transect->getLenth();
		unsigned int numZ = transect->getHeight();
		unsigned int branchHits = 0;
		unsigned int leafHits = 0;
		unsigned int startX = 0;
		unsigned int startY = 0;
		
		for(unsigned int i = 0; i < nMeas; i++)
		{
			startX = gsl_rng_uniform_int (rand, numX);
			startY = gsl_rng_uniform_int (rand, numY);
			
			for(unsigned int z = 0; z < numZ; z++) // Continue untill something is hit
			{
				char element = transect->getValue(startX, startY, z);
				
				if(element == 1)
				{
					branchHits++;
					break;
				}
				else if(element == 2)
				{
					leafHits++;
					break;
				}
			}
		}	
		
		double percentLeafHits = double(leafHits) / double(nMeas);
		double percentBranchHits = double(branchHits) / double(nMeas);
		
		double fpc = (100 * percentLeafHits) / (1 - percentBranchHits);
		
		return fpc;
	}
	
	double RSGISCalcFPC::calcFPCGroundTrans(unsigned int spaceing)
	{
		// Get transect dimensions
		unsigned int numX = transect->getWidth();
		unsigned int numY = transect->getLenth();
		unsigned int numZ = transect->getHeight();
		unsigned int branchHits = 0;
		unsigned int leafHits = 0;
		
		unsigned int startX = numX / 2; // Set start x to middle of transect
		unsigned int startY = 0;
		
		// Calculate number of measurements based on transect lenght and point spaceing
		int nMeas = numY / spaceing;
		
		while (startY < numY)
		{
			for(unsigned int z = 0; z < numZ; z++) // Continue untill something is hit
			{
				char element = transect->getValue(startX, startY, z);
				
				if((element == 1) | (element == 3))
				{
					branchHits++;
					break;
				}
				else if(element == 2)
				{
					leafHits++;
					break;
				}
			}
			startY = startY + spaceing;
		}	
		
		// Calculate FPC based on hits
		double percentLeafHits = double(leafHits) / double(nMeas);
		double percentBranchHits = double(branchHits) / double(nMeas);
		
		double fpc = (100 * percentLeafHits) / (1 - percentBranchHits);
		
		return fpc;
	}
	
	double RSGISCalcFPC::calcFPCGroundAll()
	{
		// Get transect dimensions
		unsigned int numX = transect->getWidth();
		unsigned int numY = transect->getLenth();
		unsigned int numZ = transect->getHeight();
		unsigned int branchHits = 0;
		unsigned int leafHits = 0;
		
		unsigned int nMeas = numX * numY;
		
		
		for (unsigned int x = 0; x < numX; x++)
		{
			for (unsigned int y = 0; y < numY; y++)
			{
				for(unsigned int z = 0; z < numZ; z++) // Continue untill something is hit
				{
					char element = transect->getValue(x, y, z);
					
					if(element == 1)
					{
						branchHits++;
						break;
					}
					else if(element == 2)
					{
						leafHits++;
						break;
					}
				}
			}	
		}
		
		// Calculate FPC based on hits
		double percentLeafHits = double(leafHits) / double(nMeas);
		double percentBranchHits = double(branchHits) / double(nMeas);
		
		double fpc = (100 * percentLeafHits) / (1 - percentBranchHits);
		
		return fpc;
	}
	
	
	double RSGISCalcFPC::calcFPCHeightRand(unsigned int nMeas, double randSeed)
	{
		
		/* SET UP THE RANDOM NUMBER GENERATOR
		 * The GSL libary is used for the random number generation (see http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Generation.html)
		 * The seed is passed in as input parameter.
		 */
		gsl_rng *rand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (rand, randSeed);
		
		unsigned int numX = transect->getWidth();
		unsigned int numY = transect->getLenth();
		unsigned int numZ = transect->getHeight();
		unsigned int branchHits = 0;
		unsigned int leafHits = 0;
		unsigned int startX = 0;
		unsigned int startY = 0;
		
		for(unsigned int i = 0; i < nMeas; i++)
		{
			startX = gsl_rng_uniform_int (rand, numX);
			startY = gsl_rng_uniform_int (rand, numY);
			
			for(unsigned int z = 0; z < numZ; z++) // Continue untill something is hit
			{
				char element = transect->getValue(startX, startY, z);
				
				if(element == 1)
				{
					branchHits++;
					break;
				}
				else if(element == 2)
				{
					leafHits++;
					break;
				}
			}
		}	
		
		double percentLeafHits = double(leafHits) / double(nMeas);
		double percentBranchHits = double(branchHits) / double(nMeas);
		
		double fpc = (100 * percentLeafHits) / (1 - percentBranchHits);
		
		return fpc;
	}
	
	double RSGISCalcFPC::calcFPCHeightTrans(unsigned int spaceing)
	{
		// Get transect dimensions
		unsigned int numX = transect->getWidth();
		unsigned int numY = transect->getLenth();
		unsigned int numZ = transect->getHeight();
		unsigned int branchHits = 0;
		unsigned int leafHits = 0;
		
		unsigned int startX = numX / 2; // Set start x to middle of transect
		unsigned int startY = 0;
		
		// Calculate number of measurements based on transect lenght and point spaceing
		int nMeas = numY / spaceing;
		
		while (startY < numY)
		{
			for(unsigned int z = 0; z < numZ; z++) // Continue untill something is hit
			{
				char element = transect->getValue(startX, startY, z);
				
				if(element == 1)
				{
					branchHits++;
					break;
				}
				else if(element == 2)
				{
					leafHits++;
					break;
				}
			}
			startY = startY + spaceing;
		}	
		
		// Calculate FPC based on hits
		double percentLeafHits = double(leafHits) / double(nMeas);
		double percentBranchHits = double(branchHits) / double(nMeas);
		
		double fpc = (100 * percentLeafHits) / (1 - percentBranchHits);
		
		return fpc;
	}
	
	double RSGISCalcFPC::calcFPCTopRand(unsigned int nMeas, double randSeed)
	{
		
		/* SET UP THE RANDOM NUMBER GENERATOR
		 * The GSL libary is used for the random number generation (see http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Generation.html)
		 * The seed is passed in as input parameter.
		 */
		gsl_rng *rand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (rand, randSeed);
		
		unsigned int numX = transect->getWidth();
		unsigned int numY = transect->getLenth();
		unsigned int numZ = transect->getHeight();
		unsigned int leafHits = 0;
		unsigned int startX = 0;
		unsigned int startY = 0;
		
		for(unsigned int i = 0; i < nMeas; i++)
		{
			startX = gsl_rng_uniform_int (rand, numX);
			startY = gsl_rng_uniform_int (rand, numY);
			
			for(unsigned int z = (numZ - 1); z > 0; z--) // Start from the top and work down until something is hit.
			{
				char element = transect->getValue(startX, startY, z);
				
				if(element == 2)
				{
					leafHits++;
					break;
				}
			}
		}	
		
		// Calculate FPC based on hits		
		double fpc = 100 * (double(leafHits) / double(nMeas));
		
		return fpc;
		
		return fpc;
	}
	
	double RSGISCalcFPC::calcFPCTopTrans(unsigned int spaceing)
	{
		// Get transect dimensions
		unsigned int numX = transect->getWidth();
		unsigned int numY = transect->getLenth();
		unsigned int numZ = transect->getHeight();
		unsigned int leafHits = 0;
		
		unsigned int startX = numX / 2; // Set start x to middle of transect
		unsigned int startY = 0;
		
		// Calculate number of measurements based on transect lenght and point spaceing
		int nMeas = numY / spaceing;
		
		while (startY < numY)
		{
			for(unsigned int z = (numZ - 1); z > 0; z--) // Start from the top and work down until something is hit.
			{
				char element = transect->getValue(startX, startY, z);
				
				if(element == 2)
				{
					leafHits++;
					break;
				}
			}
			startY = startY + spaceing;
		}	
		
		// Calculate FPC based on hits		
		double fpc = 100 * (double(leafHits) / double(nMeas));
		
		return fpc;
	}
	
}}

