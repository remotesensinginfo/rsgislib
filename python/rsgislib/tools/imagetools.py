#!/usr/bin/env python
"""
The tools.imagetools module contains functions for doing simple non-spatial
image processing.
"""
import os
import shutil
from typing import List, Dict

import tqdm

import rsgislib.tools.filetools
import rsgislib.tools.utils


def animate_img_set(
    input_imgs: List[str],
    output_img: str,
    fps: int = 1,
    loop: int = 0,
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
    :param loop: How many times the GIF animation should be looped. Default: 0.
                 If 0 (default) then it will loop forever.
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
    with imageio.v2.get_writer(output_img, mode="i", fps=fps, loop=loop) as writer:
        for in_img in tqdm.tqdm(ani_input_imgs):
            image = imageio.v2.imread(in_img)
            writer.append_data(image)

    if resize:
        shutil.rmtree(lcl_tmp_dir)


def list_exif_info_pil(input_img: str):
    """
    A function which prints the exif information from an input image to the console
    using the PIL module.

    :param input_img: the path to the input image

    """
    from PIL import Image
    from PIL.ExifTags import TAGS, IFD, GPSTAGS

    pil_image = Image.open(input_img)
    pil_exif_data = pil_image.getexif()

    for ifd_id in IFD:
        print(">>>>>>>>>", ifd_id.name, "<<<<<<<<<<")
        try:
            ifd = pil_exif_data.get_ifd(ifd_id)

            if ifd_id == IFD.GPSInfo:
                resolve = GPSTAGS
            else:
                resolve = TAGS

            for k, v in ifd.items():
                tag = resolve.get(k, k)
                if tag not in ["UserComment", "PrintImageMatching", "MakerNote"]:
                    if isinstance(v, bytes):
                        v = v.decode("utf-8")
                    print(f"{tag}:\t {v}")
        except KeyError:
            pass
    pil_image.close()


def list_exif_info(input_img: str):
    """
    A function which prints the exif information from an input image to the console
    using the exiftool module.

    :param input_img: the path to the input image

    """
    import exiftool

    with exiftool.ExifToolHelper() as et:
        exif_data = et.get_metadata(input_img)

        for exif_item in exif_data:
            for exif_tag in exif_item:
                print(f"{exif_tag}:\t{exif_item[exif_tag]}")


def get_exif_info(input_img: str) -> Dict:
    """
    A function which returns a dict of the exif information from an input image
    using the exiftool module.

    :param input_img: the path to the input image
    :return: dict of exif data

    """
    import exiftool

    out_exif_data = dict()
    with exiftool.ExifToolHelper() as et:
        exif_data = et.get_metadata(input_img)

        for exif_item in exif_data:
            for exif_tag in exif_item:
                out_exif_data[exif_tag] = exif_item[exif_tag]
    return out_exif_data


def extract_images_from_pdf(input_pdf: str, output_dir: str):
    """
    A function which extracts the images from a PDF file and files them
    to a directory as PNG image files.

    Note this function requires the fitz module to be installed:

    pip install fitz

    The fitz module also requires the PyMuPDF module which is not installed by pip.

    pip install PyMuPDF

    :param input_pdf: the path to the input PDF file.
    :param output_dir: the output directory where the images will be stored.

    """
    import fitz

    doc = fitz.Document(input_pdf)

    for i in tqdm.tqdm(range(len(doc)), desc="pages"):
        for img in tqdm.tqdm(doc.get_page_images(i), desc="Page"):
            xref = img[0]
            image = doc.extract_image(xref)
            pix = fitz.Pixmap(doc, xref)
            pix.save(os.path.join(output_dir, f"p{i}_{xref}.png"))


def add_pdf_blank_pages(input_pdf: str, output_pdf: str):
    """
    A function which adds a blank page after each page within
    the existing PDF document.

    :param input_pdf: the input PDF file.
    :param output_pdf: the output PDF file.

    """
    import fitz

    pdf_doc = fitz.Document(input_pdf)
    n_pages = pdf_doc.page_count

    n_add_pages = 1
    for i in range(n_pages):
        pdf_doc.new_page(pno=i + n_add_pages)
        n_add_pages += 1

    pdf_doc.save(output_pdf)


def create_qr_code(
    output_img: str, qr_data: str, qr_code_size: int = 10, qr_code_border: int = 4
):
    """
    A function which uses the qrcode module to create a QR code image file.
    Needs the qrcode module installed (pip install qrcode)

    :param output_img: file path to the output image
    :param qr_data: the data to be encoded in the QR code (e.g., URL)
    :param qr_code_size: the size of the QR code (default: 10)
    :param qr_code_border: the border to the QA code (default: 4)

    """
    import qrcode

    qr = qrcode.QRCode(
        version=5,
        error_correction=qrcode.constants.ERROR_CORRECT_L,
        box_size=qr_code_size,
        border=qr_code_border,
    )
    qr.add_data(qr_data)
    qr.make(fit=True)

    img = qr.make_image(fill_color="black", back_color="white")
    img.save(output_img)
