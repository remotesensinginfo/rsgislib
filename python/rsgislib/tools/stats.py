#!/usr/bin/env python
"""
The tools.stats module contains some useful tools for calculating useful statistics which aren't easily available else where.
"""

def calculate_VIF(X):
    """
    A function to calculate variance inflation factors to
    investigate multicollinearity between predictor variables. 
    Function written by Osian Roberts.

    :param X: a 2D numpy array of shape = (nSamples, nPredictors).
    :returns: VIF_scores; A numpy array containing the VIF for each predictor variable.

    Interpretation of VIF scores (this is somewhat subjective):
    1 = No multicollinearity.
    1-5 = Moderate multicollinearity.
    > 5 = High multicollinearity.
    > 10 = This predictor should be removed from the model.
    
    Example::
    
    import numpy
    # create some fake predictors to test the function:
    x1 = numpy.random.uniform(1, 100, 500)  # this should be highly collinear with x3.
    x2 = numpy.random.uniform(1, 100, 500)  # this should be independent of x1.
    x3 = 0.85 * x1 + 50 # this should be highly collinear with x1.
    x4 = 0.85 * numpy.log10(x1) + 50 # this should be moderately collinear with x1.
    
    X = numpy.array([x1, x2, x3, x4]).T
    
    vif_scores = calculate_VIF(X)
        
    for i in range(vif_scores.size):
        print('VIF for x' + str(i + 1) + ':', vif_scores[i])
    
    """
    import numpy
    from sklearn.linear_model import LinearRegression
    
    print('Calculating VIF for {} predictors variables...'.format(X.shape[1]))
    # fit auxiliary regressions to each predictor in X:
    lm = LinearRegression()
    VIF_scores = []
    for i in range(X.shape[1]):
        # fit an OLS linear model to each predictor (y) using the remaining predictor variables (x):
        y = X[:, i]
        x = numpy.delete(X, i, axis=1)
        lm.fit(x, y)
        y_pred = lm.predict(x)
        del x

        # calculate the coefficient of determination:
        ss_tot = sum((y - numpy.mean(y))**2)
        ss_res = sum((y - y_pred)**2)
        r2 = 1 - (ss_res / ss_tot)
        del y, y_pred, ss_res, ss_tot

        # calculate VIF:
        if r2 == 1:
            r2 = 0.999  # this prevents division by zero errors when r2 == 1.
        VIF_scores.append(1 / (1 - r2))
        del r2
    del lm, X
    VIF_scores = numpy.array(VIF_scores).round(decimals=1)
    return VIF_scores


