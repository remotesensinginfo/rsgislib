/*
 *  RSGISDefiniensWorkspaceFileName.cpp
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

#include "RSGISDefiniensWorkspaceFileName.h"

namespace rsgis{namespace utils{
	
	RSGISDefiniensWorkspaceFileName::RSGISDefiniensWorkspaceFileName(std::string filename) 
	{
		this->parseFileName(filename);
		this->oldVersion = false;
	}
	
	const std::string RSGISDefiniensWorkspaceFileName::getFileName() const
	{
		rsgis::math::RSGISMathsUtils mathsUtils;
		std::string filename = start + std::string(".tile") + tileNumStr + std::string(".v") + mathsUtils.inttostring(version) + extension;
		return filename;
	}
	
	std::string RSGISDefiniensWorkspaceFileName::getFileNameWithPath()
	{
		rsgis::math::RSGISMathsUtils mathsUtils;
		std::string filename = path + start + std::string(".tile") + tileNumStr + std::string(".v") + mathsUtils.inttostring(version) + extension;
		return filename;
	}
	
	std::string RSGISDefiniensWorkspaceFileName::getFileNameNoExtension()
	{
		rsgis::math::RSGISMathsUtils mathsUtils;
		std::string filename = start + std::string(".tile") + tileNumStr + std::string(".v") + mathsUtils.inttostring(version);
		return filename;
	}
	
	int RSGISDefiniensWorkspaceFileName::getTile()
	{
		return this->tile;
	}
	
	int RSGISDefiniensWorkspaceFileName::getVersion()
	{
		return this->version;
	}
	
	std::string RSGISDefiniensWorkspaceFileName::getBaseName()
	{
		return this->start;
	}
	
	void RSGISDefiniensWorkspaceFileName::setOldVersion(bool oldVersion)
	{
		this->oldVersion = oldVersion;
	}
	
	bool RSGISDefiniensWorkspaceFileName::getOldVersion() const
	{
		return this->oldVersion;
	}
	
    std::ostream& operator<<(std::ostream& ostr, const RSGISDefiniensWorkspaceFileName& name)
	{
		ostr << name.getFileName();
		if(name.getOldVersion())
		{
			ostr << "(OLD VERSION)";
		}
		return ostr;
	}
	
	std::ostream& RSGISDefiniensWorkspaceFileName::operator<<(std::ostream& ostr)
	{
		ostr << start << ".tile" << tile << ".v" << version << "." << extension;
		if(oldVersion)
		{
			ostr << "(OLD VERSION)";
		}
		return ostr;
	}
	
	bool RSGISDefiniensWorkspaceFileName::operator==(RSGISDefiniensWorkspaceFileName name) const
	{
		if(start != name.start)
		{
			return false;
		}
		else if(extension != name.extension)
		{
			return false;
		}
		else if(tile != name.tile)
		{
			return false;
		}
		else if(version != name.version)
		{
			return false;
		}
		
		return true;
	}
	
	bool RSGISDefiniensWorkspaceFileName::operator!=(RSGISDefiniensWorkspaceFileName name) const
	{
		if(start != name.start)
		{
			return true;
		}
		else if(extension != name.extension)
		{
			return true;
		}
		else if(tile != name.tile)
		{
			return true;
		}
		else if(version != name.version)
		{
			return true;
		}
		
		return false;
	}
	
	bool RSGISDefiniensWorkspaceFileName::operator>(RSGISDefiniensWorkspaceFileName name) const
	{
		if(tile > name.tile)
		{
			return true;
		}
		else if(tile == name.tile & version > name.version)
		{
			return true;
		}
		
		return false;
	}
	
	bool RSGISDefiniensWorkspaceFileName::operator<(RSGISDefiniensWorkspaceFileName name) const
	{
		if(tile < name.tile)
		{
			return true;
		}
		else if(tile == name.tile & version < name.version)
		{
			return true;
		}
		
		return false;
	}
	
	bool RSGISDefiniensWorkspaceFileName::operator>=(RSGISDefiniensWorkspaceFileName name) const
	{
		if(tile == name.tile & version == name.version)
		{
		   return true;
		}
		else if(tile > name.tile)
		{
			return true;
		}
		else if(tile == name.tile & version > name.version)
		{
			return true;
		}
		return false;
	}
	
	bool RSGISDefiniensWorkspaceFileName::operator<=(RSGISDefiniensWorkspaceFileName name) const
	{
		if(tile == name.tile & version == name.version)
		{
			return true;
		}
		else if(tile < name.tile)
		{
			return true;
		}
		else if(tile == name.tile & version < name.version)
		{
			return true;
		}
		
		return false;
	}
	
	void RSGISDefiniensWorkspaceFileName::parseFileName(std::string filename)
	{
		RSGISFileUtils fileUtils;
        rsgis::math::RSGISMathsUtils mathsUtils;
		this->path = fileUtils.getFileDirectoryPath(filename);
		this->extension = fileUtils.getExtension(filename);
		std::string filenameNoExtension = fileUtils.getFileNameNoExtension(filename);
		std::string versionStr = fileUtils.getExtension(filenameNoExtension);
		std::string versionNumStr = versionStr.substr(2);
		this->version = mathsUtils.strtoint(versionNumStr);
		std::string filenameNoVersion = fileUtils.getFileNameNoExtension(filenameNoExtension);
		std::string tileStr = fileUtils.getExtension(filenameNoVersion);
		this->tileNumStr = tileStr.substr(5);
		this->tile = mathsUtils.strtoint(tileNumStr);
		this->start = fileUtils.getFileNameNoExtension(filenameNoVersion);
	}
	
	RSGISDefiniensWorkspaceFileName::~RSGISDefiniensWorkspaceFileName()
	{
		
	}
	
}} //rsgis::utils


