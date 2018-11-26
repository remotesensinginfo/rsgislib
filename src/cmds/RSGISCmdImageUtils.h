/*
 *  RSGISCmdImageUtils.h
 *
 *
 *  Created by Pete Bunting on 03/05/2013.
 *  Copyright 2013 RSGISLib.
 *
 *  RSGISLib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RSGISLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef RSGISCmdImageUtils_H
#define RSGISCmdImageUtils_H

#include <iostream>
#include <string>
#include <vector>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_cmds_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{ namespace cmds {
    
    enum RSGISStretches
    {
        linearMinMax,
        linearPercent,
        linearStdDev,
        histogram,
        exponential,
        logarithmic,
        powerLaw
    };
    
    enum RSGISInitSharpenBandStatus
    {
        rsgis_init_ignore = 0,
        rsgis_init_lowres = 1,
        rsgis_init_highres = 2
    };
    
    struct DllExport RSGISInitSharpenBandInfo
    {
        unsigned int band;
        RSGISInitSharpenBandStatus status;
        std::string bandName;
    };
    
    struct DllExport RSGISCmdCompositeInfo
    {
        unsigned int year;
        unsigned int day;
        std::string compImg;
        std::string imgRef;
        bool outRef;
    };
    
    /** Function to run the stretch image command */
    DllExport void executeStretchImage(std::string inputImage, std::string outputImage, bool saveOutStats, std::string outStatsFile, bool ignoreZeros, bool onePassSD, std::string gdalFormat, RSGISLibDataType outDataType, RSGISStretches stretchType, float stretchParam)throw(RSGISCmdException);
    
    /** Function to run the stretch image command with predefined stretch parameters*/
    DllExport void executeStretchImageWithStats(std::string inputImage, std::string outputImage, std::string inStatsFile, std::string gdalFormat, RSGISLibDataType outDataType, RSGISStretches stretchType, float stretchParam)throw(RSGISCmdException);
    
    /** Function to run the stretch image command with predefined stretch parameters*/
    DllExport void executeNormaliseImgPxlVals(std::string inputImage, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType, float inNoDataVal, float outNoDataVal, float outMinVal, float outMaxVal, RSGISStretches stretchType, float stretchParam)throw(RSGISCmdException);
    
    /** Function to run the mask image command */
    DllExport void executeMaskImage(std::string inputImage, std::string imageMask, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType, float outValue, std::vector<float> maskValues)throw(RSGISCmdException);
    
    /** A function to split an image into image tiles.
        An overlap between tiles may be specified.
        Optionally the tiles may be offset from the image boundries by half a pixel, useful for creating two overlapping lots of tiles.
        The filenames for each tile are passed back as a vector.
     */
    DllExport void executeCreateTiles(std::string inputImage, std::string outputImageBase, unsigned int width, unsigned int height, unsigned int tileOverlap, bool offsetTiling, std::string gdalFormat, RSGISLibDataType outDataType, std::string outFileExtension, std::vector<std::string> *outFileNames = NULL)throw(RSGISCmdException);
    
    /** A function to run the populate statistics command */
    DllExport void executePopulateImgStats(std::string inputImage, bool useIgnoreVal, float nodataValue, bool calcImgPyramids, std::vector<int> pyraScaleVals=std::vector<int>())throw(RSGISCmdException);
    
    /** A function to mosaic a set of input images
        Pixels with a value of 'skipValue' in band 'skipBand' are excluded (all bands).
        Where pixels overlap:
        - The pixel is overwritten by the next image (overlapBehaviour=0)
        - The minimum value is taken (overlapBehaviour=1)
        - The maximum behaviour is taken (overlapBehaviour=1)
     */
    DllExport void executeImageMosaic(std::string *inputImages, int numDS, std::string outputImage, float background, float skipVal, unsigned int skipBand, unsigned int overlapBehaviour, std::string format, RSGISLibDataType outDataType) throw(RSGISCmdException);
    
    /** A command to add images to an existing image*/
    DllExport void executeImageInclude(std::string *inputImages, int numDS, std::string baseImage, bool bandsDefined, std::vector<int> bands, float skipVal=0.0, bool useSkipVal=false) throw(RSGISCmdException);
    
    /** A command to add images to an existing image ignoring the overlaps*/
    DllExport void executeImageIncludeOverlap(std::string *inputImages, int numDS, std::string baseImage, int numOverlapPxls) throw(RSGISCmdException);
    
    /** A command to add images to an existing image where the input images can over areas outside of the base image*/
    DllExport void executeImageIncludeIndImgIntersect(std::string *inputImages, int numDS, std::string baseImage) throw(RSGISCmdException);
    
    /** A command to create overview images in the base image by mosaicking the overviews from the tiles/subsets images */
    DllExport void executeImageIncludeOverviews(std::string baseImage, std::vector<std::string> inputImages, std::vector<int> pyraScaleVals) throw(RSGISCmdException);
    
    /** A command to order a set of input images based on the proportion of valid data within each of the scenes */
    DllExport std::vector<std::string> executeOrderImageUsingValidDataProp(std::vector<std::string> images, float noDataValue) throw(RSGISCmdException);
    
    /** A function to assign the projection on an image file */
    DllExport void executeAssignProj(std::string inputImage, std::string wktStr, bool readWKTFromFile=false, std::string wktFile="")throw(RSGISCmdException);
    
    /** A function to assign the spatial information on an image file */
    DllExport void executeAssignSpatialInfo(std::string inputImage, double xTL, double yTL, double xRes, double yRes, double xRot, double yRot, bool xTLDef, bool yTLDef, bool xResDef, bool yResDef, bool xRotDef, bool yRotDef)throw(RSGISCmdException);
    
    /** A function to copy the projection from one file to another (i.e., similar to executeAssignProj) */
    DllExport void executeCopyProj(std::string inputImage, std::string refImageFile)throw(RSGISCmdException);
    
    /** A function to copy the projection and spaital info from one file to another (i.e., similar to executeAssignProj and executeAssignSpatialInfo combined) */
    DllExport void executeCopyProjSpatial(std::string inputImage, std::string refImageFile)throw(RSGISCmdException);
    
    /** A function to stack image bands into a single output image */
    DllExport void executeStackImageBands(std::string *imageFiles, std::string *imageBandNames, int numImages, std::string outputImage, bool skipPixels, float skipValue, float noDataValue, std::string gdalFormat, RSGISLibDataType outDataType, bool replaceBandNames)throw(RSGISCmdException);
    
    /** A function to extract image values to a HDF file */
    DllExport void executeImageRasterZone2HDF(std::string imageFile, std::string maskImage, std::string outputHDF, float maskVal)throw(RSGISCmdException);
    
    /** A function to extract image band values to a HDF file */
    DllExport void executeImageBandRasterZone2HDF(std::string imageFile, std::string maskImage, std::string outputHDF, float maskVal, std::vector<unsigned int> bands)throw(RSGISCmdException);
    
    /** A function to extract image band values to a HDF file */
    DllExport void executeImageBandRasterZone2HDF(std::vector<std::pair<std::string, std::vector<unsigned int> > > imageFiles, std::string maskImage, std::string outputHDF, float maskVal)throw(RSGISCmdException);

    /** A function to sample a list of values saved in a HDF5 file */
    DllExport void executeRandomSampleH5File(std::string inputH5, std::string outputH5, unsigned int nSample, int seed)throw(RSGISCmdException);

    /** A function to sample a list of values saved in a HDF5 file */
    DllExport void executeSplitSampleH5File(std::string inputH5, std::string outputP1H5, std::string outputP2H5, unsigned int nSample, int seed)throw(RSGISCmdException);
    
    /** A function to subset an image to the bounding box of a polygon */
    DllExport void executeSubset(std::string inputImage, std::string inputVector, std::string outputImage, std::string imageFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);
    
    /** A function to subset an image to a bounding box */
    DllExport void executeSubsetBBox(std::string inputImage, std::string outputImage, std::string imageFormat, RSGISLibDataType outDataType, double xMin, double xMax, double yMin, double yMax) throw(RSGISCmdException);
    
    /** A function to subset an image to polygons within shapefile */
    DllExport void executeSubset2Polys(std::string inputImage, std::string inputVector, std::string filenameAttribute, std::string outputImageBase, std::string imageFormat, RSGISLibDataType outDataType, std::string outFileExtension, std::vector<std::string> *outFileNames = NULL) throw(RSGISCmdException);
    
    /** A function to subset an image to another image*/
    DllExport void executeSubset2Img(std::string inputImage, std::string inputROIImage, std::string outputImage, std::string imageFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);

    /** A function to subset an input data by to a set of image bands */
    DllExport void executeSubsetImageBands(std::string inputImage, std::string outputImage, std::vector<unsigned int> bands, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    
    /** A function to create a new blank image */
    DllExport void executeCreateBlankImage(std::string outputImage, unsigned int numBands, unsigned int width, unsigned int height, double tlX, double tlY, double resolution, float pxlVal, std::string wktFile, std::string wktStr, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);

    /** A function to create a new blank image using an exising image as a base. */
    DllExport void executeCreateCopyBlankImage(std::string inputImage, std::string outputImage, unsigned int numBands, float pxlVal, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);
    
    /** A function to create a new blank image using an exising image as a base. */
    DllExport void executeCreateCopyBlankImage(std::string inputImage, std::string outputImage, unsigned int numBands, double xMin, double xMax, double yMin, double yMax, double resX, double resY, float pxlVal, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);
    
    /** A function to create a new blank image using an exising image as a base but cutting to extent of shapefile. */
    DllExport void executeCreateCopyBlankImageVecExtent(std::string inputImage, std::string inputVector, std::string outputImage, unsigned int numBands, float pxlVal, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);
    
    /** A function to calculate summary statistics for every band in a stack or every n bands */
    DllExport void executeStackStats(std::string inputImage, std::string outputImage, std::string calcStat, bool allBands, unsigned int numBands, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);

    /** A function to produce an image with pixel values on a cycle with a specified range */
    DllExport void executeProduceRegularGridImage(std::string inputImage, std::string outputImage, std::string gdalFormat, float pxlRes, int minVal=0, int maxVal=1, bool singleLine=false) throw(RSGISCmdException);
    
    /** A function to produce a binary image for regions with finite data values */
    DllExport void executeFiniteImageMask(std::string inputImage, std::string outputImage, std::string gdalFormat) throw(RSGISCmdException);
        
    /** A function to produce a binary image for valid regions within all the input images (i.e., not the no data value) */
    DllExport void executeValidImageMask(std::vector<std::string> inputImages, std::string outputImage, std::string gdalFormat, float noDataVal=0.0) throw(RSGISCmdException);
    
    /** A function to combine images together into a single image band by excluding the no data value */
    DllExport void executeCombineImagesSingleBandIgnoreNoData(std::vector<std::string> inputImages, std::string outputImage, float noDataVal, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);
    
    /** A function to create a random sample of points within a mask */
    DllExport void executePerformRandomPxlSample(std::string inputImage, std::string outputImage, std::string gdalFormat, std::vector<int> maskVals, unsigned long numSamples) throw(RSGISCmdException);
    
    /** A function to create a random sample of points within a mask - for regions with smaller number of pixels within large image */
    DllExport void executePerformRandomPxlSampleSmallPxlCount(std::string inputImage, std::string outputImage, std::string gdalFormat, std::vector<int> maskVals, unsigned long numSamples, int rndSeed) throw(RSGISCmdException);
    
    /** A function to perform a pan-sharpening using a Hyperspherical Colour Space technique */
    DllExport void executePerformHCSPanSharpen(std::string inputImage, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType, unsigned int winSize=7, bool useNaiveMethod=false) throw(RSGISCmdException);
    
    /** A function to sharpen nn resampled lower resolution image bands using high native resolution image bands in the same stack */
    DllExport void executeSharpenLowResImgBands(std::string inputImage, std::string outputImage, std::vector<RSGISInitSharpenBandInfo> bandInfo, unsigned int winSize, int noDataVal, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);
    
    /** A function to create a composite image where the pixel from the image with the high NDVI is outputted. */
    DllExport void executeCreateMaxNDVICompsiteImage(std::vector<std::string> inputImages, std::string outputImage, unsigned int redBand, unsigned int nirBand, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);
    
    /** A function to create a composite image where the pixel defined in the reference image is outputted - note the order of the input images needs to correspond with the indexes in the reference image. */
    DllExport void executeCreateRefImgCompsiteImage(std::vector<std::string> inputImages, std::string outputImage, std::string refImage, std::string gdalFormat, RSGISLibDataType outDataType, float outNoDataVal) throw(RSGISCmdException);
    
    /** A function to use the composite reference images to identify regions which need filling and creates a new reference image for each composite as to where the fill should come from. */
    DllExport void executeGenTimeseriesFillCompositeImg(std::vector<RSGISCmdCompositeInfo> inCompInfo, std::string validMaskImage, std::string outFillRefImg, std::string outCompImg, std::string outCompRefImg, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);
    
    /** A function to combine multiple image bands into a single band using a reference image. */
    DllExport void executeExportSingleMergedImgBand(std::string inputImage, std::string inputRefImage, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException);
    
}}


#endif

