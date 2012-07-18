/*
 *  RSGISAttributeTableNeighbours.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/03/2012.
 *  Copyright 2012 RSGISLib.
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

#include "RSGISAttributeTableNeighbours.h"

namespace rsgis{namespace rastergis{
    
    
    RSGISAttributeTableNeighbours::RSGISAttributeTableNeighbours()
    {
        
    }
    
    std::vector<std::vector<unsigned long > >* RSGISAttributeTableNeighbours::importFromTextFile(std::string inFilePath) throw(RSGISAttributeTableException)
    {
        std::vector<std::vector<unsigned long > > *neighbours = NULL;
        
        try 
        {
            rsgis::utils::RSGISTextUtils txtUtils;
            rsgis::utils::RSGISTextFileLineReader txtReader;
            txtReader.openFile(inFilePath);
            
            std::vector<std::string> *tokens = new std::vector<std::string>();
            std::string line = "";
            unsigned long lineCount = 0;
            unsigned long numNeighbours = 0;
            unsigned long numVals = 0;
            unsigned long valCount = 0;
            unsigned long featCount = 0;
            
            while(!txtReader.endOfFile())
            {
                tokens->clear();
                line = txtReader.readLine();
                if((!txtUtils.lineStart(line, '#')) & (!txtUtils.blankline(line)))
                {
                    if(lineCount == 0)
                    {
                        numNeighbours = txtUtils.strto64bitUInt(line);
                        neighbours = new std::vector<std::vector<unsigned long > >();
                        neighbours->reserve(numNeighbours);
                    }
                    else
                    {
                        txtUtils.tokenizeString(line, ',', tokens, true);
                        valCount = 0;
                        for(std::vector<std::string>::iterator iterStr = tokens->begin(); iterStr != tokens->end(); ++iterStr)
                        {
                            if(valCount == 0)
                            {
                                // IGNORE...
                                // Could check they are in order?!?
                            }
                            else if(valCount == 1)
                            {
                                numVals = txtUtils.strto64bitUInt(*iterStr);
                                std::vector<unsigned long > valsList = std::vector<unsigned long >();
                                if(numVals > 0)
                                {
                                    valsList.reserve(numVals);
                                }
                                neighbours->push_back(valsList);
                            }
                            else
                            {
                                neighbours->at(featCount).push_back(txtUtils.strto64bitUInt(*iterStr));
                            }
                            
                            ++valCount;
                        }
                        ++featCount;
                    }
                    
                    ++lineCount;
                }
            }
            
        }
        catch (RSGISException &e) 
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        return neighbours;
    }
    
    void RSGISAttributeTableNeighbours::exportToTextFile(std::vector<std::list<unsigned long >* > *neighbours, std::string outFilePath) throw(RSGISAttributeTableException)
    {
        try 
        {
            std::ofstream outTxtFile;
            outTxtFile.open(outFilePath.c_str(), std::ios::out | std::ios::trunc);
            
            outTxtFile << "# Number of features.\n";
            outTxtFile << neighbours->size() << std::endl;
            outTxtFile << "# FID, Number of Neighbours, Neighbour FIDs.\n";
            unsigned long cFID = 0;
            std::vector<std::list<unsigned long >* >::iterator iterNeighbours;
            std::list<unsigned long >::iterator iterNeighbourFIDs;
            for(iterNeighbours = neighbours->begin(); iterNeighbours != neighbours->end(); ++iterNeighbours)
            {
                outTxtFile << cFID++ << "," << (*iterNeighbours)->size();
                for(iterNeighbourFIDs = (*iterNeighbours)->begin(); iterNeighbourFIDs != (*iterNeighbours)->end(); ++iterNeighbourFIDs)
                {
                    outTxtFile << "," << *iterNeighbourFIDs;
                }
                outTxtFile << std::endl;
            }
            outTxtFile.flush();
            outTxtFile.close();
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    void RSGISAttributeTableNeighbours::exportToTextFile(std::vector<std::vector<unsigned long > > *neighbours, std::string outFilePath) throw(RSGISAttributeTableException)
    {
        try 
        {
            std::ofstream outTxtFile;
            outTxtFile.open(outFilePath.c_str(), std::ios::out | std::ios::trunc);
            
            outTxtFile << "# Number of features.\n";
            outTxtFile << neighbours->size() << std::endl;
            outTxtFile << "# FID, Number of Neighbours, Neighbour FIDs.\n";
            unsigned long cFID = 0;
            std::vector<std::vector<unsigned long > >::iterator iterNeighbours;
            std::vector<unsigned long >::iterator iterNeighbourFIDs;
            for(iterNeighbours = neighbours->begin(); iterNeighbours != neighbours->end(); ++iterNeighbours)
            {
                outTxtFile << cFID++ << "," << (*iterNeighbours).size();
                for(iterNeighbourFIDs = (*iterNeighbours).begin(); iterNeighbourFIDs != (*iterNeighbours).end(); ++iterNeighbourFIDs)
                {
                    outTxtFile << "," << *iterNeighbourFIDs;
                }
                outTxtFile << std::endl;
            }
            outTxtFile.flush();
            outTxtFile.close();
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    void RSGISAttributeTableNeighbours::clearMemory(std::vector<std::list<unsigned long >* > *neighbours)
    {
        std::vector<std::list<unsigned long >* >::iterator iterNeighbours;
        for(iterNeighbours = neighbours->begin(); iterNeighbours != neighbours->end(); ++iterNeighbours)
        {
            delete *iterNeighbours;
        }
        delete neighbours;
    }
        
    RSGISAttributeTableNeighbours::~RSGISAttributeTableNeighbours()
    {
        
    }
    
}}


