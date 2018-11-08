cmake_minimum_required(VERSION 3.10)
project(5)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (APPLE)
    set (CMAKE_CXX_FLAGS "-lX11 -L/usr/X11/lib -I/usr/X11/include")
    add_executable(morphing-mac
            CImg.h main.cpp ImageMorphing.cpp ImageMorphing.h)
#    add_executable(img2vid-mac
#            CImg.h img2vid.cpp)
endif (APPLE)

if(UNIX AND NOT APPLE)
    add_executable(morphing-linux
            CImg.h main.cpp ImageMorphing.cpp ImageMorphing.h)
#    add_executable(img2vid-linux
#            CImg.h img2vid.cpp)
    set (CMAKE_CXX_FLAGS "-lX11 -pthread")
    message(STATUS "Looking for X11...")
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
    target_link_libraries(morphing-linux ${X11_LIBRARIES})
    #    target_link_libraries(img2vid-linux ${X11_LIBRARIES})
endif(UNIX AND NOT APPLE)

if (WIN32)
    set (CMAKE_CXX_FLAGS "-lgdi32")
    set (CMAKE_SHARED_LINKER_FLAGS "-lgdi32")
    add_executable(morphing
            CImg.h main.cpp ImageMorphing.cpp ImageMorphing.h)
#    add_executable(img2vid
#            CImg.h img2vid.cpp)
endif(WIN32)

include_directories(.)
