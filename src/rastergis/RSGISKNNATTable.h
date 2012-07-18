/*
 *  RSGISKNNATTable.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/06/2012.
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

#ifndef RSGISKNNATTable_H
#define RSGISKNNATTable_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <algorithm>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

#include "common/RSGISAttributeTableException.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
#include "math/RSGISMathException.h"

#include "rastergis/RSGISAttributeTable.h"

namespace rsgis{namespace rastergis{
    
    class RSGISKNNATTable
    {
    public:
        RSGISKNNATTable();
        void initKNN(RSGISAttributeTable *attTable, string trainField, string classField, bool limitToClass, int classVal, unsigned int k, float distThreshold, rsgis::math::rsgisdistmetrics distMetric, std::vector<string> *attributeNames)throw(RSGISAttributeTableException);
        virtual void performKNN()throw(RSGISAttributeTableException)=0;
        virtual ~RSGISKNNATTable();
    protected:
        double calcDist(rsgis::math::rsgisdistmetrics distMetric, std::vector<double> *vals1, std::vector<double> *vals2) throw(rsgis::math::RSGISMathException);
        rsgis::math::Vector* calcVariableMeans() throw(rsgis::math::RSGISMathException);
        rsgis::math::Matrix* calcCovarianceMatrix(rsgis::math::Vector *attMeans) throw(rsgis::math::RSGISMathException);
        double calcCovariance(RSGISAttribute *a, RSGISAttribute *b, double aMean, double bMean) throw(rsgis::math::RSGISMathException);
        bool initialised;
        RSGISAttributeTable *attTable;
        string trainField;
        unsigned int trainFieldIdx;
        RSGISAttributeDataType trainFieldDT;
        bool limitToClass;
        string classField;
        int classVal;
        unsigned int classFieldIdx;
        RSGISAttributeDataType classFieldDT;
        unsigned int k;
        float distThreshold;
        rsgis::math::rsgisdistmetrics distMetric;
        std::vector<RSGISAttribute*> attributes;
        std::vector< std::vector<double>* > *knownData;
        bool mahDistInit;
        gsl_matrix *invCovarianceMatrix;
        gsl_vector *dVals;
        gsl_vector *outVec;
    };
    
    
    class RSGISKNNATTableExtrapolation : public RSGISKNNATTable
    {
    public:
        RSGISKNNATTableExtrapolation(string valField);
        void performKNN()throw(RSGISAttributeTableException);
        ~RSGISKNNATTableExtrapolation();
    protected:
        double calcNewVal(unsigned int k, float distThreshold, rsgis::math::rsgisdistmetrics distMetric, std::vector<double> *knownVals, std::vector< std::vector<double>* > *knownData, std::vector<double> *unknownData) throw(rsgis::math::RSGISMathException);
        string valField;
        unsigned int valFieldIdx;
        RSGISAttributeDataType valFieldDT;
    };
    
}}

#endif
