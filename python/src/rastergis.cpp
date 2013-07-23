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
/**
 * Helper func for cleaner dec reffing
 */
static void FreePythonObjects(std::vector<PyObject*> toFree) {
    std::vector<PyObject*>::iterator iter;
    for(iter = toFree.begin(); iter != toFree.end(); ++iter) {
        Py_XDECREF(*iter);
    }
}

/**
 * Helper Function for converting a python sequence of strings to a vector
 */
static std::vector<std::string> ExtractVectorStringFromSequence(PyObject sequence) {
    Py_ssize_t nFields = PySequence_Size(pFields);
    std::vector<std::string> fields;
    fields.reserve(nFields);

    for(int i = 0; i < nFields; ++i) {
        PyObject *fieldObj = PySequence_GetItem(pFields, i);

        if(!RSGISPY_CHECK_STRING(fieldObj)) {
            PyErr_SetString(GETSTATE(self)->error, "Fields must be strings");
            Py_DECREF(fieldObj);
            return NULL;
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
    if(fields == NULL) { return NULL; }

    try {
        rsgis::cmds::executeCopyGDALATTColumns(std::string(inputImage), std::string(clumpImage), fields);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_SpatialLocation(PyObject *self, PyObject *args) {
    const char *clumpsImage, *inputImage, *eastingsField, *northingsField;

    if(!PyArg_ParseTuple(args, "ssss:spatialLocation", &inputImage, &clumpsImage, &eastingsField, &northingsField))
        return NULL;

    try {
        rsgis::cmds::executeSpatialLocation(std::string(clumpsImage), std::string(inputImage), std::string(eastingsField), std::string(northingsField));
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *RasterGIS_PopulateRATWithStats(PyObject *self, PyObject *args) {
    const char *inputImage, *clumpsImage;
    PyObject pBandAttStatsCmds;

    if(!PyArg_ParseTuple(args, "ssO:populateRATWithStats", &inputImage, &clumpsImage, &pBandAttStatsCmds))
        return NULL;

     if(!PySequence_Check(pBandAttStats)) {
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
        PyObject *pBand, *pThreshold, *pCalcCount, *pCountField, *pCalcMin, *pMinField, *pCalcMax, *pMaxField, *pCalcStdDev, *pStdDevField, *pCalcMedian, *pMedianField, *pCalcSum, *pSumField, *pCalcMean, *pMeanField;
        pBand = pThreshold = pCalcCount = pCountField = pCalcMin = pMinField = pCalcMax = pMaxField = pMeanField = pCalcMean = pCalcStdDev = pStdDevField = pCalcMedian = pMedianField = pCalcSum = pSumField = NULL;

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

        pCalcCount = PyObject_GetAttrString(o, "calcCount");
        extractedAttributes.push_back(pCalcCount);
        if( ( pCalcCount == NULL ) || ( pCalcCount == Py_None ) || !RSGISPY_CHECK_INT(pCalcCount) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find bool (int) attribute \'calcCount\'" );
            FreePythonObjects(extractedAttributes);
            delete cmdObj;
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            return NULL;
        }

        pCountField = PyObject_GetAttrString(o, "countField");
        extractedAttributes.push_back(pCountField);
        if( ( pCountField == NULL ) || ( pCountField == Py_None ) || !RSGISPY_CHECK_STRING(pCountField) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'countField\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pCalcMin = PyObject_GetAttrString(o, "calcMin");
        extractedAttributes.push_back(pCalcMin);
        if( ( pCalcMin == NULL ) || ( pCalcMin == Py_None ) || !RSGISPY_CHECK_INT(pCalcMin) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find bool (int) attribute \'calcMin\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pMinField = PyObject_GetAttrString(o, "minField");
        extractedAttributes.push_back(pMinField);
        if( ( pMinField == NULL ) || ( pMinField == Py_None ) || !RSGISPY_CHECK_STRING(pMinField) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'minField\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pCalcMax = PyObject_GetAttrString(o, "calcMax");
        extractedAttributes.push_back(pCalcMax);
        if( ( pCalcMax == NULL ) || ( pCalcMax == Py_None ) || !RSGISPY_CHECK_INT(pCalcMax) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find bool (int) attribute \'calcMax\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pMaxField = PyObject_GetAttrString(o, "maxField");
        extractedAttributes.push_back(pMaxField);
        if( ( pMaxField == NULL ) || ( pMaxField == Py_None ) || !RSGISPY_CHECK_STRING(pMaxField) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'maxField\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pCalcMean = PyObject_GetAttrString(o, "calcMean");
        extractedAttributes.push_back(pCalcMean);
        if( ( pCalcMean == NULL ) || ( pCalcMean == Py_None ) || !RSGISPY_CHECK_INT(pCalcMean) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find bool (int) attribute \'calcMean\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pMeanField = PyObject_GetAttrString(o, "meanField");
        extractedAttributes.push_back(pMeanField);
        if( ( pMeanField == NULL ) || ( pMeanField == Py_None ) || !RSGISPY_CHECK_STRING(pMeanField) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'meanField\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pCalcStdDev = PyObject_GetAttrString(o, "calcStdDev");
        extractedAttributes.push_back(pCalcStdDev);
        if( ( pCalcStdDev == NULL ) || ( pCalcStdDev == Py_None ) || !RSGISPY_CHECK_INT(pCalcStdDev) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find bool (int) attribute \'stdDev\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pStdDevField = PyObject_GetAttrString(o, "stdDevField");
        extractedAttributes.push_back(pStdDevField);
        if( ( pStdDevField == NULL ) || ( pStdDevField == Py_None ) || !RSGISPY_CHECK_STRING(pStdDevField) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'stdDevField\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pCalcMedian = PyObject_GetAttrString(o, "calcMedian");
        extractedAttributes.push_back(pCalcMedian);
        if( ( pCalcMedian == NULL ) || ( pCalcMedian == Py_None ) || !RSGISPY_CHECK_INT(pCalcMedian) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find bool (int) attribute \'calcMedian\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pMedianField = PyObject_GetAttrString(o, "medianField");
        extractedAttributes.push_back(pMedianField);
        if( ( pMedianField == NULL ) || ( pMedianField == Py_None ) || !RSGISPY_CHECK_STRING(pMedianField) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'medianField\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pCalcSum = PyObject_GetAttrString(o, "calcSum");
        extractedAttributes.push_back(pCalcSum);
        if( ( pCalcSum == NULL ) || ( pCalcSum == Py_None ) || !RSGISPY_CHECK_INT(pCalcSum) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find bool (int) attribute \'calcSum\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        pSumField = PyObject_GetAttrString(o, "sumField");
        extractedAttributes.push_back(pSumField);
        if( ( pSumField == NULL ) || ( pSumField == Py_None ) || !RSGISPY_CHECK_STRING(pSumField) ) {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'sumField\'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISBandAttStatsCmds*>::iterator iter = bandStatsCmds.begin(); iter != bandStatsCmds.end(); ++iter) {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }

        // extract the values from the objects
        cmdObj->band = RSGISPY_INT_EXTRACT(pBand);
        cmdObj->threshold = RSGISPY_FLOAT_EXTRACT(pThreshold);
        cmdObj->calcCount = (RSGISPY_INT_EXTRACT(pCalcCount) != 0);
        cmdObj->calcMax = (RSGISPY_INT_EXTRACT(pCalcMax) != 0);
        cmdObj->calcMean = (RSGISPY_INT_EXTRACT(pCalcMean) != 0);
        cmdObj->calcMedian = (RSGISPY_INT_EXTRACT(pCalcMedian) != 0);
        cmdObj->calcMin = (RSGISPY_INT_EXTRACT(pCalcMin) != 0);
        cmdObj->calcStdDev = (RSGISPY_INT_EXTRACT(pCalcStdDev) != 0);
        cmdObj->calcSum = (RSGISPY_INT_EXTRACT(pCalcSum) != 0);
        cmdObj->countField = RSGISPY_STRING_EXTRACT(pCountField);
        cmdObj->maxField = RSGISPY_STRING_EXTRACT(pMaxField);
        cmdObj->meanField = RSGISPY_STRING_EXTRACT(pMeanField);
        cmdObj->medianField = RSGISPY_STRING_EXTRACT(pMedianField);
        cmdObj->minField = RSGISPY_STRING_EXTRACT(pMinField);
        cmdObj->stdDevField = RSGISPY_STRING_EXTRACT(pStdDevField);
        cmdObj->sumField = RSGISPY_STRING_EXTRACT(pSumField);

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
    PyObject pBandPercentilesCmds;

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
        if( ( pFieldName == NULL ) || ( pFieldName == Py_None ) || !RSGISPY_CHECK_INT(pFieldName)) {
            PyErr_SetString(GETSTATE(self)->error, "could not find int attribute \'fieldName\'" );
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
    const char *clumpsImage, *categoriesImage, *outColsName, *classField;
    int copyClassNames;

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
    PyObject pFields;

    if(!PyArg_ParseTuple(args, "sssiO:copyCategoriesColours", &inputImage, &outputFile, &imageFormat, &dataType, &pFields))
        return NULL;

    if(!PySequence_Check(pFields)) {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return NULL;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)dataType;

    std::vector<std::string> fields = ExtractVectorStringFromSequence(pFields);
    if(fields == NULL) { return NULL; }

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
    if(fields == NULL) { return NULL; }

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

    if(!PyArg_ParseTuple(args, "ssssff:findSpecClose", &inputImage, &distanceField, &spatialDistField, &outputField, &specDistThreshold, &distThreshold))
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
    PyObject pFields;

    if(!PyArg_ParseTuple(args, "ssO:export2Ascii", &inputImage, &outputFile, &pFields))
        return NULL;

    if(!PySequence_Check(pFields)) {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return NULL;
    }

    std::vector<std::string> fields = ExtractVectorStringFromSequence(pFields);
    if(fields == NULL) { return NULL; }

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
    PyObject pClassPairs;

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

    while (PyDict_Next(pClassPairs, &pos, &key, &value)) {
        if(!RSGISPY_CHECK_INT(key)) {
            PyErr_SetString(GETSTATE(self)->error, "dict keys must be ints");
            return NULL;
        }

        PyObject *red, *green, *blue, *alpha;
        red = green = blue = alpha = NULL;

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

    while (PyDict_Next(pClassPairs, &pos, &key, &value)) {
        if(!RSGISPY_CHECK_STRING(key)) {
            PyErr_SetString(GETSTATE(self)->error, "dict keys must be strings");
            return NULL;
        }

        PyObject *red, *green, *blue, *alpha;
        red = green = blue = alpha = NULL;

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
        classPairs[(size_t)RSGISPY_STRING_EXTRACT(key)] = colour;
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

    if(!PyArg_ParseTuple(args, "ssss:strClassMajority", &baseSegment, &infoSegment, &bassClassCol, &infoClassCol)
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

    vector<std::string> fields = ExtractVectorStringFromSequence(pFields);
    if(fields == NULL) {
        return NULL;
    }

    rsgis::cmds::SpectralDistanceMethodCmds method = (rsgis::cmds::SpectralDistanceMethodCmds)distMethod;

    try {
        rsgis::cmds::executeSpecDistMajorityClassifier(std::string(inputImage), std::string(inClassNameField), std::string(outClassNameField), std::string(trainingSelectCol), std::string(eastingsField),
                                                       std::string(northingsField), std::string(areaField), std::string(majWeightField), fields, distThreshold, specDistThreshold, method, specThreshOriginDist));
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyMethodDef RasterGISMethods[] = {
    {"populateStats", RasterGIS_PopulateStats, METH_VARARGS,
"Populates statics for thermatic imagess\n"
"call signature: rastergis.populateStats(clumpsImage, addColourTable2Img, calcImgPyramids)\n"
"where:\n"
"  clumpsImage is a string containing the name of the input clump file\n"
"  addColourTable2Img is a boolean TODO: expand\n"
"  calcImagePyramids is a boolean\n"
},

    {"copyRAT", RasterGIS_CopyRAT, METH_VARARGS,
"Copies a GDAL RAT from one image to anoother\n"
"call signature: rastergis.copyRAT(inputImage, clumpsImage)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file\n"
"  clumpsImage is a string containing the name of the input clump file\n"
},

    {"copyGDALATTColumns", RasterGIS_CopyGDALATTColumns, METH_VARARGS,
"Copies GDAL RAT columns from one image to another\n"
"call signature: rastergis.copyGDALATTColumns(inputImage, clumpsImage, fields)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file\n"
"  clumpsImage is a string containing the name of the input clump file\n"
"  fields is a sequence of strings containing the names of the fields TODO: expand"
},

   {"spatialLocation", RasterGIS_SpatialLocation, METH_VARARGS,
"Adds spatial location columns to the attribute table\n"
"call signature: rastergis.spatialLocation(inputImage, clumpsImage, eastingsField, northingsField)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file\n"
"  clumpsImage is a string containing the name of the input clump file\n"
"  eastingsField is a string containing the name of the eastings field\n"
"  northingsField is a string containing the name of the northings field\n"
},

    {"populateRATWithStats", RasterGIS_PopulateRATWithStats, METH_VARARGS,
"Populates an attribute table from an image */\n"
"call signature: rastergis.populateRATWithStats(inputImage, clumpsImage, bandStatsCmds)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file\n"
"  clumpsImage is a string containing the name of the input clump file\n"
"  bandStatsCmds is a sequence of objects that have attributes matching rsgis.cmds.RSGISBandAttStatsCmds\n"
"    Requires: TODO: Check\n"
"      band: int defining the image band to process\n"
"      calcCount: boolean defining if the count value should be calculated\n"
"      calcMin: boolean defining if the min value should be calculated\n"
"      calcMax: boolean defining if the max value should be calculated\n"
"      calcSum: boolean defining if the sum value should be calculated\n"
"      calcMean: boolean defining if the mean value should be calculated\n"
"      calcStdDev: boolean defining if the standard deviation should be calculated\n"
"      calcMedian: boolean defining if the median value should be calculated\n"
"      countField: string defining the name of the field for count value\n"
"      minField: string defining the name of the field for min value\n"
"      maxField: string defining the name of the field for max value\n"
"      sumField: string defining the name of the field for sum value\n"
"      meanField: string defining the name of the field for mean value\n"
"      stdDevField: string defining the name of the field for standard deviation value\n"
"      medianField: string defining the name of the field for median value\n"
},

    {"populateRATWithPercentiles", RasterGIS_PopulateRATWithPercentiles, METH_VARARGS,
"Populates an attribute table with a percentile of the pixel values from an image */\n"
"call signature: rastergis.populateRATWithPercentiles(inputImage, clumpsImage, bandPercentiles)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file\n"
"  clumpsImage is a string containing the name of the input clump file\n"
"  bandPercentiles is a sequence of objects that have attributes matching rsgis.cmds.RSGISBandAttPercentilesCmds\n"
"    Requires: TODO: Check\n"
"      band: int defining the image band to process\n"
"      percentile: int defining the percentile to calculate\n"
"      fieldName: string defining the name of the field to use for this percentile\n"
},

   {"populateCategoryProportions", RasterGIS_PopulateCategoryProportions, METH_VARARGS,
"Populates the attribute table with the proporations of intersecting catagories\n"
"call signature: rastergis.populateCategoryProportions(categoriesImage, clumpsImage, outColsName, majorityColName, copyClassNames, majClassNameField, classNameField)\n"
"where:\n"
"  categoriesImage is a string containing the name of the categories image file TODO: check and expand\n"
"  clumpsImage is a string containing the name of the input clump file\n"
"  outColsName is a string\n"
"  majorityColName is a string\n"
"  copyClassNames is a boolean defining whether class names should be copied\n"
"  majClassNameField is a string\n"
"  classNameField is a string\n"
},

   {"copyCategoriesColours", RasterGIS_CopyCategoriesColours, METH_VARARGS,
"Copies an attribute tables colour table to another table based on class column\n"
"call signature: rastergis.copyCategoriesColours(categoriesImage, clumpsImage, classField)\n"
"where:\n"
"  categoriesImage is a string containing the name of the categories image file TODO: check and expand\n"
"  clumpsImage is a string containing the name of the input clump file\n"
"  outColsName is a string containing the name of the class field\n"
},

   {"exportCols2GDALImage", RasterGIS_ExportCols2GDALImage, METH_VARARGS,
"Exports columns of the attribute table as GDAL images\n"
"call signature: rastergis.exportCols2GDALImage(inputImage, outputFile, gdalFormat, gdalDataType, fields)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file TODO: check and expand\n"
"  outputFile is a string containing the name of the output gdal file\n"
"  gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  gdaltype is an containing one of the values from rsgislib.TYPE_*\n"
"  fields is a sequence of strings\n"
},

   {"eucDistFromFeature", RasterGIS_EucDistFromFeature, METH_VARARGS,
"Calculates the euclidean distance from a feature to all other features\n"
"call signature: rastergis.eucDistFromFeature(inputImage, fid, outputField, fields)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file TODO: check and expand\n"
"  fid is an integer\n"
"  outputField is a string continaing the name of the output field\n"
"  fields is a sequence of strings\n"
},

   {"findTopN", RasterGIS_FindTopN, METH_VARARGS,
"Calculates the top N features within a given spatial distance\n"
"call signature: rastergis.findTopN(inputImage, spatialDistField, distanceField, outputField, nFeatures, distThreshold)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file TODO: check and expand\n"
"  spatialDistField is a string containing the name of the field containing the spatial distance\n"
"  distanceField is a string containing the name of the field containing the distance\n"
"  outputField is a string continaing the name of the output field\n"
"  nFeatures is an int containing the number of features to find\n"
"  distThreshold is a float specifying the distance threshold with which to operate\n"
},

   {"findSpecClose", RasterGIS_FindSpecClose, METH_VARARGS,
"Calculates the features within a given spatial and spectral distance\n"
"call signature: rastergis.findSpecClose(inputImage, distanceField, spatialDistField, outputField, specDistThreshold, distThreshold)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file TODO: check and expand\n"
"  distanceField is a string containing the name of the field containing the distance\n"
"  spatialDistField is a string containing the name of the field containing the spatial distance\n"
"  outputField is a string continaing the name of the output field\n"
"  specDistThreshold is a float specifying the spectral distance threshold with which to operate\n"
"  distThreshold is a float specifying the spatial distance threshold with which to operate\n"
},

    {"export2Ascii", RasterGIS_Export2Ascii, METH_VARARGS,
"Exports columns from a GDAL RAT to ascii\n"
"call signature: rastergis.export2Ascii(inputImage, outputFile, fields)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file TODO: check and expand\n"
"  outputFile is a string containing the name of the output file \n"
"  fields is a sequence of strings containing the field names\n"
},

    {"classTranslate", RasterGIS_ClassTranslate, METH_VARARGS,
"Translates a set of classes to another\n"
"call signature: rastergis.classTranslate(inputImage, classInField, classOutField, classPairs)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file TODO: check and expand\n"
"  classInField is a string containing the name of the input class field\n"
"  classOutField is a string containing the name of the output class field\n"
"  classPairs is a dict of int key value pairs mapping the classes. TODO: Fixme\n"
},

    {"colourClasses", RasterGIS_ColourClasses, METH_VARARGS,
"Sets a colour table for a set of classes within the attribute table\n"
"call signature: rastergis.colourClasses(inputImage, classInField, classColourPairs)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file TODO: check and expand\n"
"  classInField is a string containing the name of the input class field\n"
"  classColourPairs is dict mapping int class ids to an object having attributes matching rsgis.cmds.RSGISColourIntCmds TODO: Fixme\n"
"  Requires: TODO: Check\n"
"      red: int defining the red colour component\n"
"      green: int defining the green colour component\n"
"      blue: int defining the bluecolour component\n"
"      alpha: int defining the alpha colour component\n"
},

    {"colourStrClasses", RasterGIS_ColourStrClasses, METH_VARARGS,
"Sets a colour table for a set of classes (string column) within the attribute table\n"
"call signature: rastergis.colourStrClasses(inputImage, classInField, classColourPairs)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file TODO: check and expand\n"
"  classInField is a string containing the name of the input class field\n"
"  classColourPairs is dict mapping string class columns to an object having attributes matching rsgis.cmds.RSGISColourIntCmds TODO: Fixme\n"
"  Requires: TODO: Check\n"
"      red: int defining the red colour component\n"
"      green: int defining the green colour component\n"
"      blue: int defining the bluecolour component\n"
"      alpha: int defining the alpha colour component\n"
},

    {"generateColourTable", RasterGIS_GenerateColourTable, METH_VARARGS,
"Generates a colour table using an input image\n"
"call signature: rastergis.generateColourTable(inputImage, classInField, classColourPairs)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file TODO: check and expand\n"
"  classInField is a string containing the name of the input class field\n"
"  classColourPairs is dict mapping string class columns to an object having attributes matching rsgis.cmds.RSGISColourIntCmds TODO: Fixme\n"
"  Requires: TODO: Check\n"
"      red: int defining the red colour component\n"
"      green: int defining the green colour component\n"
"      blue: int defining the bluecolour component\n"
"      alpha: int defining the alpha colour component\n"
},

    {"strClassMajority", RasterGIS_StrClassMajority, METH_VARARGS,
"Finds the majority for class (string - field) from a set of small objects to large objects\n"
"call signature: rastergis.strClassMajority(baseSegment, infoSegment, baseClassCol, infoClassCol)\n"
"where:\n"
"  baseSegment is a string TODO: expand\n"
"  infoSegment is a string\n"
"  baseClassCol is a string\n"
"  infoClassCol is a string\n"
},

    {"specDistMajorityClassifier", RasterGIS_SpecDistMajorityClassifier, METH_VARARGS,
"Classifies segments using a spectral distance majority classification\n"
"call signature: rastergis.specDistMajorityClassifier(inputImage, inClassNameField, outClassNameField, trainingSelectCol, eastingsField, northingsField, areaField, \n"
"                                                     majWeightField, fields, distThreshold, specDistThreshold, distMethod, specThreshOriginDist)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file TODO: expand\n"
"  inClassNameField is a string\n"
"  outClassNameField is a string\n"
"  trainingSelectCol is a string\n"
"  eastingsField is a string\n"
"  northingsField is a string\n"
"  areaField is a string\n"
"  majWeightField is a string\n"
"  fields is a sequence of strings containing field names\n"
"  distThreshold is a float\n"
"  specDistThreshold is a float\n"
"  specThreshOriginDist is a float\n"
},
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

    // add constants
    PyModule_AddIntConstant(pModule, "INITCLUSTER_RANDOM", rsgis::cmds::rsgis_init_random);
    PyModule_AddIntConstant(pModule, "INITCLUSTER_DIAGONAL_FULL", rsgis::cmds::rsgis_init_diagonal_full);
    PyModule_AddIntConstant(pModule, "INITCLUSTER_DIAGONAL_STDDEV", rsgis::cmds::rsgis_init_diagonal_stddev);
    PyModule_AddIntConstant(pModule, "INITCLUSTER_DIAGONAL_FULL_ATTACH", rsgis::cmds::rsgis_init_diagonal_full_attach);
    PyModule_AddIntConstant(pModule, "INITCLUSTER_DIAGONAL_STDDEV_ATTACH", rsgis::cmds::rsgis_init_diagonal_stddev_attach);

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
