/*
 *  RSGISCommons.h
 *
 *  RSGIS Common
 *
 *	A class providing the base Exception for the
 *	the RSGIS library
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

#ifndef RSGISCommons_H
#define RSGISCommons_H

#include <iostream>
#include <string>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _WIN32
    #define DllExport   __declspec( dllexport )
#else
    #define DllExport
#endif

namespace rsgis
{
    enum RSGISLibDataType
    {
        rsgis_undefined = 0,
        rsgis_8int = 1,
        rsgis_16int = 2,
        rsgis_32int = 3,
        rsgis_64int = 4,
        rsgis_8uint = 5,
        rsgis_16uint = 6,
        rsgis_32uint = 7,
        rsgis_64uint = 8,
        rsgis_32float = 9,
        rsgis_64float = 10
    };
    
    inline std::string getDataTypeAsStr(RSGISLibDataType dataType)
    {
        std::string strDT = "Unknown";
        
        if(dataType == rsgis_8int)
        {
            strDT = "Integer 8 bit";
        }
        else if(dataType == rsgis_16int)
        {
            strDT = "Integer 16 bit";
        }
        else if(dataType == rsgis_32int)
        {
            strDT = "Integer 32 bit";
        }
        else if(dataType == rsgis_64int)
        {
            strDT = "Integer 64 bit";
        }
        else if(dataType == rsgis_8uint)
        {
            strDT = "Unsigned Integer 8 bit";
        }
        else if(dataType == rsgis_16uint)
        {
            strDT = "Unsigned Integer 16 bit";
        }
        else if(dataType == rsgis_32uint)
        {
            strDT = "Unsigned Integer 32 bit";
        }
        else if(dataType == rsgis_64uint)
        {
            strDT = "Unsigned Integer 64 bit";
        }
        else if(dataType == rsgis_32float)
        {
            strDT = "Float 32 bit";
        }
        else if(dataType == rsgis_64float)
        {
            strDT = "Float 64 bit";
        }
        else
        {
            strDT = "Unknown";
        }
        
        return strDT;
    }
    
}


#endif

