import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")


def test_perform_least_cost_path_calc(tmp_path):
    import rsgislib.imagecalc.leastcostpath

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    start_coord = (257938, 280795)
    stop_coord = (260201, 280445)
    rsgislib.imagecalc.leastcostpath.perform_least_cost_path_calc(
        input_img, output_img, start_coord, stop_coord, gdalformat="KEA", cost_img_band=1
    )
    assert os.path.exists(output_img)
