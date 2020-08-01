cmake_minimum_required(VERSION 3.12)

include(ExternalProject)
  ExternalProject_Add(gdal_external
      GIT_REPOSITORY         https://github.com/nextgis-borsch/borsch.git
      GIT_TAG                master
      CONFIGURE_COMMAND     ""
      BUILD_COMMAND         ""
      INSTALL_COMMAND       ""
      DOWNLOAD_NO_PROGRESS  1
      LOG_CONFIGURE         1
      LOG_BUILD             1
      LOG_INSTALL           1
    )


list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/subprojects/Source/gdal_external/cmake)
# set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH})
message("cmake module path:"  ${CMAKE_MODULE_PATH})
