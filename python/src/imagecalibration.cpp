/*
 *  imagecalibration.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/07/2013.
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
#include "cmds/RSGISCmdImageCalibration.h"

/* An exception object for this module */
/* created in the init function */
struct ImageCalibrationState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ImageCalibrationState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ImageCalibrationState _state;
#endif

static PyObject *ImageCalibration_landsat2Radiance(PyObject *self, PyObject *args)
{
    const char *pszOutputFile, *pszGDALFormat;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTuple(args, "ssO:landsat2Radiance", &pszOutputFile, &pszGDALFormat, &pBandDefnObj))
    {
        return NULL;
    }

    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return NULL;
    }

    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    std::vector<rsgis::cmds::CmdsLandsatRadianceGainsOffsets> landsatRadGainOffs;
    landsatRadGainOffs.reserve(nBandDefns);
    
    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);

        PyObject *pBandName = PyObject_GetAttrString(o, "bandName");
        if( ( pBandName == NULL ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'bandName\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            return NULL;
        }

        PyObject *pFileName = PyObject_GetAttrString(o, "fileName");
        if( ( pFileName == NULL ) || ( pFileName == Py_None ) || !RSGISPY_CHECK_STRING(pFileName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'fileName\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pFileName);
            Py_DECREF(o);
            return NULL;
        }

        PyObject *pBandIndex = PyObject_GetAttrString(o, "bandIndex");
        if( ( pBandIndex == NULL ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find integer attribute \'bandIndex\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pLMin = PyObject_GetAttrString(o, "lMin");
        if( ( pLMin == NULL ) || ( pLMin == Py_None ) || !RSGISPY_CHECK_FLOAT(pLMin) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'lMin\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pLMin);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pLMax = PyObject_GetAttrString(o, "lMax");
        if( ( pLMax == NULL ) || ( pLMax == Py_None ) || !RSGISPY_CHECK_FLOAT(pLMax) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'lMax\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pLMin);
            Py_XDECREF(pLMax);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pQCalMin = PyObject_GetAttrString(o, "qCalMin");
        if( ( pQCalMin == NULL ) || ( pQCalMin == Py_None ) || !RSGISPY_CHECK_FLOAT(pQCalMin) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'qCalMin\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pLMin);
            Py_XDECREF(pLMax);
            Py_XDECREF(pQCalMin);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pQCalMax = PyObject_GetAttrString(o, "qCalMax");
        if( ( pQCalMax == NULL ) || ( pQCalMax == Py_None ) || !RSGISPY_CHECK_FLOAT(pQCalMax) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'qCalMax\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pLMin);
            Py_XDECREF(pLMax);
            Py_XDECREF(pQCalMin);
            Py_XDECREF(pQCalMax);
            Py_DECREF(o);
            return NULL;
        }
                
        rsgis::cmds::CmdsLandsatRadianceGainsOffsets radVals;
        radVals.bandName = RSGISPY_STRING_EXTRACT(pBandName);
        radVals.imagePath = RSGISPY_STRING_EXTRACT(pFileName);
        radVals.band = RSGISPY_INT_EXTRACT(pBandIndex);
        radVals.lMin = RSGISPY_FLOAT_EXTRACT(pLMin);
        radVals.lMax = RSGISPY_FLOAT_EXTRACT(pLMax);
        radVals.qCalMin = RSGISPY_FLOAT_EXTRACT(pQCalMin);
        radVals.qCalMax = RSGISPY_FLOAT_EXTRACT(pQCalMax);
        
        landsatRadGainOffs.push_back(radVals);

        Py_DECREF(pBandName);
        Py_DECREF(pFileName);
        Py_DECREF(pBandIndex);
        Py_XDECREF(pLMin);
        Py_XDECREF(pLMax);
        Py_XDECREF(pQCalMin);
        Py_XDECREF(pQCalMax);
        Py_DECREF(o);
    }
    
    try
    {
        rsgis::cmds::executeConvertLandsat2Radiance(pszOutputFile, pszGDALFormat, landsatRadGainOffs);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_landsat2RadianceMultiAdd(PyObject *self, PyObject *args)
{
    const char *pszOutputFile, *pszGDALFormat;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTuple(args, "ssO:landsat2RadianceMultiAdd", &pszOutputFile, &pszGDALFormat, &pBandDefnObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    std::vector<rsgis::cmds::CmdsLandsatRadianceGainsOffsetsMultiAdd> landsatRadGainOffs;
    landsatRadGainOffs.reserve(nBandDefns);
    
    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);
        
        PyObject *pBandName = PyObject_GetAttrString(o, "bandName");
        if( ( pBandName == NULL ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'bandName\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pFileName = PyObject_GetAttrString(o, "fileName");
        if( ( pFileName == NULL ) || ( pFileName == Py_None ) || !RSGISPY_CHECK_STRING(pFileName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'fileName\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pFileName);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pBandIndex = PyObject_GetAttrString(o, "bandIndex");
        if( ( pBandIndex == NULL ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find integer attribute \'bandIndex\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pAddVal = PyObject_GetAttrString(o, "addVal");
        if( ( pAddVal == NULL ) || ( pAddVal == Py_None ) || !RSGISPY_CHECK_FLOAT(pAddVal) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'addVal\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pAddVal);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pMultiVal = PyObject_GetAttrString(o, "multiVal");
        if( ( pMultiVal == NULL ) || ( pMultiVal == Py_None ) || !RSGISPY_CHECK_FLOAT(pMultiVal) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'multiVal\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pAddVal);
            Py_XDECREF(pMultiVal);
            Py_DECREF(o);
            return NULL;
        }
        
        
        rsgis::cmds::CmdsLandsatRadianceGainsOffsetsMultiAdd radVals;
        radVals.bandName = RSGISPY_STRING_EXTRACT(pBandName);
        radVals.imagePath = RSGISPY_STRING_EXTRACT(pFileName);
        radVals.band = RSGISPY_INT_EXTRACT(pBandIndex);
        radVals.addVal = RSGISPY_FLOAT_EXTRACT(pAddVal);
        radVals.multiVal = RSGISPY_FLOAT_EXTRACT(pMultiVal);
        
        landsatRadGainOffs.push_back(radVals);
        
        Py_DECREF(pBandName);
        Py_DECREF(pFileName);
        Py_DECREF(pBandIndex);
        Py_XDECREF(pAddVal);
        Py_XDECREF(pMultiVal);
        Py_DECREF(o);
    }
    
    try
    {
        rsgis::cmds::executeConvertLandsat2RadianceMultiAdd(pszOutputFile, pszGDALFormat, landsatRadGainOffs);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_Radiance2TOARefl(PyObject *self, PyObject *args)
{
    const char *pszInputFile, *pszOutputFile, *pszGDALFormat;
    int nDataType, year, month, day;
    float scaleFactor, solarZenith;
    PyObject *pSolarIrrObj;
    if( !PyArg_ParseTuple(args, "sssifiiifO:radiance2TOARefl", &pszInputFile, &pszOutputFile, &pszGDALFormat, &nDataType, &scaleFactor, &year, &month, &day, &solarZenith, &pSolarIrrObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pSolarIrrObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nSolarIrrDefns = PySequence_Size(pSolarIrrObj);
    unsigned int numSolarIrrVals = nSolarIrrDefns;
    float *solarIrradiance = new float[numSolarIrrVals];
    
    for( Py_ssize_t n = 0; n < nSolarIrrDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pSolarIrrObj, n);
        
        PyObject *pIrradiance = PyObject_GetAttrString(o, "irradiance");
        if( ( pIrradiance == NULL ) || ( pIrradiance == Py_None ) || !RSGISPY_CHECK_FLOAT(pIrradiance) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'irradiance\'" );
            Py_XDECREF(pIrradiance);
            Py_DECREF(o);
            return NULL;
        }
        
        solarIrradiance[n] = RSGISPY_FLOAT_EXTRACT(pIrradiance);
        
        Py_DECREF(pIrradiance);
        Py_DECREF(o);
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeConvertRadiance2TOARefl(pszInputFile, pszOutputFile, pszGDALFormat, type, scaleFactor, 0, false, year, month, day, (solarZenith*(M_PI/180)), solarIrradiance, numSolarIrrVals);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_Apply6SCoefficentsSingleParam(PyObject *self, PyObject *args)
{
    const char *pszInputFile, *pszOutputFile, *pszGDALFormat;
    int nDataType, useNoDataVal;
    float scaleFactor, noDataVal;
    PyObject *pBandValuesObj;
    if( !PyArg_ParseTuple(args, "sssiffiO:apply6SCoeffSingleParam", &pszInputFile, &pszOutputFile, &pszGDALFormat, &nDataType, &scaleFactor, &noDataVal, &useNoDataVal, &pBandValuesObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pBandValuesObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nBandValDefns = PySequence_Size(pBandValuesObj);
    
    int numValues = nBandValDefns;
    unsigned int *imageBands = new unsigned int[numValues];
    float *aX = new float[numValues];
    float *bX = new float[numValues];
    float *cX = new float[numValues];
    
    
    for( Py_ssize_t n = 0; n < nBandValDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandValuesObj, n);
        
        PyObject *pBand = PyObject_GetAttrString(o, "band");
        if( ( pBand == NULL ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'band\'" );
            Py_XDECREF(pBand);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pAX = PyObject_GetAttrString(o, "aX");
        if( ( pAX == NULL ) || ( pAX == Py_None ) || !RSGISPY_CHECK_FLOAT(pAX) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'aX\'" );
            Py_XDECREF(pBand);
            Py_XDECREF(pAX);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pBX = PyObject_GetAttrString(o, "bX");
        if( ( pBX == NULL ) || ( pBX == Py_None ) || !RSGISPY_CHECK_FLOAT(pBX) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'bX\'" );
            Py_XDECREF(pBand);
            Py_XDECREF(pAX);
            Py_XDECREF(pBX);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pCX = PyObject_GetAttrString(o, "cX");
        if( ( pCX == NULL ) || ( pCX == Py_None ) || !RSGISPY_CHECK_FLOAT(pCX) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'cX\'" );
            Py_XDECREF(pBand);
            Py_XDECREF(pAX);
            Py_XDECREF(pBX);
            Py_XDECREF(pCX);
            Py_DECREF(o);
            return NULL;
        }
        
        imageBands[n] = RSGISPY_INT_EXTRACT(pBand);
        aX[n] = RSGISPY_FLOAT_EXTRACT(pAX);
        bX[n] = RSGISPY_FLOAT_EXTRACT(pBX);
        cX[n] = RSGISPY_FLOAT_EXTRACT(pCX);
        
        Py_DECREF(pBand);
        Py_DECREF(pAX);
        Py_DECREF(pBX);
        Py_DECREF(pCX);
        Py_DECREF(o);
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeRad2SREFSingle6sParams(pszInputFile, pszOutputFile, pszGDALFormat, type, scaleFactor, imageBands, aX, bX, cX, numValues, noDataVal, useNoDataVal);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    delete[] aX;
    delete[] bX;
    delete[] cX;
    delete[] imageBands;
    
    Py_RETURN_NONE;
}



// Our list of functions in this module
static PyMethodDef ImageCalibrationMethods[] = {
    {"landsat2Radiance", ImageCalibration_landsat2Radiance, METH_VARARGS,
"Converts Landsat DN values to at sensor radiance.\n"
"call signature: imagecalibration.landsat2Radiance(outputImage, gdalformat, bandDefnSeq)\n"
"where:\n"
"  outputImage is a string containing the name of the output file\n"
"  gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  bandDefnSeq is a sequence of rsgislib.imagecalibration.CmdsLandsatRadianceGainsOffsets objects that define the inputs\n"
"    Requires:\n"
"      bandName - Name of image band in output file.\n"
"      fileName - input image file.\n"
"      bandIndex - Index (starting from 1) of the band in the image file.\n"
"      lMin - lMin value from Landsat header.\n"
"      lMax - lMax value from Landsat header.\n"
"      qCalMin - qCalMin value from Landsat header.\n"
"      qCalMax - qCalMax value from Landsat header.\n"},
    
    {"landsat2RadianceMultiAdd", ImageCalibration_landsat2RadianceMultiAdd, METH_VARARGS,
        "Converts Landsat DN values to at sensor radiance.\n"
        "call signature: imagecalibration.landsat2RadianceMultiAdd(outputImage, gdalformat, bandDefnSeq)\n"
        "where:\n"
        "  outputImage is a string containing the name of the output file\n"
        "  gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
        "  bandDefnSeq is a sequence of rsgislib.imagecalibration.CmdsLandsatRadianceGainsOffsets objects that define the inputs\n"
        "    Requires:\n"
        "      bandName - Name of image band in output file.\n"
        "      fileName - input image file.\n"
        "      bandIndex - Index (starting from 1) of the band in the image file.\n"
        "      addVal - RADIANCE_ADD value from Landsat header.\n"
        "      multiVal - RADIANCE_MULT value from Landsat header.\n"},
    
{"radiance2TOARefl", ImageCalibration_Radiance2TOARefl, METH_VARARGS,
    "Converts at sensor radiance values to Top of Atmosphere Reflectance.\n"
    "call signature: imagecalibration.radiance2TOARefl(inputFile, outputFile, gdalFormat, gdaltype, scaleFactor, julianDay, solarZenith, solarIrradianceVals)\n"
    "where:\n"
    "  inputFile is a string containing the name of the input image file\n"
    "  outputFile is a string containing the name of the output image file\n"
    "  gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "  gdaltype is an containing one of the values from rsgislib.TYPE_*\n"
    "  scaleFactor is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 if not wanted.\n"
    "  year is an int with the year of the sensor acquisition.\n"
    "  month is an int with the month of the sensor acquisition.\n"
    "  day is an int with the day of the sensor acquisition.\n"
    "  solarZenith is a a float with the solar zenith in degrees at the time of the acquisition (note 90-solarElevation = solarZenith).\n"
    "  solarIrradianceVals is a sequence of floats each with the name \'irradiance\' which is in order of the bands in the input image.\n"},

{"apply6SCoeffSingleParam", ImageCalibration_Apply6SCoefficentsSingleParam, METH_VARARGS,
    "Converts at sensor radiance values to surface reflectance by apply the 6S model parameters for each band (aX, bX, cX).\n"
    "call signature: imagecalibration.apply6SCoeffSingleParam(inputFile, outputFile, gdalFormat, gdaltype, scaleFactor, noDataValue, useNoDataValue, bandCoeffs)\n"
    "where:\n"
    "  inputFile is a string containing the name of the input image file\n"
    "  outputFile is a string containing the name of the output image file\n"
    "  gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "  gdaltype is an containing one of the values from rsgislib.TYPE_*\n"
    "  scaleFactor is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 if not wanted.\n"
    "  noDataValue is a float which if all bands contain that value will be ignored.\n"
    "  useNoDataValue is a boolean as to whether the no data value specified is to be used.\n"
    "  bandCoeffs is a sequence of objects with the following named fields.\n"
    "    Requires:\n"
    "      band - An integer specifying the image band in the input file.\n"
    "      aX - A float for the aX coefficient.\n"
    "      bX - A float for the bX coefficient.\n"
    "      cX - A float for the cX coefficient.\n"},
    
    {NULL}        /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3

static int ImageCalibration_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int ImageCalibration_clear(PyObject *m)
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_imagecalibration",
        NULL,
        sizeof(struct ImageCalibrationState),
        ImageCalibrationMethods,
        NULL,
        ImageCalibration_traverse,
        ImageCalibration_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__imagecalibration(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_imagecalibration(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_imagecalibration", ImageCalibrationMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ImageCalibrationState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_imagecalibration.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

    // add constants
    //PyModule_AddIntConstant(pModule, "INITCLUSTER_RANDOM", rsgis::cmds::rsgis_init_random);
    //PyModule_AddIntConstant(pModule, "INITCLUSTER_DIAGONAL_FULL", rsgis::cmds::rsgis_init_diagonal_full);
    //PyModule_AddIntConstant(pModule, "INITCLUSTER_DIAGONAL_STDDEV", rsgis::cmds::rsgis_init_diagonal_stddev);
    //PyModule_AddIntConstant(pModule, "INITCLUSTER_DIAGONAL_FULL_ATTACH", rsgis::cmds::rsgis_init_diagonal_full_attach);
    //PyModule_AddIntConstant(pModule, "INITCLUSTER_DIAGONAL_STDDEV_ATTACH", rsgis::cmds::rsgis_init_diagonal_stddev_attach);

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
