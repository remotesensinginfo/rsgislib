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
	
	PlotTypes RSGISParseExportForPlotting::identify(string file) throw(RSGISInputStreamException)
	{
		PlotTypes returnType = unknown;
		
		ifstream openInFile;
		openInFile.open(file.c_str());
		if(!openInFile.is_open())
		{
			throw RSGISInputStreamException("Could not open input text file.");
		}
		else
		{
			string strLine;
			openInFile.seekg(ios_base::beg);
			getline(openInFile, strLine);
			openInFile.close();
			cout << "strLine: " << strLine << endl;
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
	
	void RSGISParseExportForPlotting::parseFrequency2D(string file, list<double> *value) throw(RSGISInputStreamException)
	{
		throw RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseFrequency3D(string file, list<double> *value1, list<double> *value2) throw(RSGISInputStreamException)
	{
		throw RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseScatter2D(string file, list<double> *x, list<double> *y) throw(RSGISInputStreamException)
	{
		throw RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseScatter3D(string file, list<double> *x, list<double> *y, list<double> *z) throw(RSGISInputStreamException)
	{
		throw RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseCScatter2D(string file, list<double> *x, list<double> *y, list<double> *c) throw(RSGISInputStreamException)
	{
		throw RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseCScatter3D(string file, list<double> *x, list<double> *y, list<double> *z, list<double> *c) throw(RSGISInputStreamException)
	{
		throw RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseDensity(string file, list<double> *x, list<double> *y) throw(RSGISInputStreamException)
	{
		throw RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseSurface(string file, list<double> *x, list<double> *y, list<double> *z) throw(RSGISInputStreamException)
	{
		throw RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseCSurface(string file, list<double> *x, list<double> *y, list<double> *z, list<double> *c) throw(RSGISInputStreamException)
	{
		throw RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseLines2D(string file, list<double> *x1, list<double> *y1, list<double> *x2, list<double> *y2) throw(RSGISInputStreamException)
	{
		RSGISTextUtils textUtils;
		RSGISMathsUtils mathUtils;
		ifstream openInFile;
		openInFile.open(file.c_str());
		if(!openInFile.is_open())
		{
			throw RSGISInputStreamException("Could not open input text file.");
		}
		else
		{
			string strLine;
			vector<string> *tokens = new vector<string>();
			openInFile.seekg(ios_base::beg);
			getline(openInFile, strLine);
			if(strLine != "#lines2D")
			{
				openInFile.close();
				throw RSGISInputStreamException("Could not parse lines2D file as format incorrect.");
			}
			
			openInFile.seekg(ios_base::beg);
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
					//cout << strLine << endl;
					textUtils.tokenizeString(strLine, ',', tokens, true);
					if(tokens->size() != 4)
					{
						openInFile.close();
						throw RSGISInputStreamException("Error while parsing. File format is incorrect for lines2D file.");
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
	
	void RSGISParseExportForPlotting::parseLines3D(string file, list<double> *x1, list<double> *y1, list<double> *z1, list<double> *x2, list<double> *y2, list<double> *z2) throw(RSGISInputStreamException)
	{
		RSGISTextUtils textUtils;
		RSGISMathsUtils mathUtils;
		ifstream openInFile;
		openInFile.open(file.c_str());
		if(!openInFile.is_open())
		{
			throw RSGISInputStreamException("Could not open input text file.");
		}
		else
		{
			string strLine;
			vector<string> *tokens = new vector<string>();
			openInFile.seekg(ios_base::beg);
			getline(openInFile, strLine);
			if(strLine != "#lines3D")
			{
				openInFile.close();
				throw RSGISInputStreamException("Could not parse lines3D file as format incorrect.");
			}
			
			openInFile.seekg(ios_base::beg);
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
					//cout << strLine << endl;
					textUtils.tokenizeString(strLine, ',', tokens, true);
					if(tokens->size() != 6)
					{
						openInFile.close();
						throw RSGISInputStreamException("Error while parsing. File format is incorrect for lines3D file.");
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
	
	void RSGISParseExportForPlotting::parseTriangles2D(string file, list<double> *x, list<double> *y) throw(RSGISInputStreamException)
	{
		throw RSGISInputStreamException("Parser not written yet.");
	}
	
	void RSGISParseExportForPlotting::parseTriangles3D(string file, list<double> *x, list<double> *y, list<double> *z) throw(RSGISInputStreamException)
	{
		throw RSGISInputStreamException("Parser not written yet.");
	}
	
	RSGISParseExportForPlotting::~RSGISParseExportForPlotting()
	{
		
	}
}}


