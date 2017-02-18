/*
 *  RSGISHistoCubeFileIO.h
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

#ifndef RSGISHistoCubeFileIO_H
#define RSGISHistoCubeFileIO_H


// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
#ifdef rsgis_histocube_EXPORTS
#define DllExport   __declspec( dllexport )
#else
#define DllExport   __declspec( dllimport )
#endif
#else
#define DllExport
#endif


// MSVC 2008 uses different names.... COPIED FROM LIBKEA
#ifdef _MSC_VER
    #if _MSC_VER >= 1600
        #include <stdint.h>
    #else
        typedef __int8              int8_t;
        typedef __int16             int16_t;
        typedef __int32             int32_t;
        typedef __int64             int64_t;
        typedef unsigned __int8     uint8_t;
        typedef unsigned __int16    uint16_t;
        typedef unsigned __int32    uint32_t;
        typedef unsigned __int64    uint64_t;
    #endif
#else
    #include <stdint.h>
#endif

#include <string>
#include <iostream>
#include <vector>
#include "common/RSGISHistoCubeException.h"

#include "H5Cpp.h"

namespace rsgis {namespace histocube{
    
    static const int HC_MDC_NELMTS( 0 ); // 0
    static const hsize_t  HC_RDCC_NELMTS( 512 ); // 512
    static const hsize_t  HC_RDCC_NBYTES( 1048576 ); // 1048576
    static const double HC_RDCC_W0( 0.75 ); // 0.75
    static const hsize_t  HC_SIEVE_BUF( 65536 ); // 65536
    static const hsize_t  HC_META_BLOCKSIZE( 2048 ); // 2048
    static const unsigned int HC_DEFLATE( 1 ); // 1
    
    static const std::string HC_FILE_ATT_FILETYPE( "FILETYPE" );
    static const std::string HC_FILE_ATT_VERSION( "VERSION" );
    
    static const std::string HC_DATASETNAME_DATA( "/DATA" );
    static const std::string HC_DATASETNAME_METADATA( "/METADATA" );
    
    
    class DllExport RSGISHistoCubeFile
    {
    public:
        RSGISHistoCubeFile();
        virtual void openFile(std::string filePath, bool rwAccess, int mdcElmts=HC_MDC_NELMTS, hsize_t rdccNElmts=HC_RDCC_NELMTS, hsize_t rdccNBytes=HC_RDCC_NBYTES, double rdccW0=HC_RDCC_W0, hsize_t sieveBuf=HC_SIEVE_BUF, hsize_t metaBlockSize=HC_META_BLOCKSIZE) throw(rsgis::RSGISHistoCubeException);
        virtual void createNewFile(std::string filePath, int mdcElmts=HC_MDC_NELMTS, hsize_t rdccNElmts=HC_RDCC_NELMTS, hsize_t rdccNBytes=HC_RDCC_NBYTES, double rdccW0=HC_RDCC_W0, hsize_t sieveBuf=HC_SIEVE_BUF, hsize_t metaBlockSize=HC_META_BLOCKSIZE) throw(rsgis::RSGISHistoCubeException);
        virtual void createDataset(std::string name, unsigned int lowRange, unsigned int upRange, int scale, int offset) throw(rsgis::RSGISHistoCubeException);
        virtual void closeFile() throw(rsgis::RSGISHistoCubeException);
        virtual ~RSGISHistoCubeFile();
    protected:
        bool fileOpen;
        bool rwAccess;
        H5::H5File *hcH5File;
    };
    
    
    
    
    
}}

#endif

