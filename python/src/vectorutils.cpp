/*
 *  vectorutils.cpp
 *  RSGIS_LIB
 *
 *  Created by Dan Clewley on 08/11/2013.
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
#include "cmds/RSGISCmdVectorUtils.h"

/* An exception object for this module */
/* created in the init function */
struct VectorUtilsState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct VectorUtilsState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct VectorUtilsState _state;
#endif

static void FreePythonObjects(std::vector<PyObject*> toFree) {
    std::vector<PyObject*>::iterator iter;
    for(iter = toFree.begin(); iter != toFree.end(); ++iter) {
        Py_XDECREF(*iter);
    }
}

static PyObject *VectorUtils_GenerateConvexHullsGroups(PyObject *self, PyObject *args)
{
    const char *pszInputFile, *pszOutputVector, *pszOutVecProj;
    int force, eastingsColIdx, northingsColIdx, attributeColIdx;
    if( !PyArg_ParseTuple(args, "sssiiii:generateConvexHullsGroups", &pszInputFile, &pszOutputVector, &pszOutVecProj, 
                                &force, &eastingsColIdx, &northingsColIdx, &attributeColIdx))
        return NULL;

    try
    {
        rsgis::cmds::executeGenerateConvexHullsGroups(pszInputFile, pszOutputVector, pszOutVecProj, force, 
                eastingsColIdx, northingsColIdx, attributeColIdx);
     
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *VectorUtils_RemoveAttributes(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszOutputVector;
    int force = false;
    if( !PyArg_ParseTuple(args, "ss|i:removeattributes", &pszInputVector, &pszOutputVector, &force))
        return NULL;

    try
    {
        rsgis::cmds::executeRemoveAttributes(pszInputVector, pszOutputVector, force);
     
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *VectorUtils_PrintPolyGeom(PyObject *self, PyObject *args)
{
    const char *pszInputVector;
    if( !PyArg_ParseTuple(args, "s:printpolygeom", &pszInputVector))
        return NULL;

    try
    {
        rsgis::cmds::executePrintPolyGeom(pszInputVector);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *VectorUtils_BufferVector(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszOutputVector;
    float bufferDist;
    int force = false;
    if( !PyArg_ParseTuple(args, "ssf|i:buffervector", &pszInputVector, &pszOutputVector, &bufferDist, &force))
        return NULL;

    try
    {
        rsgis::cmds::executeBufferVector(pszInputVector, pszOutputVector, bufferDist, force);
     
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}




// Our list of functions in this module
static PyMethodDef VectorUtilsMethods[] = {
    {"generateConvexHullsGroups", VectorUtils_GenerateConvexHullsGroups, METH_VARARGS, 
"vectorutils.generateConvexHullsGroups(inputfile, outputvector, outVecProj, force, eastingsColIdx, northingsColIdx, attributeColIdx))\n"
"A command to produce convex hulls for groups of (X, Y, Attribute) point locations.\n\n"
"Where:\n"
"\n"
"* inputfile is a string containing the name of the input file\n"
"* outputvector is a string containing the name of the output vector\n"
"* outVecProj is a string specifying the projection of the output vector\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"* eastingsColIdx an integer specifying the easting column in the input text file \n"
"* northingsColIdx an integer specifying the northing column in the input text file \n"
"* attributeColIdx an integer specifying the attribute column in the input text file \n"
"\n"},

    {"removeattributes", VectorUtils_RemoveAttributes, METH_VARARGS, 
"vectorutils.removeattributes(inputvector, outputvector, force))\n"
"A command to copy the geometry, dropping attributes.\n\n"
"Where:\n"
"\n"
"* inputvector is a string containing the name of the input vector\n"
"* outputvector is a string containing the name of the output vector\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"Example::\n"
"\n"
"  inputVector = './Vectors/injune_p142_stem_locations.shp'\n"
"  outputVector = './TestOutputs/injune_p142_stem_locations_noatts.shp'\n"
"  vectorutils.removeattributes(inputVector, outputVector, True)\n"
"\n"},

    {"buffervector", VectorUtils_BufferVector, METH_VARARGS, 
"vectorutils.buffervector(inputvector, outputvector, bufferDist, force))\n"
"A command to buffer a vector by a specified distance.\n\n"
"Where:\n"
"\n"
"* inputvector is a string containing the name of the input vector\n"
"* outputvector is a string containing the name of the output vector\n"
"* bufferDist is a float specifying the distance of the buffer, in map units.\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"Example::\n"
"\n"
"  inputVector = './Vectors/injune_p142_stem_locations.shp'\n"
"  outputVector = './TestOutputs/injune_p142_stem_locations_1mbuffer.shp'\n"
"  bufferDist = 1\n"
"  vectorutils.buffervector(inputVector, outputVector, bufferDist, True)\n"
"\n"},

    {"printpolygeom", VectorUtils_PrintPolyGeom, METH_VARARGS, 
"vectorutils.printpolygeom(inputvector))\n"
"A command to print the polygon geometries (to the console) of the inputted shapefile\n"
"Where:\n"
"\n"
"* inputvector is a string containing the name of the input vector\n"
"Example::\n"
"\n"
"   inputVector = './Vectors/injune_p142_psu_utm.shp'\n"
"   vectorutils.printpolygeom(inputVector)\n"
"\n"},

    {NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int VectorUtils_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int VectorUtils_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_vectorutils",
        NULL,
        sizeof(struct VectorUtilsState),
        VectorUtilsMethods,
        NULL,
        VectorUtils_traverse,
        VectorUtils_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__vectorutils(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_vectorutils(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_vectorutils", VectorUtilsMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct VectorUtilsState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_vectorutils.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
