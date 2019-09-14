"""
The tools.plotting module contains functions for extracting and plotting remote sensing data.
"""

# Import the RSGISLib module
import rsgislib
# Import the RSGISLib Image Utils module
from rsgislib import imageutils
# Import the RSGISLib Zonal Stats module
from rsgislib import zonalstats
# Import the RSGISLib Image Calc module
from rsgislib import imagecalc
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
    import matplotlib.colors as mClrs
except ImportError as pltErr:
    haveMatPlotLib = False   
    
haveNumpy = True
try:
    import numpy
except ImportError as numErr:
    haveNumpy = False    
    
    
def plotImageSpectra(inputImage, roiFile, outputPlotFile, wavelengths, plotTitle, scaleFactor=0.1, showReflStd=True, reflMax=None):
    """A utility function to extract and plot image spectra.
Where:

:param inputImage: is the input image
:param roiFile: is the region of interest file as a shapefile - if multiple polygons are defined the spectra for each will be added to the plot.
:param outputPlotFile: is the output PDF file for the plot which has been create
:param wavelengths: is list of numbers with the wavelength of each band (must have the same number of wavelengths as image bands)
:param plotTitle: is a string with the title for the plot
:param scaleFactor: is a float specifying the scaling to percentage (0 - 100). (Default is 0.1, i.e., pixel values are scaled between 0-1000; ARCSI default).
:param showReflStd: is a boolean (default: True) to specify whether a shaded region showing 1 standard deviation from the mean on the plot alongside the mean spectra.
:param reflMax: is a parameter for setting the maximum reflectance value on the Y axis (if None the maximum value in the dataset is used

Example::

    from rsgislib import tools
        
    inputImage = 'injune_p142_casi_sub_utm.kea'
    roiFile = 'spectraROI.shp'
    outputPlotFile = 'SpectraPlot.pdf'
    wavelengths = [446.0, 530.0, 549.0, 569.0, 598.0, 633.0, 680.0, 696.0, 714.0, 732.0, 741.0, 752.0, 800.0, 838.0]
    plotTitle = "Image Spectral from CASI Image"
    
    tools.plotting.plotImageSpectra(inputImage, roiFile, outputPlotFile, wavelengths, plotTitle)
    
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


def plotImageComparison(inputImage1, inputImage2, img1Band, img2Band, outputPlotFile, numBins=100, img1Min=None, img1Max=None, img2Min=None, img2Max=None, img1Scale=1, img2Scale=1, img1Off=0, img2Off=0, normOutput=False, plotTitle='2D Histogram', xLabel='X Axis', yLabel='Y Axis', ctable='jet', interp='nearest'):
    """A function to plot two images against each other. 
Where:

:param inputImage1: is a string with the path to the first image.
:param inputImage2: is a string with the path to the second image.
:param img1Band: is an int specifying the band in the first image to be plotted.
:param img2Band: is an int specifying the band in the second image to be plotted.
:param outputPlotFile: is a string specifying the output PDF for the plot.
:param numBins: is an int specifying the number of bins within each axis of the histogram (default: 100)
:param img1Min: is a double specifying the minimum value to be used in the histogram from image 1. If value is None then taken from the image.
:param img1Max: is a double specifying the maximum value to be used in the histogram from image 1. If value is None then taken from the image.
:param img2Min: is a double specifying the minimum value to be used in the histogram from image 2. If value is None then taken from the image.
:param img2Max: is a double specifying the maximum value to be used in the histogram from image 2. If value is None then taken from the image.
:param img1Scale: is a double specifying the scale for image 1 (Default 1).
:param img2Scale: is a double specifying the scale for image 2 (Default 1).
:param img1Off: is a double specifying the offset for image 1 (Default 0).
:param img2Off: is a double specifying the offset for image 2 (Default 0).
:param normOutput: is a boolean specifying whether the histogram should be normalised (Default: False).
:param plotTitle: is a string specifying the title of the plot (Default: '2D Histogram').
:param xLabel: is a string specifying the x axis label (Default: 'X Axis')
:param yLabel: is a string specifying the y axis label (Default: 'Y Axis')
:param ctable: is a string specifying the colour table to be used (Default: jet), list of available colour tables specified by matplotlib: http://matplotlib.org/examples/color/colormaps_reference.html
:param interp: is a string specifying the interpolation algorithm to be used (Default: 'nearest'). Available values are ‘none’, ‘nearest’, ‘bilinear’, ‘bicubic’, ‘spline16’, ‘spline36’, ‘hanning’, ‘hamming’, ‘hermite’, ‘kaiser’, ‘quadric’, ‘catrom’, ‘gaussian’, ‘bessel’, ‘mitchell’, ‘sinc’, ‘lanczos’.
    
Example::

    from rsgislib.tools import plotting
    
    inputImage1 = 'LS5TM_20000613_lat10lon6217_r67p231_rad_sref_ndvi.kea'
    inputImage2 = 'LS5TM_20000613_lat10lon6217_r67p231_rad_ndvi.kea'
    outputPlotFile = 'ARCSI_RAD_SREF_NDVI.pdf'
    
    plotting.plotImageComparison(inputImage1, inputImage2, 1, 1, outputPlotFile, img1Min=-0.5, img1Max=1, img2Min=-0.5, img2Max=1, plotTitle='ARCSI SREF NDVI vs ARCSI RAD NDVI', xLabel='ARCSI SREF NDVI', yLabel='ARCSI RAD NDVI')
    
    """
    try:
        # Check gdal is available
        if not haveGDALPy:
            raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
        # Check matplotlib is available
        if not haveMatPlotLib:
            raise Exception("The matplotlib module is required for this function could not be imported\n\t" + pltErr)
        # Check matplotlib is available
        if not haveNumpy:
            raise Exception("The numpy module is required for this function could not be imported\n\t" + numErr)
        
        gdalformat = "KEA"      
        tmpOutFile = os.path.splitext(outputPlotFile)[0] + "_hist2dimgtmp.kea"
        #tmpOutFileStch = os.path.splitext(outputPlotFile)[0] + "_hist2dimgtmpStch.kea"
        
        if (img1Min is None) or (img1Max is None):
            # Calculate image 1 stats
            imgGDALDS = gdal.Open(inputImage1, gdal.GA_ReadOnly)
            imgGDALBand = imgGDALDS.GetRasterBand(img1Band)
            min, max = imgGDALBand.ComputeRasterMinMax(False)
            imgGDALDS = None
            if img1Min is None:
                img1Min = min
            if img1Max is None:
                img1Max = max
            
        if (img2Min is None) or (img2Max is None):
            # Calculate image 2 stats
            imgGDALDS = gdal.Open(inputImage2, gdal.GA_ReadOnly)
            imgGDALBand = imgGDALDS.GetRasterBand(img2Band)
            min, max = imgGDALBand.ComputeRasterMinMax(False)
            imgGDALDS = None
            if img2Min is None:
                img2Min = min
            if img2Max is None:
                img2Max = max
        
        # Images are flipped so axis' come out correctly.
        outBinSizeImg1, outBinSizeImg2, rSq = imagecalc.get2DImageHistogram(inputImage2, inputImage1, tmpOutFile, gdalformat, img2Band, img1Band, numBins, img2Min, img2Max, img1Min, img1Max, img2Scale, img1Scale, img2Off, img1Off, normOutput)
        print("Image1 Bin Size: ", outBinSizeImg1)
        print("Image2 Bin Size: ", outBinSizeImg2)
        print("rSq: ", rSq)
                
        print("Read Image Data")
        plotGDALImg = gdal.Open(tmpOutFile, gdal.GA_ReadOnly)
        plotImgBand = plotGDALImg.GetRasterBand(1)
        dataArr = plotImgBand.ReadAsArray().astype(float)
        plotGDALImg = None
        
        fig = plt.figure(figsize=(7, 7), dpi=80)
        ax1 = fig.add_subplot(111)
        
        img1MinSc = img1Off + (img1Min*img1Scale)
        img1MaxSc = img1Off + (img1Max*img1Scale)
        img2MinSc = img2Off + (img2Min*img2Scale)
        img2MaxSc = img2Off + (img2Max*img2Scale)
        
        minVal = numpy.min(dataArr[dataArr!=0])
        maxVal = numpy.max(dataArr)
        
        print("Min Value: ", minVal)
        print("Max Value: ", maxVal)
        
        cmap=plt.get_cmap(ctable)
        mClrs.Colormap.set_under(cmap,color='white')
        mClrs.Colormap.set_over(cmap,color='white')
        
        imPlot = plt.imshow(dataArr, cmap=cmap, aspect='equal', interpolation=interp, norm=mClrs.Normalize(vmin=minVal, vmax=maxVal), vmin=minVal, vmax=maxVal, origin=[0,0], extent=[img1MinSc, img1MaxSc, img2MinSc, img2MaxSc])
        plt.grid(color='k', linestyle='--', linewidth=0.5)
        rSqStr = ''
        if rSq < 0:
            rSq = 0.00
        rSqStr = "$r^2 = " + str(round(rSq, 3)) + "$"
        plt.text(0.05, 0.95, rSqStr, va='center', transform=ax1.transAxes)
        fig.colorbar(imPlot)
        plt.title(plotTitle)
        plt.xlabel(xLabel)
        plt.ylabel(yLabel)
    
        plt.savefig(outputPlotFile, format='PDF')
        
        # Tidy up temporary file.
        gdalDriver = gdal.GetDriverByName(gdalformat)
        gdalDriver.Delete(tmpOutFile)
                
    except Exception as e:
        raise e


def plotImageHistogram(inputImage, imgBand, outputPlotFile, numBins=100, imgMin=None, imgMax=None, normOutput=False, plotTitle='Histogram', xLabel='X Axis', colour='blue', edgecolour='black', linewidth=None):
    """
A function to plot the histogram of an image.

Where:

:param inputImage: is a string with the path to the image.
:param imgBand: is an int specifying the band in the image to be plotted.
:param outputPlotFile: is a string specifying the output PDF for the plot.
:param numBins: is an int specifying the number of bins within each axis of the histogram (default: 100)
:param imgMin: is a double specifying the minimum value to be used in the histogram from the image. If value is None then taken from the image.
:param imgMax: is a double specifying the maximum value to be used in the histogram from the image. If value is None then taken from the image.
:param normOutput: is a boolean specifying whether the histogram should be normalised (Default: False).
:param plotTitle: is a string specifying the title of the plot (Default: '2D Histogram').
:param xLabel: is a string specifying the x axis label (Default: 'X Axis')
:param colour: is the colour of the bars in the plot (see matplotlib documentation for how to specify, either keyword or RGB values (e.g., [1.0,0,0])
:param edgecolour: is the colour of the edges of the bars
:param linewidth: is the thickness of the edges of the bars in the plot.
    
Example::

    from rsgislib.tools import plotting
    
    plotting.plotImageHistogram("Baccini_Manaus_AGB_30.kea", 1, "BacciniHistogram.pdf", numBins=100, imgMin=0, imgMax=400, normOutput=True, plotTitle='Histogram of Baccini Biomass', xLabel='Baccini Biomass', color=[1.0,0.2,1.0], edgecolor='red', linewidth=0)
    
    """
    try:
        # Check gdal is available
        if not haveGDALPy:
            raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
        # Check matplotlib is available
        if not haveMatPlotLib:
            raise Exception("The matplotlib module is required for this function could not be imported\n\t" + pltErr)
        # Check matplotlib is available
        if not haveNumpy:
            raise Exception("The numpy module is required for this function could not be imported\n\t" + numErr)
        
        
        if (imgMin is None) or (imgMax is None):
            # Calculate image 1 stats
            imgGDALDS = gdal.Open(inputImage, gdal.GA_ReadOnly)
            imgGDALBand = imgGDALDS.GetRasterBand(imgBand)
            min, max = imgGDALBand.ComputeRasterMinMax(False)
            imgGDALDS = None
            if imgMin is None:
                imgMin = min
            if imgMax is None:
                imgMax = max

        binWidth = (imgMax - imgMin) / numBins
        print("Bin Size: ", binWidth)
        
        bins, hMin, hMax = imagecalc.getHistogram(inputImage, imgBand, binWidth, False, imgMin, imgMax)
                
        if normOutput:
            sumBins = numpy.sum(bins)
            bins = bins/sumBins
        
        numBins = len(bins)
        xLocs = numpy.arange(numBins)
        xLocs = (xLocs * binWidth) - (binWidth / 2)
        
        fig = plt.figure(figsize=(7, 7), dpi=80)
        plt.bar(xLocs, bins, width=binWidth, color=colour, edgecolor=edgecolour, linewidth=linewidth)
        plt.xlim(imgMin, imgMax)
        
        plt.title(plotTitle)
        plt.xlabel(xLabel)
        plt.ylabel('Freq.')
        plt.savefig(outputPlotFile, format='PDF')
                
    except Exception as e:
        raise e





