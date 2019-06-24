/*
 *  RSGISExportForPlottingIncremental.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/10/2009.
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

#include "RSGISExportForPlottingIncremental.h"


namespace rsgis{namespace utils{

	RSGISExportForPlottingIncremental::RSGISExportForPlottingIncremental()
	{
		outputFileStream = NULL;
		open = false;
		type = unknown;
	}
	
	bool RSGISExportForPlottingIncremental::openFile(std::string file, PlotTypes inType)
	{
		outputFileStream = new std::ofstream();
		outputFileStream->open(file.c_str(), std::ios::out | std::ios::trunc);
		
		if(!outputFileStream->is_open())
		{
			open = false;
			
			std::string message = std::string("Could not open file ") + file;
			throw rsgis::RSGISOutputStreamException(message);
		}
		open = true;
		
		if(inType == freq2d)
		{
			(*outputFileStream) << "#freq2D" << std::endl;
			(*outputFileStream) << "#value" << std::endl;
		}
		else if(inType == freq3d)
		{
			(*outputFileStream) << "#freq3D" << std::endl;
			(*outputFileStream) << "#value1,value2" << std::endl;
		}
		else if(inType == scatternd)
		{
			(*outputFileStream) << "#NDScatter" << std::endl;
			(*outputFileStream) << "#v1,v2,v3,...vn" << std::endl;
		}
		else if(inType == scatter2d)
		{
			(*outputFileStream) << "#2DScatter" << std::endl;
			(*outputFileStream) << "#x,y" << std::endl;
		}
		else if(inType == scatter3d)
		{
			(*outputFileStream) << "#3DScatter" << std::endl;
			(*outputFileStream) << "#x,y,z" << std::endl;
		}
		else if(inType == cscatter2d)
		{
			(*outputFileStream) << "#c2DScatter" << std::endl;
			(*outputFileStream) << "#x,y,c" << std::endl;
		}
		else if(inType == cscatter3d)
		{
			(*outputFileStream) << "#c3DScatter" << std::endl;
			(*outputFileStream) << "#x,y,z,c" << std::endl;
		}
		else if(inType == density)
		{
			(*outputFileStream) << "#Density" << std::endl;
			(*outputFileStream) << "#x,y" << std::endl;
		}
		else if(inType == surface)
		{
			(*outputFileStream) << "#Surface" << std::endl;
			(*outputFileStream) << "#x,y,z" << std::endl;
		}
		else if(inType == csurface)
		{
			(*outputFileStream) << "#cSurface" << std::endl;
			(*outputFileStream) << "#x,y,z,c" << std::endl;
		}
		else if(inType == lines2d)
		{
			(*outputFileStream) << "#lines2D" << std::endl;
			(*outputFileStream) << "#x1,y1,x2,y2" << std::endl;
		}
		else if(inType == lines3d)
		{
			(*outputFileStream) << "#lines3D" << std::endl;
			(*outputFileStream) << "#x1,y1,z1,x2,y2,z2" << std::endl;
		}
		else if(inType == triangle2d)
		{
			(*outputFileStream) << "#triangle2d" << std::endl;
			(*outputFileStream) << "#x1,y1" << std::endl;
			(*outputFileStream) << "#x2,y2" << std::endl;
			(*outputFileStream) << "#x3,y3" << std::endl;
		}
		else if(inType == triangle3d)
		{
			(*outputFileStream) << "#triangle3d" << std::endl;
			(*outputFileStream) << "#x1,y1,z1" << std::endl;
			(*outputFileStream) << "#x2,y2,z2" << std::endl;
			(*outputFileStream) << "#x3,y3,z3" << std::endl;
		}			
		else 
		{
			throw rsgis::RSGISOutputStreamException("Type is unknown.");
		}
		
		this->type = inType;
		
		return open;
		
	}
	
	void RSGISExportForPlottingIncremental::writeFrequency2DLine(double value)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != freq2d)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << value << std::endl;
	}
	
	void RSGISExportForPlottingIncremental::writeFrequency3DLine(double value1, double value2)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != freq3d)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << value1 << "," << value2 << std::endl;	
	}

	void RSGISExportForPlottingIncremental::writeScatterNDLine(std::vector<double> *vals)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != scatter2d)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		bool first = true;
        std::vector<double>::iterator iterVals;
		for(iterVals = vals->begin(); iterVals != vals->end(); ++iterVals)
		{
			if(first)
			{
				(*outputFileStream) << *iterVals;
			}
			else 
			{
				(*outputFileStream) << "," << *iterVals;
			}

		}
		 (*outputFileStream) << std::endl;	
	}
	
	void RSGISExportForPlottingIncremental::writeScatter2DLine(double x, double y)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != scatter2d)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << std::endl;	
	}
	
	void RSGISExportForPlottingIncremental::writeScatter3DLine(double x, double y, double z)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != scatter3d)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << "," << z << std::endl;
	}
	
	void RSGISExportForPlottingIncremental::writeCScatter2DLine(double x, double y, double c)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != cscatter2d)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << "," << c << std::endl;
	}
	
	void RSGISExportForPlottingIncremental::writeCScatter3DLine(double x, double y, double z, double c)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != cscatter3d)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << "," << z << "," << c << std::endl;
	}
	
	void RSGISExportForPlottingIncremental::writeDensityLine(double x, double y)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != density)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << std::endl;
	}
	
	void RSGISExportForPlottingIncremental::writeSurfaceLine(double x, double y, double z)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != surface)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << "," << z << std::endl;
	}
	
	void RSGISExportForPlottingIncremental::writeCSurfaceLine(double x, double y, double z, double c)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != csurface)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << "," << z << "," << c << std::endl;
	}
	
	void RSGISExportForPlottingIncremental::writeLines2DLine(double x1, double y1, double x2, double y2)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != lines2d)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x1 << "," << y1 << "," << x2 << "," << y2 << std::endl;
	}
	
	void RSGISExportForPlottingIncremental::writeLines3DLine(double x1, double y1, double z1, double x2, double y2, double z2)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != lines3d)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x1 << "," << y1 << "," << z1 << "," << x2 << "," << y2 << "," << z2 << std::endl;
	}
	
	void RSGISExportForPlottingIncremental::writeTriangle2D(double x1, double y1, double x2, double y2, double x3, double y3)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != triangle2d)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type triangle2d");
		}
		
		(*outputFileStream) << x1 << "," << y1 << std::endl;
		(*outputFileStream) << x2 << "," << y2 << std::endl;
		(*outputFileStream) << x3 << "," << y3 << std::endl;
	}
	
	void RSGISExportForPlottingIncremental::writeTriangle3D(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
	{
		if(!open)
		{
			throw rsgis::RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != triangle3d)
		{
			throw rsgis::RSGISOutputStreamException("Open file is not of type triangle3d");
		}
		
		(*outputFileStream) << x1 << "," << y1 << "," << z1 << std::endl;
		(*outputFileStream) << x2 << "," << y2 << "," << z2 << std::endl;
		(*outputFileStream) << x3 << "," << y3 << "," << z3 << std::endl;
	}
	
	PlotTypes RSGISExportForPlottingIncremental::getPlotType()
	{
		return type;
	}
	
	void RSGISExportForPlottingIncremental::close()
	{
		if(open)
		{
			outputFileStream->flush();
			outputFileStream->close();
			delete outputFileStream;
			open = false;
			type = unknown;
		}
		
	}
	
	RSGISExportForPlottingIncremental::~RSGISExportForPlottingIncremental()
	{
		this->close();
	}
}}


