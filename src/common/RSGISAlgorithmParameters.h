/*
 *  RSGISAlgorithmParameters.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/12/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISAlgorithmParameters_H
#define RSGISAlgorithmParameters_H

#include <string>
#include <iostream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "RSGISException.h"
#include "RSGISXMLArgumentsException.h"

namespace rsgis
{	
	class DllExport RSGISAlgorithmParameters
		{
		public:
			RSGISAlgorithmParameters();
			virtual RSGISAlgorithmParameters* getInstance() = 0;
			virtual void retrieveParameters(xercesc::DOMElement *argElement) throw(RSGISXMLArgumentsException) = 0;
			virtual void runAlgorithm() throw(RSGISException) = 0;
			virtual void printParameters() = 0;
			virtual std::string getDescription() = 0;
			virtual std::string getXMLSchema() = 0;
			virtual void help() = 0;
			virtual std::string getAlgorithm();
			virtual ~RSGISAlgorithmParameters(){};
		protected:
			std::string algorithm;
			bool parsed;
		};
}

#endif

