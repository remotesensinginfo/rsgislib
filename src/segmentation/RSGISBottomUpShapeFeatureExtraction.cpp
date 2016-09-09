/*
 *  RSGISBottomUpShapeFeatureExtraction.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/06/2013.
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


#include "RSGISBottomUpShapeFeatureExtraction.h"

namespace rsgis{namespace segment{
   /*
    RSGISBottomUpShapeFeatureExtraction::RSGISBottomUpShapeFeatureExtraction()
    {
        
    }
    
    void RSGISBottomUpShapeFeatureExtraction::extractBrightFeatures(GDALDataset *inputImage, GDALDataset *maskImage, GDALDataset *outputImage, GDALDataset *temp1Image, GDALDataset *temp2Image, float initThres, float thresIncrement, float thresholdUpper, std::vector<rsgis::segment::FeatureShapeDescription*> shapeFeatDescript)throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            std::cout << "Starting to Extract features\n";
            RSGISClumpPxls clumpPxls;
            rsgis::img::RSGISImageUtils imgUtils;
            std::cout << "Set the output image to zero.\n";
            imgUtils.zerosUIntGDALDataset(outputImage);
            std::cout << "Clump the Mask.\n";
            clumpPxls.performClump(maskImage, temp1Image, true, 0);
            
            this->outputFeatID = 0;
            shapeIndexes = new std::vector<rsgis::rastergis::RSGISShapeParam*>();
            rsgis::rastergis::RSGISShapeParam *shpParamArea = new rsgis::rastergis::RSGISShapeParam();
            shpParamArea->idx = rsgis::rastergis::rsgis_shapearea;
            shpParamArea->colName = "Area";
            shapeIndexes->push_back(shpParamArea);
            rsgis::rastergis::RSGISShapeParam *shpParamLenWidth = new rsgis::rastergis::RSGISShapeParam();
            shpParamLenWidth->idx = rsgis::rastergis::rsgis_lengthwidth;
            shpParamLenWidth->colName = "LengthWidth";
            shapeIndexes->push_back(shpParamLenWidth);
            
            GDALDataset *tempDataset = NULL;
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = temp1Image;
            datasets[1] = inputImage;
            RSGISApplyThresholdToFeatureImage *calcApplyThreshold = new RSGISApplyThresholdToFeatureImage(initThres);
            rsgis::img::RSGISCalcImage imgCalcApplyThres = rsgis::img::RSGISCalcImage(calcApplyThreshold);
            
            GDALDataset **growSeedsDatasets = new GDALDataset*[2];
            RSGISGrowFeatures *growFeatures = new RSGISGrowFeatures();
            rsgis::img::RSGISCalcImage imgCalcGrowFeatures = rsgis::img::RSGISCalcImage(growFeatures);
            
            // Loop through thresholds
            float threshold = initThres;
            bool expandSeeds = true;
            bool growthChange = false;
            while(expandSeeds)
            {
                std::cout << "Splitting Mask using threshold " << threshold << std::endl;
                std::cout << "Find and copy any features meeting the shape requirements to the output image.\n";
                this->foundFeaturesCopyToOutput(outputImage, temp1Image, shapeFeatDescript);
                
                // Apply the threshold to the temp image.
                std::cout << "Apply Threshold to create islands\n";
                calcApplyThreshold->setThreshold(threshold);
                imgCalcApplyThres.calcImage(datasets, 2, temp1Image);
                
                // Clump the temp image ignoring the regions to be grown into (-1) and the background value zero.
                std::cout << "Clump the islands\n";
                clumpPxls.performClumpPosVals(temp1Image, temp2Image);
                tempDataset = temp1Image;
                temp1Image = temp2Image;
                temp2Image = tempDataset;
                
                // Grow the seeds.
                std::cout << "Grow the islands\n";
                growSeedsDatasets[0] = temp1Image;
                growSeedsDatasets[1] = temp2Image;
                growthChange = true;
                int iterCount = 0;
                while(growthChange)
                {
                    growthChange = false;
                    growFeatures->resetChange();
                    imgCalcGrowFeatures.calcImageWindowData(growSeedsDatasets, 2, temp1Image, 3);
                    growthChange = growFeatures->getChange();
                    std::cout << "\tNumber of Change Pixels = " << growFeatures->getNumChangePxls() << std::endl;
                    ++iterCount;
                    if(iterCount>20)
                    {
                        growthChange = false;
                        break;
                    }
                }
                
                threshold += thresIncrement;
                if(threshold > thresholdUpper)
                {
                    expandSeeds = false;
                }
            }
            
            delete calcApplyThreshold;
            delete[] datasets;
            
            delete growFeatures;
            delete[] growSeedsDatasets;
            
            for(std::vector<rsgis::rastergis::RSGISShapeParam*>::iterator iterIndexes = shapeIndexes->begin(); iterIndexes != shapeIndexes->end(); ++iterIndexes)
            {
                delete *iterIndexes;
            }
            delete shapeIndexes;
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
    
    void RSGISBottomUpShapeFeatureExtraction::foundFeaturesCopyToOutput(GDALDataset *outputImage, GDALDataset *featsImage, std::vector<rsgis::segment::FeatureShapeDescription*> shapeFeatDescript)throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            rsgis::rastergis::RSGISCalcClumpShapeParameters calcClumpShps;
            calcClumpShps.calcClumpShapeParams(featsImage, shapeIndexes);
            
            unsigned int areaIdx = 0;
            unsigned int lenWidthIdx = 0;
            for(std::vector<rsgis::rastergis::RSGISShapeParam*>::iterator iterShps = shapeIndexes->begin(); iterShps != shapeIndexes->end(); ++iterShps)
            {
                if((*iterShps)->idx == rsgis::rastergis::rsgis_shapearea)
                {
                    areaIdx = (*iterShps)->colIdx;
                }
                else if((*iterShps)->idx == rsgis::rastergis::rsgis_lengthwidth)
                {
                    lenWidthIdx = (*iterShps)->colIdx;
                }
            }
            
            const GDALRasterAttributeTable *attTableTmp = featsImage->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *attTable = NULL;
            if(attTableTmp != NULL)
            {
                attTable = new GDALRasterAttributeTable(*attTableTmp);
            }
            else
            {
                throw rsgis::RSGISException("Attribute table was not available, cannot continue.");
            }
            
            unsigned int outIdx = attUtils.findColumnIndexOrCreate(attTable, "FinalFeature", GFT_Integer);
            
            unsigned int numRows = attTable->GetRowCount();

            double area = 0;
            double lenwidth = 0;
            bool requirementsMet = true;
            attTable->SetValue(0, outIdx, 0);
            for(size_t i = 1; i < numRows; ++i)
            {
                // Get Area
                area = attTable->GetValueAsDouble(i, areaIdx);
                // Get Length / Width
                lenwidth = attTable->GetValueAsDouble(i, lenWidthIdx);
                
                //std::cout << "Area = " << area << " length/Width = " << lenwidth << std::endl;
                
                requirementsMet = true;
                for(std::vector<rsgis::segment::FeatureShapeDescription*>::iterator iterDescripts = shapeFeatDescript.begin(); iterDescripts != shapeFeatDescript.end(); ++iterDescripts)
                {
                    //std::cout << "Area " << area << " (" << (*iterDescripts)->areaLower << "," << (*iterDescripts)->areaUpper << ")\n";
                    //std::cout << "LengthWidth " << lenwidth << " (" << (*iterDescripts)->lenWidthLower << "," << (*iterDescripts)->lenWidthUpper << ")\n";
                    if(area < (*iterDescripts)->areaLower)
                    {
                        requirementsMet = false;
                        break;
                    }
                    else if(area > (*iterDescripts)->areaUpper)
                    {
                        requirementsMet = false;
                        break;
                    }
                    else if(lenwidth < (*iterDescripts)->lenWidthLower)
                    {
                        requirementsMet = false;
                        break;
                    }
                    else if(lenwidth > (*iterDescripts)->lenWidthUpper)
                    {
                        requirementsMet = false;
                        break;
                    }
                }
                    
                if(requirementsMet)
                {
                    attTable->SetValue(i, outIdx, this->outputFeatID++);
                }
                else
                {
                    attTable->SetValue(i, outIdx, 0);
                }
            }
            
            featsImage->GetRasterBand(1)->SetDefaultRAT(attTable);
            
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = featsImage;
            datasets[1] = outputImage;
            rsgis::img::RSGISCalcImageValue *calcOutFeats = new RSGISAddFeaturesToOutputImage(attTable, outIdx);
            rsgis::img::RSGISCalcImage imgCalcOutputFeats = rsgis::img::RSGISCalcImage(calcOutFeats);
            imgCalcOutputFeats.calcImage(datasets, 2, outputImage);
            delete calcOutFeats;
            delete[] datasets;
            
            rsgis::img::RSGISCalcImageValue *calcOutMask = new RSGISMaskFeaturesFromFeatureImage(attTable, outIdx);
            rsgis::img::RSGISCalcImage imgCalcMaskFeats = rsgis::img::RSGISCalcImage(calcOutMask);
            imgCalcMaskFeats.calcImage(&featsImage, 1, featsImage);
            delete calcOutMask;
            
            
            
            delete attTable;
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
    
    RSGISBottomUpShapeFeatureExtraction::~RSGISBottomUpShapeFeatureExtraction()
    {
        
    }
    
    
    
    
    
    RSGISAddFeaturesToOutputImage::RSGISAddFeaturesToOutputImage(GDALRasterAttributeTable *attTable, unsigned int featIdx) : rsgis::img::RSGISCalcImageValue(1)
    {
        this->attTable = attTable;
        this->featIdx = featIdx;
        this->numRows = attTable->GetRowCount();
    }
    
    void RSGISAddFeaturesToOutputImage::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
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
                
                int val = attTable->GetValueAsInt(fid, featIdx);
                
                output[0] = val;
            }
            else
            {
                output[0] = bandValues[1];
            }
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
    
    RSGISAddFeaturesToOutputImage::~RSGISAddFeaturesToOutputImage()
    {
        
    }
    
    
    
    
    RSGISMaskFeaturesFromFeatureImage::RSGISMaskFeaturesFromFeatureImage(GDALRasterAttributeTable *attTable, unsigned int featIdx) : rsgis::img::RSGISCalcImageValue(1)
    {
        this->attTable = attTable;
        this->featIdx = featIdx;
        this->numRows = attTable->GetRowCount();
    }
    
    void RSGISMaskFeaturesFromFeatureImage::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
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
                
                int val = attTable->GetValueAsInt(fid, featIdx);
                if(val > 0)
                {
                    output[0] = 0;
                }
                else
                {
                    output[0] = bandValues[0];
                }
            }
            else
            {
                output[0] = bandValues[0];
            }
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
    
    RSGISMaskFeaturesFromFeatureImage::~RSGISMaskFeaturesFromFeatureImage()
    {
        
    }
    
    
    
    RSGISApplyThresholdToFeatureImage::RSGISApplyThresholdToFeatureImage(float threshold) : rsgis::img::RSGISCalcImageValue(1)
    {
        this->threshold = threshold;
    }
    
    void RSGISApplyThresholdToFeatureImage::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            if(bandValues[0] > 0)
            {
                if(bandValues[1] < threshold)
                {
                    output[0] = -1;
                }
                else
                {
                    output[0] = bandValues[0];
                }
            }
            else
            {
                output[0] = bandValues[0];
            }
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
    
    RSGISApplyThresholdToFeatureImage::~RSGISApplyThresholdToFeatureImage()
    {
        
    }
    
    
    RSGISGrowFeatures::RSGISGrowFeatures():rsgis::img::RSGISCalcImageValue(1)
    {
        
    }
    
    void RSGISGrowFeatures::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            int midWin = (winSize-1)/2;
            if(dataBlock[1][midWin][midWin] == -1)
            {
                if(dataBlock[0][midWin-1][midWin] > 0)
                {
                    output[0] = dataBlock[0][midWin-1][midWin];
                    change = true;
                    ++numChangePxls;
                }
                else if(dataBlock[0][midWin+1][midWin] > 0)
                {
                    output[0] = dataBlock[0][midWin+1][midWin];
                    change = true;
                    ++numChangePxls;
                }
                else if(dataBlock[0][midWin][midWin-1] > 0)
                {
                    output[0] = dataBlock[0][midWin][midWin-1];
                    change = true;
                    ++numChangePxls;
                }
                else if(dataBlock[0][midWin][midWin+1] > 0)
                {
                    output[0] = dataBlock[0][midWin][midWin+1];
                    change = true;
                    ++numChangePxls;
                }
                else
                {
                    output[0] = dataBlock[0][midWin][midWin];
                }
            }
            else
            {
                output[0] = dataBlock[0][midWin][midWin];
            }
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
    
    RSGISGrowFeatures::~RSGISGrowFeatures()
    {
        
    }
    */
    
}}
