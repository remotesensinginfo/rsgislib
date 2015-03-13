"""
The Raster GIS module contains functions for attributing and manipulating raster attribute tables.
"""

# import the C++ extension into this level
from ._rastergis import *

haveGDALPy = True
try:
    import osgeo.gdal as gdal
except ImportError as gdalErr:
    haveGDALPy = False

haveNumpy = True
try:
    import numpy
except ImportError as numErr:
    haveNumpy = False

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








