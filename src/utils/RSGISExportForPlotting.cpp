/*
 *  RSGISExportForPlotting.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/07/2009.
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

#include "RSGISExportForPlotting.h"

namespace rsgis{namespace utils{
	
	RSGISExportForPlotting* RSGISExportForPlotting::instance = NULL;
	
	RSGISExportForPlotting::RSGISExportForPlotting(string filepath)
	{
		this->filepath = filepath;
		instance = NULL;
	}
	
	RSGISExportForPlotting* RSGISExportForPlotting::getInstance(string filepath)
	{
		if(instance == NULL)
		{
			instance = new RSGISExportForPlotting(filepath);
		}
		return instance;
	}
	
	RSGISExportForPlotting* RSGISExportForPlotting::getInstance() throw(RSGISException)
	{
		if(instance == NULL)
		{
			throw RSGISException("Plotter is not instanciated, a filepath needs to be specified: <rsgis:command algor=\"visualisation\" option=\"setupplotter\" outdir=\"string\" />");
		}
		return instance;
	}
	
	string RSGISExportForPlotting::getFilePath()
	{
		return filepath;
	}
	
	void RSGISExportForPlotting::export2DFreq(string filename, list<double> *values) throw(RSGISOutputStreamException)
	{
		string outputFilename = this->filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#freq2D" << endl;
			outTxtFile << "#value" << endl;
			
			list<double>::iterator iterValues;
			
			for(iterValues = values->begin(); iterValues != values->end(); ++iterValues)
			{
				outTxtFile << (*iterValues) << endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}
	}
	
	void RSGISExportForPlotting::export3DFreq(string filename, list<double> *values1, list<double> *values2) throw(RSGISOutputStreamException)
	{
		string outputFilename = filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(values1->size() != values2->size())
		{
			throw RSGISOutputStreamException("Number of values for 1 and 2 are not equal.");
		}
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#freq3D" << endl;
			outTxtFile << "#value1,value2" << endl;
			
			list<double>::iterator iterValues1;
			list<double>::iterator iterValues2;
			
			iterValues1 = values1->begin();
			iterValues2 = values2->begin();
			
			while(iterValues1 != values1->end() & iterValues2 != values2->end())
			{
				outTxtFile << (*iterValues1) << "," << (*iterValues2) << endl;
				
				++iterValues1;
				++iterValues2;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::export2DLines(string filename, list<LineSegment> *lines) throw(RSGISOutputStreamException)
	{
		string outputFilename = filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#lines2D" << endl;
			outTxtFile << "#x1,y1,x2,y2" << endl;
			
			list<LineSegment>::iterator iterLines;
			
			for(iterLines = lines->begin(); iterLines != lines->end(); iterLines++)
			{
				outTxtFile << (*iterLines).p0.x << "," << (*iterLines).p0.y << "," << (*iterLines).p1.x << "," << (*iterLines).p1.y << endl;
			}
		
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::export3DLines(string filename, list<LineSegment> *lines) throw(RSGISOutputStreamException)
	{
		string outputFilename = filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#lines3D" << endl;
			outTxtFile << "#x1,y1,z1,x2,y2,z2" << endl;
			
			list<LineSegment>::iterator iterLines;
			
			for(iterLines = lines->begin(); iterLines != lines->end(); iterLines++)
			{
				outTxtFile << (*iterLines).p0.x << "," << (*iterLines).p0.y << "," << (*iterLines).p0.z << "," << (*iterLines).p1.x << "," << (*iterLines).p1.y << "," << (*iterLines).p1.z << endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::exportDensity(string filename, double *x, double *y, int numPts) throw(RSGISOutputStreamException)
	{
		string outputFilename = filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#Density" << endl;
			outTxtFile << "#x,y" << endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::export2DScatter(string filename, double *x, double *y, int numPts) throw(RSGISOutputStreamException)
	{
		string outputFilename = filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#2DScatter" << endl;
			outTxtFile << "#x,y" << endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::export3DScatter(string filename, double *x, double *y, double *z, int numPts) throw(RSGISOutputStreamException)
	{
		string outputFilename = filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#3DScatter" << endl;
			outTxtFile << "#x,y,z" << endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << "," << z[i] << endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::export2DColourScatter(string filename, double *x, double *y, double *c, int numPts) throw(RSGISOutputStreamException)
	{
		string outputFilename = filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#c2DScatter" << endl;
			outTxtFile << "#x,y,c" << endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << "," << c[i] << endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::export3DColourScatter(string filename, double *x, double *y, double *z, double *c, int numPts) throw(RSGISOutputStreamException)
	{
		string outputFilename = filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#c3DScatter" << endl;
			outTxtFile << "#x,y,z,c" << endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << "," << z[i] << "," << c[i] << endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}			
	}
	
	void RSGISExportForPlotting::exportSurface(string filename, double *x, double *y, double *z, int numPts) throw(RSGISOutputStreamException)
	{
		string outputFilename = filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#Surface" << endl;
			outTxtFile << "#x,y,z" << endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << "," << z[i] << endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::exportColourSurface(string filename, double *x, double *y, double *z, double *c, int numPts) throw(RSGISOutputStreamException)
	{
		string outputFilename = filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#cSurface" << endl;
			outTxtFile << "#x,y,z,c" << endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << "," << z[i] << "," << c[i] << endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}			
	}
	
	void RSGISExportForPlotting::exportTriangles2d(string filename, double *x, double *y, unsigned long numPts) throw(RSGISOutputStreamException)
	{
		string outputFilename = filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#triangle2d" << endl;
			outTxtFile << "#x1,y1" << endl;
			outTxtFile << "#x2,y2" << endl;
			outTxtFile << "#x3,y3" << endl;
			
			for(unsigned int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}			
		
	}
	
	void RSGISExportForPlotting::exportTriangles3d(string filename, double *x, double *y, double *z, unsigned long numPts) throw(RSGISOutputStreamException)
	{
		string outputFilename = filepath + filename + string(".ptxt");
		ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), ios::out | ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#triangle3d" << endl;
			outTxtFile << "#x1,y1,z1" << endl;
			outTxtFile << "#x2,y2,z2" << endl;
			outTxtFile << "#x3,y3,z3" << endl;
			
			for(unsigned int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << "," << z[i] << endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw RSGISOutputStreamException("Could not open text file.");
		}			
		
	}
	
	RSGISExportForPlotting::~RSGISExportForPlotting()
	{
		instance = NULL;
	}	
}}


