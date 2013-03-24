/*
 *  RSGISCalcClumpShapeParameters.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 20/03/2013.
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

#include "RSGISCalcClumpShapeParameters.h"

namespace rsgis{namespace rastergis{
    
    RSGISCalcClumpShapeParameters::RSGISCalcClumpShapeParameters()
    {
        
    }
    
    void RSGISCalcClumpShapeParameters::calcClumpShapeParams(GDALDataset *clumpImage, std::vector<rsgis::rastergis::RSGISShapeParam*> *shapeIndexes) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            std::cout << "Read RAT\n";
            RSGISRasterAttUtils attUtils;
            const GDALRasterAttributeTable *attTableTmp = clumpImage->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *attTable = NULL;
            if(attTableTmp != NULL)
            {
                attTable = new GDALRasterAttributeTable(*attTableTmp);
            }
            else
            {
                attTable = new GDALRasterAttributeTable();
            }
            
            // Make sure it is long enough and extend if required.
            int numRows = attTable->GetRowCount();
            
            std::cout << "Check image statistics\n";
            double maxVal = 0;
            clumpImage->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
            
            if(maxVal > numRows)
            {
                attTable->SetRowCount(maxVal+1);
            }
            numRows = attTable->GetRowCount();
            
            double *trans = new double[6];
            clumpImage->GetGeoTransform(trans);
            
            double xRes = trans[1];
            double yRes = trans[5];
            if(yRes < 0)
            {
                yRes = yRes * (-1);
            }
            delete[] trans;

            bool needNumPixels = false;
            bool needBorderLength = false;
            bool needPixelCoords = false;
            bool needWidthLength = false;
            
            for(std::vector<rsgis::rastergis::RSGISShapeParam*>::iterator iterIndexes = shapeIndexes->begin(); iterIndexes != shapeIndexes->end(); ++iterIndexes)
            {
                //std::cout << rsgis::rastergis::RSGISCalcClumpShapeParameters::getRSGISShapeIndexAsString((*iterIndexes)->idx) << " Index with output column name \'" << (*iterIndexes)->colName << "\'" << std::endl;
                (*iterIndexes)->colIdx = attUtils.findColumnIndexOrCreate(attTable, (*iterIndexes)->colName, GFT_Real);
                
                if((*iterIndexes)->idx == rsgis_shapearea)
                {
                    needNumPixels = true;
                }
                else if((*iterIndexes)->idx == rsgis_asymmetry)
                {
                    needPixelCoords = true;
                }
                else if((*iterIndexes)->idx == rsgis_borderindex)
                {
                    needBorderLength = true;
                    needPixelCoords = true;
                    needWidthLength = true;
                }
                else if((*iterIndexes)->idx == rsgis_borderlength)
                {
                    needBorderLength = true;
                }
                else if((*iterIndexes)->idx == rsgis_compactness)
                {
                    needNumPixels = true;
                    needPixelCoords = true;
                    needWidthLength = true;
                }
                else if((*iterIndexes)->idx == rsgis_density)
                {
                    needNumPixels = true;
                    needPixelCoords = true;
                }
                else if((*iterIndexes)->idx == rsgis_ellipticfit)
                {
                    needNumPixels = true;
                    needPixelCoords = true;
                    needWidthLength = true;
                }
                else if((*iterIndexes)->idx == rsgis_length)
                {
                    needNumPixels = true;
                    needPixelCoords = true;
                    needWidthLength = true;
                }
                else if((*iterIndexes)->idx == rsgis_lengthwidth)
                {
                    needNumPixels = true;
                    needPixelCoords = true;
                    needWidthLength = true;
                }
                else if((*iterIndexes)->idx == rsgis_width)
                {
                    needNumPixels = true;
                    needPixelCoords = true;
                    needWidthLength = true;
                }
                else if((*iterIndexes)->idx == rsgis_maindirection)
                {
                    needPixelCoords = true;
                }
                else if((*iterIndexes)->idx == rsgis_radiuslargestenclosedellipse)
                {
                    needPixelCoords = true;
                }
                else if((*iterIndexes)->idx == rsgis_radiussmallestenclosedellipse)
                {
                    needPixelCoords = true;
                }
                else if((*iterIndexes)->idx == rsgis_rectangularfit)
                {
                    needNumPixels = true;
                    needPixelCoords = true;
                    needWidthLength = true;
                }
                else if((*iterIndexes)->idx == rsgis_roundness)
                {
                    needPixelCoords = true;
                }
                else if((*iterIndexes)->idx == rsgis_shapeindex)
                {
                    needBorderLength = true;
                    needNumPixels = true;
                }
            }
            
            size_t *numPxls = NULL;
            if(needNumPixels)
            {
                numPxls = new size_t[numRows];
            }
            double *borderLen = NULL;
            if(needBorderLength)
            {
                borderLen = new double[numRows];
            }
            std::vector<ShapePoint*> **pts = NULL;
            if(needPixelCoords)
            {
                pts = new std::vector<ShapePoint*>*[numRows];
            }
            for(size_t i = 0; i < numRows; ++i)
            {
                if(needBorderLength)
                {
                    borderLen[i] = 0;
                }
                if(needNumPixels)
                {
                    numPxls[i] = 0;
                }
                if(needPixelCoords)
                {
                    pts[i] = new std::vector<ShapePoint*>();
                }
            }
            
            if(needBorderLength)
            {
                std::cout << "Calculating the Border Lengths\n";
                rsgis::img::RSGISCalcImageValue *calcBorderLen = new RSGISCalcBorderLenInPixels(borderLen, numRows, xRes, yRes, true);
                rsgis::img::RSGISCalcImage imgCalcBorderLen = rsgis::img::RSGISCalcImage(calcBorderLen);
                imgCalcBorderLen.calcImageWindowData(&clumpImage, 1, 3);
                delete calcBorderLen;
            }
            
            if(needNumPixels | needPixelCoords)
            {
                std::cout << "Calculating the clump area and pixel locations\n";
                rsgis::img::RSGISCalcImageValue *calcPxlsLocsArea = new RSGISCalcShapePrimativesAreaPxlLocs(numPxls, pts, numRows, needNumPixels ,needPixelCoords);
                rsgis::img::RSGISCalcImage imgCalcPxlsLocsArea = rsgis::img::RSGISCalcImage(calcPxlsLocsArea);
                imgCalcPxlsLocsArea.calcImageExtent(&clumpImage, 1);
                delete calcPxlsLocsArea;
            }
            
            rsgis::math::RSGISMathsUtils mathUtils;
            rsgis::math::RSGISMatrices matrixUtils;
            double pxlArea = xRes * yRes;
            double length = 0.0;
            double width = 0.0;
            double lengthwidthratio = 0.0;
            double eigenHigh = 0.0;
            double eigenLow = 0.0;
            std::cout << "Pixel Area = " << pxlArea << std::endl;
            std::vector<double> *xVals = NULL;
            std::vector<double> *yVals = NULL;
            std::vector<double> *xyVals = NULL;
            rsgis::math::RSGISStatsSummary *summaryStats = NULL;
            rsgis::math::RSGISPrincipalComponentAnalysis *pca = NULL;
            rsgis::math::Matrix *inputDataMatrix = NULL;
            const rsgis::math::Matrix *eigenVals = NULL;
            
            if(needPixelCoords)
            {
                xVals = new std::vector<double>();
                yVals = new std::vector<double>();
                xyVals = new std::vector<double>();
                summaryStats = new rsgis::math::RSGISStatsSummary();
                mathUtils.initStatsSummary(summaryStats);
            }
            
            for(size_t i = 1; i < numRows; ++i)
            {
                if(needPixelCoords)
                {
                    for(std::vector<ShapePoint*>::iterator iterPts = pts[i]->begin(); iterPts != pts[i]->end(); ++iterPts)
                    {
                        xVals->push_back((*iterPts)->x);
                        yVals->push_back((*iterPts)->y);
                    }
                    
                    if(needWidthLength)
                    {
                        inputDataMatrix = matrixUtils.createMatrix(2, pts[i]->size());
                        size_t idx = 0;
                        for(std::vector<ShapePoint*>::iterator iterPts = pts[i]->begin(); iterPts != pts[i]->end(); ++iterPts)
                        {
                            inputDataMatrix->matrix[idx++] = (*iterPts)->x;
                            inputDataMatrix->matrix[idx++] = (*iterPts)->y;
                        }
                        pca = new rsgis::math::RSGISPrincipalComponentAnalysis(inputDataMatrix);
                        
                        eigenVals = pca->getEigenvalues();
                        
                        if((eigenVals->n != 2) & (eigenVals->m != 2))
                        {
                            std::cout << "eigenVals->n = " << eigenVals->n << std::endl;
                            std::cout << "eigenVals->m = " << eigenVals->m << std::endl;
                            rsgis::img::RSGISImageCalcException("Returned Eigenvalues matrix not the expected shape.");
                        }
                        
                        if(eigenVals->matrix[0] > eigenVals->matrix[1])
                        {
                            eigenHigh = eigenVals->matrix[0];
                            eigenLow = eigenVals->matrix[1];
                        }
                        else
                        {
                            eigenHigh = eigenVals->matrix[1];
                            eigenLow = eigenVals->matrix[0];
                        }
                        
                        lengthwidthratio = eigenHigh/eigenLow;
                        if(needNumPixels)
                        {
                            length = sqrt((((double)numPxls[i])*pxlArea)*lengthwidthratio);
                            width = (((double)numPxls[i])*pxlArea)/lengthwidthratio;
                        }
                    }
                    
                    mathUtils.initStatsSummary(summaryStats);
                    summaryStats->calcMean= true;
                    mathUtils.generateStats(xVals, summaryStats);
                    double xMean = summaryStats->mean;
                    summaryStats->mean = 0;
                    mathUtils.generateStats(yVals, summaryStats);
                    double yMean = summaryStats->mean;
                    
                    for(size_t j = 0; j < pts[i]->size(); ++j)
                    {
                        xVals->at(j) = xVals->at(j)-xMean;
                        yVals->at(j) = yVals->at(j)-yMean;
                        xyVals->push_back(xVals->at(j));
                        xyVals->push_back(yVals->at(j));
                    }
                }
                
                
                for(std::vector<rsgis::rastergis::RSGISShapeParam*>::iterator iterIndexes = shapeIndexes->begin(); iterIndexes != shapeIndexes->end(); ++iterIndexes)
                {                
                    if((*iterIndexes)->idx == rsgis_shapearea)
                    {
                        attTable->SetValue(i, (*iterIndexes)->colIdx, (((double)numPxls[i])*pxlArea));
                    }
                    else if((*iterIndexes)->idx == rsgis_asymmetry)
                    {
                        mathUtils.initStatsSummary(summaryStats);
                        summaryStats->calcStdDev = true;
                        mathUtils.generateStats(xVals, summaryStats);
                        double varX = summaryStats->stdDev * summaryStats->stdDev;
                        summaryStats->stdDev = 0;
                        mathUtils.generateStats(yVals, summaryStats);
                        double varY = summaryStats->stdDev * summaryStats->stdDev;
                        summaryStats->stdDev = 0;
                        mathUtils.generateStats(xyVals, summaryStats);
                        double varXY = summaryStats->stdDev * summaryStats->stdDev;
                        
                        double lowerFrac = varX + varY;
                        double upperFrac = 2*sqrt((((varX + varY)*(varX + varY))/4) + (varXY * varXY) - (varX * varY));
                        
                        double asymmetry = upperFrac/lowerFrac;
                        
                        attTable->SetValue(i, (*iterIndexes)->colIdx, asymmetry);
                    }
                    else if((*iterIndexes)->idx == rsgis_borderindex)
                    {
                        double borderIdx = borderLen[i] / (2*(length+width));
                        attTable->SetValue(i, (*iterIndexes)->colIdx, borderIdx);
                    }
                    else if((*iterIndexes)->idx == rsgis_borderlength)
                    {
                        attTable->SetValue(i, (*iterIndexes)->colIdx, borderLen[i]);
                    }
                    else if((*iterIndexes)->idx == rsgis_compactness)
                    {
                        double compactness = (length+width)/(((double)numPxls[i])*pxlArea);
                        attTable->SetValue(i, (*iterIndexes)->colIdx, compactness);
                    }
                    else if((*iterIndexes)->idx == rsgis_density)
                    {
                        mathUtils.initStatsSummary(summaryStats);
                        summaryStats->calcStdDev = true;
                        mathUtils.generateStats(xVals, summaryStats);
                        double varX = summaryStats->stdDev * summaryStats->stdDev;
                        summaryStats->stdDev = 0;
                        mathUtils.generateStats(yVals, summaryStats);
                        double varY = summaryStats->stdDev * summaryStats->stdDev;
                        
                        double density = sqrt(((double)numPxls[i])*pxlArea)/(1+sqrt(varX+varY));
                        
                        attTable->SetValue(i, (*iterIndexes)->colIdx, density );
                    }
                    else if((*iterIndexes)->idx == rsgis_ellipticfit)
                    {
                        
                    }
                    else if((*iterIndexes)->idx == rsgis_length)
                    {
                        attTable->SetValue(i, (*iterIndexes)->colIdx, length );
                    }
                    else if((*iterIndexes)->idx == rsgis_lengthwidth)
                    {
                        attTable->SetValue(i, (*iterIndexes)->colIdx, lengthwidthratio );
                    }
                    else if((*iterIndexes)->idx == rsgis_width)
                    {
                        attTable->SetValue(i, (*iterIndexes)->colIdx, width );
                    }
                    else if((*iterIndexes)->idx == rsgis_maindirection)
                    {
                        mathUtils.initStatsSummary(summaryStats);
                        summaryStats->calcStdDev = true;
                        summaryStats->stdDev = 0;
                        mathUtils.generateStats(yVals, summaryStats);
                        double varY = summaryStats->stdDev * summaryStats->stdDev;
                        summaryStats->stdDev = 0;
                        mathUtils.generateStats(xyVals, summaryStats);
                        double varXY = summaryStats->stdDev * summaryStats->stdDev;
                        double mainDirection = ((180/M_PI) * atan2(varXY, eigenHigh-varY)) + 90;
                        attTable->SetValue(i, (*iterIndexes)->colIdx, mainDirection);
                    }
                    else if((*iterIndexes)->idx == rsgis_radiuslargestenclosedellipse)
                    {
                        
                    }
                    else if((*iterIndexes)->idx == rsgis_radiussmallestenclosedellipse)
                    {
                        
                    }
                    else if((*iterIndexes)->idx == rsgis_rectangularfit)
                    {
                        
                    }
                    else if((*iterIndexes)->idx == rsgis_roundness)
                    {
                        
                    }
                    else if((*iterIndexes)->idx == rsgis_shapeindex)
                    {
                        if(borderLen[i] != 0)
                        {
                            attTable->SetValue(i, (*iterIndexes)->colIdx, (borderLen[i]) / (4*sqrt(((double)numPxls[i])*pxlArea)) );
                        }
                        else
                        {
                            attTable->SetValue(i, (*iterIndexes)->colIdx, 0.0 );
                        }
                    }
                }
                
                xVals->clear();
                yVals->clear();
                xyVals->clear();
                if(needWidthLength)
                {
                    delete pca;
                }
                //std::cout << std::endl;
            }
            
            if(needPixelCoords)
            {
                delete xVals;
                delete yVals;
                delete xyVals;
            }
            
            for(size_t i = 0; i < numRows; ++i)
            {                
                if(needPixelCoords)
                {
                    delete pts[i];
                }
            }
            
            if(needBorderLength)
            {
                delete[] borderLen;
            }
            if(needNumPixels)
            {
                delete[] numPxls;
            }
            if(needPixelCoords)
            {
                delete[] pts;
            }
            
            clumpImage->GetRasterBand(1)->SetDefaultRAT(attTable);
        }
        catch (rsgis::img::RSGISImageCalcException &e)
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
    
    rsgisshapeindex RSGISCalcClumpShapeParameters::getRSGISShapeIndex(std::string indexName) throw(rsgis::RSGISException)
    {
        rsgisshapeindex idx = rsgis_shapena;
        if(indexName == "Area")
        {
            idx = rsgis_shapearea;
        }
        else if(indexName == "Asymmetry")
        {
            idx = rsgis_asymmetry;
        }
        else if(indexName == "BorderIndex")
        {
            idx = rsgis_borderindex;
        }
        else if(indexName == "BorderLength")
        {
            idx = rsgis_borderlength;
        }
        else if(indexName == "Compactness")
        {
            idx = rsgis_compactness;
        }
        else if(indexName == "Density")
        {
            idx = rsgis_density;
        }
        else if(indexName == "EllipticFit")
        {
            idx = rsgis_ellipticfit;
        }
        else if(indexName == "Length")
        {
            idx = rsgis_length;
        }
        else if(indexName == "LengthWidth")
        {
            idx = rsgis_lengthwidth;
        }
        else if(indexName == "Width")
        {
            idx = rsgis_width;
        }
        else if(indexName == "MainDirection")
        {
            idx = rsgis_maindirection;
        }
        else if(indexName == "RadiusLargestEnclosedEllipse")
        {
            idx = rsgis_radiuslargestenclosedellipse;
        }
        else if(indexName == "RadiusSmallestEnclosedEllipse")
        {
            idx = rsgis_radiussmallestenclosedellipse;
        }
        else if(indexName == "RectangularFit")
        {
            idx = rsgis_rectangularfit;
        }
        else if(indexName == "Roundness")
        {
            idx = rsgis_roundness;
        }
        else if(indexName == "ShapeIndex")
        {
            idx = rsgis_shapeindex;
        }
        else
        {
            std::string message = std::string("Did not recognise name: \'") + indexName + std::string("\'");
            throw rsgis::RSGISException(message);
        }
        
        return idx;
    }
    
    std::string RSGISCalcClumpShapeParameters::getRSGISShapeIndexAsString(rsgisshapeindex index) throw(rsgis::RSGISException)
    {
        std::string idxName = "";
        if(index == rsgis_shapearea)
        {
            idxName = "Area";
        }
        else if(index == rsgis_asymmetry)
        {
            idxName = "Asymmetry";
        }
        else if(index == rsgis_borderindex)
        {
            idxName = "BorderIndex";
        }
        else if(index == rsgis_borderlength)
        {
            idxName = "BorderLength";
        }
        else if(index == rsgis_compactness)
        {
            idxName = "Compactness";
        }
        else if(index == rsgis_density)
        {
            idxName = "Density";
        }
        else if(index == rsgis_ellipticfit)
        {
            idxName = "EllipticFit";
        }
        else if(index == rsgis_length)
        {
            idxName = "Length";
        }
        else if(index == rsgis_lengthwidth)
        {
            idxName = "LengthWidth";
        }
        else if(index == rsgis_width)
        {
            idxName = "Width";
        }
        else if(index == rsgis_maindirection)
        {
            idxName = "MainDirection";
        }
        else if(index == rsgis_radiuslargestenclosedellipse)
        {
            idxName = "RadiusLargestEnclosedEllipse";
        }
        else if(index == rsgis_radiussmallestenclosedellipse)
        {
            idxName = "RadiusSmallestEnclosedEllipse";
        }
        else if(index == rsgis_rectangularfit)
        {
            idxName = "RectangularFit";
        }
        else if(index == rsgis_roundness)
        {
            idxName = "Roundness";
        }
        else if(index == rsgis_shapeindex)
        {
            idxName = "ShapeIndex";
        }
        else
        {
            throw rsgis::RSGISException("");
        }
        
        return idxName;
    }
    
    RSGISCalcClumpShapeParameters::~RSGISCalcClumpShapeParameters()
    {
        
    }
    
    
    
    
    
    
    
    
    RSGISCalcShapePrimativesAreaPxlLocs::RSGISCalcShapePrimativesAreaPxlLocs(size_t *pxlCount, std::vector<ShapePoint*> **pts, size_t numRows, bool needNumPxls, bool needPxlLocs) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->pxlCount = pxlCount;
        this->pts = pts;
        this->numRows = numRows;
        this->needNumPxls = needNumPxls;
        this->needPxlLocs = needPxlLocs;
    }

    void RSGISCalcShapePrimativesAreaPxlLocs::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {            
            if((bandValues[0] > 0) & (bandValues[0] < numRows))
            {
                size_t fid = 0;
                try
                {
                    fid = boost::lexical_cast<size_t>(bandValues[0]);
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                
                if(needNumPxls)
                {
                    ++pxlCount[fid];
                }
                
                if(needPxlLocs)
                {
                    ShapePoint *pt = new ShapePoint();
                    pt->x = extent.getMinX() + (extent.getWidth()/2);
                    pt->y = extent.getMinY() + (extent.getHeight()/2);
                    pts[fid]->push_back(pt);
                }
            }
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    RSGISCalcShapePrimativesAreaPxlLocs::~RSGISCalcShapePrimativesAreaPxlLocs()
    {
        
    }
    
    
    
}}
