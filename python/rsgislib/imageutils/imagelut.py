#!/usr/bin/env python
"""
These utilities allow for easier use of the Look Up Table (LUT) functionality within
RSGISLib.
"""
############################################################################
#  imagelut.py
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
#
# Purpose:  Provide a set of utilities for working with image LUTs in RSGISLib
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 23/10/2019
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
############################################################################

import rsgislib
import os.path

haveGDALPy = True
try:
    import osgeo.gdal as gdal, ogr
except ImportError as gdalErr:
    haveGDALPy = False


def createImgExtentLUT(imgList, vectorFile, vectorLyr, vecDriver, ignore_none_imgs=False, out_proj_wgs84=False,
                       overwrite_lut_file=False):
    """
    Create a vector layer look up table (LUT) for a directory of images.

    This function is the same as rsgislib.vectorutils.createImgExtentLUT and just calls that function but
    is here as it might be a more logical place for people to find this functionality.

    :param imgList: list of input images for the LUT. All input images should be the same projection/coordinate system.
    :param vectorFile: output vector file/path
    :param vectorLyr: output vector layer
    :param vecDriver: the output vector layer type (e.g., GPKG).
    :param ignore_none_imgs: if a NULL epsg is returned from an image then ignore and don't include in LUT else throw exception.
    :param out_proj_wgs84: if True then the image bounding boxes will be re-projected to EPSG:4326.
    :param overwrite_lut_file: if True then output file will be overwritten. If false then not, e.g., can add extra layer to GPKG

    Example::

        import glob
        import rsgislib.imageutils.imagelut
        imgList = glob.glob('/Users/pete/Temp/GabonLandsat/Hansen*.kea')
        rsgislib.imageutils.imagelut.createImgExtentLUT(imgList, './ImgExtents.gpkg', 'HansenImgExtents', 'GPKG')

    """
    import rsgislib.vectorutils
    rsgislib.vectorutils.createImgExtentLUT(imgList, vectorFile, vectorLyr, vecDriver, ignore_none_imgs,
                                            out_proj_wgs84, overwrite_lut_file)


def query_img_lut(scn_bbox, lutdbfile, lyrname):
    """

    :param scn_bbox: A bbox (MinX, MaxX, MinY, MaxY) in the same projection as the LUT for the area of interest.
    :param lutdbfile: The file path to the LUT datbase file (e.g., lut.gpkg).
    :param lyrname: The layer name within the database file.
    :return: a list of files from the LUT
    """
    import rsgislib.vectorutils
    fileListLUT = rsgislib.vectorutils.getAttLstSelectBBoxFeats(lutdbfile, lyrname, ['path', 'filename'], scn_bbox)
    imgs = []
    for item in fileListLUT:
        print(item)
        imgs.append(os.path.join(item['path'], item['filename']))
    return imgs


def get_all_lut_imgs(lutdbfile, lyrname):
    """
    Get a list of all the images within the LUT.
    :param lutdbfile: The file path to the LUT datbase file (e.g., lut.gpkg).
    :param lyrname: The layer name within the database file.
    :return: a list of files from the LUT
    """
    import rsgislib.vectorutils
    fileListLUT = rsgislib.vectorutils.readVecColumns(lutdbfile, lyrname, ['path', 'filename'])
    imgs = []
    for item in fileListLUT:
        print(item)
        imgs.append(os.path.join(item['path'], item['filename']))
    return imgs


def getRasterLyr(scn_bbox, lutdbfile, lyrname, tmp_path):
    """
    This function provides a single raster layer using the LUT file provided. If
    a single image file intersecting with the BBOX is not within the LUT then a
    VRT generated from a number of images will be returned. The VRT will be within
    the tmp_path directory which will need to be cleaned up afterwards. If no image(s)
    is available then None is returned.

    :param scn_bbox: A bbox (MinX, MaxX, MinY, MaxY) in the same projection as the LUT for the area of interest.
    :param lutdbfile: The file path to the LUT datbase file (e.g., lut.gpkg).
    :param lyrname: The layer name within the database file.
    :param tmp_path: A directory where temporary files can be saved to.
    :return: a string with the file path to the raster layer. If no image intersects then None is returned.
    """
    if not haveGDALPy:
        raise Exception("Need to have GDAL library available for getRasterLyr function to work.")
    import rsgislib.vectorutils

    rsgis_utils = rsgislib.RSGISPyUtils()
    fileListLUT = rsgislib.vectorutils.getAttLstSelectBBoxFeats(lutdbfile, lyrname, ['path', 'filename'], scn_bbox)

    imgbase = "imglyr_{}".format(rsgis_utils.uidGenerator())
    # if number of scenes available is > 0 then create VRT
    if len(fileListLUT) > 1:
        imgs = []
        for item in fileListLUT:
            imgs.append(os.path.join(item['path'], item['filename']))
        outimgfile = os.path.join(tmp_path, '{}_tmp.vrt'.format(imgbase))
        gdal.BuildVRT(outimgfile, imgs)
    elif len(fileListLUT) == 1:
        outimgfile = os.path.join(fileListLUT[0]['path'], fileListLUT[0]['filename'])
    else:
        outimgfile = None
    return outimgfile


