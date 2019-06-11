"""
The tools.projection module contains some useful tools for working with projections.
"""

# Import numpy
import numpy

haveGDALPy = True
try:
    import osgeo.gdal as gdal
except ImportError as gdalErr:
    haveGDALPy = False
   
def degrees_to_metres(latitude, lonsize, latsize):
    """ 
Convert pixel sizes or distances in degrees to metres.

Where:

* latitude - latitude
* lonsize - numpy array of x pixel sizes (degrees)
* latsize - numpy array of y pixel sizes (degrees)

Returns:

* xsize - numpy array of x pixel sizes (m)
* ysize - numpy array of y pixel sizes (m)

Example::

    from rsgislib import tools
    xsize, ysize = tools.degrees_to_metres(52,1.0,1.0)
    

    """

    # Set up parameters for ellipse
    # Semi-major and semi-minor for WGS-84 ellipse
    ellipse = [6378137.0, 6356752.314245]
    
    radlat = numpy.deg2rad(latitude)
    
    Rsq = (ellipse[0]*numpy.cos(radlat))**2+(ellipse[1]*numpy.sin(radlat))**2
    Mlat = (ellipse[0]*ellipse[1])**2/(Rsq**1.5)
    Nlon = ellipse[0]**2/numpy.sqrt(Rsq)
    xsize = numpy.pi/180*numpy.cos(radlat)*Nlon*lonsize
    ysize = numpy.pi/180*Mlat*latsize

    return xsize, ysize


def metres_to_degrees(latitude, xsize, ysize):
    """ 
Convert pixel sizes or distances in metres to degrees.

Where:

* latitude - latitude
* xsize - numpy array of x pixel sizes (m)
* ysize - numpy array of y pixel sizes (m)

Returns:

* lonsize - numpy array of x pixel sizes (degrees)
* latsize - numpy array of y pixel sizes (degrees)

Example::

    from rsgislib import tools
    xsize, ysize = tools.metres_to_degrees(52,1.0,1.0)
    
    """

    # Set up parameters for ellipse
    # Semi-major and semi-minor for WGS-84 ellipse
    ellipse = [6378137.0, 6356752.314245]
    
    radlat = numpy.deg2rad(latitude)
    
    Rsq = (ellipse[0]*numpy.cos(radlat))**2+(ellipse[1]*numpy.sin(radlat))**2
    Mlat = (ellipse[0]*ellipse[1])**2/(Rsq**1.5)
    Nlon = ellipse[0]**2/numpy.sqrt(Rsq)
    lonsize = xsize / (numpy.pi/180*numpy.cos(radlat)*Nlon)
    latsize = ysize / (numpy.pi/180*Mlat)

    return lonsize, latsize


