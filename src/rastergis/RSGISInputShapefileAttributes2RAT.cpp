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

    void RSGISInputShapefileAttributes2RAT::copyVectorAtt2Rat(GDALDataset *clumpsImage, unsigned int ratBand, OGRLayer *vecLayer, std::string fidColStr, std::vector<std::string> *colNames)throw(RSGISAttributeTableException)
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

            std::cout << "Check the number of clumps\n";
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
            std::cout << "Importing columns: \n";
            int *intDataVal = new int[numRows];
            double *realDataVal = new double[numRows];
            std::string *strDataVal = new std::string[numRows];

            OGRFeatureDefn *ogrFeatDef = vecLayer->GetLayerDefn();
            int fididx = ogrFeatDef->GetFieldIndex(fidColStr.c_str());
            int fid = 0;
            OGRFieldDefn *fieldDef = NULL;
            OGRFeature *feat = NULL;
            int fieldIdx = 0;
            for(std::vector<std::string>::iterator iterColNames = colNames->begin(); iterColNames != colNames->end(); ++iterColNames)
            {
                std::cout << *iterColNames << std::endl;
                fieldIdx = ogrFeatDef->GetFieldIndex((*iterColNames).c_str());
                fieldDef = ogrFeatDef->GetFieldDefn(fieldIdx);
                if(fieldDef->GetType() == OFTInteger)
                {
                    for(int i = 0; i < numRows; ++i)
                    {
                        intDataVal[i] = 0;
                    }

                    int feedbackstep = numVecFeats/10;
                    int nextfeedback = feedbackstep;
                    int feedbackCounter = 0;
                    int i = 0;
                    std::cout << "\tStarted" << std::flush;
                    vecLayer->ResetReading();
                    while( (feat = vecLayer->GetNextFeature()) != NULL )
                    {
                        if((numVecFeats > 10) && (i == nextfeedback))
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                            nextfeedback = nextfeedback + feedbackstep;
                        }
                        fid = feat->GetFieldAsInteger(fididx);
                        intDataVal[fid] = feat->GetFieldAsInteger(fieldIdx);
                        ++i;
                    }
                    std::cout << " Complete.\n";
                    ratUtils.writeIntColumn(rat, (*iterColNames), intDataVal, numRows);
                }
                else if(fieldDef->GetType() == OFTReal)
                {
                    for(int i = 0; i < numRows; ++i)
                    {
                        realDataVal[i] = 0;
                    }

                    int feedbackstep = numVecFeats/10;
                    int nextfeedback = feedbackstep;
                    int feedbackCounter = 0;
                    int i = 0;
                    std::cout << "\tStarted" << std::flush;
                    vecLayer->ResetReading();
                    while( (feat = vecLayer->GetNextFeature()) != NULL )
                    {
                        if((numVecFeats > 10) && (i == nextfeedback))
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                            nextfeedback = nextfeedback + feedbackstep;
                        }
                        fid = feat->GetFieldAsInteger(fididx);
                        realDataVal[fid] = feat->GetFieldAsDouble(fieldIdx);
                        ++i;
                    }
                    std::cout << " Complete.\n";
                    ratUtils.writeRealColumn(rat, (*iterColNames), realDataVal, numRows);
                }
                else if(fieldDef->GetType() == OFTString)
                {
                    for(int i = 0; i < numRows; ++i)
                    {
                        strDataVal[i] = std::string("");
                    }

                    int feedbackstep = numVecFeats/10;
                    int nextfeedback = feedbackstep;
                    int feedbackCounter = 0;
                    int i = 0;
                    std::cout << "\tStarted" << std::flush;
                    vecLayer->ResetReading();
                    while( (feat = vecLayer->GetNextFeature()) != NULL )
                    {
                        if((numVecFeats > 10) && (i == nextfeedback))
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                            nextfeedback = nextfeedback + feedbackstep;
                        }
                        fid = feat->GetFieldAsInteger(fididx);
                        strDataVal[fid] = std::string(feat->GetFieldAsString(fieldIdx));
                        ++i;
                    }
                    std::cout << " Complete.\n";
                    ratUtils.writeStrColumn(rat, (*iterColNames), strDataVal, numRows);
                }
                else
                {
                    std::string message = "Data type could not be represented in RAT for field '" + (*iterColNames) + "'.";
                    throw RSGISAttributeTableException(message);
                }
            }

            delete[] intDataVal;
            delete[] realDataVal;
            delete[] strDataVal;

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

}}


