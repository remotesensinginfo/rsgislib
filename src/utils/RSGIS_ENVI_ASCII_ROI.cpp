/*
 *  RSGIS_ENVI_ASCII_ROI.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/08/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#include "RSGIS_ENVI_ASCII_ROI.h"

namespace rsgis{namespace utils{
	
	RSGISReadENVIASCIIROI::RSGISReadENVIASCIIROI(string file) throw(RSGISInputStreamException,RSGISTextException)
	{
		this->inputfile = file;
		this->parsefile();
	}
	
	void RSGISReadENVIASCIIROI::printROIs()
	{
		RSGISMatrices matrixUtils;
		for(int i = 0; i < this->numrois; i++)
		{
			cout << "ROI " << rois[i].name << " has " << rois[i].samples << endl;
			matrixUtils.printMatrix(rois[i].data);
		}
	}
	
	int RSGISReadENVIASCIIROI::getNumROIs()
	{
		return numrois;
	}
	
	string* RSGISReadENVIASCIIROI::getName(int i) throw(RSGISENVIROIException)
	{
		if(i < 0 & i >= numrois)
		{
			throw RSGISENVIROIException("There are insufficient ROIs in datastructure..");
		}
		return &rois[i].name;
	}
	
	Matrix* RSGISReadENVIASCIIROI::getMatrix(int i) throw(RSGISENVIROIException)
	{
		if(i < 0 & i >= numrois)
		{
			throw RSGISENVIROIException("There are insufficient ROIs in datastructure..");
		}
		return rois[i].data;
	}
	
	int RSGISReadENVIASCIIROI::getNumSamples(int i) throw(RSGISENVIROIException)
	{
		if(i < 0 & i >= numrois)
		{
			throw RSGISENVIROIException("There are insufficient ROIs in datastructure..");
		}
		return rois[i].samples;
	}
	
	RSGISColour* RSGISReadENVIASCIIROI::getColour(int i) throw(RSGISENVIROIException)
	{
		if(i < 0 & i >= numrois)
		{
			throw RSGISENVIROIException("There are insufficient ROIs in datastructure..");
		}
		return rois[i].colour;
	}
	
	int RSGISReadENVIASCIIROI::getNumVariables()
	{
		return numVariables;
	}
	
	enviroi* RSGISReadENVIASCIIROI::getENVIROI(int i) throw(RSGISENVIROIException)
	{
		if(i < 0 & i >= numrois)
		{
			throw RSGISENVIROIException("There are insufficient ROIs in datastructure..");
		}
		return &rois[i];
	}
			
	RSGISReadENVIASCIIROI::~RSGISReadENVIASCIIROI()
	{
		
	}
	
	void RSGISReadENVIASCIIROI::parsefile() throw(RSGISInputStreamException,RSGISTextException)
	{
		RSGISMatrices matrixUtils;
		RSGISTextUtils textUtils;
		ifstream inputROIFile;
		inputROIFile.open(inputfile.c_str());
		if(!inputROIFile.is_open())
		{
			string message = string("Could not open input text file: ") + inputfile;
			throw RSGISInputStreamException(message);
		}
		else
		{
			//cout << "ROI file open\n";
			string strLine;
			string word;
			int lineCounter = 0;
			int roicount = 0;
			int headingsLine = 0;
			int sampleCount = 0;
			int variable = 0;
			int dataindex = 0;
			int dataStart = 0;
			inputROIFile.seekg(ios_base::beg);
			vector<string> *tokens = new vector<string>();
			while(!inputROIFile.eof())
			{
				getline(inputROIFile, strLine);
				if(textUtils.lineStart(strLine, ';'))
				{
					// Header
					//cout << lineCounter << ") " << strLine << endl;
					
					if(lineCounter == 0)
					{
						textUtils.tokenizeString(strLine, ' ', tokens, true);
						for(unsigned int i = 0; i < tokens->size(); i++)
						{
							//cout << i << ") " << tokens->at(i) << endl;
							if(tokens->at(1) == string("ENVI") &
							   tokens->at(2) == string("Output") &
							   tokens->at(3) == string("of") &
							   tokens->at(4) == string("ROIs"))
							{
								// correct file type;
							}
							else
							{
								throw RSGISTextException("Incorrect file format");
							}
						}
						tokens->clear();
					}
					else if(lineCounter == 1)
					{
						textUtils.tokenizeString(strLine, ':', tokens, true);
						numrois = strtol(tokens->at(1).c_str(), NULL, 10);
						//cout << "Number of ROI\'s = " << numrois << endl;
						rois = new enviroi[numrois];
						roicount = 0;
						tokens->clear();
						headingsLine = (3 * numrois) + 3 + numrois;
					}
					else
					{
						if(lineCounter == headingsLine)
						{
							//cout << "Headings: " << strLine << endl;
							textUtils.tokenizeString(strLine, ' ', tokens, true);
							//cout << "Number tokens = " << tokens->size() << endl;
							int varDiff = 0;
							string b1 = string("B1");
							for(unsigned int i = 0; i < tokens->size(); i++)
							{
								//cout << i << ") \'" << tokens->at(i) << "\'" << endl;
								if(string(tokens->at(i)) == b1)
								{
									varDiff = i;
								}
							}
							//cout << "varDiff = " << varDiff << endl;
							//cout << "numVaribles (using varDiff) = " << tokens->size() - varDiff << endl;
							numVariables = tokens->size() - varDiff;
							
							if(varDiff == 4)
							{
								dataStart = varDiff - 1;
							}
							else
							{
								dataStart = varDiff - 3;
							}
							cout << "There are " << numrois << " roi's where each sample has " << numVariables << " variables\n";
							for(int i = 0; i < numrois; i++)
							{
								cout << "ROI " << rois[i].name << " has " << rois[i].samples << " samples\n";
								rois[i].data = matrixUtils.createMatrix(numVariables, rois[i].samples);
								//matrixUtils.printMatrix(rois[i].data);
							}
							tokens->clear();
							roicount = 0;
						}
						else
						{
							textUtils.tokenizeString(strLine, ':', tokens, true);
							for(unsigned int i = 0; i < tokens->size(); i++)
							{
								//cout << i << ") \'" << tokens->at(i) << "\'" << endl;
								if(tokens->at(i) == string("; ROI name"))
								{
									rois[roicount].name = tokens->at(i+1);
								}
								else if(tokens->at(i) == string("; ROI rgb value"))
								{
									rois[roicount].colour = new RSGISColour();
								}
								else if(tokens->at(i) == string("; ROI npts"))
								{
									rois[roicount].samples = strtol(tokens->at(1).c_str(), NULL, 10);
									roicount++;
								}
							}
							tokens->clear();
						}
					}
				}
				else if(textUtils.blankline(strLine))
				{
					cout << "START OF NEW ROI DATA\n";
					roicount++;
					sampleCount = 0;
					dataindex = 0;
				}
				else
				{
					// DATA
					//cout << "dataStart = " << dataStart << endl;
					//cout << lineCounter << ") " << strLine << endl;
					//cout << sampleCount << "] ROI: " << roicount << endl;
					variable = 0;
					textUtils.tokenizeString(strLine, ' ', tokens, true);
					//cout << "Found " << tokens->size() << " tokens\n";
					for(unsigned int i = dataStart; i < tokens->size(); i++)
					{
						rois[roicount].data->matrix[dataindex++] = strtod(tokens->at(i).c_str(), NULL);
						//cout << variable << ") " << tokens->at(i) << endl;
						variable++;
					}
					tokens->clear();
					sampleCount++;
				}
				lineCounter++;
			}
		}
	}

}}

