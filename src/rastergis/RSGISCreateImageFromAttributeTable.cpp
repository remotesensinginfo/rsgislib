/*
 *  RSGISCreateImageFromAttributeTable.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/02/2012.
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

#include "RSGISCreateImageFromAttributeTable.h"


namespace rsgis{namespace rastergis{
    

    RSGISCreateImageFromAttributeTable::RSGISCreateImageFromAttributeTable()
    {
        
    }
        
    void RSGISCreateImageFromAttributeTable::createImageFromAttTable(GDALDataset *clumpsDataset, std::string outputImage, RSGISAttributeTable *attTable, std::vector<std::pair<unsigned int, std::string> > *bands, std::string outImageFormat)throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException)
    {
        std::pair<RSGISAttributeDataType, unsigned int> *attBands = new std::pair<RSGISAttributeDataType, unsigned int>[bands->size()];
        std::string *outputNames = new std::string[bands->size()];
        for(unsigned int i = 0; i < bands->size(); ++i)
        {
            outputNames[i] = bands->at(i).second;
            attBands[i].first = attTable->getDataType(bands->at(i).second);
            attBands[i].second = attTable->getFieldIndex(bands->at(i).second);
        }
        
        GDALDataset **datasets = new GDALDataset*[1];
        datasets[0] = clumpsDataset;
        RSGISCreateImageFromAttributeTableCalcImg *popImgBands = new RSGISCreateImageFromAttributeTableCalcImg(bands->size(), attTable, attBands);
        rsgis::img::RSGISCalcImage calcImage(popImgBands);
        calcImage.calcImage(datasets, 1, outputImage, true, outputNames, outImageFormat);
        delete popImgBands;

        delete[] datasets;
    }
    
    RSGISCreateImageFromAttributeTable::~RSGISCreateImageFromAttributeTable()
    {
        
    }

    
    
    
    
    
    RSGISCreateImageFromAttributeTableCalcImg::RSGISCreateImageFromAttributeTableCalcImg(int numberOutBands, RSGISAttributeTable *attTable, std::pair<RSGISAttributeDataType, unsigned int> *attBands): RSGISCalcImageValue(numberOutBands)
    {
        this->attTable = attTable;
        this->attBands = attBands;
    }
    
    void RSGISCreateImageFromAttributeTableCalcImg::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        for(int i = 0; i < this->numOutBands; ++i)
        {
            output[i] = 0;
        }
        
        try
        {
            clumpIdx = boost::lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            try
            {
                RSGISFeature *feat = attTable->getFeature(clumpIdx);
                
                for(int i = 0; i < this->numOutBands; ++i)
                {
                    if(attBands[i].first == rsgis_bool)
                    {
                        output[i] = feat->boolFields->at(attBands[i].second);
                    }
                    else if(attBands[i].first == rsgis_int)
                    {
                        output[i] = feat->intFields->at(attBands[i].second);
                    }
                    else if(attBands[i].first == rsgis_float)
                    {
                        //std::cout << "clumpIdx = " << clumpIdx << " band[" << i << "]:\t" << feat->floatFields->at(attBands[i].second) << std::endl;
                        output[i] = feat->floatFields->at(attBands[i].second);
                    }
                    else
                    {
                        throw rsgis::img::RSGISImageCalcException("Data type is either not recognised or cannot be written to an image.");
                    }
                    
                }
                
            }
            catch(RSGISAttributeTableException &e)
            {
                std::cout << "clumpIdx = " << clumpIdx << std::endl;
                throw rsgis::img::RSGISImageCalcException(e.what());
            }
            catch(rsgis::img::RSGISImageCalcException &e)
            {
                std::cout << "clumpIdx = " << clumpIdx << std::endl;
                throw e;
            }
        }
    }
    
    RSGISCreateImageFromAttributeTableCalcImg::~RSGISCreateImageFromAttributeTableCalcImg()
    {
        
    }
    
    
    
    RSGISAttributeTableImageUtils::RSGISAttributeTableImageUtils(RSGISAttributeTable *attTable)
    {
        this->attTable = attTable;
    }
    
    void RSGISAttributeTableImageUtils::populateGDALDataset(GDALDataset *clumps, GDALDataset *output, RSGISAttributeDataType dataType, unsigned int attributeIdx)throw(RSGISImageException, RSGISAttributeTableException)
    {
        if(clumps->GetRasterXSize() != output->GetRasterXSize())
        {
            throw RSGISImageException("The image size in the X axis is different.");
        }
        
        if(clumps->GetRasterYSize() != output->GetRasterYSize())
        {
            throw RSGISImageException("The image size in the Y axis is different.");
        }
        
        unsigned int width = clumps->GetRasterXSize();
        unsigned int height = clumps->GetRasterYSize();
        
        unsigned int *clumpIdxs = new unsigned int[width];
        void *outData = NULL;
        
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        GDALRasterBand *outBand = output->GetRasterBand(1);
        
        if(outBand->GetRasterDataType() == GDT_UInt32)
        {
            outData = new unsigned int[width];
        }
        else
        {
            outData = new float[width];
        }
        
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] > 0)
                {
                    --clumpIdxs[j];
                    try
                    {
                        RSGISFeature *feat = attTable->getFeature(clumpIdxs[j]);
                        
                        if(dataType == rsgis_bool)
                        {
                            if(outBand->GetRasterDataType() == GDT_UInt32)
                            {
                                ((unsigned int*)outData)[j] = feat->boolFields->at(attributeIdx);
                            }
                            else
                            {
                                ((float*)outData)[j] = feat->boolFields->at(attributeIdx);
                            }
                        }
                        else if(dataType == rsgis_int)
                        {
                            if(outBand->GetRasterDataType() == GDT_UInt32)
                            {
                                ((unsigned int*)outData)[j] = feat->intFields->at(attributeIdx);
                            }
                            else
                            {
                                ((float*)outData)[j] = feat->intFields->at(attributeIdx);
                            }
                        }
                        else if(dataType == rsgis_float)
                        {
                            if(outBand->GetRasterDataType() == GDT_UInt32)
                            {
                                ((unsigned int*)outData)[j] = feat->floatFields->at(attributeIdx);
                            }
                            else
                            {
                                ((float*)outData)[j] = feat->floatFields->at(attributeIdx);
                            }
                        }
                        else
                        {
                            throw RSGISImageException("Data type is either not recognised or cannot be written to an image.");
                        }
                        
                    }
                    catch(RSGISAttributeTableException &e)
                    {
                        std::cout << "clumpIdx = " << clumpIdxs[j] << std::endl;
                        throw RSGISImageException(e.what());
                    }
                    catch(RSGISImageException &e)
                    {
                        std::cout << "clumpIdx = " << clumpIdxs[j] << std::endl;
                        throw e;
                    }
                }
                else
                {
                    if(outBand->GetRasterDataType() == GDT_UInt32)
                    {
                        ((unsigned int*)outData)[j] = 0;
                    }
                    else
                    {
                        ((float*)outData)[j] = 0;
                    }

                }
            }
            if(outBand->GetRasterDataType() == GDT_UInt32)
            {
                outBand->RasterIO(GF_Write, 0, i, width, 1, outData, width, 1, GDT_UInt32, 0, 0);
            }
            else
            {
                outBand->RasterIO(GF_Write, 0, i, width, 1, outData, width, 1, GDT_Float32, 0, 0);
            }
        }
        
        outBand->FlushCache();
        output->FlushCache();
        
        
        if(outBand->GetRasterDataType() == GDT_UInt32)
        {
            delete[] ((unsigned int*)outData);
        }
        else
        {
            delete[] ((float*)outData);
        }
        
        delete[] clumpIdxs;
    }
    
    RSGISAttributeTableImageUtils::~RSGISAttributeTableImageUtils()
    {
        
    }

    
    
}}



