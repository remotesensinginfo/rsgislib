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
            for(this->start(); this->end(); ++(*this))
            {
                for(vector<RSGISIfStatement*>::iterator iterStatement = statements->begin(); iterStatement != statements->end(); ++iterStatement)
                {
                    if((*iterStatement)->noExp || (*iterStatement)->exp->evaluate(*(*this)))
                    {
                        if(!(*iterStatement)->ignore)
                        {
                            RSGISAttributeDataType dataType = this->getDataType((*iterStatement)->field);
                            unsigned int idx = this->getFieldIndex((*iterStatement)->field);
                                                    
                            if(dataType == rsgis_int)
                            {
                                (*(*this))->intFields->at(idx) = (*iterStatement)->value;
                            }
                            else if(dataType == rsgis_float)
                            {
                                (*(*this))->floatFields->at(idx) = (double)(*iterStatement)->value;
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
    
    void RSGISAttributeTable::processIfStatements(RSGISIfStatement *statement, RSGISProcessFeature *processTrue, RSGISProcessFeature *processFalse) throw(RSGISAttributeTableException)
    {
        try
        {
            for(this->start(); this->end(); ++(*this))
            {
                this->holdFID((*(*this))->fid);
                if(statement->exp->evaluate(*(*this)))
                {
                    processTrue->processFeature(*(*this), this);
                }
                else if(processFalse != NULL)
                {
                    processFalse->processFeature(*(*this), this);
                }
                this->removeHoldFID((*(*this))->fid);
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
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
            for(this->start(); this->end(); ++(*this))
            {
                unsigned int i = 0;
                for(vector<RSGISMathAttVariable*>::iterator iterVars = variables->begin(); iterVars != variables->end(); ++iterVars)
                {
                    if((*iterVars)->fieldDT == rsgis_int)
                    {
                        inVals[i] = (*(*this))->intFields->at((*iterVars)->fieldIdx);
                    }
                    else if((*iterVars)->fieldDT == rsgis_float)
                    {
                        inVals[i] = (*(*this))->floatFields->at((*iterVars)->fieldIdx);
                    }
                    
                    ++i;
                } 
                
                result = muParser->Eval();
                
                if(outFieldDT == rsgis_int)
                {
                    try
                    {
                        (*(*this))->intFields->at(outFieldIdx)  = lexical_cast<unsigned long>(result);
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
                            (*(*this))->intFields->at(outFieldIdx) = lexical_cast<unsigned long>(floor(result+0.5));
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
                        (*(*this))->floatFields->at(outFieldIdx)  = lexical_cast<double>(result);
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
            
            for(this->start(); this->end(); ++(*this))
            {
                if(fromFieldDT == rsgis_bool)
                {
                    (*(*this))->boolFields->at(toFieldIdx) = (*(*this))->boolFields->at(fromFieldIdx);
                }
                else if(fromFieldDT == rsgis_int)
                {
                    (*(*this))->intFields->at(toFieldIdx) = (*(*this))->intFields->at(fromFieldIdx);
                }
                else if(fromFieldDT == rsgis_float)
                {
                    (*(*this))->floatFields->at(toFieldIdx) = (*(*this))->floatFields->at(fromFieldIdx);
                }
                else if(fromFieldDT == rsgis_string)
                {
                    (*(*this))->stringFields->at(toFieldIdx) = (*(*this))->stringFields->at(fromFieldIdx);
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
            data->reserve(this->getSize());
            
            for(this->start(); this->end(); ++(*this))
            {
                if(dataType == rsgis_int)
                {
                    data->push_back((*(*this))->intFields->at(fieldIdx));
                }
                else if(dataType == rsgis_float)
                {
                    data->push_back((*(*this))->floatFields->at(fieldIdx));
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
        outTxtFile << this->getSize() << endl;
        
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
        for(this->start(); this->end(); ++(*this))
        {
            outTxtFile << (*(*this))->fid;
            for(vector<bool>::iterator iterBools = (*(*this))->boolFields->begin(); iterBools != (*(*this))->boolFields->end(); ++iterBools)
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
            
            for(vector<long>::iterator iterInts = (*(*this))->intFields->begin(); iterInts != (*(*this))->intFields->end(); ++iterInts)
            {
                outTxtFile << "," << *iterInts;
            }
            
            outTxtFile.precision(12);
            for(vector<double>::iterator iterFloats = (*(*this))->floatFields->begin(); iterFloats != (*(*this))->floatFields->end(); ++iterFloats)
            {
                outTxtFile << "," << *iterFloats;
            }
            
            for(vector<string>::iterator iterStrs = (*(*this))->stringFields->begin(); iterStrs != (*(*this))->stringFields->end(); ++iterStrs)
            {
                outTxtFile << "," << *iterStrs;
            }
            
            outTxtFile << endl;
        }
        
        outTxtFile << "# Data - FID,neighbours\n";
        for(this->start(); this->end(); ++(*this))
        {
            outTxtFile << (*(*this))->fid;
            if((*(*this))->neighbours != NULL)
            {
                for(vector<size_t>::iterator iterNeighbours = (*(*this))->neighbours->begin(); iterNeighbours != (*(*this))->neighbours->end(); ++iterNeighbours)
                {
                    outTxtFile << "," << *iterNeighbours;
                }
            }
            outTxtFile << endl;
        }
        
        outTxtFile.flush();
        outTxtFile.close();
    }
    
    void RSGISAttributeTable::exportHDF5(string outFile) throw(RSGISAttributeTableException)
    {
        try
        {
            Exception::dontPrint();
            
            FileAccPropList attAccessPlist = FileAccPropList(FileAccPropList::DEFAULT);
            attAccessPlist.setCache(ATT_WRITE_MDC_NELMTS, ATT_WRITE_RDCC_NELMTS, ATT_WRITE_RDCC_NBYTES, ATT_WRITE_RDCC_W0);
            attAccessPlist.setSieveBufSize(ATT_WRITE_SIEVE_BUF);
            hsize_t metaBlockSize = ATT_WRITE_META_BLOCKSIZE;
            attAccessPlist.setMetaBlockSize(metaBlockSize);
            
            const H5std_string attFilePath( outFile );
            H5File *attH5File = new H5File( attFilePath, H5F_ACC_TRUNC, FileCreatPropList::DEFAULT, attAccessPlist);
            
            attH5File->createGroup( ATT_GROUPNAME_HEADER );
			attH5File->createGroup( ATT_GROUPNAME_DATA );
            attH5File->createGroup( ATT_GROUPNAME_NEIGHBOURS );
            
            hsize_t dimsAttSize[1];
			dimsAttSize[0] = 1;
            size_t numFeatures = this->getSize();
			DataSpace attSizeDataSpace(1, dimsAttSize);
            DataSet sizeDataset = attH5File->createDataSet(ATT_SIZE_HEADER, PredType::STD_U64LE, attSizeDataSpace);
			sizeDataset.write( &numFeatures, PredType::STD_U64LE );
            attSizeDataSpace.close();
            
            RSGISAttributeIdx *boolFields = NULL;
            if(this->numBoolFields > 0)
            {
                boolFields = new RSGISAttributeIdx[this->numBoolFields];
            }
            RSGISAttributeIdx *intFields = NULL;
            if(this->numIntFields > 0)
            {
                intFields = new RSGISAttributeIdx[this->numIntFields];
            }
            RSGISAttributeIdx *floatFields = NULL;
            if(this->numFloatFields > 0)
            {
                floatFields = new RSGISAttributeIdx[this->numFloatFields];
            }
            
            unsigned int boolFieldsIdx = 0;
            unsigned int intFieldsIdx = 0;
            unsigned int floatFieldsIdx = 0;
            for(map<string, unsigned int>::iterator iterFields = fieldIdx->begin(); iterFields != fieldIdx->end(); ++iterFields)
            {
                RSGISAttributeDataType dt = this->getDataType((*iterFields).first);
                if(dt == rsgis_bool)
                {
                    boolFields[boolFieldsIdx].name = const_cast<char*>((*iterFields).first.c_str());
                    boolFields[boolFieldsIdx].idx = (*iterFields).second;
                    ++boolFieldsIdx;
                }
                else if(dt == rsgis_int)
                {
                    intFields[intFieldsIdx].name = const_cast<char*>((*iterFields).first.c_str());
                    intFields[intFieldsIdx].idx = (*iterFields).second;
                    ++intFieldsIdx;
                }
                else if(dt == rsgis_float)
                {
                    floatFields[floatFieldsIdx].name = const_cast<char*>((*iterFields).first.c_str());
                    floatFields[floatFieldsIdx].idx = (*iterFields).second;
                    ++floatFieldsIdx;
                }
                else if(dt == rsgis_string)
                {
                    cout << "String data type is not currently supported in HDF output - fields ignored.\n";
                }
                else
                {
                    throw RSGISAttributeTableException("Data type not recognised.");
                }
            }
            
            CompType *fieldDtDisk = this->createAttibuteIdxCompTypeDisk();
            if(this->numBoolFields > 0)
            {
                hsize_t initDimsBoolFieldsDS[1];
                initDimsBoolFieldsDS[0] = 0;
                hsize_t maxDimsBoolFieldsDS[1];
                maxDimsBoolFieldsDS[0] = H5S_UNLIMITED;
                DataSpace boolFieldsDataSpace = DataSpace(1, initDimsBoolFieldsDS, maxDimsBoolFieldsDS);
                
                hsize_t dimsBoolFieldsChunk[1];
                dimsBoolFieldsChunk[0] = ATT_WRITE_CHUNK_SIZE;
                
                DSetCreatPropList creationBoolFieldsDSPList;
                creationBoolFieldsDSPList.setChunk(1, dimsBoolFieldsChunk);
                creationBoolFieldsDSPList.setShuffle();
                creationBoolFieldsDSPList.setDeflate(ATT_WRITE_DEFLATE);
                DataSet *boolFieldsDataset = new DataSet(attH5File->createDataSet(ATT_BOOL_FIELDS_HEADER, *fieldDtDisk, boolFieldsDataSpace, creationBoolFieldsDSPList));

                hsize_t extendBoolFieldsDatasetTo[1];
                extendBoolFieldsDatasetTo[0] = this->numBoolFields;
                boolFieldsDataset->extend( extendBoolFieldsDatasetTo );
                
                hsize_t boolFieldsOffset[1];
                boolFieldsOffset[0] = 0;
                hsize_t boolFieldsDataDims[1];
                boolFieldsDataDims[0] = this->numBoolFields;
                
                DataSpace boolFieldsWriteDataSpace = boolFieldsDataset->getSpace();
                boolFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolFieldsDataDims, boolFieldsOffset);
                DataSpace newBoolFieldsDataspace = DataSpace(1, boolFieldsDataDims);
                
                boolFieldsDataset->write(boolFields, *fieldDtDisk, newBoolFieldsDataspace, boolFieldsWriteDataSpace);
                
                delete boolFieldsDataset;
                delete[] boolFields;
            }
            
            if(this->numIntFields > 0)
            {
                hsize_t initDimsIntFieldsDS[1];
                initDimsIntFieldsDS[0] = 0;
                hsize_t maxDimsIntFieldsDS[1];
                maxDimsIntFieldsDS[0] = H5S_UNLIMITED;
                DataSpace intFieldsDataSpace = DataSpace(1, initDimsIntFieldsDS, maxDimsIntFieldsDS);
                
                hsize_t dimsIntFieldsChunk[1];
                dimsIntFieldsChunk[0] = ATT_WRITE_CHUNK_SIZE;
                
                DSetCreatPropList creationIntFieldsDSPList;
                creationIntFieldsDSPList.setChunk(1, dimsIntFieldsChunk);
                creationIntFieldsDSPList.setShuffle();
                creationIntFieldsDSPList.setDeflate(ATT_WRITE_DEFLATE);
                DataSet *intFieldsDataset = new DataSet(attH5File->createDataSet(ATT_INT_FIELDS_HEADER, *fieldDtDisk, intFieldsDataSpace, creationIntFieldsDSPList));
                
                hsize_t extendIntFieldsDatasetTo[1];
                extendIntFieldsDatasetTo[0] = this->numIntFields;
                intFieldsDataset->extend( extendIntFieldsDatasetTo );
                
                hsize_t intFieldsOffset[1];
                intFieldsOffset[0] = 0;
                hsize_t intFieldsDataDims[1];
                intFieldsDataDims[0] = this->numIntFields;
                
                DataSpace intFieldsWriteDataSpace = intFieldsDataset->getSpace();
                intFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intFieldsDataDims, intFieldsOffset);
                DataSpace newIntFieldsDataspace = DataSpace(1, intFieldsDataDims);
                
                intFieldsDataset->write(intFields, *fieldDtDisk, newIntFieldsDataspace, intFieldsWriteDataSpace);
                
                delete intFieldsDataset;
                delete[] intFields;
            }
            
            if(this->numFloatFields > 0)
            {
                hsize_t initDimsFloatFieldsDS[1];
                initDimsFloatFieldsDS[0] = 0;
                hsize_t maxDimsFloatFieldsDS[1];
                maxDimsFloatFieldsDS[0] = H5S_UNLIMITED;
                DataSpace floatFieldsDataSpace = DataSpace(1, initDimsFloatFieldsDS, maxDimsFloatFieldsDS);
                
                hsize_t dimsFloatFieldsChunk[1];
                dimsFloatFieldsChunk[0] = ATT_WRITE_CHUNK_SIZE;
                
                DSetCreatPropList creationFloatFieldsDSPList;
                creationFloatFieldsDSPList.setChunk(1, dimsFloatFieldsChunk);
                creationFloatFieldsDSPList.setShuffle();
                creationFloatFieldsDSPList.setDeflate(ATT_WRITE_DEFLATE);
                DataSet *floatFieldsDataset = new DataSet(attH5File->createDataSet(ATT_FLOAT_FIELDS_HEADER, *fieldDtDisk, floatFieldsDataSpace, creationFloatFieldsDSPList));
                
                hsize_t extendFloatFieldsDatasetTo[1];
                extendFloatFieldsDatasetTo[0] = this->numFloatFields;
                floatFieldsDataset->extend( extendFloatFieldsDatasetTo );
                
                hsize_t floatFieldsOffset[1];
                floatFieldsOffset[0] = 0;
                hsize_t floatFieldsDataDims[1];
                floatFieldsDataDims[0] = this->numFloatFields;
                
                DataSpace floatFieldsWriteDataSpace = floatFieldsDataset->getSpace();
                floatFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatFieldsDataDims, floatFieldsOffset);
                DataSpace newFloatFieldsDataspace = DataSpace(1, floatFieldsDataDims);
                
                floatFieldsDataset->write(floatFields, *fieldDtDisk, newFloatFieldsDataspace, floatFieldsWriteDataSpace);
                
                delete floatFieldsDataset;
                delete[] floatFields;
            }
            
            delete fieldDtDisk;

            DataSet *boolDataset = NULL;
            if(this->numBoolFields > 0)
            {
                hsize_t initDimsBoolDS[2];
                initDimsBoolDS[0] = 0;
                initDimsBoolDS[1] = this->numBoolFields;
                hsize_t maxDimsBoolDS[2];
                maxDimsBoolDS[0] = H5S_UNLIMITED;
                maxDimsBoolDS[1] = H5S_UNLIMITED;
                DataSpace boolDataSpace = DataSpace(2, initDimsBoolDS, maxDimsBoolDS);
                
                hsize_t dimsBoolChunk[2];
                dimsBoolChunk[0] = ATT_WRITE_CHUNK_SIZE;
                dimsBoolChunk[1] = 1;
                
                int fillValueInt = 0;
                DSetCreatPropList creationBoolDSPList;
                creationBoolDSPList.setChunk(2, dimsBoolChunk);
                creationBoolDSPList.setShuffle();
                creationBoolDSPList.setDeflate(ATT_WRITE_DEFLATE);
                creationBoolDSPList.setFillValue( PredType::STD_I8LE, &fillValueInt);
                
                boolDataset = new DataSet(attH5File->createDataSet(ATT_BOOL_DATA, PredType::STD_I8LE, boolDataSpace, creationBoolDSPList));
            }
            
            DataSet *intDataset = NULL;
            if(this->numIntFields > 0)
            {
                hsize_t initDimsIntDS[2];
                initDimsIntDS[0] = 0;
                initDimsIntDS[1] = this->numIntFields;
                hsize_t maxDimsIntDS[2];
                maxDimsIntDS[0] = H5S_UNLIMITED;
                maxDimsIntDS[1] = H5S_UNLIMITED;
                DataSpace intDataSpace = DataSpace(2, initDimsIntDS, maxDimsIntDS);
                
                hsize_t dimsIntChunk[2];
                dimsIntChunk[0] = ATT_WRITE_CHUNK_SIZE;
                dimsIntChunk[1] = 1;
                
                long fillValueLongInt = 0;
                DSetCreatPropList creationIntDSPList;
                creationIntDSPList.setChunk(2, dimsIntChunk);
                creationIntDSPList.setShuffle();
                creationIntDSPList.setDeflate(ATT_WRITE_DEFLATE);
                creationIntDSPList.setFillValue( PredType::STD_I64LE, &fillValueLongInt);
                
                intDataset = new DataSet(attH5File->createDataSet(ATT_INT_DATA, PredType::STD_I64LE, intDataSpace, creationIntDSPList));
            }

            DataSet *floatDataset = NULL;
            if(this->numFloatFields > 0)
            {
                hsize_t initDimsFloatDS[2];
                initDimsFloatDS[0] = 0;
                initDimsFloatDS[1] = this->numFloatFields;
                hsize_t maxDimsFloatDS[2];
                maxDimsFloatDS[0] = H5S_UNLIMITED;
                maxDimsFloatDS[1] = H5S_UNLIMITED;
                DataSpace floatDataSpace = DataSpace(2, initDimsFloatDS, maxDimsFloatDS);
                
                hsize_t dimsFloatChunk[2];
                dimsFloatChunk[0] = ATT_WRITE_CHUNK_SIZE;
                dimsFloatChunk[1] = 1;
                
                double fillValueFloat = 0;
                DSetCreatPropList creationFloatDSPList;
                creationFloatDSPList.setChunk(2, dimsFloatChunk);
                creationFloatDSPList.setShuffle();
                creationFloatDSPList.setDeflate(ATT_WRITE_DEFLATE);
                creationFloatDSPList.setFillValue( PredType::IEEE_F64LE, &fillValueFloat);
                
                floatDataset = new DataSet(attH5File->createDataSet(ATT_FLOAT_DATA, PredType::IEEE_F64LE, floatDataSpace, creationFloatDSPList));
            }
            
            // Create Neighbours dataset
            hsize_t initDimsNeighboursDS[1];
            initDimsNeighboursDS[0] = 0;
            hsize_t maxDimsNeighboursDS[1];
            maxDimsNeighboursDS[0] = H5S_UNLIMITED;
            DataSpace neighboursDataspace = DataSpace(1, initDimsNeighboursDS, maxDimsNeighboursDS);
            
            hsize_t dimsNeighboursChunk[1];
            dimsNeighboursChunk[0] = ATT_WRITE_CHUNK_SIZE;
            
            DataType intVarLenDiskDT = VarLenType(&PredType::STD_U64LE);
            hvl_t neighboursDataFillVal[1];
            neighboursDataFillVal[0].p = NULL;
            neighboursDataFillVal[0].length = 0;
            DSetCreatPropList creationNeighboursDSPList;
            creationNeighboursDSPList.setChunk(1, dimsNeighboursChunk);
            creationNeighboursDSPList.setShuffle();
            creationNeighboursDSPList.setDeflate(ATT_WRITE_DEFLATE);
            creationNeighboursDSPList.setFillValue( intVarLenDiskDT, &neighboursDataFillVal);
            
            DataSet neighboursDataset = attH5File->createDataSet(ATT_NEIGHBOURS_DATA, intVarLenDiskDT, neighboursDataspace, creationNeighboursDSPList);
            
            unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
            unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
            
            int *boolVals = NULL;
            if(this->numBoolFields > 0)
            {
                boolVals = new int[ATT_WRITE_CHUNK_SIZE*this->numBoolFields];
            }
                        
            long *intVals = NULL;
            if(this->numIntFields > 0)
            {
                intVals = new long[ATT_WRITE_CHUNK_SIZE*this->numIntFields];
            }
            
            double *floatVals = NULL;
            if(this->numFloatFields > 0)
            {
                floatVals = new double[ATT_WRITE_CHUNK_SIZE*this->numFloatFields];
            }
            
            hvl_t *neighbourVals = new hvl_t[ATT_WRITE_CHUNK_SIZE];
            
            
            hsize_t extendBoolDatasetTo[2];
            extendBoolDatasetTo[0] = 0;
            extendBoolDatasetTo[1] = this->numBoolFields;
            hsize_t boolDataOffset[2];
            boolDataOffset[0] = 0;
            boolDataOffset[1] = 0;
            hsize_t boolDataDims[2];
            boolDataDims[0] = ATT_WRITE_CHUNK_SIZE;
            boolDataDims[1] = this->numBoolFields;
            
            hsize_t extendIntDatasetTo[2];
            extendIntDatasetTo[0] = 0;
            extendIntDatasetTo[1] = this->numIntFields;
            hsize_t intDataOffset[2];
            intDataOffset[0] = 0;
            intDataOffset[1] = 0;
            hsize_t intDataDims[2];
            intDataDims[0] = ATT_WRITE_CHUNK_SIZE;
            intDataDims[1] = this->numIntFields;
            
            hsize_t extendFloatDatasetTo[2];
            extendFloatDatasetTo[0] = 0;
            extendFloatDatasetTo[1] = this->numFloatFields;
            hsize_t floatDataOffset[2];
            floatDataOffset[0] = 0;
            floatDataOffset[1] = 0;
            hsize_t floatDataDims[2];
            floatDataDims[0] = ATT_WRITE_CHUNK_SIZE;
            floatDataDims[1] = this->numFloatFields;
            
            hsize_t extendNeighboursDatasetTo[1];
            extendNeighboursDatasetTo[0] = 0;
            hsize_t neighboursDataOffset[1];
            neighboursDataOffset[0] = 0;
            hsize_t neighboursDataDims[1];
            neighboursDataDims[0] = ATT_WRITE_CHUNK_SIZE;
            DataType intVarLenMemDT = VarLenType(&PredType::NATIVE_UINT64);
            
            size_t currentSize = 0;
            size_t rowIdx = 0;
            RSGISFeature *feat = NULL;
            for(unsigned long i = 0; i < numChunks; ++i)
            {
                for(unsigned int j = 0; j < ATT_WRITE_CHUNK_SIZE; ++j)
                {
                    feat = this->getFeature(rowIdx++);
                    
                    if(this->numBoolFields > 0)
                    {
                        for(unsigned int k = 0; k < feat->boolFields->size(); ++k)
                        {
                            boolVals[(j*this->numBoolFields)+k] = feat->boolFields->at(k);
                        }
                    }
                    
                    if(this->numIntFields > 0)
                    {
                        for(unsigned int k = 0; k < feat->intFields->size(); ++k)
                        {
                            intVals[(j*this->numIntFields)+k] = feat->intFields->at(k);
                        }
                    }
                    
                    if(this->numFloatFields > 0)
                    {
                        for(unsigned int k = 0; k < feat->floatFields->size(); ++k)
                        {
                            floatVals[(j*this->numFloatFields)+k] = feat->floatFields->at(k);
                        }
                    }
                    
                    if(feat->neighbours->size() > 0)
                    {
                        neighbourVals[j].length = feat->neighbours->size();
                        neighbourVals[j].p = new size_t[feat->neighbours->size()];
                        for(unsigned int k = 0; k < feat->neighbours->size(); ++k)
                        {
                            ((size_t*)neighbourVals[j].p)[k] = feat->neighbours->at(k);
                        }
                    }
                    else
                    {
                        neighbourVals[j].length = 0;
                        neighbourVals[j].p = NULL;
                    }
                }
                
                if(this->numBoolFields > 0)
                {
                    extendBoolDatasetTo[0] = currentSize + ATT_WRITE_CHUNK_SIZE;
                    extendBoolDatasetTo[1] = this->numBoolFields;
                    boolDataset->extend( extendBoolDatasetTo );
                    
                    boolDataOffset[0] = currentSize;
                    boolDataOffset[1] = 0;
                    
                    DataSpace boolWriteDataSpace = boolDataset->getSpace();
                    boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                    DataSpace newBoolDataspace = DataSpace(2, boolDataDims);
                    
                    boolDataset->write(boolVals, PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                }

                if(this->numIntFields > 0)
                {
                    extendIntDatasetTo[0] = currentSize + ATT_WRITE_CHUNK_SIZE;
                    extendIntDatasetTo[1] = this->numIntFields;
                    intDataset->extend( extendIntDatasetTo );
                    
                    intDataOffset[0] = currentSize;
                    intDataOffset[1] = 0;
                    
                    DataSpace intWriteDataSpace = intDataset->getSpace();
                    intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                    DataSpace newIntDataspace = DataSpace(1, intDataDims);
                    
                    intDataset->write(intVals, PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                }

                if(this->numFloatFields > 0)
                {
                    extendFloatDatasetTo[0] = currentSize + ATT_WRITE_CHUNK_SIZE;
                    extendFloatDatasetTo[1] = this->numFloatFields;
                    floatDataset->extend( extendFloatDatasetTo );
                    
                    floatDataOffset[0] = currentSize;
                    floatDataOffset[1] = 0;
                    
                    DataSpace floatWriteDataSpace = floatDataset->getSpace();
                    floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                    DataSpace newFloatDataspace = DataSpace(2, floatDataDims);
                    
                    floatDataset->write(floatVals, PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                }
                
                extendNeighboursDatasetTo[0] = currentSize + ATT_WRITE_CHUNK_SIZE;
                neighboursDataset.extend( extendNeighboursDatasetTo );
                neighboursDataOffset[0] = currentSize;
                neighboursDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                
                DataSpace neighboursWriteDataSpace = neighboursDataset.getSpace();
                neighboursWriteDataSpace.selectHyperslab(H5S_SELECT_SET, neighboursDataDims, neighboursDataOffset);
                DataSpace newNeighboursDataspace = DataSpace(1, neighboursDataDims);
                
                neighboursDataset.write(neighbourVals, intVarLenMemDT, newNeighboursDataspace, neighboursWriteDataSpace);
                
                for(size_t i = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    if(neighbourVals[i].length > 0)
                    {
                        neighbourVals[i].length = 0;
                        delete[] ((size_t*)neighbourVals[i].p);
                    }
                }
                
                currentSize += ATT_WRITE_CHUNK_SIZE;
            }
            
            for(unsigned int j = 0; j < remainingRows; ++j)
            {
                feat = this->getFeature(rowIdx++);
                
                if(this->numBoolFields > 0)
                {
                    for(unsigned int k = 0; k < feat->boolFields->size(); ++k)
                    {
                        boolVals[(j*this->numBoolFields)+k] = feat->boolFields->at(k);
                    }
                }
                
                if(this->numIntFields > 0)
                {
                    for(unsigned int k = 0; k < feat->intFields->size(); ++k)
                    {
                        intVals[(j*this->numIntFields)+k] = feat->intFields->at(k);
                    }
                }
                
                if(this->numFloatFields > 0)
                {
                    for(unsigned int k = 0; k < feat->floatFields->size(); ++k)
                    {
                        floatVals[(j*this->numFloatFields)+k] = feat->floatFields->at(k);
                    }
                }
                
                if(feat->neighbours->size() > 0)
                {
                    neighbourVals[j].length = feat->neighbours->size();
                    neighbourVals[j].p = new size_t[feat->neighbours->size()];
                    for(unsigned int k = 0; k < feat->neighbours->size(); ++k)
                    {
                        ((size_t*)neighbourVals[j].p)[k] = feat->neighbours->at(k);
                    }
                }
                else
                {
                    neighbourVals[j].length = 0;
                    neighbourVals[j].p = NULL;
                }
            }
            
            if(this->numBoolFields > 0)
            {
                extendBoolDatasetTo[0] = currentSize + remainingRows;
                extendBoolDatasetTo[1] = this->numBoolFields;
                boolDataset->extend( extendBoolDatasetTo );
                
                boolDataOffset[0] = currentSize;
                boolDataOffset[1] = 0;
                
                boolDataDims[0] = remainingRows;
                boolDataDims[1] = this->numIntFields;
                
                DataSpace boolWriteDataSpace = boolDataset->getSpace();
                boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                DataSpace newBoolDataspace = DataSpace(2, boolDataDims);
                
                boolDataset->write(boolVals, PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
            }
            
            if(this->numIntFields > 0)
            {
                extendIntDatasetTo[0] = currentSize + remainingRows;
                extendIntDatasetTo[1] = this->numIntFields;
                intDataset->extend( extendIntDatasetTo );
                
                intDataOffset[0] = currentSize;
                intDataOffset[1] = 0;
                
                intDataDims[0] = remainingRows;
                intDataDims[1] = this->numIntFields;
                
                DataSpace intWriteDataSpace = intDataset->getSpace();
                intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                DataSpace newIntDataspace = DataSpace(1, intDataDims);
                
                intDataset->write(intVals, PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
            }
            
            if(this->numFloatFields > 0)
            {
                extendFloatDatasetTo[0] = currentSize + remainingRows;
                extendFloatDatasetTo[1] = this->numFloatFields;
                floatDataset->extend( extendFloatDatasetTo );
                
                floatDataOffset[0] = currentSize;
                floatDataOffset[1] = 0;
                
                floatDataDims[0] = remainingRows;
                floatDataDims[1] = this->numIntFields;
                
                DataSpace floatWriteDataSpace = floatDataset->getSpace();
                floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                DataSpace newFloatDataspace = DataSpace(2, floatDataDims);
                
                floatDataset->write(floatVals, PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
            }
            
            
            extendNeighboursDatasetTo[0] = currentSize + remainingRows;
            neighboursDataset.extend( extendNeighboursDatasetTo );
            neighboursDataOffset[0] = currentSize;
            neighboursDataDims[0] = remainingRows;
            
            DataSpace neighboursWriteDataSpace = neighboursDataset.getSpace();
            neighboursWriteDataSpace.selectHyperslab(H5S_SELECT_SET, neighboursDataDims, neighboursDataOffset);
            DataSpace newNeighboursDataspace = DataSpace(1, neighboursDataDims);
            
            neighboursDataset.write(neighbourVals, intVarLenMemDT, newNeighboursDataspace, neighboursWriteDataSpace);
            
            for(size_t i = 0; i < remainingRows; ++i)
            {
                if(neighbourVals[i].length > 0)
                {
                    neighbourVals[i].length = 0;
                    delete[] ((size_t*)neighbourVals[i].p);
                }
            }
            
            
            delete boolDataset;
            delete intDataset;
            delete floatDataset;
            
            attH5File->flush(H5F_SCOPE_GLOBAL);
			attH5File->close();
            delete attH5File;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( FileIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( DataSetIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( DataSpaceIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( DataTypeIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    CompType* RSGISAttributeTable::createAttibuteIdxCompTypeDisk() throw(RSGISAttributeTableException)
    {
        try
        {
            StrType strTypeOut(0, H5T_VARIABLE);
            
            CompType *attIdxDataType = new CompType( sizeof(RSGISAttributeIdx) );
            attIdxDataType->insertMember(ATT_NAME_FIELD, HOFFSET(RSGISAttributeIdx, name), strTypeOut);
            attIdxDataType->insertMember(ATT_INDEX_FIELD, HOFFSET(RSGISAttributeIdx, idx), PredType::STD_U32LE);
            return attIdxDataType;
        }
        catch( FileIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( DataSetIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( DataSpaceIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( DataTypeIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    CompType* RSGISAttributeTable::createAttibuteIdxCompTypeMem() throw(RSGISAttributeTableException)
    {
        try
        {
            StrType strTypeIn(0, H5T_VARIABLE);
            
            CompType *attIdxDataType = new CompType( sizeof(RSGISAttributeIdx) );
            attIdxDataType->insertMember(ATT_NAME_FIELD, HOFFSET(RSGISAttributeIdx, name), strTypeIn);
            attIdxDataType->insertMember(ATT_INDEX_FIELD, HOFFSET(RSGISAttributeIdx, idx), PredType::NATIVE_UINT);
            return attIdxDataType;
        }
        catch( FileIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( DataSetIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( DataSpaceIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( DataTypeIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
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
            
            size_t rowCounter = 0;
            for(this->start(); this->end(); ++(*this))
            {
                rasterAtt->SetValue(rowCounter, fidRATIdx, ((int)(*(*this))->fid+1));
                
                for(vector<pair<unsigned int, unsigned int> >::iterator iterAtts = intFieldRATIdxs.begin(); iterAtts != intFieldRATIdxs.end(); ++iterAtts)
                {
                    rasterAtt->SetValue(rowCounter, (*iterAtts).second, ((int)(*(*this))->intFields->at((*iterAtts).first)));
                }
                
                for(vector<pair<unsigned int, unsigned int> >::iterator iterAtts = floatFieldRATIdxs.begin(); iterAtts != floatFieldRATIdxs.end(); ++iterAtts)
                {
                    rasterAtt->SetValue(rowCounter, (*iterAtts).second, ((float)(*(*this))->floatFields->at((*iterAtts).first)));
                }
                
                for(vector<pair<unsigned int, unsigned int> >::iterator iterAtts = stringFieldRATIdxs.begin(); iterAtts != stringFieldRATIdxs.end(); ++iterAtts)
                {
                    rasterAtt->SetValue(rowCounter, (*iterAtts).second, (*(*this))->stringFields->at((*iterAtts).first).c_str());
                }
                
                for(vector<pair<unsigned int, unsigned int> >::iterator iterAtts = boolFieldRATIdxs.begin(); iterAtts != boolFieldRATIdxs.end(); ++iterAtts)
                {
                    rasterAtt->SetValue(rowCounter, (*iterAtts).second, ((int)(*(*this))->boolFields->at((*iterAtts).first)));
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
            for(this->start(); this->end(); ++(*this))
            {
                vals->push_back((*(*this))->floatFields->at(idx));
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
            for(this->start(); this->end(); ++(*this))
            {
                vals->push_back((*(*this))->intFields->at(idx));
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
            for(this->start(); this->end(); ++(*this))
            {
                vals->push_back((*(*this))->boolFields->at(idx));
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
            for(this->start(); this->end(); ++(*this))
            {
                vals->push_back((*(*this))->stringFields->at(idx));
            }
        }
        else
        {
            throw RSGISAttributeTableException("Field must be of type string.");
        }
        
        return vals;
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
    
    
    void RSGISAttributeTable::printSummaryHDFAtt(string inFile)throw(RSGISAttributeTableException)
    {
        try
        {
            Exception::dontPrint();
            
            FileAccPropList attAccessPlist = FileAccPropList(FileAccPropList::DEFAULT);
            attAccessPlist.setCache(ATT_READ_MDC_NELMTS, ATT_READ_RDCC_NELMTS, ATT_READ_RDCC_NBYTES, ATT_READ_RDCC_W0);
            attAccessPlist.setSieveBufSize(ATT_READ_SIEVE_BUF);
            hsize_t metaBlockSize = ATT_READ_META_BLOCKSIZE;
            attAccessPlist.setMetaBlockSize(metaBlockSize);
            
            H5File *attH5File = new H5File( inFile, H5F_ACC_RDONLY, FileCreatPropList::DEFAULT, attAccessPlist);
            
            bool hasBoolFields = true;
            bool hasIntFields = true;
            bool hasFloatFields = true;
            
            hsize_t dimsAttSize[1];
			dimsAttSize[0] = 1;
            size_t numFeats = 0;
			DataSpace attSizeDataSpace(1, dimsAttSize);
            DataSet sizeDataset = attH5File->openDataSet( ATT_SIZE_HEADER );
            sizeDataset.read(&numFeats, PredType::NATIVE_ULLONG, attSizeDataSpace);
            attSizeDataSpace.close();
            
            cout << "Table has " << numFeats << " features\n";
            
            CompType *fieldCompTypeMem = RSGISAttributeTable::createAttibuteIdxCompTypeMem();
            try
            {
                DataSet boolFieldsDataset = attH5File->openDataSet( ATT_BOOL_FIELDS_HEADER );
                DataSpace boolFieldsDataspace = boolFieldsDataset.getSpace();
                
                unsigned int numBoolFields = boolFieldsDataspace.getSelectNpoints();
                
                cout << "There are " << numBoolFields << " boolean fields." << endl;
                
                RSGISAttributeIdx *fields = new RSGISAttributeIdx[numBoolFields];
                
                hsize_t boolFieldsDims[1]; 
                boolFieldsDims[0] = numBoolFields;
                DataSpace boolFieldsMemspace(1, boolFieldsDims);
                
                boolFieldsDataset.read(fields, *fieldCompTypeMem, boolFieldsMemspace, boolFieldsDataspace);
                
                for(unsigned int i = 0; i < numBoolFields; ++i)
                {
                    cout << "Boolean Field: " << fields[i].name << " has index " << fields[i].idx << endl;
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
                
                unsigned int numIntFields = intFieldsDataspace.getSelectNpoints();
                
                cout << "There are " << numIntFields << " integer fields." << endl;
                
                RSGISAttributeIdx *fields = new RSGISAttributeIdx[numIntFields];
                
                hsize_t intFieldsDims[1]; 
                intFieldsDims[0] = numIntFields;
                DataSpace intFieldsMemspace(1, intFieldsDims);
                
                intFieldsDataset.read(fields, *fieldCompTypeMem, intFieldsMemspace, intFieldsDataspace);
                
                for(unsigned int i = 0; i < numIntFields; ++i)
                {
                    cout << "Integer Field: " << fields[i].name << ": " << fields[i].idx << endl;
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
                
                unsigned int numFloatFields = floatFieldsDataspace.getSelectNpoints();
                
                cout << "There are " << numFloatFields << " float fields." << endl;
                
                RSGISAttributeIdx *fields = new RSGISAttributeIdx[numFloatFields];
                
                hsize_t floatFieldsDims[1]; 
                floatFieldsDims[0] = numFloatFields;
                DataSpace floatFieldsMemspace(1, floatFieldsDims);
                
                floatFieldsDataset.read(fields, *fieldCompTypeMem, floatFieldsMemspace, floatFieldsDataspace);
                
                for(unsigned int i = 0; i < numFloatFields; ++i)
                {
                    cout << "Float Field: " << fields[i].name << ": " << fields[i].idx << endl;
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
            
            unsigned long numChunks = numFeats / ATT_WRITE_CHUNK_SIZE;
            unsigned long remainingRows = numFeats - (numChunks * ATT_WRITE_CHUNK_SIZE);
            
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
            
            
            unsigned int maxNumNeighbours = 0; 
            unsigned int minNumNeighbours = 0;
            bool first = 0;
                        
            for(unsigned long i = 0; i < numChunks; ++i)
            {
                neighboursOffset[0] = i*ATT_WRITE_CHUNK_SIZE;
                neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
                neighboursDataset.read(neighbourVals, intVarLenMemDT, neighboursMemspace, neighboursDataspace);
                
                
                for(hsize_t j = 0; j < ATT_WRITE_CHUNK_SIZE; ++j)
                {
                    if(first)
                    {
                        maxNumNeighbours = neighbourVals[j].length;
                        minNumNeighbours = neighbourVals[j].length;
                        first = false;
                    }
                    else if(neighbourVals[j].length > maxNumNeighbours)
                    {
                        maxNumNeighbours = neighbourVals[j].length;
                    }
                    else if(neighbourVals[j].length < minNumNeighbours)
                    {
                        minNumNeighbours = neighbourVals[j].length;
                    }
                    
                    delete[] ((size_t*)neighbourVals[j].p);
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
                        
            neighboursDataset.read(neighbourVals, intVarLenMemDT, neighboursMemspace, neighboursDataspace);
            
            for(hsize_t j = 0; j < remainingRows; ++j)
            {
                if(first)
                {
                    maxNumNeighbours = neighbourVals[j].length;
                    minNumNeighbours = neighbourVals[j].length;
                    first = false;
                }
                else if(neighbourVals[j].length > maxNumNeighbours)
                {
                    maxNumNeighbours = neighbourVals[j].length;
                }
                else if(neighbourVals[j].length < minNumNeighbours)
                {
                    minNumNeighbours = neighbourVals[j].length;
                }
                
                delete[] ((size_t*)neighbourVals[j].p);
            }
            
            attH5File->close();
            delete attH5File;

            delete[] neighbourVals;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch( FileIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( DataSetIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( DataSpaceIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( DataTypeIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    void RSGISAttributeTable::freeFeature(RSGISFeature *feat)
    {
        delete feat->boolFields;
        delete feat->intFields;
        delete feat->floatFields;
        delete feat->stringFields;
        delete feat->neighbours;
        delete feat;
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
    }
    
}}




