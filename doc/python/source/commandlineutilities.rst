Command Line Utilities
======================

RSGISLib comes with a number of command line utilities. To get more information on a command and usage use -h or --help.

Batch Processing
----------------
* rsgisapplycmd.py - A tool to enable a command line tool (e.g., gdal_translate) to be applied to a number of input files.
* rsgisbuildimglut.py - A command to build a look up table (LUT) for a set of input imanges.
* rsgisfilelut.py - A command to use a look up table (LUT) to find images and create commands for processing  



Data Processing
----------------
* rsgislibsegmentation.py - Run image segmentation
* rsgislibattributerat.py - Attribute Raster Attribute Table
* rsgislibmosaic.py - Mosaic a set of input image files.
* rsgislibcalcindices.py - Easy tool for calculating image indicies
* rsgiscalcimgstats.py - Calculate the image statisitics and pyramids to enable fast display of image data.
* rsgiscopybandnames.py - Copy the band names from an input image to an output image.
* rsgisimg2kmz.py - convert an image dataset to KMZ for display in Google Earth Viewer
* rsgisimg2webtiles.py - create a website using leaflet using input image data
* rsgisimginfo.py - simple to gdalinfo, get basic information for the image of interest.
* rsgisproj.py - Find informaation and confirm between the first lie.
* rsgissegmentation.py - Perform an image segmentation using the Shepherd et al, (2019) method.
* rsgisvectools.py - Find a list of layer and column names for a vector layer.
* rsgislibzonalstats.py - Undertake zonal statistics between a vector and raster layer.


Post Processing
----------------
* rsgiscalcaccmatrix.py - Produce classification error matrix
* rsgisplot.py - visualise some image datasets
* rsgisplotrastergis.py - visualise some datasets stored in RAT.


Other
-----
* flip - Convert line endings (e.g., Windows to UNIX, UNIX to Mac Classic).
* rsgis-config - developer tool to find the installation paths and build options.
