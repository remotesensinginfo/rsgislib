"""
The image registration module contains algorithms for generating tie points matching two image and warping images based on tie points.

There are two algorithms are available for registration: basic, and singlelayer. The single layer algorithm is a simplified version of the algorithm proposed in:

Bunting, P.J., Labrosse, F. & Lucas, R.M., 2010. A multi-resolution area-based technique for automatic multi-modal image registration. Image and Vision Computing, 28(8), pp.1203-1219.
"""

# import the C++ extension into this level
from ._imageregistration import *

METRIC_EUCLIDEAN = 1
METRIC_SQDIFF = 2
METRIC_MANHATTEN = 3
METRIC_CORELATION = 4

TYPE_ENVI_IMG2IMG = 1
TYPE_ENVI_IMG2MAP = 2
TYPE_RSGIS_IMG2MAP = 3
TYPE_RSGIS_MAPOFFS = 4
