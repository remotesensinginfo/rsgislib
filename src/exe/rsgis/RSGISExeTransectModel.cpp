/*
 *  RSGISExeTransectModel.cpp
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

#include "RSGISExeTransectModel.h"

namespace rsgisexe{

    RSGISExeTransectModel::RSGISExeTransectModel() : rsgis::RSGISAlgorithmParameters()
{
	this->algorithm = "transectModel";
	this->option = RSGISExeTransectModel::none;
	this->transectWidth = 0;
	this->transectLenght = 0;
	this->transectHeight = 0;
	this->nTrees = 0;
	
}

    rsgis::RSGISAlgorithmParameters* RSGISExeTransectModel::getInstance()
{
	return new RSGISExeTransectModel();
}

    void RSGISExeTransectModel::retrieveParameters(DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException)
{
	/******************************
	 * READ IN VARIABLES FROM XML * 
	 ******************************/
	
	rsgis::math::RSGISMathsUtils mathUtils;
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	
	// Options
	XMLCh *typeSingleSpeciesCanopyRandom = XMLString::transcode("singleSpeciesCanopyRandom");
	XMLCh *typeSingleSpeciesCanopyPsudoRandom = XMLString::transcode("singleSpeciesCanopyPsudoRandom");
	XMLCh *typeSingleSpeciesRandom = XMLString::transcode("singleSpeciesRandom");
	XMLCh *typeSingleSpeciesPsudoRandom = XMLString::transcode("singleSpeciesPsudoRandom");
	XMLCh *typeSingleSpeciesCanopyRandomList = XMLString::transcode("singleSpeciesCanopyRandomList");
	XMLCh *typeSingleSpeciesCanopyPsudoRandomList = XMLString::transcode("singleSpeciesCanopyPsudoRandomList");
	XMLCh *typeSingleSpeciesRandomList = XMLString::transcode("singleSpeciesRandomList");
	XMLCh *typeSingleSpeciesPsudoRandomList = XMLString::transcode("singleSpeciesPsudoRandomList");
	XMLCh *typeVisualiseModelInput = XMLString::transcode("visualiseModelInput");
	
	// Species
	XMLCh *speciesAHaropohylla = XMLString::transcode("aHarpophylla");
	
	const XMLCh *algorNameEle = argElement->getAttribute(XMLString::transcode("algor"));
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw rsgis::RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	
	const XMLCh *optionStr = argElement->getAttribute(XMLString::transcode("option"));
	if(XMLString::equals(typeSingleSpeciesCanopyRandom,optionStr)) // Single species, simulate one canopy. Random numbers seeded using system time.
	{
		this->option = RSGISExeTransectModel::singleSpeciesCanopyRandom;
		// Get transect width
		XMLCh *transectWidthCh = XMLString::transcode("transectWidth");
		if(argElement->hasAttribute(transectWidthCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectWidthCh));
			string transectWidthStr = string(charValue);
			this->transectWidth = mathUtils.strtoint(transectWidthStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect width");
		}
		XMLString::release(&transectWidthCh);
		
		// Get transect lenght
		XMLCh *transectLenghtCh = XMLString::transcode("transectLenght");
		if(argElement->hasAttribute(transectLenghtCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectLenghtCh));
			string transectLengthStr = string(charValue);
			this->transectLenght = mathUtils.strtoint(transectLengthStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect lenght");
		}
		XMLString::release(&transectLenghtCh);
		
		// Get transect height
		XMLCh *transectHeightCh = XMLString::transcode("transectHeight");
		if(argElement->hasAttribute(transectHeightCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectHeightCh));
			string transectHeightStr = string(charValue);
			this->transectHeight = mathUtils.strtoint(transectHeightStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect height");
		}
		XMLString::release(&transectHeightCh);
		
		// Get transect res
		XMLCh *transectResCh = XMLString::transcode("transectRes");
		if(argElement->hasAttribute(transectResCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectResCh));
			string transectResStr = string(charValue);
			this->transectRes = mathUtils.strtodouble(transectResStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect res");
		}
		XMLString::release(&transectResCh);
		
		// Get quadrat size
		XMLCh *qSizeCh = XMLString::transcode("quadratSize");
		if(argElement->hasAttribute(qSizeCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(qSizeCh));
			string qSizeStr = string(charValue);
			this->quadratSize = mathUtils.strtoint(qSizeStr);
			XMLString::release(&charValue);
		}
		else
		{
			cout << "\tNo Quadrat size set using default of 5 m" << endl;
			this->quadratSize = 5;
		}
		XMLString::release(&qSizeCh);
		
		// Get number of trees
		XMLCh *nTreesCh = XMLString::transcode("nTrees");
		if(argElement->hasAttribute(nTreesCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(nTreesCh));
			string nTreesStr = string(charValue);
			this->nTrees = mathUtils.strtoint(nTreesStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for number of trees");
		}
		XMLString::release(&nTreesCh);
		
		// Get tree height
		XMLCh *treeHeightCh = XMLString::transcode("treeHeight");
		if(argElement->hasAttribute(treeHeightCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(treeHeightCh));
			string treeHeightStr = string(charValue);
			this->treeHeight = mathUtils.strtodouble(treeHeightStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for tree height");
		}
		XMLString::release(&nTreesCh);
		
		// Get tree species
		const XMLCh *speciesStr = argElement->getAttribute(XMLString::transcode("species"));
		if(XMLString::equals(speciesAHaropohylla,speciesStr))
		{
			this->species = aHarpophylla;
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Species not recognised");
		}
		
		// Check for output to image
		XMLCh *outImageCh = XMLString::transcode("outImage");
		if(argElement->hasAttribute(outImageCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outImageCh));
			this->outImageFile = string(charValue);
			this->exportImage = true;
			cout << "\tSaving transect as image file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportImage = false;
		}
		XMLString::release(&outImageCh);
		
		// Check for output to vector
		XMLCh *outVectorCh = XMLString::transcode("outVector");
		if(argElement->hasAttribute(outVectorCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outVectorCh));
			this->outVectorFile = string(charValue);
			this->exportVector = true;
			cout << "\tSaving transect as vector file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportVector = false;
		}
		XMLString::release(&outVectorCh);
		
		// Check for output to pTxt
		XMLCh *outPlotCh = XMLString::transcode("outPlotName");
		if(argElement->hasAttribute(outPlotCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outPlotCh));
			this->outPlotName = string(charValue);
			this->exportPtxt = true;
			cout << "\tSaving transect as plot file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportPtxt = false;
		}
		XMLString::release(&outPlotCh);
		
		// Check for calculate FPC
		XMLCh *calculateFPCCh = XMLString::transcode("calculateFPC");
		if(argElement->hasAttribute(calculateFPCCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(calculateFPCCh));
			string calcFPCStr = string(charValue);
			if (calcFPCStr == "yes") 
			{
				this->calcFPC = true;
				cout << "\tCalculating FPC from transect" << endl;
				
				// Get number of runs for FPC
				XMLCh *nRunsCh = XMLString::transcode("nRuns");
				if(argElement->hasAttribute(nRunsCh))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(nRunsCh));
					string nRunsStr = string(charValue);
					this->nRuns = mathUtils.strtoint(nRunsStr);
					XMLString::release(&charValue);
				}
				else
				{
					this->nRuns = 1;
				}
				XMLString::release(&nRunsCh);	
			}
			else 
			{
				this->calcFPC = false;
			}
			XMLString::release(&charValue);
		}
		else
		{
			this->calcFPC = false;
		}
		XMLString::release(&calculateFPCCh);
		
		// Check for calculate Canopy Cover
		XMLCh *calculateCCCh = XMLString::transcode("calculateCC");
		if(argElement->hasAttribute(calculateCCCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(calculateCCCh));
			string calcCCStr = string(charValue);
			if (calcCCStr == "yes") 
			{
				this->calcCC = true;
				cout << "\tCalculating Canopy Cover from transect" << endl;
				
				// Get number of runs for Canopy Cover
				XMLCh *nRunsCh = XMLString::transcode("nRuns");
				if(argElement->hasAttribute(nRunsCh))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(nRunsCh));
					string nRunsStr = string(charValue);
					this->nRuns = mathUtils.strtoint(nRunsStr);
					XMLString::release(&charValue);
				}
				else
				{
					this->nRuns = 1;
				}
				XMLString::release(&nRunsCh);	
			}
			else 
			{
				this->calcCC = false;
			}
			XMLString::release(&charValue);
		}
		else
		{
			this->calcCC = false;
		}
		XMLString::release(&calculateCCCh);
		
	}
	else if(XMLString::equals(typeSingleSpeciesCanopyPsudoRandom,optionStr)) // Single species, simulate canopy. Ranodom numbers use seed of 1, allowing repeatability.
	{
		this->option = RSGISExeTransectModel::singleSpeciesCanopyPsudoRandom;
		// Get transect width
		XMLCh *transectWidthCh = XMLString::transcode("transectWidth");
		if(argElement->hasAttribute(transectWidthCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectWidthCh));
			string transectWidthStr = string(charValue);
			this->transectWidth = mathUtils.strtoint(transectWidthStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect width");
		}
		XMLString::release(&transectWidthCh);
		
		// Get transect lenght
		XMLCh *transectLenghtCh = XMLString::transcode("transectLenght");
		if(argElement->hasAttribute(transectLenghtCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectLenghtCh));
			string transectLengthStr = string(charValue);
			this->transectLenght = mathUtils.strtoint(transectLengthStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect lenght");
		}
		XMLString::release(&transectLenghtCh);
		
		// Get transect height
		XMLCh *transectHeightCh = XMLString::transcode("transectHeight");
		if(argElement->hasAttribute(transectHeightCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectHeightCh));
			string transectHeightStr = string(charValue);
			this->transectHeight = mathUtils.strtoint(transectHeightStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect height");
		}
		XMLString::release(&transectHeightCh);
		
		// Get transect res
		XMLCh *transectResCh = XMLString::transcode("transectRes");
		if(argElement->hasAttribute(transectResCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectResCh));
			string transectResStr = string(charValue);
			this->transectRes = mathUtils.strtodouble(transectResStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect res");
		}
		XMLString::release(&transectResCh);
		
		// Get quadrat size
		XMLCh *qSizeCh = XMLString::transcode("quadratSize");
		if(argElement->hasAttribute(qSizeCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(qSizeCh));
			string qSizeStr = string(charValue);
			this->quadratSize = mathUtils.strtoint(qSizeStr);
			XMLString::release(&charValue);
		}
		else
		{
			cout << "\tNo Quadrat size set using default of 5 m" << endl;
			this->quadratSize = 5;
		}
		XMLString::release(&qSizeCh);
		
		// Get number of trees
		XMLCh *nTreesCh = XMLString::transcode("nTrees");
		if(argElement->hasAttribute(nTreesCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(nTreesCh));
			string nTreesStr = string(charValue);
			this->nTrees = mathUtils.strtoint(nTreesStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for number of trees");
		}
		XMLString::release(&nTreesCh);
		
		// Get tree height
		XMLCh *treeHeightCh = XMLString::transcode("treeHeight");
		if(argElement->hasAttribute(treeHeightCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(treeHeightCh));
			string treeHeightStr = string(charValue);
			this->treeHeight = mathUtils.strtodouble(treeHeightStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for tree height");
		}
		XMLString::release(&nTreesCh);
		
		// Get tree species
		const XMLCh *speciesStr = argElement->getAttribute(XMLString::transcode("species"));
		if(XMLString::equals(speciesAHaropohylla,speciesStr))
		{
			this->species = aHarpophylla;
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Species not recognised");
		}
		
		// Check for output to image
		XMLCh *outImageCh = XMLString::transcode("outImage");
		if(argElement->hasAttribute(outImageCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outImageCh));
			this->outImageFile = string(charValue);
			this->exportImage = true;
			cout << "\tSaving transect as image file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportImage = false;
		}
		XMLString::release(&outImageCh);
		
		// Check for output to vector
		XMLCh *outVectorCh = XMLString::transcode("outVector");
		if(argElement->hasAttribute(outVectorCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outVectorCh));
			this->outVectorFile = string(charValue);
			this->exportVector = true;
			cout << "\tSaving transect as vector file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportVector = false;
		}
		XMLString::release(&outVectorCh);
		
		// Check for output to pTxt
		XMLCh *outPlotCh = XMLString::transcode("outPlotName");
		if(argElement->hasAttribute(outPlotCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outPlotCh));
			this->outPlotName = string(charValue);
			this->exportPtxt = true;
			cout << "\tSaving transect as plot file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportPtxt = false;
		}
		XMLString::release(&outPlotCh);
		
		// Check for calculate FPC
		XMLCh *calculateFPCCh = XMLString::transcode("calculateFPC");
		if(argElement->hasAttribute(calculateFPCCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(calculateFPCCh));
			string calcFPCStr = string(charValue);
			if (calcFPCStr == "yes") 
			{
				this->calcFPC = true;
				cout << "\tCalculating FPC from transect" << endl;
				
				// Get number of runs for FPC
				XMLCh *nRunsCh = XMLString::transcode("nRuns");
				if(argElement->hasAttribute(nRunsCh))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(nRunsCh));
					string nRunsStr = string(charValue);
					this->nRuns = mathUtils.strtoint(nRunsStr);
					XMLString::release(&charValue);
				}
				else
				{
					this->nRuns = 1;
				}
				XMLString::release(&nRunsCh);	
			}
			else 
			{
				this->calcFPC = false;
			}
			XMLString::release(&charValue);
		}
		else
		{
			this->calcFPC = false;
		}
		XMLString::release(&calculateFPCCh);
		
		// Check for calculate Canopy Cover
		XMLCh *calculateCCCh = XMLString::transcode("calculateCC");
		if(argElement->hasAttribute(calculateCCCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(calculateCCCh));
			string calcCCStr = string(charValue);
			if (calcCCStr == "yes") 
			{
				this->calcCC = true;
				cout << "\tCalculating Canopy Cover from transect" << endl;
				
				// Get number of runs for Canopy Cover
				XMLCh *nRunsCh = XMLString::transcode("nRuns");
				if(argElement->hasAttribute(nRunsCh))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(nRunsCh));
					string nRunsStr = string(charValue);
					this->nRuns = mathUtils.strtoint(nRunsStr);
					XMLString::release(&charValue);
				}
				else
				{
					this->nRuns = 1;
				}
				XMLString::release(&nRunsCh);	
			}
			else 
			{
				this->calcCC = false;
			}
			XMLString::release(&charValue);
		}
		else
		{
			this->calcCC = false;
		}
		XMLString::release(&calculateCCCh);
	}
	else if(XMLString::equals(typeSingleSpeciesRandom,optionStr)) // Single species, simulates trees for one transect. Random numbers seeded using system time.
	{
		this->option = RSGISExeTransectModel::singleSpeciesRandom;
		// Get transect width
		XMLCh *transectWidthCh = XMLString::transcode("transectWidth");
		if(argElement->hasAttribute(transectWidthCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectWidthCh));
			string transectWidthStr = string(charValue);
			this->transectWidth = mathUtils.strtoint(transectWidthStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect width");
		}
		XMLString::release(&transectWidthCh);
		
		// Get transect lenght
		XMLCh *transectLenghtCh = XMLString::transcode("transectLenght");
		if(argElement->hasAttribute(transectLenghtCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectLenghtCh));
			string transectLengthStr = string(charValue);
			this->transectLenght = mathUtils.strtoint(transectLengthStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect lenght");
		}
		XMLString::release(&transectLenghtCh);
		
		// Get transect height
		XMLCh *transectHeightCh = XMLString::transcode("transectHeight");
		if(argElement->hasAttribute(transectHeightCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectHeightCh));
			string transectHeightStr = string(charValue);
			this->transectHeight = mathUtils.strtoint(transectHeightStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect height");
		}
		XMLString::release(&transectHeightCh);
		
		// Get transect res
		XMLCh *transectResCh = XMLString::transcode("transectRes");
		if(argElement->hasAttribute(transectResCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectResCh));
			string transectResStr = string(charValue);
			this->transectRes = mathUtils.strtodouble(transectResStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect res");
		}
		XMLString::release(&transectResCh);
		
		// Get quadrat size
		XMLCh *qSizeCh = XMLString::transcode("quadratSize");
		if(argElement->hasAttribute(qSizeCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(qSizeCh));
			string qSizeStr = string(charValue);
			this->quadratSize = mathUtils.strtoint(qSizeStr);
			XMLString::release(&charValue);
		}
		else
		{
			cout << "\tNo Quadrat size set using default of 5 m" << endl;
			this->quadratSize = 5;
		}
		XMLString::release(&qSizeCh);
		
		// Get number of trees
		XMLCh *nTreesCh = XMLString::transcode("nTrees");
		if(argElement->hasAttribute(nTreesCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(nTreesCh));
			string nTreesStr = string(charValue);
			this->nTrees = mathUtils.strtoint(nTreesStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for number of trees");
		}
		XMLString::release(&nTreesCh);
		
		// Get tree height
		XMLCh *treeHeightCh = XMLString::transcode("treeHeight");
		if(argElement->hasAttribute(treeHeightCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(treeHeightCh));
			string treeHeightStr = string(charValue);
			this->treeHeight = mathUtils.strtodouble(treeHeightStr);
			if(treeHeight > transectHeight)
			{
				transectHeight = treeHeight; // Transect must be at least as hight as tree
			}
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for tree height");
		}
		XMLString::release(&nTreesCh);
		
		// Get tree species
		const XMLCh *speciesStr = argElement->getAttribute(XMLString::transcode("species"));
		if(XMLString::equals(speciesAHaropohylla,speciesStr))
		{
			this->species = aHarpophylla;
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Species not recognised");
		}
		
		// Check for output to image
		XMLCh *outImageCh = XMLString::transcode("outImage");
		if(argElement->hasAttribute(outImageCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outImageCh));
			this->outImageFile = string(charValue);
			this->exportImage = true;
			cout << "\tSaving transect as image file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportImage = false;
		}
		XMLString::release(&outImageCh);
		
		// Check for output to vector
		XMLCh *outVectorCh = XMLString::transcode("outVector");
		if(argElement->hasAttribute(outVectorCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outVectorCh));
			this->outVectorFile = string(charValue);
			this->exportVector = true;
			cout << "\tSaving transect as vector file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportVector = false;
		}
		XMLString::release(&outVectorCh);
		
		// Check for output to pTxt
		XMLCh *outPlotCh = XMLString::transcode("outPlotName");
		if(argElement->hasAttribute(outPlotCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outPlotCh));
			this->outPlotName = string(charValue);
			this->exportPtxt = true;
			cout << "\tSaving transect as plot file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportPtxt = false;
		}
		XMLString::release(&outPlotCh);
		
		// Check for calculate FPC
		XMLCh *calculateFPCCh = XMLString::transcode("calculateFPC");
		if(argElement->hasAttribute(calculateFPCCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(calculateFPCCh));
			string calcFPCStr = string(charValue);
			if (calcFPCStr == "yes") 
			{
				this->calcFPC = true;
				cout << "\tCalculating FPC from transect" << endl;
				
				// Get number of runs for FPC
				XMLCh *nRunsCh = XMLString::transcode("nRuns");
				if(argElement->hasAttribute(nRunsCh))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(nRunsCh));
					string nRunsStr = string(charValue);
					this->nRuns = mathUtils.strtoint(nRunsStr);
					XMLString::release(&charValue);
				}
				else
				{
					this->nRuns = 1;
				}
				XMLString::release(&nRunsCh);	
			}
			else 
			{
				this->calcFPC = false;
			}
			XMLString::release(&charValue);
		}
		else
		{
			this->calcFPC = false;
		}
		XMLString::release(&calculateFPCCh);
		
		// Check for calculate Canopy Cover
		XMLCh *calculateCCCh = XMLString::transcode("calculateCC");
		if(argElement->hasAttribute(calculateCCCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(calculateCCCh));
			string calcCCStr = string(charValue);
			if (calcCCStr == "yes") 
			{
				this->calcCC = true;
				cout << "\tCalculating Canopy Cover from transect" << endl;
				
				// Get number of runs for Canopy Cover
				XMLCh *nRunsCh = XMLString::transcode("nRuns");
				if(argElement->hasAttribute(nRunsCh))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(nRunsCh));
					string nRunsStr = string(charValue);
					this->nRuns = mathUtils.strtoint(nRunsStr);
					XMLString::release(&charValue);
				}
				else
				{
					this->nRuns = 1;
				}
				XMLString::release(&nRunsCh);	
			}
			else 
			{
				this->calcCC = false;
			}
			XMLString::release(&charValue);
		}
		else
		{
			this->calcCC = false;
		}
		XMLString::release(&calculateCCCh);
		
	}
	else if(XMLString::equals(typeSingleSpeciesPsudoRandom,optionStr)) // Single species, simulates trees for one transect. Random numbers use seed of 0, allowing repeatability.
	{
		this->option = RSGISExeTransectModel::singleSpeciesPsudoRandom;
		// Get transect width
		XMLCh *transectWidthCh = XMLString::transcode("transectWidth");
		if(argElement->hasAttribute(transectWidthCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectWidthCh));
			string transectWidthStr = string(charValue);
			this->transectWidth = mathUtils.strtoint(transectWidthStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect width");
		}
		XMLString::release(&transectWidthCh);
		
		// Get transect lenght
		XMLCh *transectLenghtCh = XMLString::transcode("transectLenght");
		if(argElement->hasAttribute(transectLenghtCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectLenghtCh));
			string transectLengthStr = string(charValue);
			this->transectLenght = mathUtils.strtoint(transectLengthStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect lenght");
		}
		XMLString::release(&transectLenghtCh);
		
		// Get transect height
		XMLCh *transectHeightCh = XMLString::transcode("transectHeight");
		if(argElement->hasAttribute(transectHeightCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectHeightCh));
			string transectHeightStr = string(charValue);
			this->transectHeight = mathUtils.strtoint(transectHeightStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect height");
		}
		XMLString::release(&transectHeightCh);
		
		// Get transect res
		XMLCh *transectResCh = XMLString::transcode("transectRes");
		if(argElement->hasAttribute(transectResCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(transectResCh));
			string transectResStr = string(charValue);
			this->transectRes = mathUtils.strtodouble(transectResStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for transect res");
		}
		XMLString::release(&transectResCh);
		
		// Get quadrat size
		XMLCh *qSizeCh = XMLString::transcode("quadratSize");
		if(argElement->hasAttribute(qSizeCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(qSizeCh));
			string qSizeStr = string(charValue);
			this->quadratSize = mathUtils.strtoint(qSizeStr);
			XMLString::release(&charValue);
		}
		else
		{
			cout << "\tNo Quadrat size set using default of 5 m" << endl;
			this->quadratSize = 5;
		}
		XMLString::release(&qSizeCh);
		
		// Get number of trees
		XMLCh *nTreesCh = XMLString::transcode("nTrees");
		if(argElement->hasAttribute(nTreesCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(nTreesCh));
			string nTreesStr = string(charValue);
			this->nTrees = mathUtils.strtoint(nTreesStr);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for number of trees");
		}
		XMLString::release(&nTreesCh);
		
		// Get tree height
		XMLCh *treeHeightCh = XMLString::transcode("treeHeight");
		if(argElement->hasAttribute(treeHeightCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(treeHeightCh));
			string treeHeightStr = string(charValue);
			this->treeHeight = mathUtils.strtodouble(treeHeightStr);
			if(treeHeight > transectHeight)
			{
				transectHeight = treeHeight; // Transect must be at least as hight as tree
			}
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("No value provided for tree height");
		}
		XMLString::release(&nTreesCh);
		
		// Get tree species
		const XMLCh *speciesStr = argElement->getAttribute(XMLString::transcode("species"));
		if(XMLString::equals(speciesAHaropohylla,speciesStr))
		{
			this->species = aHarpophylla;
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Species not recognised");
		}
		
		// Check for output to image
		XMLCh *outImageCh = XMLString::transcode("outImage");
		if(argElement->hasAttribute(outImageCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outImageCh));
			this->outImageFile = string(charValue);
			this->exportImage = true;
			cout << "\tSaving transect as image file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportImage = false;
		}
		XMLString::release(&outImageCh);
		
		// Check for output to vector
		XMLCh *outVectorCh = XMLString::transcode("outVector");
		if(argElement->hasAttribute(outVectorCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outVectorCh));
			this->outVectorFile = string(charValue);
			this->exportVector = true;
			cout << "\tSaving transect as vector file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportVector = false;
		}
		XMLString::release(&outVectorCh);
		
		// Check for output to pTxt
		XMLCh *outPlotCh = XMLString::transcode("outPlotName");
		if(argElement->hasAttribute(outPlotCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outPlotCh));
			this->outPlotName = string(charValue);
			this->exportPtxt = true;
			cout << "\tSaving transect as plot file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportPtxt = false;
		}
		XMLString::release(&outPlotCh);
		
		// Check for calculate FPC
		XMLCh *calculateFPCCh = XMLString::transcode("calculateFPC");
		if(argElement->hasAttribute(calculateFPCCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(calculateFPCCh));
			string calcFPCStr = string(charValue);
			if (calcFPCStr == "yes") 
			{
				this->calcFPC = true;
				cout << "\tCalculating FPC from transect" << endl;
				
				// Get number of runs for FPC
				XMLCh *nRunsCh = XMLString::transcode("nRuns");
				if(argElement->hasAttribute(nRunsCh))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(nRunsCh));
					string nRunsStr = string(charValue);
					this->nRuns = mathUtils.strtoint(nRunsStr);
					XMLString::release(&charValue);
				}
				else
				{
					this->nRuns = 1;
				}
				XMLString::release(&nRunsCh);	
			}
			else 
			{
				this->calcFPC = false;
			}
			XMLString::release(&charValue);
		}
		else
		{
			this->calcFPC = false;
		}
		XMLString::release(&calculateFPCCh);
		
		// Check for calculate Canopy Cover
		XMLCh *calculateCCCh = XMLString::transcode("calculateCC");
		if(argElement->hasAttribute(calculateCCCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(calculateCCCh));
			string calcCCStr = string(charValue);
			if (calcCCStr == "yes") 
			{
				this->calcCC = true;
				cout << "\tCalculating Canopy Cover from transect" << endl;
				
				// Get number of runs for Canopy Cover
				XMLCh *nRunsCh = XMLString::transcode("nRuns");
				if(argElement->hasAttribute(nRunsCh))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(nRunsCh));
					string nRunsStr = string(charValue);
					this->nRuns = mathUtils.strtoint(nRunsStr);
					XMLString::release(&charValue);
				}
				else
				{
					this->nRuns = 1;
				}
				XMLString::release(&nRunsCh);	
			}
			else 
			{
				this->calcCC = false;
			}
			XMLString::release(&charValue);
		}
		else
		{
			this->calcCC = false;
		}
		XMLString::release(&calculateCCCh);
	}
	else if(XMLString::equals(typeSingleSpeciesCanopyRandomList, optionStr) || XMLString::equals(typeSingleSpeciesCanopyPsudoRandomList, optionStr)) // Single species, simulates canopy for a number of transects, with patrameters provided by input text file. Random numbers seeded using system time.
	{
		if(XMLString::equals(typeSingleSpeciesCanopyRandomList, optionStr)){this->option = RSGISExeTransectModel::singleSpeciesCanopyRandomList;}
		else{this->option = RSGISExeTransectModel::singleSpeciesCanopyPsudoRandomList;}
		
		// Always retrieve FPC and Canopy Cover for list
		this->calcFPC = true;
		
		// Retrieve input file
		XMLCh *inputXMLStr = XMLString::transcode("input");
		if(argElement->hasAttribute(inputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
			this->inputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Input parameter file not provided..");
		}
		XMLString::release(&inputXMLStr);
		
		// Retrieve output file
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Output file not provided..");
		}
		XMLString::release(&outputXMLStr);
		
		// Get tree species
		const XMLCh *speciesStr = argElement->getAttribute(XMLString::transcode("species"));
		if(XMLString::equals(speciesAHaropohylla,speciesStr))
		{
			this->species = aHarpophylla;
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Species not recognised");
		}
		
		// Get number of runs for FPC
		XMLCh *nRunsCh = XMLString::transcode("nRuns");
		if(argElement->hasAttribute(nRunsCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(nRunsCh));
			string nRunsStr = string(charValue);
			this->nRuns = mathUtils.strtoint(nRunsStr);
			XMLString::release(&charValue);
		}
		else
		{
			this->nRuns = 1;
		}
		XMLString::release(&nRunsCh);
		
		// Get quadrat size
		XMLCh *qSizeCh = XMLString::transcode("quadratSize");
		if(argElement->hasAttribute(qSizeCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(qSizeCh));
			string qSizeStr = string(charValue);
			this->quadratSize = mathUtils.strtodouble(qSizeStr);
			XMLString::release(&charValue);
		}
		else
		{
			cout << "\tNo Quadrat size set using default of 5 m" << endl;
			this->quadratSize = 5.;
		}
		XMLString::release(&qSizeCh);
		
		// Check for output to image
		XMLCh *outImageCh = XMLString::transcode("outImage");
		if(argElement->hasAttribute(outImageCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outImageCh));
			this->outImageFile = string(charValue);
			this->exportImage = true;
			cout << "\tSaving transects as image files" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportImage = false;
		}
		XMLString::release(&outImageCh);
		
		// Check for output to vector
		XMLCh *outVectorCh = XMLString::transcode("outVector");
		if(argElement->hasAttribute(outVectorCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outVectorCh));
			this->outVectorFile = string(charValue);
			this->exportVector = true;
			cout << "\tSaving transect as vector file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportVector = false;
		}
		XMLString::release(&outVectorCh);
		
		// Check for output to pTxt
		XMLCh *outPlotCh = XMLString::transcode("outPlotName");
		if(argElement->hasAttribute(outPlotCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outPlotCh));
			this->outPlotName = string(charValue);
			this->exportPtxt = true;
			cout << "\tSaving transects as plot files" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportPtxt = false;
		}
		XMLString::release(&outPlotCh);
	}
	else if(XMLString::equals(typeVisualiseModelInput, optionStr)) // Visualise model input. Random numbers use seed of 0, allowing repeatability.
	{
		this->option = RSGISExeTransectModel::visualiseModelInput;
		
		// Retrieve input file
		XMLCh *inputXMLStr = XMLString::transcode("input");
		if(argElement->hasAttribute(inputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
			this->inputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Input parameter file not provided..");
		}
		XMLString::release(&inputXMLStr);
		
		// Retrieve output file
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Output file not provided..");
		}
		XMLString::release(&outputXMLStr);
		
		// Get tree species
		const XMLCh *speciesStr = argElement->getAttribute(XMLString::transcode("species"));
		if(XMLString::equals(speciesAHaropohylla,speciesStr))
		{
			this->species = aHarpophylla;
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Species not recognised");
		}
		
		// Get number of runs for FPC
		XMLCh *nRunsCh = XMLString::transcode("nRuns");
		if(argElement->hasAttribute(nRunsCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(nRunsCh));
			string nRunsStr = string(charValue);
			this->nRuns = mathUtils.strtoint(nRunsStr);
			/*if(nRuns > 1)
			 {
			 cout << "For Psudo-random only one run is required!" << endl;
			 this->nRuns = 1;
			 }*/
			XMLString::release(&charValue);
		}
		else
		{
			this->nRuns = 1;
		}
		XMLString::release(&nRunsCh);
		
		// Get quadrat size
		XMLCh *qSizeCh = XMLString::transcode("quadratSize");
		if(argElement->hasAttribute(qSizeCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(qSizeCh));
			string qSizeStr = string(charValue);
			this->quadratSize = mathUtils.strtodouble(qSizeStr);
			XMLString::release(&charValue);
		}
		else
		{
			cout << "\tNo Quadrat size set using default of 5 m" << endl;
			this->quadratSize = 5.;
		}
		XMLString::release(&qSizeCh);
		
		// Check for output to image
		XMLCh *outImageCh = XMLString::transcode("outImage");
		if(argElement->hasAttribute(outImageCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outImageCh));
			this->outImageFile = string(charValue);
			this->exportImage = true;
			cout << "\tSaving transects as image files" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportImage = false;
		}
		XMLString::release(&outImageCh);
		
		// Check for output to vector
		XMLCh *outVectorCh = XMLString::transcode("outVector");
		if(argElement->hasAttribute(outVectorCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outVectorCh));
			this->outVectorFile = string(charValue);
			this->exportVector = true;
			cout << "\tSaving transect as vector file" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportVector = false;
		}
		XMLString::release(&outVectorCh);
		
		// Check for output to pTxt
		XMLCh *outPlotCh = XMLString::transcode("outPlotName");
		if(argElement->hasAttribute(outPlotCh))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outPlotCh));
			this->outPlotName = string(charValue);
			this->exportPtxt = true;
			cout << "\tSaving transects as plot files" << endl;
			XMLString::release(&charValue);
		}
		else
		{
			this->exportPtxt = false;
		}
		XMLString::release(&outPlotCh);
	}
	else // Throw exception if none of the options above.
	{
		throw rsgis::RSGISXMLArgumentsException("Option not recognised");
	}

	parsed = true;
	
	// Release XML
	XMLString::release(&typeSingleSpeciesCanopyRandom);
	XMLString::release(&typeSingleSpeciesCanopyPsudoRandom);
	XMLString::release(&typeSingleSpeciesRandom);
	XMLString::release(&typeSingleSpeciesPsudoRandom);
	XMLString::release(&typeSingleSpeciesCanopyRandomList);
	XMLString::release(&typeSingleSpeciesCanopyPsudoRandomList);
	XMLString::release(&typeSingleSpeciesRandomList);
	XMLString::release(&typeSingleSpeciesPsudoRandomList);
	XMLString::release(&speciesAHaropohylla);
}

void RSGISExeTransectModel::runAlgorithm() throw(rsgis::RSGISException)
{
	if(!parsed)
	{
		throw rsgis::RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		/**********************
		 * RUN TRANSECT MODEL *
		 **********************/
		
		if (option == RSGISExeTransectModel::singleSpeciesCanopyRandom) 
		{			
			// Set up transect
			unsigned int transectWidthVox = int(this->transectWidth / this->transectRes);
			unsigned int transectLenghtVox = int(this->transectLenght / this->transectRes);
			unsigned int transectHeightVox = int(this->transectHeight / this->transectRes);
			
			RSGISTransect *transect = NULL;
			transect = new RSGISTransect(transectWidthVox, transectLenghtVox, transectHeightVox, transectRes);
			transect->setZero();
			
			RSGISModelTreeCanopy *treeCanopy = NULL; // Set up canopy model
			rsgis::math::RSGISProbDistNeymanTypeA *stemDistro = NULL; // Set up stem distro
			rsgis::math::RSGISProbDistNeymanTypeA *posDistroX = NULL; // Set up stem position offset distrobution
			rsgis::math::RSGISProbDistNeymanTypeA *posDistroY = NULL; // Set up stem position offset distrobution
			
			vector <double> *modelPar = new std::vector<double>; // Set up vector to hold model parameters
			
			// Set up tree distrobutions
			rsgis::math::RSGISProbDistNeymanTypeA *leafPosHDistro = NULL;
			rsgis::math::RSGISRandDistroUniformInt *leafPosVDistro = NULL;
			rsgis::math::RSGISProbDistNeymanTypeA *leafThetaDistro = NULL;
			rsgis::math::RSGISProbDistNeymanTypeA *smallBranchPosHDistro = NULL;
			rsgis::math::RSGISRandDistroUniformInt *smallBranchPosVDistro = NULL;
			rsgis::math::RSGISProbDistNeymanTypeA *smallBranchThetaDistro = NULL;
			rsgis::math::RSGISRandDistroUniformInt *smallBranchPhiDistro = NULL;
			rsgis::math::RSGISProbDistNeymanTypeA *largeBranchPosHDistro = NULL;
			rsgis::math::RSGISRandDistroGamma *largeBranchPosVDistro = NULL;
			rsgis::math::RSGISProbDistNeymanTypeA *largeBranchThetaDistro = NULL;
			rsgis::math::RSGISRandDistroUniformInt *largeBranchPhiDistro = NULL;
			
			if (species == rsgis::utils::aHarpophylla) 
			{
				// Get tree parameters
				RSGISAcaciaHarpophylla *bgl = NULL;
				bgl = new RSGISAcaciaHarpophylla(this->treeHeight);
				
				double leafLenght = bgl->getLeafLength(); // Leaf lenght
				double leafWidth = bgl->getLeafWidth(); // Leaf width
				double leafThickness = bgl->getLeafThickness(); // Leaf thickness
				double leafDensity = bgl->getNumLeaves(); // Number of leaves
				double smallBranchLenght = bgl->getSmallBranchLength(); // Small branch lenght
				double smallBranchRadius = bgl->getSmallBranchRadius(); // Small branch radius
				double smallBranchDensity = bgl->getNumSmallBranches(); // Number of small branches
				double largeBranchLenght = bgl->getLargeBranchLength(); // Large branch lenght
				double largeBranchRadius = bgl->getLargeBranchRadius(); // Large branch radius
				double largeBranchDensity = bgl->getNumLargeBranches(); // Number of large branches
				
				modelPar->push_back(leafLenght);
				modelPar->push_back(leafWidth);
				modelPar->push_back(leafThickness);
				modelPar->push_back(leafDensity); 
				modelPar->push_back(smallBranchLenght); 
				modelPar->push_back(smallBranchRadius); 
				modelPar->push_back(smallBranchDensity); 
				modelPar->push_back(largeBranchLenght); 
				modelPar->push_back(largeBranchRadius); 
				modelPar->push_back(largeBranchDensity);
				
				// Set up tree distrobutions
				leafPosHDistro = new rsgis::math::RSGISProbDistNeymanTypeA(0, (smallBranchLenght / transectRes)/2); // Distribution of leaf loctions (horizontal)
				leafPosVDistro = new rsgis::math::RSGISRandDistroUniformInt((smallBranchLenght / transectRes)/-4, (smallBranchLenght / transectRes)/4);
				leafThetaDistro = new rsgis::math::RSGISProbDistNeymanTypeA(-6, 93); // Distribution of leaf incidence angles
				smallBranchPosHDistro = new rsgis::math::RSGISProbDistNeymanTypeA(0, (smallBranchLenght / transectRes)); // Distribution of small branch locations (horizontal)
				smallBranchPosVDistro = new rsgis::math::RSGISRandDistroUniformInt((smallBranchLenght / transectRes)/-4, (smallBranchLenght / transectRes)/4);
				smallBranchThetaDistro = new rsgis::math::RSGISProbDistNeymanTypeA(72, 40); // Distribution of small branch incidence angles
				smallBranchPhiDistro = new rsgis::math::RSGISRandDistroUniformInt(0,90); // Distribution of small branch angles into the incidence plane
				largeBranchPosHDistro = new rsgis::math::RSGISProbDistNeymanTypeA(0, 50); // Distribution of large branch positions (horizontal)
				largeBranchPosVDistro = new rsgis::math::RSGISRandDistroGamma(1, 0.1); // Distribution of large branch positions (vertical)
				largeBranchThetaDistro = new rsgis::math::RSGISProbDistNeymanTypeA(60, 40); // Distribution of large branch incidence angles
				largeBranchPhiDistro = new rsgis::math::RSGISRandDistroUniformInt(0,360); // Distribution of large branch angles into the incidence plane
				
				// Set up canopy model
				treeCanopy = new RSGISModelTreeCanopy(modelPar, leafPosHDistro, leafPosVDistro, leafThetaDistro,smallBranchPosHDistro, smallBranchPosVDistro, smallBranchThetaDistro, smallBranchPhiDistro, largeBranchPosHDistro, largeBranchPosVDistro, largeBranchThetaDistro, largeBranchPhiDistro);
				
				// Set up stem distrobution
				int neyManA = 4;
				int neyManB = (transectLenght / 2) / neyManA;
				stemDistro = new rsgis::math::RSGISProbDistNeymanTypeA(neyManA,neyManB);
				posDistroX = new rsgis::math::RSGISProbDistNeymanTypeA(0, (transectWidth / this->transectRes)/ 3);
				posDistroY = new rsgis::math::RSGISProbDistNeymanTypeA(0, (this->quadratSize / this->transectRes) / 1);
			}
			else 
			{
				throw rsgis::RSGISXMLArgumentsException("Species not recognised");
			}
			
			/*
			cout << "Leaf lenght = " << modelPar->at(0) << endl;
			cout << "Leaf width = " << modelPar->at(1) << endl; 
			cout << "Leaf thickness = " << modelPar->at(2) << endl;
			cout << "Number of leaves = " << modelPar->at(3) << endl;
			cout << "Small branch lenght = " << modelPar->at(4) << endl; 
			cout << "Small branch radius = " << modelPar->at(5) << endl; 
			cout << "Number of small branches = " << modelPar->at(6) << endl;
			cout << "Large branch lenght = " << modelPar->at(7) << endl; 
			cout << "Large branch radius = " << modelPar->at(8) << endl;
			cout << "Number of large branches = " << modelPar->at(9) << endl;
			*/
			
			// Populate transect
			RSGISModelTransect *transectModel = NULL;
			transectModel = new RSGISModelTransect(transect);
			
			std::vector<geos::geom::Polygon*> *canopyPolys = new std::vector<geos::geom::Polygon*>;
			
			if (exportVector == true || calcCC == true) 
			{
				transectModel->createConvexHull(canopyPolys);
			}
			
			transectModel->populateSingleSizeSpecies(this->quadratSize, this->nTrees, stemDistro, posDistroX, posDistroY, treeCanopy);
						
			// Calculate FPC
			if (this->calcFPC == true) 
			{
				RSGISCalcFPC *calcFPC = NULL;
				int fpcSpaceing = int(transectLenghtVox / 100);
				calcFPC = new RSGISCalcFPC(transect);
				double fpc = calcFPC->calcFPCGroundTrans(fpcSpaceing);
				cout << "FPC = " << fpc << endl;
			}
			
			// Calculate Canopy Cover
			if (this->calcCC == true || this->exportVector == true) 
			{
				RSGISCalcCanopyCover *calculateCC = NULL;
				calculateCC = new RSGISCalcCanopyCover(transect, canopyPolys);
				if (calcCC == true) 
				{
					double canopyCover = calculateCC->calcCanopyCover();
					cout << "Canopy Cover = " << canopyCover << endl;
				}
				if (exportVector == true) 
				{
					calculateCC->exportCanopyPoly(outVectorFile);
				}
			}
			
			// Export as image
			if (this->exportImage == true)
			{
				transect->exportImage(this->outImageFile);
			}
			
			// Export as pTxt
			if (this->exportPtxt == true) 
			{
				unsigned int nPoints = transect->countPoints();	
				transect->exportPlot(this->outPlotName, nPoints);
			}
			
			// Free memory
			delete transect;
			delete modelPar;
			delete transectModel;
			delete treeCanopy;
			delete stemDistro;
			delete posDistroX;
			delete posDistroY;
			delete leafPosHDistro;
			delete leafPosVDistro;
			delete leafThetaDistro;
			delete smallBranchPosHDistro;
			delete smallBranchPosVDistro;
			delete smallBranchThetaDistro;
			delete smallBranchPhiDistro;
			delete largeBranchPosHDistro;
			delete largeBranchPosVDistro;
			delete largeBranchThetaDistro;
			delete largeBranchPhiDistro;
		}
		else if (option == RSGISExeTransectModel::singleSpeciesCanopyPsudoRandom) 
		{			
			// Set up transect
			unsigned int transectWidthVox = int(this->transectWidth / this->transectRes);
			unsigned int transectLenghtVox = int(this->transectLenght / this->transectRes);
			unsigned int transectHeightVox = int(this->transectHeight / this->transectRes);
			
			RSGISTransect *transect = NULL;
			transect = new RSGISTransect(transectWidthVox, transectLenghtVox, transectHeightVox, transectRes);
			transect->setZero();
			
			RSGISModelTreeCanopy *treeCanopy = NULL; // Set up canopy model
			rsgis::math::RSGISProbDistNeymanTypeA *stemDistro = NULL; // Set up stem distro
			rsgis::math::RSGISPsudoRandDistroUniformInt *posDistroX = NULL; // Set up stem position offset distrobution
			rsgis::math::RSGISPsudoRandDistroUniformInt *posDistroY = NULL; // Set up stem position offset distrobution
			vector <double> *modelPar = new std::vector<double>; // Set up vector to hold model parameters
			
			// Set up tree distrobutions
			rsgis::math::RSGISPsudoRandDistroGaussian *leafPosHDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroUniformInt *leafPosVDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGaussian *leafThetaDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGaussian *smallBranchPosHDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroUniformInt *smallBranchPosVDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGaussian *smallBranchThetaDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroUniformInt *smallBranchPhiDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGaussian *largeBranchPosHDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGamma *largeBranchPosVDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGaussian *largeBranchThetaDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroUniformInt *largeBranchPhiDistro = NULL;
			
			if (species == aHarpophylla) 
			{
				// Get tree parameters
				RSGISAcaciaHarpophylla *bgl = NULL;
				bgl = new RSGISAcaciaHarpophylla(this->treeHeight);
				
				double leafLenght = bgl->getLeafLength(); // Leaf lenght
				double leafWidth = bgl->getLeafWidth(); // Leaf width
				double leafThickness = bgl->getLeafThickness(); // Leaf thickness
				double leafDensity = bgl->getNumLeaves(); // Number of leaves
				double smallBranchLenght = bgl->getSmallBranchLength(); // Small branch lenght
				double smallBranchRadius = bgl->getSmallBranchRadius(); // Small branch radius
				double smallBranchDensity = bgl->getNumSmallBranches(); // Number of small branches
				double largeBranchLenght = bgl->getLargeBranchLength(); // Large branch lenght
				double largeBranchRadius = bgl->getLargeBranchRadius(); // Large branch radius
				double largeBranchDensity = bgl->getNumLargeBranches(); // Number of large branches
				
				modelPar->push_back(leafLenght);
				modelPar->push_back(leafWidth);
				modelPar->push_back(leafThickness);
				modelPar->push_back(leafDensity); 
				modelPar->push_back(smallBranchLenght); 
				modelPar->push_back(smallBranchRadius); 
				modelPar->push_back(smallBranchDensity); 
				modelPar->push_back(largeBranchLenght); 
				modelPar->push_back(largeBranchRadius); 
				modelPar->push_back(largeBranchDensity);
				
				// Set up tree distrobutions
				leafPosHDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(0, (smallBranchLenght / transectRes)/2); // Distribution of leaf loctions (horizontal);
				leafPosVDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt((smallBranchLenght / transectRes)/-4, (smallBranchLenght / transectRes)/4);
				leafThetaDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(-6, 93); // Distribution of leaf incidence angles
				smallBranchPosHDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(0, (smallBranchLenght / transectRes)); // Distribution of small branch locations (horizontal)
				smallBranchPosVDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt((smallBranchLenght / transectRes)/-4, (smallBranchLenght / transectRes)/4);
				smallBranchThetaDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(72, 40); // Distribution of small branch incidence angles
				smallBranchPhiDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,90); // Distribution of small branch angles into the incidence plane
				largeBranchPosHDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(0, 50); // Distribution of large branch positions (horizontal)
				largeBranchPosVDistro = new rsgis::math::RSGISPsudoRandDistroGamma(1, 0.1); // Distribution of large branch positions (vertical)
				largeBranchThetaDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(60, 40); // Distribution of large branch incidence angles
				largeBranchPhiDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,360); // Distribution of large branch angles into the incidence plane
				
				// Set up canopy model
				treeCanopy = new RSGISModelTreeCanopy(modelPar, leafPosHDistro, leafPosVDistro, leafThetaDistro,smallBranchPosHDistro, smallBranchPosVDistro, smallBranchThetaDistro, smallBranchPhiDistro, largeBranchPosHDistro, largeBranchPosVDistro, largeBranchThetaDistro, largeBranchPhiDistro);
				
				// Set up stem distrobution
				int neyManA = 4;
				int neyManB = (transectLenght / 2) / neyManA;
				stemDistro = new rsgis::math::RSGISProbDistNeymanTypeA(neyManA,neyManB);
				
				posDistroX = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,int((transectWidth / this->transectRes)));
				posDistroY = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,int((this->quadratSize / this->transectRes)));
			}
			else 
			{
				throw rsgis::RSGISXMLArgumentsException("Species not recognised");
			}
			
			/*
			cout << "Leaf lenght = " << modelPar->at(0) << endl;
			cout << "Leaf width = " << modelPar->at(1) << endl; 
			cout << "Leaf thickness = " << modelPar->at(2) << endl;
			cout << "Number of leaves = " << modelPar->at(3) << endl;
			cout << "Small branch lenght = " << modelPar->at(4) << endl; 
			cout << "Small branch radius = " << modelPar->at(5) << endl; 
			cout << "Number of small branches = " << modelPar->at(6) << endl;
			cout << "Large branch lenght = " << modelPar->at(7) << endl; 
			cout << "Large branch radius = " << modelPar->at(8) << endl;
			cout << "Number of large branches = " << modelPar->at(9) << endl;
			*/
			
			// Populate transect
			RSGISModelTransect *transectModel = NULL;
			transectModel = new RSGISModelTransect(transect);
			
			std::vector<geos::geom::Polygon*> *canopyPolys = new std::vector<geos::geom::Polygon*>;
			
			if (exportVector == true || calcCC == true) 
			{
				transectModel->createConvexHull(canopyPolys);
			}
			
			transectModel->populateSingleSizeSpecies(this->quadratSize, this->nTrees, stemDistro, posDistroX, posDistroY, treeCanopy);
			
			
			// Calculate FPC
			if (this->calcFPC == true) 
			{
				RSGISCalcFPC *calcFPC = NULL;
				int fpcSpaceing = int(transectLenghtVox / 100);
				calcFPC = new RSGISCalcFPC(transect);
				double fpc = calcFPC->calcFPCGroundTrans(fpcSpaceing);
				cout << "FPC = " << fpc << endl;
			}
			
			// Calculate Canopy Cover
			if (this->calcCC == true || this->exportVector == true) 
			{
				RSGISCalcCanopyCover *calculateCC = NULL;
				calculateCC = new RSGISCalcCanopyCover(transect, canopyPolys);
				if (calcCC == true) 
				{
					double canopyCover = calculateCC->calcCanopyCover();
					cout << "Canopy Cover = " << canopyCover << endl;
				}
				if (exportVector == true) 
				{
					calculateCC->exportCanopyPoly(outVectorFile);
				}
			}
			
			// Export as image
			if (this->exportImage == true)
			{
				transect->exportImage(this->outImageFile);
			}
			
			// Export as pTxt
			if (this->exportPtxt == true) 
			{
				unsigned int nPoints = transect->countPoints();	
				transect->exportPlot(this->outPlotName, nPoints);
			}
			
			// Free memory
			delete transect;
			delete modelPar;
			delete transectModel;
			delete treeCanopy;
			delete stemDistro;
			delete posDistroX;
			delete posDistroY;
			delete leafPosHDistro;
			delete leafPosVDistro;
			delete leafThetaDistro;
			delete smallBranchPosHDistro;
			delete smallBranchPosVDistro;
			delete smallBranchThetaDistro;
			delete smallBranchPhiDistro;
			delete largeBranchPosHDistro;
			delete largeBranchPosVDistro;
			delete largeBranchThetaDistro;
			delete largeBranchPhiDistro;
		}
		else if (option == RSGISExeTransectModel::singleSpeciesRandom) 
		{			
			// Set up transect
			unsigned int transectWidthVox = int(this->transectWidth / this->transectRes);
			unsigned int transectLenghtVox = int(this->transectLenght / this->transectRes);
			unsigned int transectHeightVox = int(this->transectHeight / this->transectRes);
			
			RSGISTransect *transect = NULL;
			transect = new RSGISTransect(transectWidthVox, transectLenghtVox, transectHeightVox, transectRes);
			transect->setZero();
			
			RSGISModelTree *treeModel = NULL; // Set up tree model
			rsgis::math::RSGISProbDistNeymanTypeA *stemDistro = NULL; // Set up stem distro
			rsgis::math::RSGISProbDistNeymanTypeA *posDistroX = NULL; // Set up stem position offset distrobution
			rsgis::math::RSGISProbDistNeymanTypeA *posDistroY = NULL; // Set up stem position offset distrobution
			vector <double> *modelPar = new std::vector<double>; // Set up vector to hold model parameters
			
			// Set up tree distrobutions
			rsgis::math::RSGISProbDistNeymanTypeA *leafPosHDistro = NULL;
			rsgis::math::RSGISRandDistroUniformInt *leafPosVDistro = NULL;
			rsgis::math::RSGISProbDistNeymanTypeA *leafThetaDistro = NULL;
			rsgis::math::RSGISProbDistNeymanTypeA *smallBranchPosHDistro = NULL;
			rsgis::math::RSGISRandDistroUniformInt *smallBranchPosVDistro = NULL;
			rsgis::math::RSGISProbDistNeymanTypeA *smallBranchThetaDistro = NULL;
			rsgis::math::RSGISRandDistroUniformInt *smallBranchPhiDistro = NULL;
			rsgis::math::RSGISProbDistNeymanTypeA *largeBranchPosHDistro = NULL;
			rsgis::math::RSGISRandDistroGamma *largeBranchPosVDistro = NULL;
			rsgis::math::RSGISProbDistNeymanTypeA *largeBranchThetaDistro = NULL;
			rsgis::math::RSGISRandDistroUniformInt *largeBranchPhiDistro = NULL;
			
			if (species == aHarpophylla) 
			{
				// Get tree parameters
				RSGISAcaciaHarpophylla *bgl = NULL;
				bgl = new RSGISAcaciaHarpophylla(this->treeHeight);
				
				double stemHeight = bgl->getStemHeight(); // Stem height
				double stemRadius = bgl->getStemRadius(); // Stem radius
				double canopyDepth = bgl->getCanopyDepth(); // Canopy depth
				double leafLenght = bgl->getLeafLength(); // Leaf lenght
				double leafWidth = bgl->getLeafWidth(); // Leaf width
				double leafThickness = bgl->getLeafThickness(); // Leaf thickness
				double leafDensity = bgl->getNumLeaves(); // Number of leaves
				double smallBranchLenght = bgl->getSmallBranchLength(); // Small branch lenght
				double smallBranchRadius = bgl->getSmallBranchRadius(); // Small branch radius
				double smallBranchDensity = bgl->getNumSmallBranches(); // Number of small branches
				double largeBranchLenght = bgl->getLargeBranchLength(); // Large branch lenght
				double largeBranchRadius = bgl->getLargeBranchRadius(); // Large branch radius
				double largeBranchDensity = bgl->getNumLargeBranches(); // Number of large branches
				
				
				modelPar->push_back(stemHeight);
				modelPar->push_back(stemRadius);
				modelPar->push_back(canopyDepth);
				modelPar->push_back(leafLenght);
				modelPar->push_back(leafWidth);
				modelPar->push_back(leafThickness);
				modelPar->push_back(leafDensity); 
				modelPar->push_back(smallBranchLenght); 
				modelPar->push_back(smallBranchRadius); 
				modelPar->push_back(smallBranchDensity); 
				modelPar->push_back(largeBranchLenght); 
				modelPar->push_back(largeBranchRadius); 
				modelPar->push_back(largeBranchDensity);
				
				// Set up tree distrobutions
				leafPosHDistro = new rsgis::math::RSGISProbDistNeymanTypeA(0, (smallBranchLenght / transectRes)/2); // Distribution of leaf loctions (horizontal)
				leafPosVDistro = new rsgis::math::RSGISRandDistroUniformInt((smallBranchLenght / transectRes)/-4, (smallBranchLenght / transectRes)/4);
				leafThetaDistro = new rsgis::math::RSGISProbDistNeymanTypeA(-6, 93); // Distribution of leaf incidence angles
				smallBranchPosHDistro = new rsgis::math::RSGISProbDistNeymanTypeA(0, (smallBranchLenght / transectRes)); // Distribution of small branch locations (horizontal)
				smallBranchPosVDistro = new rsgis::math::RSGISRandDistroUniformInt((smallBranchLenght / transectRes)/-4, (smallBranchLenght / transectRes)/4);
				smallBranchThetaDistro = new rsgis::math::RSGISProbDistNeymanTypeA(72, 40); // Distribution of small branch incidence angles
				smallBranchPhiDistro = new rsgis::math::RSGISRandDistroUniformInt(0,90); // Distribution of small branch angles into the incidence plane
				largeBranchPosHDistro = new rsgis::math::RSGISProbDistNeymanTypeA(0, 50); // Distribution of large branch positions (horizontal)
				largeBranchPosVDistro = new rsgis::math::RSGISRandDistroGamma(1, 0.1); // Distribution of large branch positions (vertical)
				largeBranchThetaDistro = new rsgis::math::RSGISProbDistNeymanTypeA(60, 40); // Distribution of large branch incidence angles
				largeBranchPhiDistro = new rsgis::math::RSGISRandDistroUniformInt(0,360); // Distribution of large branch angles into the incidence plane
				
				// Set up tree model
				treeModel = new RSGISModelTree(modelPar, leafPosHDistro, leafPosVDistro, leafThetaDistro,smallBranchPosHDistro, smallBranchPosVDistro, smallBranchThetaDistro, smallBranchPhiDistro, largeBranchPosHDistro, largeBranchPosVDistro, largeBranchThetaDistro, largeBranchPhiDistro);
				// Set up stem distrobution
				int neyManA = 4;
				int neyManB = (transectLenght / 2) / neyManA;
				stemDistro = new rsgis::math::RSGISProbDistNeymanTypeA(neyManA,neyManB);
				posDistroX = new rsgis::math::RSGISProbDistNeymanTypeA(0, (transectWidth / this->transectRes)/ 3);
				posDistroY = new rsgis::math::RSGISProbDistNeymanTypeA(0, (this->quadratSize / this->transectRes) / 1);
			}
			else 
			{
				throw rsgis::RSGISXMLArgumentsException("Species not recognised");
			}
			
			cout << "Stem height = " << modelPar->at(0) << endl;
			cout << "Stem radius = " << modelPar->at(1) << endl;
			cout << "Canopy Depth = " << modelPar->at(2) << endl;
			cout << "Leaf lenght = " << modelPar->at(3) << endl;
			cout << "Leaf width = " << modelPar->at(4) << endl; 
			cout << "Leaf thickness = " << modelPar->at(5) << endl;
			cout << "Number of leaves = " << modelPar->at(6) << endl;
			cout << "Small branch lenght = " << modelPar->at(7) << endl; 
			cout << "Small branch radius = " << modelPar->at(8) << endl; 
			cout << "Number of small branches = " << modelPar->at(9) << endl;
			cout << "Large branch lenght = " << modelPar->at(10) << endl; 
			cout << "Large branch radius = " << modelPar->at(11) << endl;
			cout << "Number of large branches = " << modelPar->at(12) << endl;
			
			
			// Populate transect
			RSGISModelTransect *transectModel = NULL;
			transectModel = new RSGISModelTransect(transect);
			
			std::vector<geos::geom::Polygon*> *canopyPolys = new std::vector<geos::geom::Polygon*>;
			
			if (exportVector == true || calcCC == true) 
			{
				transectModel->createConvexHull(canopyPolys);
			}
			
			transectModel->populateSingleSizeSpecies(this->quadratSize, this->nTrees, stemDistro, posDistroX, posDistroY, treeModel);
			
			// Calculate FPC
			if (this->calcFPC == true) 
			{
				RSGISCalcFPC *calcFPC = NULL;
				int fpcSpaceing = int(transectLenghtVox / 100);
				calcFPC = new RSGISCalcFPC(transect);
				double fpc = calcFPC->calcFPCGroundTrans(fpcSpaceing);
				cout << "FPC = " << fpc << endl;
			}
			
			// Calculate Canopy Cover
			if (this->calcCC == true || this->exportVector == true) 
			{
				RSGISCalcCanopyCover *calculateCC = NULL;
				calculateCC = new RSGISCalcCanopyCover(transect, canopyPolys);
				if (calcCC == true) 
				{
					double canopyCover = calculateCC->calcCanopyCover();
					cout << "Canopy Cover = " << canopyCover << endl;
				}
				if (exportVector == true) 
				{
					calculateCC->exportCanopyPoly(outVectorFile);
				}
			}
			
			// Export as image
			if (this->exportImage == true)
			{
				transect->exportImage(this->outImageFile);
			}
			
			// Export as pTxt
			if (this->exportPtxt == true) 
			{
				unsigned int nPoints = transect->countPoints();	
				transect->exportPlot(this->outPlotName, nPoints);
			}
			
			// Free memory
			delete transect;
			delete modelPar;
			delete transectModel;
			delete treeModel;
			delete stemDistro;
			delete posDistroX;
			delete posDistroY;
			delete leafPosHDistro;
			delete leafPosVDistro;
			delete leafThetaDistro;
			delete smallBranchPosHDistro;
			delete smallBranchPosVDistro;
			delete smallBranchThetaDistro;
			delete smallBranchPhiDistro;
			delete largeBranchPosHDistro;
			delete largeBranchPosVDistro;
			delete largeBranchThetaDistro;
			delete largeBranchPhiDistro;
		}
		else if (option == RSGISExeTransectModel::singleSpeciesPsudoRandom) 
		{			
			// Set up transect
			unsigned int transectWidthVox = int(this->transectWidth / this->transectRes);
			unsigned int transectLenghtVox = int(this->transectLenght / this->transectRes);
			unsigned int transectHeightVox = int(this->transectHeight / this->transectRes);
			
			RSGISTransect *transect = NULL;
			transect = new RSGISTransect(transectWidthVox, transectLenghtVox, transectHeightVox, transectRes);
			transect->setZero();
			
			RSGISModelTree *treeModel = NULL; // Set up tree model
			rsgis::math::RSGISProbDistNeymanTypeA *stemDistro = NULL; // Set up stem distro
			rsgis::math::RSGISPsudoRandDistroUniformInt *posDistroX = NULL; // Set up stem position offset distrobution
			rsgis::math::RSGISPsudoRandDistroUniformInt *posDistroY = NULL; // Set up stem position offset distrobution
			vector <double> *modelPar = new std::vector<double>; // Set up vector to hold model parameters
			
			// Set up tree distrobutions
			rsgis::math::RSGISPsudoRandDistroGaussian *leafPosHDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGaussian *leafPosVDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGaussian *leafThetaDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGaussian *smallBranchPosHDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroUniformInt *smallBranchPosVDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGaussian *smallBranchThetaDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroUniformInt *smallBranchPhiDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGaussian *largeBranchPosHDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGamma *largeBranchPosVDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroGaussian *largeBranchThetaDistro = NULL;
			rsgis::math::RSGISPsudoRandDistroUniformInt *largeBranchPhiDistro = NULL;
			
			if (species == aHarpophylla) 
			{
				// Get tree parameters
				RSGISAcaciaHarpophylla *bgl = NULL;
				bgl = new RSGISAcaciaHarpophylla(this->treeHeight);
				
				double stemHeight = bgl->getStemHeight(); // Stem height
				double stemRadius = bgl->getStemRadius(); // Stem radius
				double canopyDepth = bgl->getCanopyDepth(); // Canopy depth
				double leafLenght = bgl->getLeafLength(); // Leaf lenght
				double leafWidth = bgl->getLeafWidth(); // Leaf width
				double leafThickness = bgl->getLeafThickness(); // Leaf thickness
				double leafDensity = bgl->getNumLeaves(); // Number of leaves
				double smallBranchLenght = bgl->getSmallBranchLength(); // Small branch lenght
				double smallBranchRadius = bgl->getSmallBranchRadius(); // Small branch radius
				double smallBranchDensity = bgl->getNumSmallBranches(); // Number of small branches
				double largeBranchLenght = bgl->getLargeBranchLength(); // Large branch lenght
				double largeBranchRadius = bgl->getLargeBranchRadius(); // Large branch radius
				double largeBranchDensity = bgl->getNumLargeBranches(); // Number of large branches
				
				modelPar->push_back(stemHeight);
				modelPar->push_back(stemRadius);
				modelPar->push_back(canopyDepth);
				modelPar->push_back(leafLenght);
				modelPar->push_back(leafWidth);
				modelPar->push_back(leafThickness);
				modelPar->push_back(leafDensity); 
				modelPar->push_back(smallBranchLenght); 
				modelPar->push_back(smallBranchRadius); 
				modelPar->push_back(smallBranchDensity); 
				modelPar->push_back(largeBranchLenght); 
				modelPar->push_back(largeBranchRadius); 
				modelPar->push_back(largeBranchDensity);
				
				// Set up tree distrobutions
                // Set up tree distrobutions
                leafPosHDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(0, 10); // Distribution of leaf loctions (horizontal)
                leafPosVDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(0, 10);
                leafThetaDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(-6, 93); // Distribution of leaf incidence angles
                smallBranchPosHDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(0, 1 / transectRes); // Distribution of small branch locations (horizontal)
                smallBranchPosVDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt((largeBranchLenght / transectRes) / 2,largeBranchLenght / transectRes); // Start small branches from halfway up large branch.
                smallBranchThetaDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(72, 10); // Distribution of small branch incidence angles
                smallBranchPhiDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,360); // Distribution of small branch angles into the incidence plane
                largeBranchPosHDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(0, 1 / transectRes); // Distribution of large branch positions (horizontal)
                largeBranchPosVDistro = new rsgis::math::RSGISPsudoRandDistroGamma(1, 0.1); // Distribution of large branch positions (vertical)
                largeBranchThetaDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(60, 10); // Distribution of large branch incidence angles
                largeBranchPhiDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,360); // Distribution of large branch angles into the incidence plane
                
								
				// Set up tree model
				treeModel = new RSGISModelTree(modelPar, leafPosHDistro, leafPosVDistro, leafThetaDistro,smallBranchPosHDistro, smallBranchPosVDistro, smallBranchThetaDistro, smallBranchPhiDistro, largeBranchPosHDistro, largeBranchPosVDistro, largeBranchThetaDistro, largeBranchPhiDistro);
				// Set up stem distrobution
				int neyNanB = 6;
				int neyManA = (transectLenght / 2) / neyNanB;
				stemDistro = new rsgis::math::RSGISProbDistNeymanTypeA(neyManA,neyNanB);
				posDistroX = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,int((transectWidth / this->transectRes)));
				posDistroY = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,int((this->quadratSize / this->transectRes)));
			}
			
			else 
			{
				throw rsgis::RSGISXMLArgumentsException("Species not recognised");
			}
			
			/*
			cout << "Stem height = " << modelPar->at(0) << endl;
			cout << "Stem radius = " << modelPar->at(1) << endl;
			cout << "Canopy Depth = " << modelPar->at(2) << endl;
			cout << "Leaf lenght = " << modelPar->at(3) << endl;
			cout << "Leaf width = " << modelPar->at(4) << endl; 
			cout << "Leaf thickness = " << modelPar->at(5) << endl;
			cout << "Number of leaves = " << modelPar->at(6) << endl;
			cout << "Small branch lenght = " << modelPar->at(7) << endl; 
			cout << "Small branch radius = " << modelPar->at(8) << endl; 
			cout << "Number of small branches = " << modelPar->at(9) << endl;
			cout << "Large branch lenght = " << modelPar->at(10) << endl; 
			cout << "Large branch radius = " << modelPar->at(11) << endl;
			cout << "Number of large branches = " << modelPar->at(12) << endl;
			 */
			
			// Populate transect
			RSGISModelTransect *transectModel = NULL;
			transectModel = new RSGISModelTransect(transect);
			
			std::vector<geos::geom::Polygon*> *canopyPolys = new std::vector<geos::geom::Polygon*>;
			
			if (exportVector == true || calcCC == true) 
			{
				transectModel->createConvexHull(canopyPolys);
			}
			
			transectModel->populateSingleSizeSpecies(this->quadratSize, this->nTrees, stemDistro, posDistroX, posDistroY, treeModel);

			
			// Calculate FPC
			if (this->calcFPC == true) 
			{
				RSGISCalcFPC *calcFPC = NULL;
				int fpcSpaceing = int(transectLenghtVox / 100);
				calcFPC = new RSGISCalcFPC(transect);
				//int fpc = calcFPC->calcFPCGroundTrans(fpcSpaceing);
				//int fpc = calcFPC->calcFPCTopTrans(fpcSpaceing);
				double fpcGround = calcFPC->calcFPCGroundTrans(fpcSpaceing);
				double fpcTop = calcFPC->calcFPCTopTrans(fpcSpaceing);
				cout << "FPC Ground = " << fpcGround << endl;
				cout << "FPC Top = " << fpcTop << endl;
			}
			
			// Calculate Canopy Cover
			if (this->calcCC == true || this->exportVector == true) 
			{
				RSGISCalcCanopyCover *calculateCC = NULL;
				calculateCC = new RSGISCalcCanopyCover(transect, canopyPolys);
				if (calcCC == true) 
				{
					double canopyCover = calculateCC->calcCanopyCover();
					cout << "Canopy Cover = " << canopyCover << endl;
				}
				if (exportVector == true) 
				{
					calculateCC->exportCanopyPoly(outVectorFile);
				}
			}
			
			// Export as image
			if (this->exportImage == true)
			{
				transect->exportImage(this->outImageFile);
			}
			
			// Export as pTxt
			if (this->exportPtxt == true) 
			{
				unsigned int nPoints = transect->countPoints();	
				transect->exportPlot(this->outPlotName, nPoints);
			}
			
			// Free memory
			delete transect;
			delete modelPar;
			delete transectModel;
			delete treeModel;
			delete stemDistro;
			delete posDistroX;
			delete posDistroY;
			delete leafPosHDistro;
			delete leafPosVDistro;
			delete leafThetaDistro;
			delete smallBranchPosHDistro;
			delete smallBranchPosVDistro;
			delete smallBranchThetaDistro;
			delete smallBranchPhiDistro;
			delete largeBranchPosHDistro;
			delete largeBranchPosVDistro;
			delete largeBranchThetaDistro;
			delete largeBranchPhiDistro;
		}
		else if (option == RSGISExeTransectModel::singleSpeciesCanopyRandomList) 
		{			
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::RSGISMathsUtils mathUtils;
			gsl_matrix *inPar = matrixUtils.readGSLMatrixFromGridTxt(this->inputFile);
			if(inPar->size2 != 15)
			{
				throw rsgis::RSGISException("The correct number of parameters was not provided");
			}
			
			rsgis::math::Matrix *outPar = matrixUtils.createMatrix(inPar->size1, 10);
			
			unsigned int indexIn = 0;
			unsigned int indexOut = 0;
			
			RSGISTransect *transect = NULL;
			
			double pi = 3.1415926536;
			
			for(unsigned int i = 0; i < inPar->size1; i++)
			{
				this->transectWidth=gsl_matrix_get(inPar, i, 0);
				this->transectLenght=gsl_matrix_get(inPar, i, 1);
				this->transectHeight=gsl_matrix_get(inPar, i, 2);
				this->transectRes=gsl_matrix_get(inPar, i, 3);
				this->nTrees=gsl_matrix_get(inPar, i, 4);
				this->treeHeight=gsl_matrix_get(inPar, i, 5);
				
				/*
				 
				 // For Testing
				unsigned int newSize = 24;
				this->transectRes = 0.02;
				
				this->nTrees = (this->nTrees * (newSize*newSize))/ (this->transectWidth * this->transectLenght);
				
				this->transectWidth = newSize;
				this->transectLenght = newSize;
				
				nRuns = 10;
				
				quadratSize = 10;
				 */
				
				double leafRadii = gsl_matrix_get(inPar, i, 6); // Leaf radii
				double leafLenght = sqrt(pi*(leafRadii*leafRadii)); // Leaf lenght - based on square leaves.
				double leafWidth = leafLenght; // Leaf width
				double leafThickness = gsl_matrix_get(inPar, i, 7); // Leaf thickness
				double leafDensity = gsl_matrix_get(inPar, i, 8); // Number of leaves
				double smallBranchLenght = gsl_matrix_get(inPar, i, 9); // Small branch lenght
				double smallBranchRadius = gsl_matrix_get(inPar, i, 10); // Small branch radius
				double smallBranchDensity = gsl_matrix_get(inPar, i, 11); // Number of small branches
				double largeBranchLenght = gsl_matrix_get(inPar, i, 12); // Large branch lenght
				double largeBranchRadius = gsl_matrix_get(inPar, i, 13); // Large branch radius
				double largeBranchDensity = gsl_matrix_get(inPar, i, 14); // Number of large branches
				
				if(largeBranchDensity == 0){largeBranchLenght = smallBranchLenght;}
				
				/*
				 cout << "transectWidth = " << transectWidth << endl;
				 cout << "transect Lenght = " << transectLenght << endl;
				 cout << "transect Height = " << transectHeight << endl;
				 cout << "transect Res = " << transectRes << endl;
				 cout << "nTrees = " << nTrees << endl;
				 cout << "tree Height = " << treeHeight << endl;
				 */
				
				// Set up transect
				unsigned int transectWidthVox = int(this->transectWidth / this->transectRes);
				unsigned int transectLenghtVox = int(this->transectLenght / this->transectRes);
				unsigned int transectHeightVox = int(this->transectHeight / this->transectRes);
				
				if(i == 0)
				{
					transect = new RSGISTransect(transectWidthVox, transectLenghtVox, transectHeightVox, transectRes);
				}
				else
				{
					if (transectWidthVox == transect->getWidth() and transectLenghtVox == transect->getLenth() and transectHeightVox == transect->getHeight()) 
					{
						// If transect is the same size, reuse instead of re-allocating memory
						transect->setZero();
					}
					else 
					{
						delete transect;
						transect = new RSGISTransect(transectWidthVox, transectLenghtVox, transectHeightVox, transectRes);
						transect->setZero();
					}					
				}
				
				RSGISModelTreeCanopy *treeCanopy = NULL; // Set up canopy model
				rsgis::math::RSGISProbDistNeymanTypeA *stemDistro = NULL; // Set up stem distro
				rsgis::math::RSGISProbDistNeymanTypeA *posDistroX = NULL; // Set up stem position offset distrobution
				rsgis::math::RSGISProbDistNeymanTypeA *posDistroY = NULL; // Set up stem position offset distrobution
				vector <double> *modelPar = new std::vector<double>; // Set up vector to hold model parameters
				
				// Set up tree distrobutions
				rsgis::math::RSGISProbDistNeymanTypeA *leafPosHDistro = NULL;
				//rsgis::math::RSGISRandDistroGamma *leafPosHDistro = NULL;
				rsgis::math::RSGISProbDistNeymanTypeA *leafPosVDistro = NULL;
				rsgis::math::RSGISProbDistNeymanTypeA *leafThetaDistro = NULL;
				rsgis::math::RSGISProbDistNeymanTypeA *smallBranchPosHDistro = NULL;
				rsgis::math::RSGISRandDistroUniformInt *smallBranchPosVDistro = NULL;
				rsgis::math::RSGISProbDistNeymanTypeA *smallBranchThetaDistro = NULL;
				rsgis::math::RSGISRandDistroUniformInt *smallBranchPhiDistro = NULL;
				rsgis::math::RSGISProbDistNeymanTypeA *largeBranchPosHDistro = NULL;
				rsgis::math::RSGISRandDistroGamma *largeBranchPosVDistro = NULL;
				rsgis::math::RSGISProbDistNeymanTypeA *largeBranchThetaDistro = NULL;
				rsgis::math::RSGISRandDistroUniformInt *largeBranchPhiDistro = NULL;
				
				/*rsgis::math::RSGISRandDistroUniformInt *leafPosHDistro = NULL;
				//rsgis::math::RSGISRandDistroGamma *leafPosHDistro = NULL;
				rsgis::math::RSGISRandDistroUniformInt *leafPosVDistro = NULL;
				rsgis::math::RSGISRandDistroUniformInt *leafThetaDistro = NULL;
				rsgis::math::RSGISRandDistroUniformInt *smallBranchPosHDistro = NULL;
				rsgis::math::RSGISRandDistroUniformInt *smallBranchPosVDistro = NULL;
				rsgis::math::RSGISProbDistNeymanTypeA *smallBranchThetaDistro = NULL;
				rsgis::math::RSGISRandDistroUniformInt *smallBranchPhiDistro = NULL;
				rsgis::math::RSGISRandDistroUniformInt *largeBranchPosHDistro = NULL;
				rsgis::math::RSGISRandDistroUniformInt *largeBranchPosVDistro = NULL;
				rsgis::math::RSGISProbDistNeymanTypeA *largeBranchThetaDistro = NULL;
				rsgis::math::RSGISRandDistroUniformInt *largeBranchPhiDistro = NULL;*/
				
				if (species == aHarpophylla) 
				{
					// Get tree parameters
					RSGISAcaciaHarpophylla *bgl = NULL;
					bgl = new RSGISAcaciaHarpophylla(this->treeHeight);
					/*
					 double leafLenght = bgl->getLeafLength(); // Leaf lenght
					 double leafWidth = bgl->getLeafWidth(); // Leaf width
					 double leafThickness = bgl->getLeafThickness(); // Leaf thickness
					 double leafDensity = bgl->getNumLeaves(); // Number of leaves
					 double smallBranchLenght = bgl->getSmallBranchLength(); // Small branch lenght
					 double smallBranchRadius = bgl->getSmallBranchRadius(); // Small branch radius
					 double smallBranchDensity = bgl->getNumSmallBranches(); // Number of small branches
					 double largeBranchLenght = bgl->getLargeBranchLength(); // Large branch lenght
					 double largeBranchRadius = bgl->getLargeBranchRadius(); // Large branch radius
					 double largeBranchDensity = bgl->getNumLargeBranches(); // Number of large branches
					*/
					
					modelPar->push_back(leafLenght);
					modelPar->push_back(leafWidth);
					modelPar->push_back(leafThickness);
					modelPar->push_back(leafDensity); 
					modelPar->push_back(smallBranchLenght); 
					modelPar->push_back(smallBranchRadius); 
					modelPar->push_back(smallBranchDensity); 
					modelPar->push_back(largeBranchLenght); 
					modelPar->push_back(largeBranchRadius); 
					modelPar->push_back(largeBranchDensity);
					
					// Set up tree distrobutions
					leafPosHDistro = new rsgis::math::RSGISProbDistNeymanTypeA(0, (smallBranchLenght / transectRes)/3); // Distribution of leaf loctions (horizontal)
					//leafPosHDistro = new rsgis::math::RSGISRandDistroGamma(5.73, gammaScale); // Distribution of leaf loctions (horizontal)
					leafPosVDistro = new rsgis::math::RSGISProbDistNeymanTypeA(0, (smallBranchLenght / transectRes)/3);
					leafThetaDistro = new rsgis::math::RSGISProbDistNeymanTypeA(-6, 93); // Distribution of leaf incidence angles
					smallBranchPosHDistro = new rsgis::math::RSGISProbDistNeymanTypeA(0,(smallBranchLenght / transectRes)/2); // Distribution of small branch locations (horizontal)
					smallBranchPosVDistro = new rsgis::math::RSGISRandDistroUniformInt((largeBranchLenght / transectRes) * 0.667,largeBranchLenght / transectRes);
					smallBranchThetaDistro = new rsgis::math::RSGISProbDistNeymanTypeA(72, 10); // Distribution of small branch incidence angles
					smallBranchPhiDistro = new rsgis::math::RSGISRandDistroUniformInt(0,360); // Distribution of small branch angles into the incidence plane
					largeBranchPosHDistro = new rsgis::math::RSGISProbDistNeymanTypeA(0, (smallBranchLenght / transectRes)/2); // Distribution of large branch positions (horizontal)
					largeBranchPosVDistro = new rsgis::math::RSGISRandDistroGamma(1, 0.1); // Distribution of large m positions (vertical)
					largeBranchThetaDistro = new rsgis::math::RSGISProbDistNeymanTypeA(60, 10); // Distribution of large branch incidence angles
					largeBranchPhiDistro = new rsgis::math::RSGISRandDistroUniformInt(0,360); // Distribution of large branch angles into the incidence plane
					
					//double gammaScale = (0.033*this->treeHeight - 0.039) / transectRes;
					/*int qSizeVox = int(quadratSize / transectRes);
					leafPosHDistro = new rsgis::math::RSGISRandDistroUniformInt(-250, 250); // Distribution of leaf loctions (horizontal)
					//leafPosHDistro = new rsgis::math::RSGISRandDistroGamma(5.73, gammaScale); // Distribution of leaf loctions (horizontal)
					leafPosVDistro = new rsgis::math::RSGISRandDistroUniformInt(0, 4  / transectRes);
					leafThetaDistro = new rsgis::math::RSGISRandDistroUniformInt(-45, 45); // Distribution of leaf incidence angles
					smallBranchPosHDistro = new rsgis::math::RSGISRandDistroUniformInt(-250, 250); // Distribution of small branch locations (horizontal)
					smallBranchPosVDistro = new rsgis::math::RSGISRandDistroUniformInt(0, 4  / transectRes);
					smallBranchThetaDistro = new rsgis::math::RSGISProbDistNeymanTypeA(72, 10); // Distribution of small branch incidence angles
					smallBranchPhiDistro = new rsgis::math::RSGISRandDistroUniformInt(0,360); // Distribution of small branch angles into the incidence plane
					largeBranchPosHDistro = new rsgis::math::RSGISRandDistroUniformInt(-250, 250); // Distribution of large branch positions (horizontal)
					largeBranchPosVDistro = new rsgis::math::RSGISRandDistroUniformInt(0, 4  / transectRes); // Distribution of large m positions (vertical)
					largeBranchThetaDistro = new rsgis::math::RSGISProbDistNeymanTypeA(60, 10); // Distribution of large branch incidence angles
					largeBranchPhiDistro = new rsgis::math::RSGISRandDistroUniformInt(0,360); // Distribution of large branch angles into the incidence plane*/
					
					// Set up canopy model
					treeCanopy = new RSGISModelTreeCanopy(modelPar, leafPosHDistro, leafPosVDistro, leafThetaDistro,smallBranchPosHDistro, smallBranchPosVDistro, smallBranchThetaDistro, smallBranchPhiDistro, largeBranchPosHDistro, largeBranchPosVDistro, largeBranchThetaDistro, largeBranchPhiDistro);
					
					// Set up stem distrobution
					int neyNanB = 6;
					stemDistro = new rsgis::math::RSGISProbDistNeymanTypeA(60 / neyNanB , neyNanB);
					//posDistroX = new rsgis::math::RSGISRandDistroUniformInt(0,int((transectWidth / this->transectRes)/10));
					//posDistroY = new rsgis::math::RSGISRandDistroUniformInt(0,int((this->quadratSize / this->transectRes)/10));
					posDistroX = new rsgis::math::RSGISProbDistNeymanTypeA(0,(quadratSize / transectRes) / 2);
					posDistroY = new rsgis::math::RSGISProbDistNeymanTypeA(0,(quadratSize / transectRes) / 2);
				}
				else 
				{
					throw rsgis::RSGISXMLArgumentsException("Species not recognised");
				}
				
				/*
				 cout << "\tLeaf lenght = " << modelPar->at(0) << endl;
				 cout << "\tLeaf width = " << modelPar->at(1) << endl; 
				 cout << "\tLeaf thickness = " << modelPar->at(2) << endl;
				 cout << "\tNumber of leaves = " << modelPar->at(3) << endl;
				 cout << "\tSmall branch lenght = " << modelPar->at(4) << endl; 
				 cout << "\tSmall branch radius = " << modelPar->at(5) << endl; 
				 cout << "\tNumber of small branches = " << modelPar->at(6) << endl;
				 cout << "\tLarge branch lenght = " << modelPar->at(7) << endl; 
				 cout << "\tLarge branch radius = " << modelPar->at(8) << endl;
				 cout << "\tNumber of large branches = " << modelPar->at(9) << endl;
				 */ 
				
				double totalFPC = 0;
				double averageFPC = 0;
				double totalCanopyCover = 0;
				double averageCanopyCover = 0;
				RSGISModelTransect *transectModel = NULL;
				double *fpcElements = new double[nRuns];
				double *ccElements = new double[nRuns];
				
				for(unsigned int r = 0; r < nRuns; r++)
				{
					// Populate transect
					//cout << "Populating transect" << endl;
					transect->setZero();
					transectModel = new RSGISModelTransect(transect);
					
					std::vector<geos::geom::Polygon*> *canopyPolys = new std::vector<geos::geom::Polygon*>;
					
					transectModel->createConvexHull(canopyPolys);
					
					transectModel->populateSingleSizeSpecies(this->quadratSize, this->nTrees, stemDistro, posDistroX, posDistroY, treeCanopy);
					
					RSGISCalcFPC *calcFPC = NULL;
					RSGISCalcCanopyCover *calculateCC = NULL;
					
					// Calculate FPC
					int fpcSpaceing = int(transectLenghtVox / 100);
					calcFPC = new RSGISCalcFPC(transect);
					double fpc = calcFPC->calcFPCGroundTrans(fpcSpaceing);
					
					totalFPC = totalFPC + fpc;
					fpcElements[r] = fpc;
					
					// Calculate Canopy Cover
					calculateCC = new RSGISCalcCanopyCover(transect, canopyPolys);
					double cc = calculateCC->calcCanopyCover();
					ccElements[r] = cc;
					
					totalCanopyCover = totalCanopyCover + cc;
					
					if(r == 0) // Export images and plot for first run only
					{
						
						// Export as image
						if (this->exportImage == true)
						{
							string outFileName = outImageFile;
							string fileString = mathUtils.inttostring(i + 1);
							outFileName.append("_");
							outFileName.append(fileString);
							transect->exportImage(this->outImageFile);
						}
						
						// Export as pTxt
						if (this->exportPtxt == true) 
						{
							string outFileName = outPlotName;
							string fileString = mathUtils.inttostring(i + 1);
							outFileName.append("_");
							outFileName.append(fileString);
							unsigned int nPoints = transect->countPoints();	
							transect->exportPlot(outFileName, nPoints);
						}
					}
					delete canopyPolys;
				}
				
				// Calculate average FPC
				averageFPC = totalFPC / nRuns;
				averageCanopyCover = totalCanopyCover / nRuns;
				
				double stDevFPC = 0;
				double stDevCC = 0;
				
				for (unsigned int r = 0; r < nRuns; ++r) 
				{
					stDevFPC = stDevFPC + ((fpcElements[r] - averageFPC)*(fpcElements[r] - averageFPC)); 
					stDevCC = stDevCC + ((ccElements[r] - averageCanopyCover)*(ccElements[r] - averageCanopyCover)); 
				}
				
				stDevFPC = sqrt(stDevFPC / nRuns);
				stDevCC = sqrt(stDevCC / nRuns);
				
				cout << "Record " << i + 1 << " of " << inPar->size1 << " - FPC = " << averageFPC << " (± " << stDevFPC << "), Canopy Cover = " << averageCanopyCover << endl;
				
				// Write out parameters				
				outPar->matrix[indexOut] = transectWidth;
				outPar->matrix[indexOut+1] = transectLenght;
				outPar->matrix[indexOut+2] = transectHeight;
				outPar->matrix[indexOut+3] = transectRes;
				outPar->matrix[indexOut+4] = nTrees;
				outPar->matrix[indexOut+5] = treeHeight;
				outPar->matrix[indexOut+6] = averageFPC;
				outPar->matrix[indexOut+7] = averageCanopyCover;
				outPar->matrix[indexOut+8] = stDevFPC;
				outPar->matrix[indexOut+9] = stDevCC;
				
				indexIn = indexIn + 6;
				indexOut = indexOut + 10;
				
				// Free memory
				delete modelPar;
				delete transectModel;
				delete treeCanopy;
				delete stemDistro;
				delete posDistroX;
				delete posDistroY;
				delete leafPosHDistro;
				delete leafPosVDistro;
				delete leafThetaDistro;
				delete smallBranchPosHDistro;
				delete smallBranchPosVDistro;
				delete smallBranchThetaDistro;
				delete smallBranchPhiDistro;
				delete largeBranchPosHDistro;
				delete largeBranchPosVDistro;
				delete largeBranchThetaDistro;
				delete largeBranchPhiDistro;
				delete[] fpcElements;
				delete[] ccElements;
				
			}
			
			matrixUtils.saveMatrix2CSV(outPar, outputFile);
			cout << "Output file saved to: " << outputFile << ".csv" << endl;
			
			delete inPar;
			delete outPar;
			delete transect;
		}
		else if (option == RSGISExeTransectModel::singleSpeciesCanopyPsudoRandomList) 
		{			
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::RSGISMathsUtils mathUtils;
			gsl_matrix *inPar = matrixUtils.readGSLMatrixFromGridTxt(this->inputFile);
			if(inPar->size2 != 15)
			{
				throw rsgis::RSGISException("The correct number of parameters was not provided");
			}
			
			rsgis::math::Matrix *outPar = matrixUtils.createMatrix(inPar->size1, 9);
			
			unsigned int indexIn = 0;
			unsigned int indexOut = 0;
			
			RSGISTransect *transect = NULL;
			
			double pi = 3.1415926536;
			
			for(unsigned int i = 0; i < inPar->size1; i++)
			{
				this->transectWidth=gsl_matrix_get(inPar, i, 0);
				this->transectLenght=gsl_matrix_get(inPar, i, 1);
				this->transectHeight=gsl_matrix_get(inPar, i, 2);
				this->transectRes=gsl_matrix_get(inPar, i, 3);
				this->nTrees=gsl_matrix_get(inPar, i, 4);
				this->treeHeight=gsl_matrix_get(inPar, i, 5);
				
				double leafRadii = gsl_matrix_get(inPar, i, 6); // Leaf radii
				double leafLenght = sqrt(pi*(leafRadii*leafRadii)); // Leaf lenght - based on square leaves.
				double leafWidth = leafLenght; // Leaf width
				double leafThickness = gsl_matrix_get(inPar, i, 7); // Leaf thickness
				double leafDensity = gsl_matrix_get(inPar, i, 8); // Number of leaves
				double smallBranchLenght = gsl_matrix_get(inPar, i, 9); // Small branch lenght
				double smallBranchRadius = sqrt(pi*(gsl_matrix_get(inPar, i, 10)*gsl_matrix_get(inPar, i, 10))); // gsl_matrix_get(inPar, i, 10); // Small branch radius
				double smallBranchDensity = gsl_matrix_get(inPar, i, 11); // Number of small branches
				double largeBranchLenght = gsl_matrix_get(inPar, i, 12); // Large branch lenght
				double largeBranchRadius = sqrt(pi*(gsl_matrix_get(inPar, i, 13)*gsl_matrix_get(inPar, i, 13))); ;//gsl_matrix_get(inPar, i, 13); // Large branch radius
				double largeBranchDensity = gsl_matrix_get(inPar, i, 14); // Number of large branches
				
				if(largeBranchDensity == 0){largeBranchLenght = treeHeight;}
				
				/*
				 cout << "transectWidth = " << transectWidth << endl;
				 cout << "transect Lenght = " << transectLenght << endl;
				 cout << "transect Height = " << transectHeight << endl;
				 cout << "transect Res = " << transectRes << endl;
				 cout << "nTrees = " << nTrees << endl;
				 cout << "tree Height = " << treeHeight << endl;
				*/
				
				// Set up transect
				unsigned int transectWidthVox = int(this->transectWidth / this->transectRes);
				unsigned int transectLenghtVox = int(this->transectLenght / this->transectRes);
				unsigned int transectHeightVox = int(this->transectHeight / this->transectRes);
				
				if(i == 0)
				{
					transect = new RSGISTransect(transectWidthVox, transectLenghtVox, transectHeightVox, transectRes);
				}
				else
				{
					if (transectWidthVox == transect->getWidth() and transectLenghtVox == transect->getLenth() and transectHeightVox == transect->getHeight()) 
					{
						// If transect is the same size, reuse instead of re-allocating memory
						transect->setZero();
					}
					else 
					{
						delete transect;
						transect = new RSGISTransect(transectWidthVox, transectLenghtVox, transectHeightVox, transectRes);
						transect->setZero();
					}					
				}
				
				RSGISModelTreeCanopy *treeCanopy = NULL; // Set up canopy model
				rsgis::math::RSGISPsudoRandDistNeymanTypeA *stemDistro = NULL; // Set up stem distro
				rsgis::math::RSGISPsudoRandDistroGaussian *posDistroX = NULL; // Set up stem position offset distrobution
				rsgis::math::RSGISPsudoRandDistroGaussian *posDistroY = NULL; // Set up stem position offset distrobution
				vector <double> *modelPar = new std::vector<double>; // Set up vector to hold model parameters
				
				// Set up tree distrobutions
				rsgis::math::RSGISPsudoRandDistroGaussian *leafPosHDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroGaussian *leafPosVDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroGaussian *leafThetaDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroGaussian *smallBranchPosHDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroUniformInt *smallBranchPosVDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroGaussian *smallBranchThetaDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroUniformInt *smallBranchPhiDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroGaussian *largeBranchPosHDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroGamma *largeBranchPosVDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroGaussian *largeBranchThetaDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroUniformInt *largeBranchPhiDistro = NULL;
				
				if (species == aHarpophylla) 
				{
					// Get tree parameters
					RSGISAcaciaHarpophylla *bgl = NULL;
					bgl = new RSGISAcaciaHarpophylla(this->treeHeight);
					
					/*double leafLenght = bgl->getLeafLength(); // Leaf lenght
					double leafWidth = bgl->getLeafWidth(); // Leaf width
					double leafThickness = bgl->getLeafThickness(); // Leaf thickness
					double leafDensity = bgl->getNumLeaves(); // Number of leaves
					double smallBranchLenght = bgl->getSmallBranchLength(); // Small branch lenght
					double smallBranchRadius = bgl->getSmallBranchRadius(); // Small branch radius
					double smallBranchDensity = bgl->getNumSmallBranches(); // Number of small branches
					double largeBranchLenght = bgl->getLargeBranchLength(); // Large branch lenght
					double largeBranchRadius = bgl->getLargeBranchRadius(); // Large branch radius
					double largeBranchDensity = bgl->getNumLargeBranches(); // Number of large branches*/
					
					modelPar->push_back(leafLenght);
					modelPar->push_back(leafWidth);
					modelPar->push_back(leafThickness);
					modelPar->push_back(leafDensity); 
					modelPar->push_back(smallBranchLenght); 
					modelPar->push_back(smallBranchRadius); 
					modelPar->push_back(smallBranchDensity); 
					modelPar->push_back(largeBranchLenght); 
					modelPar->push_back(largeBranchRadius); 
					modelPar->push_back(largeBranchDensity);
					
					// Set up tree distrobutions
					//double gammaScale = (0.033*this->treeHeight - 0.039) / transectRes;
					leafPosHDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(0, (smallBranchLenght / transectRes)/4); // Distribution of leaf loctions (horizontal)
					leafPosVDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(0, (smallBranchLenght / transectRes)/4);
					leafThetaDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(-6, 93); // Distribution of leaf incidence angles
					//smallBranchPosHDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(0, 50); // Distribution of small branch locations (horizontal) - 2cm res
					smallBranchPosHDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(0, 0.25 / transectRes); // Distribution of small branch locations (horizontal)
					smallBranchPosVDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt((largeBranchLenght / transectRes) * 0.667,largeBranchLenght / transectRes);
					smallBranchThetaDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(72, 10); // Distribution of small branch incidence angles
					smallBranchPhiDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,360); // Distribution of small branch angles into the incidence plane
					largeBranchPosHDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(0, smallBranchLenght / transectRes); // Distribution of large branch positions (horizontal)
					largeBranchPosVDistro = new rsgis::math::RSGISPsudoRandDistroGamma(1, 0.1); // Distribution of large m positions (vertical)
					largeBranchThetaDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(60, 10); // Distribution of large branch incidence angles
					largeBranchPhiDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,360); // Distribution of large branch angles into the incidence plane
					
					// Set up canopy model
					treeCanopy = new RSGISModelTreeCanopy(modelPar, leafPosHDistro, leafPosVDistro, leafThetaDistro,smallBranchPosHDistro, smallBranchPosVDistro, smallBranchThetaDistro, smallBranchPhiDistro, largeBranchPosHDistro, largeBranchPosVDistro, largeBranchThetaDistro, largeBranchPhiDistro);
					
					// Set up stem distrobution
					int neyNanB = 6;
					stemDistro = new rsgis::math::RSGISPsudoRandDistNeymanTypeA(60 / neyNanB , neyNanB);
					
					//posDistroX = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,int((transectWidth / this->transectRes)/10));
					//posDistroY = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,int((this->quadratSize / this->transectRes)/10));
					posDistroX = new rsgis::math::RSGISPsudoRandDistroGaussian(0,(quadratSize / transectRes) / 2);
					posDistroY = new rsgis::math::RSGISPsudoRandDistroGaussian(0,(quadratSize / transectRes) / 2);
				}
				else 
				{
					throw rsgis::RSGISXMLArgumentsException("Species not recognised");
				}
				
				/*
				 cout << "\tLeaf lenght = " << modelPar->at(0) << endl;
				 cout << "\tLeaf width = " << modelPar->at(1) << endl; 
				 cout << "\tLeaf thickness = " << modelPar->at(2) << endl;
				 cout << "\tNumber of leaves = " << modelPar->at(3) << endl;
				 cout << "\tSmall branch lenght = " << modelPar->at(4) << endl; 
				 cout << "\tSmall branch radius = " << modelPar->at(5) << endl; 
				 cout << "\tNumber of small branches = " << modelPar->at(6) << endl;
				 cout << "\tLarge branch lenght = " << modelPar->at(7) << endl; 
				 cout << "\tLarge branch radius = " << modelPar->at(8) << endl;
				 cout << "\tNumber of large branches = " << modelPar->at(9) << endl;
				*/ 
				
				double totalFPC = 0;
				double averageFPC = 0;
				double totalCanopyCover = 0;
				double averageCanopyCover = 0;
				double *fpcElements = new double[nRuns];
				RSGISModelTransect *transectModel = NULL;
				
				for(unsigned int r = 0; r < nRuns; r++)
				{
					// Populate transect
					//cout << "Populating transect" << endl;
					transect->setZero();
					transectModel = new RSGISModelTransect(transect);
					
					std::vector<geos::geom::Polygon*> *canopyPolys = new std::vector<geos::geom::Polygon*>;
					
					transectModel->createConvexHull(canopyPolys);
					
					transectModel->populateSingleSizeSpecies(this->quadratSize, this->nTrees, stemDistro, posDistroX, posDistroY, treeCanopy);
					
					RSGISCalcFPC *calcFPC = NULL;
					RSGISCalcCanopyCover *calculateCC = NULL;
					
					// Calculate FPC
					calcFPC = new RSGISCalcFPC(transect);
					//double fpc = calcFPC->calcFPCGroundTrans(fpcSpaceing);
					double fpc = calcFPC->calcFPCGroundAll();
					
					totalFPC = totalFPC + fpc;
					fpcElements[r] = fpc; // Save into array
					
					//cout << "\t" << r << ") - FPC = " << fpc << endl;
					
					// Calculate Canopy Cover
					calculateCC = new RSGISCalcCanopyCover(transect, canopyPolys);
					double cc = calculateCC->calcCanopyCover();
					
					totalCanopyCover = totalCanopyCover + cc;
					
					if(r == 0) // Export images and plot for first run only
					{
						
						// Export as image
						if (this->exportImage == true)
						{
							string outFileName = outImageFile;
							string fileString = mathUtils.inttostring(i + 1);
							outFileName.append("_");
							outFileName.append(fileString);
							transect->exportImage(this->outImageFile);
						}
						
						// Export as pTxt
						if (this->exportPtxt == true) 
						{
							string outFileName = outPlotName;
							string fileString = mathUtils.inttostring(i + 1);
							outFileName.append("_");
							outFileName.append(fileString);
							unsigned int nPoints = transect->countPoints();	
							transect->exportPlot(outFileName, nPoints);
						}
					}
					delete canopyPolys;
				}
				
				// Calculate average FPC
				averageFPC = totalFPC / nRuns;
				averageCanopyCover = totalCanopyCover / nRuns;
				double stDevFPC = 0;
				
				for (unsigned int r = 0; r < nRuns; ++r) 
				{
					stDevFPC = stDevFPC + ((fpcElements[r] - averageFPC)*(fpcElements[r] - averageFPC)); 
				}
				
				stDevFPC = sqrt(stDevFPC / nRuns);
				
				cout << "Record " << i + 1 << " of " << inPar->size1 << " - FPC = " << averageFPC << " (± " << stDevFPC << "), Canopy Cover = " << averageCanopyCover << endl;
				
				// Write out parameters				
				outPar->matrix[indexOut] = transectWidth;
				outPar->matrix[indexOut+1] = transectLenght;
				outPar->matrix[indexOut+2] = transectHeight;
				outPar->matrix[indexOut+3] = transectRes;
				outPar->matrix[indexOut+4] = nTrees;
				outPar->matrix[indexOut+5] = treeHeight;
				outPar->matrix[indexOut+6] = averageFPC;
				outPar->matrix[indexOut+7] = averageCanopyCover;
				outPar->matrix[indexOut+8] = stDevFPC;
				
				indexIn = indexIn + 6;
				indexOut = indexOut + 9;
				
				
				// Free memory
				delete modelPar;
				delete transectModel;
				delete treeCanopy;
				delete stemDistro;
				delete posDistroX;
				delete posDistroY;
				delete leafPosHDistro;
				delete leafPosVDistro;
				delete leafThetaDistro;
				delete smallBranchPosHDistro;
				delete smallBranchPosVDistro;
				delete smallBranchThetaDistro;
				delete smallBranchPhiDistro;
				delete largeBranchPosHDistro;
				delete largeBranchPosVDistro;
				delete largeBranchThetaDistro;
				delete largeBranchPhiDistro;
				delete[] fpcElements;
				
			}
			
			matrixUtils.saveMatrix2CSV(outPar, outputFile);
			cout << "Output file saved to: " << outputFile << ".csv" << endl;
			
			delete inPar;
			delete outPar;
			delete transect;
		}
		else if (option == RSGISExeTransectModel::visualiseModelInput) 
		{			
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::RSGISMathsUtils mathUtils;
			
			
			cout << "Reading parameters from: " << this->inputFile << endl;
			gsl_matrix *inPar = matrixUtils.readGSLMatrixFromGridTxt(this->inputFile);
			
			if(inPar->size2 != 42)
			{
				throw rsgis::RSGISException("The correct number of parameters was not provided");
			}
						
			RSGISTransect *transect = NULL;
			
			for(unsigned int i = 0; i < inPar->size1; i++)
			{
				this->transectWidth=50;
				this->transectLenght=50;
				this->transectHeight=10;
				this->transectRes=0.05;
				this->nTrees=gsl_matrix_get(inPar, i, 34) *  this->transectWidth*this->transectLenght; // Trees in 1 ha plot
				this->treeHeight=gsl_matrix_get(inPar, i, 4);
				
				double stemHeight = gsl_matrix_get(inPar, i, 30); // Stem height
				double stemRadius = (gsl_matrix_get(inPar, i, 31) + gsl_matrix_get(inPar, i, 32)) / 2; // Stem radius
				double canopyDepth = gsl_matrix_get(inPar, i, 5); // Canopy depth
				double leafRadii = gsl_matrix_get(inPar, i, 26); // Leaf radii
				double leafLenght = leafRadii;
				double leafWidth = leafRadii;
				double leafThickness = gsl_matrix_get(inPar, i, 25); // Leaf thickness
				double leafDensity = (gsl_matrix_get(inPar, i, 27) * canopyDepth*this->transectWidth*this->transectLenght) / nTrees  ; // Number of leaves
				double smallBranchLenght = gsl_matrix_get(inPar, i, 17); // Small branch lenght
				double smallBranchRadius = (gsl_matrix_get(inPar, i, 18) + gsl_matrix_get(inPar, i, 19)) / 2; // gsl_matrix_get(inPar, i, 10); // Small branch radius
				double smallBranchDensity = (gsl_matrix_get(inPar, i, 20)  * canopyDepth*this->transectWidth*this->transectLenght) / nTrees  ; // Number of small branches
				double smallBranchAngle = gsl_matrix_get(inPar, i, 22); // Angle of small branches, in incidence plane
				double largeBranchLenght = gsl_matrix_get(inPar, i, 9); // Large branch lenght
				double largeBranchRadius = (gsl_matrix_get(inPar, i, 10) + gsl_matrix_get(inPar, i, 11)) / 2; ;//gsl_matrix_get(inPar, i, 13); // Large branch radius
				double largeBranchDensity = (gsl_matrix_get(inPar, i, 12)  * canopyDepth*this->transectWidth*this->transectLenght) / nTrees  ; // Number of large branches
				double largeBranchAngle = gsl_matrix_get(inPar, i, 14); // Angle of small branches, in incidence plane
				
				vector <double> *modelPar = new std::vector<double>; // Set up vector to hold model parameters
				
				modelPar->push_back(stemHeight);
				modelPar->push_back(stemRadius);
				modelPar->push_back(canopyDepth);
				modelPar->push_back(leafLenght); 
				modelPar->push_back(leafWidth); 
				modelPar->push_back(leafThickness); 
				modelPar->push_back(leafDensity); 
				modelPar->push_back(smallBranchLenght); 
				modelPar->push_back(smallBranchRadius); 
				modelPar->push_back(smallBranchDensity);
				modelPar->push_back(largeBranchLenght); 
				modelPar->push_back(largeBranchRadius); 
				modelPar->push_back(largeBranchDensity);

				// Set up transect
				unsigned int transectWidthVox = int(this->transectWidth / this->transectRes);
				unsigned int transectLenghtVox = int(this->transectLenght / this->transectRes);
				unsigned int transectHeightVox = int(this->transectHeight / this->transectRes);
				
				if(i == 0)
				{
					transect = new RSGISTransect(transectWidthVox, transectLenghtVox, transectHeightVox, transectRes);
				}
				else
				{
					if (transectWidthVox == transect->getWidth() and transectLenghtVox == transect->getLenth() and transectHeightVox == transect->getHeight()) 
					{
						// If transect is the same size, reuse instead of re-allocating memory
						transect->setZero();
					}
					else 
					{
						delete transect;
						transect = new RSGISTransect(transectWidthVox, transectLenghtVox, transectHeightVox, transectRes);
						transect->setZero();
					}					
				}
				
				RSGISModelTree *tree = NULL; // Set up canopy model
				rsgis::math::RSGISPsudoRandDistroGaussian *stemDistro = NULL; // Set up stem distro
				rsgis::math::RSGISPsudoRandDistroUniformInt *posDistroX = NULL; // Set up stem position offset distrobution
				rsgis::math::RSGISPsudoRandDistroUniformInt *posDistroY = NULL; // Set up stem position offset distrobution
				
				// Set up tree distrobutions
				rsgis::math::RSGISPsudoRandDistroUniformInt *leafPosHDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroUniformInt *leafPosVDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroGaussian *leafThetaDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroUniformInt *smallBranchPosHDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroUniformInt *smallBranchPosVDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroGaussian *smallBranchThetaDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroUniformInt *smallBranchPhiDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroUniformInt *largeBranchPosHDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroUniformInt *largeBranchPosVDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroGaussian *largeBranchThetaDistro = NULL;
				rsgis::math::RSGISPsudoRandDistroUniformInt *largeBranchPhiDistro = NULL;
				
				// Set up tree distrobutions
				leafPosHDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0, this->transectLenght / this->transectRes);
				leafPosVDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0, canopyDepth / transectRes);
				leafThetaDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(45, 45); // Distribution of leaf incidence angles
				smallBranchPosHDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0, this->transectLenght / this->transectRes); // Distribution of small branch locations (horizontal)
				smallBranchPosVDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0, canopyDepth / transectRes); 
				smallBranchThetaDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(smallBranchAngle, 20); // Distribution of small branch incidence angles
				smallBranchPhiDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,360); // Distribution of small branch angles into the incidence plane
				largeBranchPosHDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0, this->transectLenght / this->transectRes);; // Distribution of large branch positions (horizontal)
				largeBranchPosVDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0, canopyDepth / transectRes); // Distribution of large branch positions (vertical)
				largeBranchThetaDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(largeBranchAngle, 20); // Distribution of large branch incidence angles
				largeBranchPhiDistro = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,360); // Distribution of large branch angles into the incidence plane
				
				// Set up canopy model
				tree = new RSGISModelTree(modelPar, leafPosHDistro, leafPosVDistro, leafThetaDistro,smallBranchPosHDistro, smallBranchPosVDistro, smallBranchThetaDistro, smallBranchPhiDistro, largeBranchPosHDistro, largeBranchPosVDistro, largeBranchThetaDistro, largeBranchPhiDistro);
				
				stemDistro = new rsgis::math::RSGISPsudoRandDistroGaussian(nTrees / (this->transectLenght/ this->quadratSize),1);
				posDistroX = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,this->transectLenght / this->transectRes);
				posDistroY = new rsgis::math::RSGISPsudoRandDistroUniformInt(0,this->transectWidth / this->transectRes);
				
				double totalFPC = 0;
				double averageFPC = 0;
				double totalCanopyCover = 0;
				double averageCanopyCover = 0;
				double *fpcElements = new double[nRuns];
				RSGISModelTransect *transectModel = NULL;
				
				for(unsigned int r = 0; r < nRuns; r++)
				{
					// Populate transect
					//cout << "Populating transect" << endl;
					transect->setZero();
					transectModel = new RSGISModelTransect(transect);
					
					std::vector<geos::geom::Polygon*> *canopyPolys = new std::vector<geos::geom::Polygon*>;
					
					transectModel->createConvexHull(canopyPolys);
					
					transectModel->populateSingleSizeSpecies(this->quadratSize, this->nTrees, stemDistro, posDistroX, posDistroY, tree);
					
					RSGISCalcFPC *calcFPC = NULL;
					RSGISCalcCanopyCover *calculateCC = NULL;
					
					// Calculate FPC
					calcFPC = new RSGISCalcFPC(transect);
					double fpc = calcFPC->calcFPCGroundAll();
					
					totalFPC = totalFPC + fpc;
					fpcElements[r] = fpc; // Save into array
					
					//cout << "\t" << r << ") - FPC = " << fpc << endl;
					
					// Calculate Canopy Cover
					calculateCC = new RSGISCalcCanopyCover(transect, canopyPolys);
					double cc = calculateCC->calcCanopyCover();
					
					totalCanopyCover = totalCanopyCover + cc;
					
					if(r == 0) // Export images and plot for first run only
					{
						
						// Export as image
						if (this->exportImage == true)
						{
							string outFileName = outImageFile;
							string fileString = mathUtils.inttostring(i + 1);
							outFileName.append("_");
							outFileName.append(fileString);
							transect->exportImage(this->outImageFile);
						}
						
						// Export as pTxt
						if (this->exportPtxt == true) 
						{
							string outFileName = outPlotName;
							string fileString = mathUtils.inttostring(i + 1);
							outFileName.append("_");
							outFileName.append(fileString);
							unsigned int nPoints = transect->countPoints();	
							transect->exportPlot(outFileName, nPoints);
						}
					}
					delete canopyPolys;
				}
				
				// Calculate average FPC
				averageFPC = totalFPC / nRuns;
				averageCanopyCover = totalCanopyCover / nRuns;
				double stDevFPC = 0;
				
				for (unsigned int r = 0; r < nRuns; ++r) 
				{
					stDevFPC = stDevFPC + ((fpcElements[r] - averageFPC)*(fpcElements[r] - averageFPC)); 
				}
				
				stDevFPC = sqrt(stDevFPC / nRuns);
				
				cout << "Record " << i + 1 << " of " << inPar->size1 << " - FPC = " << averageFPC << " (± " << stDevFPC << "), Canopy Cover = " << averageCanopyCover << endl;
				
				
				// Free memory
				delete modelPar;
				delete transectModel;
				delete tree;
				delete stemDistro;
				delete posDistroX;
				delete posDistroY;
				delete leafPosHDistro;
				delete leafPosVDistro;
				delete leafThetaDistro;
				delete smallBranchPosHDistro;
				delete smallBranchPosVDistro;
				delete smallBranchThetaDistro;
				delete smallBranchPhiDistro;
				delete largeBranchPosHDistro;
				delete largeBranchPosVDistro;
				delete largeBranchThetaDistro;
				delete largeBranchPhiDistro;
				delete[] fpcElements;
				
			}
			
			delete inPar;
			delete transect;
		}
		else 
		{
			throw rsgis::RSGISXMLArgumentsException("Option not recognised");
		}

	}
}

void RSGISExeTransectModel::printParameters()
{
	
}

string RSGISExeTransectModel::getDescription()
{
	return "Voxel based transect model, may be used to calculate FPC";
}

string RSGISExeTransectModel::getXMLSchema()
{
	return "NOT DONE!";
}

void RSGISExeTransectModel::help()
{
	cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
    cout << "<rsgis:command algor=\"transectModel\" option=\"singleSpeciesCanopyRandomList | singleSpeciesCanopyPsudoRandomList\"" << endl;
    cout << " input=\"input file\" output=\"output file\" species=\"aHarpophylla\" calculateFPC=\"yes | no\"" << endl;
    cout << " calculateCC=\"yes | no\" nRuns=\"int (random only)\" quadratSize=\"float (m)\"" << endl;
    cout << " [outPlotName=\"outPlotName\"] [outImage=\"outImage\"] [outVector=\"outVector\"] />" << endl;
	cout << "</rsgis:commands>" << endl;
}

RSGISExeTransectModel::~RSGISExeTransectModel()
{
	
}
    
    
}
    
