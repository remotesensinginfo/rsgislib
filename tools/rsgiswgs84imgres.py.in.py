#!/usr/bin/env python

#############################################
# rsgisbatchconvert2tif.py
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
# Purpose:  Calculate the image resolution in metres for an input image
#           projected in WGS84 lat / lon
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 28/04/2025
# Version: 1.0
#
#############################################

import argparse

import rsgislib.imageutils
import rsgislib.tools.projection


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-i",
        "--input",
        type=str,
        required=True,
        help="A string with the path to the input image",
    )

    args = parser.parse_args()

    img = args.input
    
    bbox = rsgislib.imageutils.get_img_bbox(img)
    
    mid_lat = bbox[2] + ((bbox[3]-bbox[2])/2)
    
    x_d_res, y_d_res = rsgislib.imageutils.get_img_res(input_img = img, abs_vals = False)
    
    x_m_res, y_m_res = rsgislib.tools.projection.degrees_to_metres(latitude = mid_lat, lon_size = x_d_res, lat_size = y_d_res)
    
    avg_res = (abs(x_m_res) + abs(y_m_res)) / 2

    print(f"Image Resolution in Metres: x={x_m_res} y={y_m_res}. Avg={avg_res}")
