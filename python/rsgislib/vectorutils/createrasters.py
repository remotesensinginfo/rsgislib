#!/usr/bin/env python
"""
The vector conversion tools for converting between raster and vector
"""

import osgeo.gdal as gdal
import osgeo.ogr as ogr

import rsgislib

gdal.UseExceptions()


def rasterise_vec_lyr(
    vec_file: str,
    vec_lyr: str,
    input_img: str,
    output_img: str,
    gdalformat: str = "KEA",
    burn_val: int = 1,
    datatype=rsgislib.TYPE_8UINT,
    att_column=None,
    use_vec_extent=False,
    thematic=True,
    no_data_val=0,
):
    """
    A utility to rasterise a vector layer to an image covering the same region and at
    the same resolution as the input image.

    Where:

    :param vec_file: is a string specifying the input vector file
    :param vec_lyr: is a string specifying the input vector layer name.
    :param input_img: is a string specifying the input image defining the grid, pixel
                      resolution and area for the rasterisation (if None and vecExt is
                      False them assumes output image already exists and just uses it
                      as is burning vector into it)
    :param output_img: is a string specifying the output image for the rasterised
                       vector file
    :param gdalformat: is the output image format (Default: KEA).
    :param burn_val: is the value for the output image pixels if no attribute is
                     provided.
    :param datatype: of the output file, default is rsgislib.TYPE_8UINT
    :param att_column: is a string specifying the attribute to be rasterised, value of
                       None creates a binary mask and \"FID\" creates a temp vector file
                       with a "FID" column and rasterises that column.
    :param use_vec_extent: is a boolean specifying that the output image should be cut
                           to the same extent as the input shapefile (Default is False
                           and therefore output image will be the same as the input).
    :param thematic: is a boolean (default True) specifying that the output image is
                     an thematic dataset so a colour table will be populated.
    :param no_data_val: is a float specifying the no data value associated with a
                        continuous output image.

    Example::

        from rsgislib import vectorutils

        inputVector = 'crowns.shp'
        inputVectorLyr = 'crowns'
        inputImage = 'injune_p142_casi_sub_utm.kea'
        outputImage = 'psu142_crowns.kea'
        vectorutils.rasterise_vec_lyr(inputVector,
                                      inputVectorLyr,
                                      inputImage,
                                      outputImage,
                                      'KEA',
                                      vecAtt='FID')

    """
    import rsgislib.imageutils

    try:
        if use_vec_extent:
            xRes, yRes = rsgislib.imageutils.get_img_res(input_img)
            if yRes < -1:
                yRes = yRes * (-1)
            outRes = xRes
            if xRes > yRes:
                outRes = yRes

            rsgislib.imageutils.create_copy_img_vec_extent_snap_to_grid(
                vec_file, vec_lyr, output_img, outRes, 1, gdalformat, datatype
            )
        elif input_img is None:
            print("Assuming output image is already created so just using.")
        else:
            print("Creating output image using input image")
            rsgislib.imageutils.create_copy_img(
                input_img, output_img, 1, 0, gdalformat, datatype
            )

        print("Running Rasterise now...")
        out_img_ds = gdal.Open(output_img, gdal.GA_Update)
        if out_img_ds is None:
            raise Exception("Could not open '{}'".format(output_img))

        vec_ds = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
        if vec_ds is None:
            raise Exception("Could not open '{}'".format(vec_file))

        vec_lyr_obj = vec_ds.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise Exception("Could not find layer '{}'".format(vec_lyr))

        # Run the algorithm.
        err = 0
        if att_column is None:
            err = gdal.RasterizeLayer(
                out_img_ds, [1], vec_lyr_obj, burn_values=[burn_val]
            )
        else:
            err = gdal.RasterizeLayer(
                out_img_ds, [1], vec_lyr_obj, options=["ATTRIBUTE=" + att_column]
            )
        if err != 0:
            raise Exception("Rasterisation Error: " + str(err))

        out_img_ds = None
        vec_ds = None

        if thematic and (gdalformat == "KEA"):
            import rsgislib.rastergis

            rsgislib.rastergis.pop_rat_img_stats(
                clumps_img=output_img,
                add_clr_tab=True,
                calc_pyramids=True,
                ignore_zero=True,
            )
        else:
            rsgislib.imageutils.pop_img_stats(output_img, True, no_data_val, True)
    except Exception as e:
        raise e


def rasterise_vec_lyr_obj(
    vec_lyr_obj: ogr.Layer,
    output_img: str,
    burn_val: int = 1,
    att_column: str = None,
    calc_stats: bool = True,
    thematic: bool = True,
    no_data_val: float = 0,
):
    """
    A utility to rasterise a vector layer to an image covering the same region.

    Where:

    :param vec_lyr_obj: is a OGR Vector Layer Object
    :param output_img: is a string specifying the output image, this image must already
                     exist and intersect within the input vector layer.
    :param burn_val: is the value for the output image pixels if no attribute is
                     provided.
    :param att_column: is a string specifying the attribute to be rasterised, value
                       of None creates a binary mask and \"FID\" creates a temp
                       vector layer with a "FID" column and rasterises that column.
    :param calc_stats: is a boolean specifying whether image stats and pyramids
                       should be calculated.
    :param thematic: is a boolean (default True) specifying that the output image is
                     an thematic dataset so a colour table will be populated.
    :param no_data_val: is a float specifying the no data value associated with a
                   continuous output image.

    """
    try:
        if vec_lyr_obj is None:
            raise Exception("The vec_lyr_obj passed to the function was None.")

        print("Running Rasterise now...")
        out_img_ds = gdal.Open(output_img, gdal.GA_Update)
        if out_img_ds is None:
            raise Exception("Could not open '{}'".format(output_img))

        # Run the algorithm.
        err = 0
        if att_column is None:
            err = gdal.RasterizeLayer(
                out_img_ds, [1], vec_lyr_obj, burn_values=[burn_val]
            )
        else:
            err = gdal.RasterizeLayer(
                out_img_ds, [1], vec_lyr_obj, options=["ATTRIBUTE=" + att_column]
            )
        if err != 0:
            raise Exception("Rasterisation Error: {}".format(err))

        out_img_ds = None

        if calc_stats:
            if thematic:
                import rsgislib.rastergis

                rsgislib.rastergis.pop_rat_img_stats(
                    clumps_img=output_img,
                    add_clr_tab=True,
                    calc_pyramids=True,
                    ignore_zero=True,
                )
            else:
                import rsgislib.imageutils

                rsgislib.imageutils.pop_img_stats(output_img, True, no_data_val, True)
    except Exception as e:
        print("Failed rasterising: {}".format(output_img))
        raise e


def copy_vec_to_rat(
    vec_file: str, vec_lyr: str, input_img: str, output_img: str, fid_col: str = "FID"
):
    """
    A utility to create raster copy of a polygon vector layer. The output image is
    a KEA file and the attribute table has the attributes from the vector layer.

    Where:

    :param vec_file: is a string specifying the input vector file
    :param vec_lyr: is a string specifying the layer within the input vector file
    :param input_img: is a string specifying the input image defining the grid,
                      pixel resolution and area for the rasterisation
    :param output_img: is a string specifying the output KEA image for the
                       rasterised vector layer

    Example::

        from rsgislib import vectorutils

        inputVector = 'crowns.shp'
        inputImage = 'injune_p142_casi_sub_utm.kea'
        outputImage = 'psu142_crowns.kea'

        vectorutils.copy_vec_to_rat(inputVector, 'crowns', inputImage, outputImage)

    """
    import rsgislib
    import rsgislib.rastergis
    import rsgislib.vectorutils

    cols = rsgislib.vectorutils.get_vec_lyr_cols(vec_file, vec_lyr)
    if fid_col not in cols:
        raise rsgislib.RSGISPyException("FID Column not within the input layer")

    rasterise_vec_lyr(
        vec_file,
        vec_lyr,
        input_img,
        output_img,
        gdalformat="KEA",
        datatype=rsgislib.TYPE_32UINT,
        att_column=fid_col,
        use_vec_extent=False,
        thematic=True,
        no_data_val=0,
    )
    rsgislib.rastergis.import_vec_atts(output_img, vec_file, vec_lyr, "pxlval", None)
