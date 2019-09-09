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

###############################################################################
# Set Project name and version
project (RSGISLIB)

set (PROJECT_BINARY_DIR bin)
set (PROJECT_LIBRARY_DIR lib)
set (PROJECT_DOC_DIR doc)
set (PROJECT_SOURCE_DIR src)
set (PROJECT_TOOLS_DIR tools)

# The version number.
set (RSGISLIB_VERSION_MAJOR 3)
set (RSGISLIB_VERSION_MINOR 7)
set (RSGISLIB_VERSION_PATCH 57)
set (RSGISLIB_VERSION "${RSGISLIB_VERSION_MAJOR}.${RSGISLIB_VERSION_MINOR}.${RSGISLIB_VERSION_PATCH}")
set (RSGISLIB_PACKAGE_VERSION "${RSGISLIB_VERSION_MAJOR}.${RSGISLIB_VERSION_MINOR}.${RSGISLIB_VERSION_PATCH}")
set (RSGISLIB_PACKAGE_STRING "RSGISLib ${RSGISLIB_VERSION_MAJOR}.${RSGISLIB_VERSION_MINOR}.${RSGISLIB_VERSION_PATCH}")
set (RSGISLIB_PACKAGE_BUGREPORT "rsgislib-support@googlegroups.com")
set (RSGISLIB_PACKAGE "RSGISLib")
set (RSGISLIB_COPYRIGHT_YEAR 2014)

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

set (RSGISLIB_GEOM_VERSION_MAJOR 1)
set (RSGISLIB_GEOM_VERSION_MINOR 1)
set (RSGISLIB_GEOM_VERSION_PATCH 0)
set (RSGISLIB_GEOM_VERSION "${RSGISLIB_GEOM_VERSION_MAJOR}.${RSGISLIB_GEOM_VERSION_MINOR}.${RSGISLIB_GEOM_VERSION_PATCH}")

set (RSGISLIB_IMG_VERSION_MAJOR 3)
set (RSGISLIB_IMG_VERSION_MINOR 0)
set (RSGISLIB_IMG_VERSION_PATCH 0)
set (RSGISLIB_IMG_VERSION "${RSGISLIB_IMG_VERSION_MAJOR}.${RSGISLIB_IMG_VERSION_MINOR}.${RSGISLIB_IMG_VERSION_PATCH}")

set (RSGISLIB_MATHS_VERSION_MAJOR 1)
set (RSGISLIB_MATHS_VERSION_MINOR 1)
set (RSGISLIB_MATHS_VERSION_PATCH 0)
set (RSGISLIB_MATHS_VERSION "${RSGISLIB_MATHS_VERSION_MAJOR}.${RSGISLIB_MATHS_VERSION_MINOR}.${RSGISLIB_MATHS_VERSION_PATCH}")

set (RSGISLIB_MODELING_VERSION_MAJOR 1)
set (RSGISLIB_MODELING_VERSION_MINOR 1)
set (RSGISLIB_MODELING_VERSION_PATCH 0)
set (RSGISLIB_MODELING_VERSION "${RSGISLIB_MODELING_VERSION_MAJOR}.${RSGISLIB_MODELING_VERSION_MINOR}.${RSGISLIB_MODELING_VERSION_PATCH}")

set (RSGISLIB_RADAR_VERSION_MAJOR 1)
set (RSGISLIB_RADAR_VERSION_MINOR 1)
set (RSGISLIB_RADAR_VERSION_PATCH 1)
set (RSGISLIB_RADAR_VERSION "${RSGISLIB_RADAR_VERSION_MAJOR}.${RSGISLIB_RADAR_VERSION_MINOR}.${RSGISLIB_RADAR_VERSION_PATCH}")

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

set (RSGISLIB_HISTOCUBE_VERSION_MAJOR 1)
set (RSGISLIB_HISTOCUBE_VERSION_MINOR 0)
set (RSGISLIB_HISTOCUBE_VERSION_PATCH 0)
set (RSGISLIB_HISTOCUBE_VERSION "${RSGISLIB_HISTOCUBE_VERSION_MAJOR}.${RSGISLIB_HISTOCUBE_VERSION_MINOR}.${RSGISLIB_HISTOCUBE_VERSION_PATCH}")

# Name of C++ library
set(RSGISLIB_CALIBRATION_LIB_NAME rsgis_calib)
set(RSGISLIB_CLASSIFY_LIB_NAME rsgis_classify)
set(RSGISLIB_COMMONS_LIB_NAME rsgis_commons)
set(RSGISLIB_DATASTRUCT_LIB_NAME rsgis_datastruct)
set(RSGISLIB_FILTERING_LIB_NAME rsgis_filter)
set(RSGISLIB_GEOM_LIB_NAME rsgis_geom)
set(RSGISLIB_IMG_LIB_NAME rsgis_img)
set(RSGISLIB_MATHS_LIB_NAME rsgis_maths)
set(RSGISLIB_MODELING_LIB_NAME rsgis_modeling)
set(RSGISLIB_RADAR_LIB_NAME rsgis_radar)
set(RSGISLIB_RASTERGIS_LIB_NAME rsgis_rastergis)
set(RSGISLIB_REGISTRATION_LIB_NAME rsgis_registration)
set(RSGISLIB_SEGMENTATION_LIB_NAME rsgis_segmentation)
set(RSGISLIB_UTILS_LIB_NAME rsgis_utils)
set(RSGISLIB_VECTOR_LIB_NAME rsgis_vec)
set(RSGISLIB_HISTOCUBE_LIB_NAME rsgis_histocube)
set(RSGISLIB_CMDSINTERFACE_LIB_NAME rsgis_cmds)

option (BUILD_SHARED_LIBS "Build with shared library" ON)
set(RSGISLIB_WITH_UTILTIES TRUE CACHE BOOL "Choose if RSGISLib utilities should be built")
set(RSGISLIB_WITH_DOCUMENTS TRUE CACHE BOOL "Choose if RSGISLib documentation should be installed.")

set(BOOST_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for Boost")
set(BOOST_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for Boost")

set(GSL_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for GSL")
set(GSL_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for GSL")

set(GDAL_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for GDAL")
set(GDAL_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for GDAL")

set(GEOS_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for GEOS")
set(GEOS_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for GEOS")

set(XERCESC_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for Xerces-c")
set(XERCESC_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for Xerces-c")

set(MUPARSER_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for MU Parser")
set(MUPARSER_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for MU Parser")

set(CGAL_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for CGAL")
set(CGAL_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for CGAL")

set(HDF5_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for HDF5")
set(HDF5_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for HDF5")

set(GMP_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for GMP")
set(GMP_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for GMP")

set(MPFR_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for MPFR")
set(MPFR_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for MPFR")

set(KEA_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for KEA")
set(KEA_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for KEA")

option(RSGIS_PYTHON "Build Python bindings" ON)

###############################################################################

###############################################################################
# CMake settings
cmake_minimum_required(VERSION 3.12)

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
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()

  elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER}" MATCHES "clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RSGISLIB_COMMON_CXX_FLAGS} -std=c++11")
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
    file(GLOB BOOST_FS_LIB ${BOOST_LIB_PATH}/libboost_filesystem.lib )
    if(NOT BOOST_FS_LIB)
        message(FATAL_ERROR "Could not find boost_filesystem lib")
    endif()
    file(GLOB BOOST_SYS_LIB ${BOOST_LIB_PATH}/libboost_system.lib )
    if(NOT BOOST_SYS_LIB)
        message(FATAL_ERROR "Could not find boost_system lib")
    endif()
    file(GLOB BOOST_DT_LIB ${BOOST_LIB_PATH}/libboost_date_time.lib )
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
  set(GDAL_LIBRARIES -LIBPATH:${GDAL_LIB_PATH} gdal_i.lib)
else()
  set(GDAL_LIBRARIES -L${GDAL_LIB_PATH} -lgdal)
endif(MSVC)

include_directories(${GEOS_INCLUDE_DIR})
# Needed with newer geos versions to get complete class definitions
add_definitions(-DGEOS_INLINE)
if (MSVC)
    set(GEOS_LIBRARIES -LIBPATH:${GEOS_LIB_PATH} geos.lib)
else()
    set(GEOS_LIBRARIES -L${GEOS_LIB_PATH} -lgeos)
endif(MSVC)

include_directories(${GSL_INCLUDE_DIR})
if (MSVC)
    set(GSL_LIBRARIES -LIBPATH:${GSL_LIB_PATH} gsl.lib gslcblas.lib)
else()
    set(GSL_LIBRARIES -L${GSL_LIB_PATH} -lgsl -lgslcblas)
endif(MSVC)

include_directories(${XERCESC_INCLUDE_DIR})
if (MSVC)
    set(XERCESC_LIBRARIES -LIBPATH:${XERCESC_LIB_PATH} xerces-c_3.lib)
else()
    set(XERCESC_LIBRARIES -L${XERCESC_LIB_PATH} -lxerces-c)
endif(MSVC)

include_directories(${MUPARSER_INCLUDE_DIR})
if (MSVC)
    set(MUPARSER_LIBRARIES -LIBPATH:${MUPARSER_LIB_PATH} muparser.lib)
else()
    set(MUPARSER_LIBRARIES -L${MUPARSER_LIB_PATH} -lmuparser)
endif(MSVC)

include_directories(${CGAL_INCLUDE_DIR})
if (MSVC)
    file(GLOB CGAL_LIB_NAME ${BOOST_LIB_PATH}/CGAL-${MSVC_STRING}-*.lib )
    if(NOT CGAL_LIB_NAME)
        message(FATAL_ERROR "Could not find CGAL lib")
    endif()
    message(STATUS "Using CGAL lib " ${CGAL_LIB_NAME})
    set(CGAL_LIBRARIES -LIBPATH:${CGAL_LIB_PATH} ${CGAL_LIB_NAME})
else()
    set(CGAL_LIBRARIES -L${CGAL_LIB_PATH} -lCGAL)
endif(MSVC)

include_directories(${HDF5_INCLUDE_DIR})
if (MSVC)
    set(HDF5_LIBRARIES -LIBPATH:${HDF5_LIB_PATH} hdf5.lib hdf5_hl.lib hdf5_cpp.lib)
else()
    set(HDF5_LIBRARIES -L${HDF5_LIB_PATH} -lhdf5 -lhdf5_hl -lhdf5_cpp)
endif(MSVC)

include_directories(${GMP_INCLUDE_DIR})
if (MSVC)
    # Conda CGAL uses MPIR rather than GMP (check other platforms)
    set(GMP_LIBRARIES -LIBPATH:${GMP_LIB_PATH} mpir.lib)
else()
    set(GMP_LIBRARIES -L${GMP_LIB_PATH} -lgmp)
endif(MSVC)

include_directories(${MPFR_INCLUDE_DIR})
if (MSVC)
    set(MPFR_LIBRARIES -LIBPATH:${MPFR_LIB_PATH} mpfr.lib)
else()
    set(MPFR_LIBRARIES -L${MPFR_LIB_PATH} -lmpfr)
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
	add_executable(flip ${PROJECT_TOOLS_DIR}/flip.cpp)
	target_link_libraries (flip )
	add_executable(gdalsimpleinfo ${PROJECT_TOOLS_DIR}/gdalsimpleinfo.cpp)
	target_link_libraries (gdalsimpleinfo ${GDAL_LIBRARIES} )
    if (MSVC)
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgis-config.bat.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgis-config.bat" )
    else()
        configure_file ( "${PROJECT_TOOLS_DIR}/rsgis-config.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgis-config" )
    endif(MSVC)
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisENVIPlot2MTXT.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisENVIPlot2MTXT.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisplot.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisplot.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisplotrastergis.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisplotrastergis.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgissegmentation.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgissegmentation.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisattributerat.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisattributerat.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgiszonalstats.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiszonalstats.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgismosaic.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgismosaic.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscalcindices.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscalcindices.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscalcaccmatrix.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscalcaccmatrix.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscalcimgstats.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscalcimgstats.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscopybandnames.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscopybandnames.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisimg2kmz.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimg2kmz.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisimg2webtiles.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimg2webtiles.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisvectools.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisvectools.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisimginfo.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimginfo.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisbuildimglut.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbuildimglut.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisfilelut.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisfilelut.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgisproj.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisproj.py" )
endif(RSGISLIB_WITH_UTILTIES)

if (RSGISLIB_WITH_DOCUMENTS)
	configure_file ( "${PROJECT_DOC_DIR}/Doxyfile.in" "${PROJECT_DOC_DIR}/Doxyfile" )
	configure_file ( "${PROJECT_DOC_DIR}/dox_files/index.dox.in" "${PROJECT_DOC_DIR}/dox_files/index.dox" )
endif(RSGISLIB_WITH_DOCUMENTS)

###############################################################################

###############################################################################
# Installation
if (RSGISLIB_WITH_UTILTIES)
	install (TARGETS flip DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (TARGETS gdalsimpleinfo DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    if (MSVC)
        install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgis-config.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    else()
        install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgis-config" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    endif(MSVC)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisENVIPlot2MTXT.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisplot.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisplotrastergis.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgissegmentation.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisattributerat.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiszonalstats.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgismosaic.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscalcindices.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscalcaccmatrix.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscalcimgstats.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgiscopybandnames.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimg2kmz.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimg2webtiles.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisvectools.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisimginfo.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisbuildimglut.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisfilelut.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/rsgisproj.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif(RSGISLIB_WITH_UTILTIES)


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
