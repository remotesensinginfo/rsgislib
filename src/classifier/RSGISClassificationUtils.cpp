/*
 *  RSGISClassificationUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/11/2008.
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

#include "RSGISClassificationUtils.h"


namespace rsgis{ namespace classifier{

	RSGISClassificationUtils::RSGISClassificationUtils()
	{
		
	}

	ClassData* RSGISClassificationUtils::convertROIs2ClassData(enviroi *enviroi, int id)
	{
		RSGISMatrices matrixUtils;
		ClassData *classData = new ClassData();
		classData->className = enviroi->name;
		classData->classID = id;
		classData->data = matrixUtils.copyMatrix(enviroi->data);
		return classData;
	}

	ClassData** RSGISClassificationUtils::parseClassificationInputFile(string inputFile, int *numClasses) throw(RSGISClassificationException, RSGISInputStreamException, RSGISMatricesException)
	{
		RSGISMathsUtils mathsUtils;
		RSGISMatrices matrixUtils;
		
		XMLCh tempStr[100];
		DOMImplementation *impl = NULL;
		DOMLSParser* parser = NULL;
		ErrorHandler* errHandler = NULL;
		DOMDocument *doc = NULL;
		DOMElement *rootClassificationElement = NULL;
		DOMNodeList *classesNode = NULL;
		DOMElement *classesElement = NULL;
		DOMNodeList *classNodesList = NULL;
		DOMElement *classElement = NULL;
		const XMLCh *className = NULL;
		const XMLCh *classID = NULL;
		const XMLCh *classMatrix = NULL;
		
		ClassData **classData = NULL;
		
		try 
		{
			XMLPlatformUtils::Initialize();
			
			XMLString::transcode("LS", tempStr, 99);
			impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
			if(impl == NULL)
			{
				throw RSGISClassificationException("DOMImplementation is NULL");
			}
			// Create Parser
			parser = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
			errHandler = (ErrorHandler*) new HandlerBase();
			parser->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler, errHandler);

			// Open Document
			doc = parser->parseURI(inputFile.c_str());	
			
			// Get the Root element
			rootClassificationElement = doc->getDocumentElement();
			//cout << "Root Element: " << XMLString::transcode(rootClassificationElement->getTagName()) << endl;
			if(!XMLString::equals(rootClassificationElement->getTagName(), XMLString::transcode("classification")))
			{
				throw RSGISClassificationException("Incorrect root element; Root element should be \"classification\"");
			}
			
			classesNode = rootClassificationElement->getElementsByTagName(XMLString::transcode("classes"));
			if(classesNode->getLength() != 1)
			{
				throw RSGISClassificationException("There should be only 1 classes node");
			}
			classesElement = static_cast<DOMElement*>(classesNode->item(0));
			
			classNodesList = classesElement->getElementsByTagName(XMLString::transcode("class"));
			*numClasses = classNodesList->getLength();
			//cout << "There are " << *numClasses << "class nodes." << endl;
			
			classData = new ClassData*[*numClasses];
			for(int i = 0; i < *numClasses; i++)
			{
				classData[i] = new ClassData();
				classElement = static_cast<DOMElement*>(classNodesList->item(i));
				className = classElement->getAttribute(XMLString::transcode("name"));
				//cout << "Found class with name " << XMLString::transcode(className) << endl;
				classData[i]->className = string(XMLString::transcode(className));
				classID = classElement->getAttribute(XMLString::transcode("id"));
				//cout << "Found class with id " << XMLString::transcode(classID) << endl;
				classData[i]->classID = mathsUtils.strtoint(XMLString::transcode(classID));
				classMatrix = classElement->getAttribute(XMLString::transcode("matrix"));
				//cout << "Found class with matrix " << XMLString::transcode(classMatrix) << endl;
				classData[i]->data = matrixUtils.readMatrixFromTxt(string((XMLString::transcode(classMatrix))));
			}
				
			parser->release();
			delete errHandler;
			
			XMLPlatformUtils::Terminate();
		}
		catch (const XMLException& e) 
		{
			char *message = XMLString::transcode(e.getMessage());
			string outMessage =  string("XMLException : ") + string(message);
			throw RSGISClassificationException(outMessage.c_str());
		}
		catch (const DOMException& e) 
		{
			char *message = XMLString::transcode(e.getMessage());
			string outMessage =  string("DOMException : ") + string(message);
			throw RSGISClassificationException(outMessage.c_str());
		}
		catch(RSGISInputStreamException e)
		{
			throw e;
		}
		catch(RSGISClassificationException e)
		{
			throw e;
		}
		catch(RSGISMatricesException e)
		{
			throw e;
		}
		
		/*for(int i = 0; i < *numClasses; i++)
		{
			cout << "Class: " << classData[i]->className << " has ID: " << classData[i]->classID << endl;
		}*/
		
		return classData;
	}
	
	void RSGISClassificationUtils::convertShapeFile2SpecLib(string vector, string outputFile, string classAttribute, std::vector<std::string> *attributes, bool group) throw(RSGISClassificationException)
	{
		OGRRegisterAll();
		
		RSGISVectorUtils vecUtils;
		RSGISMatrices matrixUtils;
		
		string vectorLayerName = vecUtils.getLayerName(vector);
		
		OGRDataSource *inputSHPDS = NULL;
		OGRLayer *inputSHPLayer = NULL;

		try
		{
			/////////////////////////////////////
			//
			// Open Input Shapfile.
			//
			/////////////////////////////////////
			inputSHPDS = OGRSFDriverRegistrar::Open(vector.c_str(), FALSE);
			if(inputSHPDS == NULL)
			{
				string message = string("Could not open vector file ") + vector;
				throw RSGISFileException(message.c_str());
			}
			
			string sqlStatment = string("SELECT * FROM ") + vectorLayerName + string(" ORDER BY ") + classAttribute + string(" ASC");
			
			inputSHPLayer = inputSHPDS->ExecuteSQL(sqlStatment.c_str(), NULL, NULL);
			if(inputSHPLayer == NULL)
			{
				string message = string("Could not open vector layer ") + vectorLayerName + string(" with SQL statment: \'") + sqlStatment + string("\'");
				throw RSGISFileException(message.c_str());
			}
			
			
			std::vector<std::string>::iterator iterAttributes;
			unsigned int numAttributes = attributes->size();
			
			if(group)
			{
				std::vector<std::string> *classes = vecUtils.findUniqueVals(inputSHPLayer, classAttribute);
				std::vector<std::string>::iterator iterClasses;
				unsigned int numClasses = classes->size();
				Matrix *specLib = matrixUtils.createMatrix(numAttributes, numClasses);
				
				unsigned int classCount = 0;
				unsigned int featureCount = 0;
				unsigned int attributeCount = 0;
				
				inputSHPDS->ReleaseResultSet(inputSHPLayer);
				
				for(iterClasses = classes->begin(); iterClasses != classes->end(); ++iterClasses)
				{
					cout << "Processing class " << *iterClasses << endl;
					string sqlStatment = string("SELECT * FROM ") + vectorLayerName + string(" WHERE ") + classAttribute + string(" = ") + (*iterClasses);
					inputSHPLayer = inputSHPDS->ExecuteSQL(sqlStatment.c_str(), NULL, NULL);
					if(inputSHPLayer == NULL)
					{
						string message = string("Could not open vector layer ") + vectorLayerName + string(" with SQL statment: \'") + sqlStatment + string("\'");
						throw RSGISFileException(message.c_str());
					}
					
					featureCount = 0;
					
					OGRFeature *feature = NULL;
					OGRFeatureDefn *featureDefn = inputSHPLayer->GetLayerDefn();
					
					inputSHPLayer->ResetReading();
					while( (feature = inputSHPLayer->GetNextFeature()) != NULL )
					{						
						attributeCount = 0;
						for(iterAttributes = attributes->begin(); iterAttributes != attributes->end(); ++iterAttributes)
						{
							double attrVal = feature->GetFieldAsDouble(featureDefn->GetFieldIndex((*iterAttributes).c_str()));
							specLib->matrix[(classCount*numAttributes)+attributeCount] += attrVal;
							++attributeCount;
						}
						
						++featureCount;
						OGRFeature::DestroyFeature(feature);
					}
					
					attributeCount = 0;
					for(iterAttributes = attributes->begin(); iterAttributes != attributes->end(); ++iterAttributes)
					{
						specLib->matrix[(classCount*numAttributes)+attributeCount] = specLib->matrix[(classCount*numAttributes)+attributeCount]/featureCount;
						++attributeCount;
					}
					
					
					++classCount;
				}
				
				matrixUtils.saveMatrix2txt(specLib, outputFile);
				matrixUtils.freeMatrix(specLib);
			}
			else 
			{
				unsigned int numFeatures = inputSHPLayer->GetFeatureCount(true);
				Matrix *specLib = matrixUtils.createMatrix(numAttributes, numFeatures);
				
				unsigned int featureCount = 0;
				unsigned int attributeCount = 0;
				
				OGRFeature *feature = NULL;
				OGRFeatureDefn *featureDefn = inputSHPLayer->GetLayerDefn();
				
				inputSHPLayer->ResetReading();
				while( (feature = inputSHPLayer->GetNextFeature()) != NULL )
				{
					string attrStrVal = feature->GetFieldAsString(featureDefn->GetFieldIndex(classAttribute.c_str()));
					cout << "Class " << featureCount << " is " << attrStrVal << endl;
					
					attributeCount = 0;
					for(iterAttributes = attributes->begin(); iterAttributes != attributes->end(); ++iterAttributes)
					{
						double attrVal = feature->GetFieldAsDouble(featureDefn->GetFieldIndex((*iterAttributes).c_str()));
						specLib->matrix[(featureCount*numAttributes)+attributeCount] = attrVal;
						++attributeCount;
					}
					
					++featureCount;
					OGRFeature::DestroyFeature(feature);
				}
				
				matrixUtils.saveMatrix2txt(specLib, outputFile);
				matrixUtils.freeMatrix(specLib);
			}
			
			OGRDataSource::DestroyDataSource(inputSHPDS);
			OGRCleanupAll();			
		}
		catch(RSGISClassificationException &e)
		{
			throw e;
		}
		catch(RSGISException &e)
		{
			throw RSGISClassificationException(e.what());
		}
	}

	RSGISClassificationUtils::~RSGISClassificationUtils()
	{
		
	}

}}


