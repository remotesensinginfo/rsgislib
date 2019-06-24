/*
 *  RSGISCmdFilterImages.cpp
 *
 *
 *  Created by Dan Clewley on 09/11/2013.
 *  Copyright 2013 RSGISLib.
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


#include "RSGISCmdFilterImages.h"
#include "RSGISCmdParent.h"

#include "filtering/RSGISFilterBank.h"
#include "filtering/RSGISImageFilter.h"
#include "filtering/RSGISCalcImageFilters.h"
#include "filtering/RSGISGenerateFilter.h"
#include "filtering/RSGISImageKernelFilter.h"
#include "filtering/RSGISSobelFilter.h"
#include "filtering/RSGISPrewittFilter.h"
#include "filtering/RSGISStatsFilters.h"
#include "filtering/RSGISSpeckleFilters.h"
#include "filtering/RSGISSARTextureFilters.h"


namespace rsgis{ namespace cmds {

    void executeFilter(std::string inputImage, std::vector<rsgis::cmds::RSGISFilterParameters*> *filterParameters, std::string outputImageBase, std::string imageFormat, std::string imageExt, RSGISLibDataType outDataType) 
    {
        try
        {
            // Set up filter bank
            rsgis::filter::RSGISFilterBank *filterBank = NULL;
            filterBank = new rsgis::filter::RSGISFilterBank();
         
            // Get filter parameters and add to filter bank
            for(std::vector<rsgis::cmds::RSGISFilterParameters*>::iterator iterFilter = filterParameters->begin(); iterFilter != filterParameters->end(); ++iterFilter)
            {
                
                if((*iterFilter)->type == ""){std::cerr << "No type set, skipping filter" << std::endl;}
                                        
                else if((*iterFilter)->type == "GaussianSmooth")
                {
                    rsgis::filter::RSGISCalcGaussianSmoothFilter *calcGaussianSmoothFilter = new rsgis::filter::RSGISCalcGaussianSmoothFilter((*iterFilter)->stddevX, (*iterFilter)->stddevY, (*iterFilter)->angle);
                    rsgis::filter::RSGISGenerateFilter *genFilter = new rsgis::filter::RSGISGenerateFilter(calcGaussianSmoothFilter);
                    rsgis::filter::ImageFilter *filterKernal = genFilter->generateFilter((*iterFilter)->size);
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISImageKernelFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding, filterKernal);
                    filterBank->addFilter(filter);
                    delete calcGaussianSmoothFilter;
                    delete genFilter;
                }
                else if((*iterFilter)->type == "Gaussian1st")
                {
                    rsgis::filter::RSGISCalcGaussianFirstDerivativeFilter *calcGaussian1stDerivFilter = new rsgis::filter::RSGISCalcGaussianFirstDerivativeFilter((*iterFilter)->stddevX, (*iterFilter)->stddevY, (*iterFilter)->angle);
                    rsgis::filter::RSGISGenerateFilter *genFilter = new rsgis::filter::RSGISGenerateFilter(calcGaussian1stDerivFilter);
                    rsgis::filter::ImageFilter *filterKernal = genFilter->generateFilter((*iterFilter)->size);
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISImageKernelFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding, filterKernal);
                    filterBank->addFilter(filter);
                    delete calcGaussian1stDerivFilter;
                    delete genFilter;
                }
                else if((*iterFilter)->type == "Gaussian2nd")
                {
                     rsgis::filter::RSGISCalcGaussianSecondDerivativeFilter *calcGaussian2ndDerivFilter = new rsgis::filter::RSGISCalcGaussianSecondDerivativeFilter((*iterFilter)->stddevX, (*iterFilter)->stddevY, (*iterFilter)->angle);
                    rsgis::filter::RSGISGenerateFilter *genFilter = new rsgis::filter::RSGISGenerateFilter(calcGaussian2ndDerivFilter);
                    rsgis::filter::ImageFilter *filterKernal = genFilter->generateFilter((*iterFilter)->size);
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISImageKernelFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding, filterKernal);
                    filterBank->addFilter(filter);
                    delete calcGaussian2ndDerivFilter;
                    delete genFilter;
                }
                else if((*iterFilter)->type == "Laplacian")
                {
                    rsgis::filter::RSGISCalcLapacianFilter *calcLapacianFilter = new rsgis::filter::RSGISCalcLapacianFilter((*iterFilter)->stddev);
                    rsgis::filter::RSGISGenerateFilter *genFilter = new rsgis::filter::RSGISGenerateFilter(calcLapacianFilter);
                    rsgis::filter::ImageFilter *filterKernal = genFilter->generateFilter((*iterFilter)->size);
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISImageKernelFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding, filterKernal);
                    filterBank->addFilter(filter);
                    delete calcLapacianFilter;
                    delete genFilter;
                }
                else if((*iterFilter)->type == "Sobel")
                {
                    
                    rsgis::filter::RSGISImageFilter *filter = NULL;
                    
                    if((*iterFilter)->option == "x")
                    {
                        filter = new rsgis::filter::RSGISSobelFilter(0, 3, (*iterFilter)->fileEnding, rsgis::filter::RSGISSobelFilter::x);
                    }
                    else if((*iterFilter)->option == "y")
                    {
                        filter = new rsgis::filter::RSGISSobelFilter(0, 3, (*iterFilter)->fileEnding, rsgis::filter::RSGISSobelFilter::y);
                    }
                    else if ((*iterFilter)->option == "xy")
                    {
                        filter = new rsgis::filter::RSGISSobelFilter(0, 3, (*iterFilter)->fileEnding, rsgis::filter::RSGISSobelFilter::xy);
                    }
                    else
                    {
                        std::cerr << "Sobel type not recognised, skipping filter" << std::endl;
                    }
                    
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "Prewitt")
                {
                    
                    rsgis::filter::RSGISImageFilter *filter = NULL;
                    
                    if((*iterFilter)->option == "x")
                    {
                        filter = new rsgis::filter::RSGISPrewittFilter(0, 3, (*iterFilter)->fileEnding, rsgis::filter::RSGISPrewittFilter::x);
                    }
                    else if((*iterFilter)->option == "y")
                    {
                        filter = new rsgis::filter::RSGISPrewittFilter(0, 3, (*iterFilter)->fileEnding, rsgis::filter::RSGISPrewittFilter::y);
                    }
                    else if ((*iterFilter)->option == "xy")
                    {
                        filter = new rsgis::filter::RSGISPrewittFilter(0, 3, (*iterFilter)->fileEnding, rsgis::filter::RSGISPrewittFilter::xy);
                    }
                    else
                    {
                        std::cerr << "Prewitt type not recognised, skipping filter" << std::endl;
                    }
                    
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "Mean")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISMeanFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "Median")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISMedianFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "Mode")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISModeFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "Range")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISRangeFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "StdDev")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISStdDevFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "CoeffOfVar")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISCoeffOfVarFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "Min")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISMinFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "Max")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISMaxFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "Total")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISTotalFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "Kuwahara")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISKuwaharaFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "Lee")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISLeeFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding, (*iterFilter)->nLooks);
                    filterBank->addFilter(filter);
                }
                else if( ((*iterFilter)->type == "NormVar") | ((*iterFilter)->type == "NormVarPower") )
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISNormVarPowerFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if( ((*iterFilter)->type == "NormVarSqrt") | ((*iterFilter)->type == "NormVarAmplitude") )
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISNormVarAmplitudeFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if( ((*iterFilter)->type == "NormVarLn") | ((*iterFilter)->type == "NormVarLnPower") )
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISNormVarLnPowerFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "NormLn")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISNormLnFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "TextureVar")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISTextureVar(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "MeanDiff")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISMeanDiffFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "MeanDiffAbs")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISMeanDiffAbsFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "TotalDiff")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISTotalDiffFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else if((*iterFilter)->type == "TotalDiffAbs")
                {
                    rsgis::filter::RSGISImageFilter *filter = new rsgis::filter::RSGISTotalDiffAbsFilter(0, (*iterFilter)->size, (*iterFilter)->fileEnding);
                    filterBank->addFilter(filter);
                }
                else{std::cerr << "Filter not recognised - skipping" << std::endl;}

            }
            
            GDALAllRegister();
            GDALDataset **dataset = NULL;

            dataset = new GDALDataset*[1];
            std::cout << inputImage << std::endl;
            dataset[0] = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            filterBank->executeFilters(dataset, 1, outputImageBase, imageFormat, imageExt, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset[0]);
            delete[] dataset;
            delete filterBank;
            
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
                    
                    
    std::vector<rsgis::cmds::RSGISFilterParameters*> *createLeungMalikFilterBank() 
    {
        try
        {
            float stddev = 0;
            float stddevX = 0;
            float stddevY = 0;
            float angle = 0;
            int size = 51;
            std::string fileEnding = "";
            std::string angleStr = "";

            std::vector<rsgis::cmds::RSGISFilterParameters*> *filterParameters = new std::vector<rsgis::cmds::RSGISFilterParameters*>();
            filterParameters->reserve(48);
            
            rsgis::cmds::RSGISFilterParameters *filterPar = NULL;
            

            // GENERATE LAPACIAN FILTERS
            for(int i = 0; i < 8; i++)
            {
                if(i == 0)
                {
                    stddev = 1;
                    fileEnding = "lap_1";
                }
                else if(i == 1)
                {
                    stddev = sqrt(2.0);
                    fileEnding = "lap_sqrt2";
                }
                else if(i == 2)
                {
                    stddev = 2;
                    fileEnding = "lap_2";
                }
                else if(i == 3)
                {
                    stddev = 2*sqrt(2.0);
                    fileEnding = "lap_2sqrt2";
                }
                else if(i == 4)
                {
                    stddev = 3;
                    fileEnding = "lap_3";
                }
                else if(i == 5)
                {
                    stddev = 3*sqrt(2.0);
                    fileEnding = "lap_3sqrt2";
                }
                else if(i == 6)
                {
                    stddev = 6;
                    fileEnding = "lap_6";
                }
                else if(i == 7)
                {
                    stddev = 6*sqrt(2.0);
                    fileEnding = "lap_6sqrt2";
                }
                
                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "Laplacian";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                filterPar->stddev = stddev;
                
                filterParameters->push_back(filterPar);
            }
            
            
            // Gaussian Smoothing Filters
            angle = 0;
            for(int i = 0; i < 4; i++)
            {
                if(i == 0)
                {
                    stddevX = 1;
                    stddevY = 1;
                    fileEnding = "gausmooth1";
                }
                else if(i == 1)
                {
                    stddevX = sqrt(2.0);
                    stddevY = sqrt(2.0);
                    fileEnding = "gausmoothsqrt2";
                }
                else if(i == 2)
                {
                    stddevX = 2;
                    stddevY = 2;
                    fileEnding = "gausmooth2";
                }
                else if(i == 3)
                {
                    stddevX = 2*sqrt(2.0);
                    stddevY = 2*sqrt(2.0);
                    fileEnding = "gausmooth2sqrt2";
                }
                
                filterPar = new rsgis::cmds::RSGISFilterParameters();
                
                filterPar->type = "GaussianSmooth";
                filterPar->fileEnding = fileEnding;
                filterPar->size = size;
                filterPar->stddevX = stddevX;
                filterPar->stddevY = stddevY;
                filterPar->angle = angle;
                
                filterParameters->push_back(filterPar);
            }
            
            //Gaussain First Filters
            for(int i = 0; i < 6; i++)
            {
                if(i == 0)
                {
                    angle = 0;
                    angleStr = "0";
                }
                else if(i == 1)
                {
                    angle = 30 * (M_PI/180);
                    angleStr = "30";
                }
                else if(i == 2)
                {
                    angle = 60 * (M_PI/180);
                    angleStr = "60";
                }
                else if(i == 3)
                {
                    angle = 90 * (M_PI/180);
                    angleStr = "90";
                }
                else if(i == 4)
                {
                    angle = 120 * (M_PI/180);
                    angleStr = "120";
                }
                else if(i == 5)
                {
                    angle = 150 * (M_PI/180);
                    angleStr = "150";
                }
                
                for(int j = 0; j < 3; j++)
                {
                    if(j == 0)
                    {
                        stddevX = 3;
                        stddevY = 1;
                        fileEnding = "gau1st_3_1_" + angleStr;
                    }
                    else if(j == 1)
                    {
                        stddevX = 3*sqrt(2.0);
                        stddevY = sqrt(2.0);
                        fileEnding = "gau1st_3sqrt1_sqrt2_" + angleStr;
                    }
                    else if(j == 2)
                    {
                        stddevX = 6;
                        stddevY = 2;
                        fileEnding = "gau1st_6_2_" + angleStr;
                    }
                    
                    filterPar = new rsgis::cmds::RSGISFilterParameters();
                    
                    filterPar->type = "Gaussian1st";
                    filterPar->fileEnding = fileEnding;
                    filterPar->size = size;
                    filterPar->stddevX = stddevX;
                    filterPar->stddevY = stddevY;
                    filterPar->angle = angle;
                    
                    filterParameters->push_back(filterPar);
			
                }
            }
            
            //Gaussain Second Filters
            for(int i = 0; i < 6; i++)
            {
                if(i == 0)
                {
                    angle = 0;
                    angleStr = "0";
                }
                else if(i == 1)
                {
                    angle = 30 * (M_PI/180);
                    angleStr = "30";
                }
                else if(i == 2)
                {
                    angle = 60 * (M_PI/180);
                    angleStr = "60";
                }
                else if(i == 3)
                {
                    angle = 90 * (M_PI/180);
                    angleStr = "90";
                }
                else if(i == 4)
                {
                    angle = 120 * (M_PI/180);
                    angleStr = "120";
                }
                else if(i == 5)
                {
                    angle = 150 * (M_PI/180);
                    angleStr = "150";
                }
                
                
                for(int j = 0; j < 3; j++)
                {
                    if(j == 0)
                    {
                        stddevX = 3;
                        stddevY = 1;
                        fileEnding = "gau2nd_3_1_" + angleStr;
                    }
                    else if(j == 1)
                    {
                        stddevX = 3*sqrt(2.0);
                        stddevY = sqrt(2.0);
                        fileEnding = "gau2nd_3sqrt1_sqrt2_" + angleStr;
                    }
                    else if(j == 2)
                    {
                        stddevX = 6;
                        stddevY = 2;
                        fileEnding = "gau2nd_6_2_" + angleStr;
                    }
                    
                    filterPar = new rsgis::cmds::RSGISFilterParameters();
                    
                    filterPar->type = "Gaussian2nd";
                    filterPar->fileEnding = fileEnding;
                    filterPar->size = size;
                    filterPar->stddevX = stddevX;
                    filterPar->stddevY = stddevY;
                    filterPar->angle = angle;
                    
                    filterParameters->push_back(filterPar);
		
                }
            }
            
            return filterParameters;
            
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
}}
