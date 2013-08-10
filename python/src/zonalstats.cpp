/*
 *  zonalstats.cpp
 *  RSGIS_LIB
 *
 *  Created by Dan Clewley on 09/08/2013.
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
#include "cmds/RSGISCmdZonalStats.h"

/* An exception object for this module */
/* created in the init function */
struct ZonalStatsState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ZonalStatsState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ZonalStatsState _state;
#endif

static PyObject *ZonalStats_PointValue2SHP(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputVector, *pszOutputVector;
    int force, useBandNames;
    if( !PyArg_ParseTuple(args, "sssii:stretchImage", &pszInputImage, &pszInputVector, &pszOutputVector, 
                                &force, &useBandNames))
        return NULL;

    try
    {
        rsgis::cmds::executePointValue2SHP(pszInputImage, pszInputVector, pszOutputVector, force, useBandNames);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ZonalStats_PointValue2TXT(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputVector, *pszOutputTxt;
    int useBandNames;
    if( !PyArg_ParseTuple(args, "sssi:stretchImage", &pszInputImage, &pszInputVector, &pszOutputTxt, 
                                &useBandNames))
        return NULL;

    try
    {
        rsgis::cmds::executePointValue2TXT(pszInputImage, pszInputVector, pszOutputTxt, useBandNames);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef ZonalStatsMethods[] = {
    {"pointValue2SHP", ZonalStats_PointValue2SHP, METH_VARARGS, 
"Stretch\n"
"call signature: zonalstats.pointValue2SHP(inputimage, inputvector, outputvector, force, useBandNames)\n"
"where:\n"
"  * inputimage is a string containing the name of the input image\n"
"  * inputvector is a string containing the name of the input vector\n"
"  * outputvector is a string containing the name of the output vector\n"
"  * force is a bool, specifying whether to force removal of the output vector if it exists\n"
"  * useBandNames is a bool, specifying whether to use the band names of the input dataset in the output file (if not uses b1, b2, etc.,\n"},

    {"pointValue2TXT", ZonalStats_PointValue2TXT, METH_VARARGS, 
"Stretch\n"
"call signature: zonalstats.pointValue2TXT(inputimage, inputvector, outputtxt, useBandNames)\n"
"where:\n"
"  * inputimage is a string containing the name of the input image\n"
"  * inputvector is a string containing the name of the input vector\n"
"  * outputtxt is a string containing the name of the output text file\n"
"  * useBandNames is a bool, specifying whether to use the band names of the input dataset in the output file (if not uses b1, b2, etc.,\n"},

    {NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int ZonalStats_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int ZonalStats_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_zonalstats",
        NULL,
        sizeof(struct ZonalStatsState),
        ZonalStatsMethods,
        NULL,
        ZonalStats_traverse,
        ZonalStats_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__zonalstats(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_zonalstats(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_zonalstats", ZonalStatsMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ZonalStatsState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_zonalstats.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
