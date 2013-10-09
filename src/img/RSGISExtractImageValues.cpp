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
    
}}


