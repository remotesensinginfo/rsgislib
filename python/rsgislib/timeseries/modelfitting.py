#! /usr/bin/env python
############################################################################
#  modelfitting.py
#
#  Copyright 2019 RSGISLib.
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
# Purpose:  Provides functions for processing dense time series data
#           through model fitting.
#           See https://github.com/klh5/rios_st_model
#
# Author: Katie Awty-Carroll (ed by Pete Bunting)
# Email: petebunting@mac.com
# Date: 1/8/2019
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

import sys
import json
from datetime import datetime

import numpy

from rios import applier
from rios import fileinfo
from rios import cuiprogress

from sklearn import linear_model

# could have two functions- one for Lasso, one for OLS
# all of the stored class variables should work for either model type
class MakeSeasonTrendModel(object):
    """
    Class containing all information and functions relating to fitting a
    season-trend model to a single pixel.
    """

    def __init__(self, datetimes, band_data):

        self.T = 365.25
        self.pi_val_simple = (2 * numpy.pi) / self.T
        self.pi_val_advanced = (4 * numpy.pi) / self.T
        self.pi_val_full = (6 * numpy.pi) / self.T
        self.datetimes = datetimes
        self.band_data = band_data

        self.st_model = None  # Model object
        self.residuals = None
        self.RMSE = None
        self.coefficients = None
        self.predicted = None
        self.alpha = None  # Needed to store alpha if CV is used
        self.num_obs = len(datetimes)

        # Get minimum/earliest date. This is used to rescale dates so that they
        # start from 0
        self.start_date = numpy.min(self.datetimes)

        # Rescale dates to start from 0
        rescaled = self.datetimes - self.start_date

        # Complexity of fit is based on Zhu et al. 2015: Generating synthetic Landsat images based on all available Landsat data: Predicting Landsat surface reflectance at any given time.
        # There should be at least three times more data points that the number of coefficients.

        # Less than 18 observations but at least 12. Fit one harmonic term (simple model, four coefficients inc. intercept)
        x = numpy.array([rescaled,
                      numpy.cos(self.pi_val_simple * rescaled),
                      numpy.sin(self.pi_val_simple * rescaled)])

        # 18 or more observations. Fit two harmonic terms (advanced model, six coefficients)
        if (self.num_obs >= 18):
            x = numpy.vstack((x, numpy.array([numpy.cos(self.pi_val_advanced * rescaled),
                                              numpy.sin(self.pi_val_advanced * rescaled)])))

        # 24 or more observations. Fit three harmonic terms (full model, eight coefficients)
        if (self.num_obs >= 24):
            x = numpy.vstack((x, numpy.array([numpy.cos(self.pi_val_full * rescaled),
                                              numpy.sin(self.pi_val_full * rescaled)])))

        self.x = x.T

    def getRMSE(self):
        """
        A function which calculates the Root Mean Squared error (RMSE) of the model.
        """
        self.predicted = self.model.predict(self.x)
        self.coefficients = self.model.coef_
        self.residuals = self.band_data - self.predicted
        # Get overall RMSE of model
        self.RMSE = numpy.sqrt(numpy.mean(self.residuals ** 2))

    def fit_lasso_model(self, cv, alpha):
        """
        Given a 1D time series of values, fit a Lasso model to the data and
        store the resulting model coefficients and Root Mean Square Error.
        """
        if (cv):  # If cross validation should be used to find alpha parameter
            self.model = linear_model.LassoCV(fit_intercept=True).fit(self.x, self.band_data)
            self.alpha = self.model.alpha_
        else:
            self.model = linear_model.Lasso(fit_intercept=True, alpha=alpha).fit(self.x, self.band_data)
            self.alpha = alpha
        self.getRMSE()

    def fit_ols_model(self):
        """
        Given a 1D time series of values, fit an OLS model to the data and
        store the resulting model coefficients and Root Mean Square Error.
        """
        self.model = linear_model.LinearRegression(fit_intercept=True).fit(self.x, self.band_data)
        self.getRMSE()


def gen_per_band_models(info, inputs, outputs, other_args):
    """
Run per-block by RIOS. In this case each block is a
single pixel. Given a block of values for each band for each date, returns
a numpy array containing the model coefficients, RMSE, and an overall
value for each band.
"""
    nodata_val = other_args.nodata_val
    num_bands = other_args.num_bands

    # Calculate number of outputs
    num_outputs = num_bands * 11

    # Set up array with the correct output shape
    px_out = numpy.zeros((num_outputs, 1, 1), dtype='float64')

    # Keep track of which layer to write to in the output file
    layer = 0

    # Get data for one band at a time
    for band in range(0, num_bands):
        band_data = numpy.array([inputs.images[t][band][0][0] for t in range(0, len(inputs.images))])

        # Get indices of missing values
        mask = numpy.where(band_data == nodata_val)

        # Drop missing data points from band data
        masked = numpy.delete(band_data, mask)

        # Check if any data is left once no data values have been removed
        if masked.size >= 6:
            # Drop missing data points from dates
            masked_dates = numpy.delete(other_args.dates, mask)

            # Initialise model class
            st_model = MakeSeasonTrendModel(masked_dates, masked)

            # If Lasso model is chosen, fit the model using the variables provided
            # By default, alpha=1 and cross validation is not done
            if (other_args.model_type == 'Lasso'):
                st_model.fit_lasso_model(other_args.cv, other_args.alpha)

            else:
                st_model.fit_ols_model()

            # Extract coefficients for output
            coeffs = st_model.coefficients  # Slope, cos1, sin1, cos2, sin2, cos3, sin3
            slope = coeffs[0]
            intercept = st_model.model.intercept_
            px_out[layer] = slope
            px_out[layer + 1] = intercept

            # Pad out coefficients
            # Some models might not have second or third harmonic terms - these are set to 0 to allow the same classifier
            # to be used
            coeffs = numpy.pad(coeffs, (0, 7 - len(coeffs)), 'constant')

            # Add harmonic coefficients to output
            px_out[layer + 2] = coeffs[1]
            px_out[layer + 3] = coeffs[2]
            px_out[layer + 4] = coeffs[3]
            px_out[layer + 5] = coeffs[4]
            px_out[layer + 6] = coeffs[5]
            px_out[layer + 7] = coeffs[6]

            px_out[layer + 8] = st_model.RMSE

            # Get middle date
            mid_ts = (masked_dates[-1] - masked_dates[0]) / 2

            # Calculate overall value for period
            intercept = st_model.model.intercept_
            overall_val = intercept + (slope * mid_ts)

            px_out[layer + 9][0][0] = overall_val
            px_out[layer + 10] = st_model.start_date

        layer += 11  # There are always 11 outputs per band
    outputs.outimage = px_out


def gen_layer_names(bands):
    """
Given a list of band numbers, returns a list of layer names. These
make it easier to identify which values are which in the output image.
"""
    layer_names = []
    for band in bands:
        layer_names.append('{}_slope'.format(band))
        layer_names.append('{}_intercept'.format(band))
        layer_names.append('{}_cos1'.format(band))
        layer_names.append('{}_sin1'.format(band))
        layer_names.append('{}_cos2'.format(band))
        layer_names.append('{}_sin2'.format(band))
        layer_names.append('{}_cos3'.format(band))
        layer_names.append('{}_sin3'.format(band))
        layer_names.append('{}_RMSE'.format(band))
        layer_names.append('{}_overall'.format(band))
        layer_names.append('{}_start'.format(band))
    return (layer_names)


def get_ST_model_coeffs(json_fp, output_fp, gdalformat='KEA', bands=None, num_processes=1, model_type='Lasso',
                        alpha=20, cv=False):
    """
Main function to run to generate the output image. Given an input JSON file
and an output file path, generates a multi-band output image where each pixel
contains the model details for that pixel. Opening/closing of files, generation
of blocks and use of multiprocessing is all handled by RIOS.

:param json_fp:       Path to JSON file of date/filepath pairs.
:param output_fp:     Path for output file.
:param gdalformat:    Short driver name for GDAL, e.g. KEA, GTiff.
:param bands:         List of GDAL band numbers to use in the analysis, e.g. [2, 5, 7].
:param num_processes: Number of concurrent processes to use.
:param model_type:    Either 'Lasso' or 'OLS'. The type of model fitting to use. OLS will
                      be faster, but more likely to overfit. Both types will adjust the number of model
                      coefficients depending on the number of observations.
:param alpha:         If using Lasso fitting, the alpha value controls the degree of
                      penalization of the coefficients. The lower the value, the closer
                      the model will fit the data. For surface reflectance, a value of
                      around 20 (the default) is usually OK.
:param cv:            If using Lasso fitting, you can use cross validation to choose
                      the value of alpha by setting cv=True. However, this is not recommended and will
                      substantially increase run time.
"""
    paths = []
    dates = []

    try:
        # Open and read JSON file containing date:filepath pairs
        with open(json_fp) as json_file:
            image_list = json.load(json_file)

            for date, img_path in image_list.items():
                dates.append(datetime.strptime(date, '%Y-%m-%d').toordinal())
                paths.append(img_path)
    except FileNotFoundError:
        print('Could not find the provided JSON file.')
        sys.exit()
    except json.decoder.JSONDecodeError as e:
        print('There is an error in the provided JSON file: {}'.format(e))
        sys.exit()

    # Create object to hold input files
    infiles = applier.FilenameAssociations()
    infiles.images = paths

    # Create object to hold output file
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_fp

    # ApplierControls object holds details on how processing should be done
    app = applier.ApplierControls()

    # Set window size to 1 because we are working per-pixel
    app.setWindowXsize(1)
    app.setWindowYsize(1)

    # Set output file type
    app.setOutputDriverName(gdalformat)
    
    # Set progress
    app.progress = cuiprogress.CUIProgressBar()

    # Set that pyramids and statistics are not calculated.
    app.omitPyramids = True
    app.calcStats = False

    # Use Python's multiprocessing module
    app.setJobManagerType('multiprocessing')
    app.setNumThreads(num_processes)

    # Open first image in list to use as a template
    template_image = fileinfo.ImageInfo(infiles.images[0])

    # Get no data value
    nodata_val = template_image.nodataval[0]

    if not bands:  # No bands specified - default to all
        num_bands = template_image.rasterCount
        bands = [i for i in range(1, num_bands + 1)]

    else:  # If a list of bands is provided
        # Number of bands determines things like the size of the output array
        num_bands = len(bands)

        # Need to tell the applier to only use the specified bands
        app.selectInputImageLayers(bands)

    # Create list of actual names
    full_names = [template_image.layerNameFromNumber(i) for i in bands]
    template_image = None

    # Set up output layer names based on band numbers
    layer_names = gen_layer_names(full_names)
    app.setLayerNames(layer_names)

    # Additional arguments - have to be passed as a single object
    other_args = applier.OtherInputs()
    other_args.dates = dates
    other_args.num_bands = num_bands
    other_args.nodata_val = nodata_val
    other_args.model_type = model_type
    other_args.alpha = alpha
    other_args.cv = cv

    try:
        applier.apply(gen_per_band_models, infiles, outfiles, otherArgs=other_args, controls=app)
    except RuntimeError as e:
        print('There was an error processing the images: {}'.format(e))
        print('Do all images in the JSON file exist?')


def gen_prediction(info, infile, outfile, other_args):
    """
Run per-block by RIOS. Given a block from the input image of coefficient
values, returns a predicted image with n bands which is the size of the block.
"""
    T = 365.25
    pi_val_simple = (2 * numpy.pi) / T
    pi_val_advanced = (4 * numpy.pi) / T
    pi_val_full = (6 * numpy.pi) / T

    date = other_args.date_to_predict

    num_input_bands = infile.coeff_img.shape[0]

    # Get number of bands
    num_output_bands = num_input_bands // 11

    # Set up array with the correct output shape
    px_out = numpy.zeros((num_output_bands, info.getBlockSize()[1], info.getBlockSize()[0]), dtype='float64')

    # Each band is predicted separately
    for i in range(0, num_input_bands, 11):
        # Generate predicted values for this block, for this band
        prediction = (infile.coeff_img[i] * (date - infile.coeff_img[i + 10])) + infile.coeff_img[i + 1] + (
                    infile.coeff_img[i + 2] * numpy.cos(pi_val_simple * (date - infile.coeff_img[i + 10]))) + (
                                 infile.coeff_img[i + 3] * numpy.sin(
                             pi_val_simple * (date - infile.coeff_img[i + 10]))) + (infile.coeff_img[i + 4] * numpy.cos(
            pi_val_advanced * (date - infile.coeff_img[i + 10]))) + (infile.coeff_img[i + 5] * numpy.sin(
            pi_val_advanced * (date - infile.coeff_img[i + 10]))) + (infile.coeff_img[i + 6] * numpy.cos(
            pi_val_full * (date - infile.coeff_img[i + 10]))) + (
                                 infile.coeff_img[i + 7] * numpy.sin(pi_val_full * (date - infile.coeff_img[i + 10])))

        output_band = i // 11
        px_out[output_band] = prediction

    outfile.output_img = px_out


def predict_for_date(date, input_path, output_path, gdalformat='KEA', num_processes=1):
    """
Main function to generate the predicted image. Given an input image containing
per-band model coefficients, outputs a multi-band predicted image over the same area.
Opening/closing of files, generation of blocks and use of multiprocessing is
all handled by RIOS.

:param date:          The date to predict in YYYY-MM-DD format.
:param input_path:    Path to the input image generated by get_model_coeffs.py.
:param output_path:   Path for the output image.
:param gdalformat:    Short driver name for GDAL, e.g. KEA, GTiff.
:param num_processes: Number of concurrent processes to use.
"""

    # Create object to hold input files
    infile = applier.FilenameAssociations()
    infile.coeff_img = input_path

    # Create object to hold output file
    outfile = applier.FilenameAssociations()
    outfile.output_img = output_path

    # ApplierControls object holds details on how processing should be done
    app = applier.ApplierControls()

    # Set output file type
    app.setOutputDriverName(gdalformat)
    
    # Set progress
    app.progress = cuiprogress.CUIProgressBar()

    # Set that pyramids and statistics are not calculated.
    app.omitPyramids = True
    app.calcStats = False

    # Use Python's multiprocessing module
    app.setJobManagerType('multiprocessing')
    app.setNumThreads(num_processes)

    # Convert provided date to ordinal
    ordinal_date = datetime.strptime(date, '%Y-%m-%d').toordinal()

    # Additional arguments - have to be passed as a single object
    other_args = applier.OtherInputs()
    other_args.date_to_predict = ordinal_date

    # Get band names
    try:
        input_img = fileinfo.ImageInfo(infile.coeff_img)
    except:
        sys.exit('Could not find input image.')

    layer_names = numpy.unique([name.split('_')[0] for name in input_img.lnames])
    app.setLayerNames(layer_names)

    applier.apply(gen_prediction, infile, outfile, otherArgs=other_args, controls=app)
