/*
 *  imageutils.cpp
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
#include "cmds/RSGISCmdImageUtils.h"
#include <vector>

/* An exception object for this module */
/* created in the init function */
struct ImageUtilsState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ImageUtilsState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ImageUtilsState _state;
#endif

// Helper function to extract python sequence to array of strings
static std::string* ExtractStringArrayFromSequence(PyObject *sequence, int *nElements)
{
    Py_ssize_t nFields = PySequence_Size(sequence);
    *nElements = nFields;
    if(nFields == 0)
    {
        return nullptr;
    }
    std::string *stringsArray = new std::string[nFields];

    for(int i = 0; i < nFields; ++i)
    {
        PyObject *stringObj = PySequence_GetItem(sequence, i);
        
        if(!RSGISPY_CHECK_STRING(stringObj))
        {
            std::cout << "Error: Sequence fields must be strings\n";
            PyErr_SetString(GETSTATE(sequence)->error, "Fields must be strings");
            Py_DECREF(stringObj);
            delete[] stringsArray;
            *nElements = 0;
            return nullptr;
        }

        stringsArray[i] = RSGISPY_STRING_EXTRACT(stringObj);
        
        Py_DECREF(stringObj);
    }

    return stringsArray;
}

static std::vector<std::string> ExtractStringVectorFromSequence(PyObject *sequence)
{
    int numVals = 0;
    std::string *stringsArray = ExtractStringArrayFromSequence(sequence, &numVals);
    std::vector<std::string> stringsVec = std::vector<std::string>();
    stringsVec.reserve(numVals);
    
    for(int i = 0; i < numVals; ++i)
    {
        stringsVec.push_back(stringsArray[i]);
    }
    delete[] stringsArray;
    
    return stringsVec;
}

/*
// Helper function to extract python sequence to array of integers
static std::vector<int> ExtractIntVectorFromSequence(PyObject *sequence)
{
    Py_ssize_t nFields = PySequence_Size(sequence);
    std::vector<int> intVals;
    intVals.reserve(nFields);

    for(int i = 0; i < nFields; ++i)
    {
        PyObject *intObj = PySequence_GetItem(sequence, i);

        if(!RSGISPY_CHECK_INT(intObj))
        {
            Py_DECREF(intObj);
            PyErr_SetString(GETSTATE(sequence)->error, "Fields must be integers");
        }

        intVals.push_back(RSGISPY_INT_EXTRACT(intObj));
        Py_DECREF(intObj);
    }

    return intVals;
}
*/

static PyObject *ImageUtils_StretchImage(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("save_stats"),
                             RSGIS_PY_C_TEXT("out_stats_file"), RSGIS_PY_C_TEXT("no_data_val"), RSGIS_PY_C_TEXT("one_pass_std"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("stretch_type"),
                             RSGIS_PY_C_TEXT("stretch_param"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat, *pszOutStatsFile;
    int saveOutStats, onePassSD;
    int nOutDataType, nStretchType;
    float fStretchParam = 2.0;
    float inNoData = 0.0;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssisfisii|f:stretch_img", kwlist, &pszInputImage, &pszOutputFile,
                                     &saveOutStats, &pszOutStatsFile, &inNoData, &onePassSD, &pszGDALFormat,
                                     &nOutDataType, &nStretchType, &fStretchParam))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeStretchImageNoData(pszInputImage, pszOutputFile, inNoData, saveOutStats,
                                               pszOutStatsFile, onePassSD, pszGDALFormat,
                                               (rsgis::RSGISLibDataType)nOutDataType, (rsgis::cmds::RSGISStretches)nStretchType, fStretchParam);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_StretchImageWithStats(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("in_stats_file"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("no_data_val"),
                             RSGIS_PY_C_TEXT("stretch_type"), RSGIS_PY_C_TEXT("stretch_param"), nullptr};
    
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat, *pszInStatsFile;
    int nOutDataType, nStretchType;
    float fStretchParam = 2.0;
    float nodataval = 0.0;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssifi|f:stretch_img_with_stats", kwlist, &pszInputImage, &pszOutputFile,
                                     &pszInStatsFile, &pszGDALFormat, &nOutDataType, &nodataval, &nStretchType, &fStretchParam))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeStretchImageWithStatsNoData(pszInputImage, pszOutputFile, pszInStatsFile,
                                                        pszGDALFormat, (rsgis::RSGISLibDataType)nOutDataType,
                                                        (rsgis::cmds::RSGISStretches)nStretchType, fStretchParam, nodataval);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_NormaliseImagePxlVals(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("in_no_data_val"), RSGIS_PY_C_TEXT("out_no_data_val"),
                             RSGIS_PY_C_TEXT("out_min"), RSGIS_PY_C_TEXT("out_max"),
                             RSGIS_PY_C_TEXT("stretch_type"), RSGIS_PY_C_TEXT("stretch_param"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    int nOutDataType, nStretchType;
    float fStretchParam = 2.0;
    float inNoDataVal = 0;
    float outNoDataVal = -1;
    float outMinVal = 0;
    float outMaxVal = 1;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssiffffi|f:normalise_img_pxl_vals", kwlist, &pszInputImage, &pszOutputFile,
                                     &pszGDALFormat, &nOutDataType, &inNoDataVal, &outNoDataVal, &outMinVal, &outMaxVal, &nStretchType, &fStretchParam))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeNormaliseImgPxlVals(std::string(pszInputImage), std::string(pszOutputFile),
                                                std::string(pszGDALFormat), (rsgis::RSGISLibDataType)nOutDataType, inNoDataVal,
                                                outNoDataVal, outMinVal, outMaxVal, (rsgis::cmds::RSGISStretches)nStretchType, fStretchParam);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_maskImage(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("in_msk_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("out_value"),
                             RSGIS_PY_C_TEXT("mask_value"),  nullptr};
    const char *pszInputImage, *pszImageMask, *pszOutputImage, *pszGDALFormat;
    int nDataType;
    float outValue;
    PyObject *maskValueObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssifO:mask_img", kwlist, &pszInputImage, &pszImageMask, &pszOutputImage, &pszGDALFormat, &nDataType, &outValue, &maskValueObj ))
    {
        return nullptr;
    }

    std::vector<float> maskValues;
    if( !PySequence_Check(maskValueObj))
    {
        if(RSGISPY_CHECK_FLOAT(maskValueObj) || RSGISPY_CHECK_INT(maskValueObj))
        {
            float val = RSGISPY_FLOAT_EXTRACT(maskValueObj);
            maskValues.push_back(val);
        }
        else
        {
            PyErr_SetString(GETSTATE(self)->error, "Mask value must be numeric or a list of numeric.");
            return nullptr;
        }
    }
    else
    {
        Py_ssize_t numMaskVals = PySequence_Size(maskValueObj);
        for( Py_ssize_t n = 0; n < numMaskVals; n++ )
        {
            PyObject *o = PySequence_GetItem(maskValueObj, n);
            if(RSGISPY_CHECK_FLOAT(o) || RSGISPY_CHECK_INT(o))
            {
                float val = RSGISPY_FLOAT_EXTRACT(o);
                maskValues.push_back(val);
            }
            else
            {
                PyErr_SetString(GETSTATE(self)->error, "Mask value must be numeric or a list of numeric.");
                return nullptr;
            }
        }
    }
    
    try
    {
        rsgis::cmds::executeMaskImage(pszInputImage, pszImageMask, pszOutputImage, pszGDALFormat, (rsgis::RSGISLibDataType)nDataType, outValue, maskValues);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_createTiles(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("out_img_base"),
                             RSGIS_PY_C_TEXT("tile_width"), RSGIS_PY_C_TEXT("tile_height"),
                             RSGIS_PY_C_TEXT("tile_overlap"), RSGIS_PY_C_TEXT("offset_tiles"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("out_img_ext"), nullptr};
    const char *pszInputImage, *pszImageBase, *pszGDALFormat, *pszExt = "";
    unsigned int imgWidth, imgHeight, imgTileOverlap = 0;
    int offsetTiling = false;
    int nDataType;
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssIIIisis:create_tiles", kwlist, &pszInputImage, &pszImageBase, &imgWidth, &imgHeight, &imgTileOverlap, &offsetTiling, &pszGDALFormat, &nDataType, &pszExt))
    {
        return nullptr;
    }
    
    PyObject *pOutList;
    try
    {
        std::vector<std::string> outFileNames;
        rsgis::cmds::executeCreateTiles(pszInputImage, pszImageBase, imgWidth, imgHeight, imgTileOverlap, offsetTiling, pszGDALFormat, (rsgis::RSGISLibDataType)nDataType, pszExt, &outFileNames);
        
        pOutList = PyList_New(outFileNames.size());
        Py_ssize_t nIndex = 0;
        for( auto itr = outFileNames.begin(); itr != outFileNames.end(); itr++)
        {
            PyObject *pVal = RSGISPY_CREATE_STRING((*itr).c_str());
            PyList_SetItem(pOutList, nIndex, pVal ); // steals a reference
            nIndex++;
        }
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    return pOutList;
}

static PyObject *ImageUtils_createImageMosaic(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("background_val"), RSGIS_PY_C_TEXT("skip_val"),
                             RSGIS_PY_C_TEXT("skip_band"), RSGIS_PY_C_TEXT("overlap_behaviour"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"), nullptr};
    const char *pszOutputImage, *pszGDALFormat;
    float backgroundVal, skipVal;
    int skipBand, nDataType, overlapBehaviour;
    PyObject *pInputImages; // List of input images

    // Check parameters are present and of correct type
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "Osffiisi:create_img_mosaic", kwlist, &pInputImages, &pszOutputImage,
                                &backgroundVal, &skipVal, &skipBand, &overlapBehaviour,&pszGDALFormat, &nDataType))
    {
        return nullptr;
    }

    // TODO: Look into this function - doesn't seem to catch when only a single image is provided.
    if(!PySequence_Check(pInputImages))
    {
        PyErr_SetString(GETSTATE(self)->error, "First argument must be a sequence");
        return nullptr;
    }

    // Extract list of images to array of strings.
    int numImages = 0;
    std::string *inputImages = ExtractStringArrayFromSequence(pInputImages, &numImages);
    if(numImages == 0) 
    { 
        PyErr_SetString(GETSTATE(self)->error, "No input images provided");
        return nullptr; 
    }
    
    try
    {
        rsgis::cmds::executeImageMosaic(inputImages, numImages, pszOutputImage, backgroundVal, 
                    skipVal, skipBand-1, overlapBehaviour, pszGDALFormat, (rsgis::RSGISLibDataType)nDataType);

    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_IncludeImages(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("include_imgs"),
                             RSGIS_PY_C_TEXT("input_bands"), RSGIS_PY_C_TEXT("skip_val"), nullptr};
    const char *pszBaseImage;
    PyObject *pInputImages; // List of input images
    PyObject *pInputBands = Py_None; // List of bands
    PyObject *pSkipVal = Py_None;

    // Check parameters are present and of correct type
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sO|OO:include_imgs", kwlist, &pszBaseImage, &pInputImages, &pInputBands, &pSkipVal))
        return nullptr;

    // TODO: Look into this function - doesn't seem to catch when only a single image is provided.
    if(!PySequence_Check(pInputImages))
    {
        PyErr_SetString(GETSTATE(self)->error, "Second argument must be a list of images");
        return nullptr;
    }

    // Extract list of images to array of strings.
    int numImages = 0;
    std::string *inputImages = ExtractStringArrayFromSequence(pInputImages, &numImages);
    if(numImages == 0) 
    { 
        PyErr_SetString(GETSTATE(self)->error, "No input images provided");
        return nullptr; 
    }

    // Extract bands to an array (if using)
    bool bandsDefined = false;
    std::vector<int> imgBands;
    if(pInputBands != Py_None)
    {
        bandsDefined = true;
        Py_ssize_t nFields = PySequence_Size(pInputBands);
        
        for(int i = 0; i < nFields; ++i)
        {
            PyObject *intObj = PySequence_GetItem(pInputBands, i);
            
            if(!RSGISPY_CHECK_INT(intObj))
            {
                PyErr_SetString(GETSTATE(pInputBands)->error, "Bands must be integers");
                Py_DECREF(intObj);
                return nullptr;
            }
            
            imgBands.push_back(RSGISPY_INT_EXTRACT(intObj));
            Py_DECREF(intObj);
        }
    }
    
    bool useSkipVal = false;
    float skipVal = 0.0;
    if(pSkipVal != Py_None)
    {
        useSkipVal = true;
        if(!RSGISPY_CHECK_FLOAT(pSkipVal))
        {
            PyErr_SetString(GETSTATE(pInputBands)->error, "Skip value must be a float.");
            Py_DECREF(pSkipVal);
            return nullptr;
        }
        
        skipVal = RSGISPY_FLOAT_EXTRACT(pSkipVal);
    }
    
    try
    {
        rsgis::cmds::executeImageInclude(inputImages, numImages, pszBaseImage, bandsDefined, imgBands, skipVal, useSkipVal);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_IncludeImagesOverlap(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("include_imgs"),
                             RSGIS_PY_C_TEXT("overlap"), nullptr};
    const char *pszBaseImage;
    PyObject *pInputImages; // List of input images
    int pxlOverlap = 0;
    
    // Check parameters are present and of correct type
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sOi:include_imgs_with_overlap", kwlist, &pszBaseImage, &pInputImages, &pxlOverlap))
    {
        return nullptr;
    }
    
    // TODO: Look into this function - doesn't seem to catch when only a single image is provided.
    if(!PySequence_Check(pInputImages))
    {
        PyErr_SetString(GETSTATE(self)->error, "Second argument must be a list of images");
        return nullptr;
    }
    
    // Extract list of images to array of strings.
    int numImages = 0;
    std::string *inputImages = ExtractStringArrayFromSequence(pInputImages, &numImages);
    if(numImages == 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "No input images provided");
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageIncludeOverlap(inputImages, numImages, pszBaseImage, pxlOverlap);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_IncludeImagesIndImgIntersect(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("include_imgs"), nullptr};
    const char *pszBaseImage;
    PyObject *pInputImages; // List of input images
    
    // Check parameters are present and of correct type
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sO:include_imgs_ind_img_intersect", kwlist, &pszBaseImage, &pInputImages))
    {
        return nullptr;
    }
    
    // TODO: Look into this function - doesn't seem to catch when only a single image is provided.
    if(!PySequence_Check(pInputImages))
    {
        PyErr_SetString(GETSTATE(self)->error, "Second argument must be a list of images");
        return nullptr;
    }
    
    // Extract list of images to array of strings.
    int numImages = 0;
    std::string *inputImages = ExtractStringArrayFromSequence(pInputImages, &numImages);
    if(numImages == 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "No input images provided");
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageIncludeIndImgIntersect(inputImages, numImages, pszBaseImage);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_PopImageStats(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage;
    int useNoDataValue = true;
    int buildPyramids = true;
    float noDataValue = 0;
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("use_no_data"),
                             RSGIS_PY_C_TEXT("no_data_val"), RSGIS_PY_C_TEXT("calc_pyramids"), nullptr};

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "s|ifi:pop_img_stats", kwlist, &pszInputImage,
                    &useNoDataValue, &noDataValue, &buildPyramids))
    {
        return nullptr;
    }
    
    std::vector<int> pyraScaleVals;
    
    try
    {
        rsgis::cmds::executePopulateImgStats(pszInputImage, useNoDataValue, noDataValue, buildPyramids, pyraScaleVals);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_AssignProj(PyObject *self, PyObject *args, PyObject *keywds)
{
    // TODO REPLACE WITH PURE PYTHON FUNCTION. ADD VERSIONS FOR WKT, PROJ4 and EPSG.
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("wkt_str"), RSGIS_PY_C_TEXT("wkt_file"), nullptr};
    const char *pszInputImage;
    std::string pszInputProj = "";
    std::string pszInputProjFile = "";
    bool readWKTFromFile = false;
    PyObject *pszInputProjObj = Py_None;
    PyObject *pszInputProjFileObj = Py_None;
    

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "s|OO:assign_wkt_proj", kwlist, &pszInputImage, &pszInputProjObj, &pszInputProjFileObj))
    {
        return nullptr;
    }
    
    if(pszInputProjObj == Py_None)
    {
        pszInputProj = "";
        if(pszInputProjFileObj == Py_None)
        {
            pszInputProjFile = "";
            PyErr_SetString(GETSTATE(self)->error, "Must specify either a wkt string or a file from which it can be read." );
            return nullptr;
        }
        else
        {
            readWKTFromFile = true;
            pszInputProjFile = RSGISPY_STRING_EXTRACT(pszInputProjFileObj);
        }
    }
    else
    {
        pszInputProj = RSGISPY_STRING_EXTRACT(pszInputProjObj);
        readWKTFromFile = false;
    }
    
    try
    {
        rsgis::cmds::executeAssignProj(pszInputImage, pszInputProj, readWKTFromFile, pszInputProjFile);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_CopyProjFromImage(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage;
    const char *pszInputRefImage;
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("ref_img"), nullptr};
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ss:copy_proj_from_img", kwlist, &pszInputImage, &pszInputRefImage))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeCopyProj(pszInputImage, pszInputRefImage);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_CopySpatialAndProjFromImage(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage;
    const char *pszInputRefImage;
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("ref_img"), nullptr};
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ss:copy_spatial_and_proj_from_img", kwlist, &pszInputImage, &pszInputRefImage))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeCopyProjSpatial(pszInputImage, pszInputRefImage);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_AssignSpatialInfo(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage;
    PyObject *xTLObj;
    PyObject *yTLObj;
    PyObject *xResObj;
    PyObject *yResObj;
    PyObject *xRotObj;
    PyObject *yRotObj;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("tl_x"), RSGIS_PY_C_TEXT("tl_y"),
                             RSGIS_PY_C_TEXT("res_x"), RSGIS_PY_C_TEXT("res_y"), RSGIS_PY_C_TEXT("rot_x"),
                             RSGIS_PY_C_TEXT("rot_y"), nullptr};
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sOOOOOO:assign_spatial_info", kwlist, &pszInputImage, &xTLObj, &yTLObj, &xResObj, &yResObj, &xRotObj, &yRotObj))
    {
        return nullptr;
    }
    
    
    double xTL = 0.0;
    double yTL = 0.0;
    double xRes = 0.0;
    double yRes = 0.0;
    double xRot = 0.0;
    double yRot = 0.0;
    
    bool xTLDef = false;
    bool yTLDef = false;
    bool xResDef = false;
    bool yResDef = false;
    bool xRotDef = false;
    bool yRotDef = false;
    
    if(xTLObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(xTLObj) | RSGISPY_CHECK_INT(xTLObj))
        {
            xTL = RSGISPY_FLOAT_EXTRACT(xTLObj);
            xTLDef = true;
        }
    }
    
    if(yTLObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(yTLObj) | RSGISPY_CHECK_INT(yTLObj))
        {
            yTL = RSGISPY_FLOAT_EXTRACT(yTLObj);
            yTLDef = true;
        }
    }
    
    if(xResObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(xResObj) | RSGISPY_CHECK_INT(xResObj))
        {
            xRes = RSGISPY_FLOAT_EXTRACT(xResObj);
            xResDef = true;
        }
    }
    
    if(yResObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(yResObj) | RSGISPY_CHECK_INT(yResObj))
        {
            yRes = RSGISPY_FLOAT_EXTRACT(yResObj);
            yResDef = true;
        }
    }
    
    if(xRotObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(xRotObj) | RSGISPY_CHECK_INT(xRotObj))
        {
            xRot = RSGISPY_FLOAT_EXTRACT(xRotObj);
            xRotDef = true;
        }
    }
    
    if(yRotObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(yRotObj) | RSGISPY_CHECK_INT(yRotObj))
        {
            yRot = RSGISPY_FLOAT_EXTRACT(yRotObj);
            yRotDef = true;
        }
    }
    
    try
    {
        rsgis::cmds::executeAssignSpatialInfo(pszInputImage, xTL, yTL, xRes, yRes, xRot, yRot, xTLDef, yTLDef, xResDef, yResDef, xRotDef, yRotDef);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyObject *ImageUtils_SelectImageBands(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("bands"), nullptr};
    const char *pszInputImage;
    const char *pszOutputFile;
    const char *pszGDALFormat;
    int nDataType;
    PyObject *pImageBands = nullptr;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssiO:select_img_bands", kwlist, &pszInputImage, &pszOutputFile, &pszGDALFormat, &nDataType, &pImageBands))
    {
        return nullptr;
    }
    
    if(!PySequence_Check(pImageBands))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence of image bands (int)");
        return nullptr;
    }
    
    std::vector<unsigned int> imgBands;
    Py_ssize_t nFields = PySequence_Size(pImageBands);
    
    for(int i = 0; i < nFields; ++i)
    {
        PyObject *intObj = PySequence_GetItem(pImageBands, i);
        
        if(!RSGISPY_CHECK_INT(intObj))
        {
            PyErr_SetString(GETSTATE(pImageBands)->error, "Fields must be integers");
            Py_DECREF(intObj);
            return nullptr;
        }
        
        imgBands.push_back(RSGISPY_INT_EXTRACT(intObj));
        Py_DECREF(intObj);
    }
    
    try
    {
        rsgis::cmds::executeSubsetImageBands(std::string(pszInputImage), std::string(pszOutputFile),
                                             imgBands, std::string(pszGDALFormat), (rsgis::RSGISLibDataType)nDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_Subset(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("vec_file"),
                             RSGIS_PY_C_TEXT("vec_lyr"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"), nullptr};
    const char *pszInputImage, *pszInputVectorFile, *pszInputVectorLyr, *pszOutputImage, *pszGDALFormat;
    int nOutDataType;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssssi:subset", kwlist, &pszInputImage, &pszInputVectorFile, &pszInputVectorLyr, &pszOutputImage, &pszGDALFormat, &nOutDataType))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeSubset(std::string(pszInputImage), std::string(pszInputVectorFile),
                                   std::string(pszInputVectorLyr), std::string(pszOutputImage),
                                   std::string(pszGDALFormat), (rsgis::RSGISLibDataType)nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_SubsetBBox(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("min_x"), RSGIS_PY_C_TEXT("max_x"),
                             RSGIS_PY_C_TEXT("min_y"), RSGIS_PY_C_TEXT("max_y"), nullptr};

    const char *pszInputImage, *pszOutputImage, *pszGDALFormat;
    int nOutDataType;
    double xMin, xMax, yMin, yMax = 0.0;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssidddd:subset_bbox", kwlist, &pszInputImage, &pszOutputImage, &pszGDALFormat, &nOutDataType, &xMin, &xMax, &yMin, &yMax))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeSubsetBBox(pszInputImage, pszOutputImage, pszGDALFormat, (rsgis::RSGISLibDataType)nOutDataType, xMin, xMax, yMin, yMax);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_Subset2Img(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("in_roi_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    const char *pszInputImage, *pszInputROI, *pszOutputImage, *pszGDALFormat;
    int nOutDataType;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssi:subset_to_img", kwlist, &pszInputImage, &pszInputROI, &pszOutputImage, &pszGDALFormat, &nOutDataType))
        return nullptr;
    
    try
    {
        rsgis::cmds::executeSubset2Img(std::string(pszInputImage), std::string(pszInputROI),
                                       std::string(pszOutputImage), std::string(pszGDALFormat),
                                       (rsgis::RSGISLibDataType)nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_StackImageBands(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("band_names"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("skip_value"),
                             RSGIS_PY_C_TEXT("no_data_val"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    const char *pszOutputFile;
    const char *pszGDALFormat;
    int nDataType;
    float noDataValue;
    PyObject *skipValueObj = nullptr;
    PyObject *pInputImages = nullptr;
    PyObject *pimageBandNames = nullptr;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "OOsOfsi:stack_img_bands", kwlist, &pInputImages, &pimageBandNames,
                                     &pszOutputFile, &skipValueObj, &noDataValue, &pszGDALFormat, &nDataType))
    {
        return nullptr;
    }
    
    bool skipPixels = false;
    float skipValue = 0.0;
    if(skipValueObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(skipValueObj) | RSGISPY_CHECK_INT(skipValueObj))
        {
            skipValue = RSGISPY_FLOAT_EXTRACT(skipValueObj);
            skipPixels = true;
        }
    }
    
    if(!PySequence_Check(pInputImages))
    {
        PyErr_SetString(GETSTATE(self)->error, "First argument must be a sequence with a list of band names.");
        return nullptr;
    }
    
    // Extract list of images to array of strings.
    int numImages = 0;
    std::string *inputImages = ExtractStringArrayFromSequence(pInputImages, &numImages);
    if(numImages == 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "No input images were provided");
        return nullptr;
    }
    
    bool replaceBandNames = false;
    std::string *imageBandNames = nullptr;
    if(PySequence_Check(pimageBandNames))
    {
        // Extract list of images to array of strings.
        int numBandName = 0;
        imageBandNames = ExtractStringArrayFromSequence(pimageBandNames, &numBandName);
        if(numBandName == 0)
        {
            replaceBandNames = false;
        }
        else if(numBandName != numImages)
        {
            PyErr_SetString(GETSTATE(self)->error, "The number of band names must match the number of input images.");
            return nullptr;
        }
        else
        {
            replaceBandNames = true;
        }
    }
    
    try
    {
        rsgis::cmds::executeStackImageBands(inputImages, imageBandNames, numImages, std::string(pszOutputFile),
                                            skipPixels, skipValue, noDataValue, std::string(pszGDALFormat),
                                            (rsgis::RSGISLibDataType)nDataType, replaceBandNames);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyObject *ImageUtils_CreateBlankImage(PyObject *self, PyObject *args, PyObject *keywds)
{
    // TODO specify projection using EPSG code or wkt string or proj4
    static char *kwlist[] = {RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("n_bands"),
                             RSGIS_PY_C_TEXT("width"), RSGIS_PY_C_TEXT("height"),
                             RSGIS_PY_C_TEXT("tl_x"), RSGIS_PY_C_TEXT("tl_y"),
                             RSGIS_PY_C_TEXT("res_x"), RSGIS_PY_C_TEXT("res_y"),
                             RSGIS_PY_C_TEXT("pxl_val"), RSGIS_PY_C_TEXT("wkt_file"),
                             RSGIS_PY_C_TEXT("wkt_str"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};

    const char *pszOutputImage, *pszGDALFormat, *wktFile, *wktString;
    int nOutDataType;
    unsigned int numBands, width, height = 0;
    double tlX, tlY, res_x, res_y = 0;
    float pxlVal = 0;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sIIIddddfsssi:create_blank_img", kwlist, &pszOutputImage, &numBands, &width,
                                     &height, &tlX, &tlY, &res_x, &res_y, &pxlVal, &wktFile, &wktString, &pszGDALFormat, &nOutDataType))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeCreateBlankImage(std::string(pszOutputImage), numBands, width, height, tlX, tlY,
                                             res_x, res_y, pxlVal, std::string(wktFile), std::string(wktString),
                                             std::string(pszGDALFormat), (rsgis::RSGISLibDataType)nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_CreateCopyImage(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage, *pszGDALFormat;
    int nOutDataType;
    unsigned int numBands;
    float pxlVal = 0;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("n_bands"), RSGIS_PY_C_TEXT("pxl_val"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"), nullptr};
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssIfsi:create_copy_img", kwlist, &pszInputImage, &pszOutputImage,
                                     &numBands, &pxlVal, &pszGDALFormat, &nOutDataType))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeCreateCopyBlankImage(std::string(pszInputImage), std::string(pszOutputImage),
                                                 numBands, pxlVal, std::string(pszGDALFormat), (rsgis::RSGISLibDataType)nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_CreateCopyImageDefExtent(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage, *pszGDALFormat;
    int nOutDataType;
    unsigned int numBands;
    float pxlVal = 0;
    double xMin, xMax, yMin, yMax, xRes, yRes = 0.0;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("n_bands"), RSGIS_PY_C_TEXT("pxl_val"),
                             RSGIS_PY_C_TEXT("min_x"), RSGIS_PY_C_TEXT("max_x"),
                             RSGIS_PY_C_TEXT("min_y"), RSGIS_PY_C_TEXT("max_y"),
                             RSGIS_PY_C_TEXT("res_x"), RSGIS_PY_C_TEXT("res_y"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"), nullptr};
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssIfddddddsi:create_copy_img_def_extent", kwlist, &pszInputImage,
                                     &pszOutputImage, &numBands, &pxlVal, &xMin, &xMax, &yMin, &yMax, &xRes, &yRes,
                                     &pszGDALFormat, &nOutDataType))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeCreateCopyBlankDefExtImage(std::string(pszInputImage), std::string(pszOutputImage),
                                                 numBands, xMin, xMax, yMin, yMax, xRes, yRes, pxlVal,
                                                 std::string(pszGDALFormat), (rsgis::RSGISLibDataType)nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_CreateCopyImageVecExtent(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage, *pszInputVectorFile, *pszInputVectorLyr, *pszGDALFormat;
    int nOutDataType;
    unsigned int numBands;
    float pxlVal = 0;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("vec_file"),
                             RSGIS_PY_C_TEXT("vec_lyr"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("n_bands"), RSGIS_PY_C_TEXT("pxl_val"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"), nullptr};
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssIfsi:create_copy_img_vec_extent", kwlist, &pszInputImage, &pszInputVectorFile, &pszInputVectorLyr,
                                     &pszOutputImage, &numBands, &pxlVal, &pszGDALFormat, &nOutDataType))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeCreateCopyBlankImageVecExtent(std::string(pszInputImage), std::string(pszInputVectorFile),
                                                          std::string(pszInputVectorLyr), std::string(pszOutputImage),
                                                          numBands, pxlVal, std::string(pszGDALFormat),
                                                          (rsgis::RSGISLibDataType)nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_OrderImagesUsingPropValidData(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("no_data_val"), nullptr};
    float noDataValue;
    PyObject *pInputImages; // List of input images
    
    // Check parameters are present and of correct type
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "Of:order_img_using_prop_valid_pxls", kwlist, &pInputImages, &noDataValue))
    {
        return nullptr;
    }
    
    // TODO: Look into this function - doesn't seem to catch when only a single image is provided.
    if(!PySequence_Check(pInputImages)) {
        PyErr_SetString(GETSTATE(self)->error, "First argument must be a sequence");
        return nullptr;
    }
    
    // Extract list of images to array of strings.
    std::vector<std::string> inputImages = ExtractStringVectorFromSequence(pInputImages);
    int numImages = inputImages.size();
    if(numImages == 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "No input images provided");
        return nullptr;
    }

    PyObject *outImagesList = nullptr;
    try
    {
        std::vector<std::string> orderedInputImages = rsgis::cmds::executeOrderImageUsingValidDataProp(inputImages, noDataValue);
        
        outImagesList = PyTuple_New(orderedInputImages.size());
        
        if(outImagesList == nullptr)
        {
            throw rsgis::cmds::RSGISCmdException("Could not create a python list...");
        }
        
        for(unsigned int i = 0; i < orderedInputImages.size(); ++i)
        {
            //std::cout << i << " = " << orderedInputImages.at(i) << std::endl;
            if(PyTuple_SetItem(outImagesList, i, Py_BuildValue("s", orderedInputImages.at(i).c_str())) == -1)
            {
                throw rsgis::cmds::RSGISCmdException("Failed to add a value to the list...");
            }
        }
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    return outImagesList;
}


static PyObject *ImageUtils_GenSamplingGrid(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("pxl_res"),
                             RSGIS_PY_C_TEXT("min_val"), RSGIS_PY_C_TEXT("max_val"),
                             RSGIS_PY_C_TEXT("single_line"), nullptr};
    const char *pszInputImage, *pszOutputImage, *pszGDALFormat;
    float pxlRes = 0.0;
    int minVal = 0;
    int maxVal = 1;
    int singleLine = false;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssfiii:gen_sampling_grid", kwlist, &pszInputImage, &pszOutputImage,
                                     &pszGDALFormat, &pxlRes, &minVal, &maxVal, &singleLine))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeProduceRegularGridImage(std::string(pszInputImage), std::string(pszOutputImage),
                                                    std::string(pszGDALFormat), pxlRes, minVal, maxVal, (bool)singleLine);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyObject *ImageUtils_GenFiniteMask(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInputImage = "";
    const char *pszOutputImage = "";
    const char *pszGDALFormat = "";
    
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sss:gen_finite_mask", kwlist, &pszInputImage, &pszOutputImage, &pszGDALFormat))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeFiniteImageMask(std::string(pszInputImage), std::string(pszOutputImage), std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_GenValidMask(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("no_data_val"), nullptr};
    PyObject *pInputImages;
    const char *pszOutputImage = "";
    const char *pszGDALFormat = "";
    float noDataVal = 0.0;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "Oss|f:gen_valid_mask", kwlist, &pInputImages, &pszOutputImage, &pszGDALFormat, &noDataVal))
    {
        return nullptr;
    }
    
    std::vector<std::string> inputImages;
    if(RSGISPY_CHECK_STRING(pInputImages))
    {
        inputImages.push_back(RSGISPY_STRING_EXTRACT(pInputImages));
    }
    else if(PySequence_Check(pInputImages))
    {
        Py_ssize_t nImages = PySequence_Size(pInputImages);
        inputImages.reserve(nImages);
        for( Py_ssize_t n = 0; n < nImages; n++ )
        {
            PyObject *o = PySequence_GetItem(pInputImages, n);
            
            if(!RSGISPY_CHECK_STRING(o))
            {
                PyErr_SetString(GETSTATE(self)->error, "Input images must be strings");
                Py_DECREF(o);
                return nullptr;
            }
            
            inputImages.push_back(RSGISPY_STRING_EXTRACT(o));
        }
    }
    else
    {
        PyErr_SetString(GETSTATE(self)->error, "Input images must be a sequence or string");
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeValidImageMask(inputImages, std::string(pszOutputImage), std::string(pszGDALFormat), noDataVal);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_GenImageEdgeMask(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("n_edge_pxls"), nullptr};
    const char *pInputImage = "";
    const char *pszOutputImage = "";
    const char *pszGDALFormat = "";
    unsigned int nEdgePixels = 0.0;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssI:gen_img_edge_mask", kwlist, &pInputImage, &pszOutputImage, &pszGDALFormat, &nEdgePixels))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeImageEdgeMask(std::string(pInputImage), std::string(pszOutputImage), std::string(pszGDALFormat), nEdgePixels);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_CombineImages2Band(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("no_data_val"), nullptr};
    PyObject *pInputImages;
    const char *pszOutputImage = "";
    const char *pszGDALFormat = "";
    int nDataType;
    float noDataVal = 0.0;
    
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "Ossi|f:combine_imgs_to_band", kwlist, &pInputImages, &pszOutputImage, &pszGDALFormat, &nDataType, &noDataVal))
    {
        return nullptr;
    }
    
    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
    
    if( !PySequence_Check(pInputImages))
    {
        PyErr_SetString(GETSTATE(self)->error, "Input images must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nImages = PySequence_Size(pInputImages);
    std::vector<std::string> inputImages;
    inputImages.reserve(nImages);
    for( Py_ssize_t n = 0; n < nImages; n++ )
    {
        PyObject *o = PySequence_GetItem(pInputImages, n);
        
        if(!RSGISPY_CHECK_STRING(o))
        {
            PyErr_SetString(GETSTATE(self)->error, "Input images must be strings");
            Py_DECREF(o);
            return nullptr;
        }
        
        inputImages.push_back(RSGISPY_STRING_EXTRACT(o));
    }
    
    try
    {
        rsgis::cmds::executeCombineImagesSingleBandIgnoreNoData(inputImages, std::string(pszOutputImage), noDataVal, std::string(pszGDALFormat), type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_PerformRandomPxlSample(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("mask_vals"),
                             RSGIS_PY_C_TEXT("n_samples"), nullptr};
    const char *pszInputImage = "";
    const char *pszOutputImage = "";
    const char *pszGDALFormat = "";
    PyObject *maskValsObj;
    unsigned int numSamples = 0;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssOI:perform_random_pxl_sample_in_mask", kwlist, &pszInputImage, &pszOutputImage, &pszGDALFormat, &maskValsObj, &numSamples))
    {
        return nullptr;
    }
    
    std::vector<int> maskVals;
    
    if(!PySequence_Check(maskValsObj))
    {
        if(RSGISPY_CHECK_INT(maskValsObj))
        {
            maskVals.push_back(RSGISPY_INT_EXTRACT(maskValsObj));
        }
        else
        {
            PyErr_SetString(GETSTATE(self)->error, "Mask value was not a sequence but a single value, however that value was not an integer.");
            return nullptr;
        }
    }
    else
    {
        Py_ssize_t nMaskVals = PySequence_Size(maskValsObj);
        for( Py_ssize_t n = 0; n < nMaskVals; n++ )
        {
            PyObject *o = PySequence_GetItem(maskValsObj, n);
            if(RSGISPY_CHECK_INT(o))
            {
                maskVals.push_back(RSGISPY_INT_EXTRACT(o));
            }
            else
            {
                Py_DECREF(o);
                PyErr_SetString(GETSTATE(self)->error, "Mask value was not a sequence but a single value, however that value was not an integer.");
                return nullptr;
            }
        }
    }
    
    try
    {
        rsgis::cmds::executePerformRandomPxlSample(std::string(pszInputImage), std::string(pszOutputImage), std::string(pszGDALFormat), maskVals, numSamples);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_PerformRandomPxlSampleSmallPxlCount(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("mask_vals"),
                             RSGIS_PY_C_TEXT("n_samples"), RSGIS_PY_C_TEXT("rnd_seed"), nullptr};
    const char *pszInputImage = "";
    const char *pszOutputImage = "";
    const char *pszGDALFormat = "";
    PyObject *maskValsObj;
    unsigned int numSamples = 0;
    int rndSeed = 0;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssOI|i:perform_random_pxl_sample_in_mask_low_pxl_count", kwlist, &pszInputImage, &pszOutputImage, &pszGDALFormat, &maskValsObj, &numSamples, &rndSeed))
    {
        return nullptr;
    }
    
    std::vector<int> maskVals;
    
    if(!PySequence_Check(maskValsObj))
    {
        if(RSGISPY_CHECK_INT(maskValsObj))
        {
            maskVals.push_back(RSGISPY_INT_EXTRACT(maskValsObj));
        }
        else
        {
            PyErr_SetString(GETSTATE(self)->error, "Mask value was not a sequence but a single value, however that value was not an integer.");
            return nullptr;
        }
    }
    else
    {
        Py_ssize_t nMaskVals = PySequence_Size(maskValsObj);
        for( Py_ssize_t n = 0; n < nMaskVals; n++ )
        {
            PyObject *o = PySequence_GetItem(maskValsObj, n);
            if(RSGISPY_CHECK_INT(o))
            {
                maskVals.push_back(RSGISPY_INT_EXTRACT(o));
            }
            else
            {
                Py_DECREF(o);
                PyErr_SetString(GETSTATE(self)->error, "Mask value was not a sequence but a single value, however that value was not an integer.");
                return nullptr;
            }
        }
    }
    
    try
    {
        rsgis::cmds::executePerformRandomPxlSampleSmallPxlCount(std::string(pszInputImage), std::string(pszOutputImage), std::string(pszGDALFormat), maskVals, numSamples, rndSeed);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_PanSharpenHCS(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("win_size"), RSGIS_PY_C_TEXT("use_naive_method"), nullptr};
    const char *pszInputImage = "";
    const char *pszOutputImage = "";
    const char *pszGDALFormat = "";
    int nDataType;
    unsigned int winSize = 7;
    int useNaiveMethInt = false;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssi|Ii:pan_sharpen_hcs", kwlist, &pszInputImage, &pszOutputImage, &pszGDALFormat, &nDataType, &winSize, &useNaiveMethInt))
    {
        return nullptr;
    }
    
    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
    
    try
    {
        bool useNaiveMeth = (bool)useNaiveMethInt;
        rsgis::cmds::executePerformHCSPanSharpen(std::string(pszInputImage), std::string(pszOutputImage), std::string(pszGDALFormat), type, winSize, useNaiveMeth);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_SharpenLowResImageBands(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("band_info"), RSGIS_PY_C_TEXT("win_size"),
                             RSGIS_PY_C_TEXT("no_data_val"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    const char *pszInputImage = "";
    const char *pszOutputImage = "";
    PyObject *bandInfoPyObj;
    const char *pszGDALFormat = "";
    int nDataType;
    unsigned int winSize;
    int nodata;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssOIisi:sharpen_low_res_bands", kwlist, &pszInputImage, &pszOutputImage,
                                     &bandInfoPyObj, &winSize, &nodata, &pszGDALFormat, &nDataType))
    {
        return nullptr;
    }
    
    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
    
    
    
    if( !PySequence_Check(bandInfoPyObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(bandInfoPyObj);
    
    std::vector<rsgis::cmds::RSGISInitSharpenBandInfo> bandInfo;
    bandInfo.reserve(nBandDefns);
    
    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(bandInfoPyObj, n);
        
        PyObject *pBand = PyObject_GetAttrString(o, "band");
        if( ( pBand == nullptr ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'band\'" );
            Py_DECREF(pBand);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pStatus = PyObject_GetAttrString(o, "status");
        if( ( pStatus == nullptr ) || ( pStatus == Py_None ) || !RSGISPY_CHECK_INT(pStatus) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'status\'" );
            Py_DECREF(pStatus);
            Py_DECREF(pBand);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pName = PyObject_GetAttrString(o, "name");
        if( ( pName == nullptr ) || ( pName == Py_None ) || !RSGISPY_CHECK_STRING(pName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'name\'" );
            Py_DECREF(pName);
            Py_DECREF(pStatus);
            Py_DECREF(pBand);
            Py_DECREF(o);
            return nullptr;
        }
        
        rsgis::cmds::RSGISInitSharpenBandInfo sharpInfo = rsgis::cmds::RSGISInitSharpenBandInfo();
        sharpInfo.band = RSGISPY_INT_EXTRACT(pBand);
        sharpInfo.bandName = RSGISPY_STRING_EXTRACT(pName);
        int statusInt = RSGISPY_INT_EXTRACT(pStatus);
        if(statusInt == 0)
        {
            sharpInfo.status = rsgis::cmds::rsgis_init_ignore;
        }
        else if(statusInt == 1)
        {
            sharpInfo.status = rsgis::cmds::rsgis_init_lowres;
        }
        else if(statusInt == 2)
        {
            sharpInfo.status = rsgis::cmds::rsgis_init_highres;
        }
        else
        {
            PyErr_SetString(GETSTATE(self)->error, "\'status\' must have a value SHARP_RES_IGNORE, SHARP_RES_LOW or SHARP_RES_HIGH." );
            Py_DECREF(pName);
            Py_DECREF(pStatus);
            Py_DECREF(pBand);
            Py_DECREF(o);
            return nullptr;
        }
        bandInfo.push_back(sharpInfo);
        
        Py_DECREF(pName);
        Py_DECREF(pStatus);
        Py_DECREF(pBand);
        Py_DECREF(o);
    }
    
    try
    {
        rsgis::cmds::executeSharpenLowResImgBands(std::string(pszInputImage), std::string(pszOutputImage),
                                                  bandInfo, winSize, nodata, std::string(pszGDALFormat), type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_CreateRefImageCompositeImg(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("in_ref_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("out_no_data"), nullptr};
    PyObject *pInputImages;
    const char *pszOutputImage = "";
    const char *pszRefImage = "";
    const char *pszGDALFormat = "";
    int nDataType;
    float outNoData = 0.0;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "Osssif:create_ref_img_composite_img", kwlist, &pInputImages,
                                     &pszOutputImage, &pszRefImage, &pszGDALFormat, &nDataType, &outNoData))
    {
        return nullptr;
    }
    
    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
    
    if( !PySequence_Check(pInputImages))
    {
        PyErr_SetString(GETSTATE(self)->error, "Input images must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nImages = PySequence_Size(pInputImages);
    std::vector<std::string> inputImages;
    inputImages.reserve(nImages);
    for( Py_ssize_t n = 0; n < nImages; n++ )
    {
        PyObject *o = PySequence_GetItem(pInputImages, n);
        
        if(!RSGISPY_CHECK_STRING(o))
        {
            PyErr_SetString(GETSTATE(self)->error, "Input images must be strings");
            Py_DECREF(o);
            return nullptr;
        }
        
        inputImages.push_back(RSGISPY_STRING_EXTRACT(o));
    }
    
    try
    {
        rsgis::cmds::executeCreateRefImgCompsiteImage(inputImages, std::string(pszOutputImage),
                                                      std::string(pszRefImage), std::string(pszGDALFormat),
                                                      type, outNoData);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_GenTimeseriesFillCompositeImg(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("comp_info"), RSGIS_PY_C_TEXT("in_vld_img"),
                             RSGIS_PY_C_TEXT("out_ref_fill_img"), RSGIS_PY_C_TEXT("out_comp_img"),
                             RSGIS_PY_C_TEXT("out_comp_ref_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    PyObject *pCompInfo;
    const char *pszValidMaskImage = "";
    const char *pszOutRefFillImage = "";
    const char *pszOutCompImage = "";
    const char *pszOutCompRefImage = "";
    const char *pszGDALFormat = "";
    int nDataType;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "Osssssi:gen_timeseries_fill_composite_img", kwlist, &pCompInfo, &pszValidMaskImage,
                                     &pszOutRefFillImage, &pszOutCompImage, &pszOutCompRefImage, &pszGDALFormat, &nDataType))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pCompInfo))
    {
        PyErr_SetString(GETSTATE(self)->error, "Composite Info object must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nImages = PySequence_Size(pCompInfo);
    std::vector<rsgis::cmds::RSGISCmdCompositeInfo> inCompInfo;
    inCompInfo.reserve(nImages);
    for( Py_ssize_t n = 0; n < nImages; n++ )
    {
        PyObject *o = PySequence_GetItem(pCompInfo, n);
        
        PyObject *pYear = PyObject_GetAttrString(o, "year");
        if( ( pYear == nullptr ) || ( pYear == Py_None ) || !RSGISPY_CHECK_INT(pYear) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'year\'" );
            Py_DECREF(pYear);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pDay = PyObject_GetAttrString(o, "day");
        if( ( pDay == nullptr ) || ( pDay == Py_None ) || !RSGISPY_CHECK_INT(pDay) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'day\'" );
            Py_DECREF(pYear);
            Py_DECREF(pDay);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pCompImg = PyObject_GetAttrString(o, "compImg");
        if( ( pCompImg == nullptr ) || ( pCompImg == Py_None ) || !RSGISPY_CHECK_STRING(pCompImg) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'compImg\'" );
            Py_DECREF(pYear);
            Py_DECREF(pDay);
            Py_DECREF(pCompImg);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pImgRef = PyObject_GetAttrString(o, "imgRef");
        if( ( pImgRef == nullptr ) || ( pImgRef == Py_None ) || !RSGISPY_CHECK_STRING(pImgRef) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'imgRef\'" );
            Py_DECREF(pYear);
            Py_DECREF(pDay);
            Py_DECREF(pCompImg);
            Py_DECREF(pImgRef);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pOutRef = PyObject_GetAttrString(o, "outRef");
        if( ( pOutRef == nullptr ) || ( pOutRef == Py_None ) || !RSGISPY_CHECK_INT(pOutRef) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'outRef\'" );
            Py_DECREF(pYear);
            Py_DECREF(pDay);
            Py_DECREF(pCompImg);
            Py_DECREF(pImgRef);
            Py_DECREF(pOutRef);
            Py_DECREF(o);
            return nullptr;
        }
        
        rsgis::cmds::RSGISCmdCompositeInfo compInfoObj = rsgis::cmds::RSGISCmdCompositeInfo();
        compInfoObj.year = RSGISPY_INT_EXTRACT(pYear);
        compInfoObj.day = RSGISPY_INT_EXTRACT(pDay);
        compInfoObj.compImg = RSGISPY_STRING_EXTRACT(pCompImg);
        compInfoObj.imgRef = RSGISPY_STRING_EXTRACT(pImgRef);
        compInfoObj.outRef = (bool)RSGISPY_INT_EXTRACT(pOutRef);
        
        inCompInfo.push_back(compInfoObj);
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeGenTimeseriesFillCompositeImg(inCompInfo, std::string(pszValidMaskImage),
                                                          std::string(pszOutRefFillImage), std::string(pszOutCompImage),
                                                          std::string(pszOutCompRefImage), std::string(pszGDALFormat), type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyObject *ImageUtils_ExportSingleMergedImgBand(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("int_band_ref_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    const char *pInputImg = "";
    const char *pInputBandRefImg = "";
    const char *pOutputImg = "";
    const char *pszGDALFormat = "";
    int nDataType;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssi:export_single_merged_img_band", kwlist, &pInputImg, &pInputBandRefImg, &pOutputImg, &pszGDALFormat, &nDataType))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeExportSingleMergedImgBand(std::string(pInputImg), std::string(pInputBandRefImg), std::string(pOutputImg), std::string(pszGDALFormat), type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_GetGDALImageCreationOpts(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pGDALFormat = "";

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "s:get_gdal_img_creation_opts", kwlist, &pGDALFormat))
    {
        return nullptr;
    }
    
    bool rtn_dict = false;
    PyObject *out_info_dict = nullptr;
    try
    {
        std::map<std::string, std::string> gdalCreationOpts = rsgis::cmds::executeGetGDALImageCreationOpts(std::string(pGDALFormat));
        
        if(gdalCreationOpts.size() > 0)
        {
            out_info_dict = PyDict_New();
            for(auto iterOpts = gdalCreationOpts.begin(); iterOpts != gdalCreationOpts.end(); ++iterOpts)
            {
                PyDict_SetItem(out_info_dict, Py_BuildValue("s", (iterOpts->first).c_str()), Py_BuildValue("s", (iterOpts->second).c_str()));
            }
            rtn_dict = true;
        }
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    if(rtn_dict)
    {
        return out_info_dict;
    }
    else
    {
        Py_RETURN_NONE;
    }
}



static PyObject *ImageUtils_UnPackPixelVals(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("img_band"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pInputImage = "";
    unsigned int pInputImageBand = 1;
    const char *pszOutputImage = "";
    const char *pszGDALFormat = "";

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sIss:unpack_pxl_vals", kwlist, &pInputImage, &pInputImageBand, &pszOutputImage, &pszGDALFormat))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeUnpackPxlValues(std::string(pInputImage), pInputImageBand, std::string(pszOutputImage), std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef ImageUtilsMethods[] = {
{"stretch_img", (PyCFunction)ImageUtils_StretchImage, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.stretch_img(input_img, output_img, save_stats, out_stats_file, no_data_val, one_pass_std, gdalformat, datatype, stretch_type, stretch_param)\n"
"Stretches (scales) pixel values to a range of 0 - 255, which is typically for visualisation but the function can also be used for normalisation.\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param save_stats: is a bool specifying if stats should be saved to a text file.\n"
":param out_stats_file: is a string providing the name of the file to save stats to.\n"
":param no_data_val: is a float specifying the no data value of the input image.\n"
":param one_pass_std: is a bool specifying if is single pass should be used for calculating standard deviation (faster but less accurate)\n"
":param gdalformat: is a string providing the output gdalformat of the tiles (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the output data type.\n"
":param stretch_type: is a STRETCH_* value providing the type of stretch, options are:\n"
"        * imageutils.STRETCH_LINEARMINMAX - Stretches between min and max.\n"
"        * imageutils.STRETCH_LINEARPERCENT - Stretches between percentage of image range. Parameter defines percent.\n"
"        * imageutils.STRETCH_LINEARSTDDEV - Stretches between mean - sd to mean + sd. Parameter defines number of standard deviations.\n"
"        * imageutils.STRETCH_EXPONENTIAL - Exponential stretch between mean - 2*sd to mean + 2*sd. No parameter.\n"
"        * imageutils.STRETCH_LOGARITHMIC - Logarithmic stretch between mean - 2*sd to mean + 2*sd. No parameter.\n"
"        * imageutils.STRETCH_POWERLAW - Power law stretch between mean - 2*sd to mean + 2*sd. Parameter defines power.\n"
":param stretch_param: is a float, providing the input parameter to the stretch (if required).\n"
"\n"
".. code:: python\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   outputImage = './TestOutputs/injune_p142_casi_sub_utm_2sd.kea'\n"
"   gdalformat = 'KEA'\n"
"   datatype = rsgislib.TYPE_8INT\n"
"   imageutils.stretch_img(inputImage, outputImage, False, '', 0.0, False, gdalformat, datatype, imageutils.STRETCH_LINEARSTDDEV, 2)\n"
"\n"},

{"stretch_img_with_stats", (PyCFunction)ImageUtils_StretchImageWithStats, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.stretch_img_with_stats(input_img, output_img, in_stats_file, gdalformat, datatype, no_data_val, stretch_type, stretch_param)\n"
"Stretches (scales) pixel values to a range of 0 - 255, which is typically for visualisation but the function can also be used for normalisation.\n"
"This function uses pre-calculated statistics - normally from rsgislib.imageutils.stretch_img.\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param in_stats_file: is a string providing the name of the file to read stats from.\n"
":param gdalformat: is a string providing the output gdalformat of the tiles (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the output data type.\n"
":param no_data_val: is a float with the no data value of the input image.\n"
":param stretch_type: is a STRETCH_* value providing the type of stretch, options are:\n"
"        * imageutils.STRETCH_LINEARMINMAX - Stretches between min and max.\n"
"        * imageutils.STRETCH_LINEARPERCENT - Stretches between percentage of image range. Parameter defines percent.\n"
"        * imageutils.STRETCH_LINEARSTDDEV - Stretches between mean - sd to mean + sd. Parameter defines number of standard deviations.\n"
"        * imageutils.STRETCH_EXPONENTIAL - Exponential stretch between mean - 2*sd to mean + 2*sd. No parameter.\n"
"        * imageutils.STRETCH_LOGARITHMIC - Logarithmic stretch between mean - 2*sd to mean + 2*sd. No parameter.\n"
"        * imageutils.STRETCH_POWERLAW - Power law stretch between mean - 2*sd to mean + 2*sd. Parameter defines power.\n"
":param stretch_param: is a float, providing the input parameter to the stretch (if required).\n"
"\n"
".. code:: python\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   inputImageStats = './Rasters/injune_p142_casi_sub_utm_stats.txt'\n"
"   outputImage = './TestOutputs/injune_p142_casi_sub_utm_2sd.kea'\n"
"   gdalformat = 'KEA'\n"
"   datatype = rsgislib.TYPE_8UINT\n"
"   nodataval = 0.0\n"
"   imageutils.stretch_img_with_stats(inputImage, outputImage, inputImageStats, True, False, gdalformat, datatype, nodataval, imageutils.STRETCH_LINEARSTDDEV, 2)\n"
"\n"
"\n"},
    
    
{"normalise_img_pxl_vals", (PyCFunction)ImageUtils_NormaliseImagePxlVals, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.normalise_img_pxl_vals(input_img=string, output_img=string, gdalformat=string, datatype=rsgislib.TYPE_*, in_no_data_val=float, out_no_data_val=float, out_min=float, out_max=float, stretch_type=imageutils.STRETCH_*, stretch_param=float)\n"
"Normalises the image pixel values to a range of outmin to outmax (default 0-1) where the no data value is specified by the user.\n"
"This function is largely similar to rsgislib.imageutils.stretch_img but deals with no data values correctly and intended\n"
"for data processing normalisation rather than visualisation.\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string providing the output gdalformat of the tiles (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the output data type.\n"
":param in_no_data_val: is a float with the input image no data value. (Default = 0)\n"
":param out_no_data_val: is a float with the no data value used within the output image. (Default = -1)\n"
":param out_min: is a float which specifies the output minimum pixel value (Default = 0)\n"
":param out_max: is a float which specifies the output maximum pixel value (Default = 1)\n"
":param stretch_type: is a STRETCH_* value providing the type of stretch, options are:\n"
"        * imageutils.STRETCH_LINEARMINMAX - Stretches between min and max.\n"
"        * imageutils.STRETCH_LINEARPERCENT - Stretches between percentage of image range. Parameter defines percent.\n"
"        * imageutils.STRETCH_LINEARSTDDEV - Stretches between mean - sd to mean + sd. Parameter defines number of standard deviations.\n"
"        * imageutils.STRETCH_EXPONENTIAL - Exponential stretch between mean - 2*sd to mean + 2*sd. No parameter.\n"
"        * imageutils.STRETCH_LOGARITHMIC - Logarithmic stretch between mean - 2*sd to mean + 2*sd. No parameter.\n"
"        * imageutils.STRETCH_POWERLAW - Power law stretch between mean - 2*sd to mean + 2*sd. Parameter defines power.\n"
":param stretch_param: is a float, providing the input parameter to the stretch (if required; Default=2.0).\n"
"\n"
".. code:: python\n"
"\n"
"    import rsgislib\n"
"    import rsgislib.imageutils\n"
"\n"
"    inImg = './LS5TM_19851990Comp_lat7lon3896_r65p166_stdsref.kea'\n"
"\n"
"    outImg = 'LS5TM_19851990Comp_lat7lon3896_r65p166_stdsref_NORM_0-255.kea'\n"
"    rsgislib.imageutils.normalise_img_pxl_vals(inImg, outImg, 'KEA', rsgislib.TYPE_8UINT, innodataval=0, outnodataval=0, outmin=0, outmax=255,\n"
"                                              stretchtype=rsgislib.imageutils.STRETCH_LINEARSTDDEV, stretchparam=2)\n"
"    rsgislib.imageutils.pop_img_stats(outImg, usenodataval=True, nodataval=0, calcpyramids=True)\n"
"\n"
"    outImg = 'LS5TM_19851990Comp_lat7lon3896_r65p166_stdsref_NORM_0-1.kea'\n"
"    rsgislib.imageutils.normalise_img_pxl_vals(inImg, outImg, 'KEA', rsgislib.TYPE_32FLOAT, innodataval=0, outnodataval=0, outmin=0, outmax=1,\n"
"                                              stretchtype=rsgislib.imageutils.STRETCH_LINEARSTDDEV, stretchparam=2)\n"
"    rsgislib.imageutils.pop_img_stats(outImg, usenodataval=True, nodataval=0, calcpyramids=True)\n"
"\n"
"\n"},

{"mask_img", (PyCFunction)ImageUtils_maskImage, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.mask_img(input_img, in_msk_img, output_img, gdalformat, datatype, out_value, mask_value)\n"
"This command will mask an input image using a single band mask image - commonly this is a binary image.\n"
"\n"
":param input_img: is a string containing the name and path of the input image file.\n"
":param in_msk_img: is a string containing the name and path of the mask image file.\n"
":param output_img: is a string containing the name and path for the output image following application of the mask.\n"
":param gdalformat: is a string representing the output image file format (e.g., KEA, ENVI, GTIFF, HFA etc).\n"
":param datatype: is a rsgislib.TYPE_* value for the data type of the output image.\n"
":param out_value: is a float representing the value written to the output image in place of the regions being masked.\n"
":param mask_value: is a float or list of floats representing the value(s) within the mask image for the regions which are to be replaced with the outvalue.\n"
"\n"
".. code:: python\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   \n"
"   inImg = './LS5/Outputs/LS5TM_20110926_lat53lon511_r23p205_rad_toa.kea'\n"
"   imgMask = './LS5/Outputs/LS5TM_20110926_lat53lon511_r23p205_clouds.kea'\n"
"   outImg = './LS5/Outputs/LS5TM_20110926_lat53lon511_r23p205_rad_toa_mclds.kea'\n"
"   \n"
"   imageutils.mask_img(inImg, imgMask, outImg, 'KEA', rsgislib.TYPE_16UINT, 0, [1,2])\n"
"   imageutils.pop_img_stats(outImg, True, 0.0, True)\n"
"\n"},

{"create_tiles", (PyCFunction)ImageUtils_createTiles, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.create_tiles(input_img, out_img_base, tile_width, tile_height, tile_overlap, offset_tiles, gdalformat, datatype, out_img_ext)\n"
"Create tiles from a larger image, useful for splitting a large image into multiple smaller ones for processing.\n"
"\n"
"Where\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param out_img_base: is a string containing the base name of the output file the number of the tile and file extension will be appended.\n"
":param tile_width: is the width of each tile, in pixels.\n"
":param tile_height: is the height of each tile, in pixels.\n"
":param tile_overlap: is the overlap between tiles, in pixels\n"
":param offset_tiles: is a bool, determining if tiles should start halfway into the image useful for generating overlapping sets of tiles.\n"
":param gdalformat: is a string providing the output gdalformat of the tiles (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the output data type of the tiles.\n"
":param out_img_ext: is a string providing the extension for the tiles (as required by the specified data type).\n"
"\n"
":return: list of tile file names\n"
"\n"
".. code:: python\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   outBase = './TestOutputs/Tiles/injune_p142_casi_sub_utm'\n"
"   width = 100\n"
"   height = width\n"
"   overlap = 5\n"
"   offsettiling = False\n"
"   gdalformat = 'KEA'\n"
"   datatype = rsgislib.TYPE_32INT\n"
"   ext='kea'\n"
"   tiles = imageutils.create_tiles(inputImage, outBase, width, height, overlap, offsettiling, gdalformat, datatype, ext)\n"
"\n"},
    
{"create_img_mosaic", (PyCFunction)ImageUtils_createImageMosaic, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.create_img_mosaic(input_imgs, output_img, background_val, skip_val, skip_band, overlap_behaviour, gdalformat, datatype)\n"
"Create mosaic from list of input images.\n"
"\n"
"Where\n"
"\n"
":param input_imgs: is a list of input images.\n"
":param output_img: is a string containing the name of the output mosaic\n"
":param background_val: is a float providing the background (nodata) value for the mosaic\n"
":param skip_val: is a float providing the value to be skipped (nodata values) in the input images\n"
":param skip_band: is an integer providing the band to check for skipVal\n"
":param overlap_behaviour: is an integer specifying the behaviour for overlaping regions\n"
"      * 0 - Overwrite\n"
"      * 1 - Overwrite if value of new pixel is lower (minimum)\n"
"      * 2 - Overwrite if value of new pixel is higher (maximum)\n"
":param gdalformat: is a string providing the gdalformat of the output image (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\n"
".. code:: python\n"
"\n"
"	import rsgislib\n"
"	from rsgislib import imageutils\n"
"	import glob\n"
"   \n"
"	# Search for all files with the extension 'kea'\n"
"	inputList = glob.glob('./TestOutputs/Tiles/*.kea')\n"
"	outImage = './TestOutputs/injune_p142_casi_sub_utm_mosaic.kea'\n"
"	backgroundVal = 0.\n"
"	skipVal = 0.\n"
"	skipBand = 1\n"
"	overlapBehaviour = 0\n"
"	gdalformat = 'KEA'\n"
"	datatype = rsgislib.TYPE_32FLOAT\n"
"	imageutils.create_img_mosaic(inputList, outImage, backgroundVal, skipVal, skipBand, overlapBehaviour, gdalformat, datatype)\n"
"\n"},
 
    {"include_imgs", (PyCFunction)ImageUtils_IncludeImages, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.include_imgs(input_img, include_imgs, input_bands=None, skip_val=None)\n"
"Create mosaic from list of input images.\n"
"\n"
":param input_img: is a string containing the name of the input image to add image to\n"
":param include_imgs: is a list of input images\n"
":param input_bands: is a subset of input bands to use (optional)\n"
":param skip_val: is a float specifying a value which should be ignored and not copied into the new image (optional). To use you must also provided a list of subset image bands.\n"
"\n"
".. code:: python\n"
"\n"
"	import rsgislib\n"
"	from rsgislib import imageutils\n"
"	import glob\n"
"	# Search for all files with the extension 'kea'\n"
"	baseImage = './TestOutputs/injune_p142_casi_sub_utm_mosaic.kea'\n"
"	inputList = glob.glob('./TestOutputs/Tiles/*.kea')\n"
"	imageutils.include_imgs(baseImage, inputList)\n"
"\n"},
 
{"include_imgs_with_overlap", (PyCFunction)ImageUtils_IncludeImagesOverlap, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.include_imgs_with_overlap(input_img, include_imgs, overlap)\n"
"Create mosaic from list of input images where the input images have an overlap.\n"
"\n"
":param input_img: is a string containing the name of the input image to add image to\n"
":param include_imgs: is a list of input images\n"
":param overlap: the size, in pixels, of the each on the images which should be ignored as an overlapping region.\n"
"\n"
".. code:: python\n"
"\n"
"	import rsgislib\n"
"	from rsgislib import imageutils\n"
"	import glob\n"
"   inputImg = 'LandsatImg.kea'\n"
"   tilesImgBase = './tiles/LandsatTile'\n"
"   outputImg = 'LandsatImgProcessed.kea'\n"
"   imageutils.create_tiles(inputImg, tilesImgBase, 1000, 1000, 10, False, 'KEA', rsgislib.TYPE_32FLOAT, 'kea')\n"
"   # Do some processing on the tiles... \n"
"   imageutils.create_copy_img(inputImg, outputImg, 6, 0, 'KEA', rsgislib.TYPE_32FLOAT)\n"
"	inputList = glob.glob('./tiles/LandsatTile*.kea')\n"
"	imageutils.include_imgs_with_overlap(outputImg, inputList, 10)\n"
"\n"},

{"include_imgs_ind_img_intersect", (PyCFunction)ImageUtils_IncludeImagesIndImgIntersect, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.include_imgs_ind_img_intersect(input_img, include_imgs)\n"
"Create mosaic from list of input images written to the base image. The no data value will be honored and values over written.\n"
"\n"
":param input_img: is a string containing the name of the input image to add image to\n"
":param include_imgs: is a list of input images\n"
"\n"
".. code:: python\n"
"\n"
"    import rsgislib\n"
"    from rsgislib import imageutils\n"
"    import glob\n"
"    # Search for all files with the extension 'kea'\n"
"    baseImage = './TestOutputs/injune_p142_casi_sub_utm_mosaic.kea'\n"
"    inputList = glob.glob('./TestOutputs/Tiles/*.kea')\n"
"    imageutils.include_imgs_ind_img_intersect(baseImage, inputList)\n"
"\n"},

{"pop_img_stats", (PyCFunction)ImageUtils_PopImageStats, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.pop_img_stats(input_img, use_no_data=True, no_data_val=0, calc_pyramids=True)\n"
"Calculate the image statistics and build image pyramids populating the image file.\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param use_no_data: is a boolean stating whether the no data value is to be used (default=True).\n"
":param no_data_val: is a floating point value to be used as the no data value (default=0.0).\n"
":param calc_pyramids: is a boolean stating whether image pyramids should be calculated (default=True).\n"
"\n"
".. code:: python\n"
"\n"
"   from rsgislib import imageutils\n"
"   inputImage = './TestOutputs/injune_p142_casi_sub_utm.kea'\n"
"   imageutils.pop_img_stats(inputImage,True,0.,True)\n"
"\n"},
    
    {"assign_wkt_proj", (PyCFunction)ImageUtils_AssignProj, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.assign_wkt_proj(input_img, wkt_str, wkt_file)\n"
"Assign a projection to the input GDAL image file.\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param wkt_str: is the wkt string to be assigned to the image. If None then it will be read from the wktStringFile.\n"
":param wkt_file: is a file path to a text file containing the WKT string to be assigned. This is ignored if wktString is not None.\n"
"\n"
".. code:: python\n"
"\n"
"   from rsgislib import imageutils\n"
"   wkt_str = '''PROJCS[\"WGS 84 / UTM zone 55S\",\n"
"    GEOGCS[\"WGS 84\",\n"
"        DATUM[\"WGS_1984\",\n"
"            SPHEROID[\"WGS 84\",6378137,298.257223563,\n"
"                AUTHORITY[\"EPSG\",\"7030\"]],\n"
"            AUTHORITY[\"EPSG\",\"6326\"]],\n"
"        PRIMEM[\"Greenwich\",0],\n"
"        UNIT[\"degree\",0.0174532925199433],\n"
"        AUTHORITY[\"EPSG\",\"4326\"]],\n"
"    PROJECTION[\"Transverse_Mercator\"],\n"
"    PARAMETER[\"latitude_of_origin\",0],\n"
"    PARAMETER[\"central_meridian\",147],\n"
"    PARAMETER[\"scale_factor\",0.9996],\n"
"    PARAMETER[\"false_easting\",500000],\n"
"    PARAMETER[\"false_northing\",10000000],\n"
"    UNIT[\"metre\",1,\n"
"        AUTHORITY[\"EPSG\",\"9001\"]],\n"
"    AUTHORITY[\"EPSG\",\"32755\"]]'''\n"
"   input_img = './TestOutputs/injune_p142_casi_sub_utm.kea'\n"
"   imageutils.assign_wkt_proj(input_img, wkt_str)\n"
"\n"},
    
    {"copy_proj_from_img", (PyCFunction)ImageUtils_CopyProjFromImage, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.copy_proj_from_img(input_img, ref_img)\n"
"Copy the projection from a reference image to an input GDAL image file.\n"
"\n"
":param input_img: is a string containing the name and path of the input file\n"
":param ref_img: is a string containing the name and path of the reference image.\n"
"\n"},
    
    {"copy_spatial_and_proj_from_img", (PyCFunction)ImageUtils_CopySpatialAndProjFromImage, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.copy_spatial_and_proj_from_img(input_img, ref_img)\n"
"Copy the spatial information and projection from a reference image to an input GDAL image file.\n"
"\n"
":param input_img: is a string containing the name and path of the input file\n"
":param ref_img: is a string containing the name and path of the reference image.\n"
"\n"},

    {"assign_spatial_info", (PyCFunction)ImageUtils_AssignSpatialInfo, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.assign_spatial_info(input_img, tl_x, tl_y, res_x, res_y, rot_x, rot_y)\n"
"Assign the spatial information to an input GDAL image file.\n"
"\n"
":param input_img: is a string containing the name and path of the input file\n"
":param tl_x: is a double representing the top left X coordinate of the image.\n"
":param tl_y: is a double representing the top left Y coordinate of the image.\n"
":param res_x: is a double representing X resolution of the image.\n"
":param res_y: is a double representing Y resolution of the image.\n"
":param rot_x: is a double representing X rotation of the image.\n"
":param rot_y: is a double representing Y rotation of the image.\n"
"\n"},

{"select_img_bands", (PyCFunction)ImageUtils_SelectImageBands, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.select_img_bands(input_img, output_img, gdalformat, datatype, bands)\n"
"Copy selected image bands from an image to a new image.\n"
"\n"
":param input_img: is a string containing the name and path of the input file\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param gdalformat: is a string providing the gdalformat of the output image (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
":param bands: is a list of integers for the bands in the input image to exported to the output image (Note band count starts at 1).\n"
"\n"
".. code:: python\n"
"\n"
"   import rsgislib.imageutils\n"
"   import rsgislib\n"
"   bands = [1,2]\n"
"   rsgislib.imageutils.select_img_bands('N06W053_07_ALL_sl_sub.kea', 'N06W053_07_ALL_sl_sub_HHVV.kea', 'KEA', rsgislib.TYPE_32INT, bands)\n"
"\n"
"\n"
},

{"subset", (PyCFunction)ImageUtils_Subset, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.subset(input_img, vec_file, vec_lyr, output_img, gdalformat, datatype)\n"
"Subset an image to the bounding box of a vector.\n"
"\n"
":param input_img: is a string providing the name of the input file.\n"
":param vec_file: is a string providing the vector which the image is to be clipped to. \n"
":param vec_lyr: is a string specifying the layer within the vector file to be used\n"
":param output_img: is a string providing the output image. \n"
":param gdalformat: is a string providing the gdalformat of the output image (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\n"
".. code:: python\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   input_img = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   vec_file = './Vectors/injune_p142_plot_location_utm.shp'\n"
"   vec_lyr = 'injune_p142_plot_location_utm'\n"
"   output_img = './TestOutputs/injune_p142_casi_sub_utm_subset.kea'\n"
"   gdalformat = 'KEA'\n"
"   datatype = rsgislib.TYPE_32FLOAT\n"
"   imageutils.subset(input_img, vec_file, vec_lyr, output_img, gdalformat, datatype)\n"
"\n"},
    
{"subset_bbox", (PyCFunction)ImageUtils_SubsetBBox, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.subset_bbox(input_img, output_img, gdalformat, datatype, min_x, max_x, min_y, max_y)\n"
"Subset an image to the bounding box of a vector.\n"
"\n"
":param input_img: is a string providing the name of the input file.\n"
":param output_img: is a string providing the output image. \n"
":param gdalformat: is a string providing the gdalformat of the output image (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
":param min_x: double within the minimum X for the bounding box\n"
":param max_x: double within the maximum X for the bounding box\n"
":param min_y: double within the minimum Y for the bounding box\n"
":param max_y: double within the maximum X for the bounding box\n"
"\n"
".. code:: python\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   outputImage = './TestOutputs/injune_p142_casi_sub_utm_subset.kea'\n"
"   gdalformat = 'KEA'\n"
"   datatype = rsgislib.TYPE_32FLOAT\n"
"   xMin = 295371.5\n"
"   xMax = 295401.5\n"
"   yMin = 359470.8\n"
"   yMax = 359500.8\n"
"   imageutils.subset_bbox(inputImage, outputImage, gdalformat, datatype, xMin, xMax, yMin, yMax)\n"
"\n"},

    {"subset_to_img", (PyCFunction)ImageUtils_Subset2Img, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.subset_to_img(input_img:str, in_roi_img:str, output_img:str, gdalformat:str, datatype:int)\n"
"Subset an image to the bounding box of an image.\n"
"\n"
":param input_img: is a string providing the name of the input file.\n"
":param in_roi_img: is a string providing the image which the 'inputimage' is to be clipped to. \n"
":param output_img: is a string providing the output image. \n"
":param gdalformat: is a string providing the gdalformat of the output image (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\n"
".. code:: python\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   inputROIimage = './Vectors/injune_p142_roi.kea'\n"
"   outputImage = './TestOutputs/injune_p142_casi_sub_utm_subset.kea'\n"
"   gdalformat = 'KEA'\n"
"   gdaltype = rsgislib.TYPE_32FLOAT\n"
"   imageutils.subset_to_img(inputImage, inputROIimage, outputImage, gdalformat, datatype)\n"
"\n"},
    
    
{"stack_img_bands", (PyCFunction)ImageUtils_StackImageBands, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.stack_img_bands(input_imgs, band_names, output_img, skip_value, no_data_val, gdalformat, datatype)\n"
"Create a single image from list of input images through band stacking.\n"
"\n"
":param input_imgs: is a list of input images.\n"
":param band_names: is a list of band names (one for each input image). If None then ignored.\n"
":param output_img: is a string containing the name and path for the outputted image.\n"
":param skip_value: is a float providing the value to be skipped (nodata values) in the input images (If None then ignored)\n"
":param no_data_val: is float specifying a no data value.\n"
":param gdalformat: is a string providing the gdalformat of the output image (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\n"
".. code:: python\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   imageList = ['./Rasters/injune_p142_casi_sub_utm_single_band.vrt','./Rasters/injune_p142_casi_sub_utm_single_band.vrt']\n"
"   bandNamesList = ['Image1','Image2']\n"
"   outputImage = './TestOutputs/injune_p142_casi_sub_stack.kea'\n"
"   gdalformat = 'KEA'\n"
"   gdaltype = rsgislib.TYPE_32FLOAT\n"
"   imageutils.stack_img_bands(imageList, bandNamesList, outputImage, None, 0, gdalformat, gdaltype)\n"
"\n"},
    
{"create_blank_img", (PyCFunction)ImageUtils_CreateBlankImage, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.create_blank_img(output_img, n_bands, width, height, tl_x, tl_y, res_x, res_y, pxl_val, wkt_file, wkt_str, gdalformat, datatype)\n"
"Create a new blank image with the parameters specified.\n"
"\n"
":param output_img: is a string containing the name and path for the outputted image.\n"
":param n_bands: is an integer specifying the number of image bands in the output image.\n"
":param width: is an integer specifying the width of the output image.\n"
":param height: is an integer specifying the height of the output image.\n"
":param tl_x: is a double specifying the Top Left pixel X coordinate (eastings) of the output image.\n"
":param tl_y: is a double specifying the Top Left pixel Y coordinate (northings) of the output image.\n"
":param res_x: is a double specifying the pixel resolution in the x-axis for the output image.\n"
":param res_y: is a double specifying the pixel resolution in the y-axis for the output image.\n"
":param pxl_val: is a float specifying the pixel value of the output image.\n"
":param wkt_file: is a string specifying the location of a file containing the WKT string representing the coordinate system and projection of the output image (if specified this parameter overrides the wktString parameter).\n"
":param wkt_str: is a string specifying the WKT string representing the coordinate system and projection of the output image.\n"
":param gdalformat: is a string providing the gdalformat of the output image (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\n"},

{"create_copy_img", (PyCFunction)ImageUtils_CreateCopyImage, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.create_copy_img(input_img, output_img, n_bands, pxl_val, gdalformat, datatype)\n"
"Create a new blank image with the parameters specified.\n"
"\n"
":param input_img: is a string containing the name and path for the input image, which is to be copied.\n"
":param output_img: is a string containing the name and path for the outputted image.\n"
":param n_bands: is an integer specifying the number of image bands in the output image.\n"
":param pxl_val: is a float specifying the pixel value of the output image.\n"
":param gdalformat: is a string providing the gdalformat of the output image (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\n"
".. code:: python\n"
"\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   outputImage = './TestOutputs/injune_p142_casi_sub_utm_blank.kea'\n"
"   gdalformat = 'KEA'\n"
"   datatype = rsgislib.TYPE_32FLOAT\n"
"   imageutils.create_copy_img(inputImage, outputImage, 1, 3, gdalformat, datatype)\n"
"\n"},
    
{"create_copy_img_def_extent", (PyCFunction)ImageUtils_CreateCopyImageDefExtent, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.create_copy_img_def_extent(input_img, output_img, n_bands, pxl_val, min_x, max_x, min_y, max_y, res_x, res_y, gdalformat, datatype)\n"
"Create a new blank image with the parameters specified.\n"
"\n"
":param input_img: is a string containing the name and path for the input image, which is to be copied.\n"
":param output_img: is a string containing the name and path for the outputted image.\n"
":param n_bands: is an integer specifying the number of image bands in the output image.\n"
":param pxl_val: is a float specifying the pixel value of the output image.\n"
":param min_x: is a double specifying the X minimum coordinate of the output image.\n"
":param max_x: is a double specifying the X maximum coordinate of the output image.\n"
":param min_y: is a double specifying the Y minimum coordinate of the output image.\n"
":param max_y: is a double specifying the Y maximum coordinate of the output image.\n"
":param res_x: is a double specifying the X resolution of the output image.\n"
":param res_y: is a double specifying the Y resolution of the output image.\n"
":param gdalformat: is a string providing the gdalformat of the output image (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\n"},

{"create_copy_img_vec_extent", (PyCFunction)ImageUtils_CreateCopyImageVecExtent, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.create_copy_img_vec_extent(input_img, vec_file, vec_lyr, output_img, n_bands, pxl_val, gdalformat, datatype)\n"
"Create a new blank image with the parameters specified but with the extent of the inputted shapefile.\n"
"\n"
":param input_img: is a string containing the name and path for the input image, which is to be copied.\n"
":param vec_file: is a string specifying the name and path of the vector file to which the image extent will be defined\n"
":param vec_lyr: is a string specifying the vector layer within the vector file.\n"
":param output_img: is a string containing the name and path for the outputted image.\n"
":param n_bands: is an integer specifying the number of image bands in the output image.\n"
":param pxl_val: is a float specifying the pixel value of the output image.\n"
":param gdalformat: is a string providing the gdalformat of the output image (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\n"
".. code:: python\n"
"\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   vec_file = './Rasters/injune_p142_roi.shp'\n"
"   vec_lyr = 'injune_p142_roi'\n"
"   outputImage = './TestOutputs/injune_p142_casi_sub_utm_blank.kea'\n"
"   gdalformat = 'KEA'\n"
"   datatype = rsgislib.TYPE_32FLOAT\n"
"   imageutils.create_copy_img_vec_extent(inputImage, vec_file, vec_lyr, outputImage, 3, 1, gdalformat, datatype)\n"
"\n"},

{"order_img_using_prop_valid_pxls", (PyCFunction)ImageUtils_OrderImagesUsingPropValidData, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.order_img_using_prop_valid_pxls(input_imgs, no_data_val)\n"
"Order the list of input images based on the their proportion of valid image pixels.\n"
"The primary use of this function is expected to be order (rank) images ahead of mosaicing.\n"
"\n"
":param input_imgs: is a list of string containing the name and path for the input images.\n"
":param no_data_val: is a float which specifies the no data value used to defined \'invalid\' pixels.\n"
"\n"
":return: a list of images ordered, from low to high (i.e., the first image will be the image with the smallest number of valid image pixels).\n"
"\n"},

{"gen_sampling_grid", (PyCFunction)ImageUtils_GenSamplingGrid, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.gen_sampling_grid(input_img, output_img, gdalformat, pxl_res, min_val, max_val, single_line)\n"
"Generate a regular sampling grid.\n"
"\n"
":param input_img: is a string specifying an image which defines the area of interest.\n"
":param output_img: is a string specifying an output image location.\n"
":param gdalformat: is a string providing the gdalformat of the output image (e.g., KEA).\n"
":param pxl_res: is a float specifying the output image resolution.\n"
":param min_val: is a minimum value for the output image pixel values.\n"
":param max_val: is a maximum value for the output image pixel values.\n"
":param single_line: is a boolean specifying whether the image is seen as a single line or new line with an offset in the starting value.\n"
"\n"},

{"gen_finite_mask", (PyCFunction)ImageUtils_GenFiniteMask, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.gen_finite_mask(input_img=string, output_img=string, gdalformat=string)\n"
"Generate a binary image mask defining the finite image regions.\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file.\n"
":param gdalformat: is a string with the GDAL output file format.\n"
"\n"
"\n.. code:: python\n"
"\n"
"   from rsgislib import imageutils\n"
"   inputImage = './injune_p142_casi_sub_utm.kea'\n"
"   outputImage = './injune_p142_casi_sub_utm.kea'\n"
"   imageutils.gen_finite_mask(inputImage, outputImage, \'KEA\')\n"
"\n"},
    
{"gen_valid_mask", (PyCFunction)ImageUtils_GenValidMask, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.gen_valid_mask(input_imgs=string|list, output_img=string, gdalformat=string, no_data_val=float)\n"
"Generate a binary image mask defining the regions which are not 'no data'.\n"
"\n"
":param input_imgs: can be either a string or a list containing the input file(s)\n"
":param output_img: is a string containing the name of the output file.\n"
":param gdalformat: is a string with the GDAL output file format.\n"
":param no_data_val: is a float defining the no data value (Optional and default is 0.0)\n"
"\n"
"\n.. code:: python\n"
"\n"
"   from rsgislib import imageutils\n"
"   inputImage = './injune_p142_casi_sub_utm.kea'\n"
"   outputImage = './injune_p142_casi_sub_utm.kea'\n"
"   imageutils.gen_valid_mask(inputImage, outputImage, \'KEA\', 0.0)\n"
"\n"},

{"gen_img_edge_mask", (PyCFunction)ImageUtils_GenImageEdgeMask, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.gen_img_edge_mask(input_img=string, output_img=string, gdalformat=string, n_edge_pxls=int)\n"
"Generate a binary image mask defining the edges of the pixel. The n_edge_pxls parameter specifies the \n"
" number of pixels from the edge of the input image which will be provided as the output mask.\n"
"\n"
":param input_img: the input image\n"
":param output_img: is a string containing the name of the output file.\n"
":param gdalformat: is a string with the GDAL output file format.\n"
":param n_edge_pxls: is int specifying the number of pixels from the edge to create the mask for.\n"
"\n"
"\n"},

   
{"combine_imgs_to_band", (PyCFunction)ImageUtils_CombineImages2Band, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.combine_imgs_to_band(input_imgs=list, output_img=string, gdalformat=string, datatype=int, no_data_val=float)\n"
"Combine images together into a single image band by excluding the no data value.\n"
"\n"
":param input_imgs: is a list of strings containing the names and paths of the input image files\n"
":param output_img: is a string containing the name of the output file.\n"
":param gdalformat: is a string with the GDAL output file format.\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param no_data_val: is the no data value which will be ignored (Default is 0)\n"
"\n"
"\n.. code:: python\n"
"\n"
"   from rsgislib import imageutils\n"
"   input_imgs = ['./forest.kea', './urban.kea', './water.kea']\n"
"   output_img = './classes.kea'\n"
"   datatype = rsgislib.TYPE_8UINT\n"
"   format = 'KEA'\n"
"   imageutils.combine_imgs_to_band(inputImages, outputImage, format, datatype, 0.0)\n"
"\n"},
    
{"perform_random_pxl_sample_in_mask", (PyCFunction)ImageUtils_PerformRandomPxlSample, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.perform_random_pxl_sample_in_mask(input_img=string, output_img=string, gdalformat=string, mask_vals=int|list, n_samples=unsigned int)\n"
"Randomly sample with a mask (e.g., classification). The same number of samples will be identified within each mask value listed by maskvals.\n"
"\n"
":param input_img: is a string for the input image mask - mask is typically whole values within regions (e.g., classifications).\n"
":param output_img: is a string with the name and path of the output image. Output is the mask pixel values.\n"
":param gdalformat: is a string with the GDAL output file format.\n"
":param mask_vals: can either be a single integer value or a list of values. If a list of values is specified then the total number of points identified (numSamples x n-maskVals).\n"
":param n_samples: is the number of samples to be created within each region.\n"
"\n"},
    
{"perform_random_pxl_sample_in_mask_low_pxl_count", (PyCFunction)ImageUtils_PerformRandomPxlSampleSmallPxlCount, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.perform_random_pxl_sample_in_mask_low_pxl_count(input_img=string, output_img=string, gdalformat=string, mask_vals=int|list, n_samples=unsigned int, rnd_seed=int)\n"
"Randomly sample with a mask (e.g., classification). The same number of samples will be identified within each mask value listed by maskvals.\n"
"This function produces a similar result to rsgislib.imageutils.perform_random_pxl_sample_in_mask but is more efficient for classes where only a small number of\n"
"pixels have that value. However, this function uses much more memory.\n"
"\n"
":param input_img: is a string for the input image mask - mask is typically whole values within regions (e.g., classifications).\n"
":param output_img: is a string with the name and path of the output image. Output is the mask pixel values.\n"
":param gdalformat: is a string with the GDAL output file format.\n"
":param mask_vals: can either be a single integer value or a list of values. If a list of values is specified then the total number of points identified (numSamples x n-maskVals).\n"
":param n_samples: is the number of samples to be created within each region.\n"
":param rnd_seed: is a an integer providing a seed for the random number generator. Please not that if this number is the same then the same random set of points will be generated.\n"
"\n"},
    
{"pan_sharpen_hcs", (PyCFunction)ImageUtils_PanSharpenHCS, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.pan_sharpen_hcs(input_img=string, output_img=string, gdalformat=string, datatype=int, win_size=unsigned int, use_naive_method=boolean)\n"
"A function which performs a Hyperspherical Colour Space (HSC) Pan Sharpening of an input image.\n"
"Padwick, C., Deskevich, M., Pacifici, F., Smallwood, S. 2010. WorldView-2 Pan-Sharpening.\n"
"ASPRS 2010 Annual Conference, San Diego, California (2010) pp. 26-30.\n"
"\n"
":param input_img: is a string for the input file, where the single panchromatic band must be the last in the stack.\n"
":param output_img: is a string with the name and path of the output image.\n"
":param gdalformat: is a string with the GDAL output file format.\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param win_size: is an optional integer, which must be an odd number, specifying the window size used for the analysis (Default = 7; Only used if useNaiveMethod=False).\n"
":param use_naive_method: is an optional boolean option to specify whether the naive or smart method should be used - False=Smart (Default), True=Naive Method.\n"
"\n"
"\n.. code:: python\n"
"\n"
"    import rsgislib\n"
"    import rsgislib.imageutils\n"
"\n"
"    rsgislib.imageutils.resampleImage2Match('./14SEP03025718-P2AS-054000253010_01_P001.TIF', './14SEP03025718-M2AS-054000253010_01_P001.TIF',\n"
"                                        './14SEP03025718-M2AS-054000253010_01_P001_resample.kea', 'KEA', 'nearestneighbour', rsgislib.TYPE_16UINT)\n"
"\n"
"    rsgislib.imageutils.stack_img_bands(['14SEP03025718-M2AS-054000253010_01_P001_resample.kea', '14SEP03025718-P2AS-054000253010_01_P001.TIF'],\n"
"                                         None, 'StackPanImg.kea', 0.0, 0.0, 'KEA', rsgislib.TYPE_16UINT)\n"
"\n"
"    rsgislib.imageutils.pan_sharpen_hcs(inimage='StackPanImg.kea', outimage='StackPanImgSharp.kea', gdalformat='KEA', datatype=rsgislib.TYPE_16UINT)\n"
"\n"
"    rsgislib.imageutils.pop_img_stats('StackPanImgSharp.kea', usenodataval=True, nodataval=0, calcpyramids=True)\n"
"\n"
"\n"},
    
{"sharpen_low_res_bands", (PyCFunction)ImageUtils_SharpenLowResImageBands, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.sharpen_low_res_bands(input_img=string, output_img=string, band_info=list, win_size=unsigned int, no_data_val=int, gdalformat=string, datatype=int)\n"
"A function which performs band sharpening using local linear fitting (orignal method proposed by Shepherd and Dymond).\n"
"\n"
":param input_img: is a string for the input file where the high resolution input image bands have been resampled \n"
"             (recommend nearest neighbour) to the same resolution has the higher resolution bands\n"
":param output_img: is a string with the name and path of the output image.\n"
":param band_info: is a list of the input image bands (type: rsgislib.imageutils.SharpBandInfo) specifying the band number, name and status.\n"
"           the status is either rsgislib.SHARP_RES_IGNORE, rsgislib.SHARP_RES_LOW or rsgislib.SHARP_RES_HIGH\n"
":param win_size: is an integer, which must be an odd number, specifying the window size (in pixels) used for the analysis (Default = 7). \n"
"          Recommend that the window size values fits at least 9 low resolution image pixels. \n"
"          For example, if the high resolution image is 10 m and the low 20 m then a 7 x 7 window\n"
"          will include 12.25 low resolution pixels.\n"
":param no_data_val: is an integer specifying the no data value for the scene\n"
":param gdalformat: is a string with the GDAL output file format.\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
"\n"
"\n.. code:: python\n"
"\n"
"    import rsgislib\n"
"    from rsgislib import imageutils\n"
"\n"
"    bandInfo = []\n"
"    bandInfo.append(imageutils.SharpBandInfo(band=1, status=rsgislib.SHARP_RES_LOW, name='Blue'))\n"
"    bandInfo.append(imageutils.SharpBandInfo(band=2, status=rsgislib.SHARP_RES_LOW, name='Green'))\n"
"    bandInfo.append(imageutils.SharpBandInfo(band=3, status=rsgislib.SHARP_RES_LOW, name='Red'))\n"
"    bandInfo.append(imageutils.SharpBandInfo(band=4, status=rsgislib.SHARP_RES_LOW, name='NIR'))\n"
"    bandInfo.append(imageutils.SharpBandInfo(band=5, status=rsgislib.SHARP_RES_HIGH, name='PAN'))\n"
"\n"
"    imageutils.sharpen_low_res_bands(inimage='./wv2/wv2_20140903_panstack.kea',\n"
"                                  outimage='./wv2/wv2_20140903_panstack_sharp.kea',\n"
"                                  bandinfo=bandInfo, winsize=7, nodata=0,\n"
"                                  gdalformat='KEA', datatype=rsgislib.TYPE_UINT16)\n"
"\n"
"\n"},

{"create_ref_img_composite_img", (PyCFunction)ImageUtils_CreateRefImageCompositeImg, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.create_ref_img_composite_img(input_imgs=list, output_img=string, in_ref_img=string, gdalformat=string, datatype=int, out_no_data=float)\n"
"A function which creates a composite image where the pixel values going into the output image by the\n"
"reference image. The reference image can be created using the rsgislib.imagecalc.get_img_idx_for_stat function.\n"
"\n"
":param input_imgs: is a list of input images, each image must have the same number of bands in the same order.\n"
":param output_img: is a string with the name and path of the output image.\n"
":param in_ref_img: is an image which specifies index of the image in inimages for which output pixel will be derived. Indexes start at 1, where 0 is no data.\n"
":param gdalformat: is a string with the GDAL output file format.\n"
":param datatype: is an integer containing one of the values from rsgislib.TYPE_*\n"
":param out_no_data: is the value which will be given to no data pixels in the output image."
"\n"
"\n"
".. code:: python\n"
"\n"
"    import rsgislib\n"
"    import rsgislib.imagecalc\n"
"    import rsgislib.imageutils\n"
"    import rsgislib.rastergis\n"
"\n"
"    import glob\n"
"    import os.path\n"
"\n"
"    # Get List of input images:\n"
"    inImages = glob.glob('./Outputs/*stdsref.kea')\n"
"\n"
"    # Generate Comp Ref layers:\n"
"    refLyrsLst = []\n"
"    refLayerPath = './CompRefLyrs/'\n"
"    idx = 1\n"
"    for img in inImages:\n"
"        print('In Image ('+str(idx) + '):\t' + img)\n"
"        baseImgName = os.path.splitext(os.path.basename(img))[0]\n"
"        refLyrImg = os.path.join(refLayerPath, baseImgName+'_ndvi.kea')\n"
"        rsgislib.imagecalc.calc_ndvi(img, 3, 4, refLyrImg)\n"
"        refLyrsLst.append(refLyrImg)\n"
"        idx = idx + 1\n"
"\n"
"    # Create REF Image\n"
"    pxlRefImg = 'LS5TM_19851990CompRefImg_lat7lon3896_r65p166_vmsk_mclds_topshad_rad_srefdem_stdsref.kea'\n"
"    rsgislib.imagecalc.get_img_idx_for_stat(refLyrsLst, pxlRefImg, 'KEA', -999, rsgislib.SUMTYPE_MAX)\n"
"\n"
"    # Pop Ref Image with stats\n"
"    rsgislib.rastergis.populateStats(pxlRefImg, True, True, True)\n"
"\n"
"    # Create Composite Image\n"
"    outCompImg = 'LS5TM_19851990CompRefImgMAX_lat7lon3896_r65p166_vmsk_mclds_topshad_rad_srefdem_stdsref.kea'\n"
"    rsgislib.imageutils.create_ref_img_composite_img(inImages, outCompImg, pxlRefImg, 'KEA', rsgislib.TYPE_16UINT, 0.0)\n"
"\n"
"    # Calc Stats\n"
"    rsgislib.imageutils.pop_img_stats(outCompImg, usenodataval=True, nodataval=0, calcpyramids=True)\n"
"\n"
"\n"},
    
    
{"gen_timeseries_fill_composite_img", (PyCFunction)ImageUtils_GenTimeseriesFillCompositeImg, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.gen_timeseries_fill_composite_img(comp_info=list, in_vld_img=string, out_ref_fill_img=string, out_comp_img=string, out_comp_ref_img=string, gdalformat=string, datatype=int)\n"
"A function which aids the creation of timeseries composites. This function uses reference images to identify\n"
"which pixels should be used to in-fill composite gaps.\n"
"\n"
":param comp_info: is a list of rsgislib.imageutils.RSGISTimeseriesFillInfo objects\n"
":param in_vld_img: is a string with the name and path to an image specifying the valid area within which the compsites are being generated.\n"
":param out_ref_fill_img: \n"
":param out_comp_img: \n"
":param out_comp_ref_img: \n"
":param gdalformat: is a string with the GDAL outCompImg file format.\n"
":param datatype: is an integer containing one of the values from rsgislib.TYPE_*, used for outCompImg\n"
"\n"
"\n"},
    
    
{"export_single_merged_img_band", (PyCFunction)ImageUtils_ExportSingleMergedImgBand, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.export_single_merged_img_band(input_img=string, int_band_ref_img=string, output_img=string, gdalformat=string, datatype=int)\n"
"A function which exports a single image band where the reference image specifies \n"
"from which image band the output pixel is extracted from. \n"
"\n"
":param input_img: is a string for the full (multi-band) image.\n"
":param int_band_ref_img: is a string with the reference image specifying the band to be outputted for each pixel.\n"
":param output_img: is a string for the single band output image.\n"
":param gdalformat: is a string with the GDAL outCompImg file format.\n"
":param datatype: is an integer containing one of the values from rsgislib.TYPE_*, used for outCompImg\n"
"\n"
"\n"},
    
{"get_gdal_img_creation_opts", (PyCFunction)ImageUtils_GetGDALImageCreationOpts, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.get_gdal_img_creation_opts(gdalformat=string)\n"
"This function returns a dict from by reading an environment variable to retrieve any creation options for \n"
"a particular image file format. Variables should have the name RSGISLIB_IMG_CRT_OPTS_<GDAL_FORMAT> where \n"
"a key value pairs separated by colons (:) is provided. The gdal format string must be upper case.\n"
"For example: \n"
"export RSGISLIB_IMG_CRT_OPTS_GTIFF=TILED=YES:COMPRESS=LZW:BIGTIFF=YES\n"
"export RSGISLIB_IMG_CRT_OPTS_HFA=COMPRESSED=YES:USE_SPILL=YES:AUX=NO:STATISTICS=YES\n"
"\n"
":param gdalformat: is a string specifying the GDAL image file format of interest.\n"
":returns: a dict of the options.\n"
"\n"
"\n"},

{"unpack_pxl_vals", (PyCFunction)ImageUtils_UnPackPixelVals, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.unpack_pxl_vals(input_img=string, img_band=int, output_img=string, gdalformat=string)\n"
"This function unpacks the image pixel values in the first band to a multiple band output image (1 band per bit).\n"
"\n"
":param input_img: is a string specifying the input image file.\n"
":param img_band: is the image band in the input image to use (index starts at 1).\n"
":param output_img: is a string specifying the output image file\n"
":param gdalformat: is a string specifying the GDAL image file format for the output file.\n"
"\n"
"\n"},

{nullptr}        /* Sentinel */
};



#if PY_MAJOR_VERSION >= 3

static int ImageUtils_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int ImageUtils_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_imageutils",
        nullptr,
        sizeof(struct ImageUtilsState),
        ImageUtilsMethods,
        nullptr,
        ImageUtils_traverse,
        ImageUtils_clear,
        nullptr
};

#define INITERROR return nullptr

PyMODINIT_FUNC 
PyInit__imageutils(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_imageutils(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_imageutils", ImageUtilsMethods);
#endif
    if( pModule == nullptr )
        INITERROR;

    struct ImageUtilsState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_imageutils.error", nullptr, nullptr);
    if( state->error == nullptr )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

    // add constants
    PyModule_AddIntConstant(pModule, "STRETCH_LINEARMINMAX", rsgis::cmds::linearMinMax);
    PyModule_AddIntConstant(pModule, "STRETCH_LINEARPERCENT", rsgis::cmds::linearPercent);
    PyModule_AddIntConstant(pModule, "STRETCH_LINEARSTDDEV", rsgis::cmds::linearStdDev);
    PyModule_AddIntConstant(pModule, "STRETCH_HISTOGRAM", rsgis::cmds::histogram);
    PyModule_AddIntConstant(pModule, "STRETCH_EXPONENTIAL", rsgis::cmds::exponential);
    PyModule_AddIntConstant(pModule, "STRETCH_LOGARITHMIC", rsgis::cmds::logarithmic);
    PyModule_AddIntConstant(pModule, "STRETCH_POWERLAW", rsgis::cmds::powerLaw);

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
