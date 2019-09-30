#! /usr/bin/env python
############################################################################
#  classratutils.py
#
#  Copyright 2015 RSGISLib.
#
#  RSGISLib: 'The remote sensing and GIS Software Library'
#
#  RSGISLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  RSGISLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
#
#
# Purpose:  Provide a set of utilities which combine commands to create
#           useful extra functionality and make it more easily available
#           to be reused.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 28/11/2015
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

from __future__ import print_function
haveGDALPy = True
try:
    import osgeo.gdal as gdal
except ImportError as gdalErr:
    haveGDALPy = False
    
haveNumpy = True
try:
    import numpy
except ImportError as numErr:
    haveNumpy = False

haveRIOSRat = True
try:
    from rios import rat
    from rios import ratapplier
except ImportError as riosRatErr:
    haveRIOSRat = False

haveSKLearnRF = True
try:
    from sklearn.ensemble import RandomForestClassifier
except ImportError as sklearnRFErr:
    haveSKLearnRF = False
    raise Exception("The scikit-learn random forests tools are required for this module could not be imported\n\t {}".format(sklearnRFErr))
    
haveSKLearnGS = True
try:
    from sklearn.model_selection import GridSearchCV
except ImportError as sklearnGSErr:
    haveSKLearnRF = False
    raise Exception("The scikit-learn grid search tools are required for this module could not be imported\n\t" + sklearnGSErr)

haveSKLearnPreProcess = True
try:
    from sklearn.preprocessing import MaxAbsScaler
except ImportError as sklearnPreProcessErr:
    haveSKLearnPreProcess = False
    raise Exception("The scikit-learn pre-processing modules not available.\n\t" + sklearnPreProcessErr)

haveSKLearnKM = True
try:
    from sklearn.cluster import MiniBatchKMeans
except ImportError as sklearnMBKMErr:
    haveSKLearnKM = False
    raise Exception("The scikit-learn Mini Batch KMeans tools are required for this module could not be imported\n\t" + sklearnMBKMErr)
    

def classifyWithinRAT(clumpsImg, classesIntCol, classesNameCol, variables, classifier=RandomForestClassifier(n_estimators=100, max_features=3, oob_score=True, n_jobs=-1), outColInt="OutClass", outColStr="OutClassName", roiCol=None, roiVal=1, classColours=None, preProcessor=None, justFit=False):
    """
A function which will perform a classification within the RAT using a classifier from scikit-learn

:param clumpsImg: is the clumps image on which the classification is to be performed
:param classesIntCol: is the column with the training data as int values
:param classesNameCol: is the column with the training data as string class names
:param variables: is an array of column names which are to be used for the classification
:param classifier: is an instance of a scikit-learn classifier (e.g., RandomForests which is Default)
:param outColInt: is the output column name for the int class representation (Default: 'OutClass')
:param outColStr: is the output column name for the class names column (Default: 'OutClassName')
:param roiCol: is a column name for a column which specifies the region to be classified. If None ignored (Default: None)
:param roiVal: is a int value used within the roiCol to select a region to be classified (Default: 1)
:param classColours: is a python dict using the class name as the key along with arrays of length 3 specifying the RGB colours for the class.
:param preProcessor: is a scikit-learn processors such as sklearn.preprocessing.MaxAbsScaler() which can rescale the input variables independently as read in (Define: None; i.e., not in use).
:param justFit: is a boolean specifying that the classifier should just be fitted to the data and not applied (Default: False; i.e., apply classification)


Example::

    from sklearn.ensemble import ExtraTreesClassifier
    from rsgislib.classification import classratutils
    
    classifier = ExtraTreesClassifier(n_estimators=100, max_features=3, n_jobs=-1, verbose=0)
    
    classColours = dict()
    classColours['Forest'] = [0,138,0]
    classColours['NonForest'] = [200,200,200]
    
    variables = ['GreenAvg', 'RedAvg', 'NIR1Avg', 'NIR2Avg', 'NDVI']
    classifyWithinRAT(clumpsImg, classesIntCol, classesNameCol, variables, classifier=classifier, classColours=classColours)
    
    from sklearn.preprocessing import MaxAbsScaler
    
    # With pre-processor
    classifyWithinRAT(clumpsImg, classesIntCol, classesNameCol, variables, classifier=classifier, classColours=classColours, preProcessor=MaxAbsScaler())

"""
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
    # Check numpy is available
    if not haveNumpy:
        raise Exception("The numpy module is required for this function could not be imported\n\t" + numErr)
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception("The RIOS rat tools are required for this function could not be imported\n\t" + riosRatErr)
    # Check scikit-learn RF is available
    if not haveSKLearnRF:
        raise Exception("The scikit-learn random forests tools are required for this function could not be imported\n\t" + sklearnRFErr)
    # Check scikit-learn pre-processing is available
    if not haveSKLearnPreProcess:
        raise Exception("The scikit-learn pre-processing tools are required for this function could not be imported\n\t" + sklearnPreProcessErr)
        
    ratDataset = gdal.Open(clumpsImg, gdal.GA_Update)
    numpyVars = []
    for var in variables:
        print("Reading " + var)
        tmpArr = rat.readColumn(ratDataset, var)
        if not preProcessor is None:
            tmpArr = tmpArr.reshape(-1, 1)
            tmpArr = preProcessor.fit_transform(tmpArr)
            tmpArr = tmpArr.reshape(-1)
        numpyVars.append(tmpArr)
    
    # Read in training classes
    classesInt = rat.readColumn(ratDataset, classesIntCol)
    classesStr = rat.readColumn(ratDataset, classesNameCol)
        
    roi = None
    if not ((roiCol == None) or (roiCol == "")):
        roi = rat.readColumn(ratDataset, roiCol)
    
    # Set up output array
    outLabels = numpy.zeros_like(classesInt, dtype=numpy.int16)
    outClassNames = numpy.empty_like(classesInt, dtype=numpy.dtype('a255'))
    ID = numpy.arange(outLabels.shape[0])
    
    xData = numpy.array(numpyVars)
    xData = xData.transpose()
    xData = numpy.where(numpy.isfinite(xData), xData,0)

    print("Input data size: {} x {}".format(xData.shape[0], xData.shape[1]))
    
    trainingData = xData[numpy.isfinite(xData).all(axis=1)]
    classesInt = classesInt[numpy.isfinite(xData).all(axis=1)]
    classesStr = classesStr[numpy.isfinite(xData).all(axis=1)]
    ID = ID[numpy.isfinite(xData).all(axis=1)]
    
    trainingData = trainingData[classesInt > 0]
    classesStr = classesStr[classesInt > 0]
    classesInt = classesInt[classesInt > 0]
        
    print("Training data size: {} x {}".format(trainingData.shape[0], trainingData.shape[1]))
        
    print('Training Classifier')
    classifier.fit(trainingData, classesInt)
    
    print('Calc Classifier Accuracy')
    accVal = classifier.score(trainingData, classesInt)
    print('Classifier Score = {}'.format(round(accVal*100, 2)))
    
    if not justFit:
        if not roi is None:
            xData = xData[roi == roiVal]
            ID = ID[roi == roiVal]
            print("ROI Subsetted data size: {} x {}".format(xData.shape[0], xData.shape[1]))
        
        predClass = classifier.predict(xData)
        
        outLabels[ID] = predClass
        
        print("Writing Columns")
        rat.writeColumn(ratDataset, outColInt, outLabels)
        
        print("Create and Write Output Class Names")
        classNames = numpy.unique(classesStr)
        classes = numpy.zeros_like(classNames, dtype=numpy.int16)
        
      
        i = 0
        classNameIDs = dict()
        for className in classNames:
            classNameStr = str(className.decode())
            if not classNameStr is '':
                classes[i] = classesInt[classesStr == className][0]
                classNameIDs[classNameStr] = classes[i]
                print("Class \'" + classNameStr + "\' has numerical " + str(classes[i]))  
                i = i + 1
        
        outClassNames[...] = ''
        for className in classNameIDs:
            classID = classNameIDs[className]
            outClassNames[outLabels==classID] = className
        
        rat.writeColumn(ratDataset, outColStr, outClassNames)
        
        if not classColours is None:
            print("Set Colours")
            red = rat.readColumn(ratDataset, "Red")
            green = rat.readColumn(ratDataset, "Green")
            blue = rat.readColumn(ratDataset, "Blue")
            
            # Set Background to black
            red[...] = 0
            green[...] = 0
            blue[...] = 0
            
            # Set colours
            for className in classNameIDs:
                print("Colouring class " + className)
                classID = classNameIDs[className]
                colours = classColours[className]
                
                red   = numpy.where(outLabels == classID, colours[0], red)
                green = numpy.where(outLabels == classID, colours[1], green)
                blue  = numpy.where(outLabels == classID, colours[2], blue)
        
            rat.writeColumn(ratDataset, "Red", red)
            rat.writeColumn(ratDataset, "Green", green)
            rat.writeColumn(ratDataset, "Blue", blue)
    
    ratDataset = None



def _extractTrainDataFromRAT(info, inputs, outputs, otherargs):
    """
    This function is used internally within classifyWithinRATTiled using the RIOS ratapplier function
    """
    progress = round((info.startrow / info.rowCount)*100)
    print("{} %".format(progress), end="\r")
    
    numpyVars = []
    for var in otherargs.vars:
        varVals = getattr(inputs.inrat, var)
        numpyVars.append(varVals)
    
    xData = numpy.array(numpyVars)
    xData = xData.transpose()
    
    classIntVals = getattr(inputs.inrat, otherargs.classIntCol)
    trainingData = xData[classIntVals > 0]
    
    otherargs.trainData[otherargs.trainDataOff:otherargs.trainDataOff+trainingData.shape[0]] = trainingData
    otherargs.trainDataOff += trainingData.shape[0]

def _applyClassifier(info, inputs, outputs, otherargs):
    """
    This function is used internally within classifyWithinRATTiled using the RIOS ratapplier function
    """
    progress = round((info.startrow / info.rowCount)*100)
    print("{} %".format(progress), end="\r")    
    numpyVars = []
    for var in otherargs.vars:
        varVals = getattr(inputs.inrat, var)
        numpyVars.append(varVals)
    
    xData = numpy.array(numpyVars)
    xData = xData.transpose()
    
    ID = numpy.arange(xData.shape[0])
    outClassIntVals = numpy.zeros(xData.shape[0], dtype=numpy.int16)
    outClassNamesVals = numpy.empty(xData.shape[0], dtype=numpy.dtype('a255'))
    outClassNamesVals[...] = ''
    
    ID = ID[numpy.isfinite(xData).all(axis=1)]
    vData = xData[numpy.isfinite(xData).all(axis=1)]
        
    if otherargs.roiCol is not None:
        roi = getattr(inputs.inrat, otherargs.roiCol)
        roi = roi[numpy.isfinite(xData).all(axis=1)]
        vData = vData[roi == otherargs.roiVal]
        ID = ID[roi == otherargs.roiVal]
        #print("ROI Subsetted data size: {} x {}".format(vData.shape[0], vData.shape[1]))
        
    predClass = otherargs.classifier.predict(vData)
    
    outClassIntVals[ID] = predClass
    setattr(outputs.outrat, otherargs.outColInt, outClassIntVals)
    
    for className in otherargs.classNameIDs:
        classID = otherargs.classNameIDs[className]
        outClassNamesVals[outClassIntVals==classID] = className
    setattr(outputs.outrat, otherargs.outColStr, outClassNamesVals)
    
    if not otherargs.classColours is None:
        red = getattr(inputs.inrat, "Red")
        green = getattr(inputs.inrat, "Green")
        blue = getattr(inputs.inrat, "Blue")
        
        # Set Background to black
        red[...] = 0
        green[...] = 0
        blue[...] = 0
        
        # Set colours
        for className in otherargs.classNameIDs:
            #print("Colouring class " + className)
            classID = otherargs.classNameIDs[className]
            colours = otherargs.classColours[className]
            
            red   = numpy.where(outClassIntVals == classID, colours[0], red)
            green = numpy.where(outClassIntVals == classID, colours[1], green)
            blue  = numpy.where(outClassIntVals == classID, colours[2], blue)
    
        setattr(outputs.outrat, "Red", red)
        setattr(outputs.outrat, "Green", green)
        setattr(outputs.outrat, "Blue", blue)
    
    

def classifyWithinRATTiled(clumpsImg, classesIntCol, classesNameCol, variables, classifier=RandomForestClassifier(n_estimators=100, max_features=3, oob_score=True, n_jobs=-1), outColInt="OutClass", outColStr="OutClassName", roiCol=None, roiVal=1, classColours=None, scaleVarsRange=False, justFit=False):
    """
A function which will perform a classification within the RAT using a classifier from scikit-learn using the rios ratapplier interface allowing very large RATs to be processed. 

:param clumpsImg: is the clumps image on which the classification is to be performed
:param classesIntCol: is the column with the training data as int values
:param classesNameCol: is the column with the training data as string class names
:param variables: is an array of column names which are to be used for the classification
:param classifier: is an instance of a scikit-learn classifier (e.g., RandomForests which is Default)
:param outColInt: is the output column name for the int class representation (Default: 'OutClass')
:param outColStr: is the output column name for the class names column (Default: 'OutClassName')
:param roiCol: is a column name for a column which specifies the region to be classified. If None ignored (Default: None)
:param roiVal: is a int value used within the roiCol to select a region to be classified (Default: 1)
:param classColours: is a python dict using the class name as the key along with arrays of length 3 specifying the RGB colours for the class.
:param scaleVarsRange: will rescale each variable independently to a range of 0-1 (default: False).
:param justFit: is a boolean specifying that the classifier should just be fitted to the data and not applied (Default: False; i.e., apply classification)


Example::

    from sklearn.ensemble import ExtraTreesClassifier
    from rsgislib.classification import classratutils
    
    classifier = ExtraTreesClassifier(n_estimators=100, max_features=3, n_jobs=-1, verbose=0)
    
    classColours = dict()
    classColours['Forest'] = [0,138,0]
    classColours['NonForest'] = [200,200,200]
    
    variables = ['GreenAvg', 'RedAvg', 'NIR1Avg', 'NIR2Avg', 'NDVI']
    classifyWithinRATTiled(clumpsImg, classesIntCol, classesNameCol, variables, classifier=classifier, classColours=classColours)
        
    # With using range scaling.
    classifyWithinRATTiled(clumpsImg, classesIntCol, classesNameCol, variables, classifier=classifier, classColours=classColours, scaleVarsRange=True)

"""
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
    # Check numpy is available
    if not haveNumpy:
        raise Exception("The numpy module is required for this function could not be imported\n\t" + numErr)
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception("The RIOS rat tools are required for this function could not be imported\n\t" + riosRatErr)
    # Check scikit-learn RF is available
    if not haveSKLearnRF:
        raise Exception("The scikit-learn random forests tools are required for this function could not be imported\n\t" + sklearnRFErr)
    # Check scikit-learn pre-processing is available
    if not haveSKLearnPreProcess:
        raise Exception("The scikit-learn pre-processing tools are required for this function could not be imported\n\t" + sklearnPreProcessErr)
        
    ratDataset = gdal.Open(clumpsImg, gdal.GA_Update)
    
    # Read in training classes
    classesInt = rat.readColumn(ratDataset, classesIntCol)
    classesStr = rat.readColumn(ratDataset, classesNameCol)
    ratDataset = None
    
    validClassStr = classesStr[classesInt > 0]
    validClassInt = classesInt[classesInt > 0]
    
    #print(validClassInt.shape)
    classNames = numpy.unique(validClassStr)
    classes = numpy.zeros_like(classNames, dtype=numpy.int16)
    
    i = 0
    classNameIDs = dict()
    for className in classNames:
        classNameStr = str(className.decode())
        if not classNameStr is '':
            #print(validClassInt[validClassStr == className])
            classes[i] = validClassInt[validClassStr == className][0]
            classNameIDs[classNameStr] = classes[i]
            #print("Class \'" + classNameStr + "\' has numerical " + str(classes[i]))  
            i = i + 1    
    
    trainLen = validClassInt.shape[0]
    numVars = len(variables)
    
    #print("Create numpy {} x {} array for training".format(trainLen, numVars))
    
    trainData = numpy.zeros((trainLen, numVars), numpy.float64)    
    
    in_rats = ratapplier.RatAssociations()
    out_rats = ratapplier.RatAssociations()
    in_rats.inrat = ratapplier.RatHandle(clumpsImg)

    otherargs = ratapplier.OtherArguments()
    otherargs.vars = variables
    otherargs.classIntCol = classesIntCol
    otherargs.trainData = trainData
    otherargs.trainDataOff = 0
    
    print("Extract Training Data")
    ratapplier.apply(_extractTrainDataFromRAT, in_rats, out_rats, otherargs=otherargs, controls=None)
    print("100%")
    
    
    validClassInt = validClassInt[numpy.isfinite(trainData).all(axis=1)]
    validClassStr = validClassStr[numpy.isfinite(trainData).all(axis=1)]
    trainData = trainData[numpy.isfinite(trainData).all(axis=1)]
    
    print("Training data size: {} x {}".format(trainData.shape[0], trainData.shape[1]))
    
    print('Training Classifier')
    classifier.fit(trainData, validClassInt)
    print("Completed")
    
    print('Calc Classifier Accuracy')
    accVal = classifier.score(trainData, validClassInt)
    print('Classifier Score = {}'.format(round(accVal*100, 2)))
    
    if not justFit:
        print("Apply Classifier")
        in_rats = ratapplier.RatAssociations()
        out_rats = ratapplier.RatAssociations()
        in_rats.inrat = ratapplier.RatHandle(clumpsImg)
        out_rats.outrat = ratapplier.RatHandle(clumpsImg)
        
        otherargs = ratapplier.OtherArguments()
        otherargs.vars = variables
        otherargs.classifier = classifier
        otherargs.outColInt = outColInt
        otherargs.outColStr = outColStr
        otherargs.roiCol = roiCol
        otherargs.roiVal = roiVal
        otherargs.classColours = classColours
        otherargs.classNameIDs = classNameIDs
        
        
        ratapplier.apply(_applyClassifier, in_rats, out_rats, otherargs=otherargs, controls=None)
        print("100%")





def clusterWithinRAT(clumpsImg, variables, clusterer=MiniBatchKMeans(n_clusters=8, init='k-means++', max_iter=100, batch_size=100), outColInt="OutCluster", roiCol=None, roiVal=1, clrClusters=True, clrSeed=10, addConnectivity=False, preProcessor=None):
    """
A function which will perform a clustering within the RAT using a clustering algorithm from scikit-learn

:param clumpsImg: is the clumps image on which the classification is to be performed.
:param variables: is an array of column names which are to be used for the clustering.
:param clusterer: is an instance of a scikit-learn clusterer (e.g., MiniBatchKMeans which is Default; Note with 8 clusters).
:param outColInt: is the output column name identifying the clusters (Default: 'OutCluster').
:param roiCol: is a column name for a column which specifies the region to be clustered. If None ignored (Default: None).
:param roiVal: is a int value used within the roiCol to select a region to be clustered (Default: 1).
:param clrClusters: is a boolean specifying whether the colour table should be updated to correspond to the clusters (Default: True).
:param clrSeed: is an integer seeding the random generator used to generate the colours (Default=10; if None provided system time used).
:param addConnectivity: is a boolean which adds a kneighbors_graph to the clusterer (just an option for the AgglomerativeClustering algorithm)
:param preProcessor: is a scikit-learn processors such as sklearn.preprocessing.MaxAbsScaler() which can rescale the input variables independently as read in (Define: None; i.e., not in use).


Example::

    from rsgislib.classification import classratutils
    from sklearn.cluster import DBSCAN
    
    sklearnClusterer = DBSCAN(eps=1, min_samples=50)
    classratutils.clusterWithinRAT('MangroveClumps.kea', ['MinX', 'MinY'], clusterer=sklearnClusterer, outColInt="OutCluster", roiCol=None, roiVal=1, clrClusters=True, clrSeed=10, addConnectivity=False)
    
    # With pre-processor
    from sklearn.preprocessing import MaxAbsScaler
    classratutils.clusterWithinRAT('MangroveClumps.kea', ['MinX', 'MinY'], clusterer=sklearnClusterer, outColInt="OutCluster", roiCol=None, roiVal=1, clrClusters=True, clrSeed=10, addConnectivity=False, preProcessor=MaxAbsScaler())

"""
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
    # Check numpy is available
    if not haveNumpy:
        raise Exception("The numpy module is required for this function could not be imported\n\t" + numErr)
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception("The RIOS rat tools are required for this function could not be imported\n\t" + riosRatErr)
    # Check scikit-learn RF is available
    if not haveSKLearnKM:
        raise Exception("The scikit-learn Mini Batch KMeans tools are required for this function could not be imported\n\t" + sklearnMBKMErr)
    # Check scikit-learn pre-processing is available
    if not haveSKLearnPreProcess:
        raise Exception("The scikit-learn pre-processing tools are required for this function could not be imported\n\t" + sklearnPreProcessErr)
        
    ratDataset = gdal.Open(clumpsImg, gdal.GA_Update)
    Histogram = rat.readColumn(ratDataset, 'Histogram')
    numpyVars = []
    for var in variables:
        print("Reading " + var)
        tmpArr = rat.readColumn(ratDataset, var)
        if not preProcessor is None:
            tmpArr = tmpArr.reshape(-1, 1)
            tmpArr = preProcessor.fit_transform(tmpArr)
            tmpArr = tmpArr.reshape(-1)
        numpyVars.append(tmpArr)
        
    roi = None
    if not ((roiCol == None) or (roiCol == "")):
        roi = rat.readColumn(ratDataset, roiCol)
    
    # Set up output array
    outLabels = numpy.zeros_like(Histogram, dtype=numpy.int16)
    ID = numpy.arange(outLabels.shape[0])
    
    xData = numpy.array(numpyVars)
    xData = xData.transpose()
    ID = ID[numpy.isfinite(xData).all(axis=1)]
    if not roi is None:
        roi = roi[numpy.isfinite(xData).all(axis=1)]
    xData = xData[numpy.isfinite(xData).all(axis=1)]
    
    if not roi is None:
        xData = xData[roi == roiVal]
        ID = ID[roi == roiVal]    
    
    print("Input Data Size: {} x {}".format(xData.shape[0], xData.shape[1]))
    
    if addConnectivity:
        from sklearn.neighbors import kneighbors_graph
        inConnectivity = kneighbors_graph(xData, n_neighbors=10, include_self=False)
        clusterer.set_params(**{'connectivity': inConnectivity})
        
    print('Fit Clusterer')
    outClust = clusterer.fit_predict(xData)    
    
    minClusterID = numpy.min(outClust)
    if minClusterID <= 0:
        minOff = 1 - minClusterID
        outClust = outClust + minOff
    
    outLabels[ID] = outClust
    
    print("Writing Columns")
    rat.writeColumn(ratDataset, outColInt, outLabels)
    
    print("Create and Write Output Class Names")
    clustersIDs = numpy.unique(outClust)
      
    if clrClusters:
        import random
        random.seed(clrSeed)
    
        print("Set Colours")
        red = rat.readColumn(ratDataset, "Red")
        green = rat.readColumn(ratDataset, "Green")
        blue = rat.readColumn(ratDataset, "Blue")
        
        # Set Background to black
        red[...] = 0
        green[...] = 0
        blue[...] = 0
        
        # Set colours
        for clusterID in clustersIDs:
            print("Colouring cluster: " + str(clusterID))
            
            red   = numpy.where(outLabels == clusterID, random.randint(0,255), red)
            green = numpy.where(outLabels == clusterID, random.randint(0,255), green)
            blue  = numpy.where(outLabels == clusterID, random.randint(0,255), blue)
    
        rat.writeColumn(ratDataset, "Red", red)
        rat.writeColumn(ratDataset, "Green", green)
        rat.writeColumn(ratDataset, "Blue", blue)

    ratDataset = None


def findClassifierParameters(clumpsImg, classesIntCol, variables, preProcessor=None, gridSearch=GridSearchCV(RandomForestClassifier(), {})):
    """
Find the optimal parameters for a classifier using a grid search and return a classifier instance with those optimal parameters.

:param clumpsImg: is the clumps image on which the classification is to be performed
:param classesIntCol: is the column with the training data as int values
:param variables: is an array of column names which are to be used for the classification
:param preProcessor: is a scikit-learn processors such as sklearn.preprocessing.MaxAbsScaler() which can rescale the input variables independently as read in (Define: None; i.e., not in use).
:param gridSearch: is an instance of GridSearchCV parameterised with a classifier and parameters to be searched.

:return: Instance of the classifier with optimal parameters defined.

Example::

    from rsgislib.classification import classratutils
    from sklearn.svm import SVC
    from sklearn.model_selection import GridSearchCV
    from sklearn.preprocessing import MaxAbsScaler
    
    clumpsImg = "./LS8_20150621_lat10lon652_r67p233_clumps.kea"
    classesIntCol = 'ClassInt'
    
    classParameters = {'kernel':['linear', 'rbf',  'poly', 'sigmoid'], 'C':[1, 2, 3, 4, 5, 10, 100, 400, 500, 1e3, 5e3, 1e4, 5e4, 1e5], 'gamma':[0.0001, 0.0005, 0.001, 0.005, 0.01, 0.1, 'auto'], 'degree':[2, 3, 4, 5, 6, 7, 8], 'class_weight':['', 'balanced'], 'decision_function_shape':['ovo', 'ovr', None]}
    variables = ['BlueRefl', 'GreenRefl', 'RedRefl', 'NIRRefl', 'SWIR1Refl', 'SWIR2Refl']
    
    gSearch = GridSearchCV(SVC(), classParameters)
    classifier = classratutils.findClassifierParameters(clumpsImg, classesIntCol, variables, preProcessor=MaxAbsScaler(), gridSearch=gSearch)

"""
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
    # Check numpy is available
    if not haveNumpy:
        raise Exception("The numpy module is required for this function could not be imported\n\t" + numErr)
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception("The RIOS rat tools are required for this function could not be imported\n\t" + riosRatErr)
    # Check scikit-learn pre-processing is available
    if not haveSKLearnPreProcess:
        raise Exception("The scikit-learn pre-processing tools are required for this function could not be imported\n\t" + sklearnPreProcessErr)
    # Check scikit-learn Grid Search is available
    if not haveSKLearnGS:
        raise Exception("The scikit-learn grid search tools are required for this function could not be imported\n\t" + sklearnGSErr)
        
    ratDataset = gdal.Open(clumpsImg, gdal.GA_Update)
    numpyVars = []
    for var in variables:
        print("Reading " + var)
        tmpArr = rat.readColumn(ratDataset, var)
        if not preProcessor is None:
            tmpArr = tmpArr.reshape(-1, 1)
            tmpArr = preProcessor.fit_transform(tmpArr)
            tmpArr = tmpArr.reshape(-1)
        numpyVars.append(tmpArr)
    
    # Read in training classes
    classesInt = rat.readColumn(ratDataset, classesIntCol)
        
    xData = numpy.array(numpyVars)
    xData = xData.transpose()
    xData = numpy.where(numpy.isfinite(xData), xData,0)

    print("Input data size: {} x {}".format(xData.shape[0], xData.shape[1]))
    
    trainingData = xData[numpy.isfinite(xData).all(axis=1)]
    classesInt = classesInt[numpy.isfinite(xData).all(axis=1)]
    
    trainingData = trainingData[classesInt > 0]
    classesInt = classesInt[classesInt > 0]
        
    print("Training data size: {} x {}".format(trainingData.shape[0], trainingData.shape[1]))
    print("Training data IDs size: {}".format(classesInt.shape[0]))
    
    classIDs = numpy.unique(classesInt)
    print(classIDs)
    for id in classIDs:
        print("Class {} has {} samples.".format(id, classesInt[classesInt==id].shape[0]))
    
    
    gridSearch.fit(trainingData, classesInt)
    if not gridSearch.refit:
        raise Exception("Grid Search did no find a fit therefore failed...")
    
    print("Best score was {} and has parameters {}.".format(gridSearch.best_score_, gridSearch.best_params_))
   
    return gridSearch.best_estimator_


def balanceSampleTrainingRandom(clumpsImg, trainCol, outTrainCol, minNoSamples, maxNoSamples):
    """
A function to balance the number of training samples for classification so the number is above
a minimum threshold (minNoSamples) and all equal to the class with the smallest number of samples
unless that is above a set maximum (maxNoSamples).

:param clumpsImg: is a string with the file path to the input image with RAT
:param trainCol: is a string for the name of the input column specifying the training samples (zero is no data)
:param outTrainCol: is a string with the name of the outputted training samples.
:param minNoSamples: is an int specifying the minimum number of training samples for a class (if below threshold class is removed).
:param maxNoSamples: is an int specifiying the maximum number of training samples per class.

"""
     # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
    # Check numpy is available
    if not haveNumpy:
        raise Exception("The numpy module is required for this function could not be imported\n\t" + numErr)
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception("The RIOS rat tools are required for this function could not be imported\n\t" + riosRatErr)
    
    ratDataset = gdal.Open(clumpsImg, gdal.GA_Update)
    trainColVals = rat.readColumn(ratDataset, trainCol)
    trainColOutVals = numpy.zeros_like(trainColVals)
    
    classIDs = numpy.unique(trainColVals)
    classIDs = classIDs[classIDs != 0]
    
    numSampPerClass = []    
    print("Number of input samples:")
    for id in classIDs:
        numVals = trainColVals[trainColVals==id].shape[0]
        print("\tClass {} has {} samples.".format(id, numVals))
        numSampPerClass.append(numVals)
    
    minNumSamples = 0
    first = True
    for i in range(len(numSampPerClass)):
        if numSampPerClass[i] < minNoSamples:
            trainColOutVals[trainColVals == classIDs[i]] = 0
        else:
            if first:
                minNumSamples = numSampPerClass[i]
                first = False
            elif numSampPerClass[i] < minNumSamples:
                minNumSamples = numSampPerClass[i]
        
    if minNumSamples > maxNoSamples:
        minNumSamples = maxNoSamples
    
    print("Number of output samples:") 
    for i in range(len(numSampPerClass)):
        if numSampPerClass[i] >= minNoSamples:
            indexes = numpy.where(trainColVals == classIDs[i])
            sampleIdx = numpy.random.choice(indexes[0], minNumSamples, replace=False)
            trainColOutVals[sampleIdx] = classIDs[i]
        print("\tClass {} has {} samples.".format(classIDs[i], trainColOutVals[trainColOutVals==classIDs[i]].shape[0]))
    
    rat.writeColumn(ratDataset, outTrainCol, trainColOutVals)
    
    ratDataset = None

