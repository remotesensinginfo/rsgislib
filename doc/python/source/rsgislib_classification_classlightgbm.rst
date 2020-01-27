RSGISLib LightGBM Pixel Classification Module
==============================================

.. automodule:: rsgislib.classification.classlightgbm

LightGBM (https://lightgbm.readthedocs.io) is an alternative library to scikit-learn which with specialist support for Gradient Boosted Decision Tree (GBDT), but it also supports random forests, Dropouts meet Multiple Additive Regression Trees (DART), and Gradient Based One-Side Sampling (Goss).

When considering ensemble learning, there are two primary methods: bagging and boosting. Bagging involves the training of many independent models and combines their predictions through some form of aggregation (averaging, voting etc.). An example of a bagging ensemble is a Random Forest.

Boosting instead trains models sequentially, where each model learns from the errors of the previous model. Starting with a weak base model, models are trained iteratively, each adding to the prediction of the previous model to produce a strong overall prediction.

In the case of gradient boosted decision trees, successive models are found by applying gradient descent in the direction of the average gradient, calculated with respect to the error residuals of the loss function, of the leaf nodes of previous models.


Training
---------

.. autofunction:: rsgislib.classification.classlightgbm.train_lightgbm_binary_classifer
.. autofunction:: rsgislib.classification.classlightgbm.train_lightgbm_multiclass_classifer

Classify
--------

.. autofunction:: rsgislib.classification.classlightgbm.apply_lightgbm_binary_classifier
.. autofunction:: rsgislib.classification.classlightgbm.apply_lightgbm_multiclass_classifier




