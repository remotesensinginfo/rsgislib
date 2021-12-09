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

static PyObject *Classification_CollapseClasses(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("class_col"),
                             RSGIS_PY_C_TEXT("class_int_col"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat, *pszClassesColumn;
    PyObject *pClassIntCol = Py_None;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssss|O:collapse_classes", kwlist, &pszInputImage, &pszOutputFile,
                                     &pszGDALFormat, &pszClassesColumn, &pClassIntCol))
    {
        return nullptr;
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
            return nullptr;
        }
    }
    
    try
    {
        rsgis::cmds::executeCollapseRAT2Class(std::string(pszInputImage), std::string(pszOutputFile),
                                              std::string(pszGDALFormat), std::string(pszClassesColumn),
                                              classIntColStr, classIntColPresent);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *Classification_Colour3Bands(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), nullptr};
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sss:gen_rgb_img_from_clr_tbl", kwlist, &pszInputImage, &pszOutputFile, &pszGDALFormat))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeGenerate3BandFromColourTable(std::string(pszInputImage), std::string(pszOutputFile),
                                                         std::string(pszGDALFormat));
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Classification_GenRandomAccuracyPts(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("out_vec_file"),
                             RSGIS_PY_C_TEXT("out_vec_lyr"), RSGIS_PY_C_TEXT("out_format"),
                             RSGIS_PY_C_TEXT("rat_class_col"), RSGIS_PY_C_TEXT("vec_class_col"),
                             RSGIS_PY_C_TEXT("vec_ref_col"), RSGIS_PY_C_TEXT("num_pts"),
                             RSGIS_PY_C_TEXT("seed"), RSGIS_PY_C_TEXT("del_exist_vec"), nullptr};
    const char *pszInputImage, *pszOutputVecFile, *pszOutputVecLyr, *pszFormat, *pszClassImgCol, *pszClassImgVecCol, *pszClassRefVecCol;
    int numPts;
    int del_exist_vec = false;
    int seed = 10;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssssssi|ii:generate_random_accuracy_pts", kwlist, &pszInputImage,
                                     &pszOutputVecFile, &pszOutputVecLyr, &pszFormat, &pszClassImgCol, &pszClassImgVecCol,
                                     &pszClassRefVecCol, &numPts, &seed, &del_exist_vec))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeGenerateRandomAccuracyPts(std::string(pszInputImage), std::string(pszOutputVecFile),
                                                      std::string(pszOutputVecLyr), std::string(pszFormat),
                                                      std::string(pszClassImgCol), std::string(pszClassImgVecCol),
                                                      std::string(pszClassRefVecCol), numPts, seed, del_exist_vec);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Classification_GenStratifiedRandomAccuracyPts(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("out_vec_file"),
                             RSGIS_PY_C_TEXT("out_vec_lyr"), RSGIS_PY_C_TEXT("out_format"),
                             RSGIS_PY_C_TEXT("rat_class_col"), RSGIS_PY_C_TEXT("vec_class_col"),
                             RSGIS_PY_C_TEXT("vec_ref_col"), RSGIS_PY_C_TEXT("num_pts"),
                             RSGIS_PY_C_TEXT("seed"), RSGIS_PY_C_TEXT("del_exist_vec"),
                             RSGIS_PY_C_TEXT("use_pxl_lst"), nullptr};
    const char *pszInputImage, *pszOutputVecFile, *pszOutputVecLyr, *pszFormat, *pszClassImgCol, *pszClassImgVecCol, *pszClassRefVecCol;
    int numPts;
    int del_exist_vec = false;
    int seed = 10;
    int usePxlLst = false;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssssssi|iii:generate_stratified_random_accuracy_pts", kwlist, &pszInputImage,
                                     &pszOutputVecFile, &pszOutputVecLyr, &pszFormat, &pszClassImgCol, &pszClassImgVecCol,
                                     &pszClassRefVecCol, &numPts, &seed, &del_exist_vec, &usePxlLst))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeGenerateStratifiedRandomAccuracyPts(std::string(pszInputImage), std::string(pszOutputVecFile),
                                                                std::string(pszOutputVecLyr), std::string(pszFormat),
                                                                std::string(pszClassImgCol), std::string(pszClassImgVecCol),
                                                                std::string(pszClassRefVecCol), numPts, seed, del_exist_vec,
                                                                usePxlLst);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *Classification_PopClassInfoAccuracyPts(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("vec_file"),
                             RSGIS_PY_C_TEXT("vec_lyr"), RSGIS_PY_C_TEXT("rat_class_col"),
                             RSGIS_PY_C_TEXT("vec_class_col"), RSGIS_PY_C_TEXT("vec_ref_col"),
                             RSGIS_PY_C_TEXT("vec_process_col"), nullptr};
    const char *pszInputImage, *pszVecFile, *pszVecLyr, *pszClassImgCol, *pszClassImgVecCol;
    PyObject *classRefVecColObj = nullptr;
    PyObject *processVecColObj = nullptr;
    
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssss|OO:pop_class_info_accuracy_pts", kwlist, &pszInputImage, &pszVecFile,
                                     &pszVecLyr, &pszClassImgCol, &pszClassImgVecCol, &classRefVecColObj, &processVecColObj))
    {
        return nullptr;
    }
    
    bool addRefCol = false;
    bool addProcessCol = false;
    std::string pszClassRefVecCol = std::string();
    std::string pszProcessVecCol = std::string();
    
    if(RSGISPY_CHECK_STRING(classRefVecColObj))
    {
        pszClassRefVecCol = RSGISPY_STRING_EXTRACT(classRefVecColObj);
        addRefCol = true;
    }

    if(RSGISPY_CHECK_STRING(processVecColObj))
    {
        pszProcessVecCol = RSGISPY_STRING_EXTRACT(processVecColObj);
        addProcessCol = true;
    }
    
    try
    {
        rsgis::cmds::executePopClassInfoAccuracyPts(std::string(pszInputImage), std::string(pszVecFile),
                                                    std::string(pszVecLyr), std::string(pszClassImgCol),
                                                    std::string(pszClassImgVecCol),
                                                    pszClassRefVecCol, addRefCol,
                                                    pszProcessVecCol, addProcessCol);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}




// Our list of functions in this module
static PyMethodDef ClassificationMethods[] = {
{"collapse_classes", (PyCFunction)Classification_CollapseClasses, METH_VARARGS | METH_KEYWORDS,
"rsgislib.classification.collapse_classes(input_img, output_img, gdalformat, class_col, class_int_col)\n"
"Collapses an attribute table with a large number of classified clumps (segments) to\n"
"a attribute table with a single row per class (i.e. a classification rather than segmentation.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name and path of the input file with attribute table.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"
":param class_col: is a string with the name of the column with the class names - internally this will be treated as a string column even if a numerical column is specified.\n"
":param class_int_col: is a sting specifying the name of a column with the integer class representation. This is an optional parameter but if specified then the int reprentation of the classes will be reserved."},

{"gen_rgb_img_from_clr_tbl", (PyCFunction)Classification_Colour3Bands, METH_VARARGS | METH_KEYWORDS,
"rsgislib.classification.gen_rgb_img_from_clr_tbl(input_img, output_img, gdalformat)\n"
"Generates a 3 band colour image from the colour table in the input file.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name and path of the input file with attribute table.\n"
":param output_img: is a string containing the name and path of the output file.\n"
":param gdalformat: is a string with the output image format for the GDAL driver.\n"},
    
{"generate_random_accuracy_pts", (PyCFunction)Classification_GenRandomAccuracyPts, METH_VARARGS | METH_KEYWORDS,
"rsgislib.classification.generate_random_accuracy_pts(input_img, out_vec_file, out_vec_lyr, out_format, rat_class_col, vec_class_col, vec_ref_col, num_pts, seed, del_exist_vec)\n"
"Generates a set of random points for accuracy assessment.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name and path of the input image with attribute table.\n"
":param out_vec_file: is a string containing the name and path of the output vector file.\n"
":param out_vec_lyr: is a string containing the vector file layer name.\n"
":param out_format: the output vector file format (e.g., GPKG)\n"
":param rat_class_col: is a string speciyfing the name of the column in the image file containing the class names.\n"
":param vec_class_col: is a string specifiying the output column in the vector file for the classified class names.\n"
":param vec_ref_col: is a string specifiying an output column in the vector file which can be used in the accuracy assessment for the reference data.\n"
":param num_pts: is an int specifying the total number of points which should be created.\n"
":param seed: is an int specifying the seed for the random number generator. (Optional: Default 10)\n"
":param del_exist_vec: is a bool, specifying whether to force removal of the output vector if it exists. (Optional: Default False)\n"
},

{"generate_stratified_random_accuracy_pts", (PyCFunction)Classification_GenStratifiedRandomAccuracyPts, METH_VARARGS | METH_KEYWORDS,
"rsgislib.classification.generate_stratified_random_accuracy_pts(input_img, out_vec_file, out_vec_lyr, out_format, rat_class_col, vec_class_col, vec_ref_col, num_pts, seed, del_exist_vec, use_pxl_lst)\n"
"Generates a set of stratified random points for accuracy assessment.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name and path of the input image with attribute table.\n"
":param out_vec_file: is a string containing the name and path of the output vector file.\n"
":param out_vec_lyr: is a string containing the vector file layer name.\n"
":param out_format: the output vector file format (e.g., GPKG)\n"
":param rat_class_col: is a string speciyfing the name of the column in the image file containing the class names.\n"
":param vec_class_col: is a string specifiying the output column in the vector file for the classified class names.\n"
":param vec_ref_col: is a string specifiying an output column in the vector file which can be used in the accuracy assessment for the reference data.\n"
":param num_pts: is an int specifying the number of points for each class which should be created.\n"
":param seed: is an int specifying the seed for the random number generator. (Optional: Default 10)\n"
":param del_exist_vec: is a bool, specifying whether to force removal of the output vector if it exists. (Optional: Default False)\n"
":param use_pxl_lst: is a bool, if there are only a small number of pixels then creating a list of all the pixel locations will speed up processing. (Optional: Default False)\n"
},
    
{"pop_class_info_accuracy_pts", (PyCFunction)Classification_PopClassInfoAccuracyPts, METH_VARARGS | METH_KEYWORDS,
"rsgislib.classification.pop_class_info_accuracy_pts(input_img:str, vec_file:str, vec_lyr:str, rat_class_col:str, vec_class_col:str, vec_ref_col:str = None, vec_process_col:str = None)\n"
"Generates a set of stratified random points for accuracy assessment.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name and path of the input image with attribute table.\n"
":param vec_file: is a string containing the name and path of the input vector file.\n"
":param vec_lyr: is a string containing the vector file layer name.\n"
":param rat_class_col: is a string specifying the name of the column in the image file containing the class names.\n"
":param vec_class_col: is a string specifying the output column in the vector file for the classified class names.\n"
":param vec_ref_col: is an optional string specifying an output column in the vector file which can be used in the accuracy assessment for the reference data.\n"
":param vec_process_col: is an optional string specifying an output column in the vector file which is used allocate points as processed or otherwise."
},

    {nullptr}        /* Sentinel */
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
        nullptr,
        sizeof(struct ClassificationState),
        ClassificationMethods,
        nullptr,
        Classification_traverse,
        Classification_clear,
        nullptr
};

#define INITERROR return nullptr

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
    if( pModule == nullptr )
        INITERROR;

    struct ClassificationState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_classification.error", nullptr, nullptr);
    if( state->error == nullptr )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
