/*
 *  RSGISExportData2HDF.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/04/2013.
 *  Copyright 2013 RSGISLib. All rights reserved.
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

#include "RSGISExportData2HDF.h"


namespace rsgis{namespace utils{
    
    RSGISExportColumnData2HDF::RSGISExportColumnData2HDF()
    {
        
    }
    
    void RSGISExportColumnData2HDF::createFile(std::string filePath, unsigned int numCols, std::string description, H5::DataType dataType) throw(rsgis::RSGISFileException)
    {
        try
        {
            H5::Exception::dontPrint();
            
            H5::FileAccPropList dataAccessPlist = H5::FileAccPropList(H5::FileAccPropList::DEFAULT);
            dataAccessPlist.setCache(HDF5_WRITE_MDC_NELMTS, HDF5_WRITE_RDCC_NELMTS, HDF5_WRITE_RDCC_NBYTES, HDF5_WRITE_RDCC_W0);
            dataAccessPlist.setSieveBufSize(HDF5_WRITE_SIEVE_BUF);
            hsize_t metaBlockSize = HDF5_WRITE_META_BLOCKSIZE;
            dataAccessPlist.setMetaBlockSize(metaBlockSize);
            
            const H5std_string dataFilePath( filePath );
            this->dataH5File = new H5::H5File( dataFilePath, H5F_ACC_TRUNC, H5::FileCreatPropList::DEFAULT, dataAccessPlist);
            
            // Create the data folders.
            this->dataH5File->createGroup( "/DATA" );
            this->dataH5File->createGroup( "/META-DATA" );
            
            unsigned int numLinesStr = 1;
            unsigned int rankStr = 1;
            const char **wStrdata = NULL;
            
            // Create the data description.
            hsize_t	dims1Str[1];
            dims1Str[0] = numLinesStr;
            H5::DataSpace dataspaceStrAll(rankStr, dims1Str);
            H5::StrType strTypeAll(0, H5T_VARIABLE);
            
            H5::DataSet datasetDescription = this->dataH5File->createDataSet( "/META-DATA/DESCRIPTION", strTypeAll, dataspaceStrAll);
            wStrdata = new const char*[numLinesStr];
            wStrdata[0] = description.c_str();
            datasetDescription.write((void*)wStrdata, strTypeAll);
            datasetDescription.close();
            delete[] wStrdata;
            
            this->blockSize = 1000;
            int deflate = 1;
            int initFillVal = 0;
            
            hsize_t dimsDataChunk[] = { blockSize, numCols };
            H5::DSetCreatPropList initParamsData;
            initParamsData.setChunk(2, dimsDataChunk);
            initParamsData.setShuffle();
            initParamsData.setDeflate(deflate);
            initParamsData.setFillValue( H5::PredType::NATIVE_INT, &initFillVal);
            
            hsize_t initDataDims[] = { 0, numCols };
            hsize_t maxDataDims[] = { H5S_UNLIMITED, numCols };
            H5::DataSpace dataSpaceData(2, initDataDims, maxDataDims);
            
            this->columnDataSet = this->dataH5File->createDataSet("/DATA/DATA", dataType, dataSpaceData, initParamsData);
            
            this->numColsWritten = 0;
            this->numCols = numCols;            
        }
        catch (rsgis::RSGISFileException &e)
        {
            throw e;
        }
        catch (H5::FileIException &e)
        {
            throw RSGISFileException(e.getCDetailMsg());
        }
        catch (H5::DataSetIException &e)
        {
            throw RSGISFileException(e.getCDetailMsg());
        }
        catch (H5::DataSpaceIException &e)
        {
            throw RSGISFileException(e.getCDetailMsg());
        }
        catch (H5::DataTypeIException &e)
        {
            throw RSGISFileException(e.getCDetailMsg());
        }
        catch ( std::exception &e)
        {
            throw RSGISFileException(e.what());
        }
    }
    
    void RSGISExportColumnData2HDF::addDataRow(void *data, H5::DataType h5Datatype) throw(rsgis::RSGISFileException)
    {
        try
        {
            H5::Exception::dontPrint();
            
            hsize_t extendDatasetTo[2];
            extendDatasetTo[0] = this->numColsWritten + 1;
            extendDatasetTo[1] = this->numCols;
            columnDataSet.extend( extendDatasetTo );
            
            hsize_t dataOffset[2];
            dataOffset[0] = this->numColsWritten;
            dataOffset[1] = 0;
            hsize_t dataDims[2];
            dataDims[0] = 1;
            dataDims[1] = numCols;
            
            H5::DataSpace colWriteDataSpace = columnDataSet.getSpace();
            colWriteDataSpace.selectHyperslab(H5S_SELECT_SET, dataDims, dataOffset);
            H5::DataSpace newDataspace = H5::DataSpace(2, dataDims);
            
            columnDataSet.write(data, h5Datatype, newDataspace, colWriteDataSpace);
            
            ++this->numColsWritten;
        }
        catch (rsgis::RSGISFileException &e)
        {
            throw e;
        }
        catch (H5::FileIException &e)
        {
            throw RSGISFileException(e.getCDetailMsg());
        }
        catch (H5::DataSetIException &e)
        {
            throw RSGISFileException(e.getCDetailMsg());
        }
        catch (H5::DataSpaceIException &e)
        {
            throw RSGISFileException(e.getCDetailMsg());
        }
        catch (H5::DataTypeIException &e)
        {
            throw RSGISFileException(e.getCDetailMsg());
        }
        catch ( std::exception &e)
        {
            throw RSGISFileException(e.what());
        }
    }
    
    void RSGISExportColumnData2HDF::close()
    {
        this->columnDataSet.close();
        this->dataH5File->flush(H5F_SCOPE_GLOBAL);
        this->dataH5File->close();
        delete this->dataH5File;
    }
		
    RSGISExportColumnData2HDF::~RSGISExportColumnData2HDF()
    {
        
    }
    
}}




