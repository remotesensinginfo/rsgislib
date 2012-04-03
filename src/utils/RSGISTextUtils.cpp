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
	
	int RSGISTextUtils::countLines(string input)throw(RSGISTextException)
	{
		int count = 0;
		ifstream inputFile;
		inputFile.open(input.c_str());
		if(inputFile.is_open())
		{
			string strLine;		
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
			string message = string("Text file ") + input + string(" could not be openned.");
			throw RSGISTextException(message.c_str());
		}
		
		return count;
	}
	
	void RSGISTextUtils::transpose(string input, string output, string delimiter) throw(RSGISTextException)
	{
		
	}
	
	bool RSGISTextUtils::lineStart(string line, char token)
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
	
	bool RSGISTextUtils::blankline(string line)
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
	
	void RSGISTextUtils::tokenizeString(string line, char token, vector<string> *tokens, bool ignoreDuplicateTokens, bool trimWhitespace)
	{
		string word;
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
                            trim(word);
                        }
						tokens->push_back(word);
					}
				}
				else 
				{
                    if(trimWhitespace)
                    {
                        trim(word);
                    }
					tokens->push_back(word);
				}
				
				start = start + i-start+1;
			}
		}
		word = line.substr(start);
        if(trimWhitespace)
        {
            trim(word);
        }
		tokens->push_back(word);
	}
	
	string RSGISTextUtils::removeNewLine(string line)
	{
		int lineLength = line.length();
		string output = line.substr(0, lineLength-1);
		return output;
	}
	
	string RSGISTextUtils::readFileToString(string input) throw(RSGISTextException)
	{
		string wholeFile = "";
		ifstream inputFileStream;
		inputFileStream.open(input.c_str(), ios_base::in);
		if(!inputFileStream.is_open())
		{
			throw RSGISTextException("File could not be opened.");
		}
		
		string strLine = "";
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
			}
			else 
			{
				strLine += ch;
			}
			
			lastch = ch;
			inputFileStream.get(ch);      
		}
		wholeFile += strLine;
		inputFileStream.close();
		
		return wholeFile;
	}
	
	double RSGISTextUtils::strtodouble(string inValue) throw(RSGISTextException)
	{
		double outValue = 0;
		try
        {
            outValue = lexical_cast<double>(inValue);
        }
        catch(bad_lexical_cast &e)
        {
			string message = string("Trying to convert \"") + inValue + string("\" - ") + string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	float RSGISTextUtils::strtofloat(string inValue) throw(RSGISTextException)
	{
		float outValue = 0;
		try
        {
            outValue = lexical_cast<float>(inValue);
        }
        catch(bad_lexical_cast &e)
        {
            string message = string("Trying to convert \"") + inValue + string("\" - ") + string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	uint_fast8_t RSGISTextUtils::strto8bitUInt(string inValue) throw(RSGISTextException)
	{
		uint_fast8_t outValue = 0;
		try
        {
            outValue = lexical_cast<uint_fast8_t>(inValue);
        }
        catch(bad_lexical_cast &e)
        {
            string message = string("Trying to convert \"") + inValue + string("\" - ") + string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	uint_fast16_t RSGISTextUtils::strto16bitUInt(string inValue) throw(RSGISTextException)
	{
		uint_fast16_t outValue = 0;
		try
        {
            outValue = lexical_cast<uint_fast16_t>(inValue);
        }
        catch(bad_lexical_cast &e)
        {
            string message = string("Trying to convert \"") + inValue + string("\" - ") + string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	uint_fast32_t RSGISTextUtils::strto32bitUInt(string inValue) throw(RSGISTextException)
	{
		uint_fast32_t outValue = 0;
		try
        {
            outValue = lexical_cast<uint_fast32_t>(inValue);
        }
        catch(bad_lexical_cast &e)
        {
            string message = string("Trying to convert \"") + inValue + string("\" - ") + string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	uint_fast64_t RSGISTextUtils::strto64bitUInt(string inValue) throw(RSGISTextException)
	{
		uint_fast64_t outValue = 0;
		try
        {
            outValue = lexical_cast<uint_fast64_t>(inValue);
        }
        catch(bad_lexical_cast &e)
        {
            string message = string("Trying to convert \"") + inValue + string("\" - ") + string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	int_fast8_t RSGISTextUtils::strto8bitInt(string inValue) throw(RSGISTextException)
	{
		int_fast8_t outValue = 0;
		try
        {
            outValue = lexical_cast<int_fast8_t>(inValue);
        }
        catch(bad_lexical_cast &e)
        {
            string message = string("Trying to convert \"") + inValue + string("\" - ") + string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	int_fast16_t RSGISTextUtils::strto16bitInt(string inValue) throw(RSGISTextException)
	{
		int_fast16_t outValue = 0;
		try
        {
            outValue = lexical_cast<int_fast16_t>(inValue);
        }
        catch(bad_lexical_cast &e)
        {
            string message = string("Trying to convert \"") + inValue + string("\" - ") + string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	int_fast32_t RSGISTextUtils::strto32bitInt(string inValue) throw(RSGISTextException)
	{
		int_fast32_t outValue = 0;
		try
        {
            outValue = lexical_cast<int_fast32_t>(inValue);
        }
        catch(bad_lexical_cast &e)
        {
            string message = string("Trying to convert \"") + inValue + string("\" - ") + string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	int_fast64_t RSGISTextUtils::strto64bitInt(string inValue) throw(RSGISTextException)
	{
		int_fast64_t outValue = 0;
		try
        {
            outValue = lexical_cast<int_fast64_t>(inValue);
        }
        catch(bad_lexical_cast &e)
        {
            string message = string("Trying to convert \"") + inValue + string("\" - ") + string(e.what());
            throw RSGISTextException(message);
        }
		return outValue;
	}
	
	string RSGISTextUtils::doubletostring(double number) throw(RSGISTextException)
	{
		string outValue = "";
		try
        {
            outValue = lexical_cast<string>(number);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	string RSGISTextUtils::floattostring(float number) throw(RSGISTextException)
	{
		string outValue = "";
		try
        {
            outValue = lexical_cast<string>(number);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	string RSGISTextUtils::uInt8bittostring(uint_fast8_t number) throw(RSGISTextException)
	{
		string outValue = "";
		try
        {
            outValue = lexical_cast<string>(number);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	string RSGISTextUtils::uInt16bittostring(uint_fast16_t number) throw(RSGISTextException)
	{
		string outValue = "";
		try
        {
            outValue = lexical_cast<string>(number);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	string RSGISTextUtils::uInt32bittostring(uint_fast32_t number) throw(RSGISTextException)
	{
		string outValue = "";
		try
        {
            outValue = lexical_cast<string>(number);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	string RSGISTextUtils::uInt64bittostring(uint_fast64_t number) throw(RSGISTextException)
	{
		string outValue = "";
		try
        {
            outValue = lexical_cast<string>(number);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	string RSGISTextUtils::int8bittostring(int_fast8_t number) throw(RSGISTextException)
	{
		string outValue = "";
		try
        {
            outValue = lexical_cast<string>(number);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	string RSGISTextUtils::int16bittostring(int_fast16_t number) throw(RSGISTextException)
	{
		string outValue = "";
		try
        {
            outValue = lexical_cast<string>(number);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	string RSGISTextUtils::int32bittostring(int_fast32_t number) throw(RSGISTextException)
	{
		string outValue = "";
		try
        {
            outValue = lexical_cast<string>(number);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISTextException(e.what());
        }
		return outValue;
	}
	
	string RSGISTextUtils::int64bittostring(int_fast64_t number) throw(RSGISTextException)
	{
		string outValue = "";
		try
        {
            outValue = lexical_cast<string>(number);
        }
        catch(bad_lexical_cast &e)
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
	
	void RSGISTextFileLineReader::openFile(string filepath)throw(RSGISTextException)
	{
		inputFileStream.open(filepath.c_str(), ios_base::in);
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
	
	string RSGISTextFileLineReader::readLine()throw(RSGISTextException)
	{
		string strLine = "";
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

