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
			
			classData = new ClassData*[*numClasses];
			for(int i = 0; i < *numClasses; i++)
			{
				classData[i] = new ClassData();
				classElement = static_cast<xercesc::DOMElement*>(classNodesList->item(i));
				className = classElement->getAttribute(xercesc::XMLString::transcode("name"));

                classData[i]->className = std::string(xercesc::XMLString::transcode(className));
				classID = classElement->getAttribute(xercesc::XMLString::transcode("id"));

                classData[i]->classID = mathsUtils.strtoint(xercesc::XMLString::transcode(classID));
				classMatrix = classElement->getAttribute(xercesc::XMLString::transcode("matrix"));

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
		
		return classData;
	}
	
	void RSGISClassificationUtils::convertShapeFile2SpecLib(std::string vector, std::string outputFile, std::string classAttribute, std::vector<std::string> *attributes, bool group) throw(RSGISClassificationException)
	{
		OGRRegisterAll();
		
        rsgis::vec::RSGISVectorUtils vecUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		std::string vectorLayerName = vecUtils.getLayerName(vector);
		
		GDALDataset *inputSHPDS = NULL;
		OGRLayer *inputSHPLayer = NULL;

		try
		{
			/////////////////////////////////////
			//
			// Open Input Shapfile.
			//
			/////////////////////////////////////
			inputSHPDS = (GDALDataset*) GDALOpenEx(vector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL );
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
			
			GDALClose(inputSHPDS);
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
    
    
    RSGISEliminateSingleClassPixels::RSGISEliminateSingleClassPixels()
    {
        
    }
    
    void RSGISEliminateSingleClassPixels::eliminate(GDALDataset *inImageData, GDALDataset *tmpData, std::string outputImage, float noDataVal, bool noDataValProvided, std::string format, rsgis::img::RSGISRasterConnectivity filterConnectivity)throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            // Check images have the same size!
            if(inImageData->GetRasterXSize() != tmpData->GetRasterXSize())
            {
                throw rsgis::img::RSGISImageCalcException("Widths are not the same (spectral and temp)");
            }
            if(inImageData->GetRasterYSize() != tmpData->GetRasterYSize())
            {
                throw rsgis::img::RSGISImageCalcException("Heights are not the same (spectral and temp)");
            }
            
            GDALDataset *outData = NULL;
            rsgis::img::RSGISImageUtils imgUtils;
            outData = imgUtils.createCopy(inImageData, outputImage, format, inImageData->GetRasterBand(1)->GetRasterDataType());
            imgUtils.copyUIntGDALDataset(inImageData, outData);
            
            unsigned long singlesCount = 0;
            bool singlesRemoved = false;
            
            while(!singlesRemoved)
            {
                if(filterConnectivity == rsgis::img::rsgis_4connect)
                {
                    singlesCount = this->findSinglePixelsConnect4(outData, tmpData, noDataVal, noDataValProvided);
                }
                else if(filterConnectivity == rsgis::img::rsgis_8connect)
                {
                    singlesCount = this->findSinglePixelsConnect8(outData, tmpData, noDataVal, noDataValProvided);
                }
                else
                {
                    throw rsgis::img::RSGISImageCalcException("Connectivity not recoginised (Only 4 or 8 are valid inputs)");
                }
                
                if(singlesCount > 0)
                {
                    std::cout << "There are " << singlesCount << " single pixels within the image\n";
                    if(filterConnectivity == rsgis::img::rsgis_4connect)
                    {
                        if(!eliminateSinglePixelsConnect4(outData, tmpData, outData, noDataVal, noDataValProvided))
                        {
                            singlesRemoved = true;
                            break;
                        }
                    }
                    else//if(filterConnectivity == rsgis::img::rsgis_8connect)
                    {
                        if(!eliminateSinglePixelsConnect8(outData, tmpData, outData, noDataVal, noDataValProvided))
                        {
                            singlesRemoved = true;
                            break;
                        }
                    }
                }
                else
                {
                    singlesRemoved = true;
                    break;
                }
            }
            std::cout << "Complete, all connected single pixels have been removed\n";
            
            GDALClose(outData);
            
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(RSGISImageException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    unsigned long RSGISEliminateSingleClassPixels::findSinglePixelsConnect4(GDALDataset *inImageData, GDALDataset *tmpData, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException)
    {
        unsigned long countSingles = 0;
        try
        {
            unsigned int width = inImageData->GetRasterXSize();
            unsigned int height = inImageData->GetRasterYSize();
            
            GDALRasterBand *clumpBand = inImageData->GetRasterBand(1);
            GDALRasterBand *tmpBand = tmpData->GetRasterBand(1);
            
            unsigned int **inData = new unsigned int*[3];
            inData[0] = new unsigned int[width];
            inData[1] = new unsigned int[width];
            inData[2] = new unsigned int[width];
            unsigned int *outData = new unsigned int[width];
            bool notSingle = false;
            
            int feedback = height/10;
            int feedbackCounter = 0;
            std::cout << "Started" << std::flush;
            for(unsigned int i = 0; i < height; ++i)
            {
                if((i % feedback) == 0)
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                if(i == 0)
                {
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, inData[2], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        notSingle = false;
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(j == 0)
                            {
                                //right
                                if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                            }
                            else if(j == (width-1))
                            {
                                //left
                                if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                            }
                            else
                            {
                                //left
                                if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                            }
                        }
                        else
                        {
                            notSingle = true;
                        }
                        
                        if(notSingle)
                        {
                            outData[j] = 0;
                        }
                        else
                        {
                            outData[j] = 1;
                            ++countSingles;
                        }
                    }
                }
                else if(i == (height-1))
                {
                    clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, inData[0], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        notSingle = false;
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(j == 0)
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                            else if(j == (width-1))
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //left
                                else if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                            else
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //left
                                else if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                        }
                        else
                        {
                            notSingle = true;
                        }
                        
                        if(notSingle)
                        {
                            outData[j] = 0;
                        }
                        else
                        {
                            outData[j] = 1;
                            ++countSingles;
                        }
                    }
                }
                else
                {
                    clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, inData[0], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, inData[2], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        notSingle = false;
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(j == 0)
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                            }
                            else if(j == (width-1))
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //left
                                else if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                            }
                            else
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //left
                                else if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                            }
                        }
                        else
                        {
                            notSingle = true;
                        }
                        
                        if(notSingle)
                        {
                            outData[j] = 0;
                        }
                        else
                        {
                            outData[j] = 1;
                            ++countSingles;
                        }
                    }
                }
                
                tmpBand->RasterIO(GF_Write, 0, i, width, 1, outData, width, 1, GDT_UInt32, 0, 0);
            }
            std::cout << ". Complete\n";
            
            delete[] inData[0];
            delete[] inData[1];
            delete[] inData[2];
            delete[] inData;
            delete[] outData;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        
        return countSingles;
    }
    
    bool RSGISEliminateSingleClassPixels::eliminateSinglePixelsConnect4(GDALDataset *inImageData, GDALDataset *tmpData, GDALDataset *outDataset, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException)
    {
        bool hasChangeOccured = false;
        try
        {
            unsigned int width = inImageData->GetRasterXSize();
            unsigned int height = inImageData->GetRasterYSize();
            
            GDALRasterBand *clumpBand = inImageData->GetRasterBand(1);
            GDALRasterBand *singlePxlBand = tmpData->GetRasterBand(1);
            GDALRasterBand *outBand = outDataset->GetRasterBand(1);
            
            unsigned int **inData = new unsigned int*[3];
            inData[0] = new unsigned int[width];
            inData[1] = new unsigned int[width];
            inData[2] = new unsigned int[width];
            
            unsigned int **singlePxl = new unsigned int*[3];
            singlePxl[0] = new unsigned int[width];
            singlePxl[1] = new unsigned int[width];
            singlePxl[2] = new unsigned int[width];
            
            rsgis::datastruct::SortedGenericList<unsigned int> *sortedList = new rsgis::datastruct::SortedGenericList<unsigned int>();
            
            unsigned int *outData = new unsigned int[width];
            
            int feedback = height/10;
            int feedbackCounter = 0;
            std::cout << "Started" << std::flush;
            for(unsigned int i = 0; i < height; ++i)
            {
                if((i % feedback) == 0)
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                if(i == 0)
                {
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, inData[2], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i, width, 1, singlePxl[1], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i+1, width, 1, singlePxl[2], width, 1, GDT_UInt32, 0, 0);
                    
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(singlePxl[1][j] == 1)
                            {
                                if(j == 0)
                                {
                                    //right
                                    if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        sortedList->add(&inData[1][j+1]);
                                    }
                                    //bottom
                                    if(singlePxl[2][j] == 0)
                                    {
                                        sortedList->add(&inData[2][j]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                                else if(j == (width-1))
                                {
                                    //left
                                    if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        sortedList->add(&inData[1][j-1]);
                                    }
                                    //bottom
                                    if(singlePxl[2][j] == 0)
                                    {
                                        sortedList->add(&inData[2][j]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                                else
                                {
                                    //left
                                    if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        sortedList->add(&inData[1][j-1]);
                                    }
                                    //right
                                    if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        sortedList->add(&inData[1][j+1]);
                                    }
                                    //bottom
                                    if(singlePxl[2][j] == 0)
                                    {
                                        sortedList->add(&inData[2][j]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                            }
                            else
                            {
                                outData[j] = inData[1][j];
                            }
                        }
                        else
                        {
                            outData[j] = inData[1][j];
                        }
                    }
                    
                }
                else if(i == (height-1))
                {
                    clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, inData[0], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i-1, width, 1, singlePxl[0], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i, width, 1, singlePxl[1], width, 1, GDT_UInt32, 0, 0);
                    
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(singlePxl[1][j] == 1)
                            {
                                if(j == 0)
                                {
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        sortedList->add(&inData[0][j]);
                                    }
                                    //right
                                    if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        sortedList->add(&inData[1][j+1]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                                else if(j == (width-1))
                                {
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        sortedList->add(&inData[0][j]);
                                    }
                                    //left
                                    if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        sortedList->add(&inData[1][j-1]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                                else
                                {
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        sortedList->add(&inData[0][j]);
                                    }
                                    //left
                                    if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        sortedList->add(&inData[1][j-1]);
                                    }
                                    //right
                                    if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        sortedList->add(&inData[1][j+1]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                            }
                            else
                            {
                                outData[j] = inData[1][j];
                            }
                        }
                        else
                        {
                            outData[j] = inData[1][j];
                        }
                    }
                }
                else
                {
                    clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, inData[0], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, inData[2], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i-1, width, 1, singlePxl[0], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i, width, 1, singlePxl[1], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i+1, width, 1, singlePxl[2], width, 1, GDT_UInt32, 0, 0);
                    
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(singlePxl[1][j] == 1)
                            {
                                if(j == 0)
                                {
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        sortedList->add(&inData[0][j]);
                                    }
                                    //right
                                    if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        sortedList->add(&inData[1][j+1]);
                                    }
                                    //bottom
                                    if(singlePxl[2][j] == 0)
                                    {
                                        sortedList->add(&inData[2][j]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                                else if(j == (width-1))
                                {
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        sortedList->add(&inData[0][j]);
                                    }
                                    //left
                                    if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        sortedList->add(&inData[1][j-1]);
                                    }
                                    //bottom
                                    if(singlePxl[2][j] == 0)
                                    {
                                        sortedList->add(&inData[2][j]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                                else
                                {
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        sortedList->add(&inData[0][j]);
                                    }
                                    //left
                                    if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        sortedList->add(&inData[1][j-1]);
                                    }
                                    //right
                                    if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        sortedList->add(&inData[1][j+1]);
                                    }
                                    //bottom
                                    if(singlePxl[2][j] == 0)
                                    {
                                        sortedList->add(&inData[2][j]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                            }
                            else
                            {
                                outData[j] = inData[1][j];
                            }
                        }
                        else
                        {
                            outData[j] = inData[1][j];
                        }
                    }
                }
                
                outBand->RasterIO(GF_Write, 0, i, width, 1, outData, width, 1, GDT_UInt32, 0, 0);
            }
            std::cout << ". Complete\n";
            
            delete sortedList;
            delete[] inData[0];
            delete[] inData[1];
            delete[] inData[2];
            delete[] inData;
            delete[] singlePxl[0];
            delete[] singlePxl[1];
            delete[] singlePxl[2];
            delete[] singlePxl;
            delete[] outData;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        return hasChangeOccured;
    }
    
    unsigned long RSGISEliminateSingleClassPixels::findSinglePixelsConnect8(GDALDataset *inImageData, GDALDataset *tmpData, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException)
    {
        unsigned long countSingles = 0;
        try
        {
            unsigned int width = inImageData->GetRasterXSize();
            unsigned int height = inImageData->GetRasterYSize();
            
            GDALRasterBand *clumpBand = inImageData->GetRasterBand(1);
            GDALRasterBand *tmpBand = tmpData->GetRasterBand(1);
            
            unsigned int **inData = new unsigned int*[3];
            inData[0] = new unsigned int[width];
            inData[1] = new unsigned int[width];
            inData[2] = new unsigned int[width];
            unsigned int *outData = new unsigned int[width];
            bool notSingle = false;
            
            int feedback = height/10;
            int feedbackCounter = 0;
            std::cout << "Started" << std::flush;
            for(unsigned int i = 0; i < height; ++i)
            {
                if((i % feedback) == 0)
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                if(i == 0)
                {
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, inData[2], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        notSingle = false;
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(j == 0)
                            {
                                //right
                                if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //bottom right
                                else if((j < width-1) && (inData[2][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                            else if(j == (width-1))
                            {
                                //left
                                if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //bottom left
                                else if((j > 0) && (inData[2][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                            else
                            {
                                //left
                                if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom left
                                else if((j > 0) && (inData[2][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //bottom right
                                else if((j < width-1) && (inData[2][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                        }
                        else
                        {
                            notSingle = true;
                        }
                        
                        if(notSingle)
                        {
                            outData[j] = 0;
                        }
                        else
                        {
                            outData[j] = 1;
                            ++countSingles;
                        }
                    }
                }
                else if(i == (height-1))
                {
                    clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, inData[0], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        notSingle = false;
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(j == 0)
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //top right
                                else if((j < width-1) && (inData[0][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                            else if(j == (width-1))
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //top left
                                else if((j > 0) && (inData[0][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //left
                                else if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                            else
                            {
                                //top left
                                if((j > 0) && (inData[0][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //top
                                else if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //top right
                                else if((j < width-1) && (inData[0][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //left
                                else if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                        }
                        else
                        {
                            notSingle = true;
                        }
                        
                        if(notSingle)
                        {
                            outData[j] = 0;
                        }
                        else
                        {
                            outData[j] = 1;
                            ++countSingles;
                        }
                    }
                }
                else
                {
                    clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, inData[0], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, inData[2], width, 1, GDT_UInt32, 0, 0);
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        notSingle = false;
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(j == 0)
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //top right
                                else if((j < width-1) && (inData[0][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //bottom right
                                else if((j < width-1) && (inData[2][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                            else if(j == (width-1))
                            {
                                //top
                                if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //top left
                                else if((j > 0) && (inData[0][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //left
                                else if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //bottom left
                                else if((j > 0) && (inData[2][j] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                            else
                            {
                                //top left
                                if((j > 0) && (inData[0][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //top
                                else if(inData[0][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //top right
                                else if((j < width-1) && (inData[0][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //left
                                else if((j > 0) && (inData[1][j-1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //right
                                else if((j < width-1) && (inData[1][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom left
                                else if((j > 0) && (inData[2][j] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                                //bottom
                                else if(inData[2][j] == inData[1][j])
                                {
                                    notSingle = true;
                                }
                                //bottom right
                                else if((j < width-1) && (inData[2][j+1] == inData[1][j]))
                                {
                                    notSingle = true;
                                }
                            }
                        }
                        else
                        {
                            notSingle = true;
                        }
                        
                        if(notSingle)
                        {
                            outData[j] = 0;
                        }
                        else
                        {
                            outData[j] = 1;
                            ++countSingles;
                        }
                    }
                }
                
                tmpBand->RasterIO(GF_Write, 0, i, width, 1, outData, width, 1, GDT_UInt32, 0, 0);
            }
            std::cout << ". Complete\n";
            
            delete[] inData[0];
            delete[] inData[1];
            delete[] inData[2];
            delete[] inData;
            delete[] outData;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        
        return countSingles;
    }
    
    bool RSGISEliminateSingleClassPixels::eliminateSinglePixelsConnect8(GDALDataset *inImageData, GDALDataset *tmpData, GDALDataset *outDataset, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException)
    {
        bool hasChangeOccured = false;
        try
        {
            unsigned int width = inImageData->GetRasterXSize();
            unsigned int height = inImageData->GetRasterYSize();
            
            GDALRasterBand *clumpBand = inImageData->GetRasterBand(1);
            GDALRasterBand *singlePxlBand = tmpData->GetRasterBand(1);
            GDALRasterBand *outBand = outDataset->GetRasterBand(1);
            
            unsigned int **inData = new unsigned int*[3];
            inData[0] = new unsigned int[width];
            inData[1] = new unsigned int[width];
            inData[2] = new unsigned int[width];
            
            unsigned int **singlePxl = new unsigned int*[3];
            singlePxl[0] = new unsigned int[width];
            singlePxl[1] = new unsigned int[width];
            singlePxl[2] = new unsigned int[width];
            
            rsgis::datastruct::SortedGenericList<unsigned int> *sortedList = new rsgis::datastruct::SortedGenericList<unsigned int>();
            
            unsigned int *outData = new unsigned int[width];
            
            int feedback = height/10;
            int feedbackCounter = 0;
            std::cout << "Started" << std::flush;
            for(unsigned int i = 0; i < height; ++i)
            {
                if((i % feedback) == 0)
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                if(i == 0)
                {
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, inData[2], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i, width, 1, singlePxl[1], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i+1, width, 1, singlePxl[2], width, 1, GDT_UInt32, 0, 0);
                    
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(singlePxl[1][j] == 1)
                            {
                                if(j == 0)
                                {
                                    //right
                                    if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        sortedList->add(&inData[1][j+1]);
                                    }
                                    //bottom
                                    else if(singlePxl[2][j] == 0)
                                    {
                                        sortedList->add(&inData[2][j]);
                                    }
                                    //bottom right
                                    else if((j < width-1) && (singlePxl[2][j+1] == 0))
                                    {
                                        sortedList->add(&inData[2][j+1]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                                else if(j == (width-1))
                                {
                                    //left
                                    if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        sortedList->add(&inData[1][j-1]);
                                    }
                                    //bottom left 
                                    else if((j > 0) && (singlePxl[2][j-1] == 0))
                                    {
                                        sortedList->add(&inData[2][j-1]);
                                    }
                                    //bottom
                                    else if(singlePxl[2][j] == 0)
                                    {
                                        sortedList->add(&inData[2][j]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                                else
                                {
                                    //left
                                    if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        sortedList->add(&inData[1][j-1]);
                                    }
                                    //right
                                    else if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        sortedList->add(&inData[1][j+1]);
                                    }
                                    //bottom left
                                    else if((j > 0) && (singlePxl[2][j-1] == 0))
                                    {
                                        sortedList->add(&inData[2][j-1]);
                                    }
                                    //bottom
                                    else if(singlePxl[2][j] == 0)
                                    {
                                        sortedList->add(&inData[2][j]);
                                    }
                                    //bottom right
                                    else if((j < width-1) && (singlePxl[2][j+1] == 0))
                                    {
                                        sortedList->add(&inData[2][j+1]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                            }
                            else
                            {
                                outData[j] = inData[1][j];
                            }
                        }
                        else
                        {
                            outData[j] = inData[1][j];
                        }
                    }
                    
                }
                else if(i == (height-1))
                {
                    clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, inData[0], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i-1, width, 1, singlePxl[0], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i, width, 1, singlePxl[1], width, 1, GDT_UInt32, 0, 0);
                    
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(singlePxl[1][j] == 1)
                            {
                                if(j == 0)
                                {
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        sortedList->add(&inData[0][j]);
                                    }
                                    //top right
                                    if((j < width-1) && (singlePxl[0][j+1] == 0))
                                    {
                                        sortedList->add(&inData[0][j+1]);
                                    }
                                    //right
                                    else if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        sortedList->add(&inData[1][j+1]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                                else if(j == (width-1))
                                {
                                    //top left
                                    if((j > 0) && (singlePxl[0][j-1] == 0))
                                    {
                                        sortedList->add(&inData[0][j-1]);
                                    }
                                    //top
                                    else if(singlePxl[0][j] == 0)
                                    {
                                        sortedList->add(&inData[0][j]);
                                    }
                                    //left
                                    else if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        sortedList->add(&inData[1][j-1]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                                else
                                {
                                    //top left
                                    if((j > 0) && (singlePxl[0][j-1] == 0))
                                    {
                                        sortedList->add(&inData[0][j-1]);
                                    }
                                    //top
                                    else if(singlePxl[0][j] == 0)
                                    {
                                        sortedList->add(&inData[0][j]);
                                    }
                                    //top right
                                    if((j < width-1) && (singlePxl[0][j+1] == 0))
                                    {
                                        sortedList->add(&inData[0][j+1]);
                                    }
                                    //left
                                    else if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        sortedList->add(&inData[1][j-1]);
                                    }
                                    //right
                                    else if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        sortedList->add(&inData[1][j+1]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                            }
                            else
                            {
                                outData[j] = inData[1][j];
                            }
                        }
                        else
                        {
                            outData[j] = inData[1][j];
                        }
                    }
                }
                else
                {
                    clumpBand->RasterIO(GF_Read, 0, i-1, width, 1, inData[0], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i, width, 1, inData[1], width, 1, GDT_UInt32, 0, 0);
                    clumpBand->RasterIO(GF_Read, 0, i+1, width, 1, inData[2], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i-1, width, 1, singlePxl[0], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i, width, 1, singlePxl[1], width, 1, GDT_UInt32, 0, 0);
                    singlePxlBand->RasterIO(GF_Read, 0, i+1, width, 1, singlePxl[2], width, 1, GDT_UInt32, 0, 0);
                    
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        if((noDataValProvided & (inData[1][j] != noDataVal)) | !noDataValProvided)
                        {
                            if(singlePxl[1][j] == 1)
                            {
                                if(j == 0)
                                {
                                    //top
                                    if(singlePxl[0][j] == 0)
                                    {
                                        sortedList->add(&inData[0][j]);
                                    }
                                    //top right
                                    else if((j < width-1) && (singlePxl[0][j+1] == 0))
                                    {
                                        sortedList->add(&inData[0][j+1]);
                                    }
                                    //right
                                    else if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        sortedList->add(&inData[1][j+1]);
                                    }
                                    //bottom
                                    else if(singlePxl[2][j] == 0)
                                    {
                                        sortedList->add(&inData[2][j]);
                                    }
                                    //bottom right
                                    else if(singlePxl[2][j+1] == 0)
                                    {
                                        sortedList->add(&inData[2][j+1]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                                else if(j == (width-1))
                                {
                                    //top left
                                    if((j > 0) && (singlePxl[0][j-1] == 0))
                                    {
                                        sortedList->add(&inData[0][j-1]);
                                    }
                                    //top
                                    else if(singlePxl[0][j] == 0)
                                    {
                                        sortedList->add(&inData[0][j]);
                                    }
                                    //left
                                    else if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        sortedList->add(&inData[1][j-1]);
                                    }
                                    //bottom left
                                    else if((j > 0) && (singlePxl[2][j-1] == 0))
                                    {
                                        sortedList->add(&inData[2][j-1]);
                                    }
                                    //bottom
                                    else if(singlePxl[2][j] == 0)
                                    {
                                        sortedList->add(&inData[2][j]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                                else
                                {
                                    //top left
                                    if((j > 0) && (singlePxl[0][j-1] == 0))
                                    {
                                        sortedList->add(&inData[0][j-1]);
                                    }
                                    //top
                                    else if(singlePxl[0][j] == 0)
                                    {
                                        sortedList->add(&inData[0][j]);
                                    }
                                    //top right
                                    else if((j < width-1) && (singlePxl[0][j+1] == 0))
                                    {
                                        sortedList->add(&inData[0][j+1]);
                                    }
                                    //left
                                    else if((j > 0) && (singlePxl[1][j-1] == 0))
                                    {
                                        sortedList->add(&inData[1][j-1]);
                                    }
                                    //right
                                    else if((j < width-1) && (singlePxl[1][j+1] == 0))
                                    {
                                        sortedList->add(&inData[1][j+1]);
                                    }
                                    //bottom left
                                    else if((j > 0) && (singlePxl[2][j-1] == 0))
                                    {
                                        sortedList->add(&inData[2][j-1]);
                                    }
                                    //bottom
                                    else if(singlePxl[2][j] == 0)
                                    {
                                        sortedList->add(&inData[2][j]);
                                    }
                                    //bottom right
                                    else if(singlePxl[2][j+1] == 0)
                                    {
                                        sortedList->add(&inData[2][j+1]);
                                    }
                                    
                                    if(sortedList->getSize() <= 1)
                                    {
                                        outData[j] = inData[1][j];
                                    }
                                    else
                                    {
                                        outData[j] = *sortedList->getMostCommonValue();
                                        hasChangeOccured = true;
                                    }
                                    sortedList->clearList();
                                }
                            }
                            else
                            {
                                outData[j] = inData[1][j];
                            }
                        }
                        else
                        {
                            outData[j] = inData[1][j];
                        }
                    }
                }
                
                outBand->RasterIO(GF_Write, 0, i, width, 1, outData, width, 1, GDT_UInt32, 0, 0);
            }
            std::cout << ". Complete\n";
            
            delete sortedList;
            delete[] inData[0];
            delete[] inData[1];
            delete[] inData[2];
            delete[] inData;
            delete[] singlePxl[0];
            delete[] singlePxl[1];
            delete[] singlePxl[2];
            delete[] singlePxl;
            delete[] outData;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        return hasChangeOccured;
    }
    
    unsigned int RSGISEliminateSingleClassPixels::findMostCommonVal(std::vector<unsigned int> values)
    {
        return 0;
    }
    
    RSGISEliminateSingleClassPixels::~RSGISEliminateSingleClassPixels()
    {
        
    }

}}


