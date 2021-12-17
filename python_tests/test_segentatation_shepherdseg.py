import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
SEGMENT_DATA_DIR = os.path.join(DATA_DIR, "segmentation")


def test_run_shepherd_segmentation_all_bands(tmp_path):
    import rsgislib.segmentation.shepherdseg

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    tmp_dir = os.path.join(tmp_path, "seg_tmp")
    out_clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    out_mean_img = os.path.join(tmp_path, "sen2_20210527_aber_mean_img.kea")

    rsgislib.segmentation.shepherdseg.run_shepherd_segmentation(
        input_img, out_clumps_img, out_mean_img, tmp_dir
    )

    assert os.path.exists(out_clumps_img) and os.path.exists(out_mean_img)


def test_run_shepherd_segmentation_sub_bands(tmp_path):
    import rsgislib.segmentation.shepherdseg

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    tmp_dir = os.path.join(tmp_path, "seg_tmp")
    out_clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    out_mean_img = os.path.join(tmp_path, "sen2_20210527_aber_mean_img.kea")

    rsgislib.segmentation.shepherdseg.run_shepherd_segmentation(
        input_img, out_clumps_img, out_mean_img, tmp_dir, bands=[8, 9, 7]
    )

    assert os.path.exists(out_clumps_img) and os.path.exists(out_mean_img)


def test_run_shepherd_segmentation_pre_calcd_stats(tmp_path):
    import rsgislib.segmentation.shepherdseg

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    tmp_dir = os.path.join(tmp_path, "seg_tmp")
    out_clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    out_mean_img = os.path.join(tmp_path, "sen2_20210527_aber_mean_img.kea")
    kmeans_centres = os.path.join(SEGMENT_DATA_DIR, "kmeans_centres.gmtxt")
    img_stretch_stats = os.path.join(SEGMENT_DATA_DIR, "img_stch_stats.txt")

    rsgislib.segmentation.shepherdseg.run_shepherd_segmentation_pre_calcd_stats(
        input_img,
        out_clumps_img,
        kmeans_centres,
        img_stretch_stats,
        out_mean_img=out_mean_img,
        tmp_dir=tmp_dir,
        gdalformat="KEA",
        calc_stats=True,
        no_stretch=False,
        no_delete=False,
        min_n_pxls=100,
        dist_thres=100,
        bands=None,
        process_in_mem=False,
    )

    assert os.path.exists(out_clumps_img) and os.path.exists(out_mean_img)


def test_run_shepherd_segmentation_pre_calcd_stats_sub_bands(tmp_path):
    import rsgislib.segmentation.shepherdseg

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    tmp_dir = os.path.join(tmp_path, "seg_tmp")
    out_clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    out_mean_img = os.path.join(tmp_path, "sen2_20210527_aber_mean_img.kea")
    kmeans_centres = os.path.join(SEGMENT_DATA_DIR, "kmeans_centres_bandsub.gmtxt")
    img_stretch_stats = os.path.join(SEGMENT_DATA_DIR, "img_stch_stats_bandsub.txt")

    rsgislib.segmentation.shepherdseg.run_shepherd_segmentation_pre_calcd_stats(
        input_img,
        out_clumps_img,
        kmeans_centres,
        img_stretch_stats,
        out_mean_img=out_mean_img,
        tmp_dir=tmp_dir,
        gdalformat="KEA",
        calc_stats=True,
        no_stretch=False,
        no_delete=False,
        min_n_pxls=100,
        dist_thres=100,
        bands=[8, 9, 7],
        process_in_mem=False,
    )

    assert os.path.exists(out_clumps_img) and os.path.exists(out_mean_img)
