/*
 *  RSGISLogicExpEvaluation.cpp
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

#include "RSGISLogicExpEvaluation.h"

namespace rsgis{namespace math{


    bool RSGISLogicAndExpression::evaluate()throw(RSGISMathLogicException)
    {
        bool outVal = true;
        try
        {
            for(std::vector<RSGISLogicExpression*>::iterator iterExps = exps->begin(); iterExps != exps->end(); ++iterExps)
            {
                if((*iterExps)->evaluate() == false)
                {
                    outVal = false;
                    break;
                }
            }
        }
        catch (RSGISMathLogicException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        
        return outVal;
    }

    
    bool RSGISLogicOrExpression::evaluate()throw(RSGISMathLogicException)
    {
        bool outVal = false;
        try
        {
            for(std::vector<RSGISLogicExpression*>::iterator iterExps = exps->begin(); iterExps != exps->end(); ++iterExps)
            {
                if((*iterExps)->evaluate() == true)
                {
                    outVal = true;
                    break;
                }
            }
        }
        catch (RSGISMathLogicException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        
        return outVal;
    }
    

    bool RSGISLogicNotExpression::evaluate()throw(RSGISMathLogicException)
    {
        bool outVal = false;
        try
        {
            outVal = !(exp->evaluate());
        }
        catch (RSGISMathLogicException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        
        return outVal;
    }
    
    bool RSGISLogicEqualsExpression::evaluate()throw(RSGISMathLogicException)
    {
        bool outVal = true;
        try
        {
            bool first = true;
            bool tmpVal = true;
            
            for(std::vector<RSGISLogicExpression*>::iterator iterExps = exps->begin(); iterExps != exps->end(); ++iterExps)
            {
                if(first)
                {
                    tmpVal = (*iterExps)->evaluate();
                    first = false;
                }
                else if((*iterExps)->evaluate() != tmpVal)
                {
                    outVal = false;
                    break;
                }
            }
        }
        catch (RSGISMathLogicException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        
        return outVal;
    }
    
    
    

    bool RSGISLogicEqualsValueExpression::evaluate()throw(RSGISMathLogicException)
    {
        bool outVal = false;
        try
        {
            if((boost::math::isnan)(*val1))
            {
                throw RSGISMathLogicException("Value 1 is NaN.");
            }
            if((boost::math::isnan)(*val2))
            {
                throw RSGISMathLogicException("Value 2 is NaN.");
            }
            
            outVal = ((*val1) == (*val2));
        }
        catch (RSGISMathLogicException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        
        return outVal;
    }
    
    bool RSGISLogicGreaterThanValueExpression::evaluate()throw(RSGISMathLogicException)
    {
        bool outVal = false;
        try
        {
            if((boost::math::isnan)(*val1))
            {
                throw RSGISMathLogicException("Value 1 is NaN.");
            }
            if((boost::math::isnan)(*val2))
            {
                throw RSGISMathLogicException("Value 2 is NaN.");
            }
            
            outVal = ((*val1) > (*val2));
        }
        catch (RSGISMathLogicException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        
        return outVal;
    }
    
    bool RSGISLogicLessThanValueExpression::evaluate()throw(RSGISMathLogicException)
    {
        bool outVal = false;
        try
        {
            if((boost::math::isnan)(*val1))
            {
                throw RSGISMathLogicException("Value 1 is NaN.");
            }
            if((boost::math::isnan)(*val2))
            {
                throw RSGISMathLogicException("Value 2 is NaN.");
            }
            
            outVal = ((*val1) < (*val2));
        }
        catch (RSGISMathLogicException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        
        return outVal;
    }
    
    bool RSGISLogicGreaterEqualToValueExpression::evaluate()throw(RSGISMathLogicException)
    {
        bool outVal = false;
        try
        {
            if((boost::math::isnan)(*val1))
            {
                throw RSGISMathLogicException("Value 1 is NaN.");
            }
            if((boost::math::isnan)(*val2))
            {
                throw RSGISMathLogicException("Value 2 is NaN.");
            }
            
            outVal = ((*val1) >= (*val2));
        }
        catch (RSGISMathLogicException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        
        return outVal;
    }
    
    bool RSGISLogicLessEqualToValueExpression::evaluate()throw(RSGISMathLogicException)
    {
        bool outVal = false;
        try
        {
            if((boost::math::isnan)(*val1))
            {
                throw RSGISMathLogicException("Value 1 is NaN.");
            }
            if((boost::math::isnan)(*val2))
            {
                throw RSGISMathLogicException("Value 2 is NaN.");
            }
            
            outVal = ((*val1) <= (*val2));
        }
        catch (RSGISMathLogicException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        
        return outVal;
    }
    
    bool RSGISLogicNotValueExpression::evaluate()throw(RSGISMathLogicException)
    {
        bool outVal = false;
        try
        {
            if((boost::math::isnan)(*val1))
            {
                throw RSGISMathLogicException("Value 1 is NaN.");
            }
            if((boost::math::isnan)(*val2))
            {
                throw RSGISMathLogicException("Value 2 is NaN.");
            }
            
            outVal = ((*val1) != (*val2));
        }
        catch (RSGISMathLogicException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISMathLogicException(e.what());
        }
        
        return outVal;
    }
    
    
}}



