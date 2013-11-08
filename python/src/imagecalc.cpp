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

static PyObject *ImageCalc_BandMath(PyObject *self, PyObject *args) {
    const char *pszOutputFile, *pszExpression, *pszGDALFormat;
    int nDataType;
    PyObject *pBandDefnObj;
    if( !PyArg_ParseTuple(args, "sssiO:bandMath", &pszOutputFile, &pszExpression, &pszGDALFormat,
                                        &nDataType, &pBandDefnObj))
        return NULL;

    // we made these values the same so should work
    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;

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
        rsgis::cmds::executeBandMaths(pRSGISStruct, nBandDefns, pszOutputFile, pszExpression, pszGDALFormat, type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    delete[] pRSGISStruct;

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ImageMath(PyObject *self, PyObject *args) {
    const char *pszInputImage, *pszOutputFile, *pszExpression, *pszGDALFormat;
    int nDataType;
    if( !PyArg_ParseTuple(args, "ssssi:bandMath", &pszInputImage, &pszOutputFile, &pszExpression,
                                &pszGDALFormat, &nDataType ))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;

    try
    {
        rsgis::cmds::executeImageMaths(pszInputImage, pszOutputFile, pszExpression, pszGDALFormat, type);
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
    unsigned int dataType, winSize;

    if(!PyArg_ParseTuple(args, "ssIsI:mahalanobisDistFilter", &inputImage, &outputImage, &winSize, &gdalFormat, &dataType))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)dataType;

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
    unsigned int dataType, winSize;

    if(!PyArg_ParseTuple(args, "ssIsI:mahalanobisDist2ImgFilter", &inputImage, &outputImage, &winSize, &gdalFormat, &dataType))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)dataType;

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
    unsigned int dataType;
    int useNoDataValue;
    float noDataValue;
    PyObject *summaryStats;
    
    if(!PyArg_ParseTuple(args, "ssOsIfi:imagePixelColumnSummary", &inputImage, &outputImage, &summaryStats, &gdalFormat, &dataType, &noDataValue, &useNoDataValue))
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

    PyObject *pMin = PyObject_GetAttrString(summaryStats, "min");
    if( ( pMin == NULL ) || ( pMin == Py_None ) || !RSGISPY_CHECK_FLOAT(pMin) ) {
        PyErr_SetString(GETSTATE(self)->error, "could not find float attribute \'min\'" );
        Py_XDECREF(pMin);
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
        PyErr_SetString(GETSTATE(self)->error, "could not find float attribute \'median\'" );
        Py_XDECREF(pMedian);
        Py_DECREF(pStdDev);
        Py_DECREF(pSum);
        Py_DECREF(pMean);
        Py_DECREF(pMax);
        Py_DECREF(pMin);
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

    summary.min = RSGISPY_FLOAT_EXTRACT(pMin);
    summary.max = RSGISPY_FLOAT_EXTRACT(pMax);
    summary.mean = RSGISPY_FLOAT_EXTRACT(pMean);
    summary.sum = RSGISPY_FLOAT_EXTRACT(pSum);
    summary.stdDev = RSGISPY_FLOAT_EXTRACT(pStdDev);
    summary.median = RSGISPY_FLOAT_EXTRACT(pMedian);
    
    Py_DECREF(pCalcMin);
    Py_DECREF(pCalcMax);
    Py_DECREF(pCalcMean);
    Py_DECREF(pCalcSum);
    Py_DECREF(pCalcStdDev);
    Py_DECREF(pCalcMedian);
    Py_DECREF(pMin);
    Py_DECREF(pMax);
    Py_DECREF(pMean);
    Py_DECREF(pSum);
    Py_DECREF(pStdDev);
    Py_DECREF(pMedian);

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)dataType;

    try {
        rsgis::cmds::executeImagePixelColumnSummary(inputImage, outputImage, summary, gdalFormat, type, noDataValue, useNoDataValue);
    } catch (rsgis::cmds::RSGISCmdException &e) {
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

    for(int i = 0; i < nImages; ++i) {
        PyObject *inImageObj = PySequence_GetItem(pInputImages, i);
        PyObject *outImageObj = PySequence_GetItem(pOutputImages, i);

        if(!RSGISPY_CHECK_STRING(inImageObj) || !RSGISPY_CHECK_STRING(outImageObj)) {
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

static PyObject *ImageCalc_Correlation(PyObject *self, PyObject *args) {
    const char *inputImageA, *inputImageB, *outputMatrix;

    if(!PyArg_ParseTuple(args, "sss:correlation", &inputImageA, &inputImageB, &outputMatrix))
        return NULL;

    try {
        rsgis::cmds::executeCorrelation(inputImageA, inputImageB, outputMatrix);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
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

static PyObject *ImageCalc_PCA(PyObject *self, PyObject *args) {
    const char *eigenVectors, *inputImage, *outputImage;
    unsigned int numComponents;

    if(!PyArg_ParseTuple(args, "sssI:pca", &inputImage, &eigenVectors, &outputImage, &numComponents))
        return NULL;

    try {
        rsgis::cmds::executePCA(eigenVectors, inputImage, outputImage, numComponents);
    } catch (rsgis::cmds::RSGISCmdException &e) {
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

static PyObject *ImageCalc_MovementSpeed(PyObject *self, PyObject *args) {
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
        return NULL;

    try {
        rsgis::cmds::executeCalculateRMSE(inputImageA, bandA, inputImageB, bandB);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
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

static PyObject *ImageCalc_UnconLinearSpecUnmix(PyObject *self, PyObject *args) {
    const char *inputImage, *imageFormat, *outputFile, *endmembersFile;
    float lsumGain, lsumOffset;
    int dataType;

    if(!PyArg_ParseTuple(args, "ssiffss:unconLinearSpecUnmix", &inputImage, &imageFormat, &dataType, &lsumGain, &lsumOffset, &outputFile, &endmembersFile))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)dataType;

    try {
        rsgis::cmds::executeUnconLinearSpecUnmix(inputImage, imageFormat, type, lsumGain, lsumOffset, outputFile, endmembersFile);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ExhconLinearSpecUnmix(PyObject *self, PyObject *args) {
    const char *inputImage, *imageFormat, *outputFile, *endmembersFile;
    float lsumGain, lsumOffset, stepResolution;
    int dataType;

    if(!PyArg_ParseTuple(args, "ssiffssf:exhconLinearSpecUnmix", &inputImage, &imageFormat, &dataType, &lsumGain, &lsumOffset, &outputFile, &endmembersFile, &stepResolution))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)dataType;

    try {
        rsgis::cmds::executeExhconLinearSpecUnmix(inputImage, imageFormat, type, lsumGain, lsumOffset, outputFile, endmembersFile, stepResolution);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_ConSum1LinearSpecUnmix(PyObject *self, PyObject *args) {
    const char *inputImage, *imageFormat, *outputFile, *endmembersFile;
    float lsumGain, lsumOffset, lsumWeight;
    int dataType;

    if(!PyArg_ParseTuple(args, "ssifffss:conSum1LinearSpecUnmix", &inputImage, &imageFormat, &dataType, &lsumGain, &lsumOffset, &lsumWeight, &outputFile, &endmembersFile))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)dataType;

    try {
        rsgis::cmds::executeConSum1LinearSpecUnmix(inputImage, imageFormat, type, lsumGain, lsumOffset, lsumWeight, outputFile, endmembersFile);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_NnConSum1LinearSpecUnmix(PyObject *self, PyObject *args) {
    const char *inputImage, *imageFormat, *outputFile, *endmembersFile;
    float lsumGain, lsumOffset, lsumWeight;
    int dataType;

    if(!PyArg_ParseTuple(args, "ssifffss:nnConSum1LinearSpecUnmix", &inputImage, &imageFormat, &dataType, &lsumGain, &lsumOffset, &lsumWeight, &outputFile, &endmembersFile))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)dataType;

    try {
        rsgis::cmds::executeNnConSum1LinearSpecUnmix(inputImage, imageFormat, type, lsumGain, lsumOffset, lsumWeight, outputFile, endmembersFile);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageCalc_AllBandsEqualTo(PyObject *self, PyObject *args) {
    const char *inputImage, *outputImage, *imageFormat;
    float imgValue, outputTrueVal, outputFalseVal;
    int dataType;

    if(!PyArg_ParseTuple(args, "sfffssi:allBandsEqualTo", &inputImage, &imgValue, &outputTrueVal, &outputFalseVal, &outputImage, &imageFormat, &dataType))
        return NULL;

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)dataType;

    try {
        rsgis::cmds::executeAllBandsEqualTo(inputImage, imgValue, outputTrueVal, outputFalseVal, outputImage, imageFormat, type);
    } catch (rsgis::cmds::RSGISCmdException &e) {
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
    //PyObject *val = Py_BuildValue("I", 42);
    try
    {
        unsigned int nBins = 0;
        double inMinVal = inMin;
        double inMaxVal = inMax;
        unsigned int *bins = rsgis::cmds::executeGetHistogram(inputImage, imgBand, binWidth, &nBins, calcInMinMax, &inMinVal, &inMaxVal);
        
        //std::cout << "nBins = " << nBins << std::endl;
        
        //std::cout << "inMinVal = " << inMinVal << std::endl;
        //std::cout << "inMaxVal = " << inMaxVal << std::endl;
        
        Py_ssize_t listLen = nBins;
        
        //std::cout << "listLen = " << listLen << std::endl;
        
        binsList = PyTuple_New(listLen);
        if(binsList == NULL)
        {
            delete[] bins;
            throw rsgis::cmds::RSGISCmdException("Could not create a python list...");
        }
        
        for(unsigned int i = 0; i < nBins; ++i)
        {
            //std::cout << i << " = " << bins[i] << std::endl;
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

static PyObject *ImageCalc_BandPercentile(PyObject *self, PyObject *args) {
    const char *inputImage, *outputFile;
    float percentile, noDataValue;
    int noDataValueSpecified;

    if(!PyArg_ParseTuple(args, "sffis:bandPercentile", &inputImage, &percentile, &noDataValue, &noDataValueSpecified, &outputFile))
        return NULL;

    try {
        rsgis::cmds::executeBandPercentile(inputImage, percentile, noDataValue, noDataValueSpecified, outputFile);
    } catch (rsgis::cmds::RSGISCmdException &e) {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
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


// Our list of functions in this module
static PyMethodDef ImageCalcMethods[] = {
    {"bandMath", ImageCalc_BandMath, METH_VARARGS,
"imagecalc.bandMath(outputImage, expression, gdalformat, gdaltype, bandDefnSeq)\n"
"Performs band math calculation.\n"
"where:\n"
"  * outputImage is a string containing the name of the output file\n"
"  * expression is a string containing the expression to run over the images, uses muparser syntax.\n"
"  * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  * gdaltype is an containing one of the values from rsgislib.TYPE_*\n"
"  * bandDefnSeq is a sequence of rsgislib.imagecalc.BandDefn objects that define the inputs\n"
"Example::\n"
"\n"
"   outputImage = './TestOutputs/PSU142_b1mb2.kea'\n"
"   format = 'KEA'\n"
"   dataType = rsgislib.TYPE_32FLOAT\n"
"   expression = 'b1*b2'\n"
"   bandDefns = []\n"
"   bandDefns.append(BandDefn('b1', inFileName, 1))\n"
"   bandDefns.append(BandDefn('b2', inFileName, 2))\n"
"   imagecalc.bandMath(outputImage, expression, format, dataType, bandDefns)\n"
"\n"},

    {"imageMath", ImageCalc_ImageMath, METH_VARARGS,
"imagecalc.imageMath(inputImage, outputImage, expression, gdalformat, gdaltype)\n"
"Performs image math calculation.\n"
"where:\n"
"  * inimage is a string containing the name of the input file\n"
"  * outputImage is a string containing the name of the output file\n"
"  * expression is a string containing the expression to run over the images, uses myparser syntax.\n"
"  * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  * gdaltype is an containing one of the values from rsgislib.TYPE_*\n"
"Example::\n"
"\n"
"   outputImage = path + 'TestOutputs/PSU142_multi1000.kea'\n"
"   format = 'KEA'\n"
"   dataType = rsgislib.TYPE_32UINT\n"
"   expression = 'b1*1000'\n"
"   imagecalc.imageMath(inFileName, outputImage, expression, format, dataType)\n"
"\n"},

    {"kMeansClustering", ImageCalc_KMeansClustering, METH_VARARGS,
"imagecalc.kMeansClustering(inputImage, outputMatrix, numClusters, maxIterations, subSample, ignoreZeros, degreeOfChange, initMethod)\n"
"Performs K Means Clustering and saves cluster centres to a text file.\n"
"where:\n"
"  * inputImage is a string providing the input image\n"
"  * outputMatrix is a string providing the output matrix (text file) to save the cluster centres to.\n"
"  * numClusters is the number of clusters to use.\n"
"  * maxIterations is the maximum number of itterations.\n"
"  * subSample is an int specifying what fraction of the total pixels should be considered (e.g., 100 = 1/100 pixels).\n"
"  * ignoreZeros is a bool specifying if zeros in the image should be treated as no data.\n"
"  * degreeofChange is a float providing the minimum change between itterations before terminating.\n"
"  * initMethod the method for initialising the clusters and is one of INITCLUSTER_* values\n"
"\n"
"Example::\n"
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
"imagecalc.isoDataClustering(inputImage, outputMatrix, numClusters, maxIterations, subSample, ignoreZeros, degreeOfChange, initMethod, minDistBetweenClusters, minNumFeatures, maxStdDev, minNumClusters, startIteration, endIteration)\n"
"Performs ISO Data Clustering and saves cluster centres to a text file.\n"
"where:\n"
"  * inputImage is a string providing the input image\n"
"  * outputMatrix is a string providing the output matrix (text file) to save the cluster centres to.\n"
"  * numClusters is the number of clusters to start with.\n"
"  * maxIterations is the maximum number of itterations.\n"
"  * subSample is an int specifying what fraction of the total pixels should be considered (e.g., 100 = 1/100 pixels).\n"
"  * ignoreZeros is a bool specifying if zeros in the image should be treated as no data.\n"
"  * initMethod the method for initialising the clusters and is one of INITCLUSTER_* values\n"
"  * minDistBetweenClusters is a float\n"
"  * minNumFeatures is an int\n"
"  * maxStdDev is a float\n"
"  * minNumClusters is an int\n"
"  * startIteration is an int\n"
"  * endIteration is an int\n"
"\n"
"Example::\n"
"   inputImage = path + 'Rasters/injune_p142_casi_sub_right_utm.kea'\n"
"   output = './TestOutputs/isocentres'\n"
"   imagecalc.isoDataClustering(inputImage, output, 10, 200, 1, True, 0.0025, rsgislib.INITCLUSTER_DIAGONAL_FULL_ATTACH, 2, 5, 5, 5, 8, 50)\n"
"\n"},

    {"mahalanobisDistFilter", ImageCalc_MahalanobisDistFilter, METH_VARARGS,
"imagecalc.mahalanobisDistFilter(inputImage, outputImage, windowSize, gdalFormat, gdalDataType)\n"
"Performs mahalanobis distance window filter.\n"
"where:\n"
"  * inputImage is a string containing the name of the input file\n"
"  * outputImage is a string containing the name of the output file\n"
"  * windowSize is an int defining the size of the window to be used\n"
"  * gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  * gdalDataType is an int containing one of the values from rsgislib.TYPE_*\n"
},

    {"mahalanobisDist2ImgFilter", ImageCalc_MahalanobisDist2ImgFilter, METH_VARARGS,
"imagecalc.mahalanobisDist2ImgFilter(inputImage, outputImage, windowSize, gdalFormat, gdalDataType)\n"
"Performs mahalanobis distance image to window filter.\n"
"where:\n"
"  * inputImage is a string containing the name of the input file\n"
"  * outputImage is a string containing the name of the output file\n"
"  * windowSize is an int defining the size of the window to be used\n"
"  * gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  * gdalDataType is an int containing one of the values from rsgislib.TYPE_*\n"
},

    {"imageCalcDistance", ImageCalc_ImageCalcDistance, METH_VARARGS,
"imagecalc.imageCalcDistance(inputImage, outputImage, gdalFormat)\n"
"Performs image calculate distance command.\n"
"where:\n"
"  * inputImage is a string containing the name of the input file\n"
"  * outputImage is a string containing the name of the output file\n"
"  * gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
},

    {"imagePixelColumnSummary", ImageCalc_ImagePixelColumnSummary, METH_VARARGS,
"imagecalc.imagePixelColumnSummary(inputImage, outputImage, summaryStats, gdalFormat, gdalDataType, noDataValue, useNoDataValue)\n"
"Calculates summary statistics for a column of pixels.\n"
"where:\n"
"  * inputImage is a string containing the name of the input file\n"
"  * outputImage is a string containing the name of the output file\n"
"  * summaryStats is an rsgislib.imagecalc.StatsSummary object that has attributes matching rsgis.cmds.RSGISCmdStatsSummary\n"
"    Requires: TODO: Check\n"
""
"      *  calcMin: boolean defining if the min value should be calculated\n"
"      *  calcMax: boolean defining if the max value should be calculated\n"
"      *  calcSum: boolean defining if the sum value should be calculated\n"
"      *  calcMean: boolean defining if the mean value should be calculated\n"
"      *  calcStdDev: boolean defining if the standard deviation should be calculated\n"
"      *  calcMedian: boolean defining if the median value should be calculated\n"
"      *  min: float defining the min value to use\n"
"      *  max: float defining the max value to use\n"
"      *  mean: float defining the mean value to use\n"
"      *  sum: float defining the sum value to use\n"
"      *  stdDev: float defining the standard deviation value to use\n"
"      *  median: float defining the median value to use\n"
""
"  * gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  * gdalDataType is an int containing one of the values from rsgislib.TYPE_*\n"
"  * noDataValue is a float specifying what value is used to signify no data\n"
"  * useNoDataValue is a boolean specifying whether the noDataValue should be used\n"
},

    {"imagePixelLinearFit", ImageCalc_ImagePixelLinearFit, METH_VARARGS,
"imagecalc.imagePixelLinearFit(inputImage, outputImage, gdalFormat, bandValues, noDataValue, useNoDataValue)\n"
"Performs a linear regression on each column of pixels.\n"
"where:\n"
"  * inputImage is a string containing the name of the input file\n"
"  * outputImage is a string containing the name of the output file\n"
"  * gdalFormat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  * bandValues is TODO\n"
"  * noDataValue is a float specifying what value is used to signify no data\n"
"  * useNoDataValue is a boolean specifying whether the noDataValue should be used\n"
},

    {"normalisation", ImageCalc_Normalisation, METH_VARARGS,
"imagecalc.normalisation(inputImages, outputImages, calcInMinMax, inMin, inMax, outMin, outMax)\n"
"Performs image normalisation\n"
"where:\n"
"  * inputImages is a sequence of strings containing the names of the input files\n"
"  * outputImages is a sequence of strings containing the names of the output files\n"
"  * calcInMinMax is a boolean specifying whether to calculate inMin and inMax values TODO: Check'\n"
"  * inMin is a float specifying the TODO\n"
"  * inMax is a float specifying the TODO\n"
"  * outMin is a float specifying the TODO\n"
"  * outMax is a float specifying the TODO\n"
},

    {"correlation", ImageCalc_Correlation, METH_VARARGS,
"imagecalc.correlation(inputImageA, inputImageB, outputMatrix)\n"
"Calculates the correlation between two images\n"
"where:\n"
"  * inputImageA is a string containing the name of the first input image file\n"
"  * inputImageB is a string containing the name of the second input image file\n"
"  * outputMatrix is a string containing the name of the output matrix\n"
},

    {"covariance", ImageCalc_Covariance, METH_VARARGS,
"imagecalc.covariance(inputImageA, inputImageB, inputMatrixA, inputMatrixB, shouldCalcMean, outputMatrix)\n"
"Calculates the covariance between two images\n"
"where:\n"
"  * inputImageA is a string containing the name of the first input image file\n"
"  * inputImageB is a string containing the name of the second input image file\n"
"  * inputMatrixA is a string containing the name of the first input matrix file\n"
"  * inputMatrixB is a string containing the name of the second input matrix file\n"
"  * shouldCalcMean is a boolean defining whether the mean should be calculated TODO: check\n"
"  * outputMatrix is a string containing the name of the output matrix\n"
},

    {"meanVector", ImageCalc_MeanVector, METH_VARARGS,
"imagecalc.meanVector(inputImage, outputMatrix)\n"
"Calculates the mean vector of an image\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * outputMatrix is a string containing the name of the output matrix\n"
},


    {"pca", ImageCalc_PCA, METH_VARARGS,
"imagecalc.pca(eigenVectors, inputImage, outputImage, numComponents)\n"
"Performs a principal components analysis of an image\n"
"where:\n"
"  * eigenVectors is a string containing the name of the file of eigen vectors for the PCA\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * outputImage is a string containing the name of the output image file\n"
"  * numComponents is an int containing number of components to use for PCA\n"
},

    {"standardise", ImageCalc_Standardise, METH_VARARGS,
"imagecalc.standardise(meanVector, inputImage, outputImage)\n"
"Generates a standardised image using the mean vector provided\n"
"where:\n"
"  * meanVector is a string containing the name of the file containing the mean vector TODO: check\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * outputImage is a string containing the name of the output image file\n"
},

    {"replaceValuesLessThan", ImageCalc_ReplaceValuesLessThan, METH_VARARGS,
"imagecalc.replaceValuesLessThan(inputImage, outputImage, threshold, value)\n"
"Replaces values in an image that are less than the provided, according to the provided threshold\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * outputImage is a string containing the name of the output image file\n"
"  * threshold is a float containing the threshold to use\n"
"  * value is a float containing the value below which replacement will occur\n"
},

    {"unitArea", ImageCalc_UnitArea, METH_VARARGS,
"imagecalc.unitArea(inputImage, outputImage, inputMatrixFile)\n"
"Converts the image spectra to unit area\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * outputImage is a string containing the name of the output image file\n"
"  * inputMatrixFile is a string containing the name of the input matrix file TODO: check\n"
},

    {"movementSpeed", ImageCalc_MovementSpeed, METH_VARARGS,
"imagecalc.movementSpeed(inputImages, imageBands, imageTimes, upper, lower, outputImage)\n"
"Calculates the speed of movement in images (mean, min and max)\n"
"where:\n"
"  * inputImages is a python sequence of strings of the input image files\n"
"  * imageBands is a python sequence of integers defining the band of each image to use\n"
"  * imageTimes is a python sequence of floats defining the time corresponding to each image\n"
"  * upper is a float TODO: expand\n"
"  * lower is a float TODO: expand\n"
"  * outputImage is a string defining the output image file name\n"
},

    {"countValsInCols", ImageCalc_CountValsInCols, METH_VARARGS,
"imagecalc.countValsInCols(inputImage, upper, lower, outputImage)\n"
"Counts the number of values within a given range for each column\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * upper is a float containing the upper limit of values to count\n"
"  * lower is a float containing the lower limit of values to count\n"
"  * outputImage is a string containing the name of the output image file\n"
},

    {"calculateRMSE", ImageCalc_CalculateRMSE, METH_VARARGS,
"imagecalc.calculateRMSE(inputImageA, inputBandA, inputImageB, inputBandB)\n"
"Calculates the root mean squared error between two images\n"
"where:\n"
"  * inputImageA is a string containing the name of the first input image file\n"
"  * inputBandA is an integer defining which band should be processed from inputImageA\n"
"  * inputImageB is a string containing the name of the second input image file\n"
"  * inputBandB is an integer defining which band should be processed from inputImageB\n"
},

    {"dist2Geoms", ImageCalc_Dist2Geoms, METH_VARARGS,
"imagecalc.dist2Geoms(inputVector, imageResolution, outputImage)\n"
"Calculates the distance to the nearest geometry for every pixel in an image\n"
"where:\n"
"  * inputVector is a string defining the geometry TODO: Check this and find out about input image\n"
"  * imageResolution is a float defining the resolution of which to process the image\n"
"  * outputImage is a string containing the name of the file for the output image\n"
},

    {"imageBandStats", ImageCalc_ImageBandStats, METH_VARARGS,
"imagecalc.imageBandStats(inputImage, outputFile, ignoreZeros)\n"
"Calculates statistics for individuals bands of an image\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * outputFile is a string containing the name of the output file\n"
"  * ignoreZeros is a boolean defining whether zeros are ignored in the statistic calculation\n"
},

    {"imageStats", ImageCalc_ImageStats, METH_VARARGS,
"imagecalc.imageStats(inputImage, outputFile, ignoreZeros)\n"
"Calculates statistics for an image across all bands\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * outputFile is a string containing the name of the output file\n"
"  * ignoreZeros is a boolean defining whether zeros are ignored in the statistic calculation\n"
},

    {"unconLinearSpecUnmix", ImageCalc_UnconLinearSpecUnmix, METH_VARARGS,
"imagecalc.unconLinearSpecUnmix(inputImage, gdalformat, gdaltype, lsumGain, lsumOffset, outputFile, endmembersFile)\n"
"Performs unconstrained linear spectral unmixing of the input image for a set of endmembers\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  * gdaltype is an containing one of the values from rsgislib.TYPE_*\n"
"  * lsumGain is a float TODO: Complete here and below\n"
"  * lsumOffset is a float\n"
"  * outputFile is a string containing the name of the output file\n"
"  * endmembersFile is a string containing the names of the file containing the end members\n"
},

    {"exhconLinearSpecUnmix", ImageCalc_ExhconLinearSpecUnmix, METH_VARARGS,
"imagecalc.exhconLinearSpecUnmix(inputImage, gdalformat, gdaltype, lsumGain, lsumOffset, outputFile, endmembersFile, stepResolution)\n"
"Performs an exhaustive constrained linear spectral unmixing of the input image for a set of endmembers\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  * gdaltype is an containing one of the values from rsgislib.TYPE_*\n"
"  * lsumGain is a float TODO: Complete here and below\n"
"  * lsumOffset is a float\n"
"  * outputFile is a string containing the name of the output file\n"
"  * endmembersFile is a string containing the names of the file containing the end members\n"
"  * stepResolution is a float\n"
},

    {"conSum1LinearSpecUnmix", ImageCalc_ConSum1LinearSpecUnmix, METH_VARARGS,
"imagecalc.conSum1LinearSpecUnmix(inputImage, gdalformat, gdaltype, lsumGain, lsumOffset, lsumWeight, outputFile, endmembersFile, stepResolution)\n"
"Performs a partially constrained linear spectral unmixing of the input image for a set of endmembers where the sum of the unmixing will be approximately 1\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  * gdaltype is an containing one of the values from rsgislib.TYPE_*\n"
"  * lsumGain is a float TODO: Complete here and below\n"
"  * lsumOffset is a float\n"
"  * lsumWeight is a float\n"
"  * outputFile is a string containing the name of the output file\n"
"  * endmembersFile is a string containing the names of the file containing the end members\n"
},

    {"nnConSum1LinearSpecUnmix", ImageCalc_NnConSum1LinearSpecUnmix, METH_VARARGS,
"imagecalc.nnConSum1LinearSpecUnmix(inputImage, gdalformat, gdaltype, lsumGain, lsumOffset, lsumWeight, outputFile, endmembersFile, stepResolution)\n"
"Performs a constrained linear spectral unmixing of the input image for a set of endmembers where the sum of the unmixing will be approximately 1 and non-negative\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  * gdaltype is an containing one of the values from rsgislib.TYPE_*\n"
"  * lsumGain is a float TODO: Complete here and below\n"
"  * lsumOffset is a float\n"
"  * lsumWeight is a float\n"
"  * outputFile is a string containing the name of the output file\n"
"  * endmembersFile is a string containing the names of the file containing the end members\n"
},

    {"allBandsEqualTo", ImageCalc_AllBandsEqualTo, METH_VARARGS,
"imagecalc.allBandsEqualTo(inputImage, imgValue, outputTrueVal, outputFalseVal, outputImage, gdalformat, gdaltype)\n"
"Tests whether all bands are equal to the same value\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * imgValue is a float specifying the value against which others are tested for equality TODO: Check this and below\n"
"  * ouputTrueVal is a float specifying the value in the output image representing true \n"
"  * outputFalseVal is a float specifying the value in the output image representing false \n"
"  * outputImage is a string containing the name of the output image file\n"
"  * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  * gdaltype is an containing one of the values from rsgislib.TYPE_*\n"
},

    {"histogram", ImageCalc_Histogram, METH_VARARGS,
"imagecalc.histogram(inputImage, imageMask, outputFile, imgBand, imgValue, binWidth, calcInMinMax, inMin, inMax)\n"
"Generates a histogram for the region of the mask selected\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * imageMask is a string containing the name of the image mask file\n"
"  * outputFile is a string containing the name of the file for histogram output\n"
"  * imgValue is a float\n"
"  * binWidth is a float specifying the width of the histogram bins\n"
"  * calcInMinMax is a boolean specifying whether inMin and inMax should be calculated\n"
"  * inMin is a float for the minimum image value to be included in the histogram\n"
"  * inMax is a floatf or the maximum image value to be included in the histogram\n"
},
    
{"getHistogram", ImageCalc_GetHistogram, METH_VARARGS,
"imagecalc.getHistogram(inputImage, imgBand, binWidth, calcInMinMax, inMin, inMax)\n"
"Generates and returns a histogram for the image.\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * imgBand is an unsigned int specifying the image band starting from 1.\n"
"  * binWidth is a float specifying the width of the histogram bins\n"
"  * calcInMinMax is a boolean specifying whether inMin and inMax should be calculated\n"
"  * inMin is a float for the minimum image value to be included in the histogram\n"
"  * inMax is a floatf or the maximum image value to be included in the histogram\n"
"returns:\n"
"  * "
},

    {"bandPercentile", ImageCalc_BandPercentile, METH_VARARGS,
"imagecalc.bandPercentile(inputImage, percentile, noDataValue, noDataValueSpecified, outputFile)\n"
"Calculates image band percentiles\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * percentile is a float TODO: more info\n"
"  * noDataValue is a float specifying the value used to represent no data\n"
"  * noDataValueSpecified is a boolean specifying whether to use the previous noDataValue parameter\n"
"  * outputFile is a string containing the name of the file for histogram output\n"
},

    {"imageDist2Geoms", ImageCalc_ImageDist2Geoms, METH_VARARGS,
"imagecalc.imageDist2Geoms(inputImage, inputVector, gdalformat, outputImage)\n"
"Calculates the distance to the nearest geometry for every pixel in an image\n"
"where:\n"
"  * inputImage is a string containing the name of the input image file\n"
"  * inputVector is a string containing the name of the input vector file containing the geometry TODO: check this\n"
"  * gdalformat is a string containing the GDAL format for the output file - eg 'KEA'\n"
"  * outputImage is a string containing the name of the output image file\n"
},
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
