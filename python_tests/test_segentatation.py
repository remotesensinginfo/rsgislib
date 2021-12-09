import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
SEGMENT_DATA_DIR = os.path.join(DATA_DIR, "segment")
IMGCALC_DATA_DIR = os.path.join(DATA_DIR, "imagecalc")


def test_clump(tmp_path):
    import rsgislib.segmentation.tiledclump

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    clumps_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.segmentation.clump(
        input_img,
        clumps_img,
        gdalformat="KEA",
        in_memory=False,
        no_data_val=0,
        add_to_rat=False,
    )
    assert os.path.exists(clumps_img)


# TODO rsgislib.segmentation.label_pixels_from_cluster_centres
# TODO rsgislib.segmentation.relabel_clumps
# TODO rsgislib.segmentation.eliminate_single_pixels
# TODO rsgislib.segmentation.rm_small_clumps
# TODO rsgislib.segmentation.rm_small_clumps_stepwise


def test_union_of_clumps(tmp_path):
    import rsgislib.segmentation.tiledclump

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    clumps_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.segmentation.union_of_clumps(
        [input_img, input_img],
        clumps_img,
        gdalformat="KEA",
        no_data_val=0,
        add_to_rat=False,
    )
    assert os.path.exists(clumps_img)


# TODO rsgislib.segmentation.mean_image
# TODO rsgislib.segmentation.merge_segmentation_tiles


def test_generate_regular_grid(tmp_path):
    import rsgislib.segmentation.tiledclump

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    clumps_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.segmentation.generate_regular_grid(
        input_img, clumps_img, "KEA", 500, 500, 250
    )
    assert os.path.exists(clumps_img)


# TODO rsgislib.segmentation.drop_selected_clumps
# TODO rsgislib.segmentation.find_tile_borders_mask
# TODO rsgislib.segmentation.include_regions_in_clumps
# TODO rsgislib.segmentation.merge_clump_images
# TODO rsgislib.segmentation.merge_equiv_clumps
# TODO rsgislib.segmentation.merge_segments_to_neighbours
