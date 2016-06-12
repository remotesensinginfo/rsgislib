/*
 *  RSGIS2DPoint.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2009.
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

#include "RSGIS2DPoint.h"

namespace rsgis{namespace geom{
	
	RSGIS2DPoint::RSGIS2DPoint()
	{
		index = 0;
		classID = 0;
		touched = false;
	}
	
	RSGIS2DPoint::RSGIS2DPoint(geos::geom::Coordinate *pt)
	{
		this->point = pt;
		index = 0;
		classID = 0;
		touched = false;
	}
	
	RSGIS2DPoint::RSGIS2DPoint(geos::geom::Coordinate *pt, int classID)
	{
		this->point = pt;
		index = 0;
		touched = false;
		this->classID = classID;
	}
	
	void RSGIS2DPoint::setPoint(geos::geom::Coordinate *pt)
	{
		point = pt;
	}
	
	const geos::geom::Coordinate* RSGIS2DPoint::getPoint()
	{
		return point;
	}
    
    geos::geom::Coordinate RSGIS2DPoint::getCoordPoint()
    {
        return geos::geom::Coordinate(this->point->x, this->point->y);
    }
    
    geos::geom::Point* RSGIS2DPoint::getAsGeosPoint()
    {
        geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
        geos::geom::Point *geosPt = geomFactory->createPoint(*point);
        return geosPt;
    }
	
	double RSGIS2DPoint::getX()
	{
		return point->x;
	}
	
	double RSGIS2DPoint::getY()
	{
		return point->y;
	}
	
	double RSGIS2DPoint::getZ()
	{
		return point->z;
	}
	
	int RSGIS2DPoint::getIndex()
	{
		return index;
	}
	
	void RSGIS2DPoint::setIndex(int index)
	{
		this->index = index;
	}
	
	void RSGIS2DPoint::setTouched(bool touched)
	{
		this->touched = touched;
	}
	
	bool RSGIS2DPoint::getTouched()
	{
		return touched;
	}
	
	void RSGIS2DPoint::setClassID(unsigned int classID)
	{
		this->classID = classID;
	}
	
	unsigned int RSGIS2DPoint::getClassID()
	{
		return this->classID;
	}
	
	double RSGIS2DPoint::distance(RSGIS2DPoint *pt)
	{
		return sqrt(((pt->getPoint()->x - point->x)*(pt->getPoint()->x - point->x))+((pt->getPoint()->y - point->y)*(pt->getPoint()->y - point->y))); 
	}
	
	double RSGIS2DPoint::distance(geos::geom::Coordinate *pt)
	{
		return sqrt(((pt->x - point->x)*(pt->x - point->x))+((pt->y - point->y)*(pt->y - point->y)));
	}
    
    bool RSGIS2DPoint::equals(RSGIS2DPoint *pt)
    {
        //std::cout << "[" << point->x << "," << point->y << "][" << pt->point->x << "," << pt->point->y << "]\n";
        if((pt->point->x == point->x) && (pt->point->y == point->y))
        {
            //std::cout << "\t Are Equal\n";
            return true;
        }
        //std::cout << "\t Are Not Equal\n";
        return false;
    }
    
    bool RSGIS2DPoint::equals(geos::geom::Coordinate *pt)
    {
        //std::cout << "[" << point->x << "," << point->y << "][" << pt->x << "," << pt->y << "]\n";
        if((pt->x == point->x) && (pt->y == point->y))
        {
            //std::cout << "\t Are Equal\n";
            return true;
        }
        //std::cout << "\t Are Not Equal\n";
        return false;
    }
	
	std::ostream& operator<<(std::ostream& ostr, const RSGIS2DPoint& pt)
	{
		ostr << "[" << pt.point->x << "," << pt.point->y << "," << pt.point->z << "]";
		return ostr;
	}
	
	std::ostream& RSGIS2DPoint::operator<<(std::ostream& ostr)
	{
		ostr << "[" << point->x << "," << point->y << "," << point->z << "]";
		return ostr;
	}
	
	bool RSGIS2DPoint::operator==(RSGIS2DPoint pt) const
	{
		if(pt.point->x == point->x &&
		   pt.point->y == point->y)
		{
			return true;
		}
		return false;
	}
	
	bool RSGIS2DPoint::operator!=(RSGIS2DPoint pt) const
	{
		if(pt.point->x != point->x ||
		   pt.point->y != point->y)
		{
			return true;
		}
		return false;
	}
	
	bool RSGIS2DPoint::operator>(RSGIS2DPoint pt) const
	{
		bool returnValue = false;
		if(this->point->y > pt.point->y)
		{
			returnValue = true;
		}
		else if(this->point->y == pt.point->y)
		{
			if(this->point->x > pt.point->x)
			{
				returnValue = true;
			}
		}
		return returnValue;
	}
	
	bool RSGIS2DPoint::operator<(RSGIS2DPoint pt) const
	{
		bool returnValue = false;
		if(this->point->y < pt.point->y)
		{
			returnValue = true;
		}
		else if(this->point->y == pt.point->y)
		{
			if(this->point->x < pt.point->x)
			{
				returnValue = true;
			}
		}
		
		return returnValue;
	}
	
	bool RSGIS2DPoint::operator>=(RSGIS2DPoint pt) const
	{
		bool returnValue = false;
		
		if(pt.point->x == this->point->x && 
		   pt.point->y == this->point->y)
		{
			returnValue = true;
		}
		else if(this->point->y > pt.point->y)
		{
			returnValue = true;
		}
		else if(this->point->y == pt.point->y)
		{
			if(this->point->x > pt.point->x)
			{
				returnValue = true;
			}
		}
		return returnValue;
	}
	
	bool RSGIS2DPoint::operator<=(RSGIS2DPoint pt) const
	{
		bool returnValue = false;
		
		if(pt.point->x == this->point->x && 
		   pt.point->y == this->point->y)
		{
			returnValue = true;
		}
		else if(this->point->y < pt.point->y)
		{
			returnValue = true;
		}
		else if(this->point->y == pt.point->y)
		{
			if(this->point->x < pt.point->x)
			{
				returnValue = true;
			}
		}		
		return returnValue;
	}
	
	bool RSGIS2DPoint::operator==(RSGIS2DPoint *pt) const
	{
		if(pt->point->x == point->x &&
		   pt->point->y == point->y)
		{
			return true;
		}
		return false;
	}
	
	bool RSGIS2DPoint::operator!=(RSGIS2DPoint *pt) const
	{
		if((pt->point->x != point->x) || (pt->point->y != point->y))
		{
			return true;
		}
		return false;
	}
	
	bool RSGIS2DPoint::operator>(RSGIS2DPoint *pt) const
	{
		bool returnValue = false;
		if(this->point->y > pt->point->y)
		{
			returnValue = true;
		}
		else if(this->point->y == pt->point->y)
		{
			if(this->point->x > pt->point->x)
			{
				returnValue = true;
			}
		}
		return returnValue;
	}
	
	bool RSGIS2DPoint::operator<(RSGIS2DPoint *pt) const
	{
		bool returnValue = false;
		if(this->point->y < pt->point->y)
		{
			returnValue = true;
		}
		else if(this->point->y == pt->point->y)
		{
			if(this->point->x < pt->point->x)
			{
				returnValue = true;
			}
		}
		
		return returnValue;
	}
	
	bool RSGIS2DPoint::operator>=(RSGIS2DPoint *pt) const
	{
		bool returnValue = false;
		
		if(pt->point->x == this->point->x & 
		   pt->point->y == this->point->y)
		{
			returnValue = true;
		}
		else if(this->point->y > pt->point->y)
		{
			returnValue = true;
		}
		else if(this->point->y == pt->point->y)
		{
			if(this->point->x > pt->point->x)
			{
				returnValue = true;
			}
		}
		return returnValue;
	}
	
	bool RSGIS2DPoint::operator<=(RSGIS2DPoint *pt) const
	{
		bool returnValue = false;
		
		if(pt->point->x == this->point->x && 
		   pt->point->y == this->point->y)
		{
			returnValue = true;
		}
		else if(this->point->y < pt->point->y)
		{
			returnValue = true;
		}
		else if(this->point->y == pt->point->y)
		{
			if(this->point->x < pt->point->x)
			{
				returnValue = true;
			}
		}		
		return returnValue;
	}
	
	RSGIS2DPoint::~RSGIS2DPoint()
	{
		delete point;
	}

}}

