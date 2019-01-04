#!/usr/bin/env python

#############################################
# rsgisimginfo.py
#
#  Copyright 2019 RSGISLib.
#
#  RSGISLib: 'The remote sensing and GIS Software Library'
#
#  RSGISLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  RSGISLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
#
# Purpose:  Get information on vector files/layers. 
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 02/01/2019
# Version: 1.0
# 
#############################################

import argparse
import osgeo.gdal as gdal
from rios import rat

def listRATColumns(imgFile, band=1):
    dataset = gdal.Open(imgFile, gdal.GA_ReadOnly)
    nBands = dataset.RasterCount
    if (band == 0) or (band > nBands):
        raise Exception("Specified band is within the image.")
    bandObj = dataset.GetRasterBand(band)
    colNames = rat.getColumnNamesFromBand(bandObj)
    i = 1
    for col in colNames:
        print("{0}: {1}".format(i, col))
        i = i + 1

if __name__ == "__main__":

    # Read in config file
    parser = argparse.ArgumentParser(description="Obtain image info.")
    
    parser.add_argument("--ratcols", 
                        action='store_true', 
                        default=False, 
                        help="List the columns in a RAT.")
    parser.add_argument("-i", "--imgfile",
                        type=str,
                        required=True,
                        help="image file")
    parser.add_argument("-b", "--imgband",
                        type=int,
                        default=1,
                        help="image band")
    args = parser.parse_args()
    
    if args.ratcols:
        listRATColumns(args.imgfile, args.imgband)

