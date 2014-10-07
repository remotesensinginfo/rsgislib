/*
 *  RSGISPrincipalComponentAnalysis.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/08/2008.
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

#ifndef RSGISPrincipalComponentAnalysis_H
#define RSGISPrincipalComponentAnalysis_H

#include "math/RSGISMatricesException.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISMultivariantStats.h"
#include "math/RSGISMultivariantStatsException.h"

namespace rsgis{namespace math{
    	
	class DllExport RSGISPrincipalComponentAnalysis
		{
		public:
			RSGISPrincipalComponentAnalysis(Matrix *inputData) throw(RSGISMatricesException,RSGISMultivariantStatsException);
			Matrix* getMeanVector();
			Matrix* getStandardisedData();
			Matrix* getCovarianceMatrix();
			Matrix* getEigenvalues();
			Matrix* getEigenvectors();
			Matrix* getComponent(int component);
			Matrix* getComponents(int num);
			Matrix* getAllComponents();
			~RSGISPrincipalComponentAnalysis();
		protected:
			void calcIntermediateSteps() throw(RSGISMatricesException,RSGISMultivariantStatsException);
			Matrix *inputData;
			Matrix *stdInputData;
			Matrix *meanVec;
			Matrix *stdMeanVec;
			Matrix *covariance;
			Matrix *eigenvalues;
			Matrix *eigenvectors;
		};
}}

#endif

