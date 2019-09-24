RSGISLib Timeseries Analysis Module
======================================

.. automodule:: rsgislib.timeseries
   :members:
   :undoc-members:

Model Fitting
--------------

Thw following functions allow for a stack of timeseries raster images to be converted into a single output image containing per-band season-trend model coefficients, RMSE, and an overall value per-band. The outputs and model fitting are based on the following paper:

Zhu, Z.; Woodcock, C.E.; Holden, C.; Yang, Z. Generating synthetic Landsat images based on all available Landsat data: Predicting Landsat surface reflectance at any given time. Remote Sensing of Environment 2015, 162, 67–83. doi:10.1016/j.rse.2015.02.009.

Models are fitted over the entire provided time series, i.e. the script does not look for breaks/changes.

The input is a JSON file with a list of date:filepath pairs as strings, e.g::

    {
        "YYYY-MM-DD": "/path/to/image/file/1.tif", 
        "YYYY-MM-DD": "/path/to/image/file/2.tif", 
        "YYYY-MM-DD": "/path/to/image/file/3.tif"
    }

To fit the model use the following function::
    
    rsgislib.timeseries.modelfitting.get_ST_model_coeffs('example.json', 'coeffs.kea', bands=[3,4,5,6,7], num_processes=4)

The output image can then be used directly (e.g., for classification) or use to predict an output image of particular date::

    rsgislib.timeseries.modelfitting.predict_for_date('2019-01-15', 'coeffs.kea', 'predicted.kea')


.. autofunction:: rsgislib.timeseries.modelfitting.get_ST_model_coeffs
.. autofunction:: rsgislib.timeseries.modelfitting.predict_for_date




* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

