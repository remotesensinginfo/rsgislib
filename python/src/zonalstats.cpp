/*
 *  zonalstats.cpp
 *  RSGIS_LIB
 *
 *  Created by Dan Clewley on 09/08/2013.
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
#include "cmds/RSGISCmdZonalStats.h"

/* An exception object for this module */
/* created in the init function */
struct ZonalStatsState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ZonalStatsState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ZonalStatsState _state;
#endif

static void FreePythonObjects(std::vector<PyObject*> toFree) {
    std::vector<PyObject*>::iterator iter;
    for(iter = toFree.begin(); iter != toFree.end(); ++iter) {
        Py_XDECREF(*iter);
    }
}

static PyObject *ZonalStats_PointValue2SHP(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputVector, *pszOutputVector;
    int force, useBandNames;
    if( !PyArg_ParseTuple(args, "sssii:stretchImage", &pszInputImage, &pszInputVector, &pszOutputVector, 
                                &force, &useBandNames))
        return NULL;

    try
    {
        rsgis::cmds::executePointValue(pszInputImage, pszInputVector, pszOutputVector, false, force, useBandNames);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ZonalStats_PointValue2TXT(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputVector, *pszOutputTxt;
    int useBandNames;
    if( !PyArg_ParseTuple(args, "sssi:stretchImage", &pszInputImage, &pszInputVector, &pszOutputTxt, 
                                &useBandNames))
        return NULL;

    try
    {
        rsgis::cmds::executePointValue(pszInputImage, pszInputVector, pszOutputTxt, true, false, useBandNames);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *ZonalStats_PixelStats2SHP(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputVector, *pszOutputVector;
    int force, useBandNames;
    PyObject *pBandAttZonalStatsCmds;
    if( !PyArg_ParseTuple(args, "sssoii:stretchImage", &pszInputImage, &pszInputVector, &pszOutputVector, 
                                &pBandAttZonalStatsCmds, &force, &useBandNames))
        return NULL;

    PyObject *o = pBandAttZonalStatsCmds;     // the python object

    rsgis::cmds::RSGISBandAttZonalStatsCmds *cmdObj = new rsgis::cmds::RSGISBandAttZonalStatsCmds();   // the c++ object we need to pass pointers of

    // declare and initialise pointers for all the attributes of the struct
    PyObject *pMinThreshold, *pMaxThreshold, *pCalcCount, *pCalcMin, *pCalcMax, *pCalcMean, *pCalcStDev, *pCalcMode, *pCalcSum;
    pMinThreshold = pMaxThreshold = pCalcCount = pCalcMin = pCalcMax = pCalcMean = pCalcStDev = pCalcMode = pCalcSum = NULL;

    std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference
    extractedAttributes.push_back(o);

    
    /* Check if values have been set. 
        If not set assume false.
        If set, assume true for now - will check the value later
    */
    pCalcCount = PyObject_GetAttrString(o, "count");
    extractedAttributes.push_back(pCalcCount);
    cmdObj->calcCount = !(pCalcCount == NULL || !RSGISPY_CHECK_STRING(pCalcCount));
    
    pCalcMin = PyObject_GetAttrString(o, "min");
    extractedAttributes.push_back(pCalcMin);
    cmdObj->calcMin = !(pCalcMin == NULL || !RSGISPY_CHECK_STRING(pCalcMin));

    pCalcMax = PyObject_GetAttrString(o, "max");
    extractedAttributes.push_back(pCalcMax);
    cmdObj->calcMax = !(pCalcMax == NULL || !RSGISPY_CHECK_STRING(pCalcMax));
    
    pCalcMean = PyObject_GetAttrString(o, "mean");
    extractedAttributes.push_back(pCalcMean);
    cmdObj->calcMean = !(pCalcMean == NULL || !RSGISPY_CHECK_STRING(pCalcMean));
    
    pCalcStDev = PyObject_GetAttrString(o, "stDev");
    extractedAttributes.push_back(pCalcStDev);
    cmdObj->calcStdDev= !(pCalcStDev == NULL || !RSGISPY_CHECK_STRING(pCalcStDev));
    
    pCalcMode = PyObject_GetAttrString(o, "mode");
    extractedAttributes.push_back(pCalcMode);
    cmdObj->calcMode = !(pCalcMode == NULL || !RSGISPY_CHECK_STRING(pCalcMode));
    
    pCalcSum = PyObject_GetAttrString(o, "sum");
    extractedAttributes.push_back(pCalcSum);
    cmdObj->calcSum = !(pCalcSum == NULL || !RSGISPY_CHECK_STRING(pCalcSum));

    // check the calcValue and extract fields if required
    if(cmdObj->calcCount) {cmdObj->calcCount = RSGISPY_INT_EXTRACT(pCalcCount);}
    if(cmdObj->calcMin) {cmdObj->calcMin = RSGISPY_INT_EXTRACT(pCalcMin);}
    if(cmdObj->calcMax) {cmdObj->calcMax = RSGISPY_INT_EXTRACT(pCalcMax);}
    if(cmdObj->calcMean) {cmdObj->calcCount = RSGISPY_INT_EXTRACT(pCalcMean);}
    if(cmdObj->calcStdDev) {cmdObj->calcCount = RSGISPY_INT_EXTRACT(pCalcStDev);}
    if(cmdObj->calcMode) {cmdObj->calcCount = RSGISPY_INT_EXTRACT(pCalcMode);}
    if(cmdObj->calcSum) {cmdObj->calcCount = RSGISPY_INT_EXTRACT(pCalcSum);}
    
    // Check if thresholds have been set - use default values (+/- Inf) if not.
    pMinThreshold = PyObject_GetAttrString(o, "minThreshold");
    extractedAttributes.push_back(pMinThreshold);
    if( ( pMinThreshold == NULL ) || ( pMinThreshold == Py_None ) || !RSGISPY_CHECK_FLOAT(pMinThreshold) ) 
    {
        cmdObj->minThreshold = -std::numeric_limits<double>::infinity();
    }
    else{cmdObj->minThreshold = RSGISPY_FLOAT_EXTRACT(pMinThreshold);}
    
    pMaxThreshold = PyObject_GetAttrString(o, "maxThreshold");
    extractedAttributes.push_back(pMaxThreshold);
    if( ( pMaxThreshold == NULL ) || ( pMaxThreshold == Py_None ) || !RSGISPY_CHECK_FLOAT(pMaxThreshold) ) 
    {
        cmdObj->maxThreshold = +std::numeric_limits<double>::infinity();
    }
    else{cmdObj->maxThreshold = RSGISPY_FLOAT_EXTRACT(pMaxThreshold);}
    
    FreePythonObjects(extractedAttributes);
    
    try
    {
        rsgis::cmds::executePixelStats(pszInputImage, pszInputVector, pszOutputVector, cmdObj, 
            "", false, force, useBandNames);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef ZonalStatsMethods[] = {
    {"pointValue2SHP", ZonalStats_PointValue2SHP, METH_VARARGS, 
"Stretch\n"
"call signature: zonalstats.pointValue2SHP(inputimage, inputvector, outputvector, force, useBandNames)\n"
"where:\n"
"  * inputimage is a string containing the name of the input image\n"
"  * inputvector is a string containing the name of the input vector\n"
"  * outputvector is a string containing the name of the output vector\n"
"  * force is a bool, specifying whether to force removal of the output vector if it exists\n"
"  * useBandNames is a bool, specifying whether to use the band names of the input dataset in the output file (if not uses b1, b2, etc.,\n"},

    {"pointValue2TXT", ZonalStats_PointValue2TXT, METH_VARARGS, 
"Stretch\n"
"call signature: zonalstats.pointValue2TXT(inputimage, inputvector, outputtxt, useBandNames)\n"
"where:\n"
"  * inputimage is a string containing the name of the input image\n"
"  * inputvector is a string containing the name of the input vector\n"
"  * outputtxt is a string containing the name of the output text file\n"
"  * useBandNames is a bool, specifying whether to use the band names of the input dataset in the output file (if not uses b1, b2, etc.,\n"},

    {"pixelValue2SHP", ZonalStats_PixelStats2SHP, METH_VARARGS, 
"Stretch\n"
"call signature: zonalstats.pixelValue2SHP(inputimage, inputvector, outputvector, zonalattributes, force, useBandNames)\n"
"where:\n"
"  * inputimage is a string containing the name of the input image\n"
"  * inputvector is a string containing the name of the input vector\n"
"  * outputvector is a string containing the name of the output vector\n"
"  * zonalattributes is a sequence containing:\n"
"\n"
"       * minThreshold, a float providing the minimum pixel value to include when calculating statistics.\n"
"       * maxThreshold, a float providing the maximum pixel value to include when calculating statistics.\n"
"       * count, a bool specifying whether to report a count of pixels between thresholds.\n"
"       * min, a bool specifying whether to report the minimum of pixels between thresholds.\n"
"       * max, a bool specifying whether to report the maximum of pixels between thresholds.\n"
"       * mean, a bool specifying whether to report the mean of pixels between thresholds.\n"
"       * stDev, a bool specifying whether to report the standard deviation of pixels between thresholds.\n"
"       * mode, a bool specifying whether to report the mode of pixels between thresholds (for integer datasets only).\n"
"       * sum, a bool specifying whether to report the sum of pixels between thresholds.\n"
"\n"
"  * force is a bool, specifying whether to force removal of the output vector if it exists\n"
"  * useBandNames is a bool, specifying whether to use the band names of the input dataset in the output file (if not uses b1, b2, etc.,\n"},

    {NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int ZonalStats_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int ZonalStats_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_zonalstats",
        NULL,
        sizeof(struct ZonalStatsState),
        ZonalStatsMethods,
        NULL,
        ZonalStats_traverse,
        ZonalStats_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__zonalstats(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_zonalstats(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_zonalstats", ZonalStatsMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ZonalStatsState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_zonalstats.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
