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
			Matrix* createMatrix(int n, int m) throw(RSGISMatricesException);
			Matrix* createMatrix(Matrix *matrix) throw(RSGISMatricesException);
			Matrix* copyMatrix(Matrix *matrix) throw(RSGISMatricesException);
			void freeMatrix(Matrix *matrix);
            void setValues(Matrix *matrix, double val) throw(RSGISMatricesException);
			double determinant(Matrix *matrix) throw(RSGISMatricesException);
			Matrix* cofactors(Matrix *matrix) throw(RSGISMatricesException);
			Matrix* transpose(Matrix *matrix) throw(RSGISMatricesException);
			void transposeGSL(gsl_matrix *inMatrix, gsl_matrix *outMatrix) throw(RSGISMatricesException);
			void transposeNonSquareGSL(gsl_matrix *inMatrix, gsl_matrix *outMatrix) throw(RSGISMatricesException);
			void inv2x2GSLMatrix(gsl_matrix * inMatrix, gsl_matrix *outMatrix) throw(RSGISMatricesException);
			void multipleSingle(Matrix *matrix, double multiple) throw(RSGISMatricesException);
			Matrix* multiplication(Matrix *matrixA, Matrix *matrixB) throw(RSGISMatricesException);
			void productMatrixVectorGSL(gsl_matrix *inMatrix, gsl_vector *inVector, gsl_vector *outVector) throw(RSGISMatricesException);
			void printMatrix(Matrix *matrix);
			void printGSLMatrix(gsl_matrix *matrix);
			void saveMatrix2GridTxt(Matrix *matrix, std::string filepath) throw(RSGISMatricesException,RSGISOutputStreamException);
			void saveMatrix2CSV(Matrix *matrix, std::string filepath) throw(RSGISMatricesException,RSGISOutputStreamException);
			void saveMatrix2txt(Matrix *matrix, std::string filepath) throw(RSGISMatricesException,RSGISOutputStreamException);
			void saveMatrix2txtOptions(Matrix *matrix, std::string filepath, outTXTform outtxt) throw(RSGISMatricesException,RSGISOutputStreamException);
			void saveMatrix2Binary(Matrix *matrix, std::string filepath) throw(RSGISMatricesException,RSGISOutputStreamException);
			void saveGSLMatrix2GridTxt(gsl_matrix *gslMatrix, std::string filepath) throw(RSGISMatricesException,RSGISOutputStreamException);
			void saveGSLMatrix2Txt(gsl_matrix *gslMatrix, std::string filepath) throw(RSGISMatricesException,RSGISOutputStreamException);
			void saveGSLMatrix2CSV(gsl_matrix *gslMatrix, std::string filepath) throw(RSGISMatricesException,RSGISOutputStreamException);
            void exportAsImage(Matrix *matrix, std::string filepath, std::string format="KEA") throw(RSGISMatricesException);
			Matrix* readMatrixFromTxt(std::string filepath) throw(RSGISMatricesException,RSGISInputStreamException);
			Matrix* readMatrixFromGridTxt(std::string filepath) throw(RSGISMatricesException,RSGISInputStreamException);
			Matrix* readMatrixFromBinary(std::string filepath) throw(RSGISMatricesException,RSGISInputStreamException);
			gsl_matrix* readGSLMatrixFromTxt(std::string filepath) throw(RSGISMatricesException);
			gsl_matrix* readGSLMatrixFromGridTxt(std::string filepath) throw(RSGISMatricesException);
			gsl_matrix* readGSLMatrixFromBinary(std::string filepath) throw(RSGISMatricesException);
			void calcEigenVectorValue(Matrix *matrix, Matrix *eigenvalues, Matrix *eigenvectors) throw(RSGISMatricesException);
			Matrix* normalisedMatrix(Matrix *matrix, double min, double max) throw(RSGISMatricesException);
            Matrix* normalisedColumnsMatrix(Matrix *matrix) throw(RSGISMatricesException);
            gsl_matrix* normalisedColumnsMatrix(gsl_matrix *matrix) throw(RSGISMatricesException);
			Matrix* duplicateMatrix(Matrix *matrix, int xDuplications, int yDuplications) throw(RSGISMatricesException);
			gsl_matrix* convertRSGIS2GSLMatrix(Matrix *matrix) throw(RSGISMatricesException);
			Matrix* convertGSL2RSGISMatrix(gsl_matrix *matrix) throw(RSGISMatricesException);
            void makeCircularBinaryMatrix(Matrix *matrix) throw(RSGISMatricesException);
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
