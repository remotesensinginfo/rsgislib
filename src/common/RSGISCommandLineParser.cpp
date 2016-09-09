/*
 *  RSGISCommandLineParser.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 28/04/2008.
 *  Copyright 2008 RSGISLib.
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

#include "RSGISCommandLineParser.h"

namespace rsgis
{

	RSGISCommandLineParser::RSGISCommandLineParser()
	{
		
	}
	
	void RSGISCommandLineParser::parseArguments(int argc, char **argv) throw(RSGISCommandLineException)
	{
		std::string tmpStr;
		numArguments = 0;
		for(int i = 1; i < argc; i++)
		{
			if(argv[i][0] == '-')
			{
				numArguments++;
			}
		}
		
		if(numArguments == 0)
		{
			throw RSGISCommandLineException("No arguments present");
		}
		
		arguments = new argpair*[numArguments];
		for(int i = 0; i < numArguments; i++)
		{
			arguments[i] = new argpair();
			arguments[i]->arg = std::string("");
			arguments[i]->numVals = 0;
		}
		
		int argNum = 0;
		for(int i = 1; i < argc; i++)
		{
			if(argv[i][0] == '-')
			{
				arguments[argNum++]->arg = std::string(argv[i]);
			}
			else
			{
				arguments[argNum-1]->numVals++;
			}
		}
		
		argNum = 0;
		int valNum = 0;
		for(int i = 1; i < argc; i++)
		{
			if(argv[i][0] == '-')
			{
				if(arguments[argNum]->numVals > 0)
				{
					arguments[argNum]->value = new std::string[arguments[argNum]->numVals];
				}
				else
				{
					arguments[argNum]->value = NULL;
				}
				argNum++;
				valNum = 0;
			}
			else
			{
				tmpStr = std::string(argv[i]);
				if(tmpStr.at(0) == '%')
				{
					tmpStr = tmpStr.substr(1);
				}
				//cout << "Adding tmpStr '" << tmpStr << "' tmpStr has a size " << tmpStr.size() << endl;
				arguments[argNum-1]->value[valNum++] = tmpStr;
			}
		}
	}
	
	int RSGISCommandLineParser::getNumArgs()
	{
		return numArguments;
	}
	
	bool RSGISCommandLineParser::argumentPresent(std::string arg)
	{
		for(int i = 0; i < numArguments; i++)
		{
			if(arguments[i]->arg == arg)
			{
				return true;
			}
		}
		return false;
	}
	
	argpair* RSGISCommandLineParser::findArgument(std::string arg)
	{
		for(int i = 0; i < numArguments; i++)
		{
			if(arguments[i]->arg == arg)
			{
				return arguments[i];
			}
		}
		return NULL;
	}
	
	RSGISCommandLineParser::~RSGISCommandLineParser()
	{
		if(arguments != NULL)
		{
			/*
			for(int i = 0; i < numArguments; i++)
			{
				cout << "Number of arg values = " << arguments[i]->numVals << endl;
				if(arguments[i]->numVals > 0)
				{
					cout << "Value = " << arguments[i]->value[0] << endl;
					delete[] arguments[i]->value;
				}
			}
			*/
			delete[] arguments;
		}
	}
}

