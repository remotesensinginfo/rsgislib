# Install script for directory: /Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_commons.1.3.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_commons.1.3.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_commons.1.3.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_commons.1.3.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_commons.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_commons.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_commons.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_commons.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_datastruct.1.1.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_datastruct.1.1.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_datastruct.1.1.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_datastruct.1.1.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_datastruct.1.1.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_datastruct.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_datastruct.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_datastruct.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_datastruct.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_datastruct.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_maths.1.1.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_maths.1.1.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_maths.1.1.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_maths.1.1.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_maths.1.1.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_maths.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_maths.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_maths.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_maths.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_maths.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_utils.1.1.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_utils.1.1.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_utils.1.1.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_utils.1.1.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_utils.1.1.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_utils.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_utils.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_utils.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_utils.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_utils.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_img.3.0.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_img.3.0.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_img.3.0.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_img.3.0.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_img.3.0.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_img.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_img.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_img.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_img.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_img.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_registration.1.2.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_registration.1.2.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_registration.1.2.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_registration.1.2.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_registration.1.2.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_registration.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_registration.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_registration.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_registration.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_registration.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_calib.1.1.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_calib.1.1.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_calib.1.1.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_calib.1.1.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_calib.1.1.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_calib.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_calib.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_calib.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_calib.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_calib.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_filter.1.0.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_filter.1.0.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_filter.1.0.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_filter.1.0.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_filter.1.0.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_filter.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_filter.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_filter.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_filter.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_filter.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_rastergis.2.0.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_rastergis.2.0.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_rastergis.2.0.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_rastergis.2.0.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_rastergis.2.0.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_rastergis.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_rastergis.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_rastergis.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_rastergis.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_rastergis.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_segmentation.1.0.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_segmentation.1.0.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_segmentation.1.0.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_segmentation.1.0.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_segmentation.1.0.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_segmentation.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_segmentation.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_segmentation.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_segmentation.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_segmentation.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_vec.2.2.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_vec.2.2.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_vec.2.2.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_vec.2.2.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_vec.2.2.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_vec.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_vec.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_vec.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_vec.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_vec.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_classify.1.1.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_classify.1.1.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_classify.1.1.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_classify.1.1.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_classify.1.1.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_classify.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_classify.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_classify.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_classify.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_classify.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_cmds.1.1.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_cmds.1.1.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_cmds.1.1.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_cmds.1.1.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_cmds.1.1.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/librsgis_cmds.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_cmds.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_cmds.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_cmds.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librsgis_cmds.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/calibration" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/calibration/RSGISApply6SCoefficients.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/calibration/RSGISCalculateTopOfAtmosphereReflectance.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/calibration/RSGISDEMTools.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/calibration/RSGISStandardDN2RadianceCalibration.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/calibration/RSGISApplySubtractOffsets.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/calibration/RSGISCloudMasking.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/calibration/RSGISHydroDEMFillSoilleGratin94.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/calibration/RSGISImgCalibUtils.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/classifier" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/classifier/RSGISClassifier.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/classifier/RSGISClassificationUtils.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/classifier/RSGISMinimumDistanceClassifier.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/classifier/RSGISNearestNeighbourClassifier.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/classifier/RSGISSpectralAngleMapper.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/classifier/RSGISSpectralCorrelationMapper.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/classifier/RSGISCumulativeAreaClassifier.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/classifier/RSGISKMeanImageClassifier.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/classifier/RSGISISODATAImageClassifier.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/classifier/RSGISRATClassificationUtils.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/classifier/RSGISGenAccuracyPoints.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/common" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/common/RSGISCommons.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/common/RSGISException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/common/RSGISFileException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/common/RSGISImageException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/common/RSGISClassificationException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/common/RSGISOutputStreamException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/common/RSGISInputStreamException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/common/RSGISVectorException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/common/RSGISRegistrationException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/common/RSGISAttributeTableException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/common/rsgis-tqdm.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/local_build/src/common/rsgis-config.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/datastruct" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/datastruct/RSGISListException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/datastruct/RSGISItemNotFoundException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/datastruct/SortedGenericList.cpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/filtering" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISCalcFilter.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISGenerateFilter.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISCalcImageFilters.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISImageFilter.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISFilterBank.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISImageKernelFilter.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISStatsFilters.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISPrewittFilter.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISSobelFilter.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISMorphologyDilate.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISMorphologyErode.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISMorphologyGradient.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISMorphologyFindExtrema.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISMorphologyClosing.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISMorphologyOpening.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISMorphologyTopHat.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISSpeckleFilters.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISSARTextureFilters.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/filtering/RSGISNonLocalDenoising.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/img" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageBandException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageCalcException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCalcImageValue.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCalcImageSingleValue.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageUtils.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCalcImage.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCalcImageSingle.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISAddBands.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISAddNoise.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISApplyEigenvectors.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISBandMath.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCalcCorrelationCoefficient.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCalcCovariance.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageMosaic.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageStatistics.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageNormalisation.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCalcImageMatrix.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISMaskImage.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISMeanVector.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISPolygonPixelCount.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISPopulateMatrix.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCopyImage.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCopyImageBands.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISStandardiseImage.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageCalcValueBaysianNoPrior.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageCalcValueBaysianPrior.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageBand2Matrix.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCreatePixelPolygons.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISSavitzkyGolaySmoothingFilters.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISConvertSpectralToUnitArea.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISPixelInPoly.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageMaths.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCumulativeArea.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISStretchImage.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCountValsAboveThresInCol.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCalcRMSE.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISExtractImagePixelsInPolygon.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISPanSharpen.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISLinearSpectralUnmixing.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageClustering.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageComposite.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISRelabelPixelValuesFromLUT.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISPopWithStats.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISGenHistogram.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISSampleImage.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImageWindowStats.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISFitFunction2Pxls.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCalcImgValProb.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISApplyGainOffset2Img.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISSharpenLowResImagery.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISImgSummaryStatsFromMultiResImgs.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/img/RSGISCalcImageLocalMin.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/math" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISMathException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISMatricesException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISMultivariantStatsException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISBaysianStatsException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISVectorsException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISClustererException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISOptimisationException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISBaysianDeltaType.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISMathsUtils.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISMathFunction.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISIntergration.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISMatrices.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISVectors.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISMultivariantStats.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISPrincipalComponentAnalysis.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISBaysianIntergrateFunctionNoPrior.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISBaysianIntergrateFunctionPrior.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISBaysianStatsNoPrior.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISBaysianStatsPrior.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISProbabilityDistributions.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISSingularValueDecomposition.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISPolyFit.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISProbDistro.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISRandomDistro.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISFunctions.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISLinearFit.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISNonLinearFit.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISClustering.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISMaximumLikelihood.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISMaximumLikelihoodException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISLogicExpEvaluation.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISDistMetrics.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/math/RSGISFitGaussianMixModel.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/rastergis" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISRasterAttUtils.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISExportColumns2Image.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISPopRATWithStats.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISCalcImageStatsAndPyramids.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISCalcClusterLocation.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISDefineClumpsInTiles.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISFindClumpNeighbours.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISFindClumpCatagoryStats.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISSelectClumps.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISRATCalcValue.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISRATCalc.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISFindChangeClumps.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISFindInfoBetweenLayers.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISClumpBorders.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISCalcNeighbourStats.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISCollapseRAT.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISInputShapefileAttributes2RAT.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISRATKNN.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISRATFunctionFitting.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/rastergis/RSGISRATStats.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/registration" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/registration/RSGISImageSimilarityMetric.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/registration/RSGISImageRegistration.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/registration/RSGISBasicImageRegistration.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/registration/RSGISStandardImageSimilarityMetrics.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/registration/RSGISSingleConnectLayerImageRegistration.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/registration/RSGISGCPImg2MapNode.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/registration/RSGISImagePixelRegistration.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/registration/RSGISAddGCPsGDAL.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/registration/RSGISFindImageOffset.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/segmentation" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISGenMeanSegImage.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISEliminateSmallClumps.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISClumpPxls.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISRandomColourClumps.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISMergeSmallClumps.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISLabelPixelsUsingClusters.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISLabelPixelsUsingPixels.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISDefineSpectralDivision.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISEliminateSinglePixels.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISMergeSegmentationTiles.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISMergeSegmentations.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISMergeSegments.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISCreateImageGrid.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/segmentation/RSGISDropClumps.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/utils" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/utils/RSGISTextException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/utils/RSGISFileUtils.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/utils/RSGISColour.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/utils/RSGISTextUtils.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/utils/RSGISAllometricSpecies.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/utils/RSGISAllometricEquations.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/utils/RSGISExportData2HDF.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/utils/RSGISGeometryUtils.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/vec" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISVectorZonalException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISVectorOutputException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISProcessOGRFeature.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISProcessVector.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISVectorUtils.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISVectorIO.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISZonalStats2Matrix.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISProcessOGRGeometry.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISProcessGeometry.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISCopyCheckPolygons.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISProcessVectorSQL.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISVectorMaths.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/vec/RSGISZonalImage2HDF.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rsgis/cmds" TYPE FILE FILES
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdException.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdParent.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdCommon.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdClassification.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdFilterImages.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdImageCalc.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdImageCalibration.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdImageMorphology.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdImageRegistration.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdImageUtils.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdSegmentation.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdRasterGIS.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdVectorUtils.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdZonalStats.h"
    "/Users/nmthoma1/OneDrive - NASA/Developer/rsgislib/src/cmds/RSGISCmdElevationTools.h"
    )
endif()

