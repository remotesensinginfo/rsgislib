/*
 *  RSGISInterpolateClumpValues2Image.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 14/09/2013.
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

#include "RSGISInterpolateClumpValues2Image.h"

namespace rsgis{namespace rastergis{
    
    RSGISInterpolateClumpValues2Image::RSGISInterpolateClumpValues2Image()
    {
        
    }
    
    void RSGISInterpolateClumpValues2Image::interpolateImageFromClumps(GDALDataset *clumpsDataset, std::string selectField, std::string eastingsField, std::string northingsField, std::string valueField, std::string outputFile, std::string imageFormat, GDALDataType dataType, rsgis::math::RSGIS2DInterpolator *interpolator, unsigned int ratband)throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            std::cout << "Open Attribute Table.\n";
            GDALRasterAttributeTable *gdalAtt = clumpsDataset->GetRasterBand(ratband)->GetDefaultRAT();
            
            if((gdalAtt == NULL) || (gdalAtt->GetRowCount() == 0))
            {
                throw RSGISAttributeTableException("Input image does not have an attribute table.");
            }
            
            RSGISRasterAttUtils attUtils;
            rsgis::img::RSGISImageUtils imgUtils;
            
            unsigned int selectIdx = attUtils.findColumnIndex(gdalAtt, selectField);
            unsigned int eastingsIdx = attUtils.findColumnIndex(gdalAtt, eastingsField);
            unsigned int northingsIdx = attUtils.findColumnIndex(gdalAtt, northingsField);
            unsigned int valueIdx = attUtils.findColumnIndex(gdalAtt, valueField);
    
            
            unsigned int numSelectValues = 0;
            std::vector<rsgis::math::RSGISInterpolatorDataPoint> *pts = new std::vector<rsgis::math::RSGISInterpolatorDataPoint>();
            
            RSGISExtractPtValues extractVals = RSGISExtractPtValues(pts, &numSelectValues);
            RSGISRATCalc ratCalc = RSGISRATCalc(&extractVals);
            std::vector<unsigned int> inRealColIdx;
            inRealColIdx.push_back(eastingsIdx);
            inRealColIdx.push_back(northingsIdx);
            inRealColIdx.push_back(valueIdx);
            std::vector<unsigned int> inIntColIdx;
            inIntColIdx.push_back(selectIdx);
            std::vector<unsigned int> inStrColIdx;
            std::vector<unsigned int> outRealColIdx;
            std::vector<unsigned int> outIntColIdx;
            std::vector<unsigned int> outStrColIdx;
            ratCalc.calcRATValues(gdalAtt, inRealColIdx, inIntColIdx, inStrColIdx, outRealColIdx, outIntColIdx, outStrColIdx);
            
            std::cout << "There are " << numSelectValues << " selected features.\n";
            
            // Add in the image corners using values interpolated using the overall trend.
            
            rsgis::math::RSGIS2DInterpolator *trendInterp = new rsgis::math::RSGISLinearTrendInterpolator();
            trendInterp->initInterpolator(pts);
            
            OGREnvelope *imgEnv = imgUtils.getSpatialExtent(clumpsDataset);
            
            double tmpEast = 0.0;
            double tmpNorth = 0.0;
            double tmpValue = 0.0;
            
            // TL
            tmpEast = imgEnv->MinX;
            tmpNorth = imgEnv->MaxY;
            tmpValue = trendInterp->getValue(tmpEast, tmpNorth);
            pts->push_back(rsgis::math::RSGISInterpolatorDataPoint(tmpEast, tmpNorth, tmpValue));
            
            // TR
            tmpEast = imgEnv->MaxX;
            tmpNorth = imgEnv->MaxY;
            tmpValue = trendInterp->getValue(tmpEast, tmpNorth);
            pts->push_back(rsgis::math::RSGISInterpolatorDataPoint(tmpEast, tmpNorth, tmpValue));
            
            // BR
            tmpEast = imgEnv->MaxX;
            tmpNorth = imgEnv->MinY;
            tmpValue = trendInterp->getValue(tmpEast, tmpNorth);
            pts->push_back(rsgis::math::RSGISInterpolatorDataPoint(tmpEast, tmpNorth, tmpValue));
            
            // BL
            tmpEast = imgEnv->MinX;
            tmpNorth = imgEnv->MinY;
            tmpValue = trendInterp->getValue(tmpEast, tmpNorth);
            pts->push_back(rsgis::math::RSGISInterpolatorDataPoint(tmpEast, tmpNorth, tmpValue));
            
            delete trendInterp;
            
            interpolator->initInterpolator(pts);
            
            GDALDataset *outputDS = imgUtils.createCopy(clumpsDataset, 1, outputFile, imageFormat, dataType);
            
            rsgis::img::RSGISPopulateImageFromInterpolator popImg;
            popImg.populateImage(interpolator, outputDS);
            
            GDALClose(outputDS);
            delete pts;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
        
    RSGISInterpolateClumpValues2Image::~RSGISInterpolateClumpValues2Image()
    {
        
    }
    
    
    
    
    
    RSGISExtractPtValues::RSGISExtractPtValues(std::vector<rsgis::math::RSGISInterpolatorDataPoint> *pts, unsigned int *numSelectValues):RSGISRATCalcValue()
    {
        this->pts = pts;
        this->numSelectValues = numSelectValues;
    }
    
    void RSGISExtractPtValues::calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException)
    {
        if(numInRealCols != 3)
        {
            throw RSGISAttributeTableException("3 real fields must be provided (eastings, northings, value)");
        }
        if(numInIntCols != 1)
        {
            throw RSGISAttributeTableException("1 int field must be provided (selected)");
        }
        
        if(inIntCols[0] == 1)
        {
            pts->push_back(rsgis::math::RSGISInterpolatorDataPoint(inRealCols[0], inRealCols[1], inRealCols[2]));
            ++(*numSelectValues);
        }
    }
    
    RSGISExtractPtValues::~RSGISExtractPtValues()
    {
        
    }
    
}}



