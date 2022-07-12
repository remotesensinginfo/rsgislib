#!/usr/bin/env python
"""
The tools.imagetools module contains functions for doing simple non-spatial
image processing.
"""
from typing import List
import os
import shutil
import tqdm
import rsgislib.tools.filetools
import rsgislib.tools.utils


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
