###############################################################################
#
# CMake build scripts for RSGISLib
# 
# Created 2012/02/07 by Peter Bunting
#
# These scripts were initial based on those used for libLAS (http://liblas.org/)
# and then edited for SPDLib (http://www.spdlib.org) before being edited again
# for RSGISLib.
#
# History
# 2010/12/14 - Created by Peter Bunting for SPDLib
# 2012/02/07 - Edited by Peter Bunting for RSGISLib
#
###############################################################################

# CMake Min Version
cmake_minimum_required(VERSION 3.12)

###############################################################################
# Set Project name and version
project (RSGISLIB)

set (PROJECT_BINARY_DIR bin)
set (PROJECT_LIBRARY_DIR lib)
set (PROJECT_DOC_DIR doc)
set (PROJECT_SOURCE_DIR src)
set (PROJECT_TOOLS_DIR tools)
set (PROJECT_DATA_DIR share)

# The version number.
set (RSGISLIB_VERSION_MAJOR 5)
set (RSGISLIB_VERSION_MINOR 1)
set (RSGISLIB_VERSION_PATCH 9)
set (RSGISLIB_VERSION "${RSGISLIB_VERSION_MAJOR}.${RSGISLIB_VERSION_MINOR}.${RSGISLIB_VERSION_PATCH}")
set (RSGISLIB_PACKAGE_VERSION "${RSGISLIB_VERSION_MAJOR}.${RSGISLIB_VERSION_MINOR}.${RSGISLIB_VERSION_PATCH}")
set (RSGISLIB_PACKAGE_STRING "RSGISLib ${RSGISLIB_VERSION_MAJOR}.${RSGISLIB_VERSION_MINOR}.${RSGISLIB_VERSION_PATCH}")
set (RSGISLIB_PACKAGE_BUGREPORT "rsgislib-support@googlegroups.com")
set (RSGISLIB_PACKAGE "RSGISLib")
set (RSGISLIB_COPYRIGHT_YEAR 2021)

set (RSGISLIB_CALIBRATION_VERSION_MAJOR 1)
set (RSGISLIB_CALIBRATION_VERSION_MINOR 1)
set (RSGISLIB_CALIBRATION_VERSION_PATCH 0)
set (RSGISLIB_CALIBRATION_VERSION "${RSGISLIB_CALIBRATION_VERSION_MAJOR}.${RSGISLIB_CALIBRATION_VERSION_MINOR}.${RSGISLIB_CALIBRATION_VERSION_PATCH}")

set (RSGISLIB_CLASSIFY_VERSION_MAJOR 1)
set (RSGISLIB_CLASSIFY_VERSION_MINOR 1)
set (RSGISLIB_CLASSIFY_VERSION_PATCH 0)
set (RSGISLIB_CLASSIFY_VERSION "${RSGISLIB_CLASSIFY_VERSION_MAJOR}.${RSGISLIB_CLASSIFY_VERSION_MINOR}.${RSGISLIB_CLASSIFY_VERSION_PATCH}")

set (RSGISLIB_COMMONS_VERSION_MAJOR 1)
set (RSGISLIB_COMMONS_VERSION_MINOR 3)
set (RSGISLIB_COMMONS_VERSION_PATCH 0)
set (RSGISLIB_COMMONS_VERSION "${RSGISLIB_COMMONS_VERSION_MAJOR}.${RSGISLIB_COMMONS_VERSION_MINOR}.${RSGISLIB_COMMONS_VERSION_PATCH}")

set (RSGISLIB_DATASTRUCT_VERSION_MAJOR 1)
set (RSGISLIB_DATASTRUCT_VERSION_MINOR 1)
set (RSGISLIB_DATASTRUCT_VERSION_PATCH 0)
set (RSGISLIB_DATASTRUCT_VERSION "${RSGISLIB_DATASTRUCT_VERSION_MAJOR}.${RSGISLIB_DATASTRUCT_VERSION_MINOR}.${RSGISLIB_DATASTRUCT_VERSION_PATCH}")

set (RSGISLIB_FILTERING_VERSION_MAJOR 1)
set (RSGISLIB_FILTERING_VERSION_MINOR 0)
set (RSGISLIB_FILTERING_VERSION_PATCH 0)
set (RSGISLIB_FILTERING_VERSION "${RSGISLIB_FILTERING_VERSION_MAJOR}.${RSGISLIB_FILTERING_VERSION_MINOR}.${RSGISLIB_FILTERING_VERSION_PATCH}")

set (RSGISLIB_IMG_VERSION_MAJOR 3)
set (RSGISLIB_IMG_VERSION_MINOR 0)
set (RSGISLIB_IMG_VERSION_PATCH 0)
set (RSGISLIB_IMG_VERSION "${RSGISLIB_IMG_VERSION_MAJOR}.${RSGISLIB_IMG_VERSION_MINOR}.${RSGISLIB_IMG_VERSION_PATCH}")

set (RSGISLIB_MATHS_VERSION_MAJOR 1)
set (RSGISLIB_MATHS_VERSION_MINOR 1)
set (RSGISLIB_MATHS_VERSION_PATCH 0)
set (RSGISLIB_MATHS_VERSION "${RSGISLIB_MATHS_VERSION_MAJOR}.${RSGISLIB_MATHS_VERSION_MINOR}.${RSGISLIB_MATHS_VERSION_PATCH}")

set (RSGISLIB_RASTERGIS_VERSION_MAJOR 2)
set (RSGISLIB_RASTERGIS_VERSION_MINOR 0)
set (RSGISLIB_RASTERGIS_VERSION_PATCH 0)
set (RSGISLIB_RASTERGIS_VERSION "${RSGISLIB_RASTERGIS_VERSION_MAJOR}.${RSGISLIB_RASTERGIS_VERSION_MINOR}.${RSGISLIB_RASTERGIS_VERSION_PATCH}")

set (RSGISLIB_REGISTRATION_VERSION_MAJOR 1)
set (RSGISLIB_REGISTRATION_VERSION_MINOR 2)
set (RSGISLIB_REGISTRATION_VERSION_PATCH 0)
set (RSGISLIB_REGISTRATION_VERSION "${RSGISLIB_REGISTRATION_VERSION_MAJOR}.${RSGISLIB_REGISTRATION_VERSION_MINOR}.${RSGISLIB_REGISTRATION_VERSION_PATCH}")

set (RSGISLIB_SEGMENTATION_VERSION_MAJOR 1)
set (RSGISLIB_SEGMENTATION_VERSION_MINOR 0)
set (RSGISLIB_SEGMENTATION_VERSION_PATCH 0)
set (RSGISLIB_SEGMENTATION_VERSION "${RSGISLIB_SEGMENTATION_VERSION_MAJOR}.${RSGISLIB_SEGMENTATION_VERSION_MINOR}.${RSGISLIB_SEGMENTATION_VERSION_PATCH}")

set (RSGISLIB_UTILS_VERSION_MAJOR 1)
set (RSGISLIB_UTILS_VERSION_MINOR 1)
set (RSGISLIB_UTILS_VERSION_PATCH 0)
set (RSGISLIB_UTILS_VERSION "${RSGISLIB_UTILS_VERSION_MAJOR}.${RSGISLIB_UTILS_VERSION_MINOR}.${RSGISLIB_UTILS_VERSION_PATCH}")

set (RSGISLIB_VECTOR_VERSION_MAJOR 2)
set (RSGISLIB_VECTOR_VERSION_MINOR 2)
set (RSGISLIB_VECTOR_VERSION_PATCH 0)
set (RSGISLIB_VECTOR_VERSION "${RSGISLIB_VECTOR_VERSION_MAJOR}.${RSGISLIB_VECTOR_VERSION_MINOR}.${RSGISLIB_VECTOR_VERSION_PATCH}")

set (RSGISLIB_CMDSINTERFACE_VERSION_MAJOR 1)
set (RSGISLIB_CMDSINTERFACE_VERSION_MINOR 1)
set (RSGISLIB_CMDSINTERFACE_VERSION_PATCH 0)
set (RSGISLIB_CMDSINTERFACE_VERSION "${RSGISLIB_CMDSINTERFACE_VERSION_MAJOR}.${RSGISLIB_CMDSINTERFACE_VERSION_MINOR}.${RSGISLIB_CMDSINTERFACE_VERSION_PATCH}")

# Name of C++ library
set(RSGISLIB_CALIBRATION_LIB_NAME rsgis_calib)
set(RSGISLIB_CLASSIFY_LIB_NAME rsgis_classify)
set(RSGISLIB_COMMONS_LIB_NAME rsgis_commons)
set(RSGISLIB_DATASTRUCT_LIB_NAME rsgis_datastruct)
set(RSGISLIB_FILTERING_LIB_NAME rsgis_filter)
set(RSGISLIB_IMG_LIB_NAME rsgis_img)
set(RSGISLIB_MATHS_LIB_NAME rsgis_maths)
set(RSGISLIB_RASTERGIS_LIB_NAME rsgis_rastergis)
set(RSGISLIB_REGISTRATION_LIB_NAME rsgis_registration)
set(RSGISLIB_SEGMENTATION_LIB_NAME rsgis_segmentation)
set(RSGISLIB_UTILS_LIB_NAME rsgis_utils)
set(RSGISLIB_VECTOR_LIB_NAME rsgis_vec)
set(RSGISLIB_CMDSINTERFACE_LIB_NAME rsgis_cmds)

option (BUILD_SHARED_LIBS "Build with shared library" ON)
set(RSGISLIB_WITH_UTILTIES TRUE CACHE BOOL "Choose if RSGISLib utilities should be built")
set(RSGISLIB_WITH_DATA TRUE CACHE BOOL "Choose if RSGISLib datasets should be installed.")

set(BOOST_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for Boost")
set(BOOST_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for Boost")

set(GSL_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for GSL")
set(GSL_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for GSL")

set(GDAL_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for GDAL")
set(GDAL_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for GDAL")

set(MUPARSER_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for MU Parser")
set(MUPARSER_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for MU Parser")

set(HDF5_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for HDF5")
set(HDF5_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for HDF5")

set(KEA_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for KEA")
set(KEA_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for KEA")

option(RSGIS_PYTHON "Build Python bindings" ON)

###############################################################################

###############################################################################
IF(NOT CMAKE_BUILD_TYPE)
  #SET(CMAKE_BUILD_TYPE "DEBUG")
  SET(CMAKE_BUILD_TYPE "RELEASE")
  #SET(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
  #SET(CMAKE_BUILD_TYPE "MINSIZEREL")
ENDIF()

set(CMAKE_COLOR_MAKEFILE ON)

# Allow advanced users to generate Makefiles printing detailed commands
mark_as_advanced(CMAKE_VERBOSE_MAKEFILE)

# Path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ./cmake/modules/)
###############################################################################

###############################################################################
# Setup configure file
configure_file ( "${PROJECT_SOURCE_DIR}/rsgis-config.h.in" "${CMAKE_BINARY_DIR}/${PROJECT_SOURCE_DIR}/common/rsgis-config.h" )
###############################################################################

###############################################################################
# Documentation
file(READ "doc/index.txt" README )
file(WRITE "README.txt" "${README}")
###############################################################################

###############################################################################
# Platform and compiler specific settings
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    if (MSVC)
        if (MSVC80 OR MSVC90 OR MSVC10 OR MSVC14)
            add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            add_definitions(-D_CRT_NONSTDC_NO_WARNING)
            add_definitions(-D_SCL_SECURE_NO_WARNINGS)
        endif()
        
        # Set to /W2 - way too many warnings otherwise
        if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
            string(REGEX REPLACE "/W[0-4]" "/W2"
                   CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W2")
        endif()

        # Generate dot-user file with user-specific settings for Visual Studio project
        set(MSVC_ENVIRONMENT_PATH "" CACHE STRING
            "Custom PATH for Environment property in Visual Studio project configuration")
        mark_as_advanced(MSVC_ENVIRONMENT_PATH)
        set(VCPROJ_USER_ENVIRONMENT_PATH_DEBUG "${ENVIRONMENT_PATH}")
        mark_as_advanced(VCPROJ_USER_ENVIRONMENT_PATH_DEBUG)
        
        # Use cmake's MSVC_VERSION to work out the name of the Boost/CGAL DLL's 
        # to use (they use a different numbering scheme from cmake)
        set(MSVC_STRING_1500 "vc90")
        set(MSVC_STRING_1600 "vc100")
        set(MSVC_STRING_1700 "vc110")
        set(MSVC_STRING_1800 "vc120")
        set(MSVC_STRING_1900 "vc140")
        set(MSVC_STRING ${MSVC_STRING_${MSVC_VERSION}})
        message(STATUS "Using MSVC String " ${MSVC_STRING})
        
        # get GSL functions
        add_definitions(-DGSL_DLL)
        # HDF5 functions/classes
        add_definitions(-DH5_BUILT_AS_DYNAMIC_LIB)
        
    endif()

else()
  # Recommended C++ compilation flags
  # -Weffc++
  set(RSGISLIB_COMMON_CXX_FLAGS
	"-Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wredundant-decls -Wno-long-long")
	#"-pedantic -ansi -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wfloat-equal -Wredundant-decls -Wno-long-long")

  if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ${RSGISLIB_COMMON_CXX_FLAGS}")
    if (CMAKE_COMPILER_IS_GNUCXX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()

  elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER}" MATCHES "clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RSGISLIB_COMMON_CXX_FLAGS}")
  endif()
endif(WIN32)

if(APPLE)
    # use, i.e. don't skip the full RPATH for the build tree
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
    
    # when building, don't use the install RPATH already
    # (but later on when installing)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
    
    # the RPATH to be used when installing
    SET(CMAKE_INSTALL_RPATH "")
    
    # don't add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

    SET(CMAKE_MACOSX_RPATH TRUE)
endif(APPLE)

if (APPLE)
  set(SO_EXT dylib)
 # set(CMAKE_FIND_FRAMEWORK "LAST")
elseif(WIN32)
  set(SO_EXT dll)
else()
  set(SO_EXT so)
endif(APPLE)
###############################################################################

###############################################################################
# Check the required libraries are present


include_directories(${BOOST_INCLUDE_DIR})
if (MSVC)
    # Under Windows, the boost libs are strangely named - use glob to find them
    file(GLOB BOOST_FS_LIB ${BOOST_LIB_PATH}/boost_filesystem.lib )
    if(NOT BOOST_FS_LIB)
        message(FATAL_ERROR "Could not find boost_filesystem lib")
    endif()
    file(GLOB BOOST_SYS_LIB ${BOOST_LIB_PATH}/boost_system.lib )
    if(NOT BOOST_SYS_LIB)
        message(FATAL_ERROR "Could not find boost_system lib")
    endif()
    file(GLOB BOOST_DT_LIB ${BOOST_LIB_PATH}/boost_date_time.lib )
    if(NOT BOOST_DT_LIB)
        message(FATAL_ERROR "Could not find boost_date_time lib")
    endif()
    message(STATUS "Found boost libraries " ${BOOST_FS_LIB} ${BOOST_SYS_LIB} ${BOOST_DT_LIB})
    set(BOOST_LIBRARIES -LIBPATH:${BOOST_LIB_PATH} ${BOOST_FS_LIB} ${BOOST_SYS_LIB} ${BOOST_DT_LIB})
else()
    set(BOOST_LIBRARIES -L${BOOST_LIB_PATH} -lboost_filesystem -lboost_system -lboost_date_time)
endif(MSVC)

include_directories(${GDAL_INCLUDE_DIR})
if (MSVC)
  set(GDAL_LIBRARIES -LIBPATH:${GDAL_LIB_PATH} gdal.lib)
else()
  set(GDAL_LIBRARIES -L${GDAL_LIB_PATH} -lgdal)
endif(MSVC)

include_directories(${GSL_INCLUDE_DIR})
if (MSVC)
    set(GSL_LIBRARIES -LIBPATH:${GSL_LIB_PATH} gsl.lib gslcblas.lib)
else()
    set(GSL_LIBRARIES -L${GSL_LIB_PATH} -lgsl -lgslcblas)
endif(MSVC)

include_directories(${MUPARSER_INCLUDE_DIR})
if (MSVC)
    set(MUPARSER_LIBRARIES -LIBPATH:${MUPARSER_LIB_PATH} muparser.lib)
else()
    set(MUPARSER_LIBRARIES -L${MUPARSER_LIB_PATH} -lmuparser)
endif(MSVC)

include_directories(${HDF5_INCLUDE_DIR})
if (MSVC)
    set(HDF5_LIBRARIES -LIBPATH:${HDF5_LIB_PATH} hdf5.lib hdf5_hl.lib hdf5_cpp.lib)
else()
    set(HDF5_LIBRARIES -L${HDF5_LIB_PATH} -lhdf5 -lhdf5_hl -lhdf5_cpp)
endif(MSVC)

include_directories(${KEA_INCLUDE_DIR})
if (MSVC)
    set(KEA_LIBRARIES -LIBPATH:${KEA_LIB_PATH} libkea.lib)
else()
    set(KEA_LIBRARIES -L${KEA_LIB_PATH} -lkea)
endif(MSVC)
###############################################################################

###############################################################################
# Build libraries
include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory ("${PROJECT_SOURCE_DIR}")
###############################################################################
# Python
if( RSGIS_PYTHON )
    message(STATUS "Doing python bindings")
    add_subdirectory ("python")
endif(RSGIS_PYTHON)

###############################################################################
# Build executables
if (RSGISLIB_WITH_UTILTIES)
	add_executable(rsgisflip ${PROJECT_TOOLS_DIR}/rsgisflip.cpp)
	target_link_libraries (rsgisflip)
    if (MSVC)
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgis-config.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgis-config.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisapplycmd.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisapplycmd.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisbatchconvert2cog.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbatchconvert2cog.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisbatchconvert2tif.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbatchconvert2tif.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisbuildimglut.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbuildimglut.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscalcimgstats.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscalcimgstats.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgischkgdalfile.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgischkgdalfile.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscopybandnames.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscopybandnames.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisfilehash.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisfilehash.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisfilelut.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisfilelut.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisimg2kmz.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimg2kmz.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisimg2webtiles.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimg2webtiles.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisimginfo.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimginfo.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisproj.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisproj.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisshepsegment.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisshepsegment.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgistranslate2cog.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgistranslate2cog.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgistranslate2tif.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgistranslate2tif.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisuserpassfile.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisuserpassfile.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgisvectools.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisvectools.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgiswgs84imgres.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiswgs84imgres.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscreateptsvec.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscreateptsvec.bat" )
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscreatevrts.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscreatevrts.bat" )
    else()
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgis-config.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgis-config" )
    endif(MSVC)
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscalcimgstats.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscalcimgstats.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscopybandnames.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscopybandnames.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisimg2kmz.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimg2kmz.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisimg2webtiles.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimg2webtiles.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisvectools.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisvectools.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisimginfo.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimginfo.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisbuildimglut.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbuildimglut.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisfilelut.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisfilelut.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisproj.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisproj.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisapplycmd.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisapplycmd.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisbatchconvert2tif.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbatchconvert2tif.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgistranslate2tif.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgistranslate2tif.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisfilehash.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisfilehash.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgischkgdalfile.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgischkgdalfile.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisuserpassfile.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisuserpassfile.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisbatchconvert2cog.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbatchconvert2cog.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgistranslate2cog.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgistranslate2cog.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisshepsegment.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisshepsegment.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgiswgs84imgres.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiswgs84imgres.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscreateptsvec.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscreateptsvec.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscreatevrts.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscreatevrts.py" )
endif(RSGISLIB_WITH_UTILTIES)

###############################################################################

###############################################################################
# Installation
if (RSGISLIB_WITH_UTILTIES)
	install (TARGETS rsgisflip DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    if (MSVC)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgis-config.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisapplycmd.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbatchconvert2cog.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbatchconvert2tif.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbuildimglut.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscalcimgstats.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgischkgdalfile.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscopybandnames.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisfilehash.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisfilelut.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimg2kmz.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimg2webtiles.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimginfo.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisproj.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisshepsegment.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgistranslate2cog.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgistranslate2tif.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisuserpassfile.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisvectools.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiswgs84imgres.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscreateptsvec.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscreatevrts.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    else()
        install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgis-config" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    endif(MSVC)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscalcimgstats.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscopybandnames.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimg2kmz.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimg2webtiles.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisvectools.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimginfo.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbuildimglut.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisfilelut.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisproj.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisapplycmd.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbatchconvert2tif.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgistranslate2tif.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisfilehash.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgischkgdalfile.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisuserpassfile.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbatchconvert2cog.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgistranslate2cog.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisshepsegment.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiswgs84imgres.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscreateptsvec.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (PROGRAMS "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscreatevrts.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif(RSGISLIB_WITH_UTILTIES)

if (RSGISLIB_WITH_DATA)
	install (FILES "${PROJECT_DATA_DIR}/utm_zone_boundaries_lyrs_north.gpkg" DESTINATION share/rsgislib PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
	install (FILES "${PROJECT_DATA_DIR}/utm_zone_boundaries_lyrs_south.gpkg" DESTINATION share/rsgislib PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
endif(RSGISLIB_WITH_DATA)
###############################################################################

###############################################################################
# Create Distribution
SET(CPACK_SOURCE_GENERATOR "TGZ;ZIP;TBZ2")
SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")

set(CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${RSGISLIB_VERSION_MAJOR}.${RSGISLIB_VERSION_MINOR}.${RSGISLIB_VERSION_PATCH}")

# Set files to ignore
list(APPEND CPACK_SOURCE_IGNORE_FILES "_CPack_Packages")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".gz")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".bz2")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".zip")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".svn")
list(APPEND CPACK_SOURCE_IGNORE_FILES "README")
list(APPEND CPACK_SOURCE_IGNORE_FILES "HOWTORELEASE.txt")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CMakeCache.txt")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CPackConfig.cmake")
list(APPEND CPACK_SOURCE_IGNORE_FILES "schemas")

include(CPack)

add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
###############################################################################
