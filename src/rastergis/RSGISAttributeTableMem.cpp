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
    
    RSGISAttributeTableMem::RSGISAttributeTableMem(size_t numFeatures)
    {
        this->createAttributeTable(numFeatures);
    }
    
    RSGISAttributeTableMem::RSGISAttributeTableMem(size_t numFeatures, vector<pair<string, RSGISAttributeDataType> > *fields)
    {
        this->fields = fields;
        this->createAttributeTableWithFields(numFeatures);
    }
    
    void RSGISAttributeTableMem::createAttributeTableWithFields(size_t numFeatures)
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
        for(size_t i = 0; i < numFeatures; ++i)
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
            feature->neighbours = new vector<size_t>();
            
            attTable->push_back(feature);
        }
    }
    
    void RSGISAttributeTableMem::createAttributeTable(size_t numFeatures)
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
        for(size_t i = 0; i < numFeatures; ++i)
        {
            feature = new RSGISFeature();
            feature->fid = i;
            feature->boolFields = new vector<bool>();
            feature->intFields = new vector<long>();
            feature->floatFields = new vector<double>();
            feature->stringFields = new vector<string>();
            feature->neighbours = new vector<size_t>();
            
            attTable->push_back(feature);
        }
    }
    
    bool RSGISAttributeTableMem::getBoolField(size_t fid, string name) throw(RSGISAttributeTableException)
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
    
    
    long RSGISAttributeTableMem::getIntField(size_t fid, string name) throw(RSGISAttributeTableException)
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
    
    double RSGISAttributeTableMem::getDoubleField(size_t fid, string name) throw(RSGISAttributeTableException)
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
    
    string RSGISAttributeTableMem::getStringField(size_t fid, string name) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::setBoolField(size_t fid, string name, bool value) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::setIntField(size_t fid, string name, long value) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::setDoubleField(size_t fid, string name, double value) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::setStringField(size_t fid, string name, string value) throw(RSGISAttributeTableException)
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
    
    RSGISFeature* RSGISAttributeTableMem::getFeature(size_t fid) throw(RSGISAttributeTableException)
    {
        if(fid >= attTable->size())
        {
            RSGISTextUtils textUtils;
            string message = string("Feature (") + textUtils.uInt64bittostring(fid) + string(") is not within the attribute table.");
            throw RSGISAttributeTableException(message);
        }
        
        return attTable->at(fid);
    }
    
    void RSGISAttributeTableMem::flushAllFeatures(bool progressFeedback) throw(RSGISAttributeTableException)
    {
        // Nothing to do as features are stored in memory to automatically sync'd.
    }
    
    void RSGISAttributeTableMem::addAttBoolField(string name, bool val) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::addAttIntField(string name, long val) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::addAttFloatField(string name, double val) throw(RSGISAttributeTableException)
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
    
    void RSGISAttributeTableMem::addAttStringField(string name, string val) throw(RSGISAttributeTableException)
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
        
    size_t RSGISAttributeTableMem::getSize()
    {
        return attTable->size();
    }
    
    void RSGISAttributeTableMem::holdFID(size_t fid)
    {
        // Do nothing as everything is memory to nothing to 'hold'
    }
    
    void RSGISAttributeTableMem::removeHoldFID(size_t fid)
    {
        // Do nothing as everything is memory to nothing to 'hold'
    }
    
    size_t RSGISAttributeTableMem::getNumOfBlocks()
    {
        return (attTable->size()/ATT_WRITE_CHUNK_SIZE)+1;
    }
    
    void RSGISAttributeTableMem::loadBlocks(size_t startBlock, size_t endBlock) throw(RSGISAttributeTableException)
    {
        // Do nothing as all in memory...
    }
    
    bool RSGISAttributeTableMem::attInMemory()
    {
        return true;
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
    
    RSGISFileType RSGISAttributeTableMem::findFileType(string inFile)throw(RSGISAttributeTableException)
    {
        RSGISFileType fileType = rsgis_unknown_attft;
        try
        {
            Exception::dontPrint();
            H5File *attH5File = new H5File( inFile, H5F_ACC_RDONLY);
            attH5File->close();
            delete attH5File;
            
            fileType = rsgis_hdf_attft;
        }
        catch( Exception &e )
        {
            fileType = rsgis_ascii_attft;
        }
        
        return fileType;
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
            size_t numOfFeatures = 0;
            size_t numOfDataLines = 0;
            size_t tmpFid = 0;
            
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
                        feature->neighbours = new vector<size_t>();
                        
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
            //delete attTableObj;
            throw RSGISAttributeTableException(e.what());
        }
        
        return attTableObj;
    }
    
    RSGISAttributeTable* RSGISAttributeTableMem::importFromHDF5(string inFile)throw(RSGISAttributeTableException)
    {
        RSGISAttributeTableMem *attTableObj = new RSGISAttributeTableMem();
        
        try
        {
            //Exception::dontPrint();
            
            FileAccPropList attAccessPlist = FileAccPropList(FileAccPropList::DEFAULT);
            attAccessPlist.setCache(ATT_READ_MDC_NELMTS, ATT_READ_RDCC_NELMTS, ATT_READ_RDCC_NBYTES, ATT_READ_RDCC_W0);
            attAccessPlist.setSieveBufSize(ATT_READ_SIEVE_BUF);
            hsize_t metaBlockSize = ATT_READ_META_BLOCKSIZE;
            attAccessPlist.setMetaBlockSize(metaBlockSize);
            
            H5File *attH5File = new H5File( inFile, H5F_ACC_RDONLY, FileCreatPropList::DEFAULT, attAccessPlist);
                        
            bool hasBoolFields = true;
            bool hasIntFields = true;
            bool hasFloatFields = true;
            
            attTableObj->numStrFields = 0;
            
            attTableObj->fields = new vector<pair<string, RSGISAttributeDataType> >();
            attTableObj->fieldIdx = new map<string, unsigned int>();
            attTableObj->fieldDataType = new map<string, RSGISAttributeDataType>();
            
            attTableObj->numBoolFields = 0;
            attTableObj->numIntFields = 0;
            attTableObj->numFloatFields = 0;
            attTableObj->numStrFields = 0;
            
            hsize_t dimsAttSize[1];
			dimsAttSize[0] = 1;
            size_t numFeats = 0;
			DataSpace attSizeDataSpace(1, dimsAttSize);
            DataSet sizeDataset = attH5File->openDataSet( ATT_SIZE_HEADER );
            sizeDataset.read(&numFeats, PredType::NATIVE_ULLONG, attSizeDataSpace);
            attSizeDataSpace.close();
            
            cout << "Table has " << numFeats << " features\n";
            
            CompType *fieldCompTypeMem = attTableObj->createAttibuteIdxCompTypeMem();
            try
            {
                DataSet boolFieldsDataset = attH5File->openDataSet( ATT_BOOL_FIELDS_HEADER );
                DataSpace boolFieldsDataspace = boolFieldsDataset.getSpace();
                
                attTableObj->numBoolFields = boolFieldsDataspace.getSelectNpoints();
                
                cout << "There are " << attTableObj->numBoolFields << " boolean fields." << endl;
                
                RSGISAttributeIdx *fields = new RSGISAttributeIdx[attTableObj->numBoolFields];
                
                hsize_t boolFieldsDims[1]; 
                boolFieldsDims[0] = attTableObj->numBoolFields;
                DataSpace boolFieldsMemspace(1, boolFieldsDims);
                
                boolFieldsDataset.read(fields, *fieldCompTypeMem, boolFieldsMemspace, boolFieldsDataspace);
                
                for(unsigned int i = 0; i < attTableObj->numBoolFields; ++i)
                {
                    //cout << fields[i].name << ": " << fields[i].idx << endl;
                    attTableObj->fields->push_back(pair<string, RSGISAttributeDataType>(fields[i].name, rsgis_bool));
                    attTableObj->fieldIdx->insert(pair<string, unsigned int>(fields[i].name, fields[i].idx));
                    attTableObj->fieldDataType->insert(pair<string, RSGISAttributeDataType>(fields[i].name, rsgis_bool));
                }
                
                delete[] fields;
            }
            catch( Exception &e )
            {
                hasBoolFields = false;
            }
            
            try
            {
                DataSet intFieldsDataset = attH5File->openDataSet( ATT_INT_FIELDS_HEADER );
                DataSpace intFieldsDataspace = intFieldsDataset.getSpace();
                
                attTableObj->numIntFields = intFieldsDataspace.getSelectNpoints();
                
                cout << "There are " << attTableObj->numIntFields << " integer fields." << endl;
                
                RSGISAttributeIdx *fields = new RSGISAttributeIdx[attTableObj->numIntFields];
                
                hsize_t intFieldsDims[1]; 
                intFieldsDims[0] = attTableObj->numIntFields;
                DataSpace intFieldsMemspace(1, intFieldsDims);
                
                intFieldsDataset.read(fields, *fieldCompTypeMem, intFieldsMemspace, intFieldsDataspace);
                
                for(unsigned int i = 0; i < attTableObj->numIntFields; ++i)
                {
                    //cout << fields[i].name << ": " << fields[i].idx << endl;
                    attTableObj->fields->push_back(pair<string, RSGISAttributeDataType>(fields[i].name, rsgis_int));
                    attTableObj->fieldIdx->insert(pair<string, unsigned int>(fields[i].name, fields[i].idx));
                    attTableObj->fieldDataType->insert(pair<string, RSGISAttributeDataType>(fields[i].name, rsgis_int));
                }
                
                delete[] fields;
            }
            catch( Exception &e )
            {
                hasIntFields = false;
            }
            
            try
            {
                DataSet floatFieldsDataset = attH5File->openDataSet( ATT_FLOAT_FIELDS_HEADER );
                DataSpace floatFieldsDataspace = floatFieldsDataset.getSpace();
                
                attTableObj->numFloatFields = floatFieldsDataspace.getSelectNpoints();
                
                cout << "There are " << attTableObj->numFloatFields << " float fields." << endl;
                
                RSGISAttributeIdx *fields = new RSGISAttributeIdx[attTableObj->numFloatFields];
                
                hsize_t floatFieldsDims[1]; 
                floatFieldsDims[0] = attTableObj->numFloatFields;
                DataSpace floatFieldsMemspace(1, floatFieldsDims);
                
                floatFieldsDataset.read(fields, *fieldCompTypeMem, floatFieldsMemspace, floatFieldsDataspace);
                
                for(unsigned int i = 0; i < attTableObj->numFloatFields; ++i)
                {
                    //cout << fields[i].name << ": " << fields[i].idx << endl;
                    attTableObj->fields->push_back(pair<string, RSGISAttributeDataType>(fields[i].name, rsgis_float));
                    attTableObj->fieldIdx->insert(pair<string, unsigned int>(fields[i].name, fields[i].idx));
                    attTableObj->fieldDataType->insert(pair<string, RSGISAttributeDataType>(fields[i].name, rsgis_float));
                }
                
                delete[] fields;
            }
            catch( Exception &e )
            {
                hasFloatFields = false;
            }
            
            delete fieldCompTypeMem;
                        
            DataSet neighboursDataset = attH5File->openDataSet( ATT_NEIGHBOURS_DATA );
            DataSpace neighboursDataspace = neighboursDataset.getSpace();
            
            int neighboursNDims = neighboursDataspace.getSimpleExtentNdims();
            if(neighboursNDims != 1)
            {
                throw RSGISAttributeTableException("The neighbours datasets needs to have 1 dimension.");
            }
            
            hsize_t *neighboursDims = new hsize_t[neighboursNDims];
            neighboursDataspace.getSimpleExtentDims(neighboursDims);
            
            if(neighboursDims[0] != numFeats)
            {
                throw RSGISAttributeTableException("The number of features in neighbours datasets does not match expected values.");
            }
            
            delete[] neighboursDims;
            
            //cout << "Number of Features = " << numFeats << endl;
            //cout << "Neighbours Line Length = " << neighboursLineLength << endl;
            
            unsigned long numChunks = numFeats / ATT_WRITE_CHUNK_SIZE;
            unsigned long remainingRows = numFeats - (numChunks * ATT_WRITE_CHUNK_SIZE);
            
            //cout << "Number of Chunks: " << numChunks << endl;
            //cout << "Remaining Rows: " << remainingRows << endl;
            
            DataSet boolDataset;
            DataSpace boolDataspace;
            int *boolVals = NULL;
            if(hasBoolFields)
            {
                boolDataset = attH5File->openDataSet( ATT_BOOL_DATA );
                boolDataspace = boolDataset.getSpace();
                
                int boolNDims = boolDataspace.getSimpleExtentNdims();
                
                if(boolNDims != 2)
                {
                    throw RSGISAttributeTableException("The boolean datasets needs to have 2 dimensions.");
                }
                
                hsize_t *boolDims = new hsize_t[boolNDims];
                boolDataspace.getSimpleExtentDims(boolDims);
                
                if(boolDims[0] != numFeats)
                {
                    throw RSGISAttributeTableException("The number of features in boolean datasets does not match expected values.");
                }
                
                if(boolDims[1] != attTableObj->numBoolFields)
                {
                    throw RSGISAttributeTableException("The number of boolean fields does not match expected values.");
                }
                
                boolVals = new int[ATT_WRITE_CHUNK_SIZE*attTableObj->numBoolFields];
            }
            
            DataSet intDataset;
            DataSpace intDataspace;
            long *intVals = NULL;
            if(hasIntFields)
            {
                intDataset = attH5File->openDataSet( ATT_INT_DATA );
                intDataspace = intDataset.getSpace();
                
                int intNDims = intDataspace.getSimpleExtentNdims();
                
                if(intNDims != 2)
                {
                    throw RSGISAttributeTableException("The integer datasets needs to have 2 dimensions.");
                }
                
                hsize_t *intDims = new hsize_t[intNDims];
                intDataspace.getSimpleExtentDims(intDims);
                
                if(intDims[0] != numFeats)
                {
                    throw RSGISAttributeTableException("The number of features in integer datasets does not match expected values.");
                }
                
                if(intDims[1] != attTableObj->numIntFields)
                {
                    throw RSGISAttributeTableException("The number of integer fields does not match expected values.");
                }
                
                intVals = new long[ATT_WRITE_CHUNK_SIZE*attTableObj->numIntFields];
            }
            
            DataSet floatDataset;
            DataSpace floatDataspace;
            double *floatVals = NULL;
            if(hasFloatFields)
            {
                floatDataset = attH5File->openDataSet( ATT_FLOAT_DATA );
                floatDataspace = floatDataset.getSpace();
                
                int floatNDims = floatDataspace.getSimpleExtentNdims();
                
                if(floatNDims != 2)
                {
                    throw RSGISAttributeTableException("The float datasets needs to have 2 dimensions.");
                }
                
                hsize_t *floatDims = new hsize_t[floatNDims];
                floatDataspace.getSimpleExtentDims(floatDims);
                
                if(floatDims[0] != numFeats)
                {
                    throw RSGISAttributeTableException("The number of features in float datasets does not match expected values.");
                }
                
                if(floatDims[1] != attTableObj->numFloatFields)
                {
                    throw RSGISAttributeTableException("The number of float fields does not match expected values.");
                }
                
                 floatVals = new double[ATT_WRITE_CHUNK_SIZE*attTableObj->numFloatFields];
            }
            
            attTableObj->attTable = new vector<RSGISFeature*>();
            attTableObj->attTable->reserve(numFeats);
 
            /* Neighbours */
            hvl_t *neighbourVals = new hvl_t[ATT_WRITE_CHUNK_SIZE];
            DataType intVarLenMemDT = VarLenType(&PredType::NATIVE_UINT64);
            hsize_t neighboursOffset[1];
			neighboursOffset[0] = 0;
			hsize_t neighboursCount[1];
			neighboursCount[0] = ATT_WRITE_CHUNK_SIZE;
			neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
			
			hsize_t neighboursDimsRead[1]; 
			neighboursDimsRead[0] = ATT_WRITE_CHUNK_SIZE;
			DataSpace neighboursMemspace( 1, neighboursDimsRead );
			
			hsize_t neighboursOffset_out[1];
            neighboursOffset_out[0] = 0;
			hsize_t neighboursCount_out[1];
			neighboursCount_out[0] = ATT_WRITE_CHUNK_SIZE;
			neighboursMemspace.selectHyperslab( H5S_SELECT_SET, neighboursCount_out, neighboursOffset_out );
            
            /* Bool fields */
            hsize_t boolFieldsOffset[2];
			boolFieldsOffset[0] = 0;
            boolFieldsOffset[1] = 0;
			hsize_t boolFieldsCount[2];
			boolFieldsCount[0] = ATT_WRITE_CHUNK_SIZE;
            boolFieldsCount[1] = attTableObj->numBoolFields;
            if(hasBoolFields)
            {
                boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
            }
			
			hsize_t boolFieldsDimsRead[2]; 
			boolFieldsDimsRead[0] = ATT_WRITE_CHUNK_SIZE;
            boolFieldsDimsRead[1] = attTableObj->numBoolFields;
			DataSpace boolFieldsMemspace( 2, boolFieldsDimsRead );
			
			hsize_t boolFieldsOffset_out[2];
            boolFieldsOffset_out[0] = 0;
            boolFieldsOffset_out[1] = 0;
			hsize_t boolFieldsCount_out[2];
			boolFieldsCount_out[0] = ATT_WRITE_CHUNK_SIZE;
            boolFieldsCount_out[1] = attTableObj->numBoolFields;
            if(hasBoolFields)
            {
                boolFieldsMemspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount_out, boolFieldsOffset_out );
            }
            
            /* Int fields */
            hsize_t intFieldsOffset[2];
			intFieldsOffset[0] = 0;
            intFieldsOffset[1] = 0;
			hsize_t intFieldsCount[2];
			intFieldsCount[0] = ATT_WRITE_CHUNK_SIZE;
            intFieldsCount[1] = attTableObj->numIntFields;
            if(hasIntFields)
            {
                intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );
            }
			
			hsize_t intFieldsDimsRead[2]; 
			intFieldsDimsRead[0] = ATT_WRITE_CHUNK_SIZE;
            intFieldsDimsRead[1] = attTableObj->numIntFields;
			DataSpace intFieldsMemspace( 2, intFieldsDimsRead );
			
			hsize_t intFieldsOffset_out[2];
            intFieldsOffset_out[0] = 0;
            intFieldsOffset_out[1] = 0;
			hsize_t intFieldsCount_out[2];
			intFieldsCount_out[0] = ATT_WRITE_CHUNK_SIZE;
            intFieldsCount_out[1] = attTableObj->numIntFields;
			if(hasIntFields)
            {
                intFieldsMemspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount_out, intFieldsOffset_out );
            }
            
            /* Float fields */
            hsize_t floatFieldsOffset[2];
			floatFieldsOffset[0] = 0;
            floatFieldsOffset[1] = 0;
			hsize_t floatFieldsCount[2];
			floatFieldsCount[0] = ATT_WRITE_CHUNK_SIZE;
            floatFieldsCount[1] = attTableObj->numFloatFields;
            if(hasFloatFields)
            {
                floatDataspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount, floatFieldsOffset );
            }
			
			hsize_t floatFieldsDimsRead[2]; 
			floatFieldsDimsRead[0] = ATT_WRITE_CHUNK_SIZE;
            floatFieldsDimsRead[1] = attTableObj->numFloatFields;
			DataSpace floatFieldsMemspace( 2, floatFieldsDimsRead );
			
			hsize_t floatFieldsOffset_out[2];
            floatFieldsOffset_out[0] = 0;
            floatFieldsOffset_out[1] = 0;
			hsize_t floatFieldsCount_out[2];
			floatFieldsCount_out[0] = ATT_WRITE_CHUNK_SIZE;
            floatFieldsCount_out[1] = attTableObj->numFloatFields;
            if(hasFloatFields)
            {
                floatFieldsMemspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount_out, floatFieldsOffset_out );
            }
            
            
            
            RSGISFeature *feature = NULL;
            size_t cFid = 0;
            for(unsigned long i = 0; i < numChunks; ++i)
            {
                neighboursOffset[0] = i*ATT_WRITE_CHUNK_SIZE;
                neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
                //cout << "reading neighbours\n";
                neighboursDataset.read(neighbourVals, intVarLenMemDT, neighboursMemspace, neighboursDataspace);
                //cout << "neighbours read\n";
                
                if(hasBoolFields)
                {
                    boolFieldsOffset[0] = i*ATT_WRITE_CHUNK_SIZE;
                    boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
                    boolDataset.read(boolVals, PredType::NATIVE_INT32, boolFieldsMemspace, boolDataspace);
                }
                
                if(hasIntFields)
                {
                    intFieldsOffset[0] = i*ATT_WRITE_CHUNK_SIZE;
                    //cout << "intFieldsOffset[0] = " << intFieldsOffset[0] << endl;
                    intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );
                    intDataset.read(intVals, PredType::NATIVE_INT64, intFieldsMemspace, intDataspace);
                }
                
                if(hasFloatFields)
                {
                    floatFieldsOffset[0] = i*ATT_WRITE_CHUNK_SIZE;
                    //cout << "floatFieldsOffset[0] = " << floatFieldsOffset[0] << endl;
                    floatDataspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount, floatFieldsOffset );
                    floatDataset.read(floatVals, PredType::NATIVE_DOUBLE, floatFieldsMemspace, floatDataspace);
                    //cout << "read floats\n";
                }
                
                for(hsize_t j = 0; j < ATT_WRITE_CHUNK_SIZE; ++j)
                {
                    feature = new RSGISFeature();
                    feature->fid = cFid++;
                    feature->boolFields = new vector<bool>();
                    if(hasBoolFields)
                    {
                        feature->boolFields->reserve(attTableObj->numBoolFields);
                        for(hsize_t n = 0; n < attTableObj->numBoolFields; ++n)
                        {
                            feature->boolFields->push_back(boolVals[(j*attTableObj->numBoolFields)+n]);
                        }
                    }
                    feature->intFields = new vector<long>();
                    if(hasIntFields)
                    {
                        feature->intFields->reserve(attTableObj->numIntFields);
                        for(hsize_t n = 0; n < attTableObj->numIntFields; ++n)
                        {
                            feature->intFields->push_back(intVals[(j*attTableObj->numIntFields)+n]);
                        }
                    }
                    feature->floatFields = new vector<double>();
                    if(hasFloatFields)
                    {
                        feature->floatFields->reserve(attTableObj->numFloatFields);
                        for(hsize_t n = 0; n < attTableObj->numFloatFields; ++n)
                        {
                            feature->floatFields->push_back(floatVals[(j*attTableObj->numFloatFields)+n]);
                            //cout << feature->fid << ":\t" << floatVals[(j*attTableObj->numFloatFields)+n] << endl;
                        }
                    }
                    feature->stringFields = new vector<string>();
                    feature->neighbours = new vector<size_t>();
                    feature->neighbours->reserve(neighbourVals[j].length);

                    for(hsize_t n = 0; n < neighbourVals[j].length; ++n)
                    {
                        feature->neighbours->push_back(((unsigned long long*)neighbourVals[j].p)[n]);
                    }
                    delete[] ((size_t*)neighbourVals[j].p);
                    
                    //cout << cFid << " has " << neighbourVals[j].length << " neighbours\n";
                    
                    attTableObj->attTable->push_back(feature);
                }
            }
            
            /* Neighbours */
			neighboursOffset[0] = numChunks*ATT_WRITE_CHUNK_SIZE;
			neighboursCount[0] = remainingRows;
			neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
			
			neighboursDimsRead[0] = remainingRows;
			neighboursMemspace = DataSpace( 1, neighboursDimsRead );
			
            neighboursOffset_out[0] = 0;
			neighboursCount_out[0] = remainingRows;
			neighboursMemspace.selectHyperslab( H5S_SELECT_SET, neighboursCount_out, neighboursOffset_out );
            
            /* Bool fields */
			boolFieldsOffset[0] = numChunks*ATT_WRITE_CHUNK_SIZE;
            boolFieldsOffset[1] = 0;
			boolFieldsCount[0] = remainingRows;
            boolFieldsCount[1] = attTableObj->numBoolFields;
            if(hasBoolFields)
            {
                boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
            }
			
			boolFieldsDimsRead[0] = remainingRows;
            boolFieldsDimsRead[1] = attTableObj->numBoolFields;
			boolFieldsMemspace = DataSpace( 2, boolFieldsDimsRead );
			
            boolFieldsOffset_out[0] = 0;
            boolFieldsOffset_out[1] = 0;
			boolFieldsCount_out[0] = remainingRows;
            boolFieldsCount_out[1] = attTableObj->numBoolFields;
            if(hasBoolFields)
            {
                boolFieldsMemspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount_out, boolFieldsOffset_out );
            }
            
            /* Int fields */
			intFieldsOffset[0] = numChunks*ATT_WRITE_CHUNK_SIZE;
            intFieldsOffset[1] = 0;
			intFieldsCount[0] = remainingRows;
            intFieldsCount[1] = attTableObj->numIntFields;
            if(hasIntFields)
            {
                intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );
            }
			
			intFieldsDimsRead[0] = remainingRows;
            intFieldsDimsRead[1] = attTableObj->numIntFields;
			intFieldsMemspace = DataSpace( 2, intFieldsDimsRead );
			
            intFieldsOffset_out[0] = 0;
            intFieldsOffset_out[1] = 0;
			intFieldsCount_out[0] = remainingRows;
            intFieldsCount_out[1] = attTableObj->numIntFields;
			if(hasIntFields)
            {
                intFieldsMemspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount_out, intFieldsOffset_out );
            }
            
            /* Float fields */
			floatFieldsOffset[0] = numChunks*ATT_WRITE_CHUNK_SIZE;
            floatFieldsOffset[1] = 0;
			floatFieldsCount[0] = remainingRows;
            floatFieldsCount[1] = attTableObj->numFloatFields;
            if(hasFloatFields)
            {
                floatDataspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount, floatFieldsOffset );
            }
			
			floatFieldsDimsRead[0] = remainingRows;
            floatFieldsDimsRead[1] = attTableObj->numFloatFields;
			floatFieldsMemspace = DataSpace( 2, floatFieldsDimsRead );
			
            floatFieldsOffset_out[0] = 0;
            floatFieldsOffset_out[1] = 0;
			floatFieldsCount_out[0] = remainingRows;
            floatFieldsCount_out[1] = attTableObj->numFloatFields;
            if(hasFloatFields)
            {
                floatFieldsMemspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount_out, floatFieldsOffset_out );
            }
            
            neighboursDataset.read(neighbourVals, intVarLenMemDT, neighboursMemspace, neighboursDataspace);
            
            if(hasBoolFields)
            {
                boolDataset.read(boolVals, PredType::NATIVE_INT32, boolFieldsMemspace, boolDataspace);
            }
            
            if(hasIntFields)
            {
                //cout << "remaining: intFieldsOffset[0] = " << intFieldsOffset[0] << endl;
                intDataset.read(intVals, PredType::NATIVE_INT64, intFieldsMemspace, intDataspace);
            }
            
            if(hasFloatFields)
            {
                //cout << "remaining: floatFieldsOffset[0] = " << floatFieldsOffset[0] << endl;
                floatDataset.read(floatVals, PredType::NATIVE_DOUBLE, floatFieldsMemspace, floatDataspace);
            }
            
            for(hsize_t j = 0; j < remainingRows; ++j)
            {
                feature = new RSGISFeature();
                feature->fid = cFid++;
                feature->boolFields = new vector<bool>();
                if(hasBoolFields)
                {
                    feature->boolFields->reserve(attTableObj->numBoolFields);
                    for(hsize_t n = 0; n < attTableObj->numBoolFields; ++n)
                    {
                        feature->boolFields->push_back(boolVals[(j*attTableObj->numBoolFields)+n]);
                    }
                }
                feature->intFields = new vector<long>();
                if(hasIntFields)
                {
                    feature->intFields->reserve(attTableObj->numIntFields);
                    for(hsize_t n = 0; n < attTableObj->numIntFields; ++n)
                    {
                        feature->intFields->push_back(intVals[(j*attTableObj->numIntFields)+n]);
                    }
                }
                feature->floatFields = new vector<double>();
                if(hasFloatFields)
                {
                    feature->floatFields->reserve(attTableObj->numFloatFields);
                    for(hsize_t n = 0; n < attTableObj->numFloatFields; ++n)
                    {
                        feature->floatFields->push_back(floatVals[(j*attTableObj->numFloatFields)+n]);
                    }
                }
                feature->stringFields = new vector<string>();
                feature->neighbours = new vector<size_t>();
                feature->neighbours->reserve(neighbourVals[j].length);
                
                for(hsize_t n = 0; n < neighbourVals[j].length; ++n)
                {
                    feature->neighbours->push_back(((unsigned long long*)neighbourVals[j].p)[n]);
                }
                delete[] ((size_t*)neighbourVals[j].p);
                
                //cout << cFid << " has " << neighbourVals[j].length << " neighbours\n";
                                
                attTableObj->attTable->push_back(feature);
            }
            
            attH5File->close();
            delete attH5File;
            
            if(hasBoolFields)
            {
                delete[] boolVals;
            }
            if(hasIntFields)
            {
                delete[] intVals;
            }
            if(hasFloatFields)
            {
                delete[] floatVals;
            }
            delete[] neighbourVals;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch( Exception &e )
        {
            /*e.printError();
            cout << e.getCFuncName() << endl;
            cout << e.getCDetailMsg() << endl;
            cout << e.getFuncName() << endl;
            cout << e.getDetailMsg() << endl;*/
            
            throw RSGISAttributeTableException(e.getDetailMsg());
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
                this->freeFeature(*iterFeats);
            }
            
            delete attTable;
        }
    }
    
}}




