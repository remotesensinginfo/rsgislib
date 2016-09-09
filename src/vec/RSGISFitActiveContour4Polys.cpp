/*
 *  RSGISFitActiveContour4Polys.cpp
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

#include "RSGISFitActiveContour4Polys.h"

namespace rsgis{namespace vec{
    
    RSGISFitActiveContour2Geoms::RSGISFitActiveContour2Geoms()
    {
        
    }
    
    void RSGISFitActiveContour2Geoms::fitActiveContours2Polys(OGRLayer *inputOGRLayer, OGRLayer *outputOGRLayer, GDALDataset *externalForceImg, double interAlpha, double interBeta, double extGamma, double minExtThres, bool force)throw(rsgis::RSGISVectorException)
    {
        try
        {
            RSGISFitActiveContourProcessOGRGeometry fitContour = RSGISFitActiveContourProcessOGRGeometry(externalForceImg, interAlpha, interBeta, extGamma, minExtThres);
            RSGISProcessGeometry processGeoms = RSGISProcessGeometry(&fitContour);
            processGeoms.processGeometryPolygonOutput(inputOGRLayer, outputOGRLayer, true, true);//DEBUGGING change to false (last input)
        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw e;
        }
        catch(RSGISImageException &e)
        {
            throw rsgis::RSGISVectorException(e.what());
        }
        catch(RSGISException &e)
        {
            throw rsgis::RSGISVectorException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::RSGISVectorException(e.what());
        }
    }
    
    
    RSGISFitActiveContour2Geoms::~RSGISFitActiveContour2Geoms()
    {
        
    }
    
    
    
    
    RSGISFitActiveContourProcessOGRGeometry::RSGISFitActiveContourProcessOGRGeometry(GDALDataset *externalForceImg, double interAlpha, double interBeta, double extGamma, double minExtThres):RSGISProcessOGRGeometry()
    {
        vecUtils = new RSGISVectorUtils();
        this->externalForceImg = externalForceImg;
        this->interAlpha = interAlpha;
        this->interBeta = interBeta;
        this->extGamma = extGamma;
        this->minExtThres = minExtThres;
        
        double *trans = new double[6];
        this->externalForceImg->GetGeoTransform(trans);
        this->polyBoundary.init(trans[0], trans[0]+(externalForceImg->GetRasterXSize()*trans[1]), trans[3]+(externalForceImg->GetRasterYSize()*trans[5]), trans[3]);
        delete[] trans;
    }
    
    OGRPolygon* RSGISFitActiveContourProcessOGRGeometry::processGeometry(OGRGeometry *geom) throw(RSGISVectorException)
    {
        OGRPolygon *ogrNewPoly = NULL;
        try
        {
            if( geom != NULL && wkbFlatten(geom->getGeometryType()) == wkbPolygon )
            {
                std::cout << "Process Polygon\n";
                OGRPolygon *polygon = (OGRPolygon *) geom;
                OGRLinearRing *polyRing = polygon->getExteriorRing();
                
                int numPts = polyRing->getNumPoints();
                std::cout << "Number of Points : " << numPts << std::endl;
                std::vector<RSGISACCoordFit> *coords = new std::vector<RSGISACCoordFit>();
                for(int i = 0; i < numPts; ++i)
                {
                    //std::cout << "\t pt " << i << ": [" << polyRing->getX(i) << ", " << polyRing->getY(i) << "]" << std::endl;
                    coords->push_back(RSGISACCoordFit(polyRing->getX(i), polyRing->getY(i)));
                }
                
                /*
                RSGISFitActiveContourOptimiseFunc optimiseFunc = RSGISFitActiveContourOptimiseFunc(this->externalForceImg, interAlpha, interBeta, interGamma);
                rsgis::math::RSGISGlobalHillClimbingOptimiser2DVaryNumPts optimise = rsgis::math::RSGISGlobalHillClimbingOptimiser2DVaryNumPts(&optimiseFunc, false, 10);
                std::vector<geos::geom::Coordinate*> *newCoords = optimise.optimise4Neighbor(&coords, 50, &polyBoundary);
                */
                
                RSGISFitActiveContour2Coords fitcontour;
                fitcontour.fitActiveCountour(coords, externalForceImg, interAlpha, interBeta, extGamma, minExtThres, 100, 0.1);
                
                ogrNewPoly = new OGRPolygon();
                OGRLinearRing *ogrRing = new OGRLinearRing();
                
                for(std::vector<RSGISACCoordFit>::iterator iterCoords = coords->begin(); iterCoords != coords->end(); ++iterCoords)
                {
                    ogrRing->addPoint((*iterCoords).x, (*iterCoords).y, 0.0);
                }
                delete coords;
                
                ogrRing->closeRings();
                ogrNewPoly->addRingDirectly(ogrRing);
                /*
                int numOfHoles = polygon->getNumInteriorRings();
                std::cout << "Number of holes : " << numOfHoles << std::endl;
                if(numOfHoles > 0)
                {
                    // Process holes as seperate polygon
                    for(int i = 0; i < numOfHoles; ++i)
                    {
                        OGRLinearRing *hole = polygon->getInteriorRing(i);
                        OGRPolygon *ogrTmpPoly = new OGRPolygon();
                        ogrTmpPoly->addRingDirectly((OGRLinearRing*)hole->clone());
                        OGRPolygon *ogrTmpPoly2 = this->processGeometry((OGRGeometry*)ogrTmpPoly);
                        if(ogrNewPoly->Contains(ogrTmpPoly2))
                        {
                            ogrNewPoly->addRingDirectly((OGRCurve*)ogrTmpPoly2->getExteriorRing());
                        }
                        else
                        {
                            delete ogrTmpPoly2;
                        }
                        delete ogrTmpPoly;
                    }
                }
                 */
            }
            else
            {
                throw RSGISVectorException("Input vector must only contain polygons (multi-polygons are also not supported).");
            }
        }
        catch(RSGISVectorException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw rsgis::RSGISVectorException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::RSGISVectorException(e.what());
        }
        return ogrNewPoly;
    }
    
    RSGISFitActiveContourProcessOGRGeometry::~RSGISFitActiveContourProcessOGRGeometry()
    {
        delete vecUtils;
    }
    
    
    
    
    RSGISFitActiveContour2Coords::RSGISFitActiveContour2Coords()
    {
        
    }
    
    void RSGISFitActiveContour2Coords::fitActiveCountour(std::vector<RSGISACCoordFit> *coords, GDALDataset *externalForceImg, double interAlpha, double interBeta, double extGamma, double minExtThres, unsigned int maxNumIters, float propChanges2Term)throw(rsgis::RSGISVectorException)
    {
        try
        {
            GDALRasterBand *extImgBand = externalForceImg->GetRasterBand(1);
            unsigned int imgWidth = extImgBand->GetXSize();
            unsigned int imgHeight = extImgBand->GetYSize();
            
            double *trans = new double[6];
            externalForceImg->GetGeoTransform(trans);
            double imgRes = trans[1];
            
            double bXMin = trans[0];
            double bXMax = trans[0]+(imgWidth*imgRes);
            double bYMin = trans[3]-(imgHeight*imgRes);
            double bYMax = trans[3];
            delete[] trans;
            
            double step = imgRes;
            
            bool first = true;
            double distance = 0.0;
            RSGISACCoordFit coord1;
            RSGISACCoordFit coord2;
            // Check initial coordinates are not too close and that the gaps are not too big.
            for(std::vector<RSGISACCoordFit>::iterator iterCoords = coords->begin(); iterCoords != coords->end(); )
            {
                if(first)
                {
                    coord1.x = (*iterCoords).x;
                    coord1.y = (*iterCoords).y;
                    first = false;
                }
                else
                {
                    distance = this->calcDist(&(*iterCoords), &coord1);
                    if((distance < 0.5) & (coords->size() > 3))
                    {
                        // Removed Coordinate
                        coords->erase(iterCoords);
                    }
                    else if(distance > (step*3))
                    {
                        // Add Coordinate
                        this->findPointOnLine(&coord1, &(*iterCoords), (distance/2), &coord2);
                        iterCoords = coords->insert(iterCoords,RSGISACCoordFit(coord2.x, coord2.y));
                    }
                    else 
                    {
                        // Continue
                        coord1.x = (*iterCoords).x;
                        coord1.y = (*iterCoords).y;
                        ++iterCoords;
                    }
                }					
            }
            
            // Initial all the coordinates
            RSGISACCoordFit next;
            RSGISACCoordFit prev;
            RSGISACCoordFit next1;
            RSGISACCoordFit prev1;
            int xPxl = 0;
            int yPxl = 0;
            float dataVal;
            double externalEnergy = 0.0;
            double internalEnergy = 0.0;
            double totalEnergy = 0.0;
            double currentNextDist = 0.0;
            double currentPrevDist = 0.0;
            double elasEnergy = 0.0;
            double stiffEnergy = 0.0;
            for(std::vector<RSGISACCoordFit>::iterator iterCoords = coords->begin(); iterCoords != coords->end(); ++iterCoords)
            {
                if(iterCoords == coords->begin())
                {
                    next = (*(iterCoords+1));
                    prev = coords->back();
                }
                else if(*iterCoords == coords->back())
                {
                    next = coords->front();
                    prev = (*(iterCoords-1));
                }
                else
                {
                    next = (*(iterCoords+1));
                    prev = (*(iterCoords-1));
                }
                
                
                yPxl = floor((bYMax - (*iterCoords).y)/imgRes)+0.5;
                xPxl = floor(((*iterCoords).x - bXMin)/imgRes)+0.5;
                
                if((yPxl < 0) | (yPxl >= imgHeight))
                {
                    std::cout << "Image Height: " << imgHeight << std::endl;
                    std::cout << "Image Width: " << imgWidth << std::endl;
                    std::cout << "XY = [" << xPxl << ", " << yPxl << std::endl;
                    throw rsgis::RSGISVectorException("Y Does not fit within the image");
                }
                
                if((xPxl < 0) | (xPxl >= imgWidth))
                {
                    std::cout << "Image Height: " << imgHeight << std::endl;
                    std::cout << "Image Width: " << imgWidth << std::endl;
                    std::cout << "XY = [" << xPxl << ", " << yPxl << std::endl;
                    throw rsgis::RSGISVectorException("X Does not fit within the image");
                }
                
                extImgBand->RasterIO(GF_Read, xPxl, yPxl, 1, 1, &dataVal, 1, 1, GDT_Float32, 0, 0);
                (*iterCoords).extEnergy = (dataVal * extGamma);
                externalEnergy += (*iterCoords).extEnergy;
                
                currentNextDist = this->calcDist(&next, &(*iterCoords));
                currentPrevDist = this->calcDist(&prev, &(*iterCoords));
                elasEnergy = interAlpha * ((currentNextDist + currentPrevDist)/2);
                //std::cout << "Elastic Energy: " << elasEnergy << std::endl;
                
                stiffEnergy = interBeta * this->calcPointStiffnesss(&prev, &next, &(*iterCoords));
                //std::cout << "Stiffness Energy: " << stiffEnergy << std::endl;
                
                (*iterCoords).intEnergy = elasEnergy + stiffEnergy;
                internalEnergy += (*iterCoords).intEnergy;
            }
            
            //std:: cout << "Internal Energy: " << internalEnergy << std::endl;
            //std:: cout << "External Energy: " << externalEnergy << std::endl;
            totalEnergy = internalEnergy + externalEnergy;
            
            
            std::cout << "Started " << std::flush;
            double tmpInternalEnergy = 0.0;
            double tmpExternalEnergy = 0.0;
            double tmpTotalEnergy = 0.0;
            double minTotalEnergy = 0.0;
            
            double minCExt = 0.0;
            double minCInt = 0.0;
            double minNInt = 0.0;
            double minPInt = 0.0;
            
            double cCExt = 0.0;
            double cCInt = 0.0;
            double cNInt = 0.0;
            double cPInt = 0.0;
            
            unsigned int numChangePts = 0;
            double propChange = 1.0;
            unsigned int nIter = 0;
            int selectIdx = 0;
            bool change = true;
            while(change)
            {
                if(nIter == maxNumIters)
                {
                    std::cout << " (Max. Iterations)" << std::flush;
                    break;
                }
                if(propChange < propChanges2Term)
                {
                    std::cout << " (Proportion Change)" << std::flush;
                    break;
                }
                std::cout << "." << std::flush;
                numChangePts = 0;
                propChange = 0.0;
                change = false;
                
                for(std::vector<RSGISACCoordFit>::iterator iterCoords = coords->begin(); iterCoords != coords->end(); ++iterCoords)
                {
                    // Get the points either side of current point
                    if(iterCoords == coords->begin())
                    {
                        next = (*(iterCoords+1));
                        next1 = (*(iterCoords+2));
                        prev = coords->back();
                        prev1 = coords->at(coords->size()-2);
                    }
                    else if(*iterCoords == coords->back())
                    {
                        prev1 = (*(iterCoords-2));
                        prev = (*(iterCoords-1));
                        next = coords->front();
                        next1 = coords->at(1);
                        
                    }
                    else
                    {
                        next = (*(iterCoords+1));
                        prev = (*(iterCoords-1));
                        
                        // Get Next 1 (i.e., one after next)
                        if(next == coords->back())
                        {
                            next1 = coords->front();
                        }
                        else
                        {
                            next1 = (*(iterCoords+2));
                        }
                        
                        // Get Prex 1 (i.e., one before prev)
                        if(prev == *coords->begin())
                        {
                            prev1 = coords->back();
                        }
                        else
                        {
                            prev1 = (*(iterCoords-2));
                        }
                    }
                    
                    
                    
                    
                    coord1.x = (*iterCoords).x;
                    coord1.y = (*iterCoords).y;
                    
                    tmpInternalEnergy = internalEnergy - prev.intEnergy - (*iterCoords).intEnergy - next.intEnergy;
                    tmpExternalEnergy = externalEnergy - (*iterCoords).extEnergy;
                    
                    first = true;
                    for(int i = 0; i < 8; ++i)
                    {
                        //std::cout << " i  = " << i << std::endl;
                        if(i == 0)
                        {
                            coord1.x = (*iterCoords).x - step;
                            coord1.y = (*iterCoords).y;
                        }
                        else if(i == 1)
                        {
                            coord1.x = (*iterCoords).x + step;
                            coord1.y = (*iterCoords).y;
                        }
                        else if(i == 2)
                        {
                            coord1.x = (*iterCoords).x;
                            coord1.y = (*iterCoords).y - step;
                        }
                        else if(i == 3)
                        {
                            coord1.x = (*iterCoords).x;
                            coord1.y = (*iterCoords).y + step;
                        }
                        else if(i == 4)
                        {
                            coord1.x = (*iterCoords).x + step;
                            coord1.y = (*iterCoords).y + step;
                        }
                        else if(i == 5)
                        {
                            coord1.x = (*iterCoords).x + step;
                            coord1.y = (*iterCoords).y - step;
                        }
                        else if(i == 6)
                        {
                            coord1.x = (*iterCoords).x - step;
                            coord1.y = (*iterCoords).y - step;
                        }
                        else if(i == 7)
                        {
                            coord1.x = (*iterCoords).x - step;
                            coord1.y = (*iterCoords).y + step;
                        }
                        
                        try
                        {
                            if(((coord1.x > bXMin) & (coord1.x < bXMax)) & ((coord1.y > bYMin) & (coord1.y < bYMax)))
                            {
                                yPxl = floor((bYMax - coord1.y)/imgRes)+0.5;
                                xPxl = floor((coord1.x - bXMin)/imgRes)+0.5;
                                
                                if((yPxl < 0) | (yPxl >= imgHeight))
                                {
                                    std::cout << "Image Height: " << imgHeight << std::endl;
                                    std::cout << "Image Width: " << imgWidth << std::endl;
                                    std::cout << "XY = [" << xPxl << ", " << yPxl << std::endl;
                                    throw rsgis::RSGISVectorException("Y Does not fit within the image");
                                }
                                
                                if((xPxl < 0) | (xPxl >= imgWidth))
                                {
                                    std::cout << "Image Height: " << imgHeight << std::endl;
                                    std::cout << "Image Width: " << imgWidth << std::endl;
                                    std::cout << "XY = [" << xPxl << ", " << yPxl << std::endl;
                                    throw rsgis::RSGISVectorException("X Does not fit within the image");
                                }
                                
                                extImgBand->RasterIO(GF_Read, xPxl, yPxl, 1, 1, &dataVal, 1, 1, GDT_Float32, 0, 0);
                                cCExt = (dataVal * extGamma);
                                
                                //std::cout << "\tExternal Energy = " << cCExt << "(" << (*iterCoords).extEnergy << ")\n";
                                
                                this->calcUpdateInternalEnergies(interAlpha, interBeta, &coord1, &next, &prev, &next1, &prev1, &cCInt, &cNInt, &cPInt);
                                
                                //std::cout << "\tInternal Energy = " << cCInt << "(" << (*iterCoords).intEnergy << ")\n";
                                
                                tmpTotalEnergy = (tmpExternalEnergy + cCExt) + (tmpInternalEnergy + cCInt + cNInt + cPInt);
                            }
                            else
                            {
                                tmpTotalEnergy = -1;
                            }
                        }
                        catch (RSGISException &e)
                        {
                            tmpTotalEnergy = -1;
                        }
                        //std::cout << "\tTmp Total Energy: " << tmpTotalEnergy << "(" << totalEnergy << ")" << std::endl;
                        
                        if(tmpTotalEnergy != -1)
                        {
                            if(first)
                            {
                                minTotalEnergy = tmpTotalEnergy;
                                selectIdx = i;
                                minCExt = cCExt;
                                minCInt = cCInt;
                                minNInt = cNInt;
                                minPInt = cPInt;
                                first = false;
                            }
                            else if(tmpTotalEnergy < minTotalEnergy)
                            {
                                minTotalEnergy = tmpTotalEnergy;
                                selectIdx = i;
                                minCExt = cCExt;
                                minCInt = cCInt;
                                minNInt = cNInt;
                                minPInt = cPInt;
                            }
                        }
                    }
                    
                    if(minTotalEnergy < totalEnergy)
                    {
                        
                        if(selectIdx == 0)
                        {
                            (*iterCoords).x = (*iterCoords).x - step;
                        }
                        else if(selectIdx == 1)
                        {
                            (*iterCoords).x = (*iterCoords).x + step;
                        }
                        else if(selectIdx == 2)
                        {
                            (*iterCoords).y = (*iterCoords).y - step;
                        }
                        else if(selectIdx == 3)
                        {
                            (*iterCoords).y = (*iterCoords).y + step;
                        }
                        else if(selectIdx == 4)
                        {
                            (*iterCoords).x = (*iterCoords).x + step;
                            (*iterCoords).y = (*iterCoords).y + step;
                        }
                        else if(selectIdx == 5)
                        {
                            (*iterCoords).x = (*iterCoords).x + step;
                            (*iterCoords).y = (*iterCoords).y - step;
                        }
                        else if(selectIdx == 6)
                        {
                            (*iterCoords).x = (*iterCoords).x - step;
                            (*iterCoords).y = (*iterCoords).y - step;
                        }
                        else if(selectIdx == 7)
                        {
                            (*iterCoords).x = (*iterCoords).x - step;
                            (*iterCoords).y = (*iterCoords).y + step;
                        }
                        
                        (*iterCoords).extEnergy = minCExt;
                        (*iterCoords).intEnergy = minCInt;
                        next.intEnergy = minNInt;
                        prev.intEnergy = minPInt;
                        
                        internalEnergy = tmpInternalEnergy + minCInt + minNInt + minPInt;
                        externalEnergy = tmpExternalEnergy + minCExt;
                        
                        totalEnergy = internalEnergy + externalEnergy;
                        
                        change = true;
                        ++numChangePts;
                    }
                }
                
                propChange = (double)numChangePts / (double)coords->size();
                
                //std::cout << "Proportion of Change = " << propChange << std::endl;
                
                
                // Check the coordinates are not too close and that the gaps are not too big.
                for(std::vector<RSGISACCoordFit>::iterator iterCoords = coords->begin(); iterCoords != coords->end(); )
                {
                    if(first)
                    {
                        coord1.x = (*iterCoords).x;
                        coord1.y = (*iterCoords).y;
                        first = false;
                    }
                    else
                    {
                        distance = this->calcDist(&(*iterCoords), &coord1);
                        if((distance < 0.5) & (coords->size() > 3))
                        {
                            // Removed Coordinate
                            coords->erase(iterCoords);
                        }
                        else if(distance > (step*3))
                        {
                            // Add Coordinate
                            this->findPointOnLine(&coord1, &(*iterCoords), (distance/2), &coord2);
                            iterCoords = coords->insert(iterCoords,RSGISACCoordFit(coord2.x, coord2.y));
                        }
                        else
                        {
                            // Continue
                            coord1.x = (*iterCoords).x;
                            coord1.y = (*iterCoords).y;
                            ++iterCoords;
                        }
                    }					
                }
                ++nIter;
            }
            std::cout << " Completed\n";
        }
        catch(RSGISVectorException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw rsgis::RSGISVectorException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::RSGISVectorException(e.what());
        }
    }
    
    void RSGISFitActiveContour2Coords::calcUpdateInternalEnergies(double interAlpha, double interBeta, RSGISACCoordFit *c, RSGISACCoordFit *next, RSGISACCoordFit *prev, RSGISACCoordFit *next1, RSGISACCoordFit *prev1, double *cCInt, double *cNInt, double *cPInt) throw(RSGISVectorException)
    {
        double dist1 = this->calcDist(c, next);
        double dist2 = this->calcDist(c, prev);
        double elasEnergy = interAlpha * ((dist1 + dist2)/2);
        //std::cout << "Elastic Energy: " << elasEnergy << std::endl;
        double stiffEnergy = interBeta * this->calcPointStiffnesss(prev, next, c);
        //std::cout << "Stiffness Energy: " << stiffEnergy << std::endl;
        *cCInt = elasEnergy + stiffEnergy;
        
        dist1 = this->calcDist(next, next1);
        dist2 = this->calcDist(next, c);
        elasEnergy = interAlpha * ((dist1 + dist2)/2);
        //std::cout << "Elastic Energy: " << elasEnergy << std::endl;
        stiffEnergy = interBeta * this->calcPointStiffnesss(c, next1, next);
        //std::cout << "Stiffness Energy: " << stiffEnergy << std::endl;
        *cNInt = elasEnergy + stiffEnergy;
        
        dist1 = this->calcDist(prev, prev1);
        dist2 = this->calcDist(prev, c);
        elasEnergy = interAlpha * ((dist1 + dist2)/2);
        //std::cout << "Elastic Energy: " << elasEnergy << std::endl;
        stiffEnergy = interBeta * this->calcPointStiffnesss(prev1, c, prev);
        //std::cout << "Stiffness Energy: " << stiffEnergy << std::endl;
        *cPInt = elasEnergy + stiffEnergy;
    }
    
    
    double RSGISFitActiveContour2Coords::calcDist(RSGISACCoordFit *a, RSGISACCoordFit *b)
    {
        return sqrt(((a->x - b->x)*(a->x - b->x)) + ((a->y - b->y)*(a->y - b->y)));
    }
    
    void RSGISFitActiveContour2Coords::findPointOnLine(RSGISACCoordFit *p1, RSGISACCoordFit *p2, double distance, RSGISACCoordFit *p3)
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
    }
    
    
    double RSGISFitActiveContour2Coords::calcPointStiffnesss(RSGISACCoordFit *p1, RSGISACCoordFit *p2, RSGISACCoordFit *stiffPt)throw(RSGISVectorException)
    {
        double distance = this->calcDist(p1, stiffPt);
        if(distance == 0)
        {
            throw RSGISVectorException("Points are too close together to fit a line.");
        }
        double dx = p2->x - p1->x;
        double dy = p2->y - p1->y;
        double theta = atan(dy/dx);
        double y1 = distance * sin(theta);
        double x1 = distance * cos(theta);
        
        RSGISACCoordFit tmpPt;
        
        if((dx >= 0) & (dy > 0))
        {
            tmpPt.x = p1->x + x1;
            tmpPt.y = p1->y + y1;
        }
        else if((dx >= 0) & (dy <= 0))
        {
            tmpPt.x = p1->x + x1;
            tmpPt.y = p1->y + y1;
        }
        else if((dx < 0) & (dy > 0))
        {
            tmpPt.x = p1->x - x1;
            tmpPt.y = p1->y - y1;
        }
        else if((dx < 0) & (dy <= 0))
        {
            tmpPt.x = p1->x - x1;
            tmpPt.y = p1->y - y1;
        }
        
        return calcDist(stiffPt, &tmpPt);
    }
    
    
    
    RSGISFitActiveContour2Coords::~RSGISFitActiveContour2Coords()
    {
        
    }
    
    
    
    
    
    /*
    
    
    RSGISFitActiveContourOptimiseFunc::RSGISFitActiveContourOptimiseFunc(GDALDataset *extImg, double alpha, double beta, double gamma)
    {
        this->extImgBand = extImg->GetRasterBand(1);
        this->imgWidth = extImgBand->GetXSize();
        this->imgHeight = extImgBand->GetYSize();
        
        double *trans = new double[6];
        extImg->GetGeoTransform(trans);
        this->imgRes = trans[1];
        
        xMin = trans[0];
        xMax = trans[0]+(imgWidth*imgRes);
        yMin = trans[3]-(imgHeight*imgRes);
        yMax = trans[3];
        delete[] trans;
        
        this->alpha = alpha;
        this->beta = beta;
        this->gamma = gamma;
    }
    
    double RSGISFitActiveContourOptimiseFunc::calcValue(std::vector<geos::geom::Coordinate*> *coords) throw(rsgis::math::RSGISOptimisationException)
    {
        double energyValue = 0;
        double internalEnergy = 0;
        double externalEnergy = 0;
        double elasEnergy = 0;
        double currentNextDist = 0;
        double currentPrevDist = 0;
        double stiffEnergy = 0;
        double currentNextStiffX = 0;
        double currentNextStiffY = 0;
        double currentNextStiffPart = 0;
        
        int y = 0;
        int x = 0;
        
        int nodeCount = 0;
        
        float dataVal = 0;
        
        geos::geom::Coordinate *prev;
        geos::geom::Coordinate *next;
        geos::geom::Coordinate *current2 = new geos::geom::Coordinate();
        
        std::vector<geos::geom::Coordinate*>::iterator iterCoords;
        for(iterCoords = coords->begin(); iterCoords != coords->end(); ++iterCoords)
        {
            
            if(iterCoords == coords->begin())
            {
                next = (*(iterCoords+1));
                prev = coords->back();
            }
            else if(*iterCoords == coords->back())
            {
                next = coords->front();
                prev = (*(iterCoords-1));
            }
            else
            {
                next = (*(iterCoords+1));
                prev = (*(iterCoords-1));
            }
            
            y = floor((yMax - (*iterCoords)->y)/imgRes)+0.5;
            x = floor(((*iterCoords)->x - xMin)/imgRes)+0.5;
            
            if((y < 0) | (y >= imgHeight))
            {
                std::cout << "Image Height: " << imgHeight << std::endl;
                std::cout << "Image Width: " << imgWidth << std::endl;
                std::cout << "XY = [" << x << ", " << y << std::endl;
                throw rsgis::math::RSGISOptimisationException("Y Does not fit within the image");
            }
            
            if((x < 0) | (x >= imgWidth))
            {
                std::cout << "Image Height: " << imgHeight << std::endl;
                std::cout << "Image Width: " << imgWidth << std::endl;
                std::cout << "XY = [" << x << ", " << y << std::endl;
                throw rsgis::math::RSGISOptimisationException("X Does not fit within the image");
            }
            
            this->extImgBand->RasterIO(GF_Read, x, y, 1, 1, &dataVal, 1, 1, GDT_Float32, 0, 0);
            externalEnergy += (dataVal * gamma);

            currentNextDist = sqrt(((next->x - (*iterCoords)->x)*(next->x - (*iterCoords)->x)) + (((next)->y - (*iterCoords)->y)*((next)->y - (*iterCoords)->y)));
            currentPrevDist = sqrt(((prev->x - (*iterCoords)->x)*(prev->x - (*iterCoords)->x)) + (((prev)->y - (*iterCoords)->y)*((prev)->y - (*iterCoords)->y)));
            
            std::cout << "Dist to Next: " << currentNextDist << std::endl;
            std::cout << "Dist to Next: " << currentPrevDist << std::endl;
            
            // alpha * sq(abs(V+1 - V))
            elasEnergy = alpha * (currentNextDist + currentPrevDist);
            
            std::cout << "Elastic Energy: " << elasEnergy << std::endl;
            
            current2->x = (*iterCoords)->x * 2;
            current2->y = (*iterCoords)->y * 2;
            
            currentNextStiffX = next->x - current2->x + prev->x;
            currentNextStiffY = next->y - current2->y + prev->y;
            currentNextStiffPart = sqrt((currentNextStiffX * currentNextStiffX) + (currentNextStiffY * currentNextStiffY));
            // beta * sq(abs(V+1 - 2V + V-1))
            stiffEnergy = beta * (currentNextStiffPart * currentNextStiffPart);
            std::cout << "Stiffness Energy: " << stiffEnergy << std::endl;
            
            internalEnergy += (elasEnergy + stiffEnergy);
            
            ++nodeCount;
        }
        
        //internalEnergy = internalEnergy/nodeCount;
        //externalEnergy = externalEnergy/nodeCount;
        
        delete current2;
        
        energyValue = externalEnergy + internalEnergy;
        
        //std::cout << "external Energy = " << externalEnergy << std::endl;
        //std::cout << "internal Energy = " << internalEnergy << std::endl;
        //std::cout << "Total Energy = " << energyValue << std::endl;
        
        return energyValue;
    }
    
    RSGISFitActiveContourOptimiseFunc::~RSGISFitActiveContourOptimiseFunc()
    {
        
    }
    
    */
    
    
    
    
    
    
}}






