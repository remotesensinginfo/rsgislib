/*
 *  RSGISPlotPolygonsCSVParse.cpp
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

#include "RSGISPlotPolygonsCSVParse.h"

namespace rsgis{ namespace utils{
	

	RSGISPlotPolygonsCSVParse::RSGISPlotPolygonsCSVParse()
	{
		
	}
	
	std::vector<PlotPoly*>* RSGISPlotPolygonsCSVParse::parsePolyPlots(std::string inputfile)
	{
		std::vector<PlotPoly*> *plotPolys = new std::vector<PlotPoly*>();
		
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
				PlotPoly *poly = NULL;
				inputCSVFile.seekg(std::ios_base::beg);
				while(!inputCSVFile.eof())
				{
					getline(inputCSVFile, strLine);
					if(!textUtils.lineStart(strLine, '#') & !textUtils.blankline(strLine))
					{
						textUtils.tokenizeString(strLine, ',', tokens, false);
						if(tokens->size() == 14)
						{
							poly = new PlotPoly();
							poly->fid = mathsUtils.strtoint(tokens->at(0));
							poly->zone = mathsUtils.strtoint(tokens->at(1));
							poly->eastings = mathsUtils.strtodouble(tokens->at(2));
							poly->northings = mathsUtils.strtodouble(tokens->at(3));
							poly->sw_Eastings = mathsUtils.strtodouble(tokens->at(4));
							poly->sw_Northings = mathsUtils.strtodouble(tokens->at(5));
							poly->eastSide = mathsUtils.strtodouble(tokens->at(6));
							poly->northSide = mathsUtils.strtodouble(tokens->at(7));
							poly->orientation = mathsUtils.strtodouble(tokens->at(8));
							poly->siteName = tokens->at(9);
							poly->study = tokens->at(10);
							poly->site = tokens->at(11);
							poly->extraID = mathsUtils.strtoint(tokens->at(12));
							poly->issues = tokens->at(13);
							plotPolys->push_back(poly);
						}
						else 
						{
							std::string message = std::string("Line does not an sufficient tokens (") + mathsUtils.inttostring(tokens->size()) + std::string("): ") + strLine;
							throw RSGISTextException(message);
						}
						tokens->clear();
					}
					// ELSE IGNORE AS EITHER BLANK LINE OR COMMENT
				}
				inputCSVFile.close();
			}
		}
		catch (rsgis::RSGISInputStreamException &e) 
		{
			throw e;
		}
		catch (RSGISTextException &e) 
		{
			throw e;
		}
				
		return plotPolys;
	}
	
	std::string RSGISPlotPolygonsCSVParse::formattedString(PlotPoly *poly)
	{
        rsgis::math::RSGISMathsUtils mathsUtils;
		
		std::string output = "";
		
		output = mathsUtils.inttostring(poly->fid) + "," + 
				mathsUtils.inttostring(poly->zone) + "," +
				mathsUtils.doubletostring(poly->eastings) + "," +
				mathsUtils.doubletostring(poly->northings) + "," +
				mathsUtils.doubletostring(poly->sw_Eastings) + "," +
				mathsUtils.doubletostring(poly->sw_Northings) + "," +
				mathsUtils.doubletostring(poly->eastSide) + "," +
				mathsUtils.doubletostring(poly->northSide) + "," +
				mathsUtils.doubletostring(poly->orientation) + "," +
				poly->siteName + "," +
				poly->study + "," +
				poly->site + "," +
				mathsUtils.inttostring(poly->extraID) + "," + 
				poly->issues;
		
		return output;
	}
	
	RSGISPlotPolygonsCSVParse::~RSGISPlotPolygonsCSVParse()
	{
		
	}
}}


