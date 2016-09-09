/*
 *  RSGISAlgorParamsFactory.cpp
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

#include "RSGISAlgorParamsFactory.h"


namespace rsgis
{	
	RSGISAlgorParamsFactory::RSGISAlgorParamsFactory(std::vector<RSGISAlgorithmParameters*> *params)
	{
		this->params = params;
	}
	
	RSGISAlgorithmParameters* RSGISAlgorParamsFactory::getAlgorithmParamterObj(std::string algorithm)
	{
		RSGISAlgorithmParameters *param = NULL;
		
        std::vector<RSGISAlgorithmParameters*>::iterator iterParams;
		for(iterParams = params->begin(); iterParams != params->end(); ++iterParams)
		{
			if((*iterParams)->getAlgorithm() == algorithm)
			{
				param = (*iterParams)->getInstance();
				break;
			}
		}
		
		if(param == NULL)
		{
			throw RSGISXMLArgumentsException("Algorithm not available");
		}
		
		return param;
	}
	
    std::string* RSGISAlgorParamsFactory::availableAlgorithms(int *numAlgor)
	{
		*numAlgor = params->size();
        std::string *algorithms = new std::string[*numAlgor];
		for(int i = 0; i < *numAlgor; i++)
		{
			algorithms[i] = this->params->at(i)->getAlgorithm();
		}
		return algorithms;
	}
	
	RSGISAlgorParamsFactory::~RSGISAlgorParamsFactory()
	{
        std::vector<RSGISAlgorithmParameters*>::iterator iterParams;
		for(iterParams = params->begin(); iterParams != params->end(); )
		{
			delete *iterParams;
			params->erase(iterParams);
		}
	}
}

