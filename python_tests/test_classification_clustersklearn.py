import os
import pytest

SKLEARN_NOT_AVAIL = False
try:
    import sklearn
except ImportError:
    SKLEARN_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")


#@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
@pytest.mark.skip(
    reason="TODO: Function need updating to not use rios.imagereader.ImageReader"
)
def test_img_pixel_sample_cluster(tmp_path):
    import rsgislib.classification.clustersklearn

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_clustered.kea")
    rsgislib.classification.clustersklearn.img_pixel_sample_cluster(
        input_img, output_img, gdalformat="KEA"
    )
    assert os.path.exists(output_img)


#@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
@pytest.mark.skip(
    reason="TODO: Function need updating to not use rios.imagereader.ImageReader"
)
def test_img_pixel_tiled_cluster(tmp_path):
    import rsgislib.classification.clustersklearn

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_clustered.kea")
    rsgislib.classification.clustersklearn.img_pixel_tiled_cluster(
        input_img, output_img, gdalformat="KEA"
    )
    assert os.path.exists(output_img)


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_img_pixel_cluster(tmp_path):
    import rsgislib.classification.clustersklearn

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_clustered.kea")
    rsgislib.classification.clustersklearn.img_pixel_cluster(
        input_img, output_img, gdalformat="KEA"
    )
    assert os.path.exists(output_img)
