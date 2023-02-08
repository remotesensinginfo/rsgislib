#!/usr/bin/env python

#############################################
# rsgisbatchconvert2cog.py
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
# Purpose:  Batch convert a set of images to COGs
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 8/2/2023
# Version: 1.0
#
#############################################

import argparse
import glob
import os.path
import subprocess

import rsgislib.imageutils
import rsgislib.tools.filetools
import rsgislib.tools.utils

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-i",
        "--input",
        type=str,
        required=True,
        help="A search string where for the input files (e.g., './images/*.kea'). "
        "The search string much contain at least one * wildcard.",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=str,
        required=True,
        help="Output directory where output COG (i.e., GTIFF) files will be written.",
    )
    parser.add_argument(
        "--chkimgs",
        action="store_true",
        default=False,
        help="Only convert images which are not already "
        "present in the output directory.",
    )

    args = parser.parse_args()

    imgs = glob.glob(args.input)

    for img in imgs:
        basename = rsgislib.tools.filetools.get_file_basename(img, check_valid=True)
        basename = rsgislib.tools.utils.check_str(
            basename, rm_non_ascii=True, rm_dashs=False, rm_spaces=True, rm_punc=True
        )
        over_resample = "CUBIC"
        if rsgislib.imageutils.is_img_thematic(img):
            over_resample = "NEAREST"
        out_img = os.path.join(args.output, "{}.tif".format(basename))
        if not (args.chkimgs and os.path.exists(out_img)):
            cmd = (
                f"gdal_translate -of COG -co COMPRESS=LZW "
                f"-co OVERVIEW_RESAMPLING={over_resample} "
                f"-co BIGTIFF=IF_SAFER {img} {out_img}"
            )
            print("Running: '{}'".format(cmd))
            try:
                subprocess.check_call(cmd, shell=True)
            except OSError as e:
                raise Exception("Could not execute command: {}".format(cmd))
