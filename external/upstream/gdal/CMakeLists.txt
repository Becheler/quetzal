cmake_minimum_required(VERSION 3.12)

find_package(GDAL QUIET)

if(GDAL_FOUND)
  get_property(_loc TARGET GDAL::GDAL PROPERTY LOCATION)
  message(STATUS "Found GDAL: ${_loc} (version ${GDAL_VERSION})")
  add_library(gdal_external INTERFACE)  # dummy
else()
  message(STATUS "GDAL could not be located, downloading and building version 3.1 instead.")

  include(ExternalProject)
  ExternalProject_Add(gdal_external
      URL                   https://github.com/nextgis-borsch/lib_gdal/archive/v2.4.0.tar.gz
      UPDATE_COMMAND        ""
      CONFIGURE_COMMAND     ""
      BUILD_COMMAND         ""
      INSTALL_COMMAND       ""
      DOWNLOAD_NO_PROGRESS  1
      LOG_CONFIGURE         1
      LOG_BUILD             1
      LOG_INSTALL           1
      CMAKE_ARGS            -DCMAKE_INSTALL_PREFIX=${STAGED_INSTALL_PREFIX}
    )

    set(
      GDAL_ROOT ${STAGED_INSTALL_PREFIX}/gdal
      CACHE PATH "Path to internally built gdal installation root"
      FORCE
      )
    set(
      GDAL_INCLUDE_DIRS ${STAGED_INSTALL_PREFIX}/gdal/include
      CACHE PATH "Path to internally built gdal include directories"
      FORCE
      )
    set(
      GDAL_LIBRARY ${STAGED_INSTALL_PREFIX}/gdal/lib
      CACHE PATH "Path to internally built gdal library directories"
      FORCE
      )

endif()
