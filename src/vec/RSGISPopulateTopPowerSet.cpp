/*
 *  RSGISPopulateTopPowerSet.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/03/2009.
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

#include "RSGISPopulateTopPowerSet.h"

namespace rsgis{namespace vec{
	
	RSGISPopulateTopPowerSet::RSGISPopulateTopPowerSet(GDALDataset *image, GDALDataset *rasterFeatures, CountAttributes** attributes, int numAttributes, int nTop)
	{
		this->datasets = new GDALDataset*[2];
		this->datasets[0] = rasterFeatures;
		this->datasets[1] = image;
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->nTop = nTop;
	}
	
	void RSGISPopulateTopPowerSet::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		RSGISMathsUtils mathUtils;
		
		OGRFeatureDefn *featureDefn = inFeature->GetDefnRef();
		OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
		
		string *topAttributes = NULL;
		CountAttributes **powerSetAttributes = NULL;
		
		string nullStr = "NULL";
		string name = "";
		string fieldValue = "";
		int numPowerSetAttributes = 0;
		int value = 0;
		int numTopValues = 0;
		
		try
		{
			//cout << "fid: " << fid << ": \n";
			
			for(int j = 0; j < nTop; j++)
			{
				name = "Top_" + mathUtils.inttostring(j+1);
				fieldValue = string(inFeature->GetFieldAsString(featureDefn->GetFieldIndex(name.c_str())));
				if(fieldValue != nullStr)
				{
					numTopValues++;
				}
				else
				{
					break;
				}
			}
			
			//cout << "numTopValues = " << numTopValues << endl;

			
			
			if(numTopValues > 0)
			{
				
				topAttributes = new string[numTopValues];
				for(int j = 0; j < numTopValues; j++)
				{
					name = "Top_" + mathUtils.inttostring(j+1);
					topAttributes[j] = string(inFeature->GetFieldAsString(featureDefn->GetFieldIndex(name.c_str())));
					//cout << "Attribute " << j+1 << ": " << topAttributes[j] << " = " << string(inFeature->GetFieldAsString(featureDefn->GetFieldIndex(topAttributes[j].c_str()))) << endl;
					
				}
				
				if(numTopValues == 1)
				{
					numPowerSetAttributes = 1;
				}
				else
				{
					value = 2;
					for(int i = 0; i < (numTopValues-1); i++)
					{
						value = value * 2;
					}
					numPowerSetAttributes = value - 1;
				}
				powerSetAttributes = new CountAttributes*[numPowerSetAttributes];
				
				//cout << "Number of Power Set Attributes = " << numPowerSetAttributes << endl;
				
				//cout << "identifying powerset\n";
				this->identifyPowerSet(attributes, numAttributes, topAttributes, numTopValues, powerSetAttributes, numPowerSetAttributes);
				//cout << "identified powerset\n";
				
				/*
				for(int j = 0; j < numPowerSetAttributes; j++)
				{
					cout << j << ": " << flush;
					cout << powerSetAttributes[j]->name << ": [";
					for(int k = 0; k < powerSetAttributes[j]->numBands; k++)
					{
						if(k == 0)
						{
							cout << powerSetAttributes[j]->bands[k] << "(" << powerSetAttributes[j]->thresholds[k] << ")";
						}
						else
						{
							cout << " " << powerSetAttributes[j]->bands[k] << "(" << powerSetAttributes[j]->thresholds[k] << ")";
						}
					}
					cout << "]\n";
				}
				*/
				
				dataSize = numPowerSetAttributes + 2; // 0 Pxl Count // 1 Empty Set // 2 ..  n Attribute data
				data = new double[dataSize];
				
				calcValue = new RSGISCalcZonalCountFromRasterPolygon(dataSize, powerSetAttributes, numPowerSetAttributes);
				calcImage = new RSGISCalcImageSingle(calcValue);
				
				// Calculate Stats HERE!!
				//calcValue->reset();
				calcImage->calcImageWithinRasterPolygon(datasets, 2, data, env, fid, true);
				
				//cout << "Number of Pixels = " << data[0] << endl;
				
				int count = 1;
				for(int j = 0; j < numPowerSetAttributes; j++)
				{
					name = "PS_" + mathUtils.inttostring(count) + "_Desc";
					outFeature->SetField(outFeatureDefn->GetFieldIndex(name.c_str()), powerSetAttributes[j]->name.c_str());
					
					//cout << "name = " << name << " output: " << powerSetAttributes[j]->name.c_str() << " = " << data[j+2] << endl;
					
					name = "PS_" + mathUtils.inttostring(count) + "_Sum";
					outFeature->SetField(outFeatureDefn->GetFieldIndex(name.c_str()), data[j+2]);
					
					count++;
				}
				
				name = "PS_" + mathUtils.inttostring(count) + "_Desc";
				outFeature->SetField(outFeatureDefn->GetFieldIndex(name.c_str()), "Empty");
				
				//cout << "name = " << name << " output: Empty = " << data[1] << endl;
				
				name = "PS_" + mathUtils.inttostring(count) + "_Sum";
				outFeature->SetField(outFeatureDefn->GetFieldIndex(name.c_str()), (data[1]));
				
				delete calcImage;
				delete calcValue;
				delete[] topAttributes;
				delete[] data;
				
				for(int i = 0; i < numPowerSetAttributes; i++)
				{
					delete[] powerSetAttributes[i]->bands;
					delete[] powerSetAttributes[i]->thresholds;
					delete powerSetAttributes[i];
				}
				delete[] powerSetAttributes;
				//cout << endl;
			}
			else
			{
				//cout << "No Top attributes.. continuing...\n";
				
				//ELSE PROVIDE PIXEL COUNT AS EMPTY SET....
				
				dataSize = 1; // 0 Pxl Count
				data = new double[dataSize];
				
				calcValue = new RSGISCalcPixelCountFromRasterPolygon(dataSize);
				calcImage = new RSGISCalcImageSingle(calcValue);
				
				calcImage->calcImageWithinRasterPolygon(datasets, 2, data, env, fid, true);
				
				name = "PS_1_Desc";
				outFeature->SetField(outFeatureDefn->GetFieldIndex(name.c_str()), "Empty");
				
				//cout << "name = " << name << " output: " << powerSetAttributes[j]->name.c_str() << " = " << data[j+2] << endl;
				
				name = "PS_1_Sum";
				outFeature->SetField(outFeatureDefn->GetFieldIndex(name.c_str()), data[0]);
				
				delete calcImage;
				delete calcValue;
				delete[] data;
			}

		}
		catch(RSGISMathException& e)
		{
			throw RSGISVectorException(string(e.what()).c_str());
		}
		catch(RSGISVectorException& e)
		{
			throw e;
		}
	}
	
	void RSGISPopulateTopPowerSet::processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISPopulateTopPowerSet::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		RSGISMathsUtils mathUtils;
		string name = "";
		
		int numPowerSetAttributes = 2;
		
		if(nTop == 0)
		{
			throw RSGISVectorOutputException("Number of top attributes equals zero, cannot create powerset.");
		}
		else if(nTop == 1)
		{
			numPowerSetAttributes = 1;
		}
		else
		{
			numPowerSetAttributes = 2;
			for(int i = 0; i < (nTop-1); i++)
			{
				numPowerSetAttributes = numPowerSetAttributes * 2;
			}
			
		}
		
		cout << "numPowerSetAttributes: " << numPowerSetAttributes << endl;
		
		for(int i = 0; i < numPowerSetAttributes; i++)
		{
			name = "PS_" + mathUtils.inttostring(i+1) + "_Desc";
			OGRFieldDefn shpField(name.c_str(), OFTString);
			shpField.SetWidth(100);
			if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				string message = string("Creating shapefile field") + name + string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
			
			name = "PS_" + mathUtils.inttostring(i+1) + "_Sum";
			OGRFieldDefn shpFieldSummary(name.c_str(), OFTReal);
			shpFieldSummary.SetPrecision(10);
			if( outputLayer->CreateField( &shpFieldSummary ) != OGRERR_NONE )
			{
				string message = string("Creating shapefile field") + name + string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
	}
	
	void RSGISPopulateTopPowerSet::identifyPowerSet(CountAttributes** allAttributes, int numAttributes, string *topAttributeNames, int numTop, CountAttributes** powerSetAttributes, int numPSAttributes) throw(RSGISMathException,RSGISVectorException)
	{
		RSGISMathsUtils mathUtils;
		int **indexes = NULL;
		int *numIndexes = NULL;
		int numSets = 0;
		int numBands = 0;
		int count = 0;
		
		bool found = false;
		CountAttributes **topAttributes = NULL;
		
		string name;
		
		try
		{
			if(numTop == 0)
			{
				throw RSGISMathException("Number of top attributes equals zero, cannot create powerset.");
			}
			else if(numTop == 1)
			{
				numSets = 1;
			}
			else
			{
				int value = 2;
				for(int i = 0; i < (numTop-1); i++)
				{
					value = value * 2;
				}
				numSets = value - 1;
			}
			
			topAttributes = new CountAttributes*[numTop];
			
			//cout << "number of sets = " << numSets << endl;
			
			if(numSets != numPSAttributes)
			{
				throw RSGISMathException("Inconsistance in the number of sets defined.");
			}
			
			for(int i = 0; i < numTop; i++)
			{
				found = false;
				for(int j = 0; j < numAttributes; j++)
				{
					if(allAttributes[j]->name == topAttributeNames[i])
					{
						topAttributes[i] = allAttributes[j];
						found = true;
						break;
					}
				}
				if(!found)
				{
					string message = "Attribute \'" + topAttributeNames[i] + "\' was not found in list.";
					throw RSGISVectorException(message.c_str());
				}
			}
			
			numIndexes = new int[numSets];
			indexes = new int*[numSets];
			
			mathUtils.getPowerSet(numTop, numIndexes, indexes, numSets);
			
			for(int i = 0; i < numSets; i++)
			{
				name = "";
				for(int j = 0; j < numIndexes[i]; j++)
				{
					if(j == 0)
					{
						name = topAttributes[indexes[i][j]]->name;
					}
					else
					{
						name = name + " " + topAttributes[indexes[i][j]]->name;
					}
				}
								
				numBands = 0;
				for(int j = 0; j < numIndexes[i]; j++)
				{
					numBands += topAttributes[indexes[i][j]]->numBands;
				}
				
				powerSetAttributes[i] = new CountAttributes();
				powerSetAttributes[i]->name = name;
				powerSetAttributes[i]->numBands = numBands;
				powerSetAttributes[i]->bands = new int[numBands];
				powerSetAttributes[i]->thresholds = new float[numBands];
				count = 0;
				for(int j = 0; j < numIndexes[i]; j++)
				{
					for(int n = 0; n < topAttributes[indexes[i][j]]->numBands; n++)
					{
						powerSetAttributes[i]->bands[count] = topAttributes[indexes[i][j]]->bands[n];
						powerSetAttributes[i]->thresholds[count] = topAttributes[indexes[i][j]]->thresholds[n];
						count++;
					}
				}
			}
			
		}
		catch(RSGISMathException& e)
		{
			throw e;
		}
		
		delete[] numIndexes;
		
		for(int i = 0; i < numSets; i++)
		{
			delete[] indexes[i];
		}
		delete[] indexes;
		
		delete[] topAttributes;
	}
	
	RSGISPopulateTopPowerSet::~RSGISPopulateTopPowerSet()
	{
		delete[] datasets;
	}
	
	
	
	
}}
