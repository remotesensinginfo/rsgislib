/*
 *  RSGISParseLandXML.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 06/07/2009.
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

#include "RSGISParseLandXML.h"

namespace rsgis { namespace vec {
	
	RSGISParseLandXML::RSGISParseLandXML()
	{
	}
	
	void RSGISParseLandXML::getLandXMLLine(std::string xmlFile, std::vector<geos::geom::LineSegment*> *outLines)
	{
		this->xmlFile = xmlFile;
		XMLCh tempStr[100];
        xercesc::DOMImplementation *impl = NULL;
		xercesc::DOMLSParser* parser = NULL;
		xercesc::ErrorHandler* errHandler = NULL;
		xercesc::DOMDocument *doc = NULL;
		xercesc::DOMElement *rootElement = NULL;
		xercesc::DOMNodeList *linesList = NULL;
		//DOMNodeList *singlelineList = NULL;
		xercesc::DOMNode *argNode = NULL;
		//DOMNode *lineStartNode = NULL;
		//DOMNode *lineEndNode = NULL;
		XMLCh *landXMLRootTag = NULL;
		XMLCh *coordGeomTag = NULL;
		XMLCh *lineTag = NULL;
		XMLCh *lineStartTag = NULL;
		XMLCh *lineEndTag = NULL;
		std::string lineStart = "";
        geos::geom::Coordinate *startCoord;
        geos::geom::Coordinate *endCoord;
		int numLines = 0;
		
		try
		{
			xercesc::XMLPlatformUtils::Initialize();
            rsgis::math::RSGISMathsUtils mathUtils;
			landXMLRootTag = xercesc::XMLString::transcode("LandXML"); // LandXML root tag
			coordGeomTag = xercesc::XMLString::transcode("CoordGeom"); // Coordinate Geometry Tag
			lineTag = xercesc::XMLString::transcode("Line"); // Line Tag
			lineStartTag = xercesc::XMLString::transcode("Start"); // Start of line Tag
			lineEndTag = xercesc::XMLString::transcode("End"); // End of line Tag
			
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
			//std::cout << "Root Element: " << XMLString::transcode(rootElement->getTagName()) << std::endl;
			if(!xercesc::XMLString::equals(rootElement->getTagName(), landXMLRootTag))
			{
				throw RSGISXMLArgumentsException("XML file not in valid LandXML format");
			}
			
			linesList =  rootElement->getElementsByTagName(lineTag);
			numLines = linesList->getLength();
			
			std::cout << "The file contains " << numLines << " lines" << std::endl;
			
			for(int i = 0; i < numLines; i++)
			{
				/* Get all the text elements within each line tag (6 numbers)
				 * and split them by spaces, using the Boost tokenizer
				 */
				
				std::vector <double> *tempCoord = new std::vector<double>(); // Set up tempory vector to hold the coordinates
				startCoord = NULL;
				endCoord = NULL;
				argNode = static_cast<xercesc::DOMElement*>(linesList->item(i));
				
				// Get all text elements between <Line> and </Line> tag
				char *charValue = xercesc::XMLString::transcode(argNode->getTextContent());
                std::string lineStr = std::string(charValue);
				xercesc::XMLString::release(&charValue);
				
				// Split segments based on space's and new lines using boost
				typedef boost::tokenizer<boost::char_separator<char> > 
				tokenizer;
				boost::char_separator<char> sep(" ", "\n");
				tokenizer tokens(lineStr, sep);
				int j = 0;
				for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) 
				{
					
					if(*tok_iter != "\n") // If split segment is not a new line tag
					{
						//std::cout << "j = " << j << std::endl;
						//std::cout << "xml " << *tok_iter << std::endl;
						double test = mathUtils.strtodouble(*tok_iter);
						tempCoord->push_back(test); // Store coordinates to array when read in
						//std::cout << "double " << tempCoord->at(j) << std::endl;
						j = j + 1; // Advance itterater if coordinate (and not blank line)
					}
				}
				// Create geos coordinate for start and end of line
				startCoord = new geos::geom::Coordinate(tempCoord->at(0), tempCoord->at(1), tempCoord->at(2));
				endCoord = new geos::geom::Coordinate(tempCoord->at(3), tempCoord->at(4), tempCoord->at(5));
				
				// Construct geos line segment from coordinates
				outLines->push_back(new geos::geom::LineSegment(*startCoord, *endCoord));

				delete tempCoord; // Delete tempory coordinates
			}
			
			// Tidy up
			std::cout << "XML File saved as geos line segments" << std::endl;
			
			parser->release();
			delete errHandler;
			xercesc::XMLString::release(&coordGeomTag);
			xercesc::XMLString::release(&landXMLRootTag);
			xercesc::XMLString::release(&lineTag);
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
	}
	
	RSGISParseLandXML::~RSGISParseLandXML()
	{
	}
	
	
	
}}
