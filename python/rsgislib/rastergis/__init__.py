#!/usr/bin/env python
"""
The Raster GIS module contains functions for attributing and manipulating
raster attribute tables.
"""
import os
import sys

import numpy
from osgeo import gdal

import rsgislib

# import the C++ extension into this level
from ._rastergis import *


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

    def __init__(self, percentile: float, field_name: str):
        self.percentile = percentile
        self.field_name = field_name


class ShapeIndex:
    """This is passed to the calcShapeIndices function"""

    def __init__(self, col_name: str, idx: int, col_idx: int = 0):
        self.col_name = col_name
        self.col_idx = col_idx
        self.idx = idx


class ChangeFeats:
    def __init__(self, cls_name: str, cls_out_name: str = None, threshold: float = 0):
        self.cls_name = cls_name
        self.cls_out_name = cls_out_name
        self.threshold = threshold


def export_cols_to_gdal_img(
    clumps_img: str,
    output_img: str,
    gdalformat: str,
    datatype: int,
    fields: list[str],
    rat_band: int = 1,
    tmp_dir: str = None,
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
    :param rat_band: is an optional (default = 1) integer parameter specifying the
                     image band to which the RAT is associated.

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
    import rsgislib.imageutils

    if tmp_dir is None:
        tmp_dir = os.path.split(output_img)[0]

    out_ext = os.path.splitext(output_img)[-1]
    tmp_file_list = []

    # Export each field
    for field in fields:
        print("Exporting: " + field)
        out_temp_file = os.path.join(tmp_dir, field + out_ext)
        export_col_to_gdal_img(
            clumps_img, out_temp_file, gdalformat, datatype, field, rat_band
        )
        tmp_file_list.append(out_temp_file)

    # Stack Bands
    print("Stacking Bands")
    rsgislib.imageutils.stack_img_bands(
        tmp_file_list, fields, output_img, None, 0, gdalformat, datatype
    )

    # Remove temp files
    print("Removing temp files")
    for tmp_file in tmp_file_list:
        rsgislib.tools.filetools.delete_file_with_basename(tmp_file)


def get_rat_length(clumps_img: str, rat_band: int = 1) -> int:
    """
    A function which returns the length (i.e., number of rows) within the RAT.

    :param clumps_img: path to the image file with the RAT
    :param rat_band: the band within the image file for which the RAT is to read.
    :returns: an int with the number of rows.

    """
    # Open input image file
    clumps_img_ds = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    if clumps_img_ds is None:
        raise rsgislib.RSGISPyException("Could not open the inputted clumps image.")

    clumps_img_band = clumps_img_ds.GetRasterBand(rat_band)
    if clumps_img_band is None:
        raise rsgislib.RSGISPyException(
            "Could not open the inputted clumps image band."
        )

    clumps_img_rat = clumps_img_band.GetDefaultRAT()
    if clumps_img_rat is None:
        raise rsgislib.RSGISPyException(
            "Could not open the inputted clumps image band RAT."
        )

    nrows = clumps_img_rat.GetRowCount()

    clumps_img_ds = None
    return nrows


def get_rat_columns(clumps_img: str, rat_band: int = 1) -> list[str]:
    """
    A function which returns a list of column names within the RAT.

    :param clumps_img: path to the image file with the RAT
    :param rat_band: the band within the image file for which the RAT is to read.
    :returns: list of column names.

    """
    # Open input image file
    clumps_img_ds = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    if clumps_img_ds is None:
        raise rsgislib.RSGISPyException("Could not open the inputted clumps image.")

    clumps_img_band = clumps_img_ds.GetRasterBand(rat_band)
    if clumps_img_band is None:
        raise rsgislib.RSGISPyException(
            "Could not open the inputted clumps image band."
        )

    clumps_img_rat = clumps_img_band.GetDefaultRAT()
    if clumps_img_rat is None:
        raise rsgislib.RSGISPyException(
            "Could not open the inputted clumps image band RAT."
        )

    ncols = clumps_img_rat.GetColumnCount()
    col_names = []
    for col_idx in range(ncols):
        col_names.append(clumps_img_rat.GetNameOfCol(col_idx))

    clumps_img_ds = None
    return col_names


def get_rat_columns_info(clumps_img: str, rat_band: int = 1):
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
        raise rsgislib.RSGISPyException("Could not open the inputted clumps image.")

    clumps_img_band = clumps_img_ds.GetRasterBand(rat_band)
    if clumps_img_band is None:
        raise rsgislib.RSGISPyException(
            "Could not open the inputted clumps image band."
        )

    clumps_img_rat = clumps_img_band.GetDefaultRAT()
    if clumps_img_rat is None:
        raise rsgislib.RSGISPyException(
            "Could not open the inputted clumps image band RAT."
        )

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


def set_class_names_colours(
    clumps_img: str, class_names_col: str, class_info_dict: dict
):
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
    from rios import rat

    n_rows = get_rat_length(clumps_img)
    col_names = get_rat_columns(clumps_img)

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

    rat_dataset = gdal.Open(clumps_img, gdal.GA_Update)

    if red_avail:
        red_arr = rat.readColumn(rat_dataset, "Red")
    else:
        red_arr = numpy.zeros(n_rows, dtype=int)

    if green_avail:
        green_arr = rat.readColumn(rat_dataset, "Green")
    else:
        green_arr = numpy.zeros(n_rows, dtype=int)

    if blue_avail:
        blue_arr = rat.readColumn(rat_dataset, "Blue")
    else:
        blue_arr = numpy.zeros(n_rows, dtype=int)

    if class_names_col_avail:
        class_names_arr = rat.readColumn(rat_dataset, class_names_col)
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

    rat.writeColumn(rat_dataset, class_names_col, class_names_arr)
    rat.writeColumn(rat_dataset, "Red", red_arr)
    rat.writeColumn(rat_dataset, "Green", green_arr)
    rat.writeColumn(rat_dataset, "Blue", blue_arr)

    rat_dataset = None


def get_column_data(clumps_img: str, col_name: str) -> numpy.array:
    """
    A function to read a column of data from a RAT.

    :param clumps_img: Input clumps image
    :param col_name: Name of the column to be read.

    :return: numpy array with values from the clumpsImg
    """
    from rios import rat

    col_names = get_rat_columns(clumps_img)

    if col_name not in col_names:
        raise rsgislib.RSGISPyException("Column specified is not within the RAT.")

    rat_dataset = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    if rat_dataset is None:
        raise rsgislib.RSGISPyException("The input image could not be opened.")

    col_data = rat.readColumn(rat_dataset, col_name)
    rat_dataset = None
    return col_data


def read_rat_neighbours(
    clumps_img: str, start_row: int = None, end_row: int = None, rat_band: int = 1
) -> list[list[int]]:
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
    import h5py

    # Check that 'NumNeighbours' column exists
    rat_columns = get_rat_columns(clumps_img, rat_band)
    if "NumNeighbours" not in rat_columns:
        raise rsgislib.RSGISPyException(
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
    clumps_img: str,
    str_col: str,
    rm_punc: bool = False,
    rm_spaces: bool = False,
    rm_non_ascii: bool = False,
    rm_dashs: bool = False,
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


def define_class_names(
    clumps_img: str, class_num_col: str, class_name_col: str, class_names_dict: dict
):
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
    from rios import ratapplier

    in_rats = ratapplier.RatAssociations()
    out_rats = ratapplier.RatAssociations()

    in_rats.inrat = ratapplier.RatHandle(clumps_img)
    out_rats.outrat = ratapplier.RatHandle(clumps_img)

    otherargs = ratapplier.OtherArguments()
    otherargs.classNumCol = class_num_col
    otherargs.classNameCol = class_name_col
    otherargs.classNamesDict = class_names_dict

    def _ratapplier_def_class_names(info, inputs, outputs, otherargs):
        classNum = getattr(inputs.inrat, otherargs.classNumCol)

        class_names = numpy.empty_like(classNum, dtype=numpy.dtype("a255"))
        class_names[...] = ""

        for key in otherargs.classNamesDict:
            class_names = numpy.where(
                (classNum == key), otherargs.classNamesDict[key], class_names
            )

        setattr(outputs.outrat, otherargs.classNameCol, class_names)

    ratapplier.apply(_ratapplier_def_class_names, in_rats, out_rats, otherargs)


def set_column_data(clumps_img: str, col_name: str, col_data: numpy.array):
    """
    A function to read a column of data from a RAT.

    :param clumps_img: Input clumps image
    :param col_name: Name of the column to be written.
    :param col_data: Data to be written to the column.
    """
    from rios import rat

    rat_length = get_rat_length(clumps_img)
    if rat_length != (col_data.shape[0]):
        raise rsgislib.RSGISPyException(
            "The input data array is not the same length as the RAT."
        )

    rat_dataset = gdal.Open(clumps_img, gdal.GA_Update)
    if rat_dataset is None:
        raise rsgislib.RSGISPyException("The input image could not be opened.")

    rat.writeColumn(rat_dataset, col_name, col_data)
    rat_dataset = None


def create_uid_col(clumps_img: str, col_name: str = "UID"):
    """
    A function which adds a unique ID value (starting at 0) to each clump within a RAT.

    :param clumps_img: Input clumps image
    :param col_name: The output column name (default is UID).

    """
    n_rows = get_rat_length(clumps_img)
    uid_col = numpy.arange(0, n_rows, 1, dtype=numpy.uint32)
    set_column_data(clumps_img, col_name, uid_col)


def take_random_sample(
    clumps_img: str,
    in_col_name: str,
    in_col_val: float,
    out_col_name: str,
    sample_ratio: float,
    rnd_seed: int = 0,
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
    import rsgislib
    from rios import rat

    if (sample_ratio <= 0) or (sample_ratio >= 1):
        raise rsgislib.RSGISPyException("Input sample ratio must be between 0 and 1.")

    # Define the seed for the random number generation.
    numpy.random.seed(rnd_seed)

    # READ COL FROM RAT:
    rat_dataset = gdal.Open(clumps_img, gdal.GA_Update)
    in_col_vals = rat.readColumn(rat_dataset, in_col_name)

    # Create an array for the original array indexes.
    id_vals = numpy.arange(in_col_vals.shape[0])

    # Cut array and array indexes to be only bins with a value of 1
    # (i.e., there is lidar data here).
    id_vals = id_vals[in_col_vals == in_col_val]

    # Sample of the subsetted input array where it equals inColVal.
    num_of_samples = int(id_vals.shape[0] * sample_ratio)
    id_samples = numpy.arange(id_vals.shape[0])
    id_samp_perms = numpy.random.permutation(id_samples)
    id_samp_perms_select = id_samp_perms[0:num_of_samples]

    # Find the array indexes for the whole input array (i.e., the whole RAT).
    out_arry_idxs_sel = id_vals[id_samp_perms_select]

    # Create output columns for writing to RAT
    out_col_vals = numpy.zeros_like(in_col_vals)

    # Populate columns where those selected have value 1.
    out_col_vals[out_arry_idxs_sel] = 1

    # WRITE COL TO RAT:
    rat.writeColumn(rat_dataset, out_col_name, out_col_vals)
    rat_dataset = None


def identify_small_units(
    clumps_img: str,
    class_col: str,
    tmp_dir: str,
    out_col_name: str,
    small_clumps_thres: float,
    use_tiled_clump: bool = False,
    n_cores: int = 1,
    tile_width: int = 2000,
    tile_height: int = 2000,
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
    import shutil
    from rios import rat
    import rsgislib.tools.filetools
    import rsgislib.segmentation

    if len(out_col_name) != len(small_clumps_thres):
        raise rsgislib.RSGISPyException(
            "The number of threshold values and output "
            "column names should be the same."
        )

    num_thresholds = len(small_clumps_thres)

    created_dir = False
    if not os.path.isdir(tmp_dir):
        os.makedirs(tmp_dir)
        created_dir = True

    base_name = os.path.splitext(os.path.basename(clumps_img))[0]
    class_mask_img = os.path.join(tmp_dir, base_name + "_TmpClassMask.kea")
    class_mask_clumps = os.path.join(tmp_dir, base_name + "_TmpClassMaskClumps.kea")
    small_clumps_mask = os.path.join(tmp_dir, base_name + "_SmallClassClumps.kea")

    export_col_to_gdal_img(
        clumps_img, class_mask_img, "KEA", rsgislib.TYPE_16UINT, class_col
    )
    if use_tiled_clump:
        from rsgislib.segmentation import tiledclump

        if n_cores > 1:
            tiledclump.perform_clumping_multi_process(
                class_mask_img,
                class_mask_clumps,
                tmpDIR=os.path.join(tmp_dir, base_name + "_ClumpTmp"),
                width=tile_width,
                height=tile_height,
                nCores=n_cores,
            )
        else:
            tiledclump.perform_clumping_single_thread(
                class_mask_img,
                class_mask_clumps,
                tmpDIR=os.path.join(
                    tmp_dir,
                    base_name + "_ClumpTmp",
                    width=tile_width,
                    height=tile_height,
                ),
            )
    else:
        rsgislib.segmentation.clump(class_mask_img, class_mask_clumps, "KEA", False, 0)
    pop_rat_img_stats(class_mask_clumps, False, False)

    for i in range(num_thresholds):
        print(
            "Processing thresold "
            + str(small_clumps_thres[i])
            + " - "
            + out_col_name[i]
        )
        rat_dataset = gdal.Open(class_mask_clumps, gdal.GA_Update)
        histogram = rat.readColumn(rat_dataset, "Histogram")
        small_units = numpy.zeros_like(histogram, dtype=numpy.int16)
        small_units[histogram < small_clumps_thres[i]] = 1
        rat.writeColumn(rat_dataset, "smallUnits", small_units)
        rat_dataset = None

        export_col_to_gdal_img(
            class_mask_clumps,
            small_clumps_mask,
            "KEA",
            rsgislib.TYPE_8UINT,
            "smallUnits",
        )

        bs = []
        bs.append(BandAttStats(band=1, maxField=out_col_name[i]))
        populate_rat_with_stats(small_clumps_mask, clumps_img, bs)

    rsgislib.tools.filetools.delete_file_with_basename(class_mask_img)
    rsgislib.tools.filetools.delete_file_with_basename(class_mask_clumps)
    rsgislib.tools.filetools.delete_file_with_basename(small_clumps_mask)
    if created_dir:
        shutil.rmtree(tmp_dir)


def _computeProximityArrArgsFunc(arg_vals: list):
    """
    This function is used internally within calc_dist_to_classes for the
    multiprocessing Pool

    """
    import rsgislib.imageutils

    class_img_ds = gdal.Open(arg_vals[0], gdal.GA_ReadOnly)
    class_img_band = class_img_ds.GetRasterBand(1)
    rsgislib.imageutils.create_copy_img(
        arg_vals[0], arg_vals[1], 1, arg_vals[3], arg_vals[4], rsgislib.TYPE_32FLOAT
    )
    dist_img_ds = gdal.Open(arg_vals[1], gdal.GA_Update)
    dist_img_band = dist_img_ds.GetRasterBand(1)
    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress
    gdal.ComputeProximity(class_img_band, dist_img_band, arg_vals[2], callback=callback)
    dist_img_band = None
    dist_img_ds = None
    class_img_band = None
    class_img_ds = None


def calc_dist_to_classes(
    clumps_img: str,
    class_col: str,
    out_img_base: str,
    tmp_dir: str = "./tmp",
    tile_size: int = 2000,
    max_dist: int = 1000,
    no_data_val: int = 1000,
    n_cores: int = -1,
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
    import multiprocessing
    import math
    import glob
    import shutil
    from rios import rat
    import rsgislib.tools.utils
    import rsgislib.tools.filetools
    import rsgislib.imageutils

    tmp_present = True
    if not os.path.exists(tmp_dir):
        print("WARNING: '" + tmp_dir + "' directory does not exist so creating it...")
        os.makedirs(tmp_dir)
        tmp_present = False

    if n_cores <= 0:
        n_cores = multiprocessing.cpu_count()

    uid_str = rsgislib.tools.utils.uid_generator()

    classes_img = os.path.join(tmp_dir, f"ClassImg_{uid_str}.kea")
    export_col_to_gdal_img(
        clumps_img, classes_img, "KEA", rsgislib.TYPE_32UINT, class_col
    )

    rat_dataset = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    class_col_int = rat.readColumn(rat_dataset, class_col)
    rat_dataset = None

    class_ids = numpy.unique(class_col_int)

    x_res, y_res = rsgislib.imageutils.get_img_res(classes_img)

    x_max_dist_pxl = math.ceil(max_dist / x_res)
    y_max_dist_pxl = math.ceil(max_dist / y_res)

    print("Max Dist Pxls X = {}, Y = {}".format(x_max_dist_pxl, y_max_dist_pxl))

    tile_overlap = x_max_dist_pxl
    if y_max_dist_pxl > x_max_dist_pxl:
        tile_overlap = y_max_dist_pxl

    class_tiles_dir = os.path.join(tmp_dir, f"ClassTiles_{uid_str}")
    class_tiles_dir_present = True
    if not os.path.exists(class_tiles_dir):
        os.makedirs(class_tiles_dir)
        class_tiles_dir_present = False

    classes_img_tile_base = os.path.join(class_tiles_dir, "ClassImgTile")
    rsgislib.imageutils.create_tiles(
        classes_img,
        classes_img_tile_base,
        tile_size,
        tile_size,
        tile_overlap,
        0,
        "KEA",
        rsgislib.TYPE_32UINT,
        "kea",
    )
    img_tile_files = glob.glob(f"{classes_img_tile_base}*.kea")

    dist_tiles_dir = os.path.join(tmp_dir, f"DistTiles_{uid_str}")
    dist_tiles_dir_present = True
    if not os.path.exists(dist_tiles_dir):
        os.makedirs(dist_tiles_dir)
        dist_tiles_dir_present = False

    prox_options_base = [
        "MAXDIST=" + str(max_dist),
        "DISTUNITS=GEO",
        "NODATA=" + str(no_data_val),
    ]

    for class_id in class_ids:
        print("Class {}".format(class_id))
        proxOptions = list(prox_options_base)
        proxOptions.append("VALUES=" + str(class_id))

        dist_tiles = []
        dist_tile_args = []
        for class_tile_file in img_tile_files:
            base_tile_name = os.path.basename(class_tile_file)
            dist_tile_file = os.path.join(dist_tiles_dir, base_tile_name)
            tile_args = [
                class_tile_file,
                dist_tile_file,
                proxOptions,
                no_data_val,
                "KEA",
            ]
            dist_tiles.append(dist_tile_file)
            dist_tile_args.append(tile_args)

        with multiprocessing.Pool(n_cores) as p:
            p.map(_computeProximityArrArgsFunc, dist_tile_args)

        dist_image = "{}_{}.kea".format(out_img_base, class_id)
        # Mosaic Tiles
        rsgislib.imageutils.create_img_mosaic(
            dist_tiles,
            dist_image,
            no_data_val,
            no_data_val,
            1,
            1,
            "KEA",
            rsgislib.TYPE_32FLOAT,
        )
        rsgislib.imageutils.pop_img_stats(
            dist_image, use_no_data=True, no_data_val=no_data_val, calc_pyramids=True
        )
        for img_file in dist_tiles:
            rsgislib.tools.filetools.delete_file_with_basename(img_file)

    if not class_tiles_dir_present:
        shutil.rmtree(class_tiles_dir, ignore_errors=True)
    else:
        for class_tile_file in img_tile_files:
            rsgislib.tools.filetools.delete_file_with_basename(class_tile_file)

    if not dist_tiles_dir_present:
        shutil.rmtree(dist_tiles_dir, ignore_errors=True)

    if not tmp_present:
        shutil.rmtree(tmp_dir, ignore_errors=True)
    else:
        os.remove(classes_img)


def calc_dist_between_clumps(
    clumps_img: str,
    out_col_name: str,
    tmp_dir: str = "./tmp",
    use_idx: bool = False,
    max_dist_thres: float = 10,
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
    import shutil
    import rsgislib.tools.utils
    import rsgislib.tools.filetools
    import rsgislib.vectorutils

    tmp_present = True
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        tmp_present = False

    base_name = "{}_{}".format(
        rsgislib.tools.filetools.get_file_basename(clumps_img),
        rsgislib.tools.utils.uid_generator(),
    )

    polys_shp = os.path.join(tmp_dir, base_name + "_shp.shp")
    rsgislib.vectorutils.polygoniseRaster(
        clumps_img, polys_shp, imgBandNo=1, maskImg=clumps_img, imgMaskBandNo=1
    )

    print(
        "Calculating Distance - can take some time. Try using index "
        "and decreasing max distance threshold."
    )
    t_obj = rsgislib.RSGISTime()
    t_obj.start(True)
    polys_shp_geom_dist = os.path.join(tmp_dir, base_name + "_dist_shp.shp")
    rsgislib.vectorutils.dist2NearestGeom(
        polys_shp, polys_shp_geom_dist, out_col_name, True, use_idx, max_dist_thres
    )
    t_obj.end()

    import_vec_atts(clumps_img, polys_shp_geom_dist, [out_col_name])

    if not tmp_present:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def calc_dist_to_large_clumps(
    clumps_img: str,
    out_col_name: str,
    size_thres: float,
    tmp_dir: str = "./tmp",
    use_idx: bool = False,
    max_dist_thres: float = 10,
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
    import shutil
    import rsgislib.tools.utils
    import rsgislib.vectorutils
    from rios import rat

    tmp_present = True
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        tmp_present = False

    uid_str = rsgislib.tools.utils.uid_generator()
    base_name = "{}_{}".format(
        rsgislib.tools.filetools.get_file_basename(clumps_img), uid_str
    )

    rat_dataset = gdal.Open(clumps_img, gdal.GA_Update)
    histogram = rat.readColumn(rat_dataset, "Histogram")
    small_units = numpy.zeros_like(histogram, dtype=numpy.int16)
    small_units[histogram < size_thres] = 1
    rat.writeColumn(rat_dataset, "smallUnits", small_units)

    id_vals = numpy.arange(histogram.shape[0])

    sm_units_ids = id_vals[small_units == 1]
    sm_units_ids = sm_units_ids[sm_units_ids > 0]
    lrg_unit_ids = id_vals[small_units == 0]
    lrg_unit_ids = lrg_unit_ids[lrg_unit_ids > 0]

    print("There are {} small clumps.".format(sm_units_ids.shape[0]))
    print("There are {} large clumps.".format(lrg_unit_ids.shape[0]))

    sm_unit_fids = numpy.arange(sm_units_ids.shape[0])
    lrg_unit_fids = numpy.arange(lrg_unit_ids.shape[0])

    sm_unit_clump_ids = numpy.zeros_like(histogram, dtype=numpy.int16)
    sm_unit_clump_ids[sm_units_ids] = sm_unit_fids
    lrg_unit_clump_ids = numpy.zeros_like(histogram, dtype=numpy.int16)
    lrg_unit_clump_ids[lrg_unit_ids] = lrg_unit_fids

    rat.writeColumn(rat_dataset, "SmUnits", sm_unit_clump_ids)
    rat.writeColumn(rat_dataset, "LrgUnits", lrg_unit_clump_ids)
    rat.writeColumn(rat_dataset, "smallUnitsBin", small_units)

    sm_clumps_img = os.path.join(tmp_dir, base_name + "_smclumps.kea")
    export_col_to_gdal_img(
        clumps_img, sm_clumps_img, "KEA", rsgislib.TYPE_32UINT, "SmUnits"
    )
    pop_rat_img_stats(
        clumps_img=sm_clumps_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
    )

    lrg_clumps_img = os.path.join(tmp_dir, base_name + "_lrgclumps.kea")
    export_col_to_gdal_img(
        clumps_img, lrg_clumps_img, "KEA", rsgislib.TYPE_32UINT, "LrgUnits"
    )
    pop_rat_img_stats(
        clumps_img=lrg_clumps_img,
        add_clr_tab=True,
        calc_pyramids=True,
        ignore_zero=True,
    )

    sm_polys_shp = os.path.join(tmp_dir, base_name + "_smClumps_shp.shp")
    rsgislib.vectorutils.polygoniseRaster(
        sm_clumps_img, sm_polys_shp, imgBandNo=1, maskImg=sm_clumps_img, imgMaskBandNo=1
    )

    lgr_polys_shp = os.path.join(tmp_dir, base_name + "_lgrClumps_shp.shp")
    rsgislib.vectorutils.polygoniseRaster(
        lrg_clumps_img,
        lgr_polys_shp,
        imgBandNo=1,
        maskImg=lrg_clumps_img,
        imgMaskBandNo=1,
    )

    print(
        "Calculating Distance - can take some time. Try using index and "
        "decreasing max distance threshold."
    )
    t = rsgislib.RSGISTime()
    t.start(True)
    sm_polys_dist_shp = os.path.join(tmp_dir, base_name + "_smClumps_dist_shp.shp")
    rsgislib.vectorutils.dist2NearestSecGeomSet(
        sm_polys_shp,
        lgr_polys_shp,
        sm_polys_dist_shp,
        out_col_name,
        True,
        use_idx,
        max_dist_thres,
    )
    t.end()
    import_vec_atts(sm_clumps_img, sm_polys_dist_shp, [out_col_name])

    sm_clumps_rat_dataset = gdal.Open(sm_clumps_img, gdal.GA_Update)
    min_dist_col = rat.readColumn(sm_clumps_rat_dataset, out_col_name)

    minDistSmlClumpsArr = numpy.zeros_like(histogram, dtype=numpy.float32)
    minDistSmlClumpsArr[sm_units_ids] = min_dist_col

    rat.writeColumn(rat_dataset, out_col_name, minDistSmlClumpsArr)

    sm_clumps_rat_dataset = None
    rat_dataset = None

    if not tmp_present:
        shutil.rmtree(tmp_dir, ignore_errors=True)
