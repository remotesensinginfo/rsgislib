############################################################################
#  imagecomp.py
#
#  Copyright 2017 RSGISLib.
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
#
# Purpose:  Provide a set of utilities for creating image composites
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 04/10/2017
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
############################################################################

import rsgislib
import rsgislib.imagecalc
import rsgislib.imagecalc.calcindices
import rsgislib.imageutils
import rsgislib.rastergis

import glob
import os
import os.path
import numpy
import shutil

import osgeo.gdal
import rios.rat

def createMaxNDVIComposite(inImgsPattern, rBand, nBand, tmpPath, outRefImg, outCompImg):
    rsgisUtils = rsgislib.RSGISPyUtils()
    uidStr = rsgisUtils.uidGenerator()
    
    # Get List of input images:
    inImages = glob.glob(inImgsPattern)
    
    if len(inImages) > 1:
        tmpPresent = True
        if not os.path.exists(tmpPath):
            os.makedirs(tmpPath)
            tmpPresent = False 
        
        refLayersPath = os.path.join(tmpPath, 'RefLyrs_'+uidStr)
        
        refImgTmpPresent = True
        if not os.path.exists(refLayersPath):
            os.makedirs(refLayersPath)
            refImgTmpPresent = False
        
        numInLyrs = len(inImages)
        
        numpy.random.seed(5)
        red = numpy.random.randint(1, 255, numInLyrs+1, int)
        red[0] = 0
        numpy.random.seed(2)
        green = numpy.random.randint(1, 255, numInLyrs+1, int)
        green[0] = 0
        numpy.random.seed(9)
        blue = numpy.random.randint(1, 255, numInLyrs+1, int)
        blue[0] = 0
        alpha = numpy.zeros_like(blue)
        alpha[...] = 255
        imgLyrs = numpy.empty(numInLyrs+1, dtype=numpy.dtype('a255'))
            
        # Generate Comp Ref layers:
        refLyrsLst = []
        idx = 1
        for img in inImages:
            print('In Image ('+str(idx) + '):\t' + img)
            imgLyrs[idx] = os.path.basename(img)
            baseImgName = os.path.splitext(os.path.basename(img))[0]
            refLyrImg = os.path.join(refLayersPath, baseImgName+'_ndvi.kea')
            rsgislib.imagecalc.calcindices.calcNDVI(img, 3, 4, refLyrImg, False)
            refLyrsLst.append(refLyrImg)
            idx = idx + 1
    
        # Create REF Image
        rsgislib.imagecalc.getImgIdxForStat(refLyrsLst, outRefImg, 'KEA', -999, rsgislib.SUMTYPE_MAX)
        # Pop Ref Image with stats
        rsgislib.rastergis.populateStats(outRefImg, True, True, True)
        
        # Open the clumps dataset as a gdal dataset
        ratDataset = osgeo.gdal.Open(outRefImg, osgeo.gdal.GA_Update)
        
        # Write colours to RAT
        rios.rat.writeColumn(ratDataset, "Red", red)
        rios.rat.writeColumn(ratDataset, "Green", green)
        rios.rat.writeColumn(ratDataset, "Blue", blue)
        rios.rat.writeColumn(ratDataset, "Alpha", alpha)
        rios.rat.writeColumn(ratDataset, "Image", imgLyrs)
        
        ratDataset = None
        
        # Create Composite Image
        rsgislib.imageutils.createRefImgCompositeImg(inImages, outCompImg, outRefImg, 'KEA', rsgislib.TYPE_16UINT, 0.0)
        # Calc Stats
        rsgislib.imageutils.popImageStats(outCompImg, usenodataval=True, nodataval=0, calcpyramids=True)
    
        if not refImgTmpPresent:
            shutil.rmtree(refLayersPath, ignore_errors=True)
    
        if not tmpPresent:
            shutil.rmtree(tmpPath, ignore_errors=True)
    elif len(inImages) == 1:
        print("Only 1 Input Image, Just Copying File to output")
        shutil.copy(inImages[0], outCompImg)
    else:
        raise rsgislib.RSGISPyException("There were no input images for " + inImgsPattern)



