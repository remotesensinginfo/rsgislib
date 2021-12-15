#!/usr/bin/env python
"""
The Raster GIS module contains functions for attributing and manipulating raster attribute tables.
"""

# import the C++ extension into this level
from ._rastergis import *

import osgeo.gdal as gdal

import numpy

haveHDF5 = True
try:
    import h5py
except ImportError as h5Err:
    haveHDF5 = False


class BandAttStats:
    """This is passed to the populate_rat_with_stats function"""

    def __init__(
        self,
        band,
        min_field=None,
        max_field=None,
        sum_field=None,
        std_dev_field=None,
        mean_field=None,
    ):
        self.band = band
        self.min_field = min_field
        self.max_field = max_field
        self.sum_field = sum_field
        self.mean_field = mean_field
        self.std_dev_field = std_dev_field


class FieldAttStats:
    """This is passed to the calcRelDiffNeighStats function"""

    def __init__(
        self,
        field,
        min_field=None,
        max_field=None,
        sum_field=None,
        std_dev_field=None,
        mean_field=None,
    ):
        self.field = field
        self.min_field = min_field
        self.max_field = max_field
        self.sum_field = sum_field
        self.mean_field = mean_field
        self.std_dev_field = std_dev_field


class BandAttPercentiles:
    """This is passed to the populateRATWithPercentiles function"""

    def __init__(self, percentile, field_name):
        self.percentile = percentile
        self.field_name = field_name


class ShapeIndex:
    """This is passed to the calcShapeIndices function"""

    def __init__(self, col_name, idx, col_idx=0):
        self.col_name = col_name
        self.col_idx = col_idx
        self.idx = idx


def export_cols_to_gdal_img(
    clumps_img, output_img, gdalformat, datatype, fields, rat_band=1, tmp_dir=None
):
    """
    Exports columns of the raster attribute table as bands in a GDAL image.
    Utility function, exports each column individually then stacks them.

    :param clumps_img: is a string containing the name of the input image file with RAT
    :param output_img: is a string containing the name of the output gdal file
    :param gdalformat: is a string containing the GDAL format for the output
                       file - eg 'KEA'
    :param datatype: is an int containing one of the values from rsgislib.TYPE_*
    :param field: is a list of strings, providing the names of the column to be exported
    :param rat_band: is an optional (default = 1) integer parameter specifying the image
                    band to which the RAT is associated.

    Example:

    .. code:: python

       clumps='./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea'
       outimage='./TestOutputs/RasterGIS/injune_p142_casi_rgb_export.kea'
       gdalformat = 'KEA'
       datatype = rsgislib.TYPE_32FLOAT
       fields = ['RedAvg','GreenAvg','BlueAvg']
       rastergis.export_cols_to_gdal_image(clumps, outimage, gdalformat,
                                           datatype, fields)

    """
    import os
    import rsgislib.tools.filetools
    from rsgislib import imageutils

    if tmp_dir is None:
        tmp_dir = os.path.split(output_img)[0]

    outExt = os.path.splitext(output_img)[-1]
    tempFileList = []

    # Export each field
    for field in fields:
        print("Exporting: " + field)
        outTempFile = os.path.join(tmp_dir, field + outExt)
        export_col_to_gdal_img(
            clumps_img, outTempFile, gdalformat, datatype, field, rat_band
        )
        tempFileList.append(outTempFile)

    # Stack Bands
    print("Stacking Bands")
    imageutils.stack_img_bands(
        tempFileList, fields, output_img, None, 0, gdalformat, datatype
    )

    # Remove temp files
    print("Removing temp files")
    for tempFile in tempFileList:
        rsgislib.tools.filetools.delete_file_with_basename(tempFile)


def get_rat_length(clumps_img, rat_band=1):
    """
    A function which returns the length (i.e., number of rows) within the RAT.

    :param clumps_img: path to the image file with the RAT
    :param rat_band: the band within the image file for which the RAT is to read.
    :returns: an int with the number of rows.

    """
    # Open input image file
    clumps_img_ds = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    if clumps_img_ds is None:
        raise Exception("Could not open the inputted clumps image.")

    clumps_img_band = clumps_img_ds.GetRasterBand(rat_band)
    if clumps_img_band is None:
        raise Exception("Could not open the inputted clumps image band.")

    clumps_img_rat = clumps_img_band.GetDefaultRAT()
    if clumps_img_rat is None:
        raise Exception("Could not open the inputted clumps image band RAT.")

    nrows = clumps_img_rat.GetRowCount()

    clumps_img_ds = None
    return nrows


def get_rat_columns(clumps_img, rat_band=1):
    """
    A function which returns a list of column names within the RAT.

    :param clumps_img: path to the image file with the RAT
    :param rat_band: the band within the image file for which the RAT is to read.
    :returns: list of column names.

    """
    # Open input image file
    clumps_img_ds = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    if clumps_img_ds is None:
        raise Exception("Could not open the inputted clumps image.")

    clumps_img_band = clumps_img_ds.GetRasterBand(rat_band)
    if clumps_img_band is None:
        raise Exception("Could not open the inputted clumps image band.")

    clumps_img_rat = clumps_img_band.GetDefaultRAT()
    if clumps_img_rat is None:
        raise Exception("Could not open the inputted clumps image band RAT.")

    ncols = clumps_img_rat.GetColumnCount()
    col_names = []
    for col_idx in range(ncols):
        col_names.append(clumps_img_rat.GetNameOfCol(col_idx))

    clumps_img_ds = None
    return col_names


def get_rat_columns_info(clumps_img, rat_band=1):
    """
    A function which returns a dictionary of column names with type (GFT_Integer,
    GFT_Real, GFT_String) and usage (e.g., GFU_Generic, GFU_PixelCount,
    GFU_Name, etc.) within the RAT.

    :param clumps_img: path to the image file with the RAT
    :param rat_band: the band within the image file for which the RAT is to read.
    :returns: dict of column information.

    """
    # Open input image file
    clumps_img_ds = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    if clumps_img_ds is None:
        raise Exception("Could not open the inputted clumps image.")

    clumps_img_band = clumps_img_ds.GetRasterBand(rat_band)
    if clumps_img_band is None:
        raise Exception("Could not open the inputted clumps image band.")

    clumps_img_rat = clumps_img_band.GetDefaultRAT()
    if clumps_img_rat is None:
        raise Exception("Could not open the inputted clumps image band RAT.")

    ncols = clumps_img_rat.GetColumnCount()
    col_info = dict()
    for col_idx in range(ncols):
        col_name = clumps_img_rat.GetNameOfCol(col_idx)
        col_type = clumps_img_rat.GetTypeOfCol(col_idx)
        col_usage = clumps_img_rat.GetUsageOfCol(col_idx)
        col_info[col_name] = dict()
        col_info[col_name]["type"] = col_type
        col_info[col_name]["usage"] = col_usage

    clumps_img_ds = None
    return col_info



def set_class_names_colours(clumps_img, class_names_col, class_info_dict):
    """
    A function to define a class names column and define the class colours.

    classInfoDict = dict()
    classInfoDict[1] = {'classname':'Forest', 'red':0, 'green':255, 'blue':0}
    classInfoDict[2] = {'classname':'Water', 'red':0, 'green':0, 'blue':255}

    :param clumps_img: Input clumps image - expecting a classification
                      (rather than segmentation) where the number is the pixel value.
    :param class_names_col: The output column for the class names.
    :param class_info_dict: a dict where the key is the pixel value for the class.
    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this function "
            "could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this function "
            "could not be imported\n\t" + riosRatErr
        )

    n_rows = rsgislib.rastergis.get_rat_length(clumps_img)
    col_names = rsgislib.rastergis.get_rat_columns(clumps_img)

    red_avail = False
    green_avail = False
    blue_avail = False
    if "Red" in col_names:
        red_avail = True
    if "Green" in col_names:
        green_avail = True
    if "Blue" in col_names:
        blue_avail = True

    class_names_col_avail = False
    if class_names_col in col_names:
        class_names_col_avail = True

    ratDataset = gdal.Open(clumps_img, gdal.GA_Update)

    if red_avail:
        red_arr = rat.readColumn(ratDataset, "Red")
    else:
        red_arr = numpy.zeros(n_rows, dtype=int)

    if green_avail:
        green_arr = rat.readColumn(ratDataset, "Green")
    else:
        green_arr = numpy.zeros(n_rows, dtype=int)

    if blue_avail:
        blue_arr = rat.readColumn(ratDataset, "Blue")
    else:
        blue_arr = numpy.zeros(n_rows, dtype=int)

    if class_names_col_avail:
        class_names_arr = rat.readColumn(ratDataset, class_names_col)
    else:
        class_names_arr = numpy.zeros(n_rows, dtype=numpy.dtype("a255"))

    for class_key in class_info_dict:
        if (class_key >= 0) and (class_key < n_rows):
            class_names_arr[class_key] = class_info_dict[class_key]["classname"]
            red_arr[class_key] = class_info_dict[class_key]["red"]
            green_arr[class_key] = class_info_dict[class_key]["green"]
            blue_arr[class_key] = class_info_dict[class_key]["blue"]
        else:
            print(
                "Class key ({}) was not within the number of rows in the RAT.".format(
                    class_key
                ),
                file=sys.stderr,
            )

    rat.writeColumn(ratDataset, class_names_col, class_names_arr)
    rat.writeColumn(ratDataset, "Red", red_arr)
    rat.writeColumn(ratDataset, "Green", green_arr)
    rat.writeColumn(ratDataset, "Blue", blue_arr)

    ratDataset = None



def get_column_data(clumps_img, col_name):
    """
    A function to read a column of data from a RAT.

    :param clumps_img: Input clumps image
    :param col_name: Name of the column to be read.

    :return: numpy array with values from the clumpsImg
    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this function "
            "could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this function "
            "could not be imported\n\t" + riosRatErr
        )

    col_names = rsgislib.rastergis.get_rat_columns(clumps_img)

    if col_name not in col_names:
        raise Exception("Column specified is not within the RAT.")

    ratDataset = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    if ratDataset is None:
        raise Exception("The input image could not be opened.")

    col_data = rat.readColumn(ratDataset, col_name)
    ratDataset = None
    return col_data



def read_rat_neighbours(clumps_img, start_row=None, end_row=None, rat_band=1):
    """
    A function which returns a list of clumps neighbours from a KEA RAT. Note, the
    neighbours are popualted using the function rsgislib.rastergis.findNeighbours.
    By default the whole datasets of neightbours is read to memory but the start_row
    and end_row variables can be used to read a subset of the RAT.

    :param clumps_img: path to the image file with the RAT
    :param start_row: the row within the RAT to start reading, if None will start
                      at 0 (Default: None).
    :param end_row: the row within the RAT to end reading, if None will end at n_rows
                    within the RAT. (Default: None)
    :param rat_band: the band within the image file for which the RAT is to read.
    :returns: list of lists with neighbour indexes.
    """
    if not haveHDF5:
        raise Exception("Need the h5py library for this function")

    # Check that 'NumNeighbours' column exists
    rat_columns = get_rat_columns(clumps_img, rat_band)
    if "NumNeighbours" not in rat_columns:
        raise Exception(
            "Clumps image RAT does not contain 'NumNeighbours' "
            "column - have you populated neightbours?"
        )

    n_rows = get_rat_length(clumps_img)

    if start_row is None:
        start_row = 0

    if end_row is None:
        end_row = n_rows

    clumps_h5_file = h5py.File(clumps_img)
    neighbours_path = "BAND{}/ATT/NEIGHBOURS/NEIGHBOURS".format(rat_band)
    neighbours = clumps_h5_file[neighbours_path]
    neighbours_data = neighbours[start_row:end_row]
    clumps_h5_file = None
    return neighbours_data


def check_string_col_valid(
    clumps_img,
    str_col,
    rm_punc=False,
    rm_spaces=False,
    rm_non_ascii=False,
    rm_dashs=False,
):
    """
    A function which checks a string column to ensure nothing is invalid.

    :param clumps_img: input clumps image.
    :param str_col: the column to check
    :param rm_punc: If True removes punctuation from column name other
                    than dashs and underscores.
    :param rm_spaces: If True removes spaces from the column name, replacing
                      them with underscores.
    :param rm_non_ascii: If True removes characters which are not in the
                         ascii range of characters.
    :param rm_dashs: If True then dashs are removed from the column name.

    """
    import numpy
    import rsgislib.tools.utils
    from rios import ratapplier

    def _ratapplier_check_string_col_valid(info, inputs, outputs, otherargs):
        str_col_vals = getattr(inputs.inrat, otherargs.str_col)
        out_col_vals = numpy.empty_like(str_col_vals)
        for i in range(str_col_vals.shape[0]):
            try:
                str_val_tmp = str_col_vals[i].decode("utf-8")
            except:
                str_val_tmp = ""
            str_val_tmp = str_val_tmp.strip()
            str_val_tmp = rsgislib.tools.utils.check_str(
                str_val_tmp,
                rm_non_ascii=rm_non_ascii,
                rm_dashs=rm_dashs,
                rm_spaces=rm_spaces,
                rm_punc=rm_punc,
            )
            out_col_vals[i] = str_val_tmp
        setattr(outputs.outrat, otherargs.str_col, out_col_vals)

    in_rats = ratapplier.RatAssociations()
    out_rats = ratapplier.RatAssociations()

    in_rats.inrat = ratapplier.RatHandle(clumps_img)
    out_rats.outrat = ratapplier.RatHandle(clumps_img)

    otherargs = ratapplier.OtherArguments()
    otherargs.str_col = str_col

    ratapplier.apply(_ratapplier_check_string_col_valid, in_rats, out_rats, otherargs)


def define_class_names(clumps_img, class_num_col, class_name_col, class_names_dict):
    """
    A function to create a class names column in a RAT based on segmented clumps
     where a number of clumps have the same number class.

    :param clumps_img: input clumps image.
    :param class_num_col: column specifying the class number (e.g., where clumps
                        are segments in a segmentation)
    :param class_name_col: the output column name where a string will be created if
                         it doesn't already exists.
    :param class_names_dict: Dictionary to look up the class names. The key needs to
                           the integer number for the class

    """
    in_rats = ratapplier.RatAssociations()
    out_rats = ratapplier.RatAssociations()

    in_rats.inrat = ratapplier.RatHandle(clumps_img)
    out_rats.outrat = ratapplier.RatHandle(clumps_img)

    otherargs = ratapplier.OtherArguments()
    otherargs.classNumCol = class_num_col
    otherargs.classNameCol = class_name_col
    otherargs.classNamesDict = class_names_dict

    def _ratapplier_defClassNames(info, inputs, outputs, otherargs):
        classNum = getattr(inputs.inrat, otherargs.classNumCol)

        classNames = numpy.empty_like(classNum, dtype=numpy.dtype("a255"))
        classNames[...] = ""

        for key in otherargs.classNamesDict:
            classNames = numpy.where(
                (classNum == key), otherargs.classNamesDict[key], classNames
            )

        setattr(outputs.outrat, otherargs.classNameCol, classNames)

    ratapplier.apply(_ratapplier_defClassNames, in_rats, out_rats, otherargs)


def set_column_data(clumps_img, col_name, col_data):
    """
    A function to read a column of data from a RAT.

    :param clumps_img: Input clumps image
    :param col_name: Name of the column to be written.
    :param col_data: Data to be written to the column.
    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this function "
            "could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this function "
            "could not be imported\n\t" + riosRatErr
        )

    rat_length = rsgislib.rastergis.get_rat_length(clumps_img)
    if rat_length != (col_data.shape[0]):
        raise Exception("The input data array is not the same length as the RAT.")

    ratDataset = gdal.Open(clumps_img, gdal.GA_Update)
    if ratDataset is None:
        raise Exception("The input image could not be opened.")

    rat.writeColumn(ratDataset, col_name, col_data)
    ratDataset = None


def create_uid_col(clumps_img, col_name="UID"):
    """
    A function which adds a unique ID value (starting at 0) to each clump within a RAT.

    :param clumps_img: Input clumps image
    :param col_name: The output column name (default is UID).

    """
    n_rows = rsgislib.rastergis.get_rat_length(clumps_img)
    uid_col = numpy.arange(0, n_rows, 1, dtype=numpy.uint32)
    set_column_data(clumps_img, col_name, uid_col)



def take_random_sample(
    clumps_img, in_col_name, in_col_val, out_col_name, sample_ratio, rnd_seed=0
):
    """
    A function to take a random sample of an input column.

    :param clumps_img: clumps image.
    :param in_col_name: input column name.
    :param in_col_val: numeric value for which the random sample is to be taken for.
    :param out_col_name: output column where value of 1 is selected within the random
                       sample and 0 is not selected.
    :param sample_ratio: the size of the sample (0 - 1.0; i.e., 10% = 0.1) to be
                        taken of the number of rows within input value.
    :param rnd_seed: is the seed for the random number generation (optional;
                     default is 0).
    """
    if (sample_ratio <= 0) or (sample_ratio >= 1):
        raise rsgislib.RSGISPyException("Input sample ratio must be between 0 and 1.")

    # Define the seed for the random number generation.
    numpy.random.seed(rnd_seed)

    # READ COL FROM RAT:
    ratDataset = gdal.Open(clumps_img, gdal.GA_Update)
    inColVals = rat.readColumn(ratDataset, in_col_name)

    # Create an array for the original array indexes.
    ID = numpy.arange(inColVals.shape[0])

    # Cut array and array indexes to be only bins with a value of 1
    # (i.e., there is lidar data here).
    ID = ID[inColVals == in_col_val]

    # Sample of the subsetted input array where it equals inColVal.
    numOfSamples = int(ID.shape[0] * sample_ratio)
    IDSamples = numpy.arange(ID.shape[0])
    IDSampPerms = numpy.random.permutation(IDSamples)
    IDSampPermsSelect = IDSampPerms[0:numOfSamples]

    # Find the array indexes for the whole input array (i.e., the whole RAT).
    outArryIdxsSel = ID[IDSampPermsSelect]

    # Create output columns for writing to RAT
    outColVals = numpy.zeros_like(inColVals)

    # Populate columns where those selected have value 1.
    outColVals[outArryIdxsSel] = 1

    # WRITE COL TO RAT:
    rat.writeColumn(ratDataset, out_col_name, outColVals)
    ratDataset = None



def identify_small_units(
    clumps_img,
    class_col,
    tmp_dir,
    out_col_name,
    small_clumps_thres,
    use_tiled_clump=False,
    n_cores=1,
    tile_width=2000,
    tile_height=2000,
):
    """
    Identify small connected units within a classification. The threshold to define
    small is provided by the user in pixels. Note, the outColName and smallClumpsThres
    variables can be provided as lists to identify a number of thresholds of
    small units.

    :param clumps_img: string for the clumps image file containing input classification
    :param class_col: string for the column name representing the classification as
                     integer values
    :param tmp_dir: directory path where temporary layers are stored (if directory is
                    created within the function it will be deleted once function
                    is complete).
    :param out_col_name: a list of output column names (i.e., one for each threshold)
    :param small_clumps_thres: a list of thresholds for identifying small clumps.
    :param use_tiled_clump: a boolean to specify whether the tiled clumping algorithm
                          should be used (Default is False; select True for large
                          datasets)
    :param n_cores: if the tiled version of the clumping algorithm is being used then
                   there is an option to use multiple processing cores; specify the
                   number to be used (Default is 2).
    :param tile_width: is the width of the image tile (in pixels) if tiled clumping
                      is used.
    :param tile_height: is the height of the image tile (in pixels) if tiled clumping
                       is used.

    Example:

    .. code:: python
        import rsgislib.rastergis

        clumpsImg = "LS2MSS_19750620_lat10lon6493_r67p250_rad_srefdem_30m_clumps.kea"
        tmpPath = "./tmp/"
        classCol = "OutClass"
        outColName = ["SmallUnits25", "SmallUnits50", "SmallUnits100"]
        smallClumpsThres = [25, 50, 100]
        rastergis.identify_small_units(clumpsImg, classCol, tmpPath,
                                       outColName, smallClumpsThres)

    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this function "
            "could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this function "
            "could not be imported\n\t" + riosRatErr
        )

    import rsgislib.tools.filetools

    if len(out_col_name) is not len(small_clumps_thres):
        print(
            "The number of threshold values and output "
            "column names should be the same."
        )
        sys.exit(-1)

    numThresholds = len(small_clumps_thres)

    createdDIR = False
    if not os.path.isdir(tmp_dir):
        os.makedirs(tmp_dir)
        createdDIR = True

    baseName = os.path.splitext(os.path.basename(clumps_img))[0]
    classMaskImg = os.path.join(tmp_dir, baseName + "_TmpClassMask.kea")
    classMaskClumps = os.path.join(tmp_dir, baseName + "_TmpClassMaskClumps.kea")
    smallClumpsMask = os.path.join(tmp_dir, baseName + "_SmallClassClumps.kea")

    rastergis.export_col_to_gdal_img(
        clumps_img, classMaskImg, "KEA", rsgislib.TYPE_16UINT, class_col
    )
    if use_tiled_clump:
        from rsgislib.segmentation import tiledclump

        if n_cores > 1:
            tiledclump.perform_clumping_multi_process(
                classMaskImg,
                classMaskClumps,
                tmpDIR=os.path.join(tmp_dir, baseName + "_ClumpTmp"),
                width=tile_width,
                height=tile_height,
                nCores=n_cores,
            )
        else:
            tiledclump.perform_clumping_single_thread(
                classMaskImg,
                classMaskClumps,
                tmpDIR=os.path.join(
                    tmp_dir,
                    baseName + "_ClumpTmp",
                    width=tile_width,
                    height=tile_height,
                ),
            )
    else:
        segmentation.clump(classMaskImg, classMaskClumps, "KEA", False, 0)
    rastergis.pop_rat_img_stats(classMaskClumps, False, False)

    for i in range(numThresholds):
        print(
            "Processing thresold "
            + str(small_clumps_thres[i])
            + " - "
            + out_col_name[i]
        )
        ratDataset = gdal.Open(classMaskClumps, gdal.GA_Update)
        Histogram = rat.readColumn(ratDataset, "Histogram")
        smallUnits = numpy.zeros_like(Histogram, dtype=numpy.int16)
        smallUnits[Histogram < small_clumps_thres[i]] = 1
        rat.writeColumn(ratDataset, "smallUnits", smallUnits)
        ratDataset = None

        rastergis.export_col_to_gdal_img(
            classMaskClumps, smallClumpsMask, "KEA", rsgislib.TYPE_8UINT, "smallUnits"
        )

        bs = []
        bs.append(rastergis.BandAttStats(band=1, maxField=out_col_name[i]))
        rastergis.populate_rat_with_stats(smallClumpsMask, clumps_img, bs)

    rsgislib.tools.filetools.delete_file_with_basename(classMaskImg)
    rsgislib.tools.filetools.delete_file_with_basename(classMaskClumps)
    rsgislib.tools.filetools.delete_file_with_basename(smallClumpsMask)
    if createdDIR:
        shutil.rmtree(tmp_dir)


def _computeProximityArrArgsFunc(arg_vals):
    """
    This function is used internally within calc_dist_to_classes for the multiprocessing Pool

    """
    classImgDS = gdal.Open(arg_vals[0], gdal.GA_ReadOnly)
    classImgBand = classImgDS.GetRasterBand(1)
    imageutils.create_copy_img(
        arg_vals[0], arg_vals[1], 1, arg_vals[3], arg_vals[4], rsgislib.TYPE_32FLOAT
    )
    distImgDS = gdal.Open(arg_vals[1], gdal.GA_Update)
    distImgBand = distImgDS.GetRasterBand(1)
    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress
    gdal.ComputeProximity(classImgBand, distImgBand, arg_vals[2], callback=callback)
    distImgBand = None
    distImgDS = None
    classImgBand = None
    classImgDS = None


def calc_dist_to_classes(
    clumps_img,
    class_col,
    out_img_base,
    tmp_dir="./tmp",
    tile_size=2000,
    max_dist=1000,
    no_data_val=1000,
    n_cores=-1,
):
    """
    A function which will calculate proximity rasters for a set of classes
    defined within the RAT.

    :param clumps_img: is a string specifying the input image with the associated RAT
    :param class_col: is the column in the RAT which has the classification
    :param out_img_base: is the base name of the output image - output files will
                       be KEA files.
    :param tmp_dir: is a directory to be used for storing the image tiles and
                   other temporary files - if not directory does not exist it
                   will be created and deleted on completion (Default: ./tmp).
    :param tile_size: is an int specifying in pixels the size of the image tiles
                     used for processing (Default: 2000)
    :param max_dist: is the maximum distance in units of the geographic units of
                    the projection of the input image (Default: 1000).
    :param no_data_val: is the value applied to the pixels outside of the maxDist
                   threshold (Default: 1000; i.e., the same as maxDist).
    :param n_cores: is the number of processing cores which are available to be
                   used for this processing. If -1 all available cores will be
                   used. (Default: -1)

    """
    import rsgislib.tools.utils
    import rsgislib.tools.filetools

    tmpPresent = True
    if not os.path.exists(tmp_dir):
        print("WARNING: '" + tmp_dir + "' directory does not exist so creating it...")
        os.makedirs(tmp_dir)
        tmpPresent = False

    if n_cores <= 0:
        n_cores = multiprocessing.cpu_count()

    uid = rsgislib.tools.utils.uid_generator()

    classesImg = os.path.join(tmp_dir, "ClassImg_" + uid + ".kea")
    rastergis.export_col_to_gdal_img(
        clumps_img, classesImg, "KEA", rsgislib.TYPE_32UINT, class_col
    )

    ratDataset = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    classColInt = rat.readColumn(ratDataset, class_col)
    ratDataset = None

    classIDs = numpy.unique(classColInt)

    xRes, yRes = rsgislib.imageutils.get_img_res(classesImg)

    # print("Image Res {} x {}".format(xRes, yRes))

    xMaxDistPxl = math.ceil(max_dist / xRes)
    yMaxDistPxl = math.ceil(max_dist / yRes)

    print("Max Dist Pxls X = {}, Y = {}".format(xMaxDistPxl, yMaxDistPxl))

    tileOverlap = xMaxDistPxl
    if yMaxDistPxl > xMaxDistPxl:
        tileOverlap = yMaxDistPxl

    classTilesDIR = os.path.join(tmp_dir, "ClassTiles_" + uid)
    classTilesDIRPresent = True
    if not os.path.exists(classTilesDIR):
        os.makedirs(classTilesDIR)
        classTilesDIRPresent = False

    classesImgTileBase = os.path.join(classTilesDIR, "ClassImgTile")
    imageutils.create_tiles(
        classesImg,
        classesImgTileBase,
        tile_size,
        tile_size,
        tileOverlap,
        0,
        "KEA",
        rsgislib.TYPE_32UINT,
        "kea",
    )
    imgTileFiles = glob.glob(classesImgTileBase + "*.kea")

    distTilesDIR = os.path.join(tmp_dir, "DistTiles_" + uid)
    distTilesDIRPresent = True
    if not os.path.exists(distTilesDIR):
        os.makedirs(distTilesDIR)
        distTilesDIRPresent = False

    proxOptionsBase = [
        "MAXDIST=" + str(max_dist),
        "DISTUNITS=GEO",
        "NODATA=" + str(no_data_val),
    ]

    for classID in classIDs:
        print("Class {}".format(classID))
        proxOptions = list(proxOptionsBase)
        proxOptions.append("VALUES=" + str(classID))

        distTiles = []
        distTileArgs = []
        for classTileFile in imgTileFiles:
            baseTileName = os.path.basename(classTileFile)
            distTileFile = os.path.join(distTilesDIR, baseTileName)
            tileArgs = [classTileFile, distTileFile, proxOptions, no_data_val, "KEA"]
            distTiles.append(distTileFile)
            distTileArgs.append(tileArgs)

        with Pool(n_cores) as p:
            p.map(_computeProximityArrArgsFunc, distTileArgs)

        distImage = out_img_base + "_" + str(classID) + ".kea"
        # Mosaic Tiles
        imageutils.create_img_mosaic(
            distTiles,
            distImage,
            no_data_val,
            no_data_val,
            1,
            1,
            "KEA",
            rsgislib.TYPE_32FLOAT,
        )
        imageutils.pop_img_stats(
            distImage, use_no_data=True, no_data_val=no_data_val, calc_pyramids=True
        )
        for imgFile in distTiles:
            rsgislib.tools.filetools.delete_file_with_basename(imgFile)

    if not classTilesDIRPresent:
        shutil.rmtree(classTilesDIR, ignore_errors=True)
    else:
        for classTileFile in imgTileFiles:
            rsgislib.tools.filetools.delete_file_with_basename(classTileFile)

    if not distTilesDIRPresent:
        shutil.rmtree(distTilesDIR, ignore_errors=True)

    if not tmpPresent:
        shutil.rmtree(tmp_dir, ignore_errors=True)
    else:
        os.remove(classesImg)


def calc_dist_between_clumps(
    clumps_img, out_col_name, tmp_dir="./tmp", use_idx=False, max_dist_thres=10
):
    """
    Calculate the distance between all clumps

    :param clumps_img: image clumps for which the distance will be calculated.
    :param out_col_name: output column within the clumps image.
    :param tmp_dir: directory out tempory files will be outputted to.
    :param use_idx: use a spatial index when calculating the distance between
                   clumps (needed for large number of clumps).
    :param max_dist_thres: if using an index than an upper limit on the distance
                         between clumps can be defined.

    """
    import rsgislib.tools.utils
    import rsgislib.tools.filetools

    tmpPresent = True
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        tmpPresent = False

    baseName = "{}_{}".format(
        rsgislib.tools.filetools.get_file_basename(clumps_img),
        rsgislib.tools.utils.uid_generator(),
    )

    polysShp = os.path.join(tmp_dir, baseName + "_shp.shp")
    vectorutils.polygoniseRaster(
        clumps_img, polysShp, imgBandNo=1, maskImg=clumps_img, imgMaskBandNo=1
    )

    print(
        "Calculating Distance - can take some time. Try using index "
        "and decreasing max distance threshold."
    )
    t = rsgislib.RSGISTime()
    t.start(True)
    polysShpGeomDist = os.path.join(tmp_dir, baseName + "_dist_shp.shp")
    vectorutils.dist2NearestGeom(
        polysShp, polysShpGeomDist, out_col_name, True, use_idx, max_dist_thres
    )
    t.end()

    rastergis.import_vec_atts(clumps_img, polysShpGeomDist, [out_col_name])

    if not tmpPresent:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def calc_dist_to_large_clumps(
    clumps_img,
    out_col_name,
    size_thres,
    tmp_dir="./tmp",
    use_idx=False,
    max_dist_thres=10,
):
    """
    Calculate the distance from each small clump to a large clump. Split defined by
    the threshold provided.

    :param clumps_img: image clumps for which the distance will be calculated.
    :param out_col_name: output column within the clumps image.
    :param size_thres: is a threshold to seperate the sets of large and small clumps.
    :param tmp_dir: directory out tempory files will be outputted to.
    :param use_idx: use a spatial index when calculating the distance between clumps
                   (needed for large number of clumps).
    :param max_dist_thres: if using an index than an upper limit on the distance
                         between clumps can be defined.

    """
    import rsgislib.tools.utils

    tmpPresent = True
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        tmpPresent = False

    uidStr = rsgislib.tools.utils.uid_generator()
    baseName = "{}_{}".format(
        rsgislib.tools.filetools.get_file_basename(clumps_img), uidStr
    )

    ratDataset = gdal.Open(clumps_img, gdal.GA_Update)
    Histogram = rat.readColumn(ratDataset, "Histogram")
    smallUnits = numpy.zeros_like(Histogram, dtype=numpy.int16)
    smallUnits[Histogram < size_thres] = 1
    rat.writeColumn(ratDataset, "smallUnits", smallUnits)

    ID = numpy.arange(Histogram.shape[0])

    smUnitIDs = ID[smallUnits == 1]
    smUnitIDs = smUnitIDs[smUnitIDs > 0]
    lrgUnitIDs = ID[smallUnits == 0]
    lrgUnitIDs = lrgUnitIDs[lrgUnitIDs > 0]

    print("There are {} small clumps.".format(smUnitIDs.shape[0]))
    print("There are {} large clumps.".format(lrgUnitIDs.shape[0]))

    smUnitFIDs = numpy.arange(smUnitIDs.shape[0])
    lrgUnitFIDs = numpy.arange(lrgUnitIDs.shape[0])

    smUnitClumpIDs = numpy.zeros_like(Histogram, dtype=numpy.int16)
    smUnitClumpIDs[smUnitIDs] = smUnitFIDs
    lrgUnitClumpIDs = numpy.zeros_like(Histogram, dtype=numpy.int16)
    lrgUnitClumpIDs[lrgUnitIDs] = lrgUnitFIDs

    rat.writeColumn(ratDataset, "SmUnits", smUnitClumpIDs)
    rat.writeColumn(ratDataset, "LrgUnits", lrgUnitClumpIDs)
    rat.writeColumn(ratDataset, "smallUnitsBin", smallUnits)

    smClumpsImg = os.path.join(tmp_dir, baseName + "_smclumps.kea")
    rastergis.export_col_to_gdal_img(
        clumps_img, smClumpsImg, "KEA", rsgislib.TYPE_32UINT, "SmUnits"
    )
    rastergis.pop_rat_img_stats(
        clumps_img=smClumpsImg, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
    )

    lrgClumpsImg = os.path.join(tmp_dir, baseName + "_lrgclumps.kea")
    rastergis.export_col_to_gdal_img(
        clumps_img, lrgClumpsImg, "KEA", rsgislib.TYPE_32UINT, "LrgUnits"
    )
    rastergis.pop_rat_img_stats(
        clumps_img=lrgClumpsImg, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
    )

    smPolysShp = os.path.join(tmp_dir, baseName + "_smClumps_shp.shp")
    rsgislib.vectorutils.polygoniseRaster(
        smClumpsImg, smPolysShp, imgBandNo=1, maskImg=smClumpsImg, imgMaskBandNo=1
    )

    lgrPolysShp = os.path.join(tmp_dir, baseName + "_lgrClumps_shp.shp")
    rsgislib.vectorutils.polygoniseRaster(
        lrgClumpsImg, lgrPolysShp, imgBandNo=1, maskImg=lrgClumpsImg, imgMaskBandNo=1
    )

    print(
        "Calculating Distance - can take some time. Try using index and "
        "decreasing max distance threshold."
    )
    t = rsgislib.RSGISTime()
    t.start(True)
    smPolysDistShp = os.path.join(tmp_dir, baseName + "_smClumps_dist_shp.shp")
    rsgislib.vectorutils.dist2NearestSecGeomSet(
        smPolysShp,
        lgrPolysShp,
        smPolysDistShp,
        out_col_name,
        True,
        use_idx,
        max_dist_thres,
    )
    t.end()
    rsgislib.rastergis.import_vec_atts(smClumpsImg, smPolysDistShp, [out_col_name])

    smClumpsRATDataset = gdal.Open(smClumpsImg, gdal.GA_Update)
    minDistCol = rat.readColumn(smClumpsRATDataset, out_col_name)

    minDistSmlClumpsArr = numpy.zeros_like(Histogram, dtype=numpy.float32)
    minDistSmlClumpsArr[smUnitIDs] = minDistCol

    rat.writeColumn(ratDataset, out_col_name, minDistSmlClumpsArr)

    smClumpsRATDataset = None
    ratDataset = None

    if not tmpPresent:
        shutil.rmtree(tmp_dir, ignore_errors=True)



