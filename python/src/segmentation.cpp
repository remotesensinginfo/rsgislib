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
    const char *pszInputImage, *pszOutputImage, *pszClusterCentres, *pszgdalformat;
    int ignoreZeros;
    if( !PyArg_ParseTuple(args, "sssis:labelPixelsFromClusterCentres", &pszInputImage, &pszOutputImage, 
                                &pszClusterCentres, &ignoreZeros, &pszgdalformat ))
        return NULL;

    try
    {
        rsgis::cmds::executeLabelPixelsFromClusterCentres(pszInputImage, pszOutputImage, pszClusterCentres,
                        ignoreZeros, pszgdalformat );
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
    const char *pszInputImage, *pszOutputImage, *pszClumpsImage, *pszgdalformat, *pszTempImage;
    int processInMemory, ignoreZeros;

    if( !PyArg_ParseTuple(args, "sssssii:eliminateSinglePixels", &pszInputImage, &pszClumpsImage, &pszOutputImage, &pszTempImage, &pszgdalformat, &processInMemory, &ignoreZeros ))
    {
        return NULL;
    }

    try
    {
        rsgis::cmds::executeEliminateSinglePixels(pszInputImage, pszClumpsImage, pszOutputImage, pszTempImage, pszgdalformat, processInMemory, ignoreZeros);
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
    const char *pszInputImage, *pszOutputImage, *pszgdalformat;
    int processInMemory = false;
    bool nodataprovided;
    float fnodata;
    int addRatPxlVals = false;
    PyObject *pNoData = Py_None; //could be none or a number
    if( !PyArg_ParseTuple(args, "sss|iOi:clump", &pszInputImage, &pszOutputImage, &pszgdalformat, &processInMemory, &pNoData, &addRatPxlVals))
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
        rsgis::cmds::executeClump(pszInputImage, pszOutputImage, pszgdalformat,
                                processInMemory, nodataprovided, fnodata, addRatPxlVals);
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
    const char *pszInputImage, *pszClumpsImage, *pszOutputImage, *pszgdalformat, *pszStretchStatsFile;
    int storeMean,processInMemory,stretchStatsAvail;
    unsigned int minClumpSize;
    float specThreshold;                   
    if( !PyArg_ParseTuple(args, "ssssisiiIf:rmSmallClumpsStepwise", &pszInputImage, &pszClumpsImage, &pszOutputImage, &pszgdalformat,
                    &stretchStatsAvail, &pszStretchStatsFile, &storeMean, &processInMemory, &minClumpSize, &specThreshold))            
        return NULL;
    
    try
    {
        rsgis::cmds::executeRMSmallClumpsStepwise(pszInputImage, pszClumpsImage, pszOutputImage, pszgdalformat,
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
    const char *pszInputImage, *pszOutputImage, *pszgdalformat;
    int processInMemory;
    if( !PyArg_ParseTuple(args, "sssi:relabelClumps", &pszInputImage, 
                        &pszOutputImage, &pszgdalformat, &processInMemory ))
        return NULL;

    try
    {
        rsgis::cmds::executeRelabelClumps(pszInputImage, pszOutputImage,
                    pszgdalformat, processInMemory);
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
    const char *pszOutputImage, *pszgdalformat;
    std::string inputImage;
    bool nodataprovided;
    float fnodata;
    PyObject *pNoData; //could be none or a number
    PyObject *pInputListObj;
    int addRatPxlVals = false;
    if( !PyArg_ParseTuple(args, "ssOOi:unionOfClumps", &pszOutputImage, &pszgdalformat,
                                &pInputListObj, &pNoData, &addRatPxlVals))
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
        //Py_DECREF(strObj);
    
    }    
    
    try
    {
                        
        rsgis::cmds::executeUnionOfClumps(inputImagePaths, pszOutputImage, pszgdalformat, nodataprovided, fnodata, addRatPxlVals);

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
        //Py_DECREF(strObj);
    
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


static PyObject *Segmentation_mergeClumpImages(PyObject *self, PyObject *args)
{
    const char *pszOutputImage;
    PyObject *pInputListObj;
    std::string inputImage;
    int mergeRATs = false;
    if( !PyArg_ParseTuple(args, "Os|i:mergeClumpImages", &pInputListObj, &pszOutputImage, &mergeRATs))
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
    }    
    
    try
    {
        rsgis::cmds::executeMergeClumpImages(inputImagePaths, pszOutputImage, mergeRATs);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *Segmentation_findTileBordersMask(PyObject *self, PyObject *args)
{
    const char *pszBorderMaskImage, *pszColsName;
    std::string inputImage;
    unsigned int tileBoundary, tileOverlap, tileBody;
    PyObject *pInputListObj;
    if( !PyArg_ParseTuple(args, "sIIIsO:findTileBordersMask", &pszBorderMaskImage,
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
        //Py_DECREF(strObj);
    
    }    
    
    try
    {
                        
        rsgis::cmds::executeFindTileBordersMask(inputImagePaths, pszBorderMaskImage,
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
    const char *pszInputClumps, *pszOutputClumps, *pszgdalformat;
    float areaThreshold;
    if( !PyArg_ParseTuple(args, "ssfs:rmSmallClumps", &pszInputClumps, &pszOutputClumps, &areaThreshold, &pszgdalformat))
        return NULL;
    
    try
    {
        rsgis::cmds::executeRMSmallClumps(pszInputClumps, pszOutputClumps, areaThreshold, pszgdalformat);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *Segmentation_meanImage(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputClumps, *pszOutputImage, *pszgdalformat;
    int nDataType;
    if( !PyArg_ParseTuple(args, "ssssi:mergeImage", &pszInputImage, &pszInputClumps, &pszOutputImage, &pszgdalformat, &nDataType))
        return NULL;
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        rsgis::cmds::executeMeanImage(pszInputImage, pszInputClumps, pszOutputImage, pszgdalformat, type, false);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *Segmentation_GenerateRegularGrid(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImage, *pszgdalformat;
    unsigned int numXPxls, numYPxls;
    int offset = 0;
    if( !PyArg_ParseTuple(args, "sssII|i:generateRegularGrid", &pszInputImage, &pszOutputImage, &pszgdalformat, &numXPxls, &numYPxls, &offset))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeGenerateRegularGrid(std::string(pszInputImage), std::string(pszOutputImage), std::string(pszgdalformat), numXPxls, numYPxls, (bool)offset);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *Segmentation_IncludeRegionsInClumps(PyObject *self, PyObject *args)
{
    const char *pszClumpsImage, *pszRegionsImage, *pszOutputImage, *pszgdalformat;

    if( !PyArg_ParseTuple(args, "ssss:includeRegionsInClumps", &pszClumpsImage, &pszRegionsImage, &pszOutputImage, &pszgdalformat ))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeIncludeClumpedRegion(std::string(pszClumpsImage), std::string(pszRegionsImage), std::string(pszOutputImage), std::string(pszgdalformat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Segmentation_mergeSegments2Neighbours(PyObject *self, PyObject *args)
{
    const char *pszInputClumpsImage, *pszInputSpecImage, *pszOutputImage, *pszgdalformat, *selectClumpsCol, *noDataClumpsCol;
    if( !PyArg_ParseTuple(args, "ssssss:mergeSegments2Neighbours", &pszInputClumpsImage, &pszInputSpecImage, &pszOutputImage, &pszgdalformat, &selectClumpsCol, &noDataClumpsCol))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeMergeSelectClumps2Neighbour(std::string(pszInputSpecImage), std::string(pszInputClumpsImage), std::string(pszOutputImage), std::string(pszgdalformat), std::string(selectClumpsCol), std::string(noDataClumpsCol));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Segmentation_dropSelectedSegments(PyObject *self, PyObject *args)
{
    const char *pszInputClumpsImage, *pszOutputImage, *pszgdalformat, *selectClumpsCol;
    if( !PyArg_ParseTuple(args, "ssss:dropSelectedClumps", &pszInputClumpsImage, &pszOutputImage, &pszgdalformat, &selectClumpsCol ))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeDropSelectedClumps(std::string(pszInputClumpsImage), std::string(pszOutputImage), std::string(pszgdalformat), std::string(selectClumpsCol));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Segmentation_mergeEquivalentClumps(PyObject *self, PyObject *args)
{
    const char *pszInputClumpsImage, *pszOutputImage, *pszgdalformat;
    PyObject *valClumpsCols;
    if( !PyArg_ParseTuple(args, "sssO:mergeEquivClumps", &pszInputClumpsImage, &pszOutputImage, &pszgdalformat, &valClumpsCols))
    {
        return NULL;
    }
    
    Py_ssize_t nValCols = PyList_Size(valClumpsCols);
    if( nValCols < 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a list");
        return NULL;
    }
    
    std::vector<std::string> cols;
    for(Py_ssize_t n = 0; n < nValCols; n++)
    {
        PyObject *strObj = PyList_GetItem(valClumpsCols, n);
        if( !RSGISPY_CHECK_STRING(strObj) )
        {
            PyErr_SetString(GETSTATE(self)->error, "must pass a list of strings");
            Py_DECREF(strObj);
            return NULL;
        }
        std::string colName = RSGISPY_STRING_EXTRACT(strObj);
        cols.push_back(colName);
    }
    
    try
    {
        rsgis::cmds::executeMergeClumpsEquivalentVal(std::string(pszInputClumpsImage), std::string(pszOutputImage), std::string(pszgdalformat), cols);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *Segmentation_PxlGrowRegions(PyObject *self, PyObject *args)
{
    const char *pszInputClumpsImage, *pszValsImage, *pszOutputImage, *pszgdalformat, *pszMuParseCriteria;
    PyObject *varNameBandPairsObj;
    if( !PyArg_ParseTuple(args, "sssssO:pxlGrowRegions", &pszInputClumpsImage, &pszValsImage, &pszOutputImage, &pszgdalformat, &pszMuParseCriteria, &varNameBandPairsObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(varNameBandPairsObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "varNameBandPairs argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nValCols = PyList_Size(varNameBandPairsObj);
    if( nValCols < 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a list");
        return NULL;
    }
    
    std::vector<rsgis::cmds::VarImgBandPairs> varNameBandPairs;
    for(Py_ssize_t n = 0; n < nValCols; n++)
    {
        PyObject *o = PySequence_GetItem(varNameBandPairsObj, n);
        
        PyObject *pVarName = PyObject_GetAttrString(o, "varName");
        if( ( pVarName == NULL ) || ( pVarName == Py_None ) || !RSGISPY_CHECK_STRING(pVarName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'varName\'" );
            Py_XDECREF(pVarName);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pBandIndex = PyObject_GetAttrString(o, "bandIndex");
        if( ( pBandIndex == NULL ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find integer attribute \'bandIndex\'" );
            Py_DECREF(pVarName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            return NULL;
        }
        
        rsgis::cmds::VarImgBandPairs varPair;
        varPair.varName = RSGISPY_STRING_EXTRACT(pVarName);
        varPair.imgBand = RSGISPY_INT_EXTRACT(pBandIndex);
        varNameBandPairs.push_back(varPair);
        
        Py_DECREF(pVarName);
        Py_XDECREF(pBandIndex);
        Py_DECREF(o);
    }
    
    try
    {
        rsgis::cmds::executePxlGrowRegions(std::string(pszInputClumpsImage), std::string(pszValsImage), std::string(pszOutputImage), std::string(pszgdalformat), std::string(pszMuParseCriteria), varNameBandPairs);
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
"segmentation.labelPixelsFromClusterCentres(inputimage, outputimage, clustercenters, ignorezeros, gdalformat)\n"
"Labels image pixels with the ID of the nearest cluster centre.\n"
"\n"
"Where:\n"
"\n"
":param inputimage: is a string containing the name of the input file\n"
":param outputimage: is a string containing the name of the output file\n"
":param clustercentres: is a string containing the name of the cluster centre file\n"
":param ignore: zeros is a bool\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
"\n"},

    {"eliminateSinglePixels", Segmentation_eliminateSinglePixels, METH_VARARGS, 
"segmentation.eliminateSinglePixels(inputimage, clumpsimage, outputimage, tempfile, gdalformat, processinmemory, ignorezeros)\n"
"Eliminates single pixels\n"
"\n"
"Where:\n"
"\n"
":param inputimage: is a string containing the name of the input file\n"
":param clumpsimage: is a string containing the name of the clump file\n"
":param outputimage: is a string containing the name of the output file\n"
":param tempfile: is a string containing the name of the temporary file to use\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param processinmemory: is a bool specifying if processing should be carried out in memory (faster if sufficient RAM is available, set to False if unsure).\n"
":param ignorezeros: is a bool\n"
"\n"},

    {"clump", Segmentation_clump, METH_VARARGS,
"segmentation.clump(inputimage, outputimage, gdalformat, processinmemory, nodata, addPxlVal2Rat)\n"
"A function which clumps an input image (of int pixel data type) to identify connected independent sets of pixels.\n"
"\n"
"Where:\n"
"\n"
":param inputimage: is a string containing the name of the input file\n"
":param outputimage: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param processinmemory: is a bool specifying if processing should be carried out in memory (faster if sufficient RAM is available, set to False if unsure).\n"
":param nodata: is None or float\n"
":param addPxlVal2Rat: is a boolean specifying whether the pixel value (from inputimage) should be added as a RAT.\n"
"\n"},

    {"rmSmallClumpsStepwise", Segmentation_RMSmallClumpsStepwise, METH_VARARGS,
"segmentation.rmSmallClumpsStepwise(inputimage, clumpsimage, outputimage, gdalformat, stretchstatsavail, stretchstatsfile, storemean, processinmemory, minclumpsize, specThreshold)\n"
"eliminate clumps smaller than a given size from the scene, small clumps will be combined with their spectrally closest neighbouring  clump in a stepwise fashion unless over spectral distance threshold\n"
"\n"
"Where:\n"
"\n"
":param inputimage: is a string containing the name of the input file\n"
":param clumpsimage: is a string containing the name of the clump file\n"
":param outputimage: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param stretchstatsavail: is a bool\n"
":param stretchstatsfile: is a string containing the name of the stretch stats file\n"
":param storemean: is a bool\n"
":param processinmemory: is a bool specifying if processing should be carried out in memory (faster if sufficient RAM is available, set to False if unsure).\n"
":param minclumpsize: is an unsigned integer providing the minimum size for clumps.\n"
":param specThreshold: is a float providing the maximum (Euclidian distance) spectral separation for which to merge clumps. Set to a large value to ignore spectral separation and always merge.\n"
"\n"},

    {"relabelClumps", Segmentation_relabelClumps, METH_VARARGS,
"segmentation.relabelClumps(inputimage, outputimage, gdalformat, processinmemory)\n"
"Relabel clumps\n"
"\n"
"Where:\n"
"\n"
":param inputimage: is a string containing the name of the input file\n"
":param outputimage: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param processinmemory: is a bool specifying if processing should be carried out in memory (faster if sufficient RAM is available, set to False if unsure).\n"
"\n"},
                                
    {"unionOfClumps", Segmentation_unionOfClumps, METH_VARARGS,
"segmentation.unionOfClumps(outputimage, gdalformat, inputimagepaths, nodata, addPxlVals2Rat)\n"
"The function takes the union of clumps images - combining them so all lines from all clumps are preserved in the new outputted clumps image.\n"
"\n"
"Where:\n"
"\n"
":param outputimage: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param inputimagepaths: is a list of input image paths\n"
":param nodata: is None or float\n"
":param addPxlVals2Rat: is a boolean specifying whether the pixel values (from inputimagepaths) should be added as a RAT; column names have prefix 'ClumpVal_' with index starting at 1 for each variable.\n"
"\n"
"\n"
},

    {"mergeSegmentationTiles", Segmentation_mergeSegmentationTiles, METH_VARARGS,
"segmentation.mergeSegmentationTiles(outputimage, bordermaskimage, tileboundary, tileoverlap, tilebody, colsname, inputimagepaths)\n"
"Merge body clumps from tile segmentations into outputfile\n"
"\n"
"Where:\n"
"\n"
":param outputimage: is a string containing the name of the output file\n"
":param bordermaskimage: is a string containing the name of the border mask file\n"
":param tileboundary: is an unsigned integer containing the tile boundary pixel value\n"
":param tileoverlap: is an unsigned integer containing the tile overlap pixel value\n"
":param tilebody: is an unsigned integer containing the tile body pixel value\n"
":param colsname: is a string containing the name of the object id column\n"
":param inputimagepaths: is a list of input image paths\n"
"\n"},

    {"mergeClumpImages", Segmentation_mergeClumpImages, METH_VARARGS,
"segmentation.mergeClumpImages(inputimagepaths, outputimage, mergeRATs)\n"
"Merge all clumps from tile segmentations into outputfile\n"
"\n"
"Where:\n"
"\n"
":param inputimagepaths: is a list of input image paths\n"
":param outputimage: is a string containing the name of the output file\n"
":param mergeRATs: is a boolean specifying with the image RATs are to merged (Default: false; Optional)\n"
"\n"},

    {"findTileBordersMask", Segmentation_findTileBordersMask, METH_VARARGS,
"segmentation.findTileBordersMask(bordermaskimage, tileboundary, tileoverlap, tilebody, colsname, inputimagepaths)\n"
"Mask tile borders\n"
"\n"
"Where:\n"
"\n"
":param bordermaskimage: is a string containing the name of the border mask file\n"
":param tileboundary: is an unsigned integer containing the tile boundary pixel value\n"
":param tileoverlap: is an unsigned integer containing the tile overlap pixel value\n"
":param tilebody: is an unsigned integer containing the tile body pixel value\n"
":param colsname: is a string containing the name of the object id column\n"
":param inputimagepaths: is a list of input clump image paths\n"
"\n"},

    {"rmSmallClumps", Segmentation_rmSmallClumps, METH_VARARGS,
"segmentation.rmSmallClumps(clumpsImage, outputImage, threshold, gdalformat)\n"
"A function to remove small clumps and set them with a value of 0 (i.e., no data) \n"
"\n"
"Where:\n"
"\n"
":param clumpsImage: is a string containing the name of the input clumps file - note a column called \'Histogram\'.\n"
":param outputImage: is a string containing the name of the output clumps file\n"
":param threshold: is a float containing the area threshold (in pixels)\n"
":param gdalformat: is a string defining the format of the output image.\n"
"\n"},
    
    {"meanImage", Segmentation_meanImage, METH_VARARGS,
"segmentation.meanImage(inputImage, inputClumps, outputImage, gdalformat, datatype)\n"
"A function to generate an image where with the mean value for each clump. Primarily for visualisation and evaluating segmentation.\n"
"\n"
"Where:\n"
"\n"
":param inputImage: is a string containing the name of the input image file from which the mean is taken.\n"
":param inputClumps: is a string containing the name of the input clumps file\n"
":param outputImage: is a string containing the name of the output image.\n"
":param gdalformat: is a string defining the format of the output image.\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
"\n"},

{"generateRegularGrid", Segmentation_GenerateRegularGrid, METH_VARARGS,
"segmentation.generateRegularGrid(inputImage, outputClumps, gdalformat, numXPxls, numYPxls, offset)\n"
"A function to generate an image where with the mean value for each clump. Primarily for visualisation and evaluating segmentation.\n"
"\n"
"Where:\n"
"\n"
":param inputImage: is a string containing the name of the input image file specifying the dimensions of the output image.\n"
":param outputClumps: is a string containing the name and path of the output clumps image\n"
":param gdalformat: is a string defining the format of the output image.\n"
":param numXPxls: is the size of the grid cells in the X axis in pixel units.\n"
":param numYPxls: is the size of the grid cells in the Y axis in pixel units.\n"
":param offset: is a boolean specifying whether the grid should be offset, i.e., starts half way point of numXPxls and numYPxls (Default is false; optional)"
"\n"},
    
{"includeRegionsInClumps", Segmentation_IncludeRegionsInClumps, METH_VARARGS,
"segmentation.includeRegionsInClumps(clumpsImage, regionsImage, outputClumps, gdalformat)\n"
"A function to include a set of clumped regions within an existing clumps (i.e., segmentation) image.\n"
"NOTE. You should run the relabelClumps function on the output of this command before using further.\n"
"\n"
"Where:\n"
"\n"
":param clumpsImage: is a string containing the filepath for the input clumps image.\n"
":param regionsImage: is a string containing the filepath for the input regions image.\n"
":param outputClumps: is a string containing the name and path of the output clumps image\n"
":param gdalformat: is a string defining the format of the output image.\n"
"\n"},
    
{"mergeSegments2Neighbours", Segmentation_mergeSegments2Neighbours, METH_VARARGS,
"segmentation.mergeSegments2Neighbours(clumpsImage, spectralImage, outputClumps, gdalformat, selectedClumpsCol, noDataClumpsCol)\n"
"A function to merge some selected clumps with the neighbours based on colour (spectral) distance where clumps identified as no data are ignored.\n"
"\n"
"Where:\n"
"\n"
":param clumpsImage: is a string containing the filepath for the input clumps image.\n"
":param spectralImage: is a string containing the filepath for the input image used to define 'distance'.\n"
":param outputClumps: is a string containing the name and path of the output clumps image\n"
":param gdalformat: is a string defining the format of the output image.\n"
":param selectClumpsCol: is a string defining the binary column for defining the segments to be merged (1 == selected clumps).\n"
":param noDataClumpsCol: is a string defining the binary column for defining the segments to be ignored as no data (1 == no-data clumps).\n"
"\n"},
    
{"dropSelectedClumps", Segmentation_dropSelectedSegments, METH_VARARGS,
"segmentation.dropSelectedClumps(clumpsImage, outputClumps, gdalformat)\n"
"A function to drop the selected clumps from the segmentation.\n"
"\n"
"Where:\n"
"\n"
":param clumpsImage: is a string containing the filepath for the input clumps image.\n"
":param outputClumps: is a string containing the name and path of the output clumps image\n"
":param gdalformat: is a string defining the format of the output image.\n"
":param selectClumpsCol: is a string defining the binary column for defining the segments to be merged (1 == selected clumps).\n"
"\n"},
    
{"mergeEquivClumps", Segmentation_mergeEquivalentClumps, METH_VARARGS,
"segmentation.mergeEquivClumps(clumpsImage, outputClumps, gdalformat, valClumpsCols)\n"
"A function to merge neighbouring clumps which have the same value - for example when merging across tile boundaries.\n"
"\n"
"Where:\n"
"\n"
":param clumpsImage: is a string containing the filepath for the input clumps image.\n"
":param outputClumps: is a string containing the name and path of the output clumps image\n"
":param gdalformat: is a string defining the format of the output image.\n"
":param valClumpsCol: is a list of strings defining the value(s) used to define equivalence (typically it might be the original pixel values when clumping through tiling).\n"
"\n"},

{"pxlGrowRegions", Segmentation_PxlGrowRegions, METH_VARARGS,
"segmentation.pxlGrowRegions(clumpsImage, valsImage, outputImage, gdalformat, muParseCriteria, varNameBandPairs)\n"
"A function to merge neighbouring clumps which have the same value - for example when merging across tile boundaries.\n"
"\n"
"Where:\n"
"\n"
":param clumpsImage: is a string containing the filepath for the input clumps image.\n"
":param valsImage: is a string containing the file path for the values (criteria) image.\n"
":param outputClumps: is a string containing the name and path of the output clumps image\n"
":param gdalformat: is a string defining the format of the output image.\n"
":param muParseCriteria: is a string with an muparser criteria (muparser; e.g., b1 < 20?1:0). Expression output must be 0 or 1 (1 for True).\n"
":param varNameBandPairs: is a list pairs specifying the variable name (in muparser expression) and the band number to which it refers in valsImage (note band numbers start a 1).\n"
"\n"
"Example::\n"
"\n"
"    varBandPair = collections.namedtuple('VarBandPair', ['varName', 'bandIndex'])\n"
"    varBandPairSeq = list()\n"
"    varBandPairSeq.append(varBandPair(varName='b1', bandIndex=1))\n"
"    muParseCriteria = 'b1 > 1000?1:0'\n"
"    rsgislib.segmentation.pxlGrowRegions(tmpInitClearSkyRegionsFinal, tmpCloudsImgDist2CloudsNoData, tmpClearSkyRegionsGrow, 'KEA', muParseCriteria, varBandPairSeq)\n"
"\n"},
    
    
   

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

