import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
SEGMENT_DATA_DIR = os.path.join(DATA_DIR, "segmentation")


def test_run_shepherd_segmentation_all_bands(tmp_path):
    import rsgislib.segmentation.segutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    tmp_dir = os.path.join(tmp_path, 'seg_tmp')
    out_clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    out_mean_img = os.path.join(tmp_path, "sen2_20210527_aber_mean_img.kea")

    rsgislib.segmentation.segutils.run_shepherd_segmentation(input_img, out_clumps_img, out_mean_img, tmp_dir)

    # TODO: check whether the segmentation output will always be the same - probably will.
    assert os.path.exists(out_clumps_img) and os.path.exists(out_mean_img)

def test_run_shepherd_segmentation_sub_bands(tmp_path):
    import rsgislib.segmentation.segutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    tmp_dir = os.path.join(tmp_path, 'seg_tmp')
    out_clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    out_mean_img = os.path.join(tmp_path, "sen2_20210527_aber_mean_img.kea")

    rsgislib.segmentation.segutils.run_shepherd_segmentation(input_img, out_clumps_img, out_mean_img, tmp_dir, bands=[8,9,7])

    # TODO: check whether the segmentation output will always be the same - probably will.
    assert os.path.exists(out_clumps_img) and os.path.exists(out_mean_img)

