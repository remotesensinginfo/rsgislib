/*
 *  RSGIS_ENVI_ASCII_ROI.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/08/2008.
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

#ifndef RSGIS_ENVI_ASCII_ROI_H
#define RSGIS_ENVI_ASCII_ROI_H

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include "utils/RSGISColour.h"
#include "utils/RSGISTextUtils.h"
#include "common/RSGISInputStreamException.h"
#include "math/RSGISMatrices.h"
#include "utils/RSGISENVIROIException.h"

namespace rsgis{namespace utils{
	
    using namespace std;
    using namespace rsgis::math;
    
	struct enviroi
	{
		string name;
		int samples;
		RSGISColour *colour;
		Matrix *data;
	};
	
	class RSGISReadENVIASCIIROI
		{
		public:
			RSGISReadENVIASCIIROI(string file)throw(RSGISInputStreamException,RSGISTextException);
			void printROIs();
			int getNumROIs();
			string* getName(int i) throw(RSGISENVIROIException);
			Matrix* getMatrix(int i) throw(RSGISENVIROIException);
			int getNumSamples(int i) throw(RSGISENVIROIException);
			RSGISColour* getColour(int i) throw(RSGISENVIROIException);
			int getNumVariables();
			enviroi* getENVIROI(int i) throw(RSGISENVIROIException);
			~RSGISReadENVIASCIIROI();
		protected:
			void parsefile() throw(RSGISInputStreamException,RSGISTextException);
			string inputfile;
			enviroi *rois;
			int numrois;
			int numVariables;
		};
}}

#endif
