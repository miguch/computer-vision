cmake_minimum_required(VERSION 3.10)
project(number-detection)

set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV REQUIRED )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (APPLE)
    add_executable(number-detection-mac src/main.cpp src/adaboost.cpp src/adaboost.h src/mnistData.cpp src/mnistData.h src/testUtils.cpp src/testUtils.h)
    target_link_libraries( number-detection-mac ${OpenCV_LIBS} )
endif(APPLE)

if(UNIX AND NOT APPLE)
    add_executable(number-detection-linux src/main.cpp src/adaboost.cpp src/adaboost.h src/mnistData.cpp src/mnistData.h src/testUtils.cpp src/testUtils.h)
    target_link_libraries( number-detection-linux ${OpenCV_LIBS} )
endif(UNIX AND NOT APPLE)

if (WIN32)
    add_executable(number-detection src/main.cpp src/adaboost.cpp src/adaboost.h src/mnistData.cpp src/mnistData.h src/testUtils.cpp src/testUtils.h)
    target_link_libraries( number-detection ${OpenCV_LIBS} )
endif(WIN32)