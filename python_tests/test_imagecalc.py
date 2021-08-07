import pytest
import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data", "imagecalc")

def test_countPxlsOfVal_band1():
    import rsgislib.imagecalc
    input_img = os.path.join(DATA_DIR, "test_int_pxls.kea")
    val_counts = rsgislib.imagecalc.countPxlsOfVal(input_img, [1,2,3,4], img_band=1)
    assert (val_counts[0] == 614) and (val_counts[1] == 612) and (val_counts[2] == 618) and (val_counts[3] == 656)

def test_countPxlsOfVal_band1_selVals():
    import rsgislib.imagecalc
    input_img = os.path.join(DATA_DIR, "test_int_pxls.kea")
    val_counts = rsgislib.imagecalc.countPxlsOfVal(input_img, [2,1], img_band=1)
    assert (val_counts[0] == 612) and (val_counts[1] == 614)

def test_countPxlsOfVal_all_bands():
    import rsgislib.imagecalc
    input_img = os.path.join(DATA_DIR, "test_int_pxls.kea")
    val_counts = rsgislib.imagecalc.countPxlsOfVal(input_img, [1,2,3,4], img_band=None)
    assert (val_counts[0] == 1890) and (val_counts[1] == 1868) and (val_counts[2] == 1861) and (val_counts[3] == 1881)

def test_getUniqueValues():
    import rsgislib.imagecalc
    input_img = os.path.join(DATA_DIR, "test_int_pxls.kea")
    unq_vals = rsgislib.imagecalc.getUniqueValues(input_img, img_band=1)
    for val in [1,2,3,4]:
        if val not in unq_vals:
            assert False
    assert True

