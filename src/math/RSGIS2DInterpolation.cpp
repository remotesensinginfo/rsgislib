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
            
            double varX = 0;
            double varY = 0;
            for(std::vector<RSGISInterpolatorDataPoint>::iterator iterPts = pts->begin(); iterPts != pts->end(); ++iterPts)
            {
                varX += fabs((*iterPts).x - meanX);
                varY += fabs((*iterPts).y - meanY);
            }
            
            varX = fabs(varX / pts->size());
            varY = fabs(varY / pts->size());
            
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
    
   
    void RSGISLinearTrendInterpolator::initInterpolator(std::vector<RSGISInterpolatorDataPoint> *pts) throw(RSGISInterpolationException)
    {
        RSGISMatrices matrices;
		Matrix *matrixA = NULL;
		Matrix *matrixB = NULL;
		Matrix *matrixCoFactors = NULL;
		Matrix *matrixCoFactorsT = NULL;
		Matrix *outputs = NULL;
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
			
			for(unsigned int i = 0; i < pts->size(); i++)
			{
				sXY += (pts->at(i).x * pts->at(i).y);
				sX += pts->at(i).x;
				sXSqu += (pts->at(i).x * pts->at(i).x);
				sY += pts->at(i).y;
				sYSqu += (pts->at(i).y * pts->at(i).y);
				sXZ += (pts->at(i).x * pts->at(i).value);
				sYZ += (pts->at(i).y * pts->at(i).value);
				sZ += pts->at(i).value;
			}
			
			matrixA = matrices.createMatrix(3, 3);
			matrixA->matrix[0] = sXSqu;
			matrixA->matrix[1] = sXY;
			matrixA->matrix[2] = sX;
			matrixA->matrix[3] = sXY;
			matrixA->matrix[4] = sYSqu;
			matrixA->matrix[5] = sY;
			matrixA->matrix[6] = sX;
			matrixA->matrix[7] = sY;
			matrixA->matrix[8] = pts->size();
			matrixB = matrices.createMatrix(1, 3);
			matrixB->matrix[0] = sXZ;
			matrixB->matrix[1] = sYZ;
			matrixB->matrix[2] = sZ;
			
			double determinantA = matrices.determinant(matrixA);
			matrixCoFactors = matrices.cofactors(matrixA);
			matrixCoFactorsT = matrices.transpose(matrixCoFactors);
			double multiplier = 1/determinantA;
			matrices.multipleSingle(matrixCoFactorsT, multiplier);
			outputs = matrices.multiplication(matrixCoFactorsT, matrixB);
			a = outputs->matrix[0];
			b = outputs->matrix[1];
			c = outputs->matrix[2];
		}
		catch(RSGISMatricesException e)
		{
			if(matrixA != NULL)
			{
                matrices.freeMatrix(matrixA);
			}
			if(matrixB != NULL)
			{
				matrices.freeMatrix(matrixB);
			}
			if(matrixCoFactors != NULL)
			{
				matrices.freeMatrix(matrixCoFactors);
			}
			if(matrixCoFactorsT != NULL)
			{
				matrices.freeMatrix(matrixCoFactorsT);
			}
			if(outputs != NULL)
			{
				matrices.freeMatrix(outputs);
			}
			throw RSGISInterpolationException(e.what());
		}
        catch(RSGISException &e)
        {
            throw RSGISInterpolationException(e.what());
        }
		
        matrices.freeMatrix(matrixA);
        matrices.freeMatrix(matrixB);
        matrices.freeMatrix(matrixCoFactors);
        matrices.freeMatrix(matrixCoFactorsT);
        matrices.freeMatrix(outputs);
    }
    
    double RSGISLinearTrendInterpolator::getValue(double eastings, double northings) throw(RSGISInterpolationException)
    {
        double outVal = 0.0;
        try
        {
            outVal = (a * eastings) + (b * northings) + c;
        }
        catch(std::exception &e)
        {
            throw RSGISInterpolationException(e.what());
        }
        catch(RSGISInterpolationException &e)
        {
            throw e;
        }
        
        return outVal;
    }
    
    
    

    void RSGISCombine2DInterpolators::initInterpolator(std::vector<RSGISInterpolatorDataPoint> *pts) throw(RSGISInterpolationException)
    {
        try
        {
            double valSum = 0.0;
            for(std::vector<RSGISInterpolatorDataPoint>::iterator iterPts = pts->begin(); iterPts != pts->end(); ++iterPts)
            {
                valSum += (*iterPts).value;
            }
            double meanVal = valSum / pts->size();
            
            double valDiffSum = 0.0;
            for(std::vector<RSGISInterpolatorDataPoint>::iterator iterPts = pts->begin(); iterPts != pts->end(); ++iterPts)
            {
                valDiffSum += (((*iterPts).value - meanVal) * ((*iterPts).value - meanVal));
            }
            double stdDevVal = sqrt(valDiffSum / pts->size());
            
            this->upperThres = meanVal + (stdDevVal * stdDevThres);
            this->lowerThres = meanVal - (stdDevVal * stdDevThres);
            
            std::cout << "Upper = " << upperThres << std::endl;
            std::cout << "Lower = " << lowerThres << std::endl;
            
            this->interp1->initInterpolator(pts);
            this->interp2->initInterpolator(pts);
            this->init = true;
        }
        catch(std::exception &e)
        {
            throw RSGISInterpolationException(e.what());
        }
        catch(RSGISInterpolationException &e)
        {
            throw e;
        }
    }
    
    double RSGISCombine2DInterpolators::getValue(double eastings, double northings) throw(RSGISInterpolationException)
    {
        double outVal = 0.0;
        try
        {
            outVal = this->interp1->getValue(eastings, northings);
            
            if((outVal > upperThres) | (outVal < lowerThres))
            {
                outVal = this->interp2->getValue(eastings, northings);
            }
            
        }
        catch(std::exception &e)
        {
            throw RSGISInterpolationException(e.what());
        }
        catch(RSGISInterpolationException &e)
        {
            throw e;
        }
        
        return outVal;
    }
    
    
}}


