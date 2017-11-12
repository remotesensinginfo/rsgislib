/*
 *  RSGISGenAccuracyPoints.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/11/2012.
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

#include "RSGISGenAccuracyPoints.h"

namespace rsgis{namespace classifier{

    
    RSGISGenAccuracyPoints::RSGISGenAccuracyPoints()
    {
        
    }
    
    void RSGISGenAccuracyPoints::generateRandomPoints(GDALDataset *inputImage, GDALDataset *inputDEM, bool demProvided, std::string outputFile, std::string classColName, unsigned int numPts, unsigned int seed) throw(rsgis::RSGISImageException)
    {
        try
        {
            GDALRasterAttributeTable *attTable = inputImage->GetRasterBand(1)->GetDefaultRAT();
            
            int numRows = attTable->GetRowCount();
            int numColumns = attTable->GetColumnCount();
            
            if(numRows == 0)
            {
                throw rsgis::RSGISImageException("The attribute table does not have any rows.");
            }
            
            if(numColumns == 0)
            {
                throw rsgis::RSGISImageException("The attribute table does not have any column.");
            }
            
            // Get the column indexes and create output column if not present.
            int inClassColIdx = 0;
            bool inClassColFound = false;
            
            for(int i = 0; i < numColumns; ++i)
            {
                if(!inClassColFound && (std::string(attTable->GetNameOfCol(i)) == classColName))
                {
                    inClassColFound = true;
                    inClassColIdx = i;
                }
            }
            
            if(!inClassColFound)
            {
                throw rsgis::RSGISImageException("Could not find the input class column.");
            }
            
            double tlX = 0;
            double tlY = 0;
            double xRes = 0;
            double yRes = 0;
            unsigned int imgSizeX = 0;
            unsigned int imgSizeY = 0;
            double demTlX = 0;
            double demTlY = 0;
            double demXRes = 0;
            double demYRes = 0;
            unsigned int demSizeX = 0;
            unsigned int demSizeY = 0;
            
            // Get Transformation for Image
            double *trans = new double[6];
            inputImage->GetGeoTransform(trans);
            tlX = trans[0];
            tlY = trans[3];
            xRes = trans[1];
            yRes = trans[5];
            if(yRes < 0)
            {
                yRes = yRes * (-1);
            }
            imgSizeX = inputImage->GetRasterXSize();
            imgSizeY = inputImage->GetRasterYSize();
            
            if(demProvided)
            {
                // Get Transformation for DEM
                inputDEM->GetGeoTransform(trans);
                demTlX = trans[0];
                demTlY = trans[3];
                demXRes = trans[1];
                demYRes = trans[5];
                if(demYRes < 0)
                {
                    demYRes = demYRes * (-1);
                }
                demSizeX = inputDEM->GetRasterXSize();
                demSizeY = inputDEM->GetRasterYSize();
            }
            delete[] trans;
            
            srand(seed);
            
            unsigned int xPxl = 0;
            unsigned int yPxl = 0;
            double eastings = 0;
            double northings = 0;
            float demVal = 0;
            std::string classVal = "";
            
            RSGISAccPoint *tmpAccPt = NULL;
            std::list<RSGISAccPoint*> *accPts = new std::list<RSGISAccPoint*>();
            
            while(accPts->size() < numPts)
            {
                xPxl = rand() % imgSizeX;
                yPxl = rand() % imgSizeY;
                
                eastings = tlX + (((double)xPxl)*xRes);
                northings = tlY - (((double)yPxl)*yRes);
                
                if(demProvided)
                {
                    try
                    {
                        demVal = findPixelVal(inputDEM, 1, eastings, northings, demTlX, demTlY, demXRes, demYRes, demSizeX, demSizeY);
                    }
                    catch (rsgis::RSGISImageException &e)
                    {
                        demVal = -99999;
                    }
                }
                else
                {
                    demVal = 0;
                }
                
                try
                {
                    classVal = findClassVal(inputImage, 1, attTable, inClassColIdx, xPxl, yPxl);
                    
                    tmpAccPt = new RSGISAccPoint();
                    tmpAccPt->ptID = accPts->size()+1;
                    tmpAccPt->eastings = eastings;
                    tmpAccPt->northings = northings;
                    tmpAccPt->elevation = demVal;
                    tmpAccPt->mapClassName = classVal;
                    tmpAccPt->trueClassName = classVal;
                    tmpAccPt->status = -1;
                    tmpAccPt->comment = "";
                    
                    accPts->push_back(tmpAccPt);
                }
                catch (rsgis::RSGISImageException &e)
                {
                    // Ignore as class not found...
                }
                catch (std::exception &e)
                {
                    throw rsgis::RSGISImageException(e.what());
                }
            }           
            
            std::ofstream outFile;
            
            accPts->sort(compareMapClass);
            
            outFile.open(outputFile.c_str());
            if(!outFile.is_open())
            {
                throw rsgis::RSGISImageException("Could not open the output file.");
            }
            outFile.precision(14);
            
            outFile << "# Generated Random Points.\n";
            outFile << "# Input Image: " << inputImage->GetFileList()[0] << std::endl;
            if(demProvided)
            {
                outFile << "# Input DEM: " << inputDEM->GetFileList()[0] << std::endl;
            }
            else
            {
                outFile << "# Not DEM provided.\n";
            }
            outFile << "# Output File: " << outputFile << std::endl;
            outFile << "# Column Name: " << classColName << std::endl;
            outFile << "# Number of Points: " << numPts << std::endl;
            outFile << "# Seed: " << seed << std::endl;
            
            
            for(std::list<RSGISAccPoint*>::iterator iterPts = accPts->begin(); iterPts != accPts->end(); ++iterPts)
            {
                outFile << (*iterPts)->ptID << "\t" << (*iterPts)->eastings << "\t" << (*iterPts)->northings << "\t" << (*iterPts)->elevation << "\t" << (*iterPts)->mapClassName << "\t" << (*iterPts)->trueClassName << "\t" << (*iterPts)->status << "\t" << (*iterPts)->comment << std::endl;
                delete *iterPts;
            }
            delete accPts;
            
            outFile.flush();
            outFile.close();
            
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    
    void RSGISGenAccuracyPoints::generateStratifiedRandomPoints(GDALDataset *inputImage, GDALDataset *inputDEM, bool demProvided, std::string outputFile, std::string classColName, unsigned int numPts, unsigned int seed) throw(rsgis::RSGISImageException)
    {
        try
        {
            GDALRasterAttributeTable *attTable = inputImage->GetRasterBand(1)->GetDefaultRAT();
            
            int numRows = attTable->GetRowCount();
            int numColumns = attTable->GetColumnCount();
            
            if(numRows == 0)
            {
                throw rsgis::RSGISImageException("The attribute table does not have any rows.");
            }
            
            if(numColumns == 0)
            {
                throw rsgis::RSGISImageException("The attribute table does not have any column.");
            }
            
            // Get the column indexes and create output column if not present.
            int inClassColIdx = 0;
            bool inClassColFound = false;
            
            int histoColIdx = 0;
            bool histoColFound = false;
            
            for(int i = 0; i < numColumns; ++i)
            {
                if(!inClassColFound && (std::string(attTable->GetNameOfCol(i)) == classColName))
                {
                    inClassColFound = true;
                    inClassColIdx = i;
                }
                else if(!histoColFound && (std::string(attTable->GetNameOfCol(i)) == "Histogram"))
                {
                    histoColFound = true;
                    histoColIdx = i;
                }
            }
            
            if(!inClassColFound)
            {
                throw rsgis::RSGISImageException("Could not find the input class column.");
            }
            
            if(!histoColFound)
            {
                throw rsgis::RSGISImageException("Could not find the Histogram column.");
            }
            
            double tlX = 0;
            double tlY = 0;
            double xRes = 0;
            double yRes = 0;
            unsigned int imgSizeX = 0;
            unsigned int imgSizeY = 0;
            double demTlX = 0;
            double demTlY = 0;
            double demXRes = 0;
            double demYRes = 0;
            unsigned int demSizeX = 0;
            unsigned int demSizeY = 0;
            
            // Get Transformation for Image
            double *trans = new double[6];
            inputImage->GetGeoTransform(trans);
            tlX = trans[0];
            tlY = trans[3];
            xRes = trans[1];
            yRes = trans[5];
            if(yRes < 0)
            {
                yRes = yRes * (-1);
            }
            imgSizeX = inputImage->GetRasterXSize();
            imgSizeY = inputImage->GetRasterYSize();
            
            if(demProvided)
            {
                // Get Transformation for DEM
                inputDEM->GetGeoTransform(trans);
                demTlX = trans[0];
                demTlY = trans[3];
                demXRes = trans[1];
                demYRes = trans[5];
                if(demYRes < 0)
                {
                    demYRes = demYRes * (-1);
                }
                demSizeX = inputDEM->GetRasterXSize();
                demSizeY = inputDEM->GetRasterYSize();
            }
            delete[] trans;
            
            std::list<std::string> *classes = this->findUniqueClasses(attTable, inClassColIdx, histoColIdx);
            
            std::vector<std::vector<RSGISAccPoint*> > *accClassPts = new std::vector<std::vector<RSGISAccPoint*> >();
            std::map<std::string, size_t> classesLookUp;
            
            size_t idx = 0;
            for(std::list<std::string>::iterator iterClasses = classes->begin(); iterClasses != classes->end(); ++iterClasses)
            {
                std::cout << "Class: \'" <<  *iterClasses << "\'" << std::endl;
                accClassPts->push_back(std::vector<RSGISAccPoint*>());
                classesLookUp.insert(std::pair<std::string, size_t>(*iterClasses, idx));
                ++idx;
            }
            
            
            srand(seed);
            
            unsigned int xPxl = 0;
            unsigned int yPxl = 0;
            double eastings = 0;
            double northings = 0;
            float demVal = 0;
            std::string classVal = "";
            std::map<std::string, size_t>::iterator iterMap;
            
            RSGISAccPoint *tmpAccPt = NULL;
            
            unsigned long totalNumPtsRequired = classes->size() * numPts;
            unsigned long ptsCount = 0;
            
            
            std::cout << "Number of points to be generated: " << totalNumPtsRequired << std::endl;
            
            while(ptsCount < totalNumPtsRequired)
            {
                std::cout << "Completed " << floor((((double)ptsCount)/((double)totalNumPtsRequired))*100) << " %\r" << std::flush;
                
                xPxl = rand() % imgSizeX;
                yPxl = rand() % imgSizeY;
                                
                eastings = tlX + (((double)xPxl)*xRes);
                northings = tlY - (((double)yPxl)*yRes);
                                
                try
                {
                    classVal = findClassVal(inputImage, 1, attTable, inClassColIdx, xPxl, yPxl);
                                       
                    iterMap = classesLookUp.find(classVal);
                    
                    if(iterMap == classesLookUp.end())
                    {
                        rsgis::RSGISImageException("Class name was not found!");
                    }
                    
                    idx = iterMap->second;
                    
                    if(accClassPts->at(idx).size() < numPts)
                    {
                        if(demProvided)
                        {
                            try
                            {
                                demVal = findPixelVal(inputDEM, 1, eastings, northings, demTlX, demTlY, demXRes, demYRes, demSizeX, demSizeY);
                            }
                            catch (rsgis::RSGISImageException &e)
                            {
                                demVal = -99999;
                            }
                        }
                        else
                        {
                            demVal = 0;
                        }
                        
                        tmpAccPt = new RSGISAccPoint();
                        tmpAccPt->ptID = ptsCount+1;
                        tmpAccPt->eastings = eastings;
                        tmpAccPt->northings = northings;
                        tmpAccPt->elevation = demVal;
                        tmpAccPt->mapClassName = classVal;
                        tmpAccPt->trueClassName = classVal;
                        tmpAccPt->status = -1;
                        tmpAccPt->comment = "";
                        
                        accClassPts->at(idx).push_back(tmpAccPt);
                        
                        ++ptsCount;
                    }                    
                }
                catch (rsgis::RSGISImageException &e)
                {
                    // Ignore as class not found...
                }
                catch (std::exception &e)
                {
                    throw rsgis::RSGISImageException(e.what());
                }
            }
            
            
            std::ofstream outFile;
                       
            outFile.open(outputFile.c_str());
            if(!outFile.is_open())
            {
                throw rsgis::RSGISImageException("Could not open the output file.");
            }
            outFile.precision(14);
            
            outFile << "# Generated Stratified Random Points.\n";
            outFile << "# Input Image: " << inputImage->GetFileList()[0] << std::endl;
            if(demProvided)
            {
                outFile << "# Input DEM: " << inputDEM->GetFileList()[0] << std::endl;
            }
            else
            {
                outFile << "# Not DEM provided.\n";
            }
            outFile << "# Output File: " << outputFile << std::endl;
            outFile << "# Column Name: " << classColName << std::endl;
            outFile << "# Number of Points Per Class: " << numPts << std::endl;
            outFile << "# Seed: " << seed << std::endl;
            
            for(std::vector<std::vector<RSGISAccPoint*> >::iterator iterPtsVecs = accClassPts->begin(); iterPtsVecs != accClassPts->end(); ++iterPtsVecs)
            {
                for(std::vector<RSGISAccPoint*>::iterator iterPts = (*iterPtsVecs).begin(); iterPts != (*iterPtsVecs).end(); ++iterPts)
                {
                    outFile << (*iterPts)->ptID << "\t" << (*iterPts)->eastings << "\t" << (*iterPts)->northings << "\t" << (*iterPts)->elevation << "\t" << (*iterPts)->mapClassName << "\t" << (*iterPts)->trueClassName << "\t" << (*iterPts)->status << "\t" << (*iterPts)->comment << std::endl;
                    delete *iterPts;
                }
            }
            
            outFile.flush();
            outFile.close();
             
            
            delete classes;
            delete accClassPts;
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    
    void RSGISGenAccuracyPoints::generateRandomPointsVecOut(GDALDataset *inputImage, OGRLayer *outputSHPLayer, std::string imgClassCol, std::string vecClassImgCol, std::string vecClassRefCol, unsigned int numPts, unsigned int seed) throw(rsgis::RSGISImageException)
    {
        try
        {
            // Get attribute table...
            GDALRasterAttributeTable *attTable = inputImage->GetRasterBand(1)->GetDefaultRAT();
            
            if(attTable == NULL)
            {
                throw RSGISImageException("The image dataset does not have an attribute table.");
            }
            
            rsgis::rastergis::RSGISRasterAttUtils ratUtils;
            std::vector<std::string> *imgClassColVals = ratUtils.readStrColumnAsVec(attTable, imgClassCol);
            
            double tlX = 0;
            double tlY = 0;
            double xRes = 0;
            double yRes = 0;
            unsigned int imgSizeX = 0;
            unsigned int imgSizeY = 0;
            
            // Get Transformation for Image
            double *trans = new double[6];
            inputImage->GetGeoTransform(trans);
            tlX = trans[0];
            tlY = trans[3];
            xRes = trans[1];
            yRes = trans[5];
            if(yRes < 0)
            {
                yRes = yRes * (-1);
            }
            imgSizeX = inputImage->GetRasterXSize();
            imgSizeY = inputImage->GetRasterYSize();
            
            delete[] trans;
            
            srand(seed);
            
            unsigned int xPxl = 0;
            unsigned int yPxl = 0;
            double eastings = 0;
            double northings = 0;
            std::string classVal = "";
            long pxlVal = 0.0;
            
            RSGISAccPoint *tmpAccPt = NULL;
            std::list<RSGISAccPoint*> *accPts = new std::list<RSGISAccPoint*>();
            
            while(accPts->size() < numPts)
            {
                xPxl = rand() % imgSizeX;
                yPxl = rand() % imgSizeY;
                
                eastings = tlX + (((double)xPxl)*xRes);
                northings = tlY - (((double)yPxl)*yRes);
                
                
                try
                {
                    pxlVal = long(this->findPixelVal(inputImage, 1, eastings, northings, tlX, tlY, xRes, yRes, imgSizeX, imgSizeY));
                    
                    if((pxlVal > 0) & (pxlVal < imgClassColVals->size()))
                    {
                        classVal = imgClassColVals->at(pxlVal);
                        
                        tmpAccPt = new RSGISAccPoint();
                        tmpAccPt->ptID = accPts->size()+1;
                        tmpAccPt->eastings = eastings;
                        tmpAccPt->northings = northings;
                        tmpAccPt->elevation = 0.0;
                        tmpAccPt->mapClassName = classVal;
                        tmpAccPt->trueClassName = "";
                        tmpAccPt->status = -1;
                        tmpAccPt->comment = "";
                        
                        accPts->push_back(tmpAccPt);
                    }
                }
                catch (rsgis::RSGISImageException &e)
                {
                    // Ignore as class not found...
                }
                catch (std::exception &e)
                {
                    throw rsgis::RSGISImageException(e.what());
                }
            }
            
            std::ofstream outFile;
            
            accPts->sort(compareMapClass);
            
            OGRFieldDefn imgClassField(vecClassImgCol.c_str(), OFTString);
            imgClassField.SetWidth(254);
            if( outputSHPLayer->CreateField( &imgClassField ) != OGRERR_NONE )
            {
                std::string message = std::string("Creating shapefile field \'") + vecClassImgCol + std::string("\' has failed");
                throw rsgis::RSGISException(message);
            }
            
            OGRFieldDefn refClassField(vecClassRefCol.c_str(), OFTString);
            refClassField.SetWidth(254);
            if( outputSHPLayer->CreateField( &refClassField ) != OGRERR_NONE )
            {
                std::string message = std::string("Creating shapefile field \'") + vecClassRefCol + std::string("\' has failed");
                throw rsgis::RSGISException(message);
            }
            
            OGRFeatureDefn *featDefn = outputSHPLayer->GetLayerDefn();
            int imgClassColIdx = featDefn->GetFieldIndex(vecClassImgCol.c_str());
            int refClassColIdx = featDefn->GetFieldIndex(vecClassRefCol.c_str());
            
            for(std::list<RSGISAccPoint*>::iterator iterPts = accPts->begin(); iterPts != accPts->end(); ++iterPts)
            {
                OGRFeature *poFeature = new OGRFeature(featDefn);
                OGRPoint *pt = new OGRPoint((*iterPts)->eastings, (*iterPts)->northings, 0.0);
                poFeature->SetGeometryDirectly(pt);
                
                poFeature->SetField(imgClassColIdx, (*iterPts)->mapClassName.c_str());
                poFeature->SetField(refClassColIdx, (*iterPts)->trueClassName.c_str());
                
                outputSHPLayer->CreateFeature(poFeature);
            }
            
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    
    void RSGISGenAccuracyPoints::generateStratifiedRandomPointsVecOut(GDALDataset *inputImage, OGRLayer *outputSHPLayer, std::string imgClassCol, std::string vecClassImgCol, std::string vecClassRefCol, unsigned int numPts, unsigned int seed) throw(rsgis::RSGISImageException)
    {
        try
        {
            // Get attribute table...
            GDALRasterAttributeTable *attTable = inputImage->GetRasterBand(1)->GetDefaultRAT();
            
            if(attTable == NULL)
            {
                throw RSGISImageException("The image dataset does not have an attribute table.");
            }
            
            rsgis::rastergis::RSGISRasterAttUtils ratUtils;
            std::vector<std::string> *imgClassColVals = ratUtils.readStrColumnAsVec(attTable, imgClassCol);
            std::vector<int> *histogram = ratUtils.readIntColumnAsVec(attTable, "Histogram");
            
            std::list<std::string> *classNames = new std::list<std::string>();
            bool foundClassName = false;
            for(size_t i = 0; i < histogram->size(); ++i)
            {
                imgClassColVals->at(i) = boost::trim_all_copy(imgClassColVals->at(i));
                if((histogram->at(i) > 0) & (imgClassColVals->at(i) != ""))
                {
                    if(classNames->empty())
                    {
                        classNames->push_back(imgClassColVals->at(i));
                    }
                    else
                    {
                        foundClassName = false;
                        for(std::list<std::string>::iterator iterClass = classNames->begin(); iterClass != classNames->end(); ++iterClass)
                        {
                            if(*iterClass == imgClassColVals->at(i))
                            {
                                foundClassName = true;
                                break;
                            }
                        }
                        
                        if(!foundClassName)
                        {
                            classNames->push_back(imgClassColVals->at(i));
                        }
                    }
                }
            }
            classNames->sort(rsgis::utils::compareStringNoCase);
            
            std::vector<std::vector<RSGISAccPoint*> > *accClassPts = new std::vector<std::vector<RSGISAccPoint*> >();
            std::map<std::string, size_t> classesLookUp;
            
            size_t idx = 0;
            for(std::list<std::string>::iterator iterClasses = classNames->begin(); iterClasses != classNames->end(); ++iterClasses)
            {
                std::cout << "Class: \'" <<  *iterClasses << "\'" << std::endl;
                accClassPts->push_back(std::vector<RSGISAccPoint*>());
                classesLookUp.insert(std::pair<std::string, size_t>(*iterClasses, idx));
                ++idx;
            }
            
            double tlX = 0;
            double tlY = 0;
            double xRes = 0;
            double yRes = 0;
            unsigned int imgSizeX = 0;
            unsigned int imgSizeY = 0;
            
            // Get Transformation for Image
            double *trans = new double[6];
            inputImage->GetGeoTransform(trans);
            tlX = trans[0];
            tlY = trans[3];
            xRes = trans[1];
            yRes = trans[5];
            if(yRes < 0)
            {
                yRes = yRes * (-1);
            }
            imgSizeX = inputImage->GetRasterXSize();
            imgSizeY = inputImage->GetRasterYSize();
            
            delete[] trans;
            
            srand(seed);
            
            unsigned int xPxl = 0;
            unsigned int yPxl = 0;
            double eastings = 0;
            double northings = 0;
            std::string classVal = "";
            long pxlVal = 0.0;
            
            RSGISAccPoint *tmpAccPt = NULL;
            std::map<std::string, size_t>::iterator iterMap;
            
            unsigned long totalNumPtsRequired = classNames->size() * numPts;
            unsigned long ptsCount = 0;
            
            std::cout << "Number of points to be generated: " << totalNumPtsRequired << std::endl;
            
            while(ptsCount < totalNumPtsRequired)
            {
                std::cout << "Completed " << floor((((double)ptsCount)/((double)totalNumPtsRequired))*100) << " %\r" << std::flush;
                
                xPxl = rand() % imgSizeX;
                yPxl = rand() % imgSizeY;
                
                eastings = tlX + (((double)xPxl)*xRes);
                northings = tlY - (((double)yPxl)*yRes);
                
                try
                {
                    pxlVal = long(this->findPixelVal(inputImage, 1, eastings, northings, tlX, tlY, xRes, yRes, imgSizeX, imgSizeY));
                    
                    if((pxlVal > 0) & (pxlVal < imgClassColVals->size()))
                    {
                        classVal = imgClassColVals->at(pxlVal);
                        
                        iterMap = classesLookUp.find(classVal);
                        
                        if(iterMap == classesLookUp.end())
                        {
                            rsgis::RSGISImageException("Class name was not found!");
                        }
                        
                        idx = iterMap->second;
                        
                        if(accClassPts->at(idx).size() < numPts)
                        {
                            tmpAccPt = new RSGISAccPoint();
                            tmpAccPt->ptID = ptsCount+1;
                            tmpAccPt->eastings = eastings;
                            tmpAccPt->northings = northings;
                            tmpAccPt->elevation = 0.0;
                            tmpAccPt->mapClassName = classVal;
                            tmpAccPt->trueClassName = "";
                            tmpAccPt->status = -1;
                            tmpAccPt->comment = "";
                        
                            accClassPts->at(idx).push_back(tmpAccPt);
                            
                            ++ptsCount;
                        }
                    }
                }
                catch (rsgis::RSGISImageException &e)
                {
                    // Ignore as class not found...
                }
                catch (std::exception &e)
                {
                    throw rsgis::RSGISImageException(e.what());
                }
            }
            
            OGRFieldDefn imgClassField(vecClassImgCol.c_str(), OFTString);
            imgClassField.SetWidth(254);
            if( outputSHPLayer->CreateField( &imgClassField ) != OGRERR_NONE )
            {
                std::string message = std::string("Creating shapefile field \'") + vecClassImgCol + std::string("\' has failed");
                throw rsgis::RSGISException(message);
            }
            
            OGRFieldDefn refClassField(vecClassRefCol.c_str(), OFTString);
            refClassField.SetWidth(254);
            if( outputSHPLayer->CreateField( &refClassField ) != OGRERR_NONE )
            {
                std::string message = std::string("Creating shapefile field \'") + vecClassRefCol + std::string("\' has failed");
                throw rsgis::RSGISException(message);
            }
            
            OGRFeatureDefn *featDefn = outputSHPLayer->GetLayerDefn();
            int imgClassColIdx = featDefn->GetFieldIndex(vecClassImgCol.c_str());
            int refClassColIdx = featDefn->GetFieldIndex(vecClassRefCol.c_str());
            
            for(std::vector<std::vector<RSGISAccPoint*> >::iterator iterPtsVecs = accClassPts->begin(); iterPtsVecs != accClassPts->end(); ++iterPtsVecs)
            {
                for(std::vector<RSGISAccPoint*>::iterator iterPts = (*iterPtsVecs).begin(); iterPts != (*iterPtsVecs).end(); ++iterPts)
                {
                    OGRFeature *poFeature = new OGRFeature(featDefn);
                    OGRPoint *pt = new OGRPoint((*iterPts)->eastings, (*iterPts)->northings, 0.0);
                    poFeature->SetGeometryDirectly(pt);
                    
                    poFeature->SetField(imgClassColIdx, (*iterPts)->mapClassName.c_str());
                    poFeature->SetField(refClassColIdx, (*iterPts)->trueClassName.c_str());
                    
                    outputSHPLayer->CreateFeature(poFeature);
                }
            }
            
            delete classNames;
            delete accClassPts;
            
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    
    void RSGISGenAccuracyPoints::generateStratifiedRandomPointsVecOutUsePxlLst(GDALDataset *inputImage, OGRLayer *outputSHPLayer, std::string imgClassCol, std::string vecClassImgCol, std::string vecClassRefCol, unsigned int numPts, unsigned int seed) throw(rsgis::RSGISImageException)
    {
        try
        {
            // Get attribute table...
            GDALRasterAttributeTable *attTable = inputImage->GetRasterBand(1)->GetDefaultRAT();
            
            if(attTable == NULL)
            {
                throw RSGISImageException("The image dataset does not have an attribute table.");
            }
            
            rsgis::rastergis::RSGISRasterAttUtils ratUtils;
            std::vector<std::string> *imgClassColVals = ratUtils.readStrColumnAsVec(attTable, imgClassCol);
            std::vector<int> *histogram = ratUtils.readIntColumnAsVec(attTable, "Histogram");
            
            std::vector<std::string> *classNames = new std::vector<std::string>();
            bool foundClassName = false;
            for(size_t i = 0; i < histogram->size(); ++i)
            {
                imgClassColVals->at(i) = boost::trim_all_copy(imgClassColVals->at(i));
                if((histogram->at(i) > 0) & (imgClassColVals->at(i) != ""))
                {
                    if(classNames->empty())
                    {
                        classNames->push_back(imgClassColVals->at(i));
                    }
                    else
                    {
                        foundClassName = false;
                        for(std::vector<std::string>::iterator iterClass = classNames->begin(); iterClass != classNames->end(); ++iterClass)
                        {
                            if(*iterClass == imgClassColVals->at(i))
                            {
                                foundClassName = true;
                                break;
                            }
                        }
                        
                        if(!foundClassName)
                        {
                            classNames->push_back(imgClassColVals->at(i));
                        }
                    }
                }
            }
            
            unsigned long numClasses = classNames->size();
            std::vector<std::pair<double, double> > **classPxlLst = new std::vector<std::pair<double, double> >*[numClasses];
            unsigned int idx = 0;
            for(std::vector<std::string>::iterator iterClasses = classNames->begin(); iterClasses != classNames->end(); ++iterClasses)
            {
                std::cout << "Class: \'" <<  *iterClasses << "\'" << std::endl;
                classPxlLst[idx] = new std::vector<std::pair<double, double> >();
                ++idx;
            }
            
            rsgis::img::RSGISCalcImageValue *exClassPxlLocs = new RSGISExtractClassPxllocs(classPxlLst, numClasses);
            rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(exClassPxlLocs);
            calcImg.calcImageExtent(&inputImage, 1, 0);
            delete exClassPxlLocs;
            
            bool **usedPxl = new bool*[numClasses];
            for(unsigned long i = 0; i < numClasses; ++i)
            {
                usedPxl[i] = new bool[classPxlLst[i]->size()];
                for(unsigned long j = 0; j < classPxlLst[i]->size(); ++j)
                {
                    usedPxl[i][j] = false;
                }
            }
            
            OGRFieldDefn imgClassField(vecClassImgCol.c_str(), OFTString);
            imgClassField.SetWidth(254);
            if( outputSHPLayer->CreateField( &imgClassField ) != OGRERR_NONE )
            {
                std::string message = std::string("Creating shapefile field \'") + vecClassImgCol + std::string("\' has failed");
                throw rsgis::RSGISException(message);
            }
            
            OGRFieldDefn refClassField(vecClassRefCol.c_str(), OFTString);
            refClassField.SetWidth(254);
            if( outputSHPLayer->CreateField( &refClassField ) != OGRERR_NONE )
            {
                std::string message = std::string("Creating shapefile field \'") + vecClassRefCol + std::string("\' has failed");
                throw rsgis::RSGISException(message);
            }
            
            OGRFeatureDefn *featDefn = outputSHPLayer->GetLayerDefn();
            int imgClassColIdx = featDefn->GetFieldIndex(vecClassImgCol.c_str());
            int refClassColIdx = featDefn->GetFieldIndex(vecClassRefCol.c_str());
            unsigned long pxlIdx = 0;
            bool foundPxl = false;
            unsigned long findPxlIterCount = 0;
            unsigned long checkPt = 0;
            bool usedAllPxls = false;
            for(unsigned long i = 0; i < numClasses; ++i)
            {
                std::cout << "Processing Class \"" << classNames->at(i) << "\"\n";
                srand(seed);
                checkPt = classPxlLst[i]->size();
                for(unsigned long j = 0; j < numPts; ++j)
                {
                    foundPxl = false;
                    while(!foundPxl)
                    {
                        pxlIdx = rand() % classPxlLst[i]->size();
                        if(!usedPxl[i][pxlIdx])
                        {
                            foundPxl = true;
                            usedPxl[i][pxlIdx] = true;
                        }
                        else if(findPxlIterCount > checkPt)
                        {
                            usedAllPxls = true;
                            for(unsigned long n = 0; n < classPxlLst[i]->size(); ++n)
                            {
                                if(!usedPxl[i][n])
                                {
                                    usedAllPxls = false;
                                    break;
                                }
                            }
                            
                            if(usedAllPxls)
                            {
                                rsgis::utils::RSGISTextUtils txtUtils;
                                throw rsgis::RSGISImageException("All pixels (n="+txtUtils.sizettostring(classPxlLst[i]->size())+") for class \""+classNames->at(i)+"\" have been sampled within the image");
                            }
                            checkPt = checkPt + classPxlLst[i]->size();
                        }
                        
                        ++findPxlIterCount;
                    }
                    
                    OGRFeature *poFeature = new OGRFeature(featDefn);
                    OGRPoint *pt = new OGRPoint(classPxlLst[i]->at(pxlIdx).first, classPxlLst[i]->at(pxlIdx).second, 0.0);
                    poFeature->SetGeometryDirectly(pt);
                    
                    poFeature->SetField(imgClassColIdx, classNames->at(i).c_str());
                    poFeature->SetField(refClassColIdx, classNames->at(i).c_str());
                    
                    outputSHPLayer->CreateFeature(poFeature);
                }
            }
            
            delete classNames;
            for(idx = 0; idx < numClasses; ++idx)
            {
                delete classPxlLst[idx];
            }
            delete[] classPxlLst;
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    
    void RSGISGenAccuracyPoints::popClassInfo2Vec(GDALDataset *inputImage, OGRLayer *inputVecLayer, std::string imgClassCol, std::string vecClassImgCol, std::string vecClassRefCol, bool addRefCol)throw(rsgis::RSGISImageException)
    {
        try
        {
            // Get attribute table...
            GDALRasterAttributeTable *attTable = inputImage->GetRasterBand(1)->GetDefaultRAT();
            
            if(attTable == NULL)
            {
                throw RSGISImageException("The image dataset does not have an attribute table.");
            }
            
            rsgis::rastergis::RSGISRasterAttUtils ratUtils;
            std::vector<std::string> *imgClassColVals = ratUtils.readStrColumnAsVec(attTable, imgClassCol);
            
            double tlX = 0;
            double tlY = 0;
            double xRes = 0;
            double yRes = 0;
            unsigned int imgSizeX = 0;
            unsigned int imgSizeY = 0;
            
            // Get Transformation for Image
            double *trans = new double[6];
            inputImage->GetGeoTransform(trans);
            tlX = trans[0];
            tlY = trans[3];
            xRes = trans[1];
            yRes = trans[5];
            if(yRes < 0)
            {
                yRes = yRes * (-1);
            }
            imgSizeX = inputImage->GetRasterXSize();
            imgSizeY = inputImage->GetRasterYSize();
            
            delete[] trans;
            
            double eastings = 0.0;
            double northings = 0.0;
            
            OGRFieldDefn imgClassField(vecClassImgCol.c_str(), OFTString);
            imgClassField.SetWidth(254);
            if( inputVecLayer->CreateField( &imgClassField, false ) != OGRERR_NONE )
            {
                std::string message = std::string("Creating shapefile field \'") + vecClassImgCol + std::string("\' has failed");
                throw rsgis::RSGISException(message);
            }
            std::string vecClassImgField = std::string(imgClassField.GetNameRef());
            
            std::string vecClassRefField = "";
            if(addRefCol)
            {
                OGRFieldDefn refClassField(vecClassRefCol.c_str(), OFTString);
                refClassField.SetWidth(254);
                if( inputVecLayer->CreateField( &refClassField, false ) != OGRERR_NONE )
                {
                    std::string message = std::string("Creating shapefile field \'") + vecClassRefCol + std::string("\' has failed");
                    throw rsgis::RSGISException(message);
                }
                vecClassRefField = std::string(refClassField.GetNameRef());
            }
            
            
            
            OGRFeatureDefn *featDefn = inputVecLayer->GetLayerDefn();
            int imgClassColIdx = featDefn->GetFieldIndex(vecClassImgField.c_str());
            int refClassColIdx = -1;
            if(addRefCol)
            {
                refClassColIdx = featDefn->GetFieldIndex(vecClassRefField.c_str());
            }
            
            int numFeatures = inputVecLayer->GetFeatureCount(TRUE);
            
            bool nullGeometry = false;
            OGREnvelope *ogrEnv = NULL;
            OGRGeometry *geometry = NULL;
            std::string classVal = "";
            std::string emptyStr = "";
            long pxlVal = 0.0;
            
            int feedback = numFeatures/10;
            int feedbackCounter = 0;
            int i = 0;
            
            OGRFeature *featObj = NULL;
            inputVecLayer->ResetReading();
            std::cout << "Started. " << std::flush;
            while( (featObj = inputVecLayer->GetNextFeature()) != NULL )
            {
                if((numFeatures > 10) && ((i % feedback) == 0) && feedbackCounter <= 100)
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    
                    feedbackCounter = feedbackCounter + 10;
                }
                
                // Get Geometry.
                nullGeometry = false;
                geometry = featObj->GetGeometryRef();
                if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPoint )
                {
                    OGRPoint *pt = (OGRPoint *) geometry;
                    eastings = pt->getX();
                    northings = pt->getY();
                }
                else if(geometry != NULL)
                {
                    ogrEnv = new OGREnvelope();
                    geometry->getEnvelope(ogrEnv);
                    eastings = ogrEnv->MinX + ((ogrEnv->MaxX - ogrEnv->MinX)/2);
                    northings = ogrEnv->MinY + ((ogrEnv->MaxY - ogrEnv->MinY)/2);
                }
                else
                {
                    nullGeometry = true;
                    std::cout << "WARNING: NULL Geometry Present within input file - IGNORED\n";
                }
                
                if(!nullGeometry)
                {
                    pxlVal = long(this->findPixelVal(inputImage, 1, eastings, northings, tlX, tlY, xRes, yRes, imgSizeX, imgSizeY));
                    if((pxlVal > 0) & (pxlVal < imgClassColVals->size()))
                    {
                        classVal = imgClassColVals->at(pxlVal);
                        if(classVal == "")
                        {
                            classVal = "NA";
                        }

                        featObj->SetField(imgClassColIdx, classVal.c_str());
                        if(addRefCol)
                        {
                            featObj->SetField(refClassColIdx, emptyStr.c_str());
                        }
                        
                    }
                    else
                    {
                        classVal = "NA";
                        featObj->SetField(imgClassColIdx, classVal.c_str());
                        if(addRefCol)
                        {
                            featObj->SetField(refClassColIdx, emptyStr.c_str());
                        }
                    }
                    
                    if( inputVecLayer->SetFeature(featObj) != OGRERR_NONE )
                    {
                        throw rsgis::RSGISException("Failed to write feature to the shapefile.");
                    }
                }
                
                OGRFeature::DestroyFeature(featObj);
                i++;
            }
            std::cout << ". Complete.\n";
            
        
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    
    float RSGISGenAccuracyPoints::findPixelVal(GDALDataset *image, unsigned int band, double eastings, double northings, double tlX, double tlY, double xRes, double yRes, unsigned int xSize, unsigned int ySize) throw(rsgis::RSGISImageException)
    {
        
        double diffX = eastings - tlX;
        double diffY = tlY - northings;
        
        if(diffX < 0)
        {
            throw rsgis::RSGISImageException("Not found within the scene (xMin).");
        }
        
        if(diffY < 0)
        {
            throw rsgis::RSGISImageException("Not found within the scene (yMax).");
        }
        
        unsigned int xPxl = diffX/xRes;
        unsigned int yPxl = diffY/yRes;
        
        if(xPxl >= xSize)
        {
            throw rsgis::RSGISImageException("Not found within the scene (xMax).");
        }
        
        if(yPxl >= ySize)
        {
            throw rsgis::RSGISImageException("Not found within the scene (yMin).");
        }
        
        if((image->GetRasterCount() < band) | (band == 0))
        {
            throw rsgis::RSGISImageException("Band not within image.");
        }
        
        float val = 0.0;
        
        image->GetRasterBand(band)->RasterIO(GF_Read, xPxl, yPxl, 1, 1, &val, 1, 1, GDT_Float32, 0, 0);
        
        
        return val;
    }
    
    std::string RSGISGenAccuracyPoints::findClassVal(GDALDataset *image, unsigned int band, GDALRasterAttributeTable *attTable, unsigned int classNameColIdx, unsigned int xPxl, unsigned int yPxl) throw(rsgis::RSGISImageException)
    {
        if((image->GetRasterCount() < band) | (band == 0))
        {
            throw rsgis::RSGISImageException("Band not within image.");
        }
        
        if(xPxl > image->GetRasterXSize())
        {
            throw rsgis::RSGISImageException("Not found within the scene (xMax).");
        }
        
        if(yPxl > image->GetRasterYSize())
        {
            throw rsgis::RSGISImageException("Not found within the scene (yMin).");
        }
        
        int val = 0;
        
        image->GetRasterBand(band)->RasterIO(GF_Read, xPxl, yPxl, 1, 1, &val, 1, 1, GDT_Int32, 0, 0);
        
        if((val == 0) | (val > attTable->GetRowCount()))
        {
            throw rsgis::RSGISImageException("Row is not available.");
        }
        
        std::string className = boost::trim_all_copy(std::string(attTable->GetValueAsString(val, classNameColIdx)));
        
        return className;
    }
    
    std::list<std::string>* RSGISGenAccuracyPoints::findUniqueClasses(GDALRasterAttributeTable *attTable, unsigned int classNameColIdx, int histoColIdx) throw(rsgis::RSGISImageException)
    {
        std::list<std::string> *classes = new std::list<std::string>();
        
        std::string className = "";
        bool foundClassName = false;
        long histVal = 0;
        for(unsigned long i = 1; i < attTable->GetRowCount(); ++i)
        {
            histVal = attTable->GetValueAsInt(i, histoColIdx);
            if(histVal > 0)
            {
                className = boost::trim_all_copy(std::string(attTable->GetValueAsString(i, classNameColIdx)));
                foundClassName = false;
                
                if(className != "")
                {
                    if(classes->empty())
                    {
                        classes->push_back(className);
                    }
                    else
                    {
                        for(std::list<std::string>::iterator iterClasses = classes->begin(); iterClasses != classes->end(); ++iterClasses)
                        {
                            if(*iterClasses == className)
                            {
                                foundClassName = true;
                                break;
                            }
                        }
                        
                        if(!foundClassName)
                        {
                            classes->push_back(className);
                        }
                    }
                }
            }
        }
        
        classes->sort(rsgis::utils::compareStringNoCase);
        
        return classes;
    }
    
    RSGISGenAccuracyPoints::~RSGISGenAccuracyPoints()
    {
        
    }
    
    
    
    RSGISExtractClassPxllocs::RSGISExtractClassPxllocs(std::vector<std::pair<double, double> > **classPxlLst, unsigned long numClasses):rsgis::img::RSGISCalcImageValue(0)
    {
        this->classPxlLst = classPxlLst;
        this->numClasses = numClasses;
    }
    
    void RSGISExtractClassPxllocs::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException)
    {
        if((numIntVals != 1) & (numfloatVals != 0))
        {
            throw rsgis::img::RSGISImageCalcException("Only expecting a single integer input image band.");
        }
        
        if(intBandValues[0] > 0)
        {
            if(intBandValues[0] <= this->numClasses)
            {
                double x = extent.getMinX() + (extent.getWidth()/2);
                double y = extent.getMinY() + (extent.getHeight()/2);
                this->classPxlLst[intBandValues[0]-1]->push_back(std::pair<double, double>(x, y));
            }
        }
    }

    RSGISExtractClassPxllocs::~RSGISExtractClassPxllocs()
    {
        
    }
    
}}






