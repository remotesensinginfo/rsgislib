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
        
    RSGISAttributeDataType RSGISAttributeTable::getDataType(std::string name) throw(RSGISAttributeTableException)
    {
        std::map<std::string, RSGISAttributeDataType>::iterator iter;
        iter = fieldDataType->find(name);
        
        if(iter == fieldDataType->end())
        {
            std::string message = std::string("Field \'") + name + std::string("\' could not be found.");
            throw RSGISAttributeTableException(message);
        }
        
        return (*iter).second;
    }
    
    unsigned int RSGISAttributeTable::getFieldIndex(std::string name) throw(RSGISAttributeTableException)
    {
        std::map<std::string, unsigned int>::iterator iter;
        iter = fieldIdx->find(name);
        
        
        if(iter == fieldIdx->end())
        {
            std::string message = std::string("Field \'") + name + std::string("\' could not be found.");
            throw RSGISAttributeTableException(message);
        }
        
        return (*iter).second;
    }
    
    std::vector<std::string>* RSGISAttributeTable::getAttributeNames()
    {
        std::vector<std::string> *names = new std::vector<std::string>();
        for(std::vector<std::pair<std::string, RSGISAttributeDataType> >::iterator iterFields = fields->begin(); iterFields != fields->end(); ++iterFields)
        {
            names->push_back((*iterFields).first);
        }
        
        return names;
    }
    
    bool RSGISAttributeTable::hasAttribute(std::string name)
    {
        std::map<std::string, unsigned int>::iterator iter;
        iter = fieldIdx->find(name);
        
        
        if(iter == fieldIdx->end())
        {
            return false;
        }
        
        return true;
    }
    
    void RSGISAttributeTable::applyIfStatements(std::vector<RSGISIfStatement*> *statements) throw(RSGISAttributeTableException)
    {
        try
        {
            for(this->start(); this->end(); ++(*this))
            {
                for(std::vector<RSGISIfStatement*>::iterator iterStatement = statements->begin(); iterStatement != statements->end(); ++iterStatement)
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
    
    bool RSGISAttributeTable::applyIfStatementsToFeature(RSGISFeature *feat, std::vector<RSGISIfStatement*> *statements) throw(RSGISAttributeTableException)
    {
        bool changeOccured = false;
        try
        {
            for(std::vector<RSGISIfStatement*>::iterator iterStatement = statements->begin(); iterStatement != statements->end(); ++iterStatement)
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
    
    void RSGISAttributeTable::processIfStatementsInBlocks(RSGISIfStatement *statement, RSGISProcessFeature *processTrue, RSGISProcessFeature *processFalse) throw(RSGISAttributeTableException)
    {
        try
        {
            size_t totalNumBlocks = (this->getSize()/ATT_WRITE_CHUNK_SIZE)+1;
            size_t numBlocksInSample = this->getNumOfBlocks()*0.6;
            size_t numSamples = (totalNumBlocks/numBlocksInSample)+1;
            size_t startBlock = 0;
            size_t endBlock = numBlocksInSample;
            
            size_t numFeatInSample = ATT_WRITE_CHUNK_SIZE * numBlocksInSample;
            size_t startFID = 0;
            size_t endFID = numFeatInSample;
            
            size_t maxFID = 0;
            size_t minFID = 0;

            long diffFID = 0;
            size_t numBlocks = 0;
            RSGISFeature *feat = NULL;
            std::cout << "Processing " << numSamples << " samples: " << std::endl;
            for(size_t n = 0; n < numSamples; ++n)
            {
                std::cout << "Sample " << n+1 << " of " << numSamples << " - " << std::flush;
                
                if(n == numSamples-1)
                {
                    endFID = this->getSize();
                }
                
                this->loadBlocks(startBlock, endBlock);
                this->findFIDRangeInNeighbours(startFID, endFID, &minFID, &maxFID);
                
                diffFID = startFID - minFID;
                if(diffFID > 0)
                {
                    numBlocks = (diffFID/ATT_WRITE_CHUNK_SIZE)+1;
                    this->loadBlocks(startFID-numBlocks, startFID);
                }
                
                diffFID = maxFID - endFID;
                if(diffFID > 0)
                {
                    numBlocks = (diffFID/ATT_WRITE_CHUNK_SIZE)+1;
                    this->loadBlocks(endBlock, endBlock+numBlocks);
                }
                
                for(size_t j = startFID; j < endFID; ++j)
                {
                    //std::cout << "processing " << j << std::flush;
                    feat = this->getFeature(j);
                    //std::cout << " with fid " << feat->fid << std::endl;
                    this->holdFID(feat->fid);
                    if(statement->exp->evaluate(feat))
                    {
                        processTrue->processFeature(feat, this);
                    }
                    else if(processFalse != NULL)
                    {
                        processFalse->processFeature(feat, this);
                    }
                    this->removeHoldFID(feat->fid);
                }
                
                this->flushAllFeatures(true);
                startBlock += numBlocksInSample;
                endBlock += numBlocksInSample;
                startFID += numFeatInSample;
                endFID += numFeatInSample;
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    
    void RSGISAttributeTable::applyIfStatementBoolOut(RSGISIfStatement *statement) throw(RSGISAttributeTableException)
    {
        try
        {
            for(this->start(); this->end(); ++(*this))
            {
                if(statement->noExp || statement->exp->evaluate(*(*this)))
                {
                    if(statement->dataType == rsgis_bool)
                    {
                        (*(*this))->boolFields->at(statement->fldIdx) = true;
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Only boolean output types are supported.");
                    }
                }
                else
                {
                    if(statement->dataType == rsgis_bool)
                    {
                        (*(*this))->boolFields->at(statement->fldIdx) = false;
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Only boolean output types are supported.");
                    }
                }
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    void RSGISAttributeTable::populateIfStatementsWithIdxs(std::vector<RSGISIfStatement*> *statements) throw(RSGISAttributeTableException)
    {
        try
        {
            for(std::vector<RSGISIfStatement*>::iterator iterStatement = statements->begin(); iterStatement != statements->end(); ++iterStatement)
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
    
    void RSGISAttributeTable::createIfStatementsFields(std::vector<RSGISIfStatement*> *statements, RSGISAttributeDataType dataType) throw(RSGISAttributeTableException)
    {
        try
        {
            for(std::vector<RSGISIfStatement*>::iterator iterStatement = statements->begin(); iterStatement != statements->end(); ++iterStatement)
            {
                if((*iterStatement)->field != "")
                {
                    if(!this->hasAttribute((*iterStatement)->field))
                    {
                        if(dataType == rsgis_int)
                        {
                            this->addAttIntField((*iterStatement)->field, 0);
                        }
                        else if(dataType == rsgis_float)
                        {
                            this->addAttFloatField((*iterStatement)->field, 0);
                        }
                        else if(dataType == rsgis_bool)
                        {
                            this->addAttBoolField((*iterStatement)->field, false);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("Field data type is not recognised.");
                        }
                    }
                    (*iterStatement)->dataType = this->getDataType((*iterStatement)->field);
                    (*iterStatement)->fldIdx = this->getFieldIndex((*iterStatement)->field);
                }
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    void RSGISAttributeTable::calculateFieldsMUParser(std::string expression, std::string outField, RSGISAttributeDataType outFieldDT, std::vector<RSGISMathAttVariable*> *variables) throw(RSGISAttributeTableException)
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
            
            mu::Parser *muParser = new mu::Parser();
			mu::value_type *inVals = new mu::value_type[variables->size()];
            unsigned int i = 0;
            for(std::vector<RSGISMathAttVariable*>::iterator iterVars = variables->begin(); iterVars != variables->end(); ++iterVars)
            {
                if(!this->hasAttribute((*iterVars)->field))
                {
                    std::string message = std::string("Field \'") + (*iterVars)->field + std::string("\' is not present within the attribute table.");
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
            mu::value_type result = 0;
            for(this->start(); this->end(); ++(*this))
            {
                unsigned int i = 0;
                for(std::vector<RSGISMathAttVariable*>::iterator iterVars = variables->begin(); iterVars != variables->end(); ++iterVars)
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
                        (*(*this))->intFields->at(outFieldIdx)  = boost::lexical_cast<unsigned long>(result);
                    }
                    catch(boost::bad_lexical_cast &e)
                    {
                        if(intTrunkWarning)
                        {
                            std::cerr << "Warning you are losing precision in the output using an integer.\n";
                            intTrunkWarning = false;
                        }
                        try
                        {
                            (*(*this))->intFields->at(outFieldIdx) = boost::lexical_cast<unsigned long>(floor(result+0.5));
                        }
                        catch(boost::bad_lexical_cast &e)
                        {
                            throw RSGISAttributeTableException(e.what());
                        }
                    }
                }
                else if(outFieldDT == rsgis_float)
                {
                    try
                    {
                        (*(*this))->floatFields->at(outFieldIdx)  = boost::lexical_cast<double>(result);
                    }
                    catch(boost::bad_lexical_cast &e)
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
    
    void RSGISAttributeTable::copyValues(std::string fromField, std::string toField) throw(RSGISAttributeTableException)
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
    
    
    
    std::vector<double>* RSGISAttributeTable::getFieldValues(std::string field) throw(RSGISAttributeTableException)
    {
        std::vector<double> *data = NULL;
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
            
            data = new std::vector<double>();
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
    
    
    void RSGISAttributeTable::exportASCII(std::string outFile) throw(RSGISAttributeTableException)
    {
        std::ofstream outTxtFile;
		outTxtFile.open(outFile.c_str(), std::ios::out | std::ios::trunc);
        
        outTxtFile << "# Number of features in table\n";
        outTxtFile << this->getSize() << std::endl;
        
        outTxtFile << "# Number of Fields\n";
        outTxtFile << "nBool: " << this->numBoolFields << std::endl;
        outTxtFile << "nInt: " << this->numIntFields << std::endl;
        outTxtFile << "nFloat: " << this->numFloatFields << std::endl;
        outTxtFile << "nString: " << this->numStrFields << std::endl;
        
        outTxtFile << "# Field Name / Type Pairs\n";
        std::vector<std::pair<std::string, RSGISAttributeDataType> >::iterator iterFields;
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
            for(std::vector<bool>::iterator iterBools = (*(*this))->boolFields->begin(); iterBools != (*(*this))->boolFields->end(); ++iterBools)
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
            
            for(std::vector<long>::iterator iterInts = (*(*this))->intFields->begin(); iterInts != (*(*this))->intFields->end(); ++iterInts)
            {
                outTxtFile << "," << *iterInts;
            }
            
            outTxtFile.precision(12);
            for(std::vector<double>::iterator iterFloats = (*(*this))->floatFields->begin(); iterFloats != (*(*this))->floatFields->end(); ++iterFloats)
            {
                outTxtFile << "," << *iterFloats;
            }
            
            for(std::vector<std::string>::iterator iterStrs = (*(*this))->stringFields->begin(); iterStrs != (*(*this))->stringFields->end(); ++iterStrs)
            {
                outTxtFile << "," << *iterStrs;
            }
            
            outTxtFile << std::endl;
        }
        
        outTxtFile << "# Data - FID,neighbours\n";
        for(this->start(); this->end(); ++(*this))
        {
            outTxtFile << (*(*this))->fid;
            if((*(*this))->neighbours != NULL)
            {
                for(std::vector<boost::uint_fast32_t>::iterator iterNeighbours = (*(*this))->neighbours->begin(); iterNeighbours != (*(*this))->neighbours->end(); ++iterNeighbours)
                {
                    outTxtFile << "," << *iterNeighbours;
                }
            }
            outTxtFile << std::endl;
        }
        
        outTxtFile.flush();
        outTxtFile.close();
    }
    
    void RSGISAttributeTable::exportHDF5(std::string outFile) throw(RSGISAttributeTableException)
    {
        try
        {
            H5::Exception::dontPrint();
            
            H5::FileAccPropList attAccessPlist = H5::FileAccPropList(H5::FileAccPropList::DEFAULT);
            attAccessPlist.setCache(ATT_WRITE_MDC_NELMTS, ATT_WRITE_RDCC_NELMTS, ATT_WRITE_RDCC_NBYTES, ATT_WRITE_RDCC_W0);
            attAccessPlist.setSieveBufSize(ATT_WRITE_SIEVE_BUF);
            hsize_t metaBlockSize = ATT_WRITE_META_BLOCKSIZE;
            attAccessPlist.setMetaBlockSize(metaBlockSize);
            
            const H5std_string attFilePath( outFile );
            H5::H5File *attH5File = new H5::H5File( attFilePath, H5F_ACC_TRUNC, H5::FileCreatPropList::DEFAULT, attAccessPlist);
            
            attH5File->createGroup( ATT_GROUPNAME_HEADER );
			attH5File->createGroup( ATT_GROUPNAME_DATA );
            attH5File->createGroup( ATT_GROUPNAME_NEIGHBOURS );
            
            hsize_t dimsAttSize[1];
			dimsAttSize[0] = 1;
            size_t numFeatures = this->getSize();
			H5::DataSpace attSizeDataSpace(1, dimsAttSize);
            H5::DataSet sizeDataset = attH5File->createDataSet(ATT_SIZE_HEADER, H5::PredType::STD_U64LE, attSizeDataSpace);
			sizeDataset.write( &numFeatures, H5::PredType::STD_U64LE );
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
            for(std::map<std::string, unsigned int>::iterator iterFields = fieldIdx->begin(); iterFields != fieldIdx->end(); ++iterFields)
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
                    std::cout << "String data type is not currently supported in HDF output - fields ignored.\n";
                }
                else
                {
                    throw RSGISAttributeTableException("Data type not recognised.");
                }
            }
            
            H5::CompType *fieldDtDisk = this->createAttibuteIdxCompTypeDisk();
            if(this->numBoolFields > 0)
            {
                hsize_t initDimsBoolFieldsDS[1];
                initDimsBoolFieldsDS[0] = 0;
                hsize_t maxDimsBoolFieldsDS[1];
                maxDimsBoolFieldsDS[0] = H5S_UNLIMITED;
                H5::DataSpace boolFieldsDataSpace = H5::DataSpace(1, initDimsBoolFieldsDS, maxDimsBoolFieldsDS);
                
                hsize_t dimsBoolFieldsChunk[1];
                dimsBoolFieldsChunk[0] = ATT_WRITE_CHUNK_SIZE;
                
                H5::DSetCreatPropList creationBoolFieldsDSPList;
                creationBoolFieldsDSPList.setChunk(1, dimsBoolFieldsChunk);
                creationBoolFieldsDSPList.setShuffle();
                creationBoolFieldsDSPList.setDeflate(ATT_WRITE_DEFLATE);
                H5::DataSet *boolFieldsDataset = new H5::DataSet(attH5File->createDataSet(ATT_BOOL_FIELDS_HEADER, *fieldDtDisk, boolFieldsDataSpace, creationBoolFieldsDSPList));

                hsize_t extendBoolFieldsDatasetTo[1];
                extendBoolFieldsDatasetTo[0] = this->numBoolFields;
                boolFieldsDataset->extend( extendBoolFieldsDatasetTo );
                
                hsize_t boolFieldsOffset[1];
                boolFieldsOffset[0] = 0;
                hsize_t boolFieldsDataDims[1];
                boolFieldsDataDims[0] = this->numBoolFields;
                
                H5::DataSpace boolFieldsWriteDataSpace = boolFieldsDataset->getSpace();
                boolFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolFieldsDataDims, boolFieldsOffset);
                H5::DataSpace newBoolFieldsDataspace = H5::DataSpace(1, boolFieldsDataDims);
                
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
                H5::DataSpace intFieldsDataSpace = H5::DataSpace(1, initDimsIntFieldsDS, maxDimsIntFieldsDS);
                
                hsize_t dimsIntFieldsChunk[1];
                dimsIntFieldsChunk[0] = ATT_WRITE_CHUNK_SIZE;
                
                H5::DSetCreatPropList creationIntFieldsDSPList;
                creationIntFieldsDSPList.setChunk(1, dimsIntFieldsChunk);
                creationIntFieldsDSPList.setShuffle();
                creationIntFieldsDSPList.setDeflate(ATT_WRITE_DEFLATE);
                H5::DataSet *intFieldsDataset = new H5::DataSet(attH5File->createDataSet(ATT_INT_FIELDS_HEADER, *fieldDtDisk, intFieldsDataSpace, creationIntFieldsDSPList));
                
                hsize_t extendIntFieldsDatasetTo[1];
                extendIntFieldsDatasetTo[0] = this->numIntFields;
                intFieldsDataset->extend( extendIntFieldsDatasetTo );
                
                hsize_t intFieldsOffset[1];
                intFieldsOffset[0] = 0;
                hsize_t intFieldsDataDims[1];
                intFieldsDataDims[0] = this->numIntFields;
                
                H5::DataSpace intFieldsWriteDataSpace = intFieldsDataset->getSpace();
                intFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intFieldsDataDims, intFieldsOffset);
                H5::DataSpace newIntFieldsDataspace = H5::DataSpace(1, intFieldsDataDims);
                
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
                H5::DataSpace floatFieldsDataSpace = H5::DataSpace(1, initDimsFloatFieldsDS, maxDimsFloatFieldsDS);
                
                hsize_t dimsFloatFieldsChunk[1];
                dimsFloatFieldsChunk[0] = ATT_WRITE_CHUNK_SIZE;
                
                H5::DSetCreatPropList creationFloatFieldsDSPList;
                creationFloatFieldsDSPList.setChunk(1, dimsFloatFieldsChunk);
                creationFloatFieldsDSPList.setShuffle();
                creationFloatFieldsDSPList.setDeflate(ATT_WRITE_DEFLATE);
                H5::DataSet *floatFieldsDataset = new H5::DataSet(attH5File->createDataSet(ATT_FLOAT_FIELDS_HEADER, *fieldDtDisk, floatFieldsDataSpace, creationFloatFieldsDSPList));
                
                hsize_t extendFloatFieldsDatasetTo[1];
                extendFloatFieldsDatasetTo[0] = this->numFloatFields;
                floatFieldsDataset->extend( extendFloatFieldsDatasetTo );
                
                hsize_t floatFieldsOffset[1];
                floatFieldsOffset[0] = 0;
                hsize_t floatFieldsDataDims[1];
                floatFieldsDataDims[0] = this->numFloatFields;
                
                H5::DataSpace floatFieldsWriteDataSpace = floatFieldsDataset->getSpace();
                floatFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatFieldsDataDims, floatFieldsOffset);
                H5::DataSpace newFloatFieldsDataspace = H5::DataSpace(1, floatFieldsDataDims);
                
                floatFieldsDataset->write(floatFields, *fieldDtDisk, newFloatFieldsDataspace, floatFieldsWriteDataSpace);
                
                delete floatFieldsDataset;
                delete[] floatFields;
            }
            
            delete fieldDtDisk;
            
            H5::DataSet *boolDataset = NULL;
            if(this->numBoolFields > 0)
            {
                hsize_t initDimsBoolDS[2];
                initDimsBoolDS[0] = 0;
                initDimsBoolDS[1] = this->numBoolFields;
                hsize_t maxDimsBoolDS[2];
                maxDimsBoolDS[0] = H5S_UNLIMITED;
                maxDimsBoolDS[1] = H5S_UNLIMITED;
                H5::DataSpace boolDataSpace = H5::DataSpace(2, initDimsBoolDS, maxDimsBoolDS);
                
                hsize_t dimsBoolChunk[2];
                dimsBoolChunk[0] = ATT_WRITE_CHUNK_SIZE;
                dimsBoolChunk[1] = 1;
                
                int fillValueInt = 0;
                H5::DSetCreatPropList creationBoolDSPList;
                creationBoolDSPList.setChunk(2, dimsBoolChunk);
                creationBoolDSPList.setShuffle();
                creationBoolDSPList.setDeflate(ATT_WRITE_DEFLATE);
                creationBoolDSPList.setFillValue( H5::PredType::STD_I8LE, &fillValueInt);
                
                boolDataset = new H5::DataSet(attH5File->createDataSet(ATT_BOOL_DATA, H5::PredType::STD_I8LE, boolDataSpace, creationBoolDSPList));
            }
            
            H5::DataSet *intDataset = NULL;
            if(this->numIntFields > 0)
            {
                hsize_t initDimsIntDS[2];
                initDimsIntDS[0] = 0;
                initDimsIntDS[1] = this->numIntFields;
                hsize_t maxDimsIntDS[2];
                maxDimsIntDS[0] = H5S_UNLIMITED;
                maxDimsIntDS[1] = H5S_UNLIMITED;
                H5::DataSpace intDataSpace = H5::DataSpace(2, initDimsIntDS, maxDimsIntDS);
                
                hsize_t dimsIntChunk[2];
                dimsIntChunk[0] = ATT_WRITE_CHUNK_SIZE;
                dimsIntChunk[1] = 1;
                
                long fillValueLongInt = 0;
                H5::DSetCreatPropList creationIntDSPList;
                creationIntDSPList.setChunk(2, dimsIntChunk);
                creationIntDSPList.setShuffle();
                creationIntDSPList.setDeflate(ATT_WRITE_DEFLATE);
                creationIntDSPList.setFillValue( H5::PredType::STD_I64LE, &fillValueLongInt);
                
                intDataset = new H5::DataSet(attH5File->createDataSet(ATT_INT_DATA, H5::PredType::STD_I64LE, intDataSpace, creationIntDSPList));
            }

            H5::DataSet *floatDataset = NULL;
            if(this->numFloatFields > 0)
            {
                hsize_t initDimsFloatDS[2];
                initDimsFloatDS[0] = 0;
                initDimsFloatDS[1] = this->numFloatFields;
                hsize_t maxDimsFloatDS[2];
                maxDimsFloatDS[0] = H5S_UNLIMITED;
                maxDimsFloatDS[1] = H5S_UNLIMITED;
                H5::DataSpace floatDataSpace = H5::DataSpace(2, initDimsFloatDS, maxDimsFloatDS);
                
                hsize_t dimsFloatChunk[2];
                dimsFloatChunk[0] = ATT_WRITE_CHUNK_SIZE;
                dimsFloatChunk[1] = 1;
                
                double fillValueFloat = 0;
                H5::DSetCreatPropList creationFloatDSPList;
                creationFloatDSPList.setChunk(2, dimsFloatChunk);
                creationFloatDSPList.setShuffle();
                creationFloatDSPList.setDeflate(ATT_WRITE_DEFLATE);
                creationFloatDSPList.setFillValue( H5::PredType::IEEE_F64LE, &fillValueFloat);
                
                floatDataset = new H5::DataSet(attH5File->createDataSet(ATT_FLOAT_DATA, H5::PredType::IEEE_F64LE, floatDataSpace, creationFloatDSPList));
            }
            
            // Create Neighbours dataset
            hsize_t initDimsNeighboursDS[1];
            initDimsNeighboursDS[0] = 0;
            hsize_t maxDimsNeighboursDS[1];
            maxDimsNeighboursDS[0] = H5S_UNLIMITED;
            H5::DataSpace neighboursDataspace = H5::DataSpace(1, initDimsNeighboursDS, maxDimsNeighboursDS);
            
            hsize_t dimsNeighboursChunk[1];
            dimsNeighboursChunk[0] = ATT_WRITE_CHUNK_SIZE;
            
            H5::DataType intVarLenDiskDT = H5::VarLenType(&H5::PredType::STD_U64LE);
            hvl_t neighboursDataFillVal[1];
            neighboursDataFillVal[0].p = NULL;
            neighboursDataFillVal[0].length = 0;
            H5::DSetCreatPropList creationNeighboursDSPList;
            creationNeighboursDSPList.setChunk(1, dimsNeighboursChunk);
            creationNeighboursDSPList.setShuffle();
            creationNeighboursDSPList.setDeflate(ATT_WRITE_DEFLATE);
            creationNeighboursDSPList.setFillValue( intVarLenDiskDT, &neighboursDataFillVal);
            
            H5::DataSet neighboursDataset = attH5File->createDataSet(ATT_NEIGHBOURS_DATA, intVarLenDiskDT, neighboursDataspace, creationNeighboursDSPList);
            
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
            H5::DataType intVarLenMemDT = H5::VarLenType(&H5::PredType::NATIVE_UINT64);
            
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
                    
                    H5::DataSpace boolWriteDataSpace = boolDataset->getSpace();
                    boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                    H5::DataSpace newBoolDataspace = H5::DataSpace(2, boolDataDims);
                    
                    boolDataset->write(boolVals, H5::PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                }
                
                if(this->numIntFields > 0)
                {
                    extendIntDatasetTo[0] = currentSize + ATT_WRITE_CHUNK_SIZE;
                    extendIntDatasetTo[1] = this->numIntFields;
                    intDataset->extend( extendIntDatasetTo );
                    
                    intDataOffset[0] = currentSize;
                    intDataOffset[1] = 0;
                    
                    H5::DataSpace intWriteDataSpace = intDataset->getSpace();
                    intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                    H5::DataSpace newIntDataspace = H5::DataSpace(2, intDataDims);
                    
                    intDataset->write(intVals, H5::PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                }
                
                if(this->numFloatFields > 0)
                {
                    extendFloatDatasetTo[0] = currentSize + ATT_WRITE_CHUNK_SIZE;
                    extendFloatDatasetTo[1] = this->numFloatFields;
                    floatDataset->extend( extendFloatDatasetTo );
                    
                    floatDataOffset[0] = currentSize;
                    floatDataOffset[1] = 0;
                    
                    H5::DataSpace floatWriteDataSpace = floatDataset->getSpace();
                    floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                    H5::DataSpace newFloatDataspace = H5::DataSpace(2, floatDataDims);
                    
                    floatDataset->write(floatVals, H5::PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                }
                
                extendNeighboursDatasetTo[0] = currentSize + ATT_WRITE_CHUNK_SIZE;
                neighboursDataset.extend( extendNeighboursDatasetTo );
                neighboursDataOffset[0] = currentSize;
                neighboursDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                
                H5::DataSpace neighboursWriteDataSpace = neighboursDataset.getSpace();
                neighboursWriteDataSpace.selectHyperslab(H5S_SELECT_SET, neighboursDataDims, neighboursDataOffset);
                H5::DataSpace newNeighboursDataspace = H5::DataSpace(1, neighboursDataDims);
                
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
            //std::cout << "Write remaining\n";
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
                        //std::cout << feat->fid << ": b" << k << " = " << feat->floatFields->at(k) << std::endl;
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
                extendBoolDatasetTo[0] = (numChunks * ATT_WRITE_CHUNK_SIZE) + remainingRows;
                extendBoolDatasetTo[1] = this->numBoolFields;
                boolDataset->extend( extendBoolDatasetTo );
                
                boolDataOffset[0] = numChunks * ATT_WRITE_CHUNK_SIZE;
                boolDataOffset[1] = 0;
                
                boolDataDims[0] = remainingRows;
                boolDataDims[1] = this->numBoolFields;
                
                H5::DataSpace boolWriteDataSpace = boolDataset->getSpace();
                boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                H5::DataSpace newBoolDataspace = H5::DataSpace(2, boolDataDims);
                
                boolDataset->write(boolVals, H5::PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
            }
            
            if(this->numIntFields > 0)
            {
                extendIntDatasetTo[0] = (numChunks * ATT_WRITE_CHUNK_SIZE) + remainingRows;
                extendIntDatasetTo[1] = this->numIntFields;
                intDataset->extend( extendIntDatasetTo );
                
                intDataOffset[0] = numChunks * ATT_WRITE_CHUNK_SIZE;
                intDataOffset[1] = 0;
                
                intDataDims[0] = remainingRows;
                intDataDims[1] = this->numIntFields;
                
                H5::DataSpace intWriteDataSpace = intDataset->getSpace();
                intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                H5::DataSpace newIntDataspace = H5::DataSpace(2, intDataDims);
                
                intDataset->write(intVals, H5::PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
            }
            
            if(this->numFloatFields > 0)
            {
                extendFloatDatasetTo[0] = (numChunks * ATT_WRITE_CHUNK_SIZE) + remainingRows;
                extendFloatDatasetTo[1] = this->numFloatFields;
                floatDataset->extend( extendFloatDatasetTo );
                
                floatDataOffset[0] = numChunks * ATT_WRITE_CHUNK_SIZE;
                floatDataOffset[1] = 0;
                
                floatDataDims[0] = remainingRows;
                floatDataDims[1] = this->numFloatFields;
                
                H5::DataSpace floatWriteDataSpace = floatDataset->getSpace();
                floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                H5::DataSpace newFloatDataspace = H5::DataSpace(2, floatDataDims);
                
                floatDataset->write(floatVals, H5::PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
            }
            
            
            extendNeighboursDatasetTo[0] = currentSize + remainingRows;
            neighboursDataset.extend( extendNeighboursDatasetTo );
            neighboursDataOffset[0] = numChunks * ATT_WRITE_CHUNK_SIZE;
            neighboursDataDims[0] = remainingRows;
            
            H5::DataSpace neighboursWriteDataSpace = neighboursDataset.getSpace();
            neighboursWriteDataSpace.selectHyperslab(H5S_SELECT_SET, neighboursDataDims, neighboursDataOffset);
            H5::DataSpace newNeighboursDataspace = H5::DataSpace(1, neighboursDataDims);
            
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
        catch( H5::FileIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    H5::CompType* RSGISAttributeTable::createAttibuteIdxCompTypeDisk() throw(RSGISAttributeTableException)
    {
        try
        {
            H5::StrType strTypeOut(0, H5T_VARIABLE);
            
            H5::CompType *attIdxDataType = new H5::CompType( sizeof(RSGISAttributeIdx) );
            attIdxDataType->insertMember(ATT_NAME_FIELD, HOFFSET(RSGISAttributeIdx, name), strTypeOut);
            attIdxDataType->insertMember(ATT_INDEX_FIELD, HOFFSET(RSGISAttributeIdx, idx), H5::PredType::STD_U32LE);
            return attIdxDataType;
        }
        catch( H5::FileIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    H5::CompType* RSGISAttributeTable::createAttibuteIdxCompTypeMem() throw(RSGISAttributeTableException)
    {
        try
        {
            H5::StrType strTypeIn(0, H5T_VARIABLE);
            
            H5::CompType *attIdxDataType = new H5::CompType( sizeof(RSGISAttributeIdx) );
            attIdxDataType->insertMember(ATT_NAME_FIELD, HOFFSET(RSGISAttributeIdx, name), strTypeIn);
            attIdxDataType->insertMember(ATT_INDEX_FIELD, HOFFSET(RSGISAttributeIdx, idx), H5::PredType::NATIVE_UINT);
            return attIdxDataType;
        }
        catch( H5::FileIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    void RSGISAttributeTable::exportGDALRaster(GDALDataset *inDataset, unsigned int inBand) throw(RSGISAttributeTableException)
    {
        try
        {
            GDALRasterAttributeTable *rasterAtt = NULL;
#ifdef HAVE_RFC40
            rasterAtt = new GDALDefaultRasterAttributeTable();
#else
            rasterAtt = new GDALRasterAttributeTable();
#endif // HAVE_RFC40
            
            rasterAtt->CreateColumn("FID", GFT_Integer, GFU_MinMax);
            unsigned int fidRATIdx = rasterAtt->GetColumnCount()-1;
            
            std::vector<std::pair<unsigned int, unsigned int> > intFieldRATIdxs;
            std::vector<std::pair<unsigned int, unsigned int> > floatFieldRATIdxs;
            std::vector<std::pair<unsigned int, unsigned int> > stringFieldRATIdxs;
            std::vector<std::pair<unsigned int, unsigned int> > boolFieldRATIdxs;
            
            std::cout << "There are " << fields->size() << " fields within the attribute table\n";
            
            std::vector<std::pair<std::string, RSGISAttributeDataType> >::iterator iterFields;
            for(iterFields = fields->begin(); iterFields != fields->end(); ++iterFields)
            {
                std::cout << "Adding Field: " << (*iterFields).first << std::endl;
                if((*iterFields).second == rsgis_int)
                {
                    rasterAtt->CreateColumn((*iterFields).first.c_str(), GFT_Integer, GFU_Generic);
                    intFieldRATIdxs.push_back(std::pair<unsigned int, unsigned int>(this->getFieldIndex((*iterFields).first), rasterAtt->GetColumnCount()-1));
                }
                else if((*iterFields).second == rsgis_float)
                {
                    rasterAtt->CreateColumn((*iterFields).first.c_str(), GFT_Real, GFU_Generic);
                    floatFieldRATIdxs.push_back(std::pair<unsigned int, unsigned int>(this->getFieldIndex((*iterFields).first), rasterAtt->GetColumnCount()-1));
                }
                else if((*iterFields).second == rsgis_string)
                {
                    rasterAtt->CreateColumn((*iterFields).first.c_str(), GFT_String, GFU_Generic);
                    stringFieldRATIdxs.push_back(std::pair<unsigned int, unsigned int>(this->getFieldIndex((*iterFields).first), rasterAtt->GetColumnCount()-1));
                }
                else if((*iterFields).second == rsgis_bool)
                {
                    rasterAtt->CreateColumn((*iterFields).first.c_str(), GFT_Integer, GFU_Generic);
                    boolFieldRATIdxs.push_back(std::pair<unsigned int, unsigned int>(this->getFieldIndex((*iterFields).first), rasterAtt->GetColumnCount()-1));
                }
                else
                {
                    std::cout << "Warning: " << (*iterFields).first << " will not be output as type unknown.\n";
                }
            }
            
            rasterAtt->SetRowCount(this->getSize()+1);
            
            size_t rowCounter = 1;
            for(this->start(); this->end(); ++(*this))
            {
                rasterAtt->SetValue(rowCounter, fidRATIdx, ((int)(*(*this))->fid+1));
                
                for(std::vector<std::pair<unsigned int, unsigned int> >::iterator iterAtts = intFieldRATIdxs.begin(); iterAtts != intFieldRATIdxs.end(); ++iterAtts)
                {
                    rasterAtt->SetValue(rowCounter, (*iterAtts).second, ((int)(*(*this))->intFields->at((*iterAtts).first)));
                }
                
                for(std::vector<std::pair<unsigned int, unsigned int> >::iterator iterAtts = floatFieldRATIdxs.begin(); iterAtts != floatFieldRATIdxs.end(); ++iterAtts)
                {
                    rasterAtt->SetValue(rowCounter, (*iterAtts).second, ((float)(*(*this))->floatFields->at((*iterAtts).first)));
                }
                
                for(std::vector<std::pair<unsigned int, unsigned int> >::iterator iterAtts = stringFieldRATIdxs.begin(); iterAtts != stringFieldRATIdxs.end(); ++iterAtts)
                {
                    rasterAtt->SetValue(rowCounter, (*iterAtts).second, (*(*this))->stringFields->at((*iterAtts).first).c_str());
                }
                
                for(std::vector<std::pair<unsigned int, unsigned int> >::iterator iterAtts = boolFieldRATIdxs.begin(); iterAtts != boolFieldRATIdxs.end(); ++iterAtts)
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
        catch(std::exception &e)
        {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
    }
    
    std::vector<double>* RSGISAttributeTable::getDoubleField(std::string field) throw(RSGISAttributeTableException)
    {
        std::vector<double> *vals = new std::vector<double>();
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
    
    std::vector<long>* RSGISAttributeTable::getLongField(std::string field) throw(RSGISAttributeTableException)
    {
        std::vector<long> *vals = new std::vector<long>();
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
    
    std::vector<bool>* RSGISAttributeTable::getBoolField(std::string field) throw(RSGISAttributeTableException)
    {
        std::vector<bool> *vals = new std::vector<bool>();
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
     
    std::vector<std::string>* RSGISAttributeTable::getStringField(std::string field) throw(RSGISAttributeTableException)
    {
        std::vector<std::string> *vals = new std::vector<std::string>();
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
    
    std::vector<RSGISIfStatement*>* RSGISAttributeTable::generateStatments(xercesc::DOMElement *argElement)throw(RSGISAttributeTableException)
    {
        std::vector<RSGISIfStatement*> *statements = new std::vector<RSGISIfStatement*>();
        
        try
        {
            rsgis::utils::RSGISTextUtils textUtils;
            XMLCh *rsgisIfXMLStr = xercesc::XMLString::transcode("rsgis:if");
            xercesc::DOMNodeList *ifNodesList = argElement->getElementsByTagName(rsgisIfXMLStr);
            unsigned int numIfStatments = ifNodesList->getLength();        
            
            if(numIfStatments == 0)
            {
                throw RSGISAttributeTableException("Must have at least 1 if statment.");
            }
            
            XMLCh *rsgisElseXMLStr = xercesc::XMLString::transcode("rsgis:else");
            xercesc::DOMNodeList *elseNodesList = argElement->getElementsByTagName(rsgisElseXMLStr);
            unsigned int numElseStatments = elseNodesList->getLength();        
            
            if(numElseStatments != 1)
            {
                throw RSGISAttributeTableException("Must have at 1 and only 1 else statment.");
            }
            
            XMLCh *fieldXMLStr = xercesc::XMLString::transcode("field");
            XMLCh *valueXMLStr = xercesc::XMLString::transcode("value");
            XMLCh *typeXMLStr = xercesc::XMLString::transcode("type");

            
            unsigned int numChildElements = argElement->getChildElementCount();
            xercesc::DOMElement *ifElement = argElement->getFirstElementChild();
            
            for(boost::uint_fast32_t i = 0; i < numChildElements; ++i)
            {
                const XMLCh* tagName = ifElement->getTagName();
                if(xercesc::XMLString::equals(tagName, rsgisIfXMLStr))
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
                        char *charValue = xercesc::XMLString::transcode(ifElement->getAttribute(fieldXMLStr));
                        ifStatment->field = std::string(charValue);
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("No \'field\' attribute was provided for if.");
                    }
                    
                    if(ifElement->hasAttribute(valueXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(ifElement->getAttribute(valueXMLStr));
                        ifStatment->value = textUtils.strto32bitInt(std::string(charValue));
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        std::cerr << "WARNING: No \'value\' attribute was provided for if - Defaulting to 0.\n";
                        ifStatment->value = 0;
                    }
                    statements->push_back(ifStatment);
                }
                else if(xercesc::XMLString::equals(tagName, rsgisElseXMLStr))
                {
                    RSGISIfStatement *elseStatment = new RSGISIfStatement();
                    elseStatment->exp = NULL;
                    elseStatment->noExp = true;
                    
                    bool fieldSet = false;
                    bool valueSet = false;
                    
                    if(ifElement->hasAttribute(fieldXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(ifElement->getAttribute(fieldXMLStr));
                        elseStatment->field = std::string(charValue);
                        fieldSet = true;
                        xercesc::XMLString::release(&charValue);
                    }
                    else
                    {
                        fieldSet = false;
                        //throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                    }
                    
                    if(ifElement->hasAttribute(valueXMLStr))
                    {
                        char *charValue = xercesc::XMLString::transcode(ifElement->getAttribute(valueXMLStr));
                        elseStatment->value = textUtils.strto32bitInt(std::string(charValue));
                        valueSet = true;
                        xercesc::XMLString::release(&charValue);
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
            
			xercesc::XMLString::release(&fieldXMLStr);
            xercesc::XMLString::release(&valueXMLStr);
            xercesc::XMLString::release(&typeXMLStr);
            xercesc::XMLString::release(&rsgisIfXMLStr);
            xercesc::XMLString::release(&rsgisElseXMLStr);
            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        
        return statements;
    }
    
    
    RSGISAttExpression* RSGISAttributeTable::generateExpression(xercesc::DOMElement *expElement)throw(RSGISAttributeTableException)
    {
        RSGISAttExpression *exp = NULL;
        
        try 
        {
            rsgis::utils::RSGISTextUtils textUtils;
            XMLCh *rsgisExpXMLStr = xercesc::XMLString::transcode("rsgis:exp");
            const XMLCh* tagName = expElement->getTagName();
            if(xercesc::XMLString::equals(tagName, rsgisExpXMLStr))
            {
                XMLCh *andXMLStr = xercesc::XMLString::transcode("and");
                XMLCh *orXMLStr = xercesc::XMLString::transcode("or");
                XMLCh *notXMLStr = xercesc::XMLString::transcode("not");
                XMLCh *gtXMLStr = xercesc::XMLString::transcode("gt");
                XMLCh *ltXMLStr = xercesc::XMLString::transcode("lt");
                XMLCh *gteqXMLStr = xercesc::XMLString::transcode("gteq");
                XMLCh *lteqXMLStr = xercesc::XMLString::transcode("lteq");
                XMLCh *eqXMLStr = xercesc::XMLString::transcode("eq");
                XMLCh *neqXMLStr = xercesc::XMLString::transcode("neq");
                XMLCh *gtConstXMLStr = xercesc::XMLString::transcode("gtconst");
                XMLCh *ltConstXMLStr = xercesc::XMLString::transcode("ltconst");
                XMLCh *gteqConstXMLStr = xercesc::XMLString::transcode("gteqconst");
                XMLCh *lteqConstXMLStr = xercesc::XMLString::transcode("lteqconst");
                XMLCh *eqConstXMLStr = xercesc::XMLString::transcode("eqconst");
                XMLCh *neqConstXMLStr = xercesc::XMLString::transcode("netconst");
                XMLCh *constGTXMLStr = xercesc::XMLString::transcode("constgt");
                XMLCh *constLTXMLStr = xercesc::XMLString::transcode("constlt");
                XMLCh *constGTEQXMLStr = xercesc::XMLString::transcode("constgteq");
                XMLCh *constLTEQXMLStr = xercesc::XMLString::transcode("constlteq");
                
                XMLCh *field1XMLStr = xercesc::XMLString::transcode("field1");
                XMLCh *field2XMLStr = xercesc::XMLString::transcode("field2");
                XMLCh *fieldXMLStr = xercesc::XMLString::transcode("field");
                XMLCh *valueXMLStr = xercesc::XMLString::transcode("value");
                XMLCh *typeXMLStr = xercesc::XMLString::transcode("type");
                
                
                std::string f1Name = "";
                unsigned int f1Idx = 0;
                RSGISAttributeDataType f1Type = rsgis_na;
                std::string f2Name = "";
                unsigned int f2Idx = 0;
                RSGISAttributeDataType f2Type = rsgis_na;
                float value = 0;
                
                if(expElement->hasAttribute(typeXMLStr))
                {
                    const XMLCh *typeInXMLStr = expElement->getAttribute(typeXMLStr);
                    
                    if(xercesc::XMLString::equals(typeInXMLStr, andXMLStr))
                    {
                        unsigned int numChildElements = expElement->getChildElementCount();
                        if(numChildElements == 0)
                        {
                            throw RSGISAttributeTableException("There must be at least one child element of a AND expression.");
                        }
                        
                        std::vector<RSGISAttExpression*> *exps = new std::vector<RSGISAttExpression*>();
                        
                        xercesc::DOMElement *expElementChild = expElement->getFirstElementChild();
                        for(unsigned int i = 0; i < numChildElements; ++i)
                        {
                            exps->push_back(RSGISAttributeTable::generateExpression(expElementChild));
                            
                            expElementChild = expElementChild->getNextElementSibling();
                        }
                        
                        exp = new RSGISAttExpressionAND(exps);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, orXMLStr))
                    {
                        unsigned int numChildElements = expElement->getChildElementCount();
                        if(numChildElements == 0)
                        {
                            throw RSGISAttributeTableException("There must be at least one child element of a OR expression.");
                        }
                        
                        std::vector<RSGISAttExpression*> *exps = new std::vector<RSGISAttExpression*>();
                        
                        xercesc::DOMElement *expElementChild = expElement->getFirstElementChild();
                        for(unsigned int i = 0; i < numChildElements; ++i)
                        {
                            exps->push_back(RSGISAttributeTable::generateExpression(expElementChild));
                            
                            expElementChild = expElementChild->getNextElementSibling();
                        }
                        
                        exp = new RSGISAttExpressionOR(exps);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, notXMLStr))
                    {
                        unsigned int numChildElements = expElement->getChildElementCount();
                        if(numChildElements != 1)
                        {
                            throw RSGISAttributeTableException("There can only be one child element of a NOT expression.");
                        }                        
                        exp = new RSGISAttExpressionNot(RSGISAttributeTable::generateExpression(expElement->getFirstElementChild()));
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, gtXMLStr))
                    {
                        if(expElement->hasAttribute(field1XMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(field1XMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field1\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(field2XMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(field2XMLStr));
                            f2Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field2\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionGreaterThan(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, ltXMLStr))
                    {
                        if(expElement->hasAttribute(field1XMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(field1XMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field1\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(field2XMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(field2XMLStr));
                            f2Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field2\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionLessThan(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, gteqXMLStr))
                    {
                        if(expElement->hasAttribute(field1XMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(field1XMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field1\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(field2XMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(field2XMLStr));
                            f2Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field2\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionGreaterThanEq(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, lteqXMLStr))
                    {
                        if(expElement->hasAttribute(field1XMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(field1XMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field1\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(field2XMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(field2XMLStr));
                            f2Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field2\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionLessThanEq(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, eqXMLStr))
                    {
                        if(expElement->hasAttribute(field1XMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(field1XMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field1\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(field2XMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(field2XMLStr));
                            f2Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field2\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionEquals(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, neqXMLStr))
                    {
                        if(expElement->hasAttribute(field1XMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(field1XMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field1\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(field2XMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(field2XMLStr));
                            f2Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field2\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionNotEquals(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, gtConstXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(std::string(charValue));
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionGreaterThanConst(f1Name, f1Idx, f1Type, value);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, ltConstXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(std::string(charValue));
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionLessThanConst(f1Name, f1Idx, f1Type, value);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, gteqConstXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(std::string(charValue));
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionGreaterThanConstEq(f1Name, f1Idx, f1Type, value);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, lteqConstXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(std::string(charValue));
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionLessThanConstEq(f1Name, f1Idx, f1Type, value);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, eqConstXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(std::string(charValue));
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionEqualsConst(f1Name, f1Idx, f1Type, value);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, neqConstXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(std::string(charValue));
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionNotEqualsConst(f1Name, f1Idx, f1Type, value);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, constGTXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(std::string(charValue));
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionConstGreaterThan(f1Name, f1Idx, f1Type, value);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, constLTXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(std::string(charValue));
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionConstLessThan(f1Name, f1Idx, f1Type, value);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, constGTEQXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(std::string(charValue));
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'value\' attribute was provided for else.");
                        }
                        
                        exp = new RSGISAttExpressionConstGreaterThanEq(f1Name, f1Idx, f1Type, value);
                    }
                    else if(xercesc::XMLString::equals(typeInXMLStr, constLTEQXMLStr))
                    {
                        if(expElement->hasAttribute(fieldXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(fieldXMLStr));
                            f1Name = std::string(charValue);
                            xercesc::XMLString::release(&charValue);
                        }
                        else
                        {
                            throw RSGISAttributeTableException("No \'field\' attribute was provided for else.");
                        }
                        
                        if(expElement->hasAttribute(valueXMLStr))
                        {
                            char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(valueXMLStr));
                            value = textUtils.strtofloat(std::string(charValue));
                            xercesc::XMLString::release(&charValue);
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
                
                xercesc::XMLString::release(&typeXMLStr);
                xercesc::XMLString::release(&valueXMLStr);
                xercesc::XMLString::release(&fieldXMLStr);
                xercesc::XMLString::release(&field2XMLStr);
                xercesc::XMLString::release(&field1XMLStr);
                xercesc::XMLString::release(&andXMLStr);
                xercesc::XMLString::release(&orXMLStr);
                xercesc::XMLString::release(&notXMLStr);
                xercesc::XMLString::release(&gtXMLStr);
                xercesc::XMLString::release(&ltXMLStr);
                xercesc::XMLString::release(&gteqXMLStr);
                xercesc::XMLString::release(&lteqXMLStr);
                xercesc::XMLString::release(&eqXMLStr);
                xercesc::XMLString::release(&neqXMLStr);
                xercesc::XMLString::release(&gtConstXMLStr);
                xercesc::XMLString::release(&ltConstXMLStr);
                xercesc::XMLString::release(&gteqConstXMLStr);
                xercesc::XMLString::release(&lteqConstXMLStr);
                xercesc::XMLString::release(&eqConstXMLStr);
                xercesc::XMLString::release(&neqConstXMLStr);
                xercesc::XMLString::release(&constGTXMLStr);
                xercesc::XMLString::release(&constLTXMLStr);
                xercesc::XMLString::release(&constGTEQXMLStr);
                xercesc::XMLString::release(&constLTEQXMLStr);
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
    
    
    void RSGISAttributeTable::printSummaryHDFAtt(std::string inFile)throw(RSGISAttributeTableException)
    {
        try
        {
            H5::Exception::dontPrint();
            
            H5::FileAccPropList attAccessPlist = H5::FileAccPropList(H5::FileAccPropList::DEFAULT);
            attAccessPlist.setCache(ATT_READ_MDC_NELMTS, ATT_READ_RDCC_NELMTS, ATT_READ_RDCC_NBYTES, ATT_READ_RDCC_W0);
            attAccessPlist.setSieveBufSize(ATT_READ_SIEVE_BUF);
            hsize_t metaBlockSize = ATT_READ_META_BLOCKSIZE;
            attAccessPlist.setMetaBlockSize(metaBlockSize);
            
            H5::H5File *attH5File = new H5::H5File( inFile, H5F_ACC_RDONLY, H5::FileCreatPropList::DEFAULT, attAccessPlist);
            
            bool hasBoolFields = true;
            bool hasIntFields = true;
            bool hasFloatFields = true;
            
            hsize_t dimsAttSize[1];
			dimsAttSize[0] = 1;
            size_t numFeats = 0;
			H5::DataSpace attSizeDataSpace(1, dimsAttSize);
            H5::DataSet sizeDataset = attH5File->openDataSet( ATT_SIZE_HEADER );
            sizeDataset.read(&numFeats, H5::PredType::NATIVE_ULLONG, attSizeDataSpace);
            attSizeDataSpace.close();
            
            std::cout << "Table has " << numFeats << " features\n";
            
            H5::CompType *fieldCompTypeMem = RSGISAttributeTable::createAttibuteIdxCompTypeMem();
            try
            {
                H5::DataSet boolFieldsDataset = attH5File->openDataSet( ATT_BOOL_FIELDS_HEADER );
                H5::DataSpace boolFieldsDataspace = boolFieldsDataset.getSpace();
                
                unsigned int numBoolFields = boolFieldsDataspace.getSelectNpoints();
                
                std::cout << "There are " << numBoolFields << " boolean fields." << std::endl;
                
                RSGISAttributeIdx *fields = new RSGISAttributeIdx[numBoolFields];
                
                hsize_t boolFieldsDims[1]; 
                boolFieldsDims[0] = numBoolFields;
                H5::DataSpace boolFieldsMemspace(1, boolFieldsDims);
                
                boolFieldsDataset.read(fields, *fieldCompTypeMem, boolFieldsMemspace, boolFieldsDataspace);
                
                for(unsigned int i = 0; i < numBoolFields; ++i)
                {
                    std::cout << "Boolean Field: " << fields[i].name << " has index " << fields[i].idx << std::endl;
                }
                
                delete[] fields;
            }
            catch( H5::Exception &e )
            {
                hasBoolFields = false;
            }
            
            try
            {
                H5::DataSet intFieldsDataset = attH5File->openDataSet( ATT_INT_FIELDS_HEADER );
                H5::DataSpace intFieldsDataspace = intFieldsDataset.getSpace();
                
                unsigned int numIntFields = intFieldsDataspace.getSelectNpoints();
                
                std::cout << "There are " << numIntFields << " integer fields." << std::endl;
                
                RSGISAttributeIdx *fields = new RSGISAttributeIdx[numIntFields];
                
                hsize_t intFieldsDims[1]; 
                intFieldsDims[0] = numIntFields;
                H5::DataSpace intFieldsMemspace(1, intFieldsDims);
                
                intFieldsDataset.read(fields, *fieldCompTypeMem, intFieldsMemspace, intFieldsDataspace);
                
                for(unsigned int i = 0; i < numIntFields; ++i)
                {
                    std::cout << "Integer Field: " << fields[i].name << ": " << fields[i].idx << std::endl;
                }
                
                delete[] fields;
            }
            catch( H5::Exception &e )
            {
                hasIntFields = false;
            }
            
            try
            {
                H5::DataSet floatFieldsDataset = attH5File->openDataSet( ATT_FLOAT_FIELDS_HEADER );
                H5::DataSpace floatFieldsDataspace = floatFieldsDataset.getSpace();
                
                unsigned int numFloatFields = floatFieldsDataspace.getSelectNpoints();
                
                std::cout << "There are " << numFloatFields << " float fields." << std::endl;
                
                RSGISAttributeIdx *fields = new RSGISAttributeIdx[numFloatFields];
                
                hsize_t floatFieldsDims[1]; 
                floatFieldsDims[0] = numFloatFields;
                H5::DataSpace floatFieldsMemspace(1, floatFieldsDims);
                
                floatFieldsDataset.read(fields, *fieldCompTypeMem, floatFieldsMemspace, floatFieldsDataspace);
                
                for(unsigned int i = 0; i < numFloatFields; ++i)
                {
                    std::cout << "Float Field: " << fields[i].name << ": " << fields[i].idx << std::endl;
                }
                
                delete[] fields;
            }
            catch( H5::Exception &e )
            {
                hasFloatFields = false;
            }
            
            delete fieldCompTypeMem;
            
            H5::DataSet neighboursDataset = attH5File->openDataSet( ATT_NEIGHBOURS_DATA );
            H5::DataSpace neighboursDataspace = neighboursDataset.getSpace();
            
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
            H5::DataType intVarLenMemDT = H5::VarLenType(&H5::PredType::NATIVE_UINT64);
            hsize_t neighboursOffset[1];
			neighboursOffset[0] = 0;
			hsize_t neighboursCount[1];
			neighboursCount[0] = ATT_WRITE_CHUNK_SIZE;
			neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
			
			hsize_t neighboursDimsRead[1]; 
			neighboursDimsRead[0] = ATT_WRITE_CHUNK_SIZE;
			H5::DataSpace neighboursMemspace( 1, neighboursDimsRead );
			
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
			neighboursMemspace = H5::DataSpace( 1, neighboursDimsRead );
			
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
        catch( H5::FileIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        catch( H5::Exception &e )
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




