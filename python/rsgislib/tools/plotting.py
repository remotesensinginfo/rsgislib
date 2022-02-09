#!/usr/bin/env python
"""
The tools.plotting module contains functions for extracting and plotting remote sensing data.
"""

from typing import Tuple, List, Dict, Union
import os

import rsgislib
from rsgislib import zonalstats
from rsgislib import imagecalc

from osgeo import gdal
import numpy

have_matplotlib = True
try:
    import matplotlib.pyplot as plt
    import matplotlib.colors as mClrs
    from matplotlib.patches import Patch
    from matplotlib import rcParams
    from matplotlib import gridspec
except ImportError:
    have_matplotlib = False


def plot_image_spectra(
    input_img,
    vec_file,
    vec_lyr,
    output_plot_file,
    wavelengths,
    plot_title,
    scale_factor=0.1,
    show_refl_std=True,
    refl_max=None,
):
    """
    A utility function to extract and plot image spectra.

    :param input_img: is the input image
    :param vec_file: is the region of interest file as a vector file - if multiple polygons are defined the spectra for each will be added to the plot.
    :param vec_lyr:
    :param output_plot_file: is the output PDF file for the plot which has been create
    :param wavelengths: is list of numbers with the wavelength of each band (must have the same number of wavelengths as image bands)
    :param plot_title: is a string with the title for the plot
    :param scale_factor: is a float specifying the scaling to percentage (0 - 100). (Default is 0.1, i.e., pixel values are scaled between 0-1000; ARCSI default).
    :param show_refl_std: is a boolean (default: True) to specify whether a shaded region showing 1 standard deviation from the mean on the plot alongside the mean spectra.
    :param refl_max: is a parameter for setting the maximum reflectance value on the Y axis (if None the maximum value in the dataset is used

    .. code:: python

        from rsgislib.tools import plotting

        inputImage = 'injune_p142_casi_sub_utm.kea'
        roiFile = 'spectraROI.shp'
        outputPlotFile = 'SpectraPlot.pdf'
        wavelengths = [446.0, 530.0, 549.0, 569.0, 598.0, 633.0, 680.0, 696.0, 714.0, 732.0, 741.0, 752.0, 800.0, 838.0]
        plotTitle = "Image Spectral from CASI Image"

        plotting.plot_image_spectra(inputImage, roiFile, outputPlotFile, wavelengths, plotTitle)

    """
    import rsgislib.tools.filetools

    if not have_matplotlib:
        raise rsgislib.RSGISPyException(
            "The matplotlib module is required and could not be imported."
        )

    dataset = gdal.Open(input_img, gdal.GA_ReadOnly)
    numBands = dataset.RasterCount
    dataset = None

    if not len(wavelengths) == numBands:
        raise rsgislib.RSGISPyException(
            "The number of wavelengths and image bands must be equal."
        )

    tmpOutFile = os.path.splitext(output_plot_file)[0] + "_statstmp.txt"
    zonalattributes = zonalstats.ZonalAttributes(
        minThreshold=0,
        maxThreshold=10000,
        calcCount=False,
        calcMin=False,
        calcMax=False,
        calcMean=True,
        calcStdDev=True,
        calcMode=False,
        calcSum=False,
    )
    zonalstats.pixelStats2TXT(
        input_img,
        vec_file,
        tmpOutFile,
        zonalattributes,
        True,
        True,
        zonalstats.METHOD_POLYCONTAINSPIXELCENTER,
        False,
    )

    meanVals = []
    stdDevVals = []
    stats = open(tmpOutFile, "r")
    row = 0
    for statsRow in stats:
        statsRow = statsRow.strip()
        if row > 0:
            meanVal = []
            stdDevVal = []
            data = statsRow.split(",")

            if not len(data) == (numBands * 2) + 2:
                raise rsgislib.RSGISPyException(
                    "The number of outputted stats values is incorrect!"
                )
            for band in range(numBands):
                meanVal.append(float(data[(band * 2) + 1]) * scale_factor)
                stdDevVal.append(float(data[(band * 2) + 2]) * scale_factor)
            meanVals.append(meanVal)
            stdDevVals.append(stdDevVal)
        row += 1
    stats.close()

    # print("Mean: ", meanVals)
    # print("Std Dev: ", stdDevVals)
    print("Creating Plot")
    fig = plt.figure(figsize=(7, 5), dpi=80)
    ax1 = fig.add_subplot(111)
    for feat in range(len(meanVals)):
        ax1.plot(wavelengths, meanVals[feat], "k-", zorder=10)
        if show_refl_std:
            lowerVals = []
            upperVals = []
            for band in range(numBands):
                lowerVals.append(meanVals[feat][band] - stdDevVals[feat][band])
                upperVals.append(meanVals[feat][band] + stdDevVals[feat][band])
            ax1.fill_between(
                wavelengths,
                lowerVals,
                upperVals,
                alpha=0.2,
                linewidth=1.0,
                facecolor=[0.70, 0.70, 0.70],
                edgecolor=[0.70, 0.70, 0.70],
                zorder=-1,
            )

    ax1Range = ax1.axis("tight")

    if refl_max is None:
        ax1.axis((ax1Range[0], ax1Range[1], 0, ax1Range[3]))
    else:
        ax1.axis((ax1Range[0], ax1Range[1], 0, refl_max))

    plt.grid(color="k", linestyle="--", linewidth=0.5)
    plt.title(plot_title)
    plt.xlabel("Wavelength")
    plt.ylabel("Reflectance (%)")

    plt.savefig(output_plot_file, format="PDF")
    rsgislib.tools.filetools.delete_file_silent(tmpOutFile)
    print("Completed.\n")


def plot_image_comparison(
    inputImage1,
    inputImage2,
    img1Band,
    img2Band,
    outputPlotFile,
    numBins=100,
    img1Min=None,
    img1Max=None,
    img2Min=None,
    img2Max=None,
    img1Scale=1,
    img2Scale=1,
    img1Off=0,
    img2Off=0,
    normOutput=False,
    plotTitle="2D Histogram",
    xLabel="X Axis",
    yLabel="Y Axis",
    ctable="jet",
    interp="nearest",
):
    """
    A function to plot two images against each other.

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

    .. code:: python

        from rsgislib.tools import plotting

        inputImage1 = 'LS5TM_20000613_lat10lon6217_r67p231_rad_sref_ndvi.kea'
        inputImage2 = 'LS5TM_20000613_lat10lon6217_r67p231_rad_ndvi.kea'
        outputPlotFile = 'ARCSI_RAD_SREF_NDVI.pdf'

        plotting.plot_image_comparison(inputImage1, inputImage2, 1, 1, outputPlotFile, img1Min=-0.5, img1Max=1, img2Min=-0.5, img2Max=1, plotTitle='ARCSI SREF NDVI vs ARCSI RAD NDVI', xLabel='ARCSI SREF NDVI', yLabel='ARCSI RAD NDVI')

    """
    if not have_matplotlib:
        raise rsgislib.RSGISPyException(
            "The matplotlib module is required and could not be imported."
        )

    gdalformat = "KEA"
    tmpOutFile = os.path.splitext(outputPlotFile)[0] + "_hist2dimgtmp.kea"
    # tmpOutFileStch = os.path.splitext(outputPlotFile)[0] + "_hist2dimgtmpStch.kea"

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
    outBinSizeImg1, outBinSizeImg2, rSq = imagecalc.get2DImageHistogram(
        inputImage2,
        inputImage1,
        tmpOutFile,
        gdalformat,
        img2Band,
        img1Band,
        numBins,
        img2Min,
        img2Max,
        img1Min,
        img1Max,
        img2Scale,
        img1Scale,
        img2Off,
        img1Off,
        normOutput,
    )
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

    img1MinSc = img1Off + (img1Min * img1Scale)
    img1MaxSc = img1Off + (img1Max * img1Scale)
    img2MinSc = img2Off + (img2Min * img2Scale)
    img2MaxSc = img2Off + (img2Max * img2Scale)

    minVal = numpy.min(dataArr[dataArr != 0])
    maxVal = numpy.max(dataArr)

    print("Min Value: ", minVal)
    print("Max Value: ", maxVal)

    cmap = plt.get_cmap(ctable)
    mClrs.Colormap.set_under(cmap, color="white")
    mClrs.Colormap.set_over(cmap, color="white")

    imPlot = plt.imshow(
        dataArr,
        cmap=cmap,
        aspect="equal",
        interpolation=interp,
        norm=mClrs.Normalize(vmin=minVal, vmax=maxVal),
        vmin=minVal,
        vmax=maxVal,
        origin=[0, 0],
        extent=[img1MinSc, img1MaxSc, img2MinSc, img2MaxSc],
    )
    plt.grid(color="k", linestyle="--", linewidth=0.5)
    rSqStr = ""
    if rSq < 0:
        rSq = 0.00
    rSqStr = "$r^2 = " + str(round(rSq, 3)) + "$"
    plt.text(0.05, 0.95, rSqStr, va="center", transform=ax1.transAxes)
    fig.colorbar(imPlot)
    plt.title(plotTitle)
    plt.xlabel(xLabel)
    plt.ylabel(yLabel)

    plt.savefig(outputPlotFile, format="PDF")

    # Tidy up temporary file.
    gdalDriver = gdal.GetDriverByName(gdalformat)
    gdalDriver.Delete(tmpOutFile)


def plot_image_histogram(
    input_img,
    imgBand,
    outputPlotFile,
    numBins=100,
    imgMin=None,
    imgMax=None,
    normOutput=False,
    plotTitle="Histogram",
    xLabel="X Axis",
    colour="blue",
    edgecolour="black",
    linewidth=None,
):
    """
    A function to plot the histogram of an image.

    :param input_img: is a string with the path to the image.
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

    .. code:: python

        from rsgislib.tools import plotting

        plotting.plot_image_histogram("Baccini_Manaus_AGB_30.kea", 1, "BacciniHistogram.pdf", numBins=100, imgMin=0, imgMax=400, normOutput=True, plotTitle='Histogram of Baccini Biomass', xLabel='Baccini Biomass', color=[1.0,0.2,1.0], edgecolor='red', linewidth=0)

    """
    if not have_matplotlib:
        raise rsgislib.RSGISPyException(
            "The matplotlib module is required and could not be imported."
        )

    if (imgMin is None) or (imgMax is None):
        # Calculate image 1 stats
        imgGDALDS = gdal.Open(input_img, gdal.GA_ReadOnly)
        imgGDALBand = imgGDALDS.GetRasterBand(imgBand)
        min, max = imgGDALBand.ComputeRasterMinMax(False)
        imgGDALDS = None
        if imgMin is None:
            imgMin = min
        if imgMax is None:
            imgMax = max

    binWidth = (imgMax - imgMin) / numBins
    print("Bin Size: ", binWidth)

    bins, hMin, hMax = imagecalc.getHistogram(
        input_img, imgBand, binWidth, False, imgMin, imgMax
    )

    if normOutput:
        sumBins = numpy.sum(bins)
        bins = bins / sumBins

    numBins = len(bins)
    xLocs = numpy.arange(numBins)
    xLocs = (xLocs * binWidth) - (binWidth / 2)

    fig = plt.figure(figsize=(7, 7), dpi=80)
    plt.bar(
        xLocs,
        bins,
        width=binWidth,
        color=colour,
        edgecolor=edgecolour,
        linewidth=linewidth,
    )
    plt.xlim(imgMin, imgMax)

    plt.title(plotTitle)
    plt.xlabel(xLabel)
    plt.ylabel("Freq.")
    plt.savefig(outputPlotFile, format="PDF")


def residual_plot(y_true, residuals, out_file, out_format="PNG", title=None):
    """
    A function to create a residual plot to investigate the
    normality and homoscedasticity of model residuals.

    :param y_true: A numpy 1D array containing true/observed values.
    :param residuals: A numpy 1D array containing model residuals.
    :param out_file: Path to the output file.
    :param out_format: Output format supported by matplotlib (e.g. "PNG" or "PDF").
                       Default: PNG
    :param title: A title for the plot. Optional, if None then ignored. (Default: None)

    """
    if not have_matplotlib:
        raise rsgislib.RSGISPyException(
            "The matplotlib module is required and could not be imported."
        )

    if not isinstance(residuals, numpy.ndarray):
        residuals = numpy.array(residuals)
    if not isinstance(y_true, numpy.ndarray):
        y_true = numpy.array(y_true)
    if y_true.ndim != 1:
        raise rsgislib.RSGISPyException("y_true has more than 1 dimension.")
    if residuals.ndim != 1:
        raise rsgislib.RSGISPyException("Residuals has more than 1 dimension.")
    if residuals.size != y_true.size:
        raise rsgislib.RSGISPyException("y_true.size != residuals.size.")

    # setup plot:
    # rcParams.update({'font.family': 'cmr10'})  # use latex fonts.
    # rcParams['axes.unicode_minus'] = False
    rcParams.update({"font.size": 8.5})
    rcParams["axes.linewidth"] = 0.5
    rcParams["xtick.major.pad"] = "2"
    rcParams["ytick.major.pad"] = "2"
    fig = plt.figure(figsize=(5, 2.5))
    gs = gridspec.GridSpec(nrows=1, ncols=2, width_ratios=[3.5, 1])
    ax1 = plt.subplot(gs[0])
    ax2 = plt.subplot(gs[1])
    plt.tight_layout(w_pad=-1, h_pad=0)

    # draw scatterplot:
    ax1.axhline(y=0.0, c="k", ls=":", lw=0.5, zorder=2)
    ax1.scatter(y_true, residuals, s=16, color="C0", marker=".", linewidth=0, zorder=1)
    ax1.set_xlabel("Observed value", fontsize=9)
    ax1.set_ylabel("Residuals", fontsize=9)
    if title is not None:
        ax1.set_title(title)

    # draw histogram:
    ax2.get_xaxis().tick_bottom()
    ax2.get_yaxis().tick_right()
    ax2.get_yaxis().set_visible(False)
    ax2.hist(residuals, bins=50, orientation="horizontal", color="C0")
    ax2.axhline(y=0.0, c="k", ls=":", lw=0.5, zorder=2)
    ax2.set_xlabel("Frequency", fontsize=9)
    plt.savefig(out_file, format=out_format, dpi=300, bbox_inches="tight")
    plt.close()


def quantile_plot(residuals, ylabel, out_file, out_format="PNG", title=None):
    """
    A function to create a Quantile-Quantile plot to investigate the
    normality of model residuals.

    :param residuals: A numpy 1D array containing model residuals.
    :param ylabel: A string defining a label for the y axis
    :param out_file: Path to the output file.
    :param out_format: Output format supported by matplotlib (e.g. "PNG" or "PDF").
                       Default: PNG
    :param title: A title for the plot. Optional, if None then ignored. (Default: None)

    """
    if not have_matplotlib:
        raise rsgislib.RSGISPyException(
            "The matplotlib module is required and could not be imported."
        )
    from scipy.stats import probplot

    if not isinstance(residuals, numpy.ndarray):
        residuals = numpy.array(residuals)

        # rcParams.update({'font.family': 'cmr10'})  # use latex fonts.
    # rcParams['axes.unicode_minus'] = False
    rcParams.update({"font.size": 8.5})
    rcParams["axes.linewidth"] = 0.5
    rcParams["xtick.major.pad"] = "2"
    rcParams["ytick.major.pad"] = "2"
    fig, ax = plt.subplots(nrows=1, ncols=1, figsize=(3.5, 2.5))
    ax.get_yaxis().set_tick_params(which="major", direction="out")
    ax.get_xaxis().set_tick_params(which="major", direction="out")
    ax.get_xaxis().set_tick_params(which="minor", direction="out", length=0, width=0)
    ax.get_yaxis().set_tick_params(which="minor", direction="out", length=0, width=0)
    ax.get_xaxis().tick_bottom()
    ax.get_yaxis().tick_left()
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.xaxis.set_tick_params(width=0.5)
    ax.yaxis.set_tick_params(width=0.5)

    probplot(residuals, dist="norm", plot=ax)

    if title is not None:
        ax.set_title(title)
    else:
        ax.set_title("")
    ax.set_ylabel(ylabel)
    ax.get_lines()[0].set_marker(".")
    ax.get_lines()[0].set_markerfacecolor("k")
    ax.get_lines()[0].set_markeredgecolor("k")
    ax.get_lines()[0].set_markeredgewidth(0)
    ax.get_lines()[0].set_markersize(4.0)
    ax.get_lines()[1].set_linewidth(1.0)
    ax.get_lines()[1].set_color("r")
    plt.savefig(out_file, format=out_format, dpi=300, bbox_inches="tight")
    plt.close()


def plot_histogram_threshold(
    data: numpy.array,
    out_file: str,
    threshold: float = None,
    title_str: str = None,
    hist_clr: str = "gray",
    out_format: str = None,
    plot_dpi: int = 300,
):
    """
    A function which creates a histogram plot for a numpy array of data and optionally
    displaying a threshold on the histogram

    :param data: The numpy array to be plotted. The array should be a 1d array.
    :param out_file: The output image file path for the plot
    :param threshold: An optional threshold to be plotted. If None then ignored.
    :param title_str: An optional string for the plot title.
    :param hist_clr: the colour for the histogram bars.
    :param out_format: the output format of the plot. If None then matplotlib uses
                       the output file extension.
    :param plot_dpi: the dpi for the output plot.

    """
    import rsgislib

    if not have_matplotlib:
        raise rsgislib.RSGISPyException(
            "The matplotlib module is required and could not be imported."
        )
    import rsgislib.tools.stats

    n_bins, bin_width = rsgislib.tools.stats.get_nbins_histogram(data)

    plt.figure()
    plt.hist(data, bins=n_bins, color=hist_clr)
    if threshold is not None:
        plt.axvline(x=threshold, color="red")
    if title_str is not None:
        plt.title(title_str)
    plt.savefig(out_file, format=out_format, dpi=plot_dpi)


def get_gdal_raster_mpl_imshow(
    input_img: str, bands: List[int] = None, bbox: List[float] = None
) -> Tuple[numpy.array, List[float]]:
    """
    A function which retrieves image data as an array in an appropriate structure
    for use within the matplotlib imshow function. The extent is also returned.
    Note, this function assumes that the image pixels values are within an appropriate
    range for display.

    :param input_img: The input image file path.
    :param bands: Optional list of image bands to be selected and returned. If not
                  provided then all bands will be read. However, note that only 3
                  or 1 band(s) are valid for visualisation and an error will be thrown
                  if the number of bands is not 3 or 1.
    :param bbox: Optional bbox (xmin, xmax, ymin, ymax) used to subset the
                 input image so only data for the subset are returned.
    :return: numpy.array either [n,m,3] or [n,m] and a bbox (xmin, xmax, ymin, ymax)
             specifying the extent of the image data.

    .. code:: python

        img_sub_bbox = [554756, 577168, 9903924, 9944315]
        input_img = "sen2_img_strch.kea"

        img_data_arr, coords_bbox = get_gdal_raster_mpl_imshow(input_img,
                                                               bands=[8,9,3],
                                                               bbox=img_sub_bbox)


        import matplotlib.pyplot as plt
        fig, ax = plt.subplots()
        im = ax.imshow(img_data_arr, extent=coords_bbox)
        plt.show()

    """
    import rsgislib.imageutils

    n_bands = rsgislib.imageutils.get_img_band_count(input_img)
    ref_bands = list(range(1, n_bands + 1, 1))
    if bands is None:
        bands = ref_bands
    else:
        for band in bands:
            if band not in ref_bands:
                raise rsgislib.RSGISPyException(
                    f"Band {band} is not valid (i.e., within the image)"
                )

    if not (len(bands) == 3 or len(bands) == 1):
        raise rsgislib.RSGISPyException("The number of bands specified must be 3 or 1.")

    x_size, y_size = rsgislib.imageutils.get_img_size(input_img)
    pxl_bbox = [0, x_size, 0, y_size]
    if bbox is not None:
        pxl_bbox = rsgislib.imageutils.get_img_subset_pxl_bbox(input_img, bbox)

    n_x_pxls = pxl_bbox[1] - pxl_bbox[0]
    n_y_pxls = pxl_bbox[3] - pxl_bbox[2]

    print(f"Image Data Size: {n_x_pxls} x {n_y_pxls}")

    coords_bbox = rsgislib.imageutils.get_img_pxl_spatial_coords(input_img, pxl_bbox)

    image_ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if image_ds is None:
        raise rsgislib.RSGISPyException(
            f"Could not open the input image file: '{input_img}'"
        )

    img_data_arr = image_ds.ReadAsArray(
        xoff=pxl_bbox[0],
        yoff=pxl_bbox[2],
        xsize=n_x_pxls,
        ysize=n_y_pxls,
        band_list=bands,
    )
    image_ds = None
    if len(bands) > 1:
        img_data_arr = numpy.moveaxis(img_data_arr, 0, -1)

    return img_data_arr, coords_bbox


def get_gdal_thematic_raster_mpl_imshow(
    input_img: str,
    band: int = 1,
    bbox: List[float] = None,
    out_patches=False,
    cls_names_lut=None,
) -> Tuple[numpy.array, List[float], list]:
    """
    A function which retrieves thematic image data with a colour table as an
    array in an appropriate structure for use within the matplotlib imshow function.
    The image pixel values are converted from there thematic integer values
    to a three band array using the RGB values from the colour table. If the
    pixel values are required then use the get_gdal_raster_mpl_imshow function.
    The extent is also returned and optionally a list of matplotlib patches
    which can be used to create a legend.

    :param input_img: The input image file path.
    :param band: The image band to be used for the visualisation (Default = 1).
    :param bbox: Optional bbox (xmin, xmax, ymin, ymax) used to subset the
                 input image so only data for the subset are returned.
    :param out_patches: Boolean to specify whether patches should be returned to
                        create a legend.
    :param cls_names_lut: A dictionary LUT with labels for the classes. The dict
                          key is the pixel value for the class and
    :return: numpy.array either [n,m,3], a bbox (xmin, xmax, ymin, ymax)
             specifying the extent of the image data and list of matplotlib patches,
             if out_patches=False then None is returned.

    .. code:: python

        img_sub_bbox = [554756, 577168, 9903924, 9944315]
        input_img = "class_img.kea"

        cls_names_lut = dict()
        cls_names_lut[1] = "Vegetation"
        cls_names_lut[2] = "Non-Veg"
        cls_names_lut[3] = "Productive Veg"

        (img_data_arr,
        coords_bbox,
        lgd_patches) = get_gdal_thematic_raster_mpl_imshow(input_img,
                                                           band=1,
                                                           bbox=img_sub_bbox,
                                                           out_patches=True,
                                                           cls_names_lut=cls_names_lut)


        import matplotlib.pyplot as plt
        fig, ax = plt.subplots()
        ax.imshow(img_data_arr, extent=coords_bbox)
        ax.legend(handles=lgd_patches)
        plt.show()

    """
    import rsgislib.imageutils

    n_bands = rsgislib.imageutils.get_img_band_count(input_img)
    if (band <= 0) or (band > n_bands):
        raise rsgislib.RSGISPyException(
            f"Band {band} is not valid (i.e., within the image)"
        )

    x_size, y_size = rsgislib.imageutils.get_img_size(input_img)
    pxl_bbox = [0, x_size, 0, y_size]
    if bbox is not None:
        pxl_bbox = rsgislib.imageutils.get_img_subset_pxl_bbox(input_img, bbox)

    n_x_pxls = pxl_bbox[1] - pxl_bbox[0]
    n_y_pxls = pxl_bbox[3] - pxl_bbox[2]

    print(f"Image Data Size: {n_x_pxls} x {n_y_pxls}")

    coords_bbox = rsgislib.imageutils.get_img_pxl_spatial_coords(input_img, pxl_bbox)

    image_ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if image_ds is None:
        raise rsgislib.RSGISPyException(
            f"Could not open the input image file: '{input_img}'"
        )

    clr_tab = image_ds.GetRasterBand(band).GetRasterColorTable()
    if clr_tab is None:
        raise rsgislib.RSGISPyException("No colour table was present")

    img_data_arr = image_ds.ReadAsArray(
        xoff=pxl_bbox[0],
        yoff=pxl_bbox[2],
        xsize=n_x_pxls,
        ysize=n_y_pxls,
        band_list=[band],
    )

    red_arr = numpy.zeros_like(img_data_arr, dtype=numpy.uint8)
    grn_arr = numpy.zeros_like(img_data_arr, dtype=numpy.uint8)
    blu_arr = numpy.zeros_like(img_data_arr, dtype=numpy.uint8)

    lgd_out_patches = None
    if out_patches:
        lgd_out_patches = list()

    for i in range(clr_tab.GetCount()):
        clr_tab_entry = clr_tab.GetColorEntry(i)
        red_arr[img_data_arr == i] = clr_tab_entry[0]
        grn_arr[img_data_arr == i] = clr_tab_entry[1]
        blu_arr[img_data_arr == i] = clr_tab_entry[2]

        if out_patches and (i > 0):
            cls_name = f"{i}"
            if (cls_names_lut is not None) and (i in cls_names_lut):
                cls_name = f"{cls_names_lut[i]}"
            rgb_clr = (
                clr_tab_entry[0] / 255.0,
                clr_tab_entry[1] / 255.0,
                clr_tab_entry[2] / 255.0,
            )
            lgd_out_patches.append(
                Patch(facecolor=rgb_clr, edgecolor=rgb_clr, label=cls_name)
            )

    img_clr_data_arr = numpy.stack([red_arr, grn_arr, blu_arr], axis=-1)

    image_ds = None
    img_data_arr = None

    return img_clr_data_arr, coords_bbox, lgd_out_patches


def limit_range_np_arr(
    arr_data: numpy.array,
    min_thres: float = 0,
    min_out_val: float = 0,
    max_thres: float = 1,
    max_out_val: float = 1,
) -> numpy.array:
    """
    A function which can be used to limit the range of the numpy array.
    For example, to mask values less than 0 to 0 and values greater than
    1 to 1.

    :param arr_data: input numpy array.
    :param min_thres: the threshold for the minimum value.
    :param min_out_val: the value assigned to values below the min_thres
    :param max_thres: the threshold for the maximum value.
    :param max_out_val: the value assigned to the values above the max_thres
    :return: numpy array with output values.

    """
    arr_data_out = arr_data.copy()
    arr_data_out[arr_data < min_thres] = min_out_val
    arr_data_out[arr_data > max_thres] = max_out_val
    return arr_data_out


def linear_stretch_np_arr(
    arr_data: numpy.array,
    no_data_val: float = None,
    out_off: float = 0,
    out_gain: float = 1,
    out_int_type=False,
    min_out_val: float = 0,
    max_out_val: float = 1,
) -> numpy.array:
    """
    A function which performs a linear stretch using the min-max values on a per
    band basis for a numpy array representing an image dataset. This function
    is useful in combination with get_gdal_raster_mpl_imshow for displaying
    raster data from an input image as a plot. By default this function returns
    values in a range 0 - 1 but if you prefer 0 - 255 then set the out_gain to
    255 and the out_int_type to be True to get an 8bit unsigned integer value.

    :param arr_data: The numpy array as either [n,m,b] or [n,m] where n and m are
                     the number of image pixels in the x and y axis' and b is the
                     number of image bands.
    :param no_data_val: the no data value for the input data. If there isn't a no
                        data value then leave as None (default)
    :param out_off: Output offset value (value * gain) + offset. Default: 0
    :param out_gain: Output gain value (value * gain) + offset. Default: 1
    :param out_int_type: False (default) and the output type will be float and
                         True and the output type with be integers.
    :param min_out_val: Minimum output value within the output array (default: 0)
    :param max_out_val: Maximum output value within the output array (default: 1)
    :return: A number array with the rescaled values but same dimensions as the
             input numpy array.

    .. code:: python

        img_sub_bbox = [554756, 577168, 9903924, 9944315]
        input_img = "sen2_img_strch.kea"

        img_data_arr, coords_bbox = get_gdal_raster_mpl_imshow(input_img,
                                                               bands=[8,9,3],
                                                               bbox=img_sub_bbox)

        img_data_arr = linear_stretch_np_arr(img_data_arr, no_data_val=0.0)


        import matplotlib.pyplot as plt
        fig, ax = plt.subplots()
        im = ax.imshow(img_data_arr, extent=coords_bbox)
        plt.show()

    """
    arr_shp = arr_data.shape

    if no_data_val is not None:
        arr_data_out = arr_data.astype(float)
        arr_data_out[arr_data == no_data_val] = numpy.nan
    else:
        arr_data_out = arr_data.copy()

    if len(arr_shp) == 2:
        min_val = numpy.nanmin(arr_data_out)
        max_val = numpy.nanmax(arr_data_out)
        range_val = max_val - min_val

        arr_data_out = (((arr_data_out - min_val) / range_val) * out_gain) + out_off
    else:
        n_bands = arr_shp[2]
        for n in range(n_bands):
            min_val = numpy.nanmin(arr_data_out[..., n])
            max_val = numpy.nanmax(arr_data_out[..., n])
            range_val = max_val - min_val

            arr_data_out[..., n] = (
                ((arr_data_out[..., n] - min_val) / range_val) * out_gain
            ) + out_off

    arr_data_out = limit_range_np_arr(
        arr_data_out,
        min_thres=min_out_val,
        min_out_val=min_out_val,
        max_thres=max_out_val,
        max_out_val=max_out_val,
    )

    if out_int_type:
        arr_data_out = arr_data_out.astype(int)

    return arr_data_out


def cumulative_stretch_np_arr(
    arr_data: numpy.array,
    no_data_val: float = None,
    lower: int = 2,
    upper: int = 98,
    out_off: float = 0,
    out_gain: float = 1,
    out_int_type=False,
    min_out_val: float = 0,
    max_out_val: float = 1,
) -> numpy.array:
    """
    A function which performs a cumulative stretch using an upper and lower
    percentile to define the min-max values. This analysis is on a per
    band basis for a numpy array representing an image dataset. This function
    is useful in combination with get_gdal_raster_mpl_imshow for displaying
    raster data from an input image as a plot. By default this function returns
    values in a range 0 - 1 but if you prefer 0 - 255 then set the out_gain to
    255 and the out_int_type to be True to get an 8bit unsigned integer value.

    :param arr_data: The numpy array as either [n,m,b] or [n,m] where n and m are
                     the number of image pixels in the x and y axis' and b is the
                     number of image bands.
    :param no_data_val: the no data value for the input data. If there isn't a no
                        data value then leave as None (default)
    :param lower: lower percentile (default: 2)
    :param upper: upper percentile (default: 98)
    :param out_off: Output offset value (value * gain) + offset. Default: 0
    :param out_gain: Output gain value (value * gain) + offset. Default: 1
    :param out_int_type: False (default) and the output type will be float and
                         True and the output type with be integers.
    :param min_out_val: Minimum output value within the output array (default: 0)
    :param max_out_val: Maximum output value within the output array (default: 1)
    :return: A number array with the rescaled values but same dimensions as the
             input numpy array.

    .. code:: python

        img_sub_bbox = [554756, 577168, 9903924, 9944315]
        input_img = "sen2_img_strch.kea"

        img_data_arr, coords_bbox = get_gdal_raster_mpl_imshow(input_img,
                                                               bands=[8,9,3],
                                                               bbox=img_sub_bbox)

        img_data_arr = cumulative_stretch_np_arr(img_data_arr, no_data_val=0.0)

        import matplotlib.pyplot as plt
        fig, ax = plt.subplots()
        im = ax.imshow(img_data_arr, extent=coords_bbox)
        plt.show()

    """
    arr_shp = arr_data.shape

    if no_data_val is not None:
        arr_data_out = arr_data.astype(float)
        arr_data_out[arr_data == no_data_val] = numpy.nan
    else:
        arr_data_out = arr_data.copy()

    if len(arr_shp) == 2:
        min_val, max_val = numpy.nanpercentile(arr_data_out, [lower, upper])
        range_val = max_val - min_val

        arr_data_out = (((arr_data_out - min_val) / range_val) * out_gain) + out_off
    else:
        n_bands = arr_shp[2]
        for n in range(n_bands):
            min_val, max_val = numpy.nanpercentile(arr_data_out[..., n], [lower, upper])
            range_val = max_val - min_val

            arr_data_out[..., n] = (
                ((arr_data_out[..., n] - min_val) / range_val) * out_gain
            ) + out_off

    arr_data_out = limit_range_np_arr(
        arr_data_out,
        min_thres=min_out_val,
        min_out_val=min_out_val,
        max_thres=max_out_val,
        max_out_val=max_out_val,
    )

    if out_int_type:
        arr_data_out = arr_data_out.astype(int)

    return arr_data_out


def stdev_stretch_np_arr(
    arr_data: numpy.array,
    no_data_val: float = None,
    n_stdevs: float = 2.0,
    out_off: float = 0,
    out_gain: float = 1,
    out_int_type=False,
    min_out_val: float = 0,
    max_out_val: float = 1,
) -> numpy.array:
    """
    A function which performs a standard deviation stretch using an upper and lower
    (mean + n*std) and (mean - n*std) to define the min-max values. This analysis
    is on a per band basis for a numpy array representing an image dataset.
    This function is useful in combination with get_gdal_raster_mpl_imshow for
    displaying raster data from an input image as a plot. By default this function
    returns values in a range 0 - 1 but if you prefer 0 - 255 then set the out_gain
    to 255 and the out_int_type to be True to get an 8bit unsigned integer value.

    :param arr_data: The numpy array as either [n,m,b] or [n,m] where n and m are
                     the number of image pixels in the x and y axis' and b is the
                     number of image bands.
    :param no_data_val: the no data value for the input data. If there isn't a no
                        data value then leave as None (default)
    :param n_stdevs: number of standard deviations to be used for the stretch.
                     Default: 2.0
    :param out_off: Output offset value (value * gain) + offset. Default: 0
    :param out_gain: Output gain value (value * gain) + offset. Default: 1
    :param out_int_type: False (default) and the output type will be float and
                         True and the output type with be integers.
    :param min_out_val: Minimum output value within the output array (default: 0)
    :param max_out_val: Maximum output value within the output array (default: 1)
    :return: A number array with the rescaled values but same dimensions as the
             input numpy array.

    .. code:: python

        img_sub_bbox = [554756, 577168, 9903924, 9944315]
        input_img = "sen2_img_strch.kea"

        img_data_arr, coords_bbox = get_gdal_raster_mpl_imshow(input_img,
                                                               bands=[8,9,3],
                                                               bbox=img_sub_bbox)

        img_data_arr = stdev_stretch_np_arr(img_data_arr, no_data_val=0.0)

        import matplotlib.pyplot as plt
        fig, ax = plt.subplots()
        im = ax.imshow(img_data_arr, extent=coords_bbox)
        plt.show()

    """
    arr_shp = arr_data.shape

    if no_data_val is not None:
        arr_data_out = arr_data.astype(float)
        arr_data_out[arr_data == no_data_val] = numpy.nan
    else:
        arr_data_out = arr_data.copy()

    if len(arr_shp) == 2:
        min_val = numpy.nanmin(arr_data_out)
        max_val = numpy.nanmax(arr_data_out)
        mean_val = numpy.nanmean(arr_data_out)
        std_val = numpy.nanstd(arr_data_out)
        low_val = mean_val - (std_val * n_stdevs)
        up_val = mean_val + (std_val * n_stdevs)
        if low_val < min_val:
            low_val = min_val
        if up_val > max_val:
            up_val = max_val

        range_val = up_val - low_val

        arr_data_out = (((arr_data_out - low_val) / range_val) * out_gain) + out_off
    else:
        n_bands = arr_shp[2]
        for n in range(n_bands):
            min_val = numpy.nanmin(arr_data_out[..., n])
            max_val = numpy.nanmax(arr_data_out[..., n])
            mean_val = numpy.nanmean(arr_data_out[..., n])
            std_val = numpy.nanstd(arr_data_out[..., n])
            low_val = mean_val - (std_val * n_stdevs)
            up_val = mean_val + (std_val * n_stdevs)
            if low_val < min_val:
                low_val = min_val
            if up_val > max_val:
                up_val = max_val

            range_val = up_val - low_val

            arr_data_out[..., n] = (
                ((arr_data_out[..., n] - low_val) / range_val) * out_gain
            ) + out_off

    arr_data_out[arr_data_out < out_off] = out_off
    arr_data_out[arr_data_out > (out_gain + out_off)] = out_gain + out_off

    arr_data_out = limit_range_np_arr(
        arr_data_out,
        min_thres=min_out_val,
        min_out_val=min_out_val,
        max_thres=max_out_val,
        max_out_val=max_out_val,
    )

    if out_int_type:
        arr_data_out = arr_data_out.astype(int)

    return arr_data_out


def manual_stretch_np_arr(
    arr_data: numpy.array,
    min_max_vals: Union[Dict, List[Dict]],
    no_data_val: float = None,
    out_off: float = 0,
    out_gain: float = 1,
    out_int_type=False,
    min_out_val: float = 0,
    max_out_val: float = 1,
) -> numpy.array:
    """
    A function which performs a linear stretch using the min-max values provided
    on a per band basis for a numpy array representing an image dataset. This
    function is useful in combination with get_gdal_raster_mpl_imshow for
    displaying raster data from an input image as a plot. By default this function
    returns values in a range 0 - 1 but if you prefer 0 - 255 then set the out_gain
    to 255 and the out_int_type to be True to get an 8bit unsigned integer value.

    :param arr_data: The numpy array as either [n,m,b] or [n,m] where n and m are
                     the number of image pixels in the x and y axis' and b is the
                     number of image bands.
    :param min_max_vals: either a list of dicts each with a 'min' and 'max' key
                         specifying the min and max value for the stretch of each
                         band. Or, if just a single band then provide a single
                         dict rather than a list. The number items in the list
                         must equal the number of dimensions within the arr_data.
    :param no_data_val: the no data value for the input data. If there isn't a no
                        data value then leave as None (default)
    :param out_off: Output offset value (value * gain) + offset. Default: 0
    :param out_gain: Output gain value (value * gain) + offset. Default: 1
    :param out_int_type: False (default) and the output type will be float and
                         True and the output type with be integers.
    :param min_out_val: Minimum output value within the output array (default: 0)
    :param max_out_val: Maximum output value within the output array (default: 1)
    :return: A number array with the rescaled values but same dimensions as the
             input numpy array.

    .. code:: python

        img_sub_bbox = [554756, 577168, 9903924, 9944315]
        input_img = "sen2_img_strch.kea"

        img_data_arr, coords_bbox = get_gdal_raster_mpl_imshow(input_img,
                                                               bands=[8,9,3],
                                                               bbox=img_sub_bbox)

        min_max_vals = list()
        min_max_vals.append({'min':10, 'max':400})
        min_max_vals.append({'min':22, 'max':300})
        min_max_vals.append({'min':1, 'max':120})

        img_data_arr = manual_stretch_np_arr(img_data_arr,
                                             min_max_vals,
                                             no_data_val=0.0)


        import matplotlib.pyplot as plt
        fig, ax = plt.subplots()
        im = ax.imshow(img_data_arr, extent=coords_bbox)
        plt.show()

    """
    arr_shp = arr_data.shape

    if no_data_val is not None:
        arr_data_out = arr_data.astype(float)
        arr_data_out[arr_data == no_data_val] = numpy.nan
    else:
        arr_data_out = arr_data.copy()

    if len(arr_shp) == 2:
        if type(min_max_vals) is not dict:
            raise rsgislib.RSGISPyException(
                "Just 1 dimension within arr_data and therefore "
                "min_max_vals variable must be a dict."
            )

        if ("min" not in min_max_vals) or ("max" not in min_max_vals):
            raise rsgislib.RSGISPyException(
                "min and max keys must be provided within the dict"
            )

        min_val = min_max_vals["min"]
        max_val = min_max_vals["max"]
        range_val = max_val - min_val

        arr_data_out = (((arr_data_out - min_val) / range_val) * out_gain) + out_off
    else:
        if type(min_max_vals) is not list:
            raise rsgislib.RSGISPyException(
                "arr_data has more than 1 dimension and therefore "
                "min_max_vals variable must be a list."
            )

        n_bands = arr_shp[2]

        if n_bands != len(min_max_vals):
            raise rsgislib.RSGISPyException(
                "length of min_max_vals must be the same as the number "
                "of bands in arr_data."
            )

        for n in range(n_bands):
            if ("min" not in min_max_vals[n]) or ("max" not in min_max_vals[n]):
                raise rsgislib.RSGISPyException(
                    "min and max keys must be provided within the dict"
                )

            min_val = min_max_vals[n]["min"]
            max_val = min_max_vals[n]["max"]
            range_val = max_val - min_val

            arr_data_out[..., n] = (
                ((arr_data_out[..., n] - min_val) / range_val) * out_gain
            ) + out_off

    arr_data_out = limit_range_np_arr(
        arr_data_out,
        min_thres=min_out_val,
        min_out_val=min_out_val,
        max_thres=max_out_val,
        max_out_val=max_out_val,
    )

    if out_int_type:
        arr_data_out = arr_data_out.astype(int)

    return arr_data_out
