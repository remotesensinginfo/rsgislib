/*
 *  classification.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/11/2013.
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
#include "cmds/RSGISCmdHistoCube.h"
#include <vector>

/* An exception object for this module */
/* created in the init function */
struct HistoCubeState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct HistoCubeState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct HistoCubeState _state;
#endif

// Helper function to extract python sequence to array of strings
static std::string *ExtractStringArrayFromSequence(PyObject *sequence, int *nElements)
{
    Py_ssize_t nFields = PySequence_Size(sequence);
    *nElements = nFields;
    std::string *stringsArray = new std::string[nFields];

    for(int i = 0; i < nFields; ++i) {
        PyObject *stringObj = PySequence_GetItem(sequence, i);

        if(!RSGISPY_CHECK_STRING(stringObj))
        {
            PyErr_SetString(GETSTATE(sequence)->error, "Fields must be strings");
            Py_DECREF(stringObj);
            return stringsArray;
        }

        stringsArray[i] = RSGISPY_STRING_EXTRACT(stringObj);
        Py_DECREF(stringObj);
    }

    return stringsArray;
}

static PyObject *HistoCube_CreateEmptyHistCube(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszCubeFile;
    unsigned long numOfFeats = 0;

    static char *kwlist[] = {"filename", "numOfFeats", NULL};
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sk:createEmptyHistoCube", kwlist, &pszCubeFile, &numOfFeats))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeCreateEmptyHistoCube(std::string(pszCubeFile), numOfFeats);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *HistoCube_CreateHistoCubeLayer(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszCubeFile;
    const char *pszLayerName;
    const char *pszDataTime = "";
    int lowBin;
    int upBin;
    float scale = 1;
    float offset = 0;
    int hasDateTimeInt = false;
    
    
    static char *kwlist[] = {"filename", "layerName", "lowBin", "upBin", "scale", "offset", "hasDateTime", "dataTime", NULL};
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssii|ffis:createHistoCubeLayer", kwlist, &pszCubeFile, &pszLayerName, &lowBin, &upBin, &scale, &offset, &hasDateTimeInt, &pszDataTime))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeCreateHistoCubeLayer(std::string(pszCubeFile), std::string(pszLayerName), lowBin, upBin, scale, offset, (bool)hasDateTimeInt, std::string(pszDataTime));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *HistoCube_PopulateHistoCubeLayer(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszCubeFile;
    const char *pszLayerName;
    const char *pszClumpsImg;
    const char *pszValsImg;
    unsigned int imgBand;
    int inMem = true;
    
    static char *kwlist[] = {"filename", "layerName", "clumpsImg", "valsImg", "band", "inMem", NULL};
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssI|i:populateHistoCubeLayer", kwlist, &pszCubeFile, &pszLayerName, &pszClumpsImg, &pszValsImg, &imgBand, &inMem))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executePopulateSingleHistoCubeLayer(std::string(pszCubeFile), std::string(pszLayerName), std::string(pszClumpsImg), std::string(pszValsImg), imgBand, (bool)inMem);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *HistoCube_ExportHistoBins2ImgBands(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszCubeFile;
    const char *pszLayerName;
    const char *pszClumpsImg;
    const char *pszOutputImg;
    const char *pszGDALFormat;
    PyObject *binIdxsObj;
    
    static char *kwlist[] = {"filename", "layerName", "clumpsImg", "outputImg", "gdalformat", "binidxs", NULL};
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssssO:exportHistoBins2ImgBands", kwlist, &pszCubeFile, &pszLayerName, &pszClumpsImg, &pszOutputImg, &pszGDALFormat, &binIdxsObj))
    {
        return NULL;
    }
    
    std::vector<unsigned int> exportBins;
    
    if( !PySequence_Check(binIdxsObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "binidxs argument must be a sequence");
        return NULL;
    }
    Py_ssize_t nBinIdxs = PySequence_Size(binIdxsObj);
    exportBins.reserve(nBinIdxs);
    
    for( Py_ssize_t n = 0; n < nBinIdxs; n++ )
    {
        PyObject *o = PySequence_GetItem(binIdxsObj, n);
        if( ( o == NULL ) || ( o == Py_None ) || !RSGISPY_CHECK_INT(o) )
        {
            PyErr_SetString(GETSTATE(self)->error, "value in binidxs was not an int." );
            Py_DECREF(o);
            return NULL;
        }
        exportBins.push_back(RSGISPY_UINT_EXTRACT(o));
    }
    
    try
    {
        rsgis::cmds::executeExportHistBins2Img(std::string(pszCubeFile), std::string(pszLayerName), std::string(pszClumpsImg), std::string(pszOutputImg), std::string(pszGDALFormat), exportBins);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *HistoCube_GetLayersNames(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszCubeFile;
    
    static char *kwlist[] = {"filename", NULL};
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "s:getLayerNames", kwlist, &pszCubeFile))
    {
        return NULL;
    }
    
    std::vector<std::string> lyrNames;
    try
    {
        lyrNames = rsgis::cmds::executeExportHistBins2Img(std::string(pszCubeFile));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    
    PyObject *pyLyrNames = PyList_New(lyrNames.size());
    unsigned int idx = 0;
    for(std::vector<std::string>::iterator iterLyrs = lyrNames.begin(); iterLyrs != lyrNames.end(); ++iterLyrs)
    {
        PyObject* name = RSGISPY_CREATE_STRING((*iterLyrs).c_str());
        if(PyList_SetItem(pyLyrNames, idx, name) != 0)
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not add name to list to be returned - report as bug.");
            return NULL;
        }
        idx = idx + 1;
    }
    
    return pyLyrNames;
}

static PyObject *HistoCube_ExportHistoStats2ImgBands(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszCubeFile;
    const char *pszLayerName;
    const char *pszClumpsImg;
    const char *pszOutputImg;
    const char *pszGDALFormat;
    int nDataType;
    PyObject *binSumStatsObj;
    
    static char *kwlist[] = {"filename", "layerName", "clumpsImg", "outputImg", "gdalformat", "datatype", "binstats", NULL};
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssssiO:exportHistoStats2ImgBands", kwlist, &pszCubeFile, &pszLayerName, &pszClumpsImg, &pszOutputImg, &pszGDALFormat, &nDataType, &binSumStatsObj))
    {
        return NULL;
    }
    
    std::vector<rsgis::cmds::RSGISCmdsHistSummariseStats> exportSumStats;
    
    if( !PySequence_Check(binSumStatsObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "binstats argument must be a sequence");
        return NULL;
    }
    Py_ssize_t nBinSumStats = PySequence_Size(binSumStatsObj);
    exportSumStats.reserve(nBinSumStats);
    rsgis::cmds::RSGISCmdsHistSummariseStats sumVal = rsgis::cmds::rsgiscmds_hstat_none;
    for( Py_ssize_t n = 0; n < nBinSumStats; n++ )
    {
        PyObject *o = PySequence_GetItem(binSumStatsObj, n);
        if( ( o == NULL ) || ( o == Py_None ) || !RSGISPY_CHECK_INT(o) )
        {
            PyErr_SetString(GETSTATE(self)->error, "value in binstats was not an int." );
            Py_DECREF(o);
            return NULL;
        }
        sumVal = (rsgis::cmds::RSGISCmdsHistSummariseStats)RSGISPY_INT_EXTRACT(o);
        exportSumStats.push_back(sumVal);
    }
    
    // we made these values the same so should work
    rsgis::RSGISLibDataType dType = (rsgis::RSGISLibDataType)nDataType;
    
    try
    {
        rsgis::cmds::executeExportHistStats2Img(std::string(pszCubeFile), std::string(pszLayerName), std::string(pszClumpsImg), std::string(pszOutputImg), std::string(pszGDALFormat), dType, exportSumStats);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef HistoCubeMethods[] = {
{"createEmptyHistoCube", (PyCFunction)HistoCube_CreateEmptyHistCube, METH_VARARGS | METH_KEYWORDS,
"rsgislib.histocube.createEmptyHistoCube(filename=string, numOfFeats=unsigned long)\n"
"Create an empty histogram cube file ready for populating.\n"
"\n"
"Where:\n"
"\n"
"* filename - is the file path and name for the histogram cube file.\n"
"* numOfFeats - is the number of features within the cube - this is defined globally within the file.\n"
"\n"
"Example::\n"
"\n"
"import rsgislib.histocube\n"
"rsgislib.histocube.createEmptyHistoCube('HistoCubeTest.hcf', 10000)\n"
"\n"
},
    
{"createHistoCubeLayer", (PyCFunction)HistoCube_CreateHistoCubeLayer, METH_VARARGS | METH_KEYWORDS,
"rsgislib.histocube.createHistoCubeLayer(filename=string, layerName=string, lowBin=int, upBin=int, scale=float, offset=float, hasDateTime=boolean, dataTime=string)\n"
"Create an empty histogram cube layer, with all zero'd.\n"
"The histogram is made up of integer bins and with a scale and offset to define the bin sizes.\n"
"\n"
"Where:\n"
"\n"
"* filename - is the file path and name for the histogram cube file.\n"
"* layerName - is the name of the layer to be created.\n"
"* lowBin - is the lower limit of the histogram bins created (Can be negative).\n"
"* upBin - is the upper limit of the histogram bins created.\n"
"* scale - is the scale parameter used to scale/offset the input data (Optional: default = 1)\n"
"* offset - is the offset parameter used to scale/offset the input data (Optional: default = 0) \n"
"* hasDateTime - is a boolean parameter specifying whether the layer has a date/time associated with it (Optional: default = False).\n"
"* dataTime - is an ISO string representing the date and time associated with the layer.\n"
"\n"
"Example::\n"
"\n"
"import rsgislib.histocube\n"
"\n"
"hcFile = 'HistoCubeTest.hcf'\n"
"rsgislib.histocube.createEmptyHistoCube(hcFile, 1000)\n"
"\n"
"layerName = 'LyrName'\n"
"rsgislib.histocube.createHistoCubeLayer(hcFile, layerName=layerName, lowBin=0, upBin=100)\n"
"\n"
},

{"populateHistoCubeLayer", (PyCFunction)HistoCube_PopulateHistoCubeLayer, METH_VARARGS | METH_KEYWORDS,
"rsgislib.histocube.populateHistoCubeLayer(filename=string, layerName=string, clumpsImg=string, valsImg=string, band=int, inMem=bool)\n"
"Populate the histogram layer with information from an image band.\n"
"Note, data from this band is 'added' to any existing data already within the histogram(s).\n"
"\n"
"Where:\n"
"\n"
"* filename - is the file path and name for the histogram cube file.\n"
"* layerName - is the name of the layer to be created.\n"
"* clumpsImg - is a clumps image that specifies which histogram cube row pixels in with values image are associated (note resolution must be the same as the values image).\n"
"* valsImg - is the image with the values which are populated into the histogram cube.\n"
"* band - is the band number (note band numbers start at 1)\n"
"* inMem - is a boolean specifying whether the data array should be kept in memory; much faster in memory. (Optional, default is True)\n"
"\n"
"Example::\n"
"\n"
"import rsgislib.histocube\n"
"import rsgislib.imagecalc\n"
"import math\n"
"import rsgislib\n"
"\n"
"clumpsImg = 'WV2_525N040W_20110727_TOARefl_clumps_final.kea'\n"
"minVal, maxVal = rsgislib.imagecalc.getImageBandMinMax(clumpsImg, 1, False, 0)\n"
"\n"
"hcFile = 'HistoCubeTest.hcf'\n"
"rsgislib.histocube.createEmptyHistoCube(hcFile, math.ceil(maxVal)+1)\n"
"\n"
"layerName = 'SceneCount'\n"
"rsgislib.histocube.createHistoCubeLayer(hcFile, layerName=layerName, lowBin=0, upBin=256)\n"
"\n"
"timer = rsgislib.RSGISTime()\n"
"timer.start(True)\n"
"valsImg = 'WV2_525N040W_20110727_TOARefl_b762_stch.kea'\n"
"rsgislib.histocube.populateHistoCubeLayer(hcFile, layerName=layerName, clumpsImg=clumpsImg, valsImg=valsImg, band=1)\n"
"timer.end()\n"
"\n"
},

{"exportHistoBins2ImgBands", (PyCFunction)HistoCube_ExportHistoBins2ImgBands, METH_VARARGS | METH_KEYWORDS,
"rsgislib.histocube.exportHistoBins2ImgBands(filename=string, layerName=string, clumpsImg=string, outputImg=string, gdalformat=string, binidxs=list)\n"
"Export bins from the histogram cube to an output image.\n"
"\n"
"Where:\n"
"\n"
"* filename - is the file path and name for the histogram cube file.\n"
"* layerName - is the name of the layer to be created.\n"
"* clumpsImg - is a clumps image that specifies which histogram cube row pixels in with values image are associated (note resolution must be the same as the values image).\n"
"* outputImg - is the file path to the output image.\n"
"* gdalformat - is the format of the output image (e.g., KEA)\n"
"* binidxs - is list of indexed (start 0) within the histogram to export.\n"
"\n"
"Example::\n"
"\n"
"import rsgislib.histocube\n"
"\n"
"hcTileFile='./HistoCube/LandsatWalesRegion_60m_tile7.hcf'\n"
"tile='./RefImages/LandsatWalesRegion_60m_tile7.kea'\n"
"outputImg='./FreqImgs/LandsatWalesRegion_60m_tile7.kea'\n"
"rsgislib.histocube.exportHistoBins2ImgBands(filename=hcTileFile, layerName='OverallPxlCount', clumpsImg=tile, outputImg=outputImg, gdalformat='KEA', binidxs=[0,1])\n"
"\n"
},

{"getLayerNames", (PyCFunction)HistoCube_GetLayersNames, METH_VARARGS | METH_KEYWORDS,
"rsgislib.histocube.getLayerNames(filename=string)\n"
"Get a list of the layer names within the histocube file..\n"
"\n"
"Where:\n"
"\n"
"* filename - is the file path and name for the histogram cube file.\n"
"\n"
"Returns:\n"
"List of layer names of string type.\n"
"\n"
"Example::\n"
"\n"
"import rsgislib.histocube\n"
"\n"
"hcTileFile='./HistoCube/LandsatWalesRegion_60m_tile7.hcf'\n"
"lyrNames = rsgislib.histocube.getLayerNames(filename=hcTileFile)\n"
"\n"
},
    
{"exportHistoStats2ImgBands", (PyCFunction)HistoCube_ExportHistoStats2ImgBands, METH_VARARGS | METH_KEYWORDS,
"rsgislib.histocube.exportHistoStats2ImgBands(filename=string, layerName=string, clumpsImg=string, outputImg=string, gdalformat=string, datatype=int, binstats=list)\n"
"Export summary statistics (e.g., median, mean, mode, min, max etc.) from the histogram cube to an output image.\n"
"\n"
"Where:\n"
"\n"
"* filename - is the file path and name for the histogram cube file.\n"
"* layerName - is the name of the layer to be created.\n"
"* clumpsImg - is a clumps image that specifies which histogram cube row pixels in with values image are associated (note resolution must be the same as the values image).\n"
"* outputImg - is the file path to the output image.\n"
"* gdalformat - is the format of the output image (e.g., KEA)\n"
"* datatype - specifies one of the values from rsgislib.TYPE_*\n"
"* binstats - is list of summary statistics which will be calculated for each histogram and exported (Must be of type: rsgislib.histocube.SUMTYPE_HC_*).\n"
"\n"
"Example::\n"
"\n"
"import rsgislib\n"
"import rsgislib.histocube\n"
"\n"
"hcTileFile='./HistoCube/LandsatWalesRegion_60m_tile7.hcf'\n"
"tile='./RefImages/LandsatWalesRegion_60m_tile7.kea'\n"
"outputImg='./StatsImgs/LandsatWalesRegion_60m_tile7.kea'\n"
"binstats=[rsgislib.histocube.SUMTYPE_HC_MEDIAN, rsgislib.histocube.SUMTYPE_HC_MEAN]\n"
"rsgislib.histocube.exportHistoStats2ImgBands(filename=hcTileFile, layerName='OverallPxlCount', clumpsImg=tile, outputImg=outputImg, gdalformat='KEA', datatype=rsgislib.TYPE_32FLOAT binstats=binstats)\n"
"\n"
},

    
{NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int HistoCube_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int HistoCube_clear(PyObject *m)
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_histocube",
        NULL,
        sizeof(struct HistoCubeState),
        HistoCubeMethods,
        NULL,
        HistoCube_traverse,
        HistoCube_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__histocube(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_histocube(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_histcube", HistoCubeMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct HistoCubeState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_histocube.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
