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

    try {
        rsgis::cmds::executeCopyGDALATTColumns(std::string(inputImage), std::string(clumpImage), fields);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


executeSpatialLocation(std::string inputImage, std::string clumpsImage, std::string eastingsField, std::string northingsField)throw(RSGISCmdException);


static PyObject *RasterGIS_CopySpatialLocation(PyObject *self, PyObject *args) {
    const char *clumpsImage, *inputImage, *eastingsField, *northingsField;

    if(!PyArg_ParseTuple(args, "ssss:copySpatialLocation", &inputImage, &clumpsImage, &eastingsField, &northingsField))
        return NULL;

    try {
        rsgis::cmds::executeCopySpatialLocation(std::string(clumpsImage), std::string(inputImage), std::string(eastingsField), std::string(northingsField));
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

   {"copySpatialLocation", RasterGIS_CopySpatialLocation, METH_VARARGS,
"Adds spatial location columns to the attribute table\n"
"call signature: rastergis.copySpatialLocation(inputImage, clumpsImage, eastingsField, northingsField)\n"
"where:\n"
"  inputImage is a string containing the name of the input image file\n"
"  clumpsImage is a string containing the name of the input clump file\n"
"  eastingsField is a string containing the name of the eastings field\n"
"  northingsField is a string containing the name of the northings field\n"
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
