import time
import os
from typing import Dict, List
import ftplib

import rsgislib


def _traverse_mlsd_ftp_dirs(
    ftp_conn: ftplib.FTP, ftp_path: str, ftp_files: Dict, try_n_times: int = 5
) -> List[str]:
    """
    An internal function used within the get_ftp_file_listings function
    to recursively retrieve all the files from an FTP server. This function
    uses the mlsd function which isn't supported by all ftp servers but
    is preferable to use if it is supported.

    Note, this is a recursive function which is used internally. You
    probably want to call get_ftp_file_listings to use this function.

    :param ftp_conn: an ftplib connection object.
    :param ftp_path: The path on the ftp server to start from
    :param ftp_files: of the directory and list of files within that directory
    :param try_n_times: if server connection fails try again (sleeping for
                        5 secs in between) n times for failing.
    :return: list of files
    """
    dirs = list()
    non_dirs = list()
    if ftp_path not in ftp_files:
        ftp_files[ftp_path] = list()

    dir_lst = []
    count = 0
    for i in range(try_n_times):
        if count > try_n_times:
            break
        count = count + 1
        try:
            dir_lst = ftp_conn.mlsd(ftp_path, ["type"])
            break
        except Exception as e:
            print("FTP connection failed but trying again: {0}".format(e))
            time.sleep(5)
            continue
    if count > try_n_times:
        raise rsgislib.RSGISPyException(
            "Tried multiple times which failed to get directory "
            "listing on FTP server so failing."
        )

    for item in dir_lst:
        if item[1]["type"] == "dir":
            c_dir = os.path.join(ftp_path, item[0])
            dirs.append(c_dir)
            if c_dir not in ftp_files:
                ftp_files[c_dir] = list()
        elif not ((item[0] == ".") or (item[0] == "..")):
            c_file = os.path.join(ftp_path, item[0])
            non_dirs.append(c_file)
            ftp_files[ftp_path].append(c_file)

    for subdir in sorted(dirs):
        print(subdir)
        tmp_files_lst = _traverse_mlsd_ftp_dirs(
            ftp_conn, subdir, ftp_files, try_n_times
        )
        non_dirs = non_dirs + tmp_files_lst
    return non_dirs


def _traverse_nlst_ftp_dirs(
    ftp_conn: ftplib.FTP, ftp_path: str, ftp_files: Dict, try_n_times: int = 5
) -> List[str]:
    """
    An internal function used within the get_ftp_file_listings function
    to recursively retrieve all the files from an FTP server. This function
    uses the retrlines function which should be supported by all servers.
    Use this function if _traverse_mlsd_ftp_dirs does not work.

    Note, this is a recursive function which is used internally. You
    probably want to call get_ftp_file_listings to use this function.

    :param ftp_conn: an ftplib connection object.
    :param ftp_path: The path on the ftp server to start from
    :param ftp_files: of the directory and list of files within that directory
    :param try_n_times: if server connection fails try again (sleeping for
                        5 secs in between) n times for failing.
    :return: list of files
    """
    dirs = list()
    non_dirs = list()
    if ftp_path not in ftp_files:
        ftp_files[ftp_path] = list()

    ftp_conn.cwd(ftp_path)

    dir_lst = []
    count = 0
    for i in range(try_n_times):
        if count > try_n_times:
            break
        count = count + 1
        try:
            ftp_conn.retrlines("LIST", dir_lst.append)
            break
        except Exception as e:
            print("FTP connection failed but trying again: {0}".format(e))
            time.sleep(5)
            continue
    if count > try_n_times:
        raise rsgislib.RSGISPyException(
            "Tried multiple times which failed to get directory "
            "listing on FTP server so failing."
        )

    dir_lst = map(str.split, dir_lst)
    for item in dir_lst:
        if item[0][0] == "d":
            c_dir = os.path.join(ftp_path, item[-1])
            dirs.append(c_dir)
            if c_dir not in ftp_files:
                ftp_files[c_dir] = list()
        elif not ((item[-1] == ".") or (item[-1] == "..")):
            c_file = os.path.join(ftp_path, item[-1])
            non_dirs.append(c_file)
            ftp_files[ftp_path].append(c_file)

    for subdir in sorted(dirs):
        print(subdir)
        tmp_files_lst = _traverse_nlst_ftp_dirs(
            ftp_conn, subdir, ftp_files, try_n_times
        )
        non_dirs = non_dirs + tmp_files_lst

    return non_dirs


def get_ftp_file_listings(
    ftp_url: str,
    ftp_path: str,
    ftp_user: str,
    ftp_pass: str,
    ftp_timeout: int = 300,
    try_n_times: int = 5,
    mlsd_not_avail: bool = False,
) -> (Dict[str, str], List[str]):
    """
    Traverse the FTP server directory structure to create a list of all the
    files (full paths).

    Note, it is not good practise to hard code passwords into you
    scripts. It would be recommended that you get the password
    from an environmental variable or config file rather than
    hard code the password.

    :param ftp_url: the url for the ftp server
    :param ftp_path: the path on the ftp server where to start the
                     transverse of the tree.
    :param ftp_user: The username for the ftp server
    :param ftp_pass: the password for the ftp server
    :param ftp_timeout: in seconds (None and system default will be used;
                        system defaults are usual above 300 seconds)
    :param try_n_times: if server connection fails try again (sleeping for 5
                        secs in between) n times for failing.
    :param mlsd_not_avail: Option to support older FTP servers which don't
                           support the mlsd function. Default is False but
                           if you get errors then suggest that you try
                           setting this to True and trying again.
    :return: directory by directory and simple list of files as tuple

    """
    ftp_files = dict()
    print("Opening FTP Connection to {}".format(ftp_url))
    ftp_conn = ftplib.FTP(ftp_url, timeout=ftp_timeout)
    ftp_conn.login(user=ftp_user, passwd=ftp_pass)
    print("Traverse the file system and get a list of paths")
    if mlsd_not_avail:
        non_dirs_lst = _traverse_nlst_ftp_dirs(
            ftp_conn, ftp_path, ftp_files, try_n_times
        )
    else:
        non_dirs_lst = _traverse_mlsd_ftp_dirs(
            ftp_conn, ftp_path, ftp_files, try_n_times
        )
    ftp_conn.quit()
    print("Finished traversing the ftp server file system.")
    return ftp_files, non_dirs_lst


def create_file_listings_db(
    db_json: str,
    ftp_url: str,
    ftp_path: str,
    ftp_user: str,
    ftp_pass: str,
    ftp_timeout: int = 300,
    try_n_times: int = 5,
    mlsd_not_avail: bool = False,
):
    """
    A function which transverses the directory structure on an ftp server
    and saves the result to a JSON database using the pysondb module.

    Note, it is not good practise to hard code passwords into you
    scripts. It would be recommended that you get the password
    from an environmental variable or config file rather than
    hard code the password.

    :param db_json: The file path for the databases JSON file.
    :param ftp_url: the url for the ftp server
    :param ftp_path: the path on the ftp server where to start the
                     transverse of the tree.
    :param ftp_user: The username for the ftp server
    :param ftp_pass: the password for the ftp server
    :param ftp_timeout: in seconds (None and system default will be used;
                        system defaults are usual above 300 seconds)
    :param try_n_times: if server connection fails try again (sleeping for 5
                        secs in between) n times for failing.
    :param mlsd_not_avail: Option to support older FTP servers which don't
                           support the mlsd function. Default is False but
                           if you get errors then suggest that you try
                           setting this to True and trying again.

    """
    import pysondb
    import tqdm

    ftp_files, non_dirs_lst = get_ftp_file_listings(
        ftp_url, ftp_path, ftp_user, ftp_pass, ftp_timeout, try_n_times, mlsd_not_avail
    )

    lst_db = pysondb.getDb(db_json)
    db_data = []
    for c_file in tqdm.tqdm(non_dirs_lst):
        db_data.append(
            {
                "ftp_url": ftp_url,
                "rmt_path": c_file,
                "lcl_path": "",
                "downloaded": False,
            }
        )

    lst_db.addMany(db_data)


def download_ftp_file(
    ftp_url: str,
    remote_path: str,
    local_path: str,
    time_out: int = 300,
    ftp_user: str = None,
    ftp_pass: str = None,
    print_info: bool = True,
) -> bool:
    """
    A function to download a file from an FTP server.

    :param ftp_url: The remote URL for the ftp server.
    :param remote_path: the remote path on the ftp server for the file to be downloaded
    :param local_path: the local path to where the file should be downloaded to.
    :param time_out: the timeout for the download. Default: 300 seconds.
    :param ftp_user: the username, if required, for the ftp server.
    :param ftp_pass: the password, if required, for the ftp server.
    :param print_info: bool for whether info should be printed to the console
                       (default: True)
    :return: boolean as to whether the file was successfully downloaded or not.

    """
    try:
        ftp_conn = ftplib.FTP(ftp_url, timeout=time_out)
        ftp_conn.login(user=ftp_user, passwd=ftp_pass)

        # Write file in binary mode
        if print_info:
            print(f"Downloading: {remote_path}")
        with open(local_path, "wb") as lcl_file_obj:
            # Command for Downloading the file "RETR remote_path"
            ftp_conn.retrbinary(f"RETR {remote_path}", lcl_file_obj.write)
        if print_info:
            print(f"Finished Downloading: {local_path}")
        ftp_conn.quit()
    except:
        return False

    return True


def download_curl_ftp_file(
    ftp_url: str,
    remote_path: str,
    local_path: str,
    time_out: int = 300,
    ftp_user: str = None,
    ftp_pass: str = None,
    print_info: bool = True,
) -> bool:
    """
    A function to download a file from an FTP server using curl.

    :param ftp_url: The remote URL for the ftp server.
    :param remote_path: the remote path on the ftp server for the file to be downloaded
    :param local_path: the local path to where the file should be downloaded to.
    :param time_out: the timeout for the download. Default: 300 seconds.
    :param ftp_user: the username, if required, for the ftp server.
    :param ftp_pass: the password, if required, for the ftp server.
    :param print_info: bool for whether info should be printed to the console
                       (default: True)
    :return: boolean as to whether the file was successfully downloaded or not.

    """
    import pycurl

    full_path_url = ftp_url + remote_path
    try:
        fp = open(local_path, "wb")
        curl = pycurl.Curl()
        curl.setopt(pycurl.URL, full_path_url)
        curl.setopt(pycurl.FOLLOWLOCATION, True)
        if print_info:
            curl.setopt(pycurl.NOPROGRESS, 0)
        else:
            curl.setopt(pycurl.NOPROGRESS, 1)
        curl.setopt(pycurl.FOLLOWLOCATION, 1)
        curl.setopt(pycurl.MAXREDIRS, 5)
        curl.setopt(pycurl.CONNECTTIMEOUT, 50)
        curl.setopt(pycurl.TIMEOUT, time_out)
        curl.setopt(pycurl.FTP_RESPONSE_TIMEOUT, 600)
        curl.setopt(pycurl.NOSIGNAL, 1)
        if (not ftp_user is None) and (not ftp_pass is None):
            curl.setopt(pycurl.HTTPAUTH, pycurl.HTTPAUTH_ANY)
            curl.setopt(pycurl.USERPWD, ftp_user + ":" + ftp_pass)
        curl.setopt(pycurl.WRITEDATA, fp)
        if print_info:
            print("Starting download of {}".format(full_path_url))
        curl.perform()
        if print_info:
            print(
                "Finished download in {0} of {1} bytes for {2}".format(
                    curl.getinfo(curl.TOTAL_TIME),
                    curl.getinfo(curl.SIZE_DOWNLOAD),
                    full_path_url,
                )
            )
        success = True
    except:
        print(
            "An error occurred when downloading {}.".format(
                os.path.join(ftp_url, remote_path)
            )
        )
        success = False
    return success


def download_files_use_lst_db(
    db_json: str,
    out_dir_path: str,
    time_out: int = 300,
    ftp_user: str = None,
    ftp_pass: str = None,
    create_dir_struct: bool = False,
    use_curl: bool = False,
):
    """
    A function which uses the pysondb JSON database to download all the files
    recording whether files have been downloaded successful and the output
    path for the file.

    :param db_json: file path for the JSON db file.
    :param out_dir_path: the output path where data should be downloaded to.
    :param time_out: the timeout for the download. Default: 300 seconds.
    :param ftp_user: the username, if required, for the ftp server.
    :param ftp_pass: the password, if required, for the ftp server.
    :param create_dir_struct: boolean specifying whether the folder structure on the
                              ftp server should be maintained within the out_dir_path
                              (True) or ignored and all the individual files just
                              downloaded into the output directory as a flat
                              structure (False; Default).
    :param use_curl: boolean specifying whether to use CURL to download the files.
                     (Default: False).

    """
    import pysondb
    import tqdm

    lst_db = pysondb.getDb(db_json)

    dwld_files = lst_db.getBy({"downloaded": False})

    for dwn_file in tqdm.tqdm(dwld_files):
        basename = os.path.basename(dwn_file["rmt_path"])
        if create_dir_struct:
            dir_path = os.path.dirname(dwn_file["rmt_path"])
            if dir_path[0] == "/":
                dir_path = dir_path[1:]
            local_dir_path = os.path.join(out_dir_path, dir_path)
            if not os.path.exists(local_dir_path):
                os.makedirs(local_dir_path)
            local_path = os.path.join(local_dir_path, basename)
        else:
            local_path = os.path.join(out_dir_path, basename)
        if use_curl:
            dwnlded = download_curl_ftp_file(
                ftp_url=dwn_file["ftp_url"],
                remote_path=dwn_file["rmt_path"],
                local_path=local_path,
                time_out=time_out,
                ftp_user=ftp_user,
                ftp_pass=ftp_pass,
                print_info=False,
            )
        else:
            dwnlded = download_ftp_file(
                ftp_url=dwn_file["ftp_url"],
                remote_path=dwn_file["rmt_path"],
                local_path=local_path,
                time_out=time_out,
                ftp_user=ftp_user,
                ftp_pass=ftp_pass,
                print_info=False,
            )
        if dwnlded:
            lst_db.updateById(
                dwn_file["id"], {"lcl_path": local_path, "downloaded": True}
            )
