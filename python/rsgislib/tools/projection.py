#!/usr/bin/env python
"""
The tools.projection module contains some useful tools for working with projections.
"""

import numpy
import osgeo.gdal as gdal
import osgeo.osr as osr


def getEPSGCodeFromWKT(wkt_str):
    """
    Using GDAL to return the EPSG code for inputted WKT string.

    :return: the EPSG code.

    """
    epsg_code = None
    try:
        spatRef = osr.SpatialReference()
        spatRef.ImportFromWkt(wkt_str)
        spatRef.AutoIdentifyEPSG()
        epsg_code = spatRef.GetAuthorityCode(None)
    except Exception:
        epsg_code = None
    return epsg_code


def getWKTFromEPSGCode(epsg_code):
    """
    Using GDAL to return the WKT string for inputted EPSG Code.

    :param epsg_code: integer variable of the epsg code.

    :return: string with WKT representation of the projection.

    """
    wkt_str = None
    try:
        spatRef = osr.SpatialReference()
        spatRef.ImportFromEPSG(epsg_code)
        wkt_str = spatRef.ExportToWkt()
    except Exception:
        wkt_str = None
    return wkt_str


def get_osr_prj_obj(epsg_code):
    """
    A function which returns an OSR SpatialReference object
    for a given EPSG code.

    :param epsg_code: An EPSG code for the projection. Must be an integer.
    :return: osr.SpatialReference object.

    """
    spat_ref = osr.SpatialReference()
    spat_ref.ImportFromEPSG(int(epsg_code))
    return spat_ref


def degrees_to_metres(latitude, lon_size, lat_size):
    """ 
Convert pixel sizes or distances in degrees to metres.

Where:

:param latitude: latitude
:param lon_size: numpy array of x pixel sizes (degrees)
:param lat_size: numpy array of y pixel sizes (degrees)

:return: x_size (numpy array of x pixel sizes (m)), y_size (numpy array of y pixel sizes (m))

Example::

    from rsgislib import tools
    x_size, y_size = tools.degrees_to_metres(52,1.0,1.0)

    """

    # Set up parameters for ellipse
    # Semi-major and semi-minor for WGS-84 ellipse
    ellipse = [6378137.0, 6356752.314245]
    
    radlat = numpy.deg2rad(latitude)
    
    Rsq = (ellipse[0]*numpy.cos(radlat))**2+(ellipse[1]*numpy.sin(radlat))**2
    Mlat = (ellipse[0]*ellipse[1])**2/(Rsq**1.5)
    Nlon = ellipse[0]**2/numpy.sqrt(Rsq)
    x_size = numpy.pi/180*numpy.cos(radlat)*Nlon*lon_size
    y_size = numpy.pi/180*Mlat*lat_size

    return x_size, y_size


def metres_to_degrees(latitude, x_size, y_size):
    """ 
Convert pixel sizes or distances in metres to degrees.

Where:

:param latitude: latitude
:param x_size: numpy array of x pixel sizes (m)
:param y_size: numpy array of y pixel sizes (m)

:return: lon_size (numpy array of x pixel sizes (degrees)), lat_size (numpy array of y pixel sizes (degrees))

Example::

    from rsgislib import tools
    x_size, y_size = tools.metres_to_degrees(52, 1.0, 1.0)
    
    """

    # Set up parameters for ellipse
    # Semi-major and semi-minor for WGS-84 ellipse
    ellipse = [6378137.0, 6356752.314245]
    
    radlat = numpy.deg2rad(latitude)
    
    Rsq = (ellipse[0]*numpy.cos(radlat))**2+(ellipse[1]*numpy.sin(radlat))**2
    Mlat = (ellipse[0]*ellipse[1])**2/(Rsq**1.5)
    Nlon = ellipse[0]**2/numpy.sqrt(Rsq)
    lon_size = x_size / (numpy.pi/180*numpy.cos(radlat)*Nlon)
    lat_size = y_size / (numpy.pi/180*Mlat)

    return lon_size, lat_size


