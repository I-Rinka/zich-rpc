cmake_minimum_required(VERSION 2.6)

project(helloboost C CXX)

# SET(CMAKE_CXX_FLAGS "-g -w -O2")
SET(CMAKE_CXX_FLAGS "-g")

#default binary and lib path
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
#begin to set boost static library
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost_1_63_0/include)
set(Boost_USE_STATIC_LIBS ON)
set(BOOST_COMPONENTS date_time chrono filesystem iostreams program_options regex system thread unit_test_framework)
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

# ADD_EXECUTABLE(helloboost main.cpp)


file(GLOB_RECURSE mains RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/main*.cpp")
foreach(mainfile IN LISTS mains)
    # Get file name without directory
    get_filename_component(mainname ${mainfile} NAME_WE)
    add_executable(${mainname} ${mainfile})
    TARGET_LINK_LIBRARIES(${mainname} ${Boost_LIBRARIES})
endforeach()