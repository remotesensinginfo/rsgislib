/*
 *  imagecalc.cpp
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
#include "cmds/RSGISCmdImageCalc.h"

/* An exception object for this module */
/* created in the init function */
struct ImageCalcState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ImageCalcState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ImageCalcState _state;
#endif

static PyObject *ImageCalc_bandMath(PyObject *self, PyObject *args)
{
    const char *pszOutputFile, *pszExpression, *pszGDALFormat;
    int nDataType;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTuple(args, "sssiO:bandMath", &pszOutputFile, &pszExpression, &pszGDALFormat, 
                                        &nDataType, &pBandDefnObj))
        return NULL;

    // we made these values the same so should work
    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;

    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return NULL;
    }

    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    rsgis::cmds::VariableStruct *pRSGISStruct = new rsgis::cmds::VariableStruct[nBandDefns];

    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);

        PyObject *pBandName = PyObject_GetAttrString(o, "bandName");
        if( ( pBandName == NULL ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'bandName\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return NULL;
        }

        PyObject *pFileName = PyObject_GetAttrString(o, "fileName");
        if( ( pFileName == NULL ) || ( pFileName == Py_None ) || !RSGISPY_CHECK_STRING(pFileName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'fileName\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pFileName);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return NULL;
        }

        PyObject *pBandIndex = PyObject_GetAttrString(o, "bandIndex");
        if( ( pBandIndex == NULL ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find integer attribute \'bandIndex\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return NULL;
        }

        pRSGISStruct[n].name = RSGISPY_STRING_EXTRACT(pBandName);
        pRSGISStruct[n].image = RSGISPY_STRING_EXTRACT(pFileName);
        pRSGISStruct[n].bandNum = RSGISPY_INT_EXTRACT(pBandIndex);

        Py_DECREF(pBandName);
        Py_DECREF(pFileName);
        Py_DECREF(pBandIndex);
        Py_DECREF(o);
    }

    try
    {
        rsgis::cmds::executeBandMaths(pRSGISStruct, nBandDefns, pszOutputFile, pszExpression, pszGDALFormat, type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    delete[] pRSGISStruct;

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_imageMath(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszExpression, *pszGDALFormat;
    int nDataType;
    if( !PyArg_ParseTuple(args, "ssssi:bandMath", &pszInputImage, &pszOutputFile, &pszExpression, 
                                &pszGDALFormat, &nDataType ))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;

    try
    {
        rsgis::cmds::executeImageMaths(pszInputImage, pszOutputFile, pszExpression, pszGDALFormat, type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_KMeansClustering(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile;
    unsigned int nNumClusters, nMaxNumIterations, nSubSample;
    int nIgnoreZeros; // passed as a bool - seems the only way to pass into C
    float fDegreeOfChange;
    int nClusterMethod;
    if( !PyArg_ParseTuple(args, "ssIIIifi:KMeansClustering", &pszInputImage, &pszOutputFile, &nNumClusters, 
                                &nMaxNumIterations, &nSubSample, &nIgnoreZeros, &fDegreeOfChange, &nClusterMethod ))
        return NULL;

    try
    {
        rsgis::cmds::executeKMeansClustering(pszInputImage, pszOutputFile, nNumClusters, nMaxNumIterations,
                            nSubSample, nIgnoreZeros, fDegreeOfChange, (rsgis::cmds::RSGISInitClustererMethods)nClusterMethod);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ISODataClustering(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile;
    unsigned int nNumClusters, nMaxNumIterations, nSubSample, minNumFeatures, minNumClusters;
    unsigned int startIteration, endIteration;
    int nIgnoreZeros; // passed as a bool - seems the only way to pass into C
    float fDegreeOfChange, fMinDistBetweenClusters, maxStdDev;
    int nClusterMethod;
    if( !PyArg_ParseTuple(args, "ssIIIififIfIII:ISODataClustering", &pszInputImage, &pszOutputFile, &nNumClusters, 
                                &nMaxNumIterations, &nSubSample, &nIgnoreZeros, &fDegreeOfChange, &nClusterMethod,
                                &fMinDistBetweenClusters, &minNumFeatures, &maxStdDev, &minNumClusters, 
                                &startIteration, &endIteration ))
        return NULL;

    try
    {
        rsgis::cmds::executeISODataClustering(pszInputImage, pszOutputFile, nNumClusters, nMaxNumIterations, 
                            nSubSample, nIgnoreZeros, fDegreeOfChange, (rsgis::cmds::RSGISInitClustererMethods)nClusterMethod, fMinDistBetweenClusters,
                            minNumFeatures, maxStdDev, minNumClusters, startIteration, endIteration);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef ImageCalcMethods[] = {
    {"bandMath", ImageCalc_bandMath, METH_VARARGS, 
"Performs band math calculation.\n"
"call signature: imagecalc.bandMath(outputImage, expression, gdalformat, gdaltype, bandDefnSeq)\n"
"where:\n"
"  outputImage is a string containing the name of the output file\n"
"  expression is a string containing the expression to run over the images\n"
"  gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  gdaltype is an containing one of the values from rsgislib.TYPE_*\n"
"  bandDefnSeq is a sequence of rsgislib.imagecalc.BandDefn objects that define the inputs"},

    {"imageMath", ImageCalc_imageMath, METH_VARARGS,
"Performs image math calculation.\n"
"call signature: imagecalc.imageMath(inputImage, outputImage, expression, gdalformat, gdaltype)\n"
"where:\n"
"  inimage is a string containing the name of the input file\n"
"  outputImage is a string containing the name of the output file\n"
"  expression is a string containing the expression to run over the images\n"
"  gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  gdaltype is an containing one of the values from rsgislib.TYPE_*\n"},

    {"KMeansClustering", ImageCalc_KMeansClustering, METH_VARARGS,
"Performs K Means Clustering.\n"
"call signature: imagecalc.KMeansClustering(inputImage, outputMatrix, numClusters, maxIterations, subSample, ignoreZeros, degreeOfChange, initMethod)\n"
"where:\n"
"  inputImage is a string\n"
"  outputMatrix is a string\n"
"  numClusters is an int\n"
"  maxIterations is an int\n"
"  subSample is an int\n"
"  ignoreZeros is a bool\n"
"  degreeofChange is a float\n"
"  initMethod is one of INITCLUSTER_* values\n"},

    {"ISODataClustering", ImageCalc_ISODataClustering, METH_VARARGS,
"Performs ISO Data Clustering.\n"
"call signature: imagecalc.ISODataClustering(inputImage, outputMatrix, numClusters, maxIterations, subSample, ignoreZeros, degreeOfChange, initMethod, minDistBetweenClusters, minNumFeatures, maxStdDev, minNumClusters, startIteration, endIteration)\n"
"where:\n"
"  inputImage is a string\n"
"  outputMatrix is a string\n"
"  numClusters is an int\n"
"  maxIterations is an int\n"
"  subSample is an int\n"
"  ignoreZeros is a bool\n"
"  initMethod is one of INITCLUSTER_* values\n"
"  minDistBetweenClusters is a float\n"
"  minNumFeatures is an int\n"
"  maxStdDev is a float\n"
"  minNumClusters is an int\n"
"  startIteration is an int\n"
"  endIteration is an int\n"
},

    {NULL}        /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3

static int ImageCalc_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int ImageCalc_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_imagecalc",
        NULL,
        sizeof(struct ImageCalcState),
        ImageCalcMethods,
        NULL,
        ImageCalc_traverse,
        ImageCalc_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__imagecalc(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_imagecalc(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_imagecalc", ImageCalcMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ImageCalcState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_imagecalc.error", NULL, NULL);
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
