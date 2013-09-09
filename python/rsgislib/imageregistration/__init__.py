"""
Contains image registration functionality for rsgislib
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
