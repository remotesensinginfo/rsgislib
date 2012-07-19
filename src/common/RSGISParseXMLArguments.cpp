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

	RSGISParseXMLArguments::RSGISParseXMLArguments(std::string xmlFile)
	{
		this->xmlFile = xmlFile;
	}
	
	RSGISAlgorithmParameters** RSGISParseXMLArguments::parseArguments(int *numParams, RSGISAlgorParamsFactory *algorFactory) throw(RSGISXMLArgumentsException)
	{
        XMLCh tempStr[100];
		xercesc::DOMImplementation *impl = NULL;
		xercesc::DOMLSParser* parser = NULL;
		xercesc::ErrorHandler* errHandler = NULL;
		xercesc::DOMDocument *doc = NULL;
		xercesc::DOMElement *rootElement = NULL;
		xercesc::DOMNodeList *argumentsList = NULL;
		xercesc::DOMElement *argElement = NULL;
		//const XMLCh *algorName = NULL;
		XMLCh *algorXMLStr = NULL;
		char *algorNameChar = NULL;
        std::string algorNameStr = "";
		XMLCh *commandsTag = NULL;
		XMLCh *commandTag = NULL;
		
		RSGISAlgorithmParameters **args = NULL;
		
		try 
		{
			xercesc::XMLPlatformUtils::Initialize();
			
			commandsTag = xercesc::XMLString::transcode("rsgis:commands");
			commandTag = xercesc::XMLString::transcode("rsgis:command");
			algorXMLStr = xercesc::XMLString::transcode("algor");

			xercesc::XMLString::transcode("LS", tempStr, 99);
			impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
			if(impl == NULL)
			{
				throw RSGISXMLArgumentsException("DOMImplementation is NULL");
			}
			
			// Create Parser
			parser = ((xercesc::DOMImplementationLS*)impl)->createLSParser(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
			errHandler = (xercesc::ErrorHandler*) new xercesc::HandlerBase();
			parser->getDomConfig()->setParameter(xercesc::XMLUni::fgDOMErrorHandler, errHandler);
			
			// Open Document
			doc = parser->parseURI(xmlFile.c_str());	
			
			// Get the Root element
			rootElement = doc->getDocumentElement();
			//cout << "Root Element: " << XMLString::transcode(rootElement->getTagName()) << endl;
			if(!xercesc::XMLString::equals(rootElement->getTagName(), commandsTag))
			{
				throw RSGISXMLArgumentsException("Incorrect root element; Root element should be \"rsgis:commands\"");
			}
			
			argumentsList = rootElement->getElementsByTagName(commandTag);
			*numParams = argumentsList->getLength();
			//cout << "Number of parameters = " << *numParams << endl;
			
			args = new RSGISAlgorithmParameters*[*numParams];
			
			for(int i = 0; i < *numParams; i++)
			{
				argElement = static_cast<xercesc::DOMElement*>(argumentsList->item(i));
				algorNameChar = xercesc::XMLString::transcode(argElement->getAttribute(algorXMLStr));
				algorNameStr = std::string(algorNameChar);
                std::cout << "Algorithm = " << algorNameStr << std::endl;
				
				args[i] = algorFactory->getAlgorithmParamterObj(algorNameStr);
				xercesc::XMLString::release(&algorNameChar);
				algorNameStr.erase();
				
				args[i]->retrieveParameters(argElement);
			}
			
			parser->release();
			delete errHandler;
			xercesc::XMLString::release(&commandsTag);
			xercesc::XMLString::release(&commandTag);
			xercesc::XMLString::release(&algorXMLStr);
			
			xercesc::XMLPlatformUtils::Terminate();
		}
		catch (const xercesc::XMLException& e) 
		{
			parser->release();
			char *message = xercesc::XMLString::transcode(e.getMessage());
            std::string outMessage =  std::string("XMLException : ") + std::string(message);
			throw RSGISXMLArgumentsException(outMessage.c_str());
		}
		catch (const xercesc::DOMException& e) 
		{
			parser->release();
			char *message = xercesc::XMLString::transcode(e.getMessage());
            std::string outMessage =  std::string("DOMException : ") + std::string(message);
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
