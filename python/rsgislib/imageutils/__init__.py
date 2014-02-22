"""
The imageutils module contains general utilities for applying to images.
"""

# import the C++ extension into this level
from ._imageutils import *

haveGDALPy = True
try:
    import osgeo.gdal as gdal
except ImportError as gdalErr:
    haveGDALPy = False

def setBandNames(inputImage, bandNames):
    """A utility function to set band names.
Where:

* inImage is the input image
* bandNames is a list of band names

Example::

    from rsgislib import imageutils

    inputImage = 'injune_p142_casi_sub_utm.kea'
    bandNames = ['446nm','530nm','549nm','569nm','598nm','633nm','680nm','696nm','714nm','732nm','741nm','752nm','800nm','838nm']
    
    imageutils.setBandNames(inputImage, bandNames)
    

"""
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported" + gdalErr)

    print('Set band names.')
 
    dataset = gdal.Open(inputImage, gdal.GA_Update)
    
    for i in range(len(bandNames)):
        band = i+1
        bandName = bandNames[i]

        imgBand = dataset.GetRasterBand(band)
        # Check the image band is available
        if not imgBand is None:
            print('Setting Band {0} to "{1}"'.format(band, bandName))
            imgBand.SetDescription(bandName)
        else:
            raise Exception("Could not open the image band: ", band)

