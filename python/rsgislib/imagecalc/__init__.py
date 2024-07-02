#!/usr/bin/env python
"""
The imagecalc module contains functions for performing a number of
calculating on images.
"""
# import the C++ extension into this level
from ._imagecalc import *

import rsgislib

import math
import os
from typing import Dict, List, Tuple, Union

import numpy
from osgeo import gdal

TQDM_AVAIL = True
try:
    import tqdm
except ImportError:
    import rios.cuiprogress

    TQDM_AVAIL = False


gdal.UseExceptions()


# define our own classes
class BandDefn(object):
    """
    Create a list of these objects to pass to the band_math function
    as the 'bands' parameter.
    """

    def __init__(self, band_name=None, input_img=None, img_band=None):
        self.band_name = band_name
        self.input_img = input_img
        self.img_band = img_band


class StatsSummary:
    """
    This is passed to the imagePixelColumnSummary function"""

    def __init__(
        self,
        min=0.0,
        max=0.0,
        sum=0.0,
        median=0.0,
        stdev=0.0,
        mean=0.0,
        mode=0.0,
        calc_min=False,
        calc_max=False,
        calc_sum=False,
        calc_mean=False,
        calc_stdev=False,
        calc_median=False,
        calc_mode=False,
    ):
        self.min = min
        self.max = max
        self.sum = sum
        self.mean = mean
        self.stdev = stdev
        self.median = median
        self.mode = mode
        self.calc_min = calc_min
        self.calc_max = calc_max
        self.calc_sum = calc_sum
        self.calc_mean = calc_mean
        self.calc_stdev = calc_stdev
        self.calc_median = calc_median
        self.calc_mode = calc_mode


class ImageBandRescale:
    """
    Data structure for rescaling information for rescale_img_pxl_vals function.
    :param band: specified image band (band numbering starts at 1).
    :param in_min: the input image band minimum value for rescaling.
    :param in_max: the input image band maximum value for rescaling.
    :param no_data_val: no data value for the input image band.
    :param out_min: the output image band minimum value for rescaling.
    :param out_max: the output image band maximum value for rescaling.
    :param out_no_data: no data value for the output image band.
    """

    def __init__(
        self,
        band=0,
        in_min=0.0,
        in_max=0.0,
        no_data_val=0,
        out_min=0.0,
        out_max=0.0,
        out_no_data=0.0,
    ):
        """
        :param band: specified image band (band numbering starts at 1).
        :param in_min: the input image band minimum value for rescaling.
        :param in_max: the input image band maximum value for rescaling.
        :param no_data_val: no data value for the input image band.
        :param out_min: the output image band minimum value for rescaling.
        :param out_max: the output image band maximum value for rescaling.
        :param out_no_data: no data value for the output image band.

        """
        self.band = band
        self.in_min = in_min
        self.in_max = in_max
        self.no_data_val = no_data_val
        self.out_min = out_min
        self.out_max = out_max
        self.out_no_data = out_no_data

    def __str__(self):
        str_val = "Band {} [{}, {}, {}] [{}, {}, {}]".format(
            self.band,
            self.in_min,
            self.in_max,
            self.no_data_val,
            self.out_min,
            self.out_max,
            self.out_no_data,
        )
        return str_val

    def __repr__(self):
        str_val = "Band {} [{}, {}, {}] [{}, {}, {}]".format(
            self.band,
            self.in_min,
            self.in_max,
            self.no_data_val,
            self.out_min,
            self.out_max,
            self.out_no_data,
        )
        return str_val


def calc_dist_to_img_vals(
    input_img: str,
    output_img: str,
    pxl_vals: List[int],
    img_band: int = 1,
    gdalformat: str = "KEA",
    max_dist: float = None,
    no_data_val: float = None,
    out_no_data_val: float = None,
    unit_geo: bool = True,
):
    """
    A function to calculate the distance to the nearest pixel value with one
    of the specified values.

    :param input_img: is a string specifying the input image file.
    :param output_img: is a string specfiying the output image file.
    :param pxl_vals: is a number of list of numbers specifying the features to which
                     the distance from should be calculated.
    :param img_band: is an integer specifying the image band of the input image to
                     be used (Default = 1).
    :param gdalformat: is a string specifying the output image format (Default = KEA)
    :param max_dist: is a number specifying the maximum distance to be calculated, if
                     None not max value is used (Default = None).
    :param no_data_val: is the no data value in the input image for which distance
                        should not be calculated for (Default = None; None = no
                        specified no data value).
    :param out_no_data_val: is output image no data value. If None then set as the
                            max_dist value.
    :param unit_geo: is a boolean specifying the output distance units.
                     True = Geographic units (e.g., metres),
                     False is in Pixels (Default = True).

    .. code:: python

        import rsgislib.imagecalc
        cloudsImg = 'LS5TM_20110701_lat52lon421_r24p204_clouds.kea'
        dist2Clouds = 'LS5TM_20110701_lat52lon421_r24p204_distclouds.kea'
        # Pixel value 1 == Clouds
        # Pixel value 2 == Cloud Shadows
        rsgislib.imagecalc.calc_dist_to_img_vals(cloudsImg, dist2Clouds, pxl_vals=[1,2], out_no_data_val=-9999)

    """
    # Check gdal is available
    import rsgislib.imageutils

    have_list_vals = False
    if isinstance(pxl_vals, list):
        have_list_vals = True

    proxOptions = []

    if max_dist is not None:
        proxOptions.append("MAXDIST=" + str(max_dist))
    if no_data_val is not None:
        proxOptions.append("NODATA=" + str(no_data_val))
    if unit_geo:
        proxOptions.append("DISTUNITS=GEO")
    else:
        proxOptions.append("DISTUNITS=PIXEL")

    if have_list_vals:
        strVals = ""
        first = True
        for val in pxl_vals:
            if first:
                strVals = str(val)
                first = False
            else:
                strVals = strVals + "," + str(val)
        proxOptions.append("VALUES=" + strVals)
    else:
        proxOptions.append("VALUES=" + str(pxl_vals))

    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    if out_no_data_val is None:
        out_no_data_val = max_dist
    if out_no_data_val is None:
        raise ValueError("Either out_no_data_val or max_dist must be supplied.")

    valsImgDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    valsImgBand = valsImgDS.GetRasterBand(img_band)
    rsgislib.imageutils.create_copy_img(
        input_img, output_img, 1, out_no_data_val, gdalformat, rsgislib.TYPE_32FLOAT
    )
    distImgDS = gdal.Open(output_img, gdal.GA_Update)
    distImgBand = distImgDS.GetRasterBand(1)
    gdal.ComputeProximity(valsImgBand, distImgBand, proxOptions, callback=callback)
    distImgBand = None
    distImgDS = None
    valsImgBand = None
    valsImgDS = None


def _computeProximityArrArgsFunc(arg_vals):
    """
    This function is used internally within calc_dist_to_classes
    for the multiprocessing Pool
    """
    import rsgislib.imageutils

    # 0: tileFile
    # 1: distTileFile
    # 2: proxOptions
    # 3: no_data_val,
    # 4: format (e.g., KEA)
    # 5: img_band
    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress
    classImgDS = gdal.Open(arg_vals[0], gdal.GA_ReadOnly)
    classImgBand = classImgDS.GetRasterBand(arg_vals[5])
    rsgislib.imageutils.create_copy_img(
        arg_vals[0], arg_vals[1], 1, arg_vals[3], arg_vals[4], rsgislib.TYPE_32FLOAT
    )
    distImgDS = gdal.Open(arg_vals[1], gdal.GA_Update)
    distImgBand = distImgDS.GetRasterBand(arg_vals[5])
    gdal.ComputeProximity(classImgBand, distImgBand, arg_vals[2], callback=callback)
    distImgBand = None
    distImgDS = None
    classImgBand = None
    classImgDS = None


def calc_dist_to_img_vals_tiled(
    input_img: str,
    output_img: str,
    pxl_vals: List[int],
    img_band: int = 1,
    max_dist: float = 1000,
    no_data_val: float = 1000,
    out_no_data_val: float = None,
    gdalformat: str = "KEA",
    unit_geo: bool = True,
    tmp_dir: str = "tmp",
    tile_size: int = 2000,
    n_cores: int = -1,
):
    """
    A function to calculate the distance to the nearest pixel value with
    one of the specified values.

    :param input_img: is a string specifying the input image file.
    :param output_img: is a string specfiying the output image file.
    :param pxl_vals: is a number of list of numbers specifying the features to
                     which the distance from should be calculated.
    :param img_band: is an integer specifying the image band of the input image
                     to be used (Default = 1).
    :param gdalformat: is a string specifying the output image format (Default = KEA)
    :param max_dist: is a number specifying the maximum distance to be calculated,
                     if None not max value is used (Default = None).
    :param no_data_val: is the no data value in the input image for which distance
                        should not be calculated for (Default = None;
                        None = no specified no data value).
    :param out_no_data_val: is output image no data value. If None then set as the
                            max_dist value.
    :param unit_geo: is a boolean specifying the output distance units.
                     True = Geographic units (e.g., metres),
                     False is in Pixels (Default = True).
    :param tmp_dir: is a directory to be used for storing the image tiles and other
                    temporary files - if not directory does not exist it will be
                    created and deleted on completion (Default: tmp).
    :param tile_size: is an int specifying in pixels the size of the image tiles
                      used for processing (Default: 2000)
    :param n_cores: is the number of processing cores which are available to be used
                    for this processing. If -1 all available cores will be
                    used. (Default: -1)

    .. code:: python

        import rsgislib.imagecalc
        cloudsImg = 'LS5TM_20110701_lat52lon421_r24p204_clouds.kea'
        dist2Clouds = 'LS5TM_20110701_lat52lon421_r24p204_distclouds.kea'
        # Pixel value 1 == Clouds
        # Pixel value 2 == Cloud Shadows
        rsgislib.imagecalc.calc_dist_to_img_vals_tiled(cloudsImg, dist2Clouds, pxl_vals=[1,2])

    """

    # Check gdal is available
    import glob
    import shutil
    from multiprocessing import Pool

    import rsgislib.imageutils
    import rsgislib.tools.filetools
    import rsgislib.tools.utils

    have_list_vals = False
    if isinstance(pxl_vals, list):
        have_list_vals = True

    tmpPresent = True
    if not os.path.exists(tmp_dir):
        print(
            "WARNING: '{}' directory does not exist so creating it...".format(tmp_dir)
        )
        os.makedirs(tmp_dir)
        tmpPresent = False

    if n_cores <= 0:
        n_cores = rsgislib.tools.utils.num_process_cores()

    if out_no_data_val is None:
        out_no_data_val = max_dist

    uid = rsgislib.tools.utils.uid_generator()

    xRes, yRes = rsgislib.imageutils.get_img_res(input_img)
    if unit_geo:
        xMaxDistPxl = math.ceil(max_dist / xRes) + 10
        yMaxDistPxl = math.ceil(max_dist / yRes) + 10
    else:
        xMaxDistPxl = max_dist + 10
        yMaxDistPxl = max_dist + 10

    tileOverlap = xMaxDistPxl
    if yMaxDistPxl > xMaxDistPxl:
        tileOverlap = yMaxDistPxl

    imgTilesDIR = os.path.join(tmp_dir, "ImgTiles_" + uid)
    imgTilesDIRPresent = True
    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
        imgTilesDIRPresent = False

    imgTileBase = os.path.join(imgTilesDIR, "ImgTile")
    rsgislib.imageutils.create_tiles(
        input_img,
        imgTileBase,
        tile_size,
        tile_size,
        tileOverlap,
        0,
        "KEA",
        rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img),
        "kea",
    )
    imgTileFiles = glob.glob(imgTileBase + "*.kea")

    distTilesDIR = os.path.join(tmp_dir, "DistTiles_" + uid)
    distTilesDIRPresent = True
    if not os.path.exists(distTilesDIR):
        os.makedirs(distTilesDIR)
        distTilesDIRPresent = False

    proxOptions = []

    if max_dist is not None:
        proxOptions.append("MAXDIST=" + str(max_dist))
    if no_data_val is not None:
        proxOptions.append("NODATA=" + str(no_data_val))
    if unit_geo:
        proxOptions.append("DISTUNITS=GEO")
    else:
        proxOptions.append("DISTUNITS=PIXEL")

    if have_list_vals:
        strVals = ""
        first = True
        for val in pxl_vals:
            if first:
                strVals = str(val)
                first = False
            else:
                strVals = strVals + "," + str(val)
        proxOptions.append("VALUES=" + strVals)
    else:
        proxOptions.append("VALUES=" + str(pxl_vals))

    distTiles = []
    distTileArgs = []
    for tileFile in imgTileFiles:
        baseTileName = os.path.basename(tileFile)
        distTileFile = os.path.join(distTilesDIR, baseTileName)
        tileArgs = [
            tileFile,
            distTileFile,
            proxOptions,
            out_no_data_val,
            "KEA",
            img_band,
        ]
        distTiles.append(distTileFile)
        distTileArgs.append(tileArgs)

    with Pool(n_cores) as p:
        p.map(_computeProximityArrArgsFunc, distTileArgs)

    # Mosaic Tiles
    rsgislib.imageutils.create_img_mosaic(
        distTiles, output_img, 0, 0, 1, 1, gdalformat, rsgislib.TYPE_32FLOAT
    )
    rsgislib.imageutils.pop_img_stats(
        output_img, use_no_data=True, no_data_val=out_no_data_val, calc_pyramids=True
    )

    for imgFile in distTiles:
        rsgislib.tools.filetools.delete_file_with_basename(imgFile)

    if not imgTilesDIRPresent:
        shutil.rmtree(imgTilesDIR, ignore_errors=True)
    else:
        for tileFile in imgTileFiles:
            rsgislib.tools.filetools.delete_file_with_basename(tileFile)

    if not distTilesDIRPresent:
        shutil.rmtree(distTilesDIR, ignore_errors=True)

    if not tmpPresent:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def buffer_img_pxl_vals(
    input_img: str,
    output_img: str,
    pxl_vals: List[int],
    buf_thres: float,
    tmp_dir: str,
    gdalformat: str = "KEA",
    img_band: int = 1,
    unit_geo: bool = True,
):
    """
    A function which uses the calc_dist_to_img_vals function and a threshold to
    buffer image pixel value(s) to create a binary mask

    :param input_img: The input image
    :param output_img: the output image
    :param pxl_vals: a list of pixel values defining the region to be buffered
    :param buf_thres: the threshold below which the buffered region will be defined.
    :param tmp_dir: a tmp directory for intermediate outputs
    :param gdalformat: output image format
    :param img_band: the input image bands
    :param unit_geo: is a boolean specifying the buf_thres distance units.
                     True = Geographic units (e.g., metres),
                     False is in Pixels (Default = True).

    """
    import rsgislib.tools.filetools

    if not os.path.exists(tmp_dir):
        raise rsgislib.RSGISPyException("The tmp_dir does not exist.")

    basename = rsgislib.tools.filetools.get_file_basename(input_img)

    dist_img = os.path.join(tmp_dir, f"{basename}_dist.kea")

    calc_dist_to_img_vals(
        input_img,
        dist_img,
        pxl_vals,
        img_band,
        "KEA",
        max_dist=(buf_thres * 1.1),
        no_data_val=None,
        out_no_data_val=-999,
        unit_geo=unit_geo,
    )

    image_math(
        dist_img,
        output_img,
        "(b1>=0)&&(b1<{})?1:0".format(buf_thres),
        gdalformat,
        rsgislib.TYPE_32FLOAT,
    )

    rsgislib.tools.filetools.delete_file_silent(dist_img)


def count_pxls_of_val(input_img: str, vals: List[int], img_band: int = None):
    """
    Function which counts the number of pixels of a set of values returning a
    list in the same order as the list of values provided.

    :param input_img: the input image
    :param vals: is a list of pixel values to be counted
    :param img_band: specify the image band for which the analysis is to be undertaken.
                     If None (default) then all bands will be used.
    :return: list of pixel counts in same order as the vals input list

    """
    from rios import applier

    import rsgislib.imageutils

    if vals is None:
        raise rsgislib.RSGISPyException("Input vals list must not be None.")
    if len(vals) == 0:
        raise rsgislib.RSGISPyException(
            "At least 1 value should be provided within the vals input variable."
        )
    n_bands = rsgislib.imageutils.get_img_band_count(input_img)
    if (img_band is not None) and ((img_band < 1) or (img_band > n_bands)):
        raise rsgislib.RSGISPyException(
            "The specified input image band is not within the input image."
        )

    def _count_pxl_vals(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        for idx in range(otherargs.num_vals):
            if otherargs.img_band_idx is None:
                otherargs.out_vals[idx] = (
                    otherargs.out_vals[idx]
                    + (inputs.image == otherargs.vals[idx]).sum()
                )
            else:
                otherargs.out_vals[idx] = (
                    otherargs.out_vals[idx]
                    + (
                        inputs.image[otherargs.img_band_idx,] == otherargs.vals[idx]
                    ).sum()
                )

    if img_band is not None:
        img_band_idx = img_band - 1
    else:
        img_band_idx = None

    num_vals = len(vals)
    out_vals = numpy.zeros(num_vals, dtype=numpy.int64)

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    outfiles = applier.FilenameAssociations()
    otherargs = applier.OtherInputs()
    otherargs.out_vals = out_vals
    otherargs.num_vals = num_vals
    otherargs.vals = vals
    otherargs.img_band_idx = img_band_idx
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar

    applier.apply(_count_pxl_vals, infiles, outfiles, otherargs, controls=aControls)

    return out_vals


def get_unique_values(input_img: str, img_band: int = 1):
    """
    Find the unique image values within an image band.
    Note, the whole image band gets read into memory.

    :param input_img: input image file path
    :param img_band: image band to be processed (starts at 1)

    :return: array of unique values.

    """
    imgDS = gdal.Open(input_img)
    if imgDS is None:
        raise rsgislib.RSGISPyException("Could not open output image")
    imgBand = imgDS.GetRasterBand(img_band)
    if imgBand is None:
        raise rsgislib.RSGISPyException(
            "Could not open output image band ({})".format(img_band)
        )
    valsArr = imgBand.ReadAsArray()
    imgDS = None

    uniq_vals = numpy.unique(valsArr)
    return uniq_vals


def get_pca_eigen_vector(
    input_img: str,
    pxl_n_sample: int,
    no_data_val: float = None,
    out_matrix_file: str = None,
):
    """
    A function which takes a sample from an input image and uses it to
    generate eigenvector for a PCA. Note. this can be used as input
    to rsgislib.imagecalc.pca

    :param input_img: the image from which the random sample will be taken.
    :param pxl_n_sample: the sample to be taken (e.g., a value of 100 will
                         sample every 100th pixel)
    :param no_data_val: provide a no data value which is to be ignored during
                        processing. If None then ignored (Default: None)
    :param out_matrix_file: path and name for the output rsgislib matrix file.
                            If None file is not created (Default: None)

    :returns: 1. array with the eigenvector, 2. array with the ratio of the
              explained variance
    """
    from sklearn.decomposition import PCA

    import rsgislib.imageutils

    # Read input data from image file.
    X = rsgislib.imageutils.extract_img_pxl_sample(input_img, pxl_n_sample, no_data_val)

    print(str(X.shape[0]) + " values were extracted from the input image.")

    pca = PCA()
    pca.fit(X)

    if out_matrix_file is not None:
        f = open(out_matrix_file, "w")
        f.write("m=" + str(pca.components_.shape[0]) + "\n")
        f.write("n=" + str(pca.components_.shape[1]) + "\n")
        first = True
        for val in pca.components_.flatten():
            if first:
                f.write(str(val))
                first = False
            else:
                f.write("," + str(val))
        f.write("\n\n")
        f.flush()
        f.close()

    pcaComp = 1
    print("Prop. of variance explained:")
    for val in pca.explained_variance_ratio_:
        print("\t PCA Component " + str(pcaComp) + " = " + str(round(val, 4)))
        pcaComp = pcaComp + 1

    return pca.components_, pca.explained_variance_ratio_


def perform_image_pca(
    input_img: str,
    output_img: str,
    out_eigen_vec_file: str,
    n_comps: int = None,
    pxl_n_sample: int = 100,
    gdalformat: str = "KEA",
    datatype: int = rsgislib.TYPE_32FLOAT,
    no_data_val: float = None,
    calc_stats: bool = True,
):
    """
    A function which performs a PCA on the input image.

    :param input_img: the image from which the random sample will be taken.
    :param output_img: the output image transformed using the calculated PCA
    :param out_eigen_vec_file: path and name for the output rsgislib matrix file
                               containing the eigenvector for the PCA.
    :param n_comps: the number of PCA compoents outputted. A value of None is
                    all components (Default: None)
    :param pxl_n_sample: the sample to be taken (e.g., a value of 100 will sample
                         every 100th pixel) (Default: 100)
    :param gdalformat: the output gdal supported file format (Default KEA)
    :param datatype: the output data type of the input image
                     (Default: rsgislib.TYPE_32FLOAT)
    :param no_data_val: provide a no data value which is to be ignored during
                        processing. If None then ignored (Default: None)
    :param calc_stats: Boolean specifying whether pyramids and statistics should be
                       calculated for the output image. (Default: True)
    :returns: an array with the ratio of the explained variance per band.

    """
    import rsgislib.imageutils

    eigenVec, varExplain = get_pca_eigen_vector(
        input_img, pxl_n_sample, no_data_val, out_eigen_vec_file
    )
    outNComp = varExplain.shape[0]
    if n_comps is not None:
        if n_comps > varExplain.shape[0]:
            raise rsgislib.RSGISPyException(
                "You cannot output more components than "
                "the number of input image bands."
            )
        outNComp = n_comps

    pca(input_img, out_eigen_vec_file, output_img, outNComp, gdalformat, datatype)
    if calc_stats:
        use_no_data = False
        lcl_no_data_val = 0
        if no_data_val is not None:
            use_no_data = True
            lcl_no_data_val = no_data_val
        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data, lcl_no_data_val, True
        )

    return varExplain


def perform_image_mnf(
    input_img: str,
    output_img: str,
    n_comps: int = None,
    pxl_n_sample: int = 100,
    in_img_no_data: float = None,
    tmp_dir: str = "tmp",
    gdalformat: str = "KEA",
    datatype: int = rsgislib.TYPE_32FLOAT,
    calc_stats: bool = True,
):
    """
    A function which takes a sample from an input image and uses it to
    generate eigenvector for a MNF. Note. this can be used as input to
    rsgislib.imagecalc.pca

    :param input_img: the image to which the MNF will be applied
    :param output_img: the output image file with the MNF result
    :param n_comps: the number of components to be outputted
    :param pxl_n_sample: the sample to be taken (e.g., a value of 100 will sample
                         every 100th pixel) for the PCA
    :param in_img_no_data: provide a no data value which is to be ignored during
                           processing. If None then try to read from input image.
    :param tmp_dir: a directory where temporary output files will be stored. If it
                    doesn't exist it will be created.
    :param gdalformat: output image file format
    :param datatype: data type for the output image. Note, it is common to have
                     negative values.
    :param calc_stats: whether image statistics and pyramids could be calculated.
    :returns: array with the ratio of the explained variance

    """
    import shutil

    from sklearn.decomposition import PCA

    import rsgislib.imageutils
    import rsgislib.tools.filetools

    created_tmp_dir = False
    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)
        created_tmp_dir = True

    img_basename = rsgislib.tools.filetools.get_file_basename(input_img)

    if in_img_no_data is None:
        in_img_no_data = rsgislib.imageutils.get_img_no_data_value(input_img)
        if in_img_no_data is None:
            raise rsgislib.RSGISPyException(
                "A no data value for the input image must be provided."
            )

    valid_msk_img = os.path.join(tmp_dir, "{}_vld_msk.kea".format(img_basename))
    rsgislib.imageutils.gen_valid_mask(input_img, valid_msk_img, "KEA", in_img_no_data)

    whiten_img = os.path.join(tmp_dir, "{}_whiten.kea".format(img_basename))
    rsgislib.imageutils.whiten_image(input_img, valid_msk_img, 1, whiten_img, "KEA")

    # Read input data from image file.
    X = rsgislib.imageutils.extract_img_pxl_sample(
        whiten_img, pxl_n_sample, in_img_no_data
    )
    print("{} values were extracted from the input image.".format(X.shape[0]))

    pca_obj = PCA()
    pca_obj.fit(X)

    eigenVecFile = os.path.join(tmp_dir, "{}_eigen_vec.txt".format(img_basename))
    f = open(eigenVecFile, "w")
    f.write("m=" + str(pca_obj.components_.shape[0]) + "\n")
    f.write("n=" + str(pca_obj.components_.shape[1]) + "\n")
    first = True
    for val in pca_obj.components_.flatten():
        if first:
            f.write(str(val))
            first = False
        else:
            f.write("," + str(val))
    f.write("\n\n")
    f.flush()
    f.close()

    pcaComp = 1
    print("Prop. of variance explained:")
    for val in pca_obj.explained_variance_ratio_:
        print("\t PCA Component " + str(pcaComp) + " = " + str(round(val, 4)))
        pcaComp = pcaComp + 1
    varExplain = pca_obj.explained_variance_ratio_

    outNComp = varExplain.shape[0]
    if n_comps is not None:
        if n_comps > varExplain.shape[0]:
            raise rsgislib.RSGISPyException(
                "You cannot output more components than "
                "the number of input image bands."
            )
        outNComp = n_comps

    pca(whiten_img, eigenVecFile, output_img, outNComp, gdalformat, datatype)
    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, in_img_no_data, True)

    if created_tmp_dir:
        shutil.rmtree(tmp_dir)
    else:
        rsgislib.tools.filetools.delete_file_silent(valid_msk_img)
        rsgislib.tools.filetools.delete_file_silent(whiten_img)
        rsgislib.tools.filetools.delete_file_silent(eigenVecFile)

    return varExplain


def rescale_img_pxl_vals(
    input_img: str,
    output_img: str,
    gdalformat: str,
    datatype: int,
    band_rescale_objs: List[ImageBandRescale],
    trim_to_limits: bool = True,
):
    """
    Function which rescales an input image base on a list of rescaling parameters.

    :param input_img: the input image
    :param output_img: the output image file name and path (will be same
                       dimensions as the input)
    :param gdalformat: the GDAL image file format of the output image file.
    :param band_rescale_objs: list of ImageBandRescale objects
    :param trim_to_limits: whether to trim the output to the output min/max values.

    """
    from rios import applier

    bandRescaleDict = dict()
    for rescaleObj in band_rescale_objs:
        bandRescaleDict[rescaleObj.band - 1] = rescaleObj

    numpyDT = rsgislib.get_numpy_datatype(datatype)

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.rescaleDict = bandRescaleDict
    otherargs.trim = trim_to_limits
    otherargs.numpyDT = numpyDT
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyRescale(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        outputs.outimage = numpy.zeros_like(inputs.image, dtype=numpyDT)
        for idx in range(inputs.image.shape[0]):
            outputs.outimage[idx] = numpy.where(
                inputs.image[idx] == otherargs.rescaleDict[idx].no_data_val,
                otherargs.rescaleDict[idx].out_no_data,
                (
                    (
                        (inputs.image[idx] - otherargs.rescaleDict[idx].in_min)
                        / (
                            inputs.image[idx]
                            - otherargs.rescaleDict[idx].in_max
                            - inputs.image[idx]
                            - otherargs.rescaleDict[idx].in_min
                        )
                    )
                    * (
                        inputs.image[idx]
                        - otherargs.rescaleDict[idx].out_max
                        - inputs.image[idx]
                        - otherargs.rescaleDict[idx].out_min
                    )
                )
                + inputs.image[idx]
                - otherargs.rescaleDict[idx].out_min,
            )
            if otherargs.trim:
                outputs.outimage[idx] = numpy.where(
                    (outputs.outimage[idx] != otherargs.rescaleDict[idx].out_no_data)
                    & (outputs.outimage[idx] < otherargs.rescaleDict[idx].out_min),
                    otherargs.rescaleDict[idx].out_min,
                    outputs.outimage[idx],
                )
                outputs.outimage[idx] = numpy.where(
                    (outputs.outimage[idx] != otherargs.rescaleDict[idx].out_no_data)
                    & (outputs.outimage[idx] > otherargs.rescaleDict[idx].out_max),
                    otherargs.rescaleDict[idx].out_max,
                    outputs.outimage[idx],
                )

    applier.apply(_applyRescale, infiles, outfiles, otherargs, controls=aControls)


def calc_histograms_for_msk_vals(
    input_img: str,
    img_band: int,
    img_msk: str,
    msk_band: int,
    min_val: float,
    max_val: float,
    bin_width: float,
    msk_vals: List[float] = None,
):
    """
    A function which reads the image bands (values and mask) into memory and creates a
    histogram for each value within the mask value. Within the mask 0 is considered
    to be no data.

    :param input_img: image values image file path.
    :param img_band: values image band
    :param img_msk: file path for image mask.
    :param msk_band: mask image band
    :param min_val: minimum value for the histogram bins
    :param max_val: maximum value for the histogram bins
    :param bin_width: the width of the histograms bins.
    :param msk_vals: a list of values within the mask can be provided to just consider
                    a limited number of mask values when calculating the histograms.
                    If None (default) then calculated for all mask values.

    :return: returns a dict of mask values with an array for the histogram.

    """
    min_val = float(min_val)
    max_val = float(max_val)
    nBins = math.ceil((max_val - min_val) / bin_width)
    max_val = float(min_val + (bin_width * nBins))

    imgValsDS = gdal.Open(input_img)
    imgValsBand = imgValsDS.GetRasterBand(img_band)
    valsArr = imgValsBand.ReadAsArray()
    imgValsDS = None

    imgMskDS = gdal.Open(img_msk)
    imgMskBand = imgMskDS.GetRasterBand(msk_band)
    mskArr = imgMskBand.ReadAsArray()
    imgMskDS = None

    if msk_vals is None:
        uniq_vals = numpy.unique(mskArr)
    else:
        uniq_vals = msk_vals

    hist_dict = dict()

    for msk_val in uniq_vals:
        if msk_val != 0:
            mskd_vals = valsArr[mskArr == msk_val]
            hist_arr, bin_edges = numpy.histogram(
                mskd_vals, bins=nBins, range=(min_val, max_val)
            )
            hist_dict[msk_val] = hist_arr

    valsArr = None
    mskArr = None

    return hist_dict


def calc_sum_stats_msk_vals(
    input_img: str,
    img_band: int,
    img_msk: str,
    msk_band: int,
    msk_vals: List[int] = None,
    use_no_data: bool = True,
    no_data_val: float = None,
    out_no_data_val: float = -9999,
):
    """
    A function which reads the image bands (values and mask) into memory
    calculate standard summary statistics (min, max, mean, std dev, median)

    :param input_img: image values image file path.
    :param img_band: values image band
    :param img_msk: file path for image mask.
    :param msk_band: mask image band
    :param msk_vals: a list of values within the mask can be provided to just consider
                    a limited number of mask values when calculating the histograms.
                    If None (default) then calculated for all mask values.
    :param use_no_data: Use no data value for the input image.
    :param no_data_val: no data value for the input image (if None then read from
                        input image header)
    :param out_no_data_val: output no data value written to output dict if there are
                            no valid pixel values.
    :return: returns a dict summary statistics (Min, Max, Mean, Std Dev, Median)

    """
    import rsgislib.imageutils

    if use_no_data and (no_data_val is None):
        no_data_val = rsgislib.imageutils.get_img_no_data_value(
            input_img, img_band=img_band
        )

    img_vals_ds = gdal.Open(input_img)
    img_vals_band = img_vals_ds.GetRasterBand(img_band)
    vals_arr = img_vals_band.ReadAsArray()
    img_vals_ds = None

    img_msk_ds = gdal.Open(img_msk)
    img_msk_band = img_msk_ds.GetRasterBand(msk_band)
    msk_arr = img_msk_band.ReadAsArray()
    img_msk_ds = None

    if msk_vals is None:
        uniq_vals = numpy.unique(msk_arr)
        uniq_vals = uniq_vals[uniq_vals != 0]
    else:
        uniq_vals = msk_vals

    pxls_vals_lst = list()
    for msk_val in uniq_vals:
        pxls_vals_lst.append(vals_arr[msk_arr == msk_val])

    stats_dict = dict()
    stats_dict["min"] = out_no_data_val
    stats_dict["max"] = out_no_data_val
    stats_dict["mean"] = out_no_data_val
    stats_dict["stddev"] = out_no_data_val
    stats_dict["median"] = out_no_data_val

    pxls_vals = numpy.stack(pxls_vals_lst).flatten()
    if use_no_data:
        pxls_vals = pxls_vals[pxls_vals != no_data_val]

    if len(pxls_vals) > 0:
        stats_dict["min"] = pxls_vals.min()
        stats_dict["max"] = pxls_vals.max()
        stats_dict["mean"] = pxls_vals.mean()
        stats_dict["stddev"] = pxls_vals.std()
        stats_dict["median"] = numpy.median(pxls_vals)

    vals_arr = None
    msk_arr = None
    pxls_vals = None

    return stats_dict


def calc_imgs_pxl_mode(
    input_imgs: List[str], output_img: str, gdalformat: str, no_data_val: float = 0
):
    """
    Function which calculates the mode of a group of images.

    Warning, this function can be very slow!!!
    You probably want to use rsgislib.imagecalc.imagePixelColumnSummary

    :param input_imgs: the list of images
    :param output_img: the output image file name and path (will be
                       same dimensions as the input)
    :param gdalformat: the GDAL image file format of the output image file.

    """
    import scipy.stats
    from rios import applier

    import rsgislib.imageutils

    datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_imgs[0])
    numpyDT = rsgislib.get_numpy_datatype(datatype)

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.images = input_imgs
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.no_data_val = no_data_val
    otherargs.numpyDT = numpyDT
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyCalcMode(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        image_data = numpy.concatenate(inputs.images, axis=0).astype(numpy.float32)
        image_data[image_data == otherargs.no_data_val] = numpy.nan
        mode_arr, count_arr = scipy.stats.mode(image_data, axis=0, nan_policy="omit")
        if len(mode_arr.shape) == 2:
            outputs.outimage = numpy.expand_dims(
                mode_arr.astype(otherargs.numpyDT), axis=0
            )
        else:
            outputs.outimage = mode_arr.astype(otherargs.numpyDT)

    applier.apply(_applyCalcMode, infiles, outfiles, otherargs, controls=aControls)


def calc_imgs_pxl_percentiles(
    input_imgs: List[str],
    percentiles: List[float],
    output_img: str,
    gdalformat: str,
    no_data_val: float = 0,
):
    """
    Function which calculates percentiles on a per-pixel basis for a
    group of images. Note, all bands in all the input images are used
    for the analysis.

    :param input_imgs: the list of images - note all bands are used.
    :param percentiles: a list of percentiles (0-100) to be calculated.
    :param output_img: the output image file name and path (will be
                       same dimensions as the input and then number of
                       bands will be the same at the number of percentiles.)
    :param gdalformat: the GDAL image file format of the output image file.

    """
    from rios import applier

    for percent in percentiles:
        if percent < 0:
            raise Exception(f"Percentile is less than 0 ({percent})")
        elif percent > 100:
            raise Exception(f"Percentile is greater than 100 ({percent})")

    import rsgislib.imageutils

    datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_imgs[0])
    numpyDT = rsgislib.get_numpy_datatype(datatype)

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.images = input_imgs
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.no_data_val = no_data_val
    otherargs.numpyDT = numpyDT
    otherargs.percentiles = percentiles
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyCalcPercentile(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        image_data = numpy.concatenate(inputs.images, axis=0).astype(numpy.float32)
        image_data[image_data == otherargs.no_data_val] = numpy.nan
        percentiles_arr = numpy.nanpercentile(image_data, otherargs.percentiles, axis=0)
        if len(percentiles_arr.shape) == 2:
            outputs.outimage = numpy.expand_dims(
                percentiles_arr.astype(otherargs.numpyDT), axis=0
            )
        else:
            outputs.outimage = percentiles_arr.astype(otherargs.numpyDT)

    applier.apply(
        _applyCalcPercentile, infiles, outfiles, otherargs, controls=aControls
    )


def calc_img_basic_stats_for_ref_region(
    in_ref_img: str, in_stats_imgs: List[str], output_img: str, gdalformat: str = "KEA"
):
    """
    A function which calculates the mean and standard deviation through a series of
    input images. The region for processing is defined by the reference image and
    images padded with no-data where no data is present.

    The output image has twice the number of bands as the input image providing
    a mean and standard deviation for each input band.

    If the input images has 2 bands then the output bands will have the following
    order:

    1. band 1 mean
    2. band 1 std dev
    3. band 2 mean
    4. band 2 std dev

    :param in_ref_img: reference image which defines the output image
    :param in_stats_imgs: a list of input images over which the stats will
                          be calculated.
    :param output_img: the output image path and file name
    :param gdalformat: the output image file format. Default KEA.

    """
    from rios import applier

    import rsgislib.imageutils

    first = True
    n_bands = 0
    no_data_val = 0
    for img in in_stats_imgs:
        print(img)
        if first:
            n_bands = rsgislib.imageutils.get_img_band_count(img)
            no_data_val = rsgislib.imageutils.get_img_no_data_value(img)
            first = False
        else:
            if n_bands != rsgislib.imageutils.get_img_band_count(img):
                raise rsgislib.RSGISPyException(
                    "The number of bands must be the same in all input images."
                )
            if no_data_val != rsgislib.imageutils.get_img_no_data_value(img):
                raise rsgislib.RSGISPyException(
                    "The no data value should be the same in all input images."
                )

    # Internal function to calculate mean and standard deviation of
    # the input images
    def _calcBasicStats(info, inputs, outputs, otherargs):
        n_imgs = len(inputs.imgs)
        blk_shp = inputs.imgs[0].shape
        if blk_shp[0] != otherargs.n_bands:
            raise rsgislib.RSGISPyException(
                "Block shape and the number of input image bands do not align."
            )
        outputs.output_img = numpy.zeros(
            (blk_shp[0] * 2, blk_shp[1], blk_shp[2]), dtype=float
        )

        band_arr = []
        for band in range(blk_shp[0]):
            band_arr.append(numpy.zeros((n_imgs, blk_shp[1], blk_shp[2]), dtype=float))

        img_idx = 0
        for img_blk in inputs.imgs:
            for band in range(blk_shp[0]):
                band_arr[band][img_idx] = img_blk[band]
            img_idx = img_idx + 1

        for band in range(blk_shp[0]):
            band_arr[band][band_arr[band] == otherargs.no_data_val] = numpy.nan

            outputs.output_img[band * 2] = numpy.nanmean(band_arr[band], axis=0)
            outputs.output_img[band * 2 + 1] = numpy.nanstd(band_arr[band], axis=0)

            outputs.output_img[band * 2][
                numpy.isnan(outputs.output_img[band * 2])
            ] = otherargs.no_data_val
            outputs.output_img[band * 2 + 1][
                numpy.isnan(outputs.output_img[band * 2 + 1])
            ] = 0.0

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.imgs = in_stats_imgs

    otherargs = applier.OtherInputs()
    otherargs.n_bands = n_bands
    otherargs.no_data_val = no_data_val

    outfiles = applier.FilenameAssociations()
    outfiles.output_img = output_img

    aControls = applier.ApplierControls()
    aControls.referenceImage = in_ref_img
    aControls.footprint = applier.BOUNDS_FROM_REFERENCE
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    print("Calculating Stats Image.")
    applier.apply(_calcBasicStats, infiles, outfiles, otherargs, controls=aControls)
    print("Completed")

    use_no_data = True
    if no_data_val is None:
        no_data_val = 0.0
        use_no_data = False

    rsgislib.imageutils.pop_img_stats(
        output_img, use_no_data=use_no_data, no_data_val=no_data_val, calc_pyramids=True
    )


def normalise_image_band(
    input_img: str, band: int, output_img: str, gdal_format: str = "KEA"
):
    """
    Perform a simple normalisation a single image band (val - min)/range.

    :param input_img: The input image file.
    :param band: the image band (starts at 1) to be normalised
    :param output_img: the output image file (will just be a single band).
    :param gdal_format: The output image format.

    """
    import rsgislib
    import rsgislib.imageutils

    no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img, band)
    use_no_data_val = True
    if no_data_val is None:
        use_no_data_val = False
        no_data_val = 0.0

    band_min, band_max = get_img_band_min_max(
        input_img, band, use_no_data_val, no_data_val
    )

    band_defns = [BandDefn("b1", input_img, band)]
    band_range = band_max - band_min
    exp = "(b1=={0})?0.0:(b1-{1})/{2}".format(no_data_val, band_min, band_range)
    band_math(output_img, exp, gdal_format, rsgislib.TYPE_32FLOAT, band_defns)
    rsgislib.imageutils.pop_img_stats(
        output_img, use_no_data=True, no_data_val=0.0, calc_pyramids=True
    )


def recode_int_raster(
    input_img: str,
    output_img: str,
    recode_dict: Dict[int, int],
    keep_vals_not_in_dict: bool = True,
    gdalformat: str = "KEA",
    datatype: int = rsgislib.TYPE_32INT,
):
    """
    A function recodes an input image. Assuming image only has a single image band so
    it will be band 1 which is recoded. The recode is provided as a dict where the key
    is the value to be recoded and the value of the dict is the output value.

    :param input_img: Input image file.
    :param output_img: Output image file.
    :param recode_dict: dict for recode (key: int to be recoded, value: int
                        to recode to)
    :param keep_vals_not_in_dict: boolean whether pixels not being recoded should be
                              copied to the output (True) or whether only those pixels
                              recoded should be outputted (False) (default: True)
    :param gdalformat: output file format (default: KEA)
    :param datatype: is a rsgislib.TYPE_* value providing the data type of
                     the output image.

    """
    from rios import applier

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    # Generated the combined mask.
    infiles = applier.FilenameAssociations()
    infiles.inimage = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.np_dtype = rsgislib.get_numpy_datatype(datatype)
    otherargs.keep_vals_not_in_dict = keep_vals_not_in_dict
    otherargs.recode_dict = recode_dict
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = False
    aControls.calcStats = False

    def _recode(info, inputs, outputs, otherargs):
        out_arr = numpy.zeros_like(inputs.inimage[0], dtype=otherargs.np_dtype)
        if otherargs.keep_vals_not_in_dict:
            numpy.copyto(out_arr, inputs.inimage[0], casting="same_kind")

        for rc_val in recode_dict:
            out_arr[inputs.inimage[0] == rc_val] = recode_dict[rc_val]
        outputs.outimage = numpy.expand_dims(out_arr, axis=0)

    applier.apply(_recode, infiles, outfiles, otherargs, controls=aControls)


def calc_fill_regions_knn(
    in_ref_img: str,
    ref_no_data: int,
    in_fill_regions_img: str,
    fill_region_val: int,
    output_img: str,
    k: int = 5,
    summary: int = rsgislib.SUMTYPE_MODE,
    gdalformat: str = "KEA",
    datatype: int = rsgislib.TYPE_32INT,
):
    """
    A function will fills regions (defined by having a value == fill_region_val) of
    the in_fill_regions_img image using a KNN approach based of pixels within the
    in_ref_img image. The KNN distance is the spatial distance based off the
    pixel locations.

    The in_ref_img and in_fill_regions_img must both be in the same projection.

    :param in_ref_img: The reference image (can be a different resolution) from
                    which the KNN will be trained.
    :param ref_no_data: The no data value used within the reference image.
    :param in_fill_regions_img: A mask image defining the regions to be filled.
    :param fill_region_val: The pixel value specifying the pixels in the
                            in_fill_regions_img image for the KNN value will
                            be calculated.
    :param output_img: The output image file name and path.
    :param k: The k parameter of the KNN.
    :param summary: Summary method (rsgislib.SUMTYPE_*) to be used (Default is Mode).
                    Options: Mode, Median, Sum, Mean, Range, Min, Max.
    :param gdalformat: output file format (default: KEA)
    :param datatype: is a rsgislib.TYPE_* value providing the data type of the
                     output image.

    """
    import rtree
    import scipy
    import scipy.stats
    from rios import applier

    import rsgislib.imageutils

    if not rsgislib.imageutils.do_gdal_layers_have_same_proj(
        in_ref_img, in_fill_regions_img
    ):
        raise rsgislib.RSGISPyException(
            "The reference image and fill regions image "
            "do not have the same projection."
        )

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    x_res, y_res = rsgislib.imageutils.get_img_res(in_ref_img)
    if x_res < 0:
        x_res *= -1
    if y_res < 0:
        y_res *= -1

    print("Building Spatial Index...")
    # Create the index.
    kd_idx = rtree.index.Index()

    # Populate Index
    infiles = applier.FilenameAssociations()
    infiles.in_ref_img = in_ref_img
    outfiles = applier.FilenameAssociations()
    otherargs = applier.OtherInputs()
    otherargs.kd_idx = kd_idx
    otherargs.ref_no_data = ref_no_data
    otherargs.w_box = x_res / 2
    otherargs.h_box = y_res / 2
    otherargs.n = 0
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar

    def _retrieve_idx_info(info, inputs, outputs, otherargs):
        xBlock, yBlock = info.getBlockCoordArrays()
        xBlock = xBlock.flatten()
        yBlock = yBlock.flatten()
        ref_data = inputs.in_ref_img[0].flatten()

        x_coords = xBlock[ref_data != otherargs.ref_no_data]
        y_coords = yBlock[ref_data != otherargs.ref_no_data]
        val_data = ref_data[ref_data != otherargs.ref_no_data]

        left_coords = x_coords - otherargs.w_box
        right_coords = x_coords + otherargs.w_box

        top_coords = y_coords + otherargs.h_box
        bot_coords = y_coords - otherargs.h_box

        coords = numpy.stack(
            (left_coords, bot_coords, right_coords, top_coords), axis=1
        )
        for coord, cls in zip(coords, val_data):
            otherargs.kd_idx.insert(otherargs.n, coord, cls)
            otherargs.n += 1

    applier.apply(_retrieve_idx_info, infiles, outfiles, otherargs, controls=aControls)

    print("\nFilling Image Regions using KNN (k={})".format(k))
    x_res, y_res = rsgislib.imageutils.get_img_res(in_fill_regions_img)
    if x_res < 0:
        x_res *= -1
    if y_res < 0:
        y_res *= -1

    # Apply KNN fill.
    infiles = applier.FilenameAssociations()
    infiles.inimage = in_fill_regions_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.kd_idx = kd_idx
    otherargs.k = k
    otherargs.fill_region_val = fill_region_val
    otherargs.np_dtype = rsgislib.get_numpy_datatype(datatype)
    otherargs.w_box = x_res / 2
    otherargs.h_box = y_res / 2
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = False
    aControls.calcStats = False

    def _knn_fill_regions(info, inputs, outputs, otherargs):
        out_arr = numpy.zeros_like(inputs.inimage[0], dtype=otherargs.np_dtype)
        xBlock, yBlock = info.getBlockCoordArrays()
        data_shp = out_arr.shape
        for i in range(data_shp[0]):
            for j in range(data_shp[1]):
                if inputs.inimage[0, i, j] == otherargs.fill_region_val:
                    bbox = (
                        xBlock[i, j] - otherargs.w_box,
                        yBlock[i, j] - otherargs.h_box,
                        xBlock[i, j] + otherargs.w_box,
                        yBlock[i, j] + otherargs.h_box,
                    )

                    vals = list(
                        otherargs.kd_idx.nearest(bbox, otherargs.k, objects="raw")
                    )
                    int_vals = list()
                    for val in vals:
                        int_vals.append(int(val))
                    if summary == rsgislib.SUMTYPE_MODE:
                        out_arr[i, j] = scipy.stats.mode(int_vals, keepdims=True).mode[
                            0
                        ]
                    elif summary == rsgislib.SUMTYPE_MEDIAN:
                        out_arr[i, j] = numpy.median(int_vals)
                    elif summary == rsgislib.SUMTYPE_MAX:
                        out_arr[i, j] = numpy.amax(int_vals)
                    elif summary == rsgislib.SUMTYPE_MIN:
                        out_arr[i, j] = numpy.amin(int_vals)
                    elif summary == rsgislib.SUMTYPE_MEAN:
                        out_arr[i, j] = numpy.mean(int_vals)
                    elif summary == rsgislib.SUMTYPE_SUM:
                        out_arr[i, j] = numpy.sum(int_vals)
                    elif summary == rsgislib.SUMTYPE_RANGE:
                        out_arr[i, j] = numpy.amax(int_vals) - numpy.amin(int_vals)
                    else:
                        raise rsgislib.RSGISPyException(
                            "Summary method not recognised/available."
                        )

        outputs.outimage = numpy.expand_dims(out_arr, axis=0)

    applier.apply(_knn_fill_regions, infiles, outfiles, otherargs, controls=aControls)
    print("Finished fill")


def are_imgs_equal(
    in_ref_img: str, in_cmp_img: str, prop_eql: float = 1.0, flt_dif: float = 0.0001
):
    """
    A function to check whether two images have equal pixel values within the spatial
    overlap. Note, if the two input images only have a partial overlap (i.e., one is
    a subset of the other) then they are still be equal if the overlapping region
    has matching pixel values.

    :param in_ref_img: The input reference image for the comparison.
    :param in_cmp_img: The input comparison image to be compared to the reference image.
    :param prop_eql: The proportion of pixels within the scene which need to be
                     identified as identical to return True. Range is 0 - 1. Default
                     is 1.0 (i.e., 100 % of pixels have to be identical to return True).
    :param flt_dif: A threshold for comparing two floating point numbers as being
                    identical - this avoids issues with rounding and the number of
                    decimal figures stored.
    :return: Boolean (match), float (proportion of pixels which matched)

    """
    from rios import applier

    import rsgislib.imageutils

    if rsgislib.imageutils.get_img_band_count(
        in_ref_img
    ) != rsgislib.imageutils.get_img_band_count(in_cmp_img):
        raise rsgislib.RSGISPyException(
            "The number of image bands is not the same between the two images."
        )

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    # Generated the combined mask.
    infiles = applier.FilenameAssociations()
    infiles.in_ref_img = in_ref_img
    infiles.in_cmp_img = in_cmp_img
    outfiles = applier.FilenameAssociations()
    otherargs = applier.OtherInputs()
    otherargs.flt_dif = flt_dif
    otherargs.n_pxls = 0.0
    otherargs.n_eq_pxls = 0.0
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar

    def _calcPropEqual(info, inputs, outputs, otherargs):
        ref_pxls = inputs.in_ref_img.flatten()
        cmp_pxls = inputs.in_cmp_img.flatten()

        cmp_pxls = cmp_pxls[~numpy.isnan(ref_pxls)]
        ref_pxls = ref_pxls[~numpy.isnan(ref_pxls)]

        ref_pxls = ref_pxls[~numpy.isnan(cmp_pxls)]
        cmp_pxls = cmp_pxls[~numpy.isnan(cmp_pxls)]

        pxl_diff = numpy.abs(ref_pxls - cmp_pxls)
        eql_pxls = numpy.where(pxl_diff < otherargs.flt_dif, 1, 0)

        otherargs.n_eq_pxls += eql_pxls.sum()
        otherargs.n_pxls += ref_pxls.shape[0]

    applier.apply(_calcPropEqual, infiles, outfiles, otherargs, controls=aControls)

    prop_pxls_eq = otherargs.n_eq_pxls / otherargs.n_pxls
    return (prop_pxls_eq >= prop_eql), prop_pxls_eq


def are_img_bands_equal(
    in_ref_img, img_ref_band, in_cmp_img, img_cmp_band, prop_eql=1.0, flt_dif=0.0001
):
    """
    A function to check whether two image bands have equal pixel values within the
    spatial overlap. Note, if the two input images only have a partial overlap
    (i.e., one is a subset of the other) then they are still be equal if the
    overlapping region has matching pixel values.

    :param in_ref_img: The input reference image for the comparison.
    :param img_ref_band: The band from the reference image
    :param in_cmp_img: The input comparison image to be compared to the reference image.
    :param img_cmp_band: The band from the comparison image
    :param prop_eql: The proportion of pixels within the scene which need to be
                     identified as identical to return True. Range is 0 - 1. Default
                     is 1.0 (i.e., 100 % of pixels have to be identical to return True).
    :param flt_dif: A threshold for comparing two floating point numbers as being
                    identical - this avoids issues with rounding and the number of
                    decimal figures stored.
    :return: Boolean (match), float (proportion of pixels which matched)

    """
    from rios import applier

    import rsgislib.imageutils

    n_ref_bands = rsgislib.imageutils.get_img_band_count(in_ref_img)
    n_cmp_bands = rsgislib.imageutils.get_img_band_count(in_cmp_img)

    if (img_ref_band < 1) or (img_ref_band > n_ref_bands):
        raise rsgislib.RSGISPyException(
            "The specified band is not within the reference image."
        )

    if (img_cmp_band < 1) or (img_cmp_band > n_cmp_bands):
        raise rsgislib.RSGISPyException(
            "The specified band is not within the comparison image."
        )

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    # Generated the combined mask.
    infiles = applier.FilenameAssociations()
    infiles.in_ref_img = in_ref_img
    infiles.in_cmp_img = in_cmp_img
    outfiles = applier.FilenameAssociations()
    otherargs = applier.OtherInputs()
    otherargs.ref_band_idx = img_ref_band - 1
    otherargs.cmp_band_idx = img_cmp_band - 1
    otherargs.flt_dif = flt_dif
    otherargs.n_pxls = 0.0
    otherargs.n_eq_pxls = 0.0
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar

    def _calcBandPropEqual(info, inputs, outputs, otherargs):
        ref_pxls = inputs.in_ref_img[otherargs.ref_band_idx, ...].flatten()
        cmp_pxls = inputs.in_cmp_img[otherargs.cmp_band_idx, ...].flatten()

        cmp_pxls = cmp_pxls[~numpy.isnan(ref_pxls)]
        ref_pxls = ref_pxls[~numpy.isnan(ref_pxls)]

        ref_pxls = ref_pxls[~numpy.isnan(cmp_pxls)]
        cmp_pxls = cmp_pxls[~numpy.isnan(cmp_pxls)]

        pxl_diff = numpy.abs(ref_pxls - cmp_pxls)
        eql_pxls = numpy.where(pxl_diff < otherargs.flt_dif, 1, 0)

        otherargs.n_eq_pxls += eql_pxls.sum()
        otherargs.n_pxls += ref_pxls.shape[0]

    applier.apply(_calcBandPropEqual, infiles, outfiles, otherargs, controls=aControls)

    prop_pxls_eq = otherargs.n_eq_pxls / otherargs.n_pxls
    return (prop_pxls_eq >= prop_eql), prop_pxls_eq


def calc_split_win_thresholds(
    input_img: str,
    win_size: int = 500,
    thres_meth: int = rsgislib.THRES_METH_OTSU,
    output_file: str = None,
    no_data_val: float = None,
    lower_valid: float = None,
    upper_valid: float = None,
    min_n_vals: int = 100,
    **thres_kwrds,
) -> Dict[int, List[float]]:
    """
    A function which undertakes a split window based thresholding where a threshold
    is calculated for each window (tile) within the scene and a list of thresholds
    is returned from which a single or range of thresholds can be calculated.

    If you know the range of values within which the threshold should exist it
    would probably be useful to use the upper and lower bounds thresholds to
    limit the data used within the thresholding and therefore the thresholds
    returned are more likely to be useful...

    :param input_img: the input image
    :param win_size: the window size
    :param thres_meth: the thresholding method rsgislib.THRES_METH_*. Default if otsu
                       thresholding. For details of other thresholding methods
                       see rsgislib.tools.stats for function descriptions.
    :param output_file: An optional JSON output file with a list of the thresholds
                        for each band within the input file.
    :param no_data_val: an optional no data value for the input image pixel values.
                        If provided these values will be ignored in the input image.
    :param lower_valid: a lower bounds for pixel values to be used for the analysis.
    :param upper_valid: an upper bounds for pixel values to be used for the analysis.
    :param min_n_vals: the minimum number of values used to calculate the threshold
                       within a window.
    :param thres_kwrds: some thresholding methods have arguments which need
                        to be provided. Use the keywords arguments to provide
                        those inputs. See rsgislib.tools.stats for function inputs.
    :return: dict of bands and a list of thresholds for the bands.

    """
    from rios import applier

    import rsgislib.imageutils
    import rsgislib.tools.stats
    import rsgislib.tools.utils

    def _calc_win_thres(info, inputs, outputs, otherargs):
        for n in range(otherargs.n_bands):
            data = inputs.image[n].flatten()
            if no_data_val is not None:
                data = data[data != otherargs.no_data_val]
            if lower_valid is not None:
                data = data[data > otherargs.lower_valid]
            if upper_valid is not None:
                data = data[data < otherargs.upper_valid]
            if data.shape[0] > otherargs.min_n_vals:
                if thres_meth == rsgislib.THRES_METH_OTSU:
                    band_thres = rsgislib.tools.stats.calc_otsu_threshold(data)
                elif thres_meth == rsgislib.THRES_METH_YEN:
                    band_thres = rsgislib.tools.stats.calc_yen_threshold(data)
                elif thres_meth == rsgislib.THRES_METH_ISODATA:
                    band_thres = rsgislib.tools.stats.calc_isodata_threshold(data)
                elif thres_meth == rsgislib.THRES_METH_CROSS_ENT:
                    band_thres = rsgislib.tools.stats.calc_hist_cross_entropy(
                        data, **otherargs.thres_kwrds
                    )
                elif thres_meth == rsgislib.THRES_METH_LI:
                    band_thres = rsgislib.tools.stats.calc_li_threshold(
                        data, **otherargs.thres_kwrds
                    )
                elif thres_meth == rsgislib.THRES_METH_KURT_SKEW:
                    band_thres = rsgislib.tools.stats.calc_kurt_skew_threshold(
                        data, **otherargs.thres_kwrds
                    )
                else:
                    raise rsgislib.RSGISPyException("Thresholding method unknown.")
                otherargs.band_thresholds[n + 1].append(band_thres)

    n_bands = rsgislib.imageutils.get_img_band_count(input_img)
    band_thresholds = dict()
    for n in range(n_bands):
        band_thresholds[n + 1] = list()

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    outfiles = applier.FilenameAssociations()
    otherargs = applier.OtherInputs()
    otherargs.no_data_val = no_data_val
    otherargs.n_bands = n_bands
    otherargs.thres_meth = thres_meth
    otherargs.lower_valid = lower_valid
    otherargs.upper_valid = upper_valid
    otherargs.min_n_vals = min_n_vals
    otherargs.thres_kwrds = thres_kwrds
    otherargs.band_thresholds = band_thresholds
    otherargs.out_arr = None
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.windowxsize = win_size
    aControls.windowysize = win_size

    applier.apply(_calc_win_thres, infiles, outfiles, otherargs, controls=aControls)

    if output_file is not None:
        rsgislib.tools.utils.write_dict_to_json(band_thresholds, output_file)

    return band_thresholds


def count_imgs_int_val_occur(
    input_imgs: List[str], output_img: str, bin_vals: List[int], gdalformat: str = "KEA"
):
    """
    Function which calculates the histogram for each pixel across all the input
    images and bands within the those images.

    Note, when the list of input images is very large (i.e., over 200) an error
    cannot occur but if you stack the images into a VRT then more images can be
    processed.

    :param input_imgs: the list of images
    :param output_img: the output image file name and path. The number
                       of bands with be equal to the length of the
                       bin_vals lists.
    :param bin_vals: list of integer bin used to calculate the histograms
    :param gdalformat: the GDAL image file format of the output image file.

    """
    from rios import applier

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.images = input_imgs
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.bin_vals = bin_vals
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyCalcPxlValCount(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        image_data = numpy.concatenate(inputs.images, axis=0).astype(numpy.int32)
        val_count_arrs = []
        for val in otherargs.bin_vals:
            val_count_arrs.append(
                numpy.expand_dims(
                    numpy.count_nonzero(image_data == val, axis=0), axis=0
                )
            )
        outputs.outimage = numpy.concatenate(val_count_arrs, axis=0).astype(numpy.int32)

    applier.apply(
        _applyCalcPxlValCount, infiles, outfiles, otherargs, controls=aControls
    )


def create_categories_sgl_band(
    input_img: str,
    output_img: str,
    recode_lut: List[Tuple[int, Tuple[float, float]]],
    img_band: int = 1,
    gdalformat: str = "KEA",
    datatype: int = rsgislib.TYPE_8UINT,
    backgrd_val: int = 0,
):
    """
    A function which categories a continuous image band based on a look up
    table (LUT) provided. The LUT should be a list specifying the output
    value and lower (>=) and upper (<) thresholds for that category.
    For example, (1, (10, 20)). If you do not want to specify a lower or
    upper value then use math.nan. For example, (2, (math.nan, 10))
    or (3, (20, math.nan)).

    :param input_img: The input image file path.
    :param output_img: The output image where the distance has been recoded to
                       categories using the recode_lut.
    :param recode_lut: The recoding LUT specifying the categories to split the
                       continuous band into.
    :param img_band: The image band within the image (note. band indexes start at 1).
                     Default = 1.
    :param gdalformat: the output image file format (default: KEA)
    :param datatype: the output image file data type (default: rsgislib.TYPE_8UINT)
    :param backgrd_val: The background value used when recoding the distance image.
                        i.e., if a pixel does not fall into any of the categories
                        specified then it will be given this value.

    """
    recode_exp = ""
    first = True
    for lut_ent in recode_lut:
        if math.isnan(lut_ent[1][0]):
            ent_exp = f"(b{img_band} < {lut_ent[1][1]})?{lut_ent[0]}"
        elif math.isnan(lut_ent[1][1]):
            ent_exp = f"(b{img_band} >= {lut_ent[1][0]})?{lut_ent[0]}"
        else:
            ent_exp = (
                f"(b{img_band} >= {lut_ent[1][0]})&&"
                f"(b{img_band} < {lut_ent[1][1]})?{lut_ent[0]}"
            )

        if first:
            recode_exp = ent_exp
            first = False
        else:
            recode_exp = f"{recode_exp}:{ent_exp}"

    recode_exp = f"{recode_exp}:{backgrd_val}"
    image_math(input_img, output_img, recode_exp, gdalformat, datatype)


def calc_img_correlation(
    in_a_img: str,
    in_b_img: str,
    img_a_band: int,
    img_b_band: int,
    img_a_no_data: float = None,
    img_b_no_data: float = None,
    corr_stat_method: int = rsgislib.STATS_CORR_PEARSONS,
) -> (float, float):
    """
    A function which calculates the correlation between two input image bands.

    :param in_a_img: The path to input image A.
    :param in_b_img: The path to input image B.
    :param img_a_band: The band within input image A
    :param img_b_band: The band within input image B
    :param img_a_no_data: The no data value in image A.
                          If None then read from image header.
    :param img_b_no_data: The no data value in image B.
                          If None then read from image header.
    :param corr_stat_method: The correlation method rsgislib.STATS_CORR_.
                             Default: rsgislib.STATS_CORR_PEARSONS
    :return: correlation coefficient and p value
    """
    import rsgislib.tools.geometrytools
    import rsgislib.imageutils
    import scipy.stats

    in_a_img_band_count = rsgislib.imageutils.get_img_band_count(in_a_img)
    if (img_a_band < 1) or (img_a_band > in_a_img_band_count):
        raise rsgislib.RSGISPyException(
            f"The band specified ({img_a_band}) for image A is not within the image."
        )
    in_b_img_band_count = rsgislib.imageutils.get_img_band_count(in_b_img)
    if (img_b_band < 1) or (img_b_band > in_b_img_band_count):
        raise rsgislib.RSGISPyException(
            f"The band specified ({img_b_band}) for image B is not within the image."
        )

    if img_a_no_data is None:
        img_a_no_data = rsgislib.imageutils.get_img_no_data_value(in_a_img, img_a_band)
    if img_a_no_data is None:
        raise rsgislib.RSGISPyException(
            "A no data value is not available for image A - please specify."
        )

    if img_b_no_data is None:
        img_b_no_data = rsgislib.imageutils.get_img_no_data_value(in_b_img, img_b_band)
    if img_b_no_data is None:
        raise rsgislib.RSGISPyException(
            "A no data value is not available for image B - please specify."
        )

    img_a_bbox = rsgislib.imageutils.get_img_bbox(in_a_img)
    img_b_bbox = rsgislib.imageutils.get_img_bbox(in_b_img)
    bbox_sub = rsgislib.tools.geometrytools.bbox_intersection(img_a_bbox, img_b_bbox)

    img_a_arr = rsgislib.imageutils.get_img_band_pxl_data(
        in_a_img, img_band=img_a_band, bbox_sub=bbox_sub
    )
    img_a_arr = img_a_arr.flatten()

    img_b_arr = rsgislib.imageutils.get_img_band_pxl_data(
        in_b_img, img_band=img_b_band, bbox_sub=bbox_sub
    )
    img_b_arr = img_b_arr.flatten()

    finite_pxls = numpy.logical_and(
        numpy.isfinite(img_a_arr), numpy.isfinite(img_b_arr)
    )
    valid_rng_pxls = numpy.logical_and(
        img_a_arr != img_a_no_data, img_b_arr != img_b_no_data
    )
    vld_pxls = numpy.logical_and(valid_rng_pxls, finite_pxls)

    img_a_arr = img_a_arr[vld_pxls]
    img_b_arr = img_b_arr[vld_pxls]

    if (len(img_a_arr) > 5) and (len(img_b_arr) > 5):
        if corr_stat_method == rsgislib.STATS_CORR_PEARSONS:
            imgs_corr_coeff, imgs_corr_p = scipy.stats.pearsonr(img_a_arr, img_b_arr)
        elif corr_stat_method == rsgislib.STATS_CORR_SPEARMAN:
            imgs_corr_coeff, imgs_corr_p = scipy.stats.spearmanr(img_a_arr, img_b_arr)
        elif corr_stat_method == rsgislib.STATS_CORR_KENDALL_TAU:
            imgs_corr_coeff, imgs_corr_p = scipy.stats.kendalltau(img_a_arr, img_b_arr)
        elif corr_stat_method == rsgislib.STATS_CORR_POINT_BISERIAL:
            imgs_corr_coeff, imgs_corr_p = scipy.stats.pointbiserialr(
                img_a_arr, img_b_arr
            )
        else:
            raise rsgislib.RSGISPyException(
                "Do not recognise the correlation method specified"
            )
        imgs_corr_coeff = float(imgs_corr_coeff)
        imgs_corr_p = float(imgs_corr_p)
    else:
        imgs_corr_coeff = None
        imgs_corr_p = None

    return imgs_corr_coeff, imgs_corr_p


def calc_img_mutual_info(
    in_a_img: str,
    in_b_img: str,
    img_a_band: int,
    img_b_band: int,
    img_a_no_data: float = None,
    img_b_no_data: float = None,
    hist_bins=None,
    img_a_min: float = None,
    img_a_max: float = None,
    img_b_min: float = None,
    img_b_max: float = None,
) -> float:
    """
    A function which calculates the mutual information metric
    between two input image bands.

    :param in_a_img: The path to input image A.
    :param in_b_img: The path to input image B.
    :param img_a_band: The band within input image A
    :param img_b_band: The band within input image B
    :param img_a_no_data: The no data value in image A.
                          If None then read from image header.
    :param img_b_no_data: The no data value in image B.
                          If None then read from image header.
    :param hist_bins: Inputted into numpy.histogram2d function. This
                      should be an int or array_like
                      or [int, int] or [array, array]. Optional.
    :param img_a_min: The minimum image A data value to be included in the calculation.
    :param img_a_max: The maximum image A data value to be included in the calculation.
    :param img_b_min: The minimum image B data value to be included in the calculation.
    :param img_b_max: The maximum image B data value to be included in the calculation.
    :return: mutual information value (float)
    """
    from sklearn.metrics import mutual_info_score
    import rsgislib.tools.geometrytools
    import rsgislib.imageutils

    in_a_img_band_count = rsgislib.imageutils.get_img_band_count(in_a_img)
    if (img_a_band < 1) or (img_a_band > in_a_img_band_count):
        raise rsgislib.RSGISPyException(
            f"The band specified ({img_a_band}) for image A is not within the image."
        )
    in_b_img_band_count = rsgislib.imageutils.get_img_band_count(in_b_img)
    if (img_b_band < 1) or (img_b_band > in_b_img_band_count):
        raise rsgislib.RSGISPyException(
            f"The band specified ({img_b_band}) for image B is not within the image."
        )

    if img_a_no_data is None:
        img_a_no_data = rsgislib.imageutils.get_img_no_data_value(in_a_img, img_a_band)
    if img_a_no_data is None:
        raise rsgislib.RSGISPyException(
            "A no data value is not available for image A - please specify."
        )

    if img_b_no_data is None:
        img_b_no_data = rsgislib.imageutils.get_img_no_data_value(in_b_img, img_b_band)
    if img_b_no_data is None:
        raise rsgislib.RSGISPyException(
            "A no data value is not available for image B - please specify."
        )

    img_a_bbox = rsgislib.imageutils.get_img_bbox(in_a_img)
    img_b_bbox = rsgislib.imageutils.get_img_bbox(in_b_img)
    bbox_sub = rsgislib.tools.geometrytools.bbox_intersection(img_a_bbox, img_b_bbox)

    img_a_arr = rsgislib.imageutils.get_img_band_pxl_data(
        in_a_img, img_band=img_a_band, bbox_sub=bbox_sub
    )
    img_a_arr = img_a_arr.flatten()
    img_b_arr = rsgislib.imageutils.get_img_band_pxl_data(
        in_b_img, img_band=img_b_band, bbox_sub=bbox_sub
    )
    img_b_arr = img_b_arr.flatten()

    finite_pxls = numpy.logical_and(
        numpy.isfinite(img_a_arr), numpy.isfinite(img_b_arr)
    )
    valid_rng_pxls = numpy.logical_and(
        img_a_arr != img_a_no_data, img_b_arr != img_b_no_data
    )
    vld_pxls = numpy.logical_and(valid_rng_pxls, finite_pxls)

    img_a_arr = img_a_arr[vld_pxls]
    img_b_arr = img_b_arr[vld_pxls]

    if (
        (img_a_min is not None)
        and (img_a_max is not None)
        and (img_b_min is not None)
        and (img_b_max is not None)
    ):
        msk_img_a_pxls = numpy.logical_and(
            (img_a_arr > img_a_min),
            (img_a_arr < img_a_max),
        )

        msk_img_b_pxls = numpy.logical_and(
            img_b_arr > img_b_min,
            img_b_arr < img_b_max,
        )
        msk_val_pxls = numpy.logical_and(msk_img_a_pxls, msk_img_b_pxls)

        img_a_arr = img_a_arr[msk_val_pxls]
        img_b_arr = img_b_arr[msk_val_pxls]

    if (len(img_a_arr) > 5) and (len(img_b_arr) > 5):
        c_xy = numpy.histogram2d(img_a_arr, img_b_arr, hist_bins)[0]
        mi = mutual_info_score(None, None, contingency=c_xy)
        imgs_mi = float(mi)
    else:
        imgs_mi = None

    return imgs_mi


def calc_img_earth_move_dist(
    in_a_img: str,
    in_b_img: str,
    img_a_band: int,
    img_b_band: int,
    img_a_no_data: float = None,
    img_b_no_data: float = None,
    hist_a_bins: int = None,
    hist_b_bins: int = None,
    use_glb_range: bool = None,
) -> float:
    """
    A function which calculates the earth movers distance
    between two input image bands.

    :param in_a_img: The path to input image A.
    :param in_b_img: The path to input image B.
    :param img_a_band: The band within input image A
    :param img_b_band: The band within input image B
    :param img_a_no_data: The no data value in image A.
                          If None then read from image header.
    :param img_b_no_data: The no data value in image B.
                          If None then read from image header.
    :param hist_a_bins: Number of bins used for the histogram created for image band A
    :param hist_b_bins: Number of bins used for the histogram created for image band B
    :param use_glb_range: Use the global range of the two image bands for both
                          histograms. Therefore, the range of the histograms
                          are the same.
    :return: earth movers distance (float)
    """
    import scipy.stats
    import rsgislib.tools.geometrytools
    import rsgislib.imageutils

    in_a_img_band_count = rsgislib.imageutils.get_img_band_count(in_a_img)
    if (img_a_band < 1) or (img_a_band > in_a_img_band_count):
        raise rsgislib.RSGISPyException(
            f"The band specified ({img_a_band}) for image A is not within the image."
        )
    in_b_img_band_count = rsgislib.imageutils.get_img_band_count(in_b_img)
    if (img_b_band < 1) or (img_b_band > in_b_img_band_count):
        raise rsgislib.RSGISPyException(
            f"The band specified ({img_b_band}) for image B is not within the image."
        )

    if img_a_no_data is None:
        img_a_no_data = rsgislib.imageutils.get_img_no_data_value(in_a_img, img_a_band)
    if img_a_no_data is None:
        raise rsgislib.RSGISPyException(
            "A no data value is not available for image A - please specify."
        )

    if img_b_no_data is None:
        img_b_no_data = rsgislib.imageutils.get_img_no_data_value(in_b_img, img_b_band)
    if img_b_no_data is None:
        raise rsgislib.RSGISPyException(
            "A no data value is not available for image B - please specify."
        )

    img_a_bbox = rsgislib.imageutils.get_img_bbox(in_a_img)
    img_b_bbox = rsgislib.imageutils.get_img_bbox(in_b_img)
    bbox_sub = rsgislib.tools.geometrytools.bbox_intersection(img_a_bbox, img_b_bbox)

    img_a_arr = rsgislib.imageutils.get_img_band_pxl_data(
        in_a_img, img_band=img_a_band, bbox_sub=bbox_sub
    )
    img_a_arr = img_a_arr.flatten()
    img_b_arr = rsgislib.imageutils.get_img_band_pxl_data(
        in_b_img, img_band=img_b_band, bbox_sub=bbox_sub
    )
    img_b_arr = img_b_arr.flatten()

    finite_pxls = numpy.logical_and(
        numpy.isfinite(img_a_arr), numpy.isfinite(img_b_arr)
    )
    valid_rng_pxls = numpy.logical_and(
        img_a_arr != img_a_no_data, img_b_arr != img_b_no_data
    )
    vld_pxls = numpy.logical_and(valid_rng_pxls, finite_pxls)

    img_a_arr = img_a_arr[vld_pxls]
    img_b_arr = img_b_arr[vld_pxls]

    if (len(img_a_arr) > 5) and (len(img_b_arr) > 5):
        img_range = None
        if use_glb_range:
            img_min = numpy.min(img_a_arr)
            img_max = numpy.max(img_a_arr)

            img_b_min = numpy.min(img_b_arr)
            img_b_max = numpy.max(img_b_arr)

            if img_b_min < img_min:
                img_min = img_b_min
            if img_b_max > img_max:
                img_max = img_b_max

            img_range = [img_min, img_max]

        hist_a, bin_edges_a = numpy.histogram(
            img_a_arr, bins=hist_a_bins, range=img_range
        )
        hist_b, bin_edges_b = numpy.histogram(
            img_b_arr, bins=hist_b_bins, range=img_range
        )

        hist_a_prob = hist_a / len(img_a_arr)
        hist_b_prob = hist_b / len(img_b_arr)

        em_val = scipy.stats.wasserstein_distance(hist_a_prob, hist_b_prob)
        imgs_em = float(em_val)
    else:
        imgs_em = None

    return imgs_em


def calc_img_min_max(input_img: str, no_data_val: float = None) -> numpy.array:
    """
    Function which calculates the minimum and maximum value of each
    image band in the input image.

    :param input_img: the input image file path.
    :param no_data_val: the no data value
    :return: two arrays min_vals, max_vals.

    """
    from rios import applier
    import rsgislib.imageutils

    if no_data_val is None:
        no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

        if no_data_val is None:
            raise rsgislib.RSGISPyException(
                "A no data value needs to specified "
                "either passed to the function or read "
                "from the input image header."
            )

    datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
    numpyDT = rsgislib.get_numpy_datatype(datatype)

    n_bands = rsgislib.imageutils.get_img_band_count(input_img)
    min_vals = numpy.zeros(n_bands, dtype=numpyDT)
    max_vals = numpy.zeros(n_bands, dtype=numpyDT)
    first_arr = numpy.ones(n_bands, dtype=bool)

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.input_img = input_img
    outfiles = applier.FilenameAssociations()
    otherargs = applier.OtherInputs()
    otherargs.no_data_val = no_data_val
    otherargs.n_bands = n_bands
    otherargs.min_vals = min_vals
    otherargs.max_vals = max_vals
    otherargs.no_data_val = no_data_val
    otherargs.first_arr = first_arr
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar

    def _applyCalcImgMinMax(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        for n in range(otherargs.n_bands):
            data = inputs.input_img[n].flatten()
            data = data[data != otherargs.no_data_val]
            if data.shape[0] > 0:
                data_max = data.max()
                data_min = data.min()

                if otherargs.first_arr[n]:
                    otherargs.min_vals[n] = data_min
                    otherargs.max_vals[n] = data_max
                    otherargs.first_arr[n] = False
                else:
                    if data_min < otherargs.min_vals[n]:
                        otherargs.min_vals[n] = data_min
                    if data_max > otherargs.max_vals[n]:
                        otherargs.max_vals[n] = data_max

    applier.apply(_applyCalcImgMinMax, infiles, outfiles, otherargs, controls=aControls)

    return min_vals, max_vals


def calc_img_mean(input_img: str, no_data_val: float = None) -> numpy.array:
    """
    Function which calculates the mean value of each
    image band in the input image.

    :param input_img: the input image file path.
    :param no_data_val: the no data value
    :return: array mean_vals

    """
    from rios import applier
    import rsgislib.imageutils

    if no_data_val is None:
        no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

        if no_data_val is None:
            raise rsgislib.RSGISPyException(
                "A no data value needs to specified "
                "either passed to the function or read "
                "from the input image header."
            )

    n_bands = rsgislib.imageutils.get_img_band_count(input_img)
    sum_vals = numpy.zeros(n_bands, dtype=float)
    n_vals = numpy.zeros(n_bands, dtype=int)

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.input_img = input_img
    outfiles = applier.FilenameAssociations()
    otherargs = applier.OtherInputs()
    otherargs.no_data_val = no_data_val
    otherargs.n_bands = n_bands
    otherargs.sum_vals = sum_vals
    otherargs.n_vals = n_vals
    otherargs.no_data_val = no_data_val
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar

    def _applyCalcImgMean(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        for n in range(otherargs.n_bands):
            data = inputs.input_img[n].flatten()
            data = data[data != otherargs.no_data_val]
            if data.shape[0] > 0:
                otherargs.sum_vals[n] += data.sum()
                otherargs.n_vals[n] += data.shape[0]

    applier.apply(_applyCalcImgMean, infiles, outfiles, otherargs, controls=aControls)

    return sum_vals / n_vals


def calc_img_stdev(
    input_img: str, no_data_val: float = None, mean_vals: numpy.array = None
) -> numpy.array:
    """
    Function which calculates the standard deviation value of each
    image band in the input image.

    :param input_img: the input image file path.
    :param no_data_val: the no data value
    :param mean_vals: if available an array with the mean values for each band.
                      If None then the mean values will be calculated using the
                      calc_img_mean function.
    :return: array stdev_vals

    """
    from rios import applier
    import rsgislib.imageutils

    if no_data_val is None:
        no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

        if no_data_val is None:
            raise rsgislib.RSGISPyException(
                "A no data value needs to specified "
                "either passed to the function or read "
                "from the input image header."
            )

    if mean_vals is None:
        mean_vals = calc_img_mean(input_img, no_data_val)

    n_bands = rsgislib.imageutils.get_img_band_count(input_img)

    if len(mean_vals) != n_bands:
        raise rsgislib.RSGISPyException(
            "The length of the mean_vals array must " "equal the number of image bands."
        )

    sum_vals = numpy.zeros(n_bands, dtype=float)
    n_vals = numpy.zeros(n_bands, dtype=int)

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.input_img = input_img
    outfiles = applier.FilenameAssociations()
    otherargs = applier.OtherInputs()
    otherargs.no_data_val = no_data_val
    otherargs.n_bands = n_bands
    otherargs.mean_vals = mean_vals
    otherargs.sum_vals = sum_vals
    otherargs.n_vals = n_vals
    otherargs.no_data_val = no_data_val
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar

    def _applyCalcImgStdev(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        for n in range(otherargs.n_bands):
            data = inputs.input_img[n].flatten()
            data = data[data != otherargs.no_data_val]
            if data.shape[0] > 0:
                otherargs.sum_vals[n] += numpy.power(
                    data - otherargs.mean_vals[n], 2
                ).sum()
                otherargs.n_vals[n] += data.shape[0]

    applier.apply(_applyCalcImgStdev, infiles, outfiles, otherargs, controls=aControls)

    return numpy.sqrt(sum_vals / n_vals)


def normalise_img_pxl_vals_py(
    input_img: str,
    output_img: str,
    norm_type: int = rsgislib.IMG_STRETCH_CUMULATIVE,
    gdalformat: str = "KEA",
    datatype: int = rsgislib.TYPE_32FLOAT,
    in_no_data_val: float = None,
    out_no_data_val: float = None,
    out_min: float = 0,
    out_max: float = 1,
    stch_min_max_vals: Union[Dict, List[Dict]] = None,
    stch_n_stdevs: float = 2.0,
    stch_cuml_low: float = 0.02,
    stch_cuml_upp: float = 0.98,
    clip_vals: bool = True,
) -> List[Dict]:
    """
    A function for normalising pixel values similar to rsgislib.imageutils.stretch_img
    and rsgislib.imageutils.normalise_img_pxl_vals but is a pure python implementation.
    It is intended that this function will replace the older C++ functions so use
    this for any new code.


    :param input_img: The path to the input image
    :param output_img: The path to the output image
    :param norm_type: The normalisation to be applied (e.g.,
                      rsgislib.IMG_STRETCH_CUMULATIVE). Options are:
                      IMG_STRETCH_STDEV, IMG_STRETCH_CUMULATIVE, IMG_STRETCH_LINEAR
                      and IMG_STRETCH_USER.
    :param gdalformat: The output file format (e.g., KEA)
    :param datatype: the data type of the output image.
    :param in_no_data_val: the input no data value.
    :param out_no_data_val: the output no data value.
    :param out_min: the minimum output pixel value
    :param out_max: the maximum output pixel value
    :param stch_min_max_vals: either a list of dicts each with a 'min' and 'max' key
                              specifying the min and max value for the normalisation
                              of each band. Or, if just a single band then provide
                              a single dict rather than a list. The number items in
                              the list must equal the number of bands within the
                              input image.
    :param stch_n_stdevs: if using IMG_STRETCH_STDEV then this is the number of
                          standard deviations from the mean
    :param stch_cuml_low: if using IMG_STRETCH_CUMULATIVE this is the lower percentile
    :param stch_cuml_upp: if using IMG_STRETCH_CUMULATIVE this is the upper percentile
    :param clip_vals: boolean specifying whether the output images pixel values should
                      be clipped to out_min and out_max. Values below out_min will be
                      given the value out_min and values above out_max will be given
                      the value out_max.
    :return: a list of dicts with min and max keys for each band. Can be inputted
             as stch_min_max_vals to applying to other images.
    """
    import rsgislib.imageutils
    from rios import applier

    n_bands = rsgislib.imageutils.get_img_band_count(input_img)

    if in_no_data_val is None:
        in_no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

        if in_no_data_val is None:
            raise rsgislib.RSGISPyException(
                "A no data value needs to specified "
                "either passed to the function or read "
                "from the input image header."
            )
    if out_no_data_val is None:
        out_no_data_val = in_no_data_val

    if norm_type == rsgislib.IMG_STRETCH_STDEV:
        if stch_n_stdevs < 0:
            raise rsgislib.RSGISPyException("stch_n_stdevs must be greater than 0.")

        img_mean_vals = calc_img_mean(input_img, no_data_val=in_no_data_val)
        img_stdev_vals = calc_img_stdev(
            input_img, no_data_val=in_no_data_val, mean_vals=img_mean_vals
        )
        img_min_vals, img_max_vals = calc_img_min_max(
            input_img=input_img,
            no_data_val=in_no_data_val,
        )

        stch_min_max_vals = list()
        for b_min, b_max, b_mean, b_stdev in zip(
            img_min_vals, img_max_vals, img_mean_vals, img_stdev_vals
        ):

            calcd_min = b_mean - (b_stdev * stch_n_stdevs)
            calcd_max = b_mean + (b_stdev * stch_n_stdevs)

            if calcd_min < b_min:
                calcd_min = b_min

            if calcd_max > b_max:
                calcd_max = b_max

            stch_min_max_vals.append({"min": calcd_min, "max": calcd_max})

    elif norm_type == rsgislib.IMG_STRETCH_CUMULATIVE:
        if (stch_cuml_low < 0) or (stch_cuml_low > 1.0):
            raise rsgislib.RSGISPyException(
                "stch_cuml_low must be greater than 0 and less than 1."
            )
        if (stch_cuml_upp < 0) or (stch_cuml_upp > 1.0):
            raise rsgislib.RSGISPyException(
                "stch_cuml_upp must be greater than 0 and less than 1."
            )

        low_band_percent = calc_band_percentile(
            input_img, stch_cuml_low, no_data_val=in_no_data_val
        )
        upp_band_percent = calc_band_percentile(
            input_img, stch_cuml_upp, no_data_val=in_no_data_val
        )

        stch_min_max_vals = list()
        for b_low, b_upp in zip(low_band_percent, upp_band_percent):
            stch_min_max_vals.append({"min": b_low, "max": b_upp})

    elif norm_type == rsgislib.IMG_STRETCH_LINEAR:
        img_min_vals, img_max_vals = calc_img_min_max(
            input_img=input_img,
            no_data_val=in_no_data_val,
        )
        stch_min_max_vals = list()
        for b_min, b_max in zip(img_min_vals, img_max_vals):
            stch_min_max_vals.append({"min": b_min, "max": b_max})

    elif norm_type == rsgislib.IMG_STRETCH_USER:
        if n_bands == 1:
            if isinstance(stch_min_max_vals, list):
                if len(stch_min_max_vals) != 1:
                    raise rsgislib.RSGISPyException(
                        "The input image has 1 band and therefore stch_min_max_vals "
                        "variable must be a dict or list of length 1 with "
                        "a single dict."
                    )
                else:
                    stch_min_max_vals = stch_min_max_vals[0]

            if not isinstance(stch_min_max_vals, dict):
                raise rsgislib.RSGISPyException(
                    "The input image has 1 band and therefore stch_min_max_vals "
                    "variable must be a dict or list of length 1 with "
                    "a single dict."
                )

            if ("min" not in stch_min_max_vals) or ("max" not in stch_min_max_vals):
                raise rsgislib.RSGISPyException(
                    "min and max keys must be provided within the dict"
                )

            stch_min_max_vals = [stch_min_max_vals]
        else:
            if not isinstance(stch_min_max_vals, list):
                raise rsgislib.RSGISPyException(
                    "There are more than 1 band and therefore "
                    "stch_min_max_vals variable must be a list."
                )

            if n_bands != len(stch_min_max_vals):
                raise rsgislib.RSGISPyException(
                    "length of stch_min_max_vals must be the same as the number "
                    "of bands in the input image."
                )

            for n in range(n_bands):
                if ("min" not in stch_min_max_vals[n]) or (
                    "max" not in stch_min_max_vals[n]
                ):
                    raise rsgislib.RSGISPyException(
                        "min and max keys must be provided within "
                        "the stch_min_max_vals dict"
                    )

    numpyDT = rsgislib.get_numpy_datatype(datatype)

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.input_img = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.output_img = output_img
    otherargs = applier.OtherInputs()
    otherargs.n_bands = n_bands
    otherargs.in_no_data_val = in_no_data_val
    otherargs.out_no_data_val = out_no_data_val
    otherargs.clip_vals = clip_vals
    otherargs.numpyDT = numpyDT
    otherargs.stch_min_max_vals = stch_min_max_vals
    otherargs.out_min = out_min
    otherargs.out_max = out_max
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyNormalisation(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        out_data_arr = numpy.zeros_like(inputs.input_img, dtype=float)

        for n in range(otherargs.n_bands):
            stch_range = (
                otherargs.stch_min_max_vals[n]["max"]
                - otherargs.stch_min_max_vals[n]["min"]
            )
            out_range = otherargs.out_max - otherargs.out_min

            out_data_arr[n] = (
                (
                    (inputs.input_img[n] - otherargs.stch_min_max_vals[n]["min"])
                    / stch_range
                )
                * out_range
            ) + otherargs.out_min

        if otherargs.clip_vals:
            out_data_arr[out_data_arr < otherargs.out_min] = otherargs.out_min
            out_data_arr[out_data_arr > otherargs.out_max] = otherargs.out_max

        out_data_arr[otherargs.in_no_data_val == inputs.input_img] = (
            otherargs.out_no_data_val
        )

        outputs.output_img = out_data_arr.astype(otherargs.numpyDT)

    applier.apply(_applyNormalisation, infiles, outfiles, otherargs, controls=aControls)

    return stch_min_max_vals


def calc_band_percentile_msk(
    input_img: str,
    in_msk_img: str,
    msk_val: int,
    percentiles: List[float],
    no_data_val: float = None,
):
    """
    A function to calculate the percentiles for all the bands in the
    input image for the pixels specified within the input image mask.

    :param input_img: Input image on which the percentiles will be calculated
    :param in_msk_img: Input mask for which the percentiles will be calculated
    :param msk_val: The image mask value
    :param percentiles: list of percentiles to be calculated
    :param no_data_val: no data value for the input image.

    """
    n_bands = rsgislib.imageutils.get_img_band_count(input_img)
    img_bands = list(range(1, n_bands + 1, 1))

    pxl_vals = rsgislib.imageutils.extract_img_pxl_vals_in_msk(
        input_img=input_img,
        img_bands=img_bands,
        in_msk_img=in_msk_img,
        img_mask_val=msk_val,
        no_data_val=no_data_val,
    )
    percent_vals = numpy.percentile(pxl_vals, percentiles, axis=0)

    return percent_vals


def calc_band_range_thres_msk(
    input_img: str,
    output_img: str,
    band_thres_dict: Dict[int, Tuple[float, float]],
    gdalformat: str = "KEA",
    combine_mthd: int = rsgislib.LOGIC_AND,
):
    """
    A function which applies a list of min/max thresholds to the image bands witin
    the input image. The thresholds are specified as a dictionary with the key being
    the band number (band numbering starts from 1) and the value being a tuple
    (min, max) where a true pixel value is > min and < max. A band can be used multiple
    times within the thresholds dictionary. The different thresholds produce a series
    of binary mask which are combined using either an 'and' (min) or a 'or' (max)
    operation.

    :param input_img: Input image path
    :param output_img: Output image path
    :param band_thres_dict: Dictionary of band thresholds using the key being the
                            band number and the value being a tuple (min, max).
    :param gdalformat: The output gdal format
    :param combine_mthd: the method of combining the band thresholds into a single
                         output image. Options are either rsgislib.LOGIC_AND (Default)
                         or rsgislib.LOGIC_OR

    """
    from rios import applier

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    # Generated the combined mask.
    infiles = applier.FilenameAssociations()
    infiles.inimage = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.band_thres_dict = band_thres_dict
    otherargs.combine_mthd = combine_mthd
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = False
    aControls.calcStats = False

    def _calc_thres_msk(info, inputs, outputs, otherargs):
        msk_arrs = []
        for band_n in band_thres_dict:
            band_arr = inputs.inimage[band_n - 1]
            msk_arrs.append(
                numpy.logical_and(
                    band_arr > band_thres_dict[band_n][0],
                    band_arr < band_thres_dict[band_n][1],
                )
            )

        msk_arr = numpy.array(msk_arrs, dtype=numpy.uint8)
        if otherargs.combine_mthd == rsgislib.LOGIC_OR:
            out_msk_arr = msk_arr.max(axis=0)
        elif otherargs.combine_mthd == rsgislib.LOGIC_AND:
            out_msk_arr = msk_arr.min(axis=0)
        else:
            raise rsgislib.RSGISPyException(
                "Do not recognise combine_mthd: should be "
                "rsgislib.LOGIC_OR or rsgislib.LOGIC_AND"
            )
        outputs.outimage = numpy.expand_dims(out_msk_arr, axis=0)

    applier.apply(_calc_thres_msk, infiles, outfiles, otherargs, controls=aControls)


def calc_img_band_pxl_percentiles(
    input_imgs: List[str],
    percentiles: List[float],
    output_img: str,
    gdalformat: str,
    no_data_val: float = 0,
):
    """
    Function which calculates percentiles on a per-pixel basis for a
    group of images on a per-band basis. Therefore, all the input images
    need to have the same number of bands and the number of output images
    will be the number of percentiles x the number of input bands. The
    output band order will be band 1 percentiles, band 2 percentiles, etc.
    band n percentiles are calculated on a per-band basis.

    :param input_imgs: the list of images - note all bands are used.
    :param percentiles: a list of percentiles (0-100) to be calculated.
    :param output_img: the output image file name and path
    :param gdalformat: the GDAL image file format of the output image file.

    """
    from rios import applier
    import rsgislib.imageutils

    n_bands = rsgislib.imageutils.get_img_band_count(input_imgs[0])
    for img in input_imgs:
        tmp_n_bands = rsgislib.imageutils.get_img_band_count(img)
        if tmp_n_bands != n_bands:
            raise rsgislib.RSGISPyException(
                "Input images have a different number of bands."
            )

    for percent in percentiles:
        if percent < 0:
            raise Exception(f"Percentile is less than 0 ({percent})")
        elif percent > 100:
            raise Exception(f"Percentile is greater than 100 ({percent})")

    datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_imgs[0])
    numpyDT = rsgislib.get_numpy_datatype(datatype)

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.images = input_imgs
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.no_data_val = no_data_val
    otherargs.n_bands = n_bands
    otherargs.numpyDT = numpyDT
    otherargs.percentiles = percentiles
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyCalcPercentile(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        out_percent_arrs = list()
        for band in range(otherargs.n_bands):
            band_arrs = list()
            for img in inputs.images:
                band_arrs.append(numpy.expand_dims(img[band], axis=0))
            img_band_arr = numpy.concatenate(band_arrs, axis=0).astype(numpy.float32)
            img_band_arr[img_band_arr == otherargs.no_data_val] = numpy.nan
            percentiles_arr = numpy.nanpercentile(
                img_band_arr, otherargs.percentiles, axis=0
            )
            percentiles_arr = numpy.nan_to_num(
                percentiles_arr, copy=False, nan=otherargs.no_data_val
            )
            out_percent_arrs.append(percentiles_arr)

        percentiles_arr = numpy.concatenate(out_percent_arrs, axis=0).astype(
            otherargs.numpyDT
        )

        if len(percentiles_arr.shape) == 2:
            outputs.outimage = numpy.expand_dims(percentiles_arr, axis=0)
        else:
            outputs.outimage = percentiles_arr

    applier.apply(
        _applyCalcPercentile, infiles, outfiles, otherargs, controls=aControls
    )
