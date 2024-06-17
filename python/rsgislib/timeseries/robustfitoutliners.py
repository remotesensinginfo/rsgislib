#!/usr/bin/env python

import json
import sys
from datetime import datetime

import numpy
import statsmodels.api
from rios import applier, fileinfo

import rsgislib
import rsgislib.imageutils

TQDM_AVAIL = True
try:
    import tqdm
except ImportError:
    import rios.cuiprogress

    TQDM_AVAIL = False


class RobustModel(object):
    def __init__(self, datetimes, num_years):
        self.T = 365.25
        self.pi_val = (2 * numpy.pi) / self.T
        self.pi_val_change = (2 * numpy.pi) / (num_years * self.T)
        self.datetimes = datetimes
        self.robust_model = None
        self.RMSE = None
        self.start_date = numpy.min(self.datetimes)
        self.residuals = None

    def fitModel(self, band_data):
        x = self.prepareXData(self.datetimes)

        self.robust_model = statsmodels.api.RLM(
            band_data, x, M=statsmodels.api.robust.norms.TukeyBiweight(c=0.4685)
        ).fit()

        predicted = self.robust_model.predict(x)
        self.residuals = band_data - predicted

        # Get overall RMSE of model
        self.RMSE = numpy.sqrt(numpy.mean(self.residuals**2))

    def prepareXData(self, datetimes):
        rescaled = datetimes - numpy.min(self.datetimes)
        x = numpy.array(
            [
                numpy.ones_like(datetimes),  # Add constant
                numpy.cos(self.pi_val * rescaled),
                numpy.sin(self.pi_val * rescaled),
                numpy.cos(self.pi_val_change * rescaled),
                numpy.sin(self.pi_val_change * rescaled),
            ]
        ).T
        return x


def _gen_band_masks(info, inputs, outputs, other_args):
    """
    Run per-block by RIOS. In this case each block is a
    single pixel. Given a block of values for each band for each date, returns
    a numpy array containing a mask where screened out data = 1 and data
    to keep = 0.
    """
    dates = other_args.dates
    nodata = other_args.nodata
    num_bands = other_args.num_bands
    thresh = other_args.threshold

    # Set up default output
    # Assumes all pixels are clear
    # Remember this is output for a single pixel through time
    results = numpy.zeros((len(inputs.images), num_bands, 1, 1), dtype="int16")
    all_band_data = numpy.array(dates)

    # Get data for one band at a time
    for b in range(0, num_bands):
        band_data = numpy.array(
            [[inputs.images[t][b][0][0]] for t in range(0, len(inputs.images))]
        )
        all_band_data = numpy.hstack((all_band_data, band_data))

    drop_indices = numpy.where(numpy.any(all_band_data == nodata, axis=1))
    drop_indices = numpy.array(drop_indices).reshape(-1)

    # Remove any rows where all band values are 0
    all_band_data = all_band_data[numpy.all(all_band_data != nodata, axis=1)]
    # print(all_band_data.shape)
    # Need a minimum of 12 observations
    if len(all_band_data) >= 12:
        num_years = numpy.ceil((numpy.max(dates) - numpy.min(dates)) / 365)
        # Output array needs to be matched in size to input array
        output_arr = numpy.delete(results, drop_indices, axis=0)
        try:
            for i in range(1, num_bands + 1):
                # Create model object
                rm = RobustModel(all_band_data[:, 0], num_years)
                # Get data for this band
                bd = all_band_data[:, i]
                # Fit the robust model
                rm.fitModel(bd)
                # Find low outliers
                too_low = numpy.where(rm.residuals < -rm.RMSE * thresh)
                output_arr[too_low, i - 1] = -1
                # Find high outliers
                too_high = numpy.where(rm.residuals > rm.RMSE * thresh)
                output_arr[too_high, i - 1] = 1
            results = numpy.insert(
                output_arr,
                drop_indices,
                numpy.zeros((len(drop_indices), num_bands, 1, 1)),
                axis=0,
            )
        except numpy.linalg.LinAlgError as e:
            print("Warning: {}".format(e))
            pass

    outputs.outimage = results


def get_ST_masks(
    json_fp, bands=None, roi_img=None, gdal_format="KEA", num_processes=1, threshold=3
):
    """Main function to run to generate the output masks. Given an input JSON file,
    generates a mask for each date, for each band where 0=Inlier, 1=High outlier,
    -1=Low outlier. Opening/closing of files, generation of blocks and use of
    multiprocessing is all handled by RIOS.

    A minimum of 12 observations is required to create the masks.


    json_fp:       Path to JSON file which provides a dictionary where for each
                   date, an input file name and an output file name are provided.
    gdal_format: Short driver name for GDAL, e.g. KEA, GTiff.
    num_processes: Number of concurrent processes to use.
    bands:         List of GDAL band numbers to use, e.g. [1, 3, 5]. Defaults to all.
    threshold:     Threshold for screening. Defaults to 3, meaning that observations
                   outside 3*RMSE of the fitted model will be counted as outliers.
                   Lower values will result in more outliers being detected.
    """
    ip_paths = []
    op_paths = []
    dates = []

    try:
        # Open and read JSON file containing date:filepath pairs
        with open(json_fp) as json_file:
            image_list = json.load(json_file)

            for date in image_list.items():
                dates.append([datetime.strptime(date[0], "%Y-%m-%d").toordinal()])
                ip_paths.append(date[1]["input"])
                op_paths.append(date[1]["output"])
    except FileNotFoundError:
        print("Could not find the provided JSON file.")
        sys.exit()
    except json.decoder.JSONDecodeError as e:
        print("There is an error in the provided JSON file: {}".format(e))
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

    if TQDM_AVAIL:
        app.progress = rsgislib.TQDMProgressBar()
    else:
        app.progress = rios.cuiprogress.GDALProgressBar()

    # Set output file type
    app.setOutputDriverName(gdal_format)

    if roi_img is not None:
        app.setReferenceImage(roi_img)
        app.setFootprintType(applier.BOUNDS_FROM_REFERENCE)
        app.setResampleMethod("near")

    # Use Python's multiprocessing module
    app.setJobManagerType("multiprocessing")
    app.setNumThreads(num_processes)

    # Open first image in list to use as a template
    template_image = fileinfo.ImageInfo(infiles.images[0])

    # Get no data value
    nodata = template_image.nodataval[0]

    if not bands:  # No bands specified - default to all
        num_bands = template_image.rasterCount
        bands = [i for i in range(1, num_bands + 1)]
    else:  # If a list of bands is provided
        # Number of bands determines things like the size of the output array
        num_bands = len(bands)
        # Need to tell the applier to only use the specified bands
        app.selectInputImageLayers(bands)

    full_names = [template_image.layerNameFromNumber(i) for i in bands]
    # Set up output layer name
    app.setLayerNames(full_names)

    # Additional arguments - have to be passed as a single object
    other_args = applier.OtherInputs()
    other_args.dates = dates
    other_args.threshold = threshold
    other_args.nodata = nodata
    other_args.num_bands = num_bands
    template_image = None

    try:
        applier.apply(
            _gen_band_masks, infiles, outfiles, otherArgs=other_args, controls=app
        )
    except RuntimeError as e:
        print("There was an error processing the images: {}".format(e))
        print("Do all images in the JSON file exist?")


def create_datejson_file(image_list, out_msk_dir, out_json_file, gdal_format="KEA"):
    """
    Assume the date is the second component of the file name (XXX_YYYYMMDD_XXXX.xxx)

    """
    import datetime
    import os.path

    import rsgislib.tools.filetools

    out_img_ext = rsgislib.imageutils.get_file_img_extension(gdal_format)
    date_imgs = dict()
    for img in image_list:
        basename = rsgislib.tools.filetools.get_file_basename(img)
        basename_comps = basename.split("_")
        print(basename_comps)
        if len(basename_comps) < 2:
            raise rsgislib.RSGISPyException(
                "The filename must have at least two components split by '_'."
            )
        date_str = basename_comps[1]
        if len(date_str) != 8:
            raise rsgislib.RSGISPyException(
                "The second component must have 8 characters YYYYMMDD. String provided: '{}'".format(
                    date_str
                )
            )

        date_obj = datetime.datetime.strptime(date_str, "%Y%m%d").strftime("%Y-%m-%d")
        date_imgs[date_obj] = dict()
        date_imgs[date_obj]["input"] = img
        date_imgs[date_obj]["output"] = os.path.join(
            out_msk_dir, "{}_chngmsk{}".format(basename, out_img_ext)
        )

    with open(out_json_file, "w") as out_json_file_obj:
        json.dump(date_imgs, out_json_file_obj, sort_keys=True, indent=4)
