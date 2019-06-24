/*
 *  RSGISMatrices.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/06/2008.
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

#ifndef RSGISMatrices_H
#define RSGISMatrices_H

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include "gdal_priv.h"

#include "math/RSGISMatricesException.h"
#include "common/RSGISInputStreamException.h"
#include "common/RSGISOutputStreamException.h"

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_math.h>

#include <boost/math/special_functions/fpclassify.hpp>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
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
	/// Utilities for RSGIS and GSL matrices
    
	/**
	 * m - x axis
	 * n - y axis
	 */
	struct DllExport Matrix
	{
		double *matrix;
		int m;
		int n;
	};
	
	enum outTXTform 
	{
		mtxt,
		gtxt,
		csv
	};
	
	
	class DllExport RSGISMatrices
		{
		public:
			RSGISMatrices();
			Matrix* createMatrix(int n, int m);
			Matrix* createMatrix(Matrix *matrix);
			Matrix* copyMatrix(Matrix *matrix);
			void freeMatrix(Matrix *matrix);
            void setValues(Matrix *matrix, double val);
			double determinant(Matrix *matrix);
			Matrix* cofactors(Matrix *matrix);
			Matrix* transpose(Matrix *matrix);
			void transposeGSL(gsl_matrix *inMatrix, gsl_matrix *outMatrix);
			void transposeNonSquareGSL(gsl_matrix *inMatrix, gsl_matrix *outMatrix);
			void inv2x2GSLMatrix(gsl_matrix * inMatrix, gsl_matrix *outMatrix);
			void multipleSingle(Matrix *matrix, double multiple);
			Matrix* multiplication(Matrix *matrixA, Matrix *matrixB);
			void productMatrixVectorGSL(gsl_matrix *inMatrix, gsl_vector *inVector, gsl_vector *outVector);
			void printMatrix(Matrix *matrix);
			void printGSLMatrix(gsl_matrix *matrix);
			void saveMatrix2GridTxt(Matrix *matrix, std::string filepath);
			void saveMatrix2CSV(Matrix *matrix, std::string filepath);
			void saveMatrix2txt(Matrix *matrix, std::string filepath);
			void saveMatrix2txtOptions(Matrix *matrix, std::string filepath, outTXTform outtxt);
			void saveMatrix2Binary(Matrix *matrix, std::string filepath);
			void saveGSLMatrix2GridTxt(gsl_matrix *gslMatrix, std::string filepath);
			void saveGSLMatrix2Txt(gsl_matrix *gslMatrix, std::string filepath);
			void saveGSLMatrix2CSV(gsl_matrix *gslMatrix, std::string filepath);
            void exportAsImage(Matrix *matrix, std::string filepath, std::string format="KEA");
			Matrix* readMatrixFromTxt(std::string filepath);
			Matrix* readMatrixFromGridTxt(std::string filepath);
			Matrix* readMatrixFromBinary(std::string filepath);
			gsl_matrix* readGSLMatrixFromTxt(std::string filepath);
			gsl_matrix* readGSLMatrixFromGridTxt(std::string filepath);
			gsl_matrix* readGSLMatrixFromBinary(std::string filepath);
			void calcEigenVectorValue(Matrix *matrix, Matrix *eigenvalues, Matrix *eigenvectors);
			Matrix* normalisedMatrix(Matrix *matrix, double min, double max);
            Matrix* normalisedColumnsMatrix(Matrix *matrix);
            gsl_matrix* normalisedColumnsMatrix(gsl_matrix *matrix);
			Matrix* duplicateMatrix(Matrix *matrix, int xDuplications, int yDuplications);
			gsl_matrix* convertRSGIS2GSLMatrix(Matrix *matrix);
			Matrix* convertGSL2RSGISMatrix(gsl_matrix *matrix);
            void makeCircularBinaryMatrix(Matrix *matrix);
			~RSGISMatrices();
        protected:
            std::string getFileExt(std::string filepath)
            {
                int strSize = filepath.size();
                int lastpt = 0;
                for(int i = 0; i < strSize; i++)
                {
                    if(filepath.at(i) == '.')
                    {
                        lastpt = i;
                    }
                }
                
                std::string extension = filepath.substr(lastpt);
                return extension;	
            }
		};
}}

#endif
