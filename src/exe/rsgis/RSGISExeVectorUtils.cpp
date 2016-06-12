/*
 *  RSGISExeVectorUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/12/2008.
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

#include "RSGISExeVectorUtils.h"

namespace rsgisexe{

RSGISExeVectorUtils::RSGISExeVectorUtils() : RSGISAlgorithmParameters()
{
	this->algorithm = "vectorutils";
	this->inputVectors = NULL;
	this->inputVector= "";
	this->outputVector = "";
	this->output_DIR = "";
	this->areaAttribute = "";
	this->heightAttribute = "";
	this->radiusAttribute = "";
	this->attribute = "";
	this->find = "";
	this->replace = "";
	this->inputRasterVector = "";
	this->buffer = 0;
	this->resolution = 0;
	this->attributes = NULL;
	this->newAttributes = NULL;
	this->numAttributes = 0;
	this->numTop = 0;
	this->summary = rsgis::math::sumtype_value;
	this->force = false;
	this->snap2Grid = false;
	this->tolerance = 0;
	this->dissolve = 0;
	this->area = 0;
	this->curvesegments = 0;
	this->relBorderThreshold = 0;
	this->singlevector = true;
	this->extension = "";
	this->outputPlot = "";
	this->outputImage = "";
}

RSGISAlgorithmParameters* RSGISExeVectorUtils::getInstance()
{
	return new RSGISExeVectorUtils();
}

void RSGISExeVectorUtils::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;
	RSGISFileUtils fileUtils;

	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *rsgisattributeXMLStr = XMLString::transcode("rsgis:attribute");
	XMLCh *optionRemoveAttributes = XMLString::transcode("removeattributes");
	XMLCh *optionBufferVector = XMLString::transcode("buffervector");
	XMLCh *optiongeom2circles = XMLString::transcode("geom2circles");
	XMLCh *optionPrintPolygonGeoms = XMLString::transcode("printpolygeom");
	XMLCh *optionFindReplaceText = XMLString::transcode("findreplacetext");
	XMLCh *optionListAttributes = XMLString::transcode("listattributes");
	XMLCh *optionPrintAttribute = XMLString::transcode("printattribute");
	XMLCh *optionTopAttributes = XMLString::transcode("topattributes");
	XMLCh *optionAddAttributes = XMLString::transcode("addattributes");
	XMLCh *optionCopyPolygons = XMLString::transcode("copypolygons");
	XMLCh *optionPrintFIDs = XMLString::transcode("printfids");
	XMLCh *optionCountPxlsArea = XMLString::transcode("countpxlsarea");
	XMLCh *optionPoints2ASCII = XMLString::transcode("points2ascii");
	XMLCh *optionRotate = XMLString::transcode("rotate");
	XMLCh *optionArea = XMLString::transcode("calcarea");
	XMLCh *optionSplitFeatures = XMLString::transcode("splitfeatures");
	XMLCh *optionRemovePolyOverlaps = XMLString::transcode("removepolyoverlaps");
	XMLCh *optionMergeSmallPolysNear = XMLString::transcode("mergesmallpolysnear");
	XMLCh *optionMergeSmallPolysTouching = XMLString::transcode("mergesmallpolystouching");
	XMLCh *optionMorphClosing= XMLString::transcode("morphologicalclosing");
	XMLCh *optionMorphOpening = XMLString::transcode("morphologicalopening");
	XMLCh *optionMorphErosion = XMLString::transcode("morphologicalerosion");
	XMLCh *optionMorphDilation = XMLString::transcode("morphologicaldilation");
	XMLCh *optionNonConvexDelaunay = XMLString::transcode("nonconvexoutlinedelaunay");
	XMLCh *optionNonConvexLineProj = XMLString::transcode("nonconvexoutlinelineproj");
	XMLCh *optionNonConvexSnakes = XMLString::transcode("nonconvexoutlinesnakes");
	XMLCh *optionConvexOutline = XMLString::transcode("convexoutline");
	XMLCh *optionMergetouchingpolys = XMLString::transcode("mergetouchingpolys");
	XMLCh *optionFreqDist = XMLString::transcode("freqdist");
	XMLCh *optionRasterise = XMLString::transcode("rasterise");
	XMLCh *optionRasterize = XMLString::transcode("rasterize");
    XMLCh *optionBurnRasterise = XMLString::transcode("burnrasterise");
	XMLCh *optionBurnRasterize = XMLString::transcode("burnrasterize");
	XMLCh *optionPolygonPlots = XMLString::transcode("polygonplots");
	XMLCh *optionPolygonImageFootprints = XMLString::transcode("polygonImageFootprints");
	XMLCh *optionPolygoniseMPolys = XMLString::transcode("polygonizempolys");
	XMLCh *optionScatter2D = XMLString::transcode("scatter2D");
	XMLCh *optionCopyCheckPolys = XMLString::transcode("copycheckpolys");
	XMLCh *optionSplitByAttribute = XMLString::transcode("splitbyattribute");
	XMLCh *optionRemoveContainedPolygons = XMLString::transcode("removecontainedpolygons");
	XMLCh *optionRemoveContainedPolygonsDIR = XMLString::transcode("removecontainedpolygonsDIR");
	XMLCh *optionPolygonsInPolygon = XMLString::transcode("polygonsInPolygon");
	XMLCh *optionMergetouchingpolysDIR = XMLString::transcode("mergetouchingpolysDIR");
	XMLCh *optionSplitlargesmall = XMLString::transcode("splitlargesmall");
	XMLCh *optionRemovepolygonscontainedwithingeom = XMLString::transcode("removepolygonscontainedwithingeom");
	XMLCh *optionMergeSHPs = XMLString::transcode("mergeshps");
	XMLCh *optionFixPolyExtBoundary = XMLString::transcode("fixpolyextboundary");
	XMLCh *optionRemoveHoles = XMLString::transcode("removepolyholes");
	XMLCh *optionDropSmallPolys = XMLString::transcode("dropsmallpolys");
	XMLCh *optionExtractLargestPoly = XMLString::transcode("extractlargestpoly");
	XMLCh *optionGenerateSinglePoly = XMLString::transcode("generatesinglepoly");
	XMLCh *optionVectorMaths = XMLString::transcode("vectormaths");
    XMLCh *optionVectorSelect = XMLString::transcode("select");
    XMLCh *optionCalcMeanMinDist = XMLString::transcode("calcmeanmindist");
    XMLCh *optionCopyAssignProj = XMLString::transcode("copyassignproj");
    XMLCh *optionPrintWKT = XMLString::transcode("printwkt");
    XMLCh *optionAddFIDCol = XMLString::transcode("addfidcol");
    XMLCh *optionMinDist2Polys = XMLString::transcode("mindist2polys");
    XMLCh *optionConvexHullGrps = XMLString::transcode("convexhullgrps");

	XMLCh *optionGenerateGrid = XMLString::transcode("generategrid");
	XMLCh *optionGenerateImageGrid = XMLString::transcode("generateimagegrid");

	const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}

	this->inputFileNotVector = false;
	bool noInputProvide = true;
	XMLCh *inputXMLStr = XMLString::transcode("input");
	XMLCh *vectorXMLStr = XMLString::transcode("vector");
	XMLCh *dirXMLStr = XMLString::transcode("dirlist");
	XMLCh *vectortagXMLStr = XMLString::transcode("rsgis:vector");
	if(argElement->hasAttribute(vectorXMLStr))
	{
		this->singlevector = true;
		char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
		this->inputVector = string(charValue);
		XMLString::release(&charValue);
		noInputProvide = false;
	}
	else if(argElement->hasAttribute(inputXMLStr))
	{
		this->inputFileNotVector = true;
		char *charValue = XMLString::transcode(argElement->getAttribute(inputXMLStr));
		this->inputFile = string(charValue);
		XMLString::release(&charValue);
		noInputProvide = false;
	}
	else if(argElement->hasAttribute(dirXMLStr))
	{
		this->singlevector = false;
		char *charValue = XMLString::transcode(argElement->getAttribute(dirXMLStr));
		string inputDIR = string(charValue);
		XMLString::release(&charValue);

		XMLCh *extXMLStr = XMLString::transcode("ext");
		if(argElement->hasAttribute(extXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(extXMLStr));
			this->extension = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'ext\' attribute was provided.");
		}
		XMLString::release(&extXMLStr);

		inputVectors = new list<string>();
		try
		{
			fileUtils.getDIRList(inputDIR, extension, inputVectors, true);
		}
		catch(RSGISException &e)
		{
			throw RSGISXMLArgumentsException(e.what());
		}
		noInputProvide = false;
	}
	else
	{
		DOMNodeList *vectorNodesList = argElement->getElementsByTagName(vectortagXMLStr);
		if(vectorNodesList->getLength() > 0)
		{
			this->singlevector = false;
			inputVectors = new list<string>();
			DOMElement *vectorElement = NULL;
			for(unsigned int i = 0; i < vectorNodesList->getLength(); ++i)
			{
				vectorElement = static_cast<DOMElement*>(vectorNodesList->item(i));

				XMLCh *fileXMLStr = XMLString::transcode("file");
				if(vectorElement->hasAttribute(fileXMLStr))
				{
					char *charValue = XMLString::transcode(vectorElement->getAttribute(fileXMLStr));
					inputVectors->push_back(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'file\' attribute was provided.");
				}
				XMLString::release(&fileXMLStr);
			}
			noInputProvide = false;
		}
		else
		{
			noInputProvide = true;
		}
	}
	XMLString::release(&vectorXMLStr);
	XMLString::release(&dirXMLStr);
	XMLString::release(&vectortagXMLStr);
	XMLString::release(&inputXMLStr);

	const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
	if(XMLString::equals(optionRemoveAttributes, optionXML))
	{
		this->option = RSGISExeVectorUtils::removeAttributes;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}
		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionBufferVector, optionXML))
	{
		this->option = RSGISExeVectorUtils::bufferVector;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		bufferValueInText = false;
		XMLCh *bufferXMLStr = XMLString::transcode("buffer");
		if(argElement->hasAttribute(bufferXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bufferXMLStr));
			buffer = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			XMLCh *bufferTxtXMLStr = XMLString::transcode("buffertxt");
            if(argElement->hasAttribute(bufferTxtXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(bufferTxtXMLStr));
                bufferValueFile = string(charValue);
                XMLString::release(&charValue);
                bufferValueInText = true;
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'buffertxt\' or 'buffer' attribute was provided.");
            }
            XMLString::release(&bufferTxtXMLStr);
		}
		XMLString::release(&bufferXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optiongeom2circles, optionXML))
	{
		this->option = RSGISExeVectorUtils::geom2circles;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *areaattributeXMLStr = XMLString::transcode("areaattribute");
		if(argElement->hasAttribute(areaattributeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(areaattributeXMLStr));
			this->areaAttribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'areaattribute\' attribute was provided.");
		}
		XMLString::release(&areaattributeXMLStr);

		XMLCh *radiusattributeXMLStr = XMLString::transcode("radiusattribute");
		if(argElement->hasAttribute(radiusattributeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(radiusattributeXMLStr));
			this->radiusAttribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'radiusattribute\' attribute was provided.");
		}
		XMLString::release(&radiusattributeXMLStr);


		XMLCh *radiusValueXMLStr = XMLString::transcode("radius");
		if(argElement->hasAttribute(radiusValueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(radiusValueXMLStr));
			this->radius = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'radius\' attribute was provided.");
		}
		XMLString::release(&radiusValueXMLStr);


		XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
		if(argElement->hasAttribute(resolutionXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resolutionXMLStr));
			resolution = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided.");
		}
		XMLString::release(&resolutionXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionPrintPolygonGeoms, optionXML))
	{
		this->option = RSGISExeVectorUtils::printpolygeom;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}
	}
	else if(XMLString::equals(optionFindReplaceText, optionXML))
	{
		this->option = RSGISExeVectorUtils::findreplacetext;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *attributeXMLStr = XMLString::transcode("attribute");
		if(argElement->hasAttribute(attributeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(attributeXMLStr));
			this->attribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'attribute\' attribute was provided.");
		}
		XMLString::release(&attributeXMLStr);


		XMLCh *findXMLStr = XMLString::transcode("find");
		if(argElement->hasAttribute(findXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(findXMLStr));
			this->find = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'find\' attribute was provided.");
		}
		XMLString::release(&findXMLStr);


		XMLCh *replaceXMLStr = XMLString::transcode("replace");
		if(argElement->hasAttribute(replaceXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(replaceXMLStr));
			this->replace = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'replace\' attribute was provided.");
		}
		XMLString::release(&replaceXMLStr);
	}
	else if(XMLString::equals(optionListAttributes, optionXML))
	{
		this->option = RSGISExeVectorUtils::listattributes;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}
	}
	else if(XMLString::equals(optionPrintAttribute, optionXML))
	{
		this->option = RSGISExeVectorUtils::printattribute;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *attributeXMLStr = XMLString::transcode("attribute");
		if(argElement->hasAttribute(attributeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(attributeXMLStr));
			this->attribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'attribute\' attribute was provided.");
		}
		XMLString::release(&attributeXMLStr);

	}
	else if(XMLString::equals(optionTopAttributes, optionXML))
	{
		this->option = RSGISExeVectorUtils::topattributes;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);


		XMLCh *summaryXMLStr = XMLString::transcode("summary");
		if(argElement->hasAttribute(summaryXMLStr))
		{
			const XMLCh *summaryValueStr = argElement->getAttribute(summaryXMLStr);

			XMLCh *valueStr = XMLString::transcode("value");
			XMLCh *aggregateStr = XMLString::transcode("aggregate");

			if(XMLString::equals(summaryValueStr, valueStr))
			{
				this->summary = rsgis::math::sumtype_value;
			}
			else if(XMLString::equals(summaryValueStr, aggregateStr))
			{
				this->summary = rsgis::math::sumtype_aggregate;
			}
			else
			{
				throw RSGISXMLArgumentsException("Summary type not recognised.");
			}

			XMLString::release(&valueStr);
			XMLString::release(&aggregateStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'summary\' attribute was provided.");
		}
		XMLString::release(&summaryXMLStr);


		XMLCh *topXMLStr = XMLString::transcode("top");
		if(argElement->hasAttribute(topXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(topXMLStr));
			this->numTop = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'top\' attribute was provided.");
		}
		XMLString::release(&topXMLStr);


		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);


		DOMNodeList *attributeNodesList = argElement->getElementsByTagName(rsgisattributeXMLStr);
		this->numAttributes = attributeNodesList->getLength();

		//cout << "Found " << this->numAttributes << " attributes" << endl;

		if(numAttributes > 0)
		{
			attributes = new string[numAttributes];
			DOMElement *attributeElement = NULL;
			for(int i = 0; i < numAttributes; i++)
			{
				attributeElement = static_cast<DOMElement*>(attributeNodesList->item(i));


				XMLCh *nameXMLStr = XMLString::transcode("name");
				if(attributeElement->hasAttribute(nameXMLStr))
				{
					char *charValue = XMLString::transcode(attributeElement->getAttribute(nameXMLStr));
					attributes[i] = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
				}
				XMLString::release(&nameXMLStr);
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No attributes \'rsgis:attribute\' tags were provided.");
		}
	}
	else if(XMLString::equals(optionAddAttributes, optionXML))
	{
		this->option = RSGISExeVectorUtils::addattributes;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *OFTIntegerStr = XMLString::transcode("OFTInteger");
		XMLCh *OFTIntegerListStr = XMLString::transcode("OFTIntegerList");
		XMLCh *OFTRealStr = XMLString::transcode("OFTReal");
		XMLCh *OFTRealListStr = XMLString::transcode("OFTRealList");
		XMLCh *OFTStringStr = XMLString::transcode("OFTString");
		XMLCh *OFTStringListStr = XMLString::transcode("OFTStringList");
		XMLCh *OFTWideStringStr = XMLString::transcode("OFTWideString");
		XMLCh *OFTWideStringListStr = XMLString::transcode("OFTWideStringList");
		XMLCh *OFTBinaryStr = XMLString::transcode("OFTBinary");
		XMLCh *OFTDateStr = XMLString::transcode("OFTDate");
		XMLCh *OFTTimeStr = XMLString::transcode("OFTTime");
		XMLCh *OFTDateTimeStr = XMLString::transcode("OFTDateTime");

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		DOMNodeList *attributeNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:attribute"));
		this->numAttributes = attributeNodesList->getLength();

		//cout << "Found " << this->numAttributes << " attributes" << endl;

		if(numAttributes > 0)
		{
			newAttributes = new Attribute*[numAttributes];
			DOMElement *attributeElement = NULL;
			for(int i = 0; i < numAttributes; i++)
			{
				newAttributes[i] = new Attribute();
				attributeElement = static_cast<DOMElement*>(attributeNodesList->item(i));

				XMLCh *nameXMLStr = XMLString::transcode("name");
				if(attributeElement->hasAttribute(nameXMLStr))
				{
					char *charValue = XMLString::transcode(attributeElement->getAttribute(nameXMLStr));
					newAttributes[i]->name = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
				}
				XMLString::release(&nameXMLStr);



				XMLCh *typeXMLStr = XMLString::transcode("type");
				if(attributeElement->hasAttribute(typeXMLStr))
				{
					const XMLCh *typeStr = attributeElement->getAttribute(typeXMLStr);
					if(XMLString::equals(typeStr, OFTIntegerStr))
					{
						newAttributes[i]->type = OFTInteger;
					}
					else if(XMLString::equals(typeStr, OFTIntegerListStr))
					{
						newAttributes[i]->type = OFTIntegerList;
					}
					else if(XMLString::equals(typeStr, OFTRealStr))
					{
						newAttributes[i]->type = OFTReal;
					}
					else if(XMLString::equals(typeStr, OFTRealListStr))
					{
						newAttributes[i]->type = OFTRealList;
					}
					else if(XMLString::equals(typeStr, OFTStringStr))
					{
						newAttributes[i]->type = OFTString;
					}
					else if(XMLString::equals(typeStr, OFTStringListStr))
					{
						newAttributes[i]->type = OFTStringList;
					}
					else if(XMLString::equals(typeStr, OFTWideStringStr))
					{
						newAttributes[i]->type = OFTWideString;
					}
					else if(XMLString::equals(typeStr, OFTWideStringListStr))
					{
						newAttributes[i]->type = OFTWideStringList;
					}
					else if(XMLString::equals(typeStr, OFTBinaryStr))
					{
						newAttributes[i]->type = OFTBinary;
					}
					else if(XMLString::equals(typeStr, OFTDateStr))
					{
						newAttributes[i]->type = OFTDate;
					}
					else if(XMLString::equals(typeStr, OFTTimeStr))
					{
						newAttributes[i]->type = OFTTime;
					}
					else if(XMLString::equals(typeStr, OFTDateTimeStr))
					{
						newAttributes[i]->type = OFTDateTime;
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'type\' value was now recognised.");
					}
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'type\' attribute was provided.");
				}
				XMLString::release(&typeXMLStr);
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No attributes \'rsgis:attribute\' tags were provided.");
		}

		XMLString::release(&OFTIntegerStr);
		XMLString::release(&OFTIntegerListStr);
		XMLString::release(&OFTRealStr);
		XMLString::release(&OFTRealListStr);
		XMLString::release(&OFTStringStr);
		XMLString::release(&OFTStringListStr);
		XMLString::release(&OFTWideStringStr);
		XMLString::release(&OFTWideStringListStr);
		XMLString::release(&OFTBinaryStr);
		XMLString::release(&OFTDateStr);
		XMLString::release(&OFTTimeStr);
		XMLString::release(&OFTDateTimeStr);

	}
	else if(XMLString::equals(optionCopyPolygons, optionXML))
	{
		this->option = RSGISExeVectorUtils::copypolygons;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionPrintFIDs, optionXML))
	{
		this->option = RSGISExeVectorUtils::printfids;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}
	}
	else if(XMLString::equals(optionCountPxlsArea, optionXML))
	{
		this->option = RSGISExeVectorUtils::countpxlsarea;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *rasterXMLStr = XMLString::transcode("raster");
		if(argElement->hasAttribute(rasterXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rasterXMLStr));
			this->inputRasterVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'raster\' attribute was provided.");
		}
		XMLString::release(&rasterXMLStr);

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

	}
	else if(XMLString::equals(optionPoints2ASCII, optionXML))
	{
		this->option = RSGISExeVectorUtils::points2ascii;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
	}
	else if(XMLString::equals(optionRotate, optionXML))
	{
		this->option = RSGISExeVectorUtils::rotate;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *angleXMLStr = XMLString::transcode("angle");
		if(argElement->hasAttribute(angleXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(angleXMLStr));
			this->angle = mathUtils.degreesToRadians(mathUtils.strtofloat(string(charValue)));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'angle\' attribute was provided.");
		}
		XMLString::release(&angleXMLStr);


		DOMNodeList *pointsNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:point"));
		int numPoints = pointsNodesList->getLength();


		cout << "Found " << numPoints << " point" << endl;

		if(numPoints == 1)
		{
			this->fixedPt = new Coordinate(0,0,0);
			DOMElement *pointElement = static_cast<DOMElement*>(pointsNodesList->item(0));

			XMLCh *xXMLStr = XMLString::transcode("x");
			if(pointElement->hasAttribute(xXMLStr))
			{
				char *charValue = XMLString::transcode(pointElement->getAttribute(xXMLStr));
				this->fixedPt->x = mathUtils.strtodouble(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'x\' attribute was provided.");
			}
			XMLString::release(&xXMLStr);

			XMLCh *yXMLStr = XMLString::transcode("y");
			if(pointElement->hasAttribute(yXMLStr))
			{
				char *charValue = XMLString::transcode(pointElement->getAttribute(yXMLStr));
				this->fixedPt->y = mathUtils.strtodouble(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'y\' attribute was provided.");
			}
			XMLString::release(&yXMLStr);

			XMLCh *zXMLStr = XMLString::transcode("z");
			if(pointElement->hasAttribute(zXMLStr))
			{
				char *charValue = XMLString::transcode(pointElement->getAttribute(zXMLStr));
				this->fixedPt->z = mathUtils.strtodouble(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'z\' attribute was provided.");
			}
			XMLString::release(&zXMLStr);

		}

	}
	else if(XMLString::equals(optionArea, optionXML))
	{
		this->option = RSGISExeVectorUtils::calcarea;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionSplitFeatures, optionXML))
	{
		this->option = RSGISExeVectorUtils::splitfeatures;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *dirXMLStr = XMLString::transcode("dir");
		if(argElement->hasAttribute(dirXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(dirXMLStr));
			this->output_DIR = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'dir\' attribute was provided.");
		}
		XMLString::release(&dirXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionRemovePolyOverlaps, optionXML))
	{
		this->option = RSGISExeVectorUtils::removepolyoverlaps;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *snapXMLStr = XMLString::transcode("snap2grid");
		if(argElement->hasAttribute(snapXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *snapValue = argElement->getAttribute(snapXMLStr);

			if(XMLString::equals(snapValue, yesStr))
			{
				this->snap2Grid = true;
			}
			else
			{
				this->snap2Grid = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'snap2grid\' attribute was provided.");
		}
		XMLString::release(&snapXMLStr);

		XMLCh *toleranceXMLStr = XMLString::transcode("tolerance");
		if(argElement->hasAttribute(toleranceXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(toleranceXMLStr));
			this->tolerance = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'tolerance\' attribute was provided.");
		}
		XMLString::release(&toleranceXMLStr);

		XMLCh *dissolveXMLStr = XMLString::transcode("dissolve");
		if(argElement->hasAttribute(dissolveXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(dissolveXMLStr));
			this->dissolve = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'dissolve\' attribute was provided.");
		}
		XMLString::release(&dissolveXMLStr);
	}
	else if(XMLString::equals(optionMergeSmallPolysNear, optionXML))
	{
		this->option = RSGISExeVectorUtils::mergesmallpolysnear;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *areaXMLStr = XMLString::transcode("area");
		if(argElement->hasAttribute(areaXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(areaXMLStr));
			this->area = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'area\' attribute was provided.");
		}
		XMLString::release(&areaXMLStr);

		XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
		if(argElement->hasAttribute(resolutionXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resolutionXMLStr));
			resolution = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided.");
		}
		XMLString::release(&resolutionXMLStr);
	}
	else if(XMLString::equals(optionMergeSmallPolysTouching, optionXML))
	{
		this->option = RSGISExeVectorUtils::mergesmallpolystouching;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *areaXMLStr = XMLString::transcode("area");
		if(argElement->hasAttribute(areaXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(areaXMLStr));
			this->area = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'area\' attribute was provided.");
		}
		XMLString::release(&areaXMLStr);


		XMLCh *relborderXMLStr = XMLString::transcode("relborder");
		if(argElement->hasAttribute(relborderXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(relborderXMLStr));
			this->relBorderThreshold = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'relborder\' attribute was provided.");
		}
		XMLString::release(&relborderXMLStr);
	}
	else if(XMLString::equals(optionMorphClosing, optionXML))
	{
		this->option = RSGISExeVectorUtils::morphologicalclosing;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *bufferXMLStr = XMLString::transcode("buffer");
		if(argElement->hasAttribute(bufferXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bufferXMLStr));
			this->buffer = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'buffer\' attribute was provided.");
		}
		XMLString::release(&bufferXMLStr);

		XMLCh *curveXMLStr = XMLString::transcode("curve");
		if(argElement->hasAttribute(curveXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(curveXMLStr));
			this->curvesegments = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'curve\' attribute was provided.");
		}
		XMLString::release(&curveXMLStr);
	}
	else if(XMLString::equals(optionMorphOpening, optionXML))
	{
		this->option = RSGISExeVectorUtils::morphologicalopening;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *bufferXMLStr = XMLString::transcode("buffer");
		if(argElement->hasAttribute(bufferXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bufferXMLStr));
			this->buffer = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'buffer\' attribute was provided.");
		}
		XMLString::release(&bufferXMLStr);

		XMLCh *curveXMLStr = XMLString::transcode("curve");
		if(argElement->hasAttribute(curveXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(curveXMLStr));
			this->curvesegments = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'curve\' attribute was provided.");
		}
		XMLString::release(&curveXMLStr);
	}
	else if(XMLString::equals(optionMorphDilation, optionXML))
	{
		this->option = RSGISExeVectorUtils::morphologicaldilation;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *bufferXMLStr = XMLString::transcode("buffer");
		if(argElement->hasAttribute(bufferXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bufferXMLStr));
			this->buffer = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'buffer\' attribute was provided.");
		}
		XMLString::release(&bufferXMLStr);

		XMLCh *curveXMLStr = XMLString::transcode("curve");
		if(argElement->hasAttribute(curveXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(curveXMLStr));
			this->curvesegments = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'curve\' attribute was provided.");
		}
		XMLString::release(&curveXMLStr);
	}
	else if(XMLString::equals(optionMorphErosion, optionXML))
	{
		this->option = RSGISExeVectorUtils::morphologicalerosion;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *bufferXMLStr = XMLString::transcode("buffer");
		if(argElement->hasAttribute(bufferXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bufferXMLStr));
			this->buffer = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'buffer\' attribute was provided.");
		}
		XMLString::release(&bufferXMLStr);

		XMLCh *curveXMLStr = XMLString::transcode("curve");
		if(argElement->hasAttribute(curveXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(curveXMLStr));
			this->curvesegments = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'curve\' attribute was provided.");
		}
		XMLString::release(&curveXMLStr);
	}
	else if(XMLString::equals(optionNonConvexLineProj, optionXML))
	{
		this->option = RSGISExeVectorUtils::nonconvexoutlinelineproj;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
		if(argElement->hasAttribute(resolutionXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resolutionXMLStr));
			this->resolution = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided.");
		}
		XMLString::release(&resolutionXMLStr);
	}
	else if(XMLString::equals(optionNonConvexDelaunay, optionXML))
	{
		this->option = RSGISExeVectorUtils::nonconvexoutlinedelaunay;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionNonConvexSnakes, optionXML))
	{
		this->option = RSGISExeVectorUtils::nonconvexoutlinesnakes;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionConvexOutline, optionXML))
	{
		this->option = RSGISExeVectorUtils::convexoutline;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionMergetouchingpolys, optionXML))
	{
		this->option = RSGISExeVectorUtils::mergetouchingpoly;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionFreqDist, optionXML))
	{
		this->option = RSGISExeVectorUtils::freqdist;

		if(singlevector)
		{
			inputVectors = new list<string>();
			inputVectors->push_back(inputVector);
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputPlot = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *attributeXMLStr = XMLString::transcode("attribute");
		if(argElement->hasAttribute(attributeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(attributeXMLStr));
			this->attribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'attribute\' attribute was provided.");
		}
		XMLString::release(&attributeXMLStr);

	}
	else if(XMLString::equals(optionRasterise, optionXML) | XMLString::equals(optionRasterize, optionXML) ) // Check for British or American spelling
	{
		this->option = RSGISExeVectorUtils::rasterise;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);


		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
			this->useExistingRaster = true;
		}
		else
		{
			this->useExistingRaster = false;
		}
		XMLString::release(&imageXMLStr);

		XMLCh *attributeXMLStr = XMLString::transcode("attribute");
		if(argElement->hasAttribute(attributeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(attributeXMLStr));
			this->attribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'attribute\' attribute was provided.");
		}
		XMLString::release(&attributeXMLStr);

		// Retrieve method for calculating pixels in polygon
		XMLCh *methodXMLStr = XMLString::transcode("method");
		if(argElement->hasAttribute(methodXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(methodXMLStr));
			string methodStr = string(charValue);
			// Polygon completely contains pixel
			if(methodStr == "polyContainsPixel"){this->method = polyContainsPixel;}
			// Pixel center is within the polygon
			else if(methodStr == "polyContainsPixelCenter") {this->method = polyContainsPixelCenter;}
			// Polygon overlaps the pixel
			else if(methodStr == "polyOverlapsPixel"){this->method = polyOverlapsPixel;}
			// Polygon overlaps or contains the pixel
			else if(methodStr == "polyOverlapsOrContainsPixel"){this->method = polyOverlapsOrContainsPixel;}
			// Pixel contains the polygon
			else if(methodStr == "pixelContainsPoly"){this->method = pixelContainsPoly;}
			// Polygon center is within pixel
			else if(methodStr == "pixelContainsPolyCenter"){this->method = pixelContainsPolyCenter;}
			// The method is chosen based on relative areas of pixel and polygon.
			else if(methodStr == "adaptive"){this->method = adaptive;}
			// Everything within the polygons envelope is chosen (for debugging)
			else if(methodStr == "envelope"){this->method = envelope;}
			// Set to default value if not recognised.
			else {cout << "Method not recognised, using default of \'polyContainsPixelCenter\'." << endl;this->method = polyContainsPixelCenter;}
			XMLString::release(&charValue);
		}
		else
		{
			cout << "No method was provided, using default of \'polyContainsPixelCenter\'." << endl;
			this->method = polyContainsPixelCenter;
		}
		XMLString::release(&methodXMLStr);

		XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
		if(argElement->hasAttribute(resolutionXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resolutionXMLStr));
			this->resolution = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided.");
		}
		XMLString::release(&resolutionXMLStr);

		XMLCh *constValXMLStr = XMLString::transcode("constval");
		if(argElement->hasAttribute(constValXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(constValXMLStr));
			this->constVal = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			cout << "No \'constval\' attribute was provided, using default of 0" << endl;
			this->constVal = 0;
		}
		XMLString::release(&constValXMLStr);

	}
    else if(XMLString::equals(optionBurnRasterise, optionXML) | XMLString::equals(optionBurnRasterize, optionXML) ) // Check for British or American spelling
	{
		this->option = RSGISExeVectorUtils::burnrasterise;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);

		XMLCh *attributeXMLStr = XMLString::transcode("attribute");
		if(argElement->hasAttribute(attributeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(attributeXMLStr));
			this->attribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'attribute\' attribute was provided.");
		}
		XMLString::release(&attributeXMLStr);

		// Retrieve method for calculating pixels in polygon
		XMLCh *methodXMLStr = XMLString::transcode("method");
		if(argElement->hasAttribute(methodXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(methodXMLStr));
			string methodStr = string(charValue);
			// Polygon completely contains pixel
			if(methodStr == "polyContainsPixel"){this->method = polyContainsPixel;}
			// Pixel center is within the polygon
			else if(methodStr == "polyContainsPixelCenter") {this->method = polyContainsPixelCenter;}
			// Polygon overlaps the pixel
			else if(methodStr == "polyOverlapsPixel"){this->method = polyOverlapsPixel;}
			// Polygon overlaps or contains the pixel
			else if(methodStr == "polyOverlapsOrContainsPixel"){this->method = polyOverlapsOrContainsPixel;}
			// Pixel contains the polygon
			else if(methodStr == "pixelContainsPoly"){this->method = pixelContainsPoly;}
			// Polygon center is within pixel
			else if(methodStr == "pixelContainsPolyCenter"){this->method = pixelContainsPolyCenter;}
			// The method is chosen based on relative areas of pixel and polygon.
			else if(methodStr == "adaptive"){this->method = adaptive;}
			// Everything within the polygons envelope is chosen (for debugging)
			else if(methodStr == "envelope"){this->method = envelope;}
			// Set to default value if not recognised.
			else {cout << "Method not recognised, using default of \'polyContainsPixelCenter\'." << endl;this->method = polyContainsPixelCenter;}
			XMLString::release(&charValue);
		}
		else
		{
			cout << "No method was provided, using default of \'polyContainsPixelCenter\'." << endl;
			this->method = polyContainsPixelCenter;
		}
		XMLString::release(&methodXMLStr);
	}
	else if(XMLString::equals(optionPolygonPlots, optionXML))
	{
		this->option = RSGISExeVectorUtils::polygonplots;

		if(!inputFileNotVector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single CSV file input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionPolygonImageFootprints, optionXML))
	{
		this->option = RSGISExeVectorUtils::polygonImageFootprints;

		if(!inputFileNotVector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single CSV file input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionPolygoniseMPolys, optionXML))
	{
		this->option = RSGISExeVectorUtils::polygonizempolys;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *polygonizertagXMLStr = XMLString::transcode("rsgis:polygonizer");
		XMLCh *polygonizerLineProj = XMLString::transcode("lineproj");
		XMLCh *polygonizerDelaunay1 = XMLString::transcode("delaunay1");
		XMLCh *polygonizerConvexHull = XMLString::transcode("convexhull");
		XMLCh *polygonizerSnakes = XMLString::transcode("snakes");

		DOMNodeList *polgonizerNodesList = argElement->getElementsByTagName(polygonizertagXMLStr);
		if(polgonizerNodesList->getLength() == 1)
		{
			DOMElement *polygonizerElement = static_cast<DOMElement*>(polgonizerNodesList->item(0));
			if(polygonizerElement->hasAttribute(algorXMLStr))
			{
				const XMLCh *polygonizerAlgor = polygonizerElement->getAttribute(algorXMLStr);

				if(XMLString::equals(polygonizerLineProj, polygonizerAlgor))
				{
					polygonizertype = lineproj;

					XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
					if(polygonizerElement->hasAttribute(resolutionXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(resolutionXMLStr));
						this->resolution = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided for polygonizer.");
					}
					XMLString::release(&resolutionXMLStr);

				}
				else if(XMLString::equals(polygonizerDelaunay1, polygonizerAlgor))
				{
					polygonizertype = delaunay1;
				}
				else if(XMLString::equals(polygonizerConvexHull, polygonizerAlgor))
				{
					polygonizertype = convexhull;
				}
				else if(XMLString::equals(polygonizerSnakes, polygonizerAlgor))
				{
					polygonizertype = snakes;

					XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
					if(polygonizerElement->hasAttribute(resolutionXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(resolutionXMLStr));
						this->resolution = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided for polygonizer.");
					}
					XMLString::release(&resolutionXMLStr);

					XMLCh *alphaXMLStr = XMLString::transcode("alpha");
					if(polygonizerElement->hasAttribute(alphaXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(alphaXMLStr));
						this->alpha = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'alpha\' attribute was provided for polygonizer.");
					}
					XMLString::release(&alphaXMLStr);

					XMLCh *betaXMLStr = XMLString::transcode("beta");
					if(polygonizerElement->hasAttribute(betaXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(betaXMLStr));
						this->beta = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'beta\' attribute was provided for polygonizer.");
					}
					XMLString::release(&betaXMLStr);

					XMLCh *gammaXMLStr = XMLString::transcode("gamma");
					if(polygonizerElement->hasAttribute(gammaXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(gammaXMLStr));
						this->gamma = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'gamma\' attribute was provided for polygonizer.");
					}
					XMLString::release(&gammaXMLStr);


					XMLCh *deltaXMLStr = XMLString::transcode("delta");
					if(polygonizerElement->hasAttribute(deltaXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(deltaXMLStr));
						this->delta = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'delta\' attribute was provided for polygonizer.");
					}
					XMLString::release(&deltaXMLStr);


					XMLCh *maxNumIterationsXMLStr = XMLString::transcode("maxNumIterations");
					if(polygonizerElement->hasAttribute(maxNumIterationsXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(maxNumIterationsXMLStr));
						this->maxNumIterations = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'maxNumIterations\' attribute was provided for polygonizer.");
					}
					XMLString::release(&maxNumIterationsXMLStr);
				}
				else
				{
					throw RSGISXMLArgumentsException("Polygonizer algorithm was not recognised");
				}

			}
		    else
			{
				throw RSGISXMLArgumentsException("No \'algor\' attribute was provided for polygonizer.");
			}

		}
		else if(polgonizerNodesList->getLength() == 0)
		{
			throw RSGISXMLArgumentsException("A rsgis:polygonizer tag needs to be provided.");
		}
		else
		{
			throw RSGISXMLArgumentsException("Only one rsgis:polygonizer tag should be provided.");
		}

		XMLString::release(&polygonizertagXMLStr);
		XMLString::release(&polygonizerLineProj);
		XMLString::release(&polygonizerDelaunay1);
		XMLString::release(&polygonizerConvexHull);
		XMLString::release(&polygonizerSnakes);
	}
	else if(XMLString::equals(optionScatter2D, optionXML))
	{
		this->option = RSGISExeVectorUtils::scatter2D;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputPlot = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);


		XMLCh *attribute1XMLStr = XMLString::transcode("attribute1");
		if(argElement->hasAttribute(attribute1XMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(attribute1XMLStr));
			this->attribute1 = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'attribute1\' attribute was provided.");
		}
		XMLString::release(&attribute1XMLStr);

		XMLCh *attribute2XMLStr = XMLString::transcode("attribute2");
		if(argElement->hasAttribute(attribute2XMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(attribute2XMLStr));
			this->attribute2 = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'attribute2\' attribute was provided.");
		}
		XMLString::release(&attribute2XMLStr);

	}
	else if(XMLString::equals(optionCopyCheckPolys, optionXML))
	{
		this->option = RSGISExeVectorUtils::copycheckpolys;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionSplitByAttribute, optionXML))
	{
		this->option = RSGISExeVectorUtils::splitbyattribute;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVectorBase = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *attributeXMLStr = XMLString::transcode("attribute");
		if(argElement->hasAttribute(attributeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(attributeXMLStr));
			this->attribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'attribute\' attribute was provided.");
		}
		XMLString::release(&attributeXMLStr);

	}
	else if(XMLString::equals(optionRemoveContainedPolygons, optionXML))
	{
		this->option = RSGISExeVectorUtils::removecontainedpolygons;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionRemoveContainedPolygonsDIR, optionXML))
	{
		this->option = RSGISExeVectorUtils::removecontainedpolygonsDIR;

		if(!inputFileNotVector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires a single input parameter specifying the input directory.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		this->inputDIR = this->inputFile;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->output_DIR = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionPolygonsInPolygon, optionXML))
	{
		this->option = RSGISExeVectorUtils::polygonsInPolygon;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		// Get attribute to use shapefile
		XMLCh *attributeNameXMLStr = XMLString::transcode("attributeName");
		if(argElement->hasAttribute(attributeNameXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(attributeNameXMLStr));
			this->attributeName = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			cout << "No attribute name provided for cover shapefile, using default of FID" << endl;
		}
		XMLString::release(&attributeNameXMLStr);

		// Get covering shapefile
		XMLCh *inputCoverVectorXMLStr = XMLString::transcode("coverVector");
		if(argElement->hasAttribute(inputCoverVectorXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inputCoverVectorXMLStr));
			this->inputCoverVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \"inputCoverVector\" attribute provided");
		}
		XMLString::release(&inputCoverVectorXMLStr);

		// Get output direcrory
		XMLCh *dirXMLStr = XMLString::transcode("dir");
		if(argElement->hasAttribute(dirXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(dirXMLStr));
			this->output_DIR = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'dir\' attribute was provided.");
		}
		XMLString::release(&dirXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionMergetouchingpolysDIR, optionXML))
	{
		this->option = RSGISExeVectorUtils::mergetouchingpolyDIR;

		if(!inputFileNotVector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires a single input parameter specifying the input directory.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		this->inputDIR = this->inputFile;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->output_DIR = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *attributeXMLStr = XMLString::transcode("attribute");
		if(argElement->hasAttribute(attributeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(attributeXMLStr));
			this->attribute = string(charValue);
			XMLString::release(&charValue);
			attributeDef = true;
		}
		else
		{
			attributeDef = false;
		}
		XMLString::release(&attributeXMLStr);


	}
	else if(XMLString::equals(optionSplitlargesmall, optionXML))
	{
		this->option = RSGISExeVectorUtils::splitlargesmall;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputLargeXMLStr = XMLString::transcode("outputlarge");
		if(argElement->hasAttribute(outputLargeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputLargeXMLStr));
			this->outputlarge = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'outputlarge\' attribute was provided.");
		}
		XMLString::release(&outputLargeXMLStr);

		XMLCh *outputSmallXMLStr = XMLString::transcode("outputsmall");
		if(argElement->hasAttribute(outputSmallXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputSmallXMLStr));
			this->outputsmall = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'outputsmall\' attribute was provided.");
		}
		XMLString::release(&outputSmallXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			this->threshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
		}
		XMLString::release(&thresholdXMLStr);

	}
	else if(XMLString::equals(optionRemovepolygonscontainedwithingeom, optionXML))
	{
		this->option = RSGISExeVectorUtils::removepolygonscontainedwithingeom;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *geometryXMLStr = XMLString::transcode("geometry");
		if(argElement->hasAttribute(geometryXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(geometryXMLStr));
			this->inputGeometry = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'geometry\' attribute was provided.");
		}
		XMLString::release(&geometryXMLStr);

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionMergeSHPs, optionXML))
	{
		this->option = RSGISExeVectorUtils::mergeshps;

		if(singlevector)
		{
			inputVectors = new list<string>();
			inputVectors->push_back(inputVector);
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);


		XMLCh *ignoreAttrXMLStr = XMLString::transcode("ignoreattributes");
		if(argElement->hasAttribute(ignoreAttrXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *strValue = argElement->getAttribute(ignoreAttrXMLStr);

			if(XMLString::equals(strValue, yesStr))
			{
				this->ignoreAttr = true;
			}
			else
			{
				this->ignoreAttr = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'ignoreattributes\' attribute was provided.");
		}
		XMLString::release(&ignoreAttrXMLStr);
	}
	else if(XMLString::equals(optionFixPolyExtBoundary, optionXML))
	{
		this->option = RSGISExeVectorUtils::fixpolyextboundary;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionRemoveHoles, optionXML))
	{
		this->option = RSGISExeVectorUtils::removepolyholes;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

        XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			this->threshold = mathUtils.strtofloat(string(charValue));
            this->areaThresholdProvided = true;
			XMLString::release(&charValue);
		}
		else
		{
			this->areaThresholdProvided = false;
		}
		XMLString::release(&thresholdXMLStr);
	}
	else if(XMLString::equals(optionDropSmallPolys, optionXML))
	{
		this->option = RSGISExeVectorUtils::dropsmallpolys;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
		if(argElement->hasAttribute(thresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(thresholdXMLStr));
			this->threshold = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
		}
		XMLString::release(&thresholdXMLStr);

	}
	else if(XMLString::equals(optionExtractLargestPoly, optionXML))
	{
		this->option = RSGISExeVectorUtils::extractlargestpoly;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
	}
	else if(XMLString::equals(optionGenerateSinglePoly, optionXML))
	{
		this->option = RSGISExeVectorUtils::generatesinglepoly;

		if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *polygonizertagXMLStr = XMLString::transcode("rsgis:polygonizer");
		XMLCh *polygonizerLineProj = XMLString::transcode("lineproj");
		XMLCh *polygonizerDelaunay1 = XMLString::transcode("delaunay1");
		XMLCh *polygonizerConvexHull = XMLString::transcode("convexhull");
		XMLCh *polygonizerSnakes = XMLString::transcode("snakes");

		DOMNodeList *polgonizerNodesList = argElement->getElementsByTagName(polygonizertagXMLStr);
		if(polgonizerNodesList->getLength() == 1)
		{
			DOMElement *polygonizerElement = static_cast<DOMElement*>(polgonizerNodesList->item(0));
			if(polygonizerElement->hasAttribute(algorXMLStr))
			{
				const XMLCh *polygonizerAlgor = polygonizerElement->getAttribute(algorXMLStr);

				if(XMLString::equals(polygonizerLineProj, polygonizerAlgor))
				{
					polygonizertype = lineproj;

					XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
					if(polygonizerElement->hasAttribute(resolutionXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(resolutionXMLStr));
						this->resolution = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided for polygonizer.");
					}
					XMLString::release(&resolutionXMLStr);

				}
				else if(XMLString::equals(polygonizerDelaunay1, polygonizerAlgor))
				{
					polygonizertype = delaunay1;
				}
				else if(XMLString::equals(polygonizerConvexHull, polygonizerAlgor))
				{
					polygonizertype = convexhull;
				}
				else if(XMLString::equals(polygonizerSnakes, polygonizerAlgor))
				{
					polygonizertype = snakes;

					XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
					if(polygonizerElement->hasAttribute(resolutionXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(resolutionXMLStr));
						this->resolution = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided for polygonizer.");
					}
					XMLString::release(&resolutionXMLStr);

					XMLCh *alphaXMLStr = XMLString::transcode("alpha");
					if(polygonizerElement->hasAttribute(alphaXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(alphaXMLStr));
						this->alpha = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'alpha\' attribute was provided for polygonizer.");
					}
					XMLString::release(&alphaXMLStr);

					XMLCh *betaXMLStr = XMLString::transcode("beta");
					if(polygonizerElement->hasAttribute(betaXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(betaXMLStr));
						this->beta = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'beta\' attribute was provided for polygonizer.");
					}
					XMLString::release(&betaXMLStr);

					XMLCh *gammaXMLStr = XMLString::transcode("gamma");
					if(polygonizerElement->hasAttribute(gammaXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(gammaXMLStr));
						this->gamma = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'gamma\' attribute was provided for polygonizer.");
					}
					XMLString::release(&gammaXMLStr);


					XMLCh *deltaXMLStr = XMLString::transcode("delta");
					if(polygonizerElement->hasAttribute(deltaXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(deltaXMLStr));
						this->delta = mathUtils.strtofloat(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'delta\' attribute was provided for polygonizer.");
					}
					XMLString::release(&deltaXMLStr);


					XMLCh *maxNumIterationsXMLStr = XMLString::transcode("maxNumIterations");
					if(polygonizerElement->hasAttribute(maxNumIterationsXMLStr))
					{
						char *charValue = XMLString::transcode(polygonizerElement->getAttribute(maxNumIterationsXMLStr));
						this->maxNumIterations = mathUtils.strtoint(string(charValue));
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'maxNumIterations\' attribute was provided for polygonizer.");
					}
					XMLString::release(&maxNumIterationsXMLStr);
				}
				else
				{
					throw RSGISXMLArgumentsException("Polygonizer algorithm was not recognised");
				}

			}
		    else
			{
				throw RSGISXMLArgumentsException("No \'algor\' attribute was provided for polygonizer.");
			}

		}
		else if(polgonizerNodesList->getLength() == 0)
		{
			throw RSGISXMLArgumentsException("A rsgis:polygonizer tag needs to be provided.");
		}
		else
		{
			throw RSGISXMLArgumentsException("Only one rsgis:polygonizer tag should be provided.");
		}

		XMLString::release(&polygonizertagXMLStr);
		XMLString::release(&polygonizerLineProj);
		XMLString::release(&polygonizerDelaunay1);
		XMLString::release(&polygonizerConvexHull);
		XMLString::release(&polygonizerSnakes);
	}
	else if(XMLString::equals(optionGenerateGrid, optionXML))
	{
		this->option = RSGISExeVectorUtils::generategrid;

		if(!noInputProvide)
		{
			cerr << "An input file has been provided but this will be ignored\n";
		}

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *proj4XMLStr = XMLString::transcode("proj4");
		if(argElement->hasAttribute(proj4XMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(proj4XMLStr));
			this->proj4 = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj4\' attribute was provided.");
		}
		XMLString::release(&proj4XMLStr);



		XMLCh *tlxXMLStr = XMLString::transcode("tlx");
		if(argElement->hasAttribute(tlxXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(tlxXMLStr));
			this->tlx = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'tlx\' attribute was provided.");
		}
		XMLString::release(&tlxXMLStr);


		XMLCh *tlyXMLStr = XMLString::transcode("tly");
		if(argElement->hasAttribute(tlyXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(tlyXMLStr));
			this->tly = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'tly\' attribute was provided.");
		}
		XMLString::release(&tlyXMLStr);


		XMLCh *xresXMLStr = XMLString::transcode("xres");
		if(argElement->hasAttribute(xresXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(xresXMLStr));
			this->xres = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
			if(this->xres <= 0)
			{
				throw RSGISXMLArgumentsException("Resolution can't be 0 or negative");
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'xres\' attribute was provided.");
		}
		XMLString::release(&xresXMLStr);


		XMLCh *yresXMLStr = XMLString::transcode("yres");
		if(argElement->hasAttribute(yresXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(yresXMLStr));
			this->yres = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
			if(this->yres <= 0)
			{
				throw RSGISXMLArgumentsException("Resolution can't be 0 or negative");
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'yres\' attribute was provided.");
		}
		XMLString::release(&yresXMLStr);


		XMLCh *widthXMLStr = XMLString::transcode("width");
		if(argElement->hasAttribute(widthXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(widthXMLStr));
			this->width = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'width\' attribute was provided.");
		}
		XMLString::release(&widthXMLStr);

		XMLCh *heightXMLStr = XMLString::transcode("height");
		if(argElement->hasAttribute(heightXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(heightXMLStr));
			this->height = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'height\' attribute was provided.");
		}
		XMLString::release(&heightXMLStr);

	}
	else if(XMLString::equals(optionGenerateImageGrid, optionXML))
	{
		this->option = RSGISExeVectorUtils::generateimagegrid;

		if(!inputFileNotVector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single image file input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}


		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *xresXMLStr = XMLString::transcode("xres");
		if(argElement->hasAttribute(xresXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(xresXMLStr));
			this->xres = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'xres\' attribute was provided.");
		}
		XMLString::release(&xresXMLStr);

		XMLCh *yresXMLStr = XMLString::transcode("yres");
		if(argElement->hasAttribute(yresXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(yresXMLStr));
			this->yres = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'yres\' attribute was provided.");
		}
		XMLString::release(&yresXMLStr);
	}
	else if(XMLString::equals(optionVectorMaths, optionXML))
	{
		this->option = RSGISExeVectorUtils::vectormaths;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *expressionXMLStr = XMLString::transcode("expression");
		XMLCh *expressionFileXMLStr = XMLString::transcode("expressionFile");
		if(argElement->hasAttribute(expressionXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(expressionXMLStr));
            string muExpression = string(charValue);
            replace_all(muExpression, "&lt;", "<");
            replace_all(muExpression, "&gt;", ">");
            replace_all(muExpression, "&ge;", ">=");
            replace_all(muExpression, "&le;", "<=");

            replace_all(muExpression, "lt", "<");
            replace_all(muExpression, "gt", ">");
            replace_all(muExpression, "ge", ">=");
            replace_all(muExpression, "le", "<=");

			this->mathsExpression = muExpression;
			XMLString::release(&charValue);
		}
		else if(argElement->hasAttribute(expressionFileXMLStr)) // Get expression from text file
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(expressionFileXMLStr));
			string inputExprFileName = string(charValue);
            string muExpression = "";
			ifstream inputExprFile;
			inputExprFile.open(inputExprFileName.c_str());
			if(!inputExprFile.is_open())
			{
				throw RSGISXMLArgumentsException("Could not open input expression file.");
			}
			string strLine;
			while(!inputExprFile.eof())
			{
				getline(inputExprFile, strLine, '\n');
				muExpression += strLine;
			}
			// Shouldn't need to replace XML specific expressions but try anyway.
            replace_all(muExpression, "&lt;", "<");
            replace_all(muExpression, "&gt;", ">");
            replace_all(muExpression, "&ge;", ">=");
            replace_all(muExpression, "&le;", "<=");

            replace_all(muExpression, "lt", "<");
            replace_all(muExpression, "gt", ">");
            replace_all(muExpression, "ge", ">=");
            replace_all(muExpression, "le", "<=");

			// Replace tabs
			replace_all(muExpression, "\t", "");
			replace_all(muExpression, "\n", "");
			replace_all(muExpression, " ", "");

			this->mathsExpression = muExpression;
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'expression\' attribute was provided.");
		}
		XMLString::release(&expressionXMLStr);
		XMLString::release(&expressionFileXMLStr);

		XMLCh *outHeadingXMLStr = XMLString::transcode("outHeading");
		if(argElement->hasAttribute(outHeadingXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outHeadingXMLStr));
			this->outHeading = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'outHeading\' attribute was provided.");
		}
		XMLString::release(&outHeadingXMLStr);


		DOMNodeList *varNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:variable"));
		this->numVars = varNodesList->getLength();

		cout << "Found " << this->numVars << " Variables \n";

		DOMElement *varElement = NULL;
		variables = new VariableFields*[numVars];

		for(int i = 0; i < numVars; i++)
		{
			varElement = static_cast<DOMElement*>(varNodesList->item(i));

			variables[i] = new VariableFields();

			XMLCh *varNameXMLStr = XMLString::transcode("name");
			if(varElement->hasAttribute(varNameXMLStr))
			{
				char *charValue = XMLString::transcode(varElement->getAttribute(varNameXMLStr));
				this->variables[i]->name = string(charValue);
                if((this-> variables[i]->name == "lt") || (this-> variables[i]->name == "gt") || (this-> variables[i]->name == "le") || (this-> variables[i]->name == "ge"))
                {
                    throw RSGISXMLArgumentsException("Can't use \'lt\', \'gt\',\'ge\' or \'le\' for variable names");
                }
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			XMLString::release(&varNameXMLStr);

			XMLCh *varFieldNameXMLStr = XMLString::transcode("fieldname");
			if(varElement->hasAttribute(varFieldNameXMLStr))
			{
				char *charValue = XMLString::transcode(varElement->getAttribute(varFieldNameXMLStr));
				this->variables[i]->fieldName = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'fieldname\' attribute was provided.");
			}
			XMLString::release(&varFieldNameXMLStr);
		}

	}
    else if(XMLString::equals(optionVectorSelect, optionXML))
    {
        this->option = RSGISExeVectorUtils::select;

        if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

        XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);

		XMLCh *expressionXMLStr = XMLString::transcode("expression");
		if(argElement->hasAttribute(expressionXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(expressionXMLStr));
            this->sqlExpression = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'expression\' attribute was provided.");
		}
		XMLString::release(&expressionXMLStr);
    }
    else if(XMLString::equals(optionCalcMeanMinDist, optionXML))
    {
        this->option = RSGISExeVectorUtils::calcmeanmindist;

        if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

        XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputTextFile = string(charValue);
			XMLString::release(&charValue);
            outTextFile = true;
		}
		else
		{
            outTextFile = false;
			cerr << "Output will just be printed to the console\n";
		}
		XMLString::release(&outputXMLStr);
    }
    else if (XMLString::equals(optionCopyAssignProj, optionXML))
    {
        this->option = RSGISExeVectorUtils::copyassignproj;

        if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

        XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);


        XMLCh *projWKTXMLStr = XMLString::transcode("projwkt");
		if(argElement->hasAttribute(projWKTXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(projWKTXMLStr));
			this->projFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'projwkt\' attribute was provided.");
		}
		XMLString::release(&projWKTXMLStr);

        XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
    }
    else if (XMLString::equals(optionPrintWKT, optionXML))
    {
        this->option = RSGISExeVectorUtils::printwkt;

        if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}
    }
    else if (XMLString::equals(optionAddFIDCol, optionXML))
    {
        this->option = RSGISExeVectorUtils::addfidcol;

        if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

        XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

        XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);


        XMLCh *initXMLStr = XMLString::transcode("init");
		if(argElement->hasAttribute(initXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(initXMLStr));
			this->initFID = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			this->initFID = 0;
		}
		XMLString::release(&initXMLStr);
    }
    else if (XMLString::equals(optionMinDist2Polys, optionXML))
    {
        this->option = RSGISExeVectorUtils::mindist2polys;

        if(!singlevector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single vector input.");
		}

		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}

        XMLCh *polysXMLStr = XMLString::transcode("polygons");
		if(argElement->hasAttribute(polysXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polysXMLStr));
			this->inputGeometry = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'polygons\' attribute was provided.");
		}
		XMLString::release(&polysXMLStr);

        XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

        XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);



    }
    else if (XMLString::equals(optionConvexHullGrps, optionXML))
    {
        this->option = RSGISExeVectorUtils::convexhullgrps;
        
        if(!inputFileNotVector)
		{
			throw RSGISXMLArgumentsException("This algorithm requires only a single input CSV file to be provided.");
		}
        
		if(noInputProvide)
		{
			throw RSGISXMLArgumentsException("No input file has been provided.");
		}
        
        XMLCh *polysXMLStr = XMLString::transcode("polygons");
		if(argElement->hasAttribute(polysXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(polysXMLStr));
			this->outputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'polygons\' attribute was provided.");
		}
		XMLString::release(&polysXMLStr);
        
        XMLCh *projWKTXMLStr = XMLString::transcode("projwkt");
		if(argElement->hasAttribute(projWKTXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(projWKTXMLStr));
			this->projFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'projwkt\' attribute was provided.");
		}
		XMLString::release(&projWKTXMLStr);
        
        XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);
            
			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
        
        XMLCh *eastingsColXMLStr = XMLString::transcode("eastingscol");
		if(argElement->hasAttribute(eastingsColXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(eastingsColXMLStr));
			this->eastingsColIdx = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'eastingscol\' attribute was provided.");
		}
		XMLString::release(&eastingsColXMLStr);
        
        XMLCh *northingsColXMLStr = XMLString::transcode("northingscol");
		if(argElement->hasAttribute(northingsColXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(northingsColXMLStr));
			this->northingsColIdx = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'northingscol\' attribute was provided.");
		}
		XMLString::release(&northingsColXMLStr);
        
        XMLCh *attributeColXMLStr = XMLString::transcode("attributecol");
		if(argElement->hasAttribute(attributeColXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(attributeColXMLStr));
			this->attributeColIdx = mathUtils.strtounsignedint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'attributecol\' attribute was provided.");
		}
		XMLString::release(&attributeColXMLStr);
        
    }
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeVectorUtils.");
		throw RSGISXMLArgumentsException(message.c_str());
	}

	parsed = true;

	XMLString::release(&algorName);
	XMLString::release(&optionRemoveAttributes);
	XMLString::release(&optionBufferVector);
	XMLString::release(&optiongeom2circles);
	XMLString::release(&optionPrintPolygonGeoms);
	XMLString::release(&optionFindReplaceText);
	XMLString::release(&optionListAttributes);
	XMLString::release(&optionPrintAttribute);
	XMLString::release(&optionTopAttributes);
	XMLString::release(&optionAddAttributes);
	XMLString::release(&optionCopyPolygons);
	XMLString::release(&optionPrintFIDs);
	XMLString::release(&optionCountPxlsArea);
	XMLString::release(&algorXMLStr);
	XMLString::release(&optionXMLStr);
	XMLString::release(&rsgisattributeXMLStr);
	XMLString::release(&optionPoints2ASCII);
	XMLString::release(&optionRotate);
	XMLString::release(&optionArea);
	XMLString::release(&optionSplitFeatures);
	XMLString::release(&optionRemovePolyOverlaps);
	XMLString::release(&optionMergeSmallPolysNear);
	XMLString::release(&optionMergeSmallPolysTouching);
	XMLString::release(&optionMorphClosing);
	XMLString::release(&optionMorphOpening);
	XMLString::release(&optionMorphErosion);
	XMLString::release(&optionMorphDilation);
	XMLString::release(&optionNonConvexDelaunay);
	XMLString::release(&optionNonConvexLineProj);
	XMLString::release(&optionMergetouchingpolys);
	XMLString::release(&optionNonConvexSnakes);
	XMLString::release(&optionFreqDist);
	XMLString::release(&optionRasterise);
	XMLString::release(&optionRasterize);
    XMLString::release(&optionBurnRasterise);
	XMLString::release(&optionBurnRasterize);
	XMLString::release(&optionConvexOutline);
	XMLString::release(&optionPolygonPlots);
	XMLString::release(&optionPolygoniseMPolys);
	XMLString::release(&optionScatter2D);
	XMLString::release(&optionCopyCheckPolys);
	XMLString::release(&optionSplitByAttribute);
	XMLString::release(&optionRemoveContainedPolygons);
	XMLString::release(&optionRemoveContainedPolygonsDIR);
	XMLString::release(&optionPolygonsInPolygon);
	XMLString::release(&optionMergetouchingpolysDIR);
	XMLString::release(&optionSplitlargesmall);
	XMLString::release(&optionRemovepolygonscontainedwithingeom);
	XMLString::release(&optionMergeSHPs);
	XMLString::release(&optionFixPolyExtBoundary);
	XMLString::release(&optionRemoveHoles);
	XMLString::release(&optionDropSmallPolys);
	XMLString::release(&optionExtractLargestPoly);
	XMLString::release(&optionGenerateSinglePoly);
	XMLString::release(&optionGenerateGrid);
	XMLString::release(&optionGenerateImageGrid);
	XMLString::release(&optionVectorMaths);
    XMLString::release(&optionVectorSelect);
    XMLString::release(&optionCalcMeanMinDist);
    XMLString::release(&optionCopyAssignProj);
    XMLString::release(&optionPrintWKT);
    XMLString::release(&optionAddFIDCol);
    XMLString::release(&optionMinDist2Polys);
    XMLString::release(&optionConvexHullGrps);
}

void RSGISExeVectorUtils::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		cout.precision(12);
		if(option == RSGISExeVectorUtils::removeAttributes)
		{
			cout << "Copy geometry and remove attributes\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
            try
            {
                rsgis::cmds::executeRemoveAttributes(this->inputVector, this->outputVector, this->force);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw RSGISException(e.what());
            }
		}
		else if(option == RSGISExeVectorUtils::bufferVector)
		{
			cout << "Buffer Vector data geometry\n";
			cout << "Input Vector: " << this->inputVector << endl;
			
            cout << "Output Vector: " << this->outputVector << endl;
            if(bufferValueInText)
            {
                RSGISTextUtils textUtils;
                string fileContents = textUtils.readFileToString(bufferValueFile);
                this->buffer = textUtils.strtofloat(fileContents);
            }
            cout << "Buffer: " << this->buffer << endl;
            
            try
            {
                cmds::executeBufferVector(this->inputVector, this->outputVector, this->buffer, this->force);
            }
			catch (RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::geom2circles)
		{
			cout << "Converting Geometries to Circles\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Area Attribute: " << this->areaAttribute << endl;
			cout << "Radius Attribute: " << this->radiusAttribute << endl;
			cout << "Resolution: " << this->resolution << endl;
			cout << "Radius Value: " << this->radius << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				//if(fileUtils.checkFilePresent(this->outputVector))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

				processFeature = new RSGISGeometryToCircle(this->resolution, this->areaAttribute, this->radiusAttribute, this->radius);
				processVector = new RSGISProcessVector(processFeature);

				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, false, false);

				GDALClose(inputSHPDS);
				GDALClose(outputSHPDS);

				delete processVector;
				delete processFeature;

				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}

		}
		else if(option == RSGISExeVectorUtils::printpolygeom)
		{
			cout << "Print Polygon Geometry\n";
			cout << "Input Vector: " << this->inputVector << endl;

            try
            {
                rsgis::cmds::executePrintPolyGeom(this->inputVector);
            }
			catch (RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::findreplacetext)
		{
			cout << "Find and replace text attribute\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Attribute: " << this->attribute << endl;
			cout << "Find: " << this->find << endl;
			cout << "Replace: " << this->replace << endl;

            try
            {
                rsgis::cmds::executeFindReplaceText(this->inputVector, this->attribute , this->find, this->replace);
            }
            catch (RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::listattributes)
		{
			cout << "List attributes\n";
			cout << "Input Vector: " << this->inputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			OGRRegisterAll();

			RSGISVectorUtils vecUtils;
			RSGISVectorProcessing vecProcessing;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}

				vecProcessing.listAttributes(inputSHPLayer);

				GDALClose(inputSHPDS);

				//OGRCleanupAll();
			}
			catch (RSGISException e)
			{
				throw e;
			}

		}
		else if(option == RSGISExeVectorUtils::printattribute)
		{
			cout << "Print attribute for all features\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Attribute: " << this->attribute << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			OGRRegisterAll();

			RSGISVectorUtils vecUtils;
			RSGISVectorProcessing vecProcessing;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(this->inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}

				vecProcessing.printAttribute(inputSHPLayer, this->attribute);

				GDALClose(inputSHPDS);

				//OGRCleanupAll();
			}
			catch (RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::topattributes)
		{
			cout << "Calculate the top attributes\n";

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

				processFeature = new RSGISIdentifyTopLayerAttributes(attributes, numAttributes, numTop, summary);
				processVector = new RSGISProcessVector(processFeature);

				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, false, true);

				GDALClose(inputSHPDS);
				GDALClose(outputSHPDS);

				delete[] attributes;

				delete processVector;
				delete processFeature;

				//OGRCleanupAll();
			}
			catch (RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::addattributes)
		{
			cout << "Add attributes to a Vector Layer\n";

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();


			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

				processFeature = new RSGISAddAttributes(newAttributes, numAttributes);
				processVector = new RSGISProcessVector(processFeature);

				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, false, false);

				GDALClose(inputSHPDS);
				GDALClose(outputSHPDS);
				delete processVector;
				delete processFeature;

				delete[] newAttributes;

				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}

		}
		else if(option == RSGISExeVectorUtils::copypolygons)
		{
			cout << "Copy Polygon Vector Layer\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();


			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayer->GetFeature(0);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

				processFeature = new RSGISProcessFeatureCopyVector();
				processVector = new RSGISProcessVector(processFeature);

				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, false, false);

				GDALClose(inputSHPDS);
				GDALClose(outputSHPDS);

				delete processVector;
				delete processFeature;

				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::printfids)
		{
			cout << "Print FIDs\n";
			cout << "Input Vector: " << this->inputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			OGRRegisterAll();

			RSGISVectorUtils vecUtils;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}

				processFeature = new RSGISPrintFIDs();
				processVector = new RSGISProcessVector(processFeature);

				processVector->processVectorsNoOutput(inputSHPLayer, true);

				GDALClose(inputSHPDS);

				delete processVector;
				delete processFeature;
				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::countpxlsarea)
		{
			cout << "Calculate the number of pixels within a polygon and the polygon area\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Raster Polygons: " << this->inputRasterVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();


			OGRRegisterAll();
			GDALAllRegister();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputPolysImage = NULL;
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

				///////////////////////
				//
				// Open raster polygons
				//
				////////////////////////
				inputPolysImage = (GDALDataset *) GDALOpenShared(inputRasterVector.c_str(), GA_ReadOnly);
				if(inputPolysImage == NULL)
				{
					string message = string("Could not open image ") + inputRasterVector;
					throw RSGISException(message.c_str());
				}

				processFeature = new RSGISPixelAreaCountInPolygon(inputPolysImage);
				processVector = new RSGISProcessVector(processFeature);

				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);

				GDALClose(inputSHPDS);
				GDALClose(outputSHPDS);
				GDALClose(inputPolysImage);

				delete processVector;
				delete processFeature;

				//OGRCleanupAll();
				GDALDestroyDriverManager();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::points2ascii)
		{
			cout << "Convert points shapefile to a comma separated text file.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();


			OGRRegisterAll();
			GDALAllRegister();

			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}

				processFeature = new RSGISConvertToASCII(this->outputVector);
				processVector = new RSGISProcessVector(processFeature);

				processVector->processVectorsNoOutput(inputSHPLayer, false);

				GDALClose(inputSHPDS);

				delete processVector;
				delete processFeature;

				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				cout << "ERROR: " << e.what() << endl;;
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::rotate)
		{
			cout << "Rotate geometry around a fixed point\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Point: " << this->fixedPt->toString() << endl;
			cout << "Angle: " << this->angle << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();


			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;
			OGRFeatureDefn *inFeatureDefn = NULL;

			RSGISProcessGeometry *processVector = NULL;
			RSGISProcessOGRGeometry *processGeom = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();
				inFeatureDefn = inputSHPLayer->GetLayerDefn();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

				processGeom = new RSGISGeometryRotateAroundFixedPoint(fixedPt, angle);
				processVector = new RSGISProcessGeometry(processGeom);

				processVector->processGeometry(inputSHPLayer, outputSHPLayer, true, false);

				GDALClose(inputSHPDS);
				GDALClose(outputSHPDS);

				delete processVector;
				delete processGeom;

				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::calcarea)
		{
			cout << "Calculating Polygon Area\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            try
            {
                rsgis::cmds::executeCalcPolyArea(this->inputVector, this->outputVector, this->force);
            }
			catch (RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::splitfeatures)
		{
			cout << "Split features into separate shapefiles\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output DIR: " << this->output_DIR << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();


			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorProcessing vecProcessing;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}

				vecProcessing.splitFeatures(inputSHPLayer, this->output_DIR, this->force);

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::removepolyoverlaps)
		{
			cout << "Remove overlapping polygons in shapefile..\n";

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			RSGISGeometry geomUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;

				vector<Polygon*>::iterator iterPolys;
				Envelope *env = new Envelope();
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); iterPolys++)
				{
					Envelope *tmpEnv = geomUtils.getEnvelope(*iterPolys);
					env->expandToInclude(tmpEnv);
					delete tmpEnv;
				}

				if(snap2Grid)
				{
					cout << "Snapping to grid\n";
					vector<Polygon*> *snappedPolygons = new vector<Polygon*>();

					for(iterPolys = polygons->begin(); iterPolys != polygons->end(); iterPolys++)
					{
						snappedPolygons->push_back(geomUtils.snapToXYGrid(*iterPolys, tolerance, false, env));
					}

					cout << "Identifying and solving overlaps\n";
					geomUtils.removeOverlaps(snappedPolygons, env, tolerance, dissolve);
					cout << "Outputting Shapefile\n";
					vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, snappedPolygons);

					vector<Polygon*>::iterator iterPolys;
					for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
					{
						delete *iterPolys;
						polygons->erase(iterPolys);
					}
					delete polygons;

					for(iterPolys = snappedPolygons->begin(); iterPolys != snappedPolygons->end(); )
					{
						delete *iterPolys;
						snappedPolygons->erase(iterPolys);
					}
					delete snappedPolygons;
				}
				else
				{
					cout << "Identifying and solving overlaps\n";
					geomUtils.removeOverlaps(polygons, env, tolerance, dissolve);
					cout << "Outputting Shapefile\n";
					vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, polygons, spatialRef);

					vector<Polygon*>::iterator iterPolys;
					for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
					{
						delete *iterPolys;
						polygons->erase(iterPolys);
					}
					delete polygons;
				}

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::mergesmallpolysnear)
		{
			cout << "Merge small polygons to the nearest large polygon..\n";

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			RSGISGeometry geomUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;

				cout << "Identifying small polygons\n";
				vector<Polygon*> *smallPolygons = new vector<Polygon*>();
				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					if((*iterPolys)->getArea() < this->area)
					{
						smallPolygons->push_back(*iterPolys);
						polygons->erase(iterPolys);
					}
					else
					{
						iterPolys++;
					}
				}

				cout << "Merge small polygons to nearest large polygon\n";
				RSGISIdentifyNonConvexPolygons *identifyNonConvexPolygons = new RSGISIdentifyNonConvexPolygonsDelaunay();
				geomUtils.mergeWithNearest(polygons, smallPolygons, identifyNonConvexPolygons);
				delete identifyNonConvexPolygons;

				cout << "Outputting Shapefile\n";
				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, polygons, spatialRef);

				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::mergesmallpolystouching)
		{
			cout << "Merge small polygons with neighboring polygons with a shared bordered..\n";

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			RSGISGeometry geomUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;

				cout << "Identifying small polygons\n";
				vector<Polygon*> *smallPolygons = new vector<Polygon*>();
				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					if((*iterPolys)->getArea() < this->area)
					{
						smallPolygons->push_back(*iterPolys);
						polygons->erase(iterPolys);
					}
					else
					{
						++iterPolys;
					}
				}

				RSGISIdentifyNonConvexPolygons *identifyNonConvexPolygons = new RSGISIdentifyNonConvexPolygonsDelaunay();
				cout << "Merge small polygons to touching neighboring large polygons\n";
				geomUtils.mergeWithNeighbor(polygons, smallPolygons, relBorderThreshold, identifyNonConvexPolygons);
				delete identifyNonConvexPolygons;

				for(iterPolys = smallPolygons->begin(); iterPolys != smallPolygons->end(); )
				{
					polygons->push_back(*iterPolys);
					smallPolygons->erase(iterPolys);
				}
				delete smallPolygons;

				cout << "Outputting Shapefile\n";
				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, polygons, spatialRef);

				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				delete identifyNonConvexPolygons;

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::morphologicalclosing)
		{
			cout << "Perform a morphological closing\n";

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			RSGISGeometry geomUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message);
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message);
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				cout << "created objects\n";

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;


				cout << "Performing Morphological Operation\n";
				geomUtils.performMorphologicalOperation(polygons, closing, buffer, curvesegments);

				cout << "Outputting Shapefile\n";
				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, polygons, spatialRef);

				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch (RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::morphologicalopening)
		{
			cout << "Perform a morphological opening\n";

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			RSGISGeometry geomUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;


				cout << "Performing Morphological Operation\n";
				geomUtils.performMorphologicalOperation(polygons, opening, buffer, curvesegments);

				cout << "Outputting Shapefile\n";
				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, polygons, spatialRef);

				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::morphologicaldilation)
		{
			cout << "Perform a morphological dilation\n";

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			RSGISGeometry geomUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);


				delete processVector;
				delete processFeature;


				cout << "Performing Morphological Operation\n";
				geomUtils.performMorphologicalOperation(polygons, dilation, buffer, curvesegments);

				cout << "Outputting Shapefile\n";
				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, polygons, spatialRef);
				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::morphologicalerosion)
		{
			cout << "Perform a morphological erosion\n";

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			RSGISGeometry geomUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				GDALClose(inputSHPDS);
				delete processVector;
				delete processFeature;


				cout << "Performing Morphological Operation\n";
				geomUtils.performMorphologicalOperation(polygons, erosion, buffer, curvesegments);

				cout << "Outputting Shapefile\n";
				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, polygons, spatialRef);
				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::nonconvexoutlinesnakes)
		{
			cout << "Identify the non-convex outline of the input vector\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;

				cout.precision(10);

				resolution = 1;
				double alpha = 0.01;
				double beta = 0.01;
				double gamma = 1;
				double delta = 1;
				int maxNumIterations = 5;

				RSGISIdentifyNonConvexPolygons *calcNewPoly = new RSGISIdentifyNonConvexPolygonsSnakes(resolution, spatialRef, alpha, beta, gamma, delta, maxNumIterations);

				vector<Polygon*> *newPolys = new vector<Polygon*>();
				newPolys->push_back(calcNewPoly->retrievePolygon(polygons));

				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, newPolys, spatialRef);

				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = newPolys->begin(); iterPolys != newPolys->end(); )
				{
					delete *iterPolys;
					newPolys->erase(iterPolys);
				}
				delete newPolys;

				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				delete (RSGISIdentifyNonConvexPolygonsSnakes*) calcNewPoly;

				delete RSGISGEOSFactoryGenerator::getInstance();

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::convexoutline)
		{
			cout << "Identify the non-convex outline of the input vector\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;

				RSGISGeometry geomUtils;

				vector<Polygon*> *newPolys = new vector<Polygon*>();
				newPolys->push_back(geomUtils.findConvexHull(polygons));

				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, newPolys, spatialRef);

				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = newPolys->begin(); iterPolys != newPolys->end(); )
				{
					delete *iterPolys;
					newPolys->erase(iterPolys);
				}
				delete newPolys;

				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				delete RSGISGEOSFactoryGenerator::getInstance();

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::nonconvexoutlinedelaunay)
		{
			cout << "Identify the non-convex outline of the input vector\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;

				RSGISIdentifyNonConvexPolygons *calcNewPoly = new RSGISIdentifyNonConvexPolygonsDelaunay();

				vector<Polygon*> *newPolys = new vector<Polygon*>();
				newPolys->push_back(calcNewPoly->retrievePolygon(polygons));

				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, newPolys, spatialRef);
				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				for(iterPolys = newPolys->begin(); iterPolys != newPolys->end(); )
				{
					delete *iterPolys;
					newPolys->erase(iterPolys);
				}
				delete newPolys;

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::nonconvexoutlinelineproj)
		{
			cout << "Identify the non-convex outline of the input vector\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Resolution: " << this->resolution << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;

				RSGISIdentifyNonConvexPolygons *calcNewPoly = new RSGISIdentifyNonConvexPolygonsLineProject(resolution);

				vector<Polygon*> *newPolys = new vector<Polygon*>();
				newPolys->push_back(calcNewPoly->retrievePolygon(polygons));

				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, newPolys, spatialRef);
				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				for(iterPolys = newPolys->begin(); iterPolys != newPolys->end(); )
				{
					delete *iterPolys;
					newPolys->erase(iterPolys);
				}
				delete newPolys;

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::mergetouchingpoly)
		{
			cout << "Merge touching polygons\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;


				RSGISGeometry geomUtils;
				//geomUtils.mergeTouchingPolygons(polygons);
				geomUtils.mergeTouchingPolygonsWithIndex(polygons);

				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, polygons, spatialRef);

				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::freqdist)
		{
			cout << "Produce a frequency distribution for the attribute in the input shapefile.\n";

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			try
			{
				OGRRegisterAll();

				RSGISFileUtils fileUtils;
				RSGISVectorUtils vecUtils;
				RSGISVectorIO vecIO;

				string SHPFileInLayer = "";

				GDALDataset *inputSHPDS = NULL;
				OGRLayer *inputSHPLayer = NULL;

				RSGISProcessVector *processVector = NULL;
				RSGISProcessOGRFeature *processFeature = NULL;

				list<double> *values = new list<double>();
				processFeature = new RSGISGetAttributeValues(values, this->attribute);
				processVector = new RSGISProcessVector(processFeature);

				if(inputVectors->size() > 0)
				{
					list<string>::iterator iterFiles;
					for(iterFiles = inputVectors->begin(); iterFiles != inputVectors->end(); ++iterFiles)
					{
						inputVector =  *iterFiles;

						/////////////////////////////////////
						//
						// Open Input Shapfile.
						//
						/////////////////////////////////////
						SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

						inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
						if(inputSHPDS == NULL)
						{
							string message = string("Could not open vector file ") + this->inputVector;
							throw RSGISFileException(message.c_str());
						}
						inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
						if(inputSHPLayer == NULL)
						{
							string message = string("Could not open vector layer ") + SHPFileInLayer;
							throw RSGISFileException(message.c_str());
						}

						// Get data from attribute..
						processVector->processVectorsNoOutput(inputSHPLayer, false);

						GDALClose(inputSHPDS);
					}

					string filename = fileUtils.getFileNameNoExtension(outputPlot);
					cout << "filename: " << filename << endl;

					RSGISExportForPlotting::getInstance()->export2DFreq(filename, values);
				}
				else
				{
					throw RSGISException("There were no input files.");
				}

				values->clear();
				delete values;

				delete processVector;
				delete processFeature;

				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				throw e;
			}

		}
		else if(option == RSGISExeVectorUtils::rasterise)
		{
			cout << "Rasterise a vector\n";
			cout << "Input Vector = " << this->inputVector << endl;
			cout << "Output Image = " << this->outputImage << endl;
			cout << "Attribute = " << this->attribute << endl;
			cout << "Resolution = " << this->resolution << endl;
			cout << "Const Value = " << this->constVal << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			try
			{
				OGRRegisterAll();
				GDALAllRegister();

				RSGISVectorUtils vecUtils;
				RSGISRasterizeVector rasteriseVec;

				GDALDataset *inputSHPDS = NULL;
				OGRLayer *inputSHPLayer = NULL;

				GDALDriver *gdalDriver = NULL;
				GDALDataset *imageData = NULL;

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}

				if (this->useExistingRaster)
				{
					RSGISImageUtils imgUtils;
					// OPEN INPUT IMAGE
					GDALDataset *inputImageDS = NULL;
					GDALDataset **dataset = NULL;
					inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
					if(inputImageDS == NULL)
					{
						string message = string("Could not open image ") + this->inputImage;
						throw RSGISException(message.c_str());
					}

					// CREATE OUTPUT IMAGE
					GDALDriver *gdalDriver = NULL;
					double *gdalTranslation = new double[6];
					int **dsOffsets = new int*[1];
					dsOffsets[0] = new int[2];
					int height = 0;
					int width = 0;

					dataset = new GDALDataset*[1];
					dataset[0] = inputImageDS;

					imgUtils.getImageOverlap(dataset, 1, dsOffsets, &width, &height, gdalTranslation);
					gdalDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
					if(gdalDriver == NULL)
					{
						throw RSGISImageBandException("ENVI driver does not exists..");
					}
					cout << "New image width = " << width << " height = " << height << " bands = " << 1 << endl;

					imageData = imgUtils.createBlankImage(this->outputImage.c_str(), gdalTranslation, width, height, 1, inputImageDS->GetProjectionRef(), this->constVal);

					delete[] gdalTranslation;
					delete[] dataset;
					GDALClose(inputImageDS);
				}
				else
				{
					gdalDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
					if(gdalDriver == NULL)
					{
						throw RSGISImageException("ENVI driver does not exists..");
					}
					cout << "Creating dataset..." << endl;
					imageData = rasteriseVec.createDataset(gdalDriver, inputSHPLayer, this->outputImage, this->resolution, this->constVal);
					cout << "Dataset created ..." << endl;
				}

				rasteriseVec.rasterizeLayer(inputSHPLayer, imageData, this->attribute, this->method);

				GDALClose(imageData);
				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				throw e;
			}
		}
        else if(option == RSGISExeVectorUtils::burnrasterise)
		{
			cout << "Rasterise a vector\n";
			cout << "Input Vector = " << this->inputVector << endl;
			cout << "Input Image = " << this->inputImage << endl;
			cout << "Attribute = " << this->attribute << endl;

			// Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			try
			{
				OGRRegisterAll();
				GDALAllRegister();

				RSGISVectorUtils vecUtils;
				RSGISRasterizeVector rasteriseVec;

				GDALDataset *inputSHPDS = NULL;
				OGRLayer *inputSHPLayer = NULL;

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}

                GDALDataset *imgDataset = NULL;
                imgDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                if(imgDataset == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISException(message.c_str());
                }

				rasteriseVec.rasterizeLayer(inputSHPLayer, imgDataset, this->attribute, this->method);

				GDALClose(imgDataset);
				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::polygonplots)
		{
			cout << "Create plot polygons from input CSV file\n";
			cout << "Input CSV = " << this->inputFile << endl;
			cout << "Output Vector = " << this->outputVector << endl;

			// Convert to absolute path
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			try
			{
				RSGISPlotPolygonsCSVParse *parse = new RSGISPlotPolygonsCSVParse();
				vector<PlotPoly*> *polyDetails = parse->parsePolyPlots(this->inputFile);
				delete parse;

				RSGISVectorProcessing *vecProcessing = new RSGISVectorProcessing();
				vecProcessing->createPlotPolygons(polyDetails, this->outputVector, this->force);
				delete vecProcessing;

				vector<PlotPoly*>::iterator iterPolyDetails;
				for(iterPolyDetails = polyDetails->begin(); iterPolyDetails != polyDetails->end(); )
				{
					delete *iterPolyDetails;
					polyDetails->erase(iterPolyDetails);
				}
				delete polyDetails;
			}
			catch (RSGISException &e)
			{
				throw e;
			}

		}
		else if(option == RSGISExeVectorUtils::polygonImageFootprints)
		{
			cout << "Create plot polygons from input CSV file\n";
			cout << "Input CSV = " << this->inputFile << endl;
			cout << "Output Vector = " << this->outputVector << endl;

            // Convert to absolute path
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			try
			{
				RSGISImageFootprintPolygonsCSVParse *parse = new RSGISImageFootprintPolygonsCSVParse();
				vector<ImageFootPrintPoly*> *polyDetails = parse->parsePoly(this->inputFile);
				delete parse;

				RSGISVectorProcessing *vecProcessing = new RSGISVectorProcessing();
				vecProcessing->createImageFootprintPolygons(polyDetails, this->outputVector, this->force);
				delete vecProcessing;

				vector<ImageFootPrintPoly*>::iterator iterPolyDetails;
				for(iterPolyDetails = polyDetails->begin(); iterPolyDetails != polyDetails->end(); )
				{
					delete *iterPolyDetails;
					polyDetails->erase(iterPolyDetails);
				}
				delete polyDetails;
			}
			catch (RSGISException &e)
			{
				throw e;
			}

		}
		else if(option == RSGISExeVectorUtils::polygonizempolys)
		{
			cout << "Create a polygon around each Multi-Polygon.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			if(polygonizertype == lineproj)
			{
				cout << "The line projection polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
			}
			else if(polygonizertype == delaunay1)
			{
				cout << "The delaunay1 polygonizer will be used\n";
			}
			else if(polygonizertype == delaunay2)
			{
				cout << "The delaunay2 polygonizer will be used\n";
			}
			else if(polygonizertype == convexhull)
			{
				cout << "The convex hull polygonizer will be used\n";
			}
			else if(polygonizertype == multipoly)
			{
				cout << "The multi-polygon polygonizer will be used\n";
			}
			else if(polygonizertype == snakes)
			{
				cout << "The snakes polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
				cout << "Alpha = " << this->alpha << endl;
				cout << "Beta = " << this->beta << endl;
				cout << "Gamma = " << this->gamma << endl;
				cout << "Delta = " << this->delta << endl;
			}

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<MultiPolygon*> *mPolys = new vector<MultiPolygon*>();
				processFeature = new RSGISGEOSMultiPolygonReader(mPolys);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;

				RSGISGeometry geomUtils;

				vector<Polygon*> *polygons = new vector<Polygon*>();
				vector<Polygon*> *tmpPolys = new vector<Polygon*>();
				vector<Polygon*>::iterator iterPolys;
				vector<MultiPolygon*>::iterator iterMultiPolys;
				for(iterMultiPolys = mPolys->begin(); iterMultiPolys != mPolys->end(); ++iterMultiPolys)
				{
					geomUtils.retrievePolygons((*iterMultiPolys), tmpPolys);

					if(polygonizertype == lineproj)
					{
						RSGISIdentifyNonConvexPolygonsLineProject *identifyNonConvexLineProj = new RSGISIdentifyNonConvexPolygonsLineProject(resolution);
						polygons->push_back(identifyNonConvexLineProj->retrievePolygon(tmpPolys));
						delete identifyNonConvexLineProj;
					}
					else if(polygonizertype == delaunay1)
					{
						RSGISIdentifyNonConvexPolygonsDelaunay *identifyNonConvexDelaunay = new RSGISIdentifyNonConvexPolygonsDelaunay();
						polygons->push_back(identifyNonConvexDelaunay->retrievePolygon(tmpPolys));
						delete identifyNonConvexDelaunay;
					}
					else if(polygonizertype == convexhull)
					{
						polygons->push_back(geomUtils.findConvexHull(tmpPolys));
					}
					else if(polygonizertype == snakes)
					{
						RSGISIdentifyNonConvexPolygonsSnakes *identifyNonConvexSnakes = new RSGISIdentifyNonConvexPolygonsSnakes(resolution, spatialRef, alpha, beta, gamma, delta, maxNumIterations);
						polygons->push_back(identifyNonConvexSnakes->retrievePolygon(tmpPolys));
						delete identifyNonConvexSnakes;
					}
					else
					{
						throw RSGISException("Do not know polygonization option.");
					}


					for(iterPolys = tmpPolys->begin(); iterPolys != tmpPolys->end(); )
					{
						delete *iterPolys;
						tmpPolys->erase(iterPolys);
					}

				}
				delete tmpPolys;

				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, polygons, spatialRef);

				for(iterMultiPolys = mPolys->begin(); iterMultiPolys != mPolys->end(); )
				{
					delete *iterMultiPolys;
					mPolys->erase(iterMultiPolys);
				}
				delete mPolys;

				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch(RSGISException e)
			{
				throw e;
			}

		}
		else if(option == RSGISExeVectorUtils::scatter2D)
		{
			cout << "Create scatter2d plot file from two attributes\n";
			cout << "Input Vector = " << this->inputVector << endl;
			cout << "Output Plot = " << this->outputPlot << endl;
			cout << "Attribute1 = " << this->attribute1 << endl;
			cout << "Attribute2 = " << this->attribute2 << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}

				RSGISExportForPlottingIncremental *plotter = new RSGISExportForPlottingIncremental();
				plotter->openFile(this->outputPlot, scatter2d);
				processFeature = new RSGIS2DScatterPlotVariables(plotter, attribute1, attribute2);
				processVector = new RSGISProcessVector(processFeature);

				processVector->processVectorsNoOutput(inputSHPLayer, false);

				plotter->close();
				delete plotter;

				delete processVector;
				delete processFeature;


				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch(RSGISException e)
			{
				throw e;
			}

		}
		else if(option == RSGISExeVectorUtils::copycheckpolys)
		{
			cout << "Copy and check Polygon Vector Layer\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayer->GetFeature(0);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

				RSGISCopyCheckPolygons copyCheckPolygons;
				try
				{
					copyCheckPolygons.copyCheckPolygons(inputSHPLayer, outputSHPLayer, true);

					GDALClose(inputSHPDS);
					GDALClose(outputSHPDS);
					//OGRCleanupAll();
				}
				catch (RSGISVectorException &e)
				{
					GDALClose(inputSHPDS);
					GDALClose(outputSHPDS);
					//OGRCleanupAll();
					throw e;
				}
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::splitbyattribute)
		{
			cout << "Split input vector by attribute value\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector Base: " << this->outputVectorBase << endl;
			cout << "Attribute: " << this->attribute << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVectorBase = boost::filesystem::absolute(this->outputVectorBase).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessVectorSQL *processVectorSQL = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			vector<string> *attributeValues = new vector<string>();

			try
			{
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayer->GetFeature(0);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();

				processFeature = new RSGISCreateListOfAttributeValues(attributeValues, attribute);
				processVector = new RSGISProcessVector(processFeature);
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;

				cout << "There are " << attributeValues->size() << " independent values within the shapefile\n";

				string outputShpFilePath = "";
				string SHPFileOutLayer = "";
				string outputDIR = "";
				string sql = "";

				processFeature = new RSGISCopyFeatures();
				processVectorSQL = new RSGISProcessVectorSQL(processFeature);

				vector<string>::iterator iterAttr;
				for(iterAttr = attributeValues->begin(); iterAttr != attributeValues->end(); ++iterAttr)
				{
					outputShpFilePath = this->outputVectorBase + "_" + *iterAttr + ".shp";
					cout << outputShpFilePath << endl;
					SHPFileOutLayer = vecUtils.getLayerName(outputShpFilePath);
					cout << "SHPFileOutLayer = " << SHPFileOutLayer << endl;
					outputDIR = fileUtils.getFileDirectoryPath(outputShpFilePath);
					cout << "outputDIR = " << outputDIR << endl;
					if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
					{
						if(this->force)
						{
							vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
						}
						else
						{
							throw RSGISException("Shapefile already exists, either delete or select force.");
						}
					}

					/////////////////////////////////////
					//
					// Create Output Shapfile.
					//
					/////////////////////////////////////
					outputSHPDS = shpFiledriver->Create(outputShpFilePath.c_str(), 0, 0, 0, GDT_Unknown, NULL );
					if( outputSHPDS == NULL )
					{
						string message = string("Could not create vector file ") + outputShpFilePath;
						throw RSGISVectorOutputException(message.c_str());
					}
					outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
					if( outputSHPLayer == NULL )
					{
						string message = string("Could not create vector layer ") + SHPFileOutLayer;
						throw RSGISVectorOutputException(message.c_str());
					}

					sql = "SELECT * FROM " + SHPFileInLayer + " WHERE " + this->attribute + " = " + *iterAttr;

					cout << "SQL: " << sql << endl;

					processVectorSQL->processVectors(inputSHPDS, outputSHPLayer, true, false, sql);

					GDALClose(outputSHPDS);
				}
				GDALClose(inputSHPDS);

				delete processFeature;
				delete processVectorSQL;

				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::removecontainedpolygons)
		{
			cout << "Remove polygons which are contained within another polygon.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayer->GetFeature(0);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}


				try
				{
					RSGISRemoveContainedPolygons removeContainedPolys;
					unsigned long numOutPolys = removeContainedPolys.removeContainedPolygons(inputSHPLayer, outputSHPLayer);
					cout << numOutPolys << " Polygons have been outputted.\n";

					GDALClose(inputSHPDS);
					GDALClose(outputSHPDS);

				}
				catch (RSGISVectorException &e)
				{
					GDALClose(inputSHPDS);
					GDALClose(outputSHPDS);
					throw e;
				}

				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::polygonsInPolygon)
		{
			std::cout << "Split polgons in:" << std::endl;
			std::cout << "\t" << this->inputVector << std::endl;
			std::cout << "By polygons in:" << std::endl;
            std::cout << "\t" << this->inputCoverVector << std::endl;
			std::cout << "Output DIR = " <<  this->output_DIR << std::endl;

            try
            {
                rsgis::cmds::executePolygonsInPolygon(this->inputVector, this->inputCoverVector, this->output_DIR, this->attributeName, this->force);
            }
            catch(rsgis::RSGISException &e)
            {
                throw e;
            }
            catch(std::exception &e)
            {
                throw RSGISException(e.what());
            }
		}
		else if(option == RSGISExeVectorUtils::removecontainedpolygonsDIR)
		{
			cout << "Remove polygons which are contained within another polygon for files within a DIR.\n";
			cout << "Input DIR: " << this->inputDIR << endl;
			cout << "Output DIR: " << this->output_DIR << endl;

            // Convert to absolute path
            this->inputDIR = boost::filesystem::absolute(this->inputDIR).c_str();
            this->output_DIR = boost::filesystem::absolute(this->output_DIR).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = "";
			string SHPFileOutLayer = "";
			string outputDIR = "";

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}

			RSGISRemoveContainedPolygons removeContainedPolys;

			vector<string>::iterator iterFiles;
			vector<string> *inputShpFiles = new vector<string>();
			fileUtils.getDIRList(this->inputDIR, ".shp", inputShpFiles, true);

			if(inputShpFiles->size() == 0)
			{
				throw RSGISException("There were no shapefiles within the specified directory.");
			}
			else
			{
				for(iterFiles = inputShpFiles->begin(); iterFiles != inputShpFiles->end(); ++iterFiles)
				{
					try
					{

						cout << "Processing: " << *iterFiles << endl;
						this->inputVector = *iterFiles;
						SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
						//cout << "In layer name: " << SHPFileInLayer << endl;
						SHPFileOutLayer = SHPFileInLayer + "_nocontained";
						//cout << "Out layer name: " << SHPFileOutLayer << endl;

						this->outputVector = output_DIR + SHPFileOutLayer + ".shp";
						cout << "Output: " << this->outputVector << endl;

						if(vecUtils.checkDIR4SHP(output_DIR, SHPFileOutLayer))
						{
							if(this->force)
							{
								vecUtils.deleteSHP(output_DIR, SHPFileOutLayer);
							}
							else
							{
								throw RSGISException("Shapefile already exists, either delete or select force.");
							}
						}

						/////////////////////////////////////
						//
						// Open Input Shapfile.
						//
						/////////////////////////////////////
						inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
						if(inputSHPDS == NULL)
						{
							string message = string("Could not open vector file ") + this->inputVector;
							throw RSGISFileException(message.c_str());
						}
						inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
						if(inputSHPLayer == NULL)
						{
							string message = string("Could not open vector layer ") + SHPFileInLayer;
							throw RSGISFileException(message.c_str());
						}
						inputSpatialRef = inputSHPLayer->GetSpatialRef();

						// Get Geometry Type.
						OGRFeature *feature = inputSHPLayer->GetFeature(0);
						OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();

						/////////////////////////////////////
						//
						// Create Output Shapfile.
						//
						/////////////////////////////////////

						outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
						if( outputSHPDS == NULL )
						{
							string message = string("Could not create vector file ") + this->outputVector;
							throw RSGISVectorOutputException(message.c_str());
						}
						outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
						if( outputSHPLayer == NULL )
						{
							string message = string("Could not create vector layer ") + SHPFileOutLayer;
							throw RSGISVectorOutputException(message.c_str());
						}

						try
						{
							unsigned long numOutPolys = removeContainedPolys.removeContainedPolygons(inputSHPLayer, outputSHPLayer);
							cout << numOutPolys << " Polygons have been outputted.\n";

							GDALClose(inputSHPDS);
							GDALClose(outputSHPDS);

						}
						catch (RSGISVectorException &e)
						{
							GDALClose(inputSHPDS);
							GDALClose(outputSHPDS);
							throw e;
						}
					}
					catch (RSGISException& e)
					{
						throw e;
					}
				}

				delete inputShpFiles;

				//OGRCleanupAll();
			}
		}
		else if(option == RSGISExeVectorUtils::mergetouchingpolyDIR)
		{
			cout << "Merge touching polygons for a shapefile within a directory.\n";
			cout << "Input DIR: " << this->inputDIR << endl;
			cout << "Output DIR: " << this->output_DIR << endl;

            // Convert to absolute path
            this->inputDIR = boost::filesystem::absolute(this->inputDIR).c_str();
            this->output_DIR = boost::filesystem::absolute(this->output_DIR).c_str();

			if(attributeDef)
			{
				cout << "The attribute " << this->attribute << " will be added to the output.\n";
			}

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;

			string SHPFileInLayer = "";
			string SHPFileOutLayer = "";
			string outputDIR = "";

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			vector<string>::iterator iterFiles;
			vector<string> *inputShpFiles = new vector<string>();
			fileUtils.getDIRList(this->inputDIR, ".shp", inputShpFiles, true);

			string attributeValue = "";

			vector<Polygon*> *polygons = new vector<Polygon*>();

			if(inputShpFiles->size() == 0)
			{
				throw RSGISException("There were no shapefiles within the specified directory.");
			}
			else
			{
				for(iterFiles = inputShpFiles->begin(); iterFiles != inputShpFiles->end(); ++iterFiles)
				{
					try
					{

						cout << "Processing: " << *iterFiles << endl;
						this->inputVector = *iterFiles;
						SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
						//cout << "In layer name: " << SHPFileInLayer << endl;
						SHPFileOutLayer = SHPFileInLayer + "_notouch";
						//cout << "Out layer name: " << SHPFileOutLayer << endl;

						this->outputVector = output_DIR + SHPFileOutLayer + ".shp";
						cout << "Output: " << this->outputVector << endl;

						if(vecUtils.checkDIR4SHP(output_DIR, SHPFileOutLayer))
						{
							if(this->force)
							{
								vecUtils.deleteSHP(output_DIR, SHPFileOutLayer);
							}
							else
							{
								throw RSGISException("Shapefile already exists, either delete or select force.");
							}
						}

						/////////////////////////////////////
						//
						// Open Input Shapfile.
						//
						/////////////////////////////////////
						inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
						if(inputSHPDS == NULL)
						{
							string message = string("Could not open vector file ") + this->inputVector;
							throw RSGISFileException(message.c_str());
						}
						inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
						if(inputSHPLayer == NULL)
						{
							string message = string("Could not open vector layer ") + SHPFileInLayer;
							throw RSGISFileException(message.c_str());
						}
						inputSpatialRef = inputSHPLayer->GetSpatialRef();
						if(attributeDef)
						{
							OGRFeature *feature = inputSHPLayer->GetFeature(0);
							OGRFeatureDefn *featureDefn = feature->GetDefnRef();
							int fieldIdx = featureDefn->GetFieldIndex(attribute.c_str());
							if(fieldIdx < 0)
							{
								string message = "This layer does not contain a field with the name \'" + attribute + "\'";
								throw RSGISVectorException(message.c_str());
							}
							attributeValue = feature->GetFieldAsString(fieldIdx);
						}


						try
						{
							processFeature = new RSGISGEOSPolygonReader(polygons);
							processVector = new RSGISProcessVector(processFeature);

							cout << "Read input Shapefile\n";
							processVector->processVectorsNoOutput(inputSHPLayer, false);

							delete processVector;
							delete processFeature;


							RSGISGeometry geomUtils;
							//geomUtils.mergeTouchingPolygons(polygons);
                            geomUtils.mergeTouchingPolygonsWithIndex(polygons);

							if(attributeDef)
							{
								vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, polygons, inputSpatialRef, attribute, attributeValue);
							}
							else
							{
								vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, polygons, inputSpatialRef);
							}

							vector<Polygon*>::iterator iterPolys;
							for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
							{
								delete *iterPolys;
								polygons->erase(iterPolys);
							}


							GDALClose(inputSHPDS);

						}
						catch (RSGISVectorException &e)
						{
							GDALClose(inputSHPDS);
							throw e;
						}
					}
					catch (RSGISException& e)
					{
						throw e;
					}
				}
			}

			delete polygons;
			delete inputShpFiles;

			//OGRCleanupAll();
		}
		else if(option == RSGISExeVectorUtils::splitlargesmall)
		{
			cout << "Split small and large polygons into separate shapefiles.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Large Vector: " << this->outputlarge << endl;
			cout << "Output Small Vector: " << this->outputsmall << endl;
			cout << "Threshold = " << this->threshold << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputlarge = boost::filesystem::absolute(this->outputlarge).c_str();
            this->outputsmall = boost::filesystem::absolute(this->outputsmall).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayerLarge = vecUtils.getLayerName(this->outputlarge);
			string SHPFileOutLayerSmall = vecUtils.getLayerName(this->outputsmall);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDSLarge = NULL;
			OGRLayer *outputSHPLayerLarge = NULL;
			GDALDataset *outputSHPDSSmall = NULL;
			OGRLayer *outputSHPLayerSmall = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}

			string outputLargeDIR = "";
			string outputSmallDIR = "";
			try
			{
				outputLargeDIR = fileUtils.getFileDirectoryPath(this->outputlarge);
				outputSmallDIR = fileUtils.getFileDirectoryPath(this->outputsmall);

				if(vecUtils.checkDIR4SHP(outputLargeDIR, SHPFileOutLayerLarge))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputLargeDIR, SHPFileOutLayerLarge);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				if(vecUtils.checkDIR4SHP(outputSmallDIR, SHPFileOutLayerSmall))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputSmallDIR, SHPFileOutLayerSmall);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayer->GetFeature(0);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();

				/////////////////////////////////////
				//
				// Create Output Shapfile Large
				//
				/////////////////////////////////////
				outputSHPDSLarge = shpFiledriver->Create(this->outputlarge.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDSLarge == NULL )
				{
					string message = string("Could not create vector file ") + this->outputlarge;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayerLarge = outputSHPDSLarge->CreateLayer(SHPFileOutLayerLarge.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayerLarge == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayerLarge;
					throw RSGISVectorOutputException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Create Output Shapfile Small
				//
				/////////////////////////////////////
				outputSHPDSSmall = shpFiledriver->Create(this->outputsmall.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDSSmall == NULL )
				{
					string message = string("Could not create vector file ") + this->outputsmall;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayerSmall = outputSHPDSSmall->CreateLayer(SHPFileOutLayerSmall.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayerSmall == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayerSmall;
					throw RSGISVectorOutputException(message.c_str());
				}

				try
				{
					RSGISSplitSmallLargePolygons splitSmallLarge;
					splitSmallLarge.splitPolygons(inputSHPLayer, outputSHPLayerSmall, outputSHPLayerLarge, threshold);

					GDALClose(inputSHPDS);
					GDALClose(outputSHPDSLarge);
					GDALClose(outputSHPDSSmall);
					//OGRCleanupAll();
				}
				catch (RSGISVectorException &e)
				{
					GDALClose(inputSHPDS);
					GDALClose(outputSHPDSLarge);
					GDALClose(outputSHPDSSmall);
					//OGRCleanupAll();
					throw e;
				}
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::removepolygonscontainedwithingeom)
		{
			cout << "Remove Polygons contained with the inputted geometry.\n";
			cout << "Input Geometry: " << this->inputGeometry << endl;
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputGeometry = boost::filesystem::absolute(this->inputGeometry).c_str();
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileInLayerGeom = vecUtils.getLayerName(this->inputGeometry);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDataset *inputSHPDSGeom = NULL;
			OGRLayer *inputSHPLayerGeom = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile. (Geometry)
				//
				/////////////////////////////////////
				inputSHPDSGeom = (GDALDataset*) GDALOpenEx(this->inputGeometry.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDSGeom == NULL)
				{
					string message = string("Could not open vector file ") + this->inputGeometry;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayerGeom = inputSHPDSGeom->GetLayerByName(SHPFileInLayerGeom.c_str());
				if(inputSHPLayerGeom == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayerGeom;
					throw RSGISFileException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayer->GetFeature(0);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}


				try
				{
					vector<OGRPolygon*> *polys = new vector<OGRPolygon*>();
					processFeature = new RSGISOGRPolygonReader(polys);
					processVector = new RSGISProcessVector(processFeature);
					processVector->processVectorsNoOutput(inputSHPLayerGeom, false);

					delete processVector;
					delete processFeature;

					RSGISRemoveContainedPolygons removeContainedPolys;
					unsigned long numOutPolys = removeContainedPolys.removeContainedPolygons(inputSHPLayer, outputSHPLayer, polys);
					cout << numOutPolys << " Polygons have been outputted.\n";

					GDALClose(inputSHPDS);
					GDALClose(outputSHPDS);

				}
				catch (RSGISVectorException &e)
				{
					GDALClose(inputSHPDS);
					GDALClose(outputSHPDS);
					throw e;
				}

				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::mergeshps)
		{
			cout << "Merge the input shapefiles to a single shapefile.\n";
			cout << "Input Files: \n";
			list<string>::iterator iterFiles;
			for(iterFiles = inputVectors->begin(); iterFiles != inputVectors->end(); ++iterFiles)
			{
				cout << *iterFiles << endl;
			}
			cout << "Output Vector: " << this->outputVector << endl;
			if(ignoreAttr)
			{
				cout << "The attributes will be ignored\n";
			}
			else
			{
				cout << "The attributes will be outputed\n";
			}
			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = "";
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			RSGISAppendToVectorLayer appendToVector;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				bool first = true;

				for(iterFiles = inputVectors->begin(); iterFiles != inputVectors->end(); ++iterFiles)
				{
					inputVector = *iterFiles;

					/////////////////////////////////////
					//
					// Open Input Shapfile.
					//
					/////////////////////////////////////
					SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

					inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
					if(inputSHPDS == NULL)
					{
						string message = string("Could not open vector file ") + this->inputVector;
						throw RSGISFileException(message.c_str());
					}
					inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
					if(inputSHPLayer == NULL)
					{
						string message = string("Could not open vector layer ") + SHPFileInLayer;
						throw RSGISFileException(message.c_str());
					}

					if(first)
					{
						inputSpatialRef = inputSHPLayer->GetSpatialRef();

						// Get Geometry Type.
						OGRFeature *feature = inputSHPLayer->GetFeature(0);
						OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();

						/////////////////////////////////////
						//
						// Create Output Shapfile.
						//
						/////////////////////////////////////
						const char *pszDriverName = "ESRI Shapefile";
						shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
						if( shpFiledriver == NULL )
						{
							throw RSGISVectorOutputException("SHP driver not available.");
						}
						outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
						if( outputSHPDS == NULL )
						{
							string message = string("Could not create vector file ") + this->outputVector;
							throw RSGISVectorOutputException(message.c_str());
						}
						outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
						if( outputSHPLayer == NULL )
						{
							string message = string("Could not create vector layer ") + SHPFileOutLayer;
							throw RSGISVectorOutputException(message.c_str());
						}

						if(!ignoreAttr)
						{
							appendToVector.copyFeatureDefn(outputSHPLayer, inputSHPLayer->GetLayerDefn());
						}

						first = false;
					}
					else
					{
						// Do Nothing...
					}

					try
					{
						appendToVector.appendLayer(inputSHPLayer, outputSHPLayer, ignoreAttr);

						GDALClose(inputSHPDS);

					}
					catch (RSGISVectorException &e)
					{
						GDALClose(inputSHPDS);
						GDALClose(outputSHPDS);
						//OGRCleanupAll();
						throw e;
					}

				}

				GDALClose(outputSHPDS);
				//OGRCleanupAll();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::fixpolyextboundary)
		{
			cout << "Fix geometric errors in polygon boundary (note not all errors can be corrected.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayer->GetFeature(0);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

				RSGISCopyCheckPolygons copyCheckPolygons;
				try
				{
					copyCheckPolygons.copyCheckPolygons(inputSHPLayer, outputSHPLayer, true);

					GDALClose(inputSHPDS);
					GDALClose(outputSHPDS);
					//OGRCleanupAll();
				}
				catch (RSGISVectorException &e)
				{
					GDALClose(inputSHPDS);
					GDALClose(outputSHPDS);
					//OGRCleanupAll();
					throw e;
				}
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::removepolyholes)
		{
			cout << "Remove holes in polygons.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
            if(this->areaThresholdProvided)
            {
                cout << "Area threshold: " << this->threshold << endl;
            }

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayer->GetFeature(0);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}


				try
				{
                    RSGISRemovePolygonHoles removePolyHolesObj = RSGISRemovePolygonHoles(this->threshold, this->areaThresholdProvided);
                    removePolyHolesObj.removeholes(inputSHPLayer, outputSHPLayer);

					GDALClose(inputSHPDS);
					GDALClose(outputSHPDS);
					//OGRCleanupAll();
				}
				catch (RSGISVectorException &e)
				{
					GDALClose(inputSHPDS);
					GDALClose(outputSHPDS);
					//OGRCleanupAll();
					throw e;
				}
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::dropsmallpolys)
		{
			cout << "Drop Small Polygons\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Threshold: " << this->threshold << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();

				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayer->GetFeature(0);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

				RSGISDropSmallPolygons dropSmallPolys;
				try
				{
					dropSmallPolys.dropSmallPolys(inputSHPLayer, outputSHPLayer, threshold);

					GDALClose(inputSHPDS);
					GDALClose(outputSHPDS);
					//OGRCleanupAll();
				}
				catch (RSGISVectorException &e)
				{
					GDALClose(inputSHPDS);
					GDALClose(outputSHPDS);
					//OGRCleanupAll();
					throw e;
				}
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::extractlargestpoly)
		{
			cout << "Extract the largest polygon in file.\n";
			cout << "Input File: " << this->inputVector << endl;
			cout << "Output File: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;

				vector<Polygon*> *largestPolyVec = new vector<Polygon*>();

				bool first = true;
				Polygon *largePoly;
				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); ++iterPolys)
				{
					if(first)
					{
						largePoly = *iterPolys;
						first = false;
					}
					else if(largePoly->getArea() < (*iterPolys)->getArea())
					{
						largePoly = *iterPolys;
					}
				}
				largestPolyVec->push_back(largePoly);

				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, largestPolyVec, spatialRef);

				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;
				delete largestPolyVec;

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::generatesinglepoly)
		{
			cout << "Create a polygon around the polygons within the input file\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			if(polygonizertype == lineproj)
			{
				cout << "The line projection polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
			}
			else if(polygonizertype == delaunay1)
			{
				cout << "The delaunay1 polygonizer will be used\n";
			}
			else if(polygonizertype == delaunay2)
			{
				cout << "The delaunay2 polygonizer will be used\n";
			}
			else if(polygonizertype == convexhull)
			{
				cout << "The convex hull polygonizer will be used\n";
			}
			else if(polygonizertype == multipoly)
			{
				cout << "The multi-polygon polygonizer will be used\n";
			}
			else if(polygonizertype == snakes)
			{
				cout << "The snakes polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
				cout << "Alpha = " << this->alpha << endl;
				cout << "Beta = " << this->beta << endl;
				cout << "Gamma = " << this->gamma << endl;
				cout << "Delta = " << this->delta << endl;
			}

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();

				vector<Polygon*> *polygons = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(polygons);
				processVector = new RSGISProcessVector(processFeature);

				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);

				delete processVector;
				delete processFeature;

				vector<Polygon*> *outPoly = new vector<Polygon*>();
				RSGISGeometry geomUtils;
				if(polygonizertype == lineproj)
				{
					RSGISIdentifyNonConvexPolygonsLineProject *identifyNonConvexLineProj = new RSGISIdentifyNonConvexPolygonsLineProject(resolution);
					outPoly->push_back(identifyNonConvexLineProj->retrievePolygon(polygons));
					delete identifyNonConvexLineProj;
				}
				else if(polygonizertype == delaunay1)
				{
					RSGISIdentifyNonConvexPolygonsDelaunay *identifyNonConvexDelaunay = new RSGISIdentifyNonConvexPolygonsDelaunay();
					outPoly->push_back(identifyNonConvexDelaunay->retrievePolygon(polygons));
					delete identifyNonConvexDelaunay;
				}
				else if(polygonizertype == convexhull)
				{
					outPoly->push_back(geomUtils.findConvexHull(polygons));
				}
				else if(polygonizertype == snakes)
				{
					RSGISIdentifyNonConvexPolygonsSnakes *identifyNonConvexSnakes = new RSGISIdentifyNonConvexPolygonsSnakes(resolution, spatialRef, alpha, beta, gamma, delta, maxNumIterations);
					outPoly->push_back(identifyNonConvexSnakes->retrievePolygon(polygons));
					delete identifyNonConvexSnakes;
				}
				else
				{
					throw RSGISException("Do not know polygonization option.");
				}

				vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, outPoly, spatialRef);

				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				delete polygons;

				for(iterPolys = outPoly->begin(); iterPolys != outPoly->end(); )
				{
					delete *iterPolys;
					outPoly->erase(iterPolys);
				}
				delete outPoly;

				GDALClose(inputSHPDS);
				//OGRCleanupAll();
			}
			catch(RSGISException e)
			{
				throw e;
			}

		}
		else if(option == RSGISExeVectorUtils::generategrid)
		{
			cout << "A command to generate a regular grid of polygons\n";
			cout << "Output File: " << this->outputVector << endl;
			cout << "Projection: " << this->proj4 << endl;
			cout << "TLX: " << tlx << endl;
			cout << "TLY: " << tly << endl;
			cout << "X Resolution: " << xres << endl;
			cout << "Y Resolution: " << yres << endl;
			cout << "Width: " << width << endl;
			cout << "Height: " << height << endl;

			// Convert to absolute path
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			RSGISVectorProcessing vecProcessing;
			OGRSpatialReference ogrSpatial = OGRSpatialReference();
			ogrSpatial.importFromProj4(this->proj4.c_str());
			try
			{
				vecProcessing.createGrid(outputVector, &ogrSpatial, force, tlx, tly, xres, yres, width, height);
			}
			catch (RSGISVectorException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::generateimagegrid)
		{
			cout << "A command to generate a regular grid of polygons for an image\n";
			cout << "Input Image: " << this->inputFile << endl;
			cout << "Output File: " << this->outputVector << endl;
			cout << "X Resolution: " << xres << endl;
			cout << "Y Resolution: " << yres << endl;

            // Convert to absolute path
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			GDALAllRegister();
			GDALDataset *dataset = NULL;
			dataset = (GDALDataset *) GDALOpenShared(this->inputFile.c_str(), GA_ReadOnly);
			double *transform = new double[6];
			dataset->GetGeoTransform(transform);
			int pxlWidth = dataset->GetRasterXSize();
			int pxlHeight = dataset->GetRasterYSize();
			OGRSpatialReference ogrSpatial = OGRSpatialReference(dataset->GetProjectionRef());
			GDALClose(dataset);

			if(transform[1] < 0)
			{
				transform[1] = transform[1] * (-1);
			}

			if(transform[5] < 0)
			{
				transform[5] = transform[5] * (-1);
			}

			this->tlx = transform[0];
			this->tly = transform[3];
			this->width = ((double)pxlWidth)*transform[1];
			this->height = ((double)pxlHeight)*transform[5];

			delete[] transform;

			cout << "TLX: " << tlx << endl;
			cout << "TLY: " << tly << endl;
			cout << "Width: " << width << endl;
			cout << "Height: " << height << endl;

			RSGISVectorProcessing vecProcessing;
			try
			{
				vecProcessing.createGrid(outputVector, &ogrSpatial, force, tlx, tly, xres, yres, width, height);
			}
			catch (RSGISVectorException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeVectorUtils::vectormaths)
		{
			cout << "Vector Maths\n";
			cout << "Input Vector: " << this->inputFile << endl;
			cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputFile);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;
			OGRFeatureDefn *inFeatureDefn = NULL;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(this->inputFile.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputFile;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();
				inFeatureDefn = inputSHPLayer->GetLayerDefn();

                // Get Geometry Type
                OGRFeature *feature = inputSHPLayer->GetFeature(0);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

				//Parser *muParser = new Parser();
				/*VariableFields **processVaribles = new VariableFields*[this->numVars];

				for(int i = 0; i < this->numVars; ++i)
				{
					cout << i << endl;
					processVaribles[i]->name = this->variables[i].name;
					processVaribles[i]->fieldName = this->variables[i].fieldName;
				}*/

				/*value_type *inVals = new value_type[this->numVars];
				for(int i = 0; i < this->numVars; ++i)
				{
					inVals[i] = 0;
					muParser->DefineVar(_T(this->variables[i].name.c_str()), &inVals[i]);
				}

				muParser->SetExpr(this->mathsExpression.c_str());*/

				processFeature = new RSGISVectorMaths(this->variables, this->numVars, this->mathsExpression, this->outHeading);
				processVector = new RSGISProcessVector(processFeature);
				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);

				GDALClose(inputSHPDS);
				GDALClose(outputSHPDS);

				delete processVector;
				delete processFeature;

				//OGRCleanupAll();
			}
			catch (RSGISException e)
			{
				throw e;
			}
		}
        else if(option == RSGISExeVectorUtils::select)
        {
            cout << "Select features from shapefile using SQL statement and exporting as new file\n";
            cout << "Input Vector: " << this->inputFile << endl;
			cout << "Output Vector: " << this->outputVector << endl;
            cout << "Expression: " << this->sqlExpression << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

            OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputFile);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;
			OGRFeatureDefn *inFeatureDefn = NULL;

			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(this->inputFile.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputFile;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();
				inFeatureDefn = inputSHPLayer->GetLayerDefn();
                OGRwkbGeometryType wktGeomType = inputSHPLayer->GetGeomType();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wktGeomType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

                string sqlStatement =  string("SELECT * FROM ") + SHPFileInLayer + string(" WHERE ") + sqlExpression;
                cout << "SQL: " << sqlStatement << endl;

                RSGISCopyFeatures *copyFeatures = new RSGISCopyFeatures();
                RSGISProcessVectorSQL processSQLFeatures(copyFeatures);

                processSQLFeatures.processVectors(inputSHPDS, outputSHPLayer, true, false, sqlStatement);

                delete copyFeatures;

				GDALClose(inputSHPDS);
				GDALClose(outputSHPDS);

				//OGRCleanupAll();
			}
			catch (RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeVectorUtils::calcmeanmindist)
        {
            cout << "Calculate the mean minimum distance between all the geometries.\n";
            cout << "Input Vector: " << this->inputFile << endl;
            if(outTextFile)
            {
                cout << "Output Text File: " << this->outputTextFile << endl;
            }

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

            OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputFile);

			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			try
			{

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(this->inputFile.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputFile;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}

                // Get Geometries into memory
                vector<OGRGeometry*> *ogrGeoms = new vector<OGRGeometry*>();
                RSGISGetOGRGeometries *getOGRGeoms = new RSGISGetOGRGeometries(ogrGeoms);
                RSGISProcessVector processVector = RSGISProcessVector(getOGRGeoms);
                processVector.processVectorsNoOutput(inputSHPLayer, false);
                delete getOGRGeoms;

                // Calculate mean min distance
                RSGISVectorProcessing vecProcess;
                float meanMinDist = vecProcess.calcMeanMinDistance(ogrGeoms);

                cout << "Mean Minimum Distance = " << meanMinDist << endl;

                if(outTextFile)
                {
                    ofstream outTxtFile;
                    outTxtFile.open(outputTextFile.c_str(), ios::out | ios::trunc);
                    if(outTxtFile.is_open())
                    {
                        outTxtFile.precision(10);
                        outTxtFile << meanMinDist << endl;
                        outTxtFile.flush();
                        outTxtFile.close();
                    }
                    else
                    {
                        throw RSGISException("Text file could not be created.");
                    }
                }

                // Clean up memory.
                for(vector<OGRGeometry*>::iterator iterGeoms = ogrGeoms->begin(); iterGeoms != ogrGeoms->end(); )
                {
                    OGRGeometryFactory::destroyGeometry(*iterGeoms);
                    iterGeoms = ogrGeoms->erase(iterGeoms);
                }
                delete ogrGeoms;

				GDALClose(inputSHPDS);

				//OGRCleanupAll();
			}
			catch (RSGISException e)
			{
				throw e;
			}
        }
        else if(option == RSGISExeVectorUtils::copyassignproj)
        {
            cout << "Copy and assign layer to a specific projection\n";
            cout << "Input Vector: " << this->inputVector << endl;
            cout << "Output Vector: " << this->outputVector << endl;
            cout << "Projection File: " << this->projFile << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

            try
            {
                OGRRegisterAll();

                RSGISTextUtils textUtils;
                string projWKTStr = textUtils.readFileToString(this->projFile);

                RSGISFileUtils fileUtils;
                RSGISVectorUtils vecUtils;

                GDALDataset *inputSHPDS = NULL;
                OGRLayer *inputSHPLayer = NULL;
                GDALDriver *shpFiledriver = NULL;
                GDALDataset *outputSHPDS = NULL;
                OGRLayer *outputSHPLayer = NULL;
                OGRFeatureDefn *inFeatureDefn = NULL;

                string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
                string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

                string outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inFeatureDefn = inputSHPLayer->GetLayerDefn();
                OGRwkbGeometryType wktGeomType = inputSHPLayer->GetGeomType();


                OGRSpatialReference *outSpatialRef = new OGRSpatialReference(projWKTStr.c_str());

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), outSpatialRef, wktGeomType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

                RSGISCopyFeatures *copyFeatures = new RSGISCopyFeatures();
                RSGISProcessVector *processVector = new RSGISProcessVector(copyFeatures);
				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);

                delete copyFeatures;
                delete processVector;

                GDALClose(inputSHPDS);
                GDALClose(outputSHPDS);
            }
            catch (RSGISException &e)
            {
                throw e;
            }
        }
        else if(option == RSGISExeVectorUtils::printwkt)
        {
            cout << "Printing WKT string for vector layer\n";
            cout << "Input Vector: " << this->inputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();

            try
            {
                OGRRegisterAll();

                //RSGISFileUtils fileUtils;
                RSGISVectorUtils vecUtils;

                GDALDataset *inputSHPDS = NULL;
                OGRLayer *inputSHPLayer = NULL;

                string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
                OGRSpatialReference *spatialRef = inputSHPLayer->GetSpatialRef();

                char **wktPrettySpatialRef = new char*[1];
                spatialRef->exportToPrettyWkt(wktPrettySpatialRef);
                cout << wktPrettySpatialRef[0] << endl;
                OGRFree(wktPrettySpatialRef);

                GDALClose(inputSHPDS);
            }
            catch (RSGISException &e)
            {
                throw e;
            }
        }
        else if(option == RSGISExeVectorUtils::addfidcol)
        {
            cout << "Copy the data and add a FID column\n";
            cout << "Input Vector: " << this->inputVector << endl;
            cout << "Output Vector: " << this->outputVector << endl;
            cout << "Init Value: " << this->initFID << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

            try
            {
                OGRRegisterAll();

                RSGISFileUtils fileUtils;
                RSGISVectorUtils vecUtils;

                GDALDataset *inputSHPDS = NULL;
                OGRLayer *inputSHPLayer = NULL;
                GDALDriver *shpFiledriver = NULL;
                GDALDataset *outputSHPDS = NULL;
                OGRLayer *outputSHPLayer = NULL;
                OGRFeatureDefn *inFeatureDefn = NULL;

                string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
                string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

                string outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inFeatureDefn = inputSHPLayer->GetLayerDefn();
                OGRwkbGeometryType wktGeomType = inputSHPLayer->GetGeomType();
                OGRSpatialReference *outSpatialRef = inputSHPLayer->GetSpatialRef();

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), outSpatialRef, wktGeomType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

                RSGISCopyFeaturesAddFIDCol *copyFeatures = new RSGISCopyFeaturesAddFIDCol(initFID);
                RSGISProcessVector *processVector = new RSGISProcessVector(copyFeatures);
				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);

                delete copyFeatures;
                delete processVector;

                GDALClose(inputSHPDS);
                GDALClose(outputSHPDS);
            }
            catch (RSGISException &e)
            {
                throw e;
            }
        }
        else if(option == RSGISExeVectorUtils::mindist2polys)
        {
            cout << "A command to calculate the minimum distance to each polygon for each point.\n";
            cout << "Input Vector: " << this->inputVector << endl;
            cout << "Polygons Vector: " << this->inputGeometry << endl;
            cout << "Output Vector: " << this->outputVector << endl;

            // Convert to absolute path
            this->inputVector = boost::filesystem::absolute(this->inputVector).c_str();
            this->inputGeometry = boost::filesystem::absolute(this->inputGeometry).c_str();
            this->outputVector = boost::filesystem::absolute(this->outputVector).c_str();

            try
            {
                OGRRegisterAll();

                RSGISFileUtils fileUtils;
                RSGISVectorUtils vecUtils;

                GDALDataset *inputSHPDS = NULL;
                OGRLayer *inputSHPLayer = NULL;
                GDALDataset *inputPolysSHPDS = NULL;
                OGRLayer *inputPolysSHPLayer = NULL;
                GDALDriver *shpFiledriver = NULL;
                GDALDataset *outputSHPDS = NULL;
                OGRLayer *outputSHPLayer = NULL;
                OGRFeatureDefn *inFeatureDefn = NULL;

                string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
                string SHPFileInPolysLayer = vecUtils.getLayerName(this->inputGeometry);
                string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);

                string outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);

				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inFeatureDefn = inputSHPLayer->GetLayerDefn();
                OGRwkbGeometryType wktGeomType = inputSHPLayer->GetGeomType();
                OGRSpatialReference *outSpatialRef = inputSHPLayer->GetSpatialRef();


                /////////////////////////////////////
				//
				// Open Input Polygons Shapfile.
				//
				/////////////////////////////////////
				inputPolysSHPDS = (GDALDataset*) GDALOpenEx(this->inputGeometry.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputPolysSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputGeometry;
					throw RSGISFileException(message.c_str());
				}
				inputPolysSHPLayer = inputPolysSHPDS->GetLayerByName(SHPFileInPolysLayer.c_str());
				if(inputPolysSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInPolysLayer;
					throw RSGISFileException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), outSpatialRef, wktGeomType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}

                cout << "Reading Polygons\n";
                std::vector<OGRGeometry*> *polys = new std::vector<OGRGeometry*>();
                RSGISGetOGRGeometries *getPolys = new RSGISGetOGRGeometries(polys);
                RSGISProcessVector *readVector = new RSGISProcessVector(getPolys);
				readVector->processVectorsNoOutput(inputPolysSHPLayer, false);
                delete readVector;
                delete getPolys;

                cout << "There are " << polys->size() << " geometries to which distances will be calulated\n";

                cout << "Calculate Distances\n";
                RSGISCalcMinDists2Polys *calcMinDists = new RSGISCalcMinDists2Polys(polys);
                RSGISProcessVector *processVector = new RSGISProcessVector(calcMinDists);
				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);

                delete calcMinDists;
                delete processVector;

                for(std::vector<OGRGeometry*>::iterator iterGeoms = polys->begin(); iterGeoms != polys->end(); ++iterGeoms)
                {
                    delete *iterGeoms;
                }
                delete polys;

                GDALClose(inputSHPDS);
                GDALClose(inputPolysSHPDS);
                GDALClose(outputSHPDS);
            }
            catch (RSGISException &e)
            {
                throw e;
            }

        }
        else if(option == RSGISExeVectorUtils::convexhullgrps)
        {
            std::cout << "A command to produce convex hulls for groups of (X, Y, Attribute) point locations\n";
            std::cout << "Input File: " << this->inputFile << std::endl;
            std::cout << "Output File: " << this->outputVector << std::endl;
            std::cout << "Proj: " << this->projFile << std::endl;
            std::cout << "Eastings Column: " << this->eastingsColIdx << std::endl;
            std::cout << "Northings Column: " << this->northingsColIdx << std::endl;
            std::cout << "Attribute Column: " << this->attributeColIdx << std::endl;
            
            try
            {
                rsgis::cmds::executeGenerateConvexHullsGroups(this->inputFile, this->outputVector, this->projFile, this->force, this->eastingsColIdx, this->northingsColIdx, this->attributeColIdx);
            }
            catch (rsgis::cmds::RSGISCmdException &e)
            {
                throw RSGISException(e.what());
            }
            catch(rsgis::RSGISException &e)
            {
                throw e;
            }
            catch(std::exception &e)
            {
                throw RSGISException(e.what());
            }
        }
		else
		{
			cout << "Options not recognised\n";
		}

		delete RSGISGEOSFactoryGenerator::getInstance();
	}
}


void RSGISExeVectorUtils::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeVectorUtils::removeAttributes)
		{
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::bufferVector)
		{
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Buffer: " << this->buffer << endl;
		}
		else if(option == RSGISExeVectorUtils::geom2circles)
		{
			cout << "Converting Geometries to Circles\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Area Attribute: " << this->areaAttribute << endl;
			cout << "Height Attribute: " << this->heightAttribute << endl;
			cout << "Radius Attribute: " << this->radiusAttribute << endl;
			cout << "Radius Value: " << this->radius << endl;
			cout << "Resolution: " << this->resolution << endl;
		}
		else if(option == RSGISExeVectorUtils::printpolygeom)
		{
			cout << "Print Polygon Geometry\n";
			cout << "Input Vector: " << this->inputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::findreplacetext)
		{
			cout << "Find and replace text attribute\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Attribute: " << this->attribute << endl;
			cout << "Find: " << this->find << endl;
			cout << "Replace: " << this->replace << endl;
		}
		else if(option == RSGISExeVectorUtils::listattributes)
		{
			cout << "List attributes\n";
			cout << "Input Vector: " << this->inputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::printattribute)
		{
			cout << "Print attribute for each feature\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Attribute: " << this->attribute << endl;
		}
		else if(option == RSGISExeVectorUtils::topattributes)
		{
			cout << "Calculate the top attributes\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Number of top attributes: " << this->numTop << endl;
			if(summary == rsgis::math::sumtype_aggregate)
			{
				cout << "Summary aggregate columns will be provided\n";
			}
			else if(summary == rsgis::math::sumtype_value)
			{
				cout << "The column values will be used for summary\n";
			}
			cout << "Columns to be considered are:\n";
			for(int i = 0; i < numAttributes; i++)
			{
				cout << i << ":\t" << attributes[i] << endl;
			}
		}
		else if(option == RSGISExeVectorUtils::addattributes)
		{
			cout << "Add attributes to a Vector Layer\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

			for(int i = 0; i < numAttributes; i++)
			{
				cout << i << ":\t" << newAttributes[i]->name << " ";
				if(newAttributes[i]->type == OFTInteger)
				{
					cout << "OFTInteger" << endl;
				}
				else if(newAttributes[i]->type == OFTIntegerList)
				{
					cout << "OFTIntegerList" << endl;
				}
				else if(newAttributes[i]->type == OFTReal)
				{
					cout << "OFTReal" << endl;
				}
				else if(newAttributes[i]->type == OFTRealList)
				{
					cout << "OFTRealList" << endl;
				}
				else if(newAttributes[i]->type == OFTString)
				{
					cout << "OFTString" << endl;
				}
				else if(newAttributes[i]->type == OFTStringList)
				{
					cout << "OFTStringList" << endl;
				}
				else if(newAttributes[i]->type == OFTWideString)
				{
					cout << "OFTWideString" << endl;
				}
				else if(newAttributes[i]->type == OFTWideStringList)
				{
					cout << "OFTWideStringList" << endl;
				}
				else if(newAttributes[i]->type == OFTBinary)
				{
					cout << "OFTBinary" << endl;
				}
				else if(newAttributes[i]->type == OFTDate)
				{
					cout << "OFTDate" << endl;
				}
				else if(newAttributes[i]->type == OFTTime)
				{
					cout << "OFTTime" << endl;
				}
				else if(newAttributes[i]->type == OFTDateTime)
				{
					cout << "OFTDateTime" << endl;
				}
				else
				{
					cout << "UNKNOWN!\n";
				}
			}
		}
		else if(option == RSGISExeVectorUtils::copypolygons)
		{
			cout << "Copy Vector Layer\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::printfids)
		{
			cout << "Print FIDs\n";
			cout << "Input Vector: " << this->inputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::countpxlsarea)
		{
			cout << "Calculate the number of pixels within a polygon and the polygon area\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Raster Polygons: " << this->inputRasterVector << endl;
		}
		else if(option == RSGISExeVectorUtils::points2ascii)
		{
			cout << "Convert points shapefile to a comma separated text file.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::rotate)
		{
			cout << "Rotate geometry around a fixed point\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Point: " << this->fixedPt->toString() << endl;
			cout << "Angle: " << this->angle << endl;
		}
		else if(option == RSGISExeVectorUtils::calcarea)
		{
			cout << "Calculating Polygon Area\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::splitfeatures)
		{
			cout << "Split features into individual shapefiles..\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output DIR: " << this->output_DIR << endl;
		}
		else if(option == RSGISExeVectorUtils::removepolyoverlaps)
		{
			cout << "Remove overlapping polygons in shapefile..\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Snapped to a grid tolerance: " << this->tolerance << endl;
			cout << "Area below which polygons are dissolve (if contained): " << this->dissolve << endl;
		}
		else if(option == RSGISExeVectorUtils::mergesmallpolysnear)
		{
			cout << "Merge small polygons to the nearest large polygon..\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Area threshold for small polygons: " << this->area << endl;
			cout << "Resolution for polygon outline identification: " << this->resolution << endl;
		}
		else if(option == RSGISExeVectorUtils::mergesmallpolystouching)
		{
			cout << "Merge small polygons a neighboring polygon with which is shares a border..\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Area threshold for small polygons: " << this->area << endl;
		}
		else if(option == RSGISExeVectorUtils::morphologicalclosing)
		{
			cout << "Perform a morphological closing\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Buffer: " << this->buffer << endl;
		}
		else if(option == RSGISExeVectorUtils::morphologicalopening)
		{
			cout << "Perform a morphological opening\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Buffer: " << this->buffer << endl;
		}
		else if(option == RSGISExeVectorUtils::morphologicaldilation)
		{
			cout << "Perform a morphological dilation\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Buffer: " << this->buffer << endl;
		}
		else if(option == RSGISExeVectorUtils::morphologicalerosion)
		{
			cout << "Perform a morphological erosion\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Buffer: " << this->buffer << endl;
		}
		else if(option == RSGISExeVectorUtils::nonconvexoutlinedelaunay)
		{
			cout << "Identify the non-convex outline of the input vector\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::convexoutline)
		{
			cout << "Identify the convex outline of the input vector\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::nonconvexoutlinesnakes)
		{
			cout << "Identify the non-convex outline of the input vector\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::nonconvexoutlinelineproj)
		{
			cout << "Identify the non-convex outline of the input vector\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Resolution: " << this->resolution << endl;
		}
		else if(option == RSGISExeVectorUtils::mergetouchingpoly)
		{
			cout << "Merge touching polygons\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::freqdist)
		{
			cout << "Produce a frequency distribution for the attribute in the input shapefile.\n";
			cout << "Input Files: \n";
			list<string>::iterator iterFiles;
			for(iterFiles = inputVectors->begin(); iterFiles != inputVectors->end(); ++iterFiles)
			{
				cout << *iterFiles << endl;
			}
			cout << "Output plot file: " << this->outputPlot << endl;
			cout << "Attribute is: " << this->attribute << endl;
		}
		else if(option == RSGISExeVectorUtils::rasterise)
		{
			cout << "Rasterise a vector\n";
			cout << "Input Vector = " << this->inputVector << endl;
			cout << "Output Image = " << this->outputImage << endl;
			cout << "Attribute = " << this->attribute << endl;
			cout << "Resolution = " << this->resolution << endl;
			cout << "Const Value = " << this->constVal << endl;
		}
		else if(option == RSGISExeVectorUtils::polygonplots)
		{
			cout << "Create plot polygons from input CSV file\n";
			cout << "Input CSV = " << this->inputFile << endl;
			cout << "Output Vector = " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::polygonizempolys)
		{
			cout << "Create a polygon around each Multi-Polygon.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

			if(polygonizertype == lineproj)
			{
				cout << "The line projection polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
			}
			else if(polygonizertype == delaunay1)
			{
				cout << "The delaunay1 polygonizer will be used\n";
			}
			else if(polygonizertype == delaunay2)
			{
				cout << "The delaunay2 polygonizer will be used\n";
			}
			else if(polygonizertype == convexhull)
			{
				cout << "The convex hull polygonizer will be used\n";
			}
			else if(polygonizertype == multipoly)
			{
				cout << "The multi-polygon polygonizer will be used\n";
			}
			else if(polygonizertype == snakes)
			{
				cout << "The snakes polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
				cout << "Alpha = " << this->alpha << endl;
				cout << "Beta = " << this->beta << endl;
				cout << "Gamma = " << this->gamma << endl;
				cout << "Delta = " << this->delta << endl;
			}
		}
		else if(option == RSGISExeVectorUtils::scatter2D)
		{
			cout << "Create scatter2d plot file from two attributes\n";
			cout << "Input Vector = " << this->inputVector << endl;
			cout << "Output Plot = " << this->outputPlot << endl;
			cout << "Attribute1 = " << this->attribute1 << endl;
			cout << "Attribute2 = " << this->attribute2 << endl;
		}
		else if(option == RSGISExeVectorUtils::copycheckpolys)
		{
			cout << "Copy and check Polygon Layer\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::splitbyattribute)
		{
			cout << "Split input vector by attribute value.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector Base: " << this->outputVectorBase << endl;
			cout << "Attribute: " << this->attribute << endl;
		}
		else if(option == RSGISExeVectorUtils::removecontainedpolygons)
		{
			cout << "Remove polygons which are contained within another polygon.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::removecontainedpolygonsDIR)
		{
			cout << "Remove polygons which are contained within another polygon for a directory.\n";
			cout << "Input DIR: " << this->inputDIR << endl;
			cout << "Output DIR: " << this->output_DIR << endl;
		}
		else if(option == RSGISExeVectorUtils::mergetouchingpolyDIR)
		{
			cout << "Merge touching polygons with a shapefile for a directory.\n";
			cout << "Input DIR: " << this->inputDIR << endl;
			cout << "Output DIR: " << this->output_DIR << endl;
			if(attributeDef)
			{
				cout << "The attribute " << this->attribute << " will be added to the output.\n";
			}
		}
		else if(option == RSGISExeVectorUtils::splitlargesmall)
		{
			cout << "Split small and large polygons into separate shapefiles.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Large Vector: " << this->outputlarge << endl;
			cout << "Output Small Vector: " << this->outputsmall << endl;
			cout << "Threshold = " << this->threshold << endl;
		}
		else if(option == RSGISExeVectorUtils::removepolygonscontainedwithingeom)
		{
			cout << "Remove Polygons contained with the inputted geometry.\n";
			cout << "Input Geometry: " << this->inputGeometry << endl;
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::mergeshps)
		{
			cout << "Merge the input shapefiles to a single shapefile.\n";
			cout << "Input Files: \n";
			list<string>::iterator iterFiles;
			for(iterFiles = inputVectors->begin(); iterFiles != inputVectors->end(); ++iterFiles)
			{
				cout << *iterFiles << endl;
			}
			cout << "Output Vector: " << this->outputVector << endl;
			if(ignoreAttr)
			{
				cout << "The attributes will be ignored\n";
			}
			else
			{
				cout << "The attributes will be outputed\n";
			}
		}
		else if(option == RSGISExeVectorUtils::fixpolyextboundary)
		{
			cout << "Fix geometric errors in polygon boundary (note not all errors can be corrected.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::removepolyholes)
		{
			cout << "Remove holes in polygons.\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::dropsmallpolys)
		{
			cout << "Drop Small Polygons\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;
			cout << "Threshold: " << this->threshold << endl;
		}
		else if(option == RSGISExeVectorUtils::extractlargestpoly)
		{
			cout << "Extract the largest polygon in file.\n";
			cout << "Input File: " << this->inputVector << endl;
			cout << "Output File: " << this->outputVector << endl;
		}
		else if(option == RSGISExeVectorUtils::generatesinglepoly)
		{
			cout << "Create a polygon around the polygons within the input file\n";
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Output Vector: " << this->outputVector << endl;

			if(polygonizertype == lineproj)
			{
				cout << "The line projection polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
			}
			else if(polygonizertype == delaunay1)
			{
				cout << "The delaunay1 polygonizer will be used\n";
			}
			else if(polygonizertype == delaunay2)
			{
				cout << "The delaunay2 polygonizer will be used\n";
			}
			else if(polygonizertype == convexhull)
			{
				cout << "The convex hull polygonizer will be used\n";
			}
			else if(polygonizertype == multipoly)
			{
				cout << "The multi-polygon polygonizer will be used\n";
			}
			else if(polygonizertype == snakes)
			{
				cout << "The snakes polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
				cout << "Alpha = " << this->alpha << endl;
				cout << "Beta = " << this->beta << endl;
				cout << "Gamma = " << this->gamma << endl;
				cout << "Delta = " << this->delta << endl;
			}
		}
        else if(option == RSGISExeVectorUtils::select)
        {
            cout << "Select features from shapefile using SQL statement and exporting as new file\n";
            cout << "Input Vector: " << this->inputFile << endl;
			cout << "Output Vector: " << this->outputVector << endl;
            cout << "Expression: " << this->sqlExpression << endl;
        }
        else if(option == RSGISExeVectorUtils::calcmeanmindist)
        {
            cout << "Calculate the mean minimum distance between all the geometries.\n";
            cout << "Input Vector: " << this->inputFile << endl;
            if(outTextFile)
            {
                cout << "Output Text File: " << this->outputTextFile << endl;
            }
        }
        else if(option == RSGISExeVectorUtils::copyassignproj)
        {
            cout << "Copy and assign layer to a specific projection\n";
            cout << "Input Vector: " << this->inputVector << endl;
            cout << "Output Vector: " << this->outputVector << endl;
            cout << "Projection File: " << this->projFile << endl;
        }
        else if(option == RSGISExeVectorUtils::printwkt)
        {
            cout << "Printing WKT string for vector layer\n";
            cout << "Input Vector: " << this->inputVector << endl;
        }
        else if(option == RSGISExeVectorUtils::addfidcol)
        {
            cout << "Copy the data and add a FID column\n";
            cout << "Input Vector: " << this->inputVector << endl;
            cout << "Output Vector: " << this->outputVector << endl;
        }
        else if(option == RSGISExeVectorUtils::mindist2polys)
        {
            cout << "A command to calculate the minimum distance to each polygon for each point.\n";
            cout << "Input Vector: " << this->inputVector << endl;
            cout << "Polygons Vector: " << this->inputGeometry << endl;
            cout << "Output Vector: " << this->outputVector << endl;
        }
        else if(option == RSGISExeVectorUtils::convexhullgrps)
        {
            std::cout << "A command to produce convex hulls for groups of (X, Y, Attribute) point locations\n";
            std::cout << "Input File: " << this->inputFile << std::endl;
            std::cout << "Output File: " << this->outputVector << std::endl;
            std::cout << "Proj: " << this->projFile << std::endl;
            std::cout << "Eastings Column: " << this->eastingsColIdx << std::endl;
            std::cout << "Northings Column: " << this->northingsColIdx << std::endl;
            std::cout << "Attribute Column: " << this->attributeColIdx << std::endl;
        }
		else
		{
			cout << "Options not recognised\n";
		}
	}
	else
	{
		cout << "The parameters have yet to be parsed\n";
	}
}

void RSGISExeVectorUtils::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"bufferpolygons\" vector=\"vector.shp\" output=\"vector_out.shp\" buffer=\"float\" force=\"yes | no\"/>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"removeattributes\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\"/>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"geom2circles\" vector=\"vector.shp\" output=\"vector_out.shp\" areaattribute=\"string\" radiusattribute=\"string\" radius=\"float\" resolution=\"float\" force=\"yes | no\"/>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"printpolygeom\" vector=\"vector.shp\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"listattributes\" vector=\"vector.shp\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"findreplacetext\" vector=\"vector.shp\" attribute=\"name\" find=\"find_text\" replace=\"replace_text\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"printattribute\" vector=\"vector.shp\" attribute=\"name\" />\n";
	cout <<"\t<rsgis:command algor=\"vectorutils\" option=\"topattributes\" vector=\"vector.shp\" output=\"vector_out.shp\" top=\"int\" summary=\"aggregate | value\" force=\"yes | no\">\n";
	cout <<"\t\t<rsgis:attribute name=\"string\" />\n";
	cout <<"\t\t<rsgis:attribute name=\"string\" />\n";
	cout <<"\t\t<rsgis:attribute name=\"string\" />\n";
	cout <<"\t\t<rsgis:attribute name=\"string\" />\n";
	cout <<"\t</rsgis:command>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"addattributes\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\">\n";
	cout << "\t\t<rsgis:attribute name=\"string\" type=\"OFTInteger | OFTIntegerList | OFTReal | OFTRealList | OFTString | OFTStringList | OFTWideString | OFTWideStringList | OFTBinary | OFTDate | OFTTime | OFTDateTime\"/>\n";
	cout << "\t\t<rsgis:attribute name=\"string\" type=\"OFTInteger | OFTIntegerList | OFTReal | OFTRealList | OFTString | OFTStringList | OFTWideString | OFTWideStringList | OFTBinary | OFTDate | OFTTime | OFTDateTime\"/>\n";
	cout << "\t\t<rsgis:attribute name=\"string\" type=\"OFTInteger | OFTIntegerList | OFTReal | OFTRealList | OFTString | OFTStringList | OFTWideString | OFTWideStringList | OFTBinary | OFTDate | OFTTime | OFTDateTime\"/>\n";
	cout << "\t</rsgis:command>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"copypolygons\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"printfids\" vector=\"vector.shp\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"countpxlsarea\" vector=\"vector.shp\" raster=\"raster_poly.env\" output=\"vector_out.shp\" force=\"yes | no\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"points2ascii\" vector=\"vector.shp\" output=\"vector_out.txt\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"rotate\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" angle=\"float\">\n";
	cout << "\t\t<rsgis:point x=\"double\" y=\"double\" z=\"double\" />\n";
	cout << "\t</rsgis:command>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"calcarea\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"splitfeatures\" vector=\"vector.shp\" dir=\"output_dir\" force=\"yes | no\"/>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"removeoverlaps\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\"  snap2grid=\"yes | no\" tolerance=\"float\" dissolve=\"float\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"mergesmallpolysnear\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" area=\"float\" resolution=\"float\"/>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"mergesmallpolystouching\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" area=\"float\" relborder=\"float\"/>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"morphologicalclosing\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" buffer=\"float\" curve=\"int\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"morphologicalopening\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" buffer=\"float\" curve=\"int\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"morphologicalerosion\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" buffer=\"float\" curve=\"int\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"morphologicaldilation\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" buffer=\"float\" curve=\"int\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"convexoutline\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" resolution=\"float\"/>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"nonconvexoutlinesnakes\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"nonconvexoutlinedelaunay\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"nonconvexoutlinelineproj\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" resolution=\"float\"/>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"mergetouchingpolys\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"freqdist\" output=\"output\" dirlist=\"directory\" ext=\"string\" attribute=\"string\"/>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"freqdist\" output=\"output\" attribute=\"string\">\n";
	cout << "\t\t<rsgis:vector file=\"vector1\" />\n";
	cout << "\t\t<rsgis:vector file=\"vector2\" />\n";
	cout << "\t\t<rsgis:vector file=\"vector3\" />\n";
	cout << "\t\t<rsgis:vector file=\"vector4\" />\n";
	cout << "\t</rsgis:command>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"rasterise\" vector=\"vector.shp\" output=\"image_out.env\" resolution=\"float\" attribute=\"string\"\n";
	cout << "\t constval=\"float\" method=\"polyContainsPixel | polyContainsPixelCenter | polyOverlapsPixel | pixelContainsPoly | pixelContainsPolyCenter | adaptive\"/>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"polygonplots\" input=\"input.csv\" output=\"vector_out.shp\" force=\"yes | no\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"polygonizempolys\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" >\n";
	cout << "\t\t<rsgis:polygonizer algor=\"lineproj | delaunay1 | convexhull | snakes\" resolution=\"float\" alpha=\"float\" beta=\"float\" gamma=\"float\" delta=\"float\"/>\n";
	cout << "\t</rsgis:command>\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"copycheckpolys\" vector=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" />\n";
	cout << "\t<rsgis:command algor=\"vectorutils\" option=\"polygonsInPolygon\" vector=\"vector.shp\" coverVector=\"coverVector.shp\" \n";
	cout << "\t\tdir=\"output_dir\" attributeName=\"int\" force=\"yes | no\" />\n";
	cout <<	"\t<rsgis:command algor=\"vectorutils\" option=\"vectormaths\" input=\"string\" output=\"string\" expression=\"string\" outHeading=\"string\" force=\"yes | no\">\n";
	cout << "\t\t<rsgis:variable name=\"string\" fieldname=\"string\"/>\n";
	cout << "\t\t<rsgis:variable name=\"string\" fieldname=\"string\"/>\n";
	cout << "\t</rsgis:command>\n";
	cout << "</rsgis:commands>\n";
}

string RSGISExeVectorUtils::getDescription()
{
	return "Utilities for vector processing.";
}

string RSGISExeVectorUtils::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeVectorUtils::~RSGISExeVectorUtils()
{
	if(inputVectors != NULL)
	{
		inputVectors->clear();
		delete inputVectors;
	}
}

}

