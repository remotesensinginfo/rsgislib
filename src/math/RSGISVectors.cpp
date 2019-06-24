/*
 *  RSGISVectors.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 21/01/2009.
 *  Copyright 2009 RSGISLib.
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

#include "RSGISVectors.h"

namespace rsgis{namespace math{
	
	
	RSGISVectors::RSGISVectors()
	{
		
	}
	
	Vector* RSGISVectors::createVector(int n)
	{
		if(n < 1)
		{
			throw RSGISVectorsException("Size n must be at least 1.");
		}
		Vector *vector = new Vector();
		vector->n = n;
		vector->vector = new double[n];
		
		int length = n;
		
		for(int i = 0; i < length; i++)
		{
			vector->vector[i] = 0;
		}
		return vector;
	}
		
	void RSGISVectors::freeVector(Vector *vector)
	{
		if(vector != NULL)
		{
			if(vector->vector != NULL)
			{
				delete[] vector->vector;
			}
			delete vector;
		}
	}
	
	void RSGISVectors::printVector(Vector *vector)
	{		
		int index = 0;
		
		for(int j = 0; j < vector->n; j++)
		{
			std::cout << vector->vector[index++] << " ";
		}
		std::cout << std::endl;
	}
	
	void RSGISVectors::printGSLVector(gsl_vector *vector)
	{
		for(unsigned int j = 0; j < vector->size; j++)
		{
			double outm = gsl_vector_get(vector, j); 
			std::cout << outm << " ";
		}
		std::cout << std::endl;		
	}
	
	void RSGISVectors::saveVector2GridTxt(Vector *vector, std::string filepath)
	{
		std::string outputFilename = filepath + std::string(".gmtxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile << "n=" << vector->n << std::endl;
			
			int totalElements = vector->n;
			int lastElement = totalElements-1;
			for(int i = 0; i < totalElements; i++)
			{
				if(i %  vector->n == 0)
				{
					outTxtFile << std::endl;
				}
				if(i == lastElement)
				{
					outTxtFile << vector->vector[i];
				}
				else
				{
					outTxtFile << vector->vector[i] << ",";
				}
			}
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}
		
	}
	
	void RSGISVectors::saveVector2txt(Vector *vector, std::string filepath)
	{
		std::string outputFilename = filepath + std::string(".mtxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile << "n=" << vector->n << std::endl;
			
			int totalElements = vector->n;
			int lastElement = totalElements-1;
			for(int i = 0; i < totalElements; i++)
			{
				if(i == lastElement)
				{
					outTxtFile << vector->vector[i];
				}
				else
				{
					outTxtFile << vector->vector[i] << ",";
				}
			}
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		} 
	}
	
	void RSGISVectors::saveGSLVector2GridTxt(gsl_vector *gslVector, std::string filepath)
	{
		Vector *rsgisVector;
		rsgisVector = this->convertGSL2RSGISVector(gslVector);
		this->saveVector2GridTxt(rsgisVector, filepath);
		this->freeVector(rsgisVector);
	}
	
	void RSGISVectors::saveGSLVector2Txt(gsl_vector *gslVector, std::string filepath)
	{
		Vector *rsgisVector;
		rsgisVector = this->convertGSL2RSGISVector(gslVector);
		this->saveVector2txt(rsgisVector, filepath);
		this->freeVector(rsgisVector);
	}	
	
	Vector* RSGISVectors::readVectorFromTxt(std::string filepath)
	{
		Vector *vector = new Vector();
		std::ifstream inputVector;
		inputVector.open(filepath.c_str());
		if(!inputVector.is_open())
		{
			throw RSGISInputStreamException("Could not open input text file.");
		}
		else
		{
			std::string strLine;
			std::string word;
			int number;
			float value;
			int lineCounter = 0;
			inputVector.seekg(std::ios_base::beg);
			while(!inputVector.eof())
			{
				getline(inputVector, strLine, '\n');
				if(strLine.length() > 0)
				{
					if(lineCounter == 0)
					{
						// n
						word = strLine.substr(2);
						number = strtol(word.c_str(), NULL, 10);
						vector->n = number;
					}
					else if(lineCounter == 1)
					{
						// data
						int dataCounter = 0;
						int start = 0;
						int lineLength = strLine.length();
						int numDataPoints = vector->n;
						vector->vector = new double[numDataPoints];
						for(int i = 0; i < lineLength; i++)
						{
							if(strLine.at(i) == ',')
							{
								word = strLine.substr(start, i-start);								
								value = strtod(word.c_str(), NULL);
								vector->vector[dataCounter] = value;
								dataCounter++;
								
								start = start + i-start+1;
							}
							
							if(dataCounter >= numDataPoints)
							{
								throw RSGISVectorsException("Too many data values, compared to header.");
							}
						}
						word = strLine.substr(start);
						value = strtod(word.c_str(), NULL);
						vector->vector[dataCounter] = value;
						dataCounter++;
						
						if(dataCounter != (vector->n))
						{
							throw RSGISVectorsException("An incorrect number of data points were read in.");
						}
						
					}
					else
					{
						break;
					}
				}
				lineCounter++;
			}
			
			if(lineCounter < 2)
			{
				throw RSGISVectorsException("A complete vector has not been reconstructed.");
			}
			inputVector.close();
		}
		return vector;
	}
	
	Vector* RSGISVectors::readVectorFromGridTxt(std::string filepath)
	{
		Vector *vector = new Vector();
		std::ifstream inputVector;
		inputVector.open(filepath.c_str());
		if(!inputVector.is_open())
		{
			throw RSGISInputStreamException("Could not open input text file.");
		}
		else
		{
			std::string strLine;
			std::string wholeline;
			std::string word;
			int number;
			float value;
			int lineCounter = 0;
			bool first = true;
			inputVector.seekg(std::ios_base::beg);
			while(!inputVector.eof())
			{
				getline(inputVector, strLine, '\n');
				if(strLine.length() > 0)
				{

					if(lineCounter == 0)
					{
						// n
						word = strLine.substr(2);
						number = strtol(word.c_str(), NULL, 10);
						vector->n = number;
					}
					else 
					{
						if(first)
						{
							wholeline = strLine;
							first = false;
						}
						else 
						{
							wholeline = wholeline + std::string(",") + strLine;
						}
					}
					lineCounter++;
				}
			}
			inputVector.close();
			
			// data
			int dataCounter = 0;
			int start = 0;
			int lineLength = wholeline.length();;
			int numDataPoints = vector->n;
			vector->vector = new double[numDataPoints];
			
			for(int i = 0; i < lineLength; i++)
			{
				if(wholeline.at(i) == ',')
				{
					word = wholeline.substr(start, i-start);								
					value = strtod(word.c_str(), NULL);
					vector->vector[dataCounter] = value;
					dataCounter++;
					
					start = start + i-start+1;
				}
				
				if(dataCounter >= numDataPoints)
				{
					throw RSGISVectorsException("Too many data values, compared to header.");
				}
			}
			
			word = wholeline.substr(start);
			value = strtod(word.c_str(), NULL);
			vector->vector[dataCounter] = value;
			dataCounter++;
			
			if(dataCounter != (vector->n))
			{
				throw RSGISVectorsException("An incorrect number of data points were read in.");
			}
		}
		return vector;
	}
	
	gsl_vector* RSGISVectors::readGSLVectorFromTxt(std::string filepath)
	{
		Vector *rsgisVector;
		gsl_vector *gslVector;
		rsgisVector = this->readVectorFromTxt(filepath);
		gslVector = this->convertRSGIS2GSLVector(rsgisVector);
		this->freeVector(rsgisVector);
		return gslVector;
	}
	
	gsl_vector* RSGISVectors::readGSLVectorFromGridTxt(std::string filepath)
	{
		Vector *rsgisVector;
		gsl_vector *gslVector;
		rsgisVector = this->readVectorFromGridTxt(filepath);
		gslVector = this->convertRSGIS2GSLVector(rsgisVector);
		this->freeVector(rsgisVector);
		return gslVector;
	}
	
	gsl_vector* RSGISVectors::convertRSGIS2GSLVector(Vector *inVector)
	{
		gsl_vector *convertedVector = gsl_vector_alloc(inVector->n);
		
		int index = 0;

		for(int j = 0; j < inVector->n; j++)
		{
			double velement = inVector->vector[index++];
			gsl_vector_set(convertedVector,j,velement);
		}
		
		return convertedVector;
	}

	Vector* RSGISVectors::convertGSL2RSGISVector(gsl_vector *inVector)
	{
		/// Converts GSL vector to a RSGIS vector
		Vector *convertedVector = this->createVector(inVector->size);		
		int index = 0;
		for(unsigned int i = 0; i < inVector->size; i++)
		{
			double velement = gsl_vector_get(inVector, i);
			convertedVector->vector[index++] = velement; 
		}
		
		return convertedVector;
	}
	
	double RSGISVectors::dotProductVectorVectorGSL(gsl_vector *inVectorA, gsl_vector *inVectorB)
	{
		/// Calculates the dot product of two vectors.
		/**
		* For an explantion of dot products see <a href="http://mathworld.wolfram.com/DotProduct.html">
		*/
		if(inVectorA->size != inVectorB->size)
		{
			throw RSGISVectorsException("Vectors are not of equal length");
		}
		
		double outValue = 0;
		
		for(unsigned int i = 0; i < inVectorA->size; i++)
		{
			double elementA = gsl_vector_get(inVectorA, i);
			double elementB = gsl_vector_get(inVectorB, i);
			outValue = outValue + (elementA * elementB);
		}
		
		return outValue;
		
	}

	gsl_vector* RSGISVectors::crossProductVectorVectorGSL(gsl_vector *inVectorA, gsl_vector *inVectorB)
	{
		/// Calculates the cross product of two vectors.
		/**
		 * For an explantion of cross products see <a href="http://mathworld.wolfram.com/CrossProduct.html">
		 */
		return NULL;
	}
	
	double RSGISVectors::euclideanDistance(Vector *vecA, Vector *vecB)
	{
		if(vecA->n != vecB->n)
		{
			throw RSGISVectorsException("Vectors need to be of the same length for the distance to be calculated.");
		}
		
		double sum = 0;
		
		for(int i = 0; i < vecA->n; ++i)
		{
			sum += ((vecA->vector[i] - vecB->vector[i])*(vecA->vector[i] - vecB->vector[i]));
		}
		
		double distance = sqrt(sum/vecA->n);
		return distance;
	}
	
	RSGISVectors::~RSGISVectors()
	{
		
	}
}}
