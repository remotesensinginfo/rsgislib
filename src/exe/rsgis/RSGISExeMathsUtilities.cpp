/*
 *  RSGISExeMathsUtilities.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 08/12/2008.
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

#include "RSGISExeMathsUtilities.h"

namespace rsgisexe{

RSGISExeMathsUtilities::RSGISExeMathsUtilities() : RSGISAlgorithmParameters()
{
	this->algorithm = "mathsutils";
	
	this->inputMatrix = "";
	this->outputMatrix = "";
	this->outputMeanVector = "";
	this->outputEigenVector = "";
	this->outputImage = "";	
	this->xDup = 0;
	this->yDup = 0;
	this->option = RSGISExeMathsUtilities::none;
}

RSGISAlgorithmParameters* RSGISExeMathsUtilities::getInstance()
{
	return new RSGISExeMathsUtilities();
}

void RSGISExeMathsUtilities::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{	
	RSGISMathsUtils mathUtils;
	
	const XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	const XMLCh *optionEigenVector = XMLString::transcode("eigenvectors");
	const XMLCh *optionMeanVector = XMLString::transcode("meanvector");
	const XMLCh *optionEigenMeanVector = XMLString::transcode("eigenmeanvector");
	const XMLCh *optionMatrix2Image = XMLString::transcode("matrix2image");
	const XMLCh *optionDupMatrix = XMLString::transcode("duplicatematrix");
	
	const XMLCh *algorNameEle = argElement->getAttribute(XMLString::transcode("algor"));
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	
	const XMLCh *matrix = argElement->getAttribute(XMLString::transcode("matrix"));
	this->inputMatrix = XMLString::transcode(matrix);
	
	const XMLCh *optionXML = argElement->getAttribute(XMLString::transcode("option"));
	if(XMLString::equals(optionEigenVector, optionXML))
	{		
		this->option = RSGISExeMathsUtilities::eigenvectors;
		
		const XMLCh *eigen = argElement->getAttribute(XMLString::transcode("eigen"));
		this->outputEigenVector = XMLString::transcode(eigen);
	}
	else if(XMLString::equals(optionMeanVector, optionXML))
	{		
		this->option = RSGISExeMathsUtilities::meanvector;
		
		const XMLCh *mean = argElement->getAttribute(XMLString::transcode("mean"));
		this->outputMeanVector = XMLString::transcode(mean);
	}
	else if(XMLString::equals(optionEigenMeanVector, optionXML))
	{		
		this->option = RSGISExeMathsUtilities::eigenmeanvector;
		
		const XMLCh *eigen = argElement->getAttribute(XMLString::transcode("eigen"));
		this->outputEigenVector = XMLString::transcode(eigen);
		
		const XMLCh *mean = argElement->getAttribute(XMLString::transcode("mean"));
		this->outputMeanVector = XMLString::transcode(mean);
	}
	else if(XMLString::equals(optionMatrix2Image, optionXML))
	{		
		this->option = RSGISExeMathsUtilities::matrix2image;
		
		const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
		this->outputImage = XMLString::transcode(output);
	}
	else if(XMLString::equals(optionDupMatrix, optionXML))
	{		
		this->option = RSGISExeMathsUtilities::duplicatematrix;
		
		const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
		this->outputMatrix = XMLString::transcode(output);
		
		const XMLCh *xDupStr = argElement->getAttribute(XMLString::transcode("xdup"));
		this->xDup = mathUtils.strtoint(XMLString::transcode(xDupStr));
		
		const XMLCh *yDupStr = argElement->getAttribute(XMLString::transcode("ydup"));
		this->yDup = mathUtils.strtoint(XMLString::transcode(yDupStr));
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeMathUtilities.");
		throw RSGISXMLArgumentsException(message.c_str());
	}
	
	parsed = true;
}

void RSGISExeMathsUtilities::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(option == RSGISExeMathsUtilities::eigenvectors)
		{
			RSGISPrincipalComponentAnalysis *pca = NULL;
			RSGISMatrices matrixUtils;
			Matrix *eigenVectors = NULL;
			Matrix *matrix = NULL;
			try
			{
				matrix = matrixUtils.readMatrixFromTxt(this->inputMatrix);
				pca = new RSGISPrincipalComponentAnalysis(matrix);
				eigenVectors = pca->getEigenvectors();
				matrixUtils.saveMatrix2txt(eigenVectors, this->outputEigenVector);
				delete pca;
				matrixUtils.freeMatrix(matrix);
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeMathsUtilities::meanvector)
		{
			RSGISMultivariantStats *mvarStats;
			RSGISMatrices matrixUtils;
			Matrix *meanVector = NULL;
			Matrix *matrix = NULL;
			try
			{
				mvarStats = new RSGISMultivariantStats();
				matrix = matrixUtils.readMatrixFromTxt(this->inputMatrix);
				meanVector = mvarStats->findMeanVector(matrix);
				matrixUtils.saveMatrix2txt(meanVector, this->outputMeanVector);
				matrixUtils.freeMatrix(matrix);
				matrixUtils.freeMatrix(meanVector);
				delete mvarStats;
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeMathsUtilities::eigenmeanvector)
		{
			RSGISPrincipalComponentAnalysis *pca = NULL;
			RSGISMatrices matrixUtils;
			Matrix *eigenVectors = NULL;
			Matrix *meanVector = NULL;
			Matrix *matrix = NULL;
			try
			{
				matrix = matrixUtils.readMatrixFromTxt(this->inputMatrix);
				pca = new RSGISPrincipalComponentAnalysis(matrix);
				eigenVectors = pca->getEigenvectors();
				matrixUtils.saveMatrix2txt(eigenVectors, this->outputEigenVector);
				meanVector = pca->getMeanVector();
				matrixUtils.saveMatrix2txt(meanVector, this->outputMeanVector);
				delete pca;
				matrixUtils.freeMatrix(matrix);
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeMathsUtilities::matrix2image)
		{
			RSGISMatrices matrixUtils;
			Matrix *matrix = NULL;
			try
			{
				matrix = matrixUtils.readMatrixFromTxt(this->inputMatrix);
				matrixUtils.exportAsImage(matrix, this->outputImage);
				matrixUtils.freeMatrix(matrix);
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeMathsUtilities::duplicatematrix)
		{
			RSGISMatrices matrixUtils;
			Matrix *matrix = NULL;
			Matrix *dupMatrix = NULL;
			try
			{
				matrix = matrixUtils.readMatrixFromTxt(this->inputMatrix);
				dupMatrix = matrixUtils.duplicateMatrix(matrix, this->xDup, this->yDup);
				matrixUtils.saveMatrix2txt(dupMatrix, this->outputMatrix);
				matrixUtils.freeMatrix(matrix);
				matrixUtils.freeMatrix(dupMatrix);
				
			}
			catch(RSGISException e)
			{
				throw e;
			}
		}
		else
		{
			throw RSGISException("Option not recognised");
		}		
	}
}


void RSGISExeMathsUtilities::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeMathsUtilities::eigenvectors)
		{
			cout << "input matrix: " << this->inputMatrix << endl;
			cout << "Output matrix: " << this->outputEigenVector << endl;
		}
		else if(option == RSGISExeMathsUtilities::meanvector)
		{
			cout << "input matrix: " << this->inputMatrix << endl;
			cout << "Output matrix: " << this->outputMeanVector << endl;
		}
		else if(option == RSGISExeMathsUtilities::eigenmeanvector)
		{
			cout << "input matrix: " << this->inputMatrix << endl;
			cout << "Output matrix (Eigen): " << this->outputEigenVector << endl;
			cout << "Output matrix (Mean): " << this->outputMeanVector << endl;
		}
		else if(option == RSGISExeMathsUtilities::matrix2image)
		{
			cout << "input matrix: " << this->inputMatrix << endl;
			cout << "Output Image: " << this->outputImage << endl;
		}
		else if(option == RSGISExeMathsUtilities::duplicatematrix)
		{
			cout << "input matrix: " << this->inputMatrix << endl;
			cout << "Output matrix: " << this->outputMatrix << endl;
			cout << "X duplication: " << this->xDup << endl;
			cout << "Y duplication: " << this->yDup << endl;
		}
		else
		{
			cout << "Options not recognised\n";
		}
	}
	else
	{
		cout << "The parameters have yet to be parsed\n";
	}
}

void RSGISExeMathsUtilities::help()
{
	
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
	cout << "\t<rsgis:command algor=\"mathsutils\" option=\"eigenvectors\" matrix=\"matrix.mtxt\" eigen=\"out_matrix.mtxt\" />\n";
	cout << "\t<rsgis:command algor=\"mathsutils\" option=\"meanvector\" matrix=\"matrix.mtxt\" mean=\"out_matrix.mtxt\" />\n";
	cout << "\t<rsgis:command algor=\"mathsutils\" option=\"eigenmeanvector\" matrix=\"matrix.mtxt\" eigen=\"out_matrix.mtxt\" mean=\"out_matrix.mtxt\" />\n";
	cout << "\t<rsgis:command algor=\"mathsutils\" option=\"matrix2image\" matrix=\"matrix.mtxt\" output=\"out_image.env\" />\n";
	cout << "\t<rsgis:command algor=\"mathsutils\" option=\"duplicatematrix\" matrix=\"matrix.mtxt\" output=\"out_matrix.mtxt\" xdup=\"int\" ydup=\"int\" />\n";
	cout << "</rsgis:commands>\n";
}

string RSGISExeMathsUtilities::getDescription()
{
	return "Interface of a series of general maths utilities.";
}

string RSGISExeMathsUtilities::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeMathsUtilities::~RSGISExeMathsUtilities()
{
	
}

}


