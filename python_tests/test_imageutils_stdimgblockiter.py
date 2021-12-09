import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")


def test_stdimgblockiter_ndvi(tmp_path):
    import rsgislib.imageutils
    import rsgislib.imageutils.stdimgblockiter
    import numpy
    import tqdm

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_ndvi.kea")

    imgBandInfo = []
    imgBandInfo.append(
        rsgislib.imageutils.ImageBandInfo(
            file_name=input_img, name="sen2", bands=[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        )
    )

    imgOutInfo = []
    imgOutInfo.append(
        rsgislib.imageutils.OutImageInfo(
            file_name=output_img,
            name="sen2_ndvi",
            nbands=1,
            no_data_val=-999,
            gdalformat="KEA",
            datatype=rsgislib.TYPE_32FLOAT,
        )
    )

    img_iter = rsgislib.imageutils.stdimgblockiter.StdImgBlockIter(imgBandInfo, 64)
    img_iter.create_output_imgs(imgOutInfo)
    for x_block, y_block, out_img_data in tqdm.tqdm(img_iter):
        ndvi = (
            out_img_data["sen2"]["np_arr"][8] - out_img_data["sen2"]["np_arr"][3]
        ) / (out_img_data["sen2"]["np_arr"][8] + out_img_data["sen2"]["np_arr"][3])
        out_data = {"sen2_ndvi": numpy.expand_dims(ndvi, axis=0)}
        img_iter.write_block_to_image(x_block, y_block, out_data)

    assert os.path.exists(output_img)


def test_stdimgblockiter_ndvi_ndwi(tmp_path):
    import rsgislib.imageutils
    import rsgislib.imageutils.stdimgblockiter
    import numpy
    import tqdm

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_ndvi_img = os.path.join(tmp_path, "sen2_20210527_aber_ndvi.kea")
    out_ndwi_img = os.path.join(tmp_path, "sen2_20210527_aber_ndwi.kea")

    imgBandInfo = []
    imgBandInfo.append(
        rsgislib.imageutils.ImageBandInfo(
            file_name=input_img, name="sen2", bands=[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        )
    )

    imgOutInfo = []
    imgOutInfo.append(
        rsgislib.imageutils.OutImageInfo(
            file_name=out_ndvi_img,
            name="sen2_ndvi",
            nbands=1,
            no_data_val=-999,
            gdalformat="KEA",
            datatype=rsgislib.TYPE_32FLOAT,
        )
    )
    imgOutInfo.append(
        rsgislib.imageutils.OutImageInfo(
            file_name=out_ndwi_img,
            name="sen2_ndwi",
            nbands=1,
            no_data_val=-999,
            gdalformat="KEA",
            datatype=rsgislib.TYPE_32FLOAT,
        )
    )

    img_iter = rsgislib.imageutils.stdimgblockiter.StdImgBlockIter(imgBandInfo, 64)
    img_iter.create_output_imgs(imgOutInfo)
    for x_block, y_block, out_img_data in tqdm.tqdm(img_iter):
        ndvi = (
            out_img_data["sen2"]["np_arr"][8] - out_img_data["sen2"]["np_arr"][3]
        ) / (out_img_data["sen2"]["np_arr"][8] + out_img_data["sen2"]["np_arr"][3])
        ndwi = (
            out_img_data["sen2"]["np_arr"][8] - out_img_data["sen2"]["np_arr"][9]
        ) / (out_img_data["sen2"]["np_arr"][8] + out_img_data["sen2"]["np_arr"][9])
        out_data = {
            "sen2_ndvi": numpy.expand_dims(ndvi, axis=0),
            "sen2_ndwi": numpy.expand_dims(ndwi, axis=0),
        }
        img_iter.write_block_to_image(x_block, y_block, out_data)

    assert os.path.exists(out_ndvi_img) and os.path.exists(out_ndwi_img)
