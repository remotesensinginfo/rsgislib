#!/usr/bin/env python
"""
Contains image filtering functionality for rsgislib
"""

# import the C++ extension into this level
from ._imagefilter import *
import os.path


class FilterParameters:
    """Object, specifying the type of filter and filter parameters"""

    def __init__(
        self,
        filter_type,
        file_ending,
        size=3,
        option=None,
        n_looks=None,
        stddev=None,
        stddev_x=None,
        stddev_y=None,
        angle=None,
    ):
        self.filter_type = filter_type
        self.file_ending = file_ending
        self.size = size
        self.option = option
        self.n_looks = n_looks
        self.stddev = stddev
        self.stddev_x = stddev_x
        self.stddev_y = stddev_y
        self.angle = angle


def apply_median_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a median filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_median3.kea'
        imagefilter.apply_median_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="Median", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_mean_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a mean filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_mean3.kea'
        imagefilter.apply_mean_filter(input_img, outImgFile, 3, "KEA",
                                    rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="Mean", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_min_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a min filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_min3.kea'
        imagefilter.apply_min_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="Min", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_max_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a max filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_max3.kea'
        imagefilter.apply_max_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="Max", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_mode_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a mode filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_mode3.kea'
        imagefilter.apply_mode_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="Mode", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_stddev_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a std dev filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_stdev3.kea'
        imagefilter.apply_stddev_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="StdDev", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_range_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a range filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_range3.kea'
        imagefilter.apply_range_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="Range", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_mean_diff_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a mean difference filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_meandiff3.kea'
        imagefilter.apply_mean_diff_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="MeanDiff", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_mean_diff_abs_filter(
    input_img, output_img, filter_size, gdalformat, datatype
):
    """
    Apply a mean absolute difference filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_meandiffabs3.kea'
        imagefilter.apply_mean_diff_abs_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="MeanDiffAbs", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_total_diff_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a total (i.e., sum) difference filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_totaldiff3.kea'
        imagefilter.apply_total_diff_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="TotalDiff", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_total_diff_abs_filter(
    input_img, output_img, filter_size, gdalformat, datatype
):
    """
    Apply a total absolute difference filter to the specified input image.

    ::param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_totaldiffabs3.kea'
        imagefilter.apply_total_diff_abs_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="TotalDiffAbs", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_coeff_of_var_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a coefficient of variance filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_cofvar3.kea'
        imagefilter.apply_coeff_of_var_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="CoeffOfVar", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_total_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a total filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_total3.kea'
        imagefilter.apply_total_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="Total", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_norm_var_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a normalised variance filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_NormVar3.kea'
        imagefilter.apply_norm_var_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="NormVar", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_norm_var_sqrt_filter(
    input_img, output_img, filter_size, gdalformat, datatype
):
    """
    Apply a normalised variance square root filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_NormVarSqrt3.kea'
        imagefilter.apply_norm_var_sqrt_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="NormVarSqrt", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_norm_var_ln_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a normalised variance natural log filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_NormVarLn3.kea'
        imagefilter.apply_norm_var_ln_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="NormVarLn", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_texture_var_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a texture variance filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_NormVarLn3.kea'
        imagefilter.apply_texture_var_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="TextureVar", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_kuwahara_filter(input_img, output_img, filter_size, gdalformat, datatype):
    """
    Apply a kuwahara filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter
                        (must be an odd number, i.e., 3, 5, 7, etc).
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_kuwa3.kea'
        imagefilter.apply_kuwahara_filter(input_img, outImgFile, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(filter_type="Kuwahara", file_ending="", size=filter_size)
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_sobel_filter(input_img, output_img, gdalformat, datatype):
    """
    Apply a sobel filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_sobel.kea'
        imagefilter.apply_sobel_filter(input_img, outImgFile, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filter_type="Sobel", file_ending="", option="xy"))
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_sobel_x_filter(input_img, output_img, gdalformat, datatype):
    """
    Apply a sobel filter in X axis to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_sobelx.kea'
        imagefilter.apply_sobel_x_filter(input_img, outImgFile, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filter_type="Sobel", file_ending="", option="x"))
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_sobel_y_filter(input_img, output_img, gdalformat, datatype):
    """
    Apply a sobel filter in Y axis to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_sobely.kea'
        imagefilter.apply_sobel_y_filter(input_img, outImgFile, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filter_type="Sobel", file_ending="", option="y"))
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_prewitt_filter(input_img, output_img, gdalformat, datatype):
    """
    Apply a Prewitt filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_prewitt.kea'
        imagefilter.apply_prewitt_filter(input_img, outImgFile, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filter_type="Prewitt", file_ending="", option="xy"))
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_prewitt_x_filter(input_img, output_img, gdalformat, datatype):
    """
    Apply a Prewitt filter in X axis to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_prewitt.kea'
        imagefilter.apply_prewitt_x_filter(input_img, outImgFile, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filter_type="Prewitt", file_ending="", option="x"))
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_prewitt_y_filter(input_img, output_img, gdalformat, datatype):
    """
    Apply a Prewitt filter in Y axis to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_prewitt.kea'
        imagefilter.apply_prewitt_y_filter(input_img, outImgFile, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filter_type="Prewitt", file_ending="", option="y"))
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_gaussian_smooth_filter(
    input_img,
    output_img,
    filter_size,
    stddev_x,
    stddev_y,
    filter_angle,
    gdalformat,
    datatype,
):
    """
    Apply a Gaussian smoothing filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter (must be an
                        odd number, i.e., 3, 5, 7, etc).
    :param stddev_x: standard deviation in the x-axis
    :param stddev_y: standard deviation in the y-axis
    :param filter_angle: the angle of the filter
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_gausmooth.kea'
        imagefilter.apply_gaussian_smooth_filter(input_img, outImgFile, 3, 1.0, 1.0. 0.0,
                                              "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(
            filter_type="GaussianSmooth",
            file_ending="",
            size=filter_size,
            stddev_x=stddev_x,
            stddev_y=stddev_y,
            angle=filter_angle,
        )
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_gaussian_1st_deriv_filter(
    input_img,
    output_img,
    filter_size,
    stddev_x,
    stddev_y,
    filter_angle,
    gdalformat,
    datatype,
):
    """
    Apply a Gaussian first derivative filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter (must be an
                        odd number, i.e., 3, 5, 7, etc).
    :param stddev_x: standard deviation in the x-axis
    :param stddev_y: standard deviation in the y-axis
    :param filter_angle: the angle of the filter
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_gau1st.kea'
        imagefilter.apply_gaussian_1st_deriv_filter(input_img, outImgFile, 3, 1.0, 1.0. 0.0,
                                                "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(
            filter_type="Gaussian1st",
            file_ending="",
            size=filter_size,
            stddev_x=stddev_x,
            stddev_y=stddev_y,
            angle=filter_angle,
        )
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_gaussian_2nd_deriv_filter(
    input_img,
    output_img,
    filter_size,
    stddev_x,
    stddev_y,
    filter_angle,
    gdalformat,
    datatype,
):
    """
    Apply a Gaussian second derivative filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter (must be an
                        odd number, i.e., 3, 5, 7, etc).
    :param stddev_x: standard deviation in the x-axis
    :param stddev_y: standard deviation in the y-axis
    :param filter_angle: the angle of the filter
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_gau1st.kea'
        imagefilter.apply_gaussian_2nd_deriv_filter(input_img, outImgFile, 3, 1.0, 1.0. 0.0,
                                                "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(
            filter_type="Gaussian2nd",
            file_ending="",
            size=filter_size,
            stddev_x=stddev_x,
            stddev_y=stddev_y,
            angle=filter_angle,
        )
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_laplacian_filter(
    input_img, output_img, filter_size, stddev, gdalformat, datatype
):
    """
    Apply a Laplacian filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter (must be an
                        odd number, i.e., 3, 5, 7, etc).
    :param stddev: standard deviation of the filter
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_laplacian.kea'
        imagefilter.apply_laplacian_filter(input_img, outImgFile, 3, 1.0, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(
            filter_type="Laplacian", file_ending="", size=filter_size, stddev=stddev
        )
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)


def apply_lee_filter(input_img, output_img, filter_size, n_looks, gdalformat, datatype):
    """Apply a Lee SAR filter to the specified input image.

    :param input_img: string specifying the input image to be filtered.
    :param output_img: string specifying the output image file..
    :param filter_size: int specifying the size of the image filter (must be an
                        odd number, i.e., 3, 5, 7, etc).
    :param n_looks: int specifying the number of looks applied to the SAR image.
    :param gdalformat: string specifying the output image format (e.g., KEA).
    :param datatype: Specifying the output image pixel data type
                     (e.g., rsgislib.TYPE_32FLOAT).

    Example::

        import rsgislib
        from rsgislib import imagefilter
        input_img = 'jers1palsar_stack.kea'
        outImgFile = 'jers1palsar_stack_lee.kea'
        imagefilter.apply_lee_filter(input_img, outImgFile, 3, 3, "KEA",
                                      rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(output_img)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(
        FilterParameters(
            filter_type="Lee", file_ending="", size=filter_size, n_looks=n_looks
        )
    )
    apply_filters(input_img, outputImageBase, filters, gdalformat, outExt, datatype)
