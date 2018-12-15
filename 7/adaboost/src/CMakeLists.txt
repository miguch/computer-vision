cmake_minimum_required(VERSION 3.10)
project(number-detection)

set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV REQUIRED )

if (APPLE)
    add_executable(number-detection-mac main.cpp)
    target_link_libraries( number-detection-mac ${OpenCV_LIBS} )
endif(APPLE)

if(UNIX AND NOT APPLE)
    add_executable(number-detection-linux main.cpp)
    target_link_libraries( number-detection-linux ${OpenCV_LIBS} )
endif(UNIX AND NOT APPLE)

if (WIN32)
    add_executable(number-detection main.cpp)
    target_link_libraries( number-detection ${OpenCV_LIBS} )
endif(WIN32)