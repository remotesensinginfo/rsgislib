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

import numpy

import rsgislib


def readEndmembersMTXT(endmembers_file, gain=1, weight=None):
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


def specUnmixSpTsUCLS(
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
                            rsgislib.zonalstats.extractAvgEndMembers function
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

    m, n, endmembers_arr = readEndmembersMTXT(endmembers_file, gain, weight)

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
        rsgislib.imageutils.popImageStats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def specUnmixSpTsNNLS(
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
                            rsgislib.zonalstats.extractAvgEndMembers function
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

    m, n, endmembers_arr = readEndmembersMTXT(endmembers_file, gain, weight)

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
        rsgislib.imageutils.popImageStats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def specUnmixSpTsFCLS(
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
                            rsgislib.zonalstats.extractAvgEndMembers function
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

    m, n, endmembers_arr = readEndmembersMTXT(endmembers_file, gain)

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
        rsgislib.imageutils.popImageStats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def specUnmixPyMcrNNLS(
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
                            rsgislib.zonalstats.extractAvgEndMembers function
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

    m, n, endmembers_arr = readEndmembersMTXT(endmembers_file, gain, weight)

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
        rsgislib.imageutils.popImageStats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def specUnmixPyMcrFCLS(
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
                            rsgislib.zonalstats.extractAvgEndMembers function
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

    m, n, endmembers_arr = readEndmembersMTXT(endmembers_file, gain)

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
        rsgislib.imageutils.popImageStats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def rescaleUnmixingResults(input_img, output_img, gdalformat="KEA", calc_stats=True):
    """
    A function which rescales an output from a spectral unmixing
    (e.g., rsgislib.imagecalc.specunmixing.specUnmixSpTsUCLS) so that
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
        rsgislib.imageutils.popImageStats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def predictReflLinearUnmixing(
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
    endmembers_info = readEndmembersMTXT(endmembers_file)

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

        rsgislib.imageutils.popImageStats(
            output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def calcUnmixingRMSEResidualErr(
    input_img,
    input_unmix_img,
    endmembers_file,
    output_img,
    gdalformat="KEA",
    calc_stats=True,
):
    """
    A function to calculate the prediction residual and RMSE using the defined
    abundances and endmembers.

    :param input_refl_img: The original input reference image.
    :param input_unmix_coef_img: The unmixed abundance coefficients.
    :param endmembers_file: The endmembers file used for the unmixing in the
                            RSGISLib mtxt format.
    :param output_image: The file path to the GDAL writable output image
                         (Band 1: RMSE, Band 2: Residual)
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

    endmembers_info = readEndmembersMTXT(endmembers_file)

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

        # Calc RMSE
        diff_sq = diff * diff
        mean_sum_diff_sq = numpy.sum(diff_sq, axis=1) / otherargs.endmembers_p
        rmse_arr = numpy.sqrt(mean_sum_diff_sq)

        # Calc Residual Error
        abs_diff = numpy.absolute(diff)
        residual_arr = numpy.sum(abs_diff, axis=1) / otherargs.endmembers_p

        outarr = numpy.zeros((img_flat_shp[0], 2))
        outarr[ID] = numpy.stack([rmse_arr, residual_arr], axis=-1)
        outarr = outarr.T
        outputs.outimage = outarr.reshape((2, block_refl_shp[1], block_refl_shp[2]))

    applier.apply(
        _calc_unmix_err_rmse, infiles, outfiles, otherargs, controls=aControls
    )

    if calc_stats:
        import rsgislib.imageutils

        rsgislib.imageutils.setBandNames(
            output_img, ["RMSE", "Residual"], feedback=False
        )
        rsgislib.imageutils.popImageStats(
            output_img, use_no_data=False, no_data_val=0, calc_pyramids=True
        )
