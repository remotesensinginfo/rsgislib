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

static PyObject *ImageRegistration_FindImageOffset(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_ref_img"), RSGIS_PY_C_TEXT("in_float_img"),
                             RSGIS_PY_C_TEXT("ref_img_bands"), RSGIS_PY_C_TEXT("flt_img_bands"),
                             RSGIS_PY_C_TEXT("metric_type"), RSGIS_PY_C_TEXT("x_search"),
                             RSGIS_PY_C_TEXT("y_search"), RSGIS_PY_C_TEXT("sub_pxl_res"),  nullptr};
    const char *pszInputRefImage, *pszInputFloatImage;
    int subPixelResolution = 0;
    int metricType = 0;
    int xImgSearch, yImgSearch = 0;
    PyObject *pRefImageBandsObj;
    PyObject *pFltImageBandsObj;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssOOiii|i:find_image_offset", kwlist, &pszInputRefImage,
                                     &pszInputFloatImage, &pRefImageBandsObj, &pFltImageBandsObj, &metricType,
                                     &xImgSearch, &yImgSearch, &subPixelResolution))
    {
        return nullptr;
    }

    std::vector<unsigned int> refImageBands;
    if( !PySequence_Check(pRefImageBandsObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "ref_img_bands argument must be a sequence");
        return nullptr;
    }
    Py_ssize_t nRefImgBands = PySequence_Size(pRefImageBandsObj);
    for(Py_ssize_t n = 0; n < nRefImgBands; ++n)
    {
        PyObject *o = PySequence_GetItem(pRefImageBandsObj, n);
        refImageBands.push_back(RSGISPY_UINT_EXTRACT(o));
    }

    std::vector<unsigned int> fltImageBands;
    if( !PySequence_Check(pFltImageBandsObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "flt_img_bands argument must be a sequence");
        return nullptr;
    }
    Py_ssize_t nFltImgBands = PySequence_Size(pFltImageBandsObj);
    for(Py_ssize_t n = 0; n < nFltImgBands; ++n)
    {
        PyObject *o = PySequence_GetItem(pFltImageBandsObj, n);
        fltImageBands.push_back(RSGISPY_UINT_EXTRACT(o));
    }

    PyObject *outVal = PyTuple_New(2);
    try
    {
        std::pair<double, double> offsets = rsgis::cmds::excecuteFindImageOffset(std::string(pszInputRefImage),
                                                                                 std::string(pszInputFloatImage),
                                                                                 refImageBands, fltImageBands,
                                                                                 xImgSearch, yImgSearch,
                                                                                 metricType, subPixelResolution);

        if(PyTuple_SetItem(outVal, 0, Py_BuildValue("d", offsets.first)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'X Offset\' value to the list...");
        }

        if(PyTuple_SetItem(outVal, 1, Py_BuildValue("d", offsets.second)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'Y Offset\' value to the list...");
        }
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    return outVal;
}


static PyObject *ImageRegistration_BasicRegistration(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_ref_img"), RSGIS_PY_C_TEXT("in_float_img"),
                             RSGIS_PY_C_TEXT("out_gcp_file"), RSGIS_PY_C_TEXT("pixel_gap"),
                             RSGIS_PY_C_TEXT("threshold"), RSGIS_PY_C_TEXT("win_size"),
                             RSGIS_PY_C_TEXT("search_area"), RSGIS_PY_C_TEXT("sd_ref_thres"),
                             RSGIS_PY_C_TEXT("sd_flt_thres"), RSGIS_PY_C_TEXT("sub_pxl_res"),
                             RSGIS_PY_C_TEXT("metric_type"), RSGIS_PY_C_TEXT("output_type"),  nullptr};
    const char *pszInputReferenceImage, *pszInputFloatingmage, *pszOutputGCPFile;
    int pixelGap, windowSize, searchArea, subPixelResolution, metricType, outputType;
    float threshold, stdDevRefThreshold, stdDevFloatThreshold;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssifiiffiii:basic_registration", kwlist, &pszInputReferenceImage, &pszInputFloatingmage,
                                     &pszOutputGCPFile, &pixelGap, &threshold, &windowSize, &searchArea, &stdDevRefThreshold,
                                     &stdDevFloatThreshold, &subPixelResolution, &metricType, &outputType))
    {
        return nullptr;
    }

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
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageRegistration_SingleLayerRegistration(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_ref_img"), RSGIS_PY_C_TEXT("in_float_img"),
                             RSGIS_PY_C_TEXT("out_gcp_file"), RSGIS_PY_C_TEXT("pixel_gap"),
                             RSGIS_PY_C_TEXT("threshold"), RSGIS_PY_C_TEXT("win_size"),
                             RSGIS_PY_C_TEXT("search_area"), RSGIS_PY_C_TEXT("sd_ref_thres"),
                             RSGIS_PY_C_TEXT("sd_flt_thres"), RSGIS_PY_C_TEXT("sub_pxl_res"),
                             RSGIS_PY_C_TEXT("dist_threshold"), RSGIS_PY_C_TEXT("max_n_iters"),
                             RSGIS_PY_C_TEXT("move_chng_thres"), RSGIS_PY_C_TEXT("p_smooth"),
                             RSGIS_PY_C_TEXT("metric_type"), RSGIS_PY_C_TEXT("output_type"),  nullptr};
    const char *pszInputReferenceImage, *pszInputFloatingmage, *pszOutputGCPFile;
    int pixelGap, windowSize, searchArea, subPixelResolution, metricType, 
        outputType, maxNumIterations, distanceThreshold;
    float threshold, stdDevRefThreshold, stdDevFloatThreshold, moveChangeThreshold,
        pSmoothness;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssifiiffiiiffii:single_layer_registration", kwlist, &pszInputReferenceImage,
                                     &pszInputFloatingmage, &pszOutputGCPFile, &pixelGap, &threshold, &windowSize, &searchArea,
                                     &stdDevRefThreshold, &stdDevFloatThreshold, &subPixelResolution, &distanceThreshold,
                                     &maxNumIterations, &moveChangeThreshold, &pSmoothness, &metricType, &outputType))
    {
        return nullptr;
    }

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
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageRegistration_GCP2GDAL(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("in_gcp_file"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
	const char *pszInputImage, *pszInputGCPFile, *pszOutputFile, *pszGDALFormat;
	int nOutDataType;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssi:gcp_to_gdal", kwlist, &pszInputImage, &pszInputGCPFile, &pszOutputFile, &pszGDALFormat, &nOutDataType))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::excecuteAddGCPsGDAL(pszInputImage, pszInputGCPFile, pszOutputFile, pszGDALFormat, (rsgis::RSGISLibDataType) nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageRegistration_ApplyOffset2Image(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("x_offset"), RSGIS_PY_C_TEXT("y_offset"), nullptr};
    const char *pszInputImage, *pszOutputImage, *pszGDALFormat;
	int nOutDataType;
    double xOff, yOff;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssidd:apply_offset_to_image", kwlist, &pszInputImage, &pszOutputImage, &pszGDALFormat, &nOutDataType, &xOff, &yOff))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeApplyOffset2Image(std::string(pszInputImage), std::string(pszOutputImage), std::string(pszGDALFormat), (rsgis::RSGISLibDataType) nOutDataType, xOff, yOff);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

// Our list of functions in this module
static PyMethodDef ImageRegistrationMethods[] = {
{"find_image_offset", (PyCFunction)ImageRegistration_FindImageOffset, METH_VARARGS | METH_KEYWORDS,
"imageregistration.find_image_offset(in_ref_img, in_float_img, ref_img_bands, flt_img_bands, metric_type, x_search,  y_search, sub_pxl_res)\n"
"Calculate and X/Y offset between the input reference and float images.\n"
"This function will calculate the similarity intersecting regions of the\n"
"two images and identified an X/Y where the similarity is greatest.\n"
"\n"
"Where:\n"
"\n"
":param in_ref_img: is a string providing reference image which to which the floating image is to be registered.n"
":param in_float_img: is a string providing the floating image to be registered to the reference image\n"
":param ref_img_bands: is a list of image bands which are to be used to calculate the image similarity from the reference image.\n"
":param flt_img_bands: is a list of image bands which are to be used to calculate the image similarity from the floating image.\n"
":param metric_type: is an the similarity metric used to compare images of type rsgislib.imageregistration.METRIC_* \n"
":param x_search: is the number of pixels in the x-axis the image can be moved either side of the centre.\n"
":param y_search: is the number of pixels in the y-axis the image can be moved either side of the centre.\n"
":param sub_pxl_res: is an optional (if not specified then no sub-pixel component will be estimated) int specifying the sub-pixel resolution to which the pixel shifts are estimated. Note that the values are positive integers such that a value of 2 will result in a sub pixel resolution of 0.5 of a pixel and a value 4 will be 0.25 of a pixel. \n"
"\n"
"\n"
},

{"basic_registration", (PyCFunction)ImageRegistration_BasicRegistration, METH_VARARGS | METH_KEYWORDS,
"imageregistration.basic_registration(in_ref_img, in_float_img, out_gcp_file, pixel_gap, threshold, win_size, search_area, sd_ref_thres, sd_flt_thres, sub_pxl_res, metric_type, output_type)\n"
"Generate tie points between floating and reference image using basic algorithm.\n"
"\n"
"Where:\n"
"\n"
":param in_ref_img: is a string providing reference image which to which the floating image is to be registered.n"
":param in_float_img: is a string providing the floating image to be registered to the reference image\n"
":param out_gcp_file: is a string giving specifying the output file, containing the generated tie points\n"
":param pixel_gap: is an int specifying the gap, in image pixels, between the initial tie points (this is for both the x and y axis) \n"
":param threshold: is a float providing the threshold for the image metric above/below (depending on image metric) which matching is consider insufficient to be reliable and therefore the match will be ignored.\n"
":param win_size: is an int providing the size of the window around each tie point which will be used for the matching \n"
":param search_area: is an int providing the distance (in pixels) from the tie point start point which will be searched.\n"
":param sd_ref_thres: is a float which defines the standard deviation for the window around each tie point below which it is deemed there is insufficient information to perform a match \n"
":param sd_flt_thres: is a float which defines the standard deviation for the window around each tie point below which it is deemed there is insufficient information to perform a match. Note, that the tie point window has to be below the threshold for both the reference and floating image to be ignored\n"
":param sub_pxl_res: is an int specifying the sub-pixel resolution to which the pixel shifts are estimated. Note that the values are positive integers such that a value of 2 will result in a sub pixel resolution of 0.5 of a pixel and a value 4 will be 0.25 of a pixel. \n"
":param metric_type: is an the similarity metric used to compare images of type rsgislib.imageregistration.METRIC_* \n"
":param output_type: is an the format of the output file of type rsgislib.imageregistration.TYPE_* \n"
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
"   imageregistration.basic_registration(reference, floating, pixelGap, threshold, window, search, stddevRef, stddevFloat, subpixelresolution, metric, outputType, output)\n"
"\n"
},

    {"single_layer_registration", (PyCFunction)ImageRegistration_SingleLayerRegistration, METH_VARARGS | METH_KEYWORDS,
"imageregistration.single_layer_registration(in_ref_img, in_float_img, out_gcp_file, pixel_gap, threshold, win_size, search_area, sd_ref_thres, sd_flt_thres, sub_pxl_res, dist_threshold, max_n_iters, move_chng_thres, p_smooth, metric_type, output_type)\n"
"Generate tie points between floating and reference image using a single connected layer of tie points.\n"
"\n"
"Where:\n"
"\n"
":param in_ref_img: is a string providing reference image which to which the floating image is to be registered.n"
":param in_float_img: is a string providing the floating image to be registered to the reference image\n"
":param out_gcp_file: is a string giving specifying the output file, containing the generated tie points\n"
":param pixel_gap: is an int specifying the gap, in image pixels, between the initial tie points (this is for both the x and y axis) which matching is consider insufficient to be reliable and therefore the match will be ignored.\n"
":param threshold: is a float providing the threshold for the image metric above/below (depending on image metric) which matching is consider insufficient to be reliable and therefore the match will be ignored.\n"
":param win_size: is an int providing the size of the window around each tie point which will be used for the matching \n"
":param search_area: is an int providing the distance (in pixels) from the tie point start point which will be searched.\n"
":param sd_ref_thres: is a float which defines the standard deviation for the window around each tie point below which it is deemed there is insufficient information to perform a match \n"
":param sd_flt_thres: is a float which defines the standard deviation for the window around each tie point below which it is deemed there is insufficient information to perform a match. Note, that the tie point window has to be below the threshold for both the reference and floating image to be ignored\n"
":param sub_pxl_res: is an int specifying the sub-pixel resolution to which the pixel shifts are estimated. Note that the values are positive integers such that a value of 2 will result in a sub pixel resolution of 0.5 of a pixel and a value 4 will be 0.25 of a pixel. \n"
":param dist_threshold: is an int giving the distance (in pixels) to be connected within the layer.\n"
":param max_n_iters: is an int giving the maximum number of iterations of the tie point grid to find an optimal set of tie points\n"
":param move_chng_thres: is a float providing the threshold for the average amount of tie point movement for the optimisation to be terminated\n"
":param p_smooth: is a float providing the 'p' parameter for the inverse weighted distance calculation. A value of 2 should be used by default\n"
":param metric_type: is an the similarity metric used to compare images of type rsgislib.imageregistration.METRIC_* \n"
":param output_type: is an the format of the output file of type rsgislib.imageregistration.TYPE_* \n"
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
"   imageregistration.single_layer_registration(reference, floating, pixelGap, threshold, window, search, stddevRef, stddevFloat, subpixelresolution, metric, outputType, output)\n"
"\n"
},


    {"gcp_to_gdal", (PyCFunction)ImageRegistration_GCP2GDAL, METH_VARARGS | METH_KEYWORDS,
"imageregistration.gcp_to_gdal(input_img, in_gcp_file, output_img, gdalformat, datatype)\n"
"Adds tie points to GDAL file, suitable for warping using the gdalwarp command.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string providing the input image.\n"
":param in_gcp_file: is a string providing the input text file containing the tie points.\n"
":param output_img: is a string providing the output image.\n"
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
"    imageregistration.gcp_to_gdal(inputImage,inputGCPs, outputImage, gdalformat, gdaltype)\n"
"\n"
},
    
{"apply_offset_to_image", (PyCFunction)ImageRegistration_ApplyOffset2Image, METH_VARARGS | METH_KEYWORDS,
"imageregistration.apply_offset_to_image(input_img, output_img, gdalformat, datatype, x_offset, y_offset)\n"
"Apply a linear X,Y offset to the image header - does not change the pixel values.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string providing the input image.\n"
":param output_img: is a string providing the output image.\n"
":param gdalformat: is a string providing the output format (e.g., KEA).\n"
":param datatype: is a rsgislib.TYPE_* value providing the output data type.\n"
":param x_offset: is a float specifying the X offset to be applied to the image.\n"
":param y_offset: is a float specifying the Y offset to be applied to the image.\n"
"\n"
"Example::\n"
"\n"
"    from rsgislib import imageregistration\n"
"    inputImage = './Rasters/injune_p142_casi_sub_utm_single_band_offset3x3y.vrt'\n"
"    outputImage = './TestOutputs/injune_p142_casi_sub_utm_single_band_offset3x3y_fixed.kea'\n"
"    gdalformat = 'KEA'\n"
"    datatype = rsgislib.TYPE_32INT\n"
"    imageregistration.apply_offset_to_image(inputImage, outputImage, gdalformat, datatype, -3.0, -3.0)\n"
"\n"
},
    
	{nullptr}        /* Sentinel */
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
        nullptr,
        sizeof(struct ImageRegistrationState),
        ImageRegistrationMethods,
        nullptr,
        ImageRegistration_traverse,
        ImageRegistration_clear,
        nullptr
};

#define INITERROR return nullptr

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
    if( pModule == nullptr )
        INITERROR;

    struct ImageRegistrationState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_imageregistration.error", nullptr, nullptr);
    if( state->error == nullptr )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
