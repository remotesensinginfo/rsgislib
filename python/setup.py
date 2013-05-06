from numpy.distutils.core import setup, Extension

imagecalc_module = Extension(name='imagecalc._imagecalc', 
                sources=['src/imagecalc.cpp'],
                include_dirs=['../src/cmds', '../src'],
                library_dirs=['../src'],
                libraries=['rsgis_cmds'])

imageutils_module = Extension(name='imageutils._imageutils', 
                sources=['src/imageutils.cpp'],
                include_dirs=['../src/cmds', '../src'],
                library_dirs=['../src'],
                libraries=['rsgis_cmds'])

segmentation_module = Extension(name='segmentation._segmentation', 
                sources=['src/segmentation.cpp'],
                include_dirs=['../src/cmds', '../src'],
                library_dirs=['../src'],
                libraries=['rsgis_cmds'])

# do the setup
setup( name = 'RSGISLib',
        version = '0.1',
        description = 'Python interface onto RSGISLib',
        author = 'Sam Gillingham',
        author_email = 'gillingham.sam@gmail.com',
        packages = ['rsgislib', 'rsgislib.imagecalc', 'rsgislib.imageutils',
                        'rsgislib.segmentation'],
        ext_package = 'rsgislib',
        ext_modules = [imagecalc_module, imageutils_module, 
                            segmentation_module])

