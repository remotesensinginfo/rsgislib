#! /usr/bin/env python

############################################################################
# Copyright (c) 2019 Dr. Peter Bunting, Aberystwyth University
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
#
# Purpose:  A command to find projection information from inputs.
#
# Author: Pete Bunting
# Email: pfb@aber.ac.uk
# Date: 09/09/2019
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
#############################################################################

import argparse
import os.path
import osgeo.gdal as gdal
import osgeo.osr as osr

def readTextFileNoNewLines(file):
    """
    Read a text file into a single string
    removing new lines.

    :return: string

    """
    if not os.path.exists(file):
        raise Exception("The input file does not exist, please check path: '{}'".format(file))
    
    txtStr = ""
    try:
        dataFile = open(file, 'r')
        for line in dataFile:
            txtStr += line.strip()
        dataFile.close()
    except Exception as e:
        raise e
    return txtStr


if __name__ == '__main__':
    """
    The command line user interface
    """
    parser = argparse.ArgumentParser(prog='rsgisproj.py', description='''Find projection information for a given input.''')

    parser.add_argument("--image", type=str, required=False, help='''The GDAL image file for which the projection will be outputted.''')
    parser.add_argument("--vecfile", type=str, required=False, help='''The GDAL vector file for which the projection will be outputted.''')
    parser.add_argument("--veclyr", type=str, required=False, help='''The layer within the --vecfile, if specified, 
                                                                      for which the projection information will be 
                                                                      outputted. If not provided then assume layer name
                                                                      is the same as the file name (e.g., shapefile)''')
    parser.add_argument("--epsg", type=int, required=False, help='''Provide an inputted EPSG code to define the projection.''')
    parser.add_argument("--epsga", type=int, required=False, help='''Provide an inputted EPSGA code to define the projection.''')
    parser.add_argument("--wktfile", type=str, required=False, help='''Provide an inputted WKT string as a text file to define the projection.''')
    parser.add_argument("--wkt", type=str, required=False, help='''Provide an inputted WKT string to define the projection.''')
    parser.add_argument("--proj4file", type=str, required=False, help='''Provide an inputted Proj4 string as a text file to define the projection.''')
    parser.add_argument("--proj4", type=str, required=False, help='''Provide an inputted Proj4 string to define the projection.''')
    parser.add_argument("--esrifile", type=str, required=False, help='''Provide an inputted ESRI .prj file to define the projection.''')
    parser.add_argument("--utm", type=str, required=False, help='''Provide a UTM zone format must be, XXH, where XX are 
                                                                   the zone and H is the hemisphere (e.g., 36N, 01S, 02N, 54S).
                                                                   WGS84 will be used as the datum.''')
    parser.add_argument("-o", "--output", required=True, type=str, nargs='+', choices=['EPSG', 'WKT', 'WKTPretty', 'PROJ4'], 
                         help='''The output format of the projection. Multiple options can be specified and they will be printed in turn.''')

    # Call the parser to parse the arguments.
    args = parser.parse_args()
    
    spat_ref_obj = None
    
    if args.image is not None:       
        img_ds = gdal.Open(args.image, gdal.GA_ReadOnly)
        if img_ds == None:
            raise Exception("Could not open raster image: '{}'".format(args.image))
        proj_wkt_str = img_ds.GetProjection()
        img_ds = None
        spat_ref_obj = osr.SpatialReference()
        spat_ref_obj.ImportFromWkt(proj_wkt_str)        
    elif args.vecfile is not None:        
        vec_ds = gdal.OpenEx(args.vecfile, gdal.OF_VECTOR)
        if vec_ds is None:
            raise Exception("Could not open vector dataset: {}".format(vec_ds))
        
        if args.veclyr is None:
            vec_lyr_name = os.path.splitext(os.path.basename(args.vecfile))[0]   
        else:
            vec_lyr_name = args.veclyr
            
        vec_lyr_obj = vec_ds.GetLayer(vec_lyr_name)
        if vec_lyr_obj is None:
            raise Exception("Could not open layer '{0}' within file: {1}".format(vec_lyr_name, args.vecfile))
        spat_ref_obj = vec_lyr_obj.GetSpatialRef()
        vec_ds = None
    elif args.epsg is not None:
        epsg_code = int(args.epsg)
        spat_ref_obj = osr.SpatialReference()
        spat_ref_obj.ImportFromEPSG(epsg_code)
    elif args.epsga is not None:
        epsga_code = int(args.epsga)
        spat_ref_obj = osr.SpatialReference()
        spat_ref_obj.ImportFromEPSGA(epsga_code)
    elif args.wktfile is not None:
        wkt_str = readTextFileNoNewLines(args.wktfile)
        spat_ref_obj = osr.SpatialReference()
        spat_ref_obj.ImportFromWkt(wkt_str)
    elif args.wkt is not None:
        spat_ref_obj = osr.SpatialReference()
        spat_ref_obj.ImportFromWkt(args.wkt)
    elif args.proj4file is not None:
        proj4_str = readTextFileNoNewLines(args.proj4file)
        spat_ref_obj = osr.SpatialReference()
        spat_ref_obj.ImportFromProj4(proj4_str)
    elif args.proj4 is not None:
        spat_ref_obj = osr.SpatialReference()
        spat_ref_obj.ImportFromProj4(args.proj4)
    elif args.esrifile is not None:
        esri_prj_str = readTextFileNoNewLines(args.esrifile)
        spat_ref_obj = osr.SpatialReference()
        spat_ref_obj.ImportFromESRI(esri_prj_str)
    elif args.utm is not None:
        utm_str = args.utm
        if utm_str.len() == 3:
            try:
                utm_zone = int(utm_str[0:1])
                utm_hemi = utm_str[2]
            except e:
                print("Could not retrieve UTM zone from UTM string: '{0}'. Zone = {1}, Hemispher = {2}".format(utm_str, utm_str[0:1], utm_str[2]))
                raise e
            hemi_north = True
            if utm_hemi.upper() == 'S':
                hemi_north = False
            
            spat_ref_obj = osr.SpatialReference()
            spat_ref_obj.SetUTM(utm_zone, hemi_north)
        else:
            raise("UTM string should only have three characters (e.g., 36N, 01S, 02N, 54S): '{0}'. ".format(utm_str))
    else:
        raise Exception("You must provide one of the input options.")
    

    if spat_ref_obj is not None:
        print("")
        for out_format in args.output:
            if out_format == 'WKT':
                print("{}\n".format(spat_ref_obj.ExportToWkt()))
            elif out_format == 'WKTPretty':
                print("{}\n".format(spat_ref_obj.ExportToPrettyWkt()))
            elif out_format == 'EPSG':
                spat_ref_obj.AutoIdentifyEPSG()
                print("EPSG:{}\n".format(spat_ref_obj.GetAuthorityCode(None)))
            elif out_format == 'PROJ4':
                print("{}\n".format(spat_ref_obj.ExportToProj4()))
            else:
                raise Exception("The output projection format was not recognised.")
    else:
        raise Exception("The projection object was None and therefore something has gone wrong when inputting the projection.")



