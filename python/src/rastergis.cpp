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
