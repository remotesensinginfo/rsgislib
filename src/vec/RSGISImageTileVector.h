/*
 *  RSGISImageTileVector.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/11/2008.
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

#ifndef RSGISImageTileVector_H
#define RSGISImageTileVector_H

#include <iostream>
#include <string>

#include "vec/RSGISPolygonData.h"
#include "vec/RSGISVectorUtils.h"

namespace rsgis{namespace vec{
	
	class DllExport RSGISImageTileVector : public RSGISPolygonData
		{
		public:
			RSGISImageTileVector(std::string filenameAttribute);
			virtual void readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn);
			virtual void createLayerDefinition(OGRLayer *outputSHPLayer)throw(RSGISVectorOutputException);
			virtual void populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn);
            std::string getFileName();
			void setFileName(std::string name);
			virtual ~RSGISImageTileVector();
		protected:
            std::string filename;
            std::string filenameAttribute;
		};
}}

#endif


