/*
 *  RSGISStatsFilters.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/12/2008.
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

#include "RSGISStatsFilters.h"

namespace rsgis
{
    namespace filter
    {
        RSGISMeanFilter::RSGISMeanFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISMeanFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            bool use_pxl_val = true;
            double outputValue = 0;
            int numberElements = 0;
            for (int i = 0; i < numBands; i++)
            {
                outputValue = 0;
                for (int j = 0; j < this->size; j++)
                {
                    for (int k = 0; k < this->size; k++)
                    {
                        use_pxl_val = true;
                        if (this->useNoDataVal && dataBlock[i][j][k] == this->noDataVal)
                        {
                            use_pxl_val = false;
                        }
                        if (use_pxl_val)
                        {
                            outputValue = outputValue + dataBlock[i][j][k];
                            numberElements++;
                        }
                    }
                }
                if (numberElements > 0)
                {
                    output[i] = outputValue / numberElements;
                }
                else
                {
                    output[i] = this->noDataVal;
                }
            }
        }

        bool RSGISMeanFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISMeanFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISMeanFilter::~RSGISMeanFilter()
        {}

        RSGISMedianFilter::RSGISMedianFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISMedianFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            bool use_pxl_val = true;
            int numberElements = winSize * winSize;
            int median = floor(((float) numberElements) / 2.0);
            std::vector<float> sortedList;
            sortedList.reserve(numberElements);

            for (int i = 0; i < numBands; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    for (int k = 0; k < size; k++)
                    {
                        use_pxl_val = true;
                        if (this->useNoDataVal && dataBlock[i][j][k] == this->noDataVal)
                        {
                            use_pxl_val = false;
                        }
                        if (use_pxl_val)
                        {
                            sortedList.push_back(dataBlock[i][j][k]);
                        }
                    }
                }
                if (sortedList.size() > 0)
                {
                    std::sort(sortedList.begin(), sortedList.end());
                    output[i] = sortedList[median];
                    sortedList.clear();
                }
                else
                {
                    output[i] = this->noDataVal;
                }
            }
        }

        bool RSGISMedianFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISMedianFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISMedianFilter::~RSGISMedianFilter()
        {}

        RSGISModeFilter::RSGISModeFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISModeFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            bool use_pxl_val = true;

            rsgis::datastruct::SortedGenericList<float> *sortedList = new
                    rsgis::datastruct::SortedGenericList<float>();

            for (int i = 0; i < numBands; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    for (int k = 0; k < size; k++)
                    {
                        use_pxl_val = true;
                        if (this->useNoDataVal && dataBlock[i][j][k] == this->noDataVal)
                        {
                            use_pxl_val = false;
                        }
                        if (use_pxl_val)
                        {
                            sortedList->add(&dataBlock[i][j][k]);
                        }
                    }
                }
                if (sortedList->getSize() > 0)
                {
                    output[i] = *sortedList->getMostCommonValue();
                    sortedList->clearList();
                }
                else
                {
                    output[i] = this->noDataVal;
                }
            }

            delete sortedList;
        }

        bool RSGISModeFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISModeFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISModeFilter::~RSGISModeFilter()
        {}


        RSGISRangeFilter::RSGISRangeFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISRangeFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            float min = 0;
            float max = 0;
            bool first = true;
            bool use_pxl_val = true;

            for (int i = 0; i < numBands; i++)
            {
                min = 0;
                max = 0;
                first = true;
                for (int j = 0; j < this->size; j++)
                {
                    for (int k = 0; k < this->size; k++)
                    {
                        use_pxl_val = true;
                        if (this->useNoDataVal && dataBlock[i][j][k] == this->noDataVal)
                        {
                            use_pxl_val = false;
                        }
                        if (use_pxl_val)
                        {
                            if (first)
                            {
                                min = dataBlock[i][j][k];
                                max = dataBlock[i][j][k];
                                first = false;
                            }
                            else
                            {
                                if (dataBlock[i][j][k] > max)
                                {
                                    max = dataBlock[i][j][k];
                                }
                                else if (dataBlock[i][j][k] < min)
                                {
                                    min = dataBlock[i][j][k];
                                }
                            }
                        }
                    }
                }
                if (first)
                {
                    output[i] = this->noDataVal;
                }
                else
                {
                    output[i] = max - min;
                }
            }
        }

        bool RSGISRangeFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISRangeFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISRangeFilter::~RSGISRangeFilter()
        {}

        RSGISStdDevFilter::RSGISStdDevFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISStdDevFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            bool use_pxl_val = true;
            double outputValue = 0;
            double squSum = 0;
            float mean = 0;
            int numberElements = 0;
            for (int i = 0; i < numBands; i++)
            {
                outputValue = 0;
                squSum = 0;

                for (int j = 0; j < this->size; j++)
                {
                    for (int k = 0; k < this->size; k++)
                    {
                        use_pxl_val = true;
                        if (this->useNoDataVal && dataBlock[i][j][k] == this->noDataVal)
                        {
                            use_pxl_val = false;
                        }
                        if (use_pxl_val)
                        {
                            outputValue = outputValue + dataBlock[i][j][k];
                            numberElements++;
                        }
                    }
                }

                if (numberElements > 0)
                {
                    mean = outputValue / numberElements;

                    for (int j = 0; j < this->size; j++)
                    {
                        for (int k = 0; k < this->size; k++)
                        {
                            use_pxl_val = true;
                            if (this->useNoDataVal && dataBlock[i][j][k] == this->
                                noDataVal)
                            {
                                use_pxl_val = false;
                            }
                            if (use_pxl_val)
                            {
                                squSum += (
                                    (dataBlock[i][j][k] - mean) * (
                                        dataBlock[i][j][k] - mean));
                            }
                        }
                    }

                    output[i] = sqrt(squSum / numberElements);
                }
                else
                {
                    output[i] = this->noDataVal;
                }
            }
        }

        bool RSGISStdDevFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISStdDevFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISStdDevFilter::~RSGISStdDevFilter()
        {}

        RSGISCoeffOfVarFilter::RSGISCoeffOfVarFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISCoeffOfVarFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            bool use_pxl_val = true;
            double outputValue = 0;
            double squSum = 0;
            float mean = 0;
            int numberElements = 0;
            for (int i = 0; i < numBands; i++)
            {
                outputValue = 0;
                squSum = 0;

                for (int j = 0; j < this->size; j++)
                {
                    for (int k = 0; k < this->size; k++)
                    {
                        use_pxl_val = true;
                        if (this->useNoDataVal && dataBlock[i][j][k] == this->noDataVal)
                        {
                            use_pxl_val = false;
                        }
                        if (use_pxl_val)
                        {
                            outputValue = outputValue + dataBlock[i][j][k];
                            numberElements++;
                        }
                    }
                }

                if (numberElements > 0)
                {
                    mean = outputValue / numberElements;

                    for (int j = 0; j < this->size; j++)
                    {
                        for (int k = 0; k < this->size; k++)
                        {
                            use_pxl_val = true;
                            if (this->useNoDataVal && dataBlock[i][j][k] == this->
                                noDataVal)
                            {
                                use_pxl_val = false;
                            }
                            if (use_pxl_val)
                            {
                                squSum += (
                                    (dataBlock[i][j][k] - mean) * (
                                        dataBlock[i][j][k] - mean));
                            }
                        }
                    }

                    output[i] = sqrt(squSum / numberElements) / mean;
                }
                else
                {
                    output[i] = this->noDataVal;
                }
            }
        }

        bool RSGISCoeffOfVarFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISCoeffOfVarFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISCoeffOfVarFilter::~RSGISCoeffOfVarFilter()
        {}


        RSGISMinFilter::RSGISMinFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISMinFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            float min = 0;
            bool first = true;
            bool use_pxl_val = true;

            for (int i = 0; i < numBands; i++)
            {
                min = 0;
                first = true;
                for (int j = 0; j < this->size; j++)
                {
                    for (int k = 0; k < this->size; k++)
                    {
                        use_pxl_val = true;
                        if (this->useNoDataVal && dataBlock[i][j][k] == this->noDataVal)
                        {
                            use_pxl_val = false;
                        }
                        if (use_pxl_val)
                        {
                            if (first)
                            {
                                min = dataBlock[i][j][k];
                                first = false;
                            }
                            else
                            {
                                if (dataBlock[i][j][k] < min)
                                {
                                    min = dataBlock[i][j][k];
                                }
                            }
                        }
                    }
                }
                if (first)
                {
                    output[i] = this->noDataVal;
                }
                else
                {
                    output[i] = min;
                }
            }
        }

        bool RSGISMinFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISMinFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISMinFilter::~RSGISMinFilter()
        {}

        RSGISMaxFilter::RSGISMaxFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISMaxFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            float max = 0;
            bool first = true;
            bool use_pxl_val = true;

            for (int i = 0; i < numBands; i++)
            {
                max = 0;
                first = true;
                for (int j = 0; j < this->size; j++)
                {
                    for (int k = 0; k < this->size; k++)
                    {
                        use_pxl_val = true;
                        if (this->useNoDataVal && dataBlock[i][j][k] == this->noDataVal)
                        {
                            use_pxl_val = false;
                        }
                        if (use_pxl_val)
                        {
                            if (first)
                            {
                                max = dataBlock[i][j][k];
                                first = false;
                            }
                            else
                            {
                                if (dataBlock[i][j][k] > max)
                                {
                                    max = dataBlock[i][j][k];
                                }
                            }
                        }
                    }
                }
                if (first)
                {
                    output[i] = this->noDataVal;
                }
                else
                {
                    output[i] = max;
                }
            }
        }

        bool RSGISMaxFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISMaxFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISMaxFilter::~RSGISMaxFilter()
        {}

        RSGISTotalFilter::RSGISTotalFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISTotalFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            double outputValue = 0;
            bool use_pxl_val = true;
            bool found = false;

            for (int i = 0; i < numBands; i++)
            {
                outputValue = 0;
                found = false;
                for (int j = 0; j < this->size; j++)
                {
                    for (int k = 0; k < this->size; k++)
                    {
                        use_pxl_val = true;
                        if (this->useNoDataVal && dataBlock[i][j][k] == this->noDataVal)
                        {
                            use_pxl_val = false;
                        }
                        if (use_pxl_val)
                        {
                            outputValue = outputValue + dataBlock[i][j][k];
                            found = true;
                        }
                    }
                }
                if (found)
                {
                    output[i] = outputValue;
                }
                else
                {
                    output[i] = this->noDataVal;
                }
            }
        }

        bool RSGISTotalFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISTotalFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISTotalFilter::~RSGISTotalFilter()
        {}

        RSGISKuwaharaFilter::RSGISKuwaharaFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISKuwaharaFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            bool use_pxl_val = true;
            double outputValue = 0;
            int subSize = (size - 1) / 2;
            int numberElements = 0;
            double squSum = 0;

            float *variance = new float[4];
            float *mean = new float[4];

            float minValue = 0;
            int minIndex = 0;
            bool first = true;

            for (int i = 0; i < numBands; i++)
            {
                for (int n = 0; n < 4; n++)
                {
                    variance[n] = 0;
                    mean[n] = 0;
                }

                outputValue = 0;
                squSum = 0;
                numberElements = 0;
                for (int j = 0; j < subSize; j++)
                {
                    for (int k = 0; k < subSize; k++)
                    {
                        use_pxl_val = true;
                        if (this->useNoDataVal && dataBlock[i][j][k] == this->noDataVal)
                        {
                            use_pxl_val = false;
                        }
                        if (use_pxl_val)
                        {
                            outputValue += dataBlock[i][j][k];
                            numberElements++;
                        }
                    }
                }
                if (numberElements > 0)
                {
                    mean[0] = outputValue / numberElements;

                    for (int j = 0; j < subSize; j++)
                    {
                        for (int k = 0; k < subSize; k++)
                        {
                            use_pxl_val = true;
                            if (this->useNoDataVal && dataBlock[i][j][k] == this->
                                noDataVal)
                            {
                                use_pxl_val = false;
                            }
                            if (use_pxl_val)
                            {
                                squSum += (
                                    (dataBlock[i][j][k] - mean[0]) * (
                                        dataBlock[i][j][k] - mean[0]));
                            }
                        }
                    }
                    variance[0] = squSum / numberElements;


                    outputValue = 0;
                    squSum = 0;
                    numberElements = 0;
                    for (int j = (subSize + 1); j < size; j++)
                    {
                        for (int k = 0; k < subSize; k++)
                        {
                            use_pxl_val = true;
                            if (this->useNoDataVal && dataBlock[i][j][k] == this->
                                noDataVal)
                            {
                                use_pxl_val = false;
                            }
                            if (use_pxl_val)
                            {
                                outputValue += dataBlock[i][j][k];
                                numberElements++;
                            }
                        }
                    }
                    if (numberElements > 0)
                    {
                        mean[1] = outputValue / numberElements;

                        for (int j = (subSize + 1); j < size; j++)
                        {
                            for (int k = 0; k < subSize; k++)
                            {
                                use_pxl_val = true;
                                if (this->useNoDataVal && dataBlock[i][j][k] == this->
                                    noDataVal)
                                {
                                    use_pxl_val = false;
                                }
                                if (use_pxl_val)
                                {
                                    squSum += (
                                        (dataBlock[i][j][k] - mean[1]) * (
                                            dataBlock[i][j][k] - mean[1]));
                                }
                            }
                        }
                        variance[1] = squSum / numberElements;

                        outputValue = 0;
                        squSum = 0;
                        numberElements = 0;
                        for (int j = 0; j < subSize; j++)
                        {
                            for (int k = (subSize + 1); k < size; k++)
                            {
                                use_pxl_val = true;
                                if (this->useNoDataVal && dataBlock[i][j][k] == this->
                                    noDataVal)
                                {
                                    use_pxl_val = false;
                                }
                                if (use_pxl_val)
                                {
                                    outputValue += dataBlock[i][j][k];
                                    numberElements++;
                                }
                            }
                        }
                        if (numberElements > 0)
                        {
                            mean[2] = outputValue / numberElements;

                            for (int j = 0; j < subSize; j++)
                            {
                                for (int k = (subSize + 1); k < size; k++)
                                {
                                    use_pxl_val = true;
                                    if (this->useNoDataVal && dataBlock[i][j][k] == this
                                        ->noDataVal)
                                    {
                                        use_pxl_val = false;
                                    }
                                    if (use_pxl_val)
                                    {
                                        squSum += (
                                            (dataBlock[i][j][k] - mean[2]) * (
                                                dataBlock[i][j][k] - mean[2]));
                                    }
                                }
                            }
                            variance[2] = squSum / numberElements;

                            outputValue = 0;
                            squSum = 0;
                            numberElements = 0;
                            for (int j = (subSize + 1); j < size; j++)
                            {
                                for (int k = (subSize + 1); k < size; k++)
                                {
                                    use_pxl_val = true;
                                    if (this->useNoDataVal && dataBlock[i][j][k] == this
                                        ->noDataVal)
                                    {
                                        use_pxl_val = false;
                                    }
                                    if (use_pxl_val)
                                    {
                                        outputValue += dataBlock[i][j][k];
                                        numberElements++;
                                    }
                                }
                            }
                            if (numberElements > 0)
                            {
                                mean[3] = outputValue / numberElements;

                                for (int j = (subSize + 1); j < size; j++)
                                {
                                    for (int k = (subSize + 1); k < size; k++)
                                    {
                                        use_pxl_val = true;
                                        if (this->useNoDataVal && dataBlock[i][j][k] ==
                                            this->noDataVal)
                                        {
                                            use_pxl_val = false;
                                        }
                                        if (use_pxl_val)
                                        {
                                            squSum += (
                                                (dataBlock[i][j][k] - mean[3]) * (
                                                    dataBlock[i][j][k] - mean[3]));
                                        }
                                    }
                                }
                                variance[3] = squSum / numberElements;

                                minValue = 0;
                                minIndex = 0;
                                first = true;
                                for (int n = 0; n < 4; n++)
                                {
                                    if (first)
                                    {
                                        minValue = variance[n];
                                        minIndex = n;
                                        first = false;
                                    }
                                    else
                                    {
                                        if (variance[n] < minValue)
                                        {
                                            minValue = variance[n];
                                            minIndex = n;
                                        }
                                    }
                                }
                                output[i] = mean[minIndex];
                            }
                            else
                            {
                                output[i] = this->noDataVal;
                            }
                        }
                        else
                        {
                            output[i] = this->noDataVal;
                        }
                    }
                    else
                    {
                        output[i] = this->noDataVal;
                    }
                }
                else
                {
                    output[i] = this->noDataVal;
                }
            }

            delete[] variance;
            delete[] mean;
        }

        bool RSGISKuwaharaFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISKuwaharaFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISKuwaharaFilter::~RSGISKuwaharaFilter()
        {}


        RSGISMeanDiffFilter::RSGISMeanDiffFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISMeanDiffFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            bool use_pxl_val = true;
            int midPt = (size - 1) / 2;
            unsigned int nPxls = 0;
            float valPxl = 0.0;

            for (int i = 0; i < numBands; i++)
            {
                nPxls = 0;
                valPxl = dataBlock[i][midPt][midPt];
                if (this->useNoDataVal && dataBlock[i][midPt][midPt] == this->noDataVal)
                {
                    output[i] = this->noDataVal;
                    continue;
                }
                output[i] = 0.0;

                for (int j = 0; j < winSize; j++)
                {
                    for (int k = 0; k < winSize; k++)
                    {
                        if (!((j == midPt) & (k == midPt)))
                        {
                            use_pxl_val = true;
                            if (this->useNoDataVal && dataBlock[i][j][k] == this->
                                noDataVal)
                            {
                                use_pxl_val = false;
                            }
                            if (use_pxl_val)
                            {
                                output[i] += dataBlock[i][j][k] - valPxl;
                                ++nPxls;
                            }
                        }
                    }
                }

                if (nPxls > 0)
                {
                    output[i] = output[i] / nPxls;
                }
                else
                {
                    output[i] = this->noDataVal;
                }
            }
        }

        bool RSGISMeanDiffFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISMeanDiffFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISMeanDiffFilter::~RSGISMeanDiffFilter()
        {}


        RSGISMeanDiffAbsFilter::RSGISMeanDiffAbsFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISMeanDiffAbsFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            bool use_pxl_val = true;
            int midPt = (size - 1) / 2;
            unsigned int nPxls = 0;
            float valPxl = 0.0;

            for (int i = 0; i < numBands; i++)
            {
                nPxls = 0;
                valPxl = dataBlock[i][midPt][midPt];
                if (this->useNoDataVal && dataBlock[i][midPt][midPt] == this->noDataVal)
                {
                    output[i] = this->noDataVal;
                    continue;
                }
                output[i] = 0.0;

                for (int j = 0; j < winSize; j++)
                {
                    for (int k = 0; k < winSize; k++)
                    {
                        if (!((j == midPt) & (k == midPt)))
                        {
                            use_pxl_val = true;
                            if (this->useNoDataVal && dataBlock[i][j][k] == this->
                                noDataVal)
                            {
                                use_pxl_val = false;
                            }
                            if (use_pxl_val)
                            {
                                output[i] += fabs(dataBlock[i][j][k] - valPxl);
                                ++nPxls;
                            }
                        }
                    }
                }

                if (nPxls > 0)
                {
                    output[i] = output[i] / nPxls;
                }
                else
                {
                    output[i] = this->noDataVal;
                }
            }
        }

        bool RSGISMeanDiffAbsFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISMeanDiffAbsFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISMeanDiffAbsFilter::~RSGISMeanDiffAbsFilter()
        {}

        RSGISTotalDiffFilter::RSGISTotalDiffFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISTotalDiffFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            bool use_pxl_val = true;
            int midPt = (size - 1) / 2;
            float valPxl = 0.0;

            for (int i = 0; i < numBands; i++)
            {
                if (this->useNoDataVal && dataBlock[i][midPt][midPt] == this->noDataVal)
                {
                    output[i] = this->noDataVal;
                    continue;
                }
                valPxl = dataBlock[i][midPt][midPt];
                output[i] = 0.0;

                for (int j = 0; j < winSize; j++)
                {
                    for (int k = 0; k < winSize; k++)
                    {
                        if (!((j == midPt) & (k == midPt)))
                        {
                            use_pxl_val = true;
                            if (this->useNoDataVal && dataBlock[i][j][k] == this->
                                noDataVal)
                            {
                                use_pxl_val = false;
                            }
                            if (use_pxl_val)
                            {
                                output[i] += dataBlock[i][j][k] - valPxl;
                            }
                        }
                    }
                }
            }
        }

        bool RSGISTotalDiffFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISTotalDiffFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISTotalDiffFilter::~RSGISTotalDiffFilter()
        {}


        RSGISTotalDiffAbsFilter::RSGISTotalDiffAbsFilter(
            int numberOutBands, int size, std::string filenameEnding, bool useNoDataVal,
            float noDataVal
        ) : RSGISImageFilter(numberOutBands, size, filenameEnding)
        {
            this->useNoDataVal = useNoDataVal;
            this->noDataVal = noDataVal;
        }

        void RSGISTotalDiffAbsFilter::calcImageValue(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            if (this->size != winSize)
            {
                throw rsgis::img::RSGISImageCalcException("Window sizes are different");
            }

            bool use_pxl_val = true;
            int midPt = (size - 1) / 2;
            float valPxl = 0.0;

            for (int i = 0; i < numBands; i++)
            {
                if (this->useNoDataVal && dataBlock[i][midPt][midPt] == this->noDataVal)
                {
                    output[i] = this->noDataVal;
                    continue;
                }
                valPxl = dataBlock[i][midPt][midPt];
                output[i] = 0.0;

                for (int j = 0; j < winSize; j++)
                {
                    for (int k = 0; k < winSize; k++)
                    {
                        if (!((j == midPt) & (k == midPt)))
                        {
                            use_pxl_val = true;
                            if (this->useNoDataVal && dataBlock[i][j][k] == this->
                                noDataVal)
                            {
                                use_pxl_val = false;
                            }
                            if (use_pxl_val)
                            {
                                output[i] += fabs(dataBlock[i][j][k] - valPxl);
                            }
                        }
                    }
                }
            }
        }

        bool RSGISTotalDiffAbsFilter::calcImageValueCondition(
            float ***dataBlock, int numBands, int winSize, double *output
        )
        {
            throw rsgis::img::RSGISImageCalcException("Not implemented yet");
        }

        void RSGISTotalDiffAbsFilter::exportAsImage(std::string filename)
        {
            std::cout << "No Image to output\n";
        }

        RSGISTotalDiffAbsFilter::~RSGISTotalDiffAbsFilter()
        {}
    }
}
