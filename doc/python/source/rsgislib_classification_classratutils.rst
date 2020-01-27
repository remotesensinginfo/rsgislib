RSGISLib Scikit-Learn Clumps Classification Module
====================================================

.. automodule:: rsgislib.classification.classratutils

The steps to undertaking a classification using clumps are:

  * Image segmentation to generate clumps
  * Populate attributes to clumps
  * Generate training and populate to clumps
  * Train the classifier
  * Apply the classifier
  * Collapse to generate a classification.

If you have undertaken an image segmentation and want to use those segments for a classification using RSGISLib then you need to use the image clumps representation. This is described in the paper below:

Clewley, D., Bunting, P., Shepherd, J., Gillingham, S., Flood, N., Dymond, J., Lucas, R., Armston, J., Moghaddam, M. (2014). A Python-Based Open Source System for Geographic Object-Based Image Analysis (GEOBIA) Utilizing Raster Attribute Tables Remote Sensing  6(7), 6111 6135. https://dx.doi.org/10.3390/rs6076111

Commonly we would use the Shepherd et al., (2019) segmentation using the following function::

    from rsgislib.segmentation import segutils

    input_img = "S2_UVD_27sept_27700_sub.kea"
    clumps_img = "s2_uvd_27sept_clumps.kea"
    tmp_path = "./tmp"
    segutils.runShepherdSegmentation(input_img, clumps_img, tmpath=tmp_path, numClusters=60, minPxls=100, distThres=100, sampling=100, kmMaxIter=200)

Shepherd, J., Bunting, P., Dymond, J. (2019). Operational Large-Scale Segmentation of Imagery Based on Iterative Elimination Remote Sensing  11(6), 658. https://dx.doi.org/10.3390/rs11060658

To populate the clumps (i.e., segments or objects) with the attribute information used for the classification you need to use the functions within the rsgislib.rastergis module, for example::

    import rsgislib.rastergis

    # Populate with all statistics (min, max, mean, standard deviation)
    bandinfo = []
    bandinfo.append(rsgislib.rastergis.BandAttStats(band=1, minField='BlueMin', maxField='BlueMax', meanField='BlueMean', stdDevField='BlueStdev'))
    bandinfo.append(rsgislib.rastergis.BandAttStats(band=2, minField='GrnMin', maxField='GrnMax', meanField='GrnMean', stdDevField='GrnStdev'))
    bandinfo.append(rsgislib.rastergis.BandAttStats(band=3, minField='RedMin', maxField='RedMax', meanField='RedMean', stdDevField='RedStdev'))
    bandinfo.append(rsgislib.rastergis.BandAttStats(band=4, minField='RE1Min', maxField='RE1Max', meanField='RE1Mean', stdDevField='RE1Stdev'))
    rsgislib.rastergis.populateRATWithStats(input_img, clumps_img, bandinfo)

    # Populate with just mean statistic
    bandinfo = []
    bandinfo.append(rsgislib.rastergis.BandAttStats(band=1, meanField='BlueMean'))
    bandinfo.append(rsgislib.rastergis.BandAttStats(band=2, meanField='GrnMean'))
    bandinfo.append(rsgislib.rastergis.BandAttStats(band=3, meanField='RedMean'))
    bandinfo.append(rsgislib.rastergis.BandAttStats(band=4, meanField='RE1Mean'))
    rsgislib.rastergis.populateRATWithStats(input_img, clumps_img, bandinfo)


To train the classifier you need to create a column within the clump raster attribute table (RAT) specifying the class for the clumps being used for training. Training is often provided as vector layers, using a ratutils helper function you can generate the training data::

    import rsgislib.rastergis.ratutils

    classes_dict = dict()
    classes_dict['Mangroves'] = [1, 'Mangroves.shp']
    classes_dict['Other'] = [2, 'Other.shp']
    tmp_path = './tmp'
    classes_int_col_in = 'ClassInt'
    classes_name_col = 'ClassStr'
    rsgislib.rastergis.ratutils.populateClumpsWithClassTraining(clumps_img, classes_dict, tmp_path, classes_int_col_in, classes_name_col)

To balance the training samples (ensuring there are the same number for each class) you can use the following function::

   import rsgislib.classification.classratutils

   classes_int_col = 'ClassIntSamp'
   rsgislib.classification.classratutils.balanceSampleTrainingRandom(clumps_img, classes_int_col_in, classes_int_col, 100, 200)

To train the classifier you need to use the findClassifierParameters function::

    from sklearn.model_selection import GridSearchCV
    from sklearn.ensemble import RandomForestClassifier

    # RAT variables used for the classification
    variables = ['BlueMean', 'GrnMean', 'RedMean', 'RE1Mean']

    grid_search = GridSearchCV(RandomForestClassifier(), param_grid={'n_estimators':[10,20,50,100], 'max_depth':[2,4,8]})

    classifier = rsgislib.classification.classratutils.findClassifierParameters(clumps_img, classes_int_col, variables, preProcessor=None, gridSearch=grid_search)

To apply the classification you can use either the classifyWithinRAT or classifyWithinRATTiled functions. classifyWithinRAT loads the attribute table columns you are using for the classification to memory with a single read of the attribute table, this can therefore be faster to compute for smaller scenes. However, if you have a large number of clumps within your RAT then this can use more memory then you have available and you'll need to use the classifyWithinRATTiled function, which steps through the RAT in chunks using only a small amount of memory. If you are unsure use the classifyWithinRATTiled function as the extra I/O time will be minimal.

Classification use the classifyWithinRATTiled function::

    class_colours = dict()
    class_colours['Mangroves'] = [0,255,0]
    class_colours['Other'] = [100,100,100]

    out_class_int_col = 'OutClass'
    out_class_str_col = 'OutClassName'
    rsgislib.classification.classratutils.classifyWithinRATTiled(clumps_img, classes_int_col, classes_name_col, variables, classifier=classifier, outColInt=out_class_int_col, outColStr=out_class_str_col, classColours=class_colours, preProcessor=None)

Classification use the classifyWithinRAT function::

    class_colours = dict()
    class_colours['Mangroves'] = [0,255,0]
    class_colours['Other'] = [100,100,100]

    out_class_int_col = 'OutClass'
    out_class_str_col = 'OutClassName'
    rsgislib.classification.classratutils.classifyWithinRAT(clumps_img, classes_int_col, classes_name_col, variables, classifier=classifier, outColInt=out_class_int_col, outColStr=out_class_str_col, classColours=class_colours, preProcessor=None)

Finally, to produce a classification image file, rather than segmentation, where the image pixel value corresponds with the classified class, you can use the following function which 'collapses' the RAT to create a classification image::

    import rsgislib.classification

    # Export to a 'classification image' rather than a RAT...
    out_class_img = 's2_uvd_27sept_class.kea'
    rsgislib.classification.collapseClasses(clumps_img, out_class_img, 'KEA', out_class_str_col, out_class_int_col)

Training
---------

.. autofunction:: rsgislib.classification.classratutils.findClassifierParameters
.. autofunction:: rsgislib.classification.classratutils.balanceSampleTrainingRandom


Classify
--------

.. autofunction:: rsgislib.classification.classratutils.classifyWithinRAT
.. autofunction:: rsgislib.classification.classratutils.classifyWithinRATTiled
.. autofunction:: rsgislib.classification.classratutils.clusterWithinRAT





