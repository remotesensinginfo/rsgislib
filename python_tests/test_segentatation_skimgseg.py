import os
import pytest

SKIMG_NOT_AVAIL = False
try:
    import skimage.segmentation
except ImportError:
    SKIMG_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
SEGMENT_DATA_DIR = os.path.join(DATA_DIR, "segment")


@pytest.mark.skipif(SKIMG_NOT_AVAIL, reason="scikit-image dependency not available")
def test_perform_felsenszwalb_segmentation(tmp_path):
    import rsgislib.segmentation.skimgseg

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    tmp_dir = os.path.join(tmp_path, "seg_tmp")
    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)
    out_clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    rsgislib.segmentation.skimgseg.perform_felsenszwalb_segmentation(
        input_img,
        out_clumps_img,
        gdalformat="KEA",
        no_data_val=0,
        tmp_dir=tmp_dir,
        calc_stats=True,
        use_pca=True,
        n_pca_bands=3,
        pca_pxl_sample=100,
        scale=1,
        sigma=0.8,
        min_size=20,
    )
    assert os.path.exists(out_clumps_img)


@pytest.mark.skipif(SKIMG_NOT_AVAIL, reason="scikit-image dependency not available")
def test_perform_quickshift_segmentation(tmp_path):
    import rsgislib.segmentation.skimgseg

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    tmp_dir = os.path.join(tmp_path, "seg_tmp")
    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)
    out_clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    rsgislib.segmentation.skimgseg.perform_quickshift_segmentation(
        input_img,
        out_clumps_img,
        gdalformat="KEA",
        no_data_val=0,
        tmp_dir=tmp_dir,
        calc_stats=True,
        use_pca=True,
        pca_pxl_sample=100,
        ratio=1.0,
        kernel_size=5,
        max_dist=10,
        sigma=0,
        convert_to_lab=True,
        random_seed=42,
    )
    assert os.path.exists(out_clumps_img)


@pytest.mark.skipif(SKIMG_NOT_AVAIL, reason="scikit-image dependency not available")
def test_perform_slic_segmentation(tmp_path):
    import rsgislib.segmentation.skimgseg

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    tmp_dir = os.path.join(tmp_path, "seg_tmp")
    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)
    out_clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    rsgislib.segmentation.skimgseg.perform_slic_segmentation(
        input_img,
        out_clumps_img,
        gdalformat="KEA",
        no_data_val=0,
        tmp_dir=tmp_dir,
        calc_stats=True,
        use_pca=True,
        n_pca_bands=3,
        pca_pxl_sample=100,
        n_segments=100,
        compactness=10.0,
        max_iter=10,
        sigma=0,
        spacing=None,
        convert_to_lab=None,
        enforce_connectivity=True,
        min_size_factor=0.5,
        max_size_factor=3,
        slic_zero=False,
    )
    assert os.path.exists(out_clumps_img)


@pytest.mark.skipif(SKIMG_NOT_AVAIL, reason="scikit-image dependency not available")
def test_perform_watershed_segmentation(tmp_path):
    import rsgislib.segmentation.skimgseg

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    tmp_dir = os.path.join(tmp_path, "seg_tmp")
    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)
    out_clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    in_markers_img = os.path.join(
        SEGMENT_DATA_DIR, "sen2_20210527_aber_subset_lcl_min.kea"
    )
    rsgislib.segmentation.skimgseg.perform_watershed_segmentation(
        input_img,
        in_markers_img,
        out_clumps_img,
        gdalformat="KEA",
        no_data_val=0,
        tmp_dir=tmp_dir,
        calc_stats=True,
        use_pca=True,
        n_pca_bands=1,
        pca_pxl_sample=100,
        compactness=0,
        watershed_line=False,
    )
    assert os.path.exists(out_clumps_img)


# TODO need to look at further as takes too long to run.
""" 
@pytest.mark.skipif(SKIMG_NOT_AVAIL, reason="scikit-image dependency not available")
def test_perform_random_walker_segmentation(tmp_path):
    import rsgislib.segmentation.skimgseg
    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    tmp_dir = os.path.join(tmp_path, 'seg_tmp')
    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)
    out_clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    in_markers_img = os.path.join(SEGMENT_DATA_DIR, "sen2_20210527_aber_subset_lcl_min.kea")
    rsgislib.segmentation.skimgseg.perform_random_walker_segmentation(input_img, in_markers_img, out_clumps_img, gdalformat='KEA', no_data_val=0, tmp_dir=tmp_dir, calc_stats=True, use_pca=True, n_pca_bands=1, pca_pxl_sample=100, beta=130, mode='bf', tol=0.001, spacing=None)
    assert os.path.exists(out_clumps_img)
"""
