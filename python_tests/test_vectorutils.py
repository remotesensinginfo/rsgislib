import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
VECTORUTILS_DATA_DIR = os.path.join(DATA_DIR, "vectorutils")

def test_delete_vector_file(tmp_path):
    import rsgislib.vectorutils
    import shutil

    ref_vec_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    cp_vec_file = os.path.join(tmp_path, "./cls_forest_smpls.gpkg")
    shutil.copyfile(ref_vec_file, cp_vec_file)
    if not os.path.exists(cp_vec_file):
        raise Exception("Test file is not present")
    rsgislib.vectorutils.delete_vector_file(cp_vec_file)

    assert not os.path.exists(cp_vec_file)


def test_merge_to_multi_layer_vec(tmp_path):
    import rsgislib.vectorutils

    vec_file_1 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_file_2 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_file_3 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_urban_smpls.gpkg")
    vec_file_4 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_water_smpls.gpkg")

    input_file_lyrs = list()
    input_file_lyrs.append(rsgislib.vectorutils.VecLayersInfoObj(vec_file_1, "cls_forest_smpls", "cls_forest_smpls"))
    input_file_lyrs.append(rsgislib.vectorutils.VecLayersInfoObj(vec_file_2, "cls_grass_smpls", "cls_grass_smpls"))
    input_file_lyrs.append(rsgislib.vectorutils.VecLayersInfoObj(vec_file_3, "cls_urban_smpls", "cls_urban_smpls"))
    input_file_lyrs.append(rsgislib.vectorutils.VecLayersInfoObj(vec_file_4, "cls_water_smpls", "cls_water_smpls"))

    out_vec_file = os.path.join(tmp_path, "./cls_poly_smpls.gpkg")
    rsgislib.vectorutils.merge_to_multi_layer_vec(input_file_lyrs, out_vec_file, out_format="GPKG", overwrite=False)

    assert os.path.exists(out_vec_file)


def test_merge_to_multi_layer_vec_exists(tmp_path):
    import rsgislib.vectorutils

    vec_file_1 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_file_2 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_file_3 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_urban_smpls.gpkg")
    vec_file_4 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_water_smpls.gpkg")

    input_file_lyrs = list()
    input_file_lyrs.append(rsgislib.vectorutils.VecLayersInfoObj(vec_file_1, "cls_forest_smpls", "cls_forest_smpls"))
    input_file_lyrs.append(rsgislib.vectorutils.VecLayersInfoObj(vec_file_2, "cls_grass_smpls", "cls_grass_smpls"))
    input_file_lyrs.append(rsgislib.vectorutils.VecLayersInfoObj(vec_file_3, "cls_urban_smpls", "cls_urban_smpls"))
    input_file_lyrs.append(rsgislib.vectorutils.VecLayersInfoObj(vec_file_4, "cls_water_smpls", "cls_water_smpls"))

    out_vec_file = os.path.join(tmp_path, "./cls_poly_smpls.gpkg")
    rsgislib.vectorutils.merge_to_multi_layer_vec(input_file_lyrs, out_vec_file, out_format="GPKG", overwrite=False)
    # Run it again so that file exists.
    rsgislib.vectorutils.merge_to_multi_layer_vec(input_file_lyrs, out_vec_file, out_format="GPKG", overwrite=True)

    assert os.path.exists(out_vec_file)





