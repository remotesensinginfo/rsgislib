import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
ZONALSTATS_DATA_DIR = os.path.join(DATA_DIR, "zonalstats")


def test_extPointBandValuesFile(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_pt_samples.geojson")
    vec_lyr = "sen2_20210527_aber_pt_samples"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.extPointBandValuesFile(out_vec_file, out_vec_lyr, input_img, 1, 0, 1000, 0, "testcolval", reproj_vec=False, vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [39.0, 35.0, 124.0, 63.0, 36.0, 34.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_extPointBandValuesFile_ReProj(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_wgs84_pt_samples.geojson")
    vec_lyr = "sen2_20210527_aber_wgs84_pt_samples"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.extPointBandValuesFile(out_vec_file, out_vec_lyr, input_img, 1, 0, 1000, 0, "testcolval", reproj_vec=True, vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [33.0, 271.0, 33.0, 26.0, 80.0, 68.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calcZonalBandStatsTestPolyPtsFile_Min(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsTestPolyPtsFile(out_vec_file, out_vec_lyr, input_img, 1, 0,
                                      1000, 0, percentile=None, percentile_field=None,
                                      min_field='testcolval', max_field=None, mean_field=None,
                                      stddev_field=None, sum_field=None,
                                      count_field=None, mode_field=None,
                                      median_field=None,  vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [26.0, 38.0, 37.0, 163.0, 65.0, 41.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsTestPolyPtsFile_Max(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsTestPolyPtsFile(out_vec_file, out_vec_lyr, input_img, 1, 0,
                                      1000, 0, percentile=None, percentile_field=None,
                                      min_field=None, max_field='testcolval', mean_field=None,
                                      stddev_field=None, sum_field=None,
                                      count_field=None, mode_field=None,
                                      median_field=None,  vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [32.0, 48.0, 46.0, 417.0, 73.0, 52.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsTestPolyPtsFile_Mean(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsTestPolyPtsFile(out_vec_file, out_vec_lyr, input_img, 1, 0,
                                      1000, 0, percentile=None, percentile_field=None,
                                      min_field=None, max_field=None, mean_field='testcolval',
                                      stddev_field=None, sum_field=None,
                                      count_field=None, mode_field=None,
                                      median_field=None,  vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [29.299145299145298, 43.858108108108105, 41.67605633802817, 309.48275862068965, 69.29411764705883, 46.58974358974359]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsTestPolyPtsFile_StdDev(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsTestPolyPtsFile(out_vec_file, out_vec_lyr, input_img, 1, 0,
                                      1000, 0, percentile=None, percentile_field=None,
                                      min_field=None, max_field=None, mean_field=None,
                                      stddev_field='testcolval', sum_field=None,
                                      count_field=None, mode_field=None,
                                      median_field=None,  vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [1.1822400674899987, 2.708869653402217, 2.1609508072124695, 68.0732975598059, 1.8395877377778693, 2.168751841414411]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsTestPolyPtsFile_Sum(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsTestPolyPtsFile(out_vec_file, out_vec_lyr, input_img, 1, 0,
                                      1000, 0, percentile=None, percentile_field=None,
                                      min_field=None, max_field=None, mean_field=None,
                                      stddev_field=None, sum_field='testcolval',
                                      count_field=None, mode_field=None,
                                      median_field=None,  vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [6856.0, 12982.0, 2959.0, 8975.0, 2356.0, 1817.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsTestPolyPtsFile_Count(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsTestPolyPtsFile(out_vec_file, out_vec_lyr, input_img, 1, 0,
                                      1000, 0, percentile=None, percentile_field=None,
                                      min_field=None, max_field=None, mean_field=None,
                                      stddev_field=None, sum_field=None,
                                      count_field='testcolval', mode_field=None,
                                      median_field=None,  vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [234.0, 296.0, 71.0, 29.0, 34.0, 39.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsTestPolyPtsFile_Mode(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsTestPolyPtsFile(out_vec_file, out_vec_lyr, input_img, 1, 0,
                                      1000, 0, percentile=None, percentile_field=None,
                                      min_field=None, max_field=None, mean_field=None,
                                      stddev_field=None, sum_field=None,
                                      count_field=None, mode_field='testcolval',
                                      median_field=None,  vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [29.0, 46.0, 40.0, 328.0, 69.0, 46.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsTestPolyPtsFile_Median(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsTestPolyPtsFile(out_vec_file, out_vec_lyr, input_img, 1, 0,
                                      1000, 0, percentile=None, percentile_field=None,
                                      min_field=None, max_field=None, mean_field=None,
                                      stddev_field=None, sum_field=None,
                                      count_field=None, mode_field=None,
                                      median_field='testcolval',  vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [29.0, 45.0, 42.0, 326.0, 69.0, 47.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsTestPolyPtsFile_Percentile(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsTestPolyPtsFile(out_vec_file, out_vec_lyr, input_img, 1, 0,
                                      1000, 0, percentile=75, percentile_field='testcolval',
                                      min_field=None, max_field=None, mean_field=None,
                                      stddev_field=None, sum_field=None,
                                      count_field=None, mode_field=None,
                                      median_field=None,  vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [30.0, 46.0, 43.0, 356.0, 70.0, 48.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq



def test_calcZonalPolyPtsBandStatsFile(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalPolyPtsBandStatsFile(out_vec_file, out_vec_lyr, input_img, 1, "testcolval", vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [31.0, 46.0, 46.0, 326.0, 69.0, 49.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq



def test_calcZonalBandStatsFile_Min(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsFile(out_vec_file, out_vec_lyr, input_img, 1, 0, 1000, 0,
                           min_field='testcolval', max_field=None, mean_field=None, stddev_field=None, sum_field=None,
                           count_field=None, mode_field=None, median_field=None, vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [26.0, 38.0, 37.0, 163.0, 65.0, 41.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsFile_Max(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsFile(out_vec_file, out_vec_lyr, input_img, 1, 0, 1000, 0,
                           min_field=None, max_field="testcolval", mean_field=None, stddev_field=None, sum_field=None,
                           count_field=None, mode_field=None, median_field=None, vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [32.0, 48.0, 46.0, 417.0, 73.0, 52.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsFile_Mean(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsFile(out_vec_file, out_vec_lyr, input_img, 1, 0, 1000, 0,
                           min_field=None, max_field=None, mean_field="testcolval", stddev_field=None, sum_field=None,
                           count_field=None, mode_field=None, median_field=None, vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [29.299145299145298, 43.858108108108105, 41.67605633802817, 309.48275862068965, 69.29411764705883, 46.58974358974359]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsFile_StdDev(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsFile(out_vec_file, out_vec_lyr, input_img, 1, 0, 1000, 0,
                           min_field=None, max_field=None, mean_field=None, stddev_field="testcolval", sum_field=None,
                           count_field=None, mode_field=None, median_field=None, vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [1.1822400674899987, 2.708869653402217, 2.1609508072124695, 68.0732975598059, 1.8395877377778693, 2.168751841414411]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsFile_Sum(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsFile(out_vec_file, out_vec_lyr, input_img, 1, 0, 1000, 0,
                           min_field=None, max_field=None, mean_field=None, stddev_field=None, sum_field="testcolval",
                           count_field=None, mode_field=None, median_field=None, vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [6856.0, 12982.0, 2959.0, 8975.0, 2356.0, 1817.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsFile_Count(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsFile(out_vec_file, out_vec_lyr, input_img, 1, 0, 1000, 0,
                           min_field=None, max_field=None, mean_field=None, stddev_field=None, sum_field=None,
                           count_field="testcolval", mode_field=None, median_field=None, vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [234.0, 296.0, 71.0, 29.0, 34.0, 39.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsFile_Mode(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsFile(out_vec_file, out_vec_lyr, input_img, 1, 0, 1000, 0,
                           min_field=None, max_field=None, mean_field=None, stddev_field=None, sum_field=None,
                           count_field=None, mode_field="testcolval", median_field=None, vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [29.0, 46.0, 40.0, 328.0, 69.0, 46.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq

def test_calcZonalBandStatsFile_Median(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, 'GEOJSON', replace=True, in_memory=True)

    rsgislib.zonalstats.calcZonalBandStatsFile(out_vec_file, out_vec_lyr, input_img, 1, 0, 1000, 0,
                           min_field=None, max_field=None, mean_field=None, stddev_field=None, sum_field=None,
                           count_field=None, mode_field=None, median_field="testcolval", vec_def_epsg=None)

    vals = rsgislib.vectorattrs.readVecColumn(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [29.0, 45.0, 42.0, 326.0, 69.0, 47.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq