/*
 *  RSGISLabelPixelsUsingClusters.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/02/2012.
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

#include "RSGISLabelPixelsUsingClusters.h"


namespace rsgis{namespace segment{
    

    RSGISLabelPixelsUsingClusters::RSGISLabelPixelsUsingClusters()
    {
        
    }
    
    void RSGISLabelPixelsUsingClusters::labelPixelsUsingClusters(GDALDataset **datasets, int numDatasets, std::string output, std::string clusterCentresFile, bool ignoreZeros, std::string imageFormat, bool useImageProj, std::string outProjStr)
    {
        try 
        {
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *clusterCentres = matrixUtils.readMatrixFromGridTxt(clusterCentresFile);
            
            std::string *bandNames = new std::string[1];
            bandNames[0] = "Clusters";
            
            RSGISLabelPixelsUsingClustersCalcImg *calcValue = new RSGISLabelPixelsUsingClustersCalcImg(1, clusterCentres, ignoreZeros);
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(calcValue, outProjStr, useImageProj);
            calcImage.calcImage(datasets, numDatasets, output, true, bandNames, imageFormat);
            
            delete calcValue;
            matrixUtils.freeMatrix(clusterCentres);
        } 
        catch (rsgis::math::RSGISMathException &e) 
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch (rsgis::RSGISImageException &e) 
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }

    RSGISLabelPixelsUsingClusters::~RSGISLabelPixelsUsingClusters()
    {
        
    }
    
    

    RSGISLabelPixelsUsingClustersCalcImg::RSGISLabelPixelsUsingClustersCalcImg(int numberOutBands, rsgis::math::Matrix *clusterCentres, bool ignoreZeros) : RSGISCalcImageValue(numberOutBands)
    {
        this->clusterCentres = clusterCentres;
        this->ignoreZeros = ignoreZeros;
    }
    
    void RSGISLabelPixelsUsingClustersCalcImg::calcImageValue(float *bandValues, int numBands, double *output) 
    {
        unsigned int clusterID = 0;
        float minDist = 0;
        float dist = 0;
        int clustIdx = 0;
        bool nonZeroFound = false;
        
        for(int cluster = 0; cluster < clusterCentres->m; ++cluster)
        {
            dist = 0;
            for(int i = 0; i < numBands; ++i)
            {
                clustIdx = (i*clusterCentres->m)+cluster;
                dist += ((bandValues[i]-clusterCentres->matrix[clustIdx])*(bandValues[i]-clusterCentres->matrix[clustIdx]));
                if(bandValues[i] != 0)
                {
                    nonZeroFound = true;
                }
            }
            
            dist = sqrt(dist);
            
            if(cluster == 0)
            {
                clusterID = cluster+1;
                minDist = dist;
            }
            else if(dist < minDist)
            {
                clusterID = cluster+1;
                minDist = dist;
            }
            
            if(ignoreZeros & !nonZeroFound)
            {
                clusterID = 0;
                break;
            }
        }
        
        output[0] = clusterID;
    }
    
    RSGISLabelPixelsUsingClustersCalcImg::~RSGISLabelPixelsUsingClustersCalcImg()
    {
        
    }
    
}}




