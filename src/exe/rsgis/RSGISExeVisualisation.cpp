/*
 *  RSGISExeVisualisation.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/07/2009.
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

#include "RSGISExeVisualisation.h"

namespace rsgisexe{

RSGISExeVisualisation::RSGISExeVisualisation() : RSGISAlgorithmParameters()
{
	this->algorithm = "visualisation";
	
	inputFile = "";
	outputFile = "";
	force = false;
}

RSGISAlgorithmParameters* RSGISExeVisualisation::getInstance()
{
	return new RSGISExeVisualisation();
}

void RSGISExeVisualisation::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;	
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *optionLine2dtoshpfile = XMLString::transcode("line2dtoshpfile");
	XMLCh *optionSetupplotter = XMLString::transcode("setupplotter");
	XMLCh *optionDeleteplotter = XMLString::transcode("deleteplotter");
	
	try
	{
		const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
		if(!XMLString::equals(algorName, algorNameEle))
		{
			throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
		}
		
		
		const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
		if(XMLString::equals(optionSetupplotter, optionXML))
		{
			this->option = RSGISExeVisualisation::setupplotter;
			
			XMLCh *outputXMLStr = XMLString::transcode("outdir");
			if(argElement->hasAttribute(outputXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputDIR = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'outdir\' attribute was provided.");
			}
			XMLString::release(&outputXMLStr);
			
		}
		else if(XMLString::equals(optionDeleteplotter, optionXML))
		{
			this->option = RSGISExeVisualisation::deleteplotter;
		}			
		else if(XMLString::equals(optionLine2dtoshpfile, optionXML))
		{		
			this->option = RSGISExeVisualisation::line2dtoshpfile;
			
			XMLCh *vectorXMLStr = XMLString::transcode("input");
			if(argElement->hasAttribute(vectorXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
				this->inputFile = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
			}
			XMLString::release(&vectorXMLStr);
			
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
		else
		{
			string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeVisualisation.");
			throw RSGISXMLArgumentsException(message.c_str());
		}
	}
	catch(RSGISXMLArgumentsException &e)
	{
		throw e;
	}
	
	XMLString::release(&algorName);
	XMLString::release(&algorXMLStr);
	XMLString::release(&optionXMLStr);
	XMLString::release(&optionLine2dtoshpfile);
	XMLString::release(&optionSetupplotter);
	XMLString::release(&optionDeleteplotter);
	
	parsed = true;
}

void RSGISExeVisualisation::runAlgorithm() throw(RSGISException)
{
	if(parsed)
	{
		if(option == RSGISExeVisualisation::setupplotter)
		{
			cout << "Setting up the Plotter with an output directory\n";
			cout << "Output directory: " << this->outputDIR << endl;
			
			RSGISExportForPlotting::getInstance(this->outputDIR);
		}
		else if(option == RSGISExeVisualisation::deleteplotter)
		{
			cout << "Delete the plotter.\n";
			delete RSGISExportForPlotting::getInstance();
		}
		else if(option == RSGISExeVisualisation::line2dtoshpfile)
		{
			cout << "Convert 2D Lines ptxt file to shapefile.\n";
			cout << "Input File: " << this->inputFile << endl;
			cout << "Output File: " << this->outputFile << endl;
			
			RSGISParseExportForPlotting parseTextFile;
			RSGISVectorIO vecIO;
			PlotTypes type = parseTextFile.identify(inputFile);
			if(type == lines2d)
			{
				cout << "Got lines2d file\n";
				list<double> *x1 = new list<double>();
				list<double> *y1 = new list<double>();
				list<double> *x2 = new list<double>();
				list<double> *y2 = new list<double>();
				parseTextFile.parseLines2D(inputFile, x1, y1, x2, y2);
				
				vecIO.exportLinesAsShp(outputFile, x1, y1, x2, y2, force);
				
				delete x1;
				delete y1;
				delete x2;
				delete y2;
			}
			else
			{
				throw RSGISException("This is not a lines 2D file and therefore cannot be parsed.");
			}
		}
		else
		{
			cout << "Options not recognised\n";
		}
	}
	else
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
}


void RSGISExeVisualisation::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeVisualisation::setupplotter)
		{
			cout << "Setup the Plotter with an output directory\n";
			cout << "Output directory: " << this->outputDIR << endl;
		}
		else if(option == RSGISExeVisualisation::deleteplotter)
		{
			cout << "Delete the plotter.\n";
		}
		else if(option == RSGISExeVisualisation::line2dtoshpfile)
		{
			cout << "Convert 2D Lines ptxt file to shapefile.\n";
			cout << "Input File: " << this->inputFile << endl;
			cout << "Output File: " << this->outputFile << endl;
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

void RSGISExeVisualisation::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
	cout << "\t<rsgis:command algor=\"visualisation\" option=\"setupplotter\" outdir=\"string\" />\n";
	cout << "\t<rsgis:command algor=\"visualisation\" option=\"deleteplotter\" />\n";
	cout << "\t<rsgis:command algor=\"visualisation\" option=\"line2dtoshpfile\" input=\"file.ptxt\" output=\"vector_out.shp\" force=\"yes | no\" />\n";
	cout << "</rsgis:commands>\n";
}

string RSGISExeVisualisation::getDescription()
{
	return "Utilities for converting files for visualisation.";
}

string RSGISExeVisualisation::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeVisualisation::~RSGISExeVisualisation()
{
	
}

}



