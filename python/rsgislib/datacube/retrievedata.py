#!/usr/bin/env python


############################################################################
#  retrievedata.py
#
#  Copyright 2018 RSGISLib.
#
#  RSGISLib: 'The remote sensing and GIS Software Library'
#
#  RSGISLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  RSGISLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
#
#
# Purpose: Provide a set of function for accessing data in an instance of
#          open data cube.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 04/06/2018
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
############################################################################


"""
The datacube.retrievedata module contains functions for retreving data from a data cube instance.
"""

haveDC = True
try:
    import datacube
except ImportError as dcErr:
    haveDC = False

from osgeo import gdal


def getSingleTimeDataProd(dataProd, prodMeasurements, startDate, endDate, minLat, maxLat, minLon, maxLon, outImgFile, gdalformat='KEA', gdalDataType=gdal.GDT_Byte, gdalOptions=[]):
    """
A function to extract data from an instance of the open data cube (https://www.opendatacube.org) and save it to a GDAL supported file format.
Note. this function only saves files where only a single data layer is returned for the time period specified. 

* dataProd - provide the name of the data product to be extracted.
* prodMeasurements - provide input array of measurements
* startDate - start period (YYYY-MM-DD)
* endDate - end period (YYYY-MM-DD)
* minLat - minimum latitude for the query bounding box
* maxLat - maximum latitude for the query bounding box
* minLon - minimum longditude for the query bounding box
* maxLon - maximum longditude for the query bounding box
* outImgFile - file path and name for the output image file.
* gdalformat - output file format (Default: 'KEA'). Must support GDAL create function.
* gdalDataType - output file datatype (Default: gdal.GDT_Byte)
* gdalOptions - array of options for gdal file creation (e.g., for GeoTIFF ["TILED=YES", "COMPRESS=DEFLATE"]). Default: []

Example::

    getSingleTimeDataProd('fc_percentile_albers_annual', ['PV_PC_10', 'NPV_PC_10'], '2015-01-01', '2015-12-31', -24.9, -24.8, 142.5, 142.6, 
                          'FC_Percent_PV_NPV_2015.kea', gdalformat='KEA', gdalDataType=gdal.GDT_Byte, gdalOptions=[])

"""
    
    if not haveDC:
        raise Exception("The datacube module required for this function could not be imported\n\t" + dcErr)
    
    dc = datacube.Datacube(app='ExtractDataRSGISLib')
    
    query = {'time': (startDate, endDate),}
    query['x'] = (minLon, maxLon)
    query['y'] = (maxLat, minLat)
    query['crs'] = 'EPSG:4326'
    
    dataMeasurements = dc.load(product=dataProd, group_by='solar_day', measurements=prodMeasurements, **query)
    
    if not bool(dataMeasurements):
        raise Exception('Did not retrieve any measurements.')
    
    if dataMeasurements.dims['time'] != 1:
        raise Exception('This function can only handle measurements with one time period.')
    
    
    xt = dataMeasurements.dims['x']
    yt = dataMeasurements.dims['y']
    nBands = len(prodMeasurements)
    
    crswkt = dataMeasurements.crs.wkt
    affine = dataMeasurements.affine
    
    xres = affine[0]
    yres = affine[4]
        
    # Set the geotransform properties
    xcoord = affine[2]
    ycoord = affine[5]
    geotransform = (xcoord - (xres*0.5), xres, 0, ycoord + (yres*0.5), 0, yres)
        
    targetImgDS = gdal.GetDriverByName(gdalformat).Create(outImgFile, xt, yt, nBands, gdalDataType, options=gdalOptions)
    targetImgDS.SetGeoTransform(geotransform)
    targetImgDS.SetProjection(crswkt)
    
    idx = 0
    for pMeasure in prodMeasurements:
        band = targetImgDS.GetRasterBand(idx+1)
        band.SetNoDataValue(dataMeasurements[pMeasure].nodata)
        band.WriteArray(dataMeasurements[pMeasure].data[0])
        band.SetDescription(str(pMeasure))
        band = None
        idx = idx + 1
    
    targetImgDS = None

