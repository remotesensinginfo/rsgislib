
###############################################################################
# set file locations
set(CMPFIT_SRC_DIR math/cmpfit)
set(RSGIS_SRC_CALIBRATION_DIR calibration)
set(RSGIS_SRC_CLASSIFY_DIR classifier)
set(RSGIS_SRC_COMMON_DIR common)
set(RSGIS_SRC_DATASTRUCT_DIR datastruct)
set(RSGIS_SRC_FILTERING_DIR filtering)
set(RSGIS_SRC_GEOM_DIR geom)
set(RSGIS_SRC_IMG_DIR img)
set(RSGIS_SRC_MATH_DIR math)
set(RSGIS_SRC_MODELING_DIR modeling)
set(RSGIS_SRC_RADAR_DIR radar)
set(RSGIS_SRC_RASTERGIS_DIR rastergis)
set(RSGIS_SRC_REGISTRATION_DIR registration)
set(RSGIS_SRC_SEGMENTATION_DIR segmentation)
set(RSGIS_SRC_UTILS_DIR utils)
set(RSGIS_SRC_VEC_DIR vec)
set(RSGIS_SRC_CMDS_DIR cmds)
set(RSGIS_SRC_EXE_DIR exe/rsgis)
###############################################################################

###############################################################################
# Files within the cmpfit Library.
set(LIB_CMPFIT_H
	${CMPFIT_SRC_DIR}/rsgis_mpfit.h
	)
	
set(LIB_CMPFIT_CPP
	${CMPFIT_SRC_DIR}/rsgis_mpfit.c
	${CMPFIT_SRC_DIR}/rsgis_mpfit.h
	)
###############################################################################


###############################################################################
# Files within the calibration Library.
set(LIB_CALIBRATION_H
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISApply6SCoefficients.h
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISCalculateTopOfAtmosphereReflectance.h 
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISDEMTools.h
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISStandardDN2RadianceCalibration.h
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISApplySubtractOffsets.h
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISCloudMasking.h
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISHydroDEMFillSoilleGratin94.h
	)
	
set(LIB_CALIBRATION_CPP
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISApply6SCoefficients.cpp
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISApply6SCoefficients.h 
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISCalculateTopOfAtmosphereReflectance.cpp 
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISCalculateTopOfAtmosphereReflectance.h
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISDEMTools.cpp 
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISDEMTools.h
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISStandardDN2RadianceCalibration.cpp 
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISStandardDN2RadianceCalibration.h
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISApplySubtractOffsets.cpp 
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISApplySubtractOffsets.h
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISCloudMasking.cpp
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISCloudMasking.h
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISHydroDEMFillSoilleGratin94.cpp
	${RSGIS_SRC_CALIBRATION_DIR}/RSGISHydroDEMFillSoilleGratin94.h
	)
###############################################################################

###############################################################################
# Files within the classifier Library.
set(LIB_CLASSIFY_H
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISClassifier.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISClassificationUtils.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISMinimumDistanceClassifier.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISNearestNeighbourClassifier.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISSpectralAngleMapper.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISSpectralCorrelationMapper.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISCumulativeAreaClassifier.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISKMeanImageClassifier.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISISODATAImageClassifier.h
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISRATClassificationUtils.h
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISGenAccuracyPoints.h
	)
	
set(LIB_CLASSIFY_CPP
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISClassifier.cpp 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISClassifier.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISClassificationUtils.cpp 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISClassificationUtils.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISMinimumDistanceClassifier.cpp 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISMinimumDistanceClassifier.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISNearestNeighbourClassifier.cpp 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISNearestNeighbourClassifier.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISSpectralAngleMapper.cpp 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISSpectralAngleMapper.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISSpectralCorrelationMapper.cpp 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISSpectralCorrelationMapper.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISCumulativeAreaClassifier.cpp 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISCumulativeAreaClassifier.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISKMeanImageClassifier.cpp 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISKMeanImageClassifier.h 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISISODATAImageClassifier.cpp 
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISISODATAImageClassifier.h
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISRATClassificationUtils.cpp
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISRATClassificationUtils.h
    ${RSGIS_SRC_CLASSIFY_DIR}/RSGISGenAccuracyPoints.cpp
	${RSGIS_SRC_CLASSIFY_DIR}/RSGISGenAccuracyPoints.h
	)
###############################################################################

###############################################################################
# Files within the commands Library.
set(LIB_COMMANDS_H
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdException.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdParent.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdCommon.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdClassification.h
    ${RSGIS_SRC_CMDS_DIR}/RSGISCmdFilterImages.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdImageCalc.h 
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdImageCalibration.h
    ${RSGIS_SRC_CMDS_DIR}/RSGISCmdImageRegistration.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdImageUtils.h 	
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdSegmentation.h 
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdRasterGIS.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdVectorUtils.h
    ${RSGIS_SRC_CMDS_DIR}/RSGISCmdZonalStats.h
	)
	
set(LIB_COMMANDS_CPP
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdException.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdException.cpp
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdParent.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdCommon.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdClassification.cpp
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdClassification.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdElevationTools.cpp
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdElevationTools.h
    ${RSGIS_SRC_CMDS_DIR}/RSGISCmdFilterImages.cpp
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdFilterImages.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdImageCalc.cpp 
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdImageCalc.h 
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdImageCalibration.cpp 
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdImageCalibration.h
    ${RSGIS_SRC_CMDS_DIR}/RSGISCmdImageRegistration.cpp
    ${RSGIS_SRC_CMDS_DIR}/RSGISCmdImageRegistration.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdImageUtils.cpp	
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdImageUtils.h 
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdSegmentation.cpp
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdSegmentation.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdRasterGIS.cpp
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdRasterGIS.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdVectorUtils.cpp
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdVectorUtils.h
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdZonalStats.cpp
	${RSGIS_SRC_CMDS_DIR}/RSGISCmdZonalStats.h
	)
###############################################################################

###############################################################################
# Files within the common Library.
set(LIB_COMMON_H
	${RSGIS_SRC_COMMON_DIR}/RSGISCommons.h
	${RSGIS_SRC_COMMON_DIR}/RSGISException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISFileException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISImageException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISClassificationException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISOutputStreamException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISInputStreamException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISVectorException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISCommandLineException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISXMLArgumentsException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISLiDARException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISRegistrationException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISCommandLineParser.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISAlgorithmParameters.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISAlgorParamsFactory.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISParseXMLArguments.h
	${RSGIS_SRC_COMMON_DIR}/RSGISAttributeTableException.h
	${RSGIS_SRC_COMMON_DIR}/rsgis-config.h
	)
	
set(LIB_COMMON_CPP
	${RSGIS_SRC_COMMON_DIR}/RSGISCommons.h
	${RSGIS_SRC_COMMON_DIR}/RSGISException.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISFileException.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISFileException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISImageException.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISImageException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISClassificationException.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISClassificationException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISOutputStreamException.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISOutputStreamException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISInputStreamException.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISInputStreamException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISVectorException.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISVectorException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISCommandLineException.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISCommandLineException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISXMLArgumentsException.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISXMLArgumentsException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISLiDARException.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISLiDARException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISRegistrationException.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISRegistrationException.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISCommandLineParser.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISCommandLineParser.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISAlgorithmParameters.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISAlgorithmParameters.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISAlgorParamsFactory.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISAlgorParamsFactory.h 
	${RSGIS_SRC_COMMON_DIR}/RSGISParseXMLArguments.cpp 
	${RSGIS_SRC_COMMON_DIR}/RSGISParseXMLArguments.h
	${RSGIS_SRC_COMMON_DIR}/RSGISAttributeTableException.cpp
	${RSGIS_SRC_COMMON_DIR}/RSGISAttributeTableException.h
	${RSGIS_SRC_COMMON_DIR}/rsgis-config.h
	)
###############################################################################

###############################################################################
# Files within the data structure Library.
set(LIB_DATASTRUCT_H
	${RSGIS_SRC_DATASTRUCT_DIR}/RSGISListException.h 
	${RSGIS_SRC_DATASTRUCT_DIR}/RSGISItemNotFoundException.h 
	${RSGIS_SRC_DATASTRUCT_DIR}/SortedGenericList.cpp
	)
	
set(LIB_DATASTRUCT_CPP
	${RSGIS_SRC_DATASTRUCT_DIR}/RSGISListException.cpp 
	${RSGIS_SRC_DATASTRUCT_DIR}/RSGISListException.h 
	${RSGIS_SRC_DATASTRUCT_DIR}/RSGISItemNotFoundException.cpp 
	${RSGIS_SRC_DATASTRUCT_DIR}/RSGISItemNotFoundException.h 
	${RSGIS_SRC_DATASTRUCT_DIR}/SortedGenericList.cpp
	)
###############################################################################

###############################################################################
# Files within the filtering Library.
set(LIB_FILTERING_H
	${RSGIS_SRC_FILTERING_DIR}/RSGISCalcFilter.h  
	${RSGIS_SRC_FILTERING_DIR}/RSGISGenerateFilter.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISCalcImageFilters.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISImageFilter.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISFilterBank.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISImageKernelFilter.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISStatsFilters.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISPrewittFilter.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISSobelFilter.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyDilate.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyErode.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyGradient.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyFindExtrema.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyClosing.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyOpening.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyTopHat.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISSpeckleFilters.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISSARTextureFilters.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISNonLocalDenoising.h
	)
	
set(LIB_FILTERING_CPP
	${RSGIS_SRC_FILTERING_DIR}/RSGISCalcFilter.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISCalcImageFilters.cpp
	${RSGIS_SRC_FILTERING_DIR}/RSGISCalcImageFilters.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISFilterBank.cpp 
	${RSGIS_SRC_FILTERING_DIR}/RSGISFilterBank.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISGenerateFilter.cpp 
	${RSGIS_SRC_FILTERING_DIR}/RSGISGenerateFilter.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISImageFilter.cpp 
	${RSGIS_SRC_FILTERING_DIR}/RSGISImageFilter.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISImageFilterException.cpp 
	${RSGIS_SRC_FILTERING_DIR}/RSGISImageFilterException.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISImageKernelFilter.cpp 
	${RSGIS_SRC_FILTERING_DIR}/RSGISImageKernelFilter.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISPrewittFilter.cpp 
	${RSGIS_SRC_FILTERING_DIR}/RSGISPrewittFilter.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISSobelFilter.cpp 
	${RSGIS_SRC_FILTERING_DIR}/RSGISSobelFilter.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISStatsFilters.cpp 
	${RSGIS_SRC_FILTERING_DIR}/RSGISStatsFilters.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISSpeckleFilters.cpp
	${RSGIS_SRC_FILTERING_DIR}/RSGISSpeckleFilters.h
    ${RSGIS_SRC_FILTERING_DIR}/RSGISSARTextureFilters.cpp
	${RSGIS_SRC_FILTERING_DIR}/RSGISSARTextureFilters.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyDilate.cpp 
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyDilate.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyErode.cpp 
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyErode.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyGradient.cpp
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyGradient.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyFindExtrema.cpp
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyFindExtrema.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyClosing.cpp
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyClosing.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyOpening.cpp
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyOpening.h
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyTopHat.cpp
	${RSGIS_SRC_FILTERING_DIR}/RSGISMorphologyTopHat.h 
	${RSGIS_SRC_FILTERING_DIR}/RSGISNonLocalDenoising.cpp
	${RSGIS_SRC_FILTERING_DIR}/RSGISNonLocalDenoising.h
	)

###############################################################################

###############################################################################
# Files within the Geometry Library.
set(LIB_GEOM_H
	${RSGIS_SRC_GEOM_DIR}/RSGISGeometryException.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISAbstractOutlinePolygon.h 
	${RSGIS_SRC_GEOM_DIR}/RSGIS2DPoint.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISPolygon.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISCircle.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISTriangle.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISGeomTestExport.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISGeometry.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISDelaunayTriangulation.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISIdentifyNonConvexPolygons.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISIdentifyNonConvexPolygonsDelaunay.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISIdentifyNonConvexPolygonsLineProject.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISSpatialClustererInterface.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISMinSpanTreeClustererEdgeLenThreshold.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISMinSpanTreeClustererStdDevThreshold.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISIdentifyNonConvexPolygonsSnakes.h
	${RSGIS_SRC_GEOM_DIR}/RSGISFitPolygon2Points.h
	)
	
set(LIB_GEOM_CPP
	${RSGIS_SRC_GEOM_DIR}/RSGISGeometryException.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGISGeometryException.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISAbstractOutlinePolygon.h  
	${RSGIS_SRC_GEOM_DIR}/RSGIS2DPoint.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGIS2DPoint.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISPolygon.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGISPolygon.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISCircle.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGISCircle.h  
	${RSGIS_SRC_GEOM_DIR}/RSGISTriangle.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGISTriangle.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISGeomTestExport.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGISGeomTestExport.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISGeometry.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGISGeometry.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISDelaunayTriangulation.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGISDelaunayTriangulation.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISIdentifyNonConvexPolygons.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISIdentifyNonConvexPolygonsDelaunay.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGISIdentifyNonConvexPolygonsDelaunay.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISIdentifyNonConvexPolygonsLineProject.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGISIdentifyNonConvexPolygonsLineProject.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISSpatialClustererInterface.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISMinSpanTreeClustererEdgeLenThreshold.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGISMinSpanTreeClustererEdgeLenThreshold.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISMinSpanTreeClustererStdDevThreshold.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGISMinSpanTreeClustererStdDevThreshold.h 
	${RSGIS_SRC_GEOM_DIR}/RSGISIdentifyNonConvexPolygonsSnakes.cpp 
	${RSGIS_SRC_GEOM_DIR}/RSGISIdentifyNonConvexPolygonsSnakes.h
	${RSGIS_SRC_GEOM_DIR}/RSGISFitPolygon2Points.cpp
	${RSGIS_SRC_GEOM_DIR}/RSGISFitPolygon2Points.h
	)
###############################################################################

###############################################################################
# Files within the Imaging Library.
set(LIB_IMG_H
	${RSGIS_SRC_IMG_DIR}/RSGISImageBandException.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageCalcException.h 
	${RSGIS_SRC_IMG_DIR}/RSGISParseColourException.h 
	${RSGIS_SRC_IMG_DIR}/RSGISFFTException.h 
	${RSGIS_SRC_IMG_DIR}/RSGISProjectionStrings.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImageValue.h  
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImageSingleValue.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageUtils.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImageSingle.h 
	${RSGIS_SRC_IMG_DIR}/RSGISDarkTargetIdentification.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageInterpolator.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageInterpolation.h 
	${RSGIS_SRC_IMG_DIR}/RSGISAddBands.h 
	${RSGIS_SRC_IMG_DIR}/RSGISAddNoise.h 
	${RSGIS_SRC_IMG_DIR}/RSGISApplyEigenvectors.h 
	${RSGIS_SRC_IMG_DIR}/RSGISBandMath.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcCorrelationCoefficient.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcCovariance.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageMosaic.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageStatistics.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageNormalisation.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImageMatrix.h 
	${RSGIS_SRC_IMG_DIR}/RSGISMaskImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISMeanVector.h 
	${RSGIS_SRC_IMG_DIR}/RSGISPolygonPixelCount.h 
	${RSGIS_SRC_IMG_DIR}/RSGISPopulateMatrix.h 
	${RSGIS_SRC_IMG_DIR}/RSGISColourUpImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCopyImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCopyImageBands.h 
	${RSGIS_SRC_IMG_DIR}/RSGISStandardiseImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISFFTProcessing.h  
	${RSGIS_SRC_IMG_DIR}/RSGISImageCalcValueBaysianNoPrior.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageCalcValueBaysianPrior.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCreateTestImages.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageBand2Matrix.h 
	${RSGIS_SRC_IMG_DIR}/RSGISDefiniensCSVRasterise.h 
	${RSGIS_SRC_IMG_DIR}/RSGISExport2DScatterPTxt.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCreatePixelPolygons.h 
	${RSGIS_SRC_IMG_DIR}/RSGISSavitzkyGolaySmoothingFilters.h 
	${RSGIS_SRC_IMG_DIR}/RSGISConvertSpectralToUnitArea.h 
	${RSGIS_SRC_IMG_DIR}/RSGISReplaceValuesLessThanGivenValue.h 
	${RSGIS_SRC_IMG_DIR}/RSGISPixelInPoly.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageMaths.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCumulativeArea.h 
	${RSGIS_SRC_IMG_DIR}/RSGISStretchImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISColourUsingHue.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalculateImageMovementSpeed.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCountValsAboveThresInCol.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcRMSE.h 
	${RSGIS_SRC_IMG_DIR}/RSGISApplyFunction.h 
	${RSGIS_SRC_IMG_DIR}/RSGISGenerateImageFromXYZData.h 
	${RSGIS_SRC_IMG_DIR}/RSGISExtractImagePixelsInPolygon.h 
	${RSGIS_SRC_IMG_DIR}/RSGISPanSharpen.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImgAlongsideOut.h 
	${RSGIS_SRC_IMG_DIR}/RSGISClumpImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISLinearSpectralUnmixing.h
	${RSGIS_SRC_IMG_DIR}/RSGISImageClustering.h
	${RSGIS_SRC_IMG_DIR}/RSGISImageComposite.h
	${RSGIS_SRC_IMG_DIR}/RSGISRelabelPixelValuesFromLUT.h
	${RSGIS_SRC_IMG_DIR}/RSGISPopWithStats.h
	${RSGIS_SRC_IMG_DIR}/RSGISGenHistogram.h
	${RSGIS_SRC_IMG_DIR}/RSGISSampleImage.h
	${RSGIS_SRC_IMG_DIR}/RSGISImageWindowStats.h
	${RSGIS_SRC_IMG_DIR}/RSGISFitFunction2Pxls.h
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImgValProb.h
	)
	
set(LIB_IMG_CPP
	${RSGIS_SRC_IMG_DIR}/RSGISAddBands.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISAddBands.h 
	${RSGIS_SRC_IMG_DIR}/RSGISAddNoise.cpp 	
	${RSGIS_SRC_IMG_DIR}/RSGISAddNoise.h 
	${RSGIS_SRC_IMG_DIR}/RSGISApplyEigenvectors.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISApplyEigenvectors.h 
	${RSGIS_SRC_IMG_DIR}/RSGISBandMath.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISBandMath.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcCorrelationCoefficient.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcCorrelationCoefficient.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcCovariance.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcCovariance.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImage.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcEditImage.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcEditImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImageMatrix.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImageMatrix.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImageSingle.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImageSingle.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImageSingleValue.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImageSingleValue.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImageValue.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImageValue.h 
	${RSGIS_SRC_IMG_DIR}/RSGISColourUpImage.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISColourUpImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCopyImage.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCopyImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCopyImageBands.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCopyImageBands.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCreateTestImages.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCreateTestImages.h 
	${RSGIS_SRC_IMG_DIR}/RSGISDarkTargetIdentification.h
	${RSGIS_SRC_IMG_DIR}/RSGISDarkTargetIdentification.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISDefiniensCSVRasterise.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISDefiniensCSVRasterise.h 
	${RSGIS_SRC_IMG_DIR}/RSGISExport2DScatterPTxt.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISExport2DScatterPTxt.h 
	${RSGIS_SRC_IMG_DIR}/RSGISExtractImageValues.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISExtractImageValues.h
	${RSGIS_SRC_IMG_DIR}/RSGISFFTException.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISFFTException.h
	${RSGIS_SRC_IMG_DIR}/RSGISFFTProcessing.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISFFTProcessing.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageBand2Matrix.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISImageBand2Matrix.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageBandException.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISImageBandException.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageCalcException.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISImageCalcException.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageCalcValueBaysianNoPrior.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISImageCalcValueBaysianNoPrior.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageCalcValueBaysianPrior.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISImageCalcValueBaysianPrior.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageInterpolation.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISImageInterpolation.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageInterpolator.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISImageInterpolator.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageMosaic.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISImageMosaic.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageNormalisation.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISImageNormalisation.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageStatistics.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISImageStatistics.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageUtils.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISImageUtils.h 
	${RSGIS_SRC_IMG_DIR}/RSGISMaskImage.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISMaskImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISMeanVector.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISMeanVector.h 
	${RSGIS_SRC_IMG_DIR}/RSGISParseColourException.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISParseColourException.h 
	${RSGIS_SRC_IMG_DIR}/RSGISPolygonPixelCount.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISPolygonPixelCount.h 
	${RSGIS_SRC_IMG_DIR}/RSGISPopulateMatrix.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISPopulateMatrix.h  
	${RSGIS_SRC_IMG_DIR}/RSGISProjectionStrings.h
	${RSGIS_SRC_IMG_DIR}/RSGISStandardiseImage.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISStandardiseImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCreatePixelPolygons.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISCreatePixelPolygons.h 
	${RSGIS_SRC_IMG_DIR}/RSGISSavitzkyGolaySmoothingFilters.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISSavitzkyGolaySmoothingFilters.h 
	${RSGIS_SRC_IMG_DIR}/RSGISConvertSpectralToUnitArea.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISConvertSpectralToUnitArea.h 
	${RSGIS_SRC_IMG_DIR}/RSGISReplaceValuesLessThanGivenValue.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISReplaceValuesLessThanGivenValue.h 
	${RSGIS_SRC_IMG_DIR}/RSGISPixelInPoly.h 
	${RSGIS_SRC_IMG_DIR}/RSGISPixelInPoly.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISImageMaths.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISImageMaths.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCumulativeArea.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCumulativeArea.h 
	${RSGIS_SRC_IMG_DIR}/RSGISStretchImage.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISStretchImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISColourUsingHue.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISColourUsingHue.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalculateImageMovementSpeed.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCalculateImageMovementSpeed.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCountValsAboveThresInCol.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCountValsAboveThresInCol.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcRMSE.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcRMSE.h 
	${RSGIS_SRC_IMG_DIR}/RSGISApplyFunction.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISApplyFunction.h 
	${RSGIS_SRC_IMG_DIR}/RSGISGenerateImageFromXYZData.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISGenerateImageFromXYZData.h 
	${RSGIS_SRC_IMG_DIR}/RSGISExtractImagePixelsInPolygon.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISExtractImagePixelsInPolygon.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcDist2Geom.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcDist2Geom.h 
	${RSGIS_SRC_IMG_DIR}/RSGISPanSharpen.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISPanSharpen.h 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImgAlongsideOut.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImgAlongsideOut.h 
	${RSGIS_SRC_IMG_DIR}/RSGISClumpImage.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISClumpImage.h 
	${RSGIS_SRC_IMG_DIR}/RSGISLinearSpectralUnmixing.cpp 
	${RSGIS_SRC_IMG_DIR}/RSGISLinearSpectralUnmixing.h 
	${RSGIS_SRC_IMG_DIR}/RSGISImageClustering.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISImageClustering.h
    ${RSGIS_SRC_IMG_DIR}/RSGISImageComposite.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISImageComposite.h
	${RSGIS_SRC_IMG_DIR}/RSGISRelabelPixelValuesFromLUT.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISRelabelPixelValuesFromLUT.h
	${RSGIS_SRC_IMG_DIR}/RSGISPopWithStats.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISPopWithStats.h
	${RSGIS_SRC_IMG_DIR}/RSGISGenHistogram.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISGenHistogram.h
	${RSGIS_SRC_IMG_DIR}/RSGISSampleImage.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISSampleImage.h
	${RSGIS_SRC_IMG_DIR}/RSGISImageWindowStats.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISImageWindowStats.h
	${RSGIS_SRC_IMG_DIR}/RSGISFitFunction2Pxls.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISFitFunction2Pxls.h
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImgValProb.cpp
	${RSGIS_SRC_IMG_DIR}/RSGISCalcImgValProb.h
	)
###############################################################################

###############################################################################
# Files within the maths Library.
set(LIB_MATH_H
	${RSGIS_SRC_MATH_DIR}/RSGISMathException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMatricesException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMultivariantStatsException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianStatsException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISVectorsException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISClustererException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISOptimisationException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianDeltaType.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMathsUtils.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMathFunction.h 
	${RSGIS_SRC_MATH_DIR}/RSGISIntergration.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMatrices.h 
	${RSGIS_SRC_MATH_DIR}/RSGISVectors.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMultivariantStats.h 
	${RSGIS_SRC_MATH_DIR}/RSGISPrincipalComponentAnalysis.h 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianIntergrateFunctionNoPrior.h 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianIntergrateFunctionPrior.h 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianStatsNoPrior.h 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianStatsPrior.h 
	${RSGIS_SRC_MATH_DIR}/RSGISProbabilityDistributions.h 
	${RSGIS_SRC_MATH_DIR}/RSGISFFTWUtils.h 
	${RSGIS_SRC_MATH_DIR}/RSGISSingularValueDecomposition.h 
	${RSGIS_SRC_MATH_DIR}/RSGISPolyFit.h 
	${RSGIS_SRC_MATH_DIR}/RSGISOptimisationFunction.h 
	${RSGIS_SRC_MATH_DIR}/RSGISGlobalOptimisationFunction.h
	${RSGIS_SRC_MATH_DIR}/RSGISOptimiser2D.h 
	${RSGIS_SRC_MATH_DIR}/RSGISGlobalOptimiser2D.h 
	${RSGIS_SRC_MATH_DIR}/RSGISGlobalHillClimbingOptimiser2D.h 
	${RSGIS_SRC_MATH_DIR}/RSGISProbDistro.h 
	${RSGIS_SRC_MATH_DIR}/RSGISRandomDistro.h 
	${RSGIS_SRC_MATH_DIR}/RSGISFunctions.h 
	${RSGIS_SRC_MATH_DIR}/RSGISLinearFit.h 
	${RSGIS_SRC_MATH_DIR}/RSGISNonLinearFit.h
	${RSGIS_SRC_MATH_DIR}/RSGISClustering.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMaximumLikelihood.h
	${RSGIS_SRC_MATH_DIR}/RSGISMaximumLikelihoodException.h
	${RSGIS_SRC_MATH_DIR}/RSGIS2DInterpolation.h
	${RSGIS_SRC_MATH_DIR}/RSGISLogicExpEvaluation.h
	${RSGIS_SRC_MATH_DIR}/RSGISDistMetrics.h
	${RSGIS_SRC_MATH_DIR}/RSGISFitGaussianMixModel.h
	)
	
set(LIB_MATH_CPP
	${RSGIS_SRC_MATH_DIR}/RSGISMathException.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISMathException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMatricesException.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISMatricesException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMultivariantStatsException.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISMultivariantStatsException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianStatsException.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianStatsException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISVectorsException.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISVectorsException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISClustererException.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISClustererException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISOptimisationException.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISOptimisationException.h 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianDeltaType.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMathsUtils.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISMathsUtils.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMathFunction.h 
	${RSGIS_SRC_MATH_DIR}/RSGISIntergration.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISIntergration.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMatrices.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISMatrices.h 
	${RSGIS_SRC_MATH_DIR}/RSGISVectors.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISVectors.h 
	${RSGIS_SRC_MATH_DIR}/RSGISMultivariantStats.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISMultivariantStats.h 
	${RSGIS_SRC_MATH_DIR}/RSGISPrincipalComponentAnalysis.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISPrincipalComponentAnalysis.h 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianIntergrateFunctionNoPrior.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianIntergrateFunctionNoPrior.h 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianIntergrateFunctionPrior.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianIntergrateFunctionPrior.h 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianStatsNoPrior.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianStatsNoPrior.h 
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianStatsPrior.cpp  
	${RSGIS_SRC_MATH_DIR}/RSGISBaysianStatsPrior.h 
	${RSGIS_SRC_MATH_DIR}/RSGISProbabilityDistributions.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISProbabilityDistributions.h 
	${RSGIS_SRC_MATH_DIR}/RSGISFFTWUtils.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISFFTWUtils.h 
	${RSGIS_SRC_MATH_DIR}/RSGISSingularValueDecomposition.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISSingularValueDecomposition.h 
	${RSGIS_SRC_MATH_DIR}/RSGISPolyFit.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISPolyFit.h 
	${RSGIS_SRC_MATH_DIR}/RSGISOptimisationFunction.h 
	${RSGIS_SRC_MATH_DIR}/RSGISGlobalOptimisationFunction.h 
	${RSGIS_SRC_MATH_DIR}/RSGISOptimiser2D.h 
	${RSGIS_SRC_MATH_DIR}/RSGISGlobalOptimiser2D.h 
	${RSGIS_SRC_MATH_DIR}/RSGISGlobalHillClimbingOptimiser2D.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISGlobalHillClimbingOptimiser2D.h 
	${RSGIS_SRC_MATH_DIR}/RSGISRandomDistro.h 
	${RSGIS_SRC_MATH_DIR}/RSGISRandomDistro.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISProbDistro.h 
	${RSGIS_SRC_MATH_DIR}/RSGISFunctions.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISFunctions.h
	${RSGIS_SRC_MATH_DIR}/RSGISLinearFit.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISLinearFit.h 
	${RSGIS_SRC_MATH_DIR}/RSGISNonLinearFit.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISNonLinearFit.h
	${RSGIS_SRC_MATH_DIR}/RSGISClustering.cpp
	${RSGIS_SRC_MATH_DIR}/RSGISClustering.h
	${RSGIS_SRC_MATH_DIR}/RSGISnnls.cpp 
	${RSGIS_SRC_MATH_DIR}/RSGISnnls.h
	${RSGIS_SRC_MATH_DIR}/RSGISMaximumLikelihood.cpp
	${RSGIS_SRC_MATH_DIR}/RSGISMaximumLikelihood.h
	${RSGIS_SRC_MATH_DIR}/RSGISMaximumLikelihoodException.cpp
	${RSGIS_SRC_MATH_DIR}/RSGISMaximumLikelihoodException.h
	${RSGIS_SRC_MATH_DIR}/RSGIS2DInterpolation.cpp
	${RSGIS_SRC_MATH_DIR}/RSGIS2DInterpolation.h
	${RSGIS_SRC_MATH_DIR}/RSGISLogicExpEvaluation.cpp
	${RSGIS_SRC_MATH_DIR}/RSGISLogicExpEvaluation.h
	${RSGIS_SRC_MATH_DIR}/RSGISDistMetrics.cpp
	${RSGIS_SRC_MATH_DIR}/RSGISDistMetrics.h
	${RSGIS_SRC_MATH_DIR}/RSGISFitGaussianMixModel.cpp
	${RSGIS_SRC_MATH_DIR}/RSGISFitGaussianMixModel.h
	)
###############################################################################

###############################################################################
# Files within the modeling Library.
set(LIB_MODELING_H
	${RSGIS_SRC_MODELING_DIR}/RSGISTransect.h 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelTransect.h 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelAddVegetation.h 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelTree.h 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelTreeCanopy.h  
	${RSGIS_SRC_MODELING_DIR}/RSGISCalcFPC.h  
	${RSGIS_SRC_MODELING_DIR}/RSGISModelingException.h 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelTreeParameters.h 
	${RSGIS_SRC_MODELING_DIR}/RSGISCalcCanopyCover.h
	)
	
set(LIB_MODELING_CPP
	${RSGIS_SRC_MODELING_DIR}/RSGISTransect.h 
	${RSGIS_SRC_MODELING_DIR}/RSGISTransect.cpp 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelTransect.cpp 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelTransect.h 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelAddVegetation.h 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelTree.cpp 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelTree.h 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelTreeCanopy.cpp 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelTreeCanopy.h  
	${RSGIS_SRC_MODELING_DIR}/RSGISCalcFPC.cpp 
	${RSGIS_SRC_MODELING_DIR}/RSGISCalcFPC.h  
	${RSGIS_SRC_MODELING_DIR}/RSGISModelingException.cpp 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelingException.h 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelTreeParameters.cpp 
	${RSGIS_SRC_MODELING_DIR}/RSGISModelTreeParameters.h 
	${RSGIS_SRC_MODELING_DIR}/RSGISCalcCanopyCover.cpp 
	${RSGIS_SRC_MODELING_DIR}/RSGISCalcCanopyCover.h
	)
###############################################################################

###############################################################################
# Files within the radar Library.
set(LIB_RADAR_H
	${RSGIS_SRC_RADAR_DIR}/RSGISCalcGammaZero.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISRadarUtils.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISSoilDielectricMixingModel.h 
    ${RSGIS_SRC_RADAR_DIR}/RSGISCalcSaatchiBiomass.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISDefaultSplitBiomassFunction.h
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationConjugateGradient.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationParameters.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationAlgorithm.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationOptimiser.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationSimulatedAnnealing.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationFPC.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationExhaustiveSearch.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationGSLOptimiser.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationLinearLeastSquares.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationThresholdAccepting.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISObjectBasedEstimation.h
	)
	
set(LIB_RADAR_UTILS_CPP
	${RSGIS_SRC_RADAR_DIR}/RSGISCalcGammaZero.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISCalcGammaZero.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISRadarUtils.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISRadarUtils.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISSoilDielectricMixingModel.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISSoilDielectricMixingModel.h
	)

set(LIB_RADAR_BIOMASS_CPP
	${RSGIS_SRC_RADAR_DIR}/RSGISCalcSaatchiBiomass.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISCalcSaatchiBiomass.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISDefaultSplitBiomassFunction.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISDefaultSplitBiomassFunction.h
	)
	
set(LIB_RADAR_INVERSION_CPP
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationConjugateGradient.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationConjugateGradient.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationParameters.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationAlgorithm.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationAlgorithm.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationOptimiser.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationSimulatedAnnealing.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationSimulatedAnnealing.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationFPC.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationFPC.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationExhaustiveSearch.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationExhaustiveSearch.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationGSLOptimiser.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationGSLOptimiser.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationLinearLeastSquares.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationLinearLeastSquares.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationThresholdAccepting.cpp 
	${RSGIS_SRC_RADAR_DIR}/RSGISEstimationThresholdAccepting.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISObjectBasedEstimation.h 
	${RSGIS_SRC_RADAR_DIR}/RSGISObjectBasedEstimation.cpp
	)

###############################################################################

###############################################################################
# Files within the Raster GIS Library.
set(LIB_RASTERGIS_H
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRasterAttUtils.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISExportColumns2Image.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISPopRATWithStats.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISCalcImageStatsAndPyramids.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISCalcClusterLocation.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISDefineClumpsInTiles.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISFindClumpNeighbours.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISFindClumpCatagoryStats.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISSelectClumps.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATCalcValue.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATCalc.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISFindChangeClumps.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISFindInfoBetweenLayers.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISClumpBorders.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISInterpolateClumpValues2Image.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISCalcNeighbourStats.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISClumpRegionGrowing.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISBinaryClassifyClumps.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISCollapseRAT.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISInputShapefileAttributes2RAT.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATKNN.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATFunctionFitting.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATStats.h
	)
	
	
set(LIB_RASTERGIS_CPP
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRasterAttUtils.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRasterAttUtils.h
    ${RSGIS_SRC_RASTERGIS_DIR}/RSGISExportColumns2Image.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISExportColumns2Image.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISPopRATWithStats.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISPopRATWithStats.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISCalcImageStatsAndPyramids.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISCalcImageStatsAndPyramids.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISCalcClusterLocation.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISCalcClusterLocation.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISDefineClumpsInTiles.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISDefineClumpsInTiles.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISExportClumps2Imgs.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISExportClumps2Imgs.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISFindClumpNeighbours.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISFindClumpNeighbours.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISFindClumpCatagoryStats.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISFindClumpCatagoryStats.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISSelectClumps.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISSelectClumps.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATCalcValue.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATCalc.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATCalc.h
    ${RSGIS_SRC_RASTERGIS_DIR}/RSGISFindChangeClumps.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISFindChangeClumps.h
    ${RSGIS_SRC_RASTERGIS_DIR}/RSGISFindInfoBetweenLayers.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISFindInfoBetweenLayers.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISClumpBorders.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISClumpBorders.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISInterpolateClumpValues2Image.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISInterpolateClumpValues2Image.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISCalcNeighbourStats.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISCalcNeighbourStats.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISClumpRegionGrowing.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISClumpRegionGrowing.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISBinaryClassifyClumps.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISBinaryClassifyClumps.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISCollapseRAT.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISCollapseRAT.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISInputShapefileAttributes2RAT.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISInputShapefileAttributes2RAT.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATKNN.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATKNN.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATFunctionFitting.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATFunctionFitting.cpp
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATStats.h
	${RSGIS_SRC_RASTERGIS_DIR}/RSGISRATStats.cpp
	)
	
###############################################################################

###############################################################################
# Files within the registration Library.
set(LIB_REGISTRATION_H
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISImageWarpException.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISImageSimilarityMetric.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISImageRegistration.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISBasicImageRegistration.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISStandardImageSimilarityMetrics.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISSingleConnectLayerImageRegistration.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISWarpImageInterpolator.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISGCPImg2MapNode.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISWarpImage.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISWarpImageUsingTriangulation.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISBasicNNGCPImageWarp.h
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISPolynomialImageWarp.h
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISImagePixelRegistration.h
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISAddGCPsGDAL.h
	)
	
set(LIB_REGISTRATION_CPP
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISImageWarpException.cpp 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISImageWarpException.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISImageSimilarityMetric.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISImageRegistration.cpp 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISImageRegistration.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISBasicImageRegistration.cpp 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISBasicImageRegistration.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISStandardImageSimilarityMetrics.cpp 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISStandardImageSimilarityMetrics.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISSingleConnectLayerImageRegistration.cpp 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISSingleConnectLayerImageRegistration.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISWarpImageInterpolator.cpp 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISWarpImageInterpolator.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISGCPImg2MapNode.cpp 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISGCPImg2MapNode.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISWarpImage.cpp 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISWarpImage.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISWarpImageUsingTriangulation.cpp 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISWarpImageUsingTriangulation.h 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISBasicNNGCPImageWarp.cpp 
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISBasicNNGCPImageWarp.h
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISPolynomialImageWarp.cpp
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISPolynomialImageWarp.h
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISImagePixelRegistration.cpp
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISImagePixelRegistration.h
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISAddGCPsGDAL.cpp
	${RSGIS_SRC_REGISTRATION_DIR}/RSGISAddGCPsGDAL.h
	)
###############################################################################

###############################################################################
# Files within the segmentation Library.
set(LIB_SEGMENTATION_H
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISGenMeanSegImage.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISSpecGroupSegmentation.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISEliminateSmallClumps.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISClumpPxls.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISRandomColourClumps.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISRegionGrowingFromClumps.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISMergeSmallClumps.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISLabelPixelsUsingClusters.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISLabelPixelsUsingPixels.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISRegionGrowAttributeTable.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISRegionGrowSegmentsPixels.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISDefineSpectralDivision.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISEliminateSinglePixels.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISMergeSegmentationTiles.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISBottomUpShapeFeatureExtraction.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISMergeSegmentations.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISMergeSegments.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISCreateImageGrid.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISDropClumps.h
	)
	
set(LIB_SEGMENTATION_CPP
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISGenMeanSegImage.cpp 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISGenMeanSegImage.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISSpecGroupSegmentation.cpp 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISSpecGroupSegmentation.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISEliminateSmallClumps.cpp 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISEliminateSmallClumps.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISClumpPxls.cpp 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISClumpPxls.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISRandomColourClumps.cpp 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISRandomColourClumps.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISRegionGrowingFromClumps.cpp 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISRegionGrowingFromClumps.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISMergeSmallClumps.cpp 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISMergeSmallClumps.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISLabelPixelsUsingClusters.cpp
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISLabelPixelsUsingClusters.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISLabelPixelsUsingPixels.cpp
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISLabelPixelsUsingPixels.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISRegionGrowAttributeTable.cpp
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISRegionGrowAttributeTable.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISRegionGrowSegmentsPixels.cpp
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISRegionGrowSegmentsPixels.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISDefineSpectralDivision.cpp
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISDefineSpectralDivision.h 
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISEliminateSinglePixels.cpp
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISEliminateSinglePixels.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISMergeSegmentationTiles.cpp
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISMergeSegmentationTiles.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISBottomUpShapeFeatureExtraction.cpp
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISBottomUpShapeFeatureExtraction.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISMergeSegmentations.cpp
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISMergeSegmentations.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISMergeSegments.cpp
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISMergeSegments.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISCreateImageGrid.cpp
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISCreateImageGrid.h
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISDropClumps.cpp
	${RSGIS_SRC_SEGMENTATION_DIR}/RSGISDropClumps.h
	)
###############################################################################

###############################################################################
# Files within the utilities Library.
set(LIB_UTILS_H
	${RSGIS_SRC_UTILS_DIR}/RSGISENVIROIException.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISTextException.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISFileUtils.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISColour.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISTextUtils.h 
	${RSGIS_SRC_UTILS_DIR}/RSGIS_ENVI_ASCII_ROI.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISDefiniensWorkspaceFileName.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISAllometricSpecies.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISAllometricEquations.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISExportForPlotting.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISParseExportForPlotting.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISGEOSFactoryGenerator.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISPlotPolygonsCSVParse.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISImageFootprintPolygonsCSVParse.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISExportForPlottingIncremental.h
	${RSGIS_SRC_UTILS_DIR}/RSGISExportData2HDF.h
	)
	
set(LIB_UTILS_CPP
	${RSGIS_SRC_UTILS_DIR}/RSGISENVIROIException.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISENVIROIException.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISTextException.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISTextException.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISFileUtils.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISFileUtils.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISColour.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISColour.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISTextUtils.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISTextUtils.h 
	${RSGIS_SRC_UTILS_DIR}/RSGIS_ENVI_ASCII_ROI.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGIS_ENVI_ASCII_ROI.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISDefiniensWorkspaceFileName.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISDefiniensWorkspaceFileName.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISAllometricSpecies.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISAllometricEquations.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISAllometricEquations.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISExportForPlotting.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISExportForPlotting.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISParseExportForPlotting.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISParseExportForPlotting.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISGEOSFactoryGenerator.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISGEOSFactoryGenerator.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISPlotPolygonsCSVParse.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISPlotPolygonsCSVParse.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISImageFootprintPolygonsCSVParse.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISImageFootprintPolygonsCSVParse.h 
	${RSGIS_SRC_UTILS_DIR}/RSGISExportForPlottingIncremental.cpp 
	${RSGIS_SRC_UTILS_DIR}/RSGISExportForPlottingIncremental.h
	${RSGIS_SRC_UTILS_DIR}/RSGISExportData2HDF.cpp
	${RSGIS_SRC_UTILS_DIR}/RSGISExportData2HDF.h
	)
###############################################################################

###############################################################################
# Files within the vector Library.
set(LIB_VEC_REPRESENTATION_CPP
	${RSGIS_SRC_VEC_DIR}/RSGISCirclePoint.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISCirclePoint.h 
    ${RSGIS_SRC_VEC_DIR}/RSGISCirclePolygon.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISCirclePolygon.h
	${RSGIS_SRC_VEC_DIR}/RSGISClassificationPolygon.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISClassificationPolygon.h 
	${RSGIS_SRC_VEC_DIR}/RSGISClassificationPolygonReader.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISClassificationPolygonReader.h 
	${RSGIS_SRC_VEC_DIR}/RSGISClassPolygon.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISClassPolygon.h 
	${RSGIS_SRC_VEC_DIR}/RSGISEmptyPolygon.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISEmptyPolygon.h
	${RSGIS_SRC_VEC_DIR}/RSGISImageTileVector.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISImageTileVector.h
	${RSGIS_SRC_VEC_DIR}/RSGISIntValuePolygon.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISIntValuePolygon.h
    ${RSGIS_SRC_VEC_DIR}/RSGISPolygonData.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISPolygonData.h 
	${RSGIS_SRC_VEC_DIR}/RSGISPointData.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISPointData.h
	${RSGIS_SRC_VEC_DIR}/RSGISZonalPolygons.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalPolygons.h 
)

set(LIB_VEC_PROCESSING_CPP
	${RSGIS_SRC_VEC_DIR}/RSGISAddAttributes.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISAddAttributes.h
	${RSGIS_SRC_VEC_DIR}/RSGISOGRPolygonReader.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISOGRPolygonReader.h 
	${RSGIS_SRC_VEC_DIR}/RSGISGEOSMultiPolygonReader.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISGEOSMultiPolygonReader.h 
	${RSGIS_SRC_VEC_DIR}/RSGISGEOSPolygonReader.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISGEOSPolygonReader.h
	${RSGIS_SRC_VEC_DIR}/RSGISIntValuePolygonReader.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISIntValuePolygonReader.h 
	${RSGIS_SRC_VEC_DIR}/RSGISPointsInterpolator.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISPointsInterpolator.h
	${RSGIS_SRC_VEC_DIR}/RSGISPolygonReader.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISPolygonReader.h 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessFeatureCopyVector.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessFeatureCopyVector.h 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessGeometry.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessGeometry.h 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessOGRFeature.h 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessOGRGeometry.h 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessVector.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessVector.h 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessVectorSQL.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessVectorSQL.h 
    ${RSGIS_SRC_VEC_DIR}/RSGISVectorBuffer.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorBuffer.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorIO.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorIO.h
	${RSGIS_SRC_VEC_DIR}/RSGISVectorProcessing.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorProcessing.h 
    ${RSGIS_SRC_VEC_DIR}/RSGISFitActiveContour4Polys.cpp
	${RSGIS_SRC_VEC_DIR}/RSGISFitActiveContour4Polys.h
)

set(LIB_VEC_ZONALSTATS_CPP
	${RSGIS_SRC_VEC_DIR}/RSGISExtractEndMembers2Matrix.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISExtractEndMembers2Matrix.h 
	${RSGIS_SRC_VEC_DIR}/RSGISFuzzyZonalStats.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISFuzzyZonalStats.h
	${RSGIS_SRC_VEC_DIR}/RSGISPopulateTopPowerSet.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISPopulateTopPowerSet.h 
    ${RSGIS_SRC_VEC_DIR}/RSGISVectorZonalException.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorZonalException.h
	${RSGIS_SRC_VEC_DIR}/RSGISVectorZonalStats.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorZonalStats.h
	${RSGIS_SRC_VEC_DIR}/RSGISVectorZonalStatsPtxtOut.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorZonalStatsPtxtOut.h 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalCountStats.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalCountStats.h 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalLocalSpatialStats.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalLocalSpatialStats.h 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalMeanStats.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalMeanStats.h 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalStats.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalStats.h 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalStats2Matrix.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalStats2Matrix.h
	${RSGIS_SRC_VEC_DIR}/RSGISZonalImage2HDF.cpp
	${RSGIS_SRC_VEC_DIR}/RSGISZonalImage2HDF.h 
)

set(LIB_VEC_UTILS_CPP
	${RSGIS_SRC_VEC_DIR}/RSGISAppendToVectorLayer.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISAppendToVectorLayer.h 
	${RSGIS_SRC_VEC_DIR}/RSGISCalcPolygonArea.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISCalcPolygonArea.h
	${RSGIS_SRC_VEC_DIR}/RSGISCalcDistanceStats.cpp
	${RSGIS_SRC_VEC_DIR}/RSGISCalcDistanceStats.h
	${RSGIS_SRC_VEC_DIR}/RSGISClusterData.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISClusterData.h 
	${RSGIS_SRC_VEC_DIR}/RSGISClusteredClassificationPolygon.cpp
	${RSGIS_SRC_VEC_DIR}/RSGISClusteredClassificationPolygon.h 
	${RSGIS_SRC_VEC_DIR}/RSGISConvertToASCII.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISConvertToASCII.h
	${RSGIS_SRC_VEC_DIR}/RSGISCopyCheckPolygons.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISCopyCheckPolygons.h 
	${RSGIS_SRC_VEC_DIR}/RSGISCopyPolygonsInPolygon.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISCopyPolygonsInPolygon.h
	${RSGIS_SRC_VEC_DIR}/RSGISCopyFeatures.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISCopyFeatures.h 
	${RSGIS_SRC_VEC_DIR}/RSGISAttCountPolyIntersect.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISAttCountPolyIntersect.h
	${RSGIS_SRC_VEC_DIR}/RSGISCreateListOfAttributeValues.h 
	${RSGIS_SRC_VEC_DIR}/RSGISCreateListOfAttributeValues.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISDropSmallPolygons.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISDropSmallPolygons.h 
	${RSGIS_SRC_VEC_DIR}/RSGISGenerateConvexHullGroups.cpp
	${RSGIS_SRC_VEC_DIR}/RSGISGenerateConvexHullGroups.h
	${RSGIS_SRC_VEC_DIR}/RSGISGeometryRotateAroundFixedPoint.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISGeometryRotateAroundFixedPoint.h
	${RSGIS_SRC_VEC_DIR}/RSGISGeometryToCircle.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISGeometryToCircle.h
	${RSGIS_SRC_VEC_DIR}/RSGISGetAttributeValues.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISGetAttributeValues.h 
	${RSGIS_SRC_VEC_DIR}/RSGISGetOGRGeometries.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISGetOGRGeometries.h 
	${RSGIS_SRC_VEC_DIR}/RSGISIdentifyTopLayerAttributes.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISIdentifyTopLayerAttributes.h
	${RSGIS_SRC_VEC_DIR}/RSGISLabelPolygonsFromClassification.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISLabelPolygonsFromClassification.h 
	${RSGIS_SRC_VEC_DIR}/RSGISParseLandXML.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISParseLandXML.h 
	${RSGIS_SRC_VEC_DIR}/RSGISPixelAreaCountInPolygon.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISPixelAreaCountInPolygon.h 
	${RSGIS_SRC_VEC_DIR}/RSGISPrintFIDs.cpp
	${RSGIS_SRC_VEC_DIR}/RSGISPrintFIDs.h
	${RSGIS_SRC_VEC_DIR}/RSGISRasterizeVector.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISRasterizeVector.h 
	${RSGIS_SRC_VEC_DIR}/RSGISRemoveContainedPolygons.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISRemoveContainedPolygons.h 
	${RSGIS_SRC_VEC_DIR}/RSGISRemovePolygonHoles.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISRemovePolygonHoles.h 
	${RSGIS_SRC_VEC_DIR}/RSGIS2DScatterPlotVariables.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGIS2DScatterPlotVariables.h
	${RSGIS_SRC_VEC_DIR}/RSGISSplitSmallLargePolygons.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISSplitSmallLargePolygons.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorAttributeFindReplace.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorAttributeFindReplace.h
	${RSGIS_SRC_VEC_DIR}/RSGISVectorMaths.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorMaths.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorMorphology.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorMorphology.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorOutputException.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorOutputException.h
	${RSGIS_SRC_VEC_DIR}/RSGISVectorSQLClassification.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorSQLClassification.h 
    ${RSGIS_SRC_VEC_DIR}/RSGISVectorUtils.cpp 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorUtils.h
	${RSGIS_SRC_VEC_DIR}/RSGISPopulateFeatsElev.cpp
	${RSGIS_SRC_VEC_DIR}/RSGISPopulateFeatsElev.h
)


set(LIB_VEC_H
	${RSGIS_SRC_VEC_DIR}/RSGISVectorZonalException.h 
    ${RSGIS_SRC_VEC_DIR}/RSGISVectorOutputException.h 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessOGRFeature.h 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessVector.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorUtils.h 
	${RSGIS_SRC_VEC_DIR}/RSGISPolygonData.h 
	${RSGIS_SRC_VEC_DIR}/RSGISPointData.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorIO.h 
	${RSGIS_SRC_VEC_DIR}/RSGISEmptyPolygon.h
	${RSGIS_SRC_VEC_DIR}/RSGISClassPolygon.h 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalPolygons.h 
	${RSGIS_SRC_VEC_DIR}/RSGISCirclePolygon.h 
	${RSGIS_SRC_VEC_DIR}/RSGISCalcPolygonArea.h 
	${RSGIS_SRC_VEC_DIR}/RSGISClassificationPolygon.h 
	${RSGIS_SRC_VEC_DIR}/RSGISImageTileVector.h 
	${RSGIS_SRC_VEC_DIR}/RSGISPointsInterpolator.h 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalStats.h 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalStats2Matrix.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorBuffer.h 
	${RSGIS_SRC_VEC_DIR}/RSGISCirclePoint.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorProcessing.h 
	${RSGIS_SRC_VEC_DIR}/RSGISAddAttributes.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorSQLClassification.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorAttributeFindReplace.h 
	${RSGIS_SRC_VEC_DIR}/RSGISIdentifyTopLayerAttributes.h 
	${RSGIS_SRC_VEC_DIR}/RSGISFuzzyZonalStats.h 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalCountStats.h 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalMeanStats.h 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessFeatureCopyVector.h 
	${RSGIS_SRC_VEC_DIR}/RSGISPrintFIDs.h 
	${RSGIS_SRC_VEC_DIR}/RSGISPixelAreaCountInPolygon.h 
	${RSGIS_SRC_VEC_DIR}/RSGISPopulateTopPowerSet.h 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessOGRGeometry.h 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessGeometry.h 
	${RSGIS_SRC_VEC_DIR}/RSGISGeometryRotateAroundFixedPoint.h 
	${RSGIS_SRC_VEC_DIR}/RSGISConvertToASCII.h 
	${RSGIS_SRC_VEC_DIR}/RSGISGeometryToCircle.h 
	${RSGIS_SRC_VEC_DIR}/RSGISClassificationPolygonReader.h 
	${RSGIS_SRC_VEC_DIR}/RSGISGEOSPolygonReader.h 
	${RSGIS_SRC_VEC_DIR}/RSGISParseLandXML.h 
	${RSGIS_SRC_VEC_DIR}/RSGISPolygonReader.h 
	${RSGIS_SRC_VEC_DIR}/RSGISGetAttributeValues.h 
	${RSGIS_SRC_VEC_DIR}/RSGISClusteredClassificationPolygon.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorMorphology.h 
	${RSGIS_SRC_VEC_DIR}/RSGISRasterizeVector.h 
	${RSGIS_SRC_VEC_DIR}/RSGISGEOSMultiPolygonReader.h 
	${RSGIS_SRC_VEC_DIR}/RSGISClusterData.h 
	${RSGIS_SRC_VEC_DIR}/RSGISIntValuePolygon.h 
	${RSGIS_SRC_VEC_DIR}/RSGISIntValuePolygonReader.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorZonalStats.h 
	${RSGIS_SRC_VEC_DIR}/RSGIS2DScatterPlotVariables.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorZonalStatsPtxtOut.h 
	${RSGIS_SRC_VEC_DIR}/RSGISCopyCheckPolygons.h 
	${RSGIS_SRC_VEC_DIR}/RSGISProcessVectorSQL.h 
	${RSGIS_SRC_VEC_DIR}/RSGISCopyPolygonsInPolygon.h 
	${RSGIS_SRC_VEC_DIR}/RSGISCreateListOfAttributeValues.h 
	${RSGIS_SRC_VEC_DIR}/RSGISOGRPolygonReader.h 
	${RSGIS_SRC_VEC_DIR}/RSGISRemoveContainedPolygons.h 
	${RSGIS_SRC_VEC_DIR}/RSGISCopyFeatures.h 
	${RSGIS_SRC_VEC_DIR}/RSGISSplitSmallLargePolygons.h 
	${RSGIS_SRC_VEC_DIR}/RSGISAppendToVectorLayer.h 
	${RSGIS_SRC_VEC_DIR}/RSGISLabelPolygonsFromClassification.h
	${RSGIS_SRC_VEC_DIR}/RSGISRemovePolygonHoles.h 
	${RSGIS_SRC_VEC_DIR}/RSGISDropSmallPolygons.h 
	${RSGIS_SRC_VEC_DIR}/RSGISZonalLocalSpatialStats.h 
	${RSGIS_SRC_VEC_DIR}/RSGISVectorMaths.h 
	${RSGIS_SRC_VEC_DIR}/RSGISGetOGRGeometries.h 
	${RSGIS_SRC_VEC_DIR}/RSGISExtractEndMembers2Matrix.h
	${RSGIS_SRC_VEC_DIR}/RSGISAttCountPolyIntersect.h
	${RSGIS_SRC_VEC_DIR}/RSGISGenerateConvexHullGroups.h
	${RSGIS_SRC_VEC_DIR}/RSGISZonalImage2HDF.h
	${RSGIS_SRC_VEC_DIR}/RSGISPopulateFeatsElev.h
	${RSGIS_SRC_VEC_DIR}/RSGISFitActiveContour4Polys.h
	)
	
	
###############################################################################

###############################################################################
# Files within the RSGISLib executable.
set(EXE_RSGISEXE_CPP
	${RSGIS_SRC_EXE_DIR}/RSGISExeClassification.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeClassification.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeCommandLine.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeCommandLine.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeCreateTestImages.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeCreateTestImages.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeEstimationAlgorithm.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeEstimationAlgorithm.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeFFTUtils.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeFFTUtils.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeFilterImages.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeFilterImages.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeFitting.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeFitting.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeImageCalculation.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeImageCalculation.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeImageConversion.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeImageConversion.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeImageUtils.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeImageUtils.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeMathsUtilities.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeMathsUtilities.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExePostClassification.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExePostClassification.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeRadarUtils.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeRadarUtils.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeSARBayesianBiomass.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeSARBayesianBiomass.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeSARSaatchiBiomass.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeSARSaatchiBiomass.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeSegment.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeSegment.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeStackBands.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeStackBands.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeTransectModel.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeTransectModel.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeVectorUtils.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeVectorUtils.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeVisualisation.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeVisualisation.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeZonalStats.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeZonalStats.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeImageRegistration.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeImageRegistration.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeImageCalibration.cpp 
	${RSGIS_SRC_EXE_DIR}/RSGISExeImageCalibration.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeElevationDataTools.cpp
	${RSGIS_SRC_EXE_DIR}/RSGISExeElevationDataTools.h 
	${RSGIS_SRC_EXE_DIR}/RSGISExeImageMorphology.cpp
	${RSGIS_SRC_EXE_DIR}/RSGISExeImageMorphology.h
	${RSGIS_SRC_EXE_DIR}/main.cpp
	)

###############################################################################


###############################################################################
# Group source files for IDE source explorers
source_group("CMake Files" FILES CMakeLists.txt)
source_group("RSGISEXE" FILES ${EXE_RSGISEXE_CPP} )
source_group("cmpfit" FILES ${LIB_CMPFIT_CPP})
source_group("Calibration" FILES ${LIB_CALIBRATION_CPP})
source_group("Classify" FILES ${LIB_CLASSIFY_CPP})
source_group("Commands" FILES ${LIB_COMMANDS_CPP})
source_group("Common" FILES ${LIB_COMMON_CPP})
source_group("DataStructures" FILES ${LIB_DATASTRUCT_CPP})
source_group("Filtering" FILES ${LIB_FILTERING_CPP})
source_group("Geometry" FILES ${LIB_GEOM_CPP})
source_group("Imaging" FILES ${LIB_IMG_CPP})
source_group("Maths" FILES ${LIB_MATH_CPP})
source_group("Modeling" FILES ${LIB_MODELING_CPP})
#source_group("Radar" FILES ${LIB_RADAR_CPP})
source_group("Biomass" FILES ${LIB_RADAR_BIOMASS_CPP})
source_group("Utils" FILES ${LIB_RADAR_UTILS_CPP})
source_group("Inversion" FILES ${LIB_RADAR_INVERSION_CPP})
source_group("RasterGIS" FILES ${LIB_RASTERGIS_CPP})
source_group("Registration" FILES ${LIB_REGISTRATION_CPP})
source_group("Segmentation" FILES ${LIB_SEGMENTATION_CPP})
source_group("Utilities" FILES ${LIB_UTILS_CPP})
source_group("Processing" FILES ${LIB_VEC_PROCESSING_CPP})
source_group("Representation" FILES ${LIB_VEC_REPRESENTATION_CPP})
source_group("Utils" FILES ${LIB_VEC_UTILS_CPP})
source_group("ZonalStats" FILES ${LIB_VEC_ZONALSTATS_CPP})
###############################################################################

###############################################################################
# Build and link library

add_library( ${RSGISLIB_COMMONS_LIB_NAME} ${LIB_COMMON_CPP} )
target_link_libraries(${RSGISLIB_COMMONS_LIB_NAME} ${BOOST_LIBRARIES} ${XERCESC_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} )

add_library( ${RSGISLIB_DATASTRUCT_LIB_NAME} ${LIB_DATASTRUCT_CPP} )
target_link_libraries(${RSGISLIB_DATASTRUCT_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${BOOST_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} )

add_library( ${RSGISLIB_MATHS_LIB_NAME} ${LIB_MATH_CPP} ${LIB_CMPFIT_CPP} )
target_link_libraries(${RSGISLIB_MATHS_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${BOOST_LIBRARIES} ${GSL_LIBRARIES} ${MUPARSER_LIBRARIES} ${GEOS_LIBRARIES} ${GDAL_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} ${CGAL_LIBRARIES} )

add_library( ${RSGISLIB_UTILS_LIB_NAME} ${LIB_UTILS_CPP} )
target_link_libraries(${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GEOS_LIBRARIES} ${XERCESC_LIBRARIES} ${HDF5_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} )

add_library( ${RSGISLIB_GEOM_LIB_NAME} ${LIB_GEOM_CPP} )
target_link_libraries(${RSGISLIB_GEOM_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_DATASTRUCT_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME}  ${RSGISLIB_UTILS_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GEOS_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} )

add_library( ${RSGISLIB_IMG_LIB_NAME} ${LIB_IMG_CPP} )
target_link_libraries(${RSGISLIB_IMG_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_DATASTRUCT_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME}  ${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_GEOM_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GEOS_LIBRARIES} ${GSL_LIBRARIES} ${MUPARSER_LIBRARIES} ${XERCESC_LIBRARIES}  ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} ${CGAL_LIBRARIES} )

add_library( ${RSGISLIB_REGISTRATION_LIB_NAME} ${LIB_REGISTRATION_CPP} )
target_link_libraries(${RSGISLIB_REGISTRATION_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME}  ${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_GEOM_LIB_NAME} ${RSGISLIB_IMG_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GEOS_LIBRARIES} ${GSL_LIBRARIES} ${CGAL_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} )

add_library( ${RSGISLIB_FILTERING_LIB_NAME} ${LIB_FILTERING_CPP} )
target_link_libraries(${RSGISLIB_FILTERING_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME}  ${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_GEOM_LIB_NAME} ${RSGISLIB_IMG_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GEOS_LIBRARIES} ${GSL_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} )

add_library( ${RSGISLIB_RASTERGIS_LIB_NAME} ${LIB_RASTERGIS_CPP} )
target_link_libraries(${RSGISLIB_RASTERGIS_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME}  ${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_GEOM_LIB_NAME} ${RSGISLIB_IMG_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GEOS_LIBRARIES} ${GSL_LIBRARIES} ${HDF5_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} )

add_library( ${RSGISLIB_CALIBRATION_LIB_NAME} ${LIB_CALIBRATION_CPP} )
target_link_libraries(${RSGISLIB_CALIBRATION_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME}  ${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_GEOM_LIB_NAME} ${RSGISLIB_IMG_LIB_NAME} ${RSGISLIB_RASTERGIS_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GEOS_LIBRARIES} ${GSL_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} )

add_library( ${RSGISLIB_SEGMENTATION_LIB_NAME} ${LIB_SEGMENTATION_CPP} )
target_link_libraries(${RSGISLIB_SEGMENTATION_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME}  ${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_GEOM_LIB_NAME} ${RSGISLIB_IMG_LIB_NAME} ${RSGISLIB_RASTERGIS_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GEOS_LIBRARIES} ${GSL_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} )

add_library( ${RSGISLIB_VECTOR_LIB_NAME} ${LIB_VEC_REPRESENTATION_CPP} ${LIB_VEC_PROCESSING_CPP} ${LIB_VEC_ZONALSTATS_CPP} ${LIB_VEC_UTILS_CPP} )
target_link_libraries(${RSGISLIB_VECTOR_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME}  ${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_GEOM_LIB_NAME} ${RSGISLIB_IMG_LIB_NAME} ${RSGISLIB_RASTERGIS_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GEOS_LIBRARIES} ${XERCESC_LIBRARIES} ${MUPARSER_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} )

add_library( ${RSGISLIB_MODELING_LIB_NAME} ${LIB_MODELING_CPP} )
target_link_libraries(${RSGISLIB_MODELING_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME}  ${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_GEOM_LIB_NAME} ${RSGISLIB_VECTOR_LIB_NAME} ${BOOST_LIBRARIES} ${GEOS_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} )

add_library( ${RSGISLIB_RADAR_LIB_NAME} ${LIB_RADAR_UTILS_CPP} ${LIB_RADAR_BIOMASS_CPP} ${LIB_RADAR_INVERSION_CPP} )
target_link_libraries(${RSGISLIB_RADAR_LIB_NAME}  ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME}  ${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_GEOM_LIB_NAME} ${RSGISLIB_IMG_LIB_NAME} ${RSGISLIB_VECTOR_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GSL_LIBRARIES} ${GEOS_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} )

add_library( ${RSGISLIB_CLASSIFY_LIB_NAME} ${LIB_CLASSIFY_CPP} )
target_link_libraries(${RSGISLIB_CLASSIFY_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME}  ${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_GEOM_LIB_NAME} ${RSGISLIB_IMG_LIB_NAME} ${RSGISLIB_VECTOR_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GSL_LIBRARIES} ${GEOS_LIBRARIES} ${XERCES_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} )

add_library( ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${LIB_COMMANDS_CPP} )
target_link_libraries(${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME} ${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_GEOM_LIB_NAME} ${RSGISLIB_IMG_LIB_NAME} ${RSGISLIB_VECTOR_LIB_NAME} ${RSGISLIB_CALIBRATION_LIB_NAME} ${RSGISLIB_CLASSIFY_LIB_NAME} ${RSGISLIB_DATASTRUCT_LIB_NAME} ${RSGISLIB_FILTERING_LIB_NAME} ${RSGISLIB_MODELING_LIB_NAME} ${RSGISLIB_RADAR_LIB_NAME} ${RSGISLIB_RASTERGIS_LIB_NAME} ${RSGISLIB_REGISTRATION_LIB_NAME} ${RSGISLIB_SEGMENTATION_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GSL_LIBRARIES} ${CGAL_LIBRARIES} ${GEOS_LIBRARIES} ${XERCES_LIBRARIES}  ${MUPARSER_LIBRARIES} ${HDF5_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} ${BOOST_LIBRARIES})

###############################################################################

###############################################################################
# Set target properties

SET_TARGET_PROPERTIES(${RSGISLIB_COMMONS_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_COMMONS_VERSION}
        VERSION ${RSGISLIB_COMMONS_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_DATASTRUCT_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_DATASTRUCT_VERSION}
        VERSION ${RSGISLIB_DATASTRUCT_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_MATHS_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_MATHS_VERSION}
        VERSION ${RSGISLIB_MATHS_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_UTILS_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_UTILS_VERSION}
        VERSION ${RSGISLIB_UTILS_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_GEOM_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_GEOM_VERSION}
        VERSION ${RSGISLIB_GEOM_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_IMG_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_IMG_VERSION}
        VERSION ${RSGISLIB_IMG_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_REGISTRATION_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_REGISTRATION_VERSION}
        VERSION ${RSGISLIB_REGISTRATION_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_CALIBRATION_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_CALIBRATION_VERSION}
        VERSION ${RSGISLIB_CALIBRATION_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_FILTERING_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_FILTERING_VERSION}
        VERSION ${RSGISLIB_FILTERING_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_RASTERGIS_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_RASTERGIS_VERSION}
        VERSION ${RSGISLIB_RASTERGIS_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_SEGMENTATION_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_SEGMENTATION_VERSION}
        VERSION ${RSGISLIB_SEGMENTATION_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_VECTOR_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_VECTOR_VERSION}
        VERSION ${RSGISLIB_VECTOR_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_MODELING_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_MODELING_VERSION}
        VERSION ${RSGISLIB_MODELING_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_RADAR_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_RADAR_VERSION}
        VERSION ${RSGISLIB_RADAR_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_CLASSIFY_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_CLASSIFY_VERSION}
        VERSION ${RSGISLIB_CLASSIFY_VERSION}
)

SET_TARGET_PROPERTIES(${RSGISLIB_CMDSINTERFACE_LIB_NAME}
PROPERTIES
        SOVERSION ${RSGISLIB_CMDSINTERFACE_VERSION}
        VERSION ${RSGISLIB_CMDSINTERFACE_VERSION}
)

###############################################################################

###############################################################################
# Build executables
#if (RSGISLIB_WITH_RSGISEXE)
	add_executable(rsgisexe ${EXE_RSGISEXE_CPP})
	target_link_libraries (rsgisexe ${RSGISLIB_COMMONS_LIB_NAME} ${RSGISLIB_DATASTRUCT_LIB_NAME} ${RSGISLIB_MATHS_LIB_NAME} ${RSGISLIB_UTILS_LIB_NAME} ${RSGISLIB_GEOM_LIB_NAME} ${RSGISLIB_IMG_LIB_NAME} ${RSGISLIB_REGISTRATION_LIB_NAME} ${RSGISLIB_CALIBRATION_LIB_NAME} ${RSGISLIB_FILTERING_LIB_NAME} ${RSGISLIB_RASTERGIS_LIB_NAME} ${RSGISLIB_SEGMENTATION_LIB_NAME} ${RSGISLIB_VECTOR_LIB_NAME} ${RSGISLIB_MODELING_LIB_NAME} ${RSGISLIB_RADAR_LIB_NAME} ${RSGISLIB_CLASSIFY_LIB_NAME} ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${BOOST_LIBRARIES} ${GDAL_LIBRARIES} ${GEOS_LIBRARIES} ${GSL_LIBRARIES} ${MUPARSER_LIBRARIES} ${XERCESC_LIBRARIES}  ${CGAL_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${KEA_LIBRARIES} ${BOOST_LIBRARIES})
#endif(RSGISLIB_WITH_RSGISEXE)
###############################################################################

###############################################################################
# Installation 
install (TARGETS ${RSGISLIB_COMMONS_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_DATASTRUCT_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_MATHS_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_UTILS_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_GEOM_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_IMG_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_REGISTRATION_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_CALIBRATION_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_FILTERING_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_RASTERGIS_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_SEGMENTATION_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_VECTOR_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_MODELING_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_RADAR_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_CLASSIFY_LIB_NAME} DESTINATION lib)
install (TARGETS ${RSGISLIB_CMDSINTERFACE_LIB_NAME} DESTINATION lib)

install (TARGETS rsgisexe DESTINATION bin)

install (FILES ${LIB_CALIBRATION_H} DESTINATION include/rsgis/calibration)
install (FILES ${LIB_CLASSIFY_H} DESTINATION include/rsgis/classifier)
install (FILES ${LIB_COMMON_H} DESTINATION include/rsgis/common)
install (FILES ${LIB_DATASTRUCT_H} DESTINATION include/rsgis/datastruct)
install (FILES ${LIB_FILTERING_H} DESTINATION include/rsgis/filtering)
install (FILES ${LIB_GEOM_H} DESTINATION include/rsgis/geom)
install (FILES ${LIB_IMG_H} DESTINATION include/rsgis/img)
install (FILES ${LIB_MATH_H} DESTINATION include/rsgis/math)
install (FILES ${LIB_MODELING_H} DESTINATION include/rsgis/modeling)
install (FILES ${LIB_RADAR_H} DESTINATION include/rsgis/radar)
install (FILES ${LIB_RASTERGIS_H} DESTINATION include/rsgis/rastergis)
install (FILES ${LIB_REGISTRATION_H} DESTINATION include/rsgis/registration)
install (FILES ${LIB_SEGMENTATION_H} DESTINATION include/rsgis/segmentation)
install (FILES ${LIB_UTILS_H} DESTINATION include/rsgis/utils)
install (FILES ${LIB_VEC_H} DESTINATION include/rsgis/vec)
install (FILES ${LIB_COMMANDS_H} DESTINATION include/rsgis/cmds)

###############################################################################
