cmake_minimum_required(VERSION 3.17)
project(Doulos_Example5)

set(CMAKE_CXX_STANDARD 14)
# change your SystemC installation directory
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # if under linux OS
    set(CMAKE_PREFIX_PATH /home/singularity/systemc-2.3.3-install)
    message(STATUS "Current development environment is Linux,")
else(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Current development environment is Windows,")
    set(CMAKE_PREFIX_PATH E:/systemc-2.3.3/SystemC)
endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
message(STATUS "The SystemC installation directory is ${CMAKE_PREFIX_PATH}")

include_directories(${CMAKE_PREFIX_PATH}/include)
find_package(SystemCLanguage CONFIG REQUIRED)
link_directories(${CMAKE_PREFIX_PATH}/lib)

add_executable(Doulos_Example5 testbench.cpp top.h initiator1.h utilities.h initiator2.h bus.h target.h)

target_link_libraries (Doulos_Example5 SystemC::systemc)
