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
static std::vector<std::string> ExtractVectorStringFromSequence(PyObject *sequence) {
    Py_ssize_t nFields = PySequence_Size(sequence);
    std::vector<std::string> fields;
    fields.reserve(nFields);

    for(int i = 0; i < nFields; ++i) {
        PyObject *fieldObj = PySequence_GetItem(sequence, i);

        if(!RSGISPY_CHECK_STRING(fieldObj)) {
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

static PyObject *RasterGIS_PopulateStats(PyObject *self, PyObject *args) {
    const char *clumpsImage;
    int addColourTable2Img, calcImgPyramids;

    if(!PyArg_ParseTuple(args, "sii:populateStats", &clumpsImage, &addColourTable2Img, &calcImgPyramids))
        return NULL;

    try {
        rsgis::cmds::executePopulateStats(std::string(clumpsImage), addColourTable2Img, calcImgPyramids);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_CopyRAT(PyObject *self, PyObject *args) {
    const char *clumpsImage, *inputImage;

    if(!PyArg_ParseTuple(args, "ss:copyRAT", &inputImage, &clumpsImage))
        return NULL;

    try {
        rsgis::cmds::executeCopyRAT(std::string(clumpsImage), std::string(inputImage));
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_CopyGDALATTColumns(PyObject *self, PyObject *args) {
    const char *clumpsImage, *inputImage;
    PyObject *pFields;

    if(!PyArg_ParseTuple(args, "ssO:copyGDALATTColumns", &inputImage, &clumpsImage, &pFields))
        return NULL;

    if(!PySequence_Check(pFields)) {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return NULL;
    }

    std::vector<std::string> fields = ExtractVectorStringFromSequence(pFields);
    if(fields.size() == 0) { return NULL; }


    try {
        rsgis::cmds::executeCopyGDALATTColumns(std::string(inputImage), std::string(clumpsImage), fields);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_SpatialLocation(PyObject *self, PyObject *args) {
    const char *inputImage, *eastingsField, *northingsField;

    if(!PyArg_ParseTuple(args, "sss:spatialLocation", &inputImage, &eastingsField, &northingsField))
        return NULL;

    try {
        rsgis::cmds::executeSpatialLocation(std::string(inputImage), std::string(eastingsField), std::string(northingsField));
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_PopulateRATWithStats(PyObject *self, PyObject *args) {
    const char *inputImage, *clumpsImage;
    PyObject *pBandAttStatsCmds;

    if(!PyArg_ParseTuple(args, "ssO:populateRATWithStats", &inputImage, &clumpsImage, &pBandAttStatsCmds))
        return NULL;

     if(!PySequence_Check(pBandAttStatsCmds)) {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return NULL;
    }

    // extract the attributes from the sequence of objects into our structs
    Py_ssize_t nCmds = PySequence_Size(pBandAttStatsCmds);
    std::vector<rsgis::cmds::RSGISBandAttStatsCmds*> bandStatsCmds;
    bandStatsCmds.reserve(nCmds);

    for(int i = 0; i < nCmds; ++i) {
        PyObject *o = PySequence_GetItem(pBandAttStatsCmds, i);     // the python object

        rsgis::cmds::RSGISBandAttStatsCmds *cmdObj = new rsgis::cmds::RSGISBandAttStatsCmds();   // the c++ object we need to pass pointers of

        // declare and initialise pointers for all the attributes of the struct
        PyObject *pBand, *pThreshold, *pCountField, *pMinField, *pMaxField, *pStdDevField, *pMedianField, *pSumField, *pMeanField;
        pBand = pThreshold = pCountField = pMinField = pMaxField = pMeanField = pStdDevField = pMedianField = pSumField = NULL;

        std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference
        extractedAttributes.push_back(o);
        
        pBand = PyObject_GetAttrString(o, "band");
        extractedAttributes.push_back(pBand);
        if( ( pBand == NULL ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'band\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pThreshold = PyObject_GetAttrString(o, "threshold");
        extractedAttributes.push_back(pThreshold);
        if( ( pThreshold == NULL ) || ( pThreshold == Py_None ) || !RSGISPY_CHECK_FLOAT(pThreshold) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find float attribute \'threshold\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }
        
        // check if fields have been set, set calcValue accordingly
        pCountField = PyObject_GetAttrString(o, "countField");
        extractedAttributes.push_back(pCountField);
        cmdObj->calcCount = !(pCountField == NULL || !RSGISPY_CHECK_STRING(pCountField));

        pMinField = PyObject_GetAttrString(o, "minField");
        extractedAttributes.push_back(pMinField);
        cmdObj->calcMin =  !(pMinField == NULL || !RSGISPY_CHECK_STRING(pMinField));

        pMaxField = PyObject_GetAttrString(o, "maxField");
        extractedAttributes.push_back(pMaxField);
        cmdObj->calcMax = !(pMaxField == NULL || !RSGISPY_CHECK_STRING(pMaxField));

        pMeanField = PyObject_GetAttrString(o, "meanField");
        extractedAttributes.push_back(pMeanField);
        cmdObj->calcMean = !(pMeanField == NULL || !RSGISPY_CHECK_STRING(pMeanField));

        pStdDevField = PyObject_GetAttrString(o, "stdDevField");
        extractedAttributes.push_back(pStdDevField);
        cmdObj->calcStdDev = !(pStdDevField == NULL || !RSGISPY_CHECK_STRING(pStdDevField));

        pMedianField = PyObject_GetAttrString(o, "medianField");
        extractedAttributes.push_back(pMedianField);
        cmdObj->calcMedian = !(pMedianField == NULL || !RSGISPY_CHECK_STRING(pMedianField));

        pSumField = PyObject_GetAttrString(o, "sumField");
        extractedAttributes.push_back(pSumField);
        cmdObj->calcSum = !(pSumField == NULL || !RSGISPY_CHECK_STRING(pSumField));

        // extract the values from the objects
        cmdObj->band = RSGISPY_INT_EXTRACT(pBand);
        cmdObj->threshold = RSGISPY_FLOAT_EXTRACT(pThreshold);
        // check the calcValue and extract fields if required
        if(cmdObj->calcCount) {
            cmdObj->countField = RSGISPY_STRING_EXTRACT(pCountField);
        }
        if(cmdObj->calcMax) {
            cmdObj->maxField = RSGISPY_STRING_EXTRACT(pMaxField);
        }
        if(cmdObj->calcMean) {
            cmdObj->meanField = RSGISPY_STRING_EXTRACT(pMeanField);
        }
        if(cmdObj->calcMedian) {
            cmdObj->medianField = RSGISPY_STRING_EXTRACT(pMedianField);
        }
        if(cmdObj->calcMin) {
            cmdObj->minField = RSGISPY_STRING_EXTRACT(pMinField);
        }
        if(cmdObj->calcStdDev) {
            cmdObj->stdDevField = RSGISPY_STRING_EXTRACT(pStdDevField);
        }
        if(cmdObj->calcSum) {
            cmdObj->sumField = RSGISPY_STRING_EXTRACT(pSumField);
        }
        
        FreePythonObjects(extractedAttributes);
        bandStatsCmds.push_back(cmdObj);
    }
    try {
        rsgis::cmds::executePopulateRATWithStats(std::string(inputImage), std::string(clumpsImage), &bandStatsCmds);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
            delete *iter;
        }
        return NULL;
    }

    // free temp structs
    for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
        delete *iter;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_PopulateRATWithPercentiles(PyObject *self, PyObject *args) {
    const char *inputImage, *clumpsImage;
    PyObject *pBandPercentilesCmds;

    if(!PyArg_ParseTuple(args, "ssO:populateRATWithPercentiles", &inputImage, &clumpsImage, &pBandPercentilesCmds))
        return NULL;

    if(!PySequence_Check(pBandPercentilesCmds)) {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return NULL;
    }

    // extract the attributes from the sequence of objects into our structs
    Py_ssize_t nPercentiles = PySequence_Size(pBandPercentilesCmds);
    std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*> bandPercentilesCmds;
    bandPercentilesCmds.reserve(nPercentiles);

    for(int i = 0; i < nPercentiles; ++i) {
        PyObject *o = PySequence_GetItem(pBandPercentilesCmds, i);     // the python object

        rsgis::cmds::RSGISBandAttPercentilesCmds *percObj = new rsgis::cmds::RSGISBandAttPercentilesCmds;   // the c++ object we need to pass pointers of

        // declare and initialise pointers for all the attributes of the struct
        PyObject *pBand, *pPercentile, *pFieldName;
        pBand = pPercentile = pFieldName = NULL;

        std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference
        extractedAttributes.push_back(o);

        pBand = PyObject_GetAttrString(o, "band");
        extractedAttributes.push_back(pBand);
        if( ( pBand == NULL ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'band\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*>::iterator iter = bandPercentilesCmds.begin(); iter != bandPercentilesCmds.end(); ++iter) {
                delete *iter;
            }
            delete percObj;
            return NULL;
        }

        pPercentile = PyObject_GetAttrString(o, "percentile");
        extractedAttributes.push_back(pPercentile);
        if( ( pPercentile == NULL ) || ( pPercentile == Py_None ) || !RSGISPY_CHECK_INT(pPercentile)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'percentile\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*>::iterator iter = bandPercentilesCmds.begin(); iter != bandPercentilesCmds.end(); ++iter) {
                delete *iter;
            }
            delete percObj;
            return NULL;
        }

        pFieldName = PyObject_GetAttrString(o, "fieldName");
        extractedAttributes.push_back(pFieldName);
        if( ( pFieldName == NULL ) || ( pFieldName == Py_None ) || !RSGISPY_CHECK_STRING(pFieldName)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'fieldName\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*>::iterator iter = bandPercentilesCmds.begin(); iter != bandPercentilesCmds.end(); ++iter) {
                delete *iter;
            }
            delete percObj;
            return NULL;
        }

        percObj->band = RSGISPY_INT_EXTRACT(pBand);
        percObj->percentile = RSGISPY_INT_EXTRACT(pPercentile);
        percObj->fieldName = RSGISPY_STRING_EXTRACT(pFieldName);

        FreePythonObjects(extractedAttributes);
        bandPercentilesCmds.push_back(percObj);
    }

    try {
        rsgis::cmds::executePopulateRATWithPercentiles(std::string(inputImage), std::string(clumpsImage), &bandPercentilesCmds);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        for(std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*>::iterator iter = bandPercentilesCmds.begin(); iter != bandPercentilesCmds.end(); ++iter) {
            delete *iter;
        }
        return NULL;
    }

    // free temp structs
    for(std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*>::iterator iter = bandPercentilesCmds.begin(); iter != bandPercentilesCmds.end(); ++iter) {
        delete *iter;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_PopulateCategoryProportions(PyObject *self, PyObject *args) {
    const char *clumpsImage, *categoriesImage, *outColsName, *majorityColName, *majClassNameField, *classNameField;
    int copyClassNames;

    if(!PyArg_ParseTuple(args, "ssssiss:populateCategoryProportions", &categoriesImage, &clumpsImage, &outColsName, &majorityColName, &copyClassNames, &majClassNameField, &classNameField))
        return NULL;

    try {
        rsgis::cmds::executePopulateCategoryProportions(std::string(categoriesImage), std::string(clumpsImage), std::string(outColsName), std::string(majorityColName),
                                                        (copyClassNames != 0), std::string(majClassNameField), std::string(classNameField));
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_CopyCategoriesColours(PyObject *self, PyObject *args) {
    const char *clumpsImage, *categoriesImage, *classField;

    if(!PyArg_ParseTuple(args, "sss:copyCategoriesColours", &categoriesImage, &clumpsImage, &classField))
        return NULL;

    try {
        rsgis::cmds::executeCopyCategoriesColours(std::string(categoriesImage), std::string(clumpsImage), std::string(classField));
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_ExportCols2GDALImage(PyObject *self, PyObject *args) {
    const char *inputImage, *outputFile, *imageFormat;
    int dataType;
    PyObject *pFields;

    if(!PyArg_ParseTuple(args, "sssiO:copyCategoriesColours", &inputImage, &outputFile, &imageFormat, &dataType, &pFields))
        return NULL;

    if(!PySequence_Check(pFields)) {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return NULL;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)dataType;

    std::vector<std::string> fields = ExtractVectorStringFromSequence(pFields);
    if(fields.size() == 0) { return NULL; }

    try {
        rsgis::cmds::executeExportCols2GDALImage(std::string(inputImage), std::string(outputFile), std::string(imageFormat), type, fields);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_EucDistFromFeature(PyObject *self, PyObject *args) {
    const char *inputImage, *outputField;
    int fid;
    PyObject *pFields;

    if(!PyArg_ParseTuple(args, "sisO:eucDistFromFeature", &inputImage, &fid, &outputField, &pFields))
        return NULL;

    if(!PySequence_Check(pFields)) {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return NULL;
    }

    std::vector<std::string> fields = ExtractVectorStringFromSequence(pFields);
    if(fields.size() == 0) { return NULL; }

    try {
        rsgis::cmds::executeEucDistFromFeature(std::string(inputImage), (size_t)fid, std::string(outputField), fields);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_FindTopN(PyObject *self, PyObject *args) {
    const char *inputImage, *outputField, *spatialDistField, *distanceField;
    unsigned int nFeatures;
    float distThreshold;

    if(!PyArg_ParseTuple(args, "ssssIf:findTopN", &inputImage, &spatialDistField, &distanceField, &outputField, &nFeatures, &distThreshold))
        return NULL;

    try {
        rsgis::cmds::executeFindTopN(std::string(inputImage), std::string(spatialDistField), std::string(distanceField), std::string(outputField), nFeatures, distThreshold);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_FindSpecClose(PyObject *self, PyObject *args) {
    const char *inputImage, *outputField, *spatialDistField, *distanceField;
    float distThreshold, specDistThreshold;

    if(!PyArg_ParseTuple(args, "ssssff:findSpecClose", &inputImage, &spatialDistField, &distanceField, &outputField, &specDistThreshold, &distThreshold))
        return NULL;

    try {
        rsgis::cmds::executeFindSpecClose(std::string(inputImage), std::string(distanceField), std::string(spatialDistField), std::string(outputField), specDistThreshold, distThreshold);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_Export2Ascii(PyObject *self, PyObject *args) {
    const char *inputImage, *outputFile;
    PyObject *pFields;

    if(!PyArg_ParseTuple(args, "ssO:export2Ascii", &inputImage, &outputFile, &pFields))
        return NULL;

    if(!PySequence_Check(pFields)) {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return NULL;
    }

    std::vector<std::string> fields = ExtractVectorStringFromSequence(pFields);
    if(fields.size() == 0) { return NULL; }

    try {
        rsgis::cmds::executeExport2Ascii(std::string(inputImage), std::string(outputFile), fields);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_ClassTranslate(PyObject *self, PyObject *args) {
    const char *inputImage, *classInField, *classOutField;
    PyObject *pClassPairs;

    if(!PyArg_ParseTuple(args, "sssO:classTranslate", &inputImage, &classInField, &classOutField, &pClassPairs))
        return NULL;

    if(!PyDict_Check(pClassPairs)) {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a dict");
        return NULL;
    }

    std::map<size_t, size_t> classPairs;
    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(pClassPairs, &pos, &key, &value)) {
        if(!RSGISPY_CHECK_INT(key) || !RSGISPY_CHECK_INT(value)) {
            PyErr_SetString(GETSTATE(self)->error, "dict key and values must be ints");
            return NULL;
        }
        classPairs[(size_t)RSGISPY_INT_EXTRACT(key)] = (size_t)RSGISPY_INT_EXTRACT(value);
    }

    try {
        rsgis::cmds::executeClassTranslate(std::string(inputImage), std::string(classInField), std::string(classOutField), classPairs);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_ColourClasses(PyObject *self, PyObject *args) {
    const char *inputImage, *classInField;
    PyObject *pClassColourPairs;

    if(!PyArg_ParseTuple(args, "ssO:colourClasses", &inputImage, &classInField, &pClassColourPairs))
        return NULL;

    if(!PyDict_Check(pClassColourPairs)) {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a dict");
        return NULL;
    }

    std::map<size_t, rsgis::cmds::RSGISColourIntCmds> classPairs;
    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(pClassColourPairs, &pos, &key, &value)) {
        if(!RSGISPY_CHECK_INT(key)) {
            PyErr_SetString(GETSTATE(self)->error, "dict keys must be ints");
            return NULL;
        }

        PyObject *pRed, *pGreen, *pBlue, *pAlpha;
        pRed = pGreen = pBlue = pAlpha = NULL;

        std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference

        pRed = PyObject_GetAttrString(value, "red");
        extractedAttributes.push_back(pRed);
        if( ( pRed == NULL ) || ( pRed == Py_None ) || !RSGISPY_CHECK_INT(pRed)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'red\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }

        pGreen = PyObject_GetAttrString(value, "green");
        extractedAttributes.push_back(pGreen);
        if( ( pGreen == NULL ) || ( pGreen == Py_None ) || !RSGISPY_CHECK_INT(pGreen)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'green\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }

        pBlue = PyObject_GetAttrString(value, "blue");
        extractedAttributes.push_back(pBlue);
        if( ( pBlue == NULL ) || ( pBlue == Py_None ) || !RSGISPY_CHECK_INT(pBlue)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'blue\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }

        pAlpha = PyObject_GetAttrString(value, "alpha");
        extractedAttributes.push_back(pAlpha);
        if( ( pAlpha == NULL ) || ( pAlpha == Py_None ) || !RSGISPY_CHECK_INT(pAlpha)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'alpha\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }

        rsgis::cmds::RSGISColourIntCmds colour(RSGISPY_INT_EXTRACT(pRed), RSGISPY_INT_EXTRACT(pGreen), RSGISPY_INT_EXTRACT(pBlue), RSGISPY_INT_EXTRACT(pAlpha));
        classPairs[(size_t)RSGISPY_INT_EXTRACT(key)] = colour;
        FreePythonObjects(extractedAttributes);
    }

    try {
        rsgis::cmds::executeColourClasses(std::string(inputImage), std::string(classInField), classPairs);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_ColourStrClasses(PyObject *self, PyObject *args) {
    const char *inputImage, *classInField;
    PyObject *pClassColourPairs;

    if(!PyArg_ParseTuple(args, "ssO:colourClasses", &inputImage, &classInField, &pClassColourPairs))
        return NULL;

    if(!PyDict_Check(pClassColourPairs)) {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a dict");
        return NULL;
    }

    std::map<std::string, rsgis::cmds::RSGISColourIntCmds> classPairs;
    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(pClassColourPairs, &pos, &key, &value)) {
        if(!RSGISPY_CHECK_STRING(key)) {
            PyErr_SetString(GETSTATE(self)->error, "dict keys must be strings");
            return NULL;
        }

        PyObject *pRed, *pGreen, *pBlue, *pAlpha;
        pRed = pGreen = pBlue = pAlpha = NULL;

        std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference

        pRed = PyObject_GetAttrString(value, "red");
        extractedAttributes.push_back(pRed);
        if( ( pRed == NULL ) || ( pRed == Py_None ) || !RSGISPY_CHECK_INT(pRed)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'red\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }

        pGreen = PyObject_GetAttrString(value, "green");
        extractedAttributes.push_back(pGreen);
        if( ( pGreen == NULL ) || ( pGreen == Py_None ) || !RSGISPY_CHECK_INT(pGreen)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'green\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }

        pBlue = PyObject_GetAttrString(value, "blue");
        extractedAttributes.push_back(pBlue);
        if( ( pBlue == NULL ) || ( pBlue == Py_None ) || !RSGISPY_CHECK_INT(pBlue)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'blue\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }

        pAlpha = PyObject_GetAttrString(value, "alpha");
        extractedAttributes.push_back(pAlpha);
        if( ( pAlpha == NULL ) || ( pAlpha == Py_None ) || !RSGISPY_CHECK_INT(pAlpha)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'alpha\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }

        rsgis::cmds::RSGISColourIntCmds colour(RSGISPY_INT_EXTRACT(pRed), RSGISPY_INT_EXTRACT(pGreen), RSGISPY_INT_EXTRACT(pBlue), RSGISPY_INT_EXTRACT(pAlpha));
        classPairs[RSGISPY_STRING_EXTRACT(key)] = colour;
        FreePythonObjects(extractedAttributes);
    }

    try {
        rsgis::cmds::executeColourStrClasses(std::string(inputImage), std::string(classInField), classPairs);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_GenerateColourTable(PyObject *self, PyObject *args) {
    const char *inputImage, *clumpsImage;
    unsigned int redBand, greenBand, blueBand;

    if(!PyArg_ParseTuple(args, "ssIII:generateColourTable", &inputImage, &clumpsImage, &redBand, &greenBand, &blueBand))
        return NULL;


    try {
        rsgis::cmds::executeGenerateColourTable(std::string(inputImage), std::string(clumpsImage), redBand, greenBand, blueBand);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_StrClassMajority(PyObject *self, PyObject *args) {
    const char *baseSegment, *infoSegment, *bassClassCol, *infoClassCol;

    if(!PyArg_ParseTuple(args, "ssss:strClassMajority", &baseSegment, &infoSegment, &bassClassCol, &infoClassCol))
        return NULL;

    try {
        rsgis::cmds::executeStrClassMajority(std::string(baseSegment), std::string(infoSegment), std::string(bassClassCol), std::string(infoClassCol));
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_SpecDistMajorityClassifier(PyObject *self, PyObject *args) {
    const char *inputImage, *inClassNameField, *outClassNameField, *trainingSelectCol, *eastingsField, *northingsField, *areaField, *majWeightField;
    int distMethod;
    float distThreshold, specDistThreshold, specThreshOriginDist;
    PyObject *pFields;

    if(!PyArg_ParseTuple(args, "ssssssssOffif:specDistMajorityClassifier", &inputImage, &inClassNameField, &outClassNameField, &trainingSelectCol,
                         &eastingsField, &northingsField, &areaField, &majWeightField, &pFields, &distThreshold, &specDistThreshold, &distMethod, &specThreshOriginDist))
    {
        return NULL;
    }
    
    if(!PySequence_Check(pFields)) {
        PyErr_SetString(GETSTATE(self)->error, "9th argument must be a sequence");
        return NULL;
    }

    std::vector<std::string> fields = ExtractVectorStringFromSequence(pFields);
    if(fields.size() == 0) { return NULL; }
    
    rsgis::cmds::SpectralDistanceMethodCmds method = (rsgis::cmds::SpectralDistanceMethodCmds)distMethod;

    try {
        rsgis::cmds::executeSpecDistMajorityClassifier(std::string(inputImage), std::string(inClassNameField), std::string(outClassNameField), std::string(trainingSelectCol), std::string(eastingsField), std::string(northingsField), std::string(areaField), std::string(majWeightField), fields, distThreshold, specDistThreshold, method, specThreshOriginDist);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *RasterGIS_MaxLikelihoodClassifier(PyObject *self, PyObject *args) {
    const char *inputImage, *inClassNameField, *outClassNameField, *trainingSelectCol, *areaField;
    int priorsMethod;
    PyObject *pFields, *pPriorStrs;
    
    if(!PyArg_ParseTuple(args, "sssssOiO:maxLikelihoodClassifier", &inputImage, &inClassNameField, &outClassNameField, &trainingSelectCol, &areaField, &pFields, &priorsMethod, &pPriorStrs)) {
        return NULL;
    }
       
    if(!PySequence_Check(pFields) || !PySequence_Check(pPriorStrs)) {
       PyErr_SetString(GETSTATE(self)->error, "6th and last arguments must be sequences");
       return NULL;
    }
    
    std::vector<std::string> fields, priorStrs;
    fields = ExtractVectorStringFromSequence(pFields);
    priorStrs = ExtractVectorStringFromSequence(pPriorStrs);
    if(fields.size() == 0 || priorStrs.size() == 0) { return NULL; }
    rsgis::cmds::rsgismlpriorscmds method = (rsgis::cmds::rsgismlpriorscmds)priorsMethod;
    
    try {
        rsgis::cmds::executeMaxLikelihoodClassifier(std::string(inputImage), std::string(inClassNameField), std::string(outClassNameField), std::string(trainingSelectCol), std::string(areaField), fields, method, priorStrs);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
    
}

static PyObject *RasterGIS_MaxLikelihoodClassifierLocalPriors(PyObject *self, PyObject *args) {
    const char *inputImage, *inClassNameField, *outClassNameField, *trainingSelectCol, *areaField, *eastingsField, *northingsField;
    float distThreshold, weightA;
    int priorsMethod, iAllowZeroPriors;
    PyObject *pFields;
    
    if(!PyArg_ParseTuple(args, "sssssOssfifi:maxLikelihoodClassifierLocalPriors", &inputImage, &inClassNameField, &outClassNameField, &trainingSelectCol, &areaField, &pFields, &eastingsField, &northingsField, &distThreshold, &priorsMethod, &weightA, &iAllowZeroPriors)) {
        return NULL;
    }
    
    if(!PySequence_Check(pFields)) {
        PyErr_SetString(GETSTATE(self)->error, "6th argument must be a sequence");
        return NULL;
    }
    
    std::vector<std::string> fields;
    fields = ExtractVectorStringFromSequence(pFields);
    if(fields.size() == 0) { return NULL; }
    
    rsgis::cmds::rsgismlpriorscmds method = (rsgis::cmds::rsgismlpriorscmds)priorsMethod;
    bool allowZeroPriors = (iAllowZeroPriors != 0);
    
    try {
        rsgis::cmds::executeMaxLikelihoodClassifierLocalPriors(std::string(inputImage), std::string(inClassNameField), std::string(outClassNameField), std::string(trainingSelectCol), std::string(areaField), fields, std::string(eastingsField), std::string(northingsField), distThreshold, method, weightA, allowZeroPriors);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
    
}

static PyObject *RasterGIS_ClassMask(PyObject *self, PyObject *args) {
    const char *inputImage, *classField, *className, *outputFile, *imageFormat;
    int dataType;
    
    if(!PyArg_ParseTuple(args, "sssssi:classMask", &inputImage, &classField, &className, &outputFile, &imageFormat, &dataType)) {
        return NULL;
    }
    
    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType) dataType;
    
    try {
        rsgis::cmds::executeClassMask(std::string(inputImage), std::string(classField), std::string(className), std::string(outputFile), std::string(imageFormat), type);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_FindNeighbours(PyObject *self, PyObject *args) {
    const char *inputImage;
    
    if(!PyArg_ParseTuple(args, "s:findNeighbours", &inputImage)) {
        return NULL;
    }
    
    try {
        rsgis::cmds::executeFindNeighbours(std::string(inputImage));
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_FindBoundaryPixels(PyObject *self, PyObject *args) {
    const char *inputImage, *outputFile, *imageFormat;
    
    if(!PyArg_ParseTuple(args, "sss:findBoundaryPixels", &inputImage, &outputFile, &imageFormat)) {
        return NULL;
    }
    
    try {
        rsgis::cmds::executeFindBoundaryPixels(std::string(inputImage), std::string(outputFile), std::string(imageFormat));
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_CalcBorderLength(PyObject *self, PyObject *args) {
    const char *inputImage, *outColsName;
    int iIgnoreZeroEdges;
    
    if(!PyArg_ParseTuple(args, "sis:calcBorderLength", &inputImage, &iIgnoreZeroEdges, &outColsName)) {
        return NULL;
    }
    
    try {
        rsgis::cmds::executeCalcBorderLength(std::string(inputImage), (iIgnoreZeroEdges != 0), std::string(outColsName));
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_CalcRelBorder(PyObject *self, PyObject *args) {
    const char *inputImage, *outColsName, *classNameField, *className;
    int iIgnoreZeroEdges;
    
    if(!PyArg_ParseTuple(args, "ssssi:calcRelBorder", &inputImage, &outColsName, &classNameField, &className, &iIgnoreZeroEdges)) {
        return NULL;
    }
    
    try {
        rsgis::cmds::executeCalcRelBorder(std::string(inputImage), std::string(outColsName), std::string(classNameField), std::string(className), (iIgnoreZeroEdges != 0));
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_CalcShapeIndices(PyObject *self, PyObject *args) {
    const char *inputImage;
    PyObject *pShapeIndexes;
    
    if(!PyArg_ParseTuple(args, "sO:calcShapeIndices", &inputImage, &pShapeIndexes)) {
        return NULL;
    }
    
    if(!PySequence_Check(pShapeIndexes)) {
        PyErr_SetString(GETSTATE(self)->error, "6th argument must be a sequence");
        return NULL;
    }
    
    // extract the attributes from the sequence of objects into our structs
    Py_ssize_t nIndexes = PySequence_Size(pShapeIndexes);
    std::vector<rsgis::cmds::RSGISShapeParamCmds> shapeIndexes;
    shapeIndexes.reserve(nIndexes);
    
    for(int i = 0; i < nIndexes; ++i) {
        PyObject *o = PySequence_GetItem(pShapeIndexes, i);     // the python object
        
        rsgis::cmds::RSGISShapeParamCmds shapeIndex;
        
        // declare and initialise pointers for all the attributes of the struct
        PyObject *pIdx, *pColName, *pColIdx;
        pIdx = pColName = pColIdx = NULL;
        
        std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference later
        extractedAttributes.push_back(o);
        
        pIdx = PyObject_GetAttrString(o, "idx");
        extractedAttributes.push_back(pIdx);
        if( ( pIdx == NULL ) || ( pIdx == Py_None ) || !RSGISPY_CHECK_INT(pIdx)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'idx\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }
        
        pColName = PyObject_GetAttrString(o, "colName");
        extractedAttributes.push_back(pColName);
        if( ( pColName == NULL ) || ( pColName == Py_None ) || !RSGISPY_CHECK_STRING(pColName)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'colName\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }
        
        pColIdx = PyObject_GetAttrString(o, "colIdx");
        extractedAttributes.push_back(pColIdx);
        if( ( pColIdx == NULL ) || ( pColIdx == Py_None ) || !RSGISPY_CHECK_INT(pColIdx)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'colIdx\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }
        
        shapeIndex.colIdx = RSGISPY_INT_EXTRACT(pColIdx);
        shapeIndex.colName = RSGISPY_STRING_EXTRACT(pColName);
        shapeIndex.idx = (rsgis::cmds::rsgisshapeindexcmds) RSGISPY_INT_EXTRACT(pIdx);
        
        FreePythonObjects(extractedAttributes);
        shapeIndexes.push_back(shapeIndex);
    }
    
    try {
        rsgis::cmds::executeCalcShapeIndices(std::string(inputImage), shapeIndexes);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_DefineClumpTilePositions(PyObject *self, PyObject *args) {
    const char *clumpsImage, *tileImage, *outColsName;
    unsigned int tileOverlap, tileBoundary, tileBody;
    
    if(!PyArg_ParseTuple(args, "sssIII:defineClumpTilePositions", &clumpsImage, &tileImage, &outColsName, &tileOverlap, &tileBoundary, &tileBody)) {
        return NULL;
    }
    
    try {
        rsgis::cmds::executeDefineClumpTilePositions(std::string(clumpsImage), std::string(tileImage), std::string(outColsName), tileOverlap, tileBoundary, tileBody);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_DefineBorderClumps(PyObject *self, PyObject *args) {
    const char *clumpsImage, *tileImage, *outColsName;
    unsigned int tileOverlap, tileBoundary, tileBody;
    
    if(!PyArg_ParseTuple(args, "sssIII:defineBorderClumps", &clumpsImage, &tileImage, &outColsName, &tileOverlap, &tileBoundary, &tileBody)) {
        return NULL;
    }
    
    try {
        rsgis::cmds::executeDefineBorderClumps(std::string(clumpsImage), std::string(tileImage), std::string(outColsName), tileOverlap, tileBoundary, tileBody);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *RasterGIS_FindChangeClumpsFromStdDev(PyObject *self, PyObject *args) {
    const char *clumpsImage, *classField, *changeField;
    PyObject *pAttFields, *pClassFields;
    
    if(!PyArg_ParseTuple(args, "sssOO:findChangeClumpsFromStdDev", &clumpsImage, &classField, &changeField, &pAttFields, &pClassFields)) {
        return NULL;
    }
    
    if(!PySequence_Check(pAttFields) || !PySequence_Check(pClassFields)) {
        PyErr_SetString(GETSTATE(self)->error, "last 2 arguments must be sequences");
    }
    
    std::vector<std::string> attFields = ExtractVectorStringFromSequence(pAttFields);
    
    Py_ssize_t nFields = PySequence_Size(pClassFields);
    std::vector<rsgis::cmds::RSGISClassChangeFieldsCmds> classFields;
    classFields.reserve(nFields);
    
    for(int i = 0; i < nFields; ++i) {
        PyObject *o = PySequence_GetItem(pClassFields, i);     // the python object
        
        rsgis::cmds::RSGISClassChangeFieldsCmds classField;
        
        // declare and initialise pointers for all the attributes of the struct
        PyObject *name, *outName, *threshold, *means, *stdDevs, *count;
        name = outName = threshold = means = stdDevs = count = NULL;
        
        std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference later
        extractedAttributes.push_back(o);
        
        name = PyObject_GetAttrString(o, "name");
        extractedAttributes.push_back(name);
        if( ( name == NULL ) || ( name == Py_None ) || !RSGISPY_CHECK_STRING(name)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'name\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }
        
        outName = PyObject_GetAttrString(o, "outName");
        extractedAttributes.push_back(outName);
        if( ( outName == NULL ) || ( outName == Py_None ) || !RSGISPY_CHECK_INT(outName)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'outName\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }
        
        threshold = PyObject_GetAttrString(o, "threshold");
        extractedAttributes.push_back(threshold);
        if( ( threshold == NULL ) || ( threshold == Py_None ) || !RSGISPY_CHECK_FLOAT(threshold)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find float attribute \'threshold\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }
        
        count = PyObject_GetAttrString(o, "count");
        extractedAttributes.push_back(count);
        if( ( count == NULL ) || ( count == Py_None ) || !RSGISPY_CHECK_INT(count)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'count\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }
        
        means = PyObject_GetAttrString(o, "means");
        extractedAttributes.push_back(means);
        if( ( means == NULL ) || ( means == Py_None ) || !PySequence_Check(means)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find sequence attribute \'means\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }
        
        stdDevs = PyObject_GetAttrString(o, "stddev");
        extractedAttributes.push_back(stdDevs);
        if( ( stdDevs == NULL ) || ( stdDevs == Py_None ) || !PySequence_Check(stdDevs)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find sequence attribute \'stddev\'" );
            FreePythonObjects(extractedAttributes);
            return NULL;
        }
        
        classField.name = RSGISPY_STRING_EXTRACT(name);
        classField.outName = RSGISPY_INT_EXTRACT(outName);
        classField.threshold = RSGISPY_FLOAT_EXTRACT(threshold);
        classField.count = RSGISPY_INT_EXTRACT(count);
        
        // now unpack the sequences
        Py_ssize_t nMeans = PySequence_Size(means);
        classField.means = new float[nMeans];
        
        for(int i = 0; i < nMeans; ++i) {
            PyObject *fieldObj = PySequence_GetItem(means, i);
            
            if(!RSGISPY_CHECK_FLOAT(fieldObj)) {
                PyErr_SetString(GETSTATE(self)->error, "Means must be floats");
                Py_DECREF(fieldObj);
                delete classField.means;
                FreePythonObjects(extractedAttributes);
                return NULL;
            }
            
            classField.means[i] = RSGISPY_FLOAT_EXTRACT(fieldObj);
            Py_DECREF(fieldObj);
        }
        
        Py_ssize_t nStdDev = PySequence_Size(stdDevs);
        classField.stddev = new float[nStdDev];
        
        for(int i = 0; i < nStdDev; ++i) {
            PyObject *fieldObj = PySequence_GetItem(stdDevs, i);
            
            if(!RSGISPY_CHECK_FLOAT(fieldObj)) {
                PyErr_SetString(GETSTATE(self)->error, "Stddevs must be floats");
                Py_DECREF(fieldObj);
                delete classField.stddev;
                FreePythonObjects(extractedAttributes);
                return NULL;
            }
            
            classField.stddev[i] = RSGISPY_FLOAT_EXTRACT(fieldObj);
            Py_DECREF(fieldObj);
        }
        
        classFields.push_back(classField);
        FreePythonObjects(extractedAttributes);
    }
    
    try {
        rsgis::cmds::executeFindChangeClumpsFromStdDev(std::string(clumpsImage), std::string(classField), std::string(changeField), attFields, classFields);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}



static PyObject *RasterGIS_SelectClumpsOnGrid(PyObject *self, PyObject *args) {
    const char *clumpsImage, *inSelectField, *outSelectField, *eastingsCol, *northingsCol, *metricField, *methodStr;
    unsigned int rows, cols;
    
    if(!PyArg_ParseTuple(args, "sssssssII:selectClumpsOnGrid", &clumpsImage, &inSelectField, &outSelectField, &eastingsCol, &northingsCol, &metricField, &methodStr, &rows, &cols)) {
        return NULL;
    }
    
    try {
        rsgis::cmds::executeIdentifyClumpExtremesOnGrid(std::string(clumpsImage), std::string(inSelectField), std::string(outSelectField), std::string(eastingsCol), std::string(northingsCol), std::string(methodStr), rows, cols, std::string(metricField));
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *RasterGIS_InterpolateClumpValues2Img(PyObject *self, PyObject *args) {
    const char *clumpsImage, *selectField, *eastingsField, *northingsField, *methodStr, *valueField, *outputFile, *imageFormat;
    int dataType;
    
    if(!PyArg_ParseTuple(args, "ssssssssi:interpolateClumpValues2Image", &clumpsImage, &selectField, &eastingsField, &northingsField, &methodStr, &valueField, &outputFile, &imageFormat, &dataType)) {
        return NULL;
    }
    
    try {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType) dataType;
        rsgis::cmds::executeInterpolateClumpValuesToImage(std::string(clumpsImage), std::string(selectField), std::string(eastingsField), std::string(northingsField), std::string(methodStr), std::string(valueField), std::string(outputFile), std::string(imageFormat), type);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}





static PyMethodDef RasterGISMethods[] = {
    {"populateStats", RasterGIS_PopulateStats, METH_VARARGS,
"rastergis.populateStats(clumpsImage, addColourTable2Img, calcImgPyramids)\n"
"Populates statics for thermatic imagess\n"
"where:\n"
"* clumpsImage is a string containing the name of the input clump file\n"
"* addColourTable2Img is a boolean TODO: expand\n"
"* calcImagePyramids is a boolean\n"
},

    {"copyRAT", RasterGIS_CopyRAT, METH_VARARGS,
"rastergis.copyRAT(inputImage, clumpsImage)\n"
"Copies a GDAL RAT from one image to anoother\n"
"where:\n"
"* inputImage is a string containing the name of the input image file\n"
"* clumpsImage is a string containing the name of the input clump file\n"
},

    {"copyGDALATTColumns", RasterGIS_CopyGDALATTColumns, METH_VARARGS,
"rastergis.copyGDALATTColumns(inputImage, clumpsImage, fields)\n"
"Copies GDAL RAT columns from one image to another\n"
"where:\n"
"* inputImage is a string containing the name of the input image file\n"
"* clumpsImage is a string containing the name of the input clump file\n"
"* fields is a sequence of strings containing the names of the fields TODO: expand"
},

   {"spatialLocation", RasterGIS_SpatialLocation, METH_VARARGS,
"rastergis.spatialLocation(inputImage, eastingsField, northingsField)\n"
"Adds spatial location columns to the attribute table\n"
"where:\n"
"* inputImage is a string containing the name of the input image file\n"
"* eastingsField is a string containing the name of the eastings field\n"
"* northingsField is a string containing the name of the northings field\n"
},

    {"populateRATWithStats", RasterGIS_PopulateRATWithStats, METH_VARARGS,
"rsgislib.rastergis.populateRATWithStats(inputImage, clumpsImage, bandStatsCmds)\n"
"Populates an attribute table with statistics from an image.\n"
"where:\n"
"* inputImage is a string containing the name of the input image file\n"
"* clumpsImage is a string containing the name of the input clump file\n"
"* bandStatsCmds is a sequence of rsgislib.rastergis.BandAttStats objects that have attributes in line with rsgis.cmds.RSGISBandAttStatsCmds\n"
"     * band: int defining the image band to process\n"
"     * countField: string defining the name of the field for count value\n"
"     * minField: string defining the name of the field for min value\n"
"     * maxField: string defining the name of the field for max value\n"
"     * sumField: string defining the name of the field for sum value\n"
"     * meanField: string defining the name of the field for mean value\n"
"     * stdDevField: string defining the name of the field for standard deviation value\n"
"     * medianField: string defining the name of the field for median value\n"
"Example::\n"
"\n"
"	from rsgislib import rastergis\n"
"	clumps='./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_popstats.kea'\n"
"	input='./Rasters/injune_p142_casi_sub_utm.kea'\n"
"	bs = []\n"
"	bs.append(rastergis.BandAttStats(band=1, minField='b1Min', maxField='b1Max', meanField='b1Mean', sumField='b1Sum', stdDevField='b1StdDev'))\n"
"	bs.append(rastergis.BandAttStats(band=2, minField='b2Min', maxField='b2Max', meanField='b2Mean', sumField='b2Sum', stdDevField='b2StdDev'))\n"
"	bs.append(rastergis.BandAttStats(band=3, minField='b3Min', maxField='b3Max', meanField='b3Mean', sumField='b3Sum', stdDevField='b3StdDev'))\n"
"	rastergis.populateRATWithStats(input, clumps, bs)\n"
"\n"},

    {"populateRATWithPercentiles", RasterGIS_PopulateRATWithPercentiles, METH_VARARGS,
"rastergis.populateRATWithPercentiles(inputImage, clumpsImage, bandPercentiles)\n"
"Populates an attribute table with a percentile of the pixel values from an image */\n"
"where:\n"
"* inputImage is a string containing the name of the input image file\n"
"* clumpsImage is a string containing the name of the input clump file\n"
"* bandPercentiles is a sequence of objects that have attributes matching rsgis.cmds.RSGISBandAttPercentilesCmds\n"
"  Requires: TODO: Check\n"
"\n"
"  * band: int defining the image band to process\n"
"  * percentile: int defining the percentile to calculate\n"
"  * fieldName: string defining the name of the field to use for this percentile\n"
"\n"},

   {"populateCategoryProportions", RasterGIS_PopulateCategoryProportions, METH_VARARGS,
"rastergis.populateCategoryProportions(categoriesImage, clumpsImage, outColsName, majorityColName, copyClassNames, majClassNameField, classNameField)\n"
"Populates the attribute table with the proporations of intersecting catagories\n"
"where:\n"
"* categoriesImage is a string containing the name of the categories image file TODO: check and expand\n"
"* clumpsImage is a string containing the name of the input clump file\n"
"* outColsName is a string\n"
"* majorityColName is a string\n"
"* copyClassNames is a boolean defining whether class names should be copied\n"
"* majClassNameField is a string\n"
"* classNameField is a string\n"
},

   {"copyCategoriesColours", RasterGIS_CopyCategoriesColours, METH_VARARGS,
"rastergis.copyCategoriesColours(categoriesImage, clumpsImage, classField)\n"
"Copies an attribute tables colour table to another table based on class column\n"
"where:\n"
"* categoriesImage is a string containing the name of the categories image file TODO: check and expand\n"
"* clumpsImage is a string containing the name of the input clump file\n"
"* outColsName is a string containing the name of the class field\n"
},

   {"exportCols2GDALImage", RasterGIS_ExportCols2GDALImage, METH_VARARGS,
"rastergis.exportCols2GDALImage(inputImage, outputFile, gdalFormat, gdalDataType, fields)\n"
"Exports columns of the attribute table as GDAL images\n"
"where:\n"
"* inputImage is a string containing the name of the input image file TODO: check and expand\n"
"* outputFile is a string containing the name of the output gdal file\n"
"* gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* gdaltype is an int containing one of the values from rsgislib.TYPE_*\n"
"* fields is a sequence of strings\n"
},

   {"eucDistFromFeature", RasterGIS_EucDistFromFeature, METH_VARARGS,
"rastergis.eucDistFromFeature(inputImage, fid, outputField, fields)\n"
"Calculates the euclidean distance from a feature to all other features\n"
"where:\n"
"* inputImage is a string containing the name of the input image file TODO: check and expand\n"
"* fid is an integer\n"
"* outputField is a string continaing the name of the output field\n"
"* fields is a sequence of strings\n"
},

   {"findTopN", RasterGIS_FindTopN, METH_VARARGS,
"rastergis.findTopN(inputImage, spatialDistField, distanceField, outputField, nFeatures, distThreshold)\n"
"Calculates the top N features within a given spatial distance\n"
"where:\n"
"* inputImage is a string containing the name of the input image file TODO: check and expand\n"
"* spatialDistField is a string containing the name of the field containing the spatial distance\n"
"* distanceField is a string containing the name of the field containing the distance\n"
"* outputField is a string continaing the name of the output field\n"
"* nFeatures is an int containing the number of features to find\n"
"* distThreshold is a float specifying the distance threshold with which to operate\n"
},

   {"findSpecClose", RasterGIS_FindSpecClose, METH_VARARGS,
"rastergis.findSpecClose(inputImage, spatialDistField, distanceField, outputField, specDistThreshold, distThreshold)\n"
"Calculates the features within a given spatial and spectral distance\n"
"where:\n"
"* inputImage is a string containing the name of the input image file TODO: check and expand\n"
"* spatialDistField is a string containing the name of the field containing the spatial distance\n"
"* distanceField is a string containing the name of the field containing the distance\n"
"* outputField is a string continaing the name of the output field\n"
"* specDistThreshold is a float specifying the spectral distance threshold with which to operate\n"
"* distThreshold is a float specifying the spatial distance threshold with which to operate\n"
},

    {"export2Ascii", RasterGIS_Export2Ascii, METH_VARARGS,
"rastergis.export2Ascii(inputImage, outputFile, fields)\n"
"Exports selected columns from a GDAL RAT to ASCII file (comma seperated).\n"
"where:\n"
"* inputImage is a string containing the name of the input image file TODO: check and expand\n"
"* outputFile is a string containing the name of the output file \n"
"* fields is a sequence of strings containing the field names\n"
"Example::\n"
"\n"
" table='./RATS/injune_p142_casi_sub_utm_clumps_elim_final_clumps_elim_final.kea'\n"
" output='./TestOutputs/RasterGIS/injune_p142_casi_rgb_exportascii.txt'\n"
" fields = ['BlueAvg', 'GreenAvg', 'RedAvg']\n"
" rastergis.export2Ascii(table, output, fields)\n"
"\n"},

    {"classTranslate", RasterGIS_ClassTranslate, METH_VARARGS,
"rastergis.classTranslate(inputImage, classInField, classOutField, classPairs)\n"
"Translates a set of classes to another\n"
"where:\n"
"* inputImage is a string containing the name of the input image file TODO: check and expand\n"
"* classInField is a string containing the name of the input class field\n"
"* classOutField is a string containing the name of the output class field\n"
"* classPairs is a dict of int key value pairs mapping the classes. TODO: Fixme\n"
},

    {"colourClasses", RasterGIS_ColourClasses, METH_VARARGS,
"rastergis.colourClasses(inputImage, classInField, classColourPairs)\n"
"Sets a colour table for a set of classes within the attribute table\n"
"where:\n"
"* inputImage is a string containing the name of the input image file TODO: check and expand\n"
"* classInField is a string containing the name of the input class field\n"
"* classColourPairs is dict mapping int class ids to an object having attributes matching rsgis.cmds.RSGISColourIntCmds TODO: Fixme\n"
"Requires:\n"
"\n"
"  * red: int defining the red colour component (0 - 255)\n"
"  * green: int defining the green colour component (0 - 255)\n"
"  * blue: int defining the bluecolour component (0 - 255)\n"
"  * alpha: int defining the alpha colour component (0 - 255)\n"
"\n"},

    {"colourStrClasses", RasterGIS_ColourStrClasses, METH_VARARGS,
"rastergis.colourStrClasses(inputImage, classInField, classColourPairs)\n"
"Sets a colour table for a set of classes (string column) within the attribute table\n"
"where:\n"
"inputImage is a string containing the name of the input image file TODO: check and expand\n"
"classInField is a string containing the name of the input class field\n"
"classColourPairs is dict mapping string class columns to an object having attributes matching rsgis.cmds.RSGISColourIntCmds TODO: Fixme\n"
"Requires:\n"
"\n"
"  * red: int defining the red colour component (0 - 255)\n"
"  * green: int defining the green colour component (0 - 255)\n"
"  * blue: int defining the bluecolour component (0 - 255)\n"
"  * alpha: int defining the alpha colour component (0 - 255)\n"
"\n"},

    {"generateColourTable", RasterGIS_GenerateColourTable, METH_VARARGS,
"rastergis.generateColourTable(inputImage, classInField, classColourPairs)\n"
"Generates a colour table using an input image\n"
"where:\n"
"* inputImage is a string containing the name of the input image file TODO: check and expand\n"
"* classInField is a string containing the name of the input class field\n"
"* classColourPairs is dict mapping string class columns to an object having attributes matching rsgis.cmds.RSGISColourIntCmds TODO: Fixme\n"
"* Requires:\n"
"\n"
"  * red: int defining the red colour component (0 - 255)\n"
"  * green: int defining the green colour component (0 - 255)\n"
"  * blue: int defining the bluecolour component (0 - 255)\n"
"  * alpha: int defining the alpha colour component (0 - 255)\n"
"\n"},

    {"strClassMajority", RasterGIS_StrClassMajority, METH_VARARGS,
"Finds the majority for class (string - field) from a set of small objects to large objects\n"
"rastergis.strClassMajority(baseSegment, infoSegment, baseClassCol, infoClassCol)\n"
"where:\n"
"* baseSegment is a the base clumps file, to be attribured.\n"
"* infoSegment is the file to take attributes from.\n"
"* baseClassCol the output column name in the baseSegment file.\n"
"* infoClassCol is the colum name in the infoSegment file.\n"
"Example::\n"
"\n"
"	from rsgislib import rastergis\n"
"	clumps='./TestOutputs/RasterGIS/injune_p142_casi_sub_utm_segs_popstats.kea'\n"
"	classRAT='./TestOutputs/RasterGIS/reInt_rat.kea'\n"
"	rastergis.strClassMajority(clumps, classRAT, 'REInt', 'REInt')\n"
"\n"},

    {"specDistMajorityClassifier", RasterGIS_SpecDistMajorityClassifier, METH_VARARGS,
"rastergis.specDistMajorityClassifier(inputImage, inClassNameField, outClassNameField, trainingSelectCol, eastingsField, northingsField, areaField, \n"
"Classifies segments using a spectral distance majority classification\n"
"                                                   majWeightField, fields, distThreshold, specDistThreshold, distMethod, specThreshOriginDist)\n"
"where:\n"
"* inputImage is a string containing the name of the input image file TODO: expand\n"
"* inClassNameField is a string\n"
"* outClassNameField is a string\n"
"* trainingSelectCol is a string\n"
"* eastingsField is a string\n"
"* northingsField is a string\n"
"* areaField is a string\n"
"* majWeightField is a string\n"
"* fields is a sequence of strings containing field names\n"
"* distThreshold is a float\n"
"* specDistThreshold is a float\n"
"* specThreshOriginDist is a float\n"
},
    
    {"maxLikelihoodClassifier", RasterGIS_MaxLikelihoodClassifier, METH_VARARGS,
"rastergis.maxLikelihoodClassifier(inputImage, inClassNameField, outClassNameField, trainingSelectCol, areaField, \n"
"Classifies segments using a maximum likelihood classification\n"
"                                                   fields, priorsMethod, priorStrs)\n"
"where:\n"
"* inputImage is a string containing the name of the input image file TODO: expand\n"
"* inClassNameField is a string\n"
"* outClassNameField is a string\n"
"* trainingSelectCol is a string\n"
"* areaField is a string\n"
"* fields is a sequence of strings containing field names\n"
"* priorsMethod is an int containing one of the values from rsgislib.METHOD_*\n"
"* priorStrs is a sequence of strings\n"
},
    
    {"maxLikelihoodClassifierLocalPriors", RasterGIS_MaxLikelihoodClassifierLocalPriors, METH_VARARGS,
"rastergis.maxLikelihoodClassifierLocalPriors(inputImage, inClassNameField, outClassNameField, trainingSelectCol, areaField, fields, \n"
"Classifies segments using a maximum likelihood classification and local priors\n"
"                                                   eastingsField, northingsField, distThreshold, priorsMethod, weightA, allowZeroPriors)\n"
"where:\n"
"* inputImage is a string containing the name of the input image file TODO: expand\n"
"* inClassNameField is a string\n"
"* outClassNameField is a string\n"
"* trainingSelectCol is a string\n"
"* areaField is a string\n"
"* fields is a sequence of strings containing field names\n"
"* eastingsField is a string containing the name of the field holding the eastings\n"
"* northingsField is a string containing the name of the field holding the northings\n"
"* distThreshold is a float\n"
"* priorsMethod is an int containing one of the values from rsgislib.METHOD_*\n"
"* weightA is a float\n"
"* allowZeroPriors is a bool\n"
},
    
    {"classMask", RasterGIS_ClassMask, METH_VARARGS,
"rastergis.classMask(inputImage, classField, className, outputFile, gdalFormat, gdalType)\n"
"Generates a mask for a particular class\n"
"where:\n"
"* inputImage is a string containing the name of the input image file TODO: expand\n"
"* classField is a string\n"
"* className is a string\n"
"* outputFile is a string containing the name of the output file\n"
"* gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* gdaltype is an int containing one of the values from rsgislib.TYPE_*\n"
},
    
    {"findNeighbours", RasterGIS_FindNeighbours, METH_VARARGS,
"rastergis.findNeighbours(inputImage)\n"
"Finds the clump neighbours from an image\n"
"where:\n"
"* inputImage is a string containing the name of the input image file\n"
},
    
    {"findBoundaryPixels", RasterGIS_FindBoundaryPixels, METH_VARARGS,
"rastergis.findBoundaryPixels(inputImage, outputFile, gdalFormat)\n"
"Identifies the pixels on the boundary of the clumps\n"
"where:\n"
"* inputImage is a string containing the name of the input image file\n"
"* outputFile is a string containing the name of the output file\n"
"* gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
},
    
    {"calcBorderLength", RasterGIS_CalcBorderLength, METH_VARARGS,
"rastergis.calcBorderLength(inputImage, ignoreZeroEdges, outColsName)\n"
"Calculate the border length of clumps\n"
"where:\n"
"* inputImage is a string containing the name of the input image file\n"
"* ignoreZeroEdges is a bool\n"
"* outColsName is a string\n"
},
    
    {"calcRelBorder", RasterGIS_CalcRelBorder, METH_VARARGS,
"rastergis.calcRelBorder(inputImage, outColsName, classNameField, className, ignoreZeroEdges)\n"
"Calculates the relative border length of the clumps to a class\n"
"where:\n"
"* inputImage is a string containing the name of the input image file\n"
"* outColsName is a string\n"
"* classNameField is a string\n"
"* className is a string\n"
"* ignoreZeroEdges is a bool\n"
},

    {"calcShapeIndices", RasterGIS_CalcShapeIndices, METH_VARARGS,
"rastergis.calcShapeIndices(inputImage, shapeIndices)\n"
"Calculates shape indices for clumps\n"
"where:\n"
"* inputImage is a string containing the name of the input image file\n"
"* shapeIndices is a sequence of rsgislib.rastergis.ShapeIndex objects that have the following attributes:\n"
"\n"
"  * colName - a string holding the column name\n"
"  * colIdx - an int holding the column index\n"
"  * idx - an int containing one of the values from rsgis.SHAPE_*\n"
},
    
    {"defineClumpTilePositions", RasterGIS_DefineClumpTilePositions, METH_VARARGS,
"rastergis.defineClumpTilePositions(clumpsImage, tileImage, outColsName, tileOverlap, tileBoundary, tileBody)\n"
"Defines the position within the file of the clumps\n"
"where:\n"
"* clumpsImage is a string containing the name of the input clump file\n"
"* tileImage is a string containing the name of the input tile image\n"
"* outColsName is a string containing the name of the output column\n"
"* tileOverlap is an unsigned int defining the overlap between tiles\n"
"* tileBoundary is an unsigned int\n"
"* tileBody is an unsigned int\n"
},

    {"defineBorderClumps", RasterGIS_DefineBorderClumps, METH_VARARGS,
"rastergis.defineBorderClumps(clumpsImage, tileImage, outColsName, tileOverlap, tileBoundary, tileBody)\n"
"Defines the clumps which are on the border within the file of the clumps using a mask\n"
"where:\n"
"* clumpsImage is a string containing the name of the input clump file\n"
"* tileImage is a string containing the name of the input tile image\n"
"* outColsName is a string containing the name of the output column\n"
"* tileOverlap is an unsigned int defining the overlap between tiles\n"
"* tileBoundary is an unsigned int\n"
"* tileBody is an unsigned int\n"
},
    
    {"findChangeClumpsFromStdDev", RasterGIS_FindChangeClumpsFromStdDev, METH_VARARGS,
"rastergis.findChangeClumpsFromStdDev(clumpsImage, classField, changeField, attFields, classChangeFields)\n"
"Identifies segments which have changed by looking for statistical outliers (std dev) from class population\n"
"where:\n"
"* clumpsImage is a string containing the name of the input clump file\n"
"* classField is a string\n"
"* changeField is a string\n"
"* attFields is a sequence of strings\n"
"* classChangeFields is a sequence of python objects having the following attributes:\n"
"\n"
"  * name - a string holding the name\n"
"  * outName - an int\n"
"  * threshold - a float\n"
"  * count - an int, possibly number of means/std devs? TODO: Check these\n"
"  * means - a sequence of floats, containing the means\n"
"  * stddev - a sequence of floats, containing the stddevs corresponding to the means\n"
"\n"},

{"selectClumpsOnGrid", RasterGIS_SelectClumpsOnGrid, METH_VARARGS,
    "rsgislib.rastergis.selectClumpsOnGrid(clumpsImage, inSelectField, outSelectField, eastingsCol, northingsCol, metricField, methodStr, rows, cols)\n"
    "Selects a segment within a regular grid pattern across the scene. The clump is selected based on the minimum, maximum or closest to the mean.\n"
    "where:\n"
    "* clumpsImage is a string containing the name of the input clump file\n"
    "* inSelectField is a string which defines the column name where a value of 1 defines the clumps which will be included in the analysis.\n"
    "* outSelectField is a string which defines the column name where a value of 1 defines the clumps selected by the analysis.\n"
    "* eastingsCol is a string which defines a column with a eastings for each clump.\n"
    "* northingsCol is a string which defines a column with a northings for each clump.\n"
    "* metricField is a string which defines a column with a value for each clump which will be used for the distance, min, or max anaylsis.\n"
    "* methodStr is a string which defines whether the minimum, maximum or mean method of selecting a clump will be used (values can be either min, max or mean).\n"
    "* rows is an unsigned integer which defines the number of rows within which a clump will be selected.\n"
    "* cols is an unsigned integer which defines the number of columns within which a clump will be selected.\n"
    "\n"},
    
{"interpolateClumpValues2Image", RasterGIS_InterpolateClumpValues2Img, METH_VARARGS,
    "rsgislib.rastergis.interpolateClumpValues2Image(clumpsImage, selectField, eastingsField, northingsField, methodStr, valueField, outputFile, imageFormat, dataType)\n"
    "Interpolates values from clumps to the whole image of pixels.\n"
    "where:\n"
    "* clumpsImage is a string containing the name of the input clump file\n"
    "* selectField is a string which defines the column name where a value of 1 defines the clumps which will be included in the analysis.\n"
    "* eastingsField is a string which defines a column with a eastings for each clump.\n"
    "* northingsField is a string which defines a column with a northings for each clump.\n"
    "* methodStr is a string which defines a column with a value for each clump which will be used for the distance, nearestneighbour or naturalneighbour or naturalnearestneighbour or knearestneighbour anaylsis.\n"
    "* valueField is a string which defines a column containing the values to be interpolated creating the new image.\n"
    "* outputFile is a string for the path to the output image file.\n"
    "* imageFormat is string defining the GDAL format of the output image.\n"
    "* dataType is an containing one of the values from rsgislib.TYPE_*\n"
    "\n"},
    
    
    {NULL}        /* Sentinel */
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
        NULL,
        sizeof(struct RasterGisState),
        RasterGISMethods,
        NULL,
        RasterGIS_traverse,
        RasterGIS_clear,
        NULL
};

#define INITERROR return NULL

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
    if( pModule == NULL )
        INITERROR;

    struct RasterGisState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_rastergis.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }


#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
