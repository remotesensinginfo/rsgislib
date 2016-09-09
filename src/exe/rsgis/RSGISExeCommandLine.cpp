/*
 *  RSGISExeCommandLine.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/08/2009.
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

#include "RSGISExeCommandLine.h"

namespace rsgisexe{

RSGISExeCommandLine::RSGISExeCommandLine() : RSGISAlgorithmParameters()
{
	this->algorithm = "commandline";
	
	this->command = "";
}

RSGISAlgorithmParameters* RSGISExeCommandLine::getInstance()
{
	return new RSGISExeCommandLine();
}

void RSGISExeCommandLine::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *optionExeCommand = XMLString::transcode("execute");
	
	try
	{
		const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
		if(!XMLString::equals(algorName, algorNameEle))
		{
			throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
		}
		
		
		const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
		if(XMLString::equals(optionExeCommand, optionXML))
		{		
			this->option = RSGISExeCommandLine::execute;
			
			XMLCh *commandXMLStr = XMLString::transcode("command");
			if(argElement->hasAttribute(commandXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(commandXMLStr));
				this->command = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'command\' attribute was provided.");
			}
			XMLString::release(&commandXMLStr);
			
		}
		else
		{
			string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeCommandLine.");
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
	XMLString::release(&optionExeCommand);
	parsed = true;
}

void RSGISExeCommandLine::runAlgorithm() throw(RSGISException)
{
	if(parsed)
	{
		if(option == RSGISExeCommandLine::execute)
		{
			cout << "Execute command.\n";
			cout << "Command: " << this->command << endl;
			
			if(system(command.c_str()))
			{
				throw RSGISException("Command did not execute successfully.");
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


void RSGISExeCommandLine::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeCommandLine::execute)
		{
			cout << "Execute command.\n";
			cout << "Command: " << this->command << endl;
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

void RSGISExeCommandLine::help()
{
	cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
	cout << "\t<rsgis:command algor=\"commandline\" option=\"execute\" command=\"string\" />\n";
	cout << "</rsgis:commands>\n";
}

string RSGISExeCommandLine::getDescription()
{
	return "Utilities for running tools on the command line from with the XML environment.";
}

string RSGISExeCommandLine::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeCommandLine::~RSGISExeCommandLine()
{
	
}

}





