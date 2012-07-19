/*
 *  RSGISImageFootprintPolygonsCSVParse.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/09/2009.
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

#include "RSGISImageFootprintPolygonsCSVParse.h"

namespace rsgis{ namespace utils{
	

	RSGISImageFootprintPolygonsCSVParse::RSGISImageFootprintPolygonsCSVParse()
	{
		
	}
	
	std::vector<ImageFootPrintPoly*>* RSGISImageFootprintPolygonsCSVParse::parsePoly(std::string inputfile) throw(rsgis::RSGISInputStreamException, RSGISTextException)
	{
		std::vector<ImageFootPrintPoly*> *plotPolys = new std::vector<ImageFootPrintPoly*>();
		
		try 
		{
			std::ifstream inputCSVFile;
			inputCSVFile.open(inputfile.c_str());
			if(!inputCSVFile.is_open())
			{
				std::string message = std::string("Could not open input text file: ") + inputfile;
				throw rsgis::RSGISInputStreamException(message);
			}
			else
			{
				RSGISTextUtils textUtils;
                rsgis::math::RSGISMathsUtils mathsUtils;
				
				std::string strLine;
				std::string word;
				std::vector<std::string>::iterator iterTokens;
				std::vector<std::string> *tokens = new std::vector<std::string>();
				ImageFootPrintPoly *poly = NULL;
				inputCSVFile.seekg(std::ios_base::beg);
				
				int polyFID = 0;
				
				while(!inputCSVFile.eof())
				{
					getline(inputCSVFile, strLine);
					if(!textUtils.lineStart(strLine, '#') & !textUtils.blankline(strLine))
					{
						textUtils.tokenizeString(strLine, ',', tokens, false);
						if(tokens->size() == 9)
						{
							poly = new ImageFootPrintPoly();
							poly->fid = polyFID;
							poly->scene = tokens->at(0);
							poly->ulE = mathsUtils.strtodouble(tokens->at(1));
							poly->ulN = mathsUtils.strtodouble(tokens->at(2));
							poly->urE = mathsUtils.strtodouble(tokens->at(3));
							poly->urN = mathsUtils.strtodouble(tokens->at(4));
							poly->lrE = mathsUtils.strtodouble(tokens->at(5));
							poly->lrN = mathsUtils.strtodouble(tokens->at(6));
							poly->llE = mathsUtils.strtodouble(tokens->at(7));
							poly->llN = mathsUtils.strtodouble(tokens->at(8));
							plotPolys->push_back(poly);
							polyFID++;
						}
						else 
						{
							std::string message = std::string("Line does not an sufficient tokens (") + mathsUtils.inttostring(tokens->size()) + std::string("): ") + strLine;
							throw RSGISTextException(message);
						}
						tokens->clear();
						
						//cout << this->formatedstd::string(poly) << endl;
					}
					// ELSE IGNORE AS EITHER BLANK LINE OR COMMENT
				}
				inputCSVFile.close();
			}
		}
		catch (RSGISInputStreamException &e) 
		{
			throw e;
		}
		catch (RSGISTextException &e) 
		{
			throw e;
		}
				
		return plotPolys;
	}
	
	std::string RSGISImageFootprintPolygonsCSVParse::formatedString(ImageFootPrintPoly *poly)
	{
        rsgis::math::RSGISMathsUtils mathsUtils;
		
		std::string output = "";
		
		output = mathsUtils.inttostring(poly->fid) + "," + 
				poly->scene + "," +
				mathsUtils.inttostring(poly->ulE) + "," +
				mathsUtils.doubletostring(poly->ulN) + "," +
				mathsUtils.doubletostring(poly->urE) + "," +
				mathsUtils.doubletostring(poly->urN) + "," +
				mathsUtils.doubletostring(poly->lrE) + "," +
				mathsUtils.doubletostring(poly->lrN) + "," +
				mathsUtils.doubletostring(poly->llE) + "," +
		mathsUtils.doubletostring(poly->llN);

		return output;
	}
	
	RSGISImageFootprintPolygonsCSVParse::~RSGISImageFootprintPolygonsCSVParse()
	{
		
	}
}}


