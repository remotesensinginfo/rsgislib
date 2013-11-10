"""
Contains image filtering functionality for rsgislib
"""

# import the C++ extension into this level
from ._imagefilter import *

class FilterParameters:
    """ Object, specifying the type of filter and filter parameters """
    def __init__(self, filterType, fileEnding, size = 3, option = None, nLooks = None, stddev = None, stddevX = None, stddevY = None, angle = None):
        self.filterType = filterType
        self.fileEnding = fileEnding
        self.size = size
        self.option = option
        self.nLooks = nLooks
        self.stddev = stddev
        self.stddevX = stddevX
        self.stddevY = stddevY
        self.angle = angle
