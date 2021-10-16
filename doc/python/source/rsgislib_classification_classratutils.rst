RSGISLib Scikit-Learn Clumps Classification
==============================================

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


Extract Data for Training
--------------------------

.. autofunction:: rsgislib.classification.classratutils.extract_rat_col_data




