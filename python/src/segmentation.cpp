/*
 *  segmentation.cpp
 *  RSGIS_LIB
 *
 *  Created by Sam Gillingham on 02/05/2013.
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
#include "cmds/RSGISCmdSegmentation.h"

/* An exception object for this module */
/* created in the init function */
struct SegmentationState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct SegmentationState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct SegmentationState _state;
#endif


static PyObject *Segmentation_labelPixelsFromClusterCentres(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImage, *pszClusterCentres, *pszGDALFormat;
    int ignoreZeros;
    if( !PyArg_ParseTuple(args, "sssis:labelPixelsFromClusterCentres", &pszInputImage, &pszOutputImage, 
                                &pszClusterCentres, &ignoreZeros, &pszGDALFormat ))
        return NULL;

    try
    {
        rsgis::cmds::executeLabelPixelsFromClusterCentres(pszInputImage, pszOutputImage, pszClusterCentres,
                        ignoreZeros, pszGDALFormat );
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_eliminateSinglePixels(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImage, *pszClumpsImage, *pszGDALFormat, *pszTempImage;
    int processInMemory, ignoreZeros;
    if( !PyArg_ParseTuple(args, "sssssnn:eliminateSinglePixels", &pszInputImage, &pszClumpsImage, 
                        &pszOutputImage, &pszTempImage, &pszGDALFormat, &processInMemory, &ignoreZeros ))
        return NULL;

    try
    {
        rsgis::cmds::executeEliminateSinglePixels(pszInputImage, pszClumpsImage, pszOutputImage, pszTempImage,
                    pszGDALFormat, processInMemory, ignoreZeros);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_clump(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImage, *pszGDALFormat;
    int processInMemory;    
    bool nodataprovided;
    float fnodata;
    PyObject *pNoData; //could be none or a number
    if( !PyArg_ParseTuple(args, "sssiO:clump", &pszInputImage, &pszOutputImage, &pszGDALFormat, &processInMemory, &pNoData))
        return NULL;
    
    if( pNoData == Py_None )
    {
        nodataprovided = false;
        fnodata = 0;
    }
    else
    {
        // convert to a float if needed
        PyObject *pFloatNoData = PyNumber_Float(pNoData);
        if( pFloatNoData == NULL )
        {
            PyErr_SetString(GETSTATE(self)->error, "nodata parameter must be None or a valid number\n");
            return NULL;
        }

        nodataprovided = true;
        fnodata = PyFloat_AsDouble(pFloatNoData);
        Py_DECREF(pFloatNoData);
    }

    try
    {
        rsgis::cmds::executeClump(pszInputImage, pszOutputImage, pszGDALFormat, 
                                processInMemory, nodataprovided, fnodata);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_RMSmallClumpsStepwise(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszClumpsImage, *pszOutputImage, *pszGDALFormat, *pszStretchStatsFile;
    int storeMean,processInMemory,stretchStatsAvail;
    unsigned int minClumpSize;
    float specThreshold;                   
    if( !PyArg_ParseTuple(args, "ssssnsnnIf:RMSmallClumpsStepwise", &pszInputImage, &pszClumpsImage, &pszOutputImage, &pszGDALFormat,
                    &stretchStatsAvail, &pszStretchStatsFile, &storeMean, &processInMemory, &minClumpSize, &specThreshold))            
        return NULL;
    
    try
    {
        rsgis::cmds::executeRMSmallClumpsStepwise(pszInputImage, pszClumpsImage, pszOutputImage, pszGDALFormat, 
                                stretchStatsAvail, pszStretchStatsFile, storeMean, processInMemory, minClumpSize, specThreshold);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

 
static PyObject *Segmentation_relabelClumps(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImage, *pszGDALFormat;
    int processInMemory;
    if( !PyArg_ParseTuple(args, "sssi:relabelClumps", &pszInputImage, 
                        &pszOutputImage, &pszGDALFormat, &processInMemory ))
        return NULL;

    try
    {
        rsgis::cmds::executeRelabelClumps(pszInputImage, pszOutputImage,
                    pszGDALFormat, processInMemory);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_unionOfClumps(PyObject *self, PyObject *args)
{
    const char *pszOutputImage, *pszGDALFormat;
    std::string inputImage;
    bool nodataprovided;
    float fnodata;
    PyObject *pNoData; //could be none or a number
    PyObject *pInputListObj;
    if( !PyArg_ParseTuple(args, "ssOO:unionOfClumps", &pszOutputImage, &pszGDALFormat,
                                &pInputListObj, &pNoData))
        return NULL;

    if( pNoData == Py_None )
    {
        nodataprovided = false;
        fnodata = 0;
    }
    else
    {
        // convert to a float if needed
        PyObject *pFloatNoData = PyNumber_Float(pNoData);
        if( pFloatNoData == NULL )
        {
            PyErr_SetString(GETSTATE(self)->error, "nodata parameter must be None or a valid number\n");
            return NULL;
        }

        nodataprovided = true;
        fnodata = PyFloat_AsDouble(pFloatNoData);
        Py_DECREF(pFloatNoData);
    }

    Py_ssize_t nInputImages = PyList_Size(pInputListObj);
    if( nInputImages < 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a list");
        return NULL;
    }
    
    std::vector<std::string> inputImagePaths;
    for(Py_ssize_t n = 0; n < nInputImages; n++)
    {
        
        PyObject *strObj;
        strObj = PyList_GetItem(pInputListObj, n);
        if( !RSGISPY_CHECK_STRING(strObj) )
        {
            PyErr_SetString(GETSTATE(self)->error, "must pass a list of strings");
            Py_DECREF(strObj);
            return NULL;
        }
        inputImage = RSGISPY_STRING_EXTRACT(strObj);
        inputImagePaths.push_back(inputImage);      
        Py_DECREF(strObj);
    
    }    
    
    try
    {
                        
        rsgis::cmds::executeUnionOfClumps(inputImagePaths, pszOutputImage, pszGDALFormat,
                        nodataprovided, fnodata);

    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_mergeSegmentationTiles(PyObject *self, PyObject *args)
{
    const char *pszOutputImage, *pszBorderMaskImage, *pszColsName;
    std::string inputImage;
    unsigned int tileBoundary, tileOverlap, tileBody;
    PyObject *pInputListObj;
    if( !PyArg_ParseTuple(args, "ssIIIsO:mergeSegmentationTiles", &pszOutputImage, &pszBorderMaskImage,
                                &tileBoundary, &tileOverlap, &tileBody, &pszColsName, &pInputListObj))
        return NULL;

    Py_ssize_t nInputImages = PyList_Size(pInputListObj);
    if( nInputImages < 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a list");
        return NULL;
    }
    
    std::vector<std::string> inputImagePaths;
    for(Py_ssize_t n = 0; n < nInputImages; n++)
    {
        
        PyObject *strObj;
        strObj = PyList_GetItem(pInputListObj, n);
        if( !RSGISPY_CHECK_STRING(strObj) )
        {
            PyErr_SetString(GETSTATE(self)->error, "must pass a list of strings");
            Py_DECREF(strObj);
            return NULL;
        }
        inputImage = RSGISPY_STRING_EXTRACT(strObj);
        inputImagePaths.push_back(inputImage);      
        Py_DECREF(strObj);
    
    }    
    
    try
    {
                        
        rsgis::cmds::executeMergeSegmentationTiles(pszOutputImage, pszBorderMaskImage, inputImagePaths,
                        tileBoundary, tileOverlap, tileBody, pszColsName);

    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_rmSmallClumps(PyObject *self, PyObject *args)
{
    const char *pszInputClumps, *pszOutputClumps, *pszGDALFormat;
    float areaThreshold;
    PyObject *pNoData; //could be none or a number
    if( !PyArg_ParseTuple(args, "ssfs:rmSmallClumps", &pszInputClumps, &pszOutputClumps, &areaThreshold, &pszGDALFormat))
        return NULL;
    
    try
    {
        rsgis::cmds::executeRMSmallClumps(pszInputClumps, pszOutputClumps, areaThreshold, pszGDALFormat);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}




// Our list of functions in this module
static PyMethodDef SegmentationMethods[] = {
    {"labelPixelsFromClusterCentres", Segmentation_labelPixelsFromClusterCentres, METH_VARARGS, 
"Does stuff.\n"
"call signature: segmentation.labelPixelsFromClusterCentres(inputimage, outputimage, clustercenters, ignorezeros, gdalformat)\n"
"where:\n"
" * inputimage is a string containing the name of the input file\n"
" * outputimage is a string containing the name of the output file\n"
" * clustercentres is a string containing the name of the cluster centre file\n"
" * ignore zeros is a bool\n"
" * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"},

    {"eliminateSinglePixels", Segmentation_eliminateSinglePixels, METH_VARARGS, 
"Does other stuff\n"
"call signature: segmentation.eliminateSinglePixels(inputimage, clumpsimage, outputimage, tempfile, gdalformat, processinmemory, ignorezeros)\n"
"where:\n"
" * inputimage is a string containing the name of the input file\n"
" * clumpsimage is a string containing the name of the clump file\n"
" * outputimage is a string containing the name of the output file\n"
" * tempfile is a string containing the name of the temporary file to use\n"
" * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
" * processinmemory is a bool\n"
" * ignore zeros is a bool\n"},

    {"clump", Segmentation_clump, METH_VARARGS,
"clump\n"
"call signature: segmentation.clump(inputimage, outputimage, gdalformat, processinmemory, nodata)\n"
"where:\n"
" * inputimage is a string containing the name of the input file\n"
" * outputimage is a string containing the name of the output file\n"
" * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
" * processinmemory is a bool\n"
" * nodata is None or float\n"},

    {"RMSmallClumpsStepwise", Segmentation_RMSmallClumpsStepwise, METH_VARARGS,
"Does even more stuff\n"
"call signature: segmentation.RMSmallClumpsStepwise(inputimage, clumpsimage, outputimage, gdalformat, stretchstatsavail, stretchstatsfile, storemean, processinmemory, minclumpsize, specThreshold)\n"
"where:\n"
" * inputimage is a string containing the name of the input file\n"
" * clumpsimage is a string containing the name of the clump file\n"
" * outputimage is a string containing the name of the output file\n"
" * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
" * stretchstatsavail is a bool\n"
" * stretchstatsfile is a string containing the name of the stretch stats file\n"
" * storemean is a bool\n"
" * processinmemory is a bool\n"
" * minclumpsize is an unsigned integer\n"
" * specThreshold is a float\n"},

    {"relabelClumps", Segmentation_relabelClumps, METH_VARARGS,
"Relabel clumps\n"
"call signature: segmentation.relabelClumps(inputimage, outputimage, gdalformat, processinmemory)\n"
"where:\n"
" * inputimage is a string containing the name of the input file\n"
" * outputimage is a string containing the name of the output file\n"
" * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
" * processinmemory is a bool\n"},
                                
    {"UnionOfClumps", Segmentation_unionOfClumps, METH_VARARGS,
"Union of clumps\n"
"call signature: segmentation.unionOfClumps(outputimage, gdalformat, inputimagepaths, nodata)\n"
"where:\n"
" * outputimage is a string containing the name of the output file\n"
" * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
" * inputimagepaths is a list of input image paths\n"
" * nodata is None or float\n"},

    {"mergeSegmentationTiles", Segmentation_mergeSegmentationTiles, METH_VARARGS,
"Merge segmentation tiles\n"
"call signature: segmentation.mergeSegmentationTiles(outputimage, bordermaskimage, tileboundary, tileoverlap, tilebody, colsname, inputimagepaths)\n"
"where:\n"
" * outputimage is a string containing the name of the output file\n"
" * bordermaskimage is a string containing the name of the border mask file\n"
" * tileboundary is an unsigned integer containing the tile boundary pixel value\n"
" * tileoverlap is an unsigned integer containing the tile overlap pixel value\n"
" * tilebody is an unsigned integer containing the tile body pixel value\n"
" * colsname is a string containing the name of the object id column\n"
" * inputimagepaths is a list of input image paths\n"},

    {"rmSmallClumps", Segmentation_rmSmallClumps, METH_VARARGS,
"segmentation.rmSmallClumps(clumpsImage, outputImage, threshold, gdalFormat)\n"
"A function to remove small clumps and set them with a value of 0 (i.e., no data) \n"
"where:\n"
"  clumpsImage is a string containing the name of the input clumps file - note a column called \'Histogram\'.\n"
"  outputImage is a string containing the name of the output clumps file\n"
"  threshold is a float containing the area threshold (in pixels)\n"
"  gdalFormat is a string defining the format of the output image.\n"},

    {NULL}        /* Sentinel */
};
                                
#if PY_MAJOR_VERSION >= 3

static int Segmentation_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int Segmentation_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_segmentation",
        NULL,
        sizeof(struct SegmentationState),
        SegmentationMethods,
        NULL,
        Segmentation_traverse,
        Segmentation_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__segmentation(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_segmentation(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_segmentation", SegmentationMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct SegmentationState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_segmentation.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}

