"""
The segmentation module contains the segmentation functionality for RSGISLib.

The segmentation uses the algorithm descriped in:

Shepherd, J., Bunting, P., Dymond, J., 2013. Operational large-scale segmentation of imagery based on iterative elimination. Journal of Applied Remote Sensing. Submitted.

To segment an image requires a number of commands to be run in sequence.

Example::

    from rsgislib import segmentation
    from rsgislib import imagecalc
    
    # Perform k-means clustering (subsampling data to reduce memory usage).
    imagecalc.kMeansClustering(inImage, outMatrixFile, numClusters, 200, 100, True, 0.0025, rsgislib.imagecalc.INITCLUSTER_DIAGONAL_FULL_ATTACH)
    
    # Apply to image
    segmentation.labelPixelsFromClusterCentres(inImage, kMeansFileZones, outMatrixFile+str(".gmtxt"), True, gdalFormat)
    
    # Eliminate single pixels
    rsgislib.segmentation.eliminateSinglePixels(inImage, kMeansFileZones, kMeansFileZonesNoSgls, kMeansFileZonesNoSglsTmp, gdalFormat, False, True)
    
    # Perform clump
    segmentation.clump(kMeansFileZonesNoSgls, initClumpsFile, gdalFormat, False, 0)
    
    # Eliminate small pixels
    segmentation.RMSmallClumpsStepwise(inImage, initClumpsFile, elimClumpsFile, gdalFormat, False, "", False, False, minPxls, distThres)
    
    # Relabel clumps
    segmentation.relabelClumps(elimClumpsFile, outputClumps, gdalFormat, False)

Where 'inImage' is the input image (optionally masked and stretched) and 'outputClumps' is the output clumps file.

"""

# import the C++ extension into this level
from ._segmentation import *
import warnings

def UnionOfClumps(outputimage, gdalformat, inputimagepaths, nodata):
    """
    Deprecated: is now 'unionOfClumps' (note starts with lower case 'u')
    """
    warnings.warn("'UnionOfClumps' has been renamed to 'unionOfClumps' (starts with lower case 'u'), please update your code.", Warning, stacklevel=2)
    unionOfClumps(outputimage, gdalformat, inputimagepaths, nodata, False)


def RMSmallClumpsStepwise(inputimage, clumpsimage, outputimage, gdalformat, stretchstatsavail, stretchstatsfile, storemean, processinmemory, minclumpsize, specThreshold):
    """
    Deprecated: is now 'rmSmallClumpsStepwise' (note starts with lower case 'rm')
    """
    warnings.warn("'RMSmallClumpsStepwise' has been renamed to 'rmSmallClumpsStepwise' (starts with lower case 'rm'), please update your code.", Warning, stacklevel=2)
    rmSmallClumpsStepwise(inputimage, clumpsimage, outputimage, gdalformat, stretchstatsavail, stretchstatsfile, storemean, processinmemory, minclumpsize, specThreshold)
    
