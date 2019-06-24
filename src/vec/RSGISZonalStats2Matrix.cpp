/*
 *  RSGISZonalStats2Matrix.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/11/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#include "RSGISZonalStats2Matrix.h"

namespace rsgis{namespace vec{
	
	RSGISZonalStats2Matrix::RSGISZonalStats2Matrix()
	{
		
	}
	
	ClassVariables** RSGISZonalStats2Matrix::findPixelStats(GDALDataset **image, int numImgs, OGRLayer *vecLayer, std::string classAttribute, int *numMatrices, rsgis::img::pixelInPolyOption method)
	{		
        rsgis::img::RSGISCalcImageSingleValue *polyPxlCount = NULL;
		ClassVariables **classVars = NULL;
		rsgis::img::RSGISCalcImageSingle *calcImageValue;
		rsgis::img::RSGISCalcImageSingleValue *polyMatrix = NULL;
		rsgis::img::RSGISCalcImageSingle *calcPolyMatrix = NULL;

		rsgis::math::Matrix *matrix = NULL;
		int *matrixPter = NULL;
		RSGISPolygonData **polyData = NULL;
		RSGISClassPolygon **data = NULL;
		
		std::vector<std::string> *classNames = new std::vector<std::string>();
		double *numPxls = new double[1];
		
		RSGISVectorIO vecIO;
		rsgis::math::RSGISMatrices matrixUtils;
		
		int numVariables = 0;
		int numFeatures = 0;
		int numClasses = 0;
		try
		{
			for(int i = 0; i < numImgs; i++)
			{
				numVariables += image[i]->GetRasterCount();
			}
			std::cout << "Raster Band Count = " << numVariables << std::endl;
			
			// READ IN SHAPEFILE
			numFeatures = vecLayer->GetFeatureCount();
			polyData = new RSGISPolygonData*[numFeatures];
			for(int i = 0; i < numFeatures; i++)
			{
				polyData[i] = new RSGISClassPolygon(classAttribute);
			}
			std::cout << "Reading in " << numFeatures << " features\n";
			vecIO.readPolygons(vecLayer, polyData, numFeatures);
			
			//Convert to RSGISClassPolygons
			data = new RSGISClassPolygon*[numFeatures];
			for(int i = 0; i < numFeatures; i++)
			{
				data[i] = dynamic_cast<RSGISClassPolygon*>(polyData[i]);
			}
			delete[] polyData;
			
			// Count the number of classes
			bool foundClass = false;
			for(int i = 0; i < numFeatures; i++)
			{
				foundClass = false;
				for(unsigned int j = 0; j < classNames->size(); j++)
				{
					if(classNames->at(j) == data[i]->getClassName())
					{
						foundClass = true;
					}
				}
				if(!foundClass)
				{
					classNames->push_back(data[i]->getClassName());
				}
			}
			
			numClasses = classNames->size();
			std::cout << "Vector file contains " << numClasses << " classes:\n";
	
			
			// Count the number of pixels within each polygon
			std::cout << "Count the number of pixels within each polygon\n";
			polyPxlCount = new rsgis::img::RSGISPolygonPixelCount(1);
			calcImageValue = new rsgis::img::RSGISCalcImageSingle(polyPxlCount);
			for(int i = 0; i < numFeatures; i++)
			{
				polyPxlCount->reset();
				calcImageValue->calcImageWithinPolygon(image, numImgs, numPxls, data[i]->getBBox(), data[i]->getPolygon(), true, method);
				data[i]->setNumPixels(ceil(numPxls[0]));
			}
			
			// Create a matrix for each polygon
			std::cout << "Populate the matrices of each polygon.\n";
			for(int i = 0; i < numFeatures; i++)
			{
				polyMatrix = new rsgis::img::RSGISPopulateMatrix(numVariables, data[i]->getNumPixels());
				calcPolyMatrix = new rsgis::img::RSGISCalcImageSingle(polyMatrix);
				calcPolyMatrix->calcImageWithinPolygon(image, numImgs, numPxls, data[i]->getBBox(), data[i]->getPolygon(), false, rsgis::img::polyContainsPixelCenter);
				matrix = dynamic_cast<rsgis::img::RSGISPopulateMatrix*>(polyMatrix)->getMatrix();
				data[i]->setPixelValues(matrix);
				delete polyMatrix;
				delete calcPolyMatrix;
			}
			
			classVars = new ClassVariables*[numClasses];
			
			for(int i = 0; i < numClasses; i++)
			{
				classVars[i] = new ClassVariables();
				classVars[i]->name = classNames->at(i);
				classVars[i]->numPxls = 0;
			}
			
			
			for(int i = 0; i < numFeatures; i++)
			{
				for(int j = 0; j < numClasses; j++)
				{
					if(classVars[j]->name == data[i]->getClassName())
					{
						classVars[j]->numPxls += data[i]->getNumPixels();
					}
				}
			}
			
			matrixPter = new int[numClasses];
			for(int i = 0; i < numClasses; i++)
			{
				std::cout << i << ") " << classVars[i]->name << " has a total number of pixels = " << classVars[i]->numPxls << std::endl;
				classVars[i]->matrix = matrixUtils.createMatrix(numVariables, classVars[i]->numPxls);
				matrixPter[i] = 0;
			}
			
			int numElements = 0;
			for(int i = 0; i < numFeatures; i++)
			{
				for(int j = 0; j < numClasses; j++)
				{
					if(classVars[j]->name == data[i]->getClassName())
					{
						matrix = data[i]->getPixelValues();
						numElements = matrix->n * matrix->m;
						for(int k = 0; k < numElements; k++)
						{
							classVars[j]->matrix->matrix[matrixPter[j]++] = matrix->matrix[k];
						}
					}
				}
			}
		}
		catch(RSGISVectorException &e)
		{
			if(calcImageValue != NULL)
			{
				delete calcImageValue;
			}
			if(polyPxlCount != NULL)
			{
			 delete polyPxlCount;
			}
			if(matrixPter != NULL)
			{
				delete[] matrixPter;
			}
			if(classNames != NULL)
			{
				delete classNames;
			}
			if(numPxls != NULL)
			{
				delete[] numPxls;
			}
			if(data != NULL)
			{
				for(int i = 0; i < numFeatures; i++)
				{
					if(data[i] != NULL)
					{
						delete data[i];
					}
				}
				delete[] data;
			}
			
			throw e;
		}
		catch(rsgis::img::RSGISImageCalcException &e)
		{
			if(calcImageValue != NULL)
			{
				delete calcImageValue;
			}
			if(polyPxlCount != NULL)
			{
				delete polyPxlCount;
			}
			if(matrixPter != NULL)
			{
				delete[] matrixPter;
			}
			if(classNames != NULL)
			{
				delete classNames;
			}
			if(numPxls != NULL)
			{
				delete[] numPxls;
			}
			if(data != NULL)
			{
				for(int i = 0; i < numFeatures; i++)
				{
					if(data[i] != NULL)
					{
						delete data[i];
					}
				}
				delete[] data;
			}
			
			throw e;
		}
		catch(rsgis::img::RSGISImageBandException &e)
		{
			if(calcImageValue != NULL)
			{
				delete calcImageValue;
			}
			if(polyPxlCount != NULL)
			{
				delete polyPxlCount;
			}
			if(matrixPter != NULL)
			{
				delete[] matrixPter;
			}
			if(classNames != NULL)
			{
				delete classNames;
			}
			if(numPxls != NULL)
			{
				delete[] numPxls;
			}
			if(data != NULL)
			{
				for(int i = 0; i < numFeatures; i++)
				{
					if(data[i] != NULL)
					{
						delete data[i];
					}
				}
				delete[] data;
			}
			
			throw e;
		}
		catch(RSGISException &e)
		{
			if(calcImageValue != NULL)
			{
				delete calcImageValue;
			}
			if(polyPxlCount != NULL)
			{
				delete polyPxlCount;
			}
			if(matrixPter != NULL)
			{
				delete[] matrixPter;
			}
			if(classNames != NULL)
			{
				delete classNames;
			}
			if(numPxls != NULL)
			{
				delete[] numPxls;
			}
			if(data != NULL)
			{
				for(int i = 0; i < numFeatures; i++)
				{
					if(data[i] != NULL)
					{
						delete data[i];
					}
				}
				delete[] data;
			}
			
			throw e;
		}
		
		if(calcImageValue != NULL)
		{
			delete calcImageValue;
		}
		if(polyPxlCount != NULL)
		{
			delete polyPxlCount;
		}
		if(matrixPter != NULL)
		{
			delete[] matrixPter;
		}
		if(classNames != NULL)
		{
			delete classNames;
		}
		if(numPxls != NULL)
		{
			delete[] numPxls;
		}
		if(data != NULL)
		{
			for(int i = 0; i < numFeatures; i++)
			{
				if(data[i] != NULL)
				{
					delete data[i];
				}
			}
			delete[] data;
		}
		
		*numMatrices = numClasses;
		return classVars;
	}
	
	rsgis::math::Matrix** RSGISZonalStats2Matrix::findPixelsForImageBand(GDALDataset **image, int numImgs, OGRLayer *shpfile, int *numMatrices, int band)
	{
		rsgis::math::Matrix **matrices = NULL;
		rsgis::img::RSGISCalcImageSingleValue *polyMatrix = NULL;
		rsgis::img::RSGISCalcImageSingle *calcPolyMatrix = NULL;
		rsgis::math::Matrix *matrix = NULL;
		RSGISPolygonData **polyData;
		
		RSGISVectorIO vecIO;
		rsgis::math::RSGISMatrices matrixUtils;
		rsgis::img::RSGISImageUtils imgUtils;
		
		int width = 0;
		int height = 0;
		int numFeatures = 0;
		int numVariables = 0;
		double *outputValue = new double[1];
		try
		{
			for(int i = 0; i < numImgs; i++)
			{
				numVariables += image[i]->GetRasterCount();
			}
			std::cout << "Raster Band Count = " << numVariables << std::endl;
			
			// READ IN SHAPEFILE
			numFeatures = shpfile->GetFeatureCount();
			std::cout << "Reading in " << numFeatures << " features\n";
			polyData = new RSGISPolygonData*[numFeatures];
			for(int i = 0; i < numFeatures; i++)
			{
				polyData[i] = new RSGISEmptyPolygon();
			}
			vecIO.readPolygons(shpfile, polyData, numFeatures);
			
			matrices = new rsgis::math::Matrix*[numFeatures];
			
			// Create a matrix for each polygon
			std::cout << "Populate the matrices of each polygon.\n";
			for(int i = 0; i < numFeatures; i++)
			{
				imgUtils.getImageOverlap(image, numImgs, &width, &height, polyData[i]->getBBox());
				polyMatrix = new rsgis::img::RSGISImageBand2Matrix(1, band, width, height);
				calcPolyMatrix = new rsgis::img::RSGISCalcImageSingle(polyMatrix);
				calcPolyMatrix->calcImageWithinPolygon(image, numImgs, outputValue, polyData[i]->getBBox(), polyData[i]->getPolygon(), false, rsgis::img::polyContainsPixelCenter);
				matrix = dynamic_cast<rsgis::img::RSGISImageBand2Matrix*>(polyMatrix)->getMatrix();
				matrices[i] = matrixUtils.copyMatrix(matrix);
				delete polyMatrix;
				delete calcPolyMatrix;
			}
		}
		catch(RSGISVectorException &e)
		{
			if(polyData != NULL)
			{
				for(int i = 0; i < numFeatures; i++)
				{
					if(polyData[i] != NULL)
					{
						delete polyData[i];
					}
				}
				delete[] polyData;
			}
			throw e;
		}
		catch(rsgis::img::RSGISImageBandException &e)
		{
			if(polyData != NULL)
			{
				for(int i = 0; i < numFeatures; i++)
				{
					if(polyData[i] != NULL)
					{
						delete polyData[i];
					}
				}
				delete[] polyData;
			}
			throw e;
		}
		catch(RSGISException &e)
		{
			if(polyData != NULL)
			{
				for(int i = 0; i < numFeatures; i++)
				{
					if(polyData[i] != NULL)
					{
						delete polyData[i];
					}
				}
				delete[] polyData;
			}			
			throw e;
		}
		
		if(polyData != NULL)
		{
			for(int i = 0; i < numFeatures; i++)
			{
				if(polyData[i] != NULL)
				{
					delete polyData[i];
				}
			}
			delete[] polyData;
		}
		
		*numMatrices = numFeatures;
		return matrices;
	}
	
	RSGISZonalStats2Matrix::~RSGISZonalStats2Matrix()
	{
		
	}
    
    RSGISPixelVals22Txt::RSGISPixelVals22Txt(GDALDataset *image, std::string outFileBase, std::string outNameHeading, math::outTXTform outType, rsgis::img::pixelInPolyOption method, unsigned int maxPrintout)
	{
		this->nImageBands = image->GetRasterCount(); // Get number of image bands
        this->datasets = new GDALDataset*[1];
		this->datasets[0] = image;
		this->method = method; // Set method for calculating pixel in polygon.
		this->outFileBase = outFileBase; // Set up base for out file.
        this->outType = outType; // Set up type for output data.
        this->data = new double[nImageBands]; // Set up array (not used here, but needed by image calc)
        this->outNameHeading = outNameHeading;
        this->outStatusText = "\n"; // Set up string for output text, print at end.
        // Set up structure for pixel values
        this->pixelValues = new std::vector<double>*[this->nImageBands];
        this->nFeatures = 0; // Count of number of features
        this->maxPrintout = maxPrintout; // Max features to print out
		
		for (int i = 0; i < this->nImageBands; i++)
		{
			this->pixelValues[i] = new std::vector<double>();
		}
        
        // Set up calc image
		calcValue = new RSGISCalcPixelValsFromPolygon(this->pixelValues, this->nImageBands);
		calcImage = new rsgis::img::RSGISCalcImageSingle(calcValue);
        
		
    }
	
	void RSGISPixelVals22Txt::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
	{
		// Zonal stats - output to text file.
		try
		{
            RSGISVectorUtils vecUtils;
			calcValue->reset(); // Reset values
            
            this->nFeatures++;
            
            OGRPolygon *inOGRPoly;
			geos::geom::Polygon *poly;
			inOGRPoly = (OGRPolygon *) feature->GetGeometryRef();
			poly = vecUtils.convertOGRPolygon2GEOSPolygon(inOGRPoly);
            
            // Get name from attribute table (unless FID, or it doesn't exist)
            std::string outPolyName = "";
            if(this->outNameHeading == "FID")
            {
                outPolyName = boost::lexical_cast<std::string>(fid);
            }
            else
            {
                OGRFeatureDefn *inFeatureDefn = feature->GetDefnRef();
                unsigned int fieldIdx = inFeatureDefn->GetFieldIndex(this->outNameHeading.c_str());
                if(fieldIdx > 0)
                {
                    outPolyName = feature->GetFieldAsString(fieldIdx);
                }
                else
                {
					std::string message = "This layer does not contain a field with the name \'" + this->outNameHeading + "\'\nTry using FID";
					throw RSGISVectorException(message.c_str());
                }
            }
			
            // Populate vector with pixel values
			calcImage->calcImageWithinPolygon(this->datasets, 1, this->data, env, poly, false, this->method);
                        
            
            // Write to text file
            if(this->pixelValues[0]->empty())
            {
                this->outStatusText =  this->outStatusText + "No pixels found for class: " + outPolyName + "\n";
            }
            else
            {
                std::string outExt = ".csv";
                if(this->outType == math::gtxt){outExt = ".gmtxt";}
                else if(this->outType == math::mtxt){outExt = ".mtxt";}
                
                std::string outTextFilePoly = this->outFileBase + outPolyName + outExt;
                this->outStatusText =  this->outStatusText + "Saved " + boost::lexical_cast<std::string>(this->pixelValues[0]->size()) + " pixels to: " + outTextFilePoly + "\n";
                std::ofstream outTxtFile;
                
                outTxtFile.open(outTextFilePoly.c_str(), std::ios::out | std::ios::trunc);
                
                if((this->outType == math::gtxt) | (this->outType == math::mtxt))
                {
                    outTxtFile << "m=" << this->nImageBands << std::endl;
                    outTxtFile << "n=" << this->pixelValues[0]->size() << std::endl;
                }
                else
                {
                    // Write header row for CSV
                    std::string outHeader = "";
                    for(unsigned int j = 0; j < this->nImageBands - 1; ++j)
					{
                        outHeader += "b" + boost::lexical_cast<std::string>(j+1) + ",";
                    }
                    outHeader += "b" + boost::lexical_cast<std::string>(this->nImageBands);
                    outTxtFile << outHeader << std::endl;
                }
                
                for (unsigned int i = 0; i < this->pixelValues[0]->size(); ++i)
                {
                    for(unsigned int j = 0; j < this->nImageBands - 1; ++j)
                    {
                        outTxtFile << this->pixelValues[j]->at(i) << ",";
                    }
                    if ((this->outType == math::mtxt) && (i == this->pixelValues[0]->size()-1))
                    {
                        // If very last element write newline instead of comma.
                        outTxtFile << this->pixelValues[this->nImageBands-1]->at(i) << std::endl;
                    }
                    else if(this->outType == math::mtxt)
                    {
                        // No newline for mtxt format.
                        outTxtFile << this->pixelValues[this->nImageBands-1]->at(i) << ",";
                    }
                    else
                    {
                        // Write newline instead of comma for last row.
                        outTxtFile << this->pixelValues[this->nImageBands-1]->at(i) << std::endl;
                    }
                }
                
                outTxtFile.flush();
                outTxtFile.close();
            }
		}
		
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}
	}
	
	RSGISPixelVals22Txt::~RSGISPixelVals22Txt()
	{
        // Print information to screen when finished, so as not to intefere with processing status display.
        if(this->nFeatures < this->maxPrintout){std::cout << this->outStatusText << std::endl;}
        delete this->calcValue;
        delete this->calcImage;
        delete this->data;
        delete[] this->pixelValues;
	}
    
    
    RSGISCalcPixelValsFromPolygon::RSGISCalcPixelValsFromPolygon(std::vector<double> **pixelValues, unsigned int numInBands) : rsgis::img::RSGISCalcImageSingleValue(numInBands)
	{
        this->pixelValues = pixelValues;
        this->numInBands = numInBands;
	}
	
	void RSGISCalcPixelValsFromPolygon::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) 
	{
		for(int i = 0; i < this->numInBands; i++)
		{
            if(!boost::math::isnan(bandValuesImage[i]))
            {
                this->pixelValues[i]->push_back(bandValuesImage[i]);
            }
		}
	}
	
	double* RSGISCalcPixelValsFromPolygon::getOutputValues() 
	{
        return outputValues;
	}
    
	void RSGISCalcPixelValsFromPolygon::reset()
	{
		for(int i = 0; i < this->numInBands; i++)
		{
			this->pixelValues[i]->clear();
		}
	}
	
	RSGISCalcPixelValsFromPolygon::~RSGISCalcPixelValsFromPolygon()
	{
	}
    
}}
