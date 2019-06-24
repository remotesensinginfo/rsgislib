/*
 *  RSGISFindClumpNeighbours.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/03/2012.
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

#include "RSGISFindClumpNeighbours.h"

namespace rsgis{namespace rastergis{
  
    RSGISFindClumpNeighbours::RSGISFindClumpNeighbours()
    {
        
    }
    
    std::vector<std::list<size_t>* >* RSGISFindClumpNeighbours::findNeighbours(GDALDataset *clumpImage, unsigned int ratBand) 
    {
        std::vector<std::list<size_t>* > *neighbours = new std::vector<std::list<size_t>* >();
        try
        {
            
            unsigned int width = clumpImage->GetRasterXSize();
            unsigned int height = clumpImage->GetRasterYSize();
            GDALRasterBand *imgBand = clumpImage->GetRasterBand(ratBand);
            
            unsigned int *clumpIdxs = new unsigned int[width];
            unsigned long maxClumpIdx = 0;
            
            int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
            
            for(unsigned int i = 0; i < height; ++i)
            {
                if((i % feedback) == 0)
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
                imgBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
                for(unsigned int j = 0; j < width; ++j)
                {
                    if((i == 0) & (j == 0))
                    {
                        maxClumpIdx = clumpIdxs[j];
                    }
                    else if(clumpIdxs[j] > maxClumpIdx)
                    {
                        maxClumpIdx = clumpIdxs[j];
                    }
                }
            }
            std::cout << " Complete.\n";
            delete[] clumpIdxs;
            
            std::cout << "Number of clumps = " << maxClumpIdx << std::endl;
            
            neighbours->reserve(maxClumpIdx);
            
            for(unsigned long i = 0; i < maxClumpIdx; ++i)
            {
                neighbours->push_back(new std::list<size_t>());
            }
            
            int windowSize = 3;
            
            unsigned int **inputData = new unsigned int*[3];
			for(int i = 0; i < windowSize; i++)
            {
                inputData[i] = new unsigned int[width];
                for(int j = 0; j < width; j++)
                {
                    inputData[i][j] = 0;
                }
            }
            
            unsigned int **dataBlock = new unsigned int*[windowSize];
            for(int i = 0; i < windowSize; i++)
            {
                dataBlock[i] = new unsigned int[windowSize];
            }
            
            unsigned long clumpID = 0;
            
			feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{				
				if((i % feedback) == 0)
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				                
				for(int m = 0; m < windowSize; m++)
				{
					if(m == 0)
					{
						if(i == 0)
						{
							for(int k = 0; k < width; k++)
                            {
                                inputData[m][k] = 0;
                            }
						}
						else
						{
							imgBand->RasterIO(GF_Read, 0, i-1, width, 1, inputData[m], width, 1, GDT_UInt32, 0, 0);
						}
					}
					else if(m == 2)
					{
						if((i + 1) >= height)
						{
							for(int k = 0; k < width; k++)
                            {
                                inputData[m][k] = 0;
                            }
						}
						else
						{
							imgBand->RasterIO(GF_Read, 0, i+1, width, 1, inputData[m], width, 1, GDT_UInt32, 0, 0);
						}
					}
					else
					{
						imgBand->RasterIO(GF_Read, 0, i, width, 1, inputData[m], width, 1, GDT_UInt32, 0, 0);
					}
				}
				
				for(int j = 0; j < width; j++)
				{
					for(int m = 0; m < windowSize; m++)
					{
						for(int k = 0; k < windowSize; k++)
						{
                            
							if(k == 0)
							{
								if(j == 0)
								{
									dataBlock[m][k] = 0;
								}
								else
								{
									dataBlock[m][k] = inputData[m][(j-1)];
								}
							}
							else if(k == 2)
							{
								if((j + 1) >= width)
								{
									dataBlock[m][k] = 0;
								}
								else
								{
                                    dataBlock[m][k] = inputData[m][(j+1)];
								}
							}
							else
							{
								dataBlock[m][k] = inputData[m][j];
							}
						}
					}
					
					
                    // Process Window.
                    clumpID = dataBlock[1][1];
                    if(clumpID > 0)
                    {
                        if((dataBlock[0][1] > 0) & (dataBlock[0][1] != clumpID))
                        {
                            neighbours->at(clumpID-1)->push_back(dataBlock[0][1]-1);
                        }
                        if((dataBlock[1][0] > 0) & (dataBlock[1][0] != clumpID))
                        {
                            neighbours->at(clumpID-1)->push_back(dataBlock[1][0]-1);
                        }
                        if((dataBlock[1][2] > 0) & (dataBlock[1][2] != clumpID))
                        {
                            neighbours->at(clumpID-1)->push_back(dataBlock[1][2]-1);
                        }
                        if((dataBlock[2][1] > 0) & (dataBlock[2][1] != clumpID))
                        {
                            neighbours->at(clumpID-1)->push_back(dataBlock[2][1]-1);
                        }
                    }
				}
			}
			std::cout << " Complete.\n";
            
			for(int i = 0; i < windowSize; i++)
            {
                delete[] dataBlock[i];
                delete[] inputData[i];
            }
            delete[] dataBlock;
            delete[] inputData;
            
            for(unsigned long i = 0; i < maxClumpIdx; ++i)
            {
                neighbours->at(i)->sort();
                neighbours->at(i)->unique();
            }
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        
        return neighbours;
    }
    
    void RSGISFindClumpNeighbours::findNeighboursKEAImageCalc(GDALDataset *clumpImage, unsigned int ratBand) 
    {
        try
        {
            kealib::KEAImageIO *keaImgIO;
            void *internalData = clumpImage->GetInternalHandle("");
            if(internalData != NULL)
            {
                try
                {
                    keaImgIO = static_cast<kealib::KEAImageIO*>(internalData);
                    
                    if((keaImgIO == NULL) | (keaImgIO == 0))
                    {
                        throw rsgis::img::RSGISImageCalcException("Could not get hold of the internal KEA Image IO Object - was ");
                    }
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
            }
            else
            {
                throw rsgis::img::RSGISImageCalcException("Internal data on GDAL Dataset was NULL - check input file is KEA.");
            }
            
            kealib::KEAAttributeTable *keaAtt = keaImgIO->getAttributeTable(kealib::kea_att_file, ratBand);
            size_t numRows = keaAtt->getSize();
            
            std::vector<std::vector<size_t>* > *neighbours = new std::vector<std::vector<size_t>* >();
            neighbours->reserve(numRows);
            for(size_t i = 0; i < numRows; ++i)
            {
                neighbours->push_back(new std::vector<size_t>());
            }
            
            
            rsgis::img::RSGISCalcImageValue *findNeighbours = new RSGISFindNeighboursCalcImage(numRows, neighbours, ratBand);
            rsgis::img::RSGISCalcImage imgCalc = rsgis::img::RSGISCalcImage(findNeighbours);
            
            imgCalc.calcImageWindowData(&clumpImage, 1, 3);
            
            if(!keaAtt->hasField("NumNeighbours"))
            {
                keaAtt->addAttIntField("NumNeighbours", 0, "");
            }
            size_t numNeighboursIdx = keaAtt->getFieldIndex("NumNeighbours");
            
            keaAtt->setNeighbours(0, neighbours->size(), neighbours);
            
            int64_t *numNeighbours = new int64_t[neighbours->size()];
            unsigned int i = 0;
            for(std::vector<std::vector<size_t>* >::iterator iterClumps = neighbours->begin(); iterClumps != neighbours->end(); ++iterClumps)
            {
                numNeighbours[i++] = (*iterClumps)->size();
            }
            
            keaAtt->setIntFields(0, neighbours->size(), numNeighboursIdx, numNeighbours);
            
            for(std::vector<std::vector<size_t>* >::iterator iterClumps = neighbours->begin(); iterClumps != neighbours->end(); ++iterClumps)
            {
                delete *iterClumps;
            }
            delete neighbours;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(kealib::KEAATTException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(kealib::KEAException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
        
    RSGISFindClumpNeighbours::~RSGISFindClumpNeighbours()
    {
        
    }
    
    
    
    
    RSGISFindNeighboursCalcImage::RSGISFindNeighboursCalcImage(size_t numRows, std::vector<std::vector<size_t>* > *neighbours, unsigned int ratBand) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->numRows = numRows;
        this->neighbours = neighbours;
        this->ratBand = ratBand;
    }
    
    void RSGISFindNeighboursCalcImage::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
    {
        try
        {
            int winHsize = ((winSize-1)/2);
            
            if(dataBlock[ratBand-1][winHsize][winHsize] > 0)
            {
                size_t fid = 0;
                try
                {
                    fid = boost::lexical_cast<size_t>(dataBlock[ratBand-1][winHsize][winHsize]);
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize] = " << dataBlock[ratBand-1][winHsize][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize] = " << dataBlock[ratBand-1][winHsize][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize] = " << dataBlock[ratBand-1][winHsize][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }

                
                
                
                
                size_t fidLeft = 0;
                try
                {
                    fidLeft = boost::lexical_cast<size_t>(dataBlock[ratBand-1][winHsize][winHsize-1]);
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize-1] = " << dataBlock[ratBand-1][winHsize][winHsize-1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize-1] = " << dataBlock[ratBand-1][winHsize][winHsize-1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize-1] = " << dataBlock[ratBand-1][winHsize][winHsize-1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                
                if((fidLeft != 0) && (fidLeft != fid))
                {
                    if(neighbours->at(fid)->size() == 0)
                    {
                        neighbours->at(fid)->push_back(fidLeft);
                    }
                    else
                    {
                        bool found = false;
                        for(std::vector<size_t>::iterator iterVal = neighbours->at(fid)->begin(); iterVal != neighbours->at(fid)->end(); ++iterVal)
                        {
                            if((*iterVal) == fidLeft)
                            {
                                found = true;
                                break;
                            }
                        }
                        
                        if(!found)
                        {
                            neighbours->at(fid)->push_back(fidLeft);
                        }
                    }
                }
                
                
                
                
                
                size_t fidUp = 0;
                try
                {
                    fidUp = boost::lexical_cast<size_t>(dataBlock[ratBand-1][winHsize+1][winHsize]);
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize+1][winHsize] = " << dataBlock[ratBand-1][winHsize+1][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize+1][winHsize] = " << dataBlock[ratBand-1][winHsize+1][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize+1][winHsize] = " << dataBlock[ratBand-1][winHsize+1][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                
                if((fidUp != 0) && (fidUp != fid))
                {
                    if(neighbours->at(fid)->size() == 0)
                    {
                        neighbours->at(fid)->push_back(fidUp);
                    }
                    else
                    {
                        bool found = false;
                        for(std::vector<size_t>::iterator iterVal = neighbours->at(fid)->begin(); iterVal != neighbours->at(fid)->end(); ++iterVal)
                        {
                            if((*iterVal) == fidUp)
                            {
                                found = true;
                                break;
                            }
                        }
                        
                        if(!found)
                        {
                            neighbours->at(fid)->push_back(fidUp);
                        }
                    }
                }
                
                
                
                
                size_t fidRight = 0;
                try
                {
                    fidRight = boost::lexical_cast<size_t>(dataBlock[ratBand-1][winHsize][winHsize+1]);
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize+1] = " << dataBlock[ratBand-1][winHsize][winHsize+1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize+1] = " << dataBlock[ratBand-1][winHsize][winHsize+1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize+1] = " << dataBlock[ratBand-1][winHsize][winHsize+1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                
                if((fidRight != 0) && (fidRight != fid))
                {
                    if(neighbours->at(fid)->size() == 0)
                    {
                        neighbours->at(fid)->push_back(fidRight);
                    }
                    else
                    {
                        bool found = false;
                        for(std::vector<size_t>::iterator iterVal = neighbours->at(fid)->begin(); iterVal != neighbours->at(fid)->end(); ++iterVal)
                        {
                            if((*iterVal) == fidRight)
                            {
                                found = true;
                                break;
                            }
                        }
                        
                        if(!found)
                        {
                            neighbours->at(fid)->push_back(fidRight);
                        }
                    }
                }
                
                
                
                
                size_t fidDown = 0;
                try
                {
                    fidDown = boost::lexical_cast<size_t>(dataBlock[ratBand-1][winHsize-1][winHsize]);
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize-1][winHsize] = " << dataBlock[ratBand-1][winHsize-1][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize-1][winHsize] = " << dataBlock[ratBand-1][winHsize-1][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize-1][winHsize] = " << dataBlock[ratBand-1][winHsize-1][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                
                if((fidDown != 0) && (fidDown != fid))
                {
                    if(neighbours->at(fid)->size() == 0)
                    {
                        neighbours->at(fid)->push_back(fidDown);
                    }
                    else
                    {
                        bool found = false;
                        for(std::vector<size_t>::iterator iterVal = neighbours->at(fid)->begin(); iterVal != neighbours->at(fid)->end(); ++iterVal)
                        {
                            if((*iterVal) == fidDown)
                            {
                                found = true;
                                break;
                            }
                        }
                        
                        if(!found)
                        {
                            neighbours->at(fid)->push_back(fidDown);
                        }
                    }
                }
            }
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    RSGISFindNeighboursCalcImage::~RSGISFindNeighboursCalcImage()
    {
        
    }
    
    
    
    
    
    RSGISIdentifyBoundaryPixels::RSGISIdentifyBoundaryPixels(unsigned int ratBand) : rsgis::img::RSGISCalcImageValue(1)
    {
        this->ratBand = ratBand;
    }
    
    void RSGISIdentifyBoundaryPixels::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
    {
        try
        {
            int winHsize = ((winSize-1)/2);
            
            if(dataBlock[ratBand-1][winHsize][winHsize] > 0)
            {
                size_t fid = 0;
                try
                {
                    fid = boost::lexical_cast<size_t>(dataBlock[ratBand-1][winHsize][winHsize]);
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize] = " << dataBlock[ratBand-1][winHsize][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize] = " << dataBlock[ratBand-1][winHsize][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize] = " << dataBlock[ratBand-1][winHsize][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                
                
                
                
                
                size_t fidLeft = 0;
                bool fidEdge = false;
                try
                {
                    fidLeft = boost::lexical_cast<size_t>(dataBlock[ratBand-1][winHsize][winHsize-1]);
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize-1] = " << dataBlock[ratBand-1][winHsize][winHsize-1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize-1] = " << dataBlock[ratBand-1][winHsize][winHsize-1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize-1] = " << dataBlock[ratBand-1][winHsize][winHsize-1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                
                if((fidLeft != 0) && (fidLeft != fid))
                {
                    fidEdge = true;
                }
                
                size_t fidUp = 0;
                try
                {
                    fidUp = boost::lexical_cast<size_t>(dataBlock[ratBand-1][winHsize+1][winHsize]);
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize+1][winHsize] = " << dataBlock[ratBand-1][winHsize+1][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize+1][winHsize] = " << dataBlock[ratBand-1][winHsize+1][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize+1][winHsize] = " << dataBlock[ratBand-1][winHsize+1][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                
                if((fidUp != 0) && (fidUp != fid))
                {
                    fidEdge = true;
                }
                
                
                
                
                size_t fidRight = 0;
                try
                {
                    fidRight = boost::lexical_cast<size_t>(dataBlock[ratBand-1][winHsize][winHsize+1]);
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize+1] = " << dataBlock[ratBand-1][winHsize][winHsize+1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize+1] = " << dataBlock[ratBand-1][winHsize][winHsize+1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize][winHsize+1] = " << dataBlock[ratBand-1][winHsize][winHsize+1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                
                if((fidRight != 0) && (fidRight != fid))
                {
                    fidEdge = true;
                }
                
                
                
                
                size_t fidDown = 0;
                try
                {
                    fidDown = boost::lexical_cast<size_t>(dataBlock[ratBand-1][winHsize-1][winHsize]);
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize-1][winHsize] = " << dataBlock[ratBand-1][winHsize-1][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize-1][winHsize] = " << dataBlock[ratBand-1][winHsize-1][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    std::cout << "dataBlock[" << ratBand-1 << "][winHsize-1][winHsize] = " << dataBlock[ratBand-1][winHsize-1][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                
                if((fidDown != 0) && (fidDown != fid))
                {
                    fidEdge = true;
                }
                
                if(fidEdge)
                {
                    output[0] = 1;
                }
                else
                {
                    output[0] = 0;
                }
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
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    RSGISIdentifyBoundaryPixels::~RSGISIdentifyBoundaryPixels()
    {
        
    }
    
    
}}
