cmake_minimum_required(VERSION 3.0.0)
project(devInfo_process)

make_directory(${CMAKE_BINARY_DIR}/lib)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(devInfo_process_NAME ${PROJECT_NAME} CACHE STRING "devInfo_process" FORCE)

aux_source_directory(. SELF)

add_library(${devInfo_process_NAME} STATIC ${SELF})
