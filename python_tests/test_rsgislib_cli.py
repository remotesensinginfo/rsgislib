import os
import subprocess
import pprint
import glob
from shutil import copy2

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
RASTERGIS_DATA_DIR = os.path.join(DATA_DIR, "rastergis")
IMGUTILS_DATA_DIR = os.path.join(DATA_DIR, "imageutils")

def test_rsgis_config_version():
    rtn_info = subprocess.run(["rsgis-config", "--version"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("5" in rtn_info.stdout)

def test_rsgis_config_prefix():
    rtn_info = subprocess.run(["rsgis-config", "--prefix"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgis_config_libs():
    rtn_info = subprocess.run(["rsgis-config", "--libs"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgis_config_cflags():
    rtn_info = subprocess.run(["rsgis-config", "--cflags"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgis_config_ldflags():
    rtn_info = subprocess.run(["rsgis-config", "--ldflags"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgis_config_libdir():
    rtn_info = subprocess.run(["rsgis-config", "--libdir"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgis_config_includes():
    rtn_info = subprocess.run(["rsgis-config", "--includes"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgis_config_fail():
    rtn_info = subprocess.run(["rsgis-config", "--hello"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" == rtn_info.stdout)

def test_rsgisapplycmd_run(tmp_path):
    in_dir = os.path.join(tmp_path, "in")
    os.mkdir(in_dir)
    out_dir = os.path.join(tmp_path, "out")
    os.mkdir(out_dir)

    img1_in = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    img1_out = os.path.join(in_dir, "sen2_20210527_aber_vldmsk.kea")
    copy2(img1_in, img1_out)
    img2_in = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    img2_out = os.path.join(in_dir, "sen2_20210527_aber_subset_vldmsk.kea")
    copy2(img2_in, img2_out)

    rtn_info = subprocess.run(["rsgisapplycmd.py", "--indir", in_dir, "--outdir", out_dir, "--inext", "kea", "--outext", "tif", "--nameapp", "test", "--cmd", "gdal_translate -of GTIFF {{ input }} {{ output }}", "--ncores", "2"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and (len(glob.glob(os.path.join(out_dir, "*.tif")))==2)

def test_rsgisapplycmd_print(tmp_path):
    in_dir = os.path.join(tmp_path, "in")
    os.mkdir(in_dir)
    out_dir = os.path.join(tmp_path, "out")
    os.mkdir(out_dir)

    img1_in = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    img1_out = os.path.join(in_dir, "sen2_20210527_aber_vldmsk.kea")
    copy2(img1_in, img1_out)
    img2_in = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    img2_out = os.path.join(in_dir, "sen2_20210527_aber_subset_vldmsk.kea")
    copy2(img2_in, img2_out)

    rtn_info = subprocess.run(["rsgisapplycmd.py", "--indir", in_dir, "--outdir", out_dir, "--inext", "kea", "--outext", "tif", "--nameapp", "test", "--cmd", "gdal_translate -of GTIFF {{ input }} {{ output }}", "--printcmds"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("gdal_translate" in rtn_info.stdout)


def test_rsgisbatchconvert2tif(tmp_path):
    in_dir = os.path.join(tmp_path, "in")
    os.mkdir(in_dir)
    out_dir = os.path.join(tmp_path, "out")
    os.mkdir(out_dir)

    img1_in = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    img1_out = os.path.join(in_dir, "sen2_20210527_aber_vldmsk.kea")
    copy2(img1_in, img1_out)
    img2_in = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    img2_out = os.path.join(in_dir, "sen2_20210527_aber_subset_vldmsk.kea")
    copy2(img2_in, img2_out)

    in_srch = os.path.join(in_dir, "*.kea")
    rtn_info = subprocess.run(["rsgisbatchconvert2tif.py", "-i", in_srch, "-o", out_dir], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ( len(glob.glob(os.path.join(out_dir, "*.tif"))) == 2)

def test_rsgisbatchconvert2tif_chk(tmp_path):
    in_dir = os.path.join(tmp_path, "in")
    os.mkdir(in_dir)
    out_dir = os.path.join(tmp_path, "out")
    os.mkdir(out_dir)

    img1_in = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    img1_out = os.path.join(in_dir, "sen2_20210527_aber_vldmsk.kea")
    copy2(img1_in, img1_out)
    img2_in = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    img2_out = os.path.join(in_dir, "sen2_20210527_aber_subset_vldmsk.kea")
    copy2(img2_in, img2_out)

    in_srch = os.path.join(in_dir, "*.kea")
    rtn_info = subprocess.run(["rsgisbatchconvert2tif.py", "-i", in_srch, "-o", out_dir, "--chkimgs"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ( len(glob.glob(os.path.join(out_dir, "*.tif"))) == 2)

def test_rsgisbuildimglut(tmp_path):
    in_dir = os.path.join(tmp_path, "in")
    os.mkdir(in_dir)

    img1_in = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    img1_out = os.path.join(in_dir, "sen2_20210527_aber_vldmsk.kea")
    copy2(img1_in, img1_out)
    img2_in = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    img2_out = os.path.join(in_dir, "sen2_20210527_aber_subset_vldmsk.kea")
    copy2(img2_in, img2_out)

    in_srch = os.path.join(in_dir, "*.kea")
    out_lut_file = os.path.join(tmp_path, "lut.gpkg")
    rtn_info = subprocess.run(["rsgisbuildimglut.py", "-i", in_srch, "-o", out_lut_file, "--veclyr", "test_lut", "--vecformat", "GPKG"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and os.path.exists(out_lut_file)

def test_rsgisbuildimglut_wgs84(tmp_path):
    in_dir = os.path.join(tmp_path, "in")
    os.mkdir(in_dir)

    img1_in = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    img1_out = os.path.join(in_dir, "sen2_20210527_aber_vldmsk.kea")
    copy2(img1_in, img1_out)
    img2_in = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    img2_out = os.path.join(in_dir, "sen2_20210527_aber_subset_vldmsk.kea")
    copy2(img2_in, img2_out)

    in_srch = os.path.join(in_dir, "*.kea")
    out_lut_file = os.path.join(tmp_path, "lut.gpkg")
    rtn_info = subprocess.run(["rsgisbuildimglut.py", "-i", in_srch, "-o", out_lut_file, "--veclyr", "test_lut", "--vecformat", "GPKG", "--wgs84"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and os.path.exists(out_lut_file)

def test_rsgiscalcimgstats_cont(tmp_path):
    img_ref = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    img = os.path.join(tmp_path, "sen2_20210527_aber.kea")
    copy2(img_ref, img)

    rtn_info = subprocess.run(["rsgiscalcimgstats.py", "-i", img, "-n", "0.0"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)

def test_rsgiscalcimgstats_therm(tmp_path):
    img_ref = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    img = os.path.join(tmp_path, "sen2_20210527_aber_vldmsk.kea")
    copy2(img_ref, img)

    rtn_info = subprocess.run(["rsgiscalcimgstats.py", "-i", img, "-n", "0.0", "-t"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)

def test_rsgiscalcimgstats_therm_tif(tmp_path):
    img_ref = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.tif")
    img = os.path.join(tmp_path, "sen2_20210527_aber_vldmsk.tif")
    copy2(img_ref, img)

    rtn_info = subprocess.run(["rsgiscalcimgstats.py", "-i", img, "-n", "0.0", "-t"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)


def test_rsgischkgdalfile_sgl_img(tmp_path):
    img_ref = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    img = os.path.join(tmp_path, "sen2_20210527_aber_vldmsk.kea")
    copy2(img_ref, img)

    rtn_info = subprocess.run(["rsgischkgdalfile.py", "-i", img, "--nbands", "1", "--epsg", "27700", "--chkproj", "--readimg"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)

def test_rsgischkgdalfile_multi_img(tmp_path):
    img_ref = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    img = os.path.join(tmp_path, "sen2_20210527_aber_vldmsk.kea")
    copy2(img_ref, img)

    img_ref = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    img = os.path.join(tmp_path, "sen2_20210527_aber_subset_vldmsk.kea")
    copy2(img_ref, img)

    in_srch = os.path.join(tmp_path, "*.kea")
    rtn_info = subprocess.run(["rsgischkgdalfile.py", "-i", in_srch, "--nbands", "1", "--epsg", "27700", "--chkproj", "--readimg"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)

def test_rsgischkgdalfile_sgl_vec(tmp_path):
    vec_ref = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec = os.path.join(tmp_path, "aber_osgb_multi_polys.geojson")
    copy2(vec_ref, vec)

    rtn_info = subprocess.run(["rsgischkgdalfile.py", "-i", vec, "--vec", "--epsg", "27700", "--chkproj"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)

def test_rsgischkgdalfile_multi_vec(tmp_path):
    vec_ref = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec = os.path.join(tmp_path, "aber_osgb_multi_polys.geojson")
    copy2(vec_ref, vec)

    vec_ref = os.path.join(DATA_DIR, "aber_osgb_single_poly_hole.geojson")
    vec = os.path.join(tmp_path, "aber_osgb_single_poly_hole.geojson")
    copy2(vec_ref, vec)

    in_srch = os.path.join(tmp_path, "*.geojson")
    rtn_info = subprocess.run(["rsgischkgdalfile.py", "-i", in_srch, "--vec", "--epsg", "27700", "--chkproj"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)

def test_rsgiscopybandnames(tmp_path):
    img_ref = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    img = os.path.join(tmp_path, "sen2_20210527_aber.kea")
    copy2(img_ref, img)

    img_bandnames_ref = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")

    rtn_info = subprocess.run(["rsgiscopybandnames.py", "-r", img_bandnames_ref, "-o", img], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)

def test_rsgisfilehash_sha1():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisfilehash.py", "-i", img, "-o", "SHA1"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)and ("" != rtn_info.stdout)

def test_rsgisfilehash_sha224():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisfilehash.py", "-i", img, "-o", "SHA224"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)and ("" != rtn_info.stdout)

def test_rsgisfilehash_sha256():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisfilehash.py", "-i", img, "-o", "SHA256"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)and ("" != rtn_info.stdout)

def test_rsgisfilehash_sha384():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisfilehash.py", "-i", img, "-o", "SHA384"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)and ("" != rtn_info.stdout)

def test_rsgisfilehash_sha512():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisfilehash.py", "-i", img, "-o", "SHA512"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)and ("" != rtn_info.stdout)

def test_rsgisfilehash_md5():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisfilehash.py", "-i", img, "-o", "MD5"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)and ("" != rtn_info.stdout)

def test_rsgisfilehash_blake2b():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisfilehash.py", "-i", img, "-o", "Blake2B"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)and ("" != rtn_info.stdout)

def test_rsgisfilehash_black2s():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisfilehash.py", "-i", img, "-o", "Blake2S"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)and ("" != rtn_info.stdout)

def test_rsgisfilehash_sha3_224():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisfilehash.py", "-i", img, "-o", "SHA3_224"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)and ("" != rtn_info.stdout)

def test_rsgisfilehash_sha3_256():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisfilehash.py", "-i", img, "-o", "SHA3_256"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)and ("" != rtn_info.stdout)

def test_rsgisfilehash_sha3_234():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisfilehash.py", "-i", img, "-o", "SHA3_384"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)and ("" != rtn_info.stdout)

def test_rsgisfilehash_sha3_512():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisfilehash.py", "-i", img, "-o", "SHA3_512"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)and ("" != rtn_info.stdout)

def test_rsgisfilelut_copy(tmp_path):
    import rsgislib.imageutils.imagelut
    import glob

    input_imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    vec_lut_file = os.path.join(tmp_path, "test_lut.gpkg")
    vec_lut_lyr = "test_lut"
    rsgislib.imageutils.imagelut.create_img_extent_lut(
        input_imgs,
        vec_lut_file,
        vec_lut_lyr,
        "GPKG",
        ignore_none_imgs=False,
        out_proj_wgs84=False,
        overwrite_lut_file=False,
        )

    roi_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    roi_lyr = "aber_osgb_single_poly"

    rtn_info = subprocess.run(["rsgisfilelut.py", "-i", vec_lut_file, "--lutlyr", vec_lut_lyr, "--roifile", roi_file, "--roilyr", roi_lyr, "--dest", tmp_path, "--copy"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)


def test_rsgisfilelut_targz(tmp_path):
    import rsgislib.imageutils.imagelut
    import glob

    input_imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    vec_lut_file = os.path.join(tmp_path, "test_lut.gpkg")
    vec_lut_lyr = "test_lut"
    rsgislib.imageutils.imagelut.create_img_extent_lut(
        input_imgs,
        vec_lut_file,
        vec_lut_lyr,
        "GPKG",
        ignore_none_imgs=False,
        out_proj_wgs84=False,
        overwrite_lut_file=False,
        )

    roi_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    roi_lyr = "aber_osgb_single_poly"

    out_file = os.path.join(tmp_path, "out.tar.gz")

    rtn_info = subprocess.run(["rsgisfilelut.py", "-i", vec_lut_file, "--lutlyr", vec_lut_lyr, "--roifile", roi_file, "--roilyr", roi_lyr, "--dest", out_file, "--targz"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)


def test_rsgisimg2kmz(tmp_path):
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_file = os.path.join(tmp_path, "sen2_20210527_aber.kmz")

    rtn_info = subprocess.run(["rsgisimg2kmz.py", "-i", img, "-o", out_file, "-b", "3,2,1"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and (os.path.exists(out_file))

def test_rsgisimg2webtiles(tmp_path):
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisimg2webtiles.py", "-i", img, "-o", tmp_path, "-b", "3,2,1"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0)

def test_rsgisimginfo():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisimginfo.py", "-i", img], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgisimginfo_rat():
    img = os.path.join(RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_attref.kea")

    rtn_info = subprocess.run(["rsgisimginfo.py", "-i", img], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgisproj_img_epsg():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisproj.py", "--image", img, "-o", "EPSG"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgisproj_img_wkt():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisproj.py", "--image", img, "-o", "WKT"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgisproj_img_wktpretty():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisproj.py", "--image", img, "-o", "WKTPretty"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgisproj_img_proj4():
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    rtn_info = subprocess.run(["rsgisproj.py", "--image", img, "-o", "PROJ4"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgisproj_utm_36n_epsg():
    rtn_info = subprocess.run(["rsgisproj.py", "--utm", "36N", "-o", "EPSG"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgisproj_utm_01n_epsg():
    rtn_info = subprocess.run(["rsgisproj.py", "--utm", "01N", "-o", "EPSG"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgisproj_utm_55s_epsg():
    rtn_info = subprocess.run(["rsgisproj.py", "--utm", "55S", "-o", "EPSG"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgisproj_wkt_epsg():
    file_path = os.path.join(IMGUTILS_DATA_DIR, "utm30n.wkt")

    rtn_info = subprocess.run(["rsgisproj.py", "--wktfile", file_path, "-o", "EPSG"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgisproj_epsg_wkt():
    rtn_info = subprocess.run(["rsgisproj.py", "--epsg", "27700", "-o", "WKT"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgistranslate2tif(tmp_path):
    img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_img = os.path.join(tmp_path, "out_img.tif")
    rtn_info = subprocess.run(["rsgistranslate2tif.py", "-i", img, "-o", out_img], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and os.path.exists(out_img)


def test_rsgisvectools_lyrs():
    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    rtn_info = subprocess.run(["rsgisvectools.py", "--vecfile", vec_file, "--lyrs"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgisvectools_cols():
    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    rtn_info = subprocess.run(["rsgisvectools.py", "--vecfile", vec_file, "--veclyr", "aber_osgb_multi_polys", "--cols"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)

def test_rsgisvectools_nfeats():
    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    rtn_info = subprocess.run(["rsgisvectools.py", "--vecfile", vec_file, "--veclyr", "aber_osgb_multi_polys", "--nfeats"], capture_output=True, text=True, check=True)
    pprint.pprint(rtn_info)
    assert (rtn_info.returncode == 0) and ("" != rtn_info.stdout)