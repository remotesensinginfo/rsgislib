import os
import platform
import pytest
import rsgislib.tools.filetools

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
FILETOOLS_DATA_DIR = os.path.join(DATA_DIR, "tools/filetools")

unzip_cmd_avail = rsgislib.tools.filetools.is_cmd_tool_avail("unzip")
tar_cmd_avail = rsgislib.tools.filetools.is_cmd_tool_avail("tar")
on_windows = platform.system() == "Windows"


@pytest.mark.parametrize(
    "input, expected",
    [("hello.txt", "hello"), ("world/hello.txt", "hello"), ("hello.tar.gz", "hello")],
)
def test_get_file_basename(input, expected):
    assert rsgislib.tools.filetools.get_file_basename(input) == expected


def test_get_file_basename_rm_n_exts_1():
    assert (
        rsgislib.tools.filetools.get_file_basename("hello.tar.gz", rm_n_exts=1)
        == "hello.tar"
    )


def test_get_file_basename_check_valid():
    assert (
        rsgislib.tools.filetools.get_file_basename("hello!.txt", check_valid=True)
        == "hello"
    )


def test_get_file_basename_check_valid_1():
    assert (
        rsgislib.tools.filetools.get_file_basename(
            "hello_world_rsgislib.txt", n_comps=1
        )
        == "hello"
    )


def test_get_file_basename_check_valid_2():
    assert (
        rsgislib.tools.filetools.get_file_basename(
            "hello_world_rsgislib.txt", n_comps=2
        )
        == "hello_world"
    )


def test_is_path_valid_true():
    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert rsgislib.tools.filetools.is_path_valid(input_img)


def test_is_path_valid_false(tmp_path):
    input_file = os.path.join(tmp_path, "sen2_20210527_aber\x00.kea")
    assert not rsgislib.tools.filetools.is_path_valid(input_file)


def test_is_path_sibling_creatable_true(tmp_path):
    input_file = os.path.join(tmp_path, "sen2_20210527_aber.kea")
    assert rsgislib.tools.filetools.is_path_sibling_creatable(input_file)


def test_is_path_sibling_creatable_false(tmp_path):
    input_file = os.path.join(tmp_path, "hello", "world", "sen2_20210527_aber.kea")
    assert not rsgislib.tools.filetools.is_path_sibling_creatable(input_file)


def test_does_path_exists_or_creatable_true(tmp_path):
    input_file = os.path.join(tmp_path, "sen2_20210527_aber.kea")
    assert rsgislib.tools.filetools.does_path_exists_or_creatable(input_file)


def test_does_path_exists_or_creatable_false_file_name(tmp_path):
    input_file = os.path.join(tmp_path, "sen2_20210527_aber\x00.kea")
    assert not rsgislib.tools.filetools.does_path_exists_or_creatable(input_file)


def test_does_path_exists_or_creatable_false_path(tmp_path):
    input_file = os.path.join(tmp_path, "hello", "world", "sen2_20210527_aber.kea")
    assert not rsgislib.tools.filetools.does_path_exists_or_creatable(input_file)


def test_does_path_exists_or_creatable_false_path_and_name(tmp_path):
    input_file = os.path.join(tmp_path, "hello", "world", "sen2_20210527_aber\x00.kea")
    assert not rsgislib.tools.filetools.does_path_exists_or_creatable(input_file)


def test_get_file_size_bytes():
    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert rsgislib.tools.filetools.get_file_size(input_img) == 7749324


def test_get_file_size_kb():
    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert (
        abs(rsgislib.tools.filetools.get_file_size(input_img, unit="kb") - 7567.699)
        < 0.1
    )


def test_get_file_size_mb():
    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert (
        abs(rsgislib.tools.filetools.get_file_size(input_img, unit="mb") - 7.39) < 0.01
    )


def test_get_file_size_gb():
    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert (
        abs(rsgislib.tools.filetools.get_file_size(input_img, unit="gb") - 0.00722)
        < 0.0001
    )


def test_get_file_size_tb():
    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert (
        abs(rsgislib.tools.filetools.get_file_size(input_img, unit="tb") - 7.04797e-06)
        < 0.0000001
    )


def test_convert_file_size_units_bytes_kb():
    out_size = rsgislib.tools.filetools.convert_file_size_units(
        1000000, in_unit="bytes", out_unit="kb"
    )
    assert abs(out_size - 976.56) < 0.1


def test_convert_file_size_units_bytes_mb():
    import rsgislib.tools.filetools

    out_size = rsgislib.tools.filetools.convert_file_size_units(
        1000000, in_unit="bytes", out_unit="mb"
    )
    assert abs(out_size - 0.954) < 0.1


def test_convert_file_size_units_bytes_gb():
    out_size = rsgislib.tools.filetools.convert_file_size_units(
        1000000, in_unit="bytes", out_unit="gb"
    )
    assert abs(out_size - 0.000931) < 0.0001


def test_convert_file_size_units_bytes_tb():
    out_size = rsgislib.tools.filetools.convert_file_size_units(
        1000000, in_unit="bytes", out_unit="tb"
    )
    assert abs(out_size - 9.095e-07) < 0.0000001


def test_convert_file_size_units_kb_bytes():
    out_size = rsgislib.tools.filetools.convert_file_size_units(
        1000, in_unit="kb", out_unit="bytes"
    )
    assert abs(out_size - 1024000.0) < 1


def test_convert_file_size_units_kb_mb():
    out_size = rsgislib.tools.filetools.convert_file_size_units(
        1000, in_unit="kb", out_unit="mb"
    )
    assert abs(out_size - 0.977) < 0.1


def test_convert_file_size_units_kb_gb():
    out_size = rsgislib.tools.filetools.convert_file_size_units(
        1000, in_unit="kb", out_unit="gb"
    )
    assert abs(out_size - 0.000954) < 0.0001


def test_convert_file_size_units_kb_tb():
    out_size = rsgislib.tools.filetools.convert_file_size_units(
        1000, in_unit="kb", out_unit="tb"
    )
    assert abs(out_size - 9.313e-07) < 0.0000001


@pytest.mark.skipif((not tar_cmd_avail), reason="tar command not available")
def test_untar_file_gen_arch_dir_true(tmp_path):
    input_file = os.path.join(FILETOOLS_DATA_DIR, "test_file.tar")

    rsgislib.tools.filetools.untar_file(input_file, tmp_path)

    hello_file = os.path.join(tmp_path, "test_file", "hello.txt")
    world_file = os.path.join(tmp_path, "test_file", "world.txt")
    rsgislib_file = os.path.join(tmp_path, "test_file", "rsgislib.txt")

    success = (
        os.path.exists(hello_file)
        and os.path.exists(world_file)
        and os.path.exists(rsgislib_file)
    )
    assert success


@pytest.mark.skipif((not tar_cmd_avail), reason="tar command not available")
def test_untar_file_gen_arch_dir_false(tmp_path):
    input_file = os.path.join(FILETOOLS_DATA_DIR, "test_file.tar")

    rsgislib.tools.filetools.untar_file(input_file, tmp_path, gen_arch_dir=False)

    hello_file = os.path.join(tmp_path, "hello.txt")
    world_file = os.path.join(tmp_path, "world.txt")
    rsgislib_file = os.path.join(tmp_path, "rsgislib.txt")

    success = (
        os.path.exists(hello_file)
        and os.path.exists(world_file)
        and os.path.exists(rsgislib_file)
    )
    assert success


@pytest.mark.skipif((not tar_cmd_avail), reason="tar command not available")
def test_untar_gz_file_gen_arch_dir_true(tmp_path):
    input_file = os.path.join(FILETOOLS_DATA_DIR, "test_file.tar.gz")

    rsgislib.tools.filetools.untar_gz_file(input_file, tmp_path)

    hello_file = os.path.join(tmp_path, "test_file", "hello.txt")
    world_file = os.path.join(tmp_path, "test_file", "world.txt")
    rsgislib_file = os.path.join(tmp_path, "test_file", "rsgislib.txt")

    success = (
        os.path.exists(hello_file)
        and os.path.exists(world_file)
        and os.path.exists(rsgislib_file)
    )
    assert success


@pytest.mark.skipif((not tar_cmd_avail), reason="tar command not available")
def test_untar_gz_file_gen_arch_dir_false(tmp_path):
    input_file = os.path.join(FILETOOLS_DATA_DIR, "test_file.tar.gz")

    rsgislib.tools.filetools.untar_gz_file(input_file, tmp_path, gen_arch_dir=False)

    hello_file = os.path.join(tmp_path, "hello.txt")
    world_file = os.path.join(tmp_path, "world.txt")
    rsgislib_file = os.path.join(tmp_path, "rsgislib.txt")

    success = (
        os.path.exists(hello_file)
        and os.path.exists(world_file)
        and os.path.exists(rsgislib_file)
    )
    assert success


@pytest.mark.skipif(
    on_windows or (not tar_cmd_avail), reason="tar command not available or on Windows"
)
def test_untar_bz_file_gen_arch_dir_true(tmp_path):
    input_file = os.path.join(FILETOOLS_DATA_DIR, "test_file.tar.bz")

    rsgislib.tools.filetools.untar_bz_file(input_file, tmp_path)

    hello_file = os.path.join(tmp_path, "test_file", "hello.txt")
    world_file = os.path.join(tmp_path, "test_file", "world.txt")
    rsgislib_file = os.path.join(tmp_path, "test_file", "rsgislib.txt")

    success = (
        os.path.exists(hello_file)
        and os.path.exists(world_file)
        and os.path.exists(rsgislib_file)
    )
    assert success


@pytest.mark.skipif(
    on_windows or (not tar_cmd_avail), reason="tar command not available or on Windows"
)
def test_untar_bz_file_gen_arch_dir_false(tmp_path):
    input_file = os.path.join(FILETOOLS_DATA_DIR, "test_file.tar.bz")

    rsgislib.tools.filetools.untar_bz_file(input_file, tmp_path, gen_arch_dir=False)

    hello_file = os.path.join(tmp_path, "hello.txt")
    world_file = os.path.join(tmp_path, "world.txt")
    rsgislib_file = os.path.join(tmp_path, "rsgislib.txt")

    success = (
        os.path.exists(hello_file)
        and os.path.exists(world_file)
        and os.path.exists(rsgislib_file)
    )
    assert success


@pytest.mark.skipif((not unzip_cmd_avail), reason="unzip command not available")
def test_unzip_file_gen_arch_dir_true(tmp_path):
    input_file = os.path.join(FILETOOLS_DATA_DIR, "test_file.zip")

    rsgislib.tools.filetools.unzip_file(input_file, tmp_path)

    hello_file = os.path.join(tmp_path, "test_file", "hello.txt")
    world_file = os.path.join(tmp_path, "test_file", "world.txt")
    rsgislib_file = os.path.join(tmp_path, "test_file", "rsgislib.txt")

    success = (
        os.path.exists(hello_file)
        and os.path.exists(world_file)
        and os.path.exists(rsgislib_file)
    )
    assert success


@pytest.mark.skipif((not unzip_cmd_avail), reason="unzip command not available")
def test_unzip_file_gen_arch_dir_false(tmp_path):
    input_file = os.path.join(FILETOOLS_DATA_DIR, "test_file.zip")

    rsgislib.tools.filetools.unzip_file(input_file, tmp_path, gen_arch_dir=False)

    hello_file = os.path.join(tmp_path, "hello.txt")
    world_file = os.path.join(tmp_path, "world.txt")
    rsgislib_file = os.path.join(tmp_path, "rsgislib.txt")

    success = (
        os.path.exists(hello_file)
        and os.path.exists(world_file)
        and os.path.exists(rsgislib_file)
    )
    assert success


def test_delete_file_silent(tmp_path):
    import pathlib

    file_to_del = os.path.join(tmp_path, "hello.txt")

    # Create a file to be deleted.
    pathlib.Path(file_to_del).touch()

    if not os.path.exists(file_to_del):
        success = False  # Something went wrong and file wasn't created.
    else:
        rsgislib.tools.filetools.delete_file_silent(file_to_del)
        # File should no longer exist
        success = not os.path.exists(file_to_del)
    assert success


def test_delete_file_with_basename(tmp_path):
    import pathlib

    # Create some file names where the base is the same but extension is different
    file_1_to_del = os.path.join(tmp_path, "hello.shp")
    file_2_to_del = os.path.join(tmp_path, "hello.shx")
    file_3_to_del = os.path.join(tmp_path, "hello.txt")

    # Create files to be deleted.
    pathlib.Path(file_1_to_del).touch()
    pathlib.Path(file_2_to_del).touch()
    pathlib.Path(file_3_to_del).touch()

    if (
        (not os.path.exists(file_1_to_del))
        or (not os.path.exists(file_2_to_del))
        or (not os.path.exists(file_3_to_del))
    ):
        success = False  # Something went wrong and file wasn't created.
    else:
        rsgislib.tools.filetools.delete_file_with_basename(file_1_to_del)
        # Files should no longer exist
        success = (
            (not os.path.exists(file_1_to_del))
            and (not os.path.exists(file_2_to_del))
            and (not os.path.exists(file_3_to_del))
        )
    assert success
