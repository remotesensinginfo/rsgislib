from numpy.distutils.core import setup, Extension

imagecalc_module = Extension(name='imagecalc._imagecalc', 
                sources=['src/imagecalc.cpp'],
                include_dirs=['../src/cmds', '../src'],
                library_dirs=['../src'],
                libraries=['rsgis_cmds'])

# do the setup
setup( name = 'RSGISLib',
        version = '0.1',
        description = 'Python interface onto RSGISLib',
        author = 'Sam Gillingham',
        author_email = 'gillingham.sam@gmail.com',
        packages = ['rsgislib', 'rsgislib.imagecalc'],
        ext_package = 'rsgislib',
        ext_modules = [imagecalc_module])

