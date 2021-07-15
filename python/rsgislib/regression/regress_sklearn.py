from sklearn.ensemble import ExtraTreesRegressor
from sklearn.model_selection import GridSearchCV

def get_ann_obj_params(n_predictors):
    """
    Get a Artificial neural network object and parameters. 
    
    :return: set [ANN Object, ANN Parameters Dict, Boolean as to whether data needs scaling]
    
    """
    import numpy
    from sklearn.neural_network import MLPRegressor
    ann_grid = {'hidden_layer_sizes': [(10,), (25,), (50,), (100,)],
                'learning_rate': ['constant', 'adaptive'],
                'learning_rate_init': numpy.arange(0.01, 0.1, 0.005).tolist(),
                'alpha': numpy.arange(0.01, 0.3, 0.002).tolist(),
                'max_iter': numpy.arange(300, 1000, 100, dtype='uint16').tolist()}
    ann_obj = MLPRegressor(solver='lbfgs', activation='relu')
    return ann_obj, ann_grid, True


def get_en_obj_params(n_predictors):
    """
    Get a ElasticNet object and parameters. 
    
    :return: set [EN Object, EN Parameters Dict, Boolean as to whether data needs scaling]
    
    """
    import numpy
    from sklearn.linear_model import ElasticNet
    en_grid = {'alpha': numpy.arange(1.0, 5.0, 0.1).tolist(),
               'l1_ratio': numpy.arange(0.01, 1.0, 0.01).tolist()}
    en_obj = ElasticNet(max_iter=10000, tol=0.01)
    return en_obj, en_grid, False


def get_knn_obj_params(n_predictors):
    """
    Get a KNeighborsRegressor object and parameters. 
    
    :return: set [KNN Object, KNN Parameters Dict, Boolean as to whether data needs scaling]
    
    """
    import numpy
    from sklearn.neighbors import KNeighborsRegressor
    knn_grid = {'n_neighbors': numpy.arange(1, 9, 1, dtype='uint8').tolist(),
                'weights': ['uniform', 'distance']}
    knn_obj = KNeighborsRegressor(algorithm='brute')
    return knn_obj, knn_grid, True

    
def get_kr_obj_params(n_predictors):
    """
    Get a KernelRidge object and parameters. 
    
    :return: set [KR Object, KR Parameters Dict, Boolean as to whether data needs scaling]
    
    """
    import numpy
    from sklearn.kernel_ridge import KernelRidge
    kr_grid = {'alpha': numpy.arange(0.05, 2.0, 0.05).tolist(),
               'gamma': numpy.arange(0.1, 2.5, 0.1).tolist(),
               'kernel': ['rbf', 'linear', 'sigmoid']}
    kr_obj = KernelRidge()
    return kr_obj, kr_grid, False


def get_et_obj_params(n_predictors):
    """
    Get a ExtraTreesRegressor object and parameters. 
    
    :return: set [ET Object, ET Parameters Dict, Boolean as to whether data needs scaling]
    
    """
    import numpy
    from sklearn.ensemble import ExtraTreesRegressor
    et_grid = {'n_estimators': numpy.arange(100, 600, 100, dtype='uint16').tolist(),
               'max_features' : numpy.arange(1, n_predictors, 1, dtype='uint8').tolist(),
               'max_depth' : numpy.arange(1, n_predictors, 1, dtype='uint8').tolist(),
               'min_samples_split' : numpy.arange(2, 8, 1, dtype='uint8').tolist(),
               'min_samples_leaf' : numpy.arange(1, 6, 1, dtype='uint8').tolist()}
    et_obj = ExtraTreesRegressor()
    return et_obj, et_grid, False
    
def get_pls_obj_params(n_predictors):
    """
    Get a PLSRegression object and parameters. 
    
    :return: set [PLS Object, PLS Parameters Dict, Boolean as to whether data needs scaling]
    
    """
    import numpy
    from sklearn.cross_decomposition import PLSRegression
    pls_grid = {'n_components' : numpy.arange(1, n_predictors + 1, 1, dtype='uint8').tolist()}
    pls_obj = PLSRegression(scale=False, max_iter=1000, tol=0.001, copy=True)
    return pls_obj, pls_grid, False


def create_search_obj(regrs_obj, regrs_params, n_sims=250, n_cv=5, n_cores=1):
    """
    """
    from sklearn.model_selection import ParameterGrid
    from sklearn.model_selection import RandomizedSearchCV
    from sklearn.model_selection import GridSearchCV
    
    if len(ParameterGrid(regrs_params)) > n_sims:
        skl_srch_obj = GridSearchCV(regrs_obj, regrs_params, scoring='neg_mean_squared_error', cv=n_cv, n_jobs=n_cores, verbose=1)
    else:
        skl_srch_obj = RandomizedSearchCV(regrs_obj, regrs_params, scoring='neg_mean_squared_error', cv=n_cv, n_iter=n_sims, n_jobs=n_cores, verbose=1)
    
    return skl_srch_obj

def perform_search_param_opt(opt_params_file, x, y, skl_srch_obj=None, data_scaler=None):
    """
    """
    import json
    import numpy
    if data_scaler is not None:
        x_train = data_scaler.transform(x)
    else:
        x_train = numpy.copy(x)
    
    skl_srch_obj.fit(x_train, y)

    best_params = skl_srch_obj.best_params_
        
    with open(opt_params_file, 'w') as fp:
        json.dump(best_params, fp, sort_keys=True, indent=4, separators=(',', ': '), ensure_ascii=False)
    
    return skl_srch_obj.best_estimator_
    

def perform_kfold_fit(skl_regrs_obj, x, y, n_splits=5, repeats=1, shuffle=False, data_scaler=None):
    """
    """
    from sklearn.model_selection import KFold, RepeatedKFold
    import sklearn.metrics
    import numpy
    import stats_tools
    
    if repeats > 1:
        kf = RepeatedKFold(n_splits=n_splits, n_repeats=repeats)
    else:
        kf = KFold(n_splits=n_splits, shuffle=shuffle)
    
    y_true = list()
    y_pred = list()
    
    acc_metrics = []
    n_dep_var = y.shape[1]
    for i in range(n_dep_var):
        acc_metrics_dict = dict()
        acc_metrics_dict['r2'] = list()
        acc_metrics_dict['explained_variance_score'] = list()
        acc_metrics_dict['median_absolute_error'] = list()
        acc_metrics_dict['mean_absolute_error'] = list()
        acc_metrics_dict['mean_squared_error'] = list()
        acc_metrics_dict['root_mean_squared_error'] = list()
        acc_metrics_dict['norm_root_mean_squared_error'] = list()
        acc_metrics_dict['bias'] = list()
        acc_metrics_dict['norm_bias'] = list()
        acc_metrics_dict['bias_squared'] = list()
        acc_metrics_dict['variance'] = list()
        acc_metrics_dict['noise'] = list()
        acc_metrics.append(acc_metrics_dict)
    
    for train_idx, test_idx in kf.split(x):
        x_train, x_test = x[train_idx], x[test_idx]
        y_train, y_test = y[train_idx], y[test_idx]
        y_true.append(y_test)
                
        if data_scaler is not None:
            x_train = data_scaler.transform(x_train)
            x_test = data_scaler.transform(x_test)
        
        skl_regrs_obj.fit(x_train, y_train)
        
        y_hat = skl_regrs_obj.predict(x_test)
                
        if n_dep_var == 1:
            y_hat = numpy.expand_dims(y_hat, axis=1)

        for i in range(n_dep_var):
            acc_metrics[i]['r2'].append(sklearn.metrics.r2_score(y_test[...,i], y_hat[...,i]))
            acc_metrics[i]['explained_variance_score'].append(sklearn.metrics.explained_variance_score(y_test[...,i], y_hat[...,i]))
            acc_metrics[i]['median_absolute_error'].append(sklearn.metrics.median_absolute_error(y_test[...,i], y_hat[...,i]))
            acc_metrics[i]['mean_absolute_error'].append(sklearn.metrics.mean_absolute_error(y_test[...,i], y_hat[...,i]))
            acc_metrics[i]['mean_squared_error'].append(sklearn.metrics.mean_squared_error(y_test[...,i], y_hat[...,i]))
            rmse = numpy.sqrt(sklearn.metrics.mean_squared_error(y_test[...,i], y_hat[...,i]))
            acc_metrics[i]['root_mean_squared_error'].append(rmse)
            acc_metrics[i]['norm_root_mean_squared_error'].append(100*(rmse/numpy.mean(y_test[...,i])))
            bias, norm_bias = stats_tools.bias_score(y_test[...,i], y_hat[...,i])
            acc_metrics[i]['bias'].append(bias)
            acc_metrics[i]['norm_bias'].append(norm_bias)
            mse, bias_squared, variance, noise = stats_tools.decompose_bias_variance(y_test[...,i], y_hat[...,i])
            acc_metrics[i]['bias_squared'].append(bias_squared)
            acc_metrics[i]['variance'].append(variance)
            acc_metrics[i]['noise'].append(noise)

                
        y_pred.append(y_hat)
    
    # Convert the results into np.ndarrays, shape=(n_responses, n_samples)
    y_true = numpy.concatenate(y_true).T
    y_pred = numpy.concatenate(y_pred).T
    
    # Save rediduals within a list of dicts
    residuals = []
    for i in range(n_dep_var):
        c_residuals = dict()
        c_residuals['y_true'] = y_true[i]
        c_residuals['y_pred'] = y_pred[i]
        residuals.append(c_residuals)
    
    return acc_metrics, residuals


def apply_regress_sklearn_mdl(regrs_mdl, n_out_vars, predictor_img, predictor_img_bands, vld_msk_img, vld_msk_val, out_img, gdalformat='KEA', out_band_names=None, calc_stats=True, out_no_date_val=0.0):
    """
    """
    from rios import applier
    from rios import cuiprogress
    import numpy
    import rsgislib.imageutils
    
    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        progress_bar = cuiprogress.GDALProgressBar()
    
    # Convert from GDAL band index (start 1) to Numpy Array index (start 0)
    predictor_img_bands_arr = numpy.array(predictor_img_bands)
    predictor_img_bands_arr = predictor_img_bands_arr - 1
    if numpy.min(predictor_img_bands_arr) < 0:
        raise Exception("Image band numbering starts at 1; i.e., GDAL convension.")
        
    
    infiles = applier.FilenameAssociations()
    infiles.vld_msk_img = vld_msk_img
    infiles.predictor_img = predictor_img
    
    outfiles = applier.FilenameAssociations()
    outfiles.out_img = out_img
    
    otherargs = applier.OtherInputs()
    otherargs.regrs_mdl = regrs_mdl
    otherargs.vld_msk_val = vld_msk_val
    otherargs.predictor_img_bands = predictor_img_bands_arr
    otherargs.n_out_vars = n_out_vars
    otherargs.out_no_date_val = out_no_date_val
    
    
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    
    
    def _applySKRegressionModel(info, inputs, outputs, otherargs):
        """
        Internal function to apply scikit-learn regression model.
        """
        if numpy.any(inputs.vld_msk_img == otherargs.vld_msk_val):
            # Flatten the input image.
            pred_flat_data = numpy.moveaxis(inputs.predictor_img, 0, 2).reshape(-1, inputs.predictor_img.shape[0])
            # Subset to the relevant metrics.
            pred_flat_data = pred_flat_data.take(otherargs.predictor_img_bands, axis=1)
            
            ID = numpy.arange(pred_flat_data.shape[0])
            n_feats = ID.shape[0]
            
            # Mask to the valid data regions
            ID = ID[inputs.vld_msk_img.flatten() == otherargs.vld_msk_val]
            pred_flat_data = pred_flat_data[inputs.vld_msk_img.flatten() == otherargs.vld_msk_val]
            
            # Run the model
            mdl_est_vals = otherargs.regrs_mdl.predict(pred_flat_data)
            if otherargs.n_out_vars == 1:
                mdl_est_vals = numpy.expand_dims(mdl_est_vals, axis=1)

            # Create the output data array
            out_vals = numpy.zeros([n_feats, otherargs.n_out_vars])
            if otherargs.out_no_date_val != 0.0:
                out_vals[...] = otherargs.out_no_date_val
            
            # Copy the model outputs to the full array
            out_vals[ID] = mdl_est_vals
            
            # Reshape the output array.
            out_img_vals = out_vals.reshape(inputs.vld_msk_img.shape[1], inputs.vld_msk_img.shape[2], otherargs.n_out_vars)
            out_img_vals = numpy.moveaxis(out_img_vals, 2, 0)
            outputs.out_img = out_img_vals
        else:
            out_img_vals = numpy.zeros([otherargs.n_out_vars, inputs.vld_msk_img.shape[1], inputs.vld_msk_img.shape[2]], dtype=numpy.float32)
            if otherargs.out_no_date_val != 0.0:
                out_img_vals[...] = otherargs.out_no_date_val
            outputs.out_img = out_img_vals
    
    
    print("Applying the Regression model")
    applier.apply(_applySKRegressionModel, infiles, outfiles, otherargs, controls=aControls)
    print("Completed")
    
    if out_band_names is not None:
        if n_out_vars == len(out_band_names):
            rsgislib.imageutils.setBandNames(out_img, out_band_names)
        else:
            print("The number of output variables and the number of bands names provided do not match so ignoring.")
    
    if calc_stats:
        rsgislib.imageutils.popImageStats(out_img, usenodataval=True, nodataval=out_no_date_val, calcpyramids=True)
    
    
    
    