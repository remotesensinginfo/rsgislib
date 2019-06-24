/*
 *  RSGISExportData2HDF.h
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

#ifndef RSGISExportData2HDF_H
#define RSGISExportData2HDF_H

#include <string>
#include <iostream>

#include <boost/cstdint.hpp>

#include "common/RSGISFileException.h"

#include "H5Cpp.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_utils_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace utils{
    
    static const int HDF5_READ_MDC_NELMTS( 0 ); //0
    static const hsize_t HDF5_READ_RDCC_NELMTS( 512 ); // 512
    static const hsize_t HDF5_READ_RDCC_NBYTES( 1048576 ); // 1048576
    static const double HDF5_READ_RDCC_W0( 0.75 );
    static const hsize_t HDF5_READ_SIEVE_BUF( 65536 ); // 65536
    static const hsize_t HDF5_READ_META_BLOCKSIZE( 2048 ); // 2048
    
    static const int HDF5_WRITE_MDC_NELMTS( 0 );
    static const hsize_t  HDF5_WRITE_RDCC_NELMTS( 512 );
    static const hsize_t  HDF5_WRITE_RDCC_NBYTES( 1048576 );
    static const double HDF5_WRITE_RDCC_W0( 0.75 );
    static const hsize_t  HDF5_WRITE_SIEVE_BUF( 65536 );
    static const hsize_t  HDF5_WRITE_META_BLOCKSIZE( 2048 );
    static const unsigned int HDF5_WRITE_DEFLATE( 1 );
    static const hsize_t HDF5_WRITE_CHUNK_SIZE( 250 ); //100
    
	class DllExport RSGISExportColumnData2HDF
	{
	public:
		RSGISExportColumnData2HDF();
        void createFile(std::string filePath, unsigned int numCols, std::string description, H5::DataType dataType);
        void addDataRow(void *data, H5::DataType h5Datatype);
        void close();
		~RSGISExportColumnData2HDF();
    protected:
        H5::H5File *dataH5File;
        H5::DataSet columnDataSet;
        unsigned int numCols;
        unsigned int blockSize;
        unsigned int numColsWritten;
	};
    
    class DllExport RSGISReadHDFColumnData
    {
    public:
        RSGISReadHDFColumnData();
        void openFile(std::string filePath);
        unsigned int getNumRows();
        unsigned int getNumCols();
        void getDataRows(void *data, unsigned int nColsData, unsigned int nRowsData, H5::DataType h5Datatype, unsigned int nRowsOff, unsigned int nRowsRead);
        void close();
        ~RSGISReadHDFColumnData();
    protected:
        H5::H5File *dataH5File;
        H5::DataSet columnDataSet;
        unsigned int numCols;
        unsigned int blockSize;
        unsigned int numColsWritten;
        bool fileOpen;
    };

}}

#endif



