import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")


def test_create_random_int_img(tmp_path):
    import rsgislib.tools.testimages
    import rsgislib.tools.utils

    uid_str = rsgislib.tools.utils.uid_generator()

    output_img = os.path.join(tmp_path, f"out_test_img_{uid_str}.tif")

    rsgislib.tools.testimages.create_random_int_img(
        output_img,
        n_bands=1,
        x_size=100,
        y_size=250,
        out_vals=[1, 2, 3, 4],
        gdalformat="GTIFF",
        datatype=rsgislib.TYPE_8UINT,
        calc_stats=True,
        tmp_path=tmp_path,
    )

    assert os.path.exists(output_img)
