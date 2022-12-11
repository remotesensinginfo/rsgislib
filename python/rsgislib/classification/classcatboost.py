from typing import List

import rsgislib
import rsgislib.imageutils
import rsgislib.rastergis

import h5py
import numpy
from sklearn.metrics import accuracy_score

from rios import applier
from rios import cuiprogress

# from rios import rat

HAVE_CATBOOST = True
try:
    import catboost
except ImportError:
    HAVE_CATBOOST = False


def get_catboost_mdl(mdl_file: str = None, mdl_format: str = "json"):
    """
    A function which creates a default catboost classifier and optionally
    loads an existing model is available.

    :param mdl_file: a path to a saved catboost model.
    :param mdl_format: the format of the model file. cbm is catboost binary and json
                       is JSON format.
    :return: catboost.CatBoostClassifier object

    """
    if not HAVE_CATBOOST:
        raise rsgislib.RSGISPyException("Do not have catboost module installed.")

    cat_cls_mdl = catboost.CatBoostClassifier(verbose=True)
    if mdl_file is not None:
        cat_cls_mdl.load_model(mdl_file, format=mdl_format)
    return cat_cls_mdl


def train_catboost_binary_classifier(
    mdl_cls_obj,
    cls1_train_file: str,
    cls1_valid_file: str,
    cls1_test_file: str,
    cls2_train_file: str,
    cls2_valid_file: str,
    cls2_test_file: str,
    cat_cols: List = None,
    out_mdl_file: str = None,
    verbose_training: bool = False,
):
    """
    A function which trains a catboost classifier with two classes (i.e., binary)
    Class 1 is the class which you are interested in and Class 2 is the 'other class'.

    This function requires that catboost module to be installed.

    :param mdl_cls_obj: The catboost model object.
    :param cls1_train_file: Training samples HDF5 file for the primary class
                            (i.e., the one being classified)
    :param cls1_valid_file: Validation samples HDF5 file for the primary class
                            (i.e., the one being classified)
    :param cls1_test_file: Testing samples HDF5 file for the primary class
                           (i.e., the one being classified)
    :param cls2_train_file: Training samples HDF5 file for the 'other' class
    :param cls2_valid_file: Validation samples HDF5 file for the 'other' class
    :param cls2_test_file: Testing samples HDF5 file for the 'other' class
    :param cat_cols: list of indexes for variables which are categorical.
    :param out_mdl_file: An optional path for a JSON file to save the catboost
                         model to disk.
    :param verbose_training: a boolean to specifying whether a verbose output
                             should be provided during training (Default: False)

    """
    if not HAVE_CATBOOST:
        raise rsgislib.RSGISPyException("Do not have catboost module installed.")

    print("Reading Class 1 Training")
    f = h5py.File(cls1_train_file, "r")
    num_cls1_train_rows = f["DATA/DATA"].shape[0]
    print("num_cls1_train_rows = {}".format(num_cls1_train_rows))
    train_cls1 = numpy.array(f["DATA/DATA"])
    train_cls1_lbl = numpy.ones(num_cls1_train_rows, dtype=int)

    print("Reading Class 1 Validation")
    f = h5py.File(cls1_valid_file, "r")
    num_cls1_valid_rows = f["DATA/DATA"].shape[0]
    print("num_cls1_valid_rows = {}".format(num_cls1_valid_rows))
    valid_cls1 = numpy.array(f["DATA/DATA"])
    valid_cls1_lbl = numpy.ones(num_cls1_valid_rows, dtype=int)

    print("Reading Class 1 Testing")
    f = h5py.File(cls1_test_file, "r")
    num_cls1_test_rows = f["DATA/DATA"].shape[0]
    print("num_cls1_test_rows = {}".format(num_cls1_test_rows))
    test_cls1 = numpy.array(f["DATA/DATA"])
    test_cls1_lbl = numpy.ones(num_cls1_test_rows, dtype=int)

    print("Reading Class 2 Training")
    f = h5py.File(cls2_train_file, "r")
    num_cls2_train_rows = f["DATA/DATA"].shape[0]
    print("num_cls2_train_rows = {}".format(num_cls2_train_rows))
    train_cls2 = numpy.array(f["DATA/DATA"])
    train_cls2_lbl = numpy.zeros(num_cls2_train_rows, dtype=int)

    print("Reading Class 2 Validation")
    f = h5py.File(cls2_valid_file, "r")
    num_cls2_valid_rows = f["DATA/DATA"].shape[0]
    print("num_cls2_valid_rows = {}".format(num_cls2_valid_rows))
    valid_cls2 = numpy.array(f["DATA/DATA"])
    valid_cls2_lbl = numpy.zeros(num_cls2_valid_rows, dtype=int)

    print("Reading Class 2 Testing")
    f = h5py.File(cls2_test_file, "r")
    num_cls2_test_rows = f["DATA/DATA"].shape[0]
    print("num_cls2_test_rows = {}".format(num_cls2_test_rows))
    test_cls2 = numpy.array(f["DATA/DATA"])
    test_cls2_lbl = numpy.zeros(num_cls2_test_rows, dtype=int)

    print("Finished Reading Data")

    vaild_np = numpy.concatenate((valid_cls2, valid_cls1))
    vaild_lbl_np = numpy.concatenate((valid_cls2_lbl, valid_cls1_lbl))

    train_np = numpy.concatenate((train_cls2, train_cls1))
    train_lbl_np = numpy.concatenate((train_cls2_lbl, train_cls1_lbl))

    test_np = numpy.concatenate((test_cls2, test_cls1))
    test_lbl_np = numpy.concatenate((test_cls2_lbl, test_cls1_lbl))

    print("Start Training...")
    mdl_cls_obj.fit(
        train_np,
        train_lbl_np,
        cat_features=cat_cols,
        eval_set=(vaild_np, vaild_lbl_np),
        verbose=verbose_training,
    )
    print("Finished Training")

    pred_test = mdl_cls_obj.predict(data=test_np)
    test_acc = accuracy_score(test_lbl_np, pred_test)
    print("Testing Accuracy: {}".format(test_acc))

    pred_train = mdl_cls_obj.predict(data=train_np)
    train_acc = accuracy_score(train_lbl_np, pred_train)
    print("Training Accuracy: {}".format(train_acc))

    if out_mdl_file is not None:
        mdl_cls_obj.save_model(out_mdl_file, format="json")


def apply_catboost_binary_classifier(
    mdl_cls_obj,
    in_msk_img: str,
    img_mask_val: int,
    img_file_info: List,
    out_class_img: str,
    gdalformat: str = "KEA",
    out_prob_img: str = None,
):
    """
    This function applies a trained binary (i.e., two classes) catboost model.
    The function train_catboost_binary_classifier can be used to train such as model.


    :param mdl_cls_obj: a trained catboost binary model. Can be loaded from disk using
                        the get_catboost_mdl function.
    :param in_msk_img: is an image file providing a mask to specify where should
                       be classified. Simplest mask is all the valid data regions
                       (rsgislib.imageutils.gen_valid_mask)
    :param img_mask_val: the pixel value within the in_msk_img to limit the region
                         to which the classification is applied. Can be used to
                         create a hierarchical classification.
    :param img_file_info: a list of rsgislib.imageutils.ImageBandInfo objects
                          (also used within
                          rsgislib.zonalstats.extract_zone_img_band_values_to_hdf)
                          to identify which images and bands are to be used for the
                          classification so it adheres to the training data.
    :param out_class_img: output image file with the hard classification output.
    :param gdalformat: is the output image format (default: KEA)
    :param out_prob_img: Optional output image which contains the probabilities
                         for the two classes.

    """
    if not HAVE_CATBOOST:
        raise rsgislib.RSGISPyException("Do not have catboost module installed.")

    def _applyCatBClassifier(info, inputs, outputs, otherargs):
        out_class_vals = numpy.zeros_like(inputs.img_mask, dtype=numpy.uint16)
        if otherargs.out_probs:
            out_class_probs = numpy.zeros(
                (2, inputs.img_mask.shape[1], inputs.img_mask.shape[2]),
                dtype=numpy.float32,
            )
        if numpy.any(inputs.img_mask == otherargs.msk_val):
            out_class_vals = out_class_vals.flatten()
            if otherargs.out_probs:
                out_class_probs = out_class_probs.reshape((out_class_vals.shape[0], 2))
            img_mask_vals = inputs.img_mask.flatten()
            class_vars = numpy.zeros(
                (out_class_vals.shape[0], otherargs.num_class_vars), dtype=numpy.float32
            )
            # Array index which can be used to populate the output array following masking etc.
            ID = numpy.arange(img_mask_vals.shape[0])
            class_vars_idx = 0
            for img_file in otherargs.img_file_info:
                img_arr = inputs.__dict__[img_file.name]
                for band in img_file.bands:
                    class_vars[..., class_vars_idx] = img_arr[(band - 1)].flatten()
                    class_vars_idx = class_vars_idx + 1
            class_vars = class_vars[img_mask_vals == otherargs.msk_val]
            ID = ID[img_mask_vals == otherargs.msk_val]

            pred_class = otherargs.classifier.predict(class_vars)
            pred_class[pred_class == 0] = 2
            out_class_vals[ID] = pred_class
            out_class_vals = numpy.expand_dims(
                out_class_vals.reshape(
                    (inputs.img_mask.shape[1], inputs.img_mask.shape[2])
                ),
                axis=0,
            )

            if otherargs.out_probs:
                cls_probs = otherargs.classifier.predict_proba(class_vars)
                out_class_probs[ID] = cls_probs
                out_class_probs = out_class_probs.T
                out_class_probs = out_class_probs.reshape(
                    (
                        2,
                        inputs.img_mask.shape[1],
                        inputs.img_mask.shape[2],
                    )
                )

        outputs.out_image = out_class_vals
        if otherargs.out_probs:
            outputs.out_prob_img = out_class_probs

    infiles = applier.FilenameAssociations()
    infiles.img_mask = in_msk_img
    num_class_vars = 0
    for img_file in img_file_info:
        infiles.__dict__[img_file.name] = img_file.file_name
        num_class_vars = num_class_vars + len(img_file.bands)

    out_probs = out_prob_img is not None

    outfiles = applier.FilenameAssociations()
    outfiles.out_image = out_class_img
    if out_probs:
        outfiles.out_prob_img = out_prob_img
    otherargs = applier.OtherInputs()
    otherargs.classifier = mdl_cls_obj
    otherargs.msk_val = img_mask_val
    otherargs.num_class_vars = num_class_vars
    otherargs.out_probs = out_probs
    otherargs.img_file_info = img_file_info

    try:
        import tqdm

        progress_bar = rsgislib.TQDMProgressBar()
    except:
        progress_bar = cuiprogress.GDALProgressBar()

    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    print("Applying the Classifier")
    applier.apply(
        _applyCatBClassifier, infiles, outfiles, otherargs, controls=aControls
    )
    print("Completed")

    if gdalformat == "KEA":
        rsgislib.rastergis.pop_rat_img_stats(
            out_class_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )
    elif gdalformat == "GTIFF":
        rsgislib.imageutils.pop_thmt_img_stats(
            out_class_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )

    if out_probs:
        rsgislib.imageutils.pop_img_stats(
            out_prob_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )
