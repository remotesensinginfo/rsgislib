#!/usr/bin/env python
"""
The tools.plotting module contains functions for extracting and plotting remote sensing data.
"""

import os
from typing import Dict, List, Tuple, Union

import matplotlib.colors as mClrs
import matplotlib.pyplot as plt
import numpy
from matplotlib import gridspec, rcParams
from matplotlib.patches import Patch
from osgeo import gdal

import rsgislib
from rsgislib import imagecalc, zonalstats

have_mpl_scatter_density = True
try:
    import mpl_scatter_density
except ImportError:
    have_mpl_scatter_density = False


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
    :param vec_file: is the region of interest as a vector file -
                     if multiple polygons are defined the spectra for each
                     will be added to the plot.
    :param vec_lyr: is the region of interest vector layer
    :param output_plot_file: is the output image file for the plot
                             which has been create
    :param wavelengths: is list of numbers with the wavelength of each band
                        (must have the same number of wavelengths as image bands)
    :param plot_title: is a string with the title for the plot
    :param scale_factor: is a float specifying the scaling to percentage (0 - 100).
                         (Default is 0.1, i.e., pixel values are scaled between 0-1000;
                         ARCSI default).
    :param show_refl_std: is a boolean (default: True) to specify whether a shaded
                          region showing 1 standard deviation from the mean on the
                          plot alongside the mean spectra.
    :param refl_max: is a parameter for setting the maximum reflectance value on
                     the Y axis (if None the maximum value in the dataset is used

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

    dataset = gdal.Open(input_img, gdal.GA_ReadOnly)
    num_bands = dataset.RasterCount
    dataset = None

    if not len(wavelengths) == num_bands:
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

    mean_vals = []
    std_dev_vals = []
    stats = open(tmpOutFile, "r")
    row = 0
    for stats_row in stats:
        stats_row = stats_row.strip()
        if row > 0:
            mean_val = []
            std_dev_val = []
            data = stats_row.split(",")

            if not len(data) == (num_bands * 2) + 2:
                raise rsgislib.RSGISPyException(
                    "The number of outputted stats values is incorrect!"
                )
            for band in range(num_bands):
                mean_val.append(float(data[(band * 2) + 1]) * scale_factor)
                std_dev_val.append(float(data[(band * 2) + 2]) * scale_factor)
            mean_vals.append(mean_val)
            std_dev_vals.append(std_dev_val)
        row += 1
    stats.close()

    # print("Mean: ", mean_vals)
    # print("Std Dev: ", std_dev_vals)
    print("Creating Plot")
    fig = plt.figure(figsize=(7, 5), dpi=80)
    ax1 = fig.add_subplot(111)
    for feat in range(len(mean_vals)):
        ax1.plot(wavelengths, mean_vals[feat], "k-", zorder=10)
        if show_refl_std:
            lower_vals = []
            upper_vals = []
            for band in range(num_bands):
                lower_vals.append(mean_vals[feat][band] - std_dev_vals[feat][band])
                upper_vals.append(mean_vals[feat][band] + std_dev_vals[feat][band])
            ax1.fill_between(
                wavelengths,
                lower_vals,
                upper_vals,
                alpha=0.2,
                linewidth=1.0,
                facecolor=[0.70, 0.70, 0.70],
                edgecolor=[0.70, 0.70, 0.70],
                zorder=-1,
            )

    ax1_range = ax1.axis("tight")

    if refl_max is None:
        ax1.axis((ax1_range[0], ax1_range[1], 0, ax1_range[3]))
    else:
        ax1.axis((ax1_range[0], ax1_range[1], 0, refl_max))

    plt.grid(color="k", linestyle="--", linewidth=0.5)
    plt.title(plot_title)
    plt.xlabel("Wavelength")
    plt.ylabel("Reflectance (%)")

    plt.savefig(output_plot_file, format="PDF")
    rsgislib.tools.filetools.delete_file_silent(tmpOutFile)
    print("Completed.\n")


def plot_image_comparison(
    input_image1,
    input_image2,
    img1_band,
    img2_band,
    output_plot_file,
    num_bins=100,
    img1_min=None,
    img1_max=None,
    img2_min=None,
    img2_max=None,
    img1_scale=1,
    img2_scale=1,
    img1_off=0,
    img2_off=0,
    norm_output=False,
    plot_title="2D Histogram",
    x_label="X Axis",
    y_label="Y Axis",
    ctable="jet",
    interp="nearest",
):
    """
    A function to plot two images against each other.

    :param input_image1: is a string with the path to the first image.
    :param input_image2: is a string with the path to the second image.
    :param img1_band: is an int specifying the band in the first image to be plotted.
    :param img2_band: is an int specifying the band in the second image to be plotted.
    :param output_plot_file: is a string specifying the output image file for the plot.
                             Note, the output format is defined using the image
                             extension. (e.g., img.png would output a PNG image).
    :param num_bins: is an int specifying the number of bins within each axis of the
                     histogram (default: 100)
    :param img1_min: is a double specifying the minimum value to be used in the
                     histogram from image 1. If value is None then taken from the image.
    :param img1_max: is a double specifying the maximum value to be used in the
                     histogram from image 1. If value is None then taken from the image.
    :param img2_min: is a double specifying the minimum value to be used in the
                     histogram from image 2. If value is None then taken from the image.
    :param img2_max: is a double specifying the maximum value to be used in the
                     histogram from image 2. If value is None then taken from the image.
    :param img1_scale: is a double specifying the scale for image 1 (Default 1).
    :param img2_scale: is a double specifying the scale for image 2 (Default 1).
    :param img1_off: is a double specifying the offset for image 1 (Default 0).
    :param img2_off: is a double specifying the offset for image 2 (Default 0).
    :param norm_output: is a boolean specifying whether the histogram should be
                        normalised (Default: False).
    :param plot_title: is a string specifying the title of the plot
                       (Default: '2D Histogram').
    :param x_label: is a string specifying the x axis label (Default: 'X Axis')
    :param y_label: is a string specifying the y axis label (Default: 'Y Axis')
    :param ctable: is a string specifying the colour table to be used (Default: jet),
                   list of available colour tables specified by matplotlib:
                   http://matplotlib.org/examples/color/colormaps_reference.html
    :param interp: is a string specifying the interpolation algorithm to be used
                   (Default: 'nearest'). Available values are ‘none’, ‘nearest’,
                   ‘bilinear’, ‘bicubic’, ‘spline16’, ‘spline36’, ‘hanning’,
                   ‘hamming’, ‘hermite’, ‘kaiser’, ‘quadric’, ‘catrom’, ‘gaussian’,
                   ‘bessel’, ‘mitchell’, ‘sinc’, ‘lanczos’.

    .. code:: python

        from rsgislib.tools import plotting

        inputImage1 = 'LS5TM_20000613_lat10lon6217_r67p231_rad_sref_ndvi.kea'
        inputImage2 = 'LS5TM_20000613_lat10lon6217_r67p231_rad_ndvi.kea'
        outputPlotFile = 'ARCSI_RAD_SREF_NDVI.pdf'

        plotting.plot_image_comparison(inputImage1, inputImage2, 1, 1, outputPlotFile, img1Min=-0.5, img1Max=1, img2Min=-0.5, img2Max=1, plotTitle='ARCSI SREF NDVI vs ARCSI RAD NDVI', xLabel='ARCSI SREF NDVI', yLabel='ARCSI RAD NDVI')

    """
    gdalformat = "KEA"
    tmp_out_file = os.path.splitext(output_plot_file)[0] + "_hist2dimgtmp.kea"

    if (img1_min is None) or (img1_max is None):
        # Calculate image 1 stats
        img_gdalds = gdal.Open(input_image1, gdal.GA_ReadOnly)
        img_gdal_band = img_gdalds.GetRasterBand(img1_band)
        min, max = img_gdal_band.ComputeRasterMinMax(False)
        img_gdalds = None
        if img1_min is None:
            img1_min = min
        if img1_max is None:
            img1_max = max

    if (img2_min is None) or (img2_max is None):
        # Calculate image 2 stats
        img_gdalds = gdal.Open(input_image2, gdal.GA_ReadOnly)
        img_gdal_band = img_gdalds.GetRasterBand(img2_band)
        min, max = img_gdal_band.ComputeRasterMinMax(False)
        img_gdalds = None
        if img2_min is None:
            img2_min = min
        if img2_max is None:
            img2_max = max

    # Note. Y axis is flipped.
    out_bin_size_img1, out_bin_size_img2, rSq = rsgislib.imagecalc.get_2d_img_histogram(
        input_image2,
        input_image1,
        tmp_out_file,
        gdalformat,
        img2_band,
        img1_band,
        num_bins,
        img2_min,
        img2_max,
        img1_min,
        img1_max,
        img2_scale,
        img1_scale,
        img2_off,
        img1_off,
        norm_output,
    )
    print("Image1 Bin Size: ", out_bin_size_img1)
    print("Image2 Bin Size: ", out_bin_size_img2)
    print("rSq: ", rSq)

    print("Read Image Data")
    plot_gdal_img = gdal.Open(tmp_out_file, gdal.GA_ReadOnly)
    plot_img_band = plot_gdal_img.GetRasterBand(1)
    data_arr = plot_img_band.ReadAsArray().astype(float)
    print(data_arr.shape)
    data_arr = numpy.flipud(data_arr)
    print(data_arr.shape)
    plot_gdal_img = None

    fig = plt.figure(figsize=(7, 7), dpi=80)
    ax1 = fig.add_subplot(111)

    img1_min_sc = img1_off + (img1_min * img1_scale)
    img1_max_sc = img1_off + (img1_max * img1_scale)
    img2_min_sc = img2_off + (img2_min * img2_scale)
    img2_max_sc = img2_off + (img2_max * img2_scale)

    min_val = numpy.min(data_arr[data_arr != 0])
    max_val = numpy.max(data_arr)

    print("Min Value: ", min_val)
    print("Max Value: ", max_val)

    cmap = plt.get_cmap(ctable)
    mClrs.Colormap.set_under(cmap, color="white")
    mClrs.Colormap.set_over(cmap, color="white")

    im_plot = plt.imshow(
        data_arr,
        cmap=cmap,
        aspect="equal",
        interpolation=interp,
        norm=mClrs.Normalize(vmin=min_val, vmax=max_val),
        extent=[img1_min_sc, img1_max_sc, img2_min_sc, img2_max_sc],
    )
    plt.grid(color="k", linestyle="--", linewidth=0.5)
    r_sq_str = ""
    if rSq < 0:
        rSq = 0.00
    r_sq_str = "$r^2 = " + str(round(rSq, 3)) + "$"
    plt.text(0.05, 0.95, r_sq_str, va="center", transform=ax1.transAxes)
    fig.colorbar(im_plot)
    plt.title(plot_title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)

    plt.savefig(output_plot_file)

    # Tidy up temporary file.
    gdal_driver = gdal.GetDriverByName(gdalformat)
    gdal_driver.Delete(tmp_out_file)



def plot_image_histogram(
    input_img,
    img_band,
    output_plot_file,
    num_bins=100,
    img_min=None,
    img_max=None,
    norm_output=False,
    plot_title="Histogram",
    x_label="X Axis",
    colour="blue",
    edgecolour="black",
    linewidth=None,
):
    """
    A function to plot the histogram of an image.

    :param input_img: is a string with the path to the image.
    :param img_band: is an int specifying the band in the image to be plotted.
    :param output_plot_file: is a string specifying the output image file for the plot.
                             Note, the output format is defined using the image
                             extension. (e.g., img.png would output a PNG image).
    :param num_bins: is an int specifying the number of bins within each axis of
                     the histogram (default: 100)
    :param img_min: is a double specifying the minimum value to be used in the
                    histogram from the image. If value is None then taken from
                    the image.
    :param img_max: is a double specifying the maximum value to be used in the
                    histogram from the image. If value is None then taken from
                    the image.
    :param norm_output: is a boolean specifying whether the histogram should be
                        normalised (Default: False).
    :param plot_title: is a string specifying the title of the plot
                       (Default: '2D Histogram').
    :param x_label: is a string specifying the x axis label (Default: 'X Axis')
    :param colour: is the colour of the bars in the plot (see matplotlib documentation
                   for how to specify, either keyword or RGB values (e.g., [1.0,0,0])
    :param edgecolour: is the colour of the edges of the bars
    :param linewidth: is the thickness of the edges of the bars in the plot.

    .. code:: python

        from rsgislib.tools import plotting

        plotting.plot_image_histogram("Baccini_Manaus_AGB_30.kea", 1, "BacciniHistogram.pdf", numBins=100, imgMin=0, imgMax=400, normOutput=True, plotTitle='Histogram of Baccini Biomass', xLabel='Baccini Biomass', color=[1.0,0.2,1.0], edgecolor='red', linewidth=0)

    """
    if (img_min is None) or (img_max is None):
        # Calculate image 1 stats
        imgGDALDS = gdal.Open(input_img, gdal.GA_ReadOnly)
        imgGDALBand = imgGDALDS.GetRasterBand(img_band)
        min, max = imgGDALBand.ComputeRasterMinMax(False)
        imgGDALDS = None
        if img_min is None:
            img_min = min
        if img_max is None:
            img_max = max

    binWidth = (img_max - img_min) / num_bins
    print("Bin Size: ", binWidth)

    bins, hMin, hMax = imagecalc.get_histogram(
        input_img, img_band, binWidth, False, img_min, img_max
    )

    if norm_output:
        sumBins = numpy.sum(bins)
        bins = bins / sumBins

    num_bins = len(bins)
    xLocs = numpy.arange(num_bins)
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
    plt.xlim(img_min, img_max)

    plt.title(plot_title)
    plt.xlabel(x_label)
    plt.ylabel("Freq.")
    plt.savefig(output_plot_file)


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


def residual_density_plot(
    y_true: numpy.array,
    residuals: numpy.array,
    out_file: str,
    out_format: str = "PNG",
    out_dpi: int = 800,
    title: str = None,
    cmap_name: str = "viridis",
    use_log_norm: bool = False,
    density_norm_vmin: float = 1,
    density_norm_vmax: float = None,
    freq_nbins: int = 50,
    val_plt_range: List[float] = None,
    resid_plt_range: List[float] = None,
):
    """
    A function to create a residual plot where the scatter plot will be represented
    as a density plot. This plot allows the investigatation of the
    normality and homoscedasticity of model residuals.

    :param y_true: A numpy 1D array containing true/observed values.
    :param residuals: A numpy 1D array containing model residuals.
    :param out_file: Path to the output file.
    :param out_format: Output format supported by matplotlib (e.g. "PNG" or "PDF").
                       Default: PNG
    :param out_dpi: the output DPI of the save raster plot (default: 800)
    :param title: A title for the plot. Optional, if None then ignored. (Default: None)
    :param cmap_name: The name of the colour bar to use for the density plot
                      Default: viridis
    :param use_log_norm: Specify whether to use log normalisation for the density plot
                         instead of linear. (Default: False)
    :param density_norm_vmin: the minimum density value for the normalisation
                              (default: 1)
    :param density_norm_vmax: the maximum density value for the normalisation
                              (default: None)
    :param freq_nbins: the number of bins used for the frequency histogram (Default: 50)
    :param val_plt_range: A user specified x-axis range of values (Default: None). If
                          specified then must be a list of 2 values.
    :param resid_plt_range: A user specified y-axis range of values (Default: None) If
                            specified then must be a list of 2 values.

    """
    if not have_mpl_scatter_density:
        raise rsgislib.RSGISPyException(
            "The mpl_scatter_density module is required and could not be imported."
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
    if val_plt_range is not None:
        if len(val_plt_range) != 2:
            raise rsgislib.RSGISPyException("val_plt_range must have len of 2")
    if resid_plt_range is not None:
        if len(resid_plt_range) != 2:
            raise rsgislib.RSGISPyException("resid_plt_range must have len of 2")

    c_cmap = plt.get_cmap(cmap_name)
    mClrs.Colormap.set_under(c_cmap, color="white")
    if use_log_norm:
        c_norm = mClrs.LogNorm(vmin=density_norm_vmin, vmax=density_norm_vmax)
    else:
        c_norm = mClrs.Normalize(vmin=density_norm_vmin, vmax=density_norm_vmax)

    # setup plot:
    # rcParams.update({'font.family': 'cmr10'})  # use latex fonts.
    # rcParams['axes.unicode_minus'] = False
    rcParams.update({"font.size": 8.5})
    rcParams["axes.linewidth"] = 0.5
    rcParams["xtick.major.pad"] = "2"
    rcParams["ytick.major.pad"] = "2"
    fig = plt.figure(figsize=(10, 5))
    gs = gridspec.GridSpec(nrows=1, ncols=2, width_ratios=[3.5, 1])
    ax1 = plt.subplot(gs[0], projection="scatter_density")
    ax2 = plt.subplot(gs[1])
    plt.tight_layout(w_pad=-1, h_pad=0)

    # draw scatterplot:
    ax1.axhline(y=0.0, c="k", ls=":", lw=0.5, zorder=2)
    ax1.scatter_density(y_true, residuals, norm=c_norm, cmap=c_cmap, zorder=1)
    ax1.set_xlabel("Observed value", fontsize=9)
    ax1.set_ylabel("Residuals", fontsize=9)
    if val_plt_range is not None:
        ax1.set_xlim(val_plt_range[0], val_plt_range[1])
    if resid_plt_range is not None:
        ax1.set_ylim(resid_plt_range[0], resid_plt_range[1])
    if title is not None:
        ax1.set_title(title)

    # draw histogram:
    ax2.get_xaxis().tick_bottom()
    ax2.get_yaxis().tick_right()
    ax2.get_yaxis().set_visible(False)
    ax2.hist(residuals, bins=freq_nbins, orientation="horizontal", color="C0")
    ax2.axhline(y=0.0, c="k", ls=":", lw=0.5, zorder=2)
    ax2.set_xlabel("Frequency", fontsize=9)
    if resid_plt_range is not None:
        ax2.set_ylim(resid_plt_range[0], resid_plt_range[1])
    plt.savefig(out_file, format=out_format, dpi=out_dpi, bbox_inches="tight")
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
    n_bins: int = None,
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
    :parma n_bins: specify the number of bins within the histogram. If None (default)
                   then the number of bins will be calculated from the data.

    """
    import rsgislib
    import rsgislib.tools.stats

    if n_bins is None:
        n_bins, bin_width = rsgislib.tools.stats.get_nbins_histogram(data)

    plt.figure()
    plt.hist(data, bins=n_bins, color=hist_clr)
    if threshold is not None:
        plt.axvline(x=threshold, color="red")
    if title_str is not None:
        plt.title(title_str)
    plt.savefig(out_file, format=out_format, dpi=plot_dpi)


def plot_vec_fields(
    vec_file: str,
    vec_lyr: str,
    out_plot_file: str,
    x_plt_field: str,
    y_plt_field: str,
    x_lbl: str,
    y_lbl: str,
    title: str,
    feat_id_field: str = None,
    x_field_no_data: float = None,
    y_field_no_data: float = None,
    plt_width: int = 18,
    plt_height: int = 6,
    plt_line: bool = True,
):
    """
    A function which plots two variables from a vector layer.

    :param vec_file: input vector file
    :param vec_lyr: input vector layer name
    :param out_plot_file: output plot file name - specify format with extension.
    :param x_plt_field: name of the field used for x-axis
    :param y_plt_field: name of the field used for the y-axis
    :param x_lbl: label for the x-axis
    :param y_lbl: label for the y-axis
    :param title: title of the plot
    :param plt_width: optionally specify the figure width
    :param plt_height: optionally specify the figure height
    :param plt_line: if true (default) then drawn as line plot otherwise a scatter
                     plot.


    """
    import geopandas

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)

    x_vals = data_gdf[x_plt_field].values
    y_vals = data_gdf[y_plt_field].values

    if x_field_no_data is not None:
        x_vld_msk = x_vals != x_field_no_data
    else:
        x_vld_msk = numpy.ones_like(x_vals, dtype=bool)
    if y_field_no_data is not None:
        y_vld_msk = y_vals != y_field_no_data
    else:
        y_vld_msk = numpy.ones_like(y_vals, dtype=bool)

    vld_vals = numpy.logical_and(x_vld_msk, y_vld_msk)
    x_vals = x_vals[vld_vals]
    y_vals = y_vals[vld_vals]

    unq_feat_ids = [None]
    if feat_id_field is not None:
        feat_ids = data_gdf[feat_id_field].values
        feat_ids = feat_ids[vld_vals]
        unq_feat_ids = numpy.unique(feat_ids)

    fig, ax = plt.subplots(figsize=(plt_width, plt_height))
    for feat_id in unq_feat_ids:
        if feat_id is not None:
            feat_x_vals = x_vals[feat_ids == feat_id]
            feat_y_vals = y_vals[feat_ids == feat_id]
        else:
            feat_x_vals = x_vals
            feat_y_vals = y_vals

        if plt_line:
            ax.plot(feat_x_vals, feat_y_vals, label=f"Feature {feat_id}")
        else:
            ax.scatter(feat_x_vals, feat_y_vals, label=f"Feature {feat_id}")

    ax.set_xlabel(x_lbl)
    ax.set_ylabel(y_lbl)
    ax.set_title(title)
    if feat_id_field is not None:
        plt.legend()
    fig.tight_layout()
    plt.savefig(out_plot_file)


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
    out_patches: bool = False,
    cls_names_lut: Dict = None,
    alpha_lyr: bool = False,
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
    :param alpha_lyr: a boolean specifying whether an alpha channel should be
                      created and therefore the returned array will have 4
                      rather than 3 dims. If an alpha channel is created then
                      then background will be transparent.
    :return: numpy.array either [n,m,3 or 4], a bbox (xmin, xmax, ymin, ymax)
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
    if alpha_lyr:
        alp_arr = numpy.zeros_like(img_data_arr, dtype=numpy.uint8)

    lgd_out_patches = None
    if out_patches:
        lgd_out_patches = list()

    for i in range(clr_tab.GetCount()):
        clr_tab_entry = clr_tab.GetColorEntry(i)
        red_arr[img_data_arr == i] = clr_tab_entry[0]
        grn_arr[img_data_arr == i] = clr_tab_entry[1]
        blu_arr[img_data_arr == i] = clr_tab_entry[2]
        if alpha_lyr and (i > 0):
            alp_arr[img_data_arr == i] = 255

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

    if alpha_lyr:
        img_clr_data_arr = numpy.stack([red_arr, grn_arr, blu_arr, alp_arr], axis=-1)
    else:
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
        arr_data_out = arr_data_out.astype(float)

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
        arr_data_out = arr_data_out.astype(float)

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
        arr_data_out = arr_data_out.astype(float)

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
        arr_data_out = arr_data_out.astype(float)

    if len(arr_shp) == 2:
        if not isinstance(min_max_vals, dict):
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
        if not isinstance(min_max_vals, list):
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


def create_legend_img(
    legend_info: Dict,
    n_cols: int = 1,
    box_size: Tuple[int] = (10, 20),
    title_str: str = None,
    font_size: int = 12,
    font: str = None,
    font_clr: Tuple[int] = (0, 0, 0, 255),
    col_width: int = None,
    img_height: int = None,
    char_width: int = 6,
    bkgd_clr: Tuple[int] = (255, 255, 255, 255),
    title_height: int = 16,
    margin: int = 2,
):
    """
    A function which can generate a legend PIL image object. Colours can be
    specified using any format PIL supports (i.e., hex or list 3 or 4 values).
    The output image also has an alpha channel.

    :param legend_info: dict using the class names as the key and value is the colour
                        used for the class.
    :param n_cols: the number of columns the classes are split between (Default: 1).
    :param box_size: the size, in pixels, of the colour box for each class.
                     Default: (10, 20)
    :param title_str: An optional title for the legend. If None then no title,
                      default: None
    :param font_size: The size of the font to use for the legend (Default: 12)
    :param font: Optionally, pass a ttf file or font name for the font to be used.
                 Default: Arial
    :param font_clr: The font colour (Default: (0, 0, 0, 255) i.e., black)
    :param col_width: Override the calculated column width in pixels. (Default: None)
    :param img_height: Override the calculated image height in pixels. (Default: None)
    :param char_width: Define the number of pixels representing each character used
                       for calculating column widths. Try changing this before
                       overriding the column width. (Default: 6)
    :param bkgd_clr: the background colour for the legend
                     (Default: (255, 255, 255, 255) i.e., white). Note, this uses
                     an alpha channel so specifying (255, 255, 255, 0) will provide
                     a transparent background
    :param title_height: Extra height in pixels for the title (Default: 16)
    :param margin: The margin in pixels around the image each and between features
                   (Default: 2)
    :return: A PIL image object.

    """
    import math

    from PIL import Image, ImageDraw, ImageFont

    n_cls = len(legend_info)
    if n_cols > n_cls:
        n_cols = n_cls

    n_cls_col = math.ceil(n_cls / n_cols)

    cls_name_max_len = 0
    for cls_name in legend_info:
        if len(cls_name) > cls_name_max_len:
            cls_name_max_len = len(cls_name)
    print(f"Max. characters in class name: {cls_name_max_len}")

    if col_width is None:
        col_width = box_size[0] + margin + (cls_name_max_len * char_width)
    print(f"Column width: {col_width}")

    if img_height is None:
        img_height = (n_cls_col * (box_size[1] + (margin * 2))) + 10

    if title_str is not None:
        img_height += title_height

    img_width = (margin * 2) + (((margin * 2) + col_width) * n_cols)

    print(f"Image: {img_width} x  {img_height}")

    img_obj = Image.new("RGBA", (img_width, img_height), color=bkgd_clr)
    draw_obj = ImageDraw.Draw(img_obj)

    if font is not None:
        fnt = ImageFont.truetype(font, font_size)
    else:
        fnt = None

    if title_str is not None:
        title_pos = [img_width / 2, margin]
        draw_obj.text(title_pos, title_str, fill=font_clr, anchor="mt", font=fnt)

    cls_names = list(legend_info.keys())
    cls_i = 0
    x_pos = 0
    for col_i in range(n_cols):
        if col_i == 0:
            # Start from pixel margin
            x_pos = margin
        else:
            # From previous pos add pixel margin for each column and column width
            x_pos = x_pos + (margin * 2) + col_width
        # Pixel margin
        y_pos = margin
        if title_str is not None:
            y_pos += title_height
        for row_i in range(n_cls_col):
            cls_name = cls_names[cls_i]
            rec_bbox = [x_pos, y_pos, x_pos + box_size[0], y_pos + box_size[1]]
            draw_obj.rectangle(
                rec_bbox, fill=legend_info[cls_name], outline=None, width=1
            )
            txt_pos = [x_pos + margin + box_size[0], y_pos + box_size[1] / 2]
            draw_obj.text(txt_pos, cls_name, fill=font_clr, anchor="lm", font=fnt)

            y_pos = y_pos + (margin * 2) + box_size[1]
            cls_i += 1
            if cls_i >= n_cls:
                break

    return img_obj


def create_legend_img_file(
    legend_info: Dict,
    out_img_file: str,
    n_cols: int = 1,
    box_size: Tuple[int] = (10, 20),
    title_str: str = None,
    font_size: int = 12,
    font: str = None,
    font_clr: Tuple[int] = (0, 0, 0, 255),
    col_width: int = None,
    img_height: int = None,
    char_width: int = 6,
    bkgd_clr: Tuple[int] = (255, 255, 255, 255),
    title_height: int = 16,
    margin: int = 2,
):
    """
    A function which uses the create_legend_img function to generate a legend
    image file using the PIL module. Colours can be specified using any format
    PIL supports (i.e., hex or list 3 or 4 values). The output image has an
    alpha channel.

    :param legend_info: dict using the class names as the key and value is the colour
                        used for the class.
    :param out_img_file: the output image file (Recommend output as PNG).
    :param n_cols: the number of columns the classes are split between (Default: 1).
    :param box_size: the size, in pixels, of the colour box for each class.
                     Default: (10, 20)
    :param title_str: An optional title for the legend. If None then no title,
                      default: None
    :param font_size: The size of the font to use for the legend (Default: 12)
    :param font: Optionally, pass a ttf file or font name for the font to be used.
                 Default: Arial
    :param font_clr: The font colour (Default: (0, 0, 0, 255) i.e., black)
    :param col_width: Override the calculated column width in pixels. (Default: None)
    :param img_height: Override the calculated image height in pixels. (Default: None)
    :param char_width: Define the number of pixels representing each character used
                       for calculating column widths. Try changing this before
                       overriding the column width. (Default: 6)
    :param bkgd_clr: the background colour for the legend
                     (Default: (255, 255, 255, 255) i.e., white). Note, this uses
                     an alpha channel so specifying (255, 255, 255, 0) will provide
                     a transparent background
    :param title_height: Extra height in pixels for the title (Default: 16)
    :param margin: The margin in pixels around the image each and between features
                   (Default: 2)

    """
    img_obj = create_legend_img(
        legend_info,
        n_cols,
        box_size,
        title_str,
        font_size,
        font,
        font_clr,
        col_width,
        img_height,
        char_width,
        bkgd_clr,
        title_height,
        margin,
    )

    img_obj.save(out_img_file)


def create_legend_img_mpl_ax(
    ax: plt.axis,
    legend_info: Dict,
    n_cols: int = 1,
    box_size: Tuple[int] = (10, 20),
    title_str: str = None,
    font_size: int = 12,
    font: str = None,
    font_clr: Tuple[int] = (0, 0, 0, 255),
    col_width: int = None,
    img_height: int = None,
    char_width: int = 6,
    bkgd_clr: Tuple[int] = (255, 255, 255, 255),
    title_height: int = 16,
    margin: int = 2,
    turn_off_axis_feats=True,
):
    """
    A function which uses the create_legend_img function to generate a legend
    using the PIL module and addeds it to a matplotlib axis for integration.
    Colours can be specified using any format
    PIL supports (i.e., hex or list 3 or 4 values). The output image has an
    alpha channel.

    :param legend_info: dict using the class names as the key and value is the colour
                        used for the class.
    :param out_img_file: the output image file (Recommend output as PNG).
    :param n_cols: the number of columns the classes are split between (Default: 1).
    :param box_size: the size, in pixels, of the colour box for each class.
                     Default: (10, 20)
    :param title_str: An optional title for the legend. If None then no title,
                      default: None
    :param font_size: The size of the font to use for the legend (Default: 12)
    :param font: Optionally, pass a ttf file or font name for the font to be used.
                 Default: Arial
    :param font_clr: The font colour (Default: (0, 0, 0, 255) i.e., black)
    :param col_width: Override the calculated column width in pixels. (Default: None)
    :param img_height: Override the calculated image height in pixels. (Default: None)
    :param char_width: Define the number of pixels representing each character used
                       for calculating column widths. Try changing this before
                       overriding the column width. (Default: 6)
    :param bkgd_clr: the background colour for the legend
                     (Default: (255, 255, 255, 255) i.e., white). Note, this uses
                     an alpha channel so specifying (255, 255, 255, 0) will provide
                     a transparent background
    :param title_height: Extra height in pixels for the title (Default: 16)
    :param margin: The margin in pixels around the image each and between features
                   (Default: 2)
    :param turn_off_axis_feats: an option which turns off the axis boundary lines
                                and other graph features so the legend is just shown
                                as an image (Default: True).

    """
    img_obj = create_legend_img(
        legend_info,
        n_cols,
        box_size,
        title_str,
        font_size,
        font,
        font_clr,
        col_width,
        img_height,
        char_width,
        bkgd_clr,
        title_height,
        margin,
    )

    ax.imshow(img_obj)
    if turn_off_axis_feats:
        ax.spines["top"].set_visible(False)
        ax.spines["right"].set_visible(False)
        ax.spines["bottom"].set_visible(False)
        ax.spines["left"].set_visible(False)
        ax.get_xaxis().set_ticks([])
        ax.get_yaxis().set_ticks([])


def create_legend_info_dict(
    input_img: str,
    cls_names_col: str,
    use_title_case=False,
    underscore_to_space=False,
    red_col: str = "Red",
    green_col: str = "Green",
    blue_col: str = "Blue",
    alpha_col: str = "Alpha",
    histogram_col: str = "Histogram",
) -> Dict[str, Tuple[int]]:
    """
    A function which creates the legend_info dict for the create_legend_img function
    from an input image file with RAT. It assumes that the RAT contains a column with
    the class names and columns with the class colours.

    :param input_img: The input image file path
    :param cls_names_col: The name of the class names column within the RAT
    :param use_title_case: Change the class name to title case (Default: False)
    :param underscore_to_space: Convert underscores in the class name to spaces
                                (Default: False)
    :param red_col: The name of the red colour column within the RAT (Default: Red)
    :param green_col: The name of the green colour column within the RAT
                      (Default: Green)
    :param blue_col: The name of the blue colour column within the RAT
                     (Default: Blue)
    :param alpha_col: The name of the alpha colour column within the RAT
                      (Default: Alpha)
    :param histogram_col: The name of the histogram column within the RAT
                          (Default: Histogram)
    :return: dictionary with the classification name as the key and the value a tuple of [Red, Green, Blue, Alpha]
             values.

    """
    import rsgislib.rastergis

    rat_cols = rsgislib.rastergis.get_rat_columns(input_img)

    cls_names = rsgislib.rastergis.get_column_data(input_img, cls_names_col)
    red_clrs = rsgislib.rastergis.get_column_data(input_img, red_col)
    green_clrs = rsgislib.rastergis.get_column_data(input_img, green_col)
    blue_clrs = rsgislib.rastergis.get_column_data(input_img, blue_col)
    alpha_clrs = rsgislib.rastergis.get_column_data(input_img, alpha_col)
    if histogram_col in rat_cols:
        histogram_vals = rsgislib.rastergis.get_column_data(input_img, histogram_col)
    else:
        histogram_vals = numpy.ones_like(red_clrs, dtype=numpy.uint8)

    out_cls_info = dict()

    for cls_name, red_val, green_val, blue_val, alpha_val, hist_val in zip(
        cls_names, red_clrs, green_clrs, blue_clrs, alpha_clrs, histogram_vals
    ):
        cls_name_str = str(cls_name.decode())
        if (cls_name_str != "") and (hist_val > 0):
            if underscore_to_space:
                cls_name_str = cls_name_str.replace("_", " ")

            if use_title_case:
                cls_name_str = cls_name_str.title()

            out_cls_info[cls_name_str] = (red_val, green_val, blue_val, alpha_val)

    return out_cls_info


def gen_colour_lst(cmap_name: str, n_clrs: int, reverse: bool = False) -> List[str]:
    """
    A function which gets a list of colours as hex strings from a matplotlib colour
    bar.

    For available colour bars see:
    https://matplotlib.org/stable/tutorials/colors/colormaps.html


    :param cmap_name: The name of a matplotlib colour bar
    :param n_clrs: The number of colours to be returned
    :param reverse: Option to reverse the order of the colours
    :return: List of hex colour presentations

    """
    c_map = plt.cm.get_cmap(cmap_name)
    vals_arr = numpy.linspace(0, 1, n_clrs)
    clr_lst = list()
    for c in vals_arr:
        rgba = c_map(c)
        clr = mClrs.rgb2hex(rgba)  # convert to hex
        clr_lst.append(str(clr))  # create a list of these colors

    if reverse == True:
        clr_lst.reverse()
    return clr_lst


def add_img_to_axis(ax: plt.axis, img_file_path: str, turn_off_axis_feats=True):
    """
    A function which renders an image file (e.g., PNG, JPG; anything PIL will read)
    to a matplotlib axis. This can be useful for adding a separate legend or image
    to a plot with multiple axes.

    :param ax: The matplotlib axis to which to add the image to.
    :param img_file_path: the file path to the input image.
    :param turn_off_axis_feats: boolean specifying that the axis features (i.e.,
                                ticks and border) should turned off (Default: True)

    """
    import PIL.Image

    pil_img_obj = PIL.Image.open(img_file_path)
    ax.imshow(pil_img_obj)
    if turn_off_axis_feats:
        ax.spines["top"].set_visible(False)
        ax.spines["right"].set_visible(False)
        ax.spines["bottom"].set_visible(False)
        ax.spines["left"].set_visible(False)
        ax.get_xaxis().set_ticks([])
        ax.get_yaxis().set_ticks([])
    pil_img_obj = None


def update_y_tick_lbls(ax, multi=100000, integerize=False):
    """
    A function to update the y axis labels of a plot using a multiplier to
    rescale the values within the labels. For example, if the multiplier was
    100000 and the axis tick was 500000 then 5.0 will be outputted as the tick
    label (or 5 if integerized).  You can then update the axis label with the
    multiplier used (e.g., ax.set_ylabel(r"Freq. ($\times 100,000$)") )

    :param ax: the matplotlib axis
    :param multi: the multiplier to be used.
    :param integerize: boolean to specify whether the values should be integerized

    """
    y_ticks = ax.get_yticks()
    y_tick_lbls = []
    for val in y_ticks:
        val_out = val / multi
        if integerize:
            val_out = int(val_out)
        y_tick_lbls.append(f"{val_out}")
    ax.set_yticks(y_ticks, labels=y_tick_lbls)


def hide_matplotlib_axis_border(ax):
    """
    For a matplotlib axis set the border and x,y axis' as invisible.

    :param ax: the matplotlib axis

    """
    ax.get_xaxis().set_visible(False)
    ax.get_yaxis().set_visible(False)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.spines["bottom"].set_visible(False)
    ax.spines["left"].set_visible(False)
