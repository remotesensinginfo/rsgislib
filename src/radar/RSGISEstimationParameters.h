/*
 *  RSGISEstimationParameters.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 04/03/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
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

#ifndef RSGISEstimationParameters_H
#define RSGISEstimationParameters_H

namespace rsgis
{
	namespace radar
	{	
		enum estParameters
		{
			/// Available parameters for the estimation algorithm
			cDepthDensity,
			cDepthDensityReturnPredictSigma,
			diameterDensity,
			diameterDensityReturnPredictSigma,
			dielectricDensityHeight,
			dielectricDensityHeightPredictSigma,
			heightDensity,
			heightDensityReturnPredictSigma,
			general = 100,
			general2Par,
			general2ParReturnPredictSigma,
			general3Par,
			general3ParReturnPredictSigma
			
		};
	}
}

#endif
