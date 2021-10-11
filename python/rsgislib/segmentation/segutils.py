#! /usr/bin/env python
############################################################################
#  segutils.py
#
#  Copyright 2013 RSGISLib.
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
# Purpose:  Provide a set of utilities which combine commands to create
#           useful extra functionality and make it more easily available
#           to be reused.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 16/11/2013
# Version: 1.1
#
# History:
# Version 1.0 - Created.
# Version 1.1 - Update to be included into RSGISLib python modules tree.
#
###########################################################################
# Import shutil
import shutil
# Import the rsgislib module
import rsgislib
# Import the image utilities module from rsgislib
import rsgislib.imageutils
# Import the image calc module from rsgislib
import rsgislib.imagecalc
# Import the image segmentation module from rsgislib
import rsgislib.segmentation
# Import the image rastergis module from rsgislib
import rsgislib.rastergis
# Import the python OS module
import os
# Import the collections module
import collections
#import the gdal module
import osgeo.gdal as gdal
# Import the python JSON module
import json


def run_shepherd_segmentation(input_img, out_clumps_img, out_mean_img=None, tmp_dir='.', gdalformat='KEA', calc_stats=True, no_stretch=False, no_delete=False, num_clusters=60, min_n_pxls=100, dist_thres=100, bands=None, sampling=100, km_max_iter=200, process_in_mem=False, save_process_stats=False, img_stretch_stats="", kmeans_centres="", img_stats_json_file=""):
    """
Utility function to call the segmentation algorithm of Shepherd et al. (2019).

Shepherd, J. D., Bunting, P., & Dymond, J. R. (2019). Operational Large-Scale Segmentation of Imagery Based on Iterative Elimination. Remote Sensing, 11(6), 658. http://doi.org/10.3390/rs11060658

Where:

:param input_img: is a string containing the name of the input file.
:param out_clumps_img: is a string containing the name of the output clump file.
:param out_mean_img: is the output mean image file (clumps attributed with pixel mean from input image) - pass 'None' to skip creating.
:param tmp_dir: is a file path for intermediate files (default is current directory).
:param gdalformat: is a string containing the GDAL format for the output file (default = KEA).
:param calc_stats: is a bool which specifies that image statistics and pyramids should be built for the output images (default = True)/
:param no_stretch: is a bool which specifies that the input image bands should not be stretched (default = False).
:param no_delete: is a bool which specifies that the temporary images created during processing should not be deleted once processing has been completed (default = False).
:param num_clusters: is an int which specifies the number of clusters within the KMeans clustering (default = 60).
:param min_n_pxls: is an int which specifies the minimum number pixels within a segments (default = 100).
:param dist_thres: specifies the distance threshold for joining the segments (default = 100, set to large number to turn off this option).
:param bands: is an array providing a subset of image bands to use (default is None to use all bands).
:param sampling: specify the subsampling of the image for the data used within the KMeans (default = 100; 1 == no subsampling).
:param km_max_iter: maximum iterations for KMeans.
:param process_in_mem: where functions allow it perform processing in memory rather than on disk.
:param save_process_stats: is a bool which specifies that the image stretch stats and the kMeans centre stats should be saved along with a header.
:param img_stretch_stats: is a string providing the file name and path for the image stretch stats (Output).
:param kmeans_centres: is a string providing the file name and path for the KMeans clusters centres (don't include file extension; .gmtxt will be added to the end) (Output).
:param img_stats_json_file: is a string providing the name and path of a JSON file storing the image spatial extent and img_stretch_stats and kmeans_centres file paths for use by other commands (Output).

Example::

    from rsgislib.segmentation import segutils
    
    input_img = 'jers1palsar_stack.kea'
    out_clumps_img = 'jers1palsar_stack_clumps_elim_final.kea'
    out_mean_img = 'jers1palsar_stack_clumps_elim_final_mean.kea'
    
    segutils.run_shepherd_segmentation(input_img, out_clumps_img, out_mean_img, min_n_pxls=100)


    """
    import rsgislib.tools.filetools

    if save_process_stats:
        if (img_stretch_stats=="") or (kmeans_centres=="") or (img_stats_json_file==""):
            raise rsgislib.RSGISPyException("if image stretch and kmeans centres are to be saved then all file names (img_stretch_stats, kmeans_centres, img_stats_json_file) need to be provided.")

    basefile = os.path.basename(input_img)
    basename = os.path.splitext(basefile)[0]
    
    out_file_ext = rsgislib.imageutils.get_file_img_extension(gdalformat)
    
    createdDIR = False
    if not os.path.isdir(tmp_dir):
        os.makedirs(tmp_dir)
        createdDIR = True

    # Get data type of input image
    input_datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
        
    # Select Image Bands if required
    inputImgBands = input_img
    selectBands = False
    if not bands == None:
        print("Subsetting the image bands")
        selectBands = True
        inputImgBands = os.path.join(tmp_dir, "{}_bselect.{}".format(basename, out_file_ext))
        rsgislib.imageutils.select_img_bands(input_img, inputImgBands, gdalformat, input_datatype, bands)
    
    # Stretch input data if required.
    segmentFile = inputImgBands
    if not no_stretch:
        segmentFile = os.path.join(tmp_dir, "{}_stchd.{}".format(basename, out_file_ext))
        strchFile = os.path.join(tmp_dir, "{}_stchdonly.{}".format(basename, out_file_ext))
        strchFileOffset = os.path.join(tmp_dir, "{}_stchdonly_off.{}".format(basename, out_file_ext))
        strchMaskFile = os.path.join(tmp_dir, "{}_stchdmaskonly.{}".format(basename, out_file_ext))
        
        print("Stretch Input Image")
        rsgislib.imageutils.stretch_img(inputImgBands, strchFile, save_process_stats, img_stretch_stats, True, False, gdalformat, rsgislib.TYPE_8INT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)
        
        print("Add 1 to stretched file to ensure there are no all zeros (i.e., no data) regions created.")
        rsgislib.imagecalc.image_math(strchFile, strchFileOffset, "b1+1", gdalformat, rsgislib.TYPE_8INT)
        
        print("Create Input Image Mask.")
        bandMathBands = [rsgislib.imagecalc.BandDefn(band_name="b1", input_img=inputImgBands, img_band=1)]
        rsgislib.imagecalc.band_math(strchMaskFile, "b1==0?1:0", gdalformat, rsgislib.TYPE_8INT, bandMathBands)
        
        print("Mask stretched Image.")
        rsgislib.imageutils.mask_img(strchFileOffset, strchMaskFile, segmentFile, gdalformat, rsgislib.TYPE_8INT, 0, 1)
        
        if not no_delete:
            # Deleting extra files
            rsgislib.tools.filetools.delete_file_with_basename(strchFile)
            rsgislib.tools.filetools.delete_file_with_basename(strchFileOffset)
            rsgislib.tools.filetools.delete_file_with_basename(strchMaskFile)
            
    # Perform KMEANS
    print("Performing KMeans.")
    outMatrixFile = os.path.join(tmp_dir, "{}_kmeansclusters".format(basename))
    if save_process_stats:
        outMatrixFile = kmeans_centres
    rsgislib.imagecalc.kmeans_clustering(segmentFile, outMatrixFile, num_clusters, km_max_iter, sampling, True, 0.0025, rsgislib.imagecalc.INITCLUSTER_DIAGONAL_FULL_ATTACH)
    
    # Apply KMEANS
    print("Apply KMeans to image.")
    kMeansFileZones = os.path.join(tmp_dir, "{}_kmeans.{}".format(basename, out_file_ext))
    rsgislib.segmentation.label_pixels_from_cluster_centres(segmentFile, kMeansFileZones, outMatrixFile+str(".gmtxt"), True, gdalformat)
    
    # Eliminate Single Pixels
    print("Eliminate Single Pixels.")
    kMeansFileZonesNoSgls = os.path.join(tmp_dir, "{}_kmeans_nosgl.{}".format(basename, out_file_ext))
    kMeansFileZonesNoSglsTmp = os.path.join(tmp_dir, "{}_kmeans_nosgl_tmp.{}".format(basename, out_file_ext))
    rsgislib.segmentation.eliminate_single_pixels(segmentFile, kMeansFileZones, kMeansFileZonesNoSgls, kMeansFileZonesNoSglsTmp, gdalformat, process_in_mem, True)
    
    # Clump
    print("Perform clump.")
    initClumpsFile = os.path.join(tmp_dir, "{}_clumps.{}".format(basename, out_file_ext))
    rsgislib.segmentation.clump(kMeansFileZonesNoSgls, initClumpsFile, gdalformat, process_in_mem, 0)
    
    # Elimininate small clumps
    print("Eliminate small pixels.")
    elimClumpsFile = os.path.join(tmp_dir, "{}_clumps_elim.{}".format(basename, out_file_ext))
    rsgislib.segmentation.rm_small_clumps_stepwise(segmentFile, initClumpsFile, elimClumpsFile, gdalformat, False, "", False, process_in_mem, min_n_pxls, dist_thres)
    
    # Relabel clumps
    print("Relabel clumps.")
    rsgislib.segmentation.relabel_clumps(elimClumpsFile, out_clumps_img, gdalformat, process_in_mem)
    
    # Populate with stats if required.
    if calc_stats:
        print("Calculate image statistics and build pyramids.")
        rsgislib.rastergis.pop_rat_img_stats(out_clumps_img, True, True)
    
    # Create mean image if required.
    if not (out_mean_img == None):
        rsgislib.segmentation.mean_image(input_img, out_clumps_img, out_mean_img, gdalformat, input_datatype)
        if calc_stats:
            rsgislib.imageutils.pop_img_stats(out_mean_img, True, 0, True)
    
    
    if save_process_stats:
        gdalDS = gdal.Open(input_img, gdal.GA_ReadOnly)
        geotransform = gdalDS.GetGeoTransform()
        if not geotransform is None:
            xTL = geotransform[0]
            yTL = geotransform[3]
            
            xRes = geotransform[1]
            yRes = geotransform[5]
            
            width = gdalDS.RasterXSize * xRes
            if yRes < 0:
                yRes = yRes * (-1)
            height = gdalDS.RasterYSize * yRes
            xBR = xTL + width
            yBR = yTL - height
            
            xCen = xTL + (width/2)
            yCen = yBR + (height/2)
        
            sceneData = dict()
            sceneData['KCENTRES'] = kmeans_centres+str(".gmtxt")
            sceneData['STRETCHSTATS'] = img_stretch_stats
            sceneData['CENTRE_PT'] = {'X':xCen, 'Y':yCen}
            sceneData['BBOX'] = {'XMIN':xTL, 'YMIN':yBR, 'XMAX':xBR, 'YMAX':yTL}
            
            with open(img_stats_json_file, 'w') as outfile:
                json.dump(sceneData, outfile, sort_keys=True, indent=4, separators=(',', ': '), ensure_ascii=False)

        gdalDS = None
     
    if not no_delete:
        # Deleting extra files
        if not save_process_stats:
            rsgislib.tools.filetools.delete_file_with_basename(outMatrixFile + str(".gmtxt"))
        rsgislib.tools.filetools.delete_file_with_basename(kMeansFileZones)
        rsgislib.tools.filetools.delete_file_with_basename(kMeansFileZonesNoSgls)
        rsgislib.tools.filetools.delete_file_with_basename(kMeansFileZonesNoSglsTmp)
        rsgislib.tools.filetools.delete_file_with_basename(initClumpsFile)
        rsgislib.tools.filetools.delete_file_with_basename(elimClumpsFile)
        if selectBands:
            rsgislib.tools.filetools.delete_file_with_basename(inputImgBands)
        if not no_stretch:
            rsgislib.tools.filetools.delete_file_with_basename(segmentFile)
        if createdDIR:
            shutil.rmtree(tmp_dir)
            

def run_shepherd_segmentation_pre_calcd_stats(input_img, out_clumps_img, kmeans_centres, img_stretch_stats, out_mean_img=None, tmp_dir='.', gdalformat='KEA', calc_stats=True, no_stretch=False, no_delete=False, min_n_pxls=100, dist_thres=100, bands=None, process_in_mem=False):
    """
Utility function to call the segmentation algorithm of Shepherd et al. (2019) using pre-calculated stretch stats and KMeans cluster centres.

Shepherd, J. D., Bunting, P., & Dymond, J. R. (2019). Operational Large-Scale Segmentation of Imagery Based on Iterative Elimination. Remote Sensing, 11(6), 658. http://doi.org/10.3390/rs11060658

Where:

:param input_img: is a string containing the name of the input file.
:param out_clumps_img: is a string containing the name of the output clump file.
:param kmeans_centres: is a string providing the file name and path for the KMeans clusters centres (Input)
:param img_stretch_stats: is a string providing the file name and path for the image stretch stats (Input - not required if no_stretch=True)
:param out_mean_img: is the output mean image file (clumps attributed with pixel mean from input image) - pass 'None' to skip creating.
:param tmp_dir: is a file path for intermediate files (default is current directory).
:param gdalformat: is a string containing the GDAL format for the output file (default = KEA).
:param calc_stats: is a bool which specifies that image statistics and pyramids should be built for the output images (default = True)/
:param no_stretch: is a bool which specifies that the input image bands should not be stretched (default = False).
:param no_delete: is a bool which specifies that the temporary images created during processing should not be deleted once processing has been completed (default = False).
:param min_n_pxls: is an int which specifies the minimum number pixels within a segments (default = 100).
:param dist_thres: specifies the distance threshold for joining the segments (default = 100, set to large number to turn off this option).
:param bands: is an array providing a subset of image bands to use (default is None to use all bands).
:param sampling: specify the subsampling of the image for the data used within the KMeans (default = 100; 1 == no subsampling).
:param process_in_mem: where functions allow it perform processing in memory rather than on disk.

Example::

    from rsgislib.segmentation import segutils
    
    input_img = 'jers1palsar_stack.kea'
    out_clumps_img = 'jers1palsar_stack_clumps_elim_final.kea'
    out_mean_img = 'jers1palsar_stack_clumps_elim_final_mean.kea'
    kmeans_centres = 'jers1palsar_stack_kcentres.gmtxt'
    img_stretch_stats = 'jers1palsar_stack_stchstats.txt'
    
    segutils.run_shepherd_segmentation_pre_calcd_stats(input_img, out_clumps_img, kmeans_centres, img_stretch_stats, out_mean_img, min_n_pxls=100)

    """
    import rsgislib.tools.filetools
    if not no_stretch:
        if (img_stretch_stats=="") or (img_stretch_stats==None):
            raise rsgislib.RSGISPyException("A stretch stats file must be provided")

    basefile = os.path.basename(input_img)
    basename = os.path.splitext(basefile)[0]
    
    out_file_ext = rsgislib.imageutils.get_file_img_extension(gdalformat)
    
    createdDIR = False
    if not os.path.isdir(tmp_dir):
        os.makedirs(tmp_dir)
        createdDIR = True

    # Get data type of input image
    gdalDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    input_datatype = rsgislib.get_rsgislib_datatype(gdal.GetDataTypeName(gdalDS.GetRasterBand(1).DataType))
    gdalDS = None
        
    # Select Image Bands if required
    inputImgBands = input_img
    selectBands = False
    if not bands == None:
        print("Subsetting the image bands")
        selectBands = True
        inputImgBands = os.path.join(tmp_dir, "{}_bselect.{}".format(basename, out_file_ext))
        rsgislib.imageutils.select_img_bands(input_img, inputImgBands, gdalformat, input_datatype, bands)
    
    # Stretch input data if required.
    segmentFile = inputImgBands
    if not no_stretch:
        segmentFile = os.path.join(tmp_dir, "{}_stchd.{}".format(basename, out_file_ext))
        strchFile = os.path.join(tmp_dir, "{}_stchdonly.{}".format(basename, out_file_ext))
        strchFileOffset = os.path.join(tmp_dir, "{}_stchdonly_off.{}".format(basename, out_file_ext))
        strchMaskFile = os.path.join(tmp_dir, "{}_stchdmaskonly.{}".format(basename, out_file_ext))
        
        print("Stretch Input Image")
        rsgislib.imageutils.stretch_img_with_stats(inputImgBands, strchFile, img_stretch_stats, gdalformat, rsgislib.TYPE_8INT, rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
        
        print("Add 1 to stretched file to ensure there are no all zeros (i.e., no data) regions created.")
        rsgislib.imagecalc.image_math(strchFile, strchFileOffset, "b1+1", gdalformat, rsgislib.TYPE_8INT)
        
        print("Create Input Image Mask.")
        bandMathBands = [rsgislib.imagecalc.BandDefn(band_name="b1", input_img=inputImgBands, img_band=1)]
        rsgislib.imagecalc.band_math(strchMaskFile, "b1==0?1:0", gdalformat, rsgislib.TYPE_8INT, bandMathBands)
        
        print("Mask stretched Image.")
        rsgislib.imageutils.mask_img(strchFileOffset, strchMaskFile, segmentFile, gdalformat, rsgislib.TYPE_8INT, 0, 1)
        
        if not no_delete:
            # Deleting extra files
            rsgislib.tools.filetools.delete_file_with_basename(strchFile)
            rsgislib.tools.filetools.delete_file_with_basename(strchFileOffset)
            rsgislib.tools.filetools.delete_file_with_basename(strchMaskFile)
    
    # Apply KMEANS
    print("Apply KMeans to image.")
    kMeansFileZones = os.path.join(tmp_dir, "{}_kmeans.{}".format(basename, out_file_ext))
    rsgislib.segmentation.label_pixels_from_cluster_centres(segmentFile, kMeansFileZones, kmeans_centres, True, gdalformat)
    
    # Elimininate Single Pixels
    print("Eliminate Single Pixels.")
    kMeansFileZonesNoSgls = os.path.join(tmp_dir, "{}_kmeans_nosgl.{}".format(basename, out_file_ext))
    kMeansFileZonesNoSglsTmp = os.path.join(tmp_dir, "{}_kmeans_nosgl_tmp.{}".format(basename, out_file_ext))
    rsgislib.segmentation.eliminate_single_pixels(segmentFile, kMeansFileZones, kMeansFileZonesNoSgls, kMeansFileZonesNoSglsTmp, gdalformat, process_in_mem, True)
    
    # Clump
    print("Perform clump.")
    initClumpsFile = os.path.join(tmp_dir, "{}_clumps.{}".format(basename, out_file_ext))
    rsgislib.segmentation.clump(kMeansFileZonesNoSgls, initClumpsFile, gdalformat, process_in_mem, 0)
    
    # Elimininate small clumps
    print("Eliminate small pixels.")
    elimClumpsFile = os.path.join(tmp_dir, "{}_clumps_elim.{}".format(basename, out_file_ext))
    rsgislib.segmentation.rm_small_clumps_stepwise(segmentFile, initClumpsFile, elimClumpsFile, gdalformat, False, "", False, process_in_mem, min_n_pxls, dist_thres)
    
    # Relabel clumps
    print("Relabel clumps.")
    rsgislib.segmentation.relabel_clumps(elimClumpsFile, out_clumps_img, gdalformat, process_in_mem)
    
    # Populate with stats if required.
    if calc_stats:
        print("Calculate image statistics and build pyramids.")
        rsgislib.rastergis.pop_rat_img_stats(out_clumps_img, True, True)
    
    # Create mean image if required.
    if not (out_mean_img == None):
        rsgislib.segmentation.mean_image(input_img, out_clumps_img, out_mean_img, gdalformat, input_datatype)
        if calc_stats:
            rsgislib.imageutils.pop_img_stats(out_mean_img, True, 0, True)
     
    if not no_delete:
        # Deleting extra files
        rsgislib.tools.filetools.delete_file_with_basename(kMeansFileZones)
        rsgislib.tools.filetools.delete_file_with_basename(kMeansFileZonesNoSgls)
        rsgislib.tools.filetools.delete_file_with_basename(kMeansFileZonesNoSglsTmp)
        rsgislib.tools.filetools.delete_file_with_basename(initClumpsFile)
        rsgislib.tools.filetools.delete_file_with_basename(elimClumpsFile)
        if selectBands:
            rsgislib.tools.filetools.delete_file_with_basename(inputImgBands)
        if not no_stretch:
            rsgislib.tools.filetools.delete_file_with_basename(segmentFile)
        if createdDIR:
            shutil.rmtree(tmp_dir)



def run_shepherd_segmentation_test_num_clumps(input_img, out_clumps_img_base, out_stats_file, out_mean_img_base=None, tmp_dir='.', gdalformat='KEA', calc_stats=True, no_stretch=False, no_delete=False, numClustersStart=10, numClustersStep=10, numOfClustersSteps=10, min_n_pxls=10, dist_thres=1000000, bands=None, sampling=100, km_max_iter=200, process_in_mem=False, min_norm_v=None, max_norm_v=None, min_norm_mi=None, max_norm_mi=None):
    """
Utility function to call the segmentation algorithm of Shepherd et al. (2019) and to test are range of 'k' within the kMeans.

Shepherd, J. D., Bunting, P., & Dymond, J. R. (2019). Operational Large-Scale Segmentation of Imagery Based on Iterative Elimination. Remote Sensing, 11(6), 658. http://doi.org/10.3390/rs11060658

Where:

:param input_img: is a string containing the name of the input file
:param out_clumps_img: is a string containing the name of the output clump file
:param out_stats_file: is a string containing the name of the output CSV file with the image segmentation stats
:param out_mean_img: is the output mean image file (clumps attributed with pixel mean from input image) - pass 'None' to skip creating.
:param tmp_dir: is a file path for intermediate files (default is current directory).
:param gdalformat: is a string containing the GDAL format for the output file (default is KEA)
:param calc_stats: is a bool which specifies that image statistics and pyramids should be built for the output images.
:param no_stretch: is a bool which specifies that the input image bands should not be stretched.
:param no_delete: is a book which specifies that the temporary images created during processing should not be deleted once processing has been completed.
:param numClustersStart: is an int which specifies the number of clusters within the KMeans clustering to start the process
:param numClustersStep: is an int which specifies the number of clusters within the KMeans clustering added with each step
:param numOfClustersSteps: is an int which specifies the number steps (i.e., tests) which are performed.
:param min_n_pxls: is an int which specifies the minimum number pixels within a segments.
:param dist_thres: specifies the distance threshold for joining the segments (default is a very large value which turns off this option.).
:param bands: is an array providing a subset of image bands to use (default is None to use all bands)
:param sampling: specify the subsampling of the image for the data used within the KMeans (1 == no subsampling; default is 100)
:param km_max_iter: maximum iterations for KMeans.
:param process_in_mem: where functions allow it perform processing in memory rather than on disk.
:param min_norm_v: is a floating point =None
:param max_norm_v: None
:param min_norm_mi: None
:param max_norm_mi: None

Example::

    from rsgislib.segmentation import segutils


    input_img = './WV2_525N040W_20110727_TOARefl_b762_stch.kea'
    out_clumps_img_base = './OptimalTests/WV2_525N040W_20110727_Clumps'
    out_mean_img_base = './OptimalTests/WV2_525N040W_20110727_ClumpsMean'
    tmp_dir='./OptimalTests/tmp/'
    out_stats_file = './OptimalTests/StatsClumps.csv'

    # Will test clump values from 10 to 200 with intervals of 10.
    segutils.run_shepherd_segmentation_test_num_clumps(input_img, out_clumps_img_base, out_stats_file, out_mean_img_base=out_mean_img_base, tmp_dir=tmp_dir, no_stretch=True, numClustersStart=10, numClustersStep=10, numOfClustersSteps=20, min_n_pxls=50, min_norm_v=None, max_norm_v=None, min_norm_mi=None, max_norm_mi=None)


    """
    colsPrefix = 'gs'
    calcNeighbours = True
    calcNormVals = False
    if min_norm_v==None or  max_norm_v==None or min_norm_mi==None or max_norm_mi==None:
        min_norm_v = 0.0
        max_norm_v = 1.0
        min_norm_mi = 0.0
        max_norm_mi = 1.0
        calcNormVals = True
    
    
    
    outputStats = list()
    
    num_clusters = numClustersStart
    for i in range(numOfClustersSteps):
        num_clusters = numClustersStart + (i * numClustersStep)
        print("Processing ", num_clusters)
        out_clumps_img = out_clumps_img_base + "_c" + str(num_clusters) + ".kea"
        out_mean_img = out_mean_img_base + "_c" + str(num_clusters) + ".kea"
        
        run_shepherd_segmentation(input_img, out_clumps_img, out_mean_img=out_mean_img, tmp_dir=tmp_dir, gdalformat=gdalformat, calc_stats=calc_stats, no_stretch=no_stretch, no_delete=no_delete, num_clusters=num_clusters, min_n_pxls=min_n_pxls, dist_thres=dist_thres, bands=bands, sampling=sampling, km_max_iter=km_max_iter, process_in_mem=process_in_mem)
        
        segScores = rsgislib.rastergis.calcGlobalSegmentationScore(out_clumps_img, input_img, colsPrefix, calcNeighbours, min_norm_v, max_norm_v, min_norm_mi, max_norm_mi)
        
        tup = (num_clusters, segScores)
        outputStats.append(tup)
        
    
    numImgBands = int(len(outputStats[0][1][1])/4)
    
    
    if calcNormVals:
        minVar = 0.0
        maxVar = 0.0
        scaleVar = 0.0
        minMI = 0.0
        maxMI = 0.0
        scaleMI = 0.0
        first = True
        
        for stat in outputStats:        
            for i in range(numImgBands):
                idxVar = i*4
                idxMI = (i*4)+1
                if first:
                    minVar = stat[1][1][idxVar] # Var Min
                    maxVar = stat[1][1][idxVar] # Var Max
                    minMI = stat[1][1][idxMI] # MI Min
                    maxMI = stat[1][1][idxMI] # MI Max
                    first = False
                else:
                    if stat[1][1][idxVar] < minVar:
                        minVar = stat[1][1][idxVar] # Set Var Min
                    elif stat[1][1][idxVar] > maxVar:
                        maxVar = stat[1][1][idxVar] # Set Var Max
                        
                    if stat[1][1][idxMI] < minMI:
                        minMI = stat[1][1][idxMI] # Set MI Min
                    elif stat[1][1][idxMI] > maxMI:
                        maxMI = stat[1][1][idxMI] # Set MI Max
        
        print("Var Min: ", minVar)
        print("Var Max: ", maxVar)
        print("MI Min: ", minMI)
        print("MI Max: ", maxMI)
    
        if (maxVar - minVar) > 0:
            scaleVar = 1/(maxVar - minVar)
        else:
            scaleVar = 1
        
        if (maxMI - minMI) > 0:
            scaleMI = 1/(maxMI - minMI)
        else:
            scaleMI = 1
    
        print("Var Scale: ", scaleVar)
        print("MI Scale: ", scaleMI)
    
        for stat in outputStats:
            gScore = 0.0
            for i in range(numImgBands):
                idxVar = i*4
                idxMI = (i*4) + 1
                idxVarNorm = (i*4) + 2
                idxMINorm = (i*4) + 3
                
                stat[1][1][idxVarNorm] = (stat[1][1][idxVar] - minVar) * scaleVar
                stat[1][1][idxMINorm] = (stat[1][1][idxMI] - minMI) * scaleMI
            
                gScore = gScore + stat[1][1][idxVarNorm] + stat[1][1][idxMINorm]
            stat[1][0] = gScore
    
    fileStats = open(out_stats_file, "w")
    
    colNames = "Clusters, Overall Score"
    for i in range(numImgBands):
        colNames = colNames + ", B" + str(i+1) + "_Variance"
        colNames = colNames + ", B" + str(i+1) + "_MI"
        colNames = colNames + ", B" + str(i+1) + "_VarianceNorm"
        colNames = colNames + ", B" + str(i+1) + "_MINorm"
    colNames = colNames + "\n"
    
    fileStats.write(colNames)
    
    print("Clusters, Overall Score")
    for stat in outputStats:
        line = str(stat[0]) + ", " + "{:.9f}".format(stat[1][0])
        print(line)
        for val in stat[1][1]:
            line = line + ", " + "{:.9f}".format(val)
        line = line + "\n"
        fileStats.write(line)  
        
    fileStats.close()
    print("Complete.\n")

def run_shepherd_segmentation_test_min_obj_size(input_img, out_clumps_img_base, out_stats_file, out_mean_img_base=None, tmp_dir='.', gdalformat='KEA', calc_stats=True, no_stretch=False, no_delete=False, num_clusters=100, min_n_pxls_start=10, min_n_pxls_step=5, num_of_min_n_pxls_steps=20, dist_thres=1000000, bands=None, sampling=100, km_max_iter=200, min_norm_v=None, max_norm_v=None, min_norm_mi=None, max_norm_mi=None):
    """
Utility function to call the segmentation algorithm of Shepherd et al. (2019) and to test are range of 'k' within the kMeans.

Where:

:param input_img: is a string containing the name of the input file
:param out_clumps_img: is a string containing the name of the output clump file
:param out_stats_file: is a string containing the name of the output CSV file with the image segmentation stats
:param out_mean_img: is the output mean image file (clumps attributed with pixel mean from input image) - pass 'None' to skip creating.
:param tmp_dir: is a file path for intermediate files (default is current directory).
:param gdalformat: is a string containing the GDAL format for the output file (default is KEA)
:param calc_stats: is a bool which specifies that image statistics and pyramids should be built for the output images.
:param no_stretch: is a bool which specifies that the input image bands should not be stretched.
:param no_delete: is a book which specifies that the temporary images created during processing should not be deleted once processing has been completed.
:param num_clusters: is an int which specifies the number of clusters within the KMeans clustering process
:param min_n_pxls_start: is an int which specifies the minimum number pixels within a segments at the start of processing.
:param min_n_pxls_step: is an int which specifies the minimum number pixels within a segments increment each step.
:param num_of_min_n_pxls_steps: is an int which specifies the number steps (i.e., tests) which are performed.
:param dist_thres: specifies the distance threshold for joining the segments (default is a very large value which turns off this option.).
:param bands: is an array providing a subset of image bands to use (default is None to use all bands)
:param sampling: specify the subsampling of the image for the data used within the KMeans (1 == no subsampling; default is 100)
:param km_max_iter: maximum iterations for KMeans.
:param min_norm_v: is a floating point =None
:param max_norm_v: None
:param min_norm_mi: None
:param max_norm_mi: None

Example::

    from rsgislib.segmentation import segutils

    input_img = './WV2_525N040W_20110727_TOARefl_b762_stch.kea'
    out_clumps_img_base = './OptimalTests/WV2_525N040W_20110727_MinPxl'
    out_mean_img_base = './OptimalTests/WV2_525N040W_20110727_MinPxlMean'
    tmp_dir='./OptimalTests/tmp/'
    out_stats_file = './OptimalTests/StatsMinPxl.csv'

    # Will test minimum number of pixels within an object from 10 to 100 with intervals of 5.
    segutils.run_shepherd_segmentation_test_min_obj_size(input_img, out_clumps_img_base, out_stats_file, out_mean_img_base=out_mean_img_base, tmp_dir=tmp_dir, no_stretch=True, num_clusters=100, min_n_pxls_start=5, min_n_pxls_step=5, num_of_min_n_pxls_steps=20, min_norm_v=None, max_norm_v=None, min_norm_mi=None, max_norm_mi=None)

    """
    colsPrefix = 'gs'
    calcNeighbours = True
    calcNormVals = False
    if min_norm_v==None or  max_norm_v==None or min_norm_mi==None or max_norm_mi==None:
        min_norm_v = 0.0
        max_norm_v = 1.0
        min_norm_mi = 0.0
        max_norm_mi = 1.0
        calcNormVals = True
    
    
    
    outputStats = list()
    
    min_n_pxls = min_n_pxls_start
    for i in range(num_of_min_n_pxls_steps):
        min_n_pxls = min_n_pxls_start + (i * min_n_pxls_step)
        print("Processing ", min_n_pxls)
        out_clumps_img = out_clumps_img_base + "_mp" + str(min_n_pxls) + ".kea"
        out_mean_img = out_mean_img_base + "_mp" + str(min_n_pxls) + ".kea"
        
        run_shepherd_segmentation(input_img, out_clumps_img, out_mean_img=out_mean_img, tmp_dir=tmp_dir, gdalformat=gdalformat, calc_stats=calc_stats, no_stretch=no_stretch, no_delete=no_delete, num_clusters=num_clusters, min_n_pxls=min_n_pxls, dist_thres=dist_thres, bands=bands, sampling=sampling, km_max_iter=km_max_iter)
        
        segScores = rsgislib.rastergis.calcGlobalSegmentationScore(out_clumps_img, input_img, colsPrefix, calcNeighbours, min_norm_v, max_norm_v, min_norm_mi, max_norm_mi)
        
        tup = (min_n_pxls, segScores)
        outputStats.append(tup)
        
    
    numImgBands = int(len(outputStats[0][1][1])/4)
    
    
    if calcNormVals:
        minVar = 0.0
        maxVar = 0.0
        scaleVar = 0.0
        minMI = 0.0
        maxMI = 0.0
        scaleMI = 0.0
        first = True
        
        for stat in outputStats:        
            for i in range(numImgBands):
                idxVar = i*4
                idxMI = (i*4)+1
                if first:
                    minVar = stat[1][1][idxVar] # Var Min
                    maxVar = stat[1][1][idxVar] # Var Max
                    minMI = stat[1][1][idxMI] # MI Min
                    maxMI = stat[1][1][idxMI] # MI Max
                    first = False
                else:
                    if stat[1][1][idxVar] < minVar:
                        minVar = stat[1][1][idxVar] # Set Var Min
                    elif stat[1][1][idxVar] > maxVar:
                        maxVar = stat[1][1][idxVar] # Set Var Max
                        
                    if stat[1][1][idxMI] < minMI:
                        minMI = stat[1][1][idxMI] # Set MI Min
                    elif stat[1][1][idxMI] > maxMI:
                        maxMI = stat[1][1][idxMI] # Set MI Max
        
        print("Var Min: ", minVar)
        print("Var Max: ", maxVar)
        print("MI Min: ", minMI)
        print("MI Max: ", maxMI)
    
        if (maxVar - minVar) > 0:
            scaleVar = 1/(maxVar - minVar)
        else:
            scaleVar = 1
        
        if (maxMI - minMI) > 0:
            scaleMI = 1/(maxMI - minMI)
        else:
            scaleMI = 1
    
        print("Var Scale: ", scaleVar)
        print("MI Scale: ", scaleMI)
    
        for stat in outputStats:
            gScore = 0.0
            for i in range(numImgBands):
                idxVar = i*4
                idxMI = (i*4) + 1
                idxVarNorm = (i*4) + 2
                idxMINorm = (i*4) + 3
                
                stat[1][1][idxVarNorm] = (stat[1][1][idxVar] - minVar) * scaleVar
                stat[1][1][idxMINorm] = (stat[1][1][idxMI] - minMI) * scaleMI
            
                gScore = gScore + stat[1][1][idxVarNorm] + stat[1][1][idxMINorm]
            stat[1][0] = gScore
    
    fileStats = open(out_stats_file, "w")
    
    colNames = "MinNumPxls, Overall Score"
    for i in range(numImgBands):
        colNames = colNames + ", B" + str(i+1) + "_Variance"
        colNames = colNames + ", B" + str(i+1) + "_MI"
        colNames = colNames + ", B" + str(i+1) + "_VarianceNorm"
        colNames = colNames + ", B" + str(i+1) + "_MINorm"
    colNames = colNames + "\n"
    
    fileStats.write(colNames)
    
    print("Clusters, Overall Score")
    for stat in outputStats:
        line = str(stat[0]) + ", " + "{:.9f}".format(stat[1][0])
        print(line)
        for val in stat[1][1]:
            line = line + ", " + "{:.9f}".format(val)
        line = line + "\n"
        fileStats.write(line)  
        
    fileStats.close()
    print("Complete.\n")

