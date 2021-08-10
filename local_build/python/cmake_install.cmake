# Install script for directory: /Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/nmthoma1/Documents/tmp/rsgislib_builds")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RELEASE")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages" TYPE DIRECTORY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/rsgislib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalc/_imagecalc.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalc" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/python/_imagecalc.so")
  if(EXISTS "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalc/_imagecalc.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalc/_imagecalc.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/../src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalc/_imagecalc.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalc/_imagecalc.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalc/_imagecalc.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageutils/_imageutils.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageutils" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/python/_imageutils.so")
  if(EXISTS "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageutils/_imageutils.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageutils/_imageutils.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/../src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageutils/_imageutils.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageutils/_imageutils.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageutils/_imageutils.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/segmentation/_segmentation.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/segmentation" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/python/_segmentation.so")
  if(EXISTS "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/segmentation/_segmentation.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/segmentation/_segmentation.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/../src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/segmentation/_segmentation.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/segmentation/_segmentation.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/segmentation/_segmentation.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalibration/_imagecalibration.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalibration" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/python/_imagecalibration.so")
  if(EXISTS "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalibration/_imagecalibration.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalibration/_imagecalibration.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/../src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalibration/_imagecalibration.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalibration/_imagecalibration.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagecalibration/_imagecalibration.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/rastergis/_rastergis.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/rastergis" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/python/_rastergis.so")
  if(EXISTS "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/rastergis/_rastergis.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/rastergis/_rastergis.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/../src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/rastergis/_rastergis.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/rastergis/_rastergis.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/rastergis/_rastergis.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/zonalstats/_zonalstats.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/zonalstats" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/python/_zonalstats.so")
  if(EXISTS "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/zonalstats/_zonalstats.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/zonalstats/_zonalstats.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/../src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/zonalstats/_zonalstats.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/zonalstats/_zonalstats.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/zonalstats/_zonalstats.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageregistration/_imageregistration.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageregistration" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/python/_imageregistration.so")
  if(EXISTS "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageregistration/_imageregistration.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageregistration/_imageregistration.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/../src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageregistration/_imageregistration.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageregistration/_imageregistration.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imageregistration/_imageregistration.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/vectorutils/_vectorutils.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/vectorutils" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/python/_vectorutils.so")
  if(EXISTS "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/vectorutils/_vectorutils.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/vectorutils/_vectorutils.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/../src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/vectorutils/_vectorutils.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/vectorutils/_vectorutils.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/vectorutils/_vectorutils.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagefilter/_imagefilter.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagefilter" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/python/_imagefilter.so")
  if(EXISTS "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagefilter/_imagefilter.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagefilter/_imagefilter.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/../src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagefilter/_imagefilter.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagefilter/_imagefilter.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagefilter/_imagefilter.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/classification/_classification.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/classification" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/python/_classification.so")
  if(EXISTS "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/classification/_classification.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/classification/_classification.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/../src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/classification/_classification.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/classification/_classification.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/classification/_classification.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/elevation/_elevation.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/elevation" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/python/_elevation.so")
  if(EXISTS "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/elevation/_elevation.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/elevation/_elevation.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/../src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/elevation/_elevation.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/elevation/_elevation.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/elevation/_elevation.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagemorphology/_imagemorphology.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagemorphology" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/python/_imagemorphology.so")
  if(EXISTS "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagemorphology/_imagemorphology.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagemorphology/_imagemorphology.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/python/../src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagemorphology/_imagemorphology.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagemorphology/_imagemorphology.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib/imagemorphology/_imagemorphology.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND /Users/nmthoma1/miniconda/envs/RSGISLib_builds/bin/python3.9 -m compileall /Users/nmthoma1/Documents/tmp/rsgislib_builds/lib/python3.9/site-packages/rsgislib)
endif()

