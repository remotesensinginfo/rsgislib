#! /usr/bin/env python

############################################################################
#  tiledclump.py
#
#  Copyright 2016 RSGISLib.
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
#
# Purpose:  Provide functionality to clump an image using image tiles.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 21/3/2016
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

import glob
import os.path
import os
import shutil
from multiprocessing import Pool
import multiprocessing

import rsgislib
from rsgislib import segmentation
from rsgislib import imageutils

################################ Clumping Functions ################################

def performClumpingSingleThread(inputImage, clumpsImage, tmpDIR='tmp', width=2000, height=2000, gdalformat='KEA'):
    """
Clump the input image using a tiled processing chain allowing large images to be clumped more quickly.

:param inputImage: the input image to be clumped.
:param clumpsImage: the output clumped image.
:param tmpDIR: the temporary directory where intermediate files will be written (default is 'tmp'). Directory will be created and deleted if does not exist.
:param width: int for width of the image tiles used for processing (Default = 2000).
:param height: int for height of the image tiles used for processing (Default = 2000).
:param gdalformat: string with the GDAL image format for the output image (Default = KEA). NOTE. KEA is used as intermediate format internally and therefore needs to be available.

    """
    createdTmp = False
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
        createdTmp = True
    
    rsgisUtils = rsgislib.RSGISPyUtils()
    uidStr = rsgisUtils.uidGenerator()
    dataType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImage)
    baseName = os.path.splitext(os.path.basename(inputImage))[0]+"_"+uidStr
    imgTilesDIR = os.path.join(tmpDIR, "imgtiles_"+uidStr)
    tilesClumpsDIR = os.path.join(tmpDIR, "imgclumpstiles_"+uidStr)
    tilesImgBase = os.path.join(imgTilesDIR, baseName)
    initMergedClumps = os.path.join(tmpDIR, "MergedInitClumps_"+uidStr+".kea")
    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
    if not os.path.exists(tilesClumpsDIR):
        os.makedirs(tilesClumpsDIR)
    
    imageutils.createTiles(inputImage, tilesImgBase, int(width), int(height), 0, False, 'KEA', dataType, 'kea')
    imageTiles = glob.glob(tilesImgBase+"*")

    for tile in imageTiles:
        tilBaseName = os.path.splitext(os.path.basename(tile))[0]
        clumpedTile = os.path.join(tilesClumpsDIR, tilBaseName+'_clumps.kea')
        segmentation.clump(tile, clumpedTile, 'KEA', True, 0, True)
    
    clumpTiles = glob.glob(os.path.join(tilesClumpsDIR, '*_clumps.kea'))
    print("Create Blank Image")
    imageutils.createCopyImage(inputImage, initMergedClumps, 1, 0, 'KEA', rsgislib.TYPE_32UINT)
    print("Merge Tiles into Blank Image")
    segmentation.mergeClumpImages(clumpTiles, initMergedClumps, True)
    print("Merge Tile Boundaries")
    segmentation.mergeEquivClumps(initMergedClumps, clumpsImage, gdalformat, ['PixelVal'])
    
    shutil.rmtree(imgTilesDIR)
    shutil.rmtree(tilesClumpsDIR)
    os.remove(initMergedClumps)
    if createdTmp:
        shutil.rmtree(tmpDIR)


def clumpImgFunc(imgs):
    """
    Clump an image with values provides as an array for use within a multiprocessing Pool
    """
    segmentation.clump(imgs[0], imgs[1], 'KEA', True, 0, True)


def performClumpingMultiProcess(inputImage, clumpsImage, tmpDIR='tmp', width=2000, height=2000, gdalformat='KEA', nCores=-1):
    """
Clump the input image using a tiled processing chain allowing large images to be clumped more quickly.

:param inputImage: the input image to be clumped.
:param clumpsImage: the output clumped image.
:param tmpDIR: the temporary directory where intermediate files will be written (default is 'tmp'). Directory will be created and deleted if does not exist.
:param width: int for width of the image tiles used for processing (Default = 2000).
:param height: int for height of the image tiles used for processing (Default = 2000).
:param gdalformat: string with the GDAL image format for the output image (Default = KEA). NOTE. KEA is used as intermediate format internally and therefore needs to be available.
:param nCores: is an int specifying the number of cores to be used for clumping processing.

    """
    createdTmp = False
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
        createdTmp = True
    
    if nCores <= 0:
        nCores = multiprocessing.cpu_count()
    
    rsgisUtils = rsgislib.RSGISPyUtils()
    uidStr = rsgisUtils.uidGenerator()
    dataType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImage)
    baseName = os.path.splitext(os.path.basename(inputImage))[0]+"_"+uidStr
    imgTilesDIR = os.path.join(tmpDIR, "imgtiles_"+uidStr)
    tilesClumpsDIR = os.path.join(tmpDIR, "imgclumpstiles_"+uidStr)
    tilesImgBase = os.path.join(imgTilesDIR, baseName)
    initMergedClumps = os.path.join(tmpDIR, "MergedInitClumps_"+uidStr+".kea")
    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
    if not os.path.exists(tilesClumpsDIR):
        os.makedirs(tilesClumpsDIR)
    
    imageutils.createTiles(inputImage, tilesImgBase, int(width), int(height), 0, False, 'KEA', dataType, 'kea')
    imageTiles = glob.glob(tilesImgBase+"*")
    
    clumpImgsVals = []
    for tile in imageTiles:
        tilBaseName = os.path.splitext(os.path.basename(tile))[0]
        clumpedTile = os.path.join(tilesClumpsDIR, tilBaseName+'_clumps.kea')
        clumpImgsVals.append([tile, clumpedTile])
    
    with Pool(nCores) as p:
        p.map(clumpImgFunc, clumpImgsVals)
    
    clumpTiles = glob.glob(os.path.join(tilesClumpsDIR, '*_clumps.kea'))
    print("Create Blank Image")
    imageutils.createCopyImage(inputImage, initMergedClumps, 1, 0, 'KEA', rsgislib.TYPE_32UINT)
    print("Merge Tiles into Blank Image")
    segmentation.mergeClumpImages(clumpTiles, initMergedClumps, True)
    print("Merge Tile Boundaries")
    segmentation.mergeEquivClumps(initMergedClumps, clumpsImage, gdalformat, ['PixelVal'])
    
    shutil.rmtree(imgTilesDIR)
    shutil.rmtree(tilesClumpsDIR)
    os.remove(initMergedClumps)
    if createdTmp:
        shutil.rmtree(tmpDIR)
################################################################################################


################################ Union Clumping Functions ################################

def performUnionClumpingSingleThread(inputImage, refImg, clumpsImage, tmpDIR='tmp', width=2000, height=2000, gdalformat='KEA'):
    """
Clump and union with the reference image the input image using a tiled processing chain allowing large images to be clumped more quickly.

:param inputImage: the input image to be clumped.
:param refImg: the reference image which the union is undertaken with (typically an existing classification)
:param clumpsImage: the output clumped image.
:param tmpDIR: the temporary directory where intermediate files will be written (default is 'tmp'). Directory will be created and deleted if does not exist.
:param width: int for width of the image tiles used for processing (Default = 2000).
:param height: int for height of the image tiles used for processing (Default = 2000).
:param gdalformat: string with the GDAL image format for the output image (Default = KEA). NOTE. KEA is used as intermediate format internally and therefore needs to be available.

    """
    createdTmp = False
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
        createdTmp = True
    
    rsgisUtils = rsgislib.RSGISPyUtils()
    uidStr = rsgisUtils.uidGenerator()
    dataType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImage)
    baseName = os.path.splitext(os.path.basename(inputImage))[0]+"_"+uidStr
    imgTilesDIR = os.path.join(tmpDIR, "imgtiles_"+uidStr)
    tilesClumpsDIR = os.path.join(tmpDIR, "imgclumpstiles_"+uidStr)
    tilesImgBase = os.path.join(imgTilesDIR, baseName)
    initMergedClumps = os.path.join(tmpDIR, "MergedInitClumps_"+uidStr+".kea")
    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
    if not os.path.exists(tilesClumpsDIR):
        os.makedirs(tilesClumpsDIR)
    
    imageutils.createTiles(inputImage, tilesImgBase, int(width), int(height), 0, False, 'KEA', dataType, 'kea')
    imageTiles = glob.glob(tilesImgBase+"*")

    for tile in imageTiles:
        tilBaseName = os.path.splitext(os.path.basename(tile))[0]
        clumpedTile = os.path.join(tilesClumpsDIR, tilBaseName+'_clumps.kea')
        segmentation.unionOfClumps(clumpedTile, 'KEA', [tile, refImg], 0, True)
    
    clumpTiles = glob.glob(os.path.join(tilesClumpsDIR, '*_clumps.kea'))
    print("Create Blank Image")
    imageutils.createCopyImage(inputImage, initMergedClumps, 1, 0, 'KEA', rsgislib.TYPE_32UINT)
    print("Merge Tiles into Blank Image")
    segmentation.mergeClumpImages(clumpTiles, initMergedClumps, True)
    print("Merge Tile Boundaries")
    segmentation.mergeEquivClumps(initMergedClumps, clumpsImage, gdalformat, ['ClumpVal_1', 'ClumpVal_2'])
    
    shutil.rmtree(imgTilesDIR)
    shutil.rmtree(tilesClumpsDIR)
    os.remove(initMergedClumps)
    if createdTmp:
        shutil.rmtree(tmpDIR)

def unionClumpImgFunc(imgs):
    """
    Union Clump an image with values provides as an array for use within a multiprocessing Pool
    """
    segmentation.unionOfClumps(imgs[2], 'KEA', [imgs[0], imgs[1]], 0, True)
    
def performUnionClumpingMultiProcess(inputImage, refImg, clumpsImage, tmpDIR='tmp', width=2000, height=2000, gdalformat='KEA', nCores=-1):
    """
Clump and union with the reference image the input image using a tiled processing chain allowing large images to be clumped more quickly.

:param inputImage: the input image to be clumped.
:param refImg: the reference image which the union is undertaken with (typically an existing classification)
:param clumpsImage: the output clumped image.
:param tmpDIR: the temporary directory where intermediate files will be written (default is 'tmp'). Directory will be created and deleted if does not exist.
:param width: int for width of the image tiles used for processing (Default = 2000).
:param height: int for height of the image tiles used for processing (Default = 2000).
:param gdalformat: string with the GDAL image format for the output image (Default = KEA). NOTE. KEA is used as intermediate format internally and therefore needs to be available.
:param nCores: is an int specifying the number of cores to be used for clumping processing.

    """
    createdTmp = False
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
        createdTmp = True
    
    if nCores <= 0:
        nCores = multiprocessing.cpu_count()
    
    rsgisUtils = rsgislib.RSGISPyUtils()
    uidStr = rsgisUtils.uidGenerator()
    dataType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImage)
    baseName = os.path.splitext(os.path.basename(inputImage))[0]+"_"+uidStr
    imgTilesDIR = os.path.join(tmpDIR, "imgtiles_"+uidStr)
    tilesClumpsDIR = os.path.join(tmpDIR, "imgclumpstiles_"+uidStr)
    tilesImgBase = os.path.join(imgTilesDIR, baseName)
    initMergedClumps = os.path.join(tmpDIR, "MergedInitClumps_"+uidStr+".kea")
    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
    if not os.path.exists(tilesClumpsDIR):
        os.makedirs(tilesClumpsDIR)
    
    imageutils.createTiles(inputImage, tilesImgBase, int(width), int(height), 0, False, 'KEA', dataType, 'kea')
    imageTiles = glob.glob(tilesImgBase+"*")
    
    clumpImgsVals = []
    for tile in imageTiles:
        tilBaseName = os.path.splitext(os.path.basename(tile))[0]
        clumpedTile = os.path.join(tilesClumpsDIR, tilBaseName+'_clumps.kea')
        clumpImgsVals.append([tile, refImg, clumpedTile])
    
    with Pool(nCores) as p:
        p.map(unionClumpImgFunc, clumpImgsVals)
    
    clumpTiles = glob.glob(os.path.join(tilesClumpsDIR, '*_clumps.kea'))
    print("Create Blank Image")
    imageutils.createCopyImage(inputImage, initMergedClumps, 1, 0, 'KEA', rsgislib.TYPE_32UINT)
    print("Merge Tiles into Blank Image")
    segmentation.mergeClumpImages(clumpTiles, initMergedClumps, True)
    print("Merge Tile Boundaries")
    segmentation.mergeEquivClumps(initMergedClumps, clumpsImage, gdalformat, ['ClumpVal_1', 'ClumpVal_2'])
    
    shutil.rmtree(imgTilesDIR)
    shutil.rmtree(tilesClumpsDIR)
    os.remove(initMergedClumps)
    if createdTmp:
        shutil.rmtree(tmpDIR)



