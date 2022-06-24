#!/usr/bin/env python

import pprint
import math

import numpy
from osgeo import gdal

import rsgislib
import rsgislib.imageutils
import rsgislib.tools.projection


class StdImgBlockIterBatches:

    """
    A class to read and write images in blocks where all blocks are all identical in
    size. To deal with image boundaries, where block do not divide into the the image
    size, the outside blocks will be overlapped with the previous block being defined
    with reference with the right and bottom edges rather than the left and top edges.
    The iterator provides the data in batches (i.e., a number of blocks in one go)
    """

    def __init__(self, img_info_lst, block_size:int, batch_size:int):
        """

        :param img_info_lst:
        :param block_size:
        :param batch_size:

        """
        self.img_info_lst = img_info_lst
        self.n_imgs = len(img_info_lst)
        self.block_size = block_size
        self.batch_size = batch_size
        self.img_info = dict()
        self.define_overlaps()
        pprint.pprint(self.img_info)

        self.remain_x_block = False
        self.n_x_blocks = math.floor(self.overlap_width / block_size)
        if (self.n_x_blocks * block_size) < self.overlap_width:
            self.n_x_blocks += 1
            self.remain_x_block = True

        self.remain_y_block = False
        self.n_y_blocks = math.floor(self.overlap_height / block_size)
        if (self.n_y_blocks * block_size) < self.overlap_height:
            self.n_y_blocks += 1
            self.remain_y_block = True

        self.n_blocks = self.n_x_blocks * self.n_y_blocks
        print("N Blocks:  {}".format(self.n_blocks))

        self.n_batches = math.floor(self.n_blocks/batch_size)
        self.remain_batch_blocks = False
        self.n_remain_batch_blocks = self.n_blocks - (self.n_batches * batch_size)
        if self.n_remain_batch_blocks > 0:
            self.n_batches += 1
            self.remain_batch_blocks = True
        print("N Batches:  {}".format(self.n_batches))

        self.c_blocks = 0
        self.c_x_blocks = -1
        self.c_y_blocks = 0

        self.c_batch = 0

        self.total_n_bands = 0
        for img in self.img_info_lst:
            self.img_info[img.name]["n_bands"] = len(img.bands)
            self.total_n_bands  += len(img.bands)
            self.img_info[img.name]["dataset"] = gdal.Open(
                img.file_name, gdal.GA_ReadOnly
            )
            if self.img_info[img.name]["dataset"] is None:
                raise rsgislib.RSGISPyException(
                    "Could not open image file: {}".format(img.file_name)
                )
            self.img_info[img.name]["bands"] = dict()
            for band in img.bands:
                self.img_info[img.name]["bands"][band] = self.img_info[img.name][
                    "dataset"
                ].GetRasterBand(band)
                if self.img_info[img.name]["bands"][band] is None:
                    raise rsgislib.RSGISPyException(
                        "Could not open band {} within image {}".format(
                            band, img.file_name
                        )
                    )

        self.img_batch_arr = numpy.zeros((batch_size, self.total_n_bands, block_size, block_size), dtype=float)

        # Output image variables
        self.out_imgs_info = None

    def find_pxl_subset_bbox(self, spat_img_bbox, pxl_res, spat_sub_bbox):
        """

        :param spat_img_bbox:
        :param pxl_res:
        :param spat_sub_bbox:
        :return:

        """
        min_x_diff = spat_sub_bbox[0] - spat_img_bbox[0]
        max_y_diff = spat_img_bbox[3] - spat_sub_bbox[3]

        min_x_pxls = math.floor((min_x_diff / pxl_res[0]) + 0.5)
        max_y_pxls = math.floor((max_y_diff / pxl_res[1]) + 0.5)

        sub_width = math.floor(
            ((spat_sub_bbox[1] - spat_sub_bbox[0]) / pxl_res[0]) + 0.5
        )
        sub_height = math.floor(
            ((spat_sub_bbox[3] - spat_sub_bbox[2]) / pxl_res[1]) + 0.5
        )

        max_x_pxls = min_x_pxls + sub_width
        min_y_pxls = max_y_pxls + sub_height

        return [min_x_pxls, max_x_pxls, min_y_pxls, max_y_pxls]

    def define_overlaps(self):
        """
        An internal function used to calculate the amount of overlap between
        the input images.

        """
        import rsgislib.tools.geometrytools

        bboxes = []
        self.img_epsg = None
        self.img_pxl_res = None
        for img in self.img_info_lst:
            self.img_info[img.name] = dict()
            self.img_info[img.name]["res"] = rsgislib.imageutils.get_img_res(
                img.file_name, abs_vals=True
            )
            self.img_info[img.name]["bbox"] = rsgislib.imageutils.get_img_bbox(
                img.file_name
            )
            self.img_info[img.name]["pxl_size"] = rsgislib.imageutils.get_img_size(
                img.file_name
            )
            self.img_info[img.name][
                "epsg"
            ] = rsgislib.imageutils.get_epsg_proj_from_img(img.file_name)
            if self.img_info[img.name]["epsg"] is None:
                raise rsgislib.RSGISPyException(
                    "The input image ({}) does not have a "
                    "defined projection - please correct.".format(img.file_name)
                )
            if self.img_epsg is None:
                self.img_epsg = self.img_info[img.name]["epsg"]
            else:
                if self.img_epsg != self.img_info[img.name]["epsg"]:
                    raise rsgislib.RSGISPyException(
                        "The input images have different input "
                        "projections: EPSG:{} != EPSG:{}".format(
                            self.img_epsg, self.img_info[img.name]["epsg"]
                        )
                    )

            if self.img_pxl_res is None:
                self.img_pxl_res = self.img_info[img.name]["res"]
            else:
                if (self.img_pxl_res[0] != self.img_info[img.name]["res"][0]) and (
                    self.img_pxl_res[1] != self.img_info[img.name]["res"][1]
                ):
                    raise rsgislib.RSGISPyException(
                        "The input images have different "
                        "input image resolutions: {} != {}".format(
                            self.img_pxl_res, self.img_info[img.name]["res"]
                        )
                    )

            bboxes.append(self.img_info[img.name]["bbox"])

        self.bbox_intersect = rsgislib.tools.geometrytools.bboxes_intersection(bboxes)

        self.overlap_width = 0
        self.overlap_height = 0

        for img in self.img_info_lst:
            self.img_info[img.name]["pxl_bbox"] = self.find_pxl_subset_bbox(
                self.img_info[img.name]["bbox"],
                self.img_info[img.name]["res"],
                self.bbox_intersect,
            )
            if (self.overlap_width == 0) and (self.overlap_height == 0):
                self.overlap_width = (
                    self.img_info[img.name]["pxl_bbox"][1]
                    - self.img_info[img.name]["pxl_bbox"][0]
                )
                self.overlap_height = (
                    self.img_info[img.name]["pxl_bbox"][2]
                    - self.img_info[img.name]["pxl_bbox"][3]
                )
            else:
                overlap_width_tmp = (
                    self.img_info[img.name]["pxl_bbox"][1]
                    - self.img_info[img.name]["pxl_bbox"][0]
                )
                overlap_height_tmp = (
                    self.img_info[img.name]["pxl_bbox"][2]
                    - self.img_info[img.name]["pxl_bbox"][3]
                )
                if self.overlap_width != overlap_width_tmp:
                    raise rsgislib.RSGISPyException("The image widths do not match.")
                if self.overlap_height != overlap_height_tmp:
                    raise rsgislib.RSGISPyException("The image height do not match.")

    def reset_iter(self):
        """
        Reset the variables used for the iterator.

        """
        self.c_batch = 0
        self.c_blocks = 0
        self.c_x_blocks = -1
        self.c_y_blocks = 0

    def __iter__(self):
        """
        Returns an instances of itself.
        """
        return self

    def __len__(self):
        """
        A function which returns the length of the iterator (i.e., number of batches).

        :return: number of batches.

        """
        return self.n_batches

    def __next__(self):
        """
        The key function for the iterator used to read the image blocks.

        :return: x_blocks, y_blocks, img_data (np_arr variable for the image data).

        """
        if self.c_batch < self.n_batches:
            n_blocks_in_batch = self.batch_size
            if (self.c_batch == self.n_batches-1) and self.remain_batch_blocks:
                n_blocks_in_batch =  self.n_remain_batch_blocks
                self.img_batch_arr = numpy.zeros((self.n_remain_batch_blocks, self.total_n_bands, self.block_size, self.block_size), dtype=float)

            out_img_data_lst = []
            x_blocks_lst = []
            y_blocks_lst = []
            for n in range(n_blocks_in_batch):
                if self.c_blocks < self.n_blocks:
                    if self.c_x_blocks < (self.n_x_blocks - 1):
                        self.c_x_blocks += 1
                    else:
                        self.c_x_blocks = 0
                        self.c_y_blocks += 1
                    self.c_blocks += 1

                    out_img_data = dict()

                    if self.c_y_blocks == (self.n_y_blocks - 1):
                        if self.c_x_blocks == (self.n_x_blocks - 1):
                            # X,Y End Case
                            for img in self.img_info_lst:
                                out_img_data[img.name] = dict()
                                out_img_data[img.name]["pxl_x"] = (
                                    self.img_info[img.name]["pxl_bbox"][1] - self.block_size
                                )
                                out_img_data[img.name]["pxl_y"] = (
                                    self.img_info[img.name]["pxl_bbox"][2] - self.block_size
                                )
                        else:
                            # Y End Case
                            for img in self.img_info_lst:
                                out_img_data[img.name] = dict()
                                out_img_data[img.name]["pxl_x"] = self.img_info[img.name][
                                    "pxl_bbox"
                                ][0] + (self.block_size * self.c_x_blocks)
                                out_img_data[img.name]["pxl_y"] = (
                                    self.img_info[img.name]["pxl_bbox"][2] - self.block_size
                                )
                    elif self.c_x_blocks == (self.n_x_blocks - 1):
                        # X End Case.
                        for img in self.img_info_lst:
                            out_img_data[img.name] = dict()
                            out_img_data[img.name]["pxl_x"] = (
                                self.img_info[img.name]["pxl_bbox"][1] - self.block_size
                            )
                            out_img_data[img.name]["pxl_y"] = self.img_info[img.name][
                                "pxl_bbox"
                            ][3] + (self.block_size * self.c_y_blocks)
                    else:
                        # Not End Case
                        for img in self.img_info_lst:
                            out_img_data[img.name] = dict()
                            out_img_data[img.name]["pxl_x"] = self.img_info[img.name][
                                "pxl_bbox"
                            ][0] + (self.block_size * self.c_x_blocks)
                            out_img_data[img.name]["pxl_y"] = self.img_info[img.name][
                                "pxl_bbox"
                            ][3] + (self.block_size * self.c_y_blocks)

                    for img in self.img_info_lst:
                        for band in img.bands:
                            self.img_info[img.name]["bands"][band].ReadAsArray(
                                xoff=out_img_data[img.name]["pxl_x"],
                                yoff=out_img_data[img.name]["pxl_y"],
                                win_xsize=self.block_size,
                                win_ysize=self.block_size,
                                buf_obj=self.img_batch_arr[n][band - 1],
                            )
                    out_img_data_lst.append(out_img_data)
                    x_blocks_lst.append(self.c_x_blocks)
                    y_blocks_lst.append(self.c_y_blocks)

            self.c_batch += 1
            return (x_blocks_lst, y_blocks_lst, out_img_data_lst, self.img_batch_arr)
        raise StopIteration

    def create_output_imgs(self, out_imgs_info):
        """
        A function to create the output image(s) if required during processing.

        :param out_imgs_info: a list of rsgislib.imageutils.OutImageInfo
                              objects specifying the output image(s).

        """
        out_wkt_str = rsgislib.tools.projection.get_wkt_from_epsg_code(
            int(self.img_epsg)
        )
        if out_wkt_str is None:
            raise rsgislib.RSGISPyException(
                "Did not have a projection string for the EPSG code: {}".format(
                    self.img_epsg
                )
            )
        if math.fabs(self.img_pxl_res[0]) != math.fabs(self.img_pxl_res[1]):
            raise rsgislib.RSGISPyException(
                "Currently limited to only outputting images with square pixels."
            )
        out_img_res = math.fabs(self.img_pxl_res[0])
        self.out_imgs_info = out_imgs_info
        self.out_imgs_objs = dict()
        for img in out_imgs_info:
            rsgislib.imageutils.create_blank_img_from_bbox(
                self.bbox_intersect,
                out_wkt_str,
                img.file_name,
                out_img_res,
                img.no_data_val,
                img.nbands,
                img.gdalformat,
                img.datatype,
                snap_to_grid=False,
            )

            self.out_imgs_objs[img.name] = dict()
            self.out_imgs_objs[img.name]["dataset"] = gdal.Open(
                img.file_name, gdal.GA_Update
            )
            if self.out_imgs_objs[img.name]["dataset"] is None:
                raise rsgislib.RSGISPyException(
                    "Could not open image file: {}".format(img.file_name)
                )
            self.out_imgs_objs[img.name]["nbands"] = img.nbands
            self.out_imgs_objs[img.name]["bands"] = dict()
            for band in range(img.nbands):
                self.out_imgs_objs[img.name]["bands"][band + 1] = self.out_imgs_objs[
                    img.name
                ]["dataset"].GetRasterBand(band + 1)
                if self.out_imgs_objs[img.name]["bands"][band + 1] is None:
                    raise rsgislib.RSGISPyException(
                        "Could not open band {} within image {}".format(
                            band + 1, img.file_name
                        )
                    )

    def write_sgl_block_to_image(self, x_block, y_block, img_block):
        """
        A function to write the processed blocks to an output image.

        :param x_block: The X index of the block being written.
        :param y_block: The Y index of the block being written.
        :param img_block: A dict of output image block data. Keys must match
                           the name of the images. Note. data block data returned
                           must have shape (nbands, xsize, ysize)

        """
        if y_block == (self.n_y_blocks - 1):
            if x_block == (self.n_x_blocks - 1):
                # X,Y End Case
                pxl_x = self.overlap_width - self.block_size
                pxl_y = self.overlap_height - self.block_size
            else:
                # Y End Case
                pxl_x = self.block_size * x_block
                pxl_y = self.overlap_height - self.block_size
        elif x_block == (self.n_x_blocks - 1):
            # X End Case.
            pxl_x = self.overlap_width - self.block_size
            pxl_y = self.block_size * y_block
        else:
            # Not End Case
            pxl_x = self.block_size * x_block
            pxl_y = self.block_size * y_block

        for img in img_block:
            if img in self.out_imgs_objs:
                block_shp = img_block[img].shape
                if block_shp[0] != self.out_imgs_objs[img]["nbands"]:
                    raise rsgislib.RSGISPyException(
                        "The number of image bands in the output file "
                        "and returned data block do not match "
                        "(block:{}; image:{})".format(
                            block_shp[0], self.out_imgs_objs[img]["nbands"]
                        )
                    )
                if (block_shp[1] != self.block_size) and (
                    block_shp[2] != self.block_size
                ):
                    raise rsgislib.RSGISPyException(
                        "The block size is either not square or the "
                        "same size as the size expected (block: {} x {}; "
                        "parameterised block: {}).".format(
                            block_shp[1], block_shp[2], self.block_size
                        )
                    )
                for band in range(block_shp[0]):
                    self.out_imgs_objs[img]["bands"][band + 1].WriteArray(
                        img_block[img][band], pxl_x, pxl_y
                    )

    def write_blocks_to_image(self, x_blocks, y_blocks, out_img_blocks):
        """
        A function which iterates through and writes a set of blocks
        to the input image.

        :param x_blocks: array of X block indexes for the blocks being written
        :param y_blocks: array of Y block indexes for the blocks being written
        :param img_blocks: A dict of output image blocks data. Keys must match
                           the name of the images. Note. data block data returned
                           must have shape (block, nbands, xsize, ysize)

        """
        for out_img in out_img_blocks:
            for x_block, y_block, img_block in zip(x_blocks, y_blocks, out_img_blocks[out_img]):
                self.write_sgl_block_to_image(x_block, y_block, {out_img:img_block})