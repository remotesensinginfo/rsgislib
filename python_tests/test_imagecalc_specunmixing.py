import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
SPECUNMIX_DATA_DIR = os.path.join(DATA_DIR, "imagecalc", "specunmixing")


def test_readEndmembersMTXT():
    from rsgislib.imagecalc import specunmixing

    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")

    n_endmembers, n_bands, endmembers = specunmixing.readEndmembersMTXT(
        endmembers_file, gain=1, weight=None
    )

    if n_endmembers != 3:
        assert False
    if n_bands != 10:
        assert False
    endmember1 = [
        31.3333,
        58.5,
        21.8333,
        96.8333,
        453.333,
        573.167,
        608.0,
        603.0,
        156.0,
        69.3333,
    ]
    endmember2 = [89.8, 118.2, 149.8, 200.2, 274.6, 299.4, 323.4, 344.0, 371.4, 259.0]
    endmember3 = [
        67.1216,
        67.3513,
        35.4459,
        35.1757,
        34.8108,
        34.2297,
        30.5946,
        32.473,
        23.5676,
        23.1216,
    ]

    for in_val, ref_val in zip(endmembers[0], endmember1):
        if abs(in_val - ref_val) > 0.0001:
            assert False
    for in_val, ref_val in zip(endmembers[1], endmember2):
        if abs(in_val - ref_val) > 0.0001:
            assert False
    for in_val, ref_val in zip(endmembers[2], endmember3):
        if abs(in_val - ref_val) > 0.0001:
            assert False


def test_readEndmembersMTXT_weight():
    from rsgislib.imagecalc import specunmixing

    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")

    n_endmembers, n_bands, endmembers = specunmixing.readEndmembersMTXT(
        endmembers_file, gain=1, weight=42
    )

    if n_endmembers != 3:
        assert False
    if n_bands != 11:
        assert False
    endmember1 = [
        31.3333,
        58.5,
        21.8333,
        96.8333,
        453.333,
        573.167,
        608.0,
        603.0,
        156.0,
        69.3333,
        42,
    ]
    endmember2 = [
        89.8,
        118.2,
        149.8,
        200.2,
        274.6,
        299.4,
        323.4,
        344.0,
        371.4,
        259.0,
        42,
    ]
    endmember3 = [
        67.1216,
        67.3513,
        35.4459,
        35.1757,
        34.8108,
        34.2297,
        30.5946,
        32.473,
        23.5676,
        23.1216,
        42,
    ]

    for in_val, ref_val in zip(endmembers[0], endmember1):
        if abs(in_val - ref_val) > 0.0001:
            assert False
    for in_val, ref_val in zip(endmembers[1], endmember2):
        if abs(in_val - ref_val) > 0.0001:
            assert False
    for in_val, ref_val in zip(endmembers[2], endmember3):
        if abs(in_val - ref_val) > 0.0001:
            assert False


def test_readEndmembersMTXT_gain():
    from rsgislib.imagecalc import specunmixing

    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")

    n_endmembers, n_bands, endmembers = specunmixing.readEndmembersMTXT(
        endmembers_file, gain=10, weight=None
    )

    if n_endmembers != 3:
        assert False
    if n_bands != 10:
        assert False
    endmember1 = [
        3.13333,
        5.85,
        2.18333,
        9.68333,
        45.3333,
        57.3167,
        60.80,
        60.30,
        15.60,
        6.93333,
    ]
    endmember2 = [8.98, 11.82, 14.98, 20.02, 27.46, 29.94, 32.34, 34.40, 37.14, 25.90]
    endmember3 = [
        6.71216,
        6.73513,
        3.54459,
        3.51757,
        3.48108,
        3.42297,
        3.05946,
        3.2473,
        2.35676,
        2.31216,
    ]

    for in_val, ref_val in zip(endmembers[0], endmember1):
        if abs(in_val - ref_val) > 0.0001:
            assert False
    for in_val, ref_val in zip(endmembers[1], endmember2):
        if abs(in_val - ref_val) > 0.0001:
            assert False
    for in_val, ref_val in zip(endmembers[2], endmember3):
        if abs(in_val - ref_val) > 0.0001:
            assert False


def test_plotEndmembers_basic(tmp_path):
    from rsgislib.imagecalc import specunmixing

    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")
    endmember_names = ["PhotoVeg", "NonPhotoVeg", "Water"]
    out_plot_file = os.path.join(tmp_path, "plot.png")
    specunmixing.plotEndmembers(endmembers_file, endmember_names, out_plot_file)
    assert os.path.exists(out_plot_file)


def test_plotEndmembers_gain(tmp_path):
    from rsgislib.imagecalc import specunmixing

    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")
    endmember_names = ["PhotoVeg", "NonPhotoVeg", "Water"]
    out_plot_file = os.path.join(tmp_path, "plot.png")
    specunmixing.plotEndmembers(
        endmembers_file, endmember_names, out_plot_file, gain=10
    )
    assert os.path.exists(out_plot_file)


def test_plotEndmembers_title(tmp_path):
    from rsgislib.imagecalc import specunmixing

    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")
    endmember_names = ["PhotoVeg", "NonPhotoVeg", "Water"]
    title = "Endmembers Plot"
    out_plot_file = os.path.join(tmp_path, "plot.png")
    specunmixing.plotEndmembers(
        endmembers_file, endmember_names, out_plot_file, plot_title=title, gain=10
    )
    assert os.path.exists(out_plot_file)


def test_plotEndmembers_wavelengths(tmp_path):
    from rsgislib.imagecalc import specunmixing

    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")
    endmember_names = ["PhotoVeg", "NonPhotoVeg", "Water"]
    wavelengths = [490, 560, 665, 705, 740, 783, 842, 865, 1610, 2190]
    out_plot_file = os.path.join(tmp_path, "plot.png")
    specunmixing.plotEndmembers(
        endmembers_file,
        endmember_names,
        out_plot_file,
        gain=10,
        wavelengths=wavelengths,
    )
    assert os.path.exists(out_plot_file)


def test_areEndmembersEqual_True():
    from rsgislib.imagecalc import specunmixing

    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")
    assert specunmixing.areEndmembersEqual(endmembers_file, endmembers_file)


def test_areEndmembersEqual_True_FltPtEdit():
    from rsgislib.imagecalc import specunmixing

    endmembers_ref_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")
    endmembers_cmp_file = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_endmembers_eq_fltpt_edit.mtxt"
    )
    assert specunmixing.areEndmembersEqual(endmembers_ref_file, endmembers_cmp_file)


def test_areEndmembersEqual_False():
    from rsgislib.imagecalc import specunmixing

    endmembers_ref_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")
    endmembers_cmp_file = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_endmembers_not_eq.mtxt"
    )
    assert not specunmixing.areEndmembersEqual(endmembers_ref_file, endmembers_cmp_file)


def test_extractAvgEndMembers(tmp_path):
    from rsgislib.imagecalc import specunmixing

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(SPECUNMIX_DATA_DIR, "endmember_rois.geojson")
    vec_lyr = "endmember_rois"
    out_endmembers_file = os.path.join(tmp_path, "sen2_endmembers_test")

    specunmixing.extractAvgEndMembers(input_img, vec_file, vec_lyr, out_endmembers_file)

    endmembers_ref_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")
    test_endmembers_file = os.path.join(tmp_path, "sen2_endmembers_test.mtxt")
    assert specunmixing.areEndmembersEqual(endmembers_ref_file, test_endmembers_file)


def test_exhconLinearSpecUnmix(tmp_path):
    import rsgislib
    from rsgislib.imagecalc import specunmixing
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")

    output_img = os.path.join(tmp_path, "sen2_unmixed_img.kea")
    specunmixing.exhconLinearSpecUnmix(
        input_img, output_img, "KEA", rsgislib.TYPE_32FLOAT, endmembers_file, 0.1, 1, 0
    )

    ref_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_exhcon.kea"
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_unmix_img, output_img)
    assert img_eq


def test_specUnmixSpTsUCLS_noWeight(tmp_path):
    import rsgislib
    from rsgislib.imagecalc import specunmixing
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    valid_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")

    output_img = os.path.join(tmp_path, "sen2_unmixed_img.kea")
    specunmixing.specUnmixSpTsUCLS(
        input_img,
        valid_msk_img,
        1,
        output_img,
        endmembers_file,
        gdalformat="KEA",
        gain=1,
        weight=None,
        calc_stats=True,
    )

    ref_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_SpTsUCLS_noweight.kea"
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_unmix_img, output_img)
    assert img_eq


def test_specUnmixSpTsUCLS_weight100(tmp_path):
    import rsgislib
    from rsgislib.imagecalc import specunmixing
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    valid_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")

    output_img = os.path.join(tmp_path, "sen2_unmixed_img.kea")
    specunmixing.specUnmixSpTsUCLS(
        input_img,
        valid_msk_img,
        1,
        output_img,
        endmembers_file,
        gdalformat="KEA",
        gain=1,
        weight=100,
        calc_stats=True,
    )

    ref_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_SpTsUCLS_weight100.kea"
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_unmix_img, output_img)
    assert img_eq


def test_specUnmixSpTsNNLS_noWeight(tmp_path):
    import rsgislib
    from rsgislib.imagecalc import specunmixing
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    valid_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")

    output_img = os.path.join(tmp_path, "sen2_unmixed_img.kea")
    specunmixing.specUnmixSpTsNNLS(
        input_img,
        valid_msk_img,
        1,
        output_img,
        endmembers_file,
        gdalformat="KEA",
        gain=1,
        weight=None,
        calc_stats=True,
    )

    ref_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_SpTsNNLS_noWeight.kea"
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_unmix_img, output_img)
    assert img_eq


def test_specUnmixSpTsNNLS_weight100(tmp_path):
    import rsgislib
    from rsgislib.imagecalc import specunmixing
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    valid_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")

    output_img = os.path.join(tmp_path, "sen2_unmixed_img.kea")
    specunmixing.specUnmixSpTsNNLS(
        input_img,
        valid_msk_img,
        1,
        output_img,
        endmembers_file,
        gdalformat="KEA",
        gain=1,
        weight=100,
        calc_stats=True,
    )

    ref_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_SpTsNNLS_weight100.kea"
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_unmix_img, output_img)
    assert img_eq


def test_specUnmixSpTsFCLS(tmp_path):
    import rsgislib
    from rsgislib.imagecalc import specunmixing
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    valid_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")

    output_img = os.path.join(tmp_path, "sen2_unmixed_img.kea")
    specunmixing.specUnmixSpTsFCLS(
        input_img,
        valid_msk_img,
        1,
        output_img,
        endmembers_file,
        gdalformat="KEA",
        gain=1,
        calc_stats=True,
    )

    ref_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_SpTsFCLS.kea"
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_unmix_img, output_img)
    assert img_eq


def test_specUnmixPyMcrNNLS_noWeight(tmp_path):
    import rsgislib
    from rsgislib.imagecalc import specunmixing
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    valid_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")

    output_img = os.path.join(tmp_path, "sen2_unmixed_img.kea")
    specunmixing.specUnmixPyMcrNNLS(
        input_img,
        valid_msk_img,
        1,
        output_img,
        endmembers_file,
        gdalformat="KEA",
        gain=1,
        weight=None,
        calc_stats=True,
    )

    ref_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_PyMcrNNLS_noWeight.kea"
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_unmix_img, output_img)
    assert img_eq


def test_specUnmixPyMcrNNLS_weight100(tmp_path):
    import rsgislib
    from rsgislib.imagecalc import specunmixing
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    valid_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")

    output_img = os.path.join(tmp_path, "sen2_unmixed_img.kea")
    specunmixing.specUnmixPyMcrNNLS(
        input_img,
        valid_msk_img,
        1,
        output_img,
        endmembers_file,
        gdalformat="KEA",
        gain=1,
        weight=100,
        calc_stats=True,
    )

    ref_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_PyMcrNNLS_weight100.kea"
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_unmix_img, output_img)
    assert img_eq


def test_specUnmixPyMcrFCLS(tmp_path):
    import rsgislib
    from rsgislib.imagecalc import specunmixing
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    valid_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")

    output_img = os.path.join(tmp_path, "sen2_unmixed_img.kea")
    specunmixing.specUnmixPyMcrFCLS(
        input_img,
        valid_msk_img,
        1,
        output_img,
        endmembers_file,
        gdalformat="KEA",
        gain=1,
        calc_stats=True,
    )

    ref_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_PyMcrFCLS.kea"
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_unmix_img, output_img)
    assert img_eq


def test_rescaleUnmixingResults(tmp_path):
    import rsgislib
    from rsgislib.imagecalc import specunmixing
    import rsgislib.imagecalc

    input_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_SpTsUCLS_noweight.kea"
    )
    output_img = os.path.join(tmp_path, "sen2_unmixed_rescaled_img.kea")
    specunmixing.rescaleUnmixingResults(
        input_img, output_img, gdalformat="KEA", calc_stats=True
    )

    ref_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR,
        "sen2_20210527_aber_subset_unmixed_SpTsUCLS_noweight_rescaled.kea",
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_unmix_img, output_img)
    assert img_eq


def test_predictReflLinearUnmixing(tmp_path):
    import rsgislib
    from rsgislib.imagecalc import specunmixing
    import rsgislib.imagecalc

    input_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_SpTsFCLS.kea"
    )
    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")
    output_img = os.path.join(tmp_path, "sen2_unmixed_pred_refl_img.kea")
    specunmixing.predictReflLinearUnmixing(
        input_img, endmembers_file, output_img, gdalformat="KEA", calc_stats=True
    )

    ref_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_SpTsFCLS_predrefl.kea"
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_unmix_img, output_img)
    assert img_eq


def test_calcUnmixingRMSEResidualErr(tmp_path):
    import rsgislib
    from rsgislib.imagecalc import specunmixing
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR, "sen2_20210527_aber_subset_unmixed_SpTsFCLS.kea"
    )
    endmembers_file = os.path.join(SPECUNMIX_DATA_DIR, "sen2_endmembers.mtxt")
    output_img = os.path.join(tmp_path, "sen2_unmixed_rmse_residuals_img.kea")
    specunmixing.calcUnmixingRMSEResidualErr(
        input_img,
        input_unmix_img,
        endmembers_file,
        output_img,
        gdalformat="KEA",
        calc_stats=True,
    )

    ref_unmix_img = os.path.join(
        SPECUNMIX_DATA_DIR,
        "sen2_20210527_aber_subset_unmixed_SpTsFCLS_rmse_residuals.kea",
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_unmix_img, output_img)
    assert img_eq
