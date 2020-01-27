#! /usr/bin/env python
############################################################################
#  sdm.py
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
# Purpose:  A collection of Python3 functions for species distribution modelling.
#
# Author: Osian Roberts
# Email: odr1@aber.ac.uk
# Date: 21/01/2020
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

import os
import shutil
import warnings
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rcParams
from sys import stdout
from sklearn.metrics import roc_curve
from sklearn.calibration import calibration_curve
from osgeo import gdal

gdal.SetConfigOption('COMPRESS_OVERVIEW', 'DEFLATE')

rcParams.update({'font.family': 'cmr10'})
rcParams.update({'font.size': 9})
rcParams['axes.linewidth'] = 0.5
rcParams['axes.unicode_minus'] = False
rcParams['xtick.major.pad']='2'
rcParams['ytick.major.pad']='2'


def ProgressBar(n_tasks, i):
	"""
	A function to display a progress bar on the unix terminal.

    :param n_tasks: integer representing the total number of tasks to perform.
    :param i: integer representing the index of the current task.
    
	"""
	barLength, status = 50, ''
	i = float(i) / float(n_tasks)
	if i >= 1.0:
		i = 1
		status = 'Done. \n'
	block = round(barLength * i)
	text = '\r{} {:.0f}% {}'.format('#' * block + '-' * (barLength - block), round(i * 100, 0), status)
	stdout.write(text)
	stdout.flush()


def dtype2obj(dtype):
    """
    An utility function to convert strings into datatype specifiers for structured numpy arrays (i.e. np.recarray).

    :param dtype: a string defining the datatype (e.g. "float32").
    :return: a datatype specifier (e.g. "f4").
    
    """
    if not isinstance(dtype, str):
        raise SystemExit('ERROR: datatype indicator must be a string.')

    if dtype == 'float64':
        dtype = 'f8'
    elif dtype == 'float32':
        dtype = 'f4'
    elif dtype == 'float16':
        dtype = 'f2'
    elif dtype == 'uint64':
        dtype = 'u8'
    elif dtype == 'uint32':
        dtype = 'u4'
    elif dtype == 'uint16':
        dtype = 'u2'
    elif dtype == 'int64':
        dtype = 'i8'
    elif dtype == 'int32':
        dtype = 'i4'
    elif dtype == 'int16':
        dtype = 'i2'
    elif dtype == 'uint8':
        dtype = 'i1'
    elif dtype == 'int8':
        dtype = 'b1'
    else:
        raise SystemExit('ERROR: presence absence data contains unsupported datatypes (e.g. strings).')
    return dtype


def create_simulation_directory(OutDir, Overwrite=True):
    """
    A function to create the simulation directory.
    
    :param OutDir: Path to output directory.
    :param Overwrite: Boolean option to overwrite the output directory if it already exists. Default=True.
    
    """
    if not isinstance(OutDir, str):
        try:
            OutDir = str(OutDir)
        except Exception:
            raise SystemExit('ERROR: unable to create the simulation directory. The output path should be a string.')

    if os.path.exists(OutDir) and Overwrite == False:
        print('ERROR: the output directory already exists.')
        raise SystemExit('Please select another simulation name or delete the existing directory.')
    elif os.path.exists(OutDir) and Overwrite == True:
        shutil.rmtree(OutDir)
    try:
        os.makedirs(OutDir)
    except Exception:
        raise SystemExit('ERROR: unable to create the simulation directory. Please check the folder path.')


def read_raster_information(InputImage):
    """
    A function to read a GDAL image to determine:
      * geot = Image geotransform.
      * srs = Coordinate system.
      * xsize, ysize = Image dimensions along the x & y axes.
      * noDataVal = NoDataValue for invalid pixels.
    
    :param InputImage: path to the input raster.
    """
    if not isinstance(InputImage, str):
        raise SystemExit('ERROR: the image path must be a string.')
    if not os.path.exists(InputImage):
        raise SystemExit('ERROR: ' + InputImage + ' not found. Please check the file path.')
    try:
        im = gdal.Open(InputImage, 0)
        geot = im.GetGeoTransform()
        srs = im.GetProjection()
        xsize, ysize = im.RasterXSize, im.RasterYSize
        noDataVal = im.GetRasterBand(1).GetNoDataValue()
        del im
    except Exception:
        raise SystemExit('ERROR: unable to open ' + Image)
    return [geot, srs, xsize, ysize, noDataVal]


def find_invalid_images(ImageList, MaskImage, NoDataValue):
    """
    A function to check that all input rasters have the same coordinate system and extents
    as the binary mask delineating the study region.
    
    Required Parameters:
    :param ImageList: A list of input images to check (the environmental predictors).
    :param MaskImage: A binary mask of the study region.
    :param NoDataValue: A common nodata value for the input images.
    :return: a list of invalid images.
    
    """
    print('Checking input images...')
    if not isinstance(MaskImage, str):
        raise SystemExit('ERROR: MaskImage must be a string.')
    if not os.path.exists(MaskImage):
        raise SystemExit('ERROR: the binary mask image could not be found. Please check the file path.')
    if not isinstance(ImageList, list):
        raise SystemExit('ERROR: please provide a list of input images.')
    if not ImageList:
        raise SystemExit('ERROR: an empty list of input images was passed.')

    # get projection information of binary mask:
    target_geot, target_proj, target_xsize, target_ysize, noDataVal = read_raster_information(MaskImage)
    if noDataVal != 0:
        print('WARNING: the binary mask has a no data value != 0. This may lead to unexpected results.')

    # check that the image paths are valid:
    invalid_paths = []
    for image in ImageList:
        if not os.path.exists(image):
            invalid_paths.append(image)

    if invalid_paths:
        print('ERROR: the following image paths are invalid:')
        for path in invalid_paths:
            print(path)
        raise SystemExit()
    else:
        del invalid_paths

    # read raster extents and projection information:
    geotransform, proj, xsize, ysize, null_values = [list() for i in range(5)]
    for image in ImageList:
        geot, srs, x, y, noDataVal = read_raster_information(image)
        geotransform.append(geot)
        proj.append(srs)
        xsize.append(x)
        ysize.append(y)
        null_values.append(noDataVal)
        del geot, srs, x, y, noDataVal

    invalid_images = []
    for idx, image in enumerate(ImageList):
        # check image has the same projection as MaskImage:
        if proj[idx] != target_proj and image not in invalid_images:
            invalid_images.append(image)
        # check that image has the same geotransform as MaskImage:
        if geotransform[idx] != target_geot and image not in invalid_images:
            invalid_images.append(image)
        # check that image has the same dimensions as MaskImage:
        if xsize[idx] != target_xsize and image not in invalid_images:
            invalid_images.append(image)
        if ysize[idx] != target_ysize and image not in invalid_images:
            invalid_images.append(image)
        # check that all images have the same no data value as defined:
        if null_values[idx] != NoDataValue and image not in invalid_images:
            invalid_images.append(image)
    del geotransform, proj, xsize, ysize, null_values
    return invalid_images


def resampleImages2Grid(ImageList, RefImage, NoDataValue=0.0, interpMethod='near', multicore=True):
    """
    A function to resample a images to the projection and/or pixel size of a reference image.
    Warning/Deprecated. This should be updated and moved to rsgislib.imageutils...

    Required Parameters:
    :param ImageList: A list of input images to be resampled. These are single band images for each predictor variable.
    :param RefImage: Reference image to which the input images are resampled.
    :param NoDataValue: No data value to be assigned to invalid pixels.
    :param InterpMethod: Interpolation method used to resample the image [near, bilinear, lanczos, cubicspline, nearestneighbour, cubic, average, mode].
    :param multicore: boolean option to use multiple processing (Default = True).
    
    """
    print('Resampling input images to match ' + RefImage)
    if not isinstance(ImageList, list):
        raise SystemExit('ERROR: please provide a list of input images.')
    if not isinstance(RefImage, str):
        raise SystemExit('ERROR: RefImage must be a string.')
    if not os.path.exists(RefImage):
        raise SystemExit('ERROR: the reference image could not be found. Please check the file path.')
    if not isinstance(NoDataValue, int) or not isinstance(NoDataValue, float):
        print('WARNING: no data value provided is not numeric. Using a default value of zero...')
        NoDataValue = 0.0
    if not isinstance(multicore, bool):
        multicore = True

    if interpMethod == 'near':
        interpolationMethod = gdal.GRA_NearestNeighbour
    elif interpMethod == 'bilinear':
        interpolationMethod = gdal.GRA_Bilinear 
    elif interpMethod == 'lanczos':
        interpolationMethod = gdal.GRA_Lanczos 
    elif interpMethod == 'cubicspline':
        interpolationMethod = gdal.GRA_CubicSpline 
    elif interpMethod == 'nearestneighbour':
        interpolationMethod = gdal.GRA_NearestNeighbour 
    elif interpMethod == 'cubic':
        interpolationMethod = gdal.GRA_Cubic
    elif interpMethod == 'average':
        interpolationMethod = gdal.GRA_Average
    elif interpMethod == 'mode':
        interpolationMethod = gdal.GRA_Mode
    else:
        raise SystemExit('Interpolation method was not recognised.')
    
    geotransform, srs, xsize, ysize, noDataVal = read_raster_information(RefImage)
    del noDataVal

    for idx, image in enumerate(ImageList):
        # create backup copy of the input image:
        ImageBackup = '.'.join(image.split('.')[:-1]) + '_original.' + image.split('.')[-1]
        shutil.copy(image, ImageBackup)

        im = gdal.Open(ImageBackup, 0)
        Driver = im.GetDriver()
        srcNoDataValue = im.GetRasterBand(1).GetNoDataValue()
        
        if Driver.ShortName == 'GTiff':
            OutImage = Driver.Create(image, xsize, ysize, 1, im.GetRasterBand(1).DataType, options=['COMPRESS=DEFLATE', 'TILED=YES'])
        else:
            OutImage = Driver.Create(image, xsize, ysize, 1, im.GetRasterBand(1).DataType)
        
        OutImage.SetProjection(srs)
        OutImage.SetGeoTransform(geotransform)
        OutBand = OutImage.GetRasterBand(1)
        OutBand.SetNoDataValue(NoDataValue)

        if srcNoDataValue:
            warpOpts = gdal.WarpOptions(resampleAlg=interpolationMethod, srcNodata=srcNoDataValue,
                                        dstNodata=NoDataValue, multithread=multicore, callback=gdal.TermProgress)    
        else:
            warpOpts = gdal.WarpOptions(resampleAlg=interpolationMethod, dstNodata=NoDataValue, multithread=multicore, callback=gdal.TermProgress)

        gdal.Warp(OutImage, im, options=warpOpts)
        del im, OutImage, Driver, srcNoDataValue

        # build image overviews:
        im = gdal.Open(image, 1)
        im.BuildOverviews('NEAREST', [4, 8, 16, 32, 64, 128])
        del im
    del geotransform, srs, xsize, ysize


def check_simulation(problem):
    """
    A function to check that the simulation parameters are valid.
    
    :param problem: A python dictionary containing the simulation parameters.
    :return problem: A python dictionary containing corrected simulation parameters.
    
    """
    print('Verifying the simulation parameters...')
    if not isinstance(problem, dict):
        raise SystemExit('ERROR: please provide a Python dictionary containing the simulation parameters.')

    # check that required fields are present in dictionary:
    names = ['Name', 'Output Directory', 'Images', 'Categorical', 'Null Value', 'Valid Mask',
             'Environmental Predictors', 'geotransform', 'projection', 'rasterxsize', 'rasterysize']
    for name in names:
        if name not in problem:
            raise SystemExit('Error the simulation does not contain field: ' + name)

    # check that length of lists are equal:
    if len(problem['Images']) != len(problem['Environmental Predictors']):
        print('ERROR: the number of images and environmental predictors does not match.')
        print(problem['Images'])
        print(problem['Environmental Predictors'])
        raise SystemExit()
    elif len(problem['Images']) != len(problem['Categorical']):
        raise SystemExit('ERROR: the categorical index does not match the number of images.')

    # check image paths:
    for image in problem['Images']:
        if not os.path.exists(image):
            raise SystemExit('ERROR: ' + image + ' not found. Please check the file path.')

    if not os.path.exists(problem['Valid Mask']):
        raise SystemExit('ERROR: binary mask image not found. Please check the file path.')
    
    # check numeric variables:
    if problem['rasterxsize'] is not int:
        try:
            problem['rasterxsize'] = int(problem['rasterxsize'])
        except Exception:
            raise SystemExit('ERROR: rasterxsize is not numeric.')
    if problem['rasterysize'] is not int:
        try:
            problem['rasterxsize'] = int(problem['rasterxsize'])
        except Exception:
            raise SystemExit('ERROR: rasterysize is not numeric.')
    if problem['Null Value'] is not int and problem['Null Value'] is not float:
        problem['Null Value'] = 0
    return problem    


def check_recarray(x):
    """
    Utility function to check if an object is a structured numpy.recarray.

    :param x: A structured numpy array (i.e. np.recarray).
    :return: If the array is structured, returns the names of each column in the structured array.
    
    """
    if not isinstance(x, np.recarray):
        try:
            colnames = np.array(x.dtype.names)
        except Exception:
            raise SystemExit('ERROR: the presence-absence data must be a structured numpy array.')
    else:
        colnames = np.array(x.dtype.names)
    return colnames
        

def get_bounding_box(problem):
    """
    A function to determine the bounding box of the study region.
    Used by the function rsgislib.sdm.generate_pseudoabsences
    
    :param problem: A python dictionary containing the simulation parameters.
    :return: xmin, xmax, ymin, ymax. Coordinates of the bounding box.
    
    """
    if not isinstance(problem, dict):
        raise SystemExit('ERROR: please provide a Python dictionary containing the simulation parameters.')

    try:
        xsize, ysize = problem['rasterxsize'], problem['rasterysize']
        xmin, xcellsize, xrotation, ymax, yrotation, ycellsize = problem['geotransform']
        xmax = xmin + (xcellsize * xsize)
        ymin = ymax + (ycellsize * xsize)
        del xsize, ysize, xcellsize, xrotation, yrotation, ycellsize
    except Exception:
        raise SystemExit('ERROR: unable to calculate the bounding box of the study region.')
    return [xmin, xmax, ymin, ymax]


def generate_pseudoabsences(problem, n_points=10000):
    """
    A function to generate random pseudoabsence records.
    
    Required Parameters:
    :param problem: A python dictionary containing the simulation parameters.
    :param n_points: number of pseudo-absence records to generate. Default is 10000 (same as Maxent).
    :return: A np.ndarray of tupled x & y coordinates, shape=(n_samples, 2).
    
    """
    if not isinstance(problem, dict):
        raise SystemExit('ERROR: please provide a Python dictionary containing the simulation parameters.')

    print('Generating pseudo-absence background points...')
    # calculate the bounding box of the study area:
    xmin, xmax, ymin, ymax = get_bounding_box(problem)

    # check that the input coordinates are correct:
    if xmin >= xmax:
        raise SystemExit('ERROR: the longitude / eastings coordinates are invalid.')
    if ymin >= ymax:
        raise SystemExit('ERROR: the latitude / northing coordinates are invalid.')

    # generate coordinates randomly across study region:
    x = np.random.uniform(xmin, xmax, n_points)
    y = np.random.uniform(ymin, ymax, n_points)
    del xmin, xmax, ymin, ymax

    xy = np.c_[x, y]
    del x, y

    # remove pseudo-absence records in invalid regions using binary mask:
    xy = drop_invalid_records(xy, problem['Valid Mask'])

    # round xy coordinates to 7 decimal places:
    xy = xy.round(decimals=7)
    return xy
    

def drop_invalid_records(xy, BinaryMask, NullValue=False):
    """
    A function to remove presence or absence records located in invalid regions as defined by a binary mask image.
    
    :param xy: A np.ndarray of tupled x & y coordinates, shape=(n_samples, 2).
    :param BinaryMask: Path to a GDAL binary mask image delineating the valid study region.
    :param NullValue: Option to define a specific no data value for invalid pixels (e.g. -9999 or zero).
                      Otherwise the default NoDataValue is used from the raster.
    
    """
    print('Removing absence records located in invalid regions...')
    # check that xy has the correct dimensions:
    if xy.ndim != 2:
        raise SystemExit('ERROR: The xy coordinates must be two-dimensional.')
    if xy.shape[1] != 2:
        try:
            xy = xy.reshape(-1, 2)
        except Exception:
            raise SystemExit('ERROR: The xy coordinates must be two-dimensional.')

    # read mask image as np.ndarray:
    try:
        im = gdal.Open(BinaryMask, 0)
        geot = im.GetGeoTransform()
        mask = im.GetRasterBand(1).ReadAsArray()
        if not NullValue:
            NullValue = im.GetRasterBand(1).GetNoDataValue()
        del im
    except Exception:
        raise SystemExit('ERROR: unable to open ' + BinaryMask)

    # determine if mask != no data value:
    valid_records = []
    for x, y in xy:
        try:
            xidx = int((x - geot[0]) / geot[1])
            yidx = int((geot[3] - y) / -geot[5])
            if mask[yidx][xidx] != NullValue:
                valid_records.append([x, y])
            del xidx, yidx
        except Exception:
            continue
    del mask, geot

    if not valid_records:
        print('ERROR: No valid coordinates were returned.')
        print('Please check the geographic projection of the binary mask.')
        return xy
    else:
        valid_records = np.array(valid_records)
        return valid_records


def export_pseudoabsence_xy(absence_records, OutputCSV):
    """
    A function to export pseudo-absence x & y coordinates to a comma separated text file.

    :param absence_records: np.ndarray containing x & y coordinates of pseudo-absence records, shape=(n_samples, 2).
    :param OutputCSV: Path to the output CSV.
    
    """
    if not isinstance(OutputCSV, str):
        try:
            OutputCSV = str(OutputCSV)
        except Exception:
            raise SystemExit('ERROR: unable to export pseudo-absence coordinates. The output CSV must be a string.')
    if not isinstance(absence_records, np.ndarray):
        try:
            absence_records = np.array(absence_records)
        except Exception:
            raise SystemExit('ERROR: unable to export pseudo-absence coordinates. Absence records must be a two-dimensional np.array.')
    # check that absence_records has the correct dimensions:
    if absence_records.ndim != 2:
        raise SystemExit('ERROR: unable to export pseudo-absence coordinates. Absence records must be a two-dimensional np.array.')
    if absence_records.shape[1] != 2:
        try:
            absence_records = absence_records.reshape(-1, 2)
        except Exception:
            raise SystemExit('ERROR: unable to export pseudo-absence coordinates. Absence records must be a two-dimensional np.array.')

    # convert coordinates into strings:
    absence_records = absence_records.astype(str)

    # write coordinates to OutputCSV:
    try:
        f = open(OutputCSV, 'w')
    except Exception:
        raise SystemExit('ERROR: unable to create output CSV. Please verify the file path.')

    f.write('x,y\n')
    for xy in absence_records:
        f.write(','.join(xy) + '\n')
    del absence_records
    f.close()


def drop_proximal_records(presence_records, absence_records, distance, k=6):
    """
    A function to remove absence records that are close to presence records.
    This function works best for geographic coordinates (i.e. not projected).
    

    :param presence_records: np.ndarray containing x & y coordinates of presence records, shape=(n_samples, 2).
    :param absence_records: np.ndarray containing x & y coordinates of absence records, shape=(n_samples, 2).
    :param distance: A distance threshold to remove proximal absence records.
    :param k: number of nearest neighbours to find. Default = 6.
    :return: Returns x & y coordinates of valid absence records.
    
    """
    print('Removing absence records proximal to presence records...')
    if not distance:
        raise SystemExit('ERROR: Unable to drop records. Please specify a distance threshold to remove proximal absence records.')
    # check that inputs have the correct dimensions:
    if presence_records.ndim != 2:
        raise SystemExit('ERROR: Unable to drop records. The presence records must be two-dimensional x & y coordinates.')
    if absence_records.ndim != 2:
        raise SystemExit('ERROR: Unable to drop records. The absence records must be two-dimensional x & y coordinates.')
    if presence_records.shape[1] != 2:
        try:
            presence_records = presence_records.reshape(-1, 2)
        except Exception:
            raise SystemExit('ERROR: Unable to drop records. The presence records must be two-dimensional x & y coordinates.')
    if absence_records.shape[1] != 2:
        try:
            absence_records = absence_records.reshape(-1, 2)
        except Exception:
            raise SystemExit('ERROR: Unable to drop records. The absence records must be two-dimensional x & y coordinates.')

    # get nearest neighbours:
    from sklearn.neighbors import NearestNeighbors
    KNN = NearestNeighbors(n_neighbors=k, n_jobs=1)
    KNN.fit(presence_records)
    distances, index = KNN.kneighbors(absence_records, return_distance=True)
    del KNN

    # remove bad records:
    valid_absence_records = np.copy(absence_records)
    for idx, dist in enumerate(distances):
        bad = np.where(dist < distance)[0]
        if bad.size != 0:
            valid_absence_records = np.delete(valid_absence_records, index[idx][bad], axis=0)
        del bad
    del distances, index
    return valid_absence_records


def equalise_records(presence_records, absence_records):
    """
    A function to ensure that the number of absence records == presence records.
    Absence records are removed if the number of absence records > presence records.

    :param presence_data: a np.ndarray or np.recarray of presence data, shape(n_samples, n_features).
    :param absence_data: a np.ndarray or np.recarray of absence data, shape(n_samples, n_features).
    :return: a np.array of absence records where n_absences == n_presences. If the number of absence records < presence records, the original array is returned without modification.
    
    """
    if not isinstance(presence_records, np.ndarray) and not isinstance(presence_records, np.recarray):
        try:
            presence_records = np.array(presence_records)
        except Exception:
            raise SystemExit('ERROR: Unable to equalise records. Presence records must be a numpy array.')
    if not isinstance(absence_records, np.ndarray) and not isinstance(absence_records, np.recarray):
        try:
            absence_records = np.array(absence_records)
        except Exception:
            raise SystemExit('ERROR: Unable to equalise records. Absence records must be a numpy array.')

    n_presence = presence_records.shape[0]
    n_absence = absence_records.shape[0]

    if n_absence <= n_presence:
        print('WARNING: Unable to equalise records. Number of absence records <= presence records.')
        print('Please increase the number of absence records.')
        return absence_records
    else:
        idx = np.arange(1, n_presence + 1, 1, dtype='uint32')
        idx = np.random.choice(idx, size=n_presence, replace=False, p=None)
        absence_records = absence_records[idx]
        del idx
        return absence_records


def drop_categoricals(presence_absence_data, categorical_idx):
    """
    A function to remove categorical variables from presence-absence data.

    :param presence_absence_data: A np.recarray containing presence-absence data, shape=(n_samples, n_predictors).
    :param categorical_idx: A list or np.ndarray containing boolean indicators where True == categorical variable.
                            Must be same length as the number of predictors in presence_absence_data.
    
    """
    if not isinstance(categorical_idx, np.ndarray):
        categorical_idx = np.array(categorical_idx)

    colnames = check_recarray(presence_absence_data)
  
    if categorical_idx.size != colnames.size:
        print('ERROR: unable to drop categorical variables.')
        raise SystemExit('The index of categorical variables does not match the number of predictor variables.')

    # find categorical variables:
    binary_idx = np.where(categorical_idx == True)[0]
    if binary_idx.size == 0:
        del binary_idx, categorical_idx
        print('WARNING: unable to drop categorical variables. No categorical variables were specified in index.')
        return presence_absence_data
    else:
        from numpy.lib.recfunctions import rec_drop_fields
        df = rec_drop_fields(presence_absence_data, np.array(presence_absence_data.dtype.names)[binary_idx])
        del binary_idx, categorical_idx
        return df


def encode_categoricals(presence_absence_data, categorical_idx, labels='Presence-Absence'):
    """
    A function to encode categorical variables as a one-hot numeric array.
    Required for estimators that do not natively support discrete variables. 

    :param presence_absence_data: a np.recarray containing presence-absence data, shape=(n_samples, n_predictors).
    :param categorical_idx: A list or np.ndarray containing boolean indicators where True == categorical variable.
                            Must be same length as the number of predictors in presence_absence_data.
    :param labels: Column in np.recarray containing the presence/absence labels.
    
    """
    print('Encoding the categorical variables...')
    from sklearn.preprocessing import OneHotEncoder
    from numpy.lib.recfunctions import rec_append_fields

    # check inputs:
    if not isinstance(categorical_idx, np.ndarray):
        categorical_idx = np.array(categorical_idx)

    colnames = check_recarray(presence_absence_data)

    if labels not in colnames:
        raise SystemExit('ERROR: unable to encode the categorical variables. The presence-absence data does not contain the column ' + labels)
    else:
        label_idx = colnames.index(labels)
        categorical_idx = np.insert(categorical_idx, label_idx, False)
        del label_idx

    if colnames.size != categorical_idx.size:
        raise SystemExit('ERROR: unable to encode the categorical variables. The categorical index does not match the number of environmental predictors.')

    # find categorical variables:
    binary_idx = np.where(categorical_idx == True)[0]
    if binary_idx.size == 0:
        del binary_idx, categorical_idx
        print('WARNING: unable to encode the categorical variables. Mo categorical variables were specified in index.')
        return presence_absence_data
    else:
        # one-hot encode the categorical variable:
        encoder = OneHotEncoder(handle_unknown='ignore', sparse=False)
        df = np.copy(presence_absence_data)
        for idx in binary_idx:
            # read the categorical variable as a 1D array:
            colname = colnames[idx]
            x = df[colname].reshape(-1, 1)
            # define the new column names:
            names = [colname + '_' + i for i in np.unique(x).astype('str')]
            # perform transformation:
            x = encoder.fit_transform(x).astype('uint8')
            # append the categorical variables into df:
            x = x.reshape(len(names), -1)
            df = rec_append_fields(presence_absence_data, names, x)
            del colname, names, x
        del encoder, binary_idx
        # return the new recarray with encoded predictors:
        return df


def read_sightings_csv(InputCSV, sep=',', xfield='x', yfield='y'):
    """
    A function to read occurrence records from a comma separated text file.
    Returns a numpy.array containing the x & y coordinates, shape=(n_samples, 2).
    
    :param InputCSV: Path to the input CSV file.
    :param sep: Column separator / delimiter.
    :param xfield: String defining the column name for the x coordinates (aka longitude, eastings).
    :param yfield: String defining the column name for the y coordinates (aka latitude, northings).
    
    """
    print('Reading occurrence records from CSV...')
    if not os.path.exists(InputCSV):
        raise SystemExit('ERROR: unable to read occurrence records. Input file not found. Please check the file path.')

    try:
        f = open(InputCSV, 'r')
    except Exception:
        raise SystemExit('ERROR: unable to read occurrence records. Is this a valid CSV file?')

    # find column indices by name:
    header = f.readline().strip('\n').split(sep)
    try:
        xidx = header.index(xfield)
    except Exception:
        raise SystemExit('ERROR: unable to read occurrence records. Invalid column name specified: ' + xfield)
    try:
        yidx = header.index(yfield)
    except Exception:
        raise SystemExit('ERROR: unable to read occurrence records. Invalid column name specified: ' + yfield)
    del header

    # read x & y coordinates:
    xy = []
    for idx, line in enumerate(f):
        line = line.strip('\n').split(sep)
        try:
            xy.append([float(line[xidx]), float(line[yidx])])
        except Exception:
            print('WARNING: Invalid character in line ' + str(idx + 2))
            continue
    del xidx, yidx
    f.close()

    xy = np.array(xy)
    return xy


def read_sightings_vector(InputVector):
    """
    A function to read occurrence records from an OGR supported vector file (e.g. ESRI Shapefile).
    This must be a point dataset, not a line or polygon.
    
    :param problem: A python dictionary containing the simulation parameters.
    :return: a numpy.ndarray containing the x & y coordinates, shape=(n_samples, 2).
    
    """
    print('Reading sightings from vector file...')
    if not os.path.exists(InputVector):
        raise SystemExit('ERROR: unable to read occurrence records. Input file not found. Please check the file path.')

    from osgeo import ogr
    try:
        vector = ogr.Open(InputVector, 0)
        layer = vector.GetLayer()
    except Exception:
        raise SystemExit('ERROR: unable to read occurrence records. Is the input file a vector dataset?')

    # read x & y coordinates from vector:
    if layer.GetGeomType() == ogr.wkbPoint:
        xy = []
        for feature in layer:
            point = feature.GetGeometryRef()
            xy.append([point.GetX(), point.GetY()])
            del point
        del layer, vector
    elif layer.GetGeomType() == ogr.wkbMultiPoint:
        xy = []
        for feature in layer:
            multipoint = feature.GetGeometryRef()
            for i in range(multipoint.GetGeometryCount()):
                point = multipoint.GetGeometryRef(i)
                xy.append([point.GetX(), point.GetY()])
                del point
            del multipoint
        del layer, vector
    else:
        del layer, vector
        raise SystemExit('ERROR: unable to read occurrence records. The input vector does not contain points.')
    return np.array(xy)


def extract_raster_values(problem, xy):
    """
    A function to extract raster values using x & y coordinates.
    
    :param problem: A python dictionary containing the simulation parameters.
    :param xy: a numpy.ndarray containing x & y coordinates, shape=(n_samples, 2).
    :return: a structured np.recarray containing the extracted raster values.
    
    """
    print('Extracting raster values..')
    if not isinstance(problem, dict):
        raise SystemExit('ERROR: please provide a Python dictionary containing the simulation parameters.')

    RasterList = problem['Images']

    # check that RasterList is a list or np.ndarray:
    if not isinstance(RasterList, list) and not isinstance(RasterList, np.ndarray):
        raise SystemExit('ERROR: please provide a list of rasters containing the environmental predictors.')

    # check that xy has the correct dimensions:
    if xy.ndim != 2:
        raise SystemExit('ERROR: The xy coordinates must be two-dimensional.')
    if xy.shape[1] != 2:
        try:
            xy = xy.reshape(-1, 2)
        except Exception:
            raise SystemExit('ERROR: The xy coordinates must be two-dimensional.')

    # extract values from each raster:
    names, df = [[],[]]
    for raster in RasterList:
        if not os.path.exists(raster):
            raise SystemExit('ERROR: ' + raster + ' not found. Please check the file path.')

        names.append('.'.join(os.path.basename(raster).split('.')[:-1]))

        try:
            im = gdal.Open(raster, 0)
            geot = im.GetGeoTransform()
            array = im.GetRasterBand(1).ReadAsArray()
            del im
        except Exception:
            raise SystemExit('ERROR: unable to open ' + raster)

        values = []
        for x, y in xy:
            try:
                xidx = int((x - geot[0]) / geot[1])
                yidx = int((geot[3] - y) / -geot[5])
                values.append(array[yidx][xidx])
                del xidx, yidx
            except Exception:
                values.append(0.0)
        del geot, array

        values = np.array(values)

        if values.size == xy.shape[0]:
            df.append(values)
            del values
        else:
            raise SystemExit('Errors were encoutered when extracting raster values.')
            
    # convert df into a structured numpy.recarray:
    df = np.core.records.fromarrays(df, names=names)
    del names
    return df


def drop_absences(presence_absence_data, labels='Presence-Absence'):
    """
    An utility function to remove absence records and binary labels from presence-absence data.

    :param presence_absence_data: A structured np.recarray containing presence-absence data, shape=(n_samples, n_predictors).
    :param labels: Column in np.recarray containing the presence/absence binary labels.
    :return: a structured np.recarray with no labels and containing only presence records.
    
    """
    colnames = check_recarray(presence_absence_data)
    if labels not in colnames:
        # no class labels found, therefore return the original recarray:
        return presence_absence_data
    else:
        from numpy.lib.recfunctions import rec_drop_fields
        presence = np.where(presence_absence_data[labels] == 1)[0]
        if presence.size == 0:
            print('WARNING: No presence records were found in the presence-absence data.')
            del presence
            return rec_drop_fields(presence_absence_data, labels)
        else:
            data = presence_absence_data[presence]
            del presence
            return rec_drop_fields(data, labels)


def export_VIF(problem, presence_absence_data, labels='Presence-Absence'):
    """
    A function to export a variance inflation factors to a CSV file.
    Categorical variables are ignored.
    
    Interpretation of VIF scores (this is subjective)::
    
        1 = No multicollinearity.
        1-5 = Moderate multicollinearity.
        > 5 = High multicollinearity.
        > 10 = This predictor should be removed from the model.

    :param problem: A python dictionary containing the simulation parameters.
    :param presence_absence_data: a np.recarray containing presence-absence data.
    :param labels: Column in np.recarray containing the presence/absence binary labels (optional).
    
    """
    print('Exporting VIF scores...')
    if not isinstance(problem, dict):
        raise SystemExit('ERROR: please provide a Python dictionary containing the simulation parameters.')
    if not isinstance(presence_absence_data, np.recarray):
        try:
            colnames = np.array(presence_absence_data.dtype.names)
        except Exception:
            raise SystemExit('ERROR: the presence-absence data must be a structured numpy array.')
    else:
        colnames = np.array(presence_absence_data.dtype.names)

    # remove labels and drop absence records:
    data = drop_absences(presence_absence_data, labels)

    # drop categorical variables:
    data = drop_categoricals(data, problem['Categorical'])
    colnames = np.array(data.dtype.names)

    # convert np.recarray into np.ndarray:
    data = np.array(data.tolist())

    # ensure the records are not one-dimensional:
    if data.ndim != 2:
        raise SystemExit('ERROR: The input array must be two-dimensional.')

    # check that len(names) == len(n_predictors):
    if data.shape[1] != len(colnames):
        raise SystemExit('ERROR: number of names != number of predictor variables.')

    from sklearn.linear_model import LinearRegression
    linear_model = LinearRegression()
    VIF_scores = []
    for i in range(data.shape[1]):
        # fit auxiliary regressions to each predictor variable in X:
        y = data[:, i]
        x = np.delete(data, i, axis=1)
        linear_model.fit(x, y)
        y_pred = linear_model.predict(x)
        del x

        # calculate the coefficient of determination:
        ss_tot = sum((y - np.mean(y))**2)
        ss_res = sum((y - y_pred)**2)
        r2 = 1 - (ss_res / ss_tot)
        del y, y_pred, ss_res, ss_tot

        # calculate VIF:
        if r2 == 1:
            r2 = 0.9999  # this prevents division by zero errors when r2 == 1.
        VIF_scores.append(1 / (1 - r2))
        del r2
    del linear_model, data

    VIF_scores = np.array(VIF_scores).round(decimals=1).astype(str)

    # export scores to a csv file:
    OutputCSV = os.path.abspath(problem['Output Directory']) + '/VIF_Scores.csv'
    try:
        f = open(OutputCSV, 'w')
    except Exception:
        raise SystemExit('ERROR: unable to create output CSV file. Please check the file path.')

    f.write('Variable,VIF\n')
    for i, score in enumerate(VIF_scores):
        f.write(colnames[i] + ',' + score + '\n')
    f.close()
    del VIF_scores, colnames, OutputCSV


def export_correlation_matrix(problem, presence_absence_data, labels='Presence-Absence'):
    """
    A function to export a pearson correlation matrix to a CSV file.
    Categorical variables are ignored.
    
    Interpretation of Pearson correlation values (subjective)::
    
        < 0.6 = No collinearity.
        0.6-0.8 = Moderate collinearity.
        > 0.8 = Strong collinearity. One of these predictors should be removed from the model.

    :param problem: A python dictionary containing the simulation parameters.
    :param presence_absence_data: a np.recarray containing presence-absence data.
    :param labels: Column in np.recarray containing the presence/absence binary labels.
    
    
    """
    print('Exporting a Pearson correlation matrix...') 
    if not isinstance(problem, dict):
        raise SystemExit('ERROR: please provide a Python dictionary containing the simulation parameters.')
    
    colnames = check_recarray(presence_absence_data)

    # remove labels and drop absence records:
    data = drop_absences(presence_absence_data, labels)

    # drop categorical variables:
    data = drop_categoricals(data, problem['Categorical'])
    colnames = np.array(data.dtype.names)

    # convert np.recarray into np.ndarray:
    data = np.array(data.tolist())

    # ensure the records are not one-dimensional:
    if data.ndim != 2:
        raise SystemExit('ERROR: The input array must be two-dimensional.')

    # transpose data so each row contains an environmental predictor:
    data = data.T

    # check that len(names) == len(n_predictors):
    if data.shape[0] != len(colnames):
        raise SystemExit('ERROR: number of names != number of predictor variables.')

    # generate correlation matrix:
    corr = np.corrcoef(data).round(decimals=3).astype(str)
    del data

    # export matrix to csv:
    OutputCSV = os.path.abspath(problem['Output Directory']) + '/Pearson_Correlation_Matrix.csv'
    try:
        f = open(OutputCSV, 'w')
    except Exception:
        raise SystemExit('ERROR: unable to create output CSV file. Please check the file path.')

    f.write(',' + ','.join(colnames) + '\n')
    for idx, row in enumerate(corr):
        f.write(colnames[idx] + ',' + ','.join(row) + '\n')
    f.close()
    del corr, colnames, OutputCSV


def drop_null_records(problem, records):
    """
    A function to remove presence or absence records with no valid data.

    :param problem: A python dictionary containing the simulation parameters.
    :param records: a np.recarray containing values extracted from environmental rasters.
    :return: a numpy.recarray containing valid presence-absence records.
    
    """
    print('Removing null records...')
    if not isinstance(problem, dict):
        raise SystemExit('ERROR: please provide a Python dictionary containing the simulation parameters.')
    if not isinstance(records, np.recarray):
        raise SystemExit('ERROR: the presence-absence records must be a structured numpy array.')

    null_value = problem['Null Value']
    if not isinstance(null_value, int) and not isinstance(null_value, float):
        try:
            null_value = float(null_value)
        except Exception:
            raise SystemExit('ERROR: the null value must be numeric.')

    # create index of valid records:
    n_predictors = len(list(records.dtype.names))
    good_idx = []
    for idx, record in enumerate(records):
        record = np.array(record.tolist())
        bad = np.where(record == null_value)[0]
        if bad.size != n_predictors:
            good_idx.append(idx)
        del record, bad
    del null_value

    if not good_idx:
        del good_idx
        raise SystemExit('ERROR: no valid records were found.')
    else:
        records = records[good_idx]
        del good_idx
        return records


def combine_records(presence_data, absence_data):
    """
    A function to concatenate and classify presence-absence records.
    Absences are assigned values = 0, presence = 1.

    :param presence_data: a np.recarray containing presence data.
    :param absence_data: a np.recarray containing absence data.
    :return: a np.recarray containing presence-absence data, shape=(n_samples, n_predictors + label_column).
    
    """
    print('Combining the presence and absence data...')
    if not isinstance(presence_data, np.recarray):
        raise SystemExit('ERROR: the presence data must be a structured numpy array.')
    if not isinstance(absence_data, np.recarray):
        raise SystemExit('ERROR: the absence data must be a structured numpy array.')

    # classify presence / absence:
    clf = np.array([1 for i in presence_data] + [0 for i in absence_data]).astype('uint8')

    # concatenate presence & absence data
    df = np.concatenate([presence_data, absence_data])

    # merge data and labels:
    from numpy.lib.recfunctions import rec_append_fields
    df = rec_append_fields(df, 'Presence-Absence', clf)
    del clf
    return df


def train_test_split(presence_absence_data, test_fraction=0.1):
    """
    A function to split presence-absence records into training and test data.

    :param presence_absence_data: A np.ndarray or np.recarray containing presence-absence data,
                                  shape=(n_samples, n_predictors).
    :param test_fraction: Float between 0.0 and 1.0 representing the proportion of data to include in the test split.
    
    """
    # ensure presence_absence_data is a numpy array:
    if not isinstance(presence_absence_data, np.recarray) or not isinstance(presence_absence_data, np.ndarray):
        try:
            presence_absence_data = np.array(presence_absence_data)
        except Exception:
            raise SystemExit('ERROR: unable to split the presence-absence data. Must be a np.recarray or np.ndarray.')

    if test_fraction <= 0 or test_fraction >= 1:
        test_fraction = 0.1
        print('WARNING: Test fraction must be a float between 0 and 1.')
        print('Using default value of 0.1...')

    # shuffle the presence-absence data to remove any ordering:
    presence_absence_data = np.copy(presence_absence_data)
    np.random.shuffle(presence_absence_data)

    sample_size = presence_absence_data.shape[0]
    test_size = int(test_fraction * sample_size)

    test = presence_absence_data[:test_size]
    train = presence_absence_data[test_size:]
    del presence_absence_data, sample_size, test_size
    return [train, test]


def bootstrap_records(presence_absence_data):
    """
    A function to randomly resample presence-absence records with replacement.

    :param presence_absence_data: A np.ndarray or np.recarray containing presence-absence data,
                                  shape=(n_samples, n_predictors).

    :return: a list containing two numpy arrays: 1. train_partition = np.array containing training sample.
             2. test_partition = np.array containing test sample (same as original presence_absence_data).

    Reference: See page 277 (Table 16.1) in:
    Guisan et al. (2017). Habitat suitability and distribution models: with applications in R.
    
    """
    # ensure presence_absence_data is a numpy array:
    if not isinstance(presence_absence_data, np.recarray) or not isinstance(presence_absence_data, np.ndarray):
        try:
            presence_absence_data = np.array(presence_absence_data)
        except Exception:
            raise SystemExit('ERROR: unable to split the presence-absence data. Must be a np.recarray or np.ndarray.')

    sample_size = presence_absence_data.shape[0]

    # index the records:
    idx = np.arange(0, presence_absence_data.shape[0], 1, dtype='uint32')

    # randomly sample the index & use fancy indexing to select the presence-absence records:
    sample_idx = np.random.choice(idx, size=sample_size, replace=True, p=None)
    train_sample = presence_absence_data[sample_idx]
    del idx, sample_idx, sample_size
    return [train_sample, presence_absence_data]


def split_records_kfolds(presence_absence_data, n_folds=10):
    """
    A function to split presence-absence records into k folds.

    :param presence_absence_data: a np.ndarray or np.recarray containing presence-absence data,
                                  shape = (n_samples, n_predictors).
    :param n_folds: integer (default = 10).
    :return: a list of np.arrays with a length == n_folds.
    
    """
    # ensure presence_absence_data is a numpy array:
    if not isinstance(presence_absence_data, np.recarray) or not isinstance(presence_absence_data, np.ndarray):
        try:
            presence_absence_data = np.array(presence_absence_data)
        except Exception:
            raise SystemExit('ERROR: unable to split the presence-absence data. Must be a np.recarray or np.ndarray.')

    # ensure n_folds is an integer:
    try:
        n_folds = int(n_folds)
    except Exception:
        raise SystemExit('ERROR: unable to split the presence-absence data. The number of folds must be numeric.')
    try:
        x2 = np.copy(presence_absence_data)
        # shuffle the presence-absence data to remove any ordering:
        np.random.shuffle(x2)
        # split the data into k folds:
        x2 = np.array_split(x2, n_folds)
        return x2
    except Exception:
        raise SystemExit('ERROR: unable to split the presence-absence records into k folds.')


def split_xy(Records, ClassLabels='Presence-Absence'):
    """
    A function to split predictor and response variables from presence-absence records.

    :param Records: a np.recarray containing presence-absence data.
    :param ClassLabels: string defining the np.recarray column containing the presence/absence binary class labels.
    
    """
    if ClassLabels not in list(Records.dtype.names):
        print('ERROR: unable to split the predictors and labels.')
        raise SystemExit('The presence-absence data does not contain a column named ' + ClassLabels)
    try:
        from numpy.lib.recfunctions import rec_drop_fields
        y = Records[ClassLabels].astype('uint8')
        x = rec_drop_fields(Records, ClassLabels)

        # drop any columns containing strings since they cannot be used in predictive model:
        for colname in list(x.dtype.names):
            try:
                x[colname][0].decode()  # check if first element is byte string object.
                x = rec_drop_fields(x, colname)  # drop column if no exception.
            except Exception:
                continue

        # convert x from np.recarray into np.ndarray:
        x = np.array(x.tolist())
        return [x, y]
    except Exception:
        raise SystemExit('ERROR: unable to split predictor and response variables from the presence-absence records.')


def export_training_data(presence_absence_data, OutputHDF, OutputDir, Overwrite=True):
    """
    A function to export a presence absence data to a HDF5 database.

    :param presence_absence_data: A np.recarray containing presence-absence data. Shape=(n_samples, n_predictors + 1).
    :param OutputHDF: Path to the output HDF5 file.
    :param OutputDir: HDF directory in which to write the data (e.g. "/Train_Data/Model_Run_1").
    :param Overwrite: Boolean option to specify whether OutputDir should be overwritten if already present in HDF5 file (Default: True).
    
    """
    check_recarray(presence_absence_data)

    if not isinstance(OutputHDF, str):
        try:
            OutputHDF = str(OutputHDF)
        except Exception:
            raise SystemExit('ERROR: unable to export presence absence data. The ouput file name must be a string.')
    if not isinstance(OutputDir, str):
        try:
            OutputDir = str(OutputDir)
        except Exception:
            raise SystemExit('ERROR: unable to export presence absence data. The ouput path must be a string.')
    try:
        import h5py
        hdf = h5py.File(OutputHDF, 'a')
        try:
            if hdf.get(OutputDir) and Overwrite == True:
                # delete OutputDir if it's already present:
                del hdf[OutputDir]
        except Exception:
            pass
        hdf.create_dataset(OutputDir, data=presence_absence_data, compression='gzip', compression_opts=1)
        hdf.close()
    except Exception:
        raise SystemExit('ERROR: unable to export presence absence data. The ouput file could not be created.')


def import_training_data(InputHDF, InputDir, Labels='Presence-Absence'):
    """
    A function to import presence absence data from a HDF5 database.

    :param InputHDF: Path to the input HDF5 file.
    :param InputDir: HDF directory where the presence-absence data is stored (e.g. "/Train_Data/Model_Run_1").
    :param Labels: Column name containing presence/absence binary labels. Default="Presence-Absence".
    :return: a structured numpy.rearray containing training data saved in the HDF5 database.
    
    """
    print('Reading training data from HDF5 database...')
    if not isinstance(InputHDF, str):
        try:
            InputHDF = str(InputHDF)
        except Exception:
            raise SystemExit('ERROR: unable to import presence-absence data. The input file name must be a string.')

    if not os.path.exists(InputHDF):
        raise SystemExit('ERROR: unable to import presence-absence data. Input file not found.')

    if not isinstance(InputDir, str):
        try:
            OutputDir = str(OutputDir)
        except Exception:
            raise SystemExit('ERROR: unable to import presence absence data. The input path must be a string.')

    try:
        import h5py
        hdf = h5py.File(InputHDF, 'r')
        try:
            df = hdf.get(InputDir)[()]
        except Exception:
            raise SystemExit('ERROR: unable to import presence absence data. The HDF5 directory was not found.')
        hdf.close()
    except Exception:
        raise SystemExit('ERROR: unable to import presence absence data. The input file could not be read.')

    if Labels not in list(df.dtype.names):
        print('WARNING: The training data does not contain a column named ' + Labels)
    return df


def probability_accuracy_scores(y_true, y_prob):
    """
    A function to calculate accuracy measures for probabilistic responses with sklearn and scipy.

    :param y_true: binary class labels, where 0 is absence and 1 is presence.
    :param y_prob: probability of presence scores generated by a species distribution model.
    :return: a list containing two arrays: 1. metrics = names of test metrics. 2. scores = test scores for each metric.

    Useful reference:
    https://machinelearningmastery.com/how-to-score-probability-predictions-in-python
    
    """
    # check inputs:
    if not isinstance(y_true, np.ndarray):
        y_true = np.array(y_true)
    if not isinstance(y_prob, np.ndarray):
        y_prob = np.array(y_prob)
    if y_true.ndim != 1:
        raise SystemExit('ERROR: the true labels must be provided in a 1D array.')
    if y_prob.ndim != 1:
        raise SystemExit('ERROR: the probability of presence values must be provided in a 1D array.')
    if y_true.size != y_prob.size:
        raise SystemExit('ERROR: unequal number of binary labels and probabilities.')   

    # ensure that y_true contains binary labels (i.e. 0 or 1 values):
    y_true = y_true.astype('uint8')
    if np.min(y_true) != 0 or np.max(y_true) != 1:
        raise SystemExit('ERROR: the true labels are not binary (zero or one values).')

    # remove elements with nan or infinite values:
    bad_idx = np.isfinite(y_true)
    y_true = y_true[bad_idx]
    y_prob = y_prob[bad_idx]
    bad_idx = np.isfinite(y_prob)
    y_true = y_true[bad_idx]
    y_prob = y_prob[bad_idx]
    del bad_idx

    from sklearn.metrics import roc_auc_score
    # calculates area under the receiver operating curve score.
    # A score of 0.5 shows the model is unable to discriminate between presence and absence.
    roc_auc = roc_auc_score(y_true, y_prob)

    from sklearn.metrics import average_precision_score
    # calculates area under the precision-recall curve. Perfect model = 1.0. 
    average_precision = average_precision_score(y_true, y_prob)

    from sklearn.metrics import brier_score_loss
    # This is a quadratic loss function that calculates the mean squared error between
    # predicted probabilities and the true presence-absence (binary) labels.
    # A model with no false positives/negatives has a score of 0.0. Perfect model = 1.0.
    brier_score = brier_score_loss(y_true, y_prob)

    from sklearn.metrics import log_loss
    # The is logarithmic loss function that more heavily penalises false positives/negatives than the brier score.
    # A model with no false positives/negatives has a score of 0.0. There is no upper bound.
    log_loss_score = log_loss(y_true, y_prob)

    from scipy.stats import pointbiserialr
    # The point biserial correlation coefficient, range -1 to 1.
    # Quantifies the correlation between a binary and continuous variable.
    r = pointbiserialr(y_true, y_prob)[0]
  
    metrics = np.array(['Test AUC', 'Point-Biserial r', 'Av. Precision', 'Brier Score', 'Log-Loss Score'])
    scores = np.array([roc_auc, r, average_precision, brier_score, log_loss_score]).round(decimals=6)
    del roc_auc, r, average_precision, brier_score, log_loss_score, y_true, y_prob
    return [metrics, scores]


def binary_accuracy_scores(y_true, y_pred):
    """
    A function to calculate accuracy measures for a binary classification.

    Required Parameters:
    :param y_true: observed binary labels, where 0 is absence and 1 is presence.
    :param y_pred: predicted binary labels, where 0 is absence and 1 is presence.

    :return: a list containing two np.arrays: 1. metrics = names of test metrics. 2. scores = test scores for each metric.

    Reference: See pages 253 - 255 in:
    Guisan et al. (2017). Habitat suitability and distribution models: with applications in R.
    
    """
    # check inputs:
    if not isinstance(y_true, np.ndarray):
        y_true = np.array(y_true)
    if not isinstance(y_pred, np.ndarray):
        y_pred = np.array(y_pred)
    if y_true.ndim != 1:
        print(y_true)
        raise SystemExit('ERROR: the true labels are not in a 1D array.')
    if y_pred.ndim != 1:
        print(y_pred)
        raise SystemExit('ERROR: the predicted labels are not in a 1D array.')
    if y_true.size != y_pred.size:
        raise SystemExit('ERROR: unequal number of binary labels.')

    # ensure that y_true, y_pred contain binary labels (i.e. 0 or 1 values):
    y_true = y_true.astype('uint8')
    y_pred = y_pred.astype('uint8')
    if np.min(y_true) != 0 or np.max(y_true) != 1:
        print(y_true)
        raise SystemExit('ERROR: the true labels are not binary (zero or one values).')
    if np.min(y_pred) != 0 or np.max(y_pred) != 1:
        print(y_pred)
        raise SystemExit('ERROR: the predicted labels are not binary (zero or one values).')

    metrics = np.array(['Prevalence', 'Overall Diagnostic Power', 'Correct Classification Rate', 'Misclassification Rate',
                        'Presence Predictive Power', 'Absence Predictive Power', 'Accuracy', 'Balanced Accuracy',
                        'Sensitivity', 'Specificity', 'Precision', 'F1 Score', 'Matthews Correlation', 'Cohen Kappa',
                        'Normalised Mutual Information', 'True Skill (Hanssen-Kuiper) Score'])

    # remove elements with nan or infinite values:
    bad_idx = np.isfinite(y_true)
    y_true = y_true[bad_idx]
    y_pred = y_pred[bad_idx]
    bad_idx = np.isfinite(y_pred)
    y_true = y_true[bad_idx]
    y_pred = y_pred[bad_idx]
    del bad_idx

    try:
        n_presence = np.where(y_true == 1)[0].size
        n_absence = np.where(y_true == 0)[0].size

        # calculate true-presence, true-absence, false-presence and false-absence:
        TP = np.where((y_true == 1) & (y_pred == 1))[0].size
        TA = np.where((y_true == 0) & (y_pred == 0))[0].size
        FP = np.where((y_true == 1) & (y_pred == 0))[0].size
        FA = np.where((y_true == 0) & (y_pred == 1))[0].size  # aka sweet FA!

        # proportion of presence records:
        prevalence = (TP / FA) / y_true.size

        # proportion of absence records:
        ODP = 1 - prevalence

        # correct classification & misclassification rate
        CCR = (TP + TA) / y_true.size
        MR = (FP + FA) / y_true.size

        # Sensitivity (aka Recall or True Positive Rate):
        sensitivity = TP / n_presence

        # false presence rate - inverse of sensitivity (redundant?)
        #FPR = 1  - sensitivity

        # Presence and absence predictive power:
        PPP = TP / (TP + FP)
        APP = TA / (TA + FA)

        # Specificity (aka True Negative Rate):
        specificity = TA / n_absence

        # false positive rate - inverse of specificity (redundant?)
        #FPR = 1 - specificity

        # Accuracy scores:
        accuracy = (TP + TA) / (n_presence + n_absence)
        balanced_accuracy = ((TP / n_presence) + (TA / n_absence)) / 2

        # precision:
        precision = TP / (TP + FP)

        # F1 score:
        f1_score = 2 * TP / ((2*TP) + FP + FA)

        # Matthews Correlation Coefficient:
        MCC = ((TP * TA) - (FP * FA)) / (((TP + FP) * (TP + FA) * (TA + FP) * (TA + FA))**0.5)

        # Hanssen-Kuiper skill (unreliable when TA is very large):
        TSS = sensitivity + specificity - 1
        del n_presence, n_absence, TP, TA, FP, FA

        from sklearn.metrics import normalized_mutual_info_score as nmi_score
        nmi_score = nmi_score(y_true, y_pred)

        # Cohen's Kappa (caution: sensitive to sample size and proportion of presence records):
        from sklearn.metrics import cohen_kappa_score as kappa
        kappa = kappa(y_true, y_pred)

        scores = np.array([prevalence, ODP, CCR, MR, PPP, APP, accuracy, balanced_accuracy, sensitivity,
                           specificity, precision, f1_score, MCC, kappa, nmi_score, TSS]).round(decimals=6)
        del prevalence, ODP, CCR, MR, PPP, APP, accuracy, balanced_accuracy, sensitivity
        del specificity, precision, f1_score, MCC, kappa, nmi_score, TSS
    except Exception:
        scores = np.zeros(len(metrics))

    if metrics.size == scores.size:
        return [metrics, scores]
    else:
        raise SystemExit('ERROR: unable to calculate accuracy metrics.')


def scores2txt(OutputCSV, names, scores):
    """
    A function to export accuracy measures to an ASCII CSV file.

    :param OutputCSV: path to the output file.
    :param names: a numpy 1D array containing the metric names.
    :param scores: a numpy 1D array containing the scores for each metric.
    
    """
    if not isinstance(names, np.ndarray):
        names = np.array(names)
    if not isinstance(scores, np.ndarray):
        scores = np.array(scores)
    if names.ndim != 1:
        raise SystemExit('ERROR: names are not in a 1D array.')
    if scores.ndim != 1:
        raise SystemExit('ERROR: scores are not in a 1D array.')
    if names.size != scores.size:
        print('ERROR: length of names is not equal to number of scores:')
        print(names)
        print(scores)
        raise SystemExit()

    if os.path.exists(OutputCSV):
        # append metrics without writing header row:
        f = open(OutputCSV, 'a')
    else:
        try:
            f = open(OutputCSV, 'w')
            f.write('Metric,Score\n')
        except Exception:
            raise SystemExit('ERROR: unable to write metrics to output file.')

    scores = scores.round(decimals=6).astype(str)
    for i, score in enumerate(scores):
        f.write(names[i] + ',' + score + '\n')
    f.close()


def check_image_tiling(InputImages):
    """
    A function to determine if GDAL images are tiled.

    :param InputImages: A list or np.array of GDAL supported images.
    :return: a list containing three elements: 1. BlockProcessing: a boolean indicator of tiling.
             2. xBlockSize: tile size along the x axis. 3. yBlockSize: tile size along the y axis.
    
    """
    if not isinstance(InputImages, list) or not isinstance(InputImages, np.ndarray):
        try:
            InputImages = np.array(InputImages).astype(str)
        except Exception:
            raise SystemExit('ERROR: InputImages must be a list of strings.')

    # check that all of the input images are tiled rasters:
    BlockProcessing = True
    for image in InputImages:
        if not isinstance(image, str):
            raise SystemExit('ERROR: image paths must be provided as a string.')
        try:
            im = gdal.Open(image, 0)
        except Exception:
            raise SystemExit('ERROR: GDAL was unable to read the image ' + image)

        xBlockSize, yBlockSize = im.GetRasterBand(1).GetBlockSize()
        del im

        if xBlockSize == 1 or yBlockSize == 1:
            BlockProcessing = False
            break
    return [BlockProcessing, xBlockSize, yBlockSize]


def get_image_blocks(xsize, ysize, xBlockSize, yBlockSize):
    """
    A function to calculate image block coordinates and dimensions.
    Used for the processing of tiled rasters.

    Required Parameters:
    :param xsize: The number of pixels along the image x axis.
    :param ysize: The number of pixels along the image y axis.
    :param xBlockSize: Image block size along the x axis.
    :param yBlockSize: Image block size along the y axis.
    :return: a np.array with the following elements: [xblock starting index, yblock starting index, n_columns in block, n_rows in block]
    
    """
    BlockInfo = []
    for yidx in range(0, ysize, yBlockSize):
        if yidx + yBlockSize < ysize:
            Rows = yBlockSize
        else:
            Rows = ysize - yidx

        for xidx in range(0, xsize, xBlockSize):
            if xidx + xBlockSize < xsize:
                Cols = xBlockSize
            else:
                Cols = xsize - xidx

            BlockInfo.append([xidx, yidx, Cols, Rows])
    del xidx, yidx, Cols, Rows
    BlockInfo = np.array(BlockInfo).astype('uint32')
    return BlockInfo


def classify_presence(InputImage, OutputImage, Threshold, RasterBand=1, gdalformat='GTiff'):
    """
    A function to classify probability of presence into a binary (presence or absence) raster
    where 0 = absence and 1 = presence. 255 is assigned to no data regions.

    :param InputImage: path to GDAL supported image.
    :param OutputImage: path to output image.
    :param Threshold: threshold for classifying probability.
    :param gdalformat: GDAL image output format. Supported choices = ['GTiff', 'KEA']. Default GTiff
    :param RasterBand: raster band number (optional).
    
    """
    if not isinstance(InputImage, str):
        raise SystemExit('ERROR: input image must be a string.')
    if not os.path.exists(InputImage):
        raise SystemExit('ERROR: input image not found. Please check the file path.')
    if not isinstance(Threshold, float):
        try:
            Threshold = float(Threshold)
        except Exception:
            raise SystemExit('ERROR: Threshold for binary classification must be a float between 0 and 1.')
    if Threshold <= 0.0 or Threshold >= 1.0:
        raise SystemExit('ERROR: Threshold for binary classification must be a float between 0 and 1.')
    if not isinstance(gdalformat, str) or gdalformat not in ['GTiff', 'KEA']:
        print('WARNING: GDAL image output format must be "GTiff" or "KEA". Defaulting to "GTiff".')
        gdalformat = 'GTiff'

    # open the image:
    try:
        im = gdal.Open(InputImage, 0)
        NoDataValue = im.GetRasterBand(1).GetNoDataValue()
    except Exception:
        raise SystemExit('ERROR: unable to read the input image with GDAL.')

    # create output image:
    try:
        Driver = gdal.GetDriverByName(gdalformat)
        if gdalformat == 'GTiff':
            OutImage = Driver.Create(OutputImage, im.RasterXSize, im.RasterYSize, 1, 1, options=['COMPRESS=DEFLATE', 'TILED=YES'])
        else:
            OutImage = Driver.Create(OutputImage, im.RasterXSize, im.RasterYSize, 1, 1)
        del Driver
        OutImage.SetProjection(im.GetProjection())
        OutImage.SetGeoTransform(im.GetGeoTransform())
        OutBand = OutImage.GetRasterBand(1)
        OutBand.SetNoDataValue(255)
        OutBand.SetDescription('Classified Presence')
    except Exception:
        raise SystemExit('ERROR: unable to create the output image. Please check the file path.')

    xBlockSize, yBlockSize = im.GetRasterBand(1).GetBlockSize()
    if xBlockSize == 1 or yBlockSize == 1:
        # image is not tiled, therefore read the whole array into memory:
        array = im.GetRasterBand(RasterBand).ReadAsArray()

        # perform classification:
        clf = np.empty_like(array, dtype='uint8')
        clf[...] = 255
        clf = np.where((array < Threshold) & (array != NoDataValue), 0, clf)
        clf = np.where((array >= Threshold) & (array != NoDataValue), 1, clf)
        del array

        # write array to image:
        OutBand.WriteArray(clf)
        del clf, OutImage, OutBand 
    else:
        # used tiled image processing:
        RasterBlocks = get_image_blocks(im.RasterXSize, im.RasterYSize, xBlockSize, yBlockSize)

        # iterate through blocks:
        for idx, Block in enumerate(RasterBlocks):
            array = im.GetRasterBand(RasterBand).ReadAsArray(int(Block[0]), int(Block[1]), int(Block[2]), int(Block[3]))

            # perform classification:
            clf = np.empty_like(array, dtype='uint8')
            clf[...] = 255
            clf = np.where((array < Threshold) & (array != NoDataValue), 0, clf)
            clf = np.where((array >= Threshold) & (array != NoDataValue), 1, clf)
            del array

            # write block to output image:
            OutBand.WriteArray(clf, int(Block[0]), int(Block[1]))
            del clf
        del RasterBlocks, OutImage, OutBand
    del im, NoDataValue

    # build image overviews:
    im = gdal.Open(OutputImage, 1)
    im.BuildOverviews('NEAREST', [4, 8, 16, 32, 64, 128])
    del im


def habitat_overlap(OutputImage, Image1, Image2, RasterBand1=1, RasterBand2=1, gdalformat='GTiff'):
    """
    A function to calculate habitat overlap using two images where probability
    of presence has been classified as a binary 0 = absence, 1 = presence.

    Required Parameters:
    :param OutputImage: Path to output image, to be classified as a binary 0 = no overlap, 1 = overlap.
    :param Image1: Binary image of classified probability of presence for species/model 1.
    :param Image2: Binary image of classified probability of presence for species/model 2.
    :param RasterBand1: Raster band number of Image1. Default = 1.
    :param RasterBand2: Raster band number of Image2. Default = 1.
    :param gdalformat: GDAL image output format. Supported choices = ["GTiff", "KEA"].
    :return: the proportion of pixels where both species reside (0 to 1 scale).
             This value can be converted to a percentage by multiplying by 100.
    """
    print('Calculating habitat overlap...')
    if not isinstance(OutputImage, str):
        raise SystemExit('ERROR: unable to calculate habitat_overlap. Output image must be a string.')
    for Image in [Image1, Image2]:
        if not isinstance(Image, str):
            print('ERROR: unable to calculate habitat_overlap. Input image not a string:')
            raise SystemExit(Image)
        if not os.path.exists(Image):
            print('ERROR: unable to calculate habitat_overlap. Input image not found:')
            raise SystemExit(Image)
    if not isinstance(gdalformat, str) or gdalformat not in ['GTiff', 'KEA']:
        print('WARNING: GDAL image output format must be "GTiff" or "KEA". Defaulting to "GTiff".')
        gdalformat = 'GTiff'

    # check that all images have the same coordinate system and extents:
    geot1, srs1, xsize1, ysize1, noDataVal1 = read_raster_information(Image1)
    geot2, srs2, xsize2, ysize2, noDataVal2 = read_raster_information(Image2)

    if geot1 != geot2:
        print('ERROR: unable to calculate habitat_overlap. The image transformations differ:')
        print(geot1)
        print(geot2)
        raise SystemExit()
    if srs1 != srs2:
        print('ERROR: unable to calculate habitat_overlap. The image coordinate systems differ:')
        print(srs1)
        print(srs2)
        raise SystemExit()
    if xsize1 != xsize2:
        print('ERROR: unable to calculate habitat_overlap. The image dimensions differ along the X axis:')
        print(xsize1)
        print(xsize2)
        raise SystemExit()
    if ysize1 != ysize2:
        print('ERROR: unable to calculate habitat_overlap. The image dimensions differ along the Y axis:')
        print(ysize1)
        print(ysize2)
        raise SystemExit()
    del geot2, srs2, xsize2, ysize2, noDataVal1, noDataVal2

    # create output image:
    Driver = gdal.GetDriverByName(gdalformat)
    if gdalformat == 'GTiff':
        OutImage = Driver.Create(OutputImage, xsize1, ysize1, 1, 1, options=['COMPRESS=DEFLATE', 'TILED=YES'])
    else:
        OutImage = Driver.Create(OutputImage, xsize1, ysize1, 1, 1)
    OutImage.SetProjection(srs1)
    OutImage.SetGeoTransform(geot1) 
    OutBand = OutImage.GetRasterBand(1)
    OutBand.SetNoDataValue(255)
    OutBand.SetDescription('Habitat Overlap')
    del geot1, srs1

    # check if both images are tiled:
    BlockProcessing, xBlockSize, yBlockSize = check_image_tiling([Image1, Image2])

    im1 = gdal.Open(Image1, 0)
    im2 = gdal.Open(Image2, 0)
    if BlockProcessing == False:
        # image is not tiled, therefore the whole arrays must be read into memory (not efficient!):
        arr1 = im1.GetRasterBand(RasterBand1).ReadAsArray()
        arr2 = im2.GetRasterBand(RasterBand2).ReadAsArray()
        del im1, im2

        clf = np.empty_like(arr1, dtype='uint8')
        clf[...] = 255
        clf = np.where((arr1 == 1) & (arr2 == 1), 1, clf)
        clf = np.where((arr1 == 0) & (arr2 == 1), 0, clf)
        clf = np.where((arr1 == 1) & (arr2 == 0), 0, clf)
        del arr1, arr2

        OutBand.WriteArray(clf)
        agreement = np.where(clf == 1)[0].size
        disagreement = np.where(clf == 0)[0].size
        del clf

        overlap = agreement / (agreement + disagreement)
        del agreement, disagreement
    else:
        # used tiled image processing (more memory efficient):
        RasterBlocks = get_image_blocks(xsize1, ysize1, xBlockSize, yBlockSize)

        # iterate through image blocks:
        agreement, disagreement = [[],[]]
        for idx, Block in enumerate(RasterBlocks):
            arr1 = im1.GetRasterBand(RasterBand1).ReadAsArray(int(Block[0]), int(Block[1]), int(Block[2]), int(Block[3]))
            arr2 = im2.GetRasterBand(RasterBand2).ReadAsArray(int(Block[0]), int(Block[1]), int(Block[2]), int(Block[3]))

            clf = np.empty_like(arr1, dtype='uint8')
            clf[...] = 255
            clf = np.where((arr1 == 1) & (arr2 == 1), 1, clf)
            clf = np.where((arr1 == 0) & (arr2 == 1), 0, clf)
            clf = np.where((arr1 == 1) & (arr2 == 0), 0, clf)
            del arr1, arr2

            agreement.append(np.where(clf == 1)[0].size)
            disagreement.append(np.where(clf == 0)[0].size)

            # write block to output image:
            OutBand.WriteArray(clf, int(Block[0]), int(Block[1]))
            del clf
            ProgressBar(RasterBlocks.shape[0], idx + 1)
        del RasterBlocks, im1, im2

        agreement, disagreement = sum(agreement), sum(disagreement)
        overlap = agreement / (agreement + disagreement)
        del agreement, disagreement
    del OutImage, OutBand, xsize1, ysize1, BlockProcessing, xBlockSize, yBlockSize

    # build image overviews:
    im = gdal.Open(OutputImage, 1)
    im.BuildOverviews('NEAREST', [4, 8, 16, 32, 64, 128])
    del im

    print('Habitat overlap:', '%.3f' % overlap)
    return overlap


def export_presence_image(problem, OutputImage, estimator, transformer=False, encoder=False, gdalformat='GTiff'):
    """
    A function to export a probability of presence raster.

    Required Parameters:
    :param problem: A python dictionary containing the simulation parameters.
    :param OutputImage: Path to the output image.
    :param estimator: A fitted Sklearn classifier with a predict_proba method.
    :param transformer: A fitted Sklearn transformer with an apply method.
    :param encoder: A fitted Sklearn encoder with a transform method.
    :param gdalformat: GDAL image output format. Supported choices = ["GTiff", "KEA"].
    
    """
    if not isinstance(problem, dict):
        raise SystemExit('ERROR: unable to export presence image. Please provide a Python dictionary containing the simulation parameters.')
    if not isinstance(OutputImage, str):
        raise SystemExit('ERROR: unable to export presence image. Path to output image must be a string.')
    if not isinstance(gdalformat, str) or gdalformat not in ['GTiff', 'KEA']:
        print('WARNING: GDAL image output format must be "GTiff" or "KEA". Defaulting to "GTiff".')
        gdalformat = 'GTiff'

    # check if the input images are tiled rasters:
    InputImages = problem['Images']
    BlockProcessing, xBlockSize, yBlockSize = check_image_tiling(InputImages)

    # create output image:
    Driver = gdal.GetDriverByName(gdalformat)
    if gdalformat == 'GTiff':
        OutImage = Driver.Create(OutputImage, problem['rasterxsize'], problem['rasterysize'], 1, 6,
                                 options=['COMPRESS=DEFLATE', 'TILED=YES'])
    else:
        OutImage = Driver.Create(OutputImage, problem['rasterxsize'], problem['rasterysize'], 1, 6)
    del Driver
    OutImage.SetProjection(problem['projection'])
    OutImage.SetGeoTransform(problem['geotransform']) 
    OutBand = OutImage.GetRasterBand(1)
    OutBand.SetNoDataValue(0.0)
    OutBand.SetDescription('Probability of Presence')

    # generate predictions:
    if BlockProcessing == False:
        # input images are not tiled, therefore read entire arrays into memory at once (not efficient):
        ImageData = []
        for idx, image in enumerate(InputImages):
            im = gdal.Open(image, 0)
            array = im.GetRasterBand(1).ReadAsArray()

            if idx == 0:
                # create binary mask:
                ValidMask = np.ones_like(array, dtype='uint8')
                ValidMask = np.where(array == problem['Null Value'], 0, ValidMask)

            array = array.ravel()
            ImageData.append(array)
            del array, im
        ImageData = np.array(ImageData).T

        if transformer and encoder:
            ImageData = transformer.apply(ImageData)[:, :, 0]
            ImageData = encoder.transform(ImageData)

        # predict presence-absence probability:
        absence_probability, presence_probability = estimator.predict_proba(ImageData).T
        del ImageData, absence_probability
        presence_probability = presence_probability.reshape(int(Block[3]), int(Block[2]))

        # mask invalid regions:
        presence_probability = presence_probability * ValidMask  
        del ValidMask

        # add offset to prevent zero probability from being nodata:
        presence_probability = presence_probability + 0.000001

        OutBand.WriteArray(presence_probability)
        del presence_probability
    else:
        # used tiled image processing (far more memory efficient):
        im = gdal.Open(InputImages[0], 0)
        RasterBlocks = get_image_blocks(problem['rasterxsize'], problem['rasterysize'], xBlockSize, yBlockSize)
        del im

        for Block in RasterBlocks:
            ImageData = []
            for idx, image in enumerate(InputImages):
                im = gdal.Open(image, 0)
                array = im.GetRasterBand(1).ReadAsArray(int(Block[0]), int(Block[1]), int(Block[2]), int(Block[3]))
                del im

                if idx == 0:
                    mask = np.ma.masked_equal(array, problem['Null Value'])
                    valid_pixels = np.ma.compressed(mask).size

                if valid_pixels == 0:
                    # block contains no valid pixels so no need to predict:
                    presence_probability = np.zeros_like(array)
                    break
                else:
                    if idx == 0:
                        # create binary mask:
                        ValidMask = np.ones_like(array, dtype='uint8')
                        ValidMask = np.where(array == problem['Null Value'], 0, ValidMask)
                    ImageData.append(array.ravel())
                del array
            del mask, valid_pixels
            
            if ImageData:
                # predict presence probability:
                ImageData = np.array(ImageData).T

                if transformer and encoder:
                    ImageData = transformer.apply(ImageData)[:, :, 0]
                    ImageData = encoder.transform(ImageData)

                absence_probability, presence_probability = estimator.predict_proba(ImageData).T
                del ImageData, absence_probability
                presence_probability = presence_probability.reshape(int(Block[3]), int(Block[2]))

                # mask invalid regions:
                presence_probability = presence_probability * ValidMask  
                del ValidMask

                # add offset to prevent zero probability from being nodata:
                presence_probability = presence_probability + 0.000001

            OutBand.WriteArray(presence_probability, int(Block[0]), int(Block[1]))
            del presence_probability
        del RasterBlocks
    del OutImage, OutBand, InputImages

    # build image overviews:
    im = gdal.Open(OutputImage, 1)
    im.BuildOverviews('NEAREST', [4, 8, 16, 32, 64, 128])
    del im


def export_mean_probability(problem, InputImages, gdalformat='GTiff'):
    """
    A function to generate images of the mean, st.deviation and coefficient of variation
    for probability of presence.

    Note: Only the first raster band is read from each input image.

    :param problem: A python dictionary containing the simulation parameters.
    :param InputImages: A list of probability of presence images.
    :param gdalformat: GDAL image output format. Supported choices = ["GTiff", "KEA"].
    
    """
    print('Exporting mean probability of presence image..')
    # check if InputImages is a list with length > 1:
    if not isinstance(InputImages, list):
        try:
            InputImages = list(InputImages)
        except Exception:
            raise SystemExit('ERROR: InputImages must be a list with a length greater than one.')
    if len(InputImages) <= 1: 
        raise SystemExit('ERROR: InputImages must be a list with a length greater than one.')
  
    # check if the input images are tiled rasters:
    BlockProcessing, xBlockSize, yBlockSize = check_image_tiling(InputImages)

    # define the output images:
    if not isinstance(gdalformat, str) or gdalformat not in ['GTiff', 'KEA']:
        print('WARNING: GDAL image output format must be "GTiff" or "KEA". Defaulting to "GTiff".')
        gdalformat = 'GTiff'

    # create output images:
    OutDir = os.path.abspath(problem['Output Directory']) + '/'
    Driver = gdal.GetDriverByName(gdalformat)
    if gdalformat == 'GTiff':
        OutputImages = [OutDir + 'Mean.tif', OutDir + 'StDev.tif', OutDir + 'CV.tif']
        OutImage1 = Driver.Create(OutputImages[0], problem['rasterxsize'], problem['rasterysize'], 1, 6,
                                  options=['COMPRESS=DEFLATE', 'TILED=YES'])
        OutImage2 = Driver.Create(OutputImages[1], problem['rasterxsize'], problem['rasterysize'], 1, 6,
                                  options=['COMPRESS=DEFLATE', 'TILED=YES'])
        OutImage3 = Driver.Create(OutputImages[2], problem['rasterxsize'], problem['rasterysize'], 1, 6,
                                  options=['COMPRESS=DEFLATE', 'TILED=YES'])
    else:
        OutputImages = [OutDir + 'Mean.kea', OutDir + 'StDev.kea', OutDir + 'CV.kea']
        OutImage1 = Driver.Create(OutputImages[0], problem['rasterxsize'], problem['rasterysize'], 1, 6)
        OutImage2 = Driver.Create(OutputImages[1], problem['rasterxsize'], problem['rasterysize'], 1, 6)
        OutImage3 = Driver.Create(OutputImages[2], problem['rasterxsize'], problem['rasterysize'], 1, 6)
    del OutDir, Driver

    for im in [OutImage1, OutImage2, OutImage3]:
        im.SetProjection(problem['projection'])
        im.SetGeoTransform(problem['geotransform'])

    OutBand1 = OutImage1.GetRasterBand(1)
    OutBand2 = OutImage2.GetRasterBand(1)
    OutBand3 = OutImage3.GetRasterBand(1)
    OutBand1.SetNoDataValue(0.0)
    OutBand2.SetNoDataValue(0.0)
    OutBand3.SetNoDataValue(0.0)
    OutBand1.SetDescription('Mean Probability of Presence')
    OutBand2.SetDescription('StDev Probability of Presence')
    OutBand3.SetDescription('CV Probability of Presence')

    # generate predictions:
    if BlockProcessing == False:
        # image is not tiled, therefore read entire arrays into memory at once (not efficient):
        ImageData = []
        for idx, image in enumerate(InputImages):
            im = gdal.Open(image, 0)
            if idx == 0:
                array = im.GetRasterBand(1).ReadAsArray()
                # create binary mask:
                ValidMask = np.ones_like(array, dtype='uint8')
                ValidMask = np.where(array == 0.0, 0, ValidMask)
                del array
            else:
                ImageData.append(im.GetRasterBand(1).ReadAsArray())
            del im

        # calculate mean & stdev:
        ImageData = np.array(ImageData)
        mean = np.mean(ImageData, axis=0)
        stdev = np.std(ImageData, axis=0)
        del ImageData

        # mask invalid regions:
        mean = mean * ValidMask
        stdev = stdev * ValidMask

        # calculate coefficient of variation:
        with warnings.catch_warnings():
            warnings.filterwarnings('ignore',category=RuntimeWarning)
            cv = stdev / mean
            cv[np.isnan(cv)] = 0.0
            cv = cv * ValidMask
        del ValidMask
        
        # write arrays to images:
        OutBand1.WriteArray(mean)
        del mean
        OutBand2.WriteArray(stdev)
        del stdev
        OutBand3.WriteArray(cv)
        del cv
    else:
        # used tiled image processing (far more memory efficient):
        im = gdal.Open(InputImages[0], 0)
        RasterBlocks = get_image_blocks(problem['rasterxsize'], problem['rasterysize'], xBlockSize, yBlockSize)
        del im

        # iterate through each image block:
        for Block in RasterBlocks:
            ImageData = []
            for idx, image in enumerate(InputImages):
                im = gdal.Open(image, 0)
                if idx == 0:
                    array = im.GetRasterBand(1).ReadAsArray(int(Block[0]), int(Block[1]), int(Block[2]), int(Block[3]))
                    ValidMask = np.ones_like(array, dtype='uint8')
                    ValidMask = np.where(array == 0.0, 0, ValidMask)
                    ImageData.append(array)
                    del array
                else:
                    ImageData.append(im.GetRasterBand(1).ReadAsArray(int(Block[0]), int(Block[1]), int(Block[2]), int(Block[3])))
                del im

            # calculate mean & stdev:
            ImageData = np.array(ImageData)
            mean = np.mean(ImageData, axis=0)
            stdev = np.std(ImageData, axis=0)
            del ImageData

            # mask invalid regions:
            mean = mean * ValidMask
            stdev = stdev * ValidMask

            # calculate coefficient of variation:
            with warnings.catch_warnings():
                warnings.filterwarnings('ignore',category=RuntimeWarning)
                cv = stdev / mean
                cv[np.isnan(cv)] = 0.0
                cv = cv * ValidMask
            del ValidMask
            
            # write arrays to images:
            OutBand1.WriteArray(mean, int(Block[0]), int(Block[1]))
            del mean
            OutBand2.WriteArray(stdev, int(Block[0]), int(Block[1]))
            del stdev
            OutBand3.WriteArray(cv, int(Block[0]), int(Block[1]))
            del cv
        del RasterBlocks
    del OutImage1, OutBand1, OutImage2, OutBand2, OutImage3, OutBand3

    # build image overviews:
    for Image in OutputImages:
        im = gdal.Open(Image, 1)
        im.BuildOverviews('NEAREST', [4, 8, 16, 32, 64, 128])
        del im
    del OutputImages, BlockProcessing, xBlockSize, yBlockSize


def process_roc_stats(fpr, tpr, n=101):
    """
    A function to process ROC statistics to ensure that there are an equal number
    of false-positives and true-positives after each model run.

    :param fpr: list or array of false positive rates.
    :param tpr: list or array of true positive rates.
    :param n: number of measurements to retain. Default=101.
    
    """
    # check fpr and tpr:
    if not isinstance(fpr, np.ndarray):
        fpr = np.array(fpr)
    if not isinstance(tpr, np.ndarray):
        tpr = np.array(tpr)
    if fpr.ndim != 1:
        raise SystemExit('ERROR: the false positive rate is multidimensional.')
    if tpr.ndim != 1:
        raise SystemExit('ERROR: the true positive rate is multidimensional.')
    if fpr.shape[0] != tpr.shape[0]:
        raise SystemExit('ERROR: fpr and tpr are of unequal lengths.')

    # create new range of values for x:
    fpr_new = np.linspace(0, 1.0, n)
    # linearly interpolate new tpr values at each value in fpr_new:
    tpr_new = np.interp(fpr_new, fpr, tpr)
    return fpr_new, tpr_new


def plot_roc_curve(fpr, tpr, OutFile):
    """
    A function to plot a receiver operating curve.

    :param fpr: list or array containing arrays of false positive rates for each model run.
    :param tpr: list or array containing arrays of true positive rates for each model run.
    :param OutFile: path to output file.
    
    """
    # check fpr and tpr:
    if not isinstance(fpr, np.ndarray):
        fpr = np.array(fpr)
    if not isinstance(tpr, np.ndarray):
        tpr = np.array(tpr)
    if not isinstance(OutFile, str):
        raise SystemExit('ERROR: the output filename must be a string.')
    if fpr.shape[0] == 1:
        raise SystemExit('ERROR: the false positive rate must contain more than 1 model iteration.')
    if tpr.shape[0] == 1:
        raise SystemExit('ERROR: the true positive rate must contain more than 1 model iteration.')
    if fpr.shape[0] != tpr.shape[0]:
        raise SystemExit('ERROR: fpr and tpr are of unequal lengths.')

    # setup plot:
    fig, ax = plt.subplots(nrows=1, ncols=1, sharex=False, sharey=False, figsize=(3.25, 2.75))
    plt.tight_layout(w_pad=0, h_pad=0)
    ax.get_yaxis().set_tick_params(which='major', direction='out')
    ax.get_xaxis().set_tick_params(which='major', direction='out')
    ax.get_xaxis().set_tick_params(which='minor', direction='out', length=0, width=0)
    ax.get_yaxis().set_tick_params(which='minor', direction='out', length=0, width=0)
    ax.get_xaxis().tick_bottom()
    ax.get_yaxis().tick_left()
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.xaxis.set_tick_params(width=0.5)
    ax.yaxis.set_tick_params(width=0.5)

    # plot the mean ROC curve:
    mean_fpr = np.mean(fpr, axis=0)
    mean_tpr = np.mean(tpr, axis=0)
    ax.plot(mean_fpr, mean_tpr, c='steelblue', linestyle='-', lw=1.0, label='Mean ROC')

    # plot 1 standard deviation:
    std_tpr = np.std(tpr, axis=0)
    tpr_lower = mean_tpr - std_tpr
    tpr_upper = mean_tpr + std_tpr
    del mean_tpr, std_tpr
    ax.fill_between(mean_fpr, tpr_lower, tpr_upper, alpha=1, edgecolor='silver',
                    lw=0.0, facecolor='silver', label='$\pm$ 1 St.Dev')
    del mean_fpr, tpr_lower, tpr_upper

    ax.set_xlim((0, 1))
    ax.set_ylim((0, 1))
    ax.plot(ax.get_xlim(), ax.get_ylim(),  c='k', ls=':', lw=0.5)
    ax.set_xlabel('False Positive Rate', fontsize=10)
    ax.set_ylabel('True Positive Rate', fontsize=10)

    legend = ax.legend(loc='lower right', fancybox=True, frameon=True, ncol=1, handlelength=0.75, fontsize=9)
    legend.get_frame().set_linewidth(0.5)
    legend.get_frame().set_edgecolor('k')

    plt.savefig(OutFile, format='pdf', dpi=300, bbox_inches='tight')
    plt.close()
    del legend, fig, ax


def plot_calibration_curve(y_true, y_prob, OutFile, n_bins=10):
    """
    A function to plot a calibration curve.

    Reference:
    https://scikit-learn.org/stable/auto_examples/calibration/plot_compare_calibration.html

    :param y_true: binary class labels, where 0 is absence and 1 is presence.
    :param y_prob: probability of presence scores generated by a species distribution model.
    :param OutFile: path to output file.
    :param n_bins: number of xy bins to calculate. Bin size = 1 / n_bins.
    
    """
    # check y_true and y_prob:
    if not isinstance(y_true, np.ndarray):
        y_true = np.array(y_true)
    if not isinstance(y_prob, np.ndarray):
        y_prob = np.array(y_prob)
    if not isinstance(OutFile, str):
        raise SystemExit('ERROR: the output filename must be a string.')
    if y_true.ndim != 1:
        raise SystemExit('ERROR: the true labels must be provided in a 1D array.')
    if y_prob.ndim != 1:
        raise SystemExit('ERROR: the probability of presence values must be provided in a 1D array.')
    if y_true.size != y_prob.size:
        raise SystemExit('ERROR: unequal number of binary labels and probabilities.')   

    # ensure that y_true contains binary labels (i.e. 0 or 1 values):
    y_true = y_true.astype('uint8')
    if np.min(y_true) != 0 or np.max(y_true) != 1:
        raise SystemExit('ERROR: the true labels are not binary (zero or one values).')

    from sklearn.calibration import calibration_curve
    true_probability, pred_probability = calibration_curve(y_true, y_prob, n_bins=n_bins)

    # setup plot:
    fig, ax = plt.subplots(nrows=1, ncols=1, sharex=False, sharey=False, figsize=(3.25, 2.75))
    plt.tight_layout(w_pad=0, h_pad=0)
    ax.get_yaxis().set_tick_params(which='major', direction='out')
    ax.get_xaxis().set_tick_params(which='major', direction='out')
    ax.get_xaxis().set_tick_params(which='minor', direction='out', length=0, width=0)
    ax.get_yaxis().set_tick_params(which='minor', direction='out', length=0, width=0)
    ax.get_xaxis().tick_bottom()
    ax.get_yaxis().tick_left()
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.xaxis.set_tick_params(width=0.5)
    ax.yaxis.set_tick_params(width=0.5)

    ax.plot(true_probability, pred_probability, c='steelblue', linestyle='-', lw=1.0, label='Prediction')
    del true_probability, pred_probability

    ax.set_xlim((0, 1))
    ax.set_ylim((0, 1))
    ax.plot(ax.get_xlim(), ax.get_ylim(),  c='k', ls=':', lw=0.5, label='Perfect calibration')
    ax.set_xlabel('Predicted probability', fontsize=10)
    ax.set_ylabel('True probability', fontsize=10)

    legend = ax.legend(loc='lower right', fancybox=True, frameon=True, ncol=1, handlelength=0.75, fontsize=9)
    legend.get_frame().set_linewidth(0.5)
    legend.get_frame().set_edgecolor('k')

    plt.savefig(OutFile, format='pdf', dpi=300, bbox_inches='tight')
    plt.close()
    del legend, fig, ax


def plot_continuous_responses(OutDir, x_names, x_vals, y_mean, y_std, n_std=1):
    """
    A function to plot response curves for continuous variables.

    :param OutDir: Output directory.
    :param x_names: List or 1D np.array containing the names of each environmental variable.
    :param x_vals: A 2D np.array containing values along the x axis, shape=(n_predictors, n x_values).
    :param y_mean: A 2D np.array containing the mean responses, shape=(n_predictors, n x_values).
    :param y_std: A 2D np.array containing st.deviation values, shape=(n_predictors, n x_values).
    :param n_std: float defining the number of standard deviations to display on figure (Optional, default = 1).
    
    """
    # check inputs:
    if not isinstance(OutDir, str):
        raise SystemExit('ERROR: unable to create continuous response curves. The output directory must be a string.')
    if not os.path.exists(OutDir):
        raise SystemExit('ERROR: unable to create continuous response curves. The output directory does not exist.')
    if not isinstance(x_names, np.ndarray):
        x_names = np.array(x_names)
    if x_names.ndim != 1:
        raise SystemExit('ERROR: unable to create continuous response curves. x_names must be a 1D numpy array.')
    if not isinstance(x_vals, np.ndarray):
        x_vals = np.array(x_vals)
    if x_vals.ndim != 2:
        raise SystemExit('ERROR: unable to create continuous response curves. x_vals must be a 2D numpy array.')
    if not isinstance(y_mean, np.ndarray):
        y_mean = np.array(y_mean)
    if y_mean.ndim != 2:
        raise SystemExit('ERROR: unable to create continuous response curves. y_mean must be a 2D numpy array.')
    if not isinstance(y_std, np.ndarray):
        y_std = np.array(y_std)
    if y_std.ndim != 2:
        raise SystemExit('ERROR: unable to create continuous response curves. y_std must be a 2D numpy array.')
    if not isinstance(n_std, float):
        try:
            n_std = float(n_std)
        except Exception:
            n_std = 1.0
    if n_std % 1 == 0:
        n_std = int(n_std)

    n_predictors = len(x_names)
    if x_vals.shape[0] != n_predictors or y_mean.shape[0] != n_predictors or y_std.shape[0] != n_predictors:
        raise SystemExit('ERROR: unable to create continuous response curves. An unequal number of predictors were provided.')
    if x_vals.shape[1] != y_mean.shape[1] or x_vals.shape[1] != y_std.shape[1]:
        raise SystemExit('ERROR: unable to create continuous response curves. An unequal number of elements were provided.')

    # remove invalid characters in x_names:
    x_names = [name.replace('_',' ').replace('.','').replace('-','') for name in x_names]

    OutDir = os.path.abspath(OutDir) + '/'
    rcParams.update({'font.size': 7.5})
    for i in range(n_predictors):
        # setup plot:
        fig, ax = plt.subplots(nrows=1, ncols=1, sharex=False, sharey=False, figsize=(2.8, 1.8))
        plt.tight_layout(w_pad=0, h_pad=0)
        ax.get_yaxis().set_tick_params(which='major', direction='out')
        ax.get_xaxis().set_tick_params(which='major', direction='out')
        ax.get_xaxis().set_tick_params(which='minor', direction='out', length=0, width=0)
        ax.get_yaxis().set_tick_params(which='minor', direction='out', length=0, width=0)
        ax.get_xaxis().tick_bottom()
        ax.get_yaxis().tick_left()
        ax.spines['top'].set_visible(False)
        ax.spines['right'].set_visible(False)
        ax.xaxis.set_tick_params(width=0.5)
        ax.yaxis.set_tick_params(width=0.5)

        # plot mean curve:
        ax.plot(x_vals[i], y_mean[i], c='steelblue', linestyle='-', lw=0.75, label='Mean Response')

        # plot n standard deviations:
        lower = y_mean[i] - (n_std * y_std[i])
        upper = y_mean[i] + (n_std * y_std[i])
        ax.fill_between(x_vals[i], lower, upper, alpha=1, edgecolor='silver',
                        lw=0.0, facecolor='silver', label='$\pm$ ' + str(n_std) + ' St.Dev')
        del lower, upper

        #ax.set_xlim((min(), 1))
        ax.set_ylim((0, 1))
        ax.set_xlabel(x_names[i], fontsize=9)
        ax.set_ylabel('Probability of presence', fontsize=9)

        legend = ax.legend(loc=(0.05, -0.425), fancybox=True, frameon=True, ncol=2, handlelength=0.75, fontsize=8.5)
        legend.get_frame().set_linewidth(0.5)
        legend.get_frame().set_edgecolor('k')

        plt.savefig(OutDir + x_names[i] + '.pdf', format='pdf', dpi=300, bbox_inches='tight')
        plt.close()
        del legend, fig, ax
    del n_predictors, OutDir, x_names


def plot_categorical_response(OutDir, x_name, x_labels, y_mean, y_std):
    """
    A function to create a bar plot for categorical response variable.

    :param OutDir: Output directory.
    :param x_name: The name of the environmental predictor variable.
    :param x_labels: A 1D np.array containing values/labels along the x axis, shape=(n classes, ).
    :param y_mean: A 1D np.array containing the mean responses, shape=(n classes, ).
    :param y_std: A 1D np.array containing st.deviation values, shape=(n classes, ).
    
    """
    # check inputs:
    if not isinstance(OutDir, str):
        raise SystemExit('ERROR: unable to plot categorical response. The output directory must be a string.')
    if not os.path.exists(OutDir):
        raise SystemExit('ERROR: unable to plot categorical response. The output directory does not exist.')
    if not isinstance(x_name, str):
        try:
            x_name = str(x_name)
        except Exception:
            raise SystemExit('ERROR: unable to plot categorical response. The variable name must be a string.')

    err_message = 'ERROR: unable to plot categorical response. The class labels must be a 1D np.array.'
    if not isinstance(x_labels, np.ndarray):
        try:
            x_labels = np.array(x_labels)
        except Exception:
            raise SystemExit(err_message)
    if x_labels.ndim != 1:
        raise SystemExit(err_message)

    if str(x_labels.dtype) in ['float16', 'float32', 'float64']:
        try:
            x_labels = x_labels.astype(int).astype(str)
        except Exception:
            pass

    err_message = 'ERROR: unable to plot categorical response. The mean response must be a 1D np.array.'
    if not isinstance(y_mean, np.ndarray):
        try:
            y_mean = np.array(y_mean)
        except Exception:
            raise SystemExit(err_message)
    if y_mean.ndim != 1:
            raise SystemExit(err_message)
    
    err_message = 'ERROR: unable to plot categorical response. The standard deviation must be a 1D np.array.'
    if not isinstance(y_std, np.ndarray):
        try:
            y_std = np.array(y_std)
        except Exception:
            raise SystemExit(err_message)
    if y_std.ndim != 1:
            raise SystemExit(err_message)
    del err_message

    if y_mean.size != x_labels.size or y_std.size != x_labels.size:
        raise SystemExit('ERROR: unable to plot categorical response. Input arrays contain an unequal number of elements.')

    # remove invalid characters in x_name:
    x_name = x_name.replace('_',' ').replace('.', '').replace('-','')
    OutDir = os.path.abspath(OutDir) + '/'

    # setup a plot of varying horizontal size depending on n_classes:
    if x_labels.size >= 20:
        fig, ax = plt.subplots(nrows=1, ncols=1, sharex=False, sharey=False, figsize=(6.0, 2.0))
    elif x_labels.size >= 15:
        fig, ax = plt.subplots(nrows=1, ncols=1, sharex=False, sharey=False, figsize=(5.0, 2.0))
    else:
        fig, ax = plt.subplots(nrows=1, ncols=1, sharex=False, sharey=False, figsize=(4.0, 2.0))
    plt.tight_layout(w_pad=0, h_pad=0)
    ax.get_yaxis().set_tick_params(which='major', direction='out')
    ax.get_xaxis().set_tick_params(which='major', direction='out')
    ax.get_xaxis().set_tick_params(which='minor', direction='out', length=0, width=0)
    ax.get_yaxis().set_tick_params(which='minor', direction='out', length=0, width=0)
    ax.get_xaxis().tick_bottom()
    ax.get_yaxis().tick_left()
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.xaxis.set_tick_params(width=0.5)
    ax.yaxis.set_tick_params(width=0.5)

    # define the bar positions + widths along the x axis:
    bar_width = 0.5
    xloc = np.arange(x_labels.size) - (bar_width * 0.125)

    # plot the bars:
    ax.bar(xloc, y_mean, yerr=y_std, color='steelblue', width=bar_width, lw=0.25, edgecolor='k',
           tick_label=x_labels, error_kw=dict(lw=0.35, capsize=1.25, capthick=0.25))
    del xloc, bar_width

    ax.set_ylim((0, 1))
    ax.set_xlabel(x_name, fontsize=10)
    ax.set_ylabel('Probability of presence', fontsize=10)
    ax.set_xticklabels(x_labels, rotation=35, horizontalalignment='right', verticalalignment='top', fontsize=7.5)
    del x_labels

    plt.savefig(OutDir + x_name + '.pdf', format='pdf', dpi=300, bbox_inches='tight')
    plt.close()
    del fig, ax, OutDir


def create_lek_matrices(presence_absence_data, categorical_idx, labels='Presence-Absence', NoDataVal=0, OutputHDF=False):
    """
    A function to generate matrices for producing model response curves using the profile method
    developed by Lek et al. (1995, 1996). This is a one-at-a-time method where parameter interactions
    are investigated at fixed thresholds (typically, the 0, 25, 50, 75 and 100th percentiles).

    This function was originally written for the following paper:
    Roberts et al. (2020). Sensitivity Analysis of the DART Model for Forest Mensuration with Airborne Laser Scanning.
                          Remote Sensing, 12(2), 247.

    A simplified version is used here, were only the mean response curve is calculated.
    We then calculate the st.deviation for the mean response curve across n model replicates.

    :param presence_absence_data: A np.recarray containing presence-absence data. Shape=(n_samples, n_predictors).
    :param categorical_idx: A list or np.ndarray containing boolean indicators where True == categorical variable.
                            Must be the same length as the number of predictors in presence_absence_data.
    :param labels: Column in np.recarray containing the presence/absence labels.
    :param NoDataVal: Null value for the continuous and categorical raster layers. Default is 0.
    :param OutputHDF: Option to export matrices to a HDF5 database. Default=False.

    :return: 2 lists containing arrays for varying continuous and categorical parameters. The continuous arrays
             have a shape=(Lek scale, n predictors). The categorical matrices have a shape=(n unique classes, n predictors).

    References:
    Gevrey et al. (2003). Review and comparison of methods to study the contribution of variables in
                          artificial neural network models. Ecological modelling, 160(3), 249-264.
    Franceschini et al. (2018). Cascaded neural networks improving fish species prediction accuracy:
                                the role of the biotic information. Nature Scientific reports, 8(1), 4581.
    
    """
    from random import gauss
    if not isinstance(NoDataVal, float) or not isinstance(NoDataVal, int):
        try:
            NoDataVal = float(NoDataVal)
        except Exception:
            NoDataVal = 0
    if not isinstance(categorical_idx, np.ndarray):
        categorical_idx = np.array(categorical_idx)

    # check input array is structured:
    colnames = check_recarray(presence_absence_data)

    # drop column containing the presence-absence labels:
    if labels in colnames:
        from numpy.lib.recfunctions import rec_drop_fields
        rec_array = rec_drop_fields(presence_absence_data, labels)
        colnames = np.array(rec_array.dtype.names)

    # get quantity and datatypes of predictors:
    n_predictors = colnames.size
    dTypes = [str(rec_array.dtype[i]) for i in range(n_predictors)]
    dTypes = [dtype2obj(dtype) for dtype in dTypes]

    if categorical_idx.size != n_predictors:
        raise SystemExit('ERROR: the categorical index does not match the number of environmental predictors.')

    # index each continuous variable:
    continuous_idx = np.where(categorical_idx == False)[0]
    if continuous_idx.size == 0:
        raise SystemExit('ERROR: Unable to create Lek matrices as no continuous variables were found.')

    # index each categorical variable:
    categorical_idx = np.where(categorical_idx == True)[0]

    # define the scale (percentiles/intervals along the x axis for plotting each response):
    scale = np.arange(1, 100, 1, dtype='uint8')

    # calculate values for generating response matrix for continuous variables:
    x = rec_array[colnames[continuous_idx]]
    x = np.array(x.tolist())
    x = np.ma.masked_equal(x, NoDataVal)  # mask null values.

    continuous_values = []
    for percentile in np.arange(1, 100, 1, dtype='uint8'):
        continuous_values.append(np.percentile(x, percentile, axis=0))
    continuous_values = np.array(continuous_values).T

    # simplified here: calculate only the mean of each predictor rather than using percentiles:
    ### nominal_values = []
    ### for c in [1, 25, 50, 75, 99]:
    ###    nominal_values.append(np.percentile(x, c, axis=0))
    ### nominal_values = np.array(nominal_values)
    nominal_values = np.array([np.mean(x, axis=0)])
    del x

    # get the unique and modal values for categorical variables:
    if categorical_idx.size != 0:
        categorical_values, modal_classes = [[],[]]
        for i in categorical_idx:
            x = rec_array[colnames[i]]
            x = x[x != NoDataVal]  # remove null values.
            clf, counts = np.unique(x, return_counts=True)
            categorical_values.append(clf)
            modal_classes.append(clf[np.argmax(counts)])
            del x, clf, counts
        categorical_values, modal_classes = np.array(categorical_values), np.array(modal_classes)

    # create output HDF5 database:
    if OutputHDF:
        try:
            import h5py
            hdf = h5py.File(OutputHDF, 'w')
        except Exception:
            raise SystemExit('ERROR: unable to create HDF5 database to store Lek matrices for variable response curves.')

    # create Lek matrices for each categorical variable:
    categorical_matrices = []
    if categorical_idx.size != 0:
        for idx, categorical_predictor in enumerate(categorical_values):
            x_name = colnames[categorical_idx[idx]]
            matrix = []
            for unique_clf in categorical_predictor:
                for constants in nominal_values:
                    x = np.copy(constants)
                    if idx == 0 and categorical_idx.size == 1:
                        x = np.insert(x, categorical_idx[idx], unique_clf)
                    elif idx == 0 and categorical_idx.size != 1:
                        # insert unique class:
                        x = np.insert(x, categorical_idx[idx], unique_clf)
                        # then insert modal values for all subsequent categorical variables (preserving order):
                        x = np.insert(x, categorical_idx[idx + 1:], modal_classes[idx + 1:])
                    else:
                        # insert modal values for prior categorical variables (preserving order):
                        x = np.insert(x, categorical_idx[:idx], modal_classes[:idx])
                        # then insert unique class:
                        x = np.insert(x, categorical_idx[idx], unique_clf)

                    if x.size == n_predictors:
                        matrix.append(x)
                    else:
                        raise SystemExit('ERROR: unable to create Lek matrix for generating response curves.')
                    del x
            matrix = np.array(matrix).T.round(decimals=10)
            categorical_matrices.append(matrix.T)
            if OutputHDF:
                # export matrix to HDF5 file:
                matrix = np.core.records.fromarrays(matrix, names=colnames.tolist(), formats=','.join(dTypes))
                hdf.create_dataset('/Lek_Matrices/Categorical/' + x_name, data=matrix, compression='gzip', compression_opts=1)
            del matrix, categorical_predictor, idx, x_name
        del categorical_values

    # create Lek matrices for each continuous variable:
    continuous_matrices = []
    for idx, scale in enumerate(continuous_values):
        x_name = colnames[continuous_idx[idx]]
        matrix = []
        for element in scale:
            for constants in nominal_values:
                # copy the constants:
                x = np.copy(constants)

                # insert element from the predictor variable to be varied:
                x[continuous_idx[idx]] = element

                if categorical_idx.size != 0:
                    # insert modal classes of categorical variables:
                    for i, clf in enumerate(modal_classes):
                        x = np.insert(x, categorical_idx[i], clf)

                if x.size == n_predictors:
                    matrix.append(x)
                del x

        matrix = np.array(matrix).T.round(decimals=10)
        continuous_matrices.append(matrix.T)
        if OutputHDF:
            # export matrix to HDF5 file:
            matrix = np.core.records.fromarrays(matrix, names=colnames.tolist(), formats=','.join(dTypes))
            hdf.create_dataset('/Lek_Matrices/Continuous/' + x_name, data=matrix, compression='gzip', compression_opts=1)
        del matrix, x_name, scale
    del continuous_values, categorical_idx, continuous_idx, nominal_values, dTypes, n_predictors, colnames, rec_array
    if OutputHDF:
        hdf.close()
    return [continuous_matrices, categorical_matrices]


def calculate_mean_responses(responses, replicates):
    """
    An utility function to calculate the mean and st.deviation for plotting continuous response curves.

    Required Parameters:
    :param responses: A np.array containing probability of presence values from estimator.predict_proba.
                      For continuous variables: shape=(n replicates, n predictors, n x_values)
                      For categorical variables: shape=(n replicates, n unique classes).
    :param replicates: Integer defining the number of model replicates. This is used as a sanity check to ensure
                       that the length of the first dimension equals the number of replicates.
    :return: 2 numpy arrays containing mean responses and their st.deviation, shape=(n replicates, n x_values).
    
    """
    if not isinstance(replicates, int):
        try:
            replicates = int(replicates)
        except Exception:
            raise SystemExit('ERROR: Unable to calculate mean responses. The number of replicates must be an integer.')
   
    if not isinstance(responses, np.ndarray):
        try:
            responses = np.array(responses)
        except Exception:
            raise SystemExit('ERROR: Unable to calculate mean responses. Input is not a numpy array.')
    if responses.shape[0] != replicates:
        raise SystemExit('ERROR: Unable to calculate mean responses. Shape of input array does not match the number of replicates.')

    mean = np.mean(responses, axis=0)
    stdev = np.std(responses, axis=0)
    return [mean, stdev]


def apply_decision_tree_estimator(params, presence_absence_data, estimator=None, validation='split-sample', test_fraction=0.15,
                                  replicates=10, gdalformat='GTiff', clf_labels='Presence-Absence', Overwrite=True):
    """
    A function to model species distributions using scikit-learn decision tree classifiers supporting the methods: feature_importances_ and predict_proba.
    Use with: DecisionTreeClassifier, GradientBoostingClassifier, RandomForestClassifier, ExtraTreesClassifier or AdaBoostClassifier.

    :param params: A python dictionary containing the simulation parameters.
    :param presence_absence_data: np.recarray containing presence-absence training data.
    :param estimator: An sklearn decision tree classifier with methods: feature_importances_ and predict_proba.
                      If None, uses sklearn.tree.DecisionTreeClassifier by default.
    :param validation: 'bootstrap' or 'split-sample' resampling methods for cross-validation are currently supported.
    :param test_fraction: Float defining the proportion of presence-absence data to set aside for validation.
                          Only used in split-sample validation. Default = 0.15 (i.e. 15%).
    :param replicates: Number of model iterations to perform (default=10).
    :param gdalformat: Format of output images. Valid options "GTiff" or "KEA".
    :param clf_labels: Name of column in presence_absence_data delineating the binary class labels (presence=1, absence=0).
    :param Overwrite: Boolean option to overwrite the output directory if it already exists.
    
    """
    from sklearn.metrics import accuracy_score

    # check inputs:
    if not isinstance(params, dict):
        raise SystemExit('ERROR: please provide a Python dictionary containing the simulation parameters.')

    x_names = check_recarray(presence_absence_data)

    if estimator == None:
        print('WARNING: No estimator specified. Defaulting to a decision tree classifier.')
        from sklearn.tree import DecisionTreeClassifier as DT
        estimator = DT(min_samples_split=4, min_samples_leaf=2, max_features=None, max_depth=6)
    else:
        # check that a valid estimator has been passed:
        name = str(estimator).split('(')[0]
        if name not in ['DecisionTreeClassifier', 'GradientBoostingClassifier', 'RandomForestClassifier', 'ExtraTreesClassifier', 'AdaBoostClassifier']:
            print('ERROR: the chosen scikit-learn estimator is not supported in this function.')
            raise SystemExit('Please select from: DecisionTreeClassifier, GradientBoostingClassifier, RandomForestClassifier, ExtraTreesClassifier or AdaBoostClassifier.')

    if validation not in ['bootstrap', 'split-sample']:
        print('WARNING: Unrecognised validation option. Defaulting to split-sample cross-validation.')

    if not isinstance(test_fraction, float):
        try:
            test_fraction = float(test_fraction)
        except Exception:
            test_fraction = 0.1
            print('WARNING: Test fraction must be a float between 0 and 1. Using a default value of 0.1.')
            pass

    if test_fraction <= 0 or test_fraction >= 1:
        test_fraction = 0.1
        print('WARNING: Test fraction must be a float between 0 and 1. Using a default value of 0.1.')

    if not isinstance(replicates, int):
        try:
            replicates = int(replicates)
        except Exception:
            replicates = 10
            print('WARNING: The number of replicates must be an integer >= 2. Using a default value of 10.')

    if replicates < 2:
        replicates = 10
        print('WARNING: The number of replicates must be an integer >= 2. Using a default value of 10.')

    if not isinstance(clf_labels, str):
        clf_labels = 'Presence-Absence'
        print('WARNING: clf_labels not recognised. Defaulting to "Presence-Absence".')

    if not isinstance(Overwrite, bool):
        Overwrite = True

    print('Initialising simulation...')
    # verify the simulation parameters in dictionary:
    params = check_simulation(params)

    # find images with incorrect projection and/or pixel resolutions:
    invalid_images = find_invalid_images(params['Images'], params['Valid Mask'], params['Null Value'])

    # reproject any invalid images:
    if invalid_images:
        resampleImages2Grid(invalid_images, params['Valid Mask'])
    del invalid_images

    # create the output directory:
    create_simulation_directory(params['Output Directory'], Overwrite)

    # (optional) export a correlation matrix and VIF scores:
    export_correlation_matrix(params, presence_absence_data)
    export_VIF(params, presence_absence_data)

    # define the output images:
    if not isinstance(gdalformat, str) or gdalformat not in ['GTiff', 'KEA']:
        print('WARNING: GDAL image output format must be "GTiff" or "KEA". Defaulting to "GTiff".')
        gdalformat = 'GTiff'

    if gdalformat == 'GTiff':
        OutputImages = [params['Output Directory'] + '/Run_' + str(i + 1).zfill(3) + '.tif' for i in range(replicates)]
    else:
        OutputImages = [params['Output Directory'] + '/Run_' + str(i + 1).zfill(3) + '.kea' for i in range(replicates)]

    # generate matrices for predicting and plotting response curves:
    continuous_matrices, categorical_matrices = create_lek_matrices(presence_absence_data, params['Categorical'])

    # perform simulations/replicates:
    train_accuracy, importance_scores, y_true, y_pred, y_prob = [list() for i in range(5)]
    false_positive_rate, true_positive_rate = [[], []]
    continuous_responses, categorical_responses = [[], []]

    print('Performing', replicates, 'replicates...')
    for i in range(replicates):
        # split the training and test data:
        if validation == 'split-sample':
            train, test = train_test_split(presence_absence_data, test_fraction)
        elif validation == 'bootstrap':
            train, test = bootstrap_records(presence_absence_data)

        # split data into predictors and binary response:
        x_test, y_test = split_xy(test)
        x_train, y_train = split_xy(train)
        y_true.append(y_test)
        del test, train, y_test

        # fit estimator:
        estimator.fit(x_train, y_train)
        importance_scores.append(estimator.feature_importances_)
        train_accuracy.append(accuracy_score(estimator.predict(x_train), y_train))
        del x_train, y_train

        # generate predictions on unseen data:
        y_pred.append(estimator.predict(x_test))

        # get probability of absence-presence:
        prob = estimator.predict_proba(x_test)  # returns tuples of absence_prob, presence_prob
        del x_test

        # get probability estimates of the positive class (i.e. presence):
        prob = prob[:,-1]
        y_prob.append(prob)

        # calculate false positive & true positive rates for the ROC curve:
        fpr, tpr, thresholds = roc_curve(y_true[i], prob)
        del thresholds, prob
        fpr, tpr = process_roc_stats(fpr, tpr)
        false_positive_rate.append(fpr)
        true_positive_rate.append(tpr)
        del fpr, tpr

        # generate predictions for response curves:
        if continuous_matrices:
            response_probs = []
            for m in continuous_matrices:
                response_probs.append(estimator.predict_proba(m)[:,-1])
            continuous_responses.append(response_probs)
            del response_probs

        if categorical_matrices:
            response_probs = []
            for m in categorical_matrices:
                response_probs.append(estimator.predict_proba(m)[:,-1])
            categorical_responses.append(response_probs)
            del response_probs

        # export predictions to an image:
        export_presence_image(params, OutputImages[i], estimator, gdalformat=gdalformat)
        ProgressBar(replicates, i + 1)
    del estimator

    print('Generating accuracy metrics...')
    y_true = np.concatenate(y_true).astype('uint8')
    y_pred = np.concatenate(y_pred).astype('uint8')
    y_prob = np.concatenate(y_prob).astype('float64')

    # export variable importance scores:
    importance_scores = np.mean(np.array(importance_scores), axis=0)
    scores2txt(params['Output Directory'] + '/Variable_Importance_Scores.csv', params['Environmental Predictors'], importance_scores)
    del importance_scores

    # plot the receiver operating curve:
    plot_roc_curve(false_positive_rate, true_positive_rate, params['Output Directory'] + '/ROC_Curve.pdf')
    del false_positive_rate, true_positive_rate

    # export training out of bag accuracy score:
    train_accuracy = np.mean(np.array(train_accuracy)).round(decimals=6)
    scores2txt(params['Output Directory'] + '/Accuracy_Stats.csv', ['Accuracy (Training)'], [train_accuracy])
    del train_accuracy

    # plot a calibration curve:
    plot_calibration_curve(y_true, y_prob, params['Output Directory'] + '/Calibration_Curve.pdf')

    # calculate accuracy metrics on probability of presence:
    names, prob_scores = probability_accuracy_scores(y_true, y_prob)
    scores2txt(params['Output Directory'] + '/Accuracy_Stats.csv', names, prob_scores)
    del names, prob_scores, y_prob

    # calculate accuracy metrics on binary classification:
    names, scores = binary_accuracy_scores(y_true, y_pred)
    scores2txt(params['Output Directory'] + '/Accuracy_Stats.csv', names, scores)
    del y_true, y_pred, names, scores

    # generate response curves:
    print('Exporting response curves...')
    if continuous_responses:
        x_vals = np.array([continuous_matrices[i][:,i] for i in range(len(continuous_matrices))])
        del continuous_matrices
        y_mean, y_std = calculate_mean_responses(np.array(continuous_responses), replicates)
        del continuous_responses
        x_names = np.array(params['Environmental Predictors'])[np.where(np.array(params['Categorical']) == False)[0]]
        plot_continuous_responses(params['Output Directory'], x_names, x_vals, y_mean, y_std)
        del x_names, x_vals, y_mean, y_std

    if categorical_responses:
        idx = np.where(np.array(params['Categorical']) == True)[0]
        x_names = np.array(params['Environmental Predictors'])[idx]
        clf_labels = np.array([categorical_matrices[i][:, idx[i]] for i in range(len(categorical_matrices))])
        del categorical_matrices, idx

        # plot each categorical responses, shape=(n_replicates, n_categorical vars)
        for i, name in enumerate(x_names):
            y = []
            for m in categorical_responses:
                y.append(m[i])
            y_mean, y_std = calculate_mean_responses(y, replicates)
            plot_categorical_response(params['Output Directory'], name, clf_labels[i], y_mean, y_std)
            del y, y_mean, y_std
        del x_names, clf_labels

    # generate mean, st.dev and cv images:
    export_mean_probability(params, OutputImages, gdalformat)
    del OutputImages
    print('Simulation complete.')


def apply_meta_estimator(params, presence_absence_data, estimator=None, validation='split-sample', test_fraction=0.15,
                         replicates=10, max_depth=3, gdalformat='GTiff', clf_labels='Presence-Absence', Overwrite=True):
    """
    A function to model species distributions using scikit-learn classifiers supporting the method predict_proba.
    Works with: LogisticRegression (aka maxent) and MLPClassifier (perhaps others?). Is very slow with SVC.

    To avoid encoding each categorical variable, this function uses gradient boosted decision trees to perform a feature transform.
    The leaves within the fitted decision tree are then encoded into sparse binary values and a meta-estimator (e.g. logistic regression
    / maxent) is then fitted onto the transformed features/variables. This often leads to better results than can be achieved by
    fitting the meta-estimator directly. This method is related to manifold learning & stacked regression (where a model is trained
    from the predictions of previous estimators).

    References:
    https://scikit-learn.org/stable/auto_examples/ensemble/plot_feature_transformation.html
    https://www.quora.com/Why-do-people-use-gradient-boosted-decision-trees-to-do-feature-transform
    Breiman, L. (1996). Stacked regressions. Machine learning, 24(1), 49-64.

    :param params: A python dictionary containing the simulation parameters.
    :param presence_absence_data: np.recarray containing presence-absence training data.
    :param estimator: An sklearn classifier supporting a predict_proba method.
                      If None, uses sklearn.linear_model.LogisticRegression (Maxent) by default.
    :param validation: 'bootstrap' or 'split-sample' resampling methods for cross-validation are currently supported.
    :param test_fraction: Float defining the proportion of presence-absence data to set aside for validation.
                          Only used in split-sample validation. Default = 0.15 (i.e. 15%).
    :param replicates: Number of model iterations to perform (default=10).
    :param max_depth: Maximum number of nodes in the decision tree used to transform the presence absence data.
                      When max_depth is high, a very detailed transform is undertaken. Conversely, setting max_depth
                      to a low value (e.g. 3) limits overfitting.
    :param gdalformat: Format of output images. Valid options "GTiff" or "KEA".
    :param clf_labels: Name of column in presence_absence_data delineating the binary class labels (presence=1, absence=0).
    :param Overwrite: Boolean option to overwrite the output directory if it already exists.
    
    """
    from sklearn.metrics import accuracy_score

    # check inputs:
    if not isinstance(params, dict):
        raise SystemExit('ERROR: please provide a Python dictionary containing the simulation parameters.')

    x_names = check_recarray(presence_absence_data)

    if estimator == None:
        print('WARNING: No estimator specified. Defaulting to MaxEnt.')
        from sklearn.linear_model import LogisticRegression as MaxEnt
        # C = Positive float defining regularisation (smaller values specify stronger regularisation)
        estimator = MaxEnt(solver='liblinear', max_iter=500, penalty='l2', tol=0.00001, C=0.1,
                           fit_intercept=True, intercept_scaling=1, n_jobs=4)
    else:
        # check that a valid estimator has been passed:
        name = str(estimator).split('(')[0]
        if name not in ['LogisticRegression', 'MLPClassifier']:
            print('ERROR: the chosen scikit-learn estimator is not supported in this function.')
            raise SystemExit('Please select from: LogisticRegression (aka maxent) or MLPClassifier')

    if validation not in ['bootstrap', 'split-sample']:
        print('WARNING: Unrecognised validation option. Defaulting to split-sample cross-validation.')

    if not isinstance(test_fraction, float):
        try:
            test_fraction = float(test_fraction)
        except Exception:
            test_fraction = 0.1
            print('WARNING: Test fraction must be a float between 0 and 1. Using a default value of 0.1.')
            pass

    if test_fraction <= 0 or test_fraction >= 1:
        test_fraction = 0.1
        print('WARNING: Test fraction must be a float between 0 and 1. Using a default value of 0.1.')

    if not isinstance(replicates, int):
        try:
            replicates = int(replicates)
        except Exception:
            replicates = 10
            print('WARNING: The number of replicates must be an integer >= 2. Using a default value of 10.')

    if replicates < 2:
        replicates = 10
        print('WARNING: The number of replicates must be an integer >= 2. Using a default value of 10.')

    if not isinstance(clf_labels, str):
        clf_labels = 'Presence-Absence'
        print('WARNING: clf_labels not recognised. Defaulting to "Presence-Absence".')

    if not isinstance(Overwrite, bool):
        Overwrite = True

    try:
        from sklearn.ensemble import GradientBoostingClassifier as GBT
        from sklearn.preprocessing import OneHotEncoder
        transformer = GBT(learning_rate=0.01, n_estimators=25, min_samples_split=2, min_samples_leaf=1,
                          max_depth=max_depth, max_features=None)
        encoder = OneHotEncoder()
    except Exception:
        raise SystemExit('ERROR: unable to import estimator from scikit-learn.')

    print('Initialising simulation...')
    # verify the simulation parameters in dictionary:
    params = check_simulation(params)

    # find images with incorrect projection and/or pixel resolutions:
    invalid_images = find_invalid_images(params['Images'], params['Valid Mask'], params['Null Value'])

    # reproject any invalid images:
    if invalid_images:
        resampleImages2Grid(invalid_images, params['Valid Mask'])
    del invalid_images

    # create the output directory:
    create_simulation_directory(params['Output Directory'], Overwrite)

    # (optional) export a correlation matrix and VIF scores:
    export_correlation_matrix(params, presence_absence_data)
    export_VIF(params, presence_absence_data)

    # define the output images:
    if not isinstance(gdalformat, str) or gdalformat not in ['GTiff', 'KEA']:
        print('WARNING: GDAL image output format must be "GTiff" or "KEA". Defaulting to "GTiff".')
        gdalformat = 'GTiff'

    if gdalformat == 'GTiff':
        OutputImages = [params['Output Directory'] + '/Run_' + str(i + 1).zfill(3) + '.tif' for i in range(replicates)]
    else:
        OutputImages = [params['Output Directory'] + '/Run_' + str(i + 1).zfill(3) + '.kea' for i in range(replicates)]

    # generate matrices for predicting and plotting response curves:
    continuous_matrices, categorical_matrices = create_lek_matrices(presence_absence_data, params['Categorical'])

    # perform simulations/replicates:
    train_accuracy, importance_scores, y_true, y_pred, y_prob = [list() for i in range(5)]
    false_positive_rate, true_positive_rate = [[], []]
    continuous_responses, categorical_responses = [[], []]

    print('Performing', replicates, 'replicates...')
    for i in range(replicates):
        # split the training and test data:
        if validation == 'split-sample':
            train, test = train_test_split(presence_absence_data, test_fraction)
        elif validation == 'bootstrap':
            train, test = bootstrap_records(presence_absence_data)

        # split data into predictors and response:
        x_test, y_test = split_xy(test)
        x_train, y_train = split_xy(train)
        y_true.append(y_test)
        del test, train, y_test

        # Perform supervised transformation of predictors into a higher dimensional space,
        # then encode features into sparse binary values,
        # the fit the fit meta-estimator.
        transformer.fit(x_train, y_train)
        x2 = transformer.apply(x_train)[:, :, 0]  # shape=(n_samples, n_estimators in transformer).
        del x_train
        x2 = encoder.fit_transform(x2)  # convert x2 into sparse binary encode.
        estimator.fit(x2, y_train)  # fit Maxent estimator.
        importance_scores.append(transformer.feature_importances_)
        train_accuracy.append(accuracy_score(estimator.predict(x2), y_train))
        del x2, y_train

        # generate predictions on unseen data:
        x_test = transformer.apply(x_test)[:, :, 0]
        x_test = encoder.transform(x_test)
        y_pred.append(estimator.predict(x_test))

        # get probability of absence-presence:
        prob = estimator.predict_proba(x_test)  # returns tuples of absence_prob, presence_prob
        del x_test

        # get probability estimates of the positive class (i.e. presence):
        prob = prob[:,-1]
        y_prob.append(prob)

        # calculate false positive & true positive rates for the ROC curve:
        fpr, tpr, thresholds = roc_curve(y_true[i], prob)
        del thresholds, prob
        fpr, tpr = process_roc_stats(fpr, tpr)
        false_positive_rate.append(fpr)
        true_positive_rate.append(tpr)
        del fpr, tpr

        # generate predictions for response curves:
        if continuous_matrices:
            response_probs = []
            for m in continuous_matrices:
                m = transformer.apply(m)[:, :, 0]
                m = encoder.transform(m)
                response_probs.append(estimator.predict_proba(m)[:,-1])
                del m
            continuous_responses.append(response_probs)
            del response_probs

        if categorical_matrices:
            response_probs = []
            for m in categorical_matrices:
                m = transformer.apply(m)[:, :, 0]
                m = encoder.transform(m)
                response_probs.append(estimator.predict_proba(m)[:,-1])
                del m
            categorical_responses.append(response_probs)
            del response_probs

        # export predictions to an image:
        export_presence_image(params, OutputImages[i], estimator, transformer, encoder, gdalformat)
        ProgressBar(replicates, i + 1)
    del transformer, encoder, estimator

    print('Generating accuracy metrics...')
    y_true = np.concatenate(y_true).astype('uint8')
    y_pred = np.concatenate(y_pred).astype('uint8')
    y_prob = np.concatenate(y_prob).astype('float64')

    # export variable importance scores:
    importance_scores = np.mean(np.array(importance_scores), axis=0)
    scores2txt(params['Output Directory'] + '/Variable_Importance_Scores.csv', params['Environmental Predictors'], importance_scores)
    del importance_scores

    # plot the receiver operating curve:
    plot_roc_curve(false_positive_rate, true_positive_rate, params['Output Directory'] + '/ROC_Curve.pdf')
    del false_positive_rate, true_positive_rate

    # export training out of bag accuracy score:
    train_accuracy = np.mean(np.array(train_accuracy)).round(decimals=6)
    scores2txt(params['Output Directory'] + '/Accuracy_Stats.csv', ['Accuracy (Training)'], [train_accuracy])
    del train_accuracy

    # plot a calibration curve:
    plot_calibration_curve(y_true, y_prob, params['Output Directory'] + '/Calibration_Curve.pdf')

    # calculate accuracy metrics on probability of presence:
    names, prob_scores = probability_accuracy_scores(y_true, y_prob)
    scores2txt(params['Output Directory'] + '/Accuracy_Stats.csv', names, prob_scores)
    del names, prob_scores, y_prob

    # calculate accuracy metrics on binary classification:
    names, scores = binary_accuracy_scores(y_true, y_pred)
    scores2txt(params['Output Directory'] + '/Accuracy_Stats.csv', names, scores)
    del y_true, y_pred, names, scores

    # generate response curves:
    print('Exporting response curves...')
    if continuous_responses:
        x_vals = np.array([continuous_matrices[i][:,i] for i in range(len(continuous_matrices))])
        del continuous_matrices
        y_mean, y_std = calculate_mean_responses(np.array(continuous_responses), replicates)
        del continuous_responses
        x_names = np.array(params['Environmental Predictors'])[np.where(np.array(params['Categorical']) == False)[0]]
        plot_continuous_responses(params['Output Directory'], x_names, x_vals, y_mean, y_std)
        del x_names, x_vals, y_mean, y_std

    if categorical_responses:
        idx = np.where(np.array(params['Categorical']) == True)[0]
        x_names = np.array(params['Environmental Predictors'])[idx]
        clf_labels = np.array([categorical_matrices[i][:, idx[i]] for i in range(len(categorical_matrices))])
        del categorical_matrices, idx

        # plot each categorical responses, shape=(n_replicates, n_categorical vars)
        for i, name in enumerate(x_names):
            y = []
            for m in categorical_responses:
                y.append(m[i])
            y_mean, y_std = calculate_mean_responses(y, replicates)
            plot_categorical_response(params['Output Directory'], name, clf_labels[i], y_mean, y_std)
            del y, y_mean, y_std
        del x_names, clf_labels

    # generate mean, st.dev and cv images:
    export_mean_probability(params, OutputImages, gdalformat)
    del OutputImages
    print('Simulation complete.')

