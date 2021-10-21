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

/*
static void FreePythonObjects(std::vector<PyObject*> toFree) {
    std::vector<PyObject*>::iterator iter;
    for(iter = toFree.begin(); iter != toFree.end(); ++iter) {
        Py_XDECREF(*iter);
    }
}
*/

static PyObject *ZonalStats_ImageZoneToHDF(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("vec_file"),
                             RSGIS_PY_C_TEXT("vec_lyr"), RSGIS_PY_C_TEXT("out_h5_file"),
                             RSGIS_PY_C_TEXT("no_prj_warn"), RSGIS_PY_C_TEXT("pxl_in_poly_method"), nullptr};
    const char *pszInputImage, *pszInputVector, *pszInputVecLyr, *pszOutputHDF;
    int pixelInPolyMethod = 1;
    int noProjWarning = false;
    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssss|ii:image_zone_to_hdf", kwlist, &pszInputImage, &pszInputVector, &pszInputVecLyr, &pszOutputHDF, &noProjWarning, &pixelInPolyMethod))
        return nullptr;
    try
    {
        rsgis::cmds::executeZonesImage2HDF5(std::string(pszInputImage), std::string(pszInputVector), std::string(pszInputVecLyr),
                                            std::string(pszOutputHDF), (bool)noProjWarning, pixelInPolyMethod);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }
    
    Py_RETURN_NONE;
}


static PyObject *ZonalStats_ExtractZoneImageValues2HDF(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage;
    const char *pszInputMaskImage;
    const char *pszOutputFile;
    float maskValue = 0;
    int nDataType = 9;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("input_img"), RSGIS_PY_C_TEXT("in_msk_img"),
                             RSGIS_PY_C_TEXT("out_h5_file"), RSGIS_PY_C_TEXT("mask_val"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssf|i:extract_zone_img_values_to_hdf", kwlist, &pszInputImage, &pszInputMaskImage, &pszOutputFile, &maskValue, &nDataType))
    {
        return nullptr;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;

    try
    {
        rsgis::cmds::executeImageRasterZone2HDF(std::string(pszInputImage), std::string(pszInputMaskImage),
                                                std::string(pszOutputFile), maskValue, type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ZonalStats_ExtractZoneImageBandValues2HDF(PyObject *self, PyObject *args, PyObject *keywds)
{
    PyObject *inputImageFileInfoObj;
    const char *pszInputMaskImage;
    const char *pszOutputFile;
    float maskValue = 0;
    int nDataType = 9;

    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_img_info"), RSGIS_PY_C_TEXT("in_msk_img"),
                             RSGIS_PY_C_TEXT("out_h5_file"), RSGIS_PY_C_TEXT("mask_val"),
                             RSGIS_PY_C_TEXT("datatype"), nullptr};

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "Ossf|i:extract_zone_img_band_values_to_hdf", kwlist, &inputImageFileInfoObj, &pszInputMaskImage, &pszOutputFile, &maskValue, &nDataType))
    {
        return nullptr;
    }

    if( !PySequence_Check(inputImageFileInfoObj))
    {
        PyErr_SetString(GETSTATE(self)->error, "First argument (imageFileInfo) must be a sequence");
        return nullptr;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;

    Py_ssize_t nFileInfo = PySequence_Size(inputImageFileInfoObj);
    std::vector<std::pair<std::string, std::vector<unsigned int> > > imageFilesInfo;
    imageFilesInfo.reserve(nFileInfo);
    std::string tmpFileName = "";

    for( Py_ssize_t n = 0; n < nFileInfo; n++ )
    {
        PyObject *o = PySequence_GetItem(inputImageFileInfoObj, n);

        PyObject *pFileName = PyObject_GetAttrString(o, "file_name");
        if( ( pFileName == nullptr ) || ( pFileName == Py_None ) || !RSGISPY_CHECK_STRING(pFileName) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find string attribute \'file_name\'" );
            Py_XDECREF(pFileName);
            Py_DECREF(o);
            return nullptr;
        }

        PyObject *pBands = PyObject_GetAttrString(o, "bands");
        if( ( pBands == nullptr ) || ( pBands == Py_None ) || !PySequence_Check(pBands) )
        {
            PyErr_SetString(GETSTATE(self)->error, "Could not find sequence attribute \'bands\'" );
            Py_DECREF(pFileName);
            Py_XDECREF(pBands);
            Py_DECREF(o);
            return nullptr;
        }

        Py_ssize_t nBands = PySequence_Size(pBands);
        if(nBands == 0)
        {
            PyErr_SetString(GETSTATE(self)->error, "Sequence attribute \'bands\' is empty." );
            Py_DECREF(pFileName);
            Py_DECREF(pBands);
            Py_DECREF(o);
            return nullptr;
        }
        std::vector<unsigned int> bandsVec = std::vector<unsigned int>();
        bandsVec.reserve(nBands);
        for( Py_ssize_t i = 0; i < nBands; i++ )
        {
            PyObject *bO = PySequence_GetItem(pBands, i);
            if( ( bO == nullptr ) || ( bO == Py_None ) || !RSGISPY_CHECK_INT(bO) )
            {
                PyErr_SetString(GETSTATE(self)->error, "Element of 'bands' list was not an integer." );
                Py_XDECREF(bO);

                Py_DECREF(pFileName);
                Py_DECREF(pBands);
                Py_DECREF(o);
                return nullptr;
            }
            bandsVec.push_back(RSGISPY_INT_EXTRACT(bO));
        }

        tmpFileName = std::string(RSGISPY_STRING_EXTRACT(pFileName));
        imageFilesInfo.push_back(std::pair<std::string, std::vector<unsigned int> >(tmpFileName, bandsVec));
    }

    try
    {
        rsgis::cmds::executeImageBandRasterZone2HDF(imageFilesInfo, std::string(pszInputMaskImage),
                                                    std::string(pszOutputFile), maskValue, type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject *ZonalStats_RandomSampleHDF5File(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_h5_file"), RSGIS_PY_C_TEXT("out_h5_file"),
                             RSGIS_PY_C_TEXT("sample"), RSGIS_PY_C_TEXT("rnd_seed"), RSGIS_PY_C_TEXT("datatype"), nullptr};
    const char *pInputH5 = "";
    const char *pOutputH5 = "";
    unsigned int sampleSize = 0;
    int seed = 0;
    int nDataType = 9;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "ssIi|i:random_sample_hdf5_file", kwlist, &pInputH5, &pOutputH5, &sampleSize, &seed, &nDataType))
    {
        return nullptr;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;

    try
    {
        rsgis::cmds::executeRandomSampleH5File(std::string(pInputH5), std::string(pOutputH5), sampleSize, seed, type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}

static PyObject *ZonalStats_SplitSampleHDF5File(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {RSGIS_PY_C_TEXT("in_h5_file"), RSGIS_PY_C_TEXT("out_h5_p1_file"),
                             RSGIS_PY_C_TEXT("out_h5_p2_file"), RSGIS_PY_C_TEXT("sample"),
                             RSGIS_PY_C_TEXT("rnd_seed"), RSGIS_PY_C_TEXT("datatype"), nullptr};
    const char *pInputH5 = "";
    const char *pOutputP1H5 = "";
    const char *pOutputP2H5 = "";
    unsigned int sampleSize = 0;
    int seed = 0;
    int nDataType = 9;

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "sssIi|i:split_sample_hdf5_file", kwlist, &pInputH5, &pOutputP1H5, &pOutputP2H5, &sampleSize, &seed, &nDataType))
    {
        return nullptr;
    }

    rsgis::RSGISLibDataType type = (rsgis::RSGISLibDataType)nDataType;

    try
    {
        rsgis::cmds::executeSplitSampleH5File(std::string(pInputH5), std::string(pOutputP1H5), std::string(pOutputP2H5), sampleSize, seed, type);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return nullptr;
    }

    Py_RETURN_NONE;
}



// Our list of functions in this module
static PyMethodDef ZonalStatsMethods[] = {

{"image_zone_to_hdf", (PyCFunction)ZonalStats_ImageZoneToHDF, METH_VARARGS | METH_KEYWORDS,
"rsgislib.zonalstats.image_zone_to_hdf(input_img, vec_file, vec_lyr, out_h5_file, no_prj_warn=False, pxl_in_poly_method=METHOD_POLYCONTAINSPIXELCENTER)\n"
"Extract the all the pixel values for regions to a HDF5 file (1 column for each image band).\n\n"
"\n"
":param input_img: is a string containing the name of the input image.\n"
":param vec_file: is a string containing the input vector file path.\n"
":param vec_lyr: is a string containing the name of the input vector layer.\n"
":param out_h5_file: is a string containing name of the output HDF file.\n"
":param no_prj_warn: is a bool, specifying whether to skip printing a warning if the vector and image have a different projections.\n"
":param pxl_in_poly_method: is the method for determining if a pixel is included with a polygon of type rsgislib.zonalstats.METHOD_*.\n"
"\n"
"Example::\n"
"\n"
"    from rsgislib import zonalstats\n"
"    inputimage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"    inputvector = './Vectors/injune_p142_crowns_utm.shp'\n"
"    outputHDF = './TestOutputs/InjuneP142.hdf'\n"
"    zonalstats.image_zone_to_hdf(inputimage, inputvector, outputHDF, True, zonalstats.METHOD_POLYCONTAINSPIXELCENTER)\n"
"\n\n"},

{"extract_zone_img_values_to_hdf", (PyCFunction)ZonalStats_ExtractZoneImageValues2HDF, METH_VARARGS | METH_KEYWORDS,
"rsgislib.zonalstats.extract_zone_img_values_to_hdf(input_img, in_msk_img, out_h5_file, mask_val, datatype)\n"
"Extract the all the pixel values for raster regions to a HDF5 file (1 column for each image band).\n"
"\n"
":param input_img: is a string containing the name and path of the input file\n"
":param in_msk_img: is a string containing the name and path of the input image mask file; the mask file must have only 1 image band.\n"
":param out_h5_file: is a string containing the name and path of the output HDF5 file\n"
":param mask_val: is a float containing the value of the pixel within the mask for which values are to be extracted\n"
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\n\n"},

{"extract_zone_img_band_values_to_hdf", (PyCFunction)ZonalStats_ExtractZoneImageBandValues2HDF, METH_VARARGS | METH_KEYWORDS,
"rsgislib.zonalstats.extract_zone_img_band_values_to_hdf(in_img_info, in_msk_img, out_h5_file, mask_val, datatype)\n"
"Extract the all the pixel values for raster regions to a HDF5 file (1 column for each image band).\n"
"Multiple input rasters can be provided and the bands extracted selected.\n"
"\n"
":param in_img_info: is a list of rsgislib::imageutils::ImageBandInfo objects with the file names and list of image bands within that file to be extracted.\n"
":param in_msk_img: is a string containing the name and path of the input image mask file; the mask file must have only 1 image band.\n"
":param out_h5_file: is a string containing the name and path of the output HDF5 file\n"
":param mask_val: is a float containing the value of the pixel within the mask for which values are to be extracted\n"
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\n"
"Example::\n"
"\n"
"   import rsgislib.zonalstats\n"
"   fileInfo = []\n"
"   fileInfo.append(rsgislib.imageutils.ImageBandInfo('InputImg1.kea', 'Image1', [1,3,4]))\n"
"   fileInfo.append(rsgislib.imageutils.ImageBandInfo('InputImg2.kea', 'Image2', [2]))\n"
"   rsgislib.zonalstats.extract_zone_img_band_values_to_hdf(fileInfo, 'ClassMask.kea', 'ForestRefl.h5', 1.0)\n"
"\n\n"},

{"random_sample_hdf5_file", (PyCFunction)ZonalStats_RandomSampleHDF5File, METH_VARARGS | METH_KEYWORDS,
"rsgislib.zonalstats.random_sample_hdf5_file(in_h5_file, out_h5_file, sample, rnd_seed, datatype)\n"
"A function which randomly samples a HDF5 of extracted values.\n"
"\n"
":param in_h5_file: is a string with the path to the input file.\n"
":param out_h5_file: is a string with the path to the output file.\n"
":param sample: is an integer with the number values to be sampled from the input file.\n"
":param rnd_seed: is an integer which seeds the random number generator\n."
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\n\n"
},

{"split_sample_hdf5_file", (PyCFunction)ZonalStats_SplitSampleHDF5File, METH_VARARGS | METH_KEYWORDS,
"rsgislib.zonalstats.split_sample_hdf5_file(in_h5_file, out_h5_p1_file, out_h5_p2_file, sample, rnd_seed, datatype)\n"
"A function which splits samples a HDF5 of extracted values.\n"
"\n"
":param in_h5_file: is a string with the path to the input file.\n"
":param out_h5_p1_file: is a string with the path to the output file.\n"
":param out_h5_p2_file: is a string with the path to the output file.\n"
":param sample: is an integer with the number values to be sampled from the input file.\n"
":param rnd_seed: is an integer which seeds the random number generator\n."
":param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\n\n"
},

    {nullptr}        /* Sentinel */
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
        nullptr,
        sizeof(struct ZonalStatsState),
        ZonalStatsMethods,
        nullptr,
        ZonalStats_traverse,
        ZonalStats_clear,
        nullptr
};

#define INITERROR return nullptr

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
    if( pModule == nullptr )
        INITERROR;

    struct ZonalStatsState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_zonalstats.error", nullptr, nullptr);
    if( state->error == nullptr )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
