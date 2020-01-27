RSGISLib Species Distribution Modelling Module
===============================================

.. automodule:: rsgislib.sdm

Running a Species Distribution Modelling using RSGISLib
--------------------------------------------------------

To run a species distribution model you first need to define the input variables and parameters::

    import rsgislib.sdm

    # Define simulation parameters within a Python dictionary:
    ContinuousLayers = ['./Data/Environmental_Predictors/Annual_Insolation.tif', './Data/Environmental_Predictors/Elevation.tif', './Data/Environmental_Predictors/Forest_Distance.tif',
                        './Data/Environmental_Predictors/Summer_Temp.tif', './Data/Environmental_Predictors/Topographic_Wetness.tif', './Data/Environmental_Predictors/Wind_Velocity.tif',
                        './Data/Environmental_Predictors/Winter_Temp.tif']

    CategoricalLayers = ['./Data/Environmental_Predictors/Corine_CLC_2018.tif', './Data/Environmental_Predictors/Forest_Type.tif']

    simulation = {}
    simulation['Name'] = 'Test_Run_Extra_Trees'
    simulation['Output Directory'] = './' + simulation['Name']
    simulation['Images'] = ContinuousLayers + CategoricalLayers
    simulation['Categorical'] = [False for i in ContinuousLayers] + [True for i in CategoricalLayers]  # list of booleans where True == categorical variable.
    simulation['Null Value'] = 0  # No data value for the Continuous and Categorical raster layers.
    simulation['Valid Mask'] = './Data/Wales_Binary_Mask.tif'
    simulation['Environmental Predictors'] = [image.split('/')[-1].replace('.tif', '') for image in simulation['Images']]  # define names of the environmental predictors.

    # define the study region using the binary mask image:
    geotransform, projection, xsize, ysize, noDataVal = rsgislib.sdm.read_raster_information(simulation['Valid Mask'])
    simulation['geotransform'] = geotransform
    simulation['projection'] = projection
    simulation['rasterxsize'] = xsize
    simulation['rasterysize'] = ysize
    del geotransform, projection, xsize, ysize, noDataVal


The sighting records then need processing and the pseudo-absences creating::

    # read xy coordinates for the presence records:
    presence_xy = rsgislib.sdm.read_sightings_vector('./Data/Squirrel_Sightings/Red_Squirrel.geojson')  # import from OGR vector file.
    #presence_xy = rsgislib.sdm.read_sightings_csv('./Data/Squirrel_Sightings/Red_Squirrel.csv', sep=',', xfield='x (OSGB)', yfield='y (OSGB)')  # import from CSV file.

    # extract values from input images for presence records:
    presence_data = rsgislib.sdm.extract_raster_values(simulation, presence_xy)

    # generate pseudo-absence x&y coordinates:
    absence_xy = rsgislib.sdm.generate_pseudoabsences(simulation, n_points=15000)

    # (optional) drop pseudo-absence points that are proximal to presence records (e.g. within 500 metres):
    absence_xy = rsgislib.sdm.drop_proximal_records(presence_xy, absence_xy, distance=500)

    # (optional) ensure that number of presence records == number of absence records:
    # Note: this is an important consideration particularly for decision-tree estimators.
    absence_xy = rsgislib.sdm.equalise_records(presence_xy, absence_xy)

    # (optional) export pseudo-absence xy coordinates to a CSV:
    #rsgislib.sdm.export_pseudoabsence_xy(absence_xy, 'Pseudoabsence_Coords_Extra_Trees.csv')

    # extract values from input images for absence records:
    absence_data = rsgislib.sdm.extract_raster_values(simulation, absence_xy)
    del presence_xy, absence_xy

    # drop records with no valid data:
    presence_data = rsgislib.sdm.drop_null_records(simulation, presence_data)
    absence_data = rsgislib.sdm.drop_null_records(simulation, absence_data)

    # combine the presence and absence data into a single array:
    presence_absence_data = rsgislib.sdm.combine_records(presence_data, absence_data)
    del presence_data, absence_data


Finally, you can run the simulation but you also need to create an instance of the estimator you want use:

Extra-Tree / Random Forests::

    # define the estimator:
    from sklearn.ensemble import ExtraTreesClassifier as ET
    estimator = ET(n_estimators=100, max_depth=6, min_samples_split=4, min_samples_leaf=2, min_weight_fraction_leaf=0.0, max_features=None, max_leaf_nodes=None, min_impurity_split=1e-07, bootstrap=False, n_jobs=4)

    # run the simulation:
    rsgislib.sdm.apply_decision_tree_estimator(simulation, presence_absence_data, estimator, validation='split-sample', test_fraction=0.1, replicates=10, gdalformat='GTiff', clf_labels='Presence-Absence', Overwrite=True)
    del presence_absence_data, simulation

Logistic Regression (MaxEnt)::

    # define the estimator:
    from sklearn.linear_model import LogisticRegression as maxent
    estimator = maxent(solver='liblinear', max_iter=500, penalty='l2', tol=0.0001, C=0.1, fit_intercept=True, intercept_scaling=1, n_jobs=4)

    # run the simulation:
    rsgislib.sdm.apply_meta_estimator(simulation, presence_absence_data, estimator, validation='split-sample', test_fraction=0.1, replicates=10, gdalformat='GTiff', clf_labels='Presence-Absence', Overwrite=True)
    del presence_absence_data, simulation

Gradiant Boosting::

    # define the estimator:
    from sklearn.ensemble import GradientBoostingClassifier as GBT
    estimator = GBT(loss='deviance', learning_rate=0.01, n_estimators=100, subsample=1.0, min_samples_split=4, min_samples_leaf=2, min_weight_fraction_leaf=0.0, max_depth=3, min_impurity_split=1e-07, max_features=None)

    # run the simulation:
    rsgislib.sdm.apply_decision_tree_estimator(simulation, presence_absence_data, estimator, validation='split-sample', test_fraction=0.1, replicates=10, gdalformat='GTiff', clf_labels='Presence-Absence', Overwrite=True)
    del presence_absence_data, simulation


Loading Data
--------------
.. autofunction:: rsgislib.sdm.read_sightings_vector
.. autofunction:: rsgislib.sdm.read_sightings_csv
.. autofunction:: rsgislib.sdm.extract_raster_values

Manipulate Records
-------------------
.. autofunction:: rsgislib.sdm.generate_pseudoabsences
.. autofunction:: rsgislib.sdm.export_pseudoabsence_xy
.. autofunction:: rsgislib.sdm.equalise_records
.. autofunction:: rsgislib.sdm.drop_proximal_records
.. autofunction:: rsgislib.sdm.drop_null_records
.. autofunction:: rsgislib.sdm.combine_records

Run SDM
--------
.. autofunction:: rsgislib.sdm.apply_decision_tree_estimator
.. autofunction:: rsgislib.sdm.apply_meta_estimator

Utilities
----------
.. autofunction:: rsgislib.sdm.read_raster_information


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

