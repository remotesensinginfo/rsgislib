/*
 *  imagefilter.cpp
 *  RSGIS_LIB
 *
 *  Created by Dan Clewley on 09/11/2013.
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
#include "cmds/RSGISCmdFilterImages.h"

/* An exception object for this module */
/* created in the init function */
struct ImageFilterState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ImageFilterState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ImageFilterState _state;
#endif

static void FreePythonObjects(std::vector<PyObject*> toFree) {
    std::vector<PyObject*>::iterator iter;
    for(iter = toFree.begin(); iter != toFree.end(); ++iter) {
        Py_XDECREF(*iter);
    }
}

static PyObject *ImageFilter_Filter(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImageBase;
    const char *pszImageFormat = "KEA";
    const char *pszImageExt = "kea";
    int dataType = 9; // Default to 32 bit float
    PyObject *pImageFilterCmds;
    if( !PyArg_ParseTuple(args, "ssO|ssi:applyfilters", &pszInputImage, &pszOutputImageBase, &pImageFilterCmds, &pszImageFormat, &pszImageExt, &dataType))
        return NULL;

    // extract the attributes from the sequence of objects into our structs
    Py_ssize_t nFilters = PySequence_Size(pImageFilterCmds);
    std::vector<rsgis::cmds::RSGISFilterParameters*> *filterParameters = NULL;
    filterParameters = new std::vector<rsgis::cmds::RSGISFilterParameters*>();
    filterParameters->reserve(nFilters);

    if(nFilters == 1){std::cout << "Found 1 filter:" << std::endl;}
    else{std::cout << "Found " << nFilters << " filters:" << std::endl;}
    for(int i = 0; i < nFilters; ++i) 
    {
        PyObject *o = PySequence_GetItem(pImageFilterCmds, i);     // the python object

        rsgis::cmds::RSGISFilterParameters *cmdObj = new rsgis::cmds::RSGISFilterParameters();   // the c++ object we need to pass pointers of

        // declare and initialise pointers for all the attributes of the struct
        PyObject *pFilterType, *pFileEnding, *pSize, *pOption, *pNLooks, *pStdDev, *pStdDevX , *pStdDevY, *pAngle = NULL;

        std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference
        extractedAttributes.push_back(o);
        
        // Get required fields
        // Filter type
        pFilterType = PyObject_GetAttrString(o, "filterType");
        extractedAttributes.push_back(pFilterType);
        if( ( pFilterType == NULL ) || ( pFilterType == Py_None ) || !RSGISPY_CHECK_STRING(pFilterType) ) 
        {
            PyErr_SetString(GETSTATE(self)->error, "Need to provide filter 'type'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISFilterParameters*>::iterator iter = filterParameters->begin(); iter != filterParameters->end(); ++iter) 
            {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }
        cmdObj->type = RSGISPY_STRING_EXTRACT(pFilterType);
        std::cout << " " << i+1 << ") " << cmdObj->type << ": ";

        // File Ending
        pFileEnding = PyObject_GetAttrString(o, "fileEnding");
        extractedAttributes.push_back(pFileEnding);
        if( ( pFileEnding == NULL ) || ( pFileEnding == Py_None ) || !RSGISPY_CHECK_STRING(pFileEnding) ) 
        {
            PyErr_SetString(GETSTATE(self)->error, "Need to provide 'fileEnding'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISFilterParameters*>::iterator iter = filterParameters->begin(); iter != filterParameters->end(); ++iter) 
            {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }
        cmdObj->fileEnding = RSGISPY_STRING_EXTRACT(pFileEnding);
         
        // Filter size
        pSize = PyObject_GetAttrString(o, "size");
        extractedAttributes.push_back(pSize);
        if( ( pSize == NULL ) || ( pSize == Py_None ) || !RSGISPY_CHECK_INT(pSize) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Need to provide filter 'size'" );
            FreePythonObjects(extractedAttributes);
            for(std::vector<rsgis::cmds::RSGISFilterParameters*>::iterator iter = filterParameters->begin(); iter != filterParameters->end(); ++iter) 
            {
                delete *iter;
            }
            delete cmdObj;
            return NULL;
        }
        if( (cmdObj->type != "Sobel") && (cmdObj->type != "Prewitt") )
        {
            cmdObj->size = RSGISPY_INT_EXTRACT(pSize);
            std::cout << "size = " << cmdObj->size << " ";
        }

        // Check for other fileds and set accordingly
        // TODO: Add checks that required parameters are passed in for each filter.
        pOption = PyObject_GetAttrString(o, "option");
        extractedAttributes.push_back(pOption);
        if( (pOption != NULL) & RSGISPY_CHECK_STRING(pOption) )
        {
            cmdObj->option = RSGISPY_STRING_EXTRACT(pOption);
            std::cout << "option = " << cmdObj->option << " ";
        }

        pNLooks = PyObject_GetAttrString(o, "nLooks");
        extractedAttributes.push_back(pNLooks);
        if( !(pNLooks == NULL) & RSGISPY_CHECK_INT(pNLooks) )
        {
            cmdObj->nLooks = RSGISPY_INT_EXTRACT(pNLooks);
            std::cout << "nLooks = " << cmdObj->nLooks << " ";
        }

        pStdDev = PyObject_GetAttrString(o, "stddev");
        extractedAttributes.push_back(pStdDev);
        if( !(pStdDev == NULL) & (RSGISPY_CHECK_FLOAT(pStdDev) | RSGISPY_CHECK_INT(pStdDev)) )
        {
            cmdObj->stddev = RSGISPY_FLOAT_EXTRACT(pStdDev);
            std::cout << "StdDev = " << cmdObj->stddev << " ";
        }
        
        pStdDevX = PyObject_GetAttrString(o, "stddevX");
        extractedAttributes.push_back(pStdDevX);
        if( !(pStdDevX == NULL) & (RSGISPY_CHECK_FLOAT(pStdDevX) | RSGISPY_CHECK_INT(pStdDevX)) )
        {
            cmdObj->stddevX = RSGISPY_FLOAT_EXTRACT(pStdDevX);
            std::cout << "StdDevX = " << cmdObj->stddevX << " ";
        }

        pStdDevY = PyObject_GetAttrString(o, "stddevY");
        extractedAttributes.push_back(pStdDevY);
        if( !(pStdDevY == NULL) & (RSGISPY_CHECK_FLOAT(pStdDevY) | RSGISPY_CHECK_INT(pStdDevY)) )
        {
            cmdObj->stddevY = RSGISPY_FLOAT_EXTRACT(pStdDevY);
            std::cout << "StdDevY = " << cmdObj->stddevY << " ";
        }

        pAngle = PyObject_GetAttrString(o, "angle");
        extractedAttributes.push_back(pAngle);
        if( !(pAngle == NULL) & RSGISPY_CHECK_FLOAT(pAngle) )
        {
            cmdObj->angle = RSGISPY_FLOAT_EXTRACT(pAngle);
            std::cout << "angle = " << cmdObj->angle << " ";
        }
        std::cout << std::endl;

        FreePythonObjects(extractedAttributes);
        filterParameters->push_back(cmdObj);
     }
  
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType) dataType;
        rsgis::cmds::executeFilter(pszInputImage, filterParameters, pszOutputImageBase, pszImageFormat, pszImageExt, type);

        // Delete filter parameters
        for(std::vector<rsgis::cmds::RSGISFilterParameters*>::iterator iterFilter = filterParameters->begin(); iterFilter != filterParameters->end(); ++iterFilter)
        {
            delete *iterFilter;
        }
        delete filterParameters;

    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageFilter_LeungMalikFilterBank(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImageBase;
    const char *pszImageFormat = "KEA";
    const char *pszImageExt = "kea";
    int dataType = 9; // Default to 32 bit float
    if( !PyArg_ParseTuple(args, "ss|ssi:LeungMalikFilterBank", &pszInputImage, &pszOutputImageBase, &pszImageFormat, &pszImageExt, &dataType))
        return NULL;

    try
    {
        // Set up filter Band
        std::vector<rsgis::cmds::RSGISFilterParameters*> *filterParameters = NULL;
        
        filterParameters = rsgis::cmds::createLeungMalikFilterBank();
        
        std::cout << "Added 48 filters for Leung-Malik Filter Bank " << std::endl;
        
        // Excecute
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType) dataType;
        rsgis::cmds::executeFilter(pszInputImage, filterParameters, pszOutputImageBase, pszImageFormat, pszImageExt, type);

        // Delete filter parameters
        for(std::vector<rsgis::cmds::RSGISFilterParameters*>::iterator iterFilter = filterParameters->begin(); iterFilter != filterParameters->end(); ++iterFilter)
        {
            delete *iterFilter;
        }
        delete filterParameters;

    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

// Our list of functions in this module
static PyMethodDef ImageFilterMethods[] = {
    {"applyfilters", ImageFilter_Filter, METH_VARARGS, 
"imagefilter.applyfilters(inputimage, outputImageBase, filterparameters, gdalformat, outExt, datatype)\n"
"Filters images\n"
"\n"
"Where:\n"
"\n"
":param inputimage: is a string containing the name of the input image\n"
":param outputImageBase: is a string containing the base name of the output images\n"
":param filterparameters: is list of rsgislib.imagefilter.FilterParameters objects providing the type of filter and required parameters (see example)\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imagefilter\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm_single_band.vrt'\n"
"   outputImageBase = './TestOutputs/injune_p142_casi_sub_utm_single_band'\n"
"   gdalformat = 'KEA'\n"
"   outExt = 'kea'\n"
"   datatype = rsgislib.TYPE_32FLOAT\n"
"   filters = []\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'GaussianSmooth', fileEnding = 'gausmooth', size=3, stddevX = 1., stddevY = 1, angle = 0.) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Gaussian1st', fileEnding = 'gau1st', size=3, stddevX = 1., stddevY = 1, angle = 0.) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Gaussian2nd', fileEnding = 'gau1st', size=3, stddevX = 1., stddevY = 1, angle = 0.) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Laplacian', fileEnding = 'laplacian', size=3, stddev = 1) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Sobel', fileEnding = 'sobelx', option = 'x') )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Sobel', fileEnding = 'sobely', option = 'y') )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Sobel', fileEnding = 'sobelxy', option = 'xy') )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Prewitt', fileEnding = 'prewittx', option = 'x') )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Prewitt', fileEnding = 'prewitty', option = 'y') )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Prewitt', fileEnding = 'prewittxy', option = 'xy') )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Mean', fileEnding = 'mean', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Median', fileEnding = 'median', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Mode', fileEnding = 'mode', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'StdDev', fileEnding = 'stddev', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Range', fileEnding = 'range', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'CoeffOfVar', fileEnding = 'coeffofvar', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Min', fileEnding = 'min', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Max', fileEnding = 'max', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Total', fileEnding = 'total', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'MeanDiff', fileEnding = 'meandiff', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'MeanDiffAbs', fileEnding = 'meandiffabs', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'TotalDiff', fileEnding = 'meandiff', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'TotalDiffAbs', fileEnding = 'meandiffabs', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Kuwahara', fileEnding = 'kuwahara', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'Lee', fileEnding = 'lee', size=3, nLooks=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'NormVar', fileEnding = 'normvar', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'NormVarSqrt', fileEnding = 'normvarsqrt', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'NormVarLn', fileEnding = 'normvarln', size=3) )\n"
"   filters.append(imagefilter.FilterParameters(filterType = 'TextureVar', fileEnding = 'texturevar', size=3) )\n"
"   # Apply filters\n"
"   imagefilter.applyfilters(inputImage, outputImageBase, filters, gdalformat, outExt, datatype)\n"
"\n"},

    {"LeungMalikFilterBank", ImageFilter_LeungMalikFilterBank, METH_VARARGS, 
"imagefilter.(inputimage, outputImageBase, gdalformat, outExt, datatype)\n"
"Implements the Leung-Malik filter bank described in:\n"
"Leung, T., Malik, J., 2001. Representing and recognizing the visual appearance of materials using three-dimensional textons.\n"
"International Journal of Computer Vision 43 (1), 29-44.\n"
"Where:\n"
"\n"
":param inputimage: is a string containing the name of the input image\n"
":param outputImageBase: is a string containing the base name of the output images\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imagefilter\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm_single_band.vrt'\n"
"   outputImageBase = './TestOutputs/injune_p142_casi_sub_utm_single_band'\n"
"   gdalformat = 'KEA'\n"
"   outExt = 'kea'\n"
"   datatype = rsgislib.TYPE_32FLOAT\n"
"   imagefilter.LeungMalikFilterBank(inputImage, outputImageBase, gdalformat, outExt, datatype)\n"
"\n"},

    {NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int ImageFilter_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int ImageFilter_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_imagefilter",
        NULL,
        sizeof(struct ImageFilterState),
        ImageFilterMethods,
        NULL,
        ImageFilter_traverse,
        ImageFilter_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__imagefilter(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_imagefilter(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_imagefilter", ImageFilterMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ImageFilterState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_imagefilter.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
