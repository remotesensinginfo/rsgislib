import h5py
import numpy
import numpy.random
from rios import applier
from rios import cuiprogress
from sklearn.model_selection import GridSearchCV
from sklearn.ensemble import RandomForestClassifier
import rsgislib.rastergis
import rsgislib

class classInfoObj(object):
    def __init__(self, id=None, fileH5=None, red=None, green=None, blue=None):
        self.id = id
        self.fileH5 = fileH5
        self.red = red
        self.green = green
        self.blue = blue

def findClassifierParametersAndTrain(classTrainInfo, paramSearchSampNum=0, gridSearch=GridSearchCV(RandomForestClassifier(), {})):
    """
    """
    if len(classTrainInfo) < 2:
        rsgislib.RSGISPyException("Need at least 2 classes to be worth running findClassifierParametersAndTrain function.")
    
    first = True
    numVars = 0
    numVals = 0
    for classInfoVal in classTrainInfo.values():
        dataShp = h5py.File(classInfoVal.fileH5)['DATA/DATA'].shape
        if first:
            numVars = dataShp[1]
            first = False
        numVals += dataShp[0]
    
    dataArr = numpy.zeros([numVals, numVars], dtype=float)
    classArr = numpy.zeros([numVals], dtype=int)
    
    if paramSearchSampNum is not 0:
        dataArrSamp = numpy.zeros([(len(classTrainInfo)*paramSearchSampNum), numVars], dtype=float)
        classArrSamp = numpy.zeros([(len(classTrainInfo)*paramSearchSampNum)], dtype=int)
    
    rowInit = 0
    rowInitSamp = 0
    for key in classTrainInfo:
        # Open the dataset
        f = h5py.File(classTrainInfo[key].fileH5)
        numRows = f['DATA/DATA'].shape[0]
        # Copy data and populate classid array
        dataArr[rowInit:(rowInit+numRows)] = f['DATA/DATA']
        classArr[rowInit:(rowInit+numRows)] = classTrainInfo[key].id
        if paramSearchSampNum is not 0:
            # Create random index to sample the whole dataset.
            sampleIdxs = numpy.random.randint(0, high=numRows, size=paramSearchSampNum)
            # Sample the input data for classifier optimisation.
            dataArrSamp[rowInitSamp:(rowInitSamp+paramSearchSampNum)] = dataArr[rowInit:(rowInit+numRows)][sampleIdxs]
            classArrSamp[rowInitSamp:(rowInitSamp+paramSearchSampNum)] = classTrainInfo[key].id
            rowInitSamp += paramSearchSampNum
        rowInit += numRows
        f.close()
    
    if paramSearchSampNum is 0:
        dataArrSamp = dataArr
        classArrSamp = classArr
    
    print("Training data size: {} x {}".format(dataArr.shape[0], dataArr.shape[1]))
    print("Training Sample data size: {} x {}".format(dataArrSamp.shape[0], dataArrSamp.shape[1]))
    
    gridSearch.fit(dataArrSamp, classArrSamp)
    if not gridSearch.refit:
        raise Exception("Grid Search did no find a fit therefore failed...")
    
    print("Best score was {} and has parameters {}.".format(gridSearch.best_score_, gridSearch.best_params_))
    
    print('Training Classifier')
    gridSearch.best_estimator_.fit(dataArr, classArr)
    print("Completed")
    
    print('Calc Classifier Accuracy')
    accVal = gridSearch.best_estimator_.score(dataArr, classArr)
    print('Classifier Train Score = {}%'.format(round(accVal*100, 2)))   
    
    return gridSearch.best_estimator_

def trainClassifier(classTrainInfo, skClassifier):
    """
    """
    if len(classTrainInfo) < 2:
        rsgislib.RSGISPyException("Need at least 2 classes to be worth running trainClassifier function.")
    
    first = True
    numVars = 0
    numVals = 0
    for classInfoVal in classTrainInfo.values():
        dataShp = h5py.File(classInfoVal.fileH5)['DATA/DATA'].shape
        if first:
            numVars = dataShp[1]
            first = False
        numVals += dataShp[0]
    
    dataArr = numpy.zeros([numVals, numVars], dtype=float)
    classArr = numpy.zeros([numVals], dtype=int)
    
    rowInit = 0
    for key in classTrainInfo:
        # Open the dataset
        f = h5py.File(classTrainInfo[key].fileH5)
        numRows = f['DATA/DATA'].shape[0]
        # Copy data and populate classid array
        dataArr[rowInit:(rowInit+numRows)] = f['DATA/DATA']
        classArr[rowInit:(rowInit+numRows)] = classTrainInfo[key].id
        rowInit += numRows
        f.close()
        
    print("Training data size: {} x {}".format(dataArr.shape[0], dataArr.shape[1]))
    
    print('Training Classifier')
    skClassifier.fit(dataArr, classArr)
    print("Completed")
    
    print('Calc Classifier Accuracy')
    accVal = skClassifier.score(dataArr, classArr)
    print('Classifier Train Score = {}%'.format(round(accVal*100, 2)))



# RIOS function to apply classifer
def _applySKClassifier(info, inputs, outputs, otherargs):
    """
    Internal function for rios applier. Used within applyClassifer.
    """
    outClassVals = numpy.zeros_like(inputs.imageMask, dtype=numpy.uint32)
    if numpy.any(inputs.imageMask == otherargs.mskVal):
        outClassVals = outClassVals.flatten()
        imgMaskVals = inputs.imageMask.flatten()
        classVars = numpy.zeros((outClassVals.shape[0], otherargs.numClassVars), dtype=numpy.float)
        # Array index which can be used to populate the output array following masking etc.
        ID = numpy.arange(imgMaskVals.shape[0])
        classVarsIdx = 0
        for imgFile in otherargs.imgFileInfo:
            imgArr = inputs.__dict__[imgFile.name]
            for band in imgFile.bands:
                classVars[...,classVarsIdx] = imgArr[(band-1)].flatten()
                classVarsIdx = classVarsIdx + 1
        classVars = classVars[imgMaskVals==otherargs.mskVal]
        ID = ID[imgMaskVals==otherargs.mskVal]
        predClass = otherargs.classifier.predict(classVars)        
        outClassVals[ID] = predClass
        outClassVals = numpy.expand_dims(outClassVals.reshape((inputs.imageMask.shape[1],inputs.imageMask.shape[2])), axis=0)
    outputs.outimage = outClassVals
    
    
def applyClassifer(classTrainInfo, skClassifier, imgMask, imgMaskVal, imgFileInfo, outputImg, gdalFormat):
    rsgisUtils = rsgislib.RSGISPyUtils()
    infiles = applier.FilenameAssociations()
    infiles.imageMask = imgMask
    numClassVars = 0
    for imgFile in imgFileInfo:
        infiles.__dict__[imgFile.name] = imgFile.fileName
        numClassVars = numClassVars + len(imgFile.bands)
            
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = outputImg
    otherargs = applier.OtherInputs()
    otherargs.classifier = skClassifier
    otherargs.mskVal = imgMaskVal
    otherargs.numClassVars = numClassVars
    otherargs.imgFileInfo = imgFileInfo
    
    aControls = applier.ApplierControls()
    aControls.progress = cuiprogress.CUIProgressBar()
    aControls.drivername = gdalFormat
    aControls.omitPyramids = True
    aControls.calcStats = False
    print("Applying the Classifier")
    applier.apply(_applySKClassifier, infiles, outfiles, otherargs, controls=aControls)
    print("Completed")
    rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)

