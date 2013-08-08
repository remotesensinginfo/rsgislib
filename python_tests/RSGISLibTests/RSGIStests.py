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

import os
import rsgislib
from rsgislib import imageutils
from rsgislib import imagecalc
from rsgislib import rastergis
from rsgislib.imagecalc import BandDefn

path = os.sys.path[0] + '/'
inFileName = os.path.join(path,"Rasters","injune_p142_casi_sub_utm.kea")

class RSGISTests:
    numTests = 0
    failures = []

    def tryFuncAndCatch(self, function):
        self.numTests += 1
        try:
            function()
        except Exception as inst:
            print("ERROR - ERROR - ERROR")
            print inst
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
        os.system('cp ./RATS/injune_p142_casi_sub_utm_segs_nostats.kea ./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_nostats_addstats.kea')
    
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
        format = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        expression = "b1*b2"
        bandDefns = []
        bandDefns.append(BandDefn("b1", inFileName, 1))
        bandDefns.append(BandDefn("b2", inFileName, 2))
        imagecalc.bandMath(outputImage, expression, format, dataType, bandDefns)

    def testImageMaths(self):
        print("PYTHON TEST: Testing imageMath")
        outputImage = path + "TestOutputs/PSU142_multi1000.kea"
        format = "KEA"
        dataType = rsgislib.TYPE_32UINT
        expression = "b1*1000"
        imagecalc.imageMath(inFileName, outputImage, expression, format, dataType)

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
        format = "KEA"
        value = 0
        trueout = 0
        falseout = 1
        dataType = rsgislib.TYPE_8INT
        imagecalc.allBandsEqualTo(inputImage, value, trueout, falseout, output, format, dataType)

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
        format = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        imagecalc.mahalanobisDistFilter(image, output, window, format, dataType)

    def testMahalanobisDistImg2Window(self):
        print("PYTHON TEST: MahalanobisDistImg2Window")
        image = path + "Rasters/injune_p142_casi_sub_right_utm.kea"
        output = path + "TestOutputs/PSU142_MabDist2ImgWin.kea"
        window = 7
        format = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        imagecalc.mahalanobisDist2ImgFilter(image, output, window, format, dataType)

    def testCalcPxlColStats(self):
        print("PYTHON TEST: pxlColStats")
        image = path + "Rasters/injune_p142_casi_sub_right_utm.kea"
        output = path + "TestOutputs/PSU142_ImageStats.kea"
        format = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        s = imagecalc.StatsSummary(calcMin=True, calcMax=True, calcSum=True, calcMean=True, calcMedian=True, calcStdDev=True, min=0.0)
        imagecalc.imagePixelColumnSummary(image, output, s, format, dataType, 0, True)

    def testPxlColRegression(self):
        print("PYTHON TEST: pxlColRegression - skipping due to lack of bandvalues file.txt")

    def testCopyGDLATT(self):
        print("PYTHON TEST: copyRAT")
        table = "./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea"
        image = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_cptab.kea"
        rastergis.copyRAT(image, table)

    def testCopyGDLATTColumns(self):
        print("PYTHON TEST: copyGDALATTColumns")
        table = "./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea"
        image = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_cpcols.kea"
        fields = ["NIRAvg", "BlueAvg", "GreenAvg", "RedAvg"]
        rastergis.copyGDALATTColumns(image, table, fields)

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

    def testPopulateRATWithStats(self):
        print("PYTHON TEST: populateRATWithStats")
        clumps="./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_popstats.kea"
        input="./Rasters/injune_p142_casi_sub_utm.kea"
        bs = []
        bs.append(rastergis.BandAttStats(band=1, minField="b1Min", maxField="b1Max", meanField="b1Mean", sumField="b1Sum", stdDevField="b1StdDev"))
        bs.append(rastergis.BandAttStats(band=2, minField="b2Min", maxField="b2Max", meanField="b2Mean", sumField="b2Sum", stdDevField="b2StdDev"))
        bs.append(rastergis.BandAttStats(band=3, minField="b3Min", maxField="b3Max", meanField="b3Mean", sumField="b3Sum", stdDevField="b3StdDev"))
        rastergis.populateRATWithStats(input, clumps, bs)

    def testPopulateRATWithPercentiles(self):
        print("PYTHON TEST: populateRATWithPercentiles")
        clumps = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_popstats.kea"
        input = "./Rasters/injune_p142_casi_sub_utm.kea"
        bp = []
        bp.append(rastergis.BandAttPercentiles(band=1, percentile=25, fieldName="B1Per25"))
        bp.append(rastergis.BandAttPercentiles(band=1, percentile=50, fieldName="B1Per50"))
        bp.append(rastergis.BandAttPercentiles(band=1, percentile=75, fieldName="B1Per75"))
        rastergis.populateRATWithPercentiles(input, clumps, bp)


    def testExport2Ascii(self):
        print("PYTHON TEST: export2Ascii")
        table="./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea"
        output="./TestOutputs/RasterGIS/injune_p142_casi_rgb_exportascii.txt"
        fields = ["BlueAvg", "GreenAvg", "RedAvg"]
        rastergis.export2Ascii(table, output, fields)

    def testExporCols2GDALImage(self):
        print("PYTHON TEST: exportCols2GDALImage")
        clumps="./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea"
        output="./TestOutputs/RasterGIS/injune_p142_casi_rgb_export.kea"
        format = "KEA"
        dataType = rsgislib.TYPE_32FLOAT
        fields = ["BlueAvg", "GreenAvg", "RedAvg"]
        rastergis.exportCols2GDALImage(clumps, output, format, dataType, fields)

    def testFindNeighbours(self):
        print("PYTHON TEST: findNeighbours")
        input = "./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_neighbours.kea"
        rastergis.findNeighbours(input)

    def testFindBoundaryPixels(self):
        print("PYTHON TEST: findBoundaryPixels")
        clumps="./RATS/injune_p142_casi_sub_utm_segs.kea"
        output="./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_borpxls.kea"
        format="KEA"
        rastergis.findBoundaryPixels(clumps, output, format)

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

    def testCreateTiles(self):
        print("PYTHON TEST: createTiles")
        inputImage = path + "Rasters/injune_p142_casi_sub_utm.kea"
        outBase = './TestOutputs/Tiles/injune_p142_casi_sub_utm'
        width = 100
        height = width
        overlap = 5
        offsettiling = 0
        format = "KEA"
        dataType = rsgislib.TYPE_32INT
        ext='kea'
        imageutils.createTiles(inputImage, outBase, width, height, overlap, offsettiling, format, dataType, ext)


if __name__ == '__main__':
    t = RSGISTests()
    
    """ Copy Data """
    t.copyData()
    
    """ ImageCalc functions """
    t.tryFuncAndCatch(t.testNormalise1)
    t.tryFuncAndCatch(t.testNormalise2)
    t.tryFuncAndCatch(t.testCorrelation)
    t.tryFuncAndCatch(t.testCovariance1)
    t.tryFuncAndCatch(t.testCovariance2)
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
    
    """ ImageUtils functions """
    t.tryFuncAndCatch(t.testCreateTiles)
    
    """ RasterGIS functions """
    
    t.tryFuncAndCatch(t.testCopyGDLATT)
    t.tryFuncAndCatch(t.testCopyGDLATTColumns)
    t.tryFuncAndCatch(t.testSpatialLocation)
    t.tryFuncAndCatch(t.testEucDistFromFeat)
    t.tryFuncAndCatch(t.testFindTopN)
    t.tryFuncAndCatch(t.testFindSpecClose)
    t.tryFuncAndCatch(t.testPopulateRATWithStats)
    t.tryFuncAndCatch(t.testPopulateRATWithPercentiles)
    t.tryFuncAndCatch(t.testExport2Ascii)
    t.tryFuncAndCatch(t.testExporCols2GDALImage)
    t.tryFuncAndCatch(t.testFindNeighbours)
    t.tryFuncAndCatch(t.testFindBoundaryPixels)
    t.tryFuncAndCatch(t.testCalcBorderLength)
    t.tryFuncAndCatch(t.testCalcShapeIndices)
    
    print("%s TESTS COMPLETED - %s FAILURES LISTED BELOW:"%(t.numTests, len(t.failures)))
    if(len(t.failures)):
        for failure in t.failures:
            print("FAIL: %s"%failure)

    

    




