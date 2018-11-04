/*
 *  vectorutils.cpp
 *  RSGIS_LIB
 *
 *  Created by Dan Clewley on 08/11/2013.
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
#include "cmds/RSGISCmdVectorUtils.h"
#include <vector>

/* An exception object for this module */
/* created in the init function */
struct VectorUtilsState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct VectorUtilsState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct VectorUtilsState _state;
#endif

static std::vector<std::string> ExtractStringVectorFromSequence(PyObject *sequence, int *nElements) {
    Py_ssize_t nFields = PySequence_Size(sequence);
    *nElements = nFields;
    std::vector<std::string> stringsArray;
    stringsArray.reserve(*nElements);
    
    for(int i = 0; i < nFields; ++i)
    {
        PyObject *stringObj = PySequence_GetItem(sequence, i);
        
        if(!RSGISPY_CHECK_STRING(stringObj)) {
            PyErr_SetString(GETSTATE(sequence)->error, "Fields must be strings");
            Py_DECREF(stringObj);
            return stringsArray;
        }
        
        stringsArray.push_back(RSGISPY_STRING_EXTRACT(stringObj));
        Py_DECREF(stringObj);
    }
    
    return stringsArray;
}

static PyObject *VectorUtils_GenerateConvexHullsGroups(PyObject *self, PyObject *args)
{
    const char *pszInputFile, *pszOutputVector, *pszOutVecProj;
    int force, eastingsColIdx, northingsColIdx, attributeColIdx;
    if( !PyArg_ParseTuple(args, "sssiiii:generateConvexHullsGroups", &pszInputFile, &pszOutputVector, &pszOutVecProj, 
                                &force, &eastingsColIdx, &northingsColIdx, &attributeColIdx))
        return NULL;

    try
    {
        rsgis::cmds::executeGenerateConvexHullsGroups(pszInputFile, pszOutputVector, pszOutVecProj, force, 
                eastingsColIdx, northingsColIdx, attributeColIdx);
     
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *VectorUtils_RemoveAttributes(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszOutputVector;
    int force = false;
    if( !PyArg_ParseTuple(args, "ss|i:removeattributes", &pszInputVector, &pszOutputVector, &force))
        return NULL;

    try
    {
        rsgis::cmds::executeRemoveAttributes(pszInputVector, pszOutputVector, force);
     
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *VectorUtils_PrintPolyGeom(PyObject *self, PyObject *args)
{
    const char *pszInputVector;
    if( !PyArg_ParseTuple(args, "s:printpolygeom", &pszInputVector))
        return NULL;

    try
    {
        rsgis::cmds::executePrintPolyGeom(pszInputVector);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *VectorUtils_BufferVector(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszOutputVector;
    float bufferDist;
    int force = false;
    if( !PyArg_ParseTuple(args, "ssf|i:buffervector", &pszInputVector, &pszOutputVector, &bufferDist, &force))
        return NULL;

    try
    {
        rsgis::cmds::executeBufferVector(pszInputVector, pszOutputVector, bufferDist, force);
     
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *VectorUtils_FindReplaceText(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszAttribute, *pszFind, *pszReplace;
    if( !PyArg_ParseTuple(args, "ssss:findreplacetext", &pszInputVector, &pszAttribute, &pszFind, &pszReplace))
        return NULL;

    try
    {
        rsgis::cmds::executeFindReplaceText(pszInputVector, pszAttribute, pszFind, pszReplace);
     
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *VectorUtils_CalcArea(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszOutputVector;
    int force = false;
    if( !PyArg_ParseTuple(args, "ss|i:calcarea", &pszInputVector, &pszOutputVector, &force))
        return NULL;

    try
    {
        rsgis::cmds::executeCalcPolyArea(pszInputVector, pszOutputVector, force);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *VectorUtils_PolygonsInPolygon(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszInputCoverVector, *pszOutputDIR, *pszAttributeName;
    int force = false;
    if( !PyArg_ParseTuple(args, "ssss|i:polygonsInPolygon", &pszInputVector, &pszInputCoverVector, &pszOutputDIR, &pszAttributeName, &force))
        return NULL;

    try
    {
        rsgis::cmds::executePolygonsInPolygon(pszInputVector, pszInputCoverVector, pszOutputDIR, pszAttributeName, force);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *VectorUtils_PopulateGeomZField(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszInputImage, *pszOutputVector;
    int force = false;
    unsigned int imgBand;
    if( !PyArg_ParseTuple(args, "ssIs|i:populateGeomZField", &pszInputVector, &pszInputImage, &imgBand, &pszOutputVector, &force))
        return NULL;
    
    try
    {
        rsgis::cmds::executePopulateGeomZField(pszInputVector, pszInputImage, imgBand, pszOutputVector, force);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *VectorUtils_VectorMaths(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszOutputVector, *pszExpression, *pszOutColName;
    int force = false;
    PyObject *pVarsObj;
    if( !PyArg_ParseTuple(args, "ssssO|i:vectorMaths", &pszInputVector, &pszOutputVector, &pszOutColName, &pszExpression, &pVarsObj, &force))
    {
        return NULL;
    }
    
    if( !PySequence_Check(pVarsObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "variables argument must be a sequence");
        return NULL;
    }
    
    Py_ssize_t nVarsDefns = PySequence_Size(pVarsObj);
    unsigned int numVars = nVarsDefns;
    std::vector<rsgis::cmds::RSGISVariableFieldCmds> vars = std::vector<rsgis::cmds::RSGISVariableFieldCmds>();
    vars.reserve(numVars);
    
    for( Py_ssize_t n = 0; n < nVarsDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pVarsObj, n);
        
        PyObject *pName = PyObject_GetAttrString(o, "name");
        if( ( pName == NULL ) || ( pName == Py_None ) || !RSGISPY_CHECK_STRING(pName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'name\'" );
            Py_DECREF(pName);
            Py_DECREF(o);
            return NULL;
        }
        
        PyObject *pFieldName = PyObject_GetAttrString(o, "fieldName");
        if( ( pFieldName == NULL ) || ( pFieldName == Py_None ) || !RSGISPY_CHECK_STRING(pFieldName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'fieldName\'" );
            Py_DECREF(pName);
            Py_DECREF(pFieldName);
            Py_DECREF(o);
            return NULL;
        }
        
        rsgis::cmds::RSGISVariableFieldCmds var;
        var.name = RSGISPY_STRING_EXTRACT(pName);
        var.fieldName = RSGISPY_STRING_EXTRACT(pFieldName);
        
        vars.push_back(var);
        
        Py_DECREF(pName);
        Py_DECREF(pFieldName);
        Py_DECREF(o);
    }
    
    try
    {
        rsgis::cmds::executeVectorMaths(std::string(pszInputVector), std::string(pszOutputVector), std::string(pszOutColName), std::string(pszExpression), force, vars);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *VectorUtils_AddFIDColumn(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszOutputVector;
    int force = false;
    if( !PyArg_ParseTuple(args, "ss|i:addFIDColumn", &pszInputVector, &pszOutputVector, &force))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeAddFIDColumn(std::string(pszInputVector), std::string(pszOutputVector), force);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *VectorUtils_FindCommonImgExtent(PyObject *self, PyObject *args)
{
    PyObject *pInputImages;
    const char *pszOutputVector;
    int force = false;
    if( !PyArg_ParseTuple(args, "Os|i:findCommonImgExtent", &pInputImages, &pszOutputVector, &force))
    {
        return NULL;
    }
    
    // Extract list of images to array of strings.
    int numImages = 0;
    std::vector<std::string> inputImages = ExtractStringVectorFromSequence(pInputImages, &numImages);
    if(numImages == 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "No input images provided");
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeFindCommonImgExtent(inputImages, std::string(pszOutputVector), force);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *VectorUtils_SplitFeatures(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszOutputVectorBase;
    int force = false;
    if( !PyArg_ParseTuple(args, "ss|i:splitFeatures", &pszInputVector, &pszOutputVectorBase, &force))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeSplitFeatures(std::string(pszInputVector), std::string(pszOutputVectorBase), ((bool)force));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *VectorUtils_ExportPxls2Pts(PyObject *self, PyObject *args)
{
    const char *pszInputImg, *pszOutputVector;
    int force = false;
    float maskVal = 0.0;
    if( !PyArg_ParseTuple(args, "ssf|i:exportPxls2Pts", &pszInputImg, &pszOutputVector, &maskVal, &force))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeExportPxls2Pts(pszInputImg, pszOutputVector, force, maskVal);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *VectorUtils_Dist2NearestGeom(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszOutputVector, *pszOutColName;
    int forceInt = false;
    int useIdxInt = false;
    double maxSearchDist = 100;
    
    if( !PyArg_ParseTuple(args, "sss|iid:dist2NearestGeom", &pszInputVector, &pszOutputVector, &pszOutColName, &forceInt, &useIdxInt, &maxSearchDist))
    {
        return NULL;
    }
    
    PyObject *outVal = PyTuple_New(1);
    try
    {
        bool force = (bool) forceInt;
        bool useIdx = (bool) useIdxInt;
        double dist = rsgis::cmds::executeCalcDist2NearestGeom(std::string(pszInputVector), std::string(pszOutputVector), std::string(pszOutColName), force, useIdx, maxSearchDist);
        if(PyTuple_SetItem(outVal, 0, Py_BuildValue("d", dist)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'distance\' value to the list...");
        }
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    return outVal;
}

static PyObject *VectorUtils_Dist2NearestSecGeomSet(PyObject *self, PyObject *args)
{
    const char *pszInputVector, *pszInDistToVector, *pszOutputVector, *pszOutColName;
    int forceInt = false;
    int useIdxInt = false;
    double maxSearchDist = 100;
    
    if( !PyArg_ParseTuple(args, "ssss|iid:dist2NearestSecGeomSet", &pszInputVector, &pszInDistToVector, &pszOutputVector, &pszOutColName, &forceInt, &useIdxInt, &maxSearchDist))
    {
        return NULL;
    }
    
    PyObject *outVal = PyTuple_New(1);
    try
    {
        bool force = (bool) forceInt;
        bool useIdx = (bool) useIdxInt;
        double dist = rsgis::cmds::executeCalcDist2NearestGeom(std::string(pszInputVector), std::string(pszInDistToVector), std::string(pszOutputVector), std::string(pszOutColName), force, useIdx, maxSearchDist);
        if(PyTuple_SetItem(outVal, 0, Py_BuildValue("d", dist)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'distance\' value to the list...");
        }
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    return outVal;
}

static PyObject *VectorUtils_CalcMaxDist2NearestGeom(PyObject *self, PyObject *args)
{
    const char *pszInputVector;
    
    if( !PyArg_ParseTuple(args, "s:calcMaxDist2NearestGeom", &pszInputVector))
    {
        return NULL;
    }
    
    PyObject *outVal = PyTuple_New(1);
    try
    {
        double dist = rsgis::cmds::executeCalcMaxDist2NearestGeom(std::string(pszInputVector));
        if(PyTuple_SetItem(outVal, 0, Py_BuildValue("d", dist)) == -1)
        {
            throw rsgis::cmds::RSGISCmdException("Failed to add \'distance\' value to the list...");
        }

    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    return outVal;
}

static PyObject *VectorUtils_SpatialGraphClusterGeoms(PyObject *self, PyObject *args)
{
    const char *pszInputVector;
    const char *pszOutputVector;
    float sdEdgeLen = 0.0;
    double maxEdgeLen = 0.0;
    int force = false;
    int useMinSpanTree = true;
    PyObject *pszOutShpEdgesObj;
    PyObject *pszOutH5EdgeLensObj;
    
    if( !PyArg_ParseTuple(args, "ssifd|iOO:spatialGraphClusterGeoms", &pszInputVector, &pszOutputVector, &useMinSpanTree, &sdEdgeLen, &maxEdgeLen, &force, &pszOutShpEdgesObj, &pszOutH5EdgeLensObj))
    {
        return NULL;
    }
    
    std::string shpFileEdges="";
    bool outShpEdges=false;
    std::string h5EdgeLengths="";
    bool outH5EdgeLens=false;
    
    if( ( pszOutShpEdgesObj == NULL ) || ( pszOutShpEdgesObj == Py_None ) || !RSGISPY_CHECK_STRING(pszOutShpEdgesObj) )
    {
        outShpEdges = false;
    }
    else
    {
        outShpEdges = true;
        shpFileEdges = RSGISPY_STRING_EXTRACT(pszOutShpEdgesObj);
    }
    
    if( ( pszOutH5EdgeLensObj == NULL ) || ( pszOutH5EdgeLensObj == Py_None ) || !RSGISPY_CHECK_STRING(pszOutH5EdgeLensObj) )
    {
        outH5EdgeLens = false;
    }
    else
    {
        outH5EdgeLens = true;
        h5EdgeLengths = RSGISPY_STRING_EXTRACT(pszOutH5EdgeLensObj);
    }
    
    try
    {
        rsgis::cmds::executeSpatialGraphClusterGeoms(std::string(pszInputVector), std::string(pszOutputVector), useMinSpanTree, sdEdgeLen, maxEdgeLen, force, shpFileEdges, outShpEdges, h5EdgeLengths, outH5EdgeLens);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *VectorUtils_FitPolygon2Points(PyObject *self, PyObject *args)
{
    const char *pszInputVector;
    const char *pszOutputVector;
    int force = false;
    double alphaVal = -1.0;
    
    if( !PyArg_ParseTuple(args, "ss|di:fitPolygon2Points", &pszInputVector, &pszOutputVector, &alphaVal, &force))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeFitPolygonToPoints(std::string(pszInputVector), std::string(pszOutputVector), alphaVal, force);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *VectorUtils_FitPolygons2PointClusters(PyObject *self, PyObject *args)
{
    const char *pszInputVector;
    const char *pszOutputVector;
    const char *clustersField;
    int force = false;
    double alphaVal = -1.0;
    
    if( !PyArg_ParseTuple(args, "sss|di:fitPolygons2PointClusters", &pszInputVector, &pszOutputVector, &clustersField, &alphaVal, &force))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeFitPolygonsToPointClusters(std::string(pszInputVector), std::string(pszOutputVector), std::string(clustersField), alphaVal, force);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *VectorUtils_CreateLinesOfPoints(PyObject *self, PyObject *args)
{
    const char *pszInputVector;
    const char *pszOutputVector;
    double step;
    int force = false;
    
    if( !PyArg_ParseTuple(args, "ssd|i:createLinesOfPoints", &pszInputVector, &pszOutputVector, &step, &force))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeCreateLinesOfPoints(std::string(pszInputVector), std::string(pszOutputVector), step, force);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *VectorUtils_FitActiveContourBoundaries(PyObject *self, PyObject *args)
{
    const char *pszInputVector;
    const char *pszOutputVector;
    const char *pszExterForceImg;
    int force = false;
    double alphaVal;
    double betaVal;
    double gammaVal;
    double minExtThresVal;
    
    if( !PyArg_ParseTuple(args, "sssdddd|i:fitActiveContourBoundaries", &pszInputVector, &pszOutputVector, &pszExterForceImg, &alphaVal, &betaVal, &gammaVal, &minExtThresVal, &force))
    {
        return NULL;
    }
    
    try
    {
        rsgis::cmds::executeFitActiveContourBoundaries(std::string(pszInputVector), std::string(pszOutputVector), std::string(pszExterForceImg), alphaVal, betaVal, gammaVal, minExtThresVal, bool(force));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef VectorUtilsMethods[] = {

{"generateConvexHullsGroups", VectorUtils_GenerateConvexHullsGroups, METH_VARARGS,
"vectorutils.generateConvexHullsGroups(inputfile, outputvector, outVecProj, force, eastingsColIdx, northingsColIdx, attributeColIdx)\n"
"A command to produce convex hulls for groups of (X, Y, Attribute) point locations.\n\n"
"Where:\n"
"\n"
"* inputfile is a string containing the name of the input file\n"
"* outputvector is a string containing the name of the output vector\n"
"* outVecProj is a string specifying the projection of the output vector\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"* eastingsColIdx an integer specifying the easting column in the input text file \n"
"* northingsColIdx an integer specifying the northing column in the input text file \n"
"* attributeColIdx an integer specifying the attribute column in the input text file \n"
"\n"},

{"removeattributes", VectorUtils_RemoveAttributes, METH_VARARGS, 
"vectorutils.removeattributes(inputvector, outputvector, force)\n"
"A command to copy the geometry, dropping attributes.\n\n"
"Where:\n"
"\n"
"* inputvector is a string containing the name of the input vector\n"
"* outputvector is a string containing the name of the output vector\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import vectorutils\n"
"   inputVector = './Vectors/injune_p142_stem_locations.shp'\n"
"   outputVector = './TestOutputs/injune_p142_stem_locations_noatts.shp'\n"
"   vectorutils.removeattributes(inputVector, outputVector, True)\n"
"\n"},

{"buffervector", VectorUtils_BufferVector, METH_VARARGS, 
"vectorutils.buffervector(inputvector, outputvector, bufferDist, force)\n"
"A command to buffer a vector by a specified distance.\n\n"
"Where:\n"
"\n"
"* inputvector is a string containing the name of the input vector\n"
"* outputvector is a string containing the name of the output vector\n"
"* bufferDist is a float specifying the distance of the buffer, in map units.\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import vectorutils\n"
"   inputVector = './Vectors/injune_p142_stem_locations.shp'\n"
"   outputVector = './TestOutputs/injune_p142_stem_locations_1mbuffer.shp'\n"
"   bufferDist = 1\n"
"   vectorutils.buffervector(inputVector, outputVector, bufferDist, True)\n"
"\n"},

{"printpolygeom", VectorUtils_PrintPolyGeom, METH_VARARGS, 
"vectorutils.printpolygeom(inputvector)\n"
"A command to print the polygon geometries (to the console) of the inputted shapefile\n\n"
"Where:\n"
"\n"
"* inputvector is a string containing the name of the input vector\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import vectorutils\n"
"   inputVector = './Vectors/injune_p142_psu_utm.shp'\n"
"   vectorutils.printpolygeom(inputVector)\n"
"\n"},

{"findreplacetext", VectorUtils_FindReplaceText, METH_VARARGS,
"vectorutils.findreplacetext(inputvector, attribute, find, replace)\n"
"A command to undertake find and replace on a given attribute with the shapefile\n\n"
"Where:\n"
"\n"
"* inputvector is a string containing the name of the input vector.\n"
"* attribute is a string containing the name of field in the attribute table.\n"
"* find is a string to find.\n"
"* replace is a string to replace 'find'.\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import vectorutils\n"
"   inputVector = './TestOutputs/injune_p142_psu_utm_findreplace.shp'\n"
"   attribute = 'PSU'\n"
"   find = '142'\n"
"   replace = '142'\n"
"   vectorutils.findreplacetext(inputVector, attribute, find, replace)\n"
"\n"},

{"calcarea", VectorUtils_CalcArea, METH_VARARGS, 
"vectorutils.calcarea(inputvector, outputvector, force)\n"
"A command to add the area of each polygon to the attribute table, area in the same units \n"
"as the input dataset (likely m^2 or degrees^2).\n\n"
"Where:\n"
"\n"
"* inputvector is a string containing the name of the input vector\n"
"* outputvector is a string containing the name of the output vector\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import vectorutils\n"
"   inputVector = './Vectors/injune_p142_psu_utm.shp'\n"
"   outputVector = './TestOutputs/injune_p142_psu_utm_area.shp'\n"
"   vectorutils.calcarea(inputVector, outputVector, True)\n"
"\n"},

{"polygonsInPolygon", VectorUtils_PolygonsInPolygon, METH_VARARGS, 
"vectorutils.polygonsInPolygon(inputvector, inputcovervector, outputDIR, attributeName, force)\n"
"A command to create a new polygon containing only polygons within cover vector. \n"
"Loops through attributes and creates a new shapefile for each polygon in the cover vector.\n\n"
"Where:\n"
"\n"
"* inputvector is a string containing the name of the input vector\n"
"* inputcovervector is a string containing the name of the cover vector vector\n"
"* outputDIR is a string containing the name of the output directory\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import vectorutils\n"
"   inputVector = './Vectors/injune_p142_stem_locations.shp'\n"
"   coverVector = './Vectors/injune_p142_psu_utm.shp'\n"
"   outDIR = '/TestOutputs'\n"
"   attribute = 'PSU'\n"
"   vectorutils.polygonsInPolygon(inputVector, coverVector, outDIR, attribute, True)\n"
"\n"},

{"populateGeomZField", VectorUtils_PopulateGeomZField, METH_VARARGS,
"vectorutils.populateGeomZField(InputVector, InputImage, imgBand, OutputVector, force)\n"
"A command to populate the z field within the vector file making it a 3D vector rather \n"
"than just a 2d file.\n\n"
"Where:\n"
"\n"
"* InputVector is a string containing the name of the input vector\n"
"* InputImage is a string containing the name of the image (DEM) image\n"
"* imgBand is an unsigned int specifying the image band in the image file to be used (note image bands indexes start at 1)\n"
"* OutputVector is a string containing the name of the output vector file\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"
"Example::\n"
"\n"
"    import rsgislib.vectorutils\n"
"    inputVector = './Polys2D.shp'\n"
"    inputImage = './SRTM_90m.kea'\n"
"    imgBand = 1\n"
"    outputVector = './Polys3D.shp'\n"
"    force = True\n"
"    rsgislib.vectorutils.populateGeomZField(inputVector, inputImage, imgBand, outputVector, force)\n"
"\n"},

{"vectorMaths", VectorUtils_VectorMaths, METH_VARARGS,
"vectorutils.vectorMaths(inputVector, outputVector, outputColName, expression, variables, force)\n"
"A command to calculate a number column from data in existing columns.\n\n"
"Where:\n"
"\n"
"* inputVector is a string containing the name of the input vector\n"
"* outputVector is a string containing the name of the output vector file\n"
"* outputColName is a string containing the name of the output column\n"
"* expression is a string containing the muparser expression to be calculated.\n"
"* variables is a list defining the names of the variables used within the expression and defining which columns they are in the inputVector. The must be a list and contain two fields \'name\' and \'fieldName\'.\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"},

{"addFIDColumn", VectorUtils_AddFIDColumn, METH_VARARGS,
"vectorutils.addFIDColumn(inputvector, outputvector, force)\n"
"A command to add an explicit FID column to a copy of a shapefile.\n\n"
"Where:\n"
"\n"
"* inputvector is a string containing the name of the input vector\n"
"* outputvector is a string containing the name of the output vector\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import vectorutils\n"
"   inputVector = './Vectors/injune_p142_psu_utm.shp'\n"
"   outputVector = './TestOutputs/injune_p142_psu_utm_fid.shp'\n"
"   vectorutils.addFIDColumn(inputVector, outputVector, True)\n"
"\n"},

{"findCommonImgExtent", VectorUtils_FindCommonImgExtent, METH_VARARGS,
"vectorutils.findCommonImgExtent(inputImages, outputvector, force)\n"
"A command to create a shapefile representing the region of common extent\n"
"for the list of input images.\n\n"
"Where:\n"
"\n"
"* inputImages is a list of strings containing the names of the input image files\n"
"* outputvector is a string containing the name of the output vector\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import vectorutils\n"
"   inputImages = ['img1.kea', 'img2.kea', 'img3.kea', 'img4.kea', 'img5.kea']\n"
"   outputVector = 'imgSubExtent.shp'\n"
"   vectorutils.findCommonImgExtent(inputImages, outputVector, True)\n"
"\n"},

{"splitFeatures", VectorUtils_SplitFeatures, METH_VARARGS,
"vectorutils.splitFeatures(inputvector, outputvectorbase, force)\n"
"A command to split features into seperate shapefiles.\n\n"
"Where:\n"
"\n"
"* inputvector is a string containing the name of the input vector\n"
"* outputvectorbase is a string containing the base path and name of the output vectors\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"
"Example::\n"
"\n"
"   from rsgislib import vectorutils\n"
"   inputVector = './Vectors/injune_p142_psu_utm.shp'\n"
"   outputVectorBase = './TestOutputs/injune_p142_psu_utm_'\n"
"   vectorutils.splitFeatures(inputVector, outputVectorBase, True)\n"
"\n"},

{"exportPxls2Pts", VectorUtils_ExportPxls2Pts, METH_VARARGS,
"vectorutils.exportPxls2Pts(inputimg, outputvector, maskVal, force)\n"
"A command to export image pixel which have a specific value to a shapefile as points.\n\n"
"Where:\n"
"\n"
"* inputimg is a string containing the name of the input image\n"
"* outputvector is a string containing the name of the output vector\n"
"* maskVal is a float specifying the value of the image pixels to be exported\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"},

{"dist2NearestGeom", VectorUtils_Dist2NearestGeom, METH_VARARGS,
"vectorutils.dist2NearestGeom(inputVector, outputVector, minDistCol, force, useIdx, maxSearchDist)\n"
"A command to calculate the distance from each geometry to its nearest neighbouring geometry.\n"
"The function also returns the maximum minimum distance between the geometries.\n\n"
"Where:\n"
"\n"
"* inputVector is a string containing the name of the input vector\n"
"* outputVector is a string containing the name of the output vector\n"
"* minDistCol is a string with the name of the output column name\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"* useIdx is a bool, specifying whether a spatial index and max search limit should be used\n"
"* maxSearchDist is a float, with maximum search distance from each feature - only used within a spatial index.\n"
"\n"},
    
{"dist2NearestSecGeomSet", VectorUtils_Dist2NearestSecGeomSet, METH_VARARGS,
"vectorutils.dist2NearestSecGeomSet(inputVector, inDistToVector, outputVector, minDistCol, force, useIdx, maxSearchDist)\n"
"A command to calculate the distance from each geometry to its nearest neighbouring geometry.\n"
"The function also returns the maximum minimum distance between the geometries.\n\n"
"Where:\n"
"\n"
"* inputVector is a string containing the name of the input vector\n"
"* inDistToVector is a string containing the name of the input vector for which the distance to features from the input vector will be calculated.\n"
"* outputVector is a string containing the name of the output vector\n"
"* minDistCol is a string with the name of the output column name\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"* useIdx is a bool, specifying whether a spatial index and max search limit should be used\n"
"* maxSearchDist is a float, with maximum search distance from each feature - only used within a spatial index.\n"
"\n"},

{"calcMaxDist2NearestGeom", VectorUtils_CalcMaxDist2NearestGeom, METH_VARARGS,
"vectorutils.calcMaxDist2NearestGeom(inputVector)\n"
"A command to calculate the maximum minimum distance between the geometries.\n\n"
"Where:\n"
"\n"
"* inputVector is a string containing the name of the input vector\n"
"\n"},

{"spatialGraphClusterGeoms", VectorUtils_SpatialGraphClusterGeoms, METH_VARARGS,
"vectorutils.spatialGraphClusterGeoms(inputVector, outputVector, useMinSpanTree, sdEdgeLen, maxEdgeLen, force, outShpEdges, outH5EdgeLens)\n"
"A command to spatial cluster using a minimum spanning tree approach (Bunting et al 2010).\n\n"
"Where:\n"
"\n"
"* inputVector is a string containing the name of the input vector\n"
"* outputVector is a string containing the name of the output vector\n"
"* useMinSpanTree is a boolean specifying whether a minimum spanning tree should be used rather than just a graph.\n"
"* sdEdgeLen is a float\n"
"* maxEdgeLen is a double\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"* outShpEdges is a string containing the path for an output vector to export minimum spanning tree edges as a shapefile.\n"
"* outH5EdgeLens is a string containing the path for an output hdf5 file to export the minimum spanning tree edge lengths.\n"
"\n"},

{"fitPolygon2Points", VectorUtils_FitPolygon2Points, METH_VARARGS,
"vectorutils.fitPolygon2Points(inputVector, outputVector, alphaVal, force)\n"
"A command fit a polygon to the points inputted.\n\n"
"Where:\n"
"\n"
"* inputVector is a string containing the name of the input vector (must be points)\n"
"* outputVector is a string containing the name of the output vector\n"
"* alphaVal is a double specifying the alpha value to use for the calculation (if negative optimal will be calculated; default)\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"},

{"fitPolygons2PointClusters", VectorUtils_FitPolygons2PointClusters, METH_VARARGS,
"vectorutils.fitPolygons2PointClusters(inputVector, outputVector, clusterField, alphaVal, force)\n"
"A command fit a polygon to the points inputted.\n\n"
"Where:\n"
"\n"
"* inputVector is a string containing the name of the input vector (must be points)\n"
"* outputVector is a string containing the name of the output vector\n"
"* clusterField is a string specifying the column in the input shapefile which specifies the clusters\n"
"* alphaVal is a double specifying the alpha value to use for the calculation (if negative optimal will be calculated; default)\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"},

{"createLinesOfPoints", VectorUtils_CreateLinesOfPoints, METH_VARARGS,
"vectorutils.createLinesOfPoints(inputVector, outputVector, step, force)\n"
"A function to create a regularly spaced set of points following a set of lines.\n\n"
"Where:\n"
"\n"
"* inputVector is a string containing the name of the input vector (must be lines)\n"
"* outputVector is a string containing the name of the output vector (will be points)\n"
"* step is a double specifying the distance between points along the line.\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"},

{"fitActiveContourBoundaries", VectorUtils_FitActiveContourBoundaries, METH_VARARGS,
"vectorutils.fitActiveContourBoundaries(inputVector, outputVector, exterForceImg, intAlphaVal, intBetaVal, intGammaVal, minExtThresVal, force)\n"
"A command fit a polygon to the points inputted.\n\n"
"Where:\n"
"\n"
"* inputVector is a string containing the name of the input vector (must be polygons)\n"
"* outputVector is a string containing the name of the output vector\n"
"* exterForceImg is a string containing the name and path for the image file representing the external energy for the active contours\n"
"* intAlphaVal is a double specifying the alpha value for the active contour internal energy.\n"
"* intBetaVal is a double specifying the beta value for the active contour internal energy.\n"
"* intGammaVal is a double specifying the gamma value for the active contour internal energy.\n"
"* minExtThresVal is a double specifying a hard boundary for the external energy which can't be crossed.\n"
"* force is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"},
    
    
    {NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int VectorUtils_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int VectorUtils_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_vectorutils",
        NULL,
        sizeof(struct VectorUtilsState),
        VectorUtilsMethods,
        NULL,
        VectorUtils_traverse,
        VectorUtils_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__vectorutils(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_vectorutils(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_vectorutils", VectorUtilsMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct VectorUtilsState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_vectorutils.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
