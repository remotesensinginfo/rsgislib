#!/usr/bin/env python

#############################################
# rsgischkgdalfile.py
#
#  Copyright 2021 RSGISLib.
#
#  RSGISLib: 'The Remote Sensing and GIS Software Library'
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
# Purpose:  Check if a gdal file (raster or vector) is valid.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 06/05/2021
# Version: 1.0
# 
#############################################

import rsgislib
import os
import argparse
import glob

def _run_img_chk(img_params):
    img = img_params[0]
    nbands = img_params[1]
    rmerr = img_params[2]
    printnames = img_params[3]
    printerrs = img_params[4]
    chk_proj = img_params[5]
    epsg_code = img_params[6]
    read_img = img_params[7]

    rsgis_utils = rsgislib.RSGISPyUtils()

    if printnames:
        print(img)
    try:
        file_ok, err_str = rsgis_utils.check_gdal_image_file(img, check_bands=True, nbands=nbands, chk_proj=chk_proj,
                                                             epsg_code=epsg_code, read_img=read_img)
        if printerrs and (not file_ok):
            print("Error: '{}'".format(err_str))
        if not file_ok:
            if rmerr:
                os.remove(img)
                print("Removed {}".format(img))
            else:
                print("rm {}".format(img))
    except:
        if rmerr:
            os.remove(img)
            print("Removed {}".format(img))
        else:
            print("rm {}".format(img))


def _run_vec_chk(img_params):
    vec_file = img_params[0]
    rmerr = img_params[1]
    printnames = img_params[2]
    printerrs = img_params[3]

    rsgis_utils = rsgislib.RSGISPyUtils()

    if printnames:
        print(vec_file)
    try:
        file_ok, err_str = rsgis_utils.check_gdal_vector_file(vec_file)
        if printerrs and (not file_ok):
            print("Error: '{}'".format(err_str))
        if not file_ok:
            if rmerr:
                os.remove(vec_file)
                print("Removed {}".format(vec_file))
            else:
                print("rm {}".format(vec_file))
    except:
        if rmerr:
            os.remove(vec_file)
            print("Removed {}".format(vec_file))
        else:
            print("rm {}".format(vec_file))



if __name__ == "__main__":
    parser = argparse.ArgumentParser( description="A utility which can be used to check whether a GDAL "
                                                  "compatible file is valid and if there are any errors or warnings.")
    parser.add_argument("-i", "--input", type=str, required=True, help="Input file path")
    parser.add_argument("--vec", action='store_true', default=False, help="Specify that the input file is a "
                                                                          "vector layer (otherwise assumed "
                                                                          "to be a raster).")
    parser.add_argument("--rmerr", action='store_true', default=False, help="Delete error files from system.")
    parser.add_argument("--printnames", action='store_true', default=False, help="Print file names as checking")
    parser.add_argument("--printerrs", action='store_true', default=False,
                        help="Print the error messages for the images")
    parser.add_argument("--nbands", type=int, default=0,
                        help="Check the number of bands is correct. Ignored if 0; Default.")
    parser.add_argument("--epsg", type=int, default=0, help="The EPSG code for the projection of the images.")
    parser.add_argument("--chkproj", action='store_true', default=False, help="Check that a projection is defined")
    parser.add_argument("--readimg", action='store_true', default=False, help="Check the image by reading part of it.")

    args = parser.parse_args()
    print(args.input)

    chk_projection = args.chkproj
    if (not args.chkproj) and (args.epsg > 0):
        chk_projection = True

    files = glob.glob(args.input)

    print("File Checks ({} Files Found):".format(len(files)))

    from multiprocessing import Pool

    processes_pool = Pool(1)
    try:
        for cfile in files:
            try:
                if args.vec:
                    params = [cfile, args.rmerr, args.printnames, args.printerrs]
                    result = processes_pool.apply_async(_run_vec_chk, args=[params])
                    result.get(timeout=2)
                else:
                    params = [cfile, args.nbands, args.rmerr, args.printnames, args.printerrs, chk_projection, args.epsg, args.readimg]
                    result = processes_pool.apply_async(_run_img_chk, args=[params])
                    result.get(timeout=2)
            except Exception as e:
                if args.rmerr:
                    os.remove(cfile)
                    print("Removed {}".format(cfile))
                else:
                    print("rm {}".format(cfile))
                continue
        processes_pool.join()
    except Exception as inst:
        print("Finished with pool")

    print("Finish Checks")











    args = parser.parse_args()

    rsgis_utils = rsgislib.RSGISPyUtils()

    file_ok = True
    err_str = ''

    if args.vec:
        file_ok, err_str = rsgis_utils.check_gdal_vector_file(args.input, (not args.noband))
    else:
        file_ok, err_str = rsgis_utils.check_gdal_image_file(args.input)

    if file_ok:
        print("File is OK: {}".format(args.input))
    else:
        print("File has error: '{}': {}".format(err_str, args.input))
