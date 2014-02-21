#! /usr/bin/env python

############################################################################
#  segutils.py
#
#  Copyright 2013 RSGISLib.
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
# Purpose:  Provide a set of utilities which combine commands to create
#           useful extra functionality and make it more easily available
#           to be reused.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 16/11/2013
# Version: 1.1
#
# History:
# Version 1.0 - Created.
# Version 1.1 - Update to be included into RSGISLib python modules tree.
#
############################################################################
# Import the rsgislib module
import rsgislib
# Import the image utilities module from rsgislib
import rsgislib.imageutils
# Import the image calc module from rsgislib
import rsgislib.imagecalc
# Import the image segmentation module from rsgislib
import rsgislib.segmentation
# Import the image rastergis module from rsgislib
import rsgislib.rastergis
# Import the module from rsgislib
import rsgislib
import os.path
import os
# Import the collections module
import collections
import fnmatch
import osgeo.gdal as gdal

def runShepherdSegmentation(inputImg, outputClumps, outputMeanImg=None, tmpath='.', gdalFormat='KEA', noStats=False, noStretch=False, noDelete=False, numClusters=100, minPxls=10, distThres=1000000, bands=None, sampling=100, kmMaxIter=200): 
    """
Utility function to call the segmentation algorithm of Shepherd et al. (2014).

Where:

* inputImg is a string containing the name of the input file
* outputClumps is a string containing the name of the output clump file
* outputMeanImg is the output mean image file (clumps attributed with pixel mean from input image) - pass 'None' to skip creating.
* tmpath is a file path for intermediate files (default is current directory).
* gdalformat is a string containing the GDAL format for the output file (default is KEA)
* noStats is a bool which specifies that no image statistics and pyramids should be built for the output images.
* noStretch is a bool which specifies that the input image bands should not be stretched.
* noDelete is a book which specifies that the temporary images created during processing should not be deleted once processing has been completed.
* numClusters is an int which specifies the number of clusters within the KMeans clustering
* minPxls is an int which specifies the minimum number pixels within a segments.
* distThres specifies the distance threshold for joining the segments (default is a very large value which turns off this option.).
* bands is an array providing a subset of image bands to use (default is None to use all bands)
* sampling specify the subsampling of the image for the data used within the KMeans (1 == no subsampling; default is 100)
* kmMaxIter maximum iterations for KMeans.

Example::

    from rsgislib.segmentation import segutils
    
    inputImg = 'jers1palsar_stack.kea'
    outputClumps = 'jers1palsar_stack_clumps_elim_final.kea'
    outputMeanImg = 'jers1palsar_stack_clumps_elim_final_mean.kea'
    
    segutils.runShepherdSegmentation(inputImg, outputClumps, outputMeanImg, minPxls=100)


    """
    rsgisUtils = rsgislib.RSGISPyUtils()
    
    basefile = os.path.basename(inputImg)
    basename = os.path.splitext(basefile)[0]
    
    outFileExt = rsgisUtils.getFileExtension(gdalFormat)
    
    createdDIR = False
    if not os.path.isdir(tmpath):
        os.makedirs(tmpath)
        createdDIR = True
        
    # Select Image Bands if required
    inputImgBands = inputImg
    selectBands = False
    if not bands == None:
        print("Subsetting the image bands")
        selectBands = True
        gdalDS = gdal.Open(inputImg, gdal.GA_ReadOnly)
        rsgisUtils = rsgislib.RSGISPyUtils()
        dType = rsgisUtils.getRSGISLibDataType(gdal.GetDataTypeName(gdalDS.GetRasterBand(1).DataType))
        gdalDS = None
        inputImgBands = os.path.join(tmpath,basename+str("_bselect")+outFileExt)
        rsgislib.imageutils.selectImageBands(inputImg, inputImgBands, gdalFormat, dType, bands)        
    
    # Stretch input data if required.
    segmentFile = inputImgBands
    if not noStretch:
        segmentFile = os.path.join(tmpath,basename+str("_stchd")+outFileExt)
        strchFile = os.path.join(tmpath,basename+str("_stchdonly")+outFileExt)
        strchFileOffset = os.path.join(tmpath,basename+str("_stchdonlyOff")+outFileExt)
        strchMaskFile = os.path.join(tmpath,basename+str("_stchdmaskonly")+outFileExt)
        print("Stretch Input Image")
        rsgislib.imageutils.stretchImage(inputImgBands, strchFile, False, "", True, False, gdalFormat, rsgislib.TYPE_32FLOAT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)
        
        print("Add 1 to stretched file to ensure there are no all zeros (i.e., no data) regions created.")
        rsgislib.imagecalc.imageMath(strchFile, strchFileOffset, "b1+1", gdalFormat, rsgislib.TYPE_32FLOAT)
        
        print("Create Input Image Mask.")
        ImgBand = collections.namedtuple('ImgBands', ['bandName', 'fileName', 'bandIndex'])
        bandMathBands = list()
        bandMathBands.append(ImgBand(bandName="b1", fileName=inputImgBands, bandIndex=1))
        rsgislib.imagecalc.bandMath(strchMaskFile, "b1==0?1:0", gdalFormat, rsgislib.TYPE_32FLOAT, bandMathBands)
        
        print("Mask stretched Image.")
        rsgislib.imageutils.maskImage(strchFileOffset, strchMaskFile, segmentFile, gdalFormat, rsgislib.TYPE_32FLOAT, 0, 1)
        
        if not noDelete:
            # Deleting extra files
            rsgisUtils.deleteFileWithBasename(strchFile)
            rsgisUtils.deleteFileWithBasename(strchFileOffset)
            rsgisUtils.deleteFileWithBasename(strchMaskFile)
            
    # Perform KMEANS
    print("Performing KMeans.")
    outMatrixFile = os.path.join(tmpath,basename+str("_kmeansclusters"))
    rsgislib.imagecalc.kMeansClustering(segmentFile, outMatrixFile, numClusters, kmMaxIter, sampling, True, 0.0025, rsgislib.imagecalc.INITCLUSTER_DIAGONAL_FULL_ATTACH)
    
    # Apply KMEANS
    print("Apply KMeans to image.")
    kMeansFileZones = os.path.join(tmpath,basename+str("_kmeans")+outFileExt)
    rsgislib.segmentation.labelPixelsFromClusterCentres(segmentFile, kMeansFileZones, outMatrixFile+str(".gmtxt"), True, gdalFormat)
    
    # Elimininate Single Pixels
    print("Eliminate Single Pixels.")
    kMeansFileZonesNoSgls = os.path.join(tmpath,basename+str("_kmeans_nosgl")+outFileExt)
    kMeansFileZonesNoSglsTmp = os.path.join(tmpath,basename+str("_kmeans_nosglTMP")+outFileExt)
    rsgislib.segmentation.eliminateSinglePixels(segmentFile, kMeansFileZones, kMeansFileZonesNoSgls, kMeansFileZonesNoSglsTmp, gdalFormat, False, True)
    
    # Clump
    print("Perform clump.")
    initClumpsFile = os.path.join(tmpath,basename+str("_clumps")+outFileExt)
    rsgislib.segmentation.clump(kMeansFileZonesNoSgls, initClumpsFile, gdalFormat, False, 0)
    
    # Elimininate small clumps
    print("Eliminate small pixels.")
    elimClumpsFile = os.path.join(tmpath,basename+str("_clumps_elim")+outFileExt)
    rsgislib.segmentation.RMSmallClumpsStepwise(segmentFile, initClumpsFile, elimClumpsFile, gdalFormat, False, "", False, False, minPxls, distThres)
    
    # Relabel clumps
    print("Relabel clumps.")
    rsgislib.segmentation.relabelClumps(elimClumpsFile, outputClumps, gdalFormat, False)
    
    # Populate with stats if required.
    if not noStats:
        print("Calculate image statistics and build pyramids.")
        rsgislib.rastergis.populateStats(outputClumps, True, True)
    
    # Create mean image if required.
    if not (outputMeanImg == None):
        rsgislib.segmentation.meanImage(inputImg, outputClumps, outputMeanImg, gdalFormat, rsgislib.TYPE_32FLOAT)
        if not noStats:
            rsgislib.imageutils.popImageStats(outputMeanImg, True, 0, True)
        
    if not noDelete:
        # Deleting extra files
        rsgisUtils.deleteFileWithBasename(outMatrixFile+str(".gmtxt"))
        rsgisUtils.deleteFileWithBasename(kMeansFileZones)
        rsgisUtils.deleteFileWithBasename(kMeansFileZonesNoSgls)
        rsgisUtils.deleteFileWithBasename(kMeansFileZonesNoSglsTmp)
        rsgisUtils.deleteFileWithBasename(initClumpsFile)
        rsgisUtils.deleteFileWithBasename(elimClumpsFile)
        if selectBands:
            rsgisUtils.deleteFileWithBasename(inputImgBands)
        if not noStretch:
            rsgisUtils.deleteFileWithBasename(segmentFile)
        if createdDIR:
            rsgisUtils.deleteDIR(tmpath)
