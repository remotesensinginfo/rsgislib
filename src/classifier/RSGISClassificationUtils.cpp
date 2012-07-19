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

	ClassData* RSGISClassificationUtils::convertROIs2ClassData(rsgis::utils::enviroi *enviroi, int id)
	{
        rsgis::math::RSGISMatrices matrixUtils;
		ClassData *classData = new ClassData();
		classData->className = enviroi->name;
		classData->classID = id;
		classData->data = matrixUtils.copyMatrix(enviroi->data);
		return classData;
	}

	ClassData** RSGISClassificationUtils::parseClassificationInputFile(std::string inputFile, int *numClasses) throw(RSGISClassificationException, rsgis::RSGISInputStreamException, rsgis::math::RSGISMatricesException)
	{
        rsgis::math::RSGISMathsUtils mathsUtils;
        rsgis::math::RSGISMatrices matrixUtils;
		
		XMLCh tempStr[100];
        xercesc::DOMImplementation *impl = NULL;
		xercesc::DOMLSParser* parser = NULL;
		xercesc::ErrorHandler* errHandler = NULL;
		xercesc::DOMDocument *doc = NULL;
		xercesc::DOMElement *rootClassificationElement = NULL;
		xercesc::DOMNodeList *classesNode = NULL;
		xercesc::DOMElement *classesElement = NULL;
		xercesc::DOMNodeList *classNodesList = NULL;
		xercesc::DOMElement *classElement = NULL;
		const XMLCh *className = NULL;
		const XMLCh *classID = NULL;
		const XMLCh *classMatrix = NULL;
		
		ClassData **classData = NULL;
		
		try 
		{
			xercesc::XMLPlatformUtils::Initialize();
			
			xercesc::XMLString::transcode("LS", tempStr, 99);
			impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
			if(impl == NULL)
			{
				throw RSGISClassificationException("DOMImplementation is NULL");
			}
			// Create Parser
			parser = ((xercesc::DOMImplementationLS*)impl)->createLSParser(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
			errHandler = (xercesc::ErrorHandler*) new xercesc::HandlerBase();
			parser->getDomConfig()->setParameter(xercesc::XMLUni::fgDOMErrorHandler, errHandler);

			// Open Document
			doc = parser->parseURI(inputFile.c_str());	
			
			// Get the Root element
			rootClassificationElement = doc->getDocumentElement();
			//std::cout << "Root Element: " << XMLString::transcode(rootClassificationElement->getTagName()) << std::endl;
			if(!xercesc::XMLString::equals(rootClassificationElement->getTagName(), xercesc::XMLString::transcode("classification")))
			{
				throw RSGISClassificationException("Incorrect root element; Root element should be \"classification\"");
			}
			
			classesNode = rootClassificationElement->getElementsByTagName(xercesc::XMLString::transcode("classes"));
			if(classesNode->getLength() != 1)
			{
				throw RSGISClassificationException("There should be only 1 classes node");
			}
			classesElement = static_cast<xercesc::DOMElement*>(classesNode->item(0));
			
			classNodesList = classesElement->getElementsByTagName(xercesc::XMLString::transcode("class"));
			*numClasses = classNodesList->getLength();
			//std::cout << "There are " << *numClasses << "class nodes." << std::endl;
			
			classData = new ClassData*[*numClasses];
			for(int i = 0; i < *numClasses; i++)
			{
				classData[i] = new ClassData();
				classElement = static_cast<xercesc::DOMElement*>(classNodesList->item(i));
				className = classElement->getAttribute(xercesc::XMLString::transcode("name"));
				//std::cout << "Found class with name " << XMLString::transcode(className) << std::endl;
				classData[i]->className = std::string(xercesc::XMLString::transcode(className));
				classID = classElement->getAttribute(xercesc::XMLString::transcode("id"));
				//std::cout << "Found class with id " << XMLString::transcode(classID) << std::endl;
				classData[i]->classID = mathsUtils.strtoint(xercesc::XMLString::transcode(classID));
				classMatrix = classElement->getAttribute(xercesc::XMLString::transcode("matrix"));
				//std::cout << "Found class with matrix " << XMLString::transcode(classMatrix) << std::endl;
				classData[i]->data = matrixUtils.readMatrixFromTxt(std::string((xercesc::XMLString::transcode(classMatrix))));
			}
				
			parser->release();
			delete errHandler;
			
			xercesc::XMLPlatformUtils::Terminate();
		}
		catch (const xercesc::XMLException& e) 
		{
			char *message = xercesc::XMLString::transcode(e.getMessage());
			std::string outMessage =  std::string("XMLException : ") + std::string(message);
			throw RSGISClassificationException(outMessage.c_str());
		}
		catch (const xercesc::DOMException& e) 
		{
			char *message = xercesc::XMLString::transcode(e.getMessage());
			std::string outMessage =  std::string("DOMException : ") + std::string(message);
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
		catch(rsgis::math::RSGISMatricesException e)
		{
			throw e;
		}
		
		/*for(int i = 0; i < *numClasses; i++)
		{
			std::cout << "Class: " << classData[i]->className << " has ID: " << classData[i]->classID << std::endl;
		}*/
		
		return classData;
	}
	
	void RSGISClassificationUtils::convertShapeFile2SpecLib(std::string vector, std::string outputFile, std::string classAttribute, std::vector<std::string> *attributes, bool group) throw(RSGISClassificationException)
	{
		OGRRegisterAll();
		
        rsgis::vec::RSGISVectorUtils vecUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		std::string vectorLayerName = vecUtils.getLayerName(vector);
		
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
				std::string message = std::string("Could not open vector file ") + vector;
				throw RSGISFileException(message.c_str());
			}
			
			std::string sqlStatment = std::string("SELECT * FROM ") + vectorLayerName + std::string(" ORDER BY ") + classAttribute + std::string(" ASC");
			
			inputSHPLayer = inputSHPDS->ExecuteSQL(sqlStatment.c_str(), NULL, NULL);
			if(inputSHPLayer == NULL)
			{
				std::string message = std::string("Could not open vector layer ") + vectorLayerName + std::string(" with SQL statment: \'") + sqlStatment + std::string("\'");
				throw RSGISFileException(message.c_str());
			}
			
			
			std::vector<std::string>::iterator iterAttributes;
			unsigned int numAttributes = attributes->size();
			
			if(group)
			{
				std::vector<std::string> *classes = vecUtils.findUniqueVals(inputSHPLayer, classAttribute);
				std::vector<std::string>::iterator iterClasses;
				unsigned int numClasses = classes->size();
                rsgis::math::Matrix *specLib = matrixUtils.createMatrix(numAttributes, numClasses);
				
				unsigned int classCount = 0;
				unsigned int featureCount = 0;
				unsigned int attributeCount = 0;
				
				inputSHPDS->ReleaseResultSet(inputSHPLayer);
				
				for(iterClasses = classes->begin(); iterClasses != classes->end(); ++iterClasses)
				{
					std::cout << "Processing class " << *iterClasses << std::endl;
					std::string sqlStatment = std::string("SELECT * FROM ") + vectorLayerName + std::string(" WHERE ") + classAttribute + std::string(" = ") + (*iterClasses);
					inputSHPLayer = inputSHPDS->ExecuteSQL(sqlStatment.c_str(), NULL, NULL);
					if(inputSHPLayer == NULL)
					{
						std::string message = std::string("Could not open vector layer ") + vectorLayerName + std::string(" with SQL statment: \'") + sqlStatment + std::string("\'");
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
                rsgis::math::Matrix *specLib = matrixUtils.createMatrix(numAttributes, numFeatures);
				
				unsigned int featureCount = 0;
				unsigned int attributeCount = 0;
				
				OGRFeature *feature = NULL;
				OGRFeatureDefn *featureDefn = inputSHPLayer->GetLayerDefn();
				
				inputSHPLayer->ResetReading();
				while( (feature = inputSHPLayer->GetNextFeature()) != NULL )
				{
					std::string attrStrVal = feature->GetFieldAsString(featureDefn->GetFieldIndex(classAttribute.c_str()));
					std::cout << "Class " << featureCount << " is " << attrStrVal << std::endl;
					
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


