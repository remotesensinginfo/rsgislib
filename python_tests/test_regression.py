import pytest

SKLEARN_NOT_AVAIL = False
try:
    import sklearn
except ImportError:
    SKLEARN_NOT_AVAIL = True

@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_get_regression_stats():
    import rsgislib.regression
    import numpy

    ref_data = numpy.random.rand(100)
    pred_data = numpy.random.rand(100)
    rsgislib.regression.get_regression_stats(ref_data, pred_data, n_vars=1)