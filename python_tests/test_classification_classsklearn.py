import os
import pytest

H5PY_NOT_AVAIL = False
try:
    import h5py
except ImportError:
    H5PY_NOT_AVAIL = True

SKLEARN_NOT_AVAIL = False
try:
    import sklearn
except ImportError:
    SKLEARN_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
CLASSIFICATION_DATA_DIR = os.path.join(DATA_DIR, "classification")

@pytest.mark.skipif((H5PY_NOT_AVAIL or SKLEARN_NOT_AVAIL), reason="h5py or scikit-learn dependencies not available")
def test_train_sklearn_classifer_gridsearch():
    import rsgislib.classification
    import rsgislib.classification.classsklearn
    from sklearn.model_selection import GridSearchCV
    from sklearn.ensemble import RandomForestClassifier

    grid_search = GridSearchCV(RandomForestClassifier(), {'n_estimators': [10, 20, 30], 'max_depth': [2,3,4]})

    cls_train_info = dict()
    cls_train_info['Forest'] = rsgislib.classification.ClassSimpleInfoObj(id=1, file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"), red=120, green=120, blue=120)
    cls_train_info['Grass'] = rsgislib.classification.ClassSimpleInfoObj(id=2, file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"), red=120, green=120, blue=120)
    cls_train_info['Urban'] = rsgislib.classification.ClassSimpleInfoObj(id=3, file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"), red=120, green=120, blue=120)
    cls_train_info['Water'] = rsgislib.classification.ClassSimpleInfoObj(id=4, file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"), red=120, green=120, blue=120)

    rsgislib.classification.classsklearn.train_sklearn_classifer_gridsearch(cls_train_info, param_search_samp_num=100, grid_search=grid_search)

@pytest.mark.skipif((H5PY_NOT_AVAIL or SKLEARN_NOT_AVAIL), reason="h5py or scikit-learn dependencies not available")
def test_train_sklearn_classifier():
    import rsgislib.classification
    import rsgislib.classification.classsklearn
    from sklearn.ensemble import RandomForestClassifier

    cls_train_info = dict()
    cls_train_info['Forest'] = rsgislib.classification.ClassSimpleInfoObj(id=1, file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"), red=120, green=120, blue=120)
    cls_train_info['Grass'] = rsgislib.classification.ClassSimpleInfoObj(id=2, file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"), red=120, green=120, blue=120)
    cls_train_info['Urban'] = rsgislib.classification.ClassSimpleInfoObj(id=3, file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"), red=120, green=120, blue=120)
    cls_train_info['Water'] = rsgislib.classification.ClassSimpleInfoObj(id=4, file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"), red=120, green=120, blue=120)

    sk_classifier = RandomForestClassifier()

    rsgislib.classification.classsklearn.train_sklearn_classifier(cls_train_info, sk_classifier)

@pytest.mark.skipif((H5PY_NOT_AVAIL or SKLEARN_NOT_AVAIL), reason="h5py or scikit-learn dependencies not available")
def test_apply_sklearn_classifer(tmp_path):
    import rsgislib.imageutils
    import rsgislib.classification
    import rsgislib.classification.classsklearn
    from sklearn.ensemble import RandomForestClassifier

    cls_train_info = dict()
    cls_train_info['Forest'] = rsgislib.classification.ClassSimpleInfoObj(id=1, file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"), red=120, green=120, blue=120)
    cls_train_info['Grass'] = rsgislib.classification.ClassSimpleInfoObj(id=2, file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"), red=120, green=120, blue=120)
    cls_train_info['Urban'] = rsgislib.classification.ClassSimpleInfoObj(id=3, file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"), red=120, green=120, blue=120)
    cls_train_info['Water'] = rsgislib.classification.ClassSimpleInfoObj(id=4, file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"), red=120, green=120, blue=120)

    s2_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    s2_vld_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")

    img_band_info = []
    img_band_info.append(rsgislib.imageutils.ImageBandInfo(s2_img, "s2", [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]))

    sk_classifier = RandomForestClassifier()
    rsgislib.classification.classsklearn.train_sklearn_classifier(cls_train_info, sk_classifier)

    output_img = os.path.join(tmp_path, "out_cls_img.kea")
    out_score_img = os.path.join(tmp_path, "out_cls_scr_img.kea")
    rsgislib.classification.classsklearn.apply_sklearn_classifer(cls_train_info, sk_classifier, s2_vld_img, 1, img_band_info, output_img, "KEA", class_clr_names=True, out_score_img=out_score_img)

    assert os.path.exists(output_img) and os.path.exists(out_score_img)