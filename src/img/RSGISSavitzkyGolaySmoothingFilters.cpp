/*
 *  RSGISSavitzkyGolaySmoothingFilters.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/06/2010.
 *  Copyright 2010 Aberystwyth University. All rights reserved.
 *
 */

#include "RSGISSavitzkyGolaySmoothingFilters.h"


namespace rsgis{namespace img{
	

	RSGISSavitzkyGolaySmoothingFilters::RSGISSavitzkyGolaySmoothingFilters(int numberOutBands, int order, int window, rsgis::math::Vector *imagebandValues) : RSGISCalcImageValue(numberOutBands)
	{
		this->order = order;
		this->window = window;
		this->imagebandValues = imagebandValues;
	}
	
	void RSGISSavitzkyGolaySmoothingFilters::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
	{
		if(numBands != numOutBands)
		{
			throw RSGISImageCalcException("The number of input and output image bands needs to be equal.");
		}
		
		if(numBands != imagebandValues->n)
		{
			throw RSGISImageCalcException("The number of input images bands and defined values need to be equal");
		}
		
		int numCols = 2;
		int numRows = 0;
		int startVal = 0;
		
        rsgis::math::RSGISPolyFit polyFit;
		gsl_vector *coefficients = NULL;
		gsl_matrix *inputValues = NULL;
		
		for(int i = 0; i < numBands; ++i)
		{
			if((i-window) < 0)
			{
				numRows = (window + 1) + i;
				startVal = 0;
			}
			else if((i+window) >= numBands)
			{
				numRows = (window + 1) + (numBands - i);
				startVal = i - window;
			}
			else 
			{
				numRows = (window * 2) + 1;
				startVal = i - window;
			}
				
			inputValues = gsl_matrix_alloc (numRows,numCols);
			for(int j = 0; j < numRows; ++j)
			{
				gsl_matrix_set (inputValues, j, 0, imagebandValues->vector[(startVal+j)]);
				gsl_matrix_set (inputValues, j, 1, bandValues[(startVal+j)]);
			}
			
			coefficients = polyFit.PolyfitOneDimensionQuiet(this->order, inputValues);	
			
			double yPredicted = 0;
			for(int j = 0; j < order ; j++)
			{
				double xPow = pow(imagebandValues->vector[i], j); // x^n;
				double coeff = gsl_vector_get(coefficients, j); // a_n
				double coeffXPow = coeff * xPow; // a_n * x^n				
				yPredicted = yPredicted + coeffXPow;
			}
			output[i] = yPredicted;
			
			gsl_matrix_free(inputValues);
			gsl_vector_free(coefficients);
		}
	}
	
	void RSGISSavitzkyGolaySmoothingFilters::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISSavitzkyGolaySmoothingFilters::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISSavitzkyGolaySmoothingFilters::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISSavitzkyGolaySmoothingFilters::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	bool RSGISSavitzkyGolaySmoothingFilters::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	RSGISSavitzkyGolaySmoothingFilters::~RSGISSavitzkyGolaySmoothingFilters()
	{
		
	}
}}
