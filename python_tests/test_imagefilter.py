import os
import rsgislib

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")


def test_apply_median_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_median_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_mean_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_mean_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_min_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_min_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_max_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_max_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_mode_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_mode_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_stddev_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_stddev_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_range_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_range_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_mean_diff_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_mean_diff_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_mean_diff_abs_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_mean_diff_abs_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_total_diff_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_total_diff_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_median_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_median_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_total_diff_abs_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_total_diff_abs_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_coeff_of_var_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_coeff_of_var_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_total_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_total_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_norm_var_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_norm_var_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_norm_var_sqrt_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_norm_var_sqrt_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_norm_var_ln_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_norm_var_ln_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_texture_var_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_texture_var_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_kuwahara_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_kuwahara_filter(
        input_img, output_img, 3, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_sobel_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_sobel_filter(
        input_img, output_img, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_sobel_x_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_sobel_x_filter(
        input_img, output_img, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_sobel_y_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_sobel_y_filter(
        input_img, output_img, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_prewitt_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_prewitt_filter(
        input_img, output_img, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_prewitt_x_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_prewitt_x_filter(
        input_img, output_img, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_prewitt_y_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_prewitt_y_filter(
        input_img, output_img, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_gaussian_smooth_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_gaussian_smooth_filter(
        input_img, output_img, 3, 1, 1, 45, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_gaussian_1st_deriv_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_gaussian_1st_deriv_filter(
        input_img, output_img, 3, 1, 1, 45, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_gaussian_2nd_deriv_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_gaussian_2nd_deriv_filter(
        input_img, output_img, 3, 1, 1, 45, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_laplacian_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_laplacian_filter(
        input_img, output_img, 3, 1, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_apply_lee_filter(tmp_path):
    import rsgislib.imagefilter

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.apply_lee_filter(
        input_img, output_img, 3, 2, "GTIFF", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_leung_malik_filter_bank(tmp_path):
    import rsgislib.imagefilter
    import glob

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    out_img_base = os.path.join(tmp_path, "filter_output")
    rsgislib.imagefilter.leung_malik_filter_bank(
        input_img, out_img_base, "GTIFF", "tif", rsgislib.TYPE_16UINT
    )

    imgs = glob.glob("{}*.tif".format(out_img_base))

    assert len(imgs) == 48


def test_apply_filters(tmp_path):
    import rsgislib.imagefilter
    import glob

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    out_img_base = os.path.join(tmp_path, "filter_output")

    filters = []
    filters.append(
        rsgislib.imagefilter.FilterParameters(
            filter_type="GaussianSmooth",
            file_ending="gausmooth",
            size=3,
            stddev_x=1.0,
            stddev_y=1,
            angle=0.0,
        )
    )
    filters.append(
        rsgislib.imagefilter.FilterParameters(
            filter_type="Gaussian1st",
            file_ending="gau1st",
            size=3,
            stddev_x=1.0,
            stddev_y=1,
            angle=0.0,
        )
    )
    filters.append(
        rsgislib.imagefilter.FilterParameters(
            filter_type="Gaussian2nd",
            file_ending="gau2st",
            size=3,
            stddev_x=1.0,
            stddev_y=1,
            angle=0.0,
        )
    )
    # Apply filters
    rsgislib.imagefilter.apply_filters(
        input_img, out_img_base, filters, "GTIFF", "tif", rsgislib.TYPE_16UINT
    )

    imgs = glob.glob("{}*.tif".format(out_img_base))

    assert len(imgs) == 3


def test_perform_tiled_img_multi_filter(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_insts = [
        rsgislib.imagefilter.tiledfilter.RSGISMedianFilter(
            3, "GTIFF", rsgislib.TYPE_16UINT
        ),
        rsgislib.imagefilter.tiledfilter.RSGISMeanFilter(
            3, "GTIFF", rsgislib.TYPE_16UINT
        ),
    ]

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_imgs = [
        os.path.join(tmp_path, "filter_output1.tif"),
        os.path.join(tmp_path, "filter_output2.tif"),
    ]
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_multi_filter(
        input_img,
        output_imgs,
        filter_insts,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_imgs[0]) and os.path.exists(output_imgs[1])


def test_perform_tiled_img_filter_median(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISMedianFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_mean(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISMeanFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_min(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISMinFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_max(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISMaxFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_mode(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISModeFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_stddev(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISStdDevFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_range(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISRangeFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_mean_dif(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISMeanDiffFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_mean_diff_abs(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISMeanDiffAbsFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_total_diff(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISTotalDiffFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_total_diff_abs(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISTotalDiffAbsFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_coeff_of_var(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISCoeffOfVarFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_total(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISTotalFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_norm_var(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISNormVarFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_norm_var_sqrt(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISNormVarSqrtFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_norm_var_ln(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISNormVarLnFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_texture_var(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISTextureVarFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_kuwahara(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISKuwaharaFilter(
        3, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_sobel(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISSobelFilter(
        "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_sobel_x(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISSobelXFilter(
        "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_sobel_y(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISSobelYFilter(
        "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_prewitt(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISPrewittFilter(
        "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_prewitt_x(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISPrewittXFilter(
        "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_prewitt_y(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISPrewittYFilter(
        "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_gaussian_smooth(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISGaussianSmoothFilter(
        3, 1, 1, 0, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_gaussian_1st_deriv(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISGaussian1stDerivFilter(
        3, 1, 1, 0, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_gaussian_2nd_deriv(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISGaussian2ndDerivFilter(
        3, 1, 1, 0, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_laplacian(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISLaplacianFilter(
        3, 1, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)


def test_perform_tiled_img_filter_lee(tmp_path):
    import rsgislib.imagefilter.tiledfilter

    filter_inst = rsgislib.imagefilter.tiledfilter.RSGISLeeFilter(
        3, 2, "GTIFF", rsgislib.TYPE_16UINT
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.tif")
    output_img = os.path.join(tmp_path, "filter_output.tif")
    rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter(
        input_img,
        output_img,
        filter_inst,
        datatype=None,
        gdalformat="GTIFF",
        tmp_dir=os.path.join(tmp_path, "tmp"),
        width=250,
        height=250,
        n_cores=1,
    )

    assert os.path.exists(output_img)
