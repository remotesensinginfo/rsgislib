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
#include "cmds/RSGISCmdZonalStats.h"

/* An exception object for this module */
/* created in the init function */
struct SpecUnmixState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct SpecUnmixState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct SpecUnmixState _state;
#endif


static PyObject *SpecUnmix_ExhconLinearSpecUnmix(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("output_img"),
                             RSGIS_PY_C_TEXT("gdalformat"), RSGIS_PY_C_TEXT("datatype"),
                             RSGIS_PY_C_TEXT("endmember_file"), RSGIS_PY_C_TEXT("step_res"),
                             RSGIS_PY_C_TEXT("gain"), RSGIS_PY_C_TEXT("offset"), nullptr};

    const char *inputImage, *imageFormat, *outputFile, *endmembersFile;
    float lsumGain = 1;
    float lsumOffset = 0;
    float stepResolution;
    int datatype;

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "sssisf|ff:exhcon_linear_spec_unmix", kwlist, &inputImage, &outputFile, &imageFormat, &datatype, &endmembersFile, &stepResolution, &lsumGain, &lsumOffset))
    {
        return nullptr;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)datatype;

    try
    {
        rsgis::cmds::executeExhconLinearSpecUnmix(inputImage, imageFormat, type, lsumGain, lsumOffset, outputFile, endmembersFile, stepResolution);
    }
    catch (rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *SpecUnmix_ExtractAvgEndMembers(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("vec_file"),
                             RSGIS_PY_C_TEXT("vec_lyr"), RSGIS_PY_C_TEXT("out_file"),
                             RSGIS_PY_C_TEXT("pxl_in_poly_method"), nullptr};
    const char *pszInputImage, *pszInputVector, *pszInputVecLyr, *pszOutputMatrix;
    int pixelInPolyMethod = 1;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssss|i:extract_avg_endmembers", kwlist, &pszInputImage, &pszInputVector, &pszInputVecLyr, &pszOutputMatrix, &pixelInPolyMethod))
    {
        return nullptr;
    }

    try
    {
        rsgis::cmds::executeExtractAvgEndMembers(std::string(pszInputImage), std::string(pszInputVector),
                                                 std::string(pszInputVecLyr), std::string(pszOutputMatrix), pixelInPolyMethod);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

// Our list of functions in this module
static PyMethodDef SpecUnmixMethods[] = {

{"exhcon_linear_spec_unmix", (PyCFunction)SpecUnmix_ExhconLinearSpecUnmix, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.specunmixing.exhcon_linear_spec_unmix(input_img, output_img, gdalformat, datatype, endmember_file, step_res, gain, offset)\n"
"Performs an exhaustive constrained linear spectral unmixing of the input image for a set of endmembers.\n"
"\n**Warning. This methods is slow (!!) to execute**\n\n"
"Endmember polygons are extracted using rsgislib.imagecalc.specunmixing.extract_avg_endmembers() where each polygon \n"
"defines an endmember.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input image file\n"
":param output_img: is a string containing the name of the output image file\n"
":param gdalformat: is a string containing the GDAL format for the output file - eg KEA\n"
":param datatype: is an containing one of the values from rsgislib.TYPE_*\n"
":param endmember_file: is a string containing the names of the file containing the end members (.mtxt)\n"
":param step_res: is a float specifying the gap between steps in the search space. Value needs to be between 0 and 1. (i.e., 0.05)\n"
":param gain: is a float specifying a gain which can be applied to the output pixel values (outvalue = offset + (gain * value)). Optional, default = 1.\n"
":param offset: is a float specifying an offset which can be applied to the output pixel values (outvalue = offset + (gain * value)). Optional, default = 0.\n"
"\n"
"Example::\n"
"\n"
"    import rsgislib.imagecalc.specunmixing\n"
"    import rsgislib\n"
"\n"
"    imageLSImage = \"./LS8_20130519_lat52lon42_r24p204_rad_srefstdmdl.kea\"\n"
"    unmixedImage = \"./LS8_20130519_lat52lon42_r24p204_rad_srefstdmdl_unmix.kea\"\n"
"    roiSHP = \"./ROIs.shp\"\n"
"    roiSHPLyr = \"ROIs\"\n"
"    endmembersFile = \"./endmembers\"\n"
"\n"
"    rsgislib.imagecalc.specunmixing.extract_avg_endmembers(imageLSImage, roiSHP, roiSHPLyr, endmembersFile)\n"
"\n"
"    lsumGain = 1.0\n"
"    lsumOffset = 0.0\n"
"\n"
"    endmembersFile = \"./endmembers.mtxt\"\n"
"    stepResolution = 0.1\n"
"    rsgislib.imagecalc.specunmixing.exhcon_linear_spec_unmix(imageLSImage, \"KEA\", rsgislib.TYPE_32FLOAT, unmixedImage, endmembersFile, stepResolution, lsumGain, lsumOffset)\n"
"\n"
"\n"
},

{"extract_avg_endmembers", (PyCFunction)SpecUnmix_ExtractAvgEndMembers, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imagecalc.specunmixing.extract_avg_endmembers(input_img, vec_file, vec_lyr, out_file, pxl_in_poly_method)\n"
"Extract the average endmembers per class which are saved as an appropriate \n"
"matrix file to be used within the linear spectral unmixing commands. \n"
"Each polygon defined is another endmember in the outputted matric file.\n"
"\n"
"Where:\n"
"\n"
":param input_img: is a string containing the name of the input image.\n"
":param vec_file: is a string containing the name of the input vector.\n"
":param vec_lyr: is a string containing the name of the input vector layer name.\n"
":param out_file: is a string containing name of the output matrix file - do not include the file extension (.mtxt).\n"
"                 The file extension will get added during the analysis. Be aware of this when inputting this file \n"
"                 into further processing steps as the file extension (.mtxt) will then be needed. \n"
":param pxl_in_poly_method: is the method for determining if a pixel is included with a polygon of type rsgislib.zonalstats.METHOD_*.\n"
"\n"},



        {nullptr}        /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3

static int SpecUnmix_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int SpecUnmix_clear(PyObject *m)
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_specunmixing",
        nullptr,
        sizeof(struct SpecUnmixState),
        SpecUnmixMethods,
        nullptr,
        SpecUnmix_traverse,
        SpecUnmix_clear,
        nullptr
};

#define INITERROR return nullptr

PyMODINIT_FUNC
PyInit__specunmixing(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_specunmixing(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_specunmixing", SpecUnmixMethods);
#endif
    if( pModule == nullptr )
        INITERROR;

    struct SpecUnmixState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_specunmixing.error", nullptr, nullptr);
    if( state->error == nullptr )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
