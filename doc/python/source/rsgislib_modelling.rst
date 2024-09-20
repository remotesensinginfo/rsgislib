RSGISLib Modelling
===============================

Species Distribution Modelling
--------------------------------

Generate Samples
~~~~~~~~~~~~~~~~~~
.. autofunction:: rsgislib.modelling.species_distribution.gen_pseudo_absences_smpls
.. autofunction:: rsgislib.modelling.species_distribution.combine_presence_absence_data
.. autofunction:: rsgislib.modelling.species_distribution.create_train_test_sets

Populate Samples
~~~~~~~~~~~~~~~~~~
.. autofunction:: rsgislib.modelling.species_distribution.extract_env_var_data
.. autofunction:: rsgislib.modelling.species_distribution.pop_normalise_coeffs
.. autofunction:: rsgislib.modelling.species_distribution.apply_normalise_coeffs
.. autoclass:: rsgislib.modelling.species_distribution.EnvVarInfo

Summary Statistics
~~~~~~~~~~~~~~~~~~~~
.. autofunction:: rsgislib.modelling.species_distribution.comparison_box_plots
.. autofunction:: rsgislib.modelling.species_distribution.correlation_matrix


Model Fitting
~~~~~~~~~~~~~~~~~~~~
.. autofunction:: rsgislib.modelling.species_distribution.search_mdl_params
.. autofunction:: rsgislib.modelling.species_distribution.fit_sklearn_mdl
.. autofunction:: rsgislib.modelling.species_distribution.fit_sklearn_slg_cls_mdl
.. autofunction:: rsgislib.modelling.species_distribution.fit_kfold_sklearn_mdls
.. autofunction:: rsgislib.modelling.species_distribution.fit_kfold_sklearn_sgl_cls_mdls

Model Explanation
~~~~~~~~~~~~~~~~~~~~
.. autofunction:: rsgislib.modelling.species_distribution.shap_sklearn_mdl_explainer
.. autofunction:: rsgislib.modelling.species_distribution.salib_sklearn_mdl_sensitity
.. autofunction:: rsgislib.modelling.species_distribution.sklearn_mdl_variable_response_curves
.. autofunction:: rsgislib.modelling.species_distribution.sklearn_jacknife_var_importance

Apply Models
~~~~~~~~~~~~~~~
.. autofunction:: rsgislib.modelling.species_distribution.pred_sklearn_mdl_prob
.. autofunction:: rsgislib.modelling.species_distribution.pred_sklearn_mdl_cls
.. autofunction:: rsgislib.modelling.species_distribution.pred_ensemble_sklearn_mdls_prob
.. autofunction:: rsgislib.modelling.species_distribution.pred_ensemble_sklearn_mdls_cls
.. autofunction:: rsgislib.modelling.species_distribution.pred_ensemble_sklearn_slg_cls_mdls_prob
.. autofunction:: rsgislib.modelling.species_distribution.pred_ensemble_sklearn_sgl_cls_mdls_cls


Other Tools
~~~~~~~~~~~~~~~
.. autofunction:: rsgislib.modelling.species_distribution.create_finite_mask
