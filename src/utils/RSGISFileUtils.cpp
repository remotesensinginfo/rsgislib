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

	void RSGISFileUtils::getDIRList(string dir, list<string> *files) throw(RSGISFileException)
	{
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir.c_str())) == NULL) 
		{
			string message = string("Could not open ") + dir;
			throw RSGISFileException(message);
		}
		
		while ((dirp = readdir(dp)) != NULL) 
		{
			files->push_back(std::string(dirp->d_name));
		}
		closedir(dp);
	}
	
	void RSGISFileUtils::getDIRList(string dir, vector<string> *files) throw(RSGISFileException)
	{
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir.c_str())) == NULL) 
		{
			string message = string("Could not open ") + dir;
			throw RSGISFileException(message);
		}
		
		while ((dirp = readdir(dp)) != NULL) 
		{
			files->push_back(std::string(dirp->d_name));
		}
		closedir(dp);
	}
	
	void RSGISFileUtils::getDIRList(string dir, string ext, list<string> *files, bool withpath) throw(RSGISFileException)
	{
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir.c_str())) == NULL) 
		{
			string message = string("Could not open ") + dir;
			throw RSGISFileException(message);
		}
		
		string filename = "";
		
		while ((dirp = readdir(dp)) != NULL) 
		{
			filename = string(dirp->d_name);
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
	
	void RSGISFileUtils::getDIRList(string dir, string ext, vector<string> *files, bool withpath) throw(RSGISFileException)
	{
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir.c_str())) == NULL) 
		{
			string message = string("Could not open ") + dir;
			throw RSGISFileException(message);
		}
		
		string filename = "";
		
		while ((dirp = readdir(dp)) != NULL) 
		{
			filename = string(dirp->d_name);
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
	
	string* RSGISFileUtils::getDIRList(string dir, string ext, int *numFiles, bool withpath) throw(RSGISFileException)
	{
		vector<string> *files = new vector<string>();
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir.c_str())) == NULL) 
		{
			string message = string("Could not open ") + dir;
			throw RSGISFileException(message);
		}
		
		string filename = "";
		
		while ((dirp = readdir(dp)) != NULL) 
		{
			filename = string(dirp->d_name);
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
		string *outputFiles = new string[*numFiles];
		for(int i = 0; i < *numFiles; i++)
		{
			outputFiles[i] = dir + files->at(i);
		}
		delete files;
		
		return outputFiles;
	}
	
	string* RSGISFileUtils::getFilesInDIRWithName(string dir, string name, int *numFiles) throw(RSGISFileException)
	{
		vector<string> *files = new vector<string>();
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir.c_str())) == NULL) 
		{
			string message = string("Could not open ") + dir;
			throw RSGISFileException(message);
		}
		
		string filename = "";
		
		while ((dirp = readdir(dp)) != NULL) 
		{
			filename = string(dirp->d_name);
			//cout << "Filename (" << name << "): " << filename << " (" << this->getFileNameNoExtension(filename) << ")"<< endl;
			if(this->getFileNameNoExtension(filename) == name)
			{
				files->push_back(filename);
			}
		}
		closedir(dp);
		
		*numFiles = files->size();
		string *outputFiles = new string[*numFiles];
		for(int i = 0; i < *numFiles; i++)
		{
			outputFiles[i] = dir + files->at(i);
		}
		delete files;
		
		return outputFiles;
	}
	
	string RSGISFileUtils::getFileName(string filepath)
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
		string filename = filepath.substr(lastSlash);
		//cout << filename << endl;
		return filename;	
	}
	
	string RSGISFileUtils::getFileNameNoExtension(string filepath)
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
		string filename = filepath.substr(lastSlash);
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
		
		string layerName = filename.substr(0, lastpt);
		//cout << layerName << endl;
		return layerName;	
	}
	
	string RSGISFileUtils::removeExtension(string filepath)
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
		
		string layerName = filepath.substr(0, lastpt);
		//cout << layerName << endl;
		return layerName;	
	}
	
	string RSGISFileUtils::getExtension(string filepath)
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
		
		string extension = filepath.substr(lastpt);
		//cout << layerName << endl;
		return extension;	
	}
	
	string RSGISFileUtils::getFileDirectoryPath(string filepath)
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
		string path = filepath.substr(0, lastSlash);
		//cout << path << endl;
		return path;	
	}
	
	bool RSGISFileUtils::checkFilePresent(string file)
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
