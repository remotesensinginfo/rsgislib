/*
 *  RSGISFileUtils.cpp
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

#include "RSGISFileUtils.h"

namespace rsgis{namespace utils{

RSGISFileUtils::RSGISFileUtils()
{
	
}

	void RSGISFileUtils::getDIRList(std::string dir, std::list<std::string> *files) throw(RSGISFileException)
	{
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir.c_str())) == NULL) 
		{
			std::string message = std::string("Could not open ") + dir;
			throw RSGISFileException(message);
		}
		
		while ((dirp = readdir(dp)) != NULL) 
		{
			files->push_back(std::string(dirp->d_name));
		}
		closedir(dp);
	}
	
	void RSGISFileUtils::getDIRList(std::string dir, std::vector<std::string> *files) throw(RSGISFileException)
	{
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir.c_str())) == NULL) 
		{
			std::string message = std::string("Could not open ") + dir;
			throw RSGISFileException(message);
		}
		
		while ((dirp = readdir(dp)) != NULL) 
		{
			files->push_back(std::string(dirp->d_name));
		}
		closedir(dp);
	}
	
	void RSGISFileUtils::getDIRList(std::string dir, std::string ext, std::list<std::string> *files, bool withpath) throw(RSGISFileException)
	{
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir.c_str())) == NULL) 
		{
			std::string message = std::string("Could not open ") + dir;
			throw RSGISFileException(message);
		}
		
		std::string filename = "";
		
		while ((dirp = readdir(dp)) != NULL) 
		{
			filename = std::string(dirp->d_name);
			if(this->getExtension(filename) == ext)
			{
				if(withpath)
				{
					filename = dir + filename;
				}
				files->push_back(filename);
			}
		}
		closedir(dp);		
	}
	
	void RSGISFileUtils::getDIRList(std::string dir, std::string ext, std::vector<std::string> *files, bool withpath) throw(RSGISFileException)
	{
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir.c_str())) == NULL) 
		{
			std::string message = std::string("Could not open ") + dir;
			throw RSGISFileException(message);
		}
		
		std::string filename = "";
		
		while ((dirp = readdir(dp)) != NULL) 
		{
			filename = std::string(dirp->d_name);
			if(this->getExtension(filename) == ext)
			{
				if(withpath)
				{
					filename = dir + filename;
				}
				files->push_back(filename);
			}
		}
		closedir(dp);		
	}
	
	std::string* RSGISFileUtils::getDIRList(std::string dir, std::string ext, int *numFiles, bool withpath) throw(RSGISFileException)
	{
		std::vector<std::string> *files = new std::vector<std::string>();
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir.c_str())) == NULL) 
		{
			std::string message = std::string("Could not open ") + dir;
			throw RSGISFileException(message);
		}
		
		std::string filename = "";
		
		while ((dirp = readdir(dp)) != NULL) 
		{
			filename = std::string(dirp->d_name);
			if(this->getExtension(filename) == ext)
			{
				if(withpath)
				{
					filename = dir + filename;
				}
				files->push_back(filename);
			}
		}
		closedir(dp);
		
		*numFiles = files->size();
		std::string *outputFiles = new std::string[*numFiles];
		for(int i = 0; i < *numFiles; i++)
		{
			outputFiles[i] = dir + files->at(i);
		}
		delete files;
		
		return outputFiles;
	}
	
	std::string* RSGISFileUtils::getFilesInDIRWithName(std::string dir, std::string name, int *numFiles) throw(RSGISFileException)
	{
		std::vector<std::string> *files = new std::vector<std::string>();
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir.c_str())) == NULL) 
		{
			std::string message = std::string("Could not open ") + dir;
			throw RSGISFileException(message);
		}
		
		std::string filename = "";
		
		while ((dirp = readdir(dp)) != NULL) 
		{
			filename = std::string(dirp->d_name);
			//cout << "Filename (" << name << "): " << filename << " (" << this->getFileNameNoExtension(filename) << ")"<< endl;
			if(this->getFileNameNoExtension(filename) == name)
			{
				files->push_back(filename);
			}
		}
		closedir(dp);
		
		*numFiles = files->size();
		std::string *outputFiles = new std::string[*numFiles];
		for(int i = 0; i < *numFiles; i++)
		{
			outputFiles[i] = dir + files->at(i);
		}
		delete files;
		
		return outputFiles;
	}
	
	std::string RSGISFileUtils::getFileName(std::string filepath)
	{
		//cout << filepath << endl;
		int strSize = filepath.size();
		int lastSlash = 0;
		for(int i = 0; i < strSize; i++)
		{
			if(filepath.at(i) == '/')
			{
				lastSlash = i + 1;
			}
		}
		std::string filename = filepath.substr(lastSlash);
		//cout << filename << endl;
		return filename;	
	}
	
	std::string RSGISFileUtils::getFileNameNoExtension(std::string filepath)
	{
		//cout << filepath << endl;
		int strSize = filepath.size();
		int lastSlash = 0;
		for(int i = 0; i < strSize; i++)
		{
			if(filepath.at(i) == '/')
			{
				lastSlash = i + 1;
			}
		}
		std::string filename = filepath.substr(lastSlash);
		//cout << filename << endl;
		
		strSize = filename.size();
		int lastpt = 0;
		for(int i = 0; i < strSize; i++)
		{
			if(filename.at(i) == '.')
			{
				lastpt = i;
			}
		}
		
		std::string layerName = filename.substr(0, lastpt);
		//cout << layerName << endl;
		return layerName;	
	}
	
	std::string RSGISFileUtils::removeExtension(std::string filepath)
	{
		int strSize = filepath.size();
		int lastpt = 0;
		for(int i = 0; i < strSize; i++)
		{
			if(filepath.at(i) == '.')
			{
				lastpt = i;
			}
		}
		
		std::string layerName = filepath.substr(0, lastpt);
		//cout << layerName << endl;
		return layerName;	
	}
	
	std::string RSGISFileUtils::getExtension(std::string filepath)
	{
		int strSize = filepath.size();
		int lastpt = 0;
		for(int i = 0; i < strSize; i++)
		{
			if(filepath.at(i) == '.')
			{
				lastpt = i;
			}
		}
		
		std::string extension = filepath.substr(lastpt);
		//cout << layerName << endl;
		return extension;	
	}
	
	std::string RSGISFileUtils::getFileDirectoryPath(std::string filepath)
	{
		int strSize = filepath.size();
		int lastSlash = 0;
		for(int i = 0; i < strSize; i++)
		{
			if(filepath.at(i) == '/')
			{
				lastSlash = i + 1;
			}
		}
		std::string path = filepath.substr(0, lastSlash);
		//cout << path << endl;
		return path;	
	}
	
	bool RSGISFileUtils::checkFilePresent(std::string file)
	{
		struct stat stFileInfo; 
		bool blnReturn; 
		int intStat; 
		
		intStat = stat(file.c_str(), &stFileInfo); 
		if(intStat == 0) 
		{  
			blnReturn = true; 
		} 
		else 
		{ 
			blnReturn = false; 
		}
		
		return blnReturn; 
	}

RSGISFileUtils::~RSGISFileUtils()
{
	
}
	
}} //rsgis::utils
