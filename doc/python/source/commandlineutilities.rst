Command Line Utilities
======================

RSGISLib comes with a number of command line utilities. To get more information on a command and usage use -h or --help.

Batch Processing
----------------
* rsgisapplycmd.py - A tool to enable a command line tool (e.g., gdal_translate) to be applied to a number of input files.
* rsgisbuildimglut.py - A command to build a look up table (LUT) for a set of input imanges.
* rsgisfilelut.py - A command to use a look up table (LUT) to find images and create commands for processing  
* rsgisbatchconvert2tif.py - A command to convert a set of input images to GTIFF using recommended options.


Data Processing
----------------
* rsgismosaic.py - Mosaic a set of input image files.
* rsgiscalcimgstats.py - Calculate the image statistics and pyramids to enable fast display of image data.
* rsgiscopybandnames.py - Copy the band names from an input image to an output image.
* rsgisimg2kmz.py - convert an image dataset to KMZ for display in Google Earth Viewer
* rsgisimg2webtiles.py - create a website using leaflet using input image data
* rsgisimginfo.py - similar to gdalinfo, get basic information for the image of interest.
* rsgisproj.py - Find information and confirm between the first lie.
* rsgissegmentation.py - Perform an image segmentation using the Shepherd et al, (2019) method.
* rsgisvectools.py - Find a list of layer and column names for a vector layer.
* rsgistranslate3tif.py - Uses gdal_translate to convert an image file to GTIFF using recommended options.
* rsgischkgdalfile.py - A tool which will check whether a file which GDAL can open (image or vector) is valid (i.e., not corrupted)
* rsgisdwndem.py - A tool for downloading and mosaicing SRTM data for a particularly region.
* rsgisfilehash.py - A tool to calculate the hash (various options) for a file.

Post Processing
----------------
* rsgiscalcaccmatrix.py - Produce classification error matrix


Other
-----
* flip - Convert line endings (e.g., Windows to UNIX, UNIX to Mac Classic etc.).
* rsgis-config - developer tool to find the installation paths and build options.
