import os
import pytest

H5PY_NOT_AVAIL = False
try:
    import h5py
except ImportError:
    H5PY_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
ZONALSTATS_DATA_DIR = os.path.join(DATA_DIR, "zonalstats")


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_get_num_samples_pts():
    import rsgislib.classification

    in_h5_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_vals.h5")

    n_smpls = rsgislib.classification.get_num_samples(in_h5_file)

    assert n_smpls == 703


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_get_num_samples_chips():
    import rsgislib.classification

    in_h5_file = os.path.join(
        ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_chip_vals.h5"
    )

    n_smpls = rsgislib.classification.get_num_samples(in_h5_file)

    assert n_smpls == 6


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_get_num_samples_refchips():
    import rsgislib.classification

    in_h5_file = os.path.join(
        ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_refchip_vals.h5"
    )

    n_smpls = rsgislib.classification.get_num_samples(in_h5_file)

    assert n_smpls == 6
