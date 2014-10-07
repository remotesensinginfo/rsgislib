/*
 *  RSGISCommandLineParser.h
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

#ifndef RSGISCommandLineParser_H
#define RSGISCommandLineParser_H

#include <string>
#include <iostream>

#include "RSGISCommandLineException.h"

namespace rsgis
{    
	struct DllExport argpair
	{
		std::string arg;
		std::string *value;
		int numVals;
	};
	
	
	class DllExport RSGISCommandLineParser
		{
		public:
			RSGISCommandLineParser();
			void parseArguments(int argc, char **argv) throw(RSGISCommandLineException);
			int getNumArgs();
			bool argumentPresent(std::string arg);
			argpair* findArgument(std::string arg);
			~RSGISCommandLineParser();
		protected:
			argpair **arguments;
			int numArguments;
		};
}


#endif

