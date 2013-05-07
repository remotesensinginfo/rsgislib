/*
 *  imageutils.cpp
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
#include "cmds/RSGISCmdImageUtils.h"

/* An exception object for this module */
/* created in the init function */
struct ImageUtilsState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ImageUtilsState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ImageUtilsState _state;
#endif

static PyObject *ImageUtils_StretchImage(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat, *pszOutStatsFile;
    int saveOutStats, ignoreZeros, onePassSD;
    int nOutDataType, nStretchType;
    float fStretchParam;
    if( !PyArg_ParseTuple(args, "ssisiisiif:stretchImage", &pszInputImage, &pszOutputFile, &saveOutStats, 
                                &pszOutStatsFile, &ignoreZeros, &onePassSD, &pszGDALFormat, &nOutDataType, &nStretchType,
                                &fStretchParam))
        return NULL;

    try
    {
        rsgis::cmds::executeStretchImage(pszInputImage, pszOutputFile, saveOutStats, pszOutStatsFile, ignoreZeros,
                    onePassSD, pszGDALFormat, (rsgis::RSGISLibDataType)nOutDataType, 
                    (rsgis::cmds::RSGISStretches)nStretchType, fStretchParam);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_maskImage(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszImageMask, *pszOutputImage, *pszGDALFormat;
    int nDataType;
    float outValue, maskValue;
    if( !PyArg_ParseTuple(args, "ssssiff:maskImage", &pszInputImage, &pszImageMask, &pszOutputImage, 
                                &pszGDALFormat, &nDataType, &outValue, &maskValue ))
        return NULL;

    try
    {
        rsgis::cmds::executeMaskImage(pszInputImage, pszImageMask, pszOutputImage, pszGDALFormat, 
                            (rsgis::RSGISLibDataType)nDataType, outValue, maskValue);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_createTiles(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszImageBase, *pszGDALFormat, *pszExt;
    float width, height, tileOverlap; // Command actually needs ints. Won't complain if floats are provided
    int offsetTiling, nDataType;
    /* Check if required parameters are present and of the correct type
       The type and order are specified by ssfffisis.
       I.e., string, string, float ...etc
    */
    if( !PyArg_ParseTuple(args, "ssfffisis:createTiles", &pszInputImage, &pszImageBase,
                                &width, &height, &tileOverlap, &offsetTiling, &pszGDALFormat, 
                                &nDataType, &pszExt ))
        return NULL;

    PyObject *pOutList;
    try
    {
        std::vector<std::string> outFileNames;
        rsgis::cmds::executeCreateTiles(pszInputImage, pszImageBase, width, height, tileOverlap,
                        offsetTiling, pszGDALFormat, (rsgis::RSGISLibDataType)nDataType, 
                        pszExt, &outFileNames);

        pOutList = PyList_New(outFileNames.size());
        Py_ssize_t nIndex = 0;
        for( std::vector<std::string>::iterator itr = outFileNames.begin(); itr != outFileNames.end(); itr++)
        {
            PyObject *pVal = RSGISPY_CREATE_STRING((*itr).c_str());
            PyList_SetItem(pOutList, nIndex, pVal ); // steals a reference
            nIndex++;
        }

    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    return pOutList;
}


// Our list of functions in this module
static PyMethodDef ImageUtilsMethods[] = {
    {"stretchImage", ImageUtils_StretchImage, METH_VARARGS, 
"Stretch\n"
"call signature: imageutils.stretchImage(inputimage, outputimage, saveoutstats, outstatsfile, ignorezeros, onepasssd, gdalformat, outtype, stretchtype, stretchparam)\n"
"where:\n"
"  inputImage is a string containing the name of the input file\n"
"  outputImage is a string containing the name of the output file\n"
"  saveoutstats is a bool\n"
"  outstatsfile is a string\n"
"  ignorezeros is a bool\n"
"  onepasssd is a bool\n"
"  gdalformat is a string\n"
"  outtype is a rsgislib.TYPE_* value\n"
"  stretchtype is a STRETCH_* value\n"},

    {"maskImage", ImageUtils_maskImage, METH_VARARGS,
"Mask\n"
"call signature: imageutils.maskImage(inputimage, imagemask, outputimage, gdalformat, type, outvalue, maskvalue)\n"
"where:\n"
"  inputImage is a string containing the name of the input file\n"
"  imagemask is a string\n"
"  outputimage is a string\n"
"  gdalformat is a string\n"
"  type is a rsgislib.TYPE_* value\n"
"  outvalue is a float\n"
"  maskvalue is a float\n"},

    {"createTiles", ImageUtils_createTiles, METH_VARARGS,
"Create tiles from a larger image, useful for splitting a large image into multiple smaller ones for processing.\n"
"call signature: imageutils.createTiles(inputimage, baseimage, width, height, overlap, offsettiling, gdalformat, type, ext)\n"
"  inputImage is a string containing the name of the input file\n"
"  baseimage is a string containing the base name of the output file\n    the number of the tile and file extension will be appended.\n"
"  width is the width of each tile, in pixels.\n"
"  height is the height of each tile, in pixels.\n"
"  overlap is the overlap between tiles, in pixels\n"
"  offsettiling is a bool, determining if tiles should start halfway into the image\n    useful for generating overlapping sets of tiles.\n"
"  gdalformat is a string providing the output format of the tiles (e.g., KEA).\n"
"  type is a rsgislib.TYPE_* value providing the output data type of the tiles.\n"
"  ext is a string providing the extension for the tiles (as required by the specified data type).\n"
"\nA list of strings containing the filenames is returned\n"},

    {NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int ImageUtils_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int ImageUtils_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_imageutils",
        NULL,
        sizeof(struct ImageUtilsState),
        ImageUtilsMethods,
        NULL,
        ImageUtils_traverse,
        ImageUtils_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__imageutils(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_imageutils(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_imageutils", ImageUtilsMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ImageUtilsState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_imageutils.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

    // add constants
    PyModule_AddIntConstant(pModule, "STRETCH_LINEARMINMAX", rsgis::cmds::linearMinMax);
    PyModule_AddIntConstant(pModule, "STRETCH_LINEARPERCENT", rsgis::cmds::linearPercent);
    PyModule_AddIntConstant(pModule, "STRETCH_LINEARSTDDEV", rsgis::cmds::linearStdDev);
    PyModule_AddIntConstant(pModule, "STRETCH_HISTOGRAM", rsgis::cmds::histogram);
    PyModule_AddIntConstant(pModule, "STRETCH_EXPONENTIAL", rsgis::cmds::exponential);
    PyModule_AddIntConstant(pModule, "STRETCH_LOGARITHMIC", rsgis::cmds::logarithmic);
    PyModule_AddIntConstant(pModule, "STRETCH_POWERLAW", rsgis::cmds::powerLaw);

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
