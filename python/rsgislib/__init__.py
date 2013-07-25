"""
This namespace contains rsgislib Python bindings
"""
import os.path
import os
import fnmatch

TYPE_UNDEFINED = 0
TYPE_8INT = 1
TYPE_16INT = 2
TYPE_32INT = 3
TYPE_64INT = 4
TYPE_8UINT = 5
TYPE_16UINT = 6
TYPE_32UINT = 7
TYPE_64UINT = 8
TYPE_32FLOAT = 9
TYPE_64FLOAT = 10

class RSGISPyException(Exception):

    def __init__(self, value):
        """
        Init for the RSGISPyException class
        """
        self.value = value
        
    def __str__(self):
        """
        Return a string representation of the exception
        """
        return repr(self.value)

class RSGISPyUtils (object):
    """
    A class with useful utilties within RSGISLib.
    """
    
    def getFileExtension(self, format):
        ext = ".NA"
        if format.lower() == "kea":
            ext = ".kea"
        elif format.lower() == "gtiff":
            ext = ".tif"
        elif format.lower() == "hfa":
            ext = ".img"
        elif format.lower() == "envi":
            ext = ".env"
        else:
            raise RSGISPyException("The extension for the format specified is unknown.")
        return ext
    
    def deleteFileWithBasename(self, filePath):
        fileDIR = os.path.split(filePath)[0]
        fileName = os.path.split(filePath)[1]
        
        if os.path.isdir(fileDIR):
            basename = os.path.splitext(fileName)[0]
            for file in os.listdir(fileDIR):
                if fnmatch.fnmatch(file, basename+str('.*')):
                    print("Deleting file: " + str(os.path.join(fileDIR, file)))
                    os.remove(os.path.join(fileDIR, file))
                
    def deleteDIR(self, dirPath):
        for root, dirs, files in os.walk(dirPath, topdown=False):
            for name in files:
                os.remove(os.path.join(root, name))
            for name in dirs:
                os.rmdir(os.path.join(root, name))
        os.rmdir(dirPath)
        print("Deleted " + dirPath)

