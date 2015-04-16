/*
 *  RSGISMergeSegments.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/04/2015.
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

#include "segmentation/RSGISMergeSegments.h"


namespace rsgis{namespace segment{
    
    RSGISMergeSegments::RSGISMergeSegments()
    {
        
    }
    
    void RSGISMergeSegments::mergeSelectedClumps(GDALDataset *clumpsImage, GDALDataset *valsImageDS, std::string clumps2MergeCol)throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            std::cout << "Populate Neighbours\n";
            rastergis::RSGISFindClumpNeighbours findNeighboursObj;
            findNeighboursObj.findNeighboursKEAImageCalc(clumpsImage, 1);
            std::cout << "Populated Neighbours\n";
            
            std::cout << "Calculate Stats\n";
            int numSpecBands = valsImageDS->GetRasterCount();
            
            utils::RSGISTextUtils textUtils;
            
            std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats = new std::vector<rsgis::rastergis::RSGISBandAttStats*>();
            bandStats->reserve(numSpecBands);
            
            std::vector<std::string> colNames;
            colNames.reserve(numSpecBands);
            
            rsgis::rastergis::RSGISBandAttStats *bandStat = NULL;
            for(int i = 0; i < numSpecBands; ++i)
            {
                std::string bandName = "Band" + textUtils.int32bittostring(i+1);
                std::string bandNameMean = "Mean"+bandName;
                std::string bandNameSum = "Sum"+bandName;
                
                bandStat = new rsgis::rastergis::RSGISBandAttStats();
                bandStat->band = i+1;
                bandStat->calcMin = false;
                bandStat->minField = "";
                bandStat->calcMax = false;
                bandStat->maxField = "";
                bandStat->calcMean = true;
                bandStat->meanField = bandNameMean;
                bandStat->calcStdDev = false;
                bandStat->stdDevField = "";
                bandStat->calcSum = true;
                bandStat->sumField = bandNameSum;
                
                bandStats->push_back(bandStat);
                colNames.push_back(bandName);
            }
            
            rsgis::rastergis::RSGISPopRATWithStats clumpStats;
            clumpStats.populateRATWithBasicStats(clumpsImage, valsImageDS, bandStats, 1);
            
            delete bandStats;
            std::cout << "Calculated Stats\n";
            
            rastergis::RSGISRasterAttUtils attUtils;
            
            GDALRasterBand *clumpBand = clumpsImage->GetRasterBand(1);
            GDALRasterAttributeTable *rat = clumpBand->GetDefaultRAT();
            size_t numRows = rat->GetRowCount();
            std::cout << "Number of clumps is " << numRows << "\n";
            
            std::cout << "Read in neighbours\n";
            std::vector<std::vector<size_t>* > *neighbours = attUtils.getRATNeighbours(clumpsImage, 1);
            
            if(numRows != neighbours->size())
            {
                for(std::vector<std::vector<size_t>* >::iterator iterNeigh = neighbours->begin(); iterNeigh != neighbours->end(); ++iterNeigh)
                {
                    delete *iterNeigh;
                }
                delete neighbours;
                
                throw rsgis::RSGISAttributeTableException("RAT size is different to the number of neighbours retrieved.");
            }
            
            size_t tmpNumRows = 0;
            int *selectCol = attUtils.readIntColumn(rat, clumps2MergeCol, &tmpNumRows);
            std::cout << "Read input column\n";
            
            double *numPxls = new double[numRows];
            
            double **meanVals = new double*[numSpecBands];
            double **sumVals = new double*[numSpecBands];
            
            for(int i = 0; i < numSpecBands; ++i)
            {
                tmpNumRows = 0;
                meanVals[i] = attUtils.readDoubleColumn(rat, "Mean"+colNames.at(i), &tmpNumRows);
                if(tmpNumRows != numRows)
                {
                    throw rsgis::img::RSGISImageCalcException("Number of rows was incorrect. (Mean)");
                }
                tmpNumRows = 0;
                sumVals[i] = attUtils.readDoubleColumn(rat, "Sum"+colNames.at(i), &tmpNumRows);
                if(tmpNumRows != numRows)
                {
                    throw rsgis::img::RSGISImageCalcException("Number of rows was incorrect. (Sum)");
                }
            }
            
            int *clumpID = new int[numRows];
            int *clumpIDUp = new int[numRows];
            bool *updated = new bool[numRows];
            std::vector<size_t> tmpNeigh;
            for(size_t i = 0; i < numRows; ++i)
            {
                clumpID[i] = i;
                clumpIDUp[i] = i;
                updated[i] = false;
                numPxls[i] = sumVals[0][i] / meanVals[0][i];
            }
            
            double *valsRef = new double[numSpecBands];
            double *valsTest = new double[numSpecBands];
            double val = 0.0;
            bool process = true;
            bool first = true;
            size_t minIdx = 0;
            double minVal = 0.0;
            bool found = false;
            unsigned int processIter = 1;
            while(process)
            {
                std::cout << "Processing Iteration " << processIter++ << std::endl;
                process = false;
                for(size_t i = 0; i < numRows; ++i)
                {
                    updated[i] = false;
                }
                for(size_t i = 0; i < numRows; ++i)
                {
                    if(selectCol[i] == 1)
                    {
                        //std::cout << "\t Process clump " << i;
                        first = true;
                        for(int n = 0; n < numSpecBands; ++n)
                        {
                            valsRef[n] = meanVals[n][i];
                        }
                        for(std::vector<size_t>::iterator iterNeigh = neighbours->at(i)->begin(); iterNeigh != neighbours->at(i)->end(); ++iterNeigh)
                        {
                            if(selectCol[(*iterNeigh)] != 1)
                            {
                                for(int n = 0; n < numSpecBands; ++n)
                                {
                                    valsRef[n] = meanVals[n][(*iterNeigh)];
                                }
                                
                                val = this->calcDist(valsRef, valsTest, numSpecBands);
                                if(first)
                                {
                                    minIdx = (*iterNeigh);
                                    minVal = val;
                                    first = false;
                                }
                                else if(val < minVal)
                                {
                                    minIdx = (*iterNeigh);
                                    minVal = val;
                                }
                            }
                        }
                        
                        if(!first)
                        {
                            updated[i] = true;
                            clumpIDUp[i] = minIdx;
                            //std::cout << " is merged with " << minIdx;
                            process = true;
                        }
                        //std::cout << std::endl;
                    }
                }
                
                //std::cout << "Found What to Merge to...\n";
                
                for(size_t i = 0; i < numRows; ++i)
                {
                    if(updated[i])
                    {
                        //std::cout << "Updating " << i << " to " << clumpIDUp[i] << std::endl;
                        numPxls[clumpIDUp[i]] += numPxls[i];
                        for(int n = 0; n < numSpecBands; ++n)
                        {
                            sumVals[n][clumpIDUp[i]] += sumVals[n][i];
                            meanVals[n][clumpIDUp[i]] = sumVals[n][clumpIDUp[i]]/((double)numPxls[clumpIDUp[i]]);
                        }
                        
                        tmpNeigh.clear();
                        for(std::vector<size_t>::iterator iterNeighRef = neighbours->at(i)->begin(); iterNeighRef != neighbours->at(i)->end(); ++iterNeighRef)
                        {
                            found = false;
                            for(std::vector<size_t>::iterator iterNeighObj = neighbours->at(clumpIDUp[i])->begin(); iterNeighObj != neighbours->at(clumpIDUp[i])->end(); ++iterNeighObj)
                            {
                                if( (*iterNeighRef) == (*iterNeighObj) )
                                {
                                    found = true;
                                    break;
                                }
                            }
                            if(!found)
                            {
                                tmpNeigh.push_back(*iterNeighRef);
                            }
                        }
                        
                        for(std::vector<size_t>::iterator iterNeigh = tmpNeigh.begin(); iterNeigh != tmpNeigh.end(); ++iterNeigh)
                        {
                            neighbours->at(clumpIDUp[i])->push_back(*iterNeigh);
                        }
                        neighbours->at(i)->clear();
                        selectCol[i] = 0;
                        updated[i] = false;
                    }
                }
                //std::cout << "Iteration Complete\n";
            }
            std::cout << "Completed Iterations\n";
            attUtils.writeIntColumn(rat, "OutClumpIDs", clumpIDUp, numRows);
            
            delete[] valsRef;
            delete[] valsTest;
            
            
            for(int i = 0; i < numSpecBands; ++i)
            {
                delete[] meanVals[i];
                delete[] sumVals[i];
            }
            delete[] meanVals;
            delete[] sumVals;
            delete[] numPxls;
            delete[] selectCol;
            delete[] updated;
            delete[] clumpIDUp;
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    RSGISMergeSegments::~RSGISMergeSegments()
    {
        
    }
    
}}

