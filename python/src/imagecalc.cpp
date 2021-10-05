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
    static char *kwlist[] = {RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("exp"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("band_defs"), RSGIS_PY_C_TEXT("exp_band_name"),
                             RSGIS_PY_C_TEXT("output_exists"), nullptr};
    const char *pszOutputFile, *pszExpression, *pszGDALFormat;
    int nDataType;
    int bExpBandName = 0;
    int bOutputImgExists = 0;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssiO|ii:bandMath", kwlist, &pszOutputFile, &pszExpression, &pszGDALFormat, &nDataType, &pBandDefnObj, &bExpBandName, &bOutputImgExists))
    {
        return nullptr;
    }

    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "last argument must be a sequence");
        return nullptr;
    }

    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    rsgis::cmds::VariableStruct *pRSGISStruct = new rsgis::cmds::VariableStruct[nBandDefns];

    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);

        PyObject *pBandName = PyObject_GetAttrString(o, "band_name");
        if( ( pBandName == nullptr ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'band_name\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return nullptr;
        }

        PyObject *pFileName = PyObject_GetAttrString(o, "input_img");
        if( ( pFileName == nullptr ) || ( pFileName == Py_None ) || !RSGISPY_CHECK_STRING(pFileName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'input_img\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pFileName);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return nullptr;
        }

        PyObject *pBandIndex = PyObject_GetAttrString(o, "img_band");
        if( ( pBandIndex == nullptr ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find integer attribute \'img_band\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return nullptr;
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
        return nullptr;
    }

    delete[] pRSGISStruct;

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ImageMath(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("exp"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("exp_band_name"),
                             RSGIS_PY_C_TEXT("output_exists"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszExpression, *pszGDALFormat;
    int nDataType;
    int bExpBandName = 0;
    int bOutputImgExists = 0;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssi|ii:imageMath", kwlist, &pszInputImage, &pszOutputFile, &pszExpression, &pszGDALFormat, &nDataType, &bExpBandName, &bOutputImgExists))
    {
        return nullptr;
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
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ImageBandMath(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("exp"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("exp_band_name"), RSGIS_PY_C_TEXT("output_exists"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszExpression, *pszGDALFormat;
    int nDataType;
    int bExpBandName = 0;
    int bOutputImgExists = 0;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssssi|ii:imageBandMath", kwlist, &pszInputImage, &pszOutputFile, &pszExpression, &pszGDALFormat, &nDataType, &bExpBandName, &bOutputImgExists))
    {
        return nullptr;
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
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalc_KMeansClustering(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("out_file"),
                             RSGIS_PY_C_TEXT("n_clusters"), RSGIS_PY_C_TEXT("max_n_iters"),
                             RSGIS_PY_C_TEXT("sub_sample"), RSGIS_PY_C_TEXT("ignore_zeros"),
                             RSGIS_PY_C_TEXT("degree_change"), RSGIS_PY_C_TEXT("init_cluster_method"), nullptr};

    const char *pszInputImage, *pszOutputFile;
    unsigned int nNumClusters, nMaxNumIterations, nSubSample;
    int nIgnoreZeros; // passed as a bool - seems the only way to pass into C
    float fDegreeOfChange;
    int nClusterMethod;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssIIIifi:kMeansClustering", kwlist, &pszInputImage, &pszOutputFile, &nNumClusters,
                                &nMaxNumIterations, &nSubSample, &nIgnoreZeros, &fDegreeOfChange, &nClusterMethod ))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeKMeansClustering(pszInputImage, pszOutputFile, nNumClusters, nMaxNumIterations,
                            nSubSample, nIgnoreZeros, fDegreeOfChange, (rsgis::cmds::RSGISInitClustererMethods)nClusterMethod);
        
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ISODataClustering(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("out_file"),
                             RSGIS_PY_C_TEXT("n_clusters"), RSGIS_PY_C_TEXT("max_n_iters"),
                             RSGIS_PY_C_TEXT("sub_sample"), RSGIS_PY_C_TEXT("ignore_zeros"),
                             RSGIS_PY_C_TEXT("degree_change"), RSGIS_PY_C_TEXT("init_cluster_method"),
                             RSGIS_PY_C_TEXT("min_dist_clusters"), RSGIS_PY_C_TEXT("min_n_feats"),
                             RSGIS_PY_C_TEXT("max_std_dev"), RSGIS_PY_C_TEXT("min_n_clusters"),
                             RSGIS_PY_C_TEXT("start_iter"), RSGIS_PY_C_TEXT("end_iter"), nullptr};
    const char *pszInputImage, *pszOutputFile;
    unsigned int nNumClusters, nMaxNumIterations, nSubSample, minNumFeatures, minNumClusters;
    unsigned int startIteration, endIteration;
    int nIgnoreZeros; // passed as a bool - seems the only way to pass into C
    float fDegreeOfChange, fMinDistBetweenClusters, maxStdDev;
    int nClusterMethod;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssIIIififIfIII:isoDataClustering", kwlist, &pszInputImage, &pszOutputFile, &nNumClusters,
                                &nMaxNumIterations, &nSubSample, &nIgnoreZeros, &fDegreeOfChange, &nClusterMethod,
                                &fMinDistBetweenClusters, &minNumFeatures, &maxStdDev, &minNumClusters,
                                &startIteration, &endIteration ))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeISODataClustering(pszInputImage, pszOutputFile, nNumClusters, nMaxNumIterations,
                            nSubSample, nIgnoreZeros, fDegreeOfChange, (rsgis::cmds::RSGISInitClustererMethods)nClusterMethod, fMinDistBetweenClusters,
                            minNumFeatures, maxStdDev, minNumClusters, startIteration, endIteration);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_MahalanobisDistFilter(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("win_size"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};

    const char *inputImage, *outputImage, *gdalFormat;
    unsigned int datatype, winSize;

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssIsI:mahalanobisDistFilter", kwlist, &inputImage, &outputImage, &winSize, &gdalFormat, &datatype))
    {
        return nullptr;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;

    try
    {
        rsgis::cmds::executeMahalanobisDistFilter(inputImage, outputImage, winSize, gdalFormat, type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_MahalanobisDist2ImgFilter(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("win_size"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};

    const char *inputImage, *outputImage, *gdalFormat;
    unsigned int datatype, winSize;

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssIsI:mahalanobisDist2ImgFilter", kwlist, &inputImage, &outputImage, &winSize, &gdalFormat, &datatype))
    {
        return nullptr;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;

    try
    {
        rsgis::cmds::executeMahalanobisDist2ImgFilter(inputImage, outputImage, winSize, gdalFormat, type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ImagePixelColumnSummary(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("sum_stats"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("no_data_val"),
                             RSGIS_PY_C_TEXT("use_no_data"), nullptr};

    const char *inputImage, *outputImage, *gdalFormat;
    unsigned int datatype;
    int useNoDataValue;
    float noDataValue;
    PyObject *summaryStats;
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssOsIfi:imagePixelColumnSummary", kwlist, &inputImage, &outputImage, &summaryStats, &gdalFormat, &datatype, &noDataValue, &useNoDataValue))
    {
        return nullptr;
    }

    // get the kw attrs from the object
    PyObject *pCalcMin = PyObject_GetAttrString(summaryStats, "calcMin");
    if( ( pCalcMin == nullptr ) || ( pCalcMin == Py_None ) || !RSGISPY_CHECK_INT(pCalcMin) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcMin\'" );
        Py_XDECREF(pCalcMin);
        return nullptr;
    }

    PyObject *pCalcMax = PyObject_GetAttrString(summaryStats, "calcMax");
    if( ( pCalcMax == nullptr ) || ( pCalcMax == Py_None ) || !RSGISPY_CHECK_INT(pCalcMax) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcMax\'" );
        Py_XDECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return nullptr;
    }

    PyObject *pCalcMean = PyObject_GetAttrString(summaryStats, "calcMean");
    if( ( pCalcMean == nullptr ) || ( pCalcMean == Py_None ) || !RSGISPY_CHECK_INT(pCalcMean) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcMean\'" );
        Py_XDECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return nullptr;
    }

    PyObject *pCalcSum = PyObject_GetAttrString(summaryStats, "calcSum");
    if( ( pCalcSum == nullptr ) || ( pCalcSum == Py_None ) || !RSGISPY_CHECK_INT(pCalcSum) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcSum\'" );
        Py_XDECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return nullptr;
    }

    PyObject *pCalcStdDev = PyObject_GetAttrString(summaryStats, "calcStdDev");
    if( ( pCalcStdDev == nullptr ) || ( pCalcStdDev == Py_None ) || !RSGISPY_CHECK_INT(pCalcStdDev) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcStdDev\'" );
        Py_XDECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return nullptr;
    }

    PyObject *pCalcMedian = PyObject_GetAttrString(summaryStats, "calcMedian");
    if( ( pCalcMedian == nullptr ) || ( pCalcMedian == Py_None ) || !RSGISPY_CHECK_INT(pCalcMedian) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcMedian\'" );
        Py_XDECREF(pCalcMedian);
        Py_DECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return nullptr;
    }
    
    PyObject *pCalcMode = PyObject_GetAttrString(summaryStats, "calcMode");
    if( ( pCalcMode == nullptr ) || ( pCalcMode == Py_None ) || !RSGISPY_CHECK_INT(pCalcMode) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find bool attribute \'calcMode\'" );
        Py_XDECREF(pCalcMode);
        Py_DECREF(pCalcMedian);
        Py_DECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return nullptr;
    }

    PyObject *pMin = PyObject_GetAttrString(summaryStats, "min");
    if( ( pMin == nullptr ) || ( pMin == Py_None ) || !RSGISPY_CHECK_FLOAT(pMin) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find float attribute \'min\'" );
        Py_XDECREF(pMin);
        Py_DECREF(pCalcMode);
        Py_DECREF(pCalcMedian);
        Py_DECREF(pCalcStdDev);
        Py_DECREF(pCalcSum);
        Py_DECREF(pCalcMean);
        Py_DECREF(pCalcMax);
        Py_DECREF(pCalcMin);
        return nullptr;
    }

    PyObject *pMax = PyObject_GetAttrString(summaryStats, "max");
    if( ( pMax == nullptr ) || ( pMax == Py_None ) || !RSGISPY_CHECK_FLOAT(pMax) ) {
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
        return nullptr;
    }

    PyObject *pMean = PyObject_GetAttrString(summaryStats, "mean");
    if( ( pMean == nullptr ) || ( pMean == Py_None ) || !RSGISPY_CHECK_FLOAT(pMean) ) {
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
        return nullptr;
    }

    PyObject *pSum = PyObject_GetAttrString(summaryStats, "sum");
    if( ( pSum == nullptr ) || ( pSum == Py_None ) || !RSGISPY_CHECK_FLOAT(pSum) ) {
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
        return nullptr;
    }

    PyObject *pStdDev = PyObject_GetAttrString(summaryStats, "stdDev");
    if( ( pStdDev == nullptr ) || ( pStdDev == Py_None ) || !RSGISPY_CHECK_FLOAT(pStdDev) ) {
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
        return nullptr;
    }

    PyObject *pMedian = PyObject_GetAttrString(summaryStats, "median");
    if( ( pMedian == nullptr ) || ( pMedian == Py_None ) || !RSGISPY_CHECK_FLOAT(pMedian) ) {
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
        return nullptr;
    }
    
    PyObject *pMode = PyObject_GetAttrString(summaryStats, "mode");
    if( ( pMode == nullptr ) || ( pMode == Py_None ) || !RSGISPY_CHECK_FLOAT(pMode) ) {
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
        return nullptr;
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
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ImagePixelLinearFit(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("band_values"), RSGIS_PY_C_TEXT("no_data_val"),
                             RSGIS_PY_C_TEXT("use_no_data"), nullptr};
    const char *inputImage, *outputImage, *gdalFormat, *bandValues;
    double noDataValue;
    int useNoDataValue;

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssssfi:imagePixelLinearFit", kwlist, &inputImage, &outputImage, &gdalFormat, &bandValues, &noDataValue, &useNoDataValue))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeImagePixelLinearFit(inputImage, outputImage, gdalFormat, bandValues, noDataValue, useNoDataValue);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_PCA(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("eigen_vec_file"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("n_comps"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),  nullptr};
    const char *eigenVectors, *inputImage, *outputImage;
    unsigned int numComponents;
    const char *gdalFormat;
    int datatype;

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssIsi:pca", kwlist, &inputImage, &eigenVectors, &outputImage, &numComponents, &gdalFormat, &datatype))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;
        rsgis::cmds::executePCA(std::string(inputImage), std::string(eigenVectors), std::string(outputImage), numComponents, std::string(gdalFormat), type);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *ImageCalc_CalculateRMSE(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_a_img"), RSGIS_PY_C_TEXT("img_a_band"),
                             RSGIS_PY_C_TEXT("in_b_img"), RSGIS_PY_C_TEXT("img_b_band"), nullptr};

    const char *inputImageA, *inputImageB;
    unsigned int bandA, bandB;

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sIsI:calculateRMSE", kwlist, &inputImageA, &bandA, &inputImageB, &bandB))
    {
        return nullptr;
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
        return nullptr;
    }

    return outVal;
}


static PyObject *ImageCalc_AllBandsEqualTo(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("img_val"), RSGIS_PY_C_TEXT("out_true_val"),
                             RSGIS_PY_C_TEXT("out_false_val"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    const char *inputImage, *outputImage, *imageFormat;
    float imgValue, outputTrueVal, outputFalseVal;
    int datatype;

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssfffsi:allBandsEqualTo", kwlist, &inputImage, &outputImage, &imgValue, &outputTrueVal, &outputFalseVal, &imageFormat, &datatype))
    {
        return nullptr;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;

    try
    {
        rsgis::cmds::executeAllBandsEqualTo(inputImage, imgValue, outputTrueVal, outputFalseVal, outputImage, imageFormat, type);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_Histogram(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("in_msk_img"), RSGIS_PY_C_TEXT("output_file"),
                             RSGIS_PY_C_TEXT("img_band"), RSGIS_PY_C_TEXT("msk_val"),
                             RSGIS_PY_C_TEXT("bin_width"), RSGIS_PY_C_TEXT("calc_min_max"),
                             RSGIS_PY_C_TEXT("min_val"), RSGIS_PY_C_TEXT("max_val"), nullptr};

    const char *inputImage, *outputFile, *imageMask;
    float imgValue;
    float binWidth, inMin, inMax;
    int calcInMinMax;
    unsigned int imgBand;

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssIffiff:histogram", kwlist, &inputImage, &imageMask, &outputFile, &imgBand, &imgValue, &binWidth, &calcInMinMax, &inMin, &inMax))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeHistogram(inputImage, imageMask, outputFile, imgBand, imgValue, binWidth, calcInMinMax, inMin, inMax);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_GetHistogram(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("img_band"),
                             RSGIS_PY_C_TEXT("bin_width"), RSGIS_PY_C_TEXT("calc_min_max"),
                             RSGIS_PY_C_TEXT("min_val"), RSGIS_PY_C_TEXT("max_val"), nullptr};

    const char *inputImage;
    float binWidth, inMin, inMax;
    int calcInMinMax;
    unsigned int imgBand;
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sIfiff:getHistogram", kwlist, &inputImage, &imgBand, &binWidth, &calcInMinMax, &inMin, &inMax))
    {
        return nullptr;
    }
    
    PyObject *binsList = nullptr;
    PyObject *outList = PyTuple_New(3);
    try
    {
        unsigned int nBins = 0;
        double inMinVal = inMin;
        double inMaxVal = inMax;
        unsigned int *bins = rsgis::cmds::executeGetHistogram(inputImage, imgBand, binWidth, &nBins, calcInMinMax, &inMinVal, &inMaxVal);
        
        Py_ssize_t listLen = nBins;
        
        
        binsList = PyTuple_New(listLen);
        if(binsList == nullptr)
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
        return nullptr;
    }
    
    return outList;
}

static PyObject *ImageCalc_BandPercentile(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("percentile"),
                             RSGIS_PY_C_TEXT("no_data_val"), nullptr};
    const char *inputImage;
    float percentile;
    PyObject *noDataValueObj;

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sfO:bandPercentile", kwlist, &inputImage, &percentile, &noDataValueObj))
    {
        return nullptr;
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
    
    PyObject *outVals = nullptr;
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
        return nullptr;
    }
    
    return outVals;
}


static PyObject *ImageCalc_CorrelationWindow(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("win_size"), RSGIS_PY_C_TEXT("band_a"),
                             RSGIS_PY_C_TEXT("band_b"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    const char *pszInputImage, *pszOutputImage, *pszGDALFormat;
    int datatype, windowSize, bandA, bandB;

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssiiisi:correlationWindow", kwlist, &pszInputImage, &pszOutputImage, &windowSize, &bandA, &bandB, &pszGDALFormat, &datatype))
    {
        return nullptr;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;
    try 
    {
    rsgis::cmds::executeCorrelationWindow(pszInputImage, pszOutputImage, windowSize, bandA, bandB, pszGDALFormat, type);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_GetImageStatsInEnv(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("img_band"),
                             RSGIS_PY_C_TEXT("no_data_val"), RSGIS_PY_C_TEXT("lon_min"),
                             RSGIS_PY_C_TEXT("lon_max"), RSGIS_PY_C_TEXT("lat_min"),
                             RSGIS_PY_C_TEXT("lat_max"), nullptr};
    const char *inputImage;
    unsigned int imgBand;
    double latMin, latMax, longMin, longMax;
    PyObject *noDataValueObj;
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sIOdddd:getImageStatsInEnv", kwlist, &inputImage, &imgBand, &noDataValueObj, &longMin, &longMax, &latMin, &latMax))
    {
        return nullptr;
    }
    
    bool noDataValueSpecified = false;
    float noDataValue = 0.0;
    
    if( ( noDataValueObj == nullptr ) || ( noDataValueObj == Py_None ) || !RSGISPY_CHECK_FLOAT(noDataValueObj) )
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
        
        rsgis::cmds::executeImageBandStatsEnv(std::string(inputImage), stats, imgBand, noDataValueSpecified, noDataValue, longMin, longMax, latMin, latMax);
        
        
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
        return nullptr;
    }
    
    return outValsList;
}


static PyObject *ImageCalc_GetImageBandModeInEnv(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("img_band"), RSGIS_PY_C_TEXT("bin_width"),
                             RSGIS_PY_C_TEXT("no_data_val"), RSGIS_PY_C_TEXT("lon_min"),
                             RSGIS_PY_C_TEXT("lon_max"), RSGIS_PY_C_TEXT("lat_min"),
                             RSGIS_PY_C_TEXT("lat_max"), nullptr};
    const char *inputImage;
    unsigned int imgBand;
    double latMin, latMax, longMin, longMax;
    float binWidth;
    PyObject *noDataValueObj;
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sIfOdddd:getImageBandModeInEnv", kwlist, &inputImage, &imgBand, &binWidth, &noDataValueObj, &longMin, &longMax, &latMin, &latMax))
    {
        return nullptr;
    }
    
    bool noDataValueSpecified = false;
    float noDataValue = 0.0;
    
    if( ( noDataValueObj == nullptr ) || ( noDataValueObj == Py_None ) || !RSGISPY_CHECK_FLOAT(noDataValueObj) )
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
        float modeVal = rsgis::cmds::executeImageBandModeEnv(std::string(inputImage), binWidth, imgBand, noDataValueSpecified, noDataValue, longMin, longMax, latMin, latMax);
        
        if(PyTuple_SetItem(outVal, 0, Py_BuildValue("f", modeVal)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'mode\' value to the list...");
        }
        
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    return outVal;
}

static PyObject *ImageCalc_Get2DImageHistogram(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_a_img"), RSGIS_PY_C_TEXT("in_b_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("img_a_band"), RSGIS_PY_C_TEXT("img_b_band"),
                             RSGIS_PY_C_TEXT("n_bins"), RSGIS_PY_C_TEXT("img_a_min"),
                             RSGIS_PY_C_TEXT("img_a_max"), RSGIS_PY_C_TEXT("img_b_min"),
                             RSGIS_PY_C_TEXT("img_b_max"), RSGIS_PY_C_TEXT("img_a_scale"),
                             RSGIS_PY_C_TEXT("img_b_scale"), RSGIS_PY_C_TEXT("img_a_offset"),
                             RSGIS_PY_C_TEXT("img_b_offset"), RSGIS_PY_C_TEXT("normalise"), nullptr};

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
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssssIIIddddddddi:get2DImageHistogram", kwlist, &inputImage1, &inputImage2,
                                    &outputImage, &gdalFormat, &img1Band, &img2Band, &numBins, &img1Min, &img1Max, &img2Min,
                                    &img2Max, &img1Scale, &img2Scale, &img1Off, &img2Off, &normOutput))
    {
        return nullptr;
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
        return nullptr;
    }
    
    return outVal;
}

static PyObject *ImageCalc_CalcMaskImgPxlValProb(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("img_bands"),
                             RSGIS_PY_C_TEXT("in_msk_img"), RSGIS_PY_C_TEXT("msk_val"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("bin_widths"), RSGIS_PY_C_TEXT("use_no_data"),
                             RSGIS_PY_C_TEXT("rescale_probs"), nullptr};

    const char *pszInputImage, *pszMaskImage, *pszOutputImage, *pszGDALFormat;
    int maskImgVal;
    int useImgNoData = true;
    int rescaleProbs = true;
    PyObject *inImgBandIdxsPyObj;
    PyObject *histBinWidthsPyObj;
    bool calcHistBinWidth = true;
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sOsLss|Oii:calcMaskImgPxlValProb", kwlist, &pszInputImage, &inImgBandIdxsPyObj,
                                    &pszMaskImage, &maskImgVal, &pszOutputImage, &pszGDALFormat,
                                    &histBinWidthsPyObj, &useImgNoData, &rescaleProbs))
    {
        return nullptr;
    }
    
    std::vector<float> histBinWidths;
    if(histBinWidthsPyObj != nullptr)
    {
        if( !PySequence_Check(histBinWidthsPyObj))
        {
            PyErr_SetString(GETSTATE(self)->error, "If provided histogram bin widths must be provided as a list.");
            return nullptr;
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
        return nullptr;
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
            return nullptr;
        }
    }
    
    try
    {
        rsgis::cmds::executeCalcMaskImgPxlValProb(std::string(pszInputImage), inImgBandIdxs, std::string(pszMaskImage), maskImgVal, std::string(pszOutputImage), std::string(pszGDALFormat), histBinWidths, calcHistBinWidth, useImgNoData, rescaleProbs);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyObject *ImageCalc_CalcPropTrueExp(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("exp"), RSGIS_PY_C_TEXT("band_defs"),
                             RSGIS_PY_C_TEXT("in_vld_img"), nullptr};

    const char *pszExpression;
    PyObject *pBandDefnObj = nullptr;
    PyObject *pInValidImageObj = nullptr;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sO|O:calcPropTrueExp", kwlist, &pszExpression, &pBandDefnObj, &pInValidImageObj))
    {
        return nullptr;
    }

    if( !PySequence_Check(pBandDefnObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "band defs argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nBandDefns = PySequence_Size(pBandDefnObj);
    rsgis::cmds::VariableStruct *pRSGISStruct = new rsgis::cmds::VariableStruct[nBandDefns];

    for( Py_ssize_t n = 0; n < nBandDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pBandDefnObj, n);
        
        PyObject *pBandName = PyObject_GetAttrString(o, "band_name");
        if( ( pBandName == nullptr ) || ( pBandName == Py_None ) || !RSGISPY_CHECK_STRING(pBandName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'band_name\'" );
            Py_XDECREF(pBandName);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return nullptr;
        }
        
        PyObject *pFileName = PyObject_GetAttrString(o, "input_img");
        if( ( pFileName == nullptr ) || ( pFileName == Py_None ) || !RSGISPY_CHECK_STRING(pFileName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find string attribute \'input_img\'" );
            Py_DECREF(pBandName);
            Py_XDECREF(pFileName);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return nullptr;
        }
        
        PyObject *pBandIndex = PyObject_GetAttrString(o, "img_band");
        if( ( pBandIndex == nullptr ) || ( pBandIndex == Py_None ) || !RSGISPY_CHECK_INT(pBandIndex) )
        {
            PyErr_SetString(GETSTATE(self)->error, "could not find integer attribute \'img_band\'" );
            Py_DECREF(pBandName);
            Py_DECREF(pFileName);
            Py_XDECREF(pBandIndex);
            Py_DECREF(o);
            delete[] pRSGISStruct;
            return nullptr;
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
        return nullptr;
    }
    delete[] pRSGISStruct;
    
    PyObject *outVal = Py_BuildValue("f", prop);
    return outVal;
}



static PyObject *ImageCalc_calcMultiImgBandStats(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("summary_stat"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("no_data_val"),
                             RSGIS_PY_C_TEXT("use_no_data"), nullptr};

    PyObject *inImagesObj;
    const char *outputImage;
    int sumStat;
    const char *gdalFormat;
    int datatype;
    float noDataVal = 0;
    int useNoDataVal = false;
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "Osisi|fi:calcMultiImgBandStats", kwlist, &inImagesObj, &outputImage, &sumStat, &gdalFormat, &datatype, &noDataVal, &useNoDataVal))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(inImagesObj) )
    {
        PyErr_SetString(GETSTATE(self)->error, "first three arguments must be sequences");
        return nullptr;
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
            return nullptr;
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
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::calcMultiImgBandsStats(inputImages, std::string(outputImage), summaryStats, std::string(gdalFormat), (rsgis::RSGISLibDataType)datatype, (bool)useNoDataVal, noDataVal);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyObject *ImageCalc_CalcImageDifference(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_a_img"), RSGIS_PY_C_TEXT("in_b_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};

    const char *inputImage1, *inputImage2, *outputImage, *gdalFormat;
    int datatype;
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssssi:calcImageDifference", kwlist, &inputImage1, &inputImage2, &outputImage, &gdalFormat, &datatype))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;
        rsgis::cmds::calcImageDifference(std::string(inputImage1), std::string(inputImage2), std::string(outputImage), std::string(gdalFormat), type);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalc_GetImageBandMinMax(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("img_band"),
                             RSGIS_PY_C_TEXT("use_no_data"), RSGIS_PY_C_TEXT("no_data_val"), nullptr};
    const char *inputImage;
    int imgBand;
    int useNoDataVal = false;
    float noDataVal = 0.0;
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "si|if:getImageBandMinMax", kwlist, &inputImage, &imgBand, &useNoDataVal, &noDataVal))
    {
        return nullptr;
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
        return nullptr;
    }
    
    return outList;
}

static PyObject *ImageCalc_CalcImageRescale(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("c_no_data_val"),
                             RSGIS_PY_C_TEXT("c_offset"), RSGIS_PY_C_TEXT("c_gain"),
                             RSGIS_PY_C_TEXT("n_no_data_val"),
                             RSGIS_PY_C_TEXT("n_offset"), RSGIS_PY_C_TEXT("n_gain"), nullptr};
    PyObject *pInputImgsObj;
    const char *outputImage, *gdalFormat;
    int datatype;
    float cNoDataVal, cOffset, cGain, nNoDataVal, nOffset, nGain = 0.0;
    
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "Ossiffffff:calcImageRescale", kwlist, &pInputImgsObj, &outputImage, &gdalFormat, &datatype, &cNoDataVal, &cOffset, &cGain, &nNoDataVal, &nOffset, &nGain))
    {
        return nullptr;
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
                return nullptr;
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
            return nullptr;
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
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalc_GetImgIdxForStat(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_imgs"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("no_data_val"),
                             RSGIS_PY_C_TEXT("stat"), nullptr};
    PyObject *pInputImages;
    const char *pszOutputImage = "";
    const char *pszGDALFormat = "";
    float noDataVal = 0.0;
    int statType;

    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "Ossfi:getImgIdxForStat", kwlist, &pInputImages, &pszOutputImage, &pszGDALFormat, &noDataVal, &statType))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pInputImages))
    {
        PyErr_SetString(GETSTATE(self)->error, "Input images must be a sequence");
        return nullptr;
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
            return nullptr;
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
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeGetImgIdxForStat(inputImages, std::string(pszOutputImage), std::string(pszGDALFormat), noDataVal, summaryStats);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}



static PyObject *ImageCalc_GetImgSumStatsInPxl(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_ref_img"), RSGIS_PY_C_TEXT("in_stats_img"),
                             RSGIS_PY_C_TEXT("output_img"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), RSGIS_PY_C_TEXT("sum_stats"),
                             RSGIS_PY_C_TEXT("stats_img_band"), RSGIS_PY_C_TEXT("use_no_data"),
                             RSGIS_PY_C_TEXT("io_grid_x"), RSGIS_PY_C_TEXT("io_grid_y"), nullptr};
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
        return nullptr;
    }
    
    if( !PySequence_Check(sumStatsInLst))
    {
        PyErr_SetString(GETSTATE(self)->error, "Summary stats must be a sequence");
        return nullptr;
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
            return nullptr;
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
            return nullptr;
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
        return nullptr;
    }
    
    Py_RETURN_NONE;
}




static PyObject *ImageCalc_IdentifyMinPxlValueInWin(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("out_ref_img"), RSGIS_PY_C_TEXT("img_bands"),
                             RSGIS_PY_C_TEXT("win_size"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("no_data_val"), RSGIS_PY_C_TEXT("use_no_data"), nullptr};
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
        return nullptr;
    }
    
    if( !PySequence_Check(bandsLstObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Bands list must be a sequence");
        return nullptr;
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
            return nullptr;
        }
        
        bandVal = RSGISPY_INT_EXTRACT(o);
        if(bandVal < 1)
        {
            PyErr_SetString(GETSTATE(self)->error, "Band value must be an integer with a minimum value of 1 (i.e., band indexing starts at 1).");
            Py_DECREF(o);
            return nullptr;
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
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageCalc_CalcImgMeanInMask(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("in_msk_img"),
                             RSGIS_PY_C_TEXT("msk_val"), RSGIS_PY_C_TEXT("img_bands"),
                             RSGIS_PY_C_TEXT("no_data_val"), RSGIS_PY_C_TEXT("use_no_data"), nullptr};
    const char *pInputImage = "";
    const char *pInputImageMsk = "";
    PyObject *bandsLstObj;
    int useNoDataValue;
    float noDataValue;
    int mskValue = 0.0;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssiOfi:calcImgMeanInMask", kwlist, &pInputImage, &pInputImageMsk, &mskValue, &bandsLstObj, &noDataValue, &useNoDataValue))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(bandsLstObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "Bands list must be a sequence");
        return nullptr;
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
            return nullptr;
        }
        
        bandVal = RSGISPY_INT_EXTRACT(o);
        if(bandVal < 1)
        {
            PyErr_SetString(GETSTATE(self)->error, "Band value must be an integer with a minimum value of 1 (i.e., band indexing starts at 1).");
            Py_DECREF(o);
            return nullptr;
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
        return nullptr;
    }
    
    PyObject *outVal = Py_BuildValue("f", meanVal);
    return outVal;
}

// Our list of functions in this module
static PyMethodDef ImageCalcMethods[] = {
    {"bandMath", (PyCFunction)ImageCalc_BandMath, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.bandMath(output_img, exp, gdalformat, datatype, band_defs, exp_band_name, output_exists)\n"
"Performs band math calculation.\n"
"The syntax for the expression is from the muparser library ('http://muparser.beltoforion.de <http://muparser.beltoforion.de>`): `see here <http://beltoforion.de/article.php?a=muparser&hl=en&p=features&s=idPageTop>`\n."
"\n"
"Where:\n"
"\n"
":param output_img: is a string containing the name of the output file\n"
":param exp: is a string containing the expression to run over the images, uses muparser syntax.\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param band_defs: is a sequence of rsgislib.imagecalc.BandDefn objects that define the inputs\n"
":param exp_band_name: is an optional bool specifying whether the band name should be the expression (Default = False).\n"
":param output_exists: is an optional bool specifying whether the output image already exists and it should be editted rather than overwritten (Default=False)."
"\n"
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
"\n"
"\n"},

{"imageMath", (PyCFunction)ImageCalc_ImageMath, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.imageMath(input_img, output_img, exp, gdalformat, datatype, exp_band_name, output_exists)\n"
"Performs image math calculations. Produces an output image file with the same number of bands as the input image.\n"
"This function applies the same calculation to each image band (i.e., b1 is the only variable).\n"
"The syntax for the expression is from the muparser library ('http://muparser.beltoforion.de <http://muparser.beltoforion.de>`): `see here <http://beltoforion.de/article.php?a=muparser&hl=en&p=features&s=idPageTop>`\n."
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param exp: is a string containing the expression to run over the images, uses myparser syntax.\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param exp_band_name: is an optional bool specifying whether the band name should be the expression (Default = False).\n"
":param output_exists: is an optional bool specifying whether the output image already exists and it should be editted rather than overwritten (Default=False)."
"\n"
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
"\n"
"\n"},

{"imageBandMath", (PyCFunction)ImageCalc_ImageBandMath, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.imageBandMath(input_img, output_img, exp, gdalformat, datatype, exp_band_name, output_exists)\n"
"Performs image band math calculations. Produces a single output file with a single image band.\n"
"The image bands can be referred to individually using b1, b2 ... bn. where n is the number of image bands, starting at 1.\n"
"The syntax for the expression is from the muparser library ('http://muparser.beltoforion.de <http://muparser.beltoforion.de>`): `see here <http://beltoforion.de/article.php?a=muparser&hl=en&p=features&s=idPageTop>`\n."
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param exp: is a string containing the expression to run over the images, uses myparser syntax.\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param exp_band_name: is an optional bool specifying whether the band name should be the expression (Default = False).\n"
":param output_exists: is an optional bool specifying whether the output image already exists and it should be editted rather than overwritten (Default=False)."
"\n"
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
"\n"
"\n"},

{"kMeansClustering", (PyCFunction)ImageCalc_KMeansClustering, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.kMeansClustering(input_img, out_file, n_clusters, max_n_iters, sub_sample, ignore_zeros, degree_change, init_cluster_method)\n"
"Performs K Means Clustering and saves cluster centres to a text file.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string providing the input image\n"
":param out_file: is a string providing the output matrix (text file) to save the cluster centres to.\n"
":param n_clusters: is the number of clusters to use.\n"
":param max_n_iters: is the maximum number of itterations.\n"
":param sub_sample: is an int specifying what fraction of the total pixels should be considered (e.g., 100 = 1/100 pixels).\n"
":param ignore_zeros: is a bool specifying if zeros in the image should be treated as no data.\n"
":param degree_change: is a float providing the minimum change between iterations before terminating.\n"
":param init_cluster_method: the method for initialising the clusters and is one of INITCLUSTER_* values\n"
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

{"isoDataClustering", (PyCFunction)ImageCalc_ISODataClustering, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.isoDataClustering(input_img, out_file, n_clusters, max_n_iters, sub_sample, ignore_zeros, degree_change, init_cluster_method, min_dist_clusters, min_n_feats, max_std_dev, min_n_clusters, start_iter, end_iter)\n"
"Performs ISO Data Clustering and saves cluster centres to a text file.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string providing the input image\n"
":param out_file: is a string providing the output matrix (text file) to save the cluster centres to.\n"
":param n_clusters: is the number of clusters to start with.\n"
":param max_n_iters: is the maximum number of iterations.\n"
":param sub_sample: is an int specifying what fraction of the total pixels should be considered (e.g., 100 = 1/100 pixels).\n"
":param ignore_zeros: is a bool specifying if zeros in the image should be treated as no data.\n"
":param degree_change: is a float providing the minimum change between iterations before terminating.\n"
":param init_cluster_method: the method for initialising the clusters and is one of INITCLUSTER_:param values\n"
":param min_dist_clusters: is a float\n"
":param min_n_feats: is an int\n"
":param max_std_dev: is a float\n"
":param min_n_clusters: is an int\n"
":param start_iter: is an int\n"
":param end_iter: is an int\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imagecalc\n"
"   inputImage = path + 'Rasters/injune_p142_casi_sub_right_utm.kea'\n"
"   output = './TestOutputs/isocentres'\n"
"   imagecalc.isoDataClustering(inputImage, output, 10, 200, 1, True, 0.0025, rsgislib.INITCLUSTER_DIAGONAL_FULL_ATTACH, 2, 5, 5, 5, 8, 50)\n"
"\n"},

{"mahalanobisDistFilter", (PyCFunction)ImageCalc_MahalanobisDistFilter, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.mahalanobisDistFilter(input_img, output_img, win_size, gdalformat, datatype)\n"
"Performs mahalanobis distance window filter.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param win_size: is an int defining the size of the window to be used\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param dataType: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"
},

{"mahalanobisDist2ImgFilter", (PyCFunction)ImageCalc_MahalanobisDist2ImgFilter, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.mahalanobisDist2ImgFilter(input_img, output_img, win_size, gdalformat, datatype)\n"
"Performs mahalanobis distance image to window filter.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param win_size: is an int defining the size of the window to be used\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"
},

{"imagePixelColumnSummary", (PyCFunction)ImageCalc_ImagePixelColumnSummary, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.imagePixelColumnSummary(input_img, output_img, sum_stats, gdalformat, datatype, no_data_val, use_no_data)\n"
"Calculates summary statistics for a column of pixels.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param sum_stats: is an rsgislib.imagecalc.StatsSummary object that has attributes matching rsgis.cmds.RSGISCmdStatsSummary\n"
"        * calcMin: boolean defining if the min value should be calculated\n"
"        * calcMax: boolean defining if the max value should be calculated\n"
"        * calcSum: boolean defining if the sum value should be calculated\n"
"        * calcMean: boolean defining if the mean value should be calculated\n"
"        * calcStdDev: boolean defining if the standard deviation should be calculated\n"
"        * calcMedian: boolean defining if the median value should be calculated\n"
"        * calcMode: boolean defining if the mode value should be calculated; warning can be slow.\n"
"        * min: float defining the min value to use\n"
"        * max: float defining the max value to use\n"
"        * mean: float defining the mean value to use\n"
"        * sum: float defining the sum value to use\n"
"        * stdDev: float defining the standard deviation value to use\n"
"        * median: float defining the median value to use\n"
"        * mode: float defining the mode value to use\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
":param no_data_val: is a float specifying what value is used to signify no data\n"
":param use_no_data: is a boolean specifying whether the noDataValue should be used\n"
"\n"
},

{"imagePixelLinearFit", (PyCFunction)ImageCalc_ImagePixelLinearFit, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.imagePixelLinearFit(input_img, output_img, gdalformat, band_values, no_data_val, use_no_data)\n"
"Performs a linear regression on each column of pixels.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param band_values: is text file containing the value of each band (e.g. wavelength, day of year) with a separate line for each band\n"
":param no_data_val: is a float specifying what value is used to signify no data\n"
":param use_no_data: is a boolean specifying whether the noDataValue should be used\n"
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

{"pca", (PyCFunction)ImageCalc_PCA, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.pca(input_img, eigen_vec_file, output_img, n_comps, gdalformat, dataType)\n"
"Performs a principal components analysis of an image using a defined set of eigenvectors.\n"
"The eigenvectors can be calculated using the rsgislib.imagecalc.getPCAEigenVector function.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param eigen_vec_file: is a string containing the name of the file of eigen vectors for the PCA\n"
":param output_img: is a string containing the name of the output image file\n"
":param n_comps: is an int containing number of components to use for PCA\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
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

{"calculateRMSE", (PyCFunction)ImageCalc_CalculateRMSE, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.calculateRMSE(in_a_img, img_a_band, in_b_img, img_b_band)\n"
"Calculates the root mean squared error between two images\n"
"\n"
"Where:\n"
"\n"
":param in_a_img: is a string containing the name of the first input image file\n"
":param img_a_band: is an integer defining which band should be processed from inputImageA\n"
":param in_b_img: is a string containing the name of the second input image file\n"
":param img_b_band: is an integer defining which band should be processed from inputImageB\n"
":return: float\n"
"\n"
},

{"allBandsEqualTo", (PyCFunction)ImageCalc_AllBandsEqualTo, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.allBandsEqualTo(input_img, output_img,  img_val, out_true_val, out_false_val, gdalformat, datatype)\n"
"Tests whether all bands are equal to the same value\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param output_img: is a string containing the name of the output image file\n"
":param img_val: is a float specifying the value against which others are tested for equality TODO: Check this and below\n"
":param out_true_val: is a float specifying the value in the output image representing true \n"
":param out_false_val: is a float specifying the value in the output image representing false \n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
"\n"
},

{"histogram", (PyCFunction)ImageCalc_Histogram, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.histogram(input_img, in_msk_img, output_file, img_band, msk_val, bin_width, calc_min_max, min_val, max_val)\n"
"Generates a histogram for the region of the mask selected\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param in_msk_img: is a string containing the name of the image mask file\n"
":param output_file: is a string containing the name of the file for histogram output\n"
":param img_band: is an integer for the band within the image (band indexing start at 1)\n"
":param msk_val: is a float for the value within the input mask for the regions the histogram will be calculated\n"
":param bin_width: is a float specifying the width of the histogram bins\n"
":param calc_min_max: is a boolean specifying whether inMin and inMax should be calculated\n"
":param min_val: is a float for the minimum image value to be included in the histogram\n"
":param max_val: is a floatf or the maximum image value to be included in the histogram\n"
"\n"
},
    
{"getHistogram", (PyCFunction)ImageCalc_GetHistogram, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.getHistogram(input_img, img_band, bin_width, calc_min_max, min_val, max_val)\n"
"Generates and returns a histogram for the image.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param img_band: is an unsigned int specifying the image band starting from 1.\n"
":param bin_width: is a float specifying the width of the histogram bins\n"
":param calc_min_max: is a boolean specifying whether inMin and inMax should be calculated\n"
":param min_val: is a float for the minimum image value to be included in the histogram\n"
":param max_val: is a float or the maximum image value to be included in the histogram\n"
"\n"
":return: list of ints"
"\n"
},

{"bandPercentile", (PyCFunction)ImageCalc_BandPercentile, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.bandPercentile(input_img, percentile, no_data_val)\n"
"Calculates image band percentiles for the input image and results a list of values\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param percentile: is a float between 0 -- 1 specifying the percentile to be calculated.\n"
":param no_data_val: is a float specifying the value used to represent no data (used None when no value is to be specified).\n"
"\n"
":return: list of floats\n"
"\n"
},


{"correlationWindow", (PyCFunction)ImageCalc_CorrelationWindow, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.correlationWindow(input_img, output_img, win_size, band_a, band_b, gdalformat, datatype)\n"
"Calculates the correlation between two image bands within a window.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param output_img: is a string containing the name of the output image file\n"
":param win_size: is an int providing the size of the window to calculate the correlation over\n"
":param band_a: is an int providing the first band to use.\n"
":param band_b: is an int providing the second band to use.\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
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

{"getImageStatsInEnv", (PyCFunction)ImageCalc_GetImageStatsInEnv, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.getImageStatsInEnv(input_img, img_band, no_data_val, lon_min, lon_max, lat_min, lat_max)\n"
"Calculates and returns statistics (min, max, mean, stddev, sum) for a region.\n"
"defined by the bounding box (longMin, longMax, latMin, latMax) which is specified\n"
"geographic latitude and longitude. The coordinates are converted to the projection\n"
"of the input image at runtime (if required) and therefore the image projection needs\n"
"to be correctly defined so please check this is the case and define it if necessary.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param img_band: is an unsigned int specifying the image band starting from 1.\n"
":param no_data_val: is a float specifying a no data value, to be ignored in the calculation. If a value of \'None\' is provided then a no data value is not used.\n"
":param lon_min: is a double specifying the minimum longitude of the BBOX\n"
":param lon_max: is a double specifying the maximum longitude of the BBOX\n"
":param lat_min: is a double specifying the minimum latitude of the BBOX\n"
":param lat_max: is a double specifying the maximum latitude of the BBOX\n"
"\n"
":return: list with 5 values (min, max, mean, stddev, sum)\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib.imagecalc\n"
"   stats = rsgislib.imagecalc.getImageStatsInEnv(\"./FinalSRTMTanzaniaDEM_30m.kea\", 1, -32767.0, 30.0, 31.0, -7.0, -8.0)\n"
"   print(\"Min: \", stats[0])\n"
"   print(\"Max: \", stats[1])\n"
"   print(\"Mean: \", stats[2])\n"
"   print(\"StdDev: \", stats[3])\n"
"   print(\"Sum: \", stats[4])\n\n"
"\n"},
    
{"getImageBandModeInEnv", (PyCFunction)ImageCalc_GetImageBandModeInEnv, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.getImageBandModeInEnv(input_img, img_band, bin_width, no_data_val, lon_min, lon_max, lat_min, lat_max)\n"
"Calculates and returns the image mode for a region.\n"
"defined by the bounding box (longMin, longMax, latMin, latMax) which is specified\n"
"geographic latitude and longitude. The coordinates are converted to the projection\n"
"of the input image at runtime (if required) and therefore the image projection needs\n"
"to be correctly defined so please check this is the case and define it if necessary.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param img_band: is an unsigned int specifying the image band starting from 1.\n"
":param bin_width: is a float specifying the binWidth for the histogram generated to calculate the mode.\n"
":param no_data_val: is a float specifying a no data value, to be ignored in the calculation.\n"
"            If a value of \'None\' is provided then a no data value is not used.\n"
":param lon_min: is a double specifying the minimum longitude of the BBOX\n"
":param lon_max: is a double specifying the maximum longitude of the BBOX\n"
":param lat_min: is a double specifying the minimum latitude of the BBOX\n"
":param lat_max: is a double specifying the maximum latitude of the BBOX\n"
"\n"
":return: float with image mode for the region within the BBOX.\n"
"\n"
},

{"get2DImageHistogram", (PyCFunction)ImageCalc_Get2DImageHistogram, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.get2DImageHistogram(in_a_img, in_b_img, output_img, gdalformat, img_a_band, img_b_band, n_bins, img_a_min, img_a_max, img_b_min, img_b_max, img_a_scale, img_b_scale, img_a_offset, img_b_offset, normalise)\n"
"Calculates at 2D histogram between two bands of two input images\n"
"\n"
"Where:\n"
"\n"
":param in_a_img: is a string containing the name of the first input image file\n"
":param in_b_img: is a string containing the name of the second input image file\n"
":param output_img: is a string containing the name of the output image file containing the histogram.\n"
":param gdalformat: is a string specifying output image format.\n"
":param img_a_band: is an unsigned integer specifying the image band from image a to be used.\n"
":param img_b_band: is an unsigned integer specifying the image band from image b to be used.\n"
":param n_bins: is an unsigned integer specifying the number of bins to be used on each histogram axis\n"
"          (it'll produce a square histogram).\n"
":param img_a_min: is a double specifying the minimum image value for image a to be used in the histogram.\n"
":param img_a_max: is a double specifying the maximum image value for image a to be used in the histogram.\n"
":param img_b_min: is a double specifying the minimum image value for image b to be used in the histogram.\n"
":param img_b_max: is a double specifying the maximum image value for image b to be used in the histogram.\n"
":param img_a_scale: is a double specifying a scale for the pixel value in image a.\n"
":param img_b_scale: is a double specifying a scale for the pixel value in image b.\n"
":param img_a_offset: is a double specifying an offset value for the pixel values in image a.\n"
":param img_b_offset: is a double specifying an offset value for the pixel values in image b.\n"
":param normalise: is a boolean specifying whether the output histogram should be normalised to unit volume.\n"
"\n"
":return: (double with bin width of the axis of image 1), (double with bin width of the axis of image 2)\n"
"\n"
},


{"calcMaskImgPxlValProb", (PyCFunction)ImageCalc_CalcMaskImgPxlValProb, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.calcMaskImgPxlValProb(input_img, img_bands, in_msk_img, msk_val, output_img, gdalformat, bin_widths, use_no_data, rescale_probs)\n"
"Calculates the probability of each image pixel value occurring as defined by the distribution\n"
"of image pixel values within the masked region of the image.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name/path of the input image file.\n"
":param img_bands: is a list containing the image bands for which the probability will be calculated.\n"
"             (Note. number of output bands will equal number of bands specified here.\n"
":param in_msk_img: is a string containing the name/path of the input mask image file.\n"
":param msk_val: is an integer corresponding to the pixel value in the mask image defining mask used for this calculation.\n"
":param output_img: is a string containing the name of the output image file.\n"
":param gdalformat: is a string specifying output image format.\n"
":param bin_widths: is list of floating point values for the width of the histogram bins used to calculate the probability (one value for each band specified) \n"
"               (Note. larger bin widths will increase the difference between high and low probabilities) \n"
"               This parameter is optional and if not specified or value is less than 0 then the bin width will\n"
"               be estimated from the data.\n"
":param use_no_data: is a boolean specifying whether (if specified) the no data value specified in the band header\n"
"               should be excluded from the histogram (Optional and if not specified defaults to True).\n"
":param rescale_probs: is a boolean specifying whether the probabilities should be rescaled to a range of 0-1 as values\n"
"              can be very small when a number of variables are used. (Optional and if not specified the default is True)."
"\n"
},

{"calcPropTrueExp", (PyCFunction)ImageCalc_CalcPropTrueExp, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.calcPropTrueExp(exp, band_defs, in_vld_img)\n"
"Calculates the proportion of the image where the expression is true. Optionally a mask defining the valid area \n"
"can be used to restrict the area of the image used as the total number of pixels within the scene.\n"
"\n"
"Where:\n"
"\n"
":param exp: is a string containing the expression to run over the images, uses muparser syntax. Must output a value of 1 to be true.\n"
":param band_defs: is a sequence of rsgislib.imagecalc.BandDefn objects that define the inputs\n"
":param in_vld_img: is an optional string specifying a valid area image mask (assume valid is pixel values 1 in band 1). If not specified then it won't be used.\n"
"\n"
":return: Returns a float value with the proportion\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import imagecalc\n"
"   from rsgislib.imagecalc import BandDefn\n"
"   expression = 'b1<20?1:b2>100?1:0'\n"
"   bandDefns = []\n"
"   bandDefns.append(BandDefn('b1', inFileName, 1))\n"
"   bandDefns.append(BandDefn('b2', inFileName, 2))\n"
"   prop = imagecalc.calcPropTrueExp(expression, bandDefns)\n"
"   print(prop)\n"
"\n"},
    
{"calcMultiImgBandStats", (PyCFunction)ImageCalc_calcMultiImgBandStats, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.calcMultiImgBandStats(input_imgs, output_img, summary_stat, gdalformat, datatype, no_data_val, use_no_data)\n"
"Calculates the summary statistic (rsgislib.SUMTYPE_*) across multiple images on a per band basis\n."
"For example, if rsgislib.SUMTYPE_MIN is selected then for all the images the minimum value for band 1 (across all the images) and then band 2 etc.\n"
"will be outputted as a new image with the same number of bands as the inputs (Note. all the input images must have the same number of bands).\n"
"\n"
"Where:\n"
"\n"
":param input_imgs: is a list of input images (note. all inputs must have the same number of image bands).\n"
":param output_img: is a string with the name and path of the output image.\n"
":param summary_stat: is of type rsgislib.SUMTYPE_* and specifies which summary statistic is used to sumamrise the images.\n"
":param gdalformat: is a string specifying the output image format (e.g., KEA).\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param no_data_val: float with the value of the no data value, the same value for all the input images (Optional)\n"
":param use_no_data: is a boolean specifying whether the no data value should be used (Optional, default False)\n"
"\n"},

{"calcImageDifference", (PyCFunction)ImageCalc_CalcImageDifference, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.calcImageDifference(in_a_img, in_b_img, output_img, gdalformat, datatype)\n"
"Calculate the difference between two images (Image1 - Image2). Note the two images must have the same number of image bands.\n"
"\n"
"Where:\n"
"\n"
":param in_a_img: is a string containing the name of the first input file\n"
":param in_b_img: is a string containing the name of the second input file\n"
":param output_img: is a string containing the name of the output file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg 'KEA'\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
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
    
{"getImageBandMinMax", (PyCFunction)ImageCalc_GetImageBandMinMax, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.getImageBandMinMax(input_img, img_band, use_no_data, no_data_val)\n"
"Calculate and reutrn the maximum and minimum values of the input image.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input file\n"
":param img_band: is an int specifying the image band\n"
":param use_no_data: is a boolean specifying whether the no data value should be used (Optional, default is False)\n"
":param no_data_val: is a string containing the GDAL format for the output file - eg 'KEA'\n"
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
    
{"calcImageRescale", (PyCFunction)ImageCalc_CalcImageRescale, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.calcImageRescale(input_imgs, output_img, gdalformat, datatype, c_no_data_val, c_offset, c_gain, n_no_data_val, n_offset, n_gain)\n"
"A function which can take either a list of images or a single image to produce a single stacked output image.\n"
"The image values are rescaled applying the input (current; c) gain and offset and then applying the new (n) gain"
" and offset to the output image. Note, the nodata image value is also defined and can be changed. \n"
"For reference gain/offset are applied as: ImgVal = (gain x DN) + offset\n"
"\n"
"Where:\n"
"\n"
":param inputImgs: can be either a single input image file or a list of images to be stacked.\n"
":param outputImage: is the output image file.\n"
":param gdalformat: output raster format (e.g., KEA)\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_\n"
":param c_no_data_val: is a float for the current (existing) no-data value for the imagery (note, all input images have the same no-data value).\n"
":param c_offset: is a float for the current offset value.\n"
":param c_gain: is a float for the current gain value.\n"
":param n_no_data_val: is a float for the new no-data value for the imagery (note, all input images have the same no-data value).\n"
":param n_offset: is a float for the new offset value.\n"
":param n_gain: is a float for the new gain value.\n"
"\n"
"\n"},
    
    
{"getImgIdxForStat", (PyCFunction)ImageCalc_GetImgIdxForStat, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.getImgIdxForStat(input_imgs=list, output_img=string, gdalformat=string, no_data_val=float, stat=rsgislib.SUMTYPE_)\n"
"A function which calculates the index (starting at 1) of the image in the list of input images which has the stat selected. \n"
"The output image can be used within the rsgislib.imageutils.createMaxNDVICompositeImg function."
"\n"
"Where:\n"
"\n"
":param input_imgs: is a list of input images, which must each just have single image band.\n"
":param output_img: is a string with the name and path of the output image. No data value is 0 and indexes start at 1.\n"
":param gdalformat: is a string with the GDAL output file format.\n"
":param no_data_val: is the no data value in the input images (all images have the same no data value).\n"
":param stat: is of type rsgislib.SUMTYPE_* and specifies how the index is calculated. Available options are: rsgislib.SUMTYPE_MEDIAN, rsgislib.SUMTYPE_MIN, rsgislib.SUMTYPE_MAX.\n"
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
"rsgislib.imagecalc.getImgSumStatsInPxl(in_ref_img=string, in_stats_img=string, output_img=string, gdalformat=string, datatype=int, sum_stats=list, stats_img_band=int, use_no_data=bool, io_grid_x=int, io_grid_y=int)\n"
"A function which calculates a summary of the pixel values from the high resolution statsimage for\n"
"the regions defined by the pixels in the lower resolution refimage. This is similar to zonal stats.\n"
"Please note that the statsimage needs to have a pixel size which is a multiple of the refimage pixel size.\n"
"For example, is the ref image has a resolution of 10 m the statsimage can have a resolution of 1 or 2 m \n"
"but not 3 m for this function to work.\n"
"\n"
"Where:\n"
"\n"
":param in_ref_img: is a string specifying the name and path for the reference image. Note, this is only used to define the\n"
"           summary areas and output image extent.\n"
":param in_stats_img: is a string specifying the name and path to the higher resolution image which will be summarised.\n"
":param output_img: is a string specifying the output image file name and path.\n"
":param gdalformat: is a string with the GDAL output file format.\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param sum_stats: is a list of the type rsgislib.SUMTYPE_* and specifies the summary is calculated.\n"
"                  Each summary statastic is saved as a different image band."
":param stats_img_band: is an integer specifying the image band in the stats image to be used for the analysis. (Default: 1)\n"
":param use_no_data: is a boolean specifying whether the image band no data value should be used. (Default: True)\n"
":param io_grid_x: and io_grid_y are integers which control the image processing block size. The unit is pixels in the refimage. (Default: 16)\n"
"                      where the pixel resolution between the two images is closer together these values can be increased but \n"
"                      but where the statsimage pixel size is much smaller than the ref image reducing this will reduce the memory\n"
"                      footprint significantly.\n"
"\n"
"\n"},
    
{"identifyMinPxlValueInWin", (PyCFunction)ImageCalc_IdentifyMinPxlValueInWin, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.identifyMinPxlValueInWin(input_img=string, output_img=string, out_ref_img=string, img_bands=list, win_size=int, gdalformat=string, no_data_val=float, use_no_data=boolean)\n"
"A function to identify the minimum value across the image bands specified within a window.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying input image file.\n"
":param output_img: is a string specifying image with the minimum pixel value.\n"
":param out_ref_img: is a string specifying the output file for the reference image - i.e., the band index.\n"
":param img_bands: is a list of image bands (indexing starts at 1) from the input image.\n"
":param win_size: is an integer specifying the window size (must be an odd number).\n"
":param gdalformat: is a string with the GDAL output file format.\n"
":param no_data_val: is a float specifying the no data value.\n"
":param use_no_data: is a boolean specifiying whether to use the no data value.\n"
"\n"},
    
{"calcImgMeanInMask", (PyCFunction)ImageCalc_CalcImgMeanInMask, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.calcImgMeanInMask(input_img=string, in_msk_img=string, msk_val=int, img_bands=list, no_data_val=float, use_no_data=boolean)\n"
"A function to calculate the mean value of all the pixels specified within\n"
"the mask and across all the image bounds. \n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying input image file.\n"
":param in_msk_img: is a string specifying image with the mask file.\n"
":param msk_val: the mask value (integer), within the input image mask, specifying the pixels over which the mean will be taken.\n"
":param img_bands: is a list of image bands (indexing starts at 1).\n"
":param no_data_val: is a float specifying the no data value.\n"
":param use_no_data: is a boolean specifiying whether to use the no data value.\n"
"\n"
":return: float with mean value.\n"
"\n"},

{nullptr}        /* Sentinel */
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
        nullptr,
        sizeof(struct ImageCalcState),
        ImageCalcMethods,
        nullptr,
        ImageCalc_traverse,
        ImageCalc_clear,
        nullptr
};

#define INITERROR return nullptr

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
    if( pModule == nullptr )
        INITERROR;

    struct ImageCalcState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_imagecalc.error", nullptr, nullptr);
    if( state->error == nullptr )
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
