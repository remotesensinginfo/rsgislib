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

#include <dirent.h>
#include <errno.h>
#include <vector>
#include <list>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include "common/RSGISFileException.h"

namespace rsgis 
{
	namespace utils
	{
        using namespace std;
        
		class RSGISFileUtils
		{
			public: 
				RSGISFileUtils();
				void getDIRList(string dir, list<string> *files) throw(RSGISFileException);
				void getDIRList(string dir, vector<string> *files) throw(RSGISFileException);
				void getDIRList(string dir, string ext, list<string> *files, bool withpath) throw(RSGISFileException);
				void getDIRList(string dir, string ext, vector<string> *files, bool withpath) throw(RSGISFileException);
				string* getDIRList(string dir, string ext, int *numFiles, bool withpath) throw(RSGISFileException);
				string* getFilesInDIRWithName(string dir, string name, int *numFiles) throw(RSGISFileException);
				string getFileNameNoExtension(string filepath);
				string getFileName(string filepath);
				string removeExtension(string filepath);
				string getExtension(string filepath);
				string getFileDirectoryPath(string filepath);
				bool checkFilePresent(string file);
				~RSGISFileUtils();
		};
	}
}

#endif
