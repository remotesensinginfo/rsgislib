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

static PyObject *ImageCalibration_landsat2Radiance(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("band_defs"), nullptr};
    const char *pszOutputFile, *pszGDALFormat;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssO:landsat_to_radiance", kwlist, &pszOutputFile, &pszGDALFormat, &pBandDefnObj))
    {
        return nullptr;
    }

    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
    }

    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    std::vector<rsgis::cmds::CmdsLandsatRadianceGainsOffsets> landsatRadGainOffs;
    landsatRadGainOffs.reserve(nBandDefns);
    
    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);

        PyObject *pBandName = PyObject_GetAttrString(o, "band_name");
        if( ( pBandName == nullptr ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'band_name\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            return nullptr;
        }

        PyObject *pFileName = PyObject_GetAttrString(o, "input_img");
        if( ( pFileName == nullptr ) || ( pFileName == Py_None ) || !RSGISPY_CHECK_STRING(pFileName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'input_img\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pFileName);
            Py_DECREF(o);
            return nullptr;
        }

        PyObject *pBandIndex = PyObject_GetAttrString(o, "img_band");
        if( ( pBandIndex == nullptr ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find integer attribute \'img_band\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pLMin = PyObject_GetAttrString(o, "l_min");
        if( ( pLMin == nullptr ) || ( pLMin == Py_None ) || !RSGISPY_CHECK_FLOAT(pLMin) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'l_min\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pLMin);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pLMax = PyObject_GetAttrString(o, "l_max");
        if( ( pLMax == nullptr ) || ( pLMax == Py_None ) || !RSGISPY_CHECK_FLOAT(pLMax) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'l_max\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pLMin);
            Py_XDECREF(pLMax);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pQCalMin = PyObject_GetAttrString(o, "q_cal_min");
        if( ( pQCalMin == nullptr ) || ( pQCalMin == Py_None ) || !RSGISPY_CHECK_FLOAT(pQCalMin) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'q_cal_min\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pLMin);
            Py_XDECREF(pLMax);
            Py_XDECREF(pQCalMin);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pQCalMax = PyObject_GetAttrString(o, "q_cal_max");
        if( ( pQCalMax == nullptr ) || ( pQCalMax == Py_None ) || !RSGISPY_CHECK_FLOAT(pQCalMax) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'q_cal_max\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pLMin);
            Py_XDECREF(pLMax);
            Py_XDECREF(pQCalMin);
            Py_XDECREF(pQCalMax);
            Py_DECREF(o);
            return nullptr;
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
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_landsat2RadianceMultiAdd(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("band_defs"), nullptr};
    const char *pszOutputFile, *pszGDALFormat;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssO:landsat_to_radiance_multi_add", kwlist, &pszOutputFile, &pszGDALFormat, &pBandDefnObj))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    std::vector<rsgis::cmds::CmdsLandsatRadianceGainsOffsetsMultiAdd> landsatRadGainOffs;
    landsatRadGainOffs.reserve(nBandDefns);
    
    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);
        
        PyObject *pBandName = PyObject_GetAttrString(o, "band_name");
        if( ( pBandName == nullptr ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'band_name\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pFileName = PyObject_GetAttrString(o, "input_img");
        if( ( pFileName == nullptr ) || ( pFileName == Py_None ) || !RSGISPY_CHECK_STRING(pFileName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'input_img\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pFileName);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pBandIndex = PyObject_GetAttrString(o, "img_band");
        if( ( pBandIndex == nullptr ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find integer attribute \'img_band\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pAddVal = PyObject_GetAttrString(o, "add_val");
        if( ( pAddVal == nullptr ) || ( pAddVal == Py_None ) || !RSGISPY_CHECK_FLOAT(pAddVal) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'addVal\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pAddVal);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pMultiVal = PyObject_GetAttrString(o, "multi_val");
        if( ( pMultiVal == nullptr ) || ( pMultiVal == Py_None ) || !RSGISPY_CHECK_FLOAT(pMultiVal) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'multi_val\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pAddVal);
            Py_XDECREF(pMultiVal);
            Py_DECREF(o);
            return nullptr;
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
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_Radiance2TOARefl(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("scale_factor"), RSGIS_PY_C_TEXT("year"),
                             RSGIS_PY_C_TEXT("month"), RSGIS_PY_C_TEXT("day"),
                             RSGIS_PY_C_TEXT("solar_zenith"), RSGIS_PY_C_TEXT("solar_irradiance"), nullptr};
    const char *pszInputFile, *pszOutputFile, *pszGDALFormat;
    int nDataType, year, month, day;
    float scaleFactor, solarZenith;
    PyObject *pSolarIrrObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssifiiifO:radiance_to_toa_refl", kwlist, &pszInputFile, &pszOutputFile, &pszGDALFormat, &nDataType, &scaleFactor, &year, &month, &day, &solarZenith, &pSolarIrrObj))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pSolarIrrObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nSolarIrrDefns = PySequence_Size(pSolarIrrObj);
    unsigned int numSolarIrrVals = nSolarIrrDefns;
    float *solarIrradiance = new float[numSolarIrrVals];
    
    for( Py_ssize_t n = 0; n < nSolarIrrDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pSolarIrrObj, n);
        
        PyObject *pIrradiance = PyObject_GetAttrString(o, "irradiance");
        if( ( pIrradiance == nullptr ) || ( pIrradiance == Py_None ) || !RSGISPY_CHECK_FLOAT(pIrradiance) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'irradiance\'" );
            Py_XDECREF(pIrradiance);
            Py_DECREF(o);
            return nullptr;
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
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_TOARefl2Radiance(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("scale_factor"), RSGIS_PY_C_TEXT("solar_dist"),
                             RSGIS_PY_C_TEXT("solar_zenith"), RSGIS_PY_C_TEXT("solar_irradiance"), nullptr};
    const char *pszOutputFile, *pszGDALFormat;
    int nDataType;
    float scaleFactor, solarZenith, solarDistance;
    PyObject *pSolarIrrObj;
    PyObject *pInputImgsObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "OssifffO:toa_refl_to_radiance", kwlist, &pInputImgsObj, &pszOutputFile, &pszGDALFormat, &nDataType, &scaleFactor, &solarDistance, &solarZenith, &pSolarIrrObj))
    {
        return nullptr;
    }
    
    // Get input image files
    std::vector<std::string> inputImgFiles = std::vector<std::string>();
    if(PySequence_Check(pInputImgsObj))
    {
        Py_ssize_t nInputImgs = PySequence_Size(pInputImgsObj);
        for( Py_ssize_t n = 0; n < nInputImgs; n++ )
        {
            PyObject *strObj = PySequence_GetItem(pInputImgsObj, n);
            if(RSGISPY_CHECK_STRING(strObj))
            {
                inputImgFiles.push_back(RSGISPY_STRING_EXTRACT(strObj));
            }
            else
            {
                PyErr_SetString(GETSTATE(self)->error, "Input images sequence must contain a list of strings");
                return nullptr;
            }
        }
    }
    else
    {
        if(RSGISPY_CHECK_STRING(pInputImgsObj))
        {
            inputImgFiles.push_back(RSGISPY_STRING_EXTRACT(pInputImgsObj));
        }
        else
        {
            PyErr_SetString(GETSTATE(self)->error, "Input images parameter must be either a single string or a sequence of strings");
            return nullptr;
        }
    }
    
    if( !PySequence_Check(pSolarIrrObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nSolarIrrDefns = PySequence_Size(pSolarIrrObj);
    unsigned int numSolarIrrVals = nSolarIrrDefns;
    float *solarIrradiance = new float[numSolarIrrVals];
    
    for( Py_ssize_t n = 0; n < nSolarIrrDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pSolarIrrObj, n);
        
        PyObject *pIrradiance = PyObject_GetAttrString(o, "irradiance");
        if( ( pIrradiance == nullptr ) || ( pIrradiance == Py_None ) || !RSGISPY_CHECK_FLOAT(pIrradiance) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'irradiance\'" );
            Py_XDECREF(pIrradiance);
            Py_DECREF(o);
            return nullptr;
        }
        
        solarIrradiance[n] = RSGISPY_FLOAT_EXTRACT(pIrradiance);
        
        Py_DECREF(pIrradiance);
        Py_DECREF(o);
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeConvertTOARefl2Radiance(inputImgFiles, pszOutputFile, pszGDALFormat, type, scaleFactor, solarDistance, (solarZenith*(M_PI/180)), solarIrradiance, numSolarIrrVals);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_Apply6SCoefficentsSingleParam(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("scale_factor"), RSGIS_PY_C_TEXT("no_data_val"),
                             RSGIS_PY_C_TEXT("use_no_data"), RSGIS_PY_C_TEXT("band_coeffs"), nullptr};
    const char *pszInputFile, *pszOutputFile, *pszGDALFormat;
    int nDataType, useNoDataVal;
    float scaleFactor, noDataVal;
    PyObject *pBandValuesObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssiffiO:apply_6s_coeff_single_param", kwlist, &pszInputFile, &pszOutputFile, &pszGDALFormat, &nDataType, &scaleFactor, &noDataVal, &useNoDataVal, &pBandValuesObj))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pBandValuesObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
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
        if( ( pBand == nullptr ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find int attribute \'band\'" );
            Py_XDECREF(pBand);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pAX = PyObject_GetAttrString(o, "aX");
        if( ( pAX == nullptr ) || ( pAX == Py_None ) || !RSGISPY_CHECK_FLOAT(pAX) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'aX\'" );
            Py_XDECREF(pBand);
            Py_XDECREF(pAX);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pBX = PyObject_GetAttrString(o, "bX");
        if( ( pBX == nullptr ) || ( pBX == Py_None ) || !RSGISPY_CHECK_FLOAT(pBX) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'bX\'" );
            Py_XDECREF(pBand);
            Py_XDECREF(pAX);
            Py_XDECREF(pBX);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pCX = PyObject_GetAttrString(o, "cX");
        if( ( pCX == nullptr ) || ( pCX == Py_None ) || !RSGISPY_CHECK_FLOAT(pCX) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'cX\'" );
            Py_XDECREF(pBand);
            Py_XDECREF(pAX);
            Py_XDECREF(pBX);
            Py_XDECREF(pCX);
            Py_DECREF(o);
            return nullptr;
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
        return nullptr;
    }
    
    delete[] aX;
    delete[] bX;
    delete[] cX;
    delete[] imageBands;
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_Apply6SCoefficentsElevLUTParam(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_rad_img"), RSGIS_PY_C_TEXT("in_dem_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("scale_factor"), RSGIS_PY_C_TEXT("no_data_val"),
                             RSGIS_PY_C_TEXT("use_no_data"), RSGIS_PY_C_TEXT("band_lut_coeffs"), nullptr};
    const char *pszInputRadFile, *pszInputDEMFile, *pszOutputFile, *pszGDALFormat;
    int nDataType, useNoDataVal;
    float scaleFactor, noDataVal;
    PyObject *pLUTObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssiffiO:apply_6s_coeff_elev_lut_param", kwlist, &pszInputRadFile, &pszInputDEMFile, &pszOutputFile, &pszGDALFormat, &nDataType, &scaleFactor, &noDataVal, &useNoDataVal, &pLUTObj))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pLUTObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nLUTDefns = PySequence_Size(pLUTObj);
    
    std::vector<rsgis::cmds::Cmds6SElevationLUT> *elevLUT = new std::vector<rsgis::cmds::Cmds6SElevationLUT>();
    
    for( Py_ssize_t n = 0; n < nLUTDefns; ++n )
    {
        PyObject *pLUTValuesObj = PySequence_GetItem(pLUTObj, n);
        rsgis::cmds::Cmds6SElevationLUT lutVal = rsgis::cmds::Cmds6SElevationLUT();
        
        PyObject *pElev = PyObject_GetAttrString(pLUTValuesObj, "Elev");
        if( ( pElev == nullptr ) || ( pElev == Py_None ) || !RSGISPY_CHECK_FLOAT(pElev) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'Elev\' for the LUT (make sure it is a float!)" );
            Py_XDECREF(pElev);
            Py_DECREF(pLUTValuesObj);
            return nullptr;
        }
        lutVal.elev = RSGISPY_FLOAT_EXTRACT(pElev);
        Py_DECREF(pElev);
        
        PyObject *pBandValuesObj = PyObject_GetAttrString(pLUTValuesObj, "Coeffs");
        
        if( !PySequence_Check(pBandValuesObj))
        {
            PyErr_SetString(GETSTATE(self)->error, "Each element in the LUT have a sequence \'Coeffs\' be a sequence.");
            return nullptr;
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
            if( ( pBand == nullptr ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find int attribute \'band\'" );
                Py_XDECREF(pBand);
                Py_DECREF(o);
                return nullptr;
            }
        
            PyObject *pAX = PyObject_GetAttrString(o, "aX");
            if( ( pAX == nullptr ) || ( pAX == Py_None ) || !RSGISPY_CHECK_FLOAT(pAX) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'aX\'" );
                Py_XDECREF(pBand);
                Py_XDECREF(pAX);
                Py_DECREF(o);
                return nullptr;
            }
        
            PyObject *pBX = PyObject_GetAttrString(o, "bX");
            if( ( pBX == nullptr ) || ( pBX == Py_None ) || !RSGISPY_CHECK_FLOAT(pBX) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'bX\'" );
                Py_XDECREF(pBand);
                Py_XDECREF(pAX);
                Py_XDECREF(pBX);
                Py_DECREF(o);
                return nullptr;
            }
        
            PyObject *pCX = PyObject_GetAttrString(o, "cX");
            if( ( pCX == nullptr ) || ( pCX == Py_None ) || !RSGISPY_CHECK_FLOAT(pCX) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'cX\'" );
                Py_XDECREF(pBand);
                Py_XDECREF(pAX);
                Py_XDECREF(pBX);
                Py_XDECREF(pCX);
                Py_DECREF(o);
                return nullptr;
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
        return nullptr;
    }
    
    for(auto iterLUT = elevLUT->begin(); iterLUT != elevLUT->end(); ++iterLUT)
    {
        delete[] (*iterLUT).imageBands;
        delete[] (*iterLUT).aX;
        delete[] (*iterLUT).bX;
        delete[] (*iterLUT).cX;
    }
    delete elevLUT;
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_Apply6SCoefficentsElevAOTLUTParam(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_rad_img"), RSGIS_PY_C_TEXT("in_dem_img"),
                             RSGIS_PY_C_TEXT("in_aot_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("scale_factor"), RSGIS_PY_C_TEXT("no_data_val"),
                             RSGIS_PY_C_TEXT("use_no_data"), RSGIS_PY_C_TEXT("band_lut_coeffs"), nullptr};
    const char *pszInputRadFile, *pszInputDEMFile, *pszInputAOTFile, *pszOutputFile, *pszGDALFormat;
    int nDataType, useNoDataVal;
    float scaleFactor, noDataVal;
    PyObject *pLUTObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssssiffiO:apply_6s_coeff_elev_aot_lut_param", kwlist, &pszInputRadFile, &pszInputDEMFile, &pszInputAOTFile, &pszOutputFile, &pszGDALFormat, &nDataType, &scaleFactor, &noDataVal, &useNoDataVal, &pLUTObj))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pLUTObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nLUTDefns = PySequence_Size(pLUTObj);
    
    std::vector<rsgis::cmds::Cmds6SBaseElevAOTLUT> *elevAOTLUT = new std::vector<rsgis::cmds::Cmds6SBaseElevAOTLUT>();
    elevAOTLUT->reserve(nLUTDefns);
    
    for( Py_ssize_t n = 0; n < nLUTDefns; ++n )
    {
        PyObject *pElevLUTValuesObj = PySequence_GetItem(pLUTObj, n);
        rsgis::cmds::Cmds6SBaseElevAOTLUT lutElevVal = rsgis::cmds::Cmds6SBaseElevAOTLUT();
        
        PyObject *pElev = PyObject_GetAttrString(pElevLUTValuesObj, "Elev");
        if( ( pElev == nullptr ) || ( pElev == Py_None ) || !RSGISPY_CHECK_FLOAT(pElev) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'Elev\' for the LUT (make sure it is a float!)" );
            Py_XDECREF(pElev);
            Py_DECREF(pElevLUTValuesObj);
            return nullptr;
        }
        lutElevVal.elev = RSGISPY_FLOAT_EXTRACT(pElev);
        Py_DECREF(pElev);
        
        
        PyObject *pAOTLUTValuesObj = PyObject_GetAttrString(pElevLUTValuesObj, "Coeffs");
        
        if( !PySequence_Check(pAOTLUTValuesObj))
        {
            PyErr_SetString(GETSTATE(self)->error, "Each element in the Elevation LUT have a sequence of AOT \'Coeffs\'.");
            return nullptr;
        }
        Py_ssize_t nAOTLUTDefns = PySequence_Size(pAOTLUTValuesObj);
        lutElevVal.aotLUT = std::vector<rsgis::cmds::Cmds6SAOTLUT>();
        lutElevVal.aotLUT.reserve(nAOTLUTDefns);
        
        for( Py_ssize_t k = 0; k < nAOTLUTDefns; ++k )
        {
            PyObject *pAOTValuesObj = PySequence_GetItem(pAOTLUTValuesObj, k);
            rsgis::cmds::Cmds6SAOTLUT lutAOTVal = rsgis::cmds::Cmds6SAOTLUT();
            
            PyObject *pAOT = PyObject_GetAttrString(pAOTValuesObj, "AOT");
            if( ( pAOT == nullptr ) || ( pAOT == Py_None ) || !RSGISPY_CHECK_FLOAT(pAOT) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'AOT\' for the LUT (make sure it is a float!)" );
                Py_XDECREF(pAOT);
                Py_DECREF(pAOTValuesObj);
                Py_DECREF(pElevLUTValuesObj);
                return nullptr;
            }
            lutAOTVal.aot = RSGISPY_FLOAT_EXTRACT(pAOT);
            Py_DECREF(pAOT);
            
            PyObject *pBandValuesObj = PyObject_GetAttrString(pAOTValuesObj, "Coeffs");
            
            if( !PySequence_Check(pBandValuesObj))
            {
                PyErr_SetString(GETSTATE(self)->error, "Each element in the AOT LUT have a sequence \'Coeffs\'.");
                Py_DECREF(pAOTValuesObj);
                Py_DECREF(pElevLUTValuesObj);
                return nullptr;
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
                if( ( pBand == nullptr ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand) )
                {
                    PyErr_SetString(GETSTATE(self)->error, "Could not find int attribute \'band\'" );
                    Py_XDECREF(pBand);
                    Py_DECREF(o);
                    Py_DECREF(pAOTValuesObj);
                    Py_DECREF(pElevLUTValuesObj);
                    return nullptr;
                }
                
                PyObject *pAX = PyObject_GetAttrString(o, "aX");
                if( ( pAX == nullptr ) || ( pAX == Py_None ) || !RSGISPY_CHECK_FLOAT(pAX) )
                {
                    PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'aX\'" );
                    Py_XDECREF(pBand);
                    Py_XDECREF(pAX);
                    Py_DECREF(o);
                    Py_DECREF(pAOTValuesObj);
                    Py_DECREF(pElevLUTValuesObj);
                    return nullptr;
                }
                
                PyObject *pBX = PyObject_GetAttrString(o, "bX");
                if( ( pBX == nullptr ) || ( pBX == Py_None ) || !RSGISPY_CHECK_FLOAT(pBX) )
                {
                    PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'bX\'" );
                    Py_XDECREF(pBand);
                    Py_XDECREF(pAX);
                    Py_XDECREF(pBX);
                    Py_DECREF(o);
                    Py_DECREF(pAOTValuesObj);
                    Py_DECREF(pElevLUTValuesObj);
                    return nullptr;
                }
                
                PyObject *pCX = PyObject_GetAttrString(o, "cX");
                if( ( pCX == nullptr ) || ( pCX == Py_None ) || !RSGISPY_CHECK_FLOAT(pCX) )
                {
                    PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'cX\'" );
                    Py_XDECREF(pBand);
                    Py_XDECREF(pAX);
                    Py_XDECREF(pBX);
                    Py_XDECREF(pCX);
                    Py_DECREF(o);
                    Py_DECREF(pAOTValuesObj);
                    Py_DECREF(pElevLUTValuesObj);
                    return nullptr;
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
        return nullptr;
    }
    
    for(auto iterLUT = elevAOTLUT->begin(); iterLUT != elevAOTLUT->end(); ++iterLUT)
    {
        for(auto iterAOTLUT = (*iterLUT).aotLUT.begin(); iterAOTLUT != (*iterLUT).aotLUT.end(); ++iterAOTLUT)
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

static PyObject *ImageCalibration_ApplySubtractOffsets(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("in_offs_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("non_neg_int"),
                             RSGIS_PY_C_TEXT("use_no_data"), RSGIS_PY_C_TEXT("no_data_val"),
                             RSGIS_PY_C_TEXT("dark_refl_val"), nullptr};
    const char *pszInputFile, *pszInputOffsetsFile, *pszOutputFile, *pszGDALFormat;
    int nDataType, useNoDataValInt, nonNegativeInt;
    float noDataVal, darkObjReflVal;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssiiiff:apply_subtract_offsets", kwlist, &pszInputFile, &pszInputOffsetsFile, &pszOutputFile, &pszGDALFormat, &nDataType, &nonNegativeInt, &useNoDataValInt, &noDataVal, &darkObjReflVal))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeApplySubtractOffsets(std::string(pszInputFile), std::string(pszOutputFile), std::string(pszInputOffsetsFile), (bool)nonNegativeInt, std::string(pszGDALFormat), type, noDataVal, (bool)useNoDataValInt, darkObjReflVal);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_ApplySubtractSingleOffsets(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("non_neg_int"),
                             RSGIS_PY_C_TEXT("use_no_data"), RSGIS_PY_C_TEXT("no_data_val"),
                             RSGIS_PY_C_TEXT("dark_refl_val"), RSGIS_PY_C_TEXT("band_offsets"), nullptr};
    const char *pszInputFile, *pszOutputFile, *pszGDALFormat;
    int nDataType, useNoDataValInt, nonNegativeInt;
    float noDataVal, darkObjReflVal;
    PyObject *pImageOffsetsObj;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssiiiffO:apply_subtract_single_offsets", kwlist, &pszInputFile, &pszOutputFile, &pszGDALFormat, &nDataType, &nonNegativeInt, &useNoDataValInt, &noDataVal, &darkObjReflVal, &pImageOffsetsObj))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pImageOffsetsObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nImageOffsDefns = PySequence_Size(pImageOffsetsObj);
    unsigned int numImgOffsVals = nImageOffsDefns;
    std::vector<double> imageOffsVals = std::vector<double>();
    imageOffsVals.reserve(numImgOffsVals);
    
    for( Py_ssize_t n = 0; n < nImageOffsDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pImageOffsetsObj, n);
        
        PyObject *pOffset = PyObject_GetAttrString(o, "offset");
        if( ( pOffset == nullptr ) || ( pOffset == Py_None ) || !RSGISPY_CHECK_FLOAT(pOffset) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'offset\'" );
            Py_XDECREF(pOffset);
            Py_DECREF(o);
            return nullptr;
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
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_SaturatedPixelsMask(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("band_defs"), nullptr};
    const char *pszOutputFile, *pszGDALFormat;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssO:saturated_pixels_mask", kwlist, &pszOutputFile, &pszGDALFormat, &pBandDefnObj))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    std::vector<rsgis::cmds::CmdsSaturatedPixel> satBandPxlInfo;
    satBandPxlInfo.reserve(nBandDefns);
    
    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);
        
        PyObject *pBandName = PyObject_GetAttrString(o, "band_name");
        if( ( pBandName == nullptr ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'band_name\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pFileName = PyObject_GetAttrString(o, "input_img");
        if( ( pFileName == nullptr ) || ( pFileName == Py_None ) || !RSGISPY_CHECK_STRING(pFileName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'input_img\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pFileName);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pBandIndex = PyObject_GetAttrString(o, "img_band");
        if( ( pBandIndex == nullptr ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find integer attribute \'img_band\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pSatVal = PyObject_GetAttrString(o, "sat_val");
        if( ( pSatVal == nullptr ) || ( pSatVal == Py_None ) || !RSGISPY_CHECK_FLOAT(pSatVal) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'sat_val\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pSatVal);
            Py_DECREF(o);
            return nullptr;
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
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_landsatThermalRad2Brightness(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("scale_factor"), RSGIS_PY_C_TEXT("band_defs"), nullptr};
    const char *pszInputFile, *pszOutputFile, *pszGDALFormat;
    int nDataType;
    float scaleFactor;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssifO:landsat_thermal_rad_to_brightness", kwlist, &pszInputFile, &pszOutputFile, &pszGDALFormat, &nDataType, &scaleFactor, &pBandDefnObj))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    std::vector<rsgis::cmds::CmdsLandsatThermalCoeffs> thermBandPxlInfo;
    thermBandPxlInfo.reserve(nBandDefns);
    
    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);
        
        PyObject *pBandName = PyObject_GetAttrString(o, "band_name");
        if( ( pBandName == nullptr ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'bandName\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pBandIndex = PyObject_GetAttrString(o, "img_band");
        if( ( pBandIndex == nullptr ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find integer attribute \'bandIndex\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pK1 = PyObject_GetAttrString(o, "k1");
        if( ( pK1 == nullptr ) || ( pK1 == Py_None ) || !RSGISPY_CHECK_FLOAT(pK1) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'k1\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pK1);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pK2 = PyObject_GetAttrString(o, "k2");
        if( ( pK2 == nullptr ) || ( pK2 == Py_None ) || !RSGISPY_CHECK_FLOAT(pK2) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'k2\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pK1);
            Py_XDECREF(pK2);
            Py_DECREF(o);
            return nullptr;
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
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_worldview2ToRadiance(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("band_defs"), nullptr};
    const char *pszInputFile, *pszOutputFile, *pszGDALFormat;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssO:worldview2_to_radiance", kwlist, &pszInputFile, &pszOutputFile, &pszGDALFormat, &pBandDefnObj))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    std::vector<rsgis::cmds::CmdsWorldView2RadianceGainsOffsets> wv2RadGainOffs;
    wv2RadGainOffs.reserve(nBandDefns);
    
    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);
        
        PyObject *pBandName = PyObject_GetAttrString(o, "band_name");
        if( ( pBandName == nullptr ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'band_name\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pBandIndex = PyObject_GetAttrString(o, "img_band");
        if( ( pBandIndex == nullptr ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find integer attribute \'img_band\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pAbsCalFact = PyObject_GetAttrString(o, "abs_cal_fact");
        if( ( pAbsCalFact == nullptr ) || ( pAbsCalFact == Py_None ) || !RSGISPY_CHECK_FLOAT(pAbsCalFact) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'abs_cal_fact\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pAbsCalFact);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pEffBandWidth = PyObject_GetAttrString(o, "eff_bandwidth");
        if( ( pEffBandWidth == nullptr ) || ( pEffBandWidth == Py_None ) || !RSGISPY_CHECK_FLOAT(pEffBandWidth) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'eff_bandwidth\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pAbsCalFact);
            Py_XDECREF(pEffBandWidth);
            Py_DECREF(o);
            return nullptr;
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
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_spot5ToRadiance(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("band_defs"), nullptr};
    const char *pszInputFile, *pszOutputFile, *pszGDALFormat;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssO:spot5_to_radiance", kwlist, &pszInputFile, &pszOutputFile, &pszGDALFormat, &pBandDefnObj))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    std::vector<rsgis::cmds::CmdsSPOTRadianceGainsOffsets> spot5RadGainOffs;
    spot5RadGainOffs.reserve(nBandDefns);
    
    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);
        
        PyObject *pBandName = PyObject_GetAttrString(o, "band_name");
        if( ( pBandName == nullptr ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'band_name\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pBandIndex = PyObject_GetAttrString(o, "img_band");
        if( ( pBandIndex == nullptr ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find integer attribute \'img_band\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pGain = PyObject_GetAttrString(o, "gain");
        if( ( pGain == nullptr ) || ( pGain == Py_None ) || !RSGISPY_CHECK_FLOAT(pGain) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'gain\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pGain);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pBias = PyObject_GetAttrString(o, "bias");
        if( ( pBias == nullptr ) || ( pBias == Py_None ) || !RSGISPY_CHECK_FLOAT(pBias) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'bias\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pBandIndex);
            Py_XDECREF(pGain);
            Py_XDECREF(pBias);
            Py_DECREF(o);
            return nullptr;
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
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_calcNadirImgViewAngle(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("altitude"),
                             RSGIS_PY_C_TEXT("min_xx_col"), RSGIS_PY_C_TEXT("min_xy_col"),
                             RSGIS_PY_C_TEXT("max_xx_col"), RSGIS_PY_C_TEXT("max_xy_col"),
                             RSGIS_PY_C_TEXT("min_yx_col"), RSGIS_PY_C_TEXT("min_yy_col"),
                             RSGIS_PY_C_TEXT("max_yx_col"), RSGIS_PY_C_TEXT("max_yy_col"), nullptr};
    const char *pszImgFootprint, *pszOutViewAngleImg, *pszGDALFormat;
    float sateAltitude = 0.0;
    const char *pszMinXXCol, *pszMinXYCol, *pszMaxXXCol, *pszMaxXYCol, *pszMinYXCol, *pszMinYYCol, *pszMaxYXCol, *pszMaxYYCol;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssfssssssss:calc_nadir_img_view_angle", kwlist, &pszImgFootprint, &pszOutViewAngleImg, &pszGDALFormat, &sateAltitude, &pszMinXXCol, &pszMinXYCol, &pszMaxXXCol, &pszMaxXYCol, &pszMinYXCol, &pszMinYYCol, &pszMaxYXCol, &pszMaxYYCol))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeCalcNadirImageViewAngle(std::string(pszImgFootprint), std::string(pszOutViewAngleImg), std::string(pszGDALFormat), sateAltitude, std::string(pszMinXXCol), std::string(pszMinXYCol), std::string(pszMaxXXCol), std::string(pszMaxXYCol), std::string(pszMinYXCol), std::string(pszMinYYCol), std::string(pszMaxYXCol), std::string(pszMaxYYCol));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_CalcIrradianceElevLUT(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_data_msk_img"), RSGIS_PY_C_TEXT("in_dem_img"),
                             RSGIS_PY_C_TEXT("in_inc_angle_img"), RSGIS_PY_C_TEXT("in_slope_img"),
                             RSGIS_PY_C_TEXT("in_sref_img"), RSGIS_PY_C_TEXT("in_shadow_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("solar_zenith"), RSGIS_PY_C_TEXT("scale_factor"),
                             RSGIS_PY_C_TEXT("coeff_lut"), nullptr};
    const char *pszInputDataMaskImg, *pszInputDEMFile, *pszInputIncidenceAngleImg, *pszInputSlopeImg, *pszSrefInputImage, *pszShadowMaskImg, *pszOutputFile, *pszGDALFormat;
    float solarZenith, reflScaleFactor = 0.0;
    PyObject *pLUTObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssssssffO:calc_irradiance_img_elev_lut", kwlist, &pszInputDataMaskImg, &pszInputDEMFile,
                                     &pszInputIncidenceAngleImg, &pszInputSlopeImg, &pszSrefInputImage, &pszShadowMaskImg, &pszOutputFile,
                                     &pszGDALFormat, &solarZenith, &reflScaleFactor, &pLUTObj))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pLUTObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nLUTDefns = PySequence_Size(pLUTObj);
    
    std::vector<rsgis::cmds::Cmds6SElevationLUT> *elevLUT = new std::vector<rsgis::cmds::Cmds6SElevationLUT>();
    
    for( Py_ssize_t n = 0; n < nLUTDefns; ++n )
    {
        PyObject *pLUTValuesObj = PySequence_GetItem(pLUTObj, n);
        rsgis::cmds::Cmds6SElevationLUT lutVal = rsgis::cmds::Cmds6SElevationLUT();
        
        PyObject *pElev = PyObject_GetAttrString(pLUTValuesObj, "Elev");
        if( ( pElev == nullptr ) || ( pElev == Py_None ) || !RSGISPY_CHECK_FLOAT(pElev) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'Elev\' for the LUT (make sure it is a float!)" );
            Py_XDECREF(pElev);
            Py_DECREF(pLUTValuesObj);
            return nullptr;
        }
        lutVal.elev = RSGISPY_FLOAT_EXTRACT(pElev);
        Py_DECREF(pElev);
        
        PyObject *pBandValuesObj = PyObject_GetAttrString(pLUTValuesObj, "Coeffs");
        
        if( !PySequence_Check(pBandValuesObj))
        {
            PyErr_SetString(GETSTATE(self)->error, "Each element in the LUT have a sequence \'Coeffs\' be a sequence.");
            return nullptr;
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
            if( ( pBand == nullptr ) || ( pBand == Py_None ) || !RSGISPY_CHECK_INT(pBand) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find int attribute \'band\'" );
                Py_XDECREF(pBand);
                Py_DECREF(o);
                return nullptr;
            }
            
            PyObject *pDirIrr = PyObject_GetAttrString(o, "DirIrr");
            if( ( pDirIrr == nullptr ) || ( pDirIrr == Py_None ) || !RSGISPY_CHECK_FLOAT(pDirIrr) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'DirIrr\'" );
                Py_XDECREF(pBand);
                Py_XDECREF(pDirIrr);
                Py_DECREF(o);
                return nullptr;
            }
            
            PyObject *pDifIrr = PyObject_GetAttrString(o, "DifIrr");
            if( ( pDifIrr == nullptr ) || ( pDifIrr == Py_None ) || !RSGISPY_CHECK_FLOAT(pDifIrr) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'DifIrr\'" );
                Py_XDECREF(pBand);
                Py_XDECREF(pDirIrr);
                Py_XDECREF(pDifIrr);
                Py_DECREF(o);
                return nullptr;
            }
            
            PyObject *pEnvIrr = PyObject_GetAttrString(o, "EnvIrr");
            if( ( pEnvIrr == nullptr ) || ( pEnvIrr == Py_None ) || !RSGISPY_CHECK_FLOAT(pEnvIrr) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Could not find float attribute \'EnvIrr\'" );
                Py_XDECREF(pBand);
                Py_XDECREF(pDirIrr);
                Py_XDECREF(pDifIrr);
                Py_XDECREF(pEnvIrr);
                Py_DECREF(o);
                return nullptr;
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
        return nullptr;
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

static PyObject *ImageCalibration_CalcStandardisedReflectanceSD2010(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_data_msk_img"), RSGIS_PY_C_TEXT("in_sref_img"),
                             RSGIS_PY_C_TEXT("in_solar_irr_img"), RSGIS_PY_C_TEXT("in_inc_angle_img"),
                             RSGIS_PY_C_TEXT("in_exit_angle_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("scale_factor"),
                             RSGIS_PY_C_TEXT("brdf_beta"), RSGIS_PY_C_TEXT("out_inc_angle"),
                             RSGIS_PY_C_TEXT("out_exit_angle"), nullptr};
    const char *pszInputDataMaskImg, *pszSrefInputImage, *pszInputSolarIrradiance, *pszInputIncidenceAngleImg, *pszInputExitanceAngleImg, *pszOutputFile, *pszGDALFormat;
    float reflScaleFactor = 1.0;
    float brdfBeta = 1.0;
    float outIncidenceAngle = 45;
    float outExitanceAngle = 45;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssssssffff:calc_standardised_reflectance_sd2010", kwlist, &pszInputDataMaskImg, &pszSrefInputImage,
                                     &pszInputSolarIrradiance, &pszInputIncidenceAngleImg, &pszInputExitanceAngleImg, &pszOutputFile,
                                     &pszGDALFormat, &reflScaleFactor, &brdfBeta, &outIncidenceAngle, &outExitanceAngle))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeCalcStandardisedReflectanceSD2010(std::string(pszInputDataMaskImg), std::string(pszSrefInputImage), std::string(pszInputSolarIrradiance), std::string(pszInputIncidenceAngleImg), std::string(pszInputExitanceAngleImg), std::string(pszOutputFile), std::string(pszGDALFormat), brdfBeta, outIncidenceAngle, outExitanceAngle, reflScaleFactor);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalibration_GetJulianDay(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("year"), RSGIS_PY_C_TEXT("month"),
                             RSGIS_PY_C_TEXT("day"),  nullptr};
    unsigned int year, month, day;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "III:get_julian_day", kwlist, &year, &month, &day))
    {
        return nullptr;
    }
    
    unsigned int julianDay = 0;
    try
    {
        julianDay = rsgis::cmds::executeGetJulianDay(year, month, day);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    return Py_BuildValue("I", julianDay);
}

static PyObject *ImageCalibration_CalcSolarDistance(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("julian_day"),  nullptr};
    unsigned int julianDay;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "I:calc_solar_distance", kwlist, &julianDay))
    {
        return nullptr;
    }
    
    float solarDistance = 0;
    try
    {
        solarDistance = rsgis::cmds::executeGetEarthSunDistance(julianDay);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    return Py_BuildValue("f", solarDistance);
}

// Our list of functions in this module
static PyMethodDef ImageCalibrationMethods[] = {
{"landsat_to_radiance", (PyCFunction)ImageCalibration_landsat2Radiance, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.landsat_to_radiance(output_img, gdalformat, bandDefnSeq)\n"
"Converts Landsat DN values to at sensor radiance.\n"
"\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param bandDefnSeq: is a sequence of rsgislib.imagecalibration.CmdsLandsatRadianceGainsOffsets objects that define the inputs\n"
"        * band_name - Name of image band in output file.\n"
"        * input_img - input image file.\n"
"        * img_band - Index (starting from 1) of the band in the image file.\n"
"        * l_min - lMin value from Landsat header.\n"
"        * l_max - lMax value from Landsat header.\n"
"        * q_cal_min - qCalMin value from Landsat header.\n"
"        * q_cal_max - qCalMax value from Landsat header.\n"
"\n"},

{"landsat_to_radiance_multi_add", (PyCFunction)ImageCalibration_landsat2RadianceMultiAdd, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.landsat_to_radiance_multi_add(output_img, gdalformat, bandDefnSeq)\n"
"Converts Landsat DN values to at sensor radiance.\n"
"\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param bandDefnSeq: is a sequence of rsgislib.imagecalibration.CmdsLandsatRadianceGainsOffsets objects that define the inputs\n"
"        * band_name - Name of image band in output file.\n"
"        * input_img - input image file.\n"
"        * img_band - Index (starting from 1) of the band in the image file.\n"
"        * addVal - RADIANCE_ADD value from Landsat header.\n"
"        * multiVal - RADIANCE_MULT value from Landsat header.\n"
"\n"},

{"radiance_to_toa_refl", (PyCFunction)ImageCalibration_Radiance2TOARefl, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.radiance_to_toa_refl(input_img, output_img, gdalformat, datatype, scale_factor, year, month, day, solar_zenith, solar_irradiance)\n"
"Converts at sensor radiance values to Top of Atmosphere Reflectance.\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param output_img: is a string containing the name of the output image file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param scale_factor: is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 if not wanted.\n"
":param year: is an int with the year of the sensor acquisition.\n"
":param month: is an int with the month of the sensor acquisition.\n"
":param day: is an int with the day of the sensor acquisition.\n"
":param solar_zenith: is a a float with the solar zenith in degrees at the time of the acquisition (note 90-solarElevation = solarZenith).\n"
":param solar_irradiance: is a sequence of floats each with the name \'irradiance\' which is in order of the bands in the input image.\n"
"\n"},

{"toa_refl_to_radiance", (PyCFunction)ImageCalibration_TOARefl2Radiance, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.toa_refl_to_radiance(input_imgs, output_img, gdalformat, datatype, scale_factor, solar_dist, solar_zenith, solar_irradiance)\n"
"Converts at sensor (Top of Atmosphere; TOA) reflectance values to at sensor radiance.\n"
"This is the inverse of imagecalibration.radiance_to_toa_refl().\n"
"\n"
":param input_imgs: can be either a single input image file with the same number of bands as the list of ESUN values or a list of single band images (same number of as the number of ESUN values).\n"
":param output_img: is a string containing the name of the output image file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param scale_factor: is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 if not wanted.\n"
":param solar_dist: is a float specifying the solar-earth distance (see imagecalibration.calc_solar_distance).\n"
":param solar_zenith: is a a float with the solar zenith in degrees at the time of the acquisition (note 90-solarElevation = solarZenith).\n"
":param solar_irradiance: is a sequence of floats each with the name \'irradiance\' (ESUN) which is in order of the bands in the input image.\n"
"\n"},

{"apply_6s_coeff_single_param", (PyCFunction)ImageCalibration_Apply6SCoefficentsSingleParam, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.apply_6s_coeff_single_param(input_img, output_img, gdalformat, datatype, scale_factor, no_data_val, use_no_data, band_coeffs)\n"
"Converts at sensor radiance values to surface reflectance by applying coefficients from the 6S model for each band (aX, bX, cX).\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param output_img: is a string containing the name of the output image file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param scale_factor: is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 for no scaling.\n"
":param no_data_val: is a float which if all bands contain that value will be ignored.\n"
":param use_no_data: is a boolean as to whether the no data value specified is to be used.\n"
":param band_coeffs: is a sequence of objects with the following named fields.\n"
"                    * band - An integer specifying the image band in the input file.\n"
"                    * aX - A float for the aX coefficient.\n"
"                    * bX - A float for the bX coefficient.\n"
"                    * cX - A float for the cX coefficient.\n"
"\n"},

{"apply_6s_coeff_elev_lut_param", (PyCFunction)ImageCalibration_Apply6SCoefficentsElevLUTParam, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.apply_6s_coeff_elev_lut_param(in_rad_img, in_dem_img, output_img, gdalformat, datatype, scale_factor, no_data_val, use_no_data, band_lut_coeffs)\n"
"Converts at sensor radiance values to surface reflectance by applying coefficients from the 6S model for each band (aX, bX, cX), where the coefficients can be varied for surface elevation.\n"
"\n"
":param in_rad_img: is a string containing the name of the input Radiance image file\n"
":param in_dem_img: is a string containing the name of the input DEM image file (needs to be the same projection and resolution as radiance image.)\n"
":param output_img: is a string containing the name of the output image file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param scale_factor: is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 for no scaling.\n"
":param no_data_val: is a float which if all bands contain that value will be ignored.\n"
":param use_no_data: is a boolean as to whether the no data value specified is to be used.\n"
":param band_lut_coeffs: is a sequence of objects with the following named fields - note these are expected to be in elevation order (low to high).\n"
"                    * \'Elev\' - The elevation for the element in the LUT (in metres).\n"
"                    * \'Coeffs\' - The sequence of 6S coeffecients for the given elevation for the element in the LUT.\n"
"                        * \'band\' - An integer specifying the image band in the input file (band numbers start at 1).\n"
"                        * \'aX\' - A float for the aX coefficient.\n"
"                        * \'bX\' - A float for the bX coefficient.\n"
"                        * \'cX\' - A float for the cX coefficient.\n"
"\n"},

{"apply_6s_coeff_elev_aot_lut_param", (PyCFunction)ImageCalibration_Apply6SCoefficentsElevAOTLUTParam, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.apply_6s_coeff_elev_aot_lut_param(in_rad_img, in_dem_img, in_aot_img, output_img, gdalformat, datatype, scale_factor, no_data_val, use_no_data, band_lut_coeffs)\n"
"Converts at sensor radiance values to surface reflectance by applying coefficients from the 6S model for each band (aX, bX, cX), where the coefficients can be varied for surface elevation.\n"
"\n"
":param in_rad_img: is a string containing the name of the input Radiance image file\n"
":param in_dem_img: is a string containing the name of the input DEM image file (needs to be the same projection and resolution as radiance image.)\n"
":param in_aot_img: is a string containing the name of the input AOT image file\n"
":param output_img: is a string containing the name of the output image file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param scale_factor: is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 for no scaling.\n"
":param no_data_val: is a float which if all bands contain that value will be ignored.\n"
":param use_no_data: is a boolean as to whether the no data value specified is to be used.\n"
":param band_lut_coeffs: is a sequence of objects with the following named fields - note these are expected to be in elevation order (low to high and then AOT order (low to high).\n"
"                    * \'Elev\' - The elevation for the element in the LUT (in metres).\n"
"                        * \'AOT\' - The AOT value for this element within the LUT.\n"
"                        * \'Coeffs\' - The sequence of 6S coeffecients for the given elevation and AOT for the element in the LUT.\n"
"                            * \'band\' - An integer specifying the image band in the input file (band numbers start at 1).\n"
"                            * \'aX\' - A float for the aX coefficient.\n"
"                            * \'bX\' - A float for the bX coefficient.\n"
"                            * \'cX\' - A float for the cX coefficient.\n"
"\n"},

{"apply_subtract_single_offsets", (PyCFunction)ImageCalibration_ApplySubtractSingleOffsets, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.apply_subtract_single_offsets(input_img, output_img, gdalformat, datatype, non_neg_int, use_no_data, no_data_val, dark_refl_val, band_offsets)\n"
"This function performs a dark object subtraction (DOS) using a set of defined offsets for retriving surface reflectance.\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param outputFile: is a string containing the name of the output image file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param non_neg_int: is a boolean specifying whether any negative values from the offset application should be removed (i.e., set to 1; 0 being no data).\n"
":param use_no_data: a boolean specifying whether a no data value is present within the input image.\n"
":param no_data_val: is a float specifying the no data value for the input image.\n"
":param dark_refl_val: is a float specifying the minimum value within the reflectance value used for the dark targets used for the subtraction"
":param band_offsets: is a list of offset values to be applied to the input image bands (specified with keyword 'offset')."
"\n"},

{"apply_subtract_offsets", (PyCFunction)ImageCalibration_ApplySubtractOffsets, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.apply_subtract_offsets(input_img, in_offs_img, output_img, gdalformat, datatype, non_neg_int, use_no_data, no_data_val, dark_refl_val)\n"
"This function performs a dark obejct subtraction (DOS) using a set of defined offsets for retriving surface reflectance.\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param in_offs_img: is a string containing the name of the input offsets image file, which must have the same number of bands as the input image."
":param output_img: is a string containing the name of the output image file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param non_neg_int: is a boolean specifying whether any negative values from the offset application should be removed (i.e., set to 1; 0 being no data).\n"
":param use_no_data: a boolean specifying whether a no data value is present within the input image.\n"
":param no_data_val: is a float specifying the no data value for the input image.\n"
":param dark_refl_val: is a float specifying the minimum value within the reflectance value used for the dark targets used for the subtraction"
"\n"},

{"saturated_pixels_mask", (PyCFunction)ImageCalibration_SaturatedPixelsMask, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.saturated_pixels_mask(output_img, gdalformat, band_defs)\n"
"Creates a mask of the saturated image pixels on a per band basis.\n"
"\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param band_defs: is a sequence of rsgislib.imagecalibration.CmdsSaturatedPixel objects that define the inputs\n"
"        *  band_name - Name of image band in output file.\n"
"        *  input_img - input image file.\n"
"        *  img_band - Index (starting from 1) of the band in the image file.\n"
"        *  satVal - Saturation value for the image band.\n"
"\n"},

{"landsat_thermal_rad_to_brightness", (PyCFunction)ImageCalibration_landsatThermalRad2Brightness, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.landsat_thermal_rad_to_brightness(input_img, output_img, gdalformat, datatype, scale_factor, band_defs)\n"
"Converts Landsat TM thermal radiation to degrees celsius for at sensor temperature.\n"
"\n"
":param input_img: is a string containing the name of the input file path.\n"
":param output_img: is a string containing the name of the output file path.\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param scale_factor: is a float which can be used to scale the output pixel values (e.g., multiple by 1000), set as 1 for no scaling.\n"
":param band_defs: is a sequence of rsgislib.imagecalibration.CmdsLandsatThermalCoeffs objects that define the inputs\n"
"        *  band_name - Name of image band in output file.\n"
"        *  img_band - Index (starting from 1) of the band in the image file.\n"
"        *  k1 - k1 coefficient from Landsat header.\n"
"        *  k2 - k2 coefficient from Landsat header.\n"
"\n"},
    
{"worldview2_to_radiance", (PyCFunction)ImageCalibration_worldview2ToRadiance, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.worldview2_to_radiance(input_img, output_img, gdalformat, band_defs)\n"
"Converts WorldView2 DN values to at sensor radiance.\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param band_defs: is a sequence of rsgislib.imagecalibration.CmdsWorldView2RadianceGainsOffsets objects that define the inputs\n"
"                        * band_name - Name of image band in output file.\n"
"                        * img_band - Index (starting from 1) of the band in the image file.\n"
"                        * abs_cal_fact - ABSCALFACTOR value from WorldView2 XML header.\n"
"                        * eff_bandwidth - EFFECTIVEBANDWIDTH value from WorldView2 XML header.\n"
"\n"},

{"spot5_to_radiance", (PyCFunction)ImageCalibration_spot5ToRadiance, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.spot5_to_radiance(input_img, output_img, gdalformat, band_defs)\n"
"Converts WorldView2 DN values to at sensor radiance.\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param band_defs: is a sequence of rsgislib.imagecalibration.CmdsSPOT5RadianceGainsOffsets objects in order of the input image bands that define the inputs\n"
"                        * band_name - Name of image band in output file.\n"
"                        * img_band - Index (starting from 1) of the output image band order (i.e., to reorder the image bands).\n"
"                        * gain - PHYSICAL_GAIN value from SPOT5 XML header.\n"
"                        * bias - PHYSICAL_BIAS value from SPOT5 XML header.\n"
"\n"},

{"calc_nadir_img_view_angle", (PyCFunction)ImageCalibration_calcNadirImgViewAngle, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.calc_nadir_img_view_angle(input_img, output_img, gdalformat, altitude, min_xx_col, min_xy_col, max_xx_col, max_xy_col, min_yx_col, min_yy_col, max_yx_col, max_yy_col)\n"
"Calculate the sensor view angle for each pixel for a nadir sensor. Need to provide the satellite altitude in metres, for Landsat this is 705000.0. \n"
"\n"
":param input_img: is a string containing the name/path of the input file for the image footprint. This file needs to be to have a RAT with only one clump with pixel value 1.\n"
":param output_img: is a string for the name/path of the output file with the view angles.\n"
":param gdalformat: is a string for the GDAL format\n"
":param altitude: is a float in metres for the satellite altitude.\n"
":param min_xx_col: is a string for the minXX column in the RAT.\n"
":param min_xy_col: is a string for the minXY column in the RAT.\n"
":param max_xx_col: is a string for the maxXX column in the RAT.\n"
":param max_xy_col: is a string for the maxXY column in the RAT.\n"
":param min_yx_col: is a string for the minYX column in the RAT.\n"
":param min_yy_col: is a string for the minYY column in the RAT.\n"
":param max_yx_col: is a string for the maxYX column in the RAT.\n"
":param max_yy_col: is a string for the maxYY column in the RAT.\n"
"\n"},

{"calc_irradiance_img_elev_lut", (PyCFunction)ImageCalibration_CalcIrradianceElevLUT, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.calc_irradiance_img_elev_lut(in_data_msk_img, in_dem_img, in_inc_angle_img, in_slope_img, in_sref_img, in_shadow_img, output_img, gdalformat, solar_zenith, scale_factor, coeff_lut)\n"
"Calculate the incoming irradiance (Direct, Diffuse, Environment and Total) for sloped surfaces (Eq 1. Shepherd and Dymond 2010).\n"
"\n"
":param in_data_msk_img: is a string containing the name and path to a binary mask specifying the region to be calculated (1 = True)\n"
":param in_dem_img: is a string containing the name of the input DEM image file.\n"
":param in_inc_angle_img: is a string containing the name and path to a file with the incidence angle for each pixel.\n"
":param in_slope_img: is a string containing the name and path to a file with the slope in degrees for each pixel.\n"
":param in_sref_img: is a surface reflectance image with the same number of bands for measurements are provided for in the LUT\n"
":param in_shadow_img: is a binary mask image for the areas of the image in direct shadow (pixel value 1) and therefore don't recieve any direct irradiance.\n"
":param output_img: is a string containing the name of the output image file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param solar_zenith: is a float with the solar zenith for the whole scene.\n"
":param scale_factor: is a float with the scale factor to convert the SREF image to a range of 0-1\n"
":param coeff_lut: is a sequence of objects with the following named fields - note these are expected to be in elevation order (low to high).\n"
"                    * \'Elev\' - The elevation for the element in the LUT (in metres).\n"
"                    * \'BandVals\' - The sequence of solar irradiance values for the bands in the SREF image.\n"
"                        * \'band\' - An integer specifying the image band in the input file (band numbers start at 1).\n"
"                        * \'DirIrr\' - A float for the direct irradiance for this band and elevation (i.e., as provided by 6S).\n"
"                        * \'DifIrr\' - A float for the diffuse irradiance for this band and elevation (i.e., as provided by 6S).\n"
"                        * \'EnvIrr\' - A float for the environment irradiance for this band and elevation (i.e., as provided by 6S).\n"
"\n"},

{"calc_standardised_reflectance_sd2010", (PyCFunction)ImageCalibration_CalcStandardisedReflectanceSD2010, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.calc_standardised_reflectance_sd2010(in_data_msk_img, in_sref_img, in_solar_irr_img, in_inc_angle_img, in_exit_angle_img, output_img, gdalformat, scale_factor, brdf_beta, out_inc_angle, out_exit_angle)\n"
"Calculate standardised reflectance, with respect to solar and view angles, as defined by Shepherd and Dymond (2010)\n"
"\n"
":param in_data_msk_img: is a string containing the name and path to a binary mask specifying the region to be calculated (1 = True)\n"
":param in_sref_img: is a surface reflectance image\n"
":param in_solar_irr_img: is the solar irradiance for each band of the SREF image. The image will have four times the number of bands as the SREF with Direct, Diffuse, Environment and Total irradiance for each (generated by imagecalibration.calc_irradiance_img_elev_lut).\n"
":param in_inc_angle_img: is a string containing the name and path to a file with the incidence angle for each pixel.\n"
":param in_exit_angle_img: is a string containing the name and path to a file with the existance angle for each pixel.\n"
":param output_img: is a string containing the name of the output image file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param scale_factor: is a float with the scale factor to convert the SREF image to a range of 0-1 (e.g., 1000)\n"
":param brdf_beta: is the beta parameters in equation 8 in Shepherd and Dymond (2010) for solar elevations between 50 - 70 degrees a value of 1 can be used.\n"
":param out_inc_angle: is the incidence angle to which the output image is standardised to (Recommend: 0).\n"
":param out_exit_angle: is the exitance angle to which the output image is standardised to (Recommend: 0).\n"
"\n"},
    
{"get_julian_day", (PyCFunction)ImageCalibration_GetJulianDay, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.get_julian_day(year, month, day)\n"
"Calculates the julian day for the input date.\n"
"\n"
":param year: is an int with the year of the sensor acquisition.\n"
":param month: is an int with the month of the sensor acquisition.\n"
":param day: is an int with the day of the sensor acquisition.\n"
"\n"
":return: julianDay - float\n"
"\n"},
    
{"calc_solar_distance", (PyCFunction)ImageCalibration_CalcSolarDistance, METH_VARARGS | METH_KEYWORDS,
"imagecalibration.calc_solar_distance(julian_day)\n"
"Calculates the earth-solar distance from the given julian day.\n"
"\n"
":param julian_day: is an int with the julian day of the sensor acquisition.\n"
"\n"
":return: solarDistance - float\n"
"\n"},

    {nullptr}        /* Sentinel */
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
        nullptr,
        sizeof(struct ImageCalibrationState),
        ImageCalibrationMethods,
        nullptr,
        ImageCalibration_traverse,
        ImageCalibration_clear,
        nullptr
};

#define INITERROR return nullptr

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
    if( pModule == nullptr )
        INITERROR;

    struct ImageCalibrationState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_imagecalibration.error", nullptr, nullptr);
    if( state->error == nullptr )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
