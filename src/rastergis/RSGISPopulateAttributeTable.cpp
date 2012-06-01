/*
 *  RSGISPopulateAttributeTable.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 15/02/2012.
 *  Copyright 2012 RSGISLib.
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

#include "RSGISPopulateAttributeTable.h"

namespace rsgis{namespace rastergis{
    

    RSGISPopulateAttributeTableBandMeans::RSGISPopulateAttributeTableBandMeans()
    {
        
    }
    
    RSGISAttributeTable* RSGISPopulateAttributeTableBandMeans::populateWithBandsMeans(GDALDataset **datasets, int numDatasets, string attrPrefix)throw(RSGISImageCalcException, RSGISAttributeTableException)
    {
        if(numDatasets < 2)
        {
            throw RSGISImageCalcException("At least two datasets are required for this fucntion.");
        }
        RSGISMathsUtils mathUtils;
        
        RSGISAttributeTable *attTable = NULL;
        
        try 
        {
            // Calc max clump value = number of clumps.
            unsigned long long numClumps = this->calcMaxValue(datasets[0]);
            cout << "There are " << numClumps << " in the input dataset\n";
            
            unsigned int numRasterBands = 0;
            for(int i = 1; i < numDatasets; ++i)
            {
                numRasterBands += datasets[i]->GetRasterCount();
            }
            
            // Generate Attribute table
            cout << "Creating blank attribute table\n";
            vector<pair<string, RSGISAttributeDataType> > *fields = new vector<pair<string, RSGISAttributeDataType> >();
            fields->push_back(pair<string, RSGISAttributeDataType>(attrPrefix + string("_pxlcount"), rsgis_int));
            
            string fieldName = "";
            for(unsigned int i = 0; i < numRasterBands; ++i)
            {
                fieldName = attrPrefix + string("_b") + mathUtils.uinttostring(i+1);
                fields->push_back(pair<string, RSGISAttributeDataType>(fieldName, rsgis_float));
            }
            attTable = new RSGISAttributeTableMem(numClumps, fields);
            
            unsigned int pxlCountIdx;
            unsigned int *bandMeanIdxs = new unsigned int[numRasterBands];
            
            pxlCountIdx = attTable->getFieldIndex(attrPrefix + string("_pxlcount"));
            for(unsigned int i = 0; i < numRasterBands; ++i)
            {
                fieldName = attrPrefix + string("_b") + mathUtils.uinttostring(i+1);
                bandMeanIdxs[i] = attTable->getFieldIndex(fieldName);
            }
            
            // Populate the attribute table.
            cout << "Populating the attribute table with sum and count values\n";
            RSGISPopulateAttributeTableBandMeansCalcImg *popTabMeans = new RSGISPopulateAttributeTableBandMeansCalcImg(attTable, attrPrefix, pxlCountIdx, bandMeanIdxs, numRasterBands);
            RSGISCalcImage calcImage(popTabMeans);
            calcImage.calcImage(datasets, numDatasets);
            delete popTabMeans;
            
            // Calculate mean values.
            cout << "Calc mean values\n";
            long pxlCount = 0;
            double sumVal = 0;
            string pxlCountName = attrPrefix + string("_pxlcount");
            for(unsigned long long i = 0; i < numClumps; ++i)
            {
                RSGISFeature *feat = attTable->getFeature(i);
                pxlCount = feat->intFields->at(pxlCountIdx);
                //cout << i << " has " << pxlCount << " pixel count\n";
                if(pxlCount != 0)
                {
                    for(unsigned int j = 0; j < numRasterBands; ++j)
                    {
                        sumVal = feat->floatFields->at(bandMeanIdxs[j]);
                        feat->floatFields->at(bandMeanIdxs[j]) = sumVal/pxlCount;
                        //cout << "\tBand: " << j+1 << " has mean = " << feat->floatFields->at(bandMeanIdxs[j]) << endl;
                    }
                }
            }
            
            delete[] bandMeanIdxs;
        } 
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
        catch(RSGISImageBandException &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        return attTable;
    }
    
    unsigned long long RSGISPopulateAttributeTableBandMeans::calcMaxValue(GDALDataset *dataset)throw(RSGISImageCalcException)
    {
        unsigned int width = dataset->GetRasterXSize();
        unsigned int height = dataset->GetRasterYSize();
        GDALRasterBand *imgBand = dataset->GetRasterBand(1);
        
        unsigned int *clumpIdxs = new unsigned int[width];
        unsigned long long maxClumpIdx = 0;
        
        for(unsigned int i = 0; i < height; ++i)
        {
            imgBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int j = 0; j < width; ++j)
            {
                if((i == 0) & (j == 0))
                {
                    maxClumpIdx = clumpIdxs[j];
                }
                else if(clumpIdxs[j] > maxClumpIdx)
                {
                    maxClumpIdx = clumpIdxs[j];
                }
            }
        }
        
        delete clumpIdxs;
        
        return maxClumpIdx;
    }
    
    
    RSGISPopulateAttributeTableBandMeans::~RSGISPopulateAttributeTableBandMeans()
    {
        
    }
    

    RSGISPopulateAttributeTableBandMeansCalcImg::RSGISPopulateAttributeTableBandMeansCalcImg(RSGISAttributeTable *attTable, string attrPrefix, unsigned int pxlCountIdx, unsigned int *bandMeanIdxs, unsigned int numBandMeanIdxs):RSGISCalcImageValue(0)
    {
        this->attTable = attTable;
        this->attrPrefix = attrPrefix;
        this->pxlCountIdx = pxlCountIdx;
        this->bandMeanIdxs = bandMeanIdxs;
        this->numBandMeanIdxs = numBandMeanIdxs;
    }
    
    void RSGISPopulateAttributeTableBandMeansCalcImg::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        if(((long)numBandMeanIdxs) != (numBands-1))
        {
            throw RSGISImageCalcException("The number of indexes provided and input bands is different.");
        }
        
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            
            try
            {
                RSGISFeature *feat = attTable->getFeature(clumpIdx);
                ++feat->intFields->at(this->pxlCountIdx);
                
                bool nanPresent = false;
                for(int i = 1; i < numBands; ++i)
                {
                    if(boost::math::isnan(bandValues[i]))
                    {
                        nanPresent = true;
                    }
                }
                
                if(!nanPresent)
                {
                    for(int i = 1; i < numBands; ++i)
                    {
                        feat->floatFields->at(this->bandMeanIdxs[i-1]) += bandValues[i];
                    }
                }
                
            }
            catch(RSGISAttributeTableException &e)
            {
                cout << "clumpIdx = " << clumpIdx << endl;
                throw RSGISImageCalcException(e.what());
            }
        }        
    }
    
    RSGISPopulateAttributeTableBandMeansCalcImg::~RSGISPopulateAttributeTableBandMeansCalcImg()
    {
        
    }
    
    

    RSGISPopulateAttributeTableBandWithSumAndMeans::RSGISPopulateAttributeTableBandWithSumAndMeans()
    {
        
    }
    
    void RSGISPopulateAttributeTableBandWithSumAndMeans::populateWithBandStatistics(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats) throw(RSGISImageCalcException, RSGISAttributeTableException)
    {
        if(numDatasets != 2)
        {
            throw RSGISImageCalcException("Two datasets are required for this fucntion.");
        }
        
        try 
        {
            if(datasets[0]->GetRasterCount() != 1)
            {
                throw RSGISImageCalcException("Clumps image should only have 1 image band.");
            }
            
            if(datasets[1]->GetRasterCount() < ((int)bandStats->size()))
            {
                throw RSGISImageCalcException("More band stats were requested than bands in the file.");
            }
                        
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if((*iterBands)->band == 0)
                {
                    throw RSGISImageCalcException("Band numbers start at 1.");
                }
                else if(((int)(*iterBands)->band) > datasets[1]->GetRasterCount())
                {
                    throw RSGISImageCalcException("Band specified is not within the image.");
                }
                
                if((*iterBands)->calcMean)
                {
                    if(attTable->hasAttribute((*iterBands)->meanField))
                    {
                        if(attTable->getDataType((*iterBands)->meanField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->meanField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->meanField, 0);
                    }
                    (*iterBands)->meanIdx = attTable->getFieldIndex((*iterBands)->meanField);
                }
                
                if((*iterBands)->calcSum)
                {
                    if(attTable->hasAttribute((*iterBands)->sumField))
                    {
                        if(attTable->getDataType((*iterBands)->sumField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->sumField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->sumField, 0);
                    }
                    (*iterBands)->sumIdx = attTable->getFieldIndex((*iterBands)->sumField);
                }              
            }
            
            if(!attTable->hasAttribute("pxlcount"))
            {
                attTable->addAttIntField("pxlcount", 0);
            }
            else if(attTable->getDataType("pxlcount") != rsgis_int)
            {
                throw RSGISImageCalcException("Cannot proceed as \'pxlcount\' field is not of type integer.");
            }
            size_t pxlCountIdx = attTable->getFieldIndex("pxlcount");
            
            //cout << "pxlCountIdx = " << pxlCountIdx << endl;
            
            size_t fieldCount = bandStats->size();
            //cout << "field count = " << fieldCount << endl;
            size_t *bandIdxs = new size_t[fieldCount];
            size_t idx = 0;
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                bandIdxs[idx] = (*iterBands)->band;
                //cout << "bandIdxs[" << idx << "]: " << bandIdxs[idx] << endl;
                ++idx;
            }
            
            size_t *pxlCount = new size_t[attTable->getSize()];
            double **sumVals = new double*[attTable->getSize()];
            for(size_t i = 0; i < attTable->getSize(); ++i)
            {
                pxlCount[i] = 0;
                sumVals[i] = new double[fieldCount];
                for(size_t j = 0; j < fieldCount; ++j)
                {
                    sumVals[i][j] = 0;
                }
            }
            
            RSGISCalcClumpSumAndCount *clumpSumAndCount = new RSGISCalcClumpSumAndCount(pxlCount, sumVals, bandIdxs, fieldCount, attTable->getSize());
            RSGISCalcImage calcImage(clumpSumAndCount);
            calcImage.calcImage(datasets, numDatasets);
            delete clumpSumAndCount;
            
            idx = 0;
            size_t j = 0;
            int feedback = attTable->getSize()/10;
			int feedbackCounter = 0;
            cout << "Writing to attribute table\n";
			cout << "Started" << flush;
            for(attTable->start(); attTable->end(); ++(*attTable))
            {
                //cout << "Processing: " << (*(*attTable))->fid << endl;
                if((idx % feedback) == 0)
				{
					cout << "." << feedbackCounter << "." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
                (*(*attTable))->intFields->at(pxlCountIdx) = pxlCount[idx];
                j = 0;
                for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {
                    if((*iterBands)->calcMean)
                    {
                        (*(*attTable))->floatFields->at((*iterBands)->meanIdx) = sumVals[idx][j] / pxlCount[idx];
                    }
                    if((*iterBands)->calcSum)
                    {
                        (*(*attTable))->floatFields->at((*iterBands)->sumIdx) = sumVals[idx][j];
                    }
                    ++j;
                }
                delete[] sumVals[idx];
                ++idx;
            }
            cout << " Complete.\n";
            
            delete[] sumVals;
            delete[] pxlCount;
            delete[] bandIdxs;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
    }
    
    RSGISPopulateAttributeTableBandWithSumAndMeans::~RSGISPopulateAttributeTableBandWithSumAndMeans()
    {
        
    }

    
    
    RSGISPopulateAttributeTableBandStats::RSGISPopulateAttributeTableBandStats()
    {
        
    }
        
    void RSGISPopulateAttributeTableBandStats::populateWithBandStatisticsInMem(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats) throw(RSGISImageCalcException, RSGISAttributeTableException)
    {
        if(numDatasets != 2)
        {
            throw RSGISImageCalcException("Two datasets are required for this fucntion.");
        }
        
        try 
        {
            if(datasets[0]->GetRasterCount() != 1)
            {
                throw RSGISImageCalcException("Clumps image should only have 1 image band.");
            }
            
            if(datasets[1]->GetRasterCount() < ((int)bandStats->size()))
            {
                throw RSGISImageCalcException("More band stats were requested than bands in the file.");
            }
            
            unsigned int numDataBands = bandStats->size();
            unsigned int *dataBandIdxs = new unsigned int[numDataBands];
            
            bool usePxlCount = true;
            if(!attTable->hasAttribute("pxlcount"))
            {
                usePxlCount = false;
            }
            else if(attTable->getDataType("pxlcount") != rsgis_int)
            {
                usePxlCount = false;
            }
            unsigned int pxlCountIdx = attTable->getFieldIndex("pxlcount");
            
            if(!usePxlCount)
            {
                cerr << "Warning: \'pxlcount\' field is not available\n";
            }
            
            unsigned int bandCount = 0;
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if((*iterBands)->band == 0)
                {
                    throw RSGISImageCalcException("Band numbers start at 1.");
                }
                else if(((int)(*iterBands)->band) > datasets[1]->GetRasterCount())
                {
                    throw RSGISImageCalcException("Band specified is not within the image.");
                }
                
                dataBandIdxs[bandCount++] = (*iterBands)->band;                
                
                if((*iterBands)->calcMin)
                {
                    if(attTable->hasAttribute((*iterBands)->minField))
                    {
                        if(attTable->getDataType((*iterBands)->minField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->minField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->minField, 0);
                    }
                    (*iterBands)->minIdx = attTable->getFieldIndex((*iterBands)->minField);
                }
                
                if((*iterBands)->calcMax)
                {
                    if(attTable->hasAttribute((*iterBands)->maxField))
                    {
                        if(attTable->getDataType((*iterBands)->maxField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->maxField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->maxField, 0);
                    }
                    (*iterBands)->maxIdx = attTable->getFieldIndex((*iterBands)->maxField);
                }
                
                if((*iterBands)->calcMean)
                {
                    if(attTable->hasAttribute((*iterBands)->meanField))
                    {
                        if(attTable->getDataType((*iterBands)->meanField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->meanField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->meanField, 0);
                    }
                    (*iterBands)->meanIdx = attTable->getFieldIndex((*iterBands)->meanField);
                }
                
                if((*iterBands)->calcSum)
                {
                    if(attTable->hasAttribute((*iterBands)->sumField))
                    {
                        if(attTable->getDataType((*iterBands)->sumField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->sumField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->sumField, 0);
                    }
                    (*iterBands)->sumIdx = attTable->getFieldIndex((*iterBands)->sumField);
                }
                
                if((*iterBands)->calcStdDev)
                {
                    if(attTable->hasAttribute((*iterBands)->stdDevField))
                    {
                        if(attTable->getDataType((*iterBands)->stdDevField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->stdDevField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->stdDevField, 0);
                    }
                    (*iterBands)->stdDevIdx = attTable->getFieldIndex((*iterBands)->stdDevField);
                }
                
                if((*iterBands)->calcMedian)
                {
                    if(attTable->hasAttribute((*iterBands)->medianField))
                    {
                        if(attTable->getDataType((*iterBands)->medianField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->medianField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->medianField, 0);
                    }
                    (*iterBands)->medianIdx = attTable->getFieldIndex((*iterBands)->medianField);
                }                
            }
                        
            RSGISFeature *feat;
            vector<vector<double> > **clumpData = new vector<vector<double> >*[numDataBands];
            for(unsigned int n = 0; n < numDataBands; ++n)
            {
                clumpData[n] = new vector<vector<double> >();
                clumpData[n]->reserve(attTable->getSize());
            }
                
            for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
            {
                feat = attTable->getFeature(i);
                for(unsigned int n = 0; n < numDataBands; ++n)
                {
                    clumpData[n]->push_back(vector<double>());
                    if(usePxlCount)
                    {
                        clumpData[n]->at(i).reserve(feat->intFields->at(pxlCountIdx));
                    }
                }
            }
            
            // Extract Data from Image.
            RSGISGetPixelValuesForClumps *getImageVals = new RSGISGetPixelValuesForClumps(clumpData, numDataBands, dataBandIdxs);
            RSGISCalcImage calcImage(getImageVals);
            calcImage.calcImage(datasets, numDatasets);
            delete getImageVals;

            double min = 0;
            double max = 0;
            double mean = 0;
            double stddev = 0;
            double median = 0;
            double sum = 0;
            
            int feedback = attTable->getSize()/10;
			int feedbackCounter = 0;
			cout << "Started" << flush;
            // Calculate Statistics for each feature.
            for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
            {
                if((i % feedback) == 0)
				{
					cout << ".." << feedbackCounter << ".." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
                
                feat = attTable->getFeature(i);
                
                bandCount = 0;
                for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {   
                    if((*iterBands)->calcMin & (*iterBands)->calcMax)
                    {
                        gsl_stats_minmax (&min, &max, &(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->minIdx) = min;
                        feat->floatFields->at((*iterBands)->maxIdx) = max;
                    }
                    else if((*iterBands)->calcMin)
                    {
                        min = gsl_stats_min(&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->minIdx) = min;
                    }
                    else if((*iterBands)->calcMax)
                    {
                        max = gsl_stats_max(&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->maxIdx) = max;
                    }
                    
                    if((*iterBands)->calcMean & (*iterBands)->calcStdDev)
                    {
                        mean = gsl_stats_mean (&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        stddev = gsl_stats_sd_m (&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size(), mean);
                        feat->floatFields->at((*iterBands)->meanIdx) = mean;
                        feat->floatFields->at((*iterBands)->stdDevIdx) = stddev;
                    }
                    else if((*iterBands)->calcMean & !(*iterBands)->calcStdDev)
                    {
                        mean = gsl_stats_mean (&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->meanIdx) = mean;
                        //cout << feat->fid << " mean value = " << feat->floatFields->at((*iterBands)->meanIdx) << "\tPixel Count: " << clumpData[bandCount]->at(i).size()  << endl;
                    }
                    else if(!(*iterBands)->calcMean & (*iterBands)->calcStdDev)
                    {
                        stddev = gsl_stats_sd (&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->stdDevIdx) = stddev;
                    }
                    
                    if((*iterBands)->calcSum)
                    {
                        sum = 0;
                        for(vector<double>::iterator iterVals = clumpData[bandCount]->at(i).begin(); iterVals != clumpData[bandCount]->at(i).end(); ++iterVals)
                        {
                            sum += *iterVals;
                        }
                        feat->floatFields->at((*iterBands)->sumIdx) = sum;
                    }
                                        
                    if((*iterBands)->calcMedian)
                    {                        
                        sort(clumpData[bandCount]->at(i).begin(), clumpData[bandCount]->at(i).end());
                        median = gsl_stats_median_from_sorted_data(&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->medianIdx) = median;
                    }
                    
                    ++bandCount;
                }
                
            }
            cout << " Complete.\n";
            
            delete[] dataBandIdxs;
            for(unsigned int n = 0; n < numDataBands; ++n)
            {
                delete clumpData[n];
            }
            delete[] clumpData;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }

    }
    
    void RSGISPopulateAttributeTableBandStats::populateWithBandStatisticsWithinAtt(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats) throw(RSGISImageCalcException, RSGISAttributeTableException)
    {
        if(numDatasets != 2)
        {
            throw RSGISImageCalcException("Two datasets are required for this fucntion.");
        }
        
        try 
        {
            if(datasets[0]->GetRasterCount() != 1)
            {
                throw RSGISImageCalcException("Clumps image should only have 1 image band.");
            }
            
            if(datasets[1]->GetRasterCount() < ((int)bandStats->size()))
            {
                throw RSGISImageCalcException("More band stats were requested than bands in the file.");
            }
                        
            if(!attTable->hasAttribute("pxlcount"))
            {
                attTable->addAttIntField("pxlcount", 0);
            }
            else if(attTable->getDataType("pxlcount") != rsgis_int)
            {
                throw RSGISImageCalcException("Cannot proceed as \'pxlcount\' field is not of type integer.");
            }
            unsigned int pxlCountIdx = attTable->getFieldIndex("pxlcount");
                        
            if(!attTable->hasAttribute("first"))
            {
                attTable->addAttBoolField("first", true);
            }
            else if(attTable->getDataType("first") != rsgis_bool)
            {
                throw RSGISImageCalcException("Cannot proceed as \'first\' field is not of type boolean.");
            }
            unsigned int firstFieldIdx = attTable->getFieldIndex("first");
                        
            bool meanCalc = false;
            bool stdDevCalc = false;
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if((*iterBands)->band == 0)
                {
                    throw RSGISImageCalcException("Band numbers start at 1.");
                }
                else if(((int)(*iterBands)->band) > datasets[1]->GetRasterCount())
                {
                    throw RSGISImageCalcException("Band specified is not within the image.");
                }
                
                if((*iterBands)->calcMin)
                {
                    if(attTable->hasAttribute((*iterBands)->minField))
                    {
                        if(attTable->getDataType((*iterBands)->minField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->minField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->minField, 0);
                    }
                    (*iterBands)->minIdx = attTable->getFieldIndex((*iterBands)->minField);
                }
                
                if((*iterBands)->calcMax)
                {
                    if(attTable->hasAttribute((*iterBands)->maxField))
                    {
                        if(attTable->getDataType((*iterBands)->maxField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->maxField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->maxField, 0);
                    }
                    (*iterBands)->maxIdx = attTable->getFieldIndex((*iterBands)->maxField);
                }
                
                if((*iterBands)->calcMean)
                {
                    meanCalc = true;
                    if(attTable->hasAttribute((*iterBands)->meanField))
                    {
                        if(attTable->getDataType((*iterBands)->meanField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->meanField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->meanField, 0);
                    }
                    (*iterBands)->meanIdx = attTable->getFieldIndex((*iterBands)->meanField);
                }
                
                if((*iterBands)->calcSum)
                {
                    if(attTable->hasAttribute((*iterBands)->sumField))
                    {
                        if(attTable->getDataType((*iterBands)->sumField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->sumField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->sumField, 0);
                    }
                    (*iterBands)->sumIdx = attTable->getFieldIndex((*iterBands)->sumField);
                }
                
                if((*iterBands)->calcStdDev)
                {
                    stdDevCalc = true;
                    if(attTable->hasAttribute((*iterBands)->stdDevField))
                    {
                        if(attTable->getDataType((*iterBands)->stdDevField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->stdDevField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->stdDevField, 0);
                    }
                    (*iterBands)->stdDevIdx = attTable->getFieldIndex((*iterBands)->stdDevField);
                }
                
                if((*iterBands)->calcMedian)
                {
                    cout << "WARNING: Cannot calculate median value (" << (*iterBands)->medianField << ") when processing within attribute table\n";
                }                
            }
            
            // Calculate Appropriate Min, Max, Sum and Mean Values.
            attTable->setBoolValue("first", true);
            RSGISCalcClumpStatsWithinAtt *calcAttStats = new RSGISCalcClumpStatsWithinAtt(attTable, bandStats, false, pxlCountIdx, firstFieldIdx);
            RSGISCalcImage calcImage(calcAttStats);
            calcImage.calcImage(datasets, numDatasets);
            delete calcAttStats;
            
            if(meanCalc)
            {
                RSGISFeature *feat;
                int feedback = attTable->getSize()/10;
                int feedbackCounter = 0;
                double pxlCount = 0;
                cout << "Started (Calc Mean): " << flush;
                // Calculate Statistics for each feature.
                for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
                {
                    if((i % feedback) == 0)
                    {
                        cout << "." << feedbackCounter << "." << flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    feat = attTable->getFeature(i);
                    pxlCount = feat->intFields->at(pxlCountIdx);
                    for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                    {   
                        if((*iterBands)->calcMean)
                        {
                            feat->floatFields->at((*iterBands)->meanIdx) = feat->floatFields->at((*iterBands)->meanIdx) / pxlCount;
                        }
                    }
                    
                }
                cout << " Complete.\n";
                
                if(stdDevCalc)
                {
                    
                    // Extract Data from Image.
                    attTable->setBoolValue("first", true);
                    calcAttStats = new RSGISCalcClumpStatsWithinAtt(attTable, bandStats, true, pxlCountIdx, firstFieldIdx);
                    RSGISCalcImage calcImageStdDev(calcAttStats);
                    calcImageStdDev.calcImage(datasets, numDatasets);
                    delete calcAttStats;
                    
                    feedbackCounter = 0;
                    cout << "Started (Calc StdDev): " << flush;
                    // Calculate Statistics for each feature.
                    for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
                    {
                        if((i % feedback) == 0)
                        {
                            cout << "." << feedbackCounter << "." << flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        feat = attTable->getFeature(i);
                        pxlCount = feat->intFields->at(pxlCountIdx);
                        for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                        {   
                            if((*iterBands)->calcStdDev)
                            {
                                feat->floatFields->at((*iterBands)->stdDevIdx) = sqrt(feat->floatFields->at((*iterBands)->stdDevIdx) / pxlCount);
                            }
                        }
                        
                    }
                    cout << " Complete.\n";
                }
            }            
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
        
    }
    
    RSGISPopulateAttributeTableBandStats::~RSGISPopulateAttributeTableBandStats()
    {
        
    }
    
    
    
    RSGISPopulateAttributeTableBandThresholdedStats::RSGISPopulateAttributeTableBandThresholdedStats()
    {
        
    }
    
    void RSGISPopulateAttributeTableBandThresholdedStats::populateWithBandStatisticsWithinAtt(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats) throw(RSGISImageCalcException, RSGISAttributeTableException)
    {
        if(numDatasets != 2)
        {
            throw RSGISImageCalcException("Two datasets are required for this fucntion.");
        }
        
        try 
        {
            if(datasets[0]->GetRasterCount() != 1)
            {
                throw RSGISImageCalcException("Clumps image should only have 1 image band.");
            }
            
            if(datasets[1]->GetRasterCount() < ((int)bandStats->size()))
            {
                throw RSGISImageCalcException("More band stats were requested than bands in the file.");
            }
            
            if(!attTable->hasAttribute("pxlcount"))
            {
                attTable->addAttIntField("pxlcount", 0);
            }
            else if(attTable->getDataType("pxlcount") != rsgis_int)
            {
                throw RSGISImageCalcException("Cannot proceed as \'pxlcount\' field is not of type integer.");
            }
            unsigned int pxlCountIdx = attTable->getFieldIndex("pxlcount");
            
            if(!attTable->hasAttribute("first"))
            {
                attTable->addAttBoolField("first", true);
            }
            else if(attTable->getDataType("first") != rsgis_bool)
            {
                throw RSGISImageCalcException("Cannot proceed as \'first\' field is not of type boolean.");
            }
            unsigned int firstFieldIdx = attTable->getFieldIndex("first");
            
            bool meanCalc = false;
            bool stdDevCalc = false;
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if((*iterBands)->band == 0)
                {
                    throw RSGISImageCalcException("Band numbers start at 1.");
                }
                else if(((int)(*iterBands)->band) > datasets[1]->GetRasterCount())
                {
                    throw RSGISImageCalcException("Band specified is not within the image.");
                }
                
                if(attTable->hasAttribute((*iterBands)->countField))
                {
                    if(attTable->getDataType((*iterBands)->countField) != rsgis_int)
                    {
                        throw RSGISAttributeTableException("Count field must be of type int.");
                    }
                    cerr << "Warning. Reusing field \'" << (*iterBands)->countField << "\'\n";
                }
                else
                {
                    attTable->addAttIntField((*iterBands)->countField, 0);
                }
                (*iterBands)->countIdx = attTable->getFieldIndex((*iterBands)->countField);
                
                if((*iterBands)->calcMin)
                {
                    if(attTable->hasAttribute((*iterBands)->minField))
                    {
                        if(attTable->getDataType((*iterBands)->minField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->minField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->minField, 0);
                    }
                    (*iterBands)->minIdx = attTable->getFieldIndex((*iterBands)->minField);
                }
                
                if((*iterBands)->calcMax)
                {
                    if(attTable->hasAttribute((*iterBands)->maxField))
                    {
                        if(attTable->getDataType((*iterBands)->maxField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->maxField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->maxField, 0);
                    }
                    (*iterBands)->maxIdx = attTable->getFieldIndex((*iterBands)->maxField);
                }
                
                if((*iterBands)->calcMean)
                {
                    meanCalc = true;
                    if(attTable->hasAttribute((*iterBands)->meanField))
                    {
                        if(attTable->getDataType((*iterBands)->meanField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->meanField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->meanField, 0);
                    }
                    (*iterBands)->meanIdx = attTable->getFieldIndex((*iterBands)->meanField);
                }
                
                if((*iterBands)->calcSum)
                {
                    if(attTable->hasAttribute((*iterBands)->sumField))
                    {
                        if(attTable->getDataType((*iterBands)->sumField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->sumField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->sumField, 0);
                    }
                    (*iterBands)->sumIdx = attTable->getFieldIndex((*iterBands)->sumField);
                }
                
                if((*iterBands)->calcStdDev)
                {
                    stdDevCalc = true;
                    if(attTable->hasAttribute((*iterBands)->stdDevField))
                    {
                        if(attTable->getDataType((*iterBands)->stdDevField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->stdDevField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->stdDevField, 0);
                    }
                    (*iterBands)->stdDevIdx = attTable->getFieldIndex((*iterBands)->stdDevField);
                }
                
                if((*iterBands)->calcMedian)
                {
                    cout << "WARNING: Cannot calculate median value (" << (*iterBands)->medianField << ") when processing within attribute table\n";
                }                
            }
            
            // Calculate Appropriate Min, Max, Sum and Mean Values.
            attTable->setBoolValue("first", true);
            RSGISCalcClumpStatsWithinAtt *calcAttStats = new RSGISCalcClumpStatsWithinAtt(attTable, bandStats, false, pxlCountIdx, firstFieldIdx);
            RSGISCalcImage calcImage(calcAttStats);
            calcImage.calcImage(datasets, numDatasets);
            delete calcAttStats;
            
            if(meanCalc)
            {
                RSGISFeature *feat;
                int feedback = attTable->getSize()/10;
                int feedbackCounter = 0;
                double pxlCount = 0;
                cout << "Started (Calc Mean): " << flush;
                // Calculate Statistics for each feature.
                for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
                {
                    if((i % feedback) == 0)
                    {
                        cout << "." << feedbackCounter << "." << flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    feat = attTable->getFeature(i);
                    for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                    {   
                        if((*iterBands)->calcMean)
                        {
                            pxlCount = feat->intFields->at((*iterBands)->countIdx);
                            feat->floatFields->at((*iterBands)->meanIdx) = feat->floatFields->at((*iterBands)->meanIdx) / pxlCount;
                        }
                    }
                    
                }
                cout << " Complete.\n";
                
                if(stdDevCalc)
                {
                    
                    // Extract Data from Image.
                    attTable->setBoolValue("first", true);
                    calcAttStats = new RSGISCalcClumpStatsWithinAtt(attTable, bandStats, true, pxlCountIdx, firstFieldIdx);
                    RSGISCalcImage calcImageStdDev(calcAttStats);
                    calcImageStdDev.calcImage(datasets, numDatasets);
                    delete calcAttStats;
                    
                    feedbackCounter = 0;
                    cout << "Started (Calc StdDev): " << flush;
                    // Calculate Statistics for each feature.
                    for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
                    {
                        if((i % feedback) == 0)
                        {
                            cout << "." << feedbackCounter << "." << flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        feat = attTable->getFeature(i);
                        for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                        {   
                            if((*iterBands)->calcStdDev)
                            {
                                pxlCount = feat->intFields->at((*iterBands)->countIdx);
                                feat->floatFields->at((*iterBands)->stdDevIdx) = sqrt(feat->floatFields->at((*iterBands)->stdDevIdx) / pxlCount);
                            }
                        }
                        
                    }
                    cout << " Complete.\n";
                }
            }            
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
        
    }
    
    RSGISPopulateAttributeTableBandThresholdedStats::~RSGISPopulateAttributeTableBandThresholdedStats()
    {
        
    }
    
    
    
    

    RSGISPopulateAttributeTableBandStatsMeanLit::RSGISPopulateAttributeTableBandStatsMeanLit()
    {
        
    }
    
    void RSGISPopulateAttributeTableBandStatsMeanLit::populateWithBandStatisticsInMem(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats, unsigned int meanLitBand, string meanLitField, bool useMeanLitValAbove) throw(RSGISImageCalcException, RSGISAttributeTableException)
    {
        if(numDatasets != 3)
        {
            throw RSGISImageCalcException("Three datasets are required for this fucntion.");
        }
        
        try 
        {
            if(datasets[0]->GetRasterCount() != 1)
            {
                throw RSGISImageCalcException("Clumps image should only have 1 image band.");
            }
            
            if(datasets[1]->GetRasterCount() < ((int)bandStats->size()))
            {
                throw RSGISImageCalcException("More band stats were requested than bands in the file.");
            }
            
            unsigned int meanLitFieldIdx = 0;
            RSGISAttributeDataType meanLitFieldDT = rsgis_na;
            if(!attTable->hasAttribute(meanLitField))
            {
                throw RSGISAttributeTableException("The mean-lit field supplied is not present within the attribute table.");
            }
            else
            {
                meanLitFieldIdx = attTable->getFieldIndex(meanLitField);
                meanLitFieldDT = attTable->getDataType(meanLitField);
                if(!((meanLitFieldDT == rsgis_int) | (meanLitFieldDT == rsgis_float)))
                {
                    throw RSGISAttributeTableException("Mean-lit field must has data type of either int or float.");
                }
            }
            
            unsigned int numDataBands = bandStats->size();
            unsigned int *dataBandIdxs = new unsigned int[numDataBands];
            
            bool usePxlCount = true;
            if(!attTable->hasAttribute("pxlcount"))
            {
                usePxlCount = false;
            }
            else if(attTable->getDataType("pxlcount") != rsgis_int)
            {
                usePxlCount = false;
            }
            unsigned int pxlCountIdx = attTable->getFieldIndex("pxlcount");
            
            if(!usePxlCount)
            {
                cerr << "Warning: \'pxlcount\' field is not available\n";
            }
            
            unsigned int bandCount = 0;
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if((*iterBands)->band == 0)
                {
                    throw RSGISImageCalcException("Band numbers start at 1.");
                }
                else if(((int)(*iterBands)->band) > datasets[1]->GetRasterCount())
                {
                    throw RSGISImageCalcException("Band specified is not within the image.");
                }
                
                dataBandIdxs[bandCount++] = (*iterBands)->band;                
                
                if((*iterBands)->calcMin)
                {
                    if(attTable->hasAttribute((*iterBands)->minField))
                    {
                        if(attTable->getDataType((*iterBands)->minField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->minField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->minField, 0);
                    }
                    (*iterBands)->minIdx = attTable->getFieldIndex((*iterBands)->minField);
                }
                
                if((*iterBands)->calcMax)
                {
                    if(attTable->hasAttribute((*iterBands)->maxField))
                    {
                        if(attTable->getDataType((*iterBands)->maxField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->maxField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->maxField, 0);
                    }
                    (*iterBands)->maxIdx = attTable->getFieldIndex((*iterBands)->maxField);
                }
                
                if((*iterBands)->calcMean)
                {
                    if(attTable->hasAttribute((*iterBands)->meanField))
                    {
                        if(attTable->getDataType((*iterBands)->meanField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->meanField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->meanField, 0);
                    }
                    (*iterBands)->meanIdx = attTable->getFieldIndex((*iterBands)->meanField);
                }
                
                if((*iterBands)->calcSum)
                {
                    if(attTable->hasAttribute((*iterBands)->sumField))
                    {
                        if(attTable->getDataType((*iterBands)->sumField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->sumField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->sumField, 0);
                    }
                    (*iterBands)->sumIdx = attTable->getFieldIndex((*iterBands)->sumField);
                }
                
                if((*iterBands)->calcStdDev)
                {
                    if(attTable->hasAttribute((*iterBands)->stdDevField))
                    {
                        if(attTable->getDataType((*iterBands)->stdDevField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->stdDevField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->stdDevField, 0);
                    }
                    (*iterBands)->stdDevIdx = attTable->getFieldIndex((*iterBands)->stdDevField);
                }
                
                if((*iterBands)->calcMedian)
                {
                    if(attTable->hasAttribute((*iterBands)->medianField))
                    {
                        if(attTable->getDataType((*iterBands)->medianField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->medianField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->medianField, 0);
                    }
                    (*iterBands)->medianIdx = attTable->getFieldIndex((*iterBands)->medianField);
                }                
            }
            
            RSGISFeature *feat;
            vector<vector<double> > **clumpData = new vector<vector<double> >*[numDataBands];
            for(unsigned int n = 0; n < numDataBands; ++n)
            {
                clumpData[n] = new vector<vector<double> >();
                clumpData[n]->reserve(attTable->getSize());
            }
            
            for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
            {
                feat = attTable->getFeature(i);
                for(unsigned int n = 0; n < numDataBands; ++n)
                {
                    clumpData[n]->push_back(vector<double>());
                    if(usePxlCount)
                    {
                        clumpData[n]->at(i).reserve(feat->intFields->at(pxlCountIdx));
                    }
                }
            }
            
            // Extract Data from Image.
            RSGISGetPixelValuesForClumpsMeanLit *getImageVals = new RSGISGetPixelValuesForClumpsMeanLit(attTable, clumpData, numDataBands, dataBandIdxs, meanLitBand, meanLitFieldIdx, meanLitFieldDT, useMeanLitValAbove);
            RSGISCalcImage calcImage(getImageVals);
            calcImage.calcImage(datasets, numDatasets);
            delete getImageVals;
            
            double min = 0;
            double max = 0;
            double mean = 0;
            double stddev = 0;
            double median = 0;
            double sum = 0;
            
            int feedback = attTable->getSize()/10;
			int feedbackCounter = 0;
			cout << "Started" << flush;
            // Calculate Statistics for each feature.
            for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
            {
                if((i % feedback) == 0)
				{
					cout << ".." << feedbackCounter << ".." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
                
                feat = attTable->getFeature(i);
                
                bandCount = 0;
                for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {   
                    if((*iterBands)->calcMin & (*iterBands)->calcMax)
                    {
                        gsl_stats_minmax (&min, &max, &(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->minIdx) = min;
                        feat->floatFields->at((*iterBands)->maxIdx) = max;
                    }
                    else if((*iterBands)->calcMin)
                    {
                        min = gsl_stats_min(&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->minIdx) = min;
                    }
                    else if((*iterBands)->calcMax)
                    {
                        max = gsl_stats_max(&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->maxIdx) = max;
                    }
                    
                    if((*iterBands)->calcMean & (*iterBands)->calcStdDev)
                    {
                        mean = gsl_stats_mean (&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        stddev = gsl_stats_sd_m (&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size(), mean);
                        feat->floatFields->at((*iterBands)->meanIdx) = mean;
                        feat->floatFields->at((*iterBands)->stdDevIdx) = stddev;
                    }
                    else if((*iterBands)->calcMean & !(*iterBands)->calcStdDev)
                    {
                        mean = gsl_stats_mean (&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->meanIdx) = mean;
                    }
                    else if(!(*iterBands)->calcMean & (*iterBands)->calcStdDev)
                    {
                        stddev = gsl_stats_sd (&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->stdDevIdx) = stddev;
                    }
                    
                    if((*iterBands)->calcSum)
                    {
                        sum = 0;
                        for(vector<double>::iterator iterVals = clumpData[bandCount]->at(i).begin(); iterVals != clumpData[bandCount]->at(i).end(); ++iterVals)
                        {
                            sum += *iterVals;
                        }
                        feat->floatFields->at((*iterBands)->sumIdx) = sum;
                    }
                    
                    if((*iterBands)->calcMedian)
                    {                        
                        sort(clumpData[bandCount]->at(i).begin(), clumpData[bandCount]->at(i).end());
                        median = gsl_stats_median_from_sorted_data(&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->medianIdx) = median;
                    }
                    
                    ++bandCount;
                }
                
            }
            cout << " Complete.\n";
            
            delete[] dataBandIdxs;
            for(unsigned int n = 0; n < numDataBands; ++n)
            {
                delete clumpData[n];
            }
            delete[] clumpData;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
    }
    
    void RSGISPopulateAttributeTableBandStatsMeanLit::populateWithBandStatisticsWithinAtt(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats, unsigned int meanLitBand, string meanLitField, bool useMeanLitValAbove) throw(RSGISImageCalcException, RSGISAttributeTableException)
    {
        if(numDatasets != 3)
        {
            throw RSGISImageCalcException("Three datasets are required for this fucntion.");
        }
        
        try 
        {
            if(datasets[0]->GetRasterCount() != 1)
            {
                throw RSGISImageCalcException("Clumps image should only have 1 image band.");
            }
            
            if(datasets[1]->GetRasterCount() < ((int)bandStats->size()))
            {
                throw RSGISImageCalcException("More band stats were requested than bands in the file.");
            }
            
            unsigned int meanLitFieldIdx = 0;
            RSGISAttributeDataType meanLitFieldDT = rsgis_na;
            if(!attTable->hasAttribute(meanLitField))
            {
                throw RSGISAttributeTableException("The mean-lit field supplied is not present within the attribute table.");
            }
            else
            {
                meanLitFieldIdx = attTable->getFieldIndex(meanLitField);
                meanLitFieldDT = attTable->getDataType(meanLitField);
                if(!((meanLitFieldDT == rsgis_int) | (meanLitFieldDT == rsgis_float)))
                {
                    throw RSGISAttributeTableException("Mean-lit field must has data type of either int or float.");
                }
            }
            
            if(!attTable->hasAttribute("mlpxlcount"))
            {
                attTable->addAttIntField("mlpxlcount", 0);
            }
            else if(attTable->getDataType("mlpxlcount") != rsgis_int)
            {
                throw RSGISImageCalcException("Cannot proceed as \'mlpxlcount\' field is not of type integer.");
            }
            unsigned int pxlCountIdx = attTable->getFieldIndex("mlpxlcount");
            
            if(!attTable->hasAttribute("first"))
            {
                attTable->addAttBoolField("first", true);
            }
            else if(attTable->getDataType("first") != rsgis_bool)
            {
                throw RSGISImageCalcException("Cannot proceed as \'first\' field is not of type boolean.");
            }
            unsigned int firstFieldIdx = attTable->getFieldIndex("first");
                        
            bool meanCalc = false;
            bool stdDevCalc = false;
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if((*iterBands)->band == 0)
                {
                    throw RSGISImageCalcException("Band numbers start at 1.");
                }
                else if(((int)(*iterBands)->band) > datasets[1]->GetRasterCount())
                {
                    throw RSGISImageCalcException("Band specified is not within the image.");
                }
                
                if((*iterBands)->calcMin)
                {
                    if(attTable->hasAttribute((*iterBands)->minField))
                    {
                        if(attTable->getDataType((*iterBands)->minField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->minField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->minField, 0);
                    }
                    (*iterBands)->minIdx = attTable->getFieldIndex((*iterBands)->minField);
                }
                
                if((*iterBands)->calcMax)
                {
                    if(attTable->hasAttribute((*iterBands)->maxField))
                    {
                        if(attTable->getDataType((*iterBands)->maxField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->maxField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->maxField, 0);
                    }
                    (*iterBands)->maxIdx = attTable->getFieldIndex((*iterBands)->maxField);
                }
                
                if((*iterBands)->calcMean)
                {
                    meanCalc = true;
                    if(attTable->hasAttribute((*iterBands)->meanField))
                    {
                        if(attTable->getDataType((*iterBands)->meanField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->meanField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->meanField, 0);
                    }
                    (*iterBands)->meanIdx = attTable->getFieldIndex((*iterBands)->meanField);
                }
                
                if((*iterBands)->calcSum)
                {
                    if(attTable->hasAttribute((*iterBands)->sumField))
                    {
                        if(attTable->getDataType((*iterBands)->sumField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->sumField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->sumField, 0);
                    }
                    (*iterBands)->sumIdx = attTable->getFieldIndex((*iterBands)->sumField);
                }
                
                if((*iterBands)->calcStdDev)
                {
                    stdDevCalc = true;
                    if(attTable->hasAttribute((*iterBands)->stdDevField))
                    {
                        if(attTable->getDataType((*iterBands)->stdDevField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->stdDevField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->stdDevField, 0);
                    }
                    (*iterBands)->stdDevIdx = attTable->getFieldIndex((*iterBands)->stdDevField);
                }
                
                if((*iterBands)->calcMedian)
                {
                    cout << "WARNING: Cannot calculate median value (" << (*iterBands)->medianField << ") when processing within attribute table\n";
                }
            }
            
            
            // Extract Data from Image.
            attTable->setBoolValue("first", true);
            RSGISClumpsMeanLitStatsWithinAtt *calcAttStats = new RSGISClumpsMeanLitStatsWithinAtt(attTable, bandStats, false, pxlCountIdx, firstFieldIdx, meanLitBand, meanLitFieldIdx, meanLitFieldDT, useMeanLitValAbove);
            RSGISCalcImage calcImage(calcAttStats);
            calcImage.calcImage(datasets, numDatasets);
            delete calcAttStats;
            
            if(meanCalc)
            {
                RSGISFeature *feat;
                int feedback = attTable->getSize()/10;
                int feedbackCounter = 0;
                double pxlCount = 0;
                cout << "Started (Calc Mean): " << flush;
                // Calculate Statistics for each feature.
                for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
                {
                    if((i % feedback) == 0)
                    {
                        cout << "." << feedbackCounter << "." << flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    feat = attTable->getFeature(i);
                    pxlCount = feat->intFields->at(pxlCountIdx);
                    for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                    {   
                        if((*iterBands)->calcMean)
                        {
                            feat->floatFields->at((*iterBands)->meanIdx) = feat->floatFields->at((*iterBands)->meanIdx) / pxlCount;
                        }
                    }
                    
                }
                cout << " Complete.\n";
                
                if(stdDevCalc)
                {
                    
                    // Extract Data from Image.
                    attTable->setBoolValue("first", true);
                    RSGISClumpsMeanLitStatsWithinAtt *calcAttStats = new RSGISClumpsMeanLitStatsWithinAtt(attTable, bandStats, true, pxlCountIdx, firstFieldIdx, meanLitBand, meanLitFieldIdx, meanLitFieldDT, useMeanLitValAbove);
                    RSGISCalcImage calcImage(calcAttStats);
                    calcImage.calcImage(datasets, numDatasets);
                    delete calcAttStats;
                    
                    feedbackCounter = 0;
                    cout << "Started (Calc StdDev): " << flush;
                    // Calculate Statistics for each feature.
                    for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
                    {
                        if((i % feedback) == 0)
                        {
                            cout << "." << feedbackCounter << "." << flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        feat = attTable->getFeature(i);
                        pxlCount = feat->intFields->at(pxlCountIdx);
                        for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                        {   
                            if((*iterBands)->calcStdDev)
                            {
                                feat->floatFields->at((*iterBands)->stdDevIdx) = sqrt(feat->floatFields->at((*iterBands)->stdDevIdx) / pxlCount);
                            }
                        }
                        
                    }
                    cout << " Complete.\n";
                }
            } 
            
            
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
    }
    
    RSGISPopulateAttributeTableBandStatsMeanLit::~RSGISPopulateAttributeTableBandStatsMeanLit()
    {
        
    }
        
    

    RSGISPopulateAttributeTableBandStatsMeanLitBands::RSGISPopulateAttributeTableBandStatsMeanLitBands()
    {
        
    }
    
    void RSGISPopulateAttributeTableBandStatsMeanLitBands::populateWithBandStatisticsInMem(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStatsMeanLit*> *bandStats) throw(RSGISImageCalcException, RSGISAttributeTableException)
    {
        if(numDatasets != 2)
        {
            throw RSGISImageCalcException("Two datasets are required for this fucntion.");
        }
        
        try 
        {
            if(datasets[0]->GetRasterCount() != 1)
            {
                throw RSGISImageCalcException("Clumps image should only have 1 image band.");
            }
            
            if(datasets[1]->GetRasterCount() < ((int)bandStats->size()))
            {
                throw RSGISImageCalcException("More band stats were requested than bands in the file.");
            }
            
            unsigned int numDataBands = bandStats->size();
            unsigned int *dataBandIdxs = new unsigned int[numDataBands];
            
            bool usePxlCount = true;
            if(!attTable->hasAttribute("pxlcount"))
            {
                usePxlCount = false;
            }
            else if(attTable->getDataType("pxlcount") != rsgis_int)
            {
                usePxlCount = false;
            }
            unsigned int pxlCountIdx = attTable->getFieldIndex("pxlcount");
            
            if(!usePxlCount)
            {
                cerr << "Warning: \'pxlcount\' field is not available\n";
            }
            
            unsigned int bandCount = 0;
            for(vector<RSGISBandAttStatsMeanLit*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if((*iterBands)->band == 0)
                {
                    throw RSGISImageCalcException("Band numbers start at 1.");
                }
                else if(((int)(*iterBands)->band) > datasets[1]->GetRasterCount())
                {
                    throw RSGISImageCalcException("Band specified is not within the image.");
                }
                
                if(!attTable->hasAttribute((*iterBands)->fieldName))
                {
                    string message = (*iterBands)->fieldName + string(" is not found within the attribute table.");
                    throw RSGISImageCalcException(message);
                }
                else
                {
                    (*iterBands)->fieldIdx = attTable->getFieldIndex((*iterBands)->fieldName);
                    (*iterBands)->fieldDT = attTable->getDataType((*iterBands)->fieldName);
                }
                
                dataBandIdxs[bandCount++] = (*iterBands)->band;                
                
                if((*iterBands)->calcMin)
                {
                    if(attTable->hasAttribute((*iterBands)->minField))
                    {
                        if(attTable->getDataType((*iterBands)->minField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->minField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->minField, 0);
                    }
                    (*iterBands)->minIdx = attTable->getFieldIndex((*iterBands)->minField);
                }
                
                if((*iterBands)->calcMax)
                {
                    if(attTable->hasAttribute((*iterBands)->maxField))
                    {
                        if(attTable->getDataType((*iterBands)->maxField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->maxField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->maxField, 0);
                    }
                    (*iterBands)->maxIdx = attTable->getFieldIndex((*iterBands)->maxField);
                }
                
                if((*iterBands)->calcMean)
                {
                    if(attTable->hasAttribute((*iterBands)->meanField))
                    {
                        if(attTable->getDataType((*iterBands)->meanField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->meanField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->meanField, 0);
                    }
                    (*iterBands)->meanIdx = attTable->getFieldIndex((*iterBands)->meanField);
                }
                
                if((*iterBands)->calcSum)
                {
                    if(attTable->hasAttribute((*iterBands)->sumField))
                    {
                        if(attTable->getDataType((*iterBands)->sumField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->sumField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->sumField, 0);
                    }
                    (*iterBands)->sumIdx = attTable->getFieldIndex((*iterBands)->sumField);
                }
                
                if((*iterBands)->calcStdDev)
                {
                    if(attTable->hasAttribute((*iterBands)->stdDevField))
                    {
                        if(attTable->getDataType((*iterBands)->stdDevField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->stdDevField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->stdDevField, 0);
                    }
                    (*iterBands)->stdDevIdx = attTable->getFieldIndex((*iterBands)->stdDevField);
                }
                
                if((*iterBands)->calcMedian)
                {
                    if(attTable->hasAttribute((*iterBands)->medianField))
                    {
                        if(attTable->getDataType((*iterBands)->medianField) != rsgis_float)
                        {
                            throw RSGISAttributeTableException("All outputs must be of type float.");
                        }
                        cerr << "Warning. Reusing field \'" << (*iterBands)->medianField << "\'\n";
                    }
                    else
                    {
                        attTable->addAttFloatField((*iterBands)->medianField, 0);
                    }
                    (*iterBands)->medianIdx = attTable->getFieldIndex((*iterBands)->medianField);
                }                
            }
            
            RSGISFeature *feat;
            vector<vector<double> > **clumpData = new vector<vector<double> >*[numDataBands];
            for(unsigned int n = 0; n < numDataBands; ++n)
            {
                clumpData[n] = new vector<vector<double> >();
                clumpData[n]->reserve(attTable->getSize());
            }
            
            for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
            {
                feat = attTable->getFeature(i);
                for(unsigned int n = 0; n < numDataBands; ++n)
                {
                    clumpData[n]->push_back(vector<double>());
                    if(usePxlCount)
                    {
                        clumpData[n]->at(i).reserve(feat->intFields->at(pxlCountIdx));
                    }
                }
            }
            
            // Extract Data from Image.
            RSGISGetPixelValuesForClumps *getImageVals = new RSGISGetPixelValuesForClumps(clumpData, numDataBands, dataBandIdxs);
            RSGISCalcImage calcImage(getImageVals);
            calcImage.calcImage(datasets, numDatasets);
            delete getImageVals;
            
            double min = 0;
            double max = 0;
            double mean = 0;
            double stddev = 0;
            double median = 0;
            double sum = 0;
            double thresholdVal = 0;
            
            int feedback = attTable->getSize()/10;
			int feedbackCounter = 0;
			cout << "Started" << flush;
            // Calculate Statistics for each feature.
            for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
            {
                if((i % feedback) == 0)
				{
					cout << ".." << feedbackCounter << ".." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
                
                feat = attTable->getFeature(i);
                
                bandCount = 0;
                for(vector<RSGISBandAttStatsMeanLit*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {   
                    
                    if((*iterBands)->fieldDT == rsgis_int)
                    {
                        thresholdVal = feat->intFields->at((*iterBands)->fieldIdx);
                    }
                    else if((*iterBands)->fieldDT == rsgis_float)
                    {
                        thresholdVal = feat->floatFields->at((*iterBands)->fieldIdx);
                    }
                    else
                    {
                        throw RSGISImageCalcException("Threshold values must come from either an integer or floating point field.");
                    }
                    
                    for(vector<double>::iterator iterVals = clumpData[bandCount]->at(i).begin(); iterVals != clumpData[bandCount]->at(i).end(); )
                    {
                        if((*iterBands)->useUpperVals)
                        {
                            if((*iterVals) < thresholdVal)
                            {
                                iterVals = clumpData[bandCount]->at(i).erase(iterVals);
                            }
                            else
                            {
                                ++iterVals;
                            }
                        }
                        else
                        {
                            if((*iterVals) > thresholdVal)
                            {
                                iterVals = clumpData[bandCount]->at(i).erase(iterVals);
                            }
                            else
                            {
                                ++iterVals;
                            }
                        }
                    }
                        
                    
                    if((*iterBands)->calcMin & (*iterBands)->calcMax)
                    {
                        gsl_stats_minmax (&min, &max, &(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->minIdx) = min;
                        feat->floatFields->at((*iterBands)->maxIdx) = max;
                    }
                    else if((*iterBands)->calcMin)
                    {
                        min = gsl_stats_min(&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->minIdx) = min;
                    }
                    else if((*iterBands)->calcMax)
                    {
                        max = gsl_stats_max(&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->maxIdx) = max;
                    }
                    
                    if((*iterBands)->calcMean & (*iterBands)->calcStdDev)
                    {
                        mean = gsl_stats_mean (&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        stddev = gsl_stats_sd_m (&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size(), mean);
                        feat->floatFields->at((*iterBands)->meanIdx) = mean;
                        feat->floatFields->at((*iterBands)->stdDevIdx) = stddev;
                    }
                    else if((*iterBands)->calcMean & !(*iterBands)->calcStdDev)
                    {
                        mean = gsl_stats_mean (&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->meanIdx) = mean;
                    }
                    else if(!(*iterBands)->calcMean & (*iterBands)->calcStdDev)
                    {
                        stddev = gsl_stats_sd (&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->stdDevIdx) = stddev;
                    }
                    
                    if((*iterBands)->calcSum)
                    {
                        sum = 0;
                        for(vector<double>::iterator iterVals = clumpData[bandCount]->at(i).begin(); iterVals != clumpData[bandCount]->at(i).end(); ++iterVals)
                        {
                            sum += *iterVals;
                        }
                        feat->floatFields->at((*iterBands)->sumIdx) = sum;
                    }
                    
                    if((*iterBands)->calcMedian)
                    {                        
                        sort(clumpData[bandCount]->at(i).begin(), clumpData[bandCount]->at(i).end());
                        median = gsl_stats_median_from_sorted_data(&(clumpData[bandCount]->at(i))[0], 1, clumpData[bandCount]->at(i).size());
                        feat->floatFields->at((*iterBands)->medianIdx) = median;
                    }
                    
                    ++bandCount;
                }
                
            }
            cout << " Complete.\n";
            
            delete[] dataBandIdxs;
            for(unsigned int n = 0; n < numDataBands; ++n)
            {
                delete clumpData[n];
            }
            delete[] clumpData;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
    }
    
    RSGISPopulateAttributeTableBandStatsMeanLitBands::~RSGISPopulateAttributeTableBandStatsMeanLitBands()
    {
        
    }

    
    RSGISGetPixelValuesForClumps::RSGISGetPixelValuesForClumps(vector<vector<double> > **clumpData, unsigned int numDataBands, unsigned int *dataBandIdxs):RSGISCalcImageValue(0)
    {
        this->clumpData = clumpData;
        this->numDataBands = numDataBands;
        this->dataBandIdxs = dataBandIdxs;
    }
    
    void RSGISGetPixelValuesForClumps::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            
            try
            {   
                bool nanPresent = false;
                for(int i = 0; i < numDataBands; ++i)
                {
                    if(boost::math::isnan(bandValues[dataBandIdxs[i]]))
                    {
                        nanPresent = true;
                    }
                }
                
                if(!nanPresent)
                {                
                    for(unsigned int i = 0; i < numDataBands; ++i)
                    {                   
                        clumpData[i]->at(clumpIdx).push_back(bandValues[dataBandIdxs[i]]);
                    }
                }
                
            }
            catch(RSGISAttributeTableException &e)
            {
                cout << "clumpIdx = " << clumpIdx << endl;
                throw RSGISImageCalcException(e.what());
            }
        } 
    }
    
    RSGISGetPixelValuesForClumps::~RSGISGetPixelValuesForClumps()
    {
        
    }
    
    
    RSGISCalcClumpStatsWithinAtt::RSGISCalcClumpStatsWithinAtt(RSGISAttributeTable *attTable, vector<RSGISBandAttStats*> *bandStats, bool calcStdDev, unsigned int pxlCountIdx, unsigned int firstFieldIdx):RSGISCalcImageValue(0)
    {
        this->attTable = attTable;
        this->bandStats = bandStats;
        this->calcStdDev = calcStdDev;
        this->firstFieldIdx = firstFieldIdx;
        this->pxlCountIdx = pxlCountIdx;
    }
    
    void RSGISCalcClumpStatsWithinAtt::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            
            try
            {
                float bandVal = 0;
                RSGISFeature *feat = attTable->getFeature(clumpIdx);
                if(feat->boolFields->at(firstFieldIdx))
                {
                    bool nanPresent = false;
                    for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                    {
                        if(boost::math::isnan(bandValues[(*iterBands)->band]))
                        {
                            nanPresent = true;
                        }
                    }
                    
                    if(!nanPresent)
                    {
                        feat->intFields->at(pxlCountIdx) = 1;
                        for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                        {
                            bandVal = bandValues[(*iterBands)->band];
                            if(this->calcStdDev)
                            {
                                if((*iterBands)->calcStdDev)
                                {
                                    feat->floatFields->at((*iterBands)->stdDevIdx) = pow((feat->floatFields->at((*iterBands)->meanIdx) - bandVal),2);
                                }
                            }
                            else
                            {
                                if((*iterBands)->calcMin)
                                {
                                    feat->floatFields->at((*iterBands)->minIdx) = bandVal;
                                }
                                if((*iterBands)->calcMax)
                                {
                                    feat->floatFields->at((*iterBands)->maxIdx) = bandVal;
                                }
                                if((*iterBands)->calcMean)
                                {
                                    feat->floatFields->at((*iterBands)->meanIdx) = bandVal;
                                }
                                if((*iterBands)->calcSum)
                                {
                                    feat->floatFields->at((*iterBands)->sumIdx) = bandVal;
                                }
                            }
                        }
                        
                        feat->boolFields->at(firstFieldIdx) = false;
                    }                    
                }
                else
                {
                    bool nanPresent = false;
                    for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                    {
                        if(boost::math::isnan(bandValues[(*iterBands)->band]))
                        {
                            nanPresent = true;
                        }
                    }
                    
                    if(!nanPresent)
                    {
                        feat->intFields->at(pxlCountIdx) += 1;
                        
                        for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                        {
                            bandVal = bandValues[(*iterBands)->band];
                            if(this->calcStdDev)
                            {
                                if((*iterBands)->calcStdDev)
                                {
                                    feat->floatFields->at((*iterBands)->stdDevIdx) += pow((feat->floatFields->at((*iterBands)->meanIdx) - bandVal),2);
                                }
                            }
                            else
                            {
                                if((*iterBands)->calcMin)
                                {
                                    if(bandVal < feat->floatFields->at((*iterBands)->minIdx))
                                    {
                                        feat->floatFields->at((*iterBands)->minIdx) = bandVal;
                                    }
                                }
                                if((*iterBands)->calcMax)
                                {
                                    if(bandVal > feat->floatFields->at((*iterBands)->maxIdx))
                                    {
                                        feat->floatFields->at((*iterBands)->maxIdx) = bandVal;
                                    }
                                }
                                if((*iterBands)->calcMean)
                                {
                                    feat->floatFields->at((*iterBands)->meanIdx) += bandVal;
                                }
                                if((*iterBands)->calcSum)
                                {
                                    feat->floatFields->at((*iterBands)->sumIdx) += bandVal;
                                }
                            }
                        }
                    }
                }                
            }
            catch(RSGISAttributeTableException &e)
            {
                cout << "clumpIdx = " << clumpIdx << endl;
                throw RSGISImageCalcException(e.what());
            }
        } 
    }
    
    RSGISCalcClumpStatsWithinAtt::~RSGISCalcClumpStatsWithinAtt()
    {
        
    }
    
    
    
    RSGISCalcClumpThresholdedStatsWithinAtt::RSGISCalcClumpThresholdedStatsWithinAtt(RSGISAttributeTable *attTable, vector<RSGISBandAttStats*> *bandStats, bool calcStdDev, unsigned int firstFieldIdx):RSGISCalcImageValue(0)
    {
        this->attTable = attTable;
        this->bandStats = bandStats;
        this->calcStdDev = calcStdDev;
        this->firstFieldIdx = firstFieldIdx;
    }
    
    void RSGISCalcClumpThresholdedStatsWithinAtt::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            
            try
            {
                float bandVal = 0;
                RSGISFeature *feat = attTable->getFeature(clumpIdx);
                if(feat->boolFields->at(firstFieldIdx))
                {
                    for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                    {
                        bandVal = bandValues[(*iterBands)->band];
                        if((!boost::math::isnan(bandVal)) && (bandVal > (*iterBands)->threshold))
                        {
                            if(this->calcStdDev)
                            {
                                if((*iterBands)->calcStdDev)
                                {
                                    feat->floatFields->at((*iterBands)->stdDevIdx) = pow((feat->floatFields->at((*iterBands)->meanIdx) - bandVal),2);
                                }
                            }
                            else
                            {
                                if((*iterBands)->calcMin)
                                {
                                    feat->floatFields->at((*iterBands)->minIdx) = bandVal;
                                }
                                if((*iterBands)->calcMax)
                                {
                                    feat->floatFields->at((*iterBands)->maxIdx) = bandVal;
                                }
                                if((*iterBands)->calcMean)
                                {
                                    feat->floatFields->at((*iterBands)->meanIdx) = bandVal;
                                }
                                if((*iterBands)->calcSum)
                                {
                                    feat->floatFields->at((*iterBands)->sumIdx) = bandVal;
                                }
                                
                                feat->intFields->at((*iterBands)->countIdx) = 1;
                            }
                            
                            feat->boolFields->at(firstFieldIdx) = false;
                        }                        
                    }
                }
                else
                {
                    for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                    {
                        bandVal = bandValues[(*iterBands)->band];
                        if((!boost::math::isnan(bandVal)) && (bandVal > (*iterBands)->threshold))
                        {
                            if(this->calcStdDev)
                            {
                                if((*iterBands)->calcStdDev)
                                {
                                    feat->floatFields->at((*iterBands)->stdDevIdx) += pow((feat->floatFields->at((*iterBands)->meanIdx) - bandVal),2);
                                }
                            }
                            else
                            {
                                if((*iterBands)->calcMin)
                                {
                                    if(bandVal < feat->floatFields->at((*iterBands)->minIdx))
                                    {
                                        feat->floatFields->at((*iterBands)->minIdx) = bandVal;
                                    }
                                }
                                if((*iterBands)->calcMax)
                                {
                                    if(bandVal > feat->floatFields->at((*iterBands)->maxIdx))
                                    {
                                        feat->floatFields->at((*iterBands)->maxIdx) = bandVal;
                                    }
                                }
                                if((*iterBands)->calcMean)
                                {
                                    feat->floatFields->at((*iterBands)->meanIdx) += bandVal;
                                }
                                if((*iterBands)->calcSum)
                                {
                                    feat->floatFields->at((*iterBands)->sumIdx) += bandVal;
                                }
                                
                                feat->intFields->at((*iterBands)->countIdx) += 1;
                            }
                        }
                    }
                }                
            }
            catch(RSGISAttributeTableException &e)
            {
                cout << "clumpIdx = " << clumpIdx << endl;
                throw RSGISImageCalcException(e.what());
            }
        } 
    }
    
    RSGISCalcClumpThresholdedStatsWithinAtt::~RSGISCalcClumpThresholdedStatsWithinAtt()
    {
        
    }
    
    
    
    
    RSGISGetPixelValuesForClumpsMeanLit::RSGISGetPixelValuesForClumpsMeanLit(RSGISAttributeTable *attTable, vector<vector<double> > **clumpData, unsigned int numDataBands, unsigned int *dataBandIdxs, unsigned int meanLitBand, unsigned int meanLitFieldIdx, RSGISAttributeDataType meanLitFieldDT, bool useMeanLitValAbove):RSGISCalcImageValue(0)
    {
        this->attTable = attTable;
        this->clumpData = clumpData;
        this->numDataBands = numDataBands;
        this->dataBandIdxs = dataBandIdxs;
        this->meanLitBand = meanLitBand;
        this->meanLitFieldIdx = meanLitFieldIdx;
        this->meanLitFieldDT = meanLitFieldDT;
        this->useMeanLitValAbove = useMeanLitValAbove;
    }
    
    void RSGISGetPixelValuesForClumpsMeanLit::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            
            try
            {   
                RSGISFeature *feat = attTable->getFeature(clumpIdx);
                double threshold = 0;
                if(this->meanLitFieldDT == rsgis_int)
                {
                    threshold = feat->intFields->at(this->meanLitFieldIdx);
                }
                else if(this->meanLitFieldDT == rsgis_float)
                {
                    threshold = feat->floatFields->at(this->meanLitFieldIdx);
                }
                else
                {
                    RSGISImageCalcException("Mean-lit threshold field must be of type int or float.");
                }
                
                bool withinThreshold = false;
                
                if((this->useMeanLitValAbove) & (bandValues[meanLitBand] > threshold))
                {
                    withinThreshold = true;
                }
                else if((!this->useMeanLitValAbove) & (bandValues[meanLitBand] < threshold))
                {
                    withinThreshold = true;
                }
                
                if(withinThreshold)
                {
                    bool nanPresent = false;
                    for(unsigned int i = 0; i < numDataBands; ++i)
                    {  
                        if(boost::math::isnan(bandValues[dataBandIdxs[i]]))
                        {
                            nanPresent = true;
                        }
                    }
                    
                    if(!nanPresent)
                    { 
                        for(unsigned int i = 0; i < numDataBands; ++i)
                        {                   
                            clumpData[i]->at(clumpIdx).push_back(bandValues[dataBandIdxs[i]]);
                        }
                    }
                }
            }
            catch(RSGISAttributeTableException &e)
            {
                cout << "clumpIdx = " << clumpIdx << endl;
                throw RSGISImageCalcException(e.what());
            }
        } 
    }
    
    RSGISGetPixelValuesForClumpsMeanLit::~RSGISGetPixelValuesForClumpsMeanLit()
    {
        
    }
    
    RSGISClumpsMeanLitStatsWithinAtt::RSGISClumpsMeanLitStatsWithinAtt(RSGISAttributeTable *attTable, vector<RSGISBandAttStats*> *bandStats, bool calcStdDev, unsigned int pxlCountIdx, unsigned int firstFieldIdx, unsigned int meanLitBand, unsigned int meanLitFieldIdx, RSGISAttributeDataType meanLitFieldDT, bool useMeanLitValAbove):RSGISCalcImageValue(0)
    {
        this->attTable = attTable;
        this->bandStats = bandStats;
        this->calcStdDev = calcStdDev;
        this->pxlCountIdx = pxlCountIdx;
        this->firstFieldIdx = firstFieldIdx;
        this->meanLitBand = meanLitBand;
        this->meanLitFieldIdx = meanLitFieldIdx;
        this->meanLitFieldDT = meanLitFieldDT;
        this->useMeanLitValAbove = useMeanLitValAbove;
    }
    
    void RSGISClumpsMeanLitStatsWithinAtt::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            
            try
            {   
                RSGISFeature *feat = attTable->getFeature(clumpIdx);
                double threshold = 0;
                if(this->meanLitFieldDT == rsgis_int)
                {
                    threshold = feat->intFields->at(this->meanLitFieldIdx);
                }
                else if(this->meanLitFieldDT == rsgis_float)
                {
                    threshold = feat->floatFields->at(this->meanLitFieldIdx);
                }
                else
                {
                    RSGISImageCalcException("Mean-lit threshold field must be of type int or float.");
                }
                
                bool withinThreshold = false;
                
                if((this->useMeanLitValAbove) & (bandValues[meanLitBand] > threshold))
                {
                    withinThreshold = true;
                }
                else if((!this->useMeanLitValAbove) & (bandValues[meanLitBand] < threshold))
                {
                    withinThreshold = true;
                }
                
                if(withinThreshold)
                {
                    float bandVal = 0;
                    RSGISFeature *feat = attTable->getFeature(clumpIdx);
                    if(feat->boolFields->at(firstFieldIdx))
                    {
                        bool nanPresent = false;
                        for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                        {
                            if(boost::math::isnan(bandValues[(*iterBands)->band]))
                            {
                                nanPresent = true;
                            }
                        }
                        
                        if(!nanPresent)
                        {
                            feat->intFields->at(pxlCountIdx) = 1;
                            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                            {
                                bandVal = bandValues[(*iterBands)->band];
                                if(this->calcStdDev)
                                {
                                    if((*iterBands)->calcStdDev)
                                    {
                                        feat->floatFields->at((*iterBands)->stdDevIdx) = pow((feat->floatFields->at((*iterBands)->meanIdx) - bandVal),2);
                                    }
                                }
                                else
                                {
                                    if((*iterBands)->calcMin)
                                    {
                                        feat->floatFields->at((*iterBands)->minIdx) = bandVal;
                                    }
                                    if((*iterBands)->calcMax)
                                    {
                                        feat->floatFields->at((*iterBands)->maxIdx) = bandVal;
                                    }
                                    if((*iterBands)->calcMean)
                                    {
                                        feat->floatFields->at((*iterBands)->meanIdx) = bandVal;
                                    }
                                    if((*iterBands)->calcSum)
                                    {
                                        feat->floatFields->at((*iterBands)->sumIdx) = bandVal;
                                    }
                                }
                            }
                            
                            feat->boolFields->at(firstFieldIdx) = false;
                        }
                    }
                    else
                    {
                        bool nanPresent = false;
                        for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                        {
                            if(boost::math::isnan(bandValues[(*iterBands)->band]))
                            {
                                nanPresent = true;
                            }
                        }
                        
                        if(!nanPresent)
                        {
                            feat->intFields->at(pxlCountIdx) += 1;
                            
                            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                            {
                                bandVal = bandValues[(*iterBands)->band];
                                if(this->calcStdDev)
                                {
                                    if((*iterBands)->calcStdDev)
                                    {
                                        feat->floatFields->at((*iterBands)->stdDevIdx) += pow((feat->floatFields->at((*iterBands)->meanIdx) - bandVal),2);
                                    }
                                }
                                else
                                {
                                    if((*iterBands)->calcMin)
                                    {
                                        if(bandVal < feat->floatFields->at((*iterBands)->minIdx))
                                        {
                                            feat->floatFields->at((*iterBands)->minIdx) = bandVal;
                                        }
                                    }
                                    if((*iterBands)->calcMax)
                                    {
                                        if(bandVal > feat->floatFields->at((*iterBands)->maxIdx))
                                        {
                                            feat->floatFields->at((*iterBands)->maxIdx) = bandVal;
                                        }
                                    }
                                    if((*iterBands)->calcMean)
                                    {
                                        feat->floatFields->at((*iterBands)->meanIdx) += bandVal;
                                    }
                                    if((*iterBands)->calcSum)
                                    {
                                        feat->floatFields->at((*iterBands)->sumIdx) += bandVal;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            catch(RSGISAttributeTableException &e)
            {
                cout << "clumpIdx = " << clumpIdx << endl;
                throw RSGISImageCalcException(e.what());
            }
        } 
    }
    
    RSGISClumpsMeanLitStatsWithinAtt::~RSGISClumpsMeanLitStatsWithinAtt()
    {
        
    }
    
    
    RSGISPopulateAttributeTableImageStats::RSGISPopulateAttributeTableImageStats()
    {
        
    }
    
    void RSGISPopulateAttributeTableImageStats::populateWithImageStatisticsInMem(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, RSGISBandAttStats *imageStats) throw(RSGISImageCalcException, RSGISAttributeTableException)
    {
        if(numDatasets != 2)
        {
            throw RSGISImageCalcException("Two datasets are required for this fucntion.");
        }
        
        try 
        {
            if(datasets[0]->GetRasterCount() != 1)
            {
                throw RSGISImageCalcException("Clumps image should only have 1 image band.");
            }
            
            unsigned int numImageBands = datasets[1]->GetRasterCount();
                        
            bool usePxlCount = true;
            if(!attTable->hasAttribute("pxlcount"))
            {
                usePxlCount = false;
            }
            else if(attTable->getDataType("pxlcount") != rsgis_int)
            {
                usePxlCount = false;
            }
            unsigned int pxlCountIdx = attTable->getFieldIndex("pxlcount");
            
            if(!usePxlCount)
            {
                cerr << "Warning: \'pxlcount\' field is not available\n";
            }
            
            if(imageStats->calcMin)
            {
                if(attTable->hasAttribute(imageStats->minField))
                {
                    if(attTable->getDataType(imageStats->minField) != rsgis_float)
                    {
                        throw RSGISAttributeTableException("All outputs must be of type float.");
                    }
                    cerr << "Warning. Reusing field \'" << imageStats->minField << "\'\n";
                }
                else
                {
                    attTable->addAttFloatField(imageStats->minField, 0);
                }
                imageStats->minIdx = attTable->getFieldIndex(imageStats->minField);
            }
            
            if(imageStats->calcMax)
            {
                if(attTable->hasAttribute(imageStats->maxField))
                {
                    if(attTable->getDataType(imageStats->maxField) != rsgis_float)
                    {
                        throw RSGISAttributeTableException("All outputs must be of type float.");
                    }
                    cerr << "Warning. Reusing field \'" << imageStats->maxField << "\'\n";
                }
                else
                {
                    attTable->addAttFloatField(imageStats->maxField, 0);
                }
                imageStats->maxIdx = attTable->getFieldIndex(imageStats->maxField);
            }
            
            if(imageStats->calcMean)
            {
                if(attTable->hasAttribute(imageStats->meanField))
                {
                    if(attTable->getDataType(imageStats->meanField) != rsgis_float)
                    {
                        throw RSGISAttributeTableException("All outputs must be of type float.");
                    }
                    cerr << "Warning. Reusing field \'" << imageStats->meanField << "\'\n";
                }
                else
                {
                    attTable->addAttFloatField(imageStats->meanField, 0);
                }
                imageStats->meanIdx = attTable->getFieldIndex(imageStats->meanField);
            }
            
            if(imageStats->calcSum)
            {
                if(attTable->hasAttribute(imageStats->sumField))
                {
                    if(attTable->getDataType(imageStats->sumField) != rsgis_float)
                    {
                        throw RSGISAttributeTableException("All outputs must be of type float.");
                    }
                    cerr << "Warning. Reusing field \'" << imageStats->sumField << "\'\n";
                }
                else
                {
                    attTable->addAttFloatField(imageStats->sumField, 0);
                }
                imageStats->sumIdx = attTable->getFieldIndex(imageStats->sumField);
            }
            
            if(imageStats->calcStdDev)
            {
                if(attTable->hasAttribute(imageStats->stdDevField))
                {
                    if(attTable->getDataType(imageStats->stdDevField) != rsgis_float)
                    {
                        throw RSGISAttributeTableException("All outputs must be of type float.");
                    }
                    cerr << "Warning. Reusing field \'" << imageStats->stdDevField << "\'\n";
                }
                else
                {
                    attTable->addAttFloatField(imageStats->stdDevField, 0);
                }
                imageStats->stdDevIdx = attTable->getFieldIndex(imageStats->stdDevField);
            }
            
            if(imageStats->calcMedian)
            {
                if(attTable->hasAttribute(imageStats->medianField))
                {
                    if(attTable->getDataType(imageStats->medianField) != rsgis_float)
                    {
                        throw RSGISAttributeTableException("All outputs must be of type float.");
                    }
                    cerr << "Warning. Reusing field \'" << imageStats->medianField << "\'\n";
                }
                else
                {
                    attTable->addAttFloatField(imageStats->medianField, 0);
                }
                imageStats->medianIdx = attTable->getFieldIndex(imageStats->medianField);
            } 
            
            RSGISFeature *feat;
            vector<double> **clumpData = new vector<double>*[attTable->getSize()];
            
            for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
            {
                feat = attTable->getFeature(i);
                clumpData[i] = new vector<double>();
                if(usePxlCount)
                {
                    clumpData[i]->reserve(feat->intFields->at(pxlCountIdx)*numImageBands);
                }
            }
            
            // Extract Data from Image.
            RSGISGetAllBandPixelValuesForClumps *getImageVals = new RSGISGetAllBandPixelValuesForClumps(clumpData);
            RSGISCalcImage calcImage(getImageVals);
            calcImage.calcImage(datasets, numDatasets);
            delete getImageVals;
            
            double min = 0;
            double max = 0;
            double mean = 0;
            double stddev = 0;
            double median = 0;
            double sum = 0;
            
            int feedback = attTable->getSize()/10;
			int feedbackCounter = 0;
			cout << "Started" << flush;
            // Calculate Statistics for each feature.
            for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
            {
                if((i % feedback) == 0)
				{
					cout << ".." << feedbackCounter << ".." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
                
                feat = attTable->getFeature(i);
                
                if(imageStats->calcMin & imageStats->calcMax)
                {
                    gsl_stats_minmax (&min, &max, &(*clumpData[i])[0], 1, clumpData[i]->size());
                    feat->floatFields->at(imageStats->minIdx) = min;
                    feat->floatFields->at(imageStats->maxIdx) = max;
                }
                else if(imageStats->calcMin)
                {
                    min = gsl_stats_min(&(*clumpData[i])[0], 1, clumpData[i]->size());
                    feat->floatFields->at(imageStats->minIdx) = min;
                }
                else if(imageStats->calcMax)
                {
                    max = gsl_stats_max(&(*clumpData[i])[0], 1, clumpData[i]->size());
                    feat->floatFields->at(imageStats->maxIdx) = max;
                }
                
                if(imageStats->calcMean & imageStats->calcStdDev)
                {
                    mean = gsl_stats_mean (&(*clumpData[i])[0], 1, clumpData[i]->size());
                    stddev = gsl_stats_sd_m (&(*clumpData[i])[0], 1, clumpData[i]->size(), mean);
                    feat->floatFields->at(imageStats->meanIdx) = mean;
                    feat->floatFields->at(imageStats->stdDevIdx) = stddev;
                }
                else if(imageStats->calcMean & !imageStats->calcStdDev)
                {
                    mean = gsl_stats_mean (&(*clumpData[i])[0], 1, clumpData[i]->size());
                    feat->floatFields->at(imageStats->meanIdx) = mean;
                }
                else if(!imageStats->calcMean & imageStats->calcStdDev)
                {
                    stddev = gsl_stats_sd (&(*clumpData[i])[0], 1, clumpData[i]->size());
                    feat->floatFields->at(imageStats->stdDevIdx) = stddev;
                }
                
                if(imageStats->calcSum)
                {
                    sum = 0;
                    for(vector<double>::iterator iterVals = clumpData[i]->begin(); iterVals != clumpData[i]->end(); ++iterVals)
                    {
                        sum += *iterVals;
                    }
                    feat->floatFields->at(imageStats->sumIdx) = sum;
                }
                
                if(imageStats->calcMedian)
                {                        
                    sort(clumpData[i]->begin(), clumpData[i]->end());
                    median = gsl_stats_median_from_sorted_data(&(*clumpData[i])[0], 1, clumpData[i]->size());
                    feat->floatFields->at(imageStats->medianIdx) = median;
                }
                
            }
            cout << " Complete.\n";
            
            for(unsigned int n = 0; n < attTable->getSize(); ++n)
            {
                delete clumpData[n];
            }
            delete[] clumpData;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
        
    }
    
    RSGISPopulateAttributeTableImageStats::~RSGISPopulateAttributeTableImageStats()
    {
        
    }
    
    
    RSGISGetAllBandPixelValuesForClumps::RSGISGetAllBandPixelValuesForClumps(vector<double> **clumpData):RSGISCalcImageValue(0)
    {
        this->clumpData = clumpData;
    }
    
    void RSGISGetAllBandPixelValuesForClumps::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            
            try
            {    
                bool nanPresent = false;
                for(int i = 1; i < numBands; ++i)
                { 
                    if(boost::math::isnan(bandValues[i]))
                    {
                        nanPresent = true;
                    }
                }
                
                if(!nanPresent)
                {
                    for(int i = 1; i < numBands; ++i)
                    {                   
                        clumpData[clumpIdx]->push_back(bandValues[i]);
                    }
                }
                
            }
            catch(RSGISAttributeTableException &e)
            {
                cout << "clumpIdx = " << clumpIdx << endl;
                throw RSGISImageCalcException(e.what());
            }
        } 
    }
    
    RSGISGetAllBandPixelValuesForClumps::~RSGISGetAllBandPixelValuesForClumps()
    {
        
    }
    
    
    
    
    
    
    
   
    RSGISCalcAttTableWithinSegmentPixelDistStats::RSGISCalcAttTableWithinSegmentPixelDistStats()
    {
        
    }
    
    void RSGISCalcAttTableWithinSegmentPixelDistStats::populateWithImageStatistics(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttName*> *bands, RSGISBandAttStats *imageStats) throw(RSGISImageCalcException, RSGISAttributeTableException)
    {
        if(numDatasets != 2)
        {
            throw RSGISImageCalcException("Two datasets are required for this fucntion.");
        }
        
        try 
        {
            if(datasets[0]->GetRasterCount() != 1)
            {
                throw RSGISImageCalcException("Clumps image should only have 1 image band.");
            }
            
            unsigned int numImageBands = datasets[1]->GetRasterCount();
            
            bool usePxlCount = true;
            if(!attTable->hasAttribute("pxlcount"))
            {
                usePxlCount = false;
            }
            else if(attTable->getDataType("pxlcount") != rsgis_int)
            {
                usePxlCount = false;
            }
            unsigned int pxlCountIdx = attTable->getFieldIndex("pxlcount");
            
            if(!usePxlCount)
            {
                cerr << "Warning: \'pxlcount\' field is not available\n";
            }
            
            
            for(vector<RSGISBandAttName*>::iterator iterBands = bands->begin(); iterBands != bands->end(); ++iterBands)
            {
                if(!attTable->hasAttribute((*iterBands)->attName))
                {
                    string message = (*iterBands)->attName + string(" does not exist in attribute table.");
                    throw RSGISAttributeTableException(message);
                }
                else
                {
                    (*iterBands)->fieldIdx = attTable->getFieldIndex((*iterBands)->attName);
                    (*iterBands)->fieldDT = attTable->getDataType((*iterBands)->attName);
                }
            }
            
            if(imageStats->calcMin)
            {
                if(attTable->hasAttribute(imageStats->minField))
                {
                    if(attTable->getDataType(imageStats->minField) != rsgis_float)
                    {
                        throw RSGISAttributeTableException("All outputs must be of type float.");
                    }
                    cerr << "Warning. Reusing field \'" << imageStats->minField << "\'\n";
                }
                else
                {
                    attTable->addAttFloatField(imageStats->minField, 0);
                }
                imageStats->minIdx = attTable->getFieldIndex(imageStats->minField);
            }
            
            if(imageStats->calcMax)
            {
                if(attTable->hasAttribute(imageStats->maxField))
                {
                    if(attTable->getDataType(imageStats->maxField) != rsgis_float)
                    {
                        throw RSGISAttributeTableException("All outputs must be of type float.");
                    }
                    cerr << "Warning. Reusing field \'" << imageStats->maxField << "\'\n";
                }
                else
                {
                    attTable->addAttFloatField(imageStats->maxField, 0);
                }
                imageStats->maxIdx = attTable->getFieldIndex(imageStats->maxField);
            }
            
            if(imageStats->calcMean)
            {
                if(attTable->hasAttribute(imageStats->meanField))
                {
                    if(attTable->getDataType(imageStats->meanField) != rsgis_float)
                    {
                        throw RSGISAttributeTableException("All outputs must be of type float.");
                    }
                    cerr << "Warning. Reusing field \'" << imageStats->meanField << "\'\n";
                }
                else
                {
                    attTable->addAttFloatField(imageStats->meanField, 0);
                }
                imageStats->meanIdx = attTable->getFieldIndex(imageStats->meanField);
            }
            
            if(imageStats->calcSum)
            {
                if(attTable->hasAttribute(imageStats->sumField))
                {
                    if(attTable->getDataType(imageStats->sumField) != rsgis_float)
                    {
                        throw RSGISAttributeTableException("All outputs must be of type float.");
                    }
                    cerr << "Warning. Reusing field \'" << imageStats->sumField << "\'\n";
                }
                else
                {
                    attTable->addAttFloatField(imageStats->sumField, 0);
                }
                imageStats->sumIdx = attTable->getFieldIndex(imageStats->sumField);
            }
            
            if(imageStats->calcStdDev)
            {
                if(attTable->hasAttribute(imageStats->stdDevField))
                {
                    if(attTable->getDataType(imageStats->stdDevField) != rsgis_float)
                    {
                        throw RSGISAttributeTableException("All outputs must be of type float.");
                    }
                    cerr << "Warning. Reusing field \'" << imageStats->stdDevField << "\'\n";
                }
                else
                {
                    attTable->addAttFloatField(imageStats->stdDevField, 0);
                }
                imageStats->stdDevIdx = attTable->getFieldIndex(imageStats->stdDevField);
            }
            
            if(imageStats->calcMedian)
            {
                if(attTable->hasAttribute(imageStats->medianField))
                {
                    if(attTable->getDataType(imageStats->medianField) != rsgis_float)
                    {
                        throw RSGISAttributeTableException("All outputs must be of type float.");
                    }
                    cerr << "Warning. Reusing field \'" << imageStats->medianField << "\'\n";
                }
                else
                {
                    attTable->addAttFloatField(imageStats->medianField, 0);
                }
                imageStats->medianIdx = attTable->getFieldIndex(imageStats->medianField);
            } 
            
            RSGISFeature *feat;
            vector<double> **clumpData = new vector<double>*[attTable->getSize()];
            
            for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
            {
                feat = attTable->getFeature(i);
                clumpData[i] = new vector<double>();
                if(usePxlCount)
                {
                    clumpData[i]->reserve(feat->intFields->at(pxlCountIdx)*numImageBands);
                }
            }
            
            // Extract Data from Image.
            RSGISCalcEucDistWithSegments *getImageVals = new RSGISCalcEucDistWithSegments(attTable, clumpData, bands);
            RSGISCalcImage calcImage(getImageVals);
            calcImage.calcImage(datasets, numDatasets);
            delete getImageVals;
            
            double min = 0;
            double max = 0;
            double mean = 0;
            double stddev = 0;
            double median = 0;
            double sum = 0;
            
            int feedback = attTable->getSize()/10;
			int feedbackCounter = 0;
			cout << "Started" << flush;
            // Calculate Statistics for each feature.
            for(unsigned long long i  = 0; i < attTable->getSize(); ++i)
            {
                if((i % feedback) == 0)
				{
					cout << ".." << feedbackCounter << ".." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
                
                feat = attTable->getFeature(i);
                
                if(imageStats->calcMin & imageStats->calcMax)
                {
                    gsl_stats_minmax (&min, &max, &(*clumpData[i])[0], 1, clumpData[i]->size());
                    feat->floatFields->at(imageStats->minIdx) = min;
                    feat->floatFields->at(imageStats->maxIdx) = max;
                }
                else if(imageStats->calcMin)
                {
                    min = gsl_stats_min(&(*clumpData[i])[0], 1, clumpData[i]->size());
                    feat->floatFields->at(imageStats->minIdx) = min;
                }
                else if(imageStats->calcMax)
                {
                    max = gsl_stats_max(&(*clumpData[i])[0], 1, clumpData[i]->size());
                    feat->floatFields->at(imageStats->maxIdx) = max;
                }
                
                if(imageStats->calcMean & imageStats->calcStdDev)
                {
                    mean = gsl_stats_mean (&(*clumpData[i])[0], 1, clumpData[i]->size());
                    stddev = gsl_stats_sd_m (&(*clumpData[i])[0], 1, clumpData[i]->size(), mean);
                    feat->floatFields->at(imageStats->meanIdx) = mean;
                    feat->floatFields->at(imageStats->stdDevIdx) = stddev;
                }
                else if(imageStats->calcMean & !imageStats->calcStdDev)
                {
                    mean = gsl_stats_mean (&(*clumpData[i])[0], 1, clumpData[i]->size());
                    feat->floatFields->at(imageStats->meanIdx) = mean;
                }
                else if(!imageStats->calcMean & imageStats->calcStdDev)
                {
                    stddev = gsl_stats_sd (&(*clumpData[i])[0], 1, clumpData[i]->size());
                    feat->floatFields->at(imageStats->stdDevIdx) = stddev;
                }
                
                if(imageStats->calcSum)
                {
                    sum = 0;
                    for(vector<double>::iterator iterVals = clumpData[i]->begin(); iterVals != clumpData[i]->end(); ++iterVals)
                    {
                        sum += *iterVals;
                    }
                    feat->floatFields->at(imageStats->sumIdx) = sum;
                }
                
                if(imageStats->calcMedian)
                {                        
                    sort(clumpData[i]->begin(), clumpData[i]->end());
                    median = gsl_stats_median_from_sorted_data(&(*clumpData[i])[0], 1, clumpData[i]->size());
                    feat->floatFields->at(imageStats->medianIdx) = median;
                }
                
            }
            cout << " Complete.\n";
            
            for(unsigned int n = 0; n < attTable->getSize(); ++n)
            {
                delete clumpData[n];
            }
            delete[] clumpData;
        } 
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }

    }
    
    RSGISCalcAttTableWithinSegmentPixelDistStats::~RSGISCalcAttTableWithinSegmentPixelDistStats()
    {
        
    }
    
    

    RSGISCalcEucDistWithSegments::RSGISCalcEucDistWithSegments(RSGISAttributeTable *attTable, vector<double> **clumpData, vector<RSGISBandAttName*> *bands):RSGISCalcImageValue(0)
    {
        this->attTable = attTable;
        this->clumpData = clumpData;
        this->bands = bands;
    }

    void RSGISCalcEucDistWithSegments::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            
            try
            {   
                RSGISFeature *feat = attTable->getFeature(clumpIdx);
                
                double val = 0;
                double meanVal = 0;
                for(vector<RSGISBandAttName*>::iterator iterBands = this->bands->begin(); iterBands != this->bands->end(); ++iterBands)
                {
                    if((*iterBands)->fieldDT == rsgis_int)
                    {
                        meanVal = feat->intFields->at((*iterBands)->fieldIdx);
                    }
                    else if((*iterBands)->fieldDT == rsgis_float)
                    {
                        meanVal = feat->floatFields->at((*iterBands)->fieldIdx);
                    }
                    else
                    {
                        throw RSGISImageCalcException("Field data type not int or float.");
                    }
                    
                    val += (meanVal - bandValues[(*iterBands)->band]) * (meanVal - bandValues[(*iterBands)->band]);
                }
                
                clumpData[clumpIdx]->push_back(sqrt(val/this->bands->size()));
            }
            catch(RSGISAttributeTableException &e)
            {
                cout << "clumpIdx = " << clumpIdx << endl;
                throw RSGISImageCalcException(e.what());
            }
        }
    }

    
    RSGISCalcEucDistWithSegments::~RSGISCalcEucDistWithSegments()
    {
        
    }
    
    
    
    RSGISCalcClumpSumAndCount::RSGISCalcClumpSumAndCount(size_t *pxlCount, double **sumVals, size_t *bandIdxs, size_t numSpecBands, size_t numFeats):RSGISCalcImageValue(0)
    {
        this->pxlCount = pxlCount;
        this->sumVals = sumVals;
        this->bandIdxs = bandIdxs;
        this->numSpecBands = numSpecBands;
        this->numFeats = numFeats;
    }
    
    void RSGISCalcClumpSumAndCount::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            
            ++pxlCount[clumpIdx];
            
            for(size_t i = 0; i < numSpecBands; ++i)
            {
                sumVals[clumpIdx][i] += bandValues[bandIdxs[i]];
            }
        }
    }
    
    RSGISCalcClumpSumAndCount::~RSGISCalcClumpSumAndCount()
    {
        
    }
    
    
    
    
    
    
}}