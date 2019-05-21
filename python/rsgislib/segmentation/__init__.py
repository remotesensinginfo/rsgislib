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

Shepherd, J. D., Bunting, P., & Dymond, J. R. (2019). Operational Large-Scale Segmentation of Imagery Based on Iterative Elimination. Remote Sensing, 11(6), 658. http://doi.org/10.3390/rs11060658

For the wider system of data analysis using segments see the following paper:

Daniel Clewley, Peter Bunting, James Shepherd, Sam Gillingham, Neil Flood, John Dymond, Richard Lucas, John Armston and Mahta Moghaddam. 2014. A Python-Based Open Source System for Geographic Object-Based Image Analysis (GEOBIA) Utilizing Raster Attribute Tables. Remote Sensing. Volume 6, Pages 6111-6135. http://www.mdpi.com/2072-4292/6/7/6111

"""

# import the C++ extension into this level
from ._segmentation import *


