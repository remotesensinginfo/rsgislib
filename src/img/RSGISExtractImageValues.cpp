/*
 *  RSGISExtractImageValues.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 9/10/2013.
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


#include "RSGISExtractImageValues.h"


namespace rsgis{namespace img{
	
    
    RSGISExtractImageValues::RSGISExtractImageValues()
    {
        
    }
    
    void RSGISExtractImageValues::extractDataWithinMask2HDF(GDALDataset *mask, GDALDataset *image, std::string outHDFFile, float maskValue) throw(RSGISImageException)
    {
        try
        {
            if(mask->GetRasterCount() != 1)
            {
                throw RSGISImageException("Image mask must only have 1 image band.");
            }
            unsigned int numImageBands = image->GetRasterCount();
            
            
            std::vector<float*> *pxlVals = new std::vector<float*>();
            
            RSGISExtractImageValuesWithMask *extractData = new RSGISExtractImageValuesWithMask(pxlVals, maskValue);
            RSGISCalcImage calcImg = RSGISCalcImage(extractData, "", true);
            
            GDALDataset **datasets = new GDALDataset*[2];
			datasets[0] = mask;
			datasets[1] = image;
            
            calcImg.calcImage(datasets, 2);
            
            delete[] datasets;
            
            rsgis::utils::RSGISExportColumnData2HDF exportCols2HDF;
            exportCols2HDF.createFile(outHDFFile, numImageBands, std::string("Pixels Extracted from ")+std::string(image->GetFileList()[0]), H5::PredType::IEEE_F32LE);
            float *row = new float[numImageBands];
            for(unsigned int j = 0; j < pxlVals->size(); ++j)
            {
                for(unsigned int i = 0; i < numImageBands; ++i)
                {
                    row[i] = pxlVals->at(j)[i];
                }
                exportCols2HDF.addDataRow(row, H5::PredType::NATIVE_FLOAT);
            }
            exportCols2HDF.close();
            delete[] row;
            
            for(std::vector<float*>::iterator iterVals = pxlVals->begin(); iterVals != pxlVals->end(); ++iterVals)
            {
                delete[] *iterVals;
            }
            delete pxlVals;
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }
    
    RSGISExtractImageValues::~RSGISExtractImageValues()
    {
        
    }
    
    
	
    RSGISExtractImageValuesWithMask::RSGISExtractImageValuesWithMask(std::vector<float*> *pxlVals, float maskValue): RSGISCalcImageValue(0)
    {
        this->pxlVals = pxlVals;
        this->maskValue = maskValue;
    }
    
    void RSGISExtractImageValuesWithMask::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        if(bandValues[0] == maskValue)
        {
            float *data = new float[numBands-1];
            for(unsigned i = 1; i < numBands; ++i)
            {
                data[i-1] = bandValues[i];
            }
            pxlVals->push_back(data);
        }
    }
    
    RSGISExtractImageValuesWithMask::~RSGISExtractImageValuesWithMask()
    {
        
    }
    
    
    
    
    
    
    
    
    RSGISExtractPxlsAsPts::RSGISExtractPxlsAsPts()
    {
        
    }
    
    void RSGISExtractPxlsAsPts::exportPixelsAsPoints(GDALDataset *image, OGRLayer *vecLayer, float maskVal) throw(RSGISImageException)
    {
        try
        {
            RSGISExtractPxlsAsPtsImgCalc *extractPxls = new RSGISExtractPxlsAsPtsImgCalc(vecLayer, maskVal);
            RSGISCalcImage calcImg = RSGISCalcImage(extractPxls, "", true);
            
            calcImg.calcImageExtent(&image, 1);
            
            delete extractPxls;
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }
    
    void RSGISExtractPxlsAsPts::exportPixelsAsPoints(GDALDataset *image, float maskVal, std::vector<std::pair<double,double> > *pxPts, geos::geom::Envelope *env) throw(RSGISImageException)
    {
        try
        {
            RSGISExtractPxlsAsPts2VecImgCalc extractPxls = RSGISExtractPxlsAsPts2VecImgCalc(pxPts, maskVal);
            RSGISCalcImage calcImg = RSGISCalcImage(&extractPxls, "", true);
            if(env == NULL)
            {
                calcImg.calcImageExtent(&image, 1);
            }
            else
            {
                calcImg.calcImageExtent(&image, 1, env);
            }
            
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }
    
    void RSGISExtractPxlsAsPts::exportPixelsAsPointsWithVal(GDALDataset *image, float maskVal, GDALDataset *valImg, int valImgBand, std::vector<std::pair<std::pair<double,double>,double> > *pxPts, bool quiet, geos::geom::Envelope *env) throw(RSGISImageException)
    {
        try
        {
            if((valImgBand-1) >= valImg->GetRasterCount())
            {
                throw RSGISImageException("Specified value band is not in the image.");
            }
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = image;
            datasets[1] = valImg;
            
            unsigned int bandIdx = image->GetRasterCount() + (valImgBand-1);
            
            RSGISExtractPxlsAsPts2VecWithValImgCalc extractPxls = RSGISExtractPxlsAsPts2VecWithValImgCalc(pxPts, maskVal, bandIdx);
            RSGISCalcImage calcImg = RSGISCalcImage(&extractPxls, "", true);
            if(env == NULL)
            {
                calcImg.calcImageExtent(datasets, 2, NULL, quiet);
            }
            else
            {
                calcImg.calcImageExtent(datasets, 2, env, quiet);
            }
            delete[] datasets;
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }
    
    RSGISExtractPxlsAsPts::~RSGISExtractPxlsAsPts()
    {
        
    }
    

    RSGISExtractPxlsAsPtsImgCalc::RSGISExtractPxlsAsPtsImgCalc(OGRLayer *vecLayer, float maskValue) : RSGISCalcImageValue(0)
    {
        this->vecLayer = vecLayer;
        this->maskValue = maskValue;
        this->featDefn = vecLayer->GetLayerDefn();
    }
    
    void RSGISExtractPxlsAsPtsImgCalc::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
    {
        if(bandValues[0] == maskValue)
        {
            geos::geom::Coordinate centre;
            extent.centre(centre);
            
            OGRFeature *poFeature = new OGRFeature(featDefn);
            OGRPoint *pt = new OGRPoint(centre.x, centre.y, 0.0);
            poFeature->SetGeometryDirectly(pt);
            vecLayer->CreateFeature(poFeature);
        }
    }

    RSGISExtractPxlsAsPtsImgCalc::~RSGISExtractPxlsAsPtsImgCalc()
    {
        
    }

    

    RSGISExtractPxlsAsPts2VecImgCalc::RSGISExtractPxlsAsPts2VecImgCalc(std::vector<std::pair<double,double> > *pxPts, float maskValue):RSGISCalcImageValue(0)
    {
        this->pxPts = pxPts;
        this->maskValue = maskValue;
    }
    
    void RSGISExtractPxlsAsPts2VecImgCalc::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
    {
        if(bandValues[0] == maskValue)
        {
            geos::geom::Coordinate centre;
            extent.centre(centre);
            pxPts->push_back(std::pair<double,double>(centre.x, centre.y));
        }
    }
    
    RSGISExtractPxlsAsPts2VecImgCalc::~RSGISExtractPxlsAsPts2VecImgCalc()
    {
        
    }
    
    RSGISExtractPxlsAsPts2VecWithValImgCalc::RSGISExtractPxlsAsPts2VecWithValImgCalc(std::vector<std::pair<std::pair<double,double>,double> > *pxPts, float maskValue, int valIdx):RSGISCalcImageValue(0)
    {
        this->pxPts = pxPts;
        this->maskValue = maskValue;
        this->valIdx = valIdx;
    }
    
    void RSGISExtractPxlsAsPts2VecWithValImgCalc::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
    {
        if(bandValues[0] == maskValue)
        {
            geos::geom::Coordinate centre;
            extent.centre(centre);
            pxPts->push_back(std::pair<std::pair<double,double>, double>(std::pair<double,double>(centre.x, centre.y), bandValues[valIdx]));
        }
    }
    
    RSGISExtractPxlsAsPts2VecWithValImgCalc::~RSGISExtractPxlsAsPts2VecWithValImgCalc()
    {
        
    }

    
    
    
}}


