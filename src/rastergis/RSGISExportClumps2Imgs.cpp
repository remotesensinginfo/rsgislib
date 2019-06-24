/*
 *  RSGISExportClumps2Imgs.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/03/2016.
 *  Copyright 2016 RSGISLib.
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

#include "RSGISExportClumps2Imgs.h"


namespace rsgis{namespace rastergis{
    
    RSGISExportClumps2Images::RSGISExportClumps2Images()
    {
        
    }
    
    void RSGISExportClumps2Images::exportClumps2Images(GDALDataset *clumpsDataset, std::string outImgBase, std::string imgFileExt, std::string imageFormat, bool binaryOut, std::string minXPxl, std::string maxXPxl, std::string minYPxl, std::string maxYPxl, std::string tlX, std::string tlY, unsigned int ratBand)
    {
        try
        {
            RSGISRasterAttUtils attUtils;
            GDALRasterAttributeTable *attTable = clumpsDataset->GetRasterBand(ratBand)->GetDefaultRAT();
            if(attTable == NULL)
            {
                throw RSGISAttributeTableException("GDAL Dataset does not have a RAT.");
            }
            
            size_t numRows = attTable->GetRowCount();
            
            
            std::vector<int> *minXPxlVals = attUtils.readIntColumnAsVec(attTable, minXPxl);
            std::vector<int> *maxXPxlVals = attUtils.readIntColumnAsVec(attTable, maxXPxl);
            std::vector<int> *minYPxlVals = attUtils.readIntColumnAsVec(attTable, minYPxl);
            std::vector<int> *maxYPxlVals = attUtils.readIntColumnAsVec(attTable, maxYPxl);
            
            std::vector<double> *tlXVals = attUtils.readDoubleColumnAsVec(attTable, tlX);
            std::vector<double> *tlYVals = attUtils.readDoubleColumnAsVec(attTable, tlY);
            
            double geoTransform[6];
            if(clumpsDataset->GetGeoTransform(geoTransform) != CE_None)
            {
                throw rsgis::RSGISImageException("Did not retrieve the geotransform of the input image.");
            }

            std::cout << "Res: [" << geoTransform[1] << ", " << geoTransform[5] << "]\n";
            
            unsigned int xSize = 0;
            unsigned int ySize = 0;
            double *outTransform = new double[6];
            outTransform[0] = 0.0; // X Origin.
            outTransform[1] = geoTransform[1];
            outTransform[2] = geoTransform[2];
            outTransform[3] = 0.0; // Y Origin.
            outTransform[4] = geoTransform[4];
            outTransform[5] = geoTransform[5];
            
            GDALDataset *outClumpImg = NULL;
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumpsDataset;
            RSGISPopulateWithImageStats addClrTab;
            RSGISCopyImageBand4ClumpExport valueCalc = RSGISCopyImageBand4ClumpExport(ratBand, 0, binaryOut);
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(&valueCalc);
            rsgis::img::RSGISImageUtils imgUtils;
            rsgis::utils::RSGISTextUtils textUtils;
            for(size_t i = 1; i < numRows; ++i)
            {
                if( (maxXPxlVals->at(i) > 0) | (maxYPxlVals->at(i) > 0) )
                {
                    std::string outImgFileName = outImgBase + "C" + textUtils.sizettostring(i) + "." + imgFileExt;
                    std::cout << "Output Img: " << outImgFileName << std::endl;
                    
                    xSize = (maxXPxlVals->at(i) - minXPxlVals->at(i)) + 1;
                    ySize = (maxYPxlVals->at(i) - minYPxlVals->at(i)) + 1;
                    
                    std::cout << "Size: [" << xSize << ", " << ySize << "]\n";
                    std::cout << "TL: [" << tlXVals->at(i) << ", " << tlYVals->at(i) << "]\n";
                    
                    outTransform[0] = tlXVals->at(i);
                    outTransform[3] = tlYVals->at(i);
                    
                    outClumpImg = imgUtils.createBlankImage(outImgFileName, outTransform, xSize, ySize, 1, "", 0.0, imageFormat, GDT_UInt32);
                    outClumpImg->SetProjection(clumpsDataset->GetProjectionRef());
                    GDALClose(outClumpImg);
                    
                    valueCalc.setFid(i);
                    outClumpImg = (GDALDataset *) GDALOpen(outImgFileName.c_str(), GA_Update);
                    datasets[1] = outClumpImg;
                    calcImage.calcImage(datasets, 2, 0, outClumpImg);
                    addClrTab.populateImageWithRasterGISStats(outClumpImg, true, true, 1);
                    GDALClose(outClumpImg);
                    
                }
            }
            delete[] datasets;
            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch(rsgis::img::RSGISImageBandException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }

    }
    
    RSGISExportClumps2Images::~RSGISExportClumps2Images()
    {
        
    }
    
    

    RSGISCopyImageBand4ClumpExport::RSGISCopyImageBand4ClumpExport(unsigned int band, size_t fid, bool binOut): rsgis::img::RSGISCalcImageValue(1)
    {
        this->band = band;
        this->fid = fid;
        this->binOut = binOut;
    }
    
    void RSGISCopyImageBand4ClumpExport::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) 
    {
        if((band-1) >= (numIntVals-1))
        {
            throw rsgis::img::RSGISImageCalcException("The specified band is greater than the number of bands in the input image.");
        }
        
        if(intBandValues[(band-1)] == fid)
        {
            if(binOut)
            {
                output[0] = 1.0;
            }
            else
            {
                output[0] = intBandValues[(band-1)];
            }
        }
        else
        {
            output[0] = 0.0;
        }
        
    }
    
    RSGISCopyImageBand4ClumpExport::~RSGISCopyImageBand4ClumpExport()
    {
        
    }
    
}}



