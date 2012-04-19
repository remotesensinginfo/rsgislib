/*
 *  RSGISAttributeTableMem.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/04/2012.
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

#include "RSGISAttributeTableMem.h"

namespace rsgis{namespace rastergis{
    
    
    RSGISAttributeTableMem::RSGISAttributeTableMem()
    {
        
    }
    
    RSGISAttributeTableMem::RSGISAttributeTableMem(unsigned long long numFeatures)
    {
        this->createAttributeTable(numFeatures);
    }
    
    RSGISAttributeTableMem::RSGISAttributeTableMem(unsigned long long numFeatures, vector<pair<string, RSGISAttributeDataType> > *fields)
    {
        this->fields = fields;
        this->createAttributeTableWithFields(numFeatures);
    }
    
    void RSGISAttributeTableMem::createAttributeTableWithFields(unsigned long long numFeatures)
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
            feature->neighbours = new vector<unsigned long long>();
            
            attTable->push_back(feature);
        }
    }
    
    void RSGISAttributeTableMem::createAttributeTable(unsigned long long numFeatures)
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
            feature->neighbours = new vector<unsigned long long>();
            
            attTable->push_back(feature);
        }
    }
    
    bool RSGISAttributeTableMem::getBoolField(unsigned long long fid, string name) throw(RSGISAttributeTableException)
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
    
    
    long RSGISAttributeTableMem::getIntField(unsigned long long fid, string name) throw(RSGISAttributeTableException)
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
    
    double RSGISAttributeTableMem::getDoubleField(unsigned long long fid, string name) throw(RSGISAttributeTableException)
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
    
    string RSGISAttributeTableMem::getStringField(unsigned long long fid, string name) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::setBoolField(unsigned long long fid, string name, bool value) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::setIntField(unsigned long long fid, string name, long value) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::setDoubleField(unsigned long long fid, string name, double value) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::setStringField(unsigned long long fid, string name, string value) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::setBoolValue(string name, bool value) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::setIntValue(string name, long value) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::setFloatValue(string name, double value) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::setStringValue(string name, string value) throw(RSGISAttributeTableException)
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
    
    RSGISFeature* RSGISAttributeTableMem::getFeature(unsigned long long fid) throw(RSGISAttributeTableException)
    {
        if(fid >= attTable->size())
        {
            RSGISTextUtils textUtils;
            string message = string("Feature (") + textUtils.uInt64bittostring(fid) + string(") is not within the attribute table.");
            throw RSGISAttributeTableException(message);
        }
        
        return attTable->at(fid);
    }
    
    void RSGISAttributeTableMem::addAttBoolField(string name, bool val)
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
    
    void RSGISAttributeTableMem::addAttIntField(string name, long val)
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
    
    void RSGISAttributeTableMem::addAttFloatField(string name, double val)
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
    
    void RSGISAttributeTableMem::addAttStringField(string name, string val)
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
    
    void RSGISAttributeTableMem::addAttributes(vector<RSGISAttribute*> *attributes) throw(RSGISAttributeTableException)
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
        
    unsigned long long RSGISAttributeTableMem::getSize()
    {
        return attTable->size();
    }
    
    void RSGISAttributeTableMem::operator++()
    {
        ++iterIdx;
    }
    
    void RSGISAttributeTableMem::start()
    {
        iterIdx = 0;
    }
    
    bool RSGISAttributeTableMem::end()
    {
        return iterIdx < attTable->size();
    }
    
    RSGISFeature* RSGISAttributeTableMem::operator*()
    {
        return attTable->at(iterIdx);
    }
    
    RSGISAttributeTable* RSGISAttributeTableMem::importFromASCII(string inFile)throw(RSGISAttributeTableException)
    {
        RSGISAttributeTableMem *attTableObj = new RSGISAttributeTableMem();
        
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
            unsigned long long numOfDataLines = 0;
            unsigned long long tmpFid = 0;
            
            unsigned int tmpNumBoolFields = 0;
            unsigned int tmpNumIntFields = 0;
            unsigned int tmpNumFloatFields = 0;
            unsigned int tmpNumStrFields = 0;
            
            RSGISFeature *feature = NULL;
            
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
                        numOfDataLines = (5+numOfFields) + numOfFeatures;
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
                    else if(lineCount < numOfDataLines)
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
                        
                        feature = new RSGISFeature();
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
                        feature->neighbours = new vector<unsigned long long>();
                        
                        attTableObj->attTable->push_back(feature);                        
                    }
                    else
                    {
                        txtUtils.tokenizeString(line, ',', tokens, true, true);
                        if(tokens->size() ==  0)
                        {
                            cout << "Line: " << line << endl;
                            throw RSGISAttributeTableException("Incorrect number of expected tokens for neighbours.");
                        }
                        tokenIdx = 0;
                        for(unsigned i = 0; i < tokens->size(); ++i)
                        {
                            if(i == 0)
                            {
                                tmpFid = txtUtils.strto64bitUInt(tokens->at(tokenIdx++));
                                feature = attTableObj->getFeature(tmpFid);
                                feature->neighbours->reserve(tokens->size()-1);
                            }
                            else
                            {
                                feature->neighbours->push_back(txtUtils.strto64bitUInt(tokens->at(tokenIdx++)));
                            }
                        }
                        
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
    
    RSGISAttributeTable* RSGISAttributeTableMem::importFromGDALRaster(string inFile)throw(RSGISAttributeTableException)
    {
        return NULL;
    }
    
    RSGISAttributeTableMem::~RSGISAttributeTableMem()
    {
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




