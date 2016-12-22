/*
 *  RSGISGenerateConvexHullGroups.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/07/2013.
 *  Copyright 2013 RSGISLib. All rights reserved.
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

#include "RSGISGenerateConvexHullGroups.h"

namespace rsgis{namespace vec{
	

    RSGISGenerateConvexHullGroups::RSGISGenerateConvexHullGroups()
    {
        
    }
    
    std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* >* RSGISGenerateConvexHullGroups::getCoordinateGroups(std::string inputFile, unsigned int eastingsColIdx, unsigned int northingsColIdx, unsigned int attributeColIdx)throw(RSGISVectorException)
    {
        std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* > *coordGrps = NULL;
        try
        {
            std::cout << "Find the Unique Attribute Values." << std::endl;
            std::vector<std::string> grps;
            std::vector<std::string> *tokens = new std::vector<std::string>();
            
            std::string line = "";
            std::string att = "";
            bool foundAtt = false;
            
            rsgis::utils::RSGISTextUtils textUtils;
            rsgis::utils::RSGISTextFileLineReader lineReader;
            lineReader.openFile(inputFile);
            while(!lineReader.endOfFile())
            {
                line = lineReader.readLine();
                
                tokens->clear();
                textUtils.tokenizeString(line, ',', tokens, true, true);
                
                att = tokens->at(attributeColIdx);
                if(!textUtils.blankline(att))
                {
                    if(grps.empty())
                    {
                        grps.push_back(att);
                    }
                    else
                    {
                        foundAtt = false;
                        for(std::vector<std::string>::iterator iterGrps = grps.begin(); iterGrps != grps.end(); ++iterGrps)
                        {
                            if(att == (*iterGrps))
                            {
                                foundAtt = true;
                                break;
                            }
                        }
                        if(!foundAtt)
                        {
                            grps.push_back(att);
                        }
                    }
                }
            }
            lineReader.closeFile();
            
           if(grps.empty())
           {
               throw RSGISVectorException("There were no groups identified.");
           }
            
            std::cout << "Groups Are:\n";
            for(std::vector<std::string>::iterator iterGrps = grps.begin(); iterGrps != grps.end(); ++iterGrps)
            {
                std::cout << "\t" << *iterGrps << std::endl;
            }
            
            std::cout << "Populate the data structure\n";
            coordGrps = new std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* >();
            std::pair<std::string,std::vector<geos::geom::Coordinate>* > *grpPair = NULL;
            coordGrps->reserve(grps.size());
            for(size_t i = 0; i < grps.size(); ++i)
            {
                grpPair = new std::pair<std::string,std::vector<geos::geom::Coordinate>* >();
                grpPair->first = grps.at(i);
                grpPair->second = new std::vector<geos::geom::Coordinate>();
                coordGrps->push_back(grpPair);
            }
            
            double eastings = 0.0;
            double northings = 0.0;
            lineReader.openFile(inputFile);
            while(!lineReader.endOfFile())
            {
                line = lineReader.readLine();
                
                tokens->clear();
                textUtils.tokenizeString(line, ',', tokens, true, true);
                
                att = tokens->at(attributeColIdx);
                eastings = textUtils.strtodouble(tokens->at(eastingsColIdx));
                northings = textUtils.strtodouble(tokens->at(northingsColIdx));
                if(!textUtils.blankline(att))
                {
                    for(std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* >::iterator iterGrps = coordGrps->begin(); iterGrps != coordGrps->end(); ++iterGrps)
                    {
                        if((*iterGrps)->first == att)
                        {
                            (*iterGrps)->second->push_back(geos::geom::Coordinate(eastings, northings));
                        }
                    }
                }
            }
            lineReader.closeFile();
            
        }
        catch (RSGISVectorException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISVectorException(e.what());
        }
        
        return coordGrps;
    }
    
    
    void RSGISGenerateConvexHullGroups::createPolygonsAsShapefile(std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* > *coordGrps, std::string outputFile, std::string outProj, bool force)throw(RSGISVectorException)
    {
        try
        {
            rsgis::geom::RSGISGeometry geomUtils;
            std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
            std::vector<std::string> *grpNames = new std::vector<std::string>();
            geos::geom::Polygon *poly = NULL;
            
            for(std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* >::iterator iterGrps = coordGrps->begin(); iterGrps != coordGrps->end(); ++iterGrps)
            {
                if((*iterGrps)->second->size() < 3)
                {
                    poly = geomUtils.findBoundingBox((*iterGrps)->second);
                }
                else
                {
                    poly = geomUtils.findConvexHull((*iterGrps)->second);
                }
                polys->push_back(poly);
                grpNames->push_back((*iterGrps)->first);
            }
            
            OGRSpatialReference *spatialRef = new OGRSpatialReference(outProj.c_str());
            rsgis::vec::RSGISVectorIO vecIO;
            vecIO.exportGEOSPolygons2SHP(outputFile, force, polys, spatialRef, grpNames, "GrpName");
        }
        catch (RSGISVectorOutputException &e)
        {
            throw RSGISVectorException(e.what());
        }
        catch (RSGISVectorException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISVectorException(e.what());
        }
    }
		
    RSGISGenerateConvexHullGroups::~RSGISGenerateConvexHullGroups()
    {
        
    }
	
}}




