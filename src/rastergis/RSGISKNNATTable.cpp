/*
 *  RSGISKNNATTable.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/06/2012.
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

#include "RSGISKNNATTable.h"

namespace rsgis{namespace rastergis{
    
    
    RSGISKNNATTable::RSGISKNNATTable()
    {
        this->initialised = false;
        this->mahDistInit = false;
    }
        
    void RSGISKNNATTable::initKNN(RSGISAttributeTable *attTable, std::string trainField, std::string classField, bool limitToClass, int classVal, unsigned int k, float distThreshold, rsgis::math::rsgisdistmetrics distMetric, std::vector<std::string> *attributeNames)throw(RSGISAttributeTableException)
    {
        try
        {
            this->attTable = attTable;
            this->trainField = trainField;
            this->trainFieldIdx = 0;
            this->trainFieldDT = rsgis_bool;
            this->limitToClass = limitToClass;
            this->classField = classField;
            this->classVal = classVal;
            if(this->limitToClass)
            {
                this->classFieldIdx = 0;
                this->classFieldDT = rsgis_int;
            }
            this->k = k;
            this->distThreshold = distThreshold;
            this->distMetric = distMetric;
            
            if(this->k == 0)
            {
                throw RSGISAttributeTableException("K must be greater than 0.");
            }
            
            if((attributeNames == NULL) || (attributeNames->size() == 0))
            {
                throw RSGISAttributeTableException("At least 1 attribute needs to be provided.");
            }
            
            if(!this->attTable->hasAttribute(this->trainField))
            {
                std::string message = std::string("The field specified for training \'") + this->trainField + std::string("\' does not exist.");
                throw RSGISAttributeTableException(message);
            }
            else if(this->attTable->getDataType(this->trainField) != rsgis_bool)
            {
                std::string message = std::string("Cannot proceed as \'") + trainField + std::string("\' field is not of type boolean.");
                throw RSGISAttributeTableException(message);
            }
            this->trainFieldIdx = this->attTable->getFieldIndex(this->trainField);
            std::cout << "Training field index = " << this->trainFieldIdx << std::endl;
            
            RSGISAttribute *attribute = NULL;
            for(std::vector<std::string>::iterator iterNames = attributeNames->begin(); iterNames != attributeNames->end(); ++iterNames)
            {
                if(!this->attTable->hasAttribute(*iterNames))
                {
                    std::string message = std::string("The field \'") + *iterNames + std::string("\' does not exist.");
                    throw RSGISAttributeTableException(message);
                }
                else if(this->attTable->getDataType(*iterNames) == rsgis_float)
                {
                    attribute = new RSGISAttribute();
                    attribute->name = *iterNames;
                    attribute->dataType = rsgis_float;
                    attribute->idx = this->attTable->getFieldIndex(*iterNames);
                    this->attributes.push_back(attribute);
                }
                else if(this->attTable->getDataType(*iterNames) == rsgis_int)
                {
                    attribute = new RSGISAttribute();
                    attribute->name = *iterNames;
                    attribute->dataType = rsgis_int;
                    attribute->idx = this->attTable->getFieldIndex(*iterNames);
                    this->attributes.push_back(attribute);
                }
                else
                {
                    std::string message = std::string("The data type for \'") + *iterNames + std::string("\' is not float or int.");
                    throw RSGISAttributeTableException(message);
                }
            }
            
            std::cout << "All fields has present and checked\n";
            
            // Build training data.
            std::cout << "Get training data\n";
            knownData = new std::vector< std::vector<double>* >();
            
            unsigned int idx = 0;
            unsigned int rowCount = 0;
            unsigned long feedback = attTable->getSize()/10;
            int feedbackCounter = 0;
            std::cout << "Started" << std::flush;
            for(this->attTable->start(); this->attTable->end(); ++(*this->attTable))
            {
                if((rowCount % feedback) == 0)
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                if((*(*this->attTable))->boolFields->at(this->trainFieldIdx))
                {
                    knownData->push_back(new std::vector<double>());
                    knownData->at(idx)->reserve(attributes.size());
                    for(std::vector<RSGISAttribute*>::iterator iterAtt = attributes.begin(); iterAtt != attributes.end(); ++iterAtt)
                    {
                        if((*iterAtt)->dataType == rsgis_float)
                        {
                            knownData->at(idx)->push_back((*(*this->attTable))->floatFields->at((*iterAtt)->idx));
                        }
                        else if((*iterAtt)->dataType == rsgis_int)
                        {
                            knownData->at(idx)->push_back((*(*this->attTable))->intFields->at((*iterAtt)->idx));
                        }
                    }
                    ++idx;
                }
                ++rowCount;
            }
            std::cout << " Complete.\n";
            
            if(this->distMetric == rsgis::math::rsgis_mahalanobis)
            {
                std::cout << "Initilising Covariance Matrix for Mahalanobis distance - may take a little while\n";
                rsgis::math::RSGISVectors vecUtils;
                rsgis::math::RSGISMatrices matrixUtils;
                rsgis::math::Vector *varMeans = this->calcVariableMeans();
                rsgis::math::Matrix *covarianceMatrix = this->calcCovarianceMatrix(varMeans);
                
                size_t numVals = covarianceMatrix->m;
                
                //this->variableMeans = vecUtils.convertRSGIS2GSLVector(varMeans);
                gsl_matrix *coVarGSL = matrixUtils.convertRSGIS2GSLMatrix(covarianceMatrix);
                
                this->invCovarianceMatrix = gsl_matrix_alloc(covarianceMatrix->m, covarianceMatrix->n);
                gsl_permutation *p = gsl_permutation_alloc(covarianceMatrix->m);
                int signum = 0;
                gsl_linalg_LU_decomp(coVarGSL, p, &signum);
                gsl_linalg_LU_invert (coVarGSL, p, this->invCovarianceMatrix);
                gsl_permutation_free(p);
                gsl_matrix_free(coVarGSL);
                
                //std::cout << "signum = " << signum << std::endl;
                
                vecUtils.freeVector(varMeans);
                matrixUtils.freeMatrix(covarianceMatrix);
                
                std::cout << "Inverse Covariance Matrix:\n";
                matrixUtils.printGSLMatrix(this->invCovarianceMatrix);
                
                dVals = gsl_vector_alloc(numVals);
                outVec = gsl_vector_alloc(numVals);
                
                this->mahDistInit = true;
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(rsgis::math::RSGISMathException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        this->initialised = true;
    }
    
    double RSGISKNNATTable::calcDist(rsgis::math::rsgisdistmetrics distMetric, std::vector<double> *vals1, std::vector<double> *vals2) throw(rsgis::math::RSGISMathException)
    {
        double dist = std::numeric_limits<double>::signaling_NaN();
        try 
        {
            if(vals1->size() != vals2->size())
            {
                throw rsgis::math::RSGISMathException("Known data and value arrays need to be of the same length.");
            }
            
            if(distMetric == rsgis::math::rsgis_euclidean)
            {
                double sum = 0;
                size_t numVals = vals1->size();
                for(size_t i = 0; i < numVals; ++i)
                {
                    sum += pow((vals1->at(i) - vals2->at(i)), 2.0);
                }
                dist = sqrt(sum/((double)numVals));
            }
            else if(distMetric == rsgis::math::rsgis_manhatten)
            {
                double sum = 0;
                size_t numVals = vals1->size();
                for(size_t i = 0; i < numVals; ++i)
                {
                    sum += abs((vals1->at(i) - vals2->at(i)));
                }
                dist = sum/((double)numVals);
            }
            else if(distMetric == rsgis::math::rsgis_mahalanobis)
            {
                if(this->mahDistInit)
                {
                    //throw rsgis::math::RSGISMathException("Mahalanobis distance metric is currently not implemented...");
                    
                    
                    //rsgis::math::RSGISVectors vecUtils;
                    size_t numVals = vals1->size();
                    
                    //gsl_rsgis::math::Vector *dVals = gsl_vector_alloc(numVals);
                    
                    for(size_t i = 0; i < numVals; ++i)
                    {
                        gsl_vector_set(dVals, i, (vals1->at(i) - vals2->at(i)));
                    }
                    //std::cout << "\nVector D:\n";
                    //vecUtils.printGSLVector(dVals);
                    
                    //gsl_rsgis::math::Vector *outVec = gsl_vector_alloc(numVals);
                    
                    gsl_blas_dgemv(CblasNoTrans, 1.0, this->invCovarianceMatrix, dVals, 0.0, outVec );
                    
                    //std::cout << "Mah Out Vec:\n";
                    //vecUtils.printGSLVector(outVec);
                    
                    dist = 0;
                    for(size_t i = 0; i < numVals; ++i)
                    {
                        dist += gsl_vector_get(dVals, i) * gsl_vector_get(outVec, i);
                    }
                    
                    dist = sqrt(dist);
                    
                    //std::cout << "mah dist = " << dist << std::endl;
                    
                    //gsl_vector_free(dVals);
                    //gsl_vector_free(outVec);
                     
                }
                else
                {
                    throw rsgis::math::RSGISMathException("Mahalanobis distance metric is not initilised...");
                }
            }
            else 
            {
                throw rsgis::math::RSGISMathException("Distance metric is unknown.");
            }
        }
        catch (rsgis::math::RSGISMathException &e)
        {
            throw e;
        }
        
        return dist;
    }

    rsgis::math::Vector* RSGISKNNATTable::calcVariableMeans() throw(rsgis::math::RSGISMathException)
    {
        rsgis::math::RSGISVectors vecUtils;
        rsgis::math::Vector *means = vecUtils.createVector(this->attributes.size());
        
        unsigned int idx = 0;
        for(this->attTable->start(); this->attTable->end(); ++(*this->attTable))
        {
            idx = 0;
            for(std::vector<RSGISAttribute*>::iterator iterAtt = attributes.begin(); iterAtt != attributes.end(); ++iterAtt)
            {
                if((*iterAtt)->dataType == rsgis_float)
                {
                    means->vector[idx] += (*(*this->attTable))->floatFields->at((*iterAtt)->idx);
                }
                else if((*iterAtt)->dataType == rsgis_int)
                {
                     means->vector[idx] += (*(*this->attTable))->intFields->at((*iterAtt)->idx);
                }
                ++idx;
            }
        }
        
        for(unsigned int i = 0; i < means->n; ++i)
        {
            means->vector[i] = means->vector[i] / ((double)this->attTable->getSize());
        }
        
        std::cout << "Mean Vector:\n";
        vecUtils.printVector(means);
        
        return means;
    }
    
    rsgis::math::Matrix* RSGISKNNATTable::calcCovarianceMatrix(rsgis::math::Vector *attMeans) throw(rsgis::math::RSGISMathException)
    {
        rsgis::math::RSGISMatrices matrixUtils;
        rsgis::math::Matrix *covarMatrix = matrixUtils.createMatrix(this->attributes.size(), this->attributes.size());
        
        for(size_t i = 0; i < covarMatrix->n; ++i)
        {
            for(size_t j = 0; j < covarMatrix->m; ++j)
            {
                covarMatrix->matrix[(covarMatrix->m * i) + j] = this->calcCovariance(this->attributes.at(i), this->attributes.at(j), attMeans->vector[i], attMeans->vector[j]);
            }
        }
        
        std::cout << "Covariance Matrix:\n";
        matrixUtils.printMatrix(covarMatrix);
        
        return covarMatrix;
    }
    
    double RSGISKNNATTable::calcCovariance(RSGISAttribute *a, RSGISAttribute *b, double aMean, double bMean) throw(rsgis::math::RSGISMathException)
    {
        double covar = 0;
        double valA = 0;
        double valB = 0;
        
        //std::cout << "a = " << a->name << " - " << a->idx << " MEAN = " << aMean << std::endl;
        //std::cout << "b = " << b->name << " - " << b->idx << " MEAN = " << bMean << std::endl;
        
        for(this->attTable->start(); this->attTable->end(); ++(*this->attTable))
        {
            if(a->dataType == rsgis_float)
            {
                valA = (*(*this->attTable))->floatFields->at(a->idx);
            }
            else if(a->dataType == rsgis_int)
            {
                valA = (*(*this->attTable))->intFields->at(a->idx);
            }
            
            if(b->dataType == rsgis_float)
            {
                valB = (*(*this->attTable))->floatFields->at(b->idx);
            }
            else if(a->dataType == rsgis_int)
            {
                valB = (*(*this->attTable))->intFields->at(b->idx);
            }
            
            valA -= aMean;
            valB -= bMean;
            
            covar += (valA * valB);
        }
        
        covar = covar / ((double)this->attTable->getSize());
        
        //std::cout << "Covariance = " << covar << std::endl << std::endl;
        
        return covar;
    }
     
    
    RSGISKNNATTable::~RSGISKNNATTable()
    {
        for(std::vector<RSGISAttribute*>::iterator iterAtt = attributes.begin(); iterAtt != attributes.end(); ++iterAtt)
        {
            delete *iterAtt;
        }            
        for(std::vector< std::vector<double>* >::iterator iterData = knownData->begin(); iterData != knownData->end(); ++iterData)
        {
            delete *iterData;
        }
        delete knownData;
        
        if(this->mahDistInit)
        {
            gsl_matrix_free(this->invCovarianceMatrix);
            gsl_vector_free(this->dVals);
            gsl_vector_free(this->outVec);
            this->mahDistInit = false;
        }
    }

    RSGISKNNATTableExtrapolation::RSGISKNNATTableExtrapolation(std::string valField) : RSGISKNNATTable()
    {
        this->valField = valField;
        this->valFieldIdx = 0;
        this->valFieldDT = rsgis_float;
    }
    
    void RSGISKNNATTableExtrapolation::performKNN()throw(RSGISAttributeTableException)
    {
        try 
        {
            if(!this->initialised)
            {
                throw RSGISAttributeTableException("KNN needs to be initised being performing KNN.");
            }
            
            if(!this->attTable->hasAttribute(valField))
            {
                std::string message = std::string("The field specified for data values \'") + this->valField + std::string("\' does not exist.");
                throw RSGISAttributeTableException(message);
            }
            else if(this->attTable->getDataType(valField) != rsgis_float)
            {
                std::string message = std::string("Cannot proceed as \'") + this->valField + std::string("\' field is not of type float.");
                throw RSGISAttributeTableException(message);
            }
            this->valFieldIdx = this->attTable->getFieldIndex(valField);
            std::cout << "Value field index = " << this->valFieldIdx << std::endl;
            
            std::cout << "Get training known values\n";
            std::vector<double> *knownVals = new std::vector<double>();
            unsigned int idx = 0;
            unsigned int rowCount = 0;
            unsigned long feedback = attTable->getSize()/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
            for(this->attTable->start(); this->attTable->end(); ++(*this->attTable))
            {
                if((rowCount % feedback) == 0)
				{
					std::cout << "." << feedbackCounter << "." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
                if((*(*this->attTable))->boolFields->at(this->trainFieldIdx))
                {
                    knownVals->push_back((*(*this->attTable))->floatFields->at(this->valFieldIdx));
                    ++idx;
                }
                ++rowCount;
            }
            std::cout << " Complete.\n";
            
            
            // Apply to attribute table.
            std::cout << "Extrapolate values for unknowns\n";
            idx = 0;
            feedback = attTable->getSize()/10;
			feedbackCounter = 0;
            std::vector<double> *data = new std::vector<double>();
            data->reserve(attributes.size());
			std::cout << "Started" << std::flush;
            for(attTable->start(); attTable->end(); ++(*attTable))
            {
                if((idx % feedback) == 0)
				{
					std::cout << "." << feedbackCounter << "." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
                if(!(*(*attTable))->boolFields->at(trainFieldIdx))
                {
                    data->clear();
                    for(std::vector<RSGISAttribute*>::iterator iterAtt = attributes.begin(); iterAtt != attributes.end(); ++iterAtt)
                    {
                        if((*iterAtt)->dataType == rsgis_float)
                        {
                            data->push_back((*(*attTable))->floatFields->at((*iterAtt)->idx));
                        }
                        else if((*iterAtt)->dataType == rsgis_int)
                        {
                            data->push_back((*(*attTable))->intFields->at((*iterAtt)->idx));
                        }
                    }
                    
                    (*(*attTable))->floatFields->at(valFieldIdx) = calcNewVal(k, distThreshold, distMetric, knownVals, knownData, data);
                }
                ++idx;
            }
            std::cout << " Complete.\n";
            
            // Clean up memory
            delete data;
            delete knownVals;
            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(rsgis::math::RSGISMathException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    double RSGISKNNATTableExtrapolation::calcNewVal(unsigned int k, float distThreshold, rsgis::math::rsgisdistmetrics distMetric, std::vector<double> *knownLocalVals, std::vector< std::vector<double>* > *knownLocalData, std::vector<double> *unknownData) throw(rsgis::math::RSGISMathException)
    {
        double outVal = std::numeric_limits<double>::signaling_NaN();
        try 
        {
            if(knownLocalVals->size() != knownLocalData->size())
            {
                throw rsgis::math::RSGISMathException("Known data and value arrays need to be of the same length.");
            }
            
            std::vector< std::pair<size_t, double> > neighbours;
            double cDistThreshold = 0;
            double maxDist = 0;
            bool maxDistFirst = true;
            bool first = true;
            size_t numVals = knownLocalVals->size();
            double dist = 0;
            for(size_t i = 0; i < numVals; ++i)
            {
                dist = this->calcDist(distMetric, knownLocalData->at(i), unknownData);
                
                if(neighbours.size() < k)
                {
                    neighbours.push_back(std::pair<size_t, double>(i,dist));
                    if(first)
                    {
                        cDistThreshold = dist;
                        first = false;
                    }
                    else if(dist > cDistThreshold)
                    {
                        cDistThreshold = dist;
                    }
                }
                else if(dist < cDistThreshold)
                {
                    maxDistFirst = true;
                    maxDist = 0;
                    for(std::vector< std::pair<size_t, double> >::iterator iterNeighbours = neighbours.begin(); iterNeighbours != neighbours.end(); ++iterNeighbours)
                    {
                        if((*iterNeighbours).second == cDistThreshold)
                        {
                            (*iterNeighbours).first = i;
                            (*iterNeighbours).second = dist;
                        }
                        
                        if(maxDistFirst)
                        {
                            maxDist = (*iterNeighbours).second;
                            maxDistFirst = false;
                        }
                        else if((*iterNeighbours).second > maxDist)
                        {
                            maxDist = (*iterNeighbours).second;
                        }
                    }
                    cDistThreshold = maxDist;
                }
                //else ignore as outside of the knn
            }
            
            //std::cout << "\nNeighbours:\n";
            double sumDist = 0;
            for(std::vector< std::pair<size_t, double> >::iterator iterNeighbours = neighbours.begin(); iterNeighbours != neighbours.end(); ++iterNeighbours)
            {
                //std::cout << "\t" << (*iterNeighbours).first << " = " << (*iterNeighbours).second << ": Data = " << knownVals->at((*iterNeighbours).first) << std::endl;
                sumDist += (*iterNeighbours).second;
            }
            //std::cout << "Total Distance = " << sumDist << std::endl;
            
            outVal = 0;
            for(std::vector< std::pair<size_t, double> >::iterator iterNeighbours = neighbours.begin(); iterNeighbours != neighbours.end(); ++iterNeighbours)
            {
                outVal += knownLocalVals->at((*iterNeighbours).first) * ((*iterNeighbours).second/sumDist);
            }
            //std::cout << "Output Value = " << outVal << std::endl;
            
        }
        catch (rsgis::math::RSGISMathException &e)
        {
            throw e;
        }
        
        return outVal;
    }
    
    RSGISKNNATTableExtrapolation::~RSGISKNNATTableExtrapolation()
    {
        
    }
    
}}



