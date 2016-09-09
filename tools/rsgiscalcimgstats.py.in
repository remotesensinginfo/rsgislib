#! /usr/bin/env python

############################################################################
# Copyright (c) 2016 Dr. Peter Bunting, Aberystwyth University
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
# Purpose:  A command to use the rsgislib functions to provide a command line
#           tool for calculating image stats and pyramids
#
# Author: Pete Bunting
# Email: pfb@aber.ac.uk
# Date: 20/04/16
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
#############################################################################

import argparse
from rsgislib import imageutils
from rsgislib import rastergis


if __name__ == '__main__':
    """
    The command line user interface
    """
    parser = argparse.ArgumentParser(prog='rsgiscalcimgstats.py', description='''Calculate image statistics and pyramids''')

    parser.add_argument("-i", "--image", type=str, required=True, help='''The image on which stats and pyramids are to calculated.''')
    parser.add_argument("-t", "--thematic", action='store_true', default=False, help='''The image is thermatic, calc stats and pyramids accordingly.''')    
    parser.add_argument("-p", "--nopyramids", action='store_true', default=False, help='''Do not calculate image pyramids.''')
    parser.add_argument("-n", "--nodata", type=float, required=True, default=0.0, help='''No data value for the input image for when calculating stats''')
    parser.add_argument("-c", "--noclrtab", action='store_true', default=False, help='''Do not add a colour table (only for thematic images)''')    

    # Call the parser to parse the arguments.
    args = parser.parse_args()
    
    inImage = args.image
    pyramids = not args.nopyramids        
    
    if args.thematic:
        clrTab = not args.noclrtab
        ignoreZeros = False
        if args.nodata == 0.0:
            ignoreZeros = True
        rastergis.populateStats(clumps=inImage, addclrtab=clrTab, calcpyramids=pyramids, ignorezero=ignoreZeros)
    else:
        imageutils.popImageStats(inImage, usenodataval=True, nodataval=args.nodata, calcpyramids=pyramids)

