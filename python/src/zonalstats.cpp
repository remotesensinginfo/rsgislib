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

/*
static void FreePythonObjects(std::vector<PyObject*> toFree) {
    std::vector<PyObject*>::iterator iter;
    for(iter = toFree.begin(); iter != toFree.end(); ++iter) {
        Py_XDECREF(*iter);
    }
}
*/

static PyObject *ZonalStats_ImageZoneToHDF(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("vec_file"),
                             RSGIS_PY_C_TEXT("vec_lyr"), RSGIS_PY_C_TEXT("out_h5_file"),
                             RSGIS_PY_C_TEXT("no_prj_warn"), RSGIS_PY_C_TEXT("pxl_in_poly_method"), nullptr};
    const char *pszInputImage, *pszInputVector, *pszInputVecLyr, *pszOutputHDF;
    int pixelInPolyMethod = 1;
    int noProjWarning = false;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssss|ii:image_zone_to_hdf", kwlist, &pszInputImage, &pszInputVector, &pszInputVecLyr, &pszOutputHDF, &noProjWarning, &pixelInPolyMethod))
        return nullptr;
    try
    {
        rsgis::cmds::executeZonesImage2HDF5(std::string(pszInputImage), std::string(pszInputVector), std::string(pszInputVecLyr),
                                            std::string(pszOutputHDF), (bool)noProjWarning, pixelInPolyMethod);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef ZonalStatsMethods[] = {

{"image_zone_to_hdf", (PyCFunction)ZonalStats_ImageZoneToHDF, METH_VARARGS | METH_KEYWORDS,
"rsgislib.zonalstats.image_zone_to_hdf(input_img, vec_file, vec_lyr, out_h5_file, no_prj_warn=False, pxl_in_poly_method=METHOD_POLYCONTAINSPIXELCENTER)\n"
"Extract the all the pixel values for regions to a HDF5 file (1 column for each image band).\n\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input image.\n"
":param vec_file: is a string containing the input vector file path.\n"
":param vec_lyr: is a string containing the name of the input vector layer.\n"
":param out_h5_file: is a string containing name of the output HDF file.\n"
":param no_prj_warn: is a bool, specifying whether to skip printing a warning if the vector and image have a different projections.\n"
":param pxl_in_poly_method: is the method for determining if a pixel is included with a polygon of type rsgislib.zonalstats.METHOD_*.\n"
"\n"
"Example::\n"
"\n"
"    from rsgislib import zonalstats\n"
"    inputimage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"    inputvector = './Vectors/injune_p142_crowns_utm.shp'\n"
"    outputHDF = './TestOutputs/InjuneP142.hdf'\n"
"    zonalstats.image_zone_to_hdf(inputimage, inputvector, outputHDF, True, zonalstats.METHOD_POLYCONTAINSPIXELCENTER)\n"
"\n"},

    {nullptr}        /* Sentinel */
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
        nullptr,
        sizeof(struct ZonalStatsState),
        ZonalStatsMethods,
        nullptr,
        ZonalStats_traverse,
        ZonalStats_clear,
        nullptr
};

#define INITERROR return nullptr

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
    if( pModule == nullptr )
        INITERROR;

    struct ZonalStatsState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_zonalstats.error", nullptr, nullptr);
    if( state->error == nullptr )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
