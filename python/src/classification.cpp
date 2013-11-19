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
#include "cmds/RSGISCmdClassification.h"
#include <vector>

/* An exception object for this module */
/* created in the init function */
struct ClassificationState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ClassificationState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ClassificationState _state;
#endif

// Helper function to extract python sequence to array of strings
static std::string *ExtractStringArrayFromSequence(PyObject *sequence, int *nElements) {
    Py_ssize_t nFields = PySequence_Size(sequence);
    *nElements = nFields;
    std::string *stringsArray = new std::string[nFields];

    for(int i = 0; i < nFields; ++i) {
        PyObject *stringObj = PySequence_GetItem(sequence, i);

        if(!RSGISPY_CHECK_STRING(stringObj)) {
            PyErr_SetString(GETSTATE(sequence)->error, "Fields must be strings");
            Py_DECREF(stringObj);
            return stringsArray;
        }

        stringsArray[i] = RSGISPY_STRING_EXTRACT(stringObj);
        Py_DECREF(stringObj);
    }

    return stringsArray;
}

static PyObject *Classification_CollapseClasses(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat, *pszClassesColumn;
    if( !PyArg_ParseTuple(args, "ssss:collapseClasses", &pszInputImage, &pszOutputFile, &pszGDALFormat, &pszClassesColumn))
        return NULL;

    try
    {
        rsgis::cmds::executeCollapseRAT2Class(std::string(pszInputImage), std::string(pszOutputFile), std::string(pszGDALFormat), std::string(pszClassesColumn));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *Classification_Colour3Bands(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    if( !PyArg_ParseTuple(args, "sss:colour3Band", &pszInputImage, &pszOutputFile, &pszGDALFormat))
        return NULL;
    
    try
    {
        rsgis::cmds::executeGenerate3BandFromColourTable(std::string(pszInputImage), std::string(pszOutputFile), std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef ClassificationMethods[] = {
{"collapseClasses", Classification_CollapseClasses, METH_VARARGS,
"classification.collapseClasses(inputimage, outputimage, gdalformat, classColumn)\n"
"Collapses an attribute table with a large number of classified clumps (segments) to\n"
"a attribute table with a single row per class (i.e. a classification rather than segmentation.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input file with attribute table.\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* gdalformat is a string with the output image format for the GDAL driver.\n"
"* classColumn is a string with the name of the column with the class names\n"},

{"colour3bands", Classification_Colour3Bands, METH_VARARGS,
"classification.colour3bands(inputimage, outputimage, gdalformat)\n"
"Generates a 3 band colour image from the colour table in the input file.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input file with attribute table.\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* gdalformat is a string with the output image format for the GDAL driver.\n"},

    {NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int Classification_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int Classification_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_classification",
        NULL,
        sizeof(struct ClassificationState),
        ClassificationMethods,
        NULL,
        Classification_traverse,
        Classification_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__classification(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_classification(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_classification", ClassificationMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ClassificationState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_classification.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
