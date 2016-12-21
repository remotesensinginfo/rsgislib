/*
 *  elevation.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/11/2013.
 *  Copyright 2014 RSGISLib.
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
#include "cmds/RSGISCmdElevationTools.h"
#include <vector>

/* An exception object for this module */
/* created in the init function */
struct ElevationState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ElevationState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ElevationState _state;
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

static PyObject *Elevation_calcSlope(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat, *pszOutUnit;
    if( !PyArg_ParseTuple(args, "ssss:slope", &pszInputImage, &pszOutputFile, &pszOutUnit, &pszGDALFormat))
        return NULL;

    try
    {
        rsgis::cmds::RSGISAngleMeasure outAngleUnit;
        std::string angUnit = std::string(pszOutUnit);
        if(angUnit == "degrees")
        {
            outAngleUnit = rsgis::cmds::rsgis_degrees;
        }
        else if(angUnit == "radians")
        {
            outAngleUnit = rsgis::cmds::rsgis_radians;
        }
        else
        {
            throw rsgis::cmds::RSGISCmdException("The unit option needs to be specified as either \'degrees\' or \'radians\'.");
        }
        
        rsgis::cmds::executeCalcSlope(std::string(pszInputImage), std::string(pszOutputFile), outAngleUnit, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *Elevation_calcAspect(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    if( !PyArg_ParseTuple(args, "sss:aspect", &pszInputImage, &pszOutputFile, &pszGDALFormat))
        return NULL;
    
    try
    {
        rsgis::cmds::executeCalcAspect(std::string(pszInputImage), std::string(pszOutputFile), std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_catAspect(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    if( !PyArg_ParseTuple(args, "sss:catagoriseAspect", &pszInputImage, &pszOutputFile, &pszGDALFormat))
        return NULL;
    
    try
    {
        rsgis::cmds::executeCatagoriseAspect(std::string(pszInputImage), std::string(pszOutputFile), std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_calcHillshade(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    float azimuth, zenith = 0.0;
    if( !PyArg_ParseTuple(args, "ssffs:hillshade", &pszInputImage, &pszOutputFile, &azimuth, &zenith, &pszGDALFormat))
        return NULL;
    
    try
    {
        rsgis::cmds::executeCalcHillshade(std::string(pszInputImage), std::string(pszOutputFile), azimuth, zenith, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_calcShadowMask(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    float azimuth, zenith, maxHeight = 0.0;
    if( !PyArg_ParseTuple(args, "ssfffs:shadowmask", &pszInputImage, &pszOutputFile, &azimuth, &zenith, &maxHeight, &pszGDALFormat))
        return NULL;
    
    try
    {
        rsgis::cmds::executeCalcShadowMask(std::string(pszInputImage), std::string(pszOutputFile), azimuth, zenith, maxHeight, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_calcLocalIncidenceAngle(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    float azimuth, zenith = 0.0;
    if( !PyArg_ParseTuple(args, "ssffs:localIncidenceAngle", &pszInputImage, &pszOutputFile, &azimuth, &zenith, &pszGDALFormat))
        return NULL;
    
    try
    {
        rsgis::cmds::executeCalcLocalIncidenceAngle(std::string(pszInputImage), std::string(pszOutputFile), azimuth, zenith, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_calcLocalExistanceAngle(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    float azimuth, zenith = 0.0;
    if( !PyArg_ParseTuple(args, "ssffs:localExistanceAngle", &pszInputImage, &pszOutputFile, &azimuth, &zenith, &pszGDALFormat))
        return NULL;
    
    try
    {
        rsgis::cmds::executeCalcLocalExitanceAngle(std::string(pszInputImage), std::string(pszOutputFile), azimuth, zenith, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_dtmAspectMedianFilter(PyObject *self, PyObject *args)
{
    const char *pszInputDTMImage, *pszInputAspectImage, *pszOutputFile, *pszGDALFormat;
    float aspectRange = 0.0;
    int winHSize = 0;
    if( !PyArg_ParseTuple(args, "sssfis:dtmAspectMedianFilter", &pszInputDTMImage, &pszInputAspectImage, &pszOutputFile, &aspectRange, &winHSize, &pszGDALFormat))
        return NULL;
    
    try
    {
        rsgis::cmds::executeDTMAspectMedianFilter(std::string(pszInputDTMImage), std::string(pszInputAspectImage), std::string(pszOutputFile), aspectRange, winHSize, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_fillDEMSoilleGratin1994(PyObject *self, PyObject *args)
{
    const char *pszInputDTMImage, *pszValidMaskImage, *pszOutputFile, *pszGDALFormat;

    if( !PyArg_ParseTuple(args, "ssss:fillDEMSoilleGratin1994", &pszInputDTMImage, &pszValidMaskImage, &pszOutputFile, &pszGDALFormat))
        return NULL;
    
    try
    {
        rsgis::cmds::executeDEMFillSoilleGratin1994(std::string(pszInputDTMImage), std::string(pszValidMaskImage), std::string(pszOutputFile), std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef ElevationMethods[] = {
{"slope", Elevation_calcSlope, METH_VARARGS,
"rsgislib.elevation.slope(inputImage, outputImage, outAngleUnit, gdalformat)\n"
"Calculates a slope layer given an input elevation model.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input DEM file.\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* outAngleUnit is a string specifying the output unit ('degrees' or 'radians').\n"
"* gdalformat is a string with the output image format for the GDAL driver.\n"},

{"aspect", Elevation_calcAspect, METH_VARARGS,
"rsgislib.elevation.aspect(inputImage, outputImage, gdalformat)\n"
"Calculates a aspect layer given an input elevation model\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input DEM file.\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* gdalformat is a string with the output image format for the GDAL driver.\n"},
    
{"catagoriseAspect", Elevation_catAspect, METH_VARARGS,
"rsgislib.elevation.catagoriseAspect(inputImage, outputImage, gdalformat)\n"
"Creates an aspect layer which is categorised into 8 catergories from 0-45, \n"
"45-90, 90-135, 135-180, 180-225, 225-270, 270-315 and 315-360.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input aspect (in degrees) file.\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* gdalformat is a string with the output image format for the GDAL driver.\n"},
    
{"hillshade", Elevation_calcHillshade, METH_VARARGS,
"rsgislib.elevation.hillshade(inputImage, outputImage, solarAzimuth, solarZenith, gdalformat)\n"
"Calculates a hillshade layer given an input elevation model\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input DEM file.\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* solarAzimuth is a float with the solar azimuth in degrees (Good value is 315).\n"
"* solarZenith is a float with the solar zenith in degrees (Good value is 45).\n"
"* gdalformat is a string with the output image format for the GDAL driver.\n"},
    
{"shadowmask", Elevation_calcShadowMask, METH_VARARGS,
"rsgislib.elevation.shadowmask(inputImage, outputImage, solarAzimuth, solarZenith, maxHeight, gdalformat)\n"
"Calculates a shadow mask given an input elevation model\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input DEM file.\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* solarAzimuth is a float with the solar azimuth in degrees.\n"
"* solarZenith is a float with the solar zenith in degrees.\n"
"* maxHeight is a float with the maximum height for the ray tracing (should be above the maximum elevation within the scene)."
"* gdalformat is a string with the output image format for the GDAL driver.\n"},
    
    
{"localIncidenceAngle", Elevation_calcLocalIncidenceAngle, METH_VARARGS,
"rsgislib.elevation.localIncidenceAngle(inputImage, outputImage, solarAzimuth, solarZenith, gdalformat)\n"
"Calculates a local solar incidence angle layer given an input elevation model\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input DEM file.\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* solarAzimuth is a float with the solar azimuth in degrees.\n"
"* solarZenith is a float with the solar zenith in degrees.\n"
"* gdalformat is a string with the output image format for the GDAL driver.\n"},
    
{"localExistanceAngle", Elevation_calcLocalExistanceAngle, METH_VARARGS,
"rsgislib.elevation.localExistanceAngle(inputImage, outputImage, azimuth, zenith, gdalformat)\n"
"Calculates a local solar existance angle layer given an input elevation model\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input DEM file.\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* azimuth is a float with the solar azimuth in degrees.\n"
"* zenith is a float with the solar zenith in degrees.\n"
"* gdalformat is a string with the output image format for the GDAL driver.\n"},
    
{"dtmAspectMedianFilter", Elevation_dtmAspectMedianFilter, METH_VARARGS,
"rsgislib.elevation.dtmAspectMedianFilter(inputDTMImage, inputAspectImage, outputImage, aspectRange, winHSize, gdalformat)\n"
"Filter the DTM for noise using a median filter with an aspect restriction (i.e., only pixels"
" within the aspect range of the centre pixel will be used within the median filter).\n"
"\n"
"Where:\n"
"\n"
"* inputDTMImage is a string containing the name and path of the input DTM file.\n"
"* inputAspectImage is a string containing the name and path of the input Aspect file (in degrees).\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* aspectRange is a float with the range of from the centre pixel in degrees.\n"
"* winHSize is an integer for half the window size.\n"
"* gdalformat is a string with the output image format for the GDAL driver.\n"},

{"fillDEMSoilleGratin1994", Elevation_fillDEMSoilleGratin1994, METH_VARARGS,
"rsgislib.elevation.fillDEMSoilleGratin1994(inputDEMImage, validMaskImage, outputImage, gdalformat)\n"
"Filter the local minima in a DEM using the Soille and Gratin 1994 algorithm.\n\n"
"Soille, P., and Gratin, C. (1994). An efficient algorithm for drainage network\n"
"extraction on DEMs. J. Visual Communication and Image Representation. 5(2). 181-189.\n"
"\n"
"Where:\n"
"\n"
"* inputDEMImage is a string containing the name and path of the input DEM file.\n"
"* validMaskImage is a string containing the name and path to a binary image specifying the valid data region (1 == valid)\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* gdalformat is a string with the output image format for the GDAL driver.\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib.elevation\n"
"   inputDEMImage = 'DEM.kea'\n"
"   validMaskImage = 'ValidRegionMask.kea'\n"
"   outFilledImage = 'DEM_filled.kea'\n"
"   rsgislib.elevation.fillDEMSoilleGratin1994(inputDEMImage, validMaskImage, outFilledImage, 'KEA')\n"
"\n"
},
    
    {NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int Elevation_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int Elevation_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_elevation",
        NULL,
        sizeof(struct ElevationState),
        ElevationMethods,
        NULL,
        Elevation_traverse,
        Elevation_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__elevation(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_elevation(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_elevation", ElevationMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ElevationState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_elevation.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
