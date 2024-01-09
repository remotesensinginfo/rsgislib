#! /usr/bin/env python

from typing import Dict

import numpy

import rsgislib
import rsgislib.classification

def imblearn_h5_io_smplr(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], imblearn_obj, datatype: int = None
):
    """
    A function which uses imblearn sampling (over or under) instance from the
    imbalanced-learn module to balance samples between all the classes. This
    function reads the data in from a number of HDF5 files and then outputs
    the data to a number of HDF5 files to match the other functions in RSGISLib.

    More information can be found here:
    https://imbalanced-learn.org/stable/over_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param imblearn_obj: a imblearn under or over sampling class instance.
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    import h5py

    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    h5_dtype = rsgislib.get_numpy_char_codes_datatype(datatype)

    if cls_in_info.keys() != cls_out_info.keys():
        raise rsgislib.RSGISPyException(
            "The dict keys for the input and output info do not match."
        )

    for key in cls_in_info:
        if cls_in_info[key].id != cls_out_info[key].id:
            raise rsgislib.RSGISPyException(
                "The input and output class IDs don't match!"
            )

    first = True
    numVars = 0
    numVals = 0
    for class_info_val in cls_in_info.values():
        dataShp = h5py.File(class_info_val.file_h5, "r")["DATA/DATA"].shape
        if first:
            numVars = dataShp[1]
            first = False
        numVals += dataShp[0]

    data_arr = numpy.zeros([numVals, numVars], dtype=float)
    class_arr = numpy.zeros([numVals], dtype=int)

    print("Inputs:")
    rowInit = 0
    for key in cls_in_info:
        # Open the dataset
        f = h5py.File(cls_in_info[key].file_h5, "r")
        numRows = f["DATA/DATA"].shape[0]
        print("\t{}: {}".format(key, numRows))
        # Copy data and populate classid array
        data_arr[rowInit : (rowInit + numRows)] = f["DATA/DATA"]
        class_arr[rowInit : (rowInit + numRows)] = cls_in_info[key].id
        rowInit += numRows
        f.close()

    data_out_arr, class_out_arr = imblearn_obj.fit_resample(data_arr, class_arr)

    print("Outputs:")
    for key in cls_out_info:
        ind_cls_data_arr = data_out_arr[class_out_arr == cls_out_info[key].id]
        print("\t{}: {}".format(key, ind_cls_data_arr.shape[0]))

        chunk_len = 1000
        if ind_cls_data_arr.shape[0] < chunk_len:
            chunk_len = ind_cls_data_arr.shape[0]

        num_vars = ind_cls_data_arr.shape[1]

        f_h5_out = h5py.File(cls_out_info[key].file_h5, "w")
        dataGrp = f_h5_out.create_group("DATA")
        metaGrp = f_h5_out.create_group("META-DATA")
        dataGrp.create_dataset(
            "DATA",
            data=ind_cls_data_arr,
            chunks=(chunk_len, num_vars),
            compression="gzip",
            shuffle=True,
            dtype=h5_dtype,
        )
        describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
        describDS[0] = "imbalanced-learn sampled".encode()
        f_h5_out.close()


def rand_oversample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], rnd_seed: int = 42, datatype: int = None
):
    """
    A function which uses random oversampling from the imbalanced-learn module
    to balance samples between all the classes.

    More information can be found here:
    https://imbalanced-learn.org/stable/over_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param rnd_seed: the random seed used for the analysis
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.over_sampling import RandomOverSampler

    imblearn_obj = RandomOverSampler(random_state=rnd_seed)
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def smote_oversample_smpls(cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], datatype: int = None):
    """
    A function which uses SMOTE oversampling from the imbalanced-learn module
    to balance samples between all the classes. Note this function only works
    with continuous data variables.

    More information can be found here:
    https://imbalanced-learn.org/stable/over_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.over_sampling import SMOTE

    imblearn_obj = SMOTE()
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def adasyn_oversample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], datatype: int = None
):
    """
    A function which uses ADASYN oversampling from the imbalanced-learn module
    to balance samples between all the classes. Note this function only works
    with continuous data variables.

    More information can be found here:
    https://imbalanced-learn.org/stable/over_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.over_sampling import ADASYN

    imblearn_obj = ADASYN()
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def borderline_smote_oversample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], datatype: int = None
):
    """
    A function which uses BorderlineSMOTE oversampling from the imbalanced-learn module
    to balance samples between all the classes. Note this function only works
    with continuous data variables.

    More information can be found here:
    https://imbalanced-learn.org/stable/over_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.over_sampling import BorderlineSMOTE

    imblearn_obj = BorderlineSMOTE()
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def cluster_centroid_undersample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], rnd_seed: int = 42, datatype: int = None
):
    """
    A function which uses ClusterCentroids undersampling from the imbalanced-learn
    module to balance samples between all the classes.

    More information can be found here:
    https://imbalanced-learn.org/stable/under_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param rnd_seed: the random seed used for the analysis
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.under_sampling import ClusterCentroids

    imblearn_obj = ClusterCentroids(random_state=rnd_seed)
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def random_undersample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], rnd_seed: int = 42, datatype: int = None
):
    """
    A function which uses random undersampling from the imbalanced-learn
    module to balance samples between all the classes.

    More information can be found here:
    https://imbalanced-learn.org/stable/under_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param rnd_seed: the random seed used for the analysis
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.under_sampling import RandomUnderSampler

    imblearn_obj = RandomUnderSampler(random_state=rnd_seed)
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def near_miss_undersample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], version:int=1, datatype: int = None
):
    """
    A function which uses NearMiss undersampling from the imbalanced-learn
    module to balance samples between all the classes. Note this function only works
    with continuous data variables.

    More information can be found here:
    https://imbalanced-learn.org/stable/under_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.under_sampling import NearMiss

    imblearn_obj = NearMiss(version=version)
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def edited_near_neigh_undersample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], datatype: int = None
):
    """
    A function which uses EditedNearestNeighbours undersampling from the
    imbalanced-learn module to balance samples between all the classes.

    More information can be found here:
    https://imbalanced-learn.org/stable/under_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.under_sampling import EditedNearestNeighbours

    imblearn_obj = EditedNearestNeighbours()
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def repeat_edited_near_neigh_undersample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], datatype: int = None
):
    """
    A function which uses RepeatedEditedNearestNeighbours undersampling from the
    imbalanced-learn module to balance samples between all the classes.

    More information can be found here:
    https://imbalanced-learn.org/stable/under_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.under_sampling import RepeatedEditedNearestNeighbours

    imblearn_obj = RepeatedEditedNearestNeighbours()
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def all_knn_undersample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], datatype: int = None
):
    """
    A function which uses AllKNN undersampling from the imbalanced-learn
    module to balance samples between all the classes.

    More information can be found here:
    https://imbalanced-learn.org/stable/under_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.under_sampling import AllKNN

    imblearn_obj = AllKNN()
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def condensed_near_neigh_undersample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], rnd_seed: int = 42, datatype: int = None
):
    """
    A function which uses CondensedNearestNeighbour undersampling from the
    imbalanced-learn module to balance samples between all the classes.

    More information can be found here:
    https://imbalanced-learn.org/stable/under_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param rnd_seed: the random seed used for the analysis
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.under_sampling import CondensedNearestNeighbour

    imblearn_obj = CondensedNearestNeighbour(random_state=rnd_seed)
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def one_sided_sel_undersample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], rnd_seed: int = 42, datatype: int = None
):
    """
    A function which uses OneSidedSelection undersampling from the
    imbalanced-learn module to balance samples between all the classes.

    More information can be found here:
    https://imbalanced-learn.org/stable/under_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param rnd_seed: the random seed used for the analysis
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.under_sampling import OneSidedSelection

    imblearn_obj = OneSidedSelection(random_state=rnd_seed)
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def neighbourhood_clean_undersample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], datatype: int = None
):
    """
    A function which uses NeighbourhoodCleaningRule undersampling from the
    imbalanced-learn module to balance samples between all the classes.

    More information can be found here:
    https://imbalanced-learn.org/stable/under_sampling.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.under_sampling import NeighbourhoodCleaningRule

    imblearn_obj = NeighbourhoodCleaningRule()
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def smoteenn_combined_sample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], rnd_seed: int = 42, datatype: int = None
):
    """
    A function which uses SMOTEENN combined under and over sampling from the
    imbalanced-learn module to balance samples between all the classes.

    More information can be found here:
    https://imbalanced-learn.org/stable/combine.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param rnd_seed: the random seed used for the analysis
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.combine import SMOTEENN

    imblearn_obj = SMOTEENN(random_state=rnd_seed)
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)


def smotetomek_combined_sample_smpls(
    cls_in_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], cls_out_info: Dict[str, rsgislib.classification.ClassSimpleInfoObj], rnd_seed: int = 42, datatype: int = None
):
    """
    A function which uses SMOTETomek combined under and over sampling from the
    imbalanced-learn module to balance samples between all the classes.

    More information can be found here:
    https://imbalanced-learn.org/stable/combine.html

    :param cls_in_info: input dict of rsgislib.classification.ClassSimpleInfoObj
                        specifying class names, ids and HDF5 file names.
    :param cls_out_info: output dict of rsgislib.classification.ClassSimpleInfoObj
                         specifying class names, ids and HDF5 file names.
    :param rnd_seed: the random seed used for the analysis
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default) then the output
                     data type will be float32.

    """
    from imblearn.combine import SMOTETomek

    imblearn_obj = SMOTETomek(random_state=rnd_seed)
    imblearn_h5_io_smplr(cls_in_info, cls_out_info, imblearn_obj, datatype=datatype)
