/*
 *  classification.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/11/2013.
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
#include "cmds/RSGISCmdHistoCube.h"
#include <vector>

/* An exception object for this module */
/* created in the init function */
struct HistoCubeState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct HistoCubeState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct HistoCubeState _state;
#endif

// Helper function to extract python sequence to array of strings
static std::string *ExtractStringArrayFromSequence(PyObject *sequence, int *nElements)
{
    Py_ssize_t nFields = PySequence_Size(sequence);
    *nElements = nFields;
    std::string *stringsArray = new std::string[nFields];

    for(int i = 0; i < nFields; ++i) {
        PyObject *stringObj = PySequence_GetItem(sequence, i);

        if(!RSGISPY_CHECK_STRING(stringObj))
        {
            PyErr_SetString(GETSTATE(sequence)->error, "Fields must be strings");
            Py_DECREF(stringObj);
            return stringsArray;
        }

        stringsArray[i] = RSGISPY_STRING_EXTRACT(stringObj);
        Py_DECREF(stringObj);
    }

    return stringsArray;
}

static PyObject *HistoCube_CreateEmptyHistCube(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszCubeFile;
    unsigned long numOfFeats = 0;

    static char *kwlist[] = {"filename", "numOfFeats", NULL};
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sk:createEmptyHistoCube", kwlist, &pszCubeFile, &numOfFeats))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeCreateEmptyHistoCube(std::string(pszCubeFile), numOfFeats);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}





// Our list of functions in this module
static PyMethodDef HistoCubeMethods[] = {
{"createEmptyHistoCube", (PyCFunction)HistoCube_CreateEmptyHistCube, METH_VARARGS | METH_KEYWORDS,
"histocube.createEmptyHistoCube(filename=string, numOfFeats=unsigned long)\n"
"Create an empty histogram cube file ready for populating.\n"
"\n"
"Where:\n"
"\n"
"* filename - is the file path and name for the histogram cube file.\n"
"* numOfFeats - is the number of features within the cube - this is defined globally within the file.\n"
"\n"
"Example::\n"
"\n"
"import rsgislib.histocube\n"
"rsgislib.histocube.createEmptyHistoCube('HistoCubeTest.hcf', 10000)\n"
"\n"
},

{NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int HistoCube_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int HistoCube_clear(PyObject *m)
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_histocube",
        NULL,
        sizeof(struct HistoCubeState),
        HistoCubeMethods,
        NULL,
        HistoCube_traverse,
        HistoCube_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__histocube(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_histocube(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_histcube", HistoCubeMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct HistoCubeState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_histocube.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
