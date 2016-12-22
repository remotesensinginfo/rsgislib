/*
 *  RSGISInputShapefileAttributes2RAT.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/12/2014.
 *  Copyright 2014 RSGISLib. All rights reserved.
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

#include "RSGISInputShapefileAttributes2RAT.h"


namespace rsgis{namespace rastergis{
    
    RSGISInputShapefileAttributes2RAT::RSGISInputShapefileAttributes2RAT()
    {
        
    }
    
    void RSGISInputShapefileAttributes2RAT::copyVectorAtt2Rat(GDALDataset *clumpsImage, unsigned int ratBand, OGRLayer *vecLayer, std::vector<std::string> *colNames)throw(RSGISAttributeTableException)
    {
        try
        {
            rsgis::rastergis::RSGISRasterAttUtils ratUtils;
            
            if(ratBand == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT Band must be greater than zero.");
            }
            if(ratBand > clumpsImage->GetRasterCount())
            {
                throw rsgis::RSGISAttributeTableException("RAT Band is larger than the number of bands within the image.");
            }
            
            GDALRasterAttributeTable *rat = clumpsImage->GetRasterBand(ratBand)->GetDefaultRAT();
            size_t numRows = rat->GetRowCount();
            
            long minClumpID = 0;
            long maxClumpID = 0;
            ratUtils.getImageBandMinMax(clumpsImage, ratBand, &minClumpID, &maxClumpID);
            
            if(maxClumpID > numRows)
            {
                numRows = boost::lexical_cast<size_t>(maxClumpID);
                rat->SetRowCount(numRows);
            }
            
            size_t numVecFeats = vecLayer->GetFeatureCount(true);
            
            if(numVecFeats != (numRows-1))
            {
                std::cout << "Number of Vector Features = " << numVecFeats << std::endl;
                std::cout << "Number of RAT Rows    = " << numRows << std::endl;
                throw RSGISAttributeTableException("The number of rows within the vector attribute table and the number of RAT features is not the same.");
            }
            
            std::vector<RSGISRATCol> ratColInfo;
            std::vector<RSGISRATCol> *colsInfo = ratUtils.getVectorColumns(vecLayer, true);
            for(std::vector<std::string>::iterator iterColNames = colNames->begin(); iterColNames != colNames->end(); ++iterColNames)
            {
                bool foundCol = false;
                for(std::vector<RSGISRATCol>::iterator iterAllColNames = colsInfo->begin(); iterAllColNames != colsInfo->end(); ++iterAllColNames)
                {
                    if((*iterColNames) == (*iterAllColNames).name)
                    {
                        ratColInfo.push_back((*iterAllColNames));
                        
                        foundCol = true;
                        break;
                    }
                }
                if(!foundCol)
                {
                    delete colsInfo;
                    std::string message = "Could not find column \'" + (*iterColNames) + "\' within the vector layer.";
                    throw RSGISAttributeTableException(message);
                }
            }
            
            std::vector<unsigned int> inRealColIdx;
            std::vector<unsigned int> inIntColIdx;
            std::vector<unsigned int> inStrColIdx;
            std::vector<unsigned int> outRealColIdx;
            std::vector<unsigned int> outIntColIdx;
            std::vector<unsigned int> outStrColIdx;
            
            for(std::vector<RSGISRATCol>::iterator iterCols = ratColInfo.begin(); iterCols != ratColInfo.end(); ++iterCols)
            {
                (*iterCols).idx = ratUtils.findColumnIndexOrCreate(rat, (*iterCols).name, (*iterCols).type, (*iterCols).usage);
                if((*iterCols).type == GFT_Integer)
                {
                    outIntColIdx.push_back((*iterCols).idx);
                }
                else if((*iterCols).type == GFT_Real)
                {
                    outRealColIdx.push_back((*iterCols).idx);
                }
                else if((*iterCols).type == GFT_String)
                {
                    outStrColIdx.push_back((*iterCols).idx);
                }
                else
                {
                    throw RSGISAttributeTableException("Column data type is not recognised.");
                }
            }
            
            RSGISRATCalcValueCopyShapeAtts copyShpAtts(vecLayer, ratColInfo, numVecFeats);
            RSGISRATCalc calcRatVals(&copyShpAtts);
            calcRatVals.calcRATValues(rat, inRealColIdx, inIntColIdx, inStrColIdx, outRealColIdx, outIntColIdx, outStrColIdx);
            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
        
    RSGISInputShapefileAttributes2RAT::~RSGISInputShapefileAttributes2RAT()
    {
        
    }
    
    
    
    
    RSGISRATCalcValueCopyShapeAtts::RSGISRATCalcValueCopyShapeAtts(OGRLayer *vecLayer, std::vector<RSGISRATCol> ratColInfo, size_t numVecFeats):RSGISRATCalcValue()
    {
        this->vecLayer = vecLayer;
        this->ratColInfo = ratColInfo;
        this->numVecFeats = numVecFeats;
    }
    
    void RSGISRATCalcValueCopyShapeAtts::calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException)
    {
        try
        {
            if(fid == 0)
            {
                for(unsigned int i = 0; i < numOutIntCols; ++i)
                {
                    outIntCols[i] = 0;
                }
                for(unsigned int i = 0; i < numOutRealCols; ++i)
                {
                    outRealCols[i] = 0.0;
                }
                for(unsigned int i = 0; i < numOutStringCols; ++i)
                {
                    outStringCols[i] = "";
                }
            }
            else if(fid <= numVecFeats)
            {
                OGRFeature *feat = vecLayer->GetFeature(fid-1);
                
                unsigned int intFieldIdx = 0;
                unsigned int realFieldIdx = 0;
                unsigned int strFieldIdx = 0;
                
                for(std::vector<RSGISRATCol>::iterator iterCols = ratColInfo.begin(); iterCols != ratColInfo.end(); ++iterCols)
                {
                    if((*iterCols).type == GFT_Integer)
                    {
                        if(intFieldIdx == numOutIntCols)
                        {
                            throw RSGISAttributeTableException("The number of available int columns has already been reached...");
                        }
                        outIntCols[intFieldIdx++] = feat->GetFieldAsInteger((*iterCols).name.c_str());
                    }
                    else if((*iterCols).type == GFT_Real)
                    {
                        if(realFieldIdx == numOutRealCols)
                        {
                            throw RSGISAttributeTableException("The number of available real columns has already been reached...");
                        }
                        outRealCols[realFieldIdx++] = feat->GetFieldAsDouble((*iterCols).name.c_str());
                    }
                    else if((*iterCols).type == GFT_String)
                    {
                        if(strFieldIdx == numOutStringCols)
                        {
                            throw RSGISAttributeTableException("The number of available string columns has already been reached...");
                        }
                        
                        outStringCols[strFieldIdx++] = std::string(feat->GetFieldAsString((*iterCols).name.c_str()));
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Column data type is not recognised.");
                    }
                }
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISRATCalcValueCopyShapeAtts::~RSGISRATCalcValueCopyShapeAtts()
    {
        
    }
    
}}


