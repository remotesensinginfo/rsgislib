/*
 *  RSGISFitPolygon2Points.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/06/2015.
 *  Copyright 2015 RSGISLib.
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

#include "RSGISFitPolygon2Points.h"

namespace rsgis{namespace geom{
    
    
    RSGISFitAlphaShapesPolygonToPoints::RSGISFitAlphaShapesPolygonToPoints()
    {
        
    }
    
    OGRPolygon* RSGISFitAlphaShapesPolygonToPoints::fitPolygon(std::vector<OGRPoint*> *pts, double alphaVal) throw(RSGISGeometryException)
    {
        OGRPolygon *poly = new OGRPolygon();
        try
        {
            std::cout << "Create CGAL Points\n";
            std::vector<K::Point_2> points;
            for(std::vector<OGRPoint*>::iterator iterPts = pts->begin(); iterPts != pts->end(); ++iterPts)
            {
                K::Point_2 cgalPt((*iterPts)->getX(),(*iterPts)->getY());
                points.push_back(cgalPt);
            }
            std::cout << "Computer Alpha Shape\n";
            Alpha_shape_2 A(points.begin(), points.end(), FT(0), Alpha_shape_2::REGULARIZED);
            
            if(alphaVal < 0)
            {
                alphaVal = *A.find_optimal_alpha(1);
                std::cout << "Optimal Alpha = " << alphaVal << std::endl;
            }
            
            A.set_alpha(alphaVal);
            
            std::cout << "Export Segments\n";
            std::vector<K::Segment_2> segments;
            std::cout.precision(15);
            //OGRLinearRing *ogrRing = new OGRLinearRing();
            //bool first = true;
            //double firstX = 0.0;
            //double firstY = 0.0;
            std::vector<geos::geom::LineSegment *> *lines = new std::vector<geos::geom::LineSegment *>();
            for(Alpha_shape_edges_iterator it =  A.alpha_shape_edges_begin(); it != A.alpha_shape_edges_end(); ++it)
            {
                Alpha_shape_2::Classification_type edgeType = A.classify(*it, alphaVal);
                
                K::Point_2 ptSource = A.segment(*it).source();
                K::Point_2 ptTarget = A.segment(*it).target();
                
                if(edgeType == Alpha_shape_2::REGULAR)
                {
                    lines->push_back(new geos::geom::LineSegment(geos::geom::Coordinate(ptSource.x(), ptSource.y()), geos::geom::Coordinate(ptTarget.x(), ptTarget.y())));
                    std::cout << "Line Regular: [" << ptSource.x() << ", " << ptSource.y() << "][" << ptTarget.x() << ", " << ptTarget.y() << "]\n";
                }
                else if(edgeType == Alpha_shape_2::INTERIOR)
                {
                    std::cout << "Line Interior: [" << ptSource.x() << ", " << ptSource.y() << "][" << ptTarget.x() << ", " << ptTarget.y() << "]\n";
                }
                
                
                
                /*
                if(first)
                {
                    firstX = ptSource.x();
                    firstY = ptSource.y();
                    ogrRing->addPoint(ptSource.x(), ptSource.y(), 0);
                    ogrRing->addPoint(ptTarget.x(), ptTarget.y(), 0);
                    first = false;
                }
                else
                {
                    ogrRing->addPoint(ptTarget.x(), ptTarget.y(), 0);
                }
                 */
            }
            RSGISGeomTestExport geomExport;
            geomExport.exportGEOSLineSegments2SHP("./AlphaShapesLines.shp", true, lines);
            delete lines;
            
        }
        catch (RSGISGeometryException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw RSGISGeometryException(e.what());
        }
        
        return poly;
    }
    
    RSGISFitAlphaShapesPolygonToPoints::~RSGISFitAlphaShapesPolygonToPoints()
    {
        
    }
    
}}



