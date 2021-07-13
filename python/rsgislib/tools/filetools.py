#!/usr/bin/env python
"""
The tools.filemanipulation module contains functions for manipulating and moving files around.
"""

# Import modules
import rsgislib

import glob
import os.path
import os
import shutil


def getFileExtension(gdalformat):
    """
    A function to get the extension for a given file format
    (NOTE, currently only KEA, GTIFF, HFA, PCI and ENVI are supported).

    :return: string

    """
    ext = ".NA"
    if gdalformat.lower() == "kea":
        ext = ".kea"
    elif gdalformat.lower() == "gtiff":
        ext = ".tif"
    elif gdalformat.lower() == "hfa":
        ext = ".img"
    elif gdalformat.lower() == "envi":
        ext = ".env"
    elif gdalformat.lower() == "pcidsk":
        ext = ".pix"
    else:
        raise rsgislib.RSGISPyException("The extension for the gdalformat specified is unknown.")
    return ext


def getGDALFormatFromExt(input_file):
    """
    Get GDAL format, based on input_file

    :return: string

    """
    gdalStr = ''
    extension = os.path.splitext(input_file)[-1]
    if extension == '.env':
        gdalStr = 'ENVI'
    elif extension == '.kea':
        gdalStr = 'KEA'
    elif extension == '.tif' or extension == '.tiff':
        gdalStr = 'GTiff'
    elif extension == '.img':
        gdalStr = 'HFA'
    elif extension == '.pix':
        gdalStr = 'PCIDSK'
    else:
        raise rsgislib.RSGISPyException('Type not recognised')
    return gdalStr



def get_file_basename(input_file, check_valid=False, n_comps=0):
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
    :return: basename for file

    """
    import string
    basename = os.path.splitext(os.path.basename(input_file))[0]
    if check_valid:
        basename = basename.replace(' ', '_')
        for punct in string.punctuation:
            if (punct != '_') and (punct != '-'):
                basename = basename.replace(punct, '')
    if n_comps > 0:
        basename_split = basename.split('_')
        if len(basename_split) < n_comps:
            raise rsgislib.RSGISPyException("The number of components specified is more than the number of components in the basename.")
        out_basename = ""
        for i in range(n_comps):
            if i == 0:
                out_basename = basename_split[i]
            else:
                out_basename = out_basename + '_' + basename_split[i]
        basename = out_basename
    return basename

def get_dir_name(input_file):
    """
    A function which returns just the name of the directory of the input file without the rest of the path.

    :param input_file: string for the input file name and path
    :return: directory name
    """
    input_file = os.path.abspath(input_file)
    dir_path = os.path.dirname(input_file)
    dir_name = os.path.basename(dir_path)
    return dir_name

def deleteFileWithBasename(input_file):
    """
    Function to delete all the files which have a path
    and base name defined in the input_file attribute.

    """
    import glob
    baseName = os.path.splitext(input_file)[0]
    fileList = glob.glob(baseName + str('.*'))
    for file in fileList:
        print("Deleting file: " + str(file))
        os.remove(file)

def deleteDIR(dir_path):
    """
    A function which will delete a directory, if files and other directories
    are within the path specified they will be recursively deleted as well.
    So be careful you don't delete things within meaning it.

    """
    for root, dirs, files in os.walk(dir_path, topdown=False):
        for name in files:
            os.remove(os.path.join(root, name))
        for name in dirs:
            os.rmdir(os.path.join(root, name))
    os.rmdir(dir_path)
    print("Deleted " + dir_path)


def findFile(dir_path, file_search):
    """
    Search for a single file with a path using glob. Therefore, the file
    path returned is a true path. Within the file_search provide the file
    name with '*' as wildcard(s).

    :return: string

    """
    import glob
    files = glob.glob(os.path.join(dir_path, file_search))
    if len(files) != 1:
        raise RSGISPyException('Could not find a single file ('+file_search+'); found ' + str(len(files)) + ' files.')
    return files[0]

def findFileNone(dir_path, file_search):
    """
    Search for a single file with a path using glob. Therefore, the file
    path returned is a true path. Within the file_search provide the file
    name with '*' as wildcard(s). Returns None is not found.

    :return: string

    """
    import glob
    import os.path
    files = glob.glob(os.path.join(dir_path, file_search))
    if len(files) != 1:
        return None
    return files[0]

def find_files_ext(dir_path, ending):
    """
    Find all the files within a directory structure with a specific file ending.
    The files are return as dictionary using the file name as the dictionary key.
    This means you cannot have files with the same name within the structure.

    :param dir_path: the base directory path within which to search.
    :param ending: the file ending (e.g., .txt, or txt or .kea, kea).
    :return: dict with file name as key

    """
    out_file_dict = dict()
    for root, dirs, files in os.walk(dir_path):
        for file in files:
            if file.endswith(ending):
                file_found = os.path.join(root, file)
                if os.path.isfile(file_found):
                    out_file_dict[file] = file_found
    return out_file_dict

def find_files_mpaths_ext(dir_paths, ending):
    """
    Find all the files within a list of input directories and the structure beneath
    with a specific file ending. The files are return as dictionary using the file
    name as the dictionary key. This means you cannot have files with the same name
    within the structure.

    :param dir_path: the base directory path within which to search.
    :param ending: the file ending (e.g., .txt, or txt or .kea, kea).
    :return: dict with file name as key

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

def find_first_file(dir_path, file_search, rtn_except=True):
    """
    Search for a single file with a path using glob. Therefore, the file
    path returned is a true path. Within the file_search provide the file
    name with '*' as wildcard(s).
    :param dir_path: The directory within which to search, note that the search will be within
                    sub-directories within the base directory until a file meeting the search
                    criteria are met.
    :param file_search: The file search string in the file name and must contain a wild character (i.e., *).
    :param rtn_except: if True then an exception will be raised if no file or multiple files are found (default).
                       If False then None will be returned rather than an exception raised.
    :return: The file found (or None if rtn_except=False)

    """
    import glob
    files = None
    for root, dirs, files in os.walk(dir_path):
        files = glob.glob(os.path.join(root, file_search))
        if len(files) > 0:
            break
    out_file = None
    if (files is not None) and (len(files) == 1):
        out_file = files[0]
    elif rtn_except:
        raise Exception("Could not find a single file ({0}) in {1}; "
                        "found {2} files.".format(file_search, dir_path, len(files)))
    return out_file

def get_files_mtime(file_lst, dt_before=None, dt_after=None):
    """
    A function which subsets a list of files based on datetime of
    last modification. The function also does a check as to whether
    a file exists, files which don't exist will be ignored.

    :param file_lst: The list of file path - represented as strings.
    :param dt_before: a datetime object with a date/time where files modified before this will be returned
    :param dt_after: a datetime object with a date/time where files modified after this will be returned

    """
    import datetime
    if (dt_before is None) and (dt_after is None):
        raise Exception("You must define at least one of dt_before or dt_after")
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

def file_is_hidden(dir_path):
    """
    A function to test whether a file or folder is 'hidden' or not on the
    file system. Should be cross platform between Linux/UNIX and windows.

    :param dir_path: input file path to be tested
    :return: boolean (True = hidden)

    """
    dir_path = os.path.abspath(dir_path)
    if os.name == 'nt':
        import win32api, win32con
        attribute = win32api.GetFileAttributes(dir_path)
        return attribute & (win32con.FILE_ATTRIBUTE_HIDDEN | win32con.FILE_ATTRIBUTE_SYSTEM)
    else:
        file_name = os.path.basename(dir_path)
        return file_name.startswith('.')

def get_dir_list(dir_path, inc_hidden=False):
    """
    Function which get the list of directories within the specified path.

    :param dir_path: file path to search within
    :param inc_hidden: boolean specifying whether hidden files should be included (default=False)
    :return: list of directory paths

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



def getFileLock(input_file, sleep_period=1, wait_iters=120, use_except=False):
    """
    A function which gets a lock on a file.

    The lock file will be a unix hidden file (i.e., starts with a .) and it will have .lok added to the end.
    E.g., for input file hello_world.txt the lock file will be .hello_world.txt.lok. The contents of the lock
    file will be the time and date of creation.

    Using the default parameters (sleep 1 second and wait 120 iterations) if the lock isn't available
    it will be retried every second for 120 seconds (i.e., 2 mins).

    :param input_file: The input file for which the lock will be created.
    :param sleep_period: time in seconds to sleep for, if the lock isn't available. (Default=1 second)
    :param wait_iters: the number of iterations to wait for before giving up. (Default=120)
    :param use_except: Boolean. If True then an exception will be thrown if the lock is not
                       available. If False (default) False will be returned if the lock is
                       not successful.
    :return: boolean. True: lock was successfully gained. False: lock was not gained.

    """
    import datetime
    import time
    file_path, file_name = os.path.split(input_file)
    lock_file_name = ".{}.lok".format(file_name)
    lock_file_path = os.path.join(file_path, lock_file_name)

    got_lock = False
    for i in range(wait_iters+1):
        if not os.path.exists(lock_file_path):
            got_lock = True
            break
        time.sleep(sleep_period)

    if got_lock:
        c_datetime = datetime.datetime.now()
        f = open(lock_file_path, 'w')
        f.write('{}\n'.format(c_datetime.isoformat()))
        f.flush()
        f.close()
    elif use_except:
        raise Exception("Lock could not be gained for file: {}".format(input_file))

    return got_lock

def releaseFileLock(input_file):
    """
    A function which releases a lock file for the input file.

    :param input_file: The input file for which the lock will be created.

    """
    file_path, file_name = os.path.split(input_file)
    lock_file_name = ".{}.lok".format(file_name)
    lock_file_path = os.path.join(file_path, lock_file_name)
    if os.path.exists(lock_file_path):
        os.remove(lock_file_path)

def cleanFileLocks(dir_path, timeout=3600):
    """
    A function which cleans up any remaining lock file (i.e., if an application has crashed).
    The timeout time will be compared with the time written within the file.

    :param dir_path: the file path to search for lock files (i.e., ".*.lok")
    :param timeout: the time (in seconds) for the timeout. Default: 3600 (1 hours)

    """
    import datetime
    import glob
    import rsgislib.tools.utils
    c_dateime = datetime.datetime.now()
    lock_files = glob.glob(os.path.join(dir_path, ".*.lok"))
    for lock_file_path in lock_files:
        create_date_str = rsgislib.tools.utils.readTextFileNoNewLines(lock_file_path)
        create_date = datetime.datetime.fromisoformat(create_date_str)
        time_since_create = (c_dateime - create_date).total_seconds()
        if time_since_create > timeout:
            os.remove(lock_file_path)



def sortImgsUTM2DIRs(input_imgs_dir, file_search_str, out_base_dir):
    """
A function which will sort a series of input image files which
a projected using the UTM system into individual directories per
UTM zone. Please note that the input files are moved on your system!!

Where:

:param input_imgs_dir: directory where the input files are to be found.
:param file_search_str: the wildcard search string to find files within the input directory (e.g., \*.kea).
:param out_base_dir: the output directory where the UTM folders will be created and the files copied.

"""
    import rsgislib.imageutils
    inFiles = glob.glob(os.path.join(input_imgs_dir, file_search_str))
    for imgFile in inFiles:
        utmZone = rsgislib.imageutils.getUTMZone(imgFile)
        if utmZone is not None:
            outDIR = os.path.join(out_base_dir, 'utm'+utmZone)
            if not os.path.exists(outDIR):
                os.makedirs(outDIR)
            imgFileList = rsgislib.imageutils.getImageFiles(imgFile)
            for tmpFile in imgFileList:
                print('Moving: ' + tmpFile)
                outFile = os.path.join(outDIR, os.path.basename(tmpFile))
                shutil.move(tmpFile, outFile)


def createDirectoryArchive(in_dir, out_arch, arch_format):
    """
    A function which creates an archive from an input directory. This function uses
    subprocess to call the appropriate command line function.

    Please note that this function has similar functionality to shutil.make_archive
    and I would recommend you use that but I found it sometimes produces an error so
    I provided this function which uses the terminal functions as a drop in replacement.

    :param in_dir: The input directory path for which the archive with be created.
    :param out_arch: The output archive file path and name. Note this should not include an extension
                     as this will be added automatically.
    :param arch_format: The format for the archive. The options are: zip, tar, gztar, bztar, xztar
    :return: a string with the full file path and name, including the file extension.

    """
    import os
    import subprocess
    c_pwd = os.getcwd()
    out_arch = os.path.abspath(out_arch)
    in_dir = os.path.abspath(in_dir)
    base_dir = os.path.dirname(in_dir)
    dir_name = os.path.split(in_dir)[1]
    os.chdir(base_dir)

    if arch_format == 'zip':
        out_arch_file = "{}.zip".format(out_arch.strip())
        cmd = "zip -r {} {}".format(out_arch_file, dir_name)
        subprocess.call(cmd, shell=True)
    elif arch_format == 'tar':
        out_arch_file = "{}.tar".format(out_arch.strip())
        cmd = "tar -cvf {} {}".format(out_arch_file, dir_name)
        subprocess.call(cmd, shell=True)
    elif arch_format == 'gztar':
        out_arch_file = "{}.tar.gz".format(out_arch.strip())
        cmd = "tar -cvzf {} {}".format(out_arch_file, dir_name)
        subprocess.call(cmd, shell=True)
    elif arch_format == 'bztar':
        out_arch_file = "{}.tar.bz2".format(out_arch.strip())
        cmd = "tar -cvjSf {} {}".format(out_arch_file, dir_name)
        subprocess.call(cmd, shell=True)
    elif arch_format == 'xztar':
        out_arch_file = "{}.tar.xz".format(out_arch.strip())
        cmd = "tar -cvJf {} {}".format(out_arch_file, dir_name)
        subprocess.call(cmd, shell=True)
    else:
        raise Exception("Do not recognise the archive format specifed.")

    os.chdir(c_pwd)
    return out_arch_file


def createSHA1Hash(input_file, block_size=4096):
    """
    A function which calculates finds the SHA1 hash string of the input file.
    :param input_file: the input file for which the SHA1 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes (default 4096; i.e., 4kb)
    :return: SHA1 hash string of the file.

    """
    import hashlib
    sha1_hash = hashlib.sha1()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha1_hash.update(byte_block)
    return sha1_hash.hexdigest()


def createSHA224Hash(input_file, block_size=4096):
    """
    A function which calculates finds the SHA224 hash string of the input file.
    :param input_file: the input file for which the SHA224 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes (default 4096; i.e., 4kb)
    :return: SHA224 hash string of the file.

    """
    import hashlib
    sha224_hash = hashlib.sha224()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha224_hash.update(byte_block)
    return sha224_hash.hexdigest()


def createSHA256Hash(input_file, block_size=4096):
    """
    A function which calculates finds the SHA256 hash string of the input file.
    :param input_file: the input file for which the SHA256 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes (default 4096; i.e., 4kb)
    :return: SHA256 hash string of the file.

    """
    import hashlib
    sha256_hash = hashlib.sha256()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()


def createSHA384Hash(input_file, block_size=4096):
    """
    A function which calculates finds the SHA384 hash string of the input file.
    :param input_file: the input file for which the SHA384 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes (default 4096; i.e., 4kb)
    :return: SHA384 hash string of the file.

    """
    import hashlib
    sha384_hash = hashlib.sha384()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha384_hash.update(byte_block)
    return sha384_hash.hexdigest()


def createSHA512Hash(input_file, block_size=4096):
    """
    A function which calculates finds the SHA512 hash string of the input file.
    :param input_file: the input file for which the SHA512 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes (default 4096; i.e., 4kb)
    :return: SHA512 hash string of the file.

    """
    import hashlib
    sha512_hash = hashlib.sha512()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha512_hash.update(byte_block)
    return sha512_hash.hexdigest()


def createMD5Hash(input_file, block_size=4096):
    """
    A function which calculates finds the MD5 hash string of the input file.
    :param input_file: the input file for which the MD5 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes (default 4096; i.e., 4kb)
    :return: MD5 hash string of the file.

    """
    import hashlib
    md5_hash = hashlib.md5()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            md5_hash.update(byte_block)
    return md5_hash.hexdigest()


def createBlake2BHash(input_file, block_size=4096):
    """
    A function which calculates finds the Blake2B hash string of the input file.
    :param input_file: the input file for which the Blake2B hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes (default 4096; i.e., 4kb)
    :return: Blake2B hash string of the file.

    """
    import hashlib
    blake2b_hash = hashlib.blake2b()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            blake2b_hash.update(byte_block)
    return blake2b_hash.hexdigest()


def createBlake2SHash(input_file, block_size=4096):
    """
    A function which calculates finds the Blake2S hash string of the input file.
    :param input_file: the input file for which the Blake2S hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes (default 4096; i.e., 4kb)
    :return: Blake2S hash string of the file.

    """
    import hashlib
    blake2s_hash = hashlib.blake2s()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            blake2s_hash.update(byte_block)
    return blake2s_hash.hexdigest()


def createSHA3_224Hash(input_file, block_size=4096):
    """
    A function which calculates finds the SHA3_224 hash string of the input file.
    :param input_file: the input file for which the SHA3_224 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes (default 4096; i.e., 4kb)
    :return: SHA3_224 hash string of the file.

    """
    import hashlib
    sha224_hash = hashlib.sha3_224()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha224_hash.update(byte_block)
    return sha224_hash.hexdigest()


def createSHA3_256Hash(input_file, block_size=4096):
    """
    A function which calculates finds the SHA3_256 hash string of the input file.
    :param input_file: the input file for which the SHA3_256 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes (default 4096; i.e., 4kb)
    :return: SHA3_256 hash string of the file.

    """
    import hashlib
    sha256_hash = hashlib.sha3_256()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()


def createSHA3_384Hash(input_file, block_size=4096):
    """
    A function which calculates finds the SHA3_384 hash string of the input file.
    :param input_file: the input file for which the SHA3_384 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes (default 4096; i.e., 4kb)
    :return: SHA3_384 hash string of the file.

    """
    import hashlib
    sha384_hash = hashlib.sha3_384()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha384_hash.update(byte_block)
    return sha384_hash.hexdigest()


def createSHA3_512Hash(input_file, block_size=4096):
    """
    A function which calculates finds the SHA3_512 hash string of the input file.
    :param input_file: the input file for which the SHA3_512 hash string with be found.
    :param block_size: the size of the blocks the file is read in in bytes (default 4096; i.e., 4kb)
    :return: SHA3_512 hash string of the file.

    """
    import hashlib
    sha512_hash = hashlib.sha3_512()
    with open(input_file, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(block_size), b""):
            sha512_hash.update(byte_block)
    return sha512_hash.hexdigest()


