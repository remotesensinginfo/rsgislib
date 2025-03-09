#!/usr/bin/env python
"""
These utilities allow for a more 'intelligent tiling process to be carried out:

.. code:: python

    import rsgislib
    from rsgislib.imageutils import tilingutils

    inputImage = 'LS5TM_20110428_sref_submask_osgb.kea'
    tileShp = 'LS5TM_20110428_sref_submask_osgb_tiles.shp'
    outTilesMaskBase = 'tilesmeta/LS5TM_20110428_sref_submask_osgb_tile'
    outTilesImgBase = 'tiles/LS5TM_20110428_sref_submask_osgb_tile'
    width = 2500
    height = 2500
    validDataThreshold = 0.3

    tilingutils.create_min_data_tiles(inputImage, tileShp, width, height, validDataThreshold)
    tilingutils.createTileMaskImages(inputImage, tileShp, outTilesMaskBase)
    tilingutils.create_tiles_from_masks(inputImage, outTilesMaskBase, outTilesImgBase, rsgislib.TYPE_16UINT, 'KEA')

"""
############################################################################
#  tilingutils.py
#
#  Copyright 2015 RSGISLib.
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
# Date: 02/04/2015
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
############################################################################

import glob
import math
import os.path
import shutil
import sys

import rsgislib
import rsgislib.tools.filetools
from rsgislib import imageutils, rastergis, segmentation, vectorutils

haveGDALPy = True
try:
    from osgeo import gdal, ogr
except ImportError as gdalErr:
    haveGDALPy = False

haveRIOS = True
try:
    from rios import rat
except ImportError as riosErr:
    haveRIOS = False

haveNumpy = True
try:
    import numpy
except ImportError as numpyErr:
    haveNumpy = False


def create_min_data_tiles(
    inputImage,
    outshp,
    outclumpsFile,
    width,
    height,
    validDataThreshold,
    maskIntersect=None,
    offset=False,
    force=True,
    tmpdir="tilestemp",
    inImgNoDataVal=0.0,
):
    """
    A function to create a tiling for an input image where each tile has a minimum amount of valid data.

    :param inputImage: is a string for the image to be tiled
    :param outshp: is a string for the output shapefile the tiling will be written to (if None a shapefile won't be outputted).
    :param outclumpsFile: is a string for the output image file containing the tiling
    :param width: is an int for the width of the tiles
    :param height: is an int for the height of the tiles
    :param validDataThreshold: is a float (0-1) with the proportion of valid data needed within a tile.
    :param force: is a boolean (default True) to delete the output shapefile if it already exists.
    :param tmpdir: is a string with a temporary directory for temp outputs to be stored (they will be deleted).
                  if tmpdir doesn't exist it will be created and then deleted during the processing.
    :param inImgNoDataVal: is a float for providing the input image no data value (Default: 0.0)
    """
    tmpPresent = True
    if not os.path.exists(tmpdir):
        print("WARNING: tmpdir directory does not exist so creating it...")
        os.makedirs(tmpdir)
        tmpPresent = False

    inImgBaseName = os.path.splitext(os.path.basename(inputImage))[0]

    tileClumpsImage = os.path.join(tmpdir, inImgBaseName + "_tilesimg.kea")

    segmentation.generateRegularGrid(
        inputImage, tileClumpsImage, "KEA", width, height, offset
    )
    rastergis.pop_rat_img_stats(tileClumpsImage, True, True)

    if maskIntersect is not None:
        bs = []
        bs.append(rastergis.BandAttStats(band=1, maxField="Mask"))
        rastergis.populate_rat_with_stats(maskIntersect, tileClumpsImage, bs)
        ratDS = gdal.Open(tileClumpsImage, gdal.GA_Update)
        MaskField = rat.readColumn(ratDS, "Mask")
        Selected = numpy.zeros_like(MaskField, dtype=int)
        Selected[MaskField == inImgNoDataVal] = 1
        rat.writeColumn(ratDS, "Selected", Selected)
        ratDS = None

        tileClumpsImageDropClumps = os.path.join(
            tmpdir, inImgBaseName + "_tilesimgdropped.kea"
        )

        segmentation.dropSelectedClumps(
            tileClumpsImage, tileClumpsImageDropClumps, "KEA", "Selected"
        )
        rsgislib.tools.filetools.delete_file_silent(tileClumpsImage)
        tileClumpsImage = tileClumpsImageDropClumps

    rastergis.populateRATWithPropValidPxls(
        inputImage, tileClumpsImage, "ValidPxls", inImgNoDataVal
    )

    ratDS = gdal.Open(tileClumpsImage, gdal.GA_Update)
    ValidPxls = rat.readColumn(ratDS, "ValidPxls")
    Selected = numpy.zeros_like(ValidPxls, dtype=int)
    NoDataClumps = numpy.zeros_like(ValidPxls, dtype=int)
    Selected[ValidPxls < validDataThreshold] = 1
    NoDataClumps[ValidPxls == 0] = 1
    Selected[ValidPxls == 0] = 0
    rat.writeColumn(ratDS, "Selected", Selected)
    rat.writeColumn(ratDS, "NoDataClumps", NoDataClumps)
    ratDS = None

    segmentation.mergeSegments2Neighbours(
        tileClumpsImage, inputImage, outclumpsFile, "KEA", "Selected", "NoDataClumps"
    )

    if outshp is not None:
        tilesDS = gdal.Open(outclumpsFile, gdal.GA_ReadOnly)
        tilesDSBand = tilesDS.GetRasterBand(1)

        dst_layername = os.path.splitext(os.path.basename(outshp))[0]
        # print(dst_layername)
        drv = ogr.GetDriverByName("ESRI Shapefile")

        if force and os.path.exists(outshp):
            drv.DeleteDataSource(outshp)

        dst_ds = drv.CreateDataSource(outshp)
        dst_layer = dst_ds.CreateLayer(dst_layername, srs=None)

        gdal.Polygonize(tilesDSBand, tilesDSBand, dst_layer, -1, [], callback=None)

        tilesDS = None
        dst_ds = None

    if not tmpPresent:
        shutil.rmtree(tmpdir, ignore_errors=True)
    else:
        rsgislib.tools.filetools.delete_file_silent(tileClumpsImage)


def create_tile_mask_images_from_shp(
    inputImage,
    tileShp,
    tilesNameBase,
    tilesMaskDIR,
    tmpdir="tilestemp",
    imgFormat="KEA",
):
    """
    A function to create individual image masks from the tiles shapefile which can be
    individually used to mask (using rsgislib mask function) each tile from the inputimage.

    :param inputImage: is the input image being tiled.
    :param tileShp: is a shapefile containing the shapefile tiles.
    :param tilesNameBase: is the base file name for the tile masks
    :param tilesMaskDIR: is the directory where the output images will be outputted
    :param tmpdir: is a string with a temporary directory for temp outputs to be stored (they will be deleted)
                   If tmpdir doesn't exist it will be created and then deleted during the processing.
    """

    tmpPresent = True
    if not os.path.exists(tmpdir):
        print("WARNING: tmpdir directory does not exist so creating it...")
        os.makedirs(tmpdir)
        tmpPresent = False

    shpTilesBase = os.path.join(tmpdir, tilesNameBase + "_tileshp")

    vectorutils.splitFeatures(tileShp, shpTilesBase, True)

    drv = ogr.GetDriverByName("ESRI Shapefile")
    shpFiles = glob.glob(shpTilesBase + "*.shp")
    idx = 1
    for shpFile in shpFiles:
        imgTileFile = os.path.join(tilesMaskDIR, tilesNameBase + str(idx) + ".kea")
        shpFileLyr = os.path.splitext(os.path.basename(shpFile))[0]
        rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
            shpFile,
            shpFileLyr,
            inputImage,
            imgTileFile,
            gdalformat=imgFormat,
            burn_val=1,
            datatype=rsgislib.TYPE_8UINT,
            att_column=None,
            use_vec_extent=False,
            thematic=True,
            no_data_val=0,
        )
        drv.DeleteDataSource(shpFile)
        idx = idx + 1

    if not tmpPresent:
        shutil.rmtree(tmpdir, ignore_errors=True)


def create_tile_mask_images_from_clumps(
    clumpsImage, tilesNameBase, tilesMaskDIR, gdalformat="KEA"
):
    """
    A function to create individual image masks from the tiles shapefile which can be
    individually used to mask (using rsgislib mask function) each tile from the inputimage.

    :param clumpsImage: is an image file with RAT where each clump represented a tile region.
    :param tilesNameBase: is the base file name for the tile masks
    :param tilesMaskDIR: is the directory where the output images will be outputted
    :param gdalformat: is the output image file format of the tile masks
    """
    import rsgislib.tools.filetools

    outBaseImg = os.path.join(tilesMaskDIR, tilesNameBase)
    outImgExt = rsgislib.imageutils.get_file_img_extension(gdalformat)[1:]
    rastergis.exportClumps2Images(
        clumpsImage, outBaseImg, True, outImgExt, gdalformat, 1
    )


def create_tiles_from_masks(
    inputImage, tilesBase, tilesMetaDIR, tilesImgDIR, datatype, gdalformat
):
    """
    A function to apply the image tile masks defined in createTileMaskImages to the input image to extract the individual tiles.

    :param inputImage: is the input image being tiled.
    :param tileMasksBase: is the base path for the tile masks. glob will be used to find them with \*.kea added to the end.
    :param outTilesBase: is the base file name for the tiles.

    """
    maskFiles = glob.glob(os.path.join(tilesMetaDIR, tilesBase + "*.kea"))

    idx = 1
    for maskFile in maskFiles:
        tileImage = os.path.join(tilesImgDIR, os.path.basename(maskFile))
        imageutils.mask_img(inputImage, maskFile, tileImage, gdalformat, datatype, 0, 0)
        imageutils.pop_img_stats(tileImage, True, 0.0, True)
