cmake_minimum_required(VERSION 3.10)
project(recognition)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

FILE(GLOB A4_SOURCE src/a4/*.cpp )
FILE(GLOB A4_HEADER src/a4/*.h )

FILE(GLOB ROOT_SOURCE src/*.cpp )
FILE(GLOB ROOT_HEADER src/*.h)

FILE(GLOB SEGMENTATION_SOURCE src/segmentation/*.cpp )
FILE(GLOB SEGMENTATION_HEADER src/segmentation/*.h)

FILE(GLOB HISTOGRAM_SOURCE src/histogram/*.cpp )
FILE(GLOB HISTOGRAM_HEADER src/histogram/*.h)

if (APPLE)

    include_directories(/usr/X11/include ${Boost_INCLUDE_DIR})
    link_directories(/usr/X11/lib )
    link_libraries(X11)
    FIND_PACKAGE( Boost REQUIRED )
    add_executable(recognition-mac
            ${ROOT_SOURCE} ${ROOT_HEADER} ${A4_SOURCE} ${A4_HEADER}
            ${SEGMENTATION_SOURCE} ${SEGMENTATION_HEADER} ${HISTOGRAM_SOURCE}
            ${HISTOGRAM_HEADER})

endif (APPLE)

if(UNIX AND NOT APPLE)
    add_executable(recognition-linux
            ${ROOT_SOURCE} ${ROOT_HEADER} ${A4_SOURCE} ${A4_HEADER}
            ${SEGMENTATION_SOURCE} ${SEGMENTATION_HEADER} ${HISTOGRAM_SOURCE}
            ${HISTOGRAM_HEADER})
    set (CMAKE_CXX_FLAGS "-lX11 -pthread")
    message(STATUS "Looking for X11...")
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
    target_link_libraries(a4-linux ${X11_LIBRARIES})
endif(UNIX AND NOT APPLE)

if (WIN32)
    set (CMAKE_CXX_FLAGS "-lgdi32")
    set (CMAKE_SHARED_LINKER_FLAGS "-lgdi32")
    add_executable(recognition
            ${ROOT_SOURCE} ${ROOT_HEADER} ${A4_SOURCE} ${A4_HEADER}
            ${SEGMENTATION_SOURCE} ${SEGMENTATION_HEADER} ${HISTOGRAM_SOURCE}
            ${HISTOGRAM_HEADER})
endif(WIN32)

include_directories(.)
