/*
 *  imagemorphology.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/09/2016.
 *  Copyright 2016 RSGISLib.
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
#include "cmds/RSGISCmdImageMorphology.h"

/* An exception object for this module */
/* created in the init function */
struct ImageMorphologyState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ImageMorphologyState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ImageMorphologyState _state;
#endif

/*
static void FreePythonObjects(std::vector<PyObject*> toFree)
{
    std::vector<PyObject*>::iterator iter;
    for(iter = toFree.begin(); iter != toFree.end(); ++iter)
    {
        Py_XDECREF(*iter);
    }
}
*/

static PyObject *ImageMorphology_CreateCircularOp(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszOutputFile;
    int morphOpSize;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("output_file"), RSGIS_PY_C_TEXT("op_size"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "si:createCircularOp", kwlist, &pszOutputFile, &morphOpSize))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeCreateCircularOperator(std::string(pszOutputFile), morphOpSize);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageMorphology_ImageDilate(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage;
    const char *pszMorphOperator;
    const char *pszImageFormat = "KEA";
    int datatype;
    int useOperatorFile;
    int morphOpSize;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("morph_op_file"), RSGIS_PY_C_TEXT("use_op_file"),
                             RSGIS_PY_C_TEXT("op_size"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssiisi:imageDilate", kwlist, &pszInputImage, &pszOutputImage,
                                    &pszMorphOperator, &useOperatorFile, &morphOpSize, &pszImageFormat, &datatype))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeImageDilate(std::string(pszInputImage), std::string(pszOutputImage),
                                        std::string(pszMorphOperator), (bool)useOperatorFile,
                                        morphOpSize, std::string(pszImageFormat), (rsgis::RSGISLibDataType)datatype);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageMorphology_ImageErode(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage;
    const char *pszMorphOperator;
    const char *pszImageFormat = "KEA";
    int datatype;
    int useOperatorFile;
    int morphOpSize;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("morph_op_file"), RSGIS_PY_C_TEXT("use_op_file"),
                             RSGIS_PY_C_TEXT("op_size"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssiisi:imageErode", kwlist, &pszInputImage, &pszOutputImage,
                                    &pszMorphOperator, &useOperatorFile, &morphOpSize, &pszImageFormat, &datatype))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageErode(std::string(pszInputImage), std::string(pszOutputImage),
                                       std::string(pszMorphOperator), (bool)useOperatorFile,
                                       morphOpSize, std::string(pszImageFormat), (rsgis::RSGISLibDataType)datatype);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageMorphology_ImageGradiant(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage;
    const char *pszMorphOperator;
    const char *pszImageFormat = "KEA";
    int datatype;
    int useOperatorFile;
    int morphOpSize;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("morph_op_file"), RSGIS_PY_C_TEXT("use_op_file"),
                             RSGIS_PY_C_TEXT("op_size"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssiisi:imageGradiant", kwlist, &pszInputImage, &pszOutputImage,
                                    &pszMorphOperator, &useOperatorFile, &morphOpSize, &pszImageFormat, &datatype))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageGradiant(std::string(pszInputImage), std::string(pszOutputImage),
                                          std::string(pszMorphOperator), (bool)useOperatorFile,
                                          morphOpSize, std::string(pszImageFormat),
                                          (rsgis::RSGISLibDataType)datatype);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageMorphology_ImageDilateCombinedOut(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage;
    const char *pszMorphOperator;
    const char *pszImageFormat = "KEA";
    int datatype;
    int useOperatorFile;
    int morphOpSize;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("morph_op_file"), RSGIS_PY_C_TEXT("use_op_file"),
                             RSGIS_PY_C_TEXT("op_size"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssiisi:imageDilateCombinedOut", kwlist, &pszInputImage,
                                    &pszOutputImage, &pszMorphOperator, &useOperatorFile, &morphOpSize,
                                    &pszImageFormat, &datatype))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageDilateCombinedOut(std::string(pszInputImage), std::string(pszOutputImage),
                                                   std::string(pszMorphOperator), (bool)useOperatorFile,
                                                   morphOpSize, std::string(pszImageFormat),
                                                   (rsgis::RSGISLibDataType)datatype);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageMorphology_ImageErodeCombinedOut(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage;
    const char *pszMorphOperator;
    const char *pszImageFormat = "KEA";
    int datatype;
    int useOperatorFile;
    int morphOpSize;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("morph_op_file"), RSGIS_PY_C_TEXT("use_op_file"),
                             RSGIS_PY_C_TEXT("op_size"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssiisi:imageErodeCombinedOut", kwlist, &pszInputImage,
                                    &pszOutputImage, &pszMorphOperator, &useOperatorFile, &morphOpSize,
                                    &pszImageFormat, &datatype))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageErodeCombinedOut(std::string(pszInputImage), std::string(pszOutputImage),
                                                  std::string(pszMorphOperator), (bool)useOperatorFile,
                                                  morphOpSize, std::string(pszImageFormat), (rsgis::RSGISLibDataType)datatype);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageMorphology_ImageGradiantCombinedOut(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage;
    const char *pszMorphOperator;
    const char *pszImageFormat = "KEA";
    int datatype;
    int useOperatorFile;
    int morphOpSize;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("morph_op_file"), RSGIS_PY_C_TEXT("use_op_file"),
                             RSGIS_PY_C_TEXT("op_size"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssiisi:imageGradiantCombinedOut", kwlist, &pszInputImage, &pszOutputImage,
                                    &pszMorphOperator, &useOperatorFile, &morphOpSize, &pszImageFormat, &datatype))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageGradiantCombinedOut(std::string(pszInputImage), std::string(pszOutputImage),
                                                     std::string(pszMorphOperator), (bool)useOperatorFile,
                                                     morphOpSize, std::string(pszImageFormat), (rsgis::RSGISLibDataType)datatype);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageMorphology_ImageLocalMinima(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage;
    const char *pszMorphOperator;
    const char *pszImageFormat = "KEA";
    int datatype;
    int useOperatorFile;
    int morphOpSize;
    int outputSequencial;
    int allowEquals;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("sequencial_out"), RSGIS_PY_C_TEXT("allow_equal"),
                             RSGIS_PY_C_TEXT("morph_op_file"), RSGIS_PY_C_TEXT("use_op_file"),
                             RSGIS_PY_C_TEXT("op_size"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssiisiisi:imageLocalMinima", kwlist, &pszInputImage, &pszOutputImage,
                                    &outputSequencial, &allowEquals, &pszMorphOperator, &useOperatorFile, &morphOpSize,
                                    &pszImageFormat, &datatype))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageLocalMinima(std::string(pszInputImage), std::string(pszOutputImage),
                                             (bool)outputSequencial, (bool)allowEquals,
                                             std::string(pszMorphOperator), (bool)useOperatorFile,
                                             morphOpSize, std::string(pszImageFormat), (rsgis::RSGISLibDataType)datatype);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageMorphology_ImageLocalMinimaCombinedOut(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage;
    const char *pszMorphOperator;
    const char *pszImageFormat = "KEA";
    int datatype;
    int useOperatorFile;
    int morphOpSize;
    int outputSequencial;
    int allowEquals;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("sequencial_out"), RSGIS_PY_C_TEXT("allow_equal"),
                             RSGIS_PY_C_TEXT("morph_op_file"), RSGIS_PY_C_TEXT("use_op_file"),
                             RSGIS_PY_C_TEXT("op_size"), RSGIS_PY_C_TEXT("gdalformat"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssiisiisi:imageLocalMinimaCombinedOut", kwlist, &pszInputImage,
                                    &pszOutputImage, &outputSequencial, &allowEquals, &pszMorphOperator, &useOperatorFile,
                                    &morphOpSize, &pszImageFormat, &datatype))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageLocalMinimaCombinedOut(std::string(pszInputImage), std::string(pszOutputImage),
                                                        (bool)outputSequencial, (bool)allowEquals,
                                                        std::string(pszMorphOperator), (bool)useOperatorFile,
                                                        morphOpSize, std::string(pszImageFormat), (rsgis::RSGISLibDataType)datatype);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageMorphology_ImageOpening(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage, *pszTempImage;
    const char *pszMorphOperator;
    const char *pszImageFormat = "KEA";
    int datatype;
    int useOperatorFile;
    int morphOpSize;
    int numIterations=1;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("tmp_img"), RSGIS_PY_C_TEXT("morph_op_file"),
                             RSGIS_PY_C_TEXT("use_op_file"), RSGIS_PY_C_TEXT("op_size"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("niters"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssssiisi|i:imageOpening", kwlist, &pszInputImage, &pszOutputImage,
                                    &pszTempImage, &pszMorphOperator, &useOperatorFile, &morphOpSize, &pszImageFormat,
                                    &datatype, &numIterations))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageOpening(std::string(pszInputImage), std::string(pszOutputImage),
                                         std::string(pszTempImage), std::string(pszMorphOperator),
                                         (bool)useOperatorFile, morphOpSize, numIterations, std::string(pszImageFormat),
                                         (rsgis::RSGISLibDataType)datatype);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageMorphology_ImageClosing(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage, *pszTempImage;
    const char *pszMorphOperator;
    const char *pszImageFormat = "KEA";
    int datatype;
    int useOperatorFile;
    int morphOpSize;
    int numIterations=1;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("tmp_img"), RSGIS_PY_C_TEXT("morph_op_file"),
                             RSGIS_PY_C_TEXT("use_op_file"), RSGIS_PY_C_TEXT("op_size"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("niters"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssssiisi|i:imageClosing", kwlist, &pszInputImage, &pszOutputImage,
                                    &pszTempImage, &pszMorphOperator, &useOperatorFile, &morphOpSize, &pszImageFormat,
                                    &datatype, &numIterations))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageClosing(std::string(pszInputImage), std::string(pszOutputImage),
                                         std::string(pszTempImage), std::string(pszMorphOperator),
                                         (bool)useOperatorFile, morphOpSize, numIterations,
                                         std::string(pszImageFormat), (rsgis::RSGISLibDataType)datatype);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageMorphology_ImageBlackTopHat(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage, *pszTempImage;
    const char *pszMorphOperator;
    const char *pszImageFormat = "KEA";
    int datatype;
    int useOperatorFile;
    int morphOpSize;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("tmp_img"), RSGIS_PY_C_TEXT("morph_op_file"),
                             RSGIS_PY_C_TEXT("use_op_file"), RSGIS_PY_C_TEXT("op_size"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssssiisi:imageBlackTopHat", kwlist, &pszInputImage, &pszOutputImage,
                                    &pszTempImage, &pszMorphOperator, &useOperatorFile, &morphOpSize,
                                    &pszImageFormat, &datatype))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageBlackTopHat(std::string(pszInputImage), std::string(pszOutputImage),
                                             std::string(pszTempImage), std::string(pszMorphOperator),
                                             (bool)useOperatorFile, morphOpSize, std::string(pszImageFormat),
                                             (rsgis::RSGISLibDataType)datatype);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageMorphology_ImageWhiteTopHat(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage, *pszOutputImage, *pszTempImage;
    const char *pszMorphOperator;
    const char *pszImageFormat = "KEA";
    int datatype;
    int useOperatorFile;
    int morphOpSize;
    
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("tmp_img"), RSGIS_PY_C_TEXT("morph_op_file"),
                             RSGIS_PY_C_TEXT("use_op_file"), RSGIS_PY_C_TEXT("op_size"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, keywds, "ssssiisi:imageWhiteTopHat", kwlist, &pszInputImage, &pszOutputImage,
                                    &pszTempImage, &pszMorphOperator, &useOperatorFile, &morphOpSize,
                                    &pszImageFormat, &datatype))
    {
        return nullptr;
    }
    
    try
    {
        rsgis::cmds::executeImageWhiteTopHat(std::string(pszInputImage), std::string(pszOutputImage),
                                             std::string(pszTempImage), std::string(pszMorphOperator),
                                             (bool)useOperatorFile, morphOpSize, std::string(pszImageFormat),
                                             (rsgis::RSGISLibDataType)datatype);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}



// Our list of functions in this module
static PyMethodDef ImageMorphologyMethods[] = {
{"createCircularOp", (PyCFunction)ImageMorphology_CreateCircularOp, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.createCircularOp(output_file=string, op_size=int)\n"
"Performs an image morphology dilate operation. \n"
"\n"
"Where:\n"
"\n"
":param output_file: is a string specifying the name and path of the output matrix file.\n"
":param op_size: is a integer specifying the size of the operator.\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib.imagemorphology\n"
"   \n"
"   rsgislib.imagemorphology.createCircularOp(outputFile='CircularOp5.gmtxt', op_size=5)\n"
"\n"},
    
    
{"imageDilate", (PyCFunction)ImageMorphology_ImageDilate, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.imageDilate(input_img=string, output_img=string, morph_op_file=string, use_op_file=boolean, op_size=int, gdalformat=string, datatype=int)\n"
"Performs an image morphology dilate operation.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying the name and path of the input file.\n"
":param output_img: is a string specifying the name and path of the output file.\n"
":param morph_op_file: is a string with the name and path to a .gmtxt file with a square binary matrix specifying the morphology operator\n"
":param use_op_file: is a boolean specifying whether the morph_op_file file is present or whether a square operator (specified via op_size) should be used. (True = morph_op_file, False = op_size)\n"
":param op_size: is a integer specifying the square operator size (only used if use_op_file is False)\n"
":param gdalformat: is a string specifying the GDAL image format (e.g., KEA)\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   import rsgislib.imagemorphology\n"
"   \n"
"   inImg = 'clumpsImage.kea'\n"
"   outImg = 'clumpsImage_Dilate.kea'\n"
"   \n"
"   rsgislib.imagemorphology.imageDilate(input_img=inImg, output_img=outImg, morph_op_file="", use_op_file=False, op_size=5, gdalformat='KEA', datatype=rsgislib.TYPE_32UINT)\n"
"\n"},
    
{"imageErode", (PyCFunction)ImageMorphology_ImageErode, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.imageErode(input_img=string, output_img=string, morph_op_file=string, use_op_file=boolean, op_size=int, gdalformat=string, datatype=int)\n"
"Performs an image morphology erode operation. \n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying the name and path of the input file.\n"
":param output_img: is a string specifying the name and path of the output file.\n"
":param morph_op_file: is a string with the name and path to a .gmtxt file with a square binary matrix specifying the morphology operator\n"
":param use_op_file: is a boolean specifying whether the morph_op_file file is present or whether a square operator (specified via op_size) should be used. (True = morph_op_file, False = op_size)\n"
":param op_size: is a integer specifying the square operator size (only used if use_op_file is False)\n"
":param gdalformat: is a string specifying the GDAL image format (e.g., KEA)\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"},

{"imageGradiant", (PyCFunction)ImageMorphology_ImageGradiant, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.imageGradiant(input_img=string, output_img=string, morph_op_file=string, use_op_file=boolean, op_size=int, gdalformat=string, datatype=int)\n"
"Performs an image morphology gradiant operation. \n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying the name and path of the input file.\n"
":param output_img: is a string specifying the name and path of the output file.\n"
":param morph_op_file: is a string with the name and path to a .gmtxt file with a square binary matrix specifying the morphology operator\n"
":param use_op_file: is a boolean specifying whether the morph_op_file file is present or whether a square operator (specified via op_size) should be used. (True = morph_op_file, False = op_size)\n"
":param op_size: is a integer specifying the square operator size (only used if use_op_file is False)\n"
":param gdalformat: is a string specifying the GDAL image format (e.g., KEA)\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"},
    
{"imageDilateCombinedOut", (PyCFunction)ImageMorphology_ImageDilateCombinedOut, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.imageDilateCombinedOut(input_img=string, output_img=string, morph_op_file=string, use_op_file=boolean, op_size=int, gdalformat=string, datatype=int)\n"
"Performs an image morphology dilate operation, where the outputs will be combined into a single image. \n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying the name and path of the input file.\n"
":param output_img: is a string specifying the name and path of the output file.\n"
":param morph_op_file: is a string with the name and path to a .gmtxt file with a square binary matrix specifying the morphology operator\n"
":param use_op_file: is a boolean specifying whether the morph_op_file file is present or whether a square operator (specified via op_size) should be used. (True = morph_op_file, False = op_size)\n"
":param op_size: is a integer specifying the square operator size (only used if use_op_file is False)\n"
":param gdalformat: is a string specifying the GDAL image format (e.g., KEA)\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"},

{"imageErodeCombinedOut", (PyCFunction)ImageMorphology_ImageErodeCombinedOut, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.imageErodeCombinedOut(input_img=string, output_img=string, morph_op_file=string, use_op_file=boolean, op_size=int, gdalformat=string, datatype=int)\n"
"Performs an image morphology erode operation, where the outputs will be combined into a single image. \n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying the name and path of the input file.\n"
":param output_img: is a string specifying the name and path of the output file.\n"
":param morph_op_file: is a string with the name and path to a .gmtxt file with a square binary matrix specifying the morphology operator\n"
":param use_op_file: is a boolean specifying whether the morph_op_file file is present or whether a square operator (specified via op_size) should be used. (True = morph_op_file, False = op_size)\n"
":param op_size: is a integer specifying the square operator size (only used if use_op_file is False)\n"
":param gdalformat: is a string specifying the GDAL image format (e.g., KEA)\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"},

{"imageGradiantCombinedOut", (PyCFunction)ImageMorphology_ImageGradiantCombinedOut, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.imageGradiantCombinedOut(input_img=string, output_img=string, morph_op_file=string, use_op_file=boolean, op_size=int, gdalformat=string, datatype=int)\n"
"Performs an image morphology gradiant operation, where the outputs will be combined into a single image. \n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying the name and path of the input file.\n"
":param output_img: is a string specifying the name and path of the output file.\n"
":param morph_op_file: is a string with the name and path to a .gmtxt file with a square binary matrix specifying the morphology operator\n"
":param use_op_file: is a boolean specifying whether the morph_op_file file is present or whether a square operator (specified via op_size) should be used. (True = morph_op_file, False = op_size)\n"
":param op_size: is a integer specifying the square operator size (only used if use_op_file is False)\n"
":param gdalformat: is a string specifying the GDAL image format (e.g., KEA)\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"},
    
{"imageLocalMinima", (PyCFunction)ImageMorphology_ImageLocalMinima, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.imageLocalMinima(input_img=string, output_img=string, sequencial_out=boolean, allow_equal=boolean, morph_op_file=string, use_op_file=boolean, op_size=int, gdalformat=string, datatype=int)\n"
"Uses image morphology to find local minima. \n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying the name and path of the input file.\n"
":param output_img: is a string specifying the name and path of the output file.\n"
":param sequencial_out: is a boolean whether the output minima should be individual numbered (True) or just a binary mask (False).\n"
":param allow_equal: is a boolean specifying whether equal values are allowed in the output.\n"
":param morph_op_file: is a string with the name and path to a .gmtxt file with a square binary matrix specifying the morphology operator\n"
":param use_op_file: is a boolean specifying whether the morph_op_file file is present or whether a square operator (specified via op_size) should be used. (True = morph_op_file, False = op_size)\n"
":param op_size: is a integer specifying the square operator size (only used if use_op_file is False)\n"
":param gdalformat: is a string specifying the GDAL image format (e.g., KEA)\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"},
    
{"imageLocalMinimaCombinedOut", (PyCFunction)ImageMorphology_ImageLocalMinimaCombinedOut, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.imageLocalMinimaCombinedOut(input_img=string, output_img=string, sequencial_out=boolean, allow_equal=boolean, morph_op_file=string, use_op_file=boolean, op_size=int, gdalformat=string, datatype=int)\n"
"Uses image morphology to find local minima, where the outputs will be combined into a single image.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying the name and path of the input file.\n"
":param output_img: is a string specifying the name and path of the output file.\n"
":param sequencial_out: is a boolean whether the output minima should be individual numbered (True) or just a binary mask (False).\n"
":param allow_equal: is a boolean specifying whether equal values are allowed in the output.\n"
":param morph_op_file: is a string with the name and path to a .gmtxt file with a square binary matrix specifying the morphology operator\n"
":param use_op_file: is a boolean specifying whether the morph_op_file file is present or whether a square operator (specified via op_size) should be used. (True = morph_op_file, False = op_size)\n"
":param op_size: is a integer specifying the square operator size (only used if use_op_file is False)\n"
":param gdalformat: is a string specifying the GDAL image format (e.g., KEA)\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"},

{"imageOpening", (PyCFunction)ImageMorphology_ImageOpening, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.imageOpening(input_img=string, output_img=string, tmp_img=string, morph_op_file=string, use_op_file=boolean, op_size=int, gdalformat=string, datatype=int, niters=int)\n"
"Performs an image morphology opening operation. \n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying the name and path of the input file.\n"
":param output_img: is a string specifying the name and path of the output file.\n"
":param tmp_img: is a string specifying the name and path of a tempory file used for intermediate processing step(s). If empty string ('') then an in memory image will be used.\n"
":param morph_op_file: is a string with the name and path to a .gmtxt file with a square binary matrix specifying the morphology operator\n"
":param use_op_file: is a boolean specifying whether the morph_op_file file is present or whether a square operator (specified via op_size) should be used. (True = morph_op_file, False = op_size)\n"
":param op_size: is a integer specifying the square operator size (only used if use_op_file is False)\n"
":param gdalformat: is a string specifying the GDAL image format (e.g., KEA)\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
":param niters: is an int for the number of iterations of the operators (Optional, default = 1)\n"
"\n"},

{"imageClosing", (PyCFunction)ImageMorphology_ImageClosing, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.imageClosing(input_img=string, output_img=string, tmp_img=string, morph_op_file=string, use_op_file=boolean, op_size=int, gdalformat=string, datatype=int, niters=int)\n"
"Performs an image morphology closing operation. \n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying the name and path of the input file.\n"
":param output_img: is a string specifying the name and path of the output file.\n"
":param tmp_img: is a string specifying the name and path of a tempory file used for intermediate processing step(s). If empty string ('') then an in memory image will be used.\n"
":param morph_op_file: is a string with the name and path to a .gmtxt file with a square binary matrix specifying the morphology operator\n"
":param use_op_file: is a boolean specifying whether the morph_op_file file is present or whether a square operator (specified via op_size) should be used. (True = morph_op_file, False = op_size)\n"
":param op_size: is a integer specifying the square operator size (only used if use_op_file is False)\n"
":param gdalformat: is a string specifying the GDAL image format (e.g., KEA)\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
":param niters: is an int for the number of iterations of the operators (Optional, default = 1)\n"
"\n"},

{"imageBlackTopHat", (PyCFunction)ImageMorphology_ImageBlackTopHat, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.imageBlackTopHat(input_img=string, output_img=string, tmp_img=string, morph_op_file=string, use_op_file=boolean, op_size=int, gdalformat=string, datatype=int)\n"
"Performs an image morphology black top hat operation. \n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying the name and path of the input file.\n"
":param output_img: is a string specifying the name and path of the output file.\n"
":param tmp_img: is a string specifying the name and path of a tempory file used for intermediate processing step(s).\n"
":param morph_op_file: is a string with the name and path to a .gmtxt file with a square binary matrix specifying the morphology operator\n"
":param use_op_file: is a boolean specifying whether the morph_op_file file is present or whether a square operator (specified via op_size) should be used. (True = morph_op_file, False = op_size)\n"
":param op_size: is a integer specifying the square operator size (only used if use_op_file is False)\n"
":param gdalformat: is a string specifying the GDAL image format (e.g., KEA)\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"},

{"imageWhiteTopHat", (PyCFunction)ImageMorphology_ImageWhiteTopHat, METH_VARARGS | METH_KEYWORDS,
"imagemorphology.imageWhiteTopHat(input_img=string, output_img=string, tmp_img=string, morph_op_file=string, use_op_file=boolean, op_size=int, gdalformat=string, datatype=int)\n"
"Performs an image morphology white top hat operation. \n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string specifying the name and path of the input file.\n"
":param output_img: is a string specifying the name and path of the output file.\n"
":param tmp_img: is a string specifying the name and path of a tempory file used for intermediate processing step(s).\n"
":param morph_op_file: is a string with the name and path to a .gmtxt file with a square binary matrix specifying the morphology operator\n"
":param use_op_file: is a boolean specifying whether the morph_op_file file is present or whether a square operator (specified via op_size) should be used. (True = morph_op_file, False = op_size)\n"
":param op_size: is a integer specifying the square operator size (only used if use_op_file is False)\n"
":param gdalformat: is a string specifying the GDAL image format (e.g., KEA)\n"
":param datatype: is an int containing one of the values from rsgislib.TYPE_*\n"
"\n"},
    
{nullptr}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int ImageMorphology_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int ImageMorphology_clear(PyObject *m)
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_imagemorphology",
        nullptr,
        sizeof(struct ImageMorphologyState),
        ImageMorphologyMethods,
        nullptr,
        ImageMorphology_traverse,
        ImageMorphology_clear,
        nullptr
};

#define INITERROR return nullptr

PyMODINIT_FUNC 
PyInit__imagemorphology(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_imagemorphology(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_imagemorphology", ImageMorphologyMethods);
#endif
    if( pModule == nullptr )
        INITERROR;

    struct ImageMorphologyState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_imagemorphology.error", nullptr, nullptr);
    if( state->error == nullptr )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
