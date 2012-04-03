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
	
	ClassVariables** RSGISZonalStats2Matrix::findPixelStats(GDALDataset **image, int numImgs, OGRLayer *vecLayer, string classAttribute, int *numMatrices, pixelInPolyOption method)
	{		
		RSGISCalcImageSingleValue *polyPxlCount = NULL;
		ClassVariables **classVars = NULL;
		RSGISCalcImageSingle *calcImageValue;
		RSGISCalcImageSingleValue *polyMatrix = NULL;
		RSGISCalcImageSingle *calcPolyMatrix = NULL;

		Matrix *matrix = NULL;
		int *matrixPter = NULL;
		RSGISPolygonData **polyData = NULL;
		RSGISClassPolygon **data = NULL;
		
		vector<string> *classNames = new vector<string>();
		double *numPxls = new double[1];
		
		RSGISVectorIO vecIO;
		RSGISMatrices matrixUtils;
		
		int numVariables = 0;
		int numFeatures = 0;
		int numClasses = 0;
		try
		{
			for(int i = 0; i < numImgs; i++)
			{
				numVariables += image[i]->GetRasterCount();
			}
			cout << "Raster Band Count = " << numVariables << endl;
			
			// READ IN SHAPEFILE
			numFeatures = vecLayer->GetFeatureCount();
			polyData = new RSGISPolygonData*[numFeatures];
			for(int i = 0; i < numFeatures; i++)
			{
				polyData[i] = new RSGISClassPolygon(classAttribute);
			}
			cout << "Reading in " << numFeatures << " features\n";
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
			cout << "Vector file contains " << numClasses << " classes:\n";
			/*for(int i = 0; i < numClasses; i++)
			{
				cout << i << ") " << classNames->at(i) << endl;
			}*/
			
			// Count the number of pixels within each polygon
			cout << "Count the number of pixels within each polygon\n";
			polyPxlCount = new RSGISPolygonPixelCount(1);
			calcImageValue = new RSGISCalcImageSingle(polyPxlCount);
			for(int i = 0; i < numFeatures; i++)
			{
				polyPxlCount->reset();
				calcImageValue->calcImageWithinPolygon(image, numImgs, numPxls, data[i]->getBBox(), data[i]->getPolygon(), true, method);
				data[i]->setNumPixels(ceil(numPxls[0]));
			}
			
			/*
			 for(int i = 0; i < numFeatures; i++)
			 {
				 cout << "Polygon " << i << " of class " << data[i]->getClassName() << " has " << data[i]->getNumPixels()  << " pixels" << endl;
				 cout << "Envelope: [" << data[i]->getBBox()->getMinX() << "," << data[i]->getBBox()->getMaxX() << "][" << data[i]->getBBox()->getMinY() << "," << data[i]->getBBox()->getMaxY() << "]\n";
			 }*/
			
			// Create a matrix for each polygon
			cout << "Populate the matrices of each polygon.\n";
			for(int i = 0; i < numFeatures; i++)
			{
				polyMatrix = new RSGISPopulateMatrix(numVariables, data[i]->getNumPixels());
				calcPolyMatrix = new RSGISCalcImageSingle(polyMatrix);
				//cout << "Feature " << i << endl;
				//cout << "Populate matrix\n";
				calcPolyMatrix->calcImageWithinPolygon(image, numImgs, numPxls, data[i]->getBBox(), data[i]->getPolygon(), false, polyContainsPixelCenter);
				//cout << "Get Matrix\n";
				matrix = dynamic_cast<RSGISPopulateMatrix*>(polyMatrix)->getMatrix();
				//cout << "set matrix to polygon\n";
				data[i]->setPixelValues(matrix);
				//cout << "Delete\n";
				delete polyMatrix;
				delete calcPolyMatrix;
			}
			
			/*
			 for(int i = 0; i < numFeatures; i++)
			 {
				cout << "Polygon " << i << " of class " << data[i]->getClassName() << " has " << data[i]->getNumPixels()  << " pixels" << endl;
				matrixUtils.printMatrix(data[i]->getPixelValues());
			 }*/
			
			
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
				cout << i << ") " << classVars[i]->name << " has a total number of pixels = " << classVars[i]->numPxls << endl;
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
			
			/*for(int i = 0; i < numClasses; i++)
			 {
			 cout << i << ") " << classVars[i]->name << " has a total number of pixels = " << classVars[i]->numPxls << endl;
			 matrixUtils.printMatrix(classVars[i]->matrix);
			 }*/
		}
		catch(RSGISVectorException e)
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
		catch(RSGISImageCalcException e)
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
		catch(RSGISImageBandException e)
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
		catch(RSGISException e)
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
	
	Matrix** RSGISZonalStats2Matrix::findPixelsForImageBand(GDALDataset **image, int numImgs, OGRLayer *shpfile, int *numMatrices, int band)
	{
		Matrix **matrices = NULL;
		RSGISCalcImageSingleValue *polyMatrix = NULL;
		RSGISCalcImageSingle *calcPolyMatrix = NULL;
		Matrix *matrix = NULL;
		RSGISPolygonData **polyData;
		
		RSGISVectorIO vecIO;
		RSGISMatrices matrixUtils;
		RSGISImageUtils imgUtils;
		
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
			cout << "Raster Band Count = " << numVariables << endl;
			
			// READ IN SHAPEFILE
			numFeatures = shpfile->GetFeatureCount();
			cout << "Reading in " << numFeatures << " features\n";
			polyData = new RSGISPolygonData*[numFeatures];
			for(int i = 0; i < numFeatures; i++)
			{
				polyData[i] = new RSGISEmptyPolygon();
			}
			vecIO.readPolygons(shpfile, polyData, numFeatures);
			
			matrices = new Matrix*[numFeatures];
			
			// Create a matrix for each polygon
			cout << "Populate the matrices of each polygon.\n";
			for(int i = 0; i < numFeatures; i++)
			{
				imgUtils.getImageOverlap(image, numImgs, &width, &height, polyData[i]->getBBox());
				polyMatrix = new RSGISImageBand2Matrix(1, band, width, height);
				calcPolyMatrix = new RSGISCalcImageSingle(polyMatrix);
				calcPolyMatrix->calcImageWithinPolygon(image, numImgs, outputValue, polyData[i]->getBBox(), polyData[i]->getPolygon(), false, polyContainsPixelCenter);
				matrix = dynamic_cast<RSGISImageBand2Matrix*>(polyMatrix)->getMatrix();
				matrices[i] = matrixUtils.copyMatrix(matrix);
				delete polyMatrix;
				delete calcPolyMatrix;
			}
			
			/*
			for(int i = 0; i < numFeatures; i++)
			{
				cout << "Polygon " << i << endl;
				matrixUtils.printMatrix(matrices[i]);
			}*/
			
		}
		catch(RSGISVectorException e)
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
		catch(RSGISImageBandException e)
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
		catch(RSGISException e)
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
}}
