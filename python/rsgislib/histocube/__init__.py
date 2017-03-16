"""
The histocube module provides histogram cube functionality within RSGISLib.

Histogram Summary Statistics:

    * SUMTYPE_HC_UNDEFINED = 0
    * SUMTYPE_HC_MIN = 1
    * SUMTYPE_HC_MAX = 2
    * SUMTYPE_HC_MEAN = 3
    * SUMTYPE_HC_MEDIAN = 4
    * SUMTYPE_HC_RANGE = 5
    * SUMTYPE_HC_STDDEV = 6
    * SUMTYPE_HC_SUM = 7
    * SUMTYPE_HC_MODE = 8

"""

# import the C++ extension into this level
from ._histocube import *


SUMTYPE_HC_UNDEFINED = 0
SUMTYPE_HC_MIN = 1
SUMTYPE_HC_MAX = 2
SUMTYPE_HC_MEAN = 3
SUMTYPE_HC_MEDIAN = 4
SUMTYPE_HC_RANGE = 5
SUMTYPE_HC_STDDEV = 6
SUMTYPE_HC_SUM = 7
SUMTYPE_HC_MODE = 8

