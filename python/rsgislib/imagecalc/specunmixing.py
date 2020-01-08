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
# Date: 07/01/2017
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
############################################################################

# UCLS, NNLS, FCLS classes adapted from pysptools module:
# https://github.com/ctherien/pysptools
# which has has the following license:
# ------------------------------------------------------------------------------
# Copyright (c) 2013-2014, Christian Therien
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ------------------------------------------------------------------------------
#
from abc import ABCMeta, abstractmethod
import numpy
from rios import applier
from rios import cuiprogress
import rsgislib


class UnmixingImpl(object):
    """
    An abstract class which can be used to implement various
    spectral unmixing implementations and use through the same
    interface.
    """
    __metaclass__ = ABCMeta

    @abstractmethod
    def calc_abundance(self, M, U):
        """
        An abstract function which implements a method for spectral unmixing.
        N is the number of image pixels being processed. 
        p is the number of image bands
        q is the number of endmembers.

        :param M: `numpy array` 2D data matrix (N x p).
        :param U: `numpy array` 2D matrix of endmembers (q x p).
        :return: `numpy array` An abundance maps (N x q).
        """
        pass

    def read_endmembers_mtxt(self, endmembers_file):
        """
        A function to read endmembers mtxt file
        :param endmembers_file: File path to the mtxt input file.
        :return: [m, n, endmemarr]; m size, n size and a numpy array with the end members
        """
        dataList = []
        dataFile = open(endmembers_file, 'r')
        for line in dataFile:
            line = line.strip()
            if line != "":
                dataList.append(line)
        dataFile.close()
        m = int(dataList[0].split('=')[-1])
        n = int(dataList[1].split('=')[-1])
        endmemarr = numpy.zeros((m, n))
        datavals = dataList[2].split(',')

        i = 0
        for m_idx in range(m):
            for n_idx in range(n):
                endmemarr[m_idx, n_idx] = float(datavals[i])
                i = i + 1

        return m, n, endmemarr

    def read_endmembers_mtxt_weight(self, endmembers_file, weight=100):
        """
        A function to read endmembers mtxt file
        :param endmembers_file: File path to the mtxt input file.
        :return: [m, n, endmemarr]; m size, n size and a numpy array with the end members
        """
        dataList = []
        dataFile = open(endmembers_file, 'r')
        for line in dataFile:
            line = line.strip()
            if line != "":
                dataList.append(line)
        dataFile.close()
        m = int(dataList[0].split('=')[-1])
        n = int(dataList[1].split('=')[-1]) + 1
        endmemarr = numpy.zeros((m, n))
        datavals = dataList[2].split(',')

        i = 0
        for m_idx in range(m):
            for n_idx in range(n - 1):
                endmemarr[m_idx, n_idx] = float(datavals[i])
                i = i + 1
            endmemarr[m_idx, n - 1] = weight

        return m, n, endmemarr

    def perform_simple_unmixing(self, input_image, output_image, gdalformat, endmembers_file, weight=None,
                                scale_factor=1000):
        """
        A function which uses the RIOS to iterate through the input image
        and perform a simple/standard spectral unmixing on the input image
        using the calc_abundance function.

        :param input_image: The file path to a GDAL readable input image.
        :param output_image: The file path to the GDAL writable output image
                             (Note pixel values will be between 0-1000)
        :param gdalformat: The output image format to be used.
        :param endmembers_file: The endmembers for the unmixing in the RSGISLib mtxt format.
        :param weight: Optional (if None ignored) to provide a weight to implement the approach of Scarth et al (2010)
                   adding a weight to the least squares optimisation to get the abundances to sum to 1.
        :param scale_factor: Scale factor for integerising the resulting image. If value is 1 then output image
                             will be a floating point image.

        References:
            Scarth, P., Roder, A., & Schmidt, M. (2010). Tracking grazing pressure and climate
            interaction—The role of Landsat fractional cover in time series analysis. Proceedings of
            the 15th Australasian Remote Sensing and Photogrammetry ConferenceAustralia: Alice Springs.
            http://dx.doi.org/10.6084/m9.ﬁgshare.94250.

        """
        try:
            import tqdm
            progress_bar = rsgislib.TQDMProgressBar()
        except:
            progress_bar = cuiprogress.GDALProgressBar()

        if weight is not None:
            endmembers_info = self.read_endmembers_mtxt_weight(endmembers_file, weight)
        else:
            endmembers_info = self.read_endmembers_mtxt(endmembers_file)
        print(endmembers_info)

        infiles = applier.FilenameAssociations()
        infiles.image = input_image
        outfiles = applier.FilenameAssociations()
        outfiles.outimage = output_image
        otherargs = applier.OtherInputs()
        otherargs.endmembers_q = endmembers_info[0]
        otherargs.endmembers_p = endmembers_info[1]
        otherargs.endmembers_arr = endmembers_info[2]
        otherargs.weight = weight
        otherargs.scale_factor = scale_factor
        aControls = applier.ApplierControls()
        aControls.progress = progress_bar
        aControls.drivername = gdalformat
        aControls.omitPyramids = True
        aControls.calcStats = False

        def _simple_unmix(info, inputs, outputs, otherargs):
            """
            This is an internal rios function
            """
            block_shp = inputs.image.shape
            img_flat = inputs.image.reshape([block_shp[0], (block_shp[1] * block_shp[2])]).T
            if otherargs.weight is not None:
                img_flat_dtype = img_flat.dtype
                tmp_img_flat = img_flat
                img_flat = numpy.zeros((img_flat.shape[0], img_flat.shape[1] + 1), dtype=img_flat_dtype)
                img_flat[...] = weight
                img_flat[:, :-1] = tmp_img_flat
            img_flat_shp = img_flat.shape

            img_nodata = numpy.where(img_flat == 0, True, False)
            img_flat_nodata = numpy.all(img_nodata, axis=1)

            ID = numpy.arange(img_flat_shp[0])
            ID = ID[img_flat_nodata == False]
            img_flat_data = img_flat[img_flat_nodata == False, ...]

            abundances_arr = self.calc_abundance(img_flat_data, otherargs.endmembers_arr)
            if otherargs.scale_factor > 1:
                outarr = numpy.zeros([img_flat_shp[0], otherargs.endmembers_q], dtype=numpy.int16)
            else:
                outarr = numpy.zeros([img_flat_shp[0], otherargs.endmembers_q], dtype=numpy.float32)
            outarr[ID] = (abundances_arr * otherargs.scale_factor)
            outarr = outarr.T
            outputs.outimage = outarr.reshape((otherargs.endmembers_q, block_shp[1], block_shp[2]))

        applier.apply(_simple_unmix, infiles, outfiles, otherargs, controls=aControls)

    def predict_refl_img_from_simple_unmixing(self, input_unmix_img, endmembers_file, output_img, gdalformat,
                                              scale_factor=1000):
        """
        """
        try:
            import tqdm
            progress_bar = rsgislib.TQDMProgressBar()
        except:
            progress_bar = cuiprogress.GDALProgressBar()

        endmembers_info = self.read_endmembers_mtxt(endmembers_file)

        infiles = applier.FilenameAssociations()
        infiles.image_unmix = input_unmix_img
        outfiles = applier.FilenameAssociations()
        outfiles.outimage = output_img
        otherargs = applier.OtherInputs()
        otherargs.endmembers_q = endmembers_info[0]
        otherargs.endmembers_p = endmembers_info[1]
        otherargs.endmembers_arr = endmembers_info[2]
        otherargs.scale_factor = scale_factor
        aControls = applier.ApplierControls()
        aControls.progress = progress_bar
        aControls.drivername = gdalformat
        aControls.omitPyramids = True
        aControls.calcStats = False

        def _predict_refl_img(info, inputs, outputs, otherargs):
            """
            This is an internal rios function
            """
            block_unmix_shp = inputs.image_unmix.shape
            img_unmix_coef_flat = inputs.image_unmix.reshape(
                [block_unmix_shp[0], (block_unmix_shp[1] * block_unmix_shp[2])]).T

            img_unmix_coef_flat_data_flt = numpy.zeros_like(img_unmix_coef_flat, dtype=numpy.float32)
            img_unmix_coef_flat_data_flt[...] = img_unmix_coef_flat / float(scale_factor)

            img_nodata_flat_shp = img_unmix_coef_flat_data_flt.shape

            pred_refl_img = numpy.zeros((img_unmix_coef_flat.shape[0], otherargs.endmembers_p), dtype=numpy.int16)
            for i in range(img_unmix_coef_flat.shape[0]):
                for q in range(otherargs.endmembers_q):
                    pred_refl_img[i] = pred_refl_img[i] + (
                                img_unmix_coef_flat_data_flt[i, q] * otherargs.endmembers_arr[q])

            pred_refl_img = pred_refl_img.T
            outputs.outimage = pred_refl_img.reshape((otherargs.endmembers_p, block_unmix_shp[1], block_unmix_shp[2]))

        applier.apply(_predict_refl_img, infiles, outfiles, otherargs, controls=aControls)

    def calc_unmixing_rmse_residualerr(self, input_img, input_unmix_img, endmembers_file, output_img, gdalformat,
                                       scale_factor=1000):
        """
        """
        try:
            import tqdm
            progress_bar = rsgislib.TQDMProgressBar()
        except:
            progress_bar = cuiprogress.GDALProgressBar()

        endmembers_info = self.read_endmembers_mtxt(endmembers_file)

        infiles = applier.FilenameAssociations()
        infiles.image_orig = input_img
        infiles.image_unmix = input_unmix_img
        outfiles = applier.FilenameAssociations()
        outfiles.outimage = output_img
        otherargs = applier.OtherInputs()
        otherargs.endmembers_q = endmembers_info[0]
        otherargs.endmembers_p = endmembers_info[1]
        otherargs.endmembers_arr = endmembers_info[2]
        otherargs.scale_factor = scale_factor
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
            img_orig_refl_flat = inputs.image_orig.reshape(
                [block_refl_shp[0], (block_refl_shp[1] * block_refl_shp[2])]).T
            block_unmix_shp = inputs.image_unmix.shape
            img_unmix_coef_flat = inputs.image_unmix.reshape(
                [block_unmix_shp[0], (block_unmix_shp[1] * block_unmix_shp[2])]).T
            img_flat_shp = img_orig_refl_flat.shape

            img_orig_refl_nodata = numpy.where(img_orig_refl_flat == 0, True, False)
            img_orig_refl_flat_nodata = numpy.all(img_orig_refl_nodata, axis=1)

            ID = numpy.arange(img_flat_shp[0])
            ID = ID[img_orig_refl_flat_nodata == False]
            img_orig_refl_flat_data = img_orig_refl_flat[img_orig_refl_flat_nodata == False, ...]
            img_orig_refl_flat_data_flt = numpy.zeros_like(img_orig_refl_flat_data, dtype=numpy.float32)
            img_orig_refl_flat_data_flt[...] = img_orig_refl_flat_data
            img_unmix_coef_flat_data = img_unmix_coef_flat[img_orig_refl_flat_nodata == False, ...]
            img_unmix_coef_flat_data_flt = numpy.zeros_like(img_unmix_coef_flat_data, dtype=numpy.float32)
            img_unmix_coef_flat_data_flt[...] = img_unmix_coef_flat_data / float(scale_factor)

            img_nodata_flat_shp = img_unmix_coef_flat_data_flt.shape

            pred_refl_img = numpy.zeros_like(img_orig_refl_flat_data, dtype=float)
            for i in range(img_nodata_flat_shp[0]):
                for q in range(otherargs.endmembers_q):
                    pred_refl_img[i] = pred_refl_img[i] + (
                                img_unmix_coef_flat_data_flt[i, q] * otherargs.endmembers_arr[q])

            # Calc Diff
            diff = img_orig_refl_flat_data_flt - pred_refl_img

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

        applier.apply(_calc_unmix_err_rmse, infiles, outfiles, otherargs, controls=aControls)


class Unmixing_UCLS(UnmixingImpl):

    def calc_abundance(self, M, U):
        """
        Performs unconstrained least squares abundance estimation.

        :param M: `numpy array` 2D data matrix (N x p).
        :param U: `numpy array` 2D matrix of endmembers (q x p).
        :return: `numpy array` An abundance maps (N x q).
        """
        Uinv = numpy.linalg.pinv(U.T)
        return numpy.dot(Uinv, M[0:, :].T).T


class Unmixing_NNLS(UnmixingImpl):

    def calc_abundance(self, M, U):
        """
        NNLS performs non-negative constrained least squares of each pixel
        in M using the endmember signatures of U.  Non-negative constrained least
        squares with the abundance nonnegative constraint (ANC).
        Utilizes the method of Bro and de Jong (2007).

        References:
            Bro R., de Jong S., Journal of Chemometrics, 1997, 11, 393-401.

        :param M: `numpy array` 2D data matrix (N x p).
        :param U: `numpy array` 2D matrix of endmembers (q x p).
        :return: `numpy array` An abundance maps (N x q).

        """
        import scipy.optimize as opt

        N, p1 = M.shape
        q, p2 = U.shape

        X = numpy.zeros((N, q), dtype=numpy.float32)
        MtM = numpy.dot(U, U.T)
        for n1 in range(N):
            # opt.nnls() return a tuple, the first element is the result
            X[n1] = opt.nnls(MtM, numpy.dot(U, M[n1]))[0]
        return X


class Unmixing_FCLS(UnmixingImpl):

    def _numpy_None_vstack(self, A1, A2):
        if A1 is None:
            return A2
        else:
            return numpy.vstack([A1, A2])

    def _numpy_None_concatenate(self, A1, A2):
        if A1 is None:
            return A2
        else:
            return numpy.concatenate([A1, A2])

    def _numpy_to_cvxopt_matrix(self, A):
        from cvxopt import matrix
        A = numpy.array(A, dtype=numpy.float64)
        if A.ndim == 1:
            return matrix(A, (A.shape[0], 1), 'd')
        else:
            return matrix(A, A.shape, 'd')

    def calc_abundance(self, M, U):
        """
        Performs fully constrained least squares of each pixel in M
        using the endmember signatures of U. Fully constrained least squares
        is least squares with the abundance sum-to-one constraint (ASC) and the
        abundance nonnegative constraint (ANC).

        References:
             Daniel Heinz, Chein-I Chang, and Mark L.G. Fully Constrained
             Least-Squares Based Linear Unmixing. Althouse. IEEE. 1999.

        Notes:
             This code was taken from https://github.com/ctherien/pysptools, which
             used the following threes sources to build this implementation of the
             algorithm:
                 * The function hyperFclsMatlab, part of the Matlab Hyperspectral
                   Toolbox of Isaac Gerg.
                 * The Matlab (tm) help on lsqlin.
                 * And the Python implementation of lsqlin by Valera Vishnevskiy:
                   http://maggotroot.blogspot.ca/2013/11/constrained-linear-least-squares-in.html

        :param M: `numpy array` 2D data matrix (N x p).
        :param U: `numpy array` 2D matrix of endmembers (q x p).
        :return: `numpy array` An abundance maps (N x q).

        """
        from cvxopt import solvers, matrix
        solvers.options['show_progress'] = False
        N, p1 = M.shape
        nvars, p2 = U.shape

        C = self._numpy_to_cvxopt_matrix(U.T)
        Q = C.T * C

        lb_A = -numpy.eye(nvars)
        lb = numpy.repeat(0, nvars)
        A = self._numpy_None_vstack(None, lb_A)
        b = self._numpy_None_concatenate(None, -lb)
        A = self._numpy_to_cvxopt_matrix(A)
        b = self._numpy_to_cvxopt_matrix(b)

        Aeq = self._numpy_to_cvxopt_matrix(numpy.ones((1, nvars)))
        beq = self._numpy_to_cvxopt_matrix(numpy.ones(1))

        M = numpy.array(M, dtype=numpy.float64)
        X = numpy.zeros((N, nvars), dtype=numpy.float32)
        for n1 in range(N):
            d = matrix(M[n1], (p1, 1), 'd')
            q = - d.T * C
            sol = solvers.qp(Q, q.T, A, b, Aeq, beq, None, None)['x']
            X[n1] = numpy.array(sol).squeeze()
        return X


def rescale_unmixing_results(input_img, output_img, gdalformat, scale_factor=1000):
    """
    A function which rescales an output from a spectral unmixing
    (e.g., rsgislib.imagecalc.specunmixing.calc_unconstrained_unmixing) so that
    negative values are removed and each pixel sums to 1.

    :param inputImg: Input image with the spectral unmixing result (pixels need to range from 0-1)
    :param outputImg: Output image with the result of the rescaling (pixel values will be in range 0-1)
    :param gdalformat: the file format of the output file.

    """
    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.scale_factor = scale_factor
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
        if otherargs.scale_factor == 1:
            outputs.outimage = numpy.zeros_like(inputs.image, dtype=numpy.float32)
        else:
            outputs.outimage = numpy.zeros_like(inputs.image, dtype=numpy.uint16)
        for idx in range(inputs.image.shape[0]):
            outputs.outimage[idx] = (inputs.image[idx] / numpy.sum(inputs.image, axis=0)) * otherargs.scale_factor

    applier.apply(_applyUnmixRescale, infiles, outfiles, otherargs, controls=aControls)


def calc_unconstrained_unmixing(input_img, output_img, gdalformat, endmembers_file, weight=None,
                                rescaled_output_img=None, calc_stats=True, scale_factor=1000):
    """
    A function which uses the Unmixing_UCLS class to perform an uncontrained spectral
    unmixing.

    :param input_image: The file path to a GDAL readable input image.
    :param output_image: The file path to the GDAL writable output image
                         (Note pixel values will be between 0-1000)
    :param gdalformat: The output image format to be used.
    :param endmembers_file: The endmembers for the unmixing in the RSGISLib mtxt format.
    :param weight: Optional (if None ignored) to provide a weight to implement the approach of Scarth et al (2010)
                   adding a weight to the least squares optimisation to get the abundances to sum to 1.
    :param rescaled_output_img: A file path to an output image which has been rescaled so the abundances sum to 1.
                                Optional, if None then ignored.
    :param calc_stats: If True (default) then image statistics and pyramids are calculated for the output images
    :param scale_factor: Scale factor for integerising the resulting image. If value is 1 then output image
                             will be a floating point image.

    References:
        Scarth, P., Roder, A., & Schmidt, M. (2010). Tracking grazing pressure and climate
        interaction—The role of Landsat fractional cover in time series analysis. Proceedings of
        the 15th Australasian Remote Sensing and Photogrammetry ConferenceAustralia: Alice Springs.
        http://dx.doi.org/10.6084/m9.ﬁgshare.94250.

    """
    unmix_obj = Unmixing_UCLS()
    unmix_obj.perform_simple_unmixing(input_img, output_img, gdalformat, endmembers_file, weight, scale_factor)
    if calc_stats:
        import rsgislib.imageutils
        rsgislib.imageutils.popImageStats(output_img, usenodataval=True, nodataval=(scale_factor + 1),
                                          calcpyramids=True)

    if rescaled_output_img is not None:
        rescale_unmixing_results(output_img, rescaled_output_img, gdalformat, scale_factor=scale_factor)
        if calc_stats:
            rsgislib.imageutils.popImageStats(rescaled_output_img, usenodataval=True, nodataval=(scale_factor + 1),
                                              calcpyramids=True)


def calc_non_negative_unmixing(input_img, output_img, gdalformat, endmembers_file, weight=None,
                               rescaled_output_img=None, calc_stats=True, scale_factor=1000):
    """
    A function which uses the Unmixing_NNLS class to perform an non-negative constrained
    spectral unmixing.

    :param input_image: The file path to a GDAL readable input image.
    :param output_image: The file path to the GDAL writable output image
                         (Note pixel values will be between 0-1000)
    :param gdalformat: The output image format to be used.
    :param endmembers_file: The endmembers for the unmixing in the RSGISLib mtxt format.
    :param weight: Optional (if None ignored) to provide a weight to implement the approach of Scarth et al (2010)
                   adding a weight to the least squares optimisation to get the abundances to sum to 1.
    :param rescaled_output_img: A file path to an output image which has been rescaled so the abundances sum to 1.
                                Optional, if None then ignored.
    :param calc_stats: If True (default) then image statistics and pyramids are calculated for the output images
    :param scale_factor: Scale factor for integerising the resulting image. If value is 1 then output image
                             will be a floating point image.

    References:
        Scarth, P., Roder, A., & Schmidt, M. (2010). Tracking grazing pressure and climate
        interaction—The role of Landsat fractional cover in time series analysis. Proceedings of
        the 15th Australasian Remote Sensing and Photogrammetry ConferenceAustralia: Alice Springs.
        http://dx.doi.org/10.6084/m9.ﬁgshare.94250.

    """
    unmix_obj = Unmixing_NNLS()
    unmix_obj.perform_simple_unmixing(input_img, output_img, gdalformat, endmembers_file, weight, scale_factor)
    if calc_stats:
        import rsgislib.imageutils
        rsgislib.imageutils.popImageStats(output_img, usenodataval=True, nodataval=(scale_factor + 1),
                                          calcpyramids=True)
    if rescaled_output_img is not None:
        rescale_unmixing_results(output_img, rescaled_output_img, gdalformat, scale_factor=scale_factor)
        if calc_stats:
            rsgislib.imageutils.popImageStats(rescaled_output_img, usenodataval=True, nodataval=(scale_factor + 1),
                                              calcpyramids=True)


def calc_fully_constrained_unmixing(input_img, output_img, gdalformat, endmembers_file, calc_stats=True,
                                    scale_factor=1000):
    """
    A function which uses the Unmixing_FCLS class to perform a fully contrained spectral
    unmixing.

    :param input_image: The file path to a GDAL readable input image.
    :param output_image: The file path to the GDAL writable output image
                         (Note pixel values will be between 0-1000)
    :param gdalformat: The output image format to be used.
    :param endmembers_file: The endmembers for the unmixing in the RSGISLib mtxt format.
    :param calc_stats: If True (default) then image statistics and pyramids are calculated for the output images
    :param scale_factor: Scale factor for integerising the resulting image. If value is 1 then output image
                             will be a floating point image.

    """
    unmix_obj = Unmixing_FCLS()
    unmix_obj.perform_simple_unmixing(input_img, output_img, gdalformat, endmembers_file, scale_factor=scale_factor)
    if calc_stats:
        import rsgislib.imageutils
        rsgislib.imageutils.popImageStats(output_img, usenodataval=True, nodataval=(scale_factor + 1),
                                          calcpyramids=True)


def calc_unmix_rmse_residualerr(input_refl_img, input_unmix_coef_img, endmembers_file, output_img, gdalformat,
                                scale_factor=1000, calc_stats=True):
    """
    A function to calculate the prediction residual and RMSE using the defined abundances and endmembers.

    :param input_refl_img: The original input reference image.
    :param input_unmix_coef_img: The unmixed abundance coefficients.
    :param endmembers_file: The endmembers file used for the unmixing in the RSGISLib mtxt format.
    :param output_image: The file path to the GDAL writable output image (Band 1: RMSE, Band 2: Residual)
    :param gdalformat: The output image format to be used.
    :param scale_factor: Scale factor used to integerising the endmember abundances image.
    :param calc_stats: If True (default) then image statistics and pyramids are calculated for the output images

    """
    unmix_obj = Unmixing_UCLS()
    unmix_obj.calc_unmixing_rmse_residualerr(input_refl_img, input_unmix_coef_img, endmembers_file, output_img,
                                             gdalformat, scale_factor)
    if calc_stats:
        import rsgislib.imageutils
        rsgislib.imageutils.setBandNames(output_img, ['RMSE', 'Residual'], feedback=False)
        rsgislib.imageutils.popImageStats(output_img, usenodataval=False, nodataval=0, calcpyramids=True)


def predict_refl_img_from_simple_unmixing(input_unmix_coef_img, endmembers_file, output_img, gdalformat,
                                          scale_factor=1000, calc_stats=True):
    """
    A function to calculate the predicted pixel value using the inputted abundances and endmembers.

    :param input_unmix_coef_img: The unmixed abundance coefficients.
    :param endmembers_file: The endmembers file used for the unmixing in the RSGISLib mtxt format.
    :param output_image: The file path to the GDAL writable output image
    :param gdalformat: The output image format to be used.
    :param scale_factor: Scale factor used to integerising the endmember abundances image.
    :param calc_stats: If True (default) then image statistics and pyramids are calculated for the output images

    """
    unmix_obj = Unmixing_UCLS()
    unmix_obj.predict_refl_img_from_simple_unmixing(input_unmix_coef_img, endmembers_file, output_img, gdalformat,
                                                    scale_factor)
    if calc_stats:
        import rsgislib.imageutils
        rsgislib.imageutils.popImageStats(output_img, usenodataval=True, nodataval=0, calcpyramids=True)


