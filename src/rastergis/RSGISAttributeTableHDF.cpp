/*
 *  RSGISAttributeTableHDF.cpp
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
#include "RSGISAttributeTableHDF.h"

namespace rsgis{namespace rastergis{
    
    RSGISAttributeTableHDF::RSGISAttributeTableHDF() : RSGISAttributeTable()
    {
        this->attOpen = false;
        this->attSize = 0;
        this->readOnly = true;
        this->maxCacheSize = 10000;
        this->numOfReads = 0;
        this->numOfWrites = 0;
        this->featCache = NULL;
        this->cacheQ = new list<size_t>();
        this->numOfBlocks = 0;
        this->remainingFeatures = 0;
        this->maxNumOfBlockInCache = (this->maxCacheSize / ATT_WRITE_CHUNK_SIZE)+1;
        this->heldBlocks = new map<size_t,size_t>();
    }
    
    RSGISAttributeTableHDF::RSGISAttributeTableHDF(size_t numFeatures, string filePath, bool readOnly, size_t maxCacheSize) throw(RSGISAttributeTableException) : RSGISAttributeTable()
    {        
        try
        {
            this->readOnly = readOnly;
            this->maxCacheSize = maxCacheSize;
            this->featCache = NULL;
            this->cacheQ = new list<size_t>();
            this->numOfReads = 0;
            this->numOfWrites = 0;
            this->numOfBlocks = 0;
            this->remainingFeatures = 0;
            this->maxNumOfBlockInCache = (this->maxCacheSize / ATT_WRITE_CHUNK_SIZE)+1;
            this->heldBlocks = new map<size_t,size_t>();
            this->createAttributeTable(numFeatures, filePath);
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    RSGISAttributeTableHDF::RSGISAttributeTableHDF(size_t numFeatures, vector<pair<string, RSGISAttributeDataType> > *fields, string filePath, bool readOnly, size_t maxCacheSize) throw(RSGISAttributeTableException) : RSGISAttributeTable()
    {
        this->readOnly = readOnly;
        this->maxCacheSize = maxCacheSize;
        this->attOpen = false;
        this->attSize = 0;
        this->featCache = NULL;
        this->cacheQ = new list<size_t>();
        this->numOfReads = 0;
        this->numOfWrites = 0;
        this->numOfBlocks = 0;
        this->remainingFeatures = 0;
        this->maxNumOfBlockInCache = 0;
        this->heldBlocks = new map<size_t,size_t>();
    }
    
    void RSGISAttributeTableHDF::createAttributeTable(size_t numFeatures, string filePath) throw(RSGISAttributeTableException)
    {
        try
        {
            this->fields = new vector<pair<string, RSGISAttributeDataType> >();
            this->fieldIdx = new map<string, unsigned int>();
            this->fieldDataType = new map<string, RSGISAttributeDataType>();
            
            this->numBoolFields = 0;
            this->numIntFields = 0;
            this->numFloatFields = 0;
            this->numStrFields = 0;
            
            Exception::dontPrint();
            
            FileAccPropList attAccessPlist = FileAccPropList(FileAccPropList::DEFAULT);
            attAccessPlist.setCache(ATT_WRITE_MDC_NELMTS, ATT_WRITE_RDCC_NELMTS, ATT_WRITE_RDCC_NBYTES, ATT_WRITE_RDCC_W0);
            attAccessPlist.setSieveBufSize(ATT_WRITE_SIEVE_BUF);
            hsize_t metaBlockSize = ATT_WRITE_META_BLOCKSIZE;
            attAccessPlist.setMetaBlockSize(metaBlockSize);
            
            const H5std_string attFilePath( filePath );
            this->attH5File = new H5File( attFilePath, H5F_ACC_TRUNC, FileCreatPropList::DEFAULT, attAccessPlist);
            
            attH5File->createGroup( ATT_GROUPNAME_HEADER );
			attH5File->createGroup( ATT_GROUPNAME_DATA );
            attH5File->createGroup( ATT_GROUPNAME_NEIGHBOURS );
            
            hsize_t dimsAttSize[1];
			dimsAttSize[0] = 1;
			DataSpace attSizeDataSpace(1, dimsAttSize);
            sizeDataset = attH5File->createDataSet(ATT_SIZE_HEADER, PredType::STD_U64LE, attSizeDataSpace);
			sizeDataset.write( &numFeatures, PredType::STD_U64LE );
            attSizeDataSpace.close();
                        
            // Create Neighbours dataset
            hsize_t initDimsNeighboursDS[1];
            initDimsNeighboursDS[0] = 0;
            hsize_t maxDimsNeighboursDS[1];
            maxDimsNeighboursDS[0] = H5S_UNLIMITED;
            DataSpace neighboursDataspace = DataSpace(1, initDimsNeighboursDS, maxDimsNeighboursDS);
            
            hsize_t dimsNeighboursChunk[1];
            dimsNeighboursChunk[0] = ATT_WRITE_CHUNK_SIZE;
            
            DataType intVarLenDiskDT = VarLenType(&PredType::STD_U64LE);
            DataType intVarLenMemDT = VarLenType(&PredType::STD_U64LE);
            hvl_t neighboursDataFillVal[1];
            neighboursDataFillVal[0].p = NULL;
            neighboursDataFillVal[0].length = 0;
            DSetCreatPropList creationNeighboursDSPList;
            creationNeighboursDSPList.setChunk(1, dimsNeighboursChunk);
            creationNeighboursDSPList.setShuffle();
            creationNeighboursDSPList.setDeflate(ATT_WRITE_DEFLATE);
            creationNeighboursDSPList.setFillValue( intVarLenDiskDT, &neighboursDataFillVal);
            
            this->neighboursDataset = attH5File->createDataSet(ATT_NEIGHBOURS_DATA, intVarLenDiskDT, neighboursDataspace, creationNeighboursDSPList);
         
            
            hvl_t *neighbourVals = new hvl_t[ATT_WRITE_CHUNK_SIZE];
            
            for(size_t i = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
            {
                neighbourVals[i].length = 0;
                neighbourVals[i].p = NULL;
            }
            
            unsigned long numChunks = numFeatures / ATT_WRITE_CHUNK_SIZE;
            unsigned long remainingRows = numFeatures - (numChunks * ATT_WRITE_CHUNK_SIZE);
            
            hsize_t extendNeighboursDatasetTo[1];
            extendNeighboursDatasetTo[0] = numFeatures;
            neighboursDataset.extend( extendNeighboursDatasetTo );
            
            hsize_t neighboursDataOffset[1];
            neighboursDataOffset[0] = 0;
            hsize_t neighboursDataDims[1];
            neighboursDataDims[0] = ATT_WRITE_CHUNK_SIZE;

            size_t currentSize = 0;
            for(unsigned long i = 0; i < numChunks; ++i)
            {
                neighboursDataOffset[0] = currentSize;
                
                DataSpace neighboursWriteDataSpace = neighboursDataset.getSpace();
                neighboursWriteDataSpace.selectHyperslab(H5S_SELECT_SET, neighboursDataDims, neighboursDataOffset);
                DataSpace newNeighboursDataspace = DataSpace(1, neighboursDataDims);
                
                neighboursDataset.write(neighbourVals, intVarLenDiskDT, newNeighboursDataspace, neighboursWriteDataSpace);
                
                currentSize += ATT_WRITE_CHUNK_SIZE;
                neighboursWriteDataSpace.close();
                newNeighboursDataspace.close();
            }
            
            
            neighboursDataDims[0] = remainingRows;
            neighboursDataOffset[0] = currentSize;
            DataSpace neighboursWriteDataSpace = neighboursDataset.getSpace();
            neighboursWriteDataSpace.selectHyperslab(H5S_SELECT_SET, neighboursDataDims, neighboursDataOffset);
            DataSpace newNeighboursDataspace = DataSpace(1, neighboursDataDims);
            neighboursDataset.write(neighbourVals, intVarLenDiskDT, newNeighboursDataspace, neighboursWriteDataSpace);
            neighboursWriteDataSpace.close();
            newNeighboursDataspace.close();

            delete[] neighbourVals;
            
            
            hasBoolFields = false;
            hasIntFields = false;
            hasFloatFields = false;
            iterIdx = 0;
            this->attSize = numFeatures;
            attOpen = true;
            
            this->featCache = new RSGISFeature*[this->attSize];
            numOfBlocks = attSize / ATT_WRITE_CHUNK_SIZE;
            remainingFeatures = attSize - (numOfBlocks * ATT_WRITE_CHUNK_SIZE);
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
        
    bool RSGISAttributeTableHDF::getBoolField(size_t fid, string name) throw(RSGISAttributeTableException)
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
            if(fid >= this->getSize())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            if(this->featCache[fid] == NULL)
            {
                this->populateCache(fid);
            }
            outVal = this->featCache[fid]->boolFields->at(idx);
            
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        
        return outVal;
    }
    
    long RSGISAttributeTableHDF::getIntField(size_t fid, string name) throw(RSGISAttributeTableException)
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
            if(fid >= this->getSize())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            if(this->featCache[fid] == NULL)
            {
                this->populateCache(fid);
            }
            outVal = this->featCache[fid]->intFields->at(idx);
            
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        
        return outVal;
    }
    
    double RSGISAttributeTableHDF::getDoubleField(size_t fid, string name) throw(RSGISAttributeTableException)
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
            if(fid >= this->getSize())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            if(this->featCache[fid] == NULL)
            {
                this->populateCache(fid);
            }
            outVal = this->featCache[fid]->floatFields->at(idx);
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        
        return outVal;
    }

    string RSGISAttributeTableHDF::getStringField(size_t fid, string name) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("String fields are not currently supported within HDF5 attribute tables.");
        return "";
    }
    
        
    void RSGISAttributeTableHDF::setBoolField(size_t fid, string name, bool value) throw(RSGISAttributeTableException)
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
            if(fid >= this->getSize())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            if(this->featCache[fid] == NULL)
            {
                this->populateCache(fid);
            }
            this->featCache[fid]->boolFields->at(idx) = value;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    void RSGISAttributeTableHDF::setIntField(size_t fid, string name, long value) throw(RSGISAttributeTableException)
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
            if(fid >= this->getSize())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            if(this->featCache[fid] == NULL)
            {
                this->populateCache(fid);
            }
            this->featCache[fid]->intFields->at(idx) = value;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    void RSGISAttributeTableHDF::setDoubleField(size_t fid, string name, double value) throw(RSGISAttributeTableException)
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
            if(fid >= this->getSize())
            {
                string message = string("Feature (") + name + string(") is not within the attribute table.");
                throw RSGISAttributeTableException(message);
            }
            
            if(this->featCache[fid] == NULL)
            {
                this->populateCache(fid);
            }
            this->featCache[fid]->floatFields->at(idx) = value;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    void RSGISAttributeTableHDF::setStringField(size_t fid, string name, string value) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("String fields are not currently supported within HDF5 attribute tables.");
    }
        
    void RSGISAttributeTableHDF::setBoolValue(string name, bool value) throw(RSGISAttributeTableException)
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
            
            for(this->start(); this->end(); ++(*this))
            {
                (*(*this))->boolFields->at(idx) = value;
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    void RSGISAttributeTableHDF::setIntValue(string name, long value) throw(RSGISAttributeTableException)
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
            
            for(this->start(); this->end(); ++(*this))
            {
                (*(*this))->intFields->at(idx) = value;
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
        
    void RSGISAttributeTableHDF::setFloatValue(string name, double value) throw(RSGISAttributeTableException)
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
            
            for(this->start(); this->end(); ++(*this))
            {
                (*(*this))->floatFields->at(idx) = value;
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    void RSGISAttributeTableHDF::setStringValue(string name, string value) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("String fields are not currently supported within HDF5 attribute tables.");
    }
        
    RSGISFeature* RSGISAttributeTableHDF::getFeature(size_t fid) throw(RSGISAttributeTableException)
    {
        //cout << "Retrieving feature = " << fid << endl;
        if(fid >= this->attSize)
        {
            cout << "Couldn't find " << fid << endl;
            throw RSGISAttributeTableException("The requested feature is not within attribute table.");
        }
        RSGISFeature *feat = NULL;
        try
        {
            if(this->featCache[fid] == NULL)
            {
                this->populateCache(fid);
            }
            /*else
            {
                size_t block = fid / ATT_WRITE_CHUNK_SIZE;
                // Move block to back of queue.
                for(list<size_t>::iterator iterBlocks = cacheQ->begin(); iterBlocks != cacheQ->end(); )
                {
                    if((*iterBlocks) == block)
                    {
                        iterBlocks = cacheQ->erase(iterBlocks);
                        cacheQ->push_front(block);
                        break;
                    }
                    else
                    {
                        ++iterBlocks;
                    }
                }
            }*/
            
            feat = featCache[fid];
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        
        return feat;
    }
    
    void RSGISAttributeTableHDF::flushAllFeatures(bool progressFeedback) throw(RSGISAttributeTableException)
    {
        try
        {
            int feedback = cacheQ->size()/10;
			int feedbackCounter = 0;
            size_t idx = 0;
            if(progressFeedback)
            {
                cout << "Started (Flushing Att)" << flush;
            }
            for(list<size_t>::reverse_iterator iterBlocks = cacheQ->rbegin(); iterBlocks != cacheQ->rend(); ++iterBlocks)
            {
                if(progressFeedback && ((idx % feedback) == 0))
                {
                    cout << "." << feedbackCounter << "." << flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                this->removeFromCache(*iterBlocks);
                ++idx;
            }
            if(progressFeedback)
            {
                cout << " Complete.\n";
            }
            
            cacheQ->clear();
            this->heldBlocks->clear();
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
        
    void RSGISAttributeTableHDF::addAttBoolField(string name, bool val) throw(RSGISAttributeTableException)
    {
        if(this->hasAttribute(name))
        {
            string message = string("The field \'") + name + string("\' already exists within the attribute table.");
            throw RSGISAttributeTableException(message);
        }
        
        try
        {
            this->flushAllFeatures();
            
            fieldIdx->insert(pair<string, unsigned int>(name, numBoolFields));
            fieldDataType->insert(pair<string, RSGISAttributeDataType>(name, rsgis_bool));
            fields->push_back(pair<string, RSGISAttributeDataType>(name, rsgis_bool));
            
            CompType *fieldDtDisk = this->createAttibuteIdxCompTypeDisk();
            RSGISAttributeIdx *boolFields = new RSGISAttributeIdx[1];
            boolFields[0].name = const_cast<char*>(name.c_str());
            boolFields[0].idx = this->numBoolFields++;
            
            if(this->hasBoolFields)
            {
                DataSet boolFieldsDataset = attH5File->openDataSet( ATT_BOOL_FIELDS_HEADER );
                DataSpace boolFieldsDataSpace = boolFieldsDataset.getSpace();
                
                hsize_t extendBoolFieldsDatasetTo[1];
                extendBoolFieldsDatasetTo[0] = this->numBoolFields;
                boolFieldsDataset.extend( extendBoolFieldsDatasetTo );
                
                hsize_t boolFieldsOffset[1];
                boolFieldsOffset[0] = this->numBoolFields-1;
                hsize_t boolFieldsDataDims[1];
                boolFieldsDataDims[0] = 1;
                
                DataSpace boolFieldsWriteDataSpace = boolFieldsDataset.getSpace();
                boolFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolFieldsDataDims, boolFieldsOffset);
                DataSpace newBoolFieldsDataspace = DataSpace(1, boolFieldsDataDims);
                
                boolFieldsDataset.write(boolFields, *fieldDtDisk, newBoolFieldsDataspace, boolFieldsWriteDataSpace);
                
                hsize_t extendBoolDatasetTo[2];
                extendBoolDatasetTo[0] = this->getSize();
                extendBoolDatasetTo[1] = this->numBoolFields;
                this->boolDataset.extend( extendBoolDatasetTo );
                
                int *boolVals = new int[ATT_WRITE_CHUNK_SIZE];
                for(unsigned int i  = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    boolVals[i] = val;
                }
                
                unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
                unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
                
                hsize_t boolDataOffset[2];
                boolDataOffset[0] = 0;
                boolDataOffset[1] = 0;
                hsize_t boolDataDims[2];
                boolDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                boolDataDims[1] = 1;
                
                size_t currentSize = 0;
                for(unsigned long i = 0; i < numChunks; ++i)
                {
                    boolDataOffset[0] = currentSize;
                    boolDataOffset[1] = this->numBoolFields-1;
                    
                    DataSpace boolWriteDataSpace = this->boolDataset.getSpace();
                    boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                    DataSpace newBoolDataspace = DataSpace(2, boolDataDims);
                    
                    this->boolDataset.write(boolVals, PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                    
                    currentSize += ATT_WRITE_CHUNK_SIZE;
                    boolWriteDataSpace.close();
                    newBoolDataspace.close();
                }
                
                boolDataDims[0] = remainingRows;
                boolDataDims[1] = 1;
                
                boolDataOffset[0] = currentSize;
                boolDataOffset[1] = this->numBoolFields-1;
                
                DataSpace boolWriteDataSpace = this->boolDataset.getSpace();
                boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                DataSpace newBoolDataspace = DataSpace(2, boolDataDims);
                
                this->boolDataset.write(boolVals, PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                
                delete[] boolVals;
                boolFieldsDataSpace.close();
                boolFieldsDataset.close();
                boolFieldsWriteDataSpace.close();
                newBoolFieldsDataspace.close();
                boolWriteDataSpace.close();
                newBoolDataspace.close();
            }
            else
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
                DataSet boolFieldsDataset = DataSet(attH5File->createDataSet(ATT_BOOL_FIELDS_HEADER, *fieldDtDisk, boolFieldsDataSpace, creationBoolFieldsDSPList));
                
                hsize_t extendBoolFieldsDatasetTo[1];
                extendBoolFieldsDatasetTo[0] = this->numBoolFields;
                boolFieldsDataset.extend( extendBoolFieldsDatasetTo );
                
                hsize_t boolFieldsOffset[1];
                boolFieldsOffset[0] = 0;
                hsize_t boolFieldsDataDims[1];
                boolFieldsDataDims[0] = this->numBoolFields;
                
                DataSpace boolFieldsWriteDataSpace = boolFieldsDataset.getSpace();
                boolFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolFieldsDataDims, boolFieldsOffset);
                DataSpace newBoolFieldsDataspace = DataSpace(1, boolFieldsDataDims);
                
                boolFieldsDataset.write(boolFields, *fieldDtDisk, newBoolFieldsDataspace, boolFieldsWriteDataSpace);
                
                hsize_t initDimsBoolDS[2];
                initDimsBoolDS[0] = this->getSize();
                initDimsBoolDS[1] = this->numBoolFields;
                hsize_t maxDimsBoolDS[2];
                maxDimsBoolDS[0] = H5S_UNLIMITED;
                maxDimsBoolDS[1] = H5S_UNLIMITED;
                DataSpace boolDataSpace = DataSpace(2, initDimsBoolDS, maxDimsBoolDS);
                
                hsize_t dimsBoolChunk[2];
                dimsBoolChunk[0] = ATT_WRITE_CHUNK_SIZE;
                dimsBoolChunk[1] = 1;
                
                int fillValueBool = val;
                DSetCreatPropList creationBoolDSPList;
                creationBoolDSPList.setChunk(2, dimsBoolChunk);
                creationBoolDSPList.setShuffle();
                creationBoolDSPList.setDeflate(ATT_WRITE_DEFLATE);
                creationBoolDSPList.setFillValue( PredType::STD_I8LE, &fillValueBool);
                
                this->boolDataset = DataSet(attH5File->createDataSet(ATT_BOOL_DATA, PredType::STD_I8LE, boolDataSpace, creationBoolDSPList));
                
                int *boolVals = new int[ATT_WRITE_CHUNK_SIZE];
                for(unsigned int i  = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    boolVals[i] = val;
                }
                
                unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
                unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
                
                hsize_t boolDataOffset[2];
                boolDataOffset[0] = 0;
                boolDataOffset[1] = 0;
                hsize_t boolDataDims[2];
                boolDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                boolDataDims[1] = 1;
                hsize_t extendBoolDatasetTo[2];
                extendBoolDatasetTo[0] = this->getSize();
                extendBoolDatasetTo[1] = this->numBoolFields;
                this->boolDataset.extend( extendBoolDatasetTo );
                
                size_t currentSize = 0;
                for(unsigned long i = 0; i < numChunks; ++i)
                {
                    boolDataOffset[0] = currentSize;
                    boolDataOffset[1] = 0;
                    
                    DataSpace boolWriteDataSpace = this->boolDataset.getSpace();
                    boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                    DataSpace newBoolDataspace = DataSpace(2, boolDataDims);
                    
                    this->boolDataset.write(boolVals, PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                    
                    currentSize += ATT_WRITE_CHUNK_SIZE;
                    boolWriteDataSpace.close();
                    newBoolDataspace.close();
                }
                
                boolDataDims[0] = remainingRows;
                boolDataDims[1] = 1;
                
                boolDataOffset[0] = currentSize;
                boolDataOffset[1] = 0;
                
                DataSpace boolWriteDataSpace = this->boolDataset.getSpace();
                boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                DataSpace newBoolDataspace = DataSpace(2, boolDataDims);
                
                this->boolDataset.write(boolVals, PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                
                delete[] boolVals;
                boolFieldsDataSpace.close();
                boolFieldsDataset.close();
                boolFieldsWriteDataSpace.close();
                newBoolFieldsDataspace.close();
                boolDataSpace.close();
                boolWriteDataSpace.close();
                newBoolDataspace.close();
            }
            delete[] boolFields;
            delete fieldDtDisk;
            this->hasBoolFields = true;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    void RSGISAttributeTableHDF::addAttIntField(string name, long val) throw(RSGISAttributeTableException)
    {
        if(this->hasAttribute(name))
        {
            string message = string("The field \'") + name + string("\' already exists within the attribute table.");
            throw RSGISAttributeTableException(message);
        }
        
        try
        {
            this->flushAllFeatures();
            
            fieldIdx->insert(pair<string, unsigned int>(name, numIntFields));
            fieldDataType->insert(pair<string, RSGISAttributeDataType>(name, rsgis_int));
            fields->push_back(pair<string, RSGISAttributeDataType>(name, rsgis_int));
            
            CompType *fieldDtDisk = this->createAttibuteIdxCompTypeDisk();
            RSGISAttributeIdx *intFields = new RSGISAttributeIdx[1];
            intFields[0].name = const_cast<char*>(name.c_str());
            intFields[0].idx = this->numIntFields++;
            
            if(this->hasIntFields)
            {
                DataSet intFieldsDataset = attH5File->openDataSet( ATT_INT_FIELDS_HEADER );
                DataSpace intFieldsDataSpace = intFieldsDataset.getSpace();
                
                hsize_t extendIntFieldsDatasetTo[1];
                extendIntFieldsDatasetTo[0] = this->numIntFields;
                intFieldsDataset.extend( extendIntFieldsDatasetTo );
                
                hsize_t intFieldsOffset[1];
                intFieldsOffset[0] = this->numIntFields-1;
                hsize_t intFieldsDataDims[1];
                intFieldsDataDims[0] = 1;
                
                DataSpace intFieldsWriteDataSpace = intFieldsDataset.getSpace();
                intFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intFieldsDataDims, intFieldsOffset);
                DataSpace newIntFieldsDataspace = DataSpace(1, intFieldsDataDims);
                
                intFieldsDataset.write(intFields, *fieldDtDisk, newIntFieldsDataspace, intFieldsWriteDataSpace);
                
                hsize_t extendIntDatasetTo[2];
                extendIntDatasetTo[0] = this->getSize();
                extendIntDatasetTo[1] = this->numIntFields;
                this->intDataset.extend( extendIntDatasetTo );
                
                long *intVals = new long[ATT_WRITE_CHUNK_SIZE];
                for(unsigned int i  = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    intVals[i] = val;
                }
                
                unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
                unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
                
                hsize_t intDataOffset[2];
                intDataOffset[0] = 0;
                intDataOffset[1] = this->numIntFields-1;
                hsize_t intDataDims[2];
                intDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                intDataDims[1] = 1;
                
                size_t currentSize = 0;
                for(unsigned long i = 0; i < numChunks; ++i)
                {
                    intDataOffset[0] = currentSize;
                    intDataOffset[1] = this->numIntFields-1;
                    
                    DataSpace intWriteDataSpace = this->intDataset.getSpace();
                    intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                    DataSpace newIntDataspace = DataSpace(2, intDataDims);
                    
                    this->intDataset.write(intVals, PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                    
                    currentSize += ATT_WRITE_CHUNK_SIZE;
                    intWriteDataSpace.close();
                    newIntDataspace.close();
                }
                
                intDataDims[0] = remainingRows;
                intDataDims[1] = 1;
                
                intDataOffset[0] = currentSize;
                intDataOffset[1] = this->numIntFields-1;
                
                DataSpace intWriteDataSpace = this->intDataset.getSpace();
                intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                DataSpace newIntDataspace = DataSpace(2, intDataDims);
                
                this->intDataset.write(intVals, PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                
                delete[] intVals;
                intFieldsDataSpace.close();
                intFieldsDataset.close();
                intFieldsWriteDataSpace.close();
                newIntFieldsDataspace.close();
                intWriteDataSpace.close();
                newIntDataspace.close();
            }
            else
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
                DataSet intFieldsDataset = DataSet(attH5File->createDataSet(ATT_INT_FIELDS_HEADER, *fieldDtDisk, intFieldsDataSpace, creationIntFieldsDSPList));
                
                hsize_t extendIntFieldsDatasetTo[1];
                extendIntFieldsDatasetTo[0] = this->numIntFields;
                intFieldsDataset.extend( extendIntFieldsDatasetTo );
                
                hsize_t intFieldsOffset[1];
                intFieldsOffset[0] = 0;
                hsize_t intFieldsDataDims[1];
                intFieldsDataDims[0] = this->numIntFields;
                
                DataSpace intFieldsWriteDataSpace = intFieldsDataset.getSpace();
                intFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intFieldsDataDims, intFieldsOffset);
                DataSpace newIntFieldsDataspace = DataSpace(1, intFieldsDataDims);
                
                intFieldsDataset.write(intFields, *fieldDtDisk, newIntFieldsDataspace, intFieldsWriteDataSpace);
                
                hsize_t initDimsIntDS[2];
                initDimsIntDS[0] = this->getSize();
                initDimsIntDS[1] = this->numIntFields;
                hsize_t maxDimsIntDS[2];
                maxDimsIntDS[0] = H5S_UNLIMITED;
                maxDimsIntDS[1] = H5S_UNLIMITED;
                DataSpace intDataSpace = DataSpace(2, initDimsIntDS, maxDimsIntDS);
                
                hsize_t dimsIntChunk[2];
                dimsIntChunk[0] = ATT_WRITE_CHUNK_SIZE;
                dimsIntChunk[1] = 1;
                
                long fillValueLong = val;
                DSetCreatPropList creationIntDSPList;
                creationIntDSPList.setChunk(2, dimsIntChunk);
                creationIntDSPList.setShuffle();
                creationIntDSPList.setDeflate(ATT_WRITE_DEFLATE);
                creationIntDSPList.setFillValue( PredType::STD_I64LE, &fillValueLong);
                
                this->intDataset = DataSet(attH5File->createDataSet(ATT_INT_DATA, PredType::STD_I64LE, intDataSpace, creationIntDSPList));
                
                long *intVals = new long[ATT_WRITE_CHUNK_SIZE];
                for(unsigned int i  = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    intVals[i] = val;
                }
                
                unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
                unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
                
                hsize_t intDataOffset[2];
                intDataOffset[0] = 0;
                intDataOffset[1] = 0;
                hsize_t intDataDims[2];
                intDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                intDataDims[1] = 1;
                hsize_t extendIntDatasetTo[2];
                extendIntDatasetTo[0] = this->getSize();
                extendIntDatasetTo[1] = this->numIntFields;
                this->intDataset.extend( extendIntDatasetTo );

                
                size_t currentSize = 0;
                for(unsigned long i = 0; i < numChunks; ++i)
                {
                    intDataOffset[0] = currentSize;
                    intDataOffset[1] = 0;
                                        
                    DataSpace intWriteDataSpace = this->intDataset.getSpace();
                    intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                    DataSpace newIntDataspace = DataSpace(2, intDataDims);
                    
                    this->intDataset.write(intVals, PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                    
                    currentSize += ATT_WRITE_CHUNK_SIZE;
                    intWriteDataSpace.close();
                    newIntDataspace.close();
                }
                
                intDataDims[0] = remainingRows;
                intDataDims[1] = 1;
                
                intDataOffset[0] = currentSize;
                intDataOffset[1] = 0;
                
                DataSpace intWriteDataSpace = this->intDataset.getSpace();
                intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                DataSpace newIntDataspace = DataSpace(2, intDataDims);
                
                this->intDataset.write(intVals, PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                
                delete[] intVals;
                intFieldsDataSpace.close();
                intFieldsDataset.close();
                intFieldsWriteDataSpace.close();
                newIntFieldsDataspace.close();
                intDataSpace.close();
                intWriteDataSpace.close();
                newIntDataspace.close();
            }
            delete[] intFields;
            delete fieldDtDisk;
            this->hasIntFields = true;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    void RSGISAttributeTableHDF::addAttFloatField(string name, double val) throw(RSGISAttributeTableException)
    {
        if(this->hasAttribute(name))
        {
            string message = string("The field \'") + name + string("\' already exists within the attribute table.");
            throw RSGISAttributeTableException(message);
        }
        
        try
        {
            this->flushAllFeatures();
            
            fieldIdx->insert(pair<string, unsigned int>(name, numFloatFields));
            fieldDataType->insert(pair<string, RSGISAttributeDataType>(name, rsgis_float));
            fields->push_back(pair<string, RSGISAttributeDataType>(name, rsgis_float));
            
            CompType *fieldDtDisk = this->createAttibuteIdxCompTypeDisk();
            RSGISAttributeIdx *floatFields = new RSGISAttributeIdx[1];
            floatFields[0].name = const_cast<char*>(name.c_str());
            floatFields[0].idx = this->numFloatFields++;
            
            if(this->hasFloatFields)
            {
                DataSet floatFieldsDataset = attH5File->openDataSet( ATT_FLOAT_FIELDS_HEADER );
                DataSpace floatFieldsDataSpace = floatFieldsDataset.getSpace();
                
                hsize_t extendFloatFieldsDatasetTo[1];
                extendFloatFieldsDatasetTo[0] = this->numFloatFields;
                floatFieldsDataset.extend( extendFloatFieldsDatasetTo );
                
                hsize_t floatFieldsOffset[1];
                floatFieldsOffset[0] = this->numFloatFields-1;
                hsize_t floatFieldsDataDims[1];
                floatFieldsDataDims[0] = 1;
                
                DataSpace floatFieldsWriteDataSpace = floatFieldsDataset.getSpace();
                floatFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatFieldsDataDims, floatFieldsOffset);
                DataSpace newFloatFieldsDataspace = DataSpace(1, floatFieldsDataDims);
                
                floatFieldsDataset.write(floatFields, *fieldDtDisk, newFloatFieldsDataspace, floatFieldsWriteDataSpace);
                
                hsize_t extendFloatDatasetTo[2];
                extendFloatDatasetTo[0] = this->getSize();
                extendFloatDatasetTo[1] = this->numFloatFields;
                this->floatDataset.extend( extendFloatDatasetTo );
                
                double *floatVals = new double[ATT_WRITE_CHUNK_SIZE];
                for(unsigned int i  = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    floatVals[i] = val;
                }
                
                unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
                unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
                
                hsize_t floatDataOffset[2];
                floatDataOffset[0] = 0;
                floatDataOffset[1] = 0;
                hsize_t floatDataDims[2];
                floatDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                floatDataDims[1] = 1;
                
                size_t currentSize = 0;
                for(unsigned long i = 0; i < numChunks; ++i)
                {
                    floatDataOffset[0] = currentSize;
                    floatDataOffset[1] = this->numFloatFields-1;
                    
                    DataSpace floatWriteDataSpace = this->floatDataset.getSpace();
                    floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                    DataSpace newFloatDataspace = DataSpace(2, floatDataDims);
                    
                    this->floatDataset.write(floatVals, PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                    
                    currentSize += ATT_WRITE_CHUNK_SIZE;
                    floatWriteDataSpace.close();
                    newFloatDataspace.close();
                }
                
                floatDataDims[0] = remainingRows;
                floatDataDims[1] = 1;
                
                floatDataOffset[0] = currentSize;
                floatDataOffset[1] = this->numFloatFields-1;
                
                DataSpace floatWriteDataSpace = this->floatDataset.getSpace();
                floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                DataSpace newFloatDataspace = DataSpace(2, floatDataDims);
                
                this->floatDataset.write(floatVals, PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                
                delete[] floatVals;
                floatFieldsDataSpace.close();
                floatFieldsDataset.close();
                floatFieldsWriteDataSpace.close();
                newFloatFieldsDataspace.close();
                floatWriteDataSpace.close();
                newFloatDataspace.close();
            }
            else
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
                DataSet floatFieldsDataset = DataSet(attH5File->createDataSet(ATT_FLOAT_FIELDS_HEADER, *fieldDtDisk, floatFieldsDataSpace, creationFloatFieldsDSPList));
                
                hsize_t extendFloatFieldsDatasetTo[1];
                extendFloatFieldsDatasetTo[0] = this->numFloatFields;
                floatFieldsDataset.extend( extendFloatFieldsDatasetTo );
                
                hsize_t floatFieldsOffset[1];
                floatFieldsOffset[0] = 0;
                hsize_t floatFieldsDataDims[1];
                floatFieldsDataDims[0] = this->numFloatFields;
                
                DataSpace floatFieldsWriteDataSpace = floatFieldsDataset.getSpace();
                floatFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatFieldsDataDims, floatFieldsOffset);
                DataSpace newFloatFieldsDataspace = DataSpace(1, floatFieldsDataDims);
                
                floatFieldsDataset.write(floatFields, *fieldDtDisk, newFloatFieldsDataspace, floatFieldsWriteDataSpace);
                
                hsize_t initDimsFloatDS[2];
                initDimsFloatDS[0] = this->getSize();
                initDimsFloatDS[1] = this->numFloatFields;
                hsize_t maxDimsFloatDS[2];
                maxDimsFloatDS[0] = H5S_UNLIMITED;
                maxDimsFloatDS[1] = H5S_UNLIMITED;
                DataSpace floatDataSpace = DataSpace(2, initDimsFloatDS, maxDimsFloatDS);
                
                hsize_t dimsFloatChunk[2];
                dimsFloatChunk[0] = ATT_WRITE_CHUNK_SIZE;
                dimsFloatChunk[1] = 1;
                
                double fillValueDouble = val;
                DSetCreatPropList creationFloatDSPList;
                creationFloatDSPList.setChunk(2, dimsFloatChunk);
                creationFloatDSPList.setShuffle();
                creationFloatDSPList.setDeflate(ATT_WRITE_DEFLATE);
                creationFloatDSPList.setFillValue( PredType::IEEE_F64LE, &fillValueDouble);
                
                this->floatDataset = DataSet(attH5File->createDataSet(ATT_FLOAT_DATA, PredType::IEEE_F64LE, floatDataSpace, creationFloatDSPList));
                
                double *floatVals = new double[ATT_WRITE_CHUNK_SIZE];
                for(unsigned int i  = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    floatVals[i] = val;
                }
                
                unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
                unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
                
                hsize_t floatDataOffset[2];
                floatDataOffset[0] = 0;
                floatDataOffset[1] = 0;
                hsize_t floatDataDims[2];
                floatDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                floatDataDims[1] = 1;
                hsize_t extendFloatDatasetTo[2];
                extendFloatDatasetTo[0] = this->getSize();
                extendFloatDatasetTo[1] = this->numFloatFields;
                this->floatDataset.extend( extendFloatDatasetTo );
                
                size_t currentSize = 0;
                for(unsigned long i = 0; i < numChunks; ++i)
                {
                    floatDataOffset[0] = currentSize;
                    floatDataOffset[1] = 0;
                    
                    DataSpace floatWriteDataSpace = this->floatDataset.getSpace();
                    floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                    DataSpace newFloatDataspace = DataSpace(2, floatDataDims);
                    
                    this->floatDataset.write(floatVals, PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                    
                    currentSize += ATT_WRITE_CHUNK_SIZE;
                    floatWriteDataSpace.close();
                    newFloatDataspace.close();
                }
                
                floatDataDims[0] = remainingRows;
                floatDataDims[1] = 1;
                
                floatDataOffset[0] = currentSize;
                floatDataOffset[1] = 0;
                
                DataSpace floatWriteDataSpace = this->floatDataset.getSpace();
                floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                DataSpace newFloatDataspace = DataSpace(2, floatDataDims);
                
                this->floatDataset.write(floatVals, PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                
                delete[] floatVals;
                floatFieldsDataSpace.close();
                floatFieldsDataset.close();
                floatFieldsWriteDataSpace.close();
                newFloatFieldsDataspace.close();
                floatDataSpace.close();
                floatWriteDataSpace.close();
                newFloatDataspace.close();
            }
            delete[] floatFields;
            delete fieldDtDisk;
            this->hasFloatFields = true;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    void RSGISAttributeTableHDF::addAttStringField(string name, string val) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("String fields are not currently supported within HDF5 attribute tables.");
    }
        
    void RSGISAttributeTableHDF::addAttributes(vector<RSGISAttribute*> *attributes) throw(RSGISAttributeTableException)
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
        
    size_t RSGISAttributeTableHDF::getSize()
    {
        return attSize;
    }
    
    void RSGISAttributeTableHDF::holdFID(size_t fid)
    {
        map<size_t,size_t>::iterator iterBlock = heldBlocks->find(fid);
        if(iterBlock == heldBlocks->end())
        {
            size_t block = fid / ATT_WRITE_CHUNK_SIZE;
            //cout << "Adding hold: [" << fid << "," << block << "]\n";
            heldBlocks->insert(pair<size_t,size_t>(fid, block));
        }
        
    }
    
    void RSGISAttributeTableHDF::removeHoldFID(size_t fid)
    {
        //cout << "Removing hold " << fid << endl;
        map<size_t,size_t>::iterator iterBlock = heldBlocks->find(fid);
        if(iterBlock != heldBlocks->end())
        {
            heldBlocks->erase(iterBlock);
            //cout << "Removed\n";
        }
    }
    
    size_t RSGISAttributeTableHDF::getNumOfBlocks()
    {
        return (maxCacheSize/ATT_WRITE_CHUNK_SIZE)+1;
    }
    
    void RSGISAttributeTableHDF::loadBlocks(size_t startBlock, size_t endBlock) throw(RSGISAttributeTableException)
    {
        try
        {
            for(size_t i = startBlock; i < endBlock; ++i)
            {
                if(i < numOfBlocks)
                {
                    this->loadBlock(i);
                }
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    bool RSGISAttributeTableHDF::attInMemory()
    {
        return false;
    }
    
    void RSGISAttributeTableHDF::findFIDRangeInNeighbours(size_t startFID, size_t endFID, size_t *minFID, size_t *maxFID) throw(RSGISAttributeTableException)
    {
        try
        {
            if(endFID > this->getSize())
            {
                throw RSGISAttributeTableException("Features are not all within the attribute table.");
            }
            bool first = true;
            RSGISFeature *feat;
            for(size_t i = startFID; i < endFID; ++i)
            {
                feat = this->getFeature(i);
                for(vector<size_t>::iterator iterNeighbours = feat->neighbours->begin(); iterNeighbours != feat->neighbours->end(); ++iterNeighbours)
                {
                    if(first)
                    {
                        *minFID = *iterNeighbours;
                        *maxFID = *iterNeighbours;
                        first = false;
                    }
                    else if((*iterNeighbours) < *minFID)
                    {
                        *minFID = *iterNeighbours;
                    }
                    else if((*iterNeighbours) > *maxFID)
                    {
                        *maxFID = *iterNeighbours;
                    }
                }
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
        
    void RSGISAttributeTableHDF::operator++()
    {
        ++iterIdx;
    }
        
    void RSGISAttributeTableHDF::start()
    {
        iterIdx = 0;
    }
    
    bool RSGISAttributeTableHDF::end()
    {
        return iterIdx < attSize;
    }
    
    RSGISFeature* RSGISAttributeTableHDF::operator*()
    {
        return this->getFeature(iterIdx);
    }
    
    void RSGISAttributeTableHDF::populateCache(size_t fid) throw(RSGISAttributeTableException)
    {
        try
        {
            if(fid > this->attSize)
            {
                cout << "fid = " << fid << endl;
                throw RSGISAttributeTableException("The requested block is not within this file.");
            }
            
            size_t block = fid / ATT_WRITE_CHUNK_SIZE;
            size_t startFID = 0;
            size_t endFID = 0;
            size_t blockSize = ATT_WRITE_CHUNK_SIZE;
            if(block > numOfBlocks)
            {
                throw RSGISAttributeTableException("The specificed block is not within the file");
            }
            else if(block < numOfBlocks)
            {
                startFID = block * ATT_WRITE_CHUNK_SIZE;
                endFID = startFID + ATT_WRITE_CHUNK_SIZE;
            }
            else
            {
                startFID = block * ATT_WRITE_CHUNK_SIZE;
                endFID = this->attSize;
                blockSize = endFID-startFID;
            }
            
            //////////////////////////////////////////////////////////////////
            //
            // CHECK WHETHER NEED TO REMOVE DATA FROM THE CACHE!
            //
            //////////////////////////////////////////////////////////////////
            //cout << "cacheQ->size() = " << cacheQ->size() << endl;
            //cout << "this->maxNumOfBlockInCache = " << this->maxNumOfBlockInCache << endl;
            if(cacheQ->size() > this->maxNumOfBlockInCache)
            {
                size_t tmpBlock = cacheQ->back();
                bool block2BHeld = false;
                for(map<size_t,size_t>::iterator iterPairs = heldBlocks->begin(); iterPairs != heldBlocks->end(); ++iterPairs)
                {
                    if((*iterPairs).second == tmpBlock)
                    {
                        block2BHeld = true;
                        break;
                    }
                }
                
                if(block2BHeld)
                {
                    // Could try and remove another block but for the moment just skip removal and see if memory becomes an issue...
                }
                else
                {
                    cacheQ->pop_back();
                    this->removeFromCache(tmpBlock);
                }
            }
            //////////////////////////////////////////////////////////////////
            
            
            this->loadBlock(block);
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    void RSGISAttributeTableHDF::loadBlock(size_t block) throw(RSGISAttributeTableException)
    {
        //cout << "Loading block " << block << endl;
        try
        {            
            size_t startFID = 0;
            size_t endFID = 0;
            size_t blockSize = ATT_WRITE_CHUNK_SIZE;
            if(block > numOfBlocks)
            {
                throw RSGISAttributeTableException("The specificed block is not within the file");
            }
            else if(block < numOfBlocks)
            {
                startFID = block * ATT_WRITE_CHUNK_SIZE;
                endFID = startFID + ATT_WRITE_CHUNK_SIZE;
            }
            else
            {
                startFID = block * ATT_WRITE_CHUNK_SIZE;
                endFID = this->attSize;
                blockSize = endFID-startFID;
            }
            
            /* Neighbours */
    
            DataSpace neighboursDataspace = neighboursDataset.getSpace();
            hsize_t neighboursOffset[1];
			neighboursOffset[0] = startFID;
			hsize_t neighboursCount[1];
			neighboursCount[0] = blockSize;
			neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
			
			hsize_t neighboursDimsRead[1]; 
			neighboursDimsRead[0] = blockSize;
			DataSpace neighboursMemspace( 1, neighboursDimsRead );
			
			hsize_t neighboursOffset_out[1];
            neighboursOffset_out[0] = 0;
			hsize_t neighboursCount_out[1];
			neighboursCount_out[0] = blockSize;
			neighboursMemspace.selectHyperslab( H5S_SELECT_SET, neighboursCount_out, neighboursOffset_out );
            
            hvl_t *neighbourVals = new hvl_t[blockSize];
            DataType intVarLenMemDT = VarLenType(&PredType::NATIVE_UINT64);
            neighboursDataset.read(neighbourVals, intVarLenMemDT, neighboursMemspace, neighboursDataspace);
            
            neighboursDataspace.close();
            neighboursMemspace.close();
            
            int *boolVals = NULL;
            if(this->numBoolFields > 0)
            {
                boolVals = new int[blockSize*this->numBoolFields];
                
                DataSpace boolDataspace = boolDataset.getSpace();
                hsize_t boolFieldsOffset[2];
                boolFieldsOffset[0] = startFID;
                boolFieldsOffset[1] = 0;
                hsize_t boolFieldsCount[2];
                boolFieldsCount[0] = blockSize;
                boolFieldsCount[1] = this->numBoolFields;
                boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
                
                hsize_t boolFieldsDimsRead[2]; 
                boolFieldsDimsRead[0] = blockSize;
                boolFieldsDimsRead[1] = this->numBoolFields;
                DataSpace boolFieldsMemspace( 2, boolFieldsDimsRead );
                
                hsize_t boolFieldsOffset_out[2];
                boolFieldsOffset_out[0] = 0;
                boolFieldsOffset_out[1] = 0;
                hsize_t boolFieldsCount_out[2];
                boolFieldsCount_out[0] = blockSize;
                boolFieldsCount_out[1] = this->numBoolFields;
                boolFieldsMemspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount_out, boolFieldsOffset_out );
                
                boolDataset.read(boolVals, PredType::NATIVE_INT32, boolFieldsMemspace, boolDataspace);
                
                boolDataspace.close();
                boolFieldsMemspace.close();
            }
            
            long *intVals = NULL;
            if(this->numIntFields > 0)
            {
                intVals = new long[blockSize*this->numIntFields];
                
                DataSpace intDataspace = intDataset.getSpace();
                hsize_t intFieldsOffset[2];
                intFieldsOffset[0] = startFID;
                intFieldsOffset[1] = 0;
                hsize_t intFieldsCount[2];
                intFieldsCount[0] = blockSize;
                intFieldsCount[1] = this->numIntFields;
                intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );
                
                hsize_t intFieldsDimsRead[2]; 
                intFieldsDimsRead[0] = blockSize;
                intFieldsDimsRead[1] = this->numIntFields;
                DataSpace intFieldsMemspace( 2, intFieldsDimsRead );
                
                hsize_t intFieldsOffset_out[2];
                intFieldsOffset_out[0] = 0;
                intFieldsOffset_out[1] = 0;
                hsize_t intFieldsCount_out[2];
                intFieldsCount_out[0] = blockSize;
                intFieldsCount_out[1] = this->numIntFields;
                intFieldsMemspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount_out, intFieldsOffset_out );
                
                intDataset.read(intVals, PredType::NATIVE_INT64, intFieldsMemspace, intDataspace);
                
                intDataspace.close();
                intFieldsMemspace.close();
            }
            
            double *floatVals = NULL;
            if(this->numFloatFields > 0)
            {
                floatVals = new double[blockSize*this->numFloatFields];
                
                DataSpace floatDataspace = floatDataset.getSpace();
                hsize_t floatFieldsOffset[2];
                floatFieldsOffset[0] = startFID;
                floatFieldsOffset[1] = 0;
                hsize_t floatFieldsCount[2];
                floatFieldsCount[0] = blockSize;
                floatFieldsCount[1] = this->numFloatFields;
                floatDataspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount, floatFieldsOffset );
                
                hsize_t floatFieldsDimsRead[2]; 
                floatFieldsDimsRead[0] = blockSize;
                floatFieldsDimsRead[1] = this->numFloatFields;
                DataSpace floatFieldsMemspace( 2, floatFieldsDimsRead );
                
                hsize_t floatFieldsOffset_out[2];
                floatFieldsOffset_out[0] = 0;
                floatFieldsOffset_out[1] = 0;
                hsize_t floatFieldsCount_out[2];
                floatFieldsCount_out[0] = blockSize;
                floatFieldsCount_out[1] = this->numFloatFields;
                floatFieldsMemspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount_out, floatFieldsOffset_out );
                
                floatDataset.read(floatVals, PredType::NATIVE_DOUBLE, floatFieldsMemspace, floatDataspace);
                
                floatDataspace.close();
                floatFieldsMemspace.close();
            }
            
            for(size_t i = startFID, n = 0; i < endFID; ++i, ++n)
            {
                this->featCache[i] = new RSGISFeature();
                this->featCache[i]->fid = i;
                this->featCache[i]->boolFields = new vector<bool>();
                if(this->numBoolFields > 0)
                {
                    this->featCache[i]->boolFields->reserve(this->numBoolFields);
                    for(size_t j = 0; j < this->numBoolFields; ++j)
                    {
                        this->featCache[i]->boolFields->push_back(boolVals[(n*this->numBoolFields)+j]);
                    }
                }
                this->featCache[i]->intFields = new vector<long>();
                if(this->numIntFields > 0)
                {
                    this->featCache[i]->intFields->reserve(this->numIntFields);
                    for(size_t j = 0; j < this->numIntFields; ++j)
                    {
                        this->featCache[i]->intFields->push_back(intVals[(n*this->numIntFields)+j]);
                    }
                }
                this->featCache[i]->floatFields = new vector<double>();
                if(this->numFloatFields > 0)
                {
                    this->featCache[i]->floatFields->reserve(this->numFloatFields);
                    for(size_t j = 0; j < this->numFloatFields; ++j)
                    {
                        this->featCache[i]->floatFields->push_back(floatVals[(n*this->numFloatFields)+j]);
                    }
                }
                this->featCache[i]->stringFields = new vector<string>();
                this->featCache[i]->neighbours = new vector<size_t>();
                if(neighbourVals[n].length > 0)
                {
                    this->featCache[i]->neighbours->reserve(neighbourVals[n].length);
                    //cout << "neighbourVals[" << n << "].length " << neighbourVals[n].length << endl;
                    for(size_t j = 0; j < neighbourVals[n].length; ++j)
                    {
                        this->featCache[i]->neighbours->push_back(((unsigned long long*)neighbourVals[n].p)[j]);
                    }
                }
            }
            
            delete[] neighbourVals;
            if(this->numBoolFields > 0)
            {
                delete[] boolVals;
            }
            if(this->numIntFields > 0)
            {
                delete[] intVals;
            }
            if(this->numFloatFields > 0)
            {
                delete[] floatVals;
            }
            
            this->cacheQ->push_front(block);
            //cout << "Cache has " << this->cacheQ->size() << " blocks loaded\n";
            ++numOfReads;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            cout << e.getCDetailMsg() << endl;
            cout << e.getCFuncName() << endl;
            cout << e.getDetailMsg() << endl;
            cout << e.getFuncName() << endl;
            e.printError();
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    void RSGISAttributeTableHDF::removeFromCache(size_t block) throw(RSGISAttributeTableException)
    {
        //cout << "Writing block " << block << endl;
        try
        {
            size_t startFID = 0;
            size_t endFID = 0;
            size_t blockSize = ATT_WRITE_CHUNK_SIZE;
            if(block > numOfBlocks)
            {
                throw RSGISAttributeTableException("The specificed block is not within the file");
            }
            else if(block < numOfBlocks)
            {
                startFID = block * ATT_WRITE_CHUNK_SIZE;
                endFID = startFID + ATT_WRITE_CHUNK_SIZE;
            }
            else
            {
                startFID = block * ATT_WRITE_CHUNK_SIZE;
                endFID = this->attSize;
                blockSize = endFID-startFID;
            }
            
            if(!this->readOnly)
            {
                if(this->numBoolFields > 0)
                {
                    int *boolVals = new int[blockSize*this->numBoolFields];
                    
                    for(size_t i = startFID, n = 0; i < endFID; ++i, ++n)
                    {
                        for(unsigned int k = 0; k < this->featCache[i]->boolFields->size(); ++k)
                        {
                            boolVals[(n*this->numBoolFields)+k] = this->featCache[i]->boolFields->at(k);
                        }
                    }
                    
                    hsize_t boolDataOffset[2];
                    boolDataOffset[0] = startFID;
                    boolDataOffset[1] = 0;
                    hsize_t boolDataDims[2];
                    boolDataDims[0] = blockSize;
                    boolDataDims[1] = this->numBoolFields;
                    
                    DataSpace boolWriteDataSpace = boolDataset.getSpace();
                    boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                    DataSpace newBoolDataspace = DataSpace(2, boolDataDims);
                    boolDataset.write(boolVals, PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                    
                    delete[] boolVals;
                    boolWriteDataSpace.close();
                    newBoolDataspace.close();
                }
                
                if(this->numIntFields > 0)
                {
                    long *intVals = new long[blockSize*this->numIntFields];
                    
                    for(size_t i = startFID, n = 0; i < endFID; ++i, ++n)
                    {
                        for(unsigned int k = 0; k < this->featCache[i]->intFields->size(); ++k)
                        {
                            intVals[(n*this->numIntFields)+k] = this->featCache[i]->intFields->at(k);
                        }
                    }
                    
                    hsize_t intDataOffset[2];
                    intDataOffset[0] = startFID;
                    intDataOffset[1] = 0;
                    hsize_t intDataDims[2];
                    intDataDims[0] = blockSize;
                    intDataDims[1] = this->numIntFields;
                    
                    DataSpace intWriteDataSpace = intDataset.getSpace();
                    intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                    DataSpace newIntDataspace = DataSpace(2, intDataDims);
                    intDataset.write(intVals, PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                    
                    delete[] intVals;
                    intWriteDataSpace.close();
                    newIntDataspace.close();
                }
                
                if(this->numFloatFields > 0)
                {
                    double *floatVals = new double[blockSize*this->numFloatFields];
                    
                    for(size_t i = startFID, n = 0; i < endFID; ++i, ++n)
                    {
                        for(unsigned int k = 0; k < this->featCache[i]->floatFields->size(); ++k)
                        {
                            floatVals[(n*this->numFloatFields)+k] = this->featCache[i]->floatFields->at(k);
                        }
                    }
                    
                    hsize_t floatDataOffset[2];
                    floatDataOffset[0] = startFID;
                    floatDataOffset[1] = 0;
                    hsize_t floatDataDims[2];
                    floatDataDims[0] = blockSize;
                    floatDataDims[1] = this->numFloatFields;
                    
                    DataSpace floatWriteDataSpace = floatDataset.getSpace();
                    floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                    DataSpace newFloatDataspace = DataSpace(2, floatDataDims);
                    floatDataset.write(floatVals, PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                    
                    delete[] floatVals;
                    floatWriteDataSpace.close();
                    newFloatDataspace.close();
                }
                
                hvl_t *neighbourVals = new hvl_t[blockSize];
                
                for(size_t i = startFID, n = 0; i < endFID; ++i, ++n)
                {
                    neighbourVals[n].length = this->featCache[i]->neighbours->size();
                    if(this->featCache[i]->neighbours->size() > 0)
                    {
                        neighbourVals[n].p = new size_t[neighbourVals[n].length];
                        for(size_t m = 0; m < neighbourVals[n].length; ++m)
                        {
                            ((size_t*)neighbourVals[n].p)[m] = this->featCache[i]->neighbours->at(m);
                        }
                    }
                    else
                    {
                        neighbourVals[n].p = NULL;
                    }
                }
                
                hsize_t extendNeighboursDatasetTo[1];
                extendNeighboursDatasetTo[0] = this->attSize;
                neighboursDataset.extend( extendNeighboursDatasetTo );
                
                hsize_t neighboursDataOffset[1];
                neighboursDataOffset[0] = startFID;
                hsize_t neighboursDataDims[1];
                neighboursDataDims[0] = blockSize;
                
                DataType intVarLenMemDT = VarLenType(&PredType::NATIVE_UINT64);
                DataSpace neighboursWriteDataSpace = neighboursDataset.getSpace();
                neighboursWriteDataSpace.selectHyperslab(H5S_SELECT_SET, neighboursDataDims, neighboursDataOffset);
                DataSpace newNeighboursDataspace = DataSpace(1, neighboursDataDims);
                neighboursDataset.write(neighbourVals, intVarLenMemDT, newNeighboursDataspace, neighboursWriteDataSpace);
                
                neighboursWriteDataSpace.close();
                newNeighboursDataspace.close();
                
                for(size_t i = startFID, n = 0; i < endFID; ++i, ++n)
                {
                    if(neighbourVals[n].length > 0)
                    {
                        delete[] ((size_t*)neighbourVals[n].p);
                    }
                }
                delete[] neighbourVals;
                
                ++numOfWrites;
            }
            
            for(size_t i = startFID; i < endFID; ++i)
            {
                this->freeFeature(this->featCache[i]);
                this->featCache[i] = NULL;
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    RSGISAttributeTable* RSGISAttributeTableHDF::importFromHDF5(string inFile, bool readOnly, size_t maxCacheSize)throw(RSGISAttributeTableException)
    {
        RSGISAttributeTableHDF *attTableObj = new RSGISAttributeTableHDF();
        try
        {
            Exception::dontPrint();
            
            FileAccPropList attAccessPlist = FileAccPropList(FileAccPropList::DEFAULT);
            attAccessPlist.setCache(ATT_READ_MDC_NELMTS, ATT_READ_RDCC_NELMTS, ATT_READ_RDCC_NBYTES, ATT_READ_RDCC_W0);
            attAccessPlist.setSieveBufSize(ATT_READ_SIEVE_BUF);
            hsize_t metaBlockSize = ATT_READ_META_BLOCKSIZE;
            attAccessPlist.setMetaBlockSize(metaBlockSize);
            
            attTableObj->attH5File = new H5File( inFile, H5F_ACC_RDWR, FileCreatPropList::DEFAULT, attAccessPlist);
            
            attTableObj->hasBoolFields = false;
            attTableObj->hasIntFields = false;
            attTableObj->hasFloatFields = false;
            
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
			DataSpace attSizeDataSpace(1, dimsAttSize);
            attTableObj->sizeDataset = attTableObj->attH5File->openDataSet( ATT_SIZE_HEADER );
            attTableObj->sizeDataset.read(&attTableObj->attSize, PredType::NATIVE_ULLONG, attSizeDataSpace);
            attSizeDataSpace.close();
            
            cout << "Table has " << attTableObj->attSize << " features\n";
            
            
            CompType *fieldCompTypeMem = attTableObj->createAttibuteIdxCompTypeMem();
            try
            {
                DataSet boolFieldsDataset = attTableObj->attH5File->openDataSet( ATT_BOOL_FIELDS_HEADER );
                DataSpace boolFieldsDataspace = boolFieldsDataset.getSpace();
                
                attTableObj->numBoolFields = boolFieldsDataspace.getSelectNpoints();
                
                cout << "There are " << attTableObj->numBoolFields << " boolean fields." << endl;
                
                if(attTableObj->numBoolFields > 0)
                {
                    attTableObj->hasBoolFields = true;
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
            }
            catch( Exception &e )
            {
                attTableObj->hasBoolFields = false;
            }
            
            try
            {
                DataSet intFieldsDataset = attTableObj->attH5File->openDataSet( ATT_INT_FIELDS_HEADER );
                DataSpace intFieldsDataspace = intFieldsDataset.getSpace();
                
                attTableObj->numIntFields = intFieldsDataspace.getSelectNpoints();
                
                cout << "There are " << attTableObj->numIntFields << " integer fields." << endl;
                
                if(attTableObj->numIntFields > 0)
                {
                    attTableObj->hasIntFields = true;
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
            }
            catch( Exception &e )
            {
                attTableObj->hasIntFields = false;
            }
            
            try
            {
                DataSet floatFieldsDataset = attTableObj->attH5File->openDataSet( ATT_FLOAT_FIELDS_HEADER );
                DataSpace floatFieldsDataspace = floatFieldsDataset.getSpace();
                
                attTableObj->numFloatFields = floatFieldsDataspace.getSelectNpoints();
                
                cout << "There are " << attTableObj->numFloatFields << " float fields." << endl;
                
                if(attTableObj->numFloatFields > 0)
                {
                    attTableObj->hasFloatFields = true;
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
            }
            catch( Exception &e )
            {
                attTableObj->hasFloatFields = false;
            }
            
            delete fieldCompTypeMem;
            
            attTableObj->neighboursDataset = attTableObj->attH5File->openDataSet( ATT_NEIGHBOURS_DATA );
            DataSpace neighboursDataspace = attTableObj->neighboursDataset.getSpace();
            
            int neighboursNDims = neighboursDataspace.getSimpleExtentNdims();
            if(neighboursNDims != 1)
            {
                throw RSGISAttributeTableException("The neighbours datasets needs to have 1 dimension.");
            }
            
            hsize_t *neighboursDims = new hsize_t[neighboursNDims];
            neighboursDataspace.getSimpleExtentDims(neighboursDims);
            
            if(neighboursDims[0] != attTableObj->attSize)
            {
                throw RSGISAttributeTableException("The number of features in neighbours datasets does not match expected values.");
            }
            
            delete[] neighboursDims;
            
            if(attTableObj->hasBoolFields)
            {
                attTableObj->boolDataset = attTableObj->attH5File->openDataSet( ATT_BOOL_DATA );
                DataSpace boolDataspace = attTableObj->boolDataset.getSpace();
                
                int boolNDims = boolDataspace.getSimpleExtentNdims();
                
                if(boolNDims != 2)
                {
                    throw RSGISAttributeTableException("The boolean datasets needs to have 2 dimensions.");
                }
                
                hsize_t *boolDims = new hsize_t[boolNDims];
                boolDataspace.getSimpleExtentDims(boolDims);
                
                if(boolDims[0] != attTableObj->attSize)
                {
                    throw RSGISAttributeTableException("The number of features in boolean datasets does not match expected values.");
                }
                
                if(boolDims[1] != attTableObj->numBoolFields)
                {
                    throw RSGISAttributeTableException("The number of boolean fields does not match expected values.");
                }
                delete[] boolDims;                
            }
            
            if(attTableObj->hasIntFields)
            {
                attTableObj->intDataset = attTableObj->attH5File->openDataSet( ATT_INT_DATA );
                DataSpace intDataspace = attTableObj->intDataset.getSpace();
                
                int intNDims = intDataspace.getSimpleExtentNdims();
                
                if(intNDims != 2)
                {
                    throw RSGISAttributeTableException("The integer datasets needs to have 2 dimensions.");
                }
                
                hsize_t *intDims = new hsize_t[intNDims];
                intDataspace.getSimpleExtentDims(intDims);
                
                if(intDims[0] != attTableObj->attSize)
                {
                    throw RSGISAttributeTableException("The number of features in integer datasets does not match expected values.");
                }
                
                if(intDims[1] != attTableObj->numIntFields)
                {
                    throw RSGISAttributeTableException("The number of integer fields does not match expected values.");
                }
                delete[] intDims;
            }
            
            if(attTableObj->hasFloatFields)
            {
                attTableObj->floatDataset = attTableObj->attH5File->openDataSet( ATT_FLOAT_DATA );
                DataSpace floatDataspace = attTableObj->floatDataset.getSpace();
                
                int floatNDims = floatDataspace.getSimpleExtentNdims();
                
                if(floatNDims != 2)
                {
                    throw RSGISAttributeTableException("The float datasets needs to have 2 dimensions.");
                }
                
                hsize_t *floatDims = new hsize_t[floatNDims];
                floatDataspace.getSimpleExtentDims(floatDims);
                
                if(floatDims[0] != attTableObj->attSize)
                {
                    throw RSGISAttributeTableException("The number of features in float datasets does not match expected values.");
                }
                
                if(floatDims[1] != attTableObj->numFloatFields)
                {
                    throw RSGISAttributeTableException("The number of float fields does not match expected values.");
                }
                
                delete[] floatDims;
            }

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
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
                
        attTableObj->attOpen = true;
        attTableObj->featCache = new RSGISFeature*[attTableObj->attSize];
        attTableObj->numOfBlocks = attTableObj->attSize / ATT_WRITE_CHUNK_SIZE;
        attTableObj->remainingFeatures = attTableObj->attSize - (attTableObj->numOfBlocks * ATT_WRITE_CHUNK_SIZE);
        attTableObj->readOnly = readOnly;
        attTableObj->maxCacheSize = maxCacheSize;
        attTableObj->maxNumOfBlockInCache = (attTableObj->maxCacheSize / ATT_WRITE_CHUNK_SIZE)+1;
        
        return attTableObj;
    }
        
    RSGISAttributeTableHDF::~RSGISAttributeTableHDF()
    {
        this->flushAllFeatures();
        delete cacheQ;
        delete[] featCache;
        
        delete this->heldBlocks;
        
        this->attH5File->flush(H5F_SCOPE_GLOBAL);
        this->attH5File->close();
        delete this->attH5File;
        
        cout << "There have been " << numOfReads << " reads\n";
        cout << "There have been " << numOfWrites << " writes\n";
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
  /*  
    
    
    RSGISAttributeTableHDFSmallCache::RSGISAttributeTableHDFSmallCache() : RSGISAttributeTable()
    {
        attOpen = false;
        attSize = 0;
        readOnly = true;
        maxCacheSize = 0;
        numOfReads = 0;
        numOfWrites = 0;
        featCache = new  map<size_t, RSGISFeature*>();
        cacheQ = new list<size_t>();
    }
    
    RSGISAttributeTableHDFSmallCache::RSGISAttributeTableHDFSmallCache(size_t numFeatures, string filePath, bool readOnly, size_t maxCacheSize) throw(RSGISAttributeTableException) : RSGISAttributeTable()
    {        
        try
        {
            this->readOnly = readOnly;
            this->maxCacheSize = maxCacheSize;
            this->createAttributeTable(numFeatures, filePath);
            featCache = new  map<size_t, RSGISFeature*>();
            cacheQ = new list<size_t>();
            numOfReads = 0;
            numOfWrites = 0;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    RSGISAttributeTableHDFSmallCache::RSGISAttributeTableHDFSmallCache(size_t numFeatures, vector<pair<string, RSGISAttributeDataType> > *fields, string filePath, bool readOnly, size_t maxCacheSize) throw(RSGISAttributeTableException) : RSGISAttributeTable()
    {
        this->readOnly = readOnly;
        this->maxCacheSize = maxCacheSize;
        attOpen = false;
        attSize = 0;
        featCache = new  map<size_t, RSGISFeature*>();
        cacheQ = new list<size_t>();
        numOfReads = 0;
        numOfWrites = 0;
    }
    
    void RSGISAttributeTableHDFSmallCache::createAttributeTable(size_t numFeatures, string filePath) throw(RSGISAttributeTableException)
    {
        try
        {
            this->fields = new vector<pair<string, RSGISAttributeDataType> >();
            this->fieldIdx = new map<string, unsigned int>();
            this->fieldDataType = new map<string, RSGISAttributeDataType>();
            
            this->numBoolFields = 0;
            this->numIntFields = 0;
            this->numFloatFields = 0;
            this->numStrFields = 0;
            
            Exception::dontPrint();
            
            FileAccPropList attAccessPlist = FileAccPropList(FileAccPropList::DEFAULT);
            attAccessPlist.setCache(ATT_WRITE_MDC_NELMTS, ATT_WRITE_RDCC_NELMTS, ATT_WRITE_RDCC_NBYTES, ATT_WRITE_RDCC_W0);
            attAccessPlist.setSieveBufSize(ATT_WRITE_SIEVE_BUF);
            hsize_t metaBlockSize = ATT_WRITE_META_BLOCKSIZE;
            attAccessPlist.setMetaBlockSize(metaBlockSize);
            
            const H5std_string attFilePath( filePath );
            this->attH5File = new H5File( attFilePath, H5F_ACC_TRUNC, FileCreatPropList::DEFAULT, attAccessPlist);
            
            attH5File->createGroup( ATT_GROUPNAME_HEADER );
			attH5File->createGroup( ATT_GROUPNAME_DATA );
            attH5File->createGroup( ATT_GROUPNAME_NEIGHBOURS );
            
            // Create Neighbours dataset
            this->neighboursLineLength = 25;
            hsize_t initDimsNeighboursDS[2];
            initDimsNeighboursDS[0] = numFeatures;
            initDimsNeighboursDS[1] = this->neighboursLineLength;
            hsize_t maxDimsNeighboursDS[2];
            maxDimsNeighboursDS[0] = H5S_UNLIMITED;
            maxDimsNeighboursDS[1] = H5S_UNLIMITED;
            DataSpace neighboursDataspace = DataSpace(2, initDimsNeighboursDS, maxDimsNeighboursDS);
            
            hsize_t dimsNeighboursChunk[2];
            dimsNeighboursChunk[0] = ATT_WRITE_CHUNK_SIZE;
            dimsNeighboursChunk[1] = ATT_WRITE_CHUNK_SIZE;
            
            unsigned long fillValueULong = 0;
            DSetCreatPropList creationNeighboursDSPList;
            creationNeighboursDSPList.setChunk(2, dimsNeighboursChunk);
            creationNeighboursDSPList.setShuffle();
            creationNeighboursDSPList.setDeflate(ATT_WRITE_DEFLATE);
            creationNeighboursDSPList.setFillValue( PredType::STD_U64LE, &fillValueULong);
            
            this->neighboursDataset = DataSet(attH5File->createDataSet(ATT_NEIGHBOURS_DATA, PredType::STD_U64LE, neighboursDataspace, creationNeighboursDSPList));
            neighboursDataspace.close();
            
            // Create Number of Neighbours dataset
            hsize_t initDimsNumNeighboursDS[1];
            initDimsNumNeighboursDS[0] = numFeatures;
            hsize_t maxDimsNumNeighboursDS[1];
            maxDimsNumNeighboursDS[0] = H5S_UNLIMITED;
            DataSpace numNeighboursDataspace = DataSpace(1, initDimsNumNeighboursDS, maxDimsNumNeighboursDS);
            
            hsize_t dimsNumNeighboursChunk[1];
            dimsNumNeighboursChunk[0] = ATT_WRITE_CHUNK_SIZE;
            
            unsigned int fillValueUInt = 0;
            DSetCreatPropList creationNumNeighboursDSPList;
            creationNumNeighboursDSPList.setChunk(1, dimsNumNeighboursChunk);
            creationNumNeighboursDSPList.setShuffle();
            creationNumNeighboursDSPList.setDeflate(ATT_WRITE_DEFLATE);
            creationNumNeighboursDSPList.setFillValue( PredType::STD_U32LE, &fillValueUInt);
            
            this->numNeighboursDataset = DataSet(attH5File->createDataSet(ATT_NUM_NEIGHBOURS_DATA, PredType::STD_U32LE, numNeighboursDataspace, creationNumNeighboursDSPList));
            numNeighboursDataspace.close();
            
            hasBoolFields = false;
            hasIntFields = false;
            hasFloatFields = false;
            neighboursLineLength = 0;
            iterIdx = 0;
            attSize = numFeatures;
            attOpen = true;            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    
    bool RSGISAttributeTableHDFSmallCache::getBoolField(size_t fid, string name) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("getBoolField not implemented.");
        return false;
    }
    
    long RSGISAttributeTableHDFSmallCache::getIntField(size_t fid, string name) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("getIntField not implemented.");
        return 0;
    }
    
    double RSGISAttributeTableHDFSmallCache::getDoubleField(size_t fid, string name) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("getDoubleField not implemented.");
        return 0.0;
    }
    
    string RSGISAttributeTableHDFSmallCache::getStringField(size_t fid, string name) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("String fields are not currently supported within HDF5 attribute tables.");
        return "";
    }
    
    
    void RSGISAttributeTableHDFSmallCache::setBoolField(size_t fid, string name, bool value) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("setBoolField not implemented.");
    }
    
    void RSGISAttributeTableHDFSmallCache::setIntField(size_t fid, string name, long value) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("setIntField not implemented.");
    }
    
    void RSGISAttributeTableHDFSmallCache::setDoubleField(size_t fid, string name, double value) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("setDoubleField not implemented.");
    }
    
    void RSGISAttributeTableHDFSmallCache::setStringField(size_t fid, string name, string value) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("String fields are not currently supported within HDF5 attribute tables.");
    }
    
    void RSGISAttributeTableHDFSmallCache::setBoolValue(string name, bool value) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("setBoolValue not implemented.");
    }
    
    void RSGISAttributeTableHDFSmallCache::setIntValue(string name, long value) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("setIntValue not implemented.");
    }
    
    void RSGISAttributeTableHDFSmallCache::setFloatValue(string name, double value) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("setFloatValue not implemented.");
    }
    
    void RSGISAttributeTableHDFSmallCache::setStringValue(string name, string value) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("String fields are not currently supported within HDF5 attribute tables.");
    }
    
    RSGISFeature* RSGISAttributeTableHDFSmallCache::getFeature(size_t fid) throw(RSGISAttributeTableException)
    {
        //cout << "Retrieving feature = " << fid << endl;
        RSGISFeature *feat = NULL;
        try
        {
            map<size_t, RSGISFeature*>::iterator iterFeat;
            iterFeat = featCache->find(fid);
            if(iterFeat == featCache->end())
            {
                feat = new RSGISFeature();
                feat->fid = fid;
                feat->boolFields = new vector<bool>();
                feat->intFields = new vector<long>();
                feat->floatFields = new vector<double>();
                feat->stringFields = new vector<string>();
                feat->neighbours = new vector<size_t>();
                
                unsigned int numNeighboursVal = 0;
                
                // Number of Neighbours
                DataSpace numNeighboursDataspace = this->numNeighboursDataset.getSpace();
                hsize_t numNeighboursOffset[1];
                numNeighboursOffset[0] = fid;
                hsize_t numNeighboursCount[1];
                numNeighboursCount[0]  = 1;
                numNeighboursDataspace.selectHyperslab( H5S_SELECT_SET, numNeighboursCount, numNeighboursOffset );
                
                hsize_t numNeighboursDimsRead[1]; 
                numNeighboursDimsRead[0] = 1;
                DataSpace numNeighboursMemspace( 1, numNeighboursDimsRead );
                
                hsize_t numNeighboursOffset_out[1];
                numNeighboursOffset_out[0] = 0;
                hsize_t numNeighboursCount_out[1];
                numNeighboursCount_out[0] = 1;
                numNeighboursMemspace.selectHyperslab( H5S_SELECT_SET, numNeighboursCount_out, numNeighboursOffset_out );
                
                numNeighboursDataset.read(&numNeighboursVal, PredType::NATIVE_UINT32, numNeighboursMemspace, numNeighboursDataspace);
                numNeighboursDataspace.close();
                numNeighboursMemspace.close();
                
                if(numNeighboursVal > 0)
                {
                    feat->neighbours->reserve(numNeighboursVal);
                    unsigned long *neighbourVals = new unsigned long[numNeighboursVal];
                    
                    DataSpace neighboursDataspace = neighboursDataset.getSpace();
                    hsize_t neighboursOffset[2];
                    neighboursOffset[0] = fid;
                    neighboursOffset[1] = 0;
                    hsize_t neighboursCount[2];
                    neighboursCount[0] = 1;
                    neighboursCount[1] = numNeighboursVal;
                    neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
                    
                    hsize_t neighboursDimsRead[2]; 
                    neighboursDimsRead[0] = 1;
                    neighboursDimsRead[1] = numNeighboursVal;
                    DataSpace neighboursMemspace( 2, neighboursDimsRead );
                    
                    hsize_t neighboursOffset_out[2];
                    neighboursOffset_out[0] = 0;
                    neighboursOffset_out[1] = 0;
                    hsize_t neighboursCount_out[2];
                    neighboursCount_out[0] = 1;
                    neighboursCount_out[1] = numNeighboursVal;
                    neighboursMemspace.selectHyperslab( H5S_SELECT_SET, neighboursCount_out, neighboursOffset_out );
                    
                    neighboursDataset.read(neighbourVals, PredType::NATIVE_UINT64, neighboursMemspace, neighboursDataspace);
                    
                    for(unsigned int i = 0; i < numNeighboursVal; ++i)
                    {
                        feat->neighbours->push_back(neighbourVals[i]);
                    }
                    
                    delete[] neighbourVals;
                    neighboursDataspace.close();
                    neighboursMemspace.close();
                }
                
                if(this->hasBoolFields)
                {
                    feat->boolFields->reserve(this->numBoolFields);
                    
                    DataSpace boolDataspace = boolDataset.getSpace();
                    hsize_t boolFieldsOffset[2];
                    boolFieldsOffset[0] = fid;
                    boolFieldsOffset[1] = 0;
                    hsize_t boolFieldsCount[2];
                    boolFieldsCount[0] = 1;
                    boolFieldsCount[1] = this->numBoolFields;
                    boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
                    
                    hsize_t boolFieldsDimsRead[2]; 
                    boolFieldsDimsRead[0] = 1;
                    boolFieldsDimsRead[1] = this->numBoolFields;
                    DataSpace boolFieldsMemspace( 2, boolFieldsDimsRead );
                    
                    hsize_t boolFieldsOffset_out[2];
                    boolFieldsOffset_out[0] = 0;
                    boolFieldsOffset_out[1] = 0;
                    hsize_t boolFieldsCount_out[2];
                    boolFieldsCount_out[0] = 1;
                    boolFieldsCount_out[1] = this->numBoolFields;
                    boolFieldsMemspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount_out, boolFieldsOffset_out );
                    
                    int *boolVals = new int[this->numBoolFields];
                    boolDataset.read(boolVals, PredType::NATIVE_INT8, boolFieldsMemspace, boolDataspace);
                    
                    for(unsigned int i = 0; i < this->numBoolFields; ++i)
                    {
                        feat->boolFields->push_back(boolVals[i]);
                    }
                    
                    delete[] boolVals;
                    boolDataspace.close();
                    boolFieldsMemspace.close();
                }
                
                if(this->hasIntFields)
                {
                    feat->intFields->reserve(this->numIntFields);
                    
                    DataSpace intDataspace = intDataset.getSpace();
                    hsize_t intFieldsOffset[2];
                    intFieldsOffset[0] = fid;
                    intFieldsOffset[1] = 0;
                    hsize_t intFieldsCount[2];
                    intFieldsCount[0] = 1;
                    intFieldsCount[1] = this->numIntFields;
                    intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );
                    
                    hsize_t intFieldsDimsRead[2]; 
                    intFieldsDimsRead[0] = 1;
                    intFieldsDimsRead[1] = this->numIntFields;
                    DataSpace intFieldsMemspace( 2, intFieldsDimsRead );
                    
                    hsize_t intFieldsOffset_out[2];
                    intFieldsOffset_out[0] = 0;
                    intFieldsOffset_out[1] = 0;
                    hsize_t intFieldsCount_out[2];
                    intFieldsCount_out[0] = 1;
                    intFieldsCount_out[1] = this->numIntFields;
                    intFieldsMemspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount_out, intFieldsOffset_out );
                    
                    long *intVals = new long[this->numIntFields];
                    intDataset.read(intVals, PredType::NATIVE_INT64, intFieldsMemspace, intDataspace);
                    
                    for(unsigned int i = 0; i < this->numIntFields; ++i)
                    {
                        feat->intFields->push_back(intVals[i]);
                    }
                    
                    delete[] intVals;
                    intDataspace.close();
                    intFieldsMemspace.close();
                }
                
                if(this->hasFloatFields)
                {
                    feat->floatFields->reserve(this->numFloatFields);
                    
                    DataSpace floatDataspace = floatDataset.getSpace();
                    hsize_t floatFieldsOffset[2];
                    floatFieldsOffset[0] = fid;
                    floatFieldsOffset[1] = 0;
                    hsize_t floatFieldsCount[2];
                    floatFieldsCount[0] = 1;
                    floatFieldsCount[1] = this->numIntFields;
                    floatDataspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount, floatFieldsOffset );
                    
                    hsize_t floatFieldsDimsRead[2]; 
                    floatFieldsDimsRead[0] = 1;
                    floatFieldsDimsRead[1] = this->numFloatFields;
                    DataSpace floatFieldsMemspace( 2, floatFieldsDimsRead );
                    
                    hsize_t floatFieldsOffset_out[2];
                    floatFieldsOffset_out[0] = 0;
                    floatFieldsOffset_out[1] = 0;
                    hsize_t floatFieldsCount_out[2];
                    floatFieldsCount_out[0] = 1;
                    floatFieldsCount_out[1] = this->numFloatFields;
                    floatFieldsMemspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount_out, floatFieldsOffset_out );
                    
                    double *floatVals = new double[this->numFloatFields];
                    floatDataset.read(floatVals, PredType::NATIVE_DOUBLE, floatFieldsMemspace, floatDataspace);
                    
                    for(unsigned int i = 0; i < this->numFloatFields; ++i)
                    {
                        feat->floatFields->push_back(floatVals[i]);
                    }
                    
                    delete[] floatVals;
                    floatDataspace.close();
                    floatFieldsMemspace.close();
                }
                
                ++numOfReads;
                featCache->insert(pair<size_t, RSGISFeature*>(fid, feat));
                cacheQ->push_back(fid);
                
                if(featCache->size() > maxCacheSize)
                {
                    //cout << "Remove last element from fid queue\n";
                    size_t tmpFID = *cacheQ->begin();
                    cacheQ->pop_front();
                    iterFeat = featCache->find(tmpFID);
                    this->returnFeatureToDisk((*iterFeat).second, !this->readOnly);
                    featCache->erase(iterFeat);
                }
            }
            else
            {
                //cout << "Got " << fid << " from cache - size = " << featCache->size() << endl;
                feat = (*iterFeat).second;
                
                // Move fid to back of queue.
                for(list<size_t>::iterator iterFIDs = cacheQ->begin(); iterFIDs != cacheQ->end(); )
                {
                    if((*iterFIDs) == fid)
                    {
                        iterFIDs = cacheQ->erase(iterFIDs);
                        cacheQ->push_back(fid);
                        break;
                    }
                    else
                    {
                        ++iterFIDs;
                    }
                }
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        return feat;
    }
    
    void RSGISAttributeTableHDFSmallCache::returnFeatureToDisk(RSGISFeature *feat, bool sync) throw(RSGISAttributeTableException)
    {
        try
        {
            if(sync)
            {
                // Update HDF file.
                unsigned int numNeighboursVal = feat->neighbours->size();
                DataSpace numNeighboursWriteDataSpace = numNeighboursDataset.getSpace();
                hsize_t numNeighboursDataOffset[1];
                numNeighboursDataOffset[0] = feat->fid;
                hsize_t numNeighboursDataDims[1];
                numNeighboursDataDims[0] = 1;
                numNeighboursWriteDataSpace.selectHyperslab(H5S_SELECT_SET, numNeighboursDataDims, numNeighboursDataOffset);
                DataSpace newNumNeighboursDataspace = DataSpace(1, numNeighboursDataDims);
                numNeighboursDataset.write(&numNeighboursVal, PredType::NATIVE_UINT32, newNumNeighboursDataspace, numNeighboursWriteDataSpace);
                numNeighboursWriteDataSpace.close();
                newNumNeighboursDataspace.close();
                
                if(numNeighboursVal > 0)
                {
                    size_t *neighbourVals = new size_t[numNeighboursVal];
                    unsigned int i = 0; 
                    for(vector<size_t>::iterator iterNeigh = feat->neighbours->begin(); iterNeigh != feat->neighbours->end(); ++iterNeigh)
                    {
                        neighbourVals[i++] = *iterNeigh;
                    }
                    
                    if(numNeighboursVal > this->neighboursLineLength)
                    {
                        // Need to extend dataset!
                    }
                    else
                    {
                        hsize_t neighboursDataOffset[2];
                        neighboursDataOffset[0] = feat->fid;
                        neighboursDataOffset[1] = 0;
                        hsize_t neighboursDataDims[2];
                        neighboursDataDims[0] = 1;
                        neighboursDataDims[1] = numNeighboursVal;
                        DataSpace neighboursWriteDataSpace = neighboursDataset.getSpace();
                        neighboursWriteDataSpace.selectHyperslab(H5S_SELECT_SET, neighboursDataDims, neighboursDataOffset);
                        DataSpace newNeighboursDataspace = DataSpace(2, neighboursDataDims);
                        neighboursDataset.write(neighbourVals, PredType::NATIVE_UINT64, newNeighboursDataspace, neighboursWriteDataSpace);
                        neighboursWriteDataSpace.close();
                        newNeighboursDataspace.close();
                    }
                    delete[] neighbourVals;
                }
                
                if(this->numBoolFields > 0)
                {
                    int *boolVals = new int[this->numBoolFields];
                    unsigned int i = 0; 
                    for(vector<bool>::iterator iterBools = feat->boolFields->begin(); iterBools != feat->boolFields->end(); ++iterBools)
                    {
                        boolVals[i++] = *iterBools;
                    }
                    
                    hsize_t boolDataOffset[2];
                    boolDataOffset[0] = feat->fid;
                    boolDataOffset[1] = 0;
                    hsize_t boolDataDims[2];
                    boolDataDims[0] = 1;
                    boolDataDims[1] = this->numBoolFields;
                    DataSpace boolWriteDataSpace = boolDataset.getSpace();
                    boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                    DataSpace newBoolDataspace = DataSpace(2, boolDataDims);
                    boolDataset.write(boolVals, PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                    boolWriteDataSpace.close();
                    newBoolDataspace.close();
                    delete[] boolVals;
                }
                
                if(this->numIntFields > 0)
                {
                    long *intVals = new long[this->numIntFields];
                    unsigned int i = 0; 
                    for(vector<long>::iterator iterInts = feat->intFields->begin(); iterInts != feat->intFields->end(); ++iterInts)
                    {
                        intVals[i++] = *iterInts;
                    }
                    
                    hsize_t intDataOffset[2];
                    intDataOffset[0] = feat->fid;
                    intDataOffset[1] = 0;
                    hsize_t intDataDims[2];
                    intDataDims[0] = 1;
                    intDataDims[1] = this->numIntFields;
                    DataSpace intWriteDataSpace = intDataset.getSpace();
                    intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                    DataSpace newIntDataspace = DataSpace(1, intDataDims);
                    intDataset.write(intVals, PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                    intWriteDataSpace.close();
                    newIntDataspace.close();
                    delete[] intVals;
                }
                
                if(this->numFloatFields > 0)
                {
                    double *floatVals = new double[this->numFloatFields];
                    unsigned int i = 0; 
                    for(vector<double>::iterator iterFloats = feat->floatFields->begin(); iterFloats != feat->floatFields->end(); ++iterFloats)
                    {
                        floatVals[i++] = *iterFloats;
                    }
                    
                    hsize_t floatDataOffset[2];
                    floatDataOffset[0] = feat->fid;
                    floatDataOffset[1] = 0;
                    hsize_t floatDataDims[2];
                    floatDataDims[0] = 1;
                    floatDataDims[1] = this->numFloatFields;
                    DataSpace floatWriteDataSpace = floatDataset.getSpace();
                    floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                    DataSpace newFloatDataspace = DataSpace(2, floatDataDims);
                    floatDataset.write(floatVals, PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                    floatWriteDataSpace.close();
                    newFloatDataspace.close();
                    delete[] floatVals;
                }
                
                ++numOfWrites;
            }
            this->freeFeature(feat);
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    void RSGISAttributeTableHDFSmallCache::flushAllFeatures() throw(RSGISAttributeTableException)
    {
        try
        {
            for(map<size_t, RSGISFeature*>::iterator iterFeat = featCache->begin(); iterFeat != featCache->end(); ++iterFeat)
            {
                this->returnFeatureToDisk((*iterFeat).second, !this->readOnly);
            }
            featCache->clear();
            cacheQ->clear();
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    void RSGISAttributeTableHDFSmallCache::addAttBoolField(string name, bool val) throw(RSGISAttributeTableException)
    {
        if(this->hasAttribute(name))
        {
            string message = string("The field \'") + name + string("\' already exists within the attribute table.");
            throw RSGISAttributeTableException(message);
        }
        
        try
        {
            fieldIdx->insert(pair<string, unsigned int>(name, numBoolFields));
            fieldDataType->insert(pair<string, RSGISAttributeDataType>(name, rsgis_bool));
            fields->push_back(pair<string, RSGISAttributeDataType>(name, rsgis_bool));
            
            CompType *fieldDtDisk = this->createAttibuteIdxCompTypeDisk();
            RSGISAttributeIdx *boolFields = new RSGISAttributeIdx[1];
            boolFields[0].name = const_cast<char*>(name.c_str());
            boolFields[0].idx = this->numBoolFields++;
            
            if(this->hasBoolFields)
            {
                DataSet boolFieldsDataset = attH5File->openDataSet( ATT_BOOL_FIELDS_HEADER );
                DataSpace boolFieldsDataSpace = boolFieldsDataset.getSpace();
                
                hsize_t extendBoolFieldsDatasetTo[1];
                extendBoolFieldsDatasetTo[0] = this->numBoolFields;
                boolFieldsDataset.extend( extendBoolFieldsDatasetTo );
                
                hsize_t boolFieldsOffset[1];
                boolFieldsOffset[0] = this->numBoolFields-1;
                hsize_t boolFieldsDataDims[1];
                boolFieldsDataDims[0] = 1;
                
                DataSpace boolFieldsWriteDataSpace = boolFieldsDataset.getSpace();
                boolFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolFieldsDataDims, boolFieldsOffset);
                DataSpace newBoolFieldsDataspace = DataSpace(1, boolFieldsDataDims);
                
                boolFieldsDataset.write(boolFields, *fieldDtDisk, newBoolFieldsDataspace, boolFieldsWriteDataSpace);
                
                hsize_t extendBoolDatasetTo[2];
                extendBoolDatasetTo[0] = this->getSize();
                extendBoolDatasetTo[1] = this->numBoolFields;
                this->boolDataset.extend( extendBoolDatasetTo );
                
                int *boolVals = new int[ATT_WRITE_CHUNK_SIZE];
                for(unsigned int i  = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    boolVals[i] = val;
                }
                
                unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
                unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
                
                hsize_t boolDataOffset[2];
                boolDataOffset[0] = 0;
                boolDataOffset[1] = this->numBoolFields-1;
                hsize_t boolDataDims[2];
                boolDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                boolDataDims[1] = 1;
                
                size_t currentSize = 0;
                for(unsigned long i = 0; i < numChunks; ++i)
                {
                    boolDataOffset[0] = currentSize;
                    boolDataOffset[1] = this->numBoolFields-1;
                    
                    DataSpace boolWriteDataSpace = this->boolDataset.getSpace();
                    boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                    DataSpace newBoolDataspace = DataSpace(2, boolDataDims);
                    
                    this->boolDataset.write(boolVals, PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                    
                    currentSize += ATT_WRITE_CHUNK_SIZE;
                    boolWriteDataSpace.close();
                    newBoolDataspace.close();
                }
                
                boolDataDims[0] = remainingRows;
                boolDataDims[1] = 1;
                
                boolDataOffset[0] = currentSize;
                boolDataOffset[1] = this->numBoolFields-1;
                
                DataSpace boolWriteDataSpace = this->boolDataset.getSpace();
                boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                DataSpace newBoolDataspace = DataSpace(2, boolDataDims);
                
                this->boolDataset.write(boolVals, PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                
                delete[] boolVals;
                boolFieldsDataSpace.close();
                boolFieldsDataset.close();
                boolFieldsWriteDataSpace.close();
                newBoolFieldsDataspace.close();
                boolWriteDataSpace.close();
                newBoolDataspace.close();
            }
            else
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
                DataSet boolFieldsDataset = DataSet(attH5File->createDataSet(ATT_INT_FIELDS_HEADER, *fieldDtDisk, boolFieldsDataSpace, creationBoolFieldsDSPList));
                
                hsize_t extendBoolFieldsDatasetTo[1];
                extendBoolFieldsDatasetTo[0] = this->numBoolFields;
                boolFieldsDataset.extend( extendBoolFieldsDatasetTo );
                
                hsize_t boolFieldsOffset[1];
                boolFieldsOffset[0] = 0;
                hsize_t boolFieldsDataDims[1];
                boolFieldsDataDims[0] = this->numBoolFields;
                
                DataSpace boolFieldsWriteDataSpace = boolFieldsDataset.getSpace();
                boolFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolFieldsDataDims, boolFieldsOffset);
                DataSpace newBoolFieldsDataspace = DataSpace(1, boolFieldsDataDims);
                
                boolFieldsDataset.write(boolFields, *fieldDtDisk, newBoolFieldsDataspace, boolFieldsWriteDataSpace);
                
                hsize_t initDimsBoolDS[2];
                initDimsBoolDS[0] = this->getSize();
                initDimsBoolDS[1] = this->numBoolFields;
                hsize_t maxDimsBoolDS[2];
                maxDimsBoolDS[0] = H5S_UNLIMITED;
                maxDimsBoolDS[1] = H5S_UNLIMITED;
                DataSpace boolDataSpace = DataSpace(2, initDimsBoolDS, maxDimsBoolDS);
                
                hsize_t dimsBoolChunk[2];
                dimsBoolChunk[0] = ATT_WRITE_CHUNK_SIZE;
                dimsBoolChunk[1] = 1;
                
                int fillValueBool = val;
                DSetCreatPropList creationBoolDSPList;
                creationBoolDSPList.setChunk(2, dimsBoolChunk);
                creationBoolDSPList.setShuffle();
                creationBoolDSPList.setDeflate(ATT_WRITE_DEFLATE);
                creationBoolDSPList.setFillValue( PredType::STD_I8LE, &fillValueBool);
                
                this->boolDataset = DataSet(attH5File->createDataSet(ATT_BOOL_DATA, PredType::STD_I8LE, boolDataSpace, creationBoolDSPList));
                
                int *boolVals = new int[ATT_WRITE_CHUNK_SIZE];
                for(unsigned int i  = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    boolVals[i] = val;
                }
                
                unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
                unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
                
                hsize_t boolDataOffset[2];
                boolDataOffset[0] = 0;
                boolDataOffset[1] = 0;
                hsize_t boolDataDims[2];
                boolDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                boolDataDims[1] = 1;
                hsize_t extendBoolDatasetTo[2];
                extendBoolDatasetTo[0] = this->getSize();
                extendBoolDatasetTo[1] = this->numBoolFields;
                this->boolDataset.extend( extendBoolDatasetTo );
                
                size_t currentSize = 0;
                for(unsigned long i = 0; i < numChunks; ++i)
                {
                    boolDataOffset[0] = currentSize;
                    boolDataOffset[1] = 0;
                    
                    DataSpace boolWriteDataSpace = this->boolDataset.getSpace();
                    boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                    DataSpace newBoolDataspace = DataSpace(2, boolDataDims);
                    
                    this->boolDataset.write(boolVals, PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                    
                    currentSize += ATT_WRITE_CHUNK_SIZE;
                    boolWriteDataSpace.close();
                    newBoolDataspace.close();
                }
                
                boolDataDims[0] = remainingRows;
                boolDataDims[1] = 1;
                
                boolDataOffset[0] = currentSize;
                boolDataOffset[1] = 0;
                
                DataSpace boolWriteDataSpace = this->boolDataset.getSpace();
                boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                DataSpace newBoolDataspace = DataSpace(2, boolDataDims);
                
                this->boolDataset.write(boolVals, PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                
                delete[] boolVals;
                boolFieldsDataSpace.close();
                boolFieldsDataset.close();
                boolFieldsWriteDataSpace.close();
                newBoolFieldsDataspace.close();
                boolDataSpace.close();
                boolWriteDataSpace.close();
                newBoolDataspace.close();
            }
            delete[] boolFields;
            delete fieldDtDisk;
            this->hasBoolFields = true;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    void RSGISAttributeTableHDFSmallCache::addAttIntField(string name, long val) throw(RSGISAttributeTableException)
    {
        if(this->hasAttribute(name))
        {
            string message = string("The field \'") + name + string("\' already exists within the attribute table.");
            throw RSGISAttributeTableException(message);
        }
        
        try
        {
            fieldIdx->insert(pair<string, unsigned int>(name, numIntFields));
            fieldDataType->insert(pair<string, RSGISAttributeDataType>(name, rsgis_int));
            fields->push_back(pair<string, RSGISAttributeDataType>(name, rsgis_int));
            
            CompType *fieldDtDisk = this->createAttibuteIdxCompTypeDisk();
            RSGISAttributeIdx *intFields = new RSGISAttributeIdx[1];
            intFields[0].name = const_cast<char*>(name.c_str());
            intFields[0].idx = this->numIntFields++;
            
            if(this->hasIntFields)
            {
                DataSet intFieldsDataset = attH5File->openDataSet( ATT_INT_FIELDS_HEADER );
                DataSpace intFieldsDataSpace = intFieldsDataset.getSpace();
                
                hsize_t extendIntFieldsDatasetTo[1];
                extendIntFieldsDatasetTo[0] = this->numIntFields;
                intFieldsDataset.extend( extendIntFieldsDatasetTo );
                
                hsize_t intFieldsOffset[1];
                intFieldsOffset[0] = this->numIntFields-1;
                hsize_t intFieldsDataDims[1];
                intFieldsDataDims[0] = 1;
                
                DataSpace intFieldsWriteDataSpace = intFieldsDataset.getSpace();
                intFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intFieldsDataDims, intFieldsOffset);
                DataSpace newIntFieldsDataspace = DataSpace(1, intFieldsDataDims);
                
                intFieldsDataset.write(intFields, *fieldDtDisk, newIntFieldsDataspace, intFieldsWriteDataSpace);
                
                hsize_t extendIntDatasetTo[2];
                extendIntDatasetTo[0] = this->getSize();
                extendIntDatasetTo[1] = this->numIntFields;
                this->intDataset.extend( extendIntDatasetTo );
                
                long *intVals = new long[ATT_WRITE_CHUNK_SIZE];
                for(unsigned int i  = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    intVals[i] = val;
                }
                
                unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
                unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
                
                hsize_t intDataOffset[2];
                intDataOffset[0] = 0;
                intDataOffset[1] = this->numIntFields-1;
                hsize_t intDataDims[2];
                intDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                intDataDims[1] = 1;
                
                size_t currentSize = 0;
                for(unsigned long i = 0; i < numChunks; ++i)
                {
                    intDataOffset[0] = currentSize;
                    intDataOffset[1] = this->numIntFields-1;
                    
                    DataSpace intWriteDataSpace = this->intDataset.getSpace();
                    intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                    DataSpace newIntDataspace = DataSpace(2, intDataDims);
                    
                    this->intDataset.write(intVals, PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                    
                    currentSize += ATT_WRITE_CHUNK_SIZE;
                    intWriteDataSpace.close();
                    newIntDataspace.close();
                }
                
                intDataDims[0] = remainingRows;
                intDataDims[1] = 1;
                
                intDataOffset[0] = currentSize;
                intDataOffset[1] = this->numIntFields-1;
                
                DataSpace intWriteDataSpace = this->intDataset.getSpace();
                intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                DataSpace newIntDataspace = DataSpace(2, intDataDims);
                
                this->intDataset.write(intVals, PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                
                delete[] intVals;
                intFieldsDataSpace.close();
                intFieldsDataset.close();
                intFieldsWriteDataSpace.close();
                newIntFieldsDataspace.close();
                intWriteDataSpace.close();
                newIntDataspace.close();
            }
            else
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
                DataSet intFieldsDataset = DataSet(attH5File->createDataSet(ATT_INT_FIELDS_HEADER, *fieldDtDisk, intFieldsDataSpace, creationIntFieldsDSPList));
                
                hsize_t extendIntFieldsDatasetTo[1];
                extendIntFieldsDatasetTo[0] = this->numIntFields;
                intFieldsDataset.extend( extendIntFieldsDatasetTo );
                
                hsize_t intFieldsOffset[1];
                intFieldsOffset[0] = 0;
                hsize_t intFieldsDataDims[1];
                intFieldsDataDims[0] = this->numIntFields;
                
                DataSpace intFieldsWriteDataSpace = intFieldsDataset.getSpace();
                intFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intFieldsDataDims, intFieldsOffset);
                DataSpace newIntFieldsDataspace = DataSpace(1, intFieldsDataDims);
                
                intFieldsDataset.write(intFields, *fieldDtDisk, newIntFieldsDataspace, intFieldsWriteDataSpace);
                
                hsize_t initDimsIntDS[2];
                initDimsIntDS[0] = this->getSize();
                initDimsIntDS[1] = this->numIntFields;
                hsize_t maxDimsIntDS[2];
                maxDimsIntDS[0] = H5S_UNLIMITED;
                maxDimsIntDS[1] = H5S_UNLIMITED;
                DataSpace intDataSpace = DataSpace(2, initDimsIntDS, maxDimsIntDS);
                
                hsize_t dimsIntChunk[2];
                dimsIntChunk[0] = ATT_WRITE_CHUNK_SIZE;
                dimsIntChunk[1] = 1;
                
                long fillValueLong = val;
                DSetCreatPropList creationIntDSPList;
                creationIntDSPList.setChunk(2, dimsIntChunk);
                creationIntDSPList.setShuffle();
                creationIntDSPList.setDeflate(ATT_WRITE_DEFLATE);
                creationIntDSPList.setFillValue( PredType::STD_I64LE, &fillValueLong);
                
                this->intDataset = DataSet(attH5File->createDataSet(ATT_INT_DATA, PredType::STD_I64LE, intDataSpace, creationIntDSPList));
                
                long *intVals = new long[ATT_WRITE_CHUNK_SIZE];
                for(unsigned int i  = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    intVals[i] = val;
                }
                
                unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
                unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
                
                hsize_t intDataOffset[2];
                intDataOffset[0] = 0;
                intDataOffset[1] = 0;
                hsize_t intDataDims[2];
                intDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                intDataDims[1] = 1;
                hsize_t extendIntDatasetTo[2];
                extendIntDatasetTo[0] = this->getSize();
                extendIntDatasetTo[1] = this->numIntFields;
                this->intDataset.extend( extendIntDatasetTo );
                
                
                size_t currentSize = 0;
                for(unsigned long i = 0; i < numChunks; ++i)
                {
                    intDataOffset[0] = currentSize;
                    intDataOffset[1] = 0;
                    
                    DataSpace intWriteDataSpace = this->intDataset.getSpace();
                    intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                    DataSpace newIntDataspace = DataSpace(2, intDataDims);
                    
                    this->intDataset.write(intVals, PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                    
                    currentSize += ATT_WRITE_CHUNK_SIZE;
                    intWriteDataSpace.close();
                    newIntDataspace.close();
                }
                
                intDataDims[0] = remainingRows;
                intDataDims[1] = 1;
                
                intDataOffset[0] = currentSize;
                intDataOffset[1] = 0;
                
                DataSpace intWriteDataSpace = this->intDataset.getSpace();
                intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                DataSpace newIntDataspace = DataSpace(2, intDataDims);
                
                this->intDataset.write(intVals, PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                
                delete[] intVals;
                intFieldsDataSpace.close();
                intFieldsDataset.close();
                intFieldsWriteDataSpace.close();
                newIntFieldsDataspace.close();
                intDataSpace.close();
                intWriteDataSpace.close();
                newIntDataspace.close();
            }
            delete[] intFields;
            delete fieldDtDisk;
            this->hasIntFields = true;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    void RSGISAttributeTableHDFSmallCache::addAttFloatField(string name, double val) throw(RSGISAttributeTableException)
    {
        if(this->hasAttribute(name))
        {
            string message = string("The field \'") + name + string("\' already exists within the attribute table.");
            throw RSGISAttributeTableException(message);
        }
        
        try
        {
            fieldIdx->insert(pair<string, unsigned int>(name, numFloatFields));
            fieldDataType->insert(pair<string, RSGISAttributeDataType>(name, rsgis_float));
            fields->push_back(pair<string, RSGISAttributeDataType>(name, rsgis_float));
            
            CompType *fieldDtDisk = this->createAttibuteIdxCompTypeDisk();
            RSGISAttributeIdx *floatFields = new RSGISAttributeIdx[1];
            floatFields[0].name = const_cast<char*>(name.c_str());
            floatFields[0].idx = this->numFloatFields++;
            
            if(this->hasFloatFields)
            {
                DataSet floatFieldsDataset = attH5File->openDataSet( ATT_FLOAT_FIELDS_HEADER );
                DataSpace floatFieldsDataSpace = floatFieldsDataset.getSpace();
                
                hsize_t extendFloatFieldsDatasetTo[1];
                extendFloatFieldsDatasetTo[0] = this->numFloatFields;
                floatFieldsDataset.extend( extendFloatFieldsDatasetTo );
                
                hsize_t floatFieldsOffset[1];
                floatFieldsOffset[0] = this->numFloatFields-1;
                hsize_t floatFieldsDataDims[1];
                floatFieldsDataDims[0] = 1;
                
                DataSpace floatFieldsWriteDataSpace = floatFieldsDataset.getSpace();
                floatFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatFieldsDataDims, floatFieldsOffset);
                DataSpace newFloatFieldsDataspace = DataSpace(1, floatFieldsDataDims);
                
                floatFieldsDataset.write(floatFields, *fieldDtDisk, newFloatFieldsDataspace, floatFieldsWriteDataSpace);
                
                hsize_t extendFloatDatasetTo[2];
                extendFloatDatasetTo[0] = this->getSize();
                extendFloatDatasetTo[1] = this->numFloatFields;
                this->floatDataset.extend( extendFloatDatasetTo );
                
                double *floatVals = new double[ATT_WRITE_CHUNK_SIZE];
                for(unsigned int i  = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    floatVals[i] = val;
                }
                
                unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
                unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
                
                hsize_t floatDataOffset[2];
                floatDataOffset[0] = 0;
                floatDataOffset[1] = this->numFloatFields-1;
                hsize_t floatDataDims[2];
                floatDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                floatDataDims[1] = 1;
                
                size_t currentSize = 0;
                for(unsigned long i = 0; i < numChunks; ++i)
                {
                    floatDataOffset[0] = currentSize;
                    floatDataOffset[1] = this->numFloatFields-1;
                    
                    DataSpace floatWriteDataSpace = this->floatDataset.getSpace();
                    floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                    DataSpace newFloatDataspace = DataSpace(2, floatDataDims);
                    
                    this->floatDataset.write(floatVals, PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                    
                    currentSize += ATT_WRITE_CHUNK_SIZE;
                    floatWriteDataSpace.close();
                    newFloatDataspace.close();
                }
                
                floatDataDims[0] = remainingRows;
                floatDataDims[1] = 1;
                
                floatDataOffset[0] = currentSize;
                floatDataOffset[1] = this->numFloatFields-1;
                
                DataSpace floatWriteDataSpace = this->floatDataset.getSpace();
                floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                DataSpace newFloatDataspace = DataSpace(2, floatDataDims);
                
                this->floatDataset.write(floatVals, PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                
                delete[] floatVals;
                floatFieldsDataSpace.close();
                floatFieldsDataset.close();
                floatFieldsWriteDataSpace.close();
                newFloatFieldsDataspace.close();
                floatWriteDataSpace.close();
                newFloatDataspace.close();
            }
            else
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
                DataSet floatFieldsDataset = DataSet(attH5File->createDataSet(ATT_INT_FIELDS_HEADER, *fieldDtDisk, floatFieldsDataSpace, creationFloatFieldsDSPList));
                
                hsize_t extendFloatFieldsDatasetTo[1];
                extendFloatFieldsDatasetTo[0] = this->numFloatFields;
                floatFieldsDataset.extend( extendFloatFieldsDatasetTo );
                
                hsize_t floatFieldsOffset[1];
                floatFieldsOffset[0] = 0;
                hsize_t floatFieldsDataDims[1];
                floatFieldsDataDims[0] = this->numFloatFields;
                
                DataSpace floatFieldsWriteDataSpace = floatFieldsDataset.getSpace();
                floatFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatFieldsDataDims, floatFieldsOffset);
                DataSpace newFloatFieldsDataspace = DataSpace(1, floatFieldsDataDims);
                
                floatFieldsDataset.write(floatFields, *fieldDtDisk, newFloatFieldsDataspace, floatFieldsWriteDataSpace);
                
                hsize_t initDimsFloatDS[2];
                initDimsFloatDS[0] = this->getSize();
                initDimsFloatDS[1] = this->numFloatFields;
                hsize_t maxDimsFloatDS[2];
                maxDimsFloatDS[0] = H5S_UNLIMITED;
                maxDimsFloatDS[1] = H5S_UNLIMITED;
                DataSpace floatDataSpace = DataSpace(2, initDimsFloatDS, maxDimsFloatDS);
                
                hsize_t dimsFloatChunk[2];
                dimsFloatChunk[0] = ATT_WRITE_CHUNK_SIZE;
                dimsFloatChunk[1] = 1;
                
                double fillValueDouble = val;
                DSetCreatPropList creationFloatDSPList;
                creationFloatDSPList.setChunk(2, dimsFloatChunk);
                creationFloatDSPList.setShuffle();
                creationFloatDSPList.setDeflate(ATT_WRITE_DEFLATE);
                creationFloatDSPList.setFillValue( PredType::IEEE_F64LE, &fillValueDouble);
                
                this->floatDataset = DataSet(attH5File->createDataSet(ATT_FLOAT_DATA, PredType::IEEE_F64LE, floatDataSpace, creationFloatDSPList));
                
                double *floatVals = new double[ATT_WRITE_CHUNK_SIZE];
                for(unsigned int i  = 0; i < ATT_WRITE_CHUNK_SIZE; ++i)
                {
                    floatVals[i] = val;
                }
                
                unsigned long numChunks = this->getSize() / ATT_WRITE_CHUNK_SIZE;
                unsigned long remainingRows = this->getSize() - (numChunks * ATT_WRITE_CHUNK_SIZE);
                
                hsize_t floatDataOffset[2];
                floatDataOffset[0] = 0;
                floatDataOffset[1] = 0;
                hsize_t floatDataDims[2];
                floatDataDims[0] = ATT_WRITE_CHUNK_SIZE;
                floatDataDims[1] = 1;
                hsize_t extendFloatDatasetTo[2];
                extendFloatDatasetTo[0] = this->getSize();
                extendFloatDatasetTo[1] = this->numFloatFields;
                this->floatDataset.extend( extendFloatDatasetTo );
                
                size_t currentSize = 0;
                for(unsigned long i = 0; i < numChunks; ++i)
                {
                    floatDataOffset[0] = currentSize;
                    floatDataOffset[1] = 0;
                    
                    DataSpace floatWriteDataSpace = this->floatDataset.getSpace();
                    floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                    DataSpace newFloatDataspace = DataSpace(2, floatDataDims);
                    
                    this->floatDataset.write(floatVals, PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                    
                    currentSize += ATT_WRITE_CHUNK_SIZE;
                    floatWriteDataSpace.close();
                    newFloatDataspace.close();
                }
                
                floatDataDims[0] = remainingRows;
                floatDataDims[1] = 1;
                
                floatDataOffset[0] = currentSize;
                floatDataOffset[1] = 0;
                
                DataSpace floatWriteDataSpace = this->floatDataset.getSpace();
                floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                DataSpace newFloatDataspace = DataSpace(2, floatDataDims);
                
                this->floatDataset.write(floatVals, PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                
                delete[] floatVals;
                floatFieldsDataSpace.close();
                floatFieldsDataset.close();
                floatFieldsWriteDataSpace.close();
                newFloatFieldsDataspace.close();
                floatDataSpace.close();
                floatWriteDataSpace.close();
                newFloatDataspace.close();
            }
            delete[] floatFields;
            delete fieldDtDisk;
            this->hasFloatFields = true;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
    }
    
    void RSGISAttributeTableHDFSmallCache::addAttStringField(string name, string val) throw(RSGISAttributeTableException)
    {
        throw RSGISAttributeTableException("String fields are not currently supported within HDF5 attribute tables.");
    }
    
    void RSGISAttributeTableHDFSmallCache::addAttributes(vector<RSGISAttribute*> *attributes) throw(RSGISAttributeTableException)
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
    
    size_t RSGISAttributeTableHDFSmallCache::getSize()
    {
        return attSize;
    }
    
    void RSGISAttributeTableHDFSmallCache::operator++()
    {
        ++iterIdx;
    }
    
    void RSGISAttributeTableHDFSmallCache::start()
    {
        iterIdx = 0;
    }
    
    bool RSGISAttributeTableHDFSmallCache::end()
    {
        return iterIdx < attSize;
    }
    
    RSGISFeature* RSGISAttributeTableHDFSmallCache::operator*()
    {
        return this->getFeature(iterIdx);
    }
    
    RSGISAttributeTable* RSGISAttributeTableHDFSmallCache::importFromHDF5(string inFile)throw(RSGISAttributeTableException)
    {
        RSGISAttributeTableHDFSmallCache *attTableObj = new RSGISAttributeTableHDFSmallCache();
        try
        {
            Exception::dontPrint();
            
            FileAccPropList attAccessPlist = FileAccPropList(FileAccPropList::DEFAULT);
            attAccessPlist.setCache(ATT_READ_MDC_NELMTS, ATT_READ_RDCC_NELMTS, ATT_READ_RDCC_NBYTES, ATT_READ_RDCC_W0);
            attAccessPlist.setSieveBufSize(ATT_READ_SIEVE_BUF);
            hsize_t metaBlockSize = ATT_READ_META_BLOCKSIZE;
            attAccessPlist.setMetaBlockSize(metaBlockSize);
            
            attTableObj->attH5File = new H5File( inFile, H5F_ACC_RDONLY, FileCreatPropList::DEFAULT, attAccessPlist);
            
            attTableObj->hasBoolFields = true;
            attTableObj->hasIntFields = true;
            attTableObj->hasFloatFields = true;
            
            attTableObj->numStrFields = 0;
            
            attTableObj->fields = new vector<pair<string, RSGISAttributeDataType> >();
            attTableObj->fieldIdx = new map<string, unsigned int>();
            attTableObj->fieldDataType = new map<string, RSGISAttributeDataType>();
            
            attTableObj->numBoolFields = 0;
            attTableObj->numIntFields = 0;
            attTableObj->numFloatFields = 0;
            attTableObj->numStrFields = 0;
            
            CompType *fieldCompTypeMem = attTableObj->createAttibuteIdxCompTypeMem();
            try
            {
                DataSet boolFieldsDataset = attTableObj->attH5File->openDataSet( ATT_BOOL_FIELDS_HEADER );
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
                attTableObj->hasBoolFields = false;
            }
            
            try
            {
                DataSet intFieldsDataset = attTableObj->attH5File->openDataSet( ATT_INT_FIELDS_HEADER );
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
                attTableObj->hasIntFields = false;
            }
            
            try
            {
                DataSet floatFieldsDataset = attTableObj->attH5File->openDataSet( ATT_FLOAT_FIELDS_HEADER );
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
                attTableObj->hasFloatFields = false;
            }
            
            delete fieldCompTypeMem;
            
            attTableObj->neighboursDataset = attTableObj->attH5File->openDataSet( ATT_NEIGHBOURS_DATA );
            DataSpace neighboursDataspace = attTableObj->neighboursDataset.getSpace();
            
            attTableObj->numNeighboursDataset = attTableObj->attH5File->openDataSet( ATT_NUM_NEIGHBOURS_DATA );
            DataSpace numNeighboursDataspace = attTableObj->numNeighboursDataset.getSpace();
            
            int neighboursNDims = neighboursDataspace.getSimpleExtentNdims();
            
            if(neighboursNDims != 2)
            {
                throw RSGISAttributeTableException("The neighbours datasets needs to have 2 dimensions.");
            }
            
            hsize_t *neighboursDims = new hsize_t[neighboursNDims];
            neighboursDataspace.getSimpleExtentDims(neighboursDims);
            
            attTableObj->neighboursLineLength = neighboursDims[1];
            
            hsize_t *numNeighboursDims = new hsize_t[1];
            numNeighboursDataspace.getSimpleExtentDims(numNeighboursDims);
            
            if(numNeighboursDims[0] != neighboursDims[0])
            {
                throw RSGISAttributeTableException("The number features in the different neighbour datasets is not equal.");
            }
            
            attTableObj->attSize = numNeighboursDims[0];
            
            delete[] numNeighboursDims;
            delete[] neighboursDims;
            
            if(attTableObj->hasBoolFields)
            {
                attTableObj->boolDataset = attTableObj->attH5File->openDataSet( ATT_BOOL_DATA );
                DataSpace boolDataspace = attTableObj->boolDataset.getSpace();
                
                int boolNDims = boolDataspace.getSimpleExtentNdims();
                
                if(boolNDims != 2)
                {
                    throw RSGISAttributeTableException("The boolean datasets needs to have 2 dimensions.");
                }
                
                hsize_t *boolDims = new hsize_t[boolNDims];
                boolDataspace.getSimpleExtentDims(boolDims);
                
                if(boolDims[0] != attTableObj->attSize)
                {
                    throw RSGISAttributeTableException("The number of features in boolean datasets does not match expected values.");
                }
                
                if(boolDims[1] != attTableObj->numBoolFields)
                {
                    throw RSGISAttributeTableException("The number of boolean fields does not match expected values.");
                }
                delete[] boolDims;                
            }
            
            if(attTableObj->hasIntFields)
            {
                attTableObj->intDataset = attTableObj->attH5File->openDataSet( ATT_INT_DATA );
                DataSpace intDataspace = attTableObj->intDataset.getSpace();
                
                int intNDims = intDataspace.getSimpleExtentNdims();
                
                if(intNDims != 2)
                {
                    throw RSGISAttributeTableException("The integer datasets needs to have 2 dimensions.");
                }
                
                hsize_t *intDims = new hsize_t[intNDims];
                intDataspace.getSimpleExtentDims(intDims);
                
                if(intDims[0] != attTableObj->attSize)
                {
                    throw RSGISAttributeTableException("The number of features in integer datasets does not match expected values.");
                }
                
                if(intDims[1] != attTableObj->numIntFields)
                {
                    throw RSGISAttributeTableException("The number of integer fields does not match expected values.");
                }
                delete[] intDims;
            }
            
            if(attTableObj->hasFloatFields)
            {
                attTableObj->floatDataset = attTableObj->attH5File->openDataSet( ATT_FLOAT_DATA );
                DataSpace floatDataspace = attTableObj->floatDataset.getSpace();
                
                int floatNDims = floatDataspace.getSimpleExtentNdims();
                
                if(floatNDims != 2)
                {
                    throw RSGISAttributeTableException("The float datasets needs to have 2 dimensions.");
                }
                
                hsize_t *floatDims = new hsize_t[floatNDims];
                floatDataspace.getSimpleExtentDims(floatDims);
                
                if(floatDims[0] != attTableObj->attSize)
                {
                    throw RSGISAttributeTableException("The number of features in float datasets does not match expected values.");
                }
                
                if(floatDims[1] != attTableObj->numFloatFields)
                {
                    throw RSGISAttributeTableException("The number of float fields does not match expected values.");
                }
                
                delete[] floatDims;
            }
            
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
        catch( Exception &e )
        {
            throw RSGISAttributeTableException(e.getDetailMsg());
        }
        
        attTableObj->attOpen = true;
        
        return attTableObj;
    }
    
    RSGISAttributeTableHDFSmallCache::~RSGISAttributeTableHDFSmallCache()
    {
        
        if(featCache->size() > 0)
        {
            this->flushAllFeatures();
        }
        delete featCache;
        delete cacheQ;
        
        this->attH5File->flush(H5F_SCOPE_GLOBAL);
        this->attH5File->close();
        delete this->attH5File;
        
        cout << "There have been " << numOfReads << " reads\n";
        cout << "There have been " << numOfWrites << " writes\n";
    }
    
    
    */
    
    
    
    
    
    
    
    
    
}}