cmake_minimum_required(VERSION 3.10)
project(A4Detect)

set(CMAKE_CXX_STANDARD 11)

if (APPLE)
    set (CMAKE_CXX_FLAGS "-lX11 -L/usr/X11/lib -I/usr/X11/include")
    add_executable(a4detect-mac
            canny.cpp
            canny.h
            CImg.h main.cpp A4Detect.cpp A4Detect.h)
endif (APPLE)

if(UNIX AND NOT APPLE)
    add_executable(a4detect-linux
            canny.cpp
            canny.h
            CImg.h main.cpp A4Detect.cpp A4Detect.h)
    set (CMAKE_CXX_FLAGS "-lX11 -pthread")
    message(STATUS "Looking for X11...")
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
    target_link_libraries(a4detect-linux ${X11_LIBRARIES})
endif(UNIX AND NOT APPLE)

if (WIN32)
    set (CMAKE_CXX_FLAGS "-lgdi32")
    set (CMAKE_SHARED_LINKER_FLAGS "-lgdi32")
    add_executable(a4detect
            canny.cpp
            canny.h
            CImg.h main.cpp A4Detect.cpp A4Detect.h)
endif(WIN32)

include_directories(.)

