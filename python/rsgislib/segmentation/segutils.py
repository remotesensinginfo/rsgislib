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
###########################################################################
# Import shutil
import shutil
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
# Import the python OS module
import os
# Import the collections module
import collections
#import the gdal module
import osgeo.gdal as gdal
# Import the python JSON module
import json


def runShepherdSegmentation(inputImg, outputClumps, outputMeanImg=None, tmpath='.', gdalformat='KEA', noStats=False, noStretch=False, noDelete=False, numClusters=60, minPxls=100, distThres=100, bands=None, sampling=100, kmMaxIter=200, processInMem=False, saveProcessStats=False, imgStretchStats="", kMeansCentres="", imgStatsJSONFile=""): 
    """
Utility function to call the segmentation algorithm of Shepherd et al. (2019).

Shepherd, J. D., Bunting, P., & Dymond, J. R. (2019). Operational Large-Scale Segmentation of Imagery Based on Iterative Elimination. Remote Sensing, 11(6), 658. http://doi.org/10.3390/rs11060658

Where:

:param inputImg: is a string containing the name of the input file.
:param outputClumps: is a string containing the name of the output clump file.
:param outputMeanImg: is the output mean image file (clumps attributed with pixel mean from input image) - pass 'None' to skip creating.
:param tmpath: is a file path for intermediate files (default is current directory).
:param gdalformat: is a string containing the GDAL format for the output file (default = KEA).
:param noStats: is a bool which specifies that no image statistics and pyramids should be built for the output images (default = False)/
:param noStretch: is a bool which specifies that the input image bands should not be stretched (default = False).
:param noDelete: is a bool which specifies that the temporary images created during processing should not be deleted once processing has been completed (default = False).
:param numClusters: is an int which specifies the number of clusters within the KMeans clustering (default = 60).
:param minPxls: is an int which specifies the minimum number pixels within a segments (default = 100).
:param distThres: specifies the distance threshold for joining the segments (default = 100, set to large number to turn off this option).
:param bands: is an array providing a subset of image bands to use (default is None to use all bands).
:param sampling: specify the subsampling of the image for the data used within the KMeans (default = 100; 1 == no subsampling).
:param kmMaxIter: maximum iterations for KMeans.
:param processInMem: where functions allow it perform processing in memory rather than on disk.
:param saveProcessStats: is a bool which specifies that the image stretch stats and the kMeans centre stats should be saved along with a header.
:param imgStretchStats: is a string providing the file name and path for the image stretch stats (Output).
:param kMeansCentres: is a string providing the file name and path for the KMeans clusters centres (don't include file extension; .gmtxt will be added to the end) (Output).
:param imgStatsJSONFile: is a string providing the name and path of a JSON file storing the image spatial extent and imgStretchStats and kMeansCentres file paths for use by other commands (Output).

Example::

    from rsgislib.segmentation import segutils
    
    inputImg = 'jers1palsar_stack.kea'
    outputClumps = 'jers1palsar_stack_clumps_elim_final.kea'
    outputMeanImg = 'jers1palsar_stack_clumps_elim_final_mean.kea'
    
    segutils.runShepherdSegmentation(inputImg, outputClumps, outputMeanImg, minPxls=100)


    """
    
    if saveProcessStats:
        if (imgStretchStats=="") or (kMeansCentres=="") or (imgStatsJSONFile==""):
            raise rsgislib.RSGISPyException("if image stretch and kmeans centres are to be saved then all file names (imgStretchStats, kMeansCentres, imgStatsJSONFile) need to be provided.")
    
    rsgisUtils = rsgislib.RSGISPyUtils()
    
    basefile = os.path.basename(inputImg)
    basename = os.path.splitext(basefile)[0]
    
    outFileExt = rsgisUtils.getFileExtension(gdalformat)
    
    createdDIR = False
    if not os.path.isdir(tmpath):
        os.makedirs(tmpath)
        createdDIR = True

    # Get data type of input image
    gdalDS = gdal.Open(inputImg, gdal.GA_ReadOnly)
    rsgisUtils = rsgislib.RSGISPyUtils()
    input_datatype = rsgisUtils.getRSGISLibDataType(gdal.GetDataTypeName(gdalDS.GetRasterBand(1).DataType))
    gdalDS = None
        
    # Select Image Bands if required
    inputImgBands = inputImg
    selectBands = False
    if not bands == None:
        print("Subsetting the image bands")
        selectBands = True
        inputImgBands = os.path.join(tmpath,basename+str("_bselect")+outFileExt)
        rsgislib.imageutils.selectImageBands(inputImg, inputImgBands, gdalformat, input_datatype, bands)        
    
    # Stretch input data if required.
    segmentFile = inputImgBands
    if not noStretch:
        segmentFile = os.path.join(tmpath,basename+str("_stchd")+outFileExt)
        strchFile = os.path.join(tmpath,basename+str("_stchdonly")+outFileExt)
        strchFileOffset = os.path.join(tmpath,basename+str("_stchdonlyOff")+outFileExt)
        strchMaskFile = os.path.join(tmpath,basename+str("_stchdmaskonly")+outFileExt)
        
        print("Stretch Input Image")
        rsgislib.imageutils.stretchImage(inputImgBands, strchFile, saveProcessStats, imgStretchStats, True, False, gdalformat, rsgislib.TYPE_8INT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)
        
        print("Add 1 to stretched file to ensure there are no all zeros (i.e., no data) regions created.")
        rsgislib.imagecalc.imageMath(strchFile, strchFileOffset, "b1+1", gdalformat, rsgislib.TYPE_8INT)
        
        print("Create Input Image Mask.")
        ImgBand = collections.namedtuple('ImgBands', ['bandName', 'fileName', 'bandIndex'])
        bandMathBands = list()
        bandMathBands.append(ImgBand(bandName="b1", fileName=inputImgBands, bandIndex=1))
        rsgislib.imagecalc.bandMath(strchMaskFile, "b1==0?1:0", gdalformat, rsgislib.TYPE_8INT, bandMathBands)
        
        print("Mask stretched Image.")
        rsgislib.imageutils.maskImage(strchFileOffset, strchMaskFile, segmentFile, gdalformat, rsgislib.TYPE_8INT, 0, 1)
        
        if not noDelete:
            # Deleting extra files
            rsgisUtils.deleteFileWithBasename(strchFile)
            rsgisUtils.deleteFileWithBasename(strchFileOffset)
            rsgisUtils.deleteFileWithBasename(strchMaskFile)
            
    # Perform KMEANS
    print("Performing KMeans.")
    outMatrixFile = os.path.join(tmpath,basename+str("_kmeansclusters"))
    if saveProcessStats:
        outMatrixFile = kMeansCentres
    rsgislib.imagecalc.kMeansClustering(segmentFile, outMatrixFile, numClusters, kmMaxIter, sampling, True, 0.0025, rsgislib.imagecalc.INITCLUSTER_DIAGONAL_FULL_ATTACH)
    
    # Apply KMEANS
    print("Apply KMeans to image.")
    kMeansFileZones = os.path.join(tmpath,basename+str("_kmeans")+outFileExt)
    rsgislib.segmentation.labelPixelsFromClusterCentres(segmentFile, kMeansFileZones, outMatrixFile+str(".gmtxt"), True, gdalformat)
    
    # Elimininate Single Pixels
    print("Eliminate Single Pixels.")
    kMeansFileZonesNoSgls = os.path.join(tmpath,basename+str("_kmeans_nosgl")+outFileExt)
    kMeansFileZonesNoSglsTmp = os.path.join(tmpath,basename+str("_kmeans_nosglTMP")+outFileExt)
    rsgislib.segmentation.eliminateSinglePixels(segmentFile, kMeansFileZones, kMeansFileZonesNoSgls, kMeansFileZonesNoSglsTmp, gdalformat, processInMem, True)
    
    # Clump
    print("Perform clump.")
    initClumpsFile = os.path.join(tmpath,basename+str("_clumps")+outFileExt)
    rsgislib.segmentation.clump(kMeansFileZonesNoSgls, initClumpsFile, gdalformat, processInMem, 0)
    
    # Elimininate small clumps
    print("Eliminate small pixels.")
    elimClumpsFile = os.path.join(tmpath,basename+str("_clumps_elim")+outFileExt)
    rsgislib.segmentation.rmSmallClumpsStepwise(segmentFile, initClumpsFile, elimClumpsFile, gdalformat, False, "", False, processInMem, minPxls, distThres)
    
    # Relabel clumps
    print("Relabel clumps.")
    rsgislib.segmentation.relabelClumps(elimClumpsFile, outputClumps, gdalformat, processInMem)
    
    # Populate with stats if required.
    if not noStats:
        print("Calculate image statistics and build pyramids.")
        rsgislib.rastergis.populateStats(outputClumps, True, True)
    
    # Create mean image if required.
    if not (outputMeanImg == None):
        rsgislib.segmentation.meanImage(inputImg, outputClumps, outputMeanImg, gdalformat, input_datatype)
        if not noStats:
            rsgislib.imageutils.popImageStats(outputMeanImg, True, 0, True)
    
    
    if saveProcessStats:
        gdalDS = gdal.Open(inputImg, gdal.GA_ReadOnly)
        geotransform = gdalDS.GetGeoTransform()
        if not geotransform is None:
            xTL = geotransform[0]
            yTL = geotransform[3]
            
            xRes = geotransform[1]
            yRes = geotransform[5]
            
            width = gdalDS.RasterXSize * xRes
            if yRes < 0:
                yRes = yRes * (-1)
            height = gdalDS.RasterYSize * yRes
            xBR = xTL + width
            yBR = yTL - height
            
            xCen = xTL + (width/2)
            yCen = yBR + (height/2)
        
            sceneData = dict()
            sceneData['KCENTRES'] = kMeansCentres+str(".gmtxt")
            sceneData['STRETCHSTATS'] = imgStretchStats
            sceneData['CENTRE_PT'] = {'X':xCen, 'Y':yCen}
            sceneData['BBOX'] = {'XMIN':xTL, 'YMIN':yBR, 'XMAX':xBR, 'YMAX':yTL}
            
            with open(imgStatsJSONFile, 'w') as outfile:
                json.dump(sceneData, outfile, sort_keys=True, indent=4, separators=(',', ': '), ensure_ascii=False)

        gdalDS = None
     
    if not noDelete:
        # Deleting extra files
        if not saveProcessStats:
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
            shutil.rmtree(tmpath)
            

def runShepherdSegmentationPreCalcdStats(inputImg, outputClumps, kMeansCentres, imgStretchStats, outputMeanImg=None, tmpath='.', gdalformat='KEA', noStats=False, noStretch=False, noDelete=False, minPxls=100, distThres=100, bands=None, processInMem=False): 
    """
Utility function to call the segmentation algorithm of Shepherd et al. (2019) using pre-calculated stretch stats and KMeans cluster centres.

Shepherd, J. D., Bunting, P., & Dymond, J. R. (2019). Operational Large-Scale Segmentation of Imagery Based on Iterative Elimination. Remote Sensing, 11(6), 658. http://doi.org/10.3390/rs11060658

Where:

:param inputImg: is a string containing the name of the input file.
:param outputClumps: is a string containing the name of the output clump file.
:param kMeansCentres: is a string providing the file name and path for the KMeans clusters centres (Input)
:param imgStretchStats: is a string providing the file name and path for the image stretch stats (Input - not required if noStretch=True)
:param outputMeanImg: is the output mean image file (clumps attributed with pixel mean from input image) - pass 'None' to skip creating.
:param tmpath: is a file path for intermediate files (default is current directory).
:param gdalformat: is a string containing the GDAL format for the output file (default = KEA).
:param noStats: is a bool which specifies that no image statistics and pyramids should be built for the output images (default = False)/
:param noStretch: is a bool which specifies that the input image bands should not be stretched (default = False).
:param noDelete: is a bool which specifies that the temporary images created during processing should not be deleted once processing has been completed (default = False).
:param minPxls: is an int which specifies the minimum number pixels within a segments (default = 100).
:param distThres: specifies the distance threshold for joining the segments (default = 100, set to large number to turn off this option).
:param bands: is an array providing a subset of image bands to use (default is None to use all bands).
:param sampling: specify the subsampling of the image for the data used within the KMeans (default = 100; 1 == no subsampling).
:param processInMem: where functions allow it perform processing in memory rather than on disk.

Example::

    from rsgislib.segmentation import segutils
    
    inputImg = 'jers1palsar_stack.kea'
    outputClumps = 'jers1palsar_stack_clumps_elim_final.kea'
    outputMeanImg = 'jers1palsar_stack_clumps_elim_final_mean.kea'
    kMeansCentres = 'jers1palsar_stack_kcentres.gmtxt'
    imgStretchStats = 'jers1palsar_stack_stchstats.txt'
    
    segutils.runShepherdSegmentationPreCalcdStats(inputImg, outputClumps, kMeansCentres, imgStretchStats, outputMeanImg, minPxls=100)

    """
    
    if not noStretch:
        if (imgStretchStats=="") or (imgStretchStats==None):
            raise rsgislib.RSGISPyException("A stretch stats file must be provided")
    
    rsgisUtils = rsgislib.RSGISPyUtils()
    
    basefile = os.path.basename(inputImg)
    basename = os.path.splitext(basefile)[0]
    
    outFileExt = rsgisUtils.getFileExtension(gdalformat)
    
    createdDIR = False
    if not os.path.isdir(tmpath):
        os.makedirs(tmpath)
        createdDIR = True

    # Get data type of input image
    gdalDS = gdal.Open(inputImg, gdal.GA_ReadOnly)
    rsgisUtils = rsgislib.RSGISPyUtils()
    input_datatype = rsgisUtils.getRSGISLibDataType(gdal.GetDataTypeName(gdalDS.GetRasterBand(1).DataType))
    gdalDS = None
        
    # Select Image Bands if required
    inputImgBands = inputImg
    selectBands = False
    if not bands == None:
        print("Subsetting the image bands")
        selectBands = True
        inputImgBands = os.path.join(tmpath,basename+str("_bselect")+outFileExt)
        rsgislib.imageutils.selectImageBands(inputImg, inputImgBands, gdalformat, input_datatype, bands)        
    
    # Stretch input data if required.
    segmentFile = inputImgBands
    if not noStretch:
        segmentFile = os.path.join(tmpath,basename+str("_stchd")+outFileExt)
        strchFile = os.path.join(tmpath,basename+str("_stchdonly")+outFileExt)
        strchFileOffset = os.path.join(tmpath,basename+str("_stchdonlyOff")+outFileExt)
        strchMaskFile = os.path.join(tmpath,basename+str("_stchdmaskonly")+outFileExt)
        
        print("Stretch Input Image")
        rsgislib.imageutils.stretchImageWithStats(inputImgBands, strchFile, imgStretchStats, gdalformat, rsgislib.TYPE_8INT, rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
        
        print("Add 1 to stretched file to ensure there are no all zeros (i.e., no data) regions created.")
        rsgislib.imagecalc.imageMath(strchFile, strchFileOffset, "b1+1", gdalformat, rsgislib.TYPE_8INT)
        
        print("Create Input Image Mask.")
        ImgBand = collections.namedtuple('ImgBands', ['bandName', 'fileName', 'bandIndex'])
        bandMathBands = list()
        bandMathBands.append(ImgBand(bandName="b1", fileName=inputImgBands, bandIndex=1))
        rsgislib.imagecalc.bandMath(strchMaskFile, "b1==0?1:0", gdalformat, rsgislib.TYPE_8INT, bandMathBands)
        
        print("Mask stretched Image.")
        rsgislib.imageutils.maskImage(strchFileOffset, strchMaskFile, segmentFile, gdalformat, rsgislib.TYPE_8INT, 0, 1)
        
        if not noDelete:
            # Deleting extra files
            rsgisUtils.deleteFileWithBasename(strchFile)
            rsgisUtils.deleteFileWithBasename(strchFileOffset)
            rsgisUtils.deleteFileWithBasename(strchMaskFile)
    
    # Apply KMEANS
    print("Apply KMeans to image.")
    kMeansFileZones = os.path.join(tmpath,basename+str("_kmeans")+outFileExt)
    rsgislib.segmentation.labelPixelsFromClusterCentres(segmentFile, kMeansFileZones, kMeansCentres, True, gdalformat)
    
    # Elimininate Single Pixels
    print("Eliminate Single Pixels.")
    kMeansFileZonesNoSgls = os.path.join(tmpath,basename+str("_kmeans_nosgl")+outFileExt)
    kMeansFileZonesNoSglsTmp = os.path.join(tmpath,basename+str("_kmeans_nosglTMP")+outFileExt)
    rsgislib.segmentation.eliminateSinglePixels(segmentFile, kMeansFileZones, kMeansFileZonesNoSgls, kMeansFileZonesNoSglsTmp, gdalformat, processInMem, True)
    
    # Clump
    print("Perform clump.")
    initClumpsFile = os.path.join(tmpath,basename+str("_clumps")+outFileExt)
    rsgislib.segmentation.clump(kMeansFileZonesNoSgls, initClumpsFile, gdalformat, processInMem, 0)
    
    # Elimininate small clumps
    print("Eliminate small pixels.")
    elimClumpsFile = os.path.join(tmpath,basename+str("_clumps_elim")+outFileExt)
    rsgislib.segmentation.rmSmallClumpsStepwise(segmentFile, initClumpsFile, elimClumpsFile, gdalformat, False, "", False, processInMem, minPxls, distThres)
    
    # Relabel clumps
    print("Relabel clumps.")
    rsgislib.segmentation.relabelClumps(elimClumpsFile, outputClumps, gdalformat, processInMem)
    
    # Populate with stats if required.
    if not noStats:
        print("Calculate image statistics and build pyramids.")
        rsgislib.rastergis.populateStats(outputClumps, True, True)
    
    # Create mean image if required.
    if not (outputMeanImg == None):
        rsgislib.segmentation.meanImage(inputImg, outputClumps, outputMeanImg, gdalformat, input_datatype)
        if not noStats:
            rsgislib.imageutils.popImageStats(outputMeanImg, True, 0, True)
     
    if not noDelete:
        # Deleting extra files
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
            shutil.rmtree(tmpath)



def runShepherdSegmentationTestNumClumps(inputImg, outputClumpsBase, outStatsFile, outputMeanImgBase=None, tmpath='.', gdalformat='KEA', noStats=False, noStretch=False, noDelete=False, numClustersStart=10, numClustersStep=10, numOfClustersSteps=10, minPxls=10, distThres=1000000, bands=None, sampling=100, kmMaxIter=200, processInMem=False, minNormV=None, maxNormV=None, minNormMI=None, maxNormMI=None): 
    """
Utility function to call the segmentation algorithm of Shepherd et al. (2019) and to test are range of 'k' within the kMeans.

Shepherd, J. D., Bunting, P., & Dymond, J. R. (2019). Operational Large-Scale Segmentation of Imagery Based on Iterative Elimination. Remote Sensing, 11(6), 658. http://doi.org/10.3390/rs11060658

Where:

:param inputImg: is a string containing the name of the input file
:param outputClumps: is a string containing the name of the output clump file
:param outStatsFile: is a string containing the name of the output CSV file with the image segmentation stats
:param outputMeanImg: is the output mean image file (clumps attributed with pixel mean from input image) - pass 'None' to skip creating.
:param tmpath: is a file path for intermediate files (default is current directory).
:param gdalformat: is a string containing the GDAL format for the output file (default is KEA)
:param noStats: is a bool which specifies that no image statistics and pyramids should be built for the output images.
:param noStretch: is a bool which specifies that the input image bands should not be stretched.
:param noDelete: is a book which specifies that the temporary images created during processing should not be deleted once processing has been completed.
:param numClustersStart: is an int which specifies the number of clusters within the KMeans clustering to start the process
:param numClustersStep: is an int which specifies the number of clusters within the KMeans clustering added with each step
:param numOfClustersSteps: is an int which specifies the number steps (i.e., tests) which are performed.
:param minPxls: is an int which specifies the minimum number pixels within a segments.
:param distThres: specifies the distance threshold for joining the segments (default is a very large value which turns off this option.).
:param bands: is an array providing a subset of image bands to use (default is None to use all bands)
:param sampling: specify the subsampling of the image for the data used within the KMeans (1 == no subsampling; default is 100)
:param kmMaxIter: maximum iterations for KMeans.
:param processInMem: where functions allow it perform processing in memory rather than on disk.
:param minNormV: is a floating point =None
:param maxNormV: None
:param minNormMI: None
:param maxNormMI: None

Example::

    from rsgislib.segmentation import segutils


    inputImg = './WV2_525N040W_20110727_TOARefl_b762_stch.kea'
    outputClumpsBase = './OptimalTests/WV2_525N040W_20110727_Clumps'
    outputMeanImgBase = './OptimalTests/WV2_525N040W_20110727_ClumpsMean'
    tmpath='./OptimalTests/tmp/'
    outStatsFile = './OptimalTests/StatsClumps.csv'

    # Will test clump values from 10 to 200 with intervals of 10.
    segutils.runShepherdSegmentationTestNumClumps(inputImg, outputClumpsBase, outStatsFile, outputMeanImgBase=outputMeanImgBase, tmpath=tmpath, noStretch=True, numClustersStart=10, numClustersStep=10, numOfClustersSteps=20, minPxls=50, minNormV=None, maxNormV=None, minNormMI=None, maxNormMI=None)


    """
    colsPrefix = 'gs'
    calcNeighbours = True
    calcNormVals = False
    if minNormV==None or  maxNormV==None or minNormMI==None or maxNormMI==None:
        minNormV = 0.0
        maxNormV = 1.0
        minNormMI = 0.0
        maxNormMI = 1.0
        calcNormVals = True
    
    
    
    outputStats = list()
    
    numClusters = numClustersStart
    for i in range(numOfClustersSteps):
        numClusters = numClustersStart + (i * numClustersStep)
        print("Processing ", numClusters)
        outputClumps = outputClumpsBase + "_c" + str(numClusters) + ".kea"
        outputMeanImg = outputMeanImgBase + "_c" + str(numClusters) + ".kea"
        
        runShepherdSegmentation(inputImg, outputClumps, outputMeanImg=outputMeanImg, tmpath=tmpath, gdalformat=gdalformat, noStats=noStats, noStretch=noStretch, noDelete=noDelete, numClusters=numClusters, minPxls=minPxls, distThres=distThres, bands=bands, sampling=sampling, kmMaxIter=kmMaxIter, processInMem=processInMem)
        
        segScores = rsgislib.rastergis.calcGlobalSegmentationScore(outputClumps, inputImg, colsPrefix, calcNeighbours, minNormV, maxNormV, minNormMI, maxNormMI)
        
        tup = (numClusters, segScores)
        outputStats.append(tup)
        
    
    numImgBands = int(len(outputStats[0][1][1])/4)
    
    
    if calcNormVals:
        minVar = 0.0
        maxVar = 0.0
        scaleVar = 0.0
        minMI = 0.0
        maxMI = 0.0
        scaleMI = 0.0
        first = True
        
        for stat in outputStats:        
            for i in range(numImgBands):
                idxVar = i*4
                idxMI = (i*4)+1
                if first:
                    minVar = stat[1][1][idxVar] # Var Min
                    maxVar = stat[1][1][idxVar] # Var Max
                    minMI = stat[1][1][idxMI] # MI Min
                    maxMI = stat[1][1][idxMI] # MI Max
                    first = False
                else:
                    if stat[1][1][idxVar] < minVar:
                        minVar = stat[1][1][idxVar] # Set Var Min
                    elif stat[1][1][idxVar] > maxVar:
                        maxVar = stat[1][1][idxVar] # Set Var Max
                        
                    if stat[1][1][idxMI] < minMI:
                        minMI = stat[1][1][idxMI] # Set MI Min
                    elif stat[1][1][idxMI] > maxMI:
                        maxMI = stat[1][1][idxMI] # Set MI Max
        
        print("Var Min: ", minVar)
        print("Var Max: ", maxVar)
        print("MI Min: ", minMI)
        print("MI Max: ", maxMI)
    
        if (maxVar - minVar) > 0:
            scaleVar = 1/(maxVar - minVar)
        else:
            scaleVar = 1
        
        if (maxMI - minMI) > 0:
            scaleMI = 1/(maxMI - minMI)
        else:
            scaleMI = 1
    
        print("Var Scale: ", scaleVar)
        print("MI Scale: ", scaleMI)
    
        for stat in outputStats:
            gScore = 0.0
            for i in range(numImgBands):
                idxVar = i*4
                idxMI = (i*4) + 1
                idxVarNorm = (i*4) + 2
                idxMINorm = (i*4) + 3
                
                stat[1][1][idxVarNorm] = (stat[1][1][idxVar] - minVar) * scaleVar
                stat[1][1][idxMINorm] = (stat[1][1][idxMI] - minMI) * scaleMI
            
                gScore = gScore + stat[1][1][idxVarNorm] + stat[1][1][idxMINorm]
            stat[1][0] = gScore
    
    fileStats = open(outStatsFile, "w")
    
    colNames = "Clusters, Overall Score"
    for i in range(numImgBands):
        colNames = colNames + ", B" + str(i+1) + "_Variance"
        colNames = colNames + ", B" + str(i+1) + "_MI"
        colNames = colNames + ", B" + str(i+1) + "_VarianceNorm"
        colNames = colNames + ", B" + str(i+1) + "_MINorm"
    colNames = colNames + "\n"
    
    fileStats.write(colNames)
    
    print("Clusters, Overall Score")
    for stat in outputStats:
        line = str(stat[0]) + ", " + "{:.9f}".format(stat[1][0])
        print(line)
        for val in stat[1][1]:
            line = line + ", " + "{:.9f}".format(val)
        line = line + "\n"
        fileStats.write(line)  
        
    fileStats.close()
    print("Complete.\n")

def runShepherdSegmentationTestMinObjSize(inputImg, outputClumpsBase, outStatsFile, outputMeanImgBase=None, tmpath='.', gdalformat='KEA', noStats=False, noStretch=False, noDelete=False, numClusters=100, minPxlsStart=10, minPxlsStep=5, numOfMinPxlsSteps=20, distThres=1000000, bands=None, sampling=100, kmMaxIter=200, minNormV=None, maxNormV=None, minNormMI=None, maxNormMI=None): 
    """
Utility function to call the segmentation algorithm of Shepherd et al. (2019) and to test are range of 'k' within the kMeans.

Where:

:param inputImg: is a string containing the name of the input file
:param outputClumps: is a string containing the name of the output clump file
:param outStatsFile: is a string containing the name of the output CSV file with the image segmentation stats
:param outputMeanImg: is the output mean image file (clumps attributed with pixel mean from input image) - pass 'None' to skip creating.
:param tmpath: is a file path for intermediate files (default is current directory).
:param gdalformat: is a string containing the GDAL format for the output file (default is KEA)
:param noStats: is a bool which specifies that no image statistics and pyramids should be built for the output images.
:param noStretch: is a bool which specifies that the input image bands should not be stretched.
:param noDelete: is a book which specifies that the temporary images created during processing should not be deleted once processing has been completed.
:param numClusters: is an int which specifies the number of clusters within the KMeans clustering process
:param minPxlsStart: is an int which specifies the minimum number pixels within a segments at the start of processing.
:param minPxlsStep: is an int which specifies the minimum number pixels within a segments increment each step.
:param numOfMinPxlsSteps: is an int which specifies the number steps (i.e., tests) which are performed.
:param distThres: specifies the distance threshold for joining the segments (default is a very large value which turns off this option.).
:param bands: is an array providing a subset of image bands to use (default is None to use all bands)
:param sampling: specify the subsampling of the image for the data used within the KMeans (1 == no subsampling; default is 100)
:param kmMaxIter: maximum iterations for KMeans.
:param minNormV: is a floating point =None
:param maxNormV: None
:param minNormMI: None
:param maxNormMI: None

Example::

    from rsgislib.segmentation import segutils

    inputImg = './WV2_525N040W_20110727_TOARefl_b762_stch.kea'
    outputClumpsBase = './OptimalTests/WV2_525N040W_20110727_MinPxl'
    outputMeanImgBase = './OptimalTests/WV2_525N040W_20110727_MinPxlMean'
    tmpath='./OptimalTests/tmp/'
    outStatsFile = './OptimalTests/StatsMinPxl.csv'

    # Will test minimum number of pixels within an object from 10 to 100 with intervals of 5.
    segutils.runShepherdSegmentationTestMinObjSize(inputImg, outputClumpsBase, outStatsFile, outputMeanImgBase=outputMeanImgBase, tmpath=tmpath, noStretch=True, numClusters=100, minPxlsStart=5, minPxlsStep=5, numOfMinPxlsSteps=20, minNormV=None, maxNormV=None, minNormMI=None, maxNormMI=None)

    """
    colsPrefix = 'gs'
    calcNeighbours = True
    calcNormVals = False
    if minNormV==None or  maxNormV==None or minNormMI==None or maxNormMI==None:
        minNormV = 0.0
        maxNormV = 1.0
        minNormMI = 0.0
        maxNormMI = 1.0
        calcNormVals = True
    
    
    
    outputStats = list()
    
    minPxls = minPxlsStart
    for i in range(numOfMinPxlsSteps):
        minPxls = minPxlsStart + (i * minPxlsStep)
        print("Processing ", minPxls)
        outputClumps = outputClumpsBase + "_mp" + str(minPxls) + ".kea"
        outputMeanImg = outputMeanImgBase + "_mp" + str(minPxls) + ".kea"
        
        runShepherdSegmentation(inputImg, outputClumps, outputMeanImg=outputMeanImg, tmpath=tmpath, gdalformat=gdalformat, noStats=noStats, noStretch=noStretch, noDelete=noDelete, numClusters=numClusters, minPxls=minPxls, distThres=distThres, bands=bands, sampling=sampling, kmMaxIter=kmMaxIter)
        
        segScores = rsgislib.rastergis.calcGlobalSegmentationScore(outputClumps, inputImg, colsPrefix, calcNeighbours, minNormV, maxNormV, minNormMI, maxNormMI)
        
        tup = (minPxls, segScores)
        outputStats.append(tup)
        
    
    numImgBands = int(len(outputStats[0][1][1])/4)
    
    
    if calcNormVals:
        minVar = 0.0
        maxVar = 0.0
        scaleVar = 0.0
        minMI = 0.0
        maxMI = 0.0
        scaleMI = 0.0
        first = True
        
        for stat in outputStats:        
            for i in range(numImgBands):
                idxVar = i*4
                idxMI = (i*4)+1
                if first:
                    minVar = stat[1][1][idxVar] # Var Min
                    maxVar = stat[1][1][idxVar] # Var Max
                    minMI = stat[1][1][idxMI] # MI Min
                    maxMI = stat[1][1][idxMI] # MI Max
                    first = False
                else:
                    if stat[1][1][idxVar] < minVar:
                        minVar = stat[1][1][idxVar] # Set Var Min
                    elif stat[1][1][idxVar] > maxVar:
                        maxVar = stat[1][1][idxVar] # Set Var Max
                        
                    if stat[1][1][idxMI] < minMI:
                        minMI = stat[1][1][idxMI] # Set MI Min
                    elif stat[1][1][idxMI] > maxMI:
                        maxMI = stat[1][1][idxMI] # Set MI Max
        
        print("Var Min: ", minVar)
        print("Var Max: ", maxVar)
        print("MI Min: ", minMI)
        print("MI Max: ", maxMI)
    
        if (maxVar - minVar) > 0:
            scaleVar = 1/(maxVar - minVar)
        else:
            scaleVar = 1
        
        if (maxMI - minMI) > 0:
            scaleMI = 1/(maxMI - minMI)
        else:
            scaleMI = 1
    
        print("Var Scale: ", scaleVar)
        print("MI Scale: ", scaleMI)
    
        for stat in outputStats:
            gScore = 0.0
            for i in range(numImgBands):
                idxVar = i*4
                idxMI = (i*4) + 1
                idxVarNorm = (i*4) + 2
                idxMINorm = (i*4) + 3
                
                stat[1][1][idxVarNorm] = (stat[1][1][idxVar] - minVar) * scaleVar
                stat[1][1][idxMINorm] = (stat[1][1][idxMI] - minMI) * scaleMI
            
                gScore = gScore + stat[1][1][idxVarNorm] + stat[1][1][idxMINorm]
            stat[1][0] = gScore
    
    fileStats = open(outStatsFile, "w")
    
    colNames = "MinNumPxls, Overall Score"
    for i in range(numImgBands):
        colNames = colNames + ", B" + str(i+1) + "_Variance"
        colNames = colNames + ", B" + str(i+1) + "_MI"
        colNames = colNames + ", B" + str(i+1) + "_VarianceNorm"
        colNames = colNames + ", B" + str(i+1) + "_MINorm"
    colNames = colNames + "\n"
    
    fileStats.write(colNames)
    
    print("Clusters, Overall Score")
    for stat in outputStats:
        line = str(stat[0]) + ", " + "{:.9f}".format(stat[1][0])
        print(line)
        for val in stat[1][1]:
            line = line + ", " + "{:.9f}".format(val)
        line = line + "\n"
        fileStats.write(line)  
        
    fileStats.close()
    print("Complete.\n")

