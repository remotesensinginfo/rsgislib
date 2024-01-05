/*
 *  rastergis.cpp
 *  RSGIS_LIB
 *
 *  Created by Sebastian Clarke on 19/07/2013.
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
#include "cmds/RSGISCmdRasterGIS.h"

/* An exception object for this module */
/* created in the init function */
struct RasterGisState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct RasterGisState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct RasterGisState _state;
#endif

// FUNCS HERE

static void FreePythonObjects(std::vector<PyObject*> toFree) {
    std::vector<PyObject*>::iterator iter;
    for(iter = toFree.begin(); iter != toFree.end(); ++iter) {
        Py_XDECREF(*iter);
    }
}

/**
 * Helper Function for converting a python sequence of strings to a vector
 */
static std::vector<std::string> ExtractVectorStringFromSequence(PyObject *sequence)
{
    Py_ssize_t nFields = PySequence_Size(sequence);
    std::vector<std::string> fields;
    fields.reserve(nFields);

    for(int i = 0; i < nFields; ++i)
    {
        PyObject *fieldObj = PySequence_GetItem(sequence, i);

        if(!RSGISPY_CHECK_STRING(fieldObj))
        {
            PyErr_SetString(GETSTATE(sequence)->error, "Fields must be strings");
            Py_DECREF(fieldObj);
            fields.clear();
            return fields;
        }

        fields.push_back(RSGISPY_STRING_EXTRACT(fieldObj));
        Py_DECREF(fieldObj);
    }

    return fields;
}

static PyObject *RasterGIS_PopulateStats(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage;
    int addColourTable2Img = 1;
    int calcImgPyramids = 1;
    int ignoreZeroVal = 1;
    unsigned int ratBand = 1;
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("add_clr_tab"),
                             RSGIS_PY_C_TEXT("calc_pyramids"), RSGIS_PY_C_TEXT("ignore_zero"),
                             RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "s|iiiI:pop_rat_img_stats", kwlist, &clumpsImage, &addColourTable2Img, &calcImgPyramids, &ignoreZeroVal, &ratBand))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executePopulateStats(std::string(clumpsImage), addColourTable2Img, calcImgPyramids, ignoreZeroVal, ratBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_CopyRAT(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *inputImage;
    int ratBand = 1;
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds,"ss|i:copy_rat", kwlist, &inputImage, &clumpsImage, &ratBand))
        return nullptr;

    try
    {
        rsgis::cmds::executeCopyRAT(std::string(clumpsImage), std::string(inputImage),ratBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_CopyGDALATTColumns(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *inputImage;
    PyObject *pFields;
    int ratBand = 1;
    int copyColours = 1;
    int copyHist = 1;
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("fields"),  RSGIS_PY_C_TEXT("copy_colours"),
                             RSGIS_PY_C_TEXT("copy_hist"), RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssO|iii:copy_gdal_rat_columns", kwlist, &inputImage, &clumpsImage, &pFields, &copyColours, &copyHist, &ratBand))
        return nullptr;

    if(!PySequence_Check(pFields))
    {
        PyErr_SetString(GETSTATE(self)->error, "'fields'  must be a sequence");
        return nullptr;
    }

    std::vector<std::string> fields = ExtractVectorStringFromSequence(pFields);
    if(fields.size() == 0)
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeCopyGDALATTColumns(std::string(inputImage), std::string(clumpsImage), fields, copyColours, copyHist, ratBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_SpatialLocation(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage, *eastingsField, *northingsField;
    unsigned int ratBand = 1;
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("eastings"),
                             RSGIS_PY_C_TEXT("northings"), RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sss|I:clumps_spatial_location", kwlist, &inputImage, &eastingsField, &northingsField, &ratBand))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeSpatialLocation(std::string(inputImage), ratBand, std::string(eastingsField), std::string(northingsField));
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_SpatialExtent(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage, *minXXCol, *minXYCol, *maxXXCol, *maxXYCol, *minYXCol, *minYYCol, *maxYXCol, *maxYYCol;
    unsigned int ratBand = 1;
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("min_xx"), RSGIS_PY_C_TEXT("min_xy"),
                             RSGIS_PY_C_TEXT("max_xx"), RSGIS_PY_C_TEXT("max_xy"), RSGIS_PY_C_TEXT("min_yx"),
                             RSGIS_PY_C_TEXT("min_yy"), RSGIS_PY_C_TEXT("max_yx"), RSGIS_PY_C_TEXT("max_yy"),
                             RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssssssss|I:clumps_spatial_extent", kwlist, &inputImage, &minXXCol, &minXYCol, &maxXXCol, &maxXYCol, &minYXCol, &minYYCol, &maxYXCol, &maxYYCol, &ratBand))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeSpatialLocationExtent(std::string(inputImage), ratBand, std::string(minXXCol), std::string(minXYCol), std::string(maxXXCol), std::string(maxXYCol), std::string(minYXCol), std::string(minYYCol), std::string(maxYXCol), std::string(maxYYCol));
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_PopulateRATWithStats(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage, *clumpsImage;
    PyObject *pBandAttStatsCmds;
    unsigned int ratBand = 1;
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("clumps_img"),
                             RSGIS_PY_C_TEXT("band_stats"), RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssO|I:populate_rat_with_stats", kwlist, &inputImage, &clumpsImage, &pBandAttStatsCmds, &ratBand))
    {
        return nullptr;
    }

    if(!PySequence_Check(pBandAttStatsCmds))
    {
        PyErr_SetString(GETSTATE(self)->error, "bandstats argument must be a sequence");
        return nullptr;
    }

    // extract the attributes from the sequence of objects into our structs
    Py_ssize_t nCmds = PySequence_Size(pBandAttStatsCmds);
    std::vector<rsgis::cmds::RSGISBandAttStatsCmds*> bandStatsCmds;
    bandStatsCmds.reserve(nCmds);

    for(int i = 0; i < nCmds; ++i)
    {
        PyObject *o = PySequence_GetItem(pBandAttStatsCmds, i);     // the python object

        rsgis::cmds::RSGISBandAttStatsCmds *cmdObj = new rsgis::cmds::RSGISBandAttStatsCmds();   // the c++ object we need to pass pointers of

        // declare and initialise pointers for all the attributes of the struct
        PyObject *pBand, *pMinField, *pMaxField, *pStdDevField, *pSumField, *pMeanField;
        pBand = pBand = pMinField = pMaxField = pMeanField = pStdDevField = pSumField = nullptr;

        std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference
        extractedAttributes.push_back(o);

        pBand = PyObject_GetAttrString(o, "band");
        extractedAttributes.push_back(pBand);
        if( ( pBand == nullptr ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand))
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'band\'" );
            FreePythonObjects(extractedAttributes);
            for(auto iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return nullptr;
        }

        pMinField = PyObject_GetAttrString(o, "min_field");
        extractedAttributes.push_back(pMinField);
        cmdObj->calcMin =  !(pMinField == nullptr || !RSGISPY_CHECK_STRING(pMinField));

        pMaxField = PyObject_GetAttrString(o, "max_field");
        extractedAttributes.push_back(pMaxField);
        cmdObj->calcMax = !(pMaxField == nullptr || !RSGISPY_CHECK_STRING(pMaxField));

        pMeanField = PyObject_GetAttrString(o, "mean_field");
        extractedAttributes.push_back(pMeanField);
        cmdObj->calcMean = !(pMeanField == nullptr || !RSGISPY_CHECK_STRING(pMeanField));

        pStdDevField = PyObject_GetAttrString(o, "std_dev_field");
        extractedAttributes.push_back(pStdDevField);
        cmdObj->calcStdDev = !(pStdDevField == nullptr || !RSGISPY_CHECK_STRING(pStdDevField));

        pSumField = PyObject_GetAttrString(o, "sum_field");
        extractedAttributes.push_back(pSumField);
        cmdObj->calcSum = !(pSumField == nullptr || !RSGISPY_CHECK_STRING(pSumField));

        // extract the values from the objects
        cmdObj->band = RSGISPY_INT_EXTRACT(pBand);
        // check the calcValue and extract fields if required
        if(cmdObj->calcMax)
        {
            cmdObj->maxField = RSGISPY_STRING_EXTRACT(pMaxField);
        }
        if(cmdObj->calcMean)
        {
            cmdObj->meanField = RSGISPY_STRING_EXTRACT(pMeanField);
        }
        if(cmdObj->calcMin)
        {
            cmdObj->minField = RSGISPY_STRING_EXTRACT(pMinField);
        }
        if(cmdObj->calcStdDev)
        {
            cmdObj->stdDevField = RSGISPY_STRING_EXTRACT(pStdDevField);
        }
        if(cmdObj->calcSum)
        {
            cmdObj->sumField = RSGISPY_STRING_EXTRACT(pSumField);
        }

        FreePythonObjects(extractedAttributes);
        bandStatsCmds.push_back(cmdObj);
    }

    try
    {
        rsgis::cmds::executePopulateRATWithStats(std::string(inputImage), std::string(clumpsImage), &bandStatsCmds, ratBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        for(auto iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter)
        {
            delete *iter;
        }
        return nullptr;
    }

    // free temp structs
    for(auto iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter)
    {
        delete *iter;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_PopulateRATWithPercentiles(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage, *clumpsImage;
    PyObject *pBandPercentilesCmds;
    unsigned int ratBand = 1;
    unsigned int band = 1;
    unsigned int numHistBins = 200;
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("clumps_img"),
                             RSGIS_PY_C_TEXT("img_band"), RSGIS_PY_C_TEXT("band_stats"),
                             RSGIS_PY_C_TEXT("n_hist_bins"), RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssIO|II:populate_rat_with_percentiles", kwlist, &inputImage, &clumpsImage, &band, &pBandPercentilesCmds, &numHistBins, &ratBand))
    {
        return nullptr;
    }

    if(!PySequence_Check(pBandPercentilesCmds))
    {
        PyErr_SetString(GETSTATE(self)->error, "bandstats argument must be a sequence");
        return nullptr;
    }

    // extract the attributes from the sequence of objects into our structs
    Py_ssize_t nPercentiles = PySequence_Size(pBandPercentilesCmds);
    std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*> bandPercentilesCmds;
    bandPercentilesCmds.reserve(nPercentiles);

    for(int i = 0; i < nPercentiles; ++i)
    {
        PyObject *o = PySequence_GetItem(pBandPercentilesCmds, i);     // the python object

        rsgis::cmds::RSGISBandAttPercentilesCmds *percObj = new rsgis::cmds::RSGISBandAttPercentilesCmds;   // the c++ object we need to pass pointers of

        // declare and initialise pointers for all the attributes of the struct
        PyObject *pBand, *pPercentile, *pFieldName;
        pBand = pPercentile = pFieldName = nullptr;

        std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference
        extractedAttributes.push_back(o);

        pPercentile = PyObject_GetAttrString(o, "percentile");
        extractedAttributes.push_back(pPercentile);
        if( ( pPercentile == nullptr ) || ( pPercentile == Py_None ) || !RSGISPY_CHECK_FLOAT(pPercentile))
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'percentile\'" );
            FreePythonObjects(extractedAttributes);
            for(auto iter = bandPercentilesCmds.begin(); iter != bandPercentilesCmds.end(); ++iter)
            {
                delete *iter;
            }
            delete percObj;
            return nullptr;
        }

        pFieldName = PyObject_GetAttrString(o, "field_name");
        extractedAttributes.push_back(pFieldName);
        if( ( pFieldName == nullptr ) || ( pFieldName == Py_None ) || !RSGISPY_CHECK_STRING(pFieldName))
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'fieldName\'" );
            FreePythonObjects(extractedAttributes);
            for(auto iter = bandPercentilesCmds.begin(); iter != bandPercentilesCmds.end(); ++iter)
            {
                delete *iter;
            }
            delete percObj;
            return nullptr;
        }

        percObj->percentile = RSGISPY_FLOAT_EXTRACT(pPercentile);
        percObj->fieldName = RSGISPY_STRING_EXTRACT(pFieldName);

        FreePythonObjects(extractedAttributes);
        bandPercentilesCmds.push_back(percObj);
    }

    try
    {
        rsgis::cmds::executePopulateRATWithPercentiles(std::string(inputImage), std::string(clumpsImage), band, &bandPercentilesCmds, ratBand, numHistBins);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        for(auto iter = bandPercentilesCmds.begin(); iter != bandPercentilesCmds.end(); ++iter)
        {
            delete *iter;
        }
        return nullptr;
    }

    // free temp structs
    for(auto iter = bandPercentilesCmds.begin(); iter != bandPercentilesCmds.end(); ++iter)
    {
        delete *iter;
    }

    Py_RETURN_NONE;
}


static PyObject *RasterGIS_PopulateCategoryProportions(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *categoriesImage, *outColsName, *majorityColName;
    const char *majClassNameField = "";
    const char *classNameField = "";
    int copyClassNames = false;
    unsigned int ratBandClumps = 1;
    unsigned int ratBandCats = 1;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_cats_img"), RSGIS_PY_C_TEXT("clumps_img"),
                             RSGIS_PY_C_TEXT("out_cols_name"), RSGIS_PY_C_TEXT("maj_col_name"),
                             RSGIS_PY_C_TEXT("cp_cls_names"), RSGIS_PY_C_TEXT("maj_cls_name_field"),
                             RSGIS_PY_C_TEXT("cls_name_field"), RSGIS_PY_C_TEXT("rat_band_clumps"),
                             RSGIS_PY_C_TEXT("rat_band_cats"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssss|issII:populate_rat_with_cat_proportions", kwlist, &categoriesImage, &clumpsImage, &outColsName, &majorityColName, &copyClassNames, &majClassNameField, &classNameField, &ratBandClumps, &ratBandCats))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executePopulateCategoryProportions(std::string(categoriesImage), std::string(clumpsImage), std::string(outColsName), std::string(majorityColName),
                                                        (copyClassNames != 0), std::string(majClassNameField), std::string(classNameField), ratBandClumps, ratBandCats);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_PopulateRATWithMode(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *inputImage, *outColsName;
    long noDataVal = 0;
    int useNoDataVal = false;
    int outNoDataVal = true;
    unsigned int ratBand = 1;
    unsigned int modeBand = 1;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("clumps_img"),
                             RSGIS_PY_C_TEXT("out_cols_name"), RSGIS_PY_C_TEXT("use_no_data"),
                             RSGIS_PY_C_TEXT("no_data_val"), RSGIS_PY_C_TEXT("out_no_data"),
                             RSGIS_PY_C_TEXT("mode_band"), RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sss|iliII:populate_rat_with_mode", kwlist, &inputImage, &clumpsImage, &outColsName, &useNoDataVal, &noDataVal, &outNoDataVal, &modeBand, &ratBand))
    {
        return nullptr;
    }
    
    try
    {
        bool useNoDataBool = (bool) useNoDataVal;
        bool outNoDataBool = (bool) outNoDataVal;
        rsgis::cmds::executePopulateRATWithMode(std::string(inputImage), std::string(clumpsImage), std::string(outColsName), useNoDataBool, noDataVal, outNoDataBool, modeBand, ratBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_ExportCol2GDALImage(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage, *outputFile, *imageFormat, *field;
    int dataType;
    int ratBand = 1;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("field"), RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssis|i:export_col_to_gdal_img", kwlist, &inputImage, &outputFile, &imageFormat, &dataType, &field, &ratBand))
    {
        return nullptr;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType) dataType;

    try
    {
        rsgis::cmds::executeExportCols2GDALImage(std::string(inputImage), std::string(outputFile), std::string(imageFormat), type, std::string(field), ratBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_Export2Ascii(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage, *outputFile;
    unsigned int ratBand = 1;
    PyObject *pFields;
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("out_file"),
                             RSGIS_PY_C_TEXT("fields"), RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssO|i:export_rat_cols_to_ascii", kwlist, &inputImage, &outputFile, &pFields, &ratBand))
    {
        return nullptr;
    }

    if(!PySequence_Check(pFields))
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return nullptr;
    }

    std::vector<std::string> fields = ExtractVectorStringFromSequence(pFields);
    if(fields.empty())
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeExport2Ascii(std::string(inputImage), std::string(outputFile), fields, ratBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_ColourClasses(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage, *classInField;
    PyObject *pClassColourPairs;
    int ratBand = 1;
    bool intKet = true;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("field"),
                             RSGIS_PY_C_TEXT("class_colours"), RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssO|i:colour_rat_classes", kwlist, &inputImage, &classInField, &pClassColourPairs, &ratBand))
    {
        return nullptr;
    }

    if(!PyDict_Check(pClassColourPairs))
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a dict");
        return nullptr;
    }

    std::map<size_t, rsgis::cmds::RSGISColourIntCmds> classPairsInt;
    std::map<std::string, rsgis::cmds::RSGISColourIntCmds> classPairsStr;

    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(pClassColourPairs, &pos, &key, &value))
    {
        if(RSGISPY_CHECK_INT(key))
        {
            intKet = true;
        }
        else if(RSGISPY_CHECK_STRING(key))
        {
            intKet = false;
        }
        else
        {
            PyErr_SetString(GETSTATE(self)->error, "dict keys must be ints or strings");
            return nullptr;
        }

        PyObject *pRed, *pGreen, *pBlue, *pAlpha;
        pRed = pGreen = pBlue = pAlpha = nullptr;

        std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference

        pRed = PyObject_GetAttrString(value, "red");
        extractedAttributes.push_back(pRed);
        if( ( pRed == nullptr ) || ( pRed == Py_None ) || !RSGISPY_CHECK_INT(pRed))
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'red\'" );
            FreePythonObjects(extractedAttributes);
            return nullptr;
        }

        pGreen = PyObject_GetAttrString(value, "green");
        extractedAttributes.push_back(pGreen);
        if( ( pGreen == nullptr ) || ( pGreen == Py_None ) || !RSGISPY_CHECK_INT(pGreen))
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'green\'" );
            FreePythonObjects(extractedAttributes);
            return nullptr;
        }

        pBlue = PyObject_GetAttrString(value, "blue");
        extractedAttributes.push_back(pBlue);
        if( ( pBlue == nullptr ) || ( pBlue == Py_None ) || !RSGISPY_CHECK_INT(pBlue))
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'blue\'" );
            FreePythonObjects(extractedAttributes);
            return nullptr;
        }

        pAlpha = PyObject_GetAttrString(value, "alpha");
        extractedAttributes.push_back(pAlpha);
        if( ( pAlpha == nullptr ) || ( pAlpha == Py_None ) || !RSGISPY_CHECK_INT(pAlpha))
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'alpha\'" );
            FreePythonObjects(extractedAttributes);
            return nullptr;
        }

        rsgis::cmds::RSGISColourIntCmds colour(RSGISPY_INT_EXTRACT(pRed), RSGISPY_INT_EXTRACT(pGreen), RSGISPY_INT_EXTRACT(pBlue), RSGISPY_INT_EXTRACT(pAlpha));

        if(intKet)
        {
            classPairsInt[(size_t)RSGISPY_INT_EXTRACT(key)] = colour;
        }
        else
        {
            classPairsStr[RSGISPY_STRING_EXTRACT(key)] = colour;
        }
        FreePythonObjects(extractedAttributes);
    }

    try
    {
        if(intKet)
        {
            rsgis::cmds::executeColourClasses(std::string(inputImage), std::string(classInField), classPairsInt, ratBand);
        }
        else
        {
            rsgis::cmds::executeColourStrClasses(std::string(inputImage), std::string(classInField), classPairsStr, ratBand);
        }
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *RasterGIS_StrClassMajority(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *baseSegment, *infoSegment, *baseClassCol, *infoClassCol;
    int ignoreZero = 1;
    int baseRatBand = 1;
    int infoRatBand = 1;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("base_clumps_img"), RSGIS_PY_C_TEXT("info_clumps_img"),
                             RSGIS_PY_C_TEXT("base_class_col"), RSGIS_PY_C_TEXT("info_class_col"),
                             RSGIS_PY_C_TEXT("ignore_zero"), RSGIS_PY_C_TEXT("rat_band_base"),
                             RSGIS_PY_C_TEXT("rat_band_info"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssss|iii:str_class_majority", kwlist, &baseSegment, &infoSegment, &baseClassCol,
                                                                        &infoClassCol, &ignoreZero, &baseRatBand, &infoRatBand))
        return nullptr;

    try
    {
        rsgis::cmds::executeStrClassMajority(std::string(baseSegment), std::string(infoSegment), std::string(baseClassCol), std::string(infoClassCol), infoRatBand, baseRatBand, infoRatBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *RasterGIS_FindNeighbours(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage;
    unsigned int ratBand = 1;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "s|I:find_neighbours", kwlist, &inputImage, &ratBand))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeFindNeighbours(std::string(inputImage), ratBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *RasterGIS_FindBoundaryPixels(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage, *outputFile;
    const char *imageFormat = "KEA";
    unsigned int ratBand = 1;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ss|sI:find_boundary_pixels", kwlist, &inputImage, &outputFile, &imageFormat, &ratBand))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeFindBoundaryPixels(std::string(inputImage), ratBand, std::string(outputFile), std::string(imageFormat));
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *RasterGIS_CalcBorderLength(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage, *outColsName;
    int iIgnoreZeroEdges;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("out_col"), RSGIS_PY_C_TEXT("ignore_zero_edges"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssi:calc_border_length", kwlist, &inputImage, &outColsName, &iIgnoreZeroEdges))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeCalcBorderLength(std::string(inputImage), (iIgnoreZeroEdges != 0), std::string(outColsName));
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *RasterGIS_CalcRelBorder(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage, *outColsName, *classNameField, *className;
    int iIgnoreZeroEdges;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("out_col"),
                             RSGIS_PY_C_TEXT("class_names_col"), RSGIS_PY_C_TEXT("class_name"),
                             RSGIS_PY_C_TEXT("ignore_zero_edges"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssssi:calc_rel_border", kwlist, &inputImage, &outColsName, &classNameField, &className, &iIgnoreZeroEdges))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeCalcRelBorder(std::string(inputImage), std::string(outColsName), std::string(classNameField), std::string(className), (iIgnoreZeroEdges != 0));
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *RasterGIS_DefineClumpTilePositions(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *tileImage, *outColsName;
    unsigned int tileOverlap, tileBoundary, tileBody;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("tile_img"),
                             RSGIS_PY_C_TEXT("out_col"), RSGIS_PY_C_TEXT("tile_overlap"),
                             RSGIS_PY_C_TEXT("tile_boundary"), RSGIS_PY_C_TEXT("tile_body"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssIII:define_clump_tile_positions", kwlist, &clumpsImage, &tileImage, &outColsName, &tileOverlap, &tileBoundary, &tileBody))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeDefineClumpTilePositions(std::string(clumpsImage), std::string(tileImage), std::string(outColsName), tileOverlap, tileBoundary, tileBody);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *RasterGIS_DefineBorderClumps(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *outColsName;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("out_col"),  nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ss:define_border_clumps", kwlist, &clumpsImage, &outColsName))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeDefineBorderClumps(std::string(clumpsImage), std::string(outColsName));
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *RasterGIS_GetGlobalClassStats(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *classField;
    PyObject *pAttFields, *pClassFields;
    int ratBand = 1;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("class_field"),
                             RSGIS_PY_C_TEXT("attributes"), RSGIS_PY_C_TEXT("cls_chg_cols"),
                             RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssOO|i:get_global_class_stats", kwlist, &clumpsImage, &classField, &pAttFields, &pClassFields, &ratBand))
    {
        return nullptr;
    }

    if(!PySequence_Check(pAttFields) || !PySequence_Check(pClassFields))
    {
        PyErr_SetString(GETSTATE(self)->error, "last 2 arguments must be sequences");
    }

    std::vector<std::string> attFields = ExtractVectorStringFromSequence(pAttFields);

    Py_ssize_t nFields = PySequence_Size(pClassFields);
    std::vector<rsgis::cmds::RSGISClassChangeFieldsCmds> classFields;
    classFields.reserve(nFields);

    for(int i = 0; i < nFields; ++i)
    {
        PyObject *o = PySequence_GetItem(pClassFields, i);     // the python object

        rsgis::cmds::RSGISClassChangeFieldsCmds classField;

        // declare and initialise pointers for all the attributes of the struct
        PyObject *name, *outName, *threshold;
        name = outName = threshold = nullptr;

        std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference later
        extractedAttributes.push_back(o);

        name = PyObject_GetAttrString(o, "cls_name");
        extractedAttributes.push_back(name);
        if( ( name == nullptr ) || ( name == Py_None ) || !RSGISPY_CHECK_STRING(name))
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'cls_name\'" );
            FreePythonObjects(extractedAttributes);
            return nullptr;
        }

        classField.name = RSGISPY_STRING_EXTRACT(name);
        classField.outName = 0; // Not using this
        classField.threshold = 0; // Not using this either

        classFields.push_back(classField);
        FreePythonObjects(extractedAttributes);
    }

    try
    {
        rsgis::cmds::executeGetGlobalClassStats(std::string(clumpsImage), std::string(classField), attFields, classFields, ratBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *RasterGIS_SelectClumpsOnGrid(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *inSelectField, *outSelectField, *eastingsCol, *northingsCol, *metricField, *methodStr;
    unsigned int rows, cols;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("in_sel_col"),
                             RSGIS_PY_C_TEXT("out_sel_col"), RSGIS_PY_C_TEXT("eastings_col"),
                             RSGIS_PY_C_TEXT("northings_col"), RSGIS_PY_C_TEXT("metric_col"),
                             RSGIS_PY_C_TEXT("method"), RSGIS_PY_C_TEXT("rows"),
                             RSGIS_PY_C_TEXT("cols"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssssssII:select_clumps_on_grid", kwlist, &clumpsImage, &inSelectField, &outSelectField, &eastingsCol, &northingsCol, &metricField, &methodStr, &rows, &cols))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeIdentifyClumpExtremesOnGrid(std::string(clumpsImage), std::string(inSelectField), std::string(outSelectField), std::string(eastingsCol), std::string(northingsCol), std::string(methodStr), rows, cols, std::string(metricField));
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *RasterGIS_CalcRelDiffNeighbourStats(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage;
    PyObject *fieldObj;
    int useAbsDiff;
    useAbsDiff = false;
    int ratBand;
    ratBand = 1;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("field_stats"),
                             RSGIS_PY_C_TEXT("use_abs_diff"), RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sOi|i:calc_rel_diff_neigh_stats", kwlist, &clumpsImage, &fieldObj, &useAbsDiff, &ratBand))
    {
        return nullptr;
    }
    
    // declare and initialise pointers for all the attributes of the struct
    PyObject *pField, *pMinField, *pMaxField, *pStdDevField, *pSumField, *pMeanField;
    pField = pMinField = pMaxField = pMeanField = pStdDevField = pSumField = nullptr;
    
    rsgis::cmds::RSGISFieldAttStatsCmds cmdObj;
    
    std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference
    extractedAttributes.push_back(fieldObj);
    
    pField = PyObject_GetAttrString(fieldObj, "field");
    extractedAttributes.push_back(pField);
    if( ( pField == nullptr ) || ( pField == Py_None ) )
    {
        PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'field\'" );
        FreePythonObjects(extractedAttributes);
        return nullptr;
    }
    
    pMinField = PyObject_GetAttrString(fieldObj, "min_field");
    extractedAttributes.push_back(pMinField);
    cmdObj.calcMin =  !(pMinField == nullptr || !RSGISPY_CHECK_STRING(pMinField));
    
    pMaxField = PyObject_GetAttrString(fieldObj, "max_field");
    extractedAttributes.push_back(pMaxField);
    cmdObj.calcMax = !(pMaxField == nullptr || !RSGISPY_CHECK_STRING(pMaxField));
    
    pMeanField = PyObject_GetAttrString(fieldObj, "mean_field");
    extractedAttributes.push_back(pMeanField);
    cmdObj.calcMean = !(pMeanField == nullptr || !RSGISPY_CHECK_STRING(pMeanField));
    
    pStdDevField = PyObject_GetAttrString(fieldObj, "std_dev_field");
    extractedAttributes.push_back(pStdDevField);
    cmdObj.calcStdDev = !(pStdDevField == nullptr || !RSGISPY_CHECK_STRING(pStdDevField));
    
    pSumField = PyObject_GetAttrString(fieldObj, "sum_field");
    extractedAttributes.push_back(pSumField);
    cmdObj.calcSum = !(pSumField == nullptr || !RSGISPY_CHECK_STRING(pSumField));
    
    // extract the values from the objects
    cmdObj.field = RSGISPY_STRING_EXTRACT(pField);
    // check the calcValue and extract fields if required
    if(cmdObj.calcMax)
    {
        cmdObj.maxField = RSGISPY_STRING_EXTRACT(pMaxField);
    }
    if(cmdObj.calcMean)
    {
        cmdObj.meanField = RSGISPY_STRING_EXTRACT(pMeanField);
    }
    if(cmdObj.calcMin)
    {
        cmdObj.minField = RSGISPY_STRING_EXTRACT(pMinField);
    }
    if(cmdObj.calcStdDev)
    {
        cmdObj.stdDevField = RSGISPY_STRING_EXTRACT(pStdDevField);
    }
    if(cmdObj.calcSum)
    {
        cmdObj.sumField = RSGISPY_STRING_EXTRACT(pSumField);
    }

    try
    {
        bool useAbsDiffBool = (bool)useAbsDiff;
        rsgis::cmds::executeCalcRelDiffNeighbourStats(std::string(clumpsImage), cmdObj, useAbsDiffBool, ratBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyObject *RasterGIS_PopulateRATWithMeanLitStats(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage, *clumpsImage, *meanLitImage, *meanLitCol, *pxlCountCol;
    PyObject *pBandAttStatsCmds;
    unsigned int ratBand = 1;
    unsigned int meanlitBand = 1;
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_vals_img"), RSGIS_PY_C_TEXT("clumps_img"),
                             RSGIS_PY_C_TEXT("mean_lit_img"), RSGIS_PY_C_TEXT("mean_lit_band"),
                             RSGIS_PY_C_TEXT("mean_lit_col"), RSGIS_PY_C_TEXT("pxl_count_col"),
                             RSGIS_PY_C_TEXT("band_stats"), RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssIssO|I:populate_rat_with_meanlit_stats", kwlist, &inputImage, &clumpsImage,
                                    &meanLitImage, &meanlitBand, &meanLitCol, &pxlCountCol, &pBandAttStatsCmds, &ratBand))
    {
        return nullptr;
    }
    
    if(!PySequence_Check(pBandAttStatsCmds))
    {
        PyErr_SetString(GETSTATE(self)->error, "bandstats argument must be a sequence");
        return nullptr;
    }
    
    // extract the attributes from the sequence of objects into our structs
    Py_ssize_t nCmds = PySequence_Size(pBandAttStatsCmds);
    std::vector<rsgis::cmds::RSGISBandAttStatsCmds*> bandStatsCmds;
    bandStatsCmds.reserve(nCmds);
    
    for(int i = 0; i < nCmds; ++i)
    {
        PyObject *o = PySequence_GetItem(pBandAttStatsCmds, i);     // the python object
        
        rsgis::cmds::RSGISBandAttStatsCmds *cmdObj = new rsgis::cmds::RSGISBandAttStatsCmds();   // the c++ object we need to pass pointers of
        
        // declare and initialise pointers for all the attributes of the struct
        PyObject *pBand, *pMinField, *pMaxField, *pStdDevField, *pSumField, *pMeanField;
        pBand = pBand = pMinField = pMaxField = pMeanField = pStdDevField = pSumField = nullptr;
        
        std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference
        extractedAttributes.push_back(o);
        
        pBand = PyObject_GetAttrString(o, "band");
        extractedAttributes.push_back(pBand);
        if( ( pBand == nullptr ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand))
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'band\'" );
            FreePythonObjects(extractedAttributes);
            for(auto iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return nullptr;
        }
        
        pMinField = PyObject_GetAttrString(o, "min_field");
        extractedAttributes.push_back(pMinField);
        cmdObj->calcMin =  !(pMinField == nullptr || !RSGISPY_CHECK_STRING(pMinField));
        
        pMaxField = PyObject_GetAttrString(o, "max_field");
        extractedAttributes.push_back(pMaxField);
        cmdObj->calcMax = !(pMaxField == nullptr || !RSGISPY_CHECK_STRING(pMaxField));
        
        pMeanField = PyObject_GetAttrString(o, "mean_field");
        extractedAttributes.push_back(pMeanField);
        cmdObj->calcMean = !(pMeanField == nullptr || !RSGISPY_CHECK_STRING(pMeanField));
        
        pStdDevField = PyObject_GetAttrString(o, "std_dev_field");
        extractedAttributes.push_back(pStdDevField);
        cmdObj->calcStdDev = !(pStdDevField == nullptr || !RSGISPY_CHECK_STRING(pStdDevField));
        
        pSumField = PyObject_GetAttrString(o, "sum_field");
        extractedAttributes.push_back(pSumField);
        cmdObj->calcSum = !(pSumField == nullptr || !RSGISPY_CHECK_STRING(pSumField));
        
        // extract the values from the objects
        cmdObj->band = RSGISPY_INT_EXTRACT(pBand);
        // check the calcValue and extract fields if required
        if(cmdObj->calcMax)
        {
            cmdObj->maxField = RSGISPY_STRING_EXTRACT(pMaxField);
        }
        if(cmdObj->calcMean)
        {
            cmdObj->meanField = RSGISPY_STRING_EXTRACT(pMeanField);
        }
        if(cmdObj->calcMin)
        {
            cmdObj->minField = RSGISPY_STRING_EXTRACT(pMinField);
        }
        if(cmdObj->calcStdDev)
        {
            cmdObj->stdDevField = RSGISPY_STRING_EXTRACT(pStdDevField);
        }
        if(cmdObj->calcSum)
        {
            cmdObj->sumField = RSGISPY_STRING_EXTRACT(pSumField);
        }
        
        FreePythonObjects(extractedAttributes);
        bandStatsCmds.push_back(cmdObj);
    }
    
    try
    {
        rsgis::cmds::executePopulateRATWithMeanLitStats(std::string(inputImage), std::string(clumpsImage), std::string(meanLitImage), meanlitBand, std::string(meanLitCol), std::string(pxlCountCol), &bandStatsCmds, ratBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        for(auto iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter)
        {
            delete *iter;
        }
        return nullptr;
    }
    
    // free temp structs
    for(auto iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter)
    {
        delete *iter;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_CollapseRAT(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *selectField, *outputFile, *imageFormat;
    int ratBand;
    ratBand = 1;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("select_col"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssss|i:collapse_rat", kwlist, &clumpsImage, &selectField, &outputFile, &imageFormat, &ratBand))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeCollapseRAT(std::string(clumpsImage), ratBand, std::string(selectField), std::string(outputFile), std::string(imageFormat));
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_ImportVecAtts(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *vectorFile, *vectorLyrName, *fidColName;
    PyObject *pColNamesList;
    int ratBand;
    ratBand = 1;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("vec_file"),
                             RSGIS_PY_C_TEXT("vec_lyr"), RSGIS_PY_C_TEXT("fid_col"),
                             RSGIS_PY_C_TEXT("col_names"), RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssssO|i:import_vec_atts", kwlist, &clumpsImage, &vectorFile, &vectorLyrName, &fidColName, &pColNamesList, &ratBand))
    {
        return nullptr;
    }
    
    try
    {
        std::vector<std::string> colNames;
        if(PySequence_Check(pColNamesList))
        {
            Py_ssize_t nCmds = PySequence_Size(pColNamesList);
            colNames.reserve(nCmds);
            
            for(int i = 0; i < nCmds; ++i)
            {
                PyObject *o = PySequence_GetItem(pColNamesList, i);     // get the python object
                
                std::string strVal = RSGISPY_STRING_EXTRACT(o); // Get string
                colNames.push_back(strVal);
            }
        }
        
        rsgis::cmds::executeImportVecAtts(std::string(clumpsImage), ratBand, std::string(vectorFile), std::string(vectorLyrName), std::string(fidColName), colNames);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyObject *RasterGIS_ApplyKNN(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inClumpsImage = "";
    unsigned int ratBand = 1;
    const char *inExtrapField = "";
    const char *outExtrapField = "";
    const char *trainRegionsField = "";
    PyObject *applyRegionsFieldObj;
    PyObject *pFields;
    unsigned int kFeatures = 12;
    float distThreshold = 100000;
    int distKNNInt = rsgis::cmds::rsgisKNNMahalanobis;
    int summeriseKNNInt = rsgis::cmds::rsgisKNNMean;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("in_extrap_col"),
                             RSGIS_PY_C_TEXT("out_extrap_col"), RSGIS_PY_C_TEXT("train_regions_col"),
                             RSGIS_PY_C_TEXT("apply_regions_col"), RSGIS_PY_C_TEXT("val_cols"),
                             RSGIS_PY_C_TEXT("k_feat"), RSGIS_PY_C_TEXT("dist_knn"),
                             RSGIS_PY_C_TEXT("summerise_knn"), RSGIS_PY_C_TEXT("dist_thres"),
                             RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssssOO|IiifI:apply_rat_knn", kwlist, &inClumpsImage, &inExtrapField, &outExtrapField, &trainRegionsField, &applyRegionsFieldObj, &pFields, &kFeatures, &distKNNInt, &summeriseKNNInt, &distThreshold, &ratBand))
    {
        return nullptr;
    }
    
    if(!PySequence_Check(pFields))
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return nullptr;
    }
    
    std::vector<std::string> fields = ExtractVectorStringFromSequence(pFields);
    if(fields.size() == 0)
    {
        return nullptr;
    }
    
    std::string applyRegionsField = "";
    bool applyRegions = false;
    if(RSGISPY_CHECK_STRING(applyRegionsFieldObj))
    {
        applyRegionsField = RSGISPY_STRING_EXTRACT(applyRegionsFieldObj);
        applyRegions = true;
    }
    
    try
    {
        rsgis::cmds::rsgisKNNDistCmd distKNN = static_cast<rsgis::cmds::rsgisKNNDistCmd>(distKNNInt);
        rsgis::cmds::rsgisKNNSummeriseCmd summeriseKNN = static_cast<rsgis::cmds::rsgisKNNSummeriseCmd>(summeriseKNNInt);
        
        rsgis::cmds::executeApplyKNN(std::string(inClumpsImage), ratBand, std::string(inExtrapField), std::string(outExtrapField), std::string(trainRegionsField), applyRegionsField, applyRegions, fields, kFeatures, distKNN, distThreshold, summeriseKNN);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_HistoSampling(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inClumpsImage = "";
    unsigned int ratBand = 1;
    const char *varCol = "";
    const char *outSelectCol = "";
    const char *classVal = "";
    PyObject *classColumnObj;
    float binWidth = 1;
    float propOfSample = 0.1;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("val_col"),
                             RSGIS_PY_C_TEXT("out_sel_col"), RSGIS_PY_C_TEXT("prop_sample"),
                             RSGIS_PY_C_TEXT("bin_width"), RSGIS_PY_C_TEXT("cls_col"),
                             RSGIS_PY_C_TEXT("class_val"), RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssff|OsI:histo_sampling", kwlist, &inClumpsImage, &varCol, &outSelectCol, &propOfSample, &binWidth, &classColumnObj, &classVal, &ratBand))
    {
        return nullptr;
    }
    
    std::string classColumn = "";
    bool classRestrict = false;
    if(RSGISPY_CHECK_STRING(classColumnObj))
    {
        classColumn = RSGISPY_STRING_EXTRACT(classColumnObj);
        classRestrict = true;
    }
    
    try
    {
        rsgis::cmds::executeHistSampling(std::string(inClumpsImage), ratBand, std::string(varCol), std::string(outSelectCol), propOfSample, binWidth, classRestrict, classColumn, std::string(classVal));
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyObject *RasterGIS_FitHistGausianMixtureModel(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inClumpsImage = "";
    unsigned int ratBand = 1;
    const char *varCol = "";
    const char *outH5File = "";
    PyObject *outHistFileObj;
    const char *classVal = "";
    const char *classColumn = "";
    float binWidth = 1;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("out_h5_file"),
                             RSGIS_PY_C_TEXT("out_hist_file"), RSGIS_PY_C_TEXT("val_col"),
                             RSGIS_PY_C_TEXT("bin_width"), RSGIS_PY_C_TEXT("cls_col"),
                             RSGIS_PY_C_TEXT("cls_val"), RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssOsfss|I:fit_hist_gausian_mixture_model", kwlist, &inClumpsImage, &outH5File, &outHistFileObj, &varCol, &binWidth, &classColumn, &classVal, &ratBand))
    {
        return nullptr;
    }
    
    std::string outHistFile = std::string();
    bool outputHist = false;
    if(RSGISPY_CHECK_STRING(outHistFileObj))
    {
        outHistFile = RSGISPY_STRING_EXTRACT(outHistFileObj);
        outputHist = true;
    }
    
    try
    {
        rsgis::cmds::executeFitHistGausianMixtureModel(std::string(inClumpsImage), ratBand, std::string(outH5File), std::string(varCol), binWidth, std::string(classColumn), std::string(classVal), outputHist, outHistFile);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyObject *RasterGIS_ClassSplitFitHistGausianMixtureModel(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inClumpsImage = "";
    unsigned int ratBand = 1;
    const char *varCol = "";
    const char *outColumn = "";
    const char *classVal = "";
    const char *classColumn = "";
    float binWidth = 1;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("out_col"),
                             RSGIS_PY_C_TEXT("val_col"), RSGIS_PY_C_TEXT("bin_width"),
                             RSGIS_PY_C_TEXT("cls_col"), RSGIS_PY_C_TEXT("cls_val"),
                             RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssfss|I:class_split_fit_hist_gausian_mixture_model", kwlist, &inClumpsImage, &outColumn, &varCol, &binWidth, &classColumn, &classVal, &ratBand))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeClassSplitFitHistGausianMixtureModel(std::string(inClumpsImage), ratBand, std::string(outColumn), std::string(varCol), binWidth, std::string(classColumn), std::string(classVal));
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_PopulateRATWithPropValidPxls(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *inputImage, *outColsName;
    float noDataVal = 0;
    unsigned int ratBand = 1;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("clumps_img"),
                             RSGIS_PY_C_TEXT("out_col"), RSGIS_PY_C_TEXT("no_data_val"),
                             RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssf|I:populate_rat_with_prop_valid_pxls", kwlist, &inputImage, &clumpsImage, &outColsName, &noDataVal, &ratBand))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeCalcPropOfValidPixelsInClump(std::string(inputImage), std::string(clumpsImage), ratBand, std::string(outColsName), noDataVal);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyObject *RasterGIS_Calc1DJMDistance(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *varCol, *classCol, *class1Val, *class2Val;
    float binWidth = 0;
    unsigned int ratBand = 1;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("val_col"),
                             RSGIS_PY_C_TEXT("bin_width"), RSGIS_PY_C_TEXT("cls_col"),
                             RSGIS_PY_C_TEXT("class1"), RSGIS_PY_C_TEXT("class2"),
                             RSGIS_PY_C_TEXT("rat_band"), nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssfsss|I:calc_1d_jm_distance", kwlist, &clumpsImage, &varCol, &binWidth, &classCol, &class1Val, &class2Val, &ratBand))
    {
        return nullptr;
    }

    PyObject *outVal = PyTuple_New(1);
    try
    {
        double dist = rsgis::cmds::executeCalc1DJMDistance(std::string(clumpsImage), std::string(varCol), binWidth, std::string(classCol), std::string(class1Val), std::string(class2Val), ratBand);
        
        if(PyTuple_SetItem(outVal, 0, Py_BuildValue("d", dist)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'dist\' value to the PyObject...");
        }
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    return outVal;
}

static PyObject *RasterGIS_Calc2DJMDistance(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *var1Col, *var2Col, *classCol, *class1Val, *class2Val;
    float var1BinWidth = 0;
    float var2BinWidth = 0;
    unsigned int ratBand = 1;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("val1_col"),
                             RSGIS_PY_C_TEXT("val2_col"), RSGIS_PY_C_TEXT("val1_bin_width"),
                             RSGIS_PY_C_TEXT("val2_bin_width"), RSGIS_PY_C_TEXT("cls_col"),
                             RSGIS_PY_C_TEXT("class1"), RSGIS_PY_C_TEXT("class2"),
                             RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssffsss|I:calc_2d_jm_distance", kwlist, &clumpsImage, &var1Col, &var2Col, &var1BinWidth, &var2BinWidth, &classCol, &class1Val, &class2Val, &ratBand))
    {
        return nullptr;
    }
    
    PyObject *outVal = PyTuple_New(1);
    try
    {
        double dist = rsgis::cmds::executeCalc2DJMDistance(std::string(clumpsImage), std::string(var1Col), std::string(var2Col), var1BinWidth, var2BinWidth, std::string(classCol), std::string(class1Val), std::string(class2Val), ratBand);
        
        if(PyTuple_SetItem(outVal, 0, Py_BuildValue("d", dist)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'dist\' value to the PyObject...");
        }
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    return outVal;
}

static PyObject *RasterGIS_CalcBhattacharyyaDistance(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *clumpsImage, *varCol, *classCol, *class1Val, *class2Val;
    unsigned int ratBand = 1;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("val_col"),
                             RSGIS_PY_C_TEXT("cls_col"), RSGIS_PY_C_TEXT("class1"),
                             RSGIS_PY_C_TEXT("class2"), RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssss|I:calc_bhattacharyya_distance", kwlist, &clumpsImage, &varCol, &classCol, &class1Val, &class2Val, &ratBand))
    {
        return nullptr;
    }
    
    PyObject *outVal = PyTuple_New(1);
    try
    {
        double dist = rsgis::cmds::executeCalcBhattacharyyaDistance(std::string(clumpsImage), std::string(varCol), std::string(classCol), std::string(class1Val), std::string(class2Val), ratBand);
        
        if(PyTuple_SetItem(outVal, 0, Py_BuildValue("d", dist)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'dist\' value to the PyObject...");
        }
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    return outVal;
}

static PyObject *RasterGIS_ExportClumps2Images(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *inputImage, *outputBaseName, *outFileExt, *imageFormat;
    int binaryOut = false;
    int ratBand = 1;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("clumps_img"), RSGIS_PY_C_TEXT("out_img_base"),
                             RSGIS_PY_C_TEXT("bin_out"), RSGIS_PY_C_TEXT("out_img_ext"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("rat_band"), nullptr};
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssiss|i:export_clumps_to_images", kwlist, &inputImage, &outputBaseName, &binaryOut, &outFileExt, &imageFormat, &ratBand))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeExportClumps2Images(std::string(inputImage), std::string(outputBaseName), std::string(outFileExt), std::string(imageFormat), (bool)binaryOut, ratBand);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyMethodDef RasterGISMethods[] = {
    {"pop_rat_img_stats", (PyCFunction)RasterGIS_PopulateStats, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.pop_rat_img_stats(clumps_img=string, add_clr_tab=boolean, calc_pyramids=boolean, ignore_zero=boolean, rat_band=int)\n"
"Populates header statics (e.g., builds histogram) and pyramids for thematic images.\n"
"Note, this function expects that the image file format supports a raster attribute table (RAT) so will therefore not work with formats such as GTIFF\n"
"\n"
":param clumps_img: is a string containing the name of the input clump file\n"
":param add_clr_tab: is a boolean to specify whether a colour table should created and added (colours will be random) (Optional, default = True)\n"
":param calc_pyramids: is a boolean to specify where overview images could be created (Optional, default = True)\n"
":param ignore_zero: is a boolean specifying whether zero should be ignored (i.e., set as a no data value). (Optional, default = True)\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"   from rsgislib import rastergis\n"
"   clumps='injune_p142_casi_sub_utm_segs_nostats_addstats.kea'\n"
"   pyramids=True\n"
"   colourtable=True\n"
"   rastergis.pop_rat_img_stats(clumps, colourtable, pyramids)\n"
"\n"
"\n"},

    {"copy_rat", (PyCFunction)RasterGIS_CopyRAT, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.copy_rat(clumps_img, output_img, rat_band=1)\n"
"Copies a GDAL RAT from one image to another\n"
"\n"
":param clumps_img: is a string containing the name and path for the image with RAT from which columns are to copied from.\n"
":param output_img: is a string containing the name of the file to which the columns are to be copied.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"   from rsgislib import rastergis\n"
"   clumps = './RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea'\n"
"   output_img = './TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_cptab.kea'\n"
"   rastergis.copy_rat(clumps, output_img)\n"
"\n"},

{"copy_gdal_rat_columns", (PyCFunction)RasterGIS_CopyGDALATTColumns, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.copy_gdal_rat_columns(clumps_img, output_img, fields, copy_colours=True, copy_hist=True, rat_band=1)\n"
"Copies GDAL RAT columns from one image to another\n"
"\n"
":param clumps_img: is a string containing the name and path for the image with RAT from which columns are to copied from.\n"
":param output_img: is a string containing the name of the file to which the columns are to be copied.\n"
":param fields: is a sequence of strings containing the names of the fields to copy\n"
":param copy_colours: is a bool specifying if the colour columns should be copied (default = True)\n"
":param copy_hist: is a bool specifying if the histogram  should be copied (default = True)\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"   from rsgislib import rastergis\n"
"   table = './RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea'\n"
"   image = './TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_cpcols.kea'\n"
"   fields = ['NIRAvg', 'BlueAvg', 'GreenAvg', 'RedAvg']\n"
"   rastergis.copy_gdal_rat_columns(image, table, fields)\n"
"\n"
"To copy a subset of columns from one RAT to a new file the following can be used::\n"
"\n"
"   import rsgislib\n"
"   import rsgislib.imageutils\n"
"   from rsgislib import rastergis\n"
"   rat_band=1\n"
"   table='inRAT.kea'\n"
"   output='outRAT_nir_only.kea'\n"
"   bands = [rat_band]\n"
"   rsgislib.imageutils.selectImageBands(table, output,'KEA', rsgislib.TYPE_32INT, bands)\n"
"   fields = ['NIRAvg']\n"
"   rastergis.copy_gdal_rat_columns(table, output, fields, copycolours=True, copyhist=True, rat_band=rat_band)\n"
"\n"},

{"clumps_spatial_location", (PyCFunction)RasterGIS_SpatialLocation, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.clumps_spatial_location(clumps_img=string, eastings=string, northings=string, rat_band=int)\n"
"Adds spatial location columns to the attribute table\n"
"\n"
":param inputImage: is a string containing the name of the input image file\n"
":param eastings: is a string containing the name of the eastings field\n"
":param northings: is a string containing the name of the northings field\n"
":param rat_band: is an integer containing the band number for the RAT (Optional, default = 1)\n"
"\n"
".. code:: python\n"
"\n"
"   from rsgislib import rastergis\n"
"   image = 'injune_p142_casi_sub_utm_segs_spatloc_eucdist.kea'\n"
"   eastings = 'Easting'\n"
"   northings = 'Northing'\n"
"   rastergis.clumps_spatial_location(image, eastings, northings)\n"
"\n"},
    
{"clumps_spatial_extent", (PyCFunction)RasterGIS_SpatialExtent, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.clumps_spatial_extent(clumps_img=string, min_xx=string, min_xy=string, max_xx=string, max_xy=string, min_yx=string, min_yy=string, max_yx=string, max_yy=string, rat_band=int)\n"
"Adds spatial extent for each clump to the attribute table\n"
"\n"
":param clumps_img: is a string containing the name of the input image file\n"
":param min_xx: is a string containing the name of the min X X field\n"
":param min_xy: is a string containing the name of the min X Y field\n"
":param max_xx: is a string containing the name of the max X X field\n"
":param max_xy: is a string containing the name of the max X Y field\n"
":param min_yx: is a string containing the name of the min Y X field\n"
":param min_yy: is a string containing the name of the min Y Y field\n"
":param max_yx: is a string containing the name of the max Y X field\n"
":param max_yy: is a string containing the name of the max Y Y field\n"
":param rat_band: is an integer containing the band number for the RAT (Optional, default = 1)\n"
"\n"
".. code:: python\n"
"\n"
"   from rsgislib import rastergis\n"
"   image = 'injune_p142_casi_sub_utm_segs_spatloc_eucdist.kea'\n"
"   minX_X = 'minXX'\n"
"   minX_Y = 'minXY'\n"
"   maxX_X = 'maxXX'\n"
"   maxX_Y = 'maxXY'\n"
"   minY_X = 'minYX'\n"
"   minY_Y = 'minYY'\n"
"   maxY_X = 'maxYX'\n"
"   maxY_Y = 'maxYY'\n"
"   rastergis.clumps_spatial_extent(image, minX_X, minX_Y, maxX_X, maxX_Y, minY_X, minY_Y, maxY_X, maxY_Y)\n"
"\n"},

    {"populate_rat_with_stats", (PyCFunction)RasterGIS_PopulateRATWithStats, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.populate_rat_with_stats(input_img=string, clumps_img=string, band_stats=rsgislib.rastergis.BandAttStats, rat_band=int)\n"
"Populates an attribute table with statistics from an input values image.\n"
"\n"
":param input_img: is a string containing the name of the input image file from which the clumps are to populated.\n"
":param clumps_img: is a string containing the name of the input clumps image file\n"
":param band_stats: is a sequence of rsgislib.rastergis.BandAttStats objects that have attributes in line with rsgis.cmds.RSGISBandAttStatsCmds\n"
"        * band: int defining the image band to process\n"
"        * min_field: string defining the name of the field for min value\n"
"        * max_field: string defining the name of the field for max value\n"
"        * sum_field: string defining the name of the field for sum value\n"
"        * mean_field: string defining the name of the field for mean value\n"
"        * std_dev_field: string defining the name of the field for standard deviation value\n"
"* rat_band is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"	from rsgislib import rastergis\n"
"	clumps='./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_popstats.kea'\n"
"	input='./Rasters/injune_p142_casi_sub_utm.kea'\n"
"	bs = []\n"
"	bs.append(rastergis.BandAttStats(band=1, min_field='b1Min', max_field='b1Max', mean_field='b1Mean', sum_field='b1Sum', std_dev_field='b1StdDev'))\n"
"	bs.append(rastergis.BandAttStats(band=2, min_field='b2Min', max_field='b2Max', mean_field='b2Mean', sum_field='b2Sum', std_dev_field='b2StdDev'))\n"
"	bs.append(rastergis.BandAttStats(band=3, min_field='b3Min', max_field='b3Max', mean_field='b3Mean', sum_field='b3Sum', std_dev_field='b3StdDev'))\n"
"	rastergis.populate_rat_with_stats(input, clumps, bs)\n"
"\n"},

    {"populate_rat_with_percentiles", (PyCFunction)RasterGIS_PopulateRATWithPercentiles, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.populate_rat_with_percentiles(input_img=string, clumps_img=string, img_band=int, band_stats=rsgislib.rastergis.BandAttStats, n_hist_bins=int, rat_band=int)\n"
"Populates an attribute table with a percentile of the pixel values from an image.\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param clumps_img: is a string containing the name of the input clump file\n"
":param img_band: is an int which specifies the image band (from valsimage) for which the stats are to be calculated\n"
":param band_stats: is a sequence of objects that have attributes matching rsgislib.rastergis.BandAttPercentiles\n"
"        * percentile: float defining the percentile to calculate (Valid range is 0 - 100)\n"
"        * field_name: string defining the name of the field to use for this percentile\n"
":param n_hist_bins: is an optional (default = 200) integer specifying the number of bins within the histogram (note this governs the accuracy to which percentile can be calculated).\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   clumpsImage = './TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_popstats.kea'\n"
"   band=1\n"
"   bandPercentiles = []\n"
"   bandPercentiles.append(rastergis.BandAttPercentiles(percentile=25.0, field_name='B1Per25'))\n"
"   bandPercentiles.append(rastergis.BandAttPercentiles(percentile=50.0, field_name='B1Per50'))\n"
"   bandPercentiles.append(rastergis.BandAttPercentiles(percentile=75.0, field_name='B1Per75'))\n"
"   rastergis.populate_rat_with_percentiles(inputImage, clumpsImage, band, bandPercentiles)\n"
"\n"},

   {"populate_rat_with_cat_proportions", (PyCFunction)RasterGIS_PopulateCategoryProportions, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.populate_rat_with_cat_proportions(in_cats_img=string, clumps_img=string, out_cols_name=string, maj_col_name=string, cp_cls_names=boolean, maj_cls_name_field=string cls_name_field=string, rat_band_clumps=int, rat_band_cats=int)\n"
"Populates the attribute table with the proportions of intersecting categories\n"
"\n"
":param in_cats_img: is a string containing the name of the categories (classification) image file from which the propotions are calculated\n"
":param clumps_img: is a string containing the name of the input clump file to which the proportions are to be populated.\n"
":param out_cols_name: is a string representing the base name for the output columns containing the proportions.\n"
":param maj_col_name: is a string for name of the field which will hold the majority class.\n"
":param cp_cls_names: is a boolean defining whether class names should be copied (Optional, Default = false).\n"
":param maj_cls_name_field: is a string for the output column within the clumps image with the majority class names field (Optional, only used if copyClassNames == True)\n"
":param cls_name_field: is a string with the name of the column within the categories image for the class names (Optional, only used if copyClassNames == True)\n"
":param rat_band_clumps: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated in the clumps image.\n"
":param rat_band_cats: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated in the catagories image.\n"
"\n"},
    
{"populate_rat_with_mode", (PyCFunction)RasterGIS_PopulateRATWithMode, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.populate_rat_with_mode(input_img=string, clumps_img=string, out_cols_name=string, use_no_data=boolean, no_data_val=long, out_no_data=boolean, mode_band=uint, rat_band=uint)\n"
"Populates the attribute table with the mode of from a single band in the input image.\n"
"Note this only makes sense if the input pixel values are integers.\n"
"\n"
":param input_img: is a string containing the name of the input image file from which the mode is calculated\n"
":param clumps_img: is a string containing the name of the input clump file to which the mode will be populated.\n"
":param out_cols_name: is a string representing the name for the output column containing the mode.\n"
":param use_no_data: is a boolean defining whether the no data value should be ignored (Optional, Default = False).\n"
":param no_data_val: is a long defining the no data value to be used (Optional, Default = 0)\n"
":param out_no_data: is a boolean to specify that although the no data value should be used for the calculation it should not be outputted to the RAT as a output value unless there is no valid data within the clump. (Default = True)\n"
":param mode_band: is an optional (default = 1) integer parameter specifying the image band for which the mode is to be calculated.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated in the clumps image.\n"
"\n"},

   {"export_col_to_gdal_img", (PyCFunction)RasterGIS_ExportCol2GDALImage, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.export_col_to_gdal_img(clumps_img, output_img, gdalformat, datatype, field, rat_band=1)\n"
"Exports column of the raster attribute table as bands in a GDAL image.\n"
"\n"
":param clumps_img: is a string containing the name of the input image file with RAT\n"
":param output_img: is a string containing the name of the output gdal file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
":param field: is a string, providing the name of the column to be exported.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"   from rsgislib import rastergis\n"
"   clumps='./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea'\n"
"   output_img='./TestOutputs/RasterGIS/injune_p142_casi_rgb_export.kea'\n"
"   gdalformat = 'KEA'\n"
"   datatype = rsgislib.TYPE_32FLOAT\n"
"   field = 'RedAvg'\n"
"   rastergis.export_col_to_gdal_img(clumps, output_img, gdalformat, datatype, field)"
"\n"},
    
    {"export_rat_cols_to_ascii",  (PyCFunction)RasterGIS_Export2Ascii, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.export_rat_cols_to_ascii(clumps_img, out_file, fields, rat_band=1)\n"
"Exports selected columns from a GDAL RAT to ASCII file (comma separated). The first column is the object ID (FID).\n"
"\n"
":param clumps_img: is a string containing the name of the input RAT.\n"
":param out_file: is a string containing the name of the output file.\n"
":param fields: is a sequence of strings containing the field names.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"   from rsgislib import rastergis\n"
"   clumps='./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea'\n"
"   outfile='./TestOutputs/RasterGIS/injune_p142_casi_rgb_exportascii.txt'\n"
"   fields = ['BlueAvg', 'GreenAvg', 'RedAvg']\n"
"   rastergis.export_rat_cols_to_ascii(clumps, outfile, fields)\n"
"\n"},

{"colour_rat_classes", (PyCFunction)RasterGIS_ColourClasses, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.colour_rat_classes(clumps_img, field, class_colours, rat_band)\n"
"Sets a colour table for a set of classes within the attribute table\n"
"\n"
":param clumps_img: is a string containing the name of the input file\n"
":param field: is a string containing the name of the input class field (class can be a string or integer).\n"
":param class_colours: is dict mapping int class ids to an object having the following attributes:\n"
"        * red: int defining the red colour component (0 - 255)\n"
"        * green: int defining the green colour component (0 - 255)\n"
"        * blue: int defining the bluecolour component (0 - 255)\n"
"        * alpha: int defining the alpha colour component (0 - 255)\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"   import collections\n"
"   from rsgislib import rastergis\n"
"   clumps='./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_col.kea'\n"
"   field = 'outClass'\n"
"   classcolours = {}\n"
"   colourCat = collections.namedtuple('ColourCat', ['red', 'green', 'blue', 'alpha'])\n"
"   classcolours[0] = colourCat(red=200, green=50, blue=50, alpha=255)\n"
"   classcolours[1] = colourCat(red=200, green=240, blue=50, alpha=255)\n"
"   rastergis.colour_rat_classes(clumps, field, classcolours)\n"
"\n"},

    {"str_class_majority", (PyCFunction)RasterGIS_StrClassMajority, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.str_class_majority(base_clumps_img, info_clumps_img, base_class_col, info_class_col, ignore_zero=True, rat_band_base=1, rat_band_info=1)\n"
"Finds the majority for class (string - field) from a set of small objects to large objects\n"
"\n"
":param base_clumps_img: is a the base clumps file, to be attribured.\n"
":param info_clumps_img: is the file to take attributes from.\n"
":param base_class_col: the output column name in the baseSegment file.\n"
":param info_class_col: is the colum name in the infoSegment file.\n"
":param ignore_zero: is a boolean specifying if zeros should be ignored in input layer. If set to false values of 0 will be included when calculating the class majority, otherwise the majority calculation will only consider objects with a value greater than 0.\n"
":param rat_band_base: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated in the base clumps.\n"
":param rat_band_info: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated in the info clumps.\n"
"\n"
".. code:: python\n"
"\n"
"	from rsgislib import rastergis\n"
"	clumps='./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_popstats.kea'\n"
"	classRAT='./TestOutputs/RasterGIS/reInt_rat.kea'\n"
"	rastergis.str_class_majority(clumps, classRAT, 'class_dst', 'class_src')\n"
"\n"},

    {"find_neighbours", (PyCFunction)RasterGIS_FindNeighbours, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.find_neighbours(clumps_img, rat_band)\n"
"Finds the clump neighbours from an image\n"
"\n"
":param clumps_img: is a string containing the name of the input image file\n"
":param rat_band: is an int containing band for which the neighbours are to be calculated for (Optional, Default = 1)\n"
"\n"},

    {"find_boundary_pixels", (PyCFunction)RasterGIS_FindBoundaryPixels, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.find_boundary_pixels(clumps_img, output_img, gdalformat, rat_band)\n"
"Identifies the pixels on the boundary of the clumps\n"
"\n"
":param clumps_img: is a string containing the name of the input image file\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - (Optional, Default = 'KEA')\n"
":param rat_band: is an int containing band for which the neighbours are to be calculated for (Optional, Default = 1)\n"
"\n"},

    {"calc_border_length", (PyCFunction)RasterGIS_CalcBorderLength, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.calc_border_length(clumps_img, out_col, ignore_zero_edges)\n"
"Calculate the border length of clumps\n"
"\n"
":param clumps_img: is a string containing the name of the input image file\n"
":param out_col: is a string with the output column name\n"
":param ignore_zero_edges: is a boolean specifying whether zero edges (i.e., no data) should be ignored\n"

"\n"},

    {"calc_rel_border", (PyCFunction)RasterGIS_CalcRelBorder, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.calc_rel_border(clumps_img, out_col, class_names_col, class_name, ignore_zero_edges)\n"
"Calculates the relative border length of the clumps to a class\n"
"\n"
":param inputImage: is a string containing the name of the input image file\n"
":param out_col: is a string specifying the output column name\n"
":param class_names_col: is a string specifying the column which holds the class names\n"
":param class_name: is a string specifying the class for which the relative boarder is to be calculated.\n"
":param ignore_zero_edges: is a boolean specifying whether zero edges (i.e., no data) should be ignored\n"
"\n"},

    {"define_clump_tile_positions", (PyCFunction)RasterGIS_DefineClumpTilePositions, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.define_clump_tile_positions(clumps_img, tile_img, out_col, tile_overlap, tile_boundary, tile_body)\n"
"Defines the position within the file of the clumps.\n"
"\n"
":param clumps_img: is a string containing the name of the input clump file\n"
":param tile_img: is a string containing the name of the input tile image\n"
":param out_col: is a string containing the name of the output column\n"
":param tile_overlap: is an unsigned int defining the overlap between tiles\n"
":param tile_boundary: is an unsigned int\n"
":param tile_body: is an unsigned int\n"
"\n"},

    {"define_border_clumps", (PyCFunction)RasterGIS_DefineBorderClumps, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.define_border_clumps(clumps_img, out_col)\n"
"Defines the clumps which are on the border within the file of the clumps using a mask\n"
"\n"
":param clumps_img: is a string containing the name of the input clump file\n"
":param out_col: is a string containing the name of the output column where a value of 1 indicates a border clumps\n"
"\n"},

   {"get_global_class_stats", (PyCFunction)RasterGIS_GetGlobalClassStats, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.get_global_class_stats(clumps_img, class_field, attributes, cls_chg_cols, rat_band)\n"
"Similar to 'findChangeClumpsFromStdDev' but rather than applying a threshold to calculate change clumps adds global (over all objects) class mean and standard deviation to RAT.\n"
"\n"
":param clumps_img: is a string containing the name of the input clump file\n"
":param class_field: is a string providing the name of the column containing classes.\n"
":param attributes: is a sequence of strings containing the columns to use when detecting change.\n"
":param cls_chg_cols: is a sequence of python objects having the following attributes:\n"
"   * cls_name - The class name in which change is going to be search for\n"
":param rat_band: is an int containing band for which the neighbours are to be calculated for (Optional, Default = 1)\n"
"\n"
".. code:: python\n"
"\n"
"   from rsgislib import rastergis\n"
"   clumpsImage='injune_p142_casi_sub_utm_segs_popstats.kea'\n"
"   changeFeatVals = []\n"
"   changeFeatVals.append(rastergis.ChangeFeat(cls_name='Forest'))\n"
"   changeFeatVals.append(rastergis.ChangeFeat(cls_name='Scrub-Shrub))\n"
"   rastergis.get_global_class_stats(clumpsImage, 'ClassName', ['NDVI'], changeFeatVals)\n"
"\n"},

{"select_clumps_on_grid", (PyCFunction)RasterGIS_SelectClumpsOnGrid, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.select_clumps_on_grid(clumps_img, in_sel_col, out_sel_col, eastings_col, northings_col, metric_col, method, rows, cols)\n"
"Selects a segment within a regular grid pattern across the scene. The clump is selected based on the minimum, maximum or closest to the mean.\n"
"\n"
":param clumps_img: is a string containing the name of the input clump file\n"
":param in_sel_col: is a string which defines the column name where a value of 1 defines the clumps which will be included in the analysis.\n"
":param out_sel_col: is a string which defines the column name where a value of 1 defines the clumps selected by the analysis.\n"
":param eastings_col: is a string which defines a column with a eastings for each clump.\n"
":param northings_col: is a string which defines a column with a northings for each clump.\n"
":param metric_col: is a string which defines a column with a value for each clump which will be used for the distance, min, or max anaylsis.\n"
":param method: is a string which defines whether the minimum, maximum or mean method of selecting a clump will be used (values can be either min, max or mean).\n"
":param rows: is an unsigned integer which defines the number of rows within which a clump will be selected.\n"
":param cols: is an unsigned integer which defines the number of columns within which a clump will be selected.\n"
"\n"},

{"calc_rel_diff_neigh_stats", (PyCFunction)RasterGIS_CalcRelDiffNeighbourStats, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.calc_rel_diff_neigh_stats(clumps_img, field_stats, use_abs_diff, rat_band)\n"
"Calculates the difference (relative or absolute) between each clump and it's\n"
"neighbours. The differences can be summarised as min, max, mean, std dev or sum.\n"
"\n"
":param clumps_img: is a string containing the name of the input clump file\n"
":param field_stats: has the following fields\n"
"      * field: string defining the field in the RAT to compare to.\n"
"      * min_field: string defining the name of the field for min value\n"
"      * max_field: string defining the name of the field for max value\n"
"      * sum_field: string defining the name of the field for sum value\n"
"      * mean_field: string defining the name of the field for mean value\n"
"      * std_dev_field: string defining the name of the field for standard deviation value\n"
":param use_abs_diff: calculate the absolute difference."
":param rat_band: is the image band with which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"    import rsgislib.rastergis\n"
"    inputImage = './RapidEye_20130625_lat53lon389_tid3063312_oid167771_rad_toa_segs_neigh.kea'\n"
"    ratBand = 1\n"
"    rsgislib.rastergis.find_neighbours(inputImage, ratBand)\n"
"    fieldInfo = rsgislib.rastergis.FieldAttStats(field='NIRMean', min_field='MinNIRMeanDiff', max_field='MaxNIRMeanDiff')\n"
"    rsgislib.rastergis.calc_rel_diff_neigh_stats(inputImage, fieldInfo, False, ratBand)\n"
"\n"},

{"populate_rat_with_meanlit_stats", (PyCFunction)RasterGIS_PopulateRATWithMeanLitStats, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.populate_rat_with_meanlit_stats(in_vals_img=string, clumps_img=string, mean_lit_img=string, mean_lit_band=int, mean_lit_col=string, pxl_count_col=string, band_stats=rsgislib.rastergis.BandAttStats, rat_band=int)\n"
"Populates an attribute table with statistics from an input values image where only the pixels with a band value above a defined threshold are used.\n"
"This is something referred to as the mean-lit statistics, i.e., the sunlit pixels within the object.\n"
"\n"
":param in_vals_img: is a string containing the name of the input image file from which the clumps are to populated.\n"
":param clumps_img: is a string containing the name of the input clumps image file\n"
":param mean_lit_img: is a string containing the name of the input image containing the band to be used for the mean-lit stats.\n"
":param mean_lit_band: is an unsigned integer specifying the image band to be used within the meanLitImage.\n"
":param mean_lit_col: is a string specifying the column to be used for the 'mean' for each object in the mean-lit calculation\n"
":param pxl_count_col: is a string specifying the output column in the RAT where the count for the number of pixels within each clump used for the stats is outputted.\n"
":param band_stats: is a sequence of rsgislib.rastergis.BandAttStats objects that have attributes in line with rsgis.cmds.RSGISBandAttStatsCmds\n"
"                    * band: int defining the image band to process\n"
"                    * min_field: string defining the name of the field for min value\n"
"                    * max_field: string defining the name of the field for max value\n"
"                    * sum_field: string defining the name of the field for sum value\n"
"                    * mean_field: string defining the name of the field for mean value\n"
"                    * std_dev_field: string defining the name of the field for standard deviation value\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"   from rsgislib import rastergis\n"
"   inputImage = \"RapidEye_20130625_lat53lon389_tid3063312_oid167771_rad_toa.kea\"\n"
"   segmentClumps = \"RapidEye_20130625_lat53lon389_tid3063312_oid167771_rad_toa_segs.kea\"\n"
"   ndviImage = \"RapidEye_20130625_lat53lon389_tid3063312_oid167771_rad_toa_ndvi.kea\"\n"
"   bandStats = []\n"
"   bandStats.append(rastergis.BandAttStats(band=1, mean_field='BlueMeanML', std_dev_field='BlueStdDevML'))\n"
"   bandStats.append(rastergis.BandAttStats(band=2, mean_field='GreenMeanML', std_dev_field='GreenStdDevML'))\n"
"   bandStats.append(rastergis.BandAttStats(band=3, mean_field='RedMeanML', std_dev_field='RedStdDevML'))\n"
"   bandStats.append(rastergis.BandAttStats(band=4, mean_field='RedEdgeMeanML', std_dev_field='RedEdgeStdDevML'))\n"
"   bandStats.append(rastergis.BandAttStats(band=5, mean_field='NIRMeanML', std_dev_field='NIRStdDevML'))\n"
"   rastergis.populate_rat_with_meanlit_stats(valsimage=inputImage, clumps=segmentClumps, meanLitImage=ndviImage, meanlitBand=1, meanLitCol='NDVIMean', pxlCountCol='MLPxlCount', bandstats=bandStats, rat_band=1)\n"
"\n"},

{"collapse_rat", (PyCFunction)RasterGIS_CollapseRAT, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.collapse_rat(clumps_img, select_col, output_img, gdalformat, rat_band)\n"
"Collapses the image and rat to a set of selected rows (defined with a value of 1 in the selected column).\n"
"\n"
":param clumps_img: is a string containing the name of the input clump file\n"
":param select_col: is a string containing the name of the binary column used to selected the rows to which the RAT is to be collapsed to.\n"
":param output_img: is a string with the output file name\n"
":param gdalformat: is a string with the output image file format - note only KEA and HFA support RATs.\n"
":param rat_band: is the image band with which the RAT is associated.\n"
"\n"},


{"import_vec_atts", (PyCFunction)RasterGIS_ImportVecAtts, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.import_vec_atts(clumps_img, vec_file, vec_lyr, fid_col, col_names, rat_band=1)\n"
"Copies the attributes from an input shapefile to the RAT.\n"
"\n"
":param clumps_img: is a string containing the name of the input file with RAT\n"
":param vec_file: is a string containing the file path of the input vector file\n"
":param vec_lyr: is a string containing the layer name within the input vector file\n"
":param fid_col: is a string with the name of a column which has the clumps pixel value associated with the vector feature.\n"
":param col_names: is a list of strings specifying the columns to be copied to the RAT. If 'None' then all attributes will be copied.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"   from rsgislib import rastergis\n"
"   clumps = 'clumpsFiles.kea'\n"
"   vectorFile = 'vectorFile.shp'\n"
"   veclyr = 'vectorFile'\n"
"   rastergis.import_vec_atts(clumps, vectorFile, veclyr, 'pxlval', None)\n"
"\n"},

{"apply_rat_knn", (PyCFunction)RasterGIS_ApplyKNN, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.apply_rat_knn(clumps_img=string, in_extrap_col=string, out_extrap_col=string, train_regions_col=string, apply_regions_col=string, val_cols=list<string>, k_feat=uint, dist_knn=int, summerise_knn=int, dist_thres=float, rat_band=int)\n"
"This function uses the KNN algorithm to allow data values to be extrapolated to segments.\n"
"\n"
":param clumps_img: is a string containing the name of the input clumps image file\n"
":param in_extrap_col: is a string containing the name of the field with the values used for the extrapolation.\n"
":param out_extrap_col: is a string containing the name of the field where the extrapolated values will be written to.\n"
":param train_regions_col: is a string containing the name of the field specifying the clumps to be used as training - binary column (1 == training region).\n"
":param apply_regions_col: is a string containing the name of the field specifying the regions for which KNN is to be applued - binary column (1 == regions to be calculated). If None then ignored and applied to all."
":param val_cols: is a list of strings specifying the fields which will be used to calculate distance.\n"
":param k_feat: is an unsigned integer specifying the number of nearest features (i.e., K) to be used (Default: 12) \n"
":param dist_knn: specifies how the distance to identify NN is calculated (rsgislib.DIST_EUCLIDEAN, rsgislib.DIST_MANHATTEN, rsgislib.DIST_MAHALANOBIS, rsgislib.DIST_MINKOWSKI, rsgislib.DIST_CHEBYSHEV; Default: rsgislib.DIST_MAHALANOBIS).\n"
":param summerise_knn: specifies how the extrapolation value is calculated (rsgislib.SUMTYPE_MODE, rsgislib.SUMTYPE_MEAN, rsgislib.SUMTYPE_MEDIAN, rsgislib.SUMTYPE_MIN, rsgislib.SUMTYPE_MAX, rsgislib.SUMTYPE_STDDEV; Default: rsgislib.SUMTYPE_MEDIAN). Mode is used for classification.\n"
":param dist_thres: is a maximum distance threshold over which features will not be included within the \'k\'.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"    from rsgislib import rastergis\n"
"    from rsgislib import imageutils\n"
"    import rsgislib\n"
"\n"
"    forestClumpsImg='./LS5TM_20110428_forestclumps.kea'\n"
"\n"
"    rastergis.apply_rat_knn(clumps=forestClumpsImg, inExtrapField='HP95', outExtrapField='HP95Pred', trainRegionsField='LiDARForest', applyRegionsField=None, fields=['RedRefl','GreenRefl','BlueRefl'], kFeat=12, distKNN=rsgislib.DIST_EUCLIDEAN, summeriseKNN=rsgislib.SUMTYPE_MEDIAN, distThres=25)\n"
"\n"
"    # Export predicted column to GDAL image\n"
"    forestHeightImg='./LS5TM_20110428_forest95Height.kea'\n"
"    rastergis.export_col_to_gdal_img(forestClumpsImg, forestHeightImg, 'KEA', rsgislib.TYPE_32FLOAT, 'HP95Pred')\n"
"    imageutils.popImageStats(forestHeightImg,True,0.,True)\n"
"\n"},
    
{"histo_sampling", (PyCFunction)RasterGIS_HistoSampling, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.histo_sampling(clumps_img=string, val_col=string, out_sel_col=string, prop_sample=float, bin_width=float, cls_col=string, class_val=string, rat_band=int)\n"
"This function performs a histogram based sampling of the RAT for a specific column.\n"
"The output is a binary column within the RAT where rows with a value of 1 are the selected clumps.\n"
"\n"
":param clumps_img: is a string containing the name of the input clumps image file\n"
":param val_col: is a string containing the name of the field with the values used for the sampling.\n"
":param out_sel_col: is a string containing the name of the field where the binary output will be written (1 for selected clumps).\n"
":param prop_sample: is a float specifying the proportion of the datasets which should be within the outputted sample. Values range of 0-1. 0.5 would be a 50% sample."
":param bin_width: is a float specifying the width of each histogram bin.\n"
":param cls_col: is a string specifying a field within which classes have been defined. This can be used to only apply the sampling to a thematic subset of the RAT. If set as None then this is ignored. (Default = None)\n"
":param class_val: is a string specifying the class it will be limited to.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"    from rsgislib import rastergis\n"
"    \n"
"    rastergis.histo_sampling(clumps='N00E103_10_grid_knn.kea', varCol='HH', outSelectCol='HHSampling', propOfSample=0.25, binWidth=0.01, classColumn='Class', classVal='2')\n"
"\n"},
    
{"fit_hist_gausian_mixture_model", (PyCFunction)RasterGIS_FitHistGausianMixtureModel, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.fit_hist_gausian_mixture_model(clumps_img=string, out_h5_file=string, out_hist_file=string, val_col=string, bin_width=float, cls_col=string, cls_val=string, rat_band=int)\n"
"This function fits a Gaussian mixture model to the histogram for a variable in the RAT. \n"
"\n"
":param clumps_img: is a string containing the name of the input clumps image file\n"
":param out_h5_file: is a string for a HDF5 with the fitted Gaussians.\n"
":param out_hist_file: is a string to output the Histrogram as a HDF5 file.\n"
":param val_col: is a string containing the name of the field with the values used for the sampling.\n"
":param bin_width: is a float specifying the width of each histogram bin.\n"
":param cls_col: is a string specifying a field within which classes have been defined.\n"
":param cls_val: is a string specifying the class it will be limited to.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"    from rsgislib import rastergis\n"
"\n"
"    rastergis.fit_hist_gausian_mixture_model(clumps='FrenchGuiana_10_ALL_sl_HH_lee_UTM_mosaic_dB_segs.kea', outH5File='gaufit.h5', outHistFile='histfile.h5', varCol='HVdB', binWidth=0.1, classColumn='Classes', classVal='Mangrove')\n"
"\n"},

{"class_split_fit_hist_gausian_mixture_model", (PyCFunction)RasterGIS_ClassSplitFitHistGausianMixtureModel, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.class_split_fit_hist_gausian_mixture_model(clumps_img=string, out_col=string, val_col=string, bin_width=float, cls_col=string, cls_val=string, rat_band=int)\n"
"This function fits a Gaussian mixture model to the histogram for a variable in the RAT and uses it to split the class into a series of subclasses.\n"
"\n"
":param clumps_img: is a string containing the name of the input clumps image file\n"
":param out_col: is a string for a HDF5 with the fitted Gaussians.\n"
":param val_col: is a string containing the name of the field with the values used for the sampling.\n"
":param bin_width: is a float specifying the width of each histogram bin.\n"
":param cls_col: is a string specifying a field within which classes have been defined.\n"
":param cls_val: is a string specifying the class it will be limited to.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"    from rsgislib import rastergis\n"
"\n"
"    rastergis.class_split_fit_hist_gausian_mixture_model(clumps='FrenchGuiana_10_ALL_sl_HH_lee_UTM_mosaic_dB_segs.kea', outCol='MangroveSubClass', varCol='HVdB', binWidth=0.1, classColumn='Classes', classVal='Mangroves')\n"
"\n"},
    
{"populate_rat_with_prop_valid_pxls", (PyCFunction)RasterGIS_PopulateRATWithPropValidPxls, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.populate_rat_with_prop_valid_pxls(input_img=string, clumps_img=string, out_col=string, no_data_val=float, rat_band=uint)\n"
"Populates the attribute table with the proportion of valid pixels within the clump.\n"
"\n"
":param input_img: is a string containing the name of the input image file from which the valid pixels are to be identified\n"
":param clumps_img: is a string containing the name of the input clump file to which the proportion will be populated.\n"
":param out_col: is a string representing the name for the output column containing the proportion.\n"
":param no_data_val: is a float defining the no data value to be used.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated in the clumps image.\n"
"\n"},
    
{"calc_1d_jm_distance", (PyCFunction)RasterGIS_Calc1DJMDistance, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.calc_1d_jm_distance(clumps_img=string, val_col=string, bin_width=float, cls_col=string, class1=string, class2=string, rat_band=uint)\n"
"Calculate the Jeffries and Matusita distance for a single variable between two classes.\n"
"\n"
":param clumps_img: is a string containing the name of the input clump file\n"
":param val_col: is a string specifying the name of the variable column.\n"
":param bin_width: is a float specifying the bin width for the histogram.\n"
":param cls_col: is a string specifying the column name with the class names.\n"
":param class1: is a string specifying the first class.\n"
":param class2: is a string specifying the second class.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated in the clumps image.\n"
"\n"
":return: double for distance\n"
"\n"},
    
{"calc_2d_jm_distance", (PyCFunction)RasterGIS_Calc2DJMDistance, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.calc_2d_jm_distance(clumps_img=string, val1_col=string, val2_col=string, val1_bin_width=float, val2_bin_width=float, cls_col=string, class1=string, class2=string, rat_band=uint)\n"
"Calculate the Jeffries and Matusita distance for two variables between two classes.\n"
"\n"
":param clumps_img: is a string containing the name of the input clump file\n"
":param val1_col: is a string specifying the name of the first variable column.\n"
":param val2_col: is a string specifying the name of the second variable column.\n"
":param val1_bin_width: is a float specifying the bin width for the histogram for variable 1.\n"
":param val2_bin_width: is a float specifying the bin width for the histogram for variable 2.\n"
":param cls_col: is a string specifying the column name with the class names.\n"
":param class1: is a string specifying the first class.\n"
":param class2: is a string specifying the second class.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated in the clumps image.\n"
"\n"
":return: double for distance\n"
"\n"},
    
{"calc_bhattacharyya_distance", (PyCFunction)RasterGIS_CalcBhattacharyyaDistance, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.calc_bhattacharyya_distance(clumps_img=string, val_col=string, cls_col=string, class1=string, class2=string, rat_band=uint)\n"
"Calculate the Bhattacharyya distance for a single variable between two classes.\n"
"\n"
":param clumps_img: is a string containing the name of the input clump file\n"
":param val_col: is a string specifying the name of the variable column.\n"
":param cls_col: is a string specifying the column name with the class names.\n"
":param class1: is a string specifying the first class.\n"
":param class2: is a string specifying the second class.\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated in the clumps image.\n"
"\n"
":return: double for distance\n"
"\n"},

{"export_clumps_to_images", (PyCFunction)RasterGIS_ExportClumps2Images, METH_VARARGS | METH_KEYWORDS,
"rsgislib.rastergis.export_clumps_to_images(clumps_img, out_img_base, bin_out, out_img_ext, gdalformat, rat_band=1)\n"
"Exports each clump to a seperate raster which is the minimum extent for the clump.\n"
"\n"
":param clumps_img: is a string containing the name of the input image file with RAT\n"
":param out_img_base: is a string containing the base name of the output image file (C + FID will be added to identify files).\n"
":param bin_out: is a boolean specifying whether the output images should be binary or if the pixel value should be the FID of the clump.\n"
":param out_img_ext: is a sting with the output file extension (e.g., kea) without the preceeding dot to be appended to the file name.\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param rat_band: is an optional (default = 1) integer parameter specifying the image band to which the RAT is associated.\n"
"\n"
".. code:: python\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import rastergis\n"
"   clumps='./DefineTiles.kea'\n"
"   outimgbase='./Tiles/OutputImgTile_'\n"
"   outimgext='kea'\n"
"   gdalformat = 'KEA'\n"
"   binaryOut = False\n"
"   rastergis.export_clumps_to_images(clumps, outimgbase, binaryOut, outimgext, gdalformat, rat_band)\n"
"\n"},
    
    {nullptr}        /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3

static int RasterGIS_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int RasterGIS_clear(PyObject *m)
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_rastergis",
        nullptr,
        sizeof(struct RasterGisState),
        RasterGISMethods,
        nullptr,
        RasterGIS_traverse,
        RasterGIS_clear,
        nullptr
};

#define INITERROR return nullptr

PyMODINIT_FUNC
PyInit__rastergis(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_rastergis(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_rastergis", RasterGISMethods);
#endif
    if( pModule == nullptr )
        INITERROR;

    struct RasterGisState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_rastergis.error", nullptr, nullptr);
    if( state->error == nullptr )
    {
        Py_DECREF(pModule);
        INITERROR;
    }


#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
