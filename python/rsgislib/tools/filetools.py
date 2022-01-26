#!/usr/bin/env python
"""
The tools file manipulation module contains functions for manipulating and
moving files around.
"""

# Import modules
import glob
import os
import sys
import errno
import tempfile
import shutil
import datetime
import time
from typing import Union

import rsgislib


def get_file_basename(
    input_file: str, check_valid: bool = False, n_comps: int = 0, rm_n_exts: int = 0
) -> str:
    """
    Uses os.path module to return file basename (i.e., path and extension removed)

    :param input_file: string for the input file name and path
    :param check_valid: if True then resulting basename will be checked for punctuation
                       characters (other than underscores) and spaces, punctuation
                       will be either removed and spaces changed to an underscore.
                       (Default = False)
    :param n_comps: if > 0 then the resulting basename will be split using underscores
                    and the return based name will be defined using the n_comps
                    components split by under scores.
    :param rm_n_exts: used where an input file has more than one extension
                      (e.g., tar.gz) and only n extensions should be removed.
                      Default: 0 which will removed all extensions calculated
                      based on the number of full-stops (.) within the file name.
                      If a value of 1 was provided for filename.tar.gz then the
                      returns output would be filename.tar.
    :return: basename for file

    """
    import string

    file_base_name = os.path.basename(input_file)
    n_exts = file_base_name.count(".")
    if (rm_n_exts == 0) or (rm_n_exts > n_exts):
        rm_n_exts = n_exts

    basename = file_base_name
    if n_exts > 0:
        for i in range(rm_n_exts):
            basename = os.path.splitext(basename)[0]

    if check_valid:
        basename = basename.replace(" ", "_")
        for punct in string.punctuation:
            if (punct != "_") and (punct != "-"):
                basename = basename.replace(punct, "")
    if n_comps > 0:
        basename_split = basename.split("_")
        if len(basename_split) < n_comps:
            raise rsgislib.RSGISPyException(
                "The number of components specified is more than the number "
                "of components in the basename."
            )
        out_basename = ""
        for i in range(n_comps):
            if i == 0:
                out_basename = basename_split[i]
            else:
                out_basename = out_basename + "_" + basename_split[i]
        basename = out_basename
    return basename


def is_path_valid(file_path_name: str) -> bool:
    """
    This function tests whether a file path is valid in terms of the length of
    each component and the characters used. Should be cross platform.

    This function was adapted from the answer here:
    https://stackoverflow.com/questions/9532499/check-whether-a-path-is-valid-in-python-without-creating-a-file-at-the-paths-ta

    :param file_path_name: the file path to be tested
    :return: True if the passed pathname is a valid pathname for the current OS;

    """
    # Windows-specific error code indicating an invalid pathname.
    ERROR_INVALID_NAME = 123
    # See Also
    # ----------
    # https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-
    #    Official listing of all such codes.

    # If this pathname is either not a string or is but is empty, this pathname
    # is invalid.
    try:
        if not isinstance(file_path_name, str) or not file_path_name:
            return False

        # Strip this pathname's Windows-specific drive specifier (e.g., `C:\`)
        # if any. Since Windows prohibits path components from containing `:`
        # characters, failing to strip this `:`-suffixed prefix would
        # erroneously invalidate all valid absolute Windows pathnames.
        _, file_path_name = os.path.splitdrive(file_path_name)

        # Directory guaranteed to exist. If the current OS is Windows, this is
        # the drive to which Windows was installed (e.g., the "%HOMEDRIVE%"
        # environment variable); else, the typical root directory.
        root_dirname = (
            os.environ.get("HOMEDRIVE", "C:")
            if sys.platform == "win32"
            else os.path.sep
        )
        assert os.path.isdir(root_dirname)  # ...Murphy and her ironclad Law

        # Append a path separator to this directory if needed.
        root_dirname = root_dirname.rstrip(os.path.sep) + os.path.sep

        # Test whether each path component split from this pathname is valid or
        # not, ignoring non-existent and non-readable path components.
        for pathname_part in file_path_name.split(os.path.sep):
            try:
                os.lstat(root_dirname + pathname_part)
            # If an OS-specific exception is raised, its error code
            # indicates whether this pathname is valid or not. Unless this
            # is the case, this exception implies an ignorable kernel or
            # filesystem complaint (e.g., path not found or inaccessible).
            #
            # Only the following exceptions indicate invalid pathnames:
            #
            # * Instances of the Windows-specific "WindowsError" class
            #   defining the "winerror" attribute whose value is
            #   "ERROR_INVALID_NAME". Under Windows, "winerror" is more
            #   fine-grained and hence useful than the generic "errno"
            #   attribute. When a too-long pathname is passed, for example,
            #   "errno" is "ENOENT" (i.e., no such file or directory) rather
            #   than "ENAMETOOLONG" (i.e., file name too long).
            # * Instances of the cross-platform "OSError" class defining the
            #   generic "errno" attribute whose value is either:
            #   * Under most POSIX-compatible OSes, "ENAMETOOLONG".
            #   * Under some edge-case OSes (e.g., SunOS, *BSD), "ERANGE".
            except ValueError as e:
                return False
            except OSError as exc:
                if hasattr(exc, "winerror"):
                    if exc.winerror == ERROR_INVALID_NAME:
                        return False
                elif exc.errno in {errno.ENAMETOOLONG, errno.ERANGE}:
                    return False
    # If a "TypeError" exception was raised, it almost certainly has the
    # error message "embedded NUL character" indicating an invalid pathname.
    except TypeError as exc:
        return False
    # If no exception was raised, all path components and hence this
    # pathname itself are valid.
    else:
        return True


def is_path_sibling_creatable(file_path_name: str) -> bool:
    """
    A function which checks whether the file path provided would
    be creatable (i.e., directory exists and you have permission to
    write to it.

    This function was adapted from the answer here:
    https://stackoverflow.com/questions/9532499/check-whether-a-path-is-valid-in-python-without-creating-a-file-at-the-paths-ta

    :param file_path_name: the input file path to be checked.
    :return: True if the current user has sufficient permissions to create **siblings**
             (i.e., arbitrary files in the parent directory) of the passed pathname

    """
    # Parent directory of the passed path. If empty, we substitute the current
    # working directory (CWD) instead.
    dirname = os.path.dirname(file_path_name) or os.getcwd()

    try:
        # For safety, explicitly close and hence delete this temporary file
        # immediately after creating it in the passed path's parent directory.
        with tempfile.TemporaryFile(dir=dirname):
            pass
        return True
    # While the exact type of exception raised by the above function depends on
    # the current version of the Python interpreter, all such types subclass the
    # following exception superclass.
    except EnvironmentError:
        return False


def does_path_exists_or_creatable(file_path_name: str) -> bool:
    """
    A function which checks whether a file path either exists or is
    creatable if it does not exist.

    This function was adapted from the answer here:
    https://stackoverflow.com/questions/9532499/check-whether-a-path-is-valid-in-python-without-creating-a-file-at-the-paths-ta

    :param file_path_name: the input file path to be checked.
    :return: True if the passed pathname is a valid pathname on the current OS _and_
             either currently exists or is hypothetically creatable in a cross-platform
             manner optimized for POSIX-unfriendly filesystems

    """
    try:
        # To prevent "os" module calls from raising undesirable exceptions on
        # invalid path names, is_path_valid() is explicitly called first.
        return is_path_valid(file_path_name) and (
            os.path.exists(file_path_name) or is_path_sibling_creatable(file_path_name)
        )
    # Report failure on non-fatal filesystem complaints (e.g., connection
    # timeouts, permissions issues) implying this path to be inaccessible. All
    # other exceptions are unrelated fatal issues and should not be caught here.
    except OSError:
        return False


def get_dir_name(input_file: str) -> str:
    """
    A function which returns just the name of the directory of the input file
    without the rest of the path.

    :param input_file: string for the input file name and path
    :return: directory name

    """
    input_file = os.path.abspath(input_file)
    dir_path = os.path.dirname(input_file)
    dir_name = os.path.basename(dir_path)
    return dir_name


def delete_file_silent(input_file: str) -> bool:
    """
    A function which can be used in-place of os.remove to delete
    a file but if checks if the file exists and only calls os.remove
    if it does exist but also catches any Exceptions from os.remove
    and just returns a boolean as to whether the input_file has been
    removed.

    :param input_file: input file path for the file which is to be removed.
    :return: boolean (True: File was removed or did not exist. False:
             os.remove through an Exception so assume file was not removed)

    """
    try:
        if os.path.exists(input_file) and os.path.isfile(input_file):
            os.remove(input_file)
        file_removed = True
    except Exception:
        file_removed = False
    return file_removed


def delete_file_with_basename(input_file: str, print_rms=True):
    """
    Function to delete all the files which have a path
    and base name defined in the input_file attribute.

    :param input_file: string for the input file name and path
    :param print_rms: print the files being deleted (Default: True)

    """
    baseName = os.path.splitext(input_file)[0]
    fileList = glob.glob(baseName + str(".*"))
    for file in fileList:
        if print_rms:
            print("Deleting file: " + str(file))
        delete_file_silent(file)


def find_file(dir_path: str, file_search: str) -> str:
    """
    Search for a single file with a path using glob. Therefore, the file
    path returned is a true path. Within the file_search provide the file
    name with '*' as wildcard(s).

    :param dir_path: string for the input directory path
    :param file_search: string with a * wildcard for the file being searched for.
    :return: string with the path to the file

    .. code:: python

        import rsgislib.tools.filetools
        file_path = rsgislib.tools.filetools.find_file("in/dir", "*N15W093*.tif")

    """
    files = glob.glob(os.path.join(dir_path, file_search))
    if len(files) != 1:
        raise rsgislib.RSGISPyException(
            "Could not find a single file ("
            + file_search
            + "); found "
            + str(len(files))
            + " files."
        )
    return files[0]


def find_file_none(dir_path: str, file_search: str) -> Union[None, str]:
    """
    Search for a single file with a path using glob. Therefore, the file
    path returned is a true path. Within the file_search provide the file
    name with '*' as wildcard(s). Returns None is not found.

    :param dir_path: string for the input directory path
    :param file_search: string with a * wildcard for the file being searched for.
    :return: string with the path to the file

    .. code:: python

        import rsgislib.tools.filetools
        file_path = rsgislib.tools.filetools.find_file_none("in/dir", "*N15W093*.tif")
        if file_path is not None:
            print(file_path)

    """
    files = glob.glob(os.path.join(dir_path, file_search))
    if len(files) != 1:
        return None
    return files[0]


def find_files_ext(dir_path: str, ending: str) -> dict:
    """
    Find all the files within a directory structure with a specific file ending.
    The files are return as dictionary using the file name as the dictionary key.
    This means you cannot have files with the same name within the structure.

    :param dir_path: the base directory path within which to search.
    :param ending: the file ending (e.g., .txt, or txt or .kea, kea).
    :return: dict with file name as key

    .. code:: python

        import rsgislib.tools.filetools
        file_paths = rsgislib.tools.filetools.find_files_ext("in/dir", ".tif")

    """
    out_file_dict = dict()
    for root, dirs, files in os.walk(dir_path):
        for file in files:
            if file.endswith(ending):
                file_found = os.path.join(root, file)
                if os.path.isfile(file_found):
                    out_file_dict[file] = file_found
    return out_file_dict


def find_files_mpaths_ext(dir_paths: list, ending: str) -> dict:
    """
    Find all the files within a list of input directories and the structure beneath
    with a specific file ending. The files are return as dictionary using the file
    name as the dictionary key. This means you cannot have files with the same name
    within the structure.

    :param dir_paths: a list of base directory paths within which to search.
    :param ending: the file ending (e.g., .txt, or txt or .kea, kea).
    :return: dict with file name as key

    .. code:: python

        import rsgislib.tools.filetools
        dir_paths = ["in/dir", "test/dir", "img/files"]
        file_paths = rsgislib.tools.filetools.find_files_mpaths_ext(dir_paths, ".tif")


    """
    out_file_dict = dict()
    for dir_path in dir_paths:
        for root, dirs, files in os.walk(dir_path):
            for file in files:
                if file.endswith(ending):
                    file_found = os.path.join(root, file)
                    if os.path.isfile(file_found):
                        out_file_dict[file] = file_found
    return out_file_dict


def find_first_file(dir_path: str, file_search: str, rtn_except: bool = True) -> str:
    """
    Search for a single file with a path using glob. Therefore, the file
    path returned is a true path. Within the file_search provide the file
    name with '*' as wildcard(s).

    :param dir_path: The directory within which to search, note that the search will
                     be within sub-directories within the base directory until a file
                     meeting the search criteria are met.
    :param file_search: The file search string in the file name and must contain a
                        wild character (i.e., *).
    :param rtn_except: if True then an exception will be raised if no file or multiple
                       files are found (default). If False then None will be returned
                       rather than an exception raised.
    :return: The file found (or None if rtn_except=False)

    .. code:: python

        import rsgislib.tools.filetools
        file_paths = rsgislib.tools.filetools.find_first_file("in/dir", "*N15W093*.tif")

    """
    files = None
    for root, dirs, files in os.walk(dir_path):
        files = glob.glob(os.path.join(root, file_search))
        if len(files) > 0:
            break
    out_file = None
    if (files is not None) and (len(files) == 1):
        out_file = files[0]
    elif rtn_except:
        raise rsgislib.RSGISPyException(
            "Could not find a single file ({0}) in {1}; "
            "found {2} files.".format(file_search, dir_path, len(files))
        )
    return out_file


def get_files_mod_time(
    file_lst: list,
    dt_before: datetime.datetime = None,
    dt_after: datetime.datetime = None,
) -> list:
    """
    A function which subsets a list of files based on datetime of
    last modification. The function also does a check as to whether
    a file exists, files which don't exist will be ignored.

    :param file_lst: The list of file path - represented as strings.
    :param dt_before: a datetime object with a date/time where files modified
                      before this will be returned
    :param dt_after: a datetime object with a date/time where files modified
                     after this will be returned

    Example:

    .. code:: python

        import glob
        import datetime
        import rsgislib.tools.filetools

        input_files = glob.glob("in/dir/*.tif")
        dt_before = datetime.datetime(year=2020, month=12, day=25, hour=12, minute=30)
        file_path = rsgislib.tools.filetools.get_files_mod_time(input_files, dt_before)

    """
    if (dt_before is None) and (dt_after is None):
        raise rsgislib.RSGISPyException(
            "You must define at least one of dt_before or dt_after"
        )
    out_file_lst = list()
    for cfile in file_lst:
        if os.path.exists(cfile):
            mod_time_stamp = os.path.getmtime(cfile)
            mod_time = datetime.datetime.fromtimestamp(mod_time_stamp)
            if (dt_before is not None) and (mod_time < dt_before):
                out_file_lst.append(cfile)
            if (dt_after is not None) and (mod_time > dt_after):
                out_file_lst.append(cfile)
    return out_file_lst


def find_files_size_limits(
    dir_path: str, file_search: str, min_size: int = 0, max_size: int = None
) -> list:
    """
    Search for files with a path using glob. Therefore, the file
    paths returned is a true path. Within the file_search provide the file
    names with '*' as wildcard(s).

    :param dir_path: string for the input directory path
    :param file_search: string with a * wildcard for the file being searched for.
    :param min_size: the minimum file size in bytes (default is 0)
    :param max_size: the maximum file size in bytes, if None (default) then ignored.
    :return: string with the path to the file

    Example:

    .. code:: python

        import rsgislib.tools.filetools
        file_paths = rsgislib.tools.filetools.find_files_size_limits("in/dir",
                                                                     "*N15W093*.tif",
                                                                     0, 100000)

    """
    files = glob.glob(os.path.join(dir_path, file_search))
    out_files = list()
    for c_file in files:
        file_size = get_file_size(c_file)
        if (max_size is None) and (file_size > min_size):
            out_files.append(c_file)
        elif (file_size > min_size) and (file_size < max_size):
            out_files.append(c_file)
    return out_files


def is_cmd_tool_avail(cmd_name: str):
    """
    A function which finds if an executable command is available
    on the system path.

    :param cmd_name: the name of the command to test (e.g., tar, gzip etc.)
    :return: boolean (True: Command available. False Command not available)
    """
    return shutil.which(cmd_name) is not None


def file_is_hidden(dir_path: str) -> bool:
    """
    A function to test whether a file or folder is 'hidden' or not on the
    file system. Should be cross platform between Linux/UNIX and windows.

    :param dir_path: input file path to be tested
    :return: boolean (True = hidden)

    Example:

    .. code:: python

        import rsgislib.tools.filetools
        if rsgislib.tools.filetools.file_is_hidden("in/dir/img.kea"):
            print("File is hidden")

    """
    dir_path = os.path.abspath(dir_path)
    if os.name == "nt":
        import win32api
        import win32con

        attribute = win32api.GetFileAttributes(dir_path)
        return attribute & (
            win32con.FILE_ATTRIBUTE_HIDDEN | win32con.FILE_ATTRIBUTE_SYSTEM
        )
    else:
        file_name = os.path.basename(dir_path)
        return file_name.startswith(".")


def get_dir_list(dir_path: str, inc_hidden: bool = False) -> list:
    """
    Function which get the list of directories within the specified path.

    :param dir_path: file path to search within
    :param inc_hidden: boolean specifying whether hidden files should be
                       included (default=False)
    :return: list of directory paths

    Example:

    .. code:: python

        import rsgislib.tools.filetools
        files = rsgislib.tools.filetools.get_dir_list("in/dir")

    """
    out_dir_lst = list()
    dir_listing = os.listdir(dir_path)
    for item in dir_listing:
        c_path = os.path.join(dir_path, item)
        if os.path.isdir(c_path):
            if not inc_hidden:
                if not file_is_hidden(c_path):
                    out_dir_lst.append(c_path)
            else:
                out_dir_lst.append(c_path)
    return out_dir_lst


def convert_file_size_units(in_size: int, in_unit: str, out_unit: str) -> float:
    """
    A function which converts between file size units

    :param in_size: input file size
    :param in_unit: the input unit for the file size. Options: bytes, kb, mb, gb, tb
    :param out_unit: the output unit for the file size. Options: bytes, kb, mb, gb, tb
    :return: float for the output file size

    """
    in_unit = in_unit.lower()
    if in_unit not in ["bytes", "kb", "mb", "gb", "tb"]:
        raise rsgislib.RSGISPyException(
            "Input unit must be one of: bytes, kb, mb, gb, tb"
        )

    out_unit = out_unit.lower()
    if out_unit not in ["bytes", "kb", "mb", "gb", "tb"]:
        raise rsgislib.RSGISPyException(
            "Output unit must be one of: bytes, kb, mb, gb, tb"
        )

    if in_unit == "bytes":
        file_size_bytes = in_size
    elif in_unit == "kb":
        file_size_bytes = in_size * 1024.0
    elif in_unit == "mb":
        file_size_bytes = in_size * (1024.0 ** 2)
    elif in_unit == "gb":
        file_size_bytes = in_size * (1024.0 ** 3)
    elif in_unit == "tb":
        file_size_bytes = in_size * (1024.0 ** 4)
    else:
        raise rsgislib.RSGISPyException("Input unit it not recognised.")

    if out_unit == "bytes":
        out_file_size = file_size_bytes
    elif out_unit == "kb":
        out_file_size = file_size_bytes / 1024.0
    elif out_unit == "mb":
        out_file_size = file_size_bytes / (1024.0 ** 2)
    elif out_unit == "gb":
        out_file_size = file_size_bytes / (1024.0 ** 3)
    elif out_unit == "tb":
        out_file_size = file_size_bytes / (1024.0 ** 4)
    else:
        raise rsgislib.RSGISPyException("Output unit it not recognised.")

    return out_file_size


def get_file_size(file_path: str, unit: str = "bytes") -> float:
    """
    A function which returns the file size of a file in the specified unit.

    Units:
    * bytes
    * kb - kilobytes  (bytes / 1024)
    * mb - megabytes  (bytes / 1024^2)
    * gb - gigabytes  (bytes / 1024^3)
    * tb - terabytes  (bytes / 1024^4)

    :param file_path: the path to the file for which the size is to be calculated.
    :param unit: the unit for the file size. Options: bytes, kb, mb, gb, tb
    :return: float for the file size.

    """
    import pathlib

    unit = unit.lower()
    if unit not in ["bytes", "kb", "mb", "gb", "tb"]:
        raise rsgislib.RSGISPyException("Unit must be one of: bytes, kb, mb, gb, tb")

    p = pathlib.Path(file_path)
    if p.exists() and p.is_file():
        file_size_bytes = p.stat().st_size
        out_file_size = convert_file_size_units(file_size_bytes, "bytes", unit)
    else:
        raise rsgislib.RSGISPyException("Input file path does not exist")
    return out_file_size


def get_file_lock(
    input_file: str,
    sleep_period: int = 1,
    wait_iters: int = 120,
    use_except: bool = False,
) -> bool:
    """
    A function which gets a lock on a file.

    The lock file will be a unix hidden file (i.e., starts with a .) and it will
    have .lok added to the end. E.g., for input file hello_world.txt the lock file
    will be .hello_world.txt.lok. The contents of the lock file will be the time and
    date of creation.

    Using the default parameters (sleep 1 second and wait 120 iterations) if the
    lock isn't available it will be retried every second for 120 seconds (i.e., 2 mins).

    :param input_file: The input file for which the lock will be created.
    :param sleep_period: time in seconds to sleep for, if the lock isn't
                         available. (Default=1 second)
    :param wait_iters: the number of iterations to wait for before giving
                       up. (Default=120)
    :param use_except: Boolean. If True then an exception will be thrown if the lock
                       is not available. If False (default) False will be returned
                       if the lock is not successful.
    :return: boolean. True: lock was successfully gained. False: lock was not gained.

    """
    file_path, file_name = os.path.split(input_file)
    lock_file_name = ".{}.lok".format(file_name)
    lock_file_path = os.path.join(file_path, lock_file_name)

    got_lock = False
    for i in range(wait_iters + 1):
        if not os.path.exists(lock_file_path):
            got_lock = True
            break
        time.sleep(sleep_period)

    if got_lock:
        c_datetime = datetime.datetime.now()
        f = open(lock_file_path, "w")
        f.write("{}\n".format(c_datetime.isoformat()))
        f.flush()
        f.close()
    elif use_except:
        raise rsgislib.RSGISPyException(
            "Lock could not be gained for file: {}".format(input_file)
        )

    return got_lock


def release_file_lock(input_file: str):
    """
    A function which releases a lock file for the input file.

    :param input_file: The input file for which the lock will be created.

    """
    file_path, file_name = os.path.split(input_file)
    lock_file_name = ".{}.lok".format(file_name)
    lock_file_path = os.path.join(file_path, lock_file_name)
    if os.path.exists(lock_file_path):
        if not delete_file_silent(lock_file_path):
            raise rsgislib.RSGISPyException("Could not delete the lock file..."
                                            " something has gone wrong!")


def clean_file_locks(dir_path: str, timeout: int = 3600):
    """
    A function which cleans up any remaining lock file (i.e., if an application
    has crashed). The timeout time will be compared with the time written within
    the file.

    :param dir_path: the file path to search for lock files (i.e., ".*.lok")
    :param timeout: the time (in seconds) for the timeout. Default: 3600 (1 hours)

    """
    import rsgislib.tools.utils

    c_dateime = datetime.datetime.now()
    lock_files = glob.glob(os.path.join(dir_path, ".*.lok"))
    for lock_file_path in lock_files:
        create_date_str = rsgislib.tools.utils.read_text_file_no_new_lines(
            lock_file_path
        )
        create_date = datetime.datetime.fromisoformat(create_date_str)
        time_since_create = (c_dateime - create_date).total_seconds()
        if time_since_create > timeout:
            if not delete_file_silent(lock_file_path):
                raise rsgislib.RSGISPyException("Could not delete the lock file..."
                                                " something has gone wrong!")


def sort_imgs_to_dirs_utm(input_imgs_dir: str, file_search_str: str, out_base_dir: str):
    """
    A function which will sort a series of input image files which
    a projected using the UTM system into individual directories per
    UTM zone. Please note that the input files are moved on your system!!

    :param input_imgs_dir: directory where the input files are to be found.
    :param file_search_str: the wildcard search string to find files within
                            the input directory (e.g., "in_dir/*.kea").
    :param out_base_dir: the output directory where the UTM folders will be created
                         and the files copied.

    """
    import rsgislib.imageutils

    in_files = glob.glob(os.path.join(input_imgs_dir, file_search_str))
    for img_file in in_files:
        utm_zone = rsgislib.imageutils.getUTMZone(img_file)
        if utm_zone is not None:
            out_dir = os.path.join(out_base_dir, "utm" + utm_zone)
            if not os.path.exists(out_dir):
                os.makedirs(out_dir)
            img_file_list = rsgislib.imageutils.getImageFiles(img_file)
            for tmp_file in img_file_list:
                print("Moving: " + tmp_file)
                outFile = os.path.join(out_dir, os.path.basename(tmp_file))
                shutil.move(tmp_file, outFile)


def create_directory_archive(in_dir: str, out_arch: str, arch_format: str) -> str:
    """
    A function which creates an archive from an input directory. This function uses
    subprocess to call the appropriate command line function.

    Please note that this function has similar functionality to shutil.make_archive
    and I would recommend you use that but I found it sometimes produces an error so
    I provided this function which uses the terminal functions as a drop in replacement.

    :param in_dir: The input directory path for which the archive with be created.
    :param out_arch: The output archive file path and name. Note this should not
                     include an extension as this will be added automatically.
    :param arch_format: The format for the archive. The options are: zip, tar,
                        gztar, bztar, xztar
    :return: a string with the full file path and name, including the file extension.

    """
    import subprocess

    if not is_path_valid(in_dir):
        raise rsgislib.RSGISPyException(f"The input directory is not valid: {in_dir}")

    c_pwd = os.getcwd()
    out_arch = os.path.abspath(out_arch)
    in_dir = os.path.abspath(in_dir)
    base_dir = os.path.dirname(in_dir)
    dir_name = os.path.split(in_dir)[1]
    os.chdir(base_dir)

    if arch_format == "zip":
        out_arch_file = "{}.zip".format(out_arch.strip())
        if not does_path_exists_or_creatable(out_arch_file):
            raise rsgislib.RSGISPyException(
                f"Output file path is not creatable: {out_arch_file}"
            )
        cmd = ["zip", "-r", out_arch_file, dir_name]
        subprocess.run(cmd, check=True)
    elif arch_format == "tar":
        out_arch_file = "{}.tar".format(out_arch.strip())
        if not does_path_exists_or_creatable(out_arch_file):
            raise rsgislib.RSGISPyException(
                f"Output file path is not creatable: {out_arch_file}"
            )
        cmd = ["tar", "-cvf", out_arch_file, dir_name]
        subprocess.run(cmd, check=True)
    elif arch_format == "gztar":
        out_arch_file = "{}.tar.gz".format(out_arch.strip())
        if not does_path_exists_or_creatable(out_arch_file):
            raise rsgislib.RSGISPyException(
                f"Output file path is not creatable: {out_arch_file}"
            )
        cmd = ["tar", "-cvzf", out_arch_file, dir_name]
        subprocess.run(cmd, check=True)
    elif arch_format == "bztar":
        out_arch_file = "{}.tar.bz2".format(out_arch.strip())
        if not does_path_exists_or_creatable(out_arch_file):
            raise rsgislib.RSGISPyException(
                f"Output file path is not creatable: {out_arch_file}"
            )
        cmd = ["tar", "-cvjSf", out_arch_file, dir_name]
        subprocess.run(cmd, check=True)
    elif arch_format == "xztar":
        out_arch_file = "{}.tar.xz".format(out_arch.strip())
        if not does_path_exists_or_creatable(out_arch_file):
            raise rsgislib.RSGISPyException(
                f"Output file path is not creatable: {out_arch_file}"
            )
        cmd = ["tar", "-cvJf", out_arch_file, dir_name]
        subprocess.run(cmd, check=True)
    else:
        raise rsgislib.RSGISPyException("Do not recognise the archive format specifed.")

    os.chdir(c_pwd)
    return out_arch_file


def create_sha1_hash(input_file: str, block_size: int = 4096) -> str:
    """
    A function which calculates finds the SHA1 hash string of the input file.

    :param input_file: the input file for which the SHA1 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes
                       (default 4096; i.e., 4kb)
    :return: SHA1 hash string of the file.

    """
    import hashlib

    sha1_hash = hashlib.sha1()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha1_hash.update(byte_block)
    return sha1_hash.hexdigest()


def create_sha224_hash(input_file: str, block_size: int = 4096) -> str:
    """
    A function which calculates finds the SHA224 hash string of the input file.

    :param input_file: the input file for which the SHA224 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes
                       (default 4096; i.e., 4kb)
    :return: SHA224 hash string of the file.

    """
    import hashlib

    sha224_hash = hashlib.sha224()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha224_hash.update(byte_block)
    return sha224_hash.hexdigest()


def create_sha256_hash(input_file: str, block_size: int = 4096) -> str:
    """
    A function which calculates finds the SHA256 hash string of the input file.

    :param input_file: the input file for which the SHA256 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes
                       (default 4096; i.e., 4kb)
    :return: SHA256 hash string of the file.

    """
    import hashlib

    sha256_hash = hashlib.sha256()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()


def create_sha384_hash(input_file: str, block_size: int = 4096) -> str:
    """
    A function which calculates finds the SHA384 hash string of the input file.

    :param input_file: the input file for which the SHA384 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes
                       (default 4096; i.e., 4kb)
    :return: SHA384 hash string of the file.

    """
    import hashlib

    sha384_hash = hashlib.sha384()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha384_hash.update(byte_block)
    return sha384_hash.hexdigest()


def create_sha512_hash(input_file: str, block_size: int = 4096) -> str:
    """
    A function which calculates finds the SHA512 hash string of the input file.

    :param input_file: the input file for which the SHA512 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes
                       (default 4096; i.e., 4kb)
    :return: SHA512 hash string of the file.

    """
    import hashlib

    sha512_hash = hashlib.sha512()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha512_hash.update(byte_block)
    return sha512_hash.hexdigest()


def create_md5_hash(input_file: str, block_size: int = 4096) -> str:
    """
    A function which calculates finds the MD5 hash string of the input file.

    :param input_file: the input file for which the MD5 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes
                       (default 4096; i.e., 4kb)
    :return: MD5 hash string of the file.

    """
    import hashlib

    md5_hash = hashlib.md5()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            md5_hash.update(byte_block)
    return md5_hash.hexdigest()


def create_blake2b_hash(input_file: str, block_size: int = 4096) -> str:
    """
    A function which calculates finds the Blake2B hash string of the input file.

    :param input_file: the input file for which the Blake2B hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes
                       (default 4096; i.e., 4kb)
    :return: Blake2B hash string of the file.

    """
    import hashlib

    blake2b_hash = hashlib.blake2b()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            blake2b_hash.update(byte_block)
    return blake2b_hash.hexdigest()


def create_blake2s_hash(input_file: str, block_size: int = 4096) -> str:
    """
    A function which calculates finds the Blake2S hash string of the input file.

    :param input_file: the input file for which the Blake2S hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes
                       (default 4096; i.e., 4kb)
    :return: Blake2S hash string of the file.

    """
    import hashlib

    blake2s_hash = hashlib.blake2s()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            blake2s_hash.update(byte_block)
    return blake2s_hash.hexdigest()


def create_sha3_224_hash(input_file: str, block_size: int = 4096) -> str:
    """
    A function which calculates finds the SHA3_224 hash string of the input file.

    :param input_file: the input file for which the SHA3_224 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes
                       (default 4096; i.e., 4kb)
    :return: SHA3_224 hash string of the file.

    """
    import hashlib

    sha224_hash = hashlib.sha3_224()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha224_hash.update(byte_block)
    return sha224_hash.hexdigest()


def create_sha3_256_hash(input_file: str, block_size: int = 4096) -> str:
    """
    A function which calculates finds the SHA3_256 hash string of the input file.

    :param input_file: the input file for which the SHA3_256 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes
                       (default 4096; i.e., 4kb)
    :return: SHA3_256 hash string of the file.

    """
    import hashlib

    sha256_hash = hashlib.sha3_256()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()


def create_sha3_384_hash(input_file: str, block_size: int = 4096) -> str:
    """
    A function which calculates finds the SHA3_384 hash string of the input file.

    :param input_file: the input file for which the SHA3_384 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes
                       (default 4096; i.e., 4kb)
    :return: SHA3_384 hash string of the file.

    """
    import hashlib

    sha384_hash = hashlib.sha3_384()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha384_hash.update(byte_block)
    return sha384_hash.hexdigest()


def create_sha3_512_hash(input_file: str, block_size: int = 4096) -> str:
    """
    A function which calculates finds the SHA3_512 hash string of the input file.

    :param input_file: the input file for which the SHA3_512 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes
                       (default 4096; i.e., 4kb)
    :return: SHA3_512 hash string of the file.

    """
    import hashlib

    sha512_hash = hashlib.sha3_512()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha512_hash.update(byte_block)
    return sha512_hash.hexdigest()


def untar_file(
    in_file: str, out_dir: str, gen_arch_dir: bool = True, verbose: bool = False
) -> str:
    """
    A function which extracts data from a tar file into the specified
    output directory. Optionally, an output directory of the same name
    as the archive file can be created for the output files.

    :param in_file: The input archive file.
    :param out_dir: The output directory which must exist (if gen_arch_dir=True then
                    a new directory will be created within the out_dir
    :param gen_arch_dir: Create a new directory with the same name as the input file
                         where the output files will be extracted to. (Default: True)
    :param verbose: If True (default: False) then more user feedback will be printed
                    to the console.
    :return: output directory where data was extracted to.

    """
    if not os.path.exists(out_dir):
        raise rsgislib.RSGISPyException("Output directory does not exist.")
    if not os.path.exists(in_file):
        raise rsgislib.RSGISPyException("Input file does not exist.")
    in_file = os.path.abspath(in_file)
    out_dir = os.path.abspath(out_dir)
    process_dir = out_dir
    if gen_arch_dir:
        basename = get_file_basename(in_file)
        process_dir = os.path.join(out_dir, basename)
    if not os.path.exists(process_dir):
        os.makedirs(process_dir)
    c_dir = os.getcwd()
    os.chdir(process_dir)
    if verbose:
        print("Extracting: {}".format(in_file))
        print("Output to: {}".format(process_dir))
        cmd = ["tar", "-xvf", in_file]
    else:
        cmd = ["tar", "-xf", in_file]
    try:
        import subprocess

        subprocess.run(cmd, check=True)
    except OSError as e:
        os.chdir(c_dir)
        raise rsgislib.RSGISPyException("Could not extract data: {}".format(cmd))
    os.chdir(c_dir)
    return process_dir


def untar_gz_file(
    in_file: str, out_dir: str, gen_arch_dir: bool = True, verbose: bool = False
) -> str:
    """
    A function which extracts data from a tar.gz file into the specified
    output directory. Optionally, an output directory of the same name
    as the archive file can be created for the output files.

    :param in_file: The input archive file.
    :param out_dir: The output directory which must exist (if gen_arch_dir=True then
                    a new directory will be created within the out_dir
    :param gen_arch_dir: Create a new directory with the same name as the input file
                         where the output files will be extracted to. (Default: True)
    :param verbose: If True (default: False) then more user feedback will be printed
                    to the console.
    :return: output directory where data was extracted to.

    """
    if not os.path.exists(out_dir):
        raise rsgislib.RSGISPyException("Output directory does not exist.")
    if not os.path.exists(in_file):
        raise rsgislib.RSGISPyException("Input file does not exist.")
    in_file = os.path.abspath(in_file)
    out_dir = os.path.abspath(out_dir)
    process_dir = out_dir
    if gen_arch_dir:
        basename = get_file_basename(in_file)
        process_dir = os.path.join(out_dir, basename)
    if not os.path.exists(process_dir):
        os.makedirs(process_dir)
    c_dir = os.getcwd()
    os.chdir(process_dir)
    if verbose:
        print("Extracting: {}".format(in_file))
        print("Output to: {}".format(process_dir))
        cmd = ["tar", "-xvzf", in_file]
        print(cmd)
    else:
        cmd = ["tar", "-xzf", in_file]
    try:
        import subprocess

        subprocess.run(cmd, check=True)
    except OSError as e:
        os.chdir(c_dir)
        raise rsgislib.RSGISPyException("Could not extract data: {}".format(cmd))
    os.chdir(c_dir)
    return process_dir


def unzip_file(
    in_file: str, out_dir: str, gen_arch_dir: bool = True, verbose: bool = False
) -> str:
    """
    A function which extracts data from a zip file into the specified
    output directory. Optionally, an output directory of the same name
    as the archive file can be created for the output files.

    :param in_file: The input archive file.
    :param out_dir: The output directory which must exist (if gen_arch_dir=True then
                    a new directory will be created within the out_dir
    :param gen_arch_dir: Create a new directory with the same name as the input file
                         where the output files will be extracted to. (Default: True)
    :param verbose: If True (default: False) then more user feedback will be printed
                    to the console.
    :return: output directory where data was extracted to.

    """
    if not os.path.exists(out_dir):
        raise rsgislib.RSGISPyException("Output directory does not exist.")
    if not os.path.exists(in_file):
        raise rsgislib.RSGISPyException("Input file does not exist.")
    in_file = os.path.abspath(in_file)
    out_dir = os.path.abspath(out_dir)
    process_dir = out_dir
    if gen_arch_dir:
        basename = get_file_basename(in_file)
        process_dir = os.path.join(out_dir, basename)
    if not os.path.exists(process_dir):
        os.makedirs(process_dir)
    c_dir = os.getcwd()
    os.chdir(process_dir)
    cmd = ["unzip", in_file]
    if verbose:
        print("Extracting: {}".format(in_file))
        print("Output to: {}".format(process_dir))
        print(cmd)
    try:
        import subprocess

        subprocess.run(cmd, check=True)
    except OSError as e:
        os.chdir(c_dir)
        raise rsgislib.RSGISPyException("Could not extract data: {}".format(cmd))
    os.chdir(c_dir)
    return process_dir


def untar_bz_file(
    in_file: str, out_dir: str, gen_arch_dir: bool = True, verbose: bool = False
) -> str:
    """
    A function which extracts data from a tar.bz file into the specified
    output directory. Optionally, an output directory of the same name
    as the archive file can be created for the output files.

    :param in_file: The input archive file.
    :param out_dir: The output directory which must exist (if gen_arch_dir=True then
                    a new directory will be created within the out_dir
    :param gen_arch_dir: Create a new directory with the same name as the input file
                         where the output files will be extracted to. (Default: True)
    :param verbose: If True (default: False) then more user feedback will be printed
                    to the console.
    :return: output directory where data was extracted to.

    """
    if not os.path.exists(out_dir):
        raise rsgislib.RSGISPyException("Output directory does not exist.")
    if not os.path.exists(in_file):
        raise rsgislib.RSGISPyException("Input file does not exist.")
    in_file = os.path.abspath(in_file)
    out_dir = os.path.abspath(out_dir)
    process_dir = out_dir
    if gen_arch_dir:
        basename = get_file_basename(in_file)
        process_dir = os.path.join(out_dir, basename)
    if not os.path.exists(process_dir):
        os.makedirs(process_dir)
    c_dir = os.getcwd()
    os.chdir(process_dir)
    if verbose:
        print("Extracting: {}".format(in_file))
        print("Output to: {}".format(process_dir))
        cmd = ["tar", "-xvjf", in_file]
        print(cmd)
    else:
        cmd = ["tar", "-xjf", in_file]
    try:
        import subprocess

        subprocess.run(cmd, check=True)
    except OSError as e:
        os.chdir(c_dir)
        raise rsgislib.RSGISPyException("Could not extract data: {}".format(cmd))
    os.chdir(c_dir)
    return process_dir


def create_targz_arch(out_arch_file: str, file_list: list, base_path: str = None):
    """
    A function which can be used to create a tar.gz file containing the
    list of input files. If you wish to remove some of the directory
    structure from the file paths in provided then a single base_path
    can be provided and will be removed from the file paths in the archive.

    :param out_arch_file: the output tar.gz file path
    :param file_list: the list of files to be added to the archive.
    :param base_path: the base path which will be removed from all the input files.
                      Note, this means all the input files must have the same base
                      path. Optional: Default is None (i.e., ignored).

    """
    import tarfile

    out_arch_file = os.path.abspath(out_arch_file)

    if base_path is not None:
        cwd = os.getcwd()
        os.chdir(base_path)

    with tarfile.open(out_arch_file, "w:gz") as tar_out_file:
        for c_file in file_list:
            if base_path is not None:
                c_file = os.path.relpath(c_file, base_path)
            tar_out_file.add(c_file)

    if base_path is not None:
        os.chdir(cwd)
