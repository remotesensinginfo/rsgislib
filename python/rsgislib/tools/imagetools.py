#!/usr/bin/env python
"""
The tools.imagetools module contains functions for doing simple non-spatial
image processing.
"""
import os
import shutil
from typing import List

import tqdm

import rsgislib.tools.filetools
import rsgislib.tools.utils


class ImageTimeCluster(object):
    """
    A class which supports the split_photos_by_time function.
    """

    def __init__(self, cluster_id=None, init_time=None):
        """

        :param cluster_id: unique id for the cluster
        :param init_time: is the initial time of the first image

        """
        self.cluster_id = cluster_id
        self.start_time = init_time
        self.end_time = init_time
        self.images = list()

    def within_cluster(self, date_time_obj, time_split_secs=60):
        date_time_in_cluster = False
        start_diff = self.start_time - date_time_obj
        if abs(start_diff.total_seconds()) < time_split_secs:
            date_time_in_cluster = True

        if not date_time_in_cluster:
            end_diff = self.end_time - date_time_obj
            if abs(end_diff.total_seconds()) < time_split_secs:
                date_time_in_cluster = True
        return date_time_in_cluster

    def add_image_to_cluster(self, image_file, date_time_obj):
        self.images.append(image_file)
        if date_time_obj < self.start_time:
            self.start_time = date_time_obj
        elif date_time_obj > self.end_time:
            self.end_time = date_time_obj

    def does_cluster_overlap(self, cluster_obj, time_split_secs=60):
        date_time_in_cluster = False
        time_diff = self.start_time - cluster_obj.start_time
        if abs(time_diff.total_seconds()) < time_split_secs:
            date_time_in_cluster = True

        if not date_time_in_cluster:
            time_diff = self.end_time - cluster_obj.end_time
            if abs(time_diff.total_seconds()) < time_split_secs:
                date_time_in_cluster = True

        if not date_time_in_cluster:
            time_diff = self.start_time - cluster_obj.end_time
            if abs(time_diff.total_seconds()) < time_split_secs:
                date_time_in_cluster = True

        if not date_time_in_cluster:
            time_diff = self.end_time - cluster_obj.start_time
            if abs(time_diff.total_seconds()) < time_split_secs:
                date_time_in_cluster = True

        if not date_time_in_cluster:
            if (cluster_obj.start_time > self.start_time) and (
                cluster_obj.start_time < self.end_time
            ):
                date_time_in_cluster = True

        if not date_time_in_cluster:
            if (cluster_obj.end_time > self.start_time) and (
                cluster_obj.end_time < self.end_time
            ):
                date_time_in_cluster = True

        return date_time_in_cluster

    def merge_cluster(self, cluster_obj):
        self.images = self.images + cluster_obj.images
        if cluster_obj.start_time < self.start_time:
            self.start_time = cluster_obj.start_time
        elif cluster_obj.end_time > self.end_time:
            self.end_time = cluster_obj.end_time

    def __str__(self):
        return "{}: {} - {} = {}".format(
            self.cluster_id, self.start_time, self.end_time, len(self.images)
        )

    def __repr__(self):
        return "{}: {} - {} = {}".format(
            self.cluster_id, self.start_time, self.end_time, len(self.images)
        )


def animate_img_set(
    input_imgs: List[str],
    output_img: str,
    fps: int = 1,
    resize: bool = False,
    out_x_size: int = 720,
    out_y_size: int = None,
    tmp_dir: str = "./tmp",
):
    """
    A function which uses the imageio module to animate a set input images to
    create an animated GIF video file. This is intended to be used to a set of
    images which have been exported (e.g., from the mapping module).

    The input images can be resampled to a new size but this requires the
    PIL module to be installed.

    :param input_imgs: list of input image paths
    :param output_img: output image file path.
    :param fps: the number of frames per second. Higher values will result in
                faster video (i.e., less time per frame). Default: 1.
    :param resize: boolean specifying whether to resize the input images.
    :param out_x_size: the size of the output image (if resizing) for the x axis.
                       Default: 720
    :param out_y_size: the size of the output image (if resizing) for the y axis.
                       If None then the size is calculated from the input image to
                       keep the proportions the same. Default: None
    :param tmp_dir: a temp path for the resized images to be written to if resizing
                    the input images.

    """
    import imageio.v2

    uid_str = rsgislib.tools.utils.uid_generator()
    lcl_tmp_dir = os.path.join(tmp_dir, f"tmp_{uid_str}")

    if resize:
        from PIL import Image

        if not os.path.exists(tmp_dir):
            os.mkdir(tmp_dir)
        if not os.path.exists(lcl_tmp_dir):
            os.mkdir(lcl_tmp_dir)
        ani_input_imgs = list()
        print("Resizing Input Images:")
        for in_img in tqdm.tqdm(input_imgs):
            basename = rsgislib.tools.filetools.get_file_basename(in_img)
            out_resize_img = os.path.join(lcl_tmp_dir, f"{basename}_resize.png")
            lcl_img_obj = Image.open(in_img)
            if out_y_size is None:
                in_x_size, in_y_size = lcl_img_obj.size
                img_xy_ratio = in_y_size / in_x_size
                out_y_size = int(out_x_size * img_xy_ratio)
            lcl_img_resize_obj = lcl_img_obj.resize((out_x_size, out_y_size))
            lcl_img_resize_obj.save(out_resize_img)
            ani_input_imgs.append(out_resize_img)
    else:
        ani_input_imgs = input_imgs

    print("Create animated file:")
    with imageio.v2.get_writer(output_img, mode="i", fps=fps) as writer:
        for in_img in tqdm.tqdm(ani_input_imgs):
            image = imageio.v2.imread(in_img)
            writer.append_data(image)

    if resize:
        shutil.rmtree(lcl_tmp_dir)


def list_exif_info(input_img):
    """
    A function which lists the exif information from an input image
    using the PIL module.

    :param input_img: the path to the input image

    """
    from PIL import Image
    from PIL.ExifTags import TAGS

    pil_image = Image.open(input_img)
    pil_exif_data = pil_image.getexif()

    for tag_id in pil_exif_data:
        # get the tag name, instead of human unreadable tag id
        tag = TAGS.get(tag_id, tag_id)
        data = pil_exif_data.get(tag_id)
        # decode bytes
        if isinstance(data, bytes):
            data = data.decode()
        print(f"{tag:25}: {data}")
    pil_image.close()


def split_photos_by_time(
    input_imgs,
    output_base_dir,
    time_split_secs=60,
    date_time_tag="DateTime",
    date_time_format="%Y:%m:%d %H:%M:%S",
    out_date_time_format="%Y%m%d_%H%M%S",
):
    """
    A function which splits a set of images (e.g., from drone surveys) by time.
    Where photos taken within the specified time window will be clustered together
    (default: 60 seconds). The photos are then copied into a new directory based on
    the start time of the cluster. Note, photos are copied and not moved from their
    current location.

    :param input_imgs: a list of input images (e.g., glob.glob("*.jpg"))
    :param output_base_dir: the output base directory where the output directories
                            will be created.
    :param time_split_secs: The time difference in seconds for a new cluster to be
                            created.
    :param date_time_tag: the EXIF tag for the date / time of the acquisition.
                          Default is 'DataTime' but this might be different for
                          different cameras and therefore you might need to use the
                          list_exif_info function to find the correct field.
    :param date_time_format: The python datetime.strptime format string for the
                             datetime provided in the EXIF metadata.
                             Default: %Y:%m:%d %H:%M:%S
    :param out_date_time_format: The python datetime.strftime format string for the
                                 used to create the output directory for the files.
                                 Note, this must be unique. Default: %Y%m%d_%H%M%S

    """

    import datetime

    import tqdm
    from PIL import Image
    from PIL.ExifTags import TAGS

    img_dt_lut = dict()
    for img_file in input_imgs:
        pil_image = Image.open(img_file)
        pil_exif_data = pil_image.getexif()

        for tag_id in pil_exif_data:
            # get the tag name, instead of human unreadable tag id
            tag = TAGS.get(tag_id, tag_id)
            data = pil_exif_data.get(tag_id)
            # decode bytes
            if isinstance(data, bytes):
                data = data.decode()
            if tag == date_time_tag:
                img_dt_lut[img_file] = data

    img_clusters = list()
    next_cluster_id = 0
    first = True
    for img_file in img_dt_lut:
        date_time_obj = datetime.datetime.strptime(
            img_dt_lut[img_file], date_time_format
        )

        if first:
            img_cluster_obj = ImageTimeCluster(next_cluster_id, date_time_obj)
            img_cluster_obj.images.append(img_file)
            img_clusters.append(img_cluster_obj)
            next_cluster_id += 1
            first = False
        else:
            for img_cluster in img_clusters:
                if img_cluster.within_cluster(date_time_obj, time_split_secs):
                    img_cluster.add_image_to_cluster(img_file, date_time_obj)
                    break
                else:
                    img_cluster_obj = ImageTimeCluster(next_cluster_id, date_time_obj)
                    img_cluster_obj.images.append(img_file)
                    img_clusters.append(img_cluster_obj)
                    next_cluster_id += 1
                    break

        merged_cluster = False
        for img_cluster_out in img_clusters:
            for img_cluster_in in img_clusters:
                if img_cluster_out.cluster_id != img_cluster_in.cluster_id:
                    cluster_overlap = img_cluster_out.does_cluster_overlap(
                        img_cluster_in
                    )
                    if cluster_overlap:
                        merged_cluster = True
                        img_cluster_out.merge_cluster(img_cluster_in)
                        img_clusters.remove(img_cluster_in)
                        break
            if merged_cluster:
                break

    for img_cluster in img_clusters:
        print("Copying Files for: {}".format(img_cluster))
        out_dir_name = img_cluster.start_time.strftime(out_date_time_format)
        out_dir_path = os.path.join(output_base_dir, out_dir_name)
        print("Output Directory: {}".format(out_dir_path))
        if not os.path.exists(out_dir_path):
            os.mkdir(out_dir_path)
        for img in tqdm.tqdm(img_cluster.images):
            shutil.copy(img, out_dir_path)
