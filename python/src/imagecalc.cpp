/*
 *  imagecalc.cpp
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

#include <Python.h>
#include "rsgispy_common.h"
#include "cmds/RSGISCmdImageCalc.h"

/* An exception object for this module */
/* created in the init function */
struct ImageCalcState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ImageCalcState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ImageCalcState _state;
#endif

static PyObject *ImageCalc_BandMath(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {"outputimg", "exp", "gdalformat", "datatype", "banddefseq", "expbandname", "outputexists", NULL};
    const char *pszOutputFile, *pszExpression, *pszGDALFormat;
    int nDataType;
    int bExpBandName = 0;
    int bOutputImgExists = 0;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssiO|ii:bandMath", kwlist, &pszOutputFile, &pszExpression, &pszGDALFormat, &nDataType, &pBandDefnObj, &bExpBandName, &bOutputImgExists))
    {
        return NULL;
    }

    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return NULL;
    }

    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    rsgis::cmds::VariableStruct *pRSGISStruct = new rsgis::cmds::VariableStruct[nBandDefns];

    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);

        PyObject *pBandName = PyObject_GetAttrString(o, "bandName");
        if( ( pBandName == NULL ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'bandName\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return NULL;
        }

        PyObject *pFileName = PyObject_GetAttrString(o, "fileName");
        if( ( pFileName == NULL ) || ( pFileName == Py_None ) || !RSGISPY_CHECK_STRING(pFileName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'fileName\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pFileName);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return NULL;
        }

        PyObject *pBandIndex = PyObject_GetAttrString(o, "bandIndex");
        if( ( pBandIndex == NULL ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find integer attribute \'bandIndex\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return NULL;
        }

        pRSGISStruct[n].name = RSGISPY_STRING_EXTRACT(pBandName);
        pRSGISStruct[n].image = RSGISPY_STRING_EXTRACT(pFileName);
        pRSGISStruct[n].bandNum = RSGISPY_INT_EXTRACT(pBandIndex);

        Py_DECREF(pBandName);
        Py_DECREF(pFileName);
        Py_DECREF(pBandIndex);
        Py_DECREF(o);
    }

    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        bool useExpAsbandName = (bool)bExpBandName;
        bool outputImgExists = (bool)bOutputImgExists;
        rsgis::cmds::executeBandMaths(pRSGISStruct, nBandDefns, pszOutputFile, pszExpression, pszGDALFormat, type, useExpAsbandName, outputImgExists);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    delete[] pRSGISStruct;

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ImageMath(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {"inputimg", "outputimg", "exp", "gdalformat", "datatype", "expbandname", "outputexists", NULL};
    const char *pszInputImage, *pszOutputFile, *pszExpression, *pszGDALFormat;
    int nDataType;
    int bExpBandName = 0;
    int bOutputImgExists = 0;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssi|i:imageMath", kwlist, &pszInputImage, &pszOutputFile, &pszExpression, &pszGDALFormat, &nDataType, &bExpBandName, &bOutputImgExists))
    {
        return NULL;
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        bool useExpAsbandName = (bool)bExpBandName;
        bool outputImgExists = (bool)bOutputImgExists;
        rsgis::cmds::executeImageMaths(pszInputImage, pszOutputFile, pszExpression, pszGDALFormat, type, useExpAsbandName, outputImgExists);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ImageBandMath(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {"inputimg", "outputimg", "exp", "gdalformat", "datatype", "expbandname", "outputexists", NULL};
    const char *pszInputImage, *pszOutputFile, *pszExpression, *pszGDALFormat;
    int nDataType;
    int bExpBandName = 0;
    int bOutputImgExists = 0;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssi|i:imageBandMath", kwlist, &pszInputImage, &pszOutputFile, &pszExpression, &pszGDALFormat, &nDataType, &bExpBandName, &bOutputImgExists))
    {
        return NULL;
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;
        bool useExpAsbandName = (bool)bExpBandName;
        bool outputImgExists = (bool)bOutputImgExists;
        rsgis::cmds::executeImageBandMaths(pszInputImage, pszOutputFile, pszExpression, pszGDALFormat, type, useExpAsbandName, outputImgExists);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalc_KMeansClustering(PyObject *self, PyObject *args) {
    const char *pszInputImage, *pszOutputFile;
    unsigned int nNumClusters, nMaxNumIterations, nSubSample;
    int nIgnoreZeros; // passed as a bool - seems the only way to pass into C
    float fDegreeOfChange;
    int nClusterMethod;
    if( !PyArg_ParseTuple(args, "ssIIIifi:kMeansClustering", &pszInputImage, &pszOutputFile, &nNumClusters,
                                &nMaxNumIterations, &nSubSample, &nIgnoreZeros, &fDegreeOfChange, &nClusterMethod ))
        return NULL;
    
    try
    {
        rsgis::cmds::executeKMeansClustering(pszInputImage, pszOutputFile, nNumClusters, nMaxNumIterations,
                            nSubSample, nIgnoreZeros, fDegreeOfChange, (rsgis::cmds::RSGISInitClustererMethods)nClusterMethod);
        
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ISODataClustering(PyObject *self, PyObject *args) {
    const char *pszInputImage, *pszOutputFile;
    unsigned int nNumClusters, nMaxNumIterations, nSubSample, minNumFeatures, minNumClusters;
    unsigned int startIteration, endIteration;
    int nIgnoreZeros; // passed as a bool - seems the only way to pass into C
    float fDegreeOfChange, fMinDistBetweenClusters, maxStdDev;
    int nClusterMethod;
    if( !PyArg_ParseTuple(args, "ssIIIififIfIII:isoDataClustering", &pszInputImage, &pszOutputFile, &nNumClusters,
                                &nMaxNumIterations, &nSubSample, &nIgnoreZeros, &fDegreeOfChange, &nClusterMethod,
                                &fMinDistBetweenClusters, &minNumFeatures, &maxStdDev, &minNumClusters,
                                &startIteration, &endIteration ))
        return NULL;

    try
    {
        rsgis::cmds::executeISODataClustering(pszInputImage, pszOutputFile, nNumClusters, nMaxNumIterations,
                            nSubSample, nIgnoreZeros, fDegreeOfChange, (rsgis::cmds::RSGISInitClustererMethods)nClusterMethod, fMinDistBetweenClusters,
                            minNumFeatures, maxStdDev, minNumClusters, startIteration, endIteration);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_MahalanobisDistFilter(PyObject *self, PyObject *args) {
    const char *inputImage, *outputImage, *gdalFormat;
    unsigned int datatype, winSize;

    if(!PyArg_ParseTuple(args, "ssIsI:mahalanobisDistFilter", &inputImage, &outputImage, &winSize, &gdalFormat, &datatype))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;

    try {
        rsgis::cmds::executeMahalanobisDistFilter(inputImage, outputImage, winSize, gdalFormat, type);
    } catch(rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_MahalanobisDist2ImgFilter(PyObject *self, PyObject *args) {
    const char *inputImage, *outputImage, *gdalFormat;
    unsigned int datatype, winSize;

    if(!PyArg_ParseTuple(args, "ssIsI:mahalanobisDist2ImgFilter", &inputImage, &outputImage, &winSize, &gdalFormat, &datatype))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;

    try {
        rsgis::cmds::executeMahalanobisDist2ImgFilter(inputImage, outputImage, winSize, gdalFormat, type);
    } catch(rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ImageCalcDistance(PyObject *self, PyObject *args) {
    const char *inputImage, *outputImage, *gdalFormat;

    if(!PyArg_ParseTuple(args, "sss:imageCalcDistance", &inputImage, &outputImage, &gdalFormat))
        return NULL;

    try {
        rsgis::cmds::executeImageCalcDistance(inputImage, outputImage, gdalFormat);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ImagePixelColumnSummary(PyObject *self, PyObject *args) {
    const char *inputImage, *outputImage, *gdalFormat;
    unsigned int datatype;
    int useNoDataValue;
    float noDataValue;
    PyObject *summaryStats;
    
    if(!PyArg_ParseTuple(args, "ssOsIfi:imagePixelColumnSummary", &inputImage, &outputImage, &summaryStats, &gdalFormat, &datatype, &noDataValue, &useNoDataValue))
        return NULL;

    // get the kw attrs from the object
    PyObject *pCalcMin = PyObject_GetAttrString(summaryStats, "calcMin");
    if( ( pCalcMin == NULL ) || ( pCalcMin == Py_None ) || !RSGISPY_CHECK_INT(pCalcMin) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcMin\'" );
        Py_XDECREF(pCalcMin);
        return NULL;
    }

    PyObject *pCalcMax = PyObject_GetAttrString(summaryStats, "calcMax");
    if( ( pCalcMax == NULL ) || ( pCalcMax == Py_None ) || !RSGISPY_CHECK_INT(pCalcMax) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcMax\'" );
        Py_XDECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }

    PyObject *pCalcMean = PyObject_GetAttrString(summaryStats, "calcMean");
    if( ( pCalcMean == NULL ) || ( pCalcMean == Py_None ) || !RSGISPY_CHECK_INT(pCalcMean) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcMean\'" );
        Py_XDECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }

    PyObject *pCalcSum = PyObject_GetAttrString(summaryStats, "calcSum");
    if( ( pCalcSum == NULL ) || ( pCalcSum == Py_None ) || !RSGISPY_CHECK_INT(pCalcSum) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcSum\'" );
        Py_XDECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }

    PyObject *pCalcStdDev = PyObject_GetAttrString(summaryStats, "calcStdDev");
    if( ( pCalcStdDev == NULL ) || ( pCalcStdDev == Py_None ) || !RSGISPY_CHECK_INT(pCalcStdDev) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcStdDev\'" );
        Py_XDECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }

    PyObject *pCalcMedian = PyObject_GetAttrString(summaryStats, "calcMedian");
    if( ( pCalcMedian == NULL ) || ( pCalcMedian == Py_None ) || !RSGISPY_CHECK_INT(pCalcMedian) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcMedian\'" );
        Py_XDECREF(pCalcMedian);
        Py_DECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }
    
    PyObject *pCalcMode = PyObject_GetAttrString(summaryStats, "calcMode");
    if( ( pCalcMode == NULL ) || ( pCalcMode == Py_None ) || !RSGISPY_CHECK_INT(pCalcMode) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcMode\'" );
        Py_XDECREF(pCalcMode);
        Py_DECREF(pCalcMedian);
        Py_DECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }

    PyObject *pMin = PyObject_GetAttrString(summaryStats, "min");
    if( ( pMin == NULL ) || ( pMin == Py_None ) || !RSGISPY_CHECK_FLOAT(pMin) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find float attribute \'min\'" );
        Py_XDECREF(pMin);
        Py_DECREF(pCalcMode);
        Py_DECREF(pCalcMedian);
        Py_DECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }

    PyObject *pMax = PyObject_GetAttrString(summaryStats, "max");
    if( ( pMax == NULL ) || ( pMax == Py_None ) || !RSGISPY_CHECK_FLOAT(pMax) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find float attribute \'max\'" );
        Py_XDECREF(pMax);
        Py_DECREF(pMin);
        Py_DECREF(pCalcMode);
        Py_DECREF(pCalcMedian);
        Py_DECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }

    PyObject *pMean = PyObject_GetAttrString(summaryStats, "mean");
    if( ( pMean == NULL ) || ( pMean == Py_None ) || !RSGISPY_CHECK_FLOAT(pMean) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find float attribute \'mean\'" );
        Py_XDECREF(pMean);
        Py_DECREF(pMax);
        Py_DECREF(pMin);
        Py_DECREF(pCalcMode);
        Py_DECREF(pCalcMedian);
        Py_DECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }

    PyObject *pSum = PyObject_GetAttrString(summaryStats, "sum");
    if( ( pSum == NULL ) || ( pSum == Py_None ) || !RSGISPY_CHECK_FLOAT(pSum) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find float attribute \'sum\'" );
        Py_XDECREF(pSum);
        Py_DECREF(pMean);
        Py_DECREF(pMax);
        Py_DECREF(pMin);
        Py_DECREF(pCalcMode);
        Py_DECREF(pCalcMedian);
        Py_DECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }

    PyObject *pStdDev = PyObject_GetAttrString(summaryStats, "stdDev");
    if( ( pStdDev == NULL ) || ( pStdDev == Py_None ) || !RSGISPY_CHECK_FLOAT(pStdDev) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find float attribute \'stdDev\'" );
        Py_XDECREF(pStdDev);
        Py_DECREF(pSum);
        Py_DECREF(pMean);
        Py_DECREF(pMax);
        Py_DECREF(pMin);
        Py_DECREF(pCalcMode);
        Py_DECREF(pCalcMedian);
        Py_DECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }

    PyObject *pMedian = PyObject_GetAttrString(summaryStats, "median");
    if( ( pMedian == NULL ) || ( pMedian == Py_None ) || !RSGISPY_CHECK_FLOAT(pMedian) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find float attribute \'median\'" );
        Py_XDECREF(pMedian);
        Py_DECREF(pStdDev);
        Py_DECREF(pSum);
        Py_DECREF(pMean);
        Py_DECREF(pMax);
        Py_DECREF(pMin);
        Py_DECREF(pCalcMode);
        Py_DECREF(pCalcMedian);
        Py_DECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }
    
    PyObject *pMode = PyObject_GetAttrString(summaryStats, "mode");
    if( ( pMode == NULL ) || ( pMode == Py_None ) || !RSGISPY_CHECK_FLOAT(pMode) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find float attribute \'modes\'" );
        Py_XDECREF(pMode);
        Py_DECREF(pMedian);
        Py_DECREF(pStdDev);
        Py_DECREF(pSum);
        Py_DECREF(pMean);
        Py_DECREF(pMax);
        Py_DECREF(pMin);
        Py_DECREF(pCalcMode);
        Py_DECREF(pCalcMedian);
        Py_DECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return NULL;
    }

    // generate struct to pass to C function from python object attributes
    rsgis::cmds::RSGISCmdStatsSummary summary;
    summary.calcMin = RSGISPY_INT_EXTRACT(pCalcMin);
    summary.calcMax = RSGISPY_INT_EXTRACT(pCalcMax);
    summary.calcMean = RSGISPY_INT_EXTRACT(pCalcMean);
    summary.calcSum = RSGISPY_INT_EXTRACT(pCalcSum);
    summary.calcStdDev = RSGISPY_INT_EXTRACT(pCalcStdDev);
    summary.calcMedian = RSGISPY_INT_EXTRACT(pCalcMedian);
    summary.calcMode = RSGISPY_INT_EXTRACT(pCalcMode);

    summary.min = RSGISPY_FLOAT_EXTRACT(pMin);
    summary.max = RSGISPY_FLOAT_EXTRACT(pMax);
    summary.mean = RSGISPY_FLOAT_EXTRACT(pMean);
    summary.sum = RSGISPY_FLOAT_EXTRACT(pSum);
    summary.stdDev = RSGISPY_FLOAT_EXTRACT(pStdDev);
    summary.median = RSGISPY_FLOAT_EXTRACT(pMedian);
    summary.mode = RSGISPY_FLOAT_EXTRACT(pMode);
    
    Py_DECREF(pCalcMin);
    Py_DECREF(pCalcMax);
    Py_DECREF(pCalcMean);
    Py_DECREF(pCalcSum);
    Py_DECREF(pCalcStdDev);
    Py_DECREF(pCalcMedian);
    Py_DECREF(pCalcMode);
    Py_DECREF(pMin);
    Py_DECREF(pMax);
    Py_DECREF(pMean);
    Py_DECREF(pSum);
    Py_DECREF(pStdDev);
    Py_DECREF(pMedian);
    Py_DECREF(pMode);

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;

    try
    {
        rsgis::cmds::executeImagePixelColumnSummary(inputImage, outputImage, summary, gdalFormat, type, noDataValue, useNoDataValue);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ImagePixelLinearFit(PyObject *self, PyObject *args) {
    const char *inputImage, *outputImage, *gdalFormat, *bandValues;
    double noDataValue;
    int useNoDataValue;

    if(!PyArg_ParseTuple(args, "ssssfi:imagePixelLinearFit", &inputImage, &outputImage, &gdalFormat, &bandValues, &noDataValue, &useNoDataValue))
        return NULL;

    try {
        rsgis::cmds::executeImagePixelLinearFit(inputImage, outputImage, gdalFormat, bandValues, noDataValue, useNoDataValue);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_Normalisation(PyObject *self, PyObject *args) {
    PyObject *pInputImages, *pOutputImages;
    int calcInMinMax;
    double inMin, inMax, outMin, outMax;

    if(!PyArg_ParseTuple(args, "OOiffff:normalisation", &pInputImages, &pOutputImages, &calcInMinMax, &inMin, &inMax, &outMin, &outMax))
        return NULL;

    if( !PySequence_Check(pInputImages) || !PySequence_Check(pOutputImages)) {
        PyErr_SetString(GETSTATE(self)->error, "first two arguments must be sequences");
        return NULL;
    }

    // extract the image strings from the python sequences
    Py_ssize_t nImages = PySequence_Size(pInputImages);
    std::vector<std::string> inputImages, outputImages;
    inputImages.reserve(nImages);
    outputImages.reserve(nImages);

    for(int i = 0; i < nImages; ++i)
    {
        PyObject *inImageObj = PySequence_GetItem(pInputImages, i);
        PyObject *outImageObj = PySequence_GetItem(pOutputImages, i);

        if(!RSGISPY_CHECK_STRING(inImageObj) || !RSGISPY_CHECK_STRING(outImageObj))
        {
            PyErr_SetString(GETSTATE(self)->error, "Input and output images must be strings");
            Py_DECREF(inImageObj);
            Py_DECREF(outImageObj);
            return NULL;
        }

        inputImages.push_back(RSGISPY_STRING_EXTRACT(inImageObj));
        outputImages.push_back(RSGISPY_STRING_EXTRACT(outImageObj));

        Py_DECREF(inImageObj);
        Py_DECREF(outImageObj);
    }

    try {
        rsgis::cmds::executeNormalisation(inputImages, outputImages, calcInMinMax, inMin, inMax, outMin, outMax);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_Correlation(PyObject *self, PyObject *args, PyObject *keywds) 
{
    const char *inputImageA, *inputImageB;
    const char *outputMatrixFile = "";
    PyObject *outCorrelationMatrixList = NULL;
    PyObject *outRow = NULL;
    static char *kwlist[] = {"imageA", "imageB", "outmatrixfile",  NULL};

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ss|s:correlation", kwlist, &inputImageA, &inputImageB, &outputMatrixFile))
        return NULL;

    try 
    {
        unsigned int nrows = 0;
        unsigned int ncols = 0;
        double **outputMatrix = NULL;

        outputMatrix = rsgis::cmds::executeCorrelation(inputImageA, inputImageB, outputMatrixFile, &nrows, &ncols);

        outCorrelationMatrixList = PyTuple_New(nrows);
        
        for(unsigned int j = 0; j < nrows; ++j)
        {
           outRow = PyTuple_New(ncols);
           for(unsigned int i = 0; i < ncols; ++i) 
           {
               if(PyTuple_SetItem(outRow, i, Py_BuildValue("f", outputMatrix[j][i])) == -1)
               {
                    throw rsgis::cmds::RSGISCmdException("Failed to add a value to row");
               }
           }
           if(PyTuple_SetItem(outCorrelationMatrixList, j, Py_BuildValue("O", outRow)) == -1)
           {
               throw rsgis::cmds::RSGISCmdException("Failed to add a row to column");
           }
        }

    } 
    catch (rsgis::cmds::RSGISCmdException &e) 
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    return outCorrelationMatrixList;
}

static PyObject *ImageCalc_Covariance(PyObject *self, PyObject *args) {
    const char *inputImageA, *inputImageB, *inputMatrixA, *inputMatrixB, *outputMatrix;
    int shouldCalcMean;

    if(!PyArg_ParseTuple(args, "ssssis:covariance", &inputImageA, &inputImageB, &inputMatrixA, &inputMatrixB, &shouldCalcMean, &outputMatrix))
        return NULL;

    try {
        rsgis::cmds::executeCovariance(inputImageA, inputImageB, inputMatrixA, inputMatrixB, shouldCalcMean, outputMatrix);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_MeanVector(PyObject *self, PyObject *args) {
    const char *inputImage, *outputMatrix;

    if(!PyArg_ParseTuple(args, "ss:meanVector", &inputImage, &outputMatrix))
        return NULL;

    try {
        rsgis::cmds::executeMeanVector(inputImage, outputMatrix);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_PCA(PyObject *self, PyObject *args)
{
    const char *eigenVectors, *inputImage, *outputImage;
    unsigned int numComponents;
    const char *gdalFormat;
    int datatype;

    if(!PyArg_ParseTuple(args, "sssIsi:pca", &inputImage, &eigenVectors, &outputImage, &numComponents, &gdalFormat, &datatype))
    {
        return NULL;
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;
        rsgis::cmds::executePCA(std::string(inputImage), std::string(eigenVectors), std::string(outputImage), numComponents, std::string(gdalFormat), type);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_Standardise(PyObject *self, PyObject *args) {
    const char *meanVector, *inputImage, *outputImage;

    if(!PyArg_ParseTuple(args, "sss:standardise", &meanVector, &inputImage, &outputImage))
        return NULL;

    try {
        rsgis::cmds::executeStandardise(meanVector, inputImage, outputImage);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ReplaceValuesLessThan(PyObject *self, PyObject *args) {
    const char *inputImage, *outputImage;
    double threshold, value;

    if(!PyArg_ParseTuple(args, "ssff:replaceValuesLessThan", &inputImage, &outputImage, &threshold, &value))
        return NULL;

    try {
        rsgis::cmds::executeReplaceValuesLessThan(inputImage, outputImage, threshold, value);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_UnitArea(PyObject *self, PyObject *args) {
    const char *inputImage, *outputImage, *inputMatrix;

    if(!PyArg_ParseTuple(args, "sss:unitArea", &inputImage, &outputImage, &inputMatrix))
        return NULL;

    try {
        rsgis::cmds::executeUnitArea(inputImage, outputImage, inputMatrix);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_MovementSpeed(PyObject *self, PyObject *args)
{
    // declare variables
    PyObject *inImagesObj, *imageBandsObj, *imageTimesObj;
    const char *outputImage;
    float upper, lower;
    bool failedCheck = false;

    // extract (with basic checking) parameters from called python function
    if(!PyArg_ParseTuple(args, "OOOffs:movementSpeed", &inImagesObj, &imageBandsObj, &imageTimesObj, &upper, &lower, &outputImage))
        return NULL;

    // check that the objects we expect to be are sequences
    if( !PySequence_Check(inImagesObj) || !PySequence_Check(imageBandsObj) || !PySequence_Check(imageTimesObj)) {
        PyErr_SetString(GETSTATE(self)->error, "first three arguments must be sequences");
        return NULL;
    }

    // extract the items from the sequences...
    Py_ssize_t nImages = PySequence_Size(inImagesObj);

    // set up vectors to hold extracted sequence items
    std::vector<std::string> inputImages;
    std::vector<unsigned int> imageBands;
    std::vector<float> imageTimes;

    inputImages.reserve(nImages);
    imageBands.reserve(nImages);
    imageTimes.reserve(nImages);

    // for each image
    for(int i = 0; i < nImages; ++i) {
        // get the elements for this image from each of the sequences
        PyObject *inImageObj = PySequence_GetItem(inImagesObj, i);
        PyObject *imageBandObj = PySequence_GetItem(imageBandsObj, i);
        PyObject *imageTimeObj = PySequence_GetItem(imageTimesObj, i);

        // check they are the expected types
        if(!RSGISPY_CHECK_STRING(inImageObj)) {
            failedCheck = true;
            PyErr_SetString(GETSTATE(self)->error, "Input images must be strings");
        } if(!RSGISPY_CHECK_INT(imageBandObj)) {
            failedCheck = true;
            PyErr_SetString(GETSTATE(self)->error, "Image bands must be integers");
        } if (!RSGISPY_CHECK_FLOAT(imageTimeObj)) {
            failedCheck = true;
            PyErr_SetString(GETSTATE(self)->error, "Image times must be floats");
        }

        if(failedCheck) {
            Py_DECREF(inImageObj);
            Py_DECREF(imageBandObj);
            Py_DECREF(imageTimeObj);
            return NULL;
        }

        // append them to our vectors
        inputImages.push_back(RSGISPY_STRING_EXTRACT(inImageObj));
        imageBands.push_back(RSGISPY_INT_EXTRACT(imageBandObj));
        imageTimes.push_back(RSGISPY_FLOAT_EXTRACT(imageTimeObj));

        // release the python resources
        Py_DECREF(inImageObj);
        Py_DECREF(imageBandObj);
        Py_DECREF(imageTimeObj);
    }

    // run the command
    try {
        rsgis::cmds::executeMovementSpeed(inputImages, imageBands, imageTimes, upper, lower, outputImage);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_CountValsInCols(PyObject *self, PyObject *args) {
    const char *inputImage, *outputImage;
    double upper, lower;

    if(!PyArg_ParseTuple(args, "sffs:countValsInCols", &inputImage, &upper, &lower, &outputImage))
        return NULL;

    try {
        rsgis::cmds::executeCountValsInCols(inputImage, upper, lower, outputImage);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_CalculateRMSE(PyObject *self, PyObject *args) {
    const char *inputImageA, *inputImageB;
    unsigned int bandA, bandB;

    if(!PyArg_ParseTuple(args, "sIsI:calculateRMSE", &inputImageA, &bandA, &inputImageB, &bandB))
    {
        return NULL;
    }
    
    
    PyObject *outVal = PyTuple_New(1);
    try
    {
        double rmseVal = rsgis::cmds::executeCalculateRMSE(inputImageA, bandA, inputImageB, bandB);
        if(PyTuple_SetItem(outVal, 0, Py_BuildValue("d", rmseVal)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'RMSE\' value to the list...");
        }
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    return outVal;
}

static PyObject *ImageCalc_Dist2Geoms(PyObject *self, PyObject *args) {
    const char *inputVector, *outputImage;
    float imgResolution;

    if(!PyArg_ParseTuple(args, "sfs:dist2Geoms", &inputVector, &imgResolution, &outputImage))
        return NULL;

    try {
        rsgis::cmds::executeDist2Geoms(inputVector, imgResolution, outputImage);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ImageBandStats(PyObject *self, PyObject *args) {
    const char *inputImage, *outputFile;
    int ignoreZeros;

    if(!PyArg_ParseTuple(args, "ssi:imageBandStats", &inputImage, &outputFile, &ignoreZeros))
        return NULL;

    try {
        rsgis::cmds::executeImageBandStats(inputImage, outputFile, ignoreZeros);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ImageStats(PyObject *self, PyObject *args) {
    const char *inputImage, *outputFile;
    int ignoreZeros;

    if(!PyArg_ParseTuple(args, "ssi:imageStats", &inputImage, &outputFile, &ignoreZeros))
        return NULL;

    try {
        rsgis::cmds::executeImageStats(inputImage, outputFile, ignoreZeros);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_UnconLinearSpecUnmix(PyObject *self, PyObject *args)
{
    const char *inputImage, *imageFormat, *outputFile, *endmembersFile;
    float lsumGain = 1;
    float lsumOffset = 0;
    int datatype;

    if(!PyArg_ParseTuple(args, "ssiss|ff:unconLinearSpecUnmix", &inputImage, &imageFormat, &datatype, &outputFile, &endmembersFile, &lsumGain, &lsumOffset))
    {
        return NULL;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;

    try
    {
        rsgis::cmds::executeUnconLinearSpecUnmix(inputImage, imageFormat, type, lsumGain, lsumOffset, outputFile, endmembersFile);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ExhconLinearSpecUnmix(PyObject *self, PyObject *args)
{
    const char *inputImage, *imageFormat, *outputFile, *endmembersFile;
    float lsumGain = 1;
    float lsumOffset = 0;
    float stepResolution;
    int datatype;

    if(!PyArg_ParseTuple(args, "ssissf|ff:exhconLinearSpecUnmix", &inputImage, &imageFormat, &datatype, &outputFile, &endmembersFile, &stepResolution, &lsumGain, &lsumOffset))
    {
        return NULL;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;

    try
    {
        rsgis::cmds::executeExhconLinearSpecUnmix(inputImage, imageFormat, type, lsumGain, lsumOffset, outputFile, endmembersFile, stepResolution);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ConSum1LinearSpecUnmix(PyObject *self, PyObject *args)
{
    const char *inputImage, *imageFormat, *outputFile, *endmembersFile;
    float lsumGain = 1;
    float lsumOffset = 0;
    float lsumWeight = 1;
    int datatype;

    if(!PyArg_ParseTuple(args, "ssifss|ff:conSum1LinearSpecUnmix", &inputImage, &imageFormat, &datatype, &lsumWeight, &outputFile, &endmembersFile, &lsumGain, &lsumOffset))
    {
        return NULL;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;

    try
    {
        rsgis::cmds::executeConSum1LinearSpecUnmix(inputImage, imageFormat, type, lsumGain, lsumOffset, lsumWeight, outputFile, endmembersFile);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_NnConSum1LinearSpecUnmix(PyObject *self, PyObject *args)
{
    const char *inputImage, *imageFormat, *outputFile, *endmembersFile;
    float lsumGain = 1;
    float lsumOffset = 0;
    float lsumWeight = 1;
    int datatype;

    if(!PyArg_ParseTuple(args, "ssifss|ff:nnConSum1LinearSpecUnmix", &inputImage, &imageFormat, &datatype, &lsumWeight, &outputFile, &endmembersFile, &lsumGain, &lsumOffset))
    {
        return NULL;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;

    try
    {
        rsgis::cmds::executeNnConSum1LinearSpecUnmix(inputImage, imageFormat, type, lsumGain, lsumOffset, lsumWeight, outputFile, endmembersFile);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_AllBandsEqualTo(PyObject *self, PyObject *args) {
    const char *inputImage, *outputImage, *imageFormat;
    float imgValue, outputTrueVal, outputFalseVal;
    int datatype;

    if(!PyArg_ParseTuple(args, "sfffssi:allBandsEqualTo", &inputImage, &imgValue, &outputTrueVal, &outputFalseVal, &outputImage, &imageFormat, &datatype))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;

    try
    {
        rsgis::cmds::executeAllBandsEqualTo(inputImage, imgValue, outputTrueVal, outputFalseVal, outputImage, imageFormat, type);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_Histogram(PyObject *self, PyObject *args) {
    const char *inputImage, *outputFile, *imageMask;
    float imgValue;
    float binWidth, inMin, inMax;
    int calcInMinMax;
    unsigned int imgBand;

    if(!PyArg_ParseTuple(args, "sssIffiff:histogram", &inputImage, &imageMask, &outputFile, &imgBand, &imgValue, &binWidth, &calcInMinMax, &inMin, &inMax))
        return NULL;

    try {
        rsgis::cmds::executeHistogram(inputImage, imageMask, outputFile, imgBand, imgValue, binWidth, calcInMinMax, inMin, inMax);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_GetHistogram(PyObject *self, PyObject *args) {
    const char *inputImage;
    float binWidth, inMin, inMax;
    int calcInMinMax;
    unsigned int imgBand;
    
    if(!PyArg_ParseTuple(args, "sIfiff:getHistogram", &inputImage, &imgBand, &binWidth, &calcInMinMax, &inMin, &inMax))
        return NULL;
    
    PyObject *binsList = NULL;
    PyObject *outList = PyTuple_New(3);
    try
    {
        unsigned int nBins = 0;
        double inMinVal = inMin;
        double inMaxVal = inMax;
        unsigned int *bins = rsgis::cmds::executeGetHistogram(inputImage, imgBand, binWidth, &nBins, calcInMinMax, &inMinVal, &inMaxVal);
        
        Py_ssize_t listLen = nBins;
        
        
        binsList = PyTuple_New(listLen);
        if(binsList == NULL)
        {
            delete[] bins;
            throw rsgis::cmds::RSGISCmdException("Could not create a python list...");
        }
        
        for(unsigned int i = 0; i < nBins; ++i)
        {
            if(PyTuple_SetItem(binsList, i, Py_BuildValue("I", bins[i])) == -1)
            {
                throw rsgis::cmds::RSGISCmdException("Failed to add a value to the list...");
            }
        }
        delete[] bins;
        
        if(PyTuple_SetItem(outList, 0, binsList) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add a value to the list...");
        }
        if(PyTuple_SetItem(outList, 1, Py_BuildValue("d", inMinVal)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add a value to the list...");
        }
        if(PyTuple_SetItem(outList, 2, Py_BuildValue("d", inMaxVal)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add a value to the list...");
        }

    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    return outList;
}

static PyObject *ImageCalc_BandPercentile(PyObject *self, PyObject *args)
{
    const char *inputImage;
    float percentile;
    PyObject *noDataValueObj;

    if(!PyArg_ParseTuple(args, "sfO:bandPercentile", &inputImage, &percentile, &noDataValueObj))
    {
        return NULL;
    }
    
    bool haveNoDataValue = false;
    float noDataValue = 0.0;
    if(noDataValueObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(noDataValueObj) | RSGISPY_CHECK_INT(noDataValueObj))
        {
            noDataValue = RSGISPY_FLOAT_EXTRACT(noDataValueObj);
            haveNoDataValue = true;
        }
    }
    
    PyObject *outVals = NULL;
    try
    {
        std::vector<double> outPercentileVals = rsgis::cmds::executeBandPercentile(inputImage, percentile, noDataValue, haveNoDataValue);
        
        Py_ssize_t listLen = outPercentileVals.size();
        outVals = PyTuple_New(listLen);
        for(unsigned int i = 0; i < outPercentileVals.size(); ++i)
        {
            if(PyTuple_SetItem(outVals, i, Py_BuildValue("d", outPercentileVals.at(i))) == -1)
            {
                throw rsgis::cmds::RSGISCmdException("Failed to add \'percentile\' value to the list...");
            }
        }
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    return outVals;
}

static PyObject *ImageCalc_ImageDist2Geoms(PyObject *self, PyObject *args) {
    const char *inputImage, *outputImage, *inputVector, *imageFormat;

    if(!PyArg_ParseTuple(args, "ssss:imageDist2Geoms", &inputImage, &inputVector, &imageFormat, &outputImage))
        return NULL;

    try {
        rsgis::cmds::executeImageDist2Geoms(inputImage, inputVector, imageFormat, outputImage);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_CorrelationWindow(PyObject *self, PyObject *args) {
    const char *pszInputImage, *pszOutputImage, *pszGDALFormat;
    int datatype, windowSize, bandA, bandB;

    if(!PyArg_ParseTuple(args, "ssiiisi:correlationWindow", &pszInputImage, &pszOutputImage, &windowSize, &bandA, &bandB, &pszGDALFormat, &datatype))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;
    try 
    {
    rsgis::cmds::executeCorrelationWindow(pszInputImage, pszOutputImage, windowSize, bandA, bandB, pszGDALFormat, type);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_GetImageStatsInEnv(PyObject *self, PyObject *args) {
    const char *inputImage;
    unsigned int imgBand;
    double latMin, latMax, longMin, longMax;
    PyObject *noDataValueObj;
    
    if(!PyArg_ParseTuple(args, "sIOdddd:getImageStatsInEnv", &inputImage, &imgBand, &noDataValueObj, &latMin, &latMax, &longMin, &longMax))
        return NULL;
    
    bool noDataValueSpecified = false;
    float noDataValue = 0.0;
    
    if( ( noDataValueObj == NULL ) || ( noDataValueObj == Py_None ) || !RSGISPY_CHECK_FLOAT(noDataValueObj) )
    {
        noDataValueSpecified = false;
    }
    else
    {
        noDataValueSpecified = true;
        noDataValue = RSGISPY_FLOAT_EXTRACT(noDataValueObj);
    }
    
    PyObject *outValsList = PyTuple_New(5);
    try
    {
        
        rsgis::cmds::ImageStatsCmds *stats = new rsgis::cmds::ImageStatsCmds();
        stats->max = 0;
        stats->min = 0;
        stats->mean = 0;
        stats->stddev = 0;
        stats->sum = 0;
        
        rsgis::cmds::executeImageBandStatsEnv(std::string(inputImage), stats, imgBand, noDataValueSpecified, noDataValue, latMin, latMax, longMin, longMax);
        
        
        if(PyTuple_SetItem(outValsList, 0, Py_BuildValue("d", stats->min)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'min\' value to the list...");
        }
        if(PyTuple_SetItem(outValsList, 1, Py_BuildValue("d", stats->max)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'max\' value to the list...");
        }
        if(PyTuple_SetItem(outValsList, 2, Py_BuildValue("d", stats->mean)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'mean\' value to the list...");
        }
        if(PyTuple_SetItem(outValsList, 3, Py_BuildValue("d", stats->stddev)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'stddev\' value to the list...");
        }
        if(PyTuple_SetItem(outValsList, 4, Py_BuildValue("d", stats->sum)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'sum\' value to the list...");
        }
        
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    return outValsList;
}


static PyObject *ImageCalc_GetImageBandModeInEnv(PyObject *self, PyObject *args)
{
    const char *inputImage;
    unsigned int imgBand;
    double latMin, latMax, longMin, longMax;
    float binWidth;
    PyObject *noDataValueObj;
    
    if(!PyArg_ParseTuple(args, "sIfOdddd:getImageBandModeInEnv", &inputImage, &imgBand, &binWidth, &noDataValueObj, &latMin, &latMax, &longMin, &longMax))
        return NULL;
    
    bool noDataValueSpecified = false;
    float noDataValue = 0.0;
    
    if( ( noDataValueObj == NULL ) || ( noDataValueObj == Py_None ) || !RSGISPY_CHECK_FLOAT(noDataValueObj) )
    {
        noDataValueSpecified = false;
    }
    else
    {
        noDataValueSpecified = true;
        noDataValue = RSGISPY_FLOAT_EXTRACT(noDataValueObj);
    }
    
    PyObject *outVal = PyTuple_New(1);
    try
    {
        float modeVal = rsgis::cmds::executeImageBandModeEnv(std::string(inputImage), binWidth, imgBand, noDataValueSpecified, noDataValue, latMin, latMax, longMin, longMax);
        
        if(PyTuple_SetItem(outVal, 0, Py_BuildValue("f", modeVal)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'mode\' value to the list...");
        }
        
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    return outVal;
}

static PyObject *ImageCalc_Get2DImageHistogram(PyObject *self, PyObject *args)
{
    const char *inputImage1, *inputImage2, *outputImage, *gdalFormat;
    unsigned int img1Band, img2Band, numBins;
    int normOutput;
    double img1Min = 0.0;
    double img1Max = 0.0;
    double img2Min = 0.0;
    double img2Max = 0.0;
    double img1Scale = 1.0;
    double img2Scale = 1.0;
    double img1Off = 0.0;
    double img2Off = 0.0;
    
    if(!PyArg_ParseTuple(args, "ssssIIIddddddddi:get2DImageHistogram", &inputImage1, &inputImage2, &outputImage, &gdalFormat, &img1Band, &img2Band, &numBins, &img1Min, &img1Max, &img2Min, &img2Max, &img1Scale, &img2Scale, &img1Off, &img2Off, &normOutput))
    {
        return NULL;
    }
    
    PyObject *outVal = PyTuple_New(3);
    try
    {
        double binWidthImg1 = 0.0;
        double binWidthImg2 = 0.0;
        
        double rSq = rsgis::cmds::executeImageComparison2dHisto(std::string(inputImage1), std::string(inputImage2), std::string(outputImage), std::string(gdalFormat), img1Band, img2Band, numBins, &binWidthImg1, &binWidthImg2, img1Min, img1Max, img2Min, img2Max, img1Scale, img2Scale, img1Off, img2Off, ((bool)normOutput));
                
        if(PyTuple_SetItem(outVal, 0, Py_BuildValue("d", binWidthImg1)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'binWidthImg1\' value to the list...");
        }
        if(PyTuple_SetItem(outVal, 1, Py_BuildValue("d", binWidthImg2)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'binWidthImg2\' value to the list...");
        }
        if(PyTuple_SetItem(outVal, 2, Py_BuildValue("d", rSq)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'rSq\' value to the list...");
        }
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    return outVal;
}

static PyObject *ImageCalc_CalcMaskImgPxlValProb(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszMaskImage, *pszOutputImage, *pszGDALFormat;
    int maskImgVal;
    int useImgNoData = true;
    int rescaleProbs = true;
    PyObject *inImgBandIdxsPyObj;
    PyObject *histBinWidthsPyObj;
    bool calcHistBinWidth = true;
    
    if(!PyArg_ParseTuple(args, "sOsLss|Oii:calcMaskImgPxlValProb", &pszInputImage, &inImgBandIdxsPyObj, &pszMaskImage, &maskImgVal, &pszOutputImage, &pszGDALFormat, &histBinWidthsPyObj, &useImgNoData, &rescaleProbs))
    {
        return NULL;
    }
    
    std::vector<float> histBinWidths;
    if(histBinWidthsPyObj != NULL)
    {
        if( !PySequence_Check(histBinWidthsPyObj))
        {
            PyErr_SetString(GETSTATE(self)->error, "If provided histogram bin widths must be provided as a list.");
            return NULL;
        }
        
        Py_ssize_t nHistBins = PySequence_Size(histBinWidthsPyObj);
        histBinWidths.reserve(nHistBins);
        
        for( Py_ssize_t n = 0; n < nHistBins; n++ )
        {
            PyObject *o = PySequence_GetItem(histBinWidthsPyObj, n);
            histBinWidths.push_back(RSGISPY_FLOAT_EXTRACT(o));
        }
        calcHistBinWidth = false;
    }
    else
    {
        histBinWidths.clear();
        calcHistBinWidth = true;
    }
    
    std::vector<unsigned int> inImgBandIdxs;
    if( !PySequence_Check(inImgBandIdxsPyObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Input image bands must be provided as a list.");
        return NULL;
    }
    
    Py_ssize_t nBandIdxs = PySequence_Size(inImgBandIdxsPyObj);
    inImgBandIdxs.reserve(nBandIdxs);
    
    for( Py_ssize_t n = 0; n < nBandIdxs; n++ )
    {
        PyObject *o = PySequence_GetItem(inImgBandIdxsPyObj, n);
        inImgBandIdxs.push_back(RSGISPY_INT_EXTRACT(o));
    }
    
    if(!calcHistBinWidth)
    {
        if(inImgBandIdxs.size() != histBinWidths.size())
        {
            PyErr_SetString(GETSTATE(self)->error, "The number of bands specifed and the histogram bin widths must be the same.");
            return NULL;
        }
    }
    
    try
    {
        rsgis::cmds::executeCalcMaskImgPxlValProb(std::string(pszInputImage), inImgBandIdxs, std::string(pszMaskImage), maskImgVal, std::string(pszOutputImage), std::string(pszGDALFormat), histBinWidths, calcHistBinWidth, useImgNoData, rescaleProbs);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *ImageCalc_CalcPropTrueExp(PyObject *self, PyObject *args)
{
    const char *pszExpression;
    PyObject *pBandDefnObj;
    PyObject *pInValidImageObj;
    if( !PyArg_ParseTuple(args, "sO|O:calcPropTrueExp", &pszExpression, &pBandDefnObj, &pInValidImageObj))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "band defs argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    rsgis::cmds::VariableStruct *pRSGISStruct = new rsgis::cmds::VariableStruct[nBandDefns];
    
    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);
        
        PyObject *pBandName = PyObject_GetAttrString(o, "bandName");
        if( ( pBandName == NULL ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'bandName\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return NULL;
        }
        
        PyObject *pFileName = PyObject_GetAttrString(o, "fileName");
        if( ( pFileName == NULL ) || ( pFileName == Py_None ) || !RSGISPY_CHECK_STRING(pFileName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'fileName\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pFileName);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return NULL;
        }
        
        PyObject *pBandIndex = PyObject_GetAttrString(o, "bandIndex");
        if( ( pBandIndex == NULL ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find integer attribute \'bandIndex\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return NULL;
        }
        
        pRSGISStruct[n].name = RSGISPY_STRING_EXTRACT(pBandName);
        pRSGISStruct[n].image = RSGISPY_STRING_EXTRACT(pFileName);
        pRSGISStruct[n].bandNum = RSGISPY_INT_EXTRACT(pBandIndex);
        
        Py_DECREF(pBandName);
        Py_DECREF(pFileName);
        Py_DECREF(pBandIndex);
        Py_DECREF(o);
    }
    
    bool useValidImg = false;
    std::string inValidImage = "";
    if(RSGISPY_CHECK_STRING(pInValidImageObj))
    {
        useValidImg = true;
        inValidImage = RSGISPY_STRING_EXTRACT(pInValidImageObj);
    }
    
    float prop = 0.0;
    try
    {
        prop = rsgis::cmds::executeCalcPropTrueExp(pRSGISStruct, nBandDefns, std::string(pszExpression), inValidImage, useValidImg);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    delete[] pRSGISStruct;
    
    PyObject *outVal = Py_BuildValue("f", prop);
    return outVal;
}



static PyObject *ImageCalc_calcMultiImgBandStats(PyObject *self, PyObject *args)
{
    PyObject *inImagesObj;
    const char *outputImage;
    int sumStat;
    const char *gdalFormat;
    int datatype;
    float noDataVal = 0;
    int useNoDataVal = false;
    
    if(!PyArg_ParseTuple(args, "Osisi|fi:calcMultiImgBandStats", &inImagesObj, &outputImage, &sumStat, &gdalFormat, &datatype, &noDataVal, &useNoDataVal))
    {
        return NULL;
    }
    
    if( !PySequence_Check(inImagesObj) )
    {
        PyErr_SetString(GETSTATE(self)->error, "first three arguments must be sequences");
        return NULL;
    }
    
    Py_ssize_t nImages = PySequence_Size(inImagesObj);
    
    std::vector<std::string> inputImages;
    inputImages.reserve(nImages);

    for(int i = 0; i < nImages; ++i)
    {
        PyObject *inImageObj = PySequence_GetItem(inImagesObj, i);
        
        if(!RSGISPY_CHECK_STRING(inImageObj))
        {
            Py_DECREF(inImageObj);
            PyErr_SetString(GETSTATE(self)->error, "Input images must be strings");
            return NULL;
        }
        
        inputImages.push_back(RSGISPY_STRING_EXTRACT(inImageObj));
        
        Py_DECREF(inImageObj);
    }
    
    rsgis::cmds::RSGISCmdsSummariseStats summaryStats = rsgis::cmds::rsgiscmds_stat_none;
    if(sumStat == 1)
    {
        summaryStats = rsgis::cmds::rsgiscmds_stat_mode;
    }
    else if(sumStat == 2)
    {
        summaryStats = rsgis::cmds::rsgiscmds_stat_mean;
    }
    else if(sumStat == 3)
    {
        summaryStats = rsgis::cmds::rsgiscmds_stat_median;
    }
    else if(sumStat == 4)
    {
        summaryStats = rsgis::cmds::rsgiscmds_stat_min;
    }
    else if(sumStat == 5)
    {
        summaryStats = rsgis::cmds::rsgiscmds_stat_max;
    }
    else if(sumStat == 6)
    {
        summaryStats = rsgis::cmds::rsgiscmds_stat_stddev;
    } // Value 7 (count) not used here.
    else if(sumStat == 8)
    {
        summaryStats = rsgis::cmds::rsgiscmds_stat_range;
    }
    else if(sumStat == 9)
    {
        summaryStats = rsgis::cmds::rsgiscmds_stat_sum;
    }
    else
    {
        PyErr_SetString(GETSTATE(self)->error, "Do not recognise the summary statistic option.");
        return NULL;
    }
    
    try
    {
        rsgis::cmds::calcMultiImgBandsStats(inputImages, std::string(outputImage), summaryStats, std::string(gdalFormat), (rsgis::RSGISLibDataType)datatype, (bool)useNoDataVal, noDataVal);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *ImageCalc_CalcImageDifference(PyObject *self, PyObject *args)
{
    const char *inputImage1, *inputImage2, *outputImage, *gdalFormat;
    int datatype;
    
    if(!PyArg_ParseTuple(args, "ssssi:calcImageDifference", &inputImage1, &inputImage2, &outputImage, &gdalFormat, &datatype))
    {
        return NULL;
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;
        rsgis::cmds::calcImageDifference(std::string(inputImage1), std::string(inputImage2), std::string(outputImage), std::string(gdalFormat), type);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalc_GetImageBandMinMax(PyObject *self, PyObject *args)
{
    const char *inputImage;
    int imgBand;
    int useNoDataVal = false;
    float noDataVal = 0.0;
    
    if(!PyArg_ParseTuple(args, "si|if:getImageBandMinMax", &inputImage, &imgBand, &useNoDataVal, &noDataVal))
    {
        return NULL;
    }
    
    PyObject *outList = PyTuple_New(2);
    try
    {
        std::pair<double,double> outVals = rsgis::cmds::getImageBandMinMax(std::string(inputImage), imgBand, (bool)useNoDataVal, noDataVal);
        
        if(PyTuple_SetItem(outList, 0, Py_BuildValue("d", outVals.first)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add min value to output tuple...");
        }
        if(PyTuple_SetItem(outList, 1, Py_BuildValue("d", outVals.second)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add max value to output tuple...");
        }
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    return outList;
}

static PyObject *ImageCalc_CalcImageRescale(PyObject *self, PyObject *args)
{
    PyObject *pInputImgsObj;
    const char *outputImage, *gdalFormat;
    int datatype;
    float cNoDataVal, cOffset, cGain, nNoDataVal, nOffset, nGain = 0.0;
    
    if(!PyArg_ParseTuple(args, "Ossiffffff:calcImageRescale", &pInputImgsObj, &outputImage, &gdalFormat, &datatype, &cNoDataVal, &cOffset, &cGain, &nNoDataVal, &nOffset, &nGain))
    {
        return NULL;
    }
    
    std::vector<std::string> inputImgs;
    if(PySequence_Check(pInputImgsObj))
    {
        Py_ssize_t nInputImgs = PySequence_Size(pInputImgsObj);
        for( Py_ssize_t n = 0; n < nInputImgs; n++ )
        {
            PyObject *strObj = PySequence_GetItem(pInputImgsObj, n);
            if(RSGISPY_CHECK_STRING(strObj))
            {
                inputImgs.push_back(RSGISPY_STRING_EXTRACT(strObj));
            }
            else
            {
                PyErr_SetString(GETSTATE(self)->error, "Input images sequence must contain a list of strings");
                return NULL;
            }
        }
    }
    else
    {
        if(RSGISPY_CHECK_STRING(pInputImgsObj))
        {
            inputImgs.push_back(RSGISPY_STRING_EXTRACT(pInputImgsObj));
        }
        else
        {
            PyErr_SetString(GETSTATE(self)->error, "Input images parameter must be either a single string or a sequence of strings");
            return NULL;
        }
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;
        rsgis::cmds::executeRescaleImages(inputImgs, std::string(outputImage), std::string(gdalFormat), type, cNoDataVal, cOffset, cGain, nNoDataVal, nOffset, nGain);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalc_GetImgIdxForStat(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {"inimages", "outimage", "gdalformat", "nodata", "stat", NULL};
    PyObject *pInputImages;
    const char *pszOutputImage = "";
    const char *pszGDALFormat = "";
    float noDataVal = 0.0;
    int statType;

    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "Ossfi:getImgIdxForStat", kwlist, &pInputImages, &pszOutputImage, &pszGDALFormat, &noDataVal, &statType))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pInputImages))
    {
        PyErr_SetString(GETSTATE(self)->error, "Input images must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nImages = PySequence_Size(pInputImages);
    std::vector<std::string> inputImages;
    inputImages.reserve(nImages);
    for( Py_ssize_t n = 0; n < nImages; n++ )
    {
        PyObject *o = PySequence_GetItem(pInputImages, n);
        
        if(!RSGISPY_CHECK_STRING(o))
        {
            PyErr_SetString(GETSTATE(self)->error, "Input images must be strings");
            Py_DECREF(o);
            return NULL;
        }
        
        inputImages.push_back(RSGISPY_STRING_EXTRACT(o));
    }
    
    rsgis::cmds::RSGISCmdsSummariseStats summaryStats = rsgis::cmds::rsgiscmds_stat_none;
    if(statType == 3)
    {
        summaryStats = rsgis::cmds::rsgiscmds_stat_median;
    }
    else if(statType == 4)
    {
        summaryStats = rsgis::cmds::rsgiscmds_stat_min;
    }
    else if(statType == 5)
    {
        summaryStats = rsgis::cmds::rsgiscmds_stat_max;
    }
    else
    {
        PyErr_SetString(GETSTATE(self)->error, "Do not recognise the summary statistic option - only min, max and median are supported here.");
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeGetImgIdxForStat(inputImages, std::string(pszOutputImage), std::string(pszGDALFormat), noDataVal, summaryStats);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}



static PyObject *ImageCalc_GetImgSumStatsInPxl(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {"refimage", "statsimage", "outimage", "gdalformat", "datatype", "sumstats", "statsimageband", "usenodata", "iogridx", "iogridy", NULL};
    const char *pInputRefImage;
    const char *pInputStatsImage;
    const char *pszOutputImage = "";
    const char *pszGDALFormat = "";
    int useImgDataVal = true;
    unsigned int statsImgBand = 1;
    unsigned int xIOGrid = 16;
    unsigned int yIOGrid = 16;
    PyObject *sumStatsInLst;
    int datatype;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssiO|IiII:getImgSumStatsInPxl", kwlist, &pInputRefImage, &pInputStatsImage, &pszOutputImage, &pszGDALFormat, &datatype, &sumStatsInLst, &statsImgBand, &useImgDataVal, &xIOGrid, &yIOGrid))
    {
        return NULL;
    }
    
    if( !PySequence_Check(sumStatsInLst))
    {
        PyErr_SetString(GETSTATE(self)->error, "Summary stats must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nSumStats = PySequence_Size(sumStatsInLst);
    std::vector<rsgis::cmds::RSGISCmdsSummariseStats> cmdSumStats;
    cmdSumStats.reserve(nSumStats);
    int statType;
    for( Py_ssize_t n = 0; n < nSumStats; n++ )
    {
        PyObject *o = PySequence_GetItem(sumStatsInLst, n);
        
        if(!RSGISPY_CHECK_INT(o))
        {
            PyErr_SetString(GETSTATE(self)->error, "Input summary is expecting an int representation, use provided consts (e.g., rsgislib.SUMTYPE_MEAN)");
            Py_DECREF(o);
            return NULL;
        }
        
        statType = RSGISPY_INT_EXTRACT(o);
        
        if(statType == 4)
        {
            cmdSumStats.push_back(rsgis::cmds::rsgiscmds_stat_min);
        }
        else if(statType == 5)
        {
            cmdSumStats.push_back(rsgis::cmds::rsgiscmds_stat_max);
        }
        else if(statType == 2)
        {
            cmdSumStats.push_back(rsgis::cmds::rsgiscmds_stat_mean);
        }
        else if(statType == 3)
        {
            cmdSumStats.push_back(rsgis::cmds::rsgiscmds_stat_median);
        }
        else if(statType == 8)
        {
            cmdSumStats.push_back(rsgis::cmds::rsgiscmds_stat_range);
        }
        else if(statType == 6)
        {
            cmdSumStats.push_back(rsgis::cmds::rsgiscmds_stat_stddev);
        }
        else if(statType == 9)
        {
            cmdSumStats.push_back(rsgis::cmds::rsgiscmds_stat_sum);
        }
        else if(statType == 1)
        {
            cmdSumStats.push_back(rsgis::cmds::rsgiscmds_stat_mode);
        }
        else if(statType == 7)
        {
            cmdSumStats.push_back(rsgis::cmds::rsgiscmds_stat_count);
        }
        else
        {
            PyErr_SetString(GETSTATE(self)->error, "Do not recognise the summary statistic option.");
            return NULL;
        }
    }
    
    try
    {
        bool useNoData = (bool) useImgDataVal;
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;
        rsgis::cmds::executeGetWithinPxlImgStatSummaries(std::string(pInputRefImage), std::string(pInputStatsImage), statsImgBand, std::string(pszOutputImage), std::string(pszGDALFormat), type, useNoData, cmdSumStats, xIOGrid, yIOGrid);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}




static PyObject *ImageCalc_IdentifyMinPxlValueInWin(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {"inputimg", "outimage", "outrefimg", "bands", "winsize", "gdalformat", "nodataval", "usenodata", NULL};
    const char *pInputImage = "";
    const char *pszOutputRefImage = "";
    const char *pszOutputImage = "";
    const char *pszGDALFormat = "";
    unsigned int winSize = 3;
    PyObject *bandsLstObj;
    int useNoDataValue;
    float noDataValue;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssOIsfi:identifyMinPxlValueInWin", kwlist, &pInputImage, &pszOutputImage, &pszOutputRefImage, &bandsLstObj, &winSize, &pszGDALFormat, &noDataValue, &useNoDataValue))
    {
        return NULL;
    }
    
    if( !PySequence_Check(bandsLstObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Bands list must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nBands = PySequence_Size(bandsLstObj);
    std::vector<unsigned int> bandsVec;
    bandsVec.reserve(nBands);
    int bandVal = 0;
    for( Py_ssize_t n = 0; n < nBands; ++n)
    {
        PyObject *o = PySequence_GetItem(bandsLstObj, n);
        
        if(!RSGISPY_CHECK_INT(o))
        {
            PyErr_SetString(GETSTATE(self)->error, "Band value must be an integer.");
            Py_DECREF(o);
            return NULL;
        }
        
        bandVal = RSGISPY_INT_EXTRACT(o);
        if(bandVal < 1)
        {
            PyErr_SetString(GETSTATE(self)->error, "Band value must be an integer with a minimum value of 1 (i.e., band indexing starts at 1).");
            Py_DECREF(o);
            return NULL;
        }
        bandsVec.push_back(bandVal);
    }
    
    try
    {
        bool useNoData = (bool) useNoDataValue;
        rsgis::cmds::executeIdentifyMinPxlValueInWin(std::string(pInputImage), std::string(pszOutputImage), std::string(pszOutputRefImage), bandsVec, winSize, std::string(pszGDALFormat), noDataValue, useNoData);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalc_CalcImgMeanInMask(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {"inputimg", "inputImgMsk", "mskValue", "bands", "nodataval", "usenodata", NULL};
    const char *pInputImage = "";
    const char *pInputImageMsk = "";
    PyObject *bandsLstObj;
    int useNoDataValue;
    float noDataValue;
    int mskValue = 0.0;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssiOfi:calcImgMeanInMask", kwlist, &pInputImage, &pInputImageMsk, &mskValue, &bandsLstObj, &noDataValue, &useNoDataValue))
    {
        return NULL;
    }
    
    if( !PySequence_Check(bandsLstObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Bands list must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nBands = PySequence_Size(bandsLstObj);
    std::vector<unsigned int> bandsVec;
    bandsVec.reserve(nBands);
    int bandVal = 0;
    for( Py_ssize_t n = 0; n < nBands; ++n)
    {
        PyObject *o = PySequence_GetItem(bandsLstObj, n);
        
        if(!RSGISPY_CHECK_INT(o))
        {
            PyErr_SetString(GETSTATE(self)->error, "Band value must be an integer.");
            Py_DECREF(o);
            return NULL;
        }
        
        bandVal = RSGISPY_INT_EXTRACT(o);
        if(bandVal < 1)
        {
            PyErr_SetString(GETSTATE(self)->error, "Band value must be an integer with a minimum value of 1 (i.e., band indexing starts at 1).");
            Py_DECREF(o);
            return NULL;
        }
        bandsVec.push_back(bandVal);
    }
    
    float meanVal = 0.0;
    try
    {
        bool useNoData = (bool) useNoDataValue;
        meanVal = rsgis::cmds::executeCalcImgMeanInMask(std::string(pInputImage), std::string(pInputImageMsk), mskValue, bandsVec, noDataValue, useNoData);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    PyObject *outVal = Py_BuildValue("f", meanVal);
    return outVal;
}

// Our list of functions in this module
static PyMethodDef ImageCalcMethods[] = {
    {"bandMath", (PyCFunction)ImageCalc_BandMath, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.bandMath(outputimg, exp, gdalformat, datatype, banddefseq, expbandname, outputexists)\n"
"Performs band math calculation.\n"
"The syntax for the expression is from the muparser library ('http://muparser.beltoforion.de <http://muparser.beltoforion.de>`): `see here <http://beltoforion.de/article.php?a=muparser&hl=en&p=features&s=idPageTop>`\n."
"\n"
"Where:\n"
"\n"
"* outputimg is a string containing the name of the output file\n"
"* exp is a string containing the expression to run over the images, uses muparser syntax.\n"
"* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"* banddefseq is a sequence of rsgislib.imagecalc.BandDefn objects that define the inputs\n"
"* expbandname is an optional bool specifying whether the band name should be the expression (Default = False).\n"
"* outputexists is an optional bool specifying whether the output image already exists and it should be editted rather than overwritten (Default=False)."
"\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imagecalc\n"
"   from rsgislib.imagecalc import BandDefn\n"
"   outputImage = './TestOutputs/PSU142_b1mb2.kea'\n"
"   gdalformat = 'KEA'\n"
"   datatype = rsgislib.TYPE_32FLOAT\n"
"   expression = 'b1*b2'\n"
"   bandDefns = []\n"
"   bandDefns.append(BandDefn('b1', inFileName, 1))\n"
"   bandDefns.append(BandDefn('b2', inFileName, 2))\n"
"   imagecalc.bandMath(outputImage, expression, gdalformat, datatype, bandDefns)\n"
"\n"
"   ################## If Statement Example #########################\n"
"   import rsgislib\n"
"   from rsgislib import imagecalc\n"
"   bandDefns = []\n"
"   bandDefns.append(BandDefn('b1', inFileName, 1))\n"
"   bandDefns.append(BandDefn('b2', inFileName, 2))\n"
"   bandDefns.append(BandDefn('b3', inFileName, 3))\n"
"   imagecalc.bandMath('out.kea', ‘(b1==1) || (b2==1) || (b3==1)?1:0', 'KEA', rsgislib.TYPE_8UINT, bandDefns)\n"
"\n"},

{"imageMath", (PyCFunction)ImageCalc_ImageMath, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.imageMath(inputimg, outputimg, exp, gdalformat, datatype, expbandname, outputexists)\n"
"Performs image math calculations. Produces an output image file with the same number of bands as the input image.\n"
"This function applies the same calculation to each image band (i.e., b1 is the only variable).\n"
"The syntax for the expression is from the muparser library ('http://muparser.beltoforion.de <http://muparser.beltoforion.de>`): `see here <http://beltoforion.de/article.php?a=muparser&hl=en&p=features&s=idPageTop>`\n."
"\n"
"Where:\n"
"\n"
"* inputimg is a string containing the name of the input file\n"
"* outputimg is a string containing the name of the output file\n"
"* exp is a string containing the expression to run over the images, uses myparser syntax.\n"
"* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"* expbandname is an optional bool specifying whether the band name should be the expression (Default = False).\n"
"* outputexists is an optional bool specifying whether the output image already exists and it should be editted rather than overwritten (Default=False)."
"\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imagecalc\n"
"   outputImage = path + 'TestOutputs/PSU142_multi1000.kea'\n"
"   gdalformat = 'KEA'\n"
"   datatype = rsgislib.TYPE_32UINT\n"
"   expression = 'b1*1000'\n"
"   imagecalc.imageMath(inFileName, outputImage, expression, gdalformat, datatype)\n"
"\n"},

{"imageBandMath", (PyCFunction)ImageCalc_ImageBandMath, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.imageBandMath(inputimg, outputimg, exp, gdalformat, datatype, expbandname, outputexists)\n"
"Performs image band math calculations. Produces a single output file with a single image band.\n"
"The image bands can be referred to individually using b1, b2 ... bn. where n is the number of image bands, starting at 1.\n"
"The syntax for the expression is from the muparser library ('http://muparser.beltoforion.de <http://muparser.beltoforion.de>`): `see here <http://beltoforion.de/article.php?a=muparser&hl=en&p=features&s=idPageTop>`\n."
"\n"
"Where:\n"
"\n"
"* inputimg is a string containing the name of the input file\n"
"* outputimg is a string containing the name of the output file\n"
"* exp is a string containing the expression to run over the images, uses myparser syntax.\n"
"* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"* expbandname is an optional bool specifying whether the band name should be the expression (Default = False).\n"
"* outputexists is an optional bool specifying whether the output image already exists and it should be editted rather than overwritten (Default=False)."
"\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imagecalc\n"
"   outputImage = path + 'TestOutputs/PSU142_vismean.kea'\n"
"   gdalformat = 'KEA'\n"
"   datatype = rsgislib.TYPE_32UINT\n"
"   expression = '(b1+b2+b3+b4)/4'\n"
"   imagecalc.imageBandMath(inFileName, outputImage, expression, gdalformat, datatype)\n"
"\n"
"   ################## If Statement Example #########################\n"
"   import rsgislib\n"
"   from rsgislib import imagecalc\n"
"   imagecalc.imageBandMath('in.kea', 'out.kea', '(b1==1) || (b2==1) || (b3==1)?1:0', 'KEA', rsgislib.TYPE_8UINT)\n"
"\n"},

{"kMeansClustering", ImageCalc_KMeansClustering, METH_VARARGS,
"rsgislib.imagecalc.kMeansClustering(inputImage, outputMatrix, numClusters, maxIterations, subSample, ignoreZeros, degreeOfChange, initMethod)\n"
"Performs K Means Clustering and saves cluster centres to a text file.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string providing the input image\n"
"* outputMatrix is a string providing the output matrix (text file) to save the cluster centres to.\n"
"* numClusters is the number of clusters to use.\n"
"* maxIterations is the maximum number of itterations.\n"
"* subSample is an int specifying what fraction of the total pixels should be considered (e.g., 100 = 1/100 pixels).\n"
"* ignoreZeros is a bool specifying if zeros in the image should be treated as no data.\n"
"* degreeofChange is a float providing the minimum change between itterations before terminating.\n"
"* initMethod the method for initialising the clusters and is one of INITCLUSTER_* values\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imagecalc\n"
"   inputImage = path + 'Rasters/injune_p142_casi_sub_right_utm.kea'\n"
"   output = path + 'TestOutputs/kmeanscentres'\n"
"   numClust = 10\n"
"   maxIter = 200\n"
"   degChange = 0.0025\n"
"   subSample = 1\n"
"   ignoreZeros = True\n"
"   imagecalc.kMeansClustering(inputImage, output, numClust, maxIter, subSample, ignoreZeros, degChange, rsgislib.INITCLUSTER_DIAGONAL_FULL_ATTACH)\n"
"\n"},

{"isoDataClustering", ImageCalc_ISODataClustering, METH_VARARGS,
"rsgislib.imagecalc.isoDataClustering(inputImage, outputMatrix, numClusters, maxIterations, subSample, ignoreZeros, degreeOfChange, initMethod, minDistBetweenClusters, minNumFeatures, maxStdDev, minNumClusters, startIteration, endIteration)\n"
"Performs ISO Data Clustering and saves cluster centres to a text file.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string providing the input image\n"
"* outputMatrix is a string providing the output matrix (text file) to save the cluster centres to.\n"
"* numClusters is the number of clusters to start with.\n"
"* maxIterations is the maximum number of itterations.\n"
"* subSample is an int specifying what fraction of the total pixels should be considered (e.g., 100 = 1/100 pixels).\n"
"* ignoreZeros is a bool specifying if zeros in the image should be treated as no data.\n"
"* initMethod the method for initialising the clusters and is one of INITCLUSTER_* values\n"
"* minDistBetweenClusters is a float\n"
"* minNumFeatures is an int\n"
"* maxStdDev is a float\n"
"* minNumClusters is an int\n"
"* startIteration is an int\n"
"* endIteration is an int\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imagecalc\n"
"   inputImage = path + 'Rasters/injune_p142_casi_sub_right_utm.kea'\n"
"   output = './TestOutputs/isocentres'\n"
"   imagecalc.isoDataClustering(inputImage, output, 10, 200, 1, True, 0.0025, rsgislib.INITCLUSTER_DIAGONAL_FULL_ATTACH, 2, 5, 5, 5, 8, 50)\n"
"\n"},

{"mahalanobisDistFilter", ImageCalc_MahalanobisDistFilter, METH_VARARGS,
"rsgislib.imagecalc.mahalanobisDistFilter(inputImage, outputImage, windowSize, gdalFormat, gdalDataType)\n"
"Performs mahalanobis distance window filter.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input file\n"
"* outputImage is a string containing the name of the output file\n"
"* windowSize is an int defining the size of the window to be used\n"
"* gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* gdalDataType is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"
},

{"mahalanobisDist2ImgFilter", ImageCalc_MahalanobisDist2ImgFilter, METH_VARARGS,
"rsgislib.imagecalc.mahalanobisDist2ImgFilter(inputImage, outputImage, windowSize, gdalFormat, gdalDataType)\n"
"Performs mahalanobis distance image to window filter.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input file\n"
"* outputImage is a string containing the name of the output file\n"
"* windowSize is an int defining the size of the window to be used\n"
"* gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* gdalDataType is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"
},

{"imageCalcDistance", ImageCalc_ImageCalcDistance, METH_VARARGS,
"rsgislib.imagecalc.imageCalcDistance(inputImage, outputImage, gdalFormat)\n"
"Performs image calculate distance command.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input file\n"
"* outputImage is a string containing the name of the output file\n"
"* gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"\n"
},

{"imagePixelColumnSummary", ImageCalc_ImagePixelColumnSummary, METH_VARARGS,
"rsgislib.imagecalc.imagePixelColumnSummary(inputImage, outputImage, summaryStats, gdalFormat, gdalDataType, noDataValue, useNoDataValue)\n"
"Calculates summary statistics for a column of pixels.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input file\n"
"* outputImage is a string containing the name of the output file\n"
"* summaryStats is an rsgislib.imagecalc.StatsSummary object that has attributes matching rsgis.cmds.RSGISCmdStatsSummary\n"
"   *  calcMin: boolean defining if the min value should be calculated\n"
"   *  calcMax: boolean defining if the max value should be calculated\n"
"   *  calcSum: boolean defining if the sum value should be calculated\n"
"   *  calcMean: boolean defining if the mean value should be calculated\n"
"   *  calcStdDev: boolean defining if the standard deviation should be calculated\n"
"   *  calcMedian: boolean defining if the median value should be calculated\n"
"   *  calcMode: boolean defining if the mode value should be calculated; warning can be slow.\n"
"   *  min: float defining the min value to use\n"
"   *  max: float defining the max value to use\n"
"   *  mean: float defining the mean value to use\n"
"   *  sum: float defining the sum value to use\n"
"   *  stdDev: float defining the standard deviation value to use\n"
"   *  median: float defining the median value to use\n"
"   *  mode: float defining the mode value to use\n"
"* gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* gdalDataType is an int containing one of the values from rsgislib.TYPE_*\n"
"* noDataValue is a float specifying what value is used to signify no data\n"
"* useNoDataValue is a boolean specifying whether the noDataValue should be used\n"
"\n"
},

{"imagePixelLinearFit", ImageCalc_ImagePixelLinearFit, METH_VARARGS,
"rsgislib.imagecalc.imagePixelLinearFit(inputImage, outputImage, gdalFormat, bandValues, noDataValue, useNoDataValue)\n"
"Performs a linear regression on each column of pixels.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input file\n"
"* outputImage is a string containing the name of the output file\n"
"* gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* bandValues is text file containing the value of each band (e.g. wavelength, day of year) with a separate line for each band\n"
"* noDataValue is a float specifying what value is used to signify no data\n"
"* useNoDataValue is a boolean specifying whether the noDataValue should be used\n"
"\n"
"Example::\n"
"\n"
"   image = 'injune_p142_casi_sub_utm.kea'\n"
"   output = 'injune_p142_casi_sub_utm_linear_fit.kea'\n"
"   gdalformat = 'KEA'\n"
"   bandValues = [446,530,549,569,598,633,680,696,714,732,741,752,800,838]\n"
"   \n"   
"   bandValuesFile =  'injune_p142_casi_wavelengths.txt'\n"
"   \n"  
"   with open(bandValuesFile,'w') as f:\n"
"       for bandVal in bandValues:\n"
"           f.write(str(bandVal) + '\\n')\n"
"   \n"
"   imagecalc.imagePixelLinearFit(image, output, gdalformat, bandValuesFile, 0, True)\n"
"\n"
},

{"normalisation", ImageCalc_Normalisation, METH_VARARGS,
"rsgislib.imagecalc.normalisation(inputImages, outputImages, calcInMinMax, inMin, inMax, outMin, outMax)\n"
"Performs image normalisation\n"
"\n"
"Where:\n"
"\n"
"* inputImages is a sequence of strings containing the names of the input files\n"
"* outputImages is a sequence of strings containing the names of the output files\n"
"* calcInMinMax is a boolean specifying whether to calculate inMin and inMax values TODO: Check'\n"
"* inMin is a float specifying the TODO\n"
"* inMax is a float specifying the TODO\n"
"* outMin is a float specifying the TODO\n"
"* outMax is a float specifying the TODO\n"
"\n"
},

{"correlation", (PyCFunction)ImageCalc_Correlation, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.correlation(imageA, imageB, outmatrixfile=None)\n"
"Calculates the correlation between two images\n"
"\n"
"Where:\n"
"\n"
"* inputImageA is a string containing the name of the first input image file\n"
"* inputImageB is a string containing the name of the second input image file\n"
"* outputMatrix is a string containing the basename of the output matrix (optional)\n"
"\n"
"Returns:\n"
"\n"
"* Correlation matrix (tuple of tuples)\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import imagecalc\n"
"   inImageA = 'imageA.kea'\n"
"   inImageB = 'imageB.kea'\n"
"   outMatrixBase = 'imageABCorrelation'\n"
"   outMatrix = imagecalc.correlation(inImage, inImage, outMatrixBase)\n"
"\n"},

{"covariance", ImageCalc_Covariance, METH_VARARGS,
"rsgislib.imagecalc.covariance(inputImageA, inputImageB, inputMatrixA, inputMatrixB, shouldCalcMean, outputMatrix)\n"
"Calculates the covariance between two images\n"
"\n"
"Where:\n"
"\n"
"* inputImageA is a string containing the name of the first input image file\n"
"* inputImageB is a string containing the name of the second input image file\n"
"* inputMatrixA is a string containing the name of the first input matrix file\n"
"* inputMatrixB is a string containing the name of the second input matrix file\n"
"* shouldCalcMean is a boolean defining whether the mean should be calculated TODO: check\n"
"* outputMatrix is a string containing the name of the output matrix\n"
"\n"
},

{"meanVector", ImageCalc_MeanVector, METH_VARARGS,
"rsgislib.imagecalc.meanVector(inputImage, outputMatrix)\n"
"Calculates the mean vector of an image\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* outputMatrix is a string containing the name of the output matrix\n"
"\n"
},

{"pca", ImageCalc_PCA, METH_VARARGS,
"rsgislib.imagecalc.pca(inputImage, eigenVectors, outputImage, numComponents, gdalFormat, dataType)\n"
"Performs a principal components analysis of an image using a defined set of eigenvectors.\n"
"The eigenvectors can be calculated using the rsgislib.imagecalc.getPCAEigenVector function.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* eigenVectors is a string containing the name of the file of eigen vectors for the PCA\n"
"* outputImage is a string containing the name of the output image file\n"
"* numComponents is an int containing number of components to use for PCA\n"
"* gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* dataType is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib.imageutils\n"
"   import rsgislib.imagecalc\n"
"   inputImg = 'Input.kea'\n"
"   eigenVecFile = 'EigenVec.mtxt'\n"
"   outputImg = './Output.kea'\n"
"   eigenVec, varExplain = rsgislib.imagecalc.getPCAEigenVector(inputImg, 1000, None, eigenVecFile)\n"
"   rsgislib.imagecalc.pca(inputImg, eigenVecFile, outputImg, varExplain.shape[0], 'KEA', rsgislib.TYPE_32FLOAT)\n"
"   rsgislib.imageutils.popImageStats(outputImg, usenodataval, nodataval, True)\n"
"\n"
"\n"
},

{"standardise", ImageCalc_Standardise, METH_VARARGS,
"rsgislib.imagecalc.standardise(meanVector, inputImage, outputImage)\n"
"Generates a standardised image using the mean vector provided\n"
"\n"
"Where:\n"
"\n"
"* meanVector is a string containing the name of the file containing the mean vector TODO: check\n"
"* inputImage is a string containing the name of the input image file\n"
"* outputImage is a string containing the name of the output image file\n"
"\n"
},

{"replaceValuesLessThan", ImageCalc_ReplaceValuesLessThan, METH_VARARGS,
"rsgislib.imagecalc.replaceValuesLessThan(inputImage, outputImage, threshold, value)\n"
"Replaces values in an image that are less than the provided, according to the provided threshold\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* outputImage is a string containing the name of the output image file\n"
"* threshold is a float containing the threshold to use\n"
"* value is a float containing the value below which replacement will occur\n"
"\n"
},

{"unitArea", ImageCalc_UnitArea, METH_VARARGS,
"rsgislib.imagecalc.unitArea(inputImage, outputImage, inputMatrixFile)\n"
"Converts the image spectra to unit area\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* outputImage is a string containing the name of the output image file\n"
"* inputMatrixFile is a string containing the name of the input matrix file TODO: check\n"
"\n"
},

{"movementSpeed", ImageCalc_MovementSpeed, METH_VARARGS,
"rsgislib.imagecalc.movementSpeed(inputImages, imageBands, imageTimes, upper, lower, outputImage)\n"
"Calculates the speed of movement in images (mean, min and max)\n"
"\n"
"Where:\n"
"\n"
"* inputImages is a python sequence of strings of the input image files\n"
"* imageBands is a python sequence of integers defining the band of each image to use\n"
"* imageTimes is a python sequence of floats defining the time corresponding to each image\n"
"* upper is a float TODO: expand\n"
"* lower is a float TODO: expand\n"
"* outputImage is a string defining the output image file name\n"
"\n"
},

{"countValsInCols", ImageCalc_CountValsInCols, METH_VARARGS,
"rsgislib.imagecalc.countValsInCols(inputImage, upper, lower, outputImage)\n"
"Counts the number of values within a given range for each column\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* upper is a float containing the upper limit of values to count\n"
"* lower is a float containing the lower limit of values to count\n"
"* outputImage is a string containing the name of the output image file\n"
"\n"
},

{"calculateRMSE", ImageCalc_CalculateRMSE, METH_VARARGS,
"rsgislib.imagecalc.calculateRMSE(inputImageA, inputBandA, inputImageB, inputBandB)\n"
"Calculates the root mean squared error between two images\n"
"\n"
"Where:\n"
"\n"
"* inputImageA is a string containing the name of the first input image file\n"
"* inputBandA is an integer defining which band should be processed from inputImageA\n"
"* inputImageB is a string containing the name of the second input image file\n"
"* inputBandB is an integer defining which band should be processed from inputImageB\n"
"\n"
},

{"dist2Geoms", ImageCalc_Dist2Geoms, METH_VARARGS,
"rsgislib.imagecalc.dist2Geoms(inputVector, imageResolution, outputImage)\n"
"Calculates the distance to the nearest geometry for every pixel in an image\n"
"\n"
"Where:\n"
"\n"
"* inputVector is a string defining the geometry TODO: Check this and find out about input image\n"
"* imageResolution is a float defining the resolution of which to process the image\n"
"* outputImage is a string containing the name of the file for the output image\n"
"\n"
},

{"imageBandStats", ImageCalc_ImageBandStats, METH_VARARGS,
"rsgislib.imagecalc.imageBandStats(inputImage, outputFile, ignoreZeros)\n"
"Calculates statistics for individuals bands of an image\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* outputFile is a string containing the name of the output file\n"
"* ignoreZeros is a boolean defining whether zeros are ignored in the statistic calculation\n"
"\n"
},

{"imageStats", ImageCalc_ImageStats, METH_VARARGS,
"rsgislib.imagecalc.imageStats(inputImage, outputFile, ignoreZeros)\n"
"Calculates statistics for an image across all bands\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* outputFile is a string containing the name of the output file\n"
"* ignoreZeros is a boolean defining whether zeros are ignored in the statistic calculation\n"
"\n"
},

{"unconLinearSpecUnmix", ImageCalc_UnconLinearSpecUnmix, METH_VARARGS,
"rsgislib.imagecalc.unconLinearSpecUnmix(inputImage, gdalformat, datatype, outputFile, endmembersFile, lsumGain, lsumOffset)\n"
"Performs unconstrained linear spectral unmixing of the input image for a set of endmembers.\n"
"Endmember polygons are extracted using rsgislib.zonalstats.extractAvgEndMembers() where each polygon \n"
"defines an endmember.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"* outputFile is a string containing the name of the output file\n"
"* endmembersFile is a string containing the names of the file containing the end members\n"
"* lsumGain is a float specifying a gain which can be applied to the output pixel values (outvalue = offset + (gain * value)). Optional, default = 1.\n"
"* lsumOffset is a float specifying an offset which can be applied to the output pixel values (outvalue = offset + (gain * value)). Optional, default = 0.\n"
"\n"
"Example::\n"
"\n"
"    import rsgislib.zonalstats\n"
"    import rsgislib.imagecalc\n"
"    import rsgislib\n"
"\n"
"    imageLSImage = \"./LS8_20130519_lat52lon42_r24p204_rad_srefstdmdl.kea\"\n"
"    unmixedImage = \"./LS8_20130519_lat52lon42_r24p204_rad_srefstdmdl_unmix.kea\"\n"
"    roiSHP = \"./ROIs.shp\"\n"
"    endmembersFile = \"./endmembers\"\n"
"\n"
"    rsgislib.zonalstats.extractAvgEndMembers(imageLSImage, roiSHP, endmembersFile)\n"
"\n"
"    lsumGain = 1.0\n"
"    lsumOffset = 0.0\n"
"\n"
"    endmembersFile = \"./endmembers.mtxt\"\n"
"    rsgislib.imagecalc.unconLinearSpecUnmix(imageLSImage, \"KEA\", rsgislib.TYPE_32FLOAT, unmixedImage, endmembersFile, lsumGain, lsumOffset)\n"
"\n"
"\n"
},

{"exhconLinearSpecUnmix", ImageCalc_ExhconLinearSpecUnmix, METH_VARARGS,
"rsgislib.imagecalc.exhconLinearSpecUnmix(inputImage, gdalformat, datatype, outputFile, endmembersFile, stepResolution, lsumGain, lsumOffset)\n"
"Performs an exhaustive constrained linear spectral unmixing of the input image for a set of endmembers.\n"
"\n**Warning. This methods is slow (!!) to execute**\n\n"
"Endmember polygons are extracted using rsgislib.zonalstats.extractAvgEndMembers() where each polygon \n"
"defines an endmember.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* gdalformat is a string containing the GDAL format for the output file - eg KEA\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"* outputFile is a string containing the name of the output file\n"
"* endmembersFile is a string containing the names of the file containing the end members\n"
"* stepResolution is a float specifying the gap between steps in the search space. Value needs to be between 0 and 1. (i.e., 0.05)\n"
"* lsumGain is a float specifying a gain which can be applied to the output pixel values (outvalue = offset + (gain * value)). Optional, default = 1.\n"
"* lsumOffset is a float specifying an offset which can be applied to the output pixel values (outvalue = offset + (gain * value)). Optional, default = 0.\n"
"\n"
"Example::\n"
"\n"
"    import rsgislib.zonalstats\n"
"    import rsgislib.imagecalc\n"
"    import rsgislib\n"
"\n"
"    imageLSImage = \"./LS8_20130519_lat52lon42_r24p204_rad_srefstdmdl.kea\"\n"
"    unmixedImage = \"./LS8_20130519_lat52lon42_r24p204_rad_srefstdmdl_unmix.kea\"\n"
"    roiSHP = \"./ROIs.shp\"\n"
"    endmembersFile = \"./endmembers\"\n"
"\n"
"    rsgislib.zonalstats.extractAvgEndMembers(imageLSImage, roiSHP, endmembersFile)\n"
"\n"
"    lsumGain = 1.0\n"
"    lsumOffset = 0.0\n"
"\n"
"    endmembersFile = \"./endmembers.mtxt\"\n"
"    stepResolution = 0.1\n"
"    rsgislib.imagecalc.exhconLinearSpecUnmix(imageLSImage, \"KEA\", rsgislib.TYPE_32FLOAT, unmixedImage, endmembersFile, stepResolution, lsumGain, lsumOffset)\n"
"\n"
"\n"
},

{"conSum1LinearSpecUnmix", ImageCalc_ConSum1LinearSpecUnmix, METH_VARARGS,
"rsgislib.imagecalc.conSum1LinearSpecUnmix(inputImage, gdalformat, datatype, lsumWeight, outputFile, endmembersFile, lsumGain, lsumOffset)\n"
"Performs a partially constrained linear spectral unmixing of the input image for a set of endmembers where the sum of the unmixing will be approximately 1\n"
"Endmember polygons are extracted using rsgislib.zonalstats.extractAvgEndMembers() where each polygon \n"
"defines an endmember.\n\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"* lsumWeight is a float specifying a weight which is added to the Least-Squares matrix to ensure summation to 1.\n"
"* outputFile is a string containing the name of the output file\n"
"* endmembersFile is a string containing the names of the file containing the end members\n"
"* lsumGain is a float specifying a gain which can be applied to the output pixel values (outvalue = offset + (gain * value)). Optional, default = 1.\n"
"* lsumOffset is a float specifying an offset which can be applied to the output pixel values (outvalue = offset + (gain * value)). Optional, default = 0.\n"
"\n"
"Example::\n"
"\n"
"    import rsgislib.zonalstats\n"
"    import rsgislib.imagecalc\n"
"    import rsgislib\n"
"\n"
"    imageLSImage = \"./LS8_20130519_lat52lon42_r24p204_rad_srefstdmdl.kea\"\n"
"    unmixedImage = \"./LS8_20130519_lat52lon42_r24p204_rad_srefstdmdl_unmix.kea\"\n"
"    roiSHP = \"./ROIs.shp\"\n"
"    endmembersFile = \"./endmembers\"\n"
"\n"
"    rsgislib.zonalstats.extractAvgEndMembers(imageLSImage, roiSHP, endmembersFile)\n"
"\n"
"    lsumGain = 1.0\n"
"    lsumOffset = 0.0\n"
"    lsumWeight = 40\n"
"\n"
"    endmembersFile = \"./endmembers.mtxt\"\n"
"    rsgislib.imagecalc.conSum1LinearSpecUnmix(imageLSImage, \"KEA\", rsgislib.TYPE_32FLOAT, lsumWeight, unmixedImage, endmembersFile, lsumGain, lsumOffset)\n"
"\n"
"\n"
},

{"nnConSum1LinearSpecUnmix", ImageCalc_NnConSum1LinearSpecUnmix, METH_VARARGS,
"rsgislib.imagecalc.nnConSum1LinearSpecUnmix(inputImage, gdalformat, datatype, lsumWeight, outputFile, endmembersFile, lsumGain, lsumOffset)\n"
"Performs a constrained linear spectral unmixing of the input image for a set of endmembers where the sum of the unmixing will be approximately 1 and non-negative.\n"
"Endmember polygons are extracted using rsgislib.zonalstats.extractAvgEndMembers() where each polygon \n"
"defines an endmember.\n\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"* lsumWeight is a float specifying a weight which is added to the Least-Squares matrix to ensure summation to 1.\n"
"* outputFile is a string containing the name of the output file\n"
"* endmembersFile is a string containing the names of the file containing the end members\n"
"* lsumGain is a float specifying a gain which can be applied to the output pixel values (outvalue = offset + (gain * value)). Optional, default = 1.\n"
"* lsumOffset is a float specifying an offset which can be applied to the output pixel values (outvalue = offset + (gain * value)). Optional, default = 0.\n"
"\n"
},

{"allBandsEqualTo", ImageCalc_AllBandsEqualTo, METH_VARARGS,
"rsgislib.imagecalc.allBandsEqualTo(inputImage, imgValue, outputTrueVal, outputFalseVal, outputImage, gdalformat, datatype)\n"
"Tests whether all bands are equal to the same value\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* imgValue is a float specifying the value against which others are tested for equality TODO: Check this and below\n"
"* ouputTrueVal is a float specifying the value in the output image representing true \n"
"* outputFalseVal is a float specifying the value in the output image representing false \n"
"* outputImage is a string containing the name of the output image file\n"
"* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"\n"
},

{"histogram", ImageCalc_Histogram, METH_VARARGS,
"rsgislib.imagecalc.histogram(inputImage, imageMask, outputFile, imgBand, imgValue, binWidth, calcInMinMax, inMin, inMax)\n"
"Generates a histogram for the region of the mask selected\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* imageMask is a string containing the name of the image mask file\n"
"* outputFile is a string containing the name of the file for histogram output\n"
"* imgValue is a float\n"
"* binWidth is a float specifying the width of the histogram bins\n"
"* calcInMinMax is a boolean specifying whether inMin and inMax should be calculated\n"
"* inMin is a float for the minimum image value to be included in the histogram\n"
"* inMax is a floatf or the maximum image value to be included in the histogram\n"
"\n"
},
    
{"getHistogram", ImageCalc_GetHistogram, METH_VARARGS,
"rsgislib.imagecalc.getHistogram(inputImage, imgBand, binWidth, calcInMinMax, inMin, inMax)\n"
"Generates and returns a histogram for the image.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* imgBand is an unsigned int specifying the image band starting from 1.\n"
"* binWidth is a float specifying the width of the histogram bins\n"
"* calcInMinMax is a boolean specifying whether inMin and inMax should be calculated\n"
"* inMin is a float for the minimum image value to be included in the histogram\n"
"* inMax is a floatf or the maximum image value to be included in the histogram\n"
"\n"
"returns:\n"
"\n"
"* returns a list"
"\n"
},

{"bandPercentile", ImageCalc_BandPercentile, METH_VARARGS,
"rsgislib.imagecalc.bandPercentile(inputImage, percentile, noDataValue)\n"
"Calculates image band percentiles for the input image and results a list of values\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* percentile is a float between 0 -- 1 specifying the percentile to be calculated.\n"
"* noDataValue is a float specifying the value used to represent no data (used None when no value is to be specified).\n"
"\n"
},

{"imageDist2Geoms", ImageCalc_ImageDist2Geoms, METH_VARARGS,
"rsgislib.imagecalc.imageDist2Geoms(inputImage, inputVector, gdalformat, outputImage)\n"
"Calculates the distance to the nearest geometry for every pixel in an image\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* inputVector is a string containing the name of the input vector file containing the geometry TODO: check this\n"
"* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* outputImage is a string containing the name of the output image file\n"
"\n"
},

{"correlationWindow", ImageCalc_CorrelationWindow, METH_VARARGS,
"rsgislib.imagecalc.correlationWindow(inputImage, outputImage, windowSize, bandA, bandB, gdalformat, datatype)\n"
"Tests whether all bands are equal to the same value\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* outputImage is a string containing the name of the output image file\n"
"* windowSize is an int providing the size of the window to calculate the correlation over\n"
"* bandA is an int providing the first band to use.\n"
"* bandB is an int providing the second band to use.\n"
"* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"\n"
"Example::\n"
"\n"
"   image = path + 'injune_p142_casi_sub_utm.kea'\n"
"   output = path + 'injune_p142_casi_sub_utm_correlation.kea'\n"
"   window = 9\n"
"   bandA = 1\n"
"   bandB = 1\n"
"   gdalformat = 'KEA'\n"
"   datatype = rsgislib.TYPE_32FLOAT\n"
"   imagecalc.correlationWindow(image, output, window, bandA, bandB, gdalformat, datatype)\n"
"\n"},

{"getImageStatsInEnv", ImageCalc_GetImageStatsInEnv, METH_VARARGS,
"rsgislib.imagecalc.getImageStatsInEnv(inputImage, imgBand, noDataVal, latMin, latMax, longMin, longMax)\n"
"Calculates and returns statistics (min, max, mean, stddev, sum) for a region.\n"
"defined by the bounding box (latMin, latMax, longMin, longMax) which is specified\n"
"geographic latitude and longitude. The coordinates are converted to the projection\n"
"of the input image at runtime (if required) and therefore the image projection needs\n"
"to be correctly defined so please check this is the case and define it if necessary.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* imgBand is an unsigned int specifying the image band starting from 1.\n"
"* noDataVal is a float specifying a no data value, to be ignored in the calculation. If a value of \'None\' is provided then a no data value is not used.\n"
"* latMin is a double specifying the minimum latitude of the BBOX\n"
"* latMax is a double specifying the maximum latitude of the BBOX\n"
"* longMin is a double specifying the minimum longitude of the BBOX\n"
"* longMax is a double specifying the maximum longitude of the BBOX\n"
"\n"
"Returns:\n"
"\n"
"* list with 5 values (min, max, mean, stddev, sum)\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib.imagecalc\n"
"   stats = rsgislib.imagecalc.getImageStatsInEnv(\"./FinalSRTMTanzaniaDEM_30m.kea\", 1, -32767.0, -7.0, -8.0, 30.0, 31.0)\n"
"   print(\"Min: \", stats[0])\n"
"   print(\"Max: \", stats[1])\n"
"   print(\"Mean: \", stats[2])\n"
"   print(\"StdDev: \", stats[3])\n"
"   print(\"Sum: \", stats[4])\n\n"
"\n"},
    
{"getImageBandModeInEnv", ImageCalc_GetImageBandModeInEnv, METH_VARARGS,
"rsgislib.imagecalc.getImageBandModeInEnv(inputImage, imgBand, binWidth, noDataVal, latMin, latMax, longMin, longMax)\n"
"Calculates and returns the image mode for a region.\n"
"defined by the bounding box (latMin, latMax, longMin, longMax) which is specified\n"
"geographic latitude and longitude. The coordinates are converted to the projection\n"
"of the input image at runtime (if required) and therefore the image projection needs\n"
"to be correctly defined so please check this is the case and define it if necessary.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input image file\n"
"* imgBand is an unsigned int specifying the image band starting from 1.\n"
"* binWidth is a float specifying the binWidth for the histogram generated to calculate the mode.\n"
"* noDataVal is a float specifying a no data value, to be ignored in the calculation.\n"
"            If a value of \'None\' is provided then a no data value is not used.\n"
"* latMin is a double specifying the minimum latitude of the BBOX\n"
"* latMax is a double specifying the maximum latitude of the BBOX\n"
"* longMin is a double specifying the minimum longitude of the BBOX\n"
"* longMax is a double specifying the maximum longitude of the BBOX\n"
"\n"
"Returns:\n"
"\n"
"* float with image mode for the region within the BBOX.\n"
"\n"
},

{"get2DImageHistogram", ImageCalc_Get2DImageHistogram, METH_VARARGS,
"rsgislib.imagecalc.get2DImageHistogram(inputImage1, inputImage2, outputImage, gdalFormat, img1Band, img2Band, numBins, img1Min, img1Max, img2Min, img2Max, normOutput)\n"
"Calculates at 2D histogram between two bands of two input images\n"
"\n"
"Where:\n"
"\n"
"* inputImage1 is a string containing the name of the first input image file\n"
"* inputImage2 is a string containing the name of the second input image file\n"
"* outputImage is a string containing the name of the output image file containing the histogram.\n"
"* gdalFormat is a string specifying output image format.\n"
"* img1Band is an unsigned integer specifying the image band from image 1 to be used.\n"
"* img2Band is an unsigned integer specifying the image band from image 2 to be used.\n"
"* numBins is an unsigned integer specifying the number of bins to be used on each histogram axis\n"
"          (it'll produce a square histogram).\n"
"* img1Min is a double specifying the minimum image value for image 1 to be used in the histogram.\n"
"* img1Max is a double specifying the maximum image value for image 1 to be used in the histogram.\n"
"* img2Min is a double specifying the minimum image value for image 2 to be used in the histogram.\n"
"* img2Max is a double specifying the maximum image value for image 2 to be used in the histogram.\n"
"* normOutput is a boolean specifying whether the output histogram should be normalised to unit volume.\n"
"\n"
"Returns:\n"
"\n"
"* double with bin width of the axis of image 1.\n"
"* double with bin width of the axis of image 2.\n"
"\n"
},


{"calcMaskImgPxlValProb", ImageCalc_CalcMaskImgPxlValProb, METH_VARARGS,
"rsgislib.imagecalc.calcMaskImgPxlValProb(inputImage, inImgBands, maskImg, maskImgVal, outputImage, gdalFormat, histBinWidths, useImgNoData, rescaleProbs)\n"
"Calculates the probability of each image pixel value occuring as defined by the distrubution\n"
"of image pixel values within the masked region of the image.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name/path of the input image file.\n"
"* inImgBands is a list containing the image bands for which the probability will be calculated.\n"
"             (Note. number of output bands will equal number of bands specified here.\n"
"* maskImg is a string containing the name/path of the input mask image file.\n"
"* maskImgVal is an integer corresponding to the pixel value in the mask image defining mask used for this calculation.\n"
"* outputImage is a string containing the name of the output image file.\n"
"* gdalFormat is a string specifying output image format.\n"
"* histBinWidths is list of floating point values for the width of the histogram bins used to calculate the probability (one value for each band specified) \n"
"               (Note. larger bin widths will increase the difference between high and low probabilities) \n"
"               This parameter is optional and if not specified or value is less than 0 then the bin width will\n"
"               be estimated from the data.\n"
"* useImgNoData is a boolean specifying whether (if specified) the no data value specified in the band header\n"
"               should be excluded from the histogram (Optional and if not specfied defaults to True).\n"
"* rescaleProbs is a boolean specifying whether the probabilities should be rescaled to a range of 0-1 as values\n"
"              can be very small when a number of variables are used. (Optional and if not specified the default is True)."
"\n"
},


{"calcPropTrueExp", ImageCalc_CalcPropTrueExp, METH_VARARGS,
"rsgislib.imagecalc.calcPropTrueExp(expression, bandDefnSeq, validImgMask)\n"
"Calculates the proportion of the image where the expression is true. Optionally a mask defining the valid area \n"
"can be used to restrict the area of the image used as the total number of pixels within the scene.\n"
"\n"
"Where:\n"
"\n"
"* expression is a string containing the expression to run over the images, uses muparser syntax. Must output a value of 1 to be true.\n"
"* bandDefnSeq is a sequence of rsgislib.imagecalc.BandDefn objects that define the inputs\n"
"* validImgMask is an optional string specifying a valid area image mask. If not specified then it won't be used.\n"
"\n"
"Returns:\n"
"\n"
"* Returns a float value with the proportion\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import imagecalc\n"
"   from rsgislib.imagecalc import BandDefn\n"
"   expression = 'b1<20?1:b2>100?1:0'\n"
"   bandDefns = []\n"
"   bandDefns.append(BandDefn('b1', inFileName, 1))\n"
"   bandDefns.append(BandDefn('b2', inFileName, 2))\n"
"   prop = imagecalc.calcPropTrueExp(bandDefns, expression)\n"
"   print(prop)\n"
"\n"},
    
{"calcMultiImgBandStats", ImageCalc_calcMultiImgBandStats, METH_VARARGS,
"rsgislib.imagecalc.calcMultiImgBandStats(inputImages, outputImage, summaryStatOption, gdalFormat, datatype, noDataVal, useNoDataVal)\n"
"Calculates the summary statistic (rsgislib.SUMTYPE_*) across multiple images on a per band basis\n."
"For example, if rsgislib.SUMTYPE_MIN is selected then for all the images the minimum value for band 1 (across all the images) and then band 2 etc.\n"
"will be outputted as a new image with the same number of bands as the inputs (Note. all the input images must have the same number of bands).\n"
"\n"
"Where:\n"
"\n"
"* inputImages is a list of input images (note. all inputs must have the same number of image bands).\n"
"* outputImage is a string with the name and path of the output image.\n"
"* summaryStatOption is of type rsgislib.SUMTYPE_* and specifies which summary statistic is used to sumamrise the images.\n"
"* gdalFormat is a string specifying the output image format (e.g., KEA).\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"* noDataVal float with the value of the no data value, the same value for all the input images (Optional)\n"
"* useNoDataVal is a boolean specifying whether the no data value should be used (Optional, default False)\n"
"\n"},

{"calcImageDifference", ImageCalc_CalcImageDifference, METH_VARARGS,
"rsgislib.imagecalc.calcImageDifference(inputImage1, inputImage2, outputImage, gdalformat, datatype)\n"
"Calculate the difference between two images (Image1 - Image2). Note the two images must have the same number of image bands.\n"
"\n"
"Where:\n"
"\n"
"* inputImage1 is a string containing the name of the first input file\n"
"* inputImage2 is a string containing the name of the second input file\n"
"* outputImage is a string containing the name of the output file\n"
"* gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imagecalc\n"
"   from rsgislib import imageutils\n"
"   \n"
"   inputImage1 = 'output_orig/LS8_20131111_lat29lon8717_r40p140_vmsk_rad_srefdem.kea'\n"
"   inputImage2 = 'output_lutinterp/LS8_20131111_lat29lon8717_r40p140_vmsk_rad_srefdem.kea'\n"
"   outputImage = 'LS8_20131111_lat29lon8717_r40p140_srefdemDiff.kea'\n"
"   \n"
"   imagecalc.calcImageDifference(inputImage1, inputImage2, outputImage, 'KEA', rsgislib.TYPE_32FLOAT)\n"
"   imageutils.popImageStats(outputImage, usenodataval=False, nodataval=0, calcpyramids=True)\n"
"   \n"
"\n"},
    
{"getImageBandMinMax", ImageCalc_GetImageBandMinMax, METH_VARARGS,
"rsgislib.imagecalc.getImageBandMinMax(inputImage, imageBand, useNoDataVal, noDataVal)\n"
"Calculate and reutrn the maximum and minimum values of the input image.\n"
"\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input file\n"
"* imageBand is an int specifying the image band\n"
"* useNoDataVal is a boolean specifying whether the no data value should be used (Optional, default is False)\n"
"* noDataVal is a string containing the GDAL format for the output file - eg 'KEA'\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imagecalc\n"
"   \n"
"   inputImage = 'LS8_20131111_lat29lon8717_r40p140_vmsk_rad_srefdem.kea'\n"
"   imgBand = 1\n"
"   \n"
"   minMaxVals = imagecalc.getImageBandMinMax(inputImage, imgBand)\n"
"   print('MIN: ', minMaxVals[0])\n"
"   print('MAX: ', minMaxVals[1])\n"
"   \n"
"\n"},
    
{"calcImageRescale", ImageCalc_CalcImageRescale, METH_VARARGS,
"rsgislib.imagecalc.calcImageRescale(inputImgs, outputImage, gdalFormat, datatype, cNoDataVal, cOffset, cGain, nNoDataVal, nOffset, nGain)\n"
"A function which can take either a list of images or a single image to produce a single stacked output image.\n"
"The image values are rescaled applying the input (current; c) gain and offset and then applying the new (n) gain"
" and offset to the output image. Note, the nodata image value is also defined and can be changed. \n"
"For reference gain/offset are applied as: ImgVal = (gain * DN) + offset\n"
"\n"
"Where:\n"
"\n"
"* inputImgs can be either a single input image file or a list of images to be stacked.\n"
"* outputImage is the output image file.\n"
"* gdalFormat output raster format (e.g., KEA)\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"* cNoDataVal is a float for the current (existing) no-data value for the imagery (note, all input images have the same no-data value).\n"
"* cOffset is a float for the current offset value.\n"
"* cGain is a float for the current gain value.\n"
"* nNoDataVal is a float for the new no-data value for the imagery (note, all input images have the same no-data value).\n"
"* nOffset is a float for the new offset value.\n"
"* nGain is a float for the new gain value.\n"
"\n"},
    
    
{"getImgIdxForStat", (PyCFunction)ImageCalc_GetImgIdxForStat, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.getImgIdxForStat(inimages=list, outimage=string, gdalformat=string, nodata=float, stat=rsgislib.SUMTYPE_*)\n"
"A function which calculates the index (starting at 1) of the image in the list of input images which has the stat selected. \n"
"The output image can be used within the rsgislib.imageutils.createMaxNDVICompositeImg function."
"\n"
"Where:\n"
"\n"
"* inimages is a list of input images, which must each just have single image band.\n"
"* outputImage is a string with the name and path of the output image. No data value is 0 and indexes start at 1.\n"
"* gdalformat is a string with the GDAL output file format.\n"
"* nodata is the no data value in the input images (all images have the same no data value).\n"
"* stat is of type rsgislib.SUMTYPE_\\* and specifies how the index is calculated. Available options are: rsgislib.SUMTYPE_MEDIAN, rsgislib.SUMTYPE_MIN, rsgislib.SUMTYPE_MAX.\n"
"\n"
"Example::\n"
"\n"
"    import rsgislib\n"
"    import rsgislib.imagecalc\n"
"    import rsgislib.imageutils\n"
"    import rsgislib.rastergis\n"
"\n"
"    import glob\n"
"    import os.path\n"
"\n"
"    # Get List of input images:\n"
"    inImages = glob.glob('./Outputs/*stdsref.kea')\n"
"\n"
"    # Generate Comp Ref layers:\n"
"    refLyrsLst = []\n"
"    refLayerPath = './CompRefLyrs/'\n"
"    idx = 1\n"
"    for img in inImages:\n"
"        print('In Image ('+str(idx) + '):\t' + img)\n"
"        baseImgName = os.path.splitext(os.path.basename(img))[0]\n"
"        refLyrImg = os.path.join(refLayerPath, baseImgName+'_ndvi.kea')\n"
"        rsgislib.imagecalc.calcNDVI(img, 3, 4, refLyrImg)\n"
"        refLyrsLst.append(refLyrImg)\n"
"        idx = idx + 1\n"
"\n"
"    # Create REF Image\n"
"    pxlRefImg = 'LS5TM_19851990CompRefImg_lat7lon3896_r65p166_vmsk_mclds_topshad_rad_srefdem_stdsref.kea'\n"
"    rsgislib.imagecalc.getImgIdxForStat(refLyrsLst, pxlRefImg, 'KEA', -999, rsgislib.SUMTYPE_MAX)\n"
"\n"
"    # Pop Ref Image with stats\n"
"    rsgislib.rastergis.populateStats(pxlRefImg, True, True, True)\n"
"\n"
"    # Create Composite Image\n"
"    outCompImg = 'LS5TM_19851990CompRefImgMAX_lat7lon3896_r65p166_vmsk_mclds_topshad_rad_srefdem_stdsref.kea'\n"
"    rsgislib.imageutils.createRefImgCompositeImg(inImages, outCompImg, pxlRefImg, 'KEA', rsgislib.TYPE_16UINT, 0.0)\n"
"\n"
"    # Calc Stats\n"
"    rsgislib.imageutils.popImageStats(outCompImg, usenodataval=True, nodataval=0, calcpyramids=True)\n"
"\n"
"\n"
},
    
{"getImgSumStatsInPxl", (PyCFunction)ImageCalc_GetImgSumStatsInPxl, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.getImgSumStatsInPxl(refimage=string, statsimage=string, outimage=string, gdalformat=string, datatype=int, sumstats=list, statsimageband=int, usenodata=bool, iogridx=int, iogridy=int)\n"
"A function which calculates a summary of the pixel values from the high resolution statsimage for\n"
"the regions defined by the pixels in the lower resolution refimage. This is similar to zonal stats.\n"
"Please note that the statsimage needs to have a pixel size which is a multiple of the refimage pixel size.\n"
"For example, is the ref image has a resolution of 10 m the statsimage can have a resolution of 1 or 2 m \n"
"but not 3 m for this function to work.\n"
"\n"
"Where:\n"
"\n"
"* refimage is a string specifying the name and path for the reference image. Note, this is only used to define the\n"
"           summary areas and output image extent.\n"
"* statsimage is a string specifying the name and path to the higher resolution image which will be summarised.\n"
"* outimage is a string specifying the output image file name and path.\n"
"* gdalformat is a string with the GDAL output file format.\n"
"* datatype is an containing one of the values from rsgislib.TYPE_*\n"
"* sumstats is a list of the type rsgislib.SUMTYPE* and specifies the summary is calculated.\n"
"           Each summary statastic is saved as a different image band."
"* statsimageband is an integer specifying the image band in the stats image to be used for the analysis. (Default: 1)\n"
"* usenodata is a boolean specifying whether the image band no data value should be used. (Default: True)\n"
"* iogridx and iogridy are integers which control the image processing block size. The unit is pixels in the refimage. (Default: 16)\n"
"                      where the pixel resolution between the two images is closer together these values can be increased but \n"
"                      but where the statsimage pixel size is much smaller than the ref image reducing this will reduce the memory\n"
"                      footprint significantly.\n"
"\n"
"\n"},
    
{"identifyMinPxlValueInWin", (PyCFunction)ImageCalc_IdentifyMinPxlValueInWin, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.identifyMinPxlValueInWin(inputimg=string, outimage=string, outrefimg=string, bands=list, winsize=int, gdalformat=string, nodataval=float, usenodata=boolean)\n"
"A function to identify the minimum value across the image bands specified within a window.\n"
"\n"
"Where:\n"
"\n"
"* inputimg is a string specifying input image file.\n"
"* outimage is a string specifying image with the minimum pixel value.\n"
"* outrefimg is a string specifying the output file for the reference image - i.e., the band index.\n"
"* bands is a list of image bands (indexing starts at 1).\n"
"* winsize is an integer specifying the window size (must be an odd number).\n"
"* gdalformat is a string with the GDAL output file format.\n"
"* nodataval is a float specifying the no data value.\n"
"* usenodata is a boolean specifiying whether to use the no data value.\n"
"\n"
"\n"},
    
{"calcImgMeanInMask", (PyCFunction)ImageCalc_CalcImgMeanInMask, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.calcImgMeanInMask(inputimg=string, inputImgMsk=string, mskValue=int, bands=list, nodataval=float, usenodata=boolean)\n"
"A function to calculate the mean value of all the pixels specified within\n"
"the mask and across all the image bounds. \n"
"\n"
"Where:\n"
"\n"
"* inputimg is a string specifying input image file.\n"
"* inputImgMsk is a string specifying image with the mask file.\n"
"* mskValue the mask value (integer), within the input image mask, specifying the pixels over which the mean will be taken.\n"
"* bands is a list of image bands (indexing starts at 1).\n"
"* nodataval is a float specifying the no data value.\n"
"* usenodata is a boolean specifiying whether to use the no data value.\n"
"\n"
"Return: \t float with mean value.\n"
"\n"},

{NULL}        /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3

static int ImageCalc_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int ImageCalc_clear(PyObject *m)
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_imagecalc",
        NULL,
        sizeof(struct ImageCalcState),
        ImageCalcMethods,
        NULL,
        ImageCalc_traverse,
        ImageCalc_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC
PyInit__imagecalc(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_imagecalc(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_imagecalc", ImageCalcMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ImageCalcState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_imagecalc.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

    // add constants
    PyModule_AddIntConstant(pModule, "INITCLUSTER_RANDOM", rsgis::cmds::rsgis_init_random);
    PyModule_AddIntConstant(pModule, "INITCLUSTER_DIAGONAL_FULL", rsgis::cmds::rsgis_init_diagonal_full);
    PyModule_AddIntConstant(pModule, "INITCLUSTER_DIAGONAL_STDDEV", rsgis::cmds::rsgis_init_diagonal_stddev);
    PyModule_AddIntConstant(pModule, "INITCLUSTER_DIAGONAL_FULL_ATTACH", rsgis::cmds::rsgis_init_diagonal_full_attach);
    PyModule_AddIntConstant(pModule, "INITCLUSTER_DIAGONAL_STDDEV_ATTACH", rsgis::cmds::rsgis_init_diagonal_stddev_attach);

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
