/*
 *  RSGISParseExportForPlotting.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/07/2009.
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

#include "RSGISParseExportForPlotting.h"

namespace rsgis{namespace utils{
	
	RSGISParseExportForPlotting::RSGISParseExportForPlotting()
	{
		
	}
	
	PlotTypes RSGISParseExportForPlotting::identify(std::string file) throw(rsgis::RSGISInputStreamException)
	{
		PlotTypes returnType = unknown;
		
		std::ifstream openInFile;
		openInFile.open(file.c_str());
		if(!openInFile.is_open())
		{
			throw rsgis::RSGISInputStreamException("Could not open input text file.");
		}
		else
		{
			std::string strLine;
			openInFile.seekg(std::ios_base::beg);
			getline(openInFile, strLine);
			openInFile.close();
			std::cout << "strLine: " << strLine << std::endl;
			if(strLine == "#freq2D")
			{
				returnType = freq2d;
			}
			else if(strLine == "#freq3D")
			{
				returnType = freq3d;
			}
			else if(strLine == "#lines2D")
			{
				returnType = lines2d;
			}
			else if(strLine == "#lines3D")
			{
				returnType = lines3d;
			}
			else if(strLine == "#Density")
			{
				returnType = density;
			}
			else if(strLine == "#2DScatter")
			{
				returnType = scatter2d;
			}
			else if(strLine == "#3DScatter")
			{
				returnType = scatter3d;
			}
			else if(strLine == "#c2DScatter")
			{
				returnType = cscatter2d;
			}
			else if(strLine == "#c3DScatter")
			{
				returnType = cscatter3d;
			}
			else if(strLine == "#Surface")
			{
				returnType = surface;
			}
			else if(strLine == "#cSurface")
			{
				returnType = csurface;
			}
			else if(strLine == "#triangle2d")
			{
				returnType = triangle2d;
			}
			else if(strLine == "#triangle3d")
			{
				returnType = triangle3d;
			}
			else
			{
				returnType = unknown;
			}
		}
		
		return returnType;
	}
	
	void RSGISParseExportForPlotting::parseFrequency2D(std::string file, std::list<double> *value) throw(rsgis::RSGISInputStreamException)
	{
		throw rsgis::RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseFrequency3D(std::string file, std::list<double> *value1, std::list<double> *value2) throw(rsgis::RSGISInputStreamException)
	{
		throw rsgis::RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseScatter2D(std::string file, std::list<double> *x, std::list<double> *y) throw(rsgis::RSGISInputStreamException)
	{
		throw rsgis::RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseScatter3D(std::string file, std::list<double> *x, std::list<double> *y, std::list<double> *z) throw(rsgis::RSGISInputStreamException)
	{
		throw rsgis::RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseCScatter2D(std::string file, std::list<double> *x, std::list<double> *y, std::list<double> *c) throw(rsgis::RSGISInputStreamException)
	{
		throw rsgis::RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseCScatter3D(std::string file, std::list<double> *x, std::list<double> *y, std::list<double> *z, std::list<double> *c) throw(rsgis::RSGISInputStreamException)
	{
		throw rsgis::RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseDensity(std::string file, std::list<double> *x, std::list<double> *y) throw(rsgis::RSGISInputStreamException)
	{
		throw rsgis::RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseSurface(std::string file, std::list<double> *x, std::list<double> *y, std::list<double> *z) throw(rsgis::RSGISInputStreamException)
	{
		throw rsgis::RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseCSurface(std::string file, std::list<double> *x, std::list<double> *y, std::list<double> *z, std::list<double> *c) throw(rsgis::RSGISInputStreamException)
	{
		throw rsgis::RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseLines2D(std::string file, std::list<double> *x1, std::list<double> *y1, std::list<double> *x2, std::list<double> *y2) throw(rsgis::RSGISInputStreamException)
	{
		RSGISTextUtils textUtils;
		rsgis::math::RSGISMathsUtils mathUtils;
		std::ifstream openInFile;
		openInFile.open(file.c_str());
		if(!openInFile.is_open())
		{
			throw rsgis::RSGISInputStreamException("Could not open input text file.");
		}
		else
		{
			std::string strLine;
			std::vector<std::string> *tokens = new std::vector<std::string>();
			openInFile.seekg(std::ios_base::beg);
			getline(openInFile, strLine);
			if(strLine != "#lines2D")
			{
				openInFile.close();
				throw rsgis::RSGISInputStreamException("Could not parse lines2D file as format incorrect.");
			}
			
			openInFile.seekg(std::ios_base::beg);
			while(!openInFile.eof())
			{
				getline(openInFile, strLine);
				if(textUtils.lineStart(strLine, '#'))
				{
					// IGNORE HEADER/METADATA
				}
				else if(strLine.size() == 0)
				{
					// IGNORE BLANK LINES
				}
				else
				{
					textUtils.tokenizeString(strLine, ',', tokens, true);
					if(tokens->size() != 4)
					{
						openInFile.close();
						throw rsgis::RSGISInputStreamException("Error while parsing. File format is incorrect for lines2D file.");
					}
					x1->push_back(mathUtils.strtodouble(tokens->at(0)));
					y1->push_back(mathUtils.strtodouble(tokens->at(1)));
					x2->push_back(mathUtils.strtodouble(tokens->at(2)));
					y2->push_back(mathUtils.strtodouble(tokens->at(3)));
					tokens->clear();
				}
			}
			delete tokens;
		}
		openInFile.close();
	}
	
	void RSGISParseExportForPlotting::parseLines3D(std::string file, std::list<double> *x1, std::list<double> *y1, std::list<double> *z1, std::list<double> *x2, std::list<double> *y2, std::list<double> *z2) throw(rsgis::RSGISInputStreamException)
	{
		RSGISTextUtils textUtils;
		rsgis::math::RSGISMathsUtils mathUtils;
		std::ifstream openInFile;
		openInFile.open(file.c_str());
		if(!openInFile.is_open())
		{
			throw rsgis::RSGISInputStreamException("Could not open input text file.");
		}
		else
		{
			std::string strLine;
			std::vector<std::string> *tokens = new std::vector<std::string>();
			openInFile.seekg(std::ios_base::beg);
			getline(openInFile, strLine);
			if(strLine != "#lines3D")
			{
				openInFile.close();
				throw rsgis::RSGISInputStreamException("Could not parse lines3D file as format incorrect.");
			}
			
			openInFile.seekg(std::ios_base::beg);
			while(!openInFile.eof())
			{
				getline(openInFile, strLine);
				if(textUtils.lineStart(strLine, '#'))
				{
					// IGNORE HEADER/METADATA
				}
				else if(strLine.size() == 0)
				{
					// IGNORE BLANK LINES
				}
				else
				{
					textUtils.tokenizeString(strLine, ',', tokens, true);
					if(tokens->size() != 6)
					{
						openInFile.close();
						throw rsgis::RSGISInputStreamException("Error while parsing. File format is incorrect for lines3D file.");
					}
					x1->push_back(mathUtils.strtodouble(tokens->at(0)));
					y1->push_back(mathUtils.strtodouble(tokens->at(1)));
					z1->push_back(mathUtils.strtodouble(tokens->at(2)));
					x2->push_back(mathUtils.strtodouble(tokens->at(3)));
					y2->push_back(mathUtils.strtodouble(tokens->at(4)));
					z2->push_back(mathUtils.strtodouble(tokens->at(5)));
					tokens->clear();
				}
			}
			delete tokens;
		}
		openInFile.close();
	}
	
	void RSGISParseExportForPlotting::parseTriangles2D(std::string file, std::list<double> *x, std::list<double> *y) throw(rsgis::RSGISInputStreamException)
	{
		throw rsgis::RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseTriangles3D(std::string file, std::list<double> *x, std::list<double> *y, std::list<double> *z) throw(rsgis::RSGISInputStreamException)
	{
		throw rsgis::RSGISInputStreamException("Parser not written yet.");
	}
	
	RSGISParseExportForPlotting::~RSGISParseExportForPlotting()
	{
		
	}
}}


