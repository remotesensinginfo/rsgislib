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

static PyObject *Elevation_calcSlope(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("unit"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat, *pszOutUnit;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssss:slope", kwlist, &pszInputImage, &pszOutputFile, &pszOutUnit, &pszGDALFormat))
    {
        return nullptr;
    }

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
            throw rsgis::cmds::RSGISCmdException("The unit option needs to be specified as either 'degrees' or 'radians'.");
        }
        
        rsgis::cmds::executeCalcSlope(std::string(pszInputImage), std::string(pszOutputFile), outAngleUnit, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *Elevation_calcSlopePxlResImg(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_dem_img"), RSGIS_PY_C_TEXT("in_pxl_res_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("unit"),
                             RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInDEMImage, *pszInPxlResImage, *pszOutputFile, *pszGDALFormat, *pszOutUnit;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssss:slope_pxl_res_img", kwlist, &pszInDEMImage, &pszInPxlResImage, &pszOutputFile, &pszOutUnit, &pszGDALFormat))
    {
        return nullptr;
    }

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
            throw rsgis::cmds::RSGISCmdException("The unit option needs to be specified as either 'degrees' or 'radians'.");
        }

        rsgis::cmds::executeCalcSlopeImgPxlRes(std::string(pszInDEMImage), std::string(pszInPxlResImage), std::string(pszOutputFile), outAngleUnit, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *Elevation_calcAspect(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sss:aspect", kwlist, &pszInputImage, &pszOutputFile, &pszGDALFormat))
        return nullptr;
    
    try
    {
        rsgis::cmds::executeCalcAspect(std::string(pszInputImage), std::string(pszOutputFile), std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_calcAspectPxlResImg(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_dem_img"), RSGIS_PY_C_TEXT("in_pxl_res_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInDEMImage, *pszInPxlResImage, *pszOutputFile, *pszGDALFormat;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssss:aspect_pxl_res_img", kwlist, &pszInDEMImage, &pszInPxlResImage, &pszOutputFile, &pszGDALFormat))
        return nullptr;

    try
    {
        rsgis::cmds::executeCalcAspectImgPxlRes(std::string(pszInDEMImage), std::string(pszInPxlResImage), std::string(pszOutputFile), std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *Elevation_catAspect(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sss:catagorise_aspect", kwlist, &pszInputImage, &pszOutputFile, &pszGDALFormat))
        return nullptr;
    
    try
    {
        rsgis::cmds::executeCatagoriseAspect(std::string(pszInputImage), std::string(pszOutputFile), std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_calcHillshade(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("azimuth"), RSGIS_PY_C_TEXT("zenith"),
                             RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    float azimuth, zenith = 0.0;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssffs:hillshade", kwlist, &pszInputImage, &pszOutputFile, &azimuth, &zenith, &pszGDALFormat))
        return nullptr;
    
    try
    {
        rsgis::cmds::executeCalcHillshade(std::string(pszInputImage), std::string(pszOutputFile), azimuth, zenith, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_calcHillshadePxlResImg(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_dem_img"), RSGIS_PY_C_TEXT("in_pxl_res_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("azimuth"),
                             RSGIS_PY_C_TEXT("zenith"), RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInDEMImage, *pszInPxlResImage, *pszOutputFile, *pszGDALFormat;
    float azimuth, zenith = 0.0;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssffs:hillshade_pxl_res_img", kwlist, &pszInDEMImage, &pszInPxlResImage, &pszOutputFile, &azimuth, &zenith, &pszGDALFormat))
        return nullptr;

    try
    {
        rsgis::cmds::executeCalcHillshadeImgPxlRes(std::string(pszInDEMImage), std::string(pszInPxlResImage), std::string(pszOutputFile), azimuth, zenith, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *Elevation_calcShadowMask(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("azimuth"), RSGIS_PY_C_TEXT("zenith"),
                             RSGIS_PY_C_TEXT("max_height"), RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    float azimuth, zenith, maxHeight = 0.0;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssfffs:shadow_mask", kwlist, &pszInputImage, &pszOutputFile, &azimuth, &zenith, &maxHeight, &pszGDALFormat))
        return nullptr;
    
    try
    {
        rsgis::cmds::executeCalcShadowMask(std::string(pszInputImage), std::string(pszOutputFile), azimuth, zenith, maxHeight, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_calcLocalIncidenceAngle(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("azimuth"), RSGIS_PY_C_TEXT("zenith"),
                             RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    float azimuth, zenith = 0.0;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssffs:local_incidence_angle", kwlist, &pszInputImage, &pszOutputFile, &azimuth, &zenith, &pszGDALFormat))
        return nullptr;
    
    try
    {
        rsgis::cmds::executeCalcLocalIncidenceAngle(std::string(pszInputImage), std::string(pszOutputFile), azimuth, zenith, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_calcLocalExistanceAngle(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("azimuth"), RSGIS_PY_C_TEXT("zenith"),
                             RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    float azimuth, zenith = 0.0;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssffs:local_existance_angle", kwlist, &pszInputImage, &pszOutputFile, &azimuth, &zenith, &pszGDALFormat))
        return nullptr;
    
    try
    {
        rsgis::cmds::executeCalcLocalExitanceAngle(std::string(pszInputImage), std::string(pszOutputFile), azimuth, zenith, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_dtmAspectMedianFilter(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_dem_img"), RSGIS_PY_C_TEXT("in_aspect_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("aspect_range"),
                             RSGIS_PY_C_TEXT("win_h_size"), RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInputDTMImage, *pszInputAspectImage, *pszOutputFile, *pszGDALFormat;
    float aspectRange = 0.0;
    int winHSize = 0;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssfis:dtm_aspect_median_filter", kwlist, &pszInputDTMImage, &pszInputAspectImage, &pszOutputFile, &aspectRange, &winHSize, &pszGDALFormat))
        return nullptr;
    
    try
    {
        rsgis::cmds::executeDTMAspectMedianFilter(std::string(pszInputDTMImage), std::string(pszInputAspectImage), std::string(pszOutputFile), aspectRange, winHSize, std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_fillDEMSoilleGratin1994(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_dem_img"), RSGIS_PY_C_TEXT("in_vld_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInputDTMImage, *pszValidMaskImage, *pszOutputFile, *pszGDALFormat;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssss:fill_dem_soille_gratin_1994", kwlist, &pszInputDTMImage, &pszValidMaskImage, &pszOutputFile, &pszGDALFormat))
        return nullptr;
    
    try
    {
        rsgis::cmds::executeDEMFillSoilleGratin1994(std::string(pszInputDTMImage), std::string(pszValidMaskImage), std::string(pszOutputFile), std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Elevation_planeFitDetreadDEM(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("win_size"), nullptr};
    const char *pszInputDEMImage, *pszOutputFile, *pszGDALFormat;
    int winSize;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssi:plane_fit_detreat_dem", kwlist, &pszInputDEMImage, &pszOutputFile, &pszGDALFormat, &winSize))
        return nullptr;
    
    try
    {
        rsgis::cmds::executePlaneFitDetreadDEM(std::string(pszInputDEMImage), std::string(pszOutputFile), std::string(pszGDALFormat), winSize);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef ElevationMethods[] = {
{"slope", (PyCFunction)Elevation_calcSlope, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.slope(input_img, output_img, gdalformat, unit)\n"
"Calculates a slope layer given an input elevation model.\n"
"\n"
":param input_img: is a string containing the name and path of the input DEM file.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"
":param unit: is a string specifying the output unit ('degrees' or 'radians').\n"},

{"slope_pxl_res_img", (PyCFunction)Elevation_calcSlopePxlResImg, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.slope_pxl_res_img(in_dem_img, in_pxl_res_img, output_img, unit, gdalformat)\n"
"Calculates a slope layer given an input elevation model and external pixel \n"
"resolution image, which allows the slope to be calculated for images in \n"
"projections (e.g., EPSG:4326) which do not use the same units as the \n"
"elevation values (e.g., metres).\n"
"\n"
":param in_dem_img: is a string containing the name and path of the input DEM file.\n"
":param in_pxl_res_img: is a string containing the name and path of the input image.\n"
"                       specifying the image pixel resolutions. Band 1: East-West \n"
"                       resolution and Band 2 is the North-South resolution.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param unit: is a string specifying the output unit ('degrees' or 'radians').\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"},

{"aspect", (PyCFunction)Elevation_calcAspect, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.aspect(input_img, output_img, gdalformat)\n"
"Calculates a aspect layer given an input elevation model\n"
"\n"
":param input_img: is a string containing the name and path of the input DEM file.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"},

{"aspect_pxl_res_img", (PyCFunction)Elevation_calcAspectPxlResImg, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.aspect_pxl_res_img(in_dem_img, in_pxl_res_img, output_img, gdalformat)\n"
"Calculates a aspect layer given an input elevation model and external pixel \n"
"resolution image, which allows the slope to be calculated for images in \n"
"projections (e.g., EPSG:4326) which do not use the same units as the \n"
"elevation values (e.g., metres).\n"
"\n"
":param in_dem_img: is a string containing the name and path of the input DEM file.\n"
":param in_pxl_res_img: is a string containing the name and path of the input image.\n"
"                       specifying the image pixel resolutions. Band 1: East-West \n"
"                       resolution and Band 2 is the North-South resolution.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"},
    
{"catagorise_aspect", (PyCFunction)Elevation_catAspect, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.catagorise_aspect(input_img, output_img, gdalformat)\n"
"Creates an aspect layer which is categorised into 8 catergories from 0-45, \n"
"45-90, 90-135, 135-180, 180-225, 225-270, 270-315 and 315-360.\n"
"\n"
":param input_img: is a string containing the name and path of the input aspect (in degrees) file.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"},
    
{"hillshade", (PyCFunction)Elevation_calcHillshade, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.hillshade(input_img, output_img, azimuth, zenith, gdalformat)\n"
"Calculates a hillshade layer given an input elevation model\n"
"\n"
":param input_img: is a string containing the name and path of the input DEM file.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param azimuth: is a float with the solar azimuth in degrees (Good value is 315).\n"
":param zenith: is a float with the solar zenith in degrees (Good value is 45).\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"},

{"hillshade_pxl_res_img", (PyCFunction)Elevation_calcHillshadePxlResImg, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.hillshade_pxl_res_img(in_dem_img, in_pxl_res_img, output_img, azimuth, zenith, gdalformat)\n"
"Calculates a hillshade layer given an input elevation model and external pixel \n"
"resolution image, which allows the slope to be calculated for images in \n"
"projections (e.g., EPSG:4326) which do not use the same units as the \n"
"elevation values (e.g., metres).\n"
"\n"
":param in_dem_img: is a string containing the name and path of the input DEM file.\n"
":param in_pxl_res_img: is a string containing the name and path of the input image.\n"
"                       specifying the image pixel resolutions. Band 1: East-West \n"
"                       resolution and Band 2 is the North-South resolution.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param azimuth: is a float with the solar azimuth in degrees (Good value is 315).\n"
":param zenith: is a float with the solar zenith in degrees (Good value is 45).\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"},
    
{"shadow_mask", (PyCFunction)Elevation_calcShadowMask, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.shadow_mask(input_img, output_img, azimuth, zenith, max_height, gdalformat)\n"
"Calculates a shadow mask given an input elevation model\n"
"\n"
":param input_img: is a string containing the name and path of the input DEM file.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param azimuth: is a float with the solar azimuth in degrees.\n"
":param zenith: is a float with the solar zenith in degrees.\n"
":param max_height: is a float with the maximum height for the ray tracing (should be above the maximum elevation within the scene)."
":param gdalformat: is a string with the output image format for the GDAL driver.\n"},
    
    
{"local_incidence_angle", (PyCFunction)Elevation_calcLocalIncidenceAngle, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.local_incidence_angle(input_img, output_img, azimuth, zenith, gdalformat)\n"
"Calculates a local solar incidence angle layer given an input elevation model\n"
"\n"
":param input_img: is a string containing the name and path of the input DEM file.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param azimuth: is a float with the solar azimuth in degrees.\n"
":param zenith: is a float with the solar zenith in degrees.\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"},
    
{"local_existance_angle", (PyCFunction)Elevation_calcLocalExistanceAngle, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.local_existance_angle(input_img, output_img, azimuth, zenith, gdalformat)\n"
"Calculates a local solar existance angle layer given an input elevation model\n"
"\n"
":param input_img: is a string containing the name and path of the input DEM file.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param azimuth: is a float with the solar azimuth in degrees.\n"
":param zenith: is a float with the solar zenith in degrees.\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"},
    
{"dtm_aspect_median_filter", (PyCFunction)Elevation_dtmAspectMedianFilter, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.dtm_aspect_median_filter(in_dem_img, in_aspect_img, output_img, aspect_range, win_h_size, gdalformat)\n"
"Filter the DTM for noise using a median filter with an aspect restriction (i.e., only pixels"
" within the aspect range of the centre pixel will be used within the median filter).\n"
"\n"
":param in_dem_img: is a string containing the name and path of the input DTM file.\n"
":param in_aspect_img: is a string containing the name and path of the input Aspect file (in degrees).\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param aspect_range: is a float with the range of from the centre pixel in degrees.\n"
":param win_h_size: is an integer for half the window size.\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"},

{"fill_dem_soille_gratin_1994", (PyCFunction)Elevation_fillDEMSoilleGratin1994, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.fill_dem_soille_gratin_1994(in_dem_img, in_vld_img, output_img, gdalformat)\n"
"Filter the local minima in a DEM using the Soille and Gratin 1994 algorithm.\n\n"
"Soille, P., and Gratin, C. (1994). An efficient algorithm for drainage network\n"
"extraction on DEMs. J. Visual Communication and Image Representation. 5(2). 181-189.\n"
"\n"
":param in_dem_img: is a string containing the name and path of the input DEM file.\n"
":param in_vld_img: is a string containing the name and path to a binary image specifying the valid data region (1 == valid)\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"
"\n"
".. code:: python\n"
"\n"
"   import rsgislib.elevation\n"
"   inputDEMImage = 'DEM.kea'\n"
"   validMaskImage = 'ValidRegionMask.kea'\n"
"   outFilledImage = 'DEM_filled.kea'\n"
"   rsgislib.elevation.fill_dem_soille_gratin_1994(inputDEMImage, validMaskImage, outFilledImage, 'KEA')\n"
"\n"
},
    
{"plane_fit_detreat_dem", (PyCFunction)Elevation_planeFitDetreadDEM, METH_VARARGS | METH_KEYWORDS,
"rsgislib.elevation.plane_fit_detreat_dem(input_img, output_img, gdalformat, win_size)\n"
"An algorithm to detread a DEM using local plane fitting. The winSize will define the scale\n"
"at which the DEM will be detreaded.\n"
"\n"
":param input_img: is a string containing the name and path of the input DEM file.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"
":param win_size: is an integer with the window size within which the plane is fitted.\n"
"\n"
".. code:: python\n"
"\n"
"   import rsgislib.elevation\n"
"   inputDEMImage = 'DEM.kea'\n"
"   outDEMImage = 'DEM_Detread.kea'\n"
"   rsgislib.elevation.plane_fit_detreat_dem(inputDEMImage, outDEMImage, 'KEA', 11)\n"
"\n"
},
    
    {nullptr}        /* Sentinel */
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
        nullptr,
        sizeof(struct ElevationState),
        ElevationMethods,
        nullptr,
        Elevation_traverse,
        Elevation_clear,
        nullptr
};

#define INITERROR return nullptr

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
    if( pModule == nullptr )
        INITERROR;

    struct ElevationState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_elevation.error", nullptr, nullptr);
    if( state->error == nullptr )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
