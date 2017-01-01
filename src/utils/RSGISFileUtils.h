/*
 *  RSGISFileUtils.h
 *  
 *
 *  Created by Pete Bunting on 06/04/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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

#ifndef RSGISFileUtils_H
#define RSGISFileUtils_H

#include <vector>
#include <list>
#include <iostream>
#include <string>

#include <boost/filesystem.hpp>

#include "common/RSGISFileException.h"

namespace rsgis{namespace utils{
        
    class DllExport RSGISFileUtils
    {
        public: 
            RSGISFileUtils();
            void getDIRList(std::string dir, std::list<std::string> *files) throw(RSGISFileException);
            void getDIRList(std::string dir, std::vector<std::string> *files) throw(RSGISFileException);
            void getDIRList(std::string dir, std::string ext, std::list<std::string> *files, bool withpath) throw(RSGISFileException);
            void getDIRList(std::string dir, std::string ext, std::vector<std::string> *files, bool withpath) throw(RSGISFileException);
            std::string* getDIRList(std::string dir, std::string ext, int *numFiles, bool withpath) throw(RSGISFileException);
            std::string* getFilesInDIRWithName(std::string dir, std::string name, int *numFiles) throw(RSGISFileException);
            std::string getFileNameNoExtension(std::string filepath);
            std::string getFileName(std::string filepath);
            std::string removeExtension(std::string filepath);
            std::string getExtension(std::string filepath);
            std::string getFileDirectoryPath(std::string filepath);
            bool checkFilePresent(std::string file);
            ~RSGISFileUtils();
    };
    
}}

#endif
