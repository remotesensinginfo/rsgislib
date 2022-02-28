#!/usr/bin/env python
"""
The imagecalc module contains functions for performing a number of
calculating on images.
"""

import math
import os
from typing import List, Union

import numpy
from osgeo import gdal

import rsgislib

# import the C++ extension into this level
from ._imagecalc import *

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
    input_img,
    output_img,
    pxl_vals,
    img_band=1,
    max_dist=1000,
    no_data_val=1000,
    out_no_data_val=None,
    gdalformat="KEA",
    unit_geo=True,
    tmp_dir="tmp",
    tile_size=2000,
    n_cores=-1,
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
    import rsgislib.tools.utils
    import rsgislib.tools.filetools

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
    from rios.imagereader import ImageReader
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
    if img_band is not None:
        img_band_idx = img_band - 1

    numVals = len(vals)
    outVals = numpy.zeros(numVals, dtype=numpy.int64)

    reader = ImageReader(input_img)
    for (info, block) in reader:
        for idx in range(numVals):
            if img_band is None:
                outVals[idx] = outVals[idx] + (block == vals[idx]).sum()
            else:
                outVals[idx] = (
                    outVals[idx]
                    + (
                        block[
                            img_band_idx,
                        ]
                        == vals[idx]
                    ).sum()
                )

    return outVals


def get_unique_values(input_img, img_band=1):
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
    input_img, pxl_n_sample, no_data_val=None, out_matrix_file=None
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
    input_img,
    output_img,
    out_eigen_vec_file,
    n_comps=None,
    pxl_n_sample=100,
    gdalformat="KEA",
    datatype=rsgislib.TYPE_32FLOAT,
    no_data_val=None,
    calc_stats=True,
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
    input_img,
    output_img,
    n_comps=None,
    pxl_n_sample=100,
    in_img_no_data=None,
    tmp_dir="tmp",
    gdalformat="KEA",
    datatype=rsgislib.TYPE_32FLOAT,
    calc_stats=True,
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
    from sklearn.decomposition import PCA
    import rsgislib.tools.filetools
    import rsgislib.imageutils
    import shutil

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
    input_img, output_img, gdalformat, datatype, band_rescale_objs, trim_to_limits=True
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

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

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
    input_img, img_band, img_msk, msk_band, min_val, max_val, bin_width, msk_vals=None
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


def calc_imgs_pxl_mode(input_imgs, output_img, gdalformat, no_data_val=0):
    """
    Function which calculates the mode of a group of images.

    Warning, this function can be very slow!!!
    You probably want to use rsgislib.imagecalc.imagePixelColumnSummary

    :param input_imgs: the list of images
    :param output_img: the output image file name and path (will be
                       same dimensions as the input)
    :param gdalformat: the GDAL image file format of the output image file.

    """
    import rsgislib.imageutils
    import scipy.stats
    from rios import applier

    datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_imgs[0])
    numpyDT = rsgislib.get_numpy_datatype(datatype)

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.images = input_imgs
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.no_data_val = no_data_val
    otherargs.numpyDT = numpyDT
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
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
        outputs.outimage = mode_arr.astype(otherargs.numpyDT)

    applier.apply(_applyCalcMode, infiles, outfiles, otherargs, controls=aControls)


def calc_img_basic_stats_for_ref_region(
    in_ref_img, in_stats_imgs, output_img, gdalformat="KEA"
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
    import rsgislib.imageutils
    from rios import applier

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

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

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


def normalise_image_band(input_img, band, output_img, gdal_format="KEA"):
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
    input_img,
    output_img,
    recode_dict,
    keep_vals_not_in_dict=True,
    gdalformat="KEA",
    datatype=rsgislib.TYPE_32INT,
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

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

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
    aControls.drivername = gdalformat
    aControls.omitPyramids = False
    aControls.calcStats = False

    def _recode(info, inputs, outputs, otherargs):
        out_arr = numpy.zeros_like(inputs.inimage[0], dtype=otherargs.np_dtype)
        if otherargs.keep_vals_not_in_dict:
            out_arr = inputs.inimage[0]

        for rc_val in recode_dict:
            out_arr[inputs.inimage[0] == rc_val] = recode_dict[rc_val]
        outputs.outimage = numpy.expand_dims(out_arr, axis=0)

    applier.apply(_recode, infiles, outfiles, otherargs, controls=aControls)


def calc_fill_regions_knn(
    in_ref_img,
    ref_no_data,
    in_fill_regions_img,
    fill_region_val,
    output_img,
    k=5,
    summary=rsgislib.SUMTYPE_MODE,
    gdalformat="KEA",
    datatype=rsgislib.TYPE_32INT,
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
    import rsgislib.imageutils
    from rios import applier

    import scipy
    import scipy.stats
    import rtree

    if not rsgislib.imageutils.do_gdal_layers_have_same_proj(
        in_ref_img, in_fill_regions_img
    ):
        raise rsgislib.RSGISPyException(
            "The reference image and fill regions image "
            "do not have the same projection."
        )

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

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
                        out_arr[i, j] = scipy.stats.mode(int_vals).mode[0]
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


def are_imgs_equal(in_ref_img, in_cmp_img, prop_eql=1.0, flt_dif=0.0001):
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
    import rsgislib.imageutils
    from rios import applier

    if rsgislib.imageutils.get_img_band_count(
        in_ref_img
    ) != rsgislib.imageutils.get_img_band_count(in_cmp_img):
        raise rsgislib.RSGISPyException(
            "The number of image bands is not the same between the two images."
        )

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

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
    import rsgislib.imageutils
    from rios import applier

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

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

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
