 /*
 *  RSGISExeTransectModel.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 25/11/2009.
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

#ifndef RSGISExeTransectModel_H
#define RSGISExeTransectModel_H

#include <string>
#include <iostream>
#include <cstdlib>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "utils/RSGISAllometricSpecies.h"
#include "utils/RSGISAllometricEquations.h"
#include "math/RSGISMathsUtils.h"
#include "math/RSGISProbabilityDistributions.h"
#include "math/RSGISProbDistro.h"
#include "math/RSGISRandomDistro.h"
#include "modeling/RSGISTransect.h"
#include "modeling/RSGISModelTransect.h"
#include "modeling/RSGISModelTreeCanopy.h"
#include "modeling/RSGISModelTree.h"
#include "modeling/RSGISCalcFPC.h"
#include "modeling/RSGISCalcCanopyCover.h"
#include "modeling/RSGISModelTreeParameters.h"
#include "modeling/RSGISCalcCanopyCover.h"

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis::utils;
using namespace rsgis::modeling;

    class DllExport RSGISExeTransectModel : public rsgis::RSGISAlgorithmParameters
{
	/**
	 * Code for creating a model transect which may be exported as an image and / or point cloud and / or<br>
	 * used to calculate FPC.<br>
	 * The code may be run on a single set of parameters of a list<br>
	 * For a list the input is a text file containing the following data:
	 * 	transectWidth<br>
	 *  transectLenght<br>
	 *  transectHeight<br>
	 *  transectRes<br>
	 *  nTrees<br>
	 *  treeHeight<br>
	 */
	
	enum options
	{
		none,
		singleSpeciesCanopyRandom,  // Single species, simulates canopy for one transect. Random numbers seeded using system time.
		singleSpeciesCanopyPsudoRandom,  // Single species, simulates canopy for one transect. Random numbers use seed of 0, allowing repeatability.
		singleSpeciesRandom, // Single species, simulates trees for one transect. Random numbers seeded using system time.
		singleSpeciesPsudoRandom, // Single species, simulates trees for one transect.  Random numbers use seed of 0, allowing repeatability.
		singleSpeciesCanopyRandomList, // Single species, simulates canopy for a number of transects, with patrameters provided by input text file. Random numbers seeded using system time.
		singleSpeciesCanopyPsudoRandomList, // Single species, simulates canopy for a number of transects, with patrameters provided by input text file. Random numbers use seed of 0, allowing repeatability.
		singleSpeciesRandomList, // Single species, simulates trees for a number of transects, with patrameters provided by input text file. Random numbers seeded using system time.
		singleSpeciesPsudoRandomList, // Single species, simulates trees for a number of transects, with patrameters provided by input text file. Random numbers use seed of 0, allowing repeatability.
		visualiseModelInput // Visualise input to backscatter model
	};
	
public:
	RSGISExeTransectModel();
	virtual rsgis::RSGISAlgorithmParameters* getInstance();
	virtual void retrieveParameters(DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException);
	virtual void runAlgorithm() throw(rsgis::RSGISException);
	virtual void printParameters();
	virtual string getDescription();
	virtual string getXMLSchema();
	virtual void help();
	~RSGISExeTransectModel();
protected:
	options option;
	unsigned int transectWidth, transectLenght, transectHeight;
	unsigned int nTrees;
	double quadratSize;
	unsigned int nRuns;
	double treeHeight;
	double transectRes;
	treeSpecies species;
	unsigned int treeSize;
	bool calcFPC, calcCC, exportImage, exportPtxt, exportVector;
	string outImageFile;
	string outPlotName;
	string outVectorFile;
	string inputFile, outputFile;
};
}

#endif

