/*
 *  RSGISRATFunctionFitting.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/03/2015.
 *  Copyright 2015 RSGISLib.
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

#include "RSGISRATFunctionFitting.h"

namespace rsgis{namespace rastergis{
    
 
    void RSGISFitHistGausianMixtureModel::performFit(GDALDataset *clumpsDataset, std::string outH5File, std::string varCol, float binWidth, std::string classColumn, std::string classVal, bool outputHist, std::string outHistFile, unsigned int ratBand)
    {
        try
        {
            std::cout << "Import attribute tables to memory.\n";
            GDALRasterAttributeTable *gdalRAT = clumpsDataset->GetRasterBand(1)->GetDefaultRAT();
            
            RSGISRasterAttUtils ratUtils;
            rsgis::math::RSGISMathsUtils mathUtils;
            
            unsigned int varColIdx = ratUtils.findColumnIndex(gdalRAT, varCol);
            unsigned int classNamesColIdx = ratUtils.findColumnIndex(gdalRAT, classColumn);
            
            std::cout << "Reading in the data\n";
            double minVal = 0.0;
            double maxVal = 0.0;
            std::vector<double> *data = new std::vector<double>();
            RSGISGetClassData getClassData = RSGISGetClassData(classVal, data, &minVal, &maxVal);
            RSGISRATCalc ratCalcGetData = RSGISRATCalc(&getClassData);
            std::vector<unsigned int> inRealColIdx;
            inRealColIdx.push_back(varColIdx);
            std::vector<unsigned int> inIntColIdx;
            std::vector<unsigned int> inStrColIdx;
            inStrColIdx.push_back(classNamesColIdx);
            std::vector<unsigned int> outRealColIdx;
            std::vector<unsigned int> outIntColIdx;
            std::vector<unsigned int> outStrColIdx;
            ratCalcGetData.calcRATValues(gdalRAT, inRealColIdx, inIntColIdx, inStrColIdx, outRealColIdx, outIntColIdx, outStrColIdx);
            
            std::cout << "DATA [" << minVal << ", " << maxVal << "]: " << (maxVal-minVal) << "\t Num Vals = " << data->size() << "\n";
            
            std::vector<std::pair<double, double> > *hist = mathUtils.calcHistogram(data, minVal, maxVal, binWidth, true);
            
            /*
            for(std::vector<std::pair<double, double> >::iterator iterHist = hist->begin(); iterHist != hist->end(); ++iterHist)
            {
                std::cout << (*iterHist).first << ":\t" << (*iterHist).second << std::endl;
            }
            */
            
            rsgis::math::RSGISFitGaussianMixModel fitGausModel;
            double ampVar = 0.01;
            double peakThres = 0.005;
            unsigned int peakLocVar = 2;
            unsigned int initWidth = 2;
            double minWidth = 0.01;
            double maxWidth = 10.0;
            bool debug_info = false;
            std::vector<rsgis::math::GaussianModelParams> outGauParams = fitGausModel.performFit(hist, binWidth, peakThres, ampVar, peakLocVar, initWidth, minWidth, maxWidth, debug_info);
            
            rsgis::utils::RSGISExportColumnData2HDF exportGauParams2HDF;
            exportGauParams2HDF.createFile(outH5File, 4, "Output Parameters for Gaussian Models", H5::PredType::IEEE_F64LE);
            double *gausParams = new double[4];
            
            std::cout << "Number of Gaussians: " << outGauParams.size() << std::endl;
            for(std::vector<rsgis::math::GaussianModelParams>::iterator iterGauParams = outGauParams.begin(); iterGauParams != outGauParams.end(); ++iterGauParams)
            {
                std::cout << "\tOFF: " << (*iterGauParams).offset << "\tAMP: " << (*iterGauParams).amplitude << "\tFWHM: " << (*iterGauParams).fwhm << "\tNOISE: " << (*iterGauParams).noise << std::endl;
                gausParams[0] = (*iterGauParams).offset;
                gausParams[1] = (*iterGauParams).amplitude;
                gausParams[2] = (*iterGauParams).fwhm;
                gausParams[3] = (*iterGauParams).noise;
                exportGauParams2HDF.addDataRow(gausParams, H5::PredType::NATIVE_DOUBLE);
            }
            delete[] gausParams;
            exportGauParams2HDF.close();
            
            if(outputHist)
            {
                rsgis::utils::RSGISExportColumnData2HDF exportHist2HDF;
                exportHist2HDF.createFile(outHistFile, 2, "Histogram", H5::PredType::IEEE_F64LE);
                double *histRow = new double[2];
                for(std::vector<std::pair<double, double> >::iterator iterHist = hist->begin(); iterHist != hist->end(); ++iterHist)
                {
                    histRow[0] = (*iterHist).first;
                    histRow[1] = (*iterHist).second;
                    exportHist2HDF.addDataRow(histRow, H5::PredType::NATIVE_DOUBLE);
                }
                delete[] histRow;
                exportHist2HDF.close();
            }
            
            std::cout << "Exported to output file. Finished.\n";
            
            delete data;
            delete hist;
        }
        catch(rsgis::RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(std::exception &e)
        {
            throw e;
        }
    }
    
    RSGISGetClassData::RSGISGetClassData(std::string className, std::vector<double> *data, double *minVal, double *maxVal):RSGISRATCalcValue()
    {
        this->className = className;
        this->data = data;
        this->minVal = minVal;
        this->maxVal = maxVal;
        this->firstVal = true;
    }
    
    void RSGISGetClassData::calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException)
    {
        if(fid > 0)
        {
            if(numInRealCols == 0)
            {
                throw rsgis::RSGISAttributeTableException("RSGISGetClassData::calcRATValue must have at least 1 double column specified.");
            }
            
            if(numInStringCols != 1)
            {
                throw rsgis::RSGISAttributeTableException("RSGISGetClassData::calcRATValue must have 1 string column specified if class names are to be used.");
            }
            
            if(inStringCols[0] == className)
            {
                if(firstVal)
                {
                    *minVal = inRealCols[0];
                    *maxVal = inRealCols[0];
                    firstVal = false;
                }
                else
                {
                    if(inRealCols[0] < (*minVal))
                    {
                        *minVal = inRealCols[0];
                    }
                    
                    if(inRealCols[0] > (*maxVal))
                    {
                        *maxVal = inRealCols[0];
                    }
                }
                data->push_back(inRealCols[0]);
            }
        }
    }
    
    RSGISGetClassData::~RSGISGetClassData()
    {
        
    }

    
}}



