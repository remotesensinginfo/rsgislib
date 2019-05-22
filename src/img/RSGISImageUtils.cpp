/*
 *  RSGISImageUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/04/2008.
 *  Copyright 2008 RSGISLib.
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

#include "RSGISImageUtils.h"

namespace rsgis{namespace img{

	RSGISImageUtils::RSGISImageUtils(double resDiffThresh)
	{
		 this->resDiffThresh = resDiffThresh;
	}
	
	void RSGISImageUtils::getImageOverlap(GDALDataset **datasets, int numDS, int **dsOffsets, int *width, int *height, double *gdalTransform) throw(RSGISImageBandException)
	{
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		bool first = true;
		const char *proj = NULL;
		
		try
		{
			for(int i = 0; i < numDS; i++)
			{
                if(transformations[i] == NULL)
                {
                    throw RSGISImageBandException("No projection transformation has been provided..");
                }
                
				if(first)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}

					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if((this->closeResTest(pixelXRes, transformations[i][1]) == false) | (this->closeResTest(pixelYRes, transformations[i][5]) == false))
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(std::string(datasets[i]->GetProjectionRef()) != std::string(proj))
					{
						std::cout << "Not all image bands have the same projection" << std::endl;
					}
                    
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
				}
			}

            if(maxX - minX <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the Y axis");
			}
			
			gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
            
			*width = floor(((maxX - minX)/pixelXRes)+0.5);
			*height = floor(((maxY - minY)/pixelYResPos)+0.5);
			
			double diffX = 0;
			double diffY = 0;
			for(int i = 0; i < numDS; i++)
			{
				diffX = minX - transformations[i][0];
				diffY = transformations[i][3] - maxY;

                if(!((diffX > -0.0001) & (diffX < 0.0001)))
				{
                    dsOffsets[i][0] = floor((diffX/pixelXRes)+0.5);
				}
				else
				{
                    dsOffsets[i][0] = 0;
				}
				
				if(!((diffY > -0.0001) & (diffY < 0.0001)))
				{
					dsOffsets[i][1] = floor((diffY/pixelYResPos)+0.5);
				}
				else
				{
					dsOffsets[i][1] = 0;
				}
			}
			
            double tmpMinX = 0;
            double tmpMaxY = 0;
            tmpMaxX = 0;
            tmpMinY = 0;
            
            double maxDiffX = 0;
            double maxDiffY = 0;
            bool foundXDiff = false;
            bool foundYDiff = false;
            
            for(int i = 0; i < numDS; i++)
            {
                tmpMinX = transformations[i][0] + (dsOffsets[i][0] * pixelXRes);
                tmpMaxY = transformations[i][3] - (dsOffsets[i][1] * pixelYResPos);
                
                tmpMaxX = tmpMinX + ((*width)*pixelXRes);
                tmpMinY = tmpMaxY - ((*height)*pixelYResPos);
                
                if(tmpMaxX > maxX)
                {
                    diffX = (tmpMaxX - maxX);

                    if(!foundXDiff)
                    {
                        maxDiffX = diffX;
                        foundXDiff = true;
                    }
                    else if(diffX > maxDiffX)
                    {
                        maxDiffX = diffX;
                    }
                }
                
                if(tmpMinY < minY)
                {
                    diffY = (minY - tmpMinY);

                    if(!foundYDiff)
                    {
                        maxDiffY = diffY;
                        foundYDiff = true;
                    }
                    else if(diffY > maxDiffY)
                    {
                        maxDiffY = diffY;
                    }
                }
            }
            
            if(foundXDiff)
            {
                int nPxl = floor((maxDiffX/pixelXRes)+0.5);
                if(nPxl > 0)
                {
                    (*width) = (*width) - nPxl;
                }
            }
            
            if(foundYDiff)
            {
                int nPxl = floor((maxDiffY/pixelYResPos)+0.5);
                if(nPxl > 0)
                {
                    (*height) = (*height) - nPxl;
                }
            }
            
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
    
    void RSGISImageUtils::getImageOverlap(std::vector<GDALDataset*> *datasets, int **dsOffsets, int *width, int *height, double *gdalTransform) throw(RSGISImageBandException)
	{
        unsigned int numDS = datasets->size();
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets->at(i)->GetGeoTransform(transformations[i]);
			xSize[i] = datasets->at(i)->GetRasterXSize();
			ySize[i] = datasets->at(i)->GetRasterYSize();
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		bool first = true;
		const char *proj = NULL;
		
		try
		{
			for(int i = 0; i < numDS; i++)
			{
                if(transformations[i] == NULL)
                {
                    throw RSGISImageBandException("No projection transformation has been provided..");
                }
                
				if(first)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets->at(i)->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if((this->closeResTest(pixelXRes, transformations[i][1]) == false) | (this->closeResTest(pixelYRes, transformations[i][5]) == false))
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(std::string(datasets->at(i)->GetProjectionRef()) != std::string(proj))
					{
						std::cout << "Not all image bands have the same projection" << std::endl;
					}
                    
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
				}
			}
            
			if(maxX - minX <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the Y axis");
			}
			
			gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
            
			*width = floor(((maxX - minX)/pixelXRes)+0.5);
			*height = floor(((maxY - minY)/pixelYResPos)+0.5);
			
			double diffX = 0;
			double diffY = 0;
			
			for(int i = 0; i < numDS; i++)
			{
				diffX = minX - transformations[i][0];
				diffY = transformations[i][3] - maxY;
				
				if(!((diffX > -0.0001) & (diffX < 0.0001)))
				{
					dsOffsets[i][0] = floor((diffX/pixelXRes)+0.5);
				}
				else
				{
					dsOffsets[i][0] = 0;
				}
				
				if(!((diffY > -0.0001) & (diffY < 0.0001)))
				{
					dsOffsets[i][1] = floor((diffY/pixelYResPos)+0.5);
				}
				else
				{
					dsOffsets[i][1] = 0;
				}
			}
			
            
            double tmpMinX = 0;
            double tmpMaxY = 0;
            tmpMaxX = 0;
            tmpMinY = 0;
            
            double maxDiffX = 0;
            double maxDiffY = 0;
            bool foundXDiff = false;
            bool foundYDiff = false;
            
            for(int i = 0; i < numDS; i++)
            {
                tmpMinX = transformations[i][0] + (dsOffsets[i][0] * pixelXRes);
                tmpMaxY = transformations[i][3] - (dsOffsets[i][1] * pixelYResPos);
                
                tmpMaxX = tmpMinX + ((*width)*pixelXRes);
                tmpMinY = tmpMaxY - ((*height)*pixelYResPos);
                
                if(tmpMaxX > maxX)
                {
                    diffX = (tmpMaxX - maxX);
                    if(!foundXDiff)
                    {
                        maxDiffX = diffX;
                        foundXDiff = true;
                    }
                    else if(diffX > maxDiffX)
                    {
                        maxDiffX = diffX;
                    }
                }
                
                if(tmpMinY < minY)
                {
                    diffY = (minY - tmpMinY);
                    if(!foundYDiff)
                    {
                        maxDiffY = diffY;
                        foundYDiff = true;
                    }
                    else if(diffY > maxDiffY)
                    {
                        maxDiffY = diffY;
                    }
                }
            }
            
            if(foundXDiff)
            {
                int nPxl = floor((maxDiffX/pixelXRes)+0.5);
                if(nPxl > 0)
                {
                    (*width) = (*width) - nPxl;
                }
            }
            
            if(foundYDiff)
            {
                int nPxl = floor((maxDiffY/pixelYResPos)+0.5);
                if(nPxl > 0)
                {
                    (*height) = (*height) - nPxl;
                }
            }
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
    
    void RSGISImageUtils::getImageOverlap(GDALDataset **datasets, int numDS,  int **dsOffsets, int *width, int *height, double *gdalTransform, int *maxBlockX, int *maxBlockY) throw(RSGISImageBandException)
	{
        std::cout.precision(12);
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
        int *xBlockSize = new int[numDS];
		int *yBlockSize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
            datasets[i]->GetRasterBand(1)->GetBlockSize(&xBlockSize[i], &yBlockSize[i]);
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		bool first = true;
		const char *proj = NULL;
		
		try
		{
			for(int i = 0; i < numDS; i++)
			{                
                if(transformations[i] == NULL)
                {
                    throw RSGISImageBandException("No projection transformation has been provided..");
                }
                
				if(first)
				{
                    *maxBlockX = xBlockSize[i];
                    *maxBlockY = yBlockSize[i];
                    
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{                    
					if((this->closeResTest(pixelXRes, transformations[i][1]) == false) | (this->closeResTest(pixelYRes, transformations[i][5]) == false))
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(std::string(datasets[i]->GetProjectionRef()) != std::string(proj))
					{
						std::cout << "Not all image bands have the same projection" << std::endl;
					}
                    
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
                    
                    if(xBlockSize[i] > (*maxBlockX))
                    {
                        *maxBlockX = xBlockSize[i];
                    }
                    
                    if(yBlockSize[i] > (*maxBlockY))
                    {
                        *maxBlockY = yBlockSize[i];
                    }
				}
			}
            
			if(maxX - minX <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the Y axis");
			}
			
			gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
            
			*width = floor(((maxX - minX)/pixelXRes)+0.5);
			*height = floor(((maxY - minY)/pixelYResPos)+0.5);
			
			double diffX = 0;
			double diffY = 0;
			
			for(int i = 0; i < numDS; i++)
			{
				diffX = minX - transformations[i][0];
				diffY = transformations[i][3] - maxY;
				
				if(!((diffX > -0.0001) & (diffX < 0.0001)))
				{
					dsOffsets[i][0] = floor((diffX/pixelXRes)+0.5);
				}
				else
				{
					dsOffsets[i][0] = 0;
				}
				
				if(!((diffY > -0.0001) & (diffY < 0.0001)))
				{
					dsOffsets[i][1] = floor((diffY/pixelYResPos)+0.5);
				}
				else
				{
					dsOffsets[i][1] = 0;
				}
			}
            
            double tmpMinX = 0;
            double tmpMaxY = 0;
            tmpMaxX = 0;
            tmpMinY = 0;
            
            double maxDiffX = 0;
            double maxDiffY = 0;
            bool foundXDiff = false;
            bool foundYDiff = false;
            
            for(int i = 0; i < numDS; i++)
            {
                tmpMinX = transformations[i][0] + (dsOffsets[i][0] * pixelXRes);
                tmpMaxY = transformations[i][3] - (dsOffsets[i][1] * pixelYResPos);
                
                tmpMaxX = tmpMinX + ((*width)*pixelXRes);
                tmpMinY = tmpMaxY - ((*height)*pixelYResPos);
                
                if(tmpMaxX > maxX)
                {
                    diffX = (tmpMaxX - maxX);
                    if(!foundXDiff)
                    {
                        maxDiffX = diffX;
                        foundXDiff = true;
                    }
                    else if(diffX > maxDiffX)
                    {
                        maxDiffX = diffX;
                    }
                }
                
                if(tmpMinY < minY)
                {
                    diffY = (minY - tmpMinY);
                    if(!foundYDiff)
                    {
                        maxDiffY = diffY;
                        foundYDiff = true;
                    }
                    else if(diffY > maxDiffY)
                    {
                        maxDiffY = diffY;
                    }
                }
            }
            
            if(foundXDiff)
            {
                int nPxl = floor((maxDiffX/pixelXRes)+0.5);
                if(nPxl > 0)
                {
                    (*width) = (*width) - nPxl;
                }
            }
            
            if(foundYDiff)
            {
                int nPxl = floor((maxDiffY/pixelYResPos)+0.5);
                if(nPxl > 0)
                {
                    (*height) = (*height) - nPxl;
                }
            }
			
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
    
    void RSGISImageUtils::getImageOverlap(std::vector<GDALDataset*> *datasets,  int **dsOffsets, int *width, int *height, double *gdalTransform, int *maxBlockX, int *maxBlockY) throw(RSGISImageBandException)
	{
        unsigned int numDS = datasets->size();
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
        int *xBlockSize = new int[numDS];
		int *yBlockSize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets->at(i)->GetGeoTransform(transformations[i]);
			xSize[i] = datasets->at(i)->GetRasterXSize();
			ySize[i] = datasets->at(i)->GetRasterYSize();
            datasets->at(i)->GetRasterBand(1)->GetBlockSize(&xBlockSize[i], &yBlockSize[i]);
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		bool first = true;
		const char *proj = NULL;
		
		try
		{
			for(int i = 0; i < numDS; i++)
			{
                if(transformations[i] == NULL)
                {
                    throw RSGISImageBandException("No projection transformation has been provided..");
                }
                
				if(first)
				{
                    *maxBlockX = xBlockSize[i];
                    *maxBlockY = yBlockSize[i];
                    
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets->at(i)->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if((this->closeResTest(pixelXRes, transformations[i][1]) == false) | (this->closeResTest(pixelYRes, transformations[i][5]) == false))
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(std::string(datasets->at(i)->GetProjectionRef()) != std::string(proj))
					{
						std::cout << "Not all image bands have the same projection" << std::endl;
					}
                    
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
                    
                    if(xBlockSize[i] > (*maxBlockX))
                    {
                        *maxBlockX = xBlockSize[i];
                    }
                    
                    if(yBlockSize[i] > (*maxBlockY))
                    {
                        *maxBlockY = yBlockSize[i];
                    }
				}
			}
            
			if(maxX - minX <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the Y axis");
			}
			
			gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
            
			*width = floor(((maxX - minX)/pixelXRes)+0.5);
			*height = floor(((maxY - minY)/pixelYResPos)+0.5);
			
			double diffX = 0;
			double diffY = 0;
			
			for(int i = 0; i < numDS; i++)
			{
				diffX = minX - transformations[i][0];
				diffY = transformations[i][3] - maxY;
				
				if(!((diffX > -0.0001) & (diffX < 0.0001)))
				{
					dsOffsets[i][0] = floor((diffX/pixelXRes)+0.5);
				}
				else
				{
					dsOffsets[i][0] = 0;
				}
				
				if(!((diffY > -0.0001) & (diffY < 0.0001)))
				{
					dsOffsets[i][1] = floor((diffY/pixelYResPos)+0.5);
				}
				else
				{
					dsOffsets[i][1] = 0;
				}
			}
            
            double tmpMinX = 0;
            double tmpMaxY = 0;
            tmpMaxX = 0;
            tmpMinY = 0;
            
            double maxDiffX = 0;
            double maxDiffY = 0;
            bool foundXDiff = false;
            bool foundYDiff = false;
            
            for(int i = 0; i < numDS; i++)
            {
                tmpMinX = transformations[i][0] + (dsOffsets[i][0] * pixelXRes);
                tmpMaxY = transformations[i][3] - (dsOffsets[i][1] * pixelYResPos);
                
                tmpMaxX = tmpMinX + ((*width)*pixelXRes);
                tmpMinY = tmpMaxY - ((*height)*pixelYResPos);
                
                if(tmpMaxX > maxX)
                {
                    diffX = (tmpMaxX - maxX);
                    if(!foundXDiff)
                    {
                        maxDiffX = diffX;
                        foundXDiff = true;
                    }
                    else if(diffX > maxDiffX)
                    {
                        maxDiffX = diffX;
                    }
                }
                
                if(tmpMinY < minY)
                {
                    diffY = (minY - tmpMinY);
                    if(!foundYDiff)
                    {
                        maxDiffY = diffY;
                        foundYDiff = true;
                    }
                    else if(diffY > maxDiffY)
                    {
                        maxDiffY = diffY;
                    }
                }
            }
            
            if(foundXDiff)
            {
                int nPxl = floor((maxDiffX/pixelXRes)+0.5);
                if(nPxl > 0)
                {
                    (*width) = (*width) - nPxl;
                }
            }
            
            if(foundYDiff)
            {
                int nPxl = floor((maxDiffY/pixelYResPos)+0.5);
                if(nPxl > 0)
                {
                    (*height) = (*height) - nPxl;
                }
            }
			
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
    
	void RSGISImageUtils::getImageOverlap(GDALDataset **datasets, int numDS,  int **dsOffsets, int *width, int *height, double *gdalTransform, geos::geom::Envelope *env) throw(RSGISImageBandException)
	{
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		const char *proj = NULL;
		bool first = true;
		
		
		try
		{
			// Calculate Image Overlap.
			for(int i = 0; i < numDS; ++i)
			{
				if(first)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if((this->closeResTest(pixelXRes, transformations[i][1]) == false) | (this->closeResTest(pixelYRes, transformations[i][5]) == false))
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(std::string(datasets[i]->GetProjectionRef()) != std::string(proj))
					{
						throw RSGISImageBandException("Not all image bands have the same projection..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
				}
			}

			if(maxX - minX <= 0)
			{
				std::cout << "MinX = " << minX << std::endl;
				std::cout << "MaxX = " << maxX << std::endl;
				throw RSGISImageBandException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				std::cout << "MinY = " << minY << std::endl;
				std::cout << "MaxY = " << maxY << std::endl;
				throw RSGISImageBandException("Images do not overlap in the Y axis");
			}
			
            
			// Check if OK to process.
			// Changed from throwing exception (left old code) - Dan
			bool process = true;
			
			if(minX > env->getMinX())
			{
				process = false;
			}
			
			if(minY > env->getMinY())
			{
				process = false;
			}
			
			if(maxX < env->getMaxX())
			{
				process = false;
			}
			
			if(maxY < env->getMaxY())
			{
				process = false;
			}
			
			if(process)
			{
				// Trim to the envelope
				minX = env->getMinX();
				maxX = env->getMaxX();
				minY = env->getMinY();
				maxY = env->getMaxY();
                
				// Define output values.
				gdalTransform[0] = minX;
				gdalTransform[1] = pixelXRes;
				gdalTransform[2] = rotateX;
				gdalTransform[3] = maxY;
				gdalTransform[4] = rotateY;
				gdalTransform[5] = pixelYRes;
				
				*width = floor(((maxX - minX)/pixelXRes)+0.5);
				*height = floor(((maxY - minY)/pixelYResPos)+0.5);
				
				double diffX = 0;
				double diffY = 0;
				
				for(int i = 0; i < numDS; i++)
				{
                    diffX = minX - transformations[i][0];
                    diffY = transformations[i][3] - maxY;
                    
                    if(!((diffX > -0.0001) & (diffX < 0.0001)))
                    {
                        dsOffsets[i][0] = floor((diffX/pixelXRes)+0.5);
                    }
                    else
                    {
                        dsOffsets[i][0] = 0;
                    }
                    
                    if(!((diffY > -0.0001) & (diffY < 0.0001)))
                    {
                        dsOffsets[i][1] = floor((diffY/pixelYResPos)+0.5);
                    }
                    else
                    {
                        dsOffsets[i][1] = 0;
                    }
				}
			}
            /* Commented out else statement, this was added to fix problem with cut2poly but caused problems with zonal stats commands
			 * It has therefore been commented out - Dan Clewley 24/01/11 
             * Update (16/03/11) - Dan Clewley
             * - The uncommented code fixed the problem of a polygon that was larger than the image but caused problems when
             *  the polygon was outside the scene.
             * - If the polygon does not fit inside the scene the center is calculated, if this is within the scene it calculates the overlap.
             * Update (09/12/13) - Pete Bunting
             * Uncommented lines, doesn't seem to be causing problems.
             */
			else
			{				
                // Calculate centre of envelope
                double centreEnvelopeX = (env->getMaxX() + env->getMinX()) / 2.0;
                double centreEnvelopeY = (env->getMaxY() + env->getMinY()) / 2.0;
                
                if((centreEnvelopeX < maxX) && (centreEnvelopeY < maxY)) // If center of envolope within overlap proceed
                {
                    // Define output values.
                    gdalTransform[0] = minX;
                    gdalTransform[1] = pixelXRes;
                    gdalTransform[2] = rotateX;
                    gdalTransform[3] = maxY;
                    gdalTransform[4] = rotateY;
                    gdalTransform[5] = pixelYRes;
                    
                    *width = floor(((maxX - minX)/pixelXRes)+0.5);
                    *height = floor(((maxY - minY)/pixelYResPos)+0.5);
                    
                    double diffX = 0;
                    double diffY = 0;
                    
                    for(int i = 0; i < numDS; i++)
                    {
                        diffX = minX - transformations[i][0];
                        diffY = transformations[i][3] - maxY;
                        
                        if(!((diffX > -0.0001) & (diffX < 0.0001)))
                        {
                            dsOffsets[i][0] = floor((diffX/pixelXRes)+0.5);
                        }
                        else
                        {
                            dsOffsets[i][0] = 0;
                        }
                        
                        if(!((diffY > -0.0001) & (diffY < 0.0001)))
                        {
                            dsOffsets[i][1] = floor((diffY/pixelYResPos)+0.5);
                        }
                        else
                        {
                            dsOffsets[i][1] = 0;
                        }
                    }
                    
                    
                }
			}
			
            double tmpMinX = 0;
            double tmpMaxY = 0;
            double tmpMaxX = 0;
            double tmpMinY = 0;
            double diffX = 0.0;
            double diffY = 0.0;
            
            double maxDiffX = 0;
            double maxDiffY = 0;
            bool foundXDiff = false;
            bool foundYDiff = false;
            
            for(int i = 0; i < numDS; i++)
            {
                tmpMinX = transformations[i][0] + (dsOffsets[i][0] * pixelXRes);
                tmpMaxY = transformations[i][3] - (dsOffsets[i][1] * pixelYResPos);
                
                tmpMaxX = tmpMinX + ((*width)*pixelXRes);
                tmpMinY = tmpMaxY - ((*height)*pixelYResPos);
                
                if(tmpMaxX > maxX)
                {
                    diffX = (tmpMaxX - maxX);
                    if(!foundXDiff)
                    {
                        maxDiffX = diffX;
                        foundXDiff = true;
                    }
                    else if(diffX > maxDiffX)
                    {
                        maxDiffX = diffX;
                    }
                }
                
                if(tmpMinY < minY)
                {
                    diffY = (minY - tmpMinY);
                    if(!foundYDiff)
                    {
                        maxDiffY = diffY;
                        foundYDiff = true;
                    }
                    else if(diffY > maxDiffY)
                    {
                        maxDiffY = diffY;
                    }
                }
            }
            
            if(foundXDiff)
            {
                int nPxl = floor((maxDiffX/pixelXRes)+0.5);
                if(nPxl > 0)
                {
                    (*width) = (*width) - nPxl;
                }
            }
            
            if(foundYDiff)
            {
                int nPxl = floor((maxDiffY/pixelYResPos)+0.5);
                if(nPxl > 0)
                {
                    (*height) = (*height) - nPxl;
                }
            }
            
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
    
    void RSGISImageUtils::getImageOverlapCut2Env(GDALDataset **datasets, int numDS,  int **dsOffsets, int *width, int *height, double *gdalTransform, geos::geom::Envelope *env) throw(RSGISImageBandException)
	{
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		const char *proj = NULL;
		bool first = true;
		
		
		try
		{
			// Calculate Image Overlap.
			for(int i = 0; i < numDS; ++i)
			{
				if(first)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if((this->closeResTest(pixelXRes, transformations[i][1]) == false) | (this->closeResTest(pixelYRes, transformations[i][5]) == false))
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(std::string(datasets[i]->GetProjectionRef()) != std::string(proj))
					{
						throw RSGISImageBandException("Not all image bands have the same projection..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
				}
			}
            
			if(maxX - minX <= 0)
			{
				std::cout << "MinX = " << minX << std::endl;
				std::cout << "MaxX = " << maxX << std::endl;
				throw RSGISImageBandException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				std::cout << "MinY = " << minY << std::endl;
				std::cout << "MaxY = " << maxY << std::endl;
				throw RSGISImageBandException("Images do not overlap in the Y axis");
			}
            
			
			// Cut to env extent
			if(env->getMinX() > minX)
			{
                minX = env->getMinX();
			}
			
			if(env->getMinY() > minY)
			{
				minY = env->getMinY();
			}
			
			if(env->getMaxX() < maxX)
			{
				maxX = env->getMaxX();
			}
			
			if(env->getMaxY() < maxY)
			{
				maxY = env->getMaxY();
			}
			
            if(maxX - minX <= 0)
			{
				std::cout << "MinX = " << minX << std::endl;
				std::cout << "MaxX = " << maxX << std::endl;
				throw RSGISImageBandException("Images and Envelope do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				std::cout << "MinY = " << minY << std::endl;
				std::cout << "MaxY = " << maxY << std::endl;
				throw RSGISImageBandException("Images and Envelope do not overlap in the Y axis");
			}
            
            gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
			
			*width = floor(((maxX - minX)/pixelXRes)+0.5);
			*height = floor(((maxY - minY)/pixelYResPos)+0.5);
			
			double diffX = 0;
			double diffY = 0;
			
			for(int i = 0; i < numDS; i++)
			{
                diffX = minX - transformations[i][0];
                diffY = transformations[i][3] - maxY;
                
                if(!((diffX > -0.0001) & (diffX < 0.0001)))
                {
                    dsOffsets[i][0] = floor((diffX/pixelXRes)+0.5);
                }
                else
                {
                    dsOffsets[i][0] = 0;
                }
                
                if(!((diffY > -0.0001) & (diffY < 0.0001)))
                {
                    dsOffsets[i][1] = floor((diffY/pixelYResPos)+0.5);
                }
                else
                {
                    dsOffsets[i][1] = 0;
                }
			}
            
            double tmpMinX = 0;
            double tmpMaxY = 0;
            double tmpMaxX = 0;
            double tmpMinY = 0;
            
            double maxDiffX = 0;
            double maxDiffY = 0;
            bool foundXDiff = false;
            bool foundYDiff = false;
            
            for(int i = 0; i < numDS; i++)
            {
                tmpMinX = transformations[i][0] + (dsOffsets[i][0] * pixelXRes);
                tmpMaxY = transformations[i][3] - (dsOffsets[i][1] * pixelYResPos);
                
                tmpMaxX = tmpMinX + ((*width)*pixelXRes);
                tmpMinY = tmpMaxY - ((*height)*pixelYResPos);
                
                if(tmpMaxX > maxX)
                {
                    diffX = (tmpMaxX - maxX);
                    if(!foundXDiff)
                    {
                        maxDiffX = diffX;
                        foundXDiff = true;
                    }
                    else if(diffX > maxDiffX)
                    {
                        maxDiffX = diffX;
                    }
                }
                
                if(tmpMinY < minY)
                {
                    diffY = (minY - tmpMinY);
                    if(!foundYDiff)
                    {
                        maxDiffY = diffY;
                        foundYDiff = true;
                    }
                    else if(diffY > maxDiffY)
                    {
                        maxDiffY = diffY;
                    }
                }
            }
            
            if(foundXDiff)
            {
                int nPxl = floor((maxDiffX/pixelXRes)+0.5);
                if(nPxl > 0)
                {
                    (*width) = (*width) - nPxl;
                }
            }
            
            if(foundYDiff)
            {
                int nPxl = floor((maxDiffY/pixelYResPos)+0.5);
                if(nPxl > 0)
                {
                    (*height) = (*height) - nPxl;
                }
            }
			
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
    
    void RSGISImageUtils::getImageOverlapCut2Env(GDALDataset **datasets, int numDS,  int **dsOffsets, int *width, int *height, double *gdalTransform, geos::geom::Envelope *env, int *maxBlockX, int *maxBlockY) throw(RSGISImageBandException)
	{
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
        int *xBlockSize = new int[numDS];
		int *yBlockSize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
			datasets[i]->GetRasterBand(1)->GetBlockSize(&xBlockSize[i], &yBlockSize[i]);
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		const char *proj = NULL;
		bool first = true;
		
		
		try
		{
			// Calculate Image Overlap.
			for(int i = 0; i < numDS; ++i)
			{
				if(first)
				{
                    *maxBlockX = xBlockSize[i];
                    *maxBlockY = yBlockSize[i];
                    
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if((this->closeResTest(pixelXRes, transformations[i][1]) == false) | (this->closeResTest(pixelYRes, transformations[i][5]) == false))
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(std::string(datasets[i]->GetProjectionRef()) != std::string(proj))
					{
						throw RSGISImageBandException("Not all image bands have the same projection..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
                    
                    if(xBlockSize[i] > (*maxBlockX))
                    {
                        *maxBlockX = xBlockSize[i];
                    }
                    
                    if(yBlockSize[i] > (*maxBlockY))
                    {
                        *maxBlockY = yBlockSize[i];
                    }
				}
			}
            
			if(maxX - minX <= 0)
			{
				std::cout << "MinX = " << minX << std::endl;
				std::cout << "MaxX = " << maxX << std::endl;
				throw RSGISImageBandException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				std::cout << "MinY = " << minY << std::endl;
				std::cout << "MaxY = " << maxY << std::endl;
				throw RSGISImageBandException("Images do not overlap in the Y axis");
			}
			
			// Cut to env extent
			if(env->getMinX() > minX)
			{
                minX = env->getMinX();
			}
			
			if(env->getMinY() > minY)
			{
				minY = env->getMinY();
			}
			
			if(env->getMaxX() < maxX)
			{
				maxX = env->getMaxX();
			}
			
			if(env->getMaxY() < maxY)
			{
				maxY = env->getMaxY();
			}
			
            if(maxX - minX <= 0)
			{
				std::cout << "MinX = " << minX << std::endl;
				std::cout << "MaxX = " << maxX << std::endl;
				throw RSGISImageBandException("Images and Envelope do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				std::cout << "MinY = " << minY << std::endl;
				std::cout << "MaxY = " << maxY << std::endl;
				throw RSGISImageBandException("Images and Envelope do not overlap in the Y axis");
			}
            
            gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
			
			*width = floor(((maxX - minX)/pixelXRes)+0.5);
			*height = floor(((maxY - minY)/pixelYResPos)+0.5);
			
			double diffX = 0;
			double diffY = 0;
			
			for(int i = 0; i < numDS; i++)
			{
				diffX = minX - transformations[i][0];
				diffY = transformations[i][3] - maxY;
				
                if(!((diffX > -0.0001) & (diffX < 0.0001)))
                {
                    dsOffsets[i][0] = floor((diffX/pixelXRes)+0.5);
                }
                else
                {
                    dsOffsets[i][0] = 0;
                }
                
                if(!((diffY > -0.0001) & (diffY < 0.0001)))
                {
                    dsOffsets[i][1] = floor((diffY/pixelYResPos)+0.5);
                }
                else
                {
                    dsOffsets[i][1] = 0;
                }
			}
			
            double tmpMinX = 0;
            double tmpMaxY = 0;
            tmpMaxX = 0;
            tmpMinY = 0;
            
            double maxDiffX = 0;
            double maxDiffY = 0;
            bool foundXDiff = false;
            bool foundYDiff = false;
            
            for(int i = 0; i < numDS; i++)
            {
                tmpMinX = transformations[i][0] + (dsOffsets[i][0] * pixelXRes);
                tmpMaxY = transformations[i][3] - (dsOffsets[i][1] * pixelYResPos);
                
                tmpMaxX = tmpMinX + ((*width)*pixelXRes);
                tmpMinY = tmpMaxY - ((*height)*pixelYResPos);
                
                if(tmpMaxX > maxX)
                {
                    diffX = (tmpMaxX - maxX);
                    if(!foundXDiff)
                    {
                        maxDiffX = diffX;
                        foundXDiff = true;
                    }
                    else if(diffX > maxDiffX)
                    {
                        maxDiffX = diffX;
                    }
                }
                
                if(tmpMinY < minY)
                {
                    diffY = (minY - tmpMinY);
                    if(!foundYDiff)
                    {
                        maxDiffY = diffY;
                        foundYDiff = true;
                    }
                    else if(diffY > maxDiffY)
                    {
                        maxDiffY = diffY;
                    }
                }
            }
            
            if(foundXDiff)
            {
                int nPxl = floor((maxDiffX/pixelXRes)+0.5);
                if(nPxl > 0)
                {
                    (*width) = (*width) - nPxl;
                }
            }
            
            if(foundYDiff)
            {
                int nPxl = floor((maxDiffY/pixelYResPos)+0.5);
                if(nPxl > 0)
                {
                    (*height) = (*height) - nPxl;
                }
            }
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
        
        delete[] xBlockSize;
        delete[] yBlockSize;
	}
	
	void RSGISImageUtils::getImageOverlap(GDALDataset **datasets, int numDS, int *width, int *height, geos::geom::Envelope *env) throw(RSGISImageBandException)
	{
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		const char *proj = NULL;
		bool first = true;
		
		try
		{
			
			// Calculate Image Overlap.
			for(int i = 0; i < numDS; i++)
			{
				if(first)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);

					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if((this->closeResTest(pixelXRes, transformations[i][1]) == false) | (this->closeResTest(pixelYRes, transformations[i][5]) == false))
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(std::string(datasets[i]->GetProjectionRef()) != std::string(proj))
					{
						throw RSGISImageBandException("Not all image bands have the same projection..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
				}
			}
			
			if(maxX - minX <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the Y axis");
			}
			
			// Trim to the envelope
			if(minX > env->getMinX())
			{
				throw RSGISImageBandException("Envelope does not fit within the image overlap (MinX)");
			}
			
			if(minY > env->getMinY())
			{
				throw RSGISImageBandException("Envelope does not fit within the image overlap (MinY)");
			}
			
			if(maxX < env->getMaxX())
			{
				throw RSGISImageBandException("Envelope does not fit within the image overlap (MaxX)");
			}
			
			if(maxY < env->getMaxY())
			{
				throw RSGISImageBandException("Envelope does not fit within the image overlap (MaxY)");
			}
			
			minX = env->getMinX();
			maxX = env->getMaxX();
			minY = env->getMinY();
			maxY = env->getMaxY();
			
			*width = floor(((maxX - minX)/pixelXRes)+0.5);
			*height = floor(((maxY - minY)/pixelYResPos)+0.5);
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
    
    void RSGISImageUtils::getImageOverlap(GDALDataset **datasets, int numDS, geos::geom::Envelope *env) throw(RSGISImageBandException)
    {
        double **transformations = new double*[numDS];
        int *xSize = new int[numDS];
        int *ySize = new int[numDS];
        for(int i = 0; i < numDS; i++)
        {
            transformations[i] = new double[6];
            datasets[i]->GetGeoTransform(transformations[i]);
            xSize[i] = datasets[i]->GetRasterXSize();
            ySize[i] = datasets[i]->GetRasterYSize();
        }
        double rotateX = 0;
        double rotateY = 0;
        double minX = 0;
        double maxX = 0;
        double tmpMaxX = 0;
        double minY = 0;
        double tmpMinY = 0;
        double maxY = 0;
        const char *proj = NULL;
        bool first = true;
        
        try
        {
            
            // Calculate Image Overlap.
            for(int i = 0; i < numDS; i++)
            {
                if(first)
                {
                    rotateX = transformations[i][2];
                    rotateY = transformations[i][4];
                    
                    if(transformations[i][5] < 0)
                    {
                        transformations[i][5] = transformations[i][5] * (-1);
                    }
   
                    minX = transformations[i][0];
                    maxY = transformations[i][3];
                    
                    maxX = minX + (xSize[i] * transformations[i][1]);
                    minY = maxY - (ySize[i] * transformations[i][5]);
                    
                    proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
                    
                    first = false;
                }
                else
                {
                    if(std::string(datasets[i]->GetProjectionRef()) != std::string(proj))
                    {
                        std::cerr << "WARNING: \'" << datasets[i]->GetFileList()[0] << "\' does not have the same projection...\n";
                    }
                    
                    if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
                    {
                        throw RSGISImageBandException("Not all image bands have the same rotation..");
                    }
                    
                    if(transformations[i][0] > minX)
                    {
                        minX = transformations[i][0];
                    }
                    
                    if(transformations[i][3] < maxY)
                    {
                        maxY = transformations[i][3];
                    }
                    
                    if(transformations[i][5] < 0)
                    {
                        transformations[i][5] = transformations[i][5] * (-1);
                    }
                    
                    tmpMaxX = transformations[i][0] + (xSize[i] * transformations[i][1]);
                    tmpMinY = transformations[i][3] - (ySize[i] * transformations[i][5]);
                    
                    if(tmpMaxX < maxX)
                    {
                        maxX = tmpMaxX;
                    }
                    
                    if(tmpMinY > minY)
                    {
                        minY = tmpMinY;
                    }
                }
            }
            
            if(maxX - minX <= 0)
            {
                std::cout << "X: [" << minX << ", " << maxX << "]\n";
                throw RSGISImageBandException("Images do not overlap in the X axis");
            }
            
            if(maxY - minY <= 0)
            {
                std::cout << "Y: [" << minY << ", " << maxY << "]\n";
                throw RSGISImageBandException("Images do not overlap in the Y axis");
            }
            
            env->init(minX, maxX, minY, maxY);
        }
        catch(RSGISImageBandException& e)
        {
            if(transformations != NULL)
            {
                for(int i = 0; i < numDS; i++)
                {
                    delete[] transformations[i];
                }
                delete[] transformations;
            }
            if(xSize != NULL)
            {
                delete[] xSize;
            }
            if(ySize != NULL)
            {
                delete[] ySize;
            }
            throw e;
        }
        
        if(transformations != NULL)
        {
            for(int i = 0; i < numDS; i++)
            {
                delete[] transformations[i];
            }
            delete[] transformations;
        }
        if(xSize != NULL)
        {
            delete[] xSize;
        }
        if(ySize != NULL)
        {
            delete[] ySize;
        }
    }
	
	void RSGISImageUtils::getImagesExtent(GDALDataset **datasets, int numDS, int *width, int *height, double *gdalTransform) throw(RSGISImageBandException)
	{
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		const char *proj = NULL;
		bool first = true;
		
		try
		{
			for(int i = 0; i < numDS; i++)
			{
				if(first)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if((this->closeResTest(pixelXRes, transformations[i][1]) == false) | (this->closeResTest(pixelYRes, transformations[i][5]) == false))
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(std::string(datasets[i]->GetProjectionRef()) != std::string(proj))
					{
						throw RSGISImageBandException("Not all image bands have the same projection..");
					}
					
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(transformations[i][0] < minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] > maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX > maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY < minY)
					{
						minY = tmpMinY;
					}
				}
			}
			
			gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
			
			*width = floor(((maxX - minX)/pixelXRes)+0.5);
			*height = floor(((maxY - minY)/pixelYResPos)+0.5);
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
	
    void RSGISImageUtils::getImagesExtent(std::string *inputImages, int numDS, int *width, int *height, double *gdalTransform) throw(RSGISImageBandException)
    {
        double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
        GDALDataset *dataset = NULL;
		for(int i = 0; i < numDS; i++)
		{
            dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImages[i];
                throw RSGISImageException(message.c_str());
            }
            
			transformations[i] = new double[6];
			dataset->GetGeoTransform(transformations[i]);
			xSize[i] = dataset->GetRasterXSize();
			ySize[i] = dataset->GetRasterYSize();
			
            GDALClose(dataset);
		}
        
        double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		std::string proj = "";
		
		try
		{
			for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
				if(i == 0)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = std::string(dataset->GetProjectionRef()); // Get projection of first band in image
				}
				else
				{
					if((this->closeResTest(pixelXRes, transformations[i][1]) == false) | (this->closeResTest(pixelYRes, transformations[i][5]) == false))
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(std::string(dataset->GetProjectionRef()) != proj)
					{
                        std::cout << "First: (" << i << ")" << proj << std::endl;
                        std::cout << "Dataset: (" << i << ")" << std::string(dataset->GetProjectionRef()) << std::endl;
						throw RSGISImageBandException("Not all image bands have the same projection..");
					}
					
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(transformations[i][0] < minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] > maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX > maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY < minY)
					{
						minY = tmpMinY;
					}
				}
                GDALClose(dataset);
			}
			
			gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
			
			*width = floor(((maxX - minX)/pixelXRes)+0.5);
			*height = floor(((maxY - minY)/pixelYResPos)+0.5);
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
    }
    
    void RSGISImageUtils::getImagesExtent(std::vector<std::string> inputImages, int *width, int *height, double *gdalTransform) throw(RSGISImageBandException)
    {
        int numDS = inputImages.size();
        double **transformations = new double*[numDS];
        int *xSize = new int[numDS];
        int *ySize = new int[numDS];
        GDALDataset *dataset = NULL;
        for(int i = 0; i < numDS; i++)
        {
            dataset = (GDALDataset *) GDALOpenShared(inputImages.at(i).c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImages.at(i);
                throw RSGISImageException(message.c_str());
            }
            
            transformations[i] = new double[6];
            dataset->GetGeoTransform(transformations[i]);
            xSize[i] = dataset->GetRasterXSize();
            ySize[i] = dataset->GetRasterYSize();
            
            GDALClose(dataset);
        }
        
        double rotateX = 0;
        double rotateY = 0;
        double pixelXRes = 0;
        double pixelYRes = 0;
        double pixelYResPos = 0;
        double minX = 0;
        double maxX = 0;
        double tmpMaxX = 0;
        double minY = 0;
        double tmpMinY = 0;
        double maxY = 0;
        std::string proj = "";
        
        try
        {
            for(int i = 0; i < numDS; i++)
            {
                dataset = (GDALDataset *) GDALOpenShared(inputImages.at(i).c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages.at(i);
                    throw RSGISImageException(message.c_str());
                }
                
                if(i == 0)
                {
                    pixelXRes = transformations[i][1];
                    pixelYRes = transformations[i][5];
                    
                    rotateX = transformations[i][2];
                    rotateY = transformations[i][4];
                    
                    if(pixelYRes < 0)
                    {
                        pixelYResPos = pixelYRes * (-1);
                    }
                    else
                    {
                        pixelYResPos = pixelYRes;
                    }
                    
                    minX = transformations[i][0];
                    maxY = transformations[i][3];
                    
                    maxX = minX + (xSize[i] * pixelXRes);
                    minY = maxY - (ySize[i] * pixelYResPos);
                    
                    proj = std::string(dataset->GetProjectionRef()); // Get projection of first band in image
                }
                else
                {
                    if((this->closeResTest(pixelXRes, transformations[i][1]) == false) | (this->closeResTest(pixelYRes, transformations[i][5]) == false))
                    {
                        throw RSGISImageBandException("Not all image bands have the same resolution..");
                    }
                    
                    if(std::string(dataset->GetProjectionRef()) != proj)
                    {
                        std::cout << "First: (" << i << ")" << proj << std::endl;
                        std::cout << "Dataset: (" << i << ")" << std::string(dataset->GetProjectionRef()) << std::endl;
                        throw RSGISImageBandException("Not all image bands have the same projection..");
                    }
                    
                    
                    if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
                    {
                        throw RSGISImageBandException("Not all image bands have the same rotation..");
                    }
                    
                    if(transformations[i][0] < minX)
                    {
                        minX = transformations[i][0];
                    }
                    
                    if(transformations[i][3] > maxY)
                    {
                        maxY = transformations[i][3];
                    }
                    
                    tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
                    tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
                    
                    if(tmpMaxX > maxX)
                    {
                        maxX = tmpMaxX;
                    }
                    
                    if(tmpMinY < minY)
                    {
                        minY = tmpMinY;
                    }
                }
                GDALClose(dataset);
            }
            
            gdalTransform[0] = minX;
            gdalTransform[1] = pixelXRes;
            gdalTransform[2] = rotateX;
            gdalTransform[3] = maxY;
            gdalTransform[4] = rotateY;
            gdalTransform[5] = pixelYRes;
            
            *width = floor(((maxX - minX)/pixelXRes)+0.5);
            *height = floor(((maxY - minY)/pixelYResPos)+0.5);
        }
        catch(RSGISImageBandException& e)
        {
            if(transformations != NULL)
            {
                for(int i = 0; i < numDS; i++)
                {
                    delete[] transformations[i];
                }
                delete[] transformations;
            }
            if(xSize != NULL)
            {
                delete[] xSize;
            }
            if(ySize != NULL)
            {
                delete[] ySize;
            }
            throw e;
        }
        
        if(transformations != NULL)
        {
            for(int i = 0; i < numDS; i++)
            {
                delete[] transformations[i];
            }
            delete[] transformations;
        }
        if(xSize != NULL)
        {
            delete[] xSize;
        }
        if(ySize != NULL)
        {
            delete[] ySize;
        }
    }
    
    void RSGISImageUtils::getImagePixelOverlaps(GDALDataset **datasets, int numDS, int **dsOffsets, unsigned int *width, unsigned int *height) throw(RSGISImageBandException)
    {
        std::cout.precision(12);
        double **transformations = new double*[numDS];
        int *xSize = new int[numDS];
        int *ySize = new int[numDS];
        int *xBlockSize = new int[numDS];
        int *yBlockSize = new int[numDS];
        for(int i = 0; i < numDS; i++)
        {
            transformations[i] = new double[6];
            datasets[i]->GetGeoTransform(transformations[i]);
            xSize[i] = datasets[i]->GetRasterXSize();
            ySize[i] = datasets[i]->GetRasterYSize();
            datasets[i]->GetRasterBand(1)->GetBlockSize(&xBlockSize[i], &yBlockSize[i]);
        }
        double rotateX = 0;
        double rotateY = 0;
        double pixelXRes = 0;
        double pixelYRes = 0;
        double pixelYResPos = 0;
        double minX = 0;
        double maxX = 0;
        double tmpMaxX = 0;
        double minY = 0;
        double tmpMinY = 0;
        double maxY = 0;
        bool first = true;
        const char *proj = NULL;
        double *gdalTransform = new double[6];
        
        try
        {
            for(int i = 0; i < numDS; i++)
            {
                if(transformations[i] == NULL)
                {
                    throw RSGISImageBandException("No projection transformation has been provided..");
                }
                
                if(first)
                {
                    pixelXRes = transformations[i][1];
                    pixelYRes = transformations[i][5];
                    
                    rotateX = transformations[i][2];
                    rotateY = transformations[i][4];
                    
                    if(pixelYRes < 0)
                    {
                        pixelYResPos = pixelYRes * (-1);
                    }
                    else
                    {
                        pixelYResPos = pixelYRes;
                    }
                    
                    minX = transformations[i][0];
                    maxY = transformations[i][3];
                    
                    maxX = minX + (xSize[i] * pixelXRes);
                    minY = maxY - (ySize[i] * pixelYResPos);
                    
                    proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
                    
                    first = false;
                }
                else
                {
                    if((this->closeResTest(pixelXRes, transformations[i][1]) == false) | (this->closeResTest(pixelYRes, transformations[i][5]) == false))
                    {
                        throw RSGISImageBandException("Not all image bands have the same resolution..");
                    }
                    
                    if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
                    {
                        throw RSGISImageBandException("Not all image bands have the same rotation..");
                    }
                    
                    if(std::string(datasets[i]->GetProjectionRef()) != std::string(proj))
                    {
                        std::cout << "Not all image bands have the same projection" << std::endl;
                    }
                        
                    if(transformations[i][0] > minX)
                    {
                        minX = transformations[i][0];
                    }
                    
                    if(transformations[i][3] < maxY)
                    {
                        maxY = transformations[i][3];
                    }
                    
                    tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
                    tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
                    
                    if(tmpMaxX < maxX)
                    {
                        maxX = tmpMaxX;
                    }
                    
                    if(tmpMinY > minY)
                    {
                        minY = tmpMinY;
                    }
                    
                }
            }
            
            if(maxX - minX <= 0)
            {
                throw RSGISImageBandException("Images do not overlap in the X axis");
            }
            
            if(maxY - minY <= 0)
            {
                throw RSGISImageBandException("Images do not overlap in the Y axis");
            }
        
            
            gdalTransform[0] = minX;
            gdalTransform[1] = pixelXRes;
            gdalTransform[2] = rotateX;
            gdalTransform[3] = maxY;
            gdalTransform[4] = rotateY;
            gdalTransform[5] = pixelYRes;
            
            *width = floor(((maxX - minX)/pixelXRes)+0.5);
            *height = floor(((maxY - minY)/pixelYResPos)+0.5);
            
            double diffX = 0;
            double diffY = 0;
            
            for(int i = 0; i < numDS; i++)
            {
                diffX = minX - transformations[i][0];
                diffY = transformations[i][3] - maxY;
                
                if(!((diffX > -0.0001) & (diffX < 0.0001)))
                {
                    dsOffsets[i][0] = floor((diffX/pixelXRes)+0.5);
                }
                else
                {
                    dsOffsets[i][0] = 0;
                }
                
                if(!((diffY > -0.0001) & (diffY < 0.0001)))
                {
                    dsOffsets[i][1] = floor((diffY/pixelYResPos)+0.5);
                }
                else
                {
                    dsOffsets[i][1] = 0;
                }
            }
            
            double tmpMinX = 0;
            double tmpMaxY = 0;
            tmpMaxX = 0;
            tmpMinY = 0;
            
            double maxDiffX = 0;
            double maxDiffY = 0;
            bool foundXDiff = false;
            bool foundYDiff = false;
            
            for(int i = 0; i < numDS; i++)
            {
                tmpMinX = transformations[i][0] + (dsOffsets[i][0] * pixelXRes);
                tmpMaxY = transformations[i][3] - (dsOffsets[i][1] * pixelYResPos);
                
                tmpMaxX = tmpMinX + ((*width)*pixelXRes);
                tmpMinY = tmpMaxY - ((*height)*pixelYResPos);
                
                if(tmpMaxX > maxX)
                {
                    diffX = (tmpMaxX - maxX);
                    if(!foundXDiff)
                    {
                        maxDiffX = diffX;
                        foundXDiff = true;
                    }
                    else if(diffX > maxDiffX)
                    {
                        maxDiffX = diffX;
                    }
                }
                
                if(tmpMinY < minY)
                {
                    diffY = (minY - tmpMinY);
                    if(!foundYDiff)
                    {
                        maxDiffY = diffY;
                        foundYDiff = true;
                    }
                    else if(diffY > maxDiffY)
                    {
                        maxDiffY = diffY;
                    }
                }
            }
            
            if(foundXDiff)
            {
                int nPxl = floor((maxDiffX/pixelXRes)+0.5);
                if(nPxl > 0)
                {
                    width = width - nPxl;
                }
            }
            
            if(foundYDiff)
            {
                int nPxl = floor((maxDiffY/pixelYResPos)+0.5);
                if(nPxl > 0)
                {
                    height = height - nPxl;
                }
            }
            
            for(int i = 0; i < numDS; i++)
            {
                dsOffsets[i][2] = dsOffsets[i][0] + *width;
                dsOffsets[i][3] = dsOffsets[i][1] + *height;
            }
            
        }
        catch(RSGISImageBandException& e)
        {
            if(transformations != NULL)
            {
                for(int i = 0; i < numDS; i++)
                {
                    delete[] transformations[i];
                }
                delete[] transformations;
            }
            if(xSize != NULL)
            {
                delete[] xSize;
            }
            if(ySize != NULL)
            {
                delete[] ySize;
            }
            if(gdalTransform != NULL)
            {
                delete[] gdalTransform;
            }
            throw e;
        }
        
        if(transformations != NULL)
        {
            for(int i = 0; i < numDS; i++)
            {
                delete[] transformations[i];
            }
            delete[] transformations;
        }
        if(xSize != NULL)
        {
            delete[] xSize;
        }
        if(ySize != NULL)
        {
            delete[] ySize;
        }
        if(gdalTransform != NULL)
        {
            delete[] gdalTransform;
        }
    }
    
    OGREnvelope* RSGISImageUtils::getSpatialExtent(GDALDataset *dataset) throw(RSGISImageBandException)
    {
        OGREnvelope *env = new OGREnvelope();
        try
        {
            double *gdalTranslation = new double[6];
            dataset->GetGeoTransform(gdalTranslation);
            
            double minX = gdalTranslation[0];
            double maxY = gdalTranslation[3];
            double maxX = minX + (dataset->GetRasterXSize() * gdalTranslation[1]);
            double minY = maxY + (dataset->GetRasterYSize() * gdalTranslation[5]);
            
            env->MinX = minX;
            env->MaxX = maxX;
            env->MinY = minY;
            env->MaxY = maxY;
        }
        catch(RSGISImageBandException &e)
        {
            delete env;
            throw e;
        }
        
        return env;
    }
    
    
    bool RSGISImageUtils::doImageSpatAndExtMatch(GDALDataset **datasets, int numDS) throw(RSGISImageBandException)
    {
        bool match = true;
        try
        {
            double *gdalTransRef = new double[6];
            datasets[0]->GetGeoTransform(gdalTransRef);
            int heightRef = datasets[0]->GetRasterYSize();
            int widthRef = datasets[0]->GetRasterXSize();
            
            double *gdalTransSec = new double[6];
            int heightSec = 0;
            int widthSec = 0;

            for(int i = 1; i < numDS; i++)
            {
                datasets[i]->GetGeoTransform(gdalTransSec);
                heightSec = datasets[i]->GetRasterYSize();
                widthSec = datasets[i]->GetRasterXSize();
                
                if(heightRef != heightSec)
                {
                    match = false;
                    break;
                }
                
                if(widthRef != widthSec)
                {
                    match = false;
                    break;
                }
                
                for(int j = 0; j < 6; ++j)
                {
                    if(gdalTransRef[j] != gdalTransSec[j])
                    {
                        match = false;
                        break;
                    }
                }
                
                if(!match)
                {
                    break;
                }
            }
            
            delete[] gdalTransRef;
            delete[] gdalTransSec;
            
        }
        catch (RSGISImageBandException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw RSGISImageBandException(e.what());
        }
        
        return match;
    }
    
    
    
	void RSGISImageUtils::exportImageToTextCol(GDALDataset *image, int band, std::string outputText)throw(RSGISImageBandException, RSGISOutputStreamException)
	{
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		float *inputData = NULL;
		
		GDALRasterBand *inputRasterBand = NULL;
		
		int width = 0;
		int height = 0;
		int numberOfBands = 0;
		
		try
		{
			width = image->GetRasterXSize();
			height = image->GetRasterYSize();
			numberOfBands = image->GetRasterCount();
			
			if(band == 0)
			{
				band++;
			}
			else if(band > numberOfBands)
			{
				throw RSGISImageBandException("There are not enough bands within the image.");
			}
			
			// Create textfile
			std::ofstream outFile(outputText.c_str(), std::ios::out | std::ios::trunc);
			
			if(!outFile.is_open())
			{
				throw RSGISOutputStreamException("Could not open text file.");
			}
			
			// Allocate memory
			inputData = (float *) CPLMalloc(sizeof(float)*width);
			
			// Get Raster band
			inputRasterBand = image->GetRasterBand(band);
			
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				if((i % feedback) == 0)
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				inputRasterBand->RasterIO(GF_Read, 0, i, width, 1, inputData, width, 1, GDT_Float32, 0, 0);
				
				for(int j = 0; j < width; j++)
				{
					// Output to text file...
					outFile << inputData[j] << std::endl;
				}
			}
			std::cout << "..100 Complete.\n";
		}
		catch(RSGISImageBandException e)
		{
			
			throw e;
		}
		catch(RSGISOutputStreamException e)
		{
			
			throw e;
		}
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		if(inputData != NULL)
		{
			delete inputData;
		}
	}
	
	GDALDataset* RSGISImageUtils::createBlankImage(std::string imageFile, double *transformation, int xSize, int ySize, int numBands, std::string projection, float value, std::string gdalFormat, GDALDataType imgDataType) throw(RSGISImageException, RSGISImageBandException)
	{
		GDALAllRegister();
		GDALDriver *poDriver = NULL;
		GDALDataset *outputImage = NULL;
        GDALRasterBand **outputRasterBands = NULL;
        
		float **imgData = NULL;
		
		if(transformation[1] <= 0)
		{
			throw RSGISImageException("The resolution of the image needs to be > 0.");
		}
		if(numBands <= 0)
		{
			throw RSGISImageException("The number of bands needs to be > 0.");
		}
		
		try
		{
			if(xSize <= 0)
			{
				throw RSGISImageException("The image needs to have a xSize > 0");
			}
			if(ySize <= 0)
			{
				throw RSGISImageException("The image needs to have a ySize > 0");
			}
			
			poDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(poDriver == NULL)
			{
				throw RSGISImageException("Image driver is not available.");
			}
			
			// Create new file. 
			// Set unsupported options to NULL
			outputImage = poDriver->Create(imageFile.c_str(), xSize, ySize, numBands, imgDataType, NULL);
			
			if(outputImage == NULL)
			{
				throw RSGISImageException("Image could not be created.");
			}
			outputImage->SetGeoTransform(transformation);
			outputImage->SetProjection(projection.c_str());
			
            this->assignValGDALDataset(outputImage, value);
		}
		catch(RSGISImageBandException e)
		{
			if(transformation != NULL)
			{
				delete transformation;
			}
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
            
            if(imgData != NULL)
            {
                for(int i = 0; i < numBands; i++)
                {
                    if(imgData[i] != NULL)
                    {
                        CPLFree(imgData[i]);
                    }
                }
                delete[] imgData;
            }
			throw e;
		}
		catch(RSGISImageException e)
		{
			if(transformation != NULL)
			{
				delete transformation;
			}
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
            
            if(imgData != NULL)
            {
                for(int i = 0; i < numBands; i++)
                {
                    if(imgData[i] != NULL)
                    {
                        CPLFree(imgData[i]);
                    }
                }
                delete[] imgData;
            }
			throw e;
		}
        
        if(outputRasterBands != NULL)
        {
            delete[] outputRasterBands;
        }
		
		if(imgData != NULL)
		{
			for(int i = 0; i < numBands; i++)
			{
				if(imgData[i] != NULL)
				{
					CPLFree(imgData[i]);
				}
			}
			delete[] imgData;
		}
		
		
		return outputImage;
	}
    
    GDALDataset* RSGISImageUtils::createBlankImage(std::string imageFile, double *transformation, int xSize, int ySize, int numBands, std::string projection, float value, std::vector<std::string> bandNames, std::string gdalFormat, GDALDataType imgDataType) throw(RSGISImageException, RSGISImageBandException)
	{
		GDALAllRegister();
		GDALDriver *poDriver = NULL;
		GDALDataset *outputImage = NULL;
        GDALRasterBand **outputRasterBands = NULL;
        
		float **imgData = NULL;
		
		if(transformation[1] <= 0)
		{
			throw RSGISImageException("The resolution of the image needs to be > 0.");
		}
		if(numBands <= 0)
		{
			throw RSGISImageException("The number of bands needs to be > 0.");
		}
		
		try
		{
			if(xSize <= 0)
			{
				throw RSGISImageException("The image needs to have a xSize > 0");
			}
			if(ySize <= 0)
			{
				throw RSGISImageException("The image needs to have a ySize > 0");
			}
			
			poDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(poDriver == NULL)
			{
				throw RSGISImageException("Image driver is not available.");
			}
			
			// Create new file.
			// Set unsupported options to NULL
			outputImage = poDriver->Create(imageFile.c_str(), xSize, ySize, numBands, imgDataType, NULL);
			
			if(outputImage == NULL)
			{
				throw RSGISImageException("Image could not be created.");
			}
			outputImage->SetGeoTransform(transformation);
			outputImage->SetProjection(projection.c_str());
			
            this->assignValGDALDataset(outputImage, value);
		}
		catch(RSGISImageBandException e)
		{
			if(transformation != NULL)
			{
				delete transformation;
			}
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
            
            if(imgData != NULL)
            {
                for(int i = 0; i < numBands; i++)
                {
                    if(imgData[i] != NULL)
                    {
                        CPLFree(imgData[i]);
                    }
                }
                delete[] imgData;
            }
			throw e;
		}
		catch(RSGISImageException e)
		{
			if(transformation != NULL)
			{
				delete transformation;
			}
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
            
            if(imgData != NULL)
            {
                for(int i = 0; i < numBands; i++)
                {
                    if(imgData[i] != NULL)
                    {
                        CPLFree(imgData[i]);
                    }
                }
                delete[] imgData;
            }
			throw e;
		}
        
        if(outputRasterBands != NULL)
        {
            delete[] outputRasterBands;
        }
		
		if(imgData != NULL)
		{
			for(int i = 0; i < numBands; i++)
			{
				if(imgData[i] != NULL)
				{
					CPLFree(imgData[i]);
				}
			}
			delete[] imgData;
		}
		
		
		return outputImage;
	}
	
	GDALDataset* RSGISImageUtils::createBlankImage(std::string imageFile, geos::geom::Envelope extent, double resolution, int numBands, std::string projection, float value, std::string gdalFormat, GDALDataType imgDataType) throw(RSGISImageException, RSGISImageBandException)
	{
		GDALAllRegister();
		GDALDriver *poDriver = NULL;
		GDALDataset *outputImage = NULL;
		
		float *imgData = NULL;
		double *transformation = NULL;
		int xSize = 0;
		int ySize = 0;
		
		if(resolution <= 0)
		{
			throw RSGISImageException("The resolution of the image needs to be > 0.");
		}
		if(numBands <= 0)
		{
			throw RSGISImageException("The number of bands needs to be > 0.");
		}
		
		try
		{
			xSize = ceil((extent.getMaxX() - extent.getMinX())/resolution);
			ySize = ceil((extent.getMaxY() - extent.getMinY())/resolution);
			
			if(xSize <= 0)
			{
				throw RSGISImageException("The image needs to have a xSize > 0");
			}
			if(ySize <= 0)
			{
				throw RSGISImageException("The image needs to have a ySize > 0");
			}
			
			transformation = new double[6];
			transformation[0] = extent.getMinX();
			transformation[1] = resolution;
			transformation[2] = 0;
			transformation[3] = extent.getMaxY();
			transformation[4] = 0;
			transformation[5] = resolution * (-1);
			
			poDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(poDriver == NULL)
			{
				throw RSGISImageException("Image driver is not available.");
			}
			
			// Create new file.
			outputImage = poDriver->Create(imageFile.c_str(), xSize, ySize, numBands, imgDataType, NULL);
			
			if(outputImage == NULL)
			{
				throw RSGISImageException("Image could not be created.");
			}
            
            std::cout << "Projection = " << projection << std::endl;
            
			outputImage->SetGeoTransform(transformation);
			outputImage->SetProjection(projection.c_str());
			
			this->assignValGDALDataset(outputImage, value);
		}
		catch(RSGISImageBandException e)
		{
			if(transformation != NULL)
			{
				delete transformation;
			}
			if(imgData != NULL)
			{
				delete imgData;
			}
			throw e;
		}
		catch(RSGISImageException e)
		{
			if(transformation != NULL)
			{
				delete transformation;
			}
			if(imgData != NULL)
			{
				delete imgData;
			}
			throw e;
		}
		
		if(transformation != NULL)
		{
			delete transformation;
		}
		if(imgData != NULL)
		{
			delete imgData;
		}
		
		return outputImage;
	}
	
	void RSGISImageUtils::exportImageBands(std::string imageFile, std::string outputFilebase, std::string format) throw(RSGISImageException, RSGISImageBandException)
	{
		GDALAllRegister();
		GDALDataset *dataset = NULL;
		GDALDriver *gdalDriver = NULL;
		GDALRasterBand *inputImgBand = NULL;
		GDALRasterBand *outputImgBand = NULL;
		GDALDataset *outputImage = NULL;
		std::string outImageFile = "";
		std::stringstream *outStrStream;
		double *transformation = new double[6];
		float *imageData = NULL;
		char **gdalDriverMetaInfo;
		
		try
		{
			std::cout << imageFile << std::endl;
			dataset = (GDALDataset *) GDALOpenShared(imageFile.c_str(), GA_ReadOnly);
			if(dataset == NULL)
			{
				std::string message = std::string("Could not open image ") + imageFile;
				throw RSGISImageException(message.c_str());
			}
			
			gdalDriver = GetGDALDriverManager()->GetDriverByName(format.c_str());
			if(gdalDriver == NULL)
			{
				std::string message = format + std::string(" image driver is not available.");
				throw RSGISImageException(message.c_str());
			}
			
			gdalDriverMetaInfo = gdalDriver->GetMetadata();
			if(CSLFetchBoolean(gdalDriverMetaInfo, GDAL_DCAP_CREATE, FALSE ))
			{
				std::cout << "Driver for " << format << " supports CreateCopy\n";
			}
			else
			{
				throw RSGISImageException("Image driver does not support create. Therefore cannot create file of this type.");
			}
			
			
			int numBands = dataset->GetRasterCount();
			int xSize = dataset->GetRasterXSize();
			int ySize = dataset->GetRasterYSize();
			dataset->GetGeoTransform(transformation);
			imageData = (float *) CPLMalloc(sizeof(float)*xSize);
			
			for(int i = 1; i <= numBands; i++)
			{
				std::cout << "Outputting band " << i << " of " << numBands << std::endl;
				outStrStream = new std::stringstream();
				*outStrStream << outputFilebase << "_b" << i << ".tif";
				outImageFile = outStrStream->str();
				std::cout << "File: " << outImageFile << std::endl;

				outputImage = gdalDriver->Create(outImageFile.c_str(), xSize, ySize, 1, GDT_Float32, NULL);
				
				if(outputImage == NULL)
				{
					throw RSGISImageException("Image could not be created.");
				}
				outputImage->SetGeoTransform(transformation);
				outputImage->SetProjection(dataset->GetProjectionRef());
				
				inputImgBand = dataset->GetRasterBand(i);
				outputImgBand = outputImage->GetRasterBand(1);
				for(int j = 0; j < ySize; j++)
				{
					inputImgBand->RasterIO(GF_Read, 0, j, xSize, 1, imageData, xSize, 1, GDT_Float32, 0, 0);
					outputImgBand->RasterIO(GF_Write, 0, j, xSize, 1, imageData, xSize, 1, GDT_Float32, 0, 0);
				}
				GDALClose(outputImage);
				delete outStrStream;
			}
			GDALClose(dataset);
		}
		catch(RSGISImageException e)
		{
			
			throw e;
		}
	}
	
	void RSGISImageUtils::exportImageStack(std::string *inputImages, std::string *outputImages, std::string outputFormat, int numImages)  throw(RSGISImageException, RSGISImageBandException)
	{
		GDALAllRegister();
		GDALDataset **inDatasets = NULL;
		GDALDriver *gdalDriver = NULL;
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand *inputRasterBand = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		
		float *data = NULL;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numImages];
		for(int i = 0; i < numImages; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int stackHeight = 0;
		int stackWidth = 0;
		int numOutBands = 0;
		
		try
		{
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(outputFormat.c_str());
			if(gdalDriver == NULL)
			{
				std::string message = std::string("Driver for ") + outputFormat + std::string(" does not exist\n");
				throw RSGISImageException(message.c_str());
			}
			
			inDatasets = new GDALDataset*[numImages];
			for(int i = 0; i < numImages; i++)
			{
				std::cout << inputImages[i] << std::endl;
				inDatasets[i] = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
				if(inDatasets[i] == NULL)
				{
					std::string message = std::string("Could not open image ") + inputImages[i];
					throw RSGISImageException(message.c_str());
				}
			}
			
			// Find image overlap
			this->getImageOverlap(inDatasets, numImages, dsOffsets, &stackWidth, &stackHeight, gdalTranslation);
			
			std::cout << "Stack Height = " << stackHeight << std::endl;
			std::cout << "Stack Width = " << stackWidth << std::endl;
			
			data = (float *) CPLMalloc(sizeof(float)*stackWidth);
			
			for(int i = 0; i < numImages; i++)
			{
				std::cout << "Converting image " << inputImages[i] << std::endl;
				numOutBands = inDatasets[i]->GetRasterCount();

				outputImageDS = gdalDriver->Create(outputImages[i].c_str(), stackWidth, stackHeight, numOutBands, GDT_Float32, NULL);
				
				outputImageDS->SetGeoTransform(gdalTranslation);
				outputImageDS->SetProjection(inDatasets[0]->GetProjectionRef());
				
				for(int n = 1; n <= numOutBands; n++)
				{
					std::cout << "Image Band " << n << " of " << numOutBands << std::endl;
					
					inputRasterBand = inDatasets[i]->GetRasterBand(n);
					outputRasterBand = outputImageDS->GetRasterBand(n);
					
					int feedback = stackHeight/10;
					int feedbackCounter = 0;
					std::cout << "Started" << std::flush;
					
					for(int m = 0; m < stackHeight; m++)
					{
						if((m % feedback) == 0)
						{
							std::cout << ".." << feedbackCounter << ".." << std::flush;
							feedbackCounter = feedbackCounter + 10;
						}
						inputRasterBand->RasterIO(GF_Read, dsOffsets[i][0], (dsOffsets[i][1]+m), stackWidth, 1, data, stackWidth, 1, GDT_Float32, 0, 0);						
						outputRasterBand->RasterIO(GF_Write, 0, m, stackWidth, 1, data, stackWidth, 1, GDT_Float32, 0, 0);
					}
					std::cout << " Complete.\n";
				}

				GDALClose(outputImageDS);
			}
			
			for(int i = 0; i < numImages; i++)
			{
				GDALClose(inDatasets[i]);
			}
			delete inDatasets;
			delete data;
			delete[] gdalTranslation;
			
		}
		catch(RSGISImageException e)
		{
			throw e;
		}
	}
	
	void RSGISImageUtils::exportImageStackWithMask(std::string *inputImages, std::string *outputImages, std::string imageMask, std::string outputFormat, int numImages, float maskValue)  throw(RSGISImageException, RSGISImageBandException)
	{
		GDALAllRegister();
		GDALDataset **inDatasets = NULL;
		GDALDriver *gdalDriver = NULL;
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand *imageMaskBand = NULL;
		GDALRasterBand *inputRasterBand = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		
		float *data = NULL;
		float *mask = NULL;
		double *gdalTranslation = new double[6];
		int **dsOffsets = NULL;
		int stackHeight = 0;
		int stackWidth = 0;
		int numOutBands = 0;
		numImages++; //include image mask
		
		try
		{
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(outputFormat.c_str());
			if(gdalDriver == NULL)
			{
				std::string message = std::string("Driver for ") + outputFormat + std::string(" does not exist\n");
				throw RSGISImageException(message.c_str());
			}
			
			inDatasets = new GDALDataset*[numImages];
			std::cout << imageMask << std::endl;
			inDatasets[0] = (GDALDataset *) GDALOpenShared(imageMask.c_str(), GA_ReadOnly);
			if(inDatasets[0] == NULL)
			{
				std::string message = std::string("Could not open image ") + imageMask;
				throw RSGISImageException(message.c_str());
			}
			for(int i = 1; i < numImages; i++)
			{
				std::cout << inputImages[i-1] << std::endl;
				inDatasets[i] = (GDALDataset *) GDALOpenShared(inputImages[i-1].c_str(), GA_ReadOnly);
				if(inDatasets[i] == NULL)
				{
					std::string message = std::string("Could not open image ") + inputImages[i-1];
					throw RSGISImageException(message.c_str());
				}
			}
						
			dsOffsets = new int*[numImages];
			for(int i = 0; i < numImages; i++)
			{
				dsOffsets[i] = new int[2];
			}
			
			// Find image overlap
			this->getImageOverlap(inDatasets, numImages, dsOffsets, &stackWidth, &stackHeight, gdalTranslation);
			
			std::cout << "Stack Height = " << stackHeight << std::endl;
			std::cout << "Stack Width = " << stackWidth << std::endl;
			
			data = (float *) CPLMalloc(sizeof(float)*stackWidth);
			mask = (float *) CPLMalloc(sizeof(float)*stackWidth);
			imageMaskBand = inDatasets[0]->GetRasterBand(1);
			for(int i = 1; i < numImages; i++)
			{
				std::cout << "Converting image " << inputImages[i-1] << std::endl;
				numOutBands = inDatasets[i]->GetRasterCount();
				outputImageDS = gdalDriver->Create(outputImages[i-1].c_str(), stackWidth, stackHeight, numOutBands, GDT_Float32, NULL);
				outputImageDS->SetGeoTransform(gdalTranslation);
				outputImageDS->SetProjection(inDatasets[0]->GetProjectionRef());
				
				for(int n = 1; n <= numOutBands; n++)
				{
					std::cout << "Image Band " << n << " of " << numOutBands << std::endl;
					inputRasterBand = inDatasets[i]->GetRasterBand(n);
					outputRasterBand = outputImageDS->GetRasterBand(n);
					
					int feedback = stackHeight/10;
					int feedbackCounter = 0;
					std::cout << "Started" << std::flush;
					
					for(int m = 0; m < stackHeight; m++)
					{
						if((m % feedback) == 0)
						{
							std::cout << ".." << feedbackCounter << ".." << std::flush;
							feedbackCounter = feedbackCounter + 10;
						}
						
						inputRasterBand->RasterIO(GF_Read, dsOffsets[i][0], (dsOffsets[i][1]+m), stackWidth, 1, data, stackWidth, 1, GDT_Float32, 0, 0);
						imageMaskBand->RasterIO(GF_Read, dsOffsets[0][0], (dsOffsets[0][1]+m), stackWidth, 1, mask, stackWidth, 1, GDT_Float32, 0, 0);
						
						for(int k = 0; k < stackWidth; k++)
						{
							if(mask[k] == 0)
							{
								data[k] = maskValue;
							}
						}
						
						outputRasterBand->RasterIO(GF_Write, 0, m, stackWidth, 1, data, stackWidth, 1, GDT_Float32, 0, 0);
					}
					std::cout << " Complete.\n";
				}
				GDALClose(outputImageDS);
			}
			for(int i = 0; i < numImages; i++)
			{
				GDALClose(inDatasets[i]);
			}
			delete inDatasets;
			delete data;
			delete[] gdalTranslation;
		}
		catch(RSGISImageException e)
		{
			throw e;
		}
	}
	
	void RSGISImageUtils::convertImageFileFormat(std::string inputImage, std::string outputImage, std::string outputImageFormat, bool projFromImage, std::string wktProjStr)
	{
		GDALAllRegister();
		GDALDataset *inDataset = NULL;
		GDALDriver *gdalDriver = NULL;
		GDALDataset *outDataset = NULL;
		
		GDALRasterBand *inputRasterBand = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		
		float *data = NULL;
		
		char **gdalDriverMetaInfo;
		int numOutBands = 0;
		int width = 0;
		int height = 0;
		
		try
		{
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(outputImageFormat.c_str());
			if(gdalDriver == NULL)
			{
				std::string message = std::string("Driver for ") + outputImageFormat + std::string(" does not exist\n");
				throw RSGISImageException(message.c_str());
			}
			
			gdalDriverMetaInfo = gdalDriver->GetMetadata();
			if(CSLFetchBoolean(gdalDriverMetaInfo, GDAL_DCAP_CREATECOPY, FALSE ))
			{
				std::cout << "Driver for " << outputImageFormat << " supports CreateCopy\n";
			}
			else
			{
				throw RSGISImageException("Image driver does not support image copy. Therefore cannot create file of this type.");
			}
			
			std::cout << "Openning image " << inputImage << std::endl;
			inDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
			if(inDataset == NULL)
			{
				std::string message = std::string("Could not open image ") + inputImage;
				throw RSGISImageException(message.c_str());
			}
			
			std::cout << "Creating image " << outputImage << std::endl;
			outDataset = gdalDriver->CreateCopy(outputImage.c_str(), inDataset, FALSE, NULL, NULL, NULL);
			if(outDataset == NULL)
			{
				std::string message = std::string("Could not open image ") + outputImage;
				throw RSGISImageException(outputImage.c_str());
			}
            
            if(!projFromImage)
            {
                outDataset->SetProjection(wktProjStr.c_str());
            }
			
			width = inDataset->GetRasterXSize();
			height = inDataset->GetRasterYSize();
			data = (float *) CPLMalloc(sizeof(float)*width);
			std::cout << "Image size [" << width << "," << height << "]\n";
			
			numOutBands = inDataset->GetRasterCount();
			for(int n = 1; n <= numOutBands; n++)
			{
				std::cout << "Image Band " << n << " of " << numOutBands << std::endl;
				inputRasterBand = inDataset->GetRasterBand(n);
				outputRasterBand = outDataset->GetRasterBand(n);
				
				int feedback = height/10;
				int feedbackCounter = 0;
				std::cout << "Started" << std::flush;
				
				for(int m = 0; m < height; m++)
				{
					if((m % feedback) == 0)
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
						feedbackCounter = feedbackCounter + 10;
					}
					
					inputRasterBand->RasterIO(GF_Read, 0, m, width, 1, data, width, 1, GDT_Float32, 0, 0);
					outputRasterBand->RasterIO(GF_Write, 0, m, width, 1, data, width, 1, GDT_Float32, 0, 0);
				}
				std::cout << " Complete.\n";
			}
			
			GDALClose(outDataset);
			GDALClose(inDataset);
			delete data;
		}
		catch(RSGISImageException e)
		{
			throw e;
		}
	}
	
	float** RSGISImageUtils::getImageDataBlock(GDALDataset *dataset, int *dsOffsets, unsigned int width, unsigned int height, unsigned int *numVals)
	{
		unsigned int numImageBands = dataset->GetRasterCount();
		*numVals = width*height;
		
		GDALRasterBand **rasterBands = new GDALRasterBand*[numImageBands];
		float **imgData = new float*[numImageBands];
		float **outVals = new float*[numImageBands];
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			outVals[i] = new float[*numVals];
			rasterBands[i] = dataset->GetRasterBand(i+1);
			imgData[i] = (float *) CPLMalloc(sizeof(float)*width);
		}
		
		unsigned int outValCounter = 0;
		for(unsigned int i = 0; i < height; ++i)
		{
			for(unsigned k = 0; k < numImageBands; ++k)
			{
				rasterBands[k]->RasterIO(GF_Read, dsOffsets[0], dsOffsets[1]+i, width, 1, imgData[k], width, 1, GDT_Float32, 0, 0);
			}
			
			for(unsigned int j = 0; j < width; ++j)
			{
				for(unsigned k = 0; k < numImageBands; ++k)
				{
					outVals[k][outValCounter] = imgData[k][j];
				}
				++outValCounter;
			}
		}
		
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			CPLFree(imgData[i]);
		}
		delete[] imgData;
		delete[]rasterBands;
				
		return outVals;
	}
    
    std::vector<double>* RSGISImageUtils::getImageBandValues(GDALDataset *dataset, unsigned int band, bool noDataValDefined, float noDataVal)throw(RSGISImageException)
    {
        std::vector<double> *imgVals = new std::vector<double>();
        try
        {
            unsigned int width = dataset->GetRasterXSize();
            unsigned int height = dataset->GetRasterYSize();
            imgVals->reserve(width*height);
            GDALRasterBand *gdalBand = dataset->GetRasterBand(band);
            int blockSizeX = 0;
            int blockSizeY = 0;
            gdalBand->GetBlockSize(&blockSizeX, &blockSizeY);
            unsigned int bufSize = width*blockSizeY;
            float *imgData = new float[bufSize];
            
            int numBlocks = floor((float)height/(float)blockSizeY);
            unsigned int numRowsRemaining = height - (numBlocks*blockSizeY);
            
            int yOff = 0;
            for(unsigned int n = 0; n < numBlocks; ++n)
            {
                gdalBand->RasterIO(GF_Read, 0, yOff, width, blockSizeY, imgData, width, blockSizeY, GDT_Float32, 0, 0);
                
                for(unsigned int i = 0; i < bufSize; ++i)
                {
                    if(noDataValDefined && (imgData[i] != noDataVal))
                    {
                        imgVals->push_back(imgData[i]);
                    }
                }
                
                yOff += blockSizeY;
            }            
            
            if(numRowsRemaining > 0)
            {
                bufSize = width*numRowsRemaining;
                
                gdalBand->RasterIO(GF_Read, 0, yOff, width, numRowsRemaining, imgData, width, numRowsRemaining, GDT_Float32, 0, 0);
                
                for(unsigned int i = 0; i < bufSize; ++i)
                {
                    if(noDataValDefined && (imgData[i] != noDataVal))
                    {
                        imgVals->push_back(imgData[i]);
                    }
                }
            }
            
            delete[] imgData;
        }
        catch (RSGISImageException *e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        
        return imgVals;
    }
    
	
	void RSGISImageUtils::copyImageRemoveSpatialReference(std::string inputImage, std::string outputImage)throw(RSGISImageException)
	{
		GDALAllRegister();
		GDALDataset *inDataset = NULL;
		GDALDriver *gdalDriver = NULL;
		GDALDataset *outDataset = NULL;
		
		GDALRasterBand *inputRasterBand = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		
		float *data = NULL;
		
		int numOutBands = 0;
		int width = 0;
		int height = 0;
		double *transformation = NULL;
		
		try
		{
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(gdalDriver == NULL)
			{
				std::string message = std::string("Driver for ENVI does not exist\n");
				throw RSGISImageException(message.c_str());
			}
			
			
			std::cout << "Openning image " << inputImage << std::endl;
			inDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
			if(inDataset == NULL)
			{
				std::string message = std::string("Could not open image ") + inputImage;
				throw RSGISImageException(message.c_str());
			}
			
			numOutBands = inDataset->GetRasterCount();
			
			width = inDataset->GetRasterXSize();
			height = inDataset->GetRasterYSize();
			
                transformation = new double[6];
			transformation[0] = 0;
			transformation[1] = 1;
			transformation[2] = 0;
			transformation[3] = 0;
			transformation[4] = 0;
			transformation[5] = -1;
			
			std::cout << "Creating image " << outputImage << std::endl;
			outDataset = gdalDriver->Create(outputImage.c_str(), width, height, numOutBands, GDT_Float32, NULL);
			
			if(outDataset == NULL)
			{
				std::string message = std::string("Could not open image ") + outputImage;
				throw RSGISImageException(outputImage.c_str());
			}
			
			outDataset->SetGeoTransform(transformation);
			outDataset->SetProjection("");
			
			
			data = (float *) CPLMalloc(sizeof(float)*width);
			std::cout << "Image size [" << width << "," << height << "]\n";
			
			
			for(int n = 1; n <= numOutBands; n++)
			{
				std::cout << "Image Band " << n << " of " << numOutBands << std::endl;
				inputRasterBand = inDataset->GetRasterBand(n);
				outputRasterBand = outDataset->GetRasterBand(n);
				
				int feedback = height/10;
				int feedbackCounter = 0;
				std::cout << "Started" << std::flush;
				
				for(int m = 0; m < height; m++)
				{
					if((m % feedback) == 0)
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
						feedbackCounter = feedbackCounter + 10;
					}
					
					inputRasterBand->RasterIO(GF_Read, 0, m, width, 1, data, width, 1, GDT_Float32, 0, 0);
					outputRasterBand->RasterIO(GF_Write, 0, m, width, 1, data, width, 1, GDT_Float32, 0, 0);
				}
				std::cout << " Complete.\n";
			}
			
			GDALClose(outDataset);
			GDALClose(inDataset);
			delete data;
			delete transformation;
		}
		catch(RSGISImageException e)
		{
			throw e;
		}
	}

	void RSGISImageUtils::copyImageDefiningSpatialReference(std::string inputImage, std::string outputImage, std::string proj, double tlX, double tlY, float xRes, float yRes)throw(RSGISImageException)
	{
		GDALAllRegister();
		GDALDataset *inDataset = NULL;
		GDALDriver *gdalDriver = NULL;
		GDALDataset *outDataset = NULL;
		
		GDALRasterBand *inputRasterBand = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		
		float *data = NULL;
		
		int numOutBands = 0;
		int width = 0;
		int height = 0;
		double *transformation = NULL;
		
		try
		{
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(gdalDriver == NULL)
			{
				std::string message = std::string("Driver for ENVI does not exist\n");
				throw RSGISImageException(message.c_str());
			}
			
			
			std::cout << "Openning image " << inputImage << std::endl;
			inDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
			if(inDataset == NULL)
			{
				std::string message = std::string("Could not open image ") + inputImage;
				throw RSGISImageException(message.c_str());
			}
			
			numOutBands = inDataset->GetRasterCount();
			
			width = inDataset->GetRasterXSize();
			height = inDataset->GetRasterYSize();
			
			transformation = new double[6];
			transformation[0] = tlX;
			transformation[1] = xRes;
			transformation[2] = 0;
			transformation[3] = tlY;
			transformation[4] = 0;
			transformation[5] = yRes;
			
			std::cout << "Creating image " << outputImage << std::endl;
			outDataset = gdalDriver->Create(outputImage.c_str(), width, height, numOutBands, GDT_Float32, NULL);
			if(outDataset == NULL)
			{
				std::string message = std::string("Could not open image ") + outputImage;
				throw RSGISImageException(outputImage.c_str());
			}
			
			outDataset->SetGeoTransform(transformation);
			std::cout << "Defining projections as :\'" << proj << "\'\n";
			char **wktInSpatialRef = new char*[1];
			wktInSpatialRef[0] = const_cast<char *>(proj.c_str());
			OGRSpatialReference ogrSpatial = OGRSpatialReference();
			ogrSpatial.importFromWkt(wktInSpatialRef);
			
			char **wktspatialref = new char*[1];
			wktspatialref[0] = new char[10000];
			ogrSpatial.exportToWkt(wktspatialref);			
			
			CPLErr errorCode = outDataset->SetProjection(wktspatialref[0]);
			if(errorCode == CE_Failure)
			{
				throw RSGISImageException("Projection could not be defined.");
			}
			
			char **proj4spatialref = new char*[1];
			proj4spatialref[0] = new char[1000];
			ogrSpatial.exportToProj4(proj4spatialref);
			std::cout << "As Proj4: \'" << proj4spatialref[0] << "\'" << std::endl;
			
			CPLFree(wktspatialref);
			CPLFree(wktInSpatialRef);
			CPLFree(proj4spatialref);
			
			data = (float *) CPLMalloc(sizeof(float)*width);
			std::cout << "Image size [" << width << "," << height << "]\n";
			
			
			for(int n = 1; n <= numOutBands; n++)
			{
				std::cout << "Image Band " << n << " of " << numOutBands << std::endl;
				inputRasterBand = inDataset->GetRasterBand(n);
				outputRasterBand = outDataset->GetRasterBand(n);
				
				int feedback = height/10;
				int feedbackCounter = 0;
				std::cout << "Started" << std::flush;
				
				for(int m = 0; m < height; m++)
				{
					if((m % feedback) == 0)
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
						feedbackCounter = feedbackCounter + 10;
					}
					
					inputRasterBand->RasterIO(GF_Read, 0, m, width, 1, data, width, 1, GDT_Float32, 0, 0);
					outputRasterBand->RasterIO(GF_Write, 0, m, width, 1, data, width, 1, GDT_Float32, 0, 0);
				}
				std::cout << " Complete.\n";
			}
			
			GDALClose(outDataset);
			GDALClose(inDataset);
			delete data;
			delete transformation;
		}
		catch(RSGISImageException e)
		{
			throw e;
		}
	}
    
    void RSGISImageUtils::createImageSlices(GDALDataset *dataset, std::string outputImageBase) throw(RSGISImageException)
    {
        rsgis::math::RSGISMathsUtils mathUtils;
        
        GDALDriver *gdalDriver = NULL;
        GDALDataset *outDataset = NULL;
        GDALRasterBand *outputRasterBand = NULL;
        GDALRasterBand **inputBands = NULL;
        float *data = NULL;
        
        try
        {
            // Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(gdalDriver == NULL)
			{
				std::string message = std::string("Driver for ENVI does not exist\n");
				throw RSGISImageException(message.c_str());
			}
            
            unsigned int width = dataset->GetRasterXSize();
            unsigned int height = dataset->GetRasterCount();
            
            data = (float *) CPLMalloc(sizeof(float)*width);
            
            inputBands = new GDALRasterBand*[dataset->GetRasterCount()];
            for(int n = 0; n < dataset->GetRasterCount(); ++n)
            {
                inputBands[n] = dataset->GetRasterBand(n+1);
            }
            
            for(int i = 0; i < dataset->GetRasterYSize(); ++i)
            {
                std::string outputImage = outputImageBase + mathUtils.inttostring(i) + std::string(".env");
				outDataset = gdalDriver->Create(outputImage.c_str(), width, height, 1, GDT_Float32, NULL);
                if(outDataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + outputImage;
                    throw RSGISImageException(outputImage.c_str());
                }
                outputRasterBand = outDataset->GetRasterBand(1);
                
                for(int j = 0; j < dataset->GetRasterCount(); ++j)
                {
                    inputBands[j]->RasterIO(GF_Read, 0, i, width, 1, data, width, 1, GDT_Float32, 0, 0);
                    outputRasterBand->RasterIO(GF_Write, 0, ((dataset->GetRasterCount()-j)-1), width, 1, data, width, 1, GDT_Float32, 0, 0);
                }
                
                GDALClose(outDataset);
            }
            
            delete[] inputBands;
            delete[] data;
            
        }
        catch(RSGISImageBandException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::copyFloatGDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException)
    {
        try
        {
            // Change dimensions are the same.
            if(inData->GetRasterXSize() != outData->GetRasterXSize())
            {
                throw RSGISImageException("Widths are not the same");
            }
            if(inData->GetRasterYSize() != outData->GetRasterYSize())
            {
                throw RSGISImageException("Heights are not the same");
            }
            if(inData->GetRasterCount() != outData->GetRasterCount())
            {
                throw RSGISImageException("Number of bands are not the same");
            }
            
            unsigned long width = inData->GetRasterXSize();
            unsigned long height = inData->GetRasterYSize();
            unsigned int numBands = inData->GetRasterCount();
            
            GDALRasterBand **inputRasterBands = new GDALRasterBand*[numBands];
            GDALRasterBand **outputRasterBands = new GDALRasterBand*[numBands];
            float *data = new float[width];
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                inputRasterBands[n] = inData->GetRasterBand(n+1);
                outputRasterBands[n] = outData->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    inputRasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, data, width, 1, GDT_Float32, 0, 0);
                    outputRasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, data, width, 1, GDT_Float32, 0, 0);
                }
            }
                
            delete[] inputRasterBands;
            delete[] outputRasterBands;
            delete[] data;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::copyIntGDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException)
    {
        try
        {
            // Change dimensions are the same.
            if(inData->GetRasterXSize() != outData->GetRasterXSize())
            {
                throw RSGISImageException("Widths are not the same");
            }
            if(inData->GetRasterYSize() != outData->GetRasterYSize())
            {
                throw RSGISImageException("Heights are not the same");
            }
            if(inData->GetRasterCount() != outData->GetRasterCount())
            {
                throw RSGISImageException("Number of bands are not the same");
            }
            
            unsigned long width = inData->GetRasterXSize();
            unsigned long height = inData->GetRasterYSize();
            unsigned int numBands = inData->GetRasterCount();
            
            GDALRasterBand **inputRasterBands = new GDALRasterBand*[numBands];
            GDALRasterBand **outputRasterBands = new GDALRasterBand*[numBands];
            int *data = new int[width];
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                inputRasterBands[n] = inData->GetRasterBand(n+1);
                outputRasterBands[n] = outData->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    inputRasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, data, width, 1, GDT_Int32, 0, 0);
                    outputRasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, data, width, 1, GDT_Int32, 0, 0);
                }
            }
            
            delete[] inputRasterBands;
            delete[] outputRasterBands;
            delete[] data;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::copyUIntGDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException)
    {
        try
        {
            // Change dimensions are the same.
            if(inData->GetRasterXSize() != outData->GetRasterXSize())
            {
                throw RSGISImageException("Widths are not the same");
            }
            if(inData->GetRasterYSize() != outData->GetRasterYSize())
            {
                throw RSGISImageException("Heights are not the same");
            }
            if(inData->GetRasterCount() != outData->GetRasterCount())
            {
                throw RSGISImageException("Number of bands are not the same");
            }
            
            unsigned long width = inData->GetRasterXSize();
            unsigned long height = inData->GetRasterYSize();
            unsigned int numBands = inData->GetRasterCount();
            
            GDALRasterBand **inputRasterBands = new GDALRasterBand*[numBands];
            GDALRasterBand **outputRasterBands = new GDALRasterBand*[numBands];
            unsigned int *data = new unsigned int[width];
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                inputRasterBands[n] = inData->GetRasterBand(n+1);
                outputRasterBands[n] = outData->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    inputRasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, data, width, 1, GDT_UInt32, 0, 0);
                    outputRasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, data, width, 1, GDT_UInt32, 0, 0);
                }
            }
            
            delete[] inputRasterBands;
            delete[] outputRasterBands;
            delete[] data;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::copyFloat32GDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException)
    {
        try
        {
            // Change dimensions are the same.
            if(inData->GetRasterXSize() != outData->GetRasterXSize())
            {
                throw RSGISImageException("Widths are not the same");
            }
            if(inData->GetRasterYSize() != outData->GetRasterYSize())
            {
                throw RSGISImageException("Heights are not the same");
            }
            if(inData->GetRasterCount() != outData->GetRasterCount())
            {
                throw RSGISImageException("Number of bands are not the same");
            }
            
            unsigned long width = inData->GetRasterXSize();
            unsigned long height = inData->GetRasterYSize();
            unsigned int numBands = inData->GetRasterCount();
            
            GDALRasterBand **inputRasterBands = new GDALRasterBand*[numBands];
            GDALRasterBand **outputRasterBands = new GDALRasterBand*[numBands];
            float *data = new float[width];
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                inputRasterBands[n] = inData->GetRasterBand(n+1);
                outputRasterBands[n] = outData->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    inputRasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, data, width, 1, GDT_Float32, 0, 0);
                    outputRasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, data, width, 1, GDT_Float32, 0, 0);
                }
            }
            
            delete[] inputRasterBands;
            delete[] outputRasterBands;
            delete[] data;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::copyByteGDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException)
    {
        try
        {
            // Change dimensions are the same.
            if(inData->GetRasterXSize() != outData->GetRasterXSize())
            {
                throw RSGISImageException("Widths are not the same");
            }
            if(inData->GetRasterYSize() != outData->GetRasterYSize())
            {
                throw RSGISImageException("Heights are not the same");
            }
            if(inData->GetRasterCount() != outData->GetRasterCount())
            {
                throw RSGISImageException("Number of bands are not the same");
            }
            
            unsigned long width = inData->GetRasterXSize();
            unsigned long height = inData->GetRasterYSize();
            unsigned int numBands = inData->GetRasterCount();
            
            GDALRasterBand **inputRasterBands = new GDALRasterBand*[numBands];
            GDALRasterBand **outputRasterBands = new GDALRasterBand*[numBands];
            int *data = new int[width];
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                inputRasterBands[n] = inData->GetRasterBand(n+1);
                outputRasterBands[n] = outData->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    inputRasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, data, width, 1, GDT_Byte, 0, 0);
                    outputRasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, data, width, 1, GDT_Byte, 0, 0);
                }
            }
            
            delete[] inputRasterBands;
            delete[] outputRasterBands;
            delete[] data;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::zerosUIntGDALDataset(GDALDataset *data) throw(RSGISImageException)
    {
        try
        {
            unsigned long xSize = data->GetRasterXSize();
            unsigned long ySize = data->GetRasterYSize();
            unsigned int numBands = data->GetRasterCount();
            int xBlockSize = 0;
            int yBlockSize = 0;
            
            GDALRasterBand **rasterBands = new GDALRasterBand*[numBands];
            for(int i = 0; i < numBands; i++)
            {
                rasterBands[i] = data->GetRasterBand(i+1);
            }
            rasterBands[0]->GetBlockSize(&xBlockSize, &yBlockSize);
            
            // Allocate memory
            unsigned int *dataVals = new unsigned int[xSize*yBlockSize];
            
            for(unsigned int i = 0; i < (xSize*yBlockSize); ++i)
            {
                dataVals[i] = 0;
            }
            
            int nYBlocks = ySize / yBlockSize;
            int remainRows = ySize - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
            int feedback = nYBlocks/10;
            int feedbackCounter = 0;
            std::cout << "Started" << std::flush;
            // Loop images to process data
            for(int i = 0; i < nYBlocks; i++)
            {
                if((feedback != 0) && (i % feedback == 0))
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                
                for(int n = 0; n < numBands; n++)
                {
                    rowOffset = yBlockSize * i;
                    rasterBands[n]->RasterIO(GF_Write, 0, rowOffset, xSize, yBlockSize, dataVals, xSize, yBlockSize, GDT_UInt32, 0, 0);
                }
            }
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numBands; n++)
                {
                    rowOffset = (yBlockSize * nYBlocks);
                    rasterBands[n]->RasterIO(GF_Write, 0, rowOffset, xSize, remainRows, dataVals, xSize, remainRows, GDT_UInt32, 0, 0);
                }
            }
            std::cout << " Complete.\n";
            
            delete[] rasterBands;
            delete[] dataVals;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::zerosFloatGDALDataset(GDALDataset *data) throw(RSGISImageException)
    {
        try
        {
            unsigned long xSize = data->GetRasterXSize();
            unsigned long ySize = data->GetRasterYSize();
            unsigned int numBands = data->GetRasterCount();
            int xBlockSize = 0;
            int yBlockSize = 0;
            
            GDALRasterBand **rasterBands = new GDALRasterBand*[numBands];
            for(int i = 0; i < numBands; i++)
            {
                rasterBands[i] = data->GetRasterBand(i+1);
            }
            rasterBands[0]->GetBlockSize(&xBlockSize, &yBlockSize);
            
            // Allocate memory
            float *dataVals = new float[xSize*yBlockSize];
            
            for(unsigned int i = 0; i < (xSize*yBlockSize); ++i)
            {
                dataVals[i] = 0;
            }
            
            int nYBlocks = ySize / yBlockSize;
            int remainRows = ySize - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
            int feedback = nYBlocks/10;
            int feedbackCounter = 0;
            std::cout << "Started" << std::flush;
            // Loop images to process data
            for(int i = 0; i < nYBlocks; i++)
            {
                if((feedback != 0) && (i % feedback == 0))
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                
                for(int n = 0; n < numBands; n++)
                {
                    rowOffset = yBlockSize * i;
                    rasterBands[n]->RasterIO(GF_Write, 0, rowOffset, xSize, yBlockSize, dataVals, xSize, yBlockSize, GDT_Float32, 0, 0);
                }
            }
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numBands; n++)
                {
                    rowOffset = (yBlockSize * nYBlocks);
                    rasterBands[n]->RasterIO(GF_Write, 0, rowOffset, xSize, remainRows, dataVals, xSize, remainRows, GDT_Float32, 0, 0);
                }
            }
            std::cout << " Complete.\n";
            
            delete[] rasterBands;
            delete[] dataVals;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::zerosByteGDALDataset(GDALDataset *data) throw(RSGISImageException)
    {
        try
        {
            unsigned long xSize = data->GetRasterXSize();
            unsigned long ySize = data->GetRasterYSize();
            unsigned int numBands = data->GetRasterCount();
            int xBlockSize = 0;
            int yBlockSize = 0;
            
            GDALRasterBand **rasterBands = new GDALRasterBand*[numBands];
            for(int i = 0; i < numBands; i++)
            {
                rasterBands[i] = data->GetRasterBand(i+1);
            }
            rasterBands[0]->GetBlockSize(&xBlockSize, &yBlockSize);
            
            // Allocate memory
            int *dataVals = new int[xSize*yBlockSize];
            
            for(unsigned int i = 0; i < (xSize*yBlockSize); ++i)
            {
                dataVals[i] = 0;
            }
            
            int nYBlocks = ySize / yBlockSize;
            int remainRows = ySize - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
            int feedback = nYBlocks/10;
            int feedbackCounter = 0;
            std::cout << "Started" << std::flush;
            // Loop images to process data
            for(int i = 0; i < nYBlocks; i++)
            {
                if((feedback != 0) && (i % feedback == 0))
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                
                for(int n = 0; n < numBands; n++)
                {
                    rowOffset = yBlockSize * i;
                    rasterBands[n]->RasterIO(GF_Write, 0, rowOffset, xSize, yBlockSize, dataVals, xSize, yBlockSize, GDT_Byte, 0, 0);
                }
            }
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numBands; n++)
                {
                    rowOffset = (yBlockSize * nYBlocks);
                    rasterBands[n]->RasterIO(GF_Write, 0, rowOffset, xSize, remainRows, dataVals, xSize, remainRows, GDT_Byte, 0, 0);
                }
            }
            std::cout << " Complete.\n";
            
            delete[] rasterBands;
            delete[] dataVals;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::assignValGDALDataset(GDALDataset *data, float value) throw(RSGISImageException)
    {
        try
        {
            unsigned long xSize = data->GetRasterXSize();
            unsigned long ySize = data->GetRasterYSize();
            unsigned int numBands = data->GetRasterCount();
            int xBlockSize = 0;
            int yBlockSize = 0;
            
            GDALRasterBand **rasterBands = new GDALRasterBand*[numBands];
            for(int i = 0; i < numBands; i++)
            {
                rasterBands[i] = data->GetRasterBand(i+1);
            }
            rasterBands[0]->GetBlockSize(&xBlockSize, &yBlockSize);
            
            // Allocate memory
            float *dataVals = (float *) CPLMalloc(sizeof(float)*(xSize*yBlockSize));
            
            for(unsigned int i = 0; i < (xSize*yBlockSize); ++i)
            {
                dataVals[i] = value;
            }
            
            int nYBlocks = ySize / yBlockSize;
            int remainRows = ySize - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
            int feedback = nYBlocks/10;
            int feedbackCounter = 0;
            std::cout << "Started" << std::flush;
            // Loop images to process data
            for(int i = 0; i < nYBlocks; i++)
            {
                if((feedback != 0) && (i % feedback == 0))
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                
                for(int n = 0; n < numBands; n++)
                {
                    rowOffset = yBlockSize * i;
                    rasterBands[n]->RasterIO(GF_Write, 0, rowOffset, xSize, yBlockSize, dataVals, xSize, yBlockSize, GDT_Float32, 0, 0);
                }
            }
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numBands; n++)
                {
                    rowOffset = (yBlockSize * nYBlocks);
                    rasterBands[n]->RasterIO(GF_Write, 0, rowOffset, xSize, remainRows, dataVals, xSize, remainRows, GDT_Float32, 0, 0);
                }
            }
            std::cout << " Complete.\n";
            
            delete[] rasterBands;
            delete[] dataVals;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    GDALDataset* RSGISImageUtils::createCopy(GDALDataset *inData, std::string outputFilePath, std::string outputFormat, GDALDataType eType, bool useImgProj, std::string proj)throw(RSGISImageException)
    {
        unsigned long width = inData->GetRasterXSize();
        unsigned long height = inData->GetRasterYSize();
        unsigned int numBands = inData->GetRasterCount();
        
        double *gdalTranslation = new double[6];
        inData->GetGeoTransform(gdalTranslation);
        
        // Process dataset in memory
        GDALDriver *gdalDriver = NULL;
        gdalDriver = GetGDALDriverManager()->GetDriverByName(outputFormat.c_str());
        if(gdalDriver == NULL)
        {
            delete[] gdalTranslation;
            std::string message = std::string("Driver for ") + outputFormat + std::string(" does not exist\n");
            throw RSGISImageException(message.c_str());
        }
        GDALDataset *dataset = gdalDriver->Create(outputFilePath.c_str(), width, height, numBands, eType, NULL);
        if(dataset == NULL)
        {
            delete[] gdalTranslation;
            std::string message = std::string("Could not create GDALDataset.");
            throw RSGISImageException(message);
        }
        
        dataset->SetGeoTransform(gdalTranslation);
        if(useImgProj)
        {
            dataset->SetProjection(inData->GetProjectionRef());
        }
        else
        {
            dataset->SetProjection(proj.c_str());
        }
        
        delete[] gdalTranslation;
        
        return dataset;

    }
    
    GDALDataset* RSGISImageUtils::createCopy(GDALDataset *inData, unsigned int numBands, std::string outputFilePath, std::string outputFormat, GDALDataType eType, bool useImgProj, std::string proj)throw(RSGISImageException)
    {
        unsigned long width = inData->GetRasterXSize();
        unsigned long height = inData->GetRasterYSize();
        
        double *gdalTranslation = new double[6];
        inData->GetGeoTransform(gdalTranslation);
        
        GDALDriver *gdalDriver = GetGDALDriverManager()->GetDriverByName(outputFormat.c_str());
        if(gdalDriver == NULL)
        {
            delete[] gdalTranslation;
            std::string message = std::string("Driver for ") + outputFormat + std::string(" does not exist\n");
            throw RSGISImageException(message.c_str());
        }
        
        GDALDataset *dataset = gdalDriver->Create(outputFilePath.c_str(), width, height, numBands, eType, NULL);
        if(dataset == NULL)
        {
            delete[] gdalTranslation;
            std::string message = std::string("Could not create GDALDataset.");
            throw RSGISImageException(message);
        }
        
        dataset->SetGeoTransform(gdalTranslation);
        if(useImgProj)
        {
            dataset->SetProjection(inData->GetProjectionRef());
        }
        else
        {
            dataset->SetProjection(proj.c_str());
        }
        delete[] gdalTranslation;
        
        return dataset;
        
    }
    
    GDALDataset* RSGISImageUtils::createCopy(GDALDataset *inData, unsigned int numBands, std::string outputFilePath, std::string outputFormat, GDALDataType eType, geos::geom::Envelope extent, bool useImgProj, std::string proj)throw(RSGISImageException)
    {
        GDALDataset *dataset = NULL;
        try
        {
            double outImgMinX = 0.0;
            //double outImgMaxX = 0.0;
            //double outImgMinY = 0.0;
            double outImgMaxY = 0.0;
            unsigned long outImgWidth = 0;
            unsigned long outImgHeight = 0;
            
            unsigned long width = inData->GetRasterXSize();
            unsigned long height = inData->GetRasterYSize();
            
            double *gdalTranslation = new double[6];
            inData->GetGeoTransform(gdalTranslation);
            
            double xPxlSize = gdalTranslation[1];
            double yPxlSize = gdalTranslation[5] * (-1);
            
            double imgMinX = gdalTranslation[0];
            double imgMaxX = imgMinX + (gdalTranslation[1] * width);
            double imgMaxY = gdalTranslation[3];
            double imgMinY = imgMaxY + (gdalTranslation[5] * height);
            
            // Check extent is within image.
            if( (extent.getMinX() >= imgMinX) & (extent.getMaxX() <= imgMaxX) & (extent.getMinY() >= imgMinY) & (extent.getMaxY() <= imgMinY) )
            {
                delete[] gdalTranslation;
                throw RSGISImageException("Extent needs to be within the image extent.");
            }
            
            // Find Min X
            double xMinDiff = extent.getMinX() - imgMinX;
            unsigned int numPxls2ExtXMin = floor(xMinDiff/xPxlSize);
            outImgMinX = imgMinX + (numPxls2ExtXMin * xPxlSize);
            
            // Find Max Y
            double yMaxDiff = imgMaxY - extent.getMaxY();
            unsigned int numPxls2ExtYMax = floor(yMaxDiff/yPxlSize);
            outImgMaxY = imgMaxY - (numPxls2ExtYMax * yPxlSize);
            
            // Find Max X and width
            double xMaxDiff = extent.getMaxX() - outImgMinX;
            outImgWidth = ceil(xMaxDiff/xPxlSize);
            
            // Find Min Y and height
            double yMinDiff = outImgMaxY - extent.getMinY();
            outImgHeight = ceil(yMinDiff/yPxlSize);
            
            gdalTranslation[0] = outImgMinX;
            gdalTranslation[3] = outImgMaxY;
            
            GDALDriver *gdalDriver = GetGDALDriverManager()->GetDriverByName(outputFormat.c_str());
            if(gdalDriver == NULL)
            {
                delete[] gdalTranslation;
                std::string message = std::string("Driver for ") + outputFormat + std::string(" does not exist\n");
                throw RSGISImageException(message.c_str());
            }
            
            dataset = gdalDriver->Create(outputFilePath.c_str(), outImgWidth, outImgHeight, numBands, eType, NULL);
            if(dataset == NULL)
            {
                delete[] gdalTranslation;
                std::string message = std::string("Could not create GDALDataset.");
                throw RSGISImageException(message);
            }
            
            dataset->SetGeoTransform(gdalTranslation);
            if(useImgProj)
            {
                dataset->SetProjection(inData->GetProjectionRef());
            }
            else
            {
                dataset->SetProjection(proj.c_str());
            }
            delete[] gdalTranslation;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
        return dataset;
    }
    
    
    GDALDataset* RSGISImageUtils::createCopy(GDALDataset *inData, unsigned int numBands, std::string outputFilePath, std::string outputFormat, GDALDataType eType, double xMin, double xMax, double yMin, double yMax, double xRes, double yRes, bool useImgProj, std::string proj)throw(RSGISImageException)
    {
        GDALDataset *dataset = NULL;
        try
        {
            if(yRes > 0)
            {
                yRes = yRes * (-1);
            }
            
            // Find width
            double xDiff = xMax - xMin;
            unsigned long outImgWidth = abs(ceil(xDiff/xRes));
                        
            // Find height
            double yDiff = yMax - yMin;
            unsigned long outImgHeight = abs(ceil(yDiff/yRes));
            
            double *gdalTranslation = new double[6];
            inData->GetGeoTransform(gdalTranslation);
            
            // Define the TL
            gdalTranslation[0] = xMin;
            gdalTranslation[3] = yMax;
            
            // Define the pixel size:
            gdalTranslation[1] = xRes;
            gdalTranslation[5] = yRes;
            
            GDALDriver *gdalDriver = GetGDALDriverManager()->GetDriverByName(outputFormat.c_str());
            if(gdalDriver == NULL)
            {
                delete[] gdalTranslation;
                std::string message = std::string("Driver for ") + outputFormat + std::string(" does not exist\n");
                throw RSGISImageException(message.c_str());
            }
                        
            dataset = gdalDriver->Create(outputFilePath.c_str(), outImgWidth, outImgHeight, numBands, eType, NULL);
            if(dataset == NULL)
            {
                delete[] gdalTranslation;
                std::string message = std::string("Could not create GDALDataset.");
                throw RSGISImageException(message);
            }
            
            dataset->SetGeoTransform(gdalTranslation);
            if(useImgProj)
            {
                dataset->SetProjection(inData->GetProjectionRef());
            }
            else
            {
                dataset->SetProjection(proj.c_str());
            }
            delete[] gdalTranslation;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
        return dataset;
    }
    
    
    GDALDataset* RSGISImageUtils::createCopy(GDALDataset **datasets, int numDS, unsigned int numBands, std::string outputFilePath, std::string outputFormat, GDALDataType eType, bool useImgProj, std::string proj)throw(RSGISImageException)
    {
        GDALDataset *dataset = NULL;
        try
        {
            int **dsOffsets = new int*[numDS];
            for(int i = 0; i < numDS; i++)
            {
                dsOffsets[i] = new int[2];
            }
            int width = 0;
            int height = 0;
            double *gdalTranslation = new double[6];
            this->getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation);
            for(int i = 0; i < numDS; ++i)
            {
                delete[] dsOffsets[i];
            }
            delete[] dsOffsets;
            
            GDALDriver *gdalDriver = NULL;
            gdalDriver = GetGDALDriverManager()->GetDriverByName(outputFormat.c_str());
            if(gdalDriver == NULL)
            {
                delete[] gdalTranslation;
                std::string message = std::string("Driver for ") + outputFormat + std::string(" does not exist\n");
                throw RSGISImageException(message.c_str());
            }
            dataset = gdalDriver->Create(outputFilePath.c_str(), width, height, numBands, eType, NULL);
            if(dataset == NULL)
            {
                delete[] gdalTranslation;
                std::string message = std::string("Could not create GDALDataset.");
                throw RSGISImageException(message);
            }
            
            dataset->SetGeoTransform(gdalTranslation);
            if(useImgProj)
            {
                dataset->SetProjection(datasets[0]->GetProjectionRef());
            }
            else
            {
                dataset->SetProjection(proj.c_str());
            }
            delete[] gdalTranslation;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
        
        return dataset;
    }
    
    void RSGISImageUtils::createKMLText(std::string inputImage, std::string outKMLFile) throw(RSGISImageBandException)
    {
        
        // Open text file for writing
        std::ofstream outKML;
        outKML.open(outKMLFile.c_str());
        
        GDALAllRegister();
        GDALDataset *dataset = NULL;
        dataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
        
        if(dataset == NULL)
        {
            std::string message = std::string("Could not open image ") + inputImage;
            throw RSGISImageException(message.c_str());
        }
        
        double *transformations = new double[6];
        dataset->GetGeoTransform(transformations);
        double xSize = dataset->GetRasterXSize();
        double ySize = dataset->GetRasterYSize();
        double pixelXRes = transformations[1];
        double pixelYRes = transformations[5];
        
        double minX = transformations[0];
        double maxY = transformations[3];
        
        double maxX = minX + (xSize * pixelXRes);
        double minY = maxY - (ySize * fabs(pixelYRes));
        
        // Write out information 
        outKML << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        outKML << "	<kml xmlns=\"http://earth.google.com/kml/2.1\">\n";
        outKML << "	  <Document>\n";
        outKML << "	    <Name>" << inputImage << "</Name>\n";
        outKML << "	    <Description></Description>\n";
        outKML << "	    <Style>\n";
        outKML << "	      <ListStyle id=\"hideChildren\">\n";
        outKML << "	        <listItemType>checkHideChildren</listItemType>\n";
        outKML << "	      </ListStyle>\n";
        outKML << "	    </Style>\n";
        outKML << "	    <GroundOverlay>\n";
        outKML << "	      <Icon>\n";
        outKML << "	        <href>" << inputImage << "</href>\n";
        outKML << "	      </Icon>\n";
        outKML << "	      <LatLonBox>\n";
        outKML << "	        <north>" <<  maxY << "</north>\n";
        outKML << "	        <south>" << minY << "</south>\n";
        outKML << "	        <east>" << minX << "</east>\n";
        outKML << "	        <west>" << maxX << "</west>\n";
        outKML << "	      </LatLonBox>\n";
        outKML << "	    </GroundOverlay>\n";
        outKML << "	    </Document>\n";
        outKML << "	</kml>\n";
        
        delete[] transformations;
        
        
        GDALClose(dataset);
        
        
    }
	
    bool RSGISImageUtils::closeResTest(double baseRes, double targetRes)
    {
    	/** Calculates if two doubles are close to each other with the threshold
    	 * defined in the class.
    	 * - A two sided test is used rather than the absolute value to prevent
    	 * 	 overflows.
    	 */
        
        if((baseRes < 0) & (targetRes > 0))
        {
            return false;
        }
        if((baseRes > 0) & (targetRes < 0))
        {
            return false;
        }

    	bool closeRes = true;
    	double resDiff = baseRes - targetRes;
    	double resDiffVal = this->resDiffThresh * baseRes;

        if(resDiff < 0)
        {
            resDiff = resDiff * (-1.0);
        }
        if(resDiffVal < 0)
        {
            resDiffVal = resDiffVal * (-1.0);
        }
        
    	if((resDiff > 0) && (resDiff > resDiffVal))
        {
            closeRes = false;
        }
    	return closeRes;
    }
    
    double RSGISImageUtils::getPixelValue(GDALDataset *image, unsigned int imgBand, double xLoc, double yLoc) throw(RSGISImageException)
    {
        double outVal = 0.0;
        try
        {
            unsigned int numImgBands = image->GetRasterCount();
            
            if(imgBand > numImgBands)
            {
                throw RSGISImageException("The band specified is not within the image.");
            }
            
            GDALRasterBand *gdalBand = image->GetRasterBand(imgBand);
            
            double geoTransform[6];
            
            double xMin = 0.0;
            double yMax = 0.0;
            
            double xMax = 0.0;
            double yMin = 0.0;
            
            double imgRes = 0.0;
            
            double *pxlValue = (double *) CPLMalloc(sizeof(double));
            
            if( image->GetGeoTransform( geoTransform ) == CE_None )
            {
                xMin = geoTransform[0];
                yMax = geoTransform[3];
                
                xMax = geoTransform[0] + (image->GetRasterXSize() * geoTransform[1]);
                yMin = geoTransform[3] + (image->GetRasterYSize() * geoTransform[5]);
                
                imgRes = geoTransform[1];
            }
            else
            {
                throw RSGISImageException("Could not read Geo Transform.");
            }
            
            if((xLoc >= xMin) &&
               (xLoc <= xMax) &&
               (yLoc >= yMin) &&
               (yLoc <= yMax))
            {
                double xDiff = xLoc - xMin;
                double yDiff = yMax - yLoc;
                
                int xPxl = static_cast<int> (xDiff/imgRes);
                int yPxl = static_cast<int> (yDiff/imgRes);
                
                gdalBand->RasterIO(GF_Read, xPxl, yPxl, 1, 1, pxlValue, 1, 1, GDT_Float64, 0, 0);
                outVal = pxlValue[0];
            }
            else
            {
                throw RSGISImageException("Point not within image file provided\n");
            }
            delete pxlValue;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
        
        return outVal;
    }
    
    double RSGISImageUtils::getPixelValue(GDALDataset *image, unsigned int imgBand, unsigned int xPxl, unsigned int yPxl) throw(RSGISImageException)
    {
        double outVal = 0.0;
        try
        {
            unsigned int numImgBands = image->GetRasterCount();
            
            if(imgBand > numImgBands)
            {
                throw RSGISImageException("The band specified is not within the image.");
            }
            
            if((xPxl < image->GetRasterXSize()) && (yPxl < image->GetRasterYSize()))
            {
                GDALRasterBand *gdalBand = image->GetRasterBand(imgBand);
                double *pxlValue = (double *) CPLMalloc(sizeof(double));
                gdalBand->RasterIO(GF_Read, xPxl, yPxl, 1, 1, pxlValue, 1, 1, GDT_Float64, 0, 0);
                outVal = pxlValue[0];
                delete pxlValue;
            }
            else
            {
                throw RSGISImageException("Point not within image file provided\n");
            }
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
        
        return outVal;
    }
    
    void RSGISImageUtils::setPixelValue(GDALDataset *image, unsigned int imgBand, unsigned int xPxl, unsigned int yPxl, double val) throw(RSGISImageException)
    {
        try
        {
            unsigned int numImgBands = image->GetRasterCount();
            
            if(imgBand > numImgBands)
            {
                throw RSGISImageException("The band specified is not within the image.");
            }
            
            if((xPxl < image->GetRasterXSize()) && (yPxl < image->GetRasterYSize()))
            {
                GDALRasterBand *gdalBand = image->GetRasterBand(imgBand);
                gdalBand->RasterIO(GF_Write, xPxl, yPxl, 1, 1, &val, 1, 1, GDT_Float64, 0, 0);
            }
            else
            {
                throw RSGISImageException("Point not within image file provided\n");
            }
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }
    
    void RSGISImageUtils::createImageGrid(GDALDataset *inData, unsigned int numXPxls, unsigned int numYPxls, bool offset) throw(RSGISImageException)
    {
        //std::cerr << "WARNING: RSGISImageUtils::createImageGrid shouldn't be used; use rsgis::segment::RSGISCreateImageGrid\n";
        try
        {
            unsigned long width = inData->GetRasterXSize();
            unsigned long height = inData->GetRasterYSize();
            unsigned int numBands = inData->GetRasterCount();
            if(numBands != 1)
            {
                throw RSGISImageException("Data must only have 1 image band.");
            }
            
            GDALRasterBand *rasterBand = inData->GetRasterBand(1);
            unsigned int *dataVals = new unsigned int[width];
            
            for(unsigned int i = 0; i < width; ++i)
            {
                dataVals[i] = 0;
            }
            
            if(!offset)
            {
                unsigned int rowStartVal = 1;
                unsigned int curPxlVal = 1;
                
                for(unsigned long y = 0; y < height; ++y)
                {
                    if((y % numYPxls) == 0)
                    {
                        rowStartVal = curPxlVal + 1;
                    }
                    
                    curPxlVal = rowStartVal;
                    
                    for(unsigned long x = 0; x < width; ++x)
                    {
                        if((x % numXPxls) == 0)
                        {
                            ++curPxlVal;
                        }
                        dataVals[x] = curPxlVal;
                    }
                    
                    rasterBand->RasterIO(GF_Write, 0, y, width, 1, dataVals, width, 1, GDT_Int32, 0, 0);
                }
            }
            else
            {
                throw RSGISImageException("RSGISImageUtils::createImageGrid with offset not implemented.");
            }
            
            delete[] dataVals;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::populateImagePixelsInRange(GDALDataset *image, int minVal, int maxVal, bool singleLine) throw(RSGISImageException)
    {
        try
        {
            if(minVal >= maxVal)
            {
                throw RSGISImageException("Min value must be smaller than Max value.");
            }
            
            unsigned long width = image->GetRasterXSize();
            unsigned long height = image->GetRasterYSize();
            unsigned int numBands = image->GetRasterCount();
            if(numBands != 1)
            {
                throw RSGISImageException("Data must only have 1 image band.");
            }
            
            GDALRasterBand *rasterBand = image->GetRasterBand(1);
            unsigned int *dataVals = new unsigned int[width];
            
            for(unsigned int i = 0; i < width; ++i)
            {
                dataVals[i] = 0;
            }
            
            int range = maxVal - minVal;
            int curPxlVal = minVal;
            int rangeLineCount = 0;
            
            for(unsigned long y = 0; y < height; ++y)
            {
                if(!singleLine)
                {
                    curPxlVal = minVal + rangeLineCount;
                }
                
                for(unsigned long x = 0; x < width; ++x)
                {
                    if(curPxlVal > maxVal)
                    {
                        curPxlVal = minVal;
                    }
                    dataVals[x] = curPxlVal;
                    ++curPxlVal;
                }
                
                rasterBand->RasterIO(GF_Write, 0, y, width, 1, dataVals, width, 1, GDT_Int32, 0, 0);
                ++rangeLineCount;
                if(rangeLineCount > range)
                {
                    rangeLineCount = 0;
                }
            }
            
            delete[] dataVals;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    
    void RSGISImageUtils::setImageBandNames(GDALDataset *dataset, std::vector<std::string> bandNames, bool quiet) throw(RSGISImageException)
    {
        try
        {
            unsigned int numBands = dataset->GetRasterCount();
            if(numBands != bandNames.size())
            {
                throw RSGISImageException("List of names must be the same length as the number of image bands.");
            }

            unsigned int bandN = 0;
            for(unsigned int n = 0; n < numBands; ++n)
            {
                bandN = n + 1;
                if(!quiet)
                {
                    std::cout << "Setting band " << bandN << " name as \'" << bandNames.at(n) << "\'\n";
                }
                dataset->GetRasterBand(bandN)->SetDescription(bandNames.at(n).c_str());
            }
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }

	RSGISImageUtils::~RSGISImageUtils()
	{
		
	}
	
}}
