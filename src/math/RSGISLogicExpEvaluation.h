/*
 *  RSGISLogicExpEvaluation.h
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

#ifndef RSGISLogicExpEvaluation_H
#define RSGISLogicExpEvaluation_H

#include <vector>
#include "math/RSGISMathsUtils.h"
#include "math/RSGISMathException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_maths_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace math{
    
    class DllExport RSGISMathLogicException : public RSGISMathException
    {
    public:
        RSGISMathLogicException() : RSGISMathException("A RSGISMathLogicException has been created.."){};
        RSGISMathLogicException(const char* message) : RSGISMathException(message){};
        RSGISMathLogicException(std::string message) : RSGISMathException(message){};
    };
    
    
	class DllExport RSGISLogicExpression
    {
    public:
        RSGISLogicExpression(std::string expName){this->expName = expName;};
        virtual bool evaluate()throw(RSGISMathLogicException) = 0;
        std::string getExpName(){return expName;};
        virtual ~RSGISLogicExpression(){};
    protected:
        std::string expName;
    };
    
    
    class DllExport RSGISLogicAndExpression : public RSGISLogicExpression
    {
    public:
        RSGISLogicAndExpression(std::vector<RSGISLogicExpression*> *exps) : RSGISLogicExpression("And")
        {
            this->exps = exps;
        };
        bool evaluate()throw(RSGISMathLogicException);
        ~RSGISLogicAndExpression()
        {
            for(std::vector<RSGISLogicExpression*>::iterator iterExps = exps->begin(); iterExps != exps->end(); ++iterExps)
            {
                delete *iterExps;
            }
            delete exps;
        };
    protected:
        std::vector<RSGISLogicExpression*> *exps;
    };
    
    class DllExport RSGISLogicOrExpression : public RSGISLogicExpression
    {
    public:
        RSGISLogicOrExpression(std::vector<RSGISLogicExpression*> *exps) : RSGISLogicExpression("Or")
        {
            this->exps = exps;
        };
        bool evaluate()throw(RSGISMathLogicException);
        ~RSGISLogicOrExpression()
        {
            for(std::vector<RSGISLogicExpression*>::iterator iterExps = exps->begin(); iterExps != exps->end(); ++iterExps)
            {
                delete *iterExps;
            }
            delete exps;
        };
    protected:
        std::vector<RSGISLogicExpression*> *exps;
    };
    
    class DllExport RSGISLogicNotExpression : public RSGISLogicExpression
    {
    public:
        RSGISLogicNotExpression(RSGISLogicExpression *exp) : RSGISLogicExpression("Not")
        {
            this->exp = exp;
        };
        bool evaluate()throw(RSGISMathLogicException);
        ~RSGISLogicNotExpression()
        {
            delete exp;
        };
    protected:
        RSGISLogicExpression *exp;
    };
    
    class DllExport RSGISLogicEqualsExpression : public RSGISLogicExpression
    {
    public:
        RSGISLogicEqualsExpression(std::vector<RSGISLogicExpression*> *exps) : RSGISLogicExpression("Equals")
        {
            this->exps = exps;
        };
        bool evaluate()throw(RSGISMathLogicException);
        ~RSGISLogicEqualsExpression()
        {
            for(std::vector<RSGISLogicExpression*>::iterator iterExps = exps->begin(); iterExps != exps->end(); ++iterExps)
            {
                delete *iterExps;
            }
            delete exps;
        };
    protected:
        std::vector<RSGISLogicExpression*> *exps;
    };
    
    class DllExport RSGISLogicEqualsValueExpression : public RSGISLogicExpression
    {
    public:
        RSGISLogicEqualsValueExpression(double *val1, double *val2) : RSGISLogicExpression("EqVals")
        {
            this->val1 = val1;
            this->val2 = val2;
        };
        bool evaluate()throw(RSGISMathLogicException);
        ~RSGISLogicEqualsValueExpression(){};
    protected:
        double *val1;
        double *val2;
    };
    
    class DllExport RSGISLogicGreaterThanValueExpression : public RSGISLogicExpression
    {
    public:
        RSGISLogicGreaterThanValueExpression(double *val1, double *val2) : RSGISLogicExpression("Greater Than")
        {
            this->val1 = val1;
            this->val2 = val2;
        };
        bool evaluate()throw(RSGISMathLogicException);
        
        ~RSGISLogicGreaterThanValueExpression(){};
    protected:
        double *val1;
        double *val2;
    };
    
    
    class DllExport RSGISLogicLessThanValueExpression : public RSGISLogicExpression
    {
    public:
        RSGISLogicLessThanValueExpression(double *val1, double *val2) : RSGISLogicExpression("Less Than")
        {
            this->val1 = val1;
            this->val2 = val2;
        };
        bool evaluate()throw(RSGISMathLogicException);
        ~RSGISLogicLessThanValueExpression(){};
    protected:
        double *val1;
        double *val2;
    };
    
    
    class DllExport RSGISLogicGreaterEqualToValueExpression : public RSGISLogicExpression
    {
    public:
        RSGISLogicGreaterEqualToValueExpression(double *val1, double *val2) : RSGISLogicExpression("Greater Than Equal")
        {
            this->val1 = val1;
            this->val2 = val2;
        };
        bool evaluate()throw(RSGISMathLogicException);
        ~RSGISLogicGreaterEqualToValueExpression(){};
    protected:
        double *val1;
        double *val2;
    };
    
    
    class DllExport RSGISLogicLessEqualToValueExpression : public RSGISLogicExpression
    {
    public:
        RSGISLogicLessEqualToValueExpression(double *val1, double *val2) : RSGISLogicExpression("Less Than Equal")
        {
            this->val1 = val1;
            this->val2 = val2;
        };
        bool evaluate()throw(RSGISMathLogicException);
        ~RSGISLogicLessEqualToValueExpression(){};
    protected:
        double *val1;
        double *val2;
    };
    
    class DllExport RSGISLogicNotValueExpression : public RSGISLogicExpression
    {
    public:
        RSGISLogicNotValueExpression(double *val1, double *val2) : RSGISLogicExpression("Not Equal")
        {
            this->val1 = val1;
            this->val2 = val2;
        };
        bool evaluate()throw(RSGISMathLogicException);
        ~RSGISLogicNotValueExpression(){};
    protected:
        double *val1;
        double *val2;
    };
    
    
    
}}

#endif


