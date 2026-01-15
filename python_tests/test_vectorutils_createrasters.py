import os
import sys
import pytest

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")

os_pltform = sys.platform

ON_MACOS = False
if os_pltform == "darwin":
    ON_MACOS = True

GEOPANDAS_NOT_AVAIL = False
try:
    import geopandas
except ImportError:
    GEOPANDAS_NOT_AVAIL = True


def test_rasterise_vec_lyr(tmp_path):
    import rsgislib.vectorutils.createrasters

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.tif")
    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    output_img = os.path.join(tmp_path, "out_img.tif")

    rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
        vec_file, vec_lyr, input_img, output_img, gdalformat="GTIFF"
    )
    assert os.path.exists(output_img)


def test_rasterise_vec_lyr_burn_col(tmp_path):
    import rsgislib.vectorutils.createrasters

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.tif")
    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    output_img = os.path.join(tmp_path, "out_img.tif")

    rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
        vec_file, vec_lyr, input_img, output_img, gdalformat="GTIFF", att_column="val"
    )
    assert os.path.exists(output_img)


def test_rasterise_vec_lyr_obj(tmp_path):
    import rsgislib.imageutils
    import rsgislib.vectorutils
    import rsgislib.vectorutils.createrasters

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.tif")
    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    vec_obj_ds, vec_lyr_obj = rsgislib.vectorutils.open_gdal_vec_lyr(vec_file, vec_lyr)

    output_img = os.path.join(tmp_path, "out_img.tif")

    rsgislib.imageutils.create_copy_img(
        input_img, output_img, 1, 0, "GTIFF", rsgislib.TYPE_8UINT
    )

    rsgislib.vectorutils.createrasters.rasterise_vec_lyr_obj(vec_lyr_obj, output_img)
    vec_obj_ds = None
    assert os.path.exists(output_img)


def test_rasterise_vec_lyr_obj_burn_col(tmp_path):
    import rsgislib.imageutils
    import rsgislib.vectorutils
    import rsgislib.vectorutils.createrasters

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.tif")
    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    vec_obj_ds, vec_lyr_obj = rsgislib.vectorutils.open_gdal_vec_lyr(vec_file, vec_lyr)

    output_img = os.path.join(tmp_path, "out_img.tif")
    rsgislib.imageutils.create_copy_img(
        input_img, output_img, 1, 0, "GTIFF", rsgislib.TYPE_8UINT
    )

    rsgislib.vectorutils.createrasters.rasterise_vec_lyr_obj(
        vec_lyr_obj, output_img, att_column="val"
    )
    vec_obj_ds = None
    assert os.path.exists(output_img)


def test_copy_vec_to_rat_exception(tmp_path):
    import rsgislib
    import rsgislib.vectorutils.createrasters

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.tif")
    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    output_img = os.path.join(tmp_path, "out_img.kea")
    with pytest.raises(rsgislib.RSGISPyException):
        rsgislib.vectorutils.createrasters.copy_vec_to_rat(
            vec_file, vec_lyr, input_img, output_img
        )


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL or ON_MACOS, reason="geopandas dependency not available or using MacOS where testing needing KEA are skipped.")
def test_copy_vec_to_rat(tmp_path):
    import rsgislib.vectorutils.createrasters
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.tif")
    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    out_vec_file = os.path.join(tmp_path, "aber_osgb_multi_polys_fid.geojson")
    out_vec_lyr = "aber_osgb_multi_polys_fid"

    rsgislib.vectorattrs.add_fid_col(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GEOJSON",
        out_col="FID",
    )

    output_img = os.path.join(tmp_path, "out_img.kea")

    rsgislib.vectorutils.createrasters.copy_vec_to_rat(
        out_vec_file, out_vec_lyr, input_img, output_img, fid_col="FID"
    )

    assert os.path.exists(output_img)
