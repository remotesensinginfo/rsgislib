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
set (PROJECT_XML_DIR xml_standards)

# The version number.
set (RSGISLIB_HG_COMMIT 715)
set (RSGISLIB_VERSION_MAJOR 2)
set (RSGISLIB_VERSION_MINOR 1)
set (RSGISLIB_VERSION_PATCH 0)
set (RSGISLIB_VERSION "${RSGISLIB_VERSION_MAJOR}.${RSGISLIB_VERSION_MINOR}.${RSGISLIB_VERSION_PATCH}")
set (RSGISLIB_PACKAGE_VERSION "${RSGISLIB_VERSION_MAJOR}.${RSGISLIB_VERSION_MINOR}.${RSGISLIB_VERSION_PATCH}")
set (RSGISLIB_PACKAGE_STRING "RSGISLib ${RSGISLIB_VERSION_MAJOR}.${RSGISLIB_VERSION_MINOR}.${RSGISLIB_VERSION_PATCH}")
set (RSGISLIB_PACKAGE_BUGREPORT "rsgislib-support@googlegroups.com")
set (RSGISLIB_PACKAGE "RSGISLib")
set (RSGISLIB_COPYRIGHT_YEAR 2013)

set (RSGISLIB_CALIBRATION_VERSION_MAJOR 1)
set (RSGISLIB_CALIBRATION_VERSION_MINOR 0)
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

set (RSGISLIB_RASTERGIS_VERSION_MAJOR 1)
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
set (RSGISLIB_CMDSINTERFACE_VERSION_MINOR 0)
set (RSGISLIB_CMDSINTERFACE_VERSION_PATCH 0)
set (RSGISLIB_CMDSINTERFACE_VERSION "${RSGISLIB_CMDSINTERFACE_VERSION_MAJOR}.${RSGISLIB_CMDSINTERFACE_VERSION_MINOR}.${RSGISLIB_CMDSINTERFACE_VERSION_PATCH}")

if(WIN32)
    # Name of C++ library
    set(RSGISLIB_CALIBRATION_LIB_NAME librsgis_calib)
	set(RSGISLIB_CLASSIFY_LIB_NAME librsgis_classify)
	set(RSGISLIB_COMMONS_LIB_NAME librsgis_commons)
	set(RSGISLIB_DATASTRUCT_LIB_NAME librsgis_datastruct)
	set(RSGISLIB_FILTERING_LIB_NAME librsgis_filter)
	set(RSGISLIB_GEOM_LIB_NAME librsgis_geom)
	set(RSGISLIB_IMG_LIB_NAME librsgis_img)
	set(RSGISLIB_MATHS_LIB_NAME librsgis_maths)
	set(RSGISLIB_MODELING_LIB_NAME librsgis_modeling)
	set(RSGISLIB_RADAR_LIB_NAME librsgis_radar)
	set(RSGISLIB_RASTERGIS_LIB_NAME librsgis_rastergis)
	set(RSGISLIB_REGISTRATION_LIB_NAME librsgis_registration)
	set(RSGISLIB_SEGMENTATION_LIB_NAME librsgis_segmentation)
	set(RSGISLIB_UTILS_LIB_NAME librsgis_utils)
	set(RSGISLIB_VECTOR_LIB_NAME librsgis_vec)
	set(RSGISLIB_CMDSINTERFACE_LIB_NAME librsgis_cmds)
else()
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
	set(RSGISLIB_CMDSINTERFACE_LIB_NAME rsgis_cmds)
endif()

option (BUILD_SHARED_LIBS "Build with shared library" ON)
set(RSGISLIB_WITH_RSGISEXE TRUE CACHE BOOL "Choose if RSGISLib rsgisexe command should be built")
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

set(FFTW_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for FFTW")
set(FFTW_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for FFTW")

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

###############################################################################

###############################################################################
# CMake settings
cmake_minimum_required(VERSION 2.6.0)

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
configure_file ( "${PROJECT_SOURCE_DIR}/rsgis-config.h.in" "${PROJECT_SOURCE_DIR}/common/rsgis-config.h" )
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
        if (MSVC80 OR MSVC90 OR MSVC10)
            add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            add_definitions(-D_CRT_NONSTDC_NO_WARNING)
            add_definitions(-D_SCL_SECURE_NO_WARNINGS)
        endif()
        
        if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
            string(REGEX REPLACE "/W[0-4]" "/W4"
                   CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
        endif()

        # Generate dot-user file with user-specific settings for Visual Studio project
        set(MSVC_ENVIRONMENT_PATH "" CACHE STRING
            "Custom PATH for Environment property in Visual Studio project configuration")
        mark_as_advanced(MSVC_ENVIRONMENT_PATH)
        set(VCPROJ_USER_ENVIRONMENT_PATH_DEBUG "${ENVIRONMENT_PATH}")
        mark_as_advanced(VCPROJ_USER_ENVIRONMENT_PATH_DEBUG)

    endif()

	set(CMAKE_INCLUDE_PATH c:/osgeo4w/include;$ENV{CMAKE_INCLUDE_PATH})
    set(CMAKE_LIBRARY_PATH c:/osgeo4w/lib;$ENV{CMAKE_LIBRARY_PATH})
else()
  # Recommended C++ compilation flags
  # -Weffc++
  set(RSGISLIB_COMMON_CXX_FLAGS
	"-Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wredundant-decls -Wno-long-long")
	#"-pedantic -ansi -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wfloat-equal -Wredundant-decls -Wno-long-long")

  if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ${RSGISLIB_COMMON_CXX_FLAGS}")
    if (CMAKE_COMPILER_IS_GNUCXX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98")
    endif()

  elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER}" MATCHES "clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RSGISLIB_COMMON_CXX_FLAGS}")
  endif()
endif(WIN32)

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
set(BOOST_LIBRARIES -L${BOOST_LIB_PATH} -lboost_filesystem -lboost_system)

FIND_PROGRAM(PYTHON_EXE python ENV PATH)
IF(PYTHON_EXE STREQUAL "PYTHON_EXE-NOTFOUND")
  MESSAGE(STATUS "Python not found. Python bindings disabled")
  SET(BUILD_PYTHON_BINDINGS OFF)
ELSEIF(NOT PYTHON_EXE STREQUAL "PYTHON_EXE-NOTFOUND")
  MESSAGE(STATUS "Building Python bindings with ${PYTHON_EXE}")
  SET(BUILD_PYTHON_BINDINGS ON)
ENDIF()

include_directories(${GDAL_INCLUDE_DIR})
set(GDAL_LIBRARIES -L${GDAL_LIB_PATH} -lgdal)

include_directories(${GEOS_INCLUDE_DIR})
set(GEOS_LIBRARIES -L${GEOS_LIB_PATH} -lgeos)

include_directories(${GSL_INCLUDE_DIR})
set(GSL_LIBRARIES -L${GSL_LIB_PATH} -lgsl -lgslcblas)

include_directories(${XERCESC_INCLUDE_DIR})
set(XERCESC_LIBRARIES -L${XERCESC_LIB_PATH} -lxerces-c)

include_directories(${FFTW_INCLUDE_DIR})
set(FFTW_LIBRARIES -L${FFTW_LIB_PATH} -lfftw3)

include_directories(${MUPARSER_INCLUDE_DIR})
set(MUPARSER_LIBRARIES -L${MUPARSER_LIB_PATH} -lmuparser)

include_directories(${CGAL_INCLUDE_DIR})
set(CGAL_LIBRARY -L${CGAL_LIB_PATH} -lCGAL -lgmp)

include_directories(${HDF5_INCLUDE_DIR})
set(HDF5_LIBRARIES -L${HDF5_LIB_PATH} -lhdf5 -lhdf5_hl -lhdf5_cpp)

include_directories(${GMP_INCLUDE_DIR})
set(GMP_LIBRARIES -L${GMP_LIB_PATH} -lgmp)

include_directories(${MPFR_INCLUDE_DIR})
set(MPFR_LIBRARIES -L${MPFR_LIB_PATH} -lmpfr)

include_directories(${KEA_INCLUDE_DIR})
set(KEA_LIBRARIES -L${KEA_LIB_PATH} -lkea)

###############################################################################

###############################################################################
# Build libraries
include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory ("${PROJECT_SOURCE_DIR}")
###############################################################################

###############################################################################
# Build executables
if (RSGISLIB_WITH_UTILTIES)
	add_executable(flip ${PROJECT_TOOLS_DIR}/flip.cpp)
	target_link_libraries (flip )
	add_executable(gdalsimpleinfo ${PROJECT_TOOLS_DIR}/gdalsimpleinfo.cpp)
	target_link_libraries (gdalsimpleinfo ${GDAL_LIBRARIES} )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgis-config.in" "${PROJECT_BINARY_DIR}/rsgis-config" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgislibENVIPlot2MTXT.py.in" "${PROJECT_BINARY_DIR}/rsgislibENVIPlot2MTXT.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgislibplot.py.in" "${PROJECT_BINARY_DIR}/rsgislibplot.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgislibxml.py.in" "${PROJECT_BINARY_DIR}/rsgislibxml.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgiscmd.py.in" "${PROJECT_BINARY_DIR}/rsgiscmd" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgislibvarsxml.py.in" "${PROJECT_BINARY_DIR}/rsgislibvarsxml.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgislibvarsxmltable.py.in" "${PROJECT_BINARY_DIR}/rsgislibvarsxmltable.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgislibcmdxml.py.in" "${PROJECT_BINARY_DIR}/rsgislibcmdxml.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgislibplotrastergis.py.in" "${PROJECT_BINARY_DIR}/rsgislibplotrastergis.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgislibsegmentation.py.in" "${PROJECT_BINARY_DIR}/rsgislibsegmentation.py" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgislibattributerat.py.in" "${PROJECT_BINARY_DIR}/rsgislibattributerat.py" )
endif(RSGISLIB_WITH_UTILTIES)

if (RSGISLIB_WITH_DOCUMENTS)
	configure_file ( "${PROJECT_XML_DIR}/input.xml.in" "${PROJECT_XML_DIR}/RSGISLib_XML_Listing.xml" )
	configure_file ( "${PROJECT_TOOLS_DIR}/rsgislibxmllist.sh.in" "${PROJECT_BINARY_DIR}/rsgislibxmllist" )
endif(RSGISLIB_WITH_DOCUMENTS)

# Python 
if (BUILD_PYTHON_BINDINGS)
  add_custom_target("Python_build" ALL COMMAND ${PYTHON_EXE} "setup.py" "build" DEPENDS ${RSGISLIB_CMDSINTERFACE_LIB_NAME} WORKING_DIRECTORY "python")
endif(BUILD_PYTHON_BINDINGS)

###############################################################################

###############################################################################
# Installation
if (RSGISLIB_WITH_UTILTIES)
	install (TARGETS flip DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (TARGETS gdalsimpleinfo DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${PROJECT_BINARY_DIR}/rsgis-config" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${PROJECT_BINARY_DIR}/rsgislibENVIPlot2MTXT.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${PROJECT_BINARY_DIR}/rsgislibplot.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${PROJECT_BINARY_DIR}/rsgislibxml.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${PROJECT_BINARY_DIR}/rsgiscmd" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${PROJECT_BINARY_DIR}/rsgislibcmdxml.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    install (FILES "${PROJECT_BINARY_DIR}/rsgislibvarsxml.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    install (FILES "${PROJECT_BINARY_DIR}/rsgislibvarsxmltable.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${PROJECT_BINARY_DIR}/rsgislibplotrastergis.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${PROJECT_BINARY_DIR}/rsgislibsegmentation.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${PROJECT_BINARY_DIR}/rsgislibattributerat.py" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif(RSGISLIB_WITH_UTILTIES)

if (RSGISLIB_WITH_DOCUMENTS)
	install (FILES "${PROJECT_XML_DIR}/RSGISLib_XML_Listing.xml" DESTINATION doc PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES "${PROJECT_BINARY_DIR}/rsgislibxmllist" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif(RSGISLIB_WITH_DOCUMENTS)

# Python 
if (BUILD_PYTHON_BINDINGS)
  install(CODE "execute_process(COMMAND ${PYTHON_EXE} \"setup.py\" \"install\" \"--prefix=${CMAKE_INSTALL_PREFIX}\"  WORKING_DIRECTORY \"python\")")
endif(BUILD_PYTHON_BINDINGS)

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
