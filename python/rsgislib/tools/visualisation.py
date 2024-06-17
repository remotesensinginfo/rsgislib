#!/usr/bin/env python
"""
The tools.visualisation module contains functions for aiding visualisation of data.
"""

# Import modules
import os
import shutil
import subprocess
from typing import List, Union

import rsgislib
import rsgislib.imageutils
import rsgislib.tools.filetools
import rsgislib.tools.projection
import rsgislib.tools.tilecacheutils
import rsgislib.tools.utils

TQDM_AVAIL = True
try:
    import tqdm
except ImportError:
    import rios.cuiprogress

    TQDM_AVAIL = False


def create_kmz_img(
    input_img: str,
    output_file: str,
    bands: str,
    reproj_wgs84: bool = True,
    finite_msk: bool = False,
    tmp_dir: str = None,
):
    """
    A function to convert an input image to a KML/KMZ file, where the input image
    is stretched and bands sub-selected / ordered as required for visualisation.

    :param input_img: input image file (any format that gdal supports)
    :param output_file: output image file (extension kmz for KMZ output /
                        kml for KML output)
    :param bands: a string (comma seperated) with the bands to be selected.
                  (e.g., '1', '1,2,3', '5,6,4')
    :param reproj_wgs84: specify whether the image should be explicitly reprojected
                         to WGS84 Lat/Long before transformation to KML.
    :param finite_msk: specify whether the image data should be masked so all
                       values are finite before stretching.
    :param tmp_dir: A temp directory path for processing files.

    """
    if not rsgislib.tools.filetools.does_path_exists_or_creatable(output_file):
        raise rsgislib.RSGISPyException(
            f"Output file path is not creatable: {output_file}"
        )

    band_lst = bands.split(",")
    if len(band_lst) == 3:
        multi_band = True
    elif len(band_lst) == 1:
        multi_band = False
    else:
        raise rsgislib.RSGISPyException(
            "You need to either provide 1 or 3 bands: {}".format(band_lst)
        )

    n_img_bands = rsgislib.imageutils.get_img_band_count(input_img)

    uid_str = rsgislib.tools.utils.uid_generator()
    if tmp_dir is None:
        tmp_dir = os.path.join(os.path.dirname(input_img), uid_str)
    else:
        tmp_dir = os.path.join(tmp_dir, uid_str)
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
    base_name = rsgislib.tools.filetools.get_file_basename(input_img)

    sel_img_bands_img = ""
    if (n_img_bands == 1) and (not multi_band):
        sel_img_bands_img = input_img
    elif (
        (n_img_bands == 3)
        and (multi_band)
        and (band_lst[0] == "1")
        and (band_lst[1] == "2")
        and (band_lst[2] == "3")
    ):
        sel_img_bands_img = input_img
    else:
        s_bands = []
        for str_band in band_lst:
            s_bands.append(int(str_band))
        sel_img_bands_img = os.path.join(tmp_dir, f"{base_name}_sband.kea")
        rsgislib.imageutils.select_img_bands(
            input_img,
            sel_img_bands_img,
            "KEA",
            rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
            s_bands,
        )

    img_to_strch = sel_img_bands_img
    if finite_msk:
        finite_msk_img = os.path.join(tmp_dir, f"{base_name}_FiniteMsk.kea")
        rsgislib.imageutils.gen_finite_mask(sel_img_bands_img, finite_msk_img, "KEA")
        img_to_strch = os.path.join(tmp_dir, f"{base_name}_Msk2FiniteRegions.kea")
        rsgislib.imageutils.mask_img(
            sel_img_bands_img,
            finite_msk_img,
            img_to_strch,
            "KEA",
            rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
            0,
            0,
        )

    stretch_img = os.path.join(tmp_dir, f"{base_name}_stretch.kea")
    rsgislib.imageutils.stretch_img(
        img_to_strch,
        stretch_img,
        False,
        "",
        True,
        False,
        "KEA",
        rsgislib.TYPE_8UINT,
        rsgislib.imageutils.STRETCH_LINEARSTDDEV,
        2,
    )

    gdal_input_img = stretch_img
    if reproj_wgs84:
        lat_long_img = os.path.join(tmp_dir, f"{base_name}_latlong.kea")
        out_wkt = os.path.join(tmp_dir, f"{base_name}_latlong.wkt")
        wgs84_wkt = rsgislib.tools.projection.get_wkt_from_epsg_code(4326)
        rsgislib.tools.utils.write_list_to_file([wgs84_wkt], out_wkt)
        rsgislib.imageutils.reproject_image(
            stretch_img,
            lat_long_img,
            out_wkt,
            gdalformat="KEA",
            interp_method=rsgislib.INTERP_CUBIC,
            in_wkt=None,
            no_data_val=0.0,
            out_pxl_res="auto",
            snap_to_grid=True,
        )
        gdal_input_img = lat_long_img

    cmd = ["gdal_translate", "-of", "KMLSUPEROVERLAY", gdal_input_img, output_file]
    print(cmd)
    try:
        subprocess.run(cmd, check=True)
    except OSError as e:
        raise rsgislib.RSGISPyException("Could not execute command: {}".format(cmd))

    shutil.rmtree(tmp_dir)


def create_webtiles_img_no_stats_msk(
    input_img: str,
    out_dir: str,
    bands: str,
    zoom_levels: str = "2-10",
    resample="average",
    finite_msk: bool = False,
    tms: bool = True,
    tmp_dir=None,
):
    """
    A function to convert an input image to a tile cache for web map servers, where the input image
    is stretched and bands sub-selected / ordered as required for visualisation.

    :param input_img: input image file (any format that gdal supports)
    :param out_dir: output directory within which the cache will be created.
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param zoom_levels: The zoom levels to be created for the web tile cache.
    :param resample: Method of resampling (average,near,bilinear,cubic,cubicspline,lanczos,antialias)
    :param finite_msk: specify whether the image data should be masked so all values are finite before stretching.
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
                (TMS: gdal2tiles.py native. XYZ: GIS Compatible)
    :param tmp_dir: A temp directory path for processing files.

    """
    if not rsgislib.tools.filetools.does_path_exists_or_creatable(out_dir):
        raise rsgislib.RSGISPyException(f"Output file path is not creatable: {out_dir}")

    band_lst = bands.split(",")
    multi_band = False
    if len(band_lst) == 3:
        multi_band = True
    elif len(band_lst) == 1:
        multi_band = False
    else:
        raise rsgislib.RSGISPyException(
            "You need to either provide 1 or 3 bands: {}".format(band_lst)
        )
    n_img_bands = rsgislib.imageutils.get_img_band_count(input_img)

    uid_str = rsgislib.tools.utils.uid_generator()

    if tmp_dir is None:
        tmp_dir = os.path.join(os.path.dirname(input_img), uid_str)
    else:
        tmp_dir = os.path.join(tmp_dir, uid_str)
    os.makedirs(tmp_dir)
    base_name = rsgislib.tools.filetools.get_file_basename(input_img)

    sel_img_bands_img = ""
    if (n_img_bands == 1) and (not multi_band):
        sel_img_bands_img = input_img
    elif (
        (n_img_bands == 3)
        and (multi_band)
        and (band_lst[0] == "1")
        and (band_lst[1] == "2")
        and (band_lst[2] == "3")
    ):
        sel_img_bands_img = input_img
    else:
        s_bands = []
        for str_band in band_lst:
            s_bands.append(int(str_band))
        sel_img_bands_img = os.path.join(tmp_dir, f"{base_name}_sband.kea")
        rsgislib.imageutils.select_img_bands(
            input_img,
            sel_img_bands_img,
            "KEA",
            rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
            s_bands,
        )

    img_to_strch = sel_img_bands_img
    if finite_msk:
        finite_msk_img = os.path.join(tmp_dir, f"{base_name}_FiniteMsk.kea")
        rsgislib.imageutils.gen_finite_mask(sel_img_bands_img, finite_msk_img, "KEA")
        img_to_strch = os.path.join(tmp_dir, f"{base_name}_Msk2FiniteRegions.kea")
        rsgislib.imageutils.mask_img(
            sel_img_bands_img,
            finite_msk_img,
            img_to_strch,
            "KEA",
            rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
            0,
            0,
        )

    stretch_img = os.path.join(tmp_dir, f"{base_name}_stretch.kea")
    rsgislib.imageutils.stretch_img(
        img_to_strch,
        stretch_img,
        False,
        "",
        True,
        False,
        "KEA",
        rsgislib.TYPE_8UINT,
        rsgislib.imageutils.STRETCH_LINEARSTDDEV,
        2,
    )

    cmd = [
        "gdal2tiles.py",
        "-r",
        resample,
        "-z",
        zoom_levels,
        "-a",
        "0",
        stretch_img,
        out_dir,
    ]
    print(cmd)
    try:
        subprocess.run(cmd, check=True)
    except OSError as e:
        raise rsgislib.RSGISPyException("Could not execute command: {}".format(cmd))

    if not tms:
        rsgislib.tools.tilecacheutils.convert_between_tms_xyz(out_dir)

    shutil.rmtree(tmp_dir)


def create_webtiles_img(
    input_img: str,
    bands: str,
    out_dir: str,
    zoom_levels: str = "2-10",
    img_stats_msk: str = None,
    img_msk_vals: int = 1,
    tmp_dir: str = None,
    webview: bool = True,
    tms: bool = True,
    no_data_val: float = None,
):
    """
    A function to produce a web cache for the input image.

    :param input_img: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected.
                  (e.g., '1', '1,2,3', '5,6,4')
    :param out_dir: output directory within which the cache will be created.
    :param zoom_levels: The zoom levels to be created for the web tile cache.
    :param img_stats_msk: Optional (default=None) input image which is used to define
                          regions calculate the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the
                         image mask (can be list of values or single value).
    :param tmp_dir: an input directory which can be used to write temporary files /
                    directories. If not provided (i.e., input is None) then a local
                    directory will be define in the same folder as the input image.
    :param webview: Provide default GDAL leaflet web viewer.
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
                (TMS: gdal2tiles.py native. XYZ: GIS Compatible)
    :param no_data_val: Optionally provide an input image no data value otherwise
                        it will be read from the input image header.

    """
    if not rsgislib.tools.filetools.does_path_exists_or_creatable(out_dir):
        raise rsgislib.RSGISPyException(f"Output file path is not creatable: {out_dir}")

    band_lst = bands.split(",")
    multi_band = False
    if len(band_lst) == 3:
        multi_band = True
    elif len(band_lst) == 1:
        multi_band = False
    else:
        raise rsgislib.RSGISPyException(
            "You need to either provide 1 or 3 bands. {}".format(band_lst)
        )
    n_img_bands = rsgislib.imageutils.get_img_band_count(input_img)
    if no_data_val is None:
        no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)
    if no_data_val is None:
        raise rsgislib.RSGISPyException("No data value is not defined.")

    uid_str = rsgislib.tools.utils.uid_generator()
    if tmp_dir is not None:
        tmp_dir = os.path.join(tmp_dir, uid_str)
    else:
        tmp_dir = os.path.join(os.path.dirname(input_img), uid_str)
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
    base_name = rsgislib.tools.filetools.get_file_basename(input_img)

    sel_img_bands_img = ""
    if (n_img_bands == 1) and (not multi_band):
        sel_img_bands_img = input_img
    elif (
        (n_img_bands == 3)
        and (multi_band)
        and (band_lst[0] == "1")
        and (band_lst[1] == "2")
        and (band_lst[2] == "3")
    ):
        sel_img_bands_img = input_img
    else:
        s_bands = []
        for str_band in band_lst:
            s_bands.append(int(str_band))
        sel_img_bands_img = os.path.join(tmp_dir, f"{base_name}_sband.kea")
        rsgislib.imageutils.select_img_bands(
            input_img,
            sel_img_bands_img,
            "KEA",
            rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
            s_bands,
        )

    img_to_strch = sel_img_bands_img
    stretch_img = os.path.join(tmp_dir, f"{base_name}_stretch.kea")
    if img_stats_msk is not None:
        img_to_strch_mskd = os.path.join(tmp_dir, f"{base_name}_MskdImg.kea")
        rsgislib.imageutils.mask_img(
            sel_img_bands_img,
            img_stats_msk,
            img_to_strch_mskd,
            "KEA",
            rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
            no_data_val,
            img_msk_vals,
        )
        stretch_img_stats = os.path.join(tmp_dir, f"{base_name}_stretch_statstmp.txt")
        stretch_img_tmp = os.path.join(tmp_dir, f"{base_name}_stretch_tmp.kea")
        rsgislib.imageutils.stretch_img(
            img_to_strch_mskd,
            stretch_img_tmp,
            True,
            stretch_img_stats,
            no_data_val,
            False,
            "KEA",
            rsgislib.TYPE_8UINT,
            rsgislib.imageutils.STRETCH_LINEARSTDDEV,
            2,
        )

        rsgislib.imageutils.stretch_img_with_stats(
            img_to_strch,
            stretch_img,
            stretch_img_stats,
            "KEA",
            rsgislib.TYPE_8UINT,
            no_data_val,
            rsgislib.imageutils.STRETCH_LINEARMINMAX,
            2,
        )
    else:
        rsgislib.imageutils.stretch_img(
            img_to_strch,
            stretch_img,
            False,
            "",
            no_data_val,
            False,
            "KEA",
            rsgislib.TYPE_8UINT,
            rsgislib.imageutils.STRETCH_LINEARSTDDEV,
            2,
        )

    webview_opt = "none"
    if webview:
        webview_opt = "leaflet"

    cmd = [
        "gdal2tiles.py",
        "-r",
        "average",
        "-z",
        zoom_levels,
        "-a",
        "0",
        "-w",
        webview_opt,
        stretch_img,
        out_dir,
    ]
    print(cmd)
    try:
        subprocess.run(cmd, check=True)
    except OSError as e:
        raise rsgislib.RSGISPyException("Could not execute command: {}".format(cmd))
    shutil.rmtree(tmp_dir)

    if not tms:
        rsgislib.tools.tilecacheutils.convert_between_tms_xyz(out_dir)


def create_quicklook_imgs(
    input_img: str,
    bands: str,
    output_imgs: Union[str, List[str]] = "quicklook.jpg",
    output_img_sizes: Union[int, List[int]] = 250,
    scale_axis: str = "auto",
    img_stats_msk: str = None,
    img_msk_vals: int = 1,
    stretch_file: str = None,
    tmp_dir: str = None,
    no_data_val: float = None,
):
    """
    A function to produce a quicklook image(s).

    :param input_img: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected.
                  (e.g., '1', '1,2,3', '5,6,4')
    :param output_imgs: a single output image or list of output images. The same
                        size as output_img_sizes.
    :param output_img_sizes: the output image size (in pixels) or list of output
                             image sizes.
    :param scale_axis: the axis to which the output_img_sizes refer. Options:
                       width, height or auto. Auto applies the output_img_sizes to
                       the longest of the two axes.
    :param img_stats_msk: Optional (default=None) input image which is used to
                          define regions calculate the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the
                         image mask (can be list of values or single value).
    :param stretch_file: a stretch stats file to standardise the stretch between
                         a number of input files.
    :param tmp_dir: an input directory which can be used to write temporary files /
                    directories. If not provided (i.e., input is None) then a local
                    directory will be define in the same folder as the input image.
    :param no_data_val: Optionally provide an input image no data value otherwise
                        it will be read from the input image header.

    """
    import rsgislib.tools.utils

    if scale_axis not in ["width", "height", "auto"]:
        raise rsgislib.RSGISPyException(
            "Input parameter 'scale_axis' must have the "
            "value 'width', 'height' or 'auto'."
        )

    n_out_imgs = 1
    if type(output_imgs) is list:
        n_out_imgs = len(output_imgs)
        if type(output_img_sizes) is not list:
            raise rsgislib.RSGISPyException(
                "If the output_imgs input is a list so must output_img_sizes."
            )
        if len(output_img_sizes) != n_out_imgs:
            raise rsgislib.RSGISPyException(
                "output_imgs and output_img_sizes must be the same length"
            )

        if n_out_imgs == 1:
            output_imgs = output_imgs[0]
            output_img_sizes = output_img_sizes[0]

    band_lst = bands.split(",")
    multi_band = False
    if len(band_lst) == 3:
        multi_band = True
    elif len(band_lst) == 1:
        multi_band = False
    else:
        raise rsgislib.RSGISPyException(
            "You need to either provide 1 or 3 bands: {}".format(band_lst)
        )
    n_img_bands = rsgislib.imageutils.get_img_band_count(input_img)
    if no_data_val is None:
        no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)
    if no_data_val is None:
        raise rsgislib.RSGISPyException("No data value is not defined.")

    uid_str = rsgislib.tools.utils.uid_generator()
    if tmp_dir is not None:
        tmp_dir = os.path.join(tmp_dir, uid_str)
    else:
        tmp_dir = os.path.join(os.path.dirname(input_img), uid_str)
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
    base_name = rsgislib.tools.filetools.get_file_basename(input_img)

    sel_img_bands_img = ""
    if (n_img_bands == 1) and (not multi_band):
        sel_img_bands_img = input_img
    elif (
        (n_img_bands == 3)
        and (multi_band)
        and (band_lst[0] == "1")
        and (band_lst[1] == "2")
        and (band_lst[2] == "3")
    ):
        sel_img_bands_img = input_img
    else:
        s_bands = []
        for str_band in band_lst:
            s_bands.append(int(str_band))
        sel_img_bands_img = os.path.join(tmp_dir, f"{base_name}_sband.kea")
        rsgislib.imageutils.select_img_bands(
            input_img,
            sel_img_bands_img,
            "KEA",
            rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
            s_bands,
        )

    img_to_strch = sel_img_bands_img
    stretch_img = os.path.join(tmp_dir, f"{base_name}_stretch.kea")
    if stretch_file is not None:
        rsgislib.imageutils.stretch_img_with_stats(
            img_to_strch,
            stretch_img,
            stretch_file,
            "KEA",
            rsgislib.TYPE_8UINT,
            no_data_val,
            rsgislib.imageutils.STRETCH_LINEARMINMAX,
            2,
        )
    elif img_stats_msk is not None:
        img_to_strch_mskd = os.path.join(tmp_dir, f"{base_name}_MskdImg.kea")
        rsgislib.imageutils.mask_img(
            sel_img_bands_img,
            img_stats_msk,
            img_to_strch_mskd,
            "KEA",
            rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
            no_data_val,
            img_msk_vals,
        )
        stretch_img_stats = os.path.join(tmp_dir, f"{base_name}_stretch_statstmp.txt")
        stretch_img_tmp = os.path.join(tmp_dir, f"{base_name}_stretch_tmp.kea")
        rsgislib.imageutils.stretch_img(
            img_to_strch_mskd,
            stretch_img_tmp,
            True,
            stretch_img_stats,
            no_data_val,
            False,
            "KEA",
            rsgislib.TYPE_8UINT,
            rsgislib.imageutils.STRETCH_LINEARSTDDEV,
            2,
        )

        rsgislib.imageutils.stretch_img_with_stats(
            img_to_strch,
            stretch_img,
            stretch_img_stats,
            "KEA",
            rsgislib.TYPE_8UINT,
            no_data_val,
            rsgislib.imageutils.STRETCH_LINEARMINMAX,
            2,
        )
    else:
        rsgislib.imageutils.stretch_img(
            img_to_strch,
            stretch_img,
            False,
            "",
            no_data_val,
            False,
            "KEA",
            rsgislib.TYPE_8UINT,
            rsgislib.imageutils.STRETCH_LINEARSTDDEV,
            2,
        )

    if scale_axis == "auto":
        x_size, y_size = rsgislib.imageutils.get_img_size(stretch_img)
        if x_size > y_size:
            scale_axis = "width"
        else:
            scale_axis = "height"

    if n_out_imgs == 1:
        if not rsgislib.tools.filetools.does_path_exists_or_creatable(output_imgs):
            raise rsgislib.RSGISPyException(
                f"Output file path is not creatable: {output_imgs}"
            )
        cmd = ["gdal_translate", "-of", "JPEG", "-ot", "Byte"]
        if scale_axis == "width":
            cmd.append("-outsize")
            cmd.append(f"{int(output_img_sizes)}")
            cmd.append("0")
        else:
            cmd.append("-outsize")
            cmd.append("0")
            cmd.append(f"{int(output_img_sizes)}")
        cmd.append("-r")
        cmd.append("average")
        cmd.append(stretch_img)
        cmd.append(output_imgs)

        print(cmd)
        try:
            subprocess.run(cmd, check=True)
        except OSError as e:
            raise rsgislib.RSGISPyException("Could not execute command: {}".format(cmd))

    elif n_out_imgs > 1:
        for i in range(n_out_imgs):
            if not rsgislib.tools.filetools.does_path_exists_or_creatable(
                output_imgs[i]
            ):
                raise rsgislib.RSGISPyException(
                    f"Output file path is not creatable: {output_imgs}"
                )
            cmd = ["gdal_translate", "-of", "JPEG", "-ot", "Byte"]
            if scale_axis == "width":
                cmd.append("-outsize")
                cmd.append(f"{int(output_img_sizes[i])}")
                cmd.append("0")
            else:
                cmd.append("-outsize")
                cmd.append("0")
                cmd.append(f"{int(output_img_sizes[i])}")
            cmd.append("-r")
            cmd.append("average")
            cmd.append(stretch_img)
            cmd.append(output_imgs[i])

            print(cmd)
            try:
                subprocess.run(cmd, check=True)
            except OSError as e:
                raise rsgislib.RSGISPyException(
                    "Could not execute command: {}".format(cmd)
                )
    shutil.rmtree(tmp_dir)


def create_mbtile_file(
    input_img: str,
    bands: str,
    output_mbtiles: str,
    scale_input_img: int = 50,
    img_stats_msk: str = None,
    img_msk_vals: int = 1,
    tmp_dir: str = None,
    tile_format: str = "PNG",
    no_data_val: float = None,
):
    """
    A function to produce an MBTile file for visualisation.

    :param input_img: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected.
                  (e.g., '1', '1,2,3', '5,6,4')
    :param output_mbtiles: output MBTiles file which will be created.
    :param scale_input_img: The scale of the output image with respect to the input
                            as a percentage (e.g., 50% reduction in size).
    :param img_stats_msk: Optional (default=None) input image which is used to
                          define regions calculate the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the
                         image mask (can be list of values or single value).
    :param tmp_dir: an input directory which can be used to write temporary files /
                    directories. If not provided (i.e., input is None) then a local
                    directory will be define in the same folder as the input image.
    :param tile_format: Specify the tile file format to use, options: PNG, PNG8
                        and JPG. Default: PNG
    :param no_data_val: Optionally provide an input image no data value otherwise
                        it will be read from the input image header.

    """
    from osgeo import gdal

    band_lst = bands.split(",")
    multi_band = False
    if len(band_lst) == 3:
        multi_band = True
    elif len(band_lst) == 1:
        multi_band = False
    else:
        raise rsgislib.RSGISPyException(
            "You need to either provide 1 or 3 bands: {}".format(band_lst)
        )
    n_img_bands = rsgislib.imageutils.get_img_band_count(input_img)
    if no_data_val is None:
        no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)
    if no_data_val is None:
        raise rsgislib.RSGISPyException("No data value is not defined.")

    uid_str = rsgislib.tools.utils.uid_generator()
    if tmp_dir is not None:
        tmp_dir = os.path.join(tmp_dir, uid_str)
    else:
        tmp_dir = os.path.join(os.path.dirname(input_img), uid_str)
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
    base_name = rsgislib.tools.filetools.get_file_basename(input_img)

    sel_img_bands_img = ""
    if (n_img_bands == 1) and (not multi_band):
        sel_img_bands_img = input_img
    elif (
        (n_img_bands == 3)
        and (multi_band)
        and (band_lst[0] == "1")
        and (band_lst[1] == "2")
        and (band_lst[2] == "3")
    ):
        sel_img_bands_img = input_img
    else:
        s_bands = []
        for str_band in band_lst:
            s_bands.append(int(str_band))
        sel_img_bands_img = os.path.join(tmp_dir, f"{base_name}_sband.kea")
        rsgislib.imageutils.select_img_bands(
            input_img,
            sel_img_bands_img,
            "KEA",
            rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
            s_bands,
        )

    img_to_strch = sel_img_bands_img
    stretch_img = os.path.join(tmp_dir, f"{base_name}_stretch.kea")
    if img_stats_msk is not None:
        img_to_strch_mskd = os.path.join(tmp_dir, f"{base_name}_MskdImg.kea")
        rsgislib.imageutils.mask_img(
            sel_img_bands_img,
            img_stats_msk,
            img_to_strch_mskd,
            "KEA",
            rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
            no_data_val,
            img_msk_vals,
        )
        stretch_img_stats = os.path.join(tmp_dir, f"{base_name}_stretch_statstmp.txt")
        stretch_img_tmp = os.path.join(tmp_dir, f"{base_name}_stretch_tmp.kea")
        rsgislib.imageutils.stretch_img(
            img_to_strch_mskd,
            stretch_img_tmp,
            True,
            stretch_img_stats,
            no_data_val,
            False,
            "KEA",
            rsgislib.TYPE_8UINT,
            rsgislib.imageutils.STRETCH_LINEARSTDDEV,
            2,
        )
        rsgislib.imageutils.stretch_img_with_stats(
            img_to_strch,
            stretch_img,
            stretch_img_stats,
            "KEA",
            rsgislib.TYPE_8UINT,
            no_data_val,
            rsgislib.imageutils.STRETCH_LINEARMINMAX,
            2,
        )
    else:
        rsgislib.imageutils.stretch_img(
            img_to_strch,
            stretch_img,
            False,
            "",
            no_data_val,
            False,
            "KEA",
            rsgislib.TYPE_8UINT,
            rsgislib.imageutils.STRETCH_LINEARSTDDEV,
            2,
        )

    stretch_img_re_proj = os.path.join(tmp_dir, f"{base_name}_stretch_epsg3857.kea")
    rsgislib.imageutils.gdal_warp(
        stretch_img,
        stretch_img_re_proj,
        3857,
        interp_method=rsgislib.INTERP_NEAREST_NEIGHBOUR,
        gdalformat="KEA",
        options=[],
    )

    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    trans_opt = gdal.TranslateOptions(
        format="MBTiles",
        widthPct=scale_input_img,
        heightPct=scale_input_img,
        noData=0,
        creationOptions=["TILE_FORMAT={}".format(tile_format)],
        callback=callback,
    )
    gdal.Translate(output_mbtiles, stretch_img_re_proj, options=trans_opt)
    rsgislib.imageutils.pop_img_stats(
        output_mbtiles, use_no_data=True, no_data_val=0, calc_pyramids=True
    )
    shutil.rmtree(tmp_dir)


def create_webtiles_vis_gtiff_img(
    input_img: str,
    bands: str,
    output_dir: str,
    scaled_gtiff_img: str,
    zoom_levels: str = "2-10",
    img_stats_msk: str = None,
    img_msk_vals: int = 1,
    stretch_file: str = None,
    tmp_dir: str = None,
    webview: bool = True,
    scale: float = 0,
    tms: bool = True,
    no_data_val: float = None,
):
    """
    A function to produce web cache and scaled and stretched geotiff.

    :param input_img: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected.
                  (e.g., '1', '1,2,3', '5,6,4')
    :param output_dir: output directory within which the cache will be created.
    :param scaled_gtiff_img: output geotiff image path and file name.
    :param zoom_levels: The zoom levels to be created for the web tile cache.
    :param img_stats_msk: Optional (default=None) input image which is used to
                          define regions calculate the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the
                         image mask (can be list of values or single value).
    :param stretch_file: a stretch stats file to standardise the stretch between
                         a number of input files.
    :param tmp_dir: an input directory which can be used to write temporary files /
                    directories. If not provided (i.e., input is None) then a local
                    directory will be define in the same folder as the input image.
    :param webview: Provide default GDAL leaflet web viewer.
    :param scale: the scale output geotiff. Input is percentage in the x-axis.
                  If zero (default) then no scaling will be applied.
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
                (TMS: gdal2tiles.py native. XYZ: GIS Compatible)
    :param no_data_val: Optionally provide an input image no data value otherwise
                        it will be read from the input image header.

    """
    if not rsgislib.tools.filetools.does_path_exists_or_creatable(output_dir):
        raise rsgislib.RSGISPyException(
            f"Output file path is not creatable: {output_dir}"
        )
    if not rsgislib.tools.filetools.does_path_exists_or_creatable(scaled_gtiff_img):
        raise rsgislib.RSGISPyException(
            f"Output file path is not creatable: {scaled_gtiff_img}"
        )

    band_lst = bands.split(",")
    multi_band = False
    if len(band_lst) == 3:
        multi_band = True
    elif len(band_lst) == 1:
        multi_band = False
    else:
        raise rsgislib.RSGISPyException(
            "You need to either provide 1 or 3 bands: {}".format(band_lst)
        )
    n_img_bands = rsgislib.imageutils.get_img_band_count(input_img)
    if no_data_val is None:
        no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)
    if no_data_val is None:
        raise rsgislib.RSGISPyException("No data value is not defined.")

    uid_str = rsgislib.tools.utils.uid_generator()
    if tmp_dir is not None:
        tmp_dir = os.path.join(tmp_dir, uid_str)
    else:
        tmp_dir = os.path.join(os.path.dirname(input_img), uid_str)
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
    base_name = rsgislib.tools.filetools.get_file_basename(input_img)

    sel_img_bands_img = ""
    if (n_img_bands == 1) and (not multi_band):
        sel_img_bands_img = input_img
    elif (
        (n_img_bands == 3)
        and (multi_band)
        and (band_lst[0] == "1")
        and (band_lst[1] == "2")
        and (band_lst[2] == "3")
    ):
        sel_img_bands_img = input_img
    else:
        s_bands = []
        for str_band in band_lst:
            s_bands.append(int(str_band))
        sel_img_bands_img = os.path.join(tmp_dir, f"{base_name}_sband.kea")
        rsgislib.imageutils.select_img_bands(
            input_img,
            sel_img_bands_img,
            "KEA",
            rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
            s_bands,
        )

    img_to_strch = sel_img_bands_img
    stretch_img = os.path.join(tmp_dir, f"{base_name}_stretch.kea")
    if stretch_file is not None:
        rsgislib.imageutils.stretch_img_with_stats(
            img_to_strch,
            stretch_img,
            stretch_file,
            "KEA",
            rsgislib.TYPE_8UINT,
            no_data_val,
            rsgislib.imageutils.STRETCH_LINEARMINMAX,
            2,
        )
    elif img_stats_msk is not None:
        img_to_strch_mskd = os.path.join(tmp_dir, f"{base_name}_MskdImg.kea")
        rsgislib.imageutils.mask_img(
            sel_img_bands_img,
            img_stats_msk,
            img_to_strch_mskd,
            "KEA",
            rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
            no_data_val,
            img_msk_vals,
        )
        stretch_img_stats = os.path.join(tmp_dir, f"{base_name}_stretch_statstmp.txt")
        stretch_img_tmp = os.path.join(tmp_dir, f"{base_name}_stretch_tmp.kea")
        rsgislib.imageutils.stretch_img(
            img_to_strch_mskd,
            stretch_img_tmp,
            True,
            stretch_img_stats,
            no_data_val,
            False,
            "KEA",
            rsgislib.TYPE_8UINT,
            rsgislib.imageutils.STRETCH_LINEARSTDDEV,
            2,
        )

        rsgislib.imageutils.stretch_img_with_stats(
            img_to_strch,
            stretch_img,
            stretch_img_stats,
            "KEA",
            rsgislib.TYPE_8UINT,
            no_data_val,
            rsgislib.imageutils.STRETCH_LINEARMINMAX,
            2,
        )
    else:
        rsgislib.imageutils.stretch_img(
            img_to_strch,
            stretch_img,
            False,
            "",
            no_data_val,
            False,
            "KEA",
            rsgislib.TYPE_8UINT,
            rsgislib.imageutils.STRETCH_LINEARSTDDEV,
            2,
        )

    webview_opt = "none"
    if webview:
        webview_opt = "leaflet"

    cmd = [
        "gdal2tiles.py",
        "-r",
        "average",
        "-z",
        zoom_levels,
        "-a",
        "0",
        "-w",
        webview_opt,
        stretch_img,
        output_dir,
    ]
    print(cmd)
    try:
        subprocess.run(cmd, check=True)
    except OSError as e:
        raise rsgislib.RSGISPyException("Could not execute command: {}".format(cmd))

    if not tms:
        rsgislib.tools.tilecacheutils.convert_between_tms_xyz(output_dir)

    if scale > 0:
        cmd = [
            "gdal_translate",
            "-of",
            "GTIFF",
            "-co",
            "TILED=YES",
            "-co",
            "COMPRESS=JPEG",
            "-co",
            "BIGTIFF=NO",
            "-ot",
            "Byte",
            "-outsize",
            f"{scale}%",
            "0",
            "-r",
            "average",
            stretch_img,
            scaled_gtiff_img,
        ]
    else:
        cmd = [
            "gdal_translate",
            "-of",
            "GTIFF",
            "-co",
            "TILED=YES",
            "-co",
            "COMPRESS=JPEG",
            "-co",
            "BIGTIFF=NO",
            "-ot",
            "Byte",
            "-r",
            "average",
            stretch_img,
            scaled_gtiff_img,
        ]

    try:
        subprocess.run(cmd, check=True)
    except OSError as e:
        raise rsgislib.RSGISPyException("Could not execute command: {}".format(cmd))
    rsgislib.imageutils.pop_img_stats(
        scaled_gtiff_img, use_no_data=True, no_data_val=0, calc_pyramids=True
    )
    shutil.rmtree(tmp_dir)


def create_quicklook_overview_imgs(
    input_imgs: List[str],
    bands: str,
    tmp_dir: str,
    output_imgs: Union[str, List[str]] = "quicklook.jpg",
    output_img_sizes: Union[int, List[int]] = 250,
    scale_axis: str = "auto",
    stretch_file: str = None,
    no_data_val: float = None,
):
    """
    A function to produce an overview quicklook for a number of input images.

    :param input_imgs: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected.
                  (e.g., '1', '1,2,3', '5,6,4')
    :param tmp_dir: an input directory which can be used to write temporary
                    files / directories.
    :param output_imgs: a single output image or list of output images. The same
                        size as output_img_sizes. The output format is JPEG.
    :param output_img_sizes: the output image size (in pixels) or list of output
                             image sizes.
    :param scale_axis: the axis to which the output_img_sizes refer. Options: width,
                       height or auto. Auto applies the output_img_sizes to the
                       longest of the two axes.
    :param stretch_file: a stretch stats file to standardise the stretch between
                         a number of input files.
    :param no_data_val: Optionally provide an input image no data value otherwise
                        it will be read from the input image header.

    """
    if scale_axis not in ["width", "height", "auto"]:
        raise rsgislib.RSGISPyException(
            "Input parameter 'scale_axis' must have the value "
            "'width', 'height' or 'auto'."
        )

    if type(input_imgs) is not list:
        raise rsgislib.RSGISPyException("Input images must be a list")
    n_in_imgs = len(input_imgs)

    n_out_imgs = 1
    if type(output_imgs) is list:
        n_out_imgs = len(output_imgs)
        if type(output_img_sizes) is not list:
            raise rsgislib.RSGISPyException(
                "If the output_imgs input is a list so must output_img_sizes."
            )
        if len(output_img_sizes) != n_out_imgs:
            raise rsgislib.RSGISPyException(
                "output_imgs and output_img_sizes must be the same length"
            )

        if n_out_imgs == 1:
            output_imgs = output_imgs[0]
            output_img_sizes = output_img_sizes[0]

    uid_str = rsgislib.tools.utils.uid_generator()
    usr_tmp_dir = os.path.join(tmp_dir, "qklk_tmp_{}".format(uid_str))
    if not os.path.exists(usr_tmp_dir):
        os.makedirs(usr_tmp_dir)

    img_basename = rsgislib.tools.filetools.get_file_basename(
        input_imgs[0], check_valid=True
    )

    band_lst = bands.split(",")
    multi_band = False
    if len(band_lst) == 3:
        multi_band = True
    elif len(band_lst) == 1:
        multi_band = False
    else:
        raise rsgislib.RSGISPyException(
            "You need to either provide 1 or 3 bands: {}".format(band_lst)
        )

    s_bands = []
    for str_band in band_lst:
        s_bands.append(int(str_band))

    n_img_bands = rsgislib.imageutils.get_img_band_count(input_imgs[0])

    usr_def_no_data = False
    if no_data_val is None:
        no_data_val = rsgislib.imageutils.get_img_no_data_value(input_imgs[0])
    else:
        usr_def_no_data = True
    if no_data_val is None:
        raise rsgislib.RSGISPyException("No data value is not defined.")

    b_sel_imgs = []
    for img in input_imgs:
        tmp_n_bands = rsgislib.imageutils.get_img_band_count(img)
        tmp_no_data_val = rsgislib.imageutils.get_img_no_data_value(img)

        if tmp_n_bands != n_img_bands:
            raise rsgislib.RSGISPyException(
                "The number of bands in the input images is different."
            )

        if (not usr_def_no_data) and (tmp_no_data_val != no_data_val):
            raise rsgislib.RSGISPyException(
                "The no data value is different between the input images."
            )

        sel_img_bands_img = ""
        if (n_img_bands == 1) and (not multi_band):
            sel_img_bands_img = img
        elif (
            (n_img_bands == 3)
            and (multi_band)
            and (band_lst[0] == "1")
            and (band_lst[1] == "2")
            and (band_lst[2] == "3")
        ):
            sel_img_bands_img = img
        else:
            lcl_img_basename = rsgislib.tools.filetools.get_file_basename(
                img, check_valid=True
            )
            sel_img_bands_img = os.path.join(
                usr_tmp_dir, "{}_sband.kea".format(lcl_img_basename)
            )
            rsgislib.imageutils.select_img_bands(
                img,
                sel_img_bands_img,
                "KEA",
                rsgislib.imageutils.get_rsgislib_datatype_from_img(img),
                s_bands,
            )
            rsgislib.imageutils.set_img_no_data_value(sel_img_bands_img, no_data_val)
        b_sel_imgs.append(sel_img_bands_img)

    tmp_vrt_img = os.path.join(usr_tmp_dir, "{}_{}.vrt".format(img_basename, uid_str))
    rsgislib.imageutils.create_mosaic_images_vrt(b_sel_imgs, tmp_vrt_img)

    stretch_img = os.path.join(usr_tmp_dir, "{}_stretch.kea".format(img_basename))
    if stretch_file is not None:
        rsgislib.imageutils.stretch_img_with_stats(
            tmp_vrt_img,
            stretch_img,
            stretch_file,
            "KEA",
            rsgislib.TYPE_8UINT,
            no_data_val,
            rsgislib.imageutils.STRETCH_LINEARMINMAX,
            2,
        )
    else:
        rsgislib.imageutils.stretch_img(
            tmp_vrt_img,
            stretch_img,
            False,
            "",
            no_data_val,
            False,
            "KEA",
            rsgislib.TYPE_8UINT,
            rsgislib.imageutils.STRETCH_LINEARSTDDEV,
            2,
        )

    if scale_axis == "auto":
        x_size, y_size = rsgislib.imageutils.get_img_size(stretch_img)
        if x_size > y_size:
            scale_axis = "width"
        else:
            scale_axis = "height"

    if n_out_imgs == 1:
        if not rsgislib.tools.filetools.does_path_exists_or_creatable(output_imgs):
            raise rsgislib.RSGISPyException(
                f"Output file path is not creatable: {output_imgs}"
            )
        cmd = ["gdal_translate", "-of", "JPEG", "-ot", "Byte"]
        if scale_axis == "width":
            cmd.append("-outsize")
            cmd.append(f"{int(output_img_sizes)}")
            cmd.append("0")
        else:
            cmd.append("-outsize")
            cmd.append("0")
            cmd.append(f"{int(output_img_sizes)}")

        cmd.append("-r")
        cmd.append("average")
        cmd.append(stretch_img)
        cmd.append(output_imgs)

        print(cmd)
        try:
            subprocess.run(cmd, check=True)
        except OSError as e:
            raise rsgislib.RSGISPyException("Could not execute command: {}".format(cmd))

    elif n_out_imgs > 1:
        for i in range(n_out_imgs):
            if not rsgislib.tools.filetools.does_path_exists_or_creatable(
                output_imgs[i]
            ):
                raise rsgislib.RSGISPyException(
                    f"Output file path is not creatable: {output_imgs[i]}"
                )
            cmd = ["gdal_translate", "-of", "JPEG", "-ot", "Byte"]
            if scale_axis == "width":
                cmd.append("-outsize")
                cmd.append(f"{int(output_img_sizes[i])}")
                cmd.append("0")
            else:
                cmd.append("-outsize")
                cmd.append("0")
                cmd.append(f"{int(output_img_sizes[i])}")

            cmd.append("-r")
            cmd.append("average")
            cmd.append(stretch_img)
            cmd.append(output_imgs[i])

            print(cmd)
            try:
                subprocess.run(cmd, check=True)
            except OSError as e:
                raise rsgislib.RSGISPyException(
                    "Could not execute command: {}".format(cmd)
                )
    shutil.rmtree(usr_tmp_dir)


def burn_in_binary_msk(
    base_image: str, mask_img: str, output_img: str, gdalformat: str, msk_colour=None
):
    """
    A function which is used for visualisation applications where a rasterised
    binary mask is 'burnt' into a base image with a user selected colour.

    :param base_image: the base image with continuous output values. It is expected
                       that this image has 8uint pixel values.
    :param mask_img: the binary mask (value 1 will be used) to be 'burnt'
                     into the base_image.
    :param output_img: the output image file name and path
    :param gdalformat: the GDAL image file format of the output image file.
    :param msk_colour: the colour of the pixels burnt in to the base image. Should
                       have the same number of dimensions as the input base image
                       has image bands. Default, if None is 255.

    """
    import numpy
    from rios import applier

    n_img_bands = rsgislib.imageutils.get_img_band_count(base_image)
    if msk_colour is None:
        msk_colour = list()
        for i in range(n_img_bands):
            msk_colour.append(255)
    elif len(msk_colour) != n_img_bands:
        raise rsgislib.RSGISPyException(
            "The number of image bands and length of "
            "the msk_colour array should be equal."
        )

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.base_image = base_image
    infiles.mask_img = mask_img
    outfiles = applier.FilenameAssociations()
    outfiles.output_img = output_img
    otherargs = applier.OtherInputs()
    otherargs.msk_colour = msk_colour
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _burn_in_values(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        outputs.output_img = numpy.copy(inputs.base_image)
        for n in range(inputs.base_image.shape[0]):
            outputs.output_img[n] = numpy.where(
                inputs.mask_img == 1, otherargs.msk_colour[n], outputs.output_img[n]
            )

    applier.apply(_burn_in_values, infiles, outfiles, otherargs, controls=aControls)


def create_quicklook_overview_imgs_vec_overlay(
    input_imgs: List[str],
    bands: str,
    tmp_dir: str,
    vec_overlay_file: str,
    vec_overlay_lyr: str,
    output_imgs: Union[str, List[str]] = "quicklook.jpg",
    output_img_sizes: Union[int, List[int]] = 250,
    gdalformat: str = "JPEG",
    scale_axis: str = "auto",
    stretch_file: str = None,
    overlay_clrs: List[int] = None,
    no_data_val: float = None,
):
    """
    A function to produce an overview quicklook with a vector overlain for
    a number of input images.

    :param input_imgs: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected.
                  (e.g., '1', '1,2,3', '5,6,4')
    :param tmp_dir: an input directory which can be used to write temporary
                    files / directories.
    :param vec_overlay_file: an vector overlay which will be rasterised to the
                             overlay on the output images.
    :param vec_overlay_lyr: the layer name for the vector overlay.
    :param output_imgs: a single output image or list of output images. The same size
                        as output_img_sizes.
    :param output_img_sizes: the output image size (in pixels) or list of output
                             image sizes.
    :param gdalformat: the output file format - probably either JPG, PNG or GTIFF.
    :param scale_axis: the axis to which the output_img_sizes refer. Options: width,
                       height or auto. Auto applies the output_img_sizes to the
                       longest of the two axes.
    :param stretch_file: a stretch stats file to standardise the stretch between a
                         number of input files.
    :param overlay_clrs: list of output pixel values defining colour for the overlay
                         image (value between 0-255). The list should be the same
                         length as the number of input image bands (i.e., specified
                         by bands). If None then default all values are all 255.
    :param no_data_val: Optionally provide an input image no data value otherwise
                        it will be read from the input image header.

    """
    import rsgislib.vectorutils.createrasters

    if scale_axis not in ["width", "height", "auto"]:
        raise rsgislib.RSGISPyException(
            "Input parameter 'scale_axis' must have "
            "the value 'width', 'height' or 'auto'."
        )

    if type(input_imgs) is not list:
        raise rsgislib.RSGISPyException("Input images must be a list")
    n_in_imgs = len(input_imgs)

    n_out_imgs = 1
    if type(output_imgs) is list:
        n_out_imgs = len(output_imgs)
        if type(output_img_sizes) is not list:
            raise rsgislib.RSGISPyException(
                "If the outputImgs input is a list so must output_img_sizes."
            )
        if len(output_img_sizes) != n_out_imgs:
            raise rsgislib.RSGISPyException(
                "outputImgs and output_img_sizes must be the same length"
            )

        if n_out_imgs == 1:
            output_imgs = output_imgs[0]
            output_img_sizes = output_img_sizes[0]

    uid_str = rsgislib.tools.utils.uid_generator()
    usr_tmp_dir = os.path.join(tmp_dir, "qklk_tmp_{}".format(uid_str))
    if not os.path.exists(usr_tmp_dir):
        os.makedirs(usr_tmp_dir)

    img_basename = rsgislib.tools.filetools.get_file_basename(
        input_imgs[0], check_valid=True
    )
    band_lst = bands.split(",")
    multi_band = False
    if len(band_lst) == 3:
        multi_band = True
        out_n_bands = 3
    elif len(band_lst) == 1:
        multi_band = False
        out_n_bands = 1
    else:
        print(band_lst)
        raise rsgislib.RSGISPyException("You need to either provide 1 or 3 bands.")

    s_bands = []
    for strBand in band_lst:
        s_bands.append(int(strBand))

    n_img_bands = rsgislib.imageutils.get_img_band_count(input_imgs[0])
    usr_def_no_data = False
    if no_data_val is None:
        no_data_val = rsgislib.imageutils.get_img_no_data_value(input_imgs[0])
    else:
        usr_def_no_data = True
    if no_data_val is None:
        raise rsgislib.RSGISPyException("No data value is not defined.")

    b_sel_imgs = []
    for img in input_imgs:
        tmp_n_bands = rsgislib.imageutils.get_img_band_count(img)
        tmp_no_data_val = rsgislib.imageutils.get_img_no_data_value(img)

        if tmp_n_bands != n_img_bands:
            raise rsgislib.RSGISPyException(
                "The number of bands in the input images is different."
            )

        if (not usr_def_no_data) and (tmp_no_data_val != no_data_val):
            raise rsgislib.RSGISPyException(
                "The no data value is different between the input images."
            )

        sel_img_bands_img = ""
        if (n_img_bands == 1) and (not multi_band):
            sel_img_bands_img = img
        elif (
            (n_img_bands == 3)
            and (multi_band)
            and (band_lst[0] == "1")
            and (band_lst[1] == "2")
            and (band_lst[2] == "3")
        ):
            sel_img_bands_img = img
        else:
            lcl_img_basename = rsgislib.tools.filetools.get_file_basename(
                img, check_valid=True
            )
            sel_img_bands_img = os.path.join(
                usr_tmp_dir, "{}_sband.kea".format(lcl_img_basename)
            )
            rsgislib.imageutils.select_img_bands(
                img,
                sel_img_bands_img,
                "KEA",
                rsgislib.imageutils.get_rsgislib_datatype_from_img(img),
                s_bands,
            )
            rsgislib.imageutils.set_img_no_data_value(sel_img_bands_img, no_data_val)
        b_sel_imgs.append(sel_img_bands_img)

    if overlay_clrs is None:
        overlay_clrs = list()
        for i in range(out_n_bands):
            overlay_clrs.append(255)

    tmp_vrt_img = os.path.join(usr_tmp_dir, "{}_{}.vrt".format(img_basename, uid_str))
    rsgislib.imageutils.create_mosaic_images_vrt(b_sel_imgs, tmp_vrt_img)

    stretch_img = os.path.join(usr_tmp_dir, "{}_stretch.kea".format(img_basename))
    if stretch_file is not None:
        rsgislib.imageutils.stretch_img_with_stats(
            tmp_vrt_img,
            stretch_img,
            stretch_file,
            "KEA",
            rsgislib.TYPE_8UINT,
            no_data_val,
            rsgislib.imageutils.STRETCH_LINEARMINMAX,
            2,
        )
    else:
        rsgislib.imageutils.stretch_img(
            tmp_vrt_img,
            stretch_img,
            False,
            "",
            no_data_val,
            False,
            "KEA",
            rsgislib.TYPE_8UINT,
            rsgislib.imageutils.STRETCH_LINEARSTDDEV,
            2,
        )

    if scale_axis == "auto":
        x_size, y_size = rsgislib.imageutils.get_img_size(stretch_img)
        if x_size > y_size:
            scale_axis = "width"
        else:
            scale_axis = "height"

    if n_out_imgs == 1:
        cmd = ["gdal_translate", "-of", "KEA", "-ot", "Byte"]
        if scale_axis == "width":
            cmd.append("-outsize")
            cmd.append(f"{int(output_img_sizes)}")
            cmd.append("0")
        else:
            cmd.append("-outsize")
            cmd.append("0")
            cmd.append(f"{int(output_img_sizes)}")

        # Create the resized output image.
        lcl_img_basename = rsgislib.tools.filetools.get_file_basename(
            output_imgs, check_valid=True
        )
        tmp_resized_img = os.path.join(
            usr_tmp_dir, "{}_resized.kea".format(lcl_img_basename)
        )

        cmd.append("-r")
        cmd.append("average")
        cmd.append(stretch_img)
        cmd.append(tmp_resized_img)

        print(cmd)
        try:
            subprocess.run(cmd, check=True)
        except OSError as e:
            raise rsgislib.RSGISPyException("Could not execute command: {}".format(cmd))
        # Rasterise the overlay vector to the output raster grid.
        tmp_vec_overlay_img = os.path.join(
            usr_tmp_dir, "{}_vec_overlay.kea".format(lcl_img_basename)
        )
        rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
            vec_overlay_file,
            vec_overlay_lyr,
            tmp_resized_img,
            tmp_vec_overlay_img,
            gdalformat="KEA",
            burn_val=1,
            datatype=rsgislib.TYPE_8UINT,
            att_column=None,
            thematic=True,
            no_data_val=0,
        )
        # Merge the overlay and base image
        tmp_final_img = os.path.join(
            usr_tmp_dir, "{}_final.kea".format(lcl_img_basename)
        )
        burn_in_binary_msk(
            tmp_resized_img, tmp_vec_overlay_img, tmp_final_img, "KEA", overlay_clrs
        )
        # Convert to final format (e.g., JPG, TIFF or PNG)
        if not rsgislib.tools.filetools.does_path_exists_or_creatable(output_imgs):
            raise rsgislib.RSGISPyException(
                f"Output file path is not creatable: {output_imgs}"
            )
        rsgislib.imageutils.gdal_translate(
            tmp_final_img, output_imgs, gdalformat=gdalformat, options=""
        )

    elif n_out_imgs > 1:
        for i in range(n_out_imgs):
            cmd = ["gdal_translate", "-of", "KEA", "-ot", "Byte"]
            if scale_axis == "width":
                cmd.append("-outsize")
                cmd.append(f"{int(output_img_sizes[i])}")
                cmd.append("0")
            else:
                cmd.append("-outsize")
                cmd.append("0")
                cmd.append(f"{int(output_img_sizes[i])}")

            # Create the resized output image.
            lcl_img_basename = rsgislib.tools.filetools.get_file_basename(
                output_imgs[i], check_valid=True
            )
            tmp_resized_img = os.path.join(
                usr_tmp_dir, "{}_resized.kea".format(lcl_img_basename)
            )
            cmd.append("-r")
            cmd.append("average")
            cmd.append(stretch_img)
            cmd.append(tmp_resized_img)

            print(cmd)
            try:
                subprocess.run(cmd, check=True)
            except OSError as e:
                raise rsgislib.RSGISPyException(
                    "Could not execute command: {}".format(cmd)
                )
            # Rasterise the overlay vector to the output raster grid.
            tmp_vec_overlay_img = os.path.join(
                usr_tmp_dir, "{}_vec_overlay.kea".format(lcl_img_basename)
            )
            rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
                vec_overlay_file,
                vec_overlay_lyr,
                tmp_resized_img,
                tmp_vec_overlay_img,
                gdalformat="KEA",
                burn_val=1,
                datatype=rsgislib.TYPE_8UINT,
                att_column=None,
                thematic=True,
                no_data_val=0,
            )
            # Merge the overlay and base image
            tmp_final_img = os.path.join(
                usr_tmp_dir, "{}_final.kea".format(lcl_img_basename)
            )
            burn_in_binary_msk(
                tmp_resized_img, tmp_vec_overlay_img, tmp_final_img, "KEA", overlay_clrs
            )
            # Convert to final format (e.g., JPG, TIFF or PNG)
            if not rsgislib.tools.filetools.does_path_exists_or_creatable(
                output_imgs[i]
            ):
                raise rsgislib.RSGISPyException(
                    f"Output file path is not creatable: {output_imgs[i]}"
                )
            rsgislib.imageutils.gdal_translate(
                tmp_final_img, output_imgs[i], gdalformat=gdalformat, options=""
            )
    shutil.rmtree(usr_tmp_dir)


def create_visual_overview_imgs_vec_extent(
    input_imgs: List[str],
    bands: str,
    tmp_dir: str,
    vec_extent_file: str = None,
    vec_extent_lyr: str = None,
    output_imgs: Union[str, List[str]] = "quicklook.tif",
    output_img_sizes: Union[int, List[int]] = 500,
    gdalformat: str = "GTIFF",
    scale_axis: str = "auto",
    stretch_file: str = None,
    export_stretch_file: bool = False,
    no_data_val: float = None,
):
    """
    A function to produce an 8bit overview image (i.e., stretched visualisation)
    with an optional specified extent.

    :param input_imgs: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected.
                  (e.g., '1', '1,2,3', '5,6,4')
    :param tmp_dir: an input directory which can be used to write temporary
                    files / directories.
    :param vec_extent_file: an vector file to define the extent of the output
                            image files.
    :param vec_extent_lyr: the layer name for the vector extent.
    :param output_imgs: a single output image or list of output images. The same
                        size as output_img_sizes.
    :param output_img_sizes: the output image size (in pixels) or list of output
                             image sizes.
    :param gdalformat: the output file format - probably either JPG, PNG or GTIFF.
    :param scale_axis: the axis to which the output_img_sizes refer. Options: width,
                       height or auto. Auto applies the output_img_sizes to the
                       longest of the two axes.
    :param stretch_file: a stretch stats file to standardise the stretch between a
                         number of input files. If export_stretch_file is True then
                         this variable is used as the output stretch file path.
    :param export_stretch_file: If true then the stretch parameters are outputted
                                as a text file to the path defined by stretch_file.
    :param no_data_val: Optionally provide an input image no data value otherwise
                        it will be read from the input image header.

    """
    import rsgislib.vectorutils.createvectors

    if (vec_extent_file is not None) and (vec_extent_lyr is None):
        raise rsgislib.RSGISPyException(
            "If a vec_extent_file is given (i.e., "
            "not None) then a vec_extent_lyr must also "
            "be provided."
        )

    if scale_axis not in ["width", "height", "auto"]:
        raise rsgislib.RSGISPyException(
            "Input parameter 'scale_axis' must have the value "
            "'width', 'height' or 'auto'."
        )

    if export_stretch_file and (stretch_file is None):
        raise rsgislib.RSGISPyException(
            "If export_stretch_file is True then a "
            "stretch_file path must be provided."
        )

    if type(input_imgs) is not list:
        raise rsgislib.RSGISPyException("Input images must be a list")
    n_in_imgs = len(input_imgs)

    n_out_imgs = 1
    if type(output_imgs) is list:
        n_out_imgs = len(output_imgs)
        if type(output_img_sizes) is not list:
            raise rsgislib.RSGISPyException(
                "If the output_imgs input is a list so must output_img_sizes."
            )
        if len(output_img_sizes) != n_out_imgs:
            raise rsgislib.RSGISPyException(
                "output_imgs and output_img_sizes must be the same length"
            )

        if n_out_imgs == 1:
            output_imgs = output_imgs[0]
            output_img_sizes = output_img_sizes[0]

    uid_str = rsgislib.tools.utils.uid_generator()
    usr_tmp_dir = os.path.join(tmp_dir, "qklk_tmp_{}".format(uid_str))
    if not os.path.exists(usr_tmp_dir):
        os.makedirs(usr_tmp_dir)

    img_basename = rsgislib.tools.filetools.get_file_basename(
        input_imgs[0], check_valid=True
    )
    band_lst = bands.split(",")
    multi_band = False
    if len(band_lst) == 3:
        multi_band = True
    elif len(band_lst) == 1:
        multi_band = False
    else:
        print(band_lst)
        raise rsgislib.RSGISPyException("You need to either provide 1 or 3 bands.")

    s_bands = []
    for str_band in band_lst:
        s_bands.append(int(str_band))

    n_img_bands = rsgislib.imageutils.get_img_band_count(input_imgs[0])

    usr_def_no_data = False
    if no_data_val is None:
        no_data_val = rsgislib.imageutils.get_img_no_data_value(input_imgs[0])
    else:
        usr_def_no_data = True
    if no_data_val is None:
        raise rsgislib.RSGISPyException("No data value is not defined.")

    b_sel_imgs = []
    for img in input_imgs:
        tmp_n_bands = rsgislib.imageutils.get_img_band_count(img)
        tmp_no_data_val = rsgislib.imageutils.get_img_no_data_value(img)

        if tmp_n_bands != n_img_bands:
            raise rsgislib.RSGISPyException(
                "The number of bands in the input images is different."
            )

        if (not usr_def_no_data) and (tmp_no_data_val != no_data_val):
            raise rsgislib.RSGISPyException(
                "The no data value is different between the input images."
            )

        sel_img_bands_img = ""
        if (n_img_bands == 1) and (not multi_band):
            sel_img_bands_img = img
        elif (
            (n_img_bands == 3)
            and (multi_band)
            and (band_lst[0] == "1")
            and (band_lst[1] == "2")
            and (band_lst[2] == "3")
        ):
            sel_img_bands_img = img
        else:
            lcl_img_basename = rsgislib.tools.filetools.get_file_basename(
                img, check_valid=True
            )
            sel_img_bands_img = os.path.join(
                usr_tmp_dir, "{}_sband.kea".format(lcl_img_basename)
            )
            rsgislib.imageutils.select_img_bands(
                img,
                sel_img_bands_img,
                "KEA",
                rsgislib.imageutils.get_rsgislib_datatype_from_img(img),
                s_bands,
            )
            rsgislib.imageutils.set_img_no_data_value(sel_img_bands_img, no_data_val)
        b_sel_imgs.append(sel_img_bands_img)

    tmp_vrt_img = os.path.join(usr_tmp_dir, "{}_{}.vrt".format(img_basename, uid_str))
    if vec_extent_file is None:
        rsgislib.imageutils.create_mosaic_images_vrt(b_sel_imgs, tmp_vrt_img)
    else:
        vec_extent_bbox = rsgislib.vectorutils.get_vec_layer_extent(
            vec_extent_file, vec_extent_lyr, compute_if_exp=True
        )
        vrt_extent = [
            vec_extent_bbox[0],
            vec_extent_bbox[2],
            vec_extent_bbox[1],
            vec_extent_bbox[3],
        ]
        rsgislib.imageutils.create_mosaic_images_vrt(
            b_sel_imgs, tmp_vrt_img, vrt_extent
        )

    stretch_img = os.path.join(usr_tmp_dir, "{}_stretch.kea".format(img_basename))
    if (stretch_file is not None) and (export_stretch_file == False):
        rsgislib.imageutils.stretch_img_with_stats(
            tmp_vrt_img,
            stretch_img,
            stretch_file,
            "KEA",
            rsgislib.TYPE_8UINT,
            no_data_val,
            rsgislib.imageutils.STRETCH_LINEARMINMAX,
            2,
        )
    else:
        rsgislib.imageutils.stretch_img(
            tmp_vrt_img,
            stretch_img,
            export_stretch_file,
            stretch_file,
            no_data_val,
            False,
            "KEA",
            rsgislib.TYPE_8UINT,
            rsgislib.imageutils.STRETCH_LINEARSTDDEV,
            2,
        )
    if scale_axis == "auto":
        x_size, y_size = rsgislib.imageutils.get_img_size(stretch_img)
        if x_size > y_size:
            scale_axis = "width"
        else:
            scale_axis = "height"

    if n_out_imgs == 1:
        if not rsgislib.tools.filetools.does_path_exists_or_creatable(output_imgs):
            raise rsgislib.RSGISPyException(
                f"Output file path is not creatable: {output_imgs}"
            )
        cmd = ["gdal_translate", "-of", gdalformat, "-ot", "Byte"]
        if scale_axis == "width":
            cmd.append("-outsize")
            cmd.append(f"{int(output_img_sizes)}")
            cmd.append("0")
        else:
            cmd.append("-outsize")
            cmd.append("0")
            cmd.append(f"{int(output_img_sizes)}")

        cmd.append("-r")
        cmd.append("average")
        cmd.append(stretch_img)
        cmd.append(output_imgs)

        print(cmd)
        try:
            subprocess.run(cmd, check=True)
        except OSError as e:
            raise rsgislib.RSGISPyException("Could not execute command: {}".format(cmd))
    else:
        for i in range(n_out_imgs):
            if not rsgislib.tools.filetools.does_path_exists_or_creatable(
                output_imgs[i]
            ):
                raise rsgislib.RSGISPyException(
                    f"Output file path is not creatable: {output_imgs[i]}"
                )
            cmd = ["gdal_translate", "-of", gdalformat, "-ot", "Byte"]
            if scale_axis == "width":
                cmd.append("-outsize")
                cmd.append(f"{int(output_img_sizes[i])}")
                cmd.append("0")
            else:
                cmd.append("-outsize")
                cmd.append("0")
                cmd.append(f"{int(output_img_sizes[i])}")

            cmd.append("-r")
            cmd.append("average")
            cmd.append(stretch_img)
            cmd.append(output_imgs[i])

            print(cmd)
            try:
                subprocess.run(cmd, check=True)
            except OSError as e:
                raise rsgislib.RSGISPyException(
                    "Could not execute command: {}".format(cmd)
                )
    shutil.rmtree(usr_tmp_dir)


def overlay_vec_on_img(
    input_img: str,
    output_img: str,
    vec_overlay_file: str,
    vec_overlay_lyr: str,
    tmp_dir: str,
    gdalformat: str = "PNG",
    overlay_clrs: List[int] = None,
):
    """
    A function to overlay a vector layer on to a raster image 'burning' in the vector
    as a particular colour. This is most appropriate for polyline vectors, polygons
    will be filled.

    :param input_img: The input image, usually a stretched byte (8uint) image ready
                      for visualisation but needs spatial header information associated.
    :param output_img: A output image file commonly in none specialist format such
                       as JPEG, PNG or TIFF.
    :param vec_overlay_file: The vector file to overlay (recommended to be a
                             polyline vector)
    :param vec_overlay_lyr: The name of the vector layer
    :param tmp_dir: A temporary were processing stage file can be written
                    during processing.
    :param gdalformat: The output format - commonly PNG, JPEG or GTIFF.
    :param overlay_clrs: An array with the output pixel values for the vector overlay,
                         needs the same length as the number of image bands. If None
                         then the overlay will be white (i.e., [255, 255, 255].

    """
    import rsgislib.vectorutils.createrasters

    # Check the overlay colour is defined and correct.
    n_img_bands = rsgislib.imageutils.get_img_band_count(input_img)
    if overlay_clrs is None:
        overlay_clrs = list()
        for i in range(n_img_bands):
            overlay_clrs.append(255)
    elif len(overlay_clrs) != n_img_bands:
        raise rsgislib.RSGISPyException(
            "The overlay colour does not have the same number of "
            "values as the number of bands within the input image."
        )

    # Create a temporary directory for processing stage outputs.
    img_basename = rsgislib.tools.filetools.get_file_basename(input_img)
    uid_str = rsgislib.tools.utils.uid_generator()
    usr_tmp_dir = os.path.join(
        tmp_dir, "{}_qklk_overlay_tmp_{}".format(img_basename, uid_str)
    )
    if not os.path.exists(usr_tmp_dir):
        os.makedirs(usr_tmp_dir)

    # Create raster of the vector to be overlain.
    tmp_vec_overlay_img = os.path.join(
        usr_tmp_dir, "{}_vec_overlay.kea".format(img_basename)
    )
    rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
        vec_overlay_file,
        vec_overlay_lyr,
        input_img,
        tmp_vec_overlay_img,
        gdalformat="KEA",
        burn_val=1,
        datatype=rsgislib.TYPE_8UINT,
        att_column=None,
        thematic=True,
        no_data_val=0,
    )

    # Merge the overlay and base image
    tmp_final_img = os.path.join(usr_tmp_dir, "{}_final.kea".format(img_basename))
    burn_in_binary_msk(
        input_img, tmp_vec_overlay_img, tmp_final_img, "KEA", overlay_clrs
    )

    # Convert to final format (e.g., JPG, TIFF or PNG)
    rsgislib.imageutils.gdal_translate(tmp_final_img, output_img, gdalformat=gdalformat)

    # Remove the temporary directory.
    shutil.rmtree(usr_tmp_dir)
