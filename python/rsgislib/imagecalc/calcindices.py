#! /usr/bin/env python
############################################################################
#  calcindices.py
#
#  Copyright 2017 RSGISLib.
#
#  RSGISLib: 'The Remote Sensing and GIS Software Library'
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
# Purpose:  Provides helper functions for calculating commmon image band 
#           indices.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 03/10/2017
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

import rsgislib
import rsgislib.imageutils
import rsgislib.imagecalc


def calcNDVI(image, rBand, nBand, outImage, stats=True, gdalformat='KEA'):
    """ 
Helper function to calculate NDVI ((NIR-RED)/(NIR+RED)), note the output no data value is -999.

:param image: is a string specifying the input image file.
:param rBand: is an int specifying the red band in the input image (band indexing starts at 1)
:param nBand: is an int specifying the nir band in the input image (band indexing starts at 1)
:param outImage: is a string specifying the output image file.
:param stats: is a boolean specifying whether pyramids and stats should be calculated (Default: True)
:param gdalformat: is a string specifing the output image file format (Default: KEA)
    
"""
    expression = '(nir+red)!=0?(nir-red)/(nir+red):-999'
    bandDefns = []
    bandDefns.append(rsgislib.imagecalc.BandDefn('red', image, rBand))
    bandDefns.append(rsgislib.imagecalc.BandDefn('nir', image, nBand))
    rsgislib.imagecalc.bandMath(outImage, expression, gdalformat, rsgislib.TYPE_32FLOAT, bandDefns)
    # Set no data value
    rsgislib.RSGISPyUtils().setImageNoDataValue(outImage, -999)
    if stats:
        rsgislib.imageutils.popImageStats(outImage,False,-999.,True)


def calcWBI(image, bBand, nBand, outImage, stats=True, gdalformat='KEA'):
    """ 
Helper function to calculate WBI (Blue/NIR), note the output no data value is -999.

:param image: is a string specifying the input image file.
:param bBand: is an int specifying the blue band in the input image (band indexing starts at 1)
:param nBand: is an int specifying the nir band in the input image (band indexing starts at 1)
:param outImage: is a string specifying the output image file.
:param stats: is a boolean specifying whether pyramids and stats should be calculated (Default: True)
:param gdalformat: is a string specifing the output image file format (Default: KEA)
    
"""
    expression = 'nir!=0?blue/nir:-999'
    bandDefns = []
    bandDefns.append(rsgislib.imagecalc.BandDefn('blue', image, bBand))
    bandDefns.append(rsgislib.imagecalc.BandDefn('nir', image, nBand))
    rsgislib.imagecalc.bandMath(outImage, expression, gdalformat, rsgislib.TYPE_32FLOAT, bandDefns)
    # Set no data value
    rsgislib.RSGISPyUtils().setImageNoDataValue(outImage, -999)
    if stats:
        rsgislib.imageutils.popImageStats(outImage,False,-999.,True)


def calcNDWI(image, nBand, sBand, outImage, stats=True, gdalformat='KEA'):
    """ 
Helper function to calculate NDWI ((NIR-SWIR)/(NIR+SWIR)), note the output no data value is -999.

See: Xu, H. (2006). Modification of normalised difference water index (NDWI) to enhance open water 
                    features in remotely sensed imagery. International Journal of Remote Sensing, 
                    27(14), 3025–3033. http://doi.org/10.1080/01431160600589179
    
Where:

:param image: is a string specifying the input image file.
:param nBand: is an int specifying the nir band in the input image (band indexing starts at 1)
:param sBand: is an int specifying the swir band (e.g., Landsat TM Band 5) in the input image (band indexing starts at 1)
:param outImage: is a string specifying the output image file.
:param stats: is a boolean specifying whether pyramids and stats should be calculated (Default: True)
:param gdalformat: is a string specifing the output image file format (Default: KEA)
    
"""
    expression = '(nir+swir)!=0?(nir-swir)/(nir+swir):-999'
    bandDefns = []
    bandDefns.append(rsgislib.imagecalc.BandDefn('swir', image, sBand))
    bandDefns.append(rsgislib.imagecalc.BandDefn('nir', image, nBand))
    rsgislib.imagecalc.bandMath(outImage, expression, gdalformat, rsgislib.TYPE_32FLOAT, bandDefns)
    # Set no data value
    rsgislib.RSGISPyUtils().setImageNoDataValue(outImage, -999)
    if stats:
        rsgislib.imageutils.popImageStats(outImage,False,-999.,True)



def calcGNDWI(image, gBand, nBand, outImage, stats=True, gdalformat='KEA'):
    """ 
Helper function to calculate NDWI ((GREEN-NIR)/(GREEN+NIR)), note the output no data value is -999.

See: Xu, H. (2006). Modification of normalised difference water index (NDWI) to enhance open water 
                    features in remotely sensed imagery. International Journal of Remote Sensing, 
                    27(14), 3025–3033. http://doi.org/10.1080/01431160600589179

Where:

:param image: is a string specifying the input image file.
:param gBand: is an int specifying the green band in the input image (band indexing starts at 1)
:param nBand: is an int specifying the nir band in the input image (band indexing starts at 1)
:param outImage: is a string specifying the output image file.
:param stats: is a boolean specifying whether pyramids and stats should be calculated (Default: True)
:param gdalformat: is a string specifing the output image file format (Default: KEA)
    
"""
    expression = '(green+nir)!=0?(green-nir)/(green+nir):-999'
    bandDefns = []
    bandDefns.append(rsgislib.imagecalc.BandDefn('green', image, gBand))
    bandDefns.append(rsgislib.imagecalc.BandDefn('nir', image, nBand))
    rsgislib.imagecalc.bandMath(outImage, expression, gdalformat, rsgislib.TYPE_32FLOAT, bandDefns)
    # Set no data value
    rsgislib.RSGISPyUtils().setImageNoDataValue(outImage, -999)
    if stats:
        rsgislib.imageutils.popImageStats(outImage,False,-999.,True)


def calcGMNDWI(image, gBand, sBand, outImage, stats=True, gdalformat='KEA'):
    """ 
Helper function to calculate NDWI ((GREEN-SWIR)/(GREEN+SWIR)), note the output no data value is -999.

See: Xu, H. (2006). Modification of normalised difference water index (NDWI) to enhance open water 
                    features in remotely sensed imagery. International Journal of Remote Sensing, 
                    27(14), 3025–3033. http://doi.org/10.1080/01431160600589179

Where:

:param image: is a string specifying the input image file.
:param gBand: is an int specifying the green band in the input image (band indexing starts at 1)
:param sBand: is an int specifying the swir band (e.g., Landsat TM Band 5) in the input image (band indexing starts at 1)
:param outImage: is a string specifying the output image file.
:param stats: is a boolean specifying whether pyramids and stats should be calculated (Default: True)
:param gdalformat: is a string specifing the output image file format (Default: KEA)
    
"""
    expression = '(green+swir)!=0?(green-swir)/(green+swir):-999'
    bandDefns = []
    bandDefns.append(rsgislib.imagecalc.BandDefn('green', image, gBand))
    bandDefns.append(rsgislib.imagecalc.BandDefn('swir', image, sBand))
    rsgislib.imagecalc.bandMath(outImage, expression, gdalformat, rsgislib.TYPE_32FLOAT, bandDefns)
    # Set no data value
    rsgislib.RSGISPyUtils().setImageNoDataValue(outImage, -999)
    if stats:
        rsgislib.imageutils.popImageStats(outImage,False,-999.,True)

       
def calcWhiteness(image, bBand, gBand, rBand, outImage, stats=True, gdalformat='KEA'):
    """ 
Helper function to calculate whiteness, note the output no data value is -999.
    
Where:

:param image: is a string specifying the input image file.
:param bBand: is an int specifying the blue band in the input image (band indexing starts at 1)
:param gBand: is an int specifying the green band in the input image (band indexing starts at 1)
:param rBand: is an int specifying the red band in the input image (band indexing starts at 1)
:param outImage: is a string specifying the output image file.
:param stats: is a boolean specifying whether pyramids and stats should be calculated (Default: True)
:param gdalformat: is a string specifing the output image file format (Default: KEA)
    
"""
    expression = '(blue+green+red)!=0?(abs(blue-((blue+green+red)/3)) + abs(green-((blue+green+red)/3)) + abs(red-((blue+green+red)/3)))/((blue+green+red)/3):-999'
    bandDefns = []
    bandDefns.append(rsgislib.imagecalc.BandDefn('blue', image, bBand))
    bandDefns.append(rsgislib.imagecalc.BandDefn('green', image, gBand))
    bandDefns.append(rsgislib.imagecalc.BandDefn('red', image, rBand))
    rsgislib.imagecalc.bandMath(outImage, expression, gdalformat, rsgislib.TYPE_32FLOAT, bandDefns)
    # Set no data value
    rsgislib.RSGISPyUtils().setImageNoDataValue(outImage, -999)
    if stats:
        rsgislib.imageutils.popImageStats(outImage,False,-999.,True)

def calcBrightness(image, bBand, gBand, rBand, outImage, stats=True, gdalformat='KEA', scalefac=1000):
    """ 
Helper function to calculate visable brightness, note the output no data value is -999.
    
Where:

:param image: is a string specifying the input image file.
:param bBand: is an int specifying the blue band in the input image (band indexing starts at 1)
:param gBand: is an int specifying the green band in the input image (band indexing starts at 1)
:param rBand: is an int specifying the red band in the input image (band indexing starts at 1)
:param outImage: is a string specifying the output image file.
:param stats: is a boolean specifying whether pyramids and stats should be calculated (Default: True)
:param gdalformat: is a string specifing the output image file format (Default: KEA)
:param scalefac: is a float which can be used retirved reflectance between 0-1 (Default: 1000 to match rsgislib/arcsi)
    
"""
    expression = '(blue+green+red)!=0?((blue/'+str(scalefac)+')+(green/'+str(scalefac)+')+(red/'+str(scalefac)+'))/3:-999'
    bandDefns = []
    bandDefns.append(rsgislib.imagecalc.BandDefn('blue', image, bBand))
    bandDefns.append(rsgislib.imagecalc.BandDefn('green', image, gBand))
    bandDefns.append(rsgislib.imagecalc.BandDefn('red', image, rBand))
    rsgislib.imagecalc.bandMath(outImage, expression, gdalformat, rsgislib.TYPE_32FLOAT, bandDefns)
    # Set no data value
    rsgislib.RSGISPyUtils().setImageNoDataValue(outImage, -999)
    if stats:
        rsgislib.imageutils.popImageStats(outImage,False,-999.,True)
        

def calcBrightnessScaled(image, bBand, gBand, rBand, outImage, stats=True, gdalformat='KEA'):
    """ 
Helper function to calculate visable brightness, note the output no data value is -999.
    
Where:

:param image: is a string specifying the input image file.
:param bBand: is an int specifying the blue band in the input image (band indexing starts at 1)
:param gBand: is an int specifying the green band in the input image (band indexing starts at 1)
:param rBand: is an int specifying the red band in the input image (band indexing starts at 1)
:param outImage: is a string specifying the output image file.
:param stats: is a boolean specifying whether pyramids and stats should be calculated (Default: True)
:param gdalformat: is a string specifing the output image file format (Default: KEA)
    
"""
    rsgisUtils = rsgislib.RSGISPyUtils()
    uidStr = rsgisUtils.uidGenerator()
    tmpImg = os.path.splitext(outImage)[0]+'_tmp'+uidStr+'.'+rsgisUtils.getFileExtension(gdalformat)
    expression = '(blue+green+red)!=0?((blue/'+str(scalefac)+')+(green/'+str(scalefac)+')+(red/'+str(scalefac)+'))/3:-999'
    bandDefns = []
    bandDefns.append(rsgislib.imagecalc.BandDefn('blue', image, bBand))
    bandDefns.append(rsgislib.imagecalc.BandDefn('green', image, gBand))
    bandDefns.append(rsgislib.imagecalc.BandDefn('red', image, rBand))
    rsgislib.imagecalc.bandMath(tmpImg, expression, gdalformat, rsgislib.TYPE_32FLOAT, bandDefns)
    rsgislib.imageutils.normaliseImagePxlVals(inputimage=tmpImg, outputimage=outImage, gdalformat=gdalformat, datatype=rsgislib.TYPE_32FLOAT, innodataval=-999, outnodataval=-999, outmin=0, outmax=1, stretchtype=rsgislib.imageutils.STRETCH_LINEARSTDDEV, stretchparam=2)
    rsgisUtils.deleteFileWithBasename(tmpImg)
    # Set no data value
    rsgisUtils.setImageNoDataValue(outImage, -999)
    if stats:
        rsgislib.imageutils.popImageStats(outImage,False,-999.,True)



def calcCTVI(image, bBand, nBand, outImage, stats=True, gdalformat='KEA'):
    """ 
Helper function to calculate Corrected Transformed Vegetation Index ((NDVI + 0.5)/sqrt(abs(NDVI + 0.5))), note the output no data value is -999.
    
Where:

:param image: is a string specifying the input image file.
:param bBand: is an int specifying the blue band in the input image (band indexing starts at 1)
:param nBand: is an int specifying the nir band in the input image (band indexing starts at 1)
:param outImage: is a string specifying the output image file.
:param stats: is a boolean specifying whether pyramids and stats should be calculated (Default: True)
:param gdalformat: is a string specifing the output image file format (Default: KEA)
    
"""
    expression = '(nir+red)!=0?(((nir-red)/(nir+red))+0.5)/sqrt(abs((nir-red)/(nir+red))+0.5)):-999'
    bandDefns = []
    bandDefns.append(rsgislib.imagecalc.BandDefn('blue', image, bBand))
    bandDefns.append(rsgislib.imagecalc.BandDefn('nir', image, nBand))
    rsgislib.imagecalc.bandMath(outImage, expression, gdalformat, rsgislib.TYPE_32FLOAT, bandDefns)
    # Set no data value
    rsgislib.RSGISPyUtils().setImageNoDataValue(outImage, -999)
    if stats:
        rsgislib.imageutils.popImageStats(outImage,False,-999.,True)
        



"""
http://bleutner.github.io/RStoolbox/rstbx-docu/spectralIndices.html
Index	Description	Source	Bands	Formula
CTVI		Perry1984	red, nir	
DVI	Difference Vegetation Index	Richardson1977	red, nir	s * nir - red
EVI	Enhanced Vegetation Index	Huete1999	red, nir, blue	G * ((nir - red)/(nir + C1 * red - C2 * blue + L_evi))
EVI2	Two-band Enhanced Vegetation Index	Jiang 2008	red, nir	G * (nir - red)/(nir + 2.4 * red + 1)
GEMI	Global Environmental Monitoring Index	Pinty1992	red, nir	(((nir^2 - red^2) * 2 + (nir * 1.5) + (red * 0.5))/(nir + red + 0.5)) * (1 - ((((nir^2 - red^2) * 2 + (nir * 1.5) + (red * 0.5))/(nir + red + 0.5)) * 0.25)) - ((red - 0.125)/(1 - red))
GNDVI	Green Normalised Difference Vegetation Index	Gitelson1998	green, nir	(nir - green)/(nir + green)
MNDWI	Modified Normalised Difference Water Index	Xu2006	green, swir2	(green - swir2)/(green + swir2)
MSAVI	Modified Soil Adjusted Vegetation Index	Qi1994	red, nir	nir + 0.5 - (0.5 * sqrt((2 * nir + 1)^2 - 8 * (nir - (2 * red))))
MSAVI2	Modified Soil Adjusted Vegetation Index 2	Qi1994	red, nir	(2 * (nir + 1) - sqrt((2 * nir + 1)^2 - 8 * (nir - red)))/2
NBRI	Normalised Burn Ratio Index	Garcia1991	nir, swir3	(nir - swir3)/(nir + swir3)
NDVI	Normalised Difference Vegetation Index	Rouse1974	red, nir	(nir - red)/(nir + red)
NDVIC	Corrected Normalised Difference Vegetation Index	Nemani1993	red, nir, swir2	(nir - red)/(nir + red) * (1 - ((swir2 - swir2ccc)/(swir2coc - swir2ccc)))
NDWI	Normalised Difference Water Index	McFeeters1996	green, nir	(green - nir)/(green + nir)
NDWI2	Normalised Difference Water Index	Gao1996	nir, swir2	(nir - swir2)/(nir + swir2)
NRVI	Normalised Ratio Vegetation Index	Baret1991	red, nir	(red/nir - 1)/(red/nir + 1)
RVI	Ratio Vegetation Index		red, nir	red/nir
SATVI	Soil Adjusted Total Vegetation Index	Marsett2006	red, swir2, swir3	(swir2 - red)/(swir2 + red + L) * (1 + L) - (swir3/2)
SAVI	Soil Adjusted Vegetation Index	Huete1988	red, nir	(nir - red) * (1 + L)/(nir + red + L)
SLAVI	Specific Leaf Area Vegetation Index	Lymburger2000	red, nir, swir2	nir/(red + swir2)
SR	Simple Ratio Vegetation Index	Birth1968	red, nir	nir/red
TVI	Transformed Vegetation Index	Deering1975	red, nir	sqrt((nir - red)/(nir + red) + 0.5)
TTVI	Thiam's Transformed Vegetation Index	Thiam1997	red, nir	sqrt(abs((nir - red)/(nir + red) + 0.5))
WDVI	Weighted Difference Vegetation Index	Richardson1977	red, nir	nir - s * red
Some indices require additional parameters, such as the slope of the soil line wich are specified via a list to the coefs argument. Although the defaults are sensible values, values like the soil brigthness factor L for SAVI should be adapted depending on the characteristics of the scene. The coefficients are:

Coefficient	Description	Affected Indices
s	slope of the soil line	DVI, WDVI
L_evi, C1, C2, G	various	EVI
L	soil brightness factor	SAVI, SATVI
swir2ccc	minimum swir2 value (completely closed forest canopy)	NDVIC
swir2coc	maximum swir2 value (completely open canopy)	NDVIC
The wavelength band names are defined following Schowengertd 2007, p10. The last column shows exemplarily which Landsat 5 TM bands correspond to which wavelength range definition.

"""


