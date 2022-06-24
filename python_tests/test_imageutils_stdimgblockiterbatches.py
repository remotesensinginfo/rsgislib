import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")


def test_stdimgblockiterbatches_ndvi(tmp_path):
    import rsgislib.imageutils
    import rsgislib.imageutils.stdimgblockiterbatches
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

    img_iter = rsgislib.imageutils.stdimgblockiterbatches.StdImgBlockIterBatches(
        imgBandInfo, 64, 50
    )
    img_iter.create_output_imgs(imgOutInfo)

    for x_block_lst, y_block_lst, img_meta_data, img_data in tqdm.tqdm(img_iter):
        img_data_shp = img_data.shape

        out_data_arr = numpy.zeros(
            (img_data_shp[0], 1, img_data_shp[2], img_data_shp[3]), dtype=float
        )

        for b in range(img_data_shp[0]):
            out_data_arr[b] = (img_data[b][8] - img_data[b][3]) / (
                img_data[b][8] + img_data[b][3]
            )

        out_data = {"sen2_ndvi": out_data_arr}
        img_iter.write_blocks_to_image(x_block_lst, y_block_lst, out_data)

    assert os.path.exists(output_img)


def test_stdimgblockiterbatches_sgl_batch_ndvi(tmp_path):
    import rsgislib.imageutils
    import rsgislib.imageutils.stdimgblockiterbatches
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

    img_iter = rsgislib.imageutils.stdimgblockiterbatches.StdImgBlockIterBatches(
        imgBandInfo, 200, 100
    )
    img_iter.create_output_imgs(imgOutInfo)

    for x_block_lst, y_block_lst, img_meta_data, img_data in tqdm.tqdm(img_iter):
        img_data_shp = img_data.shape

        out_data_arr = numpy.zeros(
            (img_data_shp[0], 1, img_data_shp[2], img_data_shp[3]), dtype=float
        )

        for b in range(img_data_shp[0]):
            out_data_arr[b] = (img_data[b][8] - img_data[b][3]) / (
                img_data[b][8] + img_data[b][3]
            )

        out_data = {"sen2_ndvi": out_data_arr}
        img_iter.write_blocks_to_image(x_block_lst, y_block_lst, out_data)

    assert os.path.exists(output_img)


def test_stdimgblockiterbatches_ndvi_ndwi(tmp_path):
    import rsgislib.imageutils
    import rsgislib.imageutils.stdimgblockiterbatches
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

    img_iter = rsgislib.imageutils.stdimgblockiterbatches.StdImgBlockIterBatches(
        imgBandInfo, 200, 50
    )
    img_iter.create_output_imgs(imgOutInfo)

    for x_block_lst, y_block_lst, img_meta_data, img_data in tqdm.tqdm(img_iter):
        img_data_shp = img_data.shape

        out_ndvi_data_arr = numpy.zeros(
            (img_data_shp[0], 1, img_data_shp[2], img_data_shp[3]), dtype=float
        )
        out_ndwi_data_arr = numpy.zeros(
            (img_data_shp[0], 1, img_data_shp[2], img_data_shp[3]), dtype=float
        )

        for b in range(img_data_shp[0]):
            out_ndvi_data_arr[b] = (img_data[b][8] - img_data[b][3]) / (
                img_data[b][8] + img_data[b][3]
            )
            out_ndwi_data_arr[b] = (img_data[b][8] - img_data[b][9]) / (
                img_data[b][8] + img_data[b][9]
            )

        out_data = {"sen2_ndvi": out_ndvi_data_arr, "sen2_ndwi": out_ndwi_data_arr}
        img_iter.write_blocks_to_image(x_block_lst, y_block_lst, out_data)

    assert os.path.exists(out_ndvi_img) and os.path.exists(out_ndwi_img)
