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
	
	RSGISExportForPlotting::RSGISExportForPlotting(std::string filepath)
	{
		this->filepath = filepath;
		instance = NULL;
	}
	
	RSGISExportForPlotting* RSGISExportForPlotting::getInstance(std::string filepath)
	{
		if(instance == NULL)
		{
			instance = new RSGISExportForPlotting(filepath);
		}
		return instance;
	}
	
	RSGISExportForPlotting* RSGISExportForPlotting::getInstance() 
	{
		if(instance == NULL)
		{
			throw rsgis::RSGISException("Plotter is not instanciated, a filepath needs to be specified: <rsgis:command algor=\"visualisation\" option=\"setupplotter\" outdir=\"std::string\" />");
		}
		return instance;
	}
	
	std::string RSGISExportForPlotting::getFilePath()
	{
		return filepath;
	}
	
	void RSGISExportForPlotting::export2DFreq(std::string filename, std::list<double> *values)
	{
		std::string outputFilename = this->filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#freq2D" << std::endl;
			outTxtFile << "#value" << std::endl;
			
			std::list<double>::iterator iterValues;
			
			for(iterValues = values->begin(); iterValues != values->end(); ++iterValues)
			{
				outTxtFile << (*iterValues) << std::endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}
	}
	
	void RSGISExportForPlotting::export3DFreq(std::string filename, std::list<double> *values1, std::list<double> *values2)
	{
		std::string outputFilename = filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(values1->size() != values2->size())
		{
			throw rsgis::RSGISOutputStreamException("Number of values for 1 and 2 are not equal.");
		}
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#freq3D" << std::endl;
			outTxtFile << "#value1,value2" << std::endl;
			
			std::list<double>::iterator iterValues1;
			std::list<double>::iterator iterValues2;
			
			iterValues1 = values1->begin();
			iterValues2 = values2->begin();
			
			while(iterValues1 != values1->end() & iterValues2 != values2->end())
			{
				outTxtFile << (*iterValues1) << "," << (*iterValues2) << std::endl;
				
				++iterValues1;
				++iterValues2;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::export2DLines(std::string filename, std::list<geos::geom::LineSegment> *lines)
	{
		std::string outputFilename = filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#lines2D" << std::endl;
			outTxtFile << "#x1,y1,x2,y2" << std::endl;
			
			std::list<geos::geom::LineSegment>::iterator iterLines;
			
			for(iterLines = lines->begin(); iterLines != lines->end(); iterLines++)
			{
				outTxtFile << (*iterLines).p0.x << "," << (*iterLines).p0.y << "," << (*iterLines).p1.x << "," << (*iterLines).p1.y << std::endl;
			}
		
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::export3DLines(std::string filename, std::list<geos::geom::LineSegment> *lines)
	{
		std::string outputFilename = filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#lines3D" << std::endl;
			outTxtFile << "#x1,y1,z1,x2,y2,z2" << std::endl;
			
			std::list<geos::geom::LineSegment>::iterator iterLines;
			
			for(iterLines = lines->begin(); iterLines != lines->end(); iterLines++)
			{
				outTxtFile << (*iterLines).p0.x << "," << (*iterLines).p0.y << "," << (*iterLines).p0.z << "," << (*iterLines).p1.x << "," << (*iterLines).p1.y << "," << (*iterLines).p1.z << std::endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::exportDensity(std::string filename, double *x, double *y, int numPts)
	{
		std::string outputFilename = filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#Density" << std::endl;
			outTxtFile << "#x,y" << std::endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << std::endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::export2DScatter(std::string filename, double *x, double *y, int numPts)
	{
		std::string outputFilename = filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#2DScatter" << std::endl;
			outTxtFile << "#x,y" << std::endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << std::endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::export3DScatter(std::string filename, double *x, double *y, double *z, int numPts)
	{
		std::string outputFilename = filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#3DScatter" << std::endl;
			outTxtFile << "#x,y,z" << std::endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << "," << z[i] << std::endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::export2DColourScatter(std::string filename, double *x, double *y, double *c, int numPts)
	{
		std::string outputFilename = filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#c2DScatter" << std::endl;
			outTxtFile << "#x,y,c" << std::endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << "," << c[i] << std::endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::export3DColourScatter(std::string filename, double *x, double *y, double *z, double *c, int numPts)
	{
		std::string outputFilename = filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#c3DScatter" << std::endl;
			outTxtFile << "#x,y,z,c" << std::endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << "," << z[i] << "," << c[i] << std::endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}			
	}
	
	void RSGISExportForPlotting::exportSurface(std::string filename, double *x, double *y, double *z, int numPts)
	{
		std::string outputFilename = filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#Surface" << std::endl;
			outTxtFile << "#x,y,z" << std::endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << "," << z[i] << std::endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}	
	}
	
	void RSGISExportForPlotting::exportColourSurface(std::string filename, double *x, double *y, double *z, double *c, int numPts)
	{
		std::string outputFilename = filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#cSurface" << std::endl;
			outTxtFile << "#x,y,z,c" << std::endl;
			
			for(int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << "," << z[i] << "," << c[i] << std::endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}			
	}
	
	void RSGISExportForPlotting::exportTriangles2d(std::string filename, double *x, double *y, unsigned long numPts)
	{
		std::string outputFilename = filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#triangle2d" << std::endl;
			outTxtFile << "#x1,y1" << std::endl;
			outTxtFile << "#x2,y2" << std::endl;
			outTxtFile << "#x3,y3" << std::endl;
			
			for(unsigned int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << std::endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}			
		
	}
	
	void RSGISExportForPlotting::exportTriangles3d(std::string filename, double *x, double *y, double *z, unsigned long numPts)
	{
		std::string outputFilename = filepath + filename + std::string(".ptxt");
		std::ofstream outTxtFile;
		outTxtFile.open(outputFilename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(10);
			outTxtFile << "#triangle3d" << std::endl;
			outTxtFile << "#x1,y1,z1" << std::endl;
			outTxtFile << "#x2,y2,z2" << std::endl;
			outTxtFile << "#x3,y3,z3" << std::endl;
			
			for(unsigned int i = 0; i < numPts; i++)
			{
				outTxtFile << x[i] << "," << y[i] << "," << z[i] << std::endl;
			}
			
			outTxtFile.flush();
			outTxtFile.close();
		}
		else
		{
			throw rsgis::RSGISOutputStreamException("Could not open text file.");
		}			
		
	}
	
	RSGISExportForPlotting::~RSGISExportForPlotting()
	{
		instance = NULL;
	}	
}}


