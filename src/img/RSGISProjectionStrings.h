/*
 *  RSGISProjectionStrings.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 20/02/2009.
 *  Copyright 2009 RSGISLib.
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

#ifndef RSGISProjectionStrings_H
#define RSGISProjectionStrings_H

namespace rsgis{namespace img{
	
	const std::string OSGB_Proj = "PROJCS[\"unnamed\",GEOGCS[\"OSGB 1936\",DATUM[\"OSGB_1936\",SPHEROID[\"Airy 1830\",6377563.4,299.3247788233662,AUTHORITY[\"EPSG\",\"7001\"]],AUTHORITY[\"EPSG\",\"6277\"]],PRIMEM[\"Greenwich\",0],UNIT[\"degree\",0.0174532925199433],AUTHORITY[\"EPSG\",\"4277\"]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",49],PARAMETER[\"central_meridian\",-2],PARAMETER[\"scale_factor\",0.999601],PARAMETER[\"false_easting\",400000],PARAMETER[\"false_northing\",-100000],UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]]]";
	const std::string NZ2000_Proj = "PROJCS[\"NZGD2000 / New Zealand Transverse Mercator 2000\",GEOGCS[\"NZGD2000\",DATUM[\"New Zealand Geodetic Datum 2000\",SPHEROID[\"GRS 1980\",6378137,298.257222101,AUTHORITY[\"EPSG\",\"7019\"]],AUTHORITY[\"EPSG\",\"6167\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",3.14159265358979,AUTHORITY[\"EPSG\",\"9001\"]],AUTHORITY[\"EPSG\",\"4167\"]],UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]],PROJECTION[\"Transverse Mercator\"],PARAMETER[\"Latitude of natural origin\", \"0\"],PARAMETER[\"Longitude of natural origin\", \"173\"],PARAMETER[\"Scale factor at natural origin\", \"0.9996\"],PARAMETER[\"False easting\", \"1600000\"],PARAMETER[\"False northing\", \"10000000\"],AUTHORITY[\"EPSG\",\"2193\"],AXIS[\"Easting\",east],AXIS[\"Northing\",north]]";
    const std::string NZ1949_Proj = "PROJCS[\"NZGD49 / New Zealand Map Grid\",GEOGCS[\"NZGD49\",DATUM[\"New Zealand Geodetic Datum 1949\",SPHEROID[\"International 1924\",6378388,297,AUTHORITY[\"EPSG\",\"7022\"]],AUTHORITY[\"EPSG\",\"6272\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",3.14159265358979,AUTHORITY[\"EPSG\",\"9001\"]],AUTHORITY[\"EPSG\",\"4272\"]],UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]],PROJECTION[\"New Zealand Map Grid\"],PARAMETER[\"Latitude of natural origin\", \"-41\"],PARAMETER[\"Longitude of natural origin\", \"173\"],PARAMETER[\"False easting\", \"2510000\"],PARAMETER[\"False northing\", \"6023150\"],AUTHORITY[\"EPSG\",\"27200\"],AXIS[\"Easting\",east],AXIS[\"Northing\",north]]";
}}

#endif
