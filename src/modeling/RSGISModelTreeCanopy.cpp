/*
 *  RSGISModelTreeCanopy.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 10/11/2009.
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

#include "RSGISModelTreeCanopy.h"

namespace rsgis { namespace modeling{
	RSGISModelTreeCanopy::RSGISModelTreeCanopy(std::vector<double> *modelPar, 
											   rsgis::math::RSGISProbDistro *leafPosHDistro, rsgis::math::RSGISProbDistro *leafPosVDistro, rsgis::math::RSGISProbDistro *leafThetaDistro,
											   rsgis::math::RSGISProbDistro *smallBranchPosHDistro, rsgis::math::RSGISProbDistro *smallBranchPosVDistro, rsgis::math::RSGISProbDistro *smallBranchThetaDistro, rsgis::math::RSGISProbDistro *smallBranchPhiDistro, 
											   rsgis::math::RSGISProbDistro *largeBranchPosHDistro, rsgis::math::RSGISProbDistro *largeBranchPosVDistro, rsgis::math::RSGISProbDistro *largeBranchThetaDistro, rsgis::math::RSGISProbDistro *largeBranchPhiDistro)
	{
		
		// Read in model parameters from input vector
		if (modelPar->size() !=10)	
		{
			std::cout << "Not enough input parameters! " << std::endl;
		}
		else 
		{
			this->leafLenght = modelPar->at(0); // Leaf lenght
			this->leafWidth = modelPar->at(1); // Leaf width
			this->leafThickness = modelPar->at(2); // Leaf thickness
			this->leafDensity = modelPar->at(3); // Number of leaves
			this->smallBranchLenght = modelPar->at(4); // Small branch lenght
			this->smallBranchRadius = modelPar->at(5); // Small branch radius
			this->smallBranchDensity = modelPar->at(6); // Number of small branches
			this->largeBranchLenght = modelPar->at(7); // Large branch lenght
			this->largeBranchRadius = modelPar->at(8); // Large branch radius
			this->largeBranchDensity = modelPar->at(9); // Number of large branches
		}
		
		// Read in Probability Distrobutions
		this->leafPosHDistro = leafPosHDistro; // Distrobution of leaf loctions (horizonal)
		this->leafPosVDistro = leafPosVDistro; // Distrobution of leaf loctions (vertical)
		this->leafThetaDistro = leafThetaDistro; // Distrobution of leaf incidence angles
		this->smallBranchPosHDistro = smallBranchPosHDistro; // Distrobution of small branch locations (horizontal)
		this->smallBranchPosVDistro = smallBranchPosVDistro; // Distrobution of small branch locations (vertical)
		this->smallBranchThetaDistro = smallBranchThetaDistro; // Distrobution of small branch indidence angles
		this->smallBranchPhiDistro = smallBranchPhiDistro; // Distribution of small branch angles into the incidence plane
		this->largeBranchPosHDistro = largeBranchPosHDistro; // Distrobution of large branch positions (horizonal)
		this->largeBranchPosVDistro = largeBranchPosVDistro; // Distrobution of large branch positions (vertical)
		this->largeBranchThetaDistro = largeBranchThetaDistro; // Distrobution of large branch incidence angles
		this->largeBranchPhiDistro = largeBranchPhiDistro; // Distrobution of large branch angles into the incidence plane
	}
	void RSGISModelTreeCanopy::addVegTrans(RSGISTransect *transect, unsigned int centerX, unsigned int centerY, unsigned int sizeX, unsigned int sizeY)
	{
		double pi = 3.1415926536;
		
		unsigned int startX = 0;
		unsigned int startY = 0;
		unsigned int startZ = 0;
		unsigned int branchX = 0;
		unsigned int branchY = 0;
		unsigned int branchZ = 0;
		unsigned int rX = 0;
		unsigned int rY = 0;
		unsigned int rZ = 0;
		unsigned int xItt = 0;
		unsigned int yItt = 0;
		unsigned int rItt = 0;
		unsigned int x = 0;
		unsigned int y = 0;
		unsigned int rYLeaf = 0;
		unsigned int rZLeaf = 0;
		unsigned int rLeafItt = 0;
		unsigned int xLeafItt = 0;
		unsigned int xLeaf = 0;
		unsigned int yLeaf = 0;
		unsigned int zLeaf = 0;
		
		double leafTheta = 0;
		double smallBranchTheta = 0;
		double smallBranchPhi = 0;
		double largeBranchTheta = 0;
		double largeBranchPhi = 0;
		
		double transectRes = transect->getRes();
		
		// Convert lenghts for parameters from meters to voxels, based on transect resolution
		unsigned int leafLenghtVox = int(leafLenght / transectRes);
		unsigned int leafWidthVox = int(leafWidth / transectRes);
		unsigned int smallBranchRadiusVox = int(smallBranchRadius / transectRes);
		unsigned int smallBranchLenghtVox = int(smallBranchLenght / transectRes);
		unsigned int largeBranchRadiusVox = int(largeBranchRadius / transectRes);
		unsigned int largeBranchLenghtVox = int(largeBranchLenght / transectRes);
		
		unsigned int smallBranchDensityInt = int(smallBranchDensity + 0.5);
		unsigned int largeBranchDensityInt = int(largeBranchDensity + 0.5);
		unsigned int leafDensityInt = int(leafDensity + 0.5);
		
		unsigned int maxX = transect->getWidth();
		unsigned int maxY = transect->getLenth();
		unsigned int maxZ = transect->getHeight();
		
		/********************************
		 * ADD SMALL BRANCHES TO CANOPY *
		 ********************************/
		
		unsigned int centerZ = int(maxZ / 2); // Vertical start point for canopy at middle of transect
		
		for(unsigned int i = 0; i < smallBranchDensityInt; i++)
		{
			// GENERATE START POINT FOR EACH BRANCH
			
			startX = smallBranchPosHDistro->calcRand() + centerX;
			startY = smallBranchPosHDistro->calcRand() + centerY;
			startZ = smallBranchPosVDistro->calcRand() + centerZ;
			
			// GENERATE ORIENTATION ANGLE FOR EACH BRANCH
			// Branches always grown away from center of the tree
			smallBranchTheta = smallBranchThetaDistro->calcRand();
			if(startX < centerX)
			{
				if (startY < centerY) 
				{
					smallBranchPhi = ((smallBranchPhiDistro->calcRand() + 180)/180)*pi;
				}
				else 
				{
					smallBranchPhi = ((smallBranchPhiDistro->calcRand() + 90)/180)*pi;
				}
			}
			else if(startX > centerX)
			{
				if (startY < centerY) 
				{
					smallBranchPhi = ((smallBranchPhiDistro->calcRand() + 270)/180)*pi;
				}
				else 
				{
					smallBranchPhi = ((smallBranchPhiDistro->calcRand())/180)*pi;
				}
			}
			
			// ADD SECTIONS TO BRANCH
			rX = 0; rY = 0; rZ = 0;
			rItt = 0;
			while(rItt < smallBranchLenghtVox)
			{
				branchX = startX + (rX * cos(smallBranchPhi) * sin(smallBranchTheta) + 0.5);
				branchY = startY + (rY * sin(smallBranchPhi) * sin(smallBranchTheta) + 0.5);
				branchZ = startZ + (rZ * cos(smallBranchTheta) + 0.5);
				
				if (branchX >= maxX)
				{
					branchX = centerX;
					startX = centerX;
					rX = 0;
				}
				if (branchY >= maxY)
				{
					branchY = centerY;
					startY = centerY;
					rY = 0;
				}
				
				// SET START POINT OF BRANCH SECTION
				transect->setValue(branchX, branchY, branchZ, 1);
				
				/* GROW OUT BRANCHES
				 * If the branch radius is less than one voxel this will not run
				 */
				x = branchX; // Initialy set x to branch start , if x > numX, x goes to zero (bondary condition).
				xItt = 0; // xItt advances by on each time until a the branch has been grown to the correct number of voxels
				
				while(xItt < smallBranchRadiusVox)
				{
					y = branchY; // Initialy set y to branch start , if y > numY, y goes to zero (bondary condition).
					yItt = 0; // yItt advances by on each time until a the branch has been grown to the correct number of voxels
					
					while (yItt < smallBranchRadiusVox) 
					{
						transect->setValue(x, y, branchZ , 1);
						
						y++; yItt++; 
					}
					x++; xItt++;
				}
				
				// Add leaves to end of branch.
				if(rItt == (smallBranchLenghtVox - 1))
				{
					for(unsigned int l = 0; l < (leafDensityInt / smallBranchDensityInt);l++)
					{
						unsigned int leafStartX = leafPosHDistro->calcRand() + branchX;
						unsigned int leafStartY = leafPosHDistro->calcRand() + branchY;
						unsigned int leafStartZ = leafPosVDistro->calcRand()+ branchZ;
						
						leafTheta = ((leafThetaDistro->calcRand()) / 180) * pi; // Leaf orientation random between 0 and 90 deg.
						
						/* SET THE START POINT OF LEAF
						 * if the leaf radius is less than 
						 * the resolution, the leaves will not be grown out */
						
						transect->setValue(leafStartX, leafStartY, leafStartZ, 2); 
						
						// GROW LEAVES OUT			
						rLeafItt = 0;
						rYLeaf = 0;
						rZLeaf = 0;
						
						while(rLeafItt < leafLenghtVox)
						{
							xLeafItt = 0;
							xLeaf = leafStartX;
							
							yLeaf = leafStartY + int(rYLeaf * sin(leafTheta) + 0.5);
							zLeaf = leafStartZ + int(rZLeaf * cos(leafTheta) + 0.5);

							while (xLeafItt < leafWidthVox) 
							{
								transect->setValue(xLeaf, yLeaf, zLeaf, 2);
								
								xLeaf++; xLeafItt++;
							}
							rZLeaf++; rYLeaf++; rLeafItt++;
						}
					}
				}
				rX++; rY++; rZ++; rItt++;
			}
		}
		
		/********************************
		 * ADD LARGE BRANCHES TO CANOPY *
		 ********************************/
		
		for(unsigned int i = 0; i < largeBranchDensityInt; i++)
		{
			// GENERATE START POINT FOR EACH BRANCH
			startX = centerX;
			startY = centerY;
			startZ = largeBranchPosVDistro->calcRand();
			
			// GENERATE ORIENTATION ANGLE FOR EACH BRANCH
			largeBranchTheta = (largeBranchThetaDistro->calcRand() / 180) * pi; // Angle in incidence plane varies between 0 - 90
			largeBranchPhi = ((largeBranchPhiDistro->calcRand())/180)*pi;
			
			// ADD SECTIONS TO BRANCH
			rX = 0; rY = 0; rZ = 0;
			rItt = 0;
			while(rItt < largeBranchLenghtVox)
			{
				branchX = startX + (rX * cos(largeBranchPhi) * sin(largeBranchTheta) + 0.5);
				branchY = startY + (rY * sin(largeBranchPhi) * sin(largeBranchTheta) + 0.5);
				branchZ = startZ + (rZ * cos(largeBranchTheta) + 0.5);
				
				// SET START POINT OF BRANCH SECTION
				transect->setValue(branchX, branchY, branchZ, 1); 
				
				/* GROW OUT BRANCHES
				 * If the branch radius is less than one voxel this will not run
				 *
				 */
				x = branchX; // Initialy set x to branch start , if x > numX, x goes to zero (bondary condition).
				xItt = 0; // xItt advances by on each time until a the branch has been grown to the correct number of voxels
				
				while(xItt < largeBranchRadiusVox)
				{
					
					y = branchY; // Initialy set y to branch start , if y > numY, y goes to zero (bondary condition).
					yItt = 0; // yItt advances by on each time until a the branch has been grown to the correct number of voxels
					
					while (yItt < largeBranchRadiusVox) 
					{

						transect->setValue(x, y, branchZ, 1);
						
						y++; yItt++; 
					}
					x++; xItt++;
				}
				
				rX++; rY++; rZ++; rItt++;
			}
			
		}

	}
	void RSGISModelTreeCanopy::addVegTransConvexHull(RSGISTransect *transect, unsigned int centerX, unsigned int centerY, unsigned int sizeX, unsigned int sizeY, std::vector<geos::geom::Polygon*> *canopyPolys)
	{
		double pi = 3.1415926536;
		
		unsigned int startX = 0;
		unsigned int startY = 0;
		unsigned int startZ = 0;
		unsigned int branchX = 0;
		unsigned int branchY = 0;
		unsigned int branchZ = 0;
		unsigned int rX = 0;
		unsigned int rY = 0;
		unsigned int rZ = 0;
		unsigned int xItt = 0;
		unsigned int yItt = 0;
		unsigned int rItt = 0;
		unsigned int x = 0;
		unsigned int y = 0;
		unsigned int rYLeaf = 0;
		unsigned int rZLeaf = 0;
		unsigned int rLeafItt = 0;
		unsigned int xLeafItt = 0;
		unsigned int xLeaf = 0;
		unsigned int yLeaf = 0;
		unsigned int zLeaf = 0;
		
		double leafTheta = 0;
		double smallBranchTheta = 0;
		double smallBranchPhi = 0;
		double largeBranchTheta = 0;
		double largeBranchPhi = 0;
		
		std::vector<geos::geom::Coordinate> *coordinates = new std::vector<geos::geom::Coordinate>;
		
		double transectRes = transect->getRes();
		
		// Convert lenghts for parameters from meters to voxels, based on transect resolution
		unsigned int leafLenghtVox = int(leafLenght / transectRes);
		unsigned int leafWidthVox = int(leafWidth / transectRes);
		unsigned int smallBranchRadiusVox = int(smallBranchRadius / transectRes);
		unsigned int smallBranchLenghtVox = int(smallBranchLenght / transectRes);
		unsigned int largeBranchRadiusVox = int(largeBranchRadius / transectRes);
		unsigned int largeBranchLenghtVox = int(largeBranchLenght / transectRes);
		
		unsigned int smallBranchDensityInt = int(smallBranchDensity + 0.5);
		unsigned int largeBranchDensityInt = int(largeBranchDensity + 0.5);
		unsigned int leafDensityInt = int(leafDensity + 0.5);
		
		unsigned int maxX = transect->getWidth();
		unsigned int maxY = transect->getLenth();
		 		
		if (largeBranchDensityInt < 2) // If less than two large braches distribute branches randomly
		{
			/********************************
			 * ADD LARGE BRANCHES TO CANOPY *
			 ********************************/
			
			for(unsigned int i = 0; i < largeBranchDensityInt; i++)
			{
				// GENERATE START POINT FOR EACH BRANCH
				startX = centerX;
				startY = centerY;
				startZ = 0;
				
				// GENERATE ORIENTATION ANGLE FOR EACH BRANCH
				largeBranchTheta = (largeBranchThetaDistro->calcRand() / 180) * pi; // Angle in incidence plane varies between 0 - 90
				largeBranchPhi = ((largeBranchPhiDistro->calcRand())/180)*pi;
				
				// ADD SECTIONS TO BRANCH
				rX = 0; rY = 0; rZ = 0;
				rItt = 0;
				while(rItt < largeBranchLenghtVox)
				{
					branchX = startX + (rX * cos(largeBranchPhi) * sin(largeBranchTheta) + 0.5);
					branchY = startY + (rY * sin(largeBranchPhi) * sin(largeBranchTheta) + 0.5);
					branchZ = startZ + (rZ * cos(largeBranchTheta) + 0.5);
					
					// SET START POINT OF BRANCH SECTION
					transect->setValue(branchX, branchY, branchZ, 3); 
					
					/* GROW OUT BRANCHES
					 * If the branch radius is less than one voxel this will not run
					 *
					 */
					x = branchX; // Initialy set x to branch start , if x > numX, x goes to zero (bondary condition).
					xItt = 0; // xItt advances by on each time until a the branch has been grown to the correct number of voxels
					
					while(xItt < largeBranchRadiusVox)
					{
						
						y = branchY; // Initialy set y to branch start , if y > numY, y goes to zero (bondary condition).
						yItt = 0; // yItt advances by on each time until a the branch has been grown to the correct number of voxels
						
						while (yItt < largeBranchRadiusVox) 
						{
							
							transect->setValue(x, y, branchZ, 3);
							
							y++; yItt++; 
						}
						x++; xItt++;
					}
					
					rX++; rY++; rZ++; rItt++;
				}
				
			}
			
			/********************************
			 * ADD SMALL BRANCHES TO CANOPY *
			 ********************************/
			
			for(unsigned int i = 0; i < smallBranchDensityInt; i++)
			{
				// GENERATE START POINT FOR EACH BRANCH
				
				startX = centerX + smallBranchPosHDistro->calcRand();
				startY = centerY + smallBranchPosHDistro->calcRand();
				startZ = smallBranchPosVDistro->calcRand();
				
				// GENERATE ORIENTATION ANGLE FOR EACH BRANCH
				// Branches always grown away from center of the tree
				smallBranchTheta = (smallBranchThetaDistro->calcRand() / 180) * pi;
				smallBranchPhi = (smallBranchPhiDistro->calcRand()/180)*pi;
				
				// ADD SECTIONS TO BRANCH
				rX = 0; rY = 0; rZ = 0;
				rItt = 0;
				while(rItt < smallBranchLenghtVox)
				{
					
					branchX = startX + (rX * cos(smallBranchPhi) * sin(smallBranchTheta) + 0.5);
					branchY = startY + (rY * sin(smallBranchPhi) * sin(smallBranchTheta) + 0.5);
					branchZ = startZ + (rZ * cos(smallBranchTheta) + 0.5);
					
					if (branchX >= maxX)
					{
						branchX = centerX;
						startX = centerX;
						rX = 0;
					}
					if (branchY >= maxY)
					{
						branchY = centerY;
						startY = centerY;
						rY = 0;
					}
					
					// SET START POINT OF BRANCH SECTION
					transect->setValue(branchX, branchY, branchZ, 1);
					
					/* GROW OUT BRANCHES
					 * If the branch radius is less than one voxel this will not run
					 */
					x = branchX; // Initialy set x to branch start , if x > numX, x goes to zero (bondary condition).
					xItt = 0; // xItt advances by on each time until a the branch has been grown to the correct number of voxels
					
					while(xItt < smallBranchRadiusVox)
					{
						y = branchY; // Initialy set y to branch start , if y > numY, y goes to zero (bondary condition).
						yItt = 0; // yItt advances by on each time until a the branch has been grown to the correct number of voxels
						
						while (yItt < smallBranchRadiusVox) 
						{
							transect->setValue(x, y, branchZ , 1);
							
							y++; yItt++; 
						}
						x++; xItt++;
					}
					
					// Add leaves to end of branch.
					if(rItt == (smallBranchLenghtVox - 1))
					{
						for(unsigned int l = 0; l < (leafDensityInt / smallBranchDensityInt);l++)
						{
							unsigned int leafStartX = leafPosHDistro->calcRand() + branchX;
							unsigned int leafStartY = leafPosHDistro->calcRand() + branchY;
							unsigned int leafStartZ = leafPosVDistro->calcRand()+ branchZ;
							
							leafTheta = ((leafThetaDistro->calcRand()) / 180) * pi; // Leaf orientation random between 0 and 90 deg.
							
							/* SET THE START POINT OF LEAF
							 * if the leaf radius is less than 
							 * the resolution, the leaves will not be grown out */
							
							transect->setValue(leafStartX, leafStartY, leafStartZ, 2);
							// Save out as geos coordinates
							if (leafStartX > 0 && leafStartX < transect->getWidth()) 
							{
								if (leafStartY > 0 && leafStartY < transect->getLenth()) 
								{
									coordinates->push_back(geos::geom::Coordinate(leafStartX, leafStartY, leafStartZ));
								}
								
							}
							
							// GROW LEAVES OUT			
							rLeafItt = 0;
							rYLeaf = 0;
							rZLeaf = 0;
							
							while(rLeafItt < leafLenghtVox)
							{
								xLeafItt = 0;
								xLeaf = leafStartX;
								
								yLeaf = leafStartY + int(rYLeaf * sin(leafTheta) + 0.5);
								zLeaf = leafStartZ + int(rZLeaf * cos(leafTheta) + 0.5);
								
								while (xLeafItt < leafWidthVox) 
								{
									transect->setValue(xLeaf, yLeaf, zLeaf, 2);
									if (xLeaf > 0 && xLeaf < transect->getWidth()) 
									{
										if (yLeaf > 0 && yLeaf < transect->getLenth()) 
										{
											coordinates->push_back(geos::geom::Coordinate(xLeaf, yLeaf, zLeaf));
										}
										
									}
									xLeaf++; xLeafItt++;
								}
								rZLeaf++; rYLeaf++; rLeafItt++;
							}
						}
					}
					rX++; rY++; rZ++; rItt++;
				}
			}
			
		}
		else 
		{
			/********************************
			 * ADD LARGE BRANCHES TO CANOPY *
			 ********************************/
			
			for(unsigned int i = 0; i < largeBranchDensityInt; i++)
			{
				// GENERATE START POINT FOR EACH BRANCH
				startX = centerX;
				startY = centerY;
				startZ = 0;//largeBranchPosVDistro->calcRand();
				
				// GENERATE ORIENTATION ANGLE FOR EACH BRANCH
				largeBranchTheta = (largeBranchThetaDistro->calcRand() / 180) * pi; // Angle in incidence plane varies between 0 - 90
				largeBranchPhi = ((largeBranchPhiDistro->calcRand())/180)*pi;
								
				// ADD SECTIONS TO BRANCH
				rX = 0; rY = 0; rZ = 0;
				rItt = 0;
				while(rItt < largeBranchLenghtVox)
				{
					branchX = startX + (rX * cos(largeBranchPhi) * sin(largeBranchTheta) + 0.5);
					branchY = startY + (rY * sin(largeBranchPhi) * sin(largeBranchTheta) + 0.5);
					branchZ = startZ + (rZ * cos(largeBranchTheta) + 0.5);
					
					// SET START POINT OF BRANCH SECTION
					transect->setValue(branchX, branchY, branchZ, 3); 
					
					/* GROW OUT BRANCHES
					 * If the branch radius is less than one voxel this will not run
					 *
					 */
					x = branchX; // Initialy set x to branch start , if x > numX, x goes to zero (bondary condition).
					xItt = 0; // xItt advances by on each time until a the branch has been grown to the correct number of voxels
					
					while(xItt < largeBranchRadiusVox)
					{
						
						y = branchY; // Initialy set y to branch start , if y > numY, y goes to zero (bondary condition).
						yItt = 0; // yItt advances by on each time until a the branch has been grown to the correct number of voxels
						
						while (yItt < largeBranchRadiusVox) 
						{
							
							transect->setValue(x, y, branchZ, 3);
							
							y++; yItt++; 
						}
						x++; xItt++;
					}
					
					rX++; rY++; rZ++; rItt++;
				}
				
				/********************************
				 * ADD SMALL BRANCHES TO CANOPY *
				 ********************************/
				for(unsigned int i = 0; i < smallBranchDensityInt / largeBranchDensityInt; i++)
				{
					// GENERATE START POINT FOR EACH BRANCH
					int smallBranchPos = smallBranchPosVDistro->calcRand();
					startX = centerX + (smallBranchPos * cos(largeBranchPhi) * sin(largeBranchTheta) + 0.5);
					startY = centerY + (smallBranchPos * sin(largeBranchPhi) * sin(largeBranchTheta) + 0.5);
					startZ = (smallBranchPos * cos(largeBranchTheta) + 0.5);
										
					// GENERATE ORIENTATION ANGLE FOR EACH BRANCH
					// Branches always grown away from center of the tree
					smallBranchTheta = (smallBranchThetaDistro->calcRand() / 180)*pi;
					smallBranchPhi = (smallBranchPhiDistro->calcRand()/180)*pi;
					
					// ADD SECTIONS TO BRANCH
					rX = 0; rY = 0; rZ = 0;
					rItt = 0;
					while(rItt < smallBranchLenghtVox)
					{
						branchX = startX + (rX * cos(smallBranchPhi) * sin(smallBranchTheta) + 0.5);
						branchY = startY + (rY * sin(smallBranchPhi) * sin(smallBranchTheta) + 0.5);
						branchZ = startZ + (rZ * cos(smallBranchTheta) + 0.5);
						
						if (branchX >= maxX)
						{
							branchX = centerX;
							startX = centerX;
							rX = 0;
						}
						if (branchY >= maxY)
						{
							branchY = centerY;
							startY = centerY;
							rY = 0;
						}
						
						// SET START POINT OF BRANCH SECTION
						transect->setValue(branchX, branchY, branchZ, 1);
						
						/* GROW OUT BRANCHES
						 * If the branch radius is less than one voxel this will not run
						 */
						x = branchX; // Initialy set x to branch start , if x > numX, x goes to zero (bondary condition).
						xItt = 0; // xItt advances by on each time until a the branch has been grown to the correct number of voxels
						
						while(xItt < smallBranchRadiusVox)
						{
							y = branchY; // Initialy set y to branch start , if y > numY, y goes to zero (bondary condition).
							yItt = 0; // yItt advances by on each time until a the branch has been grown to the correct number of voxels
							
							while (yItt < smallBranchRadiusVox) 
							{
								transect->setValue(x, y, branchZ , 1);
								
								y++; yItt++; 
							}
							x++; xItt++;
						}
						
						// Add leaves to end of branch.
						if(rItt == (smallBranchLenghtVox - 1))
						{
							for(unsigned int l = 0; l < (leafDensityInt / smallBranchDensityInt);l++)
							{
								unsigned int leafStartX = leafPosHDistro->calcRand() + branchX;
								unsigned int leafStartY = leafPosHDistro->calcRand() + branchY;
								unsigned int leafStartZ = leafPosVDistro->calcRand()+ branchZ;
								
								leafTheta = ((leafThetaDistro->calcRand()) / 180) * pi; // Leaf orientation random between 0 and 90 deg.
								
								/* SET THE START POINT OF LEAF
								 * if the leaf radius is less than 
								 * the resolution, the leaves will not be grown out */
								
								transect->setValue(leafStartX, leafStartY, leafStartZ, 2);
								// Save out as geos coordinates
								if (leafStartX > 0 && leafStartX < transect->getWidth()) 
								{
									if (leafStartY > 0 && leafStartY < transect->getLenth()) 
									{
										
										coordinates->push_back(geos::geom::Coordinate(leafStartX, leafStartY, leafStartZ));
									}
									
								}
								
								// GROW LEAVES OUT			
								rLeafItt = 0;
								rYLeaf = 0;
								rZLeaf = 0;
								
								while(rLeafItt < leafLenghtVox)
								{
									xLeafItt = 0;
									xLeaf = leafStartX;
									
									yLeaf = leafStartY + int(rYLeaf * sin(leafTheta) + 0.5);
									zLeaf = leafStartZ + int(rZLeaf * cos(leafTheta) + 0.5);
									
									while (xLeafItt < leafWidthVox) 
									{
										transect->setValue(xLeaf, yLeaf, zLeaf, 2);
										if (xLeaf > 0 && xLeaf < transect->getWidth()) 
										{
											if (yLeaf > 0 && yLeaf < transect->getLenth()) 
											{
												coordinates->push_back(geos::geom::Coordinate(xLeaf, yLeaf, zLeaf));
											}
											
										}
										xLeaf++; xLeafItt++;
									}
									rZLeaf++; rYLeaf++; rLeafItt++;
								}
							}
						}
						rX++; rY++; rZ++; rItt++;
					}
				}
				
			}
			
		}

		
		
		/*************************
		 * CALCULATE CONVEX HULL *
		 *************************/
        rsgis::geom::RSGISGeometry geometry;
		try 
		{
			canopyPolys->push_back(geometry.findConvexHull(coordinates));
		}
		catch (rsgis::geom::RSGISGeometryException) 
		{
			std::cout << "\tNot Enough Coordinates to create convex hull!" << std::endl;
		}
		
		delete coordinates;
		
	}
	RSGISModelTreeCanopy::~RSGISModelTreeCanopy()
	{
		
	}
}}

