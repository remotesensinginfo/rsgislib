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
    
    void RSGISCollapseSegmentsClassification::collapseClassification(GDALDataset *segments, std::string classNameCol, std::string classIntCol, bool useIntCol, std::string outputImage, std::string imageFormat) throw(RSGISClassificationException)
    {
        try
        {
            // Get attribute table...
            GDALRasterAttributeTable *inRAT = segments->GetRasterBand(1)->GetDefaultRAT();
            
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
            unsigned int classIntColIdx = 0;
            
            // Find Unique column names alone with colour
            std::cout << "Find the class names...\n";
            //std::map<std::string, RSGISClassInfo*> *classes = this->findAllClassNames(inRAT, classNameIdx, redIdx, greenIdx, blueIdx, alphaIdx);
            
            std::map<std::string, RSGISClassInfo*> *classes = new std::map<std::string, RSGISClassInfo*>();
            
            RSGISFindAllClassNames findClassNames = RSGISFindAllClassNames(classes, useIntCol);
            rsgis::rastergis::RSGISRATCalc ratCalc = rsgis::rastergis::RSGISRATCalc(&findClassNames);
            std::vector<unsigned int> inRealColIdx;
            std::vector<unsigned int> inIntColIdx;
            inIntColIdx.push_back(redIdx);
            inIntColIdx.push_back(greenIdx);
            inIntColIdx.push_back(blueIdx);
            inIntColIdx.push_back(alphaIdx);
            if(useIntCol)
            {
                classIntColIdx = ratUtils.findColumnIndex(inRAT, classIntCol);
                inIntColIdx.push_back(classIntColIdx);
            }
            std::vector<unsigned int> inStrColIdx;
            inStrColIdx.push_back(classNameIdx);
            std::vector<unsigned int> outRealColIdx;
            std::vector<unsigned int> outIntColIdx;
            std::vector<unsigned int> outStrColIdx;
            ratCalc.calcRATValues(inRAT, inRealColIdx, inIntColIdx, inStrColIdx, outRealColIdx, outIntColIdx, outStrColIdx);
            
            
            
            // Assign ID to each class
            bool first = true;
            size_t id = 1;
            for(std::map<std::string, RSGISClassInfo*>::iterator iterClasses = classes->begin(); iterClasses != classes->end(); ++iterClasses)
            {
                /*
                if(iterClasses->first == "")
                {
                    iterClasses->second->classID = 0;
                }
                else
                {
                    iterClasses->second->classID = id++;
                }
                */
                if(first)
                {
                    id = iterClasses->second->classID;
                    first = false;
                }
                else if (iterClasses->second->classID > id)
                {
                    id = iterClasses->second->classID;
                }
                std::cout << iterClasses->second->classID << ":\t " << iterClasses->second->classname << ": [" << iterClasses->second->red << "," << iterClasses->second->green << "," << iterClasses->second->blue << "]\n";
            }

            // Create the new RAT.
            GDALRasterAttributeTable *outRAT = new GDALDefaultRasterAttributeTable();
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
            
            size_t classNameColLen = 0;
            char **classColVals = ratUtils.readStrColumn(inRAT, classNameCol, &classNameColLen);
            
            // Create new image with new RAT and pixel IDs...
            RSGISRecodeRasterFromClasses *recodeRaster = new RSGISRecodeRasterFromClasses(inRAT, classColVals, classNameColLen, classes);
            rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(recodeRaster, "", true);
            calcImg.calcImage(&segments, 1, 0, outputImage, false, NULL, imageFormat, GDT_Int32);
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
    
    RSGISCollapseSegmentsClassification::~RSGISCollapseSegmentsClassification()
    {
        
    }
    
    
    RSGISFindAllClassNames::RSGISFindAllClassNames(std::map<std::string, RSGISClassInfo*> *classes, bool useIntCol)
    {
        this->classes = classes;
        idVal = 1;
        this->useIntCol = useIntCol;
    }
    
    void RSGISFindAllClassNames::calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException)
    {
        try
        {
            if(numInStringCols == 1)
            {
                if((classes->empty()) || (classes->count(inStringCols[0]) == 0))
                {
                    if((!useIntCol) & (numInIntCols != 4))
                    {
                        throw RSGISAttributeTableException("4 columns for RGBA need to be provided.");
                    }
                    else if((useIntCol) & (numInIntCols != 5))
                    {
                        throw RSGISAttributeTableException("5 columns for RGBA + classInt need to be provided.");
                    }
                    else
                    {
                        RSGISClassInfo *classInfo = new RSGISClassInfo();
                        classInfo->classname = inStringCols[0];
                        classInfo->red = inIntCols[0];
                        classInfo->green = inIntCols[1];
                        classInfo->blue = inIntCols[2];
                        classInfo->alpha = inIntCols[3];
                        if(useIntCol)
                        {
                            classInfo->classID = inIntCols[4];
                        }
                        else
                        {
                            classInfo->classID = idVal++;
                        }
                        classes->insert(std::pair<std::string, RSGISClassInfo*>(inStringCols[0], classInfo));
                    }
                }
            }
            else
            {
                throw RSGISAttributeTableException("A single string column needs to be provided for the classes column.");
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
    }
    
    RSGISFindAllClassNames::~RSGISFindAllClassNames()
    {
        
    }
    
    
    
    
    RSGISRecodeRasterFromClasses::RSGISRecodeRasterFromClasses(const GDALRasterAttributeTable *rat, char **classColVals, size_t classNameColLen, std::map<std::string, RSGISClassInfo*> *classes) : rsgis::img::RSGISCalcImageValue(1)
    {
        this->rat = rat;
        this->classColVals = classColVals;
        this->classNameColLen = classNameColLen;
        this->classes = classes;
        
        // Load string column to memory
    }
    
    void RSGISRecodeRasterFromClasses::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            if(intBandValues[0] >= classNameColLen)
            {
                throw rsgis::img::RSGISImageCalcException("Row is not within the RAT.");
            }
            
            if(intBandValues[0] == 0)
            {
                output[0] = 0;
            }
            else
            {
                std::string className = std::string(classColVals[intBandValues[0]]);
                
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
    
    void RSGISColourImageFromClassRAT::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            int val = boost::lexical_cast<int>(bandValues[0]);
            
            const GDALColorEntry *clr = clrTab->GetColorEntry(val);
            
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





