/*
 *  RSGISRATClassificationUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/01/2013.
 *  Copyright 2013 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 *
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISRATClassificationUtils.h"

namespace rsgis{ namespace classifier{
	
    
    
    RSGISCollapseSegmentsClassification::RSGISCollapseSegmentsClassification()
    {
        
    }
    
    void RSGISCollapseSegmentsClassification::collapseClassification(GDALDataset *segments, std::string classNameCol, std::string outputImage, std::string imageFormat) throw(RSGISClassificationException)
    {
        try
        {
            // Get attribute table...
            const GDALRasterAttributeTable *inRAT = segments->GetRasterBand(1)->GetDefaultRAT();
            
            if(inRAT == NULL)
            {
                throw RSGISClassificationException("The image dataset does not have an attribute table.");
            }
            
            // Find the columns for classname, Red, Green, blue and Alpha...
            rsgis::rastergis::RSGISRasterAttUtils ratUtils;
            unsigned int classNameIdx = ratUtils.findColumnIndex(inRAT, classNameCol);
            unsigned int redIdx = ratUtils.findColumnIndex(inRAT, "Red");
            unsigned int greenIdx = ratUtils.findColumnIndex(inRAT, "Green");
            unsigned int blueIdx = ratUtils.findColumnIndex(inRAT, "Blue");
            unsigned int alphaIdx = ratUtils.findColumnIndex(inRAT, "Alpha");
            
            // Find Unique column names alone with colour
            std::cout << "Find the class names...\n";
            std::map<std::string, RSGISClassInfo*> *classes = this->findAllClassNames(inRAT, classNameIdx, redIdx, greenIdx, blueIdx, alphaIdx);
            
            // Assign ID to each class 
            size_t id = 1;
            for(std::map<std::string, RSGISClassInfo*>::iterator iterClasses = classes->begin(); iterClasses != classes->end(); ++iterClasses)
            {
                if(iterClasses->first == "")
                {
                    iterClasses->second->classID = 0;
                }
                else
                {
                    iterClasses->second->classID = id++;
                    std::cout << iterClasses->second->classID << ":\t " << iterClasses->second->classname << ": [" << iterClasses->second->red << "," << iterClasses->second->green << "," << iterClasses->second->blue << "]\n";
                }
            }
            
            // Create the new RAT.
            GDALRasterAttributeTable *outRAT = new GDALRasterAttributeTable();
            outRAT->CreateColumn("Red", GFT_Integer, GFU_Red);
            outRAT->CreateColumn("Green", GFT_Integer, GFU_Green);
            outRAT->CreateColumn("Blue", GFT_Integer, GFU_Blue);
            outRAT->CreateColumn("Alpha", GFT_Integer, GFU_Alpha);
            outRAT->CreateColumn(classNameCol.c_str(), GFT_String, GFU_Generic);
            outRAT->SetRowCount(id);
            outRAT->SetValue(0, 0, 0);
            outRAT->SetValue(0, 1, 0);
            outRAT->SetValue(0, 2, 0);
            outRAT->SetValue(0, 3, 255);
            outRAT->SetValue(0, 4, "");
            for(std::map<std::string, RSGISClassInfo*>::iterator iterClasses = classes->begin(); iterClasses != classes->end(); ++iterClasses)
            {                
                outRAT->SetValue(iterClasses->second->classID, 0, iterClasses->second->red);
                outRAT->SetValue(iterClasses->second->classID, 1, iterClasses->second->green);
                outRAT->SetValue(iterClasses->second->classID, 2, iterClasses->second->blue);
                outRAT->SetValue(iterClasses->second->classID, 3, iterClasses->second->alpha);
                outRAT->SetValue(iterClasses->second->classID, 4, iterClasses->second->classname.c_str());
                ++id;
            }
            
            // Create new image with new RAT and pixel IDs...
            RSGISRecodeRasterFromClasses *recodeRaster = new RSGISRecodeRasterFromClasses(inRAT, classNameIdx, classes);
            rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(recodeRaster, "", true);
            calcImg.calcImage(&segments, 1, outputImage, false, NULL, imageFormat, GDT_Int32);
            delete recodeRaster;
            
            GDALDataset *imageDataset = (GDALDataset *) GDALOpen(outputImage.c_str(), GA_Update);
            if(imageDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + outputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            imageDataset->GetRasterBand(1)->SetDefaultRAT(outRAT);
            imageDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            GDALClose(imageDataset);
        }
        catch(RSGISAttributeTableException &e)
        {
            throw RSGISClassificationException(e.what());
        }
        catch (RSGISClassificationException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISClassificationException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISClassificationException(e.what());
        }
    }
    
    std::map<std::string, RSGISClassInfo*>* RSGISCollapseSegmentsClassification::findAllClassNames(const GDALRasterAttributeTable *inRAT, unsigned int classNameIdx, unsigned int redIdx, unsigned int greenIdx, unsigned int blueIdx, unsigned int alphaIdx)throw(RSGISClassificationException)
    {
        std::map<std::string, RSGISClassInfo*> *classes = new std::map<std::string, RSGISClassInfo*>();
        try
        {
            size_t numRows = inRAT->GetRowCount();
            std::string name = "";
            RSGISClassInfo *classInfo = NULL;
            size_t id = 1;
            
            for(size_t i = 0; i < numRows; ++i)
            {
                name = inRAT->GetValueAsString(i, classNameIdx);
                //std::cout << i << ": " << name << std::endl;
                if((classes->empty()) || (classes->count(name) == 0))
                {
                    classInfo = new RSGISClassInfo();
                    classInfo->classname = name;
                    classInfo->red = inRAT->GetValueAsInt(i, redIdx);
                    classInfo->green = inRAT->GetValueAsInt(i, greenIdx);
                    classInfo->blue = inRAT->GetValueAsInt(i, blueIdx);
                    classInfo->alpha = inRAT->GetValueAsInt(i, alphaIdx);
                    classInfo->classID = id++;
                    //std::cout << "Adding class: " << name << std::endl;
                    classes->insert(std::pair<std::string, RSGISClassInfo*>(name, classInfo));
                }
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw RSGISClassificationException(e.what());
        }
        catch (RSGISClassificationException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISClassificationException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISClassificationException(e.what());
        }
        
        return classes;
    }
    
    RSGISCollapseSegmentsClassification::~RSGISCollapseSegmentsClassification()
    {
        
    }
    
    
    
    RSGISRecodeRasterFromClasses::RSGISRecodeRasterFromClasses(const GDALRasterAttributeTable *rat, unsigned int classNameIdx, std::map<std::string, RSGISClassInfo*> *classes) : rsgis::img::RSGISCalcImageValue(1)
    {
        this->rat = rat;
        this->classNameIdx = classNameIdx;
        this->classes = classes;
    }
    
    void RSGISRecodeRasterFromClasses::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            int val = boost::lexical_cast<int>(bandValues[0]);
            
            std::string className = rat->GetValueAsString(val, classNameIdx);
            
            std::map<std::string, RSGISClassInfo*>::iterator iterClass = classes->find(className);
            
            if(iterClass != classes->end())
            {
                output[0] = iterClass->second->classID;
            }
            else
            {
                output[0] = 0;
            }
            
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            rsgis::img::RSGISImageCalcException(e.what());
        }
    }

    RSGISRecodeRasterFromClasses::~RSGISRecodeRasterFromClasses()
    {
        
    }
        
    
    
    
    RSGISColourImageFromClassRAT::RSGISColourImageFromClassRAT(GDALColorTable *clrTab) : rsgis::img::RSGISCalcImageValue(3)
    {
        this->clrTab = clrTab;
    }
    
    void RSGISColourImageFromClassRAT::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            int val = boost::lexical_cast<int>(bandValues[0]);
            
            const GDALColorEntry *clr = clrTab->GetColorEntry(val);
            //std::cout << "Pixel Value: " << val << "[" << clr->c1 << "," << clr->c2 << "," << clr->c3 << "]\n";
            
            if(clr != NULL)
            {
                output[0] = clr->c1;
                output[1] = clr->c2;
                output[2] = clr->c3;
            }
            else
            {
                throw rsgis::img::RSGISImageCalcException("Could not get RGB value from the colour table.");
            }
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            rsgis::img::RSGISImageCalcException(e.what());
        }
    }
		
    RSGISColourImageFromClassRAT::~RSGISColourImageFromClassRAT()
    {
        
    }
	
}}





