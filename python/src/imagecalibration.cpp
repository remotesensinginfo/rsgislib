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

#define _USE_MATH_DEFINES
#include <cmath>

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
            PyErr_SetString(GETSTATE(self)->error, "Could not find int attribute \'band\'" );
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

static PyObject *ImageCalibration_Apply6SCoefficentsElevLUTParam(PyObject *self, PyObject *args)
{
    const char *pszInputRadFile, *pszInputDEMFile, *pszOutputFile, *pszGDALFormat;
    int nDataType, useNoDataVal;
    float scaleFactor, noDataVal;
    PyObject *pLUTObj;
    if( !PyArg_ParseTuple(args, "ssssiffiO:apply6SCoeffElevLUTParam", &pszInputRadFile, &pszInputDEMFile, &pszOutputFile, &pszGDALFormat, &nDataType, &scaleFactor, &noDataVal, &useNoDataVal, &pLUTObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pLUTObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nLUTDefns = PySequence_Size(pLUTObj);
    
    std::vector<rsgis::cmds::Cmds6SElevationLUT> *elevLUT = new std::vector<rsgis::cmds::Cmds6SElevationLUT>();
    
    for( Py_ssize_t n = 0; n < nLUTDefns; ++n )
    {
        PyObject *pLUTValuesObj = PySequence_GetItem(pLUTObj, n);
        rsgis::cmds::Cmds6SElevationLUT lutVal = rsgis::cmds::Cmds6SElevationLUT();
        
        PyObject *pElev = PyObject_GetAttrString(pLUTValuesObj, "Elev");
        if( ( pElev == NULL ) || ( pElev == Py_None ) || !RSGISPY_CHECK_FLOAT(pElev) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'Elev\' for the LUT (make sure it is a float!)" );
            Py_XDECREF(pElev);
            Py_DECREF(pLUTValuesObj);
            return NULL;
        }
        lutVal.elev = RSGISPY_FLOAT_EXTRACT(pElev);
        Py_DECREF(pElev);
        
        PyObject *pBandValuesObj = PyObject_GetAttrString(pLUTValuesObj, "Coeffs");
        
        if( !PySequence_Check(pBandValuesObj))
        {
            PyErr_SetString(GETSTATE(self)->error, "Each element in the LUT have a sequence \'Coeffs\' be a sequence.");
            return NULL;
        }
        Py_ssize_t nBandValDefns = PySequence_Size(pBandValuesObj);
        
        lutVal.numValues = nBandValDefns;
        lutVal.imageBands = new unsigned int[lutVal.numValues];
        lutVal.aX = new float[lutVal.numValues];
        lutVal.bX = new float[lutVal.numValues];
        lutVal.cX = new float[lutVal.numValues];
        
        for( Py_ssize_t m = 0; m < nBandValDefns; ++m )
        {
            PyObject *o = PySequence_GetItem(pBandValuesObj, m);
            PyObject *pBand = PyObject_GetAttrString(o, "band");
            if( ( pBand == NULL ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find int attribute \'band\'" );
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
        
            lutVal.imageBands[m] = RSGISPY_INT_EXTRACT(pBand);
            lutVal.aX[m] = RSGISPY_FLOAT_EXTRACT(pAX);
            lutVal.bX[m] = RSGISPY_FLOAT_EXTRACT(pBX);
            lutVal.cX[m] = RSGISPY_FLOAT_EXTRACT(pCX);
            
            Py_DECREF(pBand);
            Py_DECREF(pAX);
            Py_DECREF(pBX);
            Py_DECREF(pCX);
            Py_DECREF(o);
        }
        Py_DECREF(pBandValuesObj);
        
        elevLUT->push_back(lutVal);
        
        Py_DECREF(pLUTValuesObj);
    }
        
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeRad2SREFElevLUT6sParams(std::string(pszInputRadFile), std::string(pszInputDEMFile), std::string(pszOutputFile), std::string(pszGDALFormat), type, scaleFactor, elevLUT, noDataVal, useNoDataVal);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    for(std::vector<rsgis::cmds::Cmds6SElevationLUT>::iterator iterLUT = elevLUT->begin(); iterLUT != elevLUT->end(); ++iterLUT)
    {
        delete[] (*iterLUT).imageBands;
        delete[] (*iterLUT).aX;
        delete[] (*iterLUT).bX;
        delete[] (*iterLUT).cX;
    }
    delete elevLUT;
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_Apply6SCoefficentsElevAOTLUTParam(PyObject *self, PyObject *args)
{
    const char *pszInputRadFile, *pszInputDEMFile, *pszInputAOTFile, *pszOutputFile, *pszGDALFormat;
    int nDataType, useNoDataVal;
    float scaleFactor, noDataVal;
    PyObject *pLUTObj;
    if( !PyArg_ParseTuple(args, "sssssiffiO:apply6SCoeffElevAOTLUTParam", &pszInputRadFile, &pszInputDEMFile, &pszInputAOTFile, &pszOutputFile, &pszGDALFormat, &nDataType, &scaleFactor, &noDataVal, &useNoDataVal, &pLUTObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pLUTObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nLUTDefns = PySequence_Size(pLUTObj);
    
    std::vector<rsgis::cmds::Cmds6SBaseElevAOTLUT> *elevAOTLUT = new std::vector<rsgis::cmds::Cmds6SBaseElevAOTLUT>();
    elevAOTLUT->reserve(nLUTDefns);
    
    for( Py_ssize_t n = 0; n < nLUTDefns; ++n )
    {
        PyObject *pElevLUTValuesObj = PySequence_GetItem(pLUTObj, n);
        rsgis::cmds::Cmds6SBaseElevAOTLUT lutElevVal = rsgis::cmds::Cmds6SBaseElevAOTLUT();
        
        PyObject *pElev = PyObject_GetAttrString(pElevLUTValuesObj, "Elev");
        if( ( pElev == NULL ) || ( pElev == Py_None ) || !RSGISPY_CHECK_FLOAT(pElev) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'Elev\' for the LUT (make sure it is a float!)" );
            Py_XDECREF(pElev);
            Py_DECREF(pElevLUTValuesObj);
            return NULL;
        }
        lutElevVal.elev = RSGISPY_FLOAT_EXTRACT(pElev);
        Py_DECREF(pElev);
        
        
        PyObject *pAOTLUTValuesObj = PyObject_GetAttrString(pElevLUTValuesObj, "Coeffs");
        
        if( !PySequence_Check(pAOTLUTValuesObj))
        {
            PyErr_SetString(GETSTATE(self)->error, "Each element in the Elevation LUT have a sequence of AOT \'Coeffs\'.");
            return NULL;
        }
        Py_ssize_t nAOTLUTDefns = PySequence_Size(pAOTLUTValuesObj);
        lutElevVal.aotLUT = std::vector<rsgis::cmds::Cmds6SAOTLUT>();
        lutElevVal.aotLUT.reserve(nAOTLUTDefns);
        
        for( Py_ssize_t k = 0; k < nAOTLUTDefns; ++k )
        {
            PyObject *pAOTValuesObj = PySequence_GetItem(pAOTLUTValuesObj, k);
            rsgis::cmds::Cmds6SAOTLUT lutAOTVal = rsgis::cmds::Cmds6SAOTLUT();
            
            PyObject *pAOT = PyObject_GetAttrString(pAOTValuesObj, "AOT");
            if( ( pAOT == NULL ) || ( pAOT == Py_None ) || !RSGISPY_CHECK_FLOAT(pAOT) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'AOT\' for the LUT (make sure it is a float!)" );
                Py_XDECREF(pAOT);
                Py_DECREF(pAOTValuesObj);
                Py_DECREF(pElevLUTValuesObj);
                return NULL;
            }
            lutAOTVal.aot = RSGISPY_FLOAT_EXTRACT(pAOT);
            Py_DECREF(pAOT);
            
            PyObject *pBandValuesObj = PyObject_GetAttrString(pAOTValuesObj, "Coeffs");
            
            if( !PySequence_Check(pBandValuesObj))
            {
                PyErr_SetString(GETSTATE(self)->error, "Each element in the AOT LUT have a sequence \'Coeffs\'.");
                Py_DECREF(pAOTValuesObj);
                Py_DECREF(pElevLUTValuesObj);
                return NULL;
            }
            Py_ssize_t nBandValDefns = PySequence_Size(pBandValuesObj);
            
            lutAOTVal.numValues = nBandValDefns;
            lutAOTVal.imageBands = new unsigned int[lutAOTVal.numValues];
            lutAOTVal.aX = new float[lutAOTVal.numValues];
            lutAOTVal.bX = new float[lutAOTVal.numValues];
            lutAOTVal.cX = new float[lutAOTVal.numValues];
            
            for( Py_ssize_t m = 0; m < nBandValDefns; ++m )
            {
                PyObject *o = PySequence_GetItem(pBandValuesObj, m);
                PyObject *pBand = PyObject_GetAttrString(o, "band");
                if( ( pBand == NULL ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand) )
                {
                    PyErr_SetString(GETSTATE(self)->error, "Could not find int attribute \'band\'" );
                    Py_XDECREF(pBand);
                    Py_DECREF(o);
                    Py_DECREF(pAOTValuesObj);
                    Py_DECREF(pElevLUTValuesObj);
                    return NULL;
                }
                
                PyObject *pAX = PyObject_GetAttrString(o, "aX");
                if( ( pAX == NULL ) || ( pAX == Py_None ) || !RSGISPY_CHECK_FLOAT(pAX) )
                {
                    PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'aX\'" );
                    Py_XDECREF(pBand);
                    Py_XDECREF(pAX);
                    Py_DECREF(o);
                    Py_DECREF(pAOTValuesObj);
                    Py_DECREF(pElevLUTValuesObj);
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
                    Py_DECREF(pAOTValuesObj);
                    Py_DECREF(pElevLUTValuesObj);
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
                    Py_DECREF(pAOTValuesObj);
                    Py_DECREF(pElevLUTValuesObj);
                    return NULL;
                }
                
                lutAOTVal.imageBands[m] = RSGISPY_INT_EXTRACT(pBand);
                lutAOTVal.aX[m] = RSGISPY_FLOAT_EXTRACT(pAX);
                lutAOTVal.bX[m] = RSGISPY_FLOAT_EXTRACT(pBX);
                lutAOTVal.cX[m] = RSGISPY_FLOAT_EXTRACT(pCX);
                
                Py_DECREF(pBand);
                Py_DECREF(pAX);
                Py_DECREF(pBX);
                Py_DECREF(pCX);
                Py_DECREF(o);
            }
            Py_DECREF(pBandValuesObj);
            
            lutElevVal.aotLUT.push_back(lutAOTVal);
            Py_DECREF(pAOTValuesObj);
        }
        
        elevAOTLUT->push_back(lutElevVal);
        Py_DECREF(pElevLUTValuesObj);
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeRad2SREFElevAOTLUT6sParams(std::string(pszInputRadFile), std::string(pszInputDEMFile), std::string(pszInputAOTFile), std::string(pszOutputFile), std::string(pszGDALFormat), type, scaleFactor, elevAOTLUT, noDataVal, useNoDataVal);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    for(std::vector<rsgis::cmds::Cmds6SBaseElevAOTLUT>::iterator iterLUT = elevAOTLUT->begin(); iterLUT != elevAOTLUT->end(); ++iterLUT)
    {
        for(std::vector<rsgis::cmds::Cmds6SAOTLUT>::iterator iterAOTLUT = (*iterLUT).aotLUT.begin(); iterAOTLUT != (*iterLUT).aotLUT.end(); ++iterAOTLUT)
        {
            delete[] (*iterAOTLUT).imageBands;
            delete[] (*iterAOTLUT).aX;
            delete[] (*iterAOTLUT).bX;
            delete[] (*iterAOTLUT).cX;
        }
    }
    delete elevAOTLUT;
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_ApplySubtractOffsets(PyObject *self, PyObject *args)
{
    const char *pszInputFile, *pszInputOffsetsFile, *pszOutputFile, *pszGDALFormat;
    int nDataType, useNoDataValInt, nonNegativeInt;
    float noDataVal, darkObjReflVal;

    if( !PyArg_ParseTuple(args, "ssssiiiff:applySubtractOffsets", &pszInputFile, &pszInputOffsetsFile, &pszOutputFile, &pszGDALFormat, &nDataType, &nonNegativeInt, &useNoDataValInt, &noDataVal, &darkObjReflVal))
    {
        return NULL;
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeApplySubtractOffsets(std::string(pszInputFile), std::string(pszOutputFile), std::string(pszInputOffsetsFile), (bool)nonNegativeInt, std::string(pszGDALFormat), type, noDataVal, (bool)useNoDataValInt, darkObjReflVal);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_ApplySubtractSingleOffsets(PyObject *self, PyObject *args)
{
    const char *pszInputFile, *pszOutputFile, *pszGDALFormat;
    int nDataType, useNoDataValInt, nonNegativeInt;
    float noDataVal, darkObjReflVal;
    PyObject *pImageOffsetsObj;
    
    if( !PyArg_ParseTuple(args, "sssiiiffO:applySubtractSingleOffsets", &pszInputFile, &pszOutputFile, &pszGDALFormat, &nDataType, &nonNegativeInt, &useNoDataValInt, &noDataVal, &darkObjReflVal, &pImageOffsetsObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pImageOffsetsObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nImageOffsDefns = PySequence_Size(pImageOffsetsObj);
    unsigned int numImgOffsVals = nImageOffsDefns;
    std::vector<double> imageOffsVals = std::vector<double>();
    imageOffsVals.reserve(numImgOffsVals);
    
    for( Py_ssize_t n = 0; n < nImageOffsDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pImageOffsetsObj, n);
        
        PyObject *pOffset = PyObject_GetAttrString(o, "offset");
        if( ( pOffset == NULL ) || ( pOffset == Py_None ) || !RSGISPY_CHECK_FLOAT(pOffset) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'offset\'" );
            Py_XDECREF(pOffset);
            Py_DECREF(o);
            return NULL;
        }
        
        imageOffsVals.push_back(RSGISPY_FLOAT_EXTRACT(pOffset));
        
        Py_DECREF(pOffset);
        Py_DECREF(o);
    }
    
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeApplySubtractSingleOffsets(std::string(pszInputFile), std::string(pszOutputFile), imageOffsVals, (bool)nonNegativeInt, std::string(pszGDALFormat), type, noDataVal, (bool)useNoDataValInt, darkObjReflVal);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_saturatedPixelsMask(PyObject *self, PyObject *args)
{
    const char *pszOutputFile, *pszGDALFormat;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTuple(args, "ssO:saturatedPixelsMask", &pszOutputFile, &pszGDALFormat, &pBandDefnObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    std::vector<rsgis::cmds::CmdsSaturatedPixel> satBandPxlInfo;
    satBandPxlInfo.reserve(nBandDefns);
    
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
        
        PyObject *pSatVal = PyObject_GetAttrString(o, "satVal");
        if( ( pSatVal == NULL ) || ( pSatVal == Py_None ) || !RSGISPY_CHECK_FLOAT(pSatVal) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'satVal\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pSatVal);
            Py_DECREF(o);
            return NULL;
        }
        
        rsgis::cmds::CmdsSaturatedPixel satVals;
        satVals.bandName = RSGISPY_STRING_EXTRACT(pBandName);
        satVals.imagePath = RSGISPY_STRING_EXTRACT(pFileName);
        satVals.band = RSGISPY_INT_EXTRACT(pBandIndex);
        satVals.satVal = RSGISPY_FLOAT_EXTRACT(pSatVal);
        
        satBandPxlInfo.push_back(satVals);
        
        Py_DECREF(pBandName);
        Py_DECREF(pFileName);
        Py_DECREF(pBandIndex);
        Py_XDECREF(pSatVal);
        Py_DECREF(o);
    }
    
    try
    {
        rsgis::cmds::executeGenerateSaturationMask(pszOutputFile, pszGDALFormat, satBandPxlInfo);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_landsatThermalRad2Brightness(PyObject *self, PyObject *args)
{
    const char *pszInputFile, *pszOutputFile, *pszGDALFormat;
    int nDataType;
    float scaleFactor;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTuple(args, "sssifO:landsatThermalRad2Brightness", &pszInputFile, &pszOutputFile, &pszGDALFormat, &nDataType, &scaleFactor, &pBandDefnObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    std::vector<rsgis::cmds::CmdsLandsatThermalCoeffs> thermBandPxlInfo;
    thermBandPxlInfo.reserve(nBandDefns);
    
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
        
        PyObject *pBandIndex = PyObject_GetAttrString(o, "bandIndex");
        if( ( pBandIndex == NULL ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find integer attribute \'bandIndex\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pK1 = PyObject_GetAttrString(o, "k1");
        if( ( pK1 == NULL ) || ( pK1 == Py_None ) || !RSGISPY_CHECK_FLOAT(pK1) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'k1\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pK1);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pK2 = PyObject_GetAttrString(o, "k2");
        if( ( pK2 == NULL ) || ( pK2 == Py_None ) || !RSGISPY_CHECK_FLOAT(pK2) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'k2\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pK1);
            Py_XDECREF(pK2);
            Py_DECREF(o);
            return NULL;
        }
        
        rsgis::cmds::CmdsLandsatThermalCoeffs thermVals;
        thermVals.bandName = RSGISPY_STRING_EXTRACT(pBandName);
        thermVals.band = RSGISPY_INT_EXTRACT(pBandIndex);
        thermVals.k1 = RSGISPY_FLOAT_EXTRACT(pK1);
        thermVals.k2 = RSGISPY_FLOAT_EXTRACT(pK2);
        
        thermBandPxlInfo.push_back(thermVals);
        
        Py_DECREF(pBandName);
        Py_DECREF(pBandIndex);
        Py_XDECREF(pK1);
        Py_XDECREF(pK2);
        Py_DECREF(o);
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeLandsatThermalRad2ThermalBrightness(std::string(pszInputFile), std::string(pszOutputFile), std::string(pszGDALFormat), type, scaleFactor, thermBandPxlInfo);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_applyLandsatTMCloudFMask(PyObject *self, PyObject *args)
{
    const char *pszInputTOAFile, *pszInputThermalFile, *pszInputSatFile, *pszValidAreaImg, *pszOutputFile, *pszTmpImgsBase, *pszTmpImgsFileExt, *pszGDALFormat;
    float sunAz, sunZen, senAz, senZen = 0.0;
    float scaleFactor;
    float whitenessThreshold = 0.7;
    int rmTmpImages = true;
    
    
    if( !PyArg_ParseTuple(args, "ssssssfffffss|fi:applyLandsatTMCloudFMask", &pszInputTOAFile, &pszInputThermalFile, &pszInputSatFile, &pszValidAreaImg, &pszOutputFile, &pszGDALFormat, &sunAz, &sunZen, &senAz, &senZen, &scaleFactor, &pszTmpImgsBase, &pszTmpImgsFileExt, &whitenessThreshold, &rmTmpImages))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeLandsatTMCloudFMask(std::string(pszInputTOAFile), std::string(pszInputThermalFile), std::string(pszInputSatFile), std::string(pszValidAreaImg), std::string(pszOutputFile), std::string(pszGDALFormat), sunAz, sunZen, senAz, senZen, whitenessThreshold, scaleFactor, std::string(pszTmpImgsBase), std::string(pszTmpImgsFileExt), (bool)rmTmpImages);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_worldview2ToRadiance(PyObject *self, PyObject *args)
{
    const char *pszInputFile, *pszOutputFile, *pszGDALFormat;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTuple(args, "sssO:worldview2ToRadiance", &pszInputFile, &pszOutputFile, &pszGDALFormat, &pBandDefnObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    std::vector<rsgis::cmds::CmdsWorldView2RadianceGainsOffsets> wv2RadGainOffs;
    wv2RadGainOffs.reserve(nBandDefns);
    
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
        
        PyObject *pBandIndex = PyObject_GetAttrString(o, "bandIndex");
        if( ( pBandIndex == NULL ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find integer attribute \'bandIndex\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pAbsCalFact = PyObject_GetAttrString(o, "absCalFact");
        if( ( pAbsCalFact == NULL ) || ( pAbsCalFact == Py_None ) || !RSGISPY_CHECK_FLOAT(pAbsCalFact) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'absCalFact\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pAbsCalFact);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pEffBandWidth = PyObject_GetAttrString(o, "effBandWidth");
        if( ( pEffBandWidth == NULL ) || ( pEffBandWidth == Py_None ) || !RSGISPY_CHECK_FLOAT(pEffBandWidth) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'effBandWidth\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pAbsCalFact);
            Py_XDECREF(pEffBandWidth);
            Py_DECREF(o);
            return NULL;
        }
        
        
        rsgis::cmds::CmdsWorldView2RadianceGainsOffsets radVals;
        radVals.bandName = RSGISPY_STRING_EXTRACT(pBandName);
        radVals.band = RSGISPY_INT_EXTRACT(pBandIndex);
        radVals.absCalFact = RSGISPY_FLOAT_EXTRACT(pAbsCalFact);
        radVals.effBandWidth = RSGISPY_FLOAT_EXTRACT(pEffBandWidth);
        
        wv2RadGainOffs.push_back(radVals);
        
        Py_DECREF(pBandName);
        Py_XDECREF(pBandIndex);
        Py_XDECREF(pAbsCalFact);
        Py_XDECREF(pEffBandWidth);
        Py_DECREF(o);
    }
    
    try
    {
        rsgis::cmds::executeConvertWorldView2ToRadiance(std::string(pszInputFile), std::string(pszOutputFile), std::string(pszGDALFormat), wv2RadGainOffs);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_spot5ToRadiance(PyObject *self, PyObject *args)
{
    const char *pszInputFile, *pszOutputFile, *pszGDALFormat;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTuple(args, "sssO:spot5ToRadiance", &pszInputFile, &pszOutputFile, &pszGDALFormat, &pBandDefnObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    std::vector<rsgis::cmds::CmdsSPOTRadianceGainsOffsets> spot5RadGainOffs;
    spot5RadGainOffs.reserve(nBandDefns);
    
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
        
        PyObject *pBandIndex = PyObject_GetAttrString(o, "bandIndex");
        if( ( pBandIndex == NULL ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find integer attribute \'bandIndex\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pGain = PyObject_GetAttrString(o, "gain");
        if( ( pGain == NULL ) || ( pGain == Py_None ) || !RSGISPY_CHECK_FLOAT(pGain) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'gain\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pGain);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pBias = PyObject_GetAttrString(o, "bias");
        if( ( pBias == NULL ) || ( pBias == Py_None ) || !RSGISPY_CHECK_FLOAT(pBias) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'bias\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pGain);
            Py_XDECREF(pBias);
            Py_DECREF(o);
            return NULL;
        }
        
        
        rsgis::cmds::CmdsSPOTRadianceGainsOffsets radVals;
        radVals.bandName = RSGISPY_STRING_EXTRACT(pBandName);
        radVals.band = RSGISPY_INT_EXTRACT(pBandIndex);
        radVals.gain = RSGISPY_FLOAT_EXTRACT(pGain);
        radVals.bias = RSGISPY_FLOAT_EXTRACT(pBias);
        
        spot5RadGainOffs.push_back(radVals);
        
        Py_DECREF(pBandName);
        Py_XDECREF(pBandIndex);
        Py_XDECREF(pGain);
        Py_XDECREF(pBias);
        Py_DECREF(o);
    }
    
    try
    {
        rsgis::cmds::executeConvertSPOT5ToRadiance(std::string(pszInputFile), std::string(pszOutputFile), std::string(pszGDALFormat), spot5RadGainOffs);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_calcNadirImgViewAngle(PyObject *self, PyObject *args)
{
    const char *pszImgFootprint, *pszOutViewAngleImg, *pszGDALFormat;
    float sateAltitude = 0.0;
    const char *pszMinXXCol, *pszMinXYCol, *pszMaxXXCol, *pszMaxXYCol, *pszMinYXCol, *pszMinYYCol, *pszMaxYXCol, *pszMaxYYCol;
    
    if( !PyArg_ParseTuple(args, "sssfssssssss:calcNadirImgViewAngle", &pszImgFootprint, &pszOutViewAngleImg, &pszGDALFormat, &sateAltitude, &pszMinXXCol, &pszMinXYCol, &pszMaxXXCol, &pszMaxXYCol, &pszMinYXCol, &pszMinYYCol, &pszMaxYXCol, &pszMaxYYCol))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeCalcNadirImageViewAngle(std::string(pszImgFootprint), std::string(pszOutViewAngleImg), std::string(pszGDALFormat), sateAltitude, std::string(pszMinXXCol), std::string(pszMinXYCol), std::string(pszMaxXXCol), std::string(pszMaxXYCol), std::string(pszMinYXCol), std::string(pszMinYYCol), std::string(pszMaxYXCol), std::string(pszMaxYYCol));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}



static PyObject *ImageCalibration_CalcIrradianceElevLUT(PyObject *self, PyObject *args)
{
    const char *pszInputDataMaskImg, *pszInputDEMFile, *pszInputIncidenceAngleImg, *pszInputSlopeImg, *pszSrefInputImage, *pszShadowMaskImg, *pszOutputFile, *pszGDALFormat;
    float solarZenith, reflScaleFactor = 0.0;
    PyObject *pLUTObj;
    if( !PyArg_ParseTuple(args, "ssssssssffO:calcIrradianceImageElevLUT", &pszInputDataMaskImg, &pszInputDEMFile, &pszInputIncidenceAngleImg, &pszInputSlopeImg, &pszSrefInputImage, &pszShadowMaskImg, &pszOutputFile, &pszGDALFormat, &solarZenith, &reflScaleFactor, &pLUTObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pLUTObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nLUTDefns = PySequence_Size(pLUTObj);
    
    std::vector<rsgis::cmds::Cmds6SElevationLUT> *elevLUT = new std::vector<rsgis::cmds::Cmds6SElevationLUT>();
    
    for( Py_ssize_t n = 0; n < nLUTDefns; ++n )
    {
        PyObject *pLUTValuesObj = PySequence_GetItem(pLUTObj, n);
        rsgis::cmds::Cmds6SElevationLUT lutVal = rsgis::cmds::Cmds6SElevationLUT();
        
        PyObject *pElev = PyObject_GetAttrString(pLUTValuesObj, "Elev");
        if( ( pElev == NULL ) || ( pElev == Py_None ) || !RSGISPY_CHECK_FLOAT(pElev) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'Elev\' for the LUT (make sure it is a float!)" );
            Py_XDECREF(pElev);
            Py_DECREF(pLUTValuesObj);
            return NULL;
        }
        lutVal.elev = RSGISPY_FLOAT_EXTRACT(pElev);
        Py_DECREF(pElev);
        
        PyObject *pBandValuesObj = PyObject_GetAttrString(pLUTValuesObj, "Coeffs");
        
        if( !PySequence_Check(pBandValuesObj))
        {
            PyErr_SetString(GETSTATE(self)->error, "Each element in the LUT have a sequence \'Coeffs\' be a sequence.");
            return NULL;
        }
        Py_ssize_t nBandValDefns = PySequence_Size(pBandValuesObj);
        
        lutVal.numValues = nBandValDefns;
        lutVal.imageBands = new unsigned int[lutVal.numValues];
        lutVal.directIrr = new float[lutVal.numValues];
        lutVal.diffuseIrr = new float[lutVal.numValues];
        lutVal.envIrr = new float[lutVal.numValues];
        lutVal.aX = new float[lutVal.numValues];
        lutVal.bX = new float[lutVal.numValues];
        lutVal.cX = new float[lutVal.numValues];
        
        for( Py_ssize_t m = 0; m < nBandValDefns; ++m )
        {
            PyObject *o = PySequence_GetItem(pBandValuesObj, m);
            PyObject *pBand = PyObject_GetAttrString(o, "band");
            if( ( pBand == NULL ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find int attribute \'band\'" );
                Py_XDECREF(pBand);
                Py_DECREF(o);
                return NULL;
            }
            
            PyObject *pDirIrr = PyObject_GetAttrString(o, "DirIrr");
            if( ( pDirIrr == NULL ) || ( pDirIrr == Py_None ) || !RSGISPY_CHECK_FLOAT(pDirIrr) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'DirIrr\'" );
                Py_XDECREF(pBand);
                Py_XDECREF(pDirIrr);
                Py_DECREF(o);
                return NULL;
            }
            
            PyObject *pDifIrr = PyObject_GetAttrString(o, "DifIrr");
            if( ( pDifIrr == NULL ) || ( pDifIrr == Py_None ) || !RSGISPY_CHECK_FLOAT(pDifIrr) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'DifIrr\'" );
                Py_XDECREF(pBand);
                Py_XDECREF(pDirIrr);
                Py_XDECREF(pDifIrr);
                Py_DECREF(o);
                return NULL;
            }
            
            PyObject *pEnvIrr = PyObject_GetAttrString(o, "EnvIrr");
            if( ( pEnvIrr == NULL ) || ( pEnvIrr == Py_None ) || !RSGISPY_CHECK_FLOAT(pEnvIrr) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'EnvIrr\'" );
                Py_XDECREF(pBand);
                Py_XDECREF(pDirIrr);
                Py_XDECREF(pDifIrr);
                Py_XDECREF(pEnvIrr);
                Py_DECREF(o);
                return NULL;
            }
            
            lutVal.imageBands[m] = RSGISPY_INT_EXTRACT(pBand);
            lutVal.directIrr[m] = RSGISPY_FLOAT_EXTRACT(pDirIrr);
            lutVal.diffuseIrr[m] = RSGISPY_FLOAT_EXTRACT(pDifIrr);
            lutVal.envIrr[m] = RSGISPY_FLOAT_EXTRACT(pEnvIrr);
            lutVal.aX[m] = 0.0;
            lutVal.bX[m] = 0.0;
            lutVal.cX[m] = 0.0;
            
            Py_DECREF(pBand);
            Py_DECREF(pDirIrr);
            Py_DECREF(pDifIrr);
            Py_DECREF(pEnvIrr);
            
            Py_DECREF(o);
        }
        Py_DECREF(pBandValuesObj);
        
        elevLUT->push_back(lutVal);
        
        Py_DECREF(pLUTValuesObj);
    }
    
    try
    {
        rsgis::cmds::executeCalcIrradianceElevLUT(std::string(pszInputDataMaskImg), std::string(pszInputDEMFile), std::string(pszInputIncidenceAngleImg), std::string(pszInputSlopeImg), std::string(pszShadowMaskImg), std::string(pszSrefInputImage), std::string(pszOutputFile), std::string(pszGDALFormat), solarZenith, reflScaleFactor, elevLUT);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    for(std::vector<rsgis::cmds::Cmds6SElevationLUT>::iterator iterLUT = elevLUT->begin(); iterLUT != elevLUT->end(); ++iterLUT)
    {
        delete[] (*iterLUT).imageBands;
        delete[] (*iterLUT).directIrr;
        delete[] (*iterLUT).diffuseIrr;
        delete[] (*iterLUT).envIrr;
        delete[] (*iterLUT).aX;
        delete[] (*iterLUT).bX;
        delete[] (*iterLUT).cX;
    }
    delete elevLUT;
    
    Py_RETURN_NONE;
}








// Our list of functions in this module
static PyMethodDef ImageCalibrationMethods[] = {
{"landsat2Radiance", ImageCalibration_landsat2Radiance, METH_VARARGS,
    "imagecalibration.landsat2Radiance(outputImage, gdalformat, bandDefnSeq)\n"
    "Converts Landsat DN values to at sensor radiance.\n"
    "Where:\n"
    "\n"
    "* outputImage is a string containing the name of the output file\n"
    "* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "* bandDefnSeq is a sequence of rsgislib.imagecalibration.CmdsLandsatRadianceGainsOffsets objects that define the inputs\n"
    "Requires:\n"
    "\n"
    "   * bandName - Name of image band in output file.\n"
    "   * fileName - input image file.\n"
    "   * bandIndex - Index (starting from 1) of the band in the image file.\n"
    "   * lMin - lMin value from Landsat header.\n"
    "   * lMax - lMax value from Landsat header.\n"
    "   * qCalMin - qCalMin value from Landsat header.\n"
    "   * qCalMax - qCalMax value from Landsat header.\n"
    "\n"},
    
{"landsat2RadianceMultiAdd", ImageCalibration_landsat2RadianceMultiAdd, METH_VARARGS,
    "imagecalibration.landsat2RadianceMultiAdd(outputImage, gdalformat, bandDefnSeq)\n"
    "Converts Landsat DN values to at sensor radiance.\n"
    "Where:\n"
    "\n"
    "* outputImage is a string containing the name of the output file\n"
    "* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "* bandDefnSeq is a sequence of rsgislib.imagecalibration.CmdsLandsatRadianceGainsOffsets objects that define the inputs\n"
    "Requires:\n"
    "\n"
    "   * bandName - Name of image band in output file.\n"
    "   * fileName - input image file.\n"
    "   * bandIndex - Index (starting from 1) of the band in the image file.\n"
    "   * addVal - RADIANCE_ADD value from Landsat header.\n"
    "   * multiVal - RADIANCE_MULT value from Landsat header.\n"
    "\n"},
    
{"radiance2TOARefl", ImageCalibration_Radiance2TOARefl, METH_VARARGS,
    "imagecalibration.radiance2TOARefl(inputFile, outputFile, gdalFormat, datatype, scaleFactor, julianDay, solarZenith, solarIrradianceVals)\n"
    "Converts at sensor radiance values to Top of Atmosphere Reflectance.\n"
    "Where:\n"
    "\n"
    "* inputFile is a string containing the name of the input image file\n"
    "* outputFile is a string containing the name of the output image file\n"
    "* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "* datatype is an containing one of the values from rsgislib.TYPE_*\n"
    "* scaleFactor is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 if not wanted.\n"
    "* year is an int with the year of the sensor acquisition.\n"
    "* month is an int with the month of the sensor acquisition.\n"
    "* day is an int with the day of the sensor acquisition.\n"
    "* solarZenith is a a float with the solar zenith in degrees at the time of the acquisition (note 90-solarElevation = solarZenith).\n"
    "* solarIrradianceVals is a sequence of floats each with the name \'irradiance\' which is in order of the bands in the input image.\n"
    "\n"},

{"apply6SCoeffSingleParam", ImageCalibration_Apply6SCoefficentsSingleParam, METH_VARARGS,
    "imagecalibration.apply6SCoeffSingleParam(inputFile, outputFile, gdalFormat, datatype, scaleFactor, noDataValue, useNoDataValue, bandCoeffs)\n"
    "Converts at sensor radiance values to surface reflectance by applying coefficients from the 6S model for each band (aX, bX, cX).\n"
    "Where:\n"
    "\n"
    "* inputFile is a string containing the name of the input image file\n"
    "* outputFile is a string containing the name of the output image file\n"
    "* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "* datatype is an containing one of the values from rsgislib.TYPE_*\n"
    "* scaleFactor is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 for no scaling.\n"
    "* noDataValue is a float which if all bands contain that value will be ignored.\n"
    "* useNoDataValue is a boolean as to whether the no data value specified is to be used.\n"
    "* bandCoeffs is a sequence of objects with the following named fields.\n"
    "Requires:\n"
    "\n"
    "   * band - An integer specifying the image band in the input file.\n"
    "   * aX - A float for the aX coefficient.\n"
    "   * bX - A float for the bX coefficient.\n"
    "   * cX - A float for the cX coefficient.\n"
    "\n"},
    
{"apply6SCoeffElevLUTParam", ImageCalibration_Apply6SCoefficentsElevLUTParam, METH_VARARGS,
    "imagecalibration.apply6SCoeffElevLUTParam(inputRadFile, inputDEMFile, outputFile, gdalFormat, datatype, scaleFactor, noDataValue, useNoDataValue, lutElev)\n"
    "Converts at sensor radiance values to surface reflectance by applying coefficients from the 6S model for each band (aX, bX, cX), where the coefficients can be varied for surface elevation.\n"
    "Where:\n"
    "\n"
    "* inputRadFile is a string containing the name of the input Radiance image file\n"
    "* inputDEMFile is a string containing the name of the input DEM image file (needs to be the same projection and resolution as radiance image.)\n"
    "* outputFile is a string containing the name of the output image file\n"
    "* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "* datatype is an containing one of the values from rsgislib.TYPE_*\n"
    "* scaleFactor is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 for no scaling.\n"
    "* noDataValue is a float which if all bands contain that value will be ignored.\n"
    "* useNoDataValue is a boolean as to whether the no data value specified is to be used.\n"
    "* lutElev is a sequence of objects with the following named fields - note these are expected to be in elevation order (low to high).\n"
    "Requires:\n"
    "\n"
    "    * \'Elev\' - The elevation for the element in the LUT (in metres).\n"
    "    * \'Coeffs\' - The sequence of 6S coeffecients for the given elevation for the element in the LUT.\n"
    "\n"
    "    \'Coeffs\' Requires a sequence with the following:\n"
    "        * \'band\' - An integer specifying the image band in the input file (band numbers start at 1).\n"
    "        * \'aX\' - A float for the aX coefficient.\n"
    "        * \'bX\' - A float for the bX coefficient.\n"
    "        * \'cX\' - A float for the cX coefficient.\n"
    "\n"},
    
{"apply6SCoeffElevAOTLUTParam", ImageCalibration_Apply6SCoefficentsElevAOTLUTParam, METH_VARARGS,
    "imagecalibration.apply6SCoeffElevLUTParam(inputRadFile, inputDEMFile, inputAOTImage, outputFile, gdalFormat, datatype, scaleFactor, noDataValue, useNoDataValue, lutElevAOT)\n"
    "Converts at sensor radiance values to surface reflectance by applying coefficients from the 6S model for each band (aX, bX, cX), where the coefficients can be varied for surface elevation.\n"
    "Where:\n"
    "\n"
    "* inputRadFile is a string containing the name of the input Radiance image file\n"
    "* inputDEMFile is a string containing the name of the input DEM image file (needs to be the same projection and resolution as radiance image.)\n"
    "* inputAOTImage is a string containing the name of the input AOT image file (needs to be the same projection and resolution as radiance image.)\n"
    "* outputFile is a string containing the name of the output image file\n"
    "* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "* datatype is an containing one of the values from rsgislib.TYPE_*\n"
    "* scaleFactor is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 for no scaling.\n"
    "* noDataValue is a float which if all bands contain that value will be ignored.\n"
    "* useNoDataValue is a boolean as to whether the no data value specified is to be used.\n"
    "* lutElevAOT is a sequence of objects with the following named fields - note these are expected to be in elevation order (low to high) and then AOT order (low to high).\n"
    "Requires:\n"
    "\n"
    "    * \'Elev\' - The elevation for the element in the LUT (in metres).\n"
    "    * \'Coeffs\' - For the specified Elevation an LUT for AOT specifying the 6S coefficients.\n"
    "\n"
    "    * \'AOT\' - The AOT value for this element within the LUT.\n"
    "    * \'Coeffs\' - The sequence of 6S coeffecients for the given elevation and AOT for the element in the LUT.\n"
    "\n"
    "        \'Coeffs\' Requires a sequence with the following:\n"
    "            * \'band\' - An integer specifying the image band in the input file (band numbers start at 1).\n"
    "            * \'aX\' - A float for the aX coefficient.\n"
    "            * \'bX\' - A float for the bX coefficient.\n"
    "            * \'cX\' - A float for the cX coefficient.\n"
    "\n"},
    
{"applySubtractSingleOffsets", ImageCalibration_ApplySubtractSingleOffsets, METH_VARARGS,
    "imagecalibration.applySubtractSingleOffsets(inputFile, outputFile, gdalformat, datatype, nonNegative, useNoDataVal, noDataVal, darkObjReflVal, offsetsList)\n"
    "Applies offsets from dark objects.\n"
    //TODO: Pete add in more details here.
    "Where:\n"
    "\n"
    "* inputFile is a string containing the name of the input image file\n"
    "* outputFile is a string containing the name of the output image file\n"
    "* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "* datatype is an containing one of the values from rsgislib.TYPE_*\n"
    "* nonNegative is a boolean specifying whether any negative values from the offset application should be removed (i.e., set to 1; 0 being no data).\n"
    "* useNoDataVal a boolean specifying whether a no data value is present within the input image.\n"
    "* noDataVal is a float specifying the no data value for the input image.\n"
    "* darkObjReflVal is a float specifying the minimum value within the reflectance value used for the dark targets used for the subtraction"
    "* offsetsList is a list of offset values to be applied to the input image bands (specified with keyword 'offset')."
    "\n"},
    
{"applySubtractOffsets", ImageCalibration_ApplySubtractOffsets, METH_VARARGS,
    "imagecalibration.applySubtractOffsets(inputFile, inputOffsetsFile, outputFile, gdalformat, datatype, nonNegative, useNoDataVal, noDataVal, darkObjReflVal)\n"
    "Applies offsets from dark objects.\n"
    //TODO: Pete add in more details here.
    "Where:\n"
    "\n"
    "* inputFile is a string containing the name of the input image file\n"
    "* inputOffsetsFile is a string containing the name of the input offsets image file, which must have the same number of bands as the input image."
    "* outputFile is a string containing the name of the output image file\n"
    "* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "* datatype is an containing one of the values from rsgislib.TYPE_*\n"
    "* nonNegative is a boolean specifying whether any negative values from the offset application should be removed (i.e., set to 1; 0 being no data).\n"
    "* useNoDataVal a boolean specifying whether a no data value is present within the input image.\n"
    "* noDataVal is a float specifying the no data value for the input image.\n"
    "* darkObjReflVal is a float specifying the minimum value within the reflectance value used for the dark targets used for the subtraction"
    "\n"},

{"saturatedPixelsMask", ImageCalibration_saturatedPixelsMask, METH_VARARGS,
    "imagecalibration.saturatedPixelsMask(outputImage, gdalformat, bandDefnSeq)\n"
    "Creates a mask of the saturated image pixels on a per band basis.\n"
    "Where:\n"
    "\n"
    "* outputImage is a string containing the name of the output file\n"
    "* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "* bandDefnSeq is a sequence of rsgislib.imagecalibration.CmdsSaturatedPixel objects that define the inputs\n"
    "Requires:\n"
    "\n"
    "    *  bandName - Name of image band in output file.\n"
    "    *  fileName - input image file.\n"
    "    *  bandIndex - Index (starting from 1) of the band in the image file.\n"
    "    *  satVal - Saturation value for the image band.\n"
    "\n"},

{"landsatThermalRad2Brightness", ImageCalibration_landsatThermalRad2Brightness, METH_VARARGS,
    "imagecalibration.landsatThermalRad2Brightness(inputImage, outputImage, gdalformat, datatype, scaleFactor, bandDefnSeq)\n"
    "Converts Landsat TM thermal radiation to degrees celsius for at sensor temperature.\n"
    "Where:\n"
    "\n"
    "* inputImage is a string containing the name of the input file path.\n"
    "outputImage is a string containing the name of the output file path.\n"
    "* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "* datatype is an containing one of the values from rsgislib.TYPE_*\n"
    "* scaleFactor is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 for no scaling.\n"
    "* bandDefnSeq is a sequence of rsgislib.imagecalibration.CmdsLandsatThermalCoeffs objects that define the inputs\n"
    "Requires:\n"
    "\n"
    "    *  bandName - Name of image band in output file.\n"
    "    *  bandIndex - Index (starting from 1) of the band in the image file.\n"
    "    *  k1 - k1 coefficient from Landsat header.\n"
    "    *  k2 - k2 coefficient from Landsat header.\n"
    "\n"},
    
{"applyLandsatTMCloudFMask", ImageCalibration_applyLandsatTMCloudFMask, METH_VARARGS,
"imagecalibration.applyLandsatTMCloudFMask(inputTOAImage, inputThermalImage, inputSaturateImage, inValidAreaImage, outputImage, gdalFormat, sunAz, sunZen, senAz, senZen, scaleFactorIn, tmpImgsBase, tmpImgsFileExt, rmTmpImgs)\n"
"Applies the FMASK (Zhu and Woodcock 2012, RSE 118, pp83-94) cloud masking algorithm to the input image returning an output image with the cloud (pixel value 1) and shadow (pixel value 2).\n"
"Where:\n"
"\n"
"* inputTOAImage is a string containing the name of the input image TOA reflectance file\n"
"* inputThermalImage is a string containing the name of the input image with at sensor temperature (in celsius)"
"* inputSaturateImage is a string containing the name of the input image file mask for the saturated pixels per band (including thermal)\n"
"* inValidAreaImage is a string containing the name of a binary image specifying the valid area of the image data (1 is valid area)\n"
"* outputImage is a string containing the name of the output image file\n"
"* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* sunAz is the solar azimuth of the input image\n"
"* sunZen is the solar azimuth of the input image\n"
"* senAz is the sensor azimuth of the input image\n"
"* senZen is the sensor azimuth of the input image\n"
"* scaleFactorIn is a float with the scale factor used to multiple the input image (reflectance and thermal) data.\n"
"* tmpImgsBase is a string specifying a base path and name for the tmp images used for this processing\n"
"* tmpImgsFileExt is a string for the file extention of the output images (e.g., .kea)\n"
"* whitenessThreshold is a float specifying the whiteness threshold (default is 0.7; Equation 2), this parameter is optional.\n"
"* rmTmpImgs is a bool specifying whether the tmp images should be deleted at the end of the processing (Optional; Default = True)\n"
"\n"
"Example:\n"
"\n"
"rsgislib.imagecalibration\n"
"inputReflImage = 'LS5TM_20110701_lat52lon421_r24p204_rad_toa.kea'\n"
"inputSatImage= 'LS5TM_20110701_lat52lon421_r24p204_sat.kea'\n"
"inValidImage = 'LS5TM_20110701_lat52lon421_r24p204_valid.kea'\n"
"inputThermalImage = 'LS5TM_20110701_lat52lon421_r24p204_thermal.kea'\n"
"outputImage = 'LS5TM_20110701_lat52lon421_r24p204_clouds.kea'\n"
"tmpImgsBase = './tmp/LS5TM_20110701_lat52lon421_r24p204'\n"
"if not os.path.exists(tmpImgsBase):\n"
"    os.makedirs(tmpImgsBase)\n"
"    tmpImgsBase = os.path.join(tmpImgsBase, 'LS5TM_20110701_lat52lon421_r24p204')\n"
"\n"
"sunAz = math.radians(143.94209355)\n"
"sunZen = math.radians(90-57.48916743)\n"
"\n"
"rsgislib.imagecalibration.applyLandsatTMCloudFMask(inputReflImage, inputThermalImage, inputSatImage, inValidImage, outputImage, 'KEA', sunAz, sunZen, senAz, senZen, 1000.0, tmpImgsBase, '.kea', 0.7, False)\n"
"\n"
},
    
{"worldview2ToRadiance", ImageCalibration_worldview2ToRadiance, METH_VARARGS,
    "imagecalibration.worldview2ToRadiance(inputImage, outputImage, gdalformat, bandDefnSeq)\n"
    "Converts WorldView2 DN values to at sensor radiance.\n"
    "Where:\n"
    "\n"
    "* inputImage is a string containing the name of the input file\n"
    "* outputImage is a string containing the name of the output file\n"
    "* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "* bandDefnSeq is a sequence of rsgislib.imagecalibration.CmdsWorldView2RadianceGainsOffsets objects that define the inputs\n"
    "Requires:\n"
    "\n"
    "   * bandName - Name of image band in output file.\n"
    "   * bandIndex - Index (starting from 1) of the band in the image file.\n"
    "   * absCalFact - ABSCALFACTOR value from WorldView2 XML header.\n"
    "   * effBandWidth - EFFECTIVEBANDWIDTH value from WorldView2 XML header.\n"
    "\n"},
    
{"spot5ToRadiance", ImageCalibration_spot5ToRadiance, METH_VARARGS,
    "imagecalibration.spot5ToRadiance(inputImage, outputImage, gdalformat, bandDefnSeq)\n"
    "Converts WorldView2 DN values to at sensor radiance.\n"
    "Where:\n"
    "\n"
    "* inputImage is a string containing the name of the input file\n"
    "* outputImage is a string containing the name of the output file\n"
    "* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
    "* bandDefnSeq is a sequence of rsgislib.imagecalibration.CmdsSPOT5RadianceGainsOffsets objects in order of the input image bands that define the inputs\n"
    "Requires:\n"
    "\n"
    "   * bandName - Name of image band in output file.\n"
    "   * bandIndex - Index (starting from 1) of the output image band order (i.e., to reorder the image bands).\n"
    "   * gain - PHYSICAL_GAIN value from SPOT5 XML header.\n"
    "   * bias - PHYSICAL_BIAS value from SPOT5 XML header.\n"
    "\n"},

{"calcNadirImgViewAngle", ImageCalibration_calcNadirImgViewAngle, METH_VARARGS,
"imagecalibration.calcNadirImgViewAngle(inImgFootprint, outViewAngleImg, gdalFormat, sateAltitude, minXXCol, minXYCol, maxXXCol, maxXYCol, minYXCol, minYYCol, maxYXCol, maxYYCol)\n"
"Calculate the sensor view angle for each pixel for a nadir sensor. Need to provide the satellite altitude in metres, for Landsat this is 705000.0. \n"
"Where:\n"
"\n"
"* inImgFootprint is a string containing the name/path of the input file. This file needs to be to have a RAT with only one clump with pixel value 1.\n"
"* outViewAngleImg is a string for the name/path of the output file.\n"
"* gdalFormat is a string for the GDAL format\n"
"* sateAltitude is a float in metres for the satellite altitude.\n"
"* minXXCol is a string for the minXX column in the RAT.\n"
"* minXYCol is a string for the minXY column in the RAT.\n"
"* maxXXCol is a string for the maxXX column in the RAT.\n"
"* maxXYCol is a string for the maxXY column in the RAT.\n"
"* minYXCol is a string for the minYX column in the RAT.\n"
"* minYYCol is a string for the minYY column in the RAT.\n"
"* maxYXCol is a string for the maxYX column in the RAT.\n"
"* maxYYCol is a string for the maxYY column in the RAT.\n"
"\n"},

{"calcIrradianceImageElevLUT", ImageCalibration_CalcIrradianceElevLUT, METH_VARARGS,
"imagecalibration.calcIrradianceImageElevLUT(inputDataMaskImg, inputDEMFile, inputIncidenceAngleImg, inputSlopeImg, srefInputImage, shadowMaskImg, outputFile, GDALFormat, solarZenith, reflScaleFactor, lutElev)\n"
"Calculate the incoming irradiance (Direct, Diffuse and Total) for sloped surfaces (Eq 1. Shepherd and Dymond 2010).\n"
"Where:\n"
"\n"
"* inputDataMaskImg is a string containing the name and path to a binary mask specifying the region to be calculated (1 = True)\n"
"* inputDEMFile is a string containing the name of the input DEM image file.\n"
"* inputIncidenceAngleImg is a string containing the name and path to a file with the incidence angle for each pixel.\n"
"* inputSlopeImg is a string containing the name and path to a file with the slope in degrees for each pixel.\n"
"* srefInputImage is a surface reflectance image with the same number of bands for measurements are provided for in the LUT\n"
"* shadowMaskImg is a binary mask image for the areas of the image in direct shadow (pixel value 1) and therefore don't recieve any direct irradiance.\n"
"* outputFile is a string containing the name of the output image file\n"
"* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* solarZenith is a float with the solar zenith for the whole scene.\n"
"* reflScaleFactor is a float with the scale factor to convert the SREF image to a range of 0-1\n"
"* lutElev is a sequence of objects with the following named fields - note these are expected to be in elevation order (low to high).\n"
"Requires:\n"
"\n"
"    * \'Elev\' - The elevation for the element in the LUT (in metres).\n"
"    * \'BandVals\' - The sequence of solar irradiance values for the bands in the SREF image.\n"
"\n"
"    \'BandVals\' Requires a sequence with the following:\n"
"        * \'band\' - An integer specifying the image band in the input file (band numbers start at 1).\n"
"        * \'DirIrr\' - A float for the direct irradiance for this band and elevation (i.e., as provided by 6S).\n"
"        * \'DifIrr\' - A float for the diffuse irradiance for this band and elevation (i.e., as provided by 6S).\n"
"        * \'EnvIrr\' - A float for the environment irradiance for this band and elevation (i.e., as provided by 6S).\n"
"\n"},
    
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
