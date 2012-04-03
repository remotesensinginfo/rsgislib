/*
 *  RSGISFuzzyZonalStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/02/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
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

#include "RSGISFuzzyZonalStats.h"

namespace rsgis{namespace vec{
	
	RSGISFuzzyZonalStats::RSGISFuzzyZonalStats(GDALDataset *image, GDALDataset *rasterFeatures, FuzzyAttributes** attributes, int numAttributes, float binSize, float threshold, bool outPxlCount, string classattribute)
	{
		this->datasets = new GDALDataset*[2];
		this->datasets[0] = rasterFeatures;
		this->datasets[1] = image;
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->outPxlCount = outPxlCount;
		this->threshold = threshold;
		this->binSize = binSize;
		this->classattribute = classattribute;
		
		cout << "numAttributes = " << numAttributes << endl;
		
		dataSize = numAttributes + 1; // 0 Pxl Count // 1 ..  n Attribute data
		data = new double[dataSize];
		cout << "Data Size: " << dataSize << endl;
		
		calcValue = new RSGISCalcFuzzyZonalStatsFromRasterPolygon(dataSize, attributes, numAttributes, binSize, threshold);
		calcImage = new RSGISCalcImageSingle(calcValue);
		
		this->setupFuzzyAttributes();
	}
	
	void RSGISFuzzyZonalStats::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		try
		{
			int groupIdx = 0;
			bool hard = false;

			OGRFeatureDefn *featureDefn = inFeature->GetDefnRef();
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			
			int classFieldIndex = featureDefn->GetFieldIndex(classattribute.c_str());
			if(classFieldIndex < 0)
			{
				string message = "This layer does not contain a field with the name \'" + classattribute + "\'";
				throw RSGISVectorException(message.c_str());
			}
			
			string featureClassGroup = string(inFeature->GetFieldAsString(classFieldIndex));

			bool contained = false;
			for(unsigned int n = 0; n < classSets->size(); n++)
			{
				if(classSets->at(n)->name == featureClassGroup)
				{
					contained = true;
					groupIdx = n;
					if(featureClassGroup == "Hard")
					{
						hard = true;
					}
				}
			}
			
			
			if(!contained)
			{
				if(foundHard)
				{
					// use hard.
					groupIdx = hardGroupIndex;
					hard = true;
				}
				else
				{
					string message = "Hard not defined and unknown feature found: " + featureClassGroup;
					throw RSGISVectorException(message.c_str());
				}
			}
			
			
			calcValue->reset();
			calcValue->updateAttributes(groupedAttributes[classSets->at(groupIdx)->index], classSets->at(groupIdx)->count, hard);
			calcImage->calcImageWithinRasterPolygon(datasets, 2, data, env, fid, true);
			
			for(int i = 0; i < (dataSize-1); i++)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex(attributes[i]->name.c_str()), data[i+1]);
			}
			
			
			if(outPxlCount)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex("TotalPxls"), data[0]);
			}
		}
		catch(RSGISException& e)
		{
			throw RSGISVectorException(e.what());
		}
	}
	
	void RSGISFuzzyZonalStats::processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISFuzzyZonalStats::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		cout << "(dataSize-1) = " << (dataSize-1) << endl;
		for(int i = 0; i < (dataSize-1); i++)
		{
			if(attributes[i]->name.length() > 10)
			{
				cout << attributes[i]->name << " will be truncated to \'" << attributes[i]->name.substr(0, 10) << "\'\n";
				attributes[i]->name = attributes[i]->name.substr(0, 10);
			}
			OGRFieldDefn shpField(attributes[i]->name.c_str(), OFTReal);
			shpField.SetPrecision(10);
			if(outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				string message = string("Creating shapefile field ") + attributes[i]->name + string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
		
		if(outPxlCount)
		{
			OGRFieldDefn shpField("TotalPxls", OFTReal);
			shpField.SetPrecision(10);
			if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				string message = string("Creating shapefile field \'TotalPxls\' has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
		 
	}
	
	void RSGISFuzzyZonalStats::setupFuzzyAttributes()
	{
		classSets = new vector<FuzzyClassSet*>();
			
		int groupIdx = 0;
		
		string featureClassGroup = "";
		
		bool first = true;
		bool contained = false;
		FuzzyClassSet *tmpFuzzySet = NULL;
		
		hardGroupIndex = 0;
		
		for(int i = 0; i < numAttributes; i++)
		{
			if(first)
			{
				tmpFuzzySet = new FuzzyClassSet();
				tmpFuzzySet->name = attributes[i]->fuzzyClass;
				tmpFuzzySet->index = 0;
				tmpFuzzySet->count = 1;
				classSets->push_back(tmpFuzzySet);
				first = false;
			}
			else
			{
				contained = false;
				for(unsigned int j = 0; j < classSets->size(); j++)
				{
					if(classSets->at(j)->name == attributes[i]->fuzzyClass)
					{
						classSets->at(j)->count++;
						contained = true;
					}
				}
				if(!contained)
				{
					tmpFuzzySet = new FuzzyClassSet();
					tmpFuzzySet->name = attributes[i]->fuzzyClass;
					tmpFuzzySet->index = classSets->size();
					tmpFuzzySet->count = 1;
					classSets->push_back(tmpFuzzySet);
				}
			}
		}
		foundHard = false;
		cout << "The following class groups have been found: \n";
		for(unsigned int i = 0; i < classSets->size(); i++)
		{
			cout << i << ": " << classSets->at(i)->name << " index = " << classSets->at(i)->index << " count = " << classSets->at(i)->count << endl;
			if(classSets->at(i)->name == "Hard")
			{
				hardGroupIndex = i;
				foundHard = true;
			}
		}
		if(!foundHard)
		{
			cout << "No \'Hard\' group found, therefore if are any unknown groups objects an error will be thrown.\n";
		}
		else
		{
			cout << "\'Hard\' group found, all unknown groups objects will be asigned to this.\n";
		}
		
		groupedAttributes = new FuzzyAttributes**[classSets->size()];
		groupedIndexes = new int[classSets->size()];
		for(unsigned int i = 0; i < classSets->size(); i++)
		{
			groupedAttributes[i] = new FuzzyAttributes*[classSets->at(i)->count];
			groupedIndexes[i] = 0;
		}
		
		groupIdx = 0;
		for(int i = 0; i < numAttributes; i++)
		{
			contained = false;
			for(unsigned int j = 0; j < classSets->size(); j++)
			{
				if(classSets->at(j)->name == attributes[i]->fuzzyClass)
				{
					groupIdx = classSets->at(j)->index;
					contained = true;
				}
			}
			
			if(!contained)
			{
				throw RSGISVectorException("Class group not recognised.");
			}
			
			groupedAttributes[groupIdx][groupedIndexes[groupIdx]++] = attributes[i];
		}
		
		for(unsigned int i = 0; i < classSets->size(); i++)
		{
			cout << " *** " << classSets->at(i)->name << " *** \n";
			for(int j = 0; j < classSets->at(i)->count; j++)
			{
				cout << j << ") " << groupedAttributes[i][j]->name << " - " << groupedAttributes[i][j]->fuzzyClass << endl;
			}
		}
	}
	
	RSGISFuzzyZonalStats::~RSGISFuzzyZonalStats()
	{
		for(unsigned int i = 0; i < classSets->size(); i++)
		{
			delete[] groupedAttributes[i];
		}		
		delete[] groupedAttributes;

		FuzzyClassSet *tmpFuzzySet = NULL;
		vector<FuzzyClassSet*>::iterator iter;
    		for( iter = classSets->begin(); iter != classSets->end(); iter++ ) 
		{
			tmpFuzzySet = *iter;
      			//classSets->erase();
			delete tmpFuzzySet;
    		}

		classSets->clear();
		delete classSets;
		delete[] groupedIndexes;		

		delete[] datasets;
		delete[] data;
		
		delete calcValue;
		delete calcImage;
	}
	
	
	RSGISCalcFuzzyZonalStatsFromRasterPolygon::RSGISCalcFuzzyZonalStatsFromRasterPolygon(int numOutputValues, FuzzyAttributes **attributes, int numAttributes, float binsize, float hardThreshold) : RSGISCalcImageSingleValue(numOutputValues)
	{
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->binsize = binsize;
		this->hardThreshold = hardThreshold;
		
		this->numPxls = 0;
		
		float lowerValue = 0 - (binsize/2);
		float upperValue = 1 + (binsize/2);
		float value = lowerValue;
		int count = 0;
		while(value < upperValue)
		{
			value += binsize;
			count++;
		}
		this->numBins = count;
		
		cout << "Number of Bins = " << this->numBins << endl;
		
		value = lowerValue;
		this->binRange = new float[numBins+1];
		for(int i = 0; i < numBins; i++)
		{
			binRange[i] = value;
			value += binsize;
		}
		binRange[numBins] = upperValue;
		
		for(int i = 0; i < numBins; i++)
		{
			cout << i << ")\t" << binRange[i] << " - " << binRange[i+1] << endl;
		}
		
		histograms = new int*[(this->numOutputValues-1)];
		for(int i = 0; i < (this->numOutputValues-1); i++)
		{
			histograms[i] = new int[numBins];
			for(int j = 0; j < numBins; j++)
			{
				histograms[i][j] = 0;
			}
		}
	}
	
	void RSGISCalcFuzzyZonalStatsFromRasterPolygon::calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISCalcFuzzyZonalStatsFromRasterPolygon::calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException)
	{
		float min = 0;
		int histogramBin = 0;
		bool histogramBinFound = false;
		bool first = true;
		
		for(int i = 0; i < numAttributes; i++)
		{
			// Get Value (and min)
			min = 0;
			first = true;
			for(int j = 0; j < attributes[i]->numBands; j++)
			{
				if(attributes[i]->bands[j] > (numBands-1) | attributes[i]->bands[j] < 0)
				{
					throw RSGISImageCalcException("The band attributes do not match the image.");
				}
				if(first)
				{
					min = bandValuesImage[attributes[i]->bands[j]];
					first = false;
				}
				else
				{
					if(bandValuesImage[attributes[i]->bands[j]] < min)
					{
						min = bandValuesImage[attributes[i]->bands[j]];
					}
				}
			}
			histogramBinFound = false;
			for(int j = 0; j < this->numBins; j++)
			{
				if(min > binRange[j] & min <= binRange[j+1])
				{
					histogramBin = j;
					histogramBinFound = true;
				}
			}
			if(!histogramBinFound)
			{
				throw RSGISImageCalcException("Value outside histogram range (0 - 1)");
			}
			
			histograms[attributes[i]->index][histogramBin]++;
		}
		numPxls++;
	}
	
	void RSGISCalcFuzzyZonalStatsFromRasterPolygon::calcImageValue(float *bandValuesImage, int numBands, Envelope *extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISCalcFuzzyZonalStatsFromRasterPolygon::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	double* RSGISCalcFuzzyZonalStatsFromRasterPolygon::getOutputValues() throw(RSGISImageCalcException)
	{
		for(int i = 0; i < this->numOutputValues; i++)
		{
			outputValues[i] = 0;
		}
		
		for(int i = 0; i < this->numAttributes; i++)
		{
			if((attributes[i]->index+1) < 0 | (attributes[i]->index+1) >= this->numOutputValues)
			{
				throw RSGISImageCalcException("Attribute index not within range.");
			}
			
			//cout << attributes[i]->name << ": [" << attributes[i]->index+1 << "] ";
			outputValues[attributes[i]->index+1] = this->calcHistogramCentre(histograms[attributes[i]->index]);
			if(hard)
			{
				if(outputValues[attributes[i]->index+1] > hardThreshold)
				{
					outputValues[attributes[i]->index+1] = 1;
				}
			}
		}
		
		outputValues[0] = numPxls;
		return outputValues;
	}
	
	void RSGISCalcFuzzyZonalStatsFromRasterPolygon::reset()
	{
		numPxls = 0;
		for(int i = 0; i < (this->numOutputValues-1); i++)
		{
			for(int j = 0; j < numBins; j++)
			{
				histograms[i][j] = 0;
			}
		}
	}
	
	void RSGISCalcFuzzyZonalStatsFromRasterPolygon::updateAttributes(FuzzyAttributes **attributes, int numAttributes, bool hard)
	{
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->hard = hard;
	}
	
	float RSGISCalcFuzzyZonalStatsFromRasterPolygon::calcHistogramCentre(int *histogram)
	{
		float *binValue = new float[numBins];
		float *normHist = new float[numBins];
		float value = 0;

		//cout << "Histogram: ";
		for(int i = 0; i < numBins; i++)
		{
			binValue[i] = value;
			normHist[i] = ((float)histogram[i])/numPxls;
			//cout << normHist[i] << " ";
			value = value + 0.1;
		}
		//cout << endl;
		
		float min = 0;
		float minValue = 0;
		int numSteps = numBins * 10;
		
		float stepIncrement = binsize/10;
		float stepValue = 0 - (binsize/2);
		float lowerWeight = 0;
		float upperWeight = 0;
		float balance = 0;
		float distance = 0;
		
		for(int i = 0; i < numSteps; i++)
		{
			lowerWeight = 0;
			upperWeight = 0;
			for(int j = 0; j < numBins; j++)
			{
				if(binValue[j] <= stepValue)
				{
					distance = (stepValue - binValue[j]);
					//cout << "Lower: " << binValue[j] << ", " << stepValue << " Distance = " << distance << endl;
					lowerWeight += distance * normHist[j];
				}
				else
				{
					distance = (binValue[j] - stepValue);
					//cout << "Upper: " << binValue[j] << ", " << stepValue << " Distance = " << distance << endl;
					upperWeight += distance * normHist[j];
				}
			}
			balance = (lowerWeight - upperWeight) * (lowerWeight - upperWeight);
			//cout << stepValue << ":\t" << balance << endl;
			if( i == 0)
			{
				min = balance;
				minValue = stepValue;
			}
			else
			{
				if(balance < min)
				{
					min = balance;
					minValue = stepValue;
				}
			}
			stepValue += stepIncrement;
		}
		
		delete[] binValue;
		delete[] normHist;
		
		//cout << " - MIN VALUE = " << minValue << endl;
		
		return minValue;
	}
	
	RSGISCalcFuzzyZonalStatsFromRasterPolygon::~RSGISCalcFuzzyZonalStatsFromRasterPolygon()
	{
		delete[] binRange;
		for(int i = 0; i < this->numOutputValues-1; i++)
		{
			delete[] histograms[i];
		}
		delete[] histograms;
	}
}}


