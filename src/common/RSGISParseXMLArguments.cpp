/*
 *  RSGISParseXMLArguments.cpp
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

#include "RSGISParseXMLArguments.h"

namespace rsgis
{	

	RSGISParseXMLArguments::RSGISParseXMLArguments(string xmlFile)
	{
		this->xmlFile = xmlFile;
	}
	
	RSGISAlgorithmParameters** RSGISParseXMLArguments::parseArguments(int *numParams, RSGISAlgorParamsFactory *algorFactory) throw(RSGISXMLArgumentsException)
	{
		XMLCh tempStr[100];
		DOMImplementation *impl = NULL;
		DOMLSParser* parser = NULL;
		ErrorHandler* errHandler = NULL;
		DOMDocument *doc = NULL;
		DOMElement *rootElement = NULL;
		DOMNodeList *argumentsList = NULL;
		DOMElement *argElement = NULL;
		//const XMLCh *algorName = NULL;
		XMLCh *algorXMLStr = NULL;
		char *algorNameChar = NULL;
		string algorNameStr = "";
		XMLCh *commandsTag = NULL;
		XMLCh *commandTag = NULL;
		
		RSGISAlgorithmParameters **args = NULL;
		
		try 
		{
			XMLPlatformUtils::Initialize();
			
			commandsTag = XMLString::transcode("rsgis:commands");
			commandTag = XMLString::transcode("rsgis:command");
			algorXMLStr = XMLString::transcode("algor");

			XMLString::transcode("LS", tempStr, 99);
			impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
			if(impl == NULL)
			{
				throw RSGISXMLArgumentsException("DOMImplementation is NULL");
			}
			
			// Create Parser
			parser = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
			errHandler = (ErrorHandler*) new HandlerBase();
			parser->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler, errHandler);
			
			// Open Document
			doc = parser->parseURI(xmlFile.c_str());	
			
			// Get the Root element
			rootElement = doc->getDocumentElement();
			//cout << "Root Element: " << XMLString::transcode(rootElement->getTagName()) << endl;
			if(!XMLString::equals(rootElement->getTagName(), commandsTag))
			{
				throw RSGISXMLArgumentsException("Incorrect root element; Root element should be \"rsgis:commands\"");
			}
			
			argumentsList = rootElement->getElementsByTagName(commandTag);
			*numParams = argumentsList->getLength();
			//cout << "Number of parameters = " << *numParams << endl;
			
			args = new RSGISAlgorithmParameters*[*numParams];
			
			for(int i = 0; i < *numParams; i++)
			{
				argElement = static_cast<DOMElement*>(argumentsList->item(i));
				algorNameChar = XMLString::transcode(argElement->getAttribute(algorXMLStr));
				algorNameStr = string(algorNameChar);
				cout << "Algorithm = " << algorNameStr << endl;
				
				args[i] = algorFactory->getAlgorithmParamterObj(algorNameStr);
				XMLString::release(&algorNameChar);
				algorNameStr.erase();
				
				args[i]->retrieveParameters(argElement);
			}
			
			parser->release();
			delete errHandler;
			XMLString::release(&commandsTag);
			XMLString::release(&commandTag);
			XMLString::release(&algorXMLStr);
			
			XMLPlatformUtils::Terminate();
		}
		catch (const XMLException& e) 
		{
			parser->release();
			char *message = XMLString::transcode(e.getMessage());
			string outMessage =  string("XMLException : ") + string(message);
			throw RSGISXMLArgumentsException(outMessage.c_str());
		}
		catch (const DOMException& e) 
		{
			parser->release();
			char *message = XMLString::transcode(e.getMessage());
			string outMessage =  string("DOMException : ") + string(message);
			throw RSGISXMLArgumentsException(outMessage.c_str());
		}
		catch(RSGISXMLArgumentsException &e)
		{
			throw e;
		}
        catch(RSGISException &e)
		{
			throw RSGISXMLArgumentsException(e.what());
		}
		
		return args;
	}
	
	RSGISParseXMLArguments::~RSGISParseXMLArguments()
	{
		
	}
}
