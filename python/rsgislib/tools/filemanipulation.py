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
    
    * inputImgsDIR - directory where the input files are to be found.
    * fileSearchStr - the wildcard search string to find files within the input directory (e.g., *.kea).
    * outBaseDIR - the output directory where the UTM folders will be created and the files copied.
    """
    rsgisUtils = rsgislib.RSGISPyUtils()
    inFiles = glob.glob(os.path.join(inputImgsDIR, fileSearchStr))
    for imgFile in inFiles:
        utmZone = rsgisUtils.getUTMZone(imgFile)
        if utmZone is not None:
            outDIR = os.path.join(outBaseDIR, utmZone)
            if not os.path.exists(outDIR):
                os.makedirs(outDIR)
            imgFileList = rsgisUtils.getImageFiles(imgFile)
            for tmpFile in imgFileList:
                print('Moving: ' + tmpFile)
                outFile = os.path.join(outDIR, os.path.basename(tmpFile))
                shutil.move(tmpFile, outFile)
