/*
 *  RSGISExeFFTUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/12/2008.
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

#include "RSGISExeFFTUtils.h"

namespace rsgisexe{

RSGISExeFFTUtils::RSGISExeFFTUtils() : RSGISAlgorithmParameters()
{
	this->algorithm = "fftutils";
	this->inputImage = "";
	this->inputMatrix = "";
	this->outputImage = "";
	this->outputMatrix = "";
	this->imageband = 0;
	this->option = RSGISExeFFTUtils::none;
}

RSGISAlgorithmParameters* RSGISExeFFTUtils::getInstance()
{
	return new RSGISExeFFTUtils();
}

void RSGISExeFFTUtils::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;
	
	const XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	const XMLCh *optionFFTMatrix = XMLString::transcode("fftmatrix");
	const XMLCh *optionFFTImage = XMLString::transcode("fftimage");
	const XMLCh *optionSizeAngle = XMLString::transcode("sizeangle");
	const XMLCh *optionShiftFFTW = XMLString::transcode("shiftfftw");
	
	const XMLCh *algorNameEle = argElement->getAttribute(XMLString::transcode("algor"));
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	
	const XMLCh *optionXML = argElement->getAttribute(XMLString::transcode("option"));
	if(XMLString::equals(optionFFTImage, optionXML))
	{		
		this->option = RSGISExeFFTUtils::fftImage;
		
		const XMLCh *input = argElement->getAttribute(XMLString::transcode("image"));
		this->inputImage = XMLString::transcode(input);
		
		const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
		this->outputImage = XMLString::transcode(output);
		
		const XMLCh *band = argElement->getAttribute(XMLString::transcode("band"));
		this->imageband = mathUtils.strtoint(XMLString::transcode(band));
	}
	else if(XMLString::equals(optionFFTMatrix, optionXML))
	{		
		this->option = RSGISExeFFTUtils::fftMatrix;
		
		const XMLCh *input = argElement->getAttribute(XMLString::transcode("image"));
		this->inputImage = XMLString::transcode(input);
		
		const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
		this->outputMatrix = XMLString::transcode(output);
		
		const XMLCh *band = argElement->getAttribute(XMLString::transcode("band"));
		this->imageband = mathUtils.strtoint(XMLString::transcode(band));
	}
	else if(XMLString::equals(optionSizeAngle, optionXML))
	{
		if(argElement->hasAttribute(XMLString::transcode("matrix")))
		{
			this->option = RSGISExeFFTUtils::sizeAngleMatrix;
			
			const XMLCh *matrix = argElement->getAttribute(XMLString::transcode("matrix"));
			this->inputMatrix = XMLString::transcode(matrix);
		}
		else if(argElement->hasAttribute(XMLString::transcode("fft")))
		{
			this->option = RSGISExeFFTUtils::sizeAngleFFT;
			
			const XMLCh *input = argElement->getAttribute(XMLString::transcode("fft"));
			this->inputImage = XMLString::transcode(input);
		}
		else
		{
			this->option = RSGISExeFFTUtils::sizeAngleImage;
			
			const XMLCh *input = argElement->getAttribute(XMLString::transcode("image"));
			this->inputImage = XMLString::transcode(input);
			
			const XMLCh *band = argElement->getAttribute(XMLString::transcode("band"));
			this->imageband = mathUtils.strtoint(XMLString::transcode(band));
		}
	}
	else if(XMLString::equals(optionShiftFFTW, optionXML))
	{		
		this->option = RSGISExeFFTUtils::shiftFFTW;
		
		const XMLCh *input = argElement->getAttribute(XMLString::transcode("image"));
		this->inputImage = XMLString::transcode(input);
		
		const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
		this->outputImage= XMLString::transcode(output);
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeFFTUtils.");
		throw RSGISXMLArgumentsException(message.c_str());
	}
	
	parsed = true;
}

void RSGISExeFFTUtils::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(option == RSGISExeFFTUtils::fftImage)
		{
            /*
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			RSGISFFTWUtils fftwUtils;
			RSGISMatrices matrixUtils;	
			RSGISCalcImageSingle *calcImage = NULL;
			RSGISCalcImageSingleValue *img2matrix = NULL;
			Matrix *inMatrix = NULL;
			Matrix *freqMatrix = NULL;
			
			int width = 0;
			int height = 0;
			double outputValue = 0;
			
			try
			{
				datasets = new GDALDataset*[1];
				cout << "Reading in image " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				width = datasets[0]->GetRasterXSize();
				height = datasets[0]->GetRasterYSize();
				
				img2matrix = new RSGISImageBand2Matrix(datasets[0]->GetRasterCount(), this->imageband, width, height);
				calcImage = new RSGISCalcImageSingle(img2matrix);
				calcImage->calcImage(datasets, 1, &outputValue, this->imageband-1);
				inMatrix = dynamic_cast<RSGISImageBand2Matrix*>(img2matrix)->getMatrix();
				freqMatrix = fftwUtils.computeFFTW(inMatrix);
				matrixUtils.exportAsImage(freqMatrix, this->outputImage);
			}
			catch(RSGISException e)
			{
				throw RSGISException(e.what());
			}
			
			delete img2matrix;
			delete calcImage;
			GDALClose(datasets[0]);
             */
		}
		else if(option == RSGISExeFFTUtils::fftMatrix)
		{
            /*
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			RSGISFFTWUtils fftwUtils;
			RSGISMatrices matrixUtils;	
			RSGISCalcImageSingle *calcImage = NULL;
			RSGISCalcImageSingleValue *img2matrix = NULL;
			Matrix *inMatrix = NULL;
			Matrix *freqMatrix = NULL;
			Matrix *freqMatrixZeroCentre = NULL;
			
			int width = 0;
			int height = 0;
			double outputValue = 0;
			
			try
			{
				datasets = new GDALDataset*[1];
				cout << "Reading in image " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				width = datasets[0]->GetRasterXSize();
				height = datasets[0]->GetRasterYSize();
				
				img2matrix = new RSGISImageBand2Matrix(datasets[0]->GetRasterCount(), this->imageband, width, height);
				calcImage = new RSGISCalcImageSingle(img2matrix);
				calcImage->calcImage(datasets, 1, &outputValue, this->imageband);
				inMatrix = dynamic_cast<RSGISImageBand2Matrix*>(img2matrix)->getMatrix();
				freqMatrix = fftwUtils.computeFFTW(inMatrix);
				freqMatrixZeroCentre = matrixUtils.copyMatrix(freqMatrix);
				fftwUtils.fftwShift(freqMatrix, freqMatrixZeroCentre);
				matrixUtils.saveMatrix2txt(freqMatrix, this->outputMatrix);
				
				delete img2matrix;
				delete calcImage;
				GDALClose(datasets[0]);
			}
			catch(RSGISException e)
			{
				throw RSGISException(e.what());
			}
             */
		}
		else if(option == RSGISExeFFTUtils::sizeAngleMatrix)
		{
            /*
			RSGISFFTProcessing fftProcessing;
			RSGISFFTWUtils fftwUtils;
			RSGISMatrices matrixUtils;
			Matrix *freqMatrix = NULL;
			Matrix *image = NULL;
			Matrix *freqMatrixZeroCentre = NULL;
			
			try
			{
				image = matrixUtils.readMatrixFromTxt(this->inputMatrix);
				freqMatrix = fftwUtils.computeFFTW(image);
				freqMatrixZeroCentre = matrixUtils.copyMatrix(freqMatrix);
				fftwUtils.fftwShift(freqMatrix, freqMatrixZeroCentre);
				//fftProcessing.findDominateFreq(freqMatrixZeroCentre, dc, 1, 50);
			}
			catch(RSGISException e)
			{
				throw RSGISException(e.what());
			}
             */
		}
		else if(option == RSGISExeFFTUtils::sizeAngleImage)
		{
            /*
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			RSGISCalcImageSingle *calcImage = NULL;
			RSGISCalcImageSingleValue *img2matrix = NULL;
			RSGISFFTProcessing fftProcessing;
			RSGISFFTWUtils fftwUtils;
			RSGISMatrices matrixUtils;
			Matrix *freqMatrix = NULL;
			Matrix *inMatrix = NULL;
			Matrix *freqMatrixZeroCentre = NULL;

			
			int width = 0;
			int height = 0;
			double outputValue = 0;
			
			try
			{
				datasets = new GDALDataset*[1];
				cout << "Reading in image " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				width = datasets[0]->GetRasterXSize();
				height = datasets[0]->GetRasterYSize();
				
				img2matrix = new RSGISImageBand2Matrix(datasets[0]->GetRasterCount(), this->imageband, width, height);
				calcImage = new RSGISCalcImageSingle(img2matrix);
				calcImage->calcImage(datasets, 1, &outputValue, this->imageband);
				inMatrix = dynamic_cast<RSGISImageBand2Matrix*>(img2matrix)->getMatrix();
				freqMatrix = fftwUtils.computeFFTW(inMatrix);
				freqMatrixZeroCentre = matrixUtils.copyMatrix(freqMatrix);
				fftwUtils.fftwShift(freqMatrix, freqMatrixZeroCentre);
				//dupMatrix = matrixUtils.duplicateMatrix(freqMatrixZeroCentre, 4, 4);
				//int numBBoxes = 0;
				//Polygon **bboxes = fftProcessing.findDominateFreq(freqMatrixZeroCentre, 4, 20, &numBBoxes);
				
				//RSGISVectorIO vecIO;
				
				//vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/FFTW/shp_out_test/bbox.shp", bboxes, numBBoxes);
				
				delete img2matrix;
				delete calcImage;
				GDALClose(datasets[0]);
			}
			catch(RSGISException e)
			{
				throw RSGISException(e.what());
			}
             */
		}
		else if(option == RSGISExeFFTUtils::sizeAngleFFT)
		{
            /*
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			RSGISCalcImageSingle *calcImage = NULL;
			RSGISCalcImageSingleValue *img2matrix = NULL;
			RSGISFFTProcessing fftProcessing;
			RSGISFFTWUtils fftwUtils;
			RSGISMatrices matrixUtils;
			Matrix *freqMatrix = NULL;

			
			int width = 0;
			int height = 0;
			double outputValue = 0;
			
			try
			{
				datasets = new GDALDataset*[1];
				cout << "Reading in image " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				width = datasets[0]->GetRasterXSize();
				height = datasets[0]->GetRasterYSize();
				
				img2matrix = new RSGISImageBand2Matrix(datasets[0]->GetRasterCount(), this->imageband, width, height);
				calcImage = new RSGISCalcImageSingle(img2matrix);
				calcImage->calcImage(datasets, 1, &outputValue, this->imageband);
				freqMatrix = dynamic_cast<RSGISImageBand2Matrix*>(img2matrix)->getMatrix();
				//int numBBoxes = 0;
				//Polygon **bboxes = fftProcessing.findDominateFreq(freqMatrix, 4, 20, &numBBoxes);
				
				delete img2matrix;
				delete calcImage;
				GDALClose(datasets[0]);
			}
			catch(RSGISException e)
			{
				throw RSGISException(e.what());
			}
             */
		}
		else if(option == RSGISExeFFTUtils::shiftFFTW)
		{
            /*
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			
			RSGISCalcImageSingle *calcImage = NULL;
			RSGISCalcImageSingleValue *img2matrix = NULL;
			RSGISFFTWUtils fftwUtils;
			RSGISMatrices matrixUtils;
			Matrix *freqInMatrix = NULL;
			Matrix *freqOutMatrix = NULL;
			
			double outputValue;
			int width = 0;
			int height = 0;			
			try
			{
				datasets = new GDALDataset*[1];
				cout << "Reading in image " << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				width = datasets[0]->GetRasterXSize();
				height = datasets[0]->GetRasterYSize();
				
				cout << "Importing Image\n";
				img2matrix = new RSGISImageBand2Matrix(datasets[0]->GetRasterCount(), this->imageband, width, height);
				calcImage = new RSGISCalcImageSingle(img2matrix);
				calcImage->calcImage(datasets, 1, &outputValue, this->imageband);
				freqInMatrix = dynamic_cast<RSGISImageBand2Matrix*>(img2matrix)->getMatrix();
				cout << "Shifting Image\n";
				freqOutMatrix = matrixUtils.copyMatrix(freqInMatrix);
				fftwUtils.fftwShift(freqInMatrix, freqOutMatrix);
				cout << "Exporting Image\n";
				matrixUtils.exportAsImage(freqOutMatrix, this->outputImage);
				
				delete img2matrix;
				delete calcImage;
				GDALClose(datasets[0]);
			}
			catch(RSGISException e)
			{
				throw RSGISException(e.what());
			}
             */
		}
		else
		{
			cout << "Options not recognised\n";
		}		
	}
}


void RSGISExeFFTUtils::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeFFTUtils::fftImage)
		{
			cout << "input image: " << this->inputImage << endl;
			cout << "image band: " << this->imageband << endl;
			cout << "output image: " << this->outputImage << endl;
		}
		else if(option == RSGISExeFFTUtils::fftMatrix)
		{
			cout << "input image: " << this->inputImage << endl;
			cout << "image band: " << this->imageband << endl;
			cout << "output matrix: " << this->outputMatrix << endl;
		}
		else if(option == RSGISExeFFTUtils::sizeAngleMatrix)
		{
			cout << "input matrix: " << this->inputMatrix << endl;
		}
		else if(option == RSGISExeFFTUtils::sizeAngleImage)
		{
			cout << "input image: " << this->inputImage << endl;
			cout << "image band: " << this->imageband << endl;
		}
		else if(option == RSGISExeFFTUtils::sizeAngleFFT)
		{
			cout << "input image: " << this->inputImage << endl;
		}
		else if(option == RSGISExeFFTUtils::shiftFFTW)
		{
			cout << "input image: " << this->inputImage << endl;
			cout << "output image: " << this->outputImage << endl;
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

void RSGISExeFFTUtils::help()
{
	cout << "<rsgis:commands>\n";
	cout << "\t<rsgis:command algor=\"fftutils\" option=\"fftimage\" image=\"image.env\" output=\"image_out.env\" band=\"int\" />\n";
	cout << "\t<rsgis:command algor=\"fftutils\" option=\"fftmatrix\" image=\"image.env\" output=\"matrix_out\" band=\"int\" />\n";
	cout << "\t<rsgis:command algor=\"fftutils\" option=\"sizeangle\" matrix=\"matrix.mtxt\" />\n";
	cout << "\t<rsgis:command algor=\"fftutils\" option=\"sizeangle\" image=\"image.env\" band=\"int\" />\n";
	cout << "\t<rsgis:command algor=\"fftutils\" option=\"sizeangle\" fft=\"image.env\" />\n";
	cout << "\t<rsgis:command algor=\"fftutils\" option=\"shiftfftw\" image=\"image.env\" output=\"image_out.env\"/>\n";
	cout << "</rsgis:commands>\n";
}

string RSGISExeFFTUtils::getDescription()
{
	return "Provides access to algorithms using a FFT.";
}

string RSGISExeFFTUtils::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeFFTUtils::~RSGISExeFFTUtils()
{
	
}
    
}

