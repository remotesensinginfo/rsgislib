/*
 *  segmentation.cpp
 *  RSGIS_LIB
 *
 *  Created by Sam Gillingham on 02/05/2013.
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
 
#include "rsgispy_common.h"
#include "cmds/RSGISCmdSegmentation.h"

/* An exception object for this module */
/* created in the init function */
struct SegmentationState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct SegmentationState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct SegmentationState _state;
#endif


static PyObject *Segmentation_labelPixelsFromClusterCentres(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("cluster_centres_file"), RSGIS_PY_C_TEXT("ignore_zeros"),
                             RSGIS_PY_C_TEXT("gdalformat"),  nullptr};
    const char *pszInputImage, *pszOutputImage, *pszClusterCentres, *pszgdalformat;
    int ignoreZeros;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssis:label_pixels_from_cluster_centres", kwlist, &pszInputImage, &pszOutputImage,
                                &pszClusterCentres, &ignoreZeros, &pszgdalformat ))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeLabelPixelsFromClusterCentres(std::string(pszInputImage), std::string(pszOutputImage),
                                                          std::string(pszClusterCentres),ignoreZeros,
                                                          std::string(pszgdalformat) );
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_eliminateSinglePixels(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("clumps_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("tmp_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("in_memory"),
                             RSGIS_PY_C_TEXT("ignore_zeros"),  nullptr};
    const char *pszInputImage, *pszOutputImage, *pszClumpsImage, *pszgdalformat, *pszTempImage;
    int processInMemory, ignoreZeros;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssssii:eliminate_single_pixels", kwlist, &pszInputImage, &pszClumpsImage, &pszOutputImage, &pszTempImage, &pszgdalformat, &processInMemory, &ignoreZeros ))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeEliminateSinglePixels(std::string(pszInputImage), std::string(pszClumpsImage),
                                                  std::string(pszOutputImage), std::string(pszTempImage),
                                                  std::string(pszgdalformat), processInMemory, ignoreZeros);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_clump(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("in_memory"),
                             RSGIS_PY_C_TEXT("no_data_val"), RSGIS_PY_C_TEXT("add_to_rat"),  nullptr};
    const char *pszInputImage, *pszOutputImage, *pszgdalformat;
    int processInMemory = false;
    bool nodataprovided;
    float fnodata;
    int addRatPxlVals = false;
    PyObject *pNoData = Py_None; //could be none or a number
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sss|iOi:clump", kwlist, &pszInputImage, &pszOutputImage, &pszgdalformat, &processInMemory, &pNoData, &addRatPxlVals))
    {
        return nullptr;
    }

    if( pNoData == Py_None )
    {
        nodataprovided = false;
        fnodata = 0;
    }
    else
    {
        // convert to a float if needed
        PyObject *pFloatNoData = PyNumber_Float(pNoData);
        if( pFloatNoData == nullptr )
        {
            PyErr_SetString(GETSTATE(self)->error, "nodata parameter must be None or a valid number\n");
            return nullptr;
        }

        nodataprovided = true;
        fnodata = PyFloat_AsDouble(pFloatNoData);
        Py_DECREF(pFloatNoData);
    }

    try
    {
        rsgis::cmds::executeClump(std::string(pszInputImage), std::string(pszOutputImage), std::string(pszgdalformat),
                                processInMemory, nodataprovided, fnodata, addRatPxlVals);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_RMSmallClumpsStepwise(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("clumps_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("use_stch_stats"), RSGIS_PY_C_TEXT("stch_stats_file"),
                             RSGIS_PY_C_TEXT("store_mean"), RSGIS_PY_C_TEXT("in_memory"),
                             RSGIS_PY_C_TEXT("min_clump_size"), RSGIS_PY_C_TEXT("pxl_val_thres"), nullptr};
    const char *pszInputImage, *pszClumpsImage, *pszOutputImage, *pszgdalformat, *pszStretchStatsFile;
    int storeMean,processInMemory,stretchStatsAvail;
    unsigned int minClumpSize;
    float specThreshold;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssisiiIf:rm_small_clumps_stepwise", kwlist, &pszInputImage, &pszClumpsImage, &pszOutputImage, &pszgdalformat,
                    &stretchStatsAvail, &pszStretchStatsFile, &storeMean, &processInMemory, &minClumpSize, &specThreshold))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeRMSmallClumpsStepwise(std::string(pszInputImage), std::string(pszClumpsImage),
                                                  std::string(pszOutputImage), std::string(pszgdalformat),
                                                  stretchStatsAvail, pszStretchStatsFile, storeMean,
                                                  processInMemory, minClumpSize, specThreshold);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_relabelClumps(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("in_memory"), nullptr};
    const char *pszInputImage, *pszOutputImage, *pszgdalformat;
    int processInMemory;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssi:relabel_clumps", kwlist, &pszInputImage,
                        &pszOutputImage, &pszgdalformat, &processInMemory ))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeRelabelClumps(std::string(pszInputImage), std::string(pszOutputImage),
                                          std::string(pszgdalformat), processInMemory);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_unionOfClumps(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("no_data_val"),
                             RSGIS_PY_C_TEXT("add_to_rat"), nullptr};
    const char *pszOutputImage, *pszgdalformat;
    std::string inputImage;
    bool nodataprovided;
    float fnodata;
    PyObject *pNoData; //could be none or a number
    PyObject *pInputListObj;
    int addRatPxlVals = false;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "OssOi:union_of_clumps", kwlist, &pInputListObj, &pszOutputImage, &pszgdalformat, &pNoData, &addRatPxlVals))
    {
        return nullptr;
    }

    if( pNoData == Py_None )
    {
        nodataprovided = false;
        fnodata = 0;
    }
    else
    {
        // convert to a float if needed
        PyObject *pFloatNoData = PyNumber_Float(pNoData);
        if( pFloatNoData == nullptr )
        {
            PyErr_SetString(GETSTATE(self)->error, "nodata parameter must be None or a valid number\n");
            return nullptr;
        }

        nodataprovided = true;
        fnodata = PyFloat_AsDouble(pFloatNoData);
        Py_DECREF(pFloatNoData);
    }

    Py_ssize_t nInputImages = PyList_Size(pInputListObj);
    if( nInputImages < 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a list");
        return nullptr;
    }

    std::vector<std::string> inputImagePaths;
    for(Py_ssize_t n = 0; n < nInputImages; n++)
    {

        PyObject *strObj;
        strObj = PyList_GetItem(pInputListObj, n);
        if( !RSGISPY_CHECK_STRING(strObj) )
        {
            PyErr_SetString(GETSTATE(self)->error, "must pass a list of strings");
            Py_DECREF(strObj);
            return nullptr;
        }
        inputImage = RSGISPY_STRING_EXTRACT(strObj);
        inputImagePaths.push_back(inputImage);
        //Py_DECREF(strObj);

    }

    try
    {
        rsgis::cmds::executeUnionOfClumps(inputImagePaths, std::string(pszOutputImage),
                                          std::string(pszgdalformat), nodataprovided, fnodata, addRatPxlVals);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_mergeSegmentationTiles(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("border_msk_img"), RSGIS_PY_C_TEXT("tile_boundary"),
                             RSGIS_PY_C_TEXT("tile_overlap"), RSGIS_PY_C_TEXT("tile_body"),
                             RSGIS_PY_C_TEXT("col_name"), nullptr};
    const char *pszOutputImage, *pszBorderMaskImage, *pszColsName;
    std::string inputImage;
    unsigned int tileBoundary, tileOverlap, tileBody;
    PyObject *pInputListObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "OssIIIs:merge_segmentation_tiles", kwlist, &pInputListObj, &pszOutputImage,
                                     &pszBorderMaskImage, &tileBoundary, &tileOverlap, &tileBody, &pszColsName))
    {
        return nullptr;
    }

    Py_ssize_t nInputImages = PyList_Size(pInputListObj);
    if( nInputImages < 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a list");
        return nullptr;
    }

    std::vector<std::string> inputImagePaths;
    for(Py_ssize_t n = 0; n < nInputImages; n++)
    {

        PyObject *strObj;
        strObj = PyList_GetItem(pInputListObj, n);
        if( !RSGISPY_CHECK_STRING(strObj) )
        {
            PyErr_SetString(GETSTATE(self)->error, "must pass a list of strings");
            Py_DECREF(strObj);
            return nullptr;
        }
        inputImage = RSGISPY_STRING_EXTRACT(strObj);
        inputImagePaths.push_back(inputImage);
        //Py_DECREF(strObj);

    }

    try
    {
        rsgis::cmds::executeMergeSegmentationTiles(std::string(pszOutputImage), std::string(pszBorderMaskImage),
                                                   inputImagePaths, tileBoundary, tileOverlap, tileBody, std::string(pszColsName));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_mergeClumpImages(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("merge_rats"), nullptr};
    const char *pszOutputImage;
    PyObject *pInputListObj;
    std::string inputImage;
    int mergeRATs = false;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "Os|i:merge_clump_images", kwlist, &pInputListObj, &pszOutputImage, &mergeRATs))
    {
        return nullptr;
    }

    Py_ssize_t nInputImages = PyList_Size(pInputListObj);
    if( nInputImages < 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a list");
        return nullptr;
    }

    std::vector<std::string> inputImagePaths;
    for(Py_ssize_t n = 0; n < nInputImages; n++)
    {

        PyObject *strObj;
        strObj = PyList_GetItem(pInputListObj, n);
        if( !RSGISPY_CHECK_STRING(strObj) )
        {
            PyErr_SetString(GETSTATE(self)->error, "must pass a list of strings");
            Py_DECREF(strObj);
            return nullptr;
        }
        inputImage = RSGISPY_STRING_EXTRACT(strObj);
        inputImagePaths.push_back(inputImage);
    }

    try
    {
        rsgis::cmds::executeMergeClumpImages(inputImagePaths, std::string(pszOutputImage), mergeRATs);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_findTileBordersMask(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("border_msk_img"),
                             RSGIS_PY_C_TEXT("tile_boundary"), RSGIS_PY_C_TEXT("tile_overlap"),
                             RSGIS_PY_C_TEXT("tile_body"), RSGIS_PY_C_TEXT("col_name"), nullptr};
    const char *pszBorderMaskImage, *pszColsName;
    std::string inputImage;
    unsigned int tileBoundary, tileOverlap, tileBody;
    PyObject *pInputListObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "OsIIIs:find_tile_borders_mask", kwlist, &pInputListObj, &pszBorderMaskImage,
                                &tileBoundary, &tileOverlap, &tileBody, &pszColsName))
    {
        return nullptr;
    }

    Py_ssize_t nInputImages = PyList_Size(pInputListObj);
    if( nInputImages < 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a list");
        return nullptr;
    }

    std::vector<std::string> inputImagePaths;
    for(Py_ssize_t n = 0; n < nInputImages; n++)
    {

        PyObject *strObj;
        strObj = PyList_GetItem(pInputListObj, n);
        if( !RSGISPY_CHECK_STRING(strObj) )
        {
            PyErr_SetString(GETSTATE(self)->error, "must pass a list of strings");
            Py_DECREF(strObj);
            return nullptr;
        }
        inputImage = RSGISPY_STRING_EXTRACT(strObj);
        inputImagePaths.push_back(inputImage);
        //Py_DECREF(strObj);

    }

    try
    {

        rsgis::cmds::executeFindTileBordersMask(inputImagePaths, std::string(pszBorderMaskImage),
                        tileBoundary, tileOverlap, tileBody, std::string(pszColsName));

    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_rmSmallClumps(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("area_threshold"), RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInputClumps, *pszOutputClumps, *pszgdalformat;
    float areaThreshold;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssfs:rm_small_clumps", kwlist, &pszInputClumps, &pszOutputClumps, &areaThreshold, &pszgdalformat))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeRMSmallClumps(std::string(pszInputClumps), std::string(pszOutputClumps), areaThreshold, std::string(pszgdalformat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_meanImage(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("clumps_img"),
                             RSGIS_PY_C_TEXT("output_img"),  RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    const char *pszInputImage, *pszInputClumps, *pszOutputImage, *pszgdalformat;
    int nDataType;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssi:mean_image", kwlist, &pszInputImage, &pszInputClumps, &pszOutputImage, &pszgdalformat, &nDataType))
    {
        return nullptr;
    }

    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeMeanImage(std::string(pszInputImage), std::string(pszInputClumps), std::string(pszOutputImage),
                                      std::string(pszgdalformat), type, false);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_GenerateRegularGrid(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("num_x_pxls"),
                             RSGIS_PY_C_TEXT("num_y_pxls"), RSGIS_PY_C_TEXT("offset"), nullptr};
    const char *pszInputImage, *pszOutputImage, *pszgdalformat;
    unsigned int numXPxls, numYPxls;
    int offset = 0;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssII|i:generate_regular_grid", kwlist, &pszInputImage, &pszOutputImage, &pszgdalformat, &numXPxls, &numYPxls, &offset))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeGenerateRegularGrid(std::string(pszInputImage), std::string(pszOutputImage),
                                                std::string(pszgdalformat), numXPxls, numYPxls, (bool)offset);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_IncludeRegionsInClumps(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("regions_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszClumpsImage, *pszRegionsImage, *pszOutputImage, *pszgdalformat;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssss:include_regions_in_clumps", kwlist, &pszClumpsImage, &pszRegionsImage, &pszOutputImage, &pszgdalformat ))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeIncludeClumpedRegion(std::string(pszClumpsImage), std::string(pszRegionsImage),
                                                 std::string(pszOutputImage), std::string(pszgdalformat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *Segmentation_mergeSegments2Neighbours(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("input_vals_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("sel_clumps_col"), RSGIS_PY_C_TEXT("no_data_clumps_col"), nullptr};
    const char *pszInputClumpsImage, *pszInputSpecImage, *pszOutputImage, *pszgdalformat, *selectClumpsCol, *noDataClumpsCol;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssss:merge_segments_to_neighbours", kwlist, &pszInputClumpsImage, &pszInputSpecImage,
                                     &pszOutputImage, &pszgdalformat, &selectClumpsCol, &noDataClumpsCol))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeMergeSelectClumps2Neighbour(std::string(pszInputSpecImage), std::string(pszInputClumpsImage),
                                                        std::string(pszOutputImage), std::string(pszgdalformat),
                                                        std::string(selectClumpsCol), std::string(noDataClumpsCol));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *Segmentation_dropSelectedSegments(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("sel_clumps_col"), nullptr};
    const char *pszInputClumpsImage, *pszOutputImage, *pszgdalformat, *selectClumpsCol;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssss:drop_selected_clumps", kwlist, &pszInputClumpsImage,
                                     &pszOutputImage, &pszgdalformat, &selectClumpsCol ))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeDropSelectedClumps(std::string(pszInputClumpsImage), std::string(pszOutputImage),
                                               std::string(pszgdalformat), std::string(selectClumpsCol));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *Segmentation_mergeEquivalentClumps(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("val_columns"), nullptr};
    const char *pszInputClumpsImage, *pszOutputImage, *pszgdalformat;
    PyObject *valClumpsCols;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssO:merge_equiv_clumps", kwlist, &pszInputClumpsImage, &pszOutputImage, &pszgdalformat, &valClumpsCols))
    {
        return nullptr;
    }

    Py_ssize_t nValCols = PyList_Size(valClumpsCols);
    if( nValCols < 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a list");
        return nullptr;
    }

    std::vector<std::string> cols;
    for(Py_ssize_t n = 0; n < nValCols; n++)
    {
        PyObject *strObj = PyList_GetItem(valClumpsCols, n);
        if( !RSGISPY_CHECK_STRING(strObj) )
        {
            PyErr_SetString(GETSTATE(self)->error, "must pass a list of strings");
            Py_DECREF(strObj);
            return nullptr;
        }
        std::string colName = RSGISPY_STRING_EXTRACT(strObj);
        cols.push_back(colName);
    }

    try
    {
        rsgis::cmds::executeMergeClumpsEquivalentVal(std::string(pszInputClumpsImage), std::string(pszOutputImage),
                                                     std::string(pszgdalformat), cols);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}




// Our list of functions in this module
static PyMethodDef SegmentationMethods[] = {
    {"label_pixels_from_cluster_centres", (PyCFunction)Segmentation_labelPixelsFromClusterCentres, METH_VARARGS | METH_KEYWORDS,
"segmentation.label_pixels_from_cluster_centres(input_img, output_img, cluster_centres_file, ignore_zeros, gdalformat)\n"
"Labels image pixels with the ID of the nearest cluster centre.\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param cluster_centres_file: is a string containing the name of the cluster centre file\n"
":param ignore_zeros: zeros is a bool\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
"\n"},

    {"eliminate_single_pixels", (PyCFunction)Segmentation_eliminateSinglePixels, METH_VARARGS | METH_KEYWORDS,
"segmentation.eliminate_single_pixels(input_img, clumps_img, output_img, tmp_img, gdalformat, in_memory, ignorezeros)\n"
"Eliminates single pixels\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param clumps_img: is a string containing the name of the clump file\n"
":param output_img: is a string containing the name of the output file\n"
":param tmp_img: is a string containing the name of the temporary file to use\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param in_memory: is a bool specifying if processing should be carried out in memory (faster if sufficient RAM is available, set to False if unsure).\n"
":param ignore_zeros: is a bool\n"
"\n"},

    {"clump", (PyCFunction)Segmentation_clump, METH_VARARGS | METH_KEYWORDS,
"segmentation.clump(input_img, output_img, gdalformat, in_memory, no_data_val, add_to_rat)\n"
"A function which clumps an input image (of int pixel data type) to identify connected independent sets of pixels.\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param in_memory: is a bool specifying if processing should be carried out in memory (faster if sufficient RAM is available, set to False if unsure).\n"
":param no_data_val: is None or float\n"
":param add_to_rat: is a boolean specifying whether the pixel value (from input_img) should be added as a RAT (Column Name: PixelVal).\n"
"\n"},

    {"rm_small_clumps_stepwise", (PyCFunction)Segmentation_RMSmallClumpsStepwise, METH_VARARGS | METH_KEYWORDS,
"segmentation.rm_small_clumps_stepwise(input_img, clumps_img, output_img, gdalformat, use_stch_stats, stch_stats_file, store_mean, in_memory, min_clump_size, pxl_val_thres)\n"
"Eliminate clumps smaller than a given size from the scene, small clumps will be combined with their spectrally closest neighbouring  clump in a stepwise fashion unless over spectral distance threshold\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param clumps_img: is a string containing the name of the clump file\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param use_stch_stats: is a bool\n"
":param stch_stats_file: is a string containing the name of the stretch stats file\n"
":param store_mean: is a bool\n"
":param in_memory: is a bool specifying if processing should be carried out in memory (faster if sufficient RAM is available, set to False if unsure).\n"
":param min_clump_size: is an unsigned integer providing the minimum size for clumps.\n"
":param pxl_val_thres: is a float providing the maximum (Euclidian distance) spectral separation for which to merge clumps. Set to a large value to ignore spectral separation and always merge.\n"
"\n"},

    {"relabel_clumps", (PyCFunction)Segmentation_relabelClumps, METH_VARARGS | METH_KEYWORDS,
"segmentation.relabel_clumps(input_img, output_img, gdalformat, in_memory)\n"
"Relabel clumps so numbering is consecutive with output gaps\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param in_memory: is a bool specifying if processing should be carried out in memory (faster if sufficient RAM is available, set to False if unsure).\n"
"\n"},

    {"union_of_clumps", (PyCFunction)Segmentation_unionOfClumps, METH_VARARGS | METH_KEYWORDS,
"segmentation.union_of_clumps(input_imgs, output_img, gdalformat, no_data_val, add_to_rat)\n"
"The function takes the union of clumps images, combining them so all lines from all clumps are preserved in the new outputted clumps image.\n"
"\n"
":param input_imgs: is a list of input image paths\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param no_data_val: is None or float\n"
":param add_to_rat: is a boolean specifying whether the pixel values (from input_imgs) should be added as a RAT; column names have prefix 'ClumpVal_' with index starting at 1 for each variable.\n"
"\n"
},

    {"merge_segmentation_tiles", (PyCFunction)Segmentation_mergeSegmentationTiles, METH_VARARGS | METH_KEYWORDS,
"segmentation.merge_segmentation_tiles(input_imgs, output_img, border_msk_img, tile_boundary, tile_overlap, tile_body, col_name)\n"
"Merge body clumps from tile segmentations into output file\n"
"\n"
":param input_imgs: is a list of input image paths\n"
":param output_img: is a string containing the name of the output file\n"
":param border_msk_img: is a string containing the name of the border mask file\n"
":param tile_boundary: is an unsigned integer containing the tile boundary pixel value\n"
":param tile_overlap: is an unsigned integer containing the tile overlap pixel value\n"
":param tile_body: is an unsigned integer containing the tile body pixel value\n"
":param col_name: is a string containing the name of the object id column\n"
"\n"},

    {"merge_clump_images", (PyCFunction)Segmentation_mergeClumpImages, METH_VARARGS | METH_KEYWORDS,
"segmentation.merge_clump_images(input_imgs, output_img, merge_rats)\n"
"Merge all clumps from tile segmentations into output file\n"
"\n"
":param input_imgs: is a list of input image paths\n"
":param output_img: is a string containing the name of the output file\n"
":param merge_rats: is a boolean specifying with the image RATs are to merged (Default: false; Optional)\n"
"\n"},

    {"find_tile_borders_mask", (PyCFunction)Segmentation_findTileBordersMask, METH_VARARGS | METH_KEYWORDS,
"segmentation.find_tile_borders_mask(input_imgs, border_msk_img, tile_boundary, tile_overlap, tile_body, col_name)\n"
"Mask tile borders\n"
"\n"
":param input_imgs: is a list of input clump image paths\n"
":param border_msk_img: is a string containing the name of the border mask file\n"
":param tile_boundary: is an unsigned integer containing the tile boundary pixel value\n"
":param tile_overlap: is an unsigned integer containing the tile overlap pixel value\n"
":param tile_body: is an unsigned integer containing the tile body pixel value\n"
":param col_name: is a string containing the name of the object id column\n"

"\n"},

    {"rm_small_clumps", (PyCFunction)Segmentation_rmSmallClumps, METH_VARARGS | METH_KEYWORDS,
"segmentation.rm_small_clumps(clumps_img, output_img, area_threshold, gdalformat)\n"
"A function to remove small clumps and set them with a value of 0 (i.e., no data) \n"
"\n"
":param clumps_img: is a string containing the name of the input clumps file - note a column called \'Histogram\'.\n"
":param output_img: is a string containing the name of the output clumps file\n"
":param area_threshold: is a float containing the area threshold (in pixels)\n"
":param gdalformat: is a string defining the format of the output image.\n"
"\n"},

    {"mean_image", (PyCFunction)Segmentation_meanImage, METH_VARARGS | METH_KEYWORDS,
"segmentation.mean_image(input_img, clumps_img, output_img, gdalformat, datatype)\n"
"A function to generate an image where with the mean value for each clump. Primarily for visualisation and evaluating segmentation.\n"
"\n"
":param input_img: is a string containing the name of the input image file from which the mean is taken.\n"
":param clumps_img: is a string containing the name of the input clumps file\n"
":param output_img: is a string containing the name of the output image.\n"
":param gdalformat: is a string defining the format of the output image.\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
"\n"},

{"generate_regular_grid", (PyCFunction)Segmentation_GenerateRegularGrid, METH_VARARGS | METH_KEYWORDS,
"segmentation.generate_regular_grid(input_img, output_img, gdalformat, num_x_pxls, num_y_pxls, offset)\n"
"A function to generate an image where with the mean value for each clump. Primarily for visualisation and evaluating segmentation.\n"
"\n"
":param input_img: is a string containing the name of the input image file specifying the dimensions of the output image.\n"
":param output_img: is a string containing the name and path of the output clumps image\n"
":param gdalformat: is a string defining the format of the output image.\n"
":param num_x_pxls: is the size of the grid cells in the X axis in pixel units.\n"
":param num_y_pxls: is the size of the grid cells in the Y axis in pixel units.\n"
":param offset: is a boolean specifying whether the grid should be offset, i.e., starts half way point of numXPxls and numYPxls (Default is false; optional)"
"\n"},

{"include_regions_in_clumps", (PyCFunction)Segmentation_IncludeRegionsInClumps, METH_VARARGS | METH_KEYWORDS,
"segmentation.include_regions_in_clumps(clumps_img, regions_img, output_img, gdalformat)\n"
"A function to include a set of clumped regions within an existing clumps (i.e., segmentation) image.\n"
"NOTE. You should run the relabel_clumps function on the output of this command before using further.\n"
"\n"
":param clumps_img: is a string containing the filepath for the input clumps image.\n"
":param regions_img: is a string containing the filepath for the input regions image.\n"
":param output_img: is a string containing the name and path of the output clumps image\n"
":param gdalformat: is a string defining the format of the output image.\n"
"\n"},

{"merge_segments_to_neighbours", (PyCFunction)Segmentation_mergeSegments2Neighbours, METH_VARARGS | METH_KEYWORDS,
"segmentation.merge_segments_to_neighbours(clumps_img, input_vals_img, output_img, gdalformat, sel_clumps_col, no_data_clumps_col)\n"
"A function to merge some selected clumps with the neighbours based on colour (spectral) distance where clumps identified as no data are ignored.\n"
"\n"
":param clumps_img: is a string containing the filepath for the input clumps image.\n"
":param input_vals_img: is a string containing the filepath for the input image used to define 'distance'.\n"
":param output_img: is a string containing the name and path of the output clumps image\n"
":param gdalformat: is a string defining the format of the output image.\n"
":param sel_clumps_col: is a string defining the binary column for defining the segments to be merged (1 == selected clumps).\n"
":param no_data_clumps_col: is a string defining the binary column for defining the segments to be ignored as no data (1 == no-data clumps).\n"
"\n"},
    
{"drop_selected_clumps", (PyCFunction)Segmentation_dropSelectedSegments, METH_VARARGS | METH_KEYWORDS,
"segmentation.drop_selected_clumps(clumps_img, output_img, gdalformat, sel_clumps_col)\n"
"A function to drop the selected clumps from the segmentation.\n"
"\n"
":param clumps_img: is a string containing the filepath for the input clumps image.\n"
":param output_img: is a string containing the name and path of the output clumps image\n"
":param gdalformat: is a string defining the format of the output image.\n"
":param sel_clumps_col: is a string defining the binary column for defining the segments to be merged (1 == selected clumps).\n"
"\n"},
    
{"merge_equiv_clumps", (PyCFunction)Segmentation_mergeEquivalentClumps, METH_VARARGS | METH_KEYWORDS,
"segmentation.merge_equiv_clumps(clumps_img, output_img, gdalformat, val_columns)\n"
"A function to merge neighbouring clumps which have the same value - for example when merging across tile boundaries.\n"
"\n"
":param clumps_img: is a string containing the filepath for the input clumps image.\n"
":param output_img: is a string containing the name and path of the output clumps image\n"
":param gdalformat: is a string defining the format of the output image.\n"
":param val_columns: is a list of strings defining the value(s) used to define equivalence (typically it might be the original pixel values when clumping through tiling).\n"
"\n"},

    {nullptr}        /* Sentinel */
};
                                
#if PY_MAJOR_VERSION >= 3

static int Segmentation_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int Segmentation_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_segmentation",
        nullptr,
        sizeof(struct SegmentationState),
        SegmentationMethods,
        nullptr,
        Segmentation_traverse,
        Segmentation_clear,
        nullptr
};

#define INITERROR return nullptr

PyMODINIT_FUNC 
PyInit__segmentation(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_segmentation(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_segmentation", SegmentationMethods);
#endif
    if( pModule == nullptr )
        INITERROR;

    struct SegmentationState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_segmentation.error", nullptr, nullptr);
    if( state->error == nullptr )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
