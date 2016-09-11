/*
 *  RSGISCmdImageMorphology.cpp
 *
 *
 *  Created by Pete Bunting on 10/09/2016.
 *  Copyright 2016 RSGISLib.
 *
 *  RSGISLib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RSGISLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISCmdImageMorphology.h"
#include "RSGISCmdParent.h"

#include "img/RSGISCalcImage.h"
#include "filtering/RSGISMorphologyDilate.h"
#include "filtering/RSGISMorphologyErode.h"
#include "filtering/RSGISMorphologyGradient.h"
#include "filtering/RSGISMorphologyFindExtrema.h"
#include "filtering/RSGISMorphologyClosing.h"
#include "filtering/RSGISMorphologyOpening.h"
#include "filtering/RSGISMorphologyTopHat.h"
#include "img/RSGISImageUtils.h"

#include "utils/RSGISFileUtils.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISMatrices.h"


namespace rsgis{ namespace cmds {
    
    
    /** A function to create a circular morphological operator */
    void executeCreateCircularOperator(std::string morphOperatorFile, unsigned int morphOpSize)throw(RSGISCmdException)
    {
        try
        {
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
            matrixUtils.makeCircularBinaryMatrix(matrixOperator);
            matrixUtils.saveMatrix2GridTxt(matrixOperator, morphOperatorFile);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    /** A function to perform a morphological dilation on an image */
    void executeImageDilate(std::string inImage, std::string outImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = NULL;
            if(useOperatorFile)
            {
                matrixOperator = matrixUtils.readMatrixFromGridTxt(morphOperatorFile);
            }
            else
            {
                matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
                matrixUtils.setValues(matrixOperator, 1);
            }
            
            rsgis::filter::RSGISImageMorphologyDilate morphObj;
            morphObj.dilateImage(&dataset, outImage, matrixOperator, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    /** A function to perform a morphological erosion on an image */
    void executeImageErode(std::string inImage, std::string outImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = NULL;
            if(useOperatorFile)
            {
                matrixOperator = matrixUtils.readMatrixFromGridTxt(morphOperatorFile);
            }
            else
            {
                matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
                matrixUtils.setValues(matrixOperator, 1);
            }
            
            rsgis::filter::RSGISImageMorphologyErode morphObj;
            morphObj.erodeImage(&dataset, outImage, matrixOperator, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    /** A function to calculate a morphological gradiance for an image */
    void executeImageGradiant(std::string inImage, std::string outImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = NULL;
            if(useOperatorFile)
            {
                matrixOperator = matrixUtils.readMatrixFromGridTxt(morphOperatorFile);
            }
            else
            {
                matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
                matrixUtils.setValues(matrixOperator, 1);
            }
            
            rsgis::filter::RSGISImageMorphologyGradient morphObj;
            morphObj.calcGradientImage(&dataset, outImage, matrixOperator, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    /** A function to perform a morphological dilation on an image combining the results of the output bands into a single image band */
    void executeImageDilateCombinedOut(std::string inImage, std::string outImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = NULL;
            if(useOperatorFile)
            {
                matrixOperator = matrixUtils.readMatrixFromGridTxt(morphOperatorFile);
            }
            else
            {
                matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
                matrixUtils.setValues(matrixOperator, 1);
            }
            
            rsgis::filter::RSGISImageMorphologyDilate morphObj;
            morphObj.dilateImageAll(&dataset, outImage, matrixOperator, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    /** A function to perform a morphological erosion on an image combining the results of the output bands into a single image band */
    void executeImageErodeCombinedOut(std::string inImage, std::string outImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = NULL;
            if(useOperatorFile)
            {
                matrixOperator = matrixUtils.readMatrixFromGridTxt(morphOperatorFile);
            }
            else
            {
                matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
                matrixUtils.setValues(matrixOperator, 1);
            }
            
            rsgis::filter::RSGISImageMorphologyErode morphObj;
            morphObj.erodeImageAll(&dataset, outImage, matrixOperator, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    /** A function to calculate a morphological gradiance for an image combining the results of the output bands into a single image band */
    void executeImageGradiantCombinedOut(std::string inImage, std::string outImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = NULL;
            if(useOperatorFile)
            {
                matrixOperator = matrixUtils.readMatrixFromGridTxt(morphOperatorFile);
            }
            else
            {
                matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
                matrixUtils.setValues(matrixOperator, 1);
            }
            
            rsgis::filter::RSGISImageMorphologyGradient morphObj;
            morphObj.calcGradientImageAll(&dataset, outImage, matrixOperator, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    /** A function to perform a morphological operation to find local minima */
    void executeImageLocalMinima(std::string inImage, std::string outImage, bool outputSequencial, bool allowEquals, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = NULL;
            if(useOperatorFile)
            {
                matrixOperator = matrixUtils.readMatrixFromGridTxt(morphOperatorFile);
            }
            else
            {
                matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
                matrixUtils.setValues(matrixOperator, 1);
            }
            
            rsgis::filter::RSGISImageMorphologyFindExtrema::RSGISMinimaOutputs minOutType = rsgis::filter::RSGISImageMorphologyFindExtrema::binary;
            if(outputSequencial)
            {
                minOutType = rsgis::filter::RSGISImageMorphologyFindExtrema::sequential;
            }
            else
            {
                minOutType = rsgis::filter::RSGISImageMorphologyFindExtrema::binary;
            }
            
            rsgis::filter::RSGISImageMorphologyFindExtrema morphObj;
            morphObj.findMinima(&dataset, outImage, matrixOperator, minOutType, allowEquals, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    /** A function to perform a morphological operation to find local minima combining the results of the output bands into a single image band */
    void executeImageLocalMinimaCombinedOut(std::string inImage, std::string outImage, bool outputSequencial, bool allowEquals, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = NULL;
            if(useOperatorFile)
            {
                matrixOperator = matrixUtils.readMatrixFromGridTxt(morphOperatorFile);
            }
            else
            {
                matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
                matrixUtils.setValues(matrixOperator, 1);
            }
            
            rsgis::filter::RSGISImageMorphologyFindExtrema::RSGISMinimaOutputs minOutType = rsgis::filter::RSGISImageMorphologyFindExtrema::binary;
            if(outputSequencial)
            {
                minOutType = rsgis::filter::RSGISImageMorphologyFindExtrema::sequential;
            }
            else
            {
                minOutType = rsgis::filter::RSGISImageMorphologyFindExtrema::binary;
            }
            
            rsgis::filter::RSGISImageMorphologyFindExtrema morphObj;
            morphObj.findMinimaAll(&dataset, outImage, matrixOperator, minOutType, allowEquals, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    /** A function to perform a morphological opening on an image */
    void executeImageOpening(std::string inImage, std::string outImage, std::string tmpImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, unsigned int numIterations, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = NULL;
            if(useOperatorFile)
            {
                matrixOperator = matrixUtils.readMatrixFromGridTxt(morphOperatorFile);
            }
            else
            {
                matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
                matrixUtils.setValues(matrixOperator, 1);
            }
            
            bool useMemory = false;
            if(tmpImage == "")
            {
                useMemory = true;
            }
            
            rsgis::filter::RSGISImageMorphologyOpening morphObj;
            morphObj.performOpening(dataset, outImage, tmpImage, useMemory, matrixOperator, numIterations, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    /** A function to perform a morphological closing on an image */
    void executeImageClosing(std::string inImage, std::string outImage, std::string tmpImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, unsigned int numIterations, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = NULL;
            if(useOperatorFile)
            {
                matrixOperator = matrixUtils.readMatrixFromGridTxt(morphOperatorFile);
            }
            else
            {
                matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
                matrixUtils.setValues(matrixOperator, 1);
            }
            
            bool useMemory = false;
            if(tmpImage == "")
            {
                useMemory = true;
            }
            
            rsgis::filter::RSGISImageMorphologyClosing morphObj;
            morphObj.performClosing(dataset, outImage, tmpImage, useMemory, matrixOperator, numIterations, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    /** A function to perform a morphological black top hat on an image */
    void executeImageBlackTopHat(std::string inImage, std::string outImage, std::string tmpImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = NULL;
            if(useOperatorFile)
            {
                matrixOperator = matrixUtils.readMatrixFromGridTxt(morphOperatorFile);
            }
            else
            {
                matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
                matrixUtils.setValues(matrixOperator, 1);
            }
            
            bool useMemory = false;
            if(tmpImage == "")
            {
                useMemory = true;
            }
            
            rsgis::filter::RSGISImageMorphologyTopHat morphObj;
            morphObj.performBlackTopHat(dataset, outImage, tmpImage, useMemory, matrixOperator, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    /** A function to perform a morphological white top hat on an image */
    void executeImageWhiteTopHat(std::string inImage, std::string outImage, std::string tmpImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::math::RSGISMatrices matrixUtils;
            rsgis::math::Matrix *matrixOperator = NULL;
            if(useOperatorFile)
            {
                matrixOperator = matrixUtils.readMatrixFromGridTxt(morphOperatorFile);
            }
            else
            {
                matrixOperator = matrixUtils.createMatrix(morphOpSize, morphOpSize);
                matrixUtils.setValues(matrixOperator, 1);
            }
            
            bool useMemory = false;
            if(tmpImage == "")
            {
                useMemory = true;
            }
            
            rsgis::filter::RSGISImageMorphologyTopHat morphObj;
            morphObj.performWhiteTopHat(dataset, outImage, tmpImage, useMemory, matrixOperator, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            matrixUtils.freeMatrix(matrixOperator);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    

    
}}



