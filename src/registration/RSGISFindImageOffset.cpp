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
                                                                    std::vector<unsigned int> refImageBands,
                                                                    std::vector<unsigned int> fltImageBands,
                                                                    unsigned int xSearch, unsigned int ySearch,
                                                                    RSGISImageSimilarityMetric *metric,
                                                                    bool calcSubPixelRes, unsigned int subPixelRes)
    {
        std::cout << "HERE! RSGISFindImageOffset::findImageOffset\n";

        std::cout << "Search [" << xSearch << ", " << ySearch << "]\n";
        if(refImageBands.size() != refImageBands.size())
        {
            throw rsgis::RSGISException("The number of bands specified from the reference and floating images must be the same.");
        }
        unsigned int nBands = refImageBands.size();
        std::cout << "N Bands: " << nBands << std::endl;



        std::pair<double, double> offsets(0.0, 0.0);
        return offsets;
    }

    RSGISFindImageOffset::~RSGISFindImageOffset()
    {

    }


}}

