/*
 *  RSGISHistoCubeFileIO.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/02/2017.
 *
 *  Copyright 2017 RSGISLib.
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

#include "RSGISHistoCubeFileIO.h"

namespace rsgis {namespace histocube{
    

    RSGISHistoCubeFile::RSGISHistoCubeFile()
    {
        fileOpen = false;
        rwAccess = false;
        H5::Exception::dontPrint();
        this->numOfFeats = 0;
        cubeLayers = new std::vector<RSGISHistCubeLayerMeta*>();
    }
    
    void RSGISHistoCubeFile::openFile(std::string filePath, bool fileRWAccess, int mdcElmts, hsize_t rdccNElmts, hsize_t rdccNBytes, double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize)throw(rsgis::RSGISHistoCubeException)
    {
        try
        {
            this->rwAccess = fileRWAccess;
            H5::FileAccPropList hcAccessPlist = H5::FileAccPropList(H5::FileAccPropList::DEFAULT);
            hcAccessPlist.setCache(mdcElmts, rdccNElmts, rdccNBytes, rdccW0);
            hcAccessPlist.setSieveBufSize(sieveBuf);
            hcAccessPlist.setMetaBlockSize(metaBlockSize);
            
            const H5std_string h5FilePath(filePath);
            if( fileRWAccess )
            {
                hcH5File = new H5::H5File(h5FilePath, H5F_ACC_RDWR, H5::FileCreatPropList::DEFAULT, hcAccessPlist);
                fileOpen = true;
            }
            else
            {
                hcH5File = new H5::H5File(h5FilePath, H5F_ACC_RDONLY, H5::FileCreatPropList::DEFAULT, hcAccessPlist);
                fileOpen = true;
            }
            
            // Check file is the correct type
            H5std_string strFileTypeReadVal("");
            H5::StrType strType(H5::PredType::C_S1, H5T_VARIABLE);
            try
            {
                H5::Attribute fileTypeAttribute = hcH5File->openAttribute(HC_FILE_FILETYPE);
                fileTypeAttribute.read(strType, strFileTypeReadVal);
                fileTypeAttribute.close();
            }
            catch ( H5::Exception &e)
            {
                throw rsgis::RSGISHistoCubeException("Could not read the file type attribute.");
            }
            std::string inFileType = std::string(strFileTypeReadVal.c_str());
            if(inFileType != "RSGISLibHistoCube")
            {
                throw rsgis::RSGISHistoCubeException("Input file is not of type 'RSGISLibHistoCube'.");
            }
            
            // Check Version Number
            H5std_string strVersionReadVal("");
            try
            {
                H5::Attribute fileVersionAttribute = hcH5File->openAttribute(HC_FILE_VERSION);
                fileVersionAttribute.read(strType, strVersionReadVal);
                fileVersionAttribute.close();
            }
            catch ( H5::Exception &e)
            {
                throw rsgis::RSGISHistoCubeException("Could not read the file type attribute.");
            }
            std::string inFileVersion = std::string(strVersionReadVal.c_str());
            if(inFileVersion != "1.0")
            {
                throw rsgis::RSGISHistoCubeException("Input file is not a reconsigned version.");
            }
            
            // Read metadata from file object.
            try
            {
                // Read the number of features.
                unsigned long numFeatSize[1];
                hsize_t dimsValue[1];
                dimsValue[0] = 1;
                H5::DataSpace valueDataSpace(1, dimsValue);
                H5::DataSet datasetAttSize = hcH5File->openDataSet( HC_NUM_OF_FEATS );
                datasetAttSize.read(numFeatSize, H5::PredType::NATIVE_ULONG, valueDataSpace);
                datasetAttSize.close();
                valueDataSpace.close();
                
                this->numOfFeats = numFeatSize[0];
            }
            catch (H5::Exception &e)
            {
                throw rsgis::RSGISHistoCubeException("The dataset for the number of features within the file is not present.");
            }
            
            try
            {
                // Read the dataset meta-data into memory
                H5::Group dataGroup = hcH5File->openGroup(HC_DATASETNAME_DATA);
                
                int numGrpObjs = dataGroup.getNumObjs();
                for(int i = 0; i < numGrpObjs; ++i)
                {
                    H5std_string objName = dataGroup.getObjnameByIdx(i);
                    if(dataGroup.childObjType(objName) == H5O_TYPE_DATASET)
                    {
                        H5::DataSet cubeLayerDataset = hcH5File->openDataSet(HC_DATASETNAME_DATA+"/"+objName);
                        
                        RSGISHistCubeLayerMeta *layerMeta = new RSGISHistCubeLayerMeta();
                        layerMeta->name = std::string(objName.c_str());
                        
                        H5::Attribute orderAttribute = cubeLayerDataset.openAttribute(HC_CUBELAYER_ORDER);
                        orderAttribute.read(H5::PredType::NATIVE_UINT, &layerMeta->order);
                        orderAttribute.close();
                        
                        H5::Attribute scaleAttribute = cubeLayerDataset.openAttribute(HC_CUBELAYER_SCALE);
                        scaleAttribute.read(H5::PredType::NATIVE_FLOAT, &layerMeta->scale);
                        scaleAttribute.close();
                        
                        H5::Attribute offsetAttribute = cubeLayerDataset.openAttribute(HC_CUBELAYER_OFFSET);
                        offsetAttribute.read(H5::PredType::NATIVE_FLOAT, &layerMeta->offset);
                        offsetAttribute.close();
                        
                        int tmpInt = 0;
                        H5::Attribute hasDateAttribute = cubeLayerDataset.openAttribute(HC_CUBELAYER_HAS_DATE_TIME);
                        hasDateAttribute.read(H5::PredType::NATIVE_INT, &tmpInt);
                        hasDateAttribute.close();
                        layerMeta->hasDateTime = (bool) tmpInt;
                        
                        H5std_string strDateTimeReadVal("");
                        H5::Attribute dateTimeAttribute = cubeLayerDataset.openAttribute(HC_CUBELAYER_DATE_TIME);
                        dateTimeAttribute.read(strType, strDateTimeReadVal);
                        dateTimeAttribute.close();
                        std::string dateTimeStr = std::string(strDateTimeReadVal.c_str());
                        if((dateTimeStr == "") | (dateTimeStr == "not-a-date-time"))
                        {
                            layerMeta->layerDateTime = boost::posix_time::ptime();
                        }
                        else
                        {
                            layerMeta->layerDateTime = boost::posix_time::from_iso_string(dateTimeStr);
                        }
                        
                        unsigned int numBins = 0;
                        H5::Attribute numBinsAttribute = cubeLayerDataset.openAttribute(HC_CUBELAYER_NUMBINS);
                        numBinsAttribute.read(H5::PredType::NATIVE_UINT, &numBins);
                        numBinsAttribute.close();
                        
                        int *binVals = new int[numBins];
                        H5::Attribute binsAttribute = cubeLayerDataset.openAttribute(HC_CUBELAYER_BINS);
                        binsAttribute.read(H5::PredType::NATIVE_INT, binVals);
                        binsAttribute.close();
                        
                        std::vector<int> bins;
                        for(int i = 0; i < numBins; ++i)
                        {
                            bins.push_back(binVals[i]);
                        }
                        delete[] binVals;
                        layerMeta->bins = bins;
                        
                        cubeLayers->push_back(layerMeta);
                        
                        cubeLayerDataset.close();
                    }
                }
                
            
            }
            catch (H5::Exception &e)
            {
                throw rsgis::RSGISHistoCubeException("The dataset for the number of features within the file is not present.");
            }
            
        }
        catch( H5::FileIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch ( rsgis::RSGISHistoCubeException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw rsgis::RSGISHistoCubeException(e.what());
        }

    }
    
    void RSGISHistoCubeFile::createNewFile(std::string filePath, unsigned long numFeats, int mdcElmts, hsize_t rdccNElmts, hsize_t rdccNBytes, double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize) throw(rsgis::RSGISHistoCubeException)
    {
        try
        {
            this->rwAccess = true;
            H5::FileAccPropList hcAccessPlist = H5::FileAccPropList(H5::FileAccPropList::DEFAULT);
            hcAccessPlist.setCache(mdcElmts, rdccNElmts, rdccNBytes, rdccW0);
            hcAccessPlist.setSieveBufSize(sieveBuf);
            hcAccessPlist.setMetaBlockSize(metaBlockSize);
            
            // CREATE THE HDF FILE - EXISTING FILE WILL BE TRUNCATED
            hcH5File = new H5::H5File( filePath, H5F_ACC_TRUNC, H5::FileCreatPropList::DEFAULT, hcAccessPlist);
            fileOpen = true;
            
            //////////// CREATE GLOBAL HEADER ////////////////
            H5::StrType strType(H5::PredType::C_S1, H5T_VARIABLE);
            H5::DataSpace attSpace(H5S_SCALAR);
            H5::Attribute fileTypeAtt = hcH5File->createAttribute( HC_FILE_FILETYPE, strType, attSpace);
            const H5std_string strFileTypeVal ("RSGISLibHistoCube");
            fileTypeAtt.write(strType, strFileTypeVal);
            fileTypeAtt.close();
            
            H5::Attribute fileVersionAtt = hcH5File->createAttribute( HC_FILE_VERSION, strType, attSpace);
            const H5std_string strVersionVal ("1.0");
            fileVersionAtt.write(strType, strVersionVal);
            fileVersionAtt.close();
            
            hcH5File->createGroup( HC_DATASETNAME_DATA );
            hcH5File->createGroup( HC_DATASETNAME_METADATA );
            
            unsigned long numFeatsObj[] = { numFeats };
            hsize_t numFeatsSize[] = { 1 };
            H5::DataSpace numFeatsDataSpace(1, numFeatsSize);
            H5::DataSet cubeNumFeatsDataset = hcH5File->createDataSet(HC_NUM_OF_FEATS, H5::PredType::STD_U64LE, numFeatsDataSpace);
            cubeNumFeatsDataset.write( numFeatsObj, H5::PredType::NATIVE_ULONG );
            cubeNumFeatsDataset.close();
            numFeatsDataSpace.close();
            
            this->numOfFeats = numFeats;
        }
        catch( H5::FileIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch ( rsgis::RSGISHistoCubeException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw rsgis::RSGISHistoCubeException(e.what());
        }
    }
    
    void RSGISHistoCubeFile::createDataset(std::string name, std::vector<int> bins, float scale, float offset, bool hasDateTime, boost::posix_time::ptime *layerDateTime, unsigned int chunkSize, int deflate) throw(rsgis::RSGISHistoCubeException)
    {
        try
        {
            if(!this->fileOpen)
            {
                throw rsgis::RSGISHistoCubeException("HCF file is not open.");
            }
            
            if(!this->rwAccess)
            {
                throw rsgis::RSGISHistoCubeException("HCF file was not opened in Read/Write mode.");
            }
            
            if(hasDateTime)
            {
                if(layerDateTime == NULL)
                {
                    throw rsgis::RSGISHistoCubeException("You have specified there is a time or a date associated with the layer - you must therefore provide a data/time object.");
                }
            }
            
            bool dupName = false;
            for(std::vector<RSGISHistCubeLayerMeta*>::iterator iterLayers = cubeLayers->begin(); iterLayers != cubeLayers->end(); ++iterLayers)
            {
                if((*iterLayers)->name == name)
                {
                    dupName = true;
                    break;
                }
            }
            
            if(dupName)
            {
                throw rsgis::RSGISHistoCubeException("The file already has a datasets with the specified name. The name must be unique within the file.");
            }
            
            
            RSGISHistCubeLayerMeta *layerMeta = new RSGISHistCubeLayerMeta();
            layerMeta->name = name;
            layerMeta->scale = scale;
            layerMeta->offset = offset;
            layerMeta->bins = bins;
            layerMeta->order = cubeLayers->size()+1;
            layerMeta->hasDateTime = hasDateTime;
            
            if(!hasDateTime)
            {
                layerMeta->layerDateTime = boost::posix_time::ptime();
            }
            else
            {
                layerMeta->layerDateTime = boost::posix_time::ptime(*layerDateTime);
            }
            
            unsigned int numBins = bins.size();
            std::string datasetName = "/DATA/"+name;
            
            
            hsize_t dimsDataChunkSize[] = { chunkSize, numBins };
            H5::DSetCreatPropList initParamsCubeLayer;
            initParamsCubeLayer.setChunk(2, dimsDataChunkSize);
            initParamsCubeLayer.setShuffle();
            initParamsCubeLayer.setDeflate(deflate);
            int initFillVal = 0;
            initParamsCubeLayer.setFillValue( H5::PredType::NATIVE_INT, &initFillVal);
            
            hsize_t dataLayerDims[] = { this->numOfFeats, numBins };
            H5::DataSpace cubeLayerDataSpace(2, dataLayerDims);
            H5::DataSet cubeLayerDataSet = hcH5File->createDataSet(datasetName, H5::PredType::STD_U32LE, cubeLayerDataSpace, initParamsCubeLayer);
            
            H5::DataSpace attrScalarDataSpace = H5::DataSpace(H5S_SCALAR);
            H5::Attribute scaleAttribute = cubeLayerDataSet.createAttribute(HC_CUBELAYER_SCALE, H5::PredType::IEEE_F32LE, attrScalarDataSpace);
            scaleAttribute.write(H5::PredType::NATIVE_FLOAT, &layerMeta->scale);
            scaleAttribute.close();
            
            H5::Attribute offsetAttribute = cubeLayerDataSet.createAttribute(HC_CUBELAYER_OFFSET, H5::PredType::IEEE_F32LE, attrScalarDataSpace);
            offsetAttribute.write(H5::PredType::NATIVE_FLOAT, &layerMeta->offset);
            offsetAttribute.close();
            
            H5::Attribute orderAttribute = cubeLayerDataSet.createAttribute(HC_CUBELAYER_ORDER, H5::PredType::STD_U8LE, attrScalarDataSpace);
            orderAttribute.write(H5::PredType::NATIVE_UINT, &layerMeta->order);
            orderAttribute.close();
            
            H5::Attribute numBinsAttribute = cubeLayerDataSet.createAttribute(HC_CUBELAYER_NUMBINS, H5::PredType::STD_U32LE, attrScalarDataSpace);
            numBinsAttribute.write(H5::PredType::NATIVE_UINT, &numBins);
            numBinsAttribute.close();
            
            H5::Attribute hasDateAttribute = cubeLayerDataSet.createAttribute(HC_CUBELAYER_HAS_DATE_TIME, H5::PredType::STD_U8LE, attrScalarDataSpace);
            int hasDateTimeInt = hasDateTime;
            hasDateAttribute.write(H5::PredType::NATIVE_INT, &hasDateTimeInt);
            hasDateAttribute.close();
            
            H5::StrType strType(H5::PredType::C_S1, H5T_VARIABLE);
            H5::Attribute datetimeAttribute = cubeLayerDataSet.createAttribute(HC_CUBELAYER_DATE_TIME, strType, attrScalarDataSpace);
            H5std_string strValDateTime(boost::posix_time::to_iso_string(layerMeta->layerDateTime));
            datetimeAttribute.write(strType, strValDateTime);
            datetimeAttribute.close();
            attrScalarDataSpace.close();
            
            int *binVals = new int[numBins];
            for(int i = 0; i < numBins; ++i)
            {
                binVals[i] = bins.at(i);
            }
            
            hsize_t binsDims[] = { numBins };
            H5::DataSpace attrBinsDataSpace = H5::DataSpace(1, binsDims);
            H5::Attribute binsAttribute = cubeLayerDataSet.createAttribute(HC_CUBELAYER_BINS, H5::PredType::STD_I32LE, attrBinsDataSpace);
            binsAttribute.write(H5::PredType::NATIVE_INT, binVals);
            binsAttribute.close();
            attrBinsDataSpace.close();
            delete[] binVals;
            
            cubeLayerDataSet.close();
            cubeLayerDataSpace.close();
            
            cubeLayers->push_back(layerMeta);
        }
        catch( H5::AttributeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::FileIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch ( rsgis::RSGISHistoCubeException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw rsgis::RSGISHistoCubeException(e.what());
        }
    }
    
    void RSGISHistoCubeFile::getHistoRow(std::string name, unsigned int row, unsigned int *data, unsigned int dataLen) throw(rsgis::RSGISHistoCubeException)
    {
        if(!this->fileOpen)
        {
            throw rsgis::RSGISHistoCubeException("File was not open.");
        }
        
        try
        {
            std::string cubeLayerName = HC_DATASETNAME_DATA + "/" + name;
            H5::DataSet cubeLayerDataset = hcH5File->openDataSet( cubeLayerName );
            H5::DataSpace cubeLayerDataspace = cubeLayerDataset.getSpace();
            
            hsize_t *cubeLayerDIMS = new hsize_t[2];
            int nCLDIMS = cubeLayerDataspace.getSimpleExtentDims(cubeLayerDIMS);
            
            if(nCLDIMS != 2)
            {
                throw rsgis::RSGISHistoCubeException("Cube Layer has the wrong dimensions.");
            }
            
            if(row >= cubeLayerDIMS[0])
            {
                std::cerr << "ROW = " << row << " Max. = " << cubeLayerDIMS[0] << std::endl;
                throw rsgis::RSGISHistoCubeException("Row is not within the cube layer.");
            }
            
            if(dataLen < cubeLayerDIMS[1])
            {
                std::cerr << "dataLen = " << dataLen << " Num Bins. = " << cubeLayerDIMS[1] << std::endl;
                throw rsgis::RSGISHistoCubeException("Data layer is smaller than the number of bins.");
            }
            
            // Set up dataspace for the 'data' array to read the data into
            hsize_t dataDims[2];
            dataDims[0] = 1;
            dataDims[1] = cubeLayerDIMS[1];
            H5::DataSpace readCubeLayerDataspace = H5::DataSpace(2, dataDims);
            
            // Select part of the file to read.
            hsize_t cubeLayerOffset[2];
            cubeLayerOffset[0] = row;
            cubeLayerOffset[1] = 0;
            
            hsize_t dataInDims[2];
            dataInDims[0] = 1;
            dataInDims[1] = cubeLayerDIMS[1];
            cubeLayerDataspace.selectHyperslab(H5S_SELECT_SET, dataInDims, cubeLayerOffset);
            
            // Read from file.
            cubeLayerDataset.read(data, H5::PredType::NATIVE_UINT, readCubeLayerDataspace, cubeLayerDataspace);
            
            readCubeLayerDataspace.close();
            cubeLayerDataspace.close();
            cubeLayerDataset.close();
        }
        catch( H5::AttributeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::FileIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch ( rsgis::RSGISHistoCubeException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw rsgis::RSGISHistoCubeException(e.what());
        }
    }
    
    void RSGISHistoCubeFile::setHistoRow(std::string name, unsigned int row, unsigned int *data, unsigned int dataLen) throw(rsgis::RSGISHistoCubeException)
    {
        if(!this->fileOpen)
        {
            throw rsgis::RSGISHistoCubeException("File was not open.");
        }
        
        if(!this->rwAccess)
        {
            throw rsgis::RSGISHistoCubeException("HCF file was not opened in Read/Write mode.");
        }
        
        try
        {
            std::string cubeLayerName = HC_DATASETNAME_DATA + "/" + name;
            H5::DataSet cubeLayerDataset = hcH5File->openDataSet( cubeLayerName );
            H5::DataSpace cubeLayerDataspace = cubeLayerDataset.getSpace();
            
            hsize_t *cubeLayerDIMS = new hsize_t[2];
            int nCLDIMS = cubeLayerDataspace.getSimpleExtentDims(cubeLayerDIMS);
            
            if(nCLDIMS != 2)
            {
                throw rsgis::RSGISHistoCubeException("Cube Layer has the wrong dimensions.");
            }
            
            if(row >= cubeLayerDIMS[0])
            {
                std::cerr << "ROW = " << row << " Max. = " << cubeLayerDIMS[0] << std::endl;
                throw rsgis::RSGISHistoCubeException("Row is not within the cube layer.");
            }
            
            if(dataLen < cubeLayerDIMS[1])
            {
                std::cerr << "dataLen = " << dataLen << " Num Bins. = " << cubeLayerDIMS[1] << std::endl;
                throw rsgis::RSGISHistoCubeException("Data layer is smaller than the number of bins.");
            }
            
            // Set up dataspace for the 'data' array to read the data into
            hsize_t dataDims[2];
            dataDims[0] = 1;
            dataDims[1] = cubeLayerDIMS[1];
            H5::DataSpace writeCubeLayerDataspace = H5::DataSpace(2, dataDims);
            
            // Select part of the file to read.
            hsize_t cubeLayerOffset[2];
            cubeLayerOffset[0] = row;
            cubeLayerOffset[1] = 0;
            
            hsize_t dataInDims[2];
            dataInDims[0] = 1;
            dataInDims[1] = cubeLayerDIMS[1];
            cubeLayerDataspace.selectHyperslab(H5S_SELECT_SET, dataInDims, cubeLayerOffset);
            
            // Write from file.
            cubeLayerDataset.write(data, H5::PredType::NATIVE_UINT, writeCubeLayerDataspace, cubeLayerDataspace);
            
            writeCubeLayerDataspace.close();
            cubeLayerDataspace.close();
            cubeLayerDataset.close();
        }
        catch( H5::AttributeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::FileIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch ( rsgis::RSGISHistoCubeException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw rsgis::RSGISHistoCubeException(e.what());
        }
    }
    
    void RSGISHistoCubeFile::getHistoRows(std::string name, unsigned int sRow, unsigned int eRow, unsigned int *data, unsigned int dataLen) throw(rsgis::RSGISHistoCubeException)
    {
        if(!this->fileOpen)
        {
            throw rsgis::RSGISHistoCubeException("File was not open.");
        }
        
        try
        {
            if(sRow >= eRow)
            {
                throw rsgis::RSGISHistoCubeException("The start row must be before the end row.");
            }
            unsigned int nRows = eRow - sRow;
            
            std::string cubeLayerName = HC_DATASETNAME_DATA + "/" + name;
            H5::DataSet cubeLayerDataset = hcH5File->openDataSet( cubeLayerName );
            H5::DataSpace cubeLayerDataspace = cubeLayerDataset.getSpace();
            
            hsize_t *cubeLayerDIMS = new hsize_t[2];
            int nCLDIMS = cubeLayerDataspace.getSimpleExtentDims(cubeLayerDIMS);
            
            if(nCLDIMS != 2)
            {
                throw rsgis::RSGISHistoCubeException("Cube Layer has the wrong dimensions.");
            }
            
            if(eRow >= cubeLayerDIMS[0])
            {
                std::cerr << "ROW = " << eRow << " Max. = " << cubeLayerDIMS[0] << std::endl;
                throw rsgis::RSGISHistoCubeException("Row is not within the cube layer.");
            }
            
            if((dataLen % cubeLayerDIMS[1]) != 0)
            {
                std::cerr << "dataLen = " << dataLen << " Num Bins. = " << cubeLayerDIMS[1] << std::endl;
                throw rsgis::RSGISHistoCubeException("Data layer is not a full number of rows.");
            }
            
            // Set up dataspace for the 'data' array to read the data into
            hsize_t dataDims[2];
            dataDims[0] = nRows;
            dataDims[1] = cubeLayerDIMS[1];
            H5::DataSpace readCubeLayerDataspace = H5::DataSpace(2, dataDims);
            
            // Select part of the file to read.
            hsize_t cubeLayerOffset[2];
            cubeLayerOffset[0] = sRow;
            cubeLayerOffset[1] = 0;
            
            hsize_t dataInDims[2];
            dataInDims[0] = nRows;
            dataInDims[1] = cubeLayerDIMS[1];
            cubeLayerDataspace.selectHyperslab(H5S_SELECT_SET, dataInDims, cubeLayerOffset);
            
            // Read from file.
            cubeLayerDataset.read(data, H5::PredType::NATIVE_UINT, readCubeLayerDataspace, cubeLayerDataspace);
            
            readCubeLayerDataspace.close();
            cubeLayerDataspace.close();
            cubeLayerDataset.close();
        }
        catch( H5::AttributeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::FileIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch ( rsgis::RSGISHistoCubeException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw rsgis::RSGISHistoCubeException(e.what());
        }
    }
    
    void RSGISHistoCubeFile::setHistoRows(std::string name, unsigned int sRow, unsigned int eRow, unsigned int *data, unsigned int dataLen) throw(rsgis::RSGISHistoCubeException)
    {
        if(!this->fileOpen)
        {
            throw rsgis::RSGISHistoCubeException("File was not open.");
        }
        
        if(!this->rwAccess)
        {
            throw rsgis::RSGISHistoCubeException("HCF file was not opened in Read/Write mode.");
        }
        
        try
        {
            if(sRow >= eRow)
            {
                throw rsgis::RSGISHistoCubeException("The start row must be before the end row.");
            }
            unsigned int nRows = eRow - sRow;
            
            std::string cubeLayerName = HC_DATASETNAME_DATA + "/" + name;
            H5::DataSet cubeLayerDataset = hcH5File->openDataSet( cubeLayerName );
            H5::DataSpace cubeLayerDataspace = cubeLayerDataset.getSpace();
            
            hsize_t *cubeLayerDIMS = new hsize_t[2];
            int nCLDIMS = cubeLayerDataspace.getSimpleExtentDims(cubeLayerDIMS);
            
            if(nCLDIMS != 2)
            {
                throw rsgis::RSGISHistoCubeException("Cube Layer has the wrong dimensions.");
            }
            
            if(eRow >= cubeLayerDIMS[0])
            {
                std::cerr << "ROW = " << eRow << " Max. = " << cubeLayerDIMS[0] << std::endl;
                throw rsgis::RSGISHistoCubeException("Row is not within the cube layer.");
            }
            
            if((dataLen % cubeLayerDIMS[1]) != 0)
            {
                std::cerr << "dataLen = " << dataLen << " Num Bins. = " << cubeLayerDIMS[1] << std::endl;
                throw rsgis::RSGISHistoCubeException("Data layer is not a full number of rows.");
            }
            
            // Set up dataspace for the 'data' array to read the data into
            hsize_t dataDims[2];
            dataDims[0] = nRows;
            dataDims[1] = cubeLayerDIMS[1];
            H5::DataSpace writeCubeLayerDataspace = H5::DataSpace(2, dataDims);
            
            // Select part of the file to read.
            hsize_t cubeLayerOffset[2];
            cubeLayerOffset[0] = sRow;
            cubeLayerOffset[1] = 0;
            
            hsize_t dataInDims[2];
            dataInDims[0] = nRows;
            dataInDims[1] = cubeLayerDIMS[1];
            cubeLayerDataspace.selectHyperslab(H5S_SELECT_SET, dataInDims, cubeLayerOffset);
            
            // Write from file.
            cubeLayerDataset.write(data, H5::PredType::NATIVE_UINT, writeCubeLayerDataspace, cubeLayerDataspace);
            
            writeCubeLayerDataspace.close();
            cubeLayerDataspace.close();
            cubeLayerDataset.close();
        }
        catch( H5::AttributeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::FileIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch ( rsgis::RSGISHistoCubeException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw rsgis::RSGISHistoCubeException(e.what());
        }
    }
    
    std::vector<RSGISHistCubeLayerMeta*>* RSGISHistoCubeFile::getCubeLayersList()
    {
        return this->cubeLayers;
    }
    
    unsigned long RSGISHistoCubeFile::getNumFeatures()
    {
        return this->numOfFeats;
    }
    
    void RSGISHistoCubeFile::closeFile() throw(rsgis::RSGISHistoCubeException)
    {
        this->hcH5File->close();
        delete this->hcH5File;
        this->hcH5File = NULL;
        this->fileOpen = false;
    }
    
    RSGISHistoCubeFile::~RSGISHistoCubeFile()
    {
        if(!cubeLayers->empty())
        {
            for(std::vector<RSGISHistCubeLayerMeta*>::iterator iterLayers = cubeLayers->begin(); iterLayers != cubeLayers->end(); ++iterLayers)
            {
                delete (*iterLayers);
            }
        }
        delete cubeLayers;
        
        if(this->fileOpen)
        {
            this->closeFile();
        }
    }
    
    
}}



