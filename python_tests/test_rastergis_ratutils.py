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


# TODO rsgislib.rastergis.ratutils.create_clumps_shp_bbox
# TODO rsgislib.rastergis.ratutils.calc_dist_between_clumps
# TODO rsgislib.rastergis.ratutils.calc_dist_to_large_clumps
# TODO rsgislib.rastergis.ratutils.define_class_names
# TODO rsgislib.rastergis.ratutils.calc_dist_to_classes
# TODO rsgislib.rastergis.ratutils.set_column_data
# TODO rsgislib.rastergis.ratutils.take_random_sample
# TODO rsgislib.rastergis.ratutils.identify_small_units
# TODO rsgislib.rastergis.ratutils.find_change_clumps_hist_skew_kurt_test
# TODO rsgislib.rastergis.ratutils.find_change_clumps_hist_skew_kurt_test_lower
# TODO rsgislib.rastergis.ratutils.find_change_clumps_hist_skew_kurt_test_upper
# TODO rsgislib.rastergis.ratutils.find_change_clumps_hist_skew_kurt_test_vote_multi_vars
# TODO rsgislib.rastergis.ratutils.find_clumps_within_existing_thresholds
# TODO rsgislib.rastergis.ratutils.calc_plot_gaussian_histo_model
# TODO rsgislib.rastergis.ratutils.set_class_names_colours
