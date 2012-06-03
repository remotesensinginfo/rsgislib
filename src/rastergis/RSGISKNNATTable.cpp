/*
 *  RSGISKNNATTable.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/06/2012.
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

#include "RSGISKNNATTable.h"

namespace rsgis{namespace rastergis{
    
    
    RSGISKNNATTable::RSGISKNNATTable()
    {
        this->initialised = false;
    }
        
    void RSGISKNNATTable::initKNN(RSGISAttributeTable *attTable, string trainField, string classField, bool limitToClass, int classVal, unsigned int k, float distThreshold, rsgisdistmetrics distMetric, vector<string> *attributeNames)throw(RSGISAttributeTableException)
    {
        try
        {
            this->attTable = attTable;
            this->trainField = trainField;
            this->trainFieldIdx = 0;
            this->trainFieldDT = rsgis_bool;
            this->limitToClass = limitToClass;
            this->classField = classField;
            this->classVal = classVal;
            if(this->limitToClass)
            {
                this->classFieldIdx = 0;
                this->classFieldDT = rsgis_int;
            }
            this->k = k;
            this->distThreshold = distThreshold;
            this->distMetric = distMetric;
            
            if(this->k == 0)
            {
                throw RSGISAttributeTableException("K must be greater than 0.");
            }
            
            if((attributeNames == NULL) || (attributeNames->size() == 0))
            {
                throw RSGISAttributeTableException("At least 1 attribute needs to be provided.");
            }
            
            if(!this->attTable->hasAttribute(this->trainField))
            {
                string message = string("The field specified for training \'") + this->trainField + string("\' does not exist.");
                throw RSGISAttributeTableException(message);
            }
            else if(this->attTable->getDataType(this->trainField) != rsgis_bool)
            {
                string message = string("Cannot proceed as \'") + trainField + string("\' field is not of type boolean.");
                throw RSGISAttributeTableException(message);
            }
            this->trainFieldIdx = this->attTable->getFieldIndex(this->trainField);
            cout << "Training field index = " << this->trainFieldIdx << endl;
            
            RSGISAttribute *attribute = NULL;
            for(vector<string>::iterator iterNames = attributeNames->begin(); iterNames != attributeNames->end(); ++iterNames)
            {
                if(!this->attTable->hasAttribute(*iterNames))
                {
                    string message = string("The field \'") + *iterNames + string("\' does not exist.");
                    throw RSGISAttributeTableException(message);
                }
                else if(this->attTable->getDataType(*iterNames) == rsgis_float)
                {
                    attribute = new RSGISAttribute();
                    attribute->name = *iterNames;
                    attribute->dataType = rsgis_float;
                    attribute->idx = this->attTable->getFieldIndex(*iterNames);
                    this->attributes.push_back(attribute);
                }
                else if(this->attTable->getDataType(*iterNames) == rsgis_int)
                {
                    attribute = new RSGISAttribute();
                    attribute->name = *iterNames;
                    attribute->dataType = rsgis_int;
                    attribute->idx = this->attTable->getFieldIndex(*iterNames);
                    this->attributes.push_back(attribute);
                }
                else
                {
                    string message = string("The data type for \'") + *iterNames + string("\' is not float or int.");
                    throw RSGISAttributeTableException(message);
                }
            }
            
            cout << "All fields has present and checked\n";
            
            // Build training data.
            cout << "Get training data\n";
            knownData = new vector< vector<double>* >();
            
            unsigned int idx = 0;
            unsigned int rowCount = 0;
            unsigned long feedback = attTable->getSize()/10;
            int feedbackCounter = 0;
            cout << "Started" << flush;
            for(this->attTable->start(); this->attTable->end(); ++(*this->attTable))
            {
                if((rowCount % feedback) == 0)
                {
                    cout << "." << feedbackCounter << "." << flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                if((*(*this->attTable))->boolFields->at(this->trainFieldIdx))
                {
                    knownData->push_back(new vector<double>());
                    knownData->at(idx)->reserve(attributes.size());
                    for(vector<RSGISAttribute*>::iterator iterAtt = attributes.begin(); iterAtt != attributes.end(); ++iterAtt)
                    {
                        if((*iterAtt)->dataType == rsgis_float)
                        {
                            knownData->at(idx)->push_back((*(*this->attTable))->floatFields->at((*iterAtt)->idx));
                        }
                        else if((*iterAtt)->dataType == rsgis_int)
                        {
                            knownData->at(idx)->push_back((*(*this->attTable))->intFields->at((*iterAtt)->idx));
                        }
                    }
                    ++idx;
                }
                ++rowCount;
            }
            cout << " Complete.\n";
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISMathException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        this->initialised = true;
    }
    
    double RSGISKNNATTable::calcDist(rsgisdistmetrics distMetric, vector<double> *vals1, vector<double> *vals2) throw(RSGISMathException)
    {
        double dist = numeric_limits<double>::signaling_NaN();
        try 
        {
            if(vals1->size() != vals2->size())
            {
                throw RSGISMathException("Known data and value arrays need to be of the same length.");
            }
            
            if(distMetric == rsgis_euclidean)
            {
                double sum = 0;
                size_t numVals = vals1->size();
                for(size_t i = 0; i < numVals; ++i)
                {
                    sum += pow((vals1->at(i) - vals2->at(i)), 2.0);
                }
                dist = sqrt(sum/((double)numVals));
            }
            else if(distMetric == rsgis_manhatten)
            {
                double sum = 0;
                size_t numVals = vals1->size();
                for(size_t i = 0; i < numVals; ++i)
                {
                    sum += abs((vals1->at(i) - vals2->at(i)));
                }
                dist = sum/((double)numVals);
            }
            else if(distMetric == rsgis_mahalanobis)
            {
                throw RSGISMathException("Mahalanobis distance metric is currently not implemented...");
            }
            else 
            {
                throw RSGISMathException("Distance metric is unknown.");
            }
        }
        catch (RSGISMathException &e)
        {
            throw e;
        }
        
        return dist;
    }

    vector<double>* RSGISKNNATTable::calcVariableMeans() throw(RSGISMathException)
    {
        return NULL;
    }
    
    Matrix* RSGISKNNATTable::calcCovarianceMatrix() throw(RSGISMathException)
    {
        return NULL;
    }
     
    
    RSGISKNNATTable::~RSGISKNNATTable()
    {
        for(vector<RSGISAttribute*>::iterator iterAtt = attributes.begin(); iterAtt != attributes.end(); ++iterAtt)
        {
            delete *iterAtt;
        }            
        for(vector< vector<double>* >::iterator iterData = knownData->begin(); iterData != knownData->end(); ++iterData)
        {
            delete *iterData;
        }
        delete knownData;
    }

    RSGISKNNATTableExtrapolation::RSGISKNNATTableExtrapolation(string valField) : RSGISKNNATTable()
    {
        this->valField = valField;
        this->valFieldIdx = 0;
        this->valFieldDT = rsgis_float;
    }
    
    void RSGISKNNATTableExtrapolation::performKNN()throw(RSGISAttributeTableException)
    {
        try 
        {
            if(!this->initialised)
            {
                throw RSGISAttributeTableException("KNN needs to be initised being performing KNN.");
            }
            
            if(!this->attTable->hasAttribute(valField))
            {
                string message = string("The field specified for data values \'") + this->valField + string("\' does not exist.");
                throw RSGISAttributeTableException(message);
            }
            else if(this->attTable->getDataType(valField) != rsgis_float)
            {
                string message = string("Cannot proceed as \'") + this->valField + string("\' field is not of type float.");
                throw RSGISAttributeTableException(message);
            }
            this->valFieldIdx = this->attTable->getFieldIndex(valField);
            cout << "Value field index = " << this->valFieldIdx << endl;
            
            cout << "Get training known values\n";
            vector<double> *knownVals = new vector<double>();
            unsigned int idx = 0;
            unsigned int rowCount = 0;
            unsigned long feedback = attTable->getSize()/10;
			int feedbackCounter = 0;
			cout << "Started" << flush;
            for(this->attTable->start(); this->attTable->end(); ++(*this->attTable))
            {
                if((rowCount % feedback) == 0)
				{
					cout << "." << feedbackCounter << "." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
                if((*(*this->attTable))->boolFields->at(this->trainFieldIdx))
                {
                    knownVals->push_back((*(*this->attTable))->floatFields->at(this->valFieldIdx));
                    ++idx;
                }
                ++rowCount;
            }
            cout << " Complete.\n";
            
            
            // Apply to attribute table.
            cout << "Extrapolate values for unknowns\n";
            idx = 0;
            feedback = attTable->getSize()/10;
			feedbackCounter = 0;
            vector<double> *data = new vector<double>();
            data->reserve(attributes.size());
			cout << "Started" << flush;
            for(attTable->start(); attTable->end(); ++(*attTable))
            {
                if((idx % feedback) == 0)
				{
					cout << "." << feedbackCounter << "." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
                if(!(*(*attTable))->boolFields->at(trainFieldIdx))
                {
                    data->clear();
                    for(vector<RSGISAttribute*>::iterator iterAtt = attributes.begin(); iterAtt != attributes.end(); ++iterAtt)
                    {
                        if((*iterAtt)->dataType == rsgis_float)
                        {
                            data->push_back((*(*attTable))->floatFields->at((*iterAtt)->idx));
                        }
                        else if((*iterAtt)->dataType == rsgis_int)
                        {
                            data->push_back((*(*attTable))->intFields->at((*iterAtt)->idx));
                        }
                    }
                    
                    (*(*attTable))->floatFields->at(valFieldIdx) = calcNewVal(k, distThreshold, distMetric, knownVals, knownData, data);
                }
                ++idx;
            }
            cout << " Complete.\n";
            
            // Clean up memory
            delete data;
            delete knownVals;
            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISMathException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    double RSGISKNNATTableExtrapolation::calcNewVal(unsigned int k, float distThreshold, rsgisdistmetrics distMetric, vector<double> *knownLocalVals, vector< vector<double>* > *knownLocalData, vector<double> *unknownData) throw(RSGISMathException)
    {
        double outVal = numeric_limits<double>::signaling_NaN();
        try 
        {
            if(knownLocalVals->size() != knownLocalData->size())
            {
                throw RSGISMathException("Known data and value arrays need to be of the same length.");
            }
            
            vector< pair<size_t, double> > neighbours;
            double cDistThreshold = 0;
            double maxDist = 0;
            bool maxDistFirst = true;
            bool first = true;
            size_t numVals = knownLocalVals->size();
            double dist = 0;
            for(size_t i = 0; i < numVals; ++i)
            {
                dist = this->calcDist(distMetric, knownLocalData->at(i), unknownData);
                
                if(neighbours.size() < k)
                {
                    neighbours.push_back(pair<size_t, double>(i,dist));
                    if(first)
                    {
                        cDistThreshold = dist;
                        first = false;
                    }
                    else if(dist > cDistThreshold)
                    {
                        cDistThreshold = dist;
                    }
                }
                else if(dist < cDistThreshold)
                {
                    maxDistFirst = true;
                    maxDist = 0;
                    for(vector< pair<size_t, double> >::iterator iterNeighbours = neighbours.begin(); iterNeighbours != neighbours.end(); ++iterNeighbours)
                    {
                        if((*iterNeighbours).second == cDistThreshold)
                        {
                            (*iterNeighbours).first = i;
                            (*iterNeighbours).second = dist;
                        }
                        
                        if(maxDistFirst)
                        {
                            maxDist = (*iterNeighbours).second;
                            maxDistFirst = false;
                        }
                        else if((*iterNeighbours).second > maxDist)
                        {
                            maxDist = (*iterNeighbours).second;
                        }
                    }
                    cDistThreshold = maxDist;
                }
                //else ignore as outside of the knn
            }
            
            //cout << "\nNeighbours:\n";
            double sumDist = 0;
            for(vector< pair<size_t, double> >::iterator iterNeighbours = neighbours.begin(); iterNeighbours != neighbours.end(); ++iterNeighbours)
            {
                //cout << "\t" << (*iterNeighbours).first << " = " << (*iterNeighbours).second << ": Data = " << knownVals->at((*iterNeighbours).first) << endl;
                sumDist += (*iterNeighbours).second;
            }
            //cout << "Total Distance = " << sumDist << endl;
            
            outVal = 0;
            for(vector< pair<size_t, double> >::iterator iterNeighbours = neighbours.begin(); iterNeighbours != neighbours.end(); ++iterNeighbours)
            {
                outVal += knownLocalVals->at((*iterNeighbours).first) * ((*iterNeighbours).second/sumDist);
            }
            //cout << "Output Value = " << outVal << endl;
            
        }
        catch (RSGISMathException &e)
        {
            throw e;
        }
        
        return outVal;
    }
    
    RSGISKNNATTableExtrapolation::~RSGISKNNATTableExtrapolation()
    {
        
    }
    
    
/*    
    
    RSGISKNNATTableExtrapolation::RSGISKNNATTableExtrapolation()
    {
        
    }
    
    void RSGISKNNATTableExtrapolation::performExtrapolation(RSGISAttributeTable *attTable, string trainField, string valField, unsigned int k, float distThreshold, rsgisdistmetrics distMetric, vector<string> *attributeNames)throw(RSGISAttributeTableException)
    {
        try 
        {
            if(k == 0)
            {
                throw RSGISAttributeTableException("K must be greater than 0.");
            }
            
            if((attributeNames == NULL) || (attributeNames->size() == 0))
            {
                throw RSGISAttributeTableException("At least 1 attribute needs to be provided.");
            }
            
            if(!attTable->hasAttribute(trainField))
            {
                string message = string("The field specified for training \'") + trainField + string("\' does not exist.");
                throw RSGISAttributeTableException(message);
            }
            else if(attTable->getDataType(trainField) != rsgis_bool)
            {
                string message = string("Cannot proceed as \'") + trainField + string("\' field is not of type boolean.");
                throw RSGISAttributeTableException(message);
            }
            unsigned int trainFieldIdx = attTable->getFieldIndex(trainField);
            cout << "Training field index = " << trainFieldIdx << endl;
            
            if(!attTable->hasAttribute(valField))
            {
                string message = string("The field specified for data values \'") + valField + string("\' does not exist.");
                throw RSGISAttributeTableException(message);
            }
            else if(attTable->getDataType(valField) != rsgis_float)
            {
                string message = string("Cannot proceed as \'") + valField + string("\' field is not of type float.");
                throw RSGISAttributeTableException(message);
            }
            unsigned int valFieldIdx = attTable->getFieldIndex(valField);
            cout << "Value field index = " << valFieldIdx << endl;
            
            vector<RSGISAttribute*> attributes;
            RSGISAttribute *attribute = NULL;
            for(vector<string>::iterator iterNames = attributeNames->begin(); iterNames != attributeNames->end(); ++iterNames)
            {
                if(!attTable->hasAttribute(*iterNames))
                {
                    string message = string("The field \'") + *iterNames + string("\' does not exist.");
                    throw RSGISAttributeTableException(message);
                }
                else if(attTable->getDataType(*iterNames) == rsgis_float)
                {
                    attribute = new RSGISAttribute();
                    attribute->name = *iterNames;
                    attribute->dataType = rsgis_float;
                    attribute->idx = attTable->getFieldIndex(*iterNames);
                    attributes.push_back(attribute);
                }
                else if(attTable->getDataType(*iterNames) == rsgis_int)
                {
                    attribute = new RSGISAttribute();
                    attribute->name = *iterNames;
                    attribute->dataType = rsgis_int;
                    attribute->idx = attTable->getFieldIndex(*iterNames);
                    attributes.push_back(attribute);
                }
                else
                {
                    string message = string("The data type for \'") + *iterNames + string("\' is not float or int.");
                    throw RSGISAttributeTableException(message);
                }
            }
            
            cout << "All fields has present and checked\n";
            
            // Build training data.
            cout << "Building training data\n";
            vector<double> *knownVals = new vector<double>();
            vector< vector<double>* > *knownData = new vector< vector<double>* >();
            
            unsigned int idx = 0;
            unsigned int rowCount = 0;
            unsigned long feedback = attTable->getSize()/10;
			int feedbackCounter = 0;
			cout << "Started" << flush;
            for(attTable->start(); attTable->end(); ++(*attTable))
            {
                if((rowCount % feedback) == 0)
				{
					cout << "." << feedbackCounter << "." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
                if((*(*attTable))->boolFields->at(trainFieldIdx))
                {
                    knownVals->push_back((*(*attTable))->floatFields->at(valFieldIdx));
                    knownData->push_back(new vector<double>());
                    knownData->at(idx)->reserve(attributes.size());
                    for(vector<RSGISAttribute*>::iterator iterAtt = attributes.begin(); iterAtt != attributes.end(); ++iterAtt)
                    {
                        if((*iterAtt)->dataType == rsgis_float)
                        {
                            knownData->at(idx)->push_back((*(*attTable))->floatFields->at((*iterAtt)->idx));
                        }
                        else if((*iterAtt)->dataType == rsgis_int)
                        {
                            knownData->at(idx)->push_back((*(*attTable))->intFields->at((*iterAtt)->idx));
                        }
                    }
                    ++idx;
                }
                ++rowCount;
            }
            cout << " Complete.\n";
            
            // Apply to attribute table.
            cout << "Extrapolate values for unknowns\n";
            idx = 0;
            feedback = attTable->getSize()/10;
			feedbackCounter = 0;
            vector<double> *data = new vector<double>();
            data->reserve(attributes.size());
			cout << "Started" << flush;
            for(attTable->start(); attTable->end(); ++(*attTable))
            {
                if((idx % feedback) == 0)
				{
					cout << "." << feedbackCounter << "." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
                if(!(*(*attTable))->boolFields->at(trainFieldIdx))
                {
                    data->clear();
                    for(vector<RSGISAttribute*>::iterator iterAtt = attributes.begin(); iterAtt != attributes.end(); ++iterAtt)
                    {
                        if((*iterAtt)->dataType == rsgis_float)
                        {
                            data->push_back((*(*attTable))->floatFields->at((*iterAtt)->idx));
                        }
                        else if((*iterAtt)->dataType == rsgis_int)
                        {
                            data->push_back((*(*attTable))->intFields->at((*iterAtt)->idx));
                        }
                    }
                    
                    (*(*attTable))->floatFields->at(valFieldIdx) = calcNewVal(k, distThreshold, distMetric, knownVals, knownData, data);
                }
                ++idx;
            }
            cout << " Complete.\n";
            
            // Clean up memory
            delete data;
            for(vector<RSGISAttribute*>::iterator iterAtt = attributes.begin(); iterAtt != attributes.end(); ++iterAtt)
            {
                delete *iterAtt;
            }            
            for(vector< vector<double>* >::iterator iterData = knownData->begin(); iterData != knownData->end(); ++iterData)
            {
                delete *iterData;
            }
            delete knownData;
            delete knownVals;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISMathException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    double RSGISKNNATTableExtrapolation::calcNewVal(unsigned int k, float distThreshold, rsgisdistmetrics distMetric, vector<double> *knownVals, vector< vector<double>* > *knownData, vector<double> *unknownData) throw(RSGISMathException)
    {
        double outVal = numeric_limits<double>::signaling_NaN();
        try 
        {
            if(knownVals->size() != knownData->size())
            {
                throw RSGISMathException("Known data and value arrays need to be of the same length.");
            }
            
            vector< pair<size_t, double> > neighbours;
            double cDistThreshold = 0;
            double maxDist = 0;
            bool maxDistFirst = true;
            bool first = true;
            size_t numVals = knownVals->size();
            double dist = 0;
            for(size_t i = 0; i < numVals; ++i)
            {
                dist = this->calcDist(distMetric, knownData->at(i), unknownData);
                
                if(neighbours.size() < k)
                {
                    neighbours.push_back(pair<size_t, double>(i,dist));
                    if(first)
                    {
                        cDistThreshold = dist;
                        first = false;
                    }
                    else if(dist > cDistThreshold)
                    {
                         cDistThreshold = dist;
                    }
                }
                else if(dist < cDistThreshold)
                {
                    maxDistFirst = true;
                    maxDist = 0;
                    for(vector< pair<size_t, double> >::iterator iterNeighbours = neighbours.begin(); iterNeighbours != neighbours.end(); ++iterNeighbours)
                    {
                        if((*iterNeighbours).second == cDistThreshold)
                        {
                            (*iterNeighbours).first = i;
                            (*iterNeighbours).second = dist;
                        }
                        
                        if(maxDistFirst)
                        {
                            maxDist = (*iterNeighbours).second;
                            maxDistFirst = false;
                        }
                        else if((*iterNeighbours).second > maxDist)
                        {
                            maxDist = (*iterNeighbours).second;
                        }
                    }
                    cDistThreshold = maxDist;
                }
                //else ignore as outside of the knn
            }
            
            //cout << "\nNeighbours:\n";
            double sumDist = 0;
            for(vector< pair<size_t, double> >::iterator iterNeighbours = neighbours.begin(); iterNeighbours != neighbours.end(); ++iterNeighbours)
            {
                //cout << "\t" << (*iterNeighbours).first << " = " << (*iterNeighbours).second << ": Data = " << knownVals->at((*iterNeighbours).first) << endl;
                sumDist += (*iterNeighbours).second;
            }
            //cout << "Total Distance = " << sumDist << endl;
            
            outVal = 0;
            for(vector< pair<size_t, double> >::iterator iterNeighbours = neighbours.begin(); iterNeighbours != neighbours.end(); ++iterNeighbours)
            {
                outVal += knownVals->at((*iterNeighbours).first) * ((*iterNeighbours).second/sumDist);
            }
            //cout << "Output Value = " << outVal << endl;
            
        }
        catch (RSGISMathException &e)
        {
            throw e;
        }
        
        return outVal;
    }
    
    double RSGISKNNATTableExtrapolation::calcDist(rsgisdistmetrics distMetric, vector<double> *vals1, vector<double> *vals2) throw(RSGISMathException)
    {
        double dist = numeric_limits<double>::signaling_NaN();
        try 
        {
            if(vals1->size() != vals2->size())
            {
                throw RSGISMathException("Known data and value arrays need to be of the same length.");
            }
            
            if(distMetric == rsgis_euclidean)
            {
                double sum = 0;
                size_t numVals = vals1->size();
                for(size_t i = 0; i < numVals; ++i)
                {
                    sum += pow((vals1->at(i) - vals2->at(i)), 2.0);
                }
                dist = sqrt(sum/((double)numVals));
            }
            else if(distMetric == rsgis_manhatten)
            {
                double sum = 0;
                size_t numVals = vals1->size();
                for(size_t i = 0; i < numVals; ++i)
                {
                    sum += abs((vals1->at(i) - vals2->at(i)));
                }
                dist = sum/((double)numVals);
            }
            else if(distMetric == rsgis_mahalanobis)
            {
                throw RSGISMathException("Mahalanobis distance metric is currently not implemented...");
            }
            else 
            {
                throw RSGISMathException("Distance metric is unknown.");
            }
        }
        catch (RSGISMathException &e)
        {
            throw e;
        }
        
        return dist;
    }
        
    RSGISKNNATTableExtrapolation::~RSGISKNNATTableExtrapolation()
    {
        
    }
 */
    
}}



