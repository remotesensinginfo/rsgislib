#!/usr/bin/env python
############################################################################
#  specunmixing.py
#
#  Copyright 2020 RSGISLib.
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
# Purpose:  Perform spectral unmixing
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 12/02/2020
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
############################################################################

import os

from osgeo import gdal
import numpy

# import the C++ extension into this level
from ._specunmixing import *
import rsgislib


class ImageEndmemberInfo(object):
    """
    Create a list of these objects to pass to the summarise_multi_endmember_linear_unmixing
    function

    :param in_unmix_img: is the input image file with the linear unmixed results.
    :param endmembers_file: is the endmembers file (.mtxt) used to produce
                            the in_unmix_img.
    :param endmember_names: is the names of the endmembers in the order they are
                            provided within the endmembers_file.

    """

    def __init__(self, in_unmix_img, endmembers_file, endmember_names):
        """
        :param in_unmix_img: is the input image file with the linear unmixed results.
        :param endmembers_file: is the endmembers file (.mtxt) used to produce
                                the in_unmix_img.
        :param endmember_names: is the names of the endmembers in the order they are
                                provided within the endmembers_file.
        """
        self.in_unmix_img = in_unmix_img
        self.endmembers_file = endmembers_file
        self.endmember_names = endmember_names


def read_endmembers_mtxt(endmembers_file, gain=1, weight=None):
    """
    A function to read endmembers mtxt file

    :param endmembers_file: File path to the mtxt input file.
    :param gain: A gain for the endmembers (input endmember / gain = output endmember).
                 If 1 then ignored.
    :param weight: Optional (if None ignored) to provide a weight to implement the
                   approach of Scarth et al (2010) adding a weight to the least squares
                   optimisation to get the abundances to sum to 1.
    :return: [m, n, endmemarr]; m size, n size and a numpy array with the end members

    """
    dataList = []
    dataFile = open(endmembers_file, "r")
    for line in dataFile:
        line = line.strip()
        if line != "":
            dataList.append(line)
    dataFile.close()
    m = int(dataList[0].split("=")[-1])
    n = int(dataList[1].split("=")[-1])
    endmemarr = numpy.zeros((m, n))
    datavals = dataList[2].split(",")

    i = 0

    for n_idx in range(n):
        for m_idx in range(m):
            endmemarr[m_idx, n_idx] = float(datavals[i])
            i += 1

    if gain > 1:
        endmemarr = endmemarr / gain

    if weight is not None:
        weights = numpy.empty([m, 1])
        weights[...] = weight
        endmemarr = numpy.hstack([endmemarr, weights])
        n += 1

    return m, n, endmemarr


def plot_endmembers(
    endmembers_file,
    endmember_names,
    out_plot_file,
    plot_title=None,
    gain=1,
    wavelengths=None,
):
    """
    A function to plot endmembers for visualisation.

    :param endmembers_file: input endmembers file (.mtxt)
    :param endmember_names: the names of the endmembers in the same order as the
                            endmembers_file.
    :param out_plot_file: the output plot file.
    :param plot_title: A title for the plot (Optional)
    :param gain: A gain to get the reflectance values as a percentage (Optional)
                 (e.g., if multiplied by 1000 then the gain would be 10
                 if multiplied by 10,000 then the gain would be 100 )
    :param wavelengths: A list of wavelengths for the image bands. Optional, if not
                        provided then band numbers will be used.

    """
    import matplotlib.pyplot as plt

    n_endmembers, n_bands, endmembers = read_endmembers_mtxt(endmembers_file, gain=gain)

    x_axis_lbl = "Wavelength"

    if len(endmember_names) != n_endmembers:
        raise rsgislib.RSGISPyException(
            "The list of names provide does not match "
            "the number of endmembers in the input file."
        )
    if (wavelengths is not None) and (not isinstance(wavelengths, list)):
        raise rsgislib.RSGISPyException(
            "If provided the wavelength variable must be a list"
        )
    elif (wavelengths is not None) and (len(wavelengths) != n_bands):
        raise rsgislib.RSGISPyException(
            "The number of wavelengths provided is not equal"
            " to the number of image bands in the endmembers"
        )
    elif wavelengths is None:
        wavelengths = range(n_bands)
        x_axis_lbl = "Band"

    fig = plt.figure(figsize=(7, 5), dpi=300)
    ax1 = fig.add_subplot(111)

    for i in range(n_endmembers):
        ax1.plot(wavelengths, endmembers[i], label=endmember_names[i])

    if plot_title is not None:
        plt.title(plot_title)
    plt.xlabel(x_axis_lbl)
    plt.ylabel("Reflectance (%)")
    plt.legend()

    plt.savefig(out_plot_file)


def are_endmembers_equal(ref_endmember_file, cmp_endmember_file, flt_dif=0.0001):
    """
    A function which compares two endmembers files to check whether they are
    equal or not.

    :param ref_endmember_file: File path to the mtxt input file.
    :param cmp_endmember_file: File path to the mtxt input file.
    :param flt_dif: A threshold for comparing two floating point numbers as being
                    identical - this avoids issues with rounding and the number of
                    decimal figures stored.
    :return: boolean

    """
    n_ref_endmembers, n_ref_bands, ref_endmembers = read_endmembers_mtxt(
        ref_endmember_file, gain=1, weight=None
    )
    n_cmp_endmembers, n_cmp_bands, cmp_endmembers = read_endmembers_mtxt(
        cmp_endmember_file, gain=1, weight=None
    )

    if n_ref_endmembers != n_cmp_endmembers:
        return False

    if n_ref_bands != n_cmp_bands:
        return False

    endmembers_eq = True
    for i in range(n_ref_endmembers):
        for j in range(n_ref_bands):
            if abs(ref_endmembers[i][j] - cmp_endmembers[i][j]) > flt_dif:
                endmembers_eq = False
                break
        if not endmembers_eq:
            break
    return endmembers_eq


def spec_unmix_spts_ucls(
    input_img,
    valid_msk_img,
    valid_msk_val,
    output_img,
    endmembers_file,
    gdalformat="KEA",
    gain=1,
    weight=None,
    calc_stats=True,
):
    """
    This function performs spectral unmixing using an unconstrain model
    using the pysptools module

    Note, pysptools needs to be installed to use this function.

    :param input_img: input image file.
    :param valid_msk_img: an image file representing the valid data region
    :param valid_msk_val: image pixel value in the mask for the valid data region
    :param output_img: the output image file
    :param endmembers_file: the endmembers (*.mtxt) file extracted using the
                            specunmixing.extractAvgEndMembers function
    :param gdalformat: the output file format. (Default: KEA)
    :param gain: A gain for the endmembers and image spectra (input spectra / gain =
                 output spectra). If 1 then ignored.
    :param weight: Optional (if None ignored) to provide a weight to implement the
                   approach of Scarth et al (2010) adding a weight to the least squares
                   optimisation to get the abundances to sum to 1.
    :param calc_stats: Boolean specifying whether to calculate pyramids and
                       metadata stats (Default: True)

    References:
            Scarth, P., Roder, A., & Schmidt, M. (2010). Tracking grazing pressure
            and climate interaction—The role of Landsat fractional cover in time
            series analysis. Proceedings of the 15th Australasian Remote Sensing
            and Photogrammetry Conference Australia: Alice Springs.
            http://dx.doi.org/10.6084/m9.ﬁgshare.94250.

    """
    from rios import applier

    import rsgislib.imageutils
    import pysptools.abundance_maps.amaps

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    m, n, endmembers_arr = read_endmembers_mtxt(endmembers_file, gain, weight)

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    infiles.valid_msk = valid_msk_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.valid_msk_val = valid_msk_val
    otherargs.endmembers_arr = endmembers_arr
    otherargs.n_endmembers = m
    otherargs.gain = gain
    otherargs.weight = weight
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyUCLS(info, inputs, outputs, otherargs):
        if numpy.any(inputs.valid_msk == otherargs.valid_msk_val):
            img_flat = numpy.moveaxis(inputs.image, 0, 2).reshape(
                -1, inputs.image.shape[0]
            )

            ID = numpy.arange(img_flat.shape[0])
            n_feats = ID.shape[0]

            ID = ID[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            img_flat = img_flat[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            ID = ID[numpy.all(numpy.isfinite(img_flat), axis=1)]
            img_flat = img_flat[numpy.all(numpy.isfinite(img_flat), axis=1)]
            ID = ID[numpy.all(img_flat > 0, axis=1)]
            img_flat = img_flat[numpy.all(img_flat > 0, axis=1)]

            if otherargs.gain > 1:
                img_flat = img_flat / otherargs.gain

            if otherargs.weight is not None:
                weights = numpy.empty([img_flat.shape[0], 1])
                weights[...] = weight
                img_flat = numpy.hstack([img_flat, weights])

            unmix_sub_arr = pysptools.abundance_maps.amaps.UCLS(
                img_flat, otherargs.endmembers_arr
            )

            unmix_arr = numpy.zeros([n_feats, otherargs.n_endmembers])
            unmix_arr[ID] = unmix_sub_arr

            out_arr = unmix_arr.reshape(
                inputs.image.shape[1], inputs.image.shape[2], otherargs.n_endmembers
            )
            out_arr = numpy.moveaxis(out_arr, 2, 0)
            outputs.outimage = out_arr
        else:
            outputs.outimage = numpy.zeros(
                [otherargs.n_endmembers, inputs.image.shape[1], inputs.image.shape[2]],
                dtype=numpy.float32,
            )

    applier.apply(_applyUCLS, infiles, outfiles, otherargs, controls=aControls)

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def spec_unmix_spts_nnls(
    input_img,
    valid_msk_img,
    valid_msk_val,
    output_img,
    endmembers_file,
    gdalformat="KEA",
    gain=1,
    weight=None,
    calc_stats=True,
):
    """
    This function performs spectral unmixing using non-negative least squares
    using the pysptools module

    Note, pysptools needs to be installed to use this function.

    :param input_img: input image file.
    :param valid_msk_img: an image file representing the valid data region
    :param valid_msk_val: image pixel value in the mask for the valid data region
    :param output_img: the output image file
    :param endmembers_file: the endmembers (*.mtxt) file extracted using the
                            specunmixing.extractAvgEndMembers function
    :param gdalformat: the output file format. (Default: KEA)
    :param gain: A gain for the endmembers and image spectra (input spectra / gain =
                 output spectra). If 1 then ignored.
    :param weight: Optional (if None ignored) to provide a weight to implement the
                   approach of Scarth et al (2010) adding a weight to the least
                   squares optimisation to get the abundances to sum to 1.
    :param calc_stats: Boolean specifying whether to calculate pyramids and
                       metadata stats (Default: True)

    References:
            Scarth, P., Roder, A., & Schmidt, M. (2010). Tracking grazing pressure
            and climate interaction—The role of Landsat fractional cover in time
            series analysis. Proceedings of the 15th Australasian Remote Sensing
            and Photogrammetry Conference Australia: Alice Springs.
            http://dx.doi.org/10.6084/m9.ﬁgshare.94250.

    """
    from rios import applier
    import rsgislib.imageutils
    import pysptools.abundance_maps.amaps

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    m, n, endmembers_arr = read_endmembers_mtxt(endmembers_file, gain, weight)

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    infiles.valid_msk = valid_msk_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.valid_msk_val = valid_msk_val
    otherargs.endmembers_arr = endmembers_arr
    otherargs.n_endmembers = m
    otherargs.gain = gain
    otherargs.weight = weight
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyNNLS(info, inputs, outputs, otherargs):
        if numpy.any(inputs.valid_msk == otherargs.valid_msk_val):
            img_flat = numpy.moveaxis(inputs.image, 0, 2).reshape(
                -1, inputs.image.shape[0]
            )

            ID = numpy.arange(img_flat.shape[0])
            n_feats = ID.shape[0]

            ID = ID[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            img_flat = img_flat[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            ID = ID[numpy.all(numpy.isfinite(img_flat), axis=1)]
            img_flat = img_flat[numpy.all(numpy.isfinite(img_flat), axis=1)]
            ID = ID[numpy.all(img_flat > 0, axis=1)]
            img_flat = img_flat[numpy.all(img_flat > 0, axis=1)]

            if otherargs.gain > 1:
                img_flat = img_flat / otherargs.gain

            if otherargs.weight is not None:
                weights = numpy.empty([img_flat.shape[0], 1])
                weights[...] = weight
                img_flat = numpy.hstack([img_flat, weights])

            unmix_sub_arr = pysptools.abundance_maps.amaps.NNLS(
                img_flat, otherargs.endmembers_arr
            )

            unmix_arr = numpy.zeros([n_feats, otherargs.n_endmembers])
            unmix_arr[ID] = unmix_sub_arr

            out_arr = unmix_arr.reshape(
                inputs.image.shape[1], inputs.image.shape[2], otherargs.n_endmembers
            )
            out_arr = numpy.moveaxis(out_arr, 2, 0)
            outputs.outimage = out_arr
        else:
            outputs.outimage = numpy.zeros(
                [otherargs.n_endmembers, inputs.image.shape[1], inputs.image.shape[2]],
                dtype=numpy.float32,
            )

    applier.apply(_applyNNLS, infiles, outfiles, otherargs, controls=aControls)

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def spec_unmix_spts_fcls(
    input_img,
    valid_msk_img,
    valid_msk_val,
    output_img,
    endmembers_file,
    gdalformat="KEA",
    gain=1,
    calc_stats=True,
):
    """
    This function performs spectral unmixing using a full constrained (non-negative
    and sum to 1) module using the pysptools module

    Note, pysptools and cvxopt modules need to be installed to use this function.

    :param input_img: input image file.
    :param valid_msk_img: an image file representing the valid data region
    :param valid_msk_val: image pixel value in the mask for the valid data region
    :param output_img: the output image file
    :param endmembers_file: the endmembers (*.mtxt) file extracted using the
                            specunmixing.extractAvgEndMembers function
    :param gdalformat: the output file format. (Default: KEA)
    :param gain: A gain for the endmembers and image spectra (input spectra / gain =
                 output spectra). If 1 then ignored.
    :param calc_stats: Boolean specifying whether to calculate pyramids and metadata
                       stats (Default: True)

    """
    from rios import applier
    import rsgislib.imageutils
    import pysptools.abundance_maps.amaps

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    m, n, endmembers_arr = read_endmembers_mtxt(endmembers_file, gain)

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    infiles.valid_msk = valid_msk_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.valid_msk_val = valid_msk_val
    otherargs.endmembers_arr = endmembers_arr
    otherargs.n_endmembers = m
    otherargs.gain = gain
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyFCLS(info, inputs, outputs, otherargs):
        if numpy.any(inputs.valid_msk == otherargs.valid_msk_val):
            img_flat = numpy.moveaxis(inputs.image, 0, 2).reshape(
                -1, inputs.image.shape[0]
            )

            ID = numpy.arange(img_flat.shape[0])
            n_feats = ID.shape[0]

            ID = ID[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            img_flat = img_flat[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            ID = ID[numpy.all(numpy.isfinite(img_flat), axis=1)]
            img_flat = img_flat[numpy.all(numpy.isfinite(img_flat), axis=1)]
            ID = ID[numpy.all(img_flat > 0, axis=1)]
            img_flat = img_flat[numpy.all(img_flat > 0, axis=1)]

            if otherargs.gain > 1:
                img_flat = img_flat / otherargs.gain

            unmix_sub_arr = pysptools.abundance_maps.amaps.FCLS(
                img_flat, otherargs.endmembers_arr
            )

            unmix_arr = numpy.zeros([n_feats, otherargs.n_endmembers])
            unmix_arr[ID] = unmix_sub_arr

            out_arr = unmix_arr.reshape(
                inputs.image.shape[1], inputs.image.shape[2], otherargs.n_endmembers
            )
            out_arr = numpy.moveaxis(out_arr, 2, 0)
            outputs.outimage = out_arr
        else:
            outputs.outimage = numpy.zeros(
                [otherargs.n_endmembers, inputs.image.shape[1], inputs.image.shape[2]],
                dtype=numpy.float32,
            )

    applier.apply(_applyFCLS, infiles, outfiles, otherargs, controls=aControls)

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def spec_unmix_pymcr_nnls(
    input_img,
    valid_msk_img,
    valid_msk_val,
    output_img,
    endmembers_file,
    gdalformat="KEA",
    gain=1,
    weight=None,
    calc_stats=True,
):
    """
    This function performs spectral unmixing using non-negative least squares
    using the pymcr module

    Note, pymcr needs to be installed to use this function.

    :param input_img: input image file.
    :param valid_msk_img: an image file representing the valid data region
    :param valid_msk_val: image pixel value in the mask for the valid data region
    :param output_img: the output image file
    :param endmembers_file: the endmembers (*.mtxt) file extracted using the
                            specunmixing.extractAvgEndMembers function
    :param gdalformat: the output file format. (Default: KEA)
    :param gain: A gain for the endmembers and image spectra (input spectra / gain =
                 output spectra). If 1 then ignored.
    :param weight: Optional (if None ignored) to provide a weight to implement the
                   approach of Scarth et al (2010) adding a weight to the least squares
                   optimisation to get the abundances to sum to 1.
    :param calc_stats: Boolean specifying whether to calculate pyramids and metadata
                       stats (Default: True)

    References:
            Scarth, P., Roder, A., & Schmidt, M. (2010). Tracking grazing pressure
            and climate interaction—The role of Landsat fractional cover in time
            series analysis. Proceedings of the 15th Australasian Remote Sensing
            and Photogrammetry Conference Australia: Alice Springs.
            http://dx.doi.org/10.6084/m9.ﬁgshare.94250.

    """
    from rios import applier
    import rsgislib.imageutils
    from pymcr.mcr import McrAR
    from pymcr.regressors import NNLS
    from pymcr.constraints import ConstraintNonneg

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    m, n, endmembers_arr = read_endmembers_mtxt(endmembers_file, gain, weight)

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    infiles.valid_msk = valid_msk_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.valid_msk_val = valid_msk_val
    otherargs.endmembers_arr = endmembers_arr
    otherargs.n_endmembers = m
    otherargs.gain = gain
    otherargs.weight = weight
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _apply_pymcr_nnls(info, inputs, outputs, otherargs):
        if numpy.any(inputs.valid_msk == otherargs.valid_msk_val):
            img_flat = numpy.moveaxis(inputs.image, 0, 2).reshape(
                -1, inputs.image.shape[0]
            )

            ID = numpy.arange(img_flat.shape[0])
            n_feats = ID.shape[0]

            ID = ID[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            img_flat = img_flat[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            ID = ID[numpy.all(numpy.isfinite(img_flat), axis=1)]
            img_flat = img_flat[numpy.all(numpy.isfinite(img_flat), axis=1)]
            ID = ID[numpy.all(img_flat > 0, axis=1)]
            img_flat = img_flat[numpy.all(img_flat > 0, axis=1)]

            if otherargs.gain > 1:
                img_flat = img_flat / otherargs.gain

            if otherargs.weight is not None:
                weights = numpy.empty([img_flat.shape[0], 1])
                weights[...] = weight
                img_flat = numpy.hstack([img_flat, weights])

            mcrar = McrAR(
                max_iter=100,
                st_regr=NNLS(),
                c_regr=NNLS(),
                c_constraints=[ConstraintNonneg()],
            )
            mcrar.fit(img_flat, ST=otherargs.endmembers_arr)

            unmix_arr = numpy.zeros([n_feats, otherargs.n_endmembers])
            unmix_arr[ID] = mcrar.C_opt_

            out_arr = unmix_arr.reshape(
                inputs.image.shape[1], inputs.image.shape[2], otherargs.n_endmembers
            )
            out_arr = numpy.moveaxis(out_arr, 2, 0)
            outputs.outimage = out_arr
        else:
            outputs.outimage = numpy.zeros(
                [otherargs.n_endmembers, inputs.image.shape[1], inputs.image.shape[2]],
                dtype=numpy.float32,
            )

    applier.apply(_apply_pymcr_nnls, infiles, outfiles, otherargs, controls=aControls)

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def spec_unmix_pymcr_fcls(
    input_img,
    valid_msk_img,
    valid_msk_val,
    output_img,
    endmembers_file,
    gdalformat="KEA",
    gain=1,
    calc_stats=True,
):
    """
    This function performs spectral unmixing using a full constrained (non-negative
    and sum to 1) module using the pymcr module

    Note, pymcr needs to be installed to use this function.

    :param input_img: input image file.
    :param valid_msk_img: an image file representing the valid data region
    :param valid_msk_val: image pixel value in the mask for the valid data region
    :param output_img: the output image file
    :param endmembers_file: the endmembers (*.mtxt) file extracted using the
                            specunmixing.extractAvgEndMembers function
    :param gdalformat: the output file format. (Default: KEA)
    :param gain: A gain for the endmembers and image spectra (input spectra / gain =
                 output spectra). If 1 then ignored.
    :param calc_stats: Boolean specifying whether to calculate pyramids and metadata
                       stats (Default: True)

    """
    from rios import applier
    import rsgislib.imageutils
    from pymcr.mcr import McrAR
    from pymcr.regressors import NNLS
    from pymcr.constraints import ConstraintNonneg, ConstraintNorm

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    m, n, endmembers_arr = read_endmembers_mtxt(endmembers_file, gain)

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    infiles.valid_msk = valid_msk_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.valid_msk_val = valid_msk_val
    otherargs.endmembers_arr = endmembers_arr
    otherargs.n_endmembers = m
    otherargs.gain = gain
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _apply_pymcr_fcls(info, inputs, outputs, otherargs):
        if numpy.any(inputs.valid_msk == otherargs.valid_msk_val):
            img_flat = numpy.moveaxis(inputs.image, 0, 2).reshape(
                -1, inputs.image.shape[0]
            )

            ID = numpy.arange(img_flat.shape[0])
            n_feats = ID.shape[0]

            ID = ID[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            img_flat = img_flat[inputs.valid_msk.flatten() == otherargs.valid_msk_val]
            ID = ID[numpy.all(numpy.isfinite(img_flat), axis=1)]
            img_flat = img_flat[numpy.all(numpy.isfinite(img_flat), axis=1)]
            ID = ID[numpy.all(img_flat > 0, axis=1)]
            img_flat = img_flat[numpy.all(img_flat > 0, axis=1)]

            if otherargs.gain > 1:
                img_flat = img_flat / otherargs.gain

            mcrar = McrAR(
                max_iter=200,
                st_regr=NNLS(),
                c_regr=NNLS(),
                c_constraints=[ConstraintNonneg(), ConstraintNorm()],
            )
            mcrar.fit(img_flat, ST=otherargs.endmembers_arr)

            unmix_arr = numpy.zeros([n_feats, otherargs.n_endmembers])
            unmix_arr[ID] = mcrar.C_opt_

            out_arr = unmix_arr.reshape(
                inputs.image.shape[1], inputs.image.shape[2], otherargs.n_endmembers
            )
            out_arr = numpy.moveaxis(out_arr, 2, 0)
            outputs.outimage = out_arr
        else:
            outputs.outimage = numpy.zeros(
                [otherargs.n_endmembers, inputs.image.shape[1], inputs.image.shape[2]],
                dtype=numpy.float32,
            )

    applier.apply(_apply_pymcr_fcls, infiles, outfiles, otherargs, controls=aControls)

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def rescale_unmixing_results(input_img, output_img, gdalformat="KEA", calc_stats=True):
    """
    A function which rescales an output from a spectral unmixing
    (e.g., rsgislib.imagecalc.specunmixing.spec_unmix_spts_ucls) so that
    negative values are removed (set to 0) and each pixel sums to 1.

    :param inputImg: Input image with the spectral unmixing result
                     (pixels need to range from 0-1)
    :param outputImg: Output image with the result of the rescaling (pixel values
                      will be in range 0-1 without scale factor > 1)
    :param gdalformat: the file format of the output file.
    :param calc_stats: if true then image statistics and pyramids for the output image.

    """
    from rios import applier

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
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyUnmixRescale(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        inputs.image[inputs.image < 0] = 0
        outputs.outimage = numpy.zeros_like(inputs.image, dtype=numpy.float32)

        for idx in range(inputs.image.shape[0]):
            outputs.outimage[idx] = inputs.image[idx] / numpy.sum(inputs.image, axis=0)

        outputs.outimage = numpy.where(
            numpy.isfinite(outputs.outimage), outputs.outimage, 0
        )

    applier.apply(_applyUnmixRescale, infiles, outfiles, otherargs, controls=aControls)

    if calc_stats:
        import rsgislib.imageutils

        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def predict_refl_linear_unmixing(
    in_unmix_coefs_img, endmembers_file, output_img, gdalformat="KEA", calc_stats=True
):
    """
    A function to calculate the predicted pixel value using the inputted
    abundances and endmembers.

    :param in_unmix_coefs_img: The unmixed abundance coefficients.
    :param endmembers_file: The endmembers file used for the unmixing in the
                            RSGISLib mtxt format.
    :param output_img: The file path to the GDAL writable output image
    :param gdalformat: The output image format to be used.
    :param calc_stats: If True (default) then image statistics and pyramids are
                       calculated for the output images

    """
    from rios import applier

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()
    endmembers_info = read_endmembers_mtxt(endmembers_file)

    infiles = applier.FilenameAssociations()
    infiles.image_unmix = in_unmix_coefs_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.endmembers_q = endmembers_info[0]
    otherargs.endmembers_p = endmembers_info[1]
    otherargs.endmembers_arr = endmembers_info[2]
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _predict_refl_img(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        img_flat = numpy.moveaxis(inputs.image_unmix, 0, 2).reshape(
            -1, inputs.image_unmix.shape[0]
        )

        pred_refl = numpy.zeros(
            (img_flat.shape[0], otherargs.endmembers_p), dtype=numpy.float32
        )
        for i in range(img_flat.shape[0]):
            for q in range(otherargs.endmembers_q):
                pred_refl[i] = pred_refl[i] + (
                    img_flat[i, q] * otherargs.endmembers_arr[q]
                )

        out_arr = pred_refl.reshape(
            inputs.image_unmix.shape[1],
            inputs.image_unmix.shape[2],
            otherargs.endmembers_p,
        )
        outputs.outimage = numpy.moveaxis(out_arr, 2, 0)

    applier.apply(_predict_refl_img, infiles, outfiles, otherargs, controls=aControls)

    if calc_stats:
        import rsgislib.imageutils

        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def calc_unmixing_rmse_residual_err(
    input_img,
    input_unmix_img,
    endmembers_file,
    output_img,
    gdalformat="KEA",
    calc_stats=True,
):
    """
    A function to calculate the prediction residual, RMSE and RMSPE using the defined
    abundances and endmembers.

    RMSE: Root Mean Squared Error
    RMSPE: Root Mean Squared Percentage Error

    Note. the residual and RMSE will be scaled with the input data so if you have
    multiplied your reflectance values by 1000 (i.e., range 0 - 1000) then the
    residual and RMSE values will also be in this range.

    :param input_refl_img: The original input reference image.
    :param input_unmix_coef_img: The unmixed abundance coefficients.
    :param endmembers_file: The endmembers file used for the unmixing in the
                            RSGISLib mtxt format.
    :param output_image: The file path to the GDAL writable output image
                         (Band 1: RMSE,  Band 2: RMSPE, Band 3: Residual)
    :param gdalformat: The output image format to be used.
    :param calc_stats: If True (default) then image statistics and pyramids are
                       calculated for the output images

    """
    from rios import applier

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    endmembers_info = read_endmembers_mtxt(endmembers_file)

    infiles = applier.FilenameAssociations()
    infiles.image_orig = input_img
    infiles.image_unmix = input_unmix_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.endmembers_q = endmembers_info[0]
    otherargs.endmembers_p = endmembers_info[1]
    otherargs.endmembers_arr = endmembers_info[2]
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _calc_unmix_err_rmse(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        block_refl_shp = inputs.image_orig.shape
        img_orig_refl_flat = numpy.moveaxis(inputs.image_orig, 0, 2).reshape(
            -1, inputs.image_orig.shape[0]
        )
        img_unmix_coef_flat = numpy.moveaxis(inputs.image_unmix, 0, 2).reshape(
            -1, inputs.image_unmix.shape[0]
        )
        img_flat_shp = img_orig_refl_flat.shape

        img_orig_refl_nodata = numpy.where(img_orig_refl_flat == 0, True, False)
        img_orig_refl_flat_nodata = numpy.all(img_orig_refl_nodata, axis=1)

        ID = numpy.arange(img_flat_shp[0])
        ID = ID[numpy.logical_not(img_orig_refl_flat_nodata)]
        img_orig_refl_flat_data = img_orig_refl_flat[
            numpy.logical_not(img_orig_refl_flat_nodata), ...
        ]
        img_unmix_coef_flat_data = img_unmix_coef_flat[
            numpy.logical_not(img_orig_refl_flat_nodata), ...
        ]

        img_nodata_flat_shp = img_unmix_coef_flat_data.shape

        pred_refl = numpy.zeros_like(img_orig_refl_flat_data, dtype=float)
        for i in range(img_nodata_flat_shp[0]):
            for q in range(otherargs.endmembers_q):
                pred_refl[i] = pred_refl[i] + (
                    img_unmix_coef_flat_data[i, q] * otherargs.endmembers_arr[q]
                )

        # Calc Diff
        diff = img_orig_refl_flat_data - pred_refl
        norm_diff = diff / img_orig_refl_flat_data

        # Calc RMSE
        diff_sq = numpy.square(diff)
        mean_sum_diff_sq = numpy.sum(diff_sq, axis=1) / otherargs.endmembers_p
        rmse_arr = numpy.sqrt(mean_sum_diff_sq)

        # Calc RMSPE
        norm_diff_sq = numpy.square(norm_diff)
        norm_mean_sum_diff_sq = numpy.sum(norm_diff_sq, axis=1) / otherargs.endmembers_p
        rmspe_arr = numpy.sqrt(norm_mean_sum_diff_sq) * 100.0

        # Calc Residual Error
        abs_diff = numpy.absolute(diff)
        residual_arr = numpy.sum(abs_diff, axis=1) / otherargs.endmembers_p

        outarr = numpy.zeros((img_flat_shp[0], 3))
        outarr[ID] = numpy.stack([rmse_arr, rmspe_arr, residual_arr], axis=-1)
        outarr = outarr.T
        outputs.outimage = outarr.reshape((3, block_refl_shp[1], block_refl_shp[2]))

    applier.apply(
        _calc_unmix_err_rmse, infiles, outfiles, otherargs, controls=aControls
    )

    if calc_stats:
        import rsgislib.imageutils

        rsgislib.imageutils.set_band_names(
            output_img, ["RMSE", "RMSPE", "Residual"], feedback=False
        )
        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data=False, no_data_val=0, calc_pyramids=True
        )


def summarise_multi_endmember_linear_unmixing(
    input_img,
    in_unmixed_datasets,
    out_unmix_img,
    out_ref_img,
    tmp_dir="tmp",
    gdalformat="KEA",
    calc_stats=True,
):
    """
    A function which merges multiple linear spectral unmixing results using different
    methods of unmixing, endmembers and number of endmembers so create a single
    combined unmixed result. The Root Mean Square Percentage Error (RMSPE) is used
    to identified which unmixing is the 'best' on a per-pixel basis.

    :param input_img: the original input reflectance image file.
    :param in_unmixed_datasets: a list of ImageEndmemberInfo objects with the unmixed
                                image, endmember file and list of endmember names.
    :param out_unmix_img: the output summarised unmixed image.
    :param out_ref_img: the output reference image referencing the image/endmembers
                        associated with each pixel. If the KEA format is used then
                        the RAT will be populated with the file path to the endmembers
    :param tmp_dir: a tmp directory where outputs will be written during processing and
                    can be deleted once processing is finished.
    :param gdalformat: output image file format (Default: KEA)
    :param calc_stats: Specify whether image pyramids and statistics will be calculated.
                       (Default: True). If True and the output file format is KEA then
                       the reference image RAT will be populated with the endmember
                       file names.

    """

    from rsgislib.tools import filetools
    import rsgislib.imagecalc
    import rsgislib.imageutils
    import rsgislib.rastergis

    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)

    if not isinstance(in_unmixed_datasets, list):
        raise rsgislib.RSGISPyException("in_unmixed_datasets must be a list")

    endmember_names = list()
    for unmixed_dataset in in_unmixed_datasets:
        if not isinstance(unmixed_dataset, ImageEndmemberInfo):
            raise rsgislib.RSGISPyException(
                "in_unmixed_datasets must contain a list of "
                "ImageEndmemberInfo instances."
            )
        if not isinstance(unmixed_dataset.endmember_names, list):
            raise rsgislib.RSGISPyException(
                "unmixed_dataset.endmember_names must be a list"
            )

        if rsgislib.imageutils.get_img_band_count(unmixed_dataset.in_unmix_img) != len(
            unmixed_dataset.endmember_names
        ):
            print(unmixed_dataset.in_unmix_img)
            print(unmixed_dataset.endmember_names)
            raise rsgislib.RSGISPyException(
                "The number of bands in the unmixed_dataset.in_unmix_img "
                " and the number of endmember names do not match"
            )

        for endmember_name in unmixed_dataset.endmember_names:
            if endmember_name not in endmember_names:
                endmember_names.append(endmember_name)
    print("There are {} endmembers: {}".format(len(endmember_names), endmember_names))

    endmember_name_bands = dict()
    band = 0
    for endmember_name in endmember_names:
        endmember_name_bands[endmember_name] = band
        band += 1

    print(endmember_name_bands)

    err_imgs_lst = list()
    endmember_files = list()
    for unmixed_dataset in in_unmixed_datasets:
        basename = filetools.get_file_basename(unmixed_dataset.in_unmix_img)
        out_err_img = os.path.join(tmp_dir, "{}_err_img.kea".format(basename))
        calc_unmixing_rmse_residual_err(
            input_img,
            unmixed_dataset.in_unmix_img,
            unmixed_dataset.endmembers_file,
            out_err_img,
            gdalformat="KEA",
            calc_stats=False,
        )
        out_rmspe_img = os.path.join(tmp_dir, "{}_rmspe_img.vrt".format(basename))
        rsgislib.imageutils.create_vrt_band_subset(out_err_img, [2], out_rmspe_img)
        err_imgs_lst.append(out_rmspe_img)
        endmember_files.append(unmixed_dataset.endmembers_file)

    rsgislib.imagecalc.get_img_idx_for_stat(
        err_imgs_lst, out_ref_img, gdalformat, -999, rsgislib.SUMTYPE_MIN
    )

    if calc_stats and gdalformat == "KEA":
        import rios.rat

        # Pop Ref Image with stats
        rsgislib.rastergis.pop_rat_img_stats(out_ref_img, True, True, True)

        # Open the clumps dataset as a gdal dataset
        ratDataset = gdal.Open(out_ref_img, gdal.GA_Update)
        # Write colours to RAT
        rios.rat.writeColumn(ratDataset, "Endmembers", endmember_files)
        ratDataset = None
    elif calc_stats:
        rsgislib.imageutils.pop_img_stats(
            out_ref_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )

    band_matched_unmixed_imgs = list()
    for unmixed_dataset in in_unmixed_datasets:
        basename = filetools.get_file_basename(unmixed_dataset.in_unmix_img)

        band_order_matches = True
        n_bands = rsgislib.imageutils.get_img_band_count(unmixed_dataset.in_unmix_img)
        if n_bands != len(endmember_names):
            band_order_matches = False

        if band_order_matches:
            for endmember_name, i in zip(
                unmixed_dataset.endmember_names, range(n_bands)
            ):
                if endmember_name_bands[endmember_name] != i:
                    band_order_matches = False
                    break

        if band_order_matches:
            band_matched_unmixed_imgs.append(unmixed_dataset.in_unmix_img)
        else:
            img_band_to_stack = dict()
            for endmember_name in endmember_name_bands:
                img_band_to_stack[endmember_name_bands[endmember_name]] = ""

            for endmember_name, i in zip(
                unmixed_dataset.endmember_names, range(n_bands)
            ):
                band_img = os.path.join(tmp_dir, "{}_band_{}.vrt".format(basename, i))
                rsgislib.imageutils.create_vrt_band_subset(
                    unmixed_dataset.in_unmix_img, [i + 1], band_img
                )
                img_band_to_stack[endmember_name_bands[endmember_name]] = band_img

            first = True
            zeros_img = os.path.join(tmp_dir, "{}_zeros_img.kea".format(basename))
            for img_band_idx in img_band_to_stack:
                if img_band_to_stack[img_band_idx] == "":
                    if first:
                        rsgislib.imageutils.create_copy_img(
                            unmixed_dataset.in_unmix_img,
                            zeros_img,
                            1,
                            0.0,
                            "KEA",
                            rsgislib.TYPE_32FLOAT,
                        )
                        first = False
                    img_band_to_stack[img_band_idx] = zeros_img

            band_to_stack = list()
            for img_band_idx in sorted(img_band_to_stack):
                band_to_stack.append(img_band_to_stack[img_band_idx])

            band_ordered_unmix_stack_img = os.path.join(
                tmp_dir, "{}_band_ordered_stack.vrt".format(basename)
            )
            rsgislib.imageutils.create_stack_images_vrt(
                band_to_stack, band_ordered_unmix_stack_img
            )
            band_matched_unmixed_imgs.append(band_ordered_unmix_stack_img)

    rsgislib.imageutils.create_ref_img_composite_img(
        band_matched_unmixed_imgs,
        out_unmix_img,
        out_ref_img,
        gdalformat,
        datatype=rsgislib.TYPE_32FLOAT,
        out_no_data=0.0,
    )

    if calc_stats:
        rsgislib.imageutils.set_band_names(
            out_unmix_img, endmember_names, feedback=False
        )
        rsgislib.imageutils.pop_img_stats(
            out_unmix_img, use_no_data=True, no_data_val=999, calc_pyramids=True
        )


def calc_ppi(
    input_img,
    output_img,
    gdalformat,
    niters=1000,
    lthres=0,
    uthres=0,
    img_gain=1,
    seed=None,
    calc_stats=True,
):
    """
    A function which calculate the pixel purity index (PPI). Using an appropriate
    number of iterations this can take a little while to run. Note, the whole input
    image is read into memory.

    It is recommended that you use the an MNF/PCA output and rescale that so all
    bands have the same range to improve the PPI result.

    Boardman J.W., Kruse F.A, and Green R.O., "Mapping Target Signatures via
        Partial Unmixing of AVIRIS Data," Pasadena, California, USA, 23 Jan 1995,
        URI: http://hdl.handle.net/2014/33635

    :param input_img: image values image file path.
    :param output_img: output image
    :param gdalformat: GDAL file format (e.g., KEA) of the output image.
    :param niters: number of iterations
    :param thres: a threshold in the image space (after again as been applied) to
                  select more pixels around the extreme (e.g., 1% reflectance)
    :param img_gain: the gain by which the image was multipled, reflectance images
                     are typically multiplied by 1000 or 10000. The result should be
                     an image with a range 0-1.
    :param seed: seed for the random squence of numbers being generated. Using the
                 same seed will result in the same seqence and therefore the
                 same output.
    :param calc_stats: whether to calculate image statistics and pyramids on
                       the output image.
    """
    # Check gdal is available
    import rsgislib.imageutils
    import tqdm

    imgDS = gdal.Open(input_img)
    if imgDS is None:
        raise rsgislib.RSGISPyException("Could not open input image")
    n_bands = imgDS.RasterCount
    x_size = imgDS.RasterXSize
    y_size = imgDS.RasterYSize
    img_data = numpy.zeros((n_bands, (x_size * y_size)), dtype=numpy.float32)
    img_data_msk = numpy.ones((x_size * y_size), dtype=bool)
    img_data_mean = numpy.zeros(n_bands, dtype=numpy.float32)

    print("Importing Bands:")
    for n in tqdm.tqdm(range(n_bands)):
        imgBand = imgDS.GetRasterBand(n + 1)
        if imgBand is None:
            raise rsgislib.RSGISPyException(
                "Could not open image band ({})".format(n + 1)
            )
        no_data_val = imgBand.GetNoDataValue()
        band_arr = imgBand.ReadAsArray().flatten()
        band_arr = band_arr.astype(numpy.float32)
        img_data[n] = band_arr
        img_data_msk[band_arr == no_data_val] = False
        band_arr[band_arr == no_data_val] = numpy.nan
        if img_gain > 1:
            band_arr = band_arr / img_gain
            img_data[n] = img_data[n] / img_gain
        img_data_mean[n] = numpy.nanmean(band_arr)
        img_data[n] = img_data[n] - img_data_mean[n]
    imgDS = None
    band_arr = None

    print("Create empty output image file")
    rsgislib.imageutils.create_copy_img(
        input_img, output_img, 1, 0, gdalformat, rsgislib.TYPE_16UINT
    )

    # Open output image
    outImgDS = gdal.Open(output_img, gdal.GA_Update)
    if outImgDS is None:
        raise rsgislib.RSGISPyException("Could not open output image")
    outImgBand = outImgDS.GetRasterBand(1)
    if outImgBand is None:
        raise rsgislib.RSGISPyException("Could not open output image band (1)")
    out_img_data = outImgBand.ReadAsArray()

    # Mask the datasets to obtain just the valid pixel values
    # (i.e., using the no data value)
    img_data = img_data.T
    out_img_data = out_img_data.flatten()
    pxl_idxs = numpy.arange(out_img_data.shape[0])
    pxl_idxs = pxl_idxs[img_data_msk]
    out_img_count = out_img_data[img_data_msk]
    img_data = img_data[img_data_msk]

    if seed is not None:
        numpy.random.seed(seed)

    print("Perform PPI iterations.")
    for i in tqdm.tqdm(range(niters)):
        r = numpy.random.rand(n_bands) - 0.5
        s = numpy.dot(img_data, r)

        imin = numpy.argmin(s)
        imax = numpy.argmax(s)
        if lthres == 0:
            # Only the two extreme pixels are incremented
            out_img_count[imin] += 1
        else:
            # All pixels within threshold distance from the two extremes
            out_img_count[s <= (s[imin] + lthres)] += 1

        if uthres == 0:
            # Only the two extreme pixels are incremented
            out_img_count[imax] += 1
        else:
            # All pixels within threshold distance from the two extremes
            out_img_count[s >= (s[imax] - uthres)] += 1
    s = None

    out_img_data[pxl_idxs] = out_img_count
    out_img_data = out_img_data.reshape((y_size, x_size))

    outImgBand.WriteArray(out_img_data)
    outImgDS = None

    if calc_stats:
        print("Calculate Image stats and pyramids.")
        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )
