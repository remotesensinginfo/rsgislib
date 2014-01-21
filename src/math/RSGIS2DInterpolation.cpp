/*
 *  RSGIS2DInterpolation.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 14/09/2013.
 *
 *  Copyright 2013 RSGISLib.
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

#include "RSGIS2DInterpolation.h"


namespace rsgis {namespace math{
    
    
    RSGISSearchKNN2DInterpolator::RSGISSearchKNN2DInterpolator(unsigned int k): RSGIS2DInterpolator()
    {
        this->k = k;
    }
    
    void RSGISSearchKNN2DInterpolator::initInterpolator(std::vector<RSGISInterpolatorDataPoint> *pts) throw(RSGISInterpolationException)
    {
        try
        {
            if(pts->size() < k)
            {
                throw RSGISInterpolationException("There are less than \'k\' points in the data points list.");
            }
            
            this->dataPTS = pts;
        }
        catch(RSGISInterpolationException &e)
        {
            throw e;
        }
        initialised = true;
    }
    
    
    std::list<std::pair<double,RSGISInterpolatorDataPoint> >* RSGISSearchKNN2DInterpolator::findKNN(double eastings, double northings) throw(RSGISInterpolationException)
    {
        std::list<std::pair<double,RSGISInterpolatorDataPoint> > *knn = new std::list<std::pair<double,RSGISInterpolatorDataPoint> >();
        try
        {
            double dist = 0.0;
            for(std::vector<RSGISInterpolatorDataPoint>::iterator iterPts = dataPTS->begin(); iterPts != dataPTS->end(); ++iterPts)
            {
                dist = sqrt(pow((eastings-(*iterPts).x),2) + pow((northings-(*iterPts).y),2));
                
                if(knn->empty())
                {
                    knn->push_back(std::pair<double,RSGISInterpolatorDataPoint>(dist, *iterPts));
                }
                else
                {
                    for(std::list<std::pair<double,RSGISInterpolatorDataPoint> >::iterator iterK = knn->begin(); iterK != knn->end(); ++iterK)
                    {
                        if((*iterK).first > dist)
                        {
                            knn->insert(iterK, std::pair<double,RSGISInterpolatorDataPoint>(dist, *iterPts));
                            break;
                        }
                    }
                }
                //std::cout << "knn->size() = " << knn->size() << std::endl;
                if(knn->size() < k)
                {
                    knn->push_back(std::pair<double,RSGISInterpolatorDataPoint>(dist, *iterPts));
                }
                else if(knn->size() > k)
                {
                    knn->pop_back();
                }
            }
        }
        catch(RSGISInterpolationException &e)
        {
            delete knn;
            throw e;
        }
        return knn;
    }
    
    
    
    void RSGIS2DTriagulatorInterpolator::initInterpolator(std::vector<RSGISInterpolatorDataPoint> *pts) throw(RSGISInterpolationException)
    {
        try
        {
            if(pts->size() < 3)
            {
                throw RSGISInterpolationException("Not enough data points, need at least 3.");
            }
            //std::cout << "pts->size() = " << pts->size() << std::endl;
            
            // Check that the points are not on a line
            double meanX = 0;
            double meanY = 0;
            for(std::vector<RSGISInterpolatorDataPoint>::iterator iterPts = pts->begin(); iterPts != pts->end(); ++iterPts)
            {
                meanX += (*iterPts).x;
                meanY += (*iterPts).y;
            }
            meanX = meanX / pts->size();
            meanY = meanY / pts->size();
            //std::cout << "meanX = " << meanX << std::endl;
            //std::cout << "meanY = " << meanY << std::endl;
            
            double varX = 0;
            double varY = 0;
            for(std::vector<RSGISInterpolatorDataPoint>::iterator iterPts = pts->begin(); iterPts != pts->end(); ++iterPts)
            {
                varX += fabs((*iterPts).x - meanX);
                varY += fabs((*iterPts).y - meanY);
            }
            
            varX = fabs(varX / pts->size());
            varY = fabs(varY / pts->size());
            
            //std::cout << "varX = " << varX << std::endl;
            //std::cout << "varY = " << varX << std::endl;
            
            if((varX < 4) | (varY < 4))
            {
                throw RSGISInterpolationException("Data points sit on a line and therefore cannot triangulate.");
            }
            
            dt = new DelaunayTriangulation();
            values = new PointValueMap();
            
            for(std::vector<RSGISInterpolatorDataPoint>::iterator iterPts = pts->begin(); iterPts != pts->end(); ++iterPts)
            {
                K::Point_2 cgalPt((*iterPts).x,(*iterPts).y);
                dt->insert(cgalPt);
                CGALCoordType value = (*iterPts).value;
                values->insert(std::make_pair(cgalPt, value));
            }
        }
        catch(RSGISInterpolationException &e)
        {
            throw e;
        }
        initialised = true;
    }
    
    
    
    
    double RSGISNearestNeighbour2DInterpolator::getValue(double eastings, double northings) throw(RSGISInterpolationException)
    {
        double outValue = std::numeric_limits<double>::signaling_NaN();
		if(initialised)
		{
            CGALPoint p(eastings, northings);
            Vertex_handle vh = dt->nearest_vertex(p);
            CGALPoint nearestPt = vh->point();
            PointValueMap::iterator iterVal = values->find(nearestPt);
            outValue = (*iterVal).second;
		}
		else
		{
			throw RSGISInterpolationException("Interpolated needs to be initialised before values can be retrieved.");
		}
		return outValue;
    }
    
    
    
    
    double RSGISNaturalNeighbor2DInterpolator::getValue(double eastings, double northings) throw(RSGISInterpolationException)
    {
        float outValue = std::numeric_limits<float>::signaling_NaN();
        if(initialised)
        {
            try
            {
                K::Point_2 p(eastings, northings);
                CoordinateVector coords;
                CGAL::Triple<std::back_insert_iterator<CoordinateVector>, K::FT, bool> result = CGAL::natural_neighbor_coordinates_2(*dt, p, std::back_inserter(coords));
                if(!result.third)
                {
                    Vertex_handle vh = dt->nearest_vertex(p);
                    CGALPoint nearestPt = vh->point();
                    PointValueMap::iterator iterVal = values->find(nearestPt);
                    outValue = (*iterVal).second;
                }
                else
                {
                    CGALCoordType norm = result.second;
                    
                    CGALCoordType outDataValue = CGAL::linear_interpolation(coords.begin(), coords.end(), norm, CGAL::Data_access<PointValueMap>(*this->values));
                    
                    outValue = outDataValue;
                }
            }
            catch(RSGISInterpolationException &e)
            {
                throw e;
            }
        }
        return outValue;
    }
    
    
    
    
    double RSGISNaturalNearestNeighbor2DInterpolator::getValue(double eastings, double northings) throw(RSGISInterpolationException)
    {
        float outValue = std::numeric_limits<float>::signaling_NaN();
        if(initialised)
        {
            try
            {
                K::Point_2 p(eastings, northings);
                CoordinateVector coords;
                CGAL::Triple<std::back_insert_iterator<CoordinateVector>, K::FT, bool> result = CGAL::natural_neighbor_coordinates_2(*dt, p, std::back_inserter(coords));
                if(!result.third)
                {
                    outValue = std::numeric_limits<float>::signaling_NaN();
                }
                else
                {
                    CGALCoordType norm = result.second;
                    
                    CGALCoordType outDataValue = CGAL::linear_interpolation(coords.begin(), coords.end(), norm, CGAL::Data_access<PointValueMap>(*this->values));
                    
                    outValue = outDataValue;
                }
            }
            catch(RSGISInterpolationException &e)
            {
                throw e;
            }
        }
        return outValue;
    }
    
    
    
    
    double RSGISKNearestNeighbour2DInterpolator::getValue(double eastings, double northings) throw(RSGISInterpolationException)
    {
        float outValue = std::numeric_limits<float>::signaling_NaN();
        if(initialised)
        {
            try
            {
                std::list<std::pair<double,RSGISInterpolatorDataPoint> > *knn = this->findKNN(eastings, northings);
                /*for(std::list<std::pair<double,RSGISInterpolatorDataPoint> >::iterator iterK = knn->begin(); iterK != knn->end(); ++iterK)
                {
                    std::cout << (*iterK).first << ":\t" << (*iterK).second.value << " [" << (*iterK).second.x << "," << (*iterK).second.y << "]\n";
                }
                std::cout << std::endl;
                 */
                if(knn->size() != this->k)
                {
                    std::cout << "this->k = " << this->k << std::endl;
                    std::cout << "knn->size() = " << knn->size() << std::endl;
                    throw RSGISInterpolationException("Insufficient number of K points where identified.");
                }
                outValue = knn->front().second.value;
                delete knn;
            }
            catch(RSGISInterpolationException &e)
            {
                throw e;
            }
        }
        return outValue;
    }
    
    
    
    
    
    void RSGISAllPointsIDWInterpolator::initInterpolator(std::vector<RSGISInterpolatorDataPoint> *pts) throw(RSGISInterpolationException)
    {
        this->pts = pts;
        this->initialised = true;
    }
    
    double RSGISAllPointsIDWInterpolator::getValue(double eastings, double northings) throw(RSGISInterpolationException)
    {
        float outValue = std::numeric_limits<float>::signaling_NaN();
        if(initialised)
        {
            try
            {
                double totalWeight = 0.0;
                double weight = 0.0;
                for(std::vector<RSGISInterpolatorDataPoint>::iterator iterPts = pts->begin(); iterPts != pts->end(); ++iterPts)
                {
                    weight = 1 / pow(sqrt(pow((eastings - (*iterPts).x), 2) + pow((northings - (*iterPts).y), 2)),(double)this->p);
                    totalWeight += weight;
                }
                
                
                outValue = 0.0;
                
                for(std::vector<RSGISInterpolatorDataPoint>::iterator iterPts = pts->begin(); iterPts != pts->end(); ++iterPts)
                {
                    weight = 1 / pow(sqrt(pow((eastings - (*iterPts).x), 2) + pow((northings - (*iterPts).y), 2)),(double)this->p);
                    
                    outValue += ((*iterPts).value * weight) / totalWeight;
                }
                
            }
            catch(RSGISInterpolationException &e)
            {
                throw e;
            }
        }
        return outValue;
    }
    
    
    
}}


