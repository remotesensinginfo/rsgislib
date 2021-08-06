/*
 *  RSGISFindImageOffset.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/07/2021.
 *  Copyright 2021 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 *
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISFindImageOffset.h"

namespace rsgis{namespace reg{

    RSGISFindImageOffset::RSGISFindImageOffset()
    {

    }

    std::pair<double, double> RSGISFindImageOffset::findImageOffset(GDALDataset *refDataset, GDALDataset *fltDataset,
                                                                    unsigned int xSearch, unsigned int ySearch,
                                                                    RSGISImageCalcSimilarityMetric *metric,
                                                                    bool calcSubPixelRes, unsigned int subPixelRes)
    {
        rsgis::img::RSGISImageUtils imgUtils;

        unsigned xSearchShifts = (xSearch * 2) + 1;
        unsigned ySearchShifts = (ySearch * 2) + 1;

        std::vector<int> x_pxl_shifts;
        for(int i = xSearch; i > 0; --i)
        {
            x_pxl_shifts.push_back(i*-1);
        }
        for(int i = 0; i < xSearch+1; ++i)
        {
            x_pxl_shifts.push_back(i);
        }

        std::vector<int> y_pxl_shifts;
        for(int i = ySearch; i > 0; --i)
        {
            y_pxl_shifts.push_back(i*-1);
        }
        for(int i = 0; i < ySearch+1; ++i)
        {
            y_pxl_shifts.push_back(i);
        }

        // Copy image to in-memory dataset
        GDALDataType fltDataType = fltDataset->GetRasterBand(1)->GetRasterDataType();
        GDALDataset *fltDatasetMemCP = imgUtils.createCopy(fltDataset, "", "MEM", fltDataType , true, "");
        if(fltDataType == GDT_Byte)
        {
            imgUtils.copyByteGDALDataset(fltDataset, fltDatasetMemCP);
        }
        else if((fltDataType == GDT_UInt16) | (fltDataType == GDT_UInt32))
        {
            imgUtils.copyUIntGDALDataset(fltDataset, fltDatasetMemCP);
        }
        else if((fltDataType == GDT_Int16) | (fltDataType == GDT_Int32))
        {
            imgUtils.copyIntGDALDataset(fltDataset, fltDatasetMemCP);
        }
        else
        {
            imgUtils.copyFloatGDALDataset(fltDataset, fltDatasetMemCP);
        }

        // Record the 0,0 TL.
        auto transform = new double[6];
        fltDataset->GetGeoTransform(transform);
        double fltTLX = transform[0];
        double fltTLY = transform[3];
        double fltPxlResX = abs(transform[1]);
        double fltPxlResY = abs(transform[5]);

        double fltShiftX = 0.0;
        double fltShiftY = 0.0;
        double fltShiftTLX = 0.0;
        double fltShiftTLY = 0.0;

        GDALDataset **datasets = new GDALDataset*[2];
        datasets[0] = refDataset;
        datasets[1] = fltDatasetMemCP;

        rsgis::img::RSGISCalcImage *imgCalc = new rsgis::img::RSGISCalcImage(metric);


        double **metricVals = new double*[ySearchShifts];
        for(unsigned int i = 0; i < ySearchShifts; ++i)
        {
            metricVals[i] = new double[xSearchShifts];
            for(unsigned int j = 0; j < xSearchShifts; ++j)
            {
                metricVals[i][j] = 0.0;
            }
        }
        double cMetricVal = 0.0;
        double opMetricVal = 0.0;
        int opXShift = 0;
        int opYShift = 0;
        unsigned int cXShiftIdx = 0;
        unsigned int cYShiftIdx = 0;
        unsigned int opXShiftIdx = 0;
        unsigned int opYShiftIdx = 0;
        bool first = true;

        for(auto iterYSize = y_pxl_shifts.begin(); iterYSize != y_pxl_shifts.end(); ++iterYSize)
        {
            cXShiftIdx = 0;
            fltShiftY = fltPxlResY * (*iterYSize);
            fltShiftTLY = fltTLY + fltShiftY;
            for(auto iterXSize = x_pxl_shifts.begin(); iterXSize != x_pxl_shifts.end(); ++iterXSize)
            {
                // Update header with new shifted location...
                fltShiftX = fltPxlResX * (*iterXSize);
                fltShiftTLX = fltTLX + fltShiftX;
                std::cout << "Test [" << (*iterXSize) << ", " << (*iterYSize) << "][" << fltShiftX << ", " << fltShiftY << "]\n";

                transform[0] = fltShiftTLX;
                transform[3] = fltShiftTLY;
                fltDatasetMemCP->SetGeoTransform(transform);
                metric->reset();
                imgCalc->calcImage(datasets, 2);
                cMetricVal = metric->metricVal();
                std::cout << "Metric: " << cMetricVal << std::endl << std::endl;
                metricVals[cYShiftIdx][cXShiftIdx] = cMetricVal;
                if(first)
                {
                    opMetricVal = cMetricVal;
                    opXShift = (*iterXSize);
                    opYShift = (*iterYSize);
                    opXShiftIdx = cXShiftIdx;
                    opYShiftIdx = cYShiftIdx;
                    first = false;
                }
                else
                {
                    if(metric->findMin() && (cMetricVal < opMetricVal))
                    {
                        opMetricVal = cMetricVal;
                        opXShift = (*iterXSize);
                        opYShift = (*iterYSize);
                        opXShiftIdx = cXShiftIdx;
                        opYShiftIdx = cYShiftIdx;
                    }
                    else if(!metric->findMin() && (cMetricVal > opMetricVal))
                    {
                        opMetricVal = cMetricVal;
                        opXShift = (*iterXSize);
                        opYShift = (*iterYSize);
                        opXShiftIdx = cXShiftIdx;
                        opYShiftIdx = cYShiftIdx;
                    }
                }
                ++cXShiftIdx;
            }
            ++cYShiftIdx;
        }
        delete[] transform;

        double outShiftX = opXShift;
        double outShiftY = opYShift;

        std::cout << "Optimal Metric: " << opMetricVal << std::endl;
        std::cout << "Pixel Shift: [" << outShiftX << ", " << outShiftY <<"]\n\n";


        if(calcSubPixelRes)
        {
            rsgis::math::RSGISPolyFit polyFit;

            gsl_matrix *inputDataMatrix = gsl_matrix_alloc(xSearchShifts,2);
            for(unsigned int i = 0; i < xSearchShifts; ++i)
            {
                gsl_matrix_set (inputDataMatrix, i, 0, x_pxl_shifts.at(i));
                gsl_matrix_set (inputDataMatrix, i, 1, metricVals[opYShiftIdx][i]);
            }

            int order = 3; // 2nd Order - starts at zero.
            if(xSearch > 1)
            {
                order = 4; // 3th Order - starts at zero.
            }
            else if(xSearch > 5)
            {
                order = 5; // 4th Order - starts at zero.
            }
            gsl_vector *coefficients = polyFit.PolyfitOneDimensionQuiet(order, inputDataMatrix);

            float subPixelXMetric = 0.0;
            double subPixelXShift = findExtreme(metric->findMin(), coefficients, order, opXShift-1, opXShift+1, subPixelRes, &subPixelXMetric);

            if(!((boost::math::isnan)(subPixelXShift)))
            {
                outShiftX = subPixelXShift;
            }

            gsl_matrix_free(inputDataMatrix);
            gsl_vector_free(coefficients);

            inputDataMatrix = gsl_matrix_alloc(ySearchShifts,2);
            for(unsigned int i = 0; i < ySearchShifts; ++i)
            {
                gsl_matrix_set (inputDataMatrix, i, 0, y_pxl_shifts.at(i));
                gsl_matrix_set (inputDataMatrix, i, 1, metricVals[i][opXShiftIdx]);
            }

            order = 3; // 2nd Order - starts at zero.
            if(ySearch > 1)
            {
                order = 4; // 3th Order - starts at zero.
            }
            else if(ySearch > 5)
            {
                order = 5; // 4th Order - starts at zero.
            }
            coefficients = polyFit.PolyfitOneDimensionQuiet(order, inputDataMatrix);

            float subPixelYMetric = 0.0;
            double subPixelYShift = findExtreme(metric->findMin(), coefficients, order, opYShift-1, opYShift+1, subPixelRes, &subPixelYMetric);
            if(!((boost::math::isnan)(subPixelYShift)))
            {
                outShiftY = subPixelYShift;
            }

            gsl_matrix_free(inputDataMatrix);
            gsl_vector_free(coefficients);
        }

        for(unsigned int i = 0; i < ySearchShifts; ++i)
        {
            delete[] metricVals[i];
        }
        delete[] metricVals;

        return std::pair<double, double>(outShiftX, outShiftY);
    }


    float RSGISFindImageOffset::findExtreme(bool findMin, gsl_vector *coefficients, unsigned int order, float minRange, float maxRange, unsigned int resolution, float *extremeVal)
    {
        double division = ((float)1)/((float)resolution);

        float range = maxRange - minRange;
        unsigned int numTests = ceil(range/division);

        double xValue = 0;
        double yPredicted = 0;

        bool first = true;
        double extremeX = 0;
        double extremeY = 0;

        // Quadratic
        if(order == 3)
        {
            // Calculate value for which first order derivative equals 0.
            extremeX = (-1.0*gsl_vector_get(coefficients,1)) / (2.0*gsl_vector_get(coefficients,2));

            // Predict corresponding y value.
            double yPredicted = 0;
            for(unsigned int j = 0; j < order; j++)
            {
                double xPow = pow(extremeX, static_cast<int>(j)); // x^n;
                double coeff = gsl_vector_get(coefficients, j); // a_n
                double coeffXPow = coeff * xPow; // a_n * x^n
                yPredicted = yPredicted + coeffXPow;
            }
            extremeY=yPredicted;
        }
        else if(order == 4)
        {
            // Find the values for which the first order derivative is equal to 0.
            double a = gsl_vector_get(coefficients,1);
            double b = 2.0*gsl_vector_get(coefficients,2);
            double c = 3.0*gsl_vector_get(coefficients,3);
            double xValue1 = ((-1.0*b)+sqrt(b*b - 4*a*c))/(2.0*a);
            double xValue2 = ((-1.0*b)-sqrt(b*b - 4*a*c))/(2.0*a);

            // Predict corresponding y value.
            double yPredicted1 = 0;
            for(unsigned int j = 0; j < order; j++)
            {
                double xPow = pow(xValue1, static_cast<int>(j)); // x^n;
                double coeff = gsl_vector_get(coefficients, j); // a_n
                double coeffXPow = coeff * xPow; // a_n * x^n
                yPredicted1 = yPredicted1 + coeffXPow;
            }

            double yPredicted2 = 0;
            for(unsigned int j = 0; j < order; j++)
            {
                double xPow = pow(xValue2, static_cast<int>(j)); // x^n;
                double coeff = gsl_vector_get(coefficients, j); // a_n
                double coeffXPow = coeff * xPow; // a_n * x^n
                yPredicted2 = yPredicted2 + coeffXPow;
            }
            if(yPredicted1 > yPredicted2)
            {
                extremeX = xValue1;
                extremeY = yPredicted1;
            }
            else
            {
                extremeX = xValue2;
                extremeY = yPredicted2;
            }

            // Check if outside range (something went wrong)
            if((extremeX < minRange) | (extremeX > maxRange))
            {
                for(unsigned int i = 0; i < numTests; ++i)
                {
                    xValue = minRange + (i*division);
                    yPredicted = 0;
                    for(unsigned int j = 0; j < order; j++)
                    {
                        double xPow = pow(xValue, static_cast<int>(j)); // x^n;
                        double coeff = gsl_vector_get(coefficients, j); // a_n
                        double coeffXPow = coeff * xPow; // a_n * x^n
                        yPredicted = yPredicted + coeffXPow;
                    }

                    if(first)
                    {
                        extremeX = xValue;
                        extremeY = yPredicted;
                        first = false;
                    }
                    else if(findMin & (yPredicted < extremeY))
                    {
                        extremeX = xValue;
                        extremeY = yPredicted;
                    }
                    else if(!findMin & (yPredicted > extremeY))
                    {
                        extremeX = xValue;
                        extremeY = yPredicted;
                    }
                }
            }
        }
        else
        {
            for(unsigned int i = 0; i < numTests; ++i)
            {
                xValue = minRange + (i*division);
                yPredicted = 0;
                for(unsigned int j = 0; j < order; j++)
                {
                    double xPow = pow(xValue, static_cast<int>(j)); // x^n;
                    double coeff = gsl_vector_get(coefficients, j); // a_n
                    double coeffXPow = coeff * xPow; // a_n * x^n
                    yPredicted = yPredicted + coeffXPow;
                }

                if(first)
                {
                    extremeX = xValue;
                    extremeY = yPredicted;
                    first = false;
                }
                else if(findMin & (yPredicted < extremeY))
                {
                    extremeX = xValue;
                    extremeY = yPredicted;
                }
                else if(!findMin & (yPredicted > extremeY))
                {
                    extremeX = xValue;
                    extremeY = yPredicted;
                }
            }
        }
        *extremeVal = extremeY;
        return extremeX;
    }

    RSGISFindImageOffset::~RSGISFindImageOffset()
    {

    }


}}

