"""
The tools module contains some useful tools and utilities for remote sensing work.
"""

# Import the RSGISLib Zonal Stats module
from rsgislib import zonalstats
# Import the os.path module
import os.path
# Import the os module
import os

haveGDALPy = True
try:
    import osgeo.gdal as gdal
except ImportError as gdalErr:
    haveGDALPy = False


haveMatPlotLib = True
try:
    import matplotlib.pyplot as plt
except ImportError as pltErr:
    haveMatPlotLib = False    
    

    
def plotImageSpectra(inputImage, roiFile, outputPlotFile, wavelengths, plotTitle, scaleFactor=0.1, showReflStd=True, reflMax=None):
    """A utility function to extract and plot image spectra.
Where:

* inputImage is the input image
* roiFile is the region of interest file as a shapefile - if multiple polygons are defined the spectra for each will be added to the plot.
* outputPlotFile is the output PDF file for the plot which has been create
* wavelengths is list of numbers with the wavelength of each band (must have the same number of wavelengths as image bands)
* plotTitle is a string with the title for the plot
* scaleFactor is a float specifying the scaling to percentage (0 - 100). (Default is 0.1, i.e., pixel values are scaled between 0-1000; ARCSI default).
* showReflStd is a boolean (default: True) to specify whether a shaded region showing 1 standard deviation from the mean on the plot alongside the mean spectra.
* reflMax is a parameter for setting the maximum reflectance value on the Y axis (if None the maximum value in the dataset is used

Example::

    from rsgislib import tools

    inputImage = 'injune_p142_casi_sub_utm.kea'
    roiFile = 'spectraROI.shp'
    outputPlotFile = 'SpectraPlot.pdf'
    wavelengths = [446.0, 530.0, 549.0, 569.0, 598.0, 633.0, 680.0, 696.0, 714.0, 732.0, 741.0, 752.0, 800.0, 838.0]
   	plotTitle = "Image Spectral from CASI Image"
   	
    tools.plotImageSpectra(inputImage, roiFile, outputPlotFile, wavelengths, plotTitle)
    

"""

    try:
        # Check gdal is available
        if not haveGDALPy:
            raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
        # Check matplotlib is available
        if not haveMatPlotLib:
            raise Exception("The matplotlib module is required for this function could not be imported\n\t" + pltErr)
        
        dataset = gdal.Open(inputImage, gdal.GA_ReadOnly)
        numBands = dataset.RasterCount
        dataset = None
        
        if not len(wavelengths) == numBands:
            raise Exception("The number of wavelengths and image bands must be equal.")
        
        tmpOutFile = os.path.splitext(outputPlotFile)[0] + "_statstmp.txt"
        zonalattributes = zonalstats.ZonalAttributes(minThreshold=0, maxThreshold=10000, calcCount=False, calcMin=False, calcMax=False, calcMean=True, calcStdDev=True, calcMode=False, calcSum=False)
        zonalstats.pixelStats2TXT(inputImage, roiFile, tmpOutFile, zonalattributes, True, True, zonalstats.METHOD_POLYCONTAINSPIXELCENTER, False)
        
        meanVals = []
        stdDevVals = []
        stats = open(tmpOutFile, 'r')
        row = 0
        for statsRow in stats:
            statsRow = statsRow.strip()
            if row > 0:
                meanVal = []
                stdDevVal = []
                data = statsRow.split(',')
                
                if not len(data) == (numBands * 2) + 2:
                    raise Exception("The number of outputted stats values is incorrect!")
                for band in range(numBands):
                    meanVal.append(float(data[(band*2)+1])*scaleFactor)
                    stdDevVal.append(float(data[(band*2)+2])*scaleFactor)
                meanVals.append(meanVal)
                stdDevVals.append(stdDevVal)
            row+=1
        stats.close()
        
        #print("Mean: ", meanVals)
        #print("Std Dev: ", stdDevVals)
        print("Creating Plot")
        fig = plt.figure(figsize=(7, 5), dpi=80)
        ax1 = fig.add_subplot(111)
        for feat in range(len(meanVals)):
            ax1.plot(wavelengths, meanVals[feat], 'k-', zorder=10)
            if showReflStd:
                lowerVals = []
                upperVals = []
                for band in range(numBands):
                    lowerVals.append(meanVals[feat][band] - stdDevVals[feat][band])
                    upperVals.append(meanVals[feat][band] + stdDevVals[feat][band])
                ax1.fill_between(wavelengths, lowerVals, upperVals, alpha=0.2, linewidth=1.0, facecolor=[0.70,0.70,0.70], edgecolor=[0.70,0.70,0.70], zorder=-1)
        
        ax1Range = ax1.axis('tight')
                
        if reflMax is None:
        	ax1.axis((ax1Range[0], ax1Range[1], 0, ax1Range[3]))
        else:
        	ax1.axis((ax1Range[0], ax1Range[1], 0, reflMax))
        
        plt.grid(color='k', linestyle='--', linewidth=0.5)
        plt.title(plotTitle)
        plt.xlabel("Wavelength")
        plt.ylabel("Reflectance (%)")
    
        plt.savefig(outputPlotFile, format='PDF')
        os.remove(tmpOutFile)
        print("Completed.\n")

    except Exception as e:
        raise e