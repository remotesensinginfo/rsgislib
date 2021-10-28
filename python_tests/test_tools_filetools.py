import os
import pytest

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
FILETOOLS_DATA_DIR = os.path.join(DATA_DIR, "tools/filetools")


@pytest.mark.parametrize(
    "input, expected",
    [("hello.txt", "hello"), ("world/hello.txt", "hello"), ("hello.tar.gz", "hello")],
)
def test_get_file_basename(input, expected):
    import rsgislib.tools.filetools

    assert rsgislib.tools.filetools.get_file_basename(input) == expected


def test_get_file_basename_rm_n_exts_1():
    import rsgislib.tools.filetools

    assert (
        rsgislib.tools.filetools.get_file_basename("hello.tar.gz", rm_n_exts=1)
        == "hello.tar"
    )


def test_get_file_basename_check_valid():
    import rsgislib.tools.filetools

    assert (
        rsgislib.tools.filetools.get_file_basename("hello!.txt", check_valid=True)
        == "hello"
    )


def test_get_file_basename_check_valid_1():
    import rsgislib.tools.filetools

    assert (
        rsgislib.tools.filetools.get_file_basename(
            "hello_world_rsgislib.txt", n_comps=1
        )
        == "hello"
    )


def test_get_file_basename_check_valid_2():
    import rsgislib.tools.filetools

    assert (
        rsgislib.tools.filetools.get_file_basename(
            "hello_world_rsgislib.txt", n_comps=2
        )
        == "hello_world"
    )


def test_get_file_size_bytes():
    import rsgislib.tools.filetools

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert rsgislib.tools.filetools.get_file_size(input_img) == 7749324


def test_get_file_size_kb():
    import rsgislib.tools.filetools

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert (
        abs(rsgislib.tools.filetools.get_file_size(input_img, unit="kb") - 7567.699)
        < 0.1
    )


def test_get_file_size_mb():
    import rsgislib.tools.filetools

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert (
        abs(rsgislib.tools.filetools.get_file_size(input_img, unit="mb") - 7.39) < 0.01
    )


def test_get_file_size_gb():
    import rsgislib.tools.filetools

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert (
        abs(rsgislib.tools.filetools.get_file_size(input_img, unit="gb") - 0.00722)
        < 0.0001
    )


def test_get_file_size_tb():
    import rsgislib.tools.filetools

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert (
        abs(rsgislib.tools.filetools.get_file_size(input_img, unit="tb") - 7.04797e-06)
        < 0.0000001
    )


def test_untar_file_gen_arch_dir_true(tmp_path):
    import rsgislib.tools.filetools

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


def test_untar_file_gen_arch_dir_false(tmp_path):
    import rsgislib.tools.filetools

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


def test_untar_gz_file_gen_arch_dir_true(tmp_path):
    import rsgislib.tools.filetools

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


def test_untar_gz_file_gen_arch_dir_false(tmp_path):
    import rsgislib.tools.filetools

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


def test_untar_bz_file_gen_arch_dir_true(tmp_path):
    import rsgislib.tools.filetools

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


def test_untar_bz_file_gen_arch_dir_false(tmp_path):
    import rsgislib.tools.filetools

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


def test_unzip_file_gen_arch_dir_true(tmp_path):
    import rsgislib.tools.filetools

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


def test_unzip_file_gen_arch_dir_false(tmp_path):
    import rsgislib.tools.filetools

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
