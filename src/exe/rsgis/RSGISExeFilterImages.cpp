/*
 *  RSGISExeFilterImages.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/12/2008.
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

#include "RSGISExeFilterImages.h"


RSGISExeFilterImages::RSGISExeFilterImages() : RSGISAlgorithmParameters()
{
	this->algorithm = "imagefilter";
	this->option = RSGISExeFilterImages::none;
	this->inputImage = "";
	this->outputImageBase = "";
	this->filterBank = NULL;
}

RSGISAlgorithmParameters* RSGISExeFilterImages::getInstance()
{
	return new RSGISExeFilterImages();
}

void RSGISExeFilterImages::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;
	
	const XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	const XMLCh *optionFilter = XMLString::transcode("filter");
	const XMLCh *optionExportFilterBank = XMLString::transcode("exportfilterbank");
	
	const XMLCh *algorNameEle = argElement->getAttribute(XMLString::transcode("algor"));
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
	
	const XMLCh *input = argElement->getAttribute(XMLString::transcode("image"));
	this->inputImage = XMLString::transcode(input);
	
	const XMLCh *output = argElement->getAttribute(XMLString::transcode("output"));
	this->outputImageBase = XMLString::transcode(output);
	
	this->filterBank = new RSGISFilterBank();
	
	if(argElement->hasAttribute(XMLString::transcode("filterbank")))
	{
		const XMLCh *filterBankXMLStr = argElement->getAttribute(XMLString::transcode("filterbank"));
		cout << "Default Filter Bank " << XMLString::transcode(filterBankXMLStr) << " to be created\n";
		
		const XMLCh *filterBankLM = XMLString::transcode("LM");
		if(XMLString::equals(filterBankXMLStr, filterBankLM))
		{
			this->filterBank->createLeungMalikFilterBank();
		}
		cout << "Created filter banks\n";
	}
	else
	{
		DOMNodeList *filterNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:filter"));
		int numFilters = filterNodesList->getLength();
		DOMElement *filterElement = NULL;
		
		XMLCh *filterTypeGuassianSmooth = XMLString::transcode("GaussianSmooth");
		XMLCh *filterTypeGuassian1st = XMLString::transcode("Gaussian1st");
		XMLCh *filterTypeGuassian2nd = XMLString::transcode("Gaussian2nd");
		XMLCh *filterTypeLaplacian = XMLString::transcode("Laplacian");
		XMLCh *filterTypeSobel = XMLString::transcode("Sobel");
		XMLCh *filterTypePrewitt = XMLString::transcode("Prewitt");
		XMLCh *filterTypeMean = XMLString::transcode("Mean");
		XMLCh *filterTypeMedian = XMLString::transcode("Median");
		XMLCh *filterTypeMode = XMLString::transcode("Mode");
		XMLCh *filterTypeRange = XMLString::transcode("Range");
		XMLCh *filterTypeStdDev = XMLString::transcode("StdDev");
		XMLCh *filterTypeMin = XMLString::transcode("Min");
		XMLCh *filterTypeMax = XMLString::transcode("Max");
		XMLCh *filterTypeTotal = XMLString::transcode("Total");
		XMLCh *filterTypeKuwahara = XMLString::transcode("Kuwahara");
		XMLCh *filterTypeHarlick = XMLString::transcode("Harlick");
		XMLCh *filterTypeFree = XMLString::transcode("Free");
        XMLCh *filterTypeLee = XMLString::transcode("Lee");
		
		XMLCh *filterTypeOptionX = XMLString::transcode("x");
		XMLCh *filterTypeOptionY = XMLString::transcode("y");
		XMLCh *filterTypeOptionXY = XMLString::transcode("xy");
		
		for(int i = 0; i < numFilters; i++)
		{
			filterElement = static_cast<DOMElement*>(filterNodesList->item(i));
			const XMLCh *filterType = filterElement->getAttribute(XMLString::transcode("type"));
			cout << "Filter: " << XMLString::transcode(filterType) << endl;
			
			if(XMLString::equals(filterTypeGuassianSmooth, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
                
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				float stddevX = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("stddevX"))));
				float stddevY = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("stddevY"))));
				float angle = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("angle"))));
				float angleRadians = angle*(M_PI/180);
				
				RSGISCalcGaussianSmoothFilter *calcGaussianSmoothFilter = new RSGISCalcGaussianSmoothFilter(stddevX, stddevY, angleRadians);
				RSGISGenerateFilter *genFilter = new RSGISGenerateFilter(calcGaussianSmoothFilter);
				ImageFilter *filterKernal = genFilter->generateFilter(size);
				RSGISImageFilter *filter = new RSGISImageKernelFilter(0, size, fileEnding, filterKernal);
				filterBank->addFilter(filter);
				delete calcGaussianSmoothFilter;
				delete genFilter;
			}
			else if(XMLString::equals(filterTypeGuassian1st, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				float stddevX = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("stddevX"))));
				float stddevY = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("stddevY"))));
				float angle = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("angle"))));
				float angleRadians = angle*(M_PI/180);
				
				RSGISCalcGaussianFirstDerivativeFilter *calcGaussian1stDerivFilter = new RSGISCalcGaussianFirstDerivativeFilter(stddevX, stddevY, angleRadians);
				RSGISGenerateFilter *genFilter = new RSGISGenerateFilter(calcGaussian1stDerivFilter);
				ImageFilter *filterKernal = genFilter->generateFilter(size);
				RSGISImageFilter *filter = new RSGISImageKernelFilter(0, size, fileEnding, filterKernal);
				filterBank->addFilter(filter);
				delete calcGaussian1stDerivFilter;
				delete genFilter;
				
			}
			else if(XMLString::equals(filterTypeGuassian2nd, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				float stddevX = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("stddevX"))));
				float stddevY = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("stddevY"))));
				float angle = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("angle"))));
				float angleRadians = angle*(M_PI/180);
				
				RSGISCalcGaussianSecondDerivativeFilter *calcGaussian2ndDerivFilter = new RSGISCalcGaussianSecondDerivativeFilter(stddevX, stddevY, angleRadians);
				RSGISGenerateFilter *genFilter = new RSGISGenerateFilter(calcGaussian2ndDerivFilter);
				ImageFilter *filterKernal = genFilter->generateFilter(size);
				RSGISImageFilter *filter = new RSGISImageKernelFilter(0, size, fileEnding, filterKernal);
				filterBank->addFilter(filter);
				delete calcGaussian2ndDerivFilter;
				delete genFilter;
			}
			else if(XMLString::equals(filterTypeLaplacian, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				float stddev = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("stddev"))));
				
				RSGISCalcLapacianFilter *calcLapacianFilter = new RSGISCalcLapacianFilter(stddev);
				RSGISGenerateFilter *genFilter = new RSGISGenerateFilter(calcLapacianFilter);
				ImageFilter *filterKernal = genFilter->generateFilter(size);
				RSGISImageFilter *filter = new RSGISImageKernelFilter(0, size, fileEnding, filterKernal);
				filterBank->addFilter(filter);
				delete calcLapacianFilter;
				delete genFilter;
			}
			else if(XMLString::equals(filterTypeSobel, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				const XMLCh *sobelFilterOption = filterElement->getAttribute(XMLString::transcode("option"));
                
				RSGISImageFilter *filter = NULL;
				
				if(XMLString::equals(filterTypeOptionX, sobelFilterOption))
				{
					filter = new RSGISSobelFilter(0, 3, fileEnding, RSGISSobelFilter::x);
				}
				else if(XMLString::equals(filterTypeOptionY, sobelFilterOption))
				{
					filter = new RSGISSobelFilter(0, 3, fileEnding, RSGISSobelFilter::y);
				}
				else if(XMLString::equals(filterTypeOptionXY, sobelFilterOption))
				{
					filter = new RSGISSobelFilter(0, 3, fileEnding, RSGISSobelFilter::xy);
				}
				else
				{
					throw RSGISXMLArgumentsException("Sobel type not recognised");
				}
                
				filterBank->addFilter(filter);
			}
			else if(XMLString::equals(filterTypePrewitt, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				const XMLCh *prewittFilterOption = filterElement->getAttribute(XMLString::transcode("option"));
				
				RSGISImageFilter *filter = NULL;
				
				if(XMLString::equals(filterTypeOptionX, prewittFilterOption))
				{
					filter = new RSGISPrewittFilter(0, 3, fileEnding, RSGISPrewittFilter::x);
				}
				else if(XMLString::equals(filterTypeOptionY, prewittFilterOption))
				{
					filter = new RSGISPrewittFilter(0, 3, fileEnding, RSGISPrewittFilter::y);
				}
				else if(XMLString::equals(filterTypeOptionXY, prewittFilterOption))
				{
					filter = new RSGISPrewittFilter(0, 3, fileEnding, RSGISPrewittFilter::xy);
				}
				else
				{
					throw RSGISXMLArgumentsException("Sobel type not recognised");
				}
				
				filterBank->addFilter(filter);
			}
			else if(XMLString::equals(filterTypeMean, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				
				RSGISImageFilter *filter = new RSGISMeanFilter(0, size, fileEnding);
				filterBank->addFilter(filter);
			}
			else if(XMLString::equals(filterTypeMedian, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				
				RSGISImageFilter *filter = new RSGISMedianFilter(0, size, fileEnding);
				filterBank->addFilter(filter);
			}
			else if(XMLString::equals(filterTypeMode, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				
				RSGISImageFilter *filter = new RSGISModeFilter(0, size, fileEnding);
				filterBank->addFilter(filter);
			}
			else if(XMLString::equals(filterTypeRange, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				
				RSGISImageFilter *filter = new RSGISRangeFilter(0, size, fileEnding);
				filterBank->addFilter(filter);
				
			}
			else if(XMLString::equals(filterTypeStdDev, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				
				RSGISImageFilter *filter = new RSGISStdDevFilter(0, size, fileEnding);
				filterBank->addFilter(filter);
				
			}
			else if(XMLString::equals(filterTypeMin, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				
				RSGISImageFilter *filter = new RSGISMinFilter(0, size, fileEnding);
				filterBank->addFilter(filter);
				
			}
			else if(XMLString::equals(filterTypeMax, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				
				RSGISImageFilter *filter = new RSGISMaxFilter(0, size, fileEnding);
				filterBank->addFilter(filter);
				
			}
			else if(XMLString::equals(filterTypeTotal, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				
				RSGISImageFilter *filter = new RSGISTotalFilter(0, size, fileEnding);
				filterBank->addFilter(filter);
				
			}
			else if(XMLString::equals(filterTypeKuwahara, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				
				RSGISImageFilter *filter = new RSGISKuwaharaFilter(0, size, fileEnding);
				filterBank->addFilter(filter);
			}
            else if(XMLString::equals(filterTypeLee, filterType))
			{
				const XMLCh *fileEndingStr = filterElement->getAttribute(XMLString::transcode("fileending"));
				string fileEnding = XMLString::transcode(fileEndingStr);
				
				int size = mathUtils.strtofloat(XMLString::transcode(filterElement->getAttribute(XMLString::transcode("size"))));
				
                unsigned int nLooks = size; // Set number of looks to window size
                
                XMLCh *nLooksXMLStr = XMLString::transcode("nLooks");
                if(argElement->hasAttribute(nLooksXMLStr))
                {
                    char *charValue = XMLString::transcode(argElement->getAttribute(nLooksXMLStr));
                    nLooks = mathUtils.strtoint(string(charValue));
                    XMLString::release(&charValue);
                }
                else
                {
                    nLooks = size;
                }
                XMLString::release(&nLooksXMLStr);
                
				RSGISImageFilter *filter = new RSGISLeeFilter(0, size, fileEnding, nLooks);
				filterBank->addFilter(filter);
			}
			else if(XMLString::equals(filterTypeHarlick, filterType))
			{
				throw RSGISXMLArgumentsException("Harlick features are not implemented");
			}
			else if(XMLString::equals(filterTypeFree, filterType))
			{
				throw RSGISXMLArgumentsException("Free filter not implemented");
			}
			else
			{
				throw RSGISXMLArgumentsException("Filter Type not recognised");
			}
		}
        
        XMLString::release(&filterTypeGuassianSmooth);
        XMLString::release(&filterTypeGuassian1st);
        XMLString::release(&filterTypeGuassian2nd);
        XMLString::release(&filterTypeLaplacian);
        XMLString::release(&filterTypeSobel);
        XMLString::release(&filterTypePrewitt);
        XMLString::release(&filterTypeMean);
        XMLString::release(&filterTypeMedian);
        XMLString::release(&filterTypeMode);
        XMLString::release(&filterTypeRange);
        XMLString::release(&filterTypeStdDev);
        XMLString::release(&filterTypeMin);
        XMLString::release(&filterTypeMax);
        XMLString::release(&filterTypeTotal);
        XMLString::release(&filterTypeKuwahara);
        XMLString::release(&filterTypeHarlick);
        XMLString::release(&filterTypeFree);
        XMLString::release(&filterTypeLee);
        
        XMLString::release(&filterTypeOptionX);
        XMLString::release(&filterTypeOptionY);
        XMLString::release(&filterTypeOptionXY);
    }
    
    const XMLCh *optionXML = argElement->getAttribute(XMLString::transcode("option"));
    if(XMLString::equals(optionFilter, optionXML))
    {		
        this->option = RSGISExeFilterImages::filter;
    }
    else if(XMLString::equals(optionExportFilterBank, optionXML))
    {		
        this->option = RSGISExeFilterImages::exportfilterbank;
    }
    else
    {
        string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeFilterImages.");
        throw RSGISXMLArgumentsException(message.c_str());
    }
    
    parsed = true;
}

void RSGISExeFilterImages::runAlgorithm() throw(RSGISException)
{
    if(!parsed)
    {
        throw RSGISException("Before running the parameters much be retrieved");
    }
    else
    {
        if(option == RSGISExeFilterImages::filter)
        {
            GDALAllRegister();
            GDALDataset **dataset = NULL;
            try
            {
                dataset = new GDALDataset*[1];
                cout << this->inputImage << endl;
                dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
                if(dataset[0] == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                filterBank->executeFilters(dataset, 1, this->outputImageBase);
                
                GDALClose(dataset[0]);
                delete[] dataset;
                delete filterBank;
            }
            catch(RSGISException e)
            {
                throw e;
            }
        }
        else if(option == RSGISExeFilterImages::exportfilterbank)
        {
            cout << "NOT IMPLEMENTED YET! - well interface not written!:)";
        }
        else
        {
            cout << "Options not recognised\n";
        }		
    }
}


void RSGISExeFilterImages::printParameters()
{
    if(parsed)
    {
        if(option == RSGISExeFilterImages::filter)
        {
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image Base: " << this->outputImageBase << endl;
        }
        else if(option == RSGISExeFilterImages::exportfilterbank)
        {
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image Base: " << this->outputImageBase << endl;
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

void RSGISExeFilterImages::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
    cout << "\t<rsgis:command algor=\"imagefilter\" option=\"filter\" image=\"image.env\" output=\"output_image_base\" >\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"GaussianSmooth\" fileending=\"gausmooth\" stddevX=\"float\" stddevY=\"float\" angle=\"int\"/>\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Gaussian1st\" fileending=\"gau1st\" stddevX=\"float\" stddevY=\"float\" angle=\"int\"/>\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Gaussian2nd\" fileending=\"gau2nd\" stddevX=\"float\" stddevY=\"float\" angle=\"int\"/>\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Laplacian\" fileending=\"laplacian\" stddev=\"float\"/>\n";
    cout << "\t\t<rsgis:filter type=\"Sobel\" fileending=\"sobel\" option=\"x | y | xy\"/>\n";
    cout << "\t\t<rsgis:filter type=\"Prewitt\" fileending=\"prewitt\" option=\"x | y | xy\"/>\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Mean\" fileending=\"mean\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Median\" fileending=\"median\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Mode\" fileending=\"mode\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Range\" fileending=\"range\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"StdDev\" fileending=\"stddev\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Min\" fileending=\"min\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Max\" fileending=\"max\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Total\" fileending=\"total\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Kuwahara\" fileending=\"kuwahara\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Lee\" fileending=\"lee\" />\n";
    cout << "\t</rsgis:command>\n";
    cout << "\t<rsgis:command algor=\"imagefilter\" option=\"filter\" image=\"image.env\" output=\"output_image_base\" filterbank=\"LM\"/>\n";
    cout << "\t<rsgis:command algor=\"imagefilter\" option=\"exportfilterbank\" output=\"output_image_base\" >\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"GaussianSmooth\" fileending=\"gausmooth\" stddevX=\"float\" stddevY=\"float\" angle=\"int\"/>\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Gaussian1st\" fileending=\"gau1st\" stddevX=\"float\" stddevY=\"float\" angle=\"int\"/>\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Gaussian2nd\" fileending=\"gau2nd\" stddevX=\"float\" stddevY=\"float\" angle=\"int\"/>\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Laplacian\" fileending=\"laplacian\" stddev=\"float\"/>\n";
    cout << "\t\t<rsgis:filter type=\"Sobel\" fileending=\"sobel\" option=\"x | y | xy\"/>\n";
    cout << "\t\t<rsgis:filter type=\"Prewitt\" fileending=\"prewitt\" option=\"x | y | xy\"/>\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Mean\" fileending=\"mean\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Median\" fileending=\"median\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Mode\" fileending=\"mode\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Range\" fileending=\"range\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"StdDev\" fileending=\"stddev\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Min\" fileending=\"min\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Max\" fileending=\"max\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Total\" fileending=\"total\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Kuwahara\" fileending=\"kuwahara\" />\n";
    cout << "\t\t<rsgis:filter size=\"int\" type=\"Lee\" fileending=\"lee\" />\n";
    cout << "\t</rsgis:command>\n";
    cout << "\t<rsgis:command algor=\"imagefilter\" option=\"exportfilterbank\" output=\"output_image_base\" filterbank=\"LM\"/>\n";
    cout << "</rsgis:commands>\n";
}

string RSGISExeFilterImages::getDescription()
{
    return "Provides access to the image filtering functionality available within the library.";
}

string RSGISExeFilterImages::getXMLSchema()
{
    return "NOT DONE!";
}

RSGISExeFilterImages::~RSGISExeFilterImages()
{
    
}

