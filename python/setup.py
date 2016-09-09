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

imagecalibration_module = Extension(name='imagecalibration._imagecalibration', 
                sources=['src/imagecalibration.cpp'],
                include_dirs=['../src/cmds', '../src'],
                library_dirs=['../src'],
                libraries=['rsgis_cmds'])

rastergis_module = Extension(name='rastergis._rastergis',
                sources=['src/rastergis.cpp'],
                include_dirs=['../src/cmds', '../src'],
                library_dirs=['../src'],
                libraries=['rsgis_cmds'])
                
zonalstats_module = Extension(name='zonalstats._zonalstats',
                sources=['src/zonalstats.cpp'],
                include_dirs=['../src/cmds', '../src'],
                library_dirs=['../src'],
                libraries=['rsgis_cmds'])
                
imageregistration_module = Extension(name='imageregistration._imageregistration',
                sources=['src/imageregistration.cpp'],
                include_dirs=['../src/cmds', '../src'],
                library_dirs=['../src'],
                libraries=['rsgis_cmds'])

vectorutils_module = Extension(name='vectorutils._vectorutils',
                sources=['src/vectorutils.cpp'],
                include_dirs=['../src/cmds', '../src'],
                library_dirs=['../src'],
                libraries=['rsgis_cmds'])

imagefilter_module = Extension(name='imagefilter._imagefilter',
                sources=['src/imagefilter.cpp'],
                include_dirs=['../src/cmds', '../src'],
                library_dirs=['../src'],
                libraries=['rsgis_cmds'])
                
classification_module = Extension(name='classification._classification',
                sources=['src/classification.cpp'],
                include_dirs=['../src/cmds', '../src'],
                library_dirs=['../src'],
                libraries=['rsgis_cmds'])
                
elevation_module = Extension(name='elevation._elevation',
                sources=['src/elevation.cpp'],
                include_dirs=['../src/cmds', '../src'],
                library_dirs=['../src'],
                libraries=['rsgis_cmds'])
                


# do the setup
setup( name = 'RSGISLib',
        version = '0.1',
        description = 'Python interface onto RSGISLib',
        author = 'Pete Bunting, Dan Clewley and Sam Gillingham',
        author_email = 'petebunting@mac.com, daniel.clewley@gmail.com, gillingham.sam@gmail.com',
        packages = ['rsgislib', 'rsgislib.imagecalc', 'rsgislib.imageutils', 'rsgislib.segmentation', 'rsgislib.imagecalibration', 'rsgislib.rastergis', 'rsgislib.zonalstats', 'rsgislib.imageregistration', 'rsgislib.vectorutils', 'rsgislib.imagefilter', 'rsgislib.classification', 'rsgislib.elevation', 'rsgislib.tools'],
        ext_package = 'rsgislib',
        ext_modules = [imagecalc_module, imageutils_module, segmentation_module, imagecalibration_module, rastergis_module, zonalstats_module, imageregistration_module, vectorutils_module, imagefilter_module, classification_module, elevation_module])

