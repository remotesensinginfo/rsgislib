/*
 *  RSGISTextUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 26/05/2008.
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

#include "RSGISTextUtils.h"

namespace rsgis{namespace utils{
	
	RSGISTextUtils::RSGISTextUtils()
	{
		
	}
	
	size_t RSGISTextUtils::countLines(std::string input)throw(RSGISTextException)
	{
		size_t count = 0;
        std::ifstream inputFile;
		inputFile.open(input.c_str());
		if(inputFile.is_open())
		{
			std::string strLine;		
			while(!inputFile.eof())
			{
				getline(inputFile, strLine, '\n');
				if(strLine.size() > 0)
				{
					count++;
				}
			}
		}
		else
		{
			std::string message = std::string("Text file ") + input + std::string(" could not be openned.");
			throw RSGISTextException(message.c_str());
		}
		
		return count;
	}
	
	void RSGISTextUtils::transpose(std::string input, std::string output, std::string delimiter) throw(RSGISTextException)
	{
		
	}
	
	bool RSGISTextUtils::lineStart(std::string line, char token)
	{
		int lineLength = line.length();
		for(int i = 0; i < lineLength; i++)
		{
			if(line.at(i) == ' ' | line.at(i) == '\t' | line.at(i) == '\n')
			{
				// spaces and tabs at the beginning of a line can be ignored.
			}
			else if(line.at(i) == token)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		return false;
	}
	
	bool RSGISTextUtils::blankline(std::string line)
	{
		int lineLength = line.length();
		if(lineLength == 1)
		{
			return true;
		}
		else
		{
			for(int i = 0; i < lineLength; i++)
			{
				if(line.at(i) == ' ' | line.at(i) == '\t' | line.at(i) == '\n')
				{
					// spaces and tabs at the beginning of a line can be ignored.
				}
				else
				{
					return false;
				}
			}
		}
		return true;
	}
	
	void RSGISTextUtils::tokenizeString(std::string line, char token, std::vector<std::string> *tokens, bool ignoreDuplicateTokens, bool trimWhitespace)
	{
		std::string word;
		int start = 0;
		int lineLength = line.length();
		for(int i = 0; i < lineLength; i++)
		{
			if(line.at(i) == token)
			{
				word = boost::algorithm::trim_copy(line.substr(start, i-start));								
				if(ignoreDuplicateTokens)
				{
					if(word.length() > 0)
					{
                        if(trimWhitespace)
                        {
                            boost::trim(word);
                        }
						tokens->push_back(word);
					}
				}
				else 
				{
                    if(trimWhitespace)
                    {
                        boost::trim(word);
                    }
					tokens->push_back(word);
				}
				
				start = start + i-start+1;
			}
		}
		word = line.substr(start);
        if(trimWhitespace)
        {
            boost::trim(word);
        }
		tokens->push_back(word);
	}
	
	std::string RSGISTextUtils::removeNewLine(std::string line)
	{
		int lineLength = line.length();
		std::string output = line.substr(0, lineLength-1);
		return output;
	}
	
	std::string RSGISTextUtils::readFileToString(std::string input) throw(RSGISTextException)
	{
		std::string wholeFile = "";
		std::ifstream inputFileStream;
		inputFileStream.open(input.c_str(), std::ios_base::in);
		if(!inputFileStream.is_open())
		{
			throw RSGISTextException("File could not be opened.");
		}
		
		std::string strLine = "";
		bool lineEnding = false;
		char ch, lastch;
		inputFileStream.get(ch);
		while (!inputFileStream.eof()) 
		{					
			if ((ch == 0x0a) && (lastch == 0x0d))
			{
				lineEnding = true; // Windows Line Ending
			}
			else if ((lastch == 0x0d) && (ch != 0x0a)) 
			{
				lineEnding = true; // Mac Line Ending
			} 
			else if (ch == 0x0a) 
			{
				lineEnding = true; // UNIX Line Ending
			}
			
			if(lineEnding)
			{
				boost::algorithm::trim(strLine);
				wholeFile += strLine;
				strLine = "";
                lineEnding = false;
			}
			else 
			{
				strLine += ch;
			}
			
			lastch = ch;
			inputFileStream.get(ch);
		}
        boost::algorithm::trim(strLine);
		wholeFile += strLine;
		inputFileStream.close();
		
		return wholeFile;
	}
    
    std::vector<std::string> RSGISTextUtils::readFileToStringVector(std::string input) throw(RSGISTextException)
    {
        std::vector<std::string> wholeFile;
		std::ifstream inputFileStream;
		inputFileStream.open(input.c_str(), std::ios_base::in);
		if(!inputFileStream.is_open())
		{
			throw RSGISTextException("File could not be opened.");
		}
		
		std::string strLine = "";
		bool lineEnding = false;
		char ch, lastch;
		inputFileStream.get(ch);
		while (!inputFileStream.eof())
		{
			if ((ch == 0x0a) && (lastch == 0x0d))
			{
				lineEnding = true; // Windows Line Ending
			}
			else if ((lastch == 0x0d) && (ch != 0x0a))
			{
				lineEnding = true; // Mac Line Ending
			}
			else if (ch == 0x0a)
			{
				lineEnding = true; // UNIX Line Ending
			}
			
			if(lineEnding)
			{
				boost::algorithm::trim(strLine);
				wholeFile.push_back(strLine);
				strLine = "";
                lineEnding = false;
			}
			else
			{
				strLine += ch;
			}
			
			lastch = ch;
			inputFileStream.get(ch);
		}
		boost::algorithm::trim(strLine);
        wholeFile.push_back(strLine);
		inputFileStream.close();
		
		return wholeFile;
    }
    
    void RSGISTextUtils::writeStringToFile(std::string file, std::string output) throw(RSGISTextException)
    {
        try 
        {
            std::ofstream outFile;
            outFile.open(file.c_str());
            if(outFile.is_open())
            {
                outFile << output << std::endl;
                outFile.flush();
                outFile.close();
            }
            else 
            {
                std::string message = std::string("File \'") + file + std::string("\' could not be created.");
                throw RSGISTextException(message);
            }
            
        }
        catch (RSGISTextException &e)
        {
            throw e;
        }
    }
	
	double RSGISTextUtils::strtodouble(std::string inValue) throw(RSGISTextException)
	{
		double outValue = 0;
		try
        {
            outValue = boost::lexical_cast<double>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
			std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	float RSGISTextUtils::strtofloat(std::string inValue) throw(RSGISTextException)
	{
		float outValue = 0;
		try
        {
            outValue = boost::lexical_cast<float>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
    
    
    int RSGISTextUtils::strtoInt(std::string inValue)throw(RSGISTextException)
    {
        int outValue = 0;
        try
        {
            outValue = boost::lexical_cast<int>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
        return outValue;
    }
    
    unsigned int RSGISTextUtils::strtoUInt(std::string inValue)throw(RSGISTextException)
    {
        unsigned int outValue = 0;
        try
        {
            outValue = boost::lexical_cast<unsigned int>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
        return outValue;
    }
    
    long RSGISTextUtils::strtoLong(std::string inValue)throw(RSGISTextException)
    {
        long outValue = 0;
        try
        {
            outValue = boost::lexical_cast<long>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
        return outValue;
    }
    
    unsigned long RSGISTextUtils::strtoULong(std::string inValue)throw(RSGISTextException)
    {
        unsigned long outValue = 0;
        try
        {
            outValue = boost::lexical_cast<unsigned long>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
        return outValue;
    }
	
	uint_fast8_t RSGISTextUtils::strto8bitUInt(std::string inValue) throw(RSGISTextException)
	{
		uint_fast8_t outValue = 0;
		try
        {
            outValue = boost::lexical_cast<uint_fast8_t>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	uint_fast16_t RSGISTextUtils::strto16bitUInt(std::string inValue) throw(RSGISTextException)
	{
		uint_fast16_t outValue = 0;
		try
        {
            outValue = boost::lexical_cast<uint_fast16_t>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	uint_fast32_t RSGISTextUtils::strto32bitUInt(std::string inValue) throw(RSGISTextException)
	{
		uint_fast32_t outValue = 0;
		try
        {
            outValue = boost::lexical_cast<uint_fast32_t>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	uint_fast64_t RSGISTextUtils::strto64bitUInt(std::string inValue) throw(RSGISTextException)
	{
		uint_fast64_t outValue = 0;
		try
        {
            outValue = boost::lexical_cast<uint_fast64_t>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	int_fast8_t RSGISTextUtils::strto8bitInt(std::string inValue) throw(RSGISTextException)
	{
		int_fast8_t outValue = 0;
		try
        {
            outValue = boost::lexical_cast<int_fast8_t>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	int_fast16_t RSGISTextUtils::strto16bitInt(std::string inValue) throw(RSGISTextException)
	{
		int_fast16_t outValue = 0;
		try
        {
            outValue = boost::lexical_cast<int_fast16_t>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	int_fast32_t RSGISTextUtils::strto32bitInt(std::string inValue) throw(RSGISTextException)
	{
		int_fast32_t outValue = 0;
		try
        {
            outValue = boost::lexical_cast<int_fast32_t>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	int_fast64_t RSGISTextUtils::strto64bitInt(std::string inValue) throw(RSGISTextException)
	{
		int_fast64_t outValue = 0;
		try
        {
            outValue = boost::lexical_cast<int_fast64_t>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
    
    size_t RSGISTextUtils::strtosizet(std::string inValue)throw(RSGISTextException)
    {
        size_t outValue = 0;
		try
        {
            outValue = boost::lexical_cast<size_t>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
    }
	
	std::string RSGISTextUtils::doubletostring(double number) throw(RSGISTextException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	std::string RSGISTextUtils::floattostring(float number) throw(RSGISTextException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	std::string RSGISTextUtils::uInt8bittostring(uint_fast8_t number) throw(RSGISTextException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	std::string RSGISTextUtils::uInt16bittostring(uint_fast16_t number) throw(RSGISTextException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	std::string RSGISTextUtils::uInt32bittostring(uint_fast32_t number) throw(RSGISTextException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	std::string RSGISTextUtils::uInt64bittostring(uint_fast64_t number) throw(RSGISTextException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	std::string RSGISTextUtils::int8bittostring(int_fast8_t number) throw(RSGISTextException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	std::string RSGISTextUtils::int16bittostring(int_fast16_t number) throw(RSGISTextException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	std::string RSGISTextUtils::int32bittostring(int_fast32_t number) throw(RSGISTextException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	std::string RSGISTextUtils::int64bittostring(int_fast64_t number) throw(RSGISTextException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
    
    std::string RSGISTextUtils::sizettostring(size_t number)throw(RSGISTextException)
    {
        std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
    }
		
	RSGISTextUtils::~RSGISTextUtils()
	{
		
	}
	
	
	
	
	
	RSGISTextFileLineReader::RSGISTextFileLineReader(): inputFileStream(), fileOpened(false)
	{
		
	}
	
	void RSGISTextFileLineReader::openFile(std::string filepath)throw(RSGISTextException)
	{
		inputFileStream.open(filepath.c_str(), std::ios_base::in);
		if(!inputFileStream.is_open())
		{
			throw RSGISTextException("File could not be opened.");
		}
		fileOpened = true;
	}
	
	bool RSGISTextFileLineReader::endOfFile()
	{
		if(fileOpened)
		{
			return inputFileStream.eof();
		}
		return true;
	}
	
	std::string RSGISTextFileLineReader::readLine()throw(RSGISTextException)
	{
		std::string strLine = "";
		bool lineEnding = false;
		char ch, lastch;
		inputFileStream.get(ch);
		while (!inputFileStream.eof()) 
		{					
			if ((ch == 0x0a) && (lastch == 0x0d))
			{
				lineEnding = true; // Windows Line Ending
                inputFileStream.unget();
			}
			else if ((lastch == 0x0d) && (ch != 0x0a)) 
			{
				lineEnding = true; // Mac Line Ending
                inputFileStream.unget();
			} 
			else if (ch == 0x0a) 
			{
				lineEnding = true; // UNIX Line Ending
			}
			
			if(lineEnding)
			{
				break;
			}
			else 
			{
				strLine += ch;
			}
			
			lastch = ch;
			inputFileStream.get(ch);      
		}
		
		boost::algorithm::trim(strLine);
		
		return strLine;
	}
	
	void RSGISTextFileLineReader::closeFile()throw(RSGISTextException)
	{
		inputFileStream.close();
		fileOpened = false;
	}
	
	RSGISTextFileLineReader::~RSGISTextFileLineReader()
	{
		
	}
	
	
}}

