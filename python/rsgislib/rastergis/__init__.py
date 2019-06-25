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
    """ This is passed to the populateRATWithStats function """
    def __init__(self, band, minField=None, maxField=None, sumField=None, stdDevField=None, meanField=None):
        self.band = band
        self.minField = minField
        self.maxField = maxField
        self.sumField = sumField
        self.meanField = meanField
        self.stdDevField = stdDevField
        
class FieldAttStats:
    """ This is passed to the calcRelDiffNeighStats function """
    def __init__(self, field, minField=None, maxField=None, sumField=None, stdDevField=None, meanField=None):
        self.field = field
        self.minField = minField
        self.maxField = maxField
        self.sumField = sumField
        self.meanField = meanField
        self.stdDevField = stdDevField

class BandAttPercentiles:
    """ This is passed to the populateRATWithPercentiles function """
    def __init__(self, percentile, fieldName):
        self.percentile = percentile
        self.fieldName = fieldName

class ShapeIndex:
    """ This is passed to the calcShapeIndices function """
    def __init__(self, colName, idx, colIdx=0):
        self.colName = colName
        self.colIdx = colIdx
        self.idx = idx

def exportCols2GDALImage(clumps, outimage, gdalformat, datatype, fields, ratband=1, tempDIR=None):
    """Exports columns of the raster attribute table as bands in a GDAL image. Utility function, exports each column individually then stacks them.

Where:

* clumps is a string containing the name of the input image file with RAT
* outimage is a string containing the name of the output gdal file
* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'
* datatype is an int containing one of the values from rsgislib.TYPE_*
* field is a list of strings, providing the names of the column to be exported.
* ratband is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.

Example::

   clumps='./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea'
   outimage='./TestOutputs/RasterGIS/injune_p142_casi_rgb_export.kea'
   gdalformat = 'KEA'
   datatype = rsgislib.TYPE_32FLOAT
   fields = ['RedAvg','GreenAvg','BlueAvg']
   rastergis.exportCols2GDALImage(clumps, outimage, gdalformat, datatype, fields)

"""
    import os
    import rsgislib
    from rsgislib import imageutils

    rsgisUtils = rsgislib.RSGISPyUtils()

    if tempDIR is None:
        tempDIR = os.path.split(outimage)[0]

    outExt=os.path.splitext(outimage)[-1]
    tempFileList = []

    # Export each field
    for field in fields:
        print('Exporting: ' + field)
        outTempFile = os.path.join(tempDIR, field + outExt)
        exportCol2GDALImage(clumps, outTempFile, gdalformat, datatype, field, ratband=1)
        tempFileList.append(outTempFile)

    # Stack Bands
    print('Stacking Bands')
    imageutils.stackImageBands(tempFileList, fields, outimage, None, 0, gdalformat, datatype)

    # Remove temp files
    print('Removing temp files')
    for tempFile in tempFileList:
        rsgisUtils.deleteFileWithBasename(tempFile)


def getRATLength(clumps_img, rat_band=1):
    """
A function which returns the length (i.e., number of rows) within the RAT.

* clumps_img - path to the image file with the RAT
* rat_band - the band within the image file for which the RAT is to read.

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


def getRATColumns(clumps_img, rat_band=1):
    """
A function which returns a list of column names within the RAT.

* clumps_img - path to the image file with the RAT
* rat_band - the band within the image file for which the RAT is to read.

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


def getRATColumnsInfo(clumps_img, rat_band=1):
    """
A function which returns a dictionary of column names with type (GFT_Integer, GFT_Real, GFT_String)
and usage (e.g., GFU_Generic, GFU_PixelCount, GFU_Name, etc.) within the RAT.

* clumps_img - path to the image file with the RAT
* rat_band - the band within the image file for which the RAT is to read.

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
        col_info[col_name]['type'] = col_type
        col_info[col_name]['usage'] = col_usage

    clumps_img_ds = None
    return col_info


def readRATNeighbours(clumps_img, start_row=None, end_row=None, rat_band=1):
    """
A function which returns a list of clumps neighbours from a KEA RAT. Note, the
neighbours are popualted using the function rsgislib.rastergis.findNeighbours. 
By default the whole datasets of neightbours is read to memory but the start_row 
and end_row variables can be used to read a subset of the RAT.

* clumps_img - path to the image file with the RAT
* start_row - the row within the RAT to start reading, if None will start at 0 (Default: None).
* end_row - the row within the RAT to end reading, if None will end at n_rows within the RAT. (Default: None)
* rat_band - the band within the image file for which the RAT is to read.

"""
    if not haveHDF5:
        raise Exception("Need the h5py library for this function")
       
    # Check that 'NumNeighbours' column exists
    rat_columns = getRATColumns(clumps_img, rat_band)
    if 'NumNeighbours' not in rat_columns:
        raise Exception("Clumps image RAT does not contain 'NumNeighbours' column - have you populated neightbours?")
    
    n_rows = getRATLength(clumps_img)
    
    if start_row is None:
        start_row = 0
    
    if end_row is None:
        end_row = n_rows
    
    clumps_h5_file = h5py.File(clumps_img)
    neighbours_path = 'BAND{}/ATT/NEIGHBOURS/NEIGHBOURS'.format(rat_band)
    neighbours = clumps_h5_file[neighbours_path]
    neighbours_data = neighbours[start_row:end_row]
    clumps_h5_file = None
    return neighbours_data



