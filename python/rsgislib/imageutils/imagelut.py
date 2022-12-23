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

import os
from typing import List

from osgeo import gdal, ogr

gdal.UseExceptions()

import rsgislib
import rsgislib.vectorutils


def create_img_extent_lut(
    input_imgs: List[str],
    vec_file: str,
    vec_lyr: str,
    out_format: str,
    ignore_none_imgs: bool = False,
    out_proj_wgs84: bool = False,
    overwrite_lut_file: bool = False,
):
    """
    Create a vector layer look up table (LUT) for a directory of images.

    :param input_imgs: list of input images for the LUT. All input images should be
                       the same projection/coordinate system.
    :param vec_file: output vector file/path
    :param vec_lyr: output vector layer
    :param out_format: the output vector layer type (e.g., GPKG).
    :param ignore_none_imgs: if a NULL epsg is returned from an image then ignore
                             and don't include in LUT else throw exception.
    :param out_proj_wgs84: if True then the image bounding boxes will be
                           re-projected to EPSG:4326.
    :param overwrite_lut_file: if True then output file will be overwritten. If
                               false then not, e.g., can add extra layer to GPKG

    .. code:: python

        import glob
        import rsgislib.imageutils.imagelut
        imgList = glob.glob('/Users/pete/Temp/GabonLandsat/Hansen*.kea')
        rsgislib.imageutils.imagelut.create_img_extent_lut(imgList,
                                                          'ImgExtents.gpkg',
                                                          'HansenImgExtents', 'GPKG')

    """
    import tqdm

    import rsgislib.imageutils
    import rsgislib.vectorutils.createvectors

    bboxs = []
    atts = dict()
    atts["filename"] = []
    atts["path"] = []

    attTypes = dict()
    attTypes["types"] = [ogr.OFTString, ogr.OFTString]
    attTypes["names"] = ["filename", "path"]

    epsgCode = 0

    first = True
    baseImg = ""
    for img in tqdm.tqdm(input_imgs):
        epsgCodeTmp = rsgislib.imageutils.get_epsg_proj_from_img(img)
        epsg_found = True
        if epsgCodeTmp is None:
            epsg_found = False
            if not ignore_none_imgs:
                raise rsgislib.RSGISPyException(
                    "The EPSG code is None: '{}'".format(img)
                )
        if epsg_found:
            if out_proj_wgs84:
                epsgCode = 4326
            else:
                epsgCodeTmp = int(epsgCodeTmp)
                if first:
                    epsgCode = epsgCodeTmp
                    baseImg = img
                    first = False
                else:
                    if epsgCodeTmp != epsgCode:
                        raise rsgislib.RSGISPyException(
                            "The EPSG codes ({0} & {1}) do not match. "
                            "(Base: '{2}', Img: '{3}')".format(
                                epsgCode, epsgCodeTmp, baseImg, img
                            )
                        )

            if out_proj_wgs84:
                img_bbox = rsgislib.imageutils.get_img_bbox_in_proj(img, 4326)
            else:
                img_bbox = rsgislib.imageutils.get_img_bbox(img)

            bboxs.append(img_bbox)
            baseName = os.path.basename(img)
            filePath = os.path.dirname(img)
            atts["filename"].append(baseName)
            atts["path"].append(filePath)
    # Create vector layer
    rsgislib.vectorutils.createvectors.create_poly_vec_bboxs(
        vec_file,
        vec_lyr,
        out_format,
        epsgCode,
        bboxs,
        atts,
        attTypes,
        overwrite=overwrite_lut_file,
    )


def query_img_lut(scn_bbox: List[float], lut_db_file: str, lyr_name: str) -> List[str]:
    """
    A function for querying the LUT DB spatially filtering using a BBOX

    :param scn_bbox: A bbox (MinX, MaxX, MinY, MaxY) in the same projection as
                     the LUT for the area of interest.
    :param lut_db_file: The file path to the LUT database file (e.g., lut.gpkg).
    :param lyr_name: The layer name within the database file.
    :return: a list of files from the LUT

    """
    file_list_lut = rsgislib.vectorutils.get_att_lst_select_bbox_feats(
        lut_db_file, lyr_name, ["path", "filename"], scn_bbox
    )
    imgs = []
    for item in file_list_lut:
        imgs.append(os.path.join(item["path"], item["filename"]))
    return imgs


def get_all_lut_imgs(lut_db_file: str, lyr_name: str) -> List[str]:
    """
    Get a list of all the images within the LUT.

    :param lut_db_file: The file path to the LUT database file (e.g., lut.gpkg).
    :param lyr_name: The layer name within the database file.
    :return: a list of files from the LUT

    """
    import rsgislib.vectorattrs

    file_list_lut = rsgislib.vectorattrs.read_vec_columns(
        lut_db_file, lyr_name, ["path", "filename"]
    )
    imgs = []
    for item in file_list_lut:
        imgs.append(os.path.join(item["path"], item["filename"]))
    return imgs


def get_raster_lyr(
    scn_bbox: List[float], lut_db_file: str, lyr_name: str, tmp_dir: str
) -> str:
    """
    This function provides a single raster layer using the LUT file provided. If
    a single image file intersecting with the BBOX is not within the LUT then a
    VRT generated from a number of images will be returned. The VRT will be within
    the tmp_path directory which will need to be cleaned up afterwards. If no image(s)
    is available then None is returned.

    :param scn_bbox: A bbox (MinX, MaxX, MinY, MaxY) in the same projection as the
                     LUT for the area of interest.
    :param lut_db_file: The file path to the LUT datbase file (e.g., lut.gpkg).
    :param lyr_name: The layer name within the database file.
    :param tmp_dir: A directory where temporary files can be saved to.
    :return: a string with the file path to the raster layer. If no image intersects
             then None is returned.

    """
    import rsgislib.tools.utils

    file_list_lut = rsgislib.vectorutils.get_att_lst_select_bbox_feats(
        lut_db_file, lyr_name, ["path", "filename"], scn_bbox
    )

    img_base = "imglyr_{}".format(rsgislib.tools.utils.uid_generator())
    # if number of scenes available is > 0 then create VRT
    if len(file_list_lut) > 1:
        imgs = []
        for item in file_list_lut:
            imgs.append(os.path.join(item["path"], item["filename"]))
        out_img_file = os.path.join(tmp_dir, "{}_tmp.vrt".format(img_base))
        gdal.BuildVRT(out_img_file, imgs)
    elif len(file_list_lut) == 1:
        out_img_file = os.path.join(
            file_list_lut[0]["path"], file_list_lut[0]["filename"]
        )
    else:
        out_img_file = None
    return out_img_file


def query_file_lut(
    lut_db_file: str,
    lyr_name: str,
    roi_file: str,
    roi_lyr: str,
    out_dest: str,
    targz_out: bool = False,
    cp_cmds: bool = False,
) -> List[str]:
    """
    A function which allows the file LUT to be queried (intersection) and commands
    generated for completing operations. Must select (pass true) for either targz_out
    or cp_cmds not both. If both are False then the list of intersecting files will
    be returned.

    :param lut_db_file: OGR vector file with the LUT.
    :param lyr_name: name of the layer within the LUT file.
    :param roi_file: region of interest OGR vector file.
    :param roi_lyr: layer name within the ROI file.
    :param out_dest: the destination for outputs from command (e.g., where are the
                     files to be copied to or output file name for tar.gz file.
    :param targz_out: boolean which specifies that the command for generating
                      a tar.gz file should be generated.
    :param cp_cmds: boolean which specifies that the command for copying the
                    LUT files to a out_dest should be generated.

    :return: returns a list of commands to be executed.

    """
    if lyr_name is None:
        lyr_name = os.path.splitext(os.path.basename(lut_db_file))[0]

    if roi_lyr is None:
        roi_lyr = os.path.splitext(os.path.basename(roi_file))[0]

    roi_mem_ds, roi_mem_lyr = rsgislib.vectorutils.read_vec_lyr_to_mem(
        roi_file, roi_lyr
    )

    roi_bbox = roi_mem_lyr.GetExtent(True)

    lut_mem_ds, lut_mem_lyr = rsgislib.vectorutils.get_mem_vec_lyr_subset(
        lut_db_file, lyr_name, roi_bbox
    )

    file_list_dict = rsgislib.vectorutils.get_att_lst_select_feats_lyr_objs(
        lut_mem_lyr, ["path", "filename"], roi_mem_lyr
    )

    out_cmds = []
    if targz_out:
        cmd = "tar -czf {}".format(out_dest)
        for file_item in file_list_dict:
            file_path = os.path.join(file_item["path"], file_item["filename"])
            cmd = "{} {}".format(cmd, file_path)
        out_cmds.append(cmd)
    elif cp_cmds:
        for file_item in file_list_dict:
            file_path = os.path.join(file_item["path"], file_item["filename"])
            out_cmds.append("cp {0} {1}".format(file_path, out_dest))
    else:
        for file_item in file_list_dict:
            file_path = os.path.join(file_item["path"], file_item["filename"])
            out_cmds.append(file_path)

    return out_cmds
