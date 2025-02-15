#!/usr/bin/env python
"""
The tools.stats module contains some useful tools for calculating useful statistics which aren't
easily available else where.
"""
from typing import Union, List

import numpy

import rsgislib


def calc_pandas_vif(df, cols=None):
    """
    A function to calculate variance inflation factors to
    investigate multicollinearity between predictor variables.

    Interpretation of VIF scores (somewhat subjective):
    1 = No multicollinearity.
    1-5 = Moderate multicollinearity.
    > 5 = High multicollinearity.
    > 10 = This predictor should be removed from the model.

    :param df: pandas dataframe where the columns are the predictor variables
    :param cols: list of columns in the dataframe
    :return: A pandas series containing the VIF for each predictor variable.

    .. code:: python

        df = pandas.read_csv('metrics.csv')
        cols = list(df.columns)
        # Subset to the column names of interest
        ind_vars = cols[6:]
        vifs_series = calc_pandas_vif(df, ind_vars)
        vifs_series.to_csv('VIF_scores.csv')

    """
    # Import the linear model module.
    import pandas
    import sklearn.linear_model

    # If the cols is None then get a list of all the columns
    if cols is None:
        cols = list(df.columns)

    # If there is less than 2 columns product error message.
    if len(cols) < 2:
        raise rsgislib.RSGISPyException(
            "The list of columns must be have a length of at least 2."
        )

    print("Calculating VIF for {} predictors variables...".format(len(cols)))

    # Create a linear model instance.
    lm = sklearn.linear_model.LinearRegression()

    # Create the dict for the output scores
    vif_scores = dict()
    # Iterative through the columns.
    for col in cols:
        # Get the list of columns without the current column
        tmp_cols = cols.copy()
        tmp_cols.remove(col)
        # Create numpy array for the y value to be fitted.
        y = df[col].values
        # Create numpy array for the x values
        x = df[tmp_cols].values
        lm.fit(x, y)
        # Use fitted model to predict y values
        y_pred = lm.predict(x)

        # Calculate the coefficient of determination:
        ss_tot = sum((y - numpy.mean(y)) ** 2)
        ss_res = sum((y - y_pred) ** 2)
        r2 = 1 - (ss_res / ss_tot)

        # Calculate VIF score:
        if r2 == 1:
            vif_scores[col] = 0.0
        else:
            vif_scores[col] = 1 / (1 - r2)

    # Create a pandas series and return it.
    return pandas.Series(vif_scores, name="VIF")


def cqv_threshold(df, cols=None, lowthreshold=0.25, highthreshold=0.75):
    """
    A function to remove features with low & high variance based on the coefficient
    of quartile variation (CQV).

    Low CQV == uninformative predictor.
    High CQV == unstable predictor variable.

    Inspired by sklearn.feature_selection.VarianceThreshold(), but more robust
    since the coefficient of quartile variation is independent of feature scaling.
    It is also less susceptible to outliers than the coefficient of variation.

    References:
    https://en.wikipedia.org/wiki/Quartile_coefficient_of_dispersion
    https://cran.r-project.org/web/packages/cvcqv/vignettes/cqv_versatile.html

    :param df: pandas dataframe where the columns are the predictor variables
    :param cols: list of columns in the dataframe
    :param lowthreshold: Float defining the CQV below which the predictors are uninfomative
    :param highthreshold: Float defining the CQV above which the predictors are unreliable
    :return: list of column names for good predictor variables

    """
    # If the cols is None then get a list of all the columns
    if cols is None:
        cols = list(df.columns)

    # If there is less than 2 columns product error message.
    if len(cols) < 2:
        raise rsgislib.RSGISPyException(
            "The list of columns must be have a length of at least 2."
        )

    # Create numpy array from the list columns
    x = df[cols].values

    # Calculate the Coefficient of Variation:
    print("Calculating CQV for {} predictor variables...".format(len(cols)))
    q1 = numpy.percentile(x, 25, axis=0)
    q3 = numpy.percentile(x, 75, axis=0)
    cqv = (q3 - q1) / (q3 + q1)

    print("Median CQV: {}".format(numpy.median(cqv)))

    # Index the good predictors:
    good_idx = numpy.where((cqv >= lowthreshold) & (cqv <= highthreshold))[0]

    # Get a list of column names
    good_cols = []
    for i in good_idx:
        good_cols.append(cols[i])

    print("Selected {} useful predictors...".format(len(good_cols)))

    return good_cols


def corr_feature_selection(df, dep_vars, ind_vars, n_min_clusters=3, n_max_clusters=25):
    """
    A function which performs a correlation based feature selection.
    This analysis will cluster the independent (predictor) variables based
    on the Pearson correlation distance metric. The Silhouette coefficient
    (Rousseeuw, 1987) is used to find the optimal number of clusters.

    :param df: pandas dataframe where the columns are the predictor variables
    :param dep_vars: list of dependent variables within the dataframe
    :param ind_vars: list of independent (predictor) variables within the dataframe
    :param n_min_clusters: The minimum number of clusters within the search (Default: 3)
    :param n_max_clusters: The maximum number of clusters within the search (Default: 25)
    :return: list of column names for good predictor variables

    """
    import scipy.stats
    import sklearn.cluster
    import sklearn.metrics
    import tqdm

    # Create a list for sorting the correlations for each dependent variable
    corr_data = list()
    # Loop through the dependent variables
    for dep_var in dep_vars:
        # Create a list for sorting the output correlation values
        corr_vals = list()
        # Loop through the independent variables
        for ind_var in ind_vars:
            # add the correlation values to the list
            corr_vals.append(df[dep_var].corr(df[ind_var], method="pearson"))
        # Add the list of correlation values to the overall list.
        corr_data.append(corr_vals)

    # Calculate the mean Pearson r value for each predictor variable,
    # using the Fisher z-transform to standardise the Pearson r values.
    # https://en.wikipedia.org/wiki/Fisher_transformation
    pearson_corr = numpy.tanh(numpy.mean(numpy.arctanh(corr_data), axis=0))

    def _cluster_features(X, n_clusters):
        """
        An internal function to cluster predictor variables using feature agglomeration.

        :params X: a 2D numpy array of shape = (nSamples, nPredictors).
        :params n_clusters: the number of clusters to output
        :returns: the labels
        """
        # check inputs:
        if X.ndim != 2:
            raise rsgislib.RSGISPyException("Error: the input array must be 2D.")

        def _pearson_affinity(a):
            """An utility function to generate an affinity matrix based on Pearson correlation."""
            return 1 - numpy.array(
                [[abs(scipy.stats.pearsonr(x, y)[0]) for x in a] for y in a]
            )

        clf = sklearn.cluster.FeatureAgglomeration(
            n_clusters=n_clusters,
            metric=_pearson_affinity,
            linkage="complete",
        )
        clf.fit(X)

        # Get the cluster id for each feature/predictor variable:
        labels = clf.labels_ + 1

        return labels

    # Create the nd array from the pandas data range with all the predictor
    # variables listed.
    x = df[ind_vars].values

    # Test for optimal number of clusters using the Silhouette Coefficient as a
    # performance metric.

    n_clusters = numpy.arange(n_min_clusters, n_max_clusters, dtype="uint8")
    cluster_score = list()
    for n in tqdm.tqdm(n_clusters):
        x_lbls = _cluster_features(x, n)
        cluster_score.append(
            sklearn.metrics.silhouette_score(x.T, x_lbls, metric="correlation")
        )

    # find the maximum Silhouette Coefficient = best clustering:
    best_idx = numpy.argmax(cluster_score)
    opt_n_clusters = n_clusters[best_idx]
    opt_n_clusters_scr = cluster_score[best_idx]
    print("Found optimal number of clusters: {}".format(opt_n_clusters))
    print("Silhouette Coefficient: {}".format(opt_n_clusters_scr))

    # Perform clustering with optimal number of clusters
    cluster_lbls = _cluster_features(x, opt_n_clusters)

    # Select one predictor variable from each cluster using the maximum Pearson correlation:
    best_predictors = []
    for lbl in numpy.unique(cluster_lbls):
        lbl_idx = numpy.where(cluster_lbls == lbl)[0]
        best_corr = numpy.argmax(pearson_corr[lbl_idx])
        best_predictors.append(ind_vars[lbl_idx[best_corr]])

    return best_predictors


def lassolars_feature_selection(df, dep_vars, ind_vars, alpha_val=None):
    """
    A function which undertake regularisation-based feature selection using
    the LassoLars regressor in Scikit-Learn. the Lasso (least absolute shrinkage
    and selection operator) regression algorithm is linear model that uses L1
    regularisation to assign coefficients of zero to uninformative predictor
    variables (effectively eliminating them from the regression model). The
    LARS algorithm (Efron et al., 2004) provides a means of estimating which
    variables to include in the model, as well as their coefficients.

    :param df: pandas dataframe where the columns are the predictor variables
    :param dep_vars: list of dependent variables within the dataframe
    :param ind_vars: list of independent (predictor) variables within the dataframe
    :param alpha_val: Value of the regularization parameter (alpha) for the Lasso estimator.
                      If None then the value will be estimated using Bayes Information criterion
                      (BIC) and cross-validation. (Default: None).
    :return: list of column names for good predictor variables

    """
    import warnings

    import sklearn.exceptions
    import sklearn.feature_selection
    import sklearn.linear_model

    # Create the nd array from the pandas data range with all the predictor
    # variables listed.
    x = df[ind_vars].values

    # Create the nd array from the pandas data range with all the dependent
    # variables listed.
    y = df[dep_vars].values

    if alpha_val is None:
        # Use the Bayes Information criterion (BIC) and cross-validation to find the
        # optimal value of the regularization parameter (alpha) for the Lasso estimator.
        estimator = sklearn.linear_model.LassoLarsIC("bic")

        with warnings.catch_warnings():
            warnings.filterwarnings(
                "ignore", category=sklearn.exceptions.ConvergenceWarning
            )
            estimator.fit(x, y[:, -1])

        optimal_alpha = estimator.alpha_
    else:
        optimal_alpha = float(alpha_val)

    print(
        "Using regularization parameter (alpha) for the Lasso estimator of: {}".format(
            optimal_alpha
        )
    )

    # Fit the regressor using tuned regularisation parameter:
    estimator = sklearn.linear_model.LassoLars(alpha=optimal_alpha)

    with warnings.catch_warnings():
        warnings.filterwarnings(
            "ignore", category=sklearn.exceptions.ConvergenceWarning
        )
        estimator.fit(x, y)

    # Select the variables with non-zero coefficients:
    selector = sklearn.feature_selection.SelectFromModel(estimator, prefit=True)
    ind_vars_arr = numpy.array(ind_vars)
    best_predictors = ind_vars_arr[selector.get_support()]

    return list(best_predictors)


def breusch_pagan_test(x, y):
    """
    A function to perform a Breusch-Pagan test for heteroskedasticity in a linear model:
    H_0 = No heteroskedasticity.
    H_1 = Heteroskedasticity is present.

    Returns a list containing three elements:
    1. the Breusch-Pagan test statistic.
    2. the p-value for the test.
    3. the test result.

    :param x: a numpy.ndarray containing the predictor variables. Shape = (nSamples, nPredictors).
    :param y: a 1D numpy.ndarray containing the response variable. Shape = (nSamples, ).
    :return: list containing 3 elements (Breusch-Pagan test statistic, p-value for the test, test result)

    """
    import sklearn.linear_model

    if x.shape[0] != y.shape[0]:
        raise SystemExit("Error: the number of samples differs between x and y.")
    else:
        n_samples = x.shape[0]

    # fit an OLS linear model to y using x:
    lm = sklearn.linear_model.LinearRegression()
    lm.fit(x, y)

    # calculate the squared errors:
    err = (y - lm.predict(x)) ** 2

    # fit an auxiliary regression to the squared errors:
    # why?: to estimate the variance in err explained by x
    lm.fit(x, err)
    pred_err = lm.predict(x)

    # calculate the coefficient of determination:
    ss_tot = numpy.sum((err - numpy.mean(err, axis=0)) ** 2)
    ss_res = numpy.sum((err - pred_err) ** 2)
    r2 = 1 - (ss_res / ss_tot)

    # calculate the Lagrange multiplier:
    LM = n_samples * r2

    # calculate p-value. degrees of freedom = number of predictors.
    # this is equivalent to (p - 1) parameter restrictions in Wikipedia entry.
    try:
        from scipy.stats import chisqprob

        pval = chisqprob(LM, x.shape[1])
    except Exception:
        from scipy.stats.distributions import chi2

        pval = chi2.sf(LM, x.shape[1])

    if pval < 0.01:
        test_result = "Heteroskedasticity present at 99% CI."
    elif pval < 0.05:
        test_result = "Heteroskedasticity present at 95% CI."
    else:
        test_result = "No significant heteroskedasticity."
    return [LM, pval, test_result]


def bin_accuracy_scores_prob(y_true, y_prob):
    """
    A function to calculate accuracy measures for probabilistic responses with sklearn and scipy.
    Function written by Osian Roberts.

    :param y_true: binary class labels, where 0 is absence and 1 is presence.
    :param y_prob: probability of presence scores e.g., generated by a species distribution model.
    :returns: a list containing two arrays - metrics = names of test metrics. scores = test scores for each metric.

    Useful reference:
    https://machinelearningmastery.com/how-to-score-probability-predictions-in-python

    """
    # check inputs:
    if not isinstance(y_true, numpy.ndarray):
        y_true = numpy.array(y_true)
    if not isinstance(y_prob, numpy.ndarray):
        y_prob = numpy.array(y_prob)
    if y_true.ndim != 1:
        raise SystemExit("ERROR: the true labels are not in a 1D array.")
    if y_prob.ndim != 1:
        raise SystemExit(
            "ERROR: the probability of presence values are not in a 1D array."
        )
    if y_true.size != y_prob.size:
        raise SystemExit("ERROR: unequal number of binary labels and probabilities.")

        # ensure that y_true contains binary labels (i.e. 0 or 1 values):
    y_true = y_true.astype("uint8")
    if numpy.min(y_true) != 0 or numpy.max(y_true) != 1:
        raise SystemExit("ERROR: the true labels are not binary (zero or one values).")

    from sklearn.metrics import roc_auc_score

    # calculates area under the receiver operating curve score.
    # A score of 0.5 shows the model is unable to discriminate between presence and absence.
    roc_auc = roc_auc_score(y_true, y_prob)

    from sklearn.metrics import average_precision_score

    # calculates area under the precision-recall curve. Perfect model = 1.0.
    average_precision = average_precision_score(y_true, y_prob)

    from sklearn.metrics import brier_score_loss

    # This is a quadratic loss function that calculates the mean squared error between
    # predicted probabilities and the true presence-absence (binary) labels.
    # A model with no false positives/negatives has a score of 0.0. Perfect model = 1.0.
    brier_score = brier_score_loss(y_true, y_prob)

    from sklearn.metrics import log_loss

    # The is logarithmic loss function that more heavily penalises false positives/negatives than the brier score.
    # A model with no false positives/negatives has a score of 0.0. There is no upper bound.
    log_loss_score = log_loss(y_true, y_prob)

    from scipy.stats import pointbiserialr

    # The point biserial correlation coefficient, range -1 to 1.
    # Quantifies the correlation between a binary and continuous variable.
    r = pointbiserialr(y_true, y_prob)[0]

    metrics = [
        "Test AUC",
        "Point-Biserial r",
        "Av. Precision",
        "Brier Score",
        "Log-Loss Score",
    ]
    scores = numpy.array(
        [roc_auc, r, average_precision, brier_score, log_loss_score]
    ).round(decimals=6)
    del roc_auc, r, average_precision, brier_score, log_loss_score, y_true, y_prob
    return [metrics, scores]


def accuracy_scores_binary(y_true, y_pred):
    """
    A function to calculate accuracy measures for a binary classification.
    Function written by Osian Roberts.

    Parameters:
    :param y_true: observed binary labels, where 0 is absence and 1 is presence.
    :param y_pred: predicted binary labels, where 0 is absence and 1 is presence.
    :returns: a list containing two numpy.arrays - (metrics: name of test metrics, scores: test scores for each metric)

    Reference: See pages 253 - 255 in:
    Guisan et al. (2017). Habitat suitability and distribution models: with applications in R.

    """
    # check inputs:
    if not isinstance(y_true, numpy.ndarray):
        y_true = numpy.array(y_true)
    if not isinstance(y_pred, numpy.ndarray):
        y_pred = numpy.array(y_pred)
    if y_true.ndim != 1:
        raise SystemExit("ERROR: the true labels are not in a 1D array.")
    if y_pred.ndim != 1:
        raise SystemExit("ERROR: the predicted labels are not in a 1D array.")
    if y_true.size != y_pred.size:
        raise SystemExit("ERROR: unequal number of binary labels.")

    # ensure that y_true, y_pred contain binary labels (i.e. 0 or 1 values):
    y_true = y_true.astype("uint8")
    y_pred = y_pred.astype("uint8")
    if numpy.min(y_true) != 0 or numpy.max(y_true) != 1:
        raise SystemExit("ERROR: the true labels are not binary (zero or one values).")
    if numpy.min(y_pred) != 0 or numpy.max(y_pred) != 1:
        raise SystemExit(
            "ERROR: the predicted labels are not binary (zero or one values)."
        )

    metrics = numpy.array(
        [
            "Prevalence",
            "Overall Diagnostic Power",
            "Correct Classification Rate",
            "Misclassification Rate",
            "Presence Predictive Power",
            "Absence Predictive Power",
            "Accuracy",
            "Balanced Accuracy",
            "Sensitivity",
            "Specificity",
            "Precision",
            "F1 Score",
            "Matthews Correlation",
            "Cohen Kappa",
            "Normalised Mutual Information",
            "Hanssen-Kuiper skill",
        ]
    )

    try:
        n_presence = numpy.where(y_true == 1)[0].size
        n_absence = numpy.where(y_true == 0)[0].size

        # calculate true-presence, true-absence, false-presence and false-absence:
        TP = numpy.where((y_true == 1) & (y_pred == 1))[0].size
        TA = numpy.where((y_true == 0) & (y_pred == 0))[0].size
        FP = numpy.where((y_true == 1) & (y_pred == 0))[0].size
        FA = numpy.where((y_true == 0) & (y_pred == 1))[0].size  # aka sweet FA!

        # proportion of presence records:
        prevalence = (TP / FA) / y_true.size

        # proportion of absence records:
        ODP = 1 - prevalence

        # correct classification & misclassification rate
        CCR = (TP + TA) / y_true.size
        MR = (FP + FA) / y_true.size

        # Sensitivity (aka Recall or True Positive Rate):
        sensitivity = TP / n_presence

        # false presence rate - inverse of sensitivity (redundant?)
        # FPR = 1  - sensitivity

        # Presence and absence predictive power:
        PPP = TP / (TP + FP)
        APP = TA / (TA + FA)

        # Specificity (aka True Negative Rate):
        specificity = TA / n_absence

        # false positive rate - inverse of specificity (redundant?)
        # FPR = 1 - specificity

        # Accuracy scores:
        accuracy = (TP + TA) / (n_presence + n_absence)
        balanced_accuracy = ((TP / n_presence) + (TA / n_absence)) / 2

        # precision:
        precision = TP / (TP + FP)

        # F1 score:
        f1_score = 2 * TP / ((2 * TP) + FP + FA)

        # Matthews Correlation Coefficient:
        MCC = ((TP * TA) - (FP * FA)) / (
            ((TP + FP) * (TP + FA) * (TA + FP) * (TA + FA)) ** 0.5
        )

        # Hanssen-Kuiper skill (unreliable when TA is very large):
        TSS = sensitivity + specificity - 1
        del n_presence, n_absence, TP, TA, FP, FA

        from sklearn.metrics import normalized_mutual_info_score as nmi_score

        nmi_score = nmi_score(y_true, y_pred)

        # Cohen's Kappa (caution: sensitive to sample size and proportion of presence records):
        from sklearn.metrics import cohen_kappa_score as kappa

        kappa = kappa(y_true, y_pred)

        scores = numpy.array(
            [
                prevalence,
                ODP,
                CCR,
                MR,
                PPP,
                APP,
                accuracy,
                balanced_accuracy,
                sensitivity,
                specificity,
                precision,
                f1_score,
                MCC,
                kappa,
                nmi_score,
                TSS,
            ]
        ).round(decimals=6)
        del prevalence, ODP, CCR, MR, PPP, APP, accuracy, balanced_accuracy, sensitivity
        del specificity, precision, f1_score, MCC, kappa, nmi_score, TSS
    except Exception:
        scores = numpy.zeros(len(metrics))

    if metrics.size == scores.size:
        return [metrics, scores]
    else:
        raise SystemExit("ERROR: unable to calculate accuracy metrics.")


def get_nbins_histogram(data):
    """
    Calculating the number of bins and the width of those bins for a histogram.

    :param data: 1-d numpy array.
    :return: (n_bins, bin_width) n_bins: int for the number of bins. bin_width: float with the width of the bins.

    """
    n = data.shape[0]
    lq = numpy.percentile(data, 25)
    uq = numpy.percentile(data, 75)
    iqr = uq - lq
    bin_width = 2 * iqr * n ** (-1 / 3)
    n_bins = int((numpy.max(data) - numpy.min(data)) / bin_width) + 2
    return n_bins, float(bin_width)


def get_bin_centres(bin_edges, geometric=False):
    """
    A function to calculate the centre points of bins from the bin edges from a histogram
    e.g., numpy.histogram. My default the arithmetic mean is provided (max+min)/2 but the
    geometric mean can also be calculated sqrt(min*max), this is useful for logarithmically
    spaced bins.

    :param bin_edges: numpy array of the bin edges
    :param geometric: boolean, if False (default) then the arithmetic mean return if True
                      then the geometric mean is returned.
    :returns: bin_centres - numpy array

    """
    if geometric:
        bin_centres = numpy.sqrt(bin_edges[1:] * bin_edges[:-1])
    else:
        bin_centres = (bin_edges[1:] + bin_edges[:-1]) / 2
    return bin_centres


def calc_otsu_threshold(data: numpy.array) -> float:
    """
    A function to calculate otsu's threshold for a dataset. Input is expected
    to be a 1d numpy array.

    Wikipedia, https://en.wikipedia.org/wiki/Otsu's_Method

    :param data: 1d numeric numpy array
    :returns: float (threshold)

    """
    # Note, this is based on the implementation within scikit-image

    # Calculate the histogram
    n_bins, bin_width = get_nbins_histogram(data)
    hist, bin_edges = numpy.histogram(data, bins=n_bins)
    bin_centres = get_bin_centres(bin_edges)
    # print(bin_centres)

    # Normalization so we have probabilities-like values (sum=1)
    hist = hist.astype(numpy.float32)
    hist = 1.0 * hist / numpy.sum(hist)

    # class probabilities for all possible thresholds
    weight1 = numpy.cumsum(hist)
    weight2 = numpy.cumsum(hist[::-1])[::-1]
    # class means for all possible thresholds
    mean1 = numpy.cumsum(hist * bin_centres) / weight1
    mean2 = (numpy.cumsum((hist * bin_centres)[::-1]) / weight2[::-1])[::-1]

    # Clip ends to align class 1 and class 2 variables:
    # The last value of ``weight1``/``mean1`` should pair with zero values in
    # ``weight2``/``mean2``, which do not exist.
    variance12 = weight1[:-1] * weight2[1:] * (mean1[:-1] - mean2[1:]) ** 2

    return bin_centres[numpy.argmax(variance12)]


def calc_yen_threshold(data: numpy.array) -> float:
    """
    A function to calculate yen threshold for a dataset. Input is expected
    to be a 1d numpy array.

    Yen J.C., Chang F.J., and Chang S. (1995) "A New Criterion
    for Automatic Multilevel Thresholding" IEEE Trans. on Image
    Processing, 4(3): 370-378. DOI:`10.1109/83.366472`

    :param data: 1d numeric numpy array
    :returns: float (threshold)

    """
    # Note, this is based on the implementation within scikit-image

    # Calculate the histogram
    n_bins, bin_width = get_nbins_histogram(data)
    hist, bin_edges = numpy.histogram(data, bins=n_bins)
    bin_centres = get_bin_centres(bin_edges)

    # Normalization so we have probabilities-like values (sum=1)
    hist = hist.astype(numpy.float32)
    hist = 1.0 * hist / numpy.sum(hist)

    # Calculate probability mass function
    pmf = hist.astype(numpy.float32) / hist.sum()
    p1 = numpy.cumsum(pmf)  # Cumulative normalized histogram
    p1_sq = numpy.cumsum(pmf**2)
    # Get cumsum calculated from end of squared array:
    p2_sq = numpy.cumsum(pmf[::-1] ** 2)[::-1]
    # P2_sq indexes is shifted +1. I assume, with P1[:-1] it's help avoid
    # '-inf' in crit. ImageJ Yen implementation replaces those values by zero.
    crit = numpy.log(
        ((p1_sq[:-1] * p2_sq[1:]) ** -1) * (p1[:-1] * (1.0 - p1[:-1])) ** 2
    )
    return bin_centres[crit.argmax()]


def calc_isodata_threshold(data: numpy.array) -> float:
    """
    A function to calculate inter-means threshold for a dataset. Input is expected
    to be a 1d numpy array. Histogram-based threshold, known as Ridler-Calvard method
    or inter-means. Threshold values returned satisfy the following equality::

    threshold = (data[data <= threshold].mean() + data[data > threshold].mean()) / 2.0

    Ridler, TW & Calvard, S (1978), "Picture thresholding using an iterative selection
    method" IEEE Transactions on Systems, Man and Cybernetics 8: 630-632,
    DOI:`10.1109/TSMC.1978.4310039`

    :param data: 1d numeric numpy array
    :return: float (threshold)

    """
    # Note, this is based on the implementation within scikit-image

    # Calculate the histogram
    n_bins, bin_width = get_nbins_histogram(data)
    hist, bin_edges = numpy.histogram(data, bins=n_bins)
    bin_centres = get_bin_centres(bin_edges)

    # Normalization so we have probabilities-like values (sum=1)
    hist = hist.astype(numpy.float32)
    hist = 1.0 * hist / numpy.sum(hist)

    # csuml and csumh contain the count of pixels in that bin or lower, and
    # in all bins strictly higher than that bin, respectively
    csuml = numpy.cumsum(hist)
    csumh = csuml[-1] - csuml

    # intensity_sum contains the total pixel intensity from each bin
    intensity_sum = hist * bin_centres

    # l and h contain average value of all pixels in that bin or lower, and
    # in all bins strictly higher than that bin, respectively.
    # Note that since exp.histogram does not include empty bins at the low or
    # high end of the range, csuml and csumh are strictly > 0, except in the
    # last bin of csumh, which is zero by construction.
    # So no worries about division by zero in the following lines, except
    # for the last bin, but we can ignore that because no valid threshold
    # can be in the top bin.
    # To avoid the division by zero, we simply skip over the last element in
    # all future computation.
    csum_intensity = numpy.cumsum(intensity_sum)
    lower = csum_intensity[:-1] / csuml[:-1]
    higher = (csum_intensity[-1] - csum_intensity[:-1]) / csumh[:-1]

    # isodata finds threshold values that meet the criterion t = (l + m)/2
    # where l is the mean of all pixels <= t and h is the mean of all pixels
    # > t, as calculated above. So we are looking for places where
    # (l + m) / 2 equals the intensity value for which those l and m figures
    # were calculated -- which is, of course, the histogram bin centers.
    # We only require this equality to be within the precision of the bin
    # width, of course.
    all_mean = (lower + higher) / 2.0
    bin_width = bin_centres[1] - bin_centres[0]

    # Look only at thresholds that are below the actual all_mean value,
    # for consistency with the threshold being included in the lower pixel
    # group. Otherwise can get thresholds that are not actually fixed-points
    # of the isodata algorithm. For float images, this matters less, since
    # there really can't be any guarantees anymore anyway.
    distances = all_mean - bin_centres[:-1]
    thresholds = bin_centres[:-1][(distances >= 0) & (distances < bin_width)]

    return thresholds[0]


def calc_hist_cross_entropy(data: numpy.array, threshold: float) -> float:
    """
    A function which computes the cross-entropy between distributions
    above and below a threshold. Cross-entropy is a measure of the difference
    between two probability distributions for a given random variable or set of events.

    See Li and Lee (1993); this is the objective function ``threshold_li``
    minimizes. This function can be improved but this implementation most
    closely matches equation 8 in Li and Lee (1993) and equations 1-3 in
    Li and Tam (1998).

    Li C.H. and Lee C.K. (1993) "Minimum Cross Entropy Thresholding"
    Pattern Recognition, 26(4): 617-625 DOI:`10.1016/0031-3203(93)90115-D`

    Li C.H. and Tam P.K.S. (1998) "An Iterative Algorithm for Minimum
    Cross Entropy Thresholding" Pattern Recognition Letters, 18(8): 771-776
    DOI: `10.1016/S0167-8655(98)00057-9`

    :param data: 1d numeric numpy array
    :param threshold: float spliting to the two parts of the histogram.
    :returns: float (cross-entropy target value)

    """
    # Note, this is based on the implementation within scikit-image

    n_bins, bin_width = get_nbins_histogram(data)
    histogram, bin_edges = numpy.histogram(data, bins=n_bins, density=True)
    bin_centres = get_bin_centres(bin_edges)

    # Make sure values are positive and not zero.
    if numpy.min(bin_centres) < 1:
        offset = abs(numpy.min(bin_centres)) + 1
        bin_centres = bin_centres + offset
        threshold = threshold + offset

    t = numpy.flatnonzero(bin_centres > threshold)[0]
    m0a = numpy.sum(histogram[:t])  # 0th moment, background
    m0b = numpy.sum(histogram[t:])
    m1a = numpy.sum(histogram[:t] * bin_centres[:t])  # 1st moment, background
    m1b = numpy.sum(histogram[t:] * bin_centres[t:])
    mua = m1a / m0a  # mean value, background
    mub = m1b / m0b

    nu = -m1a * numpy.log(mua) - m1b * numpy.log(mub)
    return nu


def calc_li_threshold(
    data: numpy.array, tolerance: float = None, initial_guess: float = None
) -> float:
    """
    A function which calculates a threshold value by Li's iterative
    Minimum Cross Entropy method.

    Li C.H. and Lee C.K. (1993) "Minimum Cross Entropy Thresholding"
    Pattern Recognition, 26(4): 617-625 DOI:`10.1016/0031-3203(93)90115-D`


    :param data: 1d numeric numpy array
    :param tolerance: float (optional) - Finish the computation when the
                     change in the threshold in an iteration is less than
                     this value. By default, this is half the smallest
                     difference between data values.
    :param initial_guess: float (optional) - Li's iterative method uses
                          gradient descent to find the optimal threshold.
                          If the histogram contains more than two modes
                          (peaks), the gradient descent could get stuck
                          in a local optimum. An initial guess for the
                          iteration can help the algorithm find the
                          globally-optimal threshold.
    :returns: float (threshold)

    """
    # Note, this is based on the implementation within scikit-image

    # At this point, the data only contains numpy.inf, -numpy.inf, or valid numbers
    data = data[numpy.isfinite(data)]

    # Li's algorithm requires positive image (because of log(mean))
    offset = 0.0
    if numpy.min(data) < 1:
        offset = abs(numpy.min(data)) + 1
        data = data + offset
        if initial_guess is not None:
            initial_guess = initial_guess + offset

    # If not provided define the tolerance
    if tolerance is None:
        tolerance = numpy.min(numpy.diff(numpy.unique(data))) / 2

    # Initial estimate for iteration. See "initial_guess" in the parameter list
    if initial_guess is None:
        t_next = numpy.mean(data)
    else:
        t_next = initial_guess

    # initial value for t_curr must be different from t_next by at
    # least the tolerance. Since the image is positive, we ensure this
    # by setting to a large-enough negative number
    t_curr = -2 * tolerance

    # Stop the iterations when the difference between the
    # new and old threshold values is less than the tolerance
    while abs(t_next - t_curr) > tolerance:
        t_curr = t_next
        foreground = data > t_curr
        mean_fore = numpy.mean(data[foreground])
        mean_back = numpy.mean(data[~foreground])

        t_next = (mean_back - mean_fore) / (numpy.log(mean_back) - numpy.log(mean_fore))

    threshold = t_next - offset
    return threshold


def calc_kurt_skew_threshold(
    data: numpy.array,
    max_val: float,
    min_val: float,
    init_thres: float,
    low_thres: bool = True,
    contamination: float = 10.0,
    only_kurtosis: bool = False,
) -> float:
    """
    A function to calculate a threshold either side of the histogram based on

    :param data: 1d numeric numpy array
    :param max_val:
    :param min_val:
    :param init_thres:
    :param low_thres:
    :param contamination:
    :param only_kurtosis:
    :return:

    """
    import scipy.optimize
    import scipy.stats

    if len(data.shape) > 1:
        raise rsgislib.RSGISPyException("Expecting a single variable.")

    if (contamination < 1) or (contamination > 100):
        raise rsgislib.RSGISPyException(
            "contamination parameter should have a value between 1 and 100."
        )

    if low_thres:
        low_percent = numpy.percentile(data, contamination)
        if low_percent < max_val:
            max_val = low_percent

        if min_val >= max_val:
            min_val = numpy.min(data)
    else:
        up_percent = numpy.percentile(data, 100 - contamination)
        if up_percent > min_val:
            min_val = up_percent

        if max_val <= min_val:
            max_val = numpy.max(data)

    if min_val == max_val:
        print("Min: {}".format(min_val))
        print("Max: {}".format(max_val))
        raise rsgislib.RSGISPyException("Min and Max values are the same.")
    elif min_val > max_val:
        print("Min: {}".format(min_val))
        print("Max: {}".format(max_val))
        raise rsgislib.RSGISPyException(
            "Min value is greater than max - note this can happened if the "
            "contamination parameter caused threshold to be changed."
        )

    if (init_thres < min_val) or (init_thres > max_val):
        init_thres = min_val + ((max_val - min_val) / 2)

    def _opt_fun(x, *args):
        data = args[0]
        if low_thres:
            # Subset by x threshold
            data_sub = data[data > x]
        else:
            # Subset by x threshold
            data_sub = data[data < x]

        # Calculate kurtosis and skewness
        kurtosis = scipy.stats.kurtosis(data_sub)
        if only_kurtosis:
            kur_skew = kurtosis
        else:
            skew = scipy.stats.skew(data_sub)
            # Product of kurtosis and skewness
            kur_skew = abs(kurtosis) + abs(skew)

        return kur_skew

    opt_rslt = scipy.optimize.dual_annealing(
        _opt_fun, bounds=[(min_val, max_val)], args=[data], x0=[init_thres]
    )

    out_thres = None
    if opt_rslt.success:
        out_thres = opt_rslt.x[0]
    else:
        raise rsgislib.RSGISPyException("Optimisation failed, no threshold found.")

    return out_thres


def mask_data_to_valid(
    data: numpy.array, lower_limit: float = None, upper_limit: float = None
):
    """
    A function which removes rows from an nxm numpy array which are
    not finite or outside of the upper and lower thresholds provided.

    :param data: nxm numpy array (n rows, m features)
    :param lower_limit: lower threshold valid data is greater than this value.
    :param upper_limit: upper threshold valid data is lower than this value.

    """
    data = data[numpy.all(numpy.isfinite(data), axis=1)]
    if lower_limit is not None:
        data = data[numpy.all(data > lower_limit, axis=1)]
    if upper_limit is not None:
        data = data[numpy.all(data < upper_limit, axis=1)]
    return data


def bias_score(y_true, y_pred):
    """
    A function to calculate the regression model bias for each response variable
    in a multivariate regression.

    :param y_true: numpy.ndarray of true y values, shape=(n_samples, n_responses).
    :param y_true: numpy.ndarray of predicted y values, shape=(n_samples, n_responses).
    :return: bias (absolute bias), norm_bias (normalised bias (aka percentage bias))

    """
    import numpy

    if not isinstance(y_true, numpy.ndarray):
        y_true = numpy.array(y_true)
    if not isinstance(y_pred, numpy.ndarray):
        y_pred = numpy.array(y_pred)
    if y_true.ndim != y_pred.ndim:
        raise rsgislib.RSGISPyException("Error: y_true dimensions != y_pred.")
    if y_true.size != y_pred.size:
        raise rsgislib.RSGISPyException("Error: y_true size != y_pred.")

    bias = numpy.mean(y_pred - y_true, axis=0)
    norm_bias = (bias / numpy.mean(y_true, axis=0)) * 100
    return bias, norm_bias


def decompose_bias_variance(y_true, y_pred):
    """
    A function to perform bias-variance decomposition.
    Decompose the mean squared error into: Bias^2 + Variance + Irreducible Error.
    Useful example: https://towardsdatascience.com/mse-and-bias-variance-decomposition-77449dd2ff55

    :param y_true: numpy.ndarray of true y values, shape=(n_samples, n_responses).
    :param y_true: numpy.ndarray of predicted y values, shape=(n_samples, n_responses).
    :return: set (mse, bias_squared, variance, noise)

    """
    import numpy

    if not isinstance(y_true, numpy.ndarray):
        y_true = numpy.array(y_true)
    if not isinstance(y_pred, numpy.ndarray):
        y_pred = numpy.array(y_pred)
    if y_true.ndim != y_pred.ndim:
        raise SystemExit("Error: y_true dimensions != y_pred.")
    if y_true.size != y_pred.size:
        raise SystemExit("Error: y_true size != y_pred.")

    mse = numpy.mean((y_pred - y_true) ** 2, axis=0)
    bias_squared = numpy.mean(y_pred - y_true, axis=0) ** 2
    variance = numpy.var(y_pred - y_true, axis=0)
    noise = mse - (bias_squared + variance)
    if isinstance(noise, numpy.ndarray):
        noise[noise < 0] = 0
    else:
        if noise < 0:
            noise = 0
    return mse, bias_squared, variance, noise


def calc_semivariogram(
    pts_coords: numpy.array,
    data_vals: numpy.array,
    out_data_file: str = None,
    out_plot_file: str = None,
    max_lag: Union[float, str] = "median",
    n_lags: int = 25,
    normalize_vals: bool = False,
):
    """
    A function which calculates semi-variance for the data provided using the
    skgstat module (https://scikit-gstat.readthedocs.io/)

    :param pts_coords: the x/y coordinates of the points for which the semi-variance is
                       calculated. Shape must be [n, 2] where n is the number of points
    :param data_vals: the data values of the points for which the semi-variance is
                      calculated.
    :param out_data_file: Optionally output a CSV file with the distance, semi-variance
                          and count. Default is None but if file path provided the
                          output will be produced.
    :param out_plot_file: Optionally output a plot file of the distance, semi-variance
                          and count. Default is None but if file path provided the
                          output will be produced.
    :param max_lag: Can specify the maximum lag distance directly by giving a value
                    larger than 1. Can also be a string with value ‘median’, ‘mean’.
                    See skgstat.Variogram documentation.
    :param n_lags: Specify the number of lag classes to be defined by the binning
                   function. See skgstat.Variogram documentation.
    :param normalize_vals: Defaults to False. If True, the independent and dependent
                           variable will be normalized to the range [0,1].
                           See skgstat.Variogram documentation.
    :return: returns a pandas dataframe with the lag_bins, variance and count

    """
    import matplotlib.pyplot as plt
    import skgstat
    import pandas

    if pts_coords.shape[1] != 2:
        raise rsgislib.RSGISPyException(
            "Error: pts_coords has wrong shape. Must be [n, 2] "
            "where n is the number of points."
        )

    if pts_coords.shape[0] != data_vals.shape[0]:
        raise rsgislib.RSGISPyException(
            "Error: the length of pts_coords must be the same as data_vals."
        )

    vario_obj = skgstat.Variogram(
        pts_coords, data_vals, normalize=normalize_vals, maxlag=max_lag, n_lags=n_lags
    )

    variogram_data = vario_obj.get_empirical()

    n_count = numpy.fromiter((g.size for g in vario_obj.lag_classes()), dtype=int)

    vario_data = {
        "distance": variogram_data[0],
        "semivariance": variogram_data[1],
        "count": n_count,
    }

    vario_out_df = pandas.DataFrame(vario_data)

    if out_data_file is not None:
        vario_out_df.to_csv(out_data_file)

    if out_plot_file is not None:
        fig = plt.figure(figsize=(8, 5))
        ax1 = plt.subplot2grid((5, 1), (1, 0), rowspan=4)
        ax2 = plt.subplot2grid((5, 1), (0, 0), sharex=ax1)
        fig.subplots_adjust(hspace=0)

        ax1.scatter(x=vario_out_df["distance"], y=vario_out_df["semivariance"])
        ax1.set_xlabel("Distance")
        ax1.set_ylabel("Semi-Variance")

        if normalize_vals:
            ax1.set_xlim([0, 1.05])
            ax1.set_ylim([0, 1.05])

        ax1.grid(False)
        ax1.vlines(
            vario_out_df["distance"],
            *ax1.axes.get_ybound(),
            colors=(0.85, 0.85, 0.85),
            linestyles="dashed"
        )

        # set the sum of hist bar widths to 70% of the x-axis space
        w = (numpy.max(vario_out_df["distance"]) * 0.7) / len(vario_out_df["count"])

        # plot bar chart with count of number of pairs
        ax2.bar(
            vario_out_df["distance"],
            vario_out_df["count"],
            width=w,
            align="center",
            color="red",
        )

        plt.setp(ax2.axes.get_xticklabels(), visible=False)
        ax2.axes.set_yticks(ax2.axes.get_yticks()[1:])

        ax2.grid(False)
        ax2.vlines(
            vario_out_df["distance"],
            *ax2.axes.get_ybound(),
            colors=(0.85, 0.85, 0.85),
            linestyles="dashed"
        )
        ax2.axes.set_ylabel("N")

        # Save figure
        plt.savefig(out_plot_file)

    return vario_out_df


def standarise_img_data(img_data: numpy.array) -> numpy.array:
    """
    Standardise the input image (minus the mean and
    divide by the standard deviation).

    :param img_data: a numpy array with the shape [bands, height, width] or
                     [bands, n_pxls].
    :return: a numpy array with the standarised pixel values.

    """
    if len(img_data.shape) == 3:
        img_bands, img_height, img_width = img_data.shape
        n_img_pxls = img_height * img_width
        img_data_reshp = numpy.reshape(img_data, (img_bands, img_height * img_width))
    elif len(img_data.shape) == 2:
        img_bands, n_img_pxls = img_data.shape
        img_data_reshp = img_data
    else:
        raise rsgislib.RSGISPyException(
            "Input array must have either 2 or 3 dimensions."
        )

    img_band_mean = numpy.mean(img_data_reshp, axis=1, keepdims=True)
    img_band_cen = img_data_reshp - img_band_mean
    img_band_var = numpy.sum(numpy.power(img_band_cen, 2), axis=1, keepdims=True) / (
        n_img_pxls
    )
    img_band_std = numpy.sqrt(img_band_var)
    std_img_data = img_band_cen / img_band_std
    if len(img_data.shape) == 3:
        std_img_data = numpy.reshape(std_img_data, (img_bands, img_height, img_width))

    return std_img_data


def normalise_img_data(img_data: numpy.array) -> numpy.array:
    """
    Normalise the input image (data - min)/range.

    :param img_data: a numpy array with the shape [bands, height, width]
    :return: a numpy array with the normalised pixel values.

    """
    if len(img_data.shape) == 3:
        img_bands, img_height, img_width = img_data.shape
        img_data_reshp = numpy.reshape(img_data, (img_bands, img_height * img_width))
    elif len(img_data.shape) == 2:
        img_bands, n_img_pxls = img_data.shape
        img_data_reshp = img_data
    else:
        raise rsgislib.RSGISPyException(
            "Input array must have either 2 or 3 dimensions."
        )

    max_value = numpy.max(img_data_reshp, axis=1, keepdims=True)
    min_value = numpy.min(img_data_reshp, axis=1, keepdims=True)
    diff_value = max_value - min_value
    nm_img = (img_data_reshp - min_value) / diff_value
    if len(img_data.shape) == 3:
        nm_img = numpy.reshape(nm_img, (img_bands, img_height, img_width))

    return nm_img
