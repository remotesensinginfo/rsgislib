/*
 *  imageregistration.cpp
 *  RSGIS_LIB
 *
 *  Created by Dan Clewley on 08/09/2013.
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
#include "cmds/RSGISCmdImageRegistration.h"

/* An exception object for this module */
/* created in the init function */
struct ImageRegistrationState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ImageRegistrationState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ImageRegistrationState _state;
#endif

static PyObject *ImageRegistration_BasicRegistration(PyObject *self, PyObject *args)
{
    const char *pszInputReferenceImage, *pszInputFloatingmage, *pszOutputGCPFile;
    int pixelGap, windowSize, searchArea, subPixelResolution, metricType, outputType;
    float threshold, stdDevRefThreshold, stdDevFloatThreshold;
    
    if( !PyArg_ParseTuple(args, "ssifiiffiiis:basicregistration", &pszInputReferenceImage, &pszInputFloatingmage, &pixelGap, 
                                &threshold, &windowSize, &searchArea, &stdDevRefThreshold, &stdDevFloatThreshold, &subPixelResolution, 
                                &metricType, &outputType, &pszOutputGCPFile))
        return NULL;

    try
    {
        rsgis::cmds:: excecuteBasicRegistration(pszInputReferenceImage, pszInputFloatingmage, pixelGap,
                                    threshold, windowSize, searchArea, stdDevRefThreshold,
                                    stdDevFloatThreshold, subPixelResolution, metricType,
                                    outputType, pszOutputGCPFile);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageRegistration_SingleLayerRegistration(PyObject *self, PyObject *args)
{
    const char *pszInputReferenceImage, *pszInputFloatingmage, *pszOutputGCPFile;
    int pixelGap, windowSize, searchArea, subPixelResolution, metricType, 
        outputType, maxNumIterations, distanceThreshold;
    float threshold, stdDevRefThreshold, stdDevFloatThreshold, moveChangeThreshold,
        pSmoothness;
    
    if( !PyArg_ParseTuple(args, "ssifiiffiiiffiis:singlelayerregistration", &pszInputReferenceImage, &pszInputFloatingmage, &pixelGap, 
                                &threshold, &windowSize, &searchArea, &stdDevRefThreshold, &stdDevFloatThreshold, &subPixelResolution,
                                &distanceThreshold, &maxNumIterations, &moveChangeThreshold, &pSmoothness,
                                &metricType, &outputType, &pszOutputGCPFile))
        return NULL;

    try
    {
        rsgis::cmds:: excecuteSingleLayerConnectedRegistration(pszInputReferenceImage, pszInputFloatingmage, pixelGap,
                                    threshold, windowSize, searchArea, stdDevRefThreshold,
                                    stdDevFloatThreshold, subPixelResolution, distanceThreshold,
                                    maxNumIterations, moveChangeThreshold, pSmoothness, metricType,
                                    outputType, pszOutputGCPFile);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageRegistration_TriangularWarp(PyObject *self, PyObject *args)
{
	const char *pszInputImage, *pszInputGCPFile, *pszOutputFile, *pszProjFile, *pszGDALFormat;
	float nResolution;
	int genTransformImage = false;
    
    if( !PyArg_ParseTuple(args, "ssssfs|i:triangularwarp", &pszInputImage, &pszInputGCPFile, &pszOutputFile, &pszProjFile, 
                        &nResolution, &pszGDALFormat, &genTransformImage))
        return NULL;

    try
    {
        rsgis::cmds::excecuteTriangularWarp(pszInputImage, pszOutputFile, pszProjFile, pszInputGCPFile,
                        nResolution, pszGDALFormat, genTransformImage);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageRegistration_NNWarp(PyObject *self, PyObject *args)
{
	const char *pszInputImage, *pszInputGCPFile, *pszOutputFile, *pszProjFile, *pszGDALFormat;
	float nResolution;
	int genTransformImage = false;
    
    if( !PyArg_ParseTuple(args, "ssssfs|i:nnwarp", &pszInputImage, &pszInputGCPFile, &pszOutputFile, &pszProjFile, 
                        &nResolution, &pszGDALFormat, &genTransformImage))
        return NULL;

    try
    {
        rsgis::cmds::excecuteNNWarp(pszInputImage, pszOutputFile, pszProjFile, pszInputGCPFile,
                        nResolution, pszGDALFormat, genTransformImage);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageRegistration_PolyWarp(PyObject *self, PyObject *args)
{
	const char *pszInputImage, *pszInputGCPFile, *pszOutputFile, *pszProjFile, *pszGDALFormat;
	float nResolution;
	int nPolyOrder;
	int genTransformImage = false;
    
    if( !PyArg_ParseTuple(args, "ssssfis|i:polywarp", &pszInputImage, &pszInputGCPFile, &pszOutputFile, &pszProjFile, 
                        &nResolution, &nPolyOrder, &pszGDALFormat, &genTransformImage))
        return NULL;

    try
    {
        rsgis::cmds::excecutePolyWarp(pszInputImage, pszOutputFile, pszProjFile, pszInputGCPFile,
                        nResolution, nPolyOrder, pszGDALFormat, genTransformImage);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageRegistration_GCP2GDAL(PyObject *self, PyObject *args)
{
	const char *pszInputImage, *pszInputGCPFile, *pszOutputFile, *pszGDALFormat;
	int nOutDataType;
    
    if( !PyArg_ParseTuple(args, "ssssi:gcp2gdal", &pszInputImage, &pszInputGCPFile, &pszOutputFile, &pszGDALFormat, &nOutDataType))
        return NULL;

    try
    {
        rsgis::cmds::excecuteAddGCPsGDAL(pszInputImage, pszInputGCPFile, pszOutputFile, pszGDALFormat, (rsgis::RSGISLibDataType) nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageRegistration_ApplyOffset2Image(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImage, *pszGDALFormat;
	int nOutDataType;
    double xOff, yOff;
    
    if( !PyArg_ParseTuple(args, "sssidd:applyOffset2Image", &pszInputImage, &pszOutputImage, &pszGDALFormat, &nOutDataType, &xOff, &yOff))
        return NULL;
    
    try
    {
        rsgis::cmds::executeApplyOffset2Image(std::string(pszInputImage), std::string(pszOutputImage), std::string(pszGDALFormat), (rsgis::RSGISLibDataType) nOutDataType, xOff, yOff);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

// Our list of functions in this module
static PyMethodDef ImageRegistrationMethods[] = {
    {"basicregistration", ImageRegistration_BasicRegistration, METH_VARARGS, 
"imageregistration.basicregistration(reference, floating, pixelGap, threshold, window, search, stddevRef, stddevFloat, subpixelresolution, metric, outputType, output)\n"
"Generate tie points between floating and reference image using basic algorithm.\n"
"\n"
"Where:\n"
"\n"
":param reference: is a string providing reference image which to which the floating image is to be registered.n"
":param floating: is a string providing the floating image to be registered to the reference image\n"
":param pixelGap: is an int specifying the gap, in image pixels, between the initial tie points (this is for both the x and y axis) \n"
":param threshold: is a float providing the threshold for the image metric above/below (depending on image metric) which matching is consider insufficient to be reliable and therefore the match will be ignored.\n"
":param window: is an int providing the size of the window around each tie point which will be used for the matching \n"
":param search: is an int providing the distance (in pixels) from the tie point start point which will be searched.\n"
":param stddevRef: is a float which defines the standard deviation for the window around each tie point below which it is deemed there is insufficient information to perform a match \n"
":param stddevFloat: is a float which defines the standard deviation for the window around each tie point below which it is deemed there is insufficient information to perform a match. Note, that the tie point window has to be below the threshold for both the reference and floating image to be ignored\n"
":param subpixelresolution: is an int specifying the sub-pixel resolution to which the pixel shifts are estimated. Note that the values are positive integers such that a value of 2 will result in a sub pixel resolution of 0.5 of a pixel and a value 4 will be 0.25 of a pixel. \n"
":param metric: is an the similarity metric used to compare images of type rsgislib.imageregistration.METRIC_* \n"
":param outputType: is an the format of the output file of type rsgislib.imageregistration.TYPE_* \n"
":param output: is a string giving specifying the output file, containing the generated tie points\n"
"\n"
"Example::\n"
"\n"
"   reference = 'ref.kea'\n"        
"   floating = 'float.kea'\n"
"   pixelGap = 50\n"
"   threshold = 0.4\n"
"   window = 100\n"
"   search = 5\n"
"   stddevRef = 2\n"
"   stddevFloat = 2\n"
"   subpixelresolution = 4\n"
"   metric = imageregistration.METRIC_CORELATION\n"
"   outputType = imageregistration.TYPE_RSGIS_IMG2MAP\n"
"   output = './TestOutputs/injune_p142_casi_sub_utm_tie_points.txt'\n"
"   imageregistration.basicregistration(reference, floating, pixelGap, threshold, window, search, stddevRef, stddevFloat, subpixelresolution, metric, outputType, output)\n"
"\n"
},

    {"singlelayerregistration", ImageRegistration_SingleLayerRegistration, METH_VARARGS, 
"imageregistration.singlelayerregistration(reference, floating, pixelGap, threshold, window, search, stddevRef, stddevFloat, subpixelresolution, distanceThreshold, maxiterations, movementThreshold, pSmoothness, metric, outputType, output)\n"
"Generate tie points between floating and reference image using a single connected layer of tie points.\n"
"\n"
"Where:\n"
"\n"
":param reference: is a string providing reference image which to which the floating image is to be registered.n"
":param floating: is a string providing the floating image to be registered to the reference image\n"
":param pixelGap: is an int specifying the gap, in image pixels, between the initial tie points (this is for both the x and y axis) which matching is consider insufficient to be reliable and therefore the match will be ignored.\n"
":param window: is an int providing the size of the window around each tie point which will be used for the matching \n"
":param search: is an int providing the distance (in pixels) from the tie point start point which will be searched.\n"
":param stddevRef: is a float which defines the standard deviation for the window around each tie point below which it is deemed there is insufficient information to perform a match \n"
":param stddevFloat: is a float which defines the standard deviation for the window around each tie point below which it is deemed there is insufficient information to perform a match. Note, that the tie point window has to be below the threshold for both the reference and floating image to be ignored\n"
":param subpixelresolution: is an int specifying the sub-pixel resolution to which the pixel shifts are estimated. Note that the values are positive integers such that a value of 2 will result in a sub pixel resolution of 0.5 of a pixel and a value 4 will be 0.25 of a pixel. \n"
":param distanceThreshold: is an int giving the distance (in pixels) to be connected within the layer.\n"
":param maxiterations: is an int giving the maximum number of iterations of the tie point grid to find an optimal set of tie points\n"
":param movementThreshold: is a float providing the threshold for the average amount of tie point movement for the optimisation to be terminated\n"
":param pSmoothness: is a float providing the 'p' parameter for the inverse weighted distance calculation. A value of 2 should be used by default\n"
":param metric: is an the similarity metric used to compare images of type rsgislib.imageregistration.METRIC_* \n"
":param outputType: is an the format of the output file of type rsgislib.imageregistration.TYPE_* \n"
":param output: is a string giving specifying the output file, containing the generated tie points\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import imageregistration\n"
"   reference = 'ref.kea'\n"        
"   floating = 'float.kea'\n"
"   pixelGap = 50\n"
"   threshold = 0.4\n"
"   window = 100\n"
"   search = 5\n"
"   stddevRef = 2\n"
"   stddevFloat = 2\n"
"   subpixelresolution = 4\n"
"   metric = imageregistration.METRIC_CORELATION\n"
"   outputType = imageregistration.TYPE_RSGIS_IMG2MAP\n"
"   output = './TestOutputs/injune_p142_casi_sub_utm_tie_points.txt'\n"
"   imageregistration.basicregistration(reference, floating, pixelGap, threshold, window, search, stddevRef, stddevFloat, subpixelresolution, metric, outputType, output)\n"
"\n"
},

    {"triangularwarp", ImageRegistration_TriangularWarp, METH_VARARGS, 
"imageregistration.triangularwarp(inputimage, inputgcps, outputimage, wktStringFile, resolution, gdalformat, transformImage=False)\n"
"Warp image from tie points using triangular interpolation.\n"
"\n"
"Where:\n"
"\n"
":param inputimage: is a string providing the input image.\n"
":param inputgcps: is a string providing the input text file containing the tie points.\n"
":param outputimage: is a string providing the output image.\n"
":param wktStringFile: is a file path to a text file containing the WKT string to use for the warped image.\n"
":param resolution: is a float providing the resolution of the output file\n"
":param gdalformat: is a string providing the output format (e.g., KEA).\n"
":param transformImage: is a bool, if set to true will generate an image providing the transform for each pixel, rather than warping the input image \n"
"\n"
"Example::\n"
"\n"
"    from rsgislib import imageregistration\n"
"    inputImage = './Rasters/injune_p142_casi_sub_utm_single_band_offset3x3y.vrt'\n"
"    inputGCPs = './TestOutputs/injune_p142_casi_sub_utm_tie_points_basic.txt'\n"
"    outputImage = './TestOutputs/injune_p142_casi_sub_utm_single_band_offset3x3y_twarp.kea'\n"
"    wktStringFile = './Vectors/injune_p142_crowns_utm.prj'\n"
"    resolution = 1\n"
"    gdalformat = 'KEA'\n"
"    imageregistration.triangularwarp(inputImage,inputGCPs, outputImage, wktStringFile, resolution, gdalformat)\n"
"\n"
},  

    {"nnwarp", ImageRegistration_NNWarp, METH_VARARGS, 
"imageregistration.nnwarp(inputimage, inputgcps, outputimage, wktStringFile, resolution, gdalformat, transformImage=False)\n"
"Warp image from tie points using a nearest neighbour interpolation.\n"
"\n"
"Where:\n"
"\n"
":param inputimage: is a string providing the input image.\n"
":param inputgcps: is a string providing the input text file containing the tie points.\n"
":param outputimage: is a string providing the output image.\n"
":param wktStringFile: is a file path to a text file containing the WKT string to use for the warped image.\n"
":param resolution: is a float providing the resolution of the output file\n"
":param gdalformat: is a string providing the output format (e.g., KEA).\n"
":param transformImage: is a bool, if set to true will generate an image providing the transform for each pixel, rather than warping the input image \n"
"\n"
"Example::\n"
"\n"
"    from rsgislib import imageregistration\n"
"    inputImage = './Rasters/injune_p142_casi_sub_utm_single_band_offset3x3y.vrt'\n"
"    inputGCPs = './TestOutputs/injune_p142_casi_sub_utm_tie_points_basic.txt'\n"
"    outputImage = './TestOutputs/injune_p142_casi_sub_utm_single_band_offset3x3y_nnwarp.kea'\n"
"    wktStringFile = './Vectors/injune_p142_crowns_utm.prj'\n"
"    resolution = 1\n"
"    gdalformat = 'KEA'\n"
"    imageregistration.nnwarp(inputImage,inputGCPs, outputImage, wktStringFile, resolution, gdalformat)\n"
"\n"
},  

    {"polywarp", ImageRegistration_PolyWarp, METH_VARARGS, 
"imageregistration.polywarp(inputimage, inputgcps, outputimage, wktStringFile, resolution, polyOrder, gdalformat, transformImage=False)\n"
"Warp image from tie points using a polynomial interpolation.\n"
"\n"
"Where:\n"
"\n"
":param inputimage: is a string providing the input image.\n"
":param inputgcps: is a string providing the input text file containing the tie points.\n"
":param outputimage: is a string providing the output image.\n"
":param wktStringFile: is a file path to a text file containing the WKT string to use for the warped image.\n"
":param resolution: is a float providing the resolution of the output file\n"
":param polyOrder: is an int specifying the order of polynomial to use.\n"
":param gdalformat: is a string providing the output format (e.g., KEA).\n"
":param transformImage: is a bool, if set to true will generate an image providing the transform for each pixel, rather than warping the input image \n"
"\n"
"Example::\n"
"\n"
"    from rsgislib import imageregistration\n"
"    inputImage = './Rasters/injune_p142_casi_sub_utm_single_band_offset3x3y.vrt'\n"
"    inputGCPs = './TestOutputs/injune_p142_casi_sub_utm_tie_points_basic.txt'\n"
"    outputImage = './TestOutputs/injune_p142_casi_sub_utm_single_band_offset3x3y_polywarp.kea'\n"
"    wktStringFile = './Vectors/injune_p142_crowns_utm.prj'\n"
"    resolution = 1\n"
"    polyOrder = 3\n"
"    gdalformat = 'KEA'\n"
"    imageregistration.polywarp(inputImage,inputGCPs, outputImage, wktStringFile, resolution, polyOrder, gdalformat)\n"
"\n"
},  

    {"gcp2gdal", ImageRegistration_GCP2GDAL, METH_VARARGS, 
"imageregistration.gcp2gdal(inputimage, inputgcps, outputimage, gdalformat, datatype)\n"
"Adds tie points to GDAL file, suitable for warping using the gdalwarp command.\n"
"\n"
"Where:\n"
"\n"
":param inputimage: is a string providing the input image.\n"
":param inputgcps: is a string providing the input text file containing the tie points.\n"
":param outputimage: is a string providing the output image.\n"
":param gdalformat: is a string providing the output format (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the output data type.\n"
"\n"
"Example::\n"
"\n"
"    from rsgislib import imageregistration\n"
"    inputImage = './Rasters/injune_p142_casi_sub_utm_single_band_offset3x3y.vrt'\n"
"    inputGCPs = './TestOutputs/injune_p142_casi_sub_utm_tie_points_basic.txt'\n"
"    outputImage = './TestOutputs/injune_p142_casi_sub_utm_single_band_offset3x3y_gcps.kea'\n"
"    gdalformat = 'KEA'\n"
"    datatype = rsgislib.TYPE_32INT\n"
"    imageregistration.gcp2gdal(inputImage,inputGCPs, outputImage, gdalformat, gdaltype)\n"
"\n"
},
    
{"applyOffset2Image", ImageRegistration_ApplyOffset2Image, METH_VARARGS,
"imageregistration.applyOffset2Image(inputImage, outputImage, gdalformat, gdaltype, xOff, yOff)\n"
"Apply a linear X,Y offset to the image header - does not change the pixel values.\n"
"\n"
"Where:\n"
"\n"
":param inputImage: is a string providing the input image.\n"
":param outputImage: is a string providing the output image.\n"
":param gdalformat: is a string providing the output format (e.g., KEA).\n"
":param gdaltype: is a rsgislib.TYPE_* value providing the output data type.\n"
":param xOff: is a float specifying the X offset to be applied to the image.\n"
":param yOff: is a float specifying the Y offset to be applied to the image.\n"
"\n"
"Example::\n"
"\n"
"    from rsgislib import imageregistration\n"
"    inputImage = './Rasters/injune_p142_casi_sub_utm_single_band_offset3x3y.vrt'\n"
"    outputImage = './TestOutputs/injune_p142_casi_sub_utm_single_band_offset3x3y_fixed.kea'\n"
"    gdalformat = 'KEA'\n"
"    datatype = rsgislib.TYPE_32INT\n"
"    imageregistration.applyOffset2Image(inputImage, outputImage, gdalformat, datatype, -3.0, -3.0)\n"
"\n"
},
    
	{NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int ImageRegistration_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int ImageRegistration_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_imageregistration",
        NULL,
        sizeof(struct ImageRegistrationState),
        ImageRegistrationMethods,
        NULL,
        ImageRegistration_traverse,
        ImageRegistration_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__imageregistration(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_imageregistration(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_imageregistration", ImageRegistrationMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ImageRegistrationState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_imageregistration.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
