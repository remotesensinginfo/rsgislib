/*
 *  RSGISFitActiveContour4Polys.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/09/2016.
 *  Copyright 2016 RSGISLib.
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

#ifndef RSGISFitActiveContour4Polys_H
#define RSGISFitActiveContour4Polys_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#include "common/RSGISImageException.h"
#include "common/RSGISVectorException.h"

#include "vec/RSGISProcessOGRGeometry.h"
#include "vec/RSGISProcessGeometry.h"

#include "gdal_priv.h"

namespace rsgis{namespace vec{
    
    struct RSGISACCoordFit
    {
        RSGISACCoordFit(){};
        RSGISACCoordFit(double x, double y)
        {
            this->x = x;
            this->y = y;
        };
        double x;
        double y;
        double extEnergy;
        double intEnergy;
        double totalEnergy;
        
        bool operator==(RSGISACCoordFit coord)
        {
            return (coord.x == x) & (coord.y == y);
        };
    };
    
    
    class DllExport RSGISFitActiveContour2Geoms
    {
    public:
        RSGISFitActiveContour2Geoms();
        
        void fitActiveContours2Polys(OGRLayer *inputOGRLayer, OGRLayer *outputOGRLayer, GDALDataset *externalForceImg, double interAlpha, double interBeta, double extGamma, double minExtThres, bool force)throw(rsgis::RSGISVectorException);
        
        ~RSGISFitActiveContour2Geoms();
    };
    
    
    class DllExport RSGISFitActiveContourProcessOGRGeometry: public RSGISProcessOGRGeometry
    {
    public:
        RSGISFitActiveContourProcessOGRGeometry(GDALDataset *externalForceImg, double interAlpha, double interBeta, double extGamma, double minExtThres);
        void processGeometry(OGRPolygon *polygon) throw(RSGISVectorException){throw RSGISVectorException("RSGISFitActiveContourProcessOGRGeometry: Function not implemented");};
        void processGeometry(OGRMultiPolygon *multiPolygon) throw(RSGISVectorException){throw RSGISVectorException("RSGISFitActiveContourProcessOGRGeometry: Function not implemented");};
        void processGeometry(OGRPoint *point) throw(RSGISVectorException){throw RSGISVectorException("RSGISFitActiveContourProcessOGRGeometry: Function not implemented");};
        void processGeometry(OGRLineString *line) throw(RSGISVectorException){throw RSGISVectorException("RSGISFitActiveContourProcessOGRGeometry: Function not implemented");};
        OGRPolygon* processGeometry(OGRGeometry *geom) throw(RSGISVectorException);
        ~RSGISFitActiveContourProcessOGRGeometry();
    protected:
        RSGISVectorUtils *vecUtils;
        GDALDataset *externalForceImg;
        double interAlpha;
        double interBeta;
        double extGamma;
        double minExtThres;
        geos::geom::Envelope polyBoundary;
    };
    
    
    
    
    class DllExport RSGISFitActiveContour2Coords
    {
    public:
        RSGISFitActiveContour2Coords();
        void fitActiveCountour(std::vector<RSGISACCoordFit> *coords, GDALDataset *externalForceImg, double interAlpha, double interBeta, double extGamma, double minExtThres, unsigned int maxNumIters, float propChanges2Term)throw(rsgis::RSGISVectorException);
        ~RSGISFitActiveContour2Coords();
    protected:
        void calcUpdateInternalEnergies(double interAlpha, double interBeta, RSGISACCoordFit *c, RSGISACCoordFit *next, RSGISACCoordFit *prev, RSGISACCoordFit *next1, RSGISACCoordFit *prev1, double *cCInt, double *cNInt, double *cPInt) throw(RSGISVectorException);
        double calcDist(RSGISACCoordFit *a, RSGISACCoordFit *b);
        void findPointOnLine(RSGISACCoordFit *p1, RSGISACCoordFit *p2, double distance, RSGISACCoordFit *p3);
        double calcPointStiffnesss(RSGISACCoordFit *p1, RSGISACCoordFit *p2, RSGISACCoordFit *stiffPt) throw(RSGISVectorException);
    };
    
}}

#endif





