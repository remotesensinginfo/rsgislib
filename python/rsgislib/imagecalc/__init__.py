"""
Contains ImageCalc functionality for rsgislib
"""

# import the C++ extension into this level
from ._imagecalc import *

# define our own classes
class BandDefn(object):
    """
    Create a list of these objects to pass to the bandMath function
    as the 'bands' parameter.
    """
    def __init__(self, bandName=None, fileName=None, bandIndex=None):
        self.bandName = bandName
        self.fileName = fileName
        self.bandIndex = bandIndex



