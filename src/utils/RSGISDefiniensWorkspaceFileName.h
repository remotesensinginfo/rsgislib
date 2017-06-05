/*
 *  RSGISDefiniensWorkspaceFileName.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/02/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
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

#ifndef RSGISDefiniensWorkspaceFileName_H
#define RSGISDefiniensWorkspaceFileName_H

#include <iostream>
#include <string>

#include "math/RSGISMathsUtils.h"
#include "utils/RSGISFileUtils.h"

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
        
    class DllExport RSGISDefiniensWorkspaceFileName
        {
        public:
            RSGISDefiniensWorkspaceFileName(std::string filename);
            const std::string getFileName() const;
            std::string getFileNameWithPath();
            std::string getFileNameNoExtension();
            int getTile();
            int getVersion();
            std::string getBaseName();
            void setOldVersion(bool oldVersion);
            bool getOldVersion() const;
            friend std::ostream& operator<<(std::ostream& ostr, const RSGISDefiniensWorkspaceFileName& name);
            std::ostream& operator<<(std::ostream& ostr);
            bool operator==(RSGISDefiniensWorkspaceFileName name) const;
            bool operator!=(RSGISDefiniensWorkspaceFileName name) const;
            bool operator>(RSGISDefiniensWorkspaceFileName name) const;
            bool operator<(RSGISDefiniensWorkspaceFileName name) const;
            bool operator>=(RSGISDefiniensWorkspaceFileName name) const;
            bool operator<=(RSGISDefiniensWorkspaceFileName name) const;
            virtual ~RSGISDefiniensWorkspaceFileName();
        private:
            void parseFileName(std::string filename);
            std::string path;
            std::string start;
            std::string tileNumStr;
            int tile;
            int version;
            std::string extension;
            bool oldVersion;
        };
    
}}

#endif





