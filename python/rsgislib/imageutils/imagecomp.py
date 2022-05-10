#!/usr/bin/env python
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

import os
import shutil

import osgeo.gdal
import rios.rat
import numpy

import rsgislib
import rsgislib.imagecalc
import rsgislib.imagecalc.calcindices
import rsgislib.imageutils
import rsgislib.rastergis



def check_build_ls8_ls9_vrts(input_imgs, output_dir):
    """
    A function which checks for Landsat 8 (LS8) and Landsat 9 (LS9) images
    in the input list and creates a band subset (i.e., removes the coastal
    (1) band). This function should be used ahead of create_max_ndvi_ndwi_composite
    when combining LS8 or LS8 with earlier landsat (i.e., LS5 and LS7) data.
    Note, files who's file name contain 'LS8' or 'lc08' are considered to be
    Landsat 8 images. files who's file name contain 'LS9' or 'lc09' are considered
    to be Landsat 8 images.

    :param input_imgs: input list of image file paths.
    :param output_dir: output directory for the VRT images (Note. an absolute path to
                   the input image is used when building the VRT.)
    """
    import rsgislib.tools.filetools

    img_bands = [2, 3, 4, 5, 6, 7]
    out_imgs = []
    for img in input_imgs:
        if ("LS8" in os.path.basename(img)) or ("lc08" in os.path.basename(img)) or ("LS9" in os.path.basename(img)) or ("lc09" in os.path.basename(img)):
            abs_img = os.path.abspath(img)
            basename = rsgislib.tools.filetools.get_file_basename(img)
            out_vrt_img = os.path.join(output_dir, f"{basename}_bsub.vrt")
            rsgislib.imageutils.create_vrt_band_subset(abs_img, img_bands, out_vrt_img)
            out_imgs.append(out_vrt_img)
        else:
            out_imgs.append(img)
    return out_imgs


def create_max_ndvi_composite(
    input_imgs,
    r_band,
    n_band,
    out_ref_img,
    out_comp_img,
    tmp_dir="tmp",
    gdalformat="KEA",
    datatype=None,
    calc_stats=True,
):
    """
    Create an image composite from multiple input images where the pixel brought
    through into the composite is the one with the maximum NDVI.

    :param input_imgs: list of input images for the analysis.
    :param r_band: is the image band within the input images (same for all) for
                   the red band - note band indexing starts at 1.
    :param n_band: is the image band within the input images (same for all) for
                   the nir band - note band indexing starts at 1.
    :param out_ref_img: is the output reference image which details which input
                        image is forming the output image pixel value (Note. this
                        file will always be a KEA file as RAT is used).
    :param out_comp_img: is the output composite image for which gdalformat and
                         datatype define the format and data type.
    :param tmp_dir: is a temp path for intermediate files, if this path doesn't
                    exist is will be created and deleted at runtime.
    :param gdalformat: is the output file format of the out_comp_img, any
                       GDAL compatible format is OK (Defaut is KEA).
    :param datatype: is the data type of the output image (out_comp_img). If
                     None is provided then the data type of the first input image
                     will be used (Default None).
    :param calc_stats: calculate image statics and pyramids (Default=True)

    """
    import rsgislib.tools.utils

    uid_str = rsgislib.tools.utils.uid_generator()

    if len(input_imgs) > 1:
        tmp_present = True
        if not os.path.exists(tmp_dir):
            os.makedirs(tmp_dir)
            tmp_present = False

        ref_layers_path = os.path.join(tmp_dir, "ref_lyrs_" + uid_str)

        ref_img_tmp_present = True
        if not os.path.exists(ref_layers_path):
            os.makedirs(ref_layers_path)
            ref_img_tmp_present = False

        if datatype is None:
            datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_imgs[0])

        num_in_lyrs = len(input_imgs)

        numpy.random.seed(5)
        red = numpy.random.randint(1, 255, num_in_lyrs + 1, int)
        red[0] = 0
        numpy.random.seed(2)
        green = numpy.random.randint(1, 255, num_in_lyrs + 1, int)
        green[0] = 0
        numpy.random.seed(9)
        blue = numpy.random.randint(1, 255, num_in_lyrs + 1, int)
        blue[0] = 0
        alpha = numpy.zeros_like(blue)
        alpha[...] = 255
        img_lyrs = numpy.empty(num_in_lyrs + 1, dtype=numpy.dtype("a255"))

        # Generate Comp Ref layers:
        ref_lyrs_lst = []
        idx = 1
        for img in input_imgs:
            print("In Image ({}):\t{}".format(idx, img))
            img_lyrs[idx] = os.path.basename(img)
            base_img_name = os.path.splitext(os.path.basename(img))[0]
            ref_lyr_img = os.path.join(ref_layers_path, base_img_name + "_ndvi.kea")
            rsgislib.imagecalc.calcindices.calc_ndvi(
                img, r_band, n_band, ref_lyr_img, False
            )
            ref_lyrs_lst.append(ref_lyr_img)
            idx = idx + 1
        img_lyrs[0] = ""

        # Create REF Image
        rsgislib.imagecalc.get_img_idx_for_stat(
            ref_lyrs_lst, out_ref_img, "KEA", -999, rsgislib.SUMTYPE_MAX
        )
        if calc_stats:
            # Pop Ref Image with stats
            rsgislib.rastergis.pop_rat_img_stats(out_ref_img, True, True, True)

            # Open the clumps dataset as a gdal dataset
            rat_dataset = osgeo.gdal.Open(out_ref_img, osgeo.gdal.GA_Update)

            # Write colours to RAT
            rios.rat.writeColumn(rat_dataset, "Red", red)
            rios.rat.writeColumn(rat_dataset, "Green", green)
            rios.rat.writeColumn(rat_dataset, "Blue", blue)
            rios.rat.writeColumn(rat_dataset, "Alpha", alpha)
            rios.rat.writeColumn(rat_dataset, "Image", img_lyrs)

            rat_dataset = None

        # Create Composite Image
        rsgislib.imageutils.create_ref_img_composite_img(
            input_imgs, out_comp_img, out_ref_img, gdalformat, datatype, 0.0
        )

        if calc_stats:
            # Calc Stats
            rsgislib.imageutils.pop_img_stats(
                out_comp_img, use_no_data=True, no_data_val=0, calc_pyramids=True
            )

        if not ref_img_tmp_present:
            shutil.rmtree(ref_layers_path, ignore_errors=True)

        if not tmp_present:
            shutil.rmtree(tmp_dir, ignore_errors=True)
    elif len(input_imgs) == 1:
        print("Only 1 Input Image, Just Copying File to output")
        shutil.copy(input_imgs[0], out_comp_img)
    else:
        raise rsgislib.RSGISPyException("At least 2 input images are needed")




def create_max_ndvi_ndwi_composite(
    ref_sp_img,
    input_imgs,
    r_band,
    n_band,
    s_band,
    out_ref_img,
    out_comp_img,
    out_msk_img,
    tmp_path="tmp",
    gdalformat="KEA",
    datatype=None,
    calc_stats=True,
    reproj_method=rsgislib.INTERP_CUBIC,
    use_mode=True,
):
    """
    Create an image composite from multiple input images where the pixel brought
    through into the composite is the one with the maximum NDVI over land and
    NDWI over water. A mask of land and water regions is also produced. The
    reference image is used to define the spatial extent of the output images
    and spatial projection.

    :param ref_sp_img: is a reference image with any number of bands and data
                       type which is used to define the output image extent
                       and projection.
    :param input_imgs: a list of all the input images being used to build the composite.
    :param r_band: is an integer specifying the red band in the input images
                   (starts at 1), used in the NDVI index.
    :param n_band: is an integer specifying the NIR band in the input images
                   (starts at 1), used in the NDVI and NDWI index.
    :param s_band: is an integer specifying the SWIR band in the input images
                   (starts at 1), used in the NDWI index.
    :param out_ref_img: is the output reference image which details which input
                        image is forming the output image pixel value (Note. this
                        file will always be a KEA file as RAT is used).
    :param out_comp_img: is the output composite image for which gdalformat and
                         datatype define the format and data type.
    :param out_msk_img: is the output mask image for regions of water and land
                        where ndvi vs ndwi are used (0=nodata, 1=land, 2=water)
    :param tmp_path: is a temp path for intermediate files, if this path doesn't
                     exist is will be created and deleted at runtime.
    :param gdalformat: is the output file format of the outCompImg, any GDAL
                       compatable format is OK (Defaut is KEA).
    :param datatype: is the data type of the output image (outCompImg). If None
                     is provided then the data type of the first input image
                     will be used (Default None).
    :param calc_stats: calculate image statics and pyramids (Default=True)
    :param reproj_method: specifies the interpolation method (rsgislib.INTERP*) used
                          to reproject the input images which are in a different
                          projection and/or pixel size as the reference image
                          (default: rsgislib.INTERP_CUBIC).
    :param use_mode: True: the land/water masks are combined using the mode
                     and False: the land water masks are combined using median.

    """
    import rsgislib.tools.utils

    uid_str = rsgislib.tools.utils.uid_generator()

    if len(input_imgs) > 1:
        init_in_images = input_imgs
        in_images_overlap = []
        in_images_to_reproj = []
        in_images_sub_to_ref = []
        num_bands = 0
        first = True
        for img in init_in_images:
            if first:
                num_bands = rsgislib.imageutils.get_img_band_count(img)
                first = False
            else:
                band_count = rsgislib.imageutils.get_img_band_count(img)
                if band_count != num_bands:
                    raise rsgislib.RSGISPyException(
                        f"The number of image bands is not "
                        f"consistent (Bands: {num_bands} and {band_count})"
                    )

            same_proj = rsgislib.imageutils.do_gdal_layers_have_same_proj(ref_sp_img, img)
            if rsgislib.imageutils.do_images_overlap(ref_sp_img, img):
                if same_proj:
                    if rsgislib.imageutils.do_img_res_match(ref_sp_img, img):
                        in_images_overlap.append(img)
                    else:
                        in_images_to_reproj.append(img)
                else:
                    in_images_to_reproj.append(img)

        n_imgs = len(in_images_overlap) + len(in_images_to_reproj)
        print(f"There are {n_imgs} images with overlap to create the composite.")
        if n_imgs > 1:
            if datatype is None:
                if len(in_images_overlap) > 0:
                    datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(
                        in_images_overlap[0]
                    )
                else:
                    datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(
                        in_images_to_reproj[0]
                    )

            tmp_present = True
            if not os.path.exists(tmp_path):
                os.makedirs(tmp_path)
                tmp_present = False

            subset_img_tmp_present = True
            if len(in_images_overlap) > 0:
                print("Subset images to reference extent.")
                subset_layers_path = os.path.join(tmp_path, f"subset_{uid_str}")
                if not os.path.exists(subset_layers_path):
                    os.makedirs(subset_layers_path)
                    subset_img_tmp_present = False

                for img in in_images_overlap:
                    basename = os.path.splitext(os.path.basename(img))[0]
                    out_img = os.path.join(subset_layers_path, basename + "_subset.kea")
                    rsgislib.imageutils.create_copy_img(
                        ref_sp_img, out_img, num_bands, 0, "KEA", datatype
                    )
                    rsgislib.imageutils.include_imgs_ind_img_intersect(out_img, [img])
                    in_images_sub_to_ref.append(out_img)

            reproj_img_tmp_present = True
            if len(in_images_to_reproj) > 0:
                print("Reproject images to reference extent.")
                reproj_layers_path = os.path.join(tmp_path, "reproj_" + uid_str)
                if not os.path.exists(reproj_layers_path):
                    os.makedirs(reproj_layers_path)
                    reproj_img_tmp_present = False

                for img in in_images_to_reproj:
                    basename = os.path.splitext(os.path.basename(img))[0]
                    out_img = os.path.join(reproj_layers_path, basename + "_reproj.kea")
                    rsgislib.imageutils.resample_img_to_match(
                        ref_sp_img, img, out_img, "KEA", reproj_method, datatype
                    )
                    in_images_sub_to_ref.append(out_img)

            ref_layers_path = os.path.join(tmp_path, f"ref_lyrs_{uid_str}")
            ref_img_tmp_present = True
            if not os.path.exists(ref_layers_path):
                os.makedirs(ref_layers_path)
                ref_img_tmp_present = False

            num_in_lyrs = len(in_images_sub_to_ref)

            numpy.random.seed(5)
            red = numpy.random.randint(1, 255, num_in_lyrs + 1, int)
            red[0] = 0
            numpy.random.seed(2)
            green = numpy.random.randint(1, 255, num_in_lyrs + 1, int)
            green[0] = 0
            numpy.random.seed(9)
            blue = numpy.random.randint(1, 255, num_in_lyrs + 1, int)
            blue[0] = 0
            alpha = numpy.zeros_like(blue)
            alpha[...] = 255
            img_lyrs = numpy.empty(num_in_lyrs + 1, dtype=numpy.dtype("a255"))

            # Generate Comp Ref layers:
            land_water_msk_band_defns = []
            msk_imgs = []
            idx = 1
            for img in in_images_sub_to_ref:
                print("In Image (" + str(idx) + "):\t" + img)
                img_lyrs[idx] = os.path.basename(img)
                base_img_name = os.path.splitext(os.path.basename(img))[0]
                ref_lyr_ndvi_img = os.path.join(ref_layers_path, f"{base_img_name}_ndvi.kea")
                ref_lyr_ndwi_img = os.path.join(ref_layers_path, f"{base_img_name}_ndwi.kea")
                rsgislib.imagecalc.calcindices.calc_ndvi(
                    img, r_band, n_band, ref_lyr_ndvi_img, False
                )
                rsgislib.imagecalc.calcindices.calc_ndwi(
                    img, n_band, s_band, ref_lyr_ndwi_img, False
                )

                ref_lyr_msk_img = os.path.join(
                    ref_layers_path, base_img_name + "_water_land_msk.kea"
                )
                band_defns = []
                band_defns.append(rsgislib.imagecalc.BandDefn("ndvi", ref_lyr_ndvi_img, 1))
                band_defns.append(rsgislib.imagecalc.BandDefn("ndwi", ref_lyr_ndwi_img, 1))
                rsgislib.imagecalc.band_math(
                    ref_lyr_msk_img,
                    "ndvi<-1?0:ndvi>0.3?1:ndwi>0.01?2:1",
                    "KEA",
                    rsgislib.TYPE_8UINT,
                    band_defns,
                )
                msk_imgs.append(ref_lyr_msk_img)
                idx = idx + 1
            img_lyrs[0] = ""

            ref_lyr_msk_stack_img = os.path.join(
                ref_layers_path, uid_str + "_water_land_msk_stack.kea"
            )
            rsgislib.imageutils.stack_img_bands(
                msk_imgs, None, ref_lyr_msk_stack_img, -1, -1, "KEA", rsgislib.TYPE_8UINT
            )
            if use_mode:
                rsgislib.imagecalc.image_pixel_column_summary(
                    ref_lyr_msk_stack_img,
                    out_msk_img,
                    rsgislib.imagecalc.StatsSummary(calc_mode=True),
                    "KEA",
                    rsgislib.TYPE_8UINT,
                    0,
                    True,
                )
            else:
                rsgislib.imagecalc.image_pixel_column_summary(
                    ref_lyr_msk_stack_img,
                    out_msk_img,
                    rsgislib.imagecalc.StatsSummary(calc_median=True),
                    "KEA",
                    rsgislib.TYPE_8UINT,
                    0,
                    True,
                )
            rsgislib.rastergis.pop_rat_img_stats(out_msk_img, True, True, True)

            idx = 1
            ref_lyrs_lst = []
            for img in in_images_sub_to_ref:
                print(f"In Image ({idx}):\t{img}")
                base_img_name = os.path.splitext(os.path.basename(img))[0]
                ref_lyr_ndvi_img = os.path.join(ref_layers_path, f"{base_img_name}_ndvi.kea")
                ref_lyr_ndwi_img = os.path.join(ref_layers_path, f"{base_img_name}_ndwi.kea")
                ref_lyr_lcl_msk_img = os.path.join(
                    ref_layers_path, f"{base_img_name}_water_land_msk.kea"
                )

                ref_lyr_img = os.path.join(ref_layers_path, f"{base_img_name}_ref_hybrid.kea")
                band_defns = []
                band_defns.append(
                    rsgislib.imagecalc.BandDefn("lmsk", ref_lyr_lcl_msk_img, 1)
                )
                band_defns.append(rsgislib.imagecalc.BandDefn("omsk", out_msk_img, 1))
                band_defns.append(rsgislib.imagecalc.BandDefn("ndvi", ref_lyr_ndvi_img, 1))
                band_defns.append(rsgislib.imagecalc.BandDefn("ndwi", ref_lyr_ndwi_img, 1))
                rsgislib.imagecalc.band_math(
                    ref_lyr_img,
                    "lmsk==0?-999:omsk==1?ndvi:omsk==2?ndwi:-999",
                    "KEA",
                    rsgislib.TYPE_32FLOAT,
                    band_defns,
                )
                ref_lyrs_lst.append(ref_lyr_img)
                idx = idx + 1

            # Create REF Image
            rsgislib.imagecalc.get_img_idx_for_stat(
                ref_lyrs_lst, out_ref_img, "KEA", -999, rsgislib.SUMTYPE_MAX
            )
            if calc_stats:
                # Pop Ref Image with stats
                rsgislib.rastergis.pop_rat_img_stats(out_ref_img, True, True, True)

                # Open the clumps dataset as a gdal dataset
                rat_dataset = osgeo.gdal.Open(out_ref_img, osgeo.gdal.GA_Update)
                # Write colours to RAT
                rios.rat.writeColumn(rat_dataset, "Red", red)
                rios.rat.writeColumn(rat_dataset, "Green", green)
                rios.rat.writeColumn(rat_dataset, "Blue", blue)
                rios.rat.writeColumn(rat_dataset, "Alpha", alpha)
                rios.rat.writeColumn(rat_dataset, "Image", img_lyrs)
                rat_dataset = None

            # Create Composite Image
            rsgislib.imageutils.create_ref_img_composite_img(
                in_images_sub_to_ref, out_comp_img, out_ref_img, gdalformat, datatype, 0.0
            )

            if calc_stats:
                # Calc Stats
                rsgislib.imageutils.pop_img_stats(
                    out_comp_img, use_no_data=True, no_data_val=0, calc_pyramids=True
                )

            if not ref_img_tmp_present:
                shutil.rmtree(ref_layers_path, ignore_errors=True)

            if not subset_img_tmp_present:
                shutil.rmtree(subset_layers_path, ignore_errors=True)

            if not reproj_img_tmp_present:
                shutil.rmtree(reproj_layers_path, ignore_errors=True)

            if not tmp_present:
                shutil.rmtree(tmp_path, ignore_errors=True)
        else:
            raise rsgislib.RSGISPyException(f"There were only {n_imgs} images which intersect with the reference image.")

    elif len(input_imgs) == 1:
        print("Only 1 Input Image, Just Copying File to output")
        num_bands = rsgislib.imageutils.get_img_band_count(input_imgs[0])
        if datatype is None:
            datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_imgs[0])
        rsgislib.imageutils.create_copy_img(
            ref_sp_img, out_comp_img, num_bands, 0, gdalformat, datatype
        )
        rsgislib.imageutils.include_imgs_ind_img_intersect(out_comp_img, [input_imgs[0]])
    else:
        raise rsgislib.RSGISPyException("List of input images was empty ")
