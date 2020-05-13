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

def sortImgsUTM2DIRs(inputImgsDIR, fileSearchStr, outBaseDIR):
    """
A function which will sort a series of input image files which
a projected using the UTM system into individual directories per
UTM zone. Please note that the input files are moved on your system!!

Where:

:param inputImgsDIR: directory where the input files are to be found.
:param fileSearchStr: the wildcard search string to find files within the input directory (e.g., \*.kea).
:param outBaseDIR: the output directory where the UTM folders will be created and the files copied.

"""
    rsgisUtils = rsgislib.RSGISPyUtils()
    inFiles = glob.glob(os.path.join(inputImgsDIR, fileSearchStr))
    for imgFile in inFiles:
        utmZone = rsgisUtils.getUTMZone(imgFile)
        if utmZone is not None:
            outDIR = os.path.join(outBaseDIR, 'utm'+utmZone)
            if not os.path.exists(outDIR):
                os.makedirs(outDIR)
            imgFileList = rsgisUtils.getImageFiles(imgFile)
            for tmpFile in imgFileList:
                print('Moving: ' + tmpFile)
                outFile = os.path.join(outDIR, os.path.basename(tmpFile))
                shutil.move(tmpFile, outFile)


def create_dir_archive(in_dir, out_arch, arch_format):
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


