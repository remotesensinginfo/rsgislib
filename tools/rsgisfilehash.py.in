#!/usr/bin/env python

#############################################
# rsgisfilehash.py
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
# Purpose: A script to find a hash string for an input file.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 08/04/2020
# Version: 1.0
#
##############################################

import os
from rsgislib import tools
import argparse

# Get input parameters
parser = argparse.ArgumentParser()
parser.add_argument("-i", "--input", type=str, required=True, help="Input file for which the hash will be created for.")
parser.add_argument("-b", '--blocksize', required=False, type=int, default=4096,
                    help="Block size used to calculate the file hash, default is 4096 bytes")
parser.add_argument("-o", '--outhash', required=False,
                    choices=['SHA1', 'SHA224', 'SHA256', 'SHA384', 'SHA512', 'MD5', 'Blake2B', 'Blake2S', 'SHA3_224',
                             'SHA3_256', 'SHA3_384', 'SHA3_512'], default='SHA256',
                    help="The hash to calculate; default: SHA256")

args = parser.parse_args()

input_file = args.input
if not os.path.exists(input_file):
    raise Exception("The input file does not exist.")

if args.outhash == 'SHA1':
    hash = tools.createSHA1Hash(input_file, args.blocksize)
elif args.outhash == 'SHA224':
    hash = tools.createSHA224Hash(input_file, args.blocksize)
elif args.outhash == 'SHA256':
    hash = tools.createSHA256Hash(input_file, args.blocksize)
elif args.outhash == 'SHA384':
    hash = tools.createSHA384Hash(input_file, args.blocksize)
elif args.outhash == 'SHA512':
    hash = tools.createSHA512Hash(input_file, args.blocksize)
elif args.outhash == 'MD5':
    hash = tools.createMD5Hash(input_file, args.blocksize)
elif args.outhash == 'Blake2B':
    hash = tools.createBlake2BHash(input_file, args.blocksize)
elif args.outhash == 'Blake2S':
    hash = tools.createBlake2SHash(input_file, args.blocksize)
elif args.outhash == 'SHA3_224':
    hash = tools.createSHA3_224Hash(input_file, args.blocksize)
elif args.outhash == 'SHA3_256':
    hash = tools.createSHA3_256Hash(input_file, args.blocksize)
elif args.outhash == 'SHA3_384':
    hash = tools.createSHA3_384Hash(input_file, args.blocksize)
elif args.outhash == 'SHA3_512':
    hash = tools.createSHA3_512Hash(input_file, args.blocksize)
else:
    raise Exception("The hash choice is unknown.")

print(hash)
