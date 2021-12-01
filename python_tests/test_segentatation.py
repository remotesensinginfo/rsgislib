import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
SEGMENT_DATA_DIR = os.path.join(DATA_DIR, "segmentation")
IMGCALC_DATA_DIR = os.path.join(DATA_DIR, "imagecalc")

def test_clump(tmp_path):
    import rsgislib.segmentation.tiledclump

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    clumps_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.segmentation.clump(input_img, clumps_img, gdalformat="KEA", in_memory=False, no_data_val=0, add_to_rat=False)
    assert os.path.exists(clumps_img)


def test_generate_regular_grid(tmp_path):
    import rsgislib.segmentation.tiledclump

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    clumps_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.segmentation.generate_regular_grid(input_img, clumps_img, "KEA", 500, 500, 250)
    assert os.path.exists(clumps_img)