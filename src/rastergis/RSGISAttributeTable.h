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

#include "H5Cpp.h"

namespace rsgis{namespace rastergis{
    
    static const std::string ATT_GROUPNAME_HEADER( "/HEADER" );
	static const std::string ATT_GROUPNAME_DATA( "/DATA" );
    static const std::string ATT_GROUPNAME_NEIGHBOURS( "/NEIGHBOURS" );
    
    static const std::string ATT_BOOL_DATA( "/DATA/BOOL" );
    static const std::string ATT_INT_DATA( "/DATA/INT" );
    static const std::string ATT_FLOAT_DATA( "/DATA/FLOAT" );
    static const std::string ATT_NEIGHBOURS_DATA( "/NEIGHBOURS/NEIGHBOURS" );
    
    static const std::string ATT_BOOL_FIELDS_HEADER( "/HEADER/BOOL_FIELDS" );
    static const std::string ATT_INT_FIELDS_HEADER( "/HEADER/INT_FIELDS" );
    static const std::string ATT_FLOAT_FIELDS_HEADER( "/HEADER/FLOAT_FIELDS" );
    static const std::string ATT_SIZE_HEADER( "/HEADER/SIZE" );
    
    static const std::string ATT_NAME_FIELD( "NAME" );
    static const std::string ATT_INDEX_FIELD( "INDEX" );
    
    static const int ATT_READ_MDC_NELMTS( 0 ); //0
    static const hsize_t ATT_READ_RDCC_NELMTS( 512 ); // 512
    static const hsize_t ATT_READ_RDCC_NBYTES( 1048576 ); // 1048576
    static const double ATT_READ_RDCC_W0( 0.75 );
    static const hsize_t ATT_READ_SIEVE_BUF( 65536 ); // 65536
    static const hsize_t ATT_READ_META_BLOCKSIZE( 2048 ); // 2048
    
    static const int ATT_WRITE_MDC_NELMTS( 0 );
    static const hsize_t  ATT_WRITE_RDCC_NELMTS( 512 );
    static const hsize_t  ATT_WRITE_RDCC_NBYTES( 1048576 );
    static const double ATT_WRITE_RDCC_W0( 0.75 );
    static const hsize_t  ATT_WRITE_SIEVE_BUF( 65536 );
    static const hsize_t  ATT_WRITE_META_BLOCKSIZE( 2048 );
    static const unsigned int ATT_WRITE_DEFLATE( 1 );
    static const hsize_t ATT_WRITE_CHUNK_SIZE( 250 ); //100
    
    enum RSGISFileType
    {
        rsgis_unknown_attft = 0,
        rsgis_ascii_attft = 1,
        rsgis_hdf_attft = 2
    };
    
    class RSGISAttributeTable;
    
    struct RSGISFeature
    {
        boost::uint_fast32_t fid;
        std::vector<bool> *boolFields;
        std::vector<long> *intFields;
        std::vector<double> *floatFields;
        std::vector<std::string> *stringFields;
        std::vector<boost::uint_fast32_t> *neighbours;
    };
    
    typedef struct {
        size_t length;
        void   *p;
    } hvl_t;

    
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
        std::string name;
        RSGISAttributeDataType dataType;
        unsigned int idx;
    };
    
    struct RSGISAttributeIdx
    {
        char *name;
        unsigned int idx;
    };
    
    struct RSGISMathAttVariable
    {
        std::string variable;
        std::string field;
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
        std::string field;
        RSGISAttributeDataType dataType;
        unsigned int fldIdx;
        long value;
        bool noExp; // i.e., it is an else statment...
        bool ignore; // i.e., used for an else statment where nothing is to be done.
    };
    
    class AttIterator
    {
    public:
        virtual void operator++()=0;
        virtual void start()=0;
        virtual bool end()=0;
        virtual RSGISFeature* operator *()=0;
        virtual ~AttIterator(){};
    };
    
    class RSGISProcessFeature
    {
    public:
        RSGISProcessFeature(){};
        virtual void processFeature(RSGISFeature *feat, RSGISAttributeTable *attTable)throw(RSGISAttributeTableException)=0;
        virtual ~RSGISProcessFeature(){};
    };
    
    class RSGISAttributeTable : public AttIterator
    {
    public:
        virtual bool getBoolField(size_t fid, std::string name) throw(RSGISAttributeTableException)=0;
        virtual long getIntField(size_t fid, std::string name) throw(RSGISAttributeTableException)=0;
        virtual double getDoubleField(size_t fid, std::string name) throw(RSGISAttributeTableException)=0;
        virtual std::string getStringField(size_t fid, std::string name) throw(RSGISAttributeTableException)=0;
        
        virtual void setBoolField(size_t fid, std::string name, bool value) throw(RSGISAttributeTableException)=0;
        virtual void setIntField(size_t fid, std::string name, long value) throw(RSGISAttributeTableException)=0;
        virtual void setDoubleField(size_t fid, std::string name, double value) throw(RSGISAttributeTableException)=0;
        virtual void setStringField(size_t fid, std::string name, std::string value) throw(RSGISAttributeTableException)=0;
        
        virtual void setBoolValue(std::string name, bool value) throw(RSGISAttributeTableException)=0;
        virtual void setIntValue(std::string name, long value) throw(RSGISAttributeTableException)=0;
        virtual void setFloatValue(std::string name, double value) throw(RSGISAttributeTableException)=0;
        virtual void setStringValue(std::string name, std::string value) throw(RSGISAttributeTableException)=0;
        
        virtual RSGISFeature* getFeature(size_t fid) throw(RSGISAttributeTableException)=0;
        virtual void flushAllFeatures(bool progressFeedback=false) throw(RSGISAttributeTableException)=0;
        
        virtual void addAttBoolField(std::string name, bool val) throw(RSGISAttributeTableException)=0;
        virtual void addAttIntField(std::string name, long val) throw(RSGISAttributeTableException)=0;
        virtual void addAttFloatField(std::string name, double val) throw(RSGISAttributeTableException)=0;
        virtual void addAttStringField(std::string name, std::string val) throw(RSGISAttributeTableException)=0;
        
        virtual void addAttributes(std::vector<RSGISAttribute*> *attributes) throw(RSGISAttributeTableException)=0;
        
        RSGISAttributeDataType getDataType(std::string name) throw(RSGISAttributeTableException);
        unsigned int getFieldIndex(std::string name) throw(RSGISAttributeTableException);
        std::vector<std::string>* getAttributeNames();
        bool hasAttribute(std::string name);
        virtual size_t getSize()=0;
        virtual void holdFID(size_t fid)=0;
        virtual void removeHoldFID(size_t fid)=0;
        virtual size_t getNumOfBlocks()=0;
        virtual void loadBlocks(size_t startBlock, size_t endBlock) throw(RSGISAttributeTableException)=0;
        virtual bool attInMemory()=0;
        virtual void findFIDRangeInNeighbours(size_t startFID, size_t endFID, size_t *minFID, size_t *maxFID) throw(RSGISAttributeTableException)=0;
        
        std::vector<double>* getFieldValues(std::string field) throw(RSGISAttributeTableException);
        
        void applyIfStatements(std::vector<RSGISIfStatement*> *statements) throw(RSGISAttributeTableException);
        bool applyIfStatementsToFeature(RSGISFeature *feat, std::vector<RSGISIfStatement*> *statements) throw(RSGISAttributeTableException);
        void processIfStatements(RSGISIfStatement *statement, RSGISProcessFeature *processTrue, RSGISProcessFeature *processFalse) throw(RSGISAttributeTableException);
        void processIfStatementsInBlocks(RSGISIfStatement *statement, RSGISProcessFeature *processTrue, RSGISProcessFeature *processFalse) throw(RSGISAttributeTableException);
        void applyIfStatementBoolOut(RSGISIfStatement *statement) throw(RSGISAttributeTableException);
        void populateIfStatementsWithIdxs(std::vector<RSGISIfStatement*> *statements) throw(RSGISAttributeTableException);
        void createIfStatementsFields(std::vector<RSGISIfStatement*> *statements, RSGISAttributeDataType dataType) throw(RSGISAttributeTableException);
        
        void calculateFieldsMUParser(std::string expression, std::string outField, RSGISAttributeDataType outFieldDT, std::vector<RSGISMathAttVariable*> *variables) throw(RSGISAttributeTableException);
        
        void copyValues(std::string fromField, std::string toField) throw(RSGISAttributeTableException);
        
        void exportASCII(std::string outFile) throw(RSGISAttributeTableException);
        
        void exportHDF5(std::string outFile) throw(RSGISAttributeTableException);
        
        void exportGDALRaster(GDALDataset *inDataset, unsigned int inBand) throw(RSGISAttributeTableException);
        
        std::vector<double>* getDoubleField(std::string field) throw(RSGISAttributeTableException);
        std::vector<long>* getLongField(std::string field) throw(RSGISAttributeTableException);
        std::vector<bool>* getBoolField(std::string field) throw(RSGISAttributeTableException);
        std::vector<std::string>* getStringField(std::string field) throw(RSGISAttributeTableException);
        
        virtual void operator++()=0;
        virtual void start()=0;
        virtual bool end()=0;
        virtual RSGISFeature* operator*()=0;
        
        virtual ~RSGISAttributeTable();

        static std::vector<RSGISIfStatement*>* generateStatments(xercesc::DOMElement *argElement)throw(RSGISAttributeTableException);
        static RSGISAttExpression* generateExpression(xercesc::DOMElement *expElement)throw(RSGISAttributeTableException);
        
        static void printSummaryHDFAtt(std::string inFile)throw(RSGISAttributeTableException);
        
    protected:
        RSGISAttributeTable();
        static H5::CompType* createAttibuteIdxCompTypeDisk() throw(RSGISAttributeTableException);
        static H5::CompType* createAttibuteIdxCompTypeMem() throw(RSGISAttributeTableException);
        void freeFeature(RSGISFeature *feat);
        std::vector<std::pair<std::string, RSGISAttributeDataType> > *fields;
        std::map<std::string, unsigned int> *fieldIdx;
        std::map<std::string, RSGISAttributeDataType> *fieldDataType;
        unsigned int numBoolFields;
        unsigned int numIntFields;
        unsigned int numFloatFields;
        unsigned int numStrFields;
    };
    
    
    class RSGISAttExpression2Fields : public RSGISAttExpression
    {
    public:
        RSGISAttExpression2Fields(std::string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, std::string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression()
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
        std::string f1Name;
        unsigned int f1Idx;
        RSGISAttributeDataType f1Type;
        std::string f2Name;
        unsigned int f2Idx;
        RSGISAttributeDataType f2Type;
    };
    
    class RSGISAttExpressionFieldAndValue : public RSGISAttExpression
    {
    public:
        RSGISAttExpressionFieldAndValue(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpression()
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
        virtual void setValue(float value)
        {
            this->value = value;
        };
        virtual ~RSGISAttExpressionFieldAndValue()
        {
            
        };
    protected:
        std::string fName;
        unsigned int fIdx;
        RSGISAttributeDataType fType;
        float value;
    };
    
    class RSGISAttExpressionAND : public RSGISAttExpression
    {
    public:
        RSGISAttExpressionAND(std::vector<RSGISAttExpression*> *exps):RSGISAttExpression()
        {
            this->exps = exps;
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            bool outVal = true;
            try
            {
                for(std::vector<RSGISAttExpression*>::iterator iterExp = exps->begin(); iterExp != exps->end(); ++iterExp)
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
                for(std::vector<RSGISAttExpression*>::iterator iterExp = exps->begin(); iterExp != exps->end(); ++iterExp)
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
                for(std::vector<RSGISAttExpression*>::iterator iterExp = exps->begin(); iterExp != exps->end(); ++iterExp)
                {
                    delete *iterExp;
                }
                delete exps;
            }
        };
    private:
        std::vector<RSGISAttExpression*> *exps;
    };
    
    class RSGISAttExpressionOR : public RSGISAttExpression
    {
    public:
        RSGISAttExpressionOR(std::vector<RSGISAttExpression*> *exps):RSGISAttExpression()
        {
            this->exps = exps;
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            bool outVal = false;
            try
            {
                for(std::vector<RSGISAttExpression*>::iterator iterExp = exps->begin(); iterExp != exps->end(); ++iterExp)
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
                for(std::vector<RSGISAttExpression*>::iterator iterExp = exps->begin(); iterExp != exps->end(); ++iterExp)
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
                for(std::vector<RSGISAttExpression*>::iterator iterExp = exps->begin(); iterExp != exps->end(); ++iterExp)
                {
                    delete *iterExp;
                }
                delete exps;
            }
        };
    private:
        std::vector<RSGISAttExpression*> *exps;
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
    
    
    class RSGISAttExpressionBoolField : public RSGISAttExpression
    {
    public:
        RSGISAttExpressionBoolField(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType):RSGISAttExpression()
        {
            this->fName = fName;
            this->fIdx = fIdx;
            this->fType = fType;
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            if(fType != rsgis_bool)
            {
                throw RSGISAttributeTableException("Field has to be of type boolean.");
            }
            
            return feat->boolFields->at(this->fIdx);
        };
        void popIdxs(RSGISAttributeTable *att)throw(RSGISAttributeTableException)
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
        ~RSGISAttExpressionBoolField()
        {
            
        };
    protected:
        std::string fName;
        unsigned int fIdx;
        RSGISAttributeDataType fType;
    };
    
    class RSGISAttExpressionNotBoolField : public RSGISAttExpression
    {
    public:
        RSGISAttExpressionNotBoolField(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType):RSGISAttExpression()
        {
            this->fName = fName;
            this->fIdx = fIdx;
            this->fType = fType;
        };
        bool evaluate(RSGISFeature *feat)throw(RSGISAttributeTableException)
        {
            if(fType != rsgis_bool)
            {
                throw RSGISAttributeTableException("Field has to be of type boolean.");
            }
            
            return !feat->boolFields->at(this->fIdx);
        };
        void popIdxs(RSGISAttributeTable *att)throw(RSGISAttributeTableException)
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
        ~RSGISAttExpressionNotBoolField()
        {
            
        };
    protected:
        std::string fName;
        unsigned int fIdx;
        RSGISAttributeDataType fType;
    };
    
    
    class RSGISAttExpressionGreaterThan : public RSGISAttExpression2Fields 
    {
    public:
        RSGISAttExpressionGreaterThan(std::string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, std::string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression2Fields(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type)
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
        RSGISAttExpressionLessThan(std::string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, std::string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression2Fields(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type)
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
        RSGISAttExpressionGreaterThanEq(std::string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, std::string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression2Fields(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type)
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
        RSGISAttExpressionLessThanEq(std::string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, std::string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression2Fields(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type)
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
        RSGISAttExpressionEquals(std::string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, std::string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression2Fields(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type)
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
        RSGISAttExpressionNotEquals(std::string f1Name, unsigned int f1Idx, RSGISAttributeDataType f1Type, std::string f2Name, unsigned int f2Idx, RSGISAttributeDataType f2Type):RSGISAttExpression2Fields(f1Name, f1Idx, f1Type, f2Name, f2Idx, f2Type)
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
        RSGISAttExpressionGreaterThanConst(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
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
        RSGISAttExpressionLessThanConst(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
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
        RSGISAttExpressionGreaterThanConstEq(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
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
        RSGISAttExpressionLessThanConstEq(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
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
        RSGISAttExpressionEqualsConst(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
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
        RSGISAttExpressionNotEqualsConst(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
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
        RSGISAttExpressionConstGreaterThan(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
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
        RSGISAttExpressionConstLessThan(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
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
        RSGISAttExpressionConstGreaterThanEq(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
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
        RSGISAttExpressionConstLessThanEq(std::string fName, unsigned int fIdx, RSGISAttributeDataType fType, float value):RSGISAttExpressionFieldAndValue(fName, fIdx, fType, value)
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


