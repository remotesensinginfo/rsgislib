/*
 *  RSGISExeZonalStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/12/2008.
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

#include "RSGISExeZonalStats.h"

namespace rsgisexe{

RSGISExeZonalStats::RSGISExeZonalStats() : RSGISAlgorithmParameters()
{
	this->algorithm = "zonalstats";

	this->option = RSGISExeZonalStats::none;
	this->inputImage = "";
	this->inputVecPolys = "";
	this->inputRasPolys = "";
	this->outputVecPolys = "";
	this->outputTextFile = "";
	this->outputMatrix = "";
	this->polyAttribute = "";
	this->classAttribute = "";
	this->attributeCountList = NULL;
	this->attributeMeanList = NULL;
	this->attributeFuzzyList = NULL;
	this->imageBand = 0;
	this->numAttributes = 0;
	this->nTop = 0;
	this->binsize = 0;
	this->mean = true;
	this->max = true;
	this->min = true;
	this->stddev = true;
	this->force = true;
}

RSGISAlgorithmParameters* RSGISExeZonalStats::getInstance()
{
	return new RSGISExeZonalStats();
}

void RSGISExeZonalStats::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;

	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *optionPolygonsToShp = XMLString::transcode("polygons2shp");
	XMLCh *optionRasterPolygonsToShp= XMLString::transcode("rasterpolygons2shp");
	XMLCh *optionRasterPolygonsToTxt = XMLString::transcode("rasterpolygons2txt");
	XMLCh *optionPixelVals2txt = XMLString::transcode("pixelVals2txt");
    XMLCh *optionVariablesToMatrix = XMLString::transcode("variables2matrix");
	XMLCh *optionImageToMatrix = XMLString::transcode("image2matrix");
	XMLCh *optionPixelCount = XMLString::transcode("pixelcount");
	XMLCh *optionPixelStats = XMLString::transcode("pixelstats");
	XMLCh *optionPixelMean = XMLString::transcode("pixelmean");
	XMLCh *optionPixelWeightedMean = XMLString::transcode("pixelWeightedMean");
	XMLCh *optionPixelMeanLSSVar = XMLString::transcode("pixelmeanLSSVar");
	XMLCh *optionFuzzy = XMLString::transcode("fuzzy");
	XMLCh *optionPowersetCount = XMLString::transcode("powersetcount");
	XMLCh *rsgisattributeXMLStr = XMLString::transcode("rsgis:attribute");
	XMLCh *rsgisbandXMLStr = XMLString::transcode("rsgis:band");
	XMLCh *optionPointValue = XMLString::transcode("pointvalue");
    XMLCh *optionEndmembers = XMLString::transcode("endmembers");
    XMLCh *optionImageZone2hdf = XMLString::transcode("imagezone2hdf");

	const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
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

	XMLCh *vectorXMLStr = XMLString::transcode("vector");
	if(argElement->hasAttribute(vectorXMLStr))
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
		this->inputVecPolys = string(charValue);
		XMLString::release(&charValue);
	}
	else
	{
		throw RSGISXMLArgumentsException("No \'vector\' attribute was provided.");
	}
	XMLString::release(&vectorXMLStr);

    // Don't check shapefile and image are the same Projection (common for all utilities)
    this->ignoreProjection = false;
    XMLCh *forceProjXMLStr = XMLString::transcode("ignoreProjection");
    if(argElement->hasAttribute(forceProjXMLStr))
    {
        XMLCh *yesStr = XMLString::transcode("yes");
        const XMLCh *forceValue = argElement->getAttribute(forceProjXMLStr);

        if(XMLString::equals(forceValue, yesStr))
        {
            this->ignoreProjection = true;
        }
        else
        {
            this->ignoreProjection = false;
        }
        XMLString::release(&yesStr);
    }
    XMLString::release(&forceProjXMLStr);

    // Check if band names, from image should be used
    this->useBandNames = false;
    XMLCh *useBandNamesXMLStr = XMLString::transcode("useBandNames");
    if(argElement->hasAttribute(useBandNamesXMLStr))
    {
        XMLCh *yesStr = XMLString::transcode("yes");
        const XMLCh *forceValue = argElement->getAttribute(useBandNamesXMLStr);

        if(XMLString::equals(forceValue, yesStr))
        {
            this->useBandNames = true;
        }
        else
        {
            this->useBandNames = false;
        }
        XMLString::release(&yesStr);
    }
    XMLString::release(&useBandNamesXMLStr);

	const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
	if(XMLString::equals(optionPolygonsToShp, optionXML))
	{
		this->option = RSGISExeZonalStats::polygons2shp;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVecPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *yes = XMLString::transcode("yes");

		XMLCh *meanXMLStr = XMLString::transcode("mean");
		if(argElement->hasAttribute(meanXMLStr))
		{
			const XMLCh *meanStr = argElement->getAttribute(meanXMLStr);
			if(XMLString::equals(meanStr, yes))
			{
				this->mean = true;
			}
			else
			{
				this->mean = false;
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'mean\' attribute was provided.");
		}
		XMLString::release(&meanXMLStr);


		XMLCh *minXMLStr = XMLString::transcode("min");
		if(argElement->hasAttribute(minXMLStr))
		{
			const XMLCh *minStr = argElement->getAttribute(minXMLStr);
			if(XMLString::equals(minStr, yes))
			{
				this->min = true;
			}
			else
			{
				this->min = false;
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'min\' attribute was provided.");
		}
		XMLString::release(&minXMLStr);


		XMLCh *maxXMLStr = XMLString::transcode("max");
		if(argElement->hasAttribute(maxXMLStr))
		{
			const XMLCh *maxStr = argElement->getAttribute(maxXMLStr);
			if(XMLString::equals(maxStr, yes))
			{
				this->max = true;
			}
			else
			{
				this->max = false;
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'max\' attribute was provided.");
		}
		XMLString::release(&maxXMLStr);

		XMLCh *stddevXMLStr = XMLString::transcode("stddev");
		if(argElement->hasAttribute(stddevXMLStr))
		{
			const XMLCh *stddevStr = argElement->getAttribute(stddevXMLStr);
			if(XMLString::equals(stddevStr, yes))
			{
				this->stddev = true;
			}
			else
			{
				this->stddev = false;
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'stddev\' attribute was provided.");
		}
		XMLString::release(&stddevXMLStr);

		XMLString::release(&yes);
	}
	else if(XMLString::equals(optionRasterPolygonsToShp, optionXML))
	{
		this->option = RSGISExeZonalStats::rasterpolygons2shp;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVecPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *rasterXMLStr = XMLString::transcode("raster");
		if(argElement->hasAttribute(rasterXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rasterXMLStr));
			this->inputRasPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'raster\' attribute was provided.");
		}
		XMLString::release(&rasterXMLStr);

		XMLCh *yes = XMLString::transcode("yes");

		XMLCh *meanXMLStr = XMLString::transcode("mean");
		if(argElement->hasAttribute(meanXMLStr))
		{
			const XMLCh *meanStr = argElement->getAttribute(meanXMLStr);
			if(XMLString::equals(meanStr, yes))
			{
				this->mean = true;
			}
			else
			{
				this->mean = false;
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'mean\' attribute was provided.");
		}
		XMLString::release(&meanXMLStr);


		XMLCh *minXMLStr = XMLString::transcode("min");
		if(argElement->hasAttribute(minXMLStr))
		{
			const XMLCh *minStr = argElement->getAttribute(minXMLStr);
			if(XMLString::equals(minStr, yes))
			{
				this->min = true;
			}
			else
			{
				this->min = false;
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'min\' attribute was provided.");
		}
		XMLString::release(&minXMLStr);


		XMLCh *maxXMLStr = XMLString::transcode("max");
		if(argElement->hasAttribute(maxXMLStr))
		{
			const XMLCh *maxStr = argElement->getAttribute(maxXMLStr);
			if(XMLString::equals(maxStr, yes))
			{
				this->max = true;
			}
			else
			{
				this->max = false;
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'max\' attribute was provided.");
		}
		XMLString::release(&maxXMLStr);

		XMLCh *stddevXMLStr = XMLString::transcode("stddev");
		if(argElement->hasAttribute(stddevXMLStr))
		{
			const XMLCh *stddevStr = argElement->getAttribute(stddevXMLStr);
			if(XMLString::equals(stddevStr, yes))
			{
				this->stddev = true;
			}
			else
			{
				this->stddev = false;
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'stddev\' attribute was provided.");
		}
		XMLString::release(&stddevXMLStr);

		XMLString::release(&yes);
	}
	else if(XMLString::equals(optionRasterPolygonsToTxt, optionXML))
	{
		this->option = RSGISExeZonalStats::rasterpolygons2txt;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputTextFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *rasterXMLStr = XMLString::transcode("raster");
		if(argElement->hasAttribute(rasterXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rasterXMLStr));
			this->inputRasPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'raster\' attribute was provided.");
		}
		XMLString::release(&rasterXMLStr);

		XMLCh *yes = XMLString::transcode("yes");

		XMLCh *meanXMLStr = XMLString::transcode("mean");
		if(argElement->hasAttribute(meanXMLStr))
		{
			const XMLCh *meanStr = argElement->getAttribute(meanXMLStr);
			if(XMLString::equals(meanStr, yes))
			{
				this->mean = true;
			}
			else
			{
				this->mean = false;
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'mean\' attribute was provided.");
		}
		XMLString::release(&meanXMLStr);


		XMLCh *minXMLStr = XMLString::transcode("min");
		if(argElement->hasAttribute(minXMLStr))
		{
			const XMLCh *minStr = argElement->getAttribute(minXMLStr);
			if(XMLString::equals(minStr, yes))
			{
				this->min = true;
			}
			else
			{
				this->min = false;
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'min\' attribute was provided.");
		}
		XMLString::release(&minXMLStr);


		XMLCh *maxXMLStr = XMLString::transcode("max");
		if(argElement->hasAttribute(maxXMLStr))
		{
			const XMLCh *maxStr = argElement->getAttribute(maxXMLStr);
			if(XMLString::equals(maxStr, yes))
			{
				this->max = true;
			}
			else
			{
				this->max = false;
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'max\' attribute was provided.");
		}
		XMLString::release(&maxXMLStr);

		XMLCh *stddevXMLStr = XMLString::transcode("stddev");
		if(argElement->hasAttribute(stddevXMLStr))
		{
			const XMLCh *stddevStr = argElement->getAttribute(stddevXMLStr);
			if(XMLString::equals(stddevStr, yes))
			{
				this->stddev = true;
			}
			else
			{
				this->stddev = false;
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'stddev\' attribute was provided.");
		}
		XMLString::release(&stddevXMLStr);

		XMLString::release(&yes);
	}
	else if(XMLString::equals(optionPixelVals2txt, optionXML))
	{
		this->option = RSGISExeZonalStats::pixelVals2txt;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputMatrix = string(charValue);
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
			this->polyAttribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'attribute\' attribute was provided.");
		}
		XMLString::release(&attributeXMLStr);

		// Get pixel in poly method
		XMLCh *methodXMLStr = XMLString::transcode("method");
		if(argElement->hasAttribute(methodXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(methodXMLStr));
			string methodStr = string(charValue);
			// Polygon completely contains pixel
			if(methodStr == "polyContainsPixel"){this->method = rsgis::img::polyContainsPixel;}
			// Pixel center is within the polygon
			else if(methodStr == "polyContainsPixelCenter") {this->method = rsgis::img::polyContainsPixelCenter;}
			// Polygon overlaps the pixel
			else if(methodStr == "polyOverlapsPixel"){this->method = rsgis::img::polyOverlapsPixel;}
			// Polygon overlaps or contains the pixel
			else if(methodStr == "polyOverlapsOrContainsPixel"){this->method = rsgis::img::polyOverlapsOrContainsPixel;}
			// Pixel contains the polygon
			else if(methodStr == "pixelContainsPoly"){this->method = rsgis::img::pixelContainsPoly;}
			// Polygon center is within pixel
			else if(methodStr == "pixelContainsPolyCenter"){this->method = rsgis::img::pixelContainsPolyCenter;}
			// The method is chosen based on relative areas of pixel and polygon.
			else if(methodStr == "adaptive"){this->method = rsgis::img::adaptive;}
			// Everything within the polygons envelope is chosen (for debugging)
			else if(methodStr == "envelope"){this->method = rsgis::img::envelope;}
			// Set to default value if not recognised.
			else {cout << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;this->method = rsgis::img::polyContainsPixelCenter;}
			XMLString::release(&charValue);
		}
		else
		{
			this->method = rsgis::img::polyContainsPixelCenter;
		}
		XMLString::release(&methodXMLStr);

		// Get output text format
		XMLCh *outTXTStr = XMLString::transcode("outTXT");
		if(argElement->hasAttribute(outTXTStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outTXTStr));
			string outStr = string(charValue);
			if(outStr == "mtxt"){cout << "\tsaving to mtxt\n";this->outtxt = "mtxt";}
			else if(outStr == "gtxt"){cout << "\tsaving to gtxt\n";this->outtxt = "gtxt";}
			else if(outStr == "csv"){cout << "\tsaving to csv\n";this->outtxt = "csv";}
			// Set to default value if not recognised.
			else {this->outtxt = csv;}
			XMLString::release(&charValue);
		}
		else
		{
			this->outtxt = csv;
		}
		XMLString::release(&outTXTStr);

	}
    else if(XMLString::equals(optionVariablesToMatrix, optionXML))
	{
		this->option = RSGISExeZonalStats::varibles2matrix;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputMatrix = string(charValue);
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
			this->polyAttribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'attribute\' attribute was provided.");
		}
		XMLString::release(&attributeXMLStr);

        XMLCh *methodXMLStr = XMLString::transcode("method");
		if(argElement->hasAttribute(methodXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(methodXMLStr));
			string methodStr = string(charValue);
			// Polygon completely contains pixel
			if(methodStr == "polyContainsPixel"){this->method = rsgis::img::polyContainsPixel;}
			// Pixel center is within the polygon
			else if(methodStr == "polyContainsPixelCenter") {this->method = rsgis::img::polyContainsPixelCenter;}
			// Polygon overlaps the pixel
			else if(methodStr == "polyOverlapsPixel"){this->method = rsgis::img::polyOverlapsPixel;}
			// Polygon overlaps or contains the pixel
			else if(methodStr == "polyOverlapsOrContainsPixel"){this->method = rsgis::img::polyOverlapsOrContainsPixel;}
			// Pixel contains the polygon
			else if(methodStr == "pixelContainsPoly"){this->method = rsgis::img::pixelContainsPoly;}
			// Polygon center is within pixel
			else if(methodStr == "pixelContainsPolyCenter"){this->method = rsgis::img::pixelContainsPolyCenter;}
			// The method is chosen based on relative areas of pixel and polygon.
			else if(methodStr == "adaptive"){this->method = rsgis::img::adaptive;}
			// Everything within the polygons envelope is chosen (for debugging)
			else if(methodStr == "envelope"){this->method = rsgis::img::envelope;}
			// Set to default value if not recognised.
			else {cerr << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;this->method = rsgis::img::polyContainsPixelCenter;}
			XMLString::release(&charValue);
		}
		else
		{
			cerr << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;
			this->method = rsgis::img::polyContainsPixelCenter;
		}
		XMLString::release(&methodXMLStr);
	}
	else if(XMLString::equals(optionImageToMatrix, optionXML))
	{
		this->option = RSGISExeZonalStats::image2matrix;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputMatrix = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *bandXMLStr = XMLString::transcode("band");
		if(argElement->hasAttribute(bandXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bandXMLStr));
			this->imageBand = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
		}
		XMLString::release(&bandXMLStr);

	}
	else if(XMLString::equals(optionPixelCount, optionXML))
	{
		this->option = RSGISExeZonalStats::pixelcount;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVecPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *rasterXMLStr = XMLString::transcode("raster");
		if(argElement->hasAttribute(rasterXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rasterXMLStr));
			this->inputRasPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'raster\' attribute was provided.");
		}
		XMLString::release(&rasterXMLStr);

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

		XMLCh *pxlcountXMLStr = XMLString::transcode("pxlcount");
		if(argElement->hasAttribute(pxlcountXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(pxlcountXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->pxlcount = true;
			}
			else
			{
				this->pxlcount = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'pxlcount\' attribute was provided.");
		}
		XMLString::release(&pxlcountXMLStr);



		XMLCh *emptysetXMLStr = XMLString::transcode("emptyset");
		if(argElement->hasAttribute(emptysetXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(emptysetXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->emptyset = true;
			}
			else
			{
				this->emptyset = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'emptyset\' attribute was provided.");
		}
		XMLString::release(&emptysetXMLStr);


		DOMNodeList *attributeNodesList = argElement->getElementsByTagName(rsgisattributeXMLStr);
		this->numAttributes = attributeNodesList->getLength();

		//cout << "Found " << this->numAttributes << " attributes" << endl;

		if(numAttributes > 0)
		{
			attributeCountList = new CountAttributes*[numAttributes];
			DOMElement *attributeElement = NULL;
			for(int i = 0; i < numAttributes; i++)
			{
				attributeCountList[i] = new CountAttributes();
				attributeElement = static_cast<DOMElement*>(attributeNodesList->item(i));

				XMLCh *nameXMLStr = XMLString::transcode("name");
				if(attributeElement->hasAttribute(nameXMLStr))
				{
					char *charValue = XMLString::transcode(attributeElement->getAttribute(nameXMLStr));
					attributeCountList[i]->name = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
				}
				XMLString::release(&nameXMLStr);

				DOMNodeList *bandNodesList = attributeElement->getElementsByTagName(rsgisbandXMLStr);
				attributeCountList[i]->numBands = bandNodesList->getLength();
				if(attributeCountList[i]->numBands > 0)
				{
					DOMElement *bandElement = NULL;
					attributeCountList[i]->bands = new int[attributeCountList[i]->numBands];
					attributeCountList[i]->thresholds = new float[attributeCountList[i]->numBands];
					for(int j = 0; j < attributeCountList[i]->numBands; j++)
					{
						bandElement = static_cast<DOMElement*>(bandNodesList->item(j));

						XMLCh *bandXMLStr = XMLString::transcode("band");
						if(bandElement->hasAttribute(bandXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
							attributeCountList[i]->bands[j] = mathUtils.strtoint(string(charValue)) - 1;
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
						}
						XMLString::release(&bandXMLStr);


						XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
						if(bandElement->hasAttribute(thresholdXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(thresholdXMLStr));
							attributeCountList[i]->thresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
						}
						XMLString::release(&thresholdXMLStr);
					}
				}
				else
				{
					throw RSGISXMLArgumentsException("No attributes \'rsgis:band\' tags were provided.");
				}

			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No attributes \'rsgis:attribute\' tags were provided.");
		}
	}
	else if(XMLString::equals(optionPixelStats, optionXML))
	{
		this->option = RSGISExeZonalStats::pixelstats;
		this->outputToText = false;
		this->outputTextFile = "";

		// Text file
		XMLCh *outputCSVXMLStr = XMLString::transcode("outputCSV");
		if(argElement->hasAttribute(outputCSVXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputCSVXMLStr));
			this->outputTextFile = string(charValue);
			XMLString::release(&charValue);
			this->outputToText = true;
		}
		XMLString::release(&outputCSVXMLStr);


		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			if(this->outputToText)
			{
				cout << "WARNING! Can't output to shapefile and CSV, ignoring shapefile." << endl;
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputVecPolys = string(charValue);
				XMLString::release(&charValue);
			}
		}
		else
		{
			if(!this->outputToText){throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");}
		}
		XMLString::release(&outputXMLStr);

		XMLCh *rasterXMLStr = XMLString::transcode("raster");
		if(argElement->hasAttribute(rasterXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rasterXMLStr));
			this->inputRasPolys = string(charValue);
			this->useRasPoly = true;
			XMLString::release(&charValue);
		}
		else
		{
			cout << "\tNot using rasterised version of polygon" << endl;
			this->useRasPoly = false;
		}
		XMLString::release(&rasterXMLStr);

		// Retrieve method for calculating pixels in polygon
		XMLCh *methodXMLStr = XMLString::transcode("method");
		if(argElement->hasAttribute(methodXMLStr))
		{
			if (useRasPoly)
			{
				cout << "\tUsing rasterised version of polygon - ignoring method" << endl;
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(methodXMLStr));
				string methodStr = string(charValue);
				// Polygon completely contains pixel
				if(methodStr == "polyContainsPixel"){this->method = rsgis::img::polyContainsPixel;}
				// Pixel center is within the polygon
				else if(methodStr == "polyContainsPixelCenter") {this->method = rsgis::img::polyContainsPixelCenter;}
				// Polygon overlaps the pixel
				else if(methodStr == "polyOverlapsPixel"){this->method = rsgis::img::polyOverlapsPixel;}
				// Polygon overlaps or contains the pixel
				else if(methodStr == "polyOverlapsOrContainsPixel"){this->method = rsgis::img::polyOverlapsOrContainsPixel;}
				// Pixel contains the polygon
				else if(methodStr == "pixelContainsPoly"){this->method = rsgis::img::pixelContainsPoly;}
				// Polygon center is within pixel
				else if(methodStr == "pixelContainsPolyCenter"){this->method = rsgis::img::pixelContainsPolyCenter;}
				// The method is chosen based on relative areas of pixel and polygon.
				else if(methodStr == "adaptive"){this->method = rsgis::img::adaptive;}
				// Everything within the polygons envelope is chosen (for debugging)
				else if(methodStr == "envelope"){this->method = rsgis::img::envelope;}
				// Set to default value if not recognised.
				else {cout << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;this->method = rsgis::img::polyContainsPixelCenter;}
				XMLString::release(&charValue);
			}
		}
		else
		{
			if (useRasPoly == false)
			{
				cout << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;
				this->method = rsgis::img::polyContainsPixelCenter;
			}
		}
		XMLString::release(&methodXMLStr);

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
			if(!this->outputToText){throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");}
		}
		XMLString::release(&forceXMLStr);

		XMLCh *copyAttributesXMLStr = XMLString::transcode("copyAttributes");
		if(argElement->hasAttribute(copyAttributesXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(copyAttributesXMLStr);

			if(XMLString::equals(forceValue, yesStr))
			{
				this->copyAttributes = true;
			}
			else
			{
				this->copyAttributes = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			this->copyAttributes = true;
		}
		XMLString::release(&copyAttributesXMLStr);

        shortenBandNames = true;
        XMLCh *shortenBandNamesXMLStr = XMLString::transcode("shortenBandNames");
		if(argElement->hasAttribute(shortenBandNamesXMLStr))
		{
			XMLCh *noStr = XMLString::transcode("no");
			const XMLCh *shortenValue = argElement->getAttribute(shortenBandNamesXMLStr);

			if(XMLString::equals(shortenValue, noStr))
			{
				this->shortenBandNames = false;
			}
			else
			{
				this->shortenBandNames = true;
			}
			XMLString::release(&noStr);
		}
		XMLString::release(&shortenBandNamesXMLStr);

		XMLCh *pxlcountXMLStr = XMLString::transcode("pxlcount");
		if(argElement->hasAttribute(pxlcountXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(pxlcountXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->pxlcount = true;
			}
			else
			{
				this->pxlcount = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			this->pxlcount = false;;
		}
		XMLString::release(&pxlcountXMLStr);

		/* Check for stats for all attributes
		 * It is possible to set for all attributes for individual
		 */
		// Set all values to default of false
		this->minAll = false;
		this->maxAll = false;
		this->meanAll = false;
		this->stdDevAll = false;
        this->modeAll = false;
        this->sumAll = false;
		this->countAll = false;
		this->minThreshAll = false;
		this->maxThreshAll = false;
		this->minThreshAllVal = -numeric_limits<double>::infinity();
		this->maxThreshAllVal = +numeric_limits<double>::infinity();

		XMLCh *minXMLStr = XMLString::transcode("min");
		if(argElement->hasAttribute(minXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(minXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->minAll = true;
			}
			XMLString::release(&yesStr);
		}
		XMLString::release(&minXMLStr);

		XMLCh *maxXMLStr = XMLString::transcode("max");
		if(argElement->hasAttribute(maxXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(maxXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->maxAll = true;
			}
			XMLString::release(&yesStr);
		}
		XMLString::release(&maxXMLStr);

		XMLCh *meanXMLStr = XMLString::transcode("mean");
		if(argElement->hasAttribute(meanXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(meanXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->meanAll = true;
			}
			XMLString::release(&yesStr);
		}
		XMLString::release(&meanXMLStr);

		XMLCh *stddevXMLStr = XMLString::transcode("stddev");
		if(argElement->hasAttribute(stddevXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(stddevXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->stdDevAll = true;
			}
			XMLString::release(&yesStr);
		}
		XMLString::release(&stddevXMLStr);

        XMLCh *modeXMLStr = XMLString::transcode("mode");
		if(argElement->hasAttribute(modeXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(modeXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->modeAll = true;
			}
			XMLString::release(&yesStr);
		}
		XMLString::release(&modeXMLStr);

        XMLCh *sumXMLStr = XMLString::transcode("sum");
		if(argElement->hasAttribute(sumXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(sumXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->sumAll = true;
			}
			XMLString::release(&yesStr);
		}
		XMLString::release(&sumXMLStr);

		XMLCh *countXMLStr = XMLString::transcode("count");
		if(argElement->hasAttribute(countXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(countXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->countAll = true;
			}
			XMLString::release(&yesStr);
		}
		XMLString::release(&countXMLStr);

		XMLCh *minThresholdXMLStr = XMLString::transcode("minThreshold");
		if(argElement->hasAttribute(minThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minThresholdXMLStr));
			minThreshAllVal= mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
			minThreshAll = true;
		}
		XMLString::release(&minThresholdXMLStr);

		XMLCh *maxThresholdXMLStr = XMLString::transcode("maxThreshold");
		if(argElement->hasAttribute(maxThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxThresholdXMLStr));
			maxThreshAllVal= mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
			maxThreshAll = true;
		}
		XMLString::release(&maxThresholdXMLStr);


		DOMNodeList *attributeNodesList = argElement->getElementsByTagName(rsgisattributeXMLStr);
		this->numAttributes = attributeNodesList->getLength();
		if ((this->numAttributes > 253) && (!this->outputToText))
		{
			throw RSGISXMLArgumentsException("Number of attributes exceeds limit for shapefile, output to text file using \'outputCSV\' instead.");
		}
		//cout << "Found " << this->numAttributes << " attributes" << endl;

		if(numAttributes > 0)
		{
			this->calcZonalAllBands = false;
			this->attributeZonalList = new ZonalAttributes*[numAttributes];
			DOMElement *attributeElement = NULL;
			for(int i = 0; i < numAttributes; i++)
			{
				attributeZonalList[i] = new ZonalAttributes();
				attributeElement = static_cast<DOMElement*>(attributeNodesList->item(i));

				XMLCh *nameXMLStr = XMLString::transcode("name");
				if(attributeElement->hasAttribute(nameXMLStr))
				{
					char *charValue = XMLString::transcode(attributeElement->getAttribute(nameXMLStr));
					attributeZonalList[i]->name = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
				}
				XMLString::release(&nameXMLStr);

				// Set statistics for each attribute to values for all attributes
				attributeZonalList[i]->outMin = minAll;
				attributeZonalList[i]->outMax = maxAll;
				attributeZonalList[i]->outMean = meanAll;
				attributeZonalList[i]->outStDev = stdDevAll;
				attributeZonalList[i]->outMode = modeAll;
                attributeZonalList[i]->outSum = sumAll;
				attributeZonalList[i]->outCount = countAll;


				// Get statistics to calculate - this overrides options set for all attributes

				XMLCh *minXMLStr = XMLString::transcode("min");
				if(attributeElement->hasAttribute(minXMLStr))
				{
					XMLCh *yesStr = XMLString::transcode("yes");
					const XMLCh *value = attributeElement->getAttribute(minXMLStr);

					if(XMLString::equals(value, yesStr))
					{
						attributeZonalList[i]->outMin = true;
					}
					else
					{
						attributeZonalList[i]->outMin = false;
					}
					XMLString::release(&yesStr);
				}
				XMLString::release(&minXMLStr);

				XMLCh *maxXMLStr = XMLString::transcode("max");
				if(attributeElement->hasAttribute(maxXMLStr))
				{
					XMLCh *yesStr = XMLString::transcode("yes");
					const XMLCh *value = attributeElement->getAttribute(maxXMLStr);

					if(XMLString::equals(value, yesStr))
					{
						attributeZonalList[i]->outMax = true;
					}
					else
					{
						attributeZonalList[i]->outMax = false;
					}
					XMLString::release(&yesStr);
				}
				XMLString::release(&maxXMLStr);

				XMLCh *meanXMLStr = XMLString::transcode("mean");
				if(attributeElement->hasAttribute(meanXMLStr))
				{
					XMLCh *yesStr = XMLString::transcode("yes");
					const XMLCh *value = attributeElement->getAttribute(meanXMLStr);

					if(XMLString::equals(value, yesStr))
					{
						attributeZonalList[i]->outMean = true;
					}
					else
					{
						attributeZonalList[i]->outMean = false;
					}
					XMLString::release(&yesStr);
				}
				XMLString::release(&meanXMLStr);

				XMLCh *stddevXMLStr = XMLString::transcode("stddev");
				if(attributeElement->hasAttribute(stddevXMLStr))
				{
					XMLCh *yesStr = XMLString::transcode("yes");
					const XMLCh *value = attributeElement->getAttribute(stddevXMLStr);

					if(XMLString::equals(value, yesStr))
					{
						attributeZonalList[i]->outStDev = true;
					}
					else
					{
						attributeZonalList[i]->outStDev = false;
					}
					XMLString::release(&yesStr);
				}
				XMLString::release(&stddevXMLStr);

                XMLCh *modeXMLStr = XMLString::transcode("mode");
				if(attributeElement->hasAttribute(modeXMLStr))
				{
					XMLCh *yesStr = XMLString::transcode("yes");
					const XMLCh *value = attributeElement->getAttribute(modeXMLStr);

					if(XMLString::equals(value, yesStr))
					{
						attributeZonalList[i]->outMode = true;
					}
					else
					{
						attributeZonalList[i]->outMode = false;
					}
					XMLString::release(&yesStr);
				}
				XMLString::release(&modeXMLStr);

                XMLCh *sumXMLStr = XMLString::transcode("sum");
				if(attributeElement->hasAttribute(sumXMLStr))
				{
					XMLCh *yesStr = XMLString::transcode("yes");
					const XMLCh *value = attributeElement->getAttribute(sumXMLStr);

					if(XMLString::equals(value, yesStr))
					{
						attributeZonalList[i]->outSum = true;
					}
					else
					{
						attributeZonalList[i]->outSum = false;
					}
					XMLString::release(&yesStr);
				}
				XMLString::release(&sumXMLStr);

				XMLCh *countXMLStr = XMLString::transcode("count");
				if(attributeElement->hasAttribute(countXMLStr))
				{
					XMLCh *yesStr = XMLString::transcode("yes");
					const XMLCh *value = attributeElement->getAttribute(countXMLStr);

					if(XMLString::equals(value, yesStr))
					{
						attributeZonalList[i]->outCount = true;
					}
					else
					{
						attributeZonalList[i]->outCount = false;
					}

					XMLString::release(&yesStr);
				}
				XMLString::release(&countXMLStr);


				DOMNodeList *bandNodesList = attributeElement->getElementsByTagName(rsgisbandXMLStr);
				attributeZonalList[i]->numBands = bandNodesList->getLength();
				if(attributeZonalList[i]->numBands > 0)
				{
					DOMElement *bandElement = NULL;
					attributeZonalList[i]->bands = new int[attributeZonalList[i]->numBands];
					attributeZonalList[i]->minThresholds = new float[attributeZonalList[i]->numBands];
					attributeZonalList[i]->maxThresholds = new float[attributeZonalList[i]->numBands];

					for(int j = 0; j < attributeZonalList[i]->numBands; j++)
					{
						bandElement = static_cast<DOMElement*>(bandNodesList->item(j));


						XMLCh *bandXMLStr = XMLString::transcode("band");
						if(bandElement->hasAttribute(bandXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
							attributeZonalList[i]->bands[j] = mathUtils.strtoint(string(charValue)) - 1;
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
						}
						XMLString::release(&bandXMLStr);


						XMLCh *minThresholdXMLStr = XMLString::transcode("minThreshold");
						if(bandElement->hasAttribute(minThresholdXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(minThresholdXMLStr));
							attributeZonalList[i]->minThresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else if(minThreshAll)
						{
							attributeZonalList[i]->minThresholds[j] = minThreshAllVal;
						}
						else
						{
							attributeZonalList[i]->minThresholds[j] = -numeric_limits<double>::infinity();
						}

						XMLString::release(&minThresholdXMLStr);

						XMLCh *maxThresholdXMLStr = XMLString::transcode("maxThreshold");
						if(bandElement->hasAttribute(maxThresholdXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(maxThresholdXMLStr));
							attributeZonalList[i]->maxThresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else if(maxThreshAll)
						{
							attributeZonalList[i]->maxThresholds[j] = maxThreshAllVal;
						}
						else
						{
							attributeZonalList[i]->maxThresholds[j] = +numeric_limits<double>::infinity();
						}
						XMLString::release(&maxThresholdXMLStr);
					}
				}
				else
				{
					throw RSGISXMLArgumentsException("No attributes \'rsgis:band\' tags were provided.");
				}

			}
		}
		else
		{
			cout << "\tCalculating stats for all bands (default names b1, b2 etc.)" << endl;
			this->calcZonalAllBands = true;
		}
	}
	else if(XMLString::equals(optionPixelMean, optionXML))
	{
		this->option = RSGISExeZonalStats::pixelmean;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVecPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *rasterXMLStr = XMLString::transcode("raster");
		if(argElement->hasAttribute(rasterXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rasterXMLStr));
			this->inputRasPolys = string(charValue);
			this->useRasPoly = true;
			XMLString::release(&charValue);
		}
		else
		{
			cout << "\tNot using rasterised version of polygon" << endl;
			this->useRasPoly = false;
		}
		XMLString::release(&rasterXMLStr);

		// Retrieve method for calculating pixels in polygon
		XMLCh *methodXMLStr = XMLString::transcode("method");
		if(argElement->hasAttribute(methodXMLStr))
		{
			if (useRasPoly)
			{
				cout << "\tUsing rasterised version of polygon - ignoring method" << endl;
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(methodXMLStr));
				string methodStr = string(charValue);
				// Polygon completely contains pixel
				if(methodStr == "polyContainsPixel"){this->method = rsgis::img::polyContainsPixel;}
				// Pixel center is within the polygon
				else if(methodStr == "polyContainsPixelCenter") {this->method = rsgis::img::polyContainsPixelCenter;}
				// Polygon overlaps the pixel
				else if(methodStr == "polyOverlapsPixel"){this->method = rsgis::img::polyOverlapsPixel;}
				// Polygon overlaps or contains the pixel
				else if(methodStr == "polyOverlapsOrContainsPixel"){this->method = rsgis::img::polyOverlapsOrContainsPixel;}
				// Pixel contains the polygon
				else if(methodStr == "pixelContainsPoly"){this->method = rsgis::img::pixelContainsPoly;}
				// Polygon center is within pixel
				else if(methodStr == "pixelContainsPolyCenter"){this->method = rsgis::img::pixelContainsPolyCenter;}
				// The method is chosen based on relative areas of pixel and polygon.
				else if(methodStr == "adaptive"){this->method = rsgis::img::adaptive;}
				// Everything within the polygons envelope is chosen (for debugging)
				else if(methodStr == "envelope"){this->method = rsgis::img::envelope;}
				// Set to default value if not recognised.
				else {cout << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;this->method = rsgis::img::polyContainsPixelCenter;}
				XMLString::release(&charValue);
			}
		}
		else
		{
			if (useRasPoly == false)
			{
				cout << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;
				this->method = rsgis::img::polyContainsPixelCenter;
			}
		}
		XMLString::release(&methodXMLStr);

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

		XMLCh *dBXMLStr = XMLString::transcode("dB");
		if(argElement->hasAttribute(dBXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *dbValue = argElement->getAttribute(dBXMLStr);

			if(XMLString::equals(dbValue, yesStr))
			{
				this->dB = true;
				cout << "\tInput data in dB" << endl;
				this->useRasPoly = false;
			}
			else
			{
				this->dB = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			this->dB = false;
		}
		XMLString::release(&dBXMLStr);

		XMLCh *pxlcountXMLStr = XMLString::transcode("pxlcount");
		if(argElement->hasAttribute(pxlcountXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(pxlcountXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->pxlcount = true;
			}
			else
			{
				this->pxlcount = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'pxlcount\' attribute was provided.");
		}
		XMLString::release(&pxlcountXMLStr);

		// Set values for minimum and maximum for all attributes
		this->minThreshAll = false;
		this->maxThreshAll = false;
		this->minThreshAllVal = -numeric_limits<double>::infinity();
		this->maxThreshAllVal = +numeric_limits<double>::infinity();

		XMLCh *minThresholdXMLStr = XMLString::transcode("minThreshold");
		if(argElement->hasAttribute(minThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minThresholdXMLStr));
			minThreshAllVal= mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
			minThreshAll = true;
		}
		XMLString::release(&minThresholdXMLStr);

		XMLCh *maxThresholdXMLStr = XMLString::transcode("maxThreshold");
		if(argElement->hasAttribute(maxThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxThresholdXMLStr));
			maxThreshAllVal= mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
			maxThreshAll = true;
		}
		XMLString::release(&maxThresholdXMLStr);

		DOMNodeList *attributeNodesList = argElement->getElementsByTagName(rsgisattributeXMLStr);
		this->numAttributes = attributeNodesList->getLength();

		//cout << "Found " << this->numAttributes << " attributes" << endl;

		if(numAttributes > 0)
		{
			this->attributeMeanList = new MeanAttributes*[numAttributes];
			DOMElement *attributeElement = NULL;
			for(int i = 0; i < numAttributes; i++)
			{
				attributeMeanList[i] = new MeanAttributes();
				attributeElement = static_cast<DOMElement*>(attributeNodesList->item(i));

				XMLCh *nameXMLStr = XMLString::transcode("name");
				if(attributeElement->hasAttribute(nameXMLStr))
				{
					char *charValue = XMLString::transcode(attributeElement->getAttribute(nameXMLStr));
					attributeMeanList[i]->name = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
				}
				XMLString::release(&nameXMLStr);

				DOMNodeList *bandNodesList = attributeElement->getElementsByTagName(rsgisbandXMLStr);
				attributeMeanList[i]->numBands = bandNodesList->getLength();
				if(attributeMeanList[i]->numBands > 0)
				{
					DOMElement *bandElement = NULL;
					attributeMeanList[i]->bands = new int[attributeMeanList[i]->numBands];
					attributeMeanList[i]->minThresholds = new float[attributeMeanList[i]->numBands];
					attributeMeanList[i]->maxThresholds = new float[attributeMeanList[i]->numBands];
					for(int j = 0; j < attributeMeanList[i]->numBands; j++)
					{
						bandElement = static_cast<DOMElement*>(bandNodesList->item(j));

						XMLCh *bandXMLStr = XMLString::transcode("band");
						if(bandElement->hasAttribute(bandXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
							int band = mathUtils.strtoint(string(charValue));
							if(band == 0)
							{
								throw RSGISXMLArgumentsException("Band numbering starts at 1");
							}
							else
							{
								attributeMeanList[i]->bands[j] = band - 1;
							}
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
						}
						XMLString::release(&bandXMLStr);


						XMLCh *minThresholdXMLStr = XMLString::transcode("minThreshold");
						if(bandElement->hasAttribute(minThresholdXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(minThresholdXMLStr));
							attributeMeanList[i]->minThresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else if(minThreshAll)
						{
							attributeMeanList[i]->minThresholds[j] = minThreshAllVal;
						}
						else
						{
							attributeMeanList[i]->minThresholds[j] = -numeric_limits<double>::infinity();
						}

						XMLString::release(&minThresholdXMLStr);

						XMLCh *thresholdXMLStr = XMLString::transcode("threshold"); // Old version of minimum threashold, used to retain compatibility
						if(bandElement->hasAttribute(thresholdXMLStr))
						{
							cout << "WARNING: \"threashold\" is no longer been used, please update scripts to use \"minThreshold\" instead" << endl;
							char *charValue = XMLString::transcode(bandElement->getAttribute(thresholdXMLStr));
							attributeMeanList[i]->minThresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						XMLString::release(&thresholdXMLStr);

						XMLCh *maxThresholdXMLStr = XMLString::transcode("maxThreshold");
						if(bandElement->hasAttribute(maxThresholdXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(maxThresholdXMLStr));
							attributeMeanList[i]->maxThresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else if(maxThreshAll)
						{
							attributeMeanList[i]->maxThresholds[j] = maxThreshAllVal;
						}
						else
						{
							attributeMeanList[i]->maxThresholds[j] = +numeric_limits<double>::infinity();
						}
						XMLString::release(&maxThresholdXMLStr);
					}
				}
				else
				{
					throw RSGISXMLArgumentsException("No attributes \'rsgis:band\' tags were provided.");
				}

			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No attributes \'rsgis:attribute\' tags were provided.");
		}
	}
	else if(XMLString::equals(optionPixelMeanLSSVar, optionXML))
	{
		this->option = RSGISExeZonalStats::pixelmeanLSSVar;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVecPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		// Retrieve method for calculating pixels in polygon
		XMLCh *methodXMLStr = XMLString::transcode("method");
		if(argElement->hasAttribute(methodXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(methodXMLStr));
			string methodStr = string(charValue);
			// Polygon completely contains pixel
			if(methodStr == "polyContainsPixel"){this->method = rsgis::img::polyContainsPixel;}
			// Pixel center is within the polygon
			else if(methodStr == "polyContainsPixelCenter") {this->method = rsgis::img::polyContainsPixelCenter;}
			// Polygon overlaps the pixel
			else if(methodStr == "polyOverlapsPixel"){this->method = rsgis::img::polyOverlapsPixel;}
			// Polygon overlaps or contains the pixel
			else if(methodStr == "polyOverlapsOrContainsPixel"){this->method = rsgis::img::polyOverlapsOrContainsPixel;}
			// Pixel contains the polygon
			else if(methodStr == "pixelContainsPoly"){this->method = rsgis::img::pixelContainsPoly;}
			// Polygon center is within pixel
			else if(methodStr == "pixelContainsPolyCenter"){this->method = rsgis::img::pixelContainsPolyCenter;}
			// The method is chosen based on relative areas of pixel and polygon.
			else if(methodStr == "adaptive"){this->method = rsgis::img::adaptive;}
			// Everything within the polygons envelope is chosen (for debugging)
			else if(methodStr == "envelope"){this->method = rsgis::img::envelope;}
			// Set to default value if not recognised.
			else {cout << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;this->method = rsgis::img::polyContainsPixelCenter;}
			XMLString::release(&charValue);
		}
		else
		{
			cerr << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;
			this->method = rsgis::img::polyContainsPixelCenter;
		}
		XMLString::release(&methodXMLStr);

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

		XMLCh *windowSizeXMLStr = XMLString::transcode("windowSize"); // Get window size
		if(argElement->hasAttribute(windowSizeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(windowSizeXMLStr));
			this->windowSize = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			cout << "\tNo value provided for 'windowSize', using default of 3 x 3" << endl;
			this->windowSize = 3;
		}
		XMLString::release(&windowSizeXMLStr);

		XMLCh *offsetSizeXMLStr = XMLString::transcode("offsetSize");
		if(argElement->hasAttribute(offsetSizeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(offsetSizeXMLStr));
			this->offsetSize = mathUtils.strtodouble(string(charValue));
			this->usePixelSize = false;
			XMLString::release(&charValue);
		}
		else
		{
			this->usePixelSize = true;
		}
		XMLString::release(&offsetSizeXMLStr);

		XMLCh *pxlcountXMLStr = XMLString::transcode("pxlcount");
		if(argElement->hasAttribute(pxlcountXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(pxlcountXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->pxlcount = true;
			}
			else
			{
				this->pxlcount = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'pxlcount\' attribute was provided.");
		}
		XMLString::release(&pxlcountXMLStr);

		// Set values for minimum and maximum for all attributes
		bool minThreshAll = false;
		bool maxThreshAll = false;
		double minThreshAllVal = -numeric_limits<double>::infinity();
		double maxThreshAllVal = +numeric_limits<double>::infinity();

		XMLCh *minThresholdXMLStr = XMLString::transcode("minThreshold");
		if(argElement->hasAttribute(minThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minThresholdXMLStr));
			minThreshAllVal= mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
			minThreshAll = true;
		}
		XMLString::release(&minThresholdXMLStr);

		XMLCh *maxThresholdXMLStr = XMLString::transcode("maxThreshold");
		if(argElement->hasAttribute(maxThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxThresholdXMLStr));
			maxThreshAllVal= mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
			maxThreshAll = true;
		}
		XMLString::release(&maxThresholdXMLStr);

		DOMNodeList *attributeNodesList = argElement->getElementsByTagName(rsgisattributeXMLStr);
		this->numAttributes = attributeNodesList->getLength();

		//cout << "Found " << this->numAttributes << " attributes" << endl;

		if(numAttributes > 0)
		{
			this->attributeMeanList = new MeanAttributes*[numAttributes];
			DOMElement *attributeElement = NULL;
			for(int i = 0; i < numAttributes; i++)
			{
				attributeMeanList[i] = new MeanAttributes();
				attributeElement = static_cast<DOMElement*>(attributeNodesList->item(i));

				XMLCh *nameXMLStr = XMLString::transcode("name");
				if(attributeElement->hasAttribute(nameXMLStr))
				{
					char *charValue = XMLString::transcode(attributeElement->getAttribute(nameXMLStr));
					attributeMeanList[i]->name = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
				}
				XMLString::release(&nameXMLStr);

				DOMNodeList *bandNodesList = attributeElement->getElementsByTagName(rsgisbandXMLStr);
				attributeMeanList[i]->numBands = bandNodesList->getLength();
				if(attributeMeanList[i]->numBands > 0)
				{
					DOMElement *bandElement = NULL;
					attributeMeanList[i]->bands = new int[attributeMeanList[i]->numBands];
					attributeMeanList[i]->minThresholds = new float[attributeMeanList[i]->numBands];
					attributeMeanList[i]->minThresholds = new float[attributeMeanList[i]->numBands];
					for(int j = 0; j < attributeMeanList[i]->numBands; j++)
					{
						bandElement = static_cast<DOMElement*>(bandNodesList->item(j));


						XMLCh *bandXMLStr = XMLString::transcode("band");
						if(bandElement->hasAttribute(bandXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
							int band = mathUtils.strtoint(string(charValue));
							if(band == 0)
							{
								throw RSGISXMLArgumentsException("Band numbering starts at 1");
							}
							else
							{
								attributeMeanList[i]->bands[j] = band - 1;
							}
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
						}
						XMLString::release(&bandXMLStr);

						XMLCh *minThresholdXMLStr = XMLString::transcode("minThreshold");
						if(bandElement->hasAttribute(minThresholdXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(minThresholdXMLStr));
							attributeMeanList[i]->minThresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else if(minThreshAll)
						{
							attributeMeanList[i]->minThresholds[j] = minThreshAllVal;
						}
						else
						{
							attributeMeanList[i]->minThresholds[j] = -numeric_limits<double>::infinity();
						}

						XMLString::release(&minThresholdXMLStr);

						XMLCh *thresholdXMLStr = XMLString::transcode("threshold"); // Old version of minimum threashold, used to retain compatibility
						if(bandElement->hasAttribute(thresholdXMLStr))
						{
							cout << "WARNING: \"threashold\" is no longer been used, please update scripts to use \"minThreshold\" instead" << endl;
							char *charValue = XMLString::transcode(bandElement->getAttribute(thresholdXMLStr));
							attributeMeanList[i]->minThresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						XMLString::release(&thresholdXMLStr);

						XMLCh *maxThresholdXMLStr = XMLString::transcode("maxThreshold");
						if(bandElement->hasAttribute(maxThresholdXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(maxThresholdXMLStr));
							attributeMeanList[i]->maxThresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else if(maxThreshAll)
						{
							attributeMeanList[i]->maxThresholds[j] = maxThreshAllVal;
						}
						else
						{
							attributeMeanList[i]->maxThresholds[j] = +numeric_limits<double>::infinity();
						}
						XMLString::release(&maxThresholdXMLStr);
					}
				}
				else
				{
					throw RSGISXMLArgumentsException("No attributes \'rsgis:band\' tags were provided.");
				}

			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No attributes \'rsgis:attribute\' tags were provided.");
		}
	}
	else if(XMLString::equals(optionPixelWeightedMean, optionXML))
	{
		this->option = RSGISExeZonalStats::pixelWeightedMean;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVecPolys = string(charValue);
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

		XMLCh *pxlcountXMLStr = XMLString::transcode("pxlcount");
		if(argElement->hasAttribute(pxlcountXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(pxlcountXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->pxlcount = true;
			}
			else
			{
				this->pxlcount = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'pxlcount\' attribute was provided.");
		}
		XMLString::release(&pxlcountXMLStr);

		// Set values for minimum and maximum for all attributes
		this->minThreshAll = false;
		this->maxThreshAll = false;
		this->minThreshAllVal = -numeric_limits<double>::infinity();
		this->maxThreshAllVal = +numeric_limits<double>::infinity();

		XMLCh *minThresholdXMLStr = XMLString::transcode("minThreshold");
		if(argElement->hasAttribute(minThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(minThresholdXMLStr));
			minThreshAllVal= mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
			minThreshAll = true;
		}
		XMLString::release(&minThresholdXMLStr);

		XMLCh *maxThresholdXMLStr = XMLString::transcode("maxThreshold");
		if(argElement->hasAttribute(maxThresholdXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maxThresholdXMLStr));
			maxThreshAllVal= mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
			maxThreshAll = true;
		}
		XMLString::release(&maxThresholdXMLStr);

		DOMNodeList *attributeNodesList = argElement->getElementsByTagName(rsgisattributeXMLStr);
		this->numAttributes = attributeNodesList->getLength();

		//cout << "Found " << this->numAttributes << " attributes" << endl;

		if(numAttributes > 0)
		{
			this->attributeMeanList = new MeanAttributes*[numAttributes];
			DOMElement *attributeElement = NULL;
			for(int i = 0; i < numAttributes; i++)
			{
				attributeMeanList[i] = new MeanAttributes();
				attributeElement = static_cast<DOMElement*>(attributeNodesList->item(i));

				XMLCh *nameXMLStr = XMLString::transcode("name");
				if(attributeElement->hasAttribute(nameXMLStr))
				{
					char *charValue = XMLString::transcode(attributeElement->getAttribute(nameXMLStr));
					attributeMeanList[i]->name = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
				}
				XMLString::release(&nameXMLStr);

				DOMNodeList *bandNodesList = attributeElement->getElementsByTagName(rsgisbandXMLStr);
				attributeMeanList[i]->numBands = bandNodesList->getLength();
				if(attributeMeanList[i]->numBands > 0)
				{
					DOMElement *bandElement = NULL;
					attributeMeanList[i]->bands = new int[attributeMeanList[i]->numBands];
					attributeMeanList[i]->minThresholds = new float[attributeMeanList[i]->numBands];
					attributeMeanList[i]->maxThresholds = new float[attributeMeanList[i]->numBands];
					for(int j = 0; j < attributeMeanList[i]->numBands; j++)
					{
						bandElement = static_cast<DOMElement*>(bandNodesList->item(j));

						XMLCh *bandXMLStr = XMLString::transcode("band");
						if(bandElement->hasAttribute(bandXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
							int band = mathUtils.strtoint(string(charValue));
							if(band == 0)
							{
								throw RSGISXMLArgumentsException("Band numbering starts at 1");
							}
							else
							{
								attributeMeanList[i]->bands[j] = band - 1;
							}
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
						}
						XMLString::release(&bandXMLStr);


						XMLCh *minThresholdXMLStr = XMLString::transcode("minThreshold");
						if(bandElement->hasAttribute(minThresholdXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(minThresholdXMLStr));
							attributeMeanList[i]->minThresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else if(minThreshAll)
						{
							attributeMeanList[i]->minThresholds[j] = minThreshAllVal;
						}
						else
						{
							attributeMeanList[i]->minThresholds[j] = -numeric_limits<double>::infinity();
						}

						XMLString::release(&minThresholdXMLStr);

						XMLCh *thresholdXMLStr = XMLString::transcode("threshold"); // Old version of minimum threashold, used to retain compatibility
						if(bandElement->hasAttribute(thresholdXMLStr))
						{
							cout << "WARNING: \"threashold\" is no longer been used, please update scripts to use \"minThreshold\" instead" << endl;
							char *charValue = XMLString::transcode(bandElement->getAttribute(thresholdXMLStr));
							attributeMeanList[i]->minThresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						XMLString::release(&thresholdXMLStr);

						XMLCh *maxThresholdXMLStr = XMLString::transcode("maxThreshold");
						if(bandElement->hasAttribute(maxThresholdXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(maxThresholdXMLStr));
							attributeMeanList[i]->maxThresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else if(maxThreshAll)
						{
							attributeMeanList[i]->maxThresholds[j] = maxThreshAllVal;
						}
						else
						{
							attributeMeanList[i]->maxThresholds[j] = +numeric_limits<double>::infinity();
						}
						XMLString::release(&maxThresholdXMLStr);
					}
				}
				else
				{
					throw RSGISXMLArgumentsException("No attributes \'rsgis:band\' tags were provided.");
				}

			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No attributes \'rsgis:attribute\' tags were provided.");
		}
	}
	else if(XMLString::equals(optionFuzzy, optionXML))
	{
		this->option = RSGISExeZonalStats::fuzzy;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVecPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *rasterXMLStr = XMLString::transcode("raster");
		if(argElement->hasAttribute(rasterXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rasterXMLStr));
			this->inputRasPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'raster\' attribute was provided.");
		}
		XMLString::release(&rasterXMLStr);

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

		XMLCh *pxlcountXMLStr = XMLString::transcode("pxlcount");
		if(argElement->hasAttribute(pxlcountXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *value = argElement->getAttribute(pxlcountXMLStr);

			if(XMLString::equals(value, yesStr))
			{
				this->pxlcount = true;
			}
			else
			{
				this->pxlcount = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'pxlcount\' attribute was provided.");
		}
		XMLString::release(&pxlcountXMLStr);

		XMLCh *classattributeXMLStr = XMLString::transcode("classattribute");
		if(argElement->hasAttribute(classattributeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(classattributeXMLStr));
			this->classAttribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'classattribute\' attribute was provided.");
		}
		XMLString::release(&classattributeXMLStr);


		XMLCh *binsizeXMLStr = XMLString::transcode("binsize");
		if(argElement->hasAttribute(binsizeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(binsizeXMLStr));
			this->binsize = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'binsize\' attribute was provided.");
		}
		XMLString::release(&binsizeXMLStr);


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

		DOMNodeList *attributeNodesList = argElement->getElementsByTagName(rsgisattributeXMLStr);
		this->numAttributes = attributeNodesList->getLength();

		//cout << "Found " << this->numAttributes << " attributes" << endl;

		if(numAttributes > 0)
		{
			attributeFuzzyList = new FuzzyAttributes*[numAttributes];
			DOMElement *attributeElement = NULL;
			for(int i = 0; i < numAttributes; i++)
			{
				attributeFuzzyList[i] = new FuzzyAttributes();
				attributeElement = static_cast<DOMElement*>(attributeNodesList->item(i));

				attributeFuzzyList[i]->index = i;

				XMLCh *nameXMLStr = XMLString::transcode("name");
				if(attributeElement->hasAttribute(nameXMLStr))
				{
					char *charValue = XMLString::transcode(attributeElement->getAttribute(nameXMLStr));
					attributeFuzzyList[i]->name = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
				}
				XMLString::release(&nameXMLStr);


				XMLCh *classXMLStr = XMLString::transcode("class");
				if(attributeElement->hasAttribute(classXMLStr))
				{
					char *charValue = XMLString::transcode(attributeElement->getAttribute(classXMLStr));
					attributeFuzzyList[i]->fuzzyClass = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'class\' attribute was provided.");
				}
				XMLString::release(&classXMLStr);

				DOMNodeList *bandNodesList = attributeElement->getElementsByTagName(rsgisbandXMLStr);
				attributeFuzzyList[i]->numBands = bandNodesList->getLength();
				if(attributeFuzzyList[i]->numBands > 0)
				{
					DOMElement *bandElement = NULL;
					attributeFuzzyList[i]->bands = new int[attributeFuzzyList[i]->numBands];
					for(int j = 0; j < attributeFuzzyList[i]->numBands; j++)
					{
						bandElement = static_cast<DOMElement*>(bandNodesList->item(j));

						XMLCh *bandXMLStr = XMLString::transcode("band");
						if(bandElement->hasAttribute(bandXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
							attributeFuzzyList[i]->bands[j] = mathUtils.strtoint(string(charValue)) - 1;
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
						}
						XMLString::release(&bandXMLStr);

					}
				}
				else
				{
					throw RSGISXMLArgumentsException("No attributes \'rsgis:band\' tags were provided.");
				}
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No attributes \'rsgis:attribute\' tags were provided.");
		}
	}
	else if(XMLString::equals(optionPowersetCount, optionXML))
	{
		this->option = RSGISExeZonalStats::powersetcount;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputVecPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *rasterXMLStr = XMLString::transcode("raster");
		if(argElement->hasAttribute(rasterXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(rasterXMLStr));
			this->inputRasPolys = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'raster\' attribute was provided.");
		}
		XMLString::release(&rasterXMLStr);

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

		XMLCh *nTopXMLStr = XMLString::transcode("nTop");
		if(argElement->hasAttribute(nTopXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(nTopXMLStr));
			this->nTop = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'nTop\' attribute was provided.");
		}
		XMLString::release(&nTopXMLStr);


		DOMNodeList *attributeNodesList = argElement->getElementsByTagName(rsgisattributeXMLStr);
		this->numAttributes = attributeNodesList->getLength();

		//cout << "Found " << this->numAttributes << " attributes" << endl;

		if(numAttributes > 0)
		{
			attributeCountList = new CountAttributes*[numAttributes];
			DOMElement *attributeElement = NULL;
			for(int i = 0; i < numAttributes; i++)
			{
				attributeCountList[i] = new CountAttributes();
				attributeElement = static_cast<DOMElement*>(attributeNodesList->item(i));

				XMLCh *nameXMLStr = XMLString::transcode("name");
				if(attributeElement->hasAttribute(nameXMLStr))
				{
					char *charValue = XMLString::transcode(attributeElement->getAttribute(nameXMLStr));
					attributeCountList[i]->name = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
				}
				XMLString::release(&nameXMLStr);

				DOMNodeList *bandNodesList = attributeElement->getElementsByTagName(rsgisbandXMLStr);
				attributeCountList[i]->numBands = bandNodesList->getLength();
				if(attributeCountList[i]->numBands > 0)
				{
					DOMElement *bandElement = NULL;
					attributeCountList[i]->bands = new int[attributeCountList[i]->numBands];
					attributeCountList[i]->thresholds = new float[attributeCountList[i]->numBands];
					for(int j = 0; j < attributeCountList[i]->numBands; j++)
					{
						bandElement = static_cast<DOMElement*>(bandNodesList->item(j));

						XMLCh *bandXMLStr = XMLString::transcode("band");
						if(bandElement->hasAttribute(bandXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(bandXMLStr));
							attributeCountList[i]->bands[j] = mathUtils.strtoint(string(charValue)) - 1;
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
						}
						XMLString::release(&bandXMLStr);


						XMLCh *thresholdXMLStr = XMLString::transcode("threshold");
						if(bandElement->hasAttribute(thresholdXMLStr))
						{
							char *charValue = XMLString::transcode(bandElement->getAttribute(thresholdXMLStr));
							attributeCountList[i]->thresholds[j] = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'threshold\' attribute was provided.");
						}
						XMLString::release(&thresholdXMLStr);
					}
				}
				else
				{
					throw RSGISXMLArgumentsException("No attributes \'rsgis:band\' tags were provided.");
				}

			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No attributes \'rsgis:attribute\' tags were provided.");
		}
	}
	else if(XMLString::equals(optionPointValue, optionXML))
	{
		this->option = RSGISExeZonalStats::pointvalue;

		// Text file
		XMLCh *outputCSVXMLStr = XMLString::transcode("outputCSV");
		if(argElement->hasAttribute(outputCSVXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputCSVXMLStr));
			this->outputTextFile = string(charValue);
			XMLString::release(&charValue);
			this->outputToText = true;
		}
		XMLString::release(&outputCSVXMLStr);


		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			if(this->outputToText)
			{
				cout << "WARNING! Can't output to shapefile and CSV, ignoring shapefile." << endl;
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputVecPolys = string(charValue);
				XMLString::release(&charValue);
			}
		}
		else
		{
			if(!this->outputToText){throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");}
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
            if(!this->outputToText){throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");}
		}
		XMLString::release(&forceXMLStr);

        shortenBandNames = true;
        XMLCh *shortenBandNamesXMLStr = XMLString::transcode("shortenBandNames");
		if(argElement->hasAttribute(shortenBandNamesXMLStr))
		{
			XMLCh *noStr = XMLString::transcode("no");
			const XMLCh *shortenValue = argElement->getAttribute(shortenBandNamesXMLStr);

			if(XMLString::equals(shortenValue, noStr))
			{
				this->shortenBandNames = false;
			}
			else
			{
				this->shortenBandNames = true;
			}
			XMLString::release(&noStr);
		}
		XMLString::release(&shortenBandNamesXMLStr);

	}
    else if(XMLString::equals(optionEndmembers, optionXML))
	{
		this->option = RSGISExeZonalStats::endmembers;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputMatrix = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

        XMLCh *methodXMLStr = XMLString::transcode("method");
		if(argElement->hasAttribute(methodXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(methodXMLStr));
			string methodStr = string(charValue);
			// Polygon completely contains pixel
			if(methodStr == "polyContainsPixel"){this->method = rsgis::img::polyContainsPixel;}
			// Pixel center is within the polygon
			else if(methodStr == "polyContainsPixelCenter") {this->method = rsgis::img::polyContainsPixelCenter;}
			// Polygon overlaps the pixel
			else if(methodStr == "polyOverlapsPixel"){this->method = rsgis::img::polyOverlapsPixel;}
			// Polygon overlaps or contains the pixel
			else if(methodStr == "polyOverlapsOrContainsPixel"){this->method = rsgis::img::polyOverlapsOrContainsPixel;}
			// Pixel contains the polygon
			else if(methodStr == "pixelContainsPoly"){this->method = rsgis::img::pixelContainsPoly;}
			// Polygon center is within pixel
			else if(methodStr == "pixelContainsPolyCenter"){this->method = rsgis::img::pixelContainsPolyCenter;}
			// The method is chosen based on relative areas of pixel and polygon.
			else if(methodStr == "adaptive"){this->method = rsgis::img::adaptive;}
			// Everything within the polygons envelope is chosen (for debugging)
			else if(methodStr == "envelope"){this->method = rsgis::img::envelope;}
			// Set to default value if not recognised.
			else {cerr << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;this->method = rsgis::img::polyContainsPixelCenter;}
			XMLString::release(&charValue);
		}
		else
		{
			cerr << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;
			this->method = rsgis::img::polyContainsPixelCenter;
		}
		XMLString::release(&methodXMLStr);
	}
    else if(XMLString::equals(optionImageZone2hdf, optionXML))
	{
		this->option = RSGISExeZonalStats::imagezone2hdf;

		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

        XMLCh *methodXMLStr = XMLString::transcode("method");
		if(argElement->hasAttribute(methodXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(methodXMLStr));
			string methodStr = string(charValue);
			// Polygon completely contains pixel
			if(methodStr == "polyContainsPixel"){this->method = rsgis::img::polyContainsPixel;}
			// Pixel center is within the polygon
			else if(methodStr == "polyContainsPixelCenter") {this->method = rsgis::img::polyContainsPixelCenter;}
			// Polygon overlaps the pixel
			else if(methodStr == "polyOverlapsPixel"){this->method = rsgis::img::polyOverlapsPixel;}
			// Polygon overlaps or contains the pixel
			else if(methodStr == "polyOverlapsOrContainsPixel"){this->method = rsgis::img::polyOverlapsOrContainsPixel;}
			// Pixel contains the polygon
			else if(methodStr == "pixelContainsPoly"){this->method = rsgis::img::pixelContainsPoly;}
			// Polygon center is within pixel
			else if(methodStr == "pixelContainsPolyCenter"){this->method = rsgis::img::pixelContainsPolyCenter;}
			// The method is chosen based on relative areas of pixel and polygon.
			else if(methodStr == "adaptive"){this->method = rsgis::img::adaptive;}
			// Everything within the polygons envelope is chosen (for debugging)
			else if(methodStr == "envelope"){this->method = rsgis::img::envelope;}
			// Set to default value if not recognised.
			else {cerr << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;this->method = rsgis::img::polyContainsPixelCenter;}
			XMLString::release(&charValue);
		}
		else
		{
			cerr << "\tMethod not recognised, using default of \'polyContainsPixelCenter\'." << endl;
			this->method = rsgis::img::polyContainsPixelCenter;
		}
		XMLString::release(&methodXMLStr);
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeZonalStats.");
		throw RSGISXMLArgumentsException(message.c_str());
	}

	parsed = true;

	XMLString::release(&algorName);
	XMLString::release(&algorXMLStr);
	XMLString::release(&optionXMLStr);
	XMLString::release(&optionPolygonsToShp);
	XMLString::release(&optionRasterPolygonsToShp);
	XMLString::release(&optionRasterPolygonsToTxt);
	XMLString::release(&optionPixelVals2txt);
    XMLString::release(&optionVariablesToMatrix);
	XMLString::release(&optionImageToMatrix);
	XMLString::release(&optionPixelCount);
	XMLString::release(&optionPixelMean);
	XMLString::release(&optionPixelWeightedMean);
	XMLString::release(&optionPixelMeanLSSVar);
	XMLString::release(&optionFuzzy);
	XMLString::release(&optionPowersetCount);
	XMLString::release(&rsgisattributeXMLStr);
	XMLString::release(&rsgisbandXMLStr);
	XMLString::release(&optionPointValue);
    XMLString::release(&optionEndmembers);
    XMLString::release(&optionImageZone2hdf);

}

void RSGISExeZonalStats::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(this->option == RSGISExeZonalStats::polygons2shp)
		{
			cout << "polygons2shp\n";

            // Convert to absolute path
            this->inputVecPolys = boost::filesystem::absolute(this->inputVecPolys).c_str();
            this->outputVecPolys = boost::filesystem::absolute(this->outputVecPolys).c_str();

			GDALAllRegister();
			OGRRegisterAll();

			RSGISVectorUtils vecUtils;
			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVecPolys);

			GDALDataset *inputImageDS = NULL;
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			ZonalStats *zonal = NULL;
			bool **toCalc = NULL;
			try
			{
				inputImageDS = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
				if(inputImageDS == NULL)
				{
					string message = string("Could not open image ") + inputImage;
					throw RSGISException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
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
				outputSHPDS = (GDALDataset*) GDALOpenEx(this->outputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVecPolys;
					throw RSGISException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISException(message.c_str());
				}

				toCalc = new bool*[inputImageDS->GetRasterCount()];
				for(int i = 0; i < inputImageDS->GetRasterCount(); i++)
				{
					toCalc[i] = new bool[4];
					if(this->mean)
					{
						toCalc[i][0] = true;
					}
					else
					{
						toCalc[i][0] = false;
					}

					if(this->min)
					{
						toCalc[i][1] = true;
					}
					else
					{
						toCalc[i][1] = false;
					}

					if(this->max)
					{
						toCalc[i][2] = true;
					}
					else
					{
						toCalc[i][2] = false;
					}

					if(this->stddev)
					{
						toCalc[i][3] = true;
					}
					else
					{
						toCalc[i][3] = false;
					}
				}


				zonal = new ZonalStats();
				zonal->zonalStatsVector(inputImageDS, inputSHPLayer, toCalc, outputSHPLayer);
			}
			catch (RSGISException e)
			{
				throw e;
			}

			if(zonal != NULL)
			{
				delete zonal;
			}
			if(toCalc != NULL)
			{
				for(int i = 0; i < inputImageDS->GetRasterCount(); i++)
				{
					delete[] toCalc[i];
				}
				delete[] toCalc;
			}
			GDALClose(inputImageDS);
			GDALClose(inputSHPDS);
			GDALClose(outputSHPDS);

			//OGRCleanupAll();
			//GDALDestroyDriverManager();
		}
		else if(this->option == RSGISExeZonalStats::rasterpolygons2shp)
		{
			cout << "rasterpolygons2shp\n";

            // Convert to absolute path
            this->inputVecPolys = boost::filesystem::absolute(this->inputVecPolys).c_str();
            this->outputVecPolys = boost::filesystem::absolute(this->outputVecPolys).c_str();

			GDALAllRegister();
			OGRRegisterAll();

			RSGISVectorUtils vecUtils;
			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVecPolys);

			GDALDataset *inputImageDS = NULL;
			GDALDataset *inputRasterFeaturesDS = NULL;
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			ZonalStats *zonal = NULL;
			bool **toCalc = NULL;
			try
			{
				inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inputImageDS == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}

				inputRasterFeaturesDS = (GDALDataset *) GDALOpen(this->inputRasPolys.c_str(), GA_ReadOnly);
				if(inputRasterFeaturesDS == NULL)
				{
					string message = string("Could not open image ") + this->inputRasPolys;
					throw RSGISException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
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
					throw RSGISException("SHP driver not available.");
				}
				outputSHPDS = (GDALDataset*) GDALOpenEx(this->outputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVecPolys;
					throw RSGISException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISException(message.c_str());
				}

				toCalc = new bool*[inputImageDS->GetRasterCount()];
				for(int i = 0; i < inputImageDS->GetRasterCount(); i++)
				{
					toCalc[i] = new bool[4];
					if(this->mean)
					{
						toCalc[i][0] = true;
					}
					else
					{
						toCalc[i][0] = false;
					}

					if(this->min)
					{
						toCalc[i][1] = true;
					}
					else
					{
						toCalc[i][1] = false;
					}

					if(this->max)
					{
						toCalc[i][2] = true;
					}
					else
					{
						toCalc[i][2] = false;
					}

					if(this->stddev)
					{
						toCalc[i][3] = true;
					}
					else
					{
						toCalc[i][3] = false;
					}
				}


				zonal = new ZonalStats();
				zonal->zonalStatsRaster(inputImageDS, inputRasterFeaturesDS, inputSHPLayer, outputSHPLayer, toCalc);
			}
			catch (RSGISException e)
			{
				throw e;
			}

			if(zonal != NULL)
			{
				delete zonal;
			}
			if(toCalc != NULL)
			{
				for(int i = 0; i < inputImageDS->GetRasterCount(); i++)
				{
					delete[] toCalc[i];
				}
				delete[] toCalc;
			}
			GDALClose(inputImageDS);
			GDALClose(inputRasterFeaturesDS);
			GDALClose(inputSHPDS);
			GDALClose(outputSHPDS);

			//OGRCleanupAll();
			//GDALDestroyDriverManager();
		}
		else if(this->option == RSGISExeZonalStats::rasterpolygons2txt)
		{
			cout << "rasterpolygons2txt\n";

            // Convert to absolute path
            this->inputVecPolys = boost::filesystem::absolute(this->inputVecPolys).c_str();

			GDALAllRegister();
			OGRRegisterAll();

			RSGISVectorUtils vecUtils;
			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);

			GDALDataset *inputImageDS = NULL;
			GDALDataset *inputRasterFeaturesDS = NULL;
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			ZonalStats *zonal = NULL;
			bool **toCalc = NULL;
			try
			{
				inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inputImageDS == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}

				inputRasterFeaturesDS = (GDALDataset *) GDALOpen(this->inputRasPolys.c_str(), GA_ReadOnly);
				if(inputRasterFeaturesDS == NULL)
				{
					string message = string("Could not open image ") + this->inputRasPolys;
					throw RSGISException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
				}

				toCalc = new bool*[inputImageDS->GetRasterCount()];
				for(int i = 0; i < inputImageDS->GetRasterCount(); i++)
				{
					toCalc[i] = new bool[4];
					if(this->mean)
					{
						toCalc[i][0] = true;
					}
					else
					{
						toCalc[i][0] = false;
					}

					if(this->min)
					{
						toCalc[i][1] = true;
					}
					else
					{
						toCalc[i][1] = false;
					}

					if(this->max)
					{
						toCalc[i][2] = true;
					}
					else
					{
						toCalc[i][2] = false;
					}

					if(this->stddev)
					{
						toCalc[i][3] = true;
					}
					else
					{
						toCalc[i][3] = false;
					}
				}

				zonal = new ZonalStats();
				zonal->zonalStatsRaster2txt(inputImageDS, inputRasterFeaturesDS, inputSHPLayer, this->outputTextFile, toCalc);
			}
			catch (RSGISException e)
			{
				throw e;
			}

			if(zonal != NULL)
			{
				delete zonal;
			}
			if(toCalc != NULL)
			{
				for(int i = 0; i < inputImageDS->GetRasterCount(); i++)
				{
					delete[] toCalc[i];
				}
				delete[] toCalc;
			}
			GDALClose(inputImageDS);
			GDALClose(inputRasterFeaturesDS);
			GDALClose(inputSHPDS);

			//OGRCleanupAll();
			//GDALDestroyDriverManager();
		}
		else if(this->option == RSGISExeZonalStats::pixelVals2txt)
		{
            std::cout << "Pixel values to CSV \n";
            std::cout << "Input Image: " << this->inputImage << std::endl;
            std::cout << "Input Vector: " << this->inputVecPolys << std::endl;

            try
            {
                rsgis::cmds::executePixelVals2txt(this->inputImage, this->inputVecPolys, this->outputMatrix,  this->polyAttribute,
                                                  this->outtxt, this->ignoreProjection, rsgis::img::pixelInPolyEnum2Int(this->method));
            }
            catch(rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }


		}
        else if(this->option == RSGISExeZonalStats::varibles2matrix)
		{
			cout << "varibles2matrix\n";
            cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Output Matrix: " << this->outputMatrix << endl;
			cout << "Attribute Name: " << this->polyAttribute << endl;

            // Convert to absolute path
            this->inputVecPolys = boost::filesystem::absolute(this->inputVecPolys).c_str();

			GDALAllRegister();
			OGRRegisterAll();

			RSGISVectorUtils vecUtils;
			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);

			GDALDataset **images = NULL;
			GDALDataset *inputVecDS = NULL;
			OGRLayer *inputVecLayer = NULL;

			RSGISZonalStats2Matrix zonalStats;
			ClassVariables **classVars = NULL;
			int numMatricies = 0;

			try
			{
				// Open Image
				images = new GDALDataset*[1];
				images[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(images[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}

				// Open vector
				inputVecDS = (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputVecDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputVecLayer = inputVecDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputVecLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
				}

				classVars = zonalStats.findPixelStats(images, 1, inputVecLayer, this->polyAttribute, &numMatricies, this->method);
				RSGISMatrices matrixUtils;
				string filepath = "";
				for(int i = 0; i < numMatricies; i++)
				{
					filepath = this->outputMatrix + classVars[i]->name;
					cout << "Saving .. " << filepath << endl;
					matrixUtils.saveMatrix2txt(classVars[i]->matrix, filepath);
				}
			}
			catch(RSGISException e)
			{
				throw e;
			}

			GDALClose(images[0]);
			GDALClose(inputVecDS);

			//OGRCleanupAll();
			//GDALDestroyDriverManager();
		}
		else if(this->option == RSGISExeZonalStats::image2matrix)
		{
			cout << "image2matrix\n";
			GDALAllRegister();
			OGRRegisterAll();

			RSGISMathsUtils mathsUtils;
			RSGISVectorUtils vecUtils;
			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);

			GDALDataset **images = NULL;
			GDALDataset *inputVecDS = NULL;
			OGRLayer *inputVecLayer = NULL;

			RSGISZonalStats2Matrix zonalStats;
			Matrix **matrices;
			int numMatricies = 0;

			try
			{
				// Open Image
				images = new GDALDataset*[1];
				images[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(images[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}

				// Open vector
				inputVecDS = (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputVecDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputVecLayer = inputVecDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputVecLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
				}

				matrices = zonalStats.findPixelsForImageBand(images, 1, inputVecLayer, &numMatricies, this->imageBand);
				RSGISMatrices matrixUtils;
				string filepath = "";
				for(int i = 0; i < numMatricies; i++)
				{
					filepath = this->outputMatrix + mathsUtils.inttostring(i);
					//cout << i << ") " << classVars[i]->name << " has a total number of pixels = " << classVars[i]->numPxls << endl;
					//matrixUtils.printMatrix(classVars[i]->matrix);
					cout << "Saving .. " << filepath << endl;
					matrixUtils.saveMatrix2txt(matrices[i], filepath);
				}
			}
			catch(RSGISException& e)
			{
				cout << "RSGISException caught: " << e.what() << endl;
			}

			GDALClose(images[0]);
			GDALClose(inputVecDS);

			//OGRCleanupAll();
			//GDALDestroyDriverManager();
		}
		else if(this->option == RSGISExeZonalStats::pixelcount)
		{
			cout << "Pixel Count Zonal Stats\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Input Raster Polygons: " << this->inputRasPolys << endl;
			cout << "Output Vector : " << this->outputVecPolys << endl;

            // Convert to absolute path
            this->inputVecPolys = boost::filesystem::absolute(this->inputVecPolys).c_str();
            this->outputVecPolys = boost::filesystem::absolute(this->outputVecPolys).c_str();

			GDALAllRegister();
			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVecPolys);

			GDALDataset *inputImageDS = NULL;
			GDALDataset *inputRasterFeaturesDS = NULL;
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";

			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVecPolys);

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

				inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inputImageDS == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}

				inputRasterFeaturesDS = (GDALDataset *) GDALOpen(this->inputRasPolys.c_str(), GA_ReadOnly);
				if(inputRasterFeaturesDS == NULL)
				{
					string message = string("Could not open image ") + this->inputRasPolys;
					throw RSGISException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
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
					throw RSGISException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVecPolys.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVecPolys;
					throw RSGISException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISException(message.c_str());
				}

				processFeature = new RSGISZonalCountStats(inputImageDS, inputRasterFeaturesDS, attributeCountList, numAttributes, pxlcount, emptyset);
				processVector = new RSGISProcessVector(processFeature);

				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);

				GDALClose(inputImageDS);
				GDALClose(inputRasterFeaturesDS);
				GDALClose(inputSHPDS);
				GDALClose(outputSHPDS);

				delete processVector;
				delete processFeature;

				for(int i = 0; i < numAttributes; i++)
				{
					delete[] attributeCountList[i]->bands;
					delete[] attributeCountList[i]->thresholds;
					delete attributeCountList[i];
				}
				delete[] attributeCountList;

				//OGRCleanupAll();
				//GDALDestroyDriverManager();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(this->option == RSGISExeZonalStats::pixelstats)
		{
			RSGISMathsUtils mathsUtil;
			cout << "Zonal Stats\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			if (useRasPoly) {cout << "Input Raster Polygons: " << this->inputRasPolys << endl;}
			if (this->outputToText)
			{
				cout << "Output CSV : " << this->outputTextFile << endl;

			}
			else
			{
				cout << "Output Vector : " << this->outputVecPolys << endl;
                // Convert to absolute path
                this->outputVecPolys = boost::filesystem::absolute(this->outputVecPolys).c_str();
			}

			// Convert to absolute path
			this->inputVecPolys = boost::filesystem::absolute(this->inputVecPolys).c_str();

			GDALAllRegister();
			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);
			string SHPFileOutLayer = "";
			if (!this->outputToText){SHPFileOutLayer = vecUtils.getLayerName(this->outputVecPolys);}

			GDALDataset *inputImageDS = NULL;
			GDALDataset *inputRasterFeaturesDS = NULL;
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";

			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVecPolys);

				// Check is output shapefile exists
				if (!this->outputToText)
				{
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
				}

				/////////////////////////////////////
				//
				// Open Input Image.
				//
				/////////////////////////////////////

				inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inputImageDS == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}


				// Set up attributes if using all bands
				if(calcZonalAllBands)
				{
					unsigned int nImageBands = inputImageDS->GetRasterCount();
					cout << "Calculating stats for " << nImageBands << " bands." << endl;
					this->numAttributes = nImageBands;
					this->attributeZonalList = new ZonalAttributes*[nImageBands];
					for(unsigned int i = 0; i < nImageBands; i++)
					{
						attributeZonalList[i] = new ZonalAttributes();
						string bandNumberStr = mathsUtil.inttostring(i + 1).c_str();
						attributeZonalList[i]->name = "b" + bandNumberStr;
						attributeZonalList[i]->outMin = this->minAll;
						attributeZonalList[i]->outMax = this->maxAll;
						attributeZonalList[i]->outMean = this->meanAll;
						attributeZonalList[i]->outStDev = this->stdDevAll;
                        attributeZonalList[i]->outMode = this->modeAll;
                        attributeZonalList[i]->outSum = this->sumAll;
						attributeZonalList[i]->outCount = this->countAll;
						attributeZonalList[i]->numBands = 1;
						attributeZonalList[i]->bands = new int[1];
						attributeZonalList[i]->minThresholds = new float[1];
						attributeZonalList[i]->maxThresholds = new float[1];
						attributeZonalList[i]->bands[0] = i;
						attributeZonalList[i]->minThresholds[0] = this->minThreshAllVal;
						attributeZonalList[i]->maxThresholds[0] = this->maxThreshAllVal;

                        // If using band names get names from image
                        if(this->useBandNames)
                        {
                            std::string bandName = inputImageDS->GetRasterBand(i+1)->GetDescription();

                            // Replace spaces and parentheses in file name
                            boost::algorithm::replace_all(bandName, " ", "_");
                            boost::algorithm::replace_all(bandName, "(", "_");
                            boost::algorithm::replace_all(bandName, ")", "_");
                            boost::algorithm::replace_all(bandName, "[", "_");
                            boost::algorithm::replace_all(bandName, "]", "_");
                            boost::algorithm::replace_all(bandName, ":", "");
                            boost::algorithm::replace_all(bandName, "?", "");
                            boost::algorithm::replace_all(bandName, ">", "gt");
                            boost::algorithm::replace_all(bandName, "<", "lt");
                            boost::algorithm::replace_all(bandName, "=", "eq");

                            /* Check if band name us longer than maximum length for shapefile field name
                             No limit on CSV but makes management easier with shorter names */
                            if((bandName.length() > 7) && this->shortenBandNames)
                            {
                                // If not using all of name, append number so unique
                                std::cerr << "WARNING: "<< bandName << " will be truncated to \'" << bandName.substr(0, 5) << i+1 << "\'" << std::endl;
                                attributeZonalList[i]->name = bandName.substr(0, 5) + mathsUtil.inttostring(i+1);
                            }
                            else if(bandName == "")
                            {
                                // If escription is empty, use b1 etc.,
                                attributeZonalList[i]->name  = std::string("b") + mathsUtil.inttostring(i+1);
                            }
                            else{attributeZonalList[i]->name = bandName;}
                        }

                        else
                        {
                            attributeZonalList[i]->name = std::string("b") + mathsUtil.inttostring(i+1);
                        }

					}
				}

				if (useRasPoly)
				{
					/////////////////////////////////////
					//
					// Open Rasterised Polygon Image.
					//
					/////////////////////////////////////
					inputRasterFeaturesDS = (GDALDataset *) GDALOpen(this->inputRasPolys.c_str(), GA_ReadOnly);
					if(inputRasterFeaturesDS == NULL)
					{
						string message = string("Could not open image ") + this->inputRasPolys;
						throw RSGISException(message.c_str());
					}

                    cout << "Using raster for point in polygon...\n";
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
				}

				inputSpatialRef = inputSHPLayer->GetSpatialRef();

                // Check the projection is the same for shapefile and image
                if(!this->ignoreProjection)
                {
                    const char *pszWKTImg = inputImageDS->GetProjectionRef();
                    char **pszWKTShp = new char*[1];
                    inputSpatialRef->exportToWkt(pszWKTShp);

                    if((string(pszWKTImg) != string(pszWKTShp[0])))
                    {
                        cerr << "WARNING: Shapefile and image are not the same projection!\n\tImage is: " + string(pszWKTImg) + "\n\tShapefile is: " + string(pszWKTShp[0]) << "\n...Continuing anyway" << endl;
                    }
                    OGRFree(pszWKTShp);
                }

				if(!this->outputToText)
				{
					/////////////////////////////////////
					//
					// Create Output Shapfile.
					//
					/////////////////////////////////////
					const char *pszDriverName = "ESRI Shapefile";
					shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
					if( shpFiledriver == NULL )
					{
						throw RSGISException("SHP driver not available.");
					}
					outputSHPDS = shpFiledriver->Create(this->outputVecPolys.c_str(), 0, 0, 0, GDT_Unknown, NULL );
					if( outputSHPDS == NULL )
					{
						string message = string("Could not create vector file ") + this->outputVecPolys;
						throw RSGISException(message.c_str());
					}
					outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
					if( outputSHPLayer == NULL )
					{
						string message = string("Could not create vector layer ") + SHPFileOutLayer;
						throw RSGISException(message.c_str());
					}

				}

				if(useRasPoly)
				{
					processFeature = new RSGISZonalStats(inputImageDS, inputRasterFeaturesDS, attributeZonalList, numAttributes, pxlcount, this->outputTextFile);
					processVector = new RSGISProcessVector(processFeature);
				}
				else
				{
					processFeature = new RSGISZonalStatsPoly(inputImageDS, attributeZonalList, numAttributes, pxlcount, method, this->outputTextFile);
					processVector = new RSGISProcessVector(processFeature);
				}
				if (this->outputToText)
				{
					processVector->processVectorsNoOutput(inputSHPLayer, true);
				}
				else
				{
					processVector->processVectors(inputSHPLayer, outputSHPLayer, this->copyAttributes, true, false);
				}


				// TIDY
				GDALClose(inputImageDS); // Close input image
				cout << "Image closed OK" << endl;
				if(useRasPoly) {GDALClose(inputRasterFeaturesDS);} // Close rasterised poly (if using)
				GDALClose(inputSHPDS); // Close inputshape
				cout << "in shp closed OK" << endl;
				if (!this->outputToText)
				{
					GDALClose(outputSHPDS); // Close output shape
					cout << "out shp closed OK" << endl;
				}
				for(int i = 0; i < numAttributes; i++)
				{
					delete[] attributeZonalList[i]->bands;
					delete[] attributeZonalList[i]->minThresholds;
					delete[] attributeZonalList[i]->maxThresholds;
					delete attributeZonalList[i];
				}
				delete[] attributeZonalList;

				delete processVector;
				delete processFeature;

				//OGRCleanupAll();
				//GDALDestroyDriverManager();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(this->option == RSGISExeZonalStats::pixelmean)
		{
			cout << "Pixel Mean Zonal Stats\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			if (useRasPoly) {cout << "Input Raster Polygons: " << this->inputRasPolys << endl;}
			cout << "Output Vector : " << this->outputVecPolys << endl;

            // Convert to absolute path
			this->inputVecPolys = boost::filesystem::absolute(this->inputVecPolys).c_str();
            this->outputVecPolys = boost::filesystem::absolute(this->outputVecPolys).c_str();

			GDALAllRegister();
			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVecPolys);

			GDALDataset *inputImageDS = NULL;
			GDALDataset *inputRasterFeaturesDS = NULL;
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";

			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVecPolys);

				// Check is output shapefile exists
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

				inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inputImageDS == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}
				if (useRasPoly)
				{
					inputRasterFeaturesDS = (GDALDataset *) GDALOpen(this->inputRasPolys.c_str(), GA_ReadOnly);
					if(inputRasterFeaturesDS == NULL)
					{
						string message = string("Could not open image ") + this->inputRasPolys;
						throw RSGISException(message.c_str());
					}
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
				}

				inputSpatialRef = inputSHPLayer->GetSpatialRef();

                // Check the projection is the same for shapefile and image
                if(!this->ignoreProjection)
                {
                    const char *pszWKTImg = inputImageDS->GetProjectionRef();
                    char **pszWKTShp = new char*[1];
                    inputSpatialRef->exportToWkt(pszWKTShp);

                    if((string(pszWKTImg) != string(pszWKTShp[0])))
                    {
                        cerr << "WARNING: Shapefile and image are not the same projection!\n\tImage is: " + string(pszWKTImg) + "\n\tShapefile is: " + string(pszWKTShp[0]) << "\n...Continuing anyway" << endl;
                    }
                    OGRFree(pszWKTShp);
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
					throw RSGISException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVecPolys.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVecPolys;
					throw RSGISException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISException(message.c_str());
				}
				if(useRasPoly)
				{
					processFeature = new RSGISZonalMeanStats(inputImageDS, inputRasterFeaturesDS, attributeMeanList, numAttributes, pxlcount);
					processVector = new RSGISProcessVector(processFeature);
				}
				else
				{
					processFeature = new RSGISZonalMeanStatsPoly(inputImageDS, attributeMeanList, numAttributes, pxlcount, method, this->dB);
					processVector = new RSGISProcessVector(processFeature);
				}

				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);

				// TIDY
				GDALClose(inputImageDS); // Close input image
				if(useRasPoly) {GDALClose(inputRasterFeaturesDS);} // Close rasterised poly (if using)
				GDALClose(inputSHPDS); // Close inputshape
				GDALClose(outputSHPDS); // Close output shape

				for(int i = 0; i < numAttributes; i++)
				{
					delete[] attributeMeanList[i]->bands;
					delete[] attributeMeanList[i]->minThresholds;
					delete[] attributeMeanList[i]->maxThresholds;
					delete attributeMeanList[i];
				}
				delete[] attributeMeanList;

				delete processVector;
				delete processFeature;

				//OGRCleanupAll();
				//GDALDestroyDriverManager();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(this->option == RSGISExeZonalStats::pixelWeightedMean)
		{
			cout << "Pixel Mean Zonal Stats\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Output Vector : " << this->outputVecPolys << endl;

            // Convert to absolute path
			this->inputVecPolys = boost::filesystem::absolute(this->inputVecPolys).c_str();
            this->outputVecPolys = boost::filesystem::absolute(this->outputVecPolys).c_str();

			GDALAllRegister();
			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVecPolys);

			GDALDataset *inputImageDS = NULL;
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";

			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVecPolys);

				// Check is output shapefile exists
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

				inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inputImageDS == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS =  (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
				}

                inputSpatialRef = inputSHPLayer->GetSpatialRef();

                // Check the projection is the same for shapefile and image
                if(!this->ignoreProjection)
                {
                    const char *pszWKTImg = inputImageDS->GetProjectionRef();
                    char **pszWKTShp = new char*[1];
                    inputSpatialRef->exportToWkt(pszWKTShp);

                    if((string(pszWKTImg) != string(pszWKTShp[0])))
                    {
                        cerr << "WARNING: Shapefile and image are not the same projection!\n\tImage is: " + string(pszWKTImg) + "\n\tShapefile is: " + string(pszWKTShp[0]) << "\n...Continuing anyway" << endl;
                    }
                    OGRFree(pszWKTShp);
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
					throw RSGISException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVecPolys.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVecPolys;
					throw RSGISException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISException(message.c_str());
				}

				processFeature = new RSGISZonalWeightedMeanStatsPoly(inputImageDS, attributeMeanList, numAttributes, pxlcount);
				processVector = new RSGISProcessVector(processFeature);


				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);

				// TIDY
				GDALClose(inputImageDS); // Close input image
				GDALClose(inputSHPDS); // Close inputshape
				GDALClose(outputSHPDS); // Close output shape

				for(int i = 0; i < numAttributes; i++)
				{
					delete[] attributeMeanList[i]->bands;
					delete[] attributeMeanList[i]->minThresholds;
					delete[] attributeMeanList[i]->maxThresholds;
					delete attributeMeanList[i];
				}
				delete[] attributeMeanList;

				delete processVector;
				delete processFeature;

				//OGRCleanupAll();
				//GDALDestroyDriverManager();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(this->option == RSGISExeZonalStats::pixelmeanLSSVar)
		{
			cout << "Pixel Mean Zonal Stats - with variance of local pixels\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Output Vector : " << this->outputVecPolys << endl;

            // Convert to absolute path
			this->inputVecPolys = boost::filesystem::absolute(this->inputVecPolys).c_str();
            this->outputVecPolys = boost::filesystem::absolute(this->outputVecPolys).c_str();

			GDALAllRegister();
			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVecPolys);

			GDALDataset *inputImageDS = NULL;
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";

			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVecPolys);

				// Check is output shapefile exists
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

				inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inputImageDS == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS =  (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
				}

                inputSpatialRef = inputSHPLayer->GetSpatialRef();

                // Check the projection is the same for shapefile and image
                if(!this->ignoreProjection)
                {
                    const char *pszWKTImg = inputImageDS->GetProjectionRef();
                    char **pszWKTShp = new char*[1];
                    inputSpatialRef->exportToWkt(pszWKTShp);

                    if((string(pszWKTImg) != string(pszWKTShp[0])))
                    {
                        cerr << "WARNING: Shapefile and image are not the same projection!\n\tImage is: " + string(pszWKTImg) + "\n\tShapefile is: " + string(pszWKTShp[0]) << "\n...Continuing anyway" << endl;
                    }
                    OGRFree(pszWKTShp);
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
					throw RSGISException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVecPolys.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVecPolys;
					throw RSGISException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISException(message.c_str());
				}

				if (usePixelSize)
				{
					double *transformations = new double[6];
					inputImageDS[0].GetGeoTransform(transformations);

					// Get pixel size
					this->offsetSize = transformations[1];
					delete[] transformations;
				}

				processFeature = new RSGISZonalLSSMeanVar(inputImageDS, attributeMeanList, numAttributes, pxlcount, windowSize, offsetSize, method);
				processVector = new RSGISProcessVector(processFeature);


				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);

				// TIDY
				GDALClose(inputImageDS); // Close input image
				GDALClose(inputSHPDS); // Close inputshape
				GDALClose(outputSHPDS); // Close output shape

				for(int i = 0; i < numAttributes; i++)
				{
					delete[] attributeMeanList[i]->bands;
					delete[] attributeMeanList[i]->minThresholds;
					delete[] attributeMeanList[i]->maxThresholds;
					delete attributeMeanList[i];
				}
				delete[] attributeMeanList;

				delete processVector;
				delete processFeature;

				//OGRCleanupAll();
				//GDALDestroyDriverManager();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(this->option == RSGISExeZonalStats::fuzzy)
		{
			cout << "Fuzzy Summary Zonal Stats\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Class Attribute: " << this->classAttribute << endl;
			cout << "Input Raster Polygons: " << this->inputRasPolys << endl;
			cout << "Output Vector : " << this->outputVecPolys << endl;
			cout << "Histogram bin size: " << this->binsize << endl;

            // Convert to absolute path
			this->inputVecPolys = boost::filesystem::absolute(this->inputVecPolys).c_str();
            this->outputVecPolys = boost::filesystem::absolute(this->outputVecPolys).c_str();

			GDALAllRegister();
			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVecPolys);

			GDALDataset *inputImageDS = NULL;
			GDALDataset *inputRasterFeaturesDS = NULL;
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";
			int numInImgBands = 0;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVecPolys);

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

				inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inputImageDS == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}
				numInImgBands = inputImageDS->GetRasterCount();

				inputRasterFeaturesDS = (GDALDataset *) GDALOpen(this->inputRasPolys.c_str(), GA_ReadOnly);
				if(inputRasterFeaturesDS == NULL)
				{
					string message = string("Could not open image ") + this->inputRasPolys;
					throw RSGISException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS =  (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
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
					throw RSGISException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVecPolys.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVecPolys;
					throw RSGISException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISException(message.c_str());
				}

				processFeature = new RSGISFuzzyZonalStats(inputImageDS, inputRasterFeaturesDS, attributeFuzzyList, numAttributes, binsize, threshold, pxlcount, classAttribute);
				processVector = new RSGISProcessVector(processFeature);

				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);

				GDALClose(inputImageDS);
				GDALClose(inputRasterFeaturesDS);
				GDALClose(inputSHPDS);
				GDALClose(outputSHPDS);

				delete processVector;
				delete processFeature;

				for(int i = 0; i < numAttributes; i++)
				{
					delete[] attributeFuzzyList[i]->bands;
					delete attributeFuzzyList[i];
				}
				delete[] attributeFuzzyList;

				//OGRCleanupAll();
				//GDALDestroyDriverManager();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(this->option == RSGISExeZonalStats::powersetcount)
		{
			cout << "Calculate the Powerset for the top n elements\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Input Raster Polygons: " << this->inputRasPolys << endl;
			cout << "Output Vector : " << this->outputVecPolys << endl;
			cout << "Number of Top elements: " << this->nTop << endl;

            // Convert to absolute path
			this->inputVecPolys = boost::filesystem::absolute(this->inputVecPolys).c_str();
            this->outputVecPolys = boost::filesystem::absolute(this->outputVecPolys).c_str();

			GDALAllRegister();
			OGRRegisterAll();

			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;

			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVecPolys);

			GDALDataset *inputImageDS = NULL;
			GDALDataset *inputRasterFeaturesDS = NULL;
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			GDALDriver *shpFiledriver = NULL;
			GDALDataset *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;

			string outputDIR = "";

			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVecPolys);

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

				inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inputImageDS == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}

				inputRasterFeaturesDS = (GDALDataset *) GDALOpen(this->inputRasPolys.c_str(), GA_ReadOnly);
				if(inputRasterFeaturesDS == NULL)
				{
					string message = string("Could not open image ") + this->inputRasPolys;
					throw RSGISException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS =  (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
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
					throw RSGISException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(this->outputVecPolys.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVecPolys;
					throw RSGISException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISException(message.c_str());
				}

				processFeature = new RSGISPopulateTopPowerSet(inputImageDS, inputRasterFeaturesDS, attributeCountList, numAttributes, nTop);
				processVector = new RSGISProcessVector(processFeature);

				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);

				GDALClose(inputImageDS);
				GDALClose(inputRasterFeaturesDS);
				GDALClose(inputSHPDS);
				GDALClose(outputSHPDS);

				delete processVector;
				delete processFeature;

				//OGRCleanupAll();
				//GDALDestroyDriverManager();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
		else if(this->option == RSGISExeZonalStats::pointvalue)
		{
			cout << "Calculate zonal stats for vector\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			if (this->outputToText)
			{
				cout << "Output CSV : " << this->outputTextFile << endl;

			}
			else
			{
				cout << "Output Vector : " << this->outputVecPolys << endl;
			}

            try
            {
                if(this->outputToText)
                {
                    rsgis::cmds::executePointValue(this->inputImage, this->inputVecPolys, this->outputTextFile, true, false, this->useBandNames, this->shortenBandNames);
                }
                else
                {
                    rsgis::cmds::executePointValue(this->inputImage, this->inputVecPolys, this->outputVecPolys, false, this->force, this->useBandNames);
                }
            }
            catch(rsgis::cmds::RSGISCmdException &e)
            {
                throw rsgis::RSGISException(e.what());
            }


		}
        else if(this->option == RSGISExeZonalStats::endmembers)
		{
			cout << "A command to extract the pixel values for regions to a matrix file as columns which can be used as endmembers for unmixing\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Output Matrix : " << this->outputMatrix << endl;

			// Convert to absolute path
            this->inputVecPolys = boost::filesystem::absolute(this->inputVecPolys).c_str();

            GDALAllRegister();
			OGRRegisterAll();

			RSGISVectorUtils vecUtils;

			string SHPFileInLayer = vecUtils.getLayerName(this->inputVecPolys);

			GDALDataset *inputImageDS = NULL;
			GDALDataset *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;


			string outputDIR = "";

			try
			{
				/////////////////////////////////////
				//
				// Open Input Image.
				//
				/////////////////////////////////////
				inputImageDS = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(inputImageDS == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS =  (GDALDataset*) GDALOpenEx(this->inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVecPolys;
					throw RSGISException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISException(message.c_str());
				}

                RSGISExtractEndMembers2Matrix extractEndMembers;
                extractEndMembers.extractColumnPerPolygon2Matrix(inputImageDS, inputSHPLayer, this->outputMatrix, this->method);

				GDALClose(inputImageDS);
				GDALClose(inputSHPDS);


				//OGRCleanupAll();
				//GDALDestroyDriverManager();
			}
			catch (RSGISException& e)
			{
				throw e;
			}
		}
        else if(this->option == RSGISExeZonalStats::imagezone2hdf)
		{
			cout << "A command to extract the all the pixel values for regions to a HDF5 file\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Output File : " << this->outputFile << endl;
            cout.precision(12);
            try
			{
                rsgis::cmds::executeZonesImage2HDF5(this->inputImage, this->inputVecPolys, this->outputFile, this->ignoreProjection, this->method);
            }
            catch (rsgis::RSGISException e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (rsgis::cmds::RSGISCmdException e)
            {
                throw rsgis::RSGISException(e.what());
            }
            catch (std::exception e)
            {
                throw rsgis::RSGISException(e.what());
            }
        }
		else
		{
			cout << "The option is not recognised\n";
		}
	}
}

void RSGISExeZonalStats::printParameters()
{
	if(parsed)
	{
		if(this->option == RSGISExeZonalStats::polygons2shp)
		{
			cout << "polygons2shp\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Output Vector : " << this->outputVecPolys << endl;
			cout << "Mean: ";
			if(mean)
			{
				cout << "yes\n";
			}
			else
			{
				cout << "no\n";
			}

			cout << "Min: ";
			if(min)
			{
				cout << "yes\n";
			}
			else
			{
				cout << "no\n";
			}

			cout << "Max: ";
			if(max)
			{
				cout << "yes\n";
			}
			else
			{
				cout << "no\n";
			}

			cout << "Standard deviation: ";
			if(stddev)
			{
				cout << "yes\n";
			}
			else
			{
				cout << "no\n";
			}
		}
		else if(this->option == RSGISExeZonalStats::rasterpolygons2shp)
		{
			cout << "rasterpolygons2shp\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Input Raster Polygons: " << this->inputRasPolys << endl;
			cout << "Output Vector : " << this->outputVecPolys << endl;
			cout << "Mean: ";
			if(mean)
			{
				cout << "yes\n";
			}
			else
			{
				cout << "no\n";
			}

			cout << "Min: ";
			if(min)
			{
				cout << "yes\n";
			}
			else
			{
				cout << "no\n";
			}

			cout << "Max: ";
			if(max)
			{
				cout << "yes\n";
			}
			else
			{
				cout << "no\n";
			}

			cout << "Standard deviation: ";
			if(stddev)
			{
				cout << "yes\n";
			}
			else
			{
				cout << "no\n";
			}
		}
		else if(this->option == RSGISExeZonalStats::rasterpolygons2txt)
		{
			cout << "rasterpolygons2txt\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Input Raster Polygons: " << this->inputRasPolys << endl;
			cout << "Output textfile : " << this->outputTextFile << endl;
			cout << "Mean: ";
			if(mean)
			{
				cout << "yes\n";
			}
			else
			{
				cout << "no\n";
			}

			cout << "Min: ";
			if(min)
			{
				cout << "yes\n";
			}
			else
			{
				cout << "no\n";
			}

			cout << "Max: ";
			if(max)
			{
				cout << "yes\n";
			}
			else
			{
				cout << "no\n";
			}

			cout << "Standard deviation: ";
			if(stddev)
			{
				cout << "yes\n";
			}
			else
			{
				cout << "no\n";
			}
		}
		else if(this->option == RSGISExeZonalStats::pixelVals2txt)
		{
			cout << "pixelVals2txt\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Output Matrix: " << this->outputMatrix << endl;
			cout << "Method: " << this->method << endl;
			cout << "Output file format: " << this->outtxt << endl;
			cout << "Attribute Name: " << this->polyAttribute << endl;
		}
        else if(this->option == RSGISExeZonalStats::varibles2matrix)
		{
			cout << "varibles2matrix\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Output Matrix: " << this->outputMatrix << endl;
			cout << "Attribute Name: " << this->polyAttribute << endl;
		}
		else if(this->option == RSGISExeZonalStats::image2matrix)
		{
			cout << "image2matrix\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Output Matrix: " << this->outputMatrix << endl;
			cout << "Image Band: " << this->imageBand << endl;
		}
		else if(this->option == RSGISExeZonalStats::pixelcount)
		{
			cout << "Pixel Count\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Input Raster Polygons: " << this->inputRasPolys << endl;
			cout << "Output Vector : " << this->outputVecPolys << endl;
			if(this->force)
			{
				cout << "If output shapefile is present then it will be deleted\n";
			}
			if(this->pxlcount)
			{
				cout << "A pixel count will be outputted\n";
			}
			if(this->emptyset)
			{
				cout << "The empty set will be outputted.\n";
			}

			cout << numAttributes << " attributes are to be summarised\n";
			for(int i = 0; i < numAttributes; i++)
			{
				cout << i << ": " << attributeCountList[i]->name << " using bands [band:threshold] [";
				for(int j = 0; j < attributeCountList[i]->numBands; j++)
				{
					if(j == 0)
					{
						cout << "[" << attributeCountList[i]->bands[j] << ":" << attributeCountList[i]->thresholds[j] << "]";
					}
					else
					{
						cout << " [" << attributeCountList[i]->bands[j] << ":" << attributeCountList[i]->thresholds[j] << "]";
					}
				}
				cout << "]\n";
			}
		}
		else if(this->option == RSGISExeZonalStats::pixelmean)
		{
			cout << "Pixel Mean\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			if(useRasPoly){cout << "Input Raster Polygons: " << this->inputRasPolys << endl;}
			cout << "Output Vector : " << this->outputVecPolys << endl;
			if(this->force)
			{
				cout << "If output shapefile is present then it will be deleted\n";
			}
			if(this->pxlcount)
			{
				cout << "A pixel count will be outputted\n";
			}

			cout << numAttributes << " attributes are to be summarised\n";
			for(int i = 0; i < numAttributes; i++)
			{
				cout << i << ": " << attributeMeanList[i]->name << " using bands [band:minThreshold, maxThreshold] [";
				for(int j = 0; j < attributeMeanList[i]->numBands; j++)
				{
					if(j == 0)
					{
						cout << "[" << attributeMeanList[i]->bands[j] << ":" << attributeMeanList[i]->minThresholds[j] << "," << attributeMeanList[i]->maxThresholds[j] << "]";
					}
					else
					{
						cout << " [" << attributeMeanList[i]->bands[j] << ":" << attributeMeanList[i]->minThresholds[j] << "," << attributeMeanList[i]->maxThresholds[j] << "]";
					}
				}
				cout << "]\n";
			}
		}
		else if(this->option == RSGISExeZonalStats::pixelWeightedMean)
		{
			cout << "Pixel Weighted Mean\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Output Vector : " << this->outputVecPolys << endl;
			if(this->force)
			{
				cout << "If output shapefile is present then it will be deleted\n";
			}
			if(this->pxlcount)
			{
				cout << "A pixel count will be outputted\n";
			}

			cout << numAttributes << " attributes are to be summarised\n";
			for(int i = 0; i < numAttributes; i++)
			{
				cout << i << ": " << attributeMeanList[i]->name << " using bands [band:minThreshold, maxThreshold] [";
				for(int j = 0; j < attributeMeanList[i]->numBands; j++)
				{
					if(j == 0)
					{
						cout << "[" << attributeMeanList[i]->bands[j] << ":" << attributeMeanList[i]->minThresholds[j] << "," << attributeMeanList[i]->maxThresholds[j] << "]";
					}
					else
					{
						cout << " [" << attributeMeanList[i]->bands[j] << ":" << attributeMeanList[i]->minThresholds[j] << "," << attributeMeanList[i]->maxThresholds[j] << "]";
					}
				}
				cout << "]\n";
			}
		}
		else if(this->option == RSGISExeZonalStats::fuzzy)
		{
			cout << "Fuzzy Summary Zonal Stats\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Class Attribute: " << this->classAttribute << endl;
			cout << "Input Raster Polygons: " << this->inputRasPolys << endl;
			cout << "Output Vector : " << this->outputVecPolys << endl;
			cout << "Histogram bin size: " << this->binsize << endl;
			if(this->force)
			{
				cout << "If output shapefile is present then it will be deleted\n";
			}
			if(this->pxlcount)
			{
				cout << "A pixel count will be outputted\n";
			}
			if(this->emptyset)
			{
				cout << "The empty set will be outputted.\n";
			}

			cout << numAttributes << " attributes are to be summarised\n";
			for(int i = 0; i < numAttributes; i++)
			{
				cout << attributeFuzzyList[i]->name << " [" << attributeFuzzyList[i]->index << "] is within class " << attributeFuzzyList[i]->fuzzyClass << " using bands " << attributeFuzzyList[i]->numBands << " [";
				for(int j = 0; j < attributeFuzzyList[i]->numBands; j++)
				{
					if(j < (attributeFuzzyList[i]->numBands-1) & attributeFuzzyList[i]->numBands > 1)
					{
						cout << attributeFuzzyList[i]->bands[j] << " ";
					}
					else
					{
						cout << attributeFuzzyList[i]->bands[j];
					}
				}
				cout << "]\n";
			}
		}
		else if(this->option == RSGISExeZonalStats::powersetcount)
		{
			cout << "Calculate the Powerset for the top n elements\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Input Raster Polygons: " << this->inputRasPolys << endl;
			cout << "Output Vector : " << this->outputVecPolys << endl;
			cout << "n top attributes: " << this->nTop << endl;

			if(this->force)
			{
				cout << "If output shapefile is present then it will be deleted\n";
			}

			cout << numAttributes << " attributes are to be summarised\n";
			for(int i = 0; i < numAttributes; i++)
			{
				cout << i << ": " << attributeCountList[i]->name << " using bands [band:threshold] [";
				for(int j = 0; j < attributeCountList[i]->numBands; j++)
				{
					if(j == 0)
					{
						cout << "[" << attributeCountList[i]->bands[j] << ":" << attributeCountList[i]->thresholds[j] << "]";
					}
					else
					{
						cout << " [" << attributeCountList[i]->bands[j] << ":" << attributeCountList[i]->thresholds[j] << "]";
					}
				}
				cout << "]\n";
			}
		}
		else if(this->option == RSGISExeZonalStats::pointvalue)
		{
			cout << "Calculate the Powerset for the top n elements\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Output Vector : " << this->outputVecPolys << endl;
		}
        else if(this->option == RSGISExeZonalStats::endmembers)
		{
			cout << "A command to extract the pixel values for regions to a matrix file as columns which can be used as endmembers for unmixing\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVecPolys << endl;
			cout << "Output Matrix : " << this->outputMatrix << endl;
		}
		else
		{
			cout << "The option is not recognised\n";
		}
	}
	else
	{
		cout << "The parameters have yet to be parsed\n";
	}
}


void RSGISExeZonalStats::help()
{
	cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
    cout << "<rsgis:command algor=\"zonalstats\" option=\"pixelVals2txt\" image=\"image.env\"" << endl;
    cout << "    vector=\"polys.shp\" output=\"output_base\" attribute=\"classname\" " << endl;
    cout << "    outTXT=\"mtxt | gtxt | csv\"/>" << endl;
    cout << "    " << endl;
    cout << "<rsgis:command algor=\"zonalstats\" option=\"image2matrix\" image=\"image.env\"" << endl;
    cout << "    vector=\"polys.shp\" output=\"output.mtxt\" band=\"int\" />" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"zonalstats\" option=\"pixelstats\" image=\"image.env\"  " << endl;
    cout << "    vector=\"polys.shp\" raster=\"polys.env\" output=\"output.shp\" force=\"yes | no\"" << endl;
    cout << "    ignoreProjection=\"yes | no\" copyAttributes=\"yes | no\" useBandNames=\"yes | no\" pxlcount=\"yes | no\"" << endl;
    cout << "    mean=\"yes | no \" min=\"yes | no\" max=\"yes | no \" stddev=\"yes | no\" " << endl;
    cout << "    mode=\"yes | no\" count=\"yes | no\" minThreshold=\"float\" maxThreshold=\"float\" >" << endl;
    cout << "    <rsgis:attribute name=\"attribute_name\" >" << endl;
    cout << "        <rsgis:band band=\"int\" minThreshold=\"float\" maxThreshold=\"float\" />" << endl;
    cout << "    </rsgis:attribute>" << endl;
    cout << "    <rsgis:attribute name=\"attribute_name\" >" << endl;
    cout << "        <rsgis:band band=\"int\" minThreshold=\"float\" maxThreshold=\"float\" />" << endl;
    cout << "    </rsgis:attribute>" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"zonalstats\" option=\"pixelmean\" image=\"image.env\"" << endl;
    cout << "    raster=\"polys.env\"  vector=\"polys.shp\" output=\"output.shp\" force=\"yes | no\"" << endl;
    cout << "    pxlcount=\"yes | no\" dB=\"yes | no\">" << endl;
    cout << "    <rsgis:attribute name=\"attribute_name\" >" << endl;
    cout << "        <rsgis:band band=\"int\" minThreshold=\"float\" maxThreshold=\"float\" />" << endl;
    cout << "    </rsgis:attribute>" << endl;
    cout << "    <rsgis:attribute name=\"attribute_name\" >" << endl;
    cout << "        <rsgis:band band=\"int\" minThreshold=\"float\" maxThreshold=\"float\" />" << endl;
    cout << "    </rsgis:attribute>" << endl;
    cout << "    <rsgis:attribute name=\"attribute_name\" >" << endl;
    cout << "        <rsgis:band band=\"int\" minThreshold=\"float\" maxThreshold=\"float\" />" << endl;
    cout << "    </rsgis:attribute>" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"zonalstats\" option=\"pixelWeightedMean\"" << endl;
    cout << "    image=\"image.env\" vector=\"polys.shp\" output=\"output.shp\" " << endl;
    cout << "    force=\"yes | no\" pxlcount=\"yes | no\">" << endl;
    cout << "    <rsgis:attribute name=\"attribute_name\" >" << endl;
    cout << "        <rsgis:band band=\"int\" minThreshold=\"float\" maxThreshold=\"float\" />" << endl;
    cout << "    </rsgis:attribute>" << endl;
    cout << "    <rsgis:attribute name=\"attribute_name\" >" << endl;
    cout << "        <rsgis:band band=\"int\" minThreshold=\"float\" maxThreshold=\"float\" />" << endl;
    cout << "    </rsgis:attribute>" << endl;
    cout << "    <rsgis:attribute name=\"attribute_name\" >" << endl;
    cout << "        <rsgis:band band=\"int\" minThreshold=\"float\" maxThreshold=\"float\"/>" << endl;
    cout << "    </rsgis:attribute>" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"zonalstats\" option=\"pixelmeanLSSVar\" image=\"image.env\"" << endl;
    cout << "    vector=\"polys.shp\"  output=\"output.shp\" windowSize=\"int\" " << endl;
    cout << "    offsetSize=\"float\" force=\"yes | no\" pxlcount=\"yes | no\">" << endl;
    cout << "    <rsgis:attribute name=\"attribute_name\" >" << endl;
    cout << "        <rsgis:band band=\"int\" minThreshold=\"float\" maxThreshold=\"float\" />" << endl;
    cout << "    </rsgis:attribute>" << endl;
    cout << "    <rsgis:attribute name=\"attribute_name\" >" << endl;
    cout << "        <rsgis:band band=\"int\" minThreshold=\"float\" maxThreshold=\"float\" />" << endl;
    cout << "    </rsgis:attribute>" << endl;
    cout << "    <rsgis:attribute name=\"attribute_name\" >" << endl;
    cout << "        <rsgis:band band=\"int\" minThreshold=\"float\" maxThreshold=\"float\"/>" << endl;
    cout << "    </rsgis:attribute>" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"zonalstats\" option=\"pointvalue\" image=\"image.env\"" << endl;
    cout << "    vector=\"points.shp\" output=\"output.shp\" force=\"yes | no\" useBandNames=\"yes | no\" />" << endl;
    cout << "<rsgis:command algor=\"zonalstats\" option=\"pointvalue\" image=\"image.env\"" << endl;
    cout << "    vector=\"points.shp\" outputCSV=\"output.csv\" force=\"yes | no\" useBandNames=\"yes | no\" />" << endl;
    cout << "" << endl;
    cout << "<rsgis:command algor=\"zonalstats\" option=\"endmembers\" image=\"image.env\"" << endl;
    cout << "    vector=\"points.shp\" output=\"output.mtxt\"  />" << endl;
	cout << "</rsgis:commands>\n";
}

string RSGISExeZonalStats::getDescription()
{
	return "An interface to a the available zonal staticists functionality.";
}

string RSGISExeZonalStats::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeZonalStats::~RSGISExeZonalStats()
{

}

}

