/*
 *  secmentation.cpp
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

static PyObject *Segmentation_labelPixelsFromClusterCentres(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImage, *pszClusterCentres, *pszGDALFormat;
    int ignoreZeros;
    if( !PyArg_ParseTuple(args, "sssis:labelPixelsFromClusterCentres", &pszInputImage, &pszOutputImage, 
                                &pszClusterCentres, &ignoreZeros, &pszGDALFormat ))
        return NULL;

    try
    {
        rsgis::cmds::executeLabelPixelsFromClusterCentres(pszInputImage, pszOutputImage, pszClusterCentres,
                        ignoreZeros, pszGDALFormat );
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *Segmentation_eliminateSinglePixels(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImage, *pszClumpsImage, *pszGDALFormat, *pszTempImage;
    int processInMemory, ignoreZeros;
    if( !PyArg_ParseTuple(args, "sssssnn:eliminateSinglePixels", &pszInputImage, &pszClumpsImage, 
                        &pszOutputImage, &pszTempImage, &pszGDALFormat, &processInMemory, &ignoreZeros ))
        return NULL;

    try
    {
        rsgis::cmds::executeEliminateSinglePixels(pszInputImage, pszClumpsImage, pszOutputImage, pszTempImage,
                    pszGDALFormat, processInMemory, ignoreZeros);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *Segmentation_clump(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImage, *pszGDALFormat;
    int processInMemory;
    bool nodataprovided;
    float fnodata;
    PyObject *pNoData; //could be none or a number
    if( !PyArg_ParseTuple(args, "sssnO:clump", &pszInputImage, &pszOutputImage, &pszGDALFormat, 
                    &processInMemory, &pNoData))
        return NULL;

    if( pNoData == Py_None )
    {
        nodataprovided = false;
        fnodata = 0;
    }
    else
    {
        // convert to a float if needed
        PyObject *pFloatNoData = PyNumber_Float(pNoData);
        if( pFloatNoData == NULL )
        {
            PyErr_SetString(GETSTATE(self)->error, "nodata parameter must be None or a valid number\n");
            return NULL;
        }

        nodataprovided = true;
        fnodata = PyFloat_AsDouble(pFloatNoData);
        Py_DECREF(pFloatNoData);
    }

    try
    {
        rsgis::cmds::executeClump(pszInputImage, pszOutputImage, pszGDALFormat, 
                                processInMemory, nodataprovided, fnodata);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef SegmentationMethods[] = {
    {"labelPixelsFromClusterCentres", Segmentation_labelPixelsFromClusterCentres, METH_VARARGS, 
"Does stuff.\n"
"call signature: segmentation.labelPixelsFromClusterCentres(inputimage, outputimage, clustercenters, ignorezeros, gdalformat)\n"
"where:\n"
"  inputimage is a string containing the name of the input file\n"
"  outputimage is a string containing the name of the output file\n"
"  clustercentres is a string containing the name of the cluster centre file\n"
"  ignore zeros is a bool\n"
"  gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"},

    {"eliminateSinglePixels", Segmentation_eliminateSinglePixels, METH_VARARGS, 
"Does other stuff\n"
"call signature: segmentation.eliminateSinglePixels(inputimage, clumpsimage, outputimage, tempfile, gdalformat, processinmemory, ignorezeros)\n"
"where:\n"
"  inputimage is a string containing the name of the input file\n"
"  clumpsimage is a string containing the name of the clump file\n"
"  outputimage is a string containing the name of the output file\n"
"  tempfile is a string containing the name of the temporary file to use\n"
"  gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  processinmemory is a bool\n"
"  ignore zeros is a bool\n"},

    {"clump", Segmentation_clump, METH_VARARGS,
"clump\n"
"call signature: segmentation.clump(inputimage, outputimage, gdalformat, processinmemory, nodata)\n"
"where:\n"
"  inputimage is a string containing the name of the input file\n"
"  outputimage is a string containing the name of the output file\n"
"  gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  processinmemory is a bool\n"
"  nodata is None or float\n"},

    {NULL}        /* Sentinel */
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
        NULL,
        sizeof(struct SegmentationState),
        SegmentationMethods,
        NULL,
        Segmentation_traverse,
        Segmentation_clear,
        NULL
};

#define INITERROR return NULL

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
    if( pModule == NULL )
        INITERROR;

    struct SegmentationState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_segmentation.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}

