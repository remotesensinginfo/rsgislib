#!/usr/bin/env python
"""
The tools.test_images module contains some functions to create images which can be
used for testing other functions or generate example datasets

"""
import rsgislib


def create_random_int_img(
    output_img,
    n_bands,
    x_size,
    y_size,
    out_vals,
    gdalformat="KEA",
    datatype=rsgislib.TYPE_8UINT,
    calc_stats=True,
):
    """
    A function which creates an image with pixels values randomly assigned from the
    list in the out_vals input list.

    :param output_img: The output image file name and path
    :param n_bands: the number of bands in the output image
    :param x_size: the number of pixels in the X axis
    :param y_size: the number of pixels in the Y axis
    :param out_vals: a list of values to be randomly populated within the output image
    :param gdalformat: the output image file format (Default: KEA)
    :param datatype: the output image data type (Default: rsgislib.TYPE_8UINT)
    :param calc_stats: calculate image statistics and pyramids

    """
    import rsgislib.imageutils
    from rios import applier
    import numpy.random

    rsgislib.imageutils.createBlankImagePy(
        output_img,
        n_bands,
        x_size,
        y_size,
        100000,
        500000,
        1,
        1,
        "",
        gdalformat,
        datatype,
        options=[],
        no_data_val=0,
    )

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.image = output_img
    outfiles = applier.FilenameAssociations()
    outfiles.out_image = output_img
    otherargs = applier.OtherInputs()
    otherargs.out_vals = out_vals
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyPopVals(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        in_img_shp = inputs.image.shape
        outputs.out_image = numpy.random.choice(otherargs.out_vals, in_img_shp).astype(
            rsgislib.getNumpyDataType(datatype)
        )

    applier.apply(_applyPopVals, infiles, outfiles, otherargs, controls=aControls)
    if calc_stats:
        import rsgislib.rastergis

        rsgislib.rastergis.populateStats(output_img, True, True, True)
