#!/usr/bin/env python

import rsgislib
import rsgislib.imageutils
import pprint
import math
import numpy
import osgeo.gdal as gdal


class StdImgBlockIter:

    """
    A class to read and write images in blocks where all blocks are all identical in size.
    To deal with image boundaries, where block do not divide into the the image size, the
    outside blocks will be overlapped with the previous block being defined with reference
    with the right and bottom edges rather than the left and top edges.
    """

    def __init__(self, img_info_lst, block_size):
        """

        :param img_info_lst:
        :param block_size:

        """
        self.img_info_lst = img_info_lst
        self.n_imgs = len(img_info_lst)
        self.block_size = block_size
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
        
        self.c_blocks = 0
        self.c_x_blocks = -1
        self.c_y_blocks = 0

        for img in self.img_info_lst:
            self.img_info[img.name]['n_bands'] = len(img.bands)
            self.img_info[img.name]['np_arr'] = numpy.zeros((len(img.bands), block_size, block_size), dtype=float)
            self.img_info[img.name]['dataset'] = gdal.Open(img.fileName, gdal.GA_ReadOnly)
            if self.img_info[img.name]['dataset'] is None:
                raise Exception("Could not open image file: {}".format(img.fileName))
            self.img_info[img.name]['bands'] = dict()
            for band in img.bands:
                self.img_info[img.name]['bands'][band] = self.img_info[img.name]['dataset'].GetRasterBand(band)
                if self.img_info[img.name]['bands'][band] is None:
                    raise Exception("Could not open band {} within image {}".format(band, img.fileName))
        
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

        sub_width = math.floor(((spat_sub_bbox[1] - spat_sub_bbox[0]) / pxl_res[0]) + 0.5)
        sub_height = math.floor(((spat_sub_bbox[3] - spat_sub_bbox[2]) / pxl_res[1]) + 0.5)

        max_x_pxls = min_x_pxls + sub_width
        min_y_pxls = max_y_pxls + sub_height

        return [min_x_pxls, max_x_pxls, min_y_pxls, max_y_pxls]
    
    def reset_iter(self):
        """
        Reset the variables used for the iterator.

        """
        self.c_blocks = 0
        self.c_x_blocks = -1
        self.c_y_blocks = 0
    
    def define_overlaps(self):
        """
        An internal function used to calculate the amount of overlap between the input images.

        """
        import rsgislib.tools.geometrytools
        bboxes = []
        self.img_epsg = None
        self.img_pxl_res = None
        for img in self.img_info_lst:
            self.img_info[img.name] = dict()
            self.img_info[img.name]['res'] = rsgislib.imageutils.getImageRes(img.fileName, abs_vals=True)
            self.img_info[img.name]['bbox'] = rsgislib.imageutils.getImageBBOX(img.fileName)
            self.img_info[img.name]['pxl_size'] = rsgislib.imageutils.getImageSize(img.fileName)
            self.img_info[img.name]['epsg'] = rsgislib.imageutils.getEPSGProjFromImage(img.fileName)
            if self.img_info[img.name]['epsg'] is None:
                raise Exception("The input image ({}) does not have a defined projection - please correct.".format(img.fileName))
            if self.img_epsg is None:
                self.img_epsg = self.img_info[img.name]['epsg']
            else:
                if self.img_epsg != self.img_info[img.name]['epsg']:
                    raise Exception("The input images have different input projections: EPSG:{} != EPSG:{}".format(self.img_epsg, self.img_info[img.name]['epsg']))
            
            if self.img_pxl_res is None:
                self.img_pxl_res = self.img_info[img.name]['res']
            else:
                if (self.img_pxl_res[0] != self.img_info[img.name]['res'][0]) and (self.img_pxl_res[1] != self.img_info[img.name]['res'][1]):
                    raise Exception("The input images have different input image resolutions: {} != {}".format(self.img_pxl_res, self.img_info[img.name]['res']))
            
            bboxes.append(self.img_info[img.name]['bbox'])
        
        self.bbox_intersect = rsgislib.tools.geometrytools.bboxes_intersection(bboxes)
        
        self.overlap_width = 0
        self.overlap_height = 0
        
        for img in self.img_info_lst:
            self.img_info[img.name]['pxl_bbox'] = self.find_pxl_subset_bbox(self.img_info[img.name]['bbox'], self.img_info[img.name]['res'], self.img_info[img.name]['pxl_size'], self.bbox_intersect)
            if (self.overlap_width == 0) and (self.overlap_height == 0):
                self.overlap_width = self.img_info[img.name]['pxl_bbox'][1] - self.img_info[img.name]['pxl_bbox'][0]
                self.overlap_height = self.img_info[img.name]['pxl_bbox'][2] - self.img_info[img.name]['pxl_bbox'][3]
            else:
                overlap_width_tmp = self.img_info[img.name]['pxl_bbox'][1] - self.img_info[img.name]['pxl_bbox'][0]
                overlap_height_tmp = self.img_info[img.name]['pxl_bbox'][2] - self.img_info[img.name]['pxl_bbox'][3]
                if self.overlap_width != overlap_width_tmp:
                    raise Exception("The image widths do not match.")
                if self.overlap_height != overlap_height_tmp:
                    raise Exception("The image height do not match.")
            
    def __iter__(self):
        """
        Returns an instances of itself.
        """
        return self

    def __next__(self):
        """
        The key function for the iterator used to read the image blocks.

        :return: x_block, y_block, img_data (dict with image name as key; each image has
                 a np_arr variable for the image data.

        """
        if self.c_blocks < self.n_blocks:
            if self.c_x_blocks < (self.n_x_blocks-1):
                self.c_x_blocks += 1
            else:
                self.c_x_blocks = 0
                self.c_y_blocks += 1
            self.c_blocks += 1
            
            out_img_data = dict()
            
            if self.c_y_blocks == (self.n_y_blocks-1):
                if self.c_x_blocks == (self.n_x_blocks-1):
                    # X,Y End Case
                    for img in self.img_info_lst:
                        out_img_data[img.name] = dict()
                        out_img_data[img.name]['pxl_x'] = self.img_info[img.name]['pxl_bbox'][1] - self.block_size
                        out_img_data[img.name]['pxl_y'] = self.img_info[img.name]['pxl_bbox'][2] - self.block_size
                else:
                    # Y End Case
                    for img in self.img_info_lst:
                        out_img_data[img.name] = dict()
                        out_img_data[img.name]['pxl_x'] = self.img_info[img.name]['pxl_bbox'][0] + (self.block_size * self.c_x_blocks)
                        out_img_data[img.name]['pxl_y'] = self.img_info[img.name]['pxl_bbox'][2] - self.block_size
            elif self.c_x_blocks == (self.n_x_blocks-1):
                # X End Case.
                for img in self.img_info_lst:
                    out_img_data[img.name] = dict()
                    out_img_data[img.name]['pxl_x'] = self.img_info[img.name]['pxl_bbox'][1] - self.block_size
                    out_img_data[img.name]['pxl_y'] = self.img_info[img.name]['pxl_bbox'][3] + (self.block_size * self.c_y_blocks)
            else:
                # Not End Case
                for img in self.img_info_lst:
                    out_img_data[img.name] = dict()
                    out_img_data[img.name]['pxl_x'] = self.img_info[img.name]['pxl_bbox'][0] + (self.block_size * self.c_x_blocks)
                    out_img_data[img.name]['pxl_y'] = self.img_info[img.name]['pxl_bbox'][3] + (self.block_size * self.c_y_blocks)
                    
            for img in self.img_info_lst:
                for band in img.bands:
                    self.img_info[img.name]['bands'][band].ReadAsArray(xoff=out_img_data[img.name]['pxl_x'], yoff=out_img_data[img.name]['pxl_y'], win_xsize=self.block_size, win_ysize=self.block_size, buf_obj=self.img_info[img.name]['np_arr'][band-1])
                out_img_data[img.name]['np_arr'] = self.img_info[img.name]['np_arr']
                                            
            return (self.c_x_blocks, self.c_y_blocks, out_img_data)
        raise StopIteration
        
    def __len__(self):
        """
        A function which returns the length of the iterator (i.e., number of blocks).

        :return: number of blocks.

        """
        return self.n_blocks
    
    def create_output_imgs(self, out_imgs_info):
        """
        A function to create the output image(s) if required during processing.

        :param out_imgs_info: a list of rsgislib.imageutils.OutImageInfo objects specifying the output image(s).

        """
        out_wkt_str = rsgislib.imageutils.getWKTFromEPSGCode(int(self.img_epsg))
        if out_wkt_str is None:
            raise Exception("Did not have a projection string for the EPSG code: {}".format(self.img_epsg))
        if math.fabs(self.img_pxl_res[0]) != math.fabs(self.img_pxl_res[1]):
            raise Exception("Currently limited to only outputting images with square pixels.")
        out_img_res = math.fabs(self.img_pxl_res[0])
        self.out_imgs_info = out_imgs_info
        self.out_imgs_objs = dict()
        for img in out_imgs_info:
            rsgislib.imageutils.createBlankImgFromBBOX(self.bbox_intersect, out_wkt_str, img.file_name, out_img_res, img.no_data_val, img.nbands, img.gdal_format, img.datatype, snap2grid=False)
            
            self.out_imgs_objs[img.name] = dict()
            self.out_imgs_objs[img.name]['dataset'] = gdal.Open(img.file_name, gdal.GA_Update)
            if self.out_imgs_objs[img.name]['dataset'] is None:
                raise Exception("Could not open image file: {}".format(img.file_name))
            self.out_imgs_objs[img.name]['nbands'] = img.nbands
            self.out_imgs_objs[img.name]['bands'] = dict()
            for band in range(img.nbands):
                self.out_imgs_objs[img.name]['bands'][band+1] = self.out_imgs_objs[img.name]['dataset'].GetRasterBand(band+1)
                if self.out_imgs_objs[img.name]['bands'][band+1] is None:
                    raise Exception("Could not open band {} within image {}".format(band+1, img.file_name))
            
    
    def write_block_to_image(self, x_block, y_block, img_blocks):
        """
        A function to write the processed blocks to an output image.

        :param x_block: The X index of the block being written.
        :param y_block: The Y index of the block being written.
        :param img_blocks: A dict of output image block data. Keys must match the name of the images.
                           Note. data block data returned must have shape (nbands, xsize, ysize)

        """
        if y_block == (self.n_y_blocks-1):
            if x_block == (self.n_x_blocks-1):
                # X,Y End Case
                pxl_x = self.overlap_width - self.block_size
                pxl_y = self.overlap_height - self.block_size
            else:
                # Y End Case
                pxl_x = self.block_size * x_block
                pxl_y = self.overlap_height - self.block_size
        elif x_block == (self.n_x_blocks-1):
            # X End Case.
            pxl_x = self.overlap_width - self.block_size
            pxl_y = self.block_size * y_block
        else:
            # Not End Case
            pxl_x = self.block_size * x_block
            pxl_y = self.block_size * y_block
        
        for img in img_blocks:
            if img in self.out_imgs_objs:
                block_shp = img_blocks[img].shape
                if block_shp[0] != self.out_imgs_objs[img]['nbands']:
                    raise Exception("The number of image bands in the output file and returned data block "
                                    "do not match (block:{}; image:{})".format(block_shp[0],
                                                                               self.out_imgs_objs[img]['nbands']))
                if (block_shp[1] != self.block_size) and (block_shp[2] != self.block_size):
                    raise Exception("The block size is either not square or the same size as the "
                                    "size expected (block: {} x {}; parameterised block: {}).".format(block_shp[1],
                                                                                                      block_shp[2],
                                                                                                      self.block_size))
                for band in range(block_shp[0]):
                    self.out_imgs_objs[img]['bands'][band+1].WriteArray(img_blocks[img][band], pxl_x, pxl_y)
                

"""
import tqdm

imgBandInfo = []
imgBandInfo.append(rsgislib.imageutils.ImageBandInfo(fileName='./S2_UVD_27sept_27700_sub.kea', name='sen2a', bands=[1,2,3,4,5,6,7,8,9,10]))
imgBandInfo.append(rsgislib.imageutils.ImageBandInfo(fileName='./S2_UVD_27sept_27700_sub_roi1.kea', name='sen2a1', bands=[1,2,3,4,5,6,7,8,9,10]))
imgBandInfo.append(rsgislib.imageutils.ImageBandInfo(fileName='./S2_UVD_27sept_27700_sub_roi2.kea', name='sen2a2', bands=[1,2,3,4,5,6,7,8,9,10]))

imgOutInfo = []
imgOutInfo.append(OutImageInfo(file_name='./S2_UVD_27sept_27700_sub_ndvi.kea', name='sen2a_ndvi', nbands=1, no_data_val=-999, gdal_format='KEA', datatype=rsgislib.TYPE_32FLOAT))
imgOutInfo.append(OutImageInfo(file_name='./S2_UVD_27sept_27700_sub_ndwi.kea', name='sen2a_ndwi', nbands=1, no_data_val=-999, gdal_format='KEA', datatype=rsgislib.TYPE_32FLOAT))

img_iter = StdImgBlockIter(imgBandInfo, 64)
img_iter.create_output_imgs(imgOutInfo)
for x_block, y_block, out_img_data in tqdm.tqdm(img_iter):
    hello = 'world'
    #print("Tile: ({} x {})".format(x_block, y_block))
    #for img in out_img_data:
    #    print("Mean = {}".format(numpy.mean(out_img_data[img]['np_arr'])))
    ndvi = (out_img_data['sen2a']['np_arr'][8] - out_img_data['sen2a']['np_arr'][3]) / (out_img_data['sen2a']['np_arr'][8] + out_img_data['sen2a']['np_arr'][3])
    ndwi = (out_img_data['sen2a']['np_arr'][8] - out_img_data['sen2a']['np_arr'][9]) / (out_img_data['sen2a']['np_arr'][8] + out_img_data['sen2a']['np_arr'][9])
    out_data = {'sen2a_ndvi': numpy.expand_dims(ndvi, axis=0), 'sen2a_ndwi': numpy.expand_dims(ndwi, axis=0)}
    #print(ndvi.shape)
    img_iter.write_block_to_image(x_block, y_block, out_data)

"""
