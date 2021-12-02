import os
from shutil import copy2

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
RASTERGIS_DATA_DIR = os.path.join(DATA_DIR, "rastergis")


def test_get_column_data():
    import rsgislib.rastergis.ratutils
    import numpy

    ref_clumps_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_attref.kea"
    )

    hist_col_vals = rsgislib.rastergis.ratutils.get_column_data(
        ref_clumps_img, "Histogram"
    )

    hist_vals_range_ok = False
    if (
        (numpy.min(hist_col_vals) >= 0)
        and (numpy.max(hist_col_vals) <= 80174)
        and (hist_col_vals.shape[0] == 11949)
    ):
        hist_col_vals = True

    assert hist_col_vals
