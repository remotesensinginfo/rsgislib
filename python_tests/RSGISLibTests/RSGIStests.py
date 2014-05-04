#!/usr/bin/env python

############################################################################
# Copyright (c) 2013 Sebastian Clarke, RSGISLib
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
#
# Purpose:  Test suite for RSGISLib python bindings
# Author: Sebastian Clarke
# Email: sec14@aber.ac.uk
# Date: 30/07/2013
# Version: 1.0
#
#############################################################################

import sys, os
try:
    import rsgislib
    from rsgislib import imageutils
    from rsgislib import vectorutils
    from rsgislib import imagecalc
    from rsgislib import rastergis
    from rsgislib import zonalstats
    from rsgislib import imageregistration
    from rsgislib import imagefilter
    from rsgislib.imagecalc import BandDefn
except ImportError as err:
    print(err)
    sys.exit()
    

path = os.sys.path[0] + '/'
inFileName = os.path.join(path,"Rasters","injune_p142_casi_sub_utm.kea")
print(inFileName)

class RSGISTests:

    def __init__(self):
        self.numTests = 0
        self.failures = []
        self.testOutputsDIR = os.path.join(path,'TestOutputs')
        self.testRasterGISDIR = os.path.join(path,'TestOutputs','RasterGIS')
        self.testTilesDIR = os.path.join(path,'TestOutputs','Tiles')
        self.testZonalTXTDIR = os.path.join(path,'TestOutputs','ZonalTXT')

    def tryFuncAndCatch(self, function):
        self.numTests += 1
        try:
            function()
        except Exception as inst:
            print("ERROR - ERROR - ERROR")
            print("inst")
            self.failures.append(function.__name__)

    def copyData(self):
        """ Copy data files from original directory to test directory """
        print("COPYING DATA")
        os.system('cp ./RATS/injune_p142_casi_sub_utm_segs.kea ./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_cptab.kea')
        os.system('cp ./RATS/injune_p142_casi_sub_utm_segs.kea ./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_cpcols.kea')
        os.system('cp ./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea ./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_spatloc_eucdist.kea')
        os.system('cp ./RATS/injune_p142_casi_sub_utm_segs.kea ./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_popstats.kea')
        os.system('cp ./RATS/injune_p142_casi_sub_utm_segs.kea ./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_neighbours.kea')
        os.system('cp ./RATS/injune_p142_casi_sub_utm_segs.kea ./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_borlen.kea')
        os.system('cp ./RATS/injune_p142_casi_sub_utm_segs.kea ./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_borlen.kea')
        os.system('cp ./RATS/injune_p142_casi_sub_utm_segs.kea ./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_shape.kea')
        os.system('cp ./Rasters/injune_p142_casi_sub_utm.kea ./TestOutputs/injune_p142_casi_sub_utm.kea')
        
        os.system('cp ./RATS/injune_p142_casi_sub_utm_segs_nostats.kea ./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_nostats_addstats.kea')
        os.system('cp ./Vectors/injune_p142_psu_utm.* ./TestOutputs/')
    
    def checkDIRStructure(self):
        """ Create directory and sub directories for test
            outputs, if they don't already exist.  """
        if os.path.isdir(self.testOutputsDIR) == False:
            os.mkdir(self.testOutputsDIR)

        if os.path.isdir(self.testRasterGISDIR) == False:
            os.mkdir(self.testRasterGISDIR)
            
        if os.path.isdir(self.testTilesDIR) == False:
            os.mkdir(self.testTilesDIR)
    
        if os.path.isdir(self.testZonalTXTDIR) == False:
            os.mkdir(self.testZonalTXTDIR)

    def removeTestFiles(self):
        """ Removes all files in test directory """
        print('Removing test files')
        os.system('rm ' + self.testOutputsDIR + '/* 2> /dev/null')
        os.system('rm ' + self.testRasterGISDIR + '/* 2> /dev/null')
        os.system('rm ' + self.testTilesDIR + '/* 2> /dev/null')
        os.system('rm ' + self.testZonalTXTDIR + '/* 2> /dev/null')

    # Image Calc

    def testNormalise1(self):
        print("PYTHON TEST: Testing normalisation no in calc")
        inImages = [inFileName];
        outImages = [path+"TestOutputs/injune_p142_casi_sub_ll_norm.env"]
        imagecalc.normalisation(inImages, outImages, False, 0, 0, 0, 100)

    def testNormalise2(self):
        print("PYTHON TEST: Testing normalisation with in calc")
        inImages = [inFileName];
        outImages = [path+"TestOutputs/injune_p142_casi_sub_ll_norm.env"]
        imagecalc.normalisation(inImages, outImages, True, 0, 1000, 0, 50)

    def testCorrelation(self):
        print("PYTHON TEST: Testing correlation")
        inImage = inFileName
        outImage = path+"TestOutputs/PSU142Correlation"
        imagecalc.correlation(inImage, inImage, outImage)

    def testCovariance1(self):
        print("PYTHON TEST: Testing covariance - with mean calculation")
        inImage = inFileName
        outImage = path+"TestOutputs/PSU142Correlation"
        imagecalc.covariance(inImage, inImage, "", "", True, outImage)

    def testCovariance2(self):
        # TODO: Missing matrix files
        print("PYTHON TEST: Testing covariance - no mean calculation")
        inImage = inFileName
        outImage = path+"TestOutputs/matrix"
        imagecalc.covariance(inImage, inImage, "matrixA.mtxt", "matrixB.mtxt", False, outImage)

    def testCalcRMSE(self):
        # note erratic behaviour
        print("PYTHON TEST: Testing RMSE")
        inImageA = inFileName
        inImageB = inFileName
        bandA = 1
        bandB = 5
        imagecalc.calculateRMSE(inImageA, bandA, inImageB, bandB)

    def testMeanVector(self):
        print("PYTHON TEST: Testing meanVector")
        inImage = inFileName
        outVector = path+"TestOutputs/PSU142MeanVec"
        imagecalc.meanVector(inImage, outVector)

    def testPCA(self):
        print("PYTHON TEST: unable to test PCA - awaiting eigenvectors")

    def testStandardise(self):
        print("PYTHON TEST: Testing standardise")
        outImage = path + "TestOutputs/PSU142_Standarised.env"
        meanVector = path + "TestOutputs/PSU142MeanVec.mtxt"
        imagecalc.standardise(meanVector, inFileName, outImage)

    def testBandMath(self):
        # NOT WORKING band not in image
        print("PYTHON TEST: Testing bandMath")
        outputImage = path + "TestOutputs/PSU142_b1mb2.kea"
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        expression = "b1*b2"
        bandDefns = []
        bandDefns.append(BandDefn("b1", inFileName, 1))
        bandDefns.append(BandDefn("b2", inFileName, 2))
        imagecalc.bandMath(outputImage, expression, gdalformat, dataType, bandDefns)

    def testImageMaths(self):
        print("PYTHON TEST: Testing imageMath")
        outputImage = path + "TestOutputs/PSU142_multi1000.kea"
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32UINT
        expression = "1e3*b1"
        imagecalc.imageMath(inFileName, outputImage, expression, gdalformat, dataType)

    def testReplaceValuesLessThan(self):
        print("PYTHON TEST: Replace values less than")
        outputImage = path + "TestOutputs/injune_p142_casi_sub_ll_lt100.kea"
        imagecalc.replaceValuesLessThan(inFileName, outputImage, 100.0, 0)

    def testUnitArea(self):
        print("PYTHON TEST: unit area - CANNOT TEST - awaiting imagebands matrix.mtxt")

    def testMovementSpeed(self):
        print("PYTHON TEST: movement speed - CANNOT TEST - awaiting multiple images")

    def testCountValsInCol(self):
        print("PYTHON TEST: countValsInCols")
        outputImage = path + "TestOutputs/injune_p142_casi_sub_ll_count.kea"
        upper = 1000
        lower = 500
        imagecalc.countValsInCols(inFileName, upper, lower, outputImage)

    def testDist2Geoms(self):
        print("PYTHON TEST: dist2Geoms - not testing due to poor time perfomance and lack of example geometries")

    def testImgDist2Geoms(self):
        print("PYTHON TEST: imgDist2Geoms - not testing due to poor time perfomance and lack of example geometries")

    def testImgCalcDist(self):
        print("PYTHON TEST: imgCalcDist - not testing due to no example in XML")

    def testImageBandStats(self):
        print("PYTHON TEST: imageBandStats - not ignoring Zeros")
        outputImage = path + "TestOutputs/BandsStats.txt"
        imagecalc.imageBandStats(inFileName, outputImage, False)

    def testImageBandStatsIgnoreZeros(self):
        print("PYTHON TEST: imageBandStats - Ignoring Zeros")
        outputImage = path + "TestOutputs/BandsStatsIgnore.txt"
        imagecalc.imageBandStats(inFileName, outputImage, True)

    def testImageStats(self):
        print("PYTHON TEST: imageStats - Not Ignoring Zeros")
        outputImage = path + "TestOutputs/ImageStats.txt"
        imagecalc.imageStats(inFileName, outputImage, False)

    def testImageStatsIgnoreZeros(self):
        print("PYTHON TEST: imageStats - Ignoring Zeros")
        outputImage = path + "TestOutputs/ImageStats.txt"
        imagecalc.imageStats(inFileName, outputImage, True)

    def testUnconLinearSpecUnmix(self):
        print("PYTHON TEST: unconLinearSpecUnmix - skipping due to lack of test data")

    def testExhConLinearSpecUnmix(self):
        print("PYTHON TEST: exhConinearSpecUnmix - skipping due to lack of test data")

    def testConSum1LinearSpecUnmix(self):
        print("PYTHON TEST: ConSum1LinearSpecUnmix - skipping due to lack of test data")

    def testNnConSum1LinearSpecUnmix(self):
        print("PYTHON TEST: NnConSum1LinearSpecUnmix - skipping due to lack of test data")

    def testKMeansCentres(self):
        # Only seems to do one iteration?
        print("PYTHON TEST: kMeansClustering")
        inputImage = path + "Rasters/injune_p142_casi_sub_right_utm.kea"
        output = path + "TestOutputs/kmeanscentres"
        numClust = 10
        maxIter = 200
        degChange = 0.0025
        subSample = 1
        ignoreZeros = True
        imagecalc.kMeansClustering(inputImage, output, numClust, maxIter, subSample, ignoreZeros, degChange, rsgislib.INITCLUSTER_DIAGONAL_FULL_ATTACH)

    def testIsoDataClustering(self):
        # Only seems to do one iteration?
        print("PYTHON TEST: isoDataClustering")
        inputImage = path + "Rasters/injune_p142_casi_sub_right_utm.kea"
        output = path + "TestOutputs/isocentres"
        imagecalc.isoDataClustering(inputImage, output, 10, 200, 1, True, 0.0025, rsgislib.INITCLUSTER_DIAGONAL_FULL_ATTACH, 2, 5, 5, 5, 8, 50)

    def testAllBandsEqualTo(self):
        print("PYTHON TEST: allBandsEqualTo")
        inputImage = path + "Rasters/injune_p142_casi_sub_right_utm.kea"
        output = path + "TestOutputs/PSU142_Allbandsequal0Mask.kea"
        gdalformat = "KEA"
        value = 0
        trueout = 0
        falseout = 1
        dataType = rsgislib.TYPE_8INT
        imagecalc.allBandsEqualTo(inputImage, value, trueout, falseout, output, gdalformat, dataType)

    def testHistogram(self):
        print("PYTHON TEST: Histogram")
        image = path + "Rasters/injune_p142_casi_sub_right_utm.kea"
        out = path + "TestOutputs/injune_p142_casi_sub_right_utm_histo.txt"
        mask = path + "TestOutputs/PSU142_Allbandsequal0Mask.kea"
        value = 1
        band = 1
        imagecalc.histogram(image, mask, out, band, value, 5, True, 0, 0)

    def testBandPercentile(self):
        print("PYTHON TEST: Band Percentile")
        image = path + "Rasters/injune_p142_casi_sub_right_utm.kea"
        output = path + "TestOutputs/PSU142_bandpercentile"
        percentile = 0.25
        nodata = 0
        imagecalc.bandPercentile(image, percentile, nodata, True, output)

    def testMahalanobisDistWindow(self):
        print("PYTHON TEST: MahalanobisDistWindow")
        image = path + "Rasters/injune_p142_casi_sub_right_utm.kea"
        output = path + "TestOutputs/PSU142_MabDistWin.kea"
        window = 7
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        imagecalc.mahalanobisDistFilter(image, output, window, gdalformat, dataType)

    def testMahalanobisDistImg2Window(self):
        print("PYTHON TEST: MahalanobisDistImg2Window")
        image = path + "Rasters/injune_p142_casi_sub_right_utm.kea"
        output = path + "TestOutputs/PSU142_MabDist2ImgWin.kea"
        window = 7
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        imagecalc.mahalanobisDist2ImgFilter(image, output, window, gdalformat, dataType)

    def testCalcPxlColStats(self):
        print("PYTHON TEST: pxlColStats")
        image = path + "Rasters/injune_p142_casi_sub_right_utm.kea"
        output = path + "TestOutputs/PSU142_ImageStats.kea"
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        s = imagecalc.StatsSummary(calcMin=True, calcMax=True, calcSum=True, calcMean=True, calcMedian=True, calcStdDev=True, min=0.0)
        imagecalc.imagePixelColumnSummary(image, output, s, gdalformat, dataType, 0, True)

    def testPxlColRegression(self):
        print("PYTHON TEST: pxlColRegression - skipping due to lack of bandvalues file.txt")

    def testCorrelationWindow(self):
        print("PYTHON TEST: correlationWindow")
        image = path + "Rasters/injune_p142_casi_sub_utm.kea"
        output = path + "TestOutputs/injune_p142_casi_sub_utm_correlation.kea"
        window = 9
        bandA = 1
        bandB = 1
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        imagecalc.correlationWindow(image, output, window, bandA, bandB, gdalformat, dataType)

    # Raster GIS

    def testCopyGDLATT(self):
        print("PYTHON TEST: copyRAT")
        table = "./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea"
        image = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_cptab.kea"
        rastergis.copyRAT(image, table)

    def testCopyGDLATTColumns(self):
        print("PYTHON TEST: copyGDALATTColumns")
        table = "./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea"
        image = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_cpcols.kea"
        fields = ["BlueAvg", "GreenAvg", "RedAvg"]
        rastergis.copyGDALATTColumns(table, image, fields)
    """
    def testSpatialLocation(self):
        print("PYTHON TEST: spatialLocation")
        image = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_spatloc_eucdist.kea"
        eastings = "Easts"
        northings = "Norths"
        rastergis.spatialLocation(image, eastings, northings)

    def testEucDistFromFeat(self):
        print("PYTHON TEST: eucDistFromFeature")
        image = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_spatloc_eucdist.kea"
        feature=10
        outfield="DistTo10"
        fields = ["Easts", "Norths"]
        rastergis.eucDistFromFeature(image, feature, outfield, fields)

    def testFindTopN(self):
        print("PYTHON TEST: findTopN")
        image = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_spatloc_eucdist.kea"
        spatialdist="DistTo10"
        metricdist="RedAvg"
        n=10
        distthreshold=100
        outfield="Top10Feats"
        rastergis.findTopN(image, spatialdist, metricdist, outfield, n, distthreshold)

    def testFindSpecClose(self):
        print("PYTHON TEST: findSpecClose")
        image = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_spatloc_eucdist.kea"
        spatialdist="DistTo10"
        metricdist="RedAvg"
        n=10
        specdistthreshold=20
        spatdistthreshold=100
        outfield="Closest2Feat10"
        rastergis.findSpecClose(image, spatialdist, metricdist, outfield, specdistthreshold, spatdistthreshold)
    """
    def testPopulateRATWithStats(self):
        print("PYTHON TEST: populateRATWithStats")
        clumps="./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_popstats.kea"
        input="./Rasters/injune_p142_casi_sub_utm.kea"
        bs = []
        bs.append(rastergis.BandAttStats(band=1, minField="b1Min", maxField="b1Max", meanField="b1Mean", sumField="b1Sum", stdDevField="b1StdDev"))
        bs.append(rastergis.BandAttStats(band=2, minField="b2Min", maxField="b2Max", meanField="b2Mean", sumField="b2Sum", stdDevField="b2StdDev"))
        bs.append(rastergis.BandAttStats(band=3, minField="b3Min", maxField="b3Max", meanField="b3Mean", sumField="b3Sum", stdDevField="b3StdDev"))
        rastergis.populateRATWithStats(input, clumps, bs)
    """
    def testPopulateRATWithPercentiles(self):
        print("PYTHON TEST: populateRATWithPercentiles")
        clumps = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_popstats.kea"
        input = "./Rasters/injune_p142_casi_sub_utm.kea"
        bp = []
        bp.append(rastergis.BandAttPercentiles(band=1, percentile=25, fieldName="B1Per25"))
        bp.append(rastergis.BandAttPercentiles(band=1, percentile=50, fieldName="B1Per50"))
        bp.append(rastergis.BandAttPercentiles(band=1, percentile=75, fieldName="B1Per75"))
        rastergis.populateRATWithPercentiles(input, clumps, bp)

    """
    def testExport2Ascii(self):
        print("PYTHON TEST: export2Ascii")
        table="./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea"
        output="./TestOutputs/RasterGIS/injune_p142_casi_rgb_exportascii.txt"
        fields = ["BlueAvg", "GreenAvg", "RedAvg"]
        rastergis.export2Ascii(table, output, fields)

    def testExporCols2GDALImage(self):
        print("PYTHON TEST: exportCol2GDALImage")
        clumps="./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea"
        output="./TestOutputs/RasterGIS/injune_p142_casi_rgb_export.kea"
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        field = "RedAvg"
        rastergis.exportCol2GDALImage(clumps, output, gdalformat, dataType, field)
    """
    def testFindNeighbours(self):
        print("PYTHON TEST: findNeighbours")
        input = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_neighbours.kea"
        rastergis.findNeighbours(input)

    def testFindBoundaryPixels(self):
        print("PYTHON TEST: findBoundaryPixels")
        clumps="./RATS/injune_p142_casi_sub_utm_segs.kea"
        output="./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_borpxls.kea"
        gdalformat="KEA"
        rastergis.findBoundaryPixels(clumps, output, gdalformat)

    def testCalcBorderLength(self):
        print("PYTHON TEST: calcBorderLength")
        clumps="./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_borlen.kea"
        ignorezeroedges=True
        colname="borderLen"
        rastergis.calcBorderLength(clumps, ignorezeroedges, colname)

    def testCalcShapeIndices(self):
        print("PYTHON TEST: calcShapeIndices")
        clumps = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_shape.kea"
        shapes = []
        shapes.append(rastergis.ShapeIndex(colName="Area", idx=rsgislib.SHAPE_SHAPEAREA))
        shapes.append(rastergis.ShapeIndex(colName="Length", idx=rsgislib.SHAPE_LENGTH))
        shapes.append(rastergis.ShapeIndex(colName="Width", idx=rsgislib.SHAPE_WIDTH))
        shapes.append(rastergis.ShapeIndex(colName="BorderLengthIdx", idx=rsgislib.SHAPE_BORDERLENGTH))
        shapes.append(rastergis.ShapeIndex(colName="Asymmetry", idx=rsgislib.SHAPE_ASYMMETRY))
        shapes.append(rastergis.ShapeIndex(colName="ShapeIndex", idx=rsgislib.SHAPE_SHAPEINDEX))
        shapes.append(rastergis.ShapeIndex(colName="Density", idx=rsgislib.SHAPE_DENSITY))
        shapes.append(rastergis.ShapeIndex(colName="LengthWidthRatio", idx=rsgislib.SHAPE_LENGTHWIDTH))
        shapes.append(rastergis.ShapeIndex(colName="BorderIndex", idx=rsgislib.SHAPE_BORDERINDEX))
        shapes.append(rastergis.ShapeIndex(colName="Compactness", idx=rsgislib.SHAPE_COMPACTNESS))
        shapes.append(rastergis.ShapeIndex(colName="MainDirection", idx=rsgislib.SHAPE_MAINDIRECTION))
        rastergis.calcShapeIndices(clumps, shapes)

    def testPopulateStats(self):
        print("PYTHON TEST: populateStats")
        clumps="./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_nostats_addstats.kea"
        pyramids=True
        colourtable=True
        rastergis.populateStats(clumps, colourtable, pyramids)
    """
    # Image Utils 
    
    def testCreateTiles(self):
        print("PYTHON TEST: createTiles")
        inputImage = path + "Rasters/injune_p142_casi_sub_utm.kea"
        outBase = './TestOutputs/Tiles/injune_p142_casi_sub_utm'
        width = 100
        height = width
        overlap = 5
        offsettiling = 0
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32INT
        ext='kea'
        imageutils.createTiles(inputImage, outBase, width, height, overlap, offsettiling, gdalformat, dataType, ext)

    def testCreateImageMosaic(self):
        print("PYTHON TEST: createImageMosaic")
        inputList = ['./TestOutputs/Tiles/injune_p142_casi_sub_utm_tile0.kea','./TestOutputs/Tiles/injune_p142_casi_sub_utm_tile1.kea','./TestOutputs/Tiles/injune_p142_casi_sub_utm_tile2.kea','./TestOutputs/Tiles/injune_p142_casi_sub_utm_tile3.kea']
        outImage = './TestOutputs/injune_p142_casi_sub_utm_remosaic.kea'
        backgroundVal = 0.
        skipVal = 0.
        skipBand = 1
        overlapBehaviour = 0
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        imageutils.createImageMosaic(inputList, outImage, backgroundVal, skipVal, skipBand, overlapBehaviour, gdalformat, dataType)

    def testIncludeImages(self):
        print("PYTHON TEST: includeImages")
        baseImage = './TestOutputs/injune_p142_casi_sub_utm_remosaic.kea'
        inputList = ['./TestOutputs/Tiles/injune_p142_casi_sub_utm_tile0.kea','./TestOutputs/Tiles/injune_p142_casi_sub_utm_tile1.kea']
        imageutils.includeImages(baseImage, inputList)
    
    def testPopImageStats(self):
        print("PYTHON TEST: popImageStats")
        inputImage = './TestOutputs/injune_p142_casi_sub_utm.kea'
        imageutils.popImageStats(inputImage,True,0.,True)
        
    def testSubset(self):
        print("PYTHON TEST: subset")
        inputImage = './Rasters/injune_p142_casi_sub_utm.kea'
        inputVector = './Vectors/injune_p142_plot_location_utm.shp'
        outputImage = './TestOutputs/injune_p142_casi_sub_utm_subset.kea'
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        imageutils.subset(inputImage, inputVector, outputImage, gdalformat, dataType)

    def testSubset2Polys(self):
        print("PYTHON TEST: subset2polys")
        inputImage = './Rasters/injune_p142_casi_sub_utm.kea'
        inputVector = './Vectors/injune_p142_plot_location_utm.shp'
        attribute = 'PLOTNO'
        outputImageBase = './TestOutputs/injune_p142_casi_sub_utm_subset_polys_'
        gdalformat = 'KEA'
        dataType = rsgislib.TYPE_32FLOAT
        ext = 'kea'
        outFiles = imageutils.subset2polys(inputImage, inputVector, attribute, outputImageBase, gdalformat, dataType, ext)

    def testSubset2Img(self):
        print("PYTHON TEST: subset2img")
        inputImage = './Rasters/injune_p142_casi_sub_utm.kea'
        inputROIImage = './TestOutputs/injune_p142_casi_sub_utm_subset.kea'
        outputImage = './TestOutputs/injune_p142_casi_sub_utm_subset2img.kea'
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        imageutils.subset2img(inputImage, inputROIImage, outputImage, gdalformat, dataType)

    def testStackBands(self):
        print("PYTHON TEST: stackImageBands")
        imageList = ['./Rasters/injune_p142_casi_sub_utm_single_band.vrt','./Rasters/injune_p142_casi_sub_utm_single_band.vrt']
        bandNamesList = ['Image1','Image2']
        outputImage = './TestOutputs/injune_p142_casi_sub_stack.kea'
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        imageutils.stackImageBands(imageList, bandNamesList, outputImage, None, 0, gdalformat, dataType)

    def testCreateCopyImage(self):
        print("PYTHON TEST: createCopyImage")
        inputImage = './Rasters/injune_p142_casi_sub_utm.kea'
        outputImage = './TestOutputs/injune_p142_casi_sub_utm_blank.kea'
        gdalformat = 'KEA'
        dataType = rsgislib.TYPE_32FLOAT
        imageutils.createCopyImage(inputImage, outputImage, 1, 3, gdalformat, dataType)

    def testStackStats(self):
        print("PYTHON TEST: stackStats")
        inputImage = './Rasters/injune_p142_casi_sub_utm.kea'
        outputImage = './TestOutputs/injune_p142_casi_sub_utm_stackStats.kea'
        gdalformat = 'KEA'
        dataType = rsgislib.TYPE_32FLOAT
        imageutils.stackStats(inputImage, outputImage, None, 'mean', gdalformat, dataType)

    def testStretchImage(self):
        print("PYTHON TEST: stretchImage")
        inputImage = './Rasters/injune_p142_casi_sub_utm.kea'
        outputImage = './TestOutputs/injune_p142_casi_sub_utm_2sd.kea'
        gdalformat = 'KEA'
        dataType = rsgislib.TYPE_8INT
        imageutils.stretchImage(inputImage, outputImage, False, "", True, False, gdalformat, dataType, imageutils.STRETCH_LINEARSTDDEV, 2)

    def testSetBandNames(self):
        print("PYTHON TEST: popImageStats")
        inputImage = './TestOutputs/injune_p142_casi_sub_utm.kea'
        bandNames = ['446nm','530nm','549nm','569nm','598nm','633nm','680nm','696nm','714nm','732nm','741nm','752nm','800nm','838nm']
        imageutils.setBandNames(inputImage, bandNames)

     # Zonal Stats

    def testPointValue2SHP(self):
        # Not all pixels are within image - should print warnings but pass test.
        print("PYTHON TEST: pointValues2SHP")
        inputImage = './Rasters/injune_p142_casi_sub_utm.kea'
        inputVector = './Vectors/injune_p142_stem_locations.shp'
        outputVector = './TestOutputs/injune_p142_stem_locations_stats.shp'
        zonalstats.pointValue2SHP(inputImage, inputVector, outputVector, True, True)

    def testPointValue2TXT(self):
        # Not all pixels are within image - should print warnings but pass test.
        print("PYTHON TEST: pointValues2TXT")
        inputImage = './Rasters/injune_p142_casi_sub_utm.kea'
        inputVector = './Vectors/injune_p142_stem_locations.shp'
        outputTxt = './TestOutputs/injune_p142_stem_locations_stats_txt.csv'
        zonalstats.pointValue2TXT(inputImage, inputVector, outputTxt, True)
        
    def testPixelStats2SHP(self, allCases=False):
        """ Check for three cases, all polygons inside image (1) and polygons outside image (2) """
        print("PYTHON TEST: pixelStats2SHP")
        inputImage1 = './Rasters/injune_p142_casi_sub_utm.kea'
        inputImage2 = './Rasters/injune_p142_casi_sub_left_utm.kea'
        inputImage3 = './Rasters/injune_p142_casi_sub_right_utm.kea'
        inputVector = './Vectors/injune_p142_crowns_utm.shp'
        outputVector1 = './TestOutputs/injune_p142_casi_sub_utm_stats1.shp'
        outputVector2 = './TestOutputs/injune_p142_casi_sub_utm_stats2.shp'
        outputVector3 = './TestOutputs/injune_p142_casi_sub_utm_stats3.shp'
        zonalattributes = zonalstats.ZonalAttributes(minThreshold=0, maxThreshold=10000, calcCount=True, calcMin=True, calcMax=True, calcMean=True, calcStdDev=True, calcMode=False, calcSum=True)
        zonalstats.pixelStats2SHP(inputImage1, inputVector, outputVector1, zonalattributes, True, True, True, zonalstats.METHOD_POLYCONTAINSPIXELCENTER)
        if allCases:
            zonalstats.pixelStats2SHP(inputImage2, inputVector, outputVector2, zonalattributes, True, True, True, zonalstats.METHOD_POLYCONTAINSPIXELCENTER)
            zonalstats.pixelStats2SHP(inputImage3, inputVector, outputVector3, zonalattributes, True, True, True, zonalstats.METHOD_POLYCONTAINSPIXELCENTER)
    
    def testPixelStats2TXT(self):
        print("PYTHON TEST: pixelStats2TXT")
        inputImage = './Rasters/injune_p142_casi_sub_left_utm.kea'
        inputVector = './Vectors/injune_p142_crowns_utm.shp'
        outputTxt = './TestOutputs/injune_p142_casi_sub_utm_stats_txt.csv'
        zonalattributes = zonalstats.ZonalAttributes(minThreshold=0, maxThreshold=10000, calcCount=True, calcMin=True, calcMax=True, calcMean=True, calcStdDev=True, calcMode=False, calcSum=True)
        zonalstats.pixelStats2TXT(inputImage, inputVector, outputTxt, zonalattributes, True, True, zonalstats.METHOD_POLYCONTAINSPIXELCENTER)
    
    def testPixelVals2TXT(self):
        print("PYTHON TEST: pixelVals2TXT")
        inputImage = './Rasters/injune_p142_casi_sub_utm.kea'
        inputVector = './Vectors/injune_p142_crowns_utm.shp'
        outputTxtBase = './TestOutputs/ZonalTXT/injune_p142_casi_sub_utm_txt'
        zonalstats.pixelVals2TXT(inputImage, inputVector, outputTxtBase, 'FID', True, zonalstats.METHOD_POLYCONTAINSPIXELCENTER)    
    
    def testImageZone2HDF(self):
        print("PYTHON TEST: pixelVals2TXT")
        inputimage = './Rasters/injune_p142_casi_sub_utm.kea'
        inputvector = './Vectors/injune_p142_crowns_withincasi_utm.shp'
        outputHDF = './TestOutputs/InjuneP142.hdf'
        zonalstats.imageZoneToHDF(inputimage, inputvector, outputHDF, True, zonalstats.METHOD_POLYCONTAINSPIXELCENTER)
        
    # Image Registration
    def testBasicRegistration(self):
        print("PYTHON TEST: basicregistration")
        reference = './Rasters/injune_p142_casi_sub_utm_single_band.vrt'        
        floating = './Rasters/injune_p142_casi_sub_utm_single_band_offset3x3y.vrt'
        pixelGap = 50 
        threshold = 0.4
        window = 100
        search = 5
        stddevRef = 2
        stddevFloat = 2
        subpixelresolution = 4
        metric = imageregistration.METRIC_CORELATION
        outputType = imageregistration.TYPE_RSGIS_IMG2MAP
        output = './TestOutputs/injune_p142_casi_sub_utm_tie_points_basic.txt'
        imageregistration.basicregistration(reference, floating, pixelGap, threshold, window, search, stddevRef, stddevFloat, subpixelresolution, metric, outputType, output)
        
    def testSingleLayerRegistration(self):
        print("PYTHON TEST: singlelayerregistration")
        reference = './Rasters/injune_p142_casi_sub_utm_single_band.vrt'        
        floating = './Rasters/injune_p142_casi_sub_utm_single_band_offset3x3y.vrt'
        pixelGap = 50 
        threshold = 0.4
        window = 100
        search = 5
        stddevRef = 2
        stddevFloat = 2
        subpixelresolution = 4
        distanceThreshold = 100
        maxiterations = 10
        movementThreshold = 0.01 
        pSmoothness = 2    
        metric = imageregistration.METRIC_CORELATION
        outputType = imageregistration.TYPE_RSGIS_IMG2MAP
        output = './TestOutputs/injune_p142_casi_sub_utm_tie_points_singlelayer.txt'
        imageregistration.singlelayerregistration(reference, floating, pixelGap, threshold, window, search, stddevRef, stddevFloat, subpixelresolution, distanceThreshold, maxiterations, movementThreshold, pSmoothness, metric, outputType, output)
        
    def testTriangularWarp(self):        
        inputImage = './Rasters/injune_p142_casi_sub_utm_single_band_offset3x3y.vrt'
        inputGCPs = './TestOutputs/injune_p142_casi_sub_utm_tie_points_basic.txt'
        outputImage = './TestOutputs/injune_p142_casi_sub_utm_single_band_offset3x3y_twarp.kea'
        wktStringFile = './Vectors/injune_p142_crowns_utm.prj'
        resolution = 1
        gdalformat = 'KEA'
        imageregistration.triangularwarp(inputImage,inputGCPs, outputImage, wktStringFile, resolution, gdalformat)
        
    def testNNWarp(self):        
        inputImage = './Rasters/injune_p142_casi_sub_utm_single_band_offset3x3y.vrt'
        inputGCPs = './TestOutputs/injune_p142_casi_sub_utm_tie_points_basic.txt'
        outputImage = './TestOutputs/injune_p142_casi_sub_utm_single_band_offset3x3y_nnwarp.kea'
        wktStringFile = './Vectors/injune_p142_crowns_utm.prj'
        resolution = 1
        gdalformat = 'KEA'
        imageregistration.nnwarp(inputImage,inputGCPs, outputImage, wktStringFile, resolution, gdalformat)

    def testPolyWarp(self):        
        inputImage = './Rasters/injune_p142_casi_sub_utm_single_band_offset3x3y.vrt'
        inputGCPs = './TestOutputs/injune_p142_casi_sub_utm_tie_points_basic.txt'
        outputImage = './TestOutputs/injune_p142_casi_sub_utm_single_band_offset3x3y_polywarp.kea'
        wktStringFile = './Vectors/injune_p142_crowns_utm.prj'
        resolution = 1
        polyOrder = 1
        gdalformat = 'KEA'
        imageregistration.polywarp(inputImage,inputGCPs, outputImage, wktStringFile, resolution, polyOrder, gdalformat)

    def testGCP2GDAL(self):
        print("PYTHON TEST: gcps2gdal")
        inputImage = './Rasters/injune_p142_casi_sub_utm_single_band_offset3x3y.vrt'
        inputGCPs = './TestOutputs/injune_p142_casi_sub_utm_tie_points_basic.txt'
        outputImage = './TestOutputs/injune_p142_casi_sub_utm_single_band_offset3x3y_gcps.kea'
        gdalformat = "KEA"
        dataType = rsgislib.TYPE_32INT
        imageregistration.gcp2gdal(inputImage,inputGCPs, outputImage, gdalformat, dataType)
    
    # Vector utils
    def testRemoveAttributes(self):
        print("PYTHON TEST: removeattributes")
        inputVector = './Vectors/injune_p142_stem_locations.shp'
        outputVector = './TestOutputs/injune_p142_stem_locations_noatts.shp'
        vectorutils.removeattributes(inputVector, outputVector, True)

    def testBufferVector(self):
        print("PYTHON TEST: buffervector")
        inputVector = './Vectors/injune_p142_stem_locations.shp'
        outputVector = './TestOutputs/injune_p142_stem_locations_1mbuffer.shp'
        bufferDist = 1
        vectorutils.buffervector(inputVector, outputVector, bufferDist, True)

    def testPrintPolyGeom(self):
        print("PYTHON TEST: buffervector")
        inputVector = './Vectors/injune_p142_psu_utm.shp'
        vectorutils.printpolygeom(inputVector)

    def testFindReplaceText(self):
        print("PYTHON TEST: findreplacetext")
        inputVector = './TestOutputs/injune_p142_psu_utm.shp'
        attribute = 'PSU'
        find = '142'
        replace = '142'
        vectorutils.findreplacetext(inputVector, attribute, find, replace)

    def testCalcArea(self):
        print("PYTHON TEST: calcarea")
        inputVector = './Vectors/injune_p142_psu_utm.shp'
        outputVector = './TestOutputs/injune_p142_psu_utm_area.shp'
        vectorutils.calcarea(inputVector, outputVector, True)

    def testPolygonsInPolygon(self):
        print("PYTHON TEST: polygonsInPolygon")
        inputVector = './Vectors/injune_p142_stem_locations.shp'
        coverVector = './Vectors/injune_p142_psu_utm.shp'
        outDIR = './TestOutputs'
        attribute = 'PSU'
        vectorutils.polygonsInPolygon(inputVector, coverVector, outDIR, attribute, True)


    # Image filter
    def testFilter(self, allFilters=False):
        print("PYTHON TEST: filter")
        inputImage = './Rasters/injune_p142_casi_sub_utm_single_band.vrt'
        outputImageBase = './TestOutputs/injune_p142_casi_sub_utm_single_band'
        gdalFormat = 'KEA'
        outExt = 'kea'
        dataType = rsgislib.TYPE_32FLOAT
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'GaussianSmooth', fileEnding = 'gausmooth', size=3, stddevX = 1., stddevY = 1, angle = 0.) )
        if allFilters:
            filters.append(imagefilter.FilterParameters(filterType = 'Gaussian1st', fileEnding = 'gau1st', size=3, stddevX = 1., stddevY = 1, angle = 0.) )
            filters.append(imagefilter.FilterParameters(filterType = 'Gaussian2nd', fileEnding = 'gau1st', size=3, stddevX = 1., stddevY = 1, angle = 0.) )
            filters.append(imagefilter.FilterParameters(filterType = 'Laplacian', fileEnding = 'laplacian', size=3, stddev = 1) )
            filters.append(imagefilter.FilterParameters(filterType = 'Sobel', fileEnding = 'sobelx', option = 'x') )
            filters.append(imagefilter.FilterParameters(filterType = 'Sobel', fileEnding = 'sobely', option = 'y') )
            filters.append(imagefilter.FilterParameters(filterType = 'Sobel', fileEnding = 'sobelxy', option = 'xy') )
            filters.append(imagefilter.FilterParameters(filterType = 'Prewitt', fileEnding = 'prewittx', option = 'x') )
            filters.append(imagefilter.FilterParameters(filterType = 'Prewitt', fileEnding = 'prewitty', option = 'y') )
            filters.append(imagefilter.FilterParameters(filterType = 'Prewitt', fileEnding = 'prewittxy', option = 'xy') )
            filters.append(imagefilter.FilterParameters(filterType = 'Mean', fileEnding = 'mean', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'Median', fileEnding = 'median', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'Mode', fileEnding = 'mode', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'StdDev', fileEnding = 'stddev', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'Range', fileEnding = 'range', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'CoeffOfVar', fileEnding = 'coeffofvar', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'Min', fileEnding = 'min', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'Max', fileEnding = 'max', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'Total', fileEnding = 'total', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'Kuwahara', fileEnding = 'kuwahara', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'Lee', fileEnding = 'lee', size=3, nLooks=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'NormVar', fileEnding = 'normvar', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'NormVarSqrt', fileEnding = 'normvarsqrt', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'NormVarLn', fileEnding = 'normvarln', size=3) )
            filters.append(imagefilter.FilterParameters(filterType = 'TextureVar', fileEnding = 'texturevar', size=3) )

        imagefilter.applyfilters(inputImage, outputImageBase, filters, gdalFormat, outExt, dataType)
    
    def testLeungMalikFilterBank(self):
        inputImage = './Rasters/injune_p142_casi_sub_utm_single_band.vrt'
        outputImageBase = './TestOutputs/injune_p142_casi_sub_utm_single_band'
        gdalFormat = 'KEA'
        outExt = 'kea'
        dataType = rsgislib.TYPE_32FLOAT
        imagefilter.LeungMalikFilterBank(inputImage, outputImageBase, gdalFormat, outExt, dataType)

if __name__ == '__main__':

    t = RSGISTests()

    # Get libraries to test (defaults to all)
    testLibraries = 'all'
    
    if len(sys.argv) >= 2:
        testLibraries = sys.argv[1].lower()
        if testLibraries == 'clean':
            t.removeTestFiles()
            sys.exit()
    
    """ Check directory structure and copy Data """
    t.checkDIRStructure()
    t.copyData()
    
    if testLibraries == 'all' or testLibraries == 'imagecalc':
        """ ImageCalc functions """

        t.tryFuncAndCatch(t.testNormalise1)
        t.tryFuncAndCatch(t.testNormalise2)
        t.tryFuncAndCatch(t.testCorrelation)
        t.tryFuncAndCatch(t.testCovariance1)
        #t.tryFuncAndCatch(t.testCovariance2) # Missing matrix files.
        t.tryFuncAndCatch(t.testCalcRMSE)
        t.tryFuncAndCatch(t.testMeanVector)
        t.tryFuncAndCatch(t.testPCA)
        t.tryFuncAndCatch(t.testStandardise)
        t.tryFuncAndCatch(t.testBandMath)
        t.tryFuncAndCatch(t.testImageMaths)
        t.tryFuncAndCatch(t.testReplaceValuesLessThan)
        t.tryFuncAndCatch(t.testUnitArea)
        t.tryFuncAndCatch(t.testMovementSpeed)
        t.tryFuncAndCatch(t.testCountValsInCol)
        t.tryFuncAndCatch(t.testDist2Geoms)
        t.tryFuncAndCatch(t.testImgDist2Geoms)
        t.tryFuncAndCatch(t.testImgCalcDist)
        t.tryFuncAndCatch(t.testImageBandStats)
        t.tryFuncAndCatch(t.testImageBandStatsIgnoreZeros)
        t.tryFuncAndCatch(t.testImageStats)
        t.tryFuncAndCatch(t.testImageStatsIgnoreZeros)
        t.tryFuncAndCatch(t.testUnconLinearSpecUnmix)
        t.tryFuncAndCatch(t.testExhConLinearSpecUnmix)
        t.tryFuncAndCatch(t.testConSum1LinearSpecUnmix)
        t.tryFuncAndCatch(t.testNnConSum1LinearSpecUnmix)
        t.tryFuncAndCatch(t.testKMeansCentres)
        t.tryFuncAndCatch(t.testIsoDataClustering)
        t.tryFuncAndCatch(t.testAllBandsEqualTo)
        t.tryFuncAndCatch(t.testHistogram)
        t.tryFuncAndCatch(t.testBandPercentile)
        t.tryFuncAndCatch(t.testMahalanobisDistWindow)
        t.tryFuncAndCatch(t.testMahalanobisDistImg2Window)
        t.tryFuncAndCatch(t.testCalcPxlColStats)
        t.tryFuncAndCatch(t.testPxlColRegression)
        t.tryFuncAndCatch(t.testCorrelationWindow)
        
    if testLibraries == 'all' or testLibraries == 'imageutils':
        
        """ ImageUtils functions """
        t.tryFuncAndCatch(t.testCreateTiles)
        t.tryFuncAndCatch(t.testCreateImageMosaic)
        t.tryFuncAndCatch(t.testIncludeImages)
        t.tryFuncAndCatch(t.testPopImageStats)
        t.tryFuncAndCatch(t.testSubset)
        t.tryFuncAndCatch(t.testSubset2Polys)
        t.tryFuncAndCatch(t.testSubset2Img)
        t.tryFuncAndCatch(t.testStackBands)
        t.tryFuncAndCatch(t.testStackStats)
        t.tryFuncAndCatch(t.testCreateCopyImage)
        t.tryFuncAndCatch(t.testStretchImage)
        t.tryFuncAndCatch(t.testSetBandNames)
        
    if testLibraries == 'all' or testLibraries == 'rastergis':
    
        """ RasterGIS functions """
        t.tryFuncAndCatch(t.testCopyGDLATT)
        t.tryFuncAndCatch(t.testCopyGDLATTColumns)
        #t.tryFuncAndCatch(t.testSpatialLocation)
        #t.tryFuncAndCatch(t.testEucDistFromFeat)
        #t.tryFuncAndCatch(t.testFindTopN)
        #t.tryFuncAndCatch(t.testFindSpecClose)
        t.tryFuncAndCatch(t.testPopulateRATWithStats)
        #t.tryFuncAndCatch(t.testPopulateRATWithPercentiles)
        t.tryFuncAndCatch(t.testExport2Ascii)
        t.tryFuncAndCatch(t.testExporCols2GDALImage)
        #t.tryFuncAndCatch(t.testFindNeighbours)
        #t.tryFuncAndCatch(t.testFindBoundaryPixels)
        #t.tryFuncAndCatch(t.testCalcBorderLength)
        #t.tryFuncAndCatch(t.testCalcShapeIndices)
        
    if testLibraries == 'all' or testLibraries == 'zonalstats':
        
        """ Zonal Stats functions """
        t.tryFuncAndCatch(t.testPointValue2SHP)
        t.tryFuncAndCatch(t.testPointValue2TXT)
        t.tryFuncAndCatch(t.testPixelStats2SHP)
        t.tryFuncAndCatch(t.testPixelStats2TXT)
        t.tryFuncAndCatch(t.testPixelVals2TXT)
        t.tryFuncAndCatch(t.testImageZone2HDF)
        
    if testLibraries == 'all' or testLibraries == 'imageregistration':
        
        """ Image Registration functions """
        t.tryFuncAndCatch(t.testBasicRegistration)
        t.tryFuncAndCatch(t.testSingleLayerRegistration)
        t.tryFuncAndCatch(t.testGCP2GDAL)
        t.tryFuncAndCatch(t.testTriangularWarp)
        t.tryFuncAndCatch(t.testNNWarp)
        t.tryFuncAndCatch(t.testPolyWarp)
    
    if testLibraries == 'all' or testLibraries == 'vectorutils':
        
        """ Vector Utils functions """
        t.testPolygonsInPolygon()
        sys.exit()
        t.tryFuncAndCatch(t.testRemoveAttributes)
        t.tryFuncAndCatch(t.testBufferVector)
        t.tryFuncAndCatch(t.testPrintPolyGeom)
        t.tryFuncAndCatch(t.testFindReplaceText)
        t.tryFuncAndCatch(t.testCalcArea)
        t.tryFuncAndCatch(t.testPolygonsInPolygon)

    if testLibraries == 'all' or testLibraries == 'imagefilter':
        """ Image filter functions """ 
        t.tryFuncAndCatch(t.testFilter)
        #t.tryFuncAndCatch(t.testLeungMalikFilterBank) # Skip as it takes a while
    
    print("%s TESTS COMPLETED - %s FAILURES LISTED BELOW:"%(t.numTests, len(t.failures)))
    if(len(t.failures)):
        for failure in t.failures:
            print("FAIL: %s"%failure)

    

    




