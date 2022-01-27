import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
TIMESERIES_DATA_DIR = os.path.join(DATA_DIR, "timeseries")

def test_get_ST_model_coeffs(tmp_path):
    from rsgislib.timeseries import modelfitting
    import rsgislib.tools.utils

    sen2_imgs_path = os.path.join(TIMESERIES_DATA_DIR, "sen2_subs")
    sen2_imgs_lut_ref_file = os.path.join(TIMESERIES_DATA_DIR, './timeseries_imgs.json')
    sen2_imgs_lut_file = os.path.join(tmp_path, './timeseries_imgs.json')

    sen2_imgs_ref_dict = rsgislib.tools.utils.read_json_to_dict(sen2_imgs_lut_ref_file)
    sen2_imgs_dict = dict()
    for date_key in sen2_imgs_ref_dict:
        sen2_imgs_dict[date_key] = os.path.join(sen2_imgs_path, sen2_imgs_ref_dict[date_key])

    rsgislib.tools.utils.write_dict_to_json(sen2_imgs_dict, sen2_imgs_lut_file)

    coeffs_img = os.path.join(tmp_path, 'timeseries_mdl_coeffs.kea')
    modelfitting.get_ST_model_coeffs(sen2_imgs_lut_file, coeffs_img, bands=[3, 7, 9], num_processes=1)

    assert os.path.exists(coeffs_img)

def test_predict_for_date(tmp_path):
    from rsgislib.timeseries import modelfitting

    coeffs_img = os.path.join(TIMESERIES_DATA_DIR, "timeseries_mdl_coeffs.kea")
    output_img = os.path.join(tmp_path, 'sen2_pred_img_20190610.kea')
    modelfitting.predict_for_date('2019-06-10', coeffs_img, output_img)

    assert os.path.exists(output_img)
