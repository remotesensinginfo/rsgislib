#!/usr/bin/env python
"""
The image registration module contains algorithms for generating tie points matching two image and warping images based on tie points.

There are two algorithms are available for registration: basic, and singlelayer. The single layer algorithm is a simplified version of the algorithm proposed in:

Bunting, P.J., Labrosse, F. & Lucas, R.M., 2010. A multi-resolution area-based technique for automatic multi-modal image registration. Image and Vision Computing, 28(8), pp.1203-1219.


Image distance metrics:

    * METRIC_EUCLIDEAN = 1
    * METRIC_SQDIFF = 2
    * METRIC_MANHATTEN = 3
    * METRIC_CORELATION = 4


GCP Output Types:

    * TYPE_ENVI_IMG2IMG = 1
    * TYPE_ENVI_IMG2MAP = 2
    * TYPE_RSGIS_IMG2MAP = 3
    * TYPE_RSGIS_MAPOFFS = 4


"""

import numpy
from osgeo import gdal

import rsgislib

# import the C++ extension into this level
from ._imageregistration import *

METRIC_EUCLIDEAN = 1
METRIC_SQDIFF = 2
METRIC_MANHATTEN = 3
METRIC_CORELATION = 4

TYPE_ENVI_IMG2IMG = 1
TYPE_ENVI_IMG2MAP = 2
TYPE_RSGIS_IMG2MAP = 3
TYPE_RSGIS_MAPOFFS = 4


def warp_with_gcps_with_gdal(
    in_ref_img: str,
    in_process_img: str,
    output_img: str,
    gdalformat: str,
    interp_method: int = rsgislib.INTERP_NEAREST_NEIGHBOUR,
    use_tps: bool = False,
    use_poly: bool = True,
    poly_order: int = 3,
    use_multi_thread: bool = False,
):
    """
    A utility function to warp an image file (in_process_img) using GCPs defined within
    the image header - this is the same as using the gdalwarp utility. However, the
    output image will have the same pixel grid and dimensions as the input reference
    image (in_ref_img).

    :param in_ref_img: is the input reference image to which the processing image
                       is to resampled to.
    :param in_process_img: is the image which is to be resampled.
    :param output_img: is the output image file.
    :param gdalformat: is the gdal format for the output image.
    :param interp_method: is the interpolation method used to resample the image
                          [bilinear, lanczos, cubicspline, nearestneighbour, cubic,
                          average, mode]
    :param use_tps: is a boolean specifying that the thin plated splines method of
                    warping should be used (i.e., rubbersheet); Default False.
    :param use_poly: is a boolean specifying that a polynomial method of warping
                     is used; Default True
    :param poly_order: is the order of the polynomial used to represent the
                       transformation (1, 2 or 3). Only used if use_poly=True
    :param use_multi_thread: is a boolean specifying whether multiple processing
                             cores should be used for the warping.

    """
    from osgeo import gdal

    import rsgislib.imageutils

    if not rsgislib.imageutils.has_gcps(in_process_img):
        raise rsgislib.RSGISPyException(
            "Input process image does not have GCPs "
            "within it's header - this is required."
        )

    numBands = rsgislib.imageutils.get_img_band_count(in_process_img)
    noDataVal = rsgislib.imageutils.get_img_no_data_value(in_process_img)
    datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(in_process_img)

    interpolationMethod = gdal.GRA_NearestNeighbour
    if interp_method == rsgislib.INTERP_BILINEAR:
        interpolationMethod = gdal.GRA_Bilinear
    elif interp_method == rsgislib.INTERP_LANCZOS:
        interpolationMethod = gdal.GRA_Lanczos
    elif interp_method == rsgislib.INTERP_CUBICSPLINE:
        interpolationMethod = gdal.GRA_CubicSpline
    elif interp_method == rsgislib.INTERP_NEAREST_NEIGHBOUR:
        interpolationMethod = gdal.GRA_NearestNeighbour
    elif interp_method == rsgislib.INTERP_CUBIC:
        interpolationMethod = gdal.GRA_Cubic
    elif interp_method == rsgislib.INTERP_AVERAGE:
        interpolationMethod = gdal.GRA_Average
    elif interp_method == rsgislib.INTERP_MODE:
        interpolationMethod = gdal.GRA_Mode
    else:
        raise rsgislib.RSGISPyException(
            "Interpolation method was not recognised or known."
        )

    rsgislib.imageutils.create_copy_img(
        in_ref_img, output_img, numBands, 0, gdalformat, datatype
    )

    inFile = gdal.Open(in_process_img, gdal.GA_ReadOnly)
    outFile = gdal.Open(output_img, gdal.GA_Update)

    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    wrpOpts = None
    if use_tps:
        wrpOpts = gdal.WarpOptions(
            resampleAlg=interpolationMethod,
            srcNodata=noDataVal,
            dstNodata=noDataVal,
            multithread=use_multi_thread,
            tps=use_tps,
            callback=callback,
        )
    elif use_poly:
        wrpOpts = gdal.WarpOptions(
            resampleAlg=interpolationMethod,
            srcNodata=noDataVal,
            dstNodata=noDataVal,
            multithread=use_multi_thread,
            polynomialOrder=poly_order,
            callback=callback,
        )
    else:
        warpOptions = None

    gdal.Warp(outFile, inFile, options=wrpOpts)

    inFile = None
    outFile = None


def add_vec_pts_as_gcps_to_img(
    input_img: str,
    output_img: str,
    vec_file: str,
    vec_lyr: str = None,
    gcp_x_col: str = "x_match",
    gcp_y_col: str = "y_match",
    gcp_z_col: str = None,
    gcp_epsg: int = None,
):
    """
    A function which uses a points vector layer to specify GCP locations within the
    image with attributes to the vector layer used to specify the GCP spatial
    location.

    Note. input_img and output_img can be the same file path if you do not
    want an new output image to be created (i.e., for the input file to be edited)

    :param input_img: The input image file path
    :param output_img: The output image path
    :param vec_file: the vector file path
    :param vec_lyr: the vector layer name
    :param gcp_x_col: column within the vector layer specifying the GCP x coordinates
    :param gcp_y_col: column within the vector layer specifying the GCP y coordinates
    :param gcp_z_col: column within the vector layer specifying the GCP x coordinates.
                      The Z coordinate is optional, Default is None.
    :param gcp_epsg: Optionally specify an EPSG code for the projection the GCP points.
                     If None (Default) then the projection of the input image will
                     used.

    """

    import shutil

    import geopandas

    import rsgislib.imageutils
    import rsgislib.tools.projection
    import rsgislib.vectorutils

    # Check the dataset geometry is points
    if (
        rsgislib.vectorutils.get_vec_lyr_geom_type(vec_file, vec_lyr)
        != rsgislib.GEOM_PT
    ):
        raise rsgislib.RSGISPyException("Input vector layer must be a point dataset.")

    gpd_pts_df = geopandas.read_file(vec_file, layer=vec_lyr)

    pt_sp_x_lst = list()
    pt_sp_y_lst = list()
    gcp_sp_x_lst = list()
    gcp_sp_y_lst = list()
    gcp_sp_z_lst = list()

    n_rows = len(gpd_pts_df)
    for i in range(n_rows):
        pt_sp_x_lst.append(gpd_pts_df.iloc[[i]].geometry.x.values[0])
        pt_sp_y_lst.append(gpd_pts_df.iloc[[i]].geometry.y.values[0])

        gcp_sp_x_lst.append(gpd_pts_df.iloc[[i]][gcp_x_col].values[0])
        gcp_sp_y_lst.append(gpd_pts_df.iloc[[i]][gcp_y_col].values[0])
        gcp_sp_z = 0.0
        if gcp_z_col is not None:
            gcp_sp_z = gpd_pts_df.iloc[[i]][gcp_z_col].values[0]

        gcp_sp_z_lst.append(gcp_sp_z)

    pt_pxl_x_lst, pt_pxl_y_lst = rsgislib.imageutils.get_img_pxl_coords(
        input_img, x_coords=numpy.array(pt_sp_x_lst), y_coords=numpy.array(pt_sp_y_lst)
    )

    gcps_lst = list()
    for i in range(n_rows):
        gcps_lst.append(
            gdal.GCP(
                gcp_sp_x_lst[i],
                gcp_sp_y_lst[i],
                gcp_sp_z_lst[i],
                int(pt_pxl_x_lst[i]),
                int(pt_pxl_y_lst[i]),
            )
        )

    if gcp_epsg is None:
        gcp_epsg = rsgislib.imageutils.get_epsg_proj_from_img(input_img)

    gcp_prj_obj = rsgislib.tools.projection.get_osr_prj_obj(gcp_epsg)

    # Make a copy of the input image so edit does not change this image file.
    if input_img != output_img:
        shutil.copy(input_img, output_img)

    img_ds = gdal.Open(output_img, gdal.GA_Update)
    if img_ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open raster image: {}".format(input_img)
        )
    img_ds.SetGCPs(gcps_lst, gcp_prj_obj)
    img_ds = None
