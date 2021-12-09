#!/usr/bin/env python

#############################################
# rsgistranslate2tif.py
#
#  Copyright 2020 RSGISLib.
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
# Purpose:  Using gdal_translate converts a single input image to GEOTIFF
#           tiling, compressing and copying any image overviews.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 29/05/2020
# Version: 1.0
#
#############################################

import argparse
import subprocess

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", type=str, required=True, help="The input image file")
    parser.add_argument("-o", "--output", type=str, required=True, help="The output image (GTIFF) file.")
    args = parser.parse_args()

    cmd = "gdal_translate -of GTIFF -co TILED=YES -co COMPRESS=LZW -co " \
          "BIGTIFF=IF_SAFER -co COPY_SRC_OVERVIEWS=YES  {} {}".format(args.input, args.output)
    print("Running: '{}'".format(cmd))
    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
        raise Exception('Could not execute command: {}'.format(cmd))

