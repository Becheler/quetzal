cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -pedantic-errors -Wextra -DNDEBUG)

### INTEGRATION TESTS

add_executable(model_1 integration_test/model_1/main.cpp)

target_link_libraries(model_1 LINK_PUBLIC
      ${GDAL_LIBRARY}
      Boost::unit_test_framework)

target_include_directories(model_1
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

add_test(SpatialExpansion model_1 ${CMAKE_CURRENT_SOURCE_DIR}/data/europe_temp.tif)

### UNIT TEST

# Keep test files in a separate source directory called test
file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} unit_test/*.cpp)

#Run through each source
foreach(testSrc ${TEST_SRCS})
        #Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${testSrc} NAME_WE)

        #Add compile target
        add_executable(${testName} ${testSrc})

        #link to Boost libraries AND your targets and dependencies
        target_link_libraries(${testName}
                                Boost::unit_test_framework
                                ${GDAL_LIBRARY})

        #I like to move testing binaries into a testBin directory
         set_target_properties(${testName} PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/test/testBin)

        target_include_directories(
          ${testName} PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

        #Finally add it to test execution -
        #Notice the WORKING_DIRECTORY and COMMAND
        add_test(NAME ${testName}
                 WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/testBin
               COMMAND ${CMAKE_BINARY_DIR}/test/testBin/${testName} )
endforeach(testSrc)
