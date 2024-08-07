
# For find_package (Python)
cmake_minimum_required(VERSION 3.12)

find_package (Python COMPONENTS Interpreter Development)
if(NOT Python_FOUND)
    message(FATAL_ERROR "Could not find Python. Set RSGIS_PYTHON=OFF to disable.")
endif(NOT Python_FOUND)

if(APPLE)
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

#set(PYRSGIS_ROOT_PATH "${Python_SITELIB}")
file(TO_CMAKE_PATH "${Python_SITELIB}" PYRSGIS_ROOT_PATH)
if(NOT MSVC AND NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    # user has set CMAKE_INSTALL_PREFIX - ensure we install
    # under there rather than the default Python install location.
    # BUT we want to keep the last part of the path the same as Python_SITELIB
    # (ie the lib/pythonX.X/site-packages part)
    string(REPLACE "${_Python_PREFIX}" "${CMAKE_INSTALL_PREFIX}" PYRSGIS_ROOT_PATH "${PYRSGIS_ROOT_PATH}")
endif(NOT MSVC AND NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
message(STATUS "installing Python libs to ${PYRSGIS_ROOT_PATH}")

#set(PYRSGIS_INSTALL_PATH "${PYRSGIS_ROOT_PATH}/rsgislib")
file(TO_CMAKE_PATH "${PYRSGIS_ROOT_PATH}/rsgislib" PYRSGIS_INSTALL_PATH)

include_directories(${Python_INCLUDE_DIRS})
include_directories(../src/cmds ../src)
link_directories(../src)

# copy all the .py files
install(DIRECTORY rsgislib DESTINATION "${PYRSGIS_ROOT_PATH}")

# _imagecalc
add_library(_imagecalc SHARED src/imagecalc.cpp)
if(APPLE)
    # see https://blog.tim-smith.us/2015/09/python-extension-modules-os-x/
    set_target_properties(_imagecalc PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_imagecalc ${RSGISLIB_CMDSINTERFACE_LIB_NAME}) 
else()
    target_link_libraries(_imagecalc ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES}) 
endif(APPLE)
set_target_properties(_imagecalc PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_imagecalc PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _imagecalc DESTINATION "${PYRSGIS_INSTALL_PATH}/imagecalc")

# _imageutils
add_library(_imageutils SHARED src/imageutils.cpp)
if(APPLE)
    set_target_properties(_imageutils PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_imageutils ${RSGISLIB_CMDSINTERFACE_LIB_NAME}) 
else()
    target_link_libraries(_imageutils ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES}) 
endif(APPLE)
set_target_properties(_imageutils PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_imageutils PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _imageutils DESTINATION "${PYRSGIS_INSTALL_PATH}/imageutils")

# _segmentation
add_library(_segmentation SHARED src/segmentation.cpp)
if(APPLE)
    set_target_properties(_segmentation PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_segmentation ${RSGISLIB_CMDSINTERFACE_LIB_NAME}) 
else()
    target_link_libraries(_segmentation ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES}) 
endif(APPLE)
set_target_properties(_segmentation PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_segmentation PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _segmentation DESTINATION "${PYRSGIS_INSTALL_PATH}/segmentation")

# _segmentation
add_library(_imagecalibration SHARED src/imagecalibration.cpp)
if(APPLE)
    set_target_properties(_imagecalibration PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_imagecalibration ${RSGISLIB_CMDSINTERFACE_LIB_NAME}) 
else()
    target_link_libraries(_imagecalibration ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES}) 
endif(APPLE)
set_target_properties(_imagecalibration PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_imagecalibration PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _imagecalibration DESTINATION "${PYRSGIS_INSTALL_PATH}/imagecalibration")

# _rastergis
add_library(_rastergis SHARED src/rastergis.cpp)
if(APPLE)
    # see https://blog.tim-smith.us/2015/09/python-extension-modules-os-x/
    set_target_properties(_rastergis PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_rastergis ${RSGISLIB_CMDSINTERFACE_LIB_NAME}) 
else()
    target_link_libraries(_rastergis ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES}) 
endif(APPLE)
set_target_properties(_rastergis PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_rastergis PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _rastergis DESTINATION "${PYRSGIS_INSTALL_PATH}/rastergis")

# _zonalstats
add_library(_zonalstats SHARED src/zonalstats.cpp)
if(APPLE)
    set_target_properties(_zonalstats PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_zonalstats ${RSGISLIB_CMDSINTERFACE_LIB_NAME}) 
else()
    target_link_libraries(_zonalstats ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES}) 
endif(APPLE)
set_target_properties(_zonalstats PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_zonalstats PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _zonalstats DESTINATION "${PYRSGIS_INSTALL_PATH}/zonalstats")

# _imageregistration
add_library(_imageregistration SHARED src/imageregistration.cpp)
if(APPLE)
    set_target_properties(_imageregistration PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_imageregistration ${RSGISLIB_CMDSINTERFACE_LIB_NAME}) 
else()
    target_link_libraries(_imageregistration ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES}) 
endif(APPLE)
set_target_properties(_imageregistration PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_imageregistration PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _imageregistration DESTINATION "${PYRSGIS_INSTALL_PATH}/imageregistration")

# _vectorutils
add_library(_vectorutils SHARED src/vectorutils.cpp)
if(APPLE)
    set_target_properties(_vectorutils PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_vectorutils ${RSGISLIB_CMDSINTERFACE_LIB_NAME}) 
else()
    target_link_libraries(_vectorutils ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES}) 
endif(APPLE)
set_target_properties(_vectorutils PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_vectorutils PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _vectorutils DESTINATION "${PYRSGIS_INSTALL_PATH}/vectorutils")

# _imagefilter
add_library(_imagefilter SHARED src/imagefilter.cpp)
if(APPLE)
    set_target_properties(_imagefilter PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_imagefilter ${RSGISLIB_CMDSINTERFACE_LIB_NAME}) 
else()
    target_link_libraries(_imagefilter ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES}) 
endif(APPLE)
set_target_properties(_imagefilter PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_imagefilter PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _imagefilter DESTINATION "${PYRSGIS_INSTALL_PATH}/imagefilter")

# _classification
add_library(_classification SHARED src/classification.cpp)
if(APPLE)
    set_target_properties(_classification PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_classification ${RSGISLIB_CMDSINTERFACE_LIB_NAME}) 
else()
    target_link_libraries(_classification ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES}) 
endif(APPLE)
set_target_properties(_classification PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_classification PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _classification DESTINATION "${PYRSGIS_INSTALL_PATH}/classification")

# _elevation
add_library(_elevation SHARED src/elevation.cpp)
if(APPLE)
    set_target_properties(_elevation PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_elevation ${RSGISLIB_CMDSINTERFACE_LIB_NAME}) 
else()
    target_link_libraries(_elevation ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES}) 
endif(APPLE)
set_target_properties(_elevation PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_elevation PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _elevation DESTINATION "${PYRSGIS_INSTALL_PATH}/elevation")

# _imagemorphology
add_library(_imagemorphology SHARED src/imagemorphology.cpp)
if(APPLE)
    set_target_properties(_imagemorphology PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_imagemorphology ${RSGISLIB_CMDSINTERFACE_LIB_NAME}) 
else()
    target_link_libraries(_imagemorphology ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES}) 
endif(APPLE)
set_target_properties(_imagemorphology PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_imagemorphology PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _imagemorphology DESTINATION "${PYRSGIS_INSTALL_PATH}/imagemorphology")


# _specunmixing
add_library(_specunmixing SHARED src/specunmixing.cpp)
if(APPLE)
    set_target_properties(_specunmixing PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    target_link_libraries(_specunmixing ${RSGISLIB_CMDSINTERFACE_LIB_NAME})
else()
    target_link_libraries(_specunmixing ${RSGISLIB_CMDSINTERFACE_LIB_NAME} ${Python_LIBRARIES})
endif(APPLE)
set_target_properties(_specunmixing PROPERTIES PREFIX "")
if(MSVC)
    set_target_properties(_specunmixing PROPERTIES SUFFIX ".pyd")
endif(MSVC)

install(TARGETS _specunmixing DESTINATION "${PYRSGIS_INSTALL_PATH}/imagecalc/specunmixing")

# custom install step to create .pyc files
install(CODE "execute_process(COMMAND ${Python_EXECUTABLE} -m compileall ${PYRSGIS_INSTALL_PATH})")

