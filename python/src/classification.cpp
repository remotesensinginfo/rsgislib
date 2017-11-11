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
#include "cmds/RSGISCmdClassification.h"
#include <vector>

/* An exception object for this module */
/* created in the init function */
struct ClassificationState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ClassificationState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ClassificationState _state;
#endif

// Helper function to extract python sequence to array of strings
static std::string *ExtractStringArrayFromSequence(PyObject *sequence, int *nElements) {
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

static PyObject *Classification_CollapseClasses(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat, *pszClassesColumn;
    PyObject *pClassIntCol = Py_None;
    if( !PyArg_ParseTuple(args, "ssss|O:collapseClasses", &pszInputImage, &pszOutputFile, &pszGDALFormat, &pszClassesColumn, &pClassIntCol))
    {
        return NULL;
    }
    
    bool classIntColPresent = false;
    std::string classIntColStr = "";
    if( pClassIntCol == Py_None )
    {
        classIntColPresent = false;
        classIntColStr = "";
    }
    else
    {
        // convert to a string
        if(RSGISPY_CHECK_STRING(pClassIntCol))
        {
            classIntColPresent = true;
            classIntColStr = RSGISPY_STRING_EXTRACT(pClassIntCol);
        }
        else
        {
            PyErr_SetString(GETSTATE(self)->error, "ClassIntCol name must be a string if provided.\n");
            return NULL;
        }
    }
    
    try
    {
        rsgis::cmds::executeCollapseRAT2Class(std::string(pszInputImage), std::string(pszOutputFile), std::string(pszGDALFormat), std::string(pszClassesColumn), classIntColStr, classIntColPresent);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *Classification_Colour3Bands(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    if( !PyArg_ParseTuple(args, "sss:colour3Band", &pszInputImage, &pszOutputFile, &pszGDALFormat))
        return NULL;
    
    try
    {
        rsgis::cmds::executeGenerate3BandFromColourTable(std::string(pszInputImage), std::string(pszOutputFile), std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Classification_GenRandomAccuracyPts(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputShp, *pszClassImgCol, *pszClassImgVecCol, *pszClassRefVecCol;
    int numPts;
    int force = false;
    int seed = 10;
    
    if( !PyArg_ParseTuple(args, "sssssi|ii:generateRandomAccuracyPts", &pszInputImage, &pszOutputShp, &pszClassImgCol, &pszClassImgVecCol, &pszClassRefVecCol, &numPts, &seed, &force))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeGenerateRandomAccuracyPts(std::string(pszInputImage), std::string(pszOutputShp), std::string(pszClassImgCol), std::string(pszClassImgVecCol), std::string(pszClassRefVecCol), numPts, seed, force);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Classification_GenStratifiedRandomAccuracyPts(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputShp, *pszClassImgCol, *pszClassImgVecCol, *pszClassRefVecCol;
    int numPts;
    int force = false;
    int seed = 10;
    int usePxlLst = false;
    
    if( !PyArg_ParseTuple(args, "sssssi|iii:generateStratifiedRandomAccuracyPts", &pszInputImage, &pszOutputShp, &pszClassImgCol, &pszClassImgVecCol, &pszClassRefVecCol, &numPts, &seed, &force, &usePxlLst))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeGenerateStratifiedRandomAccuracyPts(std::string(pszInputImage), std::string(pszOutputShp), std::string(pszClassImgCol), std::string(pszClassImgVecCol), std::string(pszClassRefVecCol), numPts, seed, force, usePxlLst);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Classification_PopClassInfoAccuracyPts(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputShp, *pszClassImgCol, *pszClassImgVecCol;
    PyObject *classRefVecColObj;
    
    if( !PyArg_ParseTuple(args, "ssss|O:popClassInfoAccuracyPts", &pszInputImage, &pszInputShp, &pszClassImgCol, &pszClassImgVecCol, &classRefVecColObj))
    {
        return NULL;
    }
    
    bool addRefCol = false;
    std::string pszClassRefVecCol = "";
    
    if(RSGISPY_CHECK_STRING(classRefVecColObj))
    {
        pszClassRefVecCol = RSGISPY_STRING_EXTRACT(classRefVecColObj);
        addRefCol = true;
    }
    
    try
    {
        rsgis::cmds::executePopClassInfoAccuracyPts(std::string(pszInputImage), std::string(pszInputShp), std::string(pszClassImgCol), std::string(pszClassImgVecCol), pszClassRefVecCol, addRefCol);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}




// Our list of functions in this module
static PyMethodDef ClassificationMethods[] = {
{"collapseClasses", Classification_CollapseClasses, METH_VARARGS,
"classification.collapseClasses(inputimage, outputimage, gdalformat, classColumn, classIntCol)\n"
"Collapses an attribute table with a large number of classified clumps (segments) to\n"
"a attribute table with a single row per class (i.e. a classification rather than segmentation.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input file with attribute table.\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* gdalformat is a string with the output image format for the GDAL driver.\n"
"* classColumn is a string with the name of the column with the class names - internally this will be treated as a string column even if a numerical column is specified.\n"
"* classIntCol is a sting specifying the name of a column with the integer class representation. This is an optional parameter but if specified then the int reprentation of the classes will be reserved."},

{"colour3bands", Classification_Colour3Bands, METH_VARARGS,
"classification.colour3bands(inputimage, outputimage, gdalformat)\n"
"Generates a 3 band colour image from the colour table in the input file.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input file with attribute table.\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* gdalformat is a string with the output image format for the GDAL driver.\n"},
    
{"generateRandomAccuracyPts", Classification_GenRandomAccuracyPts, METH_VARARGS,
"classification.generateRandomAccuracyPts(inputImage, outputShp, classImgCol, classImgVecCol, classRefVecCol, numPts, seed, force)\n"
"Generates a set of random points for accuracy assessment.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input image with attribute table.\n"
"* outputShp is a string containing the name and path of the output shapefile.\n"
"* classImgCol is a string speciyfing the name of the column in the image file containing the class names.\n"
"* classImgVecCol is a string specifiying the output column in the shapefile for the classified class names.\n"
"* classRefVecCol is a string specifiying an output column in the shapefile which can be used in the accuracy assessment for the reference data.\n"
"* numPts is an int specifying the total number of points which should be created.\n"
"* seed is an int specifying the seed for the random number generator. (Optional: Default 10)\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists. (Optional: Default False)\n"
},

{"generateStratifiedRandomAccuracyPts", Classification_GenStratifiedRandomAccuracyPts, METH_VARARGS,
"classification.generateStratifiedRandomAccuracyPts(inputImage, outputShp, classImgCol, classImgVecCol, classRefVecCol, numPts, seed, force, usePxlLst)\n"
"Generates a set of stratified random points for accuracy assessment.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input image with attribute table.\n"
"* outputShp is a string containing the name and path of the output shapefile.\n"
"* classImgCol is a string speciyfing the name of the column in the image file containing the class names.\n"
"* classImgVecCol is a string specifiying the output column in the shapefile for the classified class names.\n"
"* classRefVecCol is a string specifiying an output column in the shapefile which can be used in the accuracy assessment for the reference data.\n"
"* numPts is an int specifying the number of points for each class which should be created.\n"
"* seed is an int specifying the seed for the random number generator. (Optional: Default 10)\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists. (Optional: Default False)\n"
"* usePxlLst is a bool, if there are only a small number of pixels then creating a list of all the pixel locations will speed up processing. (Optional: Default False)\n"
},
    
{"popClassInfoAccuracyPts", Classification_PopClassInfoAccuracyPts, METH_VARARGS,
"classification.popClassInfoAccuracyPts(inputImage, inputShp, classImgCol, classImgVecCol, classRefVecCol)\n"
"Generates a set of stratified random points for accuracy assessment.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name and path of the input image with attribute table.\n"
"* inputShp is a string containing the name and path of the input shapefile.\n"
"* classImgCol is a string speciyfing the name of the column in the image file containing the class names.\n"
"* classImgVecCol is a string specifiying the output column in the shapefile for the classified class names.\n"
"* classRefVecCol is an optional string specifiying an output column in the shapefile which can be used in the accuracy assessment for the reference data.\n"
},

    {NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int Classification_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int Classification_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_classification",
        NULL,
        sizeof(struct ClassificationState),
        ClassificationMethods,
        NULL,
        Classification_traverse,
        Classification_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__classification(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_classification(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_classification", ClassificationMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ClassificationState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_classification.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
