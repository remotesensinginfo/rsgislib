import os
import pytest

PYSON_DB_NOT_AVAIL = False
try:
    import pysondb
except ImportError:
    PYSON_DB_NOT_AVAIL = True

PYCURL_DB_NOT_AVAIL = False
try:
    import pycurl
except ImportError:
    PYCURL_DB_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
TOOLS_UTILS_DATA_DIR = os.path.join(DATA_DIR, "tools", "utils")

# Using test ftp service provided by: https://dlptest.com/ftp-test/
ftp_url_ref = "ftp.dlptest.com"
ftp_user_ref = "dlpuser"
ftp_pass_ref = "rNrKYTX9g7z3RgJRmxWuGHbeu"


def test_upload_ftp_file():
    import rsgislib.tools.ftptools

    test_file = os.path.join(TOOLS_UTILS_DATA_DIR, "basic_str.txt")
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )


def test_get_ftp_file_listings():
    import rsgislib.tools.ftptools

    test_file = os.path.join(TOOLS_UTILS_DATA_DIR, "basic_str.txt")
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test/test",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )
    ftp_files, non_dirs_lst = rsgislib.tools.ftptools.get_ftp_file_listings(
        ftp_url=ftp_url_ref,
        ftp_path="/",
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        mlsd_not_avail=True,
    )

    assert len(non_dirs_lst) > 0


@pytest.mark.skipif(PYSON_DB_NOT_AVAIL, reason="pysondb dependency not available")
def test_create_file_listings_db(tmp_path):
    import rsgislib.tools.ftptools

    test_file = os.path.join(TOOLS_UTILS_DATA_DIR, "basic_str.txt")
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test/test",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )

    out_db_file = os.path.join(tmp_path, "db_file.json")
    rsgislib.tools.ftptools.create_file_listings_db(
        db_json=out_db_file,
        ftp_url=ftp_url_ref,
        ftp_path="/",
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        mlsd_not_avail=True,
    )

    assert os.path.exists(out_db_file)


def test_download_ftp_file(tmp_path):
    import rsgislib.tools.ftptools

    test_file = os.path.join(TOOLS_UTILS_DATA_DIR, "basic_str.txt")
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test/test",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )

    out_file = os.path.join(tmp_path, "test_dnwld.txt")
    rsgislib.tools.ftptools.download_ftp_file(
        ftp_url=ftp_url_ref,
        remote_file="/rsgislib_test/test/basic_str.txt",
        local_file=out_file,
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
    )

    assert os.path.exists(out_file)


@pytest.mark.skipif(PYCURL_DB_NOT_AVAIL, reason="pycurl dependency not available")
def test_download_curl_ftp_file(tmp_path):
    import rsgislib.tools.ftptools

    test_file = os.path.join(TOOLS_UTILS_DATA_DIR, "basic_str.txt")
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test/test",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )

    out_file = os.path.join(tmp_path, "test_dnwld.txt")
    rsgislib.tools.ftptools.download_curl_ftp_file(
        ftp_url=ftp_url_ref,
        remote_file="/rsgislib_test/test/basic_str.txt",
        local_file=out_file,
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
    )

    assert os.path.exists(out_file)


# @pytest.mark.skipif(PYSON_DB_NOT_AVAIL, reason="pysondb dependency not available")
@pytest.mark.skipif(True, reason="Skipping from automatically running.")
def test_download_files_use_lst_db_no_struct(tmp_path):
    import rsgislib.tools.ftptools

    test_file = os.path.join(TOOLS_UTILS_DATA_DIR, "basic_str.txt")
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test2",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test2/test1",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test2/test2",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test2/test1/test3",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test2/test2/test4",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )

    db_file = os.path.join(tmp_path, "db_file.json")
    rsgislib.tools.ftptools.create_file_listings_db(
        db_json=db_file,
        ftp_url=ftp_url_ref,
        ftp_path="/rsgislib_test2",
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        mlsd_not_avail=True,
    )

    out_dir = os.path.join(tmp_path, "tmp")
    os.makedirs(out_dir)

    rsgislib.tools.ftptools.download_files_use_lst_db(
        db_json=db_file,
        out_dir_path=out_dir,
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        create_dir_struct=False,
        use_curl=False,
    )


# @pytest.mark.skipif(PYSON_DB_NOT_AVAIL, reason="pysondb dependency not available")
@pytest.mark.skipif(True, reason="Skipping from automatically running.")
def test_download_files_use_lst_db_struct(tmp_path):
    import rsgislib.tools.ftptools

    test_file = os.path.join(TOOLS_UTILS_DATA_DIR, "basic_str.txt")
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test3",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test3/test1",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test3/test2",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test3/test1/test3",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )
    rsgislib.tools.ftptools.upload_ftp_file(
        test_file,
        ftp_url=ftp_url_ref,
        remote_path="/rsgislib_test3/test2/test4",
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        print_info=True,
    )

    db_file = os.path.join(tmp_path, "db_file.json")
    rsgislib.tools.ftptools.create_file_listings_db(
        db_json=db_file,
        ftp_url=ftp_url_ref,
        ftp_path="/rsgislib_test2",
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        mlsd_not_avail=True,
    )

    out_dir = os.path.join(tmp_path, "tmp")
    os.makedirs(out_dir)

    rsgislib.tools.ftptools.download_files_use_lst_db(
        db_json=db_file,
        out_dir_path=out_dir,
        ftp_timeout=300,
        ftp_user=ftp_user_ref,
        ftp_pass=ftp_pass_ref,
        create_dir_struct=True,
        use_curl=False,
    )
