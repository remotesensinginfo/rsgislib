Command Line Utilities
======================

RSGISLib comes with a number of command line utilities. To get more information on
a command and usage use -h or --help.

Note, when using Microsoft Windows any tool with the file extension .py will have
the extention .bat. For example, rsgiscalcimgstats.py will be rsgiscalcimgstats.bat
The options and manner of use should remain the same.

Batch Processing
----------------
* rsgisapplycmd.py - A tool to enable a command line tool (e.g., gdal_translate) to be applied to a number of input files.
* rsgisbuildimglut.py - A command to build a look up table (LUT) for a set of input imanges.
* rsgisfilelut.py - A command to use a look up table (LUT) to find images and create commands for processing  
* rsgisbatchconvert2tif.py - A command to convert a set of input images to GTIFF using recommended options.
* rsgisbatchconvert2cog.py - A command to convert a set of input images to GTIFF COGs using recommended options.


Raster
-------
* rsgiscalcimgstats.py - Calculate the image statistics and pyramids to enable fast display of image data.
* rsgiscopybandnames.py - Copy the band names from an input image to an output image.
* rsgisimg2kmz.py - convert an image dataset to KMZ for display in Google Earth Viewer
* rsgisimg2webtiles.py - create a website using leaflet using input image data
* rsgisimginfo.py - similar to gdalinfo, get basic information for the image of interest.
* rsgistranslate2tif.py - Uses gdal_translate to convert an image file to GTIFF using recommended options.
* rsgisshepsegment.py - Run the Shepherd segmentation algorithm
* rsgistranslate2cog.py - Convert a single image to a GTIFF COGs using recommended options.
* rsgistranslate2tif.py - Convert a single image to a GTIFF using recommended options.


Vector
-------
* rsgisvectools.py - Find a list of layer and column names for a vector layer.

Raster and Vector
-------------------
* rsgisproj.py - Find information and confirm between the first lie.
* rsgischkgdalfile.py - A tool which will check whether a file which GDAL can open (image or vector) is valid (i.e., not corrupted)


Other
-------
* rsgisflip - Convert line endings (e.g., Windows to UNIX, UNIX to Mac Classic etc.).
* rsgis-config - developer tool to find the installation paths and build options.
* rsgisuserpassfile.py - create an rsgislib username/password file where the data are encoded.
* rsgisfilehash.py - A tool to calculate the hash (various options) for a file.
