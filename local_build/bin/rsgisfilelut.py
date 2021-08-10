#!/usr/bin/env python

#############################################
# rsgisfilelut.py
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
# Purpose:  Utilities to handle file LUTs. 
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 28/02/2019
# Version: 2.0
#
# History:
#
# Version 1 - was written to be used outside of RSGISLib
# Version 2 - was editted to use the functions within RSGISLib rather than copying code about (bad practise!)
# 
#############################################

import argparse
import rsgislib
import rsgislib.vectorutils   
    
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--lutfile", type=str, required=True, help="Input LUT vector file.")
    parser.add_argument("--lutlyr", type=str, required=False, help="Input LUT vector file layer.")
    parser.add_argument("--roifile", type=str, required=True, help="Input vector file defining the region of interest.")
    parser.add_argument("--roilyr", type=str, required=False, help="Input vector file layer defining the region of interest.")
    parser.add_argument("--dest", type=str, required=False, help="Output destination (file or folder) depending on the option specified.")
    parser.add_argument("-o", "--outputfile", type=str, required=False, help="Output file for commands.")
    parser.add_argument("--targz", action='store_true', default=False, help="Create tar command to archive the list of files.")
    parser.add_argument("--copy", action='store_true', default=False, help="Create copy command for the list of files.")
    args = parser.parse_args()
        
    cmds = rsgislib.vectorutils.queryFileLUT(args.lutfile, args.lutlyr, args.roifile, args.roilyr, args.dest, args.targz, args.copy)
    
    if args.outputfile is None:
        for cmd in cmds:
            print(cmd)
    else:
        rsgis_utils = rsgislib.RSGISPyUtils()
        rsgis_utils.writeList2File(cmds, args.outputfile)
