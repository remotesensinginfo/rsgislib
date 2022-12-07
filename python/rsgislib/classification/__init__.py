#!/usr/bin/env python
"""
The classification module provides classification functionality within RSGISLib.
"""

# import the C++ extension into this level
from ._classification import *

from typing import List, Dict

import rsgislib
import rsgislib.imageutils
from rsgislib.imageutils import ImageBandInfo


class ClassSimpleInfoObj(object):
    """
    This is a class to store the information associated within the classification.

    :param id: Output pixel value for this class
    :param file_h5: hdf5 file (from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf) with the training
                   data for the class
    :param red: Red colour for visualisation (0-255)
    :param green: Green colour for visualisation (0-255)
    :param blue: Blue colour for visualisation (0-255)

    """

    def __init__(self, id=None, file_h5=None, red=None, green=None, blue=None):
        """

        :param id: Output pixel value for this class
        :param file_h5: hdf5 file (from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf) with the training
               data for the class
        :param red: Red colour for visualisation (0-255)
        :param green: Green colour for visualisation (0-255)
        :param blue: Blue colour for visualisation (0-255)

        """
        self.id = id
        self.file_h5 = file_h5
        self.red = red
        self.green = green
        self.blue = blue

    def __str__(self):
        return "{}: {}, ({}, {}, {})".format(
            self.id, self.file_h5, self.red, self.green, self.blue
        )

    def __repr__(self):
        return "{}: {}, ({}, {}, {})".format(
            self.id, self.file_h5, self.red, self.green, self.blue
        )


class ClassInfoObj(object):
    """
    This is a class to store the information associated within the classification.

    :param id: Internal unique ID value for this class (must start 0 and be consecutive between the classes)
    :param out_id: External unique ID for ther class which will be used as the output image pixel value, can
                   be any integer.
    :param train_file_h5: hdf5 file (from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf) with the training
                        data for the class
    :param test_file_h5: hdf5 file (from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf) with the testing
                       data for the class
    :param valid_file_h5: hdf5 file (from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf) with the validation
                        data for the class
    :param red: Red colour for visualisation (0-255)
    :param green: Green colour for visualisation (0-255)
    :param blue: Blue colour for visualisation (0-255)

    """

    def __init__(
        self,
        id=None,
        out_id=None,
        train_file_h5=None,
        test_file_h5=None,
        valid_file_h5=None,
        red=None,
        green=None,
        blue=None,
    ):
        """

        :param id: Internal unique ID value for this class (must start 0 and be consecutive between the classes)
        :param out_id: External unique ID for ther class which will be used as the output image pixel value,
                       can be any integer.
        :param train_file_h5: hdf5 file (from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf) with the training
                            data for the class
        :param test_file_h5: hdf5 file (from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf) with the testing
                           data for the class
        :param valid_file_h5: hdf5 file (from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf) with the validation
                            data for the class
        :param red: Red colour for visualisation (0-255)
        :param green: Green colour for visualisation (0-255)
        :param blue: Blue colour for visualisation (0-255)

        """
        self.id = id
        self.out_id = out_id
        self.train_file_h5 = train_file_h5
        self.test_file_h5 = test_file_h5
        self.valid_file_h5 = valid_file_h5
        self.red = red
        self.green = green
        self.blue = blue

    def __str__(self):
        return "{}={}: (Train:{}, Test:{}, Valid:{}), ({}, {}, {})".format(
            self.id,
            self.out_id,
            self.train_file_h5,
            self.test_file_h5,
            self.valid_file_h5,
            self.red,
            self.green,
            self.blue,
        )

    def __repr__(self):
        return "{}={}: (Train:{}, Test:{}, Valid:{}), ({}, {}, {})".format(
            self.id,
            self.out_id,
            self.train_file_h5,
            self.test_file_h5,
            self.valid_file_h5,
            self.red,
            self.green,
            self.blue,
        )


class ClassVecSamplesInfoObj(object):
    """
    This is a class to store the information associated with the classification vector training regions.

    :param id: Unique ID for the class (will probably be the pixel value for this class)
    :param class_name: Unique name for the class.
    :param vec_file: A vector file path with the training samples
    :param vec_lyr: The vector layer name within the vecfile for the training samples.
    :param file_h5: A file path for a HDF5 file where the pixel values for these samples will be stored.
    """

    def __init__(
        self, id=None, class_name=None, vec_file=None, vec_lyr=None, file_h5=None
    ):
        """

        :param id: Unique ID for the class (will probably be the pixel value for this class)
        :param class_name: Unique name for the class.
        :param vec_file: A vector file path with the training samples
        :param vec_lyr: The vector layer name within the vec_file for the training samples.
        :param file_h5: A file path for a HDF5 file where the pixel values for these samples will be stored.

        """
        self.id = id
        self.class_name = class_name
        self.vec_file = vec_file
        self.vec_lyr = vec_lyr
        self.file_h5 = file_h5


class SamplesInfoObj(object):
    """
    This is a class to store the information associated within the classification.

    :param class_name: The name of the class
    :param class_id: Is the classification numeric ID (i.e., output pixel value)
    :param mask_img: The input image mask from which samples are taken
    :param mask_pxl_val: The pixel value within the mask for the class
    :param out_samp_img_file: Temporary file which will store the sampled pixels.
    :param num_samps: The number of samples required.
    :param samples_h5_file: File location for the HDF5 file with the input image values for training.
    :param red: for visualisation red value.
    :param green: for visualisation green value.
    :param blue: for visualisation blue value.

    """

    def __init__(
        self,
        class_name=None,
        class_id=None,
        mask_img=None,
        mask_pxl_val=None,
        out_samp_img_file=None,
        num_samps=None,
        samples_h5_file=None,
        red=None,
        green=None,
        blue=None,
    ):
        """

        :param class_name: The name of the class
        :param class_id: Is the classification numeric ID (i.e., output pixel value)
        :param mask_img: The input image mask from which samples are taken
        :param mask_pxl_val: The pixel value within the mask for the class
        :param out_samp_img_file: Temporary file which will store the sampled pixels.
        :param num_samps: The number of samples required.
        :param samples_h5_file: File location for the HDF5 file with the input image values for training.
        :param red: for visualisation red value.
        :param green: for visualisation green value.
        :param blue: for visualisation blue value.

        """
        self.class_name = class_name
        self.class_id = class_id
        self.mask_img = mask_img
        self.mask_pxl_val = mask_pxl_val
        self.out_samp_img_file = out_samp_img_file
        self.num_samps = num_samps
        self.samples_h5_file = samples_h5_file
        self.red = red
        self.green = green
        self.blue = blue


def get_class_info_dict(
    cls_smpls_info: Dict[str, ClassSimpleInfoObj], smpls_dir: str
) -> Dict[str, ClassInfoObj]:
    """
    A function which converts a dictionary of ClassSimpleInfoObj objects into
    a dictionary of ClassInfoObj objects. This is useful when get_class_training_data
    has been used to extract samples and you then want to use
    create_train_valid_test_sets to split the samples into training, validation and
    testing datasets.

    Note. the output file names for the training, validation and testing datasets
    are as defined as the basename of the input hdf5 samples with either _train,
    _valid or _test appended on the end.

    :param cls_smpls_info: A dict with the class name as the key with a
                           ClassSimpleInfoObj instance as the value.
    :param smpls_dir: the file path for the training, validation and
                      testing datasets.
    :return: A dict with the class name as the key with a ClassInfoObj
             instance as the value.

    """
    import os
    import rsgislib.tools.filetools

    cls_smpls_fnl_info = dict()
    for id, cls_name in enumerate(cls_smpls_info):
        # Get the HDF5 file name / path
        smpls_h5_file = cls_smpls_info[cls_name].file_h5
        # Get the file basename (i.e., without directory path or file extension).
        basename = rsgislib.tools.filetools.get_file_basename(smpls_h5_file)
        train_file = os.path.join(smpls_dir, f"{basename}_train.h5")
        valid_file = os.path.join(smpls_dir, f"{basename}_valid.h5")
        test_file = os.path.join(smpls_dir, f"{basename}_test.h5")

        # Note ClassInfoObj has both an id and out_id. id has to start at 0
        # and be continuous for some of the classification algorithms
        # and therefore out_id will be the output pixel value for the class.
        # Therefore, the existing id will be copied to out_id while the
        # id value will be defined by the index from the enumerate.
        cls_smpls_fnl_info[cls_name] = ClassInfoObj(
            id=id,
            out_id=cls_smpls_info[cls_name].id,
            train_file_h5=train_file,
            test_file_h5=test_file,
            valid_file_h5=valid_file,
            red=cls_smpls_info[cls_name].red,
            green=cls_smpls_info[cls_name].green,
            blue=cls_smpls_info[cls_name].blue,
        )
        print(cls_smpls_fnl_info[cls_name])

    return cls_smpls_fnl_info


def get_class_training_data(
    img_band_info: List[ImageBandInfo],
    class_vec_sample_info: List[ClassVecSamplesInfoObj],
    tmp_dir: str,
    sub_sample: int = None,
    ref_img: str = None,
) -> dict:
    """
    A function to extract training for vector regions for a given input image set.

    :param img_band_info: A list of rsgislib.imageutils.ImageBandInfo objects to
                          define the images and bands of interest.
    :param class_vec_sample_info: List of rsgislib.classification.ClassVecSamplesInfoObj
                                  objects to define the training regions.
    :param tmp_dir: A directory for temporary outputs created during the processing.
    :param sub_sample: If not None then an integer needs to be provided which takes
                       a random selection from the available samples to balance the
                       number of samples used for the classification.
    :param ref_img: A reference image which defines the area of interest, pixel size
                    etc. for the processing. If None then an image will be generated
                    using the input images but the tmp_dir needs to be defined.
    :return: dictionary of ClassSimpleInfoObj objects.

    """
    import rsgislib.tools.filetools
    import rsgislib.tools.utils
    import rsgislib.vectorutils.createrasters
    import rsgislib.zonalstats
    import os
    import random
    import shutil

    # Get valid mask, rasterised to this
    uid_str = rsgislib.tools.utils.uid_generator()
    tmp_lcl_dir = os.path.join(tmp_dir, "get_class_training_data_{}".format(uid_str))
    if not os.path.exists(tmp_lcl_dir):
        os.makedirs(tmp_lcl_dir)

    rasterise_ref_img = ref_img
    if ref_img is None:
        rasterise_ref_img = os.path.join(tmp_lcl_dir, "ref_img_vmsk.kea")
        rsgislib.imageutils.create_valid_mask(
            img_band_info, rasterise_ref_img, "KEA", tmp_lcl_dir
        )

    class_info = dict()
    for class_sample_info in class_vec_sample_info:
        cls_basename = rsgislib.tools.filetools.get_file_basename(
            class_sample_info.file_h5
        )
        out_vec_img = os.path.join(tmp_lcl_dir, "{}_img.kea".format(cls_basename))
        rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
            class_sample_info.vec_file,
            class_sample_info.vec_lyr,
            rasterise_ref_img,
            out_vec_img,
            gdalformat="KEA",
            burn_val=class_sample_info.id,
            datatype=rsgislib.TYPE_16UINT,
            att_column=None,
            use_vec_extent=False,
            thematic=True,
            no_data_val=0,
        )

        if sub_sample is not None:
            out_vec_img_subsample = os.path.join(
                tmp_lcl_dir, "{}_img_subsample.kea".format(cls_basename)
            )
            rsgislib.imageutils.perform_random_pxl_sample_in_mask(
                out_vec_img,
                out_vec_img_subsample,
                "KEA",
                mask_vals=[class_sample_info.id],
                n_samples=sub_sample,
            )
            out_vec_img = out_vec_img_subsample

        rsgislib.zonalstats.extract_zone_img_band_values_to_hdf(
            img_band_info, out_vec_img, class_sample_info.file_h5, class_sample_info.id
        )
        rand_red_val = random.randint(1, 255)
        rand_grn_val = random.randint(1, 255)
        rand_blu_val = random.randint(1, 255)
        class_info[class_sample_info.class_name] = ClassSimpleInfoObj(
            id=class_sample_info.id,
            file_h5=class_sample_info.file_h5,
            red=rand_red_val,
            green=rand_grn_val,
            blue=rand_blu_val,
        )

    shutil.rmtree(tmp_lcl_dir)
    return class_info


def get_class_training_chips_data(
    img_band_info: List[ImageBandInfo],
    class_vec_sample_info: List[ClassVecSamplesInfoObj],
    chip_h_size: int,
    tmp_dir: str,
    ref_img: str = None,
) -> Dict[str, ClassSimpleInfoObj]:
    """
    A function to extract training chips (windows/regions) for vector regions for a given input image set.

    :param img_band_info: A list of rsgislib.imageutils.ImageBandInfo objects to define the images and bands of interest.
    :param class_vec_sample_info: A list of rsgislib.classification.ClassVecSamplesInfoObj objects to define the
                               training regions.
    :param chip_h_size: is half the chip size to be extracted (i.e., 10 with output image chips 21x21, 10 pixels
                        either size of the one of interest).
    :param tmp_dir: A directory for temporary outputs created during the processing.
    :param ref_img: A reference image which defines the area of interest, pixel size etc. for the processing.
                   If None then an image will be generated using the input images but the tmpdir needs to be defined.
    :return: dictionary of ClassSimpleInfoObj objects.

    """
    import rsgislib.tools.filetools
    import rsgislib.tools.utils
    import rsgislib.vectorutils
    import os
    import random
    import shutil

    # Get valid mask, rasterised to this
    uid_str = rsgislib.tools.utils.uid_generator()
    tmp_lcl_dir = os.path.join(
        tmp_dir, "get_class_training_chips_data_{}".format(uid_str)
    )
    if not os.path.exists(tmp_lcl_dir):
        os.makedirs(tmp_lcl_dir)

    rasterise_ref_img = ref_img
    if ref_img is None:
        rasterise_ref_img = os.path.join(tmp_lcl_dir, "ref_img_vmsk.kea")
        rsgislib.imageutils.create_valid_mask(
            img_band_info, rasterise_ref_img, "KEA", tmp_lcl_dir
        )

    classInfo = dict()
    for class_sample_info in class_vec_sample_info:
        cls_basename = rsgislib.tools.filetools.get_file_basename(
            class_sample_info.file_h5
        )
        out_vec_img = os.path.join(tmp_lcl_dir, "{}_img.kea".format(cls_basename))
        rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
            class_sample_info.vecfile,
            class_sample_info.veclyr,
            rasterise_ref_img,
            out_vec_img,
            gdalformat="KEA",
            burn_val=class_sample_info.id,
            datatype=rsgislib.TYPE_16UINT,
            att_column=None,
            use_vec_extent=False,
            thematic=True,
            no_data_val=0,
        )
        rsgislib.imageutils.extractChipZoneImageBandValues2HDF(
            img_band_info,
            out_vec_img,
            class_sample_info.id,
            chip_h_size,
            class_sample_info.file_h5,
        )
        rand_red_val = random.randint(1, 255)
        rand_grn_val = random.randint(1, 255)
        rand_blu_val = random.randint(1, 255)
        classInfo[class_sample_info.classname] = ClassSimpleInfoObj(
            id=class_sample_info.id,
            file_h5=class_sample_info.file_h5,
            red=rand_red_val,
            green=rand_grn_val,
            blue=rand_blu_val,
        )
    shutil.rmtree(tmp_lcl_dir)
    return classInfo


def create_train_valid_test_sets(
    cls_in_info: dict,
    cls_out_info: dict,
    test_sample: int,
    valid_sample: int,
    train_sample: int = None,
    rnd_seed: int = 42,
    datatype: int = None,
):
    """
    A function which takes a dict of rsgislib.classification.ClassSimpleInfoObj
    such as those retrieved from get_class_training_data and a dict of
    rsgislib.classification.ClassInfoObj and creates the train, test, valid
    data samples from a single input file for all the classes. This is a simple
    wrapper function around the split_sample_train_valid_test function making it
    easier to process multiple classes.

    :param cls_in_info: a dict of rsgislib.classification.ClassSimpleInfoObj objects
                        with the input HDF5 file paths which will be split.
    :param cls_out_info: a dict of rsgislib.classification.ClassInfoObj objects
                         which specifies the output paths for the output HDF5 files.
    :param test_sample: The size of the testing sample to be taken.
    :param valid_sample: The size of the validation sample to be taken.
    :param train_sample: The size of the training sample to be taken. If None then the
                         remaining samples are returned.
    :param rnd_seed: The random seed to be used to randomly select the sub-samples.
    :param datatype: is the data type used for the output HDF5 file
                     (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    if cls_in_info.keys() != cls_out_info.keys():
        raise rsgislib.RSGISPyException(
            "The dict keys for the input and output info do not match."
        )

    for cls_name in cls_in_info:
        split_sample_train_valid_test(
            cls_in_info[cls_name].file_h5,
            cls_out_info[cls_name].train_file_h5,
            cls_out_info[cls_name].valid_file_h5,
            cls_out_info[cls_name].test_file_h5,
            test_sample,
            valid_sample,
            train_sample,
            rnd_seed,
            datatype,
        )


def get_class_simple_info_objs(
    cls_info: Dict[str, ClassInfoObj], sel_data: int = rsgislib.CLS_TRAIN_DATA
) -> Dict[str, ClassSimpleInfoObj]:
    """
    A function which creates a dict of ClassSimpleInfoObj objects from a dict
    of ClassInfoObj objects where information such as colours and ids are copied
    across.

    :param cls_info: dict with the class name as the key and a ClassInfoObj as the
                     value.
    :param sel_data: Option to select whether the training, validation or testing
                     hdf5 files are provided as the file_h5 values in the output.
                     Option specified as rsgislib.CLS_*_DATA.
    :return: dict with the class name as key and a ClassSimpleInfoObj object as value.
    """
    out_cls_info = dict()
    for key in cls_info:
        if sel_data == rsgislib.CLS_TRAIN_DATA:
            h5_file = cls_info[key].train_file_h5
        elif sel_data == rsgislib.CLS_VALID_DATA:
            h5_file = cls_info[key].valid_file_h5
        elif sel_data == rsgislib.CLS_TEST_DATA:
            h5_file = cls_info[key].test_file_h5
        else:
            raise Exception(
                "Must select either training, validation or testing dataset"
            )

        out_cls_info[key] = ClassSimpleInfoObj(
            id=cls_info[key].id,
            file_h5=h5_file,
            red=cls_info[key].red,
            green=cls_info[key].green,
            blue=cls_info[key].blue,
        )
    return out_cls_info


def get_num_samples(in_h5_file: str) -> int:
    """
    A function to return the number of samples within the input HDF5 file.

    :param in_h5_file: Input HDF file, probably from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf.
    :return: the number of samples in the hdf5 file.
    """
    import h5py

    f = h5py.File(in_h5_file, "r")
    n_samples = f["DATA/DATA"].shape[0]
    f.close()
    return n_samples


def split_sample_train_valid_test(
    in_h5_file,
    train_h5_file,
    valid_h5_file,
    test_h5_file,
    test_sample,
    valid_sample,
    train_sample=None,
    rnd_seed=42,
    datatype=None,
):
    """
    A function to split a HDF5 samples file (from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf)
    into three (i.e., Training, Validation and Testing).

    :param in_h5_file: Input HDF file, probably from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf.
    :param train_h5_file: Output file with the training data samples (this has the number of samples left following
                          the removal of the test and valid samples if train_sample=None)
    :param valid_h5_file: Output file with the valid data samples.
    :param test_h5_file: Output file with the testing data samples.
    :param test_sample: The size of the testing sample to be taken.
    :param valid_sample: The size of the validation sample to be taken.
    :param train_sample: The size of the training sample to be taken. If None then the remaining samples are returned.
    :param rnd_seed: The random seed to be used to randomly select the sub-samples.
    :param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    import rsgislib.tools.utils
    import rsgislib.tools.filetools
    import rsgislib.zonalstats
    import os

    uid_str = rsgislib.tools.utils.uid_generator()
    out_dir = os.path.split(os.path.abspath(test_h5_file))[0]
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT
    tmp_train_valid_sample_file = os.path.join(
        out_dir, "train_valid_tmp_sample_{}.h5".format(uid_str)
    )
    rsgislib.zonalstats.split_sample_hdf5_file(
        in_h5_file,
        test_h5_file,
        tmp_train_valid_sample_file,
        test_sample,
        rnd_seed,
        datatype,
    )
    if train_sample is not None:
        tmp_train_sample_file = os.path.join(
            out_dir, "train_tmp_sample_{}.h5".format(uid_str)
        )
        rsgislib.zonalstats.split_sample_hdf5_file(
            tmp_train_valid_sample_file,
            valid_h5_file,
            tmp_train_sample_file,
            valid_sample,
            rnd_seed,
            datatype,
        )
        tmp_remain_sample_file = os.path.join(
            out_dir, "remain_tmp_sample_{}.h5".format(uid_str)
        )
        rsgislib.zonalstats.split_sample_hdf5_file(
            tmp_train_sample_file,
            train_h5_file,
            tmp_remain_sample_file,
            train_sample,
            rnd_seed,
            datatype,
        )
        rsgislib.tools.filetools.delete_file_silent(tmp_train_sample_file)
        rsgislib.tools.filetools.delete_file_silent(tmp_remain_sample_file)
    else:
        rsgislib.zonalstats.split_sample_hdf5_file(
            tmp_train_valid_sample_file,
            valid_h5_file,
            train_h5_file,
            valid_sample,
            rnd_seed,
            datatype,
        )
    rsgislib.tools.filetools.delete_file_silent(tmp_train_valid_sample_file)


def split_chip_sample_train_valid_test(
    in_h5_file,
    train_h5_file,
    valid_h5_file,
    test_h5_file,
    test_sample,
    valid_sample,
    train_sample=None,
    rnd_seed=42,
    datatype=None,
):
    """
    A function to split a chip HDF5 samples file (from rsgislib.zonalstats.extract_chip_zone_image_band_values_to_hdf)
    into three (i.e., Training, Validation and Testing).

    :param in_h5_file: Input HDF file, probably from rsgislib.zonalstats.extract_chip_zone_image_band_values_to_hdf.
    :param train_h5_file: Output file with the training data samples (this has the number of samples left following
                          the removal of the test and valid samples if train_sample=None)
    :param valid_h5_file: Output file with the valid data samples.
    :param test_h5_file: Output file with the testing data samples.
    :param test_sample: The size of the testing sample to be taken.
    :param valid_sample: The size of the validation sample to be taken.
    :param train_sample: The size of the training sample to be taken. If None then the remaining samples are returned.
    :param rnd_seed: The random seed to be used to randomly select the sub-samples.
    :param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    import rsgislib.tools.utils
    import rsgislib.tools.filetools
    import rsgislib.zonalstats
    import os

    uid_str = rsgislib.tools.utils.uid_generator()
    out_dir = os.path.split(os.path.abspath(test_h5_file))[0]
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    tmp_train_valid_sample_file = os.path.join(
        out_dir, "train_valid_tmp_sample_{}.h5".format(uid_str)
    )
    rsgislib.zonalstats.split_sample_chip_hdf5_file(
        in_h5_file,
        test_h5_file,
        tmp_train_valid_sample_file,
        test_sample,
        rnd_seed,
        datatype,
    )
    if train_sample is not None:
        tmp_train_sample_file = os.path.join(
            out_dir, "train_tmp_sample_{}.h5".format(uid_str)
        )
        rsgislib.zonalstats.split_sample_chip_hdf5_file(
            tmp_train_valid_sample_file,
            valid_h5_file,
            tmp_train_sample_file,
            valid_sample,
            rnd_seed,
            datatype,
        )
        tmp_remain_sample_file = os.path.join(
            out_dir, "remain_tmp_sample_{}.h5".format(uid_str)
        )
        rsgislib.zonalstats.split_sample_chip_hdf5_file(
            tmp_train_sample_file,
            train_h5_file,
            tmp_remain_sample_file,
            train_sample,
            rnd_seed,
            datatype,
        )
        rsgislib.tools.filetools.delete_file_silent(tmp_train_sample_file)
        rsgislib.tools.filetools.delete_file_silent(tmp_remain_sample_file)
    else:
        rsgislib.zonalstats.split_sample_chip_hdf5_file(
            tmp_train_valid_sample_file,
            valid_h5_file,
            train_h5_file,
            valid_sample,
            rnd_seed,
            datatype,
        )
    rsgislib.tools.filetools.delete_file_silent(tmp_train_valid_sample_file)


def split_chip_sample_ref_train_valid_test(
    in_h5_file,
    train_h5_file,
    valid_h5_file,
    test_h5_file,
    test_sample,
    valid_sample,
    train_sample=None,
    rnd_seed=42,
    datatype=None,
):
    """
    A function to split a chip HDF5 samples file (from rsgislib.zonalstats.extract_chip_zone_image_band_values_to_hdf)
    into three (i.e., Training, Validation and Testing).

    :param in_h5_file: Input HDF file, probably from rsgislib.zonalstats.extract_chip_zone_image_band_values_to_hdf.
    :param train_h5_file: Output file with the training data samples (this has the number of samples left following
                          the removal of the test and valid samples if train_sample=None)
    :param valid_h5_file: Output file with the valid data samples.
    :param test_h5_file: Output file with the testing data samples.
    :param test_sample: The size of the testing sample to be taken.
    :param valid_sample: The size of the validation sample to be taken.
    :param train_sample: The size of the training sample to be taken. If None then the remaining samples are returned.
    :param rnd_seed: The random seed to be used to randomly select the sub-samples.
    :param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    import rsgislib
    import rsgislib.tools.filetools
    import rsgislib.tools.utils
    from rsgislib.zonalstats import split_sample_ref_chip_hdf5_file
    import os

    uid_str = rsgislib.tools.utils.uid_generator()
    out_dir = os.path.split(os.path.abspath(test_h5_file))[0]
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    tmp_train_valid_sample_file = os.path.join(
        out_dir, "train_valid_tmp_sample_{}.h5".format(uid_str)
    )
    split_sample_ref_chip_hdf5_file(
        in_h5_file,
        test_h5_file,
        tmp_train_valid_sample_file,
        test_sample,
        rnd_seed,
        datatype,
    )
    if train_sample is not None:
        tmp_train_sample_file = os.path.join(
            out_dir, "train_tmp_sample_{}.h5".format(uid_str)
        )
        split_sample_ref_chip_hdf5_file(
            tmp_train_valid_sample_file,
            valid_h5_file,
            tmp_train_sample_file,
            valid_sample,
            rnd_seed,
            datatype,
        )
        tmp_remain_sample_file = os.path.join(
            out_dir, "remain_tmp_sample_{}.h5".format(uid_str)
        )
        split_sample_ref_chip_hdf5_file(
            tmp_train_sample_file,
            train_h5_file,
            tmp_remain_sample_file,
            train_sample,
            rnd_seed,
            datatype,
        )
        rsgislib.tools.filetools.delete_file_silent(tmp_train_sample_file)
        rsgislib.tools.filetools.delete_file_silent(tmp_remain_sample_file)
    else:
        split_sample_ref_chip_hdf5_file(
            tmp_train_valid_sample_file,
            valid_h5_file,
            train_h5_file,
            valid_sample,
            rnd_seed,
            datatype,
        )
    rsgislib.tools.filetools.delete_file_silent(tmp_train_valid_sample_file)


def flip_chip_hdf5_file(input_h5_file, output_h5_file, datatype=None):
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

    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    f = h5py.File(input_h5_file, "r")
    n_in_feats = f["DATA/DATA"].shape[0]
    chip_size = f["DATA/DATA"].shape[1]
    n_bands = f["DATA/DATA"].shape[3]

    n_out_feats = n_in_feats * 2

    feat_arr = numpy.zeros(
        [n_out_feats, chip_size, chip_size, n_bands], dtype=numpy.float32
    )

    i_feat = 0
    for n in tqdm.tqdm(range(n_in_feats)):
        numpy.copyto(
            feat_arr[i_feat], numpy.flip(f["DATA/DATA"][n].T, axis=1).T, casting="safe"
        )
        i_feat += 1
        numpy.copyto(
            feat_arr[i_feat], numpy.flip(f["DATA/DATA"][n].T, axis=2).T, casting="safe"
        )
        i_feat += 1

    f.close()

    ######################################################################
    # Create the output HDF5 file and populate with data.
    ######################################################################
    # Chunk size needs to be less than number of features
    if n_in_feats < 250:
        chunk_features = n_in_feats
    else:
        chunk_features = 250
    h5_dtype = rsgislib.get_numpy_char_codes_datatype(datatype)
    fH5Out = h5py.File(output_h5_file, "w")
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset(
        "DATA",
        data=feat_arr,
        chunks=(chunk_features, chip_size, chip_size, n_bands),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = "IMAGE REF TILES".encode()
    fH5Out.close()
    ######################################################################


def flip_ref_chip_hdf5_file(input_h5_file, output_h5_file, datatype=None):
    """
    A function which flips each sample in both the x and y axis. So the
    output file will have double the number of samples as the input file.

    :param input_h5_file: The input HDF5 file for chips extracted from images.
    :param output_h5_file: The output HDF5 file for chips extracted from images.
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    import tqdm
    import h5py
    import numpy
    import rsgislib

    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    f = h5py.File(input_h5_file, "r")
    n_in_feats = f["DATA/REF"].shape[0]
    chip_size = f["DATA/REF"].shape[1]
    n_bands = f["DATA/DATA"].shape[3]

    n_out_feats = n_in_feats * 2

    feat_arr = numpy.zeros(
        [n_out_feats, chip_size, chip_size, n_bands], dtype=numpy.float32
    )
    feat_ref_arr = numpy.zeros([n_out_feats, chip_size, chip_size], dtype=numpy.uint16)

    i_feat = 0
    for n in tqdm.tqdm(range(n_in_feats)):
        numpy.copyto(
            feat_ref_arr[i_feat],
            numpy.flip(f["DATA/REF"][n].T, axis=0).T,
            casting="safe",
        )
        numpy.copyto(
            feat_arr[i_feat], numpy.flip(f["DATA/DATA"][n].T, axis=1).T, casting="safe"
        )
        i_feat += 1
        numpy.copyto(
            feat_ref_arr[i_feat],
            numpy.flip(f["DATA/REF"][n].T, axis=1).T,
            casting="safe",
        )
        numpy.copyto(
            feat_arr[i_feat], numpy.flip(f["DATA/DATA"][n].T, axis=2).T, casting="safe"
        )
        i_feat += 1

    f.close()

    ######################################################################
    # Create the output HDF5 file and populate with data.
    ######################################################################
    # Chunk size needs to be less than number of features
    if n_in_feats < 250:
        chunk_features = n_in_feats
    else:
        chunk_features = 250
    h5_dtype = rsgislib.get_numpy_char_codes_datatype(datatype)
    fH5Out = h5py.File(output_h5_file, "w")
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset(
        "DATA",
        data=feat_arr,
        chunks=(chunk_features, chip_size, chip_size, n_bands),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    dataGrp.create_dataset(
        "REF",
        data=feat_ref_arr,
        chunks=(chunk_features, chip_size, chip_size),
        compression="gzip",
        shuffle=True,
        dtype="H",
    )
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = "IMAGE REF TILES".encode()
    fH5Out.close()
    ######################################################################


def label_pxl_sample_chips(
    sample_pxls_img,
    cls_msk_img,
    out_image,
    gdalformat,
    chip_size,
    cls_lut,
    sample_pxl_img_band=1,
    cls_msk_img_band=1,
):
    """
    A function which labels image pixels based on the proportions of a class within a chip around the
    pixel (can be used in combination with rsgislib.imageutils.assign_random_pxls). It is expected that
    this function will be used when trying to use existing maps to create deep learning chip classification
    training data.

    Pixels are labelled if the proportion of pixels is >= the threshold provided in the LUT. If more than
    one class meets the threshold then the one with the highest proportion is assigned.

    :param sample_pxls_img: The input binary image with the pixel locations (value == 1)
    :param cls_msk_img: The classification image used to assign the output pixel values.
    :param out_image: The output image. Single pixels with the class value will be outputted.
    :param gdalformat: The output image file format.
    :param chip_size: The size of the chip used to identify the class - would probably correspond
                      to the chip size being used for the deep learning classification. Areas used
                      is half the chip size around the pixel (i.e., the pixel from the samples image
                      will be at the centre of the chip).
    :param cls_lut: A dict look up table (LUT) with the thresholds per class for the pixel to be
                    classified as that class.
    :param sample_pxl_img_band: Default 1. The image band in the sample image.
    :param cls_msk_img_band: Default 1. The image band in the sample image.

    .. code:: python

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
    reader = ImageReader(
        inImgs,
        windowxsize=200,
        windowysize=200,
        overlap=img_win_h_size,
        layerselection=inImgBands,
    )
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
            writer = ImageWriter(
                out_image, info=info, firstblock=out_samp_arr, drivername=gdalformat
            )
        else:
            writer.write(out_samp_arr)
    writer.close(calcStats=False)
    rsgislib.rastergis.pop_rat_img_stats(out_image, True, True, True)


def plot_train_data(
    cls1_h5_file,
    cls2_h5_file,
    out_plots_dir,
    cls1_name="Class 1",
    cls2_name="Class 2",
    var_names=None,
):
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
    import rsgislib.tools.utils

    if not os.path.exists(out_plots_dir):
        raise rsgislib.RSGISPyException("The output directory does not exist")

    cls1_h5_obj = h5py.File(cls1_h5_file, "r")
    cls2_h5_obj = h5py.File(cls2_h5_file, "r")

    cls1_data = numpy.array(cls1_h5_obj["DATA/DATA"])
    cls2_data = numpy.array(cls2_h5_obj["DATA/DATA"])

    cls1_n = cls1_data.shape[0]
    cls2_n = cls2_data.shape[0]

    cls1_n_vars = cls1_data.shape[1]
    cls2_n_vars = cls2_data.shape[1]

    if cls1_n_vars != cls2_n_vars:
        raise rsgislib.RSGISPyException(
            "The number of variables must be the same for the two classes."
        )

    if var_names is not None:
        if len(var_names) != cls1_n_vars:
            raise rsgislib.RSGISPyException(
                "The number of variable names provided is not the same as "
                "the number of variables within the HDF5 files."
            )
    else:
        var_names = list()
        for var_n in range(cls1_n_vars):
            var_names.append("Var #{}".format(var_n))

    var_file_names = dict()
    for var_name in var_names:
        var_file_names[var_name] = rsgislib.tools.utils.check_str(
            var_name, rm_non_ascii=True, rm_dashs=True, rm_spaces=True, rm_punc=True
        )

    cls1_data_name = numpy.empty(cls1_n, dtype=numpy.dtype("U255"))
    cls1_data_name[...] = cls1_name

    cls2_data_name = numpy.empty(cls2_n, dtype=numpy.dtype("U255"))
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
            out_file_name = "{}_v_{}.png".format(
                var_file_names[var1], var_file_names[var2]
            )
            out_plot_file = os.path.join(out_plots_dir, out_file_name)

            fig = px.scatter(
                df_data,
                x=var1,
                y=var2,
                color="ClassName",
                marginal_y="histogram",
                marginal_x="histogram",
                title=out_title,
            )

            fig.update_layout(plot_bgcolor="white")
            fig.update_layout(width=1000, height=1000)
            fig.update_xaxes(
                showline=True,
                linewidth=1,
                linecolor="black",
                ticks="inside",
                gridwidth=1,
                gridcolor="Grey",
                mirror=True,
            )
            fig.update_yaxes(
                showline=True,
                linewidth=1,
                linecolor="black",
                ticks="inside",
                col=1,
                gridwidth=1,
                gridcolor="Grey",
                mirror=True,
            )
            fig.write_image(out_plot_file)


def create_acc_pt_sets(
    vec_file: str,
    vec_lyr: str,
    out_vec_file_base: str,
    out_vec_lyr: str,
    cls_col: str,
    n_sets: int,
    sets_col: str = "set_id",
    out_format: str = "GeoJSON",
    out_ext: str = "geojson",
    shuffle_rows: bool = True,
    rnd_seed: int = None,
):
    """
    A function which splits a vector layer into n_sets where a 'class' column is used
    to ensure that there are the same number of samples per 'class' within each set.
    An example of where this function might be used is to split a set of accuracy
    assessment point for assessing the classification accuracy into multiple sets.
    Note, the output vector layers

    :param vec_file: Input vector file/path
    :param vec_lyr: Input vector layer name
    :param out_vec_file_base: The output vector file base name and path. Note,
                              the output file name will be: base{n_set}.out_ext.
                              If you want a character (e.g., underscore) between the
                              basename and the set number then include in the basename.
                              Example, out/path/vec_file_name_
    :param out_vec_lyr: the output vector layer name. The same layer name is used for
                        all the output files.
    :param cls_col: The column in the vector file which has values for the classes
    :param n_sets: The number of sets you want the input vector sorted into.
    :param sets_col: A column added to the output files with an integer representing
                     the set the row belongs to so if vector files are merged again
                     then the set information is not lost. (Default: 'set_id')
    :param out_format: The output vector file format (Default: GeoJSON)
    :param out_ext: the output vector file format extension (Default: gpkg)
    :param shuffle_rows: Boolean specifying whether the vector layer rows should be
                         shuffled before splitting into sets (Default: True)
    :param rnd_seed: If shuffling the rows then this random seed can be used to ensure
                     the shuffling is the same between runs.

    """
    import geopandas
    import math
    import numpy
    import tqdm

    # Read input vector file.
    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    if cls_col not in base_gpdf.columns:
        raise rsgislib.RSGISPyException(
            "The specified class column is not within the input vector layer"
        )

    if shuffle_rows:
        base_gpdf = base_gpdf.sample(frac=1, random_state=rnd_seed).reset_index(
            drop=True
        )
    base_gpdf[sets_col] = numpy.zeros((base_gpdf.shape[0]), dtype=numpy.uint16)

    unq_cls_info = base_gpdf[cls_col].value_counts()

    sets = numpy.arange(1, n_sets + 1)

    for cls, n_vals in tqdm.tqdm(zip(unq_cls_info.index, unq_cls_info)):
        n_vals_per_set = math.floor(n_vals / n_sets)
        sets_arr = numpy.zeros(n_vals, dtype=numpy.uint8)
        s_idx = 0
        e_idx = n_vals_per_set
        for set in sets:
            sets_arr[s_idx:e_idx] = set
            s_idx = e_idx
            e_idx = e_idx + n_vals_per_set
        cls_msk = base_gpdf[cls_col] == cls
        base_gpdf.loc[cls_msk, sets_col] = sets_arr

    for set in tqdm.tqdm(sets):
        out_vec_file = "{}{}.{}".format(out_vec_file_base, set, out_ext)
        set_msk = base_gpdf[sets_col] == set
        tmp_gpdf = base_gpdf[set_msk]

        if out_format == "GPKG":
            tmp_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            tmp_gpdf.to_file(out_vec_file, driver=out_format)


def fill_class_timeseries(
    input_imgs: List[str],
    out_dir: str,
    no_data_val: int = 0,
    gdalformat="KEA",
    out_img_ext="kea",
    out_file_str="_filled",
    double_direction: bool = True,
    recheck_ends: bool = True,
    n_iters: int = 3,
    cls_names_col: str = "class_name",
):
    """
    A function which will fill gaps within a time series of classification
    using values from images either side of the input image. The function
    initially goes through the images in the order they are within the input
    list and then optionally (double_direction) goes back through in the reverse
    direction before optionally rechecking the end images again (recheck_ends).
    At can also be useful to apply this operation a number of times to fill all
    gaps and therefore the n_iters (default 3) allows the the fill to be
    iteratively applied.

    :param input_imgs: list of input file paths
    :param out_dir: output directory where the output images will be written
    :param no_data_val: the no data value used within the input images (i.e.,
                        regions to be filled.
    :param gdalformat: the output file format. (default: KEA)
    :param out_img_ext: the output file extension (default: kea)
    :param out_file_str: an optional addition to the end of the output file name
                         (Default: "_filled")
    :param double_direction: Option to do the fill in both directions. If false then
                             will just go through the order of the input list.
                             (Default: True).
    :param recheck_ends: Option to have the end images rechecked as these a missed
                         if when they are the start image in the sequence.
    :param n_iters: The number of iterations to be applied.
    :param cls_names_col: If a KEA then the column name for the class column. Used
                          for both the input and output images.

    """
    from rios import applier
    from rios import cuiprogress

    import rsgislib.rastergis
    import rsgislib.imageutils
    import rsgislib.tools.filetools

    if len(input_imgs) < 2:
        raise Exception("The list of input images must have at least 2 images.")

    in_img_gdalformat = rsgislib.imageutils.get_gdal_format_name(input_imgs[0])
    if in_img_gdalformat == "KEA":
        cls_clr_info = rsgislib.rastergis.get_rat_colours(
            input_imgs[0], cls_column=cls_names_col
        )
    else:
        cls_clr_info = rsgislib.imageutils.get_colour_tab_info(input_imgs[0])

    if gdalformat == "KEA":
        for pxl_id in cls_clr_info:
            if "classname" not in cls_clr_info[pxl_id]:
                cls_clr_info[pxl_id]["classname"] = f"class_{pxl_id}"

    in_files = applier.FilenameAssociations()
    out_files = applier.FilenameAssociations()
    out_imgs = list()
    for i, img_file in enumerate(input_imgs):
        img_basename = rsgislib.tools.filetools.get_file_basename(img_file)
        in_files.__dict__[f"cls_in_{i}"] = img_file

        out_img = os.path.join(out_dir, f"{img_basename}{out_file_str}.{out_img_ext}")
        out_files.__dict__[f"cls_out_{i}"] = out_img
        out_imgs.append(out_img)

    other_args = applier.OtherInputs()
    other_args.no_data_val = no_data_val
    other_args.double_direction = double_direction
    other_args.recheck_ends = recheck_ends
    other_args.n_imgs = len(input_imgs)
    other_args.n_iters = n_iters

    try:
        import tqdm

        progress_bar = rsgislib.TQDMProgressBar()
    except:
        progress_bar = cuiprogress.GDALProgressBar()

    a_controls = applier.ApplierControls()
    a_controls.progress = progress_bar
    a_controls.drivername = gdalformat
    a_controls.omitPyramids = True
    a_controls.calcStats = False

    # RIOS function to apply classifier
    def _fill_cls_imgs(info, inputs, outputs, other_args):
        """
        Internal function for rios applier.
        """
        for i in range(other_args.n_iters):
            for n in range(other_args.n_imgs):
                nxt_n = n + 1
                if n < other_args.n_imgs - 1:
                    cls_arr = inputs.__dict__[f"cls_in_{n}"]
                    nxt_cls_arr = inputs.__dict__[f"cls_in_{nxt_n}"]
                    cls_arr[cls_arr == other_args.no_data_val] = nxt_cls_arr[
                        cls_arr == other_args.no_data_val
                    ]

            if other_args.double_direction:
                n_vals = list(range(other_args.n_imgs))
                n_vals.reverse()
                for n in n_vals:
                    nxt_n = n - 1
                    if n > 0:
                        cls_arr = inputs.__dict__[f"cls_in_{n}"]
                        nxt_cls_arr = inputs.__dict__[f"cls_in_{nxt_n}"]
                        cls_arr[cls_arr == other_args.no_data_val] = nxt_cls_arr[
                            cls_arr == other_args.no_data_val
                        ]

            if other_args.recheck_ends:
                cls_arr = inputs.__dict__[f"cls_in_0"]
                nxt_cls_arr = inputs.__dict__[f"cls_in_1"]
                cls_arr[cls_arr == other_args.no_data_val] = nxt_cls_arr[
                    cls_arr == other_args.no_data_val
                ]

                n = other_args.n_imgs - 1
                nxt_n = n - 1
                cls_arr = inputs.__dict__[f"cls_in_{n}"]
                nxt_cls_arr = inputs.__dict__[f"cls_in_{nxt_n}"]
                cls_arr[cls_arr == other_args.no_data_val] = nxt_cls_arr[
                    cls_arr == other_args.no_data_val
                ]

        for n in range(other_args.n_imgs):
            outputs.__dict__[f"cls_out_{n}"] = inputs.__dict__[f"cls_in_{n}"]

    applier.apply(_fill_cls_imgs, in_files, out_files, other_args, controls=a_controls)

    for out_img in out_imgs:
        if gdalformat == "KEA":
            rsgislib.rastergis.pop_rat_img_stats(
                out_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
            )
            rsgislib.rastergis.set_class_names_colours(
                out_img, class_names_col=cls_names_col, class_info_dict=cls_clr_info
            )
        else:
            rsgislib.imageutils.pop_thmt_img_stats(
                out_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
            )
            rsgislib.imageutils.define_colour_table(out_img, cls_clr_info)


def convert_cls_smpls_to_pandas_df(h5_file: str, img_file_info: List[ImageBandInfo]):
    """
    A function for converting a HDF5 file generated by extracting data for
    classification using rsgislib to a pandas dataframe.

    :param h5_file: the path to the hdf5 file.
    :param img_file_info: List of rsgislib.imageutils.ImageBandInfo objects which is
                          used to provide the column names.
    :return: pandas.DataFrame

    """
    import h5py
    import pandas

    col_names = list()
    for img_info in img_file_info:
        img_name = img_info.name
        img_bands = img_info.bands
        if len(img_bands) == 1:
            col_names.append(img_name)
        else:
            for band in img_bands:
                col_names.append(f"{img_name}_{band}")

    # Open the dataset
    h5_file_obj = h5py.File(h5_file, "r")
    data_shp = h5_file_obj["DATA/DATA"].shape

    if len(col_names) != data_shp[1]:
        raise rsgislib.RSGISPyException(
            "The number of columns expected and the shape of the hdf5 data do not match"
        )

    h5_data = h5_file_obj["DATA/DATA"]

    data_df = pandas.DataFrame(data=h5_data, columns=col_names)

    h5_file_obj.close()
    return data_df


def convert_mutli_cls_smpls_to_pandas_df(
    cls_smpls_info: Dict[str, ClassSimpleInfoObj],
    img_band_info: List[ImageBandInfo],
    class_id_col: str = "class_id",
):
    """
    A function which takes a dict of ClassSimpleInfoObj objects and creates
    a pandas data frame where the classes are specified using the id within the
    ClassSimpleInfoObj object.

    :param cls_smpls_info: dict where the class name is the value and a
                           ClassSimpleInfoObj object is the value.
    :param img_band_info: a list of rsgislib.imageutils.ImageBandInfo specifying the
                          variable order within the HDF5 files. This is used to generate
                          the column names in the dataframe.
    :return: a pandas dataframe with the first column being the class and those after
             the variables.

    """
    import h5py
    import numpy
    import pandas

    col_names = list()
    for img_info in img_band_info:
        img_name = img_info.name
        img_bands = img_info.bands
        if len(img_bands) == 1:
            col_names.append(img_name)
        else:
            for band in img_bands:
                col_names.append(f"{img_name}_{band}")

    num_vars = 0
    num_vals = 0
    first = True
    for class_info_val in cls_smpls_info.values():
        data_shp = h5py.File(class_info_val.file_h5, "r")["DATA/DATA"].shape
        if first:
            num_vars = data_shp[1]
            first = False
        num_vals += data_shp[0]

    if len(col_names) != num_vars:
        raise rsgislib.RSGISPyException(
            "The number of columns expected and the shape of the hdf5 data do not match"
        )

    data_arr = numpy.zeros([num_vals, num_vars], dtype=float)
    class_arr = numpy.zeros([num_vals], dtype=int)

    row_init = 0
    for key in cls_smpls_info:
        # Open the dataset
        f = h5py.File(cls_smpls_info[key].file_h5, "r")
        num_rows = f["DATA/DATA"].shape[0]
        # Copy data and populate classid array
        data_arr[row_init : (row_init + num_rows)] = f["DATA/DATA"]
        class_arr[row_init : (row_init + num_rows)] = cls_smpls_info[key].id
        row_init += num_rows
        f.close()

    cls_id_df = pandas.DataFrame(
        data=class_arr, columns=[class_id_col], dtype=numpy.int16
    )

    data_vals_df = pandas.DataFrame(
        data=data_arr,
        columns=col_names,
    )

    data_cls_vals_df = pandas.concat((cls_id_df, data_vals_df), axis=1)
    return data_cls_vals_df
