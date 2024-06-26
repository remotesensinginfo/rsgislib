/*
 *  RSGISGeometryUtils.cpp
 *
 *
 *  Created by Pete Bunting on 07/07/2021.
 *  Copyright 2021 RSGISLib. All rights reserved.
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

#include "RSGISGeometryUtils.h"

namespace rsgis{namespace utils{

    OGREnvelope* RSGISGeometryUtils::createOGREnvelopePointer(double minX, double maxX, double minY, double maxY)
    {
        OGREnvelope *env = new OGREnvelope();
        env->MinX = minX;
        env->MaxX = maxX;
        env->MinY = minY;
        env->MaxY = maxY;

        return env;
    }

    OGREnvelope RSGISGeometryUtils::createOGREnvelopeObject(double minX, double maxX, double minY, double maxY)
    {
        OGREnvelope env = OGREnvelope();
        env.MinX = minX;
        env.MaxX = maxX;
        env.MinY = minY;
        env.MaxY = maxY;
        return env;
    }


}}