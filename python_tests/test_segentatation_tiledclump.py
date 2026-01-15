import os
import sys
import pytest

os_pltform = sys.platform

ON_MACOS = False
if os_pltform == "darwin":
    ON_MACOS = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
SEGMENT_DATA_DIR = os.path.join(DATA_DIR, "segmentation")
IMGCALC_DATA_DIR = os.path.join(DATA_DIR, "imagecalc")

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_perform_clumping_single_thread(tmp_path):
    import rsgislib.segmentation.tiledclump

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    clumps_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.segmentation.tiledclump.perform_clumping_single_thread(
        input_img, clumps_img, tmp_dir=tmp_path, width=500, height=500, gdalformat="KEA"
    )
    assert os.path.exists(clumps_img)


@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_perform_clumping_multi_process(tmp_path):
    import rsgislib.segmentation.tiledclump

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    clumps_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.segmentation.tiledclump.perform_clumping_multi_process(
        input_img,
        clumps_img,
        tmp_dir=tmp_path,
        width=500,
        height=500,
        gdalformat="KEA",
        n_cores=1,
    )
    assert os.path.exists(clumps_img)


@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_perform_union_clumping_single_thread(tmp_path):
    import rsgislib.segmentation.tiledclump

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    clumps_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.segmentation.tiledclump.perform_union_clumping_single_thread(
        input_img,
        input_img,
        clumps_img,
        tmp_dir=tmp_path,
        width=500,
        height=500,
        gdalformat="KEA",
    )
    assert os.path.exists(clumps_img)


@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_perform_union_clumping_multi_process(tmp_path):
    import rsgislib.segmentation.tiledclump

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    clumps_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.segmentation.tiledclump.perform_union_clumping_multi_process(
        input_img,
        input_img,
        clumps_img,
        tmp_dir=tmp_path,
        width=500,
        height=500,
        gdalformat="KEA",
        n_cores=1,
    )
    assert os.path.exists(clumps_img)
