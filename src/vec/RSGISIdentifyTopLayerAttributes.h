/*
 *  RSGISIdentifyTopLayerAttributes.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/02/2009.
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


#ifndef RSGISIdentifyTopLayerAttributes_H
#define RSGISIdentifyTopLayerAttributes_H

#include <iostream>
#include <string>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"

#include "datastruct/SortedGenericList.cpp"

#include "math/RSGISMathsUtils.h"

#include "geos/geom/Envelope.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_vec_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace vec{
	
	class DllExport RSGISFloatAttribute
		{
		public:
			RSGISFloatAttribute(std::string name, double value);
			std::string getName() const;
			double getValue() const;
			friend std::ostream& operator<<(std::ostream& ostr, const RSGISFloatAttribute& attribute);
			std::ostream& operator<<(std::ostream& ostr);
			bool operator==(RSGISFloatAttribute attribute) const;
			bool operator!=(RSGISFloatAttribute attribute) const;
			bool operator>(RSGISFloatAttribute attribute) const;
			bool operator<(RSGISFloatAttribute attribute) const;
			bool operator>=(RSGISFloatAttribute attribute) const;
			bool operator<=(RSGISFloatAttribute attribute) const;
			~RSGISFloatAttribute();
		private:
			std::string name;
			double value;
		};
	
	class DllExport RSGISIdentifyTopLayerAttributes : public RSGISProcessOGRFeature
		{
		public:
			RSGISIdentifyTopLayerAttributes(std::string *attributes, int numAttributes, int numTop, rsgis::math::rsgissummarytype summary);
			virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
			virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
			virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
			virtual ~RSGISIdentifyTopLayerAttributes();
		protected:
			std::string *attributes;
			int numAttributes;
			int numTop;
            rsgis::math::rsgissummarytype summary;
            rsgis::datastruct::SortedGenericList<RSGISFloatAttribute> *sortedAttributes;
		};

	
}}

#endif



									   

