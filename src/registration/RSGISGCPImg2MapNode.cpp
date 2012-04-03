/*
 *  RSGISGCPImg2MapNode.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 06/09/2010.
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

#include "RSGISGCPImg2MapNode.h"


namespace rsgis{namespace reg{

	RSGISGCPImg2MapNode::RSGISGCPImg2MapNode(double eastings, double northings, float imgXIn, float imgYIn): eastings_(0), northings_(0), imgX_(0), imgY_(0)
	{
        this->eastings_ = eastings;
        this->northings_ = northings;
		this->imgX_ = imgXIn;
		this->imgY_ = imgYIn;
	}
	
	double RSGISGCPImg2MapNode::eastings() const
	{
		return eastings_;
	}
	
	double RSGISGCPImg2MapNode::northings() const
	{
		return northings_;
	}
	
	
	float RSGISGCPImg2MapNode::imgX() const
	{
		return imgX_;
	}
	
	float RSGISGCPImg2MapNode::imgY() const
	{
		return imgY_;
	}
	
	double RSGISGCPImg2MapNode::distanceGeo(RSGISGCPImg2MapNode *pt)
	{
		double sqSum = ((this->eastings_ - pt->eastings_)*(this->eastings_ - pt->eastings_)) + ((this->northings_ - pt->northings_)*(this->northings_ - pt->northings_));
		
		return sqrt(sqSum/2);
	}
	
	RSGISGCPImg2MapNode::~RSGISGCPImg2MapNode()
	{
		
	}
	
}}


