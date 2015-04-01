/*
 *  RSGISBinaryClassifyClumps.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/08/2014.
 *  Copyright 2014 RSGISLib.
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

#include "RSGISBinaryClassifyClumps.h"

namespace rsgis{namespace rastergis{
    
    
    
    rsgis::math::RSGISLogicExpression* RSGISRATLogicXMLParse::parseLogicXML(std::string xmlStr, std::vector<RSGISColumnLogicIdxs*> *colIdxes) throw(RSGISAttributeTableException)
    {
        rsgis::math::RSGISLogicExpression *outExp = NULL;
        xercesc::DOMLSParser* parser = NULL;
        try
        {
            xercesc::XMLPlatformUtils::Initialize();
            
            XMLCh tempStr[100];
            xercesc::DOMImplementation *impl = NULL;
            xercesc::ErrorHandler* errHandler = NULL;
            xercesc::DOMDocument *xmlDoc = NULL;
            xercesc::DOMElement *rootElement = NULL;
            XMLCh *expTagStr = xercesc::XMLString::transcode("rsgis:expression");
            XMLCh *ratExpsTagStr = xercesc::XMLString::transcode("rsgis:ratlogicexps");
            XMLCh *operationXMLStr = xercesc::XMLString::transcode("operation");
            xercesc::DOMElement *expElement = NULL;
            
            xercesc::XMLString::transcode("LS", tempStr, 99);
			impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
			if(impl == NULL)
			{
				throw RSGISAttributeTableException("DOMImplementation is NULL");
			}
            
            // Create Parser
			parser = ((xercesc::DOMImplementationLS*)impl)->createLSParser(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
			errHandler = (xercesc::ErrorHandler*) new xercesc::HandlerBase();
			parser->getDomConfig()->setParameter(xercesc::XMLUni::fgDOMErrorHandler, errHandler);
            
            // Open Document
            const XMLByte *byteXMLStr = (XMLByte *)xmlStr.c_str();
            xercesc::MemBufInputSource *memSource = new xercesc::MemBufInputSource(byteXMLStr, xmlStr.length(), "xmlStr (in memory)");
            xercesc::Wrapper4InputSource *source = new xercesc::Wrapper4InputSource(memSource, true);
			xmlDoc = parser->parse(source);
            //xmlDoc = parser->parseURI("/Users/pete/Desktop/TestLogicalExpressions/Test.xml");
            
            // Get the Root element
			rootElement = xmlDoc->getDocumentElement();
            if(rootElement == NULL)
            {
                throw RSGISAttributeTableException("Root element returned NULL - check for errors in the header info on the XML file.");
            }
			if(!xercesc::XMLString::equals(rootElement->getTagName(), ratExpsTagStr))
			{
				throw RSGISAttributeTableException("Incorrect root element; Root element should be \"rsgis:ratlogicexps\"");
			}
            
            boost::uint_fast32_t numExps = rootElement->getChildElementCount();
            
            if(numExps != 1)
            {
                throw RSGISAttributeTableException("Multiple expressions provided. The top level of the expression must be a single expression as a single boolean value is required.");
            }
            
            expElement = rootElement->getFirstElementChild();
            
            if(expElement->hasAttribute(operationXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(operationXMLStr));
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw RSGISAttributeTableException("No \'operation\' attribute was provided for root expression.");
            }
            outExp = this->createExpression(expElement, colIdxes);
            
            parser->release();
			delete errHandler;
			xercesc::XMLString::release(&expTagStr);
			xercesc::XMLString::release(&ratExpsTagStr);
            xercesc::XMLString::release(&operationXMLStr);
			xercesc::XMLPlatformUtils::Terminate();
        }
        catch (const xercesc::XMLException& e)
		{
			parser->release();
			char *message = xercesc::XMLString::transcode(e.getMessage());
			std::string outMessage =  std::string("XMLException : ") + std::string(message) + std::string("\'");
			throw RSGISAttributeTableException(outMessage.c_str());
		}
		catch (const xercesc::DOMException& e)
		{
			parser->release();
			char *message = xercesc::XMLString::transcode(e.getMessage());
			std::string outMessage =  std::string("DOMException : \'") + std::string(message) + std::string("\'");
			throw RSGISAttributeTableException(outMessage.c_str());
		}
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::math::RSGISMathLogicException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }

        return outExp;
    }
    
    
    rsgis::math::RSGISLogicExpression* RSGISRATLogicXMLParse::createExpression(xercesc::DOMElement *expElement, std::vector<RSGISColumnLogicIdxs*> *colIdxes) throw(RSGISAttributeTableException)
    {
        rsgis::math::RSGISLogicExpression *outExp = NULL;
        try
        {
            rsgis::math::RSGISMathsUtils mathUtils;
            XMLCh *expAnd = xercesc::XMLString::transcode("and");
            XMLCh *expOr = xercesc::XMLString::transcode("or");
            XMLCh *expEqual = xercesc::XMLString::transcode("equal");
            XMLCh *expNot = xercesc::XMLString::transcode("not");
            XMLCh *expEval = xercesc::XMLString::transcode("evaluate");
            XMLCh *expEq = xercesc::XMLString::transcode("eq");
            XMLCh *expNotEq = xercesc::XMLString::transcode("noteq");
            XMLCh *expGt = xercesc::XMLString::transcode("gt");
            XMLCh *expLt = xercesc::XMLString::transcode("lt");
            XMLCh *expGtEq = xercesc::XMLString::transcode("gteq");
            XMLCh *expLtEq = xercesc::XMLString::transcode("lteq");
            
            XMLCh *operationXMLStr = xercesc::XMLString::transcode("operation");
            XMLCh *operationStr = NULL;
            XMLCh *operatorXMLStr = xercesc::XMLString::transcode("operator");
            XMLCh *operatorStr = NULL;
            
            if(expElement->hasAttribute(operationXMLStr))
            {
                char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(operationXMLStr));
                operationStr = xercesc::XMLString::transcode(charValue);
                xercesc::XMLString::release(&charValue);
            }
            else
            {
                throw RSGISAttributeTableException("The \'operation\' attribute was not provided for the expression element.");
            }
            
            if(xercesc::XMLString::equals(operationStr, expAnd))
            {
                unsigned int numChildExps = expElement->getChildElementCount();
                if(numChildExps < 2)
                {
                    throw RSGISAttributeTableException("The \'and\' operation needs at least two child expressions.");
                }
                
                std::vector<rsgis::math::RSGISLogicExpression*> *expsVec = new std::vector<rsgis::math::RSGISLogicExpression*>();
                
                xercesc::DOMElement *tmpExpElement = expElement->getFirstElementChild();
                for(boost::uint_fast32_t i = 0; i < numChildExps; ++i)
                {
                    // Retrieve Expression and add to list.
                    expsVec->push_back(this->createExpression(tmpExpElement, colIdxes));
                    
                    // Move on to next Expression
                    tmpExpElement = tmpExpElement->getNextElementSibling();
                }
                
                outExp = new rsgis::math::RSGISLogicAndExpression(expsVec);
            }
            else if(xercesc::XMLString::equals(operationStr, expOr))
            {
                unsigned int numChildExps = expElement->getChildElementCount();
                if(numChildExps < 2)
                {
                    throw RSGISAttributeTableException("The \'or\' operation needs at least two child expressions.");
                }
                
                std::vector<rsgis::math::RSGISLogicExpression*> *expsVec = new std::vector<rsgis::math::RSGISLogicExpression*>();
                
                xercesc::DOMElement *tmpExpElement = expElement->getFirstElementChild();
                for(boost::uint_fast32_t i = 0; i < numChildExps; ++i)
                {
                    // Retrieve Expression and add to list.
                    expsVec->push_back(this->createExpression(tmpExpElement, colIdxes));
                    
                    // Move on to next Expression
                    tmpExpElement = tmpExpElement->getNextElementSibling();
                }
                
                outExp = new rsgis::math::RSGISLogicOrExpression(expsVec);
            }
            else if(xercesc::XMLString::equals(operationStr, expEqual))
            {
                unsigned int numChildExps = expElement->getChildElementCount();
                if(numChildExps < 2)
                {
                    throw RSGISAttributeTableException("The \'equal\' operation needs at least two child expressions.");
                }
                
                std::vector<rsgis::math::RSGISLogicExpression*> *expsVec = new std::vector<rsgis::math::RSGISLogicExpression*>();
                
                xercesc::DOMElement *tmpExpElement = expElement->getFirstElementChild();
                for(boost::uint_fast32_t i = 0; i < numChildExps; ++i)
                {
                    // Retrieve Expression and add to list.
                    expsVec->push_back(this->createExpression(tmpExpElement, colIdxes));
                    
                    // Move on to next Expression
                    tmpExpElement = tmpExpElement->getNextElementSibling();
                }
                
                outExp = new rsgis::math::RSGISLogicEqualsExpression(expsVec);
            }
            else if(xercesc::XMLString::equals(operationStr, expNot))
            {
                unsigned int numChildExps = expElement->getChildElementCount();
                if(numChildExps != 1)
                {
                    throw RSGISAttributeTableException("The \'not\' operation must have just 1 child expression.");
                }
                xercesc::DOMElement *tmpExpElement = expElement->getFirstElementChild();
                
                outExp = new rsgis::math::RSGISLogicNotExpression(this->createExpression(tmpExpElement, colIdxes));
                
            }
            else if(xercesc::XMLString::equals(operationStr, expEval))
            {
                unsigned int numChildExps = expElement->getChildElementCount();
                if(numChildExps != 0)
                {
                    throw RSGISAttributeTableException("The \'evaluate\' operation must not have any child expressions.");
                }
                
                if(expElement->hasAttribute(operatorXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(operatorXMLStr));
                    operatorStr = xercesc::XMLString::transcode(charValue);
                    xercesc::XMLString::release(&charValue);
                }
                else
                {
                    throw RSGISAttributeTableException("The \'operator\' attribute was not provided for the \'evaluate\' expression element.");
                }
                
                XMLCh *columnXMLStr = xercesc::XMLString::transcode("column");
                std::string columnStr = "";
                RSGISColumnLogicIdxs *logicObj = NULL;
                XMLCh *thresholdXMLStr = xercesc::XMLString::transcode("threshold");
                std::string thresholdStr = "";
                double thresholdVal = 0.0;
                RSGISLogicExpType expType = rsgis_expNA;
                XMLCh *column1XMLStr = xercesc::XMLString::transcode("column1");
                std::string column1Str = "";
                XMLCh *column2XMLStr = xercesc::XMLString::transcode("column2");
                std::string column2Str = "";
                
                if(expElement->hasAttribute(columnXMLStr) & expElement->hasAttribute(thresholdXMLStr))
                {
                    char *charValue1 = xercesc::XMLString::transcode(expElement->getAttribute(columnXMLStr));
                    columnStr = std::string(charValue1);
                    xercesc::XMLString::release(&charValue1);
                    
                    char *charValue2 = xercesc::XMLString::transcode(expElement->getAttribute(thresholdXMLStr));
                    thresholdStr = std::string(charValue2);
                    thresholdVal = mathUtils.strtodouble(thresholdStr);
                    xercesc::XMLString::release(&charValue2);
                    
                    expType = rsgis_singlecolthres;
                    bool found = false;
                    for(std::vector<RSGISColumnLogicIdxs*>::iterator iterLogics = colIdxes->begin(); iterLogics != colIdxes->end(); ++iterLogics)
                    {
                        if(((*iterLogics)->column1Name == columnStr) & ((*iterLogics)->useThreshold) & ((*iterLogics)->singleCol) && ((*iterLogics)->thresholdVal == thresholdVal))
                        {
                            logicObj = *iterLogics;
                            found = true;
                        }
                    }
                    if(!found)
                    {
                        logicObj = new RSGISColumnLogicIdxs();
                        logicObj->column1Name = columnStr;
                        logicObj->useThreshold = true;
                        logicObj->thresholdVal = thresholdVal;
                        logicObj->singleCol = true;
                        colIdxes->push_back(logicObj);
                    }
                }
                else if(expElement->hasAttribute(column1XMLStr) & expElement->hasAttribute(column2XMLStr))
                {
                    char *charValue1 = xercesc::XMLString::transcode(expElement->getAttribute(column1XMLStr));
                    column1Str = std::string(charValue1);
                    xercesc::XMLString::release(&charValue1);
                    
                    char *charValue2 = xercesc::XMLString::transcode(expElement->getAttribute(column2XMLStr));
                    column2Str = std::string(charValue2);
                    xercesc::XMLString::release(&charValue2);
                    
                    expType = rsgis_multicols;
                    
                    bool found = false;
                    for(std::vector<RSGISColumnLogicIdxs*>::iterator iterLogics = colIdxes->begin(); iterLogics != colIdxes->end(); ++iterLogics)
                    {
                        if(((*iterLogics)->column1Name == column1Str) & ((*iterLogics)->column2Name == column2Str) & (!(*iterLogics)->useThreshold) & (!((*iterLogics)->singleCol)))
                        {
                            logicObj = *iterLogics;
                            found = true;
                        }
                    }
                    if(!found)
                    {
                        logicObj = new RSGISColumnLogicIdxs();
                        logicObj->column1Name = column1Str;
                        logicObj->column2Name = column2Str;
                        logicObj->useThreshold = false;
                        logicObj->singleCol = false;
                        colIdxes->push_back(logicObj);
                    }
                }
                else if(expElement->hasAttribute(columnXMLStr))
                {
                    char *charValue = xercesc::XMLString::transcode(expElement->getAttribute(columnXMLStr));
                    columnStr = std::string(charValue);
                    xercesc::XMLString::release(&charValue);
                    
                    expType = rsgis_singlecol;
                    bool found = false;
                    for(std::vector<RSGISColumnLogicIdxs*>::iterator iterLogics = colIdxes->begin(); iterLogics != colIdxes->end(); ++iterLogics)
                    {
                        if(((*iterLogics)->column1Name == columnStr) & (!((*iterLogics)->useThreshold)) & ((*iterLogics)->singleCol))
                        {
                            logicObj = *iterLogics;
                            found = true;
                        }
                    }
                    if(!found)
                    {
                        logicObj = new RSGISColumnLogicIdxs();
                        logicObj->column1Name = columnStr;
                        logicObj->useThreshold = false;
                        logicObj->thresholdVal = 0;
                        logicObj->singleCol = true;
                        colIdxes->push_back(logicObj);
                    }
                }
                else
                {
                    throw RSGISAttributeTableException("Either provide \'column\' and \'threshold\' or \'column1\' and \'column2\' attributes.");
                }
                
                if(xercesc::XMLString::equals(operatorStr, expEq))
                {
                    if(expType == rsgis_singlecolthres)
                    {
                        outExp = new rsgis::math::RSGISLogicEqualsValueExpression(&logicObj->col1Val, &logicObj->thresholdVal);
                    }
                    else if(expType == rsgis_multicols)
                    {
                        outExp = new rsgis::math::RSGISLogicEqualsValueExpression(&logicObj->col1Val, &logicObj->col2Val);
                    }
                    else if(expType == rsgis_singlecol)
                    {
                        outExp = new rsgis::math::RSGISLogicEqualsValueExpression(&logicObj->col1Val, &logicObj->col2Val);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("The expression type was not recognised");
                    }
                }
                else if(xercesc::XMLString::equals(operatorStr, expNotEq))
                {
                    if(expType == rsgis_singlecolthres)
                    {
                        outExp = new rsgis::math::RSGISLogicNotValueExpression(&logicObj->col1Val, &logicObj->thresholdVal);
                    }
                    else if(expType == rsgis_multicols)
                    {
                        outExp = new rsgis::math::RSGISLogicNotValueExpression(&logicObj->col1Val, &logicObj->col2Val);
                    }
                    else if(expType == rsgis_singlecol)
                    {
                        outExp = new rsgis::math::RSGISLogicNotValueExpression(&logicObj->col1Val, &logicObj->col2Val);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("The expression type was not recognised");
                    }
                }
                else if(xercesc::XMLString::equals(operatorStr, expGt))
                {
                    if(expType == rsgis_singlecolthres)
                    {
                        outExp = new rsgis::math::RSGISLogicGreaterThanValueExpression(&logicObj->col1Val, &logicObj->thresholdVal);
                    }
                    else if(expType == rsgis_multicols)
                    {
                        outExp = new rsgis::math::RSGISLogicGreaterThanValueExpression(&logicObj->col1Val, &logicObj->col2Val);
                    }
                    else if(expType == rsgis_singlecol)
                    {
                        outExp = new rsgis::math::RSGISLogicGreaterThanValueExpression(&logicObj->col1Val, &logicObj->col2Val);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("The expression type was not recognised");
                    }
                }
                else if(xercesc::XMLString::equals(operatorStr, expLt))
                {
                    if(expType == rsgis_singlecolthres)
                    {
                        outExp = new rsgis::math::RSGISLogicLessThanValueExpression(&logicObj->col1Val, &logicObj->thresholdVal);
                    }
                    else if(expType == rsgis_multicols)
                    {
                        outExp = new rsgis::math::RSGISLogicLessThanValueExpression(&logicObj->col1Val, &logicObj->col2Val);
                    }
                    else if(expType == rsgis_singlecol)
                    {
                        outExp = new rsgis::math::RSGISLogicLessThanValueExpression(&logicObj->col1Val, &logicObj->col2Val);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("The expression type was not recognised");
                    }
                }
                else if(xercesc::XMLString::equals(operatorStr, expGtEq))
                {
                    if(expType == rsgis_singlecolthres)
                    {
                        outExp = new rsgis::math::RSGISLogicGreaterEqualToValueExpression(&logicObj->col1Val, &logicObj->thresholdVal);
                    }
                    else if(expType == rsgis_multicols)
                    {
                        outExp = new rsgis::math::RSGISLogicGreaterEqualToValueExpression(&logicObj->col1Val, &logicObj->col2Val);
                    }
                    else if(expType == rsgis_singlecol)
                    {
                        outExp = new rsgis::math::RSGISLogicGreaterEqualToValueExpression(&logicObj->col1Val, &logicObj->col2Val);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("The expression type was not recognised");
                    }
                }
                else if(xercesc::XMLString::equals(operatorStr, expLtEq))
                {
                    if(expType == rsgis_singlecolthres)
                    {
                        outExp = new rsgis::math::RSGISLogicLessEqualToValueExpression(&logicObj->col1Val, &logicObj->thresholdVal);
                    }
                    else if(expType == rsgis_multicols)
                    {
                        outExp = new rsgis::math::RSGISLogicLessEqualToValueExpression(&logicObj->col1Val, &logicObj->col2Val);
                    }
                    else if(expType == rsgis_singlecol)
                    {
                        outExp = new rsgis::math::RSGISLogicLessEqualToValueExpression(&logicObj->col1Val, &logicObj->col2Val);
                    }
                    else
                    {
                        throw RSGISAttributeTableException("The expression type was not recognised");
                    }
                }
                else
                {
                    throw RSGISAttributeTableException("Operator value is not recognised. Must be \'eq\', \'noteq\', \'gt\', \'lt\' \'gteq\' or \'lteq\'.");
                }
                
                xercesc::XMLString::release(&columnXMLStr);
                xercesc::XMLString::release(&thresholdXMLStr);
                xercesc::XMLString::release(&column1XMLStr);
                xercesc::XMLString::release(&column2XMLStr);
                
            }
            else
            {
                throw RSGISAttributeTableException("Operation value is not recognised. Must be \'and\', \'or\', \'equal\', \'not\' or \'evaluate\'.");
            }
            
            xercesc::XMLString::release(&expAnd);
            xercesc::XMLString::release(&expOr);
            xercesc::XMLString::release(&expEqual);
            xercesc::XMLString::release(&expNot);
            xercesc::XMLString::release(&expEval);
            xercesc::XMLString::release(&expEq);
            xercesc::XMLString::release(&expNotEq);
            xercesc::XMLString::release(&expGt);
            xercesc::XMLString::release(&expLt);
            xercesc::XMLString::release(&expGtEq);
            xercesc::XMLString::release(&expLtEq);
            
            xercesc::XMLString::release(&operationXMLStr);
            xercesc::XMLString::release(&operatorXMLStr);
        }
        catch (const xercesc::XMLException& e)
		{
			char *message = xercesc::XMLString::transcode(e.getMessage());
			std::string outMessage =  std::string("XMLException : ") + std::string(message) + std::string("\'");
			throw RSGISAttributeTableException(outMessage.c_str());
		}
		catch (const xercesc::DOMException& e)
		{
			char *message = xercesc::XMLString::transcode(e.getMessage());
			std::string outMessage =  std::string("DOMException : \'") + std::string(message) + std::string("\'");
			throw RSGISAttributeTableException(outMessage.c_str());
		}
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::math::RSGISMathLogicException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        return outExp;
    }
    
    
    
    
    RSGISBinaryClassifyClumps::RSGISBinaryClassifyClumps()
    {
        
    }
    
    void RSGISBinaryClassifyClumps::classifyClumps(GDALDataset *inputClumps, unsigned int ratBand, std::string xmlBlock, std::string outColumn)throw(RSGISAttributeTableException)
    {
        try
        {
            RSGISRasterAttUtils attUtils;
            
            GDALRasterAttributeTable *rat = inputClumps->GetRasterBand(ratBand)->GetDefaultRAT();
            
            size_t numRows = rat->GetRowCount();
            if(numRows == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT has no rows, i.e., it is empty!");
            }
            
            rsgis::rastergis::RSGISRATLogicXMLParse parseLogicXMLObj;
            std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*> *colIdxes = new std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>();
            rsgis::math::RSGISLogicExpression* exp = parseLogicXMLObj.parseLogicXML(xmlBlock, colIdxes);
            
            unsigned outColIdx = attUtils.findColumnIndexOrCreate(rat, outColumn, GFT_Integer);
            
            std::vector<unsigned int> inRealColIdx;
            std::vector<unsigned int> inIntColIdx;
            std::vector<unsigned int> inStrColIdx;
            std::vector<unsigned int> outRealColIdx;
            std::vector<unsigned int> outIntColIdx;
            outIntColIdx.push_back(outColIdx);
            std::vector<unsigned int> outStrColIdx;
            
            for(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>::iterator iterColIdx = colIdxes->begin(); iterColIdx != colIdxes->end(); ++iterColIdx)
            {
                if((*iterColIdx)->useThreshold)
                {
                    (*iterColIdx)->col1Idx = attUtils.findColumnIndex(rat, (*iterColIdx)->column1Name);
                    std::cout << (*iterColIdx)->column1Name << " = " << (*iterColIdx)->col1Idx << std::endl;
                    inRealColIdx.push_back((*iterColIdx)->col1Idx);
                    (*iterColIdx)->col1Idx = inRealColIdx.size()-1;
                }
                else
                {
                    (*iterColIdx)->col1Idx = attUtils.findColumnIndex(rat, (*iterColIdx)->column1Name);
                    std::cout << (*iterColIdx)->column1Name << " = " << (*iterColIdx)->col1Idx << std::endl;
                    inRealColIdx.push_back((*iterColIdx)->col1Idx);
                    (*iterColIdx)->col1Idx = inRealColIdx.size()-1;
                    (*iterColIdx)->col2Idx = attUtils.findColumnIndex(rat, (*iterColIdx)->column2Name);
                    std::cout << (*iterColIdx)->column2Name << " = " << (*iterColIdx)->col2Idx << std::endl;
                    inRealColIdx.push_back((*iterColIdx)->col2Idx);
                    (*iterColIdx)->col2Idx = inRealColIdx.size()-1;
                }
            }
            
            RSGISBinaryClumpClassifier binClumpClassifier(colIdxes, exp);
            RSGISRATCalc ratCalc(&binClumpClassifier);
            ratCalc.calcRATValues(rat, inRealColIdx, inIntColIdx, inStrColIdx, outRealColIdx, outIntColIdx, outStrColIdx);
            
            for(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>::iterator iterColIdx = colIdxes->begin(); iterColIdx != colIdxes->end(); ++iterColIdx)
            {
                delete *iterColIdx;
            }
            delete colIdxes;
            delete exp;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISBinaryClassifyClumps::~RSGISBinaryClassifyClumps()
    {
        
    }
    
    
    
    RSGISBinaryClumpClassifier::RSGISBinaryClumpClassifier(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*> *colIdxes, rsgis::math::RSGISLogicExpression *exp):RSGISRATCalcValue()
    {
        this->colIdxes = colIdxes;
        this->exp = exp;
    }
    
    void RSGISBinaryClumpClassifier::calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException)
    {
        try
        {
            for(std::vector<rsgis::rastergis::RSGISColumnLogicIdxs*>::iterator iterColIdx = colIdxes->begin(); iterColIdx != colIdxes->end(); ++iterColIdx)
            {
                if((*iterColIdx)->useThreshold)
                {
                    (*iterColIdx)->col1Val = inRealCols[(*iterColIdx)->col1Idx];
                }
                else
                {
                    (*iterColIdx)->col1Val = inRealCols[(*iterColIdx)->col1Idx];
                    (*iterColIdx)->col2Val = inRealCols[(*iterColIdx)->col2Idx];
                }
            }
            
            if(exp->evaluate())
            {
                outIntCols[0] = 1;
            }
            else
            {
                outIntCols[0] = 0;
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISBinaryClumpClassifier::~RSGISBinaryClumpClassifier()
    {
        
    }
    
}}

