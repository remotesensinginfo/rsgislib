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
.. autofunction:: rsgislib.modelling.species_distribution.fit_sgl_mdl

Model Explanation
~~~~~~~~~~~~~~~~~~~~
.. autofunction:: rsgislib.modelling.species_distribution.shap_mdl_explainer

Apply Models
~~~~~~~~~~~~~~~
.. autofunction:: rsgislib.modelling.species_distribution.pred_slg_mdl_prob
.. autofunction:: rsgislib.modelling.species_distribution.pred_slg_mdl_cls
