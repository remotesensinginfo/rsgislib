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

    void RSGISFileUtils::getDIRList(std::string dir, std::list<std::string> *files)
    {
        boost::filesystem::path inDIR(dir);
        try
        {
            if(boost::filesystem::exists(inDIR))
            {
                if (boost::filesystem::is_directory(inDIR))
                {
                    boost::filesystem::directory_iterator endDirIter;
                    for(boost::filesystem::directory_iterator x(inDIR); x != endDirIter; ++x)
                    {
                        files->push_back((*x).path().string());
                    }
                }
                else
                {
                    throw RSGISFileException("Input path is not a directory.");
                }
            }
            else
            {
                throw RSGISFileException("Input path does not exist.");
            }
        }
        catch (const boost::filesystem::filesystem_error& e)
        {
            throw RSGISFileException(e.what());
        }
    }

    void RSGISFileUtils::getDIRList(std::string dir, std::vector<std::string> *files)
    {
        boost::filesystem::path inDIR(dir);
        try
        {
            if(boost::filesystem::exists(inDIR))
            {
                if (boost::filesystem::is_directory(inDIR))
                {
                    boost::filesystem::directory_iterator endDirIter;
                    for(boost::filesystem::directory_iterator x(inDIR); x != endDirIter; ++x)
                    {
                        files->push_back((*x).path().string());
                    }
                }
                else
                {
                    throw RSGISFileException("Input path is not a directory.");
                }
            }
            else
            {
                throw RSGISFileException("Input path does not exist.");
            }
        }
        catch (const boost::filesystem::filesystem_error& e)
        {
            throw RSGISFileException(e.what());
        }
    }

    void RSGISFileUtils::getDIRList(std::string dir, std::string ext, std::list<std::string> *files, bool withpath)
    {
        boost::filesystem::path tmpPath;
        boost::filesystem::path inDIR(dir);
        try
        {
            if(boost::filesystem::exists(inDIR))
            {
                if (boost::filesystem::is_directory(inDIR))
                {
                    boost::filesystem::directory_iterator endDirIter;
                    for(boost::filesystem::directory_iterator x(inDIR); x != endDirIter; ++x)
                    {
                        tmpPath = (*x).path();
                        if(tmpPath.extension().string() == ext)
                        {
                            if(withpath)
                            {
                                files->push_back(tmpPath.string());
                            }
                            else
                            {
                                files->push_back(tmpPath.filename().string());
                            }
                        }
                    }
                }
                else
                {
                    throw RSGISFileException("Input path is not a directory.");
                }
            }
            else
            {
                throw RSGISFileException("Input path does not exist.");
            }
        }
        catch (const boost::filesystem::filesystem_error& e)
        {
            throw RSGISFileException(e.what());
        }
    }

    void RSGISFileUtils::getDIRList(std::string dir, std::string ext, std::vector<std::string> *files, bool withpath)
    {
        boost::filesystem::path tmpPath;
        boost::filesystem::path inDIR(dir);
        try
        {
            if(boost::filesystem::exists(inDIR))
            {
                if (boost::filesystem::is_directory(inDIR))
                {
                    boost::filesystem::directory_iterator endDirIter;
                    for(boost::filesystem::directory_iterator x(inDIR); x != endDirIter; ++x)
                    {
                        tmpPath = (*x).path();
                        if(tmpPath.extension().string() == ext)
                        {
                            if(withpath)
                            {
                                files->push_back(tmpPath.string());
                            }
                            else
                            {
                                files->push_back(tmpPath.filename().string());
                            }
                        }
                    }
                }
                else
                {
                    throw RSGISFileException("Input path is not a directory.");
                }
            }
            else
            {
                throw RSGISFileException("Input path does not exist.");
            }
        }
        catch (const boost::filesystem::filesystem_error& e)
        {
            throw RSGISFileException(e.what());
        }
    }

    std::string* RSGISFileUtils::getDIRList(std::string dir, std::string ext, int *numFiles, bool withpath)
    {
        std::vector<std::string> files;
        boost::filesystem::path tmpPath;
        boost::filesystem::path inDIR(dir);
        try
        {
            if(boost::filesystem::exists(inDIR))
            {
                if (boost::filesystem::is_directory(inDIR))
                {
                    boost::filesystem::directory_iterator endDirIter;
                    for(boost::filesystem::directory_iterator x(inDIR); x != endDirIter; ++x)
                    {
                        tmpPath = (*x).path();
                        if(tmpPath.extension().string() == ext)
                        {
                            if(withpath)
                            {
                                files.push_back(tmpPath.string());
                            }
                            else
                            {
                                files.push_back(tmpPath.filename().string());
                            }
                        }
                    }
                }
                else
                {
                    throw RSGISFileException("Input path is not a directory.");
                }
            }
            else
            {
                throw RSGISFileException("Input path does not exist.");
            }
        }
        catch (const boost::filesystem::filesystem_error& e)
        {
            throw RSGISFileException(e.what());
        }
        
        *numFiles = files.size();
        std::string *outFiles = new std::string[*numFiles];
        for(int i = 0; i < *numFiles; i++)
        {
            outFiles[i] = files.at(i);
        }
        return outFiles;
    }

    std::string* RSGISFileUtils::getFilesInDIRWithName(std::string dir, std::string name, int *numFiles)
    {
        std::vector<std::string> files;
        boost::filesystem::path tmpPath;
        boost::filesystem::path inDIR(dir);
        try
        {
            if(boost::filesystem::exists(inDIR))
            {
                if (boost::filesystem::is_directory(inDIR))
                {
                    boost::filesystem::directory_iterator endDirIter;
                    for(boost::filesystem::directory_iterator x(inDIR); x != endDirIter; ++x)
                    {
                        tmpPath = (*x).path();
                        if(tmpPath.filename().replace_extension().string() == name)
                        {
                            files.push_back(tmpPath.string());
                        }
                    }
                }
                else
                {
                    throw RSGISFileException("Input path is not a directory.");
                }
            }
            else
            {
                throw RSGISFileException("Input path does not exist.");
            }
        }
        catch (const boost::filesystem::filesystem_error& e)
        {
            throw RSGISFileException(e.what());
        }
        
        *numFiles = files.size();
        std::string *outFiles = new std::string[*numFiles];
        for(int i = 0; i < *numFiles; i++)
        {
            outFiles[i] = files.at(i);
        }
        return outFiles;
    }

    std::string RSGISFileUtils::getFileName(std::string filepath)
    {
        return boost::filesystem::path(filepath).filename().string();
    }

    std::string RSGISFileUtils::getFileNameNoExtension(std::string filepath)
    {
        return boost::filesystem::path(filepath).filename().replace_extension().string();
    }

    std::string RSGISFileUtils::removeExtension(std::string filepath)
    {
        return boost::filesystem::path(filepath).replace_extension().string();
    }

    std::string RSGISFileUtils::getExtension(std::string filepath)
    {
        return boost::filesystem::path(filepath).extension().string();
    }

    std::string RSGISFileUtils::getFileDirectoryPath(std::string filepath)
    {
        return boost::filesystem::path(filepath).parent_path().string();
    }

    bool RSGISFileUtils::checkFilePresent(std::string file)
    {
        return boost::filesystem::exists(boost::filesystem::path(file));
    }

    RSGISFileUtils::~RSGISFileUtils()
    {
        
    }
	
}} //rsgis::utils
