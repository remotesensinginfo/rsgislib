#!/usr/bin/env python
"""
The classification module provides classification functionality within RSGISLib.
"""

# import the C++ extension into this level
from ._classification import *

class ClassSimpleInfoObj(object):
    """
    This is a class to store the information associated within the classification.

    :param id: Output pixel value for this class
    :param fileH5: hdf5 file (from rsgislib.imageutils.extractZoneImageBandValues2HDF) with the training
                   data for the class
    :param red: Red colour for visualisation (0-255)
    :param green: Green colour for visualisation (0-255)
    :param blue: Blue colour for visualisation (0-255)

    """

    def __init__(self, id=None, fileH5=None, red=None, green=None, blue=None):
        """

        :param id: Output pixel value for this class
        :param fileH5: hdf5 file (from rsgislib.imageutils.extractZoneImageBandValues2HDF) with the training
               data for the class
        :param red: Red colour for visualisation (0-255)
        :param green: Green colour for visualisation (0-255)
        :param blue: Blue colour for visualisation (0-255)

        """
        self.id = id
        self.fileH5 = fileH5
        self.red = red
        self.green = green
        self.blue = blue


class ClassInfoObj(object):
    """
    This is a class to store the information associated within the classification.

    :param id: Internal unique ID value for this class (must start 0 and be consecutive between the classes)
    :param out_id: External unique ID for ther class which will be used as the output image pixel value, can
                   be any integer.
    :param trainfileH5: hdf5 file (from rsgislib.imageutils.extractZoneImageBandValues2HDF) with the training
                        data for the class
    :param testfileH5: hdf5 file (from rsgislib.imageutils.extractZoneImageBandValues2HDF) with the testing
                       data for the class
    :param validfileH5: hdf5 file (from rsgislib.imageutils.extractZoneImageBandValues2HDF) with the validation
                        data for the class
    :param red: Red colour for visualisation (0-255)
    :param green: Green colour for visualisation (0-255)
    :param blue: Blue colour for visualisation (0-255)

    """

    def __init__(self, id=None, out_id=None, trainfileH5=None, testfileH5=None, validfileH5=None, red=None, green=None,
                 blue=None):
        """

        :param id: Internal unique ID value for this class (must start 0 and be consecutive between the classes)
        :param out_id: External unique ID for ther class which will be used as the output image pixel value,
                       can be any integer.
        :param trainfileH5: hdf5 file (from rsgislib.imageutils.extractZoneImageBandValues2HDF) with the training
                            data for the class
        :param testfileH5: hdf5 file (from rsgislib.imageutils.extractZoneImageBandValues2HDF) with the testing
                           data for the class
        :param validfileH5: hdf5 file (from rsgislib.imageutils.extractZoneImageBandValues2HDF) with the validation
                            data for the class
        :param red: Red colour for visualisation (0-255)
        :param green: Green colour for visualisation (0-255)
        :param blue: Blue colour for visualisation (0-255)

        """
        self.id = id
        self.out_id = out_id
        self.trainfileH5 = trainfileH5
        self.testfileH5 = testfileH5
        self.validfileH5 = validfileH5
        self.red = red
        self.green = green
        self.blue = blue

class ClassVecSamplesInfoObj(object):
    """
    This is a class to store the information associated with the classification vector training regions.

    :param id: Unique ID for the class (will probably be the pixel value for this class)
    :param classname: Unique name for the class.
    :param vecfile: A vector file path with the training samples
    :param veclyr: The vector layer name within the vecfile for the training samples.
    :param fileH5: A file path for a HDF5 file where the pixel values for these samples will be stored.
    """

    def __init__(self, id=None, classname=None,  vecfile=None, veclyr=None, fileH5=None):
        """

        :param id: Unique ID for the class (will probably be the pixel value for this class)
        :param classname: Unique name for the class.
        :param vecfile: A vector file path with the training samples
        :param veclyr: The vector layer name within the vecfile for the training samples.
        :param fileH5: A file path for a HDF5 file where the pixel values for these samples will be stored.

        """
        self.id = id
        self.classname = classname
        self.vecfile = vecfile
        self.veclyr = veclyr
        self.fileH5 = fileH5


class SamplesInfoObj(object):
    """
    This is a class to store the information associated within the classification.

    :param className: The name of the class
    :param classID: Is the classification numeric ID (i.e., output pixel value)
    :param maskImg: The input image mask from which samples are taken
    :param maskPxlVal: The pixel value within the mask for the class
    :param outSampImgFile: Temporary file which will store the sampled pixels.
    :param numSamps: The number of samples required.
    :param samplesH5File: File location for the HDF5 file with the input image values for training.
    :param red: for visualisation red value.
    :param green: for visualisation green value.
    :param blue: for visualisation blue value.

    """

    def __init__(self, className=None, classID=None, maskImg=None, maskPxlVal=None, outSampImgFile=None, numSamps=None,
                 samplesH5File=None, red=None, green=None, blue=None):
        """

        :param className: The name of the class
        :param classID: Is the classification numeric ID (i.e., output pixel value)
        :param maskImg: The input image mask from which samples are taken
        :param maskPxlVal: The pixel value within the mask for the class
        :param outSampImgFile: Temporary file which will store the sampled pixels.
        :param numSamps: The number of samples required.
        :param samplesH5File: File location for the HDF5 file with the input image values for training.
        :param red: for visualisation red value.
        :param green: for visualisation green value.
        :param blue: for visualisation blue value.

        """
        self.className = className
        self.classID = classID
        self.maskImg = maskImg
        self.maskPxlVal = maskPxlVal
        self.outSampImgFile = outSampImgFile
        self.numSamps = numSamps
        self.samplesH5File = samplesH5File
        self.red = red
        self.green = green
        self.blue = blue


def generateTransectAccuracyPts(inputImage, inputLinesShp, outputPtsShp, classImgCol, classImgVecCol,
                                classRefVecCol, lineStep, force=False):
    """ A tool for converting a set of lines in to point transects and 
populating with the information for undertaking an accuracy assessment.

Where:

:param inputImage: is a string specifying the input image file with classification.
:param inputLinesShp: is a string specifying the input lines shapefile path.
:param outputPtsShp: is a string specifying the output points shapefile path.
:param classImgCol: is a string speciyfing the name of the column in the image file containing the class names.
:param classImgVecCol: is a string specifiying the output column in the shapefile for the classified class names.
:param classRefVecCol: is an optional string specifiying an output column in the shapefile which can be used in the
                       accuracy assessment for the reference data.
:param lineStep: is a double specifying the step along the lines between the points
:param force: is an optional boolean specifying whether the output shapefile should be deleted if is already exists
              (True and it will be deleted; Default is False)

    """
    # Import the RSGISLib Image Utils module
    import rsgislib.vectorutils
    rsgislib.vectorutils.createLinesOfPoints(inputLinesShp, outputPtsShp, lineStep, force)
    popClassInfoAccuracyPts(inputImage, outputPtsShp, classImgCol, classImgVecCol, classRefVecCol)


def get_class_training_data(imgBandInfo, classVecSampleInfo, tmpdir, sub_sample=None, refImg=None):
    """
    A function to extract training for vector regions for a given input image set.

    :param imgBandInfo: A list of rsgislib.imageutils.ImageBandInfo objects to define the images and bands of interest.
    :param classVecSampleInfo: A list of rsgislib.classification.ClassVecSamplesInfoObj objects to define the
                               training regions.
    :param tmpdir: A directory for temporary outputs created during the processing.
    :param sub_sample: If not None then an integer needs to be provided which takes a random selection from the
                       available samples to balance the number of samples used for the classification.
    :param refImg: A reference image which defines the area of interest, pixel size etc. for the processing.
                   If None then an image will be generated using the input images but the tmpdir needs to be defined.
    :return: dictionary of ClassSimpleInfoObj objects.

    """
    import rsgislib
    import rsgislib.imageutils
    import rsgislib.vectorutils
    import os
    import os.path
    import random
    import shutil

    # Get valid mask, rasterised to this
    rsgis_utils = rsgislib.RSGISPyUtils()
    uid_str = rsgis_utils.uidGenerator()
    tmp_lcl_dir = os.path.join(tmpdir, "get_class_training_data_{}".format(uid_str))
    if not os.path.exists(tmp_lcl_dir):
        os.makedirs(tmp_lcl_dir)

    rasterise_ref_img = refImg
    if refImg is None:
        rasterise_ref_img = os.path.join(tmp_lcl_dir, "ref_img_vmsk.kea")
        rsgislib.imageutils.create_valid_mask(imgBandInfo, rasterise_ref_img, 'KEA', tmp_lcl_dir)

    classInfo = dict()
    for class_sample_info in classVecSampleInfo:
        cls_basename = rsgis_utils.get_file_basename(class_sample_info.fileH5)
        out_vec_img = os.path.join(tmp_lcl_dir, "{}_img.kea".format(cls_basename))
        rsgislib.vectorutils.rasteriseVecLyr(class_sample_info.vecfile, class_sample_info.veclyr, rasterise_ref_img,
                                             out_vec_img, gdalformat="KEA", burnVal=class_sample_info.id,
                                             datatype=rsgislib.TYPE_16UINT, vecAtt=None, vecExt=False, thematic=True,
                                             nodata=0)

        if sub_sample is not None:
            out_vec_img_subsample = os.path.join(tmp_lcl_dir, "{}_img_subsample.kea".format(cls_basename))
            rsgislib.imageutils.performRandomPxlSampleInMaskLowPxlCount(out_vec_img, out_vec_img_subsample,
                                                                        'KEA', maskvals=[class_sample_info.id],
                                                                        numSamples=sub_sample)
            out_vec_img = out_vec_img_subsample

        rsgislib.imageutils.extractZoneImageBandValues2HDF(imgBandInfo, out_vec_img, class_sample_info.fileH5,
                                                           class_sample_info.id)
        rand_red_val = random.randint(1, 255)
        rand_grn_val = random.randint(1, 255)
        rand_blu_val = random.randint(1, 255)
        classInfo[class_sample_info.classname] = ClassSimpleInfoObj(id=class_sample_info.id,
                                                                    fileH5=class_sample_info.fileH5,
                                                                    red=rand_red_val,
                                                                    green=rand_grn_val,
                                                                    blue=rand_blu_val)

    shutil.rmtree(tmp_lcl_dir)
    return classInfo


def get_class_training_chips_data(imgBandInfo, classVecSampleInfo, chip_h_size, tmpdir, refImg=None):
    """
    A function to extract training chips (windows/regions) for vector regions for a given input image set.

    :param imgBandInfo: A list of rsgislib.imageutils.ImageBandInfo objects to define the images and bands of interest.
    :param classVecSampleInfo: A list of rsgislib.classification.ClassVecSamplesInfoObj objects to define the
                               training regions.
    :param chip_h_size: is half the chip size to be extracted (i.e., 10 with output image chips 21x21, 10 pixels
                        either size of the one of interest).
    :param tmpdir: A directory for temporary outputs created during the processing.
    :param refImg: A reference image which defines the area of interest, pixel size etc. for the processing.
                   If None then an image will be generated using the input images but the tmpdir needs to be defined.
    :return: dictionary of ClassSimpleInfoObj objects.

    """
    import rsgislib
    import rsgislib.imageutils
    import rsgislib.vectorutils
    import os
    import os.path
    import random
    import shutil

    # Get valid mask, rasterised to this
    rsgis_utils = rsgislib.RSGISPyUtils()
    uid_str = rsgis_utils.uidGenerator()
    tmp_lcl_dir = os.path.join(tmpdir, "get_class_training_chips_data_{}".format(uid_str))
    if not os.path.exists(tmp_lcl_dir):
        os.makedirs(tmp_lcl_dir)

    rasterise_ref_img = refImg
    if refImg is None:
        rasterise_ref_img = os.path.join(tmp_lcl_dir, "ref_img_vmsk.kea")
        rsgislib.imageutils.create_valid_mask(imgBandInfo, rasterise_ref_img, 'KEA', tmp_lcl_dir)

    classInfo = dict()
    for class_sample_info in classVecSampleInfo:
        cls_basename = rsgis_utils.get_file_basename(class_sample_info.fileH5)
        out_vec_img = os.path.join(tmp_lcl_dir, "{}_img.kea".format(cls_basename))
        rsgislib.vectorutils.rasteriseVecLyr(class_sample_info.vecfile, class_sample_info.veclyr, rasterise_ref_img,
                                             out_vec_img, gdalformat="KEA", burnVal=class_sample_info.id,
                                             datatype=rsgislib.TYPE_16UINT, vecAtt=None, vecExt=False, thematic=True,
                                             nodata=0)
        rsgislib.imageutils.extractChipZoneImageBandValues2HDF(imgBandInfo, out_vec_img, class_sample_info.id,
                                                               chip_h_size, class_sample_info.fileH5)
        rand_red_val = random.randint(1, 255)
        rand_grn_val = random.randint(1, 255)
        rand_blu_val = random.randint(1, 255)
        classInfo[class_sample_info.classname] = ClassSimpleInfoObj(id=class_sample_info.id,
                                                                    fileH5=class_sample_info.fileH5, red=rand_red_val,
                                                                    green=rand_grn_val, blue=rand_blu_val)
    shutil.rmtree(tmp_lcl_dir)
    return classInfo


def get_num_samples(input_sample_h5_file):
    """
    A function to return the number of samples within the input HDF5 file.

    :param input_sample_h5_file: Input HDF file, probably from rsgislib.imageutils.extractZoneImageBandValues2HDF.
    :return: the number of samples in the hdf5 file.
    """
    import h5py
    f = h5py.File(input_sample_h5_file, 'r')
    n_samples = f['DATA/DATA'].shape[0]
    f.close()
    return n_samples


def split_sample_train_valid_test(input_sample_h5_file, train_h5_file, valid_h5_file, test_h5_file, test_sample,
                                  valid_sample, train_sample=None, rand_seed=42, datatype=None):
    """
    A function to split a HDF5 samples file (from rsgislib.imageutils.extractZoneImageBandValues2HDF)
    into three (i.e., Training, Validation and Testing).

    :param input_sample_h5_file: Input HDF file, probably from rsgislib.imageutils.extractZoneImageBandValues2HDF.
    :param train_h5_file: Output file with the training data samples (this has the number of samples left following
                          the removal of the test and valid samples if train_sample=None)
    :param valid_h5_file: Output file with the valid data samples.
    :param test_h5_file: Output file with the testing data samples.
    :param test_sample: The size of the testing sample to be taken.
    :param valid_sample: The size of the validation sample to be taken.
    :param train_sample: The size of the training sample to be taken. If None then the remaining samples are returned.
    :param rand_seed: The random seed to be used to randomly select the sub-samples.
    :param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    import rsgislib
    import rsgislib.imageutils
    import os
    import os.path
    rsgis_utils = rsgislib.RSGISPyUtils()
    uid_str = rsgis_utils.uidGenerator()
    out_dir = os.path.split(os.path.abspath(test_h5_file))[0]
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT
    tmp_train_valid_sample_file = os.path.join(out_dir, "train_valid_tmp_sample_{}.h5".format(uid_str))
    rsgislib.imageutils.splitSampleHDF5File(input_sample_h5_file, test_h5_file, tmp_train_valid_sample_file,
                                            test_sample, rand_seed, datatype)
    if train_sample is not None:
        tmp_train_sample_file = os.path.join(out_dir, "train_tmp_sample_{}.h5".format(uid_str))
        rsgislib.imageutils.splitSampleHDF5File(tmp_train_valid_sample_file, valid_h5_file, tmp_train_sample_file,
                                                valid_sample, rand_seed, datatype)
        tmp_remain_sample_file = os.path.join(out_dir, "remain_tmp_sample_{}.h5".format(uid_str))
        rsgislib.imageutils.splitSampleHDF5File(tmp_train_sample_file, train_h5_file, tmp_remain_sample_file,
                                                train_sample, rand_seed, datatype)
        os.remove(tmp_train_sample_file)
        os.remove(tmp_remain_sample_file)
    else:
        rsgislib.imageutils.splitSampleHDF5File(tmp_train_valid_sample_file, valid_h5_file, train_h5_file, valid_sample,
                                                rand_seed, datatype)
    os.remove(tmp_train_valid_sample_file)


def split_chip_sample_train_valid_test(input_sample_h5_file, train_h5_file, valid_h5_file, test_h5_file,
                                       test_sample, valid_sample, train_sample=None, rand_seed=42, datatype=None):
    """
    A function to split a chip HDF5 samples file (from rsgislib.imageutils.extractChipZoneImageBandValues2HDF)
    into three (i.e., Training, Validation and Testing).

    :param input_sample_h5_file: Input HDF file, probably from rsgislib.imageutils.extractZoneImageBandValues2HDF.
    :param train_h5_file: Output file with the training data samples (this has the number of samples left following
                          the removal of the test and valid samples if train_sample=None)
    :param valid_h5_file: Output file with the valid data samples.
    :param test_h5_file: Output file with the testing data samples.
    :param test_sample: The size of the testing sample to be taken.
    :param valid_sample: The size of the validation sample to be taken.
    :param train_sample: The size of the training sample to be taken. If None then the remaining samples are returned.
    :param rand_seed: The random seed to be used to randomly select the sub-samples.
    :param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    import rsgislib
    import rsgislib.imageutils
    import os
    import os.path

    rsgis_utils = rsgislib.RSGISPyUtils()
    uid_str = rsgis_utils.uidGenerator()
    out_dir = os.path.split(os.path.abspath(test_h5_file))[0]
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    tmp_train_valid_sample_file = os.path.join(out_dir, "train_valid_tmp_sample_{}.h5".format(uid_str))
    rsgislib.imageutils.splitSampleChipHDF5File(input_sample_h5_file, test_h5_file, tmp_train_valid_sample_file,
                                                test_sample, rand_seed, datatype)
    if train_sample is not None:
        tmp_train_sample_file = os.path.join(out_dir, "train_tmp_sample_{}.h5".format(uid_str))
        rsgislib.imageutils.splitSampleChipHDF5File(tmp_train_valid_sample_file, valid_h5_file, tmp_train_sample_file,
                                                    valid_sample, rand_seed, datatype)
        tmp_remain_sample_file = os.path.join(out_dir, "remain_tmp_sample_{}.h5".format(uid_str))
        rsgislib.imageutils.splitSampleChipHDF5File(tmp_train_sample_file, train_h5_file, tmp_remain_sample_file,
                                                    train_sample, rand_seed, datatype)
        os.remove(tmp_train_sample_file)
        os.remove(tmp_remain_sample_file)
    else:
        rsgislib.imageutils.splitSampleChipHDF5File(tmp_train_valid_sample_file, valid_h5_file, train_h5_file,
                                                    valid_sample, rand_seed, datatype)
    os.remove(tmp_train_valid_sample_file)


def split_chip_sample_ref_train_valid_test(input_sample_h5_file, train_h5_file, valid_h5_file, test_h5_file,
                                           test_sample, valid_sample, train_sample=None, rand_seed=42, datatype=None):
    """
    A function to split a chip HDF5 samples file (from rsgislib.imageutils.extractChipZoneImageBandValues2HDF)
    into three (i.e., Training, Validation and Testing).

    :param input_sample_h5_file: Input HDF file, probably from rsgislib.imageutils.extractZoneImageBandValues2HDF.
    :param train_h5_file: Output file with the training data samples (this has the number of samples left following
                          the removal of the test and valid samples if train_sample=None)
    :param valid_h5_file: Output file with the valid data samples.
    :param test_h5_file: Output file with the testing data samples.
    :param test_sample: The size of the testing sample to be taken.
    :param valid_sample: The size of the validation sample to be taken.
    :param train_sample: The size of the training sample to be taken. If None then the remaining samples are returned.
    :param rand_seed: The random seed to be used to randomly select the sub-samples.
    :param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    import rsgislib
    from rsgislib.imageutils import splitSampleRefChipHDF5File
    import os
    import os.path

    rsgis_utils = rsgislib.RSGISPyUtils()
    uid_str = rsgis_utils.uidGenerator()
    out_dir = os.path.split(os.path.abspath(test_h5_file))[0]
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    tmp_train_valid_sample_file = os.path.join(out_dir, "train_valid_tmp_sample_{}.h5".format(uid_str))
    splitSampleRefChipHDF5File(input_sample_h5_file, test_h5_file, tmp_train_valid_sample_file,
                               test_sample, rand_seed, datatype)
    if train_sample is not None:
        tmp_train_sample_file = os.path.join(out_dir, "train_tmp_sample_{}.h5".format(uid_str))
        splitSampleRefChipHDF5File(tmp_train_valid_sample_file, valid_h5_file, tmp_train_sample_file,
                                   valid_sample, rand_seed, datatype)
        tmp_remain_sample_file = os.path.join(out_dir, "remain_tmp_sample_{}.h5".format(uid_str))
        splitSampleRefChipHDF5File(tmp_train_sample_file, train_h5_file, tmp_remain_sample_file,
                                   train_sample, rand_seed, datatype)
        os.remove(tmp_train_sample_file)
        os.remove(tmp_remain_sample_file)
    else:
        splitSampleRefChipHDF5File(tmp_train_valid_sample_file, valid_h5_file, train_h5_file,
                                   valid_sample, rand_seed, datatype)
    os.remove(tmp_train_valid_sample_file)


def flipChipHDF5File(input_h5_file, output_h5_file, datatype=None):
    """
    A function which flips each sample in both the x and y axis. So the
    output file will have double the number of samples as the input file.

    :param input_h5_file: The input HDF5 file for chips extracted from images.
    :param output_h5_file: The output HDF5 file for chips extracted from images.
    :param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    import tqdm
    import h5py
    import numpy
    import rsgislib
    rsgis_utils = rsgislib.RSGISPyUtils()
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    f = h5py.File(input_h5_file, 'r')
    n_in_feats = f['DATA/DATA'].shape[0]
    chip_size = f['DATA/DATA'].shape[1]
    n_bands = f['DATA/DATA'].shape[3]

    n_out_feats = n_in_feats * 2

    feat_arr = numpy.zeros([n_out_feats, chip_size, chip_size, n_bands], dtype=numpy.float32)

    i_feat = 0
    for n in tqdm.tqdm(range(n_in_feats)):
        numpy.copyto(feat_arr[i_feat], numpy.flip(f['DATA/DATA'][n].T, axis=1).T, casting='safe')
        i_feat += 1
        numpy.copyto(feat_arr[i_feat], numpy.flip(f['DATA/DATA'][n].T, axis=2).T, casting='safe')
        i_feat += 1

    f.close()

    ######################################################################
    # Create the output HDF5 file and populate with data.
    ######################################################################
    h5_dtype = rsgis_utils.getNumpyCharCodesDataType(datatype)
    fH5Out = h5py.File(output_h5_file, 'w')
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset('DATA', data=feat_arr, chunks=(250, chip_size, chip_size, n_bands),
                           compression="gzip", shuffle=True, dtype=h5_dtype)
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = 'IMAGE REF TILES'.encode()
    fH5Out.close()
    ######################################################################


def flipRefChipHDF5File(input_h5_file, output_h5_file, datatype=None):
    """
    A function which flips each sample in both the x and y axis. So the
    output file will have double the number of samples as the input file.

    :param input_h5_file: The input HDF5 file for chips extracted from images.
    :param output_h5_file: The output HDF5 file for chips extracted from images.
    :param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    import tqdm
    import h5py
    import numpy
    import rsgislib
    rsgis_utils = rsgislib.RSGISPyUtils()
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    f = h5py.File(input_h5_file, 'r')
    n_in_feats = f['DATA/REF'].shape[0]
    chip_size = f['DATA/REF'].shape[1]
    n_bands = f['DATA/DATA'].shape[3]

    n_out_feats = n_in_feats * 2

    feat_arr = numpy.zeros([n_out_feats, chip_size, chip_size, n_bands], dtype=numpy.float32)
    feat_ref_arr = numpy.zeros([n_out_feats, chip_size, chip_size], dtype=numpy.uint16)

    i_feat = 0
    for n in tqdm.tqdm(range(n_in_feats)):
        numpy.copyto(feat_ref_arr[i_feat], numpy.flip(f['DATA/REF'][n].T, axis=0).T, casting='safe')
        numpy.copyto(feat_arr[i_feat], numpy.flip(f['DATA/DATA'][n].T, axis=1).T, casting='safe')
        i_feat += 1
        numpy.copyto(feat_ref_arr[i_feat], numpy.flip(f['DATA/REF'][n].T, axis=1).T, casting='safe')
        numpy.copyto(feat_arr[i_feat], numpy.flip(f['DATA/DATA'][n].T, axis=2).T, casting='safe')
        i_feat += 1

    f.close()

    ######################################################################
    # Create the output HDF5 file and populate with data.
    ######################################################################
    h5_dtype = rsgis_utils.getNumpyCharCodesDataType(datatype)
    fH5Out = h5py.File(output_h5_file, 'w')
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset('DATA', data=feat_arr, chunks=(250, chip_size, chip_size, n_bands),
                           compression="gzip", shuffle=True, dtype=h5_dtype)
    dataGrp.create_dataset('REF', data=feat_ref_arr, chunks=(250, chip_size, chip_size),
                           compression="gzip", shuffle=True, dtype='H')
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = 'IMAGE REF TILES'.encode()
    fH5Out.close()
    ######################################################################


def label_pxl_sample_chips(sample_pxls_img, cls_msk_img, output_image, gdalformat, chip_size, cls_lut,
                           sample_pxl_img_band=1, cls_msk_img_band=1):
    """
    A function which labels image pixels based on the proportions of a class within a chip around the
    pixel (can be used in combination with rsgislib.imageutils.assign_random_pxls). It is expected that
    this function will be used when trying to use existing maps to create deep learning chip classification
    training data.

    Pixels are labelled if the proportion of pixels is >= the threshold provided in the LUT. If more than
    one class meets the threshold then the one with the highest proportion is assigned.

    :param sample_pxls_img: The input binary image with the pixel locations (value == 1)
    :param cls_msk_img: The classification image used to assign the output pixel values.
    :param output_image: The output image. Single pixels with the class value will be outputted.
    :param gdalformat: The output image file format.
    :param chip_size: The size of the chip used to identify the class - would probably correspond
                      to the chip size being used for the deep learning classification. Areas used
                      is half the chip size around the pixel (i.e., the pixel from the samples image
                      will be at the centre of the chip).
    :param cls_lut: A dict look up table (LUT) with the thresholds per class for the pixel to be
                    classified as that class.
    :param sample_pxl_img_band: Default 1. The image band in the sample image.
    :param cls_msk_img_band: Default 1. The image band in the sample image.

    Example::

        sample_pxls_img = 'LS5TM_20000108_latn531lonw37_r23p204_osgb_samples.kea'
        cls_msk_img = 'LS5TM_20000108_latn531lonw37_r23p204_osgb_clouds_up.kea'
        output_image = 'LS5TM_20000108_latn531lonw37_r23p204_osgb_samples_lbld.kea'

        cls_lut = dict()
        cls_lut[1] = 0.2
        cls_lut[2] = 0.2
        cls_lut[3] = 0.99

        label_pxl_sample_chips(sample_pxls_img, cls_msk_img, output_image, 'KEA', 21, cls_lut)

    """
    import rsgislib.rastergis
    from rios.imagereader import ImageReader
    from rios.imagewriter import ImageWriter
    import tqdm
    import numpy
    import math

    chip_size_odd = False
    if (chip_size % 2) != 0:
        chip_size_odd = True

    img_win_h_size = math.floor(chip_size / 2)
    img_win_size = chip_size
    n_pxls = img_win_size * img_win_size

    inImgs = list()
    inImgBands = list()
    inImgs.append(sample_pxls_img)
    inImgBands.append([sample_pxl_img_band])
    inImgs.append(cls_msk_img)
    inImgBands.append([cls_msk_img_band])

    writer = None
    reader = ImageReader(inImgs, windowxsize=200, windowysize=200, overlap=img_win_h_size, layerselection=inImgBands)
    for (info, block) in tqdm.tqdm(reader):
        samples_msk_arr = block[0]
        blk_shp = samples_msk_arr.shape

        xSize = blk_shp[2] - (img_win_h_size * 2)
        ySize = blk_shp[1] - (img_win_h_size * 2)
        xRange = numpy.arange(img_win_h_size, img_win_h_size + xSize, 1)
        yRange = numpy.arange(img_win_h_size, img_win_h_size + ySize, 1)
        out_samp_arr = numpy.zeros_like(samples_msk_arr, dtype=numpy.uint8)
        for y in yRange:
            yMin = y - img_win_h_size
            yMax = y + img_win_h_size
            if chip_size_odd:
                yMax += 1
            for x in xRange:
                xMin = x - img_win_h_size
                xMax = x + img_win_h_size
                if chip_size_odd:
                    xMax += 1
                if samples_msk_arr[0][y][x] == 1:
                    img_blk = block[1][0, yMin:yMax, xMin:xMax]
                    uniq_vals, uniq_counts = numpy.unique(img_blk, return_counts=True)
                    uniq_dict = dict(zip(uniq_vals, uniq_counts))
                    first = True
                    for val in uniq_vals:
                        if val in cls_lut:
                            val_prop = uniq_dict[val] / n_pxls
                            if val_prop >= cls_lut[val]:
                                if first:
                                    max_val = val
                                    max_val_prop = val_prop
                                    first = False
                                elif val_prop > max_val_prop:
                                    max_val = val
                                    max_val_prop = val_prop
                    if not first:
                        out_samp_arr[0][y][x] = max_val

        if writer is None:
            writer = ImageWriter(output_image, info=info, firstblock=out_samp_arr, drivername=gdalformat)
        else:
            writer.write(out_samp_arr)
    writer.close(calcStats=False)
    rsgislib.rastergis.populateStats(output_image, True, True, True)


def plot_train_data(cls1_h5_file, cls2_h5_file, out_plots_dir, cls1_name="Class 1", cls2_name="Class 2",
                    var_names=None):
    """
    A function which plots the training data (in HDF5 format) for two classes with histograms for the
    two axis'. Note, this plot only works for training extracted for pixel or clumps and not chip based
    training.

    This function uses the plotly library (https://plotly.com). It saves the plots to disk as PNGs so
    the plotly-orca package is also required.

    :param cls1_h5_file: Input HDF5 file with the training for class 1.
    :param cls2_h5_file: Input HDF5 file with the training for class 2.
    :param out_plots_dir: Output directory for the plots
    :param cls1_name: The name of the first class (Optional, default is 'Class 1')
    :param cls2_name: The name of the second class (Optional, default is 'Class 2')
    :param var_names: An optional list of variable names for the training. Optional,
                      otherwise call 'Var #1', 'Var #2' ... 'Var #N' etc.

    """
    import plotly.express as px
    import pandas
    import h5py
    import tqdm
    import os
    import numpy
    import rsgislib

    rsgis_utils = rsgislib.RSGISPyUtils()

    if not os.path.exists(out_plots_dir):
        raise Exception("The output directory does not exist")

    cls1_h5_obj = h5py.File(cls1_h5_file, 'r')
    cls2_h5_obj = h5py.File(cls2_h5_file, 'r')

    cls1_data = numpy.array(cls1_h5_obj['DATA/DATA'])
    cls2_data = numpy.array(cls2_h5_obj['DATA/DATA'])

    cls1_n = cls1_data.shape[0]
    cls2_n = cls2_data.shape[0]

    cls1_n_vars = cls1_data.shape[1]
    cls2_n_vars = cls2_data.shape[1]

    if cls1_n_vars != cls2_n_vars:
        raise Exception("The number of variables must be the same for the two classes.")

    if var_names is not None:
        if len(var_names) != cls1_n_vars:
            raise Exception("The number of variable names provided is not the same as "
                            "the number of variables within the HDF5 files.")
    else:
        var_names = list()
        for var_n in range(cls1_n_vars):
            var_names.append("Var #{}".format(var_n))

    var_file_names = dict()
    for var_name in var_names:
        var_file_names[var_name] = rsgis_utils.check_str(var_name, rm_non_ascii=True, rm_dashs=True,
                                                         rm_spaces=True, rm_punc=True)

    cls1_data_name = numpy.empty(cls1_n, dtype=numpy.dtype('U255'))
    cls1_data_name[...] = cls1_name

    cls2_data_name = numpy.empty(cls2_n, dtype=numpy.dtype('U255'))
    cls2_data_name[...] = cls2_name

    cls_data = numpy.concatenate([cls1_data, cls2_data])
    cls_data_name = numpy.concatenate([cls1_data_name, cls2_data_name])

    df_data = dict()
    for var_n in range(cls1_n_vars):
        df_data[var_names[var_n]] = cls_data[..., var_n]
    df_data["ClassName"] = cls_data_name

    df = pandas.DataFrame(df_data)

    for var1 in tqdm.tqdm(var_names):
        for var2 in var_names:
            out_title = "{} verses {}".format(var1, var2)
            out_file_name = "{}_v_{}.png".format(var_file_names[var1], var_file_names[var2])
            out_plot_file = os.path.join(out_plots_dir, out_file_name)

            fig = px.scatter(df_data, x=var1, y=var2, color="ClassName", marginal_y="histogram", marginal_x="histogram",
                             title=out_title)

            fig.update_layout(plot_bgcolor='white')
            fig.update_layout(width=1000, height=1000)
            fig.update_xaxes(showline=True, linewidth=1, linecolor='black', ticks="inside", gridwidth=1,
                             gridcolor='Grey', mirror=True)
            fig.update_yaxes(showline=True, linewidth=1, linecolor='black', ticks="inside", col=1, gridwidth=1,
                             gridcolor='Grey', mirror=True)
            fig.write_image(out_plot_file)


