#! /usr/bin/env python
############################################################################
#  tmask.py
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
# Purpose:  Provides functions for applying the tmask cloud masking algorithm.
#           See https://github.com/klh5/rios_tmask
#
# Zhu, Z. and Woodcock, C.E. Automated cloud, cloud shadow, and snow detection
# in multitemporal Landsat data: An algorithm designed specifically for monitoring
# land cover change. Remote Sensing of Environment. 2014, 152, 217–234.
# doi:10.1016/j.rse.2014.06.012.
#
# Author: Katie Awty-Carroll (ed by Pete Bunting)
# Email: petebunting@mac.com
# Date: 24/2/2020
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

import json
import sys
from datetime import datetime

import numpy
import statsmodels.api

from rios import fileinfo
from rios import applier


class RLMRemoveOutliers(object):

    def __init__(self, toa_data, results):

        self.T = 365.25
        self.pi_val = (2 * numpy.pi) / self.T
        self.toa_data = toa_data
        self.results = results

        self.pi_val_change = None
        self.green_model = None
        self.nir_model = None
        self.swir_model = None
        self.b2_delta = None
        self.b4_delta = None
        self.b5_delta = None

    def cleanData(self, num_years, threshold):
        """
        Uses the green, NIR, and SWIR band data to build RLMs for detecting cloud and snow outliers

        :param num_years:
        :param threshold:
        :return:
        """
        self.pi_val_change = (2 * numpy.pi) / (num_years * self.T)
        start_date = self.toa_data[:, 0].min()
        rescaled = self.toa_data[:, 0] - start_date

        try:
            # Get coefficients for the three models
            self.green_model, self.b2_delta = self.makeRLMModel(rescaled, self.toa_data[:, 1])
            self.nir_model, self.b4_delta = self.makeRLMModel(rescaled, self.toa_data[:, 2])
            self.swir_model, self.b5_delta = self.makeRLMModel(rescaled, self.toa_data[:, 3])

            self.toa_data = numpy.hstack((self.toa_data, self.b2_delta.reshape(-1, 1)))
            self.toa_data = numpy.hstack((self.toa_data, self.b4_delta.reshape(-1, 1)))
            self.toa_data = numpy.hstack((self.toa_data, self.b5_delta.reshape(-1, 1)))

            self.dropOutliers(threshold)
        except numpy.linalg.LinAlgError:
            pass
        return self.results

    def makeRLMModel(self, datetimes, band_data):
        """
        Builds the model and stores the coefficients

        :param datetimes:
        :param band_data:
        :return:

        """
        x = numpy.array([numpy.ones_like(datetimes),  # Add constant
                         numpy.cos(self.pi_val * datetimes),
                         numpy.sin(self.pi_val * datetimes),
                         numpy.cos(self.pi_val_change * datetimes),
                         numpy.sin(self.pi_val_change * datetimes)]).T

        rlm_model = statsmodels.api.RLM(band_data, x, M=statsmodels.api.robust.norms.TukeyBiweight(c=0.4685))
        rlm_result = rlm_model.fit(maxiter=5)
        delta = band_data - rlm_result.predict(x)
        return rlm_result, delta

    def dropOutliers(self, threshold):
        """

        :param threshold:

        """
        self.results = numpy.where(((self.toa_data[:, 4] < threshold) & (
                    (self.toa_data[:, 5] > -threshold) | (self.toa_data[:, 6] > -threshold))), self.results,
                                   self.results + 1)


def _gen_tmask(info, inputs, outputs, other_args):
    """
    Run per-block by RIOS. In this case each block is a
    single pixel. Given a block of values for each band for each date, returns
    a numpy array containing a mask where screened out data = 1 and data
    to keep = 0.

    """
    dates = other_args.dates
    nodata = other_args.nodata

    # Set up default output
    # Assumes all pixels are clear
    results = numpy.zeros(len(inputs.images), dtype='uint8')

    green_vals = numpy.array([[inputs.images[t][0][0][0]] for t in range(0, len(inputs.images))])
    nir_vals = numpy.array([[inputs.images[t][1][0][0]] for t in range(0, len(inputs.images))])
    swir1_vals = numpy.array([[inputs.images[t][2][0][0]] for t in range(0, len(inputs.images))])

    model_inputs = numpy.hstack((other_args.dates, green_vals, nir_vals, swir1_vals))
    drop_indices = numpy.where(numpy.any(model_inputs == nodata, axis=1))
    drop_indices = numpy.array(drop_indices).reshape(-1)

    # Remove any rows where all band values are 0
    model_inputs = model_inputs[numpy.all(model_inputs != nodata, axis=1)]

    # Need a minimum of 12 observations
    if (len(model_inputs) >= 12):
        # Output array needs to be matched in size to input array
        output_arr = numpy.delete(results, drop_indices)
        tmask = RLMRemoveOutliers(model_inputs, output_arr)
        num_years = numpy.ceil((numpy.max(dates) - numpy.min(dates)) / 365)
        output_arr = tmask.cleanData(num_years, other_args.threshold)
        results = numpy.insert(output_arr, numpy.array(drop_indices), numpy.zeros(len(drop_indices)))

    results = results.reshape(len(results), 1, 1, 1)
    outputs.outimage = results


def run_tmask(json_fp, gdal_format='KEA', num_processes=1, green_band=2, nir_band=4, swir_band=5, threshold=40):
    """
    Main function to run to generate the output masks. Given an input JSON file,
    generates a mask for each date where 1=cloud/cloud shadow/snow and 0=clear.
    Opening/closing of files, generation of blocks and use of multiprocessing is
    all handled by RIOS.

    A minimum of 12 observations is required to create the masks.

    :param json_fp: Path to JSON file which provides a dictionary where for each
                    date, an input file name and an output file name are provided.
    :param gdal_format: The file format of the output image (e.g., KEA, GTIFF). (Default: KEA)
    :param num_processes: Number of concurrent processes to use. (Default: 1)
    :param green_band: GDAL band number for green spectral band. Defaults to 2.
    :param nir_band: GDAL band number for NIR spectral band. Defaults to 4.
    :param swir_band: GDAL band number for SWIR spectral band. Defaults to 5.
    :param threshold: Numerical threshold for screening out cloud, cloud shadow, and snow.
                      Defaults to 40. See Zhu & Woodcock (2014) for details.

    """
    ip_paths = []
    op_paths = []
    dates = []

    try:
        # Open and read JSON file containing date:filepath pairs
        with open(json_fp) as json_file:
            image_list = json.load(json_file)

            for date in image_list.items():
                dates.append([datetime.strptime(date[0], '%Y-%m-%d').toordinal()])
                ip_paths.append(date[1]['input'])
                op_paths.append(date[1]['output'])
    except FileNotFoundError:
        print('Could not find the provided JSON file.')
        sys.exit()
    except json.decoder.JSONDecodeError as e:
        print('There is an error in the provided JSON file: {}'.format(e))
        sys.exit()

    # Create object to hold input files
    infiles = applier.FilenameAssociations()
    infiles.images = ip_paths

    # Create object to hold output file
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = op_paths

    # ApplierControls object holds details on how processing should be done
    app = applier.ApplierControls()

    # Set window size to 1 because we are working per-pixel
    app.setWindowXsize(1)
    app.setWindowYsize(1)

    # Set output file type
    app.setOutputDriverName(gdal_format)

    # Use Python's multiprocessing module
    app.setJobManagerType('multiprocessing')
    app.setNumThreads(num_processes)

    # Open first image in list to use as a template
    template_image = fileinfo.ImageInfo(infiles.images[0])

    # Get no data value
    nodata = template_image.nodataval[0]

    # Need to tell the applier to only use the specified bands
    app.selectInputImageLayers([green_band, nir_band, swir_band])

    # Set up output layer name
    app.setLayerNames(['tmask'])

    # Additional arguments - have to be passed as a single object
    other_args = applier.OtherInputs()
    other_args.dates = dates
    other_args.threshold = threshold
    other_args.nodata = nodata

    try:
        applier.apply(_gen_tmask, infiles, outfiles, otherArgs=other_args, controls=app)
    except RuntimeError as e:
        print('There was an error processing the images: {}'.format(e))
        print('Do all images in the JSON file exist?')

