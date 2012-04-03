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
	
	void RSGISParseLandXML::getLandXMLLine(string xmlFile, vector<LineSegment*> *outLines)
	{
		this->xmlFile = xmlFile;
		XMLCh tempStr[100];
		DOMImplementation *impl = NULL;
		DOMLSParser* parser = NULL;
		ErrorHandler* errHandler = NULL;
		DOMDocument *doc = NULL;
		DOMElement *rootElement = NULL;
		DOMNodeList *linesList = NULL;
		//DOMNodeList *singlelineList = NULL;
		DOMNode *argNode = NULL;
		//DOMNode *lineStartNode = NULL;
		//DOMNode *lineEndNode = NULL;
		XMLCh *landXMLRootTag = NULL;
		XMLCh *coordGeomTag = NULL;
		XMLCh *lineTag = NULL;
		XMLCh *lineStartTag = NULL;
		XMLCh *lineEndTag = NULL;
		string lineStart = "";
		Coordinate *startCoord;
		Coordinate *endCoord;
		int numLines = 0;
		
		try
		{
			XMLPlatformUtils::Initialize();
			RSGISMathsUtils mathUtils;
			landXMLRootTag = XMLString::transcode("LandXML"); // LandXML root tag
			coordGeomTag = XMLString::transcode("CoordGeom"); // Coordinate Geometry Tag
			lineTag = XMLString::transcode("Line"); // Line Tag
			lineStartTag = XMLString::transcode("Start"); // Start of line Tag
			lineEndTag = XMLString::transcode("End"); // End of line Tag
			
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
			if(!XMLString::equals(rootElement->getTagName(), landXMLRootTag))
			{
				throw RSGISXMLArgumentsException("XML file not in valid LandXML format");
			}
			
			linesList =  rootElement->getElementsByTagName(lineTag);
			numLines = linesList->getLength();
			
			cout << "The file contains " << numLines << " lines" << endl;
			
			for(int i = 0; i < numLines; i++)
			{
				/* Get all the text elements within each line tag (6 numbers)
				 * and split them by spaces, using the Boost tokenizer
				 */
				
				vector <double> *tempCoord = new vector<double>(); // Set up tempory vector to hold the coordinates
				startCoord = NULL;
				endCoord = NULL;
				argNode = static_cast<DOMElement*>(linesList->item(i));
				
				// Get all text elements between <Line> and </Line> tag
				char *charValue = XMLString::transcode(argNode->getTextContent());
				string lineStr = string(charValue);
				XMLString::release(&charValue);
				
				// Split segments based on space's and new lines using boost
				typedef boost::tokenizer<boost::char_separator<char> > 
				tokenizer;
				char_separator<char> sep(" ", "\n");
				tokenizer tokens(lineStr, sep);
				int j = 0;
				for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) 
				{
					
					if(*tok_iter != "\n") // If split segment is not a new line tag
					{
						//cout << "j = " << j << endl;
						//cout << "xml " << *tok_iter << endl;
						double test = mathUtils.strtodouble(*tok_iter);
						tempCoord->push_back(test); // Store coordinates to array when read in
						//cout << "double " << tempCoord->at(j) << endl;
						j = j + 1; // Advance itterater if coordinate (and not blank line)
					}
				}
				// Create geos coordinate for start and end of line
				startCoord = new Coordinate(tempCoord->at(0), tempCoord->at(1), tempCoord->at(2));
				endCoord = new Coordinate(tempCoord->at(3), tempCoord->at(4), tempCoord->at(5));
				
				// Construct geos line segment from coordinates
				outLines->push_back(new LineSegment(*startCoord, *endCoord));

				delete tempCoord; // Delete tempory coordinates
			}
			
			// Tidy up
			cout << "XML File saved as geos line segments" << endl;
			
			parser->release();
			delete errHandler;
			XMLString::release(&coordGeomTag);
			XMLString::release(&landXMLRootTag);
			XMLString::release(&lineTag);
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
	}
	
	RSGISParseLandXML::~RSGISParseLandXML()
	{
	}
	
	
	
}}
