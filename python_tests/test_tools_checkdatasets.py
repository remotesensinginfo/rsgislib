import os
import pytest

H5PY_NOT_AVAIL = False
try:
    import h5py
except ImportError:
    H5PY_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
ZONALSTATS_DATA_DIR = os.path.join(DATA_DIR, "zonalstats")


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_check_gdal_image_file_base():
    import rsgislib.tools.checkdatasets

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_image_file(input_img)
    assert file_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_check_gdal_image_file_n_bands():
    import rsgislib.tools.checkdatasets

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_image_file(
        input_img, check_bands=True, n_bands=10
    )
    assert file_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_check_gdal_image_file_n_bands_err():
    import rsgislib.tools.checkdatasets

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_image_file(
        input_img, check_bands=True, n_bands=5
    )
    assert not file_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_check_gdal_image_file_proj():
    import rsgislib.tools.checkdatasets

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_image_file(
        input_img, check_bands=True, n_bands=0, chk_proj=True, epsg_code=27700
    )
    assert file_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_check_gdal_image_file_proj_err():
    import rsgislib.tools.checkdatasets

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_image_file(
        input_img, check_bands=True, n_bands=0, chk_proj=True, epsg_code=4326
    )
    assert not file_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_check_gdal_image_file_read_img():
    import rsgislib.tools.checkdatasets

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_image_file(
        input_img,
        check_bands=True,
        n_bands=0,
        chk_proj=True,
        epsg_code=27700,
        read_img=True,
        smpl_n_pxls=10,
    )
    assert file_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_check_gdal_image_file_check_sum():
    import rsgislib.tools.checkdatasets

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_image_file(
        input_img,
        check_bands=True,
        n_bands=0,
        chk_proj=True,
        epsg_code=27700,
        read_img=True,
        smpl_n_pxls=10,
        calc_chk_sum=True,
    )
    assert file_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_check_gdal_image_file_filesize():
    import rsgislib.tools.checkdatasets

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_image_file(
        input_img,
        check_bands=True,
        n_bands=0,
        chk_proj=True,
        epsg_code=27700,
        read_img=True,
        smpl_n_pxls=10,
        calc_chk_sum=True,
        max_file_size=8000000,
    )
    assert file_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_check_gdal_image_file_filesize_err():
    import rsgislib.tools.checkdatasets

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_image_file(
        input_img,
        check_bands=True,
        n_bands=0,
        chk_proj=True,
        epsg_code=27700,
        read_img=True,
        smpl_n_pxls=10,
        calc_chk_sum=True,
        max_file_size=4000000,
    )
    assert not file_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_run_check_gdal_image_file():
    import rsgislib.tools.checkdatasets

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    file_ok = rsgislib.tools.checkdatasets.run_check_gdal_image_file(
        input_img,
        check_bands=True,
        n_bands=0,
        chk_proj=True,
        epsg_code=27700,
        read_img=True,
        smpl_n_pxls=10,
        calc_chk_sum=True,
        max_file_size=8000000,
        rm_err=False,
        print_err=True,
    )
    assert file_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_run_check_gdal_image_files():
    import rsgislib.tools.checkdatasets

    input_imgs = [
        os.path.join(DATA_DIR, "sen2_20210527_aber.kea"),
        os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea"),
    ]
    files_ok = rsgislib.tools.checkdatasets.run_check_gdal_image_files(
        input_imgs,
        check_bands=True,
        n_bands=0,
        chk_proj=True,
        epsg_code=27700,
        read_img=True,
        smpl_n_pxls=10,
        calc_chk_sum=True,
        max_file_size=8000000,
        rm_err=False,
        print_err=True,
    )
    assert files_ok


def test_check_gdal_vector_file_base():
    import rsgislib.tools.checkdatasets

    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_vector_file(
        vec_file, chk_proj=False
    )
    assert file_ok


def test_check_gdal_vector_file_proj():
    import rsgislib.tools.checkdatasets

    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_vector_file(
        vec_file, chk_proj=True, epsg_code=27700, max_file_size=None
    )
    assert file_ok


def test_check_gdal_vector_file_proj_err():
    import rsgislib.tools.checkdatasets

    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_vector_file(
        vec_file, chk_proj=True, epsg_code=4326, max_file_size=None
    )
    assert not file_ok


def test_check_gdal_vector_file_filesize():
    import rsgislib.tools.checkdatasets

    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_vector_file(
        vec_file, chk_proj=True, epsg_code=27700, max_file_size=5000
    )
    assert file_ok


def test_check_gdal_vector_file_filesize_err():
    import rsgislib.tools.checkdatasets

    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_gdal_vector_file(
        vec_file, chk_proj=True, epsg_code=27700, max_file_size=2500
    )
    assert not file_ok


def test_run_check_gdal_vector_file():
    import rsgislib.tools.checkdatasets

    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    file_ok = rsgislib.tools.checkdatasets.run_check_gdal_vector_file(
        vec_file, chk_proj=True, epsg_code=27700, max_file_size=5000
    )
    assert file_ok


def test_run_check_gdal_vector_files():
    import rsgislib.tools.checkdatasets

    vec_files = [
        os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson"),
        os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_pt_samples.geojson"),
    ]
    files_ok = rsgislib.tools.checkdatasets.run_check_gdal_vector_files(
        vec_files, chk_proj=True, epsg_code=27700, max_file_size=5000
    )
    assert files_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_check_hdf5_file():
    import rsgislib.tools.checkdatasets

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    file_ok, err_msg = rsgislib.tools.checkdatasets.check_hdf5_file(input_img)
    assert file_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_run_check_hdf5_file():
    import rsgislib.tools.checkdatasets

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    file_ok = rsgislib.tools.checkdatasets.run_check_hdf5_file(input_img)
    assert file_ok


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_run_check_hdf5_files():
    import rsgislib.tools.checkdatasets

    input_imgs = [
        os.path.join(DATA_DIR, "sen2_20210527_aber.kea"),
        os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea"),
    ]
    files_ok = rsgislib.tools.checkdatasets.run_check_hdf5_files(input_imgs)
    assert files_ok
