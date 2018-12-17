"""
The zonal stats module provides functions to perform pixel-in-polygon or point-in-pixel analysis. 
"""

# import the C++ extension into this level
from ._zonalstats import *

import osgeo.gdal as gdal
import osgeo.ogr as ogr
import osgeo.osr as osr
import numpy
import math


METHOD_POLYCONTAINSPIXEL = 0           # Polygon completely contains pixel
METHOD_POLYCONTAINSPIXELCENTER = 1     # Pixel center is within the polygon
METHOD_POLYOVERLAPSPIXEL = 2           # Polygon overlaps the pixel
METHOD_POLYOVERLAPSORCONTAINSPIXEL = 3 # Polygon overlaps or contains the pixel
METHOD_PIXELCONTAINSPOLY = 4           # Pixel contains the polygon
METHOD_PIXELCONTAINSPOLYCENTER = 5     # Polygon center is within pixel
METHOD_ADAPTIVE = 6                    # The method is chosen based on relative areas of pixel and polygon.
METHOD_ENVELOPE = 7                    # All pixels in polygon envelope chosen
METHOD_PIXELAREAINPOLY = 8             # Percent of pixel area that is within the polygon
METHOD_POLYAREAINPIXEL = 9             # Percent of polygon area that is within pixel

class ZonalAttributes:
    """ Object, specifying which stats should be calculated and minimum / maximum thresholds. 
This is passed to the pixelStats2SHP and pixelStats2TXT functions. """
    def __init__(self, minThreshold=None, maxThreshold=None, calcCount=False, calcMin=False, calcMax=False, calcMean=False, calcStdDev=False, calcMode=False, calcSum=False):
        self.minThreshold = minThreshold
        self.maxThreshold = maxThreshold
        self.calcCount = calcCount
        self.calcMin = calcMin
        self.calcMax = calcMax
        self.calcMean = calcMean
        self.calcStdDev = calcStdDev 
        self.calcMode = calcMode
        self.calcSum = calcSum
        

class ZonalBandAttributes:
    """ Object, specifying which band, the band name and stats should be calculated and minimum / maximum thresholds. 
This is passed to the polyPixelStatsVecLyr function. """
    def __init__(self, band=0, basename="band", minThres=None, maxThres=None, calcCount=False, calcMin=False, calcMax=False, calcMean=False, calcStdDev=False, calcMode=False, calcMedian=False, calcSum=False):
        self.band = band
        self.basename = basename
        self.minThres = minThres
        self.maxThres = maxThres
        self.calcCount = calcCount
        self.calcMin = calcMin
        self.calcMax = calcMax
        self.calcMean = calcMean
        self.calcStdDev = calcStdDev 
        self.calcMode = calcMode
        self.calcMedian = calcMedian
        self.calcSum = calcSum

def calcZonalBandsStats(vecfile, veclyrname, valsimg, imgbandidx, minthres, maxthres, minfield=None, maxfield=None, meanfield=None, stddevfield=None, sumfield=None, countfield=None, modefield=None, medianfield=None):
    """
A function which calculates zonal statistics for a particular image band.

** USE THIS FUNCTION AS FASTEST **

* vecfile - input vector file
* veclyrname - input vector layer within the input file which specifies the features and where the output stats will be written.
* valsimg - the values image
* imgbandidx - the index (starting at 1) of the image band for which the stats will be calculated. If defined the no data value of the band will be ignored.
* minthres - a lower threshold for values which will be included in the stats calculation.
* maxthres - a upper threshold for values which will be included in the stats calculation.
* minfield - the name of the field for the min value (None or not specified to be ignored).
* maxfield - the name of the field for the max value (None or not specified to be ignored).
* meanfield - the name of the field for the mean value (None or not specified to be ignored).
* stddevfield - the name of the field for the standard deviation value (None or not specified to be ignored).
* sumfield - the name of the field for the sum value (None or not specified to be ignored).
* countfield - the name of the field for the count (of number of pixels) value (None or not specified to be ignored).
* modefield - the name of the field for the mode value (None or not specified to be ignored).
* medianfield - the name of the field for the median value (None or not specified to be ignored).
"""
    if medianfield is not None:
        import scipy.stats.mstats
    gdal.UseExceptions()
    
    if (minfield is None) and (maxfield is None) and (meanfield is None) and (stddevfield is None) and (sumfield is None) and (countfield is None) and (modefield is None) and (medianfield is None):
        raise Exception("At least one field needs to be specified for there is to an output.")

    imgDS = gdal.OpenEx(valsimg, gdal.GA_ReadOnly)
    if imgDS is None:
        raise Exception("Could not open '{}'".format(valsimg))
    imgband = imgDS.GetRasterBand(imgbandidx)
    if imgband is None:
        raise Exception("Could not find image band '{}'".format(imgbandidx))
    imgGeoTrans = imgDS.GetGeoTransform()
    img_wkt_str = imgDS.GetProjection()
    img_spatial_ref = osr.SpatialReference()
    img_spatial_ref.ImportFromWkt(img_wkt_str)

    imgNoDataVal = imgband.GetNoDataValue()

    vecDS = gdal.OpenEx(vecfile, gdal.OF_VECTOR|gdal.OF_UPDATE )
    if vecDS is None:
        raise Exception("Could not open '{}'".format(vecfile)) 

    veclyr = vecDS.GetLayerByName(veclyrname)
    if veclyr is None:
        raise Exception("Could not open layer '{}'".format(veclyrname))
    veclyr_spatial_ref = veclyr.GetSpatialRef()
    
    if not img_spatial_ref.IsSame(veclyr_spatial_ref):
        imgDS = None
        vecDS = None
        raise Exception("Inputted raster and vector layers have different projections: ('{0}' '{1}') ".format(vecfile, valsimg))
    
    veclyrDefn = veclyr.GetLayerDefn()
    
    outFieldAtts = [minfield, maxfield, meanfield, stddevfield, sumfield, countfield, modefield, medianfield]
    for outattname in outFieldAtts:
        if outattname is not None:
            found = False
            for i in range(veclyrDefn.GetFieldCount()):
                if veclyrDefn.GetFieldDefn(i).GetName().lower() in outattname.lower():
                    found = True
                    break
            if not found:
                veclyr.CreateField(ogr.FieldDefn(outattname.lower(), ogr.OFTReal))
    
    fieldAttIdxs = dict()
    for outattname in outFieldAtts:
        if outattname is not None:
            fieldAttIdxs[outattname] = veclyr.FindFieldIndex(outattname.lower(), True)
    
    vec_mem_drv = ogr.GetDriverByName('Memory')
    img_mem_drv = gdal.GetDriverByName('MEM')

    # Iterate through features.
    openTransaction = False
    nFeats = veclyr.GetFeatureCount(True)
    step = math.floor(nFeats/10)
    feedback = 10
    feedback_next = step
    counter = 0
    print("Started .0.", end='', flush=True)
    veclyr.ResetReading()
    feat = veclyr.GetNextFeature()
    while feat is not None:
        if (nFeats>10) and (counter == feedback_next):
            print(".{}.".format(feedback), end='', flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10
        
        if not openTransaction:
            veclyr.StartTransaction()
            openTransaction = True
        
        # Find the feature bbox
        feat_bbox = feat.geometry().GetEnvelope()
        pixel_width = imgGeoTrans[1]
        pixel_height = imgGeoTrans[5]
        x1 = int((feat_bbox[0] - imgGeoTrans[0]) / pixel_width)
        x2 = int((feat_bbox[1] - imgGeoTrans[0]) / pixel_width) + 1
        y1 = int((feat_bbox[3] - imgGeoTrans[3]) / pixel_height)
        y2 = int((feat_bbox[2] - imgGeoTrans[3]) / pixel_height) + 1
        xsize = x2 - x1
        ysize = y2 - y1
        # Define the image ROI for the feature
        src_offset = (x1, y1, xsize, ysize)
        # Read the band array.
        src_array = imgband.ReadAsArray(*src_offset)
        
        if src_array is not None:
    
            # calculate new geotransform of the feature subset
            subGeoTrans = ((imgGeoTrans[0] + (src_offset[0] * imgGeoTrans[1])), imgGeoTrans[1], 0.0, (imgGeoTrans[3] + (src_offset[1] * imgGeoTrans[5])), 0.0, imgGeoTrans[5])
    
            # Create a temporary vector layer in memory
            vec_mem_ds = vec_mem_drv.CreateDataSource('out')
            vec_mem_lyr = vec_mem_ds.CreateLayer('poly', veclyr_spatial_ref, ogr.wkbPolygon)
            vec_mem_lyr.CreateFeature(feat.Clone())
    
            # Rasterize the feature.
            img_tmp_ds = img_mem_drv.Create('', src_offset[2], src_offset[3], 1, gdal.GDT_Byte)
            img_tmp_ds.SetGeoTransform(subGeoTrans)
            img_tmp_ds.SetProjection(img_wkt_str)
            gdal.RasterizeLayer(img_tmp_ds, [1], vec_mem_lyr, burn_values=[1])
            rv_array = img_tmp_ds.ReadAsArray()
    
            # Mask the data vals array to feature (logical_not to flip 0<->1 to get the correct mask effect).
            if imgNoDataVal is not None:
                masked = numpy.ma.MaskedArray(src_array, mask=numpy.logical_or(src_array == imgNoDataVal, numpy.logical_not(rv_array), numpy.logical_and(src_array >= minthres, src_array < maxthres)))
            else:
                masked = numpy.ma.MaskedArray(src_array, mask=numpy.logical_or(numpy.logical_not(rv_array), numpy.logical_and(src_array >= minthres, src_array < maxthres)))
            
            if minfield is not None:
                min_val = float(masked.min())
                feat.SetField(fieldAttIdxs[minfield], min_val)
            if maxfield is not None:
                max_val = float(masked.max())
                feat.SetField(fieldAttIdxs[maxfield], max_val)
            if meanfield is not None:
                mean_val = float(masked.mean())
                feat.SetField(fieldAttIdxs[meanfield], mean_val)
            if stddevfield is not None:
                stddev_val = float(masked.std())
                feat.SetField(fieldAttIdxs[stddevfield], stddev_val)
            if sumfield is not None:
                sum_val = float(masked.sum())
                feat.SetField(fieldAttIdxs[sumfield], sum_val)
            if countfield is not None:
                count_val = float(masked.count())
                feat.SetField(fieldAttIdxs[countfield], count_val)
            if modefield is not None:
                mode_val, mode_count = scipy.stats.mstats.mode(masked.flatten())
                mode_val = float(mode_val)
                feat.SetField(fieldAttIdxs[modefield], mode_val)
            if medianfield is not None:
                median_val = float(numpy.ma.median(masked))
                feat.SetField(fieldAttIdxs[medianfield], median_val)
            # Write the updated feature to the vector layer.
            veclyr.SetFeature(feat)

            vec_mem_ds = None
            img_tmp_ds = None
        
        if ((counter % 20000) == 0) and openTransaction:
            veclyr.CommitTransaction()
            openTransaction = False
        
        feat = veclyr.GetNextFeature()
        counter = counter + 1
    if openTransaction:
        veclyr.CommitTransaction()
        openTransaction = False
    veclyr.SyncToDisk()
    print(" Completed")

    vecDS = None
    imgDS = None

