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

/*
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
*/

static PyObject *VectorUtils_VectorMaths(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("vec_file"), RSGIS_PY_C_TEXT("vec_lyr"),
                             RSGIS_PY_C_TEXT("out_vec_file"), RSGIS_PY_C_TEXT("out_vec_lyr"),
                             RSGIS_PY_C_TEXT("out_format"), RSGIS_PY_C_TEXT("out_col"), RSGIS_PY_C_TEXT("exp"),
                             RSGIS_PY_C_TEXT("vars"), RSGIS_PY_C_TEXT("del_exist_vec"), nullptr};
    const char *pszInputVectorFile, *pszInputVectorLyr, *pszOutputVectorFile, *pszOutputVectorLyr, *pszOutFormat, *pszOutColName, *pszExpression;
    int delExistVec = false;
    PyObject *pVarsObj;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssssssO|i:vector_maths", kwlist, &pszInputVectorFile, &pszInputVectorLyr,
                                     &pszOutputVectorFile, &pszOutputVectorLyr, &pszOutFormat, &pszOutColName,
                                     &pszExpression, &pVarsObj, &delExistVec))
    {
        return nullptr;
    }
    
    if( !PySequence_Check(pVarsObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "variables argument must be a sequence");
        return nullptr;
    }
    
    Py_ssize_t nVarsDefns = PySequence_Size(pVarsObj);
    unsigned int numVars = nVarsDefns;
    std::vector<rsgis::cmds::RSGISVariableFieldCmds> vars = std::vector<rsgis::cmds::RSGISVariableFieldCmds>();
    vars.reserve(numVars);
    
    for( Py_ssize_t n = 0; n < nVarsDefns; n++ )
    {
        PyObject *o = PySequence_GetItem(pVarsObj, n);
        
        PyObject *pName = PyObject_GetAttrString(o, "name");
        if( ( pName == nullptr ) || ( pName == Py_None ) || !RSGISPY_CHECK_STRING(pName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'name\'" );
            Py_DECREF(pName);
            Py_DECREF(o);
            return nullptr;
        }
        
        PyObject *pFieldName = PyObject_GetAttrString(o, "fieldName");
        if( ( pFieldName == nullptr ) || ( pFieldName == Py_None ) || !RSGISPY_CHECK_STRING(pFieldName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'fieldName\'" );
            Py_DECREF(pName);
            Py_DECREF(pFieldName);
            Py_DECREF(o);
            return nullptr;
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
        rsgis::cmds::executeVectorMaths(std::string(pszInputVectorFile), std::string(pszInputVectorLyr),
                                        std::string(pszOutputVectorFile), std::string(pszOutputVectorLyr),
                                        std::string(pszOutFormat), std::string(pszOutColName),
                                        std::string(pszExpression), (bool)delExistVec, vars);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}



static PyObject *VectorUtils_CreateLinesOfPoints(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("vec_file"), RSGIS_PY_C_TEXT("vec_lyr"),
                             RSGIS_PY_C_TEXT("out_vec_file"), RSGIS_PY_C_TEXT("out_vec_lyr"),
                             RSGIS_PY_C_TEXT("out_format"), RSGIS_PY_C_TEXT("step"),
                             RSGIS_PY_C_TEXT("del_exist_vec"), nullptr};
    const char *pszInputVectorFile, *pszInputVectorLyr, *pszOutputVectorFile, *pszOutputVectorLyr, *pszOutFormat;
    double step;
    int delExistVec = false;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssssd|i:create_lines_of_points", kwlist, &pszInputVectorFile, &pszInputVectorLyr,
                                     &pszOutputVectorFile, &pszOutputVectorLyr, &pszOutFormat, &step, &delExistVec))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeCreateLinesOfPoints(std::string(pszInputVectorFile), std::string(pszInputVectorLyr),
                                                std::string(pszOutputVectorFile), std::string(pszOutputVectorLyr),
                                                std::string(pszOutFormat), step, (bool)delExistVec);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *VectorUtils_CheckValidateGeometries(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("vec_file"), RSGIS_PY_C_TEXT("vec_lyr"),
                             RSGIS_PY_C_TEXT("out_vec_file"), RSGIS_PY_C_TEXT("out_vec_lyr"),
                             RSGIS_PY_C_TEXT("out_format"), RSGIS_PY_C_TEXT("print_err_geoms"),
                             RSGIS_PY_C_TEXT("del_exist_vec"), nullptr};
    const char *pszInputVectorFile, *pszInputVectorLyr, *pszOutputVectorFile, *pszOutputVectorLyr, *pszOutFormat;
    int printGeomErrsInt = false;
    int delExistVec = false;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssss|ii:check_validate_geometries", kwlist, &pszInputVectorFile, &pszInputVectorLyr,
                                     &pszOutputVectorFile, &pszOutputVectorLyr, &pszOutFormat, &printGeomErrsInt, &delExistVec))
    {
        return nullptr;
    }

    try
    {
        bool printGeomErrs = (bool) printGeomErrsInt;
        rsgis::cmds::executeCheckValidateGeometries(std::string(pszInputVectorFile), std::string(pszInputVectorLyr),
                                                    std::string(pszOutputVectorFile), std::string(pszOutputVectorLyr),
                                                    std::string(pszOutFormat), printGeomErrs, (bool)delExistVec);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef VectorUtilsMethods[] = {

{"vector_maths", (PyCFunction)VectorUtils_VectorMaths, METH_VARARGS | METH_KEYWORDS,
"vectorutils.vector_maths(vec_file, vec_lyr, out_vec_file, out_vec_lyr, out_col, exp, vars, del_exist_vec)\n"
"A command to calculate a number column from data in existing columns.\n"
"The syntax for the expression is from the `muparser library <http://muparser.beltoforion.de>`_ "
"`see here for available operations and syntax <http://beltoforion.de/article.php?a=muparser&hl=en&p=features&s=idPageTop>`_\n."
"\n"
":param vec_file: is a string containing the input vector file path\n"
":param vec_lyr: is a string containing the name of the input vector layer name\n"
":param out_vec_file: is a string containing the output vector file path\n"
":param out_vec_lyr: is a string containing the name of the output vector layer name\n"
":param out_format: is a string containing the output file format\n"
":param out_col: is a string containing the name of the output column\n"
":param exp: is a string containing the muparser expression to be calculated.\n"
":param vars: is a list defining the names of the variables used within the expression and defining which columns they are in the inputVector. The must be a list and contain two fields \'name\' and \'fieldName\'.\n"
":param del_exist_vec: is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"},

{"create_lines_of_points", (PyCFunction)VectorUtils_CreateLinesOfPoints, METH_VARARGS | METH_KEYWORDS,
"vectorutils.create_lines_of_points(vec_file, vec_lyr, out_vec_file, out_vec_lyr, out_format, step, del_exist_vec)\n"
"A function to create a regularly spaced set of points following a set of lines.\n"
"\n"
":param vec_file: is a string containing the input vector file path (must be lines)\n"
":param vec_lyr: is a string containing the name of the input vector layer name\n"
":param out_vec_file: is a string containing the output vector file path (will be points)\n"
":param out_vec_lyr: is a string containing the name of the output vector layer name\n"
":param out_format: is a string containing the output file format\n"
":param step: is a double specifying the distance between points along the line.\n"
":param del_exist_vec: is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"},

{"check_validate_geometries", (PyCFunction)VectorUtils_CheckValidateGeometries, METH_VARARGS | METH_KEYWORDS,
"vectorutils.check_validate_geometries(vec_file, vec_lyr, out_vec_file, out_vec_lyr, out_format, print_err_geoms, del_exist_vec)\n"
"A command fit a polygon to the points inputted.\n"
"\n"
":param vec_file: is a string containing the input vector file path\n"
":param vec_lyr: is a string containing the name of the input vector layer name\n"
":param out_vec_file: is a string containing the output vector file path\n"
":param out_vec_lyr: is a string containing the name of the output vector layer name\n"
":param out_format: is a string specifying the output vector GDAL/OGR driver (e.g., GPKG).\n"
":param print_err_geoms: is a bool, specifying whether were errors are found they are printed to the console.\n"
":param del_exist_vec: is a bool, specifying whether to force removal of the output vector if it exists\n"
"\n"},
    
{nullptr}        /* Sentinel */
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
        nullptr,
        sizeof(struct VectorUtilsState),
        VectorUtilsMethods,
        nullptr,
        VectorUtils_traverse,
        VectorUtils_clear,
        nullptr
};

#define INITERROR return nullptr

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
    if( pModule == nullptr )
        INITERROR;

    struct VectorUtilsState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_vectorutils.error", nullptr, nullptr);
    if( state->error == nullptr )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
