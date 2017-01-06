/*
 *  RSGISCmdParent.h
 *
 *
 *  Created by Pete Bunting on 29/04/2013.
 *  Copyright 2013 RSGISLib.
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

#ifndef RSGISCmdParent_H
#define RSGISCmdParent_H

#include <iostream>
#include <string>

#include "common/RSGISCommons.h"

#include "gdal_priv.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_cmds_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{ namespace cmds {

    inline GDALDataType RSGIS_to_GDAL_Type( rsgis::RSGISLibDataType rsgisType )
    {
        GDALDataType gdalType = GDT_Unknown;
        switch( rsgisType )
        {
            case rsgis::rsgis_8int:
            case rsgis::rsgis_8uint:
                gdalType = GDT_Byte;
                break;
            case rsgis::rsgis_16int:
                gdalType = GDT_Int16;
                break;
            case rsgis::rsgis_32int:
                gdalType = GDT_Int32;
                break;
            case rsgis::rsgis_16uint:
                gdalType = GDT_UInt16;
                break;
            case rsgis::rsgis_32uint:
                gdalType = GDT_UInt32;
                break;
            case rsgis::rsgis_32float:
                gdalType = GDT_Float32;
                break;
            case rsgis::rsgis_64float:
                gdalType = GDT_Float64;
                break;
            default:
                gdalType = GDT_Unknown;
                break;
        }
        return gdalType;
    };
    
    // function for converting a GDAL type to a rsgis type
    inline rsgis::RSGISLibDataType GDAL_to_RSGIS_Type( GDALDataType gdalType )
    {
        rsgis::RSGISLibDataType rsgisType = rsgis::rsgis_undefined;
        switch( gdalType )
        {
            case GDT_Byte:
                rsgisType = rsgis::rsgis_8uint;
                break;
            case GDT_Int16:
                rsgisType = rsgis::rsgis_16int;
                break;
            case GDT_Int32:
                rsgisType = rsgis::rsgis_32int;
                break;
            case GDT_UInt16:
                rsgisType = rsgis::rsgis_16uint;
                break;
            case GDT_UInt32:
                rsgisType = rsgis::rsgis_32uint;
                break;
            case GDT_Float32:
                rsgisType = rsgis::rsgis_32float;
                break;
            case GDT_Float64:
                rsgisType = rsgis::rsgis_64float;
                break;
            default:
                rsgisType = rsgis::rsgis_undefined;
                break;
        }
        return rsgisType;
    };
    
}}


#endif

