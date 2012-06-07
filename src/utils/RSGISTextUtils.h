/*
 *  RSGISTextUtils.h
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

#ifndef RSGISTextUtils_H
#define RSGISTextUtils_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "utils/RSGISTextException.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace rsgis{namespace utils{
    
    using boost::lexical_cast;
    using boost::bad_lexical_cast;
    using boost::trim;
    
    using namespace std;
    
	class RSGISTextUtils
		{
		public:
			RSGISTextUtils();
			size_t countLines(string input) throw(RSGISTextException);
			void transpose(string input, string output, string delimiter) throw(RSGISTextException);
			bool lineStart(string line, char token);
			bool blankline(string line);
			void tokenizeString(string line, char token, vector<string> *tokens, bool ignoreDuplicateTokens=true, bool trimWhitespace=false);
			string removeNewLine(string line);
			
			string readFileToString(string input) throw(RSGISTextException);
            void writeStringToFile(string file, string output) throw(RSGISTextException);
			
			double strtodouble(string inValue)throw(RSGISTextException);
			float strtofloat(string inValue)throw(RSGISTextException);
			
			uint_fast8_t strto8bitUInt(string inValue)throw(RSGISTextException);
			uint_fast16_t strto16bitUInt(string inValue)throw(RSGISTextException);
			uint_fast32_t strto32bitUInt(string inValue)throw(RSGISTextException);
			uint_fast64_t strto64bitUInt(string inValue)throw(RSGISTextException);
			
			int_fast8_t strto8bitInt(string inValue)throw(RSGISTextException);
			int_fast16_t strto16bitInt(string inValue)throw(RSGISTextException);
			int_fast32_t strto32bitInt(string inValue)throw(RSGISTextException);
			int_fast64_t strto64bitInt(string inValue)throw(RSGISTextException);
			
			string doubletostring(double number)throw(RSGISTextException);
			string floattostring(float number)throw(RSGISTextException);
			
			string uInt8bittostring(uint_fast8_t number)throw(RSGISTextException);
			string uInt16bittostring(uint_fast16_t number)throw(RSGISTextException);
			string uInt32bittostring(uint_fast32_t number)throw(RSGISTextException);
			string uInt64bittostring(uint_fast64_t number)throw(RSGISTextException);
			
			string int8bittostring(int_fast8_t number)throw(RSGISTextException);
			string int16bittostring(int_fast16_t number)throw(RSGISTextException);
			string int32bittostring(int_fast32_t number)throw(RSGISTextException);
			string int64bittostring(int_fast64_t number)throw(RSGISTextException);
			
			~RSGISTextUtils();
		};
	
	class RSGISTextFileLineReader
	{
	public:
		RSGISTextFileLineReader();
		void openFile(string filepath)throw(RSGISTextException);
		bool endOfFile();
		string readLine()throw(RSGISTextException);
		void closeFile()throw(RSGISTextException);
		~RSGISTextFileLineReader();	
	private:
		ifstream inputFileStream;
		bool fileOpened;
	};
	
}}

#endif
