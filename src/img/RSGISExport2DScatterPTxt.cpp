/*
 *  RSGISExport2DScatterPTxt.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/10/2009.
 *  Copyright 2009 RSGISLib.
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

#include "RSGISExport2DScatterPTxt.h"

namespace rsgis{namespace img{
	

	RSGISExport2DScatterPTxt::RSGISExport2DScatterPTxt(rsgis::utils::RSGISExportForPlottingIncremental *plotter, int b1, int b2): RSGISCalcImageValue(0)
	{
		this->plotter = plotter;
		this->b1 = b1;
		this->b2 = b2;
	}
	
	void RSGISExport2DScatterPTxt::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented.");
	}
	
	void RSGISExport2DScatterPTxt::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		if((b1 < numBands) & 
		   (b2 < numBands) &
		   (b1 >= 0) &
		   (b2 >= 0))
		{
			try 
			{
				plotter->writeScatter2DLine(bandValues[b1], bandValues[b2]);
			}
			catch (RSGISException &e) 
			{
				throw RSGISImageCalcException(e.what());
			}
			
		}
		else
		{
			throw RSGISImageCalcException("The required bands are not in the image..");
		}
	}
	
	void RSGISExport2DScatterPTxt::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISExport2DScatterPTxt::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented.");
	}
		
	void RSGISExport2DScatterPTxt::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented.");
	}

	bool RSGISExport2DScatterPTxt::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented.");
	}
	
	RSGISExport2DScatterPTxt::~RSGISExport2DScatterPTxt()
	{

	}
	
}}


