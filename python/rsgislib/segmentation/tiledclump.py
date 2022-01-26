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


def perform_clumping_single_thread(
    input_img, clumps_img, tmp_dir="tmp", width=2000, height=2000, gdalformat="KEA"
):
    """
    Clump the input image using a tiled processing chain allowing large images to
    be clumped more quickly.

    :param input_img: the input image to be clumped.
    :param clumps_img: the output clumped image.
    :param tmp_dir: the temporary directory where intermediate files will be
                    written (default is 'tmp'). Directory will be created and
                    deleted if does not exist.
    :param width: int for width of the image tiles used for
                  processing (Default = 2000).
    :param height: int for height of the image tiles used for
                   processing (Default = 2000).
    :param gdalformat: string with the GDAL image format for the output image
                       (Default = KEA). NOTE. KEA is used as intermediate format
                       internally and therefore needs to be available.

    """
    import rsgislib.tools.utils
    import rsgislib.tools.filetools

    createdTmp = False
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        createdTmp = True

    uidStr = rsgislib.tools.utils.uid_generator()
    dataType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
    baseName = os.path.splitext(os.path.basename(input_img))[0] + "_" + uidStr
    imgTilesDIR = os.path.join(tmp_dir, "imgtiles_" + uidStr)
    tilesClumpsDIR = os.path.join(tmp_dir, "imgclumpstiles_" + uidStr)
    tilesImgBase = os.path.join(imgTilesDIR, baseName)
    initMergedClumps = os.path.join(tmp_dir, "MergedInitClumps_" + uidStr + ".kea")
    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
    if not os.path.exists(tilesClumpsDIR):
        os.makedirs(tilesClumpsDIR)

    imageutils.create_tiles(
        input_img,
        tilesImgBase,
        int(width),
        int(height),
        0,
        False,
        "KEA",
        dataType,
        "kea",
    )
    imageTiles = glob.glob(tilesImgBase + "*")

    for tile in imageTiles:
        tilBaseName = os.path.splitext(os.path.basename(tile))[0]
        clumpedTile = os.path.join(tilesClumpsDIR, tilBaseName + "_clumps.kea")
        segmentation.clump(tile, clumpedTile, "KEA", True, 0, True)

    clumpTiles = glob.glob(os.path.join(tilesClumpsDIR, "*_clumps.kea"))
    print("Create Blank Image")
    imageutils.create_copy_img(
        input_img, initMergedClumps, 1, 0, "KEA", rsgislib.TYPE_32UINT
    )
    print("Merge Tiles into Blank Image")
    segmentation.merge_clump_images(clumpTiles, initMergedClumps, True)
    print("Merge Tile Boundaries")
    segmentation.merge_equiv_clumps(
        initMergedClumps, clumps_img, gdalformat, ["PixelVal"]
    )

    shutil.rmtree(imgTilesDIR)
    shutil.rmtree(tilesClumpsDIR)
    rsgislib.tools.filetools.delete_file_silent(initMergedClumps)
    if createdTmp:
        shutil.rmtree(tmp_dir)


def _clump_img_func(imgs):
    """
    Clump an image with values provides as an array for use within a multiprocessing Pool
    """
    segmentation.clump(imgs[0], imgs[1], "KEA", True, 0, True)


def perform_clumping_multi_process(
    input_img,
    clumps_img,
    tmp_dir="tmp",
    width=2000,
    height=2000,
    gdalformat="KEA",
    n_cores=-1,
):
    """
    Clump the input image using a tiled processing chain allowing large images
    to be clumped more quickly.

    :param input_img: the input image to be clumped.
    :param clumps_img: the output clumped image.
    :param tmp_dir: the temporary directory where intermediate files will be
                    written (default is 'tmp'). Directory will be created and
                    deleted if does not exist.
    :param width: int for width of the image tiles used for
                  processing (Default = 2000).
    :param height: int for height of the image tiles used for
                   processing (Default = 2000).
    :param gdalformat: string with the GDAL image format for the output image
                       (Default = KEA). NOTE. KEA is used as intermediate format
                       internally and therefore needs to be available.
    :param n_cores: is an int specifying the number of cores to be used for
                    clumping processing.

    """
    import rsgislib.tools.utils
    import rsgislib.tools.filetools

    createdTmp = False
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        createdTmp = True

    if n_cores <= 0:
        n_cores = multiprocessing.cpu_count()

    uidStr = rsgislib.tools.utils.uid_generator()
    dataType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
    baseName = os.path.splitext(os.path.basename(input_img))[0] + "_" + uidStr
    imgTilesDIR = os.path.join(tmp_dir, "imgtiles_" + uidStr)
    tilesClumpsDIR = os.path.join(tmp_dir, "imgclumpstiles_" + uidStr)
    tilesImgBase = os.path.join(imgTilesDIR, baseName)
    initMergedClumps = os.path.join(tmp_dir, "MergedInitClumps_" + uidStr + ".kea")
    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
    if not os.path.exists(tilesClumpsDIR):
        os.makedirs(tilesClumpsDIR)

    imageutils.create_tiles(
        input_img,
        tilesImgBase,
        int(width),
        int(height),
        0,
        False,
        "KEA",
        dataType,
        "kea",
    )
    imageTiles = glob.glob(tilesImgBase + "*")

    clumpImgsVals = []
    for tile in imageTiles:
        tilBaseName = os.path.splitext(os.path.basename(tile))[0]
        clumpedTile = os.path.join(tilesClumpsDIR, tilBaseName + "_clumps.kea")
        clumpImgsVals.append([tile, clumpedTile])

    with Pool(n_cores) as p:
        p.map(_clump_img_func, clumpImgsVals)

    clumpTiles = glob.glob(os.path.join(tilesClumpsDIR, "*_clumps.kea"))
    print("Create Blank Image")
    imageutils.create_copy_img(
        input_img, initMergedClumps, 1, 0, "KEA", rsgislib.TYPE_32UINT
    )
    print("Merge Tiles into Blank Image")
    segmentation.merge_clump_images(clumpTiles, initMergedClumps, True)
    print("Merge Tile Boundaries")
    segmentation.merge_equiv_clumps(
        initMergedClumps, clumps_img, gdalformat, ["PixelVal"]
    )

    shutil.rmtree(imgTilesDIR)
    shutil.rmtree(tilesClumpsDIR)
    rsgislib.tools.filetools.delete_file_silent(initMergedClumps)
    if createdTmp:
        shutil.rmtree(tmp_dir)


################################################################################################


################################ Union Clumping Functions ################################


def perform_union_clumping_single_thread(
    input_img,
    in_ref_img,
    clumps_img,
    tmp_dir="tmp",
    width=2000,
    height=2000,
    gdalformat="KEA",
):
    """
    Clump and union with the reference image the input image using a tiled processing
    chain allowing large images to be clumped more quickly.

    :param input_img: the input image to be clumped.
    :param in_ref_img: the reference image which the union is undertaken with
                       (typically an existing classification)
    :param clumps_img: the output clumped image.
    :param tmp_dir: the temporary directory where intermediate files will be written
                    (default is 'tmp'). Directory will be created and deleted if
                    does not exist.
    :param width: int for width of the image tiles used for
                  processing (Default = 2000).
    :param height: int for height of the image tiles used for
                   processing (Default = 2000).
    :param gdalformat: string with the GDAL image format for the output image
                       (Default = KEA). NOTE. KEA is used as intermediate format
                       internally and therefore needs to be available.

    """
    import rsgislib.tools.utils
    import rsgislib.tools.filetools

    createdTmp = False
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        createdTmp = True

    uidStr = rsgislib.tools.utils.uid_generator()
    dataType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
    baseName = os.path.splitext(os.path.basename(input_img))[0] + "_" + uidStr
    imgTilesDIR = os.path.join(tmp_dir, "imgtiles_" + uidStr)
    tilesClumpsDIR = os.path.join(tmp_dir, "imgclumpstiles_" + uidStr)
    tilesImgBase = os.path.join(imgTilesDIR, baseName)
    initMergedClumps = os.path.join(tmp_dir, "MergedInitClumps_" + uidStr + ".kea")
    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
    if not os.path.exists(tilesClumpsDIR):
        os.makedirs(tilesClumpsDIR)

    imageutils.create_tiles(
        input_img,
        tilesImgBase,
        int(width),
        int(height),
        0,
        False,
        "KEA",
        dataType,
        "kea",
    )
    imageTiles = glob.glob(tilesImgBase + "*")

    for tile in imageTiles:
        tilBaseName = os.path.splitext(os.path.basename(tile))[0]
        clumpedTile = os.path.join(tilesClumpsDIR, tilBaseName + "_clumps.kea")
        segmentation.union_of_clumps([tile, in_ref_img], clumpedTile, "KEA", 0, True)

    clumpTiles = glob.glob(os.path.join(tilesClumpsDIR, "*_clumps.kea"))
    print("Create Blank Image")
    imageutils.create_copy_img(
        input_img, initMergedClumps, 1, 0, "KEA", rsgislib.TYPE_32UINT
    )
    print("Merge Tiles into Blank Image")
    segmentation.merge_clump_images(clumpTiles, initMergedClumps, True)
    print("Merge Tile Boundaries")
    segmentation.merge_equiv_clumps(
        initMergedClumps, clumps_img, gdalformat, ["ClumpVal_1", "ClumpVal_2"]
    )

    shutil.rmtree(imgTilesDIR)
    shutil.rmtree(tilesClumpsDIR)
    rsgislib.tools.filetools.delete_file_silent(initMergedClumps)
    if createdTmp:
        shutil.rmtree(tmp_dir)


def _union_clump_img_func(imgs):
    """
    Union Clump an image with values provides as an array for use
    within a multiprocessing Pool
    """
    segmentation.union_of_clumps([imgs[0], imgs[1]], imgs[2], "KEA", 0, True)


def perform_union_clumping_multi_process(
    input_img,
    in_ref_img,
    clumps_img,
    tmp_dir="tmp",
    width=2000,
    height=2000,
    gdalformat="KEA",
    n_cores=-1,
):
    """
    Clump and union with the reference image the input image using a tiled
    processing chain allowing large images to be clumped more quickly.

    :param input_img: the input image to be clumped.
    :param in_ref_img: the reference image which the union is undertaken
                       with (typically an existing classification)
    :param clumps_img: the output clumped image.
    :param tmp_dir: the temporary directory where intermediate files will be
                    written (default is 'tmp'). Directory will be created and
                    deleted if does not exist.
    :param width: int for width of the image tiles used for
                  processing (Default = 2000).
    :param height: int for height of the image tiles used for
                   processing (Default = 2000).
    :param gdalformat: string with the GDAL image format for the output image
                       (Default = KEA). NOTE. KEA is used as intermediate format
                       internally and therefore needs to be available.
    :param n_cores: is an int specifying the number of cores to be used for
                    clumping processing.

    """
    import rsgislib.tools.utils
    import rsgislib.tools.filetools

    createdTmp = False
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        createdTmp = True

    if n_cores <= 0:
        n_cores = multiprocessing.cpu_count()

    uidStr = rsgislib.tools.utils.uid_generator()
    dataType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
    baseName = os.path.splitext(os.path.basename(input_img))[0] + "_" + uidStr
    imgTilesDIR = os.path.join(tmp_dir, "imgtiles_" + uidStr)
    tilesClumpsDIR = os.path.join(tmp_dir, "imgclumpstiles_" + uidStr)
    tilesImgBase = os.path.join(imgTilesDIR, baseName)
    initMergedClumps = os.path.join(tmp_dir, "MergedInitClumps_" + uidStr + ".kea")
    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
    if not os.path.exists(tilesClumpsDIR):
        os.makedirs(tilesClumpsDIR)

    imageutils.create_tiles(
        input_img,
        tilesImgBase,
        int(width),
        int(height),
        0,
        False,
        "KEA",
        dataType,
        "kea",
    )
    imageTiles = glob.glob(tilesImgBase + "*")

    clumpImgsVals = []
    for tile in imageTiles:
        tilBaseName = os.path.splitext(os.path.basename(tile))[0]
        clumpedTile = os.path.join(tilesClumpsDIR, tilBaseName + "_clumps.kea")
        clumpImgsVals.append([tile, in_ref_img, clumpedTile])

    with Pool(n_cores) as p:
        p.map(_union_clump_img_func, clumpImgsVals)

    clumpTiles = glob.glob(os.path.join(tilesClumpsDIR, "*_clumps.kea"))
    print("Create Blank Image")
    imageutils.create_copy_img(
        input_img, initMergedClumps, 1, 0, "KEA", rsgislib.TYPE_32UINT
    )
    print("Merge Tiles into Blank Image")
    segmentation.merge_clump_images(clumpTiles, initMergedClumps, True)
    print("Merge Tile Boundaries")
    segmentation.merge_equiv_clumps(
        initMergedClumps, clumps_img, gdalformat, ["ClumpVal_1", "ClumpVal_2"]
    )

    shutil.rmtree(imgTilesDIR)
    shutil.rmtree(tilesClumpsDIR)
    rsgislib.tools.filetools.delete_file_silent(initMergedClumps)
    if createdTmp:
        shutil.rmtree(tmp_dir)
