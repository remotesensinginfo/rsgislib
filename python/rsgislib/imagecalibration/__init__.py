"""
The image calibration module contains functions for calibrating optical data from DN to radience and top of atmosphere reflectance and, using coefficients from 6S, surface reflectance.

For obtaining the correct parameters for each function it is recomented that rather than running directly they are called through the Atmospheric and Radiometric Correction of Satellite Imagery (ARCSI) software. 

More details on ARCSI are avaialble from http://rsgislib.org/arcsi

"""

# import the C++ extension into this level
from ._imagecalibration import *
