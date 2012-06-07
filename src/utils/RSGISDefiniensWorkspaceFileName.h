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
#include "math/RSGISMathsUtils.h"
#include "utils/RSGISFileUtils.h"

namespace rsgis 
{
	namespace utils
	{
        using namespace std;
        using namespace rsgis::math;
        
		class RSGISDefiniensWorkspaceFileName
			{
			public:
				RSGISDefiniensWorkspaceFileName(string filename);
				const string getFileName() const;
				string getFileNameWithPath();
				string getFileNameNoExtension();
				int getTile();
				int getVersion();
				string getBaseName();
				void setOldVersion(bool oldVersion);
				bool getOldVersion() const;
				friend ostream& operator<<(ostream& ostr, const RSGISDefiniensWorkspaceFileName& name);
				ostream& operator<<(ostream& ostr);
				bool operator==(RSGISDefiniensWorkspaceFileName name) const;
				bool operator!=(RSGISDefiniensWorkspaceFileName name) const;
				bool operator>(RSGISDefiniensWorkspaceFileName name) const;
				bool operator<(RSGISDefiniensWorkspaceFileName name) const;
				bool operator>=(RSGISDefiniensWorkspaceFileName name) const;
				bool operator<=(RSGISDefiniensWorkspaceFileName name) const;
				virtual ~RSGISDefiniensWorkspaceFileName();
			private:
				void parseFileName(string filename);
				string path;
				string start;
				string tileNumStr;
				int tile;
				int version;
				string extension;
				bool oldVersion;
			};
	}
}

#endif





