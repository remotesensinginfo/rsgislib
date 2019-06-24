/*
 *  RSGISImageClustering.cpp
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

#include "RSGISImageClustering.h"


namespace rsgis{namespace img{
    

    RSGISImageClustering::RSGISImageClustering()
    {
        
    }
        
    void RSGISImageClustering::findKMeansCentres(GDALDataset *dataset, std::string outputMatrix, unsigned int numClusters, unsigned int maxNumIterations, unsigned int subSample, bool ignoreZeros, float degreeOfChange, rsgis::math::InitClustererMethods initMethod)
    {
        try 
        {
            unsigned int numImgBands = dataset->GetRasterCount();
            std::cout << "Subsampling the image to read into memory\n";
            std::vector< std::vector<float> > *pxlValues = this->sampleImage(dataset, subSample, ignoreZeros);
            
            std::cout << "Performing clustering\n";
            rsgis::math::RSGISKMeansClusterer clusterer(initMethod);
            std::vector< rsgis::math::RSGISClusterCentre > *clusterCentres = clusterer.calcClusterCentres(pxlValues, numImgBands, numClusters, maxNumIterations, degreeOfChange);
            
            std::cout << "Exporting cluster centres to output file\n";
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *clusterMatrix = matrixUtils.createMatrix(numImgBands, clusterCentres->size());
            unsigned int matrixIdx = 0;
            for(unsigned int i = 0; i < clusterCentres->size(); ++i)
            {
                for(unsigned int j = 0; j < numImgBands; ++j)
                {
                    matrixIdx = (j*clusterCentres->size())+i;
                    clusterMatrix->matrix[matrixIdx] = clusterCentres->at(i).centre[j];
                }
            }
            matrixUtils.saveMatrix2GridTxt(clusterMatrix, outputMatrix);
            matrixUtils.freeMatrix(clusterMatrix);
            delete pxlValues;
            delete clusterCentres;
        }
        catch (rsgis::RSGISImageException &e) 
        {
            throw e;
        }
        catch(rsgis::math::RSGISClustererException &e)
        {
            throw e;
        }
    }
        
    
    void RSGISImageClustering::findISODataCentres(GDALDataset *dataset, std::string outputMatrix, unsigned int numClusters, unsigned int maxNumIterations, unsigned int subSample, bool ignoreZeros, float degreeOfChange, rsgis::math::InitClustererMethods initMethod, float minDistBetweenClusters, unsigned int minNumFeatures, float maxStdDev, unsigned int minNumClusters, unsigned int startIteration, unsigned int endIteration)
    {
        try 
        {
            unsigned int numImgBands = dataset->GetRasterCount();
            std::cout << "Subsampling the image to read into memory\n";
            std::vector< std::vector<float> > *pxlValues = this->sampleImage(dataset, subSample, ignoreZeros);
            
            std::cout << "Performing clustering\n";
            rsgis::math::RSGISISODataClusterer clusterer(initMethod, minDistBetweenClusters, minNumFeatures, maxStdDev, minNumClusters, startIteration, endIteration);
            std::vector< rsgis::math::RSGISClusterCentre > *clusterCentres = clusterer.calcClusterCentres(pxlValues, dataset->GetRasterCount(), numClusters, maxNumIterations, degreeOfChange);
            
            std::cout << "Exporting cluster centres to output file\n";
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *clusterMatrix = matrixUtils.createMatrix(numImgBands, clusterCentres->size());
            unsigned int matrixIdx = 0;
            for(unsigned int i = 0; i < clusterCentres->size(); ++i)
            {
                for(unsigned int j = 0; j < numImgBands; ++j)
                {
                    matrixIdx = (j*clusterCentres->size())+i;
                    clusterMatrix->matrix[matrixIdx] = clusterCentres->at(i).centre[j];
                }
            }
            matrixUtils.saveMatrix2GridTxt(clusterMatrix, outputMatrix);
            matrixUtils.freeMatrix(clusterMatrix);
            delete pxlValues;
            delete clusterCentres;
        }
        catch (rsgis::RSGISImageException &e) 
        {
            throw e;
        }
        catch(rsgis::math::RSGISClustererException &e)
        {
            throw e;
        }
    }
    
    
    std::vector< std::vector<float> >* RSGISImageClustering::sampleImage(GDALDataset *dataset, unsigned int subSample, bool ignoreZeros)
    {
        std::vector< std::vector<float> > *pxlValues = new std::vector< std::vector<float> >();
        
        unsigned int numImgBands = dataset->GetRasterCount();
        
        unsigned int width = dataset->GetRasterXSize();
        unsigned int height = dataset->GetRasterYSize();
        
        float **dataRow = new float*[numImgBands];
        GDALRasterBand **bands = new GDALRasterBand*[numImgBands];
        for(unsigned n = 0; n < numImgBands; ++n)
        {
            bands[n] = dataset->GetRasterBand(n+1);
            dataRow[n] = new float[width];
        }
        
        bool nonZeroFound = false;
        unsigned long pxlCount = 0;
        for(unsigned int i = 0; i < height; ++i)
        {
            for(unsigned n = 0; n < numImgBands; ++n)
            {
                bands[n]->RasterIO(GF_Read, 0, i, width, 1, dataRow[n], width, 1, GDT_Float32, 0, 0);
            }
            
            for(unsigned int j = 0; j < width; ++j)
            {
                if((pxlCount % subSample) == 0)
                {
                    nonZeroFound = false;
                    std::vector<float> col;
                    col.reserve(numImgBands);
                    for(unsigned n = 0; n < numImgBands; ++n)
                    {
                        col.push_back(dataRow[n][j]);
                        if(dataRow[n][j] != 0)
                        {
                            nonZeroFound = true;
                        }
                    }
                    
                    if(ignoreZeros)
                    {
                        if(nonZeroFound)
                        {
                            pxlValues->push_back(col);
                        }
                    }
                    else
                    {
                        pxlValues->push_back(col);
                    }
                }
                
                ++pxlCount;
            }
        }
        
        for(unsigned n = 0; n < numImgBands; ++n)
        {
            delete[] dataRow[n];
        }
        delete[] dataRow;
        
        delete[] bands;
        
        return pxlValues;
    }
        
    RSGISImageClustering::~RSGISImageClustering()
    {
        
    }
    
}}






