#!/usr/bin/env python

#############################################
# rgsisvectools.py
#
#  Copyright 2018 RSGISLib.
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
# Date: 17/12/2018
# Version: 1.0
# 
#############################################

import rsgislib.vectorutils

import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--vecfile", type=str, required=True, help="Input vector file.")
    parser.add_argument("--lyrs", action='store_true', default=False, help="List the layers in vector file.")
    args = parser.parse_args()
    
    if args.lyrs:
        veclyrs = rsgislib.vectorutils.getVecLyrsLst(args.vecfile)
        i = 1
        for lyr in veclyrs:
            print("{0}: {1}".format(i, lyr))
            i = i + 1
