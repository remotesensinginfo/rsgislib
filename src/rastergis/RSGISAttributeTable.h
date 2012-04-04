/*
 *  RSGISAttributeTable.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 15/02/2012.
 *  Copyright 2012 RSGISLib.
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

#ifndef RSGISAttributeTable_h
#define RSGISAttributeTable_h

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"
#include "gdal_rat.h"

#include "muParser.h"

#include "common/RSGISAttributeTableException.h"

#include "utils/RSGISTextUtils.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "boost/math/special_functions/fpclassify.hpp"
#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

using namespace mu;
using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::utils;

using boost::lexical_cast;
using boost::bad_lexical_cast;

namespace rsgis{namespace rastergis{
    
    class RSGISAttributeTable;
    
    struct RSGISFeature
    {
        unsigned long long fid;
        vector<bool> *boolFields;
        vector<long> *intFields;
        vector<double> *floatFields;
        vector<string> *stringFields;
    };
    
    enum RSGISAttributeDataType
    {
        rsgis_na = 0,
        rsgis_bool = 1,
        rsgis_int = 2,
        rsgis_float = 3,
        rsgis_string = 4
    };
    
    struct RSGISAttribute
    {
        string name;
        RSGISAttributeDataType dataType;
        unsigned int idx;
    };
    
    struct RSGISMathAttVariable
    {
        string variable;
        string field;
        unsigned int fieldIdx;
        RSGISAttributeDataType fieldDT;
    };
    
    class RSGISAttExpression
    {
    public:
        RSGISAttExpression(){};
        virtual bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)=0;
        virtual void popIdxs(RSGISAttributeTable *att)throw(RSGISAttributeTableException)=0;
        virtual ~RSGISAttExpression(){};
    };
    
    struct RSGISIfStatement
    {
        RSGISAttExpression *exp;
        string field;
        RSGISAttributeDataType dataType;
        unsigned int fldIdx;
        long value;
        bool noExp; // i.e., it is an else statment...
        bool ignore; // i.e., used for an else statment where nothing is to be done.
    };
    
    class RSGISAttributeTable
    {
    public:        
        RSGISAttributeTable(unsigned long long numFeatures);
        RSGISAttributeTable(unsigned long long numFeatures, vector<pair<string, RSGISAttributeDataType> > *fields);
        
        bool getBoolField(unsigned long long fid, string name) throw(RSGISAttributeTableException);
        long getIntField(unsigned long long fid, string name) throw(RSGISAttributeTableException);
        double getDoubleField(unsigned long long fid, string name) throw(RSGISAttributeTableException);
        string getStringField(unsigned long long fid, string name) throw(RSGISAttributeTableException);
        
        void setBoolField(unsigned long long fid, string name, bool value) throw(RSGISAttributeTableException);
        void setIntField(unsigned long long fid, string name, long value) throw(RSGISAttributeTableException);
        void setDoubleField(unsigned long long fid, string name, double value) throw(RSGISAttributeTableException);
        void setStringField(unsigned long long fid, string name, string value) throw(RSGISAttributeTableException);
        
        void setBoolValue(string name, bool value) throw(RSGISAttributeTableException);
        void setIntValue(string name, long value) throw(RSGISAttributeTableException);
        void setFloatValue(string name, double value) throw(RSGISAttributeTableException);
        void setStringValue(string name, string value) throw(RSGISAttributeTableException);
        
        RSGISFeature* getFeature(unsigned long long fid) throw(RSGISAttributeTableException);
        
        void addAttBoolField(string name, bool val);
        void addAttIntField(string name, long val);
        void addAttFloatField(string name, double val);
        void addAttStringField(string name, string val);
        
        void addAttributes(vector<RSGISAttribute*> *attributes) throw(RSGISAttributeTableException);
        
        RSGISAttributeDataType getDataType(string name) throw(RSGISAttributeTableException);
        unsigned int getFieldIndex(string name) throw(RSGISAttributeTableException);
        vector<string>* getAttributeNames();
        bool hasAttribute(string name);
        unsigned long long getSize();
        
        vector<double>* getFieldValues(string field) throw(RSGISAttributeTableException);
        
        void applyIfStatements(vector<RSGISIfStatement*> *statements) throw(RSGISAttributeTableException);
        bool applyIfStatementsToFeature(RSGISFeature *feat, vector<RSGISIfStatement*> *statements) throw(RSGISAttributeTableException);
        void populateIfStatementsWithIdxs(vector<RSGISIfStatement*> *statements) throw(RSGISAttributeTableException);
        
        void calculateFieldsMUParser(string expression, string outField, RSGISAttributeDataType outFieldDT, vector<RSGISMathAttVariable*> *variables) throw(RSGISAttributeTableException);
        
        void copyValues(string fromField, string toField) throw(RSGISAttributeTableException);
        
        void exportASCII(string outFile) throw(RSGISAttributeTableException);
        
        void exportGDALRaster(GDALDataset *inDataset, unsigned int inBand) throw(RSGISAttributeTableException);
        
        vector<double>* getDoubleField(string field) throw(RSGISAttributeTableException);
        vector<long>* getLongField(string field) throw(RSGISAttributeTableException);
        vector<bool>* getBoolField(string field) throw(RSGISAttributeTableException);
        vector<string>* getStringField(string field) throw(RSGISAttributeTableException);
        
        ~RSGISAttributeTable();
        
        static RSGISAttributeTable* importFromASCII(string inFile)throw(RSGISAttributeTableException);
        static RSGISAttributeTable* importFromGDALRaster(string inFile)throw(RSGISAttributeTableException);
        static vector<RSGISIfStatement*>* generateStatments(DOMElement *argElement)throw(RSGISAttributeTableException);
        static RSGISAttExpression* generateExpression(DOMElement *expElement)throw(RSGISAttributeTableException);
        
    protected:
        RSGISAttributeTable();
        void createAttributeTable(unsigned long long numFeatures);
        void createAttributeTableWithFields(unsigned long long numFeatures);
        vector<RSGISFeature*> *attTable;
        vector<pair<string, RSGISAttributeDataType> > *fields;
        map<string, unsigned int> *fieldIdx;
        map<string, RSGISAttributeDataType> *fieldDataType;
        unsigned int numBoolFields;
        unsigned int numIntFields;
        unsigned int numFloatFields;
        unsigned int numStrFields;
    };
    
    
    class RSGISAttExpression2Fields : public RSGISAttExpression
    {
    public:
        RSGISAttExpression2Fields(string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression()
        {
            this->f1Name = f1Name;
            this->f1Idx = f1Idx;
            this->f1Type = f1Type;
            this->f2Name = f2Name;
            this->f2Idx = f2Idx;
            this->f2Type = f2Type;
        };
        virtual bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)=0;
        virtual void popIdxs(RSGISAttributeTable *att)throw(RSGISAttributeTableException)
        {
            try
            {
                this->f1Type = att->getDataType(f1Name);
                this->f1Idx = att->getFieldIndex(f1Name);
                
                this->f2Type = att->getDataType(f2Name);
                this->f2Idx = att->getFieldIndex(f2Name);
            }
            catch(RSGISAttributeTableException &e)
            {
                throw e;
            }
        };
        virtual ~RSGISAttExpression2Fields()
        {
            
        };
    protected:
        string f1Name;
        unsigned int f1Idx;
        RSGISAttributeDataType f1Type;
        string f2Name;
        unsigned int f2Idx;
        RSGISAttributeDataType f2Type;
    };
    
    class RSGISAttExpressionFieldAndValue : public RSGISAttExpression
    {
    public:
        RSGISAttExpressionFieldAndValue(string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpression()
        {
            this->fName = fName;
            this->fIdx = fIdx;
            this->fType = fType;
            this->value = value;
        }
        virtual bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)=0;
        virtual void popIdxs(RSGISAttributeTable *att)throw(RSGISAttributeTableException)
        {
            try
            {
                this->fType = att->getDataType(fName);
                this->fIdx = att->getFieldIndex(fName);
            }
            catch(RSGISAttributeTableException &e)
            {
                throw e;
            }
        };
        virtual ~RSGISAttExpressionFieldAndValue()
        {
            
        };
    protected:
        string fName;
        unsigned int fIdx;
        RSGISAttributeDataType fType;
        float value;
    };
    
    class RSGISAttExpressionAND : public RSGISAttExpression
    {
    public:
        RSGISAttExpressionAND(vector<RSGISAttExpression*> *exps):RSGISAttExpression()
        {
            this->exps = exps;
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            bool outVal = true;
            try
            {
                for(vector<RSGISAttExpression*>::iterator iterExp = exps->begin(); iterExp != exps->end(); ++iterExp)
                {
                    if(!(*iterExp)->evaluate(feat))
                    {
                        outVal = false;
                        break;
                    }
                }
            }
            catch(RSGISAttributeTableException &e)
            {
                throw e;
            }
            
            return outVal;
        };
        virtual void popIdxs(RSGISAttributeTable *att)throw(RSGISAttributeTableException)
        {
            try
            {
                for(vector<RSGISAttExpression*>::iterator iterExp = exps->begin(); iterExp != exps->end(); ++iterExp)
                {
                    (*iterExp)->popIdxs(att);
                }
            }
            catch(RSGISAttributeTableException &e)
            {
                throw e;
            }
        };
        virtual ~RSGISAttExpressionAND()
        {
            if(exps != NULL)
            {
                for(vector<RSGISAttExpression*>::iterator iterExp = exps->begin(); iterExp != exps->end(); ++iterExp)
                {
                    delete *iterExp;
                }
                delete exps;
            }
        };
    private:
        vector<RSGISAttExpression*> *exps;
    };
    
    class RSGISAttExpressionOR : public RSGISAttExpression
    {
    public:
        RSGISAttExpressionOR(vector<RSGISAttExpression*> *exps):RSGISAttExpression()
        {
            this->exps = exps;
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            bool outVal = false;
            try
            {
                for(vector<RSGISAttExpression*>::iterator iterExp = exps->begin(); iterExp != exps->end(); ++iterExp)
                {
                    if((*iterExp)->evaluate(feat))
                    {
                        outVal = true;
                        break;
                    }
                }
            }
            catch(RSGISAttributeTableException &e)
            {
                throw e;
            }
            return outVal;
        };
        virtual void popIdxs(RSGISAttributeTable *att)throw(RSGISAttributeTableException)
        {
            try
            {
                for(vector<RSGISAttExpression*>::iterator iterExp = exps->begin(); iterExp != exps->end(); ++iterExp)
                {
                    (*iterExp)->popIdxs(att);
                }
            }
            catch(RSGISAttributeTableException &e)
            {
                throw e;
            }
        };
        virtual ~RSGISAttExpressionOR()
        {
            if(exps != NULL)
            {
                for(vector<RSGISAttExpression*>::iterator iterExp = exps->begin(); iterExp != exps->end(); ++iterExp)
                {
                    delete *iterExp;
                }
                delete exps;
            }
        };
    private:
        vector<RSGISAttExpression*> *exps;
    };
    
    class RSGISAttExpressionNot : public RSGISAttExpression
    {
    public:
        RSGISAttExpressionNot(RSGISAttExpression *exp):RSGISAttExpression()
        {
            this->exp = exp;
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            try
            {
                return !exp->evaluate(feat);
            }
            catch(RSGISAttributeTableException &e)
            {
                throw e;
            }
        };
        virtual void popIdxs(RSGISAttributeTable *att)throw(RSGISAttributeTableException)
        {
            try
            {
                exp->popIdxs(att);
            }
            catch(RSGISAttributeTableException &e)
            {
                throw e;
            }
        };
        virtual ~RSGISAttExpressionNot()
        {
            if(exp != NULL)
            {
                delete exp;
            }
        };
    private:
        RSGISAttExpression *exp;
    };
    
    
    
    class RSGISAttExpressionGreaterThan : public RSGISAttExpression2Fields 
    {
    public:
        RSGISAttExpressionGreaterThan(string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression2Fields(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double f1Val = 0;
            double f2Val = 0;
            
            if(f1Type == rsgis_int)
            {
                f1Val = feat->intFields->at(f1Idx);
            }
            else if(f1Type == rsgis_float)
            {
                f1Val = feat->floatFields->at(f1Idx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(f2Type == rsgis_int)
            {
                f2Val = feat->intFields->at(f1Idx);
            }
            else if(f2Type == rsgis_float)
            {
                f2Val = feat->floatFields->at(f1Idx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(f1Val > f2Val)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionGreaterThan()
        {
            
        };
    
    };
    
    class RSGISAttExpressionLessThan : public RSGISAttExpression2Fields
    {
    public:
        RSGISAttExpressionLessThan(string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression2Fields(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double f1Val = 0;
            double f2Val = 0;
            
            if(f1Type == rsgis_int)
            {
                f1Val = feat->intFields->at(f1Idx);
            }
            else if(f1Type == rsgis_float)
            {
                f1Val = feat->floatFields->at(f1Idx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(f2Type == rsgis_int)
            {
                f2Val = feat->intFields->at(f1Idx);
            }
            else if(f2Type == rsgis_float)
            {
                f2Val = feat->floatFields->at(f1Idx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(f1Val < f2Val)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionLessThan()
        {
            
        };
    };
    
    class RSGISAttExpressionGreaterThanEq : public RSGISAttExpression2Fields
    {
    public:
        RSGISAttExpressionGreaterThanEq(string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression2Fields(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double f1Val = 0;
            double f2Val = 0;
            
            if(f1Type == rsgis_int)
            {
                f1Val = feat->intFields->at(f1Idx);
            }
            else if(f1Type == rsgis_float)
            {
                f1Val = feat->floatFields->at(f1Idx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(f2Type == rsgis_int)
            {
                f2Val = feat->intFields->at(f1Idx);
            }
            else if(f2Type == rsgis_float)
            {
                f2Val = feat->floatFields->at(f1Idx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(f1Val >= f2Val)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionGreaterThanEq()
        {
            
        };
    };
    
    class RSGISAttExpressionLessThanEq : public RSGISAttExpression2Fields
    {
    public:
        RSGISAttExpressionLessThanEq(string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression2Fields(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double f1Val = 0;
            double f2Val = 0;
            
            if(f1Type == rsgis_int)
            {
                f1Val = feat->intFields->at(f1Idx);
            }
            else if(f1Type == rsgis_float)
            {
                f1Val = feat->floatFields->at(f1Idx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(f2Type == rsgis_int)
            {
                f2Val = feat->intFields->at(f1Idx);
            }
            else if(f2Type == rsgis_float)
            {
                f2Val = feat->floatFields->at(f1Idx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(f1Val <= f2Val)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionLessThanEq()
        {
            
        };
    };

    class RSGISAttExpressionEquals : public RSGISAttExpression2Fields
    {
    public:
        RSGISAttExpressionEquals(string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression2Fields(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double f1Val = 0;
            double f2Val = 0;
            
            if(f1Type == rsgis_int)
            {
                f1Val = feat->intFields->at(f1Idx);
            }
            else if(f1Type == rsgis_float)
            {
                f1Val = feat->floatFields->at(f1Idx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(f2Type == rsgis_int)
            {
                f2Val = feat->intFields->at(f1Idx);
            }
            else if(f2Type == rsgis_float)
            {
                f2Val = feat->floatFields->at(f1Idx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(f1Val == f2Val)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionEquals()
        {
            
        };
    };
    
    class RSGISAttExpressionNotEquals : public RSGISAttExpression2Fields
    {
    public:
        RSGISAttExpressionNotEquals(string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression2Fields(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double f1Val = 0;
            double f2Val = 0;
            
            if(f1Type == rsgis_int)
            {
                f1Val = feat->intFields->at(f1Idx);
            }
            else if(f1Type == rsgis_float)
            {
                f1Val = feat->floatFields->at(f1Idx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(f2Type == rsgis_int)
            {
                f2Val = feat->intFields->at(f1Idx);
            }
            else if(f2Type == rsgis_float)
            {
                f2Val = feat->floatFields->at(f1Idx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(f1Val != f2Val)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionNotEquals()
        {
            
        };
    };
    
    
    
    class RSGISAttExpressionGreaterThanConst : public RSGISAttExpressionFieldAndValue
    {
    public:
        RSGISAttExpressionGreaterThanConst(string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double fVal = 0;
            
            if(fType == rsgis_int)
            {
                fVal = feat->intFields->at(fIdx);
            }
            else if(fType == rsgis_float)
            {
                fVal = feat->floatFields->at(fIdx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
                        
            if(fVal > value)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionGreaterThanConst()
        {
            
        };
    };
    
    class RSGISAttExpressionLessThanConst : public RSGISAttExpressionFieldAndValue
    {
    public:
        RSGISAttExpressionLessThanConst(string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double fVal = 0;
            
            if(fType == rsgis_int)
            {
                fVal = feat->intFields->at(fIdx);
            }
            else if(fType == rsgis_float)
            {
                fVal = feat->floatFields->at(fIdx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(fVal < value)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionLessThanConst()
        {
            
        };
    };
    
    class RSGISAttExpressionGreaterThanConstEq : public RSGISAttExpressionFieldAndValue
    {
    public:
        RSGISAttExpressionGreaterThanConstEq(string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double fVal = 0;
            
            if(fType == rsgis_int)
            {
                fVal = feat->intFields->at(fIdx);
            }
            else if(fType == rsgis_float)
            {
                fVal = feat->floatFields->at(fIdx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(fVal >= value)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionGreaterThanConstEq()
        {
            
        };
    };
    
    class RSGISAttExpressionLessThanConstEq : public RSGISAttExpressionFieldAndValue
    {
    public:
        RSGISAttExpressionLessThanConstEq(string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double fVal = 0;
            
            if(fType == rsgis_int)
            {
                fVal = feat->intFields->at(fIdx);
            }
            else if(fType == rsgis_float)
            {
                fVal = feat->floatFields->at(fIdx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(fVal <= value)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionLessThanConstEq()
        {
            
        };
    };
    
    class RSGISAttExpressionEqualsConst : public RSGISAttExpressionFieldAndValue
    {
    public:
        RSGISAttExpressionEqualsConst(string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double fVal = 0;
            
            if(fType == rsgis_int)
            {
                fVal = feat->intFields->at(fIdx);
            }
            else if(fType == rsgis_float)
            {
                fVal = feat->floatFields->at(fIdx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(fVal == value)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionEqualsConst()
        {
            
        };
    };
    
    class RSGISAttExpressionNotEqualsConst : public RSGISAttExpressionFieldAndValue
    {
    public:
        RSGISAttExpressionNotEqualsConst(string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double fVal = 0;
            
            if(fType == rsgis_int)
            {
                fVal = feat->intFields->at(fIdx);
            }
            else if(fType == rsgis_float)
            {
                fVal = feat->floatFields->at(fIdx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(fVal != value)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionNotEqualsConst()
        {
            
        };
    };
    
    
    
    class RSGISAttExpressionConstGreaterThan : public RSGISAttExpressionFieldAndValue
    {
    public:
        RSGISAttExpressionConstGreaterThan(string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double fVal = 0;
                        
            if(fType == rsgis_int)
            {
                fVal = feat->intFields->at(fIdx);
            }
            else if(fType == rsgis_float)
            {
                fVal = feat->floatFields->at(fIdx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(value > fVal)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionConstGreaterThan()
        {
            
        };
    };
    
    class RSGISAttExpressionConstLessThan : public RSGISAttExpressionFieldAndValue
    {
    public:
        RSGISAttExpressionConstLessThan(string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double fVal = 0;
            
            if(fType == rsgis_int)
            {
                fVal = feat->intFields->at(fIdx);
            }
            else if(fType == rsgis_float)
            {
                fVal = feat->floatFields->at(fIdx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(value < fVal)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionConstLessThan()
        {
            
        };
    };
    
    class RSGISAttExpressionConstGreaterThanEq : public RSGISAttExpressionFieldAndValue
    {
    public:
        RSGISAttExpressionConstGreaterThanEq(string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double fVal = 0;
            
            if(fType == rsgis_int)
            {
                fVal = feat->intFields->at(fIdx);
            }
            else if(fType == rsgis_float)
            {
                fVal = feat->floatFields->at(fIdx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(value >= fVal)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionConstGreaterThanEq()
        {
            
        };
    };
    
    class RSGISAttExpressionConstLessThanEq : public RSGISAttExpressionFieldAndValue
    {
    public:
        RSGISAttExpressionConstLessThanEq(string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
        {
            
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            double fVal = 0;
            
            if(fType == rsgis_int)
            {
                fVal = feat->intFields->at(fIdx);
            }
            else if(fType == rsgis_float)
            {
                fVal = feat->floatFields->at(fIdx);
            }
            else
            {
                throw RSGISAttributeTableException("Do not recognised data type - can only use int or float.");
            }
            
            if(value <= fVal)
            {
                return true;
            }
            return false;
        };
        virtual ~RSGISAttExpressionConstLessThanEq()
        {
            
        };
    };
    
}}



#endif



