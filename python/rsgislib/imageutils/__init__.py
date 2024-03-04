#!/usr/bin/env python
"""
The imageutils module contains general utilities for applying to images.
"""

import math
import os
import shutil
from typing import Dict, List

import numpy
from osgeo import gdal, osr
from rios import applier

import rsgislib

# import the C++ extension into this level
from ._imageutils import *

gdal.UseExceptions()


class OutImageInfo(object):
    """
    A class which is used to define the information to create a new output image.
    This class is used within the StdImgBlockIter class.

    :param file_name: is the output image file name and path.
    :param name: is a name associated with this layer - doesn't really matter what
                 you use but needs to be unique; this is used as a dict key in
                 some functions.
    :param nbands: is an int with the number of output image bands.
    :param no_data_val: is a no data value for the output image
    :param gdalformat: is the output image file format
    :param datatype: is the output datatype rsgislib.TYPE_*
    """

    def __init__(
        self,
        file_name=None,
        name=None,
        nbands=None,
        no_data_val=None,
        gdalformat=None,
        datatype=None,
    ):
        """
        :param file_name: is the input image file name and path.
        :param name: is a name associated with this layer - doesn't really
                     matter what you use but needs to be unique; this is used
                     as a dict key in some functions.
        :param nbands: is an int with the number of output image bands.
        :param no_data_val: is a no data value for the output image
        :param gdalformat: is the output image file format
        :param datatype: is the output datatype rsgislib.TYPE_*
        """
        self.file_name = file_name
        self.name = name
        self.nbands = nbands
        self.no_data_val = no_data_val
        self.gdalformat = gdalformat
        self.datatype = datatype


class SharpBandInfo(object):
    """
    Create a list of these objects to pass to the sharpenLowResBands function.

    :param band: is the band number (band numbering starts at 1).
    :param status: needs to be either rsgislib.SHARP_RES_IGNORE, rsgislib.SHARP_RES_LOW
                   or rsgislib.SHARP_RES_HIGH lowres bands will be sharpened using
                   the highres bands and ignored bands will just be copied into
                   the output image.
    :param name: is a name associated with this image band - doesn't really
                 matter what you put in here.
    """

    def __init__(self, band=None, status=None, name=None):
        """
        :param band: is the band number (band numbering starts at 1).
        :param status: needs to be either 'ignore', 'lowres' or 'highres' - lowres
                       bands will be sharpened using the highres bands and ignored
                       bands will just be copied into the output image.
        :param name: is a name associated with this image band - doesn't really
                     matter what you put in here.

        """
        self.band = band
        self.status = status
        self.name = name


class ImageBandInfo(object):
    """
    Create a list of these objects to pass to functions to specifying individual
    images and image bands. Functions where this class is used include:

    * rsgislib.imageutils.create_valid_mask
    * rsgislib.zonalstats.extractZoneImageBandValues2HDF

    :param file_name: is the input image file name and path.
    :param name: is a name associated with this layer - doesn't really matter what
                 you use but needs to be unique; this is used as a dict key in some
                 functions.
    :param bands: is a list of image bands within the file_name to be used for
                  processing (band numbers start at 1).

    """

    def __init__(self, file_name=None, name=None, bands=None):
        """
        :param file_name: is the input image file name and path.
        :param name: is a name associated with this layer - doesn't really matter
                     what you use but needs to be unique; this is used as a dict
                     key in some functions.
        :param bands: is a list of image bands within the file_name to be used for
                      processing (band numbers start at 1).
        """
        self.file_name = file_name
        self.name = name
        self.bands = bands

    def __repr__(self):
        return repr((self.name, self.bands, self.file_name))

    def __str__(self):
        str_val = "Image {} [Bands: {}] - {}".format(
            self.name,
            self.bands,
            self.file_name,
        )
        return str_val


# Define Class for time series fill
class RSGISTimeseriesFillInfo(object):
    """
    Create a list of these objects to pass to the fillTimeSeriesGaps function

    :param year: year the composite represents.
    :param day: the (nominal) day within the year the composite represents (a value
                of zero and day will not be used)
    :param compImg: The input compsite image which has been generated.
    :param imgRef:  The reference layer (e.g., from create_max_ndvi_composite or
                    create_max_ndvi_ndwi_composite_landsat) with zero for no
                    data regions
    :param outRef: A boolean variable specify which layer a fill reference layer
                   is to be produced.

    """

    def __init__(self, year=1900, day=0, compImg=None, imgRef=None, outRef=False):
        """
        :param year: year the composite represents.
        :param day: the (nominal) day within the year the composite represents (a
                    value of zero and day will not be used)
        :param compImg: The input compsite image which has been generated.
        :param imgRef:  The reference layer (e.g., from create_max_ndvi_composite
                        or create_max_ndvi_ndwi_composite_landsat) with zero for
                        no data regions
        :param outRef: A boolean variable specify which layer a fill reference
                       layer is to be produced.

        """
        self.year = year
        self.day = day
        self.compImg = compImg
        self.imgRef = imgRef
        self.outRef = outRef

    def __repr__(self):
        return repr((self.year, self.day, self.compImg, self.imgRef, self.outRef))


def set_env_vars_lzw_gtiff_outs(bigtiff: bool = False):
    """
    Set environmental variables such that outputted
    GeoTIFF files are outputted as tiled and compressed
    using the LZW algorithm.

    :param bigtiff: If True GTIFF files will be outputted
                    in big tiff format.

    Example:

    .. code:: python

        import rsgislib.imageutils
        rsgislib.imageutils.set_env_vars_lzw_gtiff_outs()

    """
    if bigtiff:
        os.environ["RSGISLIB_IMG_CRT_OPTS_GTIFF"] = "TILED=YES:COMPRESS=LZW:BIGTIFF=YES"
    else:
        os.environ["RSGISLIB_IMG_CRT_OPTS_GTIFF"] = "TILED=YES:COMPRESS=LZW"


def set_env_vars_deflate_gtiff_outs(bigtiff: bool = False):
    """
    Set environmental variables such that outputted
    GeoTIFF files are outputted as tiled and compressed
    using the Deflate (zlib) algorithm.

    :param bigtiff: If True GTIFF files will be outputted
                    in big tiff format.

    Example:

    .. code:: python

        import rsgislib.imageutils
        rsgislib.imageutils.set_env_vars_deflate_gtiff_outs()

    """
    if bigtiff:
        os.environ[
            "RSGISLIB_IMG_CRT_OPTS_GTIFF"
        ] = "TILED=YES:COMPRESS=DEFLATE:BIGTIFF=YES"
    else:
        os.environ["RSGISLIB_IMG_CRT_OPTS_GTIFF"] = "TILED=YES:COMPRESS=DEFLATE"


def pop_thmt_img_stats(
    input_img: str,
    add_clr_tab: bool = True,
    calc_pyramids: bool = True,
    ignore_zero: bool = True,
):
    """
    A function which populates a byte thematic input image (e.g., classification) with
    pyramids and header statistics (e.g., colour table).

    Note, for more complex thematic images using formats which support raster attribute
    tables (e.g., KEA) then use the rsgislib.rastergis.pop_rat_img_stats function.

    This function is best used aiding the visualisation of GTIFF's.

    :param input_img: input image path
    :param add_clr_tab: boolean specifying whether a colour table should be added
                        (default: True)
    :param calc_pyramids: boolean specifying whether a image pyramids should be added
                        (default: True)
    :param ignore_zero: boolean specifying whether to ignore pixel with a value of zero
                        i.e., as a no data value (default: True)

    Example:

    .. code:: python

        import rsgislib.imageutils
        rsgislib.imageutils.pop_thmt_img_stats("land_cover_cls_img.tif")

    """
    import tqdm

    # Get file data type
    data_type = get_gdal_datatype_from_img(input_img)
    if data_type != gdal.GDT_Byte:
        raise rsgislib.RSGISPyException("This function only supports byte datasets")

    # Get file format
    img_format = get_gdal_format_name(input_img)
    if img_format == "KEA":
        print("Recommend using rsgislib.rastergis.pop_rat_img_stats for KEA files")
    # Set image as being thematic
    set_img_thematic(input_img)

    if add_clr_tab:
        n_bands = get_img_band_count(input_img)
        gdal_ds = gdal.Open(input_img, gdal.GA_Update)
        for band_idx in tqdm.tqdm(range(n_bands)):
            gdal_band = gdal_ds.GetRasterBand(band_idx + 1)

            # fill in the metadata
            tmp_meta = gdal_band.GetMetadata()

            if ignore_zero:
                gdal_band.SetNoDataValue(0)
                tmp_meta["STATISTICS_EXCLUDEDVALUES"] = "0"

            try:
                (min_val, max_val, mean_val, stddev_val) = gdal_band.ComputeStatistics(
                    False
                )
            except RuntimeError as e:
                if str(e).endswith(
                    "Failed to compute statistics, no "
                    "valid pixels found in sampling."
                ):
                    min_val = 0
                    max_val = 0
                    mean_val = 0
                    stddev_val = 0
                else:
                    raise e

            tmp_meta["STATISTICS_MINIMUM"] = f"{min_val}"
            tmp_meta["STATISTICS_MAXIMUM"] = f"{max_val}"
            tmp_meta["STATISTICS_MEAN"] = f"{mean_val}"
            tmp_meta["STATISTICS_STDDEV"] = f"{stddev_val}"
            tmp_meta["STATISTICS_SKIPFACTORX"] = "1"
            tmp_meta["STATISTICS_SKIPFACTORY"] = "1"

            # byte data use 256 bins and the whole range
            hist_min = 0
            hist_max = 255
            hist_step = 1.0
            hist_calc_min = -0.5
            hist_calc_max = 255.5
            hist_n_bins = 256
            tmp_meta["STATISTICS_HISTOBINFUNCTION"] = "direct"

            hist = gdal_band.GetHistogram(
                hist_calc_min, hist_calc_max, hist_n_bins, False, False
            )

            # Check if GDAL's histogram code overflowed. This is not a fool-proof test,
            # as some overflows will not result in negative counts.
            histogram_overflow = min(hist) < 0

            if not histogram_overflow:
                # comes back as a list for some reason
                hist = numpy.array(hist)

                # Note that we have explicitly set histstep in each datatype case
                # above. In principle, this can be calculated, as it is done in the
                # float case, but for some of the others we need it to be exactly
                # equal to 1, so we set it explicitly there, to avoid rounding
                # error problems.

                # do the mode - bin with the highest count
                mode_bin = numpy.argmax(hist)
                mode_val = mode_bin * hist_step + hist_min
                tmp_meta["STATISTICS_MODE"] = f"{int(round(mode_val))}"
                tmp_meta["STATISTICS_HISTOBINVALUES"] = "|".join(map(repr, hist)) + "|"
                tmp_meta["STATISTICS_HISTOMIN"] = f"{hist_min}"
                tmp_meta["STATISTICS_HISTOMAX"] = f"{hist_max}"
                tmp_meta["STATISTICS_HISTONUMBINS"] = f"{hist_n_bins}"

                # estimate the median
                mid_num = hist.sum() / 2
                gt_mid = hist.cumsum() >= mid_num
                median_bin = gt_mid.nonzero()[0][0]
                median_val = median_bin * hist_step + hist_min
                tmp_meta["STATISTICS_MEDIAN"] = "{}".format(int(round(median_val)))

            gdal_band.SetMetadata(tmp_meta)

            clr_tab = gdal.ColorTable()
            for i in range(hist_n_bins):
                if (i == 0) and ignore_zero:
                    clr_tab.SetColorEntry(0, (0, 0, 0, 0))
                else:
                    ran_clr = numpy.random.randint(1, 255, 3, dtype=numpy.uint8)
                    clr_tab.SetColorEntry(i, (ran_clr[0], ran_clr[1], ran_clr[2], 255))
            gdal_band.SetRasterColorTable(clr_tab)
            gdal_band.SetRasterColorInterpretation(gdal.GCI_PaletteIndex)

        gdal_ds = None

    if calc_pyramids:
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()

        img_x_size, img_y_size = get_img_size(input_img)
        if img_x_size < img_y_size:
            min_size = img_x_size
        else:
            min_size = img_y_size

        n_overs = 0
        pyd_lvls = [4, 8, 16, 32, 64, 128, 256, 512]
        for i in pyd_lvls:
            if (min_size // i) > 33:
                n_overs = n_overs + 1

        gdal_ds = gdal.Open(input_img, gdal.GA_Update)
        if img_format == "GTIFF":
            gdal.SetConfigOption("COMPRESS_OVERVIEW", "LZW")
        gdal_ds.BuildOverviews("NEAREST", pyd_lvls[:n_overs], callback)
        gdal_ds = None


def get_colour_tab_info(input_img: str, img_band: int = 1) -> Dict[str, Dict]:
    """
    A function which returns the colour table info from an input image.
    The output dict has the following structure:

    clr_info[1] = {'red':0, 'green':255, 'blue':0, 'alpha': 255}
    clr_info[2] = {'red':0, 'green':0, 'blue':255, 'alpha': 255}

    :param input_img: the file path to the input image.
    :param img_band: optional image band to be read - default is 1.
    :return: dict of dicts with the colour table info.

    """
    gdal_ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if gdal_ds is None:
        raise Exception(f"Cannot open input image: {input_img}")

    gdal_band = gdal_ds.GetRasterBand(img_band)
    if gdal_band is None:
        raise Exception(f"Cannot open input image band {gdal_band} in {input_img}")

    gdal_clr_tab = gdal_band.GetColorTable()
    if gdal_clr_tab is None:
        raise Exception(f"Input does not have a colour table: {input_img}")

    cls_info = dict()
    for i in range(gdal_clr_tab.GetCount()):
        clr_entry = gdal_clr_tab.GetColorEntry(i)

        cls_info[i] = dict()
        cls_info[i]["red"] = int(clr_entry[0])
        cls_info[i]["green"] = int(clr_entry[1])
        cls_info[i]["blue"] = int(clr_entry[2])
        if len(clr_entry) > 3:
            cls_info[i]["alpha"] = int(clr_entry[3])
        else:
            cls_info[i]["alpha"] = 255

    return cls_info


def define_colour_table(input_img: str, clr_lut: dict, img_band: int = 1):
    """
    A function which defines specific colours for image values for a colour
    table. Note, this function must be used to thematic images which use
    int pixel values and replaces the existing colour table.

    :param input_img: input image path
    :param clr_lut: a dict with the pixel value as the key with a single value or
                    list of 3 or 4 values. If 3 values are provided they are
                    interpreted as RGB (values between 0 and 255) while if 4 are
                    provided they are interpreted as RGBA (values between 0 and 255).
                    If a single value is provided then it is interpreted as a
                    hexadecimal value for RGB (e.g., #b432be).
    :param img_band: int specifying the band for the colour table (default = 1)

    Example:

    .. code:: python

        import rsgislib.imageutils

        clr_lut = dict()
        clr_lut[1] = '#009600'
        clr_lut[2] = '#FFE5CC'
        clr_lut[3] = '#CCFFE5'
        rsgislib.imageutils.define_colour_table("cls_img.tif", clr_lut)

    """
    import rsgislib.tools.utils

    gdal_ds = gdal.Open(input_img, gdal.GA_Update)
    if gdal_ds is None:
        raise Exception(f"Cannot open input image: {input_img}")

    gdal_band = gdal_ds.GetRasterBand(img_band)
    if gdal_band is None:
        raise Exception(f"Cannot open input image band {gdal_band} in {input_img}")

    clr_tbl = gdal.ColorTable()
    for pxl_val in clr_lut:
        if isinstance(clr_lut[pxl_val], str):
            r, g, b = rsgislib.tools.utils.hex_to_rgb(clr_lut[pxl_val])
            clr_tbl.SetColorEntry(pxl_val, (r, g, b, 255))
        elif isinstance(clr_lut[pxl_val], list) and len(clr_lut[pxl_val]) == 3:
            clr_tbl.SetColorEntry(
                pxl_val,
                (clr_lut[pxl_val][0], clr_lut[pxl_val][1], clr_lut[pxl_val][2], 255),
            )
        elif isinstance(clr_lut[pxl_val], list) and len(clr_lut[pxl_val]) == 4:
            clr_tbl.SetColorEntry(
                pxl_val,
                (
                    clr_lut[pxl_val][0],
                    clr_lut[pxl_val][1],
                    clr_lut[pxl_val][2],
                    clr_lut[pxl_val][3],
                ),
            )
        else:
            raise rsgislib.RSGISPyException(
                "There should be single string or a list "
                "with 3 or 4 values for the colour table."
            )
    gdal_band.SetRasterColorTable(clr_tbl)
    gdal_band.SetRasterColorInterpretation(gdal.GCI_PaletteIndex)
    gdal_ds = None


def get_rsgislib_datatype_from_img(input_img: str):
    """
    Returns the rsgislib datatype ENUM (e.g., rsgislib.TYPE_8UINT)
    for the inputted raster file

    :param input_img: input image file.
    :return: int

    Example:

    .. code:: python

        import rsgislib.imageutils
        datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img("img.kea")

    """
    raster = gdal.Open(input_img, gdal.GA_ReadOnly)
    if raster is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: '" + input_img + "'"
        )
    band = raster.GetRasterBand(1)
    if band is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster band 1 in image: '" + input_img + "'"
        )
    gdal_dtype = gdal.GetDataTypeName(band.DataType)
    raster = None
    return rsgislib.get_rsgislib_datatype(gdal_dtype)


def get_gdal_datatype_from_img(input_img: str) -> int:
    """
    Returns the GDAL datatype ENUM (e.g., GDT_Float32) for the inputted raster file.

    :param input_img: input image file.
    :return: int

    Example:

    .. code:: python

        import rsgislib.imageutils
        datatype = rsgislib.imageutils.get_gdal_datatype_from_img("img.kea")

    """
    raster = gdal.Open(input_img, gdal.GA_ReadOnly)
    if raster is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: '" + input_img + "'"
        )
    band = raster.GetRasterBand(1)
    if band is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster band 1 in image: '" + input_img + "'"
        )
    gdal_dtype = band.DataType
    raster = None
    return gdal_dtype


def get_gdal_datatype_name_from_img(input_img: str) -> str:
    """
    Returns the GDAL datatype ENUM (e.g., GDT_Float32) for the inputted raster file.

    :param input_img: input image file.
    :return: str

    Example:

    .. code:: python

        import rsgislib.imageutils
        datatype = rsgislib.imageutils.get_gdal_datatype_name_from_img("img.kea")

    """
    raster = gdal.Open(input_img, gdal.GA_ReadOnly)
    if raster is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: '" + input_img + "'"
        )
    band = raster.GetRasterBand(1)
    if band is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster band 1 in image: '" + input_img + "'"
        )
    dtypeName = gdal.GetDataTypeName(band.DataType)
    raster = None
    return dtypeName


def get_file_img_extension(gdalformat: str):
    """
    A function to get the extension for a given file format
    (NOTE, currently only KEA, GTIFF, HFA, PCI and ENVI are supported).

    :param gdalformat: GDAL string for the format.
    :return: string

    Example:

    .. code:: python

        import rsgislib.imageutils
        img_ext = rsgislib.imageutils.get_file_img_extension("KEA")

    """
    ext = "NA"
    if gdalformat.lower() == "kea":
        ext = "kea"
    elif gdalformat.lower() == "gtiff":
        ext = "tif"
    elif gdalformat.lower() == "hfa":
        ext = "img"
    elif gdalformat.lower() == "envi":
        ext = "env"
    elif gdalformat.lower() == "pcidsk":
        ext = "pix"
    else:
        raise rsgislib.RSGISPyException(
            "The extension for the gdalformat specified is unknown."
        )
    return ext


def get_gdal_format_from_ext(input_img: str):
    """
    Get GDAL format, based on input_file

    :param input_img: input image file.
    :return: string

    Example:

    .. code:: python

        import rsgislib.imageutils
        img_ext = rsgislib.imageutils.get_gdal_format_from_ext("img.kea")

    """
    gdalStr = ""
    extension = os.path.splitext(input_img)[-1]
    if extension == ".env":
        gdalStr = "ENVI"
    elif extension == ".kea":
        gdalStr = "KEA"
    elif extension == ".tif" or extension == ".tiff":
        gdalStr = "GTiff"
    elif extension == ".img":
        gdalStr = "HFA"
    elif extension == ".pix":
        gdalStr = "PCIDSK"
    else:
        raise rsgislib.RSGISPyException("Type not recognised")
    return gdalStr


def get_gdal_format_name(input_img: str):
    """
    Gets the shorthand file format for the input image in uppercase.

    :param input_img: The current name of the GDAL layer.
    :return: string with the file format (e.g., KEA or GTIFF).

    Example:

    .. code:: python

        import rsgislib.imageutils
        gdal_format = rsgislib.imageutils.get_gdal_format_name("img.kea")

    """
    layerDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    gdalDriver = layerDS.GetDriver()
    layerDS = None
    return str(gdalDriver.ShortName).upper()


def rename_gdal_layer(input_img: str, output_img: str):
    """
    Rename all the files associated with a GDAL layer.

    :param input_img: The current name of the GDAL layer.
    :param output_img: The output name of the GDAL layer.

    Example:

    .. code:: python

        import rsgislib.imageutils
        rsgislib.imageutils.rename_gdal_layer("img.kea", "output_img.kea")

    """
    layerDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    gdalDriver = layerDS.GetDriver()
    layerDS = None
    gdalDriver.Rename(output_img, input_img)


def delete_gdal_layer(input_img: str):
    """
    Deletes all the files associated with a GDAL layer.

    :param input_img: The file name and path of the GDAL layer to be deleted.

    Example:

    .. code:: python

        import rsgislib.imageutils
        rsgislib.imageutils.delete_gdal_layer("img.kea")

    """
    layerDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    gdalDriver = layerDS.GetDriver()
    layerDS = None
    gdalDriver.Delete(input_img)


def get_img_res(input_img: str, abs_vals: bool = False):
    """
    A function to retrieve the image resolution.

    :param input_img: input image file
    :param abs_vals: if True then returned x/y values will be positive (default: False)

    :return: xRes, yRes

    Example:

    .. code:: python

        import rsgislib.imageutils
        x_res, y_res = rsgislib.imageutils.get_img_res("img.kea")

    """
    rasterDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    if rasterDS is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: {}".format(input_img)
        )

    geotransform = rasterDS.GetGeoTransform()
    xRes = geotransform[1]
    yRes = geotransform[5]
    if abs_vals:
        yRes = abs(yRes)
        xRes = abs(xRes)
    rasterDS = None
    return xRes, yRes


def do_img_res_match(in_a_img: str, in_b_img: str):
    """
    A function to test whether two images have the same
    image pixel resolution.

    :param in_a_img: input image file.
    :param in_b_img: input image file.
    :return: boolean

    Example:

    .. code:: python

        import rsgislib.imageutils
        if rsgislib.imageutils.do_img_res_match("img1.kea", "img2.kea"):
            print("Images match in terms of resolutions")

    """
    img1XRes, img1YRes = get_img_res(in_a_img)
    img2XRes, img2YRes = get_img_res(in_b_img)

    return (img1XRes == img2XRes) and (img1YRes == img2YRes)


def get_img_size(input_img: str):
    """
    A function to retrieve the image size in pixels.

    :param input_img: input image file.
    :return: xSize, ySize

    Example:

    .. code:: python

        import rsgislib.imageutils
        x_size, y_size = rsgislib.imageutils.get_img_size("img.kea")

    """
    rasterDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    if rasterDS is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: '" + input_img + "'"
        )

    xSize = rasterDS.RasterXSize
    ySize = rasterDS.RasterYSize
    rasterDS = None
    return xSize, ySize


def get_img_pxl_coords(
    input_img: str, x_coords: numpy.array, y_coords: numpy.array
) -> (numpy.array, numpy.array):
    """
    A function which calculates the image pixel coordinates for a set of
    spatial coordinates for the input_img. Note, the input coordinates
    must be within the input image extent.

    :param input_img: Input image file path. This image defines the spatial coordinate
                      system and extent for the conversion of the input coordinates.
    :param x_coords: Numpy array of x coordinates
    :param y_coords: Numpy array of y coordinates
    :return: x_pxl_coords, y_pxl_coords. A pair of numpy arrays with the image
             pixel coordinates for the input spatial coordinates.

    """
    img_ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if img_ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: {}".format(input_img)
        )

    x_size = img_ds.RasterXSize
    y_size = img_ds.RasterYSize

    geo_transform = img_ds.GetGeoTransform()
    tl_x = geo_transform[0]
    tl_y = geo_transform[3]

    x_res = abs(geo_transform[1])
    y_res = abs(geo_transform[5])

    br_x = tl_x + (x_res * x_size)
    br_y = tl_y - (y_res * y_size)

    if numpy.any((x_coords < tl_x) | (x_coords > br_x)):
        raise rsgislib.RSGISPyException(
            "Coordinates outside the image extent were passed (x-axis)"
        )
    if numpy.any((y_coords < br_y) | (y_coords > tl_y)):
        raise rsgislib.RSGISPyException(
            "Coordinates outside the image extent were passed (y-axis)"
        )

    x_pxl_coords = numpy.floor(((x_coords - tl_x) / x_res) + 0.5).astype("int")
    y_pxl_coords = numpy.floor(((tl_y - y_coords) / y_res) + 0.5).astype("int")

    return x_pxl_coords, y_pxl_coords


def get_img_bbox(input_img: str):
    """
    A function to retrieve the bounding box in the spatial
    coordinates of the image.

    :param input_img: input image file.
    :return: (MinX, MaxX, MinY, MaxY)

    Example:

    .. code:: python

        import rsgislib.imageutils
        bbox = rsgislib.imageutils.get_img_bbox("img.kea")

    """
    rasterDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    if rasterDS is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: '" + input_img + "'"
        )

    xSize = rasterDS.RasterXSize
    ySize = rasterDS.RasterYSize

    geotransform = rasterDS.GetGeoTransform()
    tlX = geotransform[0]
    tlY = geotransform[3]
    xRes = geotransform[1]
    yRes = geotransform[5]
    if yRes < 0:
        yRes = yRes * -1
    rasterDS = None

    brX = tlX + (xRes * xSize)
    brY = tlY - (yRes * ySize)

    return [tlX, brX, brY, tlY]


def get_img_bbox_in_proj(input_img: str, out_epsg: int):
    """
    A function to retrieve the bounding box in the spatial
    coordinates of the image.

    :param input_img: input image file.
    :param out_epsg: an EPSG code for the output BBOX
    :return: (MinX, MaxX, MinY, MaxY)

    Example:

    .. code:: python

        import rsgislib.imageutils
        bbox_osgb = rsgislib.imageutils.get_img_bbox("img.kea", out_epsg=27700)

    """
    import rsgislib.tools.geometrytools

    in_proj_wkt = get_wkt_proj_from_img(input_img)
    in_spat_ref = osr.SpatialReference()
    in_spat_ref.ImportFromWkt(in_proj_wkt)

    out_spat_ref = osr.SpatialReference()
    out_spat_ref.ImportFromEPSG(int(out_epsg))

    img_bbox = get_img_bbox(input_img)
    reproj_img_bbox = rsgislib.tools.geometrytools.reproj_bbox(
        img_bbox, in_spat_ref, out_spat_ref
    )
    return reproj_img_bbox


def get_img_subset_pxl_bbox(input_img: str, sub_bbox: List[float]) -> List[int]:
    """
    A function which returns a BBOX (xmin, xmax, ymin, ymax) with the pixel coordinates
    for an intersecting BBOX in the spatial coordinates of the image. Note, the
    sub_bbox will be subset to be contained by the input image (if outside the
    bounds of the input image).

    :param input_img: The input image file path
    :param sub_bbox: The input bbox (xmin, xmax, ymin, ymax) in the same spatial
                     coordinate system as the input image specifying the subset
                     for which the pixel coords are to be calculated.
    :return: bbox (xmin, xmax, ymin, ymax) in the image pixel coordinates.

    """
    import math

    import rsgislib.tools.geometrytools

    img_bbox = get_img_bbox(input_img)
    x_res, y_res = get_img_res(input_img, abs_vals=True)

    if not rsgislib.tools.geometrytools.do_bboxes_intersect(img_bbox, sub_bbox):
        raise rsgislib.RSGISPyException(
            "The subset BBOX does not intersect with the image bbox - "
            "is the projection of the sub_bbox correct?"
        )

    sub_inter_bbox = rsgislib.tools.geometrytools.bbox_intersection(img_bbox, sub_bbox)

    min_x_pxl = math.floor(((sub_inter_bbox[0] - img_bbox[0]) / x_res) + 0.5)
    max_x_pxl = math.floor(((sub_inter_bbox[1] - img_bbox[0]) / x_res) + 0.5)

    # Note the Y axis is from the TL so reversed the Y axis min/max.
    min_y_pxl = math.floor(((img_bbox[3] - sub_inter_bbox[3]) / y_res) + 0.5)
    max_y_pxl = math.floor(((img_bbox[3] - sub_inter_bbox[2]) / y_res) + 0.5)

    return [min_x_pxl, max_x_pxl, min_y_pxl, max_y_pxl]


def get_img_pxl_spatial_coords(input_img: str, sub_pxl_bbox: List[int]) -> List[float]:
    """
    A function which gets the spatial coordinates for a image pixel space
    BBOX (xmin, xmax, ymin, ymax). The returned BBOX will be within the
    same coordinate system as the input image.

    :param input_img: The input image file path
    :param sub_pxl_bbox: The bbox (xmin, xmax, ymin, ymax) in the image pixel
                         coordinates (e.g., [20, 120, 1000, 1230])
    :return: bbox (xmin, xmax, ymin, ymax) in the image spatial coordinates.

    """
    img_bbox = get_img_bbox(input_img)
    x_res, y_res = get_img_res(input_img, abs_vals=True)

    min_x = (sub_pxl_bbox[0] * x_res) + img_bbox[0]
    max_x = (sub_pxl_bbox[1] * x_res) + img_bbox[0]

    # Note the Y axis is from the TL so reversed the Y axis min/max.
    min_y = img_bbox[3] - (sub_pxl_bbox[3] * y_res)
    max_y = img_bbox[3] - (sub_pxl_bbox[2] * y_res)

    return [min_x, max_x, min_y, max_y]


def get_img_band_stats(input_img: str, img_band: int, compute: bool = True):
    """
    A function which calls the GDAL function on the band selected to calculate
    the pixel stats (min, max, mean, standard deviation).

    :param input_img: input image file path
    :param img_band: specified image band for which stats are to be
                     calculated (starts at 1).
    :param compute: whether the stats should be calculated (True; Default) or an
                    approximation or pre-calculated stats are OK (False).

    :return: stats (min, max, mean, stddev)

    Example:

    .. code:: python

        import rsgislib.imageutils
        min, max, mean, std = rsgislib.imageutils.get_img_band_stats("img.kea", 1)

    """
    img_ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if img_ds is None:
        raise rsgislib.RSGISPyException("Could not open image: '{}'".format(input_img))
    n_bands = img_ds.RasterCount

    if img_band > 0 and img_band <= n_bands:
        img_band_obj = img_ds.GetRasterBand(img_band)
        if img_band_obj is None:
            raise rsgislib.RSGISPyException(
                "Could not open image band ('{0}') from : '{1}'".format(
                    img_band, input_img
                )
            )
        img_stats = img_band_obj.ComputeStatistics((not compute))
    else:
        raise rsgislib.RSGISPyException(
            "Band specified is not within the image: '{}'".format(input_img)
        )
    return img_stats


def get_img_band_count(input_img: str):
    """
    A function to retrieve the number of image bands in an image file.

    :param input_img: input image file.
    :return: nBands

    Example:

    .. code:: python

        import rsgislib.imageutils
        n_bands = rsgislib.imageutils.get_img_band_count("img.kea")

    """
    rasterDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    if rasterDS is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: '" + input_img + "'"
        )

    nBands = rasterDS.RasterCount
    rasterDS = None
    return nBands


def get_img_no_data_value(input_img: str, img_band: int = 1):
    """
    A function to retrieve the no data value for the image
    (from band; default 1).

    :param input_img: input image file.
    :param img_band: the band for which the no data value should be returned.
    :return: number

    Example:

    .. code:: python

        import rsgislib.imageutils
        no_data_val = rsgislib.imageutils.get_img_no_data_value("img.kea")

    """
    rasterDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    if rasterDS is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: '" + input_img + "'"
        )

    noDataVal = rasterDS.GetRasterBand(img_band).GetNoDataValue()
    rasterDS = None
    return noDataVal


def set_img_no_data_value(input_img: str, no_data_val: float, img_band: int = None):
    """
    A function to set the no data value for an image.
    If band is not specified sets value for all bands.

    :param input_img: input image file.
    :param no_data_val: No data value to be defined to the image band
    :param img_band: the band for which the no data value should be returned.

    Example:

    .. code:: python

        import rsgislib.imageutils
        rsgislib.imageutils.set_img_no_data_value("img.kea", 0.0, 1)

    """
    rasterDS = gdal.Open(input_img, gdal.GA_Update)
    if rasterDS is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: '" + input_img + "'"
        )

    if img_band is not None:
        rasterDS.GetRasterBand(img_band).SetNoDataValue(no_data_val)
    else:
        for b in range(rasterDS.RasterCount):
            rasterDS.GetRasterBand(b + 1).SetNoDataValue(no_data_val)

    rasterDS = None


def get_img_band_colour_interp(input_img: str, img_band: int):
    """
    A function to get the colour interpretation for a specific band.

    * GCI_Undefined=0,
    * GCI_GrayIndex=1,
    * GCI_PaletteIndex=2,
    * GCI_RedBand=3,
    * GCI_GreenBand=4,
    * GCI_BlueBand=5,
    * GCI_AlphaBand=6,
    * GCI_HueBand=7,
    * GCI_SaturationBand=8,
    * GCI_LightnessBand=9,
    * GCI_CyanBand=10,
    * GCI_MagentaBand=11,
    * GCI_YellowBand=12,
    * GCI_BlackBand=13,
    * GCI_YCbCr_YBand=14,
    * GCI_YCbCr_CbBand=15,
    * GCI_YCbCr_CrBand=16,
    * GCI_Max=16

    :param input_img: input image file.
    :param img_band: the band for which the no data value should be returned.
    :return: is a GDALColorInterp value

    """
    rasterDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    if rasterDS is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: '" + input_img + "'"
        )
    clrItrpVal = rasterDS.GetRasterBand(img_band).GetRasterColorInterpretation()
    rasterDS = None
    return clrItrpVal


def set_img_band_colour_interp(input_img: str, img_band: int, clr_itrp_val: int):
    """
    A function to set the colour interpretation for a specific band.
    input is a GDALColorInterp value:

    * GCI_Undefined=0,
    * GCI_GrayIndex=1,
    * GCI_PaletteIndex=2,
    * GCI_RedBand=3,
    * GCI_GreenBand=4,
    * GCI_BlueBand=5,
    * GCI_AlphaBand=6,
    * GCI_HueBand=7,
    * GCI_SaturationBand=8,
    * GCI_LightnessBand=9,
    * GCI_CyanBand=10,
    * GCI_MagentaBand=11,
    * GCI_YellowBand=12,
    * GCI_BlackBand=13,
    * GCI_YCbCr_YBand=14,
    * GCI_YCbCr_CbBand=15,
    * GCI_YCbCr_CrBand=16,
    * GCI_Max=16

    :param input_img: input image file.
    :param img_band: the band for which the no data value should be returned.
    :param clr_itrp_val: the clr_itrp_val value to be set.

    """
    rasterDS = gdal.Open(input_img, gdal.GA_Update)
    if rasterDS is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: '" + input_img + "'"
        )
    rasterDS.GetRasterBand(img_band).SetColorInterpretation(clr_itrp_val)
    rasterDS = None


def get_wkt_proj_from_img(input_img: str):
    """
    A function which returns the WKT string representing the projection
    of the input image.

    :param input_img: input image file.
    :return: WTK string

    """
    rasterDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    if rasterDS is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: '" + input_img + "'"
        )
    projStr = rasterDS.GetProjection()
    rasterDS = None
    return projStr


def get_epsg_proj_from_img(input_img: str):
    """
    Using GDAL to return the EPSG code for the input layer.

    :param input_img: input image file.
    :return: EPSG code

    """
    epsgCode = None
    try:
        layerDS = gdal.Open(input_img, gdal.GA_ReadOnly)
        if layerDS is None:
            raise rsgislib.RSGISPyException(
                "Could not open raster image: '{}'".format(input_img)
            )
        projStr = layerDS.GetProjection()
        layerDS = None

        spatRef = osr.SpatialReference()
        spatRef.ImportFromWkt(projStr)
        spatRef.AutoIdentifyEPSG()
        epsgCode = spatRef.GetAuthorityCode(None)
        if epsgCode is not None:
            epsgCode = int(epsgCode)
    except Exception:
        epsgCode = None
    return epsgCode


def get_img_files(input_img: str):
    """
    A function which returns a list of the files associated (e.g., header etc.)
    with the input image file.

    :param input_img: input image file.
    :return: lists

    """
    imgDS = gdal.Open(input_img)
    fileList = imgDS.GetFileList()
    imgDS = None
    return fileList


def get_utm_zone(input_img: str):
    """
    A function which returns a string with the UTM (XXN | XXS) zone of the input image
    but only if it is projected within the UTM projection/coordinate system.

    :param input_img: input image file.
    :return: string

    """
    rasterDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    if rasterDS is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: '" + input_img + "'"
        )
    projStr = rasterDS.GetProjection()
    rasterDS = None

    spatRef = osr.SpatialReference()
    spatRef.ImportFromWkt(projStr)
    utmZone = None
    if spatRef.IsProjected():
        projName = spatRef.GetAttrValue("projcs")
        zone = spatRef.GetUTMZone()
        if zone != 0:
            if zone < 0:
                utmZone = str(zone * (-1))
                if len(utmZone) == 1:
                    utmZone = "0" + utmZone
                utmZone = utmZone + "S"
            else:
                utmZone = str(zone)
                if len(utmZone) == 1:
                    utmZone = "0" + utmZone
                utmZone = utmZone + "N"
    return utmZone


def do_gdal_layers_have_same_proj(in_a_img: str, in_b_img: str):
    """
    A function which tests whether two gdal compatible layers are in the same
    projection/coordinate system. This is done using the GDAL SpatialReference
    function AutoIdentifyEPSG. If the identified EPSG codes are different then
    False is returned otherwise True.

    :param in_a_img: input image file.
    :param in_b_img: input image file.
    :return: boolean

    """
    layer1EPSG = get_epsg_proj_from_img(in_a_img)
    layer2EPSG = get_epsg_proj_from_img(in_b_img)

    sameEPSG = False
    if layer1EPSG == layer2EPSG:
        sameEPSG = True

    return sameEPSG


def set_band_names(input_img: str, band_names: list, feedback: bool = False):
    """
    A utility function to set band names.

    :param input_img: input image file.
    :param band_names: is a list of band names
    :param feedback: is a boolean specifying whether feedback will be printed to the
                     console (True= Printed / False (default) Not Printed)

    .. code:: python

        from rsgislib import imageutils

        input_img = 'injune_p142_casi_sub_utm.kea'
        band_names = ['446nm','530nm','549nm','569nm','598nm','633nm','680nm','696nm',
                      '714nm','732nm','741nm','752nm','800nm','838nm']

        imageutils.set_band_names(input_img, band_names)

    """
    dataset = gdal.Open(input_img, gdal.GA_Update)

    for i in range(len(band_names)):
        band = i + 1
        bandName = band_names[i]

        imgBand = dataset.GetRasterBand(band)
        # Check the image band is available
        if not imgBand is None:
            if feedback:
                print('Setting Band {0} to "{1}"'.format(band, bandName))
            imgBand.SetDescription(bandName)
        else:
            raise rsgislib.RSGISPyException("Could not open the image band: ", band)


def get_band_names(input_img: str):
    """
    A utility function to get band names.

    :param input_img: input image file.
    :return: list of band names

    .. code:: python

        from rsgislib import imageutils

        input_img = 'injune_p142_casi_sub_utm.kea'
        bandNames = imageutils.get_band_names(input_img)

    """
    dataset = gdal.Open(input_img)
    bandNames = list()

    for i in range(dataset.RasterCount):
        imgBand = dataset.GetRasterBand(i + 1)
        # Check the image band is available
        if not imgBand is None:
            bandNames.append(imgBand.GetDescription())
        else:
            raise rsgislib.RSGISPyException(
                "Could not open the image band: {}".format(imgBand)
            )
    return bandNames


def set_img_thematic(input_img: str):
    """
    Set all image bands to be thematic.

    :param input_img: The file for which the bands are to be set as thematic

    """
    ds = gdal.Open(input_img, gdal.GA_Update)
    if ds is None:
        raise rsgislib.RSGISPyException("Could not open the input_img.")
    for bandnum in range(ds.RasterCount):
        band = ds.GetRasterBand(bandnum + 1)
        band.SetMetadataItem("LAYER_TYPE", "thematic")
    ds = None


def set_img_not_thematic(input_img: str):
    """
    Set all image bands to be not thematic (athematic).

    :param input_img: The file for which the bands are to be set
                      as not thematic (athematic)

    """
    ds = gdal.Open(input_img, gdal.GA_Update)
    if ds is None:
        raise rsgislib.RSGISPyException("Could not open the input_img.")
    for bandnum in range(ds.RasterCount):
        band = ds.GetRasterBand(bandnum + 1)
        band.SetMetadataItem("LAYER_TYPE", "athematic")
    ds = None


def is_img_thematic(input_img: str, img_band: int = 1):
    """
    Set all image bands to be thematic.

    :param input_img: The file for which the bands are to be set as thematic
    :param img_band: The image band to be tested.

    """
    ds = gdal.Open(input_img)
    if ds is None:
        raise rsgislib.RSGISPyException("Could not open the input_img.")
    band_obj = ds.GetRasterBand(img_band)
    meta_data_dict = band_obj.GetMetadata_Dict()

    thematic_img = False
    if "LAYER_TYPE" in meta_data_dict:
        if meta_data_dict["LAYER_TYPE"] == "thematic":
            thematic_img = True
    ds = None

    return thematic_img


def has_gcps(input_img: str):
    """
    Test whether the input image has GCPs - returns boolean

    :param input_img: input image file
    :return: boolean True - has GCPs; False - does not have GCPs

    """
    raster = gdal.Open(input_img, gdal.GA_ReadOnly)
    if raster is None:
        raise rsgislib.RSGISPyException("Could not open the input_img.")
    numGCPs = raster.GetGCPCount()
    hasGCPs = False
    if numGCPs > 0:
        hasGCPs = True
    raster = None
    return hasGCPs


def copy_gcps(input_img: str, output_img: str):
    """
    Copy the GCPs from the input_img to the output_img

    :param input_img: Raster layer with GCPs
    :param output_img: Raster layer to which GCPs will be added

    """
    srcDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    if srcDS is None:
        raise rsgislib.RSGISPyException("Could not open the input_img.")
    destDS = gdal.Open(output_img, gdal.GA_Update)
    if destDS is None:
        srcDS = None
        raise rsgislib.RSGISPyException("Could not open the output_img.")

    numGCPs = srcDS.GetGCPCount()
    if numGCPs > 0:
        gcpProj = srcDS.GetGCPProjection()
        gcpList = srcDS.GetGCPs()
        destDS.SetGCPs(gcpList, gcpProj)

    srcDS = None
    destDS = None


def set_img_band_metadata(
    input_img: str, img_band: int, meta_field_name: str, meta_field_value: str
):
    """
    Function to set image band metadata value.

    :param input_img: the input image data
    :param img_band: the image band for the meta-data to be written to
    :param meta_field_name: the field name of the meta-data
    :param meta_field_value: the value of the meta-data to be written.

    """
    if img_band < 1:
        raise rsgislib.RSGISPyException(
            "The band number must be 1 or greater; note band numbering starts at 1."
        )

    ds = gdal.Open(input_img, gdal.GA_Update)
    if ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the image file: {}".format(input_img)
        )

    n_bands = ds.RasterCount
    if img_band > n_bands:
        raise rsgislib.RSGISPyException(
            "Band {} is not within the image file, which has {} bands".format(
                img_band, n_bands
            )
        )

    band_obj = ds.GetRasterBand(img_band)
    band_obj.SetMetadataItem(meta_field_name, "{}".format(meta_field_value))
    ds = None


def get_img_band_metadata(input_img: str, img_band: int, meta_field_name: str):
    """
    Function to get image band metadata value.

    :param input_img: the input image data
    :param img_band: the image band for the meta-data to be read
    :param meta_field_name: the field name of the meta-data

    """
    if img_band < 1:
        raise rsgislib.RSGISPyException(
            "The band number must be 1 or greater; note band numbering starts at 1."
        )

    ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the image file: {}".format(input_img)
        )

    n_bands = ds.RasterCount
    if img_band > n_bands:
        raise rsgislib.RSGISPyException(
            "Band {} is not within the image file, which has {} bands".format(
                img_band, n_bands
            )
        )

    band_obj = ds.GetRasterBand(img_band)
    meta_field_value = band_obj.GetMetadataItem(meta_field_name)
    ds = None

    return meta_field_value


def get_img_band_metadata_fields(input_img: str, img_band: int):
    """
    Function to get a list of the image band metadata names.

    :param input_img: the input image data
    :param img_band: the image band for the meta-data to be read

    """
    if img_band < 1:
        raise rsgislib.RSGISPyException(
            "The band number must be 1 or greater; note band numbering starts at 1."
        )

    ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the image file: {}".format(input_img)
        )

    n_bands = ds.RasterCount
    if img_band > n_bands:
        raise rsgislib.RSGISPyException(
            "Band {} is not within the image file, which has {} bands".format(
                img_band, n_bands
            )
        )

    band_obj = ds.GetRasterBand(img_band)
    meta_data_dict = band_obj.GetMetadata_Dict()
    ds = None

    return list(meta_data_dict.keys())


def get_img_band_metadata_fields_dict(input_img: str, img_band: int):
    """
    Function to get image band metadata names and values as a dict.

    :param input_img: the input image data
    :param img_band: the image band for the meta-data to be read

    """
    if img_band < 1:
        raise rsgislib.RSGISPyException(
            "The band number must be 1 or greater; note band numbering starts at 1."
        )

    ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the image file: {}".format(input_img)
        )

    n_bands = ds.RasterCount
    if img_band > n_bands:
        raise rsgislib.RSGISPyException(
            "Band {} is not within the image file, which has {} bands".format(
                img_band, n_bands
            )
        )

    band_obj = ds.GetRasterBand(img_band)
    meta_data_dict = band_obj.GetMetadata_Dict()
    ds = None

    return meta_data_dict


def set_img_metadata(input_img: str, meta_field_name: str, meta_field_value: str):
    """
    Function to set image metadata value.

    :param input_img: the input image data
    :param meta_field_name: the field name of the meta-data
    :param meta_field_value: the value of the meta-data to be written.

    """
    ds = gdal.Open(input_img, gdal.GA_Update)
    if ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the image file: {}".format(input_img)
        )

    ds.SetMetadataItem(meta_field_name, "{}".format(meta_field_value))
    ds = None


def get_img_metadata(input_img: str, meta_field_name: str):
    """
    Function to get image metadata value.

    :param input_img: the input image data
    :param meta_field_name: the field name of the meta-data

    """
    ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the image file: {}".format(input_img)
        )

    meta_field_value = ds.GetMetadataItem(meta_field_name)
    ds = None
    return meta_field_value


def get_img_metadata_fields(input_img: str):
    """
    Function to get a list of the image metadata names.

    :param input_img: the input image data
    :param band: the image band for the meta-data to be read

    """
    ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the image file: {}".format(input_img)
        )

    meta_data_dict = ds.GetMetadata_Dict()
    ds = None

    return list(meta_data_dict.keys())


def get_img_metadata_fields_dict(input_img: str):
    """
    Function to get image metadata names and values as a dict.

    :param input_img: the input image data

    """
    ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the image file: {}".format(input_img)
        )

    meta_data_dict = ds.GetMetadata_Dict()
    ds = None

    return meta_data_dict


def create_blank_img_py(
    output_img: str,
    n_bands: int,
    width: int,
    height: int,
    tl_x: float,
    tl_y: float,
    out_img_res_x: float,
    out_img_res_y: float,
    wkt_string: str,
    gdalformat: str,
    datatype: int,
    options: list = [],
    no_data_val: float = 0,
):
    """
    Create a blank output image file - this is a pure python implementation
    of rsgislib.imageutils.create_blank_img

    :param output_img: the output file and path.
    :param n_bands: the number of output image bands.
    :param width: the number of x pixels.
    :param height: the number of Y pixels.
    :param tl_x: the top-left corner x coordinate
    :param tl_y: the top-left corner y coordinate
    :param out_img_res_x: the output image resolution in the x axis
    :param out_img_res_y: the output image resolution in the y axis
    :param wkt_string: a WKT string with the output image projection
    :param gdalformat: the output image file format.
    :param datatype: the output image data type - needs to be a
                     rsgislib datatype (e.g., rsgislib.TYPE_32FLOAT)
    :param options: image creation options e.g., ["TILED=YES", "INTERLEAVE=PIXEL",
                    "COMPRESS=LZW", "BIGTIFF=YES"]
    :param no_data_val: the output image no data value.

    """
    gdal_data_type = rsgislib.get_gdal_datatype(datatype)
    gdal_driver = gdal.GetDriverByName(gdalformat)
    out_img_ds_obj = gdal_driver.Create(
        output_img, width, height, n_bands, gdal_data_type, options=options
    )
    out_img_ds_obj.SetGeoTransform((tl_x, out_img_res_x, 0, tl_y, 0, out_img_res_y))
    out_img_ds_obj.SetProjection(wkt_string)

    raster = numpy.zeros((height, width), dtype=rsgislib.get_numpy_datatype(datatype))
    raster[...] = no_data_val
    for band in range(n_bands):
        band_obj = out_img_ds_obj.GetRasterBand(band + 1)
        band_obj.SetNoDataValue(no_data_val)
        band_obj.WriteArray(raster)
    out_img_ds_obj = None


def create_blank_buf_img_from_ref_img(
    input_img: str,
    output_img: str,
    gdalformat: str,
    datatype: int,
    buf_pxl_ext: int = None,
    buf_spt_ext: float = None,
    no_data_val: float = None,
):
    """
    A function to create a new image file based on the input image but buffered by
    the specified amount (e.g., 100 pixels bigger on all sides). The buffer amount
    can be specified in pixels or spatial units. If non-None value is given for both
    inputs then an error will be produced. By default the no data value will be taken
    from the input image header but if not available or specified within the function
    call then that value will be used.

    :param input_img: input reference image
    :param output_img: output image file.
    :param gdalformat: output image file format.
    :param datatype: is a rsgislib.TYPE_* value providing the data type of the
                     output image.
    :param buf_pxl_ext: the amount the input image will be buffered in pixels.
    :param buf_spt_ext: the amount the input image will be buffered in spatial distance,
                        units are defined from the projection of the input image.
    :param no_data_val: Optional no data value. If None then the no data value will be
                        taken from the input image.

    """
    if (buf_pxl_ext is None) and (buf_spt_ext is None):
        raise rsgislib.RSGISPyException(
            "You must specify either the buf_pxl_ext or buf_spt_ext value."
        )

    if (buf_pxl_ext is not None) and (buf_spt_ext is not None):
        raise rsgislib.RSGISPyException(
            "You cannot specify both the buf_pxl_ext or buf_spt_ext value."
        )

    if no_data_val is None:
        no_data_val = get_img_no_data_value(input_img)

        if no_data_val is None:
            raise rsgislib.RSGISPyException("You must specify a no data value ")

    x_res, y_res = get_img_res(input_img, abs_vals=False)
    x_res_abs = abs(x_res)
    y_res_abs = abs(y_res)
    x_in_size, y_in_size = get_img_size(input_img)
    in_img_bbox = get_img_bbox(input_img)
    n_bands = get_img_band_count(input_img)
    wkt_str = get_wkt_proj_from_img(input_img)

    if buf_spt_ext is not None:
        buf_pxl_ext_x = math.ceil(buf_spt_ext / x_res_abs)
        buf_pxl_ext_y = math.ceil(buf_spt_ext / y_res_abs)

        x_out_size = x_in_size + (2 * buf_pxl_ext_x)
        y_out_size = y_in_size + (2 * buf_pxl_ext_y)

        out_tl_x = in_img_bbox[0] - (buf_pxl_ext_x * x_res_abs)
        out_tl_y = in_img_bbox[3] + (buf_pxl_ext_y * y_res_abs)
    else:
        x_out_size = x_in_size + (2 * buf_pxl_ext)
        y_out_size = y_in_size + (2 * buf_pxl_ext)

        out_tl_x = in_img_bbox[0] - (buf_pxl_ext * x_res_abs)
        out_tl_y = in_img_bbox[3] + (buf_pxl_ext * y_res_abs)

    create_blank_img(
        output_img,
        n_bands,
        x_out_size,
        y_out_size,
        out_tl_x,
        out_tl_y,
        x_res,
        y_res,
        no_data_val,
        "",
        wkt_str,
        gdalformat,
        datatype,
    )


def create_blank_buf_img_from_4326_ref_img(
    input_img: str,
    output_img: str,
    gdalformat: str,
    datatype: int,
    buf_pxl_ext: int,
    no_data_val: float = None,
):
    """
    A function to create a new image file based on the input image but buffered by
    the specified amount (e.g., 100 pixels bigger on all sides). By default the no
    data value will be taken from the input image header but if not available or
    specified within the function call then that value will be used. Note, this
    function only works with images projected in WGS84 (EPSG:4326) and deals with
    edge cases, reducing the image size so coordinates are within -180-180, -90-90
    valid range.

    :param input_img: input reference image
    :param output_img: output image file.
    :param gdalformat: output image file format.
    :param datatype: is a rsgislib.TYPE_* value providing the data type of the
                     output image.
    :param buf_pxl_ext: the amount the input image will be buffered in pixels.
    :param no_data_val: Optional no data value. If None then the no data value will be
                        taken from the input image.

    """
    img_epsg = get_epsg_proj_from_img(input_img)
    if img_epsg != 4326:
        raise rsgislib.RSGISPyException(
            "Expecting an input image with projection EPSG:4326"
        )

    if no_data_val is None:
        no_data_val = get_img_no_data_value(input_img)

        if no_data_val is None:
            raise rsgislib.RSGISPyException("You must specify a no data value ")

    x_res, y_res = get_img_res(input_img, abs_vals=False)
    x_res_abs = abs(x_res)
    y_res_abs = abs(y_res)
    x_in_size, y_in_size = get_img_size(input_img)
    in_img_bbox = get_img_bbox(input_img)
    n_bands = get_img_band_count(input_img)
    wkt_str = get_wkt_proj_from_img(input_img)

    x_out_size = x_in_size + (2 * buf_pxl_ext)
    y_out_size = y_in_size + (2 * buf_pxl_ext)

    out_tl_x = in_img_bbox[0] - (buf_pxl_ext * x_res_abs)
    out_tl_y = in_img_bbox[3] + (buf_pxl_ext * y_res_abs)

    br_x = out_tl_x + (x_out_size * x_res_abs)
    br_y = out_tl_y - (y_out_size * y_res_abs)

    if out_tl_x < -180:
        x_diff = -180 - out_tl_x
        n_pxls = int(math.ceil(x_diff / x_res_abs))
        out_tl_x = -180.0
        x_out_size -= n_pxls

    if br_x > 180:
        x_diff = 180 - br_x
        n_pxls = int(math.ceil(x_diff / x_res_abs))
        x_out_size -= n_pxls

    if br_y < -90:
        y_diff = -90 - br_y
        n_pxls = int(math.ceil(y_diff / y_res_abs))
        y_out_size -= n_pxls

    if out_tl_y > 90:
        y_diff = 90 - out_tl_y
        n_pxls = int(math.ceil(y_diff / y_res_abs))
        y_out_size -= n_pxls
        out_tl_y = 90.0

    create_blank_img(
        output_img,
        n_bands,
        x_out_size,
        y_out_size,
        out_tl_x,
        out_tl_y,
        x_res,
        y_res,
        no_data_val,
        "",
        wkt_str,
        gdalformat,
        datatype,
    )


def create_blank_img_from_ref_vector(
    vec_file: str,
    vec_lyr: str,
    output_img: str,
    out_img_res: float,
    out_img_n_bands: int,
    gdalformat: str,
    datatype: int,
):
    """
    A function to create a new image file based on a vector layer to define the
    extent and projection of the output image.

    :param vec_file: input vector file.
    :param vec_lyr: name of the vector layer, if None then assume the layer name will
                    be the same as the file name of the input vector file.
    :param output_img: output image file.
    :param out_img_res: output image resolution, square pixels so a single value.
    :param out_img_n_bands: the number of image bands in the output image
    :param gdalformat: output image file format.
    :param datatype: is a rsgislib.TYPE_* value providing the data type of the
                     output image

    """

    import rsgislib.tools.geometrytools
    import rsgislib.vectorutils

    baseExtent = rsgislib.vectorutils.get_vec_layer_extent(vec_file, vec_lyr)
    xMin, xMax, yMin, yMax = rsgislib.tools.geometrytools.find_extent_on_grid(
        baseExtent, out_img_res, full_contain=True
    )

    tlX = xMin
    tlY = yMax

    widthCoord = xMax - xMin
    heightCoord = yMax - yMin

    width = int(math.ceil(widthCoord / out_img_res))
    height = int(math.ceil(heightCoord / out_img_res))

    wktString = rsgislib.vectorutils.get_proj_wkt_from_vec(vec_file)

    create_blank_img(
        output_img,
        out_img_n_bands,
        width,
        height,
        tlX,
        tlY,
        out_img_res,
        (out_img_res * -1),
        0.0,
        "",
        wktString,
        gdalformat,
        datatype,
    )


def create_copy_img_vec_extent_snap_to_grid(
    vec_file: str,
    vec_lyr: str,
    output_img: str,
    out_img_res: float,
    out_img_n_bands: int,
    gdalformat: str,
    datatype: int,
    buf_n_pxl: int = 0,
):
    """
    A function to create a new image file based on a vector layer to define the
    extent and projection of the output image. The image file extent is snapped
    on to the grid defined by the vector layer.

    :param vec_file: input vector file.
    :param vec_lyr: name of the vector layer, if None then assume the layer name
                    will be the same as the file name of the input vector file.
    :param output_img: output image file.
    :param out_img_res: output image resolution, square pixels so a single value.
    :param out_img_n_bands: the number of image bands in the output image
    :param gdalformat: output image file format.
    :param datatype: is a rsgislib.TYPE_* value providing the data type of the
                     output image
    :param buf_n_pxl: is an integer specifying the number of pixels to buffer the
                      vector file extent by.

    """
    import rsgislib.tools.geometrytools
    import rsgislib.vectorutils

    vec_bbox = rsgislib.vectorutils.get_vec_layer_extent(
        vec_file, vec_lyr=vec_lyr, compute_if_exp=True
    )
    xMin = vec_bbox[0] - (out_img_res * buf_n_pxl)
    xMax = vec_bbox[1] + (out_img_res * buf_n_pxl)
    yMin = vec_bbox[2] - (out_img_res * buf_n_pxl)
    yMax = vec_bbox[3] + (out_img_res * buf_n_pxl)
    xMin, xMax, yMin, yMax = rsgislib.tools.geometrytools.find_extent_on_whole_num_grid(
        [xMin, xMax, yMin, yMax], out_img_res, True
    )

    tlX = xMin
    tlY = yMax

    widthCoord = xMax - xMin
    heightCoord = yMax - yMin

    width = int(math.ceil(widthCoord / out_img_res))
    height = int(math.ceil(heightCoord / out_img_res))

    wktString = rsgislib.vectorutils.get_proj_wkt_from_vec(vec_file)

    create_blank_img(
        output_img,
        out_img_n_bands,
        width,
        height,
        tlX,
        tlY,
        out_img_res,
        (out_img_res * -1),
        0.0,
        "",
        wktString,
        gdalformat,
        datatype,
    )


def create_blank_img_from_bbox(
    bbox: list,
    wkt_str: str,
    output_img: str,
    out_img_res: float,
    out_img_pxl_val: float,
    out_img_n_bands: int,
    gdalformat: str,
    datatype: int,
    snap_to_grid: bool = False,
):
    """
    A function to create a new image file based on a bbox to define the extent.

    :param bbox: bounding box defining the extent of the output image
                 (xMin, xMax, yMin, yMax)
    :param wkt_str: the WKT string defining the bbox and output image projection.
    :param output_img: output image file.
    :param out_img_res: output image resolution, square pixels so a single value.
    :param out_img_pxl_val: output image pixel value.
    :param out_img_n_bands: the number of image bands in the output image
    :param gdalformat: output image file format.
    :param datatype: is a rsgislib.TYPE_* value providing the data type of the
                     output image.
    :param snap_to_grid: optional variable to snap the image to a grid of whole
                         numbers with respect to the image pixel resolution.

    """
    if snap_to_grid:
        import rsgislib.tools.geometrytools

        bbox = rsgislib.tools.geometrytools.find_extent_on_grid(
            bbox, out_img_res, full_contain=True
        )

    xMin = bbox[0]
    xMax = bbox[1]
    yMin = bbox[2]
    yMax = bbox[3]

    tlX = xMin
    tlY = yMax

    widthCoord = xMax - xMin
    heightCoord = yMax - yMin

    width = int(math.ceil(widthCoord / out_img_res))
    height = int(math.ceil(heightCoord / out_img_res))

    create_blank_img(
        output_img,
        out_img_n_bands,
        width,
        height,
        tlX,
        tlY,
        out_img_res,
        (out_img_res * -1),
        out_img_pxl_val,
        "",
        wkt_str,
        gdalformat,
        datatype,
    )


def create_img_for_each_vec_feat(
    vec_file: str,
    vec_lyr: str,
    file_name_col: str,
    out_img_path: str,
    out_img_ext: str,
    out_img_pxl_val: float,
    out_img_n_bands: int,
    out_img_res: float,
    gdalformat: str,
    datatype: int,
    snap_to_grid: bool = False,
    ignore_exist: bool = True,
):
    """
    A function to create a set of image files representing the extent of each
    feature in the inputted vector file.

    :param vec_file: the input vector file.
    :param vec_lyr: the input vector layer
    :param file_name_col: the name of the column in the vector layer which will be
                          used as the file names.
    :param out_img_path: output file path (directory) where the images will be saved.
    :param out_img_ext: the file extension to be added on to the output file names.
    :param out_img_pxl_val: output image pixel value
    :param out_img_n_bands: the number of image bands in the output image
    :param out_img_res: output image resolution, square pixels so a single value
    :param gdalformat: output image file format.
    :param datatype: is a rsgislib.TYPE_* value providing the data type of the
                     output image.
    :param snap_to_grid: snap the output image to a grid of whole numbers
                         with respect to the image pixel resolution.
    :param ignore_exist: ignore outputs which already exist and therefore
                         only create those which don't exist.
    """

    dsVecFile = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if dsVecFile is None:
        raise rsgislib.RSGISPyException("Could not open '" + vec_file + "'")

    lyrVecObj = dsVecFile.GetLayerByName(vec_lyr)
    if lyrVecObj is None:
        raise rsgislib.RSGISPyException("Could not find layer '" + vec_lyr + "'")

    lyrSpatRef = lyrVecObj.GetSpatialRef()
    if lyrSpatRef is not None:
        wktstr = lyrSpatRef.ExportToWkt()
    else:
        wktstr = ""

    colExists = False
    feat_idx = 0
    lyrDefn = lyrVecObj.GetLayerDefn()
    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName().lower() == file_name_col.lower():
            feat_idx = i
            colExists = True
            break

    if not colExists:
        dsVecFile = None
        raise rsgislib.RSGISPyException(
            "The specified column does not exist in the input layer; "
            "check case as some drivers are case sensitive."
        )

    lyrVecObj.ResetReading()
    for feat in lyrVecObj:
        geom = feat.GetGeometryRef()
        if geom is not None:
            env = geom.GetEnvelope()
            tilebasename = feat.GetFieldAsString(feat_idx)
            output_img = os.path.join(
                out_img_path, "{0}.{1}".format(tilebasename, out_img_ext)
            )
            create_out_file = True
            if ignore_exist and os.path.exists(output_img):
                create_out_file = False

            if create_out_file:
                print(output_img)
                create_blank_img_from_bbox(
                    env,
                    wktstr,
                    output_img,
                    out_img_res,
                    out_img_pxl_val,
                    out_img_n_bands,
                    gdalformat,
                    datatype,
                    snap_to_grid,
                )


def resample_img_to_match(
    in_ref_img: str,
    in_process_img: str,
    output_img: str,
    gdalformat: str,
    interp_method: int = rsgislib.INTERP_NEAREST_NEIGHBOUR,
    datatype: int = None,
    no_data_val: float = None,
    multicore: bool = False,
):
    """
    A utility function to resample an existing image to the projection
    and/or pixel size of another image.

    :param in_ref_img: is the input reference image to which the processing
                       image is to resampled to.
    :param in_process_img: is the image which is to be resampled.
    :param output_img: is the output image file.
    :param gdalformat: is the gdal format for the output image.
    :param interp_method: is the interpolation method used to resample the image
                          rsgislib.INTERP_XXXX (Default:
                          rsgislib.INTERP_NEAREST_NEIGHBOUR)
    :param datatype: is the rsgislib datatype of the output image (if none then
                     it will be the same as the input file).
    :param multicore: use multiple processing cores (Default = False)
    """
    numBands = get_img_band_count(in_process_img)
    if no_data_val is None:
        no_data_val = get_img_no_data_value(in_process_img)

    if datatype is None:
        datatype = get_gdal_datatype_from_img(in_process_img)

    gdal_interp_method = rsgislib.get_gdal_interp_type(interp_method)

    backVal = 0.0
    haveNoData = False
    if no_data_val != None:
        backVal = float(no_data_val)
        haveNoData = True

    create_copy_img(in_ref_img, output_img, numBands, backVal, gdalformat, datatype)

    inFile = gdal.Open(in_process_img, gdal.GA_ReadOnly)
    outFile = gdal.Open(output_img, gdal.GA_Update)

    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    wrpOpts = []
    if multicore:
        if haveNoData:
            wrpOpts = gdal.WarpOptions(
                resampleAlg=gdal_interp_method,
                srcNodata=no_data_val,
                dstNodata=no_data_val,
                multithread=True,
                callback=callback,
            )
        else:
            wrpOpts = gdal.WarpOptions(
                resampleAlg=gdal_interp_method, multithread=True, callback=callback
            )
    else:
        if haveNoData:
            wrpOpts = gdal.WarpOptions(
                resampleAlg=gdal_interp_method,
                srcNodata=no_data_val,
                dstNodata=no_data_val,
                multithread=False,
                callback=callback,
            )
        else:
            wrpOpts = gdal.WarpOptions(
                resampleAlg=gdal_interp_method, multithread=False, callback=callback
            )

    gdal.Warp(outFile, inFile, options=wrpOpts)

    inFile = None
    outFile = None


def reproject_image(
    input_img: str,
    output_img: str,
    out_wkt: str,
    gdalformat: str = "KEA",
    interp_method: int = rsgislib.INTERP_NEAREST_NEIGHBOUR,
    in_wkt: str = None,
    no_data_val: float = 0.0,
    out_pxl_res: float = "image",
    snap_to_grid: bool = True,
    multicore: bool = False,
    gdal_options: list = [],
):
    """
    This function provides a tool which uses the gdalwarp function to reproject an
    input image. When you want an simpler interface use the
    rsgislib.imageutils.gdal_warp function. This handles more automatically.

    :param input_img: the input image name and path
    :param output_img: the output image name and path
    :param out_wkt: a WKT file representing the output projection
    :param gdalformat: the output image file format (Default is KEA)
    :param interp_method: is the interpolation method used to resample the image
                          rsgislib.INTERP_XXXX (Default:
                          rsgislib.INTERP_NEAREST_NEIGHBOUR)
    :param in_wkt: if input image is not well defined this is the input image
                   projection as a WKT file (Default is None, i.e., ignored)
    :param no_data_val: float representing the not data value (Default is 0.0)
    :param out_pxl_res: three inputs can be provided. 1) 'image' where the output
                        resolution will match the input (Default is image). 2) 'auto'
                        where an output resolution maintaining the image size of the
                        input image will be used. You may consider using
                        rsgislib.imageutils.gdal_warp instead of this option.
                        3) provide a floating point value for the image resolution
                        (note. pixels will be sqaure)
    :param snap_to_grid: is a boolean specifying whether the TL pixel should be
                         snapped to a multiple of the pixel resolution (Default: True)
    :param multicore: use multiple cores for warpping (Default=False)
    :param gdal_options: GDAL file creation options e.g., ["TILED=YES",
                         "COMPRESS=LZW", "BIGTIFF=YES"]

    """
    import rsgislib.tools.geometrytools
    import rsgislib.tools.utils

    gdal_interp_method = rsgislib.get_gdal_interp_type(interp_method)

    if not os.path.exists(input_img):
        raise rsgislib.RSGISPyException(
            "The input image file does not exist: '" + input_img + "'"
        )

    inImgDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    if inImgDS is None:
        raise rsgislib.RSGISPyException(
            "Could not open the Input Image: '" + input_img + "'"
        )

    inImgProj = osr.SpatialReference()
    if not in_wkt is None:
        if not os.path.exists(in_wkt):
            raise rsgislib.RSGISPyException(
                "The input WKT file does not exist: '" + in_wkt + "'"
            )
        inWKTStr = rsgislib.tools.utils.read_text_file_no_new_lines(in_wkt)
        inImgProj.ImportFromWkt(inWKTStr)
    else:
        inImgProj.ImportFromWkt(inImgDS.GetProjectionRef())

    if not os.path.exists(out_wkt):
        raise rsgislib.RSGISPyException(
            "The output WKT file does not exist: '" + out_wkt + "'"
        )
    outImgProj = osr.SpatialReference()
    outWKTStr = rsgislib.tools.utils.read_text_file_no_new_lines(out_wkt)
    outImgProj.ImportFromWkt(outWKTStr)

    geoTransform = inImgDS.GetGeoTransform()
    if geoTransform is None:
        raise rsgislib.RSGISPyException(
            "Could read the geotransform from the Input Image: '" + input_img + "'"
        )

    xPxlRes = geoTransform[1]
    yPxlRes = geoTransform[5]

    inRes = xPxlRes
    if math.fabs(yPxlRes) < math.fabs(xPxlRes):
        inRes = math.fabs(yPxlRes)

    xSize = inImgDS.RasterXSize
    ySize = inImgDS.RasterYSize

    tlXIn = geoTransform[0]
    tlYIn = geoTransform[3]

    brXIn = tlXIn + (xSize * math.fabs(xPxlRes))
    brYIn = tlYIn - (ySize * math.fabs(yPxlRes))

    trXIn = brXIn
    trYIn = tlYIn

    blXIn = tlXIn
    blYIn = trYIn

    numBands = inImgDS.RasterCount

    inImgBand = inImgDS.GetRasterBand(1)
    gdalDataType = gdal.GetDataTypeName(inImgBand.DataType)
    rsgisDataType = rsgislib.get_rsgislib_datatype(gdalDataType)

    tlXOut, tlYOut = rsgislib.tools.geometrytools.reproj_point(
        inImgProj, outImgProj, tlXIn, tlYIn
    )
    brXOut, brYOut = rsgislib.tools.geometrytools.reproj_point(
        inImgProj, outImgProj, brXIn, brYIn
    )
    trXOut, trYOut = rsgislib.tools.geometrytools.reproj_point(
        inImgProj, outImgProj, trXIn, trYIn
    )
    blXOut, blYOut = rsgislib.tools.geometrytools.reproj_point(
        inImgProj, outImgProj, blXIn, blYIn
    )

    xValsOut = [tlXOut, brXOut, trXOut, blXOut]
    yValsOut = [tlYOut, brYOut, trYOut, blYOut]

    xMax = max(xValsOut)
    xMin = min(xValsOut)

    yMax = max(yValsOut)
    yMin = min(yValsOut)

    out_pxl_res = str(out_pxl_res).strip()
    outRes = 0.0
    if rsgislib.tools.utils.is_number(out_pxl_res):
        outRes = math.fabs(float(out_pxl_res))
    elif out_pxl_res == "image":
        outRes = inRes
    elif out_pxl_res == "auto":
        xOutRes = (brXOut - tlXOut) / xSize
        yOutRes = (tlYOut - brYOut) / ySize
        outRes = xOutRes
        if yOutRes < xOutRes:
            outRes = yOutRes
    else:
        raise rsgislib.RSGISPyException(
            "Was not able to defined the output resolution. Check Input: '"
            + out_pxl_res
            + "'"
        )

    outTLX = xMin
    outTLY = yMax
    outWidth = int(round((xMax - xMin) / outRes)) + 1
    outHeight = int(round((yMax - yMin) / outRes)) + 1

    if snap_to_grid:
        xLeft = outTLX % outRes
        yLeft = outTLY % outRes

        outTLX = (outTLX - xLeft) - (5 * outRes)
        outTLY = ((outTLY - yLeft) + outRes) + (5 * outRes)

        outWidth = int(round((xMax - xMin) / outRes)) + 10
        outHeight = int(round((yMax - yMin) / outRes)) + 10

    print("Creating blank image")
    create_blank_img_py(
        output_img,
        numBands,
        outWidth,
        outHeight,
        outTLX,
        outTLY,
        outRes,
        (outRes * (-1)),
        outWKTStr,
        gdalformat,
        rsgisDataType,
        options=gdal_options,
        no_data_val=no_data_val,
    )

    outImgDS = gdal.Open(output_img, gdal.GA_Update)

    for i in range(numBands):
        outImgDS.GetRasterBand(i + 1).SetNoDataValue(no_data_val)

    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    print("Performing the reprojection")
    wrpOpts = []
    if multicore:
        wrpOpts = gdal.WarpOptions(
            resampleAlg=gdal_interp_method,
            srcNodata=no_data_val,
            dstNodata=no_data_val,
            multithread=True,
            callback=callback,
        )
    else:
        wrpOpts = gdal.WarpOptions(
            resampleAlg=gdal_interp_method,
            srcNodata=no_data_val,
            dstNodata=no_data_val,
            multithread=False,
            callback=callback,
        )

    gdal.Warp(outImgDS, inImgDS, options=wrpOpts)

    inImgDS = None
    outImgDS = None


def gdal_warp(
    input_img: str,
    output_img: str,
    out_epsg: int,
    interp_method: int = rsgislib.INTERP_NEAREST_NEIGHBOUR,
    gdalformat: str = "KEA",
    use_multi_threaded: bool = True,
    options: list = [],
):
    """
    A function which runs GDAL Warp function to tranform an image from one projection
    to another. Use this function when you want GDAL to do processing of pixel size
    and image size automatically. rsgislib.imageutils.reproject_image
    should be used when you want to put the output image on a particular grid etc.

    :param input_img: input image file
    :param output_img: output image file
    :param out_epsg: the EPSG for the output image file.
    :param interp_method: is the interpolation method used to resample the image
                          rsgislib.INTERP_XXXX (Default:
                          rsgislib.INTERP_NEAREST_NEIGHBOUR)
    :param gdalformat: output image file format
    :param use_multi_threaded: Use multiple cores for processing (Default: True).
    :param options: GDAL file creation options e.g., ["TILED=YES", "COMPRESS=LZW",
                    "BIGTIFF=YES"]

    """
    in_no_data_val = get_img_no_data_value(input_img)
    in_epsg = get_epsg_proj_from_img(input_img)
    img_data_type = get_gdal_datatype_from_img(input_img)

    gdal_interp_method = rsgislib.get_gdal_interp_type(interp_method)

    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress
    warp_opts = gdal.WarpOptions(
        format=gdalformat,
        srcSRS="EPSG:{}".format(in_epsg),
        dstSRS="EPSG:{}".format(out_epsg),
        resampleAlg=gdal_interp_method,
        srcNodata=in_no_data_val,
        dstNodata=in_no_data_val,
        callback=callback,
        creationOptions=options,
        outputType=img_data_type,
        workingType=gdal.GDT_Float32,
        multithread=use_multi_threaded,
    )
    gdal.Warp(output_img, input_img, options=warp_opts)


def subset_pxl_bbox(
    input_img: str,
    output_img: str,
    gdalformat: str,
    datatype: int,
    x_min_pxl: int,
    x_max_pxl: int,
    y_min_pxl: int,
    y_max_pxl: int,
):
    """
    Function to subset an input image using a defined pixel bbox.

    :param input_img: input image to be subset.
    :param output_img: output image file.
    :param gdalformat: output image file format
    :param datatype: datatype is a rsgislib.TYPE_* value providing the data
                     type of the output image.
    :param x_min_pxl: min x in pixels
    :param x_max_pxl: max x in pixels
    :param y_min_pxl: min y in pixels
    :param y_max_pxl: max y in pixels

    """
    bbox = get_img_bbox(input_img)
    xRes, yRes = get_img_res(input_img, abs_vals=True)
    xSize, ySize = get_img_size(input_img)

    if (x_max_pxl > xSize) or (y_max_pxl > ySize):
        raise rsgislib.RSGISPyException(
            "The pixel extent defined is bigger than the input image."
        )

    xMin = bbox[0] + (x_min_pxl * xRes)
    xMax = bbox[0] + (x_max_pxl * xRes)
    yMin = bbox[2] + (y_min_pxl * yRes)
    yMax = bbox[2] + (y_max_pxl * yRes)

    subset_bbox(input_img, output_img, gdalformat, datatype, xMin, xMax, yMin, yMax)


def _run_subset(tileinfo):
    """Internal function for create_tiles_multi_core for multiprocessing Pool."""
    subset_pxl_bbox(
        tileinfo["input_img"],
        tileinfo["outfile"],
        tileinfo["gdalformat"],
        tileinfo["datatype"],
        tileinfo["bbox"][0],
        tileinfo["bbox"][1],
        tileinfo["bbox"][2],
        tileinfo["bbox"][3],
    )


def create_tiles_multi_core(
    input_img: str,
    out_img_base: str,
    width: int,
    height: int,
    gdalformat: str,
    datatype: int,
    out_img_ext: str,
    n_cores: int = 1,
):
    """
    Function to generate a set of tiles for the input image.

    :param input_img: input image to be subset.
    :param out_img_base: output image files base path.
    :param width: width in pixels of the tiles.
    :param height: height in pixels of the tiles.
    :param gdalformat: output image file format
    :param datatype: datatype is a rsgislib.TYPE_* value providing the data type
                     of the output image.
    :param out_img_ext: output file extension to be added to the base image
                        path (e.g., kea)
    :param n_cores: number of cores to be used; uses python multiprocessing module.

    """
    import multiprocessing

    import rsgislib.tools.filetools

    if not (
        rsgislib.tools.filetools.does_path_exists_or_creatable(out_img_base)
        and os.path.exists(os.path.split(out_img_base)[0])
    ):
        raise rsgislib.RSGISPyException("Output path is not valid or exist.")

    x_size, y_size = get_img_size(input_img)

    n_full_xtiles = math.floor(x_size / width)
    x_remain_width = x_size - (n_full_xtiles * width)
    n_full_ytiles = math.floor(y_size / height)
    y_remain_height = y_size - (n_full_ytiles * height)

    tiles = []

    for y_tile in range(n_full_ytiles):
        y_pxl_min = y_tile * height
        y_pxl_max = y_pxl_min + height

        for x_tile in range(n_full_xtiles):
            x_pxl_min = x_tile * width
            x_pxl_max = x_pxl_min + width
            tiles.append(
                {
                    "tile": "x{0}y{1}".format(x_tile + 1, y_tile + 1),
                    "bbox": [x_pxl_min, x_pxl_max, y_pxl_min, y_pxl_max],
                }
            )

        if x_remain_width > 0:
            x_pxl_min = n_full_xtiles * width
            x_pxl_max = x_pxl_min + x_remain_width
            tiles.append(
                {
                    "tile": "x{0}y{1}".format(n_full_xtiles + 1, y_tile + 1),
                    "bbox": [x_pxl_min, x_pxl_max, y_pxl_min, y_pxl_max],
                }
            )

    if y_remain_height > 0:
        y_pxl_min = n_full_ytiles * height
        y_pxl_max = y_pxl_min + y_remain_height

        for x_tile in range(n_full_xtiles):
            x_pxl_min = x_tile * width
            x_pxl_max = x_pxl_min + width
            tiles.append(
                {
                    "tile": "x{0}y{1}".format(x_tile + 1, n_full_ytiles + 1),
                    "bbox": [x_pxl_min, x_pxl_max, y_pxl_min, y_pxl_max],
                }
            )

        if x_remain_width > 0:
            x_pxl_min = n_full_xtiles * width
            x_pxl_max = x_pxl_min + x_remain_width
            tiles.append(
                {
                    "tile": "x{0}y{1}".format(n_full_xtiles + 1, n_full_ytiles + 1),
                    "bbox": [x_pxl_min, x_pxl_max, y_pxl_min, y_pxl_max],
                }
            )

    for tile in tiles:
        tile["input_img"] = input_img
        tile["outfile"] = "{0}_{1}.{2}".format(out_img_base, tile["tile"], out_img_ext)
        tile["gdalformat"] = gdalformat
        tile["datatype"] = datatype

    poolobj = multiprocessing.Pool(n_cores)
    poolobj.map(_run_subset, tiles)


def calc_pixel_locations(input_img: str, output_img: str, gdalformat: str):
    """
    Function which produces a 2 band output image with the X and Y locations
    of the image pixels.

    :param input_img: the input reference image
    :param output_img: the output image file name and path (will be same
                       dimensions as the input)
    :param gdalformat: the GDAL image file format of the output image file.

    """
    try:
        import tqdm

        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.image1 = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _getXYPxlLocs(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        xBlock, yBlock = info.getBlockCoordArrays()
        outputs.outimage = numpy.stack((xBlock, yBlock))

    applier.apply(_getXYPxlLocs, infiles, outfiles, otherargs, controls=aControls)


def calc_wgs84_pixel_area(
    input_img: str, output_img: str, scale: float = 10000, gdalformat: str = "KEA"
):
    """
    A function which calculates the area (in metres) of the pixel projected in WGS84.

    :param input_img: input image, for which the per-pixel area will be calculated.
    :param output_img: output image file.
    :param scale: scale the output area to unit of interest. Scale=10000(Ha),
                        Scale=1(sq m), Scale=1000000(sq km), Scale=4046.856(Acre),
                        Scale=2590000(sq miles), Scale=0.0929022668(sq feet)

    """
    from rios import applier

    import rsgislib.tools.projection

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    x_res, y_res = get_img_res(input_img, abs_vals=True)

    infiles = applier.FilenameAssociations()
    infiles.input_img = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.x_res = x_res
    otherargs.y_res = y_res
    otherargs.scale = float(scale)
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = False
    aControls.calcStats = False

    def _calcPixelArea(info, inputs, outputs, otherargs):
        xBlock, yBlock = info.getBlockCoordArrays()

        x_res_arr = numpy.zeros_like(yBlock, dtype=float)
        x_res_arr[...] = otherargs.x_res
        y_res_arr = numpy.zeros_like(yBlock, dtype=float)
        y_res_arr[...] = otherargs.y_res
        x_res_arr_m, y_res_arr_m = rsgislib.tools.projection.degrees_to_metres(
            yBlock, x_res_arr, y_res_arr
        )
        outputs.outimage = numpy.expand_dims(
            (x_res_arr_m * y_res_arr_m) / otherargs.scale, axis=0
        )

    applier.apply(_calcPixelArea, infiles, outfiles, otherargs, controls=aControls)


def do_images_overlap(in_a_img: str, in_b_img: str, over_thres: int = 0.0):
    """
    Function to test whether two images overlap with one another.
    If the images have a difference projection/coordinate system then corners

    :param in_a_img: path to first image
    :param in_b_img: path to second image
    :param over_thres: the amount of overlap required to return true
                       (e.g., at least 1 pixel)

    :return: Boolean specifying whether they overlap or not.

    .. code:: python

        import rsgislib.imageutils
        overlap = rsgislib.imageutils.do_images_overlap("tile_8.kea", "input.tif")
        print("Images Overlap: {}".format(overlap))

    """
    import rsgislib.tools.geometrytools

    overlap = True

    projSame = False
    if do_gdal_layers_have_same_proj(in_a_img, in_b_img):
        projSame = True

    img1DS = gdal.Open(in_a_img, gdal.GA_ReadOnly)
    if img1DS is None:
        raise rsgislib.RSGISPyException("Could not open image: " + in_a_img)

    img2DS = gdal.Open(in_b_img, gdal.GA_ReadOnly)
    if img2DS is None:
        raise rsgislib.RSGISPyException("Could not open image: " + in_b_img)

    img1GeoTransform = img1DS.GetGeoTransform()
    if img1GeoTransform is None:
        img1DS = None
        img2DS = None
        raise rsgislib.RSGISPyException("Could not get geotransform: " + in_a_img)

    img2GeoTransform = img2DS.GetGeoTransform()
    if img2GeoTransform is None:
        img1DS = None
        img2DS = None
        raise rsgislib.RSGISPyException("Could not get geotransform: " + in_b_img)

    img1TLX = img1GeoTransform[0]
    img1TLY = img1GeoTransform[3]

    img1BRX = img1GeoTransform[0] + (img1DS.RasterXSize * img1GeoTransform[1])
    img1BRY = img1GeoTransform[3] + (img1DS.RasterYSize * img1GeoTransform[5])

    img2TLX_orig = img2GeoTransform[0]
    img2TLY_orig = img2GeoTransform[3]

    img2BRX_orig = img2GeoTransform[0] + (img2DS.RasterXSize * img2GeoTransform[1])
    img2BRY_orig = img2GeoTransform[3] + (img2DS.RasterYSize * img2GeoTransform[5])

    img1EPSG = get_epsg_proj_from_img(in_a_img)
    img2EPSG = get_epsg_proj_from_img(in_b_img)

    if projSame:
        img2TLX = img2GeoTransform[0]
        img2TLY = img2GeoTransform[3]

        img2BRX = img2GeoTransform[0] + (img2DS.RasterXSize * img2GeoTransform[1])
        img2BRY = img2GeoTransform[3] + (img2DS.RasterYSize * img2GeoTransform[5])
    else:
        inProj = osr.SpatialReference()

        if img2EPSG is None:
            wktImg2 = get_wkt_proj_from_img(in_b_img)
            if (wktImg2 is None) or (wktImg2 == ""):
                raise rsgislib.RSGISPyException(
                    "Could not retrieve EPSG or WKT for image: " + in_b_img
                )
            inProj.ImportFromWkt(wktImg2)
        else:
            inProj.ImportFromEPSG(int(img2EPSG))

        outProj = osr.SpatialReference()
        if img1EPSG is None:
            wktImg1 = get_wkt_proj_from_img(in_a_img)
            if (wktImg1 is None) or (wktImg1 == ""):
                raise rsgislib.RSGISPyException(
                    "Could not retrieve EPSG or WKT for image: " + in_a_img
                )
            outProj.ImportFromWkt(wktImg1)
        else:
            outProj.ImportFromEPSG(int(img1EPSG))

        if img1EPSG is None:
            img1EPSG = 0

        img2TLX, img2TLY = rsgislib.tools.geometrytools.reproj_point(
            inProj, outProj, img2TLX_orig, img2TLY_orig
        )
        img2BRX, img2BRY = rsgislib.tools.geometrytools.reproj_point(
            inProj, outProj, img2BRX_orig, img2BRY_orig
        )

    xMin = img1TLX
    xMax = img1BRX
    yMin = img1BRY
    yMax = img1TLY

    if img2TLX > xMin:
        xMin = img2TLX
    if img2BRX < xMax:
        xMax = img2BRX
    if img2BRY > yMin:
        yMin = img2BRY
    if img2TLY < yMax:
        yMax = img2TLY

    if (xMax - xMin <= over_thres) or (yMax - yMin <= over_thres):
        overlap = False

    return overlap


def generate_random_pxl_vals_img(
    input_img: str, output_img: str, gdalformat: str, low_val: int, up_val: int
):
    """
    Function which produces a 1 band image with random integer values between
    lowVal and upVal.

    :param input_img: the input reference image
    :param output_img: the output image file name and path (will be same dimensions
                       as the input)
    :param gdalformat: the GDAL image file format of the output image file.
    :param low_val: lower value
    :param up_val: upper value

    """
    try:
        import tqdm

        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.inImg = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.lowVal = low_val
    otherargs.upVal = up_val
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _popPxlsRanVals(info, inputs, outputs, otherargs):
        """
        This is an internal rios function for generate_random_pxl_vals_img()
        """
        outputs.outimage = numpy.random.randint(
            otherargs.lowVal, high=otherargs.upVal, size=inputs.inImg.shape
        )
        outputs.outimage = outputs.outimage.astype(numpy.int32, copy=False)

    applier.apply(_popPxlsRanVals, infiles, outfiles, otherargs, controls=aControls)


def extract_img_pxl_sample(
    input_img: str, pxl_n_sample: int, no_data_val: float = None
):
    """
    A function which extracts a sample of pixels from the
    input image file to a number array.

    :param input_img: the image from which the random sample will be taken.
    :param pxl_n_sample: the sample to be taken (e.g., a value of 100 will
                         sample every 100th, valid (if noData specified), pixel)
    :param no_data_val: provide a no data value which is to be ignored during
                        processing. If None then ignored (Default: None)
    :return: outputs a numpy array (n sampled values, n bands)

    """
    # Import the RIOS image reader
    import tqdm
    from rios.imagereader import ImageReader

    first = True
    reader = ImageReader(input_img, windowxsize=200, windowysize=200)
    for info, block in tqdm.tqdm(reader):
        blkShape = block.shape
        blkBands = block.reshape((blkShape[0], (blkShape[1] * blkShape[2])))

        blkBandsTrans = numpy.transpose(blkBands)

        if no_data_val is not None:
            blkBandsTrans = blkBandsTrans[(blkBandsTrans != no_data_val).all(axis=1)]

        if blkBandsTrans.shape[0] > 0:
            nSamp = int((blkBandsTrans.shape[0]) / pxl_n_sample)
            nSampRange = numpy.arange(0, nSamp, 1) * pxl_n_sample
            blkBandsTransSamp = blkBandsTrans[nSampRange]

            if first:
                outArr = blkBandsTransSamp
                first = False
            else:
                outArr = numpy.concatenate((outArr, blkBandsTransSamp), axis=0)
    return outArr


def extract_img_pxl_vals_in_msk(
    input_img: str,
    img_bands: list,
    in_msk_img: str,
    img_mask_val: int,
    no_data_val: float = None,
):
    """
    A function which extracts the image values within a mask for the specified
    image bands.

    :param input_img: the image from which the random sample will be taken.
    :param img_bands: the image bands the values are to be read from.
    :param in_msk_img: the image mask specifying the regions of interest.
    :param img_mask_val: the pixel value within the mask defining the region
                         of interest.
    :return: outputs a numpy array (n values, n bands)

    """
    # Import the RIOS image reader
    import tqdm
    from rios.imagereader import ImageReader

    outArr = None
    first = True
    reader = ImageReader([input_img, in_msk_img], windowxsize=200, windowysize=200)
    for info, block in tqdm.tqdm(reader):
        blk_img = block[0]
        blk_msk = block[1].flatten()
        blk_img_shape = blk_img.shape

        blk_bands = blk_img.reshape(
            (blk_img_shape[0], (blk_img_shape[1] * blk_img_shape[2]))
        )
        band_lst = []
        for band in img_bands:
            if (band > 0) and (band <= blk_bands.shape[0]):
                band_lst.append(blk_bands[band - 1])
            else:
                raise rsgislib.RSGISPyException(
                    "Band ({}) specified is not within the image".format(band)
                )
        blk_bands_sel = numpy.stack(band_lst, axis=0)
        blk_bands_trans = numpy.transpose(blk_bands_sel)

        if no_data_val is not None:
            blk_msk = blk_msk[(blk_bands_trans != no_data_val).all(axis=1)]
            blk_bands_trans = blk_bands_trans[
                (blk_bands_trans != no_data_val).all(axis=1)
            ]

        if blk_bands_trans.shape[0] > 0:
            blk_bands_trans = blk_bands_trans[blk_msk == img_mask_val]
            if first:
                out_arr = blk_bands_trans
                first = False
            else:
                out_arr = numpy.concatenate((out_arr, blk_bands_trans), axis=0)
    return out_arr


def combine_binary_masks(
    msk_imgs_dict: dict, output_img: str, out_lut_file: str, gdalformat: str = "KEA"
):
    """
    A function which combines up to 8 binary image masks to create a single
    output image with a unique value for each combination of intersecting
    masks. A JSON LUT is also generated to identify the image values to a
    'class'.

    :param msk_imgs_dict: dict of input images.
    :param output_img: output image file.
    :param out_lut_file: output file path to JSON LUT file identifying the image values.
    :param gdalformat: output GDAL format (e.g., KEA)

    """
    import json

    import rsgislib.imagecalc
    import rsgislib.tools.utils

    try:
        import tqdm

        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    in_vals_dict = dict()
    msk_imgs = list()
    for key in msk_imgs_dict.keys():
        msk_imgs.append(msk_imgs_dict[key])
        in_vals_dict[key] = [0, 1]

    # Generated the combined mask.
    infiles = applier.FilenameAssociations()
    infiles.msk_imgs = msk_imgs
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = False
    aControls.calcStats = False

    def _combineMsks(info, inputs, outputs, otherargs):
        out_arr = numpy.zeros_like(inputs.msk_imgs[0], dtype=numpy.uint8)
        out_bit_arr = numpy.unpackbits(out_arr, axis=2)
        img_n = 0
        for img in inputs.msk_imgs:
            for x in range(img.shape[1]):
                for y in range(img.shape[2]):
                    if img[0, x, y] == 1:
                        out_bit_arr[0, x, (8 * y) + img_n] = 1
            img_n = img_n + 1

        out_arr = numpy.packbits(out_bit_arr, axis=2)

        outputs.outimage = out_arr

    applier.apply(_combineMsks, infiles, outfiles, otherargs, controls=aControls)

    # find the unique output image files.
    uniq_vals = rsgislib.imagecalc.get_unique_values(output_img, img_band=1)

    # find the powerset of the inputs
    possible_outputs = rsgislib.tools.utils.create_var_list(in_vals_dict, val_dict=None)

    out_poss_lut = dict()
    for poss in possible_outputs:
        val = numpy.zeros(1, dtype=numpy.uint8)
        val_bit_arr = numpy.unpackbits(val, axis=0)
        i = 0
        for key in msk_imgs_dict.keys():
            val_bit_arr[i] = poss[key]
            i = i + 1
        out_arr = numpy.packbits(val_bit_arr)
        if out_arr[0] in uniq_vals:
            out_poss_lut[str(out_arr[0])] = poss

    with open(out_lut_file, "w") as outJSONfile:
        json.dump(
            out_poss_lut,
            outJSONfile,
            sort_keys=True,
            indent=4,
            separators=(",", ": "),
            ensure_ascii=False,
        )


def gdal_translate(
    input_img: str, output_img: str, gdalformat: str = "KEA", options: str = ""
):
    """
    Using GDAL translate to convert input image to a different format, if GTIFF
    selected and no options are provided then a cloud optimised GeoTIFF will be
    outputted.

    :param input_img: Input image which is GDAL readable.
    :param output_img: The output image file.
    :param gdalformat: The output image file format
    :param options: options for the output driver (e.g., "-co TILED=YES
                    -co COMPRESS=LZW -co BIGTIFF=YES")

    """
    if (gdalformat == "GTIFF") and (options == ""):
        options = (
            "-co TILED=YES -co INTERLEAVE=PIXEL -co BLOCKXSIZE=256 "
            "-co BLOCKYSIZE=256 -co COMPRESS=LZW -co BIGTIFF=YES "
            "-co COPY_SRC_OVERVIEWS=YES"
        )

    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    trans_opt = gdal.TranslateOptions(
        format=gdalformat, options=options, callback=callback
    )
    gdal.Translate(output_img, input_img, options=trans_opt)


def create_stack_images_vrt(input_imgs: list, out_vrt_file: str):
    """
    A function which creates a GDAL VRT file from a set of input images by stacking
    the input images in a multi-band output file.

    :param input_imgs: A list of input images
    :param out_vrt_file: The output file location for the VRT.

    """
    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    build_vrt_opt = gdal.BuildVRTOptions(separate=True, callback=callback)
    gdal.BuildVRT(out_vrt_file, input_imgs, options=build_vrt_opt)


def create_mosaic_images_vrt(
    input_imgs: list,
    out_vrt_file: str,
    vrt_extent: List = None,
    vrt_out_res_x: float = None,
    vrt_out_res_y: float = None,
    interp_method: int = rsgislib.INTERP_NEAREST_NEIGHBOUR,
    align_out_pxls: bool = False,
):
    """
    A function which creates a GDAL VRT file from a set of input images by mosaicking
    the input images.

    :param input_imgs: A list of input images
    :param out_vrt_file: The output file location for the VRT.
    :param vrt_extent: An optional (If None then ignored) extent
                       (minX, minY, maxX, maxY) for the VRT image.
    :param vrt_out_res_x: An optional (If None then ignored) set defining the
                          x resolution of the output VRT.
    :param vrt_out_res_y: An optional (If None then ignored) set defining the
                          y resolution of the output VRT.
    :param interp_method: define the interpolation algorithm used when resampling
                          is required.
    :param align_out_pxls: align the output pixels to force output bounds to be
                           multiple of output resolution.

    """
    import osgeo.gdal as gdal

    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    # resolution – ‘highest’, ‘lowest’, ‘average’, ‘user’.
    res_source = "average"
    if (vrt_out_res_x is None) and (vrt_out_res_y is None):
        res_source = "average"
    elif (vrt_out_res_x is not None) and (vrt_out_res_y is not None):
        res_source = "user"
        if vrt_out_res_y < 0:
            vrt_out_res_y = vrt_out_res_y * (-1)
    else:
        raise rsgislib.RSGISPyException(
            "The X and Y resolution cannot be defined independently."
        )

    gdal_interp_method = rsgislib.get_gdal_interp_type(interp_method)

    build_vrt_opt = gdal.BuildVRTOptions(
        resolution=res_source,
        outputBounds=vrt_extent,
        xRes=vrt_out_res_x,
        yRes=vrt_out_res_y,
        targetAlignedPixels=align_out_pxls,
        resampleAlg=gdal_interp_method,
        callback=callback,
    )
    gdal.BuildVRT(out_vrt_file, input_imgs, options=build_vrt_opt)


def create_vrt_band_subset(input_img: str, img_bands: List[int], out_vrt_img: str):
    """
    A function which creates a GDAL VRT for the input image with the bands selected in
    the input list.

    :param input_img: the input GDAL image
    :param img_bands: a list of bands (in the order they will be in the VRT). Note,
                      band numbering starts at 1.
    :param out_vrt_img: the output VRT file.

    """
    input_img = os.path.abspath(input_img)
    vrt_options = gdal.BuildVRTOptions(bandList=img_bands)
    my_vrt = gdal.BuildVRT(out_vrt_img, [input_img], options=vrt_options)
    my_vrt = None


def subset_to_vec(
    input_img: str,
    output_img: str,
    gdalformat: str,
    roi_vec_file: str,
    roi_vec_lyr: str,
    datatype: int = None,
    vec_epsg: int = None,
    img_epsg: int = None,
):
    """
    A function which subsets an input image using the extent of a vector layer where
    the input vector can be a different projection to the input image. Reprojection
    will be handled.

    :param input_img: Input Image file.
    :param output_img: Output Image file.
    :param gdalformat: Output image file format.
    :param roi_vec_file: The input vector file.
    :param roi_vec_lyr: The name of the input layer.
    :param datatype: Output image data type. If None then the datatype of the input
                     image will be used.
    :param vec_epsg: If projection is poorly defined by the vector layer then it can
                     be specified.
    :param img_epsg: If projection is poorly defined by the image layer then it can
                     be specified.
    """
    import rsgislib
    import rsgislib.tools.geometrytools
    import rsgislib.vectorutils

    if vec_epsg is None:
        vec_epsg = rsgislib.vectorutils.get_proj_epsg_from_vec(
            roi_vec_file, roi_vec_lyr
        )
    if img_epsg is None:
        img_epsg = get_epsg_proj_from_img(input_img)

    if (img_epsg is None) or (vec_epsg is None):
        print("img_epsg: {}".format(img_epsg))
        print("vec_epsg: {}".format(vec_epsg))
        raise rsgislib.RSGISPyException("Either the image or vector EPSG is None!")

    vec_bbox = rsgislib.vectorutils.get_vec_layer_extent(
        roi_vec_file, roi_vec_lyr, compute_if_exp=True
    )
    img_bbox = get_img_bbox(input_img)

    if img_epsg != vec_epsg:
        vec_bbox = rsgislib.tools.geometrytools.reproj_bbox_epsg(
            vec_bbox, vec_epsg, img_epsg
        )

    if rsgislib.tools.geometrytools.do_bboxes_intersect(img_bbox, vec_bbox):
        common_bbox = rsgislib.tools.geometrytools.bbox_intersection(img_bbox, vec_bbox)
        if datatype is None:
            datatype = get_gdal_datatype_from_img(input_img)
        subset_bbox(
            input_img,
            output_img,
            gdalformat,
            datatype,
            common_bbox[0],
            common_bbox[1],
            common_bbox[2],
            common_bbox[3],
        )
    else:
        raise rsgislib.RSGISPyException(
            "The image and vector do not intersect and "
            "therefore the image cannot be subset."
        )


def subset_to_geoms_bbox(
    input_img: str,
    vec_file: str,
    vec_lyr: str,
    att_unq_val_col: str,
    out_img_base: str,
    gdalformat: str = "KEA",
    datatype: int = None,
    out_img_ext: str = "kea",
):
    """
    Subset an image to the bounding box of a each geometry in the input vector
    producing multiple output files. Useful for splitting an image into tiles
    of unequal sizes or extracting sampling plots from a larger image.

    Note, if a vector feature does not intersect with the input image then
    it will silently ignore the feature (i.e., not output image will be produced).

    :param input_img: The input image from which the subsets will be extracted.
    :param vec_file: input vector file/path
    :param vec_lyr: input vector layer name
    :param att_unq_val_col: column within the attribute table which has a value
                            to be included within the output file name so the
                            output files can be identified and have unique file
                            names.
    :param out_img_base: the output images base path and file name
    :param gdalformat: output image file format (default: KEA)
    :param datatype: output image data type. If None (default) then taken from
                     the input image.
    :param out_img_ext: output image file extension (e.g., kea)

    """
    import rsgislib.tools.geometrytools
    import rsgislib.vectorattrs
    import rsgislib.vectorgeoms

    if datatype is None:
        datatype = get_rsgislib_datatype_from_img(input_img)

    bboxs = rsgislib.vectorgeoms.get_geoms_as_bboxs(vec_file, vec_lyr)
    print(bboxs)
    print(
        "There are {} geometries for "
        "which subsets will be created".format(len(bboxs))
    )

    unq_bbox_ids = rsgislib.vectorattrs.read_vec_column(
        vec_file, vec_lyr, att_unq_val_col
    )

    in_img_bbox = get_img_bbox(input_img)

    for bbox_id, bbox in zip(unq_bbox_ids, bboxs):
        output_img = "{}{}.{}".format(out_img_base, bbox_id, out_img_ext)
        print(output_img)
        if rsgislib.tools.geometrytools.does_bbox_contain(in_img_bbox, bbox):
            subset_bbox(
                input_img,
                output_img,
                gdalformat,
                datatype,
                bbox[0],
                bbox[1],
                bbox[2],
                bbox[3],
            )
        elif rsgislib.tools.geometrytools.do_bboxes_intersect(in_img_bbox, bbox):
            inter_bbox = rsgislib.tools.geometrytools.bbox_intersection(
                in_img_bbox, bbox
            )
            subset_bbox(
                input_img,
                output_img,
                gdalformat,
                datatype,
                inter_bbox[0],
                inter_bbox[1],
                inter_bbox[2],
                inter_bbox[3],
            )


def mask_img_with_vec(
    input_img: str,
    output_img: str,
    gdalformat: str,
    roi_vec_file: str,
    roi_vec_lyr: str,
    tmp_dir: str,
    outvalue: float = 0,
    datatype: int = None,
    vec_epsg: int = None,
):
    """
    This function masks the input image using a polygon vector file.

    :param input_img: Input Image file.
    :param output_img: Output Image file.
    :param gdalformat: Output image file format.
    :param roi_vec_file: The input vector file.
    :param roi_vec_lyr: The name of the input layer.
    :param tmp_dir: a temporary directory for files generated during processing.
    :param outvalue: The output value in the regions masked.
    :param datatype: Output image data type. If None then the datatype of the
                     input image will be used.
    :param vec_epsg: If projection is poorly defined by the vector layer then it
                     can be specified.

    """
    import rsgislib
    import rsgislib.tools.filetools
    import rsgislib.tools.geometrytools
    import rsgislib.tools.utils
    import rsgislib.vectorutils
    import rsgislib.vectorutils.createrasters

    # Does the input image BBOX intersect the BBOX of the ROI vector?
    if vec_epsg is None:
        vec_epsg = rsgislib.vectorutils.get_proj_epsg_from_vec(
            roi_vec_file, roi_vec_lyr
        )
    img_epsg = get_epsg_proj_from_img(input_img)
    if img_epsg == vec_epsg:
        img_bbox = get_img_bbox(input_img)
        projs_match = True
    else:
        img_bbox = get_img_bbox_in_proj(input_img, vec_epsg)
        projs_match = False
    vec_bbox = rsgislib.vectorutils.get_vec_layer_extent(
        roi_vec_file, roi_vec_lyr, compute_if_exp=True
    )

    if rsgislib.tools.geometrytools.do_bboxes_intersect(img_bbox, vec_bbox):
        uid_str = rsgislib.tools.utils.uid_generator()
        base_vmsk_img = rsgislib.tools.filetools.get_file_basename(input_img)

        tmp_file_dir = os.path.join(tmp_dir, "{}_{}".format(base_vmsk_img, uid_str))
        if not os.path.exists(tmp_file_dir):
            os.mkdir(tmp_file_dir)

        # Rasterise the vector layer to the input image extent.
        mem_ds, mem_lyr = rsgislib.vectorutils.get_mem_vec_lyr_subset(
            roi_vec_file, roi_vec_lyr, img_bbox
        )

        if not projs_match:
            mem_result_ds, mem_result_lyr = rsgislib.vectorutils.reproj_vec_lyr_obj(
                mem_lyr,
                "mem_vec",
                img_epsg,
                out_format="MEMORY",
                out_vec_lyr=None,
                in_epsg=None,
                print_feedback=True,
            )
            mem_ds = None
        else:
            mem_result_ds = mem_ds
            mem_result_lyr = mem_lyr

        roi_img = os.path.join(tmp_file_dir, "{}_roiimg.kea".format(base_vmsk_img))
        create_copy_img(input_img, roi_img, 1, 0, "KEA", rsgislib.TYPE_8UINT)
        rsgislib.vectorutils.createrasters.rasterise_vec_lyr_obj(
            mem_result_lyr,
            roi_img,
            burn_val=1,
            att_column=None,
            calc_stats=True,
            thematic=True,
            no_data_val=0,
        )
        mem_result_ds = None

        if datatype is None:
            datatype = rsgislib.get_gdal_data_type_from_img(input_img)
        mask_img(input_img, roi_img, output_img, gdalformat, datatype, outvalue, 0)
        shutil.rmtree(tmp_file_dir)
    else:
        raise rsgislib.RSGISPyException(
            "The vector file and image file do not intersect."
        )


def create_valid_mask(
    img_band_info: list, out_msk_file: str, gdalformat: str, tmp_dir: str
):
    """
    A function to create a single valid mask from the intersection of the valid masks
    for all the input images.

    :param img_band_info: A list of rsgislib.imageutils.ImageBandInfo objects to
                          define the images and bands of interest.
    :param out_msk_file: A output image file and path
    :param gdalformat: The output file format.
    :param tmp_dir: A directory for temporary outputs created during the processing.

    """
    import rsgislib.tools.filetools
    import rsgislib.tools.utils

    if len(img_band_info) == 1:
        no_data_val = get_img_no_data_value(img_band_info[0].file_name)
        gen_valid_mask(
            img_band_info[0].file_name, out_msk_file, gdalformat, no_data_val
        )
    else:
        uid_str = rsgislib.tools.utils.uid_generator()
        tmp_lcl_dir = os.path.join(tmp_dir, "create_valid_mask_{}".format(uid_str))
        if not os.path.exists(tmp_lcl_dir):
            os.makedirs(tmp_lcl_dir)

        validMasks = []
        for imgInfo in img_band_info:
            tmpBaseName = rsgislib.tools.filetools.get_file_basename(imgInfo.file_name)
            vdmskFile = os.path.join(tmp_lcl_dir, "{}_vmsk.kea".format(tmpBaseName))
            no_data_val = get_img_no_data_value(imgInfo.file_name)
            gen_valid_mask(
                imgInfo.file_name, vdmskFile, gdalformat="KEA", no_data_val=no_data_val
            )
            validMasks.append(vdmskFile)

        gen_valid_mask(validMasks, out_msk_file, gdalformat, no_data_val=0.0)
        shutil.rmtree(tmp_lcl_dir)


def get_img_pxl_values(
    input_img: str, img_band: int, x_coords: numpy.array, y_coords: numpy.array
) -> numpy.array:
    """
    Function which gets pixel values from a image for specified
    image pixels. The coordinate space is image pixels, i.e.,
    (0 - xSize) and (0 - ySize).

    :param input_img: The input image name and path
    :param img_band: The band within the input image.
    :param x_coords: A numpy array of image X coordinates (in the image pixel
                     coordinates)
    :param y_coords: A numpy array of image Y coordinates (in the image pixel
                     coordinates)
    :return: An array of image pixel values.

    """
    import tqdm

    if x_coords.shape[0] != y_coords.shape[0]:
        raise rsgislib.RSGISPyException(
            "The X and Y image coordinates are not the same."
        )

    image_ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if image_ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the input image file: '{}'".format(input_img)
        )
    image_band = image_ds.GetRasterBand(img_band)
    if image_band is None:
        raise rsgislib.RSGISPyException("The image band wasn't opened")

    out_pxl_vals = numpy.zeros(x_coords.shape[0], dtype=float)

    img_data = image_band.ReadAsArray()
    for i in tqdm.tqdm(range(x_coords.shape[0])):
        out_pxl_vals[i] = img_data[y_coords[i], x_coords[i]]
    image_ds = None
    return out_pxl_vals


def set_img_pxl_values(
    input_img: str,
    img_band: int,
    x_coords: numpy.array,
    y_coords: numpy.array,
    pxl_value: float = 1,
):
    """
    A function which sets defined image pixels to a value.
    The coordinate space is image pixels, i.e.,
    (0 - xSize) and (0 - ySize).

    :param input_img: The input image name and path
    :param img_band: The band within the input image.
    :param x_coords: A numpy array of image X coordinates (in the image pixel
                     coordinates)
    :param y_coords: A numpy array of image Y coordinates (in the image pixel
                     coordinates)
    :param pxl_value: The value to set the image pixel to (specified by the x/y
                      coordinates)

    """
    import tqdm

    if x_coords.shape[0] != y_coords.shape[0]:
        raise rsgislib.RSGISPyException(
            "The X and Y image coordinates are not the same."
        )

    image_ds = gdal.Open(input_img, gdal.GA_Update)
    if image_ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the input image file: '{}'".format(input_img)
        )
    image_band = image_ds.GetRasterBand(img_band)
    if image_band is None:
        raise rsgislib.RSGISPyException("The image band wasn't opened")

    img_data = image_band.ReadAsArray()
    for i in tqdm.tqdm(range(x_coords.shape[0])):
        img_data[y_coords[i], x_coords[i]] = pxl_value
    image_band.WriteArray(img_data)
    image_ds = None


def get_img_pxl_column(
    input_img: str, x_pxl_coord: int, y_pxl_coord: int
) -> numpy.array:
    """
    Function which gets pixel band values for a single pixel within an image.
    The coordinate space is image pixels, i.e., (0 - xSize) and (0 - ySize).

    :param input_img: The input image name and path
    :param x_pxl_coord: An image X coordinate (in the image pixel coordinates)
    :param y_pxl_coord: An image Y coordinate (in the image pixel coordinates)
    :return: An array of image pixel values (length = the number of image bands).

    """
    import struct

    image_ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if image_ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the input image file: '{}'".format(input_img)
        )
    n_bands = image_ds.RasterCount
    out_pxl_vals_byte = image_ds.ReadRaster(
        xoff=int(x_pxl_coord),
        yoff=int(y_pxl_coord),
        xsize=1,
        ysize=1,
        band_list=None,
        buf_type=gdal.GDT_Float32,
    )
    out_pxl_vals = struct.unpack("f" * n_bands, out_pxl_vals_byte)
    image_ds = None
    return numpy.array(out_pxl_vals)


def assign_random_pxls(
    input_img: str,
    output_img: str,
    n_pts: int,
    img_band: int = 1,
    gdalformat: str = "KEA",
    edge_pxl: int = 0,
    use_no_data: bool = True,
    rnd_seed: int = None,
):
    """
    A function which can generate a set of random pixels. Can honor the image no
    data value and use an edge buffer so pixels are not identified near the image edge.

    :param input_img: The input image providing the reference area and no data value.
    :param output_img: The output image with the random pixels.
    :param n_pts: The number of pixels to be sampled.
    :param img_band: The image band from the input image used for the no data value.
    :param gdalformat: The file format of the output image.
    :param edge_pxl: The edge pixel buffer, in pixels. This is a buffer around the
                     edge of the image within which pixels will not be identified.
                     (Default: 0)
    :param use_no_data: A boolean specifying whether the image no data value should
                        be used. (Default: True)
    :param rnd_seed: A random seed for generating the pixel locations. If None then a
                 different seed is used each time the system is executed (Default None).

    .. code:: python

        input_img = 'LS5TM_20000108_latn531lonw37_r23p204_osgb_clouds_up.kea'
        output_img = 'LS5TM_20000108_latn531lonw37_r23p204_osgb_samples.kea'
        n_pts = 5000

        assign_random_pxls(input_img, output_img, n_pts, img_band=1, gdalformat='KEA')
        # Calculate the image stats and pyramids for display
        import rsgislib.rastergis
        rsgislib.rastergis.pop_rat_img_stats(output_img, True, True, True)

    """
    import numpy.random

    if rnd_seed is not None:
        numpy.random.seed(rnd_seed)

    if edge_pxl < 0:
        raise rsgislib.RSGISPyException("edge_pxl value must be greater than 0.")

    xSize, ySize = get_img_size(input_img)

    x_min = edge_pxl
    x_max = xSize - edge_pxl

    y_min = edge_pxl
    y_max = ySize - edge_pxl

    if use_no_data:
        no_data_val = get_img_no_data_value(input_img, img_band)

        out_x_coords = numpy.zeros(n_pts, dtype=numpy.uint16)
        out_y_coords = numpy.zeros(n_pts, dtype=numpy.uint16)

        out_n_pts = 0
        pts_size = n_pts
        while out_n_pts < n_pts:
            x_coords = numpy.random.randint(
                x_min, high=x_max, size=pts_size, dtype=numpy.uint16
            )
            y_coords = numpy.random.randint(
                y_min, high=y_max, size=pts_size, dtype=numpy.uint16
            )
            pxl_vals = get_img_pxl_values(input_img, img_band, x_coords, y_coords)

            for i in range(pts_size):
                if pxl_vals[i] != no_data_val:
                    out_x_coords[out_n_pts] = x_coords[i]
                    out_y_coords[out_n_pts] = y_coords[i]
                    out_n_pts += 1
            pts_size = n_pts - out_n_pts
    else:
        out_x_coords = numpy.random.randint(
            x_min, high=x_max, size=n_pts, dtype=numpy.uint16
        )
        out_y_coords = numpy.random.randint(
            y_min, high=y_max, size=n_pts, dtype=numpy.uint16
        )

    create_copy_img(input_img, output_img, 1, 0, gdalformat, rsgislib.TYPE_8UINT)
    set_img_pxl_values(output_img, 1, out_x_coords, out_y_coords, 1)


def check_img_lst(
    input_imgs: list,
    exp_x_res: float,
    exp_y_res: float,
    bbox: list = None,
    print_errors: bool = True,
    abs_res: bool = True,
):
    """
    A function which checks a list of images to ensure they resolution and optionally
    the bounding box is as expected.

    :param input_imgs: a list of input images
    :param exp_x_res: the expected image resolution in the x-axis
    :param exp_y_res: the expected image resolution in the y-axis
    :param bbox: a bbox (MinX, MaxX, MinY, MaxY) where intersection will be tested.
                 Default None and ignored.
    :param print_errors: if True then images with errors will be printed to the
                         console. Default: True
    :return: a list of images which have passed resolution and optional bbox
             intersection test.

    """
    import rsgislib.tools.geometrytools

    if abs_res:
        exp_x_res = abs(exp_x_res)
        exp_y_res = abs(exp_y_res)
    out_imgs = list()
    for img in input_imgs:
        img_res = get_img_res(img, abs_vals=abs_res)
        if bbox is not None:
            img_bbox = get_img_bbox(img)
        if (img_res[0] != exp_x_res) or (img_res[1] != exp_y_res):
            if print_errors:
                print("{} has resolution: {}".format(img, img_res))
        elif (bbox is not None) and (
            not rsgislib.tools.geometrytools.bbox_intersection(bbox, img_bbox)
        ):
            if print_errors:
                print("{} has BBOX: {}".format(img, img_bbox))
        else:
            out_imgs.append(img)
    return out_imgs


def check_img_file_comparison(
    in_base_img: str,
    in_comp_img: str,
    test_n_bands: bool = False,
    test_eql_bbox: bool = False,
    print_errors: bool = True,
):
    """
    A function which tests whether an image is comparable:
     * Image resolution
     * Intersecting bounding box
     * Optionally the number of bands
     * Optionally whether the BBOXs match rather than intersect

    :param in_base_img: base input image which will be compared to
    :param in_comp_img: the input image which will be compared to the base.
    :param test_n_bands: if true the number of image bands will be checked
                         (i.e., the same)
    :parma test_eql_bbox: if true then the bboxes will need to be identical
                          between the images.
    :param print_errors: if True then images with errors will be printed to the
                         console. Default: True
    :return: Boolean (True; images are compariable)

    """
    import rsgislib.tools.geometrytools

    imgs_match = True

    if not do_img_res_match(in_base_img, in_comp_img):
        if print_errors:
            base_img_res = get_img_res(in_base_img)
            comp_img_res = get_img_res(in_comp_img)
            print("Base Image Res: {}".format(base_img_res))
            print("Comp Image Res: {}".format(comp_img_res))
        imgs_match = False

    base_img_bbox = get_img_bbox(in_base_img)
    comp_img_bbox = get_img_bbox(in_comp_img)
    if not rsgislib.tools.geometrytools.bbox_intersection(base_img_bbox, comp_img_bbox):
        if print_errors:
            print("Base Image BBOX: {}".format(base_img_bbox))
            print("Comp Image BBOX: {}".format(comp_img_bbox))
        imgs_match = False

    if test_eql_bbox:
        if not rsgislib.tools.geometrytools.bbox_equal(base_img_bbox, comp_img_bbox):
            if print_errors:
                print("Base Image BBOX: {}".format(base_img_bbox))
                print("Comp Image BBOX: {}".format(comp_img_bbox))
            imgs_match = False

    if test_n_bands:
        base_img_nbands = get_img_band_count(in_base_img)
        comp_img_nbands = get_img_band_count(in_comp_img)
        if base_img_nbands != comp_img_nbands:
            if print_errors:
                print("Base Image n-bands: {}".format(base_img_nbands))
                print("Comp Image n-bands: {}".format(comp_img_nbands))
            imgs_match = False

    return imgs_match


def test_img_lst_intersects(input_imgs: list, stop_err: bool = False):
    """
    A function which will test a list of image to check if they intersect,
    have matching image resolution and projection. The first image in the list is
    used as the reference to which all others are compared to.

    :param input_imgs: list of images file paths
    :param stop_err: boolean. Default: False.
                     If True then an exception will be thrown if error found.
                     If False then errors will just be printed to screen.

    """
    import rsgislib.tools.geometrytools

    first = True
    for img in input_imgs:
        print(img)
        img_bbox = get_img_bbox(img)
        img_proj = get_epsg_proj_from_img(img)
        img_res = get_img_res(img)
        if first:
            first = False
            ref_img = img
            ref_bbox = img_bbox
            ref_proj = img_proj
            ref_res = img_res
            print("\tReference Image")
        else:
            if ref_proj != img_proj:
                print("\tProjection does not match the reference (i.e., first image)")
                print("\tRef (first) Image: {}".format(ref_img))
                print("\tRef (first) EPSG: {}".format(ref_proj))
                print("\tImage: {}".format(img))
                print("\tImage EPSG: {}".format(img_proj))
                if stop_err:
                    raise rsgislib.RSGISPyException(
                        "Projection does not match the reference (i.e., first image)"
                    )
            elif not rsgislib.tools.geometrytools.do_bboxes_intersect(
                ref_bbox, img_bbox
            ):
                print("\tBBOX does not intersect the reference (i.e., first image)")
                print("\tRef (first) Image: {}".format(ref_img))
                print("\tRef (first) BBOX:", ref_bbox)
                print("\tImage: {}".format(img))
                print("\tImage BBOX: ", img_bbox)
                if stop_err:
                    raise rsgislib.RSGISPyException(
                        "BBOX does not intersect the reference (i.e., first image)"
                    )
            elif (img_res[0] != ref_res[0]) or (img_res[1] != ref_res[1]):
                print(
                    "\tImage resolution does not match the reference "
                    "(i.e., first image)"
                )
                print("\tRef (first) Image: {}".format(ref_img))
                print("\tRef (first) Res: ", ref_res)
                print("\tImage: {}".format(img))
                print("\tImage Res: ", img_res)
                if stop_err:
                    raise rsgislib.RSGISPyException(
                        "Image resolution does not match the reference "
                        "(i.e., first image)"
                    )
            else:
                print("\tOK")


def whiten_image(
    input_img: str,
    valid_msk_img: str,
    valid_msk_val: int,
    output_img: str,
    gdalformat: str,
):
    """
    A function which whitens the input image where the noise covariance matrix is
    used to decorrelate and rescale the noise in the data (noise whitening).
    This results in a transformed datset in which the noise has unit variance
    and no band-to-band correlations. The valid mask is used to identify the
    areas of valid data. This function is used to an MNF transformation.

    WARNING: This function loads the whole image into memory and therefore
             can use a lot of memory for the analysis.

    :param input_img: the input image
    :param valid_msk_img: a valid input image mask
    :param valid_msk_val: the pixel value in the mask image specifying valid
                          image pixels.
    :param output_img: the output image file name and path (will be same dimensions
                       as the input)
    :param gdalformat: the GDAL image file format of the output image file.

    """
    import tqdm

    def _cov(M):
        """
        Compute the sample covariance matrix of a 2D matrix.

        Parameters:
          M: `numpy array`
            2d matrix of HSI data (N x p)

        Returns: `numpy array`
            sample covariance matrix
        """
        N = M.shape[0]
        u = M.mean(axis=0)
        M = M - numpy.kron(numpy.ones((N, 1)), u)
        C = numpy.dot(M.T, M) / (N - 1)
        return C

    def _whiten(M):
        """
        Whitens a HSI cube. Use the noise covariance matrix to decorrelate
        and rescale the noise in the data (noise whitening).
        Results in transformed data in which the noise has unit variance
        and no band-to-band correlations.

        Parameters:
            M: `numpy array`
                2d matrix of HSI data (N x p).

        Returns: `numpy array`
            Whitened HSI data (N x p).

        Reference:
            Krizhevsky, Alex, Learning Multiple Layers of Features from
            Tiny Images, MSc thesis, University of Toronto, 2009.
            See Appendix A.
        """
        sigma = _cov(M)
        U, S, V = numpy.linalg.svd(sigma)
        S_1_2 = S ** (-0.5)
        S = numpy.diag(S_1_2.T)
        Aw = numpy.dot(V, numpy.dot(S, V.T))
        return numpy.dot(M, Aw)

    img_msk_ds = gdal.Open(valid_msk_img)
    if img_msk_ds is None:
        raise rsgislib.RSGISPyException("Could not open valid mask image")
    n_msk_bands = img_msk_ds.RasterCount
    x_msk_size = img_msk_ds.RasterXSize
    y_msk_size = img_msk_ds.RasterYSize

    if n_msk_bands != 1:
        raise rsgislib.RSGISPyException(
            "Valid mask only expected to have a single band."
        )

    img_msk_band = img_msk_ds.GetRasterBand(1)
    if img_msk_band is None:
        raise rsgislib.RSGISPyException("Could not open image band (1) in valid mask")

    vld_msk_band_arr = img_msk_band.ReadAsArray().flatten()
    img_msk_ds = None

    img_ds = gdal.Open(input_img)
    if img_ds is None:
        raise rsgislib.RSGISPyException("Could not open input image")
    n_bands = img_ds.RasterCount
    x_size = img_ds.RasterXSize
    y_size = img_ds.RasterYSize

    if x_msk_size != x_size:
        raise rsgislib.RSGISPyException(
            "Mask and input image size in the x axis do not match."
        )

    if y_msk_size != y_size:
        raise rsgislib.RSGISPyException(
            "Mask and input image size in the y axis do not match."
        )

    img_data = numpy.zeros((n_bands, (x_size * y_size)), dtype=numpy.float32)

    print("Importing Bands:")
    for n in tqdm.tqdm(range(n_bands)):
        img_band = img_ds.GetRasterBand(n + 1)
        if img_band is None:
            raise rsgislib.RSGISPyException(
                "Could not open image band ({})".format(n + 1)
            )
        no_data_val = img_band.GetNoDataValue()
        band_arr = img_band.ReadAsArray().flatten()
        band_arr = band_arr.astype(numpy.float32)
        img_data[n] = band_arr
    img_ds = None
    band_arr = None

    img_data = img_data.T
    pxl_idxs = numpy.arange(vld_msk_band_arr.shape[0])
    pxl_idxs = pxl_idxs[vld_msk_band_arr == valid_msk_val]
    img_data = img_data[vld_msk_band_arr == valid_msk_val]

    img_flat_white = _whiten(img_data)

    print("Create empty output image file")
    create_copy_img(
        input_img, output_img, n_bands, 0, gdalformat, rsgislib.TYPE_32FLOAT
    )

    # Open output image
    out_img_ds = gdal.Open(output_img, gdal.GA_Update)
    if out_img_ds is None:
        raise rsgislib.RSGISPyException("Could not open output image")

    out_data_band = numpy.zeros_like(vld_msk_band_arr, dtype=numpy.float32)

    print("Output Bands:")
    for n in tqdm.tqdm(range(n_bands)):
        out_data_band[...] = 0.0
        out_data_band[pxl_idxs] = img_flat_white[..., n]
        out_img_data = out_data_band.reshape((y_size, x_size))
        out_img_band = out_img_ds.GetRasterBand(n + 1)
        if out_img_band is None:
            raise rsgislib.RSGISPyException("Could not open output image band (1)")
        out_img_band.WriteArray(out_img_data)
        out_img_band = None
    out_img_ds = None


def spectral_smoothing(
    input_img: str,
    valid_msk_img: str,
    valid_msk_val: int,
    output_img: str,
    win_len: int = 5,
    polyorder: int = 3,
    gdalformat: str = "KEA",
    datatype: int = rsgislib.TYPE_32FLOAT,
    calc_stats: bool = True,
):
    """
    This function performs spectral smoothing using a Savitzky-Golay filter.
    Typically applied to hyperspectral data to remove noise.

    :param input_img: input image file.
    :param valid_msk_img: an image file representing the valid data region
    :param valid_msk_val: image pixel value in the mask for the valid data region
    :param output_img: the output image file
    :param win_len: the window length for the Savitzky-Golay filter (Default: 5)
    :param polyorder: the order of the polynomial for the Savitzky-Golay filter
                      (Default: 3)
    :param gdalformat: the output file format. (Default: KEA)
    :param datatype: the output image datatype (Default: Float 32)
    :param calc_stats: Boolean specifying whether to calculate pyramids and
                       metadata stats (Default: True)

    """
    import scipy.signal
    from rios import applier

    try:
        import tqdm

        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    np_dtype = rsgislib.get_numpy_datatype(datatype)
    in_no_date = get_img_no_data_value(input_img)

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    infiles.valid_msk = valid_msk_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.valid_msk_val = valid_msk_val
    otherargs.win_len = win_len
    otherargs.polyorder = polyorder
    otherargs.in_no_date = in_no_date
    otherargs.np_dtype = np_dtype
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applySmoothing(info, inputs, outputs, otherargs):
        if numpy.any(inputs.valid_msk == otherargs.valid_msk_val):
            img_flat = numpy.moveaxis(inputs.image, 0, 2).reshape(
                -1, inputs.image.shape[0]
            )

            ID = numpy.arange(img_flat.shape[0])
            n_feats = ID.shape[0]

            ID = ID[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            img_flat = img_flat[inputs.valid_msk.flatten() == otherargs.valid_msk_val]

            img_flat_smooth = scipy.signal.savgol_filter(
                img_flat, otherargs.win_len, otherargs.polyorder, axis=1
            )

            img_flat_smooth_arr = numpy.zeros(
                [n_feats, inputs.image.shape[0]], dtype=otherargs.np_dtype
            )
            img_flat_smooth_arr[...] = in_no_date
            img_flat_smooth_arr[ID] = img_flat_smooth

            out_arr = img_flat_smooth_arr.reshape(
                inputs.image.shape[1], inputs.image.shape[2], inputs.image.shape[0]
            )
            out_arr = numpy.moveaxis(out_arr, 2, 0)
            outputs.outimage = out_arr
        else:
            outputs.outimage = numpy.zeros_like(inputs.image, dtype=otherargs.np_dtype)

    applier.apply(_applySmoothing, infiles, outfiles, otherargs, controls=aControls)

    if calc_stats:
        use_no_data = True
        if in_no_date is None:
            use_no_data = False
            in_no_date = 0.0
        pop_img_stats(
            output_img,
            use_no_data=use_no_data,
            no_data_val=in_no_date,
            calc_pyramids=True,
        )


def calc_wsg84_pixel_size(input_img: str, output_img: str, gdalformat: str = "KEA"):
    """
    A function which calculates the x and y pixel resolution (in metres) of each
    pixel projected in WGS84.

    :param input_img: input image, for which the per-pixel area will be calculated.
    :param output_img: output image file where band 1 is X and band 2 is the Y
                       pixel resolution.
    :param gdalformat: the output image file format (default: KEA).

    """
    from rios import applier

    import rsgislib.tools.projection

    try:
        import tqdm

        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    x_res, y_res = get_img_res(input_img, abs_vals=True)

    infiles = applier.FilenameAssociations()
    infiles.img = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.x_res = x_res
    otherargs.y_res = y_res
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = False
    aControls.calcStats = False

    def _calcPixelRes(info, inputs, outputs, otherargs):
        xBlock, yBlock = info.getBlockCoordArrays()

        x_res_arr = numpy.zeros_like(yBlock, dtype=float)
        x_res_arr[...] = otherargs.x_res
        y_res_arr = numpy.zeros_like(yBlock, dtype=float)
        y_res_arr[...] = otherargs.y_res
        x_res_arr_m, y_res_arr_m = rsgislib.tools.projection.degrees_to_metres(
            yBlock, x_res_arr, y_res_arr
        )
        outputs.outimage = numpy.stack((x_res_arr_m, y_res_arr_m), axis=0)

    applier.apply(_calcPixelRes, infiles, outfiles, otherargs, controls=aControls)


def mask_all_band_zero_vals(
    input_img: str, output_img: str, gdalformat: str, out_val: int = 1
):
    """
    Function which identifies image pixels which have a value of zero
    all bands which are defined as true 'no data' regions while other
    pixels have a value of zero or less then zero for one or few pixels
    which causes confusion between valid data pixel and no data pixels.
    This function will identify and define those pixels which are valid
    but with a value <= 0 for isolate bands to a new output value (out_val).

    This function might be used for surface reflectance data where the
    atmospheric correction has resulted in value <=0 which isn't normally
    possible and where 0 is commonly used as a no data value. In this case
    setting those pixel band values to 1 (if data has been multiplied by
    100, 1000, or 10000, for example) or a small fraction (e.g., 0.001) if
    values are between 0-1.

    :param input_img: the input image
    :param output_img: the output image file name and path
    :param gdalformat: the GDAL image file format of the output image file.
    :param out_val: Output pixel band value (default: 1)

    """
    from rios import applier

    try:
        import tqdm

        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.out_val = out_val
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyzeronodata(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        img_sum = numpy.sum(inputs.image, axis=0)
        vld_msk = img_sum > 0
        outputs.outimage = inputs.image
        outputs.outimage[(inputs.image <= 0) & vld_msk] = otherargs.out_val

    applier.apply(_applyzeronodata, infiles, outfiles, otherargs, controls=aControls)


def create_raster_tiles_bbox(
    bbox: List[float],
    tile_x_size: float,
    tile_y_size: float,
    out_img_res: float,
    out_epsg: int,
    out_tile_dir: str,
    out_tile_name: str = "base_tile_{}",
    out_img_ext: str = "kea",
    gdalformat: str = "KEA",
    datatype: int = rsgislib.TYPE_8UINT,
    out_img_pxl_val: float = 0,
    out_img_n_bands: int = 1,
):
    """
    A function which creates a set of raster tiles for a region defined by a
    bbox.

    :param bbox: the bounding box (xMin, xMax, yMin, yMax) for the whole
                 region for which the tiles are generated.
    :param tile_x_size: the size of the tiles in x axis in the units of the
                        projection (e.g., metres or degrees)
    :param tile_y_size: the size of the tiles in y axis in the units of the
                        projection (e.g., metres or degrees)
    :param out_img_res: the output image tile pixel resolution
    :param out_epsg: the output projection (which needs to match the bbox).
    :param out_tile_dir: the output directory path.
    :param out_tile_name: the base name for the output image tiles - note must
                          include {} which will be replace with the tile x and y
                          numbering. (Default: "base_tile_{}")
    :param out_img_ext: the output image file extension (Default: kea)
    :param gdalformat: the output gdal image format (Default: KEA)
    :param datatype: the output image data type (Default: rsgislib.TYPE_8UINT)
    :param out_img_pxl_val: the output image pixel values (Default: 0)
    :param out_img_n_bands: the number of output image bands (Default: 1)

    """
    import rsgislib.tools.projection

    bbox_width = bbox[1] - bbox[0]
    bbox_height = bbox[3] - bbox[2]

    if bbox_width < 0:
        raise rsgislib.RSGISPyException("The width of the bbox is negative")

    if bbox_height < 0:
        raise rsgislib.RSGISPyException("The height of the bbox is negative")

    print(f"BBOX DIMS: {bbox_width} x {bbox_height}")

    x_n_tiles = int(math.ceil(bbox_width / tile_x_size))
    y_n_tiles = int(math.ceil(bbox_height / tile_y_size))
    tot_n_tiles = x_n_tiles * y_n_tiles
    print(f"Tiles: {x_n_tiles} x {y_n_tiles} = {tot_n_tiles}")

    wkt_str = rsgislib.tools.projection.get_wkt_from_epsg_code(out_epsg)

    n_tile = 0
    for y in range(y_n_tiles):
        y_top = bbox[3] - (tile_y_size * y)
        y_bot = y_top - tile_y_size
        for x in range(x_n_tiles):
            x_lft = bbox[0] + (tile_x_size * x)
            x_rht = x_lft + tile_x_size

            tile_bbox = [x_lft, x_rht, y_bot, y_top]
            print("{} of {}: {}".format(n_tile, tot_n_tiles, tile_bbox))
            tile_unq_name = f"{x}_{y}"
            out_tile_file_name = out_tile_name.format(tile_unq_name)
            out_tile_img = os.path.join(
                out_tile_dir, f"{out_tile_file_name}.{out_img_ext}"
            )

            create_blank_img_from_bbox(
                tile_bbox,
                wkt_str=wkt_str,
                output_img=out_tile_img,
                out_img_res=out_img_res,
                out_img_pxl_val=out_img_pxl_val,
                out_img_n_bands=out_img_n_bands,
                gdalformat=gdalformat,
                datatype=datatype,
                snap_to_grid=False,
            )
            n_tile += 1


def mask_outliners_data_values(
    input_img: str,
    valid_msk_img: str,
    valid_msk_val: int,
    output_img: str,
    lower: int = 5,
    upper: int = 95,
    gdalformat: str = "KEA",
    calc_stats: bool = True,
):
    """
    This function masks outliners (assigning them to nan; output pixel type must
    therefore be Float32). Outliners are defined using an upper and lower percentile.


    :param input_img: input image file.
    :param valid_msk_img: an image file representing the valid data region
    :param valid_msk_val: image pixel value in the mask for the valid data region
    :param output_img: the output image file
    :param lower: the lower percentile threshold (value: 0-100)
    :param upper: the upper percentile threshold (value: 0-100)
    :param gdalformat: the output file format. (Default: KEA)
    :param calc_stats: Boolean specifying whether to calculate pyramids and
                       metadata stats (Default: True)

    """
    from rios import applier

    try:
        import tqdm

        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    np_dtype = rsgislib.get_numpy_datatype(rsgislib.TYPE_32FLOAT)
    in_no_date = get_img_no_data_value(input_img)

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    infiles.valid_msk = valid_msk_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.valid_msk_val = valid_msk_val
    otherargs.lower = lower
    otherargs.upper = upper
    otherargs.in_no_date = in_no_date
    otherargs.np_dtype = np_dtype
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyFill(info, inputs, outputs, otherargs):
        if numpy.any(inputs.valid_msk == otherargs.valid_msk_val):
            img_flat = numpy.moveaxis(inputs.image, 0, 2).reshape(
                -1, inputs.image.shape[0]
            )

            ID = numpy.arange(img_flat.shape[0])
            n_feats = ID.shape[0]

            ID = ID[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            img_flat = img_flat[inputs.valid_msk.flatten() == otherargs.valid_msk_val]

            img_flat_out_arr = numpy.zeros(
                [n_feats, inputs.image.shape[0]], dtype=otherargs.np_dtype
            )
            img_flat_out_arr[...] = in_no_date
            for pxl_idx in range(img_flat.shape[0]):
                percentiles = numpy.nanpercentile(
                    img_flat[pxl_idx], [otherargs.lower, otherargs.upper]
                )
                img_flat[pxl_idx][img_flat[pxl_idx] < percentiles[0]] = numpy.nan
                img_flat[pxl_idx][img_flat[pxl_idx] > percentiles[1]] = numpy.nan
                img_flat_out_arr[ID[pxl_idx]] = img_flat[pxl_idx]

            out_arr = img_flat_out_arr.reshape(
                inputs.image.shape[1], inputs.image.shape[2], inputs.image.shape[0]
            )
            out_arr = numpy.moveaxis(out_arr, 2, 0)
            outputs.outimage = out_arr
        else:
            outputs.outimage = numpy.zeros_like(inputs.image, dtype=otherargs.np_dtype)

    applier.apply(_applyFill, infiles, outfiles, otherargs, controls=aControls)

    if calc_stats:
        use_no_data = True
        if in_no_date is None:
            use_no_data = False
            in_no_date = 0.0
        pop_img_stats(
            output_img,
            use_no_data=use_no_data,
            no_data_val=in_no_date,
            calc_pyramids=True,
        )


def polyfill_nan_data_values(
    input_img: str,
    band_vals: List[float],
    valid_msk_img: str,
    valid_msk_val: int,
    output_img: str,
    polyorder: int = 3,
    mean_abs_diff: float = None,
    gdalformat: str = "KEA",
    calc_stats: bool = True,
):
    """
    This function fills missing data along (defined as nan values) the y-axis
    (i.e., along the bands).


    :param input_img: input image file.
    :param valid_msk_img: an image file representing the valid data region
    :param valid_msk_val: image pixel value in the mask for the valid data region
    :param output_img: the output image file
    :param polyorder: the order of the polynomial (Default: 3)
    :param mean_abs_diff: Optional parameter to calculate the absolute mean difference
                        between the predict values have the mean of the original
                        pixel values. This threshold can be used to replace any
                        predicted values which are > than the thresholds specified
                        with the mean.
    :param gdalformat: the output file format. (Default: KEA)
    :param calc_stats: Boolean specifying whether to calculate pyramids and
                       metadata stats (Default: True)

    """
    from rios import applier

    try:
        import tqdm

        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    np_dtype = rsgislib.get_numpy_datatype(rsgislib.TYPE_32FLOAT)
    in_no_date = get_img_no_data_value(input_img)

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    infiles.valid_msk = valid_msk_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.valid_msk_val = valid_msk_val
    otherargs.polyorder = polyorder
    otherargs.mean_abs_diff = mean_abs_diff
    otherargs.band_vals = numpy.array(band_vals)
    otherargs.in_no_date = in_no_date
    otherargs.np_dtype = np_dtype
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyFill(info, inputs, outputs, otherargs):
        if numpy.any(inputs.valid_msk == otherargs.valid_msk_val):
            img_flat = numpy.moveaxis(inputs.image, 0, 2).reshape(
                -1, inputs.image.shape[0]
            )

            ID = numpy.arange(img_flat.shape[0])
            n_feats = ID.shape[0]

            ID = ID[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            img_flat = img_flat[inputs.valid_msk.flatten() == otherargs.valid_msk_val]

            img_flat_fill_arr = numpy.zeros(
                [n_feats, inputs.image.shape[0]], dtype=otherargs.np_dtype
            )
            img_flat_fill_arr[...] = in_no_date
            for pxl_idx in range(img_flat.shape[0]):
                if numpy.isnan(img_flat[pxl_idx]).any():
                    finite_msk = numpy.isfinite(img_flat[pxl_idx])
                    if numpy.sum(finite_msk) > otherargs.polyorder:
                        x_vals = otherargs.band_vals[finite_msk]
                        y_vals = img_flat[pxl_idx][finite_msk]
                        poly_coefs = numpy.polyfit(
                            x_vals, y_vals, deg=otherargs.polyorder
                        )
                        pred_vals = numpy.polyval(poly_coefs, otherargs.band_vals)
                        if otherargs.mean_abs_diff is not None:
                            pxl_mean = numpy.nanmean(img_flat[pxl_idx])
                            pred_vals_diff = numpy.abs(pred_vals - pxl_mean)
                            pred_vals[
                                pred_vals_diff > otherargs.mean_abs_diff
                            ] = pxl_mean
                        repl_idxs = numpy.arange(0, pred_vals.shape[0])[
                            numpy.invert(finite_msk)
                        ]
                        img_flat[pxl_idx][repl_idxs] = pred_vals[repl_idxs]
                        img_flat_fill_arr[ID[pxl_idx]] = pred_vals
                    else:
                        img_flat_fill_arr[ID[pxl_idx]][...] = numpy.nan
                else:
                    img_flat_fill_arr[ID[pxl_idx]] = img_flat[pxl_idx]

            out_arr = img_flat_fill_arr.reshape(
                inputs.image.shape[1], inputs.image.shape[2], inputs.image.shape[0]
            )
            out_arr = numpy.moveaxis(out_arr, 2, 0)
            outputs.outimage = out_arr
        else:
            outputs.outimage = numpy.zeros_like(inputs.image, dtype=otherargs.np_dtype)

    applier.apply(_applyFill, infiles, outfiles, otherargs, controls=aControls)

    if calc_stats:
        use_no_data = True
        if in_no_date is None:
            use_no_data = False
            in_no_date = 0.0
        pop_img_stats(
            output_img,
            use_no_data=use_no_data,
            no_data_val=in_no_date,
            calc_pyramids=True,
        )


def grid_scattered_pts(
    vec_file: str,
    vec_lyr: str,
    vec_col: str,
    input_img: str,
    output_img: str,
    gdal_grid_alg: str = None,
    no_data_val: float = 0.0,
    gdalformat: str = "KEA",
    datatype: int = rsgislib.TYPE_32FLOAT,
):
    """
    A function which uses the GDAL grid function to create a regular grid
    (i.e., raster) from an irregular grid of points within a raster layer.

    See gdal_grid documentation for the algorithm options:

    Available algorithms and parameters with their defaults:
    Inverse distance to a power (default)
        invdist:power=2.0:smoothing=0.0:radius1=0.0:radius2=0.0:angle=0.0:max_points=0:min_points=0:nodata=0.0
    Inverse distance to a power with nearest neighbor search
        invdistnn:power=2.0:radius=1.0:max_points=12:min_points=0:nodata=0
    Moving average
        average:radius1=0.0:radius2=0.0:angle=0.0:min_points=0:nodata=0.0
    Nearest neighbor
        nearest:radius1=0.0:radius2=0.0:angle=0.0:nodata=0.0
    Various data metrics
        <metric name>:radius1=0.0:radius2=0.0:angle=0.0:min_points=0:nodata=0.0
        possible metrics are:
            minimum
            maximum
            range
            count
            average_distance
            average_distance_pts
    Linear
        linear:radius=-1.0:nodata=0.0

    :param vec_file: input vector file path.
    :param vec_lyr: input vector layer name.
    :param vec_col: column within the input vector layer with the values
                    to be interpolated across the region of interest.
    :param input_img: an input image which defines the region of interest,
                      pixel resolution and projection of the output image.
    :param output_img: The output image file path.
    :param gdal_grid_alg: the gdal formatted string with the algorithm options.
    :param no_data_val: the output no data value (Default: 0.0)
    :param gdalformat: the output image format (Default: KEA)
    :param datatype: the output image data type (Default: Float 32)

    """
    import rsgislib.vectorutils
    from osgeo import gdal

    x_img_size, y_img_size = get_img_size(input_img)
    base_img_bbox = get_img_bbox(input_img)

    vec_ds_obj, vec_lyr_obj = rsgislib.vectorutils.open_gdal_vec_lyr(vec_file, vec_lyr)

    grid_opts = {
        "format": gdalformat,
        "outputType": rsgislib.get_gdal_datatype(datatype),
        "width": x_img_size,
        "height": y_img_size,
        "outputBounds": [
            base_img_bbox[0],
            base_img_bbox[3],
            base_img_bbox[1],
            base_img_bbox[2],
        ],
        "noData": no_data_val,
        "algorithm": gdal_grid_alg,
        "layers": vec_lyr,
        "zfield": vec_col,
    }
    gdal.Grid(output_img, vec_ds_obj, **grid_opts)
    vec_ds_obj = None


def create_inmem_gdal_ds_subset(
    input_img: str, bbox_sub: List[float], no_data_val: float = None
) -> gdal.Dataset:
    """
    A function which creates an in-memory (MEM) GDAL dataset for a
    subset defined by a BBOX (MinX, MaxX, MinY, MaxY)

    :param input_img: the input image
    :param bbox_sub: the bbox (MinX, MaxX, MinY, MaxY) defining the subset.
    :param no_data_val: the image no data value. If None then read from input_img
    :return: Returns a GDAL Dataset with a subset.

    """
    import rsgislib.tools.geometrytools

    img_x_res, img_y_res = get_img_res(input_img)
    img_y_res_abs = math.fabs(img_y_res)
    img_bbox = get_img_bbox(input_img)
    img_rsgislib_dtype = get_rsgislib_datatype_from_img(input_img)
    img_gdal_dtype = get_gdal_datatype_from_img(input_img)
    img_wkt_str = get_wkt_proj_from_img(input_img)
    img_n_bands = get_img_band_count(input_img)

    if no_data_val is None:
        no_data_val = get_img_no_data_value(input_img)
    if no_data_val is None:
        raise rsgislib.RSGISPyException(
            "Either provide a no data value or define in image header."
        )

    if not rsgislib.tools.geometrytools.does_bbox_contain(img_bbox, bbox_sub):
        raise rsgislib.RSGISPyException("Subset is not within the image.")

    x_min = bbox_sub[0]
    x_max = bbox_sub[1]
    y_min = bbox_sub[2]
    y_max = bbox_sub[3]

    tl_x = x_min
    tl_y = y_max

    width_coord = x_max - x_min
    height_coord = y_max - y_min

    sub_width = int(math.ceil(width_coord / img_x_res))
    sub_height = int(math.ceil(height_coord / img_y_res_abs))

    if (sub_width < 1) or (sub_height < 1):
        raise rsgislib.RSGISPyException(
            f"Something has gone wrong the subset size is "
            f"less than 1: {sub_width} x {sub_height}."
        )

    img_coord_x_diff = bbox_sub[0] - img_bbox[0]
    img_coord_y_diff = img_bbox[3] - bbox_sub[3]

    img_pxl_x_diff = int(math.floor((img_coord_x_diff / img_x_res) + 0.5))
    img_pxl_y_diff = int(math.floor((img_coord_y_diff / img_y_res_abs) + 0.5))

    gdal_driver = gdal.GetDriverByName("MEM")
    out_img_ds_obj = gdal_driver.Create(
        "MEM", sub_width, sub_height, img_n_bands, img_gdal_dtype
    )
    out_img_ds_obj.SetGeoTransform((tl_x, img_x_res, 0, tl_y, 0, img_y_res))
    out_img_ds_obj.SetProjection(img_wkt_str)

    in_img_ds_obj = gdal.Open(input_img, gdal.GA_ReadOnly)
    if in_img_ds_obj is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: {}".format(input_img)
        )

    raster_arr = numpy.zeros(
        (sub_height, sub_width), dtype=rsgislib.get_numpy_datatype(img_rsgislib_dtype)
    )
    raster_arr[...] = no_data_val
    for band in range(img_n_bands):
        in_band_obj = in_img_ds_obj.GetRasterBand(band + 1)
        out_band_obj = out_img_ds_obj.GetRasterBand(band + 1)

        in_band_obj.ReadAsArray(
            xoff=img_pxl_x_diff,
            yoff=img_pxl_y_diff,
            win_xsize=sub_width,
            win_ysize=sub_height,
            buf_obj=raster_arr,
        )

        out_band_obj.WriteArray(raster_arr)
    in_img_ds_obj = None
    return out_img_ds_obj
