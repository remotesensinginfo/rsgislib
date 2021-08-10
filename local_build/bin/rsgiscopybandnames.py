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
# Purpose:  A command to copy band name from one file to another.
#
# Author: Pete Bunting
# Email: pfb@aber.ac.uk
# Date: 18/11/2016
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
#############################################################################

import rsgislib.imageutils
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-r", "--reference", dest="refFile", type=str, required=True, help="Input image")
parser.add_argument("-f", "--imagefile", dest="imageFile", type=str, required=True, help="Output image.")
args = parser.parse_args() 

bandNames = rsgislib.imageutils.getBandNames(args.refFile)
rsgislib.imageutils.setBandNames(args.imageFile, bandNames)
