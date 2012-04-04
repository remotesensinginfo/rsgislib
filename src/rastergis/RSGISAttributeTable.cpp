/*
 *  RSGISAttributeTable.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 15/02/2012.
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

#include "RSGISAttributeTable.h"

namespace rsgis{namespace rastergis{
    
    
    RSGISAttributeTable::RSGISAttributeTable()
    {
        
    }
    
    RSGISAttributeTable::RSGISAttributeTable(unsigned long long numFeatures)
    {
        this->createAttributeTable(numFeatures);
    }
    
    RSGISAttributeTable::RSGISAttributeTable(unsigned long long numFeatures, vector<pair<string, RSGISAttributeDataType> > *fields)
    {
        this->fields = fields;
        this->createAttributeTableWithFields(numFeatures);
    }
    
    void RSGISAttributeTable::createAttributeTableWithFields(unsigned long long numFeatures)
    {
        attTable = new vector<RSGISFeature*>();
        attTable->reserve(numFeatures);
        
        fieldIdx = new map<string, unsigned int>();
        fieldDataType = new map<string, RSGISAttributeDataType>();
        
        numBoolFields = 0;
        numIntFields = 0;
        numFloatFields = 0;
        numStrFields = 0;
        vector<pair<string, RSGISAttributeDataType> >::iterator iterFields;
        for(iterFields = fields->begin(); iterFields != fields->end(); ++iterFields)
        {
            if((*iterFields).second == rsgis_bool)
            {
                fieldIdx->insert(pair<string, unsigned int>((*iterFields).first, numBoolFields));
                ++numBoolFields;
            }
            else if((*iterFields).second == rsgis_int)
            {
                fieldIdx->insert(pair<string, unsigned int>((*iterFields).first, numIntFields));
                ++numIntFields;
            }
            else if((*iterFields).second == rsgis_float)
            {
                fieldIdx->insert(pair<string, unsigned int>((*iterFields).first, numFloatFields));
                ++numFloatFields;
            }
            else if((*iterFields).second == rsgis_string)
            {
                fieldIdx->insert(pair<string, unsigned int>((*iterFields).first, numStrFields));
                ++numStrFields;
            }
            
            fieldDataType->insert(pair<string, RSGISAttributeDataType>((*iterFields).first, (*iterFields).second));
        }
        
        RSGISFeature *feature = NULL;
        for(unsigned long long i = 0; i < numFeatures; ++i)
        {
            feature = new RSGISFeature();
            feature->fid = i;
            feature->boolFields = new vector<bool>();
            feature->boolFields->reserve(numBoolFields);
            for(unsigned int i = 0; i < numBoolFields; ++i)
            {
                feature->boolFields->push_back(false);
            }            
            feature->intFields = new vector<long>();
            feature->intFields->reserve(numIntFields);
            for(unsigned int i = 0; i < numIntFields; ++i)
            {
                feature->intFields->push_back(0);
            }
            feature->floatFields = new vector<double>();
            feature->floatFields->reserve(numFloatFields);
            for(unsigned int i = 0; i < numFloatFields; ++i)
            {
                feature->floatFields->push_back(0.0);
            }
            feature->stringFields = new vector<string>();
            feature->stringFields->reserve(numStrFields);
            for(unsigned int i = 0; i < numStrFields; ++i)
            {
                feature->stringFields->push_back("");
            }
            
            attTable->push_back(feature);
        }
    }
    
    void RSGISAttributeTable::createAttributeTable(unsigned long long numFeatures)
    {
        attTable = new vector<RSGISFeature*>();
        attTable->reserve(numFeatures);
        
        fieldIdx = new map<string, unsigned int>();
        fieldDataType = new map<string, RSGISAttributeDataType>();
        fields = new vector<pair<string, RSGISAttributeDataType> >();
        
        this->numBoolFields = 0;
        this->numIntFields = 0;
        this->numFloatFields = 0;
        this->numStrFields = 0;
        
        RSGISFeature *feature = NULL;
        for(unsigned long long i = 0; i < numFeatures; ++i)
        {
            feature = new RSGISFeature();
            feature->fid = i;
            feature->boolFields = new vector<bool>();
            feature->intFields = new vector<long>();
            feature->floatFields = new vector<double>();
            feature->stringFields = new vector<string>();
            
            attTable->push_back(feature);
        }
    }
    
    bool RSGISAttributeTable::getBoolField(unsigned long long fid, string name) throw(RSGISAttributeTableException)
    {
        bool outVal = false;
        
        try 
        {
            unsigned int idx = this->getFieldIndex(name);
            RSGISAttributeDataType dt = this->getDataType(name);
            if(dt != rsgis_bool)
            {
                throw RSGISAttributeTableException("Field is not of boolean data type.");
            }
            if(idx >= numBoolFields)
            {
                throw RSGISAttributeTableException("Field index is not within attribute.");
            }
            if(fid >= attTable->size())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            outVal = attTable->at(fid)->boolFields->at(idx);
            
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        
        return outVal;
    }
    
    
    long RSGISAttributeTable::getIntField(unsigned long long fid, string name) throw(RSGISAttributeTableException)
    {
        long outVal = 0;
        
        try 
        {
            unsigned int idx = this->getFieldIndex(name);
            RSGISAttributeDataType dt = this->getDataType(name);
            if(dt != rsgis_int)
            {
                throw RSGISAttributeTableException("Field is not of integer data type.");
            }
            if(idx >= numIntFields)
            {
                throw RSGISAttributeTableException("Field index is not within attribute.");
            }
            if(fid >= attTable->size())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            outVal = attTable->at(fid)->intFields->at(idx);
            
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        
        return outVal;
    }
    
    double RSGISAttributeTable::getDoubleField(unsigned long long fid, string name) throw(RSGISAttributeTableException)
    {
        double outVal = 0;
        
        try 
        {
            unsigned int idx = this->getFieldIndex(name);
            RSGISAttributeDataType dt = this->getDataType(name);
            if(dt != rsgis_float)
            {
                throw RSGISAttributeTableException("Field is not of float data type.");
            }
            if(idx >= numFloatFields)
            {
                throw RSGISAttributeTableException("Field index is not within attribute.");
            }
            if(fid >= attTable->size())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            outVal = attTable->at(fid)->floatFields->at(idx);
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        
        return outVal;
    }
        
    string RSGISAttributeTable::getStringField(unsigned long long fid, string name) throw(RSGISAttributeTableException)
    {
        string outVal = 0;
        
        try 
        {
            unsigned int idx = this->getFieldIndex(name);
            RSGISAttributeDataType dt = this->getDataType(name);
            if(dt != rsgis_string)
            {
                throw RSGISAttributeTableException("Field is not of string data type.");
            }
            if(idx >= numStrFields)
            {
                throw RSGISAttributeTableException("Field index is not within attribute.");
            }
            if(fid >= attTable->size())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            outVal = attTable->at(fid)->stringFields->at(idx);
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        
        return outVal;
    }
    
    void RSGISAttributeTable::setBoolField(unsigned long long fid, string name, bool value) throw(RSGISAttributeTableException)
    {
        try 
        {
            unsigned int idx = this->getFieldIndex(name);
            RSGISAttributeDataType dt = this->getDataType(name);
            if(dt != rsgis_bool)
            {
                throw RSGISAttributeTableException("Field is not of boolean data type.");
            }
            if(idx >= numBoolFields)
            {
                throw RSGISAttributeTableException("Field index is not within attribute.");
            }
            if(fid >= attTable->size())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            attTable->at(fid)->boolFields->at(idx) = value;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
        
    void RSGISAttributeTable::setIntField(unsigned long long fid, string name, long value) throw(RSGISAttributeTableException)
    {
        try 
        {
            unsigned int idx = this->getFieldIndex(name);
            RSGISAttributeDataType dt = this->getDataType(name);
            if(dt != rsgis_int)
            {
                throw RSGISAttributeTableException("Field is not of integer data type.");
            }
            if(idx >= numIntFields)
            {
                throw RSGISAttributeTableException("Field index is not within attribute.");
            }
            if(fid >= attTable->size())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            attTable->at(fid)->intFields->at(idx) = value;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    void RSGISAttributeTable::setDoubleField(unsigned long long fid, string name, double value) throw(RSGISAttributeTableException)
    {
        try 
        {
            unsigned int idx = this->getFieldIndex(name);
            RSGISAttributeDataType dt = this->getDataType(name);
            if(dt != rsgis_float)
            {
                throw RSGISAttributeTableException("Field is not of float data type.");
            }
            if(idx >= numFloatFields)
            {
                throw RSGISAttributeTableException("Field index is not within attribute.");
            }
            if(fid >= attTable->size())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            attTable->at(fid)->floatFields->at(idx) = value;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    void RSGISAttributeTable::setStringField(unsigned long long fid, string name, string value) throw(RSGISAttributeTableException)
    {
        try 
        {
            unsigned int idx = this->getFieldIndex(name);
            RSGISAttributeDataType dt = this->getDataType(name);
            if(dt != rsgis_string)
            {
                throw RSGISAttributeTableException("Field is not of string data type.");
            }
            if(idx >= numStrFields)
            {
                throw RSGISAttributeTableException("Field index is not within attribute.");
            }
            if(fid >= attTable->size())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            attTable->at(fid)->stringFields->at(idx) = value;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    void RSGISAttributeTable::setBoolValue(string name, bool value) throw(RSGISAttributeTableException)
    {
        try
        {
            if(!this->hasAttribute(name))
            {
                this->addAttBoolField(name, value);
            }
            else if(this->getDataType(name) != rsgis_bool)
            {
                throw RSGISAttributeTableException("Field is not of type boolean.");
            }
            unsigned int idx = this->getFieldIndex(name);
            
            for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
            {
                (*iterFeats)->boolFields->at(idx) = value;
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    void RSGISAttributeTable::setIntValue(string name, long value) throw(RSGISAttributeTableException)
    {
        try
        {
            if(!this->hasAttribute(name))
            {
                this->addAttIntField(name, value);
            }
            else if(this->getDataType(name) != rsgis_int)
            {
                throw RSGISAttributeTableException("Field is not of type integer.");
            }
            unsigned int idx = this->getFieldIndex(name);
            
            for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
            {
                (*iterFeats)->intFields->at(idx) = value;
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    void RSGISAttributeTable::setFloatValue(string name, double value) throw(RSGISAttributeTableException)
    {
        try
        {
            if(!this->hasAttribute(name))
            {
                this->addAttFloatField(name, value);
            }
            else if(this->getDataType(name) != rsgis_float)
            {
                throw RSGISAttributeTableException("Field is not of type float.");
            }
            unsigned int idx = this->getFieldIndex(name);
            
            for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
            {
                (*iterFeats)->floatFields->at(idx) = value;
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    void RSGISAttributeTable::setStringValue(string name, string value) throw(RSGISAttributeTableException)
    {
        try
        {
            if(!this->hasAttribute(name))
            {
                this->addAttStringField(name, value);
            }
            else if(this->getDataType(name) != rsgis_string)
            {
                throw RSGISAttributeTableException("Field is not of type float.");
            }
            unsigned int idx = this->getFieldIndex(name);
            
            for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
            {
                (*iterFeats)->stringFields->at(idx) = value;
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
        
    RSGISFeature* RSGISAttributeTable::getFeature(unsigned long long fid) throw(RSGISAttributeTableException)
    {
        if(fid >= attTable->size())
        {
            RSGISTextUtils textUtils;
            string message = string("Feature (") + textUtils.uInt64bittostring(fid) + string(") is not within the attribute table.");
            throw RSGISAttributeTableException(message);
        }
        
        return attTable->at(fid);
    }
    
    void RSGISAttributeTable::addAttBoolField(string name, bool val)
    {
        fieldIdx->insert(pair<string, unsigned int>(name, numBoolFields));
        ++numBoolFields;
        fieldDataType->insert(pair<string, RSGISAttributeDataType>(name, rsgis_bool));
        fields->push_back(pair<string, RSGISAttributeDataType>(name, rsgis_bool));
        
        for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
        {
            (*iterFeats)->boolFields->push_back(val);
        }
    }
    
    void RSGISAttributeTable::addAttIntField(string name, long val)
    {
        fieldIdx->insert(pair<string, unsigned int>(name, numIntFields));
        ++numIntFields;
        fieldDataType->insert(pair<string, RSGISAttributeDataType>(name, rsgis_int));
        fields->push_back(pair<string, RSGISAttributeDataType>(name, rsgis_int));

        for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
        {
            (*iterFeats)->intFields->push_back(val);
        }
    }
    
    
    void RSGISAttributeTable::addAttFloatField(string name, double val)
    {
        fieldIdx->insert(pair<string, unsigned int>(name, numFloatFields));
        ++numFloatFields;
        fieldDataType->insert(pair<string, RSGISAttributeDataType>(name, rsgis_float));
        fields->push_back(pair<string, RSGISAttributeDataType>(name, rsgis_float));
        
        for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
        {
            (*iterFeats)->floatFields->push_back(val);
        }
    }
    
    void RSGISAttributeTable::addAttStringField(string name, string val)
    {
        fieldIdx->insert(pair<string, unsigned int>(name, numStrFields));
        ++numStrFields;
        fieldDataType->insert(pair<string, RSGISAttributeDataType>(name, rsgis_string));
        fields->push_back(pair<string, RSGISAttributeDataType>(name, rsgis_string));
        
        for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
        {
            (*iterFeats)->stringFields->push_back(val);
        }

    }
    
    void RSGISAttributeTable::addAttributes(vector<RSGISAttribute*> *attributes) throw(RSGISAttributeTableException)
    {
        try 
        {
            for(vector<RSGISAttribute*>::iterator iterAtt = attributes->begin(); iterAtt != attributes->end(); ++iterAtt)
            {
                if((*iterAtt)->dataType == rsgis_bool)
                {
                    this->addAttBoolField((*iterAtt)->name, false);
                }
                else if((*iterAtt)->dataType == rsgis_int)
                {
                    this->addAttIntField((*iterAtt)->name, 0);
                }
                else if((*iterAtt)->dataType == rsgis_float)
                {
                    this->addAttFloatField((*iterAtt)->name, 0.0);
                }
                else if((*iterAtt)->dataType == rsgis_string)
                {
                    this->addAttStringField((*iterAtt)->name, "");
                }
                else
                {
                    throw RSGISAttributeTableException("Data type not recognised.");
                }
            }
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        
    }
        
    RSGISAttributeDataType RSGISAttributeTable::getDataType(string name) throw(RSGISAttributeTableException)
    {
        map<string, RSGISAttributeDataType>::iterator iter;
        iter = fieldDataType->find(name);
        
        if(iter == fieldDataType->end())
        {
            string message = string("Field \'") + name + string("\' could not be found.");
            throw RSGISAttributeTableException(message);
        }
        
        return (*iter).second;
    }
    
    unsigned int RSGISAttributeTable::getFieldIndex(string name) throw(RSGISAttributeTableException)
    {
        map<string, unsigned int>::iterator iter;
        iter = fieldIdx->find(name);
        
        
        if(iter == fieldIdx->end())
        {
            string message = string("Field \'") + name + string("\' could not be found.");
            throw RSGISAttributeTableException(message);
        }
        
        return (*iter).second;
    }
    
    vector<string>* RSGISAttributeTable::getAttributeNames()
    {
        vector<string> *names = new vector<string>();
        for(vector<pair<string, RSGISAttributeDataType> >::iterator iterFields = fields->begin(); iterFields != fields->end(); ++iterFields)
        {
            names->push_back((*iterFields).first);
        }
        
        return names;
    }
    
    bool RSGISAttributeTable::hasAttribute(string name)
    {
        map<string, unsigned int>::iterator iter;
        iter = fieldIdx->find(name);
        
        
        if(iter == fieldIdx->end())
        {
            return false;
        }
        
        return true;
    }
    
    void RSGISAttributeTable::applyIfStatements(vector<RSGISIfStatement*> *statements) throw(RSGISAttributeTableException)
    {
        try
        {
            for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
            {
                for(vector<RSGISIfStatement*>::iterator iterStatement = statements->begin(); iterStatement != statements->end(); ++iterStatement)
                {
                    if((*iterStatement)->noExp || (*iterStatement)->exp->evaluate(*iterFeats))
                    {
                        if(!(*iterStatement)->ignore)
                        {
                            RSGISAttributeDataType dataType = this->getDataType((*iterStatement)->field);
                            unsigned int idx = this->getFieldIndex((*iterStatement)->field);
                                                    
                            if(dataType == rsgis_int)
                            {
                                (*iterFeats)->intFields->at(idx) = (*iterStatement)->value;
                            }
                            else if(dataType == rsgis_float)
                            {
                                (*iterFeats)->floatFields->at(idx) = (double)(*iterStatement)->value;
                            }
                            else
                            {
                                throw RSGISAttributeTableException("Only int and float are supported output types.");
                            }
                        }                        
                        break;
                    }
                }
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    bool RSGISAttributeTable::applyIfStatementsToFeature(RSGISFeature *feat, vector<RSGISIfStatement*> *statements) throw(RSGISAttributeTableException)
    {
        bool changeOccured = false;
        try
        {
            for(vector<RSGISIfStatement*>::iterator iterStatement = statements->begin(); iterStatement != statements->end(); ++iterStatement)
            {
                if((*iterStatement)->noExp || (*iterStatement)->exp->evaluate(feat))
                {
                    if(!(*iterStatement)->ignore)
                    {
                        RSGISAttributeDataType dataType = this->getDataType((*iterStatement)->field);
                        unsigned int idx = this->getFieldIndex((*iterStatement)->field);
                        
                        if(dataType == rsgis_int)
                        {
                            feat->intFields->at(idx) = (*iterStatement)->value;
                        }
                        else if(dataType == rsgis_float)
                        {
                            feat->floatFields->at(idx) = (double)(*iterStatement)->value;
                        }
                        else
                        {
                            throw RSGISAttributeTableException("Only int and float are supported output types.");
                        }
                        changeOccured = true;
                    }                        
                    break;
                }
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        return changeOccured;
    }
    
    
    void RSGISAttributeTable::populateIfStatementsWithIdxs(vector<RSGISIfStatement*> *statements) throw(RSGISAttributeTableException)
    {
        try
        {
            for(vector<RSGISIfStatement*>::iterator iterStatement = statements->begin(); iterStatement != statements->end(); ++iterStatement)
            {
                if(!(*iterStatement)->noExp)
                {
                    (*iterStatement)->exp->popIdxs(this);
                }
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    
    void RSGISAttributeTable::calculateFieldsMUParser(string expression, string outField, RSGISAttributeDataType outFieldDT, vector<RSGISMathAttVariable*> *variables) throw(RSGISAttributeTableException)
    {
        try 
        {
            if(!this->hasAttribute(outField))
            {
                if(outFieldDT == rsgis_int)
                {
                    this->addAttIntField(outField, 0);
                }
                else if(outFieldDT == rsgis_float)
                {
                    this->addAttFloatField(outField, 0);
                }
                else
                {
                    throw RSGISAttributeTableException("Field data type is not recognised.");
                }
            }
            else if(this->getDataType(outField) != outFieldDT)
            {
                throw RSGISAttributeTableException("Field type specified does not match attribute table.");
            }
            
            unsigned int outFieldIdx = this->getFieldIndex(outField);
            
            Parser *muParser = new Parser();
			value_type *inVals = new value_type[variables->size()];
            unsigned int i = 0;
            for(vector<RSGISMathAttVariable*>::iterator iterVars = variables->begin(); iterVars != variables->end(); ++iterVars)
            {
                if(!this->hasAttribute((*iterVars)->field))
                {
                    string message = string("Field \'") + (*iterVars)->field + string("\' is not present within the attribute table.");
                    throw RSGISAttributeTableException(message);
                }
                (*iterVars)->fieldDT = this->getDataType((*iterVars)->field);
                
                if(((*iterVars)->fieldDT != rsgis_int) & ((*iterVars)->fieldDT != rsgis_float))
                {
                    throw RSGISAttributeTableException("Input variables must be either of float or int data type.");
                }
                
                (*iterVars)->fieldIdx = this->getFieldIndex((*iterVars)->field);
                
                muParser->DefineVar(_T((*iterVars)->variable.c_str()), &inVals[i]);
                ++i;
            }            
            
            muParser->SetExpr(expression.c_str());
            
            bool intTrunkWarning = true;
            value_type result = 0;
            for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
            {
                unsigned int i = 0;
                for(vector<RSGISMathAttVariable*>::iterator iterVars = variables->begin(); iterVars != variables->end(); ++iterVars)
                {
                    if((*iterVars)->fieldDT == rsgis_int)
                    {
                        inVals[i] = (*iterFeats)->intFields->at((*iterVars)->fieldIdx);
                    }
                    else if((*iterVars)->fieldDT == rsgis_float)
                    {
                        inVals[i] = (*iterFeats)->floatFields->at((*iterVars)->fieldIdx);
                    }
                    
                    ++i;
                } 
                
                result = muParser->Eval();
                
                if(outFieldDT == rsgis_int)
                {
                    try
                    {
                        (*iterFeats)->intFields->at(outFieldIdx)  = lexical_cast<unsigned long>(result);
                    }
                    catch(bad_lexical_cast &e)
                    {
                        if(intTrunkWarning)
                        {
                            cerr << "Warning you are losing precision in the output using an integer.\n";
                            intTrunkWarning = false;
                        }
                        try
                        {
                            (*iterFeats)->intFields->at(outFieldIdx) = lexical_cast<unsigned long>(floor(result+0.5));
                        }
                        catch(bad_lexical_cast &e)
                        {
                            throw RSGISAttributeTableException(e.what());
                        }
                    }
                }
                else if(outFieldDT == rsgis_float)
                {
                    try
                    {
                        (*iterFeats)->floatFields->at(outFieldIdx)  = lexical_cast<double>(result);
                    }
                    catch(bad_lexical_cast &e)
                    {
                        throw RSGISAttributeTableException(e.what());
                    }
                }
            }
            
            delete muParser;
            delete[] inVals;
            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    void RSGISAttributeTable::copyValues(string fromField, string toField) throw(RSGISAttributeTableException)
    {
        try
        {
            RSGISAttributeDataType fromFieldDT = this->getDataType(fromField);
            RSGISAttributeDataType toFieldDT = this->getDataType(toField);
            
            if(fromFieldDT != toFieldDT)
            {
                throw RSGISAttributeTableException("Can only copy values between fields of the same type.");
            }
            
            unsigned int fromFieldIdx = this->getFieldIndex(fromField);
            unsigned int toFieldIdx = this->getFieldIndex(toField);
            
            for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
            {
                if(fromFieldDT == rsgis_bool)
                {
                    (*iterFeats)->boolFields->at(toFieldIdx) = (*iterFeats)->boolFields->at(fromFieldIdx);
                }
                else if(fromFieldDT == rsgis_int)
                {
                    (*iterFeats)->intFields->at(toFieldIdx) = (*iterFeats)->intFields->at(fromFieldIdx);
                }
                else if(fromFieldDT == rsgis_float)
                {
                    (*iterFeats)->floatFields->at(toFieldIdx) = (*iterFeats)->floatFields->at(fromFieldIdx);
                }
                else if(fromFieldDT == rsgis_string)
                {
                    (*iterFeats)->stringFields->at(toFieldIdx) = (*iterFeats)->stringFields->at(fromFieldIdx);
                }
                else
                {
                    throw RSGISAttributeTableException("Data type from not recognised for copying");
                }
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    unsigned long long RSGISAttributeTable::getSize()
    {
        return attTable->size();
    }
    
    
    vector<double>* RSGISAttributeTable::getFieldValues(string field) throw(RSGISAttributeTableException)
    {
        vector<double> *data = NULL;
        try
        {
            if(!this->hasAttribute(field))
            {
                throw RSGISAttributeTableException("Field is not present within the attribute table.");
            }
            
            if((this->getDataType(field) != rsgis_int) & (this->getDataType(field) != rsgis_float))
            {
                throw RSGISAttributeTableException("Field must either be of type 'float' or 'int'.");
            }
            
            unsigned int fieldIdx = this->getFieldIndex(field);
            RSGISAttributeDataType dataType = this->getDataType(field);
            
            data = new vector<double>();
            data->reserve(attTable->size());
            
            for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
            {
                if(dataType == rsgis_int)
                {
                    data->push_back((*iterFeats)->intFields->at(fieldIdx));
                }
                else if(dataType == rsgis_float)
                {
                    data->push_back((*iterFeats)->floatFields->at(fieldIdx));
                }
                else
                {
                    throw RSGISAttributeTableException("Data type from not recognised for copying");
                }
            }
            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        
        return data;
    }
    
    
    void RSGISAttributeTable::exportASCII(string outFile) throw(RSGISAttributeTableException)
    {
        ofstream outTxtFile;
		outTxtFile.open(outFile.c_str(), ios::out | ios::trunc);
        
        outTxtFile << "# Number of features in table\n";
        outTxtFile << attTable->size() << endl;
        
        outTxtFile << "# Number of Fields\n";
        outTxtFile << "nBool: " << this->numBoolFields << endl;
        outTxtFile << "nInt: " << this->numIntFields << endl;
        outTxtFile << "nFloat: " << this->numFloatFields << endl;
        outTxtFile << "nString: " << this->numStrFields << endl;
        
        outTxtFile << "# Field Name / Type Pairs\n";
        vector<pair<string, RSGISAttributeDataType> >::iterator iterFields;
        for(iterFields = fields->begin(); iterFields != fields->end(); ++iterFields)
        {
            outTxtFile << (*iterFields).first << ",";
            if((*iterFields).second == rsgis_bool)
            {
                outTxtFile << "rsgis_bool\n";
            }
            else if((*iterFields).second == rsgis_int)
            {
                outTxtFile << "rsgis_int\n";
            }
            else if((*iterFields).second == rsgis_float)
            {
                outTxtFile << "rsgis_float\n";
            }
            else if((*iterFields).second == rsgis_string)
            {
                outTxtFile << "rsgis_string\n";
            }
        }
        
        outTxtFile << "# Data - bool, int, float, string\n";
        for(vector<RSGISFeature*>::iterator iterFeat = attTable->begin(); iterFeat != attTable->end(); ++iterFeat)
        {
            outTxtFile << (*iterFeat)->fid;
            for(vector<bool>::iterator iterBools = (*iterFeat)->boolFields->begin(); iterBools != (*iterFeat)->boolFields->end(); ++iterBools)
            {
                if(*iterBools)
                {
                    outTxtFile << ",TRUE";
                }
                else
                {
                    outTxtFile << ",FALSE";
                }
            }
            
            for(vector<long>::iterator iterInts = (*iterFeat)->intFields->begin(); iterInts != (*iterFeat)->intFields->end(); ++iterInts)
            {
                outTxtFile << "," << *iterInts;
            }
            
            outTxtFile.precision(12);
            for(vector<double>::iterator iterFloats = (*iterFeat)->floatFields->begin(); iterFloats != (*iterFeat)->floatFields->end(); ++iterFloats)
            {
                outTxtFile << "," << *iterFloats;
            }
            
            for(vector<string>::iterator iterStrs = (*iterFeat)->stringFields->begin(); iterStrs != (*iterFeat)->stringFields->end(); ++iterStrs)
            {
                outTxtFile << "," << *iterStrs;
            }
            
            outTxtFile << endl;
        }
        
        outTxtFile.flush();
        outTxtFile.close();
    }
    
    
    void RSGISAttributeTable::exportGDALRaster(GDALDataset *inDataset, unsigned int inBand) throw(RSGISAttributeTableException)
    {
        try
        {
            GDALRasterAttributeTable *rasterAtt = new GDALRasterAttributeTable();
            
            rasterAtt->CreateColumn("FID", GFT_Integer, GFU_MinMax);
            unsigned int fidRATIdx = rasterAtt->GetColumnCount()-1;
            
            vector<pair<unsigned int, unsigned int> > intFieldRATIdxs;
            vector<pair<unsigned int, unsigned int> > floatFieldRATIdxs;
            vector<pair<unsigned int, unsigned int> > stringFieldRATIdxs;
            vector<pair<unsigned int, unsigned int> > boolFieldRATIdxs;
            
            cout << "There are " << fields->size() << " fields within the attribute table\n";
            
            vector<pair<string, RSGISAttributeDataType> >::iterator iterFields;
            for(iterFields = fields->begin(); iterFields != fields->end(); ++iterFields)
            {
                cout << "Adding Field: " << (*iterFields).first << endl;
                if((*iterFields).second == rsgis_int)
                {
                    rasterAtt->CreateColumn((*iterFields).first.c_str(), GFT_Integer, GFU_Generic);
                    intFieldRATIdxs.push_back(pair<unsigned int, unsigned int>(this->getFieldIndex((*iterFields).first), rasterAtt->GetColumnCount()-1));
                }
                else if((*iterFields).second == rsgis_float)
                {
                    rasterAtt->CreateColumn((*iterFields).first.c_str(), GFT_Real, GFU_Generic);
                    floatFieldRATIdxs.push_back(pair<unsigned int, unsigned int>(this->getFieldIndex((*iterFields).first), rasterAtt->GetColumnCount()-1));
                }
                else if((*iterFields).second == rsgis_string)
                {
                    rasterAtt->CreateColumn((*iterFields).first.c_str(), GFT_String, GFU_Generic);
                    stringFieldRATIdxs.push_back(pair<unsigned int, unsigned int>(this->getFieldIndex((*iterFields).first), rasterAtt->GetColumnCount()-1));
                }
                else if((*iterFields).second == rsgis_bool)
                {
                    rasterAtt->CreateColumn((*iterFields).first.c_str(), GFT_Integer, GFU_Generic);
                    boolFieldRATIdxs.push_back(pair<unsigned int, unsigned int>(this->getFieldIndex((*iterFields).first), rasterAtt->GetColumnCount()-1));
                }
                else
                {
                    cout << "Warning: " << (*iterFields).first << " will not be output as type unknown.\n";
                }
            }
            
            rasterAtt->SetRowCount(this->getSize());
            
            unsigned long long rowCounter = 0;
            for(vector<RSGISFeature*>::iterator iterFeat = attTable->begin(); iterFeat != attTable->end(); ++iterFeat)
            {
                rasterAtt->SetValue(rowCounter, fidRATIdx, ((int)(*iterFeat)->fid+1));
                
                for(vector<pair<unsigned int, unsigned int> >::iterator iterAtts = intFieldRATIdxs.begin(); iterAtts != intFieldRATIdxs.end(); ++iterAtts)
                {
                    rasterAtt->SetValue(rowCounter, (*iterAtts).second, ((int)(*iterFeat)->intFields->at((*iterAtts).first)));
                }
                
                for(vector<pair<unsigned int, unsigned int> >::iterator iterAtts = floatFieldRATIdxs.begin(); iterAtts != floatFieldRATIdxs.end(); ++iterAtts)
                {
                    rasterAtt->SetValue(rowCounter, (*iterAtts).second, ((float)(*iterFeat)->floatFields->at((*iterAtts).first)));
                }
                
                for(vector<pair<unsigned int, unsigned int> >::iterator iterAtts = stringFieldRATIdxs.begin(); iterAtts != stringFieldRATIdxs.end(); ++iterAtts)
                {
                    rasterAtt->SetValue(rowCounter, (*iterAtts).second, (*iterFeat)->stringFields->at((*iterAtts).first).c_str());
                }
                
                for(vector<pair<unsigned int, unsigned int> >::iterator iterAtts = boolFieldRATIdxs.begin(); iterAtts != boolFieldRATIdxs.end(); ++iterAtts)
                {
                    rasterAtt->SetValue(rowCounter, (*iterAtts).second, ((int)(*iterFeat)->boolFields->at((*iterAtts).first)));
                }
                
                ++rowCounter;
            }
            
            GDALRasterBand *ratBand = inDataset->GetRasterBand(inBand);
            ratBand->SetDefaultRAT(rasterAtt);
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(exception &e)
        {
            cout << "ERROR: " << e.what() << endl;
        }
    }
    
    vector<double>* RSGISAttributeTable::getDoubleField(string field) throw(RSGISAttributeTableException)
    {
        vector<double> *vals = new vector<double>();
        if(this->getDataType(field) == rsgis_float)
        {
            unsigned int idx = this->getFieldIndex(field);
            for(vector<RSGISFeature*>::iterator iterFeat = attTable->begin(); iterFeat != attTable->end(); ++iterFeat)
            {
                vals->push_back((*iterFeat)->floatFields->at(idx));
            }
        }
        else
        {
            throw RSGISAttributeTableException("Field must be of type float.");
        }
        
        return vals;
    }
    
    vector<long>* RSGISAttributeTable::getLongField(string field) throw(RSGISAttributeTableException)
    {
        vector<long> *vals = new vector<long>();
        if(this->getDataType(field) == rsgis_int)
        {
            unsigned int idx = this->getFieldIndex(field);
            for(vector<RSGISFeature*>::iterator iterFeat = attTable->begin(); iterFeat != attTable->end(); ++iterFeat)
            {
                vals->push_back((*iterFeat)->intFields->at(idx));
            }
        }
        else
        {
            throw RSGISAttributeTableException("Field must be of type int.");
        }
        
        return vals;
    }
    
    vector<bool>* RSGISAttributeTable::getBoolField(string field) throw(RSGISAttributeTableException)
    {
        vector<bool> *vals = new vector<bool>();
        if(this->getDataType(field) == rsgis_bool)
        {
            unsigned int idx = this->getFieldIndex(field);
            for(vector<RSGISFeature*>::iterator iterFeat = attTable->begin(); iterFeat != attTable->end(); ++iterFeat)
            {
                vals->push_back((*iterFeat)->boolFields->at(idx));
            }
        }
        else
        {
            throw RSGISAttributeTableException("Field must be of type boolean.");
        }
        
        return vals;
    }
     
    vector<string>* RSGISAttributeTable::getStringField(string field) throw(RSGISAttributeTableException)
    {
        vector<string> *vals = new vector<string>();
        if(this->getDataType(field) == rsgis_string)
        {
            unsigned int idx = this->getFieldIndex(field);
            for(vector<RSGISFeature*>::iterator iterFeat = attTable->begin(); iterFeat != attTable->end(); ++iterFeat)
            {
                vals->push_back((*iterFeat)->stringFields->at(idx));
            }
        }
        else
        {
            throw RSGISAttributeTableException("Field must be of type string.");
        }
        
        return vals;
    }
    
    
    RSGISAttributeTable* RSGISAttributeTable::importFromASCII(string inFile)throw(RSGISAttributeTableException)
    {
        RSGISAttributeTable *attTableObj = new RSGISAttributeTable();

        try
        {
            RSGISTextUtils txtUtils;
            RSGISTextFileLineReader txtReader;
            txtReader.openFile(inFile);
            
            vector<string> *tokens = new vector<string>();
            string line = "";
            unsigned int lineCount = 0;
            unsigned int numOfFields = 0;
            unsigned int numOfExpTokens = 0;
            unsigned int tokenIdx = 0;
            unsigned long long numOfFeatures = 0;
            
            unsigned int tmpNumBoolFields = 0;
            unsigned int tmpNumIntFields = 0;
            unsigned int tmpNumFloatFields = 0;
            unsigned int tmpNumStrFields = 0;
            
            while(!txtReader.endOfFile())
            {
                tokens->clear();
                line = txtReader.readLine();
                if((!txtUtils.lineStart(line, '#')) & (!txtUtils.blankline(line)))
                {
                    if(lineCount == 0)
                    {
                        numOfFeatures = txtUtils.strto64bitUInt(line);
                        attTableObj->attTable = new vector<RSGISFeature*>();
                        attTableObj->attTable->reserve(numOfFeatures);
                        
                        attTableObj->fieldIdx = new map<string, unsigned int>();
                        attTableObj->fieldDataType = new map<string, RSGISAttributeDataType>();
                        attTableObj->fields = new vector<pair<string, RSGISAttributeDataType> >();
                    }
                    else if(lineCount == 1)
                    {
                        txtUtils.tokenizeString(line, ':', tokens, true, true);
                        if(tokens->size() != 2)
                        {
                            cout << "Line: " << line << endl;
                            throw RSGISAttributeTableException("Expected two tokens.");
                        }
                        attTableObj->numBoolFields = txtUtils.strto32bitUInt(tokens->at(1));
                        numOfFields += attTableObj->numBoolFields;
                    }
                    else if(lineCount == 2)
                    {
                        txtUtils.tokenizeString(line, ':', tokens, true, true);
                        if(tokens->size() != 2)
                        {
                            cout << "Line: " << line << endl;
                            throw RSGISAttributeTableException("Expected two tokens.");
                        }
                        attTableObj->numIntFields = txtUtils.strto32bitUInt(tokens->at(1));
                        numOfFields += attTableObj->numIntFields;
                    }
                    else if(lineCount == 3)
                    {
                        txtUtils.tokenizeString(line, ':', tokens, true, true);
                        if(tokens->size() != 2)
                        {
                            cout << "Line: " << line << endl;
                            throw RSGISAttributeTableException("Expected two tokens.");
                        }
                        attTableObj->numFloatFields = txtUtils.strto32bitUInt(tokens->at(1));
                        numOfFields += attTableObj->numFloatFields;
                    }
                    else if(lineCount == 4)
                    {
                        txtUtils.tokenizeString(line, ':', tokens, true, true);
                        if(tokens->size() != 2)
                        {
                            cout << "Line: " << line << endl;
                            throw RSGISAttributeTableException("Expected two tokens.");
                        }
                        attTableObj->numStrFields = txtUtils.strto32bitUInt(tokens->at(1));
                        numOfFields += attTableObj->numStrFields;
                    }
                    else if((lineCount > 4) & (lineCount < (5+numOfFields)))
                    {
                        txtUtils.tokenizeString(line, ',', tokens, true, true);
                        if(tokens->size() != 2)
                        {
                            cout << "Line: " << line << endl;
                            throw RSGISAttributeTableException("Expected two tokens.");
                        }
                        
                        if(tokens->at(1) == "rsgis_bool")
                        {
                            attTableObj->fieldIdx->insert(pair<string, unsigned int>(tokens->at(0), tmpNumBoolFields));
                            attTableObj->fieldDataType->insert(pair<string, RSGISAttributeDataType>(tokens->at(0), rsgis_bool));
                            attTableObj->fields->push_back(pair<string, RSGISAttributeDataType>(tokens->at(0), rsgis_bool));
                            ++tmpNumBoolFields;

                        }
                        else if(tokens->at(1) == "rsgis_int")
                        {
                            attTableObj->fieldIdx->insert(pair<string, unsigned int>(tokens->at(0), tmpNumIntFields));
                            attTableObj->fieldDataType->insert(pair<string, RSGISAttributeDataType>(tokens->at(0), rsgis_int));
                            attTableObj->fields->push_back(pair<string, RSGISAttributeDataType>(tokens->at(0), rsgis_int));
                            ++tmpNumIntFields;
                        }
                        else if(tokens->at(1) == "rsgis_float")
                        {
                            attTableObj->fieldIdx->insert(pair<string, unsigned int>(tokens->at(0), tmpNumFloatFields));
                            attTableObj->fieldDataType->insert(pair<string, RSGISAttributeDataType>(tokens->at(0), rsgis_float));
                            attTableObj->fields->push_back(pair<string, RSGISAttributeDataType>(tokens->at(0), rsgis_float));
                            ++tmpNumFloatFields;
                        }
                        else if(tokens->at(1) == "rsgis_string")
                        {
                            attTableObj->fieldIdx->insert(pair<string, unsigned int>(tokens->at(0), tmpNumStrFields));
                            attTableObj->fieldDataType->insert(pair<string, RSGISAttributeDataType>(tokens->at(0), rsgis_string));
                            attTableObj->fields->push_back(pair<string, RSGISAttributeDataType>(tokens->at(0), rsgis_string));
                            ++tmpNumStrFields;
                        }
                        else
                        {
                            cout << "Line: " << line << endl;
                            throw RSGISAttributeTableException("Did not recognise the data type.");
                        }
                    }
                    else
                    {
                        if(lineCount == (5+numOfFields))
                        {
                            if(tmpNumBoolFields != attTableObj->numBoolFields)
                            {
                                throw RSGISAttributeTableException("Number of bool fields did not match.");
                            }
                            
                            if(tmpNumIntFields != attTableObj->numIntFields)
                            {
                                throw RSGISAttributeTableException("Number of int fields did not match.");
                            }
                            
                            if(tmpNumFloatFields != attTableObj->numFloatFields)
                            {
                                throw RSGISAttributeTableException("Number of float fields did not match.");
                            }
                            
                            if(tmpNumStrFields != attTableObj->numStrFields)
                            {
                                throw RSGISAttributeTableException("Number of string fields did not match.");
                            }
                            
                            numOfExpTokens = numOfFields+1;
                        }
                        
                        txtUtils.tokenizeString(line, ',', tokens, true, true);
                        if(tokens->size() != numOfExpTokens)
                        {
                            cout << "Line: " << line << endl;
                            throw RSGISAttributeTableException("Incorrect number of expected tokens.");
                        }
                        tokenIdx = 0;
                        
                        RSGISFeature *feature = new RSGISFeature();
                        feature->fid = txtUtils.strto64bitUInt(tokens->at(tokenIdx++));
                        feature->boolFields = new vector<bool>();
                        feature->boolFields->reserve(attTableObj->numBoolFields);
                        for(unsigned int i = 0; i < attTableObj->numBoolFields; ++i)
                        {
                            if(tokens->at(tokenIdx++) == "TRUE")
                            {
                                feature->boolFields->push_back(true);
                            }
                            else
                            {
                                feature->boolFields->push_back(false);
                            }
                        }            
                        feature->intFields = new vector<long>();
                        feature->intFields->reserve(attTableObj->numIntFields);
                        for(unsigned int i = 0; i < attTableObj->numIntFields; ++i)
                        {
                            feature->intFields->push_back(txtUtils.strto32bitUInt(tokens->at(tokenIdx++)));
                        }
                        feature->floatFields = new vector<double>();
                        feature->floatFields->reserve(attTableObj->numFloatFields);
                        for(unsigned int i = 0; i < attTableObj->numFloatFields; ++i)
                        {
                            try 
                            {
                                feature->floatFields->push_back(txtUtils.strtodouble(tokens->at(tokenIdx)));
                                ++tokenIdx;
                            }
                            catch(RSGISTextException &e)
                            {
                                if(tokens->at(tokenIdx) == "nan")
                                {
                                    feature->floatFields->push_back(numeric_limits<float>::signaling_NaN());
                                    ++tokenIdx;
                                }
                                else
                                {
                                    throw e;
                                }
                            }
                        }
                        feature->stringFields = new vector<string>();
                        feature->stringFields->reserve(attTableObj->numStrFields);
                        for(unsigned int i = 0; i < attTableObj->numStrFields; ++i)
                        {
                            feature->stringFields->push_back(tokens->at(tokenIdx++));
                        }
                        
                        attTableObj->attTable->push_back(feature);                        
                    }
                    ++lineCount;
                }
            }
        }
        catch(RSGISTextException &e)
        {
            delete attTableObj;
            throw RSGISAttributeTableException(e.what());
        }
        
        return attTableObj;
    }
    
    
    RSGISAttributeTable* RSGISAttributeTable::importFromGDALRaster(string inFile)throw(RSGISAttributeTableException)
    {
        return NULL;
    }
    
    vector<RSGISIfStatement*>* RSGISAttributeTable::generateStatments(DOMElement *argElement)throw(RSGISAttributeTableException)
    {
        vector<RSGISIfStatement*> *statements = new vector<RSGISIfStatement*>();
        
        try
        {
            RSGISTextUtils textUtils;
            XMLCh *rsgisIfXMLStr = XMLString::transcode("rsgis:if");
            DOMNodeList *ifNodesList = argElement->getElementsByTagName(rsgisIfXMLStr);
            unsigned int numIfStatments = ifNodesList->getLength();        
            
            if(numIfStatments == 0)
            {
                throw RSGISAttributeTableException("Must have at least 1 if statment.");
            }
            
            XMLCh *rsgisElseXMLStr = XMLString::transcode("rsgis:else");
            DOMNodeList *elseNodesList = argElement->getElementsByTagName(rsgisElseXMLStr);
            unsigned int numElseStatments = elseNodesList->getLength();        
            
            if(numElseStatments != 1)
            {
                throw RSGISAttributeTableException("Must have at 1 and only 1 else statment.");
            }
            
            XMLCh *fieldXMLStr = XMLString::transcode("field");
            XMLCh *valueXMLStr = XMLString::transcode("value");
            XMLCh *typeXMLStr = XMLString::transcode("type");

            
            unsigned int numChildElements = argElement->getChildElementCount();
            DOMElement *ifElement = argElement->getFirstElementChild();
            
            for(boost::uint_fast32_t i = 0; i < numChildElements; ++i)
            {
                const XMLCh* tagName = ifElement->getTagName();
                if(XMLString::equals(tagName, rsgisIfXMLStr))
                {
                    if(ifElement->getChildElementCount() != 1)
                    {
                        throw RSGISAttributeTableException("An \'if statement\' must have 1 and only 1 child expression (using AND or OR to join expressions).");
                    }
                    
                    RSGISIfStatement *ifStatment = new RSGISIfStatement();
                    ifStatment->exp = RSGISAttributeTable::generateExpression(ifElement->getFirstElementChild());
                    ifStatment->noExp = false;
                    ifStatment->ignore = false;

                    if(ifElement->hasAttribute(fieldXMLStr))
                    {
                        char *charValue = XMLString::transcode(ifElement->getAttribute(fieldXMLStr));
                        ifStatment->field = string(charValue);
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                    }
                    
                    if(ifElement->hasAttribute(valueXMLStr))
                    {
                        char *charValue = XMLString::transcode(ifElement->getAttribute(valueXMLStr));
                        ifStatment->value = textUtils.strto32bitInt(string(charValue));
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                    }
                    statements->push_back(ifStatment);
                }
                else if(XMLString::equals(tagName, rsgisElseXMLStr))
                {
                    RSGISIfStatement *elseStatment = new RSGISIfStatement();
                    elseStatment->exp = NULL;
                    elseStatment->noExp = true;
                    
                    bool fieldSet = false;
                    bool valueSet = false;
                    
                    if(ifElement->hasAttribute(fieldXMLStr))
                    {
                        char *charValue = XMLString::transcode(ifElement->getAttribute(fieldXMLStr));
                        elseStatment->field = string(charValue);
                        fieldSet = true;
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        fieldSet = false;
                        //throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                    }
                    
                    if(ifElement->hasAttribute(valueXMLStr))
                    {
                        char *charValue = XMLString::transcode(ifElement->getAttribute(valueXMLStr));
                        elseStatment->value = textUtils.strto32bitInt(string(charValue));
                        valueSet = true;
                        XMLString::release(&charValue);
                    }
                    else
                    {
                        valueSet = false;
                        //throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                    }
                    
                    if(!fieldSet | !valueSet)
                    {
                        elseStatment->ignore = true;
                    }
                    else
                    {
                        elseStatment->ignore = false;
                    }
                    
                    statements->push_back(elseStatment);
                }
                else
                {
                    throw RSGISAttributeTableException("Tag was not recongised. Only rsgis:if and rsgis:else are recognised at this level.");
                }
                
                ifElement = ifElement->getNextElementSibling();
            }
            
			XMLString::release(&fieldXMLStr);
            XMLString::release(&valueXMLStr);
            XMLString::release(&typeXMLStr);
            XMLString::release(&rsgisIfXMLStr);
            XMLString::release(&rsgisElseXMLStr);
            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        
        return statements;
    }
    
    
    RSGISAttExpression* RSGISAttributeTable::generateExpression(DOMElement *expElement)throw(RSGISAttributeTableException)
    {
        RSGISAttExpression *exp = NULL;
        
        try 
        {
            RSGISTextUtils textUtils;
            XMLCh *rsgisExpXMLStr = XMLString::transcode("rsgis:exp");
            const XMLCh* tagName = expElement->getTagName();
            if(XMLString::equals(tagName, rsgisExpXMLStr))
            {
                XMLCh *andXMLStr = XMLString::transcode("and");
                XMLCh *orXMLStr = XMLString::transcode("or");
                XMLCh *notXMLStr = XMLString::transcode("not");
                XMLCh *gtXMLStr = XMLString::transcode("gt");
                XMLCh *ltXMLStr = XMLString::transcode("lt");
                XMLCh *gteqXMLStr = XMLString::transcode("gteq");
                XMLCh *lteqXMLStr = XMLString::transcode("lteq");
                XMLCh *eqXMLStr = XMLString::transcode("eq");
                XMLCh *neqXMLStr = XMLString::transcode("neq");
                XMLCh *gtConstXMLStr = XMLString::transcode("gtconst");
                XMLCh *ltConstXMLStr = XMLString::transcode("ltconst");
                XMLCh *gteqConstXMLStr = XMLString::transcode("gteqconst");
                XMLCh *lteqConstXMLStr = XMLString::transcode("lteqconst");
                XMLCh *eqConstXMLStr = XMLString::transcode("eqconst");
                XMLCh *neqConstXMLStr = XMLString::transcode("netconst");
                XMLCh *constGTXMLStr = XMLString::transcode("constgt");
                XMLCh *constLTXMLStr = XMLString::transcode("constlt");
                XMLCh *constGTEQXMLStr = XMLString::transcode("constgteq");
                XMLCh *constLTEQXMLStr = XMLString::transcode("constlteq");
                
                XMLCh *field1XMLStr = XMLString::transcode("field1");
                XMLCh *field2XMLStr = XMLString::transcode("field2");
                XMLCh *fieldXMLStr = XMLString::transcode("field");
                XMLCh *valueXMLStr = XMLString::transcode("value");
                XMLCh *typeXMLStr = XMLString::transcode("type");
                
                
                string f1Name = "";
                unsigned int f1Idx = 0;
                RSGISAttributeDataType f1Type = rsgis_na;
                string f2Name = "";
                unsigned int f2Idx = 0;
                RSGISAttributeDataType f2Type = rsgis_na;
                float value = 0;
                
                if(expElement->hasAttribute(typeXMLStr))
                {
                    const XMLCh *typeInXMLStr = expElement->getAttribute(typeXMLStr);
                    
                    if(XMLString::equals(typeInXMLStr, andXMLStr))
                    {
                        unsigned int numChildElements = expElement->getChildElementCount();
                        if(numChildElements == 0)
                        {
                            throw RSGISAttributeTableException("There must be at least one child element of a AND expression.");
                        }
                        
                        vector<RSGISAttExpression*> *exps = new vector<RSGISAttExpression*>();
                        
                        DOMElement *expElementChild = expElement->getFirstElementChild();
                        for(unsigned int i = 0; i < numChildElements; ++i)
                        {
                            exps->push_back(RSGISAttributeTable::generateExpression(expElementChild));
                            
                            expElementChild = expElementChild->getNextElementSibling();
                        }
                        
                        exp = new RSGISAttExpressionAND(exps);
                    }
                    else if(XMLString::equals(typeInXMLStr, orXMLStr))
                    {
                        unsigned int numChildElements = expElement->getChildElementCount();
                        if(numChildElements == 0)
                        {
                            throw RSGISAttributeTableException("There must be at least one child element of a OR expression.");
                        }
                        
                        vector<RSGISAttExpression*> *exps = new vector<RSGISAttExpression*>();
                        
                        DOMElement *expElementChild = expElement->getFirstElementChild();
                        for(unsigned int i = 0; i < numChildElements; ++i)
                        {
                            exps->push_back(RSGISAttributeTable::generateExpression(expElementChild));
                            
                            expElementChild = expElementChild->getNextElementSibling();
                        }
                        
                        exp = new RSGISAttExpressionOR(exps);
                    }
                    else if(XMLString::equals(typeInXMLStr, notXMLStr))
                    {
                        unsigned int numChildElements = expElement->getChildElementCount();
                        if(numChildElements != 1)
                        {
                            throw RSGISAttributeTableException("There can only be one child element of a NOT expression.");
                        }                        
                        exp = new RSGISAttExpressionNot(RSGISAttributeTable::generateExpression(expElement->getFirstElementChild()));
                    }
                    else if(XMLString::equals(typeInXMLStr, gtXMLStr))
                    {
                        if(expElement->hasAttribute(field1XMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(field1XMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field1\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(field2XMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(field2XMLStr));
                            f2Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field2\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionGreaterThan(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type);
                    }
                    else if(XMLString::equals(typeInXMLStr, ltXMLStr))
                    {
                        if(expElement->hasAttribute(field1XMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(field1XMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field1\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(field2XMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(field2XMLStr));
                            f2Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field2\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionLessThan(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type);
                    }
                    else if(XMLString::equals(typeInXMLStr, gteqXMLStr))
                    {
                        if(expElement->hasAttribute(field1XMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(field1XMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field1\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(field2XMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(field2XMLStr));
                            f2Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field2\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionGreaterThanEq(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type);
                    }
                    else if(XMLString::equals(typeInXMLStr, lteqXMLStr))
                    {
                        if(expElement->hasAttribute(field1XMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(field1XMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field1\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(field2XMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(field2XMLStr));
                            f2Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field2\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionLessThanEq(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type);
                    }
                    else if(XMLString::equals(typeInXMLStr, eqXMLStr))
                    {
                        if(expElement->hasAttribute(field1XMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(field1XMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field1\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(field2XMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(field2XMLStr));
                            f2Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field2\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionEquals(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type);
                    }
                    else if(XMLString::equals(typeInXMLStr, neqXMLStr))
                    {
                        if(expElement->hasAttribute(field1XMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(field1XMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field1\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(field2XMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(field2XMLStr));
                            f2Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field2\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionNotEquals(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type);
                    }
                    else if(XMLString::equals(typeInXMLStr, gtConstXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(string(charValue));
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionGreaterThanConst(f1Name, f1Idx, f1Type, value);
                    }
                    else if(XMLString::equals(typeInXMLStr, ltConstXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(string(charValue));
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionLessThanConst(f1Name, f1Idx, f1Type, value);
                    }
                    else if(XMLString::equals(typeInXMLStr, gteqConstXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(string(charValue));
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionGreaterThanConstEq(f1Name, f1Idx, f1Type, value);
                    }
                    else if(XMLString::equals(typeInXMLStr, lteqConstXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(string(charValue));
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionLessThanConstEq(f1Name, f1Idx, f1Type, value);
                    }
                    else if(XMLString::equals(typeInXMLStr, eqConstXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(string(charValue));
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionEqualsConst(f1Name, f1Idx, f1Type, value);
                    }
                    else if(XMLString::equals(typeInXMLStr, neqConstXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(string(charValue));
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionNotEqualsConst(f1Name, f1Idx, f1Type, value);
                    }
                    else if(XMLString::equals(typeInXMLStr, constGTXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(string(charValue));
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionConstGreaterThan(f1Name, f1Idx, f1Type, value);
                    }
                    else if(XMLString::equals(typeInXMLStr, constLTXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(string(charValue));
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionConstLessThan(f1Name, f1Idx, f1Type, value);
                    }
                    else if(XMLString::equals(typeInXMLStr, constGTEQXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(string(charValue));
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionConstGreaterThanEq(f1Name, f1Idx, f1Type, value);
                    }
                    else if(XMLString::equals(typeInXMLStr, constLTEQXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = string(charValue);
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(string(charValue));
                            XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionConstLessThanEq(f1Name, f1Idx, f1Type, value);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("The expression type is not recognised.");
                    }
                    
                }
                else
                {
                    throw RSGISAttributeTableException("No \'type\' attribute was provided for else.");
                }
                
                XMLString::release(&typeXMLStr);
                XMLString::release(&valueXMLStr);
                XMLString::release(&fieldXMLStr);
                XMLString::release(&field2XMLStr);
                XMLString::release(&field1XMLStr);
                XMLString::release(&andXMLStr);
                XMLString::release(&orXMLStr);
                XMLString::release(&notXMLStr);
                XMLString::release(&gtXMLStr);
                XMLString::release(&ltXMLStr);
                XMLString::release(&gteqXMLStr);
                XMLString::release(&lteqXMLStr);
                XMLString::release(&eqXMLStr);
                XMLString::release(&neqXMLStr);
                XMLString::release(&gtConstXMLStr);
                XMLString::release(&ltConstXMLStr);
                XMLString::release(&gteqConstXMLStr);
                XMLString::release(&lteqConstXMLStr);
                XMLString::release(&eqConstXMLStr);
                XMLString::release(&neqConstXMLStr);
                XMLString::release(&constGTXMLStr);
                XMLString::release(&constLTXMLStr);
                XMLString::release(&constGTEQXMLStr);
                XMLString::release(&constLTEQXMLStr);
            }
            else
            {
                throw RSGISAttributeTableException("Inputted element needs to be of the type rsgis:exp.");
            }
        }
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        
        return exp;
    }
        
    RSGISAttributeTable::~RSGISAttributeTable()
    {
        if(fieldDataType != NULL)
        {
            delete fieldDataType;
        }

        if(fieldIdx != NULL)
        {
            delete fieldIdx;
        }
        
        if(fields != NULL)
        {
            delete fields;
        }
        
        if(attTable != NULL)
        {
            for(vector<RSGISFeature*>::iterator iterFeats = attTable->begin(); iterFeats != attTable->end(); ++iterFeats)
            {
                delete (*iterFeats)->boolFields;
                delete (*iterFeats)->intFields;
                delete (*iterFeats)->floatFields;
                delete (*iterFeats)->stringFields;
                delete (*iterFeats);
            }
            
            delete attTable;
        }

    }
    
}}




