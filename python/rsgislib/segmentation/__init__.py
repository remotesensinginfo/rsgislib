"""
The segmentation module contains the segmentation functionality for RSGISLib.

A number of steps are required for the segmentation, for most users it is recommended to use the `runShepherdSegmentation`
helper function which will run all the required steps to generate a segmentation:

Example::

    from rsgislib.segmentation import segutils
    
    segutils.runShepherdSegmentation(inImage,
                                     outputClumps,
                                     tmpath='./',
                                     numClusters=60,
                                     minPxls=100,
                                     distThres=100,
                                     sampling=100, kmMaxIter=200)

Where 'inImage' is the input image (optionally masked and stretched) and 'outputClumps' is the output clumps file.

More information about the segmentation method is available in the following paper:

Daniel Clewley, Peter Bunting, James Shepherd, Sam Gillingham, Neil Flood, John Dymond, Richard Lucas, John Armston and Mahta Moghaddam. 2014. A Python-Based Open Source System for Geographic Object-Based Image Analysis (GEOBIA) Utilizing Raster Attribute Tables. Remote Sensing. Volume 6, Pages 6111-6135. http://www.mdpi.com/2072-4292/6/7/6111

"""

# import the C++ extension into this level
from ._segmentation import *
#import warnings
#
#
#def UnionOfClumps(outputimage, gdalformat, inputimagepaths, nodata):
#    """
#    Deprecated: is now 'unionOfClumps' (note starts with lower case 'u')
#    """
#    warnings.warn("'UnionOfClumps' has been renamed to 'unionOfClumps' (starts with lower case 'u'), please update your code.", Warning, stacklevel=2)
#    unionOfClumps(outputimage, gdalformat, inputimagepaths, nodata, False)
#
#
#def RMSmallClumpsStepwise(inputimage, clumpsimage, outputimage, gdalformat, stretchstatsavail, stretchstatsfile, storemean, processinmemory, minclumpsize, specThreshold):
#    """
#    Deprecated: is now 'rmSmallClumpsStepwise' (note starts with lower case 'rm')
#    """
#    warnings.warn("'RMSmallClumpsStepwise' has been renamed to 'rmSmallClumpsStepwise' (starts with lower case 'rm'), please update your code.", Warning, stacklevel=2)
#    rmSmallClumpsStepwise(inputimage, clumpsimage, outputimage, gdalformat, stretchstatsavail, stretchstatsfile, storemean, processinmemory, minclumpsize, specThreshold)
#    
