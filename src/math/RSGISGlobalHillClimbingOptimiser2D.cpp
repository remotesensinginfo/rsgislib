/*
 *  RSGISGlobalHillClimbingOptimiser2D.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/08/2009.
 *  Copyright 2009 RSGISLib.
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

#include "RSGISGlobalHillClimbingOptimiser2D.h"


namespace rsgis{namespace math{
	

	RSGISGlobalHillClimbingOptimiser2D::RSGISGlobalHillClimbingOptimiser2D(RSGISGlobalOptimisationFunction *func, bool maximise, int maxNumIterations) : RSGISGlobalOptimiser2D(func, maximise)
	{
		this->maxNumIterations = maxNumIterations;
	}
	
    std::vector<geos::geom::Coordinate*>* RSGISGlobalHillClimbingOptimiser2D::optimise4Neighbor(std::vector<geos::geom::Coordinate*> *coords, double step, geos::geom::Envelope *boundary) throw(RSGISOptimisationException)
	{
        std::vector<geos::geom::Coordinate*> *outCoords = new std::vector<geos::geom::Coordinate*>();
		try
		{
            std::vector<geos::geom::Coordinate*>::iterator iterCoords;
			for(iterCoords = coords->begin(); iterCoords != coords->end(); ++iterCoords)
			{
				outCoords->push_back(new geos::geom::Coordinate((*iterCoords)->x, (*iterCoords)->y, (*iterCoords)->z));
			}
			
			/*
			 * 0 - Current Pixel
			 * 1 - X-1
			 * 2 - Y+1
			 * 3 - X+1
			 * 4 - Y-1
			 */
			double value = 0;
			double selectValue = 0;
			int selectIdx = 0;
			bool first = true;
			geos::geom::Coordinate *coord = new geos::geom::Coordinate();
			int numIteration = 0;
			int nodeCount = 0;
			
			geos::geom::Envelope *minBoundary = new geos::geom::Envelope(boundary->getMinX(), (boundary->getMaxX()-step), boundary->getMinY(), (boundary->getMaxY()-step));
						
            std::cout << "Started " << std::flush;
			
			bool change = true;
			while(change)
			{
				if(numIteration == maxNumIterations)
				{
					break;
				}
                std::cout << "." << std::flush;
				
				change = false;
				nodeCount = 0;
				for(iterCoords = outCoords->begin(); iterCoords != outCoords->end(); ++iterCoords)
				{
					coord->x = (*iterCoords)->x;
					coord->y = (*iterCoords)->y;
					coord->z = (*iterCoords)->z;
					first = true;
					for(int i = 0; i < 5; ++i)
					{
						if(i == 0)
						{
							(*iterCoords)->x = coord->x;
							(*iterCoords)->y = coord->y;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 1)
						{
							(*iterCoords)->x = coord->x - step;
							(*iterCoords)->y = coord->y;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 2)
						{
							(*iterCoords)->x = coord->x;
							(*iterCoords)->y = coord->y + step;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 3)
						{
							(*iterCoords)->x = coord->x + step;
							(*iterCoords)->y = coord->y;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 4)
						{
							(*iterCoords)->x = coord->x;
							(*iterCoords)->y = coord->y - step;
							(*iterCoords)->z = coord->z;
						}
						
						if(minBoundary->contains(**iterCoords))
						{
							value = this->func->calcValue(outCoords);
						}
						else
						{
							value = -1;
						}
						
						if(value != -1)
						{
							if(first)
							{
								selectValue = value;
								selectIdx = i;
								first = false;
							}
							else if(maximise)
							{
								if(value > selectValue)
								{
									selectValue = value;
									selectIdx = i;
								}
							}
							else
							{
								if(value < selectValue)
								{
									selectValue = value;
									selectIdx = i;
								}
							}
						}
					}
					
					if(selectIdx == 0)
					{
						(*iterCoords)->x = coord->x;
						(*iterCoords)->y = coord->y;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 1)
					{
						change = true;
						(*iterCoords)->x = coord->x - step;
						(*iterCoords)->y = coord->y;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 2)
					{
						change = true;
						(*iterCoords)->x = coord->x;
						(*iterCoords)->y = coord->y + step;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 3)
					{
						change = true;
						(*iterCoords)->x = coord->x + step;
						(*iterCoords)->y = coord->y;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 4)
					{
						change = true;
						(*iterCoords)->x = coord->x;
						(*iterCoords)->y = coord->y - step;
						(*iterCoords)->z = coord->z;
					}
					++nodeCount;
				}	
				++numIteration;
			}
			
            std::cout << " Complete\n";
			
			delete minBoundary;
			delete coord;			
		}
		catch(RSGISOptimisationException &e)
		{
			throw e;
		}
		
		return outCoords;
	}
	
    std::vector<geos::geom::Coordinate*>* RSGISGlobalHillClimbingOptimiser2D::optimise8Neighbor(std::vector<geos::geom::Coordinate*> *coords, double step, geos::geom::Envelope *boundary) throw(RSGISOptimisationException)
	{
        std::vector<geos::geom::Coordinate*> *outCoords = new std::vector<geos::geom::Coordinate*>();
		try
		{
            std::vector<geos::geom::Coordinate*>::iterator iterCoords;
			for(iterCoords = coords->begin(); iterCoords != coords->end(); ++iterCoords)
			{
				outCoords->push_back(new geos::geom::Coordinate((*iterCoords)->x, (*iterCoords)->y, (*iterCoords)->z));
			}
			
			/*
			 * 0 - Current Pixel
			 * 1 - X-1
			 * 2 - X-1 Y+1
			 * 3 - Y+1
			 * 4 - X+1 Y+1
			 * 5 - X+1
			 * 6 - X+1 Y-1
			 * 7 - Y-1
			 * 8 - X-1 Y-1
			 */
			double value = 0;
			double selectValue = 0;
			int selectIdx = 0;
			bool first = true;
			geos::geom::Coordinate *coord = new geos::geom::Coordinate();
			int numIteration = 0;
			int nodeCount = 0;
			
			geos::geom::Envelope *minBoundary = new geos::geom::Envelope(boundary->getMinX(), (boundary->getMaxX()-step), boundary->getMinY(), (boundary->getMaxY()-step));
						
            std::cout << "Started " << std::flush;
			
			bool change = true;
			while(change)
			{
				if(numIteration == maxNumIterations)
				{
					break;
				}
                std::cout << "." << std::flush;
				
				change = false;
				nodeCount = 0;
				for(iterCoords = outCoords->begin(); iterCoords != outCoords->end(); ++iterCoords)
				{
					coord->x = (*iterCoords)->x;
					coord->y = (*iterCoords)->y;
					coord->z = (*iterCoords)->z;
					first = true;
					for(int i = 0; i < 9; ++i)
					{
						if(i == 0)
						{
							(*iterCoords)->x = coord->x;
							(*iterCoords)->y = coord->y;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 1)
						{
							(*iterCoords)->x = coord->x - step;
							(*iterCoords)->y = coord->y;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 2)
						{
							(*iterCoords)->x = coord->x - step;
							(*iterCoords)->y = coord->y + step;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 3)
						{
							(*iterCoords)->x = coord->x;
							(*iterCoords)->y = coord->y + step;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 4)
						{
							(*iterCoords)->x = coord->x + step;
							(*iterCoords)->y = coord->y + step;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 5)
						{
							(*iterCoords)->x = coord->x + step;
							(*iterCoords)->y = coord->y;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 6)
						{
							(*iterCoords)->x = coord->x + step;
							(*iterCoords)->y = coord->y - step;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 7)
						{
							(*iterCoords)->x = coord->x;
							(*iterCoords)->y = coord->y - step;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 8)
						{
							(*iterCoords)->x = coord->x - step;
							(*iterCoords)->y = coord->y - step;
							(*iterCoords)->z = coord->z;
						}
						
						if(minBoundary->contains(**iterCoords))
						{
							value = this->func->calcValue(outCoords);
						}
						else
						{
							value = -1;
						}
												
						if(value != -1)
						{
							if(first)
							{
								selectValue = value;
								selectIdx = i;
								first = false;
							}
							else if(maximise)
							{
								if(value > selectValue)
								{
									selectValue = value;
									selectIdx = i;
								}
							}
							else
							{
								if(value < selectValue)
								{
									selectValue = value;
									selectIdx = i;
								}
							}
						}
					}
					
					if(selectIdx == 0)
					{
						(*iterCoords)->x = coord->x;
						(*iterCoords)->y = coord->y;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 1)
					{
						change = true;
						(*iterCoords)->x = coord->x - step;
						(*iterCoords)->y = coord->y;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 2)
					{
						change = true;
						(*iterCoords)->x = coord->x - step;
						(*iterCoords)->y = coord->y + step;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 3)
					{
						change = true;
						(*iterCoords)->x = coord->x;
						(*iterCoords)->y = coord->y + step;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 4)
					{
						change = true;
						(*iterCoords)->x = coord->x + step;
						(*iterCoords)->y = coord->y + step;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 5)
					{
						change = true;
						(*iterCoords)->x = coord->x + step;
						(*iterCoords)->y = coord->y;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 6)
					{
						change = true;
						(*iterCoords)->x = coord->x + step;
						(*iterCoords)->y = coord->y - step;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 7)
					{
						change = true;
						(*iterCoords)->x = coord->x;
						(*iterCoords)->y = coord->y - step;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 8)
					{
						change = true;
						(*iterCoords)->x = coord->x - step;
						(*iterCoords)->y = coord->y - step;
						(*iterCoords)->z = coord->z;
					}
					++nodeCount;
				}
				
				++numIteration;
			}
			
            std::cout << " Complete\n";
			
			delete minBoundary;
			delete coord;			
		}
		catch(RSGISOptimisationException &e)
		{
			throw e;
		}
		
		return outCoords;
	}
	
	RSGISGlobalHillClimbingOptimiser2D::~RSGISGlobalHillClimbingOptimiser2D()
	{
		
	}
	
	
	
	RSGISGlobalHillClimbingOptimiser2DVaryNumPts::RSGISGlobalHillClimbingOptimiser2DVaryNumPts(RSGISGlobalOptimisationFunction *func, bool maximise, int maxNumIterations) : RSGISGlobalOptimiser2D(func, maximise)
	{
		this->maxNumIterations = maxNumIterations;
	}
	
    std::vector<geos::geom::Coordinate*>* RSGISGlobalHillClimbingOptimiser2DVaryNumPts::optimise4Neighbor(std::vector<geos::geom::Coordinate*> *coords, double step, geos::geom::Envelope *boundary) throw(RSGISOptimisationException)
	{
        std::vector<geos::geom::Coordinate*> *outCoords = new std::vector<geos::geom::Coordinate*>();
		try
		{
            std::vector<geos::geom::Coordinate*>::iterator iterCoords;
			for(iterCoords = coords->begin(); iterCoords != coords->end(); ++iterCoords)
			{
				outCoords->push_back(new geos::geom::Coordinate((*iterCoords)->x, (*iterCoords)->y, (*iterCoords)->z));
			}
			
			/*
			 * 0 - Current Pixel
			 * 1 - X-1
			 * 2 - Y+1
			 * 3 - X+1
			 * 4 - Y-1
			 */
			double value = 0;
			double distance = 0;
			double selectValue = 0;
			int selectIdx = 0;
			bool first = true;
			geos::geom::Coordinate *coord = new geos::geom::Coordinate();
			int numIteration = 0;
			int nodeCount = 0;
			geos::geom::Coordinate *tmpCoord = NULL;
			
			geos::geom::Envelope *minBoundary = new geos::geom::Envelope(boundary->getMinX(), (boundary->getMaxX()-step), boundary->getMinY(), (boundary->getMaxY()-step));
			
            std::cout << "Started " << std::flush;
			
			bool change = true;
			while(change)
			{
				if(numIteration == maxNumIterations)
				{
					break;
				}
                std::cout << "." << std::flush;
				
				change = false;
				nodeCount = 0;
				
				for(iterCoords = outCoords->begin(); iterCoords != outCoords->end(); ++iterCoords)
				{
					coord->x = (*iterCoords)->x;
					coord->y = (*iterCoords)->y;
					coord->z = (*iterCoords)->z;
					first = true;
					for(int i = 0; i < 5; ++i)
					{
						if(i == 0)
						{
							(*iterCoords)->x = coord->x;
							(*iterCoords)->y = coord->y;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 1)
						{
							(*iterCoords)->x = coord->x - step;
							(*iterCoords)->y = coord->y;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 2)
						{
							(*iterCoords)->x = coord->x;
							(*iterCoords)->y = coord->y + step;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 3)
						{
							(*iterCoords)->x = coord->x + step;
							(*iterCoords)->y = coord->y;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 4)
						{
							(*iterCoords)->x = coord->x;
							(*iterCoords)->y = coord->y - step;
							(*iterCoords)->z = coord->z;
						}
						
						try 
						{
							if(minBoundary->contains(**iterCoords))
							{
								value = this->func->calcValue(outCoords);
							}
							else
							{
								value = -1;
							}
						}
						catch (RSGISOptimisationException &e) 
						{
							value = -1;
						}
						
						
						if(value != -1)
						{
							if(first)
							{
								selectValue = value;
								selectIdx = i;
								first = false;
							}
							else if(maximise)
							{
								if(value > selectValue)
								{
									selectValue = value;
									selectIdx = i;
								}
							}
							else
							{
								if(value < selectValue)
								{
									selectValue = value;
									selectIdx = i;
								}
							}
						}
					}
					
					if(selectIdx == 0)
					{
						(*iterCoords)->x = coord->x;
						(*iterCoords)->y = coord->y;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 1)
					{
						change = true;
						(*iterCoords)->x = coord->x - step;
						(*iterCoords)->y = coord->y;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 2)
					{
						change = true;
						(*iterCoords)->x = coord->x;
						(*iterCoords)->y = coord->y + step;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 3)
					{
						change = true;
						(*iterCoords)->x = coord->x + step;
						(*iterCoords)->y = coord->y;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 4)
					{
						change = true;
						(*iterCoords)->x = coord->x;
						(*iterCoords)->y = coord->y - step;
						(*iterCoords)->z = coord->z;
					}
					++nodeCount;
				}	
				
				// Iterate through the coordinate and remove pts which are too
				// close to one another and introduce new points where gaps are 
				// too large.
				
				first = true;
				for(iterCoords = outCoords->begin(); iterCoords != outCoords->end(); )
				{
					if(first)
					{
						coord->x = (*iterCoords)->x;
						coord->y = (*iterCoords)->y;
						coord->z = (*iterCoords)->z;
						first = false;
					}
					else
					{
						//cout << "Distance = " << (*iterCoords)->distance(*coord) << endl;
						distance = (*iterCoords)->distance(*coord);
						if((distance < 0.5) & (outCoords->size() > 3))
						{
							//cout << "Removed Coordinate\n";
							outCoords->erase(iterCoords);
						}
						else if(distance > 3)
						{
							//cout << "Add Coordinate\n";
							float tmpDist = distance/2;
							tmpCoord = new geos::geom::Coordinate();
							this->findPointOnLine(coord, (*iterCoords), tmpDist, tmpCoord);
							//cout << "tmpCoord= [" << tmpCoord->x << "," << tmpCoord->y << "]\n";
							iterCoords = outCoords->insert(iterCoords,tmpCoord);
						}
						else 
						{
							coord->x = (*iterCoords)->x;
							coord->y = (*iterCoords)->y;
							coord->z = (*iterCoords)->z;
							++iterCoords;
						}
					}					
				}
				
				++numIteration;
			}
			
            std::cout << " Complete\n";
			
			delete minBoundary;
			delete coord;			
		}
		catch(RSGISOptimisationException &e)
		{
			throw e;
		}
		
		return outCoords;
	}
	
    std::vector<geos::geom::Coordinate*>* RSGISGlobalHillClimbingOptimiser2DVaryNumPts::optimise8Neighbor(std::vector<geos::geom::Coordinate*> *coords, double step, geos::geom::Envelope *boundary) throw(RSGISOptimisationException)
	{
        std::vector<geos::geom::Coordinate*> *outCoords = new std::vector<geos::geom::Coordinate*>();
		try
		{
            std::vector<geos::geom::Coordinate*>::iterator iterCoords;
			for(iterCoords = coords->begin(); iterCoords != coords->end(); ++iterCoords)
			{
				outCoords->push_back(new geos::geom::Coordinate((*iterCoords)->x, (*iterCoords)->y, (*iterCoords)->z));
			}
			
			/*
			 * 0 - Current Pixel
			 * 1 - X-1
			 * 2 - X-1 Y+1
			 * 3 - Y+1
			 * 4 - X+1 Y+1
			 * 5 - X+1
			 * 6 - X+1 Y-1
			 * 7 - Y-1
			 * 8 - X-1 Y-1
			 */
			double value = 0;
			double selectValue = 0;
			double distance = 0;
			int selectIdx = 0;
			bool first = true;
			geos::geom::Coordinate *coord = new geos::geom::Coordinate();
			geos::geom::Coordinate *tmpCoord = NULL;
			int numIteration = 0;
			int nodeCount = 0;
			
			geos::geom::Envelope *minBoundary = new geos::geom::Envelope(boundary->getMinX(), (boundary->getMaxX()-step), boundary->getMinY(), (boundary->getMaxY()-step));
			
            std::cout << "Started " << std::flush;
			
			bool change = true;
			while(change)
			{
				if(numIteration == maxNumIterations)
				{
					break;
				}
                std::cout << "." << std::flush;
				
				change = false;
				nodeCount = 0;
				//cout << "Iterate Through coords" << endl;
				
				for(iterCoords = outCoords->begin(); iterCoords != outCoords->end(); ++iterCoords)
				{
					coord->x = (*iterCoords)->x;
					coord->y = (*iterCoords)->y;
					coord->z = (*iterCoords)->z;
					first = true;
					for(int i = 0; i < 9; ++i)
					{
						if(i == 0)
						{
							(*iterCoords)->x = coord->x;
							(*iterCoords)->y = coord->y;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 1)
						{
							(*iterCoords)->x = coord->x - step;
							(*iterCoords)->y = coord->y;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 2)
						{
							(*iterCoords)->x = coord->x - step;
							(*iterCoords)->y = coord->y + step;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 3)
						{
							(*iterCoords)->x = coord->x;
							(*iterCoords)->y = coord->y + step;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 4)
						{
							(*iterCoords)->x = coord->x + step;
							(*iterCoords)->y = coord->y + step;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 5)
						{
							(*iterCoords)->x = coord->x + step;
							(*iterCoords)->y = coord->y;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 6)
						{
							(*iterCoords)->x = coord->x + step;
							(*iterCoords)->y = coord->y - step;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 7)
						{
							(*iterCoords)->x = coord->x;
							(*iterCoords)->y = coord->y - step;
							(*iterCoords)->z = coord->z;
						}
						else if(i == 8)
						{
							(*iterCoords)->x = coord->x - step;
							(*iterCoords)->y = coord->y - step;
							(*iterCoords)->z = coord->z;
						}
						
						try 
						{
							if(minBoundary->contains(**iterCoords))
							{
								value = this->func->calcValue(outCoords);
							}
							else
							{
								value = -1;
							}
						}
						catch (RSGISOptimisationException &e) 
						{
							value = -1;
						}
						
						if(value != -1)
						{
							if(first)
							{
								selectValue = value;
								selectIdx = i;
								first = false;
							}
							else if(maximise)
							{
								if(value > selectValue)
								{
									selectValue = value;
									selectIdx = i;
								}
							}
							else
							{
								if(value < selectValue)
								{
									selectValue = value;
									selectIdx = i;
								}
							}
						}
					}
					
					if(selectIdx == 0)
					{
						(*iterCoords)->x = coord->x;
						(*iterCoords)->y = coord->y;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 1)
					{
						change = true;
						(*iterCoords)->x = coord->x - step;
						(*iterCoords)->y = coord->y;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 2)
					{
						change = true;
						(*iterCoords)->x = coord->x - step;
						(*iterCoords)->y = coord->y + step;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 3)
					{
						change = true;
						(*iterCoords)->x = coord->x;
						(*iterCoords)->y = coord->y + step;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 4)
					{
						change = true;
						(*iterCoords)->x = coord->x + step;
						(*iterCoords)->y = coord->y + step;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 5)
					{
						change = true;
						(*iterCoords)->x = coord->x + step;
						(*iterCoords)->y = coord->y;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 6)
					{
						change = true;
						(*iterCoords)->x = coord->x + step;
						(*iterCoords)->y = coord->y - step;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 7)
					{
						change = true;
						(*iterCoords)->x = coord->x;
						(*iterCoords)->y = coord->y - step;
						(*iterCoords)->z = coord->z;
					}
					else if(selectIdx == 8)
					{
						change = true;
						(*iterCoords)->x = coord->x - step;
						(*iterCoords)->y = coord->y - step;
						(*iterCoords)->z = coord->z;
					}
					++nodeCount;
				}
				
				//cout << "Node Count = " << nodeCount << endl << endl;
				// Iterate through the coordinate and remove pts which are too
				// close to one another and introduce new points where gaps are 
				// too large.
				
				first = true;
				for(iterCoords = outCoords->begin(); iterCoords != outCoords->end(); )
				{
					if(first)
					{
						coord->x = (*iterCoords)->x;
						coord->y = (*iterCoords)->y;
						coord->z = (*iterCoords)->z;
						first = false;
					}
					else
					{
						//cout << "Distance = " << (*iterCoords)->distance(*coord) << endl;
						distance = (*iterCoords)->distance(*coord);
						if((distance < 0.5) & (outCoords->size() > 3))
						{
							//cout << "Removed Coordinate\n";
							outCoords->erase(iterCoords);
						}
						else if(distance > 3)
						{
							//cout << "Add Coordinate\n";
							float tmpDist = distance/2;
							tmpCoord = new geos::geom::Coordinate();
							this->findPointOnLine(coord, (*iterCoords), tmpDist, tmpCoord);
							//cout << "tmpCoord= [" << tmpCoord->x << "," << tmpCoord->y << "]\n";
							iterCoords = outCoords->insert(iterCoords,tmpCoord);
						}
						else 
						{
							coord->x = (*iterCoords)->x;
							coord->y = (*iterCoords)->y;
							coord->z = (*iterCoords)->z;
							++iterCoords;
						}
					}					
				}
				
				
				
				++numIteration;
			}
			
            std::cout << " Complete\n";
			
			/*
			cout << endl;
			for(iterCoords = outCoords->begin(); iterCoords != outCoords->end(); ++iterCoords)
			{
				cout << (*iterCoords)->x << ";" << (*iterCoords)->y << "\n";
			}
			cout << endl;
			*/
			
			delete minBoundary;
			delete coord;	
		}
		catch(RSGISOptimisationException &e)
		{
			throw e;
		}
		
		return outCoords;
	}
	
	void RSGISGlobalHillClimbingOptimiser2DVaryNumPts::findPointOnLine(geos::geom::Coordinate *p1, geos::geom::Coordinate *p2, float distance, geos::geom::Coordinate *p3)
	{
		if(distance == 0)
		{
			p3->x = p1->x;
			p3->y = p1->y;
		}
		else
		{
			double dx = p2->x - p1->x;
			double dy = p2->y - p1->y;
			double theta = atan(dy/dx);
			double y1 = distance * sin(theta);
			double x1 = distance * cos(theta);
			
			//cout << "dx = " << dx << endl;
			//cout << "dy = " << dy << endl;
			 
			//cout << "x1 = " << x1 << endl;
			//cout << "y1 = " << y1 << endl;
			
			if((dx >= 0) & (dy > 0))
			{
				p3->x = p1->x + x1;
				p3->y = p1->y + y1;
			}
			else if((dx >= 0) & (dy <= 0))
			{
				p3->x = p1->x + x1;
				p3->y = p1->y + y1;
			}
			else if((dx < 0) & (dy > 0))
			{
				p3->x = p1->x - x1;
				p3->y = p1->y - y1;
			}
			else if((dx < 0) & (dy <= 0))
			{
				p3->x = p1->x - x1;
				p3->y = p1->y - y1;
			}
		}
		p3->z = 0;
		
		//cout << "P1= [" << p1->x << "," << p1->y << "]\n";
		//cout << "P2= [" << p2->x << "," << p2->y << "]\n";
		//cout << "P3= [" << p3->x << "," << p3->y << "]\n";
	}
	
	RSGISGlobalHillClimbingOptimiser2DVaryNumPts::~RSGISGlobalHillClimbingOptimiser2DVaryNumPts()
	{
		
	}
}}

